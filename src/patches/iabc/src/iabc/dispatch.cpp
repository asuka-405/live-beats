/*
 * dispatch.cpp - Generic inter-thread command dispatcher
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
#include "iabc/dispatch.h"
#include "iabc/threads.h"
#include "iabc/list.cpp"
#include "iabc/event_log.h"
#include "iabc/factory.cpp"

namespace iabc
{
;

bool dispatcher::the_app_ended = false;

canvas*
wd_command::class_default_canvas = 0;

instance_counter<wd_command> wd_command::class_instance_counter;

wd_command::~wd_command()
{
    --class_instance_counter;
}

wd_command::wd_command(canvas& the_canvas):my_canvas(the_canvas)
{
    ++class_instance_counter;
}

mutex dispatcher::my_mutex;

dispatcher* 
dispatcher::get_dispatcher()
{
    // Create a log for the first issue.
    return factory<dispatcher,int>::create();
}

void 
dispatcher::end_app()
{
    the_app_ended = true;
}

int
dispatcher::log_id = 0;

void
dispatcher::dispatch(wd_command& c,wd_data d,bool the_should_block)
{
    bool tmp_gui_thread = (thread::this_thread() == &(thread::get_system_thread()))
                           ? true : false;

    global_dispatch_event_log.log_event(dispatch_event_data::enter);
    c.command_add_ref();
	this->add_ref();
    dispatcher::command_data* tmp_command = 
        new dispatcher::command_data;
    tmp_command->command = &c;
    tmp_command->data = d;
    tmp_command->should_block = tmp_gui_thread ? false : the_should_block;
    
    queue_data(tmp_command);
    
    // If we don't need to block, and we're already in the correct thread, do the
    // thing now
    if ((tmp_gui_thread) && (the_app_ended == false))
    {
        global_dispatch_event_log.log_event(dispatch_event_data::same_thread);
        execute_command();
    }
    else if (the_app_ended == false)
    {
		this->add_ref();
        platform_dispatch(tmp_command);
    
        if ((tmp_gui_thread == false) &&
            (the_should_block == true))
        {
            global_dispatch_event_log.log_event(dispatch_event_data::blocking);
            my_sem.seize();
            global_dispatch_event_log.log_event(dispatch_event_data::unblocked);
        }
		this->remove_ref();
    }
}

dispatcher::dispatcher():my_sem(0)
{
    global_dispatch_event_log.log_event(dispatch_event_data::ctor);
    class_instance_count++;
}
dispatcher::~dispatcher()
{
    global_dispatch_event_log.log_event(dispatch_event_data::dtor);
    class_instance_count--;
};

void
dispatcher::queue_data(command_data* the_data)
{
    lock tmp_lock(my_mutex);
    list<dispatcher::command_data*>::iterator tmp_cursor(my_command_list.first());
    tmp_cursor.add_after(the_data);
}

dispatcher::command_data* 
dispatcher::dequeue_data()
{
    lock tmp_lock(my_mutex);
    list<dispatcher::command_data*>::iterator tmp_cursor = 
        my_command_list.first();
    if (tmp_cursor)
    {
        command_data* tmp_data = (*tmp_cursor);
        tmp_cursor.delete_current();
        return tmp_data;
    }

    return 0;
}

void
dispatcher::execute_command()
{
    dispatcher::command_data* tmp_data = dequeue_data();
    if (tmp_data)
    {
        tmp_data->command->do_command(tmp_data->data);
        if (tmp_data->should_block == true)
        {
            my_sem.release();
        }
        tmp_data->command->command_remove_ref();
        delete tmp_data;
    }
	this->remove_ref();
}

int dispatcher::class_instance_count = 0;
}
