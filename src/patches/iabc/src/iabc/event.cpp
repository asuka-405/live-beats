/*
 * event.cpp - Platform-independent event generation.
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
#include "iabc/event.h"
#include "iabc/list.cpp"
#include "iabc/event_log.h"

namespace iabc {
;
#ifndef dword
#define dword unsigned long
#endif

int event_handler_if::class_instance_count = 0;
int event_source::class_instance_count = 0;

void 
event_source::add_event_handler(event_handler_if& the_event)
{
    global_event_event_log.log_event(event_event_data::add_handler,(dword)this,(dword)&the_event);

    pre_add_event_handler();
    the_event.event_handler_add_ref();
    add_event_handler_to_list(the_event);
    post_add_event_handler();
}

void 
event_source::remove_event_handler(event_handler_if& the_event)
{
    global_event_event_log.log_event(event_event_data::remove_handler,
                                     (dword)this,(dword)&the_event);
    lock tmp_lock(my_mutex);
    list<event_handler_if*>::iterator tmp_cursor = my_list.first();
    while (tmp_cursor)
    {
        event_handler_if* tmp_event = (*tmp_cursor);
        if (tmp_event == &the_event)
        {
            the_event.event_handler_remove_ref();
            tmp_cursor.delete_current();
            break;
        }

        tmp_cursor.next();
    }
}

void
event_source::add_event_handler_to_list(event_handler_if& the_event)
{
    global_event_event_log.log_event(event_event_data::add_handler_to_list,
                                     (dword)this,(dword)&the_event);
    lock tmp_lock(my_mutex);
    list<event_handler_if*>::iterator tmp_cursor = my_list.first();
    tmp_cursor.add_after(&the_event);
}

void 
event_source::notify_handlers()
{
    global_event_event_log.log_event(event_event_data::notify_handlers,
                                     (dword)this);
    list<event_handler_if*>::iterator tmp_new_cursor = 
        populate_handler_list();
    tmp_new_cursor.reset_to_front();
    while (tmp_new_cursor)
    {
        (*tmp_new_cursor)->handle_event();
        (*tmp_new_cursor)->event_handler_remove_ref();
        tmp_new_cursor.delete_current();
    }
}

list<event_handler_if*>::iterator 
event_source::populate_handler_list( )
{
    lock tmp_lock(my_mutex);
    list<event_handler_if*> tmp_new_list;
    list<event_handler_if*>::iterator tmp_new_cursor = tmp_new_list.first();
    list<event_handler_if*>::iterator tmp_cursor = my_list.first();
    while (tmp_cursor)
    {
        tmp_new_cursor.add_after(*tmp_cursor);
        tmp_new_cursor.next();
        tmp_cursor.delete_current();
        tmp_cursor.reset_to_front();
    }

    return tmp_new_cursor;
}

event_handler::event_handler():
my_source(0),my_next(0),my_last(0),my_sem(0),
my_armed(false),my_fired(false)
{
}

event_handler&
event_handler::operator|(event_handler& the_other)
{
    global_event_event_log.log_event(event_event_data::op_or,(dword)this,(dword)&the_other);
    my_last = &the_other;
    my_last->event_handler_add_ref();
    the_other.add_handler_to_chain(*this);
    return the_other;
}
    
void
event_handler::wait(event_handler& the_event)
{
    global_event_event_log.log_event(event_event_data::wait,(dword)&the_event);
    the_event.set_event_source();
    the_event.my_sem.seize();
    the_event.tear_down_chain();
}

void
event_handler::set_event_source()
{
    global_event_event_log.log_event(event_event_data::set_event_source,
                                     (dword)my_source,
                                     (dword)this);
    if (my_source != 0)
    {
        throw HandleException();
    }
    my_armed = true;
    if (my_next)
    {
        my_next->set_event_source();
    }
    my_source = get_event_source();
    my_source->event_source_add_ref();
    my_source->add_event_handler(*this);
}

void
event_handler::handle_event()
{
    global_event_event_log.log_event(event_event_data::handle_event,(dword)this);
    my_fired = true;
    pass_event_to_first();
}

void 
event_handler::add_handler_to_chain(event_handler& the_next)
{
    global_event_event_log.log_event(event_event_data::add_handler_to_chain,
                                     (dword)this,(dword)&the_next);
    my_next = &the_next;
    my_next->event_handler_add_ref();
}

void 
event_handler::tear_down_chain()
{
    event_handler* tmp_next; 
    event_handler* tmp_last; 
    event_source* tmp_source;
    add_all_refs(tmp_last,tmp_next,tmp_source);
    global_event_event_log.log_event(event_event_data::tear_down_chain,
                                     (dword)this,(dword)tmp_next,(dword)tmp_last,(dword)tmp_source);
    zero_all_refs();
    if (tmp_next)
    {
        tmp_next->tear_down_chain();
        tmp_next->event_handler_remove_ref();
    }
    if (tmp_source)
    {
        tmp_source->remove_event_handler(*this);
        tmp_source->event_source_remove_ref();
        tmp_source = 0;
    }
    if (tmp_last)
    {
        tmp_last->event_handler_remove_ref();
    }
    remove_all_refs(tmp_last,tmp_next,tmp_source);
}

void
event_handler::zero_all_refs()
{
    global_event_event_log.log_event(event_event_data::zero_all_refs,
                                     (dword)this);
    lock tmp_lock(my_mutex);

    my_source = 0;
    my_next = 0;
    my_last = 0;
}

void 
event_handler::pass_event_to_first()
{
    event_handler* tmp_next; 
    event_handler* tmp_last; 
    event_source* tmp_source;
    add_all_refs(tmp_last,tmp_next,tmp_source);
    global_event_event_log.log_event(event_event_data::pass_event_to_first,
                                     (dword)this,(dword)tmp_last);
    if (tmp_last)
    {
        tmp_last->pass_event_to_first();
    }
    else 
    {
        lock tmp_lock(my_mutex);
        if (my_armed == true)
        {
            my_sem.release();
            my_armed = false;
        }
    }
    remove_all_refs(tmp_last,tmp_next,tmp_source);
}

void 
event_handler::add_all_refs(event_handler*& tmp_last, 
                            event_handler*& tmp_next, 
                            event_source*& tmp_source)
{
    global_event_event_log.log_event(event_event_data::add_all_refs,
                                     (dword)this);
    lock tmp_lock(my_mutex);
    tmp_last = my_last;
    tmp_next = my_next;
    tmp_source = my_source;
	if (tmp_last)
		tmp_last->event_handler_add_ref();
	if (tmp_next)
		tmp_next->event_handler_add_ref();
	if (tmp_source)
		tmp_source->event_source_add_ref();
}

void 
event_handler::remove_all_refs(event_handler* tmp_last, 
                            event_handler* tmp_next, 
                            event_source* tmp_source)
{
    global_event_event_log.log_event(event_event_data::remove_all_refs,(dword)this);
    lock tmp_lock(my_mutex);
	if (tmp_last)
		tmp_last->event_handler_remove_ref();
	if (tmp_next)
	    tmp_next->event_handler_remove_ref();
	if (tmp_source)
		tmp_source->event_source_remove_ref();
}
#undef LOGID
}


