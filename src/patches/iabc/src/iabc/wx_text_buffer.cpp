#include "iabc/wx_text_buffer.h"
#include "iabc/factory.h"
#include "iabc/map.cpp"
#include "iabc/list.cpp"
#include "wx/filefn.h"
#include <wx/frame.h>
#include "iabc/threads.h"
#include <wx/bitmap.h>
#include <wx/caret.h>
#include <wx/fontdlg.h>
#include <wx/msgdlg.h>
#include "iabc/registry_defaults.h"
#include "wx/dataobj.h"
#include "wx/clipbrd.h"
#include "iabc/unicode.h"

// May need to change for Linux
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif
#include "iabc/drawtemp.cpp"
#ifdef __WXGTK__
#define WX_CLIPBOARD_BROKEN 1
#endif

#define MOVE_CURSOR(k) (SetInsertionPoint(GetInsertionPoint() + (k)))
#define KEY_CODE(e) ((e).the_event != 0 ? (e).the_event->GetKeyCode() : 0)
#define ALT_DOWN(e) ((e).the_event != 0 ? (e).the_event->AltDown() : 0)
#define CTRL_DOWN(e) ((e).the_event != 0 ? (e).the_event->ControlDown() : 0)

namespace iabc
{
;

IMPLEMENT_CLASS(wx_text_control, wxTextCtrl)
BEGIN_EVENT_TABLE(wx_text_control, wxTextCtrl)
    EVT_CHAR(wx_text_control::OnChar)
    EVT_SET_FOCUS(wx_text_control::OnFocus)
END_EVENT_TABLE()

wx_text_control::wx_text_control():my_frame(0),my_buffer_modify_time(time(0)),my_key_handler(0),my_is_undoing(false)
    ,my_is_modified(false)
{
}

wx_text_control::wx_text_control(wxWindow *parent,
                                 wxWindowID id,
                                 wxFrame* frame,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name):
my_frame(frame),wxTextCtrl(parent,id,value,pos,size,style,validator,name),
    my_buffer_modify_time(time(0)),my_key_handler(0),my_is_undoing(false),
    my_is_modified(false)
{
}

void 
wx_text_control::MarkDirty()
{
    my_is_modified = true;
}
void 
wx_text_control::DiscardEdits()
{
    my_is_modified = false;
}

bool 
wx_text_control::LoadFile(const wxString& filename)
{
    string s;
	bool tmp_rv = false;
    string fn = U2B(filename);
	FILE* f = fopen(fn.access_char_array(),"r");
    if (f)
    {
        int c = getc(f);
        while (c >= 0)
        {
            s += (char)c;
            c = getc(f);
        }
		tmp_rv = true;
        fclose(f);
    }
	SetValue(B2U(s.access_char_array()));
	return tmp_rv;
}

void
wx_text_control::SetInsertionPoint(long pos)
{
    wxTextCtrl::SetInsertionPoint(pos);
    UpdateCursorPos();
}

void
wx_text_control::UpdateCursorPos()
{
    if (my_frame)
    {
        long x = 0;
        long y = 0;
        long tmp_point = GetInsertionPoint();
        PositionToXY(tmp_point,&x,&y);
        ++x;++y;
        string tmp_string = (string)"Ln: " + (string)y + (string)" Col: " + (string)x;
        my_frame->SetStatusText(B2U(tmp_string.access_char_array()),1);
    }
}

void
wx_text_control::OnFocus(wxFocusEvent& event)
{
    UpdateCursorPos();
    event.Skip(false);
}

void
wx_text_control::Cut(bool the_use_clipboard)
{
    // We do a few things here.  First, create an 'undo' event
    // and queue it.  Second, remove the text from the app.  Third,
    // put the data on the global clipboard.
    undo_event tmp_event;
    tmp_event.is_modified = IsModified();
    long tmp_end;
    // Note that we want the end of the undo select area to be 0.  This is 
    // because the undo of a cut is a paste and not a replace.
    GetSelection(&(tmp_event.start_select),
                         &tmp_end);
    tmp_event.the_buffer = (string)GetRange(tmp_event.start_select,
                                            tmp_end).mb_str();
	global_text_event_log.log_event(text_event_data::cut);
    if (my_is_undoing == false)
    {
        global_text_event_log.log_event(text_event_data::push_undo,
                              (unsigned long)0,
                              (unsigned long)tmp_event.the_buffer.length(),
                              (unsigned long)tmp_event.start_select,
                              (unsigned long)tmp_event.is_modified);
    
        undo_buffer::push(tmp_event);
    }

    // wxClipboard is broken under gtk+, this is the best we can do :-(
#ifdef WX_CLIPBOARD_BROKEN
    wxTextCtrl::Cut();
#else
    // Take the text out of the app.
    Remove(tmp_event.start_select,
           tmp_end);
#endif

    // Move the data to the clipboard
    if ((the_use_clipboard == true) &&
        (wxTheClipboard->Open()))
    {
        // clipboard will delete
        wxTextDataObject* tmp_data = new  wxTextDataObject(B2U(tmp_event.the_buffer.access_char_array()));
        wxTheClipboard->SetData(tmp_data);
        wxTheClipboard->Close();
    }
}

void
wx_text_control::Copy()
{
    // We do a few things here.  First, get the data from the selection.
    // Second, put the data on the global clipboard.  Third, remove the
    // selection.
    long start_select,end_select;
    GetSelection(&start_select,
                         &end_select);
    string tmp_buffer = (string)GetRange(start_select,
                                            end_select).mb_str();
    global_text_event_log.log_event(text_event_data::copy);

    // Move the data to the clipboard
    wxTheClipboard->UsePrimarySelection();
#ifdef WX_CLIPBOARD_BROKEN
    wxTextCtrl::Copy();
#endif
    if (wxTheClipboard->Open())
    {
        // clipboard will delete
        wxTextDataObject* tmp_data = new  wxTextDataObject(B2U(tmp_buffer.access_char_array()));
        wxTheClipboard->SetData(tmp_data);
        wxTheClipboard->Close();
    }

    // Remove the selection from the selected text and put the cursor after the
    // selection.  That's what Windows apps generally do.
    SetSelection(start_select,end_select);
    SetInsertionPoint(end_select);
}

void
wx_text_control::Replace(long from,long to,const wxString& value)
{
    wxTextCtrl::Replace(from,to,value);
    my_buffer_modify_time = time(0) + (time_t)1;
}

void 
wx_text_control::Undo()
{
    my_is_undoing = true;
    pop_undo_event();
    my_is_undoing = false;
}

void
wx_text_control::Paste()
{
    wxTextDataObject data;
    undo_event tmp_event;
    tmp_event.is_modified = IsModified();
#ifdef WX_CLIPBOARD_BROKEN
    wxTextCtrl::Paste();
#endif

    if (wxTheClipboard->IsSupported( data.GetFormat() ))
    {
        if (wxTheClipboard->GetData( data ))
        {
            string tmp_string = (string)data.GetText().mb_str();
            tmp_event.start_select = GetInsertionPoint();
            tmp_event.end_select = tmp_event.start_select + tmp_string.length();
            global_text_event_log.log_event(text_event_data::paste);
            if (my_is_undoing == false)
            {
                global_text_event_log.log_event(text_event_data::push_undo,
                                      (unsigned long)0,
                                      (unsigned long)tmp_event.the_buffer.length(),
                                      (unsigned long)tmp_event.start_select,
                                      (unsigned long)tmp_event.is_modified);

                undo_buffer::push(tmp_event);
            }
            WriteText(B2U(tmp_string.access_char_array()));
            MarkDirty();
        }
    }
}

bool
wx_text_control::is_undo_event(const wxKeyEvent& event)
{
    return ((event.AltDown()) && (event.GetKeyCode() == WXK_BACK));
}

void
wx_text_control::push_undo_event(wxKeyEvent& event,long x,long y)
{
    // Get a whole bunch of information about the cursor position, selection etc.
    bool tmp_has_changed = false;
    int tmp_key = event.GetKeyCode();
    bool tmp_shift = event.ShiftDown();
    bool tmp_control = event.ControlDown();
    bool tmp_alt = event.AltDown();
    long tmp_position = GetInsertionPoint();
    long tmp_last_position = GetLastPosition();
    int tmp_chars = (int)GetLineLength(y);
    long tmp_start_selection,tmp_end_selection;
    int tmp_delta = 0;
    bool tmp_should_push = true;
    GetSelection(&tmp_start_selection,&tmp_end_selection);
    string tmp_selection;
    if (tmp_start_selection != tmp_end_selection)
    {
        tmp_selection = (const char*)U2B(GetRange(tmp_start_selection,tmp_end_selection));
    }
    undo_event tmpUndoEvent;

    if (my_frame)
    {
        if ((tmp_key == WXK_LEFT) || (tmp_key == WXK_RIGHT) ||
            (tmp_key == WXK_DOWN) || (tmp_key == WXK_UP) ||
            (tmp_key == WXK_PRIOR) || (tmp_key == WXK_NEXT) ||
            (tmp_key == WXK_HOME) || (tmp_key == WXK_END))
        {
            tmpUndoEvent.start_select = GetInsertionPoint();
            tmpUndoEvent.is_modified = IsModified();
        }
        else if (event.AltDown() == TRUE)
        {
            ;
        }
        else if (tmp_key == 13)
        {
            tmpUndoEvent.the_event = (wxKeyEvent*)event.Clone();
            tmpUndoEvent.the_event->m_keyCode = WXK_BACK;
            tmpUndoEvent.the_event->m_rawCode = WXK_BACK;
            tmpUndoEvent.the_buffer = tmp_selection;
            tmpUndoEvent.is_modified = IsModified();
        }
        else if ((tmp_key <= 0x7e) && (tmp_key >= 0x20))
        {
            tmpUndoEvent.the_event = (wxKeyEvent*)event.Clone();
            tmpUndoEvent.the_event->m_keyCode = WXK_BACK;
            tmpUndoEvent.the_event->m_rawCode = WXK_BACK;
            tmpUndoEvent.the_buffer = tmp_selection;
            tmpUndoEvent.is_modified = IsModified();
        }
        else if (tmp_key > 0x7f)
        {
            tmpUndoEvent.the_event = (wxKeyEvent*)event.Clone();
            tmpUndoEvent.the_event->m_keyCode = WXK_BACK;
            tmpUndoEvent.the_event->m_rawCode = WXK_BACK;
            tmpUndoEvent.the_buffer = tmp_selection;
            tmpUndoEvent.is_modified = IsModified();
        }
        // If this is a delete or a backspace, it is like a 'cut',
        // which creates its own undo event.
        if ((tmp_key == WXK_DELETE) || (tmp_key == WXK_BACK) && (x > 1))
        {
            tmp_should_push = false;
        }
    }

    if ((tmp_should_push == true) &&
        (my_is_undoing == false))
    {
        global_text_event_log.log_event(text_event_data::push_undo,
                                        (unsigned long)KEY_CODE(tmpUndoEvent),
                                        (unsigned long)tmpUndoEvent.the_buffer.length(),
                                        (unsigned long)tmpUndoEvent.start_select,
                                        (unsigned long)tmpUndoEvent.is_modified);

        undo_buffer::push(tmpUndoEvent);
    }
}

void
wx_text_control::handle_event(wxKeyEvent& event,long& x,long& y)
{
    wxTextCtrl::OnChar(event);

    // Get a whole bunch of information about the cursor position, selection etc.
    bool tmp_has_changed = false;
    int tmp_key = event.GetKeyCode();
    bool tmp_shift = event.ShiftDown();
    bool tmp_control = event.ControlDown();
    bool tmp_alt = event.AltDown();
    long tmp_position = GetInsertionPoint();
    long tmp_last_position = GetLastPosition();
    int tmp_chars = (int)GetLineLength(y);
    long tmp_start_selection,tmp_end_selection;
    int tmp_delta = 0;
    GetSelection(&tmp_start_selection,&tmp_end_selection);

    // Avoid a race condition during shutdown.
    if (my_frame)
    {
        // Convert between 1 index (humans) and 0 index (computers)
        ++x;++y;

        // Handle the cut/paste type of keys.
        if (tmp_control)
        {
            if (tmp_key == CONTROL_X)
            {
                Cut();
	            event.Skip(false);
                tmp_has_changed = true;
            }
            else if (tmp_key == CONTROL_V)
            {
                Paste();
	            event.Skip(false);
                tmp_has_changed = true;
            }
            else if (tmp_key == CONTROL_C)
            {
                Copy();
                event.Skip(false);
                tmp_has_changed = false;
            }
        }
        // handle the cursor movement keys.
        else if ((tmp_key == WXK_LEFT) && (x > 1))
        {
            tmp_has_changed = false;
            MOVE_CURSOR(-1);
            tmp_start_selection--;
            event.Skip(false);
            --x;
        }
        else if ((tmp_key == WXK_RIGHT) && (x <= tmp_chars))
        {
            tmp_has_changed = false;
            tmp_end_selection++;
            MOVE_CURSOR(1);
            event.Skip(false);
            ++x;
        }
        else if ((tmp_key == WXK_RIGHT) &&
                 (x > tmp_chars) &&
                 (tmp_position < tmp_last_position))
        {
            tmp_has_changed = false;
            long tmp_delta = XYToPosition(0,y + 1) - tmp_position;
            MOVE_CURSOR(tmp_delta);
            tmp_end_selection += tmp_delta;
            event.Skip(false);
            ++y;
            x = 0;
        }
        else if ((tmp_key == WXK_DOWN) &&
                 (tmp_position < tmp_last_position))

        {
            tmp_has_changed = false;

            // Note that y is not +1 since y is indexed from 1, but tmp_delta s/b
            // indexed from 0
            long tmp_delta = XYToPosition(x - 1,y) - tmp_position;
            tmp_end_selection += tmp_delta;
            MOVE_CURSOR(tmp_delta);
            event.Skip(false);
            ++y;
        }
        else if (tmp_key == WXK_END)
        {
            tmp_has_changed = false;
            x = tmp_chars;
        }
        else if (tmp_key == WXK_HOME)
        {
            tmp_has_changed = false;
            x = 1;
        }
        else if ((tmp_key == WXK_UP) && (y > 1))
        {
            tmp_has_changed = false;
            long tmp_delta = XYToPosition(x - 1,y - 2) - tmp_position;
            tmp_start_selection += tmp_delta;
            MOVE_CURSOR(tmp_delta);
            event.Skip(false);
            --y;
        }
        else if (tmp_key == 13)
        {
            WriteText(wxString((wxChar)(char)tmp_key));
            event.Skip(false);
            tmp_has_changed = true;
            ++y;x = 1;
        }
        else if (event.AltDown() == TRUE)
        {
            ;
        }
        else if ((tmp_key <= 0x7e) && (tmp_key >= 0x20))
        {
            WriteText(wxString((wxChar)(char)tmp_key));
            event.Skip(false);
            tmp_has_changed = true;
            ++x;
        }
        // Delete is like a 'cut' of the next character.  If there is a selection 
        // then we just cut that.  The data does not go on the clipboard.
        else if ((tmp_key == WXK_DELETE) && (tmp_position < tmp_last_position))
        {
            tmp_has_changed = true;
            if (tmp_start_selection == tmp_end_selection)
            {
                // Handle the linefeed difference between Linux and Win32 with this #def
#ifdef WIN32
                if (x > tmp_chars)
                {
                    SetSelection(tmp_position,tmp_position + 2);
                }
                else
#endif
                    SetSelection(tmp_position,tmp_position + 1);
            }
            Cut(false);
            event.Skip(false);
            --x;
        }
        // Backspace is like a 'cut' of the previous character.
        else if ((tmp_key == WXK_BACK) && (tmp_position > 0))
        {
            tmp_has_changed = true;
            if (tmp_start_selection == tmp_end_selection)
            {
#ifdef WIN32
                if (x == 1)
                {
                    SetSelection(tmp_position - 2,tmp_position);
                }
                else
#endif
                    SetSelection(tmp_position - 1,tmp_position);
            }
            Cut(false);
            event.Skip(false);
            --x;
        }

        // If the user is selecting...
        if ((tmp_key == WXK_LEFT) || (tmp_key == WXK_RIGHT) ||
            (tmp_key == WXK_DOWN) || (tmp_key == WXK_UP) ||
            (tmp_key == WXK_PRIOR) || (tmp_key == WXK_NEXT) ||
            (tmp_key == WXK_HOME) || (tmp_key == WXK_END))
        {
            if (tmp_shift)
            {
                SetSelection(tmp_start_selection,tmp_end_selection);
            }
        }
    }

    // If the buffer has changed, update the status bar and the 'dirty' status.
    if (tmp_has_changed)
    {
        string tmp_string = (string)"Ln: " + (string)y + (string)" Col: " + (string)x;
        my_frame->SetStatusText(B2U(tmp_string.access_char_array()),1);
        my_buffer_modify_time = time(0) + (time_t)1;
        MarkDirty();
    }
}

void
wx_text_control::OnChar(wxKeyEvent& event)
{
    global_text_event_log.log_event(text_event_data::key_press,
                                    (unsigned long)event.GetKeyCode(),
                                    (unsigned long)event.ShiftDown(),
                                    (unsigned long)event.AltDown(),
                                    (unsigned long)event.ControlDown());

    // Some key events are things like hot keys.  In this case we give the main
    // window's key handler first dibs at key events.
    if ((is_undo_event(event) == false) &&
        ((my_key_handler == 0) ||
        (my_key_handler->handle_key_event(event,my_buffer_modify_time) == false)))
    {
        // The main window did not handle the event.  Handle it here.
        long x,y;
        long tmp_point = GetInsertionPoint();

        PositionToXY(tmp_point,&x,&y);

        // Don't record an undo for this if this is an undo event.
        if (my_is_undoing == false)
        {
            push_undo_event(event,x,y);
        }
        handle_event(event,x,y);
    }
    else
    {
        if (is_undo_event(event))
        {
            Undo();
        }
        event.Skip(FALSE);
    }
}

void 
wx_text_control::Save(const string& the_filename)
{
    undo_buffer::mark_save();
    wxTextCtrl::SaveFile(B2U(the_filename.access_char_array()));
}

void
wx_text_control::pop_undo_event()
{
    bool user_cancel = false;
    if (undo_buffer::get_size() > 0)
    {
        undo_event tmp_event = undo_buffer::pop();

        global_text_event_log.log_event(text_event_data::pop_undo,
                                        (unsigned long)KEY_CODE(tmp_event),
                                        (unsigned long)tmp_event.the_buffer.length(),
                                        (unsigned long)tmp_event.start_select,
                                        (unsigned long)tmp_event.is_modified);

        if (tmp_event.is_pre_save == true)
        {
            wxMessageDialog tmp_dialog(0,_T("You are about to undo past last save; Continue?"),  _T("Warning"),wxYES_NO );
            int tmp_modal = tmp_dialog.ShowModal();

            user_cancel = (tmp_modal != wxID_YES);
        }
        if (user_cancel == false)
        {
            // We are undoing an ordinary key event.
            if (tmp_event.the_event != 0)
            {
                ProcessEvent(*tmp_event.the_event);
                if (tmp_event.the_buffer.length() != 0)
                {
                    WriteText(B2U(tmp_event.the_buffer.access_char_array()));
                }
            }
            // We are undoing a cut event
            else if (tmp_event.the_buffer.length() != 0)
            {
                // Some cut events are actually cut/replace
                // operations.  In this case there will be a range of text 
                // that we need to replace.
                if (tmp_event.end_select != 0)
                {
                    Remove(tmp_event.start_select,tmp_event.end_select);
                }
                SetInsertionPoint(tmp_event.start_select);
                WriteText(B2U(tmp_event.the_buffer.access_char_array()));
            }
            // We are undoing a paste event
            else if ((tmp_event.start_select != 0) &&
                     (tmp_event.end_select != 0))
            {
                Remove(tmp_event.start_select,tmp_event.end_select);
            }
            // We are undoing a move event of some kind.
            else
            {
                SetInsertionPoint(tmp_event.start_select);
            }
    
            // If this event was responsible for changing the buffer since
            // the last save, the buffer is now in its original state.
            if ((tmp_event.is_modified == false) &&
                (IsModified() == true))
            {
                DiscardEdits();
            }
        }
        else
        {
            // If the user elected not to undo this event, push the event back on the
            // stack.
            undo_buffer::push(tmp_event);
        }
    }
}

void 
wx_text_control::clear_undo()
{
    undo_buffer::clear();
}

point
wx_text_buffer::get_insertion_point()
{
    if (thread::this_thread() != &(thread::get_system_thread()))
    {
        throw("Thread safety violation\n");
    }
    long l = my_control->GetInsertionPoint();
    long x,y;
    my_control->PositionToXY(l,&x,&y);

    return point((int)x,(int)y);
}

void
wx_text_buffer::get_selection(long& start,long& end)
{
    my_control->GetSelection(&start,&end);
    return;
}

void wx_text_buffer::set_selection(long start,long end)
{
    my_control->SetSelection(start,end);
}

// We need to do this because preprocessor needs the
// text buffer to exist but it doesn't know about any windows
// stuff.
wx_text_buffer* the_text_buffer = 0;

template<>
text_buf*
factory<text_buf,const string&>::create(const string& the_string)
{
    if (the_text_buffer== 0)
    {
        throw("Didn't initialize text buffer, oops");
    }
    the_text_buffer->load(the_string);
    return the_text_buffer;
}

wx_text_buffer::wx_text_buffer(wxWindow& the_parent,wxFrame& the_frame,wx_key_handler& the_handler):
my_last_file_modify_time(0),my_position(0),my_length(0),my_last_redraw_time(0)
,my_has_underlying_file_changed(false),my_should_exit(false)
{
    if (thread::this_thread() != &(thread::get_system_thread()))
    {
        throw("Thread safety violation\n");
    }
    // Create the underlying object
    // NOTE:  We do _NOT_ use wxRICH here.  We handle cut/paste ourselves and the RICH
    // edit functions screw that up.
    my_control = new wx_text_control(&the_parent,-1, &the_frame,_T(""),
                         wxPoint(-1,-1), wxSize(-1,-1),
                         wxTE_MULTILINE | wxHSCROLL);
    my_control->SetEditable(true);

    // Set the parent as the key handler for key events.
    my_control->set_key_hander(the_handler);

    // Pull default registry values from the registry, or use hard-coded values
    string tmp_font_name = reg::get_default(reg::text_buffer_font_name).get_value();
    string tmp_font_size = reg::get_default(reg::text_buffer_font_size).get_value();

    // If this is Windows, we can be reasonably sure that they have Courier New
#ifdef WIN32
    if (tmp_font_name.length() == 0)
    {
        tmp_font_name = "Courier New";
    }
#endif

    // If there is no font specified, try to get the user to choose one
    if (tmp_font_name.length() == 0)
    {
        wxFontDialog tmpDialog;
        tmpDialog.Create(&the_parent);
        tmpDialog.SetTitle(B2U("Choose Font for Editor"));
        if (tmpDialog.ShowModal() == wxID_OK)
        {
            wxFont tmpNewFont = tmpDialog.GetFontData().GetChosenFont();
            tmp_font_name = tmpNewFont.GetFaceName().mb_str();
            tmp_font_size = string((long)tmpNewFont.GetPointSize());
        }
        // If the user still refuses to choose a font, choose one for him.
        else
        {
            tmp_font_name = "Courier";
        }
        reg::set_new_default_value(reg::text_buffer_font_name,tmp_font_name);
        reg::set_new_default_value(reg::text_buffer_font_size,tmp_font_size);
    }
    wxFont* tmpFont = new wxFont(tmp_font_size.as_long(),wxDEFAULT,wxNORMAL,wxNORMAL,
        false,B2U(tmp_font_name.access_char_array()));
    my_control->SetFont(*tmpFont);
    wxCaret* tmpCaret = new wxCaret(my_control,10,10);
    tmpCaret->Create(my_control,10,10);
    tmpCaret->Show();
    my_control->SetCaret(tmpCaret);
    wxCursor* tmp_cursor = new wxCursor(wxCURSOR_CROSS);
    my_control->SetCursor(*tmp_cursor);
}

void
wx_text_buffer::entry_point()
{
    while (my_should_exit == false)
    {
        if (my_current_file.length())
        {
            my_has_underlying_file_changed =
                has_underlying_file_changed(my_current_file);
        }
        thread::sleep(2000);
    }
}

wx_text_buffer::~wx_text_buffer()
{
    if (thread::this_thread() != &(thread::get_system_thread()))
    {
        throw("Thread safety violation\n");
    }
    my_should_exit = true;
    thread::sleep(2000);
    my_control->Destroy();
}

void
wx_text_buffer::load_if_newer(const string& the_string)
{
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    draw_text_buffer_command::dispatch_draw_command(tmp_window,
        the_string,*this,true);
}

void
wx_text_buffer::reset_file_modify_time()
{
    wxStructStat buf;
    int result;
    /* Get data associated with "fh": */
    result = wxStat(B2U( my_current_file.access_char_array()), &buf );

    if( result != 0 )
        ;
    else
    {
        time_t tmp_access = buf.st_mtime;
        if (tmp_access != my_last_file_modify_time)
        {
            // Only log when it changes
            global_doc_event_log.log_event(doc_event_data::file_time_reset,
                                           (unsigned long)tmp_access);
        }
        my_last_file_modify_time = tmp_access;
    }
}

bool
wx_text_buffer::has_underlying_file_changed(const string& the_string)
{
    wxStructStat buf;
    int result;
    bool tmp_rv = false;

    /* Get data associated with "fh": */
    result = wxStat( B2U(the_string.access_char_array()), &buf );

    /* Check if statistics are valid: */
    if( result != 0 )
        ;
    else
    {
        time_t tmp_access = buf.st_mtime;
        if ((the_string != my_current_file) ||
            (tmp_access > my_last_file_modify_time))
        {
            tmp_rv = true;
            my_current_file = the_string;
            global_doc_event_log.log_event(
                doc_event_data::file_modify,
                tmp_access,my_last_file_modify_time);
        }
    }
    return tmp_rv;
}

void
wx_text_buffer::draw_self(window& the_window,long the_pos)
{
    map<long,long>::iterator tmp_it = my_display_to_buffer_map.get_item(the_pos,lteq);
    if (tmp_it)
    {
        the_pos = (*tmp_it).value;
    }
    my_control->ShowPosition(the_pos);
    my_control->SetInsertionPoint(the_pos);
}

void
wx_text_buffer::draw_self(window& the_window,const string& the_string)
{
    bool tmp_newer = (my_has_underlying_file_changed ||
                      (the_string != my_current_file));
    if (tmp_newer)
    {
        my_current_file = the_string;
        my_control->LoadFile((wxString(the_string.access_char_array(),wxConvUTF8)));
        reset_file_modify_time();
        my_last_redraw_time = time(0) + 1;
    }
    my_contents = my_control->GetValue();
    my_position = 0;
    my_length = my_contents.length();
}

void
wx_text_buffer::load(const string& the_string)
{
    load_if_newer(the_string);
    int i;
    int tmp_real_char = 0;
    for (i = 0;i < (int)my_length;++i)
    {
        // if (my_contents.operator []((size_t)i) == '\n')
        if (my_contents[(size_t)i] == NEWLINE)
        {
#ifdef WIN32
            tmp_real_char++;
#endif
        }
        my_display_to_buffer_map.add_pair(i,tmp_real_char);
        tmp_real_char++;
    }
}

void
wx_text_buffer::refresh()
{
    if (thread::this_thread() != &(thread::get_system_thread()))
    {
        throw("Thread safety violation\n");
    }
    my_control->UpdateCursorPos();
    my_control->SetFocus();
}

long
wx_text_buffer::getpos()
{
    return this->my_position;
}

// METHOD: setpos
// DESCRIPTION:
// set the position of the next read to the_pos;
void
wx_text_buffer::setpos(long the_pos)
{
    my_position = the_pos;
}

// METHOD: sync_contents_to_buffer
// DESCRIPTION:
// Update the text contents with the contents of the underlying
// wxTextCtrl buffer
void
wx_text_buffer::sync_contents_to_buffer()
{
    my_last_redraw_time = my_control->get_buffer_modify_time() + 1;
    my_contents = my_control->GetValue();
    my_length = my_contents.Len();
}

void
wx_text_buffer::buffer_setpos(long l)
{
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    insert_text_buffer_command::dispatch_draw_command(tmp_window,
        l,*this,false);
}

// METHOD: getc
// DESCRIPTION:
// get the next character in the buffer.  Return text_buf::eof when the
// last position is reached.
int
wx_text_buffer::getc()
{
    // Note - no wx system calls from this so we're OK.
    if (my_has_underlying_file_changed)
    {
        load(my_current_file);
        reset_file_modify_time();
        my_last_redraw_time = my_control->get_buffer_modify_time() + 1;
    }

    if (my_length < my_position)
    {
        return text_buf::eof;
    }

    // Note on the casts:  we need to make sure the result is positive,
    // unless we are returning eof, so (char) 128, which would be -127,
    // becomes (int)128.
    int tmp_rv = (int)(unsigned char) my_contents[(size_t)my_position];
    ++my_position;
    return tmp_rv;
}

// METHOD: close
// DESCRIPTION:
// free up the resource so that it can be used by others.
void
wx_text_buffer::close()
{
    my_contents = _T("");
    my_length = 0;
    my_position = 0;
}

}
