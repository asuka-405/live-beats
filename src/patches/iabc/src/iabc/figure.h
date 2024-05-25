#ifndef iabc_figure_h
#define iabc_figure_h
#include "iabc/factory.h"
#include "iabc/winres.h"
#include "iabc/drawtemp.h"
#include "iabc/mutex.h"
#include "iabc/event_log.h"
#include "iabc/list.h"

// Contains the generic logic for rendering figures.  
namespace iabc
{
static char* figure_op_strings[] = 
{
"add_ref",
"rescale",
"remove_ref",
"get_size_in_pixels",
"dispatch_draw_self",
"draw_self",
"get_size_from_poly",
"render_poly"
};

// CLASS: figure
// DESCRIPTION:
// Define a renderable thing.  The abstract method render_poly must be
// overridden to define the point in the polygon.  Additional
// virtual method control scaling, composite objects and other goodies.
class cloned_figure;
class figure
{
public:

    // CLASS: draw_command<figure,point>
    // This will allow the drawing of a figure to be dispatched to the 'draw_self'
    // method in the windows thread.
    friend class draw_command<figure,point>;
    friend class cloned_figure;

    // CLASS: point_figure_pair
    // DESCRIPTION:
    // This will come in handy to describe a figure and the point where it is to
    // be rendered.
    class point_figure_pair
    {
    public:
        point_figure_pair():my_figure(0){};
        point_figure_pair(const point& o,figure& f):my_point(o),my_figure(&f){};
        point_figure_pair(const point_figure_pair& o):
            my_point(o.my_point),my_figure(o.my_figure){};
        point_figure_pair& operator=(const point_figure_pair& o)
        {
            my_point = o.my_point;my_figure = o.my_figure;return *this;
        };
        point my_point;
        figure* my_figure;
    };

    typedef array<point_figure_pair> point_figure_pair_array;

    typedef enum figure_op_enum 
    {
    add_ref_enum,
    rescale_enum,
    remove_ref_enum,
    get_size_in_pixels_enum,
    dispatch_draw_self_enum,
    draw_self_enum,
    get_size_from_poly_enum,
    render_poly_enum
    };
    
    // METHOD: figure(cost size&) (constructor)
    // DESCRIPTION:
    // Since rendering in abcmacs is done in bogotwips (100*twips),
    // we need to make sure that we know how many pixels to an inch.
    // The figures work out best if we transform to screen coordinates
    // when rendering the figure rather than rendering in twips and
    // fixing it later.
    figure(const size& the_ppi);

    // METHOD: clone
    // DESCRIPTION:
    // Create a new figure based on this figure
    virtual figure* clone(const rect& r1,const rect& r2,const size& ppi);

    // METHOD: rescale
    // DESCRIPTION:
    // Reset the scale.  Since we render on a scale-specific canvas,
    // that means re-rendering the polygon.  Note that, since many figures
    // are shared between many clients, re-scaling a figure could have
    // strange results if everything else isn't rescaled, also.
    void rescale(const scale& the_scale);

    // METHOD: add/remove ref
    // DESCRIPTION:
    // Handle reference counting
    void remove_ref();
	void add_ref();

    // METHOD: get...point
    // DESCRIPTION:
    // Get the extreme points (leftmost, etc) points
    // in a figure.
    point get_leftmost_point(){render_poly();return my_leftmost_point;};
    point get_rightmost_point(){render_poly();return my_rightmost_point;};
    point get_top_point(){render_poly();return my_top_point;};
    point get_bottom_point(){render_poly();return my_bottom_point;};

    // METHOD: get_size_in_pixels
    // DESCRIPTION:
    // Returns size in pixels and ul in terms of offset from starting point
    sizeloc get_size_in_pixels();

    // METHOD: get_bounding_box
    // DESCRIPTION:
    // Return the bounding box of the rectangle if it were drawn at the
    // given point.
    rect get_bounding_box(const point& the_point = point(0,0));

    // METHOD: get_middle
    // DESCRIPTION:
    // return the middle point of the bounding box that surrounds the figure.
    point get_middle();

    // METHOD get_width/height_in_pixels
    // DESCRIPTION:
    // You can get this from the size one, but its useful by itself
    int get_width_in_pixels();
    int get_height_in_pixels();

