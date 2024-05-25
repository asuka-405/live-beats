/*
 * win32threads.cpp - Windows implementation of the generic threads interface.
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
#include <windows.h>
#include <process.h>
#include "iabc/win32threads.h"
#include "iabc/event_log.h"
#include "iabc/message_box.h"
#include <stdio.h>

void   
win32_exception(_EXCEPTION_POINTERS* ep)
{
    static int entry = 0;
    if (entry != 0)
        abort();
    entry = 1;
    ::MessageBox(0,"Error","Exception! - See c:\\evlog.txt",MB_OK);
#ifndef GCC
    FILE* f = fopen("C:\\evlog.txt","w");
#else
    FILE* f = fopen("/tmp/evlog.txt","w");
#endif
    iabc::all_event_logs::print_logs(f);
}


namespace iabc
{
win32thread_mgr::win_thread_entry
win32thread_mgr::win_thread_table[MAX_THREADS];	
// using namespace iabc;

thread_mgr*
factory<thread_mgr,int>::create()
{
	static win32thread_mgr* the_instance = 0;
    if (the_instance == 0)
    {
        the_instance = new win32thread_mgr;
    }

	return the_instance;
}

unsigned long thread_mgr::get_system_thread_id()
{
    return ::GetCurrentThreadId();
}

bool 
win32thread_mgr::system_start(thread* the_thread,
                              unsigned long& the_system_id)
{
	unsigned int tmp_id;
    HANDLE tmp_handle = (void*)_beginthreadex(0,0,
        win32thread_mgr::entry_point,
        (void*) the_thread,0,&tmp_id);
    SetThreadPriority(tmp_handle,THREAD_PRIORITY_NORMAL);
	
	the_system_id = (unsigned long)tmp_id;

    int i;
    for (i=0;i<MAX_THREADS;++i)
    {
        if (win_thread_table[i].thread_ptr == 0)
        {
            win_thread_table[i].thread_ptr = the_thread;
            win_thread_table[i].handle = tmp_handle;
            break;
        }
    }
    
    if (tmp_handle == 0)
        {
        return false;
        }

    return true;
}

unsigned int WINAPI
win32thread_mgr::entry_point(LPVOID lpParameter)
{
    __try 
    {
    thread_mgr::entry_point(lpParameter);
    }
    __except(win32_exception(GetExceptionInformation()))
    {
    };

    int i;
    for (i=0;i<MAX_THREADS;++i)
    {
        if (lpParameter == 
			(LPVOID)win32thread_mgr::win_thread_table[i].thread_ptr)
        {
            win32thread_mgr::win_thread_table[i].thread_ptr = 0;
            ::_endthread();
        }
    }
    return 0;
}

void 
win32thread_mgr::shutdown_application()
{
    thread* tmp_thread = this_thread();
	int i;
    for (i=0;i<MAX_THREADS;++i)
    {
        if ((0 != 
			(LPVOID)win32thread_mgr::win_thread_table[i].thread_ptr) &&
            (tmp_thread !=
             (LPVOID)win32thread_mgr::win_thread_table[i].thread_ptr))
        {
			::SuspendThread(win32thread_mgr::win_thread_table[i].handle);
			::TerminateThread(win32thread_mgr::win_thread_table[i].handle,-1);
            win32thread_mgr::win_thread_table[i].thread_ptr = 0;
        }
    }

}

}

