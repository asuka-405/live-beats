#ifndef iabc_notetest_h
#define iabc_notetest_h
#include "iabc/threads.h"
#include "iabc/winres.h"
#include "iabc/wx_winres.h"
#include "iabc/window_media.h"
#include "iabc/abcrules.h"
#include "iabc/iabc_media.h"
#include "iabc/wx_text_buffer.h"
#include "iabc/wd_data.h"
#include "iabc/wx_list_box.h"
#include "iabc/midi_media.h"
#undef Yield
#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>

#ifndef WIN32
#define POINTER_CHAR ('>')
#else
#define POINTER_CHAR (char(0xbb))
#endif

namespace iabc
{
;

// FILE: iabc_media_manager
// SUBSYSTEM: main,music
// DESCRIPTION:
// This is it.  There is where the parser is created and the window
// that the parser draws on is created.  It must know about all things
// including wxWindows stuff because it creates things explicitly

// CLASS: wx_tune_list_control
// DESCRIPTION:
// Display a list of tunes that the media read in.  We need to use
// the tune box to do this.
class wx_tune_list_control:public wx_list_box_control,
    public tune_chooser
{
public:
    wx_tune_list_control(const registry_entry& the_entry,
                         wx_tree& the_box);
    void select(int the_index);
    virtual int get_choice(){set_final_value();return my_value;};
    virtual void remove_all_tunes();
private:
    void populate_list_box();
    virtual void retreive_value_from_box();
    map<wxTreeItemId,int> my_id2index;
    map<int,wxTreeItemId> my_index2id;
    int my_value;
};

// CLASS: iabc_media_manager
// DESCRIPTION:
// Allow a user to open and parse and iabc file, then display it on a window
// somewhere.  Also handles printing and midi.
class iabc_media_manager:public thread,public document,public voice_map_source
{
public:
    virtual map<int,bool> get_voice_map(){return my_voice_on_map;};

    // METHOD: click
    // DESCRIPTION:
    // someone has clicked on the area of the window that
    // contains this document.
    virtual void click(const point &the_rect);

    virtual void redraw(const rect &the_rect);

    // METHOD: ctor
    // DESCRIPTION:
    // Do many things.
    iabc_media_manager(wxWindow& the_parent,wx_tree& the_tune_list,
                       wx_view_canvas& the_display_window,
                       wx_text_buffer& the_text_buffer);

    virtual ~iabc_media_manager();

    // METHOD: entry_point
    // DESCRIPTION:
    // Handle the entry point of the thread.  The parsing and construction of the music is
    // done in this thread.
    virtual void entry_point();

    // METHOD: rescale
    // DESCRIPTION:
    // Handle where the user has chosen a different rendering scale.
    void rescale(const scale& the_scale);

    // METHOD: print
    // DESCRIPTION:
    // send it to the printer in wxWindows
    void print();

    // METHOD: attach_canvas
    // DESCRIPTION:
    // Someone has created a document and is now attaching it to a window.  Let the window know
    // that we exist and keep track of it so that we can draw on something.
    void attach_canvas(canvas &the_canvas);

    // METHO: detach_canvas
    // DESCRIPTION:
    // The document is closing or has closed.  Its no good anymore so remove all references to it.
    void detach_canvas(canvas &the_canvas);

    // METHOD: restart
    // DESCRIPTION:
    // Start the thread and parse the same file again.
    void restart();

    // DESCRIPTION:
    // Indicates that the parse is still on-going so we can't respond to the drawing commands yet.
    bool is_parsing(){return (my_is_parsing || (my_can_restart == false));};

    // DESCRIPTION:
    // Document method that returns what part of the window a page in a document occupies. Used
    // in printing.
    virtual rect get_rect_from_page(int the_page);

    // METHOD: open
    // DESCRIPTION:
    // Open the following abc file and parse it.
    void open(const string& the_file);

    // METHOD: play
    // DESCRIPTION:
    // Create a midi file based on the current tune.
    void play(const string& the_file,long the_last_cursor,const score_point& start_point);

    // METHOD: close
    // DESCRIPTION:
    // Close the file we were just looking at and free all resources.
    void close();

    // METHOD: has_page
    // DESCRIPTION:
    // Usually for printing, return this information
    virtual bool has_page(int the_page);

    // METHOD: choose_tune
    // DESCRIPTION:
    // Choose a tune from the file we have already opened.
    void choose_tune();

    // METHOD: refresh_current_tune
    // DESCRIPTION:
    // The user wants to refresh a tune that he is editing.
    // PARAMS:
    // the_last_cursor - the last cursor position.  We'd like to restore
    // this after the system has finished rendering the image so the user
    // doesn't have to scroll back on every redraw.
    void refresh_current_tune(long the_last_cursor);

    void handle_selection(selection_handler_if& the_handler);

    // METHOD: handle_tick
    // DESCRIPTION:
    // We check and update some things every so often on a tick from the
    // GUI.
    void handle_tick(int the_text_line);

    // METHOD: get_screen_pos_from_text_line
    // DESCRIPTION:
    // Get the screen position in pixels from the location of the cursor in the text buffer
    int get_text_position_from_score_point(const score_point& the_point);
    score_point get_score_point_from_screen_position(int the_screen_line);
    score_point get_score_point_from_text_position(int the_text_line);
    int get_screen_line_from_text_line(int the_text_line);
    int get_text_line_from_screen_line(int the_text_line);


    // METHOD: transpose-selection
    // DESCRIPTION:
    // transpose the selected text the given number of 1/2 steps (or letters if it is diatonic)
    // PARAMETERS:
    // the_steps - the number of steps (1/2 or diatonic) to transpose
    // the_diatonic - true if we transpose letter values only.
    // the_start - the start place of the selection in the buffer
    // the_stop  - the stop place of the selection in the buffer
    string transpose_selection(int the_start,int the_stop,const string& the_selection,int the_steps,bool the_diatonic = false);

