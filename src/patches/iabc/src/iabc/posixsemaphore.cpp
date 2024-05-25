/*
 * posixsemaphore.cpp - Windows implementation of the generic semaphore interface.
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
#include "/usr/include/semaphore.h"
#include "iabc/posixsemaphore.h"

namespace iabc
{

template<>
semaphore_if*
factory<semaphore_if,int>::create(int p1)
{
    return new posixsemaphore(p1);
}

posixsemaphore::posixsemaphore(int the_initial_count)
{

    int rv = ::sem_init (&my_sem_handle, 0, the_initial_count);
}

posixsemaphore::~posixsemaphore()    
{
    sem_destroy (&my_sem_handle);
}

void 
posixsemaphore::seize()
{
    sem_wait(&my_sem_handle);
}

void 
posixsemaphore::release()
{
    sem_post(&my_sem_handle);
}

}
