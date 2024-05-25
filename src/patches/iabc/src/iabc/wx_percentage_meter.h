/*
 * wx_percentage_meter.h - wxWindows implementation of a GUI progress gadget.
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
#ifndef iabc_wx_percentage_meter
#define iabc_wx_percentage_meter

#include "iabc/drawtemp.h"
#include "wx/wx.h"
#include "iabc/percentage_meter.h"

namespace iabc
{
;
class wx_percentage_meter:public percentage_meter
{
public:
    friend class draw_command<wx_percentage_meter,percentage_meter::operation>;
    wx_percentage_meter(wxButton& the_cancel,wxStaticText& the_label,
                        wxStaticText& the_number,wxGauge& the_bar,wxWindow& the_parent);
    virtual ~wx_percentage_meter();
    void set_cancelled(){this->my_is_cancelled = true;};
    static void static_set_cancelled(wxButton* the_button);
    wxWindow* get_parent(){return my_parent;};
private:
    void update(operation the_operation);
    void draw_self(window& the_window,percentage_meter::operation the_operation);
    int my_last_value;
    wxButton* my_button;
    wxStaticText* my_label;
    wxStaticText* my_number;
    wxGauge* my_bar;
    wxWindow* my_parent;
    static mutex class_map_mutex;
    static map<wxButton*,wx_percentage_meter*> class_map;
};

typedef draw_command<wx_percentage_meter,percentage_meter::operation> draw_meter_command;

}

#endif

