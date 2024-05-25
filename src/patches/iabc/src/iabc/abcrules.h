/*
 * abcrules.h - The rules for the iabc parser.
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
#ifndef iabc_abcrules_h
#define iabc_abcrules_h
#include "iabc/parse.h"
#include "iabc/note_info.h"
#include "iabc/music_info.h"

#define MAX_MEASURE_DEPTH 200

namespace iabc
{
;
// FILE:
// These classes are the rules that define the abc language.  See the
// parse file for what the base classes are; see the implementation
// file for the BNF-like meaning of these classes.
// SUBSYSTEM: parser

// CLASS: parser_state_info
// DESCRIPTION:
// This is a static class (utility class) that keeps track of the state
// of some things in the parser that are global.  Like how many '(' we've
// matched so far and what the note duration is.  Usually this state lasts
// throught out the course of a single file.
//
// It's good to reset this once in awhile.
class parser_state_info
{
public:
    parser_state_info();

    // These are all the things that are kept track
    // of about the current voice.
    voice_info my_voice_info;
    string the_copyright;

    // These are all things that are kept track of
    // as a running parser state, but aren't used
    // outside of the parser.
    bool the_tie_state;
    bool the_last_tie_state;

    // Set this to true if we match a hornpipe rule
    // as a 'post' note rule, which happens sometimes.
    string the_post_hornpipe;
    int the_paren_count;
    bool the_curlybrace_state;
    int the_staves;

    fraction the_duration;
    pitch::accidental_map the_accidentals;

    bool operator==(const parser_state_info& o);
};

class parser_state
{
public:

    // These are used to parse key expressions.  They use
    // up lots of strings so we keep them around during
    // the parsing phase but eliminate them soon afterwards.
    static array<regexp*> the_key_expressions;
    parser_state(){reset();};
    static void gc();
    int the_current_tune;
    int the_last_tune;
    bool the_history_field;
    void reset();
    static parser_state& get_instance();
    void set_current_voice(const string& the_current_voice);
    string get_current_voice() {return the_current_voice;};
    int get_current_voice_index();
    bool knows_about_voice(const string& the_voice);

    // Description: returns the parser state info for the voice.
    // This returns the real deal, so the caller can change the
    // return value and be persistent.
    static parser_state_info& get_current_voice_info();
private:
    // This is a hack so that I can understand poorly-formed
    // tunes that were written for barfly.
    bool have_specified_voice_1;
    string the_current_voice;
    static parser_state_info& get_info_for_voice(const string& the_voice);
    int the_last_current_voice;
    // Voices must first be introduced with the v: line, keep
    // track of which voices we already know about.
    map<string,parser_state_info> the_known_voices;
};

// CLASS: duration_rule
// DESCRIPTION:
// There are 3 ways to represent
// a duration.  One is as a fraction, e.g. "1/4" for a 1/4 note.
class duration_rule:public compound_rule
{
public:
    duration_rule();

    // try to match one of the abc forms of a note duration
    virtual void convert(const string& the_string,int& the_index);

    fraction get_value(){return my_fraction;};
    void reset();
protected:
    virtual non_terminal create_complete_rule();

protected:
    fraction my_fraction;
    number_rule my_numerator;
    literal my_slash;
    repeat_rule my_slashes;
    number_rule my_denominator;
    optional_whitespace my_space;
    null_rule my_null;
};

// CLASS: duration_specifier_rule
// DESCRIPTION:
// This is the whole duration specifier in abc.  It also
// contains the default duration, which all duration
// rules use in case the duration is not specified.
// Note that the default duration for a note is handled internally
// in this abc parser.  When a duration is read the value is
// computed in terms of the current default, which is handled
// in the parser.
class duration_specifier_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    duration_specifier_rule();
    virtual bool match(const string& the_string,int& the_index);
    fraction get_value() const {return get_default();};
    static fraction get_default() {return parser_state::get_current_voice_info().the_duration;};
    void reset();
private:
    virtual int convert(const string &the_string);
    duration_rule my_duration;
};

// CLASS: index rule
// DESCRIPTION:
// X: \d+
// describes an index field.  This maps titles to tunes.
class index_rule:public terminal
{
public:
    index_rule();
    virtual void convert(const string &the_string);
    int get_value() const {return parser_state::get_instance().the_current_tune;};
    static int get_last_index() {return parser_state::get_instance().the_last_tune;};
private:
};

// CLASS: copyright_rule
// DESCRIPTION:
// C: By Aaron Newman (c) 2003
class copyright_rule:public terminal
{
public:
    copyright_rule():terminal("^ *C: *(.+)"){};
    virtual void convert(const string &the_string);
    string get_value() const {return my_value;};
    void reset(){my_value = "";terminal::reset();};
private:
    string my_value;
};

// CLASS: history_rule
// DESCRIPTION:
// H: This class was written last Tuesday
// while Aaron was supposed to be doing the
// dishes.
class history_rule:public terminal
{
public:
    history_rule():terminal("^ *H: *(.+)"){};
    virtual bool match(const string& the_string,int& the_index);
    virtual void convert(const string &the_string);
    string get_value() const {return my_value;};
    void reset(){my_value = "";terminal::reset();};
private:
    string my_value;
};

// CLASS: title rule
// DESCRIPTION:
// T: the title rule.  Describes a title.  Add to the current
// title if we are still in the same tune (which you can tell by
// looking at the last index)
class title_rule:public terminal
{
public:
    title_rule():terminal("^ *T: *(.+)"){};
    virtual void convert(const string &the_string);
    string get_value() const {return my_title;};
    void reset(){my_title = "";terminal::reset();};
private:
    string my_title;
};

// CLASS: meter_header_rule
// DESCRIPTION:
// The M: header rule
class meter_header_rule:public terminal
{
    // The derived class provides the expression in the costructor,
    // and then defines the 'convert' function.  The base class will
    // call this method if there is a match.
public:
    meter_header_rule():terminal("^ *M:"){};

    // We don't really use the value but we save it anyway.
    virtual void convert(const string &the_string){my_value = the_string;};
private:
    string my_value;
};

// CLASS: tempo_header_rule
class tempo_header_rule:public terminal
{
public:
    tempo_header_rule():terminal("^ *Q:"){};

    virtual void convert(const string& the_string){};
};

// CLASS: meter_specifier_rule
// DESCRIPTION:
// The meter specifier in ABC.  This also acts
// as the source of the current meter in ABC.  ALso,
// I frequently call 'meter' 'time signature'.
class meter_specifier_rule:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    meter_specifier_rule();

    virtual void convert(const string &the_string, int &the_index);
    virtual non_terminal create_complete_rule();

    // This rule also cannot be reset.
    virtual void reset();

    static fraction
        get_default_time_signature(){return
            parser_state::get_current_voice_info().my_voice_info.my_time_signature;};
private:
    number_rule my_numerator;
    number_rule my_denominator;
    meter_header_rule my_header;
    literal my_slash; // = "/";
    optional_whitespace my_whitespace;
    literal my_common;
    literal my_cut;
    literal my_other_cut;
};

class tempo_specifier_rule:public compound_rule
{
public:
    tempo_specifier_rule();
    ~tempo_specifier_rule();
    virtual void convert(const string &the_string, int &the_index);
    virtual non_terminal create_complete_rule();

    // This rule also cannot be reset.
    virtual void reset();
private:
    number_rule my_tempo;
    number_rule my_numerator;
    number_rule my_denominator;
    tempo_header_rule my_header;
    literal my_slash;
    literal my_equals;
    optional_whitespace my_whitespace;
};

// CLASS: Voice Change Rule
// DESCRIPTION:
// This is the voice change that can be at the start of a
// line of music.
class voice_change_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    voice_change_rule();
    ~voice_change_rule();
    virtual bool match(const string &the_string, int &the_index);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();

    string get_value() const {return my_voice;};
    bool is_match() const {return my_is_match;};
private:
    bool my_is_match;
    string my_voice;
    regexp* my_expression;
};

// CLASS: letter_note_rule
// DESCRIPTION:
// The letter note rule is part of the pitch.
class letter_note_rule:public terminal
{
public:
    letter_note_rule():terminal("[a-g]|[A-G]"),my_octave(0){};
    virtual void convert(const string &the_string);
    void reset();
    pitch::letter get_value(){return my_letter;};
    int get_octave() const{return my_octave;};
protected:
    pitch::letter my_letter;
    int my_octave;
};

// DESCRIPTION:
// An octave as defined by '' or ,,.  We'll need
// to add the octave header somewhere too.
class octave_rule:public terminal
{
public:
    octave_rule():terminal(",+|'+"),my_octave(0){};
    virtual void convert(const string &the_string)
    {
        if (the_string[0] == ',')
        {
            my_octave = (-1) * the_string.length();
        }
        else if (the_string[0] == '\'')
        {
            my_octave = (1) * the_string.length();
        }
    }
    int get_value() const {return my_octave;};
    virtual void reset(){my_octave = 0;terminal::reset();};
private:
    int my_octave;
};

// CLASS: part_header_rule
// DESCRIPTION:
// A part is a section of music called out by a letter or number
// above the music.
class part_header_rule:public rule
{
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
public:
    part_header_rule(){};
    ~part_header_rule(){};
    virtual void reset();
    // return true if the_string[the_index] matches this rule
    virtual bool match(const string &the_string, int &the_index);
    string get_value();
private:
    string my_value;
};

// CLASS: key_header_rule
// DESCRIPTION:
// The entire key header rule in one take.  We need to know the key
// before we know the pitch since we will adjust the accidental
// accordingly.
class key_header_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    virtual bool match(const string &the_string, int &the_index);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();
public:
    key_header_rule();
    // METHOD: convert
    // DESCRIPTION:
    // Get the key the user wants, and return the number
    // of characters that should be skipped to get to the
    // next thing.
    virtual int convert(const string &the_string);
    pitch get_value() const {return my_pitch;};
    virtual ~key_header_rule();
private:
    // METHOD: parse_extra_stuff
    // DESCRIPTION:
    // Get the key=value pairs that can trail a key
    // declaration.
    int parse_extra_stuff(const string& the_string);

    // METHOD: construct_expressions_if_required
    // DESCRIPTION:
    // We occasionally delete the key regular expressions since
    // the take lots of memory.  Construct them before use.
    void construct_expressions_if_required();
    class key_map
    {
    public:
        const char* expression;
        pitch::letter the_letter;
        pitch::accidental the_accidental;
    };
    static key_map the_key_map[];
    pitch my_pitch;
    regexp my_bc;
    regexp my_tc;
    regexp my_transpose;
};

// CLASS: accidental_rule
// DESCRIPTION:
// The accidental that can preceed a note.
class accidental_rule:public terminal
{
public:
    accidental_rule():terminal("[\\^\\^]|\\^|[__]|_|="),
        my_accidental(pitch::natural)
    {};
    pitch::accidental get_value(){return my_accidental;};
    virtual void convert(const string &the_string);

    virtual void reset()
    {
        my_accidental = pitch::natural;
        terminal::reset();
    };

private:
    pitch::accidental my_accidental;
};

// CLASS: pitch_rule
// DESCRIPTION:
// A pitch is a letter note, possibly combined with
// an octave and an accidental.
class pitch_rule:public compound_rule
{
public:
    void reset();
    pitch_rule();
    virtual non_terminal create_complete_rule();
    virtual void convert(const string &the_string, int &the_index);
    pitch get_value() {return my_pitch;};
private:
    accidental_rule my_accidental;
    null_rule my_null;
    octave_rule my_octave;
    letter_note_rule my_letter;
    pitch my_pitch;
};

// CLASS: bar_line_rule
// DESCRIPTION:
// The end of line, which could be a repeat, bar line, etc.
class bar_line_rule:public terminal
{
public:

    bar_line_rule();
    virtual void convert(const string &the_string);
    measure_feature::measure_feature_enum
         get_value() const{return my_bar_type;};
private:
    measure_feature::measure_feature_enum my_bar_type;
};

class nth_ending_rule:public compound_rule
{
protected:
    // METHOD: create_complete_rule
    // DESCRIPTION:
    // We create the compound rule using or and 'and' only once during the
    // lifetime of the object to speed up the parse - its wasteful to keep
    // creating and deleting instances.
    virtual non_terminal create_complete_rule();

public:
    nth_ending_rule();
    virtual void convert(const string &the_string,int& the_index);
    int get_repeat_value() const{return my_repeat_number;};
    measure_feature::measure_feature_enum get_bar_value() const{return my_end_type;};
    void pre_match(const string& the_string,int the_index);
    void reset();
private:
    bar_line_rule my_bar_line;
    number_rule my_number;
    literal my_square;
    null_rule my_null;
    int my_repeat_number;
    measure_feature::measure_feature_enum my_end_type;
};

class tie_rule:public terminal
{
public:
    tie_rule():terminal(" *-"){};
    virtual void convert(const string& the_string)
    {
        parser_state::get_current_voice_info().the_tie_state = true;
    }
    static bool get_and_reset()
    {
        parser_state_info& tmp_parser_info =
            parser_state::get_current_voice_info();
        bool tmp_rv = tmp_parser_info.the_tie_state;
        tmp_parser_info.the_last_tie_state =
            tmp_parser_info.the_tie_state;
        tmp_parser_info.the_tie_state = false;
        return tmp_rv;
    }
    static bool get_and_reset_last()
    {
        parser_state_info& tmp_parser_info =
            parser_state::get_current_voice_info();
        bool tmp_rv = tmp_parser_info.the_last_tie_state;
        tmp_parser_info.the_last_tie_state = false;
        return tmp_rv;
    }
private:
};

class non_terminal_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    non_terminal_rule(const non_terminal& the_sub_rule):my_sub_rule(the_sub_rule){};

    virtual bool match(const string &the_string, int &the_index)
    {
        return non_terminal::match(the_string,the_index,my_sub_rule);
    }
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset() {my_sub_rule.reset_to_default();}
private:
    non_terminal my_sub_rule;
};

// CLASS: inline_header_rule
// DESCRIPTION:
// Return true if there is a match of a header inline a piece of
// music.
class inline_header_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    inline_header_rule();
    ~inline_header_rule();
    void create_complete_rule();
    virtual bool match(const string &the_string, int &the_index);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();
private:
    rule* my_array[128];
    non_terminal_rule* r1;
    non_terminal_rule* my_loser_nt;
    non_terminal_rule* r2;
    key_header_rule my_key;
    part_header_rule my_part;
    tempo_specifier_rule my_tempo;
    meter_specifier_rule my_meter;
    duration_specifier_rule my_duration;
    voice_change_rule my_voice_change;
    bool my_is_match;
    bool my_should_create;
    null_rule my_null;
    loser_rule my_loser_rule;
};

// CLASS: note_info_rule
// DESCRIPTION:
// A note_info is a pitch and a duration. If no duration
// is given the default one is used.
class note_info_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    note_info_rule();
    ~note_info_rule();
    void create_complete_rule();
    virtual void convert();
    virtual bool match(const string& the_string,int& the_index);
    // METHOD: matched_pitch
    // DESCRIPTION:
    // Sometimes the pitch was not changed but we received an inline
    // header instead.  Return false in this case.
    note_info get_value() const{return my_info;};
    void reset();
    bool is_matched() const {return my_is_match;};
private:
    bool my_is_match;
    bool my_should_create;
    duration_rule my_duration_rule;
    non_terminal_rule* r2;
    non_terminal_rule* r3;
    non_terminal_rule* r4;
    non_terminal_rule* my_loser_nt;
    pitch_rule my_pitch_rule;
    null_rule my_null;
    loser_rule my_loser_rule;
    note_info my_info;
    literal my_z;
    literal my_x;
    tie_rule my_tie_rule;
    rule* my_array[128];
};

// CLASS: multi_note_rule
// DESCRIPTION:
// many notes, glommed together.  Used to put together
// chords and also single notes.
class multi_note_rule:public iterative_rule
{
    // return true if the_string[the_index] matches this rule
public:
    typedef array<note_info> note_info_array;
    multi_note_rule();
    ~multi_note_rule();
    void create_complete_rule(int the_depth);
    void convert(int the_depth);
    note_info_array get_value() const {return my_notes;};
    void reset();
private:
    note_info_array my_notes;
    array<note_info_rule*> my_note_rules;
    null_rule my_null;
};

// CLASS: note_info_or_inline_header_rule
// DESCRIPTION:
// Both the header rule and the note info rule can be
// enclosed in [].  Figure out which here.
class multi_note_or_inline_header_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    multi_note_or_inline_header_rule();
    ~multi_note_or_inline_header_rule();
    void create_complete_rule();
    virtual bool match(const string &the_string, int &the_index);

    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();
    const inline_header_rule& get_header_rule() const {return my_header_rule;};
    const multi_note_rule& get_note_rule() const {return my_note_rule;};
private:
    inline_header_rule my_header_rule;
    multi_note_rule my_note_rule;
    rule* my_array[128];
    bool my_is_matched;
    bool my_should_create;
};

class embellishment_rule:public terminal
{
    // The derived class provides the expression in the costructor,
    // and then defines the 'convert' function.  The base class will
    // call this method if there is a match.
public:
    embellishment_rule();
    virtual void convert(const string &the_string);
    virtual void reset();
    chord_info::embellishment get_value() const {return my_embellishment;};
private:
    chord_info::embellishment my_embellishment;
};

// CLASS: chord_info_rule
// DESCRIPTION:
// A chord in abc is a single note or multiple notes within
// a pair of square brackets.
class chord_info_rule:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    chord_info_rule();
    ~chord_info_rule(){};
    virtual void convert(const string &the_string, int &the_index);
    virtual non_terminal create_complete_rule();
    chord_info get_value() const{return my_chord_info;};
    void reset();
private:
    chord_info my_chord_info;
    literal chord_start;
    literal chord_end;
    multi_note_or_inline_header_rule my_notes;
    note_info_rule my_note;
    null_rule my_null_rule;
};

typedef array<chord_info> chord_info_array;

// CLASS: hornpipe
// DESCRIPTION:
// The hornpipe rule is a funny little abc'ism.  The fraction
// produced by this rule is interpreted as follows:
// multiply the default duration by 1/2^#, where # is the
// number of < or >s.
// For < (>) Add that to the LHS(RHS) and subtract it from
// the RHS(LHS).  This usually (always?) results in a dotted rhythm
class hornpipe:public terminal
{
public:
    hornpipe():terminal("<+|>+"){};
    string get_value(){return my_value;};
    virtual void convert(const string &the_string);
    virtual void reset();
private:
    string my_value;
};

// CLASS: cparen_rule
// DESCRIPTION:
// For some reason abc allows a space between the closing note
// in a beam group and a closing paren.  Have a rule that does that.
class cparen_rule:public terminal
{
public:
    cparen_rule():terminal(" *\\)"){};
    void convert(const string& the_string){};
};

// CLASS: cparen_rule
// DESCRIPTION:
// For some reason abc allows a space between the closing note
// in a beam group and an opening paren.  Have a rule that does that.
class oparen_rule:public terminal
{
public:
    oparen_rule():terminal(" *\\( *"){};
    void convert(const string& the_string){};
};


// CLASS: dynamic_string
// DESCRIPTION:
// The dynamics that ABC knows about that can be displayed on the
// screen or via midi.
class dynamic_string:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    dynamic_string();
    ~dynamic_string();
    virtual void reset();
    virtual void convert(const string& the_string,int& the_index);
    chord_info::dynamic get_value() const{return my_value;};
private:
    non_terminal create_complete_rule();
    chord_info::dynamic my_value;
    literal my_ppp;
    literal my_pp;
    literal my_p;
    literal my_fff;
    literal my_ff;
    literal my_f;
    literal my_cresc_start;
    literal my_dim_start;
    literal my_cresc_end;
    literal my_dim_end;
};

class embellishment_string:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    embellishment_string();
    ~embellishment_string();
    virtual void reset();
    virtual void convert(const string& the_string,int& the_index);

    chord_info::embellishment get_value() const{return my_value;};
private:
    non_terminal create_complete_rule();
    chord_info::embellishment my_value;
    literal my_trill;
    literal my_lowermordent;
    literal my_mordent;
    literal my_pralltriller;
    literal my_accent;
    literal my_invertedfermata;
    literal my_tenuto;
    literal my_pizzicato;
    literal my_wedge;
    literal my_open;
    literal my_snap;
    literal my_turn;
    literal my_roll;
    literal my_shortphrase;
    literal my_mediumphrase;
    literal my_longphrase;
    literal my_upbow;
    literal my_downbow;
};

// CLASS:  bang_rule
// DESCRIPTION:
// some things are defined within the '!' fenceposts
// We glom these together under the bang rule.
class bang_rule:public compound_rule
{
    // METHOD: convert
    // DESCRIPTION:
    // We have found a match for the rule, which means we have matched a
    // expression in the language.  Do whatever that expression tells us to.
public:
    bang_rule();
    ~bang_rule();
    virtual void convert(const string &the_string, int &the_index);
    const dynamic_string& get_dynamics() const {return my_dynamics;};
    const embellishment_string& get_embellishment() const {return my_embellishment;};
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();
protected:
    // METHOD: create_complete_rule
    // DESCRIPTION:
    // We create the compound rule using or and 'and' only once during the
    // lifetime of the object to speed up the parse - its wasteful to keep
    // creating and deleting instances.

    virtual non_terminal create_complete_rule();

    literal my_open_bang;
    literal my_close_bang;
    dynamic_string my_dynamics;
    embellishment_string my_embellishment;
};

// CLASS: abc_quoted_string
// DESCRIPTION:
// abc allows space after quotes and before the chords they accompany.
// Handle it.
class abc_quoted_string:public compound_rule
{
public:
    abc_quoted_string();

    // METHOD: convert
    // DESCRIPTION:
    // We have found a match for the rule, which means we have matched a
    // expression in the language.  Do whatever that expression tells us to.
    virtual void convert(const string &the_string, int &the_index);
    // METHOD: create_complete_rule
    // DESCRIPTION:
    // We create the compound rule using or and 'and' only once during the
    // lifetime of the object to speed up the parse - its wasteful to keep
    // creating and deleting instances.

    string get_value(){return my_value;};
protected:
    virtual non_terminal create_complete_rule();
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
public:
    virtual void reset();
private:
    string my_value;
    optional_whitespace my_whitespace_rule;
    quoted_string_rule my_quoted_string_rule;
};

// Match the many combinations of how to make an xlet
class chord_xlet_prefix_or_oparen:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    chord_xlet_prefix_or_oparen();
    virtual ~chord_xlet_prefix_or_oparen();
    virtual bool match(const string &the_string, int &the_index);

    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();

    bool is_oparen() const {return my_is_oparen;};
    fraction get_unit_value() const{return my_unit_length;};
    int get_number_notes() const{return my_number_notes;};
    int get_xlet_value() const{return my_xlet_value;};
    bool is_match() const {return my_is_match;};
private:
    class xlet_map
    {
    public:
        typedef enum xlet_value
        {
            use_p = (int)'p',
            use_default = (int)'x',
            use_first = 2,
            use_second = 3
        } xlet_value;
        const char* my_expression;
        xlet_value p_value;
        xlet_value q_value;
        xlet_value r_value;
    };
    bool my_is_oparen;
    bool my_is_match;
    static xlet_map the_maps[];
    regexp** my_exp_array;
    int my_number_notes;
    int my_xlet_value;
    fraction my_unit_length;
};

class pre_chord_stuff:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    // Things we owe the base class
    virtual void create_complete_rule();
    virtual void reset();
    virtual bool match(const string& the_string,int& the_index);

    // Accessors once we've succeeded
    chord_xlet_prefix_or_oparen&
        get_oparen_rule() {return my_chord_xlet_prefix;};
    abc_quoted_string& get_quoted_string_rule(){return my_quoted_string_rule;};
    embellishment_rule& get_embellishment_rule(){return my_embellishment_rule;};
    literal& get_ocurly() {return my_ocurly;};
    literal& get_ccurly() {return my_ccurly;};
    hornpipe& get_hornpipe_rule() {return my_hornpipes;};
    bang_rule& get_bang_rule() {return my_bang_rule;};
    pre_chord_stuff();
    ~pre_chord_stuff();
    bool is_match() const {return my_is_match;};
private:
    bool my_should_create;
    bool my_is_match;
    hornpipe my_hornpipes;
    chord_xlet_prefix_or_oparen my_chord_xlet_prefix;
    abc_quoted_string my_quoted_string_rule;
    embellishment_rule my_embellishment_rule;
    bang_rule my_bang_rule;
    literal my_ocurly;
    literal my_ccurly;
    loser_rule my_loser;
    null_rule my_null;
    rule* my_array[128];
};

// CLASS: post_chord_stuff
// DESCRIPTION:
// This is the stuff that can occur between the notes in a chord
// or at the end of a chord.
class post_chord_stuff:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    post_chord_stuff();

    // Stuff we owe the base class
    virtual void create_complete_rule();
    virtual void reset();
    virtual bool match(const string& the_string,int& the_index);
    cparen_rule& get_cparen_rule() {return my_cparen_rule;};
    quoted_string_rule& get_quoted_string_rule(){return my_quoted_string_rule;};
    hornpipe& get_hornpipe() {return my_hornpipe;};
    bool get_ccurly(){return my_ccurly.is_match();};
    bool is_there_space(){return my_space.is_match();};
    bool is_match() const {return my_is_match;};
private:
    bool my_should_create;
    bool my_is_match;
    literal my_ccurly;
    hornpipe my_hornpipe;
    cparen_rule my_cparen_rule;
    quoted_string_rule my_quoted_string_rule;
    optional_whitespace my_space;
    loser_rule my_loser;
    rule* my_array[128];
};

typedef map<int,char> paren_map;

// CLASS: multi_chord_rule
// DESCRIPTION:
// This rule will match almost everything in legal abc that is
// expressed as multiple chords run together, except xlets.
// Note: here is where we start counting paren's for slurs/ties
class multi_chord_rule:public iterative_rule
{
    // return true if the_string[the_index] matches this rule
public:
    multi_chord_rule();
    virtual ~multi_chord_rule();
    chord_info_array get_value() const{return my_chords;};
    void reset();
    chord_xlet_prefix_or_oparen& get_xlet(int the_depth){return (*my_pre[the_depth]).get_oparen_rule();};
protected:
    void create_complete_rule(int the_depth);
    void convert(int the_depth);
private:
    array<chord_info_rule*> my_chord_array;
    array<pre_chord_stuff*> my_pre;
    array<post_chord_stuff*> my_post;
    int my_chord_depth;
    void handle_hornpipe(chord_info& tmpLHS,chord_info& tmpRHS,const string& the_value);
    chord_info_array my_chords;
    null_rule my_null;
};

// CLASS: voice_definition_rule
// DESCRIPTION:
// The V: command, which does many things like we said before
class voice_definition_rule:public terminal
{
public:
    voice_definition_rule();
    void convert(const string& the_string);
    string get_current_voice(){return parser_state::get_instance().get_current_voice();};

private:
    void normal_convert(const string& the_string,int the_voice);
    void barfly_convert(const string& the_string,int the_voice);
    regexp* find_barfly_match(const char* the_string,int the_length);
    nv_pair my_pair;
    regexp my_barfly_program;
    regexp my_barfly_bass;
    regexp my_barfly_volume;
};

// CLASS: voice_header
// DESCRIPTION:
// Matches a voice header.
class voice_header:public terminal
{
    // return true if the_string[the_index] matches this rule
public:
    voice_header();
    ~voice_header();
    void convert(const string& the_string);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
private:
};

// CLASS: voice_rule_all_inclusive
// DESCRIPTION:
// Depending on whether or not we're allowing text-based
// voice rules, proxy to the correct object.
class voice_rule_all_inclusive:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    voice_rule_all_inclusive();
    ~voice_rule_all_inclusive();
    virtual non_terminal create_complete_rule();
    virtual void convert(const string &the_string, int &the_index);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();
private:
    voice_header my_voice_header_rule;
    voice_definition_rule my_voice_definition_rule;
};

// CLASS: beam_group_rule
// DESCRIPTION:
// In abc a beam group is defined as any set of chords without a
// space.
class beam_group_rule:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    beam_group_rule();
    non_terminal create_complete_rule();
    chord_info_array get_value() const{return my_array;};
    void reset();
private:
    void convert(const string& the_string,int& the_index);
    optional_whitespace my_optional_space;
    whitespace my_required_space;
    multi_chord_rule my_chords;
    chord_info_array my_array;
    non_terminal my_space_rule;
    null_rule my_null;
};

class words_rule_info
{
public:
    string my_value;
    bool is_match;
};

// CLASS: words_rule
// DESCRIPTION:
// These are the words that actually go below the music.
class words_rule:public terminal
{
public:
    words_rule();
    void convert(const string& the_match);
    void reset();
    string get_value() const{return my_match;};
private:
    string my_match;
};

// CLASS: words_rule
// DESCRIPTION:
// These are the words that are written free-hand
// below the music.
class songwords_rule:public terminal
{
public:
    songwords_rule();
    void convert(const string& the_match);
    void reset();
    string get_value() const{return my_match;};
private:
    string my_match;
};

class measure_rule_info
{
public:
    array<chord_info_array> my_beam_groups;
    measure_feature my_measure_feature;
};

class measure_rule:public iterative_rule
{
    // return true if the_string[the_index] matches this rule
public:
    measure_rule();
    virtual ~measure_rule();
    virtual void convert(int the_iteration);
    measure_rule_info get_measure_rule_info() const;
    void reset();
private:
    void pre_match(const string& the_string,int the_index);
    virtual void create_complete_rule(int the_iteration);
    array<beam_group_rule*> my_beam_groups;
    nth_ending_rule my_ending_rule;
    optional_whitespace my_space;
    meter_specifier_rule my_meter_rule;
    tempo_specifier_rule my_tempo_rule;
    array<chord_info_array> my_beams;
    null_rule my_null;
    measure_feature my_measure_feature;
};

class clef_rule:public terminal
{
public:
    clef_rule():terminal("C:.+$"){};
    virtual void convert(const string &the_string){};
};

// A catch-all class for all the headers I haven't implemented yet.
class unknown_header_rule:public terminal
{
public:
    unknown_header_rule():terminal("^[A-z]:.+$"){};
    virtual ~unknown_header_rule();
    virtual void convert(const string &the_string)
    {
        my_match = the_string;
    };
    virtual void reset(){my_match = "";terminal::reset();};
    string get_value() const {return my_match;};
private:
    string my_match;
};

class header_rule_info
{
public:
    fraction my_default_duration;
    fraction my_time_signature;
    string my_unknown;
    string my_songwords;
    string my_copyright;
    int my_index;
    string my_title;
    string my_voice;
    pitch my_key;
};

// Header rule will match any one of the abc headers
class header_rule:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    virtual void convert(const string &the_string, int &the_index);
    virtual void reset();
    non_terminal create_complete_rule();

    fraction get_default_duration(){return my_duration;};
    pitch get_key(){return
        parser_state::get_current_voice_info().my_voice_info.my_key;};
    fraction get_time_signature(){return my_meter_rule.get_default_time_signature();};
    string get_unknown(){return my_other.get_value();};
    string get_songwords() const {return my_songwords_rule.get_value();};
    int get_index(){return my_index_rule.get_value();};
    string get_title(){return my_title_rule.get_value();};
    string get_current_voice(){return parser_state::get_instance().get_current_voice();};
    header_rule_info get_header_rule_info() const;
    header_rule();

    // Return true if the header has changed since we last checked.
    // Then reset the flag, unless  the user tells us not to.
    bool header_has_changed(bool the_reset = true);
private:
    copyright_rule my_copyright_rule;
    history_rule my_history_rule;
    songwords_rule my_songwords_rule;
    fraction my_duration;
    fraction my_time_signature;
    unknown_header_rule my_other;
    duration_specifier_rule my_duration_rule;
    key_header_rule my_key_rule;
    meter_specifier_rule my_meter_rule;
    tempo_specifier_rule my_tempo_rule;
    title_rule my_title_rule;
    index_rule my_index_rule;
    voice_rule_all_inclusive my_voice_rule;
    part_header_rule my_part_rule;
    bool my_header_has_changed;
};

/* Look for the signature x: to see if this will be
   a header */
