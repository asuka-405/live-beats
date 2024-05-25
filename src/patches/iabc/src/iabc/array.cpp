/*
 * array.cpp - An STL-like array template class.
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
#include "iabc/array.h"
#include "iabc/mutex.h"

namespace iabc
{
;
static int max_array_instances = 0;

template <class T>
int array_data<T>::my_instances = 0;

template <class T>
array_data<T>::~array_data()
{
    --my_instances;
    delete [] my_array;
}

template <class T>
array_data<T>::array_data(int the_initial_size,int the_grow_size):
    my_size(0),my_limit(0),my_array(0),my_grow_size(the_grow_size),
    my_count(0)
{
    ++my_instances;
    if (max_array_instances < my_instances)
    {
        max_array_instances = my_instances;
    }
    if (the_initial_size) 
        expand(the_initial_size);
}

template <class T>
void 
array_data<T>::expand_to(int the_index)
{
    if (my_size < the_index + 1)
    {
        expand((the_index  - my_size));
    }
}

template <class T>
void 
array_data<T>::expand(int num_elems)
{
    if (num_elems + my_size > my_limit)
    {
        if (my_grow_size < num_elems)
        {
            my_limit += num_elems;
        }
        else
        {
            my_limit += my_grow_size;
        }        
        T* new_array = new T[my_limit];
        for (int i = 0; i < my_size; ++ i)
        {
            new_array[i] = my_array[i];
        }

        delete [] my_array;
        my_array = new_array;
        my_grow_size *= 2;
    }
    my_size += num_elems;
}

template <class T>
void
array_data<T>::remove_all()
{
    my_size = 0;
}

template <class T>
array<T>::array(const array<T>& the_other)
{
    set_my_array(the_other);    
}

template <class T>
array<T>::operator T*()
{
    if (my_array->get_size() == 0)
    {
        return 0;
    }
    return my_array->my_array;
}

template <class T>
array<T>::operator const T*()const
{
    if (my_array->get_size() == 0)
    {
        return 0;
    }
    return my_array->my_array;
}

template <class T>
array<T>& 
array<T>::operator=(const array<T>& the_other)
{
	if (my_array != the_other.my_array)
	{
		my_array->remove_ref();
		set_my_array(the_other);    
	}
    return *this;
}

template <class T>
void deep_copy(const array<T>& the_input,array<T>& the_output)
{
    int index = the_output.get_size();
    the_output.expand(the_input.get_size());
    
    int i;
    for (i = 0;i < the_input.get_size(); ++ i)
    {
        the_output[index + i] = 
			the_input[i];
    }
}

template <class type_t,class functor>
void
do_to_all(array<type_t>& the_list,functor& the_predicate)
{
    for (int i = 0;i < the_list.get_size();++i)
    {
        the_predicate(the_list[i]);
    }
}

template <class type_t>
void 
array<type_t>::add_to_end(const type_t& the_elem)
{
    int tmp_index = get_size();
    expand(1);
    (*this)[tmp_index] = the_elem;
}

}
