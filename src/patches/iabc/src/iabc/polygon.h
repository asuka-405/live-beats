#ifndef iabc_polygon_h
#define iabc_polygon_h
#include "iabc/wd_data.h"
#include "iabc/list.h"
#include "iabc/array.h"
#include "iabc/map.h"

// CLASS - polygon
// DESCRIPTION:
// Represents an arbitrary shape that can later be
// rendered on some type of device.  Most of the operations
// that can be performed on this objects are derived from the
// postscript language (loosely).
namespace iabc
{
class arc_command;
class moveto_relative_command;
class moveto_command;
class lineto_relative_command;
class curveto_relative_command;
class polygon
{
    // CLASS: linear_xform
    // DESCRIPTION:
    // A polygon can be transformed the same way a window can.
    // When we transform a polygon we only apply the transform to
    // future drawing operations.  If you want to change the transform
    // of the whole polygon you need to apply the transform and then
    // redraw all the points
public:
    class linear_xform
    {
    public:
        double m;
        double b;
		linear_xform():m(1.0),b(0.0){};
		linear_xform(double the_m,double the_b);
		linear_xform(const linear_xform& o);
        int xform_scalar(int o) const{return ((int)(m * (double)o + b));};
    };

    // METHOD: linear operations
    // DESCRIPTION:
    // Allow the user to set up the zoom and offset in x and y directions
    double get_x_zoom(){return x_xform.m;};
    double get_y_zoom(){return y_xform.m;};
    double get_x_offset(){return x_xform.b;};
    double get_y_offset(){return x_xform.b;};
    void set_x_zoom(double tmp_zoom){x_xform.m = tmp_zoom;};
    void set_y_zoom(double tmp_zoom){y_xform.m = tmp_zoom;};
    void set_x_offset(double offset){x_xform.b = offset;};
    void set_y_offset(double offset){x_xform.b = offset;};
    point xform_point(const point& o) const;
    
    // Shapes can either be filled or outlined
    typedef enum fill_mode
    {
        fill,
        outline
    };

    // CLASS: fill_point
    // DESCRIPTION:
    // This class is used to represent a point in the drawing of the
    // polygon where the drawing actually occurs.  This lets us represent
    // more complex shapes in one polygon.
    class fill_point
    {
    public:
        // This indicates whether the polygon is filled or outlined
        fill_mode my_mode;
        // This is the index into the array of points where we fill
        // or outline the shape.  Some shapes have both outlined parts
        // and filled parts
        int my_index;
        fill_point():my_mode(fill){};
        fill_point(fill_mode the_mode,int the_index):
            my_mode(the_mode),my_index(the_index){};
        fill_point(const fill_point& o):
            my_mode(o.my_mode),my_index(o.my_index){};
        fill_point& operator=(const fill_point& o)
        {
            my_index = o.my_index;
            my_mode = o.my_mode;
            return *this;
        };
    };
    
    polygon(const point& the_starting_point);
    ~polygon();

    // Most of these behave the same way the equivalent postscript code does
    void arc(int the_angle1,int the_angle2,int r);
    void moveto_relative(const point& the_point);
    void moveto(const point& the_point);
    void lineto_relative(const point& the_point);
    void lineto(const point& the_point);
    void curveto_relative(const point& the_c1, 
                            const point& the_c2,
                            const point& the_endp);

    void curveto(const point& the_c1,
                 const point& the_c2,
                 const point& the_endp);
    // returns the size of the bounding box for this shape
    size get_size() ;

    // METHOD: get_...
    // DESCRIPTION:
    // returns the extreme points of the polygon. 
    point get_ul_offset();
    point get_top_point();
    point get_bottom_point();
    point get_left_point();
    point get_right_point();

    // A polygon is a collection of points.  Here is the size of the collection
    int get_segment_count() const{return my_segments.get_size();};

    // METHOD: operator[]
    // DESCRIPTION:
    // Allow the polygon to be treated like an array of points.
    point operator[](int the_index) const {
    if (my_segments.get_size() > the_index) 
        return my_segments[the_index];
    else 
        return point();
    };
    void remove_all_points();

    // METHOD: get_next_fill_point
    // DESCRIPTION:
    // Many figures are actually multiple sub-figures.  The sub-figures are either
    // filled or outlined at a certain index into the array.
    fill_point get_next_fill_point(const fill_point& the_last = fill_point(fill,0)) const;
    
    // METHOD: fill_poly
    // DESCRIPTION:
    // This sets the next fill point at the current index (the index that was added last).
    void fill_poly();

    // METHOD: outline_poly
    // DESCRIPTION:
    // This sets the next outline point at the current index (the index that was added last).
    void outline_poly();

    scale get_scale() const {return scale(x_xform.m,y_xform.m);};

    // METHOD: clone
    // DESCRIPTION:
    // Follow the same set of commands that was used to build 'o', when
    // creating a new polygon.  This can be used to render polys on a
    // different scale (like on a printer).
    void clone(polygon& o,const rect& r1,const rect& r2);

    class spline_control_points
    {
    public:
        spline_control_points(){};        
        spline_control_points(const point& p0,
                              const point& p1,
                              const point& p2,
                              const point& p3);
        spline_control_points(const spline_control_points& o);
        spline_control_points& operator=(const spline_control_points& o);
        point p0;
        point p1;
        point p2;
        point p3;
        scale a;
        scale b;
        scale c;
        point pt(double t) const;
    private:
        void calc_cx(){c.x =  3.0*(double)(p1.x - p0.x);};
        void calc_cy(){c.y =  3.0*(double)(p1.y - p0.y);};
        void calc_bx(){b.x = (3.0*(double)(p2.x - p1.x) - c.x);}
        void calc_by(){b.y = (3.0*(double)(p2.y - p1.y) - c.y);}
        void calc_ax(){a.x = (double)p3.x - (double)p0.x - c.x - b.x;}
        void calc_ay(){a.y = (double)p3.y - (double)p0.y - c.y - b.y;}
    };

