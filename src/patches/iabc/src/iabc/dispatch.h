/*
 * dispatch.h - Generic inter-thread command dispatcher
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
#ifndef iabc_dispatch
#define iabc_dispatch
#include "iabc/wd_data.h"
#include "iabc/event_log.h"
#include "iabc/mutex.h"
#include "iabc/list.h"
#include "iabc/semaphore.h"
#include "iabc/winres.h"
// FILE: dispatch.h
// These commands allow a window command to be dispatched to another thread.
namespace iabc
{
class dispatcher;

// CLASS: wd_command
// DESCRIPTION:
// The command we are trying to send to the window will be 
// embedded in a wd command class.  A derived version of
// this class will actually be called in the correct thread.
class wd_command
{
public:
    friend class dispatcher;
    // METHOD: ctor
    // DESCRIPTION:
    // The dispatcher requires a window to send the command to.
    wd_command(canvas& the_canvas);
    virtual ~wd_command();

    // METHOD: command_add_ref
    // DESCRIPTION:
    // Implement reference counting so the window gets destroyed
    // when everybody's done with it and not before.
    void command_add_ref(){++my_count;};
    void command_remove_ref(){if (--my_count == 0) delete this;};

    // METHOD: get_default_canvas
    // DESCRIPTION:
    // We assume that there's always 1 'main' window that is always available to
    // dispatch your commands for you, if you need to dispatch something but don;e
    // know about a particular window class (which is often the case with dialog
    // boxes).
    static canvas* get_default_canvas(){return class_default_canvas;};
protected:
    static instance_counter<wd_command> class_instance_counter;
    // METHOD: do_command
    // This is actually implemented by the template class that encapsulates the
    // command
    virtual void do_command(wd_data param) = 0;
    canvas& get_canvas(){return my_canvas;};
private:
    canvas& my_canvas;
    static canvas* class_default_canvas;
    atomic_counter my_count;
};

// CLASS: dispatcher
// DESCRIPTION:
// A dispatcher takes a set of wd_command objects and queues them
// up and sends them to the correct thread.  If we are already in the
// correct thread and blocking is on then we execute it right then and 
// there.
class dispatcher
{
public:
    class command_data
    {
    public:
        wd_command* command;
        wd_data data;
        bool should_block;
    };
    static dispatcher* get_dispatcher();
    static void end_app();
    void dispatch(wd_command& c,wd_data d,bool the_should_block = true);
    void add_ref(){++my_count;};
    void remove_ref(){if (--my_count == 0) delete this;};
    static int log_id;
protected:
    dispatcher();
    virtual ~dispatcher();
    void queue_data(command_data* the_data);
    command_data* dequeue_data();
    void dispatcher_add_ref(){my_count++;};
    void dispatcher_remove_ref(){if (--my_count == 0) delete this;};
    virtual void platform_dispatch(command_data* the_data) = 0;
    canvas& get_canvas(command_data* the_data){return the_data->command->get_canvas();}
    void execute_command();
    static void set_default_canvas(canvas& the_canvas){wd_command::class_default_canvas = &the_canvas;};
    atomic_counter my_count;
    semaphore my_sem;
    static mutex my_mutex;
    static bool the_app_ended;
    static int class_instance_count;
    list<command_data*> my_command_list;
};

}
#endif



