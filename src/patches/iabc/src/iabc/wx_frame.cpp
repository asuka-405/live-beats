
#include "iabc/wx_frame.h"
#include "iabc/map.cpp"
#include "iabc/factory.cpp"
#include "iabc/wx_splitter.h"
#include "iabc/event_log.h"
#include "wx/fontdlg.h"
#include <wx/clipbrd.h>
#include "iabc/unicode.h"

#ifdef GCC
#define CONTROL_L ('l')
#define CONTROL_T ('t')
#else
#define CONTROL_L (12)
#define CONTROL_T (20)
#endif

extern wxFrame* theMessageFrame;
extern void UnloadResources();
bool globalUserAbortFlag = false;

namespace iabc
{


registry& GetRegistryInstance()
{

    static registry* the_registry = 0;
    if (the_registry == 0)
        {
        string tmp_path = program_dir;
#ifndef GCC
        tmp_path += "\\iabc.ini";
#else
        tmp_path += "/iabc.ini";
#endif
        the_registry = new registry(tmp_path.access_char_array());
        };

    return *the_registry;
}


void
wx_frame_splitter_notifier::notify_sash_change(double d)
{
    my_frame->change_sash_position(d,my_direction);
}

// main.cpp : Defines the entry point for the application.
//
IMPLEMENT_CLASS(wx_view_frame, wxFrame)
BEGIN_EVENT_TABLE(wx_view_frame, wxFrame)
    EVT_MENU(wxID_HELP, wx_view_frame::OnHelp)
    EVT_MENU(DOCVIEW_ABOUT, wx_view_frame::OnAbout)
    EVT_MENU(ON_DUMP_LOG, wx_view_frame::OnDumpLog)
    EVT_MENU(ON_VIEW_PREFERENCES, wx_view_frame::OnViewPreferences)
    EVT_MENU(ON_ABC_PREFERENCES, wx_view_frame::OnABCPreferences)
    EVT_MENU(ON_CHANGE_EDITOR_FONT, wx_view_frame::OnChangeEditorFont)
    EVT_MENU(ON_SCORE_MODE, wx_view_frame::OnScoreMode)
    EVT_MENU(wxID_OPEN, wx_view_frame::OnOpen)
    EVT_MENU(wxID_NEW, wx_view_frame::OnNew)
    EVT_MENU(wxID_SAVE, wx_view_frame::OnSave)
    EVT_MENU(wxID_SAVEAS, wx_view_frame::OnSaveAs)
    EVT_MENU(wxID_PRINT, wx_view_frame::OnPrint)
    EVT_MENU(wxID_EXIT, wx_view_frame::OnCloseMenu)
    EVT_MENU(ON_REFRESH, wx_view_frame::OnRefresh)
    EVT_MENU(ON_EXPORT, wx_view_frame::OnExport)
    EVT_MENU(ON_PLAY, wx_view_frame::OnPlay)
    EVT_MENU(ON_PLAY_FROM_CURSOR, wx_view_frame::OnPlay)
    EVT_MENU(ON_SET_PLAYER, wx_view_frame::OnSetPlayer)
    EVT_MENU(ON_CUT, wx_view_frame::OnCut)
    EVT_MENU(ON_COPY, wx_view_frame::OnCopy)
    EVT_MENU(ON_PASTE, wx_view_frame::OnPaste)
    EVT_MENU(ON_UNDO, wx_view_frame::OnUndo)
    EVT_MENU(ON_TRANSPOSE_CHROMATIC,wx_view_frame::OnTranspose)
    EVT_MENU(ON_TRANSPOSE_REPEAT,wx_view_frame::OnTranspose)
    EVT_MENU(ON_TRANSPOSE_UP1,wx_view_frame::OnTranspose)
    EVT_MENU(ON_TRANSPOSE_DOWN1,wx_view_frame::OnTranspose)
    EVT_MENU(ON_TRANSPOSE_UP12,wx_view_frame::OnTranspose)
    EVT_MENU(ON_TRANSPOSE_DOWN12,wx_view_frame::OnTranspose)
    EVT_MENU(ON_VOICE,wx_view_frame::OnVoice)
    EVT_MENU(ON_SHOW_TUNE_LIST,wx_view_frame::OnShowTuneList)
    EVT_CLOSE(wx_view_frame::OnCloseWindow)
    EVT_SIZE(wx_view_frame::OnSize)
    EVT_WINDOW_CREATE(wx_view_frame::OnWindowCreate)
    EVT_TIMER(TIMER_ID,wx_view_frame::OnTick)
END_EVENT_TABLE()


class wx_view_frame_key_handler:public wx_key_handler
{
public:
    wx_view_frame* my_handler;
    wx_view_frame_key_handler(wx_view_frame* the_handler){my_handler = the_handler;};
    ~wx_view_frame_key_handler(){};
    virtual bool handle_key_event(wxKeyEvent &event,time_t& the_modify_time)
    {
        return my_handler->handle_key_event(event,the_modify_time);
    };
};

wx_view_frame::wx_view_frame(const wxString& title, const wxPoint& pos, const wxSize& size):
wxFrame((wxFrame *)NULL, -1, title, pos, size),
my_media(0),my_status(0),
my_hsplitter(0),my_vsplitter(0),my_handle_size(false),my_timer(0)
,my_horizontal_sash_stored(0),
my_horizontal_sash_current(0),
my_vertical_sash_stored(0),
my_vertical_sash_current(0),
my_text_buffer(0)
{
}

void
wx_view_frame::init()
{
    theMessageFrame = this;
    SetSizeHints(100,100);
    int x, y;
    registry_entry tmp_default_width =
        reg::get_default(reg::frame_width);
    registry_entry tmp_default_height =
        reg::get_default(reg::frame_height);
    x = (int)tmp_default_width.get_value().as_long();
    y = (int)tmp_default_height.get_value().as_long();
    SetSize(x,y);

    // The text buffer uses the staus bar, so create that before we open a
    // file.
    if (my_status == 0)
    {
        CreateStatusBar(3);
        my_status = GetStatusBar();
    }

    // My event window handles events.
    my_event_window = new wx_window("events",*this);
    my_event_window->res_add_ref();
    wx_dispatcher::wx_set_default_canvas(*my_event_window);

    this->Show(true);
    this->Raise();

    // There are 2 child windows of the frame, which are each
    // splitter windows.  This gives us 3 windows.
    my_vsplitter =
        new wx_splitter(*(new wx_frame_splitter_notifier(this,wx_splitter_notifier::vertical)),this);
    my_hsplitter =
        new wx_splitter(*(new wx_frame_splitter_notifier(this,wx_splitter_notifier::horizontal)),my_vsplitter);

    // wxWindows kills the panes if there are dragged to zero,
    // which is _not_ what we want.
    my_vsplitter->SetMinimumPaneSize(20);
    my_hsplitter->SetMinimumPaneSize(20);

    // This is where the list of tunes and voices will go.
    my_list_box = new wx_tree(my_vsplitter,wx_tree::wx_tree_select);
    my_list_box->add_ref();

    // Here's the abc source window.
    wx_view_frame_key_handler* tmp_handler = new
        wx_view_frame_key_handler(this);
    my_text_buffer =
        new wx_text_buffer(*my_hsplitter,*this,*tmp_handler);
    my_text_buffer->start();
    the_text_buffer = my_text_buffer;

    // And here's the real canvas where we do the drawing.
    my_view_canvas = new
        wx_view_canvas(my_hsplitter);
    my_view_canvas->SetBackgroundColour(*wxWHITE);

    // Now manage the geometry and stuff.
    my_view_canvas->Show(true);
    my_text_buffer->get_control()->Show(true);
    my_list_box->Show(true);

    // Whenever the sash in the splitters moves around we store the relative % of the
    // sash in the registry.  When iabc starts read these values from the registry
    // and try to restore the previous screen geometry.
    registry_entry tmp_xsash =
            reg::get_default(reg::xsash_default);
    registry_entry tmp_ysash =
            reg::get_default(reg::ysash_default);
    double x_split = tmp_ysash.get_value().as_double();
    double y_split = tmp_ysash.get_value().as_double();
    tmp_xsash =
        (double)my_hsplitter->GetSashPosition() /
        (double)x;
    tmp_ysash =
        (double)my_vsplitter->GetSashPosition() /
        (double)y;

    my_vsplitter->SplitVertically(my_hsplitter,my_list_box,(int)(y_split * (double)y));

    global_windows_event_log.log_event(windows_event_data::set_sash,true,(unsigned long)(100.0 * y_split));
    global_windows_event_log.log_event(windows_event_data::move_sash,(unsigned long)my_vsplitter,
                                       (unsigned long)(100.0 * y_split),y,(unsigned long)(y_split * (double)y));

    my_hsplitter->SplitHorizontally(my_view_canvas,my_text_buffer->get_control(),(int)(x_split * (double)x));

    global_windows_event_log.log_event(windows_event_data::set_sash,false,(unsigned long)(100.0 * x_split));
    global_windows_event_log.log_event(windows_event_data::move_sash,(unsigned long)my_hsplitter,
                                       (unsigned long)(100.0 * x_split),x,(unsigned long)(x_split * (double)x));
    my_hsplitter->Show(true);
    my_vsplitter->Show(true);

    // Create the menu bar.
    create_menu();

    // Set up the timer that we use to poll the text buffer.
    my_timer = new wxTimer(this,TIMER_ID);
    my_timer->Start(500);

    // Finally we are ready to create a music file.
    // If there's no last file, create the 'new' default
    // abc file.
    registry_entry tmp_last_tune =
        reg::get_default(reg::last_tune);
    registry_entry tmp_last_file =
        reg::get_default(reg::last_file);

    my_last_file = tmp_last_file.get_value();
    if (my_last_file.length() == 0)
    {
        my_last_file = new_file;
    }
    if (my_last_file.length() > 0)
    {
        do_open(my_last_file);
    }
    my_handle_size = true;

    // Create the default 'new' file, if its not there:
    string tmp_file = new_file;
    FILE* tmp_new_file = fopen(tmp_file.access_char_array(),"w");
    if (tmp_new_file)
      {
        fprintf(tmp_new_file,"%c iabc (C) Aaron Newman 2004\n",'%');
        fprintf(tmp_new_file,"X:1\n");
        fprintf(tmp_new_file,"T:My New Tune\n");
        fprintf(tmp_new_file,"K:Bb\n");
        fprintf(tmp_new_file,"B,C DE|^F>G A<B|c2 d2 B3- d|c8||\n");
        fclose(tmp_new_file);
        my_text_buffer->reset_file_modify_time();
      }

}

void
wx_view_frame::create_menu()
{
    //// Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(wxID_NEW, wxT("&New..."));
    file_menu->Append(wxID_OPEN, wxT("&Open..."));
    file_menu->Append(wxID_PRINT, wxT("&Print..."));
    file_menu->Append(wxID_SAVE, wxT("&Save..."));
    file_menu->Append(wxID_SAVEAS, wxT("Save &As..."));

    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, wxT("E&xit"));

