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
#ifndef iabc_mutex_h
#define iabc_mutex_h
#include "iabc/factory.h"

namespace iabc
{
;
// CLASS: mutex_interface
// DESCRIPTION:
// The abstract interface to the platform-specific object.  The generic
// mutex proxies to this object, that allows us to create mutex objects
// automatically.
// IMPORTANT NOTE:
// See IMPORTANT NOTE below...
class mutex_if
{
public:
    mutex_if(){};
    virtual ~mutex_if(){};
    virtual void seize() = 0;
    virtual void release() = 0;
    virtual bool is_mine() = 0;
private:
    mutex_if& operator=(const mutex_if& o);
    mutex_if(const mutex_if& o);
};

// CLASS: mutex
// DESCRIPTION:
// A proxy to the real mutex object.  The is called the proxy pattern.
// IMPORTANT NOTE:
// This mutex may behave differently than some other mutex objects you may
// have heard tell of.  So here is how it works:
// If you seize the mutex and you don't own it, you block.
// If you release the mutex and you own it, then you don't own it anymore.
// If you seize a mutex and own it, NOTHING happens, no count is increased.
// If you release a mutex and you don't own it, nothing happens.
// If this behavior is surprising to you, then you probably want semaphore or
// lock (see below)
class mutex:public mutex_if
{
public:
    mutex();
    virtual ~mutex(){if (my_imp) delete my_imp;--class_instance_count;};
    virtual void seize();
    virtual void release();
    virtual bool is_mine(){return my_imp->is_mine();};

private:
    mutex& operator=(const mutex& o);
    mutex(const mutex& o);
    mutex_if* my_imp;
    static int class_instance_count;
};

// CLASS: lock
// DESCRIPTION:
// Allow stackable locking and unlocking of a mutex.  This allows you
// to implement re-entrant methods.  Usage:
// void my_safe_function(){
// lock tmp_lock(my_mutex);
// ... access shared stuff
// }
class lock
{
public:
    lock(mutex& the_mutex);
    ~lock();
private:
    mutex& my_mutex;
    bool my_locked;
};

// CLASS: atomic_counter
// DESCRIPTION:
// Implement a threadsafe decrement-and-read operation.  This avoids
// nasty race conditions when implementing reference counters across multiple
// threads.
class atomic_counter
{
public:
    atomic_counter(int count=0):my_count(count){++class_instance_count;};
    ~atomic_counter(){--class_instance_count;};
    int operator++(void);
    int operator++(int ignore);
    int operator--(void);
    int operator--(int ignore);
    // Note:  not threadsafe
    int as_int() const{return my_count;};
private:
    mutex my_mutex;
    int my_count;
    atomic_counter(const atomic_counter& o);
    atomic_counter& operator=(const atomic_counter& o);
    static int class_instance_count;
};
}

#endif



