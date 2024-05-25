/*
 * string.cpp - Yet another not-quite-ubiquitous smart character array.
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
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "iabc/string.h"
#include "iabc/array.cpp"
#include "iabc/factory.cpp"

#if debug_strings
#include "iabc/list.cpp"
#endif

namespace iabc {
using namespace iabc;

instance_counter<string> string::the_number_instances;

#if debug_strings
#include "iabc/list.cpp"
list<string*>&
all_strings()
{
	static list<string*> the_strings;
	return the_strings;
}
#endif

string::string ()
{  
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    get_chars(1);
    my_rep[0] = '\0';
}

string::string (const char* the_other):my_rep(0)
{
  
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    if ((the_other) &&
        (the_other[0] != 0))
        {
        get_chars(strlen(the_other) + 1);
        strcpy(&((my_rep)[0]),the_other);
        }
    else
        {
        get_chars(1);
        my_rep[0] = '\0';
        }
}

string::string (const string& the_other)
{
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    if (the_other.length())
        {
        get_chars(the_other.length() + 1);
        strcpy(my_rep,the_other.my_rep);
        }
    else
        {
        get_chars(1);
        my_rep[0] = '\0';
        }
}

string::string (long the_num)
{
  
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    
    // Get the length
    int tmp_length = 1;
    long tmp_num = the_num;
    tmp_num = tmp_num/10;
    while (tmp_num)
        {
        ++tmp_length;
	     tmp_num = tmp_num/10;
        }

    int the_minus = (the_num < 0) ? 1 : 0;
    get_chars(tmp_length + 1 + the_minus);  //Extra spot for -
    sprintf(&((my_rep)[0]),"%ld",the_num);  
}

string::string (unsigned long the_ul)
{
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif

    // Get the length
    int tmp_length = 1;
    unsigned long tmp_num = the_ul;
    tmp_num = tmp_num/10;
    while (tmp_num)
        {
        ++tmp_length;
    	  tmp_num = tmp_num/10;
        }

    get_chars(tmp_length+1);
    sprintf(&((my_rep)[0]),"%lu",the_ul);
}

string::string (double the_float)
{
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    string tmp_rv;
    // Get the length of the mantissa part
    int tmp_length = 1;
    int tmp_mantissa = 1;
    double the_num = fabs(the_float);
    while (the_num >= 1.0)
        {
        ++tmp_length;
        tmp_mantissa *= 10;
        the_num = the_num/10.0;
        }

    // Add the string of the mantissa part
    the_num *= tmp_mantissa;
    tmp_mantissa /= 10;
    while (tmp_mantissa >= 10)
    {
        int tmp_int_num = (int)the_num;
        int tmp_mod = tmp_int_num % tmp_mantissa;
        tmp_rv = string((char)('0' + char(tmp_mod))) + tmp_rv;
        the_num /= 10.0;
        tmp_mantissa /= 10;
    }
    
    tmp_rv = string((long)the_num) + tmp_rv;
    tmp_rv += string('.');
    the_num = (double)(long)fabs(the_float);
    double tmpd = fabs(the_float) - (double)the_num;

    int tmp_fraction = 0;
    if (tmpd <= 0.0)
    {
        tmp_rv += string('0');
    }
    while ((tmpd > 0.0) && (tmp_fraction < string::precision))
        {
        ++tmp_length;
        ++tmp_fraction;
        tmpd = tmpd * 10;
        tmp_rv += string(long(tmpd));
        the_num = (double)(long)fabs(tmpd);
        tmpd = (double)(float)tmpd - (double)the_num;
        }

    // This is hideously inefficient but we don't do this very often.
    if (the_float < 0.0)
    {
        tmp_rv = string('-') + tmp_rv;
    }
    (*this) = tmp_rv;
}

string::string (char the_char)
{
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    get_chars(2);
    (my_rep)[0] = the_char;
    (my_rep)[1] = '\0';
}

string::string (const char* the_buf, long the_size)
{
    ++the_number_instances;
#if debug_strings
    add_at_beginning(all_strings(),this);
#endif
    get_chars(the_size + 1);
    memcpy(&((my_rep)[0]),the_buf,the_size);
    (my_rep)[(int)the_size] = '\0';
}


string::~string ()
{
#if debug_strings
    list<string*>::iterator tmp_it = 
        findit(all_strings(),this);
    tmp_it.delete_current();
#endif
    --the_number_instances;
    release_chars();
}

void
string::chop()
{
    if (my_size >= 2)
    {
        my_rep[my_size - 2] = '\0';
        my_size --;
    }
}

string&
string::operator = (const string& the_other)
{
    if (the_other.my_rep == my_rep)
    {
        return *this;
    }

    if (my_rep)
    {
        release_chars();
    }
    get_chars(the_other.length() + 1);
	char* my_p = (char*)my_rep;
	const char* o_p = (const char*)(the_other.my_rep);
    strcpy(my_p,o_p);
	return *this;
}

string&
string::assign(const char* the_other,int the_length)
{
    release_chars();
    get_chars(the_length + 1);
    strncpy(&(my_rep[0]),the_other,the_length);
    (my_rep)[the_length] = (char)0;
    return *this;
}

string& string::operator = (const char* the_other)
{
    release_chars();
    get_chars(strlen(the_other) + 1);
    strcpy(&((my_rep)[0]),the_other);
	return *this;
}

string& string::operator = (const long the_other)
{
    string tmp_s = the_other;
    *this = tmp_s;
    return *this;
}

string& string::operator = (const double the_other)
{
    string tmp_s = the_other;
    *this = tmp_s;
    return *this;
}

const char& string::operator [] (int the_num) const
{
    if (my_size >= the_num)
        {
        return(my_rep[the_num]);
        }
#ifndef GCC
    throw ("Invalid pointer in string.h");
#endif
}

char& string::operator [] (int the_num)
{
    if (my_size >= the_num)
    {
        return (my_rep)[the_num];
    }
    throw ("Invalid pointer in string.h");
}

string string::mid (int the_start, int the_length) const
{
    string tmps;
	if (the_length < 0)
		throw("Illegal string initializer");
    if (length() >= the_length+the_start)
        {
        tmps.release_chars();

        tmps.get_chars(the_length + 1);
        int i=the_start;
        for (i=0;i<the_length;++i)
            {
            (tmps.my_rep)[i] = (my_rep)[the_start + i];
            }

        (tmps.my_rep)[i] = '\0';
        }

    return tmps;
}

void string::reverse ()
{
  
    int i = length();
    int len = i;
    string tmp_r(*this);
    for (;i>0;--i)
        {
        tmp_r[i-1] = (my_rep)[len - i];
        }
    tmp_r[len] = '\0';

    *this = tmp_r;  
}

bool string::operator == (const string& the_other) const
{
    bool tmp_rv = false;
    if (my_rep && the_other.my_rep)
        {
        tmp_rv = ((strcmp(my_rep,(the_other.my_rep))==0) ? true : false);
        }
    return tmp_rv;
}

bool 
string::operator != (const string& the_other) const
{
    return (*this == the_other) ? false : true;
}

string 
string::operator + (string the_other) const
{
   string tmp_s;
   
   if ((my_rep == 0) || (the_other.my_rep == 0))
        {
        return tmp_s;
        }
   tmp_s.release_chars();
   tmp_s.get_chars(length() + the_other.length() + 1);
   strcpy((tmp_s.my_rep),my_rep);
   strcat((tmp_s.my_rep),(the_other.my_rep));
   return tmp_s;
}

string& 
string::operator += (const string& the_other)
{
	if (&the_other != this)
	{
        int tmp_size = my_size + the_other.length();
        // void_buf_pool::get_buffer(tmp_size);
        // char* tmp_rep = (char*) new char[tmp_size]; 
		array<char> tmp_rep;
		tmp_rep.expand_to(tmp_size);
        // char* tmp_rep = (char*) void_buf_pool::get_buffer(tmp_size);
		strcpy((char*)tmp_rep,(char*)my_rep);
		strcat((char*)tmp_rep,(const char*)the_other.my_rep);
        release_chars();
        my_size = tmp_size;
		my_rep = tmp_rep;
	}
    return *this;
}

bool 
string::operator > (const string& the_other) const
{
    if (*this == the_other)
        {
        return false;
        }
    else if ((length() > 0 ) &&
             (the_other.length() == 0))
    {
        return true;
    }
    else if ((the_other.length() > 0 ) &&
             (length() == 0))
    {
        return false;
    }
    int tmp_my_len = length();
    int tmp_other_len = the_other.length();
    bool tmp_rv = false;
    int floor;
    if (tmp_other_len <= tmp_my_len)
        {
        floor = tmp_other_len;
        }
    else
        {
        floor = tmp_my_len;
        }

    for (int index = 0;index < floor;++index)
        {
        if ((my_rep)[index] > the_other[index])
            {
            tmp_rv = true;
            break;
            }
        else if ((my_rep)[index] < the_other[index])
            {
            tmp_rv = false;
            break;
            }
        }

    return tmp_rv;
}

bool 
string::operator < (const string& the_other) const
{
    bool tmp_rv = (the_other > *this);

    return tmp_rv;
}

long 
string::as_long () const
{
  
    long tmp_rv = 0;
    if ((length() > 2) && ((my_rep)[0] == '0') && ((my_rep)[1] == 'x'))
        {
        sscanf(my_rep,"%x",&tmp_rv);
        }
    else if (my_rep)
        {
        sscanf(my_rep,"%ld",&tmp_rv);
        }
    return tmp_rv;
  
}

bool string::as_bool () const
{
    bool tmp_rv = false;
    string o = (*this);
    o.to_lower();
    if ((o == "true") ||
        (o == "1"))
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

double 
string::as_double () const
{
  
    double tmp_rv = 0.0;
    if ((my_rep))
        {
        sscanf(my_rep,"%lf",&tmp_rv);
        }
    return tmp_rv;
  
}

array<string>
string::split(const string& the_pattern) const
{
    array<string> tmp_rv;
    string tmp_first;
    int i;
    int tmp_index = 0;
    for (i = 0;i < length();++i,++tmp_index)
    {
        string tmp_possible = mid(tmp_index,the_pattern.length());
        if (tmp_possible == the_pattern)
        {
            tmp_index += the_pattern.length() - 1;
            tmp_rv.add_to_end(tmp_first);
            tmp_first = "";
        }
        else
        {
            tmp_first += (*this)[tmp_index];
        }
    }

    return tmp_rv;
}

int 
string::length () const
{
    int tmp_rv = 0;
    if (my_size)
    {
        return my_size - 1;
    }

    return tmp_rv;
}

string string::to_hex (unsigned long the_num)
{  
   const char* const tmp_hex_string="0123456789ABCDEF";

   // Note:  This assumes an 8-bit byte
   int i=(sizeof(unsigned long)-1) * 4 //  bits in a nibble
		*2 // nibbles in a byte
		+4 // bits in a nibble
		;
   unsigned long tmp_mask = 0x0f << i;
   string tmps = "0x";
   for (;i>=0;i-=4)
      {
      char tmpc = tmp_hex_string[(the_num & tmp_mask) >> i];
      string tmpcs = tmpc;
      string tmps1 = tmps;
      tmps = tmps1 + tmpcs;
      tmp_mask = tmp_mask >> 4;
      }

   return tmps;
}

string& string::to_lower ()
{
  
    int i;
    for (i=0;i<(int)strlen(my_rep);++i)
        {
        my_rep[i] = (char)tolower((my_rep)[i]);
        }
    return *this; 
}

string& string::to_upper ()
{
  
    int i;
    for (i=0;i<(int)strlen(my_rep);++i)
        {
        (my_rep)[i] = (char)toupper((my_rep)[i]);
        }
    return *this;
  
}

char* string::allocate_char_array () const
{
  
    char* tmp_rv = new char[length() + 1];
    strcpy(tmp_rv,my_rep);
    return tmp_rv;
}

void
string::clear()
{
    release_chars();
    get_chars(1);
    (my_rep)[0] = (char)0;
}

void string::strip_white_space ()
{  
}

void
string::get_chars(size_t the_chars)
{
    my_size = the_chars;
	my_rep.expand_to(the_chars);
}
void 
string::release_chars()
{
    if (my_rep)
    {
		my_rep.remove_all();
        my_size = 0;
    }
}

  
}
  