    wxMenu* edit_menu = new wxMenu;
    edit_menu->Append(ON_UNDO,wxT("Undo (Alt-Bksp)"));
    edit_menu->AppendSeparator();
    edit_menu->Append(ON_CUT,wxT("Cu&t (Ctrl-x)"));
    edit_menu->Append(ON_COPY,wxT("&Copy (Ctrl-c)"));
    edit_menu->Append(ON_PASTE,wxT("&Paste (Ctrl-v)"));
    edit_menu->AppendSeparator();
    my_transpose_item = new wxMenuItem(edit_menu,ON_TRANSPOSE_CHROMATIC,wxT("T&ranspose (Ctrl-Shift-t)"));
    my_transpose_up1_item = new wxMenuItem(edit_menu,ON_TRANSPOSE_UP1,wxT("Transpose Up 1 (Ctrl-UpArrow)"));
    my_transpose_down1_item = new wxMenuItem(edit_menu,ON_TRANSPOSE_DOWN1,wxT("Transpose Down 1 (Ctrl-DownArrow)"));
    my_transpose_up12_item = new wxMenuItem(edit_menu,ON_TRANSPOSE_UP12,wxT("Transpose Up 1/2 (Ctrl-Shift-UpArrow)"));
    my_transpose_down12_item = new wxMenuItem(edit_menu,ON_TRANSPOSE_DOWN12,wxT("Transpose Down 1/2 (Ctrl-Shift-DownArrow)"));
    my_transpose_last_item = new wxMenuItem(edit_menu,ON_TRANSPOSE_REPEAT,
                                            wxT("Repeat Last &Transpose (Ctrl-t)"));
    edit_menu->Append(my_transpose_item);
    edit_menu->Append(my_transpose_up1_item);
    edit_menu->Append(my_transpose_down1_item);
    edit_menu->Append(my_transpose_up12_item);
    edit_menu->Append(my_transpose_down12_item);
    edit_menu->Append(my_transpose_last_item);

