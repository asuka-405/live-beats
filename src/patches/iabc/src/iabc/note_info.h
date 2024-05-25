#ifndef iabc_noteinfo_h
#define iabc_noteinfo_h
#include "iabc/pitch.h"
#include "iabc/wd_data.h"
#include "iabc/array.h"
#include "iabc/mutex.h"

namespace iabc
{
;
// FILE:  note_info.h
// DESCRIPTION:
// This file contains classes that represent the notes and chords
// in the music.

// CLASS: note_info
// DESCRIPTION:
// A note info consists of a pitch and a duration.  We also keep an extra
// flag to indicate whether or not this note is a rest.
class note_info
{
public:

    note_info(bool the_is_rest = true,bool the_is_visible = true):
        my_is_rest(the_is_rest),my_is_visible(the_is_visible),
        my_starts_slur(false),my_ends_slur(false),my_starts_tie(false),
        my_ends_tie(false){};
    note_info(const note_info& o)
    {
        my_pitch = o.my_pitch;
        my_duration = o.my_duration;
        my_is_rest = o.my_is_rest;
        my_is_visible = o.my_is_visible;
        my_starts_slur = o.my_starts_slur;
        my_ends_slur = o.my_ends_slur;
        my_starts_tie = o.my_starts_tie;
        my_ends_tie = o.my_ends_tie;
    };

    note_info(const pitch& the_pitch,
              const fraction& the_duration,
              bool the_is_rest = false,
              bool the_is_visible = true)
    {
        my_pitch = the_pitch;
        my_duration = the_duration;
        my_is_rest = the_is_rest;
        my_is_visible = the_is_visible;
        my_starts_slur = false;
        my_ends_slur = false;
        my_starts_tie = false;
        my_ends_tie = false;
    };
    note_info& operator=(const note_info& o)
    {
        my_pitch = o.my_pitch;
        my_duration = o.my_duration;
        my_is_rest = o.my_is_rest;
        my_is_visible = o.my_is_visible;
        my_starts_slur = o.my_starts_slur;
        my_ends_slur = o.my_ends_slur;
        my_starts_tie = o.my_starts_tie;
        my_ends_tie = o.my_ends_tie;
		return *this;
    };
    ~note_info(){};
    fraction get_duration() const {return my_duration;};
    pitch get_pitch() const {return my_pitch;};
    bool is_rest() const {return my_is_rest;};
    string as_string() const;
    
    // METHOD: starts_slur
    // DESCRIPTION:
    // You can slur to and from a chord to another chord.  Allow
    // getting and setting of this attribute
    bool starts_slur() const{return my_starts_slur;};
    void start_slur(bool the_value = true){my_starts_slur = the_value;};
    bool ends_slur() const{return my_ends_slur;};
    void end_slur(bool the_value = true){my_ends_slur = the_value;};
    bool starts_tie() const{return my_starts_tie;};
    void start_tie(bool the_value = true){my_starts_tie = the_value;};
    bool ends_tie() const{return my_ends_tie;};
    void end_tie(bool the_value = true){my_ends_tie = the_value;};
    void change_length(const fraction& the_product,const fraction& the_sum) 
    {
        my_duration *= the_product;
        my_duration += the_sum;
    };
    void change_pitch(const pitch& the_pitch){my_pitch = the_pitch;};
private:
    pitch my_pitch;
    fraction my_duration;
    bool my_starts_slur;
    bool my_ends_slur;
    bool my_starts_tie;
    bool my_ends_tie;
    bool my_is_rest;
    bool my_is_visible;
};

// CLASS: chord_info
// DESCRIPTION:
// In ABC you can use a chord almost anywhere you can use a note, so
// a chord is the main abstraction that represents music.  A chord
// info is an array of note_infos, plus some additional information
// about how the music is to be played and notated.
class chord_info
{
public:
    typedef enum dynamic
    {
        ppp,
        pp,
        p,
        mp,
        mf,
        f,
        ff,
        fff,
        start_cresc,
        end_cresc,
        start_decresc,
        end_decresc
    } dynamic;

    typedef enum embellishment
    {
        none,
        accent,
        staccato,
        accent_staccato,
        hat,
        tenuto,
        trill,
        mordant,
        gracing,
        up_bow,
        down_bow,
        fermata,
        coda,
        signo
    } embellishment;

    chord_info();
    chord_info(const chord_info& the_info);
    chord_info(const pitch& the_pitch,const fraction& the_duration,
               bool the_starts_slur = false,bool the_ends_slur = false,
               bool the_starts_beam_group = true);
    ~chord_info();
    
    // METHOD: clone
    // DESCRIPTION:
    // Return a copy of this note, including a deep-copy of the array.
    // A shallow copy of the array happens during copy ctor or = operator.
    chord_info clone();

    // METHOD: is_beamable
    // DESCRIPTION:
    // Return true if this note is short enough to beam
    bool is_beamable();

