/*
 * map.cpp - An STL-like map container class.
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
#include "iabc/map.h"

namespace iabc
{
;
// Appease the fickle gods of template instantiation.
// GCC can't/won't deduce template arguments so specify them.
#define INSTANTIATE_MAP(t1,t2) \
template class kv_pair<t1,t2>; \
template class map_iterator<t1,t2>; \
template class map<t1,t2>;

template <class ordinal,class T>
int map_data<ordinal,T>::my_instances = 0;

template <class ordinal,class T>
map_data<ordinal,T>::map_data ():my_size(0),my_ref(0)
{
	++my_instances;
    my_head = new kv_pair<ordinal,T>;
    my_head->my_next = my_head;
    my_head->my_last = my_head;
}


template <class ordinal,class T>
map_data<ordinal,T>::~map_data ()
{
	--my_instances;
    kv_pair<ordinal,T>* tmp_head = my_head;
    while (my_head->my_next != tmp_head)
    {
        kv_pair<ordinal,T>* goner = my_head;
        my_head = my_head->my_next;
        delete goner;
    }
	delete my_head;
}

template <class ordinal,class T>
void 
map_data<ordinal,T>::insert_after (kv_pair<ordinal,T>* old_node, kv_pair<ordinal,T>* new_node)
{
    old_node->my_next->my_last = new_node;
    new_node->my_last = old_node;
    new_node->my_next = old_node->my_next;
    old_node->my_next = new_node;
    ++my_size;
}

template <class ordinal,class T>
void
map_data<ordinal,T>::insert_before (kv_pair<ordinal,T>* old_node, kv_pair<ordinal,T>* new_node)
{
    old_node->my_last->my_next = new_node;
    new_node->my_next = old_node;
    new_node->my_last = old_node->my_last;
    old_node->my_last = new_node;
    ++my_size;
}

template <class ordinal,class T>
void map_data<ordinal,T>::remove (kv_pair<ordinal,T>* pnode)
{
    pnode->my_next->my_last = pnode->my_last;
	pnode->my_last->my_next = pnode->my_next;
    --my_size;
}

template<class ordinal,class T>
map_iterator<ordinal,T>::map_iterator():
my_data(new map_data<ordinal,T>),
    my_current(0)
{
    my_data->add_ref();
}

template<class ordinal,class T>
map_iterator<ordinal,T>::~map_iterator()
{
    if (my_data)
        {
        my_data->remove_ref();
        }
}

// Copy ctor, copy my list to o's list
template<class ordinal,class T>
map_iterator<ordinal,T>::map_iterator(const map_iterator& o):
my_data(o.my_data),my_current(o.my_current)
{
    if (my_data)
        {
        my_data->add_ref();
        }
}

// Copy ctor, copy my list to o's list
template<class ordinal,class T>
map_iterator<ordinal,T>& 
map_iterator<ordinal,T>::operator=(const map_iterator<ordinal,T>& o)
{
	my_current = o.my_current;
	if (my_data != o.my_data)
	{
		my_data->remove_ref();
		my_data = o.my_data;
		if (my_data)
			{
			my_data->add_ref();
			}
	}
    return *this;
}
    
template<class ordinal,class T>
map_iterator<ordinal,T>
map_iterator<ordinal,T>::get_item(const ordinal& the_key,map_match_type the_match)
{
    (*this) = map<ordinal,T>::findit(the_key,the_match,*this);    
    return *this;
}

template<class ordinal,class T>
kv_pair<ordinal,T>& 
map_iterator<ordinal,T>::operator* ()
{
    if (my_current)
    {
        return (*my_current);
    }
    else
        throw "Null pointer in map class!";
}

template<class ordinal,class T>
kv_pair<ordinal,T> 
map_iterator<ordinal,T>::operator*  () const
{
    if (my_current)
    {
        return (*my_current);
    }
    else
        throw "Null pointer in map class!";
}

// METHOD: T*
// DESCRIPTION:
// Allow the list to be checked for validity in the pointer way, 
// e.g.:
// if (my_iterator) {       // check for end of list.
// tmp_data = *my_iterator; // now this is safe
template<class ordinal,class T>
map_iterator<ordinal,T>::operator kv_pair<ordinal,T>* ()
{
    if ((my_current) && (is_end() == false))
    {
        return &(*my_current);
    }
    return 0;
}

template<class ordinal,class T>
map_iterator<ordinal,T>::operator const 
kv_pair<ordinal,T>*() const
{
    if (my_current)
    {
        return &(*my_current);
    }
    return 0;
}

template<class ordinal,class T>
void map_iterator<ordinal,T>::next()
{
    my_current = my_current->my_next;
}


template<class ordinal,class T>
map_iterator<ordinal,T>& map_iterator<ordinal,T>::most()
{
    my_current = my_data->my_head->my_last;
    return *this;
}

template<class ordinal,class T>
map_iterator<ordinal,T>& map_iterator<ordinal,T>::least()
{
    my_current = my_data->my_head->my_next;
    return *this;
}

template<class ordinal,class T>
void
map_iterator<ordinal,T>::previous()
{
    my_current = my_current->my_last;
}

template<class ordinal,class T>
void map_iterator<ordinal,T>::add_after (const ordinal& o,const T& t)
{
    kv_pair<ordinal,T>* p = new kv_pair<ordinal,T>(o,t);
    my_data->insert_after(my_current,p);
}

template<class ordinal,class T>
void map_iterator<ordinal,T>::add_before (const ordinal& o,const T& t)
{
    kv_pair<ordinal,T>* p = new kv_pair<ordinal,T>(o,t);
    my_data->insert_before(my_current,p);
}

template<class ordinal,class T>
void map_iterator<ordinal,T>::delete_current ()
{
    if (is_end() == false)
    {
        kv_pair<ordinal,T>* tmp_current = my_current;
        my_current = my_current->my_next;
        my_data->remove(tmp_current);
        delete tmp_current;
    }
}

template<class ordinal,class T> 
void map_iterator<ordinal,T>::reset_to_front()
{
    if (my_data)
        {
        my_current = my_data->my_head->my_next;
        }
}

template<class ordinal,class T>
void map_iterator<ordinal,T>::reset_to_back()
{
    if (my_data)
        {
        my_current = my_data->my_head->my_last;
        }
}

template<class ordinal,class T>
long map_iterator<ordinal,T>::get_size() const
{
    if (my_data)
        {
        return my_data->get_size();
        }
    return 0;
}

template <class ordinal,class T,class predicate>
map_iterator<ordinal,T>
findit_forwards(map_iterator<ordinal,T>& the_list,predicate& the_predicate)
{
    while (the_list)
    {
        if (the_predicate(*the_list) == true)
        {
            break;
        }
        the_list.next();
    }

    return the_list;
}

template <class ordinal,class T,class predicate>
map_iterator<ordinal,T>
findit_backwards(map_iterator<ordinal,T>& the_list,predicate& the_predicate)
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


template<class ordinal,class T>
bool map_iterator<ordinal,T>::is_end() const
{
    if (my_data)
    {
        bool tmp_rv = false;
        if (my_data->is_end(*my_current))
        {
            tmp_rv = true;
        }
        return tmp_rv;
    }

    return true;
}

template<class ordinal,class T>
bool 
map_equality_pred<ordinal,T>::operator()(const kv_pair<ordinal,T>& the_item)
{
  bool rv = false;
  switch (my_match)
    {
    case lt:
      rv = (the_item.key < my_key);
      break;
    case gt:
      rv = (the_item.key > my_key);
      break;
    case gteq:
      rv = (the_item.key >= my_key);
      break;
    case lteq:
      rv = (the_item.key <= my_key);
      break;
    case exact:
    default:
      rv = (the_item.key == my_key);
      break;
    }

  return rv;
}

template <class ordinal,class T> 
map<ordinal,T>::map()
{
}

template <class ordinal,class T> 
map<ordinal,T>::map(const map& o)
{
    my_last_cursor = o.my_last_cursor;
}

template <class ordinal,class T> 
map<ordinal,T>& 
map<ordinal,T>::operator=(const map& o)
{
    my_last_cursor = o.my_last_cursor;
    init_my_current();
    return *this;
}

template <typename ordinal,typename T>
void 
add_at_end(map_iterator<ordinal,T>& the_list,const ordinal& o,const T& t)
{
    map_iterator<ordinal,T> tmp_cursor = the_list.most();
    tmp_cursor.add_after(o,t);
}

template <typename ordinal,typename T>
void map<ordinal,T>::init_my_current()
{
    // If we have never used the internal iterator before, my_current
    // would be 0.  Call least() in this case since the side-effect will
    // initialize my_current and this function will work correctly.
    if (my_last_cursor.my_current == 0)
        my_last_cursor = least();

}

template <class ordinal,class T>
void 
map<ordinal,T>::add_pair(const ordinal& the_key,const T& the_item)
{
    init_my_current();

    // Frequently the next added value will be immediately after or
    // in place of the previous item added.  Try that case to make going
    // through the list a little faster.   First find the greatest value
    // that is <= the key.
    map_pred tmp_pred(the_key,lteq);
    findit_backwards(my_last_cursor,tmp_pred);
    if (my_last_cursor.is_end() == true)
    {
	    my_last_cursor.least();
    }

    // Then find the first one that's >= the key
    map_equality_pred<ordinal,T> tmp_pred1(the_key,gteq);
    findit_forwards(my_last_cursor,tmp_pred1);

    // If we've found it, add it.
    if (my_last_cursor)
    {
        kv_pair<ordinal,T>& tmp_pair = (*my_last_cursor);
        if (tmp_pair.key == the_key)
        {
    	    tmp_pair.value = the_item;
            return;
        }
        else
        {
            my_last_cursor.add_before(the_key,the_item);
            return;
        }
    }

    // last (or only) item in the list goes here.
    add_at_end(my_last_cursor,the_key,the_item);
	my_last_cursor = most();
}

template <class ordinal,class T>
map_iterator<ordinal,T>
map<ordinal,T>::get_item(const ordinal& the_key,
						 map_match_type the_match)
{
    init_my_current();
    map_iterator<ordinal,T> tmp_cursor = 
        map<ordinal,T>::findit(the_key,the_match,my_last_cursor);
    
    return tmp_cursor;
}

// METHOD: least
// DESCRIPTION:
// get the item that maps to lowest ordinal in the map.
template <class ordinal,class T>
map_iterator<ordinal,T>
map<ordinal,T>::least()
{
    map_iterator<ordinal,T> rv;
    rv = my_last_cursor;
    rv.reset_to_front();
    return rv;
}

// METHOD: most
// DESCRIPTION:
// Get the item that maps to the greatest ordinal in the map.
template <class ordinal,class T>
map_iterator<ordinal,T>
map<ordinal,T>::most()
{
    map_iterator<ordinal,T> rv;
    rv = my_last_cursor;
    rv.reset_to_back();
    return rv;
}

template <class ordinal,class T>
void  
map<ordinal,T>::remove(const ordinal& the_key,map_match_type the_match)
{
    init_my_current();

    map_iterator<ordinal,T> tmp_cursor = 
        map<ordinal,T>::findit(the_key,the_match,my_last_cursor);
    if (tmp_cursor)
    {
        // Advance the last counter so we're not sitting on
        // a deleted node.
		my_last_cursor.delete_current();
    }
}

template <class ordinal,class T>
void  
map<ordinal,T>::clear()
{
    // Put my_last_cursor on an empty node so it
    // won't be pointing nowhere.
    my_last_cursor.least();
    while (my_last_cursor)
    {
        my_last_cursor.delete_current();
    }
}

template <class ordinal,class T>
map<ordinal,T>::~map()
{
}

template <class ordinal,class T>
map_iterator<ordinal,T> 
map<ordinal,T>::findit(const ordinal& the_key,map_match_type the_match,
                       map_iterator<ordinal,T>& the_last_cursor)
{
    map_equality_pred<ordinal,T> tmp_pred(the_key,the_match);
    map_iterator<ordinal,T> tmp_cursor;

    // Since we store items least-to-greatest, if we want a lt
    // value, we assume the user wants to go from back to front
    // search order.  That way you get the greatest one that is
    // less than the key.
    if ((the_match == lteq) || (the_match == lt))
    {
        map_match_type tmp_finder = gteq;
        map_pred tmp_pred1(the_key,tmp_finder);
        findit_forwards(the_last_cursor,tmp_pred1);
        if (the_last_cursor.is_end() == true)
        {
            the_last_cursor = the_last_cursor.most();
        }
        tmp_cursor = findit_backwards(the_last_cursor,tmp_pred);
    }
    else
    {
        map_match_type tmp_finder = lteq;
        map_pred tmp_pred1(the_key,tmp_finder);
        findit_backwards(the_last_cursor,tmp_pred1);
        if (the_last_cursor.is_end() == true)
        {
            the_last_cursor = the_last_cursor.least();
        }
        tmp_cursor = findit_forwards(the_last_cursor,tmp_pred);
    }

    // leave the last cursor pointing to something
    if (the_last_cursor.is_end() == true)
    {
        the_last_cursor = the_last_cursor.least();
    }
    return tmp_cursor;
}

template <class T1,class T2>
map<T1,T2> get_inverse_map(map<T2,T1>& the_map)
{
    typename map<T2,T1>::iterator tmp_it = the_map.least();
    map<T1,T2> tmp_target;
    while (tmp_it)
    {
        tmp_target.add_pair((*tmp_it).value,(*tmp_it).key);
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }

    return tmp_target;
}

template <typename ordinal,typename map_type,typename T>
extern T& get_map_value(const ordinal& o,map_type& the_map)
{
    typename map_type::iterator tmp_it = the_map.get_item(o);
    if (!tmp_it)
    {
        throw("Missing key  in get_value");
    }
    return (*tmp_it).value;
}

template <typename ordinal,typename functor,typename T>
extern void do_to_all(map<ordinal,T>& the_map,functor& the_functor)
{
    typename map<ordinal,T>::iterator tmp_it = 
        the_map.least();
    while (tmp_it)
    {
        the_functor((*tmp_it).value);
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }
}

}
