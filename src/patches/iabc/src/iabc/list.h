/*
 * list.h - An STL-like list and related container classes.
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
#ifndef iabc_list_h
#define iabc_list_h 1


namespace iabc
{
;
// compensate for bug in MS debigger, does not allow symbols > 255
#ifdef _DEBUG
#define list_iterator lc_
#define container cn_
#define findit_predicate fip_
#endif

  // This is just for debugging, GCC instantiates things
  // differently.
#ifndef GCC
  extern int max_node_instances;
  extern int max_list_instances;
#endif

// FILE: list.h
// DESCRIPTION:
// This is a bidirectional list with the default node
// serving as the head of the list.  The 'data' part of the
// default node is not valid and is never refereneced. The
// default node always exists (even in an empty list) and is used
// in comparisons to end-of-list
//
// Unlike some lists you may have known, this one doesn't really have any
// usable methods.  All interaction with the list is done through a list
// iterator.  Lists can be copied by value.  This is a shallow copy using
// reference counting
template <class T> class list;
template <class T> class list_iterator;

// CLASS: list_node
// DESCRIPTION:
// List node is the data plus the pointers
// of a linked list.
template <class T>
class list_node
{
public:
  list_node();
  list_node(const T& o){my_data = o;};
  ~list_node();
  list_node* my_next;
  list_node* my_last;
  T my_data;
  static int my_instances;
};

// CLASS: list_data
// DESCRIPTION:
// list_data does all the work of the container interface for lists.
template <class T>
class list_data
{
  public:
      ~list_data ();
      int get_size() const{return my_size;};
      void add_ref() {++my_ref;};
      void remove_ref() {if (--my_ref <= 0) delete this;};
      bool is_end(list_node<T>& the_node) const {return (my_head == &the_node);};
      list_node<T>* get_head ();
protected:
  private:
      list_data ();
      void insert_after (list_node<T>* the_old, list_node<T>* the_new);
      void insert_before (list_node<T>* the_old, list_node<T>* the_new);
      void remove (list_node<T>* the_node);
      list_data(const list_data< T > &right);
      list_data< T > & operator=(const list_data< T > &right);
      list_node<T>* my_head;
      int my_size;
      int my_ref;
      friend class list<T>;
      friend class list_iterator<T>;
      static int my_instances;
};

// CLASS: list_iterator
// DESCRIPTION:
// This iterates through a list, and also is used to add or remove
// elements (which in STL is a list function).  This is always referenced
// as a nested iterator within list (e.g. list<T>::iterator = my_list.first)
template <class T>
class list_iterator
{
protected:
private:
    friend class list<T>;
    T& get_current() {return my_current->my_data;};
    T get_current() const {return my_current->my_data;};
    list_node<T>* my_current;
    list_data<T>* my_list;
public:
    list_iterator();
    ~list_iterator ();
    list_iterator (const list_iterator& the_other);
    void add_after (const T& refOther);
    void add_before (const T& refOther);
    void delete_current ();
    void reset_to_front();
    void reset_to_back();
    long get_size() const;
    bool is_end() const;
    // METHOD: operator*
    // DESCRIPTION:
    // Acts like a pointer dereference.  Throws excepction if the list is
    // empty.  Returns a reference.
    T& operator* ();
    // METHOD: operator* const
    // DESCRIPTION:
    // Acts like a pointer dereference.  Throws excepction if the list is
    // empty.  Returns a copy.
    T operator*  () const;

    // METHOD: T*
    // DESCRIPTION:
    // Allow the list to be checked for validity in the pointer way, 
    // e.g.:
    // if (my_iterator) {       // check for end of list.
    // tmp_data = *my_iterator; // now this is safe
    operator T* ();
    operator const T*() const;

    void next();
    void previous();

    // METHOD: operator=
    // DESCRIPTION:
    // shallow, reference counted copy.
    list_iterator& operator=(const list_iterator& o);
};

// CLASS: list
// DESCRIPTION:
// list is an extra layer of indirection so that we can
// pass const lists and still access the methods needed to 
// perform const-type operations (like copying a list)
// It also allows us to assign the lists by value and lets
// the reference counting for list_data work transparently.
template <class T>
class list
{
public:
    typedef list_iterator<T> iterator;
    friend class list_iterator<T>;
    iterator first();
    iterator last();
    list(){my_data = new list_data<T>;my_data->my_ref++;};
    ~list(){if (--(my_data->my_ref) == 0) delete my_data;};
    list& operator=(const list<T>& o);
    list(const list& o);
    int get_size() const {return my_data->get_size();};
private:
    void insert_after (list_node<T>* the_old, list_node<T>* the_new)
        {my_data->insert_after(the_old,the_new);};
    void insert_before (list_node<T>* the_old, list_node<T>* the_new)
        {my_data->insert_before(the_old,the_new);};
    // void remove (list_node<T>* the_node);
    bool operator==(const list<T>& o) const{return (my_data == o.my_data);};
    bool operator!=(const list<T>& o) const{return ((*this) == o);};
private:
    list_data<T>* my_data;
};

// FUNCTION: gt_predicate
// DESCRIPTION:
// these are a bunch of predicates to find special elements of a 
// list using find_predicate function.
template <class T>
class gt_predicate
{
public:
    gt_predicate(const T& theT):myT(theT){};
    bool operator() (const T& theOther) const 
    {
        if (theOther > myT)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
private:
    T myT;
};

template <class T>
class gteq_predicate
{
public:
    gteq_predicate(const T& theT):myT(theT){};
    bool operator()(const T& theOther)
    {
        if (theOther >= myT)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
private:
    T myT;
};

template <class T>
class eq_predicate
{
public:
    eq_predicate(const T& theT):myT(theT){};
    bool operator()(const T& theOther)
    {
        if (theOther == myT)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
private:
    T myT;
};

template <class T>
class lt_predicate
{
public:
    lt_predicate(const T& theT):myT(theT){};
    bool operator()(const T& theOther)
    {
        if (theOther < myT)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
private:
    T myT;
};

template <class T>
class lteq_predicate
{
public:
    lteq_predicate(const T& theT):myT(theT){};
    bool operator()(const T& theOther)
    {
        if (theOther <= myT)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
private:
    T myT;
};

template <class list_t>
extern
void
remove_all(list<list_t>& the_list);

template <class list_t>
extern
void
add_at_beginning(list<list_t>& the_list,const list_t& the_element);

template <class list_t>
extern
void
add_at_end(list<list_t>& the_list,const list_t& the_element);

template <class list_t,class functor>
extern
void
do_to_all(list<list_t>& the_list,functor& the_functor);

template <class type_t,class predicate>
extern
typename list<type_t>::iterator
findit_predicate(list<type_t>& the_list,predicate& the_predicate);

template <class type_t,class predicate>
extern
typename list<type_t>::iterator
findit_forwards(typename list<type_t>::iterator& the_list,predicate& the_predicate);

template <class type_t,class predicate>
extern
typename list<type_t>::iterator
findit_backwards(typename list<type_t>::iterator& the_list,predicate& the_predicate);

template <class type_t>
extern
void
remove_first_ref(list<type_t>& the_list,type_t& element);

template <class type_t>
extern
void
push(list<type_t>& the_list,const type_t& element);

template <class type_t>
extern
type_t
pop(list<type_t>& the_list);

template <class type_t>
extern
type_t
deque(list<type_t>& the_list);

}


#endif



