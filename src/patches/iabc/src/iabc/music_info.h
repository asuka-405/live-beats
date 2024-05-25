/*
 * music_info.h - structures about music in general.
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
#ifndef iabc_feature_h
#define iabc_feature_h
// #include "iabc/wd_data.h"
#include "iabc/pitch.h"
#include "iabc/note_info.h"
#include "iabc/list.h"
#include "iabc/map.h"

#ifdef _DEBUG
#define part_point pp_
#endif

// FILE:   music_info.h
// DESCRIPTION:
// This contains information about the music that's not particular
// to any single medium and isn't about the notes (note stuff is in
// note info)
namespace iabc
{
;
// CLASS: measure_feature
// DESCRIPTION:
// information to describe a particular measure, including the number
// and the endpoint
class measure_feature
{
public:
    typedef enum measure_feature_enum
    {
        none,
        single_bar,
        double_bar,
        repeat_last,
        repeat_next,
        repeat_last_and_next
    }measure_feature_enum;

    measure_feature(measure_feature_enum the_end_type,int the_measure,
                    bool the_line_break = false,
                    measure_feature_enum the_begin_type = none):
        my_end_type(the_end_type),my_line_break(the_line_break),
        my_begin_type(the_begin_type),my_nth_ending_start(0),my_nth_ending_end(0){};
    measure_feature():my_end_type(single_bar),my_nth_ending_start(0),
        my_nth_ending_end(0),my_begin_type(none),my_line_break(false){};
    measure_feature(const measure_feature& o):
        my_end_type(o.my_end_type),
        my_begin_type(o.my_begin_type),my_line_break(o.my_line_break),
        my_nth_ending_start(o.my_nth_ending_start),my_nth_ending_end(o.my_nth_ending_end){};
    measure_feature& operator=(const measure_feature& o);

    ~measure_feature(){};
    measure_feature_enum end_type() const {return my_end_type;};
    measure_feature_enum begin_type() const {return my_begin_type;};

    void end_type(measure_feature::measure_feature_enum the_type) {my_end_type = the_type;};
    void begin_type(measure_feature::measure_feature_enum the_type) {my_begin_type = the_type;};
    void nth_ending_start(int the_ending){my_nth_ending_start = the_ending;};
    void nth_ending_end(int the_ending){my_nth_ending_end = the_ending;};
    int nth_ending_start() const{return my_nth_ending_start;};
    int nth_ending_end() const{return my_nth_ending_end;};
    bool is_line_break() const {return my_line_break;};
    void line_break(bool the_value = true) {my_line_break = the_value;};
protected:
    bool my_line_break;
    measure_feature_enum my_end_type;
    measure_feature_enum my_begin_type;

    // ATTRIBUTE: nth_repeat
    // DESCRIPTION:
    // 1st, 2nd ending etc.  usually 0.
    int my_nth_ending_start;
    int my_nth_ending_end;
};


// CLASS: part_point
// DESCRIPTION:
// A part point represents a particular beat in a particular measure.
// Each chord in each voice belongs to a particular part_point
class part_point
{
public:
    int measure;
    fraction beat;    part_point(int the_measure,fraction the_beat):
        measure(the_measure),beat(the_beat){};
    part_point(const part_point& o):
        measure(o.measure),beat(o.beat){};
    part_point& operator=(const part_point& o);
    part_point():measure(1){};
    bool operator==(const part_point& o) const ;
    bool operator!=(const part_point& o) const;
    bool operator>(const part_point& o) const;
    bool operator<(const part_point& o) const;
    bool operator>=(const part_point& o) const;
    bool operator<=(const part_point& o) const;
};

typedef map<part_point,chord_info> note_stream;

// CLASS: voice_info
// DESCRIPTION:
// Information about a particular part, which in ABC would be
// called a voice.
class voice_info
{
public:
    // ATTRIBUTE: my_index
    // DESCRIPTION:
    // This is the voice number or part number.
    int my_index;
    int my_midi_channel;
    int my_midi_program;
    int my_midi_volume;
    pitch my_key;
    fraction my_time_signature;
    bool my_common_or_cut_time;
    pitch::clef my_clef;
    int my_beats_per_minute;
    fraction my_division_per_beat;
    int my_transpose;
    int my_octave;
    string my_part;
    string my_name;
    string my_short_name;
    voice_info(const pitch& the_key,
              const fraction& the_time_signature,
              pitch::clef the_clef,
              int the_beats_per_minute,
              const fraction& the_division_per_beat,
              bool the_common_or_cut_time);
    voice_info();
    voice_info(const voice_info& o);
    voice_info& operator=(const voice_info& o);
    bool operator==(const voice_info& o) const;
};

inline bool operator!= (const voice_info& a,const voice_info& b)
{
    return (!(a == b));
};

template <class T>
class smart_ptr
{
public:
    smart_ptr():my_value(0){};
    smart_ptr(const T& t):my_value(new T(t)){};
    smart_ptr(const smart_ptr<T>& o):my_value(0)
    {
        if (o.my_value)
        {
            my_value = new T(*(o.my_value));
        }
    };
    ~smart_ptr(){if (my_value) delete my_value;my_value = 0;};
    smart_ptr& operator=(const T& t){if (my_value) delete my_value;my_value = new T(t);return *this;};
    smart_ptr& operator=(const smart_ptr<T>& o)
    {
        if (my_value) delete my_value;
        if (o.my_value)
        {
            my_value = new T(*(o.my_value));
        }
        else
            my_value = 0;
        return *this;
    }
    operator T&(){if (my_value) return *my_value;throw "Null pointer in smart_ptr!";};
    T& operator* (){if (my_value) return *my_value;throw "Null pointer in smart_ptr!";};
    T operator*  () const{if (my_value) return *my_value;throw "Null pointer in smart_ptr!";};;
    operator T* (){if (my_value) return my_value;return 0;};
    operator T* () const {if (my_value) return my_value;return 0;};
private:
    T* my_value;
};

typedef smart_ptr<chord_info> chord_info_ptr;
typedef smart_ptr<measure_feature> measure_feature_ptr;
typedef smart_ptr<voice_info> voice_info_ptr;
typedef smart_ptr<string> word_ptr;

// CLASS: music_info
// DESCRIPTION:
// This is a union of all things that can be in a tune that are
// related to the music at any given point.  The idea is that you
// can construct a big supermap of this to make it easy to iterate
// through the music for midi and display output.
class music_info
{
public:
    music_info();
    music_info(const music_info& o);
    music_info& operator=(const music_info& o);
    ~music_info();
    chord_info_ptr the_chord_info;
    measure_feature_ptr the_measure_feature;
    voice_info_ptr the_voice_info;
    word_ptr the_words;
};

}
#endif