    // METHOD: get_duration
    // DESCRIPTION:
    // In order to figure out what beat we're on, we need to give each
    // chord a concept of a duration.  We say that the shortest duration
    // note in a chord is the true duration, since that should determine
    // where the next beat in a measure will fall.
    fraction get_duration() const;

    // METHOD: change_length
    // DESCRIPTION:
    // Change the length of each note in the chord by multiplying by
    // product and adding sum.
    void change_length(const fraction& product,
                       const fraction& sum);

    bool is_first_in_beam() const;
    void set_first_in_beam(bool the_first_in_beam = true){my_first_in_beam = the_first_in_beam;};
    bool is_last_in_beam() const;
    
    void set_last_in_beam(bool the_last_in_beam = true){my_last_in_beam = the_last_in_beam;};

    // METHOD: get_high_note
    // DESCRIPTION:
    // Get the highest note in the chord
    note_info get_high_note() const;
    note_info get_low_note() const;

    // METHOD: get_next_in_beam
    // DESCRIPTION:
    // When we connect 2 chords in a beam we create a 2-directional list,
    // so we can get the next and last in the beam group.
    // (so if foo.get_next_in_beam() == foo there's no subsequent notes.)
    chord_info get_next_in_beam();

    // METHOD: get_size
    // DESCRIPTION:
    // the number of notes in the chord.
    int get_size() const;

    // METHOD: as_string
    // DESCRIPTION:
    // Return a string representation of the chord, for debugging
    string as_string() const;

    // METHOD: operator[]
    // DESCRIPTION:
    // Treat a chord info like an array of chords.
    note_info operator[](int the_index) const;
    note_info& operator[](int the_index);

    // METHOD: operator=, operator==, += etc.
    // DESCRIPTION:
    // We want a chord to be treated like a simple type
    // with regard to assignment and concatenation
    chord_info& operator=(const chord_info& o);
    chord_info& operator+=(const note_info& o);

    // METHOD: set_embellishment
    // DESCRIPTION:
    // A chord can have an embellishment like an accent.  Allow setting
    // and getting of that.
    void set_embellishment(chord_info::embellishment the_embellishment)
        {my_embellishment = the_embellishment;};
    chord_info::embellishment get_embellishment() const{return my_embellishment;};

    // METHOD: get/set_description
    // DESCRIPTION:
    // A chord can have a description, for example [CEG] can be
    // C.  Allow the chord to save its description.
    string get_description() const {return my_description;};
    void set_description(const string& the_string)
    {my_description = the_string;};

    // METHOD: starts_slur
    // DESCRIPTION:
    // You can slur to and from a chord to another chord.  Allow
    // getting and setting of this attribute by looking at the
    // contained notes of the chord
    bool starts_slur() const;
    void start_slur(bool the_value = true);
    bool ends_slur() const;
    void end_slur(bool the_value = true);
    bool starts_tie() const;
    void start_tie(bool the_value = true);
    bool ends_tie() const;
    void end_tie(bool the_value = true);
    
    // METHOD: get/set is_grace
    // DESCRIPTION:
    // Return true if this is a grace note
    bool is_grace() const{return my_is_grace;};
    void set_grace(bool the_value = true){my_is_grace = the_value;};
    int get_xlet_value() const {return my_xlet_value;};
    void set_xlet_value(int the_value){my_xlet_value = the_value;};
    void set_words(const string& the_words);
    void set_words(array< string >& the_words);
    void set_dynamic(chord_info::dynamic the_dynamic);
    dynamic get_dynamic() const;
    string get_dynamic_string () const;
    array<string> get_words() const;
    static string get_dynamic_string(dynamic);
    static fraction the_longest_beamed_thing;
private:
    void set_equal(const chord_info& o);

    // ATTRIBUTE: my_embellishment
    // DESCRIPTION:
    // stacatto, etc.
    embellishment my_embellishment;

    dynamic my_dynamic;

    // ATTRIBUTE: my_description
    // DESCRIPTION:
    // A textual description of the chord, e.g. 'Am7'.
    string my_description;

    // ATTRIBUTE: my_words
    // DESCRIPTION:
    // words that go along with this particular beat.
    array<string> my_words;

    // ATTRIBUTE: my_notes
    // DESCRIPTION:
    // The notes that make me a chord
    array<note_info> my_notes;

    // ATTRIBUTE: my_first_in_beam
    // DESCRIPTION:
    // true if this chord is the first in a beam group
    bool my_first_in_beam;

    // ATTRIBUTE:
    // my_last_in_beam
    // DESCRIPTION:
    // true if this is the last note in a beam group.
    bool my_last_in_beam;

    // ATTRIBUTE: my_xlet_value
    // DESCRIPTION:
    // Indicates that it's supposed to be grouped with a number,
    // like a triplet is.
    int my_xlet_value;

    // ATTRIBUTE: my_is_grace
    // DESCRIPTION:
    // pretty self-explanatory
    bool my_is_grace;

    // Check for memory leaks.
    static int static_chord_count;
private:
};

}

#endif


