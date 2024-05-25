/*
 * wx_dispatch.cpp - wxWindows implementation of a threadsafe command dispatcher.
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
#include "iabc/wx_dispatch.h"
#include "iabc/wx_winres.h"
#include "iabc/list.cpp"
#include "iabc/wd_temp.cpp"
#include "iabc/event_log.h"
#include "resource.h"
#define IDD_DIALOG1                     129

namespace iabc
{
template<>
dispatcher* 
factory<dispatcher,int>::create()
{
    return new wx_dispatcher;
}

int 
wx_window_object::class_number_instances = 0;


wx_window_object::~wx_window_object()
{
    --class_number_instances;
}

void 
wx_dispatcher::platform_dispatch(dispatcher::command_data* the_command)
{
    wxWindow* hwnd = (caster<wx_canvas>::get_resource(get_canvas(the_command))).get_wxWindow();
    wxCommandEvent tmp_event( wxEVT_USER_FIRST, 0 );
    wx_window_object* tmp_object= new wx_window_object(*this);
    tmp_event.SetEventObject(tmp_object);
    hwnd->AddPendingEvent(tmp_event);
}

void
wx_dispatcher::platform_execute()
{
    execute_command();
}

wx_window_object::wx_window_object():my_dispatcher(0)
{
    ++class_number_instances;
}
wx_window_object::wx_window_object(wx_dispatcher& the_dispatcher):my_dispatcher(&the_dispatcher)
{
    ++class_number_instances;
}

}
