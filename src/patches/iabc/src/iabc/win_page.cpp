/*
 * win_page.cpp - Keeps track of some page information.
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
#include "iabc/win_page.h"
#include "iabc/map.cpp"
#include "iabc/array.cpp"
#include "iabc/factory.cpp"
#include "iabc/iabc_preferences.h"

#ifdef print_debug
#include <windows.h>
#define debug_print(b){OutputDebugString(b);};
#define debug_param(s,p){sprintf(tmp_buf,"%s = %d\n",s,(int)p);debug_print(tmp_buf);}
#else
#define debug_print(b)
#define debug_param(s,p)
#endif

namespace iabc
{
;
static char tmp_buf[1024];

static instance_counter<page_contents> page_contents_counter ;
#define class_instance_count page_contents_counter;

page_contents::~page_contents()
{
    --class_instance_count;
    map<point_ordinal,figure_array>::iterator tmp_it = 
        my_figures.least();
    while (tmp_it)
    {
        figure_array tmp_array = 
            (*tmp_it).value;
        int i;
        for (i = 0;i < tmp_array.get_size();++i)
        {
            tmp_array[i]->remove_ref();
        }
        point tmp_key = (*tmp_it).key;
        tmp_it = my_figures.get_item(tmp_key,gt);
    }
    
    map<point_ordinal,text_figure*>::iterator tmp_it1 = 
        my_text.least();
    while (tmp_it1)
    {
        (*tmp_it1).value->remove_ref();
        point tmp_key = (*tmp_it1).key;
        tmp_it1 = my_text.get_item(tmp_key,gt);
    }
}

void
page_contents::add_ref()
{
    my_reference_count++;
}

void
page_contents::remove_ref()
{
    if ((--my_reference_count) == 0)
    {
        delete this;
    }
}

page::page():my_contents(0)
{
}

page::page(const page_settings& the_settings,
                int the_page_number,
                const point& the_ul,
                window& the_window)
{
    my_contents = new page_contents(the_settings,the_page_number,
                                    the_ul,
                                    the_window);
	my_contents->add_ref();
}

page::page(const page& o):my_contents(o.my_contents)
{
    if (my_contents)
    {
        my_contents->add_ref();
    }
}

page::~page()
{
    if (my_contents)
    {
        my_contents->remove_ref();
    }
}

page& 
page::operator=(const page& o)
{
    if (&o == this)
    {
        return *this;
    }

    if (my_contents)
    {
        my_contents->remove_ref();
    }
    my_contents = o.my_contents;
    my_contents->add_ref();
	return *this;
}

page_settings::page_settings():
        width(window_media_default_width),height(window_media_default_height),
            top_margin(window_media_top_margin),bottom_margin(window_media_bottom_margin),
        title_ul(0.7,0.5),title_lr(8.0,1.5),
        copyright_ul(0.7, 11.0-1.25),copyright_lr(8.5 - 0.7 - 0.5,11.0 - 0.5),
        left_margin(window_media_left_margin),right_margin(window_media_right_margin)
{
}

rect 
page_settings::get_title_rect(const size& the_ppi) const
{
    point tmp_ul = point((int)(title_ul.x * (double)the_ppi.x),
                         (int)(title_ul.y * (double)the_ppi.y));
    point tmp_lr = point((int)(title_lr.x * (double)the_ppi.x),
                         (int)(title_lr.y * (double)the_ppi.y));
    return rect(tmp_ul,tmp_lr);
}

rect 
page_settings::get_copyright_rect(const size& the_ppi) const
{
    point tmp_ul = point((int)(copyright_ul.x * (double)the_ppi.x),
                         (int)(copyright_ul.y * (double)the_ppi.y));
    point tmp_lr = point((int)(copyright_lr.x * (double)the_ppi.x),
                         (int)(copyright_lr.y * (double)the_ppi.y));
    return rect(tmp_ul,tmp_lr);
}

void page_settings::update_settings()
{
    copyright_ul.x = left_margin;
    copyright_ul.y = height - bottom_margin;
    copyright_lr.x = width - left_margin - 0.5;
    copyright_lr.y = height - 0.5;
    title_ul.x = 0.7;
    title_ul.y = 0.5;
    title_lr.x = width - 0.5;
    title_lr.y = 1.5;
}

page_contents::page_contents(const page_settings& the_settings,
                    int the_page_number,
                    const point& the_ul,
                    window& the_window):
    my_ppi(the_window.pixels_per_inch()),
    my_page_number(the_page_number),
    my_settings(the_settings),
    my_ul(the_ul),
    my_window(&the_window),
    my_should_erase(true)
{
    ++class_instance_count;
}

point
page_contents::lr_drawing_point() const
{
    int tmp_bottom_pixels = (int)((double)my_ppi.y * my_settings.bottom_margin);
    int tmp_right_pixels = (int)((double)my_ppi.y * my_settings.right_margin);
    return get_lr() - point(tmp_right_pixels,tmp_bottom_pixels);
}

point 
page_contents::ul_drawing_point() const
{
    int tmp_top_pixels = (int)((double)my_ppi.y * my_settings.top_margin);
    int tmp_left_pixels = (int)((double)my_ppi.x * my_settings.left_margin);
    int tmp_title_pixels = (int)((double)my_ppi.y * my_settings.title_ul.y);
    return my_ul + point(tmp_left_pixels,tmp_top_pixels + tmp_title_pixels);
}

rect 
page_contents::get_title_rect() const
{
    rect tmp_title_rect = my_settings.get_title_rect(my_ppi);
    tmp_title_rect.ul += my_ul;
    tmp_title_rect.lr += my_ul;
    return tmp_title_rect;
}

rect
page_contents::get_copyright_rect() const
{
    rect tmp_copyright_rect = my_settings.get_copyright_rect(my_ppi);
    tmp_copyright_rect.ul += my_ul;
    tmp_copyright_rect.lr += my_ul;
    return tmp_copyright_rect;
}

void
page_contents::add_copyright(const string& the_string)
{
    rect tmp_copyright_rect = get_copyright_rect();
    debug_param("Copyright UL",tmp_copyright_rect.ul.y);
    point tmp_center = tmp_copyright_rect.center();
    array<string> tmp_strings = the_string.split("\n");
    if (tmp_strings.get_size() == 0)
    {
        tmp_strings.add_to_end(the_string);
    }
    int i;
    int tmp_point_base = (int)((double)window_media_copyright_size * iabc::window_media_default_yscale);

    for (i = 0;i < tmp_strings.get_size();++i)
    {
        string tmp_string = tmp_strings[i];
        if (tmp_string.length() < 1)
        {
            continue;
        }
        if (i > 0)
        {
            tmp_point_base = window_media_copyright_size - 2;
        }
        int tmp_point_size = tmp_point_base;
        text_figure* tmp_figure = new text_figure(font::serifs,tmp_point_size,
                    *my_window,font::slanty);
        tmp_figure->add_ref();
        tmp_figure->set_string(tmp_string);
        
        rect tmp_figure_rect = tmp_figure->get_current_rect();
        add_text_figure(point(tmp_center.x - tmp_figure_rect.width()/2,
                              tmp_center.y),*tmp_figure);
        tmp_center.y += tmp_figure->get_current_rect().height();
        tmp_figure->remove_ref();
    }
}

void 
page_contents::add_title(const string& the_string)
{
    rect tmp_title_rect = get_title_rect();
    point tmp_center = tmp_title_rect.center();
    array<string> tmp_strings = the_string.split("\n");
    if (tmp_strings.get_size() == 0)
    {
        tmp_strings.add_to_end(the_string);
    }
    int i;
    int tmp_point_base = window_media_title_size;

    for (i = 0;i < tmp_strings.get_size();++i)
    {
        string tmp_string = tmp_strings[i];
        if (tmp_string.length() < 1)
        {
            continue;
        }
        if (i > 0)
        {
            tmp_point_base = window_media_title_size - 2;
        }
        int tmp_point_size = (tmp_point_base * tmp_title_rect.height() ) / get_ppi().y ;
        text_figure* tmp_figure = new text_figure(font::serifs,tmp_point_size,
                    *my_window,font::slanty);
        tmp_figure->add_ref();
        tmp_figure->set_string(tmp_string);
        
        rect tmp_figure_rect = tmp_figure->get_current_rect();
        add_text_figure(point(tmp_center.x - tmp_figure_rect.width()/2,
                              tmp_center.y),*tmp_figure);
        tmp_center.y += tmp_figure->get_current_rect().height();
        tmp_figure->remove_ref();
    }
}

point 
page_contents::get_lr() const
{
    int tmp_bottom = my_ul.y + 
        (int)(my_settings.height * (double)my_ppi.y );
    int tmp_right = my_ul.x + 
        (int)(my_settings.width * (double)my_ppi.x );

    return point(tmp_right,tmp_bottom);
}

void
page_contents::add_figure(const point_ordinal& the_point,
                          figure& the_figure)
{
    lock tmp_lock(my_mutex);
    the_figure.add_ref();
    point tmp_point = point(the_point);
    the_figure.dispatch_draw_self(*my_window,tmp_point + my_offset);
    map<point_ordinal,figure_array>::iterator tmp_it = 
        my_figures.get_item(the_point,exact);

    // Replace it if there's a figure there already.
    if (tmp_it)
    {
        figure_array tmp_array = (*tmp_it).value;
        tmp_array.add_to_end(&the_figure);
    }
	else
	{
		figure_array tmp_array;
        tmp_array.add_to_end(&the_figure);
        my_figures.add_pair(the_point,tmp_array);
	}
}

void 
page_contents::add_text_figure(const string& the_string,const point& the_point,text_figure& the_figure)
{
    // Side-effect: getting the rectangle adds the string to the figure
    // and initializes all the data.
    the_figure.get_rect(the_string);
    add_text_figure(the_point,the_figure);
}

void 
page_contents::add_text_figure(const point& the_point,
                               text_figure& the_figure)
{
    lock tmp_lock(my_mutex);
    the_figure.add_ref();
    the_figure.dispatch_draw_self(the_point + my_offset,false);
    map<point_ordinal,text_figure*>::iterator tmp_it = 
        my_text.get_item(the_point);
    if (tmp_it)
    {
        (*tmp_it).value->remove_ref();
    }
    my_text.add_pair(the_point,&the_figure);
}

void 
page_contents::redraw(const rect& the_rect)
{
    lock tmp_lock(my_mutex);

    // Clear out the rectangle
    if (my_should_erase)
    {
        pen* tmp_clear = my_window->get_pen(255,255,255,1);
        tmp_clear->res_add_ref();
        my_window->draw_rect(*tmp_clear,the_rect,true);
        tmp_clear->res_remove_ref();
    }

    // First draw the figures.  Figure out (no pun intended) the
    // real UL corner of the figure, to determine if it fits in the
    // rectangle.  This is not the same as the render point.
    point tmp_key = the_rect.ul;
    map<point_ordinal,figure_array>::iterator
        tmp_it = my_figures.get_item(tmp_key,gteq);

    while (tmp_it)
    {
        figure_array tmp_figure_array = (*tmp_it).value;
        point tmp_figure_point = (*tmp_it).key;
        int i;
        for (i = 0;i < tmp_figure_array.get_size();++i)
        {
            figure* tmp_figure = tmp_figure_array[i];
            rect tmp_figure_rect = tmp_figure->get_bounding_box(tmp_figure_point);
            if (the_rect.intersects(tmp_figure_rect) == true)
            {
                tmp_figure->dispatch_draw_self(*my_window,tmp_figure_point + my_offset,false);
            }
        }
        tmp_key = (*tmp_it).key;
        tmp_it = my_figures.get_item(tmp_key,gt);
    }

    tmp_key = the_rect.ul;
    map<point_ordinal,text_figure*>::iterator tmp_it1 = 
        my_text.get_item(tmp_key,gteq);
    while (tmp_it1)
    {
        text_figure* tmp_figure = (*tmp_it1).value;
        tmp_key = (*tmp_it1).key;
        rect tmp_rect = tmp_figure->get_current_rect();
        tmp_rect = tmp_rect.offset(tmp_key);
        if (the_rect.intersects(tmp_rect) == true)
        {
            tmp_figure->dispatch_draw_self(tmp_key + my_offset);
        }

        tmp_it1 = my_text.get_item(tmp_key,gt);
    }
}

void 
page_contents::remove_figures(const point& the_point)
{
    lock tmp_lock(my_mutex);
    map<point_ordinal,figure_array>::iterator tmp_it = 
        my_figures.get_item(the_point,exact);

    // Replace it if there's a figure there already.
    if (tmp_it)
    {
        int i;
        figure_array tmp_array = (*tmp_it).value;
        for (i = 0;i < tmp_array.get_size();++i)
        {
            figure* tmp_figure = tmp_array[i];
            tmp_figure->remove_ref();
        }
        my_figures.remove(the_point,exact);
    }
}

// METHOD: remove_text_figures
// DESCRIPTION:
// Remove all text figures at the given point
void 
page_contents::remove_text_figures(const point& the_point)
{
    lock tmp_lock(my_mutex);
    map<point_ordinal,text_figure*>::iterator tmp_it = 
        my_text.get_item(the_point,exact);
    if (tmp_it)
    {
        text_figure* tmp_figure = (*tmp_it).value;
        my_text.remove(the_point,exact);
        tmp_figure->remove_ref();
    }
}

void 
page_contents::add_offset(const point& the_offset)
{
    my_offset = the_offset;
}

page_contents*
page_contents::clone(window& w,const rect& source,const rect& target)
{
    lock tmp_lock(my_mutex);
    page_contents* tmp_rv = new 
        page_contents(this->my_settings,
                    this->my_page_number,
                    target.ul,
                    w);

    // We need to refigure all the figures and text figures.
    // Figures first.  Clone the figures based on the rectangles
    // and then stretch the points where they will be drawn.
    point tmp_key;
    map<point_ordinal,figure_array>::iterator
        tmp_it = my_figures.get_item(tmp_key,gteq);

    while (tmp_it)
    {
        figure_array tmp_figure_array = (*tmp_it).value;
        point tmp_figure_point = (*tmp_it).key;
        int i;
        for (i = 0;i < tmp_figure_array.get_size();++i)
        {
            figure* tmp_figure = tmp_figure_array[i];
            tmp_figure_point = stretch_point(tmp_figure_point,source,target);
            figure* tmp_new = tmp_figure->clone(source,target,w.pixels_per_inch());
            tmp_rv->add_figure(tmp_figure_point,*tmp_figure);
        }
        tmp_key = (*tmp_it).key;
        tmp_it = my_figures.get_item(tmp_key,gt);
    }
    
    // Now do the same thing with the text figures
    tmp_key = point(0,0);
    map<point_ordinal,text_figure*>::iterator tmp_it1 = 
        my_text.get_item(tmp_key,gteq);
    while (tmp_it1)
    {
        text_figure* tmp_figure = (*tmp_it1).value;
        tmp_key = (*tmp_it1).key;
        point tmp_new_point = stretch_point(tmp_key,source,target);
        text_figure* tmp_new = 
            tmp_figure->clone(w,source,target);
		tmp_rv->add_text_figure(tmp_new_point,*tmp_figure);
        tmp_it1 = my_text.get_item(tmp_key,gt);
    }

	return tmp_rv;
}

page 
page::clone(window& w,const rect& source,const rect& target)
{
    page_contents* tmp_contents = my_contents->clone(w,source,target);
    page tmp_page;
    tmp_page.my_contents = tmp_contents;
    tmp_contents->add_ref();
    return tmp_page;
}

paged_window::paged_window(const point& the_ul,window& the_window):
my_ul(the_ul),my_window(the_window),
	my_page_seperation(15),
	my_page_margin(0),
my_ppi(the_window.pixels_per_inch()),
    my_last_page(1)
{
    page_settings  tmp_initial_page_settings;
    page tmp_page(tmp_initial_page_settings,
                    1,
                    my_ul,
					my_window);

    my_pages.add_pair(1,tmp_page);
    my_window.set_logical_size(tmp_page.get_lr());
}

paged_window* 
paged_window::clone(window& target,const scale& the_scale)
{
    paged_window* tmp_rv = new paged_window(my_ul,target);
    int i;
    point tmp_ul;
    double tmp_x_scale = the_scale.x;
    tmp_x_scale *= (double)my_ppi.x / (double)target.pixels_per_inch().x;
    double tmp_y_scale = the_scale.y;
    tmp_y_scale *= (double)my_ppi.y / (double)target.pixels_per_inch().y;
    for (i = 0;i < my_pages.get_size();++i)
    {
        page tmp_page = (*my_pages.get_item(i + 1)).value;
        rect tmp_rect = rect(tmp_page.get_ul(),tmp_page.get_lr());
        int tmp_current_width = tmp_rect.width();
        int tmp_current_height = tmp_rect.height();
        int tmp_new_width = (int)((double)tmp_rect.width() * tmp_x_scale);
        int tmp_new_height = (int)((double)tmp_rect.height() * tmp_y_scale);
        rect tmp_new_rect = rect(tmp_ul,point(tmp_ul.x + tmp_new_width,
                                              tmp_ul.y + tmp_new_height));
        page tmp_new_page = tmp_page.clone(target,tmp_rect,tmp_new_rect);
        (*tmp_rv).my_pages.add_pair(i + 1,tmp_new_page);
    }
    tmp_rv->my_last_page = my_last_page;
    tmp_rv->my_page_margin = my_page_margin;
    tmp_rv->my_page_seperation = my_page_seperation;
    tmp_rv->my_ppi = target.pixels_per_inch();

    return tmp_rv;
}

int
paged_window::get_page_number(const point& the_point)
{
    map<int,page>::iterator tmp_it = my_pages.get_item(my_last_page);
    
    if (tmp_it)
        {
        page tmp_page = (*tmp_it).value;
        if (is_in_rect(the_point,tmp_page,false) == true)
            {
            return my_last_page;
            }
        }

    my_last_page = 1;
    tmp_it = my_pages.get_item(my_last_page);
    while (tmp_it)
        {
        page tmp_page = (*tmp_it).value;
        if (is_in_rect(the_point,tmp_page,false) == true)
            {
            return my_last_page;
            }
		++my_last_page;
	    tmp_it = my_pages.get_item(my_last_page);
        }

    return 0;
}

int
paged_window::get_page_number(const rect& the_rect)
{
    map<int,page>::iterator tmp_page = my_pages.get_item(my_last_page);
    
    if (tmp_page)
        {
        page tmp_dim = (*tmp_page).value;
        rect tmp_rect = rect(tmp_dim.get_ul(),tmp_dim.get_lr());
        if (tmp_rect.intersects(the_rect) == true)
            {
            return my_last_page;
            }
        }

    my_last_page = 1;
    tmp_page = my_pages.get_item(my_last_page);
    while (tmp_page)
        {
        page tmp_dim = (*tmp_page).value;
        rect tmp_rect = rect(tmp_dim.get_ul(),tmp_dim.get_lr());
        if (tmp_rect.intersects(the_rect) == true)
            {
            return my_last_page;
            }
		++my_last_page;
	    tmp_page = my_pages.get_item(my_last_page);
        }

    return 0;
}

void 
paged_window::change_settings(int the_page,const page_settings& the_settings)
{
    map<int,page>::iterator tmp_page = my_pages.get_item(the_page,
                                                           exact);
    
    page_settings tmp_settings;
    // First update the dimensions of this page.
    if (tmp_page)
    {
        page tmp_dimensions = (*tmp_page).value;
        tmp_dimensions.change_settings(the_settings);
    }
    else
    {
        return; // not found
    }

    // Now go through all the pages and change all of them that use the old
    // settings.
    point tmp_lr;  // Save the lr for reset of window size
    for (int i =0;i < get_number_pages();++i)
    {
        tmp_page = my_pages.get_item(the_page,exact);
		if (tmp_page)
			tmp_lr = (*tmp_page).value.get_lr();
        if ((tmp_page) &&
            ((*tmp_page).value.get_settings() == tmp_settings))
        {
            page tmp_p(tmp_settings,
                            the_page,
                            my_ul,
                            my_window);
            (*tmp_page).value = tmp_p;
        }
    }
    my_window.set_logical_size(tmp_lr);
}

void 
paged_window::add_page(int the_page)
{
    map<int,page>::iterator tmp_page = my_pages.get_item(the_page,
                                                           lteq);

    if (tmp_page)
    {
        page tmp_current = (*tmp_page).value;
        point tmp_new_ul = point(0,tmp_current.get_lr().y) + 
            point(0,(the_page - 1) * my_page_seperation) +
            point(0,my_page_margin) + my_ul;

        page tmp_new((tmp_current.get_settings()),
                    the_page,
                    tmp_new_ul,
                    my_window);

        debug_param("Page = ",the_page);
        debug_param("UL Drawing y = ",tmp_new.ul_drawing_point().y);
        debug_param("LR Drawing y = ",tmp_new.lr_drawing_point().y);
        my_window.set_logical_size(tmp_new.get_lr());
        my_pages.add_pair(the_page,tmp_new);
        if (my_last_page < the_page)
        {
            my_last_page = the_page;
        }
    }    
}

point 
paged_window::get_page_ul(int the_page)
{
    map<int,page>::iterator tmp_it = 
        my_pages.get_item(the_page,exact);

    if (tmp_it)
    {
        page tmp_dim = (*tmp_it).value;
        return tmp_dim.ul_drawing_point();
    }

    return point();
}

bool 
paged_window::is_in_rect(const point& the_point,const page& the_page,bool the_restrict)
{
    bool tmp_rv = false;
    point tmp_ul;
    if (the_restrict)
        {
        tmp_ul = the_page.ul_drawing_point();
        }
    else
        {
        tmp_ul = the_page.get_ul();
        }

    rect tmp_rect(tmp_ul,the_page.get_lr());
    if (tmp_rect.contains(the_point) == true)
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

int 
paged_window::get_number_pages() 
{
    return my_pages.get_size();
}

void
paged_window::print_page(int the_page)
{
    page tmp_page = get_page(the_page);
    rect tmp_source_rect = get_page_rect(the_page);
    tmp_page.allow_erase(false);
    tmp_page.add_offset(tmp_source_rect.ul * -1);
    tmp_page.redraw(tmp_source_rect);
}

void
paged_window::refresh(const rect& the_rect)
{
    int i = 0;
    for (;i < my_pages.get_size();++i)
    {
        rect tmp_page_rect = get_page_rect(i + 1);
        if (tmp_page_rect.intersects(the_rect))
        {
            page tmp_page = get_page(i + 1);
            tmp_page.redraw(the_rect.intersection(tmp_page_rect));
        }
    }
}

bool
paged_window::is_in_drawing_area(const point& the_point,int the_page)
{
    map<int,page>::iterator tmp_it = 
        my_pages.get_item(the_page);

    bool tmp_rv = false;
    if (tmp_it)
    {
        page tmp_dim = (*tmp_it).value;
        tmp_rv = is_in_rect(the_point,tmp_dim,true);
    }

    return tmp_rv;
}

rect
paged_window::get_title_rect(int the_page)
{
    map<int,page>::iterator tmp_page = 
        my_pages.get_item(the_page);
    
    if (tmp_page)
    {
        page tmp_dim = (*tmp_page).value;
        return tmp_dim.get_title_rect();
    }

    return rect();
}

page
paged_window::get_page(int the_page)
{
    map<int,page>::iterator tmp_page = 
        my_pages.get_item(the_page);
    
    if (tmp_page)
    {
        page tmp_dim = (*tmp_page).value;
        return tmp_dim;
    }

    return page();
}

rect 
paged_window::get_page_rect(int the_page)
{
    map<int,page>::iterator tmp_page = 
        my_pages.get_item(the_page);
    
    if (tmp_page)
    {
        page tmp_dim = (*tmp_page).value;
        return rect(tmp_dim.get_ul(),tmp_dim.get_lr());
    }

    return rect();
}

paged_window::~paged_window()
{
}

}
