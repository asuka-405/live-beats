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
#ifndef iabc_win_page_h
#define iabc_win_page_h

#include "iabc/winres.h"
#include "iabc/map.h"
#include "iabc/figure.h"
#include "iabc/text_figure.h"

namespace iabc
{
;
// FILE: win_page.h
// DESCRIPTION:
// Classes to help manage the dimensions of the pages, without any windows-specific
// stuff in it.

// CLASS: figure_container
// DESCRIPTION:
// This is the interface that describes how the pages hold the figures.
class figure_container_if
{
public:
    // METHOD: add_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.  Note that the point may not be
    // the same as the UL location where the figure is located on the window, since
    // some figures have the absolute coordinates and some have relative.  The point
    // where the figure is to be drawn is in the point part of the point-figure pair.
    virtual void add_figure(const point_ordinal& the_point,
                    figure& the_figure) = 0;
    
    // METHOD: add_text_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.
    virtual void add_text_figure(const point_ordinal& the_point,text_figure& the_figure) = 0;

    // METHOD: add_text_figure
    // DESCRIPTION:
    // Add the figure with the given string, and sort on the given point.
    virtual void add_text_figure(const string& the_string,const point_ordinal& the_point,text_figure& the_figure) = 0;
};

// CLASS: figure_container
// DESCRIPTION:
// Allow other classes to act like a page class. This allows us to
// implement the same interface as a page without having to inherit from
// anything.
template <class container>
class figure_container:public figure_container_if
{
public:
    figure_container(container& the_container){my_container = &the_container;};
    void add_figure(const point_ordinal& the_point,
                    figure& the_figure)
    {my_container->add_figure(the_point,the_figure);};
    void add_text_figure(const point_ordinal& the_point,text_figure& the_figure)
    {my_container->add_text_figure(the_point,the_figure);};
    void add_text_figure(const string& the_string,const point_ordinal& the_point,text_figure& the_figure)
    {the_figure.set_string(the_string);
     my_container->add_text_figure(the_point,the_figure);};

private:
    container* my_container;
};

// CLASS: page_settings
// DESCRIPTION:
// A structure to keep track of the settings of one page.  By settings I mean
// page size, things like that.
class page_settings
{
public:
    page_settings();
    bool operator==(const page_settings& o) const
    {
        return ((width == o.width) &&
                (height == o.height) &&
                (top_margin == o.top_margin) &&
                (bottom_margin == o.bottom_margin) &&
                (left_margin == o.left_margin) &&
                (right_margin == o.right_margin) &&
                (title_ul == o.title_ul) &&
                (title_lr == o.title_lr) &&
                (copyright_lr == o.copyright_lr) &&
                (copyright_ul == o.copyright_ul));
    };
    // Update the title and copyright rectangles based on new
    // page settings.
    void update_settings();
    double width;
    double height;
    double top_margin;
    double bottom_margin;
    double left_margin;
    double right_margin;
    scale title_ul;
    scale title_lr;
    scale copyright_ul;
    scale copyright_lr;
    rect get_title_rect(const size& the_ppi) const;
    rect get_copyright_rect(const size& the_ppi) const;
};

typedef array<figure*> figure_array;

// CLASS: page_contents
// DESCRIPTION:
// This is all the figures in a page, along with information about
// the page itself.
class page;
class page_contents
{
public:
    friend class page;
    page_contents(const page_settings& the_settings,
                    int the_page_number,
                    const point& the_ul,
                    window& the_window);
    ~page_contents();
    void add_ref();
    void remove_ref();
    point ul_drawing_point() const;
    point lr_drawing_point() const;
    size get_ppi() {return my_window->pixels_per_inch();};
    window& get_window() {return *my_window;};
    point get_lr() const;
    rect get_title_rect() const;
    rect get_copyright_rect() const;
    point get_ul() const {return my_ul;};
    page_settings get_settings() const{return my_settings;};
    void change_settings(const page_settings& the_settings){my_settings = the_settings;};

    // METHOD: add_title
    // DESCRIPTION:
    // Add the title to this page.
    void add_title(const string& the_string);

    // METHOD: add_copyright
    // DESCRIPTION:
    // Add the copyright information to the page.
    void add_copyright(const string& the_string);

    // METHOD: add_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.  Note that the point may not be
    // the same as the UL location where the figure is located on the window, since
    // some figures have the absolute coordinates and some have relative.  The point
    // where the figure is to be drawn is in the point part of the point-figure pair.
    void add_figure(const point_ordinal& the_point,
                    figure& the_figure);
    
    // METHOD: add_text_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.
    void add_text_figure(const point& the_point,text_figure& the_figure);

    // METHOD: add_text_figure
    // DESCRIPTION:
    // Add the figure with the given string, and sort on the given point.
    void add_text_figure(const string& the_string,const point& the_point,text_figure& the_figure);

    // METHOD: redraw
    // DESCRIPTION:
    // Redraw all figures who's bounding rectangle intersects the_rect.
    void redraw(const rect& the_rect);

    // METHOD: remove_figures
    // DESCRIPTION:
    // Remove all figures at the given point.
    void remove_figures(const point& the_point);

    // METHOD: remove_text_figures
    // DESCRIPTION:
    // Remove all text figures at the given point
    void remove_text_figures(const point& the_point);

    // METHOD: clone
    // DESCRIPTION:
    // Re-render all the figures on the given window, by stretching/
    // compacting the figures based on the source and target rectangles.
    page_contents* clone(window& w,const rect& source,const rect& target);

