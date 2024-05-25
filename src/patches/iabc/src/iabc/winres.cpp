/*
 * win_res.cpp - Resources for handling windows, including windows themselves.
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
#include "iabc/winres.h"
#include "iabc/list.cpp"
#include "iabc/map.cpp"
#include "iabc/drawtemp.cpp"
#include "iabc/array.cpp"

namespace iabc
{
using namespace iabc;
int
window_res::class_unused_id = 0;

mutex
window_res::class_mutex;

list<window_res*>
window_res::res_list;

int window_res::class_instance_count = 0;

void window_res::res_remove_ref()
{
    if (--my_count == 0)
    {
        delete this;
    }
}
window_res::~window_res()
{
    lock tmp_lock(class_mutex);
    remove_found_ref(res_list,this);
    --class_instance_count;
}

void
window_res::res_add_ref()
{
    ++my_count;
}

window_res::window_res()
{
    lock tmp_lock(class_mutex);
    add_at_beginning(res_list,this);
    my_id = ++class_unused_id;
    ++class_instance_count;
}

list<font*> font::class_list;
mutex font::class_mutex;

int font::font_count = 0;

font::font(canvas& c,typeface family, int point_size,style the_style):
        window_res(),my_family(family),my_point_size(point_size),
        my_canvas(c),my_style(the_style)
{
    lock tmp_lock(class_mutex);
    add_at_beginning(font::class_list,this);

    // Once we create a font, there is really no point in deleting
    // it, or we end up deleting and recreating fonts over and over.
    res_add_ref();
    ++font_count;
}


font::~font()
{
    lock tmp_lock(class_mutex);
    remove_found_ref(class_list,this);
    --font_count;
}

font*
font::get_font(canvas& c,typeface family, int point_size,font::style the_style)
{

    font* tmp_rv = get_font_from_list(c,family,point_size,the_style);
    if (tmp_rv == 0)
    {
        font_data tmp_data(&c,family,point_size,the_style);
        tmp_rv = factory<font,font_data*>::create(&tmp_data);
    }
    return tmp_rv;
}

font*
font::get_font_from_list(canvas& c,typeface family, int point_size,font::style the_style)
{
    lock tmp_lock(font::class_mutex);

    font_data tmp_data(&c,family,point_size,the_style);
    list<font*>::iterator tmp_font = findit_deref(class_list,tmp_data);
    if (tmp_font != 0)
    {
        return (*tmp_font);
    }
    return 0;
}


void
font::clear_all_fonts_for_canvas(canvas& c)
{
    lock tmp_lock(font::class_mutex);
    list<font*>::iterator tmp_font = class_list.first();
    while (tmp_font)
    {
        font* tmp_d = (*tmp_font);
        if (&(tmp_d->my_canvas) == &c)
        {
            tmp_font.delete_current();
            tmp_d->res_remove_ref();
        }
        else
            tmp_font.next();
    }
}
bool
font::operator==(const font_data& o)
{
    return ((o.c == &my_canvas) &&
        (o.family == my_family) &&
        (o.point_size == my_point_size) &&
         (o.my_style == my_style));
}

font*
font::get_font(const font& o,int point_size,font::style the_style)
{
    return get_font(o.my_canvas,o.my_family,point_size,the_style);
}

list<pen*> pen::class_list;
mutex pen::class_mutex;

pen*
pen::get_pen(canvas& c,int r,int g,int b,int width)
{
    lock tmp_lock(pen::class_mutex);
    list<pen*>::iterator tmp_pen =
        findit_deref(class_list,pen_data(c,r,g,b,width));
    if (tmp_pen != 0)
    {
        return *tmp_pen;
    }

    pen_data data(c,r,g,b,width);
    pen* tmp_rv = factory<pen,pen_data*>::create(&data);
    return tmp_rv;
}

int pen::pen_count = 0;

pen::pen(canvas& c,int r,int g,int b,int width):
        my_canvas(c),window_res(),my_width(width),
        my_red(r),my_green(g),my_blue(b)
{
    lock tmp_lock(class_mutex);
    add_at_beginning(class_list,this);
    ++pen_count;

}

pen::~pen()
{
    lock tmp_lock(class_mutex);
    remove_found_ref(class_list,this);
    --pen_count;
}

void
pen::clear_all_pens_for_canvas(canvas& c)
{
    lock tmp_lock(pen::class_mutex);
    list<pen*>::iterator tmp_pen = class_list.first();
    while (tmp_pen)
    {
        pen* tmp_d = (*tmp_pen);
        if (&(tmp_d->my_canvas) == &c)
        {
            tmp_pen.delete_current();
            tmp_d->res_remove_ref();
        }
        else
            tmp_pen.next();
    }
}

list<window*>
window::class_window_list;

mutex
window::class_mutex;

window::linear_xform::linear_xform(double the_m,double the_b):
m(the_m),b(the_b)
{};

window::linear_xform::linear_xform(const linear_xform& o):
m(o.m),b(o.b)
{};


point
window::xform_point(const point& o) const
{
    int p1 = x_xform.xform_scalar(o.x);
    int p2 = y_xform.xform_scalar(o.y);
    return point(p1,p2);
}

point
window::inverse_xform_point(const point& o) const
{
    int p1 = x_xform.inverse_xform_scalar(o.x);
    int p2 = y_xform.inverse_xform_scalar(o.y);
    return point(p1,p2);
}

rect
window::xform_rect(const rect& o) const
{
    point p1 = xform_point(o.ul);
    point p2 = xform_point(o.lr);
    rect r(p1,p2);
    return r;
}

rect
window::inverse_xform_rect(const rect& o) const
{
    point p1 = inverse_xform_point(o.ul);
    point p2 = inverse_xform_point(o.lr);
    rect r(p1,p2);
    return r;
}

int
window::scale_point_size(int the_point_size)
{
    // OK, I can't figure out how fonts work in Wx windows, it seems that
    // the point size is MM_TEXT mode is based on a ppi, even though its really
    // supposed to be points, oh well.  So this number looks good on my screen and
    // should scale to a printer pretty well, I hope it works in X.
    double tmp_scale = (double)the_point_size/(double)96;
    double tmp_ppi = (double)pixels_per_inch().y;
    the_point_size = (int)(tmp_ppi * tmp_scale);
    return the_point_size;
}

bool window::window_predicate::operator()(window* the_window)
{
    return the_window->my_name == my_string;
}

window*
window::get_window(const string& window_name)
{
    string tmp_name = window_name;
    lock tmp_lock(window::class_mutex);
    window_predicate tmp_predicate(tmp_name);
    list<window*>::iterator tmp_ptr =
        findit_predicate(window::class_window_list,tmp_predicate);

    if (tmp_ptr != 0)
        return *tmp_ptr;
    return 0;
}


void
window::draw_rect(pen& the_pen,const rect& the_rect,bool fill)
{
    raw_draw_rect(the_pen,xform_rect(the_rect),fill);
}

void
window::draw_line(pen& the_pen,const line& the_line)
{
    raw_draw_line(the_pen,
        line(xform_point(the_line.p1),xform_point(the_line.p2)));
}

void
window::draw_text(font& the_font,const string& the_string)
{
    int tmp_size = the_font.get_point_size();
    font* new_font = font::get_font(the_font,(int)((double)tmp_size * x_xform.m),font::normal);
    new_font->res_add_ref();
    raw_draw_text(the_font,the_string);
    new_font->res_remove_ref();
}

void
window::click(const point& p)
{
    if (my_document)
    {
        my_document->click(xform_point(p));
    }
}

void
window::refresh(const rect& tmp_rect)
{
    raw_refresh(xform_rect(tmp_rect));
}

void window::refresh()
{
    refresh(this->get_visible_rect());
}

void
window::push_context()
{
    lock tmp_lock(my_mutex);
    add_at_beginning(my_context_list,display_context(x_xform,y_xform,my_current_pos));
}

void
window::pop_context()
{
    lock tmp_lock(my_mutex);
    if (my_context_list.get_size() > 0)
    {
        display_context tmp_ref;
        remove_first_ref(my_context_list,tmp_ref);
        x_xform = tmp_ref.x_xform;
        y_xform = tmp_ref.y_xform;
    }
}

void
window::moveto(const point& p)
{
    my_current_pos = xform_point(p);
    my_current_pos -= this->my_scroll_point;
}

void
window::moveto_relative(const point& p)
{
    point p1 = xform_point(p);
    my_current_pos += p1;
}

window::window(const string& name):
my_name(name),x_xform(1.0,0.0),y_xform(1.0,0.0),my_document(0),
    my_thumb_scroll_size(1)
{
    lock tmp_lock(window::class_mutex);
    add_at_beginning(window::class_window_list,this);
}

window::~window()
{
    lock tmp_lock(window::class_mutex);
    remove_found_ref(window::class_window_list,this);
    font::clear_all_fonts_for_canvas(*this);
    pen::clear_all_pens_for_canvas(*this);
}
}


