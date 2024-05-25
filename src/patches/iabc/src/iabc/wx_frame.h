/*
 * wx_frame.h - Main frame window and event handler.
 * Copyright (C) 2002 Aaron Nemwan
 * e-mail: aaron@meet-the-newmans.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <wx/wx.h>
#include "iabc/wx_winres.h"
#include "iabc/wx_dispatch.h"
#include "iabc/iabc_media_manager.h"
#include "iabc/string.h"
#include "iabc/iabc_preferences.h"
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include "iabc/wx_text_buffer.h"
#include "iabc/registry.h"
#include "iabc/registry_defaults.h"
#include "iabc/wx_list_box.h"
#include "iabc/figure.h"
#include "main/system_dirs.h"
#include "main/wx24_resources.h"
#include "iabc/map.h"
#include "iabc/wx_splitter.h"
#ifdef GCC
#include <unistd.h>
#define MAX_PATH FILENAME_MAX
#else
#include <process.h>
#endif

#define TIMER_ID 0x1a51

namespace iabc
{
extern iabc::wx_text_buffer* the_text_buffer;

class wx_view_canvas;

class wx_view_frame;
class wx_frame_splitter_notifier:public wx_splitter_notifier
{
public:
    wx_frame_splitter_notifier(wx_view_frame* the_frame,
                               wx_splitter_notifier::sash_direction the_direction):
        my_frame(the_frame),my_direction(the_direction){};
    wx_view_frame* my_frame;
    wx_splitter_notifier::sash_direction my_direction;
    virtual void notify_sash_change(double d);
};

// CLASS: wx_view_frame
// DESCRIPTION:
// This is the main application window.  All the events that
// can happen in iabc come through this class.
class wx_view_frame: public wxFrame
{
    DECLARE_CLASS(wx_view_frame)
public:
    // METHOD: wx_view_frame
    // DESCRIPTION:
    // The main window is divided up into 3 panes, for the music, the tune
    // list and the abc text.  This mostly created those and also creates
    // the main sub objects and timers that define the application.
    wx_view_frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~wx_view_frame();

    // METHOD: init
    // DESCRIPTION:
    // Make debuggin a little easier by putting the init stuff into a different function.
    void init();

    // METHOD: OnAbout
    // DESCRIPTION:
    // Prints the version.
    void OnAbout(wxCommandEvent& event);

    // METHOD: OnDumpLog
    // DESCRIPTION:
    // We provide a menu option to allow the user to dump the event log to
    // a file.  This helps me debug problems that occur.
    void OnDumpLog(wxCommandEvent& event);

    // METHOD: OnHelp
    // DESCRIPTION:
    // This will attempt to start a web browser that points to the
    // included documentation.  But the online docs are probably better.
    void OnHelp(wxCommandEvent& event);

    // METHOD: OnNew
    // DESCRIPTION:
    // The user wants a clean document.  Handle save issues and create a 
    // generic tune.
    void OnNew(wxCommandEvent& event);

    // METHOD: OnOpen
    // DESCRIPTION:
    // Open a new file.  gets the file name from the user and sends the
    // parameter to the iabc_media_manager to handle.
    void OnOpen(wxCommandEvent& event);
    
    // METHOD: OnScoreMode
    // DESCRIPTION:
    // The user has requested to display the music in 'score' mode.
    void OnScoreMode(wxCommandEvent& event);

    // METHOD: OnViewPreferences
    // DESCRIPTION:
    // Display the 'view preferences' dialog that allows the user to
    // set a variety of display parameters.
    void OnViewPreferences(wxCommandEvent& event);

    // METHOD: OnABCPreferences
    // DESCRIPTION:
    // Allow the user to set up some parameters regarding the ABC language and
    // how the parser works.
    void OnABCPreferences(wxCommandEvent& event);

    // METHOD: OnViewPreferences
    // DESCRIPTION:
    // Display a dialog for changing the editor font.
    void OnChangeEditorFont(wxCommandEvent& event);

    // METHOD: OnSave
    // DESCRIPTION:
    // Save the current document.
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);

    // METHOD: ProcessEvent
    // DESCRIPTION:
    // iabc dispatch model uses the wxWindows event pump.  This is the
    // system entry points for events that are dispatched to the windows
    // thread or other threads.
    bool ProcessEvent(wxEvent &event);

    // METHOD: OnRefresh
    // DESCRIPTION:
    // The user has pressed the ctrl-L combination.  This will call the function
    // in iabc_media_manager that re-parses the file and refreshes the tune.
    // Also save the current cursor position so that the user is in the same
    // place after the refresh.
    void OnRefresh(wxCommandEvent& event);

    // METHOD: OnPlay
    // DESCRIPTION:
    // Play zha midi, ja.
    void OnPlay(wxCommandEvent& event);

    // METHOD: OnSetPlayer
    // DESCRIPTION:
    // Handle the logic to set the midi player, for the OnPlay command.
    void OnSetPlayer(wxCommandEvent& event);

    // METHOD: OnExport
    // DESCRIPTION:
    // Allow the user to export the document to midi.
    void OnExport(wxCommandEvent& event);

    // METHOD: OnPrint
    // DESCRIPTION:
    // Print the current tune using OS-specific methods.
    void OnPrint(wxCommandEvent& event);

    // METHOD: OnCut
    // DESCRIPTION:
    // Handle the Ctrl-x character sequence.  This gets sent to the
    // text buffer window.
    void OnCut(wxCommandEvent& event);
    
    // METHOD: OnCopy
    // DESCRIPTION:
    // Handle the Ctrl-c character sequence.  This gets sent to the
    // text buffer window.
    void OnCopy(wxCommandEvent& event);

    // METHOD: OnPaste
    // DESCRIPTION:
    // handle the Ctrl-v character sequence.  This gets sent to the 
    // text buffer window.
    void OnPaste(wxCommandEvent& event);

    // METHOD: OnUndo
    // DESCRIPTION:
    // Handle the 'undo' command - poorly.  This needs some work.
    void OnUndo(wxCommandEvent& event);

    // METHOD: OnCloseWindow
    // DESCRIPTION:
    // The big 'X' has been clicked, or exit from the menu.
    void OnCloseWindow(wxCloseEvent& the_event);
    void OnCloseMenu(wxCommandEvent& the_event);

    // METHOD: OnWindowCreate
    // DESCRIPTION:
    // This is called one time during creation, we just use the
    // generic windows code.
    void OnWindowCreate(wxWindowCreateEvent& the_event);

    // METHOD: OnSize
    // DESCRIPTION:
    // Recalculate the sizes of the window parts.  Someday I'd
    // like to allow the user to undock different parts of the 
    // window.
    void OnSize(wxSizeEvent& the_event);

    // METHOD: OnTick
    // DESCRIPTION:
    // We try to keep the music and the text in sync as the user
    // is typing.  Every tick we check for changed text to move the
    // cursor and do some other periodic things.
    void OnTick(wxTimerEvent& the_event);

    // METHOD: OnTranspose
    // DESCRIPTION:
    // The user has selected some text and chosen to transpose it.  Call
    // the code that handles this.
    void OnTranspose(wxCommandEvent& event);

    // METHOD: OnVoice
    // DESCRIPTION:
    // Setup voice options.  The dialog box doesn't do anything yet but this
    // will let the user show/hide voices.
    void OnVoice(wxCommandEvent& event);

    // METHOD: OnShowTuneList
    // DESCRIPTION:
    // Show or hide the window pane with the tune list in it.
    void OnShowTuneList(wxCommandEvent& event);

    // METHOD: really_exit
    // DESCRIPTION:
    // Confirm save before exit.
    bool really_exit();

    // METHOD: handle_key_event
    // DESCRIPTION:
    // Some key events that get received in the key area don't get handled
    // by the text buffer object, but have a global effect and must be
    // handled here.
    bool handle_key_event(wxKeyEvent& event,time_t& the_modify_time);

    // METHOD: get_current_line_position
    // DESCRIPTION:
    // Returns the point in the text buffer where the cursor is.
    int get_current_line_position();

    void change_sash_position(double the_new_value,wx_splitter_notifier::sash_direction the_direction);
    void commit_sash_position_change();

    // NOTE: a side-effect of this is that all future declarations are protected
    DECLARE_EVENT_TABLE()


    double my_horizontal_sash_stored;
    double my_horizontal_sash_current;
    double my_vertical_sash_stored;
    double my_vertical_sash_current;

    // METHOD: do_open
    // DESCRIPTION:
    // Documents can be opened a couple of different ways, this performs
    // the actual open.
    void do_open(const string& filename);

    // METHOD: do_close
    // DESCRIPTION:
    // remove the object and close the window.
    void do_close(bool the_close_source);

    // METHOD: get_midi_command_string
    // DESCRIPTION:
    // This constructs the command line string that we use to 
    // launch the midi player.
    string get_midi_command_string(const string& the_player,
                                   const string& the_command_line,
                                   const string& the_midi_file);

    // METHOD: get_random_filename
    // DESCRIPTION:
    // Utility function to generate a random file name.
    string get_random_filename(const string& the_directory);

    // METHOD: create_menu
    // DESCRIPTION:
    // create the main window's menu bar.
    void create_menu();

    // ATTRIBUTE: my_handle_size
    // DESCRIPTION:
    // true if we are initialized enough to handle window events.
    bool my_handle_size;

    // ATTRIBUTE: my_media
    // DESCRIPTION:
    // This is where all the music-rendering and parsing action 
    // goes on.
    iabc_media_manager* my_media;

    // ATTRIBUTE:
    // my_timer
    // DESCRIPTION:
    // handle the 500ms tick.
    wxTimer* my_timer;
    string my_last_file;
    wxStatusBar* my_status;
    wxSplitterWindow* my_hsplitter;
    wxSplitterWindow* my_vsplitter;
    wx_text_buffer* my_text_buffer;
    wx_view_canvas* my_view_canvas;
    wx_tree* my_list_box;
    wx_window* my_event_window;
    wxMenuItem* my_transpose_item;
    wxMenuItem* my_transpose_up1_item;
    wxMenuItem* my_transpose_down1_item;
    wxMenuItem* my_transpose_up12_item;
    wxMenuItem* my_transpose_down12_item;
    wxMenuItem* my_transpose_last_item;
    wxMenuItem* my_tune_list_item;
    array<string> my_temp_files;
};


#define DOCVIEW_ABOUT   2

#define ON_REFRESH (wxID_HIGHEST + 1)
#define ON_EXPORT  (ON_REFRESH + 1)
#define ON_PLAY   (ON_EXPORT + 1)
#define ON_PLAY_FROM_CURSOR   (ON_PLAY + 1)
#define ON_SET_PLAYER   (ON_PLAY_FROM_CURSOR + 1)
#define ON_VIEW_PREFERENCES (ON_SET_PLAYER + 1)
#define ON_ABC_PREFERENCES  (ON_VIEW_PREFERENCES + 1)
#define ON_SCORE_MODE  (ON_ABC_PREFERENCES + 1)
#define ON_CUT  (ON_SCORE_MODE + 1)
#define ON_COPY  (ON_CUT + 1)
#define ON_PASTE  (ON_COPY + 1)
#define ON_TRANSPOSE_CHROMATIC  (ON_PASTE + 1)
#define ON_UNDO  (ON_TRANSPOSE_CHROMATIC + 1)
#define ON_TRANSPOSE_REPEAT  (ON_UNDO + 1)
#define ON_TRANSPOSE_UP1  (ON_TRANSPOSE_REPEAT + 1)
#define ON_TRANSPOSE_DOWN1  (ON_TRANSPOSE_UP1 + 1)
#define ON_TRANSPOSE_UP12  (ON_TRANSPOSE_DOWN1 + 1)
#define ON_TRANSPOSE_DOWN12  (ON_TRANSPOSE_UP12 + 1)
#define ON_VOICE  (ON_TRANSPOSE_DOWN12 + 1)
#define ON_DUMP_LOG (ON_VOICE + 1)
#define ON_SHOW_TUNE_LIST (ON_DUMP_LOG + 1)
#define ON_CHANGE_EDITOR_FONT  (ON_SHOW_TUNE_LIST + 1)
}
