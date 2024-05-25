/*
 * win32mutex.cpp - Windows implementation of the mutex class.
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
#include "iabc/win32mutex.h"

namespace iabc
{

mutex_if* factory<mutex_if,int>::create()
{
	return new win32mutex;
}

win32mutex::win32mutex():my_owner(0)
{
	InitializeCriticalSection(&my_cs);
}

win32mutex::~win32mutex()
{
	DeleteCriticalSection(&my_cs);
}

void win32mutex::seize()
{
	DWORD id = GetCurrentThreadId();
	if (id != my_owner)
	{
		EnterCriticalSection(&my_cs);
		my_owner = id;
	}
}

void  win32mutex::release()
{
	DWORD id = GetCurrentThreadId();
	if (id == my_owner)
	{
		my_owner = 0;
		LeaveCriticalSection(&my_cs);
	}
}

bool win32mutex::is_mine()
{
	DWORD id = GetCurrentThreadId();
    return (id == my_owner);    
}

}