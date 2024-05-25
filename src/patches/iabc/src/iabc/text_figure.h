#ifndef iabc_text_figure_h
#define iabc_text_figure_h
#include "iabc/winres.h"
#include "iabc/drawtemp.h"

// Prevent debug symbols from getting too long
#ifdef _DEBUG
#define point_text_pair ptp_
#define text_figure tf_
#endif
namespace iabc
{
;
// CLASS:
// text_figure
// DESCRIPTION:
// Allow a string to be drawn on the given window in the correct
// thread.  Handle the dispatching to the GUI thread and the
// correct placement
class text_figure
{
public:
    friend class draw_command<text_figure,point>;
    friend class draw_command<text_figure,rect>;
    friend class draw_command<text_figure,rect*>;

    text_figure(font::typeface the_font,
                int tmp_point_size,
                window& the_window,
                font::style the_style = font::normal);
    ~text_figure();

    text_figure* clone(window& w,const rect& r1,const rect& r2);

    // Draw the text at the given point
    void dispatch_draw_self(const string& the_string,
                       const point& the_point,bool the_blocking = false);

    // Draw the text at the given point
    void dispatch_draw_self(const point& the_point,bool the_blocking = false);

    // Draw the text centered on the given rectangle
    void dispatch_draw_self(const string& the_string,
                       const rect& the_rect,bool the_blocking = false);

    // Draw my_string centered on the given rectangle
    void dispatch_draw_self(const rect& the_rect,bool the_blocking = false);

    // This gets the extent of the rectange from the windows thread.
    void get_rect(const string& the_string,
                       rect* the_rect);

    // METHOD: get_current_rect
    // DESCRIPTION:
    // This gets the current rectangle from the current string.
    rect get_current_rect();
    int get_current_descent(){return get_descent(my_string);};
    int get_current_leading(){return get_leading(my_string);};
    rect get_rect(const string& the_string);
    int get_descent(const string& the_string);
    int get_leading(const string& the_string);

    void add_ref(){++my_counter;};
    void remove_ref();

    void set_string(const string& the_string){my_string = the_string;my_should_calculate = true;};

    // ATTRIBUTE: my_words
    // DESCRIPTION:
    // Do the same things with words.
    class point_text_pair
    {
    public:
        point my_point;
        text_figure* my_figure;
        point_text_pair():my_figure(0){};
        point_text_pair(const point& the_point,
                        text_figure* the_figure):
            my_point(the_point),my_figure(the_figure){};
        point_text_pair(const point_text_pair& o):my_figure(o.my_figure),
            my_point(o.my_point){};
        point_text_pair& operator=(const point_text_pair& o)
        {
            my_figure = o.my_figure;my_point = o.my_point;return *this;
        }
    };
    typedef array<point_text_pair> point_text_array;

private:

    font* my_font;
    void draw_self(window& w,const point& the_origin);
    void draw_self(window& w,const rect& the_origin);

    // METHOD: calculate_text_info
    // DESCRIPTION:
    // Calculate all the size info about this string in this font.
    void calculate_text_info(const string& the_string);

    // ATTRIBUTE: my_should_calculate
    // DESCRIPTION:
    // This is true if we need to call calculate text info
    bool my_should_calculate;
    string my_string;
    window& my_window;
    rect my_rect;
    int my_leading;
    int my_descent;
    atomic_counter my_counter;
};

typedef draw_command<text_figure,point> draw_text_command;
typedef draw_command<text_figure,rect> draw_text_rect;
typedef draw_command<text_figure,rect*> dispatch_get_rect;

}

#endif