    bool does_spline_exist_for_point(const point& p);
    spline_control_points get_spline_for_point(const point& p);
#ifdef GCC
public:
#else
private:
#endif
    // CLASS: poly_command
    // DESCRIPTION:
    // this is used by clone()
    class poly_command
    {
    public:
        poly_command(){};
        virtual void execute(polygon& p) = 0;
        // Stretch or shrink the polygon by re-drawing it on
        // a different sized rectangle.
        virtual void execute(polygon& p,const rect& r1,const rect& r2) = 0;
        virtual ~poly_command(){};
    };
    friend class arc_command;    
    friend class moveto_relative_command;
    friend class moveto_command;
    friend class lineto_relative_command;
    friend class curveto_relative_command;
    friend class fill_command;

    // METHOD: expand_points
    // DESCRIPTION:
    // handle expandable array.  We figure that this object will be rendered seldom
    // but referenced many times, so an expandable array will be best.
    void expand_points(int the_number);
    void expand_fill_points(int the_number);
    
    // METHOD: must_add_current
    // DESCRIPTION:
    // return true if we must add the current point in a lineto or 
    // curveto operation.  We need to do this is the last thing was
    // a moveto or moveto relative.
    bool must_add_current();

    // METHOD: make_midpoints
    // DESCRIPTION:
    // Implement Bezier curve by replacing a n point array with
    // n + 1 midpoints.  Return false if we can't bisect the
    // midpoints because they're too close together.
    void make_midpoints(map<double,point>& t_point_map,
                        const point& first_point,
                        double first_t,
                        const point& last_point,
                        double last_t,
                        const spline_control_points& curve);

    point my_current_point;
    array<point> my_segments;
    map<point_ordinal,spline_control_points> my_spline_points;

    array<fill_point> my_fill_points;
    int my_fill_point_count;
    int my_fill_point_size;

    // ATTRIBUTE: my_ul_offset, size, etc.
    // DESCRIPTION:
    // Keep track of the size and some special points of the
    // polygon

    point my_ul_offset;
    size my_size;
    
    point my_rightmost_point;
    point my_leftmost_point;
    point my_top_point;
    point my_bottom_point;

    // ATTRIBUTE: my_need_to...
    // DESCRIPTION:
    // Keep track of whether or not we need to recalculate things.  This speeds
    // things up when referencing the size of a polygon many times.
    bool my_need_to_size;
    bool my_need_to_offset;
    
    linear_xform x_xform;
    linear_xform y_xform;

    // This is also used by clone
    array<poly_command*> my_commands;
};

// These classes are all commands that can be executed on 
// a polygon.  They are used by clone()
class fill_command:public polygon::poly_command
{
public:
    fill_command(polygon::fill_mode the_mode):my_fill_mode(the_mode){};
    virtual void execute(polygon &p);
    virtual void execute(polygon &p, const rect &r1, const rect &r2);
    polygon::fill_mode my_fill_mode;
};
class arc_command:public polygon::poly_command
{
public:
    arc_command(int the_angle1,int the_angle2,int r):
        my_angle1(the_angle1),my_angle2(the_angle2),my_r(r)
    {};
    virtual void execute(polygon& p,const rect& r1,const rect& r2);    
    virtual void execute(polygon& my_poly) 
    {
        my_poly.arc(my_angle1,my_angle2,my_r);
    };
    ~arc_command(){};
    int my_angle1;
    int my_angle2;
    int my_r;
};

class moveto_command:public polygon::poly_command
{
public:
    virtual void execute(polygon& p,const rect& r1,const rect& r2);    
    virtual void execute(polygon& my_poly) 
    {
        my_poly.moveto(my_point);
    };
    ~moveto_command(){};
    moveto_command(const point& the_point):
        my_point(the_point){};
    point my_point;
};

class moveto_relative_command:public polygon::poly_command
{
public:
    virtual void execute(polygon& p,const rect& r1,const rect& r2);    
    virtual void execute(polygon& my_poly) 
    {
        my_poly.moveto_relative(my_point);
    };
    ~moveto_relative_command(){};
    moveto_relative_command(const point& the_point):
        my_point(the_point){};
    point my_point;
};

class lineto_relative_command:public polygon::poly_command
{
public:
    virtual void execute(polygon& p,const rect& r1,const rect& r2);    
    virtual void execute(polygon& my_poly) 
    {
        my_poly.lineto_relative(my_point);
    };
    ~lineto_relative_command(){};
    lineto_relative_command(const point& the_point):
        my_point(the_point){};
    point my_point;
};

class curveto_relative_command:public polygon::poly_command
{
public:
    virtual void execute(polygon& my_poly) {my_poly.curveto_relative(my_p1,my_p2,my_p3);};
    virtual void execute(polygon& p,const rect& r1,const rect& r2);    
    curveto_relative_command(const point& p1,
                             const point& p2,
                             const point& p3):
        my_p1(p1),my_p2(p2),my_p3(p3){};
    ~curveto_relative_command(){};
    point my_p1;
    point my_p2;
    point my_p3;
};

}
#endif


