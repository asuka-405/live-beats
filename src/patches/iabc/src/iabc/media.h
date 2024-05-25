#ifndef iabc_score_point_h
#define iabc_score_point_h

#include "iabc/list.h"
#include "iabc/music_info.h"
#include "iabc/map.h"
#ifdef _DEBUG
#define score_point sp_
#endif
namespace iabc
{
;
// FILE: media.h
// DESCRIPTION:
// Classes that define how iabc handles media.  A media is a thing
// that the music is presented on.  Probably the most common media
// is a window on the screen, but it will include other types.  The
// media is populated with information about the music from a media
// source, which could be something like an abc parser.
//
// In score_point, the music looks like this:
// V:1
// ABCD EFGA ||
// V:2
// CDEF GABc ||

// In score_time, it looks like this:
// [AC][BD][CE][DF] ...

// CLASS: score_point
// DESCRIPTION:
// A score point is an ordinal type that represents a particular
// beat in a particular measure in a particular voice.
class score_point
{
public:
    int measure;
    fraction beat;
    int voice;
    score_point(int the_measure,fraction the_beat,int the_voice);
    score_point(const score_point& o);
    score_point& operator=(const score_point& o);
    score_point();
    bool operator==(const score_point& o) const;
    bool operator!=(const score_point& o) const;
    bool operator>(const score_point& o) const;
    bool operator<(const score_point& o) const;
    bool operator>=(const score_point& o) const;
    bool operator<=(const score_point& o) const;
};

typedef map<score_point,voice_info> voice_info_map;
typedef map<score_point,string> words_map;

// CLASS: score_time
// DESCRIPTION:
// Sometimes we need to sort by voice last instead of
// first, like when we're playing all the voices back at once.
// To do this we use the score_time class as an ordinal.
class score_time
{
public:
    int measure;
    fraction beat;
    int voice;
    score_time(int the_measure,fraction the_beat,int the_voice);
    score_time(const score_time& o);
    score_time& operator=(const score_time& o);
    score_time();
    bool operator==(const score_time& o) const;
    bool operator!=(const score_time& o) const;
    bool operator>(const score_time& o) const;
    bool operator<(const score_time& o) const;
    bool operator>=(const score_time& o) const;
    bool operator<=(const score_time& o) const;
};

// Allow easy exchange between one and the other
score_time as_score_time(const score_point& the_point);
score_point as_score_point(const score_time& the_time);

// Allow a NULL conversion to allow template classes to work
score_point as_score_point(const score_point& the_point);
score_time as_score_time(const score_time& the_point);


// CLASS: tune
// DESCRIPTION:
// Contains all the information that can make a tune,
// including all the music, voices, key changes, etc.  A single
// abc file could contain many small tunes, or one big one.
class tune
{
public:
    tune():index(1){};
    tune& operator=(const tune& o);
    tune(const tune& o);
    string title;
    string copyright;
    int index;
    map<score_point,chord_info> my_music;
    map<score_point,measure_feature> my_measures;
    map<score_point,voice_info> my_voice_info; 
    words_map my_words;
    list<string> my_songwords;
};

// METHOD: extract_music_from_tune_point
// DESCRIPTION:
// Sometimes its nice to have all the streams of music
// information into one map for easy iteration.  This 
// function combines the maps in a tune into on supermap.
// This function sorts by score_point.
map<score_point,music_info>
    extract_music_from_tune_point(tune& the_tune);

// METHOD: extract_music_from_tune_voice
// DESCRIPTION:
// Extract the music from the tune for the given voice only.
map<score_point,music_info>
    extract_music_from_tune_voice(tune& the_tune,int the_voice);


// METHOD: extract_music_from_tune_time
// DESCRIPTION:
// same as the other extract, except sorts by score_time
map<score_time,music_info>
    extract_music_from_tune_time(tune& the_tune);

typedef map<int,tune> tunemap;

// CLASS: voice_map_source
// DESCRIPTION:
// The media needs to get a list of voices that we want to display.
// There may be more voices in the source.  Create an abstract
// class for this.
class voice_map_source
{
public:
    voice_map_source(){};
    ~voice_map_source(){};
    virtual map<int,bool> get_voice_map() = 0;
};

// CLASS: media
// DESCRIPTION:
// Media represents a place where the music is presented to the user.  The
// media source populates the media with information about the music.  The
// media presents it to the user.
class media
{
public:
    // METHOD: media (constructor)
    media(voice_map_source* the_voice_map = 0);

