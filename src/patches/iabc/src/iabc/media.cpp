/*
 * media.cpp - Base class for a music file and its representation.
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
#include "iabc/media.h"
#include "iabc/map.cpp"
#include "iabc/list.cpp"

namespace iabc
{
;

score_point::score_point(int the_measure,fraction the_beat,int the_voice):
    measure(the_measure),voice(the_voice),beat(the_beat)
{
}

score_point::score_point(const score_point& o):
    measure(o.measure),voice(o.voice),beat(o.beat)
{
}

score_point& 
score_point::operator=(const score_point& o)
{
    measure = o.measure;
    beat = o.beat;
    voice = o.voice;
    return *this;
}

score_point::score_point():measure(1),voice(1)
{
}

bool 
score_point::operator==(const score_point& o) const
{
    return ((voice == o.voice) && (beat == o.beat) && (measure == o.measure));
}

bool 
score_point::operator!=(const score_point& o) const 
{
    return (!(*this == o));
}

bool 
score_point::operator>(const score_point& o) const
{
    bool tmp_rv = false;
    if (voice > o.voice)
    {
        tmp_rv = true;
    }
    else if ((voice == o.voice) && (measure > o.measure))
    {
        tmp_rv = true;
    }
    else if ((voice == o.voice) && (measure == o.measure) && (beat > o.beat))
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

bool 
score_point::operator<(const score_point& o) const 
{
    return (((*this > o) == false) && ((*this != o)));
}

bool 
score_point::operator>=(const score_point& o) const 
{ 
    return ((*this < o) == false);
}

bool 
score_point::operator<=(const score_point& o) const 
{
    return ((*this > o) == false);
}

score_time::score_time(int the_measure,fraction the_beat,int the_voice):
    measure(the_measure),voice(the_voice),beat(the_beat)
{
}
score_time::score_time(const score_time& o):
    measure(o.measure),voice(o.voice),beat(o.beat)
{
}

score_time& 
score_time::operator=(const score_time& o)
{
    measure = o.measure;
    beat = o.beat;
    voice = o.voice;
    return *this;
}

score_time::score_time():measure(1),voice(1)
{
}

bool 
score_time::operator==(const score_time& o) const
{
    return ((voice == o.voice) && (beat == o.beat) && (measure == o.measure));
}

bool 
score_time::operator!=(const score_time& o) const
{
    return (!(*this == o));
}

bool 
score_time::operator>(const score_time& o) const
{
    bool tmp_rv = false;
    if (measure > o.measure)
    {
        tmp_rv = true;
    }
    else if ((measure == o.measure) && (beat > o.beat))
    {
        tmp_rv = true;
    }
    else if ((measure == o.measure) && (beat == o.beat) &&
             (voice > o.voice))
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

bool 
score_time::operator<(const score_time& o) const 
{
    return (((*this > o) == false) && ((*this != o)));
}

bool 
score_time::operator>=(const score_time& o) const 
{ 
    return ((*this < o) == false);
}

bool 
score_time::operator<=(const score_time& o) const 
{
    return ((*this > o) == false);
}

score_time
as_score_time(const score_point& the_point)
{
    return score_time(the_point.measure,the_point.beat,the_point.voice);
}

score_time
as_score_time(const score_time& the_time)
{
    return the_time;
}

score_point 
as_score_point(const score_time& the_time)
{
    return score_point(the_time.measure,the_time.beat,the_time.voice);
}

score_point 
as_score_point(const score_point& the_point)
{
    return the_point;
}

map<score_point,music_info>
    extract_music_from_tune_voice(tune& the_tune,int the_voice)
{
    map<score_point,music_info> tmp_rv;
    map<score_point,music_info>::iterator tmp_music_it;
    map<score_point,chord_info>::iterator tmp_chord_it = 
        the_tune.my_music.least();
    // First add the chord info to the map.
    while (tmp_chord_it)
    {
        score_point tmp_point = (*tmp_chord_it).key;
        if (tmp_point.voice == the_voice)
        {
            music_info tmp_info;
            tmp_info.the_chord_info = (*tmp_chord_it).value;
            tmp_rv.add_pair(tmp_point,tmp_info);
        }
        tmp_chord_it = tmp_chord_it.get_item((*tmp_chord_it).key,gt);
    }
    
    // Now add the measure features.  If there is already a 
    // chord info at this point, make sure we retain it.
    map<score_point,measure_feature>::iterator tmp_measure_it = 
        the_tune.my_measures.least();
    
    while (tmp_measure_it)
    {
        music_info tmp_info;
        score_point tmp_point = (*tmp_measure_it).key;
        if (tmp_point.voice == the_voice)
        {
            tmp_music_it = 
                tmp_rv.get_item(tmp_point);
            if (tmp_music_it)
            {
                tmp_info = (*tmp_music_it).value;
            }
            tmp_info.the_measure_feature = (*tmp_measure_it).value;
            tmp_rv.add_pair((*tmp_measure_it).key,tmp_info);
        }
        tmp_measure_it = 
            tmp_measure_it.get_item((*tmp_measure_it).key,gt);
    }

    map<score_point,voice_info>::iterator tmp_voice_it = 
        the_tune.my_voice_info.least();

    while (tmp_voice_it)
    {
        music_info tmp_info;
        score_point tmp_point = (*tmp_voice_it).key;
        if (tmp_point.voice == the_voice)
        {
            tmp_music_it = 
                tmp_rv.get_item((*tmp_voice_it).key);
            if (tmp_music_it)
            {
                tmp_info = (*tmp_music_it).value;
            }
            tmp_info.the_voice_info = (*tmp_voice_it).value;
            tmp_rv.add_pair((*tmp_voice_it).key,tmp_info);
        }
        tmp_voice_it = tmp_voice_it.get_item(tmp_point,gt);
    }

    return tmp_rv;
}


map<score_point,music_info>
    extract_music_from_tune_point(tune& the_tune)
{
    map<score_point,music_info> tmp_rv;
    map<score_point,music_info>::iterator tmp_music_it;
    map<score_point,chord_info>::iterator tmp_chord_it = 
        the_tune.my_music.least();
    // First add the chord info to the map.
    while (tmp_chord_it)
    {
        music_info tmp_info;
        tmp_info.the_chord_info = (*tmp_chord_it).value;
        tmp_rv.add_pair((*tmp_chord_it).key,tmp_info);
        tmp_chord_it = tmp_chord_it.get_item((*tmp_chord_it).key,gt);
    }
    
    // Now add the measure features.  If there is already a 
    // chord info at this point, make sure we retain it.
    map<score_point,measure_feature>::iterator tmp_measure_it = 
        the_tune.my_measures.least();
    
    while (tmp_measure_it)
    {
        music_info tmp_info;
        tmp_music_it = 
            tmp_rv.get_item((*tmp_measure_it).key);
        if (tmp_music_it)
        {
            tmp_info = (*tmp_music_it).value;
        }
        tmp_info.the_measure_feature = (*tmp_measure_it).value;
        tmp_rv.add_pair((*tmp_measure_it).key,tmp_info);
        tmp_measure_it = 
            tmp_measure_it.get_item((*tmp_measure_it).key,gt);
    }

    map<score_point,voice_info>::iterator tmp_voice_it = 
        the_tune.my_voice_info.least();

    while (tmp_voice_it)
    {
        music_info tmp_info;
        tmp_music_it = 
            tmp_rv.get_item((*tmp_voice_it).key);
        if (tmp_music_it)
        {
            tmp_info = (*tmp_music_it).value;
        }
        tmp_info.the_voice_info = (*tmp_voice_it).value;
        tmp_rv.add_pair((*tmp_voice_it).key,tmp_info);
        tmp_voice_it = tmp_voice_it.get_item((*tmp_voice_it).key,gt);
    }

    return tmp_rv;
}

map<score_time,music_info>
    extract_music_from_tune_time(tune& the_tune)
{
    map<score_time,music_info> tmp_rv;
    map<score_point,music_info> tmp_map = 
        extract_music_from_tune_point(the_tune);

    map<score_point,music_info>::iterator tmp_it = 
        tmp_map.least();
    while (tmp_it)
    {
        tmp_rv.add_pair(as_score_time((*tmp_it).key),
                        (*tmp_it).value);
        tmp_it = tmp_it.get_item((*tmp_it).key,gt);
    }

    return tmp_rv;
}

tune& 
tune::operator=(const tune& o)
{
    index = o.index;
    title = o.title;
    copyright = o.copyright;
    my_music = o.my_music;
    my_measures = o.my_measures;
    my_voice_info = o.my_voice_info; 
    my_words = o.my_words;
    my_songwords = o.my_songwords;
    return *this;
}

tune::tune(const tune& o)
{
    index = o.index;
    title = o.title;
    copyright = o.copyright;
    my_music = o.my_music;
    my_measures = o.my_measures;
    my_voice_info = o.my_voice_info; 
    my_words = o.my_words;
    my_songwords = o.my_songwords;
}

media::media(voice_map_source* the_voice_map):
    my_voice_map_source(the_voice_map)
{
}

media::~media()
{
}

bool 
media::is_voice_turned_on(const score_point& the_voice)
{
    bool tmp_add_part = true;
    if (my_voice_map_source)
    {
        map<int,bool> tmp_voice_on_map = my_voice_map_source->get_voice_map();    
        map<int,bool>::iterator tmp_voice_on = tmp_voice_on_map.get_item(the_voice.voice);
        tmp_add_part = ((tmp_voice_on == 0) || (*tmp_voice_on).value);
    }
    return tmp_add_part;
}


void 
media::present(const string& the_string,const score_point& the_voice)
{

    if (is_voice_turned_on(the_voice))
    {
        my_tune.my_words.add_pair(the_voice,the_string);
    }
}

void
media::present(const tune& the_tune)
{
    my_tune.copyright = the_tune.copyright;
    my_tune.index = the_tune.index;
    my_tune.title = the_tune.title;
    my_tune.my_voice_info = the_tune.my_voice_info;
    my_tune.my_songwords = the_tune.my_songwords;
}

void
media::present(chord_info& the_chord,const score_point& the_voice)
{
    if (is_voice_turned_on(the_voice))
    {
        render_music_feature(the_chord,the_voice);
    }
}

void 
media::present(measure_feature& the_feature,const score_point& the_voice)
{
    if (is_voice_turned_on(the_voice))
    {
        render_measure_feature(the_feature,the_voice);
    }
}

void 
media::map_voices_names()
{
    map<int,bool> tmp_voice_map;
    if (my_voice_map_source)
    {
        tmp_voice_map = my_voice_map_source->get_voice_map();
    }
    map<score_point,voice_info>::iterator tmp_voice_pair = 
        my_tune.my_voice_info.least();
    while (tmp_voice_pair)
    {
        voice_info& tmp_info = (*tmp_voice_pair).value;
        score_point tmp_point = (*tmp_voice_pair).key;
        string tmp_string((long)tmp_info.my_index);
        my_voice_name_map.add_pair(tmp_info.my_index,tmp_string);
        tmp_point = score_point(0,0,tmp_point.voice + 1);
        tmp_voice_pair = tmp_voice_pair.get_item(tmp_point,gteq);
    }

    // If the voice map was populated previously, repopulate it.
    // Otherwise, don't repopulate it.
    if (tmp_voice_map.get_size() != my_voice_name_map.get_size())
    {
        map<int,string>::iterator tmp_voice_name_it = 
            my_voice_name_map.least();
        while (tmp_voice_name_it)
        {
            tmp_voice_map.add_pair((*tmp_voice_name_it).key,true);
            tmp_voice_name_it = 
                tmp_voice_name_it.get_item((*tmp_voice_name_it).key,gt);
        }
    }

}

voice_info
media::get_voice_info(const score_point& the_voice)
{
    map<score_point,voice_info>::iterator tmp_voice_pair = 
        my_tune.my_voice_info.get_item(the_voice,lteq);
    voice_info tmp_info;
    if (tmp_voice_pair != 0)
        {
        tmp_info = (*tmp_voice_pair).value;
        }

    return tmp_info;
}

void 
media::populate_repeat_info()
{
	score_point tmp_score;
	tmp_score.measure = 1;
    map<score_point,measure_feature>::iterator tmp_it = 
        my_tune.my_measures.get_item(tmp_score,gteq);
    repeat_info tmp_info;
    tmp_info.my_measure = tmp_info.my_ending_start_measure = 
        tmp_info.my_ending_number = 0;
    // Look through all the measures and figure out what is going on
    // with repeats.
    while (tmp_it)
    {
        tmp_score = (*tmp_it).key;
        measure_feature tmp_feature = (*tmp_it).value;

        // If this is a repeat sign or a double bar, then it must be the
        // end of the last ending.
        if ((tmp_feature.end_type() == measure_feature::repeat_last) ||
            (tmp_feature.end_type() == measure_feature::double_bar) ||
            (tmp_feature.end_type() == measure_feature::repeat_last_and_next))
        {
            if ((tmp_info.my_ending_number != 0) &&
                (tmp_info.my_ending_start_measure != 0))
            {
                tmp_info.my_measure = tmp_score.measure;
                my_repeat_info.add_pair(tmp_info.my_ending_start_measure,
                                        tmp_info);
            }
            tmp_info.my_ending_number = 0;
            tmp_info.my_ending_start_measure = 0;
        }

        // If the start bar has a start of end ending and the next
        // ending starts on the next bar, add the start ending to the
        // list with the end where the next one starts.  e.g.
        //    ____________ ____________
        //     1          | 2          |
        //    ------------|-------------
        //    -----------.|-------------
        //    -----------.|--------------
        //    ------------|--------------
        //    ------------|--------------
        //
        if ((tmp_feature.nth_ending_end() > 0) &&
            (tmp_feature.nth_ending_start() > 0))
        {
            tmp_info.my_ending_start_measure = tmp_score.measure;
            tmp_info.my_ending_number = tmp_feature.nth_ending_start();
            tmp_info.my_measure = tmp_score.measure;
            my_repeat_info.add_pair(tmp_info.my_ending_start_measure,tmp_info);
            tmp_info.my_ending_start_measure = tmp_info.my_measure + 1;
            tmp_info.my_ending_number = tmp_feature.nth_ending_end();
        }
        else if (tmp_feature.nth_ending_end() > 0)
        {
            // If this ending is less than the last one, we missed the
            // last ending.  This could happen if the second ending doesn't
            // have a double bar after it.  In this case just put the ending
            // after the next measure.
            if ((tmp_info.my_ending_number > 0) &&
				(tmp_feature.nth_ending_end() <= tmp_info.my_ending_number))
            {
                tmp_info.my_measure = tmp_score.measure;
                my_repeat_info.add_pair(tmp_info.my_ending_start_measure + 1,
                                        tmp_info);
            }
            tmp_info.my_ending_start_measure = tmp_score.measure + 1;
            tmp_info.my_ending_number = tmp_feature.nth_ending_end();
        }
		else if (tmp_feature.nth_ending_start() > 0)
		{
            // Same deal here.
            if ((tmp_info.my_ending_number > 0) &&
				(tmp_feature.nth_ending_start() <= tmp_info.my_ending_number))
            {
                tmp_info.my_measure = tmp_score.measure;
                my_repeat_info.add_pair(tmp_info.my_ending_start_measure + 1,
                                        tmp_info);
            }
            tmp_info.my_ending_start_measure = tmp_score.measure;
            tmp_info.my_ending_number = tmp_feature.nth_ending_start();
		}

        tmp_it = my_tune.my_measures.get_item(tmp_score,gt);
    }

    // If the last nth ending didn't match at all, just put the end of the
    // ending one measure after it started.
    if (tmp_info.my_ending_number > 0)
    {
        tmp_info.my_measure = tmp_info.my_ending_start_measure + 1;
        my_repeat_info.add_pair(tmp_info.my_measure,
                                tmp_info);
    }
}

int
media::count_voices()
{
    map<int,int> voice_map;
    score_point tmp_first;
    map<score_point,chord_info>::iterator tmp_it = 
        my_tune.my_music.get_item(tmp_first,gteq);
    while (tmp_it)
    {
        score_point tmp_point = (*tmp_it).key;
        voice_map.add_pair(tmp_point.voice,tmp_point.voice);
        tmp_it = my_tune.my_music.get_item(tmp_point,gt);
    }

    return ((voice_map.get_size() == 0) ? 1 : voice_map.get_size());
}

void 
tune_chooser::add_tune(const tune& the_tune)
{
    // The assumption here is that the entries of tunes
    // will always be mapped to their offset from the
    // first tune (i.e. indexed from 0).
    int tmp_index = my_tunes.get_size();
    is_dirty = true;
    my_tunes.add_pair(tmp_index,the_tune);
    my_current_index = tmp_index;
}

void 
tune_chooser::clear_tunes()
{
    is_dirty = true;
    int tmp_current = 0;
    tunemap::iterator tmp_it = 
        my_tunes.get_item(tmp_current,gteq);

    while (tmp_it)
    {
        tune& tmp_tune = (*tmp_it).value;
        tmp_tune.my_music.clear();
        remove_all(tmp_tune.my_songwords);
        tmp_tune.my_voice_info.clear();
        tmp_tune.my_words.clear();
        tmp_tune.my_measures.clear();
        tmp_it = my_tunes.get_item((*tmp_it).key,gt);
    }
}

tune& 
tune_chooser::operator[](int the_index)
{
    tunemap::iterator tmp_it = 
        my_tunes.get_item(the_index);
    if (tmp_it)
    {
        return (*tmp_it).value;
    }

    throw "No tune at this index!";
}

int 
tune_chooser::get_size() const
{
    return my_tunes.get_size();
}

tune&
media_source::get_tune(int the_index)
{
    my_current_tune = the_index;
    tune& tmp_tune = get_current_tune();
    return tmp_tune;
}

void 
media_source::clear_tunes()
{
	my_tunes->clear_tunes();
}

void 
media_source::remove_all_tunes()
{
    my_tunes->remove_all_tunes();
}

voice_info 
media_source::get_voice_info(const score_point& the_point)
{
    tune tmp_tune = get_tune(my_current_tune);
    map<score_point,voice_info>::iterator tmp_it = 
        tmp_tune.my_voice_info.get_item(the_point,lteq);

    if (tmp_it)
    {
        return (*tmp_it).value;
    }

	return voice_info();
}

void
media_source::change_voice_info(const score_point& the_point,
                               const string& the_title,
                               const string& the_copyright,
                               int the_index,
                               const voice_info& the_info)
{
    // Get the tune indicated by the index, or create a new one if we 
    // are on a new tune
	if (my_tunes->get_size() > 0)
	{
		tune& tmp_tune = get_tune(the_index);
        tmp_tune.title = the_title;
        tmp_tune.copyright = the_copyright;
        tmp_tune.index = the_index;
        tmp_tune.my_voice_info.add_pair(the_point,the_info);
	}
}

media_source::media_source(tune_chooser& the_tunes):
my_current_voice(1),my_current_tune(0),
    my_tunes(&the_tunes)
{
    tune tmp_tune;
    my_tunes->clear_tunes();
	//my_tunes->add_tune(tmp_tune);
}

void 
media_source::get_next_non_rest(int& the_measure,fraction& the_beat)
{
    tune& tmp_tune = get_current_tune();
    score_point tmp_key = score_point(the_measure,the_beat,my_current_voice);
    map<score_point,chord_info>::iterator tmp_it = 
        tmp_tune.my_music.get_item(tmp_key,gteq);

    while (tmp_it)
    {
        chord_info tmp_info = (*tmp_it).value;
        if (tmp_info[0].is_rest() == false)
        {
            the_measure = (*tmp_it).key.measure;
            the_beat = (*tmp_it).key.beat;
            break;
        }

        tmp_key = (*tmp_it).key;
        tmp_it = tmp_tune.my_music.get_item(tmp_key,gt);
    }
}

tune& 
media_source::get_current_tune()
{
	return (*my_tunes)[my_current_tune];
}

void
media_source::add_music_feature(chord_info& the_feature,int the_measure,const fraction& the_beat)
{
    tune& tmp_tune = (get_current_tune());
    score_point tmp_point(the_measure,the_beat,my_current_voice);
    tmp_tune.my_music.add_pair(tmp_point,the_feature);
}

void
media_source::add_measure_feature(measure_feature& the_feature,int the_measure,const fraction& the_beat)
{
    tune& tmp_tune = (get_current_tune());
    score_point tmp_point(the_measure,the_beat,my_current_voice);
    tmp_tune.my_measures.add_pair(tmp_point,the_feature);
}

void 
media_source::set_line_break(int the_measure)
{
    tune& tmp_tune = (get_current_tune());
    measure_feature tmp_feature;
    score_point tmp_point(the_measure,fraction(0),my_current_voice);
    map<score_point,measure_feature>::iterator tmp_it = 
        tmp_tune.my_measures.get_item(tmp_point,lt);
    if (tmp_it)
    {
        tmp_feature = (*tmp_it).value;
        tmp_point = (*tmp_it).key;
    }

    tmp_feature.line_break(true);
    tmp_tune.my_measures.add_pair(tmp_point,tmp_feature);
}

void
media_source::add_songwords(const string& the_word)
{
    tune& tmp_tune = (get_current_tune());
    add_at_end(tmp_tune.my_songwords,the_word);
}

void 
media_source::add_words(const string& the_word,int& the_measure,fraction& the_beat)
{
    // Make sure we're not under a rest
    get_next_non_rest(the_measure,the_beat);
    
    tune& tmp_tune = (get_current_tune());
    score_point tmp_point(the_measure,the_beat,my_current_voice);
    map<score_point,chord_info>::iterator tmp_it = 
        tmp_tune.my_music.get_item(tmp_point,gteq);

    if (tmp_it)
    {
        chord_info& tmp_info = (*tmp_it).value;
        tmp_point = (*tmp_it).key;

        // If we have advanced a measure, indicate that.
        if (the_measure != tmp_point.measure)
        {
            the_measure = tmp_point.measure;
            the_beat = 0;
        }
        the_beat += tmp_info.get_duration();

        // Only add actual words to the list, otherwise we skip these notes.
        if (the_word.length() > 0)
        {
            tmp_info.set_words(the_word);
        }
    }

    get_next_non_rest(the_measure,the_beat);
}

media_source::~media_source()
{
}

void 
media_source::set_voice(int the_voice)
{
    my_current_voice = the_voice;
}

}
