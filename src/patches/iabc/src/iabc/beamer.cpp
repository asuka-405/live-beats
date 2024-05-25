/*
 * beamer.cpp - Beams the notes in abc.
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
#include "iabc/beamer.h"
#include "iabc/array.cpp"
#include "iabc/figure_factory.h"

namespace iabc
{
beamer::beamer(const size& the_ppi,staff_info the_staff,bool the_is_grace)
:scaled_figure(the_ppi),my_depth(3),my_vertical_offset(0),
    my_note_head(0),my_upness_values(0),
    my_forced_upness(0),my_is_grace(the_is_grace)
{
    my_vertical_offset = (the_staff.size.my_size.y);
	my_vertical_position = the_staff.vertical_position;
    my_low_point = my_high_point = my_vertical_position;
        
    double tmp_twip_depth = (1.0 / 72.0) * (double)the_ppi.y;
    my_depth = (int)(tmp_twip_depth * (double)my_depth);

    if (my_depth < 3)
    {
        my_depth = 3;
    }
	
	my_staff_height = the_staff.size.my_size.y;
    my_low_point += my_staff_height;
}

beamer::~beamer()
{
    if (my_note_head)
    {
        my_note_head->remove_ref();
    }
}

void
beamer::beam_me(const point& the_point,figure& the_note_head,const fraction& the_duration,
                int the_xlet_value)
{
    my_beam_points.add_to_end(the_point);
    // my_original_beam_points.add_to_end(the_point);
	if (my_note_head)
		my_note_head->remove_ref();
    my_note_head = &the_note_head;
    the_note_head.add_ref();
    my_durations.add_to_end(the_duration);
    my_xlet_values.add_to_end(the_xlet_value);
}

point 
beamer::get_average()
{
    point tmp_rv;
    for (int i = 0;i < my_beam_points.get_size(); ++i)
    {
        tmp_rv += my_beam_points[i];   
    }

    if (my_beam_points.get_size())
    {
        tmp_rv = tmp_rv / my_beam_points.get_size();
    }

	return tmp_rv;
}

void 
beamer::create_head_offset()
{
    // Try to decide whether the stem for this note will go up
    // or down, so we know where to connect the stem to the head.
    point tmp_average = get_average();
    my_upness_values = my_forced_upness;

    // 3 cases:  1 is that the note is low, so the stems go up.
    if ((my_upness_values == 0) &&
        (tmp_average.y > my_vertical_position + my_staff_height / 2))
    {
        my_vertical_offset *= -1;
        my_upness_values = -1;
    }
    // 2 is that the note is high, so the stems go down.
    else if (my_upness_values == 0)
    {
        my_upness_values = 1;
    }
    // 3 is that the stems are forced up.  Other wise the stem is 
    // forced down.
    else if (my_upness_values < 0)
    {
        my_vertical_offset *= -1;
    }

    for (int i = 0;i < my_beam_points.get_size();++i)
    {
        if (my_upness_values < 0)
        {
            my_beam_points[i] += my_note_head->get_rightmost_point();
        }
        else
        {
            my_beam_points[i] -= this->my_note_head->get_leftmost_point();
        }
    }
    deep_copy(my_beam_points,my_connection_points);
}

void
beamer::set_hi_low(int the_high,int the_low)
{
    the_high += my_absolute_ul.y;
    the_low  += my_absolute_ul.y;
    if (the_high > my_high_point)
    {
        my_high_point = the_high;
    }
	if (the_high < my_low_point)
	{
        my_low_point = the_high;
	}
    if (the_low > my_high_point)
    {
        my_high_point = the_low;
    }
    if (the_low < my_low_point)
    {
        my_low_point = the_low;
    }
}

int
beamer::get_tail_width(const chord_info& the_info) const
{
	fraction tmp_longest_beamed_thing = 
		fraction(1,8) + fraction(1,16) + fraction(1,32) + fraction(1,64);
    if ((the_info.is_first_in_beam() == true) &&
        (the_info.get_duration() <= tmp_longest_beamed_thing))
    {
        return (int)((1.0/72.0) * (double)my_ppi.x * 4.0 * my_scale.x);
    }

    return 0;
}

void
beamer::create_tail()
{
    // If this is the only note in a beam group and it's short, beam it up.
    // (or down)
	fraction tmp_longest_beamed_thing = 
		fraction(1,8) + fraction(1,16) + fraction(1,32) + fraction(1,64);
    if ((my_beam_points.get_size() == 1) &&
        (this->my_durations[0] <= tmp_longest_beamed_thing))
    {
        double tmp_x_zoom = my_poly.get_x_zoom();
        double tmp_y_zoom = my_poly.get_y_zoom();

        double y_direction = my_upness_values;
        my_upness_values = my_forced_upness;
        if (my_upness_values == 0)
        {
            my_upness_values = (int)y_direction;
        }
        else
        {
            y_direction = (double)my_upness_values;
        }
        
        my_poly.set_x_zoom((1.0/72.0)* (0.01)*(double)my_ppi.x * my_scale.x);
        my_poly.set_y_zoom((y_direction/72.0)* (0.01)*(double)my_ppi.y * my_scale.y);
    
        fraction tmp_duration = my_durations[0];
        //fprintf(f,"  0.00 0.00 rmoveto\n");
        my_poly.moveto_relative(point(0, 0));
        while ((tmp_duration <= tmp_longest_beamed_thing) &&
            (tmp_duration > 0))
        {
            //fprintf(f,"  1.00 -2.00 0.67 -1.67 2.67 -4.00 rcurveto\n");
            my_poly.curveto_relative(point(100,-200),point(67,-167),point(267,-400));
            //fprintf(f,"  3.33 -2.67 3.33 -6.67 2.67 -9.33 rcurveto\n");
            my_poly.curveto_relative(point( 333, -267),point( 333, -667),point( 267, -933));
            //fprintf(f,"  -0.67 -2.67 -2.67 -4.00 -1.00 -1.00 rcurveto\n");
            my_poly.curveto_relative(point(-67, -267),point( -267, -400),point( -100, -100));
            //fprintf(f,"  1.67 4.33 -1.67 8.33 -4.33 9.00 rcurveto\n");
            my_poly.curveto_relative(point(  167, 433),point( -167, 833),point( -433, 900));
			my_poly.fill_poly();
            my_poly.moveto_relative(point(0,-340));

            tmp_duration *= fraction(2,1);
        }
        my_poly.set_x_zoom(tmp_x_zoom);
        my_poly.set_y_zoom(tmp_y_zoom);
    }
}

bool 
beamer::is_dotted(const fraction& the_value,const fraction& the_base)
{
    bool tmp_rv = false;
    fraction tmp_value = the_value;
    for (int i = the_base.den;i <= 32; i *= 2)
    {
        fraction tmp_value(1,i);
        for (int j = i * 2;j <= 64; j *= 2)
        {
            fraction tmp_dot(1,j);
            tmp_value += tmp_dot;
            if (tmp_value == the_value)
            {
                tmp_rv = true;
                break;
            }
        }
    }

    return tmp_rv;
}

bool 
beamer::should_beam(const fraction& the_value,const fraction& the_base)
{
    bool tmp_rv = is_dotted(the_value,the_base);

    if (tmp_rv == false)
    {
        tmp_rv = (the_value <= the_base) ? true : false;
    }
    return tmp_rv;
}

void 
beamer::draw_beam_segment(const point& the_p1,const point& the_p2,bool the_draw_half,bool the_left)
{
    point tmp_p2 = the_p2;
    point tmp_p1 = the_p1;
    if (the_draw_half == true)
    {
        tmp_p2 = (the_p1 + the_p2) / 2;
        if (the_left == false)
        {
            my_poly.moveto_relative(point(tmp_p1.x - tmp_p2.x,0));
        }
    }
    my_poly.lineto_relative(tmp_p1 - tmp_p2);
    my_poly.lineto_relative(point(0,my_depth));
    my_poly.lineto_relative(tmp_p2 - tmp_p1);
    my_poly.lineto_relative(point(0,-my_depth));
    my_poly.fill_poly();

    if ((the_draw_half == true) &&
        (the_left == false))
    {
        my_poly.moveto_relative(point(tmp_p2.x - tmp_p1.x,0));
    }
}

void 
beamer::create_beam()
{
    // Get either the highest or lowest point in the beam group,
    // depending on whether we're beaming down or up.
    point tmp_extreme_point;
    if (my_upness_values < 0)
    {
        tmp_extreme_point = get_highest_point();
    }
    else
    {
        tmp_extreme_point = get_lowest_point();
    }

    double tmp_slope = 0.0;

    point tmp_first_point = get_first_point();
    point tmp_last_point = get_last_point();
    if (((tmp_last_point.x == tmp_extreme_point.x) ||
        (tmp_first_point.x == tmp_extreme_point.x)) &&
		(my_beam_points.get_size() > 1))
    {
        // We like to make the beam slope go from first to last,
        // if first or last is the high note.  That looks pretty
        // natural
        tmp_slope = (tmp_first_point.y - tmp_last_point.y) /
            (double)(tmp_first_point.x - tmp_last_point.x);
        beamer::multiply_pred tmp_times(tmp_slope,tmp_first_point);
        do_to_all(my_beam_points,tmp_times);

        beamer::add_int_pred tmp_adder(my_vertical_offset);
        do_to_all(my_beam_points,tmp_adder);
    }
    else
    {
        // Otherwise we just draw a straight beam across the top.
        // Do this by setting the beam points to be vertically offset
        // from the connection points (which are currently the same).
        my_vertical_offset += tmp_extreme_point.y;
        if (my_beam_points.get_size() < 2)
        {
            my_vertical_offset += (int)(1.8 * (double)my_depth * (double)my_upness_values);
        }
        beamer::set_absolute_height_pred tmp_hp(my_vertical_offset);
        do_to_all(my_beam_points,tmp_hp);
    }

    // There is no beam to draw if there is no beam group
    if (my_beam_points.get_size() > 1)
    {
        // Now all of our points should contain the absolute points of the beam.
        // So just render the figure.
        my_poly.moveto(my_beam_points[0]);
        int tmp_last_index = my_beam_points.get_size() - 1;
		fraction tmp_base(1,8);
        bool tmp_continue = true;
        while (tmp_continue)
        {
            tmp_continue = false;
            
            for (int i = 1;i <= tmp_last_index;++i)
            {
                bool tmp_draw_half = false;
                point tmp_midpoint = (my_beam_points[i] + my_beam_points[i-1])/2;

                // Draw the first part of the beam from the i-1th note to the
                // ith note
                bool tmp_should_draw = false;

                // If this is the first beam over the top/bottom, always draw it
                if (tmp_base == fraction(1,8))
                {
                    tmp_should_draw = should_beam(my_durations[i - 1],tmp_base);
                }
                
                // If this is the first note in a beam group, and its short enough to
                // beam, always beam it.
                if (i == 1)
                {
                    if (should_beam(my_durations[i - 1],tmp_base) == true)
                    {
                        tmp_should_draw = true;
                        if (should_beam(my_durations[i],tmp_base) == false)
                        {
                            tmp_draw_half = true;
                        }
                    }
                }
                // If this not the first note, only beam the first part
                // of the bar if we are beaming to another note of the
                // same or shorter length
                else if ((should_beam(my_durations[i - 1],tmp_base) == true) &&
                    (should_beam(my_durations[i],tmp_base) == true))
                {
                    tmp_should_draw = true;
                }

                // Now draw the beam, if we just decided we should
                if (tmp_should_draw)
                {
                    draw_beam_segment(tmp_midpoint,my_beam_points[i - 1],tmp_draw_half,true);
                    if (tmp_base * fraction(1,2) >= my_durations[i - 1])
                    {
                        tmp_continue = true;
                    }
                }

                // Draw the second part of a beam, from the ith note to
                // the i- 1th note.
                tmp_should_draw = false;
                tmp_draw_half = false;

                // If this is the last  note in a beam group, and it is
                // short enough to beam, always beam it.
                if (i == tmp_last_index)
                {
                    if (should_beam(my_durations[i],tmp_base) == true)
                    {
                        tmp_should_draw = true;
                        if (should_beam(my_durations[i - 1],tmp_base) == false)
                        {
                            tmp_draw_half = true;
                        }
                    }
                }
                // If this is short enough to beam, and the last note was
                // also short enough, beam it
                else if ((should_beam(my_durations[i],tmp_base) == true) &&
                    (should_beam(my_durations[i - 1],tmp_base) == true))
                {
                    tmp_should_draw = true;
                }
                // If this is short enough to beam, and the next note
                // is not short enough to beam, this is where the beam shows up.
                else if ((should_beam(my_durations[i],tmp_base) == true) &&
                         (should_beam(my_durations[i + 1],tmp_base) == false))
                {
                    tmp_should_draw = true;
                    if (should_beam(my_durations[i - 1],tmp_base) == false)
                    {
                        tmp_draw_half = true;
                    }
                }

                // move to the 1/2 point for the next part of the line
                my_poly.moveto_relative(tmp_midpoint - my_beam_points[i - 1]);
                
                if (tmp_should_draw)
                {
                    draw_beam_segment(my_beam_points[i],tmp_midpoint,tmp_draw_half,false);
                    if ((tmp_base * fraction(1,2) >= my_durations[i]) &&
                        (my_durations[i] > 0))
                    {
                        tmp_continue = true;
                    }
                }
				my_poly.moveto_relative(my_beam_points[i] - tmp_midpoint);
            }
			my_poly.moveto_relative(my_beam_points[0] - my_beam_points[tmp_last_index]);
            
            // We beam 1/8th notes, and then double-beam all
            // notes 1/2 the length of 1/8th notes.
            tmp_base *= fraction(1,2);
            if (tmp_continue == true)
            {
                double tmp_depth = (double)my_depth * 2.0 * my_scale.y;
                my_poly.lineto_relative(point(0,(int)tmp_depth) * my_upness_values * -1);
                my_poly.outline_poly();
                tmp_continue = true;
            }            
        }
    }
}

void 
beamer::create_stems()
{
    for (int i = 0;i < my_beam_points.get_size();++i)
    {
        if (my_durations[i].as_int() >= 1)
            continue;
        my_poly.moveto(my_connection_points[i]);
        my_poly.lineto_relative(my_beam_points[i] - my_connection_points[i]);
        my_poly.outline_poly();
        
        // Draw the diaganol line through the grace note.
        if (my_is_grace == true)
        {
            point tmp_origin = my_beam_points[i];
            int x1,y1,x2,y2;
            x1 = (my_beam_points[i] .y - my_connection_points[i].y)/5;
            y1 = (my_beam_points[i] .y - my_connection_points[i].y)/5;
			x2 = -x1 * 2;
			y2 = -y1 * 2;
            my_poly.moveto_relative(point(-x1,-y1 * 2));
            my_poly.lineto_relative(point(-x2,(y2 - y1)/2));
            my_poly.outline_poly();
            if (i + 1 == my_beam_points.get_size())
            {
                my_poly.moveto(my_beam_points[i]);
            }
        }
    }
}

void
beamer::set_xform(polygon& poly)
{
    my_poly.set_y_zoom(1.0);
    my_poly.set_x_zoom(1.0);
    my_poly.set_y_offset(0);
    my_poly.set_x_offset(0);
}

array<point>
beamer::get_xlet_points()
{
    // Make sure my points are all there.
    render_poly();
    return my_xlet_points;
}

void
beamer::create_xlet()
{
    int tmp_last_xlet_index = 0;
    my_xlet_points.remove_all();
    int tmp_last_beam_index = 0;
    for (int i = 0;i < my_xlet_values.get_size();++i)
    {
        // Every xth note, beam the group in an xlet.
        if ((my_xlet_values[i] > 0) &&
            (((i+1) % my_xlet_values[i]) == 0))
        {
            point tmp_point = my_beam_points[i];
            int tmp_scalar = 3;
            if (my_upness_values > 0)
                tmp_scalar = 1;
            tmp_point.y += my_upness_values * my_note_head->get_height_in_pixels() * tmp_scalar;
            tmp_point.x -= (tmp_point.x - my_beam_points[tmp_last_beam_index].x)/2;
            set_hi_low(tmp_point.y,tmp_point.y);
            tmp_point += my_absolute_ul;
            tmp_last_beam_index = i;
            my_xlet_points.add_to_end(tmp_point);
        }
    }
}

void 
beamer::create_poly(polygon &poly)
{
    create_head_offset();
    
    if (my_connection_points.get_size() > 0)
    {
        my_absolute_ul = my_connection_points[0];
        add_functor<point,point> the_offsets(point(-1,-1) * my_absolute_ul);
        do_to_all(my_connection_points,the_offsets);
        do_to_all(my_beam_points,the_offsets);
        poly.moveto_relative(point(0,0));
    }

    create_beam();

    create_stems();

	create_tail();

    create_xlet();

    size tmp_size = poly.get_size();
    point tmp_offset = poly.get_ul_offset();
    set_hi_low(tmp_offset.y,
               tmp_size.y + tmp_offset.y);
}

point 
beamer::get_highest_point()
{
    beamer::get_highest_point_pred tmp_pred(my_beam_points[0]);
    do_to_all(my_beam_points,tmp_pred);
    return tmp_pred.my_highest;
}


point 
beamer::get_lowest_point()
{
    beamer::get_lowest_point_pred tmp_pred(my_beam_points[0]);
    do_to_all(my_beam_points,tmp_pred);
    return tmp_pred.my_lowest;
}

}