    wxMenu* voice_menu = new wxMenu;
    voice_menu->Append(ON_VOICE,wxT("Edit Voices"));

    wxMenu *preferences_menu = new wxMenu;
    preferences_menu->Append(ON_VIEW_PREFERENCES,wxT("View P&references..."));
    preferences_menu->Append(ON_ABC_PREFERENCES,wxT("&ABC Preferences..."));
    preferences_menu->Append(ON_CHANGE_EDITOR_FONT,wxT("Change Editor F&ont..."));

    wxMenu *midi_menu = new wxMenu;
    midi_menu->Append(ON_EXPORT, wxT("Export &Midi..."));
    midi_menu->Append(ON_PLAY, wxT("&Play Midi..."));
    midi_menu->Append(ON_PLAY_FROM_CURSOR, wxT("Play &From Cursor..."));
    midi_menu->Append(ON_SET_PLAYER, wxT("&Choose Midi Player..."));

    wxMenu *view_menu = new wxMenu;
    view_menu->Append(ON_REFRESH,wxT("Re&fresh (Ctrl-L)"));
    view_menu->Append(ON_SCORE_MODE,wxT("&View Mode"),wxT("Switch between Part and Score Modes"));
    my_tune_list_item = new wxMenuItem(view_menu,ON_SHOW_TUNE_LIST,wxT("S&how Tune List"),wxT("Show the list of tunes"),wxITEM_CHECK);

    registry_entry tmp_entry = reg::get_default(reg::show_tune_list);
    bool tmp_value = tmp_entry.get_value().as_bool();
    if (tmp_value == false)
    {
        my_vsplitter->Unsplit(my_list_box);
    }

    view_menu->Append(my_tune_list_item);
    my_tune_list_item->Check(tmp_value);

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(DOCVIEW_ABOUT, wxT("&About"));
    help_menu->Append(ON_DUMP_LOG, wxT("&Dump Log"));
    help_menu->Append(wxID_HELP, wxT("&iABC on the Web"));

    wxMenuBar* menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, wxT("&File"));
    menu_bar->Append(edit_menu, wxT("&Edit"));
    menu_bar->Append(preferences_menu , wxT("&Preferences"));
    menu_bar->Append(view_menu, wxT("&View"));
    menu_bar->Append(midi_menu, wxT("&Midi"));
    menu_bar->Append(voice_menu,wxT("V&oice"));
    menu_bar->Append(help_menu, wxT("&Help"));

    //// Associate the menu bar with the frame
    SetMenuBar(menu_bar);
}

wx_view_frame::~wx_view_frame()
{
    int i;
    for (i = 0;i < my_temp_files.get_size();++i)
    {
        unlink(my_temp_files[i].access_char_array());
    }
    if (my_timer)
    {
        my_timer->Stop();
        delete my_timer;
    }
    my_event_window->res_remove_ref();
    delete my_text_buffer;
}

void wx_view_frame::OnDumpLog(wxCommandEvent& WXUNUSED(event) )
{
#ifndef GCC
    string fn = "C:\\evlog.txt";
#else
    string fn = "/tmp/evlog.txt";
#endif
    FILE* f = fopen(fn.access_char_array(),"w");
    iabc::all_event_logs::print_logs(f);
    fclose(f);
    string tmp_message = (string)"Event log has been saved to " + 
        fn + (string)". Please send to the author";
    wxMessageBox(B2U(tmp_message.access_char_array()));
}

void wx_view_frame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox(wxT("iabc Version 1.01 (C) Aaron Newman 2005"));
}
#ifdef WIN32

