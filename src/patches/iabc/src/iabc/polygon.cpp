/*
 * polygon.cpp - The points in a polygon, constructed with drawing primitives.
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
#include "iabc/polygon.h"
#include <math.h>
#include "iabc/list.cpp"
#include "iabc/array.cpp"
#include "iabc/map.cpp"

#define debug_print(s)
// extern void debug_print(const char*);

#define STORE_COMMAND1(c,p1) \
    c* tmp_command = new                 \
        c(p1);           \
    my_commands.add_to_end(tmp_command)

#define STORE_COMMAND3(c,p1,p2,p3) \
    c* tmp_command = new                 \
        c(p1,p2,p3);           \
    my_commands.add_to_end(tmp_command)
    

namespace iabc
{

polygon::linear_xform::linear_xform(double the_m,double the_b):
m(the_m),b(the_b)
{    
};

polygon::linear_xform::linear_xform(const linear_xform& o):
m(o.m),b(o.b)
{};

polygon::polygon(const point& the_starting_point):
        my_segments(1,20),my_current_point(the_starting_point),
        my_fill_points(0,5),my_fill_point_count(0),
        x_xform(1.0,0.0),y_xform(1.0,0.0),
    my_need_to_size(true),my_need_to_offset(true)
{
	my_segments[0] = the_starting_point;
};

polygon::~polygon()
{
    delete_functor<polygon::poly_command*> f;
    do_to_all(my_commands,f);
}

point 
polygon::xform_point(const point& o) const
{
    return point(x_xform.xform_scalar(o.x),y_xform.xform_scalar(o.y));
}

// Note that for moveto and moveto relative we assume that the
// user filled or outlined the poly before moving the pen, if that
// was supposed to be done
void
polygon::moveto_relative(const point& the_point)
{
    my_current_point += xform_point(the_point);
    STORE_COMMAND1(moveto_relative_command,xform_point(the_point));

    // If this is the first point, add it or we will be coming
    // from 0,0 all the time.
    if (my_segments.get_size() == 1)
    {
        my_segments[0] = my_current_point;
    }
}

void 
polygon::lineto(const point& the_point)
{
    lineto_relative(the_point - my_current_point);
}

void 
polygon::arc(int the_angle1,int the_angle2,int r)
{
    my_need_to_size = true;
    my_need_to_offset = true;

    // Calculate the points p1 and p2 that define the chord.
    // Start by calculating the sin and cos of the angles,
    // and then multiply those by the known radius.
    double tmp_radians = 3.14159 / 180.0;
    while (the_angle1 > the_angle2)
    {
        the_angle2 -= 360;
    }
    

    double tmp_s1 = sin(tmp_radians * (double)the_angle1);
    double tmp_c1 = cos(tmp_radians * (double)the_angle1);
    double tmp_s2 = sin(tmp_radians * (double)the_angle2);
    double tmp_c2 = cos(tmp_radians * (double)the_angle2);

    point tmp_p1 = xform_point(point((int)((double)r * tmp_c1),
                         (int)((double)r * tmp_s1)));

    r = (int)(x_xform.m * (double)r) + 1;
    STORE_COMMAND3(arc_command,the_angle1,the_angle2,r);
    
    tmp_p1 += my_current_point;

    // Now we have the 2 points that define the arc for this device
    // coordinates.  Calculate the number of pixels we would use up 
    // using the circumference of the full circle as a guide.
    int tmp_circum = line(my_current_point,tmp_p1).magnitude();
    tmp_circum = (int)((double)2.0 * (double)tmp_circum * 3.14159);
    if (tmp_circum < 2)
        tmp_circum = 2;
    tmp_circum = (int)((double)tmp_circum * 
                       (double)((the_angle2 - the_angle1) / 360.0));
    int tmp_step_size = (the_angle2 - the_angle1)/tmp_circum;
    int i;
    point tmp_center = my_current_point;
    
    // Draw each pixel, moving around the circle.
    my_current_point = tmp_p1;
    point tmp_last_point = tmp_p1;
    for (i = the_angle1;i < the_angle2; i += tmp_step_size + 1)
    {
        double tmp_x = r * cos(tmp_radians * (double)i) + (double)tmp_center.x;
        double tmp_y = r * sin(tmp_radians * (double)i) + (double)tmp_center.y;
        tmp_p1 = point(point((int)tmp_x,(int)tmp_y));
        if (tmp_p1 != tmp_last_point)
        {
            if (must_add_current())
            {
                my_segments.add_to_end(my_current_point);
            }
            my_current_point = tmp_p1;
            my_segments.add_to_end(my_current_point);
            tmp_last_point = tmp_p1;
        }
    }
}

void
polygon::moveto(const point& the_point)
{
    my_current_point = xform_point(the_point);
    STORE_COMMAND1(moveto_command,my_current_point);

    // If this is the first point, add it or we will be coming
    // from 0,0 all the time.
    if (my_segments.get_size() == 1)
    {
        my_segments[0] = my_current_point;
    } else if (my_segments.get_size() == 0)
	{
		my_segments.add_to_end(my_current_point);
	}
}

bool 
polygon::must_add_current()
{
    if ((my_segments.get_size() < 1) ||
        (my_segments[my_segments.get_size() - 1] != my_current_point))
    {
        return true;
    }
	return false;
}

void 
polygon::clone(polygon& o,const rect& the_r1,const rect& the_r2)
{
    my_segments.remove_all();
    my_fill_points.remove_all();
    delete_functor<polygon::poly_command*> f;
    do_to_all(my_commands,f);

    set_x_zoom(1.0);
    set_y_zoom(1.0);
    set_x_offset(0);
    set_y_offset(0);
    point tmp_start(0,0);
	my_current_point = tmp_start;
	// my_segments.add_to_end(tmp_start);
    // moveto(tmp_start);
    int i;
    for (i = 0;i < o.my_commands.get_size();++i)
    {
        o.my_commands[i]->execute(*this,the_r1,the_r2);
    }
}

void
polygon::lineto_relative(const point& the_point)
{
    my_need_to_size = true;
    if (must_add_current())
    {
        my_segments.add_to_end(my_current_point);
    }
    STORE_COMMAND1(lineto_relative_command,xform_point(the_point));
    my_current_point += xform_point(the_point);
    my_segments.add_to_end( my_current_point);
}

void
polygon::curveto(const point& the_c1, 
                            const point& the_c2,
                            const point& the_endp)
{
    curveto_relative(the_c1 - my_current_point,
                     the_c2 - my_current_point,
                     the_endp - my_current_point);
}

bool 
polygon::does_spline_exist_for_point(const point& p)
{
    point_ordinal tmp_p(p);
    bool tmp_rv = false;
    map<point_ordinal,spline_control_points>::iterator 
        tmp_it = my_spline_points.get_item(tmp_p);
    if (tmp_it)
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

polygon::spline_control_points 
polygon::get_spline_for_point(const point& p)
{
    point_ordinal tmp_p(p);
    map<point_ordinal,spline_control_points>::iterator 
        tmp_it = my_spline_points.get_item(tmp_p);
    spline_control_points tmp_points;
    if (tmp_it)
    {
        tmp_points = (*tmp_it).value;
    }
    return tmp_points;
}

polygon::spline_control_points::spline_control_points(const point& the_p0,
                      const point& the_p1,
                      const point& the_p2,
                      const point& the_p3):
p0(the_p0),
p1(the_p1),
p2(the_p2),
p3(the_p3)
{
    calc_cx();
    calc_cy();
    calc_bx();
    calc_by();
    calc_ax();
    calc_ay();
}
polygon::spline_control_points::spline_control_points(
    const polygon::spline_control_points& o):
p0(o.p0),p1(o.p1),p2(o.p2),p3(o.p3),
a(o.a),b(o.b),c(o.c)
{
}

polygon::spline_control_points& 
polygon::spline_control_points::operator=(const spline_control_points& o)
{
p0 = (o.p0);p1 = (o.p1);p2=(o.p2);p3=(o.p3);
a=(o.a);b=(o.b);c=(o.c);
    return *this;
}

point 
polygon::spline_control_points::pt(double t) const
{
    double dx,dy;
    dx = (t*t*t)*a.x +
        (t*t)*b.x + 
        t*c.x + (double)p0.x;
    dy = (t*t*t)*a.y +
        (t*t)*b.y + 
        t*c.y + (double)p0.y;
    return point((int)dx,(int)dy);
}

void
polygon::curveto_relative(const point& the_c1, 
                            const point& the_c2,
                            const point& the_endp)

{
    my_need_to_size = true;
    my_need_to_offset = true;

    // We use a 3rd degree polynomai Bezier Curve.  We compute the first
    // and last point and recursively compute the inner points.  We store the
    // points in a map during computation, and then copy them to the segment
    // array.
    map<double,point> tmp_point_list;
    spline_control_points tmp_control_points(my_current_point,
                                             my_current_point + xform_point(the_c1),
                                             my_current_point + xform_point(the_c2),
                                             my_current_point + xform_point(the_endp));
    tmp_point_list.add_pair(0.0,tmp_control_points.pt(0.0));    
    point tmp_last_point = tmp_control_points.pt(1.0);
    tmp_point_list.add_pair(1.0,tmp_last_point);

    // The recursive call
    make_midpoints(tmp_point_list,my_current_point,0.0,
                   tmp_last_point,1.0,tmp_control_points);

    // Now all the points are in our map, so we just need to iterate
    // through them.
    map<double,point>::iterator tmp_point_it = 
        tmp_point_list.least();
    while (tmp_point_it)
    {
        point p = (*tmp_point_it).value;
        if (p != my_current_point)
        {
            my_segments.add_to_end(p);
        }
        tmp_point_it = tmp_point_it.get_item((*tmp_point_it).key,gt);
    }
    my_current_point = tmp_last_point;
}


size
polygon::get_size()
{
    if (my_need_to_size == true)
    {
        my_need_to_size = false;
        int x = my_segments[0].x;
        int y = my_segments[0].y;
        int x1 = x;
        int y1 = y;
        for (int i = 0; i < my_segments.get_size(); ++ i)
        {
            point current = my_segments[i];

            if (current.x < x)
            {
                x = current.x;
            }
            if (current.x > x1)
            {
                x1 = current.x;
            }
            if (current.y < y)
            {
                y = current.y;
            }
            if (current.y > y1)
            {
                y1 = current.y;
            }
        }
        my_size = size(abs(x - x1),abs(y - y1));
    }

    return my_size;

}

point 
polygon::get_top_point()
{
    get_ul_offset();
    return my_top_point;
}
point 
polygon::get_bottom_point()
{
    get_ul_offset();
    return my_bottom_point;
}

point 
polygon::get_left_point()
{
    get_ul_offset();
    return my_leftmost_point;
}

point polygon::get_right_point()
{
    get_ul_offset();
    return my_rightmost_point;
}

point 
polygon::get_ul_offset()
{
	// If we have reversed the meaning of -y being 'up',
	// compensate for that.

    if (my_need_to_offset == true)
    {
        my_need_to_offset = false;
        int x = my_segments[0].x;
        int y = my_segments[0].y;
        int rightmost = x;
        int leftmost = x;
        int top = y;
        int bottom = y;
        for (int i = 0; i < my_segments.get_size(); ++ i)
        {
            point current = my_segments[i];
            if (current.x < x)
            {
                x = current.x;
            }
            if (x <= leftmost)
            {
                my_leftmost_point = current;
            }
            if (x >= rightmost)
            {
                my_rightmost_point = current;
            }
            if (current.y < y)
            {
                y = current.y;
            }
            if (y <= top)
            {
                my_top_point = current;
            }
            if (y >= bottom)
            {
                my_bottom_point = current;
            }
        }
    
        my_ul_offset = point(x,y);
    }

    return my_ul_offset;
}

void
polygon::expand_points(int the_number)
{
    my_segments.expand(the_number);
}

void
polygon::expand_fill_points(int the_number)
{
    my_fill_points.expand(the_number);
}

void
polygon::remove_all_points()
{
	point tmp_point = my_segments[0];
    my_segments.remove_all();
	my_segments.add_to_end(tmp_point);
}

void 
polygon::make_midpoints(map<double,point>& t_point_map,
                    const point& first_point,
                    double first_t,
                    const point& last_point,
                    double last_t,
                    const spline_control_points& curve)
{
    // The last and next points have been computed by the last recurse.
    // Compute the midpoint, and the curve point for that midpoint.
    double t = (first_t + last_t)/2.0;
    point p = curve.pt(t);

    // We recursively bisect the line until the points become adjacent.
    // Stop if we have reached that point.
    if ((first_point != p) &&
        (last_point != p))
    {
        t_point_map.add_pair(t,p);
        make_midpoints(t_point_map,
                       p,t,
                       last_point,last_t,
                       curve);
        make_midpoints(t_point_map,
                       first_point,
                       first_t,
                       p,
                       t,
                       curve);
    }
}


polygon::fill_point 
polygon::get_next_fill_point(const fill_point& the_last) const
{
    int i = 0;
    while ((i < my_fill_points.get_size()) &&
           (my_fill_points[i].my_index <= the_last.my_index))
    {
        ++i;
    }
    if (i < my_fill_points.get_size())
    {
        return my_fill_points[i];
    }
    else
    {
        return fill_point(fill,my_segments.get_size() - 1);
    }
};
    
void 
polygon::fill_poly()
{
    STORE_COMMAND1(fill_command,polygon::fill);
    my_fill_points.add_to_end( fill_point(fill,my_segments.get_size() - 1));
}

void 
polygon::outline_poly()
{
    STORE_COMMAND1(fill_command,polygon::outline);
    my_fill_points.add_to_end(fill_point(outline,my_segments.get_size() - 1));
}

void 
fill_command::execute(polygon &p)
{
    if (my_fill_mode == polygon::fill)
    {
        p.fill_poly();
    }
    else
    {
        p.outline_poly();
    }
}

void 
fill_command::execute(polygon &p, const rect &r1, const rect &r2)
{
    execute(p);
}

void 
arc_command::execute(polygon& p,const rect& r1,const rect& r2)
{
    int tmp_r = (int)((double)my_r * (double)r2.height() / (double)r1.height());
    p.arc(my_angle1,my_angle2,tmp_r);
}

void 
moveto_relative_command::execute(polygon& p,const rect& r1,const rect& r2)
{
    point tmp_point = scale_point(my_point,r1,r2);
    p.moveto_relative(tmp_point);
}

void 
moveto_command::execute(polygon& p,const rect& r1,const rect& r2)
{
    point tmp_point = stretch_point(my_point,r1,r2);
    p.moveto(tmp_point);
}

void 
lineto_relative_command::execute(polygon& p,const rect& r1,const rect& r2)
{
    point tmp_point = scale_point(my_point,r1,r2);
    p.lineto_relative(tmp_point);
}

void 
curveto_relative_command::execute(polygon& p,const rect& r1,const rect& r2)
{
    point p1 = scale_point(my_p1,r1,r2);
    point p2 = scale_point(my_p2,r1,r2);
    point p3 = scale_point(my_p3,r1,r2);
    p.curveto_relative(p1,p2,p3);
}
}

