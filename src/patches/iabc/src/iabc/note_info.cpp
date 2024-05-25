/*
 * note_info.cpp - A media-independent representation of a note.
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
#include "iabc/note_info.h"
#include "iabc/array.cpp"

namespace iabc
{

;

int 
chord_info::static_chord_count = 0;

fraction 
chord_info::the_longest_beamed_thing(15,64);

string
note_info::as_string() const
{
    string tmp_this;
    if (this->is_rest() == true)
    {
        tmp_this += (string)"z";
    }
    else
    {
        tmp_this += my_pitch.as_string();
    }
    tmp_this += (long)my_duration.num;
    tmp_this += (string)'/';
    tmp_this += (long)my_duration.den;

    return tmp_this;
}

chord_info::chord_info(const pitch& the_pitch,const fraction& the_duration,
					   bool the_start_slur,bool the_end_slur,bool the_first_in_beam):
    my_embellishment(chord_info::none),
    my_first_in_beam(the_first_in_beam),
    my_is_grace(false),
    my_last_in_beam(true),
    my_xlet_value(0),
    my_dynamic(mf)
{
    ++static_chord_count;
    note_info tmp_info(the_pitch,the_duration);
    my_notes.add_to_end(tmp_info);
}

chord_info::chord_info():
    my_embellishment(chord_info::none),
    my_first_in_beam(true),
    my_is_grace(false),
    my_last_in_beam(true),
    my_xlet_value(0),
    my_dynamic(mf)
{
    ++static_chord_count;
}

chord_info::~chord_info()
{
    --static_chord_count;
}

void 
chord_info::set_words(const string& the_words)
{
    my_words.add_to_end(the_words);
}

void 
chord_info::set_words(array<string>& the_words)
{
    my_words = the_words;
}

void 
chord_info::set_dynamic(chord_info::dynamic the_dynamic)
{
    my_dynamic = the_dynamic;
}

chord_info::dynamic 
chord_info::get_dynamic() const
{
    return my_dynamic;
}

array<string> 
chord_info::get_words() const
{
    return my_words;
}

bool
chord_info::is_beamable()
{
    return (get_duration() < the_longest_beamed_thing);
}

fraction 
chord_info::get_duration() const
{
    fraction tmp_rv;
    for (int i = 0;i < my_notes.get_size();++ i)
    {
        if ((my_notes[i].get_duration() < tmp_rv) ||
            (i == 0))
        {
            tmp_rv = my_notes[i].get_duration();
        }
    }

    return tmp_rv;
}

note_info 
chord_info::get_high_note() const
{
    int i;
    note_info tmp_rv = my_notes[0];
    for (i = 1;i < my_notes.get_size();++i)
    {
        if (my_notes[i].get_pitch() > tmp_rv.get_pitch())
        {
            tmp_rv = my_notes[i];
        }
    }
    return tmp_rv;
}

note_info 
chord_info::get_low_note() const
{
    int i;
    note_info tmp_rv = my_notes[0];
    for (i = 1;i < my_notes.get_size();++i)
    {
        if (my_notes[i].get_pitch() < tmp_rv.get_pitch())
        {
            tmp_rv = my_notes[i];
        }
    }

    return tmp_rv;
}

int 
chord_info::get_size() const
{
    return my_notes.get_size();
}

string
chord_info::as_string() const
{
    string tmp_s;
    if (my_xlet_value > 0)
    {
        tmp_s += (string) "(" + (string)(long)my_xlet_value;
    }
    if (my_notes.get_size() > 1)
    {
        tmp_s += "[";    
    }
    for (int i = 0;i < my_notes.get_size(); ++i)
    {
        tmp_s += my_notes[i].as_string();
    }
    if (my_notes.get_size() > 1)
    {
        tmp_s += "]";    
    }

	if (my_words.get_size() > 0)
		tmp_s += (string)"\"" + my_words[0] + (string)"\"";

    return tmp_s;
}

note_info 
chord_info::operator[](int the_index) const  
{
    if (my_notes.get_size() >= the_index + 1)
    {
        return my_notes[the_index];
    }
    return note_info();
}

note_info& 
chord_info::operator[](int the_index)  
{
    if (my_notes.get_size() >= the_index + 1)
    {
        return (my_notes[the_index]);
    }

    throw("Illegal index into chord_info array\n");
}

void
chord_info::set_equal(const chord_info& o)
{
    my_embellishment = o.my_embellishment;
    my_description = o.my_description;
    my_notes = o.my_notes;
    my_first_in_beam = o.my_first_in_beam;
    my_is_grace = o.my_is_grace;
    my_last_in_beam = o.my_last_in_beam;
    my_xlet_value = o.my_xlet_value;
    my_words = o.my_words;
    my_dynamic = o.my_dynamic;
}

bool 
chord_info::starts_slur() const
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        if (my_notes[i].starts_slur() == true)
        {
            return true;
        }
    }

    return false;
}

void 
chord_info::start_slur(bool the_value)
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        my_notes[i].start_slur(the_value);
    }
}

bool 
chord_info::ends_slur() const
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        if (my_notes[i].ends_slur() == true)
        {
            return true;
        }
    }

    return false;
}

void 
chord_info::end_slur(bool the_value)
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        my_notes[i].end_slur(the_value);
    }
}
    
bool 
chord_info::starts_tie() const
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        if (my_notes[i].starts_tie() == true)
        {
            return true;
        }
    }

    return false;
}

void 
chord_info::start_tie(bool the_value)
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        my_notes[i].start_tie(the_value);
    }
}

bool 
chord_info::ends_tie() const
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        if (my_notes[i].ends_tie() == true)
        {
            return true;
        }
    }

    return false;
}

void 
chord_info::end_tie(bool the_value)
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        my_notes[i].end_tie(the_value);
    }
}
    
chord_info::chord_info(const chord_info& o)
{
    ++static_chord_count;
    set_equal(o);
}

chord_info 
chord_info::clone()
{
    chord_info o;
    o.my_description = my_description;
    o.my_dynamic = my_dynamic;
    o.my_embellishment = my_embellishment;
    o.my_first_in_beam = my_first_in_beam;
    o.my_is_grace = my_is_grace;
    o.my_last_in_beam = my_last_in_beam;
    o.my_xlet_value = my_xlet_value;
    deep_copy(my_words,o.my_words);
    deep_copy(my_notes,o.my_notes);

    return o;
}

chord_info& 
chord_info::operator=(const chord_info& o)
{
    set_equal(o);
	return *this;
}

void 
chord_info::change_length(const fraction& product,
                   const fraction& sum)
{
    for (int i = 0;i < my_notes.get_size();++i)
    {
        my_notes[i].change_length(product,sum);

    }
}

bool
chord_info::is_first_in_beam() const
{
    return my_first_in_beam;
}

bool
chord_info::is_last_in_beam() const
{
    return my_last_in_beam;
}

chord_info&
chord_info::operator+=(const note_info& o)
{
    my_notes.add_to_end(o);
    return *this;
}

string 
chord_info::get_dynamic_string() const
{
    return chord_info::get_dynamic_string(my_dynamic);
}

string
chord_info::get_dynamic_string(chord_info::dynamic the_dynamic)
{
    string tmp_rv;
    if (the_dynamic == chord_info::pp)
    {
        tmp_rv = "pp";
    }
    else if (the_dynamic == chord_info::p)
    {
        tmp_rv = "p";
    }
    else if (the_dynamic == chord_info::mp)
    {
        tmp_rv = "mp";
    }
    else if (the_dynamic == chord_info::mf)
    {
        tmp_rv = "mf";
    }
    else if (the_dynamic == chord_info::f)
    {
        tmp_rv = "f";
    }
    else if (the_dynamic == chord_info::ff)
    {
        tmp_rv = "ff";
    }
    else if (the_dynamic == chord_info::fff)
    {
        tmp_rv = "fff";
    }
    else if ((the_dynamic == chord_info::start_cresc) ||
             (the_dynamic == chord_info::end_cresc))
    {
        tmp_rv = "crescendo";
    }
    else if ((the_dynamic == chord_info::start_decresc) ||
             (the_dynamic == chord_info::end_decresc))
    {
        tmp_rv = "diminuendo";
    }

    return tmp_rv;
}


}