iabc::string GetDefaultRegKey(const string& key)
{
    DWORD   dwType = 0;
    DWORD   dwIndex = 0;
    LONG    retCode;
    CHAR    ValueName[128];
    DWORD   cbValueName = sizeof (ValueName);
    string  s;
    DWORD   len = 256;
    char buf[1024];
    // LPTSTR   buf = s.GetBuffer(len);
    HKEY    hKey;

    // OPEN THE KEY, quit on any errors

    retCode = RegOpenKeyEx(
            HKEY_CLASSES_ROOT,      // Key handle at root level.
            (B2U(key.access_char_array())),                // Path name of child key.
            0,                      // Reserved.
            KEY_READ,               // Requesting read access.
            &hKey);                 // Address of key to be returned.

    if (retCode != ERROR_SUCCESS)
        {
        throw "RegOpenKeyEx(HKEY_CLASSES_ROOT\\";
        return "";
        }

    // Now enumerate the keys, looking for the default

    while ((retCode == 0) && (cbValueName != 0))
        {
        cbValueName = sizeof (ValueName);
        len = 256;

        retCode = RegEnumValue(
            hKey,           // Key handle returned from RegOpenKeyEx.
            dwIndex,        // Value index
            (MS_LP_STRING)ValueName,      // Name of value.
            &cbValueName,   // Size of value name.
            NULL,           // Reserved, dword = NULL.
            &dwType,        // Type of data.
            (LPBYTE) buf,   // Data buffer.
            &len            // Size of data buffer.
        );

        dwIndex++;
        }

    if (retCode != ERROR_SUCCESS)
        {
        throw("RegEnumValue(HKEY_CLASSES_ROOT\\");
        buf[0] = '\0';
        }

    if (dwType != REG_SZ)
        {
        if (dwType != REG_EXPAND_SZ)
            {
            buf[0] = '\0';
            }
        else
            {
            char Buf[2000];
            int Size = ExpandEnvironmentStrings(B2U(buf),(wxChar*)(Buf), sizeof(Buf));
            if (Size == 0)
                {
                throw("HKEY_CLASSES_ROOT\\");
                buf[0] = '\0';
                }
            else if (Size > sizeof(Buf))
                {
                buf[0] = '\0';
                }
            else
                {
                ;
                }
            }
        }

    s = buf;
    return s;
}
#endif

void
wx_view_frame::OnCut(wxCommandEvent& event)
{
    if (my_text_buffer)
    {
        my_text_buffer->Cut();
    }
}

void
wx_view_frame::OnPaste(wxCommandEvent& event)
{
    if (my_text_buffer)
    {
        my_text_buffer->Paste();
    }
}

void
wx_view_frame::OnUndo(wxCommandEvent& event)
{
    if (my_text_buffer)
    {
        my_text_buffer->Undo();
    }
}

void
wx_view_frame::OnCopy(wxCommandEvent& event)
{
    if (my_text_buffer)
    {
        my_text_buffer->Copy();
    }
}

void wx_view_frame::OnHelp(wxCommandEvent& WXUNUSED(event) )
{
#ifdef WIN32
    string s = GetDefaultRegKey(".htm");
    s = GetDefaultRegKey(s+(string)"\\shell\\open\\command");
    s += " http://abc.sourceforge.net/iabc";
#else
    string s = "/usr/bin/mozilla";
    s += " http://abc.sourceforge.net/iabc";
#endif
    wxExecute(B2U(s.access_char_array()));
}

bool
wx_view_frame::ProcessEvent(wxEvent &event)
{
    if (event.GetEventType() == wxEVT_USER_FIRST)
    {
        wx_window_object* tmp_object =
            (wx_window_object*)event.GetEventObject();
        wx_dispatcher* tmp_dispatcher = tmp_object->my_dispatcher;
        tmp_dispatcher->platform_execute();
        delete tmp_object;
        event.SetEventObject(0);
        return true;
    }
    else
    {
        return (wxWindow::ProcessEvent(event));
    }
}

void
wx_view_frame::OnWindowCreate(wxWindowCreateEvent& the_event)
{
}

int
wx_view_frame::get_current_line_position()
{
    int tmp_buf_line = 0;
    if (my_text_buffer)
    {
        point tmp_text_point =
            my_text_buffer->get_insertion_point();

        tmp_buf_line = tmp_text_point.y + 1;
    }

    return tmp_buf_line;
}

void
wx_view_frame::commit_sash_position_change()
{
    if (my_horizontal_sash_current != my_horizontal_sash_stored)
    {
        global_windows_event_log.log_event(windows_event_data::set_sash,
                                           0,(unsigned long)(100.0 * my_horizontal_sash_current));
        my_horizontal_sash_stored = my_horizontal_sash_current;
        registry_entry tmp_x =
            reg::get_default(reg::xsash_default);
        tmp_x = string(my_horizontal_sash_current).access_char_array();
        globalRegistry.set_value(tmp_x);
        globalRegistry.flush();
    }
    if (my_vertical_sash_current != my_vertical_sash_stored)
    {
        global_windows_event_log.log_event(windows_event_data::set_sash,
                                           1,(unsigned long)(100.0 * my_vertical_sash_current));
        my_vertical_sash_stored = my_vertical_sash_current;
        registry_entry tmp_y =
            reg::get_default(reg::ysash_default);
        tmp_y = string(my_vertical_sash_current).access_char_array();
        globalRegistry.set_value(tmp_y);
        globalRegistry.flush();
    }
}

void
wx_view_frame::change_sash_position(double the_new_value,wx_splitter_notifier::sash_direction the_direction)
{
    if (the_direction == wx_splitter_notifier::horizontal)
    {
        my_horizontal_sash_current = the_new_value;
    }
    else
    {
        my_vertical_sash_current = the_new_value;
    }
}

void
wx_view_frame::OnShowTuneList(wxCommandEvent& event)
{
    registry_entry tmp_entry = reg::get_default(reg::show_tune_list);
    bool tmp_value = tmp_entry.get_value().as_bool();
    if (tmp_value == false)
    {
        my_tune_list_item->Check(true);
        if (my_vsplitter->IsSplit() == false)
        {
            my_vsplitter->SplitVertically(my_hsplitter,my_list_box);
            my_list_box->Show();
            tmp_entry = "true";
            globalRegistry.set_value(tmp_entry);
            globalRegistry.flush();
        }
    }
    else
    {
        my_tune_list_item->Check(false);
        if (my_hsplitter->IsSplit() == true)
        {
            my_vsplitter->Unsplit(my_list_box);
            my_list_box->Hide();
            tmp_entry = "false";
            globalRegistry.set_value(tmp_entry);
            globalRegistry.flush();
        }
    }
}

