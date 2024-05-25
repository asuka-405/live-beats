/*
 * wx_dispatch.h - wxWindows implementation of classes to dispatch to gui thread.
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
#ifndef iabc_wx_dispatch_h
#define iabc_wx_dispatch_h
#include "iabc/dispatch.h"
#include "wx/event.h"

namespace iabc
{
;
class wx_dispatcher;

class wx_window_object:public wxObject
{
public:
    wx_window_object();
    wx_window_object(wx_dispatcher& the_dispatcher);
    virtual ~wx_window_object();
    virtual void CopyObject(wxObject &object_dest) const 
    {
        // wxObject::CopyObject(object_dest);
        wx_window_object* tmp_object = (wx_window_object*)&(object_dest);
        tmp_object->my_dispatcher = my_dispatcher;
    };

    static int class_number_instances;
    wx_dispatcher* my_dispatcher;
};
class wx_dispatcher:public dispatcher
{
public:
    friend class wx_event_handler;
    wx_dispatcher(){};
    virtual ~wx_dispatcher(){};
    void platform_execute();
    static void wx_set_default_canvas(canvas& the_canvas){set_default_canvas(the_canvas);};
protected:
    virtual void platform_dispatch(dispatcher::command_data* the_command);
};

}

#endif

