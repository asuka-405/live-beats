/*
 * iabc_parse.h - Invokes the parser and pre-processor to parse an abc file.
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
#ifndef iabc_iabc_parse_h
#define iabc_iabc_parse_h
#include "iabc/media.h"
#include "iabc/abcrules.h"
#include "iabc/media.h"
#include "iabc/preprocess.h"
#include "iabc/string.h"
#include "iabc/mutex.h"

// FILE: iabc_parse.h
// SUBSYSTEM: parser
// DESCRIPTION:
// Drive the parser rules that define the abc language, at least as iabc
// understands them.
namespace iabc
{
;
// Its most convenient if the parser just grabs all of the linked lists and
// goes through them, since there are quite a few of them.  So we make the
// parser a friend of the parseable units.
class iabc_parse;

// CLASS: abc_parseable_unit
// DESCRIPTION:
// Keep track of a line or so of an abc file and the musical
// information that came out of it from the parser.  This can keep us
// from having to re-parse an unchanged line of abc.
class abc_parseable_unit
{
public:
    friend class iabc_parse;

    // METHOD: ctor
    // DESCRIPTION:
    // Parse this line of text, update the parser state and store all of the
    // musical information.
    abc_parseable_unit(parser_state_info& the_info,
                       const string& the_current_voice,
                       const string& the_input,
                       int the_line_number,
                       measure_or_header& the_rule);

    // METHOD: check_match
    // DESCRIPTION:
    // return true if this string matches the string that I was
    // created with,  or if the timestamp is earlier than the
    // last time I was created.
    // If this returns true, the_info and the_start_voice will contain
    // the voice and parser_info that were present at the end
    // of this line when it was first parsed.
    bool check_match(const string& the_line,
                     parser_state_info& the_info,
                     string& the_start_voice,
                           time_t the_time_modified);

    // METHOD: clear_match_text
    // DESCRIPTION:
    // Indicate that check_match() has returned false.
    void clear_match_text(){my_match = false;};

    void add_ref();
    void remove_ref();

private:
    // ATTRIBUTE: void match_line(measure_or_header& the_rule)
    // DESCRIPTION:
    // This is called by the constructor and actually performs the work
    // of parsing the line of code we were fed when we were created.
    void match_line(measure_or_header& the_rule);

    // METHOD: get_chord_array_from_measure
    // DESCRIPTION:
    // Get the next non-empty measure.  This is called by the method that puts the
    // words to the music.
    chord_info_array get_chord_array_from_measure(array<chord_info_array>& the_measures,
                                                  int& the_index);

    // METHOD: parse_words
    // DESCRIPTION:
    // Parse the w: header.
    void parse_words(const string& the_string);

    // METHOD: put_chords_into_array()
    // DESCRIPTION:
    // Put the chords into an array (they are stored in list format)
    // so that we may put the words to them.
    array<chord_info_array> put_chords_into_array();

    // METHOD: advance_array
    // DESCRIPTION:
    // Also used when putting words with the chord infos.
    // RETURNS:
    // true if the array still has stuff left in it.
    bool advance_array(array<chord_info_array>& the_array,int& the_chord,int& the_measure);

    // ATTRIBUTE: string my_line
    // DESCRIPTION:
    // The line that I was created with
    string my_line;

    // ATTRIBUTE:
    // DESCRIPTION:
    // Keep track of the first measure of the last line, which is where we start putting the
    // words.
    int my_last_measure_for_words;

    // ATTRIBUTE: array<measure_or_header::match_type> my_match_types
    // DESCRIPTION:
    // The array of all the matches.  The match type will indicate
    // whether the current match type is a header, measure, or whatever
    list<measure_or_header::match_type> my_match_types;
    list<parser_state_info> my_parser_info;
    list<string> my_voice_info;

    // ATTRIBUTE: list<header_rule_info> my_header_rules
    // DESCRIPTION:
    // The header rules that we created from parsing our string
    list<header_rule_info> my_header_rules;

    // ATTRIBUTE: list<measure_rule_info> my_match_rules
    // DESCRIPTION:
    // the match rules that we created from parsing our string.  The
    // parser state info could change during any
    list<measure_rule_info> my_match_rules;
    list<words_rule_info> my_words_rules;

    // ATTRIBUTE: parser_state_info my_start_info
    // DESCRIPTION:
    // Each rule is matched in the context of a parser state.  Keep track of the
    // parser state at the start of this line, if the line is same and the parser
    // state is the same then the line is unchanged so all the misical information is
    // also unchanged.
    parser_state_info my_start_info;
    parser_state_info my_end_info;
    string my_start_voice;
    string my_end_voice;

    // ATTRIBUTE: bool my_match
    // DESCRIPTION:
    // Indicates that we have a match.
    bool my_match;

    // ATTRIBUTE: int my_line_number
    // DESCRIPTION:
    // This is the line number of the text file that was matched.  We keep
    // track of this so that we can track the music with the text in the GUI.
    int my_line_number;

    // ATTRIBUTE: time_t my_modified_time
    // DESCRIPTION:
    // Don't even bother looking at the line if the buffer hasn't changed since
    // my_modified_time.
    time_t my_modified_time;
    atomic_counter my_ref;

};

// CLASS: iabc_parse
// DESCRIPTION:
// This class invokes the pre-processor to get a line of text
// and then adds the music to the supplied media_source object.
class iabc_parse
{
public:
    // CLASS: tune_index_info
    // DESCRIPTION:
    // Keep the index in to the file for quickly finding a tune.
    class tune_index_info
    {
    public:
        int the_start_line;
        int the_end_line;
        string the_title;
    };

    // METHOD: iabc_media(ctor)
    // DESCRIPTION:
    // Store the file name that we will open later and try to parse.
    iabc_parse(preprocess& the_input_buffer,
               media_source& the_source,
               map<int,score_point>*  the_lines_to_score_points=0);

    ~iabc_parse();

    // METHOD: get_start_line_of_tune
    // DESCRIPTION:
    // retunr the starting line of the tune that we just parsed.
    long get_start_line_of_current_tune() const {return my_start_line_of_tune;};

    // METHOD: parse(media& the_media)
    // DESCRIPTION:
    // parse the input file and tell the media what we've found so the
    // music can be realized somehow.  Get the tune from the file, based
    // in the tune at the given index.
    bool parse(int the_tune_index);

    // METHOD: quick_parse
    // DESCRIPTION:
    // Do a parse of just the T: and the X: headers
    map<int,tune_index_info> quick_parse();
private:

    // METHOD: parse_measure(medai&,int&,preprocess&)
    // DESCRIPTION:
    // get the next measure and deal with it from the abc file.  Some
    // measures are split across several lines so send the file in too,
    // and keep track of the measure number.
    // ARGS:
    // int the_measure - the last measure we read in
    // bool read_in_music - true if we've read in some music and we need
    //           a line break
    // preprocess& the_file - the file we're reading in.
    bool parse_measure(int& the_measure,bool& read_in_music,abc_parseable_unit& the_unit);

    // ATTRIBUTE: string get_next_line(preprocess& the_file)
    // DESCRIPTION:
    // Get the next line of text.  This can actually be more than
    // one line of text, if there are words that are associated with this
    // line of abc stuff.  Also get the line number of the file so that we
    // can associate it with this spot in the music.
    string get_next_line(preprocess& the_file,int& the_line_number,int the_end_line);

    // METHOD: is_there_text_left
    // DESCRIPTION:
    // return true if there is text left to read in the current tune.
    bool is_there_text_left(preprocess& the_file,int tmp_end_line);

    // ATTRIBUTE: bool is_word(const string&)
    // DESCRIPTION:
    // Because of the way we associate words with chord_info structures, we have to
    // parse the word header (w:) outside if the state machine rules as well as
    // in the word rule itself.
    bool is_word(const string&);

    // ATTRIBUTE: void construct_parseable_units(preprocess& the_file,measure_or_header& the_rule,int tmp_end_line)
    // DESCRIPTION:
    // Read through all the lines of the file, and feed each line to a new
    // parseable unit for parsing.  But, if we already have a parseable unit in the list that
    // has parsed this line then don't bother reparsing.
    void construct_parseable_units(preprocess& the_file,measure_or_header& the_rule,int tmp_end_line);

    // METHOD: update_measure_line_pair
    // DESCRIPTION:
    // We want to make the lowest measure in a line be the key for that line
    // for playing and indicating the place in the music.
    void update_measure_line_pair(const score_point& the_measure,long the_line);

    // METHOD: handle_measure_match(measure_rule&,media&,int&,fraction&)
    // DESCRIPTION:
    // We've found a measure's worth of music.  Handle it and update the
    // current measure and beat.
    void handle_measure_match(measure_rule_info& the_measure,
                              int& the_measure_index,
                              fraction& the_beat);

    // METHOD: handle_header_change(media&,header_rule&,const score_point&)
    // DESCRIPTION:
    // We have received a header at the given score point.  Record it and make any
    // required changes.  Also reset the_measure_index to 1 if we are on a new
    // tune.
    void handle_header_change(int& the_measure_index,
        header_rule_info& the_header,
        int the_line_number);

    // METHOD: get_measure_from_voice
    // DESCRIPTION:
    // Return the last measure we read from the_voice.  If this is the
    // first one, we return 0.
    int get_measure_from_voice(int the_voice);
    void save_measure_for_voice(int the_measure,int the_voice);

    // METHOD: update_measure_from_voice
    // DESCRIPTION:
    // We have just changed voices, so update the new measure since it may
    // be different than the last voice was.
    void update_measure_from_voice(int& the_measure,
                                   int the_current_voice,
                                   voice_info& the_info);


    void deref_parseable_units();

    measure_or_header::match_type my_last_match_type;
    measure_or_header* my_measure;
    media_source& my_source;

    string my_current_title;
    string my_current_copyright;
    int my_current_tune;

    // ATTRIBUTE: string my_last_line
    // DESCRIPTION:
    // Keep track of the last line that we read in from the scanner, if there are
    // words in this line then we combine it with earlier lines into one parseable
    // unit.
    string my_last_line;

    // ATTRIBUTE: my_last_line_number
    // DESCRIPTION:
    // Keep track of the last line number that we read in from the user.
    int my_last_line_number;

    // ATTRIBUTE: my_start_line_of_tune
    // DESCRIPTION:
    // Keep track of the line in the text buffer that a song starts on
    // so that we can scroll to there if we like.
    long my_start_line_of_tune;

    // ATTRIBUTE: my_voice_measure
    // DESCRIPTION:
    // When we switch voices in the parse, we need to keep track of what
    // measure index we were on last time.  Keep that information here.
    map<int,int> my_voice_measure;

    // ATTRIBUTE: my_songwords
    // DESCRIPTION:
    // map of words that go below the staff, to the point in the music where they go.
    map<score_point,string> my_songwords;

    // ATTRIBUTE: my_tunes
    // DESCRIPTION:
    // Tunes that we get in a quick parse of an abc file.
    map<int,tune_index_info> my_tunes;

    // ATTRIBUTE:
    // DESCRIPTION:
    // Keep track of the first measure of the last line, which is where we start putting the
    // words.
    int my_last_measure_for_words;

    // ATTRIBUTE: my_text_buffer
    // DESCRIPTION:
    // text buffer for input
    preprocess* my_input_buffer;

    // METHOD: reset
    // DESCRIPTION:
    // Reset all the internal state things that I keep track of for a fresh parse of
    // a file.
    void reset();

    // ATTRIBUTE:
    // DESCRIPTION:
    // Keep track of which line corresponds to which line of
    // text for GUI magic later on.
    map<int,score_point>* my_lines_to_score_points;

    list<abc_parseable_unit*> my_parseable_units;
};
}
#endif



