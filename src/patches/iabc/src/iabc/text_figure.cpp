/*
 * text_figure.cpp - Draws a text at the right place in the right thread.
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
#include "iabc/text_figure.h"
#include "iabc/drawtemp.cpp"
#include "iabc/factory.cpp"

namespace iabc
{
;

static instance_counter<text_figure> static_figure_count;

text_figure::text_figure(font::typeface the_font,
                int the_point_size,
                window& the_window,
                font::style the_style):
my_window(the_window),my_should_calculate(true)
{
    ++static_figure_count;
    my_font = the_window.get_font(the_font,
        the_window.scale_point_size(the_point_size),
        the_style);
    if (my_font)
    {
        my_font->res_add_ref();
    }
}

text_figure::~text_figure()
{
    --static_figure_count;
    if (my_font)
    {
        my_font->res_remove_ref();
    }
}

void
text_figure::remove_ref()
{
    if (--my_counter == 0) delete this;
}

text_figure*
text_figure::clone(window& w,const rect& r1,const rect& r2)
{
    int tmp_size = my_font->get_point_size();
    int tmp_new_size = (int)((double)tmp_size * ((double)r2.height() / r1.height()));
    text_figure* tmp_rv = new text_figure(
        my_font->get_typeface(),
                tmp_new_size,
                w,
                my_font->get_style());
    tmp_rv->set_string(my_string);
    return tmp_rv;
}

void
text_figure::dispatch_draw_self(const string& the_string,const point& the_origin,bool the_blocking)
{
    if (my_font)
    {
        my_string = the_string;
        my_should_calculate = true;
        draw_text_command::dispatch_draw_command(my_window,
            the_origin,*this,the_blocking);

    }
}

void
text_figure::dispatch_draw_self(const point& the_origin,bool the_blocking)
{
    if (my_font)
    {
        draw_text_command::dispatch_draw_command(my_window,
            the_origin,*this,the_blocking);

    }
}

rect
text_figure::get_rect(const string& the_string)
{
    // Don't get it if we have already.
    if ((my_string == the_string) &&
        ( my_should_calculate == false))
    {
        return my_rect;
    }

    rect tmp_rect;
    get_rect(the_string,&tmp_rect);
    return tmp_rect;
}

rect
text_figure::get_current_rect()
{
    rect tmp_rv;
    get_rect(my_string,&tmp_rv);
    return tmp_rv;
}

void
text_figure::get_rect(const string& the_string,rect* the_origin)
{
    if ((my_string == the_string) &&
        (my_should_calculate == false))
    {
        (*the_origin) = my_rect;
        return;
    }

    calculate_text_info(the_string);
    (*the_origin) = my_rect;
}

int
text_figure::get_descent(const string& the_string)
{
    if ((my_string == the_string) &&
        (my_should_calculate == false))
    {
        return my_descent;
    }

    calculate_text_info(the_string);
    return my_descent;
}
int
text_figure::get_leading(const string& the_string)
{
    if ((my_string == the_string) &&
        (my_should_calculate == false))
    {
        return my_leading;
    }

    calculate_text_info(the_string);
    return my_leading;
}

void
text_figure::calculate_text_info(const string& the_string)
{
    if ((my_should_calculate) &&
        (my_font))
    {
        my_should_calculate = false;
        my_rect = rect(point(0,0),point(0,0));
        my_string = the_string;

        my_rect = my_font->get_rect(my_string,&my_descent,&my_leading);
    }
}

void
text_figure::dispatch_draw_self(const string& the_string,const rect& the_origin,bool the_blocking)
{
    if (my_font)
    {
        my_string = the_string;
        my_should_calculate = true;
        draw_text_rect::dispatch_draw_command(my_window,
            the_origin,*this,the_blocking);

    }
}

void
text_figure::dispatch_draw_self(const rect& the_origin,bool the_blocking)
{
    if (my_font)
    {
        draw_text_rect::dispatch_draw_command(my_window,
            the_origin,*this,the_blocking);

    }
}

void
text_figure::draw_self(window& the_window,const point& the_origin)
{
    the_window.moveto(the_origin);
    the_window.draw_text(*my_font,my_string);
}

void
text_figure::draw_self(window& the_window,const rect& the_origin)
{
    point tmp_start_point = the_origin.center();
    rect tmp_box = my_font->get_rect(my_string);
    tmp_start_point.x -= tmp_box.width()/2;
    tmp_start_point.y -= tmp_box.height()/2;

    the_window.moveto(tmp_start_point);
    the_window.draw_text(*my_font,my_string);
}

}