    // METHOD: ~media (virtual destructor)
    virtual ~media();
    
    // METHOD: setup
    // DESCRIPTION:
    // Get any resources and parameters neccessary to display the music
    // on the media
    virtual bool setup() = 0;

    // METHOD: complete
    // DESCRIPTION:
    // Notify the media that that's all the music there is.
    virtual void complete() = 0;

    // METHOD: present
    // ARGS: chord_info& - the chord to present to the user
    //       const score_point& - where to present it
    void present(const tune& the_tune);

    // METHOD: present
    // ARGS: measure_feature& - a measure has been completed in some way, handle that
    //       const score_point& - where in the music to present it
    void present(measure_feature& the_feature,const score_point& the_part);

    // METHOD: present
    // ARGS: measure_feature& - a measure has been completed in some way, handle that
    //       const score_point& - where in the music to present it
    void present(chord_info& the_feature,const score_point& the_part);

    // METHOD: present
    // ARGS: const string& - the string to draw below the staff
    // DESCRIPTION:
    // This only applies to visual renderings of the music.  Typically abc programs
    // use this parameter to display information about dynamics and stuff.
    void present(const string& the_string,const score_point& the_part);

    // METHOD: get_voice_info
    // ARGS: const score_point& - the current point in the music.
    // DESCRIPTION: 
    // A part info is a bunch of static information that applies to the music, but
    // could change at anytime.  One example is the key signature.  Get the current
    // set of information for this point in the music.
    voice_info get_voice_info(const score_point& the_point);

    void map_voices_names();

    // ATTRIBUTE: map<int,bool>& my_voice_on_map
    // DESCRIPTION:
    // Keep track of the voices in the tune, and allow people to
    // change whether or not they are on/off
    map<int,string> my_voice_name_map;
protected:

    // METHOD: is_voice_turned_on
    // DESCRIPTION:
    // Return true if this voice has not been turned off.
    bool is_voice_turned_on(const score_point&);

    // METHOD: render_music_feature
    // DESCRIPTION:
    // The base class overrides this method to actually present the music to the user
    virtual void render_music_feature(const chord_info& the_feature,const score_point& the_point) = 0;

    // METHOD: render_music_feature
    // DESCRIPTION:
    // The base class overrides this method to actually present the end of the measure
    // to the user
    virtual void render_measure_feature(const measure_feature& the_feature,const score_point& the_point) = 0;

    // CLASS: repeat_info
    // DESCRIPTION:
    // information about a particular repeat, including which measure its
    // located in, and nth endings around the repeat.
    class repeat_info
    {
    public:
        int my_measure;
        int my_ending_start_measure;
        int my_ending_number;
    };

    // ATTRIBUTE: my_repeat_info
    // DESCRIPTION:
    // A map of measures to repeat infos.
    map<int,repeat_info> my_repeat_info;
    
    // METHOD: populate_repeat_info
    // DESCRIPTION:
    // Go through the tune and figure out which repeats go where in the
    // song.
    virtual void populate_repeat_info();

    // METHOD: count_voices
    // DESCRIPTION:
    // Count the number of voices in the tune.
    int count_voices();
    
    voice_map_source* my_voice_map_source;

    tune my_tune;
};

// CLASS: tune_chooser
// DESCRIPTION:
// Encapsulate a list of tunes, and provide an interface
// for allowing a user to choose one from a user interface of
// some kind...
class tune_chooser
{
public:
    tune_chooser():is_dirty(false),my_current_index(0){};
    virtual ~tune_chooser(){};
    // METHOD: add_tune
    // DESCRIPTION:
    // Add a tune to the list of tunes.
    void add_tune(const tune& the_tune);

