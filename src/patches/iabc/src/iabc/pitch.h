#ifndef iabc_pitch_h
#define iabc_pitch_h
#include "iabc/wd_data.h"
#include "iabc/map.h"

namespace iabc
{
template <class Type>
int get_element_index(Type* the_array,const Type& the_element,int the_array_size);

#define MAX_SHARPS 7
#define MAX_FLATS 7
#define NOTES_PER_OCTAVE 12
#define LETTERS_PER_OCTAVE 7

// CLASS: pitch
// DESCRIPTION:
// Representation of a pitch.  Consists of a letter note, accidental,
// and octave.
class pitch
{
public:
    typedef enum clef
    {
        TrebleClef,
        BassClef
    } clef;
    typedef enum letter
    {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G
    } letter;
    typedef enum accidental
    {
        sharp,
        flat,
        double_sharp,
        double_flat,
        natural
    } accidental;

    pitch():my_letter(C),my_accidental(natural),my_octave(0){};
    // Sometimes we just care about the letter and accidental part,
    // like when we are specifying key...
    pitch(letter the_letter,accidental the_accidental):
        my_letter(the_letter),my_accidental(the_accidental),my_octave(0){};
    pitch(letter the_letter,accidental the_accidental,int the_octave):
        my_letter(the_letter),my_accidental(the_accidental),my_octave(the_octave){};
    pitch(const pitch& o):
        my_letter(o.my_letter),my_accidental(o.my_accidental),my_octave(o.my_octave){};
    pitch& operator=(const pitch& o)
    {
        my_letter = o.my_letter;
        my_accidental = o.my_accidental;
        my_octave = o.my_octave;
        return *this;
    };
    accidental get_accidental() const {return my_accidental;};
    letter get_letter() const {return my_letter;};
    int get_octave() const {return my_octave;};

    // Return an enharmonic equivalent to self.
    pitch get_enharmonic_equivalent() const;

    // note on these:  They return true for enharmonic equivalents,
    // since the pitches match even though they look different.
    // Contrast to exact_match().
    bool operator==(const pitch& o) const;
    bool operator!=(const pitch& o) const{return !(*this == o);};

    // This uses < to mean 'lower than', etc.
    bool operator<=(const pitch& o) const;
    bool operator>=(const pitch& o) const;

    // This one does a compare of all the attributes, returns false on 
    // enharmonic equivalents
    bool exact_match(const pitch& o) const{ return ((my_accidental == o.my_accidental) &&
        (my_octave == o.my_octave) && (my_letter == o.my_letter));};
    ~pitch(){};

    // METHOD: as_string
    // DESCRIPTION:
    // Returns a string representation of the pitch, but not an ABC string
    // use abc_string for that.
    string as_string() const;

    // METHOD: string_value
    // DESCRIPTION:
    // Returns the pre-parsed value of the string.  Used for transposing ..toABC things 
    string abc_string(const pitch& the_key) const;

    // return the pitch up 1 letter, in the given key.
    pitch up_letter(const pitch& the_key) const;

    // return the pitch 1 letter down from this pitch, in the given key
    pitch down_letter(const pitch& the_key) const;

    // return the pitch up 1 letter, in the given key.
    pitch transpose_diatonic(const pitch& the_old_key,int the_steps) const;

    // Attempt to transpose up/down the given number of 1/2 steps.  Choose
    // the accidentals based on the_old_key, which is the key that we
    // used to be in.
    pitch transpose(const pitch& the_old_key,int the_halfsteps);
    bool operator>(const pitch& o) const;
    bool operator<(const pitch& o) const;

    // This one returns false if this is an enharmonic equivalent to a
    // note in a key.  Contrast to operator==().
    bool is_in_key(const pitch& the_key) const;

    // This stuff helps us track of key signatures
    typedef map<letter,accidental> accidental_map;

    // METHOD: add_accidental
    // ARGS:  accidental_map,const pitch&
    // DESCRIPTION:
    // These can be used by clients keeping track of accidentals
    // to decide what needs to be displayed/played based on what
    // has been played thus far in the measure and the common rules
    // of music notation
    void add_accidental(accidental_map& the_map);

    bool should_print_accidental(accidental_map& the_map,
                                        const pitch& the_key);

    // METHOD: change_accidental
    // DESCRIPTION:
    // Change the accidental to compensate for the key or the forced
    // accidental.  The assumption is that the changed accidental is 
    // already added to the map.
    void change_accidental(accidental_map& the_map,const pitch& the_key);

    // METHOD: get_accidental_from_array
    static pitch::accidental get_accidental_from_array(accidental_map& the_map,
                                                pitch::letter the_letter);
protected:
    // METHOD: has_accidental_changed
    // DESCRIPTION:
    // return true if the accidental for my_letter in the map is 
    // different than my_accidental, or if the accidental doesn't
    // appear in the map at all.
    bool has_accidental_changed(accidental_map& the_map);
    
    // METHOD: is_accidental_in_array
    // DESCRIPTION:
    // Return true if there is an accidental for my_letter in the 
    // map.
    bool is_accidental_in_array(accidental_map& the_map);

    int get_accidental_offset(accidental) const;
    int get_chromatic_index() const;
    pitch normalize() const;
    letter my_letter;
    accidental my_accidental;
    int my_octave;
    
    // This is just a list of all 12 pitches in octave
    // 0 (from B below middle 'C' up)
    static pitch chromatic_up[NOTES_PER_OCTAVE];

    // the flat and sharp keys, and the flats and
    // sharps in those key signatures
    static pitch sharp_keys[MAX_SHARPS];
    static pitch flat_keys[MAX_FLATS];
    static pitch sharps[MAX_SHARPS];
    static pitch flats[MAX_FLATS];
};

}
#endif



