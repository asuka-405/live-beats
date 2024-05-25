/*
 * beamer.h - Beams the notes in abc.
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
#ifndef iabc_beamer_h
#define iabc_beamer_h
#include "iabc/figure.h"
#include "iabc/pitch.h"
#include "iabc/note_info.h"
#include "iabc/staff_info.h"
#include "iabc/music_info.h"
#include "iabc/window_figure.h"
#include "iabc/gm.h"
#include "iabc/win_page.h"

namespace iabc
{
;
// FILE: class that handle beaming notes
// SUBSYSTEM: rendering
//
// CLASS: beamer
// DESCRIPTION:
// Beamer draws beams and flags on notes.  A note tells the
// beamer its coordinates as it is drawn, and the beamer computes
// the coordinates and shape of the beam and stem.
class beamer:public scaled_figure
{
    // derivation draws the thing here.
public:
    // METHOD: beamer (constructor)
    // DESCRIPTION:
    // Call the base class figure constructor with the screen dimensions.
    beamer(const size& the_ppi,staff_info the_staff,bool the_is_grace);
    virtual ~beamer();
    
    // METHOD: clone
    // DESCRIPTION:
    // Since we beam according to note heads, we can't just map our points
    // to another space; we need to actually clone the note heads first.
    //  virtual figure* clone(const rect& r1,const rect& r2,const size& ppi);
    
    // METHOD: beam_me
    // ARGS: const point& - the new point to draw the beam around
    //       figure& - the figure of the note head, needed so we can draw the
    //                 beam in the right place whether we draw up or down.
    //       const fraction& - the duration of the note, needed so we can draw the
    //                 right kind of beam.
    // DESCRIPTION:
    // When a note figure knows its coordinates, it calls this method to tell
    // the beamer to add this note head to the beam group.  If there is no beam
    // group then this note receives a stem but no beam.
    void beam_me(const point& the_new,figure& the_note_head,const fraction& the_duration,
                 int the_xlet_value);

    // METHOD: get_absolute_ul
    // DESCRIPTION:
    // Unlike other figures, beamers know where they're really supposed to
    // go.  Return that absolute point, and everything else is relative
    // to that.  This makes it easier to move around
    point get_absolute_ul(){return my_absolute_ul;};

    // METHOD: get_xlet_points
    // DESCRIPTION:
    // Get the points where the little xlet descriptions are supposed to go.
    array<point> get_xlet_points();
    array<int> get_xlet_values(){return my_xlet_values;};

    // METHOD: get_upness_value/high_poitn/low_point/tail_width
    // DESCRIPTION:
    // Get some geometry information about the figure so that we might draw it 
    // correctly.
    int get_upness_value(){return my_upness_values;};
    int get_high_point(){return my_high_point;};
    int get_low_point(){return my_low_point;};
    int get_tail_width(const chord_info& the_info) const;

    // METHOD: force_direction
    // DESCRIPTION:
    // For some notes, like with SATB parts, we like to force the beam to go a certain
    // way.
    void force_direction(int the_direction) {my_forced_upness = the_direction;};
    
    // METHOD: should_beam
    // ARGS:  const fraction& - the value to check
    //        const fraction& - the level of the beam
    // DESCRIPTION:
    // Return true if the value is either less than the level of the beam,
    // or is a dotted level of the beam.
    bool should_beam(const fraction& the_value,const fraction& the_base);

protected:
    // METHOD: create_poly
    // DESCRIPTION:
    // The figure virtual method.  The window wants to draw the beam so its time
    // to render the shape.
    virtual void create_poly(polygon &poly);

    // METHOD: create_xlet
    // DESCRIPTION:
    // Draw the little number and the bracket associated with triplets and 
    // other xlets.
    void create_xlet();

    // METHOD: is_dotted
    // DESCRIPTION:
    // return true if this value is a dotted version of the_base.  
    // These have different flags than triplety things.
    bool is_dotted(const fraction& the_value,const fraction& the_base);

    // METHOD:
    // DESCRIPTION:
    // The figure virtual method.
    // Most figures use a bogotwip coordinate system based on PostScript.  But this
    // shape is drawn to absolute screen coordinates, so set the linear xform
    // for this polygon to 1:1.
    virtual void set_xform(polygon& poly);

private:
    // METHOD: point get_highest_point()
    // DESCRIPTION:
    // Get the highest connection point in our beam group.
    point get_highest_point();

    // METHOD: get_lowest_point
    // DESCRIPTION:
    // Get the lowest connection point in our beam group.
    point get_lowest_point();

    // Get the first point in the group
    point get_first_point(){if (my_beam_points.get_size() > 0) return my_beam_points[0]; else return point();};

    // Get the last point in the group
    point get_last_point(){if (my_beam_points.get_size() > 0) return my_beam_points[my_beam_points.get_size() - 1]; else return point();};
    
    // average all the points in the beam group.
    point get_average();

    // If we need to beam these guys, create that
    void create_beam();

    // METHOD: draw_beam_segment
    // DESCRIPTION:
    // A helper function to create_beam, that actually draws the beam and makes
    // create_beam a little less complicated.  The draw_half variable indicates
    // whether we draw the segment all the way to p2, or only 1/2 way.  the_left
    // says we are drawing from left to right.
    //  
    //     --------                  ---------
    //     |--     |                 |    ----|
    //     |       |                 |        |
    //     |       |                 |        |
    //    0       0                  0        0
    //    half = true, left = true  half = false, left = false 
    void draw_beam_segment(const point& the_p1,const point& the_p2,
                           bool the_draw_half,bool the_left);

    // METHOD: create_tail
    // DESCRIPTION:
    // If we need to put a tail on a single unbeamed 8th note,
    // do that
    void create_tail();

    // METHOD: create_head_offset
    // DESCRIPTION:
    // Offset the connection points depending on whether the
    // stem goes up or down.
    void create_head_offset();

    // METHOD: set_hi_low
    // DESCRIPTION:
    // figure out where the high and low points are.
    void set_hi_low(int the_hi,int the_lo);

    // METHOD: create_stems
    // DESCRIPTION:
    // Create the stem going either to the beam or just up.
    void create_stems();

    class get_highest_point_pred
    {
    public:
		get_highest_point_pred(const point& the_first):my_highest(the_first){};
        void operator()(const point& the_point)
        {
            if (the_point.y < my_highest.y)
            {
                my_highest = the_point;
            }
        }
        point my_highest;
    };

    class get_lowest_point_pred
    {
    public:
		get_lowest_point_pred(const point& the_first):my_lowest(the_first){};
        void operator()(const point& the_point)
        {
            if (the_point.y > my_lowest.y)
            {
                my_lowest = the_point;
            }
        }
        point my_lowest;
    };

    // We use these predicates if we are sloping from the first
    // note to the last note in a beam
    class multiply_pred
    {
    public:
        multiply_pred(double the_num,point start_x):
            my_num(the_num),my_start(start_x){};
        void operator()(point& the_point)
        {
            the_point.y = my_start.y + 
                (int)
				(((double)the_point.x - (double)my_start.x) * my_num); 
        };
        double my_num;
        point my_start;
    };

    class add_int_pred
    {
    public:
        add_int_pred(int the_num):my_num(the_num){};
        int my_num;
        void operator()(point& tmp_point){tmp_point.y+=my_num;};
    };

    class set_absolute_height_pred
    {
    public:
        set_absolute_height_pred(int the_int):my_int(the_int){};
        void operator()(point& the_point){the_point.y = my_int; };
        int my_int;
    };

    // ATTRIBUTE: int my_depth
    // DESCRIPTION:
    // The depth in pixels of the beaming figure between note heads.
    int my_depth;
    
    // ATTRIBUTE: int my_vertical_offset
    // DESCRIPTION:
    // Beams always go up.  If the beam is actually down we need to start it below
    // the note.
    int my_vertical_offset;
    
    // ATTRIBUTE: int my_vertical_position
    // DESCRIPTION:
    // Where the note goes on the staff, sometimes used to determine the 'upness'
    // of the note head.
    int my_vertical_position;

    // ATTRIBUTE: int my_high/low_point
    // DESCRIPTION:
    // Some interesting things about the geometry of the figure.
    int my_high_point;
    int my_low_point;
    int my_staff_height;
    point my_absolute_ul;

    // ATTRIBUTE: my_connection_points:array<point>
    // DESCRIPTION:
    // This is the place where the note heads go.
    array<point> my_connection_points;
    
    // ATTRIBUTE: my_beam_points:array<point>
    // DESCRIPTION:
    // This is the place where the beams go.
    array<point> my_beam_points;

    // ATTRIBUTE: my_original_beam_points
    // DESCRIPTION:
    // This is for clone
    // array<point> my_original_beam_points;
    
    // ATTRIBUTE: my_xlet_values:array<int>
    // DESCRIPTION:
    // The xlet values that will be beamed under/over the beam groups
    array<int> my_xlet_values;
    
    // ATTRIBUTE: my_xlet_points
    // DESCRIPTION:
    // Keep track of where the descriptions of the xlets are supposed
    // to go.  We can't draw them because we don't know about fonts and
    // windows and stuff.
    array<point> my_xlet_points;
    
    // ATTRIBUTE: my_upness_values
    // DESCRIPTION:
    // Indicate whether the beams go up or down.
    int my_upness_values;
    
    // ATTRIBUTE: my_forced_upness
    // DESCRIPTION:
    // 1 or -1 if we are forced to beam up or down, otherwise it stays at
    // 0
    int my_forced_upness;
    
    // ATTRIBUTE: my_durations    
    // DESCRIPTION:
    // Keep track of the durations of notes so we can beam them
    array<fraction> my_durations;
    
    // ATTRIBUTE:
    // DESCRIPTION:
    // The note head that goes on the note.
    figure* my_note_head;

    // ATTRIBUTE: my_is_grace
    // DESCRIPTION:
    //  True if this is a grace note.
    bool my_is_grace;
};
}
#endif