void
wx_view_frame::OnVoice(wxCommandEvent& event)
{
    map<int,string> tmp_name_map = my_media->get_voice_names();
    map<int,bool> tmp_on_map = my_media->get_voice_status();

    wx_voice_options_dialog* tmp_db =
        new wx_voice_options_dialog(tmp_on_map,tmp_name_map);
    tmp_db->ShowModal();
    my_media->set_voice_on_map(tmp_on_map);
    my_media->choose_tune();
    delete tmp_db;

}

void
wx_view_frame::OnTranspose(wxCommandEvent& event)
{
    int rv = wxID_OK;
    int tmp_steps = iabc::transpose_steps;
    bool tmp_diatonic = iabc::transpose_diatonic;
    bool tmp_refresh = iabc::transpose_refresh;

    // There are lots of different flavors of transpose - diatonic etc.
    // Figure out which one and pop up a dialog if required.
    if (event.GetId() == ON_TRANSPOSE_CHROMATIC)
    {
        rv = global_transpose_dialog->ShowModal();
        tmp_diatonic = iabc::transpose_diatonic;
        tmp_refresh = iabc::transpose_refresh;
        tmp_steps = iabc::transpose_steps;
    }
    else if (event.GetId() == ON_TRANSPOSE_UP1)
    {
        tmp_steps = 1;
        tmp_diatonic = true;
        tmp_refresh = true;
    }
    else if (event.GetId() == ON_TRANSPOSE_UP12)
    {
        tmp_steps = 1;
        tmp_diatonic = false;
        tmp_refresh = true;
    }
    else if (event.GetId() == ON_TRANSPOSE_DOWN12)
    {
        tmp_steps = -1;
        tmp_diatonic = false;
        tmp_refresh = true;
    }
    else if (event.GetId() == ON_TRANSPOSE_DOWN1)
    {
        tmp_steps = -1;
        tmp_diatonic = true;
        tmp_refresh = true;
    }

    // Now we know how many steps to transpose.  Perform the action.
    if (rv == wxID_OK)
    {
        // Figure out what is selected.  We only transpose selections.
        long start,end;
        my_text_buffer->get_control()->GetSelection(&start,&end);
        string tmp_old_string = (string)U2B(my_text_buffer->get_control()->GetRange(start,end));
        string tmp_string = tmp_old_string;
        // Transpose the selected string.
        tmp_string =
            my_media->transpose_selection(start,end,
                                          tmp_string,
                                          tmp_steps,tmp_diatonic);

        // Cut the old text out of the control.
        my_text_buffer->get_control()->Remove(start,end);

        // Create a special undo event that will remove the new text
        // and replace it with the old.
        undo_event tmp_event;
		tmp_event.is_modified = my_text_buffer->get_control()->IsModified();
        tmp_event.the_buffer = tmp_old_string;
        tmp_event.start_select = start;
        tmp_event.end_select = end;
		undo_buffer::push(tmp_event);

        // Now insert the new text in the buffer and redraw the music if required.
        my_text_buffer->get_control()->SetInsertionPoint(start);
        my_text_buffer->get_control()->WriteText(B2U(tmp_string.access_char_array()));
        my_text_buffer->get_control()->MarkDirty();
        if (tmp_refresh)
        {
            OnRefresh(event);
        }

        // Select the new transpose so the user can do it again.
        end = start + tmp_string.length();
        my_text_buffer->get_control()->SetSelection(start,end);
    }
}

void
wx_view_frame::OnTick(wxTimerEvent& the_event)
{
    if ((my_media) &&
        (my_text_buffer))
    {
        commit_sash_position_change();

        if ((my_view_canvas) &&
            (my_view_canvas->get_and_reset_my_should_refresh() == true))
        {
            my_view_canvas->Refresh(true);
        }

        // Someone likes to edit abc in a different editor
        if (my_text_buffer->has_underlying_file_changed(my_text_buffer->get_filename()))
        {
            int tmp_modal = wxID_YES;
            global_doc_event_log.log_event(doc_event_data::underlying_file_changed);

            // We need to stop the timer before the modal dialog is popped or
            // we still get timer events while waiting.
            my_timer->Stop();
            wxMessageDialog tmpDialog(this,wxT("File has changed; reload?"),wxT("Information"),wxYES_NO);
            tmp_modal = tmpDialog.ShowModal();

            // Once the dialog goes its safe to restart the timer again.
            my_timer->Start(500);
            if (tmp_modal == wxID_YES)
            {
                my_text_buffer->load(my_text_buffer->get_filename());
                wxCommandEvent ev;
                ev.SetId(ON_REFRESH);
                OnRefresh(ev);
            }

            // Regardless, reset the file time to now so that we don't keep
            // getting reminded.
            my_text_buffer->reset_file_modify_time();
        }

        // Set the focus in the text window, unless we are drawing
        // somewhere else.
        if (my_media->is_parsing() == false)
        {
            my_text_buffer->refresh();
        }
        wxString tmp_title = B2U(my_text_buffer->get_filename().access_char_array());
        if (my_text_buffer->get_control()->IsModified())
        {
            tmp_title = tmp_title + wxT(" * ");
        }
        SetTitle(tmp_title);

        int tmp_buf_line = get_current_line_position();
        my_media->handle_tick(tmp_buf_line);

        // Some menu items are disabled when nothing is selected
        if (my_text_buffer)
        {
            long start,end;
            my_text_buffer->get_control()->GetSelection(&start,&end);
            bool tmp_enable = false;
            if (start != end)
            {
                tmp_enable = true;
            }

            my_transpose_item->Enable(tmp_enable);
            my_transpose_up1_item->Enable(tmp_enable);
            my_transpose_down1_item->Enable(tmp_enable);
            my_transpose_up12_item->Enable(tmp_enable);;
            my_transpose_down12_item->Enable(tmp_enable);
            my_transpose_last_item->Enable(tmp_enable);
        }

        // Refresh the view if the view has changed.
        if (the_view_parameters_changed)
        {
            the_view_parameters_changed = false;
            wxCommandEvent ev;
            ev.SetId(ON_REFRESH);
            OnRefresh(ev);
        }
    }
}

