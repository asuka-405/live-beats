/*
 * threads.cpp - Platform-independent thread stuff.
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
#include "iabc/threads.h"
#include "stdio.h"
#include "iabc/winres.h"

namespace iabc
{
using namespace iabc;

const char* get_current_thread_name()
{
    thread* tmp_thread = thread::this_thread();
    if (tmp_thread)
    {
        return tmp_thread->get_label();
    }
    return "(null)";
}

thread::thread(const char* the_label):
my_thread_sem(0),my_label(the_label)
{
    if (my_label == 0)
    {
        char* tmp_label = new char[11];
		my_label = tmp_label;
        sprintf(tmp_label,"0x%x",(unsigned long)this);
    }
}
thread::~thread()
{
}
    
void
thread::add_ref()
{
    ++my_thread_ref;
}

void
thread::remove_ref()
{
    if (--my_thread_ref == 0)
        {
        delete this;
        }
}

bool 
thread::start()
{
    add_ref();
    thread_mgr& mgr = thread_mgr::get_instance();
    return (mgr.start(this));
}

thread&
thread::get_system_thread()
{
    static thread system_thread;
    return system_thread;
}

thread*
thread::this_thread()
{
	return thread_mgr::get_instance().this_thread();
}
void
thread::sleep(int the_milliseconds)
{
	thread_mgr::get_instance().sleep(the_milliseconds);
};

thread_mgr::~thread_mgr(){};

void
thread_mgr::gc()
{
    delete &(thread_mgr::get_instance());
}
thread_mgr& 
thread_mgr::get_instance()
{
    static thread_mgr* the_instance = 0;
    if (the_instance == 0)
    {
        the_instance = factory<thread_mgr,int>::create();
    }
    return *(the_instance);
}

bool
thread_mgr::start(thread* the_thread)
{
    lock tmp_lock(my_mutex);
    unsigned long tmp_id;
    if (system_start(the_thread,tmp_id) == true)
        {
        int i = 0;
        while (i < MAX_THREADS)
            {
            if (my_table[i].thread_ptr == 0)
                {
                my_table[i].thread_ptr = the_thread;
                the_thread->my_system_id = tmp_id;
                the_thread->my_thread_sem.release();
                return true;
                }
            ++i;
            }
        }

    return false;
}

void 
thread_mgr::entry_point(void* p)
{
    thread* t = (thread*)p;
	thread_mgr& mgr = get_instance();
    t->my_thread_sem.seize();
    t->entry_point();
    mgr.my_mutex.seize();
    int i = 0;
    while (i < MAX_THREADS)
        {
        if (mgr.my_table[i].thread_ptr == t)
            {
            mgr.my_table[i].thread_ptr = 0;
            break;
            }
        ++i;
        }
    mgr.my_mutex.release();
    t->remove_ref();
}

unsigned long
thread_mgr::get_system_id(thread* the_thread)
{
    return the_thread->my_system_id;
}

thread* 
thread_mgr::this_thread()
{
    thread_mgr& tmp_instance = get_instance();
    unsigned long tmp_id = tmp_instance.get_system_thread_id();

    int i = 0;
    while (i < MAX_THREADS)
        {
        if ((tmp_instance.my_table[i].thread_ptr != 0) &&
            (tmp_instance.my_table[i].thread_ptr->my_system_id == tmp_id))
            {
            return tmp_instance.my_table[i].thread_ptr;
            }
        ++i;
        }

    return 0;
}

thread_mgr::thread_mgr()
{
    int i=0;
    while (i < MAX_THREADS)
        {
        my_table[i].thread_ptr = 0;
		++i;
        }

    thread* system = &(thread::get_system_thread());
    system->my_system_id = thread_mgr::get_system_thread_id();
    my_table[0].thread_ptr = system;
}

}

