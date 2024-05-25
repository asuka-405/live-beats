/*
 * winres.h - base class for windows resources like fonts and windows
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
#ifndef iabc_winres_h
#define iabc_winres_h
#include "iabc/list.h"
#include "iabc/mutex.h"
#include "iabc/factory.h"
#include "iabc/wd_data.h"
#include "iabc/string.h"
#include "iabc/polygon.h"

namespace iabc
{
;
// FILE: winres.h
// DESCRIPTION:
// Generic window classes.  We want this to be as portable as possible
// so we make the base classes do as much of the work as possible
//
// CLASS: window_res
// DESCRIPTION:
// A generic resource associated with a windowing system.  This could
// be a font, a pen or a window itself.
class window_res
{
public:
    static window_res* get_and_add_ref(int the_id);
    // METHOD: res_add_ref
    // DESCRIPTION:
    // Keep a reference count so it gets deleted at the
    // correct time.
    void res_add_ref();

    // METHOD: res_remove_ref
    // DESCRIPTION:
    // Decrement the reference count and delete if its
    // zero
    void res_remove_ref();
    int get_id(){return my_id;};
    virtual void initialize() = 0;
protected:
    virtual ~window_res();
    window_res();
    static mutex class_mutex;
    static list<window_res*> res_list;
    int my_id;
private:
    window_res(window_res& o);
    static int class_unused_id;
    window_res& operator=(const window_res& o);
    atomic_counter my_count;
    static int class_instance_count;
};

// CLASS: caster
// DESCRIPTION:
// A template class that will cast a window resource to
// its derived type, or return null
template <class win_res_dx>
class caster
{
public:
static win_res_dx& get_resource(window_res& base)
{
    win_res_dx* tmp_rv = dynamic_cast<win_res_dx*>(&base);
    return *tmp_rv;
};
};

// CLASS: canvas
// DESCRIPTION:
// A canvas is really a window, but we use it to get around the
// circular dependency between a window and the resources that
// can draw stuff in it.  We make the assumption that it doesn't
// really make any sense to have a pen, color, whatever unless
// there's a window to go with it.
class canvas:public window_res
{
public:
protected:
    canvas(){};
    virtual ~canvas(){};
};

// CLASS: pen
// DESCRIPTION:
// A pen that can draw on a window.
class pen:public window_res
{
public:
    class pen_data
    {
    public:
        canvas* c;
        int red;
        int green;
        int blue;
        int width;
        pen_data(canvas& the_c,int the_red,int the_green,int the_blue,int the_width):
            c(&the_c),red(the_red),green(the_green),blue(the_blue),width(the_width){};
    };
    static pen* get_pen(canvas& c,int r,int g,int b,int width);
    bool operator==(const pen& o){return
        ((&o.my_canvas == &my_canvas) &&
            (o.my_red == my_red) &&
            (o.my_blue == my_blue) &&
            (o.my_green == my_green) &&
            (o.my_width == my_width));};
    bool operator!=(const pen& o){return !((*this) == o);};
    bool operator==(const pen_data& o){return
        ((o.c == &my_canvas) &&
            (o.red == my_red) &&
            (o.blue == my_blue) &&
            (o.green == my_green) &&
            (o.width == my_width));};
    bool operator!=(const pen_data& o){return !((*this) == o);};
    int red(){return my_red;}
    int green(){return my_green;}
    int blue(){return my_blue;}
    void red(int r){my_red = r;update_settings();};
    void green(int g){my_green = g;update_settings();};
    void blue(int b){my_blue = b;update_settings();};
    void width(int w){my_width = w;update_settings();};

    // METHOD: clear_all_pens_for_canvas
    // DESCRIPTION:
    // We create pens and fonts based on a particular
    // window.  Avoid memory leaks by derefing all pens still owned
    // by this canvas.  Hopefully by now any clients that keep
    // pens around have been removed.
    static void clear_all_pens_for_canvas(canvas& c);
protected:
    virtual void update_settings() = 0;
    virtual ~pen();
    pen(canvas& c,int r,int g,int b,int width);
    int my_red;
    int my_green;
    int my_blue;
    int my_width;
    canvas& my_canvas;
    static list<pen*> class_list;
    static mutex class_mutex;
    static int pen_count;
};

class wx_font_info
{
public:
    int descent;
    int leading;
    int x;
    int y;
};

typedef array<wx_font_info> font_info_array;

#if 0
class font_map_key
{
public:
    int font_family;
    int point_size;
    int style;
    bool operator==(const font_map_key& o) const{return
        (font_family == o.font_family) &&
        (point_size == o.point_size) &&
        (style == o.style);};
    bool operator>(const font_map_key& o) const{return ((font_family > o.font_family) ||
                                                        ((font_family == font_family) &&
                                                         (point_size > o.point_size)) ||
                                                        ((font_family == font_family) &&
                                                         (point_size == o.point_size) &&
                                                         (style > o.style)));};
    bool operator>=(const font_map_key& o) const{return ((*this > o) || (*this == o));};
    bool operator<(const font_map_key& o) const{return ((*this >= o) ? false : true);};
    bool operator<=(const font_map_key& o) const{return ((*this > o) ? false : true);};
};
#endif
// CLASS: font
// DESCRIPTION:
// A font class that can write text to a window
class font:public window_res
{
public:
    // CLASS: font_data
    // DESCRIPTION:
    // font_data allows you to specify a font in a generic way, and
    // then get the font for a specific system.
    typedef enum style
    {
        normal = 0,
        italic = 1,
        bold = 2,
        slanty = 4
    } style;

    typedef enum typeface
    {
        serifs,
        sans_serifs,
        fixed
    } typeface;

    class font_data
    {
    public:
        canvas* c;
        typeface family;
        int point_size;
        style my_style;
        font_data(canvas* the_c,typeface the_family,int the_point_size,font::style the_style):
            c(the_c),family(the_family),point_size(the_point_size),my_style(the_style){};

    };
    static font* get_font(canvas&,typeface family, int point_size,font::style the_style);
    static font* get_font(const font& o,int point_size,font::style the_style);

    // METHOD: get_font_from_list
    // DESCRIPTION:
    // get the font from the list of fonts that this window knows about, or return
    // NULL if this font has not been created yet.  This can be called from outside
    // the windows thread and will not switch threads.
    static font* get_font_from_list(canvas&,typeface family, int point_size,font::style the_style);

    // METHOD: get_rect
    // DESCRIPTION:
    // Return the bounding rectangle that you get if you draw the_string
    // using this font.
    virtual rect get_rect(const string& the_string,
                          int* the_descent = 0,int* the_leading = 0) = 0;

    int get_point_size() const{return my_point_size;};
    style get_style () const{return my_style;};
    typeface get_typeface () const{return my_family;};
    bool operator==(const font_data& o);
    bool operator!=(const font_data& o){return !(*this == o);};
    static void clear_all_fonts_for_canvas(canvas& c);
protected:
    virtual ~font();
    font(canvas& c,typeface family, int point_size,style the_style);
    typeface my_family;
    int my_point_size;
    canvas& my_canvas;
    style my_style;
    static list<font*> class_list;
    static mutex class_mutex;
    static int font_count;
    font_info_array my_font_array;
};

// CLASS: document
// DESCRIPTION:
// This is like the windows view/document architecture.  A document
// gives the window something to nofity when its state changes, so that
// the document can render itself in the window properly
class document
{
public:
    document():my_canvas(0){};
    virtual ~document(){};
    // METHOD: redraw
    // DESCRIPTION:
    // redraw the rectangle the_rect in the window, as this part has changed
    virtual void redraw(const rect& the_rect) = 0;

    // METHOD: click
    // DESCRIPTION:
    // someone has clicked on the area of the window that
    // contains this document.
    virtual void click(const point& the_rect){};

    // METHOD: attach_canvas
    // DESCRIPTION:
    // attach a canvas, which is really a window, to a document
    virtual void attach_canvas(canvas& the_canvas) = 0;

    // METHOD:
    // DESCRIPTION:
    // Detach a document from a window, so that we can destroy the window
    // or the document.
    virtual void detach_canvas(canvas& the_canvas) = 0;

    // METHOD: print_page
    // DESCRIPTION:
    // Print the given page
    virtual void print_page(int the_page) = 0;

    virtual bool has_page(int the_page) = 0;
protected:
    canvas* my_canvas;
};


// CLASS: window
// DESCRIPTION:
// Finally we have enough information to define a window.  Mostly a window object
// represents a real system window, and the methods transform from the logical
// space of the application to the 'real' space.  The interface also allows almost
// all drawing to occur in a generic window without any real system specific stuff.
class window:public canvas
{
public:
    // CLASS: window::linear_xform
    // DESCRIPTION:
    // A linear transform on a thing that consists of multiplying by a
    // scalar and adding a scalar offset
    class linear_xform
    {
    public:
        double m;
        double b;
        linear_xform():m(1.0),b(0.0){};
        linear_xform(double the_m,double the_b);
        linear_xform(const linear_xform& o);
        int xform_scalar(int o) const{return ((int)(m * (double)o + b));};
        int inverse_xform_scalar(int o) const{return ((int)(- b + (double)o / m));};
    };

    // CLASS: window::display_context
    // DESCRIPTION:
    // A display context is the current linear transform that operates on
    // stuff that gets drawn in the window, and the current point (the
    // turtle) that we're drawing on.
    class display_context
    {
    public:
        display_context(){};
        display_context(const linear_xform& x,
                        const linear_xform& y,
                        const point pos):x_xform(x),y_xform(y),position(pos){};
        display_context(const display_context& o):
            x_xform(o.x_xform),y_xform(o.y_xform),position(o.position){};
        display_context& operator=(const display_context& o)
            {x_xform=o.x_xform;y_xform=o.y_xform;position=o.position;return *this;};
        linear_xform x_xform;
        linear_xform y_xform;
        point position;
    };

    // METHOD: get_window
    // DESCRIPTION:
    // Each window has a name, you can get a particular window by knowning its name
    // and calling this static function.
    static window* get_window(const string& window_name);

    // METHOD: get_font
    // DESCRIPTION:
    // create or get the font that has this typeface as a family name (e.g. Times).
    font* get_font(font::typeface family, int point_size,font::style the_style)
        {return (font::get_font(*this,family,point_size,the_style));};

    // METHOD: get_pen
    // DESCRIPTION:
    // Get the pen that draws in this color and has this width and drawn
    // on this window.
    pen* get_pen(int r,int g,int b,int width)
        {return (pen::get_pen(*this,r,g,b,width));};
    void draw_rect(pen& the_pen,const rect& the_rect,bool fill=false);
    void draw_line(pen& the_pen,const line& the_line);
    void draw_text(font& the_font,const string& the_string);
    virtual void show() = 0;
    virtual void hide() = 0;

    // METHOD:
    // DESCRIPTION:
    // pixels_per_inch return the pixels per inch that can be drawn
    // on the current window.  This is used by things that render shapes
    // on the screen.
    virtual size pixels_per_inch(){return size(pixels_per_inch_x(),
                                                     pixels_per_inch_y());};
    virtual void resize(const rect& tmp_rect) = 0;

    // METHOD: refresh
    // DESCRIPTION:
    // This can be used to refresh a region; also it is called by the OS when
    // the screen is refreshed.  Otherwise known as redraw.
    void refresh(const rect& tmp_rect);
    void click(const point& p);

    // METHOD: refresh
    // DESCRIPTION:
    // Refreshes the whole screen.
    void refresh();
    void reset_zoom(){x_xform.m = 1.0;y_xform.m = 1.0;};
    void reset_offset(){x_xform.b = 0.0;y_xform.b = 0.0;};
    void set_zoom(double tmp_zoom){x_xform.m = tmp_zoom;y_xform.m = tmp_zoom;};
    void set_x_zoom(double tmp_zoom){x_xform.m = tmp_zoom;};
    void set_y_zoom(double tmp_zoom){y_xform.m = tmp_zoom;};
    void set_x_offset(double offset){x_xform.b = offset;};
    void set_y_offset(double offset){y_xform.b = offset;};
    void set_offset(double tmp_offset){x_xform.b = tmp_offset;y_xform.b = tmp_offset;};
    void push_context();
    void pop_context();
    void moveto(const point& p);
    void moveto_relative(const point& p);
    void curveto_relative(pen& the_pen,const point& cp1,const point& cp2,const point& endp);
    void set_logical_size(const size& the_size){set_raw_size(xform_point(the_size));};
    point get_position(){return my_current_pos;};
    rect xform_rect(const rect& o) const;
    point xform_point(const point& o) const;
    rect inverse_xform_rect(const rect& o) const;
    point inverse_xform_point(const point& o) const;
    void set_thumb_scroll_size(int the_size){my_thumb_scroll_size = the_size;raw_update_thumb_scroll_size();};
    virtual rect get_visible_rect(){return inverse_xform_rect(raw_get_visible());};
    virtual void render_polygon(pen& the_pen,polygon& the_polygon) = 0;

    // METHOD: add_document
    // DESCRIPTION:
    // Sets my document to the_document.  Also notify the document that its got a windoow, things
    // are gonna be different from now on...
    void add_document(document* the_document){my_document = the_document;my_document->attach_canvas(*this);};

    // METHOD: shutdown
    // DESCRIPTION:
    // Used when the document is being destroyed.  Remove my reference to it, and set my_document
    // to 0 so we ignore any pending events.
    void shutdown(){if (my_document) {my_document->detach_canvas(*this);my_document = 0;}};

    // METHOD: scale_point_size
    // DESCRIPTION:
    // Scale the point size such that it is proportional to the size of the window.  We need to do
    // this since the mapping mode we used is pixel-based.
    int scale_point_size(int the_point_size);

    // METHOD: set_center_of_y_scroll
    // DESCRIPTION:
    // make the given point the central point visible on the screen.  Make it
    // virtual but not pure virtual so printing doesn't have to fake it out.
    virtual void set_center_of_y_scroll(int the_point){};
protected:
    virtual int pixels_per_inch_x() = 0;
    virtual int pixels_per_inch_y() = 0;
    virtual void raw_draw_rect(pen& the_pen,const rect& the_rect,bool fill=false) = 0;
    virtual void raw_draw_line(pen& the_pen,const line& the_line) = 0;
    virtual void raw_draw_text(font& the_font,const string& the_string) = 0;
    virtual rect raw_get_visible() = 0;
    virtual void raw_refresh(const rect& tmp_rect) = 0;
    virtual void raw_update_thumb_scroll_size() = 0;
    virtual void set_raw_size(const size& tmp_rect) = 0;
    class window_predicate
    {
    public:
        window_predicate(const string& the_string):my_string(the_string){};
        ~window_predicate(){};
        bool operator()(window* the_window);
    private:
        string my_string;
    };
    friend class window::window_predicate;
    window(const string& name);
    virtual ~window();
    string my_name;
    mutex my_mutex;
    static mutex class_mutex;
    static list<window*> class_window_list;
    linear_xform x_xform;
    linear_xform y_xform;
    point my_current_pos;
    document* my_document;
    point my_scroll_point;
    int my_thumb_scroll_size;
    list<display_context> my_context_list;
};

}
#endif