    // METHOD: allow_erase
    // DESCRIPTION:
    // Disallow erasing the image before draw, as on a print.
    void allow_erase(bool the_should_erase){my_should_erase = the_should_erase;};

    // METHOD: add_offset
    // DESCRIPTION:
    // Add the offset to the figures before they're drawn, as on a redraw.
    void add_offset(const point& the_offset);
    int get_page_number() const {return my_page_number;};
private:
    page_contents& operator=(const page_contents& o);
    page_contents(const page_contents& o);
    size my_ppi;
    window* my_window;
    int my_page_number;
    point my_ul;
    // ATTRIBUTE: my_should_erase
    // DESCRIPTION:
    // True if we should erase before redrawing.  False on
    // printing since there's nothing to erase on a blank
    // page.
    bool my_should_erase;
    point my_offset;

    typedef array<text_figure> text_figure_ptr_array;
    map<point_ordinal,figure_array> my_figures;
    map<point_ordinal,text_figure*> my_text;
    page_settings my_settings;
    atomic_counter my_reference_count;
    mutex my_mutex;
};

// CLASS: page
// DESCRIPTION:
// This is the interface to page_contents, which includes copy
// operators and reference counting.
class paged_window;
class page
{
public:
    friend class paged_window;
    page();
    page(const page_settings& the_settings,
                    int the_page_number,
                    const point& the_ul,
                    window& the_window);
    page(const page& o);
    ~page();
    page& operator=(const page& o);
    point ul_drawing_point() const{return my_contents->ul_drawing_point();};
    point lr_drawing_point() const{return my_contents->lr_drawing_point();};
    void allow_erase(bool the_should_erase){my_contents->allow_erase(the_should_erase);};
    void add_offset(const point& the_offset){my_contents->add_offset(the_offset);};

    size get_ppi() {return my_contents->get_ppi();};
    window& get_window() {return *(my_contents->my_window);};
    point get_lr() const{return my_contents->get_lr();};
    rect get_title_rect() const{return my_contents->get_title_rect();};
    point get_ul() const {return my_contents->get_ul();};
    page_settings get_settings() const{return my_contents->my_settings;};
    void change_settings(const page_settings& the_settings)
    {my_contents->change_settings(the_settings);};
    void add_title(const string& the_string)
    {my_contents->add_title(the_string);};
    void add_copyright(const string& the_string)
    {my_contents->add_copyright(the_string);};

    int get_page_number() const {return my_contents->get_page_number();};

    // METHOD: add_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.  Note that the point may not be
    // the same as the UL location where the figure is located on the window, since
    // some figures have the absolute coordinates and some have relative.  The point
    // where the figure is to be drawn is in the point part of the point-figure pair.
    void add_figure(const point_ordinal& the_point,
                    figure& the_figure)
    {my_contents->add_figure(the_point,the_figure);};
    
    void remove_figure(const point& the_point){my_contents->remove_figures(the_point);};
    void remove_text_figures(const point& the_point){my_contents->remove_text_figures(the_point);};

    // METHOD: add_text_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.
    void add_text_figure(const point& the_point,text_figure& the_figure)
    {my_contents->add_text_figure(the_point,the_figure);};

    // METHOD: add_text_figure
    // DESCRIPTION:
    // Add the figure, and sort on the given point.
    void add_text_figure(const string& the_string,const point& the_point,text_figure& the_figure)
    {my_contents->add_text_figure(the_string,the_point,the_figure);};

    // METHOD: redraw
    // DESCRIPTION:
    // Redraw all figures who's bounding rectangle intersects the_rect.
    void redraw(const rect& the_rect){my_contents->redraw(the_rect);};
    page clone(window& w,const rect& source,const rect& target);
private:
    page_contents* my_contents;
};

// DESCRIPTION:
// Based on an absolute pixel offset, figure out which page we're on, or vise versa.
// Return the important parts of a page like the title rectangle
class paged_window
{
public:
    paged_window(const point& the_ul,window& the_window);
	~paged_window();
    void change_settings(int the_page,const page_settings& the_settings);

    // METHOD: add_page
    // DESCRIPTION:
    // Add the page to the array, if it doesn't exist.
    void add_page(int the_page);

    // METHOD: get_stuff
    // DESCRIPTION:
    // Given a page number, 
    point get_page_ul(int the_page);
    rect get_page_rect(int the_page);
    rect get_title_rect(int the_page);
    page get_page(int the_page);

    // METHOD: clone
    // DESCRIPTION:
    // Create all new page objects using the target window and
    // scale to get the dimensions to draw on.
    paged_window* clone(window& target,const scale& the_scale);

    // METHOD: get_more stuff
    // DESCRIPTION:
    // We can also take a point and figure out which page we're on.
    int get_page_number(const point& the_point);
    int get_page_number(const rect& the_rect);
    bool is_in_drawing_area(const point& the_point,int the_page);
    int get_number_pages();

    // METHOD: refresh
    // DESCRIPTION:
    // Iterate through all the pages, and refresh any pages that intersect
    // with the_rect
    void refresh(const rect& the_rect);

    // METHOD: print_page
    // DESCRIPTION:
    // Render the page, but fix the UL to be 0,0
    void print_page(int the_page);
public:
    bool is_in_rect(const point& the_point,const page& the_page,bool the_restrict);

private:
    // Page seperation in pixels
    int my_page_seperation;
    int my_page_margin;

    point my_ul;
    size my_ppi;
    window& my_window;
    map<int,page> my_pages;
    int my_last_page;
};

}

#endif
