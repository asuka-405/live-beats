/*
 * window_media.cpp - A piece of music that can be displayed in a window.
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
#include "iabc/window_media.h"
#include "iabc/note_figure.h"
#include "iabc/map.cpp"
#include "iabc/list.cpp"
#include "iabc/drawtemp.cpp"
#include "iabc/text_figure.h"
#include "iabc/threads.h"
#include "iabc/registry_defaults.h"
#include "iabc/iabc_preferences.h"
#include "iabc/array.cpp"

namespace iabc
{
;
int window_media::class_instance_count = 0;

window_media::window_media(paged_window& the_pages,window& the_window,
                           const scale& the_page_size,const scale& the_scale,
                           voice_map_source* the_voice_map_source):
media(the_voice_map_source),my_window(&the_window),
    my_current_page(1),
    my_pages(&the_pages),my_scale(the_scale),
    my_page_size(the_page_size),
    my_last_dynamic(chord_info::mf),
    my_score_map(0),my_music_map(0)
{
    ++class_instance_count;
    my_window->res_add_ref();
    my_visible_rect = my_window->get_visible_rect();
    rect tmp_rect = my_window->get_visible_rect();
    pen* tmp_pen = pen::get_pen(*my_window,255,255,255,1);
    tmp_pen->res_add_ref();
    my_window->moveto(point(0,0));
    my_window->draw_rect(*tmp_pen,tmp_rect,true);
    tmp_pen->res_remove_ref();
}

window_media::~window_media()
{
    --class_instance_count;
    my_window->res_remove_ref();
    map< score_point,line_of_music*>::iterator tmp_it =
        my_staves.least();
    while (tmp_it)
        {
        line_of_music* tmp_staff = (*tmp_it).value;
        tmp_staff->line_remove_ref();
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
        }
}

void
window_media::complete()
{
    // Figure out where all the repeats go to.
    populate_repeat_info();

    // merge all the maps into one easy-to-use map of score-point
    // to musical thing.
    if (score_mode_on == true)
    {
        my_score_map = new map<score_time,music_info>;
        (*my_score_map) =
            extract_music_from_tune_time(my_tune);
    }
    else
    {
        my_music_map = new map<score_point,music_info>;
        (*my_music_map) =
            extract_music_from_tune_point(my_tune);
        map<score_point,music_info>::iterator tmp_init =
            (*my_music_map).least();
    }

    render_music();

    add_figures_to_page();
    render_words();

    // When we've reached the last page, repaginate
    int tmp_pages = my_pages->get_number_pages();
    int i;
    for (i = 1;i <= tmp_pages;++i)
    {
        page tmp_page = my_pages->get_page(i);
        string tmp_copyright_string = my_tune.copyright;
        tmp_copyright_string += (string)"Page " +
            (string)(long)tmp_page.get_page_number() +
            (string)" of " + (string)(long)tmp_pages +
            (string)"\n";
        if (my_staff_names.get_size() > i)
        {
            tmp_copyright_string += my_staff_names[i - 1] + (string)"\n";
        }
        tmp_page.add_copyright(tmp_copyright_string);
    }

    if (my_music_map)
    {
        delete my_music_map;
    }
    else if (my_score_map)
    {
        delete my_score_map;
    }
    my_music_map = 0;
    my_score_map = 0;
}

template<class score_iterator,class parent_class>
music_window_iterator<score_iterator,parent_class>::music_window_iterator(
    parent_class& the_parent,
    paged_window& the_pages,
    int& the_current_page):
    my_parent(&the_parent),
    my_pages(&the_pages),
    my_current_page(&the_current_page)
{
}

template<class score_iterator,class parent_class>
void
music_window_iterator<score_iterator,parent_class>::render_music(
    score_iterator& the_music)
{
    int tmp_rv = 0;
    page tmp_page = my_pages->get_page(*my_current_page);

    for (;
         the_music;
         the_music = the_music.get_item((*the_music).key,gt))
    {
        music_info tmp_info = (*the_music).value;
        score_point tmp_current_beat = as_score_point((*the_music).key);
        if (tmp_info.the_chord_info)
        {
            my_parent->create_note_figures((*tmp_info.the_chord_info),
                                          as_score_point((*the_music).key));
            my_parent->set_dynamics((*tmp_info.the_chord_info),
                                   tmp_current_beat);
        }

        if (tmp_info.the_measure_feature)
        {
            my_parent->add_measure((*tmp_info.the_measure_feature),
                               as_score_point((*the_music).key));
        }
    }

    my_parent->render_all_staffs();
}

template<class score_iterator,class parent_class>
void
execute_music_iterator(score_iterator& the_iterator,
                       paged_window& the_pages,
                       int& the_current_page,
                       parent_class& the_parent)
{
    music_window_iterator<score_iterator,parent_class>
        tmp_iterator(the_parent,the_pages,the_current_page);
    tmp_iterator.render_music(the_iterator);
}


void
window_media::render_music()
{
    if (in_score_mode())
    {
        map<score_time,music_info>::iterator
            tmp_music = (*my_score_map).get_item(score_time(),gteq);
        execute_music_iterator(tmp_music,*my_pages,my_current_page,*this);
    }
    else
    {
        consolidate_long_rests();
        map<score_point,music_info>::iterator
            tmp_music = (*my_music_map).get_item(score_point(),gteq);
        execute_music_iterator(tmp_music,*my_pages,my_current_page,*this);
    }
}

void
window_media::set_dynamics(const chord_info& the_chord,
                           const score_point& the_current_beat)
{
    if ((the_chord.get_dynamic() == chord_info::start_cresc) ||
        (the_chord.get_dynamic() == chord_info::start_decresc))
    {
        dynamic_on_beat tmp_beat;
        tmp_beat.the_beat = the_current_beat;
        tmp_beat.the_dynamic = the_chord.get_dynamic();
        my_last_crescendo_for_voice.add_pair(the_current_beat.voice,tmp_beat);
    }
    else if ((the_chord.get_dynamic() == chord_info::end_cresc) ||
             (the_chord.get_dynamic() == chord_info::end_decresc))
    {
        map<int,dynamic_on_beat>::iterator tmp_dynamic_it =
            my_last_crescendo_for_voice.get_item(the_current_beat.voice);
        if (tmp_dynamic_it)
        {
            score_point tmp_start_point = (*tmp_dynamic_it).value.the_beat;
            line_of_music* tmp_last_staff = get_or_create_line_of_music(tmp_start_point);
            line_of_music* tmp_this_staff = get_or_create_line_of_music(the_current_beat);
            if ((tmp_last_staff) &&
                (tmp_last_staff == tmp_this_staff))
            {
                tmp_last_staff->add_crescendo(staff_point(tmp_start_point.measure,
                                                           tmp_start_point.beat),
                                               (*tmp_dynamic_it).value.the_dynamic,
                                               staff_point(the_current_beat.measure,
                                                           the_current_beat.beat),
                                                           the_chord.get_dynamic());
            }
            my_last_crescendo_for_voice.remove(the_current_beat.voice);
        }
    }
    else
    {
        map<score_point,voice_info>::iterator the_voice_pair =
            my_tune.my_voice_info.get_item(the_current_beat,lt);
        if (the_chord.get_dynamic() != my_last_dynamic)
        {
            line_of_music* tmp_this_staff = get_or_create_line_of_music(the_current_beat);
            if (tmp_this_staff)
            {
                tmp_this_staff->add_dynamic(
                    staff_point(the_current_beat.measure,the_current_beat.beat),
                    the_chord.get_dynamic());
                my_last_dynamic = the_chord.get_dynamic();
            }
        }
    }
}

void
window_media::add_margins_stuff(int the_page)
{
    page tmp_page = my_pages->get_page(the_page);
    tmp_page.add_title(my_tune.title);
}

void
window_media::render_all_staffs()
{
    int i = 0;
    for (i = 0;i < my_staffs_to_render.get_size();++i)
        {
        my_staffs_to_render[i]->render();
        }

    my_staffs_to_render.remove_all();
    my_started_staffs.remove_all();
}

void
window_media::render_words()
{
    list<string>::iterator tmp_it =
        my_tune.my_songwords.first();
    point tmp_point = my_last_rendered_point;

    page tmp_page = my_pages->get_page(my_current_page);
    bool tmp_first = true;
    while (tmp_it)
    {
        int tmp_point_size = window_media_songword_size;
        font::style tmp_style =  font::normal;
        if (window_media_songword_italic)
        {
            tmp_style = (font::style)((int)tmp_style | (int)font::slanty);
        }
        text_figure* tmp_figure = new text_figure(font::serifs,tmp_point_size,
                    *my_window,font::slanty);
        string tmp_string = (*tmp_it);
        tmp_figure->add_ref();
        tmp_figure->set_string(tmp_string);
        rect tmp_rect = tmp_figure->get_current_rect();
        if (tmp_first)
        {
            tmp_point.y += tmp_rect.height();
            tmp_first = false;
        }
        // If the words go below the current page, make a new page.
        if (tmp_page.lr_drawing_point().y < tmp_rect.height() + tmp_point.y)
        {
            my_current_page++;
            my_pages->add_page(my_current_page);
            tmp_page = my_pages->get_page(my_current_page);
            add_margins_stuff(my_current_page);
            tmp_point = point(tmp_page.ul_drawing_point());
        }
        tmp_point.x = (tmp_page.get_ul().x + tmp_page.get_lr().x)/2;
        tmp_point.x -= tmp_rect.width() / 2;
        tmp_page.add_text_figure(tmp_point,*tmp_figure);
        tmp_figure->remove_ref();
        tmp_point.y += tmp_rect.height();

        tmp_it.next();
    }

    my_last_rendered_point = tmp_point;
}

void
window_media::render_measure_feature(const measure_feature& the_feature,const score_point& the_count)
{
    my_tune.my_measures.add_pair(the_count,the_feature);
}

void
window_media::add_nth_endings(line_of_music& the_staff)
{
    map<int,repeat_info>::iterator tmp_it =
        my_repeat_info.get_item(the_staff.get_start_measure(),gteq);
    while (tmp_it)
    {
        int tmp_key = (*tmp_it).key;
        repeat_info tmp_info = (*tmp_it).value;
        the_staff.add_nth_ending(tmp_info.my_ending_start_measure,
                                 tmp_info.my_measure,
                                 tmp_info.my_ending_number);

        tmp_it = my_repeat_info.get_item(tmp_key,gt);

    }
}

line_of_music*
window_media::get_or_create_line_of_music(const score_point& the_count)
{
    line_of_music* tmp_staff = 0;
    map<score_point,line_of_music*>::iterator tmp_staff_pair =
        my_staves.get_item(the_count,lteq);
    if (tmp_staff_pair == 0)
    {
        // This must be the first one
        tmp_staff = create_new_staff(the_count);
    }
    else
    {
        // map returns the staff that has the greatest measure/fraction
        // value, but is less than or equal to the key.  If the staff returned
        // is of a different voice than this staff, then we need to make a
        // new staff for the new voice.
        //
        // Likewise if the last staff has already met its 'endline' then
        // we must be on a new staff so draw a new one.
        score_point tmp_key = (*tmp_staff_pair).key;
        tmp_staff = (*tmp_staff_pair).value;
        if ((tmp_key.voice != the_count.voice) ||
            (tmp_staff->get_is_complete() == true))
        {
            tmp_staff = create_new_staff(the_count);
        }
    }
    return tmp_staff;
}

line_of_music*
window_media::get_previous_staff(const line_of_music& the_staff)
{
    line_of_music* tmp_rv = 0;
    staff_page_map::iterator tmp_it =
        my_staves_y.most();
    if (tmp_it)
    {
        tmp_rv = (*tmp_it).value;
    }
    return tmp_rv;
}

int
window_media::get_vertical_offset(const score_point& the_point,line_of_music* the_staff)
{
    staff_map::iterator tmp_it =
        my_staves.get_item(the_point,lteq);
    if (tmp_it != 0)
    {
        if ((*tmp_it).key.measure == the_point.measure)
        {
            int tmp_pos = (*tmp_it).value->get_vertical_position();

            return tmp_pos;
        }
        else
        {
            int tmp_y = (*tmp_it).value->get_vertical_position() +
                the_staff->get_size().my_size.y;
            return tmp_y;
        }
    }

    return 0;
}

void
window_media::add_measure(const measure_feature& the_feature,
                          const score_point& the_point)
{
    measure_feature::measure_feature_enum the_end_type = the_feature.end_type();
    measure_feature::measure_feature_enum the_begin_type = the_feature.begin_type();
    line_of_music* tmp_staff = get_or_create_line_of_music(the_point);
    voice_info tmp_info = get_voice_info(the_point);
    tmp_info.my_key = transpose_key(tmp_info.my_key,tmp_info.my_transpose);
    if (tmp_staff)
    {
        tmp_staff->add_measure(the_end_type,the_begin_type,the_point,tmp_info);

        if (the_feature.is_line_break())
        {
            // Now add the staff to the correct page.
            page tmp_page = my_pages->get_page(my_current_page);
            add_nth_endings(*tmp_staff);
            my_staffs_to_render.add_to_end(tmp_staff);
            tmp_staff->set_is_complete();
            if (tmp_staff->get_high_point() > tmp_page.lr_drawing_point().y)
            {
                my_current_page++;
                my_pages->add_page(my_current_page);
            }
        }
    }
}

bool
window_media::should_draw_part(const score_point& the_point)
{
    bool tmp_rv = true;
    if ((the_point.measure > 1) ||
        (the_point.beat > 0))
    {
        fraction tmp_beat;
        int tmp_measure = the_point.measure;
        if (the_point.beat == (fraction)0)
        {
            tmp_measure--;
        }
        voice_info tmp_info = get_voice_info(score_point(tmp_measure,
                                                         tmp_beat,the_point.voice));
        voice_info tmp_current_info =
            get_voice_info(the_point);
        if (tmp_current_info.my_part == tmp_info.my_part)
        {
            tmp_rv = false;
        }
    }
    return tmp_rv;
}

line_of_music*
window_media::create_new_staff(const score_point& the_point)
{
    // Get the information (key, etc) about this point in the song
    voice_info tmp_info = get_voice_info(the_point);
    tmp_info.my_key = this->transpose_key(tmp_info.my_key,tmp_info.my_transpose);

    // Look up the last staff that we drew on to get the coordinates
    // for this one.  If there is no previous line_of_music then use the ul drawing
    // point on this page.
    line_of_music* tmp_previous = 0;
    bool tmp_should_draw_part = should_draw_part(the_point);
    if (my_started_staffs.get_size() > 0)
        {
        tmp_previous = my_started_staffs[my_started_staffs.get_size() - 1];
        }
    page tmp_page = my_pages->get_page(1);
    point tmp_origin = tmp_page.ul_drawing_point();

    if (tmp_previous)
    {
        tmp_origin.y = tmp_previous->get_high_point() +
            tmp_previous->get_staff_size().y / 2;
    }
    else
    {
        tmp_origin = tmp_page.ul_drawing_point();
    }

    line_of_music* tmp_new_staff = new line_of_music(*my_window,
          my_scale,
          scale(iabc::window_media_note_scale,iabc::window_media_note_scale),
          my_page_size.x,
          tmp_info,
          tmp_origin,
          the_point.measure,
          tmp_should_draw_part);

    my_started_staffs.add_to_end(tmp_new_staff);
    my_staves.add_pair(the_point,tmp_new_staff);

    // Keep track of the new staff
    tmp_new_staff->line_add_ref();

    // Side effect of calling this function is that the scroll size on
    // the drawing window gets set to the size of one line_of_music.
    my_window->set_thumb_scroll_size(tmp_new_staff->get_staff_size().y);

    return tmp_new_staff;
}

void
window_media::get_count_by_voice(int the_count,score_point& the_measure_point,score_point& the_chord_point)
{
    map<int,score_point>::iterator tmp_measures = my_saved_measure_points.get_item(the_count);
    map<int,score_point>::iterator tmp_chords = my_saved_chord_points.get_item(the_count);

    if (tmp_measures)
    {
        the_measure_point = (*tmp_measures).value;
    }
    else
    {
        the_measure_point = score_point(0,0,1);
    }

    if (tmp_chords)
    {
        the_chord_point = (*tmp_chords).value;
    }
    else
    {
        the_chord_point = score_point(0,0,1);
    }
}

void
window_media::save_count_by_voice(const score_point& the_measure_count,const score_point& the_chord_count)
{
    my_saved_measure_points.add_pair(the_measure_count.voice,the_measure_count);
    my_saved_chord_points.add_pair(the_chord_count.voice,the_chord_count);
}

int
window_media::get_staves_per_system()
{
    if (in_score_mode())
    {
        return count_voices();
    }
    else
        return 1;
}

void
window_media::consolidate_long_rests()
{
    // Consolidate long rests into
    //     8
    //  |-----|
    // looking things
    score_point tmp_current_point;
    score_point tmp_previous_point;
    map<score_point,int> tmp_rv;
    voice_info tmp_voice_info;
    fraction tmp_signature(4,4);
    int tmp_voice = tmp_voice_info.my_index;
    int tmp_number_consecutive_rests = 0;
    chord_info tmp_chord_info;
    music_info tmp_start_rest_music_info;
    music_info tmp_end_rest_music_info;

    // If we are in part mode, this will be something
    if (my_music_map)
    {
        map<score_point,music_info>* tmp_new_map = new map<score_point,music_info>();

        // Go through all the music.
        map<score_point,music_info>::iterator tmp_it =
            (*my_music_map).get_item(tmp_current_point,gteq);
        while (tmp_it)
        {
            music_info& tmp_music_info = (*tmp_it).value;
            tmp_current_point = (*tmp_it).key;
            bool tmp_part_changed = false;

            // The voice information may have changed.
            if (tmp_music_info.the_voice_info)
            {
                voice_info tmp_current_voice_info =
                    (*tmp_music_info.the_voice_info);

                // We need to break up long rests to draw
                // a key, time or part change.
                if (tmp_current_voice_info.my_part !=
                    tmp_voice_info.my_part)
                {
                    tmp_part_changed = true;
                }
                if (tmp_current_voice_info.my_key !=
                    tmp_voice_info.my_key)
                {
                    tmp_part_changed = true;
                }
                if (tmp_current_voice_info.my_time_signature !=
                    tmp_voice_info.my_time_signature)
                {
                    tmp_part_changed = true;
                }

                tmp_voice_info = tmp_current_voice_info;
                tmp_signature = tmp_voice_info.my_time_signature;
            }

            // If this is a chord, look at whether its a rest or not.
            if (tmp_music_info.the_chord_info)
            {
                tmp_chord_info = (*tmp_music_info.the_chord_info);
                bool tmp_is_long_rest = ((tmp_chord_info.get_size() > 0) &&
                    (tmp_chord_info[0].is_rest() == true) &&
                    (tmp_chord_info[0].get_duration() >= tmp_signature));

                // If we are counting a long rest and something about the
                // voice has changed that requires a measure line to be
                // redrawn, indicate that.
                bool tmp_is_part_change_in_rest =
                    ((tmp_is_long_rest == true) &&
                     (tmp_number_consecutive_rests > 0) &&
                     (tmp_part_changed == true));

                // If this is a rest that lasts the entire measure, start counting.
                if ((tmp_is_long_rest == true) &&
                    (tmp_current_point.voice == tmp_previous_point.voice) &&
                    (tmp_is_part_change_in_rest == false))
                {
                    tmp_number_consecutive_rests++;
                    // Preserve the initial music_info structure.  It may contain
                    // other information, like voice_info changes.
                    if (tmp_number_consecutive_rests == 1)
                    {
                        tmp_start_rest_music_info = tmp_music_info;
                    }
                }

                // If this is not a long rest, and we just found a bunch of
                // long rests, we are done with a long rest.  Add the long
                // rest to the map.
                else if (tmp_number_consecutive_rests >= 1)
                {
                    // Figure out where the rest started.
                    int tmp_measure_start =
                        tmp_previous_point.measure - (tmp_number_consecutive_rests - 1);
                    score_point tmp_target(tmp_measure_start,0,tmp_previous_point.voice);

                    // Create a new chord_info structure with the appropriate rest length
                    fraction tmp_rest_fraction(tmp_number_consecutive_rests);
                    pitch tmp_rest_pitch;
                    note_info tmp_note_info(tmp_rest_pitch,tmp_rest_fraction,true);
                    chord_info tmp_rest_chord_info;
                    tmp_rest_chord_info += tmp_note_info;

                    // Add it to the map.  There might be other voice_info or measure
                    // stuff at the first rest and so we try to preserve that.
                    tmp_start_rest_music_info.the_chord_info = tmp_rest_chord_info;
                    tmp_start_rest_music_info.the_measure_feature =
                        tmp_end_rest_music_info.the_measure_feature;

                    // Add the new rest structure to the map.
                    (*tmp_new_map).add_pair(tmp_target,tmp_start_rest_music_info);

                    // We got here because there's new note information.
                    // Add that new note information to the map.
                    (*tmp_new_map).add_pair(tmp_current_point,tmp_music_info);

                    // Reset the accumulator or rest length
                    tmp_number_consecutive_rests = 0;

                }
                // Add anything that's not in a long rest group to the new map.
                else if (tmp_number_consecutive_rests == 0)
                {
                    (*tmp_new_map).add_pair(tmp_current_point,tmp_music_info);
                }
            }

            // Add anything that's not in a long rest group to the new map.
            else if (tmp_number_consecutive_rests == 0)
            {
                (*tmp_new_map).add_pair(tmp_current_point,tmp_music_info);
            }
            // If we're in a rest and this is a measure ending, save it since we need
            // to store the previous measure information.
            else if (tmp_music_info.the_measure_feature)
            {
                tmp_end_rest_music_info = tmp_music_info;
            }
            tmp_previous_point = tmp_current_point;
            tmp_it = tmp_it.get_item(tmp_current_point,gt);
        }

        // Now use the new music map with the consolidated rests in it.
        delete my_music_map;
        my_music_map = tmp_new_map;
    }

}

void
window_media::sort_staffs_in_y_order(map<int,line_of_music*>& the_y_to_staff_map)
{
    staff_map::iterator tmp_it =
        my_staves.least();
    while (tmp_it)
    {
        line_of_music* tmp_staff = (*tmp_it).value;
        int tmp_height = tmp_staff->get_vertical_position();
        the_y_to_staff_map.add_pair(tmp_height,tmp_staff);
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }
}

void
window_media::adjust_staff_height(map<int,line_of_music*> the_y_to_staff_map,
                                  map<int,line_of_music*>& the_adjusted_map)
{
    int tmp_old_height = 0;
    int tmp_y_offset = 0;
    int tmp_old_measure = 1;

    map<int,line_of_music*>::iterator tmp_tmp_map_it = the_y_to_staff_map.least();
    while (tmp_tmp_map_it)
    {
        // Calculate the offset to keep the staffs from overlapping
        line_of_music* tmp_staff = (*tmp_tmp_map_it).value;
        int tmp_staff_gap = (tmp_staff->get_staff_size().y / 2);
        int tmp_height = tmp_staff->get_low_point() + tmp_y_offset;
        if (tmp_height < tmp_old_height)
        {
            tmp_y_offset += (tmp_old_height + tmp_staff_gap) - tmp_height;
            tmp_height = tmp_old_height + tmp_staff_gap;
        }
        // If we are in score mode, put a little break between the staff groups
        if ((in_score_mode()) &&
            (tmp_staff->get_start_measure() > tmp_old_measure))
        {
            tmp_old_measure = tmp_staff->get_start_measure();
            tmp_y_offset += tmp_staff->get_staff_size().y;
        }

        tmp_staff->add_y_offset(tmp_y_offset);

        tmp_old_height = tmp_staff->get_high_point();
        the_adjusted_map.add_pair(tmp_height,tmp_staff);
        tmp_tmp_map_it = tmp_tmp_map_it.get_item((*tmp_tmp_map_it).key,gt);
    }
}

void
window_media::add_figures_to_page()
{
    map<int,line_of_music*> tmp_adjusted_y_to_staff_map;
    map<int,line_of_music*> tmp_y_to_staff_map;
    map<int,line_of_music*>::iterator tmp_map_it;

    my_current_page = 1;
    // The staff map is arranged in some order.  Sort the staffs into
    // the order from highest to lowest on the screen.
    sort_staffs_in_y_order(tmp_y_to_staff_map);

    // While we're at
    // it, make sure that the staffs don't overlap.
    adjust_staff_height(tmp_y_to_staff_map,tmp_adjusted_y_to_staff_map);

    // Now draw the staffs on the page so that they don't
    // overlap the page boundaries.
    array<line_of_music*> tmp_draw_array;
    int tmp_staves_system = get_staves_per_system();
    tmp_map_it = tmp_adjusted_y_to_staff_map.least();
    page tmp_page = my_pages->get_page(1);
    add_margins_stuff(1);
    int tmp_hi_page = tmp_page.lr_drawing_point().y;
    point tmp_offset;
    int i;
    int tmp_last_measure = 0;
    string tmp_staff_name;
    my_staff_names.expand(1);
    while (tmp_map_it)
    {
        tmp_draw_array.remove_all();

        // There is a break between each page that might occur in the
        // middle of a system.  If that happens move the whole thing down
        // to the next page.  Note that if we are not in score mode, tmp_staves_system
        // will always be 1.
        int tmp_low_staff = 0;
        for (i = 0;i < tmp_staves_system && tmp_map_it;
             ++i,tmp_map_it = tmp_map_it.get_item((*tmp_map_it).key,gt))
        {
            line_of_music* tmp_staff = (*tmp_map_it).value;
            int tmp_hi_staff = tmp_staff->get_high_point();
            if (i == 0)
            {
                tmp_low_staff = tmp_staff->get_low_point();
            }

            // There are 2 reasons that we might put a page break here: 1 is
            // if we are in score mode and the system will cross a page
            // boundary, and another is if we are in part mode and we have
            // just started a new part.
            bool tmp_staff_splits_page = tmp_hi_staff + tmp_offset.y > tmp_hi_page;
            bool tmp_staff_starts_voice = ((in_score_mode() == false) &&
                      // If we have changed voices, the start measure will start
                      // over at 1 again.
                     (tmp_last_measure) &&
                     (tmp_last_measure > tmp_staff->get_start_measure()) &&
                      (iabc::page_break_between_parts == true));

            if ((tmp_staff_splits_page) ||
                (tmp_staff_starts_voice))
            {
                int tmp_next_page = tmp_page.get_page_number() + 1;
                my_pages->add_page(tmp_next_page);
                my_staff_names.expand(1);
                add_margins_stuff(tmp_next_page);
                tmp_page = my_pages->get_page(tmp_next_page);
                tmp_offset.y = (tmp_page.ul_drawing_point().y - tmp_low_staff);
                tmp_hi_page = tmp_page.lr_drawing_point().y;
                my_current_page = tmp_next_page;
            }
            tmp_last_measure = tmp_staff->get_end_measure();
            tmp_draw_array.add_to_end(tmp_staff);
        }

        // Now that we know what offset to add to all the line_of_musics,
        // we can finally render the staffs on the page.
        for (i = 0;i < tmp_draw_array.get_size();++i)
        {
            line_of_music* tmp_staff = tmp_draw_array[i];
            tmp_staff_name = tmp_staff->get_voice_info(tmp_staff->get_start_measure()).my_name;
            if ((score_mode_on == false) && (tmp_staff_name.length() > 0))
            {
                my_staff_names[tmp_page.get_page_number() - 1] =
                    tmp_staff_name;
            }
            tmp_staff->add_figures_to_page(tmp_page,tmp_offset);

            // Store some information for the GUI's benefit.
            score_point tmp_gui_point;
            tmp_gui_point.measure = tmp_staff->get_start_measure();
            tmp_gui_point.voice = tmp_staff->get_voice_info(tmp_gui_point.measure).my_index;
            global_cursor_event_log.log_event(cursor_event_data::store_screen_point,
                                              (*tmp_staff).get_vertical_position(),
                                              tmp_gui_point.measure,
                                              tmp_gui_point.voice,
                                              LOG_FRACTION(tmp_gui_point.beat));
            my_score_points_to_lines.add_pair(tmp_gui_point,
                (*tmp_staff).get_vertical_position());
            my_lines_to_score_points.add_pair((*tmp_staff).get_vertical_position(),
                                              tmp_staff->get_lowest_score_point());
            my_last_rendered_point = point(tmp_staff->get_horizontal_position(),
                tmp_staff->get_high_point());
        }
    }
}

pitch
window_media::transpose_key(const pitch& the_pitch,int the_steps)
{
    pitch tmp_rv = the_pitch;
    if (in_score_mode() == false)
        {
        tmp_rv = tmp_rv.transpose(tmp_rv,the_steps);
        }
    return tmp_rv;
}

chord_info
window_media::transpose_chord(const chord_info& the_info,int the_steps,const pitch& the_old_key)
{
    int i;
    chord_info tmp_rv;
    if ((the_steps) && (in_score_mode() == false))
        {
        for (i = 0;i < the_info.get_size();++i)
            {
            note_info tmp_note = the_info[i];
            pitch tmp_pitch = tmp_note.get_pitch();
            tmp_pitch = tmp_pitch.transpose(the_old_key,the_steps);
            tmp_note.change_pitch(tmp_pitch);
            tmp_rv += tmp_note;
            }
        tmp_rv.set_embellishment(the_info.get_embellishment());
        tmp_rv.set_description(the_info.get_description());
        tmp_rv.set_first_in_beam(the_info.is_first_in_beam());
        tmp_rv.set_grace(the_info.is_grace());
        tmp_rv.set_last_in_beam(the_info.is_last_in_beam());
        tmp_rv.set_xlet_value(the_info.get_xlet_value());
                array<string> ar;
                ar = the_info.get_words();
        tmp_rv.set_words(ar);
        tmp_rv.set_dynamic(the_info.get_dynamic());
        }
    else
        {
        tmp_rv = the_info;
        }
    return tmp_rv;
}

void
window_media::create_note_figures(const chord_info& the_feature,const score_point& the_count)
{
    // Get the current staff we're working with.
    line_of_music* tmp_staff = get_or_create_line_of_music(the_count);
    if (tmp_staff != 0)
    {
        staff_point tmp_point = staff_point(the_count.measure,the_count.beat);
        voice_info tmp_info = this->get_voice_info(the_count);
        pitch tmp_old_key = tmp_info.my_key;
        tmp_info.my_key = transpose_key(tmp_info.my_key,tmp_info.my_transpose);
        chord_info tmp_feature = transpose_chord(the_feature,tmp_info.my_transpose,tmp_old_key);

        note_figure* tmp_note = new note_figure(*my_window,
                                                         tmp_feature,
                                                         tmp_info.my_key);
        tmp_staff->add_note_figure(*tmp_note,tmp_point);
        if ((tmp_feature.starts_slur() == true) ||
            (tmp_feature.starts_tie() == true))
        {
            tmp_staff->add_slur_from(tmp_point);
        }
        if ((tmp_feature.ends_slur() == true) ||
            (tmp_feature.ends_tie() == true))
        {
            tmp_staff->add_slur_to(tmp_point);
        }
    }
}

void
window_media::render_music_feature(const chord_info& the_feature,const score_point& the_count)
{
    // Since this is a GUI, we just save the music so we can
    // create it later.
    my_tune.my_music.add_pair(the_count,the_feature);
}
}