    // METHOD: clear_tunes
    // DESCRIPTION:
    // Clear the music out of the tunes, but leave the
    // title and index of each tune.
    void clear_tunes();

    // METHOD: get_choice
    // DESCRIPTION:
    // Abstract method that gets the current tune choice from the user.
    virtual int get_choice() = 0;

    // METHOD: get_choice
    // DESCRIPTION:
    // Abstract method that clears the list for new parsing.
    virtual void remove_all_tunes() = 0;

    // METHOD: operator[]
    // DESCRIPTION:
    // Allow the client to treat the list like an array to get or
    // modify elements.
    tune& operator[](int the_index);
    int get_size() const;
protected:
    // ATTRIBUTE: is_dirty
    // DESCRIPTION:
    // This indicates whether or not we need to repopulate the
    // box, if data has changed.
    bool is_dirty;

    tunemap my_tunes;
private:
    int my_current_index;
};

// CLASS: media_source
// DESCRIPTION:
// A media source is a thing that defines the music that a media object presents to the
// user.  The most common example would be an abc parser.
class media_source
{
public:
    media_source(tune_chooser& the_chooser);
    virtual ~media_source();
    virtual void parse(media& the_media) = 0;
    void set_voice(int the_voice);
    int get_voice(){return my_current_voice;};

    // METHOD: add_music_feature
    // DESCRIPTION:
    // Add a music feature (e.g. a note) to the current tune on the given beat.
    void add_music_feature(chord_info& the_feature,int the_measure,const fraction& the_beat);
    
    // METHOD: add_music_feature
    // DESCRIPTION:
    // Add a measure feature (e.g. a bar line) to the current tune on the given beat.
    void add_measure_feature(measure_feature& the_feature,int the_measure,const fraction& the_beat);

    // METHOD: set_line_break
    // DESCRIPTION:
    // Look up the measure feature for the last beat before this measure and make
    // it a line break.
    void set_line_break(int the_measure);

    // METHOD: add_words
    // DESCRIPTION:
    // add a word to the chord at the given time, in the current tune and voice.
    void add_words(const string& the_word,int& the_measure,fraction& the_beat);
    
    // METHOD: add_songwords
    // DESCRIPTION:
    // These are the words that show up after the music.
    void add_songwords(const string& the_word);

    // METHOD: change_voice_info
    // ARGS: const score_point& - where to change the voice information
    //       const string& the_title - title of the current tune
    //       int the_index - the index of the current tune
    //       const voice_info& - the voice info to display
    // DESCRIPTION:
    // The state of the music that is being played/displayed has changed in some way,
    // for example the key signature.
    void change_voice_info(const score_point& the_point,
                          const string& the_title,
                          const string& the_copyright,
                          int the_index,
                          const voice_info& the_info);
    
    // METHOD: get_voice_info
    // DESCRIPTION:
    // Get the voice info for as close to the given point in the
    // score as we can.
    voice_info get_voice_info(const score_point& the_point);

    // METHOD: get_tune
    // DESCRIPTION: 
    // We have read in some tunes.  Get the tune at index the_index,
    // and return an empty string when there are no more.
    tune& get_tune(int the_index);

    // METHOD: clear_tunes
    // DESCRIPTION:
    // Clear the music out of each tune.  This is done to prepare for a 
    // new parsing.  Leave the title and index number in each tune
    // so we don't have to redo the quick parse.
    void clear_tunes();

    // METHOD: remove_all_tunes
    // DESCRIPTION:
    // Remove all tunes from the tune list.  This is
    // done to prepare a modified file for new parsing.
    void remove_all_tunes();

protected:
    // METHOD: get_next_non_rest
    // DESCRIPTION:
    // Start looking at the_measure, the_beat in the current tune,
    // and iterate through the music until a non-rest is recovered.
    void get_next_non_rest(int& the_measure,fraction& the_beat);

    tune& get_current_tune();
    tune_chooser* my_tunes;
    int my_current_voice;
    int my_current_tune;
    media* my_media;
};
}
#endif

