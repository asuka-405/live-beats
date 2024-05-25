/*
 * abcrules.cpp - The rules for the iabc parser.
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
#include "iabc/abcrules.h"
#include <string.h>
#include "iabc/array.cpp"
#include "iabc/map.cpp"
#include "iabc/iabc_preferences.h"
#include "iabc/event_log.h"

namespace iabc
{

INSTANTIATE_MAP(int,parser_state_info);

parser_state_info::parser_state_info():
the_tie_state(false),the_last_tie_state(false),
the_curlybrace_state(false),
the_duration(1,8),
    the_staves(1),the_paren_count(0)
{
}

bool
parser_state_info::operator==(const parser_state_info& o)
{
    return ((my_voice_info == o.my_voice_info) &&
            (the_copyright == o.the_copyright) &&
            (the_tie_state == o.the_tie_state) &&
            (the_last_tie_state == o.the_last_tie_state) &&
            (the_post_hornpipe == o.the_post_hornpipe) &&
            (the_paren_count == o.the_paren_count) &&
            (the_curlybrace_state == o.the_curlybrace_state) &&
            (the_staves == o.the_staves) &&
            (the_duration == o.the_duration));
    // Note that the accidental map is not included.  This really only
    // applies in the course of a measure, and its assumed that the
    // parser state would not change mid-measure anyhow

}

parser_state_info&
parser_state::get_info_for_voice(const string& the_voice)
{
    map<string,parser_state_info>::iterator tmp_it =
        parser_state::get_instance().the_known_voices.get_item(the_voice);

    // Return the saved voice, if possible.
    if (tmp_it)
    {
        parser_state_info& info = (*tmp_it).value;
        return info;
    }
    parser_state_info tmp_info;
    parser_state::get_instance().the_known_voices.add_pair(the_voice,tmp_info);
    tmp_it =
        parser_state::get_instance().the_known_voices.get_item(the_voice);
    if (tmp_it)
    {
        return (*tmp_it).value;
    }
    throw ("Memory Corruption");
}

parser_state_info&
parser_state::get_current_voice_info()
{
    return get_info_for_voice(parser_state::get_instance().the_current_voice);
}

bool
parser_state::knows_about_voice(const string& the_string)
{
    map<string,parser_state_info>::iterator tmp_it =
        the_known_voices.get_item(the_string);
    return (tmp_it != 0);
}

int
parser_state::get_current_voice_index()
{
    map<string,parser_state_info>::iterator tmp_last_it =
        the_known_voices.get_item(the_current_voice);
    if (tmp_last_it)
        {
        parser_state_info& tmp_info = (*tmp_last_it).value;
        return tmp_info.my_voice_info.my_index;
        }

    return 1;
}

void
parser_state::set_current_voice(const string& tmp_current_voice)
{
    map<string,parser_state_info>::iterator tmp_last_it =
        the_known_voices.get_item(the_current_voice);
    map<string,parser_state_info>::iterator tmp_it =
        the_known_voices.get_item(tmp_current_voice);

    // Usually abc is set up to inherit all voice headers
    // globally.
    if ((tmp_it) && (tmp_last_it) &&
        (iabc::allow_local_header_values == false))
    {
        parser_state_info& tmp_info = (*tmp_it).value;
        parser_state_info& tmp_old_info = (*tmp_last_it).value;
        tmp_info.my_voice_info.my_part = tmp_old_info.my_voice_info.my_part;
        tmp_info.my_voice_info.my_beats_per_minute = tmp_old_info.my_voice_info.my_beats_per_minute;
        tmp_info.my_voice_info.my_common_or_cut_time = tmp_old_info.my_voice_info.my_common_or_cut_time;
        tmp_info.my_voice_info.my_division_per_beat = tmp_old_info.my_voice_info.my_division_per_beat;
        tmp_info.my_voice_info.my_key = tmp_old_info.my_voice_info.my_key;
        tmp_info.my_voice_info.my_time_signature = tmp_old_info.my_voice_info.my_time_signature;
        tmp_info.the_duration = tmp_old_info.the_duration;
    }

    // This may be a new voice.  Set it up based
    // on the fields in the old voice.
    else if ((tmp_it == 0) && (tmp_last_it != 0))
    {
        parser_state_info tmp_info;
        tmp_info.my_voice_info.my_part = (*tmp_last_it).value.my_voice_info.my_part;
        tmp_info.my_voice_info.my_beats_per_minute = (*tmp_last_it).value.my_voice_info.my_beats_per_minute;
        tmp_info.my_voice_info.my_common_or_cut_time = (*tmp_last_it).value.my_voice_info.my_common_or_cut_time;
        tmp_info.my_voice_info.my_division_per_beat = (*tmp_last_it).value.my_voice_info.my_division_per_beat;
        tmp_info.my_voice_info.my_key = (*tmp_last_it).value.my_voice_info.my_key;
        tmp_info.my_voice_info.my_time_signature = (*tmp_last_it).value.my_voice_info.my_time_signature;
        tmp_info.the_duration = (*tmp_last_it).value.the_duration;
        int tmp_voice_index = the_known_voices.get_size() + 1;
        tmp_info.my_voice_info.my_index = tmp_voice_index;
        tmp_info.the_duration = (*tmp_last_it).value.the_duration;
        the_known_voices.add_pair(tmp_current_voice,tmp_info);
    }
    // The first voice.
    else
    {
        parser_state_info tmp_info;
        the_known_voices.add_pair(tmp_current_voice,tmp_info);
    }

    the_current_voice = tmp_current_voice;
    if (the_current_voice == (string)"1")
    {
        have_specified_voice_1 = true;
    }
}

array<regexp*> parser_state::the_key_expressions;

parser_state&
parser_state::get_instance()
{
    static parser_state* the_instance = 0;
    if (the_instance == 0)
    {
        the_instance = new parser_state;
    }

    return (*the_instance);
}

void
parser_state::gc()
{
    delete (&(parser_state::get_instance()));
}

void parser_state::reset()
{
    // Clear out all the voice-specific stuff and start
    // with the defaults in voice 1
    the_known_voices.clear();
    parser_state_info tmp_info;
    the_current_voice = "1";
    the_last_current_voice = 1;
    have_specified_voice_1 = false;
    the_history_field = false;
    int i;
    for (i = 0;i < the_key_expressions.get_size();++i)
    {
        delete the_key_expressions[i];
    }
    the_key_expressions.remove_all();
    the_current_tune = 1;
    the_last_tune = 0;
}

const char* duration_rule_string = "duration_rule";

duration_rule::duration_rule():
my_slash('/'),my_slashes("/")
,compound_rule(duration_rule_string)
,my_denominator(4),my_numerator(1),
    my_fraction(duration_specifier_rule::get_default())
{
}

void
duration_rule::reset()
{
    my_slash.reset();
    my_slashes.reset();
    my_denominator.reset();
    my_numerator.reset();
    my_space.reset();
    my_fraction = duration_specifier_rule::get_default();
    compound_rule::reset();
}

non_terminal
duration_rule::create_complete_rule()
{
  non_terminal tmp_null(my_null);
  non_terminal tmp_sds(my_slash & my_denominator);
  non_terminal tmp_nsds = (tmp_sds | tmp_null);
    return (my_numerator &
        tmp_nsds |
        (my_slash & my_denominator) |
        (my_slashes));
}

// duration::
//     number '/' number _or_
//     '/' number _or_
//     number
//     '/+' _or_
void
duration_rule::convert(const string& the_string,int& the_index)
{
    fraction tmp_unit = duration_specifier_rule::get_default();
    if (my_slashes.is_match() == true)
    {
        my_fraction = fraction(1,(1 << my_slashes.get_value()));
    }
    else if ((my_slash.is_match() == true) && (my_numerator.is_match() == true) &&
             (my_denominator.is_match() == true))
    {
        my_fraction = fraction(my_numerator.get_value(),my_denominator.get_value());
    }
    else if (my_numerator.is_match() == true)
    {
        my_fraction = fraction(my_numerator.get_value(),1);
    }
    else
    {
        my_fraction = fraction(1,my_denominator.get_value());
    }

    my_fraction *= tmp_unit;
}

void
letter_note_rule::reset()
{
    my_octave = 0;
    terminal::reset();
}

// The letter part of a pitch expression.
void
letter_note_rule::convert(const string &the_string)
{
    if (the_string[0] <= 'G')
    {
        my_letter = (pitch::letter)(the_string[0] - 'A');
        my_octave = (the_string[0] > 'B' ? 0 : 1);
    }
    else
    {
        my_letter = (pitch::letter)(the_string[0] - 'a');
        my_octave = (the_string[0] > 'b' ? 1 : 2);
    }
}

void part_header_rule::reset()
{
    my_value = "";
}

// return true if the_string[the_index] matches this rule
bool part_header_rule::match(const string &the_string, int &the_index)
{
    bool tmp_rv = false;
    if ((the_index + 2 < the_string.length()) &&
        (the_string[the_index] == 'P') &&
        (the_string[the_index + 1] == ':'))
    {
        my_value = the_string.mid(the_index + 2,the_string.length() - (the_index + 2));
        the_index += my_value.length() + 2; // the 2 is for P:
        // my_value.chop();
        tmp_rv = true;
        parser_state_info& tmp_parser_info =
            parser_state::get_instance().get_current_voice_info();
        tmp_parser_info.my_voice_info.my_part = my_value;
    }
    return tmp_rv;
}

// In keeping with the abc way of having a gazillion ways of
// doing the same thing, there are many different ways for
// the keys.  Keep an expression for each one
key_header_rule::key_map
key_header_rule::the_key_map[] =
{
    // The order of these is somewhat important.  We
    // need to try the longer expressions first since this part
    // of the language is ambiguous.  Or else, key of Bb
    // for example will match key of 'B' followed by a b.
    {" *Hp",pitch::D,pitch::natural},
    {" *HP",pitch::C,pitch::natural},
    {" *F|f# *L|lydian",pitch::C,pitch::sharp},
    {" *Bb *L|lydian",pitch::F,pitch::natural},
    {" *Eb *L|lydian",pitch::B,pitch::flat},
    {" *Ab *L|lydian",pitch::E,pitch::flat},
    {" *Db *L|lydian",pitch::A,pitch::flat},
    {" *Gb *L|lydian",pitch::D,pitch::flat},
    {" *D|d *L|lydian",pitch::A,pitch::natural},
    {" *E|e *L|lydian",pitch::B,pitch::natural},
    {" *F|f *L|lydian",pitch::C,pitch::natural},
    {" *G|g *L|lydian",pitch::D,pitch::natural},
    {" *C|c *L|lydian",pitch::G,pitch::natural},
    {" *A|a *L|lydian",pitch::E,pitch::natural},
    {" *B|b *L|lydian",pitch::F,pitch::sharp},

    {" *Bb *M|mix[A-z]*",pitch::E,pitch::flat},
    {" *Eb *M|mix[A-z]*",pitch::A,pitch::flat},
    {" *Ab *M|mix[A-z]*",pitch::D,pitch::flat},
    {" *F# *M|mix[A-z]*",pitch::B,pitch::natural},
    {" *C|c# *M|mix[A-z]*",pitch::F,pitch::sharp},
    {" *G|g *M|mix[A-z]*",pitch::C,pitch::natural},
    {" *A|a *M|mix[A-z]*",pitch::D,pitch::natural},
    {" *B|b *M|mix[A-z]*",pitch::E,pitch::natural},
    {" *C *M|mix[A-z]*",pitch::F,pitch::natural},
    {" *D|d *M|mix[A-z]*",pitch::G,pitch::natural},
    {" *F|f *M|mix[A-z]*",pitch::B,pitch::flat},
    {" *E|e *M|mix[A-z]*",pitch::A,pitch::natural},

    {" *B|b *D|dor[A-z]*",pitch::A,pitch::natural},
    {" *C|c# *D|dor[A-z]*",pitch::B,pitch::natural},
    {" *G|g# *D|dor[A-z]*",pitch::F,pitch::sharp},
    {" *F#|f# *D|dor[A-z]*",pitch::E,pitch::natural},
    {" *Bb *D|dor[A-z]*",pitch::A,pitch::flat},
    {" *Eb *D|dor",pitch::D,pitch::flat},
    {" *D|d *D|dor[A-z]*",pitch::C,pitch::natural},
    {" *E|e *D|dor[A-z]*",pitch::D,pitch::natural},
    {" *G|g *D|dor[A-z]*",pitch::F,pitch::natural},
    {" *A|a *D|dor[A-z]*",pitch::G,pitch::natural},
    {" *C|c *D|dor[A-z]*",pitch::B,pitch::flat},
    {" *F|f *D|dor[A-z]*",pitch::E,pitch::flat},

    {" *Bb *M|maj[A-z]*[A-z]|\\.",pitch::B,pitch::flat},
    {" *Eb *M|maj[A-z]*[A-z]|\\.",pitch::E,pitch::flat},
    {" *F|f# *M|maj[A-z]*[A-z]|\\.",pitch::F,pitch::sharp},
    {" *Ab *M|maj[A-z]*[A-z]|\\.",pitch::A,pitch::flat},
    {" *Db *M|maj[A-z]*[A-z]|\\.",pitch::D,pitch::flat},
    {" *B|b *M|maj[A-z]*[A-z]|\\.",pitch::B,pitch::natural},
    {" *A|a *M|maj[A-z]*[A-z]|\\.",pitch::A,pitch::natural},
    {" *G|g *M|maj[A-z]*[A-z]|\\.",pitch::G,pitch::natural},
    {" *F|f *M|maj[A-z]*[A-z]|\\.",pitch::F,pitch::natural},
    {" *C|c *M|maj[A-z]*[A-z]|\\.",pitch::C,pitch::natural},
    {" *C|c *M|maj[A-z]*[A-z]|\\.",pitch::C,pitch::natural},
    {" *D|d *M|maj[A-z]*[A-z]|\\.",pitch::D,pitch::natural},
    {" *E|e *M|maj[A-z]*[A-z]|\\.",pitch::E,pitch::natural},


    {" *F|f# *m|Min[A-z]*[A-z]|\\.",pitch::A,pitch::natural},
    {" *G|g# *m|Min[A-z]*[A-z]|\\.",pitch::B,pitch::natural},
    {" *C|c# *m|Min[A-z]*[A-z]|\\.",pitch::E,pitch::natural},
    {" *Bb *m|Min[A-z]*[A-z]|\\.",pitch::D,pitch::flat},
    {" *D|d# *m|Min[A-z]*[A-z]|\\.",pitch::F,pitch::sharp},
    {" *A|a *m|Min[A-z]*[A-z]|\\.",pitch::C,pitch::natural},
    {" *A|a *m|Min[A-z]*[A-z]|\\.",pitch::C,pitch::natural},
    {" *B|b *m|Min[A-z]*[A-z]|\\.",pitch::D,pitch::natural},
    {" *D|d *m|Min[A-z]*[A-z]|\\.",pitch::F,pitch::natural},
    {" *E|e *m|Min[A-z]*[A-z]|\\.",pitch::G,pitch::natural},
    {" *G|g *m|Min[A-z]*[A-z]|\\.",pitch::B,pitch::flat},
    {" *C|c *m|Min[A-z]*[A-z]|\\.",pitch::E,pitch::flat},
    {" *F|f *m|Min[A-z]*[A-z]|\\.",pitch::A,pitch::flat},

    {" *F|f# *m",pitch::A,pitch::natural},
    {" *G|g# *m",pitch::B,pitch::natural},
    {" *C|c# *m",pitch::E,pitch::natural},
    {" *Bb *m",pitch::D,pitch::flat},
    {" *D|d *m",pitch::F,pitch::natural},
    {" *D|d# *m",pitch::F,pitch::sharp},
    {" *A|a *m",pitch::C,pitch::natural},
    {" *A|a *m",pitch::C,pitch::natural},
    {" *B|b *m",pitch::D,pitch::natural},
    {" *D|d *m",pitch::F,pitch::natural},
    {" *E|e *m",pitch::G,pitch::natural},
    {" *G|g *m",pitch::B,pitch::flat},
    {" *C|c *m",pitch::E,pitch::flat},
    {" *F|f *m",pitch::A,pitch::flat},

    {" *Bb *",pitch::B,pitch::flat},
    {" *Db *",pitch::D,pitch::flat},
    {" *F|f# *",pitch::F,pitch::sharp},
    {" *Eb *",pitch::E,pitch::flat},
    {" *Ab *",pitch::A,pitch::flat},
    {" *A *",pitch::A,pitch::natural},
    {" *B|b *",pitch::B,pitch::natural},
    {" *C|c *",pitch::C,pitch::natural},
    {" *D|d *",pitch::D,pitch::natural},
    {" *E|e *",pitch::E,pitch::natural},
    {" *F|f *",pitch::F,pitch::natural},
    {" *G|g *",pitch::G,pitch::natural}
};

// KeyHeader::
//      'K '':' <key specifier>
bool
key_header_rule::match(const string &the_string, int &the_index)
{
    bool tmp_rv = false;
    if ((the_index + 2 < the_string.length()) &&
        (the_string[the_index] == 'K') &&
        (the_string[the_index + 1] == ':'))
    {
        string tmp_string = the_string.mid(the_index + 2,the_string.length() - (the_index + 2));
        the_index += 2 + convert(tmp_string);
        tmp_string = the_string.mid(the_index,the_string.length() - the_index);
        the_index += parse_extra_stuff(tmp_string);
        tmp_rv = true;
    }

    return tmp_rv;
}

// We have failed to match an 'or' rule, so we need to reset
// to our default values.
void
key_header_rule::reset()
{
    my_bc.reset();
    my_tc.reset();
}

const char* key_header_rule_string = "key_header_rule";
// key_header_rule::
//     K:<key>
key_header_rule::key_header_rule():
rule(key_header_rule_string),
my_pitch(pitch::C,pitch::natural),
my_bc(".+bass.*"),
my_tc(".+treble.*"),
my_transpose(" *t *= *(\\D+)")
{
}

void
key_header_rule::construct_expressions_if_required()
{
    // Only construct all these state machines the first time
    if (parser_state::get_instance().the_key_expressions.get_size() == 0)
    {
        int tmp_size =  sizeof(key_header_rule::the_key_map)/sizeof(key_header_rule::key_map);
        int i;

        parser_state::get_instance().the_key_expressions.expand_to(tmp_size);
        for (i = 0;
             i < sizeof(key_header_rule::the_key_map)/sizeof(key_header_rule::key_map);
             ++i)
        {
            parser_state::get_instance().the_key_expressions[i] =
                new regexp(the_key_map[i].expression);
        }
    }
}

key_header_rule::~key_header_rule()
{
}

int
key_header_rule::convert(const string &the_string)
{
    string tmp_string = the_string;
    construct_expressions_if_required();
    parser_state_info& tmp_parser_info =
        parser_state::get_instance().get_current_voice_info();
    my_pitch = tmp_parser_info.my_voice_info.my_key;
    int tmp_rv = 0;

    for (int i = 0;
         i < sizeof(key_header_rule::the_key_map)/sizeof(key_header_rule::key_map);
         ++i)
    {
        if (parser_state::get_instance().the_key_expressions[i]->match(tmp_string.access_char_array(),tmp_string.length())
                == true)
        {
            my_pitch = pitch(the_key_map[i].the_letter,the_key_map[i].the_accidental);
            tmp_parser_info.my_voice_info.my_key = my_pitch;
            tmp_rv = (*parser_state::get_instance().the_key_expressions[i])[0].length();
            break;
        }
    }

    tmp_parser_info.my_voice_info.my_key = my_pitch;

    return tmp_rv;
};

int
key_header_rule::parse_extra_stuff(const string& the_string)
{
    int tmp_bool = true;
    int tmp_rv = 0;
    string tmp_string = the_string;
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();
    while ((tmp_bool) && (tmp_rv + 1 < the_string.length()))
    {
        tmp_string = string(&(the_string.access_char_array()[tmp_rv]));
        int tmp_len = tmp_string.length();
        // Look for the clef marking in here also.
        my_bc.reset();
        my_tc.reset();
        my_transpose.reset();
        if (tmp_bool = my_bc.match(tmp_string.access_char_array(),tmp_len))
        {
            tmp_parser_info.my_voice_info.my_clef = pitch::BassClef;
            tmp_rv += my_bc[0].length();
        }
        else if (tmp_bool = my_tc.match(tmp_string.access_char_array(),tmp_len))
        {
            tmp_parser_info.my_voice_info.my_clef = pitch::TrebleClef;
            tmp_rv += my_tc[0].length();
        }
        else if (tmp_bool = my_transpose.match(tmp_string.access_char_array(),tmp_len))
        {
            int tmp_offset = (int)my_transpose[1].as_long();
            // No longer honor key transpose headers
            tmp_rv += my_transpose[0].length();
        }
    }

    return tmp_rv;
}

const char* duration_specifier_rule_string = "duration_specifier_rule";

duration_specifier_rule::duration_specifier_rule()
:rule(duration_specifier_rule_string)
{
}

void
duration_specifier_rule::reset()
{
    my_duration.reset();
}

// duration_header_rule::
//     'L:' duration_rule
bool
duration_specifier_rule::match(const string& the_string,int& the_index)
{
    int tmp_success = 0;
    if ((the_string.length() > the_index + 2) &&
        (the_string[the_index] == 'L') &&
        (the_string[the_index + 1] == ':'))
    {
        string tmp_string =
            the_string.mid(the_index + 2,the_string.length() - (the_index + 2));
        tmp_success = convert(tmp_string);
        the_index += 2 + tmp_success;
    }

    return (tmp_success > 0);
}

int
duration_specifier_rule::convert(const string &the_string)
{
    int tmp_rv = 0;
    parser_state_info& tmp_parser_info =
        parser_state::get_instance().get_current_voice_info();

    if (my_duration.match(the_string,tmp_rv) == true)
    {
        // All durations in the parser are read in terms of the default
        // duration, and that even goes for the default duration itself.
        // But the duration specifier is different, even though it has the
        // same syntax, so we un-default the default before calling
        // it the real default.  OK?
        tmp_parser_info.the_duration =
            tmp_parser_info.the_duration / my_duration.get_value();
    }

    return tmp_rv;
}

const char* meter_specifier_rule_string = "meter_specifier_rule";

meter_specifier_rule::meter_specifier_rule():
compound_rule(meter_specifier_rule_string),
my_slash("/"),my_common("C"),
    my_cut("C|"),my_other_cut("C_")
{}

void
meter_specifier_rule::reset()
{
    my_numerator.reset();
    my_denominator.reset();
    my_header.reset();
    my_slash.reset();
    my_whitespace.reset();
    my_common.reset();
    my_cut.reset();
    my_other_cut.reset();
    compound_rule::reset();
}

non_terminal
meter_specifier_rule::create_complete_rule()
{
    return ((my_header & my_whitespace & my_numerator & my_slash & my_denominator) | (my_header &
                                                                                      (my_cut | my_other_cut | my_common)));
}

void
meter_specifier_rule::convert(const string &the_string, int &the_index)
{
    parser_state_info& tmp_parser_info = parser_state::get_instance().get_current_voice_info();
    if (my_common.is_match())
    {
        tmp_parser_info.my_voice_info.my_time_signature = fraction(4,4);
        tmp_parser_info.my_voice_info.my_common_or_cut_time = true;
    }
    else if ((my_cut.is_match()) ||
             (my_other_cut.is_match()))
    {
        tmp_parser_info.my_voice_info.my_time_signature = fraction(2,2);
        tmp_parser_info.my_voice_info.my_division_per_beat = fraction(1,2);
        tmp_parser_info.my_voice_info.my_common_or_cut_time = true;
    }
    else
    {
        tmp_parser_info.my_voice_info.my_time_signature =
            fraction(my_numerator.get_value(),my_denominator.get_value());
        tmp_parser_info.my_voice_info.my_common_or_cut_time = false;
    }

}

const char* tempo_specifier_rule_string = "tempo_specifier_rule";

tempo_specifier_rule::tempo_specifier_rule():
    my_slash("/"),my_equals('='),compound_rule(tempo_specifier_rule_string)
{
}

tempo_specifier_rule::~tempo_specifier_rule()
{
}

void
tempo_specifier_rule::convert(const string &the_string, int &the_index)
{
    parser_state_info& tmp_parser_info = parser_state::get_instance().get_current_voice_info();

    if (my_numerator.is_match() && my_denominator.is_match())
    {
        tmp_parser_info.my_voice_info.my_division_per_beat = fraction(my_numerator.get_value(),
            my_denominator.get_value());
    }
    else
    {
        tmp_parser_info.my_voice_info.my_division_per_beat = fraction(1,4);
    }

    tmp_parser_info.my_voice_info.my_beats_per_minute = my_tempo.get_value();
}

// Q: n/d = tempo  -or-
// Q: tempo
non_terminal
tempo_specifier_rule::create_complete_rule()
{
    return ((my_header & my_whitespace & my_numerator & my_slash & my_denominator
             & my_whitespace & my_equals & my_whitespace & my_tempo) |
              (my_header & my_whitespace & my_tempo));
}

void
tempo_specifier_rule::reset()
{
    my_tempo.reset();
    my_numerator.reset();
    my_denominator.reset();
    my_header.reset();
    my_slash.reset();
    my_whitespace.reset();
    compound_rule::reset();
}


index_rule::index_rule():
terminal("^ *X: *(\\d+)")
{
};

// INDEX::
// X: \d+
void
index_rule::convert(const string &the_string)
{
    parser_state_info& tmp_parser_info = parser_state::get_instance().get_current_voice_info();

    // Start at tune 1, whether or not the X: is present.
    if (parser_state::get_instance().the_last_tune == 0)
    {
        parser_state::get_instance().the_last_tune = 1;
    }
    else
    {
        parser_state::get_instance().the_current_tune = parser_state::get_instance().the_last_tune;
        parser_state::get_instance().the_current_tune++;
    }
}

// TITLE::
// T: \w.+
void
title_rule::convert(const string &the_string)
{
    my_title = (*my_pattern)[1];
}

void
copyright_rule::convert(const string &the_string)
{
    my_value = (*my_pattern)[1];
}

bool
history_rule::match(const string& the_string,int& the_index)
{
    if (parser_state::get_instance().the_history_field == true)
    {
        if ((the_index == 0) &&
            (the_string.length() > 2) &&
            (the_string[1] == ':'))
        {
            parser_state::get_instance().the_history_field = false;
            return false;
        }
        else
            return true;
    }
    else
    {
        return terminal::match(the_string,the_index);
    }
}

void
history_rule::convert(const string &the_string)
{
    parser_state::get_instance().the_history_field = true;
}

void
accidental_rule::convert(const string &the_string)
{
    if ((*my_pattern)[0] == "^")
    {
        my_accidental = pitch::sharp;
    }
    else if ((*my_pattern)[0] == "^^")
    {
        my_accidental = pitch::double_sharp;
    }
    else if ((*my_pattern)[0] == "_")
    {
        my_accidental = pitch::flat;
    }
    else if ((*my_pattern)[0] == "__")
    {
        my_accidental = pitch::double_flat;
    }
    else
    {
        my_accidental = pitch::natural;
    }
}

const char* pitch_rule_string = "pitch_rule: ";

pitch_rule::pitch_rule()
:compound_rule(pitch_rule_string)
{
}

void
pitch_rule::reset()
{
    my_accidental.reset();
    my_octave.reset();
    my_letter.reset();
    compound_rule::reset();
}

// pitch::
//      accidental letter octave   _or_
//      letter octave      _or_
//      accidental letter  _or_
//      letter
non_terminal
pitch_rule::create_complete_rule()
{
    non_terminal tmp_on(my_octave | my_null);

    return
      //(my_letter & (my_octave | my_null)) |
      (my_letter & tmp_on) |
        (my_accidental & my_letter & (my_octave | my_null));

}

void
pitch_rule::convert(const string& the_string,int& the_index)
{
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();
    my_pitch = pitch(my_letter.get_value(),
                     my_accidental.get_value(),
                     my_octave.get_value() + my_letter.get_octave());

    // If the accidental was not specified and this is not in the
    // given key, transpose until it is.
    pitch tmp_key = tmp_parser_info.my_voice_info.my_key;
    if (my_accidental.is_match() == false)
    {
        my_pitch.change_accidental(tmp_parser_info.the_accidentals,tmp_key);
    }
    // If there was an accidental specified, add it to the list for this
    // measure.
    else
    {
        tmp_parser_info.the_accidentals.add_pair(my_pitch.get_letter(),
                                                 my_pitch.get_accidental());
    }

    // Include the octave as per the parser state.
    my_pitch = pitch(my_pitch.get_letter(),
                     my_pitch.get_accidental(),
                     (my_pitch.get_octave() + tmp_parser_info.my_voice_info.my_octave));


}

bar_line_rule::bar_line_rule()
:terminal(" *[||]|[:|]|[|:]|[::]|[|\\]]|[|]|[:] *")
{
}

void
bar_line_rule::convert(const string& the_string)
{
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();
    string tmp_string = the_string;
    while (tmp_string[tmp_string.length() - 1] == ' ') tmp_string.chop();
    tmp_string.strip_white_space();
    if (tmp_string == "|")
    {
        my_bar_type = measure_feature::single_bar;
    }
    else if ((tmp_string == "||") ||
        (tmp_string == "|]"))
    {
        my_bar_type = measure_feature::double_bar;
    }
    else if (tmp_string == ":|")
    {
        my_bar_type = measure_feature::repeat_last;
    }
    else if (tmp_string == "|:")
    {
        my_bar_type = measure_feature::repeat_next;
    }
    else
    {
        my_bar_type = measure_feature::repeat_last_and_next;
    }
    // We've reached the end of a measure so clear out the accidentals.
    tmp_parser_info.the_accidentals.clear();
}

const char* nth_ending_rule_string = "nth ending rule: ";

nth_ending_rule::nth_ending_rule():my_square('['),
    compound_rule(nth_ending_rule_string)
{
}

void
nth_ending_rule::pre_match(const string& the_string,int the_index)
{
}

non_terminal
nth_ending_rule::create_complete_rule()
{
    return non_terminal((my_square & my_number) |
                        (my_bar_line & (my_number | my_null)));
}

void
nth_ending_rule::convert(const string& the_string,int& the_index)
{
    if (my_number.is_match() == true)
    {
        my_repeat_number =
            my_number.get_value();
    }
    else
    {
        my_repeat_number = 0;
    }
    if (my_bar_line.is_match() == true)
    {
        my_end_type = my_bar_line.get_value();
    }
}

void
nth_ending_rule::reset()
{
    my_number.reset();
    my_repeat_number = 0;
    my_end_type = measure_feature::none;
    my_bar_line.reset();
    my_square.reset();
    compound_rule::reset();
}

const char* inline_header_rule_string = "inline header rule: ";

inline_header_rule::inline_header_rule()
:
rule(inline_header_rule_string),
    my_should_create(true),
    my_is_match(false),r1(0),my_loser_nt(0)
{
}

inline_header_rule::~inline_header_rule()
{
    if (r1)
    {
        delete r1;
    }
    if (my_loser_nt)
    {
        delete my_loser_nt;
    }
}

void
inline_header_rule::create_complete_rule()
{
    if (my_should_create == true)
    {
        my_should_create = false;
        my_loser_nt = new non_terminal_rule(non_terminal(my_loser_rule));
        int i;
        for (i = 0;i < 128; ++i)
        {
            my_array[i] = my_loser_nt;
        }
        non_terminal tmp_nt(my_voice_change|
                            my_duration |
                            my_key |
                            my_meter|
                            my_part |
                            my_tempo);
        r1 = new non_terminal_rule(tmp_nt);
        my_array['K'] = r1;
        my_array['L'] = r1;
        my_array['M'] = r1;
        my_array['Q'] = r1;
        my_array['V'] = r1;
        my_array['P'] = r1;
    }
}

bool
inline_header_rule::match(const string &the_string, int &the_index)
{
    create_complete_rule();
    // Remove the ] character
    int i;
    string tmp_payload;
    for (i = the_index;i < the_string.length();++i)
    {
        if (the_string[i] == ']')
        {
            break;
        }
        tmp_payload += the_string[i];
    }
    int tmp_index = 0;
    my_is_match = my_array[tmp_payload[0]]->match(tmp_payload,tmp_index);

    bool tmp_match = my_is_match;
    while (tmp_match)
    {
        while ((the_string[the_index] == ' ') ||
               (the_string[the_index] == ',')) ++tmp_index;
        tmp_match = my_array[tmp_payload[tmp_index]]->match(tmp_payload,tmp_index);
    }
    the_index += tmp_index;
    return my_is_match;
}

// We have failed to match an 'or' rule, so we need to reset
// to our default values.
void
inline_header_rule::reset()
{
    my_duration.reset();
    my_key.reset();
    my_meter.reset();
    my_tempo.reset();
}

const char* note_info_rule_string = "note_info_rule: ";

note_info_rule::note_info_rule()
:
rule(note_info_rule_string),
    my_z("z"),
    my_x("x"),
    my_should_create(true),
    my_is_match(false),
    r2(0),r3(0),r4(0),my_loser_nt(0)
{
}

note_info_rule::~note_info_rule()
{
    if (r2)
    {
        delete r2;
    }
    if (r3)
    {
        delete r3;
    }
    if (r4)
    {
        delete r4;
    }
    if (my_loser_nt)
    {
        delete my_loser_nt;
    }
}

void
note_info_rule::reset()
{
    my_pitch_rule.reset();
    my_duration_rule.reset();
    my_z.reset();
    my_x.reset();
    my_is_match = false;
    my_tie_rule.reset();
}

// note_info::
//      pitch duration _or_
//      pitch
void
note_info_rule::create_complete_rule()
{
    if (my_should_create == true)
    {
        my_should_create = false;
        my_loser_nt = new non_terminal_rule(non_terminal(my_loser_rule));
        int i;
        for (i = 0;i < 128; ++i)
        {
            my_array[i] = my_loser_nt;
        }
        r2 = new non_terminal_rule(
            non_terminal(my_pitch_rule & (my_duration_rule | my_null) & (my_tie_rule | my_null)));
        r3 = new non_terminal_rule(
            non_terminal((my_z | my_x) & (my_duration_rule | my_null)));
        r4 = new non_terminal_rule(non_terminal(my_tie_rule));
        char c;
        for (c = 'A';c <= 'G'; ++c)
        {
            my_array[c] = r2;
            my_array[c + ('a' - 'A')] = r2;
        }
        my_array['_'] = my_array['^'] = my_array['='] = r2;
        my_array['x'] = my_array['z'] = r3;
        my_array['-'] = r4;
    }
}

bool
note_info_rule::match(const string& the_string,int& the_index)
{
    create_complete_rule();

    my_is_match = my_array[the_string[the_index]]->match(the_string,the_index);

    if (my_is_match == true)
        convert();

    return my_is_match;
}

void
note_info_rule::convert()
{
    my_info = note_info(my_pitch_rule.get_value(),
                        my_duration_rule.get_value(),
                        (my_z.is_match() | my_x.is_match()));


    // If we matched the tie rule, we might also have matched the
    // previous tie rule so handle that.
    my_info.end_tie(tie_rule::get_and_reset_last());

    if (my_tie_rule.is_match() == true)
    {
        my_info.start_tie(tie_rule::get_and_reset());
    }
}

const char* multi_note_rule_string = "multi_note_rule: ";

multi_note_rule::multi_note_rule()
:iterative_rule(multi_note_rule_string)
{
}

void
multi_note_rule::reset()
{
    // Nothing to be done, this is handled in the recursion
    my_notes.remove_all();
}

void
hornpipe::convert(const string &the_string)
{
    my_value = the_string;
}

void
hornpipe::reset()
{
    my_value = "";
    terminal::reset();
}

multi_note_rule::~multi_note_rule()
{
    delete_rule_array(my_note_rules);
    delete_rule_array(my_terminals);
}

// multi_note::
//      note_info _or_
//      note_info multi_note
void
multi_note_rule::create_complete_rule(int the_depth)
{
    setup_rule_array<note_info_rule*,note_info_rule>(the_depth,my_note_rules);
    note_info_rule& tmp_rule = *my_note_rules[the_depth - 1];
    if (my_terminals.get_size() < (the_depth))
    {
        my_terminals.expand(1);
        my_terminals[the_depth - 1] = new non_terminal
            (tmp_rule);
    }
}

void
multi_note_rule::convert(int the_depth)
{
    int tmp_new_size = the_depth - my_notes.get_size();
    if (tmp_new_size > 0)
    {
        my_notes.expand(tmp_new_size);
    }
    note_info_rule& tmp_rule = *my_note_rules[the_depth - 1];
    my_notes[the_depth - 1] = tmp_rule.get_value();
}

const char* multi_note_or_inline_header_rule_string =
    "multi_note_or_inline_header_rule: ";

multi_note_or_inline_header_rule::multi_note_or_inline_header_rule():
rule(multi_note_or_inline_header_rule_string),
my_should_create(false),my_is_matched(false)
{
}

multi_note_or_inline_header_rule::~multi_note_or_inline_header_rule()
{
}

bool
multi_note_or_inline_header_rule::match(const string &the_string, int &the_index)
{
    my_is_matched = my_header_rule.match(the_string,the_index);
    if (my_is_matched == false)
    {
        my_is_matched = my_note_rule.match(the_string,the_index);
    }

    return my_is_matched;
}

// We have failed to match an 'or' rule, so we need to reset
// to our default values.
void
multi_note_or_inline_header_rule::reset()
{
    my_header_rule.reset();
    my_note_rule.reset();
    my_is_matched = false;
}

const char* chord_info_rule_string = "chord_info_rule: ";

chord_info_rule::chord_info_rule():
chord_start("["),chord_end("]")
,compound_rule(chord_info_rule_string)
{
}

void
chord_info_rule::reset()
{
    chord_start.reset();
    chord_end.reset();
    my_notes.reset();
    my_note.reset();
    my_chord_info = chord_info();
    compound_rule::reset();
}

// chord_info::
//      '[' multi_note_info ']' _or_
//      embellishment chord_info _or_
//      quoted_string chord_info
//
//      note_info
non_terminal
chord_info_rule::create_complete_rule()
{
    return non_terminal(((chord_start &  my_notes &  chord_end ) |
        (my_note)));
}

void
chord_info_rule::convert(const string &the_string, int &the_index)
{

    multi_note_rule::note_info_array tmp_array =
        my_notes.get_note_rule().get_value();

    // First we see if the array got matched
    if (tmp_array.get_size())
    {
        for (int i = 0;i < tmp_array.get_size();++i)
        {
            my_chord_info += tmp_array[i];
        }
    }
    else if (my_note.is_matched() == true)
    {
        note_info tmp_info(my_note.get_value());
        my_chord_info += tmp_info;
    }
    // else throw ("Bad chord read");
}

embellishment_rule::embellishment_rule()
    :my_embellishment(chord_info::none),terminal("~|\\.|v|u|H|T|M|P|S")
    {};

void
embellishment_rule::reset()
{
    my_embellishment = chord_info::none;
    terminal::reset();
}

void
embellishment_rule::convert(const string& the_string)
{
    switch (the_string[0])
    {
    case '~':
        my_embellishment = chord_info::gracing;
        break;
    case '.':
        my_embellishment = chord_info::staccato;
        break;
    case 'u':
        my_embellishment = chord_info::up_bow;
        break;
    case 'v':
        my_embellishment = chord_info::down_bow;
        break;
    case 'M':
    case 'P':
        my_embellishment = chord_info::mordant;
        break;
    case 'T':
        my_embellishment = chord_info::trill;
        break;
    case 'H':
        my_embellishment = chord_info::fermata;
        break;
    case 'S':
        my_embellishment = chord_info::signo;
        break;
    case 'O':
        my_embellishment = chord_info::coda;
        break;
    case 'L':
        my_embellishment = chord_info::accent;
        break;
    default:
        my_embellishment = chord_info::none;
        break;
    }
}

const char* abc_quoted_string_string = "abc_quoted_string: ";

abc_quoted_string::abc_quoted_string():
compound_rule(abc_quoted_string_string)
{
}

non_terminal
abc_quoted_string::create_complete_rule()
{
    return
        non_terminal(my_quoted_string_rule & my_whitespace_rule);
}

void
abc_quoted_string::convert(const string& the_string,int& the_index)
{
    my_value = my_quoted_string_rule.get_value();
}

void
abc_quoted_string::reset()
{
    my_quoted_string_rule.reset();
    my_whitespace_rule.reset();
    my_value.clear();
    compound_rule::reset();
}

const char* dynamic_string_string = "dynamic_string: ";

dynamic_string::dynamic_string():
my_value(chord_info::mf),
    my_ppp("ppp"),my_pp("pp"),my_p("p"),
    my_fff("fff"),my_ff("ff"),my_f("f"),
    my_cresc_start("crescendo("),my_dim_start("diminuendo("),
    my_cresc_end("crescendo)"),my_dim_end("diminuendo)"),
    compound_rule(dynamic_string_string)
{
}

dynamic_string::~dynamic_string()
{
}

void
dynamic_string::reset()
{
    my_ppp.reset();
    my_pp.reset();
    my_p.reset();
    my_fff.reset();
    my_ff.reset();
    my_f.reset();
    my_cresc_start.reset();
    my_dim_start.reset();
    my_cresc_end.reset();
    my_dim_end.reset();
    compound_rule::reset();
}

non_terminal
dynamic_string::create_complete_rule()
{
    return non_terminal(
        my_ppp |
        my_pp |
        my_p |
        my_fff |
        my_ff |
        my_f |
        my_cresc_start |
        my_dim_start |
        my_cresc_end |
        my_dim_end
        );
}

void
dynamic_string::convert(const string& the_string,int& the_index)
{
    if (my_ppp.is_match())
    {
        my_value = chord_info::pp;
    }
    else if (my_pp.is_match())
    {
        my_value = chord_info::pp;
    }
    else if (my_p.is_match())
    {
        my_value = chord_info::p;
    }
    else if (my_fff.is_match())
    {
        my_value = chord_info::fff;
    }
    else if (my_ff.is_match())
    {
        my_value = chord_info::ff;
    }
    else if (my_f.is_match())
    {
        my_value = chord_info::f;
    }
    else if (my_cresc_start.is_match())
    {
        my_value = chord_info::start_cresc;
    }
    else if (my_dim_start.is_match())
    {
        my_value = chord_info::start_decresc;
    }
    else if (my_cresc_end.is_match())
    {
        my_value = chord_info::end_cresc;
    }
    else if (my_dim_end.is_match())
    {
        my_value = chord_info::end_decresc;
    }
}

const char* embellishment_string_string = "embellishment_string: ";

embellishment_string::embellishment_string():
    my_trill("trill"),
    my_lowermordent("lowermordent"),
    my_mordent("mordent"),
    my_pralltriller("pralltriller"),
    my_accent("accent"),
    my_invertedfermata("my_invertedfermata"),
    my_tenuto("tenuto"),
    my_pizzicato("pizzicato"),
    my_wedge("wedge"),
    my_open("open"),
    my_snap("snap"),
    my_turn("turn"),
    my_roll("roll"),
    my_shortphrase("shortphrase"),
    my_mediumphrase("mediumphrase"),
    my_longphrase("longphrase"),
    my_upbow("upbow"),
    my_downbow("downbow"),
    compound_rule(embellishment_string_string)
    {}

embellishment_string::~embellishment_string()
{
}

void
embellishment_string::reset()
{
    my_trill.reset();
    my_lowermordent.reset();
    my_mordent.reset();
    my_pralltriller.reset();
    my_mordent.reset();
    my_accent.reset();
    my_invertedfermata.reset();
    my_tenuto.reset();
    my_pizzicato.reset();
    my_wedge.reset();
    my_open.reset();
    my_snap.reset();
    my_turn.reset();
    my_roll.reset();
    my_shortphrase.reset();
    my_mediumphrase.reset();
    my_longphrase.reset();
    my_upbow.reset();
    my_downbow.reset();
    compound_rule::reset();
}

non_terminal
embellishment_string::create_complete_rule()
{
    return non_terminal(
          my_trill
        | my_lowermordent
        | my_mordent
        | my_pralltriller
        | my_mordent
        | my_accent
        | my_invertedfermata
        | my_tenuto
        | my_pizzicato
        | my_wedge
        | my_open
        | my_snap
        | my_turn
        | my_roll
        | my_shortphrase
        | my_mediumphrase
        | my_longphrase
        | my_upbow
        | my_downbow);
}

void
embellishment_string::convert(const string& the_string,int& the_index)
{
    // TODO:  Some of these do not have figures to do with them.
    if (my_trill.is_match())
    {
        my_value = chord_info::trill;
    }
    else if (my_lowermordent.is_match())
    {
        my_value = chord_info::mordant;
    }
    else if (my_mordent.is_match())
    {
        my_value = chord_info::mordant;
    }
    else if (my_pralltriller.is_match())
    {
        my_value = chord_info::trill;
    }
    else if (my_accent.is_match())
    {
        my_value = chord_info::accent;
    }
    else if (my_invertedfermata.is_match())
    {
        my_value = chord_info::fermata;
    }
    else if (my_tenuto.is_match())
    {
        my_value = chord_info::tenuto;
    }
    else if (my_pizzicato.is_match())
    {
        my_value = chord_info::staccato;
    }
    else if (my_wedge.is_match())
    {
        my_value = chord_info::up_bow;
    }
    else if (my_open.is_match())
    {
        my_value = chord_info::staccato;
    }
    else if (my_snap.is_match())
    {
        my_value = chord_info::trill;
    }
    else if (my_turn.is_match())
    {
        my_value = chord_info::mordant;
    }
    else if (my_roll.is_match())
    {
        my_value = chord_info::trill;
    }
    else if (my_shortphrase.is_match())
    {
        my_value = chord_info::none;
    }
    else if (my_mediumphrase.is_match())
    {
        my_value = chord_info::none;
    }
    else if (my_longphrase.is_match())
    {
        my_value = chord_info::none;
    }
    else if (my_upbow.is_match())
    {
        my_value = chord_info::up_bow;
    }
    else if (my_downbow.is_match())
    {
        my_value = chord_info::down_bow;
    }
}

const char* bang_rule_string = "bang_rule: ";

bang_rule::bang_rule():
my_open_bang('!'),my_close_bang('!'),
    compound_rule(bang_rule_string)
{
}

bang_rule::~bang_rule()
{
}

non_terminal
bang_rule::create_complete_rule()
{
    return non_terminal(my_open_bang &
                        (my_dynamics | my_embellishment) & my_close_bang);
}

void
bang_rule::convert(const string& the_string,int& the_index)
{
    ; //nothing to be done.
}

void
bang_rule::reset()
{
    my_dynamics.reset();
    my_embellishment.reset();
    my_open_bang.reset();
    my_close_bang.reset();
    compound_rule::reset();
}

const char* pre_chord_stuff_string = "pre_chord_stuff: ";

pre_chord_stuff::pre_chord_stuff():
rule(pre_chord_stuff_string),
my_ocurly('{'),
my_ccurly('}'),
my_should_create(true),
my_is_match(false)
{
    int i;
    for (i = 0;i <= 127; ++i)
    {
        my_array[i] = &my_loser;
    }
}

pre_chord_stuff::~pre_chord_stuff()
{
}

void
pre_chord_stuff::reset()
{
    my_chord_xlet_prefix.reset();
    my_embellishment_rule.reset();
    my_quoted_string_rule.reset();
    my_hornpipes.reset();
    my_ocurly.reset();
    my_ccurly.reset();
    my_bang_rule.reset();
    my_is_match = false;
}

void
pre_chord_stuff::create_complete_rule()
{
    if (my_should_create == true)
    {
        my_should_create = false;
        my_array['!'] = &my_bang_rule;
        my_array['('] = &my_chord_xlet_prefix;
        my_array['\"'] = &my_quoted_string_rule;
        my_array['{'] = &my_ocurly;
        my_array['}'] = &my_ccurly;
        my_array['>'] = my_array['<'] = &my_hornpipes;
        my_array['~'] = my_array['.'] = my_array['v'] = my_array['u'] =
            my_array['H'] = my_array['T'] = my_array['M'] = my_array['S'] =
            my_array['O'] = my_array['L'] = &my_embellishment_rule;
    }
    return;
}

bool
pre_chord_stuff::match(const string& the_string,int& the_index)
{
    create_complete_rule();

    bool tmp_rv = my_array[the_string[the_index]]->match(the_string,the_index);

    while (tmp_rv)
    {
        tmp_rv = my_array[the_string[the_index]]->match(the_string,the_index);
    }

    // We always return true since the null rule matches.
    my_is_match = tmp_rv;
    return true;
}

const char* post_chord_stuff_string = "post_chord_stuff";

post_chord_stuff::post_chord_stuff():
rule(post_chord_stuff_string),
my_should_create(true),
my_is_match(false),
my_ccurly('}')
{
    int i;
    for (i = 0;i <= 127; ++i)
    {
        my_array[i] = &my_loser;
    }
}

void
post_chord_stuff::create_complete_rule()
{
    if (my_should_create == true)
    {
        my_should_create = false;
        // my_array['\"'] = &my_quoted_string_rule;
        my_array[')'] = &my_cparen_rule;
        my_array['}'] = &my_ccurly;
        my_array[' '] = &my_space;
        my_array['>'] = my_array['<'] = &my_hornpipe;
    }
    return;
}

// We have failed to match an 'or' rule, so we need to reset
// to our default values.
void
post_chord_stuff::reset()
{
    my_cparen_rule.reset();
    my_quoted_string_rule.reset();
    my_space.reset();
    my_ccurly.reset();
    my_is_match = false;
    my_hornpipe.reset();
}

bool
post_chord_stuff::match(const string& the_string,int& the_index)
{
    create_complete_rule();

    bool tmp_rv = my_array[the_string[the_index]]->match(the_string,the_index);

    while (tmp_rv)
    {
        tmp_rv = my_array[the_string[the_index]]->match(the_string,the_index);
    }

    // We always return true since the null rule matches.
    my_is_match = tmp_rv;
    return true;
}

const char* multi_chord_rule_string = "multi_chord_rule: ";

multi_chord_rule::multi_chord_rule()
:iterative_rule(multi_chord_rule_string),
    my_chord_depth(1)
{
}

void
multi_chord_rule::reset()
{
    my_syntax_error = false;
    my_chords.remove_all();
    clear_rule_array(my_pre);
    clear_rule_array(my_chord_array);
    clear_rule_array(my_post);
    my_chord_depth = 1;
}

multi_chord_rule::~multi_chord_rule()
{
    delete_rule_array(my_pre);
    delete_rule_array(my_chord_array);
    delete_rule_array(my_post);
    delete_rule_array(my_terminals);
}

// multi_chord::
//      chord_info _or_
//      chord_info multi_chord _or_
//      chord_info '<+|>+' chord_info _or_
//      chord_info '-' chord_info _or_
//      chord_info '(' chord_info _or_
//      chord_info ')' chord_info
void
multi_chord_rule::create_complete_rule(int the_depth)
{
    setup_rule_array<chord_info_rule*,chord_info_rule>(the_depth,my_chord_array);
    setup_rule_array<pre_chord_stuff*,pre_chord_stuff>(the_depth,my_pre);
    setup_rule_array<post_chord_stuff*,post_chord_stuff>(the_depth,my_post);
    if (the_depth == 1)
        my_chords.remove_all();
    chord_info_rule& tmp_chord = *my_chord_array[the_depth - 1];
    pre_chord_stuff& tmp_pre = *my_pre[the_depth - 1];
    post_chord_stuff& tmp_post = *my_post[the_depth - 1];

    if (my_terminals.get_size() < the_depth)
    {
        my_terminals.expand(1);
        my_terminals[the_depth - 1] = new
            non_terminal(tmp_pre & tmp_chord & tmp_post);
    }
}

void
multi_chord_rule::convert(int the_depth)
{
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();

    // If we match an inline header, the chord array becomes
    // out of sync with the depth, so compensate for that with
    // my_chord_depth
    chord_info_rule& tmp_chord = *my_chord_array[the_depth - 1];
    pre_chord_stuff& tmp_pre = *my_pre[the_depth - 1];
    post_chord_stuff& tmp_post = *my_post[the_depth - 1];

    // If we've matched something other than a chord, don't put
    // it in the array, its just an inline header.
    if (tmp_chord.get_value().get_size() < 1)
    {
        return;
    }
    // Make sure the array has allocatd space fo the new notes.
    int tmp_new_size = my_chord_depth - my_chords.get_size();
    if (tmp_new_size > 0)
    {
        my_chords.expand(tmp_new_size);
    }
    my_chords[my_chord_depth - 1] = tmp_chord.get_value();

    string tmp_hornpipe = tmp_pre.get_hornpipe_rule().get_value();
    if (tmp_hornpipe.length() == 0)
    {
        tmp_hornpipe = tmp_parser_info.the_post_hornpipe;
    }

    // Handle the dreaded hornpipe rule...
    if (tmp_hornpipe.length())
    {
        // Some sanity checks to make sure this rule is valid
        if ((my_chords.get_size() > 1) &&
            (my_chords[my_chord_depth - 2].get_size() ==
             my_chords[my_chord_depth - 1].get_size()) &&
            (my_chords[my_chord_depth - 2].get_duration() ==
             my_chords[my_chord_depth - 1].get_duration()))
        {
            handle_hornpipe(
                my_chords[my_chord_depth - 2],
                my_chords[my_chord_depth - 1],
                tmp_hornpipe);
        }
        else
        {
            my_syntax_error = true;
        }
    }

    // Set the hornpipe rule up for next time if the '>' occurs after the
    // current value as opposed to before the next value.
    tmp_parser_info.the_post_hornpipe = tmp_post.get_hornpipe().get_value();

    // Store any paren's.
    if ((tmp_pre.get_oparen_rule().is_match() == true) &&
        (tmp_pre.get_oparen_rule().is_oparen() == true))
    {
        if (my_chord_depth <= my_chords.get_size())
        {
            my_chords[my_chord_depth - 1].start_slur();
        }
        else
        {
            my_syntax_error = true;
        }
    }

    if (tmp_post.get_cparen_rule().is_match() == true)
    {
        if (my_chord_depth <= my_chords.get_size())
        {
            my_chords[my_chord_depth - 1].end_slur();
        }
        else
        {
            my_syntax_error = true;
        }
    }

    if (tmp_pre.get_quoted_string_rule().is_matched() == true)
    {
        my_chords[my_chord_depth - 1].set_description(tmp_pre.get_quoted_string_rule().get_value());
    }

    if (tmp_pre.get_ocurly().is_match() == true)
    {
        tmp_parser_info.the_curlybrace_state = true;
    }

    if (tmp_parser_info.the_curlybrace_state == true)
    {
        my_chords[my_chord_depth - 1].set_grace(true);
    }

    if ((tmp_pre.get_ccurly().is_match() == true) ||
        (tmp_post.get_ccurly() == true))
    {
        tmp_parser_info.the_curlybrace_state = false;
    }

    // A chord symbol can either follow or precede the cord.  We assume it won't
    // do both; if it does, probably both will appear.
    if (tmp_post.get_quoted_string_rule().is_match() == true)
    {
        my_chords[my_chord_depth - 1].set_description(tmp_post.get_quoted_string_rule().get_value());
    }

    if (tmp_pre.get_embellishment_rule().is_match() == true)
    {
        my_chords[my_chord_depth - 1].set_embellishment(tmp_pre.get_embellishment_rule().get_value());
    }

    if (tmp_pre.get_bang_rule().is_matched() == true)
    {
        if (tmp_pre.get_bang_rule().get_embellishment().is_matched() == true)
        {
            my_chords[my_chord_depth - 1].set_embellishment(tmp_pre.get_bang_rule().get_embellishment().get_value());
        }
        else if (tmp_pre.get_bang_rule().get_dynamics().is_matched() == true)
        {
            my_chords[my_chord_depth - 1].set_dynamic(tmp_pre.get_bang_rule().get_dynamics().get_value());
        }
    }

    // Finally, if there is space at the end of this chord, stop because
    // we need to go on to a new beam group
    if (tmp_post.is_there_space() == true)
    {
        my_stop_please = true;
    }

    my_chord_depth++;
}

void
multi_chord_rule::handle_hornpipe(chord_info& tmpLHS,chord_info& tmpRHS,const string& the_string)
{
    int length = the_string.length();
    int divisor = 2;
    fraction tmp_fraction = tmpLHS.get_duration() * fraction(1,divisor);
    int i;
    for (i = 1;i < length; ++i)
    {
        divisor *= 2;
        tmp_fraction += fraction(1,divisor);
    }

    // Flip the +- thing if the arrow goes the other way
    if (the_string[0] == '<')
        tmp_fraction *= -1;

    // We assume the LHS and RHS are the same size here, or the
    // hornpipe rule doesn't make any sense.  This should be
    // checked earlier.
    for (i = 0;i < tmpLHS.get_size();++i)
    {
        note_info tmpNewLHS = tmpLHS[i];
        note_info tmpNewRHS = tmpRHS[i];
        tmpNewLHS.change_length(fraction(0,1),tmpNewLHS.get_duration() + tmp_fraction);
        tmpNewRHS.change_length(fraction(0,1),tmpNewRHS.get_duration() - tmp_fraction);
        tmpLHS[i] = tmpNewLHS;
        tmpRHS[i] = tmpNewRHS;
    }
}

voice_definition_rule::voice_definition_rule():terminal("V: *([0-z]+) (.+)"),
    my_barfly_program(" *program +(\\d+) +(\\d+)"),
    my_barfly_bass(" *bass"),
    my_barfly_volume(" *volume +(\\d+)")
{
}

// VOICE_HEADER::
// V:<voice number> and <stuff>
void
voice_definition_rule::convert(const string& the_string)
{
    string tmp_voice_name = (*my_pattern)[1];
    parser_state::get_instance().set_current_voice(tmp_voice_name);
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();

    string tmp_payload = (*my_pattern)[2];

    // Modify the parser state so that it knows which voice we're
    // talking about.
    my_pair.reset();

    if (iabc::allow_barfly_midi_commands == true)
    {
        barfly_convert(tmp_payload,tmp_voice_name.as_long());
    }
    else
    {
        normal_convert(tmp_payload,tmp_voice_name.as_long());
    }
}

regexp*
voice_definition_rule::find_barfly_match(const char* the_string,int the_length)
{
    parser_state_info& tmp_parser_info =
        parser_state::get_instance().get_current_voice_info();

    if (my_barfly_bass.match(the_string,the_length))
    {
        tmp_parser_info.my_voice_info.my_clef = pitch::BassClef;
        return &my_barfly_bass;
    }
    else if (my_barfly_volume.match(the_string,the_length))
    {
        string tmp_volume = my_barfly_volume[1];
        tmp_parser_info.my_voice_info.my_midi_volume = (int)tmp_volume.as_long();
        return &my_barfly_volume;
    }
    else if (my_barfly_program.match(the_string,the_length))
    {
        string tmp_program = my_barfly_program[2];
        tmp_parser_info.my_voice_info.my_midi_program = (int)tmp_program.as_long();
        return &my_barfly_program;
    }
    else return 0;
}

void
voice_definition_rule::barfly_convert(const string& the_string,int the_voice)
{
    regexp* tmp_match;
    const char* tmp_string = the_string.access_char_array();
    int tmp_length = the_string.length();
    while (tmp_match = find_barfly_match(tmp_string,tmp_length))
    {
        int tmp_match_length = (*tmp_match)[0].length();
        if (tmp_length > tmp_match_length)
        {
            tmp_length -= tmp_match_length;
        }
        tmp_string = &(tmp_string[tmp_match_length]);
    }
}

void
voice_definition_rule::normal_convert(const string& the_string,int the_voice)
{
    int tmp_index = 0;
    parser_state_info& tmp_info = parser_state::get_current_voice_info();

    // We've matched a voice header.  We may be just setting the
    // current voice, or else we're trying to set some property of
    // the current voice.  If the latter, the properties are
    // name=value pairs after the voice number.  So get them all.
    while (my_pair.match(the_string,tmp_index) == true)
    {
        // Set midi program
        if ((my_pair.get_key() == "program") ||
            (my_pair.get_key() == "instrument"))
        {
            tmp_info.my_voice_info.my_midi_program =
                (int)my_pair.get_value().as_long();
        }
        if (my_pair.get_key().mid(0,4) == "tran")
        {
            tmp_info.my_voice_info.my_transpose =
                (int)my_pair.get_value().as_long();
        }
        if (my_pair.get_key() == "volume")
        {
            tmp_info.my_voice_info.my_midi_volume =
                (int)my_pair.get_value().as_long();
        }
        if (my_pair.get_key() == "octave")
        {
            tmp_info.my_voice_info.my_octave =
                (int)my_pair.get_value().as_long();
        }
        // Set stave
        if (my_pair.get_key() == "stv")
        {
            tmp_info.the_staves = (int)my_pair.get_value().as_long();
        }
        // Set name of voice (e.g. Oboe 2)
        else if ((my_pair.get_key().to_lower() == "nm") ||
                 (my_pair.get_key().to_lower() == "name"))
        {
            tmp_info.my_voice_info.my_name = my_pair.get_value();
        }
        // Set shorter name of voice (e.g. Ob. 2)
        else if ((my_pair.get_key().to_lower() == "snm") ||
                 (my_pair.get_key().to_lower() == "subname"))
        {
            tmp_info.my_voice_info.my_short_name = my_pair.get_value();
        }
        // Set the cleff.  Need to add all the clefs here and
        // everywhere.
        else if ((my_pair.get_key() == "cleff") ||
            (my_pair.get_key() == "clef"))
        {
            string tmp_cleff = my_pair.get_value();
            tmp_cleff.to_lower();
            if (tmp_cleff == "bass")
            {
                tmp_info.my_voice_info.my_clef = pitch::BassClef;
            }
            else
            {
                tmp_info.my_voice_info.my_clef = pitch::TrebleClef;
            }
        }

        my_pair.reset();
    }
}

voice_header::voice_header():
terminal("V: *([0-z]+)")
{
}

voice_header::~voice_header()
{
}

void
voice_header::convert(const string& the_string)
{
    parser_state_info& tmp_parser_info =
        parser_state::get_current_voice_info();

    string tmp_voice_name = (*my_pattern)[1];
    parser_state::get_instance().set_current_voice (tmp_voice_name);
}

const char* voice_rule_all_inclusive_string = "voice_rule_all_inclusive: ";

voice_rule_all_inclusive::voice_rule_all_inclusive():
compound_rule(voice_rule_all_inclusive_string)
{
}

voice_rule_all_inclusive::~voice_rule_all_inclusive()
{
}

non_terminal
voice_rule_all_inclusive::create_complete_rule()
{
    non_terminal tmp_stuff(my_voice_definition_rule);
    non_terminal tmp_no_stuff(my_voice_header_rule);
    return (tmp_stuff | tmp_no_stuff);
}

void
voice_rule_all_inclusive::convert(const string &the_string, int &the_index)
{
    // Match should have taken care of things.
}

void
voice_rule_all_inclusive::reset()
{
    my_voice_header_rule.reset();
    my_voice_definition_rule.reset();
}

const char* chord_xlet_prefix_string = "chord_xlet_prefix_or_oparen: ";

chord_xlet_prefix_or_oparen::xlet_map
chord_xlet_prefix_or_oparen::the_maps[] =
{
    {"\\((\\d+):(\\d+):(\\d+) *",xlet_map::use_p,xlet_map::use_first,xlet_map::use_second},
    {"\\((\\d+)::(\\d+) *",xlet_map::use_p,xlet_map::use_default,xlet_map::use_p},
    {"\\((\\d+):(\\d+) *",xlet_map::use_p,xlet_map::use_first,xlet_map::use_p},
    {"\\((\\d+) *",xlet_map::use_p,xlet_map::use_default,xlet_map::use_p}
};

chord_xlet_prefix_or_oparen::chord_xlet_prefix_or_oparen():
rule(chord_xlet_prefix_string),
my_xlet_value(0),my_is_oparen(false),my_is_match(false)
{
    int tmp_array_size = sizeof(the_maps)/sizeof(xlet_map);
    this->my_exp_array = new regexp*[tmp_array_size];
    for (int i = 0;i < tmp_array_size;++i)
    {
        my_exp_array[i] = new regexp(the_maps[i].my_expression);
    }
}

chord_xlet_prefix_or_oparen::~chord_xlet_prefix_or_oparen()
{
    int tmp_array_size = sizeof(the_maps)/sizeof(xlet_map);
    for (int i = 0;i < tmp_array_size;++i)
    {
        delete my_exp_array[i];
    }

    delete [] my_exp_array;
}

void
chord_xlet_prefix_or_oparen::reset()
{
    int tmp_array_size = sizeof(the_maps)/sizeof(xlet_map);
    for (int i = 0;i < tmp_array_size;++i)
    {
        my_exp_array[i]->reset();
    }
    my_xlet_value = 0;
    my_unit_length = 0;
    my_number_notes = 0;
    my_is_oparen = false;
    my_is_match = false;
}

bool
chord_xlet_prefix_or_oparen::match(const string& the_string,int& the_index)
{
    my_is_match = false;

    // Look ahead to see if this is an oparen
    if ((the_string.length() > the_index + 1) &&
        (the_string[the_index] == '(') &&
        ((the_string[the_index + 1] < '0') ||
         (the_string[the_index + 1] > '9')))
    {
        my_is_oparen = my_is_match = true;
        the_index += 1;
        return true;
    }
    fraction tmp_unit_length = duration_specifier_rule::get_default();
    my_unit_length = tmp_unit_length;
    my_number_notes = 0;
    if ((the_string.length() < the_index + 2) ||
        (the_string[the_index] != '(') ||
        (the_string[the_index + 1] < '0') ||
        (the_string[the_index + 1] > '9'))
    {
        my_is_match = false;
        return false;
    }
    int tmp_array_size = sizeof(the_maps)/sizeof(xlet_map);

    // An xlet is p:q:r, which is the same as
    // r notes with the value that p notes fit in q time.
    int tmp_q = 2;

    if (meter_specifier_rule::get_default_time_signature().num % 3 == 0)
    {
        tmp_q = 3;
    }

    const char* tmp_string = the_string.access_char_array();
    tmp_string = &(tmp_string[the_index]);
    for (int i = 0; i < tmp_array_size;++i)
    {
        if ((*my_exp_array[i]).match(tmp_string,the_string.length() - the_index) == true)
        {
            my_is_match = true;
            the_index += (*my_exp_array[i])[0].length();
            // The first matched number is always the 'p' part,
            // or how many notes get put into one time unit.
            int tmp_p = (int)(*my_exp_array[i])[1].as_long();
            int tmp_r = tmp_p;
            // The other numbers depend on the format.  If not
            // specified there's a default, the static table tells
            // use how to interpret the expression.
            if (the_maps[i].q_value == xlet_map::use_p)
            {
                tmp_q = tmp_p;
            }
            else if (the_maps[i].q_value != xlet_map::use_default)
            {
                tmp_q = (int)(*(my_exp_array[i]))[(int)the_maps[i].q_value].as_long();
            }
            if (the_maps[i].r_value == xlet_map::use_p)
            {
                tmp_r = tmp_p;
            }
            else if (the_maps[i].r_value != xlet_map::use_default)
            {
                tmp_r = (int)(*my_exp_array[i])[(int)the_maps[i].r_value].as_long();
            }

            //Now we have p,q, and r.
            fraction tmp_length(1,tmp_p);
            my_xlet_value = tmp_p;
            tmp_length *= (tmp_unit_length * tmp_q);
            my_unit_length = tmp_length;
            my_number_notes = tmp_r;
            break;
        }
    }

    return my_is_match;
}


const char* beam_group_rule_string = "beam_group_rule: ";

beam_group_rule::beam_group_rule()
:compound_rule(beam_group_rule_string)
{
}

void
beam_group_rule::reset()
{
    my_chords.reset();
    my_array.remove_all();
    my_optional_space.reset();
    my_required_space.reset();
    compound_rule::reset();
}

// beam_group::
// (multi_chord_rule | xlet_rule)
non_terminal
beam_group_rule::create_complete_rule()
{
    return
        (my_chords & my_optional_space);
}

void
beam_group_rule::convert(const string& the_string,int& the_index)
{
    parser_state_info& tmp_parser_info = parser_state::get_current_voice_info();

    // Note that we assume we beam grace notes; if this is not true
    // we will need something smarter here but that is what we've
    // seen so far.
    my_array = my_chords.get_value();

    // Each note beams itself with the other notes in the
    // beam group.  Keep track of the last one in the
    // group so the beam group will kjnow when to render
    // itself.
    bool tmp_have_set_first = false;
    bool tmp_have_set_first_grace = false;

    // Handle the triplet or duplet or xlet by modifying the lengths
    fraction tmp_unit_length = tmp_parser_info.the_duration;
    chord_xlet_prefix_or_oparen& tmp_xlet = my_chords.get_xlet(0);
    int i;
    if ((tmp_xlet.is_match() == true) &&
        (tmp_xlet.is_oparen() == false))
    {
        for (i = 0;i < my_array.get_size();++i)
        {
            // Convert the length of each note to the number of unit lengths from the
            // actual length of the note.
            my_array[i].change_length(fraction(tmp_unit_length.den,tmp_unit_length.num),0);

            // Then multiply by the unit length that the xlet tells us to do.
            my_array[i].change_length(tmp_xlet.get_unit_value(),0);

            my_array[i].set_xlet_value(tmp_xlet.get_xlet_value());
        }
    }

    // We beam grace and non-grace notes independently, so handle that
    // goofy logic by tracking the beams seperately.
    int tmp_last_index = 0;
    int tmp_last_grace_index = 0;
    for (i = 0;i < my_array.get_size();++i)
    {
        //If there is no real information here don't do anything
        if (my_array[i].get_size() == 0)
            continue;

        // Since we can't beam rests, don't let a rest be the
        // first thing in a beam group.
        if (((my_array[i][0].is_rest() == true)) &&
            (tmp_have_set_first == false))
        {
            continue;
        }

        // The longest value note that we can beam is a thrice-dotted
        // eigth note.  If something is shorter and is in a beam group
        // then tell it whether it is the first one or not so we can
        // construct graphical beam groups later.
        if ((my_array[i].get_duration() <= chord_info::the_longest_beamed_thing) &&
            (tmp_have_set_first == true) &&
            (my_array[i].is_grace() == false))
        {
            my_array[i].set_first_in_beam(false);
            my_array[i].set_last_in_beam(false);
        }
        else if ((my_array[i].get_duration() <= chord_info::the_longest_beamed_thing) &&
            (tmp_have_set_first_grace == true) &&
            (my_array[i].is_grace() == true))
        {
            my_array[i].set_first_in_beam(false);
            my_array[i].set_last_in_beam(false);
        }
        else if (my_array[i].get_duration() <= chord_info::the_longest_beamed_thing)
        {
            if (my_array[i].is_grace() == false)
            {
                tmp_have_set_first = true;
            }
            else
                tmp_have_set_first_grace = true;
            my_array[i].set_first_in_beam(true);
            my_array[i].set_last_in_beam(false);
        }
        // We can't beam cause it's too long.  Close out any previous
        // groups, put this one in a group by itself, and then start a
        // new beam group for remaining notes.
        else
        {
            if (tmp_have_set_first == true)
            {
                my_array[tmp_last_index].set_last_in_beam();
            }
            if (tmp_have_set_first_grace == true)
            {
                my_array[tmp_last_grace_index].set_last_in_beam();
            }
            tmp_have_set_first = false;
            tmp_have_set_first_grace = false;
            my_array[i].set_first_in_beam();
            my_array[i].set_last_in_beam();
        }
        // Since we can't beam rests, don't let a rest be the last note in a
        // beam group either.
        if (my_array[i][0].is_rest() == false)
        {
            if (my_array[i].is_grace() == false)
            {
                tmp_last_index = i;
            }
            else
            {
                tmp_last_grace_index = i;
            }
        }

        // Finally, if it's a non-grace note, and we have started a beam group
        // on some grace notes, end the beam grace
        if ((my_array[i].is_grace() == false) &&
            (tmp_have_set_first_grace == true))
        {
            tmp_have_set_first_grace = false;
            my_array[tmp_last_grace_index].set_last_in_beam();
        }
    }

    // The catch-all
    if (tmp_have_set_first == true)
    {
        my_array[tmp_last_index].set_last_in_beam();
    }
    if (tmp_have_set_first_grace == true)
    {
        my_array[tmp_last_grace_index].set_last_in_beam();
    }
}

words_rule::words_rule():
terminal("w:.+$")
{
}

void
words_rule::convert(const string& the_string)
{
    my_match = the_string;
}

void
words_rule::reset()
{
    my_match = "";
    terminal::reset();
}

songwords_rule::songwords_rule():
terminal("W:.+$")
{
}

void
songwords_rule::convert(const string& the_string)
{
    if (the_string.length() > 3)
        my_match = the_string.mid(2,the_string.length() - 3);
}

void
songwords_rule::reset()
{
    my_match = "";
    terminal::reset();
}

const char* measure_rule_string = "measure_rule: ";

measure_rule_info
measure_rule::get_measure_rule_info() const
{
    measure_rule_info tmp_info;
    int i;
    for (i = 0;i < my_beam_groups.get_size();++i)
    {
        // The array gets erased each time the rule is reset.  Deep copy
        // the array before returning so that the array does not change
        // underneath us.
        chord_info_array tmp_old_array = my_beam_groups[i]->get_value();
        if (tmp_old_array.get_size() > 0)
        {
            chord_info_array tmp_new_array;
            deep_copy(tmp_old_array,tmp_new_array);
            tmp_info.my_beam_groups.add_to_end(tmp_new_array);
        }
    }
    tmp_info.my_measure_feature = my_measure_feature;
    return tmp_info;
}

measure_rule::measure_rule()
:iterative_rule(measure_rule_string)
{
}

void
measure_rule::pre_match(const string& the_string,int the_index)
{
}

void
measure_rule::reset()
{
    my_beams.remove_all();
    my_ending_rule.reset();
    my_is_match = false;
    my_measure_feature = measure_feature();
}

void
measure_rule::create_complete_rule(int the_depth)
{
    // expand the rule arrays if we need to.
    setup_rule_array<beam_group_rule*,beam_group_rule>(the_depth,my_beam_groups);

    // And create the permanent rule for this depth in the array
    // if we need to.
    beam_group_rule& tmp_beams = *(my_beam_groups[the_depth - 1]);
//  non_terminal tmp_rule(non_terminal(my_ending_rule) |
//                          ((tmp_beams & my_space) & (my_meter_rule | my_tempo_rule | my_space)));
    non_terminal tmp_rule(non_terminal(my_ending_rule) |
                          (tmp_beams & (my_meter_rule | my_tempo_rule | my_space)));
    if (my_terminals.get_size() < the_depth)
    {
        my_terminals.expand(1);
        my_terminals[the_depth - 1] = new  non_terminal(
            my_space & tmp_rule);
    }
}

measure_rule::~measure_rule()
{
    delete_rule_array(my_beam_groups);
    delete_rule_array(my_terminals);
}

// measure::
// (beam_group _and_ end_bar) _or_
// (beam_group _and_ ' *' _and_ measure) _or_
// (beam_group _and_ measure)
void
measure_rule::convert(int the_iteration)
{
    // If this is the end of the bar, handle that.  then stop
    // because we've read in a measure, time to return.
    beam_group_rule& tmp_beams = *(my_beam_groups[the_iteration - 1]);

    // If we see an end of line, and we're somewhere other than the
    // start of the measure, this is the end bar.
    if (my_ending_rule.is_matched() == true)
    {
        if (my_beams.get_size() > 0)
        {
            my_measure_feature.end_type(my_ending_rule.get_bar_value());
            my_measure_feature.nth_ending_end(my_ending_rule.get_repeat_value());
            my_stop_please = true;
        }
        // If the bar is at the beginning of the line, its really a
        // part of the previous measure, but it shows up as the
        // beginning of this measure since its on a new line.
        else
        {
            my_measure_feature.begin_type(my_ending_rule.get_bar_value());
            my_measure_feature.nth_ending_start(my_ending_rule.get_repeat_value());
        }
        my_ending_rule.reset();
    }
    else
    {
        // otherwise stick a fork in the music.  It's cooked and
        // ready to be copied into its universal form of chord info
        // arrays.
        int tmp_new_size = the_iteration - my_beams.get_size();
        if (tmp_new_size > 0)
        {
            my_beams.expand(tmp_new_size);
        }
        my_beams[the_iteration - 1] = tmp_beams.get_value();
    }

    if (my_space.is_match() == true)
    {
        my_space.reset();
    }
}

unknown_header_rule::~unknown_header_rule()
{
};

header_rule_info
header_rule::get_header_rule_info() const
{
    header_rule_info tmp_info;
    tmp_info.my_default_duration = my_duration;
    tmp_info.my_time_signature = my_meter_rule.get_default_time_signature();
    tmp_info.my_index = my_index_rule.get_value();
    tmp_info.my_key = parser_state::get_current_voice_info().my_voice_info.my_key;
    tmp_info.my_unknown = my_other.get_value();
    tmp_info.my_songwords = my_songwords_rule.get_value();
    tmp_info.my_title = my_title_rule.get_value();
    tmp_info.my_voice = parser_state::get_instance().get_current_voice();
    tmp_info.my_copyright = my_copyright_rule.get_value();;
    return tmp_info;
}

// Mostly we just provide accessors to our rules that may have
// matched, so there's not much to do here.
void
header_rule::convert(const string &the_string, int &the_index)
{
    parser_state_info& tmp_parser = parser_state::get_current_voice_info();
    my_header_has_changed = true;
    my_duration = my_duration_rule.get_value();
    my_time_signature = my_meter_rule.get_default_time_signature();
    if (my_copyright_rule.is_match() == true)
    {
        tmp_parser.the_copyright += my_copyright_rule.get_value();
    }
    my_other.reset();
}

void
header_rule::reset()
{
    my_duration_rule.reset();
    my_key_rule.reset();
    my_other.reset();
    my_index_rule.reset();
    my_title_rule.reset();
    my_voice_rule.reset();
    my_history_rule.reset();
    my_copyright_rule.reset();
    compound_rule::reset();
}

non_terminal
header_rule::create_complete_rule()
{
    return non_terminal(
        my_history_rule | my_index_rule | my_title_rule | my_duration_rule |
        my_key_rule | my_part_rule |
        my_meter_rule | my_tempo_rule | my_voice_rule | my_songwords_rule |
        my_copyright_rule | my_other);
}

bool
header_rule::header_has_changed(bool the_reset)
{
    bool tmp_rv = my_header_has_changed;
    if (the_reset)
    {
        my_header_has_changed = false;
    }
    return tmp_rv;
}

const char* header_rule_string = "header_rule: ";

header_rule::header_rule():
compound_rule(header_rule_string),
my_header_has_changed(false)
{
}

const char* likely_header_rule_string = "likely_header_rule: ";

likely_header_rule::likely_header_rule()
:rule(likely_header_rule_string)
{
}

likely_header_rule::~likely_header_rule()
{
}

bool
likely_header_rule::match(const string &the_string, int &the_index)
{
    if ((the_index + 2 < the_string.length()) &&
        (the_string[the_index] >= 'A') &&
        (the_string[the_index] <= 'z') &&
        (the_string[the_index + 1] == ':'))
    {
        return my_header_rule.match(the_string,the_index);
    }

    return false;
}

void
likely_header_rule::reset()
{
    my_header_rule.reset();
}

const char* voice_change_rule_string = "voice_change_rule: ";

voice_change_rule::voice_change_rule():
rule(voice_change_rule_string),
    my_expression(0),
    my_is_match(false)
{
    my_expression = new regexp("V: *([0-z]+)");
}

voice_change_rule::~voice_change_rule()
{
    delete my_expression;
}

bool
voice_change_rule::match(const string &the_string, int &the_index)
{
    my_is_match = false;
    int tmp_length = the_string.length() - the_index;

    // Old-style voice change headers are at the beginning of the line
    if ((the_index != 0) && (allow_deprecated_voice_change == true))
    {
        ;
    }
    else if (my_expression->match(&(the_string.access_char_array()[the_index]),tmp_length))
    {
        string tmp_num_string = (*my_expression)[1];

        // If this is an old-style header, we need to know about
        // the voice before we can switch to it.  Not true of
        // inline headers.
        if ((parser_state::get_instance().knows_about_voice(tmp_num_string))
            ||(allow_deprecated_voice_change == false))
        {
            my_voice = tmp_num_string;
            parser_state::get_instance().set_current_voice(my_voice);
            my_is_match = true;
            the_index += (*my_expression)[0].length();
        }
    }

    return my_is_match;
}

// We have failed to match an 'or' rule, so we need to reset
// to our default values.
void
voice_change_rule::reset()
{
    my_expression->reset();
    my_is_match = false;
    my_voice = "";
}

const char* measure_or_header_string = "measure_or_header: ";

measure_or_header::measure_or_header():
compound_rule(measure_or_header_string),
my_match_type(measure_or_header::no_match),
my_last_end_type(measure_or_header::line_end_match),
my_is_words_line(false)
{

}

measure_or_header::~measure_or_header()
{
}

words_rule_info
measure_or_header::get_words_rule() const
{
    words_rule_info tmp_info;
    tmp_info.is_match = my_words.is_match();
    tmp_info.my_value = my_words.get_value();
    return tmp_info;
}

void
measure_or_header::reset()
{
    my_header.reset();
    my_measure.reset();
    compound_rule::reset();
    my_last_end_type = my_match_type = no_match;
}

non_terminal
measure_or_header::create_complete_rule()
{
    return (my_words | my_header | my_measure);
}

bool
measure_or_header::match(const string& the_string,int& the_index)
{
    // Handle the stupid history field.
    int tmp_start_index = the_index; // used for debug
    reset();
    if (parser_state::get_instance().the_history_field == true)
    {
        if ((the_index == 0) &&
            (the_string.length() > 2) &&
            (the_string[1] == ':'))
        {
            parser_state::get_instance().the_history_field = false;
        }
        else
        {
            the_index = the_string.length();
        }
    }

    // The first point in the file might be a voice change.
    if ((the_index == 0) && (allow_deprecated_voice_change == true))
    {
        my_voice_change.match(the_string,the_index);
    }

    bool tmp_rv = false;

    // Some end-of-line exceptions to the rule that we need to handle
    // exceptionally.
    // Check for end of line.  Save this information because word matches go with
    // the previous non-word line that matched, according to tradition.
    if (the_string[the_index] == '\\')
    {
        the_index++;
        tmp_rv = true;
        my_match_type = measure_or_header::continue_line_match;
        my_last_end_type = measure_or_header::continue_line_match;
    }
    else if ((the_string[the_index] == '!') ||
        (the_string[the_index] == '\n') ||
             (the_string[the_index] == '\0'))
    {
        ++the_index;
        tmp_rv = true;
        // Matching a words line, the measures go with the previous line.
        // !!! not sure what this was meant to do...
        if ((my_is_words_line == true) && (0))
        {
            my_is_words_line = false;
            my_match_type = my_last_end_type;
        }
        else
        {
            my_match_type = measure_or_header::line_end_match;
            my_last_end_type = my_match_type;
        }
    }

    if (tmp_rv == false)
    {
        tmp_rv = compound_rule::match(the_string,the_index);

        if (my_words.is_match())
        {
            my_is_words_line = true;
            my_match_type = measure_or_header::line_end_match;
            global_parser_event_log.log_event(the_string.access_char_array(),
                                              the_index - tmp_start_index,
                                              parser_event_data::words_rule);
        }
        else if (my_measure.is_match() == true)
        {
            my_match_type = measure_or_header::measure_match;
            global_parser_event_log.log_event(the_string.access_char_array(),
                                              the_index - tmp_start_index,
                                              parser_event_data::measure_rule);
        }
        else if (my_header.get_header_rule().is_matched() == true)
        {
            my_match_type = measure_or_header::header_match;
            global_parser_event_log.log_event(the_string.access_char_array(),
                                              the_index - tmp_start_index,
                                              parser_event_data::header_rule);
        }

        // Speed things up by advancing to the next non-space
        while ((the_index + 1 <  the_string.length()) &&
               (the_string[the_index] == ' '))
        {
            ++the_index;
        }
    }

    return tmp_rv;
}

void
measure_or_header::convert(const string& the_string,int& the_index)
{
    // nothing to do, its done in the derived class
}

void
inline_words::convert(const string& the_string)
{
    my_value = (*my_pattern)[1];
}
}
