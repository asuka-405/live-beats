/*
 * array.h - An STL-like array template class.
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
#ifndef IABC_ARRAY_H
#define IABC_ARRAY_H
namespace iabc
{
;
// FILE: array.h
// SUBSYSTEM: containers
// DESCRIPTION:
// Classes to handle arrays of arbitrary types.
template <class T>
class array;

// CLASS: array_data
// DESCRIPTION:
// The array class uses the smart pointer pattern.  
// The memory allocated for the array is handled here.  Clients
// of the array classes should use array<> and not array_data.
template <class T>
class array_data
{
public:
    friend class array<T>;
    array_data(int the_initial_size = 0,int the_grow_size = 1);
	~array_data();
    void expand_to(int the_index);
    void expand(int num_elems);
    int get_size() const{return my_size;};
    T& operator[](int the_index){return my_array[the_index];};
    T operator[] (int the_index)const{return my_array[the_index];};
    operator T*(){return my_array;};
    void add_ref(){++my_count;};
    void remove_ref(){--my_count;if (my_count == 0) delete this;};
    void remove_all();
    // static void* operator new(size_t the_size);
    // static void operator delete(void* the_memory);
protected:
    int my_count;    
    static int my_instances;
    T* my_array;
    int my_limit;
    int my_size;
    int my_grow_size;
};

// CLASS: array
// DESCRIPTION:
// A parameterized array class that allows effecient assignement and
// shallow copying.
template <class T>
class array
{
public:
    // METHOD: ctor
    // DESCRIPTION:
    // Create the array.
    // Args are optional.  No argmunts creates an empty array.
    // int the_initial_size, the number of (un-initialized) elements in the array
    // at creation time.  the_grow_size is the number of elements that we add when we
    // expand the array.
    array(int the_initial_size = 0,int the_grow_size = 1):
        my_array(new array_data<T>(the_initial_size,the_grow_size)){(*my_array).add_ref();};

    // METHOD: dtor
    // DESCRIPTION:
    // dereference the underlying data, when all references have been removed the data
    // will be deleted.
    ~array() {if (my_array) my_array->remove_ref();};

    // METHOD: expand_to
    // DESCRIPTION:
    // Create un-initialized elements until the array is the_index+1 elements.
    void expand_to(int the_index){my_array->expand_to(the_index);};

    // METHOD: expand
    // DESCRIPTION:
    // create num_elems additional un-initialzied elements in the array.
    void expand(int num_elems){my_array->expand(num_elems);};
    
    // METHOD: get_size
    // DESCRIPTION:
    // return the number of elements (get_number_elements would have been a better name)
    int get_size() const {return my_array->get_size();};

    // METHOD: T& operator[](int the_index){return (*my_array)[the_index];}
    // DESCRIPTION:
    // return the element at the_index'th position.
    T& operator[](int the_index){return (*my_array)[the_index];};
    const T& operator[](int the_index) const {return (*my_array)[the_index];};
    
    // METHOD: ctor (copy)
    // DESCRIPTION:
    // dereference my data and reference the data of the_other.
    array(const array& the_other);

    // METHOD: remove_all
    // DESCRIPTION:
    // remove all the elements and set the size of the array to 0
    void remove_all(){my_array->remove_all();};

    // METHOD: remove_from_end
    // DESCRIPTION:
    // Remove the last element of the array, effectively shrinking the array size by one.
    void remove_from_end(){my_array->my_size--;};
    array<T>& operator=(const array<T>& the_other);
    void add_to_end(const T& the_elem);
    operator T*();
    operator const T*()const;
private:
    void set_my_array(const array<T>& other)
    {
        my_array = other.my_array;
        my_array->add_ref();
    }
    array_data<T>* my_array;
};

// METHOD: deep_copy
// DESCRIPTION:
// copy all elements of the array from the_intput to the_output.
// Assumption is that the underlying arrays are not the same.
template <class T>
extern 
void deep_copy(const array<T>& the_input,array<T>& the_output);

// METHOD: do_to_all
// DESCRIPTION:
// Invoke the_predicate(the_list[i]) for each element in the array.
template <class type_t,class functor>
extern void
do_to_all(array<type_t>& the_list,functor& the_predicate);

template <class pointer_t>
class delete_functor
{
public:
    void operator()(pointer_t p){delete p;};
};

template <class addend1,class addend2>
class add_functor
{
public:
    add_functor(const addend1& the_addend1):my_num(the_addend1){};
    addend1 my_num;
    void operator()(addend2& tmp_addend2){tmp_addend2 += my_num;};
};
#if 0
template <class m1,class m2>
class multiply_functor
{
public:
    multiply_functor(const m1& the_m1):
        my_m1(the_m1){}
    void operator()(m2& the_point)
    {
        m2 *= my_m1;
    };
    m1 my_m1;
};
#endif

// METHOD: add_to_end
// Add the element to the end of the array.
template <class type_t>
extern void 
add_to_end(type_t& the_elem);
}
#endif



