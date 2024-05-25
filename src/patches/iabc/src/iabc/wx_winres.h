/*
 * wx_winres.h - wxWindows implementations of window and associated resources.
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
#ifndef iabc_wx_winres_h
#define iabc_wx_winres_h
#include "wx/window.h"
#include "wx/scrolwin.h"
#include "wx/docview.h"
#include "iabc/winres.h"
#include "iabc/win_page.h"
#ifdef _MSC_VER
#include <windows.h>
#include "wx/msw/winundef.h"
#endif

namespace iabc
{
;
// CLASS: wx_canvas
// DESCRIPTION:
// A place holder to break the dependency between resources like wxFonts and the
// windows who love them.
class wx_canvas:public window
{
public:
    wx_canvas(const string& name):window(name){};
    virtual ~wx_canvas(){};
    virtual wxWindow* get_wxWindow() = 0;
};

// CLASS: font_creator
// DESCRIPTION:
// Create a font in the correct windows thread.
class font_creator
{
public:
    font_creator(canvas&,font::typeface family, int point_size,font::style the_style);
    // Draw my_string centered on the given rectangle
    font* dispatch_get_font();
    void add_ref(){};
    void remove_ref(){};
    friend class draw_command<font_creator,int>;
private:
    void draw_self(window& w,const int& tmp_ignore);
    font* my_font;
    font::typeface my_family;
    int my_point_size;
    canvas& my_canvas;
    font::style my_style;
};

typedef draw_command<font_creator,int> get_font_dispatcher;


// CLASS: wx_font
// DESCRIPTION:
// wxWindows implementation of the font concept.
class wx_font:public font
{
public:
    wx_font(canvas& c,font::typeface family, int point_size,font::style the_style);
    ~wx_font();
    // METHOD: get_rect
    // DESCRIPTION:
    // Return the rectangle that would contain the given string.  This
    // method can be called from any thread, since the geometry data associated
    // with a font is not stored when the font is created.
    virtual rect get_rect(const string &the_string,int* the_descent = 0,int* the_offset = 0);
    virtual void initialize() {
    }
    wxFont* get_wxFont(){return my_wxFont;};
    wxFont* my_wxFont;
};

// CLASS: wx_pen
// DESCRIPTION:
// A pen to draw on a window.
class wx_pen:public pen
{
protected:
    virtual void update_settings() {
    }
public:
    wx_pen(canvas& c,int r,int g,int b,int width);
    virtual ~wx_pen();

    virtual void initialize() {
    };
    wxPen* get_wxPen(){return hpen;};
    wxBrush* get_wxBrush(){return hbrush;};
    wxPen* hpen;
    wxBrush* hbrush;
};

class wx_window;

// CLASS: wx_view_canvas
// DESCRIPTION:
// We need to derive from the wxWindow object to handle the events like
// scrolling and screen updates, event though we have our own window object
// that does all the real window stuff.
class wx_view_canvas:public wxScrolledWindow
{
public:
    wx_view_canvas(wxWindow *frame,
                   const wxPoint& pos = wxPoint(0,0),
                   const wxSize&  sz = wxSize(-1,-1),
                   const long style = 0);
    ~wx_view_canvas();
    virtual void OnDraw(wxDC& dc);
    void set_window(window* the_window);
    bool ProcessEvent(wxEvent &event) ;
    int GetX(){return my_x_position;};
    int GetY(){return my_y_position;};
    void OnSize(wxSizeEvent& the_event);
    void OnLeftDown(wxMouseEvent& the_event);
    void OnScrollEvent(wxScrollWinEvent& event);
    void set_thumb_scroll_size(int the_size);
    void SetRawSize(const point& the_point);
    rect RawGetVisible();

    bool get_and_reset_my_should_refresh();
    DECLARE_EVENT_TABLE();
private:
    void CalculateScrollSize(const size& the_window_size);
    // Sometimes we scroll by more lines than the user actually moved.
    void compute_scroll_position(int& the_new_position,wxEventType the_type,
                                 int the_orient,int the_old_position,
                               int the_max,int the_screen_size);
    bool should_redraw(int the_delta,wxEventType the_event) const;
    int my_thumb_scroll_size;
    int my_y_position;
    int my_x_position;
    point my_lr;
    bool my_should_draw;
    bool my_should_refresh;
    window* my_window;
};

// CLASS: wx_dc
// DESCRIPTION:
// Proxy for the wxWindows device context, which needs to
// be established whenever we draw something on the screen
// or print something.
class wx_dc
{
public:
    wx_dc(wxWindow& the_window);
    wx_dc(wxPrintout& the_printout);
    ~wx_dc();
    operator wxDC&();
    wxDC& operator*();
    wx_dc& operator=(const wx_dc& o);
    wx_dc(const wx_dc& o);
private:
    wxPrintout* my_print;
    wxWindow* my_window;
    wxDC* my_dc;
};

// CLASS: wx_window
// DESCRIPTION:
// This is the actual window that we draw on.  It also acts
// as the event source for events and dispatches.
class wx_window:public wx_canvas
{
public:
    // METHOD: wx_window
    // DESCRIPTION:
    // Constructor.  Give wx_window its drawing area.
    wx_window(const string& the_string,wxWindow& the_window);

    // METHOD: show()
    // DESCRIPTION:
    // show/hide the window.
    virtual void show();
    virtual void hide();

    // Accessor methods.

    static window* get_window(wxWindow* the_handle);
    virtual void raw_update_thumb_scroll_size(){};

    // This is a request from the program to resize the window (which we currently
    // don't do.)  Not a notification that resize has occured.
    virtual void resize(const rect &tmp_rect) {
    };

    // METHOD: pixels_per_inch_x/y
    // DESCRIPTION:
    // Gets the window dimensions.
    virtual int pixels_per_inch_x();
    virtual int pixels_per_inch_y();
    wxWindow* get_wxWindow() {return my_wxWindow;};



    // METHOD: get_dc
    // DESCRIPTION:
    // Get the DC that we need to draw on.  This allows us to handle the DC where we
    // are printing and the one where we're not.  This needs to be public as fonts and
    // pens need to use the DC as well.
    virtual wx_dc get_dc();

protected:
    // METHOD: dtor
    // DESCRIPTION:
    // The reference count has expired us and we are gone.
    virtual ~wx_window();

    // METHOD: set_pens
    // DESCRIPTION:
    // Sets up the DC to draw with the selected pens.
    void set_pens(wxDC& the_dc,pen& the_pen,bool fill);

    // METHOD: set_raw_size
    // DESCRIPTION:
    // The contents of the document have changed such that the size of the virtual
    // window needs to be updated.
    virtual void set_raw_size(const size& tmp_size){};

    // METHOD: raw_draw_rect
    // DESCRIPTION:
    // Draw a rectangle on the screen with the given pen.
    virtual void raw_draw_rect(pen &the_pen, const rect &the_rect, bool fill=false);

    // METHOD: raw_draw_line
    // DESCRIPTION:
    // Draw a line.
    virtual void raw_draw_line(pen &the_pen, const line &the_line);

    // METHOD: raw_draw_text
    // DESCRIPTION:
    // Draw a string in the current location.
    virtual void raw_draw_text(font &the_font, const string &the_string);

    // DESCRIPTION: render_polygon
    // METHOD:
    // Draw the polygon in the current location.
    virtual void render_polygon(pen& the_pen,polygon& the_polygon);

    // METHOD: raw_refresh
    // DESCRIPTION:
    // Redraw the window in the given rectangle.
    virtual void raw_refresh(const rect &tmp_rect);

    // METHOD: raw_get_visible
    // DESCRIPTION:
    // return the absolute visible rectangle.
    virtual rect raw_get_visible(){return rect();};
protected:
    // METHOD: initialize
    // DESCRIPTION:
    // not used.
    virtual void initialize() {};

    // METHOD: get_drawable_rect
    // DESCRIPTION:
    // Determine whether or not we are printing and get either the
    // visible or printable area.
    virtual rect get_drawable_rect();

    // ATTRIBUTE: my_ppi
    // DESCRIPTION:
    // Store the ppi so we don't have to re-render it all the time
    size my_ppi;
    bool my_needs_ppi;
    wxWindow* my_wxWindow;
    wx_pen* last_pen;
    array<wxPoint> my_poly_points;
};

// CLASS: wx_drawing_window
// DESCRIPTION:
// This is a class that is used for drawing on the screen, in that it
// can handle scrolling.  It needs a specailly derived instance of
// wxWindow for scrolling and handling scroll events.
class wx_scrolling_window:public wx_window
{
public:
    wx_scrolling_window(const string& the_title,wx_view_canvas& my_canvas);
    virtual ~wx_scrolling_window();

    // METHOD: set_scroll_point
    // DESCRIPTION:
    // Move the scroll thing so tha the given position is where the
    // top of the scroll point is.
    void set_scroll_point(const point& the_point);

    // METHOD: raw_update_thumb_scroll_size
    // DESCRIPTION:
    // The document calls this when the contents of the document have changed such that
    // the scroll bars need to be re-sized.
    virtual void raw_update_thumb_scroll_size();

    // METHOD: raw_refresh
    // DESCRIPTION:
    // Redraw the window in the given rectangle.
    // virtual void raw_refresh(const rect &tmp_rect);

    // METHOD: initialize
    // DESCRIPTION:
    // not used.
    virtual void initialize() {};

    virtual void set_raw_size(const size& tmp_size);

    // METHOD: get_dc
    // DESCRIPTION:
    // Get the DC that we need to draw on.  This allows us to handle the DC where we
    // are printing and the one where we're not.  This needs to be public as fonts and
    // pens need to use the DC as well.
    wx_dc get_dc();

    virtual void set_center_of_y_scroll(int the_line);
    virtual rect raw_get_visible();
private:
    wx_view_canvas* my_view;
};

class wx_print_canvas:public wxWindow
{
public:
    wx_print_canvas(wxWindow *frame,
                   const wxPoint& pos = wxPoint(0,0),
                   const wxSize&  sz = wxSize(-1,-1),
                   const long style = 0);
    ~wx_print_canvas();
    bool ProcessEvent(wxEvent &event) ;
};

// CLASS: wx_printing_window
// DESCRIPTION:
// Like the scrolling window, but does not accept
// callbacks since it doesn't get redrawn.
class wx_printing_window:public wx_window
{
public:
    wx_printing_window(const string& the_title,wx_print_canvas& the_canvas);
    virtual ~wx_printing_window();

    // METHOD: get_dc
    // DESCRIPTION:
    // Get the DC that we need to draw on.  This allows us to handle the DC where we
    // are printing and the one where we're not.  This needs to be public as fonts and
    // pens need to use the DC as well.
    virtual wx_dc get_dc();

    // METHOD: prepare_to_print
    // DESCRIPTION:
    // This is set when we are prepared to print.
    void start_printing();

    // METHOD: start_printing
    // DESCRIPTION:
    // set the print object to be the_printout and defer rendering for
    // awhile until the pages have all been prepared.
    void set_printout(wxPrintout& the_printout)
    {my_printout = &the_printout;};

    // METHOD: end_printing
    // DESCRIPTION:
    // Set the printout object to NULL and do all future rendering on the
    // screen DC.
    void end_printing();

    //
    virtual rect get_drawable_rect();

    // METHOD: print_page
    // DESCRIPTION:
    // Get the rectangle that describes the supplied page, and redraw that page.
    void print_page(int the_page);

protected:
    virtual void raw_draw_line(pen &the_pen, const line &the_line) {
    }
    // METHOD: raw_get_visible
    // DESCRIPTION:
    // return the absolute visible rectangle.
    virtual rect raw_get_visible() {return my_printing_rect;
    }
    // ATTRIBUTE: wxPrintout*
    // DESCRIPTION:
    // The printout object that we will be printing on if we will
    // be printing on something.
    wxPrintout* my_printout;

    // ATTRIBUTE: my_printing_rect
    // DESCRIPTION:
    // If my_printing rect has some area to it, accept drawing
    // commands over that area.
    rect my_printing_rect;
};

// CLASS: wx_printout
// DESCRIPTION:
// Derive from the wxPrintout and do the stuff we need
// to.
class wx_printout:public wxPrintout
{
public:
    wx_printout(document& the_doc,wx_printing_window& the_window);
    virtual bool HasPage(int page);
    virtual bool OnPrintPage(int page);
    virtual void GetPageInfo(int* the_min,int* the_max,int* the_from,int* the_to);
    virtual void OnEndPrinting();
    virtual void OnBeginPrinting();
private:
    wx_printing_window* my_window;
    document* my_doc;
};


}
#endif


