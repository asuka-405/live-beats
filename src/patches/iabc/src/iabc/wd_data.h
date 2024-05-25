/*
 * wd_data.h - Template classes for passing data between threads.
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
#ifndef iabc_wd_data_h
#define iabc_wd_data_h
#include "iabc/string.h"
#include "iabc/mutex.h"
#include "iabc/simple.h"

// FILE: wx_data
// DESCRIPTION:
// Template files for handling notification of concurrent
// things.  This is various interpretations of the observer
// pattern.  
namespace iabc
{
;
// CLASS: wd_data
// DESCRIPTION:
// Encapsulate a multi-type container.  This class acts as a 
// proxy to another derived wd_data, which contains an instance
// of some type, possible a built in type, which is passed as
// a parameter to a dispatcher function.
class wd_data
{
public:
    wd_data();
    wd_data(wd_data& the_other);
    virtual ~wd_data();
    wd_data& operator =(wd_data & the_other);
    bool operator == (const wd_data & the_other) const
    {
        return is_equal(the_other);
    }
    wd_data * operator &() {if (my_imp) return my_imp;else return this;};
    const wd_data * operator &() const {if (my_imp) return my_imp;else return this;};
protected:
    virtual bool is_equal(const wd_data & the_other) const { return false;};
    virtual bool is_compatible(const wd_data & the_other) const{return false;};
    void add_ref(){if (my_imp) ++my_imp->my_count;else ++my_count;};
    void remove_ref(){
        if ((my_imp) &&(--my_imp->my_count == 0)) delete my_imp;
        else if ((my_imp == 0) && (--my_count == 0)) delete this; };
    atomic_counter my_count;
    wd_data * my_imp;
    static int the_number_instances;
};

// CLASS: wd_data_dx
// DESCRIPTION:
// A derived wd_data, with the parameterized type which detemrines
// what the real type is.  It can be used in the place of a
// <type1> parameter to a dispatcher call.
template 
<class type1>
class wd_data_dx:public wd_data
{
public:
    wd_data_dx();
    wd_data_dx(const type1& the_data);
    virtual ~wd_data_dx();
    operator type1(){return my_data;};
    static type1 extract(wd_data& the_data);
protected:
    wd_data_dx(wd_data& the_other,int the_private);
    bool member_compare(const wd_data_dx<type1>& the_other) const;
    virtual bool is_equal(const wd_data & the_other) const;
    virtual bool is_compatible(const wd_data & the_other) const;
    static int the_number_instances;
    type1 my_data;
};

// CLASS: selection_handler
// DESCRIPTION:
// Abstract class that allows a client to get reports of GUI things to him
// using the observer pattern.
class selection_handler_if
{
public:
    selection_handler_if();
    virtual void select() = 0;
    virtual ~selection_handler_if();
    void add_ref();
    void remove_ref();
private:
    atomic_counter my_count;
    selection_handler_if& operator=(selection_handler_if& o);
    selection_handler_if(const selection_handler_if& o);
    static int class_instance_count;
};


// CLASS: selection_handler_dx_data
// DESCRIPTION:
// This is a template class that allows the client to be
// a selection handler (e.g. receive selection events) 
// without having to inherit from selection_handler himself.
// But he needs to implement the handle_selection method.
template <class Parent>
class selection_handler_data:public selection_handler_if
{
public:
    // METHOD: ctor
    // DESCRIPTION:
    // create the class with the object that is getting notified
    // as the parent.
    selection_handler_data(Parent& the_parent);
    
    // METHOD: dtor
    virtual ~selection_handler_data();

    // METHOD: select
    // DESCRIPTION:
    // This is the method the widget calls to get the events to the user.
    // This is what calls the handle_selection method.
    virtual void select();

private:
    // ATTRIBUTE: my_paremt
    // DESCRIPTION:
    // The client class that we notify.
    Parent* my_parent;

    selection_handler_data& operator=(const selection_handler_data& o);
    selection_handler_data(const selection_handler_data& o);
};

// CLASS: selection_handler
// DESCRIPTION:
// This is the type that the client creates.  Usage:
// my_tree_ctrl->add_selection_handler(selection_handler_dx<my_type>(*this));
class selection_handler:public selection_handler_if
{
public:
    // METHOD: ctor
    // DESCRIPTION:
    // Create the data that is reference counted and initiaze
    // it with this parent.  If the parameter is another
    // instance, add ref to my data object.
    selection_handler();
    selection_handler(selection_handler_if& the_handler);
    selection_handler(const selection_handler& o);
    selection_handler& operator=(const selection_handler& o);

    // METHOD: dtor
    // DESCRIPTION:
    // Remove the reference count and delete the object if 
    // need be.
    virtual ~selection_handler();

    // METHOD: select
    // DESCRIPTION:
    // Call the select method of the data object.
    virtual void select();

private:                    
    selection_handler_if* my_data;
};

template <class Parent>
class selection_handler_dx
{
public:
    static selection_handler get_handler(Parent& the_parent);
};

}


#endif

