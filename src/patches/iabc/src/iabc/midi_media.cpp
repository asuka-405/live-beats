#include "iabc/midi_media.h"

#ifdef WIN32
// Curse Microsoft and their underscores...
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define open _open
#define close _close
#define O_WRONLY _O_WRONLY
#define O_CREAT _O_CREAT
#define O_BINARY _O_BINARY
#define O_TRUNC _O_TRUNC
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define O_BINARY 0
#endif

#include <stdio.h>
#include "iabc/map.cpp"
#include "iabc/array.cpp"
#include "iabc/event_log.h"

namespace iabc
{
;

template <class utype>
void write_absolute(int fd,utype the_value)
{
    unsigned char value;
    int tmp_size = sizeof(utype);
    utype mask = (utype)0xff << (tmp_size - 1) * 8;
    int i;
    for (i = 1;i <= tmp_size;++i)
    {
        value = (unsigned char)((the_value & mask) >> ((tmp_size - i) * 8));
        write(fd,(void*)&value,1);
        mask >>= 8;
    }
}

template <class utype>
void store_absolute(array<unsigned char>& ar,utype the_value)
{
    unsigned char value;
    int tmp_size = sizeof(utype);
    utype mask = (utype)0xff << (tmp_size - 1) * 8;
    int i;
    for (i = 1;i <= tmp_size;++i)
    {
        value = (unsigned char)((the_value & mask) >> ((tmp_size - i) * 8));
        ar.add_to_end(&value,1);
        mask >>= 8;
    }
}

typedef struct letter_midi_map_type
{
    pitch::letter my_letter;
    int my_number;
} letter_midi_map_type;

letter_midi_map_type letter_midi_map[] = 
{
    {pitch::A,57},
    {pitch::B,59},
    {pitch::C,60},
    {pitch::D,62},
    {pitch::E,64},
    {pitch::F,65},
    {pitch::G,67}
};

midi_media::midi_media(const string& the_filename,
                       const score_point& the_start_point,
                       voice_map_source* the_voice_map_source):
media(the_voice_map_source),
    my_filename(the_filename),my_fd(0),my_need_header(true),
    my_time_since_start(0),	my_start_point(the_start_point)
{
}

midi_media::~midi_media()
{
    if (my_fd)
    {
        close(my_fd);
    }
}

bool
midi_media::setup()
{
     bool tmp_rv = false;
     my_fd = open(my_filename.access_char_array(),O_CREAT | O_TRUNC | O_BINARY| O_WRONLY,S_IREAD | S_IWRITE);
     if (my_fd)
     {
         tmp_rv = true;
     }

     return tmp_rv;
}

void
midi_media::complete()
{
    if (my_fd)
    {
        int tmp_voice_count = count_voices();
        update_chords_for_ties(tmp_voice_count);
        write_header();

        // The tune has some number of voices, maybe not all of the 
        // voices are 'on'.  Only output sound for the 'on' voices.
        array<int> tmp_voices;
        if (my_voice_map_source)
        {
            map<int,bool>::iterator tmp_map = 
                my_voice_map_source->get_voice_map().least();
            while (tmp_map)
            {
                int tmp_key = (*tmp_map).key;
                if ((*tmp_map).value == true)
                {
                    tmp_voices.add_to_end(tmp_key);
                }

                tmp_map = tmp_map.get_item(tmp_key,gt);
            }
        }
        int i;
        for (i = 0;i <= tmp_voices.get_size();++i)
        {
            my_midi_data.remove_all();
            store_data_for_voice(tmp_voices[i],my_midi_data);
    
            // Now we've got the data in the array, so put it in the file.
            // First add the required midi end part.
            unsigned long tmp_track_size = my_midi_data.get_size();
            char buf[5] = "MTrk";
            write(my_fd,buf,4);
            write_absolute(my_fd,tmp_track_size);
            unsigned long i;
            for (i = 0;i < tmp_track_size;++i)
            {
                unsigned char c = my_midi_data[(int)i];
                write(my_fd,(void*)&c,1);
            }
        }

        close(my_fd);
        my_midi_data.remove_all();
        my_note_off_map.clear();
        my_tune.my_music = my_original_music;
        my_fd = 0;
    }
}

void 
midi_media::update_chords_for_ties(int the_number_voices)
{
    int tmp_current_voice = 1;
    bool tmp_in_tie = false;
    map<score_point,chord_info>::iterator tmp_it = 
        my_tune.my_music.least();

    // Copy the old array to a new array
    map<score_point,chord_info> tmp_new_music;
    score_point tmp_point;
    score_point tmp_tie_point;
    chord_info tmp_info;
    // Step through all the chords in the tune
    while (tmp_it)
    {
        // If the last note we examined was part of a tie, add
        // the duration to it and remove this note.
        if (tmp_in_tie == true)
        {
            score_point tmp_new_point = (*tmp_it).key;
            chord_info tmp_new_info = (*tmp_it).value.clone();
            tmp_in_tie = tmp_new_info.starts_tie();
            tmp_info.change_length(fraction(1,1),tmp_new_info.get_duration());
            tmp_new_music.add_pair(tmp_tie_point,tmp_info);
            tmp_point = tmp_new_point;
        }
        else
        {
            tmp_point = (*tmp_it).key;
            tmp_info = (*tmp_it).value.clone();
            tmp_in_tie = tmp_info.starts_tie();
            tmp_tie_point = tmp_point;
            tmp_new_music.add_pair(tmp_point,tmp_info);
        }
        tmp_it = tmp_it.get_item(tmp_point,gt);
    }

    my_original_music = my_tune.my_music;
    my_tune.my_music = tmp_new_music;
}

void 
midi_media::store_data_for_voice(int the_voice,array<unsigned char>& the_array)
{
    my_music_map.clear();
    my_music_map = extract_music_from_tune_voice(my_tune,the_voice);
    map<score_point,music_info>::iterator tmp_it =
        my_music_map.least();
    voice_info tmp_old_voice_info; 
    score_point tmp_old_key;
    
    // The amount of midi ticks until the next thing should happen
    unsigned long tmp_last_delta = 0;
    // This is how long this note lasts
    unsigned long tmp_this_delta = 0;
    // The midi ticks we give to a grace note
    unsigned long tmp_grace_delta = 12;
    // Keep track of grace notes that we need to account for
    int tmp_grace_outstanding = 0;
    pitch::accidental_map tmp_accidentals;
    while (tmp_it)
    {
        music_info tmp_music = (*tmp_it).value;
        if ((tmp_music.the_chord_info == 0) ||
			((*tmp_it).key.measure < my_start_point.measure))
        {
            tmp_it = tmp_it.get_item((*tmp_it).key,gt);
            continue;
        }
        score_point tmp_key = (*tmp_it).key;
        chord_info tmp_chord = tmp_music.the_chord_info;
        global_midi_event_log.log_event(midi_event_data::play_note,
                                        tmp_key.voice,
                                        tmp_key.measure,
                                        tmp_key.beat.num,
                                        tmp_key.beat.den);
        // Keep track of the voice info (key, etc.) of the current
        // voice.
        voice_info tmp_voice_info = 
            get_voice_info(tmp_key);

        // Update midi time since start to be this chord's time
        unsigned char tmp_midi_velocity;
        if (tmp_chord.is_grace() == true)
        {
            tmp_midi_velocity = 0x20;
            tmp_this_delta = tmp_grace_delta;
            if (tmp_grace_outstanding == 0)
            {
                tmp_grace_outstanding = count_outstanding_grace(tmp_key);
                unsigned long tmp_change = tmp_grace_outstanding * tmp_grace_delta;
                // If we'd go below zero, then don't take a chance.
                if (tmp_last_delta > tmp_change)
                    tmp_last_delta -= tmp_change;
            }
            else
                --tmp_grace_outstanding;
        }
        else
        {
            tmp_midi_velocity = tmp_voice_info.my_midi_volume;
            tmp_this_delta = duration_to_midi(tmp_chord.get_duration(),tmp_key);
        }

        if (tmp_old_key.measure != tmp_key.measure)
        {
            tmp_accidentals.clear();
        }

        // Update any note_off info before doing more note_on stuff.  This has
        // the side-effect of possibly updating the start time so
        // we need to recalculate the delta.
        if (tmp_last_delta > 0)
        {
            unsigned long tmp_last_stop_time = my_time_since_start;
            handle_note_off(my_midi_data,tmp_last_delta);
            tmp_last_delta -= (my_time_since_start - tmp_last_stop_time);
            tmp_old_key = tmp_key;
        }

        store_variable(my_midi_data,tmp_last_delta);
        my_time_since_start += tmp_last_delta;

        check_voice_info_change(tmp_old_voice_info,
                                      tmp_voice_info,
                                      my_midi_data);
        tmp_old_voice_info = tmp_voice_info;

        // All remaining notes start at the same time so delta
        // is 0 from here on out
        int i;
        for (i = 0;i < tmp_chord.get_size();++i)
        {
            // We treat rests as notes of velocity '0'
            if (tmp_chord[i].is_rest() == false)
            {
                tmp_midi_velocity = 0x50;
            }
            else
            {
                tmp_midi_velocity = 0x00;
            }

            // We assume that the midi delta time is 0 since the
            // last note in a chord.  Otherwise the delta time
            // was added above.
            if (i > 0)
            {
                my_midi_data.add_to_end(0);
            }
            pitch tmp_pitch = 
                tmp_chord[i].get_pitch();

            midi_media::note_on_event tmp_event;
            tmp_event.my_note =
                pitch_to_midi(tmp_pitch);
            tmp_event.my_voice = tmp_key.voice - 1;
            fraction tmp_duration = tmp_chord[i].get_duration();
            tmp_event.my_end_point = tmp_this_delta + my_time_since_start;
            tmp_event.my_start_point = my_time_since_start;

            my_note_off_map.add_pair(tmp_event.my_end_point,tmp_event);

            tmp_last_delta = tmp_this_delta;
            unsigned char tmp_status = 
                0x90 | tmp_event.my_voice;
            unsigned char tmp_midi_key = tmp_event.my_note;
            unsigned char tmp_velocity = tmp_midi_velocity;
            my_midi_data.add_to_end(tmp_status);
            my_midi_data.add_to_end(tmp_midi_key);
            my_midi_data.add_to_end(tmp_velocity);
        }
        tmp_it = tmp_it.get_item(tmp_key,gt);
    }

    // Handle any final note_off messages.
    handle_note_off(my_midi_data,tmp_last_delta);
    // Put the track end in there.
    store_track_end(my_midi_data);
}

int 
midi_media::count_outstanding_grace(const score_point& the_point)
{
    int tmp_rv = 0;
    map<score_point,music_info>::iterator tmp_it = my_music_map.get_item(
        the_point);

    while (tmp_it)
    {
        score_point tmp_key = (*tmp_it).key;
        music_info tmp_info = (*tmp_it).value;
        if (tmp_info.the_chord_info)
        {
            chord_info tmp_chord = (*tmp_info.the_chord_info);
            if (tmp_chord.is_grace() == false)
            {
                break;
            }
            ++tmp_rv;
        }
        tmp_it = tmp_it.get_item(tmp_key,gt);
    }

    return tmp_rv;
}

void
midi_media::store_track_end(array<unsigned char>& the_array)
{
    unsigned char tmp_end_track[4] = {0x00,0xff,0x2f,0x00};
    int i;
    for (i = 0;i < 4;++i)
    {
        my_midi_data.add_to_end(tmp_end_track[i]);
    }
}

unsigned char
midi_media::pitch_to_midi(pitch& the_pitch)
{
    unsigned char tmp_note = 
        (unsigned char)letter_midi_map[(int)the_pitch.get_letter()].my_number;
    if (the_pitch.get_accidental() == pitch::sharp)
    {
        ++tmp_note;
    }
    else if (the_pitch.get_accidental() == pitch::flat)
    {
        --tmp_note;
    }
    else if (the_pitch.get_accidental() == pitch::double_flat)
    {
        tmp_note -= 2;
    }
    else if (the_pitch.get_accidental() == pitch::double_sharp)
    {
        tmp_note += 2;
    }

    tmp_note += the_pitch.get_octave() * 12;

    return tmp_note;
}

void 
midi_media::check_voice_info_change(const voice_info& the_old,
                             const voice_info& the_current,
                             array<unsigned char>& the_array)
{
    if (the_old.my_time_signature != the_current.my_time_signature)
    {
        store_time_signature(the_array,the_current.my_time_signature);
    }
    if ((the_old.my_division_per_beat != the_current.my_division_per_beat) ||
             (the_old.my_beats_per_minute != the_current.my_beats_per_minute))
    {
        store_tempo(the_array,the_current.my_beats_per_minute,the_current.my_division_per_beat);
    }
    if ((the_old.my_midi_program != the_current.my_midi_program))
    {
        store_program_change(the_array,
                             (unsigned char)the_current.my_index,
                             (unsigned char)the_current.my_midi_program);
    }
}

void 
midi_media::store_program_change(array<unsigned char>& the_array,
                 unsigned char the_channel,
                 unsigned char the_program)
{
    unsigned char tmp_delta = 0;
    unsigned char tmp_status = 0xC0 + (the_channel - 1);
    the_array.add_to_end(tmp_status);
    the_array.add_to_end(the_program);
    
    // We have already added the delta time to this event.
    // So add a delta time of 0 for the next event.
    the_array.add_to_end(tmp_delta);
}

void 
midi_media::store_tempo(array<unsigned char>& the_array,
                 int the_beats_per_minute,
                 const fraction& the_division_per_beat)
{
    // Midi stores tempo as microseconds per quarter note.
    // So convert.
    double tmp_fraction_of_quarter = 
        the_division_per_beat.as_double() / 0.25;

    double tmp_beats_per_quarter = (double)the_beats_per_minute * 
        tmp_fraction_of_quarter;

    unsigned long tmp_value = (unsigned long)
        ((60.0/tmp_beats_per_quarter) * 1000000.0);

    unsigned char tmp_delta = 0;
    unsigned char tmp_meta = 0xFF;
    unsigned char tmp_midi_tempo = 0x51;
    unsigned char tmp_size = 0x3;
    the_array.add_to_end(tmp_meta);
    the_array.add_to_end(tmp_midi_tempo);
    the_array.add_to_end(tmp_size);
    the_array.add_to_end((unsigned char)(tmp_value >> 16) & 0xff);
    the_array.add_to_end((unsigned char)(tmp_value >>  8) & 0xff);
    the_array.add_to_end((unsigned char)(tmp_value) & 0xff);

    // We have already added the delta time to this event.
    // So add a delta time of 0 for the next event.
    the_array.add_to_end(tmp_delta);
}
void 
midi_media::store_time_signature(array<unsigned char>& the_array,
                                 const fraction& the_time_signature)
{
    unsigned char tmp_delta = 0;
    unsigned char tmp_meta = 0xFF;
    unsigned char tmp_ts = 0x58;
    unsigned char tmp_beats = 0x24;
    unsigned char tmp_32 = 0x8;
    unsigned char tmp_size = 4;
    
    // Midi denominator is the base 2 log of the real denominator
    unsigned char tmp_den_log = 1;
    int i;
    for (i = 0;tmp_den_log < the_time_signature.den;++i)
    {
        tmp_den_log <<= 1;
    }
    tmp_den_log = i;

    the_array.add_to_end(tmp_meta);
    the_array.add_to_end(tmp_ts);
    the_array.add_to_end(tmp_size);
    the_array.add_to_end(the_time_signature.num);
    the_array.add_to_end(tmp_den_log);
    the_array.add_to_end(tmp_beats);
    the_array.add_to_end(tmp_32);

    // We have already added the delta time to this event.
    // So add a delta time of 0 for the next event.
    the_array.add_to_end(tmp_delta);
}

unsigned short 
midi_media::duration_to_midi(const fraction& the_duration,const score_point& the_score_point)
{
    voice_info tmp_info = get_voice_info(the_score_point);
    int tmp_int = fraction(the_duration * 96).as_int();
    tmp_int *= 4;
    return (unsigned short) tmp_int;
}

void 
midi_media::write_header()
{
    if (my_need_header)
    {
        my_need_header = false;
        // First 4 bytes are midi header
        if (my_fd)
        {
            char buf[5] = "MThd";
            write(my_fd,buf,4);
            
            // Length of these headers is always 6 
            unsigned long len = 6;
            write_absolute(my_fd,len);
        
            // Next 2 bytes are the format, we use midi format
            // 1, which is multi-track.
            write_absolute(my_fd,(unsigned short)1);
        
            // Next 2 bytes is the number of voices or tracks.
            int tmp_tracks = count_voices();
            write_absolute(my_fd,(unsigned short)tmp_tracks);
        
            // Next is the tempo.  Need to fix this when we get
            // tempo in the parser.
            write_absolute(my_fd,(unsigned short)96);
            
        }
    }
}

void 
midi_media::store_variable(array<unsigned char>& the_array,unsigned long value)
{
    unsigned long buffer;
    buffer = value & 0x7f;
    while((value >>= 7) > 0)
    {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (value & 0x7f);
    }

    while (1)
    {
        unsigned char c = ((unsigned char)(buffer)) & 0xff;
        the_array.add_to_end(c);
        if (buffer & 0x80)
        {
            buffer >>= 8;
        }
        else
        {
            break;
        }
    }
}

void 
midi_media::render_music_feature(const chord_info &the_feature, const score_point &the_point)
{
    score_time tmp_time = as_score_time(the_point);
    my_tune.my_music.add_pair(the_point,the_feature);
}

void 
midi_media::handle_note_off(array<unsigned char>& the_array,unsigned long the_duration)
{
    map<unsigned long,midi_media::note_on_event>::iterator tmp_it = 
        my_note_off_map.get_item(my_time_since_start,gteq);
    while (tmp_it)
    {
        note_on_event tmp_event = (*tmp_it).value;
        unsigned long tmp_key = (*tmp_it).key;
        if (tmp_event.my_end_point <= the_duration + my_time_since_start)
        {
            unsigned long tmp_delta = (tmp_event.my_end_point -
                                       my_time_since_start);
            my_time_since_start += tmp_delta;
            the_duration -= tmp_delta;
            store_variable(the_array,tmp_delta);
            unsigned char tmp_status = 
                0x80 | tmp_event.my_voice;
            unsigned char tmp_midi_key = tmp_event.my_note;
            unsigned char tmp_velocity = 0x40;
            the_array.add_to_end(tmp_status);
            the_array.add_to_end(tmp_midi_key);
            the_array.add_to_end(tmp_velocity);
            my_note_off_map.remove(tmp_key);
        }
        tmp_it = my_note_off_map.get_item((*tmp_it).key,gt);
    }
}

fraction
midi_media::count_beats(const score_time& the_start,const score_time& the_end)
{
    voice_info tmp_start_info = get_voice_info(as_score_point(the_start));

    fraction tmp_total_beats;
    map<score_point,music_info>::iterator tmp_it = 
        my_music_map.get_item(as_score_point(the_start),gteq);
    while ((tmp_it) && ((*tmp_it).key < as_score_point(the_end)))
    {
        score_point tmp_start = (*tmp_it).key;
        music_info tmp_music = (*tmp_it).value;

        // Don't count beats of non-voice things.  This assumes that
        // all the voices line up just right or the play back will
        // be funky.
        if ((tmp_music.the_chord_info == 0) ||
            (tmp_start.voice != the_end.voice))
        {
            ;
        }
        else
        {
            chord_info tmp_info = tmp_music.the_chord_info;
            tmp_total_beats += tmp_info.get_duration();
        }
        tmp_it = tmp_it.get_item(tmp_start,gt);
    }
    return tmp_total_beats;
}

}