class likely_header_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    likely_header_rule();
    ~likely_header_rule();
    virtual bool match(const string &the_string, int &the_index);
    virtual void reset();
    header_rule& get_header_rule(){return my_header_rule;};
private:
    header_rule my_header_rule;
};

class inline_words:public terminal
{
    // The derived class provides the expression in the costructor,
    // and then defines the 'convert' function.  The base class will
    // call this method if there is a match.
public:
    inline_words():terminal(".+([A-z]:.+)"){};
    virtual void convert(const string &the_string);
    string get_value(){return my_value;};
private:
    string my_value;

};

class measure_or_header:public compound_rule
{
    // return true if the_string[the_index] matches this rule
public:
    typedef enum match_type
    {
        header_match,
        measure_match,
        line_end_match,
        continue_line_match,
        no_match
    } match_type;

    measure_or_header();
    virtual ~measure_or_header();

    virtual non_terminal create_complete_rule();
    virtual bool match(const string& the_string,int& the_index);
    virtual void convert(const string &the_string, int &the_index);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset();
    match_type get_match_type(){return my_match_type;};
    header_rule_info get_header_rule(){return my_header.get_header_rule().get_header_rule_info();};
    measure_rule_info get_measure_rule(){return my_measure.get_measure_rule_info();};
    void reset_measure_rule() {my_measure.reset();};
    words_rule_info get_words_rule() const;
private:
    measure_rule my_measure;
    likely_header_rule my_header;
    words_rule my_words;
    match_type my_match_type;
    match_type my_last_end_type;
    voice_change_rule my_voice_change;
    bool my_is_words_line;
};
}
#endif



