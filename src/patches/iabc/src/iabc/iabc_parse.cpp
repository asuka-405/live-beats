/*
 * iabc_parse.cpp - Invokes the parser and pre-processor to parse an abc file.
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
#include "iabc/iabc_parse.h"
#include "iabc/array.cpp"
#include "iabc/list.cpp"
#include "iabc/message_box.h"
#include "iabc/percentage_meter.h"
#include "iabc/map.cpp"

extern bool globalUserAbortFlag;
namespace iabc
{
;

abc_parseable_unit::abc_parseable_unit(parser_state_info& the_info,
               const string& the_current_voice,
               const string& the_input,
               int the_line_number,
               measure_or_header& the_rule):
my_line(the_input),my_start_info(the_info),
    my_start_voice(the_current_voice),
    my_end_voice(the_current_voice),
    my_line_number(the_line_number),my_match(false)
{
    parser_state::get_instance().set_current_voice(the_current_voice);
    match_line(the_rule);
    my_modified_time = time(0);
}

bool
abc_parseable_unit::check_match(const string& the_line,
                 parser_state_info& the_info,
                    string& the_start_voice,
                       time_t the_time_modified)
{
    bool tmp_rv = false;
    if (my_modified_time >= the_time_modified)
    {
        tmp_rv = true;
    }
    else if ((the_info == my_start_info) &&
             (my_start_voice == the_start_voice) &&
             (the_line == my_line))
    {
        tmp_rv = true;
    }

    if (tmp_rv == true)
    {
        the_start_voice = my_end_voice;
        the_info = my_end_info;
        global_parser_event_log.log_event(my_line.access_char_array(),
                                          my_line.length(),
                                          parser_event_data::match_parse_unit);
    }

    return tmp_rv;
}

void abc_parseable_unit::add_ref() {++my_ref;}
void abc_parseable_unit::remove_ref()
{
    --my_ref;
    if (my_ref.as_int() == 0)
    {
        delete this;
    }
}

bool
abc_parseable_unit::advance_array(array<chord_info_array>& the_array,
                                  int& the_chord_index,
                                  int& the_measure_index)
{
    bool tmp_rv = true;
    // If this is the end of a measure, but there are more measures,
    // advance the measure and reset the index into the measure.
    if ((the_array.get_size() >= the_measure_index) &&
        (the_array[the_measure_index].get_size() > the_chord_index + 1))
    {
        ++the_chord_index;
    }
    // If this is not the end of the measure, advance the measure.
    else if (the_array.get_size() > the_measure_index + 1)
    {
        ++the_measure_index;
        the_chord_index = 0;
    }
    // Otherwise we're all out of music, return false.
    else
    {
        tmp_rv = false;
    }
    return tmp_rv;
}

array<chord_info_array>
abc_parseable_unit::put_chords_into_array()
{
    array<chord_info_array> tmp_measures(0,5);
    list<measure_rule_info>::iterator tmp_match_rules =
        my_match_rules.first();

    int i,j;
    while (tmp_match_rules)
    {
        measure_rule_info tmp_info = (*tmp_match_rules);

        // If this is a bar mark, advanced the measure.
        chord_info_array tmp_current;
        for (i = 0;i < tmp_info.my_beam_groups.get_size();++i)
        {
            chord_info_array tmp_array = tmp_info.my_beam_groups[i];
            for (j = 0;j < tmp_array.get_size();++j)
            {
                // We only associate words with non-rests.
                if (tmp_info.my_beam_groups[i][j][0].is_rest() == false)
                {
                    tmp_current.add_to_end(tmp_array[j]);
                }
            }
        }
        tmp_measures.add_to_end(tmp_current);
        tmp_match_rules.next();
    }

    return tmp_measures;
}

void
abc_parseable_unit::match_line(measure_or_header& the_rule)
{
    int tmp_index = 0;
    array<string> tmp_lines = my_line.split("\n");
    int tmp_line_index = 0;
    string tmp_line;
    while (tmp_line_index < tmp_lines.get_size())
    {
        tmp_line = tmp_lines[tmp_line_index];
        tmp_index = 0;
        bool tmp_match = the_rule.match(tmp_line,tmp_index);
        while ((tmp_match) && (globalUserAbortFlag == false))
        {
            measure_rule_info tmp_measure_info;
            header_rule_info tmp_header_info;
            words_rule_info tmp_words_info;

            measure_or_header::match_type tmp_match_type =
                the_rule.get_match_type();

            // Each parseable element has some things in common: they all
            // potentially affect the parser state, and they all are for a
            // certain voice, and they are all one of the match types.
            // Depending on the match type, the measure rule or header rule
            // is updated too.
            add_at_end(my_match_types,tmp_match_type);
            add_at_end(my_parser_info,parser_state::get_current_voice_info());
            add_at_end(my_voice_info,parser_state::get_instance().get_current_voice());

            // Update the ending info.  We use this to skip parsing this line and just
            // recreating the state information at the end if the starting state and
            // line match.
            my_end_info = parser_state::get_current_voice_info();
            my_end_voice = parser_state::get_instance().get_current_voice();
            switch (tmp_match_type)
            {
            case measure_or_header::measure_match:
                tmp_measure_info = the_rule.get_measure_rule();
                add_at_end(my_match_rules,tmp_measure_info);
                break;
            case measure_or_header::header_match:
                tmp_header_info = the_rule.get_header_rule();
                add_at_end(my_header_rules,tmp_header_info);
                break;
            case measure_or_header::line_end_match:
            case measure_or_header::continue_line_match:
                tmp_words_info = the_rule.get_words_rule();
                if (tmp_words_info.is_match == true)
                {
                    add_at_end(my_words_rules,tmp_words_info);
                    parse_words(tmp_words_info.my_value);
                }
                break;
            default:
                throw("Unknown match type\n");
                break;
            }
            the_rule.reset();
            if ((tmp_match_type == measure_or_header::line_end_match) ||
                (tmp_match_type == measure_or_header::continue_line_match))
                break;

            tmp_match = the_rule.match(tmp_line,tmp_index);
        }
        ++tmp_line_index;
        tmp_index = 0;
    }
}

chord_info_array 
abc_parseable_unit::get_chord_array_from_measure(array<chord_info_array>& the_measures,
                                              int& the_index)
{
    chord_info_array tmp_rv;
    tmp_rv = the_measures[the_index];
    while ((tmp_rv.get_size() == 0) &&
           (the_index < the_measures.get_size()))
    {
        the_index++;
	    tmp_rv = the_measures[the_index];
    }
    return tmp_rv;
}

void
abc_parseable_unit::parse_words(const string& the_string)
{
    int i;

    // Put the chords into an array, so that its easy to figure out
    // which chord goes with which word.
    array<chord_info_array> tmp_chords = put_chords_into_array();
    int tmp_array_index = 0;
    int tmp_measure_index = 0;

    // The variable that holds the word itself.
    string tmp_word;
    bool tmp_music_left = (tmp_chords.get_size() > 0) ? true : false;

    // Skip the w: part, no one wants to see that
    for (i = 2;
          (i < the_string.length()) && (tmp_music_left == true);
          ++i)
    {
        // From the abc draft standard.
        //$ -       break between syllables within a word
        if (the_string[i] == '-')
        {
            tmp_word += "-";
            if (tmp_word.length())
            {
                chord_info_array tmp_measure = get_chord_array_from_measure(tmp_chords,tmp_measure_index);
                if (tmp_measure.get_size() > tmp_array_index)
                {
                    tmp_measure[tmp_array_index].set_words(tmp_word);
                }
                tmp_music_left = advance_array(tmp_chords,tmp_array_index,tmp_measure_index);
                tmp_word.clear();
            }
        }
        //$ |       advance to next bar
        else if (the_string[i] == '|')
        {
            // ordinarily add_words will advance to the next beat.  '|'
            // overrides that, I think.
            chord_info_array tmp_measure = get_chord_array_from_measure(tmp_chords,tmp_measure_index);
            if (tmp_measure.get_size() > tmp_array_index)
            {
                tmp_measure[tmp_array_index].set_words(tmp_word);
            }
            tmp_music_left = advance_array(tmp_chords,tmp_array_index,tmp_measure_index);
            tmp_word.clear();
        }
        //$ *       one note is skipped (i.e. * is equivalent to a blank syllable)
        else if ((the_string[i] == '*') ||
                 (the_string[i] == '_'))
        {
            if (tmp_word.length() > 0)
            {
                chord_info_array tmp_measure = get_chord_array_from_measure(tmp_chords,tmp_measure_index);
                if (tmp_measure.get_size() > tmp_array_index)
                {
                    tmp_measure[tmp_array_index].set_words(tmp_word);
                }
                tmp_music_left = advance_array(tmp_chords,tmp_array_index,tmp_measure_index);
                tmp_word.clear();
            }
            ++i;
            // Now skip to the next non-space
            while ((the_string[i] <= ' ') &&
                   (the_string[i] >= 0) &&
                   (i < the_string.length())) ++i;
            // Make sure a character doesn't get dropped.
            --i;
            chord_info_array tmp_measure = get_chord_array_from_measure(tmp_chords,tmp_measure_index);
            if (tmp_measure.get_size() > tmp_array_index)
            {
                tmp_measure[tmp_array_index].set_words(tmp_word);
            }
            tmp_music_left = advance_array(tmp_chords,tmp_array_index,tmp_measure_index);
        }
        //$ ~       appears as a space but connects syllables each side into one
        else if (the_string[i] == '~')
        {
            chord_info_array tmp_measure = get_chord_array_from_measure(tmp_chords,tmp_measure_index);
            if (tmp_measure.get_size() > tmp_array_index)
            {
                tmp_measure[tmp_array_index].set_words(tmp_word);
            }
            tmp_music_left = advance_array(tmp_chords,tmp_array_index,tmp_measure_index);
            tmp_word.clear();
            tmp_word = " ";
        }
        //$ \-      appears as - sign in output.
        else if (the_string[i] == '\\')
        {
            ++i;
            tmp_word += the_string[i];
        }
        // skip spaces, but process the last word if need be.
        else if ((the_string[i] <= ' ') && (the_string[i] >= 0))
        {
            if (tmp_word.length() > 0)
            {
                chord_info_array tmp_measure = get_chord_array_from_measure(tmp_chords,tmp_measure_index);
                if (tmp_measure.get_size() > tmp_array_index)
                {
                    tmp_measure[tmp_array_index].set_words(tmp_word);
                }
                tmp_music_left = advance_array(tmp_chords,tmp_array_index,tmp_measure_index);
                tmp_word.clear();
            }
            // tmp_word += " ";
        }
        else
        {
            tmp_word += the_string[i];
        }
    }
}

void
iabc_parse::deref_parseable_units()
{
    list<abc_parseable_unit*>::iterator tmp_it =
        my_parseable_units.first();
    while (tmp_it)
    {
        abc_parseable_unit* tmp_unit = (*tmp_it);
        tmp_unit->remove_ref();
        tmp_it.delete_current();
    }
}

iabc_parse::~iabc_parse()
{
    deref_parseable_units();
}

iabc_parse::iabc_parse(preprocess& the_input_buffer,
                       media_source& the_source,
                       map<int,score_point>*  the_lines_to_score_points):
        my_measure(0),
        my_source(the_source),
        my_last_measure_for_words(1),
    my_input_buffer(&the_input_buffer),my_current_tune(0),
    my_start_line_of_tune(0),
    my_lines_to_score_points(the_lines_to_score_points)
{
}

void
iabc_parse::handle_measure_match(measure_rule_info& the_measure,
                                 int& the_measure_index,
                                 fraction& the_beat)
{
    // Handle any inline headers we find
    score_point tmp_score_point(the_measure_index,the_beat,my_source.get_voice());

    for (int i = 0;i < the_measure.my_beam_groups.get_size(); ++i)
    {
        chord_info_array& tmp_chords = the_measure.my_beam_groups[i];
        for (int j = 0;j < tmp_chords.get_size(); ++ j)
        {
            chord_info tmp_info = tmp_chords[j];
            my_source.add_music_feature(tmp_info,the_measure_index,the_beat);
            the_beat += tmp_info.get_duration();
        }
    }
}

int
iabc_parse::get_measure_from_voice(int the_voice)
{
    int tmp_rv = 1;
    map<int,int>::iterator tmp_it = my_voice_measure.get_item(the_voice);
    if (tmp_it)
    {
        tmp_rv = (*tmp_it).value;
    }

    return tmp_rv;
}

void
iabc_parse::save_measure_for_voice(int the_measure,int the_voice)
{
    my_voice_measure.add_pair(the_voice,the_measure);
}

void
iabc_parse::update_measure_line_pair(const score_point& the_score_point,long the_line)
{
    if (my_lines_to_score_points)
    {
        map<int,score_point>::iterator tmp_it =
            (*my_lines_to_score_points).get_item(the_line);
        if (tmp_it)
        {
            score_point tmp_old = (*tmp_it).value;
            // If this is the lowest measure for this line, add it
            if (tmp_old.measure > the_score_point.measure)
            {
                (*my_lines_to_score_points).add_pair(the_line,the_score_point);
                global_cursor_event_log.log_event(cursor_event_data::store_text_point,
                                                  the_line,
                                                  the_score_point.measure,
                                                  the_score_point.voice,
                                                  LOG_FRACTION(the_score_point.beat));
            }
        }
         // else if this is a new line, add it
        else
        {
            (*my_lines_to_score_points).add_pair(the_line,
                                                   the_score_point);
            global_cursor_event_log.log_event(cursor_event_data::store_text_point,
                                              the_line,
                                              the_score_point.measure,
                                              the_score_point.voice,
                                              LOG_FRACTION(the_score_point.beat));
        }
    
    }
}

bool
iabc_parse::is_word(const string& the_string)
{
    int tmp_index = 0;
    bool tmp_rv = false;
    while ((tmp_index < the_string.length()) &&
            (the_string[tmp_index] == ' '))

    {
        ++tmp_index;
    }
    if ((tmp_index + 2 < the_string.length()) &&
        (the_string[tmp_index] == 'w') &&
        (the_string[tmp_index + 1] == ':'))
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

string
iabc_parse::get_next_line(preprocess& the_file,int& the_line,int the_end_line)
{
    string tmp_rv;

    // If this is the first time through, there's nothin in the buffer.
    if (the_file.get_line_number() == my_start_line_of_tune)
    {
        tmp_rv = the_file.get_next_line();
        the_line = the_file.get_line_number();
    }
    // We read ahead one line.  If we read a line from the file that we
    // haven't used, use that.
    else
    {
        tmp_rv = my_last_line;
        the_line = the_file.get_line_number();
        my_last_line.clear();
    }

    // If there are any lines left, put them in the read-ahead buffer
    if ((the_file.get_lines_left() > 0) &&
        (the_file.get_line_number() <= the_end_line))
    {
        my_last_line = the_file.get_next_line();
    }

    // If the next line is a word header, glom it onto this line.
    while ((is_word(my_last_line)) &&
           (my_last_line.length() > 0))
    {
        tmp_rv += my_last_line;
        my_last_line.clear();
        if (the_file.get_lines_left() > 0)
        {
            my_last_line = the_file.get_next_line();
        }
    }

    return tmp_rv;
}

bool
iabc_parse::is_there_text_left(preprocess& the_file,int the_end_line)
{
    if ((the_file.get_line_number() < the_end_line) ||
        (my_last_line.length() > 0))
    return true;
    return false;
}

void
iabc_parse::construct_parseable_units(preprocess& the_file,
                                      measure_or_header& the_rule,int tmp_end_line)
{
    // This is a list of parseable units that we assembled on the last run through
    list<abc_parseable_unit*>::iterator tmp_parseable_units =
        my_parseable_units.first();

    // Mark the time that we last parsed.
    time_t tmp_parse_time = time(0);

    // Reset the parser state to the start state.
    parser_state::get_instance().reset();
    string tmp_current_voice =
        parser_state::get_instance().get_current_voice();
    parser_state_info& tmp_parser_info =
        parser_state::get_instance().get_current_voice_info();

    // We assemble a new list of parseable units each time through.  If none
    // of the line changed, we just copy the list.  Then we throw out all of
    // the units that don't match lines, since they represent lines that were deleted.
    list<abc_parseable_unit*> tmp_new_list;
    list<abc_parseable_unit*>::iterator tmp_new_iterator;
    list<abc_parseable_unit*>::iterator tmp_cursor =
        my_parseable_units.first();

    // Read in all the lines of the tune
    while (is_there_text_left(the_file,tmp_end_line) == true)
    {
        int tmp_line_number;
        string tmp_line = get_next_line(the_file,tmp_line_number,tmp_end_line);
        bool tmp_match = false;

        list<abc_parseable_unit*>::iterator tmp_old_cursor =
            tmp_cursor;
        // Compare the current line to any saved parseable units.  See if there
        // is a match so that we don't have to reparse.
        while ((tmp_match == false) &&
               (tmp_cursor))
        {
            abc_parseable_unit* tmp_current = (*tmp_cursor);
            global_parser_event_log.log_event(tmp_line.access_char_array(),
                                              tmp_line.length(),
                                              parser_event_data::compare_parse_unit);
            if (tmp_current->check_match(tmp_line,
                                         tmp_parser_info,
                                         tmp_current_voice,
                                         tmp_parse_time))
            {
                tmp_match = true;
                tmp_current->add_ref();
                add_at_end(tmp_new_list,tmp_current);
            }
            tmp_cursor.next();
        }

        if (tmp_match == false)
        {
            tmp_cursor = tmp_old_cursor;
        }

        if (tmp_match == false)
        {
            abc_parseable_unit* tmp_unit = new abc_parseable_unit(
                          parser_state::get_instance().get_current_voice_info(),
                                   parser_state::get_instance().get_current_voice(),
                                   tmp_line,
                                   tmp_line_number,
                                   the_rule);
            tmp_unit->add_ref();
            add_at_end(tmp_new_list,tmp_unit);
        }
    }

    deref_parseable_units();
    my_parseable_units = tmp_new_list;
}

bool
iabc_parse::parse_measure(int& the_measure_index,
                          bool& the_read_music,
                          abc_parseable_unit& the_unit)
{
    int tmp_index = 0;
    the_read_music = false;
    bool tmp_first_in_line = true;
    fraction tmp_beat;

    // When we parse, we end up with lots of independent lists of
    // musical things that we need to put into the proper musical order.
    list<parser_state_info>::iterator tmp_parser_info =
        the_unit.my_parser_info.first();
    list<string>::iterator tmp_voice_info =
        the_unit.my_voice_info.first();
    list<header_rule_info>::iterator tmp_header_rules =
        the_unit.my_header_rules.first();
    list<measure_rule_info>::iterator tmp_measure_rules =
        the_unit.my_match_rules.first();
    list<words_rule_info>::iterator tmp_words_rules =
         the_unit.my_words_rules.first();
    list<measure_or_header::match_type>::iterator tmp_match_types =
        the_unit.my_match_types.first();

    while ((tmp_match_types) && (tmp_parser_info) && (tmp_voice_info))
    {
        // If we matched a measure, process the measure.
        my_last_match_type = (*tmp_match_types);
        parser_state_info tmp_current_parser_info = (*tmp_parser_info);
        string tmp_current_voice = (*tmp_voice_info);
        if (my_last_match_type == measure_or_header::measure_match)
        {
            the_read_music = true;

            voice_info tmp_voice_info = tmp_current_parser_info.my_voice_info;

            // If we have switched voices, the measure may also have
            // changed.  Save the current voice information and switch
            // to the new value.
            update_measure_from_voice(the_measure_index,
                                      tmp_voice_info.my_index,tmp_voice_info);

            // Keep track of the text row/measure mapping for GUI magic.
            update_measure_line_pair(score_point(the_measure_index,
                                                 0,
                                                 tmp_voice_info.my_index),
                                                 the_unit.my_line_number);

            // The first match in a line, update the next measure for words.
            if (tmp_first_in_line == true)
            {
                my_last_measure_for_words = the_measure_index;
            }
            // First render the notes and stuff in the measure
            measure_rule_info tmp_measure = (*tmp_measure_rules);
            handle_measure_match(tmp_measure,the_measure_index,tmp_beat);

            // Then render the measure itself
            score_point tmp_score_point = score_point(the_measure_index,
                tmp_beat,
                my_source.get_voice());
            measure_feature tmp_mf = tmp_measure.my_measure_feature;
            my_source.add_measure_feature(tmp_mf,the_measure_index,tmp_beat);
            ++the_measure_index;

            // We used to increment the current tune if this is a tune index, but now
            // we don't
            tmp_beat = fraction(0,1);
            tmp_measure_rules.next();
        }
        else if (my_last_match_type == measure_or_header::header_match)
        {
            // We matched a header.  Update the media with whatever the
            // global change was.  Some headers are actually handled by
            // the parser internally (such as duration) so we ignore those.
            // (and of course others just haven't been implemented yet)
            header_rule_info tmp_header = (*tmp_header_rules);
            handle_header_change(the_measure_index,tmp_header,the_unit.my_line_number);
            tmp_header_rules.next();
        }
        // We've ended this line.  Go to the calling function, update the GUI and
        // process all the stuff we've just read.
        else if (my_last_match_type == measure_or_header::line_end_match)
        {
            if (tmp_words_rules)
            {
                words_rule_info tmp_words = (*tmp_words_rules);
                if (tmp_words.is_match)
                {
                    // There may have been words in the last measure in the line.
                    // parse_words(tmp_words.my_value);
                    tmp_words_rules.next();
                }
            }
            break;
        }
        else if (my_last_match_type == measure_or_header::continue_line_match)
        {
            // This line may have had some words.
            if (tmp_words_rules)
            {
                words_rule_info tmp_words = my_measure->get_words_rule();
                if (tmp_words.is_match)
                {
                    // There may have been words in the last measure in the line.
                    // parse_words(tmp_words.my_value);
                    tmp_words_rules.next();
                }
            }
            // Otherwise just keep going
        }
        // I don't think you can get here
        else
        {
            throw("match of unknown type!");
        }
        tmp_first_in_line = false;
        tmp_match_types.next();
        tmp_parser_info.next();
        tmp_voice_info.next();
    }

#if 0
    if (tmp_match == false)
    {
        string tmp_string = (string)"Syntax line " + (string)
            (long)the_file.get_line_number() + (string)" char " +
            (string)(long)tmp_index;

        message_box::display(tmp_string);
    }
    return tmp_match;
#endif
    return true;
}

void
iabc_parse::handle_header_change(int& the_measure_index,
                                 header_rule_info& the_header,
                                 int the_line_number)
{
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();
    voice_info tmp_info;
    tmp_info.my_index = my_source.get_voice();

    // If this is one of the headers we know how to parse
    if (the_header.my_unknown.length() == 0)
    {
        // If we've changed voices, this must be the V: voice
        // change line.  save the old voice_info and update the
        // new voice_info with all the parser state stuff.
        if (tmp_info.my_index != tmp_parser_info.my_voice_info.my_index)
        {
            save_measure_for_voice(the_measure_index,tmp_info.my_index);
            the_measure_index = get_measure_from_voice(tmp_parser_info.my_voice_info.my_index);

        }

        int tmp_voice = tmp_parser_info.my_voice_info.my_index;
        // If we've had an inline key change or meter change, record that here.
        my_source.change_voice_info(score_point(the_measure_index,0,tmp_voice),
                                   my_current_title,
                                   my_current_copyright,
                                   my_current_tune,
                                   tmp_parser_info.my_voice_info);
        my_source.set_voice(tmp_voice);
        tmp_info = tmp_parser_info.my_voice_info;

        // Update title part of the tune
        my_current_title += the_header.my_title;
        if (the_header.my_copyright.length())
        {
            my_current_copyright = the_header.my_copyright + (string)'\n';
        }
        string tmp_songwords = the_header.my_songwords;
        if (tmp_songwords.length() > 0)
        {
            my_source.add_songwords(tmp_songwords);
        }
    }
    else
    {
        ;
    }
}

void
iabc_parse::update_measure_from_voice(int& the_measure_index,
                                      int the_current_voice,
                                      voice_info& the_info)
{
    int tmp_new_voice = the_current_voice;
    int tmp_old_voice = my_source.get_voice();
    score_point tmp_start_point = score_point(the_measure_index,0,tmp_new_voice);
    voice_info tmp_last =
        my_source.get_voice_info(tmp_start_point);

    // If the voices have changed, save the state of the current voice.
    if (tmp_new_voice != tmp_old_voice)
    {
        save_measure_for_voice(the_measure_index,tmp_old_voice);
        the_measure_index = get_measure_from_voice(tmp_new_voice);
        my_source.set_voice(tmp_new_voice);
        score_point tmp_score_point(the_measure_index,0,tmp_new_voice);
        voice_info tmp_old_info =
            my_source.get_voice_info(tmp_score_point);
        if (tmp_old_info != the_info)
        {
            my_source.change_voice_info(tmp_score_point,
                                        my_current_title,my_current_copyright,
                                        my_current_tune,the_info);
        }
    }
    // If they havnen't changed, we may have changed voice info stuff,
    // so check that now
    else if (tmp_last != the_info)
    {
        my_source.change_voice_info(tmp_start_point,
                                    my_current_title,my_current_copyright,
                                    my_current_tune,the_info);
    }
}

map<int,iabc_parse::tune_index_info>
iabc_parse::quick_parse()
{
    index_rule tmp_index_rule;
    title_rule tmp_title;
    preprocess& tmp_file = *my_input_buffer;
    tmp_file.set_line(0);
    int tmp_total_lines = tmp_file.get_lines_left();
    percentage_meter* tmp_meter =
    percentage_meter::create();
    tmp_meter->set_message("Scanning Line:");
    int tmp_tune_index = 0;
    tune_index_info tmp_current_tune;

    tmp_current_tune.the_title = "<untitled>";
    tmp_current_tune.the_start_line = tmp_current_tune.the_end_line = 0;

    bool tmp_read_one_index = false;
    while (tmp_file.get_lines_left() > 0)
    {
        string tmp_string = tmp_file.get_next_line();
        tmp_meter->set_percentage(tmp_file.get_line_number());
        if (tmp_string.length() > 2)
        {
            if ((tmp_string[0] == 'X') && (tmp_string[1] == ':'))
            {
                if (tmp_read_one_index == true)
                {
                    tmp_current_tune.the_end_line = tmp_file.get_line_number() - 1;
                    my_tunes.add_pair(tmp_tune_index,tmp_current_tune);
                    ++tmp_tune_index;
                    tmp_current_tune.the_title = "<untitled>";
                    tmp_current_tune.the_start_line = tmp_current_tune.the_end_line + 1;
                }
                tmp_read_one_index = true;
            }
            else if ((tmp_string[0] == 'T') && (tmp_string[1] == ':'))
            {
                if (tmp_current_tune.the_title == "<untitled>")
                {
                    tmp_current_tune.the_title =
                        tmp_string.mid(2,tmp_string.length() - 2);
                }
            }
        }
    }

    tmp_current_tune.the_end_line = tmp_file.get_line_number();
    my_tunes.add_pair(tmp_tune_index,tmp_current_tune);
    percentage_meter::destroy(*tmp_meter);
    return my_tunes;
}

void
iabc_parse::reset()
{
    my_voice_measure.clear();
    my_songwords.clear();
    my_current_title = "";
    my_current_copyright = "";
    my_measure = 0;
    my_last_measure_for_words = 1;
}

bool
iabc_parse::parse(int the_tune_index)
{
    bool tmp_parse_all = false;
    int tmp_start_line = 0;
    int tmp_end_line = 0;
    bool tmp_add_line_break = false;
    my_current_tune = the_tune_index;
    // we store the last line we have parsed, so get rid of this one which
    // is left over from the last time we parsed a line (file may have changed).
    my_last_line = "";

    // Indicate no error.
    message_box::display("");

    // We keep track of some global things while parsing.  Clear any left
    // over from the last time.
    reset();
    parser_state::get_instance().reset();
    parser_state::get_instance().the_current_tune = the_tune_index;

    map<int,tune_index_info>::iterator tmp_it =
        my_tunes.get_item(the_tune_index);
    if (tmp_it)
    {
        tune_index_info tmp_tune = (*tmp_it).value;
        tmp_start_line = tmp_tune.the_start_line;
        tmp_end_line = tmp_tune.the_end_line;
        my_start_line_of_tune = tmp_start_line;
    }
    else
    {
        tmp_parse_all = true;
    }

    int tmp_measure = 1;
    int tmp_old_measure = tmp_measure;
    my_last_match_type = measure_or_header::measure_match;
    nth_ending_rule* my_ending_rule = new nth_ending_rule;
    delete my_ending_rule;
    optional_whitespace my_space;
    meter_specifier_rule my_meter_rule;
    tempo_specifier_rule my_tempo_rule;
    array<chord_info_array> my_beams;
    null_rule my_null;
    measure_feature my_measure_feature;

    my_measure = new measure_or_header;

    preprocess& tmp_file = *my_input_buffer;
    int tmp_total_lines = 0;
    if (tmp_parse_all == true)
    {
        tmp_end_line = tmp_file.get_lines_left();;
    }
    else
    {
        tmp_file.set_line(tmp_start_line);
    }

    tmp_total_lines = tmp_end_line - tmp_start_line;
    percentage_meter* tmp_meter =
    percentage_meter::create();
    tmp_meter->set_message("Parsing");
    construct_parseable_units(tmp_file,*my_measure,tmp_end_line);
    if (my_lines_to_score_points)
    {
        (*my_lines_to_score_points).clear();
    }

    list<abc_parseable_unit*>::iterator tmp_parseable_units =
         my_parseable_units.first();
    while ((tmp_parseable_units) &&
        (globalUserAbortFlag == false) &&
        (tmp_meter->is_cancelled() == false))
    {
        abc_parseable_unit* tmp_current_unit = (*tmp_parseable_units);

        bool tmp_match = parse_measure(tmp_measure,tmp_add_line_break,*tmp_current_unit);
        tmp_meter->set_percentage(100.0 * (double)(tmp_current_unit->my_line_number - tmp_start_line)/
                                  (double)tmp_total_lines);

        // If we matched something other than a measure, don't register
        // a new measure with the media
        if (tmp_add_line_break == true)
        {
            my_source.set_line_break(tmp_measure);
            tmp_old_measure = tmp_measure;
        }
        tmp_parseable_units.next();
    }

    percentage_meter::destroy(*tmp_meter);
    delete my_measure;

    // This cleans up memory a bit.
    parser_state::get_instance().reset();
    my_measure = 0;
    return true;
}

}

