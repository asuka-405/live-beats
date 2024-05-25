/*
 * slurer.cpp - Handle slurring of notes.
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
#include "iabc/map.cpp"
#include "iabc/list.cpp"
#include "iabc/slurer.h"

namespace iabc
{
;

slurer::slurer(const size& the_ppi,const staff_info& the_staff):
my_depth(the_staff.size.my_size.y / 7),figure(the_ppi),my_staff(the_staff)
{}

void 
slurer::set_xform(polygon& poly)
{
    poly.set_y_zoom(1.0);
    poly.set_x_zoom(1.0);
    poly.set_y_offset(0);
    poly.set_x_offset(0);
}

void
slurer::add_from_point(const point& from)
{
    // We found an open paren.  Put it in the
    // orphans pile until we get a matchine
    // close paren.
    list<point>::iterator tmp_points = my_orphans.last();
    tmp_points.reset_to_back();
    tmp_points.add_after(from);
}

void 
slurer::add_to_point(const point& the_to,slur_direction the_direction)
{
    // We found a close paren.  Get one of the orphan
    // points off the orphan pile and create a slurpoint.
    list<point>::iterator tmp_points = my_orphans.first();
    point tmp_from;
    bool tmp_should_orphan = false;

    if (tmp_points)
    {
        tmp_from = (*tmp_points);

        // If the points are the same, the from link is really
        // intended to slur to a later point, and the 'to' 
        // point is an orphan from a previous line.
        if (tmp_from == the_to)
        {
            tmp_should_orphan = true;
        }
        else
        {
            tmp_points.delete_current();
            slurpoint tmp_slur(tmp_from,the_to);
		    slur_info tmp_info;
		    tmp_info.slur = tmp_slur;
            int tmp_midpoint = my_staff.vertical_position + (my_staff.size.my_size.y / 2);

		    tmp_info.direction = the_direction;
            my_points.add_pair(tmp_from.x,tmp_info);
        }
    }
    else
    {
        tmp_should_orphan = true;
    }

    // There are no orphans, which means that this slur was from a previous
    // line.  Slur to the beginning of the line.
    if (tmp_should_orphan == true)
    {
        add_at_end(my_to_orphans,the_to);
    }
}

int 
slurer::get_number_points() const 
{
    int tmp_rv = my_points.get_size() + my_orphans.get_size() + my_to_orphans.get_size();
    return tmp_rv;
}

int 
slurer::get_number_orphans()
{
    return my_orphans.get_size();
}

void 
slurer::normalize()
{
    int tmp_first_point = 0;

    map<int,slur_info>::iterator tmp_pair = 
        my_points.get_item(tmp_first_point,gt);

    // For each slur
    bool first_time = true;

    while (tmp_pair != 0)
    {
        slur_info& tmp_info = (*tmp_pair).value;
        if (first_time)
        {
            my_absolute_location = tmp_info.slur.p1;
            first_time = false;
        }
        tmp_info.slur.p1 -= my_absolute_location;
        tmp_info.slur.p2 -= my_absolute_location;
        tmp_pair = my_points.get_item((*tmp_pair).key,gt);
    }

    list<point>::iterator tmp_oit = my_orphans.first();
    while (tmp_oit)
    {
        (*tmp_oit) -= my_absolute_location;
        tmp_oit.next();
    }

    tmp_oit = my_to_orphans.first();
    while (tmp_oit)
    {
        (*tmp_oit) -= my_absolute_location;
        tmp_oit.next();
    }

}

void
slurer::create_poly(polygon& poly)
{
    int tmp_first_point = 0;

    // Gather the points around 0, and store the pixel location
    normalize();
    map<int,slur_info>::iterator tmp_pair = 
        my_points.get_item(tmp_first_point,gt);

    // For each slur
    while (tmp_pair != 0)
    {
        slurpoint tmp_points = (*tmp_pair).value.slur;
        int tmp_direction = ((*tmp_pair).value.direction == up ? -1 : 1);

        // Compute the midpoint
        point tmp_from = tmp_points.p1;
        point tmp_to = tmp_points.p2;
        tmp_from.y += 10 * tmp_direction;
		tmp_to.y += 10 * tmp_direction;
        poly.moveto(tmp_from);
        int ydelta = tmp_direction * my_depth * 2;
        int xdelta = (tmp_to.x - tmp_from.x) / 4;

        point tmp_c1 = point(tmp_from.x + xdelta,tmp_from.y + ydelta);
        point tmp_c2 = point(tmp_to.x - xdelta,tmp_to.y + ydelta);
        poly.curveto_relative(tmp_c1 - tmp_from,tmp_c2 - tmp_from,tmp_to - tmp_from);
        tmp_c1.y += tmp_direction * my_depth;
        tmp_c2.y += tmp_direction * my_depth;
        poly.curveto_relative(tmp_c2 - tmp_to,tmp_c1 - tmp_to,tmp_from - tmp_to);
        poly.fill_poly();
        tmp_pair = my_points.get_item((*tmp_pair).key,gt);
    }
    list<point>::iterator tmp_it = my_orphans.first();
    while (tmp_it)
    {
        point tmp_from = (*tmp_it);
        int tmp_direction = (tmp_from.y - my_absolute_location.y > 
            (my_staff.vertical_position - my_staff.size.my_size.y / 2))  
                ? 1 : -1;
        
        point tmp_to = point(my_staff.horizontal_position + my_staff.size.my_size.x,
                       tmp_from.y + tmp_direction * my_depth);
        tmp_to.x -= my_absolute_location.x;

        tmp_from.y += 10 * tmp_direction;
		tmp_to.y += 10 * tmp_direction;
        poly.moveto(tmp_from);
        int ydelta = tmp_direction * my_depth * 2;

        point tmp_c1 = point(tmp_from.x,tmp_from.y + ydelta);
        point tmp_c2 = point(tmp_to.x,tmp_to.y + ydelta);
        poly.curveto_relative(tmp_c1 - tmp_from,tmp_c2 - tmp_from,tmp_to - tmp_from);
        tmp_c1.y += tmp_direction * my_depth;
        tmp_c2.y += tmp_direction * my_depth;
        poly.curveto_relative(tmp_c2 - tmp_to,tmp_c1 - tmp_to,tmp_from - tmp_to);
        poly.fill_poly();
        tmp_it.next();
    }
    
    tmp_it = my_to_orphans.first();
    while (tmp_it)
    {
        point tmp_to = (*tmp_it);
        int tmp_direction = (tmp_to.y > 
            (my_staff.vertical_position - my_staff.size.my_size.y / 2))  
                ? 1 : -1;
        
        point tmp_from = point(my_staff.horizontal_position,
                       tmp_to.y + tmp_direction * my_depth);
        tmp_from.x -= my_absolute_location.x;
        tmp_from.y += 10 * tmp_direction;
		tmp_to.y += 10 * tmp_direction;
        poly.moveto(tmp_from);
        int ydelta = tmp_direction * my_depth * 2;

        point tmp_c1 = point(tmp_from.x,tmp_from.y + ydelta);
        point tmp_c2 = point(tmp_to.x,tmp_to.y + ydelta);
        poly.curveto_relative(tmp_c1 - tmp_from,tmp_c2 - tmp_from,tmp_to - tmp_from);
        tmp_c1.y += tmp_direction * my_depth;
        tmp_c2.y += tmp_direction * my_depth;
        poly.curveto_relative(tmp_c2 - tmp_to,tmp_c1 - tmp_to,tmp_from - tmp_to);
        poly.fill_poly();

        tmp_it.next();
    }
}

}

