/*
 * note_figure.cpp - Draws a note with stems and stuff.
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
#include "iabc/note_figure.h"
#include "iabc/figure_factory.h"
#include "iabc/list.cpp"
#include "iabc/note_info.h"
#include "iabc/staff_info.h"
#include "iabc/gm.h"
#include "iabc/array.cpp"
#include "iabc/text_figure.h"
#include "iabc/draw_figure.h"

namespace iabc
{
;
note_figure::note_figure(window& the_window,
                const chord_info& the_info,
                const pitch& the_key)
                :
my_window(the_window),
my_head(0),
my_note(the_info),
my_beamer(0),
need_to_beam(true),
need_to_slur(true),
my_key(the_key),
my_accidental_width(0),
my_dot(0),
my_rest(0),
my_rest_number(0),
my_to_slurer(0),
my_from_slurer(0),
my_high_point(0),
my_low_point(0),
my_head_offset(0),
    my_absolute_right(-1),
    my_absolute_left(-1),
    my_rest_count(0)
{
    my_ornaments.my_figure = 0;
    my_ornament_text.my_figure = 0;
}

note_figure::~note_figure()
{
    if (my_head)
    {
        my_head->remove_ref();
    }
    if (my_beamer)
    {
        my_beamer->remove_ref();
    }
    if (my_dot)
    {
        my_dot->remove_ref();
    }
    if (my_rest)
    {
        my_rest->remove_ref();
    }
    if (my_rest_number)
    {
        my_rest_number->remove_ref();
    }
    if (my_to_slurer)
    {
        my_to_slurer->remove_ref();
    }
    if (my_from_slurer)
    {
        my_from_slurer->remove_ref();
    }
    if (my_ornaments.my_figure)
    {
        my_ornaments.my_figure->remove_ref();
    }
    if (my_ornament_text.my_figure)
    {
        my_ornament_text.my_figure->remove_ref();
    }

    int i;
    for (i = 0;i < my_accidentals.get_size();++i)
    {
        if (my_accidentals[i] != 0)
        {
            my_accidentals[i]->remove_ref();
        }
    }
}

void
note_figure::set_beamer(beamer* the_beamer)
{
    my_beamer = the_beamer;
    the_beamer->add_ref();
}

int
note_figure::get_width()
{
    size tmp_ppi = my_window.pixels_per_inch();
    set_head(my_window.pixels_per_inch());
    int tmp_width = 0;
    int tmp_head_width = 0;
    if (my_head)
    {
        sizeloc tmp_size = my_head->get_size_in_pixels();
        tmp_head_width = tmp_size.my_size.x;
        tmp_head_width += abs(tmp_size.my_loc.x);
        tmp_width += tmp_head_width;
    }
    if (my_beamer)
    {
        tmp_width += my_beamer->get_tail_width(my_note);
    }
    if (my_note.get_size() > 0)
    {
        int tmp_num_dots = get_num_dots(my_note[0]);
        tmp_width += (tmp_num_dots * tmp_head_width);
    }
    return tmp_width + (my_accidental_width * 2);
}

void
note_figure::set_head(const size &the_ppi)
{
    if (my_head == 0)
    {
        figure_factory& f = figure_factory::get_instance();
        if (my_note.get_duration().num >= my_note.get_duration().den)
        {
            my_head = f.get_figure(the_ppi,figure_factory::WHOLE_NOTE,my_scale);
        }
        else if (my_note.get_duration().num * 2 >= my_note.get_duration().den)
        {
            my_head = f.get_figure(the_ppi,figure_factory::OPEN_HEAD,my_scale);
        }
        else
        {
            my_head = f.get_figure(the_ppi,figure_factory::CLOSED_HEAD,my_scale);
        }
        my_head->add_ref();
    }
}

void
note_figure::set_accidentals(pitch::accidental_map& the_accidentals)
{
    my_accidentals.expand_to(my_note.get_size());
    for (int i = 0;i < my_note.get_size(); ++i)
    {
        pitch tmp_pitch = my_note[i].get_pitch();
        if ((tmp_pitch.should_print_accidental(the_accidentals,my_key)
                                              == true) &&
            (my_note[i].is_rest() == false))
        {
            tmp_pitch.add_accidental(the_accidentals);
            figure_factory::FIGURE_ENUM tmp_accidental_type;
            if (my_note[i].get_pitch().get_accidental() == pitch::flat)
            {
                tmp_accidental_type = figure_factory::FLAT_SIGN;
            }
            else if (my_note[i].get_pitch().get_accidental() == pitch::sharp)
            {
                tmp_accidental_type = figure_factory::SHARP_SIGN;
            }
            else if (my_note[i].get_pitch().get_accidental() == pitch::natural)
            {
                tmp_accidental_type = figure_factory::NATURAL_SIGN;
            }
            else
            {
                tmp_accidental_type = figure_factory::SHARP_SIGN;
            }
            my_accidentals[i] = figure_factory::get_instance().get_figure(
                my_window.pixels_per_inch(),
                tmp_accidental_type,
                my_scale);
            my_accidentals[i]->add_ref();
            if (my_accidentals[i]->get_width_in_pixels() > my_accidental_width)
            {
                my_accidental_width = my_accidentals[i]->get_width_in_pixels();
            }
        }
        else
        {
            my_accidentals[i] = 0;
        }
    }
}

void
note_figure::draw_accidentals(figure_container_if& the_page,staff_info the_staff)
{
    for (int i = 0;i < my_note.get_size(); ++i)
    {
        figure* tmp_figure = my_accidentals[i];
        if (tmp_figure)
        {
            tmp_figure->add_ref();
            my_accidental_width = tmp_figure->get_width_in_pixels();
            my_origin =
                place_note(my_window,my_note[i],the_staff);
            my_origin.x -= tmp_figure->get_size_in_pixels().my_loc.x;
            update_high_low(my_origin.y,my_origin.y);
            update_left_right(tmp_figure,my_origin);
            the_page.add_figure(my_origin,*tmp_figure);
            my_origin.x += (my_accidental_width * 2);
            tmp_figure->remove_ref();
        }
    }
}

void
note_figure::dot_notes(figure_container_if& the_page,const point& the_origin)
{
    set_dot(my_window.pixels_per_inch());
    point tmp_origin = the_origin;
    tmp_origin += my_dot->get_size_in_pixels().my_loc;
    for (int i = 0;i < my_note.get_size();++i)
    {
        note_info tmp_note = my_note[i];
        for (int j = 0;j < get_num_dots(tmp_note);++j)
        {
            tmp_origin.x += (my_head->get_width_in_pixels());
            the_page.add_figure(tmp_origin,*my_dot);
        }
    }
}

void
note_figure::set_rest(const fraction& the_duration,const size& the_ppi)
{
    if (my_rest == 0)
    {
        figure_factory::FIGURE_ENUM tmp_figure = figure_factory::QUARTER_REST;
        if (the_duration <= fraction(3,32))
        {
            tmp_figure = figure_factory::SIXTEENTH_REST;
        }
        else if (the_duration <= fraction(3,16))
        {
            tmp_figure = figure_factory::EIGHTH_REST;
        }
        else if (the_duration >= fraction(1,1))
        {
            // If this is a multi-bar rest, indicate such.
            if (the_duration > fraction(1,1))
            {
                my_rest = new long_rest_figure(the_ppi,size(40,10));
                my_rest_number = new text_figure(
                    font::serifs,
                                14,
                                my_window,
                                font::normal);
                my_rest_number->set_string(string((long)the_duration.as_int()));
                my_rest_number->add_ref();
            }
            else
            {
                tmp_figure = figure_factory::WHOLE_REST;
            }
        }
        else if (the_duration >= fraction(1,2))
        {
            tmp_figure = figure_factory::HALF_REST;
        }
        if (my_rest == 0)
        {
            my_rest = figure_factory::get_instance().get_figure(the_ppi,
                                                                tmp_figure,my_scale);
        }
        my_rest->add_ref();
    }
}

void
note_figure::add_to_slurer(slurer* the_slurer)
{
    if (my_to_slurer)
    {
        my_to_slurer->remove_ref();
    }
    my_to_slurer = the_slurer;
    my_to_slurer->add_ref();
}

void
note_figure::rescale(const scale& the_scale)
{
    my_scale = the_scale;
}

void
note_figure::add_from_slurer(slurer* the_slurer)
{
    if (my_from_slurer)
    {
        my_from_slurer->remove_ref();
    }
    my_from_slurer = the_slurer;
    my_from_slurer->add_ref();
}

void
note_figure::set_dot(const size& the_ppi)
{
    if (my_dot == 0)
    {
        my_dot = figure_factory::get_instance().get_figure(
            the_ppi,figure_factory::DOT,my_scale);
        my_dot->add_ref();
    }
}

int
note_figure::get_num_dots(const note_info& the_note) const
{
    int tmp_num_dots = 0;

    // We dot up to 64/th notes, and we assume that
    // the denominator of the time signature is some
    // multiple of 2.
    fraction tmp_current_duration = the_note.get_duration();
    tmp_current_duration.reduce();
    int tmp_possible_dots = 0;
    fraction tmp_dot = 0;
    for (int j = 2;j <=64; j *=2)
    {
        // Only start looking at the note if it is larger than
        // a 1/j note.
        if (fraction(1,j)*3 <= tmp_current_duration)
        {
            tmp_possible_dots++;
            if (tmp_dot.num == 0)
            {
                tmp_dot = fraction(1,j);
                tmp_dot *= 3;
            }
            else
            {
                tmp_dot += fraction(1,j);
            }
            if (tmp_current_duration == tmp_dot)
            {
                tmp_num_dots = tmp_possible_dots;
            }
        }
    }
    return tmp_num_dots;
}

void
note_figure::handle_slurers(figure_container_if& the_page,
                            const staff_info& the_staff)
{
    // Only compute the coordinates one time
    if (need_to_slur == true)
    {
        point tmp_origin = my_origin;
        int tmp_upness = 1;
        if (my_beamer)
        {
            tmp_upness = my_beamer->get_upness_value();
        }

        if (tmp_upness == 0)
        {
            tmp_upness = the_staff.center_pitch > my_note[0].get_pitch() ?
                1 : -1;
        }
        tmp_origin.y += (my_head->get_height_in_pixels() / 4 * tmp_upness );

        // We bend the slur down, unless the stems go down, in which case
        // we bend the slur up.
        if (my_from_slurer)
        {
            need_to_slur = false;

            my_from_slurer->add_from_point(tmp_origin);
        }
        if (my_to_slurer)
        {
            need_to_slur = false;
            slurer::slur_direction tmp_direction = slurer::down;
            if ((my_beamer) && (my_beamer->get_upness_value() > 0))
            {
                tmp_direction = slurer::up;
            }
            my_to_slurer->add_to_point(tmp_origin,tmp_direction);
        }
    }
}

void
note_figure::update_high_low(int the_possible_low,int the_possible_high)
{
    if (the_possible_low < my_low_point)
    {
        my_low_point = the_possible_low;
    }
    if (the_possible_high < my_low_point)
    {
        my_low_point = the_possible_high;
    }
    if (the_possible_high > my_high_point)
    {
        my_high_point = the_possible_high;
    }
    if (the_possible_low > my_high_point)
    {
        my_high_point = the_possible_low;
    }
}

void
note_figure::update_left_right(figure* the_figure,const point& the_point)
{
    sizeloc tmp_size = the_figure->get_size_in_pixels();
    int tmp_left = the_point.x  + tmp_size.my_loc.x;
    int tmp_right = tmp_left + tmp_size.my_size.x;
    if ((my_absolute_left < 0) ||
        (tmp_left < my_absolute_left))
    {
        my_absolute_left = tmp_left;
    }
    if ((my_absolute_right < 0) ||
        (tmp_right > my_absolute_right))
    {
        my_absolute_right = tmp_right;
    }
}

void
note_figure::handle_ornaments(figure_container_if& the_page,const staff_info& the_staff)
{
    bool tmp_below = true;
    if ((my_beamer) && (my_beamer->get_upness_value() > 0))
    {
        tmp_below = false;
    }

    pitch tmp_pitch;
    if (my_note.get_embellishment() == chord_info::down_bow)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::DOWN_BOW_FIGURE,my_scale);
    }
    else if (my_note.get_embellishment() == chord_info::fermata)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::FERMATA_FIGURE,my_scale);
        my_ornaments.my_point = my_origin;
        my_ornaments.my_point.y = my_low_point;
        if (my_ornaments.my_point.y > the_staff.vertical_position)
            my_ornaments.my_point.y = the_staff.vertical_position;

    }
    else if (my_note.get_embellishment() == chord_info::accent)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::ACCENT_FIGURE,my_scale);
    }
    else if (my_note.get_embellishment() == chord_info::gracing)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::GRACING_FIGURE,my_scale);
    }
    else if (my_note.get_embellishment() == chord_info::staccato)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::DOT,my_scale);
    }
    else if (my_note.get_embellishment() == chord_info::up_bow)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::UP_BOW_FIGURE,my_scale);
    }
    else if (my_note.get_embellishment() == chord_info::mordant)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::MORDANT,my_scale);
    }
    else if (my_note.get_embellishment() == chord_info::trill)
    {
        my_ornaments.my_figure = figure_factory::get_instance().get_figure(
            my_window.pixels_per_inch(),figure_factory::TRILL,my_scale);
        my_ornament_text.my_figure =
            new text_figure(font::serifs,
                        14,
                        my_window,
                        font::slanty);

    }

    if (my_ornaments.my_figure)
    {
        my_ornaments.my_figure->add_ref();
        if (tmp_below == false)
        {
            my_ornaments.my_point = my_origin;
            note_info tmp_info = my_note.get_high_note();
            tmp_info.change_pitch(tmp_info.get_pitch().up_letter(my_key));
            tmp_info.change_pitch(tmp_info.get_pitch().up_letter(my_key));
            my_ornaments.my_point =
                place_note(my_window,tmp_info,the_staff);
            if (my_ornament_text.my_figure != 0)
            {
                my_ornament_text.my_figure->add_ref();
                tmp_info.change_pitch(tmp_info.get_pitch().up_letter(my_key));
                tmp_info.change_pitch(tmp_info.get_pitch().up_letter(my_key));
                my_ornament_text.my_point = place_note(my_window,tmp_info,the_staff);
                rect tmp_rect = my_ornament_text.my_figure->get_rect("tr");
                my_ornament_text.my_point.y -=
                    tmp_rect.height();
                my_ornament_text.my_point.x -= tmp_rect.width()/2;
            }
        }
        else if (my_note.get_embellishment() != chord_info::fermata)
        {
            my_ornaments.my_point = my_origin;
            note_info tmp_info = my_note.get_low_note();
            tmp_info.change_pitch(tmp_info.get_pitch().down_letter(my_key));
            tmp_info.change_pitch(tmp_info.get_pitch().down_letter(my_key));
            my_ornaments.my_point =
                place_note(my_window,tmp_info,the_staff);
            if (my_ornament_text.my_figure != 0)
            {
                my_ornament_text.my_figure->add_ref();
                tmp_info.change_pitch(tmp_info.get_pitch().down_letter(my_key));
                tmp_info.change_pitch(tmp_info.get_pitch().down_letter(my_key));
                rect tmp_rect = my_ornament_text.my_figure->get_rect("tr");
                my_ornament_text.my_point = place_note(my_window,tmp_info,the_staff);
                my_ornament_text.my_point.x -= tmp_rect.width()/2;
            }
        }

        update_high_low(my_ornaments.my_point.y,my_ornaments.my_point.y);
        the_page.add_figure(my_ornaments.my_point,*my_ornaments.my_figure);
        if (my_ornament_text.my_figure != 0)
        {
            update_high_low(my_ornament_text.my_point.y,my_ornament_text.my_point.y);
            the_page.add_text_figure(my_ornament_text.my_point,
                *my_ornament_text.my_figure);
        }
    }
}

void
note_figure::render(figure_container_if& the_page,
                    staff_info the_staff)
{
    if (my_note.get_size() == 0)
        return;

    my_low_point = the_staff.vertical_position;
    my_origin = point(the_staff.horizontal_position,the_staff.vertical_position);
    fraction tmp_duration = my_note.get_duration();
    tmp_duration.reduce();
    set_head(my_window.pixels_per_inch());

    // Set the accidentals for this note, if any.
    draw_accidentals(the_page,the_staff);

    int tmp_head_width = 0;

    // Place the heads of the note in the right place
    for (int i = 0;i < my_note.get_size(); ++i)
    {
        note_info tmp_note = my_note[i];
        figure* tmp_figure = my_head;
        tmp_head_width = tmp_figure->get_width_in_pixels();
        if (tmp_note.is_rest() == true)
        {
            tmp_note = note_info(
                pitch(the_staff.center_pitch),
                      tmp_note.get_duration(),
                      true);
            set_rest(tmp_note.get_duration(),my_window.pixels_per_inch());
            tmp_figure = my_rest;
        }

        my_head_offset = my_origin.x - the_staff.horizontal_position;
        the_staff.horizontal_position = my_origin.x;
        my_origin =
            place_note(my_window,tmp_note,the_staff);
        the_page.add_figure(my_origin,*tmp_figure);

        if ((my_rest_number) && (my_rest))
        {
            point tmp_number_origin = my_origin;
            tmp_number_origin.x += my_rest->get_width_in_pixels() / 2;
            tmp_number_origin.y -= (my_rest->get_height_in_pixels()) / 2;
            tmp_number_origin.y -= (my_rest_number->get_current_rect().height());
            the_page.add_text_figure(tmp_number_origin,*my_rest_number);
        }
        sizeloc tmp_figure_size = tmp_figure->get_size_in_pixels();
        update_high_low(my_origin.y - tmp_figure_size.my_loc.y,
                        my_origin.y + tmp_figure_size.my_size.y - tmp_figure_size.my_loc.y);
        update_left_right(tmp_figure,my_origin);
    }

    // beam the notes, if we have to.  First add the note to the beam group
    if ((my_beamer) && (need_to_beam) && (my_note[0].is_rest() == false))
    {
        my_beamer->beam_me(my_origin,*my_head,my_note.get_duration(),
                           my_note.get_xlet_value());
        need_to_beam = false;
    }

    array<point> tmp_xlet_array;
    array<int> tmp_xlet_value;

    // render the beam if this is the last note in a beam group.
    if ((my_beamer) &&
        (get_note().is_last_in_beam() == true)
         && (my_note[0].is_rest() == false))
    {
        rect tmp_box = my_beamer->get_bounding_box();
        the_page.add_figure(my_beamer->get_absolute_ul(),*my_beamer);

        // Also make note if the dimensions of the beam extend outside
        // the shape of the note (which it almost always will)
        fraction tmp_longest_beamed_thing =
            fraction(1,8) + fraction(1,16) + fraction(1,32) + fraction(1,64);
        update_high_low(my_beamer->get_high_point(),
                            my_beamer->get_low_point());


        // Now that we've drawn the beam we can put xlet values on.
        tmp_xlet_array = my_beamer->get_xlet_points();
        tmp_xlet_value = my_beamer->get_xlet_values();
        for (int i = 0;i < tmp_xlet_array.get_size();++i)
        {
            text_figure* tmp_figure = new text_figure(font::serifs,
                8 * int(my_scale.y + 0.5),
                my_window);
            tmp_figure->add_ref();
            tmp_figure->get_rect((string)(long)tmp_xlet_value[i]);
            rect tmp_rect = tmp_figure->get_rect((string)(long)tmp_xlet_value[i]);
            update_high_low(tmp_xlet_array[i].y + tmp_rect.ul.y,
                            tmp_xlet_array[i].y + tmp_rect.lr.y);
            // tmp_figure->dispatch_draw_self((string)(long)tmp_xlet_value[i],tmp_xlet_array[i]);
            the_page.add_text_figure(tmp_xlet_array[i],*tmp_figure);
            tmp_figure->remove_ref();
        }
    }

    // Now add any dots to this note
    dot_notes(the_page,my_origin);

    handle_slurers(the_page,the_staff);

    // handle_ornaments(the_page,the_staff);
}

}
