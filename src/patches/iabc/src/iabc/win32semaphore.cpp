/*
 * win32semaphore.cpp - Windows implementation of the generic semaphore interface.
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
#include "factory.h"
#include "semaphore.h"
#include "win32semaphore.h"

namespace iabc
{

semaphore_if*
factory<semaphore_if,int>::create(int p1)
{
    return new win32semaphore(p1);
}

win32semaphore::win32semaphore(int the_initial_count)
{
    my_sem_handle = ::CreateSemaphore(NULL,the_initial_count,5,NULL);
}

win32semaphore::~win32semaphore()    
{
    ::CloseHandle(my_sem_handle);
}
void 
win32semaphore::seize()
{
    ::WaitForSingleObject(my_sem_handle,INFINITE);
}

void 
win32semaphore::release()
{
    ::ReleaseSemaphore(my_sem_handle,1,0);
}

}
