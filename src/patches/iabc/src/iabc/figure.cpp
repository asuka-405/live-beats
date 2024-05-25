/*
 * figure.cpp - Base class for rendering figures on a window.
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
#include "iabc/figure.h"
#include "iabc/drawtemp.cpp"
#include "iabc/list.cpp"
#include "iabc/factory.cpp"

// #define print_debug
#ifdef print_debug
#include <windows.h>
#define debug_print(b){OutputDebugString(b);};
#define debug_param(s,p){sprintf(tmp_buf,"%s = %d\n",s,(int)p);debug_print(tmp_buf);}
#else
#define debug_print(b)
#define debug_param(s,p)
#endif

char tmp_buf[256];

namespace iabc
{

instance_counter<figure> static_figure_count;

figure::figure(const size& the_ppi):my_ppi(the_ppi),
                                         my_poly(point(0,0)),
                                         my_polygon_rendered(false),
                                         my_scale(1.0,1.0),
                                         my_pen_width(1),
                                         my_is_scaled(false),
                                         my_debug_string("figure")
{
    ++static_figure_count;
    int i = static_figure_count.get_number_instances();
}

figure::~figure()
{
    --static_figure_count;
    int i = static_figure_count.get_number_instances();
}

figure*
figure::clone(const rect& r1,const rect& r2,const size& the_ppi)
{
    cloned_figure* tmp_clone = 
        new cloned_figure(*this,the_ppi,r1,r2);
    tmp_clone->my_debug_string = (string)"cloned " + my_debug_string;
    return tmp_clone;
}

void 
figure::remove_ref()
{
    global_figure_event_log.log_event(figure_event_data::remove_ref,(unsigned long)this,my_count.as_int());
    if (--my_count == 0) 
        delete this;
}

void 
figure::add_ref()
{
    global_figure_event_log.log_event(figure_event_data::add_ref,(unsigned long)this,my_count.as_int());
    ++my_count;
}

void
figure::set_xform(polygon& poly)
{
    // Most of the drawing routines deal in twips, but the screen
    // is in pixels.  A twip is 1/72 th of an inch, or sometimes
    // 1/72nd of an inch
    double x_xform =  (1.0/72.0)* (0.01)*(double)my_ppi.x * my_scale.x;
    double y_xform =  (1.0/72.0)* (0.01)*(double)my_ppi.y * my_scale.y;
    poly.set_x_zoom(x_xform);
    poly.set_y_zoom(-y_xform);
}

void
figure::render_poly()
{
    if (my_polygon_rendered == false)
    {
        global_figure_event_log.log_event(figure_event_data::render_poly,(unsigned long)this);
        set_xform(my_poly);
        create_poly(my_poly);
        my_polygon_rendered = true;
        calculate_sizeloc();
        calculate_extreme_points();
    }
}

rect 
figure::get_bounding_box(const point& the_point)
{
    sizeloc tmp_size = get_size_in_pixels();
    rect tmp_rv(tmp_size.my_loc + the_point,
                tmp_size.my_loc + tmp_size.my_size + the_point);
	return tmp_rv;
}

sizeloc
figure::get_size_in_pixels()
{
    render_poly();
    return my_sizeloc;
}

void
figure::calculate_sizeloc()
{
    size tmp_size = my_poly.get_size();
	sizeloc tmp_sizeloc;
    tmp_sizeloc.my_size.x = tmp_size.x;
    tmp_sizeloc.my_size.y = tmp_size.y;
    tmp_sizeloc.my_loc = my_poly.get_ul_offset();

    global_figure_event_log.log_event(figure_event_data::calc_size,(unsigned long)this);
    global_figure_event_log.log_event(figure_event_data::calc_size1,
                                      tmp_size.x,tmp_size.y,
                                      tmp_sizeloc.my_loc.x,tmp_sizeloc.my_loc.y);
	my_sizeloc = tmp_sizeloc;
}

int 
figure::get_width_in_pixels()
{
    sizeloc tmp_size = get_size_in_pixels();
    return tmp_size.my_size.x;
}

int 
figure::get_height_in_pixels()
{
    sizeloc tmp_size = get_size_in_pixels();
    return tmp_size.my_size.y;
}

point
figure::get_middle()
{
    sizeloc tmp_size = get_size_in_pixels();
    return point(tmp_size.my_loc.x + (tmp_size.my_size.x / 2),
                 tmp_size.my_loc.y + (tmp_size.my_size.y / 2));
}

void
figure::calculate_extreme_points()
{
    point tmp_pt;
    sizeloc tmp_size = get_size_in_pixels();
    
    my_top_point = my_poly.get_top_point();
    my_bottom_point = my_poly.get_bottom_point();
    my_leftmost_point = my_poly.get_left_point();
    my_rightmost_point = my_poly.get_right_point();
}

void
figure::rescale(const scale& the_scale)
{
    global_figure_event_log.log_event(figure_event_data::rescale,
                                      (unsigned long)this,
                                      (unsigned long)the_scale.x * 100,
                                      (unsigned long)the_scale.y * 100);
    my_scale = the_scale;
    my_polygon_rendered = false;
    my_poly.remove_all_points();
}

void
figure::draw_self(window& w,const point& the_origin)
{
    global_figure_event_log.log_event(figure_event_data::draw_self,
                                      (unsigned long)this,
                                      the_origin.x,the_origin.y);
    set_pen_width();

    pen* pp = w.get_pen(0,0,0,my_pen_width);
    pen& p = *pp;
    p.res_add_ref();
    w.push_context();
    render_poly();
    w.moveto(the_origin);
    w.render_polygon(p,my_poly);
    p.res_remove_ref();
    w.pop_context();
}

void 
figure::dispatch_draw_self(window& w,const point& the_origin,bool the_should_block)
{
    global_figure_event_log.log_event(figure_event_data::dispatch_draw_self,
                                      the_origin.x,the_origin.y,the_should_block);
    draw_figure_command::dispatch_draw_command(w,
        the_origin,*this,the_should_block);
}

void 
figure::set_pen_width()
{
    if (my_is_scaled == false)
    {
        my_pen_width = 1;
    }
    else
    {
        my_pen_width = calculate_scaled_pen_width();
    }
}

int 
figure::calculate_scaled_pen_width()
{
    int tmp_pen_width = (int)
        (0.5 + my_scale.x * (double)my_ppi.x * 1.0 * (1.0/72.0));

    return tmp_pen_width;
}

scaled_figure::scaled_figure(const size& the_ppi):
figure(the_ppi)
{
    my_is_scaled = true;
}

void 
scaled_figure::set_pen_width()
{
    my_pen_width = calculate_scaled_pen_width();
}

cloned_figure::cloned_figure(figure& the_model,const size& the_ppi,
                             const rect& r1,const rect& r2):figure(the_ppi),
							 my_model_rect(r1),my_rect(r2)
{
    the_model.add_ref();
    my_model = &the_model;

    // Note that this needs to come here, not in the initializer list!
    my_is_scaled = the_model.my_is_scaled;
}

void
cloned_figure::create_poly(polygon& the_poly)
{
    the_poly.clone(my_model->my_poly,my_model_rect,my_rect);    
}

void
cloned_figure::set_xform(polygon& the_poly)
{
    // We are always dealing with absolute pixel values
    // when cloning, so there is no transforming to be done.
    the_poly.set_x_zoom(1.0);
    the_poly.set_y_zoom(1.0);
}
}