void
wx_view_frame::OnSize(wxSizeEvent& the_event)
{
    if (my_handle_size == true)
    {
        int tmp_width = my_vsplitter->GetSize().GetWidth();
        int tmp_height = my_hsplitter->GetSize().GetHeight();

        // Probably a wxWindow bug - we get size events when the
        // window is minimized, ignore those.
        if ((tmp_width > 0) && (tmp_height > 0) && (this->IsIconized() == false))
        {
            // Whenever the sash in the splitters moves around we store the relative % of the
            // sash in the registry.  Whenever we resize the windows we reset the sash so
            // that it is in the same relative position as the last resize.
            wxSize tmp_size = the_event.GetSize();
            registry_entry tmp_xsash =
                    reg::get_default(reg::xsash_default);
            double x_split = tmp_xsash.get_value().as_double();
            registry_entry tmp_ysash =
                    reg::get_default(reg::ysash_default);
            double y_split = tmp_ysash.get_value().as_double();
            tmp_xsash =
                (double)my_hsplitter->GetSashPosition() /
                (double)tmp_width;
            tmp_ysash =
                (double)my_vsplitter->GetSashPosition() /
                (double)tmp_height;

            string tmp_x_string = (string)(long)(tmp_size.GetWidth());
            string tmp_y_string = (string)(long)(tmp_size.GetHeight());
            registry_entry tmp_x =
                registry_entry(reg::frame_width.as_key(),tmp_x_string.access_char_array(),true);
            registry_entry tmp_y =
                registry_entry(reg::frame_height.as_key(),tmp_y_string.access_char_array(),true);

            my_hsplitter->SetSashPosition((int)(x_split * tmp_y_string.as_double()));
            my_vsplitter->SetSashPosition((int)(y_split * tmp_x_string.as_double()));

            globalRegistry.set_value(tmp_x);
            globalRegistry.set_value(tmp_y);
            globalRegistry.flush();
        }
    }
    wxFrame::OnSize(the_event);
}

void
wx_view_frame::OnRefresh(wxCommandEvent& the_event)
{
    // Make some attempt at not closing a document that is
    // opening although there's still some race conditions here
    if ((my_media) && (my_media->is_parsing() == false))
    {
        long start,end;
        long x,y;
        long tmp_point = (my_text_buffer->get_control()->GetInsertionPoint());
        wxTextCtrl* tmp_control = my_text_buffer->get_control();

        // The text buffer does not include the \r character for eol, but the
        // stored file does.  Compensate for that here.
        tmp_control->PositionToXY(tmp_point,&x,&y);
        tmp_point -= y;
        tmp_control->GetSelection(&start,&end);

        my_media->refresh_current_tune(tmp_point);
        tmp_control->SetSelection(start,end);
    }
}

void
wx_view_frame::OnSetPlayer(wxCommandEvent& the_event)
{
    global_midi_options_dialog->ShowModal();
}

string
wx_view_frame::get_random_filename(const string& the_directory)
{
    string tmp_file = "~tmp_";
    int i;
    for (i = 0;i < 10;++i)
    {
        tmp_file += (char)('A' + (char) (rand() % 26));
    }
    tmp_file += ".mid";
    tmp_file = the_directory + (string)"/" + tmp_file;
    return tmp_file;
}

string
wx_view_frame::get_midi_command_string(const string& the_player,
                                       const string& the_command_line,
                                       const string& the_midi_file)
{
    string tmp_command;
    bool tmp_added_player = false;
    bool tmp_added_file = false;
    enum mstate {
        percent,normal
    } tmp_state = normal;
    int i;
    for (i = 0;i < the_command_line.length();++i)
    {
        char tmp_char = the_command_line[i];
        if (tmp_state == percent)
        {
            if (tmp_char == 'f')
            {
                tmp_command += the_midi_file;
                tmp_added_file = true;
            }
            else if (tmp_char == 'p')
            {
                tmp_command += the_player;
                tmp_added_player = true;
            }
            else
            {
                tmp_command += '%';
                tmp_command += tmp_char;
            }
            tmp_state = normal;
        }
        else if (tmp_state == normal)
        {
            if (tmp_char == '%')
            {
                tmp_state = percent;
            }
            else
            {
                tmp_command += tmp_char;
            }
        }
    }

    // If the line is screwed up, just use a default.
    if ((tmp_added_file == false) ||
        (tmp_added_player == false))
    {
        tmp_command = the_player + (string)" " + the_midi_file;
    }

    return tmp_command;
}

void
wx_view_frame::OnPlay(wxCommandEvent& the_event)
{
    // Make some attempt at not closing a document that is
    // opening although there's still some race conditions here
    if ((my_media) && (my_media->is_parsing() == false))
    {
        // If the user has not selected a midi player yet,
        // give them a shot at doing so.
        if (midi_player_name.length() == 0)
        {
#ifdef GCC
            global_midi_options_dialog->ShowModal();
#else
            midi_player_name = (string)"C:\\Program Files\\Windows Media Player\\wmplayer.exe";
#endif
        }

        if (midi_player_name.length() > 0)
        {
            string tmp_midi_file =
                get_random_filename(iabc::midi_output_directory);

            string tmp_midi_command =
                get_midi_command_string(iabc::midi_player_name,
                                        iabc::midi_player_command,
                                        tmp_midi_file);


            // Make some attempt to erase the files later.
            my_temp_files.add_to_end(tmp_midi_file);

            // If this is a 'play from', figure out where in the tune
            // we are based on the cursor location.  Then use that as
            // a starting point for the midi file.
            score_point tmp_point;
            if (the_event.GetId() == ON_PLAY_FROM_CURSOR)
            {
                tmp_point =
                    my_media->get_score_point_from_text_position(
                        get_current_line_position());
                // We want all the active voices from a certain location, so
                // force the 'voice' value to be 1.
                tmp_point = score_point(tmp_point.measure,
                                        tmp_point.beat,
                                        1);
            }
            my_media->play(tmp_midi_file,
                my_text_buffer->get_control()->GetInsertionPoint(),
                tmp_point);
            wxExecute(B2U((tmp_midi_command.access_char_array())));
        }
    }
}