    // METHOD: get_voice_names
    // DESCRIPTION:
    // Return the list of voice names for the GUI.
    map<int,string> get_voice_names(){return my_window_media->my_voice_name_map;};

    // METHOD: get_voice_status
    // DESCRIPTION:
    // Get the on/off voice status of the voices in the tune.
    map<int,bool> get_voice_status(){return my_voice_on_map;};

    // METHOD: set_voice_on_map
    // DESCRIPTION:
    // Set the voice map when the user has edited it from the menu.
    void set_voice_on_map(map<int,bool>& the_map);

private:
    void cleanup_print_stuff();
    void cleanup_window_stuff();

    // METHOD: print_page
    // DESCRIPTION:
    // This is something we owe document.  We assume that
    // the printing is already set up, and we just need
    // to tell the page to print itself.
    void print_page(int the_page);

    // METHOD: setup_pages
    // DESCRIPTION:
    // Set up the page settings and stuff.
    void setup_pages();
    paged_window* my_pages;
    paged_window* my_print_pages;

    scale my_page_size;
    scale my_scale;

    // METHOD: rerender
    // DESCRIPTION:
    // We are printing.  Don't read in the music again but render
    // the music again.  Make sure the scale is 1/1
    void rerender_for_print();

    // ATTRIBUTE: my_view_canvas
    // DESCRIPTION:
    // This is the wxWindow instance where the music drawing occurs.
    wx_view_canvas* my_view_canvas;

    wx_printout* my_last_printout;
    wxPrinter* my_last_printer;
    wxWindow* my_parent_window;

    // ATTRIBUTE: map<int,bool> my_voice_on_map
    // DESCRIPTION:
    // keep track of the 'voice on map' when we are refreshing
    // the tune.  This will tell the tune which voices to turn
    // on in the new tune.
    map<int,bool> my_voice_on_map;
    
    // ATTRIBUTE: my_abc_window
    // DESCRIPTION:
    // This is the iabc-defined window, which contains a view-canvas window
    // and interacts with classes like figure and text_figure.
    //
    // This is a little confusing.  wx_window is an instance of iabc::window
    // specialized to draw on a ::wxWindow object, and which contains a
    // wx_view_canvas instance, which is a specialization of wxScrolledWindow.
    wx_window* my_abc_window;

    // ATTRIBUTE: my_list_box
    // DESCRIPTION:
    // This is the actual wx object that the tune list gets drawn into.
    wx_tree* my_list_box;

    // ATTRIBUTE: my_tune_list
    // DESCRIPTION:
    // This is the gadget that we pass to the iabc_media that accepts all
    // the tunes that it creates.
    wx_tune_list_control* my_tune_list;

    // ATTRIBUTE: my_is_parsing
    // DESCRIPTION:
    // Set this to indicate that we are still parsing this file, os
    // we should not try and open a new file.
    bool my_is_parsing;

    // ATTRIBUTE: my_is_changing_tunes
    // DESCRIPTION:
    // True if we are opening a new tune in a collection, so we need to
    // set the buffer position to the start point of the new tune.
    // Otherwise we preserve the existing buffer position.
    bool my_is_changing_tunes;

    // ATTRIBUTE: my_media
    // DESCRIPTION:
    // This is where the music information gets displayed.
    window_media* my_window_media;

    // ATTRIBUTE: my_print_media
    // DESCRIPTION:
    // This is where the print information gets displayed.
    window_media* my_print_media;
    wx_printing_window* my_printing_window;
    wx_print_canvas* my_print_wxWindow;

    // ATTRIBUTE: my_source
    // DESCRIPTION:
    // The thing that parses the abc file and makes the music data structures.
    iabc_media* my_source;

    // ATTRIBUTE: my_can_restart
    // DESCRIPTION:
    // We set this to true when we have finished displaying the music and the
    // parsing thread exits, so that we cna restart.
    bool my_can_restart;

    // ATTRIBUTE: my_filename
    // DESCRIPTION:
    // This lets us tall the iabc source which file to open.
    preprocess* my_text_buffer;

    // ATTRIBUTE: my_last_text_position
    // DESCRIPTION:
    // the last positition within the text buffer that we were.  Used to
    // reposition the cursor after refreshes.  Unless we're changing tunes,
    // then we don't want to replace the cursor since the program will change
    // the cursor position.
    long my_last_text_position;

    // ATTRIBUTE: my_last_text_y
    // DESCRIPTION:
    // This is used to center the screen on a redraw around the part of the
    // tune that we were last editing.
    long my_last_text_y;

    // ATTRIBUTE: my_last_redraw_time
    // DESCRIPTION:
    // We set this every time we redraw something so we don't have to
    // redraw when nothing's changed.
    time_t my_last_redraw_time;

    semaphore my_exit_semaphore;
    // ATTRIBUTE: midi_media* my_midi_media
    // DESCRIPTION:
    // The midi output place.
    midi_media* my_midi_media;

    // ATTRIBUTE: wx_text_buffer& my_text_buffer
    // DESCRIPTION:
    // The buffer where the ABC text lives.
    wx_text_buffer& my_wx_text_buffer;

    // ATTRIBUTE: my_last_text_pointer
    // DESCRIPTION:
    // This is the last place we drew the handy text pointer.  Note that
    // this is a pointer to the GUI music that indiates where the text is,
    // not a pointer to the text window itself.
    point my_last_gui_cursor_point;

};

}
#endif



