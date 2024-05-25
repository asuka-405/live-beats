/*
 * line_of_music.cpp - Draw a single staff, then invoke classes to draw all the music.
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
#include "iabc/line_of_music.h"
#include "iabc/wd_temp.cpp"
#include "iabc/list.h"
#include "iabc/list.cpp"
#include "iabc/map.cpp"
#include "iabc/array.cpp"
#include "iabc/text_figure.h"
#include "iabc/draw_figure.h"
#include "iabc/iabc_preferences.h"

#include <math.h>
#ifdef _MSC_VER
#include "windows.h"
#endif

namespace iabc
{
;

int 
line_of_music::static_staff_count = 0;

line_of_music::line_of_music(window& w,
      const scale& the_scale,
      const scale& the_note_scale,
      double page_width,
      const voice_info& the_info,
      const point& the_origin,
      int the_start_measure,
      bool the_draw_part_first):
my_window(w),my_last_beamer(0),
my_note_scale(the_note_scale),
my_start_measure(the_start_measure),my_end_measure(the_start_measure),
my_should_center(true),
my_origin(the_origin),
my_pixels_for_notes(0),
my_scale_factor(the_scale),
my_width(page_width),
my_high_point(the_origin.y),
my_low_point(the_origin.y),
my_rhs_overlap(0),
my_spare_pixels(0),
my_last_grace_beamer(0),
my_begin_barline(0),
my_has_rendered(false),
my_y_offset(0),
my_is_complete(false),
my_first_notes_added(false),
my_clef_x(0),my_draw_part_first(the_draw_part_first)
{
    ++static_staff_count;
    my_voice_index = the_info.my_index;
    my_voice_info_map.add_pair(my_start_measure,the_info);
    my_staff = (figure_factory::get_instance().get_figure(my_window.pixels_per_inch(),
		 figure_factory::STAFF,scale(my_scale_factor.x * (my_width/DEFAULT_PAGE_WIDTH) ,my_scale_factor.y)));
    my_staff_size = my_staff->get_size_in_pixels();
    figure_factory::FIGURE_ENUM tmp_enum = figure_factory::GCLEF;
    if (the_info.my_clef == pitch::BassClef)
        {
        tmp_enum = figure_factory::FCLEF;
        }
    my_clef = 
		figure_factory::get_instance().get_figure(my_window.pixels_per_inch(),
		tmp_enum,my_scale_factor);

    // Staffs and clefs are all the same, so we don't have to worry about re-
    // rendering them every time.
    my_clef->add_ref();
    my_staff->add_ref();
    if (the_info.my_clef == (int)pitch::TrebleClef)
    {
        my_center_pitch = pitch(pitch::B,pitch::natural,1);
    }
    else
    {
        my_center_pitch = pitch(pitch::D,pitch::natural,-1);
    }
    my_slurer = new slurer(my_window.pixels_per_inch(),*this);
	my_slurer->add_ref();
}

line_of_music::~line_of_music()
{
    --static_staff_count;

    list<text_figure::point_text_pair>::iterator tmp_text_it = 
        my_text_list.first();
    while (tmp_text_it)
    {
        (*tmp_text_it).my_figure->remove_ref();
        tmp_text_it.next();
    }

    // Some staffs don't get displayed, avoid leak and clear out
    // stuff that didn't get cleaned up after display.
    cleanup();
}

void
line_of_music::cleanup()
{
    // Release the clef figure.
    if (my_clef)
    {
        my_clef->remove_ref();
        my_clef = 0;
    }
    
    // Release the shared staff figure
    if (my_staff)
    {
        my_staff->remove_ref();
        my_staff = 0;
    }

    // Release the staff-wide slurer object
	if (my_slurer)
    {
		my_slurer->remove_ref();
        my_slurer = 0;
    }

    // Release the key signature
    delete_functor<key_sig*> the_functor;
    do_to_all(my_key_sigs,the_functor);
	my_key_sigs.clear();

    delete_functor<time_sig*> tmp_timed;
    do_to_all(my_time_sigs,tmp_timed);
	my_time_sigs.clear();

    // Now release all the notes 1 at a time.  This in turn
    // releases all the beam objects.
    map<staff_point,point_note_pair>::iterator tmp_it = 
        my_music.least();
    while (tmp_it)
    {
        note_figure* tmp_note = (*tmp_it).value.the_figure;
        delete tmp_note;
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }
    my_music.clear();

    // Do the same with the text words
    map<staff_point,text_figure::point_text_array>::iterator tmp_text = 
        my_words.get_item(staff_point(),gteq);
    while (tmp_text)
    {
		text_figure::point_text_array 
			tmp_array = (*tmp_text).value;
        int i;
        for (i = 0;i < tmp_array.get_size();++i)
        {
            text_figure* tmp_figure = tmp_array[i].my_figure;
            tmp_figure->remove_ref();
        }
        tmp_text = my_words.get_item((*tmp_text).key,gt);
    }
    my_words.clear();

    // Now release all the barline figures.
    map<int,figure*>::iterator  tmp_it1 = 
         my_barlines.least();
    while (tmp_it1)
    {
        figure* tmp_figure = (*tmp_it1).value;
        tmp_figure->remove_ref();
        tmp_it1 = tmp_it1.get_item((*tmp_it1).key,gt);
    }
    my_barlines.clear();

	if (my_begin_barline)
    {
		my_begin_barline->remove_ref();
        my_begin_barline = 0;
    }

    map<int,nth_ending_info>::iterator tmp_nth = 
        my_nth_ending_map.get_item(0,gteq);
    while (tmp_nth)
    {
        int tmp_key = (*tmp_nth).key;
        nth_ending_info tmp_info = (*tmp_nth).value;
        if (tmp_info.my_number_figure)
        {
            tmp_info.my_number_figure->remove_ref();
        }
        if (tmp_info.my_bracket_figure)
        {
            tmp_info.my_bracket_figure->remove_ref();
        }
		tmp_nth = my_nth_ending_map.get_item(tmp_key,gt);
    }

    my_nth_ending_map.clear();
}

void 
line_of_music::line_add_ref()
{
    my_count++;
}

void 
line_of_music::add_text_figure(const point_ordinal& the_point,text_figure& the_figure)
{
    text_figure::point_text_pair tmp_pair;
    tmp_pair.my_point = (point)the_point;
    tmp_pair.my_figure = &the_figure;
    the_figure.add_ref();
    add_at_end(my_text_list,tmp_pair);
    
    // Store this rectangle for placement of note figures.
    rect tmp_bounding_rect = the_figure.get_current_rect();
    tmp_bounding_rect = tmp_bounding_rect.offset((point)the_point);
    insert_rect_into_map(tmp_bounding_rect);
}

void 
line_of_music::add_figure(const point_ordinal& the_sort_point,figure& the_figure)
{
    the_figure.add_ref();
    map<point_ordinal,figure_array>::iterator tmp_it = 
        my_figure_list.get_item(the_sort_point);

    // Store this rectangle for placement of note figures.
    rect tmp_bounding_rect = the_figure.get_bounding_box();
    tmp_bounding_rect = tmp_bounding_rect.offset((point)the_sort_point);
    insert_rect_into_map(tmp_bounding_rect);

    // Replace it if there's a figure there already.
    if (tmp_it)
    {
        figure_array tmp_array = (*tmp_it).value;
        tmp_array.add_to_end(&the_figure);
    }
	else
	{
		figure_array tmp_array;
        tmp_array.add_to_end(&the_figure);
		my_figure_list.add_pair(the_sort_point,tmp_array);
	}
}

voice_info&
line_of_music::get_voice_info(int the_measure)
{
    map<int,voice_info>::iterator
        tmp_voice_info_map = my_voice_info_map.get_item(the_measure);
    if (!(tmp_voice_info_map))
    {
	    throw("Illegal index into part array");
    }

    return (*tmp_voice_info_map).value;
}

void
line_of_music::insert_rect_into_map(const rect& the_rect)
{
    map<int,rectangle_array>::iterator tmp_it =
         my_rect_map.get_item(the_rect.ul.x);

    if (tmp_it)
    {
        rectangle_array& tmp_array = (*tmp_it).value;
        tmp_array.add_to_end(the_rect);
    }
    else
    {
        rectangle_array tmp_array;
        tmp_array.add_to_end(the_rect);
        my_rect_map.add_pair(the_rect.ul.x,tmp_array);
    }
}

void 
line_of_music::add_slur_from(const staff_point& my_point)
{
    map<staff_point,point_note_pair>::iterator tmp_pair = 
        my_music.get_item(my_point,exact);

    if (tmp_pair)
    {
        note_figure* tmp_figure = (*tmp_pair).value.the_figure;
        tmp_figure->add_from_slurer(my_slurer);
    }
}
void 
line_of_music::add_slur_to(const staff_point& my_point)
{
    map<staff_point,point_note_pair>::iterator tmp_pair = 
        my_music.get_item(my_point,exact);

    if (tmp_pair)
    {
        note_figure* tmp_figure = (*tmp_pair).value.the_figure;
        tmp_figure->add_to_slurer(my_slurer);
    }
}

figure* 
line_of_music::get_measure_end_figure(measure_feature::measure_feature_enum the_feature)
{
    figure_factory::FIGURE_ENUM tmp_enum;
    switch (the_feature)
    {
    case measure_feature::double_bar:
        tmp_enum = figure_factory::THIN_THICK_BAR;
    break;
    case measure_feature::repeat_last:
        tmp_enum = figure_factory::END_REPEAT;
    break;
    case measure_feature::repeat_next:
        tmp_enum = figure_factory::BEGIN_REPEAT;
    break;
    case measure_feature::repeat_last_and_next:
        tmp_enum = figure_factory::BEGIN_END_REPEAT;
    break;
    default:
    case measure_feature::single_bar:
        tmp_enum = figure_factory::SINGLE_BAR;
    break;

    }
    figure* tmp_figure = figure_factory::get_instance().get_figure(
        my_window.pixels_per_inch(),tmp_enum,my_scale_factor);

    return tmp_figure;

}

fraction 
line_of_music::get_time_signature(int the_measure)
{
    voice_info tmp_info = get_voice_info(the_measure);
    return tmp_info.my_time_signature;
}

void 
line_of_music::add_measure(measure_feature::measure_feature_enum the_end_break,
                   measure_feature::measure_feature_enum the_begin_break,
                   const  score_point& the_point,
                   const voice_info& the_info)
{
    figure* tmp_figure = 
        get_measure_end_figure(the_end_break);

    my_barlines.add_pair(the_point.measure,tmp_figure);
    tmp_figure->add_ref();

    // If there's a key change, add a new key signature to the list
    voice_info tmp_voice_info = get_voice_info(my_end_measure);

    my_voice_info_map.add_pair(the_point.measure,the_info);

	// If there's a begin feature, its really associated with
	// the first figure in the last line, so draw it before we
	// draw anything else
    if (the_begin_break != measure_feature::none)
    {
        tmp_figure = get_measure_end_figure(the_begin_break);
		tmp_figure->add_ref();
		my_begin_barline = tmp_figure;
    }

    if ((the_info.my_key != tmp_voice_info.my_key) ||
		(the_point.measure == my_start_measure))
    {
        my_key_sigs.add_pair(the_point.measure,
                             new key_sig(the_info.my_key,
                                         // open issue, what if clef and key
                                         // change mid-line ???
                                 get_voice_info(my_start_measure).my_clef,
                                 my_scale_factor,
                               tmp_voice_info.my_key));
    }
    if ((the_info.my_time_signature != tmp_voice_info.my_time_signature) ||
		(the_point.measure == my_start_measure))
    {
        my_time_sigs.add_pair(the_point.measure,
            new time_sig(my_scale_factor,
                         the_info.my_time_signature,2,
                         the_info.my_common_or_cut_time));
    }
	my_end_measure = the_point.measure;
}

void 
line_of_music::add_nth_ending(int the_start_measure,int the_end_measure,int the_ending)
{
    nth_ending_info tmp_info;
    tmp_info.my_end_measure = the_end_measure;
    tmp_info.my_number_ending = the_ending;
    tmp_info.my_start_measure = the_start_measure;
    tmp_info.my_bracket_figure = 0;
    tmp_info.my_number_figure = 0;
    my_nth_ending_map.add_pair(the_start_measure,tmp_info);
}

void 
line_of_music::render_nth_endings()
{
    int i;
    for (i = my_start_measure;i <= my_end_measure; ++i)
    {
        map<int,line_of_music::nth_ending_info>::iterator tmp_it = 
            my_nth_ending_map.get_item(i);
        if (tmp_it)
        {
            int tmp_start = (*tmp_it).key;
            nth_ending_info tmp_info = (*tmp_it).value;

            text_figure* tmp_text = new text_figure(font::serifs,
                                                       5 * (int)(my_scale_factor.y * 2.0),
                                                    my_window);
            tmp_text->add_ref();
            
            string tmp_num_string = (string)(char)('0' + (char)tmp_info.my_number_ending);
            rect tmp_rect = (*tmp_text).get_rect(tmp_num_string);
            int tmp_number_height = tmp_rect.height();
            int tmp_number_width = tmp_rect.width();
            tmp_info.my_number_figure = tmp_text;

            // Only draw the ending to the end of the staff, even if the
            // ending goes to the next line.
            int j;
            int tmp_pixel_total = 0;
            if (tmp_info.my_end_measure > my_end_measure)
            {
                tmp_info.my_end_measure = my_end_measure;
            }

            // Figure out how wide this things supposed to be by tallying
            // all the measures widths it is to cover.
            for (j = tmp_start;j <= tmp_info.my_end_measure;++j)
            {
                tmp_pixel_total += get_bar_width(j);
            }
            
            // And scale so it covers all that.
            size tmp_bracket_size;
            tmp_bracket_size.x = tmp_pixel_total;

            // The left hand side of this figure is at the front of the staff if 
            // this is the first measure, else its at the n-1th bar point.
            int tmp_bar_x;
            if (tmp_info.my_start_measure == my_start_measure)
            {
                tmp_bar_x = my_true_origin.x + my_staff->get_width_in_pixels() -
                    my_pixels_for_notes;
            }
            else
            {
                tmp_bar_x = get_bar_point(tmp_info.my_start_measure - 1);
            }
            
            // Now figure out the height.  Start at the top of the staff.
			int tmp_staff_height = my_staff->get_height_in_pixels();
            point tmp_bar_point = point(tmp_bar_x,
                                        my_origin.y - tmp_staff_height - tmp_number_height);
            // Compensate for if the first note in the staff collides with the little number.
            int tmp_highest_thing = get_height_of_first_note(tmp_info.my_start_measure);
            if (tmp_highest_thing - tmp_number_height < tmp_bar_point.y)
            {
                tmp_bar_point.y = tmp_highest_thing - tmp_number_height;
            }

            tmp_info.my_text_point = tmp_bar_point;
            tmp_info.my_bracket_point.x = tmp_bar_point.x - tmp_number_width;

            // Now scale the y part so that it doesn't bang into any of the high notes.
            tmp_info.my_bracket_point.y = my_origin.y;
            tmp_bracket_size.y = (my_origin.y - tmp_staff_height) - my_low_point;
            if (tmp_bracket_size.y < (3 * tmp_number_height) / 2)
            {
                tmp_bracket_size.y = (3 * tmp_number_height) / 2;
            }

            // Now we know how big the figure should be and where it goes.
            tmp_info.my_bracket_figure = new nth_ending(
                my_window.pixels_per_inch(),
                my_scale_factor,
                tmp_bracket_size);
            tmp_info.my_bracket_figure->add_ref();
            add_figure(tmp_info.my_bracket_point,
                       *tmp_info.my_bracket_figure);
            rect tmp_box = tmp_info.my_bracket_figure->get_bounding_box(tmp_info.my_bracket_point);

            update_high_low(tmp_box.lr.y,tmp_box.ul.y);
            add_text_figure(tmp_info.my_text_point,
                                    *tmp_info.my_number_figure);
            my_nth_ending_map.add_pair(tmp_start,tmp_info);
        }
    }
}

int
line_of_music::count_measure_pixels()
{
    int tmp_rv = 0;
    int i;
    for (i = my_start_measure;i <= my_end_measure;++i)
    {
        int tmp_count = get_barline_width(i);
        tmp_rv += tmp_count;
    }

    // We've already counted the start key pixels, now count any
    // inline key sig changes.
    map<int,key_sig*>::iterator tmp_it = 
        my_key_sigs.least();
    if (tmp_it)
    {
        tmp_it.get_item((*tmp_it).key,gt);
    }
    while (tmp_it)
    {
        tmp_rv += ((*tmp_it).value)->get_width(my_window.pixels_per_inch(),true);
    }

    return tmp_rv;
}

int 
line_of_music::get_height_of_first_note(int the_measure)
{
    staff_point tmp_point(the_measure,fraction(0));
    map<staff_point,point_note_pair>::iterator tmp_it = 
        my_music.get_item(tmp_point,gteq);

    if (tmp_it)
    {
        point_note_pair tmp_pair = (*tmp_it).value;
        return tmp_pair.the_figure->get_low_point();
    }

    return my_origin.y;
}

int 
line_of_music::get_barline_width(int the_measure)
{
    map<int,figure*>::iterator tmp_pair = my_barlines.get_item(the_measure);
    if (tmp_pair)
    {
        figure* tmp_figure = ((*tmp_pair).value);
		return tmp_figure->get_width_in_pixels();
    }

	return 0;
}

int 
line_of_music::get_bar_point(int the_measure)
{
    int tmp_rv = 0;
    map<int,int>::iterator tmp_point = my_bar_points.get_item(the_measure);
    if (tmp_point)
    {
        tmp_rv = (*tmp_point).value;
    }

	return tmp_rv;
}

int 
line_of_music::get_bar_width(int the_measure)
{
    int tmp_rv = 0;
    map<int,int>::iterator tmp_point = my_bar_points.get_item(the_measure);
    if (tmp_point)
    {
        int tmp_start,tmp_end;
		tmp_start = tmp_end = (*tmp_point).value;
        if (the_measure > my_start_measure)
        {
            tmp_point = my_bar_points.get_item(the_measure - 1);
            // If this is not the first measure, the measure starts where the next
            // measure ends.
            if (tmp_point)
            {
                tmp_start = (*tmp_point).value;
            }
            else
            {
                throw("Can't find bar for measure");
            }
        }
        // If this is the first measure, the measure starts at the end of the line
		// fixed stuff.
        else
        {
            tmp_start = my_staff->get_width_in_pixels() - my_pixels_for_notes;
        }

        tmp_rv = tmp_end - tmp_start;
    }

    return tmp_rv;
}

void 
line_of_music::render_bar_at(int the_measure,key_sig* the_key_figure,time_sig* the_time_figure)
{
    map<int,figure*>::iterator tmp_pair = my_barlines.get_item(the_measure);
    map<int,int>::iterator tmp_point = my_bar_points.get_item(the_measure);
    if ((tmp_pair) && (tmp_point))
    {
        point tmp_start_point = my_origin;
        figure* tmp_figure = (figure*)((*tmp_pair).value);
        tmp_start_point.x = (*tmp_point).value;
        
        // The last measure we align with the end of the bar, else we align it with the 
        // figures around it.
        if (the_measure == my_end_measure )
        {
            tmp_start_point.x -= (*tmp_figure).get_width_in_pixels();

        }
        else
        {
    	    // We want the middle of the bar figure to show up at my_origin.x,,
            // so center the figure around that point.
            //
            //          |     | |    |
            //          |   . | |    |
            //          |   . | |    |
            //         o            o
            //                ^^^
            //             my_origin.x
            //              ^^^
            //        my_origin.x - width/2
            tmp_start_point.x -= (*tmp_figure).get_width_in_pixels() / 2;
            if (the_time_figure)
            {
                tmp_start_point.x -= the_time_figure->get_width(my_window) / 2;
                tmp_start_point.x += the_time_figure->get_x_offset(my_window);
            }
            if (the_key_figure)
            {
                tmp_start_point.x -= the_key_figure->get_width(my_window.pixels_per_inch(),true) / 2;
                tmp_start_point.x += the_key_figure->get_x_offset(my_window.pixels_per_inch());
            }
        }

        tmp_start_point.x -= (*tmp_figure).get_size_in_pixels().my_loc.x;
        point tmp_bar_point = tmp_start_point;
        tmp_bar_point.y -= (*tmp_figure).get_size_in_pixels().my_loc.y;
        add_figure(tmp_bar_point,*tmp_figure);

        if (the_time_figure)
        {
            staff_info tmp_info = *this;
            tmp_info.horizontal_position = tmp_start_point.x + 
                (*tmp_figure).get_width_in_pixels();
            figure_container<line_of_music> tmp_page(*this);
            the_time_figure->add_to_page(tmp_page,my_window,tmp_info);
            tmp_start_point.x += the_time_figure->get_width(my_window);
            tmp_start_point.x -= the_time_figure->get_x_offset(my_window);
        }
        if (the_key_figure)
        {
            staff_info tmp_info = *this;
            tmp_info.horizontal_position = tmp_start_point.x + (*tmp_figure).get_width_in_pixels();
            figure_container<line_of_music> tmp_page(*this);
            the_key_figure->draw_self(tmp_page,my_window,tmp_info,true);
        }
    }
}

void 
line_of_music::add_crescendo(const staff_point& the_start_point,
                    chord_info::dynamic the_start_dynamic,
                    const staff_point& the_stop_point,
                    chord_info::dynamic the_stop_dynamic)
{
    dynamic_marking tmp_marking;
    tmp_marking.the_start_dynamic = the_start_dynamic;
    tmp_marking.the_start_point = the_start_point;
    tmp_marking.the_stop_dynamic = the_stop_dynamic;
    tmp_marking.the_stop_point = the_stop_point;
    my_dynamics_info.add_pair(tmp_marking.the_start_point,tmp_marking);
}

void 
line_of_music::add_dynamic(const staff_point& the_point,chord_info::dynamic the_dynamic)
{
    my_dynamic_changes.add_pair(the_point,the_dynamic);
}


void 
line_of_music::add_note_figure(note_figure& the_figure,const staff_point& the_beat)
{
    // keep track of the lowest point in the music.
    score_point tmp_score_point(the_beat.measure,the_beat.beat,my_voice_index);
    if ((tmp_score_point < my_lowest_score_point) ||
        (my_first_notes_added == false))
    {
        my_lowest_score_point = tmp_score_point;
        my_first_notes_added = true;
    }
	fraction tmp_duration = the_figure.get_duration();
    chord_info tmp_chord = the_figure.get_note();
    this->my_beats += tmp_duration * get_time_signature(my_end_measure).den;
    the_figure.rescale(my_scale_factor);
    point_note_pair tmp_pair;
    tmp_pair.the_figure = &the_figure;
    my_music.add_pair(the_beat,tmp_pair);

    // If this note is starting a new beam group, create a new one
    if (tmp_chord.is_grace() == false)
    {
        my_last_grace_beamer = 0;
        if (tmp_chord.is_first_in_beam())
        {
            my_last_beamer = new beamer(my_window.pixels_per_inch(),*this,
				false);
            my_last_beamer->rescale(my_scale_factor);
        }
    	if (my_last_beamer != 0)
    		the_figure.set_beamer(my_last_beamer);

    }
    else
    {
        if ((the_figure.get_note().is_first_in_beam()) ||
            (my_last_grace_beamer == 0))
        {
            my_last_grace_beamer = new beamer(my_window.pixels_per_inch(),*this,
				true);
            // Force the beam in the opposite of the expected direction.  This may
            // cause the beams of the note and the grace note not to crash into 
            // each other if they overlap, and will call attention to the note in 
            // any case
            if (my_last_beamer)
            {
                int tmp_direction = 1;
                if (the_figure.get_note()[0].get_pitch() > (*this).get_center_pitch())
                {
                    tmp_direction = -1;
                }
                my_last_grace_beamer->force_direction(tmp_direction);
                my_last_beamer->force_direction(tmp_direction * -1);
            }
        }
        if (my_last_grace_beamer != 0)
        {
            the_figure.set_beamer(my_last_grace_beamer);
        }
    }
    
    // If this is a grace note make it small
    if (the_figure.get_note().is_grace() == true)
    {
        the_figure.rescale(scale(0.75,0.75) * my_scale_factor * my_note_scale);
    }
    else
    {
        the_figure.rescale(my_scale_factor * my_note_scale);
    }
}

int
line_of_music::get_pixels_from_duration(const fraction& the_duration,int the_measure)
{
    // This is how many beats are represented in this staff
    int tmp_beats = my_beats.as_int();

	// If we don't have any complete measures, don't try and
	// justify.
	if (tmp_beats == 0)
		return 0;

    // Figure out how wide the note-drawing part of the staff is
    // note to self:  need to add the width of time sig in here also.
    int tmp_pixels = my_pixels_for_notes;
    double tmp_pixels_per_beat = (double)tmp_pixels / (double)tmp_beats;
    
    double tmp_note_duration = the_duration.as_double();

    fraction tmp_sig = get_time_signature(the_measure);
	double tmp_beat_duration = fraction(1,tmp_sig.den).as_double();
	tmp_note_duration /= tmp_beat_duration;

    tmp_pixels = (int)(tmp_note_duration * tmp_pixels_per_beat);

    return tmp_pixels;
}

int 
line_of_music::get_note_pixels(note_figure& the_figure,int the_measure)
{
    int tmp_pixels = 0;

    // We use absolute justification in score mode so things line up.  Use it too if
    // there is only one measure, or the calculations don't really work.
    if ((iabc::score_mode_on == true) ||
        (my_end_measure - my_start_measure < 2))
    {        
        tmp_pixels = get_pixels_from_duration(the_figure.get_duration(),the_measure);
    }
    else
    {
        int tmp_figure_width = the_figure.get_width();
        int tmp_word_width = get_width_of_words(the_figure.get_note());
        if (tmp_word_width > tmp_figure_width)
        {
            tmp_figure_width = tmp_word_width;
        }
    
        if ((my_should_center == false) ||
            (the_figure.get_note().is_grace() == true))
        {
            return tmp_figure_width + (tmp_figure_width / 2);
        }
    
        tmp_pixels = get_pixels_from_duration(the_figure.get_duration(),the_measure);
        int tmp_spare = tmp_pixels - tmp_figure_width;
    
        // We only justify things by moving them right.  Otherwise we'd be
        // squooshing the notes together.  
        if (tmp_pixels < tmp_figure_width)
        {
            tmp_pixels = tmp_figure_width;
        }
        // If there is extra space here, and we are worried about plopping off the end,
        // make that adjustment here.
        else if ((my_rhs_overlap > 0) &&
                 (my_spare_pixels > 0) &&
                 (tmp_spare > 0))
        {
            double tmp_less_percent = (double)my_rhs_overlap / (double)my_spare_pixels;
    		int tmp_burrow = (int)((tmp_less_percent * (double)tmp_spare) + 1.0);
            tmp_pixels -= tmp_burrow;
        }
    }

    return tmp_pixels;
}

pitch 
line_of_music::get_top_note()
{
    pitch tmp_rv = get_center_pitch();
    pitch tmp_key = get_voice_info(my_start_measure).my_key;
    tmp_rv = tmp_rv.up_letter(tmp_key);
    tmp_rv = tmp_rv.up_letter(tmp_key);
    tmp_rv = tmp_rv.up_letter(tmp_key);
    tmp_rv = tmp_rv.up_letter(tmp_key);

    return tmp_rv;
}

pitch 
line_of_music::get_bottom_note()
{
    pitch tmp_rv = get_center_pitch();
    pitch tmp_key = get_voice_info(my_start_measure).my_key;
    tmp_rv = tmp_rv.down_letter(tmp_key);
    tmp_rv = tmp_rv.down_letter(tmp_key);
    tmp_rv = tmp_rv.down_letter(tmp_key);
    tmp_rv = tmp_rv.down_letter(tmp_key);

    return tmp_rv;
}

void 
line_of_music::draw_ledger_lines(note_figure& the_figure)
{
    // First get the pitch corresponding to the top and bottom
    // note of the staff.
    pitch tmp_center = this->get_center_pitch();
    pitch tmp_top_note = get_top_note();
    pitch tmp_bottom_note = get_bottom_note();
    for (int i = 0;i < the_figure.get_note().get_size();++i)
    {
        // Don't draw ledger lines on rests.
        if (the_figure.get_note()[i].is_rest() == false)
        {
            pitch tmp_pitch = the_figure.get_note()[i].get_pitch();
    
    		// For the ledger lines we only care about the letter note;
    		// this makes it a little simpler
    		tmp_pitch = pitch(tmp_pitch.get_letter(),
    			pitch::natural,tmp_pitch.get_octave());
            pitch tmp_key = get_voice_info(my_start_measure).my_key;
            tmp_top_note = tmp_top_note.up_letter(tmp_key);
            tmp_top_note = tmp_top_note.up_letter(tmp_key);
            tmp_bottom_note = tmp_bottom_note.down_letter(tmp_key);
            tmp_bottom_note = tmp_bottom_note.down_letter(tmp_key);
            // Keep drawing ledger lines until we're at the level of the
            // note.  This one's for the up lines
            while (tmp_pitch >= tmp_top_note)
            {
                figure* tmp_line = figure_factory::get_instance().get_figure(
                    my_window.pixels_per_inch(),figure_factory::LEDGER_LINE,my_scale_factor);
                point tmp_point =
                    place_note(my_window,note_info(tmp_top_note,fraction()),*this);
                tmp_point.x += the_figure.get_head_offset();
                add_figure(tmp_point,*tmp_line);
                tmp_top_note = tmp_top_note.up_letter(tmp_key);
                tmp_top_note = tmp_top_note.up_letter(tmp_key);
            }
            // This one's for the down lines.
            while (tmp_pitch <= tmp_bottom_note)
            {
                figure* tmp_line = figure_factory::get_instance().get_figure(
                    my_window.pixels_per_inch(),figure_factory::LEDGER_LINE,my_scale_factor);
                point tmp_point =
                    place_note(my_window,note_info(tmp_bottom_note,fraction()),*this);
                tmp_point.x += the_figure.get_head_offset();
                add_figure(tmp_point,*tmp_line);
                tmp_bottom_note = tmp_bottom_note.down_letter(tmp_key);
                tmp_bottom_note = tmp_bottom_note.down_letter(tmp_key);
            }
        }
    }
}

void
line_of_music::draw_little_numbers()
{
    text_figure* tmp_figure1 = new text_figure(font::serifs,
                                               4 * (int)(my_scale_factor.y * 2.0),
                                               my_window);
    tmp_figure1->add_ref();
    string tmp_num_string = string((long)my_start_measure);
    point tmp_origin = my_origin;
    tmp_origin.y -= tmp_figure1->get_rect(tmp_num_string).height() * 2;
    tmp_figure1->set_string(tmp_num_string);
    add_text_figure(tmp_origin,*tmp_figure1);
    tmp_figure1->remove_ref();
	update_high_low(tmp_origin.y,tmp_origin.y);
}

void
line_of_music::update_high_low(int the_high,int the_low)
{
    if (the_low <= 0)
    {
        throw("THat's too low");
    }
    if (my_high_point < the_high)
    {
        my_high_point = the_high;
    }
    if (my_low_point > the_low)
    {
        my_low_point = the_low;
    }
}

int
line_of_music::get_width_of_words(const chord_info& the_chord)
{
    array<string> the_words = the_chord.get_words();
    // Don't bother if there are no words
    if (the_words.get_size() == 0)
        return 0;
    text_figure* tmp_text = new text_figure(font::sans_serifs,
                                             5 * (int)(my_scale_factor.y * 2.0) - 1,
                                              my_window,font::normal);
    tmp_text->add_ref();

    int i;
    int tmp_rv = 0;
    for (i = 0;i < the_words.get_size();++i)
    {
        string tmp_string = the_words[i];
        int tmp_width = tmp_text->get_rect(tmp_string).width();

        if (tmp_width > tmp_rv)
        {
            tmp_rv = tmp_width;
        }
    }

    tmp_text->remove_ref();
    return tmp_rv;
}

void 
line_of_music::get_leftover_pixels()
{
    double tmp_duration_total = 0.0;

    staff_point tmp_key(my_start_measure,fraction());
    map<staff_point,point_note_pair>::iterator tmp_pair = 
        my_music.get_item(tmp_key,gteq);
    note_figure* tmp_figure = (*tmp_pair).value.the_figure;
    
    int tmp_total = 0;
    int tmp_last_width = 0;
    int tmp_measure = my_start_measure;
    int tmp_last_measure = tmp_measure;
    while (tmp_pair)
    {
        int tmp_current_measure = (*tmp_pair).key.measure;
        note_figure* tmp_figure = (*tmp_pair).value.the_figure;

        // We need to do this here so that the note can use the accidental width
        // when calculating the width of all the notes.  The accidental map is contained
        // by this class but the note_figure class uses it to make sure it doesn't print
        // accidentals in the wrong place.  For instance we don't want to print an 
        // accidental twice in the same measure.
        tmp_figure->set_accidentals(my_accidentals[tmp_current_measure - my_start_measure]);

        tmp_duration_total += tmp_figure->get_note().get_duration().as_double();

        int tmp_figure_width = tmp_figure->get_width();
        int tmp_bar_figure_width = get_barline_width(tmp_last_measure);
        int tmp_word_width = get_width_of_words(tmp_figure->get_note());
        int tmp_duration_pixels = 
			get_pixels_from_duration(
			tmp_figure->get_duration(),
			(*tmp_pair).key.measure);
        tmp_last_width = tmp_duration_pixels;

        if (tmp_word_width > tmp_figure_width)
        {
            tmp_figure_width = tmp_word_width;
        }

        // Otherwise, make note of the leftover stuff so we can justify later
        if (tmp_figure_width < tmp_duration_pixels)
        {
            my_spare_pixels += (tmp_duration_pixels - tmp_figure_width);
        }

        // Increase the total by the max of either duration or note pixels
        if (tmp_duration_pixels > tmp_figure_width)
        {
            tmp_figure_width = tmp_duration_pixels;
        }

        tmp_total += tmp_figure_width;

        if (tmp_measure != tmp_last_measure)
        {
            tmp_last_measure = tmp_measure;
            tmp_total += tmp_bar_figure_width;
        }
        tmp_key = (*tmp_pair).key;
        tmp_pair = my_music.get_item(tmp_key,gt);
        if (tmp_pair) tmp_measure = (*tmp_pair).key.measure;
    }

    // We need enough room for the last note, so make sure we are ending
    // that many pixels from the end.
    tmp_total += tmp_last_width;

    if (tmp_total > my_pixels_for_notes)
    {
        my_rhs_overlap = tmp_total - my_pixels_for_notes;
    }
}

void 
line_of_music::create_chord_symbol(note_figure& the_note,const staff_point& the_point)
{
    chord_info tmp_info = the_note.get_note();
    string tmp_string = tmp_info.get_description();
    if (tmp_string.length() > 0)
    {
		text_figure::point_text_pair tmp_chord;
        font::style tmp_font_style = font::normal;
        if (window_media_chord_bold)
        {
            tmp_font_style = (font::style)((int) tmp_font_style | (int)font::bold);
        }
        if (window_media_chord_italic)
        {
            tmp_font_style = (font::style)((int) tmp_font_style | (int)font::italic);
        }
        text_figure* tmp_text_figure = new text_figure(
            font::serifs,
                    (int)(my_scale_factor.y * (double)window_media_chord_size),
                    my_window,tmp_font_style);
        tmp_text_figure->add_ref();
        tmp_text_figure->set_string(tmp_string);

        tmp_chord.my_figure = tmp_text_figure;
        tmp_chord.my_point = my_origin;
        
        rect tmp_rect = tmp_text_figure->get_current_rect();
        tmp_chord.my_point.x -= tmp_rect.width()/2;

        note_text_point tmp_saved_symbol;
        tmp_saved_symbol.my_text_figure = tmp_text_figure;
        tmp_saved_symbol.my_note_figure = &the_note;
        tmp_saved_symbol.my_point = tmp_chord.my_point;
        my_chord_symbols.add_to_end(tmp_saved_symbol);
    }
}

void 
line_of_music::render_part()
{
    text_figure* tmp_figure1 = 0;
    figure* tmp_box = 0;
    point tmp_origin(my_clef_x,my_true_origin.y);

    // If there is anything in this staff.
    if (my_voice_info_map.get_size() > 0)
    {
        voice_info& tmp_info = get_voice_info(my_start_measure);

        // If the voice is defined for this staff.
        if (tmp_info.my_part.length() > 0)
        {
            tmp_figure1 = new text_figure(font::serifs,
                                                       4 * (int)(my_scale_factor.y * 2.0),
                                                       my_window);
            tmp_figure1->set_string(tmp_info.my_part);
            rect tmp_rect = tmp_figure1->get_current_rect();
            tmp_box = new box_figure(my_window.pixels_per_inch(),
                size(tmp_rect.width(),tmp_rect.height()));
        }
    }

    if ((tmp_figure1) && (tmp_box))
    {
        rect figure_rect = tmp_figure1->get_current_rect();
        figure_rect = figure_rect.offset(point(my_clef_x,tmp_origin.y));
        while (does_rect_overlap(figure_rect))
        {
            figure_rect = figure_rect.offset(point(0,-1));
        }
        add_text_figure(figure_rect.ul,*tmp_figure1);
        add_figure(point(figure_rect.ul.x,figure_rect.lr.y),*tmp_box);
    }
}

void 
line_of_music::render_voice_info()
{
    text_figure* tmp_figure1 = 0;
    point tmp_origin(my_clef_x,my_true_origin.y);
    string tmp_num_string;

    // If there is anything in this staff.
    if (my_voice_info_map.get_size() > 0)
    {
        voice_info& tmp_info = get_voice_info(my_start_measure);

        // If the voice is defined for this staff.
        if ((tmp_info.my_name.length() > 0) &&
            (my_start_measure == 1))
        {
            tmp_figure1 = new text_figure(font::serifs,
                                                       4 * (int)(my_scale_factor.y * 2.0),
                                                       my_window);
            tmp_num_string = tmp_info.my_name;
        }
        else if (tmp_info.my_short_name.length() > 0)
        {
            tmp_figure1 = new text_figure(font::serifs,
                                                       4 * (int)(my_scale_factor.y * 2.0),
                                                       my_window);
            tmp_num_string = tmp_info.my_short_name;
        }
    }

    // We use the same font to draw the voice names as the little numbers, so
    // we just draw the voice info above the numbers.
    if (tmp_figure1)
    {
        tmp_figure1->set_string(tmp_num_string);
        rect figure_rect = tmp_figure1->get_current_rect();
        figure_rect = figure_rect.offset(point(my_clef_x,tmp_origin.y));
        while (does_rect_overlap(figure_rect))
        {
            figure_rect = figure_rect.offset(point(0,-1));
        }
        add_text_figure(figure_rect.ul,*tmp_figure1);
    }
}

void
line_of_music::render_chords()
{
    int i;
    for (i = 0;i < my_chord_symbols.get_size();++i)
        {
        text_figure* tmp_text_figure = my_chord_symbols[i].my_text_figure;
        note_figure* tmp_note_figure = my_chord_symbols[i].my_note_figure;

        // The assumption is that this point contains the y coordinate of the
        // origin and the correct x coordinate.
        point tmp_point = my_chord_symbols[i].my_point;
        int tmp_lp = tmp_note_figure->get_low_point();
        rect tmp_rect = tmp_text_figure->get_current_rect();
        tmp_point.y -= tmp_rect.height(); 

		if (tmp_point.y + 
			tmp_rect.height() + 
			tmp_text_figure->get_current_descent() > tmp_lp)
        {
			tmp_point.y  = tmp_lp - 
				(tmp_rect.height() + tmp_text_figure->get_current_descent());
        }
        update_high_low(tmp_point.y + tmp_rect.height(),tmp_point.y);
        add_text_figure(tmp_point,*tmp_text_figure);
        tmp_text_figure->remove_ref();
        }
}

bool 
line_of_music::does_rect_overlap(const rect& the_rect)
{
    bool tmp_rv = false;

    // tmp_right is the end of the rectangle, we can stop
    // looking when we pass there
    int tmp_right = the_rect.right();

    map<int,rectangle_array>::iterator tmp_it = 
        my_rect_map.least();
    while ((tmp_it) && (tmp_rv == false))
    {
        rectangle_array& tmp_array = (*tmp_it).value;
        int tmp_next_key = (*tmp_it).key;
        if (tmp_next_key > tmp_right)
        {
            break;
        }
        int i;
        for (i = 0;i < tmp_array.get_size();++i)
        {
            rect tmp_rect = tmp_array[i];
            tmp_rect = tmp_rect.offset(point(0,my_y_offset));
            if (tmp_rect.intersects(the_rect))
            {
                tmp_rv = true;
                break;
            }
        }

        tmp_it = tmp_it.get_item(tmp_next_key,gt);
    }

    return tmp_rv;
}

void
line_of_music::render_figures()
{
    staff_point tmp_key(my_start_measure,fraction());
    map<staff_point,point_note_pair>::iterator tmp_pair = 
        my_music.get_item(tmp_key,gteq);
    int tmp_old_measure = tmp_key.measure;
    
    // Scan through the figures and figure out how we're going to justify
    get_leftover_pixels();

    int tmp_borrowed = 0;

    // Save this for centering the bar lines.
    int tmp_last_bar_x = my_origin.x;
    // If we have inline key changes, keep track of them
    while (tmp_pair)
    {
        // If we are in a new measure from the last thing we
		// drew, make note that we need to draw the bar line.  We won't
        // actually know where to draw it until we've drawn this figure.
        int tmp_current_measure = (*tmp_pair).key.measure;
        if (tmp_current_measure != tmp_old_measure)
        {
            my_origin.x += this->get_barline_width(tmp_old_measure);
        }

        // Likewise we compensate for inline key signature changes, which
        // occur at measure boundaries.
        voice_info tmp_old_info = get_voice_info(tmp_old_measure);
        voice_info tmp_new_info = get_voice_info(tmp_current_measure);
        if (tmp_old_info.my_key != tmp_new_info.my_key)
        {
			key_sig* tmp_sig = get_map_value<int,map<int,key_sig*>,key_sig*>(tmp_current_measure,my_key_sigs);
            my_origin.x += tmp_sig->get_width(my_window.pixels_per_inch());
            my_origin.x -= tmp_sig->get_x_offset(my_window.pixels_per_inch());
        }
        if (tmp_old_info.my_time_signature != tmp_new_info.my_time_signature)
        {
			time_sig* tmp_sig = get_map_value<int,map<int,time_sig*>,time_sig*>(tmp_current_measure,my_time_sigs);
            my_origin.x += tmp_sig->get_width(my_window);
            my_origin.x -= tmp_sig->get_x_offset(my_window);
        }
		

        note_figure* tmp_figure = (*tmp_pair).value.the_figure;

        // This is the width of the pixels
        int tmp_figure_width = tmp_figure->get_width();
        
        // This is the width of the word below it
        int tmp_word_width = get_width_of_words(tmp_figure->get_note());
        
        // This is the space the note must take up, according to the
        // current justification logic.
        int tmp_justify = get_note_pixels(*tmp_figure,tmp_old_measure);

        int tmp_duration_pixels = 
			get_pixels_from_duration(tmp_figure->get_duration(),tmp_old_measure);
        if (tmp_duration_pixels > tmp_justify)
        {
            tmp_borrowed += tmp_duration_pixels - tmp_justify;
        }

        // w1 and w2 and tmp_justify represent the width of the note, the width
        // we have to draw on, and the offset from the current location, respectively
        // like so.
        //        |
        //        |
        //       O
        //       ^^
        //       w1
        //  ^^^^^^^^^^^^
        //   tmp_justify
        //  ^^^     ^^^
        //    w2     
        // Try to center this figure in the room we have to work with
        int w1_note = tmp_figure_width;
        int w1_words = tmp_word_width;
        int w2_note = (tmp_justify - w1_note) / 2;
        int w2_words = (tmp_justify - w1_note) / 2;
        
        point_note_pair tmp_new_pair;
        tmp_new_pair.the_note_point = point(my_origin.x + w2_note,my_origin.y);
        tmp_new_pair.the_figure = tmp_figure;
        tmp_new_pair.the_word_x = my_origin.x + w2_words;

        // Finally we get to draw the notes.  We defer the words since we don't yet
        // know what the correct y coordinate is.
        my_origin.x += w2_note;
		figure_container<line_of_music> tmp_page(*this);
        tmp_figure->render(tmp_page,*this);
                           
        draw_ledger_lines(*tmp_figure);

        // See if we need to draw a new measure
        if (tmp_current_measure != tmp_old_measure)
        {
            int tmp_left = tmp_figure->get_absolute_left();

            // Handle inline key changes and time changes
            key_sig* tmp_key_sig = 0;
            time_sig* tmp_time_sig = 0;
            if (tmp_old_info.my_key != tmp_new_info.my_key)
            {
                tmp_key_sig = get_map_value<int,map<int,key_sig*>,key_sig*>(tmp_current_measure,my_key_sigs);
            }
            if (tmp_old_info.my_time_signature != tmp_new_info.my_time_signature)
            {
                tmp_time_sig = get_map_value<int,map<int,time_sig*>,time_sig*>(tmp_current_measure,my_time_sigs);
            }
            tmp_last_bar_x = (tmp_last_bar_x + tmp_left) / 2;
            my_bar_points.add_pair(tmp_old_measure,
                tmp_last_bar_x);
            render_bar_at(tmp_old_measure,tmp_key_sig,tmp_time_sig);
            
            tmp_old_measure = (*tmp_pair).key.measure;
        }

        // Since we know where the note goes, place the chord
        // symbols above it now.
        create_chord_symbol(*tmp_figure,(*tmp_pair).key);

        // Keep track of the rightmost point of this figure, in case its
        // the last figure in a bar.
        tmp_last_bar_x = tmp_figure->get_absolute_right();

        // Move to the place where the next note will start.
        my_origin.x += w2_note + w1_note;
        
        // Perform some bookkeeping for vertical justification.
        update_high_low(tmp_figure->get_high_point(),
                       tmp_figure->get_low_point());

        tmp_key = (*tmp_pair).key;
        my_music.add_pair(tmp_key,tmp_new_pair);

        tmp_pair = my_music.get_item(tmp_key,gt);
    }

    // Now draw the last bar, after the last note. We always draw the last
    // bar on the end, no matter what.
    my_bar_points.add_pair(tmp_old_measure,
        my_true_origin.x + my_staff->get_width_in_pixels());
    render_bar_at(tmp_old_measure,0,0);
}

void
line_of_music::render_dynamics()
{
    map<staff_point,chord_info::dynamic>::iterator tmp_dynamic_it =
        my_dynamic_changes.least();
    int tmp_local_high = my_low_point;
    
    // First do dynamic markings like p, pp, etc.
    while (tmp_dynamic_it)
    {
        staff_point tmp_key = (*tmp_dynamic_it).key;        
        map<staff_point,point_note_pair>::iterator tmp_pair = 
            my_music.get_item(tmp_key,gteq);
        if (tmp_pair)
        {
            note_figure* tmp_figure = (*tmp_pair).value.the_figure;
            int tmp_x = (*tmp_pair).value.the_word_x;
            point tmp_point = point(tmp_x,0);
            string tmp_dynamic_string = chord_info::get_dynamic_string((*tmp_dynamic_it).value);
            text_figure::point_text_array tmp_text_array;
        
            // Figure out where this word goes.  Directly below the staff, or
            // below the lowest thing there is.
            point tmp_old_point = tmp_point;
            tmp_point.y = get_vertical_position() + 
                (my_staff->get_size_in_pixels().my_size.y);
            if (tmp_point.y < my_high_point)
            {
                tmp_point.y = my_high_point;
            }
    
            int tmp_figure_width = tmp_figure->get_width();
            text_figure* tmp_text = new text_figure(font::serifs,
                                                     (int)(14 * my_scale_factor.y + 0.5),
													 my_window,(font::style)(font::italic | font::bold));
            tmp_text->add_ref();
            rect tmp_rect = tmp_text->get_rect(tmp_dynamic_string);
            if (tmp_figure_width < tmp_rect.width())
            {
                tmp_point.x -= (tmp_rect.width() - tmp_figure_width) / 2;
            }
            tmp_point.y += tmp_text->get_rect(tmp_dynamic_string).height();
    
            // Save this so we don't have to render it again.
            text_figure::point_text_pair tmp_new_pair;
            tmp_new_pair.my_figure = tmp_text;
            tmp_new_pair.my_point = tmp_point;
            tmp_text_array.add_to_end(tmp_new_pair);
    
            // Now draw it.
            tmp_text->set_string(tmp_dynamic_string);
            add_text_figure(tmp_point,*tmp_text);
    
            // Only update the hi-low one time, or we get
            // the staircase effect.
            if (tmp_point.y + tmp_text->get_current_rect().lr.y + tmp_text->get_current_descent() > tmp_local_high)
            {
                tmp_local_high = tmp_point.y + tmp_text->get_current_rect().lr.y + tmp_text->get_current_descent();
            }
            tmp_point = tmp_old_point;
            tmp_text->remove_ref();
        }
        tmp_dynamic_it = my_dynamic_changes.get_item(tmp_key,gt);
    }

    // Now do the cresc. decresc. markings
    map<staff_point,dynamic_marking>::iterator tmp_swell_it =
        my_dynamics_info.least();
    map<staff_point,point_note_pair>::iterator tmp_note_pair_start = 
        my_music.least();
    map<staff_point,point_note_pair>::iterator tmp_note_pair_end = 
        my_music.least();
    while (tmp_swell_it)
    {
        point tmp_start_point,tmp_end_point;
        staff_point tmp_score_point = (*tmp_swell_it).key;
        dynamic_marking tmp_marking = (*tmp_swell_it).value;
        tmp_note_pair_start = 
            tmp_note_pair_start.get_item(tmp_marking.the_start_point);
        tmp_note_pair_end = 
            my_music.get_item(tmp_marking.the_stop_point);
        if ((tmp_note_pair_start) && (tmp_note_pair_end))
        {
            point_note_pair tmp_start = (*tmp_note_pair_start).value;
            point_note_pair tmp_end = (*tmp_note_pair_end).value;
            tmp_start_point = tmp_start.the_note_point;
            tmp_start_point.y = my_high_point;
            tmp_end_point = tmp_end.the_note_point;
            int tmp_width = tmp_end.the_note_point.x - tmp_start.the_note_point.x;
            figure* tmp_figure;
            if (tmp_marking.the_start_dynamic == chord_info::start_cresc)
            {
                tmp_figure = 
                    new crescendo_figure(my_window.pixels_per_inch(),tmp_width);
            }
            else
            {
                tmp_figure = 
                    new decrescendo_figure(my_window.pixels_per_inch(),tmp_width);
            }
            tmp_figure->add_ref();
            tmp_figure->rescale(my_scale_factor);
            add_figure(tmp_start_point,*tmp_figure);
            // Only update the hi-low one time, or we get
            // the staircase effect.
            point tmp_low = tmp_figure->get_bottom_point();
            if (tmp_start_point.y + tmp_low.y > tmp_local_high)
            {
                tmp_local_high = tmp_start_point.y + tmp_low.y;
            }
            tmp_figure->remove_ref();
        }

        tmp_swell_it = tmp_swell_it.get_item(tmp_score_point,gt);
    }
    update_high_low(tmp_local_high,tmp_local_high);
}

void
line_of_music::render_ornaments()
{
    map<staff_point,point_note_pair>::iterator tmp_it = 
        my_music.least();
	figure_container<line_of_music> tmp_page(*this);
    staff_info tmp_info = *this;
    while (tmp_it)
    {
        note_figure* tmp_figure = (*tmp_it).value.the_figure;         
        tmp_info.horizontal_position = tmp_figure->get_origin().x;
        tmp_figure->handle_ornaments(tmp_page,tmp_info);
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }
}
void
line_of_music::render_words()
{
    staff_point tmp_key(my_start_measure,fraction());
    map<staff_point,point_note_pair>::iterator tmp_pair = 
        my_music.get_item(tmp_key,gteq);
    int tmp_local_high = my_low_point;
    
    while (tmp_pair)
    {
        staff_point tmp_next_key = (*tmp_pair).key;
        note_figure* tmp_figure = (*tmp_pair).value.the_figure;
        int tmp_x = (*tmp_pair).value.the_word_x;
        point tmp_point = point(tmp_x,0);
        const chord_info& tmp_info = tmp_figure->get_note();
        array<string> tmp_array = tmp_figure->get_note().get_words();
		text_figure::point_text_array tmp_text_array;
    
        // If there are several verses, there may be several lines of words.
        int i;
        for (i = 0;i < tmp_array.get_size();++i)
        {
            // Figure out where this word goes.  Directly below the staff, or
            // below the lowest thing there is.
            point tmp_old_point = tmp_point;
            tmp_point.y = this->get_vertical_position() + 
                (my_staff->get_size_in_pixels().my_size.y);
            if (tmp_point.y < my_high_point)
            {
                tmp_point.y = my_high_point;
            }

            int tmp_figure_width = tmp_figure->get_width();
            text_figure* tmp_text = new text_figure(font::sans_serifs,
                                                     (int)((double)window_media_word_size *
                                                            my_scale_factor.y + 0.5),
                                                      my_window,font::normal);
            tmp_text->add_ref();
            rect tmp_rect = tmp_text->get_rect(tmp_array[i]);
            if (tmp_figure_width < tmp_rect.width())
            {
                tmp_point.x -= (tmp_rect.width() - tmp_figure_width) / 2;
            }
            tmp_point.y += i * tmp_text->get_rect(tmp_array[i]).height();
            
            // Save this so we don't have to render it again.
			text_figure::point_text_pair tmp_new_pair;
            tmp_new_pair.my_figure = tmp_text;
            tmp_new_pair.my_point = tmp_point;
            tmp_text_array.add_to_end(tmp_new_pair);
    
            // Now draw it.
            tmp_text->set_string(tmp_array[i]);
            add_text_figure(tmp_point,*tmp_text);

            // Only update the hi-low one time, or we get
            // the staircase effect.
            if (tmp_point.y + tmp_text->get_current_rect().lr.y > tmp_local_high)
            {
                tmp_local_high = tmp_point.y;
            }
            tmp_point = tmp_old_point;
            tmp_text->remove_ref();
    
            // Keep track if we need to justify for these words.
        }
        
        // dtor function will free tmp_text
        tmp_pair = 
            my_music.get_item(tmp_next_key,gt);

    }
    // Update the height of the staff with the word stuff
    update_high_low(tmp_local_high,tmp_local_high);
}

void
line_of_music::draw_beginning_repeat(point& the_origin)
{
    if (my_begin_barline)
    {
        sizeloc tmp_size = my_begin_barline->get_size_in_pixels();
        point tmp_bar_point = point(my_origin.x,
			my_origin.y - tmp_size.my_loc.y);
        add_figure(tmp_bar_point,*my_begin_barline);
        the_origin.x += (3*tmp_size.my_size.x)/2 - tmp_size.my_loc.x;
    }
}

void 
line_of_music::draw_time_signature(point& the_origin)
{
	map<int,time_sig*>::iterator tmp_it = 
		my_time_sigs.least();
	if (tmp_it)
	{
		my_origin.x -= ((*my_time_sigs.least()).value)->get_x_offset(my_window);
		// my_origin.x -= my_time_sigs[0]->get_width(my_window);
		figure_container<line_of_music> tmp_page(*this);
		((*my_time_sigs.least()).value)->add_to_page(tmp_page,my_window,*this);
		my_origin.x += ((*my_time_sigs.least()).value)->get_width(my_window);
	}
}

void 
line_of_music::add_figures_to_page(page& the_page,const point& the_offset)
{
    map<point_ordinal,figure_array>::iterator tmp_figure_it = 
        my_figure_list.get_item(point_ordinal(point(0,0)),gteq);
    while (tmp_figure_it)
    {
        point tmp_key = (*tmp_figure_it).key;
        figure_array tmp_array = (*tmp_figure_it).value;
        int i;
        for (i = 0;i < tmp_array.get_size();++i)
        {
            point tmp_loc = point(tmp_key.x,tmp_key.y + my_y_offset ) + the_offset;
            the_page.add_figure(tmp_loc,*tmp_array[i]);
            tmp_array[i]->remove_ref();
        }
        tmp_figure_it = my_figure_list.get_item(tmp_key,gt);
    }
    list<text_figure::point_text_pair>::iterator tmp_text_it = 
        my_text_list.first();
    while (tmp_text_it)
    {
        point tmp_loc = point((*tmp_text_it).my_point.x,
                              (*tmp_text_it).my_point.y + my_y_offset) + the_offset;

        the_page.add_text_figure(tmp_loc,
                                 *(*tmp_text_it).my_figure);
        tmp_text_it.next();
    }

    // For future calls to staff, keep in mind that our origin
    // has changed, so everything is relative to that.
    my_origin += the_offset;
    my_true_origin = my_origin;
    my_high_point += the_offset.y;
    my_low_point += the_offset.y;
}

void 
line_of_music::add_y_offset(int the_offset)
{
    my_y_offset += the_offset;
    my_origin.y += the_offset;
    my_high_point += the_offset;
    my_low_point += the_offset;
    my_true_origin.y += the_offset;
}

void 
line_of_music::render()
{
    // Make the accidental array larger so that we can handle accidentals
    // for each measure.
    my_accidentals.expand_to((my_end_measure + 1) - my_start_measure);
	my_true_origin = my_origin;

    // If there's any music on this staff at all.
    if ((my_music.get_size() > 0))
    {
        bool tmp_should_block = true;
        
        // Start out with the hi/lo points for this staff to be the staff itself.
        update_high_low(my_origin.y,my_origin.y - my_staff->get_height_in_pixels());

    	size tmp_ppi = my_window.pixels_per_inch();
    
        // We assume that we are starting at the very left hand side, and that the 
        // window will take care of the margins and stuff.
        
        // First we draw the staff itself
        sizeloc tmp_staff_size = my_staff->get_size_in_pixels();
        add_figure(my_true_origin,*my_staff);
        
        // I like little numbers on the sides of the measures so people can
        // tell where they are.  But don't bother if there is a part
        // number.
        if (my_draw_part_first == false)
        {
            draw_little_numbers();
        }

        // Now draw the left-side fixtures, starting with the cleff signs
        // Treble cleff is centered more or less around G
        sizeloc tmp_clef_size = my_clef->get_size_in_pixels();
        if (get_voice_info(my_start_measure).my_clef == pitch::TrebleClef)
        {
            my_center_pitch = pitch(pitch::B,pitch::natural,1);
        }
        // Base cleff is more or less around F
        else if (get_voice_info(my_start_measure).my_clef == pitch::BassClef)
        {
            my_center_pitch = pitch(pitch::D,pitch::natural,-1);
        }

		// The clef is now stuck on the top of the staff. Move it up so
		// that it is in the middle of the staff.
		point tmp_clef_point = my_true_origin - tmp_clef_size.my_loc;
		tmp_clef_point.y += tmp_staff_size.my_size.y / 2;
		tmp_clef_point.y -= tmp_clef_size.my_size.y / 2;

        // Save the clef point cause we draw stuff after it sometimes.
        my_clef_x = tmp_clef_point.x;

        add_figure(tmp_clef_point,*my_clef);
		my_origin.x += my_clef->get_width_in_pixels();

        // Tell our initial key signature to draw itself
		map<int,key_sig*>::iterator tmp_key_it = 
			my_key_sigs.least();
        if ((my_key_sigs.get_size() > 0) && (tmp_key_it))
        {
            my_origin.x -= ((*tmp_key_it).value)->get_x_offset(tmp_ppi);
            figure_container<line_of_music> tmp_page(*this);
            ((*tmp_key_it).value)->draw_self(tmp_page,my_window,*this);
            my_origin.x += ((*tmp_key_it).value)->get_width(tmp_ppi);
        }
    
        draw_time_signature(my_origin);
    
        draw_beginning_repeat(my_origin);

        // Set this up so we can justify the notes
        my_pixels_for_notes = my_staff->get_width_in_pixels() - 
            (my_origin.x - my_true_origin.x);
        my_pixels_for_notes -= count_measure_pixels();
        
        // This draws all the note stuff, plus all the beams
        render_figures();
 
        // We draw the slurs in after we've made all the notes and stuff.
        if (my_slurer->get_number_points())
        {
            point tmp_slur_point = my_slurer->get_absolute_location();

            // Slurs may extend above/below the current staff boundaries.
            update_high_low(my_slurer->get_bottom_point().y + tmp_slur_point.y,
                            my_slurer->get_top_point().y + tmp_slur_point.y);
            add_figure(tmp_slur_point,*my_slurer);
        }

        // the words are rendered after the notes that they accompany
        render_words();
        render_dynamics();
        render_nth_endings();
        render_ornaments();
        render_chords();
        if (score_mode_on)
        {
            render_voice_info();
        }
        if (my_draw_part_first)
        {
            render_part();
        }
    }
	my_origin = my_true_origin;
    my_has_rendered = true;
    cleanup();
}

sizeloc 
line_of_music::get_size() 
{
    sizeloc tmp_size = my_staff_size;
    int tmp_old_size = tmp_size.my_size.y;

    // If we have drawn stuff above or below the staff, compensate.
    int tmp_up_diff = (my_origin.y - my_high_point);
    int tmp_down_diff = (my_low_point - my_origin.y);
    if (tmp_up_diff < tmp_old_size)
    {
        tmp_size.my_size.y += tmp_old_size - tmp_up_diff;
    }
    if (tmp_down_diff > tmp_old_size)
    {
        tmp_size.my_size.y += tmp_old_size - tmp_down_diff;
    }

    return tmp_size;
}

line_of_music::operator staff_info()
{
	staff_info the_info;
    the_info.horizontal_position = get_horizontal_position();
    the_info.vertical_position = get_vertical_position();
    the_info.size = my_staff->get_size_in_pixels();
    the_info.center_pitch = get_center_pitch();
	return the_info;
}

}