void
wx_view_frame::OnExport(wxCommandEvent& the_event)
{
    // Make some attempt at not closing a document that is
    // opening although there's still some race conditions here
    if ((my_media) && (my_media->is_parsing() == false))
    {
        string tmp_file = my_last_file;
        if ((my_last_file.length() > 4) &&
            (my_last_file.mid(my_last_file.length() - 4,4) == (string)".abc"))
        {
            tmp_file = my_last_file.mid(0,my_last_file.length() - 4);
            tmp_file += (string)(".mid");
        }
        wxFileDialog dialog(this, wxT("Open midi file"), 
			wxT(""), 
			B2U(tmp_file.access_char_array()),  
			wxT("Midi File (*.mid)|*.mid|All Files(*.*)|*.*"), 
			wxSAVE|wxOVERWRITE_PROMPT);
        if (dialog.ShowModal() == wxID_OK)
        {
            tmp_file = U2B(dialog.GetPath());
            my_media->play(tmp_file,my_text_buffer->get_control()->GetInsertionPoint(),score_point());
        }
    }
}

void
wx_view_frame::OnPrint(wxCommandEvent& event)
{
    if ((my_media == 0) ||
        (my_media->is_parsing() == false))
    {
        my_timer->Stop();
        my_media->print();
        my_timer->Start(500);
    }
}

void
wx_view_frame::OnSave(wxCommandEvent& the_event)
{
    if (my_last_file == new_file)
    {
        OnSaveAs(the_event);
    }
    else if (((my_media == 0) ||
        (my_media->is_parsing() == false)) &&
        (my_last_file.length() > 0))
    {
        my_text_buffer->get_control()->Save(my_last_file);
        my_text_buffer->reset_file_modify_time();
    }
}

void
wx_view_frame::OnSaveAs(wxCommandEvent& the_event)
{
    if (((my_media == 0) ||
        (my_media->is_parsing() == false)) &&
        (my_last_file.length() > 0))
    {
        // Fix pathname for platform-independence.
        int i;
        string tmp_newstring;
        for (i = 0;i < my_last_file.length();++i)
        {
#ifndef GCC
            if (my_last_file[i] == '/')
            {
                tmp_newstring += '\\';
            }
#else
            if (my_last_file[i] == '\\')
            {
                tmp_newstring += '/';
            }
#endif
            else
            {
                tmp_newstring += my_last_file[i];
            }
        }
        my_last_file = tmp_newstring;
        wxFileDialog dialog(this, wxT("Open abc file"), 
			wxT(""), 
			B2U(my_last_file.access_char_array()),
            wxT("ABC File (*.abc)|*.abc|All Files(*.*)|*.*"), 
			wxSAVE|wxOVERWRITE_PROMPT);
        string tmp_file;
        if (dialog.ShowModal() == wxID_OK)
        {
            tmp_file = U2B(dialog.GetPath());
			my_text_buffer->get_control()->Save(tmp_file);
            SetTitle(B2U(tmp_file.access_char_array()));
            my_text_buffer->load(tmp_file.access_char_array());
            my_last_file = tmp_file;
            registry_entry tmp_entry = registry_entry("last_file","filename","",
                                                      tmp_file.access_char_array(),true);
            globalRegistry.set_value(tmp_entry);
            globalRegistry.flush();
        }
        else
        {
            return;
        }
    }
}

void
wx_view_frame::OnChangeEditorFont(wxCommandEvent& event)
{
    // Load the default string names.
    string tmp_font_name = reg::get_default(reg::text_buffer_font_name).get_value();
    string tmp_font_size = reg::get_default(reg::text_buffer_font_size).get_value();

    // Display the font dialog
    wxFontDialog tmpDialog;
    wxFont tmp_initial_font;
    tmp_initial_font.SetFaceName(B2U(tmp_font_name.access_char_array()));
    tmp_initial_font.SetPointSize(tmp_font_size.as_long());
    tmpDialog.Create(this);
    tmpDialog.SetTitle(wxT("Choose Font for Editor"));
    if (tmpDialog.ShowModal() == wxID_OK)
    {
        wxFont tmpNewFont = tmpDialog.GetFontData().GetChosenFont();
        tmp_font_name = U2B(tmpNewFont.GetFaceName());
        tmp_font_size = string((long)tmpNewFont.GetPointSize());
        // Save the new values in the registry
        reg::set_new_default_value(reg::text_buffer_font_name,tmp_font_name);
        reg::set_new_default_value(reg::text_buffer_font_size,tmp_font_size);
        // set the font in the buffer.
        my_text_buffer->get_control()->SetFont(tmpNewFont);
    }
}

void
wx_view_frame::OnABCPreferences(wxCommandEvent& the_event)
{
    global_abc_options_dialog->ShowModal();
}

void
wx_view_frame::OnScoreMode(wxCommandEvent& the_event)
{
    global_score_mode_db->ShowModal();
    my_media->choose_tune();
}

void
wx_view_frame::OnViewPreferences(wxCommandEvent& the_event)
{
    global_window_media_pp->ShowModal();
}

