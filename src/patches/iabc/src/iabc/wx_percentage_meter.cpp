/*
 * wx_percentage_meter.cpp - wxWindows implementation of a progress indicator for slow things.
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
#include "iabc/wx_percentage_meter.h"
#include "iabc/map.cpp"
#include "iabc/drawtemp.cpp"
#include <iabc/unicode.h>

namespace iabc
{
;
mutex wx_percentage_meter::class_map_mutex;
map<wxButton*,wx_percentage_meter*> 
wx_percentage_meter::class_map;

wx_percentage_meter::wx_percentage_meter(wxButton& the_cancel,wxStaticText& the_label,
                    wxStaticText& the_number,wxGauge& the_bar,wxWindow& the_parent):
my_button(&the_cancel),
my_label(&the_label),
my_number(&the_number),
my_bar(&the_bar),
my_parent(&the_parent),
my_last_value(0)
{
    lock tmp_lock(class_map_mutex);
    if (my_parent == 0)
    {
        throw("Null parent");
    }
    class_map.add_pair(&the_cancel,this);
    update(percentage_meter::initialize_gui);
}

wx_percentage_meter::~wx_percentage_meter()
{
    lock tmp_lock(class_map_mutex);
    class_map.remove(my_button);
	update(destroy_gui);
}

void 
wx_percentage_meter::static_set_cancelled(wxButton* the_button)
{
    lock tmp_lock(class_map_mutex);
    map<wxButton*,wx_percentage_meter*>::iterator tmp_it = 
        class_map.get_item(the_button,exact);

    if (tmp_it)
    {
        (*tmp_it).value->set_cancelled();
    }
}

void
wx_percentage_meter::update(percentage_meter::operation the_operation)
{
    if ((my_last_value != (int)this->my_value) ||
        (the_operation != percentage_meter::update_gui))
    {
        my_last_value = (int)my_value;
        window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
        draw_meter_command::dispatch_draw_command(tmp_window,the_operation,*this,true);
    }
}

void
wx_percentage_meter::draw_self(window& the_window,percentage_meter::operation the_operation)
{
    if (the_operation == percentage_meter::initialize_gui)
    {
        my_bar->SetRange(100);
        my_bar->SetValue(0);
        my_bar->SetWindowStyle(wxGA_SMOOTH | wxHORIZONTAL);
        my_parent->Show(true);
        my_label->SetLabel(_T("0 %"));
        this->my_label->SetLabel(_T(""));
    }
    else if (the_operation == percentage_meter::update_gui)
    {
        if (my_value < 0)
        {
            my_value = 0;
        }
        if (my_value > 100)
        {
            my_value = 100;
        }
        my_bar->SetValue((int)(my_value));
        my_label->SetLabel(B2U(my_message.access_char_array()));
        string tmp_bar_label((long)my_value);
        my_number->SetLabel(B2U(tmp_bar_label.access_char_array()));
    }
    else if (the_operation == percentage_meter::destroy_gui)
    {
        my_parent->Show(false);
    }
}

}
