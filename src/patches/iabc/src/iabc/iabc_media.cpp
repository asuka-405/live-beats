/*
 * iabc_media.cpp - Reads in and handles an abc file.
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
#include "iabc/iabc_media.h"
#include "iabc/array.cpp"
#include "iabc/message_box.h"
#include "iabc/percentage_meter.h"
#include "iabc/map.cpp"
#include "iabc/iabc_parse.h"
#include "iabc/registry.h"
#include "iabc/registry_defaults.h"

extern bool globalUserAbortFlag;
namespace iabc
{
;

iabc_media::iabc_media(tune_chooser& the_tunes,
                       preprocess& the_text_buffer):
        media_source(the_tunes),
        my_input_buffer(&the_text_buffer),
        my_has_parsed(false),
        my_parser(0),
        my_last_modify_time(0),
        my_last_parse_time(0),
    my_number_of_tunes(0)
{
}

iabc_media::~iabc_media()
{
    if (my_parser)
    {
        delete my_parser;
    }
}

void
iabc_media::get_points(int the_part,score_point& the_measure_point,score_point& the_chord_point)
{
    map<int,score_point>::iterator tmp_measures = my_saved_measure_points.get_item(the_part);
    map<int,score_point>::iterator tmp_chords = my_saved_chord_points.get_item(the_part);

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
iabc_media::save_points(const score_point& the_measure_point,const score_point& the_chord_point)
{
    my_saved_measure_points.add_pair(the_measure_point.voice,the_measure_point);
    my_saved_chord_points.add_pair(the_chord_point.voice,the_chord_point);
}

int
iabc_media::update_current_tune()
{
    int tmp_current_tune = my_tunes->get_choice();
    registry_entry tmp_entry = registry_entry(reg::last_file.as_key(),
                                              my_input_buffer->get_filename().access_char_array(),
                                              true);
    globalRegistry.set_value(tmp_entry);
    string tmp_current_tune_string = (string)(long)(tmp_current_tune);
    tmp_entry = registry_entry(reg::last_tune.as_key(),
                                              tmp_current_tune_string.access_char_array(),true);
    globalRegistry.set_value(tmp_entry);
    globalRegistry.flush();

    return tmp_current_tune;
}

void
iabc_media::present_all()
{
    // Let the media know some music is coming.
    my_has_parsed = true;
    my_media->setup();

    // let the user choose which tune he wants to see/hear.
    int tmp_current_tune = my_current_tune;
    my_current_tune = update_current_tune();

    my_parser->parse(my_current_tune);
    tune& tmp_tune = (get_current_tune());

    // Present the non-music parts of the tune, like the title etc.  It
    // does _not_ read the music part out of the tune, maybe this interface
    // is a little confusing.
    my_media->present(tmp_tune);

    // If I already have the tune in the media, why present it bit by bit?
    // This may come in handy if we ever read from a
    // midi device of some real-time thing.  Then we can send in an empty tune and
    // fill it in as we read the midi stream.  But since we need to parse the whole
    // things before we render it, that doesn't apply for an abc file.  It also
    // allows us to filter voices that we don't care about.
    score_point tmp_chord_point(0,0,1);
    score_point tmp_measure_point(0,0,1);
    map<score_point,chord_info>::iterator tmp_notes =
        tmp_tune.my_music.get_item(tmp_chord_point,gteq);
    map<score_point,measure_feature>::iterator tmp_measures =
        tmp_tune.my_measures.get_item(tmp_measure_point,gteq);
    int tmp_start_measure = 1;

    while (tmp_measures)
    {
        // The music isn't necessarily stored in the same order in which we
        // read it.  So we need to occasionally store the current voice in the
        // music to read in another voice part.
        measure_feature tmp_measure = (*tmp_measures).value;
        if ((*tmp_measures).key.voice != tmp_measure_point.voice)
        {
            save_points(tmp_measure_point,tmp_chord_point);
            get_points((*tmp_measures).key.voice,tmp_measure_point,tmp_chord_point);
        }
        tmp_measure_point = (*tmp_measures).key;

        while ((tmp_notes) && (tmp_chord_point < tmp_measure_point))
        {
            chord_info tmp_info = (*tmp_notes).value;
            score_point tmp_note_key = (*tmp_notes).key;
            my_media->present(tmp_info,tmp_note_key);
            tmp_notes = tmp_tune.my_music.get_item(tmp_note_key,gt);
            if (tmp_notes)
            {
                tmp_chord_point = (*tmp_notes).key;
            }
        }
        my_media->present(tmp_measure,tmp_measure_point);
        tmp_measures = tmp_tune.my_measures.get_item(tmp_measure_point,gt);
    }

    // Let the device know that we have finished sending music and the whole
    // thing can be put together.
    my_media->map_voices_names();
    my_media->complete();
}

long
iabc_media::get_start_line_of_current_tune()
{
    long tmp_rv = 0;
    if (my_parser)
    {
        tmp_rv = my_parser->get_start_line_of_current_tune();
    }

    return tmp_rv;
}
void
iabc_media::parse(media& the_media)
{
    int tmp_measure = 1;
    int tmp_old_measure = tmp_measure;
    my_media = &the_media;
    // if (my_input_buffer->get_last_modify_time() > my_last_modify_time)
    clear_tunes();

    // If the buffer has changed, do a quick scan of the ifle to
    // see if there are any new tunes added.
    if ((my_parser != 0) && (my_last_modify_time > my_last_parse_time))
    {
        map<int,iabc_parse::tune_index_info> tmp_new_map =
            my_parser->quick_parse();
        if (tmp_new_map.get_size() != my_number_of_tunes)
        {
            delete my_parser;
            my_parser = 0;
        }
    }
    if (my_parser == 0)
    {
        my_has_parsed = false;
        remove_all_tunes();
        my_last_modify_time = my_input_buffer->get_last_modify_time();
        if (my_parser)
            delete my_parser;

        my_parser = new iabc_parse(*my_input_buffer,*this,&my_lines_to_score_points);
        map<int,iabc_parse::tune_index_info> tmp_map =
            (*my_parser).quick_parse();
        my_number_of_tunes = tmp_map.get_size();
        map<int,iabc_parse::tune_index_info>::iterator tmp_it =
            tmp_map.get_item(0,gteq);
        while (tmp_it)
        {
            int tmp_key = (*tmp_it).key;
            iabc_parse::tune_index_info tmp_info = (*tmp_it).value;
            tune tmp_tune;
            tmp_tune.index = tmp_key;
            tmp_tune.title = tmp_info.the_title;
            my_tunes->add_tune(tmp_tune);
            tmp_it = tmp_map.get_item(tmp_key,gt);
        }
    }

    // As a side-effect of this call, my_lines_to_score_points is populated.
    present_all();

    my_score_points_to_lines = get_inverse_map(my_lines_to_score_points);
    my_last_parse_time = time(0);
}

}

