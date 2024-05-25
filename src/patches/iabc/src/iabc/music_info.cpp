#include "iabc/music_info.h"

namespace iabc
{
;
measure_feature& 
measure_feature::operator=(const measure_feature& o)
{
    my_end_type = o.my_end_type;
	my_begin_type = o.my_begin_type;
	my_line_break = o.my_line_break;
    my_nth_ending_start = o.my_nth_ending_start;
    my_nth_ending_end = o.my_nth_ending_end;
    return *this;
}

part_point& 
part_point::operator=(const part_point& o)
{
    measure = o.measure;
    beat = o.beat;
    return *this;
}

bool 
part_point::operator==(const part_point& o) const 
{
    return ((beat == o.beat) && (measure == o.measure));
}

bool 
part_point::operator!=(const part_point& o) const 
{
    return (!(*this == o));
}

bool 
part_point::operator>(const part_point& o) const 
{
    bool tmp_rv = false;
    if ((measure > o.measure))
    {
        tmp_rv = true;
    }
    else if ((measure == o.measure) && (beat > o.beat))
    {
        tmp_rv = true;
    }

    return tmp_rv;
}

bool 
part_point::operator<(const part_point& o) const 
{
    return (((*this > o) == false) && ((*this != o)));
}

bool 
part_point::operator>=(const part_point& o) const 
{ 
    return ((*this < o) == false);
}

bool 
part_point::operator<=(const part_point& o) const 
{
    return ((*this > o) == false);
}

voice_info::voice_info(const pitch& the_key,
          const fraction& the_time_signature,
          pitch::clef the_clef,
          int the_beats_per_minute,
          const fraction& the_division_per_beat,
          bool the_common_or_cut_time):
    my_key(the_key),
    my_time_signature(the_time_signature),
    my_clef(the_clef),
    my_beats_per_minute(the_beats_per_minute),
    my_division_per_beat(the_division_per_beat),
    my_common_or_cut_time(the_common_or_cut_time),
    my_midi_program(1),
    my_midi_channel(1),
    my_midi_volume(40),
    my_transpose(0),my_octave(0)
{
}

voice_info::voice_info():
    my_beats_per_minute(96),
    my_division_per_beat(1,4),
    my_clef(pitch::TrebleClef),
    my_time_signature(4,4),
    my_common_or_cut_time(false),
    my_midi_program(1),
    my_midi_channel(1),
    my_midi_volume(40),
    my_index(1),
    my_transpose(0),my_octave(0)
{
}

voice_info::voice_info(const voice_info& o):
    my_key(o.my_key),my_time_signature(o.my_time_signature),
    my_clef(o.my_clef),my_beats_per_minute(o.my_beats_per_minute),
    my_division_per_beat(o.my_division_per_beat),
    my_common_or_cut_time(o.my_common_or_cut_time),
    my_midi_channel(o.my_midi_channel),
    my_midi_program(o.my_midi_program),
    my_midi_volume(o.my_midi_volume),
    my_index(o.my_index),
    my_transpose(o.my_transpose),
    my_octave(o.my_octave),
    my_part(o.my_part),
    my_name(o.my_name),
    my_short_name(o.my_short_name)
{};

voice_info& 
voice_info::operator=(const voice_info& o)
{
    my_key = o.my_key;
    my_time_signature = o.my_time_signature;
    my_clef = o.my_clef;
    my_beats_per_minute= o.my_beats_per_minute;
    my_common_or_cut_time = o.my_common_or_cut_time;
    my_beats_per_minute = o.my_beats_per_minute;
    my_division_per_beat = o.my_division_per_beat;
    my_midi_channel = o.my_midi_channel;
    my_midi_program = o.my_midi_program;
    my_midi_volume = o.my_midi_volume;
    my_index = o.my_index;
    my_transpose = o.my_transpose;
    my_octave = o.my_octave;
    my_part = o.my_part;
    my_name = o.my_name;
    my_short_name = o.my_short_name;
    return *this;
}

bool 
voice_info::operator==(const voice_info& o) const
{
    return ((my_key == o.my_key) &&
            (my_time_signature == o.my_time_signature) &&
            (my_clef == o.my_clef) &&
    (my_beats_per_minute ==  o.my_beats_per_minute ) &&
    (my_common_or_cut_time  ==  o.my_common_or_cut_time ) &&
    (my_beats_per_minute  ==  o.my_beats_per_minute ) &&
    (my_division_per_beat  ==  o.my_division_per_beat ) &&
    (my_midi_channel  ==  o.my_midi_channel ) &&
    (my_midi_program  ==  o.my_midi_program ) &&
    (my_midi_volume  ==  o.my_midi_volume ) &&
    (my_index  ==  o.my_index ) &&
	(my_transpose == o.my_transpose) &&
    (my_name == o.my_name) &&
    (my_part == o.my_part) &&
    (my_short_name == o.my_short_name) &&
    (my_octave == o.my_octave));
}

music_info::music_info()
{
}
music_info::~music_info()
{
}

music_info::music_info(const music_info& o):
    the_chord_info(o.the_chord_info),
    the_measure_feature(o.the_measure_feature),
    the_voice_info(o.the_voice_info),
    the_words(o.the_words)
{
}

music_info& 
music_info::operator=(const music_info& o)
{
    the_chord_info = (o.the_chord_info);
    the_measure_feature = (o.the_measure_feature);
    the_voice_info = (o.the_voice_info);
    the_words = (o.the_words);
    return *this;
}
}

