/*
 * key_sig.cpp - Renders a key signature on a staff.
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
#include "iabc/key_sig.h"
#include "iabc/gm.h"
namespace iabc
{

pitch treble_sharps[7] =
{
    pitch(pitch::F,pitch::sharp,1),
    pitch(pitch::C,pitch::sharp,1),
    pitch(pitch::G,pitch::sharp,0),
    pitch(pitch::D,pitch::sharp,1),
    pitch(pitch::A,pitch::sharp,1),
    pitch(pitch::E,pitch::sharp,1),
    pitch(pitch::B,pitch::sharp,1)
};

pitch treble_flats[7] =
{
    pitch(pitch::B,pitch::flat,1),
    pitch(pitch::E,pitch::flat,1),
    pitch(pitch::A,pitch::flat,1),
    pitch(pitch::D,pitch::flat,1),
    pitch(pitch::G,pitch::flat,0),
    pitch(pitch::C,pitch::flat,1),
    pitch(pitch::F,pitch::flat,0)
};

pitch base_sharps[7] = 
{
    pitch(pitch::F,pitch::sharp,-1),
    pitch(pitch::C,pitch::sharp,-1),
    pitch(pitch::G,pitch::sharp,-2),
    pitch(pitch::D,pitch::sharp,-1),
    pitch(pitch::A,pitch::sharp,-1),
    pitch(pitch::E,pitch::sharp,-1),
    pitch(pitch::B,pitch::sharp,-1)
};

pitch base_flats[7] =
{
    pitch(pitch::B,pitch::flat,0),
    pitch(pitch::E,pitch::flat,-1),
    pitch(pitch::A,pitch::flat,0),
    pitch(pitch::D,pitch::flat,-1),
    pitch(pitch::G,pitch::flat,-1),
    pitch(pitch::C,pitch::flat,-1),
    pitch(pitch::F,pitch::flat,-1)
};

key_sig::key_sig(const pitch& the_key,
                 pitch::clef the_clef,
                 const scale& the_scaling_factor,
                 const pitch& the_last_key):
        my_sharp(0),my_flat(0),my_clef(the_clef),my_key(the_key),
        my_horizontal_offset(0),my_bar(0),my_natural(0),
        my_scaling_factor(the_scaling_factor),
        my_last_key(the_last_key)
{
}

key_sig::~key_sig()
{
    if (my_sharp) my_sharp->remove_ref();
    if (my_flat)
    {
        my_flat->remove_ref();
    }
    if (my_bar)
    {
        my_bar->remove_ref();
    }
    if (my_natural)
    {
        my_natural->remove_ref();
    }
}

void 
key_sig::draw_self(figure_container_if& the_page, 
                   window& the_window,
                   const staff_info& the_staff,bool the_should_draw) 
{
	my_horizontal_offset = 0;

    // If there's no key signature, there's nothing to draw
    if ((is_sharp_key() == false) &&
        (is_flat_key() == false) &&
        (my_last_key == my_key))
    {
        return;
    }

    // populate the figure attribute
    size tmp_size = the_window.pixels_per_inch();
    make_figure(tmp_size,the_should_draw);
    figure* tmp_figure = is_sharp_key() ? my_sharp : my_flat;

    // Now loop through and draw all of the figures.  The
    // place they go should be in this table
    key_sig_pitches* ar = get_pitches(my_key);
    key_sig_pitches& tmp_pitches = *ar;
    int tmp_figure_count = get_number_figures(my_key);
    int tmp_natural_count = get_number_naturals();

    point tmp_place;
    my_horizontal_offset -= tmp_figure->get_size_in_pixels().my_loc.x;
	int i;
    for (i = 0;tmp_figure && (i < tmp_figure_count);++i)
    {
        chord_info tmp_info(tmp_pitches[i],fraction(1,4));
        tmp_place = place_note(the_window,tmp_info[0],the_staff);
        tmp_place.x += my_horizontal_offset;
        the_page.add_figure(tmp_place,*tmp_figure);
        my_horizontal_offset += tmp_figure->get_width_in_pixels() * 2;
    }

    for (i = 0;
         i < tmp_natural_count;
         ++i)
    {
        chord_info tmp_info(tmp_pitches[i + tmp_figure_count],fraction(1,4));
        tmp_place = place_note(the_window,tmp_info[0],the_staff);
        tmp_place.x += my_horizontal_offset;
        the_page.add_figure(tmp_place,*my_natural);
        my_horizontal_offset += my_natural->get_width_in_pixels() * 2;
    }

    if (my_bar)
    {
		point tmp_bar_point = point(tmp_place.x + tmp_figure->get_width_in_pixels(),
                                 the_staff.vertical_position - 
                                    my_bar->get_size_in_pixels().my_loc.y);
        the_page.add_figure(tmp_bar_point,*my_bar);
    }
}

int 
key_sig::get_x_offset(const size& the_ppi)
{
    make_figure(the_ppi,false);
    figure* tmp_figure = is_sharp_key() ? my_sharp : my_flat;
    return tmp_figure->get_size_in_pixels().my_loc.x;
}

int 
key_sig::get_width(const size &the_ppi,bool the_draw_bar)
{
    make_figure(the_ppi,the_draw_bar);
    figure* tmp_figure = is_sharp_key() ? my_sharp : my_flat;
    int tmp_rv = 0;
	// If there's no figure to draw, we're in key of C, else
	// the width of the key signature is the width of a figure
	// * number of figures, with one more for some padding on the 
	// end
    if (tmp_figure)
    {
        tmp_rv = 2 * 
            tmp_figure->get_width_in_pixels() * (get_number_figures(my_key));
    }
    if (my_natural)
    {
        tmp_rv += 2 * 
            my_natural->get_width_in_pixels() * (get_number_naturals());
    }
    if (the_draw_bar == true)
    {
        tmp_rv += my_bar->get_width_in_pixels();
    }

    // When we draw we leave space between the sharps, so compensate for that here
    return tmp_rv;
}

void
key_sig::make_figure(const size& the_size,bool the_draw_bar)
{
    if ((is_sharp_key() == true) && (my_sharp == 0))
    {
        my_sharp = figure_factory::get_instance().get_figure(the_size,
                                                             figure_factory::SHARP_SIGN,
                                                             my_scaling_factor);
		my_sharp->add_ref();
    }
    else if ((is_flat_key() == true) && (my_flat == 0))
    {
        my_flat = figure_factory::get_instance().get_figure(the_size,
                                                            figure_factory::FLAT_SIGN,
                                                            my_scaling_factor);
		my_flat->add_ref();
    }
    if ((the_draw_bar == true) &&
		(my_bar == 0))
    {
        my_bar = figure_factory::get_instance().get_figure(the_size,figure_factory::SINGLE_BAR,
                                                           my_scaling_factor);
        my_bar->add_ref();
    }
    int tmp_naturals = get_number_naturals();
    if ((my_natural == 0) && 
		(tmp_naturals > 0))
    {
        my_natural = figure_factory::get_instance().get_figure(the_size,
                                                               figure_factory::NATURAL_SIGN,
                                                               my_scaling_factor);
        my_natural->add_ref();
    }
}

bool 
key_sig::is_last_sharp_key()
{
    return is_sharp_key(my_last_key);
}

bool 
key_sig::is_last_flat_key()
{
    return is_flat_key(my_last_key);
}

bool
key_sig::is_sharp_key()
{
    return is_sharp_key(my_key);
}
bool
key_sig::is_flat_key()
{
    return is_flat_key(my_key);
}
bool
key_sig::is_sharp_key(const pitch& the_pitch)
{
    if ((the_pitch.get_accidental() == pitch::sharp) ||
        ((the_pitch.get_accidental() == pitch::natural) &&
         (the_pitch.get_letter() != pitch::F)))
    {
        return true;
    }
    return false;
}

bool
key_sig::is_flat_key(const pitch& the_pitch)
{
    if ((is_sharp_key(the_pitch) != true) &&
        (the_pitch.get_letter() != pitch::C))
    {
        return true;
    }
    return false;
}

int 
key_sig::get_number_figures(const pitch& the_key)
{
    int i;
    int tmp_rv = 0;
    key_sig_pitches* ar = get_pitches(the_key);
    if (ar)
    {
        key_sig_pitches& tmp_pitches = *ar;
        for (i = 0;i < 7; ++i)
        {
            if (tmp_pitches[i].is_in_key(the_key) == true)
            {
                ++tmp_rv;
            }
        }
    }
    return tmp_rv;
}

int
key_sig::get_number_naturals()
{
    int tmp_figures = get_number_figures(my_key);
    int tmp_last_figures = get_number_figures(my_last_key);
    if ((my_last_key.get_accidental() == my_key.get_accidental()) ||
        ((my_key.get_accidental() == pitch::natural) &&
         (my_key.get_letter() == pitch::C)))
    {
        if (tmp_last_figures > tmp_figures)
        {
            return tmp_last_figures - tmp_figures;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}

key_sig_pitches* 
key_sig::get_pitches(const pitch& the_pitch)
{
    if (is_sharp_key(the_pitch) == true)
    {
        if (my_clef == pitch::TrebleClef)
        {
            return &treble_sharps;
        }
        else
        {
            return &base_sharps;
        }
    }
    else
    {
        if (my_clef == pitch::TrebleClef)
        {
            return &treble_flats;
        }
        else
        {
            return &base_flats;
        }
    }
}

}