void
wx_view_frame::OnOpen(wxCommandEvent& the_event)
{
    if ((my_media == 0) ||
        (my_media->is_parsing() == false) &&
        (really_exit()))
    {
        wxFileDialog dialog(this, wxT("Open abc file"), wxT(""), wxT(""),
                            wxT("ABC File (*.abc)|*.abc|All Files(*.*)|*.*"), 0);
        if (dialog.ShowModal() == wxID_OK)
        {
            my_last_file = (string)U2B(dialog.GetPath());
            do_close(true);
            my_text_buffer->get_control()->clear_undo();
            do_open(my_last_file);
        }
        else
        {
            ;
        }
    }
}

void
wx_view_frame::OnNew(wxCommandEvent& the_event)
{
    if (((my_media == 0) ||
        (my_media->is_parsing() == false)) &&
        (really_exit()))
    {
        do_close(true);
        my_text_buffer->get_control()->clear_undo();

        // We may be in an unwritable directory, switch back now.
        wxSetWorkingDirectory(B2U(home_dir.access_char_array()));
        my_last_file = new_file;
        FILE* tmp_new_file = fopen(new_file.access_char_array(),"w");
        if (tmp_new_file)
        {
            fprintf(tmp_new_file,"%c iabc (C) Aaron Newman 2002\n",'%');
            fprintf(tmp_new_file,"X:1\n");
            fprintf(tmp_new_file,"T:My New Tune\n");
            fprintf(tmp_new_file,"K:Bb\n");
            fprintf(tmp_new_file,"B,C DE|^F>G A<B|c2 d2 B3- d|c8||\n");
            fclose(tmp_new_file);
        }
        my_text_buffer->reset_file_modify_time();
        do_open(my_last_file);
    }
}

bool
wx_view_frame::really_exit()
{
    bool tmp_rv = true;
    if ((my_media) &&
        (my_media->is_parsing() == false) &&
        (my_text_buffer->get_control()->IsModified() == true))
        {
        wxMessageDialog d(0,wxT("Save Changes before exit?"),wxT("Discard Changes"),
                          wxYES_NO | wxCANCEL);
        int resp = d.ShowModal();
        if (resp == wxID_YES)
            {
            wxCommandEvent e;
            OnSave(e);
            }
        else if (resp == wxID_CANCEL)
            {
            tmp_rv = false;
            }
        }

    return tmp_rv;
}

void
wx_view_frame::do_close(bool the_destroy_media)
{
    if ((my_media) && (the_destroy_media))
    {
        my_media->close();
        my_media->remove_ref();
        my_media = 0;
    }
}

void wx_view_frame::OnCloseMenu(wxCommandEvent& the_event)
{
    wxCloseEvent other_event;
    OnCloseWindow(other_event);
}

void
wx_view_frame::OnCloseWindow(wxCloseEvent& the_event)
{
    if (really_exit())
        {
        globalUserAbortFlag = true;
        dispatcher::end_app();
        thread::sleep(1000);
        thread_mgr::get_instance().shutdown_application();
        do_close(true);
        UnloadResources();
        Destroy();
        }
    else
        {
        the_event.Skip(FALSE);
        }
}

#define TRANSPOSE_ID(i) {\
                id = i; \
            wxCommandEvent e; \
            e.SetId(i); \
            OnTranspose(e); \
            tmp_rv = true;}

bool
wx_view_frame::handle_key_event(wxKeyEvent& event,time_t& the_modify_time)
{
    bool tmp_rv = false;
    bool tmp_control = event.ControlDown();
    bool tmp_shift = event.ShiftDown();
    int id;

    if (tmp_control == true)
    {
        int tmp_code = event.GetKeyCode();
        // I used to use the rich edit control in windows which took care of
        // paste.  But now I am not using that so that the control is more like
        // the Linux version.
#if 0
        if ((tmp_code == 22)) // paste in wxWindows
        {
            // We ignore this event in windows because the OS handles
            // this before us, before we even get the event.
            the_modify_time = time(0) + (time_t)1;
            tmp_rv = true;
        }
#endif
        // Up arrow
        if (tmp_code == 317)
        {
            if (tmp_shift)
                {TRANSPOSE_ID(ON_TRANSPOSE_UP12);}
            else
                {TRANSPOSE_ID(ON_TRANSPOSE_UP1);}
        }
        else if (tmp_code == 319)
        {
            if (tmp_shift)
                {TRANSPOSE_ID(ON_TRANSPOSE_DOWN12);}
            else
                {TRANSPOSE_ID(ON_TRANSPOSE_DOWN1);}
        }
        else if (tmp_code == (int)CONTROL_T) // 'ctrl-t in wxWindows, go figure
        {
            if (tmp_shift)
                {TRANSPOSE_ID(ON_TRANSPOSE_REPEAT);}
            else
                {TRANSPOSE_ID(ON_TRANSPOSE_CHROMATIC);}
        }
        else if (tmp_code == (int)CONTROL_L) // 'ctrl-l in wxWindows, go figure
        {
            wxCommandEvent e;
            e.SetId(ON_REFRESH);
            OnRefresh(e);
            tmp_rv = true;
        }
    }

    return tmp_rv;
}

void
wx_view_frame::do_open(const string& the_file)
{
    global_settings_event_log.log_event(the_file.access_char_array(),
                                         the_file.length(),
                                         settings_event_data::set_filename);

    if ((my_media == 0) ||
        (my_media->is_parsing() == false))
    {
        do_close(true);

        // I am the frame.  Set the title of the window frame to indicate
        // that we have opened this file.
        SetTitle(B2U(the_file.access_char_array()));

        // Erase old midi files
    #ifdef __X__
        // X seems to require a forced resize
        int x, y;
        GetSize(&x, &y);
        SetSize(-1, -1, x, y);
    #endif
        // Now create a new media object to handle the new file.
        my_media = new iabc_media_manager(*this,
                                          *my_list_box,*my_view_canvas,*my_text_buffer);
        my_media->add_ref();
        my_media->open(the_file);
    }
}
}



