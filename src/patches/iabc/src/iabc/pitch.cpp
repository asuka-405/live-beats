/*
 * pitch.cpp - Presentation-independent representation of a pitch.
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
#include "iabc/pitch.h"
#include "iabc/map.cpp"
#include "math.h"

namespace iabc
{

template <class Type>
int get_element_index(Type* the_array,const Type& the_element,int the_array_size)
{
    int i = 0;
    while ((i < the_array_size ) && 
           ((the_array[i].get_letter() != the_element.get_letter()) ||
		   (the_array[i].get_accidental() != the_element.get_accidental()))) ++i;
    return ((i < the_array_size) ? i : -1);
}
    
pitch
pitch::chromatic_up[NOTES_PER_OCTAVE] =
{
    pitch(pitch::A,pitch::flat,0),
    pitch(pitch::A,pitch::natural,0),
    pitch(pitch::B,pitch::flat,0),
    pitch(pitch::B,pitch::natural,0),
    pitch(pitch::C,pitch::natural,0),
    pitch(pitch::D,pitch::flat,0),
    pitch(pitch::D,pitch::natural,0),
    pitch(pitch::E,pitch::flat,0),
    pitch(pitch::E,pitch::natural,0),
    pitch(pitch::F,pitch::natural,0),
    pitch(pitch::F,pitch::sharp,0),
    pitch(pitch::G,pitch::natural,0)
};

pitch
pitch::sharps[MAX_SHARPS] =
{
    pitch(pitch::F,pitch::sharp,0),
    pitch(pitch::C,pitch::sharp,0),
    pitch(pitch::G,pitch::sharp,0),
    pitch(pitch::D,pitch::sharp,0),
    pitch(pitch::A,pitch::sharp,0),
    pitch(pitch::E,pitch::sharp,0),
    pitch(pitch::B,pitch::sharp,0)
};

pitch
pitch::sharp_keys[MAX_SHARPS] =
{
    pitch(pitch::G,pitch::natural,0),
    pitch(pitch::D,pitch::natural,0),
    pitch(pitch::A,pitch::natural,0),
    pitch(pitch::E,pitch::natural,0),
    pitch(pitch::B,pitch::natural,0),
    pitch(pitch::F,pitch::sharp,0),
    pitch(pitch::C,pitch::sharp,0)
};

pitch
pitch::flats[MAX_FLATS] =
{
    pitch(pitch::B,pitch::flat,0),
    pitch(pitch::E,pitch::flat,0),
    pitch(pitch::A,pitch::flat,0),
    pitch(pitch::D,pitch::flat,0),
    pitch(pitch::G,pitch::flat,0),
    pitch(pitch::C,pitch::flat,0),
    pitch(pitch::F,pitch::flat,0)
};

pitch
pitch::flat_keys[MAX_FLATS] =
{
    pitch(pitch::F,pitch::natural,0),
    pitch(pitch::B,pitch::flat,0),
    pitch(pitch::E,pitch::flat,0),
    pitch(pitch::A,pitch::flat,0),
    pitch(pitch::D,pitch::flat,0),
    pitch(pitch::G,pitch::flat,0),
    pitch(pitch::C,pitch::flat,0)
};

int
pitch::get_accidental_offset(accidental a) const
{
    // flat = - 1, sharp = + 1, etc.
    int tmp_offset = 0;
    switch (a)
    {
    case sharp:
        tmp_offset = 1;
        break;
    case flat:
        tmp_offset = -1;
        break;
    case double_sharp:
        tmp_offset = 2;
        break;
    case double_flat:
        tmp_offset = -2;
        break;
    case natural:
    default:
            tmp_offset = 0;
            break;
    }

    return tmp_offset;
}

int
pitch::get_chromatic_index() const
{
    // get the offset given by sharp, flat, etc.
    int tmp_offset = get_accidental_offset(my_accidental);
    
    // now find the letter in the chromatic scale
    pitch tmp_this = *this;
    tmp_this.my_octave = 0;
    tmp_this.my_accidental = pitch::natural;

	pitch* tmp_array = (pitch*)chromatic_up;
    int i = get_element_index(tmp_array,tmp_this,12);
    
    // now add the offset to the letter, and you have 
    // the (possibly enharmonic) chromatic note
    int tmp_reali = (i + tmp_offset) % 12;
    return tmp_reali;
}

bool
pitch::operator>(const pitch& o) const 
{ 
    // convert the notes to enharmonics in the table
    pitch tmp_this = normalize();
    pitch tmp_o = o.normalize();

    // just compare the indices into the table (adjusting for octaves)
    return ((my_octave > o.my_octave) ||
    ((my_octave == o.my_octave) && (get_chromatic_index() > o.get_chromatic_index())));
}

bool
pitch::operator<(const pitch& o) const
{
    return !((*this > o) || (*this == o));
}

pitch
pitch::normalize() const
{
    // copy this since this is a const method
    pitch tmp_this(*this);

    // get the offset given by sharp, flat, etc.
    int tmp_offset = get_accidental_offset(my_accidental);

    // get the index into the chromatic table
    int tmp_reali = get_chromatic_index();
    pitch tmp_pitch = chromatic_up[tmp_reali];

    // now restore the octave, compensating for the accidental change
    tmp_pitch.my_octave = my_octave;
    if (tmp_reali + tmp_offset > 11)
    {
        tmp_pitch.my_octave += 1;
    }
    else if (tmp_reali + tmp_offset < 0)
    {
        tmp_pitch.my_octave -= -1;
    }

    return tmp_pitch;
}

bool 
pitch::operator==(const pitch& o) const
{
    // convert the pitches to enharmonics in the table and just
    // compare the indices and octaves
    pitch p1 = normalize();
    pitch p2 = o.normalize();
    return ((p1.my_letter == p2.my_letter) &&
            (p1.my_accidental == p2.my_accidental) &&
            (p1.my_octave == p2.my_octave));
}

bool 
pitch::is_in_key(const pitch& the_key) const
{
    // dispense with key of 'C'
    if ((C == the_key.my_letter) && (natural == the_key.my_accidental))
    {
        return (my_accidental == natural);
    }
    pitch* tmp_key_array;
    pitch* tmp_accidental_array;

    // figure out whether we;re a sharp or a flat key
    accidental tmp_key_type;
    if (((the_key.my_letter == F) && (the_key.my_accidental == natural)) || 
         (the_key.my_accidental == flat))
    {
        tmp_key_type = flat;
        tmp_key_array = flat_keys;
        tmp_accidental_array = flats;
    }
    else
    {
        tmp_key_type = sharp;
        tmp_key_array = sharp_keys;
        tmp_accidental_array = sharps;
    }

    // return false if we are a flat in a sharp key
    // or a sharp in a flat key
    if ((my_accidental != tmp_key_type) &&
        (my_accidental != natural))
    {
        return false;
    }

    // return false if one of the sharps/flats in the key
    // doesn't agree with what we are.
    int tmp_index = get_element_index(tmp_key_array,the_key,MAX_SHARPS);

    int i;
    for (i = 0;i <= tmp_index; ++ i)
    {
        if (tmp_accidental_array[i].my_letter == my_letter)
		{
			if (tmp_accidental_array[i].my_accidental != my_accidental)
				return false;
			else
				return true;
        }
    }

    // else we are in the key if we are natural, or if we aren't then
	// we are an accidental
    return my_accidental == natural;
}


bool 
pitch::should_print_accidental(accidental_map& the_map,
                                    const pitch& the_key)
{
    if ((has_accidental_changed(the_map) == true) ||
        ((is_accidental_in_array(the_map) == false) &&
         (is_in_key(the_key) == false)))
    {
        return true;
    }

    return false;
}

void 
pitch::change_accidental(accidental_map& the_map,
                                    const pitch& the_key)
{
    // Assign the accidental to override if it has been specified.
    if (is_accidental_in_array(the_map) == true)
    {
        my_accidental = get_accidental_from_array(the_map,get_letter());
    }
    // Otherwise transpose until we are in the correct key.
    else
    {
        if (is_in_key(the_key) == false)
        {
            my_accidental = pitch::flat;
        }
        if (is_in_key(the_key) == false)
        {
            my_accidental = pitch::sharp;
        }
    }
}

void 
pitch::add_accidental(accidental_map& the_map)
{
    the_map.add_pair(get_letter(),get_accidental());
}

bool 
pitch::has_accidental_changed(accidental_map& the_map)
{
    accidental_map::iterator tmp_it = 
        the_map.get_item(my_letter);
    if (tmp_it)
    {
        return ((*tmp_it).value != my_accidental);
    }

    return false;
}

bool 
pitch::is_accidental_in_array(accidental_map& the_map)
{
    accidental_map::iterator tmp_it = 
        the_map.get_item(my_letter);
    if (tmp_it)
    {
        return (true);
    }

    return false;
}

// METHOD: get_accidental_from_array
pitch::accidental 
pitch::get_accidental_from_array(accidental_map& the_map,
                                            pitch::letter the_letter)
{
    pitch::accidental tmp_rv = pitch::natural;
    accidental_map::iterator tmp_it = 
        the_map.get_item(the_letter);
    if (tmp_it)
    {
        tmp_rv = (*tmp_it).value;
    }

    return tmp_rv;
}

string
pitch::abc_string(const pitch& the_key) const
{
    string tmp_rv;
    char tmp_accidental = (char)0;
    int i;
    if (is_in_key(the_key) == false)
    {
        switch (my_accidental)
        {
        case flat:
            tmp_accidental = '_';
            break;
        case sharp:
            tmp_accidental = '^';
            break;
        case natural:
        default:
            tmp_accidental = '=';
            break;
        }
    }

    char tmp_letter = (char) my_letter + 'A';
    int tmp_octave = (int)my_octave;

    string tmp_octave_string;
    // octaves are denoted by , in abc-speak
    if (tmp_octave < 0)
    {
        for (i = 0;tmp_octave < i;tmp_octave++)
        {
            tmp_octave_string += ',';
        }
    }
    else if (((tmp_octave == 2) && (my_letter >= pitch::C)) ||
		(tmp_octave > 2))
    {
		tmp_letter += 'a' - 'A';
        for (i = 0;i < tmp_octave - 1;i++)
        {
            tmp_octave_string += '\'';
        }
    }
    
    // 'A' and 'B' are actually higher than 'C' which is middle c.
    // B below middle C is 'B,'
    if ((tmp_octave <= 0) && (my_letter < pitch::C))
    {
        tmp_octave_string += ',';
    }

    // If the octave is higher than 3rd-treble line B, we use a
    // lower case letter to boost it an octave.
    if (((tmp_octave == 1) && (my_letter >= pitch::C)) ||
             ((tmp_octave == 2) && (my_letter < pitch::C)))
    {
        tmp_letter += ('a' - 'A');
    }

    // Now handle the accidental.  This goes in front of the string
    if (tmp_accidental)
    {
        tmp_rv += tmp_accidental;
    }
    tmp_rv += tmp_letter;
    if (tmp_octave_string.length())
    {
        tmp_rv += tmp_octave_string;
    }

    return tmp_rv;
}

string
pitch::as_string() const
{
    char tmp_accidental = ' ';
    switch (my_accidental)
    {
    case pitch::sharp:
        tmp_accidental = '#';
        break;

    case pitch::flat:
        tmp_accidental = 'b';
        break;

	default:
		break;
    }

    char tmp_letter = 'A' + (char)my_letter;
    string tmp_octave = (long)my_octave;

    return ((string)tmp_accidental + (string)tmp_letter + tmp_octave);
}

pitch 
pitch::up_letter(const pitch& the_key) const
{
    pitch::letter tmp_new_letter = my_letter;
    int tmp_octave = my_octave;
    // increment the letter, and carry the '1'
    if (my_letter == pitch::G)
    {
        tmp_new_letter = pitch::A;
        tmp_octave++;
    }
	else
	{
		tmp_new_letter = (pitch::letter)(1 + (int)(tmp_new_letter));
	}

    // Get the accidental right
    pitch tmp_new_pitch = pitch(tmp_new_letter,pitch::natural,tmp_octave);
    if (tmp_new_pitch.is_in_key(the_key) == false)
    {
        tmp_new_pitch.my_accidental = pitch::sharp;
    }
    if (tmp_new_pitch.is_in_key(the_key) == false)
    {
        tmp_new_pitch.my_accidental = pitch::flat;
    }
    return tmp_new_pitch;
}

pitch 
pitch::down_letter(const pitch& the_key) const
{
    pitch::letter tmp_new_letter = my_letter;
    int tmp_octave = my_octave;

    // increment the letter, and carry the '1'
    if (my_letter == pitch::A)
    {
        tmp_new_letter = pitch::G;
        tmp_octave--;
    }
	else
	{
		tmp_new_letter = (pitch::letter)(-1 + (int)(tmp_new_letter));
	}

    // Get the accidental right
    pitch tmp_new_pitch = pitch(tmp_new_letter,pitch::natural,tmp_octave);
    if (tmp_new_pitch.is_in_key(the_key) == false)
    {
        tmp_new_pitch.my_accidental = pitch::sharp;
    }
    if (tmp_new_pitch.is_in_key(the_key) == false)
    {
        tmp_new_pitch.my_accidental = pitch::flat;
    }

    return tmp_new_pitch;
}

pitch 
pitch::transpose_diatonic(const pitch& the_old_key,int the_halfsteps) const
{
    int i;
    pitch tmp_rv = *this;
    for (i = 0;i < abs(the_halfsteps);++i)
    {
        if (the_halfsteps > 0)
        {
            tmp_rv = tmp_rv.up_letter(the_old_key);
        }
        else
        {
            tmp_rv = tmp_rv.down_letter(the_old_key);
        }
    }
    return tmp_rv;
}

pitch pitch::get_enharmonic_equivalent() const
{
    if ((my_letter == pitch::A) && (my_accidental == pitch::flat))
    {
        return pitch(pitch::G,pitch::sharp,my_octave);
    }
    else if ((my_letter == pitch::B) && (my_accidental == pitch::flat))
    {
        return pitch(pitch::A,pitch::sharp,my_octave);
    }
    else if ((my_letter == pitch::B) && (my_accidental == pitch::natural))
    {
        return pitch(pitch::C,pitch::flat,my_octave);
    }
    else if ((my_letter == pitch::D) && (my_accidental == pitch::flat))
    {
        return pitch(pitch::C,pitch::sharp,my_octave);
    }
    else if ((my_letter == pitch::E) && (my_accidental == pitch::flat))
    {
        return pitch(pitch::D,pitch::sharp,my_octave);
    }
    else if ((my_letter == pitch::E) && (my_accidental == pitch::natural))
    {
        return pitch(pitch::F,pitch::flat,my_octave);
    }
    else if ((my_letter == pitch::F) && (my_accidental == pitch::natural))
    {
        return pitch(pitch::E,pitch::sharp,my_octave);
    }
    else if ((my_letter == pitch::F) && (my_accidental == pitch::sharp))
    {
        return pitch(pitch::G,pitch::flat,my_octave);
    }
    else
    {
        return *this;
    }
}

pitch
pitch::transpose(const pitch& the_old_key,int the_halfsteps)
{
    if (the_halfsteps == 0)
        return *this;

    int tmp_octave = my_octave + (the_halfsteps / NOTES_PER_OCTAVE);
    the_halfsteps %= NOTES_PER_OCTAVE;
    
    pitch rv = *this;
    int tmp_index = rv.get_chromatic_index();
    int tmp_key_index = the_old_key.get_chromatic_index();

    int i;
    int tmp_inc = (the_halfsteps > 0) ? 1 : -1;
    int tmp_limit = (the_halfsteps > 0) ? NOTES_PER_OCTAVE : -1;
    int tmp_start = (the_halfsteps > 0) ? 0 : (NOTES_PER_OCTAVE - 1);
    for (i = 0;i < abs(the_halfsteps);++i)
    {
        tmp_index += tmp_inc;
        if (tmp_index == tmp_limit)
        {
            tmp_octave+= tmp_inc;
            tmp_index = tmp_start;
        }

        tmp_key_index += tmp_limit;
        if (tmp_key_index == tmp_limit)
        {
            tmp_key_index = tmp_start;
        }
    }

    rv = chromatic_up[tmp_index];
    rv = pitch(rv.get_letter(),rv.get_accidental(),tmp_octave);
    pitch tmp_new_key = chromatic_up[tmp_key_index];

    if (rv.is_in_key(tmp_new_key) != is_in_key(the_old_key))
    {
        rv = rv.get_enharmonic_equivalent();
    }

    return rv;
}

#if 0
pitch 
pitch::transpose(const pitch& the_old_key,int the_halfsteps)
{
    if (the_halfsteps == 0)
        return *this;

    // If 5we are transposing octaves, take care of that

    // Find the new key by transposing the old key.
    int tmp_realkeyi = the_old_key.get_chromatic_index();
    tmp_realkeyi = (tmp_realkeyi + the_halfsteps);
	if (tmp_realkeyi < 0)
		tmp_realkeyi += NOTES_PER_OCTAVE;
    pitch tmp_key = chromatic_up[tmp_realkeyi];

    pitch tmp_pitch = tmp_key;
    pitch tmp_tonic = the_old_key;
    pitch tmp_this = pitch(my_letter,my_accidental,0);

    // Now find out what type of pitch this is relative to the old key by
    // transposing up through the scale.
    pitch::letter l;
	for (l = pitch::A;l <= pitch::G;l = (pitch::letter)(1 + (int)l))
        {
        if (tmp_tonic.get_letter() == tmp_this.get_letter())
            {
            break;
            }
        tmp_tonic = tmp_tonic.up_letter(the_old_key);
        tmp_pitch = tmp_pitch.up_letter(tmp_key);
        }

    // Now we have a pitch that has the correct letter, if this is an
    // augmented or diminished interval, handle that
	tmp_tonic = pitch(tmp_tonic.get_letter(),tmp_tonic.get_accidental(),0);
    if (tmp_tonic < tmp_this)
        {
        // Bring the pitch up to match the tonic
        if (tmp_pitch.get_accidental() == flat)
            {
            tmp_pitch = pitch(tmp_pitch.get_letter(),
                              pitch::natural);
            }
        else if (tmp_pitch.get_accidental() == natural)
            {
            tmp_pitch = pitch(tmp_pitch.get_letter(),
                              pitch::sharp);
            }
		else if (tmp_pitch.get_accidental() == sharp)
    		{
            pitch::letter ll = (pitch::letter)
                (((int)tmp_pitch.get_letter() + 1) % ((int)pitch::G + 1));
            tmp_pitch = pitch(ll,pitch::natural,tmp_pitch.get_octave());
    		}
        }
    if (tmp_tonic > tmp_this)
        {
        // Bring the pitch up to match the tonic
        if (tmp_pitch.get_accidental() == sharp)
            {
            tmp_pitch = pitch(tmp_pitch.get_letter(),
                              pitch::natural);
            }
        else if (tmp_pitch.get_accidental() == natural)
            {
            tmp_pitch = pitch(tmp_pitch.get_letter(),
                              pitch::flat);
            }
		else if (tmp_pitch.get_accidental() == flat)
    		{
            pitch::letter ll = (pitch::letter)
                ((int)pitch::F + (int)tmp_pitch.get_letter() % ((int)pitch::G + 1));
            tmp_pitch = pitch(ll,pitch::natural,tmp_pitch.get_octave());
    		}
        }

    if ((my_letter > tmp_pitch.get_letter()) &&
        (the_halfsteps > 0))
        {
        ++tmp_octave;
        }
    else if ((my_letter < tmp_pitch.get_letter()) &&
             (the_halfsteps < 0))
        {
        --tmp_octave;
        }
	tmp_pitch = pitch(tmp_pitch.get_letter(),tmp_pitch.get_accidental(),tmp_octave);
    return tmp_pitch;
}
#endif
bool 
pitch::operator<=(const pitch& o) const
{
    return ((*this == o) || (*this < o));
}
bool 
pitch::operator>=(const pitch& o) const
{
    return ((*this == o) || (*this > o));
}

}
