/*
 * wd_data.cpp - Base class for data that is passed between threads.
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
#include "wd_data.h"
#include "iabc/event_log.h"

namespace iabc
{
wd_data& 
wd_data::operator =(wd_data & the_other)
{
    if ((my_imp) &&
        (my_imp->is_compatible(the_other) == true))
    {
        my_imp->remove_ref();
    }
    my_imp = &the_other;
    my_imp->add_ref();
    return *this;
}

wd_data::~wd_data ()
{
    --the_number_instances;
    if (my_imp) my_imp->remove_ref();
}

wd_data::wd_data()
{
    ++the_number_instances;
    my_imp = 0;
}

wd_data::wd_data(wd_data& the_other)
{
    ++the_number_instances;
    my_imp = &the_other;
    my_imp->add_ref();
}

int 
wd_data::the_number_instances = 0;

selection_handler_if::selection_handler_if()
{    
    ++class_instance_count;
}

selection_handler_if::~selection_handler_if()
{
    --class_instance_count;
}

void
selection_handler_if::add_ref()
{
    ++my_count;
}

void
selection_handler_if::remove_ref()
{
    if (--my_count == 0) delete this;
}

int selection_handler_if::class_instance_count = 0;

selection_handler::selection_handler():my_data(0)
{
    global_doc_event_log.log_event(doc_event_data::selection_ctor,
                                   (unsigned long)this,
                                   (unsigned long)my_data);
}

selection_handler::selection_handler(selection_handler_if& the_handler):
my_data(&the_handler)
{
    global_doc_event_log.log_event(doc_event_data::selection_ctorp1,
                                   (unsigned long)this,
                                   (unsigned long)my_data);
	my_data->add_ref();
}

selection_handler::selection_handler(const selection_handler& o):
my_data(o.my_data)
{
    global_doc_event_log.log_event(doc_event_data::selection_cctor,
                                   (unsigned long)this,
                                   (unsigned long)my_data);
    if (my_data)
    {
        my_data->add_ref();
    }
}

selection_handler& 
selection_handler::operator=(const selection_handler& o)
{
    global_doc_event_log.log_event(doc_event_data::selection_opeq,
                                   (unsigned long)this,
                                   (unsigned long)my_data,
                                   (unsigned long)&o,
                                   (unsigned long)o.my_data);
    if (my_data != o.my_data)
    {
        if (my_data)
        {
            my_data->remove_ref();
        }
        my_data = o.my_data;
        if (my_data)
        {
            my_data->add_ref();
        }
    }

    return *this;
}

selection_handler::~selection_handler()
{
    global_doc_event_log.log_event(doc_event_data::selection_cctor,
                                   (unsigned long)this);
    if (my_data)
    {
        my_data->remove_ref();
    }
}

void 
selection_handler::select()
{
    global_doc_event_log.log_event(doc_event_data::select_list,
        (unsigned long)this,(unsigned long)my_data);
    if (my_data)
    {
        my_data->select();
    }
}

}


