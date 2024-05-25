#ifndef iabc_midi_media_h
#define iabc_midi_media_h
#include "iabc/media.h"
#include "iabc/map.h"

namespace iabc
{
;
// CLASS: midi_media
// DESCRIPTION:
// Output to a midi file, and hopefully someday play
// midi file as well.
class midi_media:public media
{
public:
    midi_media(const string& midi_file,const score_point& start_point,
               voice_map_source* the_voice_map_source = 0);
    virtual ~midi_media();

    // METHOD: setup
    // DESCRIPTION:
    // Get any resources and parameters neccessary to display the music
    // on the media
    virtual bool setup();

    // METHOD: complete
    // DESCRIPTION:
    // Notify the media that that's all the music there is.
    virtual void complete();

protected:
    // METHOD: store_data_for_voice
    // DESCRIPTION:
    // Store all the track data for the given voice in the array.
    void store_data_for_voice(int the_voice,array<unsigned char>& the_array);

    // METHOD: update_chords_for_ties
    // DESCRIPTION:
    // Fix the chords so that tied notes actually sound like one note.
    void update_chords_for_ties(int the_number_voices);

    // METHOD: render_music_feature
    // DESCRIPTION:
    // The base class overrides this method to actually present the music to the user
    virtual void render_music_feature(const chord_info &the_feature, const score_point &the_point);
    
    // METHOD: render_music_feature
    // DESCRIPTION:
    // The base class overrides this method to actually present the end of the measure
    // to the user
    virtual void render_measure_feature(const measure_feature &the_feature, const score_point &the_point){};

    // METHOD: count_outstanding_grace
    // DESCRIPTION:
    // The music for grace notes is stored somewhat deceptively: we give grace
    // notes some time, even though they're sort of 'out of time' to make
    // them display properly.  But we need to compensate for grace notes, put
    // them before the next beat and subtract the delta time.
    int count_outstanding_grace(const score_point& the_point);

    class note_on_event
    {
    public:
        unsigned short my_voice;
        unsigned char my_note;
        unsigned long my_start_point;
        unsigned long my_end_point;
    };

    // METHOD: check_voice_info_change
    // DESCRIPTION:
    // If the voice info has changed (tempo, meter, etc.)
    // send an midi meta-event to handle that.
    void check_voice_info_change(const voice_info& the_old,
                                 const voice_info& the_current,
                                 array<unsigned char>& the_array);

    // METHOD: count_beats
    // DESCRIPTION:
    // Count the beats from the_start to the_end.
    fraction count_beats(const score_time& the_start,
                         const score_time& the_end);

    // METHOD: write_header
    // DESCRIPTION:
    // Make sure we write the midi header if we haven't already.
    void write_header();

    // METHOD: store_time_signature
    // DESCRIPTION:
    // Store the time signature in the music array as a midi
    // meta-event.
    void store_time_signature(array<unsigned char>& the_array,const fraction& the_time_signature);
    
    // METHOD: store_time_signature
    // DESCRIPTION:
    // Store the tempo in the music array as a midi
    // meta-event.
    void store_tempo(array<unsigned char>& the_array,
                     int the_beats_per_minute,
                     const fraction& the_division_per_beat);
    
    // METHOD: store_track_end
    // DESCRIPTION:
    // Store the track end magic cookie in the array.
    void store_track_end(array<unsigned char>& the_array);
    
    // METHOD: store_program_change
    // DESCRIPTION:
    // Store the program change for the given voice in the given
    // stream.
    void store_program_change(array<unsigned char>& the_array,
                     unsigned char the_channel,
                     unsigned char the_program);    

    // METHOD: write_absolute
    // DESCRIPTION:
    // Write the given value in midi variable-sized format
    void store_variable(array<unsigned char>& the_array,unsigned long the_value);

    // ATTRIBUTE: my_next_note_off
    // DESCRIPTION:
    // Keep track of where the next note off point goes,
    // if the point we're currently talking about is beyond that.
    map<unsigned long,note_on_event> my_note_off_map;
    
    // METHOD: handle_note_off
    // DESCRIPTION:
    // Go through the array of note off events pending and see if we owe the
    // world any note off events.  Spit them out to the file, and return
    // the point where we ended up before any deltas.
    void handle_note_off(array<unsigned char>&,unsigned long the_duration);

    // METHOD: pitch_to_midi
    // DESCRIPTION:
    // Convert the internal pitch format to the midi format
    unsigned char pitch_to_midi(pitch& the_pitch);

    // METHOD: duration_to_midi
    // DESCRIPTION:
    // convert the internal duration format to a the number of midi
    // ticks.  Use the current tempo as a guide.
    unsigned short duration_to_midi(const fraction& the_duration,const score_point& the_point);

    // METHOD: get_tempo_from_time_signature.
    // DESCRIPTION:
    // Some files don't declare a tempo; here we guess what the tempo is based
    // on the time signature that is used through most of the piece.  E.g.
    // cut time tunes go twice as fast.
    int get_tempo_from_time_signature();

    // ATTRIBUTE: my_midi_data
    // DESCRIPTION:
    // Store the midi data in an array so that we can size if and incorporate
    // that stuff into the MTrk part
    array<unsigned char> my_midi_data;

    // ATTRIBUTE: my_music_map
    // DESCRIPTION:
    // Keep track of all the music in one map so we don't have to
    // keep switching maps.
    map<score_point,music_info> my_music_map;

    // ATTRIBUTE:  my_original_music
    // DESCRIPTION:
    // The music part of the tune.  We need to change this for
    // midi reasons, but we should restore the original after we
    // have constructed the midi part.
    map<score_point,chord_info> my_original_music;

    int my_fd;
	score_point my_start_point;
    string my_filename;
    bool my_need_header;
    unsigned long my_time_since_start;
};
}

#endif
