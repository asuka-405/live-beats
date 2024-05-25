/*
 * posixthreads.cpp - Windows implementation of the generic threads interface.
 * Copyright (C) 2002 Aaron Nemwan
 * e-mail: ANewman110@aol.com
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

#include "iabc/posixthreads.h"
#include <unistd.h>

namespace iabc
{

template<>
thread_mgr*
factory<thread_mgr,int>::create()
{
	static posixthread_mgr* the_instance = 0;
    if (the_instance == 0)
    {
        the_instance = new posixthread_mgr;
    }
	return the_instance;
}

unsigned long thread_mgr::get_system_thread_id()
{
    return (unsigned long)::pthread_self();
}

void 
posixthread_mgr::sleep(int the_milliseconds)
{
  usleep(the_milliseconds * 1000);
}


bool 
posixthread_mgr::system_start(thread* the_thread,
                              unsigned long& the_system_id)
{
    int tmp_rv = ::pthread_create
             (
	      &the_system_id,
                0,
	      posixthread_mgr::entry_point,
                (void *)the_thread
             );

    return tmp_rv == 0;
}

void*
posixthread_mgr::entry_point(void* the_this)
{
    thread_mgr::entry_point(the_this);
    ::pthread_detach(::pthread_self());
    ::pthread_exit(0);
    return 0;
}

void 
posixthread_mgr::shutdown_application()
{
    thread* tmp_thread = this_thread();
    int i;
    my_mutex.seize();
    for (i=0;i<MAX_THREADS;++i)
    {
        thread* tmp_target = my_table[i].thread_ptr;
        if ((0 != tmp_target) &&
            (tmp_thread != tmp_target))
        {
	    pthread_t tmp_pthread = (pthread_t)get_system_id(tmp_target);
	    ::pthread_cancel(tmp_pthread);
        }
    }
}

}

