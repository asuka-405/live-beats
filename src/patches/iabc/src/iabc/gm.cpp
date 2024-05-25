/*
 * gm.cpp - Functions that place notes in the right place.
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
#include "iabc/gm.h"
#include "iabc/list.cpp"

namespace iabc
{

int
note_height(const pitch& the_pitch,int the_staff_height,const pitch& center_line)
{
    // figure out if we're counting steps above or below the center line
    int tmp_step;
    if ((center_line.get_octave() > the_pitch.get_octave()) ||
        (center_line.get_octave() == the_pitch.get_octave() && 
         center_line.get_letter() > the_pitch.get_letter()))
    {
        tmp_step = 1;
    }
    else
    {
        tmp_step = -1;
    }

    pitch tmp_pitch = the_pitch;
    int tmp_vertical_index = 0;

    // step up or down from our location to the center line.  We 
    // only need to count raw letter changes since we're concerned
    // with vertical note placement.
    while ((tmp_pitch.get_octave() != center_line.get_octave()) ||
           (tmp_pitch.get_letter() != center_line.get_letter()))
    {
        if (tmp_step > 0)
        {
            tmp_pitch = tmp_pitch.up_letter(pitch());
        }
        else
        {
            tmp_pitch = tmp_pitch.down_letter(pitch());
        }
        ++tmp_vertical_index;
    }

    // the staff has 9 places for notes.  Figure out which one we occupy
	// the scaling is for precision.
    int tmp_vertical = (10000 * the_staff_height* tmp_vertical_index) / 80000;
    
    // return the offset in the correct direction
    return tmp_step * tmp_vertical;
}

int 
get_head_vertical(int the_staff_height,
                      const pitch& the_pitch,
                      const pitch& the_middle_pitch
                      ) 
{
    int tmp_vertical = note_height(the_pitch,the_staff_height,the_middle_pitch);
    // tmp_vertical -= the_staff_height / (LETTERS_PER_OCTAVE + 1);
	return tmp_vertical;
}

point 
place_note(window& w,
                            const note_info& the_note,
                            staff_info the_staff)
{
    int i = 0;
	point tmp_origin;
    size tmp_ppi(w.pixels_per_inch());

    // Get the vertical position of the middle pitch on the staff,
    // in pixels
    int tmp_vertical = 
        the_staff.vertical_position + 
        the_staff.size.my_size.y/2;

    // then offset the vertical position according to the pitch
    // we're trying to represent
    tmp_vertical += get_head_vertical(the_staff.size.my_size.y,
        (the_note.get_pitch()),
        the_staff.center_pitch);
    
    tmp_origin = point(the_staff.horizontal_position,tmp_vertical);
    
    // Fudge factor
    if (the_note.get_pitch() < the_staff.center_pitch)
    {
        tmp_origin.y += 1;
    }
    // we calculate the exact point where we want the thing, so remove
    // any offset
    tmp_origin -= the_staff.size.my_loc;

    return tmp_origin;
}
}
