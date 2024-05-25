/*
 * mutex.cpp - Platform-independent thread synchronization object.
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
#include "iabc/mutex.h"

namespace iabc 
{

;
int mutex::class_instance_count = 0;
int atomic_counter::class_instance_count = 0;

mutex::mutex():
my_imp(factory<mutex_if,int>::create())
{
    ++class_instance_count;
}

void 
mutex::seize()
{
    if (is_mine() == false)
    {
        my_imp->seize();
        return;
    }
    throw("Re-seize of owned mutex!");
}

void 
mutex::release()
{
    if (is_mine() == true)
    {
        my_imp->release();
        return;
    }
    throw("Release of mutex I don't own!");
}

lock::lock(mutex& the_mutex):my_mutex(the_mutex),my_locked(false)
{
    if (my_mutex.is_mine() == false)
    {
        my_mutex.seize();
        my_locked = true;
    }
}

lock::~lock()
{
    if (my_locked == true)
    {
        my_mutex.release();
    }
}

int
atomic_counter::operator++()
{
    lock tmp_lock(my_mutex);
	return (++my_count);
}

int
atomic_counter::operator++(int ignore)
{
    return ++(*this);
}

int
atomic_counter::operator--()
{
    lock tmp_lock(my_mutex);
    return (--my_count);
}
    
int
atomic_counter::operator--(int ignore)
{
    return --(*this);
}

}
