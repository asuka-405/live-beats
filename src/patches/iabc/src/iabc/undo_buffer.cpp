/*
 * undo_buffer.h - implement basic undo functionality
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
#include "iabc/undo_buffer.h"
#include "iabc/list.cpp"


namespace iabc
{
list<undo_event> undo_buffer::my_buffer;

undo_event::undo_event():
    start_select(0),
    end_select(0),
    is_modified(false),
    is_pre_save(0),
    the_event(0)
{
}

undo_event::undo_event(const undo_event& o):
    start_select(o.start_select),
    end_select(o.end_select),
    is_modified(o.is_modified),
	the_buffer(o.the_buffer),
    is_pre_save(o.is_pre_save),
	the_event(0)
{
    if (o.the_event)
    {
        the_event = (wxKeyEvent*)o.the_event->Clone();
    }
}

undo_event&
undo_event::operator=(const undo_event& o)
{
    the_buffer = o.the_buffer;
    is_modified = o.is_modified;
    is_pre_save = o.is_pre_save;
    start_select = o.start_select;
    end_select = o.end_select;
    if (the_event)
    {
        delete the_event;
        the_event = 0;
    }
    if (o.the_event)
    {
        the_event = (wxKeyEvent*)o.the_event->Clone();
    }
	return *this;
}

undo_event::~undo_event()
{
    delete the_event;
}

void 
undo_buffer::push(const undo_event& e)
{
    // Push the new event on the stack.
	iabc::push(undo_buffer::my_buffer,e);

    // If there are too many items on the stack, circular queue the oldest
    // one off
    if (undo_buffer::my_buffer.get_size() > MAX_UNDO_EVENTS)
    {
        deque(undo_buffer::my_buffer);
    }
}

undo_event 
undo_buffer::pop()
{
	return iabc::pop(undo_buffer::my_buffer);
}

int undo_buffer::get_size()
{
    return undo_buffer::my_buffer.get_size();
}

void undo_buffer::clear()
{
    remove_all(undo_buffer::my_buffer);
}

void undo_buffer::mark_save()
{
    if (undo_buffer::my_buffer.get_size() > 0)
    {
        list_iterator<undo_event> tmp_it = my_buffer.last();
        if (tmp_it)
        {
            (*tmp_it).is_pre_save = true;
        }
    }
}

}

