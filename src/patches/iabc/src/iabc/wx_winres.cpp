/*
 * wx_winres.cpp - wxWindows implementations of window and associated resources.
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
#include "iabc/wx_winres.h"
#include "wx/dcclient.h"
#include "iabc/array.cpp"
#include "wx/scrolwin.h"
#include "iabc/wx_dispatch.h"
#include "iabc/map.cpp"
#include "iabc/drawtemp.cpp"
#include "iabc/event_log.h"
#include "iabc/unicode.h"
#ifndef GCC
#include "wx/dcprint.h"
#endif
static char tmp_buf[1024];

namespace iabc
{
#ifdef GCC
template <class wx_canvas>
#endif
wx_canvas& caster<wx_canvas>::get_resource(window_res& base);

#ifdef GCC
template <class wx_font>
#endif
wx_font& caster<wx_font>::get_resource(window_res& base);

#ifdef GCC
template <class wx_pen>
#endif
wx_pen& caster<wx_pen>::get_resource(window_res& base);

template<>
font*
factory<font,font::font_data*>::create(font::font_data* the_data)
{
    font_creator the_creator(*(the_data->c),
                             the_data->family,
                             the_data->point_size,
                             the_data->my_style);
    return the_creator.dispatch_get_font();
}

font_creator::font_creator(canvas& c,font::typeface family, int point_size,font::style the_style):
my_family(family),
    my_point_size(point_size),
    my_style(the_style),
    my_canvas(c)
{
}

font*
font_creator::dispatch_get_font()
{

    window& tmp_window =
        caster<window>::get_resource(my_canvas);
    int the_ignore = 0;
    get_font_dispatcher::dispatch_draw_command(tmp_window,
                                               the_ignore,*this,true);
    return my_font;
}

void
font_creator::draw_self(window& w,const int& tmp_ignore)
{
    my_font = new wx_font(my_canvas,my_family,my_point_size,my_style);
}

int wx_font_style_to_generic(int the_style, int the_weight)
{
    if (the_style == wxITALIC)
    {
        return  font::italic;
    }
    else if (the_style == wxSLANT)
    {
        return font::slanty;
    }
    else if (the_weight == wxBOLD)
    {
        return font::bold;
    }
    else
        return font::normal;
}

void generic_style_to_wx_font(int the_style,
                              int& wx_style,
                              int& wx_weight)
{
    wx_style = wxNORMAL;
    wx_weight = wxNORMAL;
    if (the_style & font::italic)
    {
        wx_style = wxITALIC;
    }
    else if (the_style & font::slanty)
    {
        wx_style = wxSLANT;
    }
    if (the_style & font::bold)
    {
        wx_weight = wxBOLD;
    }
}

wx_font::wx_font(canvas& c,font::typeface family, int point_size,font::style the_style)
:font(c,family, point_size,the_style)
{
    global_windows_event_log.log_event(windows_event_data::create_font,
                                       family,
                                       point_size,(unsigned long)the_style);
    // Convert from the generic enumerations to the wx-widgets enumerations.
    int tmp_family = wxSWISS;
    if (family == font::serifs)
    {
        tmp_family = wxROMAN;
    }
    else if (family == font::sans_serifs)
    {
        tmp_family = wxSWISS;
    }
    else
    {
        tmp_family = wxMODERN;
    }
    int wx_style,wx_weight;
    generic_style_to_wx_font(the_style,wx_style,wx_weight);

    // Create the wx_widgets implementation of a font.
    my_wxFont = new wxFont(point_size,tmp_family,wx_style,wx_weight);

    // We store the geometry information about a font when it is created
    // so that we can access this information from multiple threads.
    // geometry information for all 255 characters.
    unsigned char tmp_count;
    my_font_array.expand(0x100);
    for (tmp_count = 0;tmp_count < 0xff;++tmp_count)
    {
        wx_font_info tmp_info;

        wx_window& tmp_window =
            caster<wx_window>::get_resource(c);
        wxWindow* tmp_wxWindow = tmp_window.get_wxWindow();
        wx_dc hdc = tmp_window.get_dc();
        wxFont tmp_font = (*hdc).GetFont();
        (*hdc).SetFont(*my_wxFont);
        string s((char)tmp_count);
        (*hdc).GetTextExtent(B2U(s.access_char_array()),
                             &(tmp_info.x),&(tmp_info.y),
            &(tmp_info.descent),&(tmp_info.leading));
        my_font_array[(int)tmp_count] = tmp_info;
    }
}

wx_font::~wx_font()
{
    delete my_wxFont;
}

rect
wx_font::get_rect(const string &the_string,
                  int* the_descent,int* the_leading)
{
    int tmp_len = the_string.length();
    int i;
    point lr;
    int tmp_descent = 0;
    int tmp_leading = 0;
    for (i = 0;i < tmp_len;++i)
    {
        // Note the order of casts, need to make sure that characters in the
        // negative range are actually indexed in the array.
        unsigned char tmp_index = (unsigned char)the_string[i];
        wx_font_info& tmp_info = my_font_array[(int)tmp_index];
        lr.x += tmp_info.x;
        if (tmp_info.y > lr.y)
            lr.y = tmp_info.y;
        if (tmp_descent != tmp_info.descent)
        {
            tmp_descent = tmp_info.descent;
        }
        if (tmp_leading != tmp_info.leading)
        {
            tmp_leading = tmp_info.leading;
        }
    }
    if (the_descent)
    {
        *the_descent = tmp_descent;
    }
    if (the_leading)
    {
        *the_leading = tmp_leading;
    }
    return rect(point(0,0),lr);
}

template<>
pen*
factory<pen,pen::pen_data*>::create(pen::pen_data* the_data)
{
    return new wx_pen(*the_data->c,
                         the_data->red,
                         the_data->green,
                         the_data->blue,
                         the_data->width);
}

wx_pen::wx_pen(canvas& c,int r,int g,int b,int width):
        pen(c,r,g,b,width)
{
    hpen = new wxPen(wxColour(r,g,b),width,wxSOLID);
    hbrush = new wxBrush(wxColour(r,g,b),wxSOLID);
};

wx_pen::~wx_pen()
{
    delete hpen;
    delete hbrush;
}

BEGIN_EVENT_TABLE(wx_view_canvas, wxScrolledWindow)
    EVT_SCROLLWIN(wx_view_canvas::OnScrollEvent)
    EVT_SIZE(wx_view_canvas::OnSize)
    EVT_LEFT_DOWN(wx_view_canvas::OnLeftDown)
END_EVENT_TABLE()

wx_view_canvas::~wx_view_canvas()
{
}

void
wx_view_canvas::set_window(window* the_window)
{
    my_window = the_window;
    my_y_position = 0;
    my_x_position = 0;
    SetScrollbars(1,1,1,1);
}

// Define a constructor for my canvas
wx_view_canvas::wx_view_canvas(wxWindow *frame, const wxPoint& pos, const wxSize& size, const long style):
    wxScrolledWindow(frame, -1, pos, size, style),
    my_x_position(0),my_y_position(0),my_window(0),
    my_thumb_scroll_size(1),my_should_draw(true)
{
    GetVirtualSize(&(my_lr.x),&(my_lr.y));
}

// Define the repainting behaviour
void wx_view_canvas::OnDraw(wxDC& dc)
{
    //dc.BeginDrawing();
    if ((my_window) && (my_should_draw))
    {
        wxRegion tmp_region = GetUpdateRegion();
        wxRect r = tmp_region.GetBox();
        rect tmp_rect(point(r.GetLeft() + my_x_position,r.GetTop() + my_y_position),
                      point(r.GetRight() + my_x_position,r.GetBottom() + my_y_position));

        my_window->refresh(tmp_rect);
    }
    else
    {
        wxScrolledWindow::OnDraw(dc);
    }
    //dc.EndDrawing();
}

void
wx_view_canvas::CalculateScrollSize(const size& the_window_size)
{
    size tmp_size = the_window_size;
    if (tmp_size == size(0,0))
    {
        GetSize(&tmp_size.x,&tmp_size.y);
    }
    if ((my_lr.y < tmp_size.y) ||
        (my_lr.x < tmp_size.x))
    {
        my_lr = tmp_size;
    }

    // To do: fix scrolling on resize, now
    // we just set to left side or resizes can permanently
    // hide part of the screen.
    SetScrollbars(1,1,my_lr.x,my_lr.y,0,0,true);
    my_x_position = 0;
    my_y_position = 0;

    if (my_window)
        my_window->get_visible_rect();
}

void
wx_view_canvas::set_thumb_scroll_size(int the_size)
{
    my_thumb_scroll_size = the_size;
}

void
wx_view_canvas::OnSize(wxSizeEvent& the_event)
{
    // Handle the first size event.
    // wxSize tmp_size = the_event.GetSize();
    // CalculateScrollSize(size(tmp_size.GetX(),tmp_size.GetY()));
    this->Refresh(true);
}

void
wx_view_canvas::OnLeftDown(wxMouseEvent& the_event)
{
    if (my_window)
    {
        wxPoint wxp = the_event.GetPosition();
        point p(wxp.x,wxp.y);
        p.y += my_y_position;
        my_window->click(p);
    }
}

void
wx_view_canvas::compute_scroll_position(int& the_new_position,wxEventType the_type,
                                        int the_orient,
                                        int the_old_position,
                                        int the_max,
                                        int the_screen_size)
{
    // The user has dragged the scrollbar to the top of the screen, so set the
    // absolute position to 0 (0=highest)
    if (the_type == wxEVT_SCROLLWIN_TOP)
    {
        the_new_position = 0;
    }
    // the user has dragged the cursor to the very bottom, set the absolute
    // position to the bottom of the logical window, minus the size of the
    // viewable area.
    else if (the_type == wxEVT_SCROLLWIN_BOTTOM)
    {
        the_new_position = the_max - the_screen_size;
    }
    // The user has clicked on the up arrow above the scrollbar, so go up the
    // default thumb size
    else if (the_type == wxEVT_SCROLLWIN_LINEUP)
    {
        the_new_position = the_old_position - my_thumb_scroll_size;
    }
    else if (the_type == wxEVT_SCROLLWIN_LINEDOWN)
    {
        the_new_position = the_old_position + my_thumb_scroll_size;
    }
    // The user has paged up, or clicked in the scrollbar above/below the
    // scrollbar
    else if (the_type == wxEVT_SCROLLWIN_PAGEUP)
    {
        the_new_position = the_old_position - the_screen_size;
    }
    else if (the_type == wxEVT_SCROLLWIN_PAGEDOWN)
    {
        the_new_position = the_old_position + the_screen_size;
    }
    // If this is one of the thumb tracking one, the_new_position
    // already contains the screen coordinates in wx 2.9.
}

bool
wx_view_canvas::should_redraw(int the_delta,wxEventType the_event) const
{
    if (the_event == wxEVT_SCROLLWIN_THUMBRELEASE)
        return true;
    return false;
}

void
wx_view_canvas::OnScrollEvent(wxScrollWinEvent& event)
{
    // Figure out all of the stuff that's going on with the window and the
    // scrollbars.
    int orient = event.GetOrientation();
    int tmp_screen_x,tmp_screen_y;
    GetClientSize(&tmp_screen_x,&tmp_screen_y);
    int tmp_max_x = my_lr.x;
    int tmp_max_y = my_lr.y;

    // If this is one of the thumb tracking one, the_new_position
    // already contains the screen coordinates in wx 2.9.  so use that
    // as a default
    int tmp_x_position = event.GetPosition();
    int tmp_y_position = event.GetPosition();
    wxEventType tmp_type = event.GetEventType();
    int tmp_delta = 0;
    int tmp_thumb_scroll_size = my_thumb_scroll_size;

    // Update the current window position based on the current scrollbar position.
    // Compute the delta of where we were before all this happened.  Also more the
    // scrollbar to the appropriate place.
    if (orient == wxVERTICAL)
    {
        compute_scroll_position(tmp_y_position,tmp_type,wxVERTICAL,my_y_position,tmp_max_y,tmp_screen_y);
        tmp_delta = tmp_y_position - my_y_position;
    }
    else if (orient == wxHORIZONTAL)
    {
        compute_scroll_position(tmp_x_position,tmp_type,wxHORIZONTAL,my_x_position,tmp_max_x,tmp_screen_x);
        tmp_delta = tmp_x_position - my_x_position;
    }

    // If the scroll exceeds the legal screen boundary
    if ((orient == wxVERTICAL) &&
        (tmp_delta + my_y_position < 0))
    {
        tmp_y_position = 0;
        tmp_delta = 0;
    }
    else if ((orient == wxVERTICAL) &&
             (tmp_delta + my_y_position > tmp_max_y - tmp_screen_y))
    {
        tmp_y_position = tmp_max_y - tmp_screen_y;
        tmp_delta = 0;
    }
    if ((orient == wxHORIZONTAL) &&
        (tmp_delta + my_x_position < 0))
    {
        tmp_x_position = 0;
        tmp_delta = 0;
    }
    else if ((orient == wxHORIZONTAL) &&
             (tmp_delta + my_x_position > tmp_max_x - tmp_screen_x))
    {
        tmp_x_position = tmp_max_x - tmp_screen_x;
        tmp_delta = 0;
    }

    // Now scroll the window itself.
    bool tmp_should_redraw = should_redraw(tmp_delta,tmp_type);

    if (orient == wxHORIZONTAL)
    {
        my_x_position = tmp_x_position;
        m_xScrollPosition = my_x_position;
        SetScrollPos(wxHORIZONTAL,my_x_position,true);
        global_windows_event_log.log_event(windows_event_data::hscroll_window,
                                           my_x_position,
                                           tmp_delta,tmp_should_redraw);
        if (tmp_should_redraw)
            m_targetWindow->ScrollWindow(tmp_delta, 0, (const wxRect *) NULL);
    }
    else
    {
        my_y_position = tmp_y_position;
        m_yScrollPosition = my_y_position;
        SetScrollPos(wxVERTICAL,my_y_position,true);
        global_windows_event_log.log_event(windows_event_data::vscroll_window,
                                           my_y_position,
                                           tmp_delta,tmp_should_redraw);
        if (tmp_should_redraw)
            m_targetWindow->ScrollWindow(0,tmp_delta,(const wxRect *) NULL);
    }

    // I don't trust this basic scrolling stuff, so redraw whenever the user
    // stops moving the mouse.
    if (tmp_should_redraw)
    {
        this->Refresh(FALSE);
    }
    my_should_refresh = true;

    // Finally, update the window object with the current
    // scroll position.
    if (my_window)
    {
        my_window->get_visible_rect();
    }

}

void
wx_view_canvas::SetRawSize(const point& the_point)
{
    my_lr = the_point;
    CalculateScrollSize(my_lr);
}

bool
wx_view_canvas::get_and_reset_my_should_refresh()
{
    bool tmp_rv = my_should_refresh;
    my_should_refresh = false;
    return tmp_rv;
}

rect
wx_view_canvas::RawGetVisible()
{
    rect tmp_rv;
    tmp_rv.lr.x = GetSize().GetWidth();
    tmp_rv.lr.y = GetSize().GetHeight();
    int tmp_ypos = this->GetScrollPos(wxSB_VERTICAL);
    int tmp_xpos = this->GetScrollPos(wxSB_HORIZONTAL);

    tmp_rv.ul.x += tmp_xpos;
    tmp_rv.lr.x += tmp_xpos;
    tmp_rv.ul.y += tmp_ypos;
    tmp_rv.lr.y += tmp_ypos;

    return tmp_rv;
}

bool
wx_view_canvas::ProcessEvent(wxEvent &event)
{
    if (event.GetEventType() == wxEVT_USER_FIRST)
    {
        wx_window_object* tmp_object =
            (wx_window_object*)event.GetEventObject();
        wx_dispatcher* tmp_dispatcher = tmp_object->my_dispatcher;
        tmp_dispatcher->platform_execute();
        delete tmp_object;
        event.SetEventObject(0);
        return true;
    }
    else
    {
        wxWindow::ProcessEvent(event);
        return false;
    }
}

bool
wx_print_canvas::ProcessEvent(wxEvent &event)
{
    if (event.GetEventType() == wxEVT_USER_FIRST)
    {
        wx_window_object* tmp_object =
            (wx_window_object*)event.GetEventObject();
        wx_dispatcher* tmp_dispatcher = tmp_object->my_dispatcher;
        tmp_dispatcher->platform_execute();
        delete tmp_object;
        event.SetEventObject(0);
        return true;
    }
    else
    {
        wxWindow::ProcessEvent(event);
        return false;
    }
}

wx_print_canvas::~wx_print_canvas()
{
}

// Define a constructor for my canvas
wx_print_canvas::wx_print_canvas(wxWindow *frame, const wxPoint& pos, const wxSize& size, const long style):
    wxWindow(frame, -1, pos, size, style)
{
}

wx_dc::wx_dc(wxWindow& the_window):
my_print(0),my_window(&the_window),my_dc(0)
{
}

wx_dc::wx_dc(wxPrintout& the_printout):
my_window(0),my_print(&the_printout),my_dc(0)
{
}

wx_dc::~wx_dc()
{
    if (my_print == 0)
    {
        delete my_dc;
        my_dc = 0;
    }
}

wx_dc::operator wxDC&()
{
    if (my_print)
    {
        my_dc = my_print->GetDC();
    }
    else if (my_dc == 0)
    {
        my_dc = new wxClientDC(my_window);
    }

    return *my_dc;
}

wxDC&
wx_dc::operator*()
{
    return (wxDC&)(*this);
}

wx_dc&
wx_dc::operator=(const wx_dc& o)
{
    my_window = o.my_window;
    my_print = o.my_print;
    my_dc = 0;
    return *this;
}

wx_dc::wx_dc(const wx_dc& o)
{
    my_window = o.my_window;
    my_print = o.my_print;
    my_dc = 0;
}

wx_printout::wx_printout(document& the_doc,wx_printing_window& the_window):
my_window(&the_window),my_doc(&the_doc)
{
}

void
wx_printout::GetPageInfo(int* the_min,int* the_max,int* the_from,int* the_to)
{
    int tmp_least = 1;
    int tmp_greatest = 0;
    while (my_doc->has_page(tmp_greatest + 1))
    {
        ++tmp_greatest;
    }
    *the_min = tmp_least;
    *the_from = tmp_least;
    *the_to = *the_max = tmp_greatest;
}

bool
wx_printout::HasPage(int the_page)
{
    return my_doc->has_page(the_page);
}

bool
wx_printout::OnPrintPage(int the_page)
{
    my_doc->print_page(the_page);
    return true;
}

void
wx_printout::OnEndPrinting()
{
    my_window->end_printing();
}

void
wx_printout::OnBeginPrinting()
{
    my_window->start_printing();
}

wx_window::~wx_window()
{
}

wx_dc
wx_scrolling_window::get_dc()
{
    wx_dc tmp_dc(*my_view);
    return tmp_dc;
}

wx_dc
wx_window::get_dc()
{
    wx_dc tmp_dc(*my_wxWindow);
    return tmp_dc;
}

wx_window::wx_window(const string& the_string,wxWindow& the_window):wx_canvas(the_string),
        my_wxWindow(&the_window),last_pen(0),
        my_needs_ppi(true)
{
}

void
wx_window::raw_refresh(const rect& the_rect)
{
    if (my_document)
    {
        global_windows_event_log.log_event(
            windows_event_data::redraw_window,
            the_rect.ul.x,the_rect.ul.y,
            the_rect.lr.x,the_rect.lr.y);
        my_document->redraw(the_rect);
    }
}

void
wx_window::show()
{
    my_wxWindow->Show();
}

void
wx_window::hide()
{
    my_wxWindow->Show(FALSE);
}

void
wx_window::raw_draw_line(pen &the_pen, const line &the_line)
{
    wx_dc hdc = get_dc();
    set_pens(*hdc,the_pen,false);
    (*hdc).DrawLine(wxPoint(the_line.p1.x,the_line.p1.y),wxPoint(the_line.p2.x,the_line.p2.y));
}

void
wx_window::set_pens(wxDC& the_dc,pen& the_pen,bool fill)
{
    wx_pen& tmp_pen =
        caster<wx_pen>::get_resource(the_pen);

    the_dc.SetPen(*(tmp_pen.get_wxPen()));
    if (fill == false)
    {
        the_dc.SetBrush(*wxTRANSPARENT_BRUSH);
    }
    else
    {
        the_dc.SetBrush(*(tmp_pen.get_wxBrush()));
    }
}

void
wx_window::raw_draw_rect(pen &the_pen, const rect &the_rect, bool fill)
{
    wx_dc hdc = get_dc();
    set_pens(*hdc,the_pen,fill);
    rect tmp_rect = the_rect;
    tmp_rect = tmp_rect.offset(my_scroll_point * point(-1,-1));

    (*hdc).DrawRectangle(wxPoint(tmp_rect.ul.x,tmp_rect.ul.y),wxSize(tmp_rect.width(),tmp_rect.height()));
}

void
wx_window::raw_draw_text(font &the_font, const string &the_string)
{
    wx_dc hdc = get_dc();
    wxFont* hfont =
        caster<wx_font>::get_resource(the_font).get_wxFont();
    rect tmp_rect = the_font.get_rect(the_string);
    tmp_rect.ul += my_current_pos;
    tmp_rect.lr += my_current_pos;

    rect tmp_visible = get_drawable_rect();

    if (tmp_visible.intersects(tmp_rect) == false)
    {
        return;
    }

    (*hdc).SetFont(*hfont);
    (*hdc).SetMapMode(wxMM_TEXT);
    (*hdc).DrawText(B2U(the_string.access_char_array()),wxPoint(tmp_rect.ul.x,tmp_rect.ul.y));
}

rect
wx_window::get_drawable_rect()
{
    rect tmp_rv;
//    if (this->my_printout == 0)
//    {
        tmp_rv = rect(point(0,0),
                      point(my_wxWindow->GetSize().GetWidth(),
                            my_wxWindow->GetSize().GetHeight()));
//    }
//    else
//    {
//        int w,h;
//        my_printout->GetPageSizePixels(&w,&h);
//        tmp_rv = rect(point(0,0),point(w,h));
//    }

    return tmp_rv;
}

int
wx_window::pixels_per_inch_x()
{
    if (my_needs_ppi)
    {
        wx_dc hdc = get_dc();
        my_ppi.x = (*hdc).GetPPI().GetWidth();
        my_ppi.y = (*hdc).GetPPI().GetHeight();
        my_needs_ppi = false;
    }
    return my_ppi.x;
}

int
wx_window::pixels_per_inch_y()
{
    if (my_needs_ppi)
    {
        wx_dc hdc = get_dc();
        my_ppi.x = (*hdc).GetPPI().GetWidth();
        my_ppi.y = (*hdc).GetPPI().GetHeight();
        my_needs_ppi = false;
    }
    return my_ppi.y;
}

void wx_window::render_polygon(pen& the_pen,polygon& the_polygon)
{
    point tmp_current_pos = my_current_pos + the_polygon.get_ul_offset();
    rect tmp_current_rect(tmp_current_pos,tmp_current_pos + point(the_polygon.get_size().x,the_polygon.get_size().y));

    rect tmp_visible_rect = get_drawable_rect();

    // Don't bother rendering things if they are off the visible screen, and also
    // don't render things if they will go to the printer later.
    if (tmp_current_rect.intersects(tmp_visible_rect) == false)
    {
        return;
    }

    wx_dc hdc = get_dc();
    (*hdc).BeginDrawing();
    wx_pen& pen = caster<wx_pen>::get_resource(the_pen);
    polygon::fill_point tmp_fill = the_polygon.get_next_fill_point();
    bool tmp_should_fill = (tmp_fill.my_mode == polygon::fill) ? true : false;
    set_pens(*hdc,the_pen,tmp_should_fill);
    my_poly_points.remove_all();
    my_poly_points.expand_to(the_polygon.get_segment_count());
    point starting_point = my_current_pos;
    int tmp_fill_index = 0;
    for (int i = 0;i < the_polygon.get_segment_count(); ++i)
    {
        point tmp_point = the_polygon[i];

        tmp_point.x = (int)((double)tmp_point.x * x_xform.m);
        tmp_point.y = (int)((double)tmp_point.y * y_xform.m);
        my_current_pos = starting_point + tmp_point;
        my_poly_points[i - tmp_fill_index].x = my_current_pos.x;
        my_poly_points[i - tmp_fill_index].y = my_current_pos.y;
        global_windows_event_log.log_event(windows_event_data::add_point,my_current_pos.x,
                                           my_current_pos.y);
        if (tmp_fill.my_index == i)
        {
            if (tmp_fill.my_mode == polygon::fill)
            {
				global_windows_event_log.log_event(windows_event_data::draw_poly,my_poly_points.get_size());
                (*hdc).DrawPolygon((i - tmp_fill_index) + 1,my_poly_points);
            }
            else
            {
                global_windows_event_log.log_event(windows_event_data::draw_line,my_poly_points.get_size());
                (*hdc).DrawLines((i - tmp_fill_index) + 1,my_poly_points);
            }
            tmp_fill = the_polygon.get_next_fill_point(tmp_fill);
            tmp_should_fill = (tmp_fill.my_mode == polygon::fill) ? true : false;
            set_pens(*hdc,the_pen,tmp_should_fill);
            tmp_fill_index = i + 1;
        }
    }
}

window*
wx_window::get_window(wxWindow* the_handle)
{
    list<window*>::iterator tmp_cursor(window::class_window_list.first());
    while (tmp_cursor.is_end() == false)
    {
        window* tmp_window = (*tmp_cursor);
        wx_window* tmp_winx = dynamic_cast<wx_window*>(tmp_window);
        if ((tmp_winx != 0) &&
            (tmp_winx->get_wxWindow() == the_handle))
        {
            return tmp_winx;
        }
        tmp_cursor.next();
    }

    return 0;
}

wx_scrolling_window::wx_scrolling_window(const string& the_title,
                                         wx_view_canvas& the_window):
my_view(&the_window),wx_window(the_title,the_window)
{
}

wx_scrolling_window::~wx_scrolling_window()
{
    my_view->set_window(0);
}

void
wx_scrolling_window::set_scroll_point(const point& the_point)
{
    wxScrollWinEvent eventy( wxEVT_SCROLLWIN_THUMBRELEASE,
                     the_point.y, wxVERTICAL);
    wxScrollWinEvent eventx( wxEVT_SCROLLWIN_THUMBRELEASE,
                     the_point.x, wxHORIZONTAL);
    my_wxWindow->ProcessEvent(eventx);
    my_wxWindow->ProcessEvent(eventy);
}

rect
wx_scrolling_window::raw_get_visible()
{
    my_scroll_point = point(my_wxWindow->GetScrollPos(wxSB_HORIZONTAL),
                            my_wxWindow->GetScrollPos(wxSB_VERTICAL));

    rect tmp_rv;
//    if (this->my_printout == 0)
//    {
        return my_view->RawGetVisible();
//    }
//    else
//    {
//        tmp_rv = my_printing_rect;
//    }

    return tmp_rv;
}

void
wx_scrolling_window::raw_update_thumb_scroll_size()
{
    my_view->set_thumb_scroll_size(my_thumb_scroll_size);
}

void
wx_scrolling_window::set_raw_size(const size& tmp_size)
{
    my_view->SetRawSize(tmp_size);
}

void
wx_scrolling_window::set_center_of_y_scroll(int the_point)
{
    rect tmp_rect = my_view->RawGetVisible();
    int tmp_height = tmp_rect.height();
    the_point = the_point - (tmp_height / 2);
    if (the_point > 0)
    {
        set_scroll_point(point(0,the_point));
    }

}

wx_printing_window::wx_printing_window(const string& the_title,
                                       wx_print_canvas& the_canvas)
:wx_window(the_title,the_canvas)
{
}

wx_printing_window::~wx_printing_window()
{
}

rect
wx_printing_window::get_drawable_rect()
{
    rect tmp_rv;
    int w,h;
    my_printout->GetPageSizePixels(&w,&h);
    tmp_rv = rect(point(0,0),point(w,h));

    return tmp_rv;
}

void
wx_printing_window::end_printing()
{
    my_printing_rect=rect(point(0,0),point(0,0));
    my_printout = 0;
    my_needs_ppi = true;
}

wx_dc
wx_printing_window::get_dc()
{
    wx_dc tmp_dc(*my_printout);
    return tmp_dc;
}

void
wx_printing_window::start_printing()
{
    my_needs_ppi = true;
}

void
wx_printing_window::print_page(int the_page)
{
    if (my_document)
        my_document->print_page(the_page);
}

}


