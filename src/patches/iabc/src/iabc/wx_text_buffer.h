/*
 * wx_text_buffer.h - wxWindows implementations of abc text edit buffer.
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
#ifndef wx_text_control_h
#define wx_text_control_h
#include <wx/textctrl.h>
#include <wx/frame.h>

#include "iabc/preprocess.h"
#include "iabc/map.h"
#include "iabc/winres.h"
#include "iabc/drawtemp.h"
#include "iabc/threads.h"
#include "iabc/undo_buffer.h"

// The key-codes that we get for some events are different under
// windows vs. the GTK platforms.
#ifdef WIN32
#define CONTROL_X (0x18)
#define CONTROL_V (0x16)
#define CONTROL_C (0x03)
#else
#define CONTROL_X ('x')
#define CONTROL_V ('v')
#define CONTROL_C ('c')
#endif

namespace iabc
{
;

// CLASS: key_handler
// DESCRIPTION:
// We need a way to pass some key events to the menu frame so we can
// bind 'hotkeys' to menu items.  This abstract class allows us to do
// that.
class wx_key_handler
{
public:
    wx_key_handler(){};
    virtual ~wx_key_handler(){};

    // METHOD:
    // handle_key_event
    // returns true if the event was handled.  The modify time is unchanged if
    // the event was not handled, or if the event did not affect the buffer
    // contents.
    virtual bool handle_key_event(wxKeyEvent& event,time_t& the_last_modify_time) = 0;
};

// CLASS: wx_text_control
// DESCRIPTION:
// This is our derivation of the text control class.  Right now we just
// update the cursor position when the user presses a character control.
// Someday maybe we will do some brief-style formatting.
class wx_text_control:public wxTextCtrl
{
    // writing text inserts it at the current position, appending always
    // inserts it at the end
public:
    wx_text_control();
    wx_text_control(wxWindow *parent, wxWindowID id,wxFrame *frame,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& tsize = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr);
    void set_key_hander(wx_key_handler& the_handler){my_key_handler = &the_handler;};
    void OnChar(wxKeyEvent& event);
    void OnFocus(wxFocusEvent& event);
    void SetInsertionPoint(long pos);
    // DESCRIPTION:
    // Update the cursor position in the status bar.
    void UpdateCursorPos();
    virtual void MarkDirty();
    virtual void DiscardEdits();
    virtual bool IsModified() const {return my_is_modified;};
    void Cut(bool the_use_clipboard);
    void Cut(){Cut(true);};
    void Paste();
    void Copy();
    void Undo();
    bool LoadFile(const wxString& filename);
    void Save(const string& the_filename);
    void Replace(long from,long to,const wxString& value);
    void clear_undo();
    time_t get_buffer_modify_time() const {return my_buffer_modify_time;};
    DECLARE_CLASS(wx_text_control)
    DECLARE_EVENT_TABLE()
private:
    // METHOD: handle_event
    // DESCRIPTION:
    // Call the default handler and return
    // true if this is an event that has changed the buffer.
    void handle_event(wxKeyEvent& e,long& x,long& y);

    // METHOD: monitor_event
    // DESCRIPTION:
    // analyze this key event and return the event that 'undoes' it based on the
    // current buffer state and the key event.
    void push_undo_event(wxKeyEvent& e,long x,long y);

    // METHOD: is_undo_event
    // DESCRIPTION:
    // Return true if this is alt-backspace
    bool is_undo_event(const wxKeyEvent& event);

    // METHOD: pop_undo_event
    // DESCRIPTION:
    // pop the last event in the undo buffer and send it to the current
    // window to process.
    void pop_undo_event();

    // DESCRIPTION:
    // keep track of whether the current event that we are processing is
    // actually an undo event.
    bool my_is_undoing;

    // DESCRIPTION:
    // Keep track of the last time the buffer was modified so
    // we know when to require a redraw.
    time_t my_buffer_modify_time;

    // DESCRIPTION:
    // We need to keep our own track of when the buffer is changed for the 
    // sake of saving it.  Windows has trouble interpreting things like
    // return, change in attributes etc.
    bool my_is_modified;

    // ATTRIBUTE: my_frame
    // DESCRIPTION:
    // Lets us update the status text based on cursor position change.
    wxFrame* my_frame;

    // ATTRIBUTE: wx_key_handler*
    // DESCRIPTION:
    // give the main window first dibs on keyboard events.
    wx_key_handler* my_key_handler;
};

// CLASS: wx_text_buffer
// DESCRIPTION:
// A buffer class that implements text_buf, which has
// pretty much the same interface as FILE* stdio.  This is
// so our preprocess class can treat this like a file text
// buffer.
class wx_text_buffer:public text_buf,public thread
{
public:
    virtual void entry_point();
    friend class draw_command<wx_text_buffer,string>;
    friend class draw_command<wx_text_buffer,long>;
    wx_text_buffer(wxWindow& the_parent,wxFrame& the_frame,wx_key_handler& k);
    ~wx_text_buffer();

    wx_text_control* get_control(){return my_control;};

    void Cut() {if (my_control) my_control->Cut();};
    void Paste() {if (my_control) my_control->Paste();};
    void Copy() {if (my_control) my_control->Copy();};
    void Undo() {if (my_control) my_control->Undo();};

    // METHOD: getpos
    // DESCRIPTION:
    // Return the current position of the pointer into the text buffer.
    virtual long getpos();

    // METHOD: setpos
    // DESCRIPTION:
    // set the position of the next read to the_pos;
    virtual void setpos(long the_pos);

    // METHOD: getc
    // DESCRIPTION:
    // get the next character in the buffer.  Return text_buf::eof when the
    // last position is reached.
    virtual int getc();

    // METHOD: close
    // DESCRIPTION:
    // free up the resource so that it can be used by others.
    virtual void close();

    // METHOD: load
    // DESCRIPTION:
    // Load the contents of the file and refresh the internal string.
    void load(const string& the_string);

    // METHOD: get_filename
    // DESCRIPTION:
    // return the file name that I was opened with.
    string get_filename() const {return my_current_file;}

    // METHOD: get_last_modify_time
    // DESCRIPTION:
    // Get the last time this control was modified by the user.  We
    // return the lesser of the modify/redraw time, since what the
    // user really wants to know is: have the contents changes since
    // last I redrew everything?
    time_t get_last_modify_time() const
    {
        time_t tmp_last_modify_time = my_control->get_buffer_modify_time();
        return (tmp_last_modify_time > my_last_redraw_time ?
                tmp_last_modify_time  : my_last_redraw_time);
    }

    // METHOD: refresh
    // DESCRIPTION:
    // Update the status bar with the new position.
    void refresh();

    // METHOD: remove_ref
    // DESCRIPTION:
    // There to satisfy dispatcher
    void add_ref(){};
    void remove_ref(){};

    // METHOD: get_insertion_point
    // DESCRIPTION:
    // Return the point of the 'carat' in the text field.
    point get_insertion_point();

    // METHOD: set_selection
    // DESCRIPTION:
    // For the real GUI application, we need to get and set the selection of the
    // text buffer so that the selection stays the same after redraw.
    virtual void set_selection(long start, long end);

    // METHOD: get_selection
    // DESCRIPTION:
    // For the real GUI application, we need to get and set the selection of the
    // text buffer so that the selection stays the same after redraw.
    void get_selection(long& start,long& end);

    // METHOD: reset_modify_time
    // DESCRIPTION:
    // Set the modify time of the underlying file to match the underlying
    // file's modify time (so we don't keep reloading the file)
    void reset_file_modify_time();

    // METHOD: has_underlying_file_changed
    // DESCRIPTION:
    // Returns true if the file underlying the control
    // has been changed since my_last_modify_time.
    bool has_underlying_file_changed(const string& the_string);

    virtual void sync_contents_to_buffer();
    virtual void buffer_setpos(long l);
private:
    // METHOD: load_if_newer
    // DESCRIPTION:
    // Load the file if it has modified since we last read it in, or if
    // we never read it in yet.
    void load_if_newer(const string& the_string);

    // METHOD: draw_self
    // DESCRIPTION:
    // GUI thread method to interact with windows.
    void draw_self(window& the_window,const string& the_string);

    // METHOD: draw_self
    // DESCRIPTION:
    // GUI thread method to insert the carat in the file.
    void draw_self(window& the_window,long the_point);

    // ATTRIBUTE: my_control
    // DESCRIPTION:
    // The underlying windows object.
    wx_text_control* my_control;

    // ATTRIBUTE: my_has_underlying_file_changed
    // DESCRIPTION:
    // The scanning thread sets this to true if the underlying file
    // has changed beneath the buffer.
    bool my_has_underlying_file_changed;

    // ATTRIBUTE: my_should_exit
    // DESCRIPTION:
    // used to control the exit of the scanning thread.
    bool my_should_exit;

    // ATTRIBUTE: my_position
    // DESCRIPTION:
    // This stuff is kept track of so we can access it outside the
    // windows thread.
    long my_position;
    wxString my_contents;
    long my_length;
    string my_current_file;
    string my_file_to_load;

    // ATTRIBUTE: my_last_file_modify_time
    // DESCRIPTION:
    // The modify time of the underlying file.  Use this to keep
    // track of when we need to load the file into the buffer.
    time_t my_last_file_modify_time;

    // ATTRIBUTE: my_last_redraw_time
    // DESCRIPTION:
    // The last time we've redrawn the graphical music.
    time_t my_last_redraw_time;

    // ATTRIBUTE: display_to_buffer_map
    // DESCRIPTION:
    // Under MS windows, the control has an extra character for each
    // return character, so handle that here.
    map<long,long> my_display_to_buffer_map;
};

typedef draw_command<wx_text_buffer,string> draw_text_buffer_command;
typedef draw_command<wx_text_buffer,long>   insert_text_buffer_command;

}

#endif