    // METHOD: dispatch_draw_self
    // DESCRIPTION:
    // Draw the figure in the window thread at the point the_origin on the
    // window w.
    void dispatch_draw_self(window& w,const point& the_origin,bool the_should_block = false);

protected:
    // METHOD: get_extreme_point
    // DESCRIPTION:
    // Returns the topmost, leftmost, etc. actual point.  Good for
    // connecting figures together.
    void calculate_extreme_points();
    void calculate_sizeloc();

    // Once we have calculated these, we don't want to do it again if we
    // don't have to.
    point my_leftmost_point;
    point my_rightmost_point;
    point my_top_point;
    point my_bottom_point;
    sizeloc my_sizeloc;

    // METHOD: dtor
    // DESCRIPTION:
    // Remove references to contained figures.
    virtual ~figure();

    // METHOD: draw_self
    // DESCRIPTION:
    // method called by dispatcher template class.  This method
    // will always be called in the windows thread.
    void draw_self(window& w,const point& the_origin); 

    // METHOD: create_poly
    // DESCRIPTION:
    // Derivation draws the thing here.
    virtual void create_poly(polygon& poly) = 0;

    // METHOD: render_poly
    // DESCRIPTION:
    // Render the polygon, either so it can be drawn or for sizing.
    void render_poly();

    // METHOD: set_xform
    // DESCRIPTION:
    // Set the scale of the drawing surface.  Many of the drawing routines
    // we have use twips in a postscript-like coordinate system, but some
    // are in absolute pixels.  The default behavior is the postscript like
    // scaling.
    virtual void set_xform(polygon& poly);

    // ATTRIBUTE: my_ppi
    // DESCRIPTION:
    // The pixels per inch that we use to reference our points.
    size my_ppi;

    // ATTRIBUTE: my_scale
    // DESCRIPTION:
    // The scaling constant we use when deciding how big things are.
    scale my_scale;

    // ATTRIBUTE: my_poly
    // DESCRIPTION:
    // The polygon that defines the figure.
    polygon my_poly;

    // ATTRIBUTE: my_polygon_rendered
    // DESCRIPTION:
    // True if we have rendered, so we only have to do it once.
    bool my_polygon_rendered;

    // ATTRIBUTE: my_count
    // DESCRIPTION:
    // reference count, delete this when 0.
    atomic_counter my_count;

    // METHOD: set_pen_width
    // DESCRIPTION:
    // set the pen width for drawing this figure before rendering.
    virtual void set_pen_width();

    // METHOD: calculate_scaled_pen_width
    // DESCRIPTION:
    // Sometimes, especially for outlined objects, its good to get 
    // the pend width based on the scale and ppi of this object.  Do
    // that here.
    int calculate_scaled_pen_width();

    // ATTRIBUTE: my_pen_width
    // Set up the width of the pen used to draw this figure.
    int my_pen_width;
    // ATTRIBUTE: my_is_scaled
    // DESCRIPTION:
    // Indicates the pen width is scaled.  I added this when I realized
    // that cloned_figure broke my scaled_figure inheritance, but left
    // the scaled_figure class out of laziness.
    bool my_is_scaled;

    string my_debug_string;
    friend class get_extreme_point_predicate;
    friend class get_sizeloc_predicate;
};
typedef draw_command<figure,point> draw_figure_command;

// CLASS: scaled_figure
// DESCRIPTION:
// A figure that uses a pen width based on the
// current scale.
class scaled_figure:public figure
{
public:
    scaled_figure(const size& the_ppi);
    virtual ~scaled_figure(){};

    virtual void set_pen_width();
};

// CLASS: cloned_figure
// DESCRIPTION:
// A cloned figure uses the polygons from a figure class
// but maps the points to a different rectangle.  We have
// to re-render the polygon in order to handle the different
// scaling without aliasing.
class cloned_figure:public figure
{
public:
    // METHOD: create_poly
    // DESCRIPTION:
    // ctor
    cloned_figure(figure& the_model,const size& the_ppi,const rect& r1,const rect& r2);
protected:
    virtual void create_poly(polygon &poly);
    // METHOD: set_xform
    // DESCRIPTION:
    // Set the scale of the drawing surface.  Many of the drawing routines
    // we have use twips in a postscript-like coordinate system, but some
    // are in absolute pixels.  The default behavior is the postscript like
    // scaling.
    virtual void set_xform(polygon &poly);
    figure* my_model;
    rect my_model_rect;
    rect my_rect;
};
}
#endif



