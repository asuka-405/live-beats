 /*
 * list.cpp - An STL-like list and related container classes.
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
#ifndef iabc_list_cpp
#define iabc_list_cpp

#include "iabc/list.h"

namespace iabc
{
;
// Not sure why GCC doesn't like this static:
#ifdef GCC
  // int max_node_instances = 0;
  // int max_list_instances = 0;
#else
static int max_node_instances = 0;
static int max_list_instances = 0;
#endif

template <class T>
int
list_data<T>::my_instances = 0;
template <class T>
int
list_node<T>::my_instances = 0;

template <class T>
list_node<T>::list_node()
{
    ++my_instances;
#ifndef GCC
    if (my_instances > max_node_instances)
    {
        max_node_instances = my_instances;
    }
#endif
}

template <class T>
list_node<T>::~list_node()
{
    --my_instances;
}

template <class T>
list_data<T>::list_data ():my_size(0),my_ref(0)
{
	++my_instances;
#ifndef GCC
    if (my_instances > max_list_instances)
    {
        max_list_instances = my_instances;
    }
#endif
    my_head = new list_node<T>;
    my_head->my_next = my_head;
    my_head->my_last = my_head;
}


template <class T>
list_data<T>::~list_data ()
{
	--my_instances;
    list_node<T>* tmp_head = my_head;
    while (my_head->my_next != tmp_head)
    {
        list_node<T>* goner = my_head;
        my_head = my_head->my_next;
        delete goner;
    }
	delete my_head;
}

template <class T>
void 
list_data<T>::insert_after (list_node<T>* old_node, list_node<T>* new_node)
{
    old_node->my_next->my_last = new_node;
    new_node->my_last = old_node;
    new_node->my_next = old_node->my_next;
    old_node->my_next = new_node;
    ++my_size;
}

template <class T>
void
list_data<T>::insert_before (list_node<T>* old_node, list_node<T>* new_node)
{
    old_node->my_last->my_next = new_node;
    new_node->my_next = old_node;
    new_node->my_last = old_node->my_last;
    old_node->my_last = new_node;
    ++my_size;
}

template <class T>
void list_data<T>::remove (list_node<T>* pnode)
{
	pnode->my_next->my_last = pnode->my_last;
	pnode->my_last->my_next = pnode->my_next;
    --my_size;
}

template <class T>
list_iterator<T>::list_iterator():my_list(0){};

template <class T>
list_iterator<T>::~list_iterator ()
{
    if (my_list)
        my_list->remove_ref();
}

template <class T>
list_iterator<T>::list_iterator (const list_iterator& the_other)
{
    my_list = the_other.my_list;
    my_current = the_other.my_current;
    my_list->add_ref();
}

template <class T>
void 
list_iterator<T>::add_after (const T& refOther)
{
    list_node<T>* new_node = new list_node<T>(refOther);
    my_list->insert_after(my_current,new_node);
}

template <class T>
void 
list_iterator<T>::add_before (const T& refOther)
{
    list_node<T>* new_node = new list_node<T>(refOther);
    my_list->insert_before(my_current,new_node);
}

template <class T>
void 
list_iterator<T>::delete_current ()
{
    if (is_end() == false)
    {
        list_node<T>* tmp_current = my_current;
        my_current = my_current->my_next;
        my_list->remove(tmp_current);
        delete tmp_current;
    }
}

template <class T>
void 
list_iterator<T>::reset_to_front()
{
    my_current = my_list->get_head()->my_next;
}

template <class T>
void 
list_iterator<T>::reset_to_back()
{
    my_current = my_list->get_head()->my_last;
}

template <class T>
long 
list_iterator<T>::get_size() const
{
    if (my_list)
    {
        return my_list->get_size();
    }
    return 0;
}

template <class T>
bool 
list_iterator<T>::is_end() const
{
    if (my_list)
    {
        bool tmp_rv = false;
        if (my_list->is_end(*my_current))
        {
            tmp_rv = true;
        }
        return tmp_rv;
    }

    return true;
}

template <class T>
T& 
list_iterator<T>::operator* ()
{
    if (is_end() == true)
    {
        throw ("Null pointer in iterator!");
    }
    return (get_current());
}

template <class T>
T 
list_iterator<T>::operator*  () const
{
    if (is_end() == true)
    {
        throw ("Null pointer in iterator!");
    }
    return &(get_current());
}

template <class T>
list_iterator<T>::operator T* ()
{
    if (is_end() == true)
    {
        return 0;
    }
    return &(get_current());
}

template <class T>
list_iterator<T>::operator const T*() const
{
    if (is_end() == true)
    {
        return 0;
    }
    return &(get_current());
}

template <class T>
void 
list_iterator<T>::next()
{
    my_current = my_current->my_next;
}

template <class T>
void 
list_iterator<T>::previous()
{
    my_current = my_current->my_last;
}

template <class T>
list_iterator<T>& 
list_iterator<T>::operator=(const list_iterator& o)
{
    if (my_list) my_list->remove_ref();
    my_list = o.my_list;
    if (o.my_list) 
    {
        my_list->add_ref();
        my_current = o.my_current;
    }
    return *this;
}

template <class T>
list<T>& 
list<T>::operator=(const list<T>& o)
{
    if (my_data)
    {
        my_data->my_ref--;
        if (my_data->my_ref == 0)
        {
            delete my_data;
        }
        my_data = 0;
    }

    this->my_data = o.my_data;
    my_data->my_ref++;

    return *this;
}

template <class T>
list<T>::list(const list<T>& o)
{
    my_data = o.my_data;
    my_data->my_ref++;
}

template <class T>
list_node<T>* list_data<T>::get_head ()
{
   return my_head;
}

template <class T>
typename list<T>::iterator
list<T>::first()
{
    typename list<T>::iterator rv;
    rv.my_list = my_data;
    ++my_data->my_ref;
    rv.reset_to_front();
    return rv;
}

template <class T>
typename list<T>::iterator
list<T>::last()
{
    typename list<T>::iterator rv;
    rv.my_list = my_data;
    ++my_data->my_ref;
    rv.reset_to_back();
    return rv;
}

// Search through the_list.  Stop searching when compare_func(current_node,compare_value)
// equals compare_test, or when you get to the end
template <class type_t,class predicate>
typename list<type_t>::iterator
findit_predicate(list<type_t>& the_list,predicate& the_predicate)
{
    typename list<type_t>::iterator tmp_list(the_list.first());
    while (tmp_list.is_end() == false)
    {
        if (the_predicate(*tmp_list) == true)
        {
            break;
        }
        tmp_list.next();
    }
    return tmp_list;
}

// Search through the_list.  Stop searching when compare_func(current_node,compare_value)
// equals compare_test, or when you get to the end
template <class type_t,class predicate>
typename list<type_t>::iterator
findit_predicate(typename list<type_t>::iterator& the_list,predicate& the_predicate)
{
    typename list<type_t>::iterator& tmp_list = the_list;
    while (tmp_list)
    {
        if (the_predicate(*tmp_list) == true)
        {
            break;
        }
        tmp_list.next();
    }
    return tmp_list;
}

template <class type_t,class predicate>
typename list<type_t>::iterator
findit_forwards(typename list<type_t>::iterator& the_list,predicate& the_predicate)
{
    while (the_list.is_end() == false)
    {
        if (the_predicate(*the_list) == true)
        {
            break;
        }
        the_list.next();
    }

    return the_list;
}

template <class type_t,class predicate>
typename list<type_t>::iterator
findit_backwards(typename list<type_t>::iterator& the_list,predicate& the_predicate)
{
    while (the_list)
    {
        if (the_predicate(*the_list) == true)
        {
            break;
        }
        the_list.previous();
    }

    return the_list;
}

template <class type_t,class functor>
void
do_to_all(list<type_t>& the_list,functor& the_functor)
{
    typename list<type_t>::iterator tmp_list = the_list.first();
    while (tmp_list)
    {
        the_functor(*tmp_list);
        tmp_list.next();
    }
}

// Search through the list until the current node == the_test or the list ends
template <class list_t,class compare_t>
typename list<list_t>::iterator
findit(list<list_t>& the_list,const compare_t& the_test)
{
    typename list<list_t>::iterator tmp_list =  the_list.first();
    while (tmp_list.is_end() == false)
    {
        if ((*tmp_list) == the_test)
        {
            break;
        }
        tmp_list.next();
    }
    return tmp_list;
}

// remove the_elem from the_list, if found
template <class type_t>
void
remove_found_ref(list<type_t>& the_list,const type_t& the_elem)
{
    typename list<type_t>::iterator tmp_cursor = findit(the_list,the_elem);
    if (tmp_cursor)
    {
        tmp_cursor.delete_current();
    }
}

// return a smart_copy to the first element in the list
template <class type_t>
void
remove_first_ref(list<type_t>& the_list,type_t& the_element)
{
    typename list<type_t>::iterator tmp_ref = the_list.first();
    
    tmp_ref.reset_to_front();
    if (tmp_ref)
    {
        the_element = (*tmp_ref);
        tmp_ref.delete_current();
    }
}

// Search through the list until the (*current node) == the_test or the list ends
template <class list_t,class compare_t>
typename list<list_t>::iterator
findit_deref(list<list_t>& the_list,const compare_t& the_test)
{
    typename list<list_t>::iterator tmp_list(the_list.first());
    while (tmp_list.is_end() == false)
    {
        if ((*(*tmp_list)) == the_test)
        {
            break;
        }
        tmp_list.next();
    }
    return tmp_list;
}

template <class list_t>
void
remove_all(list<list_t>& the_list)
{
    typename list<list_t>::iterator tmp_cursor = the_list.first();
    while (tmp_cursor.is_end() == false)
    {
        tmp_cursor.delete_current();
    }
}

template <class list_t>
void
add_at_beginning(list<list_t>& the_list,const list_t& the_element)
{
    typename list<list_t>::iterator tmp_cursor = the_list.first();
    tmp_cursor.add_before(the_element);
}

template <class list_t>
void
add_at_end(list<list_t>& the_list,const list_t& the_element)
{
    typename list<list_t>::iterator tmp_cursor = the_list.last();
    tmp_cursor.add_after(the_element);
}

template <class list_t>
void
push(list<list_t>& the_list,const list_t& the_element)
{
    add_at_end(the_list,the_element);
}

template <class list_t>
list_t
pop(list<list_t>& the_list)
{
    list_t tmp_rv;
    if (the_list.get_size() > 0)
    {
        typename list<list_t>::iterator tmp_cursor = the_list.last();
        if (tmp_cursor)
        {
			tmp_rv = (*tmp_cursor);
			tmp_cursor.delete_current();
            return tmp_rv;
        }
    }
    return tmp_rv;
}

template <class type_t>
extern
type_t
deque(list<type_t>& the_list)
{
    type_t tmp_rv;
    if (the_list.get_size() > 0)
    {
        typename list<type_t>::iterator tmp_cursor = the_list.first();
        if (tmp_cursor)
        {
			tmp_rv = (*tmp_cursor);
			tmp_cursor.delete_current();
            return tmp_rv;
        }
    }
    return tmp_rv;
}


}
#endif
