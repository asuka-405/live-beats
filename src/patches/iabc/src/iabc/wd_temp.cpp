/*
 * wd_temp.cpp - Template classes for passing data between threads.
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
#ifndef wd_temp_cpp
#define wd_temp_cpp

#include "iabc/wd_data.h"
namespace iabc
{
template <class type1>
wd_data_dx<type1>::wd_data_dx(wd_data& the_other,int the_private)
{
    ++the_number_instances;
    my_imp = 0;
}

template 
<class type1>
wd_data_dx<type1>::wd_data_dx()
{
    ++the_number_instances;
    my_imp = new wd_data_dx(*this,0);
    my_imp->add_ref();
}

template 
<class type1>
wd_data_dx<type1>::wd_data_dx(const type1& the_data)
{
    ++the_number_instances;
    wd_data_dx* new_imp = new wd_data_dx(*this,0);
    new_imp->my_data = the_data;my_imp = new_imp;
    new_imp->add_ref();
}

template 
<class type1>
wd_data_dx<type1>::~wd_data_dx()
{
    --the_number_instances;
}

template 
<class type1>
int wd_data_dx<type1>::the_number_instances = 0;

template <class type1>
bool 
wd_data_dx<type1>::is_equal(const wd_data & the_other) const
{
    const wd_data* p = (const wd_data*)(&the_other);
    const wd_data_dx* dx_other = dynamic_cast<const wd_data_dx<type1>*>(p);
    if (dx_other)
        {
        return (member_compare(*dx_other));
        }

    return false;
}

template <class type1>
bool
wd_data_dx<type1>::is_compatible(const wd_data & the_other) const
{
    const wd_data * p = (const wd_data*) &the_other;
    const wd_data_dx* dx_other = dynamic_cast<const wd_data_dx<type1>*>(p);
    if (dx_other)
        {
        return true;
        }

    return false;
}

template <class type1>
bool 
wd_data_dx<type1>::member_compare(const wd_data_dx<type1>& the_other) const
{
    if (my_data == the_other.my_data)
        {
        return true;
        }
    return false;
}

template <class type1>
type1 
wd_data_dx<type1>::extract(wd_data& the_data)
{
    const wd_data_dx<type1>* tmp_p = dynamic_cast<wd_data_dx<type1>*>(&the_data);
    if (tmp_p)
        {
        return tmp_p->my_data;
        }
    throw "Can't extract 'type1' from wd_data";
    return type1();
}

template <class Parent>
selection_handler_data<Parent>::selection_handler_data(Parent& the_parent):
    my_parent(&the_parent)
{
    my_parent->add_ref();
}

template <class Parent>
selection_handler_data<Parent>::~selection_handler_data()
{
    my_parent->remove_ref();
}

template <class Parent>
void 
selection_handler_data<Parent>::select()
{
    my_parent->handle_selection(*this);
}

template <class Parent>
selection_handler
selection_handler_dx<Parent>::get_handler(Parent& the_parent)
{
    selection_handler_data<Parent>* tmp_data = new selection_handler_data<Parent>(the_parent);
    return selection_handler(*tmp_data);
}

}

#endif

