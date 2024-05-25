#ifndef iabc_slurer_h
#define iabc_slurer_h

#include "iabc/wd_data.h"
#include "iabc/map.h"
#include "iabc/figure.h"
#include "iabc/staff_info.h"

namespace iabc
{
;
typedef line slurpoint;

// CLASS: slurer
// DESCRIPTION:
// Handles all the slur points for a single staff.  This will allow it
// (in theory) to keep the slurs from running into each other.
// We discover slurs by matching open/closed parens in a LIFO way.
// Each time we get a new to point, we match it with the last 'from'
// point and add it to the map.  Then we draw the map from left to
// right.
class slurer:public figure
{
public:
    typedef enum slur_direction
    {
       up,
       down
    } slur_direction;
    class slur_info
    {
    public:
        slurpoint slur;
        // This is whether the slur goes up or down
        slur_direction direction;
    };

    slurer(const size& the_ppi,const staff_info& the_staff);
    void add_from_point(const point& from);
    void add_to_point(const point& to,slur_direction the_direction);
    int get_number_orphans();
    point get_absolute_location() {render_poly();return my_absolute_location;};
    int get_number_points() const;
protected:
    void create_poly(polygon& poly);
    virtual void set_xform(polygon& poly);

private:
    // These are all the from/to points that make up the slurs.
    map<int,slur_info> my_points;

    // We create slurs from right to left.  Before we can match a pair we
    // need to stack them up.  Sometimes slurs are from a previous line, in
    // which case they are permanent orphans.
    list<point> my_orphans;
    list<point> my_to_orphans;

    // METHOD: normalize
    // DESCRIPTION:
    // We try to normalize around 0,0.  That way we can draw the
    // slurer on the page where it starts instead of having
    // it always offset from absolute 0,0.  Makes the page logic
    // simpler since 0,0 is always on page 1.
    void normalize();
    point my_absolute_location;

    // This is how 'fat' the slur is.
    int my_depth;

    // We need to know the coordinates of the staff so we can
    // slur the right direction.
    staff_info my_staff;
};
}

#endif


