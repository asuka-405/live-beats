/*
 * factory.h - Templates the define class factory
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
#ifndef iabc_factory_h
#define iabc_factory_h
namespace iabc {
;
template <class type1> 
class instance_counter
{
public:
    int operator++(void);
    int operator++(int ignore);
    int operator--(void);
    int operator--(int ignore);
    int get_number_instances();
    static void clock_in();
    static void clock_out();
// private:
    static int class_instance_count;
    static int clock_in_count;
};

// CLASS: factory
// DESCRIPTION:
// Define a template for a function that returns an instance of a class
// where the actual create function is defined at link time.
template <class type1,class p1_type> 
class factory
{
public:
    static type1* create();
    static type1* create(p1_type p1);
};

// CLASS: globalton
// DESCRIPTION:
// Define a static instance that is guaranteed to be created at the
// right time if it is referenced before 'main'.  Good for class factories.
template <class type1>
class globalton
{
public:
    globalton():my_ref(type1::get_instance()){};
    ~globalton(){};
    static globalton the_instance;
    type1& my_ref;
};
}

#endif

