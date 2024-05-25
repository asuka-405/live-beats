#ifndef iabc_media_h
#define iabc_media_h
#include "iabc/threads.h"
#include "iabc/winres.h"
#include "iabc/media.h"
#include "iabc/abcrules.h"
#include "iabc/preprocess.h"
#include "iabc/iabc_parse.h"


namespace iabc
{
;
// CLASS: iabc_media
// DESCRIPTION:
// This class reads in an abc file, and converts it to the
// universal internal format.  Then it sends this internal music
// to the media for the music to be realized.
class iabc_media:public media_source
{
public:
    // METHOD: iabc_media(ctor)
    // DESCRIPTION:
    // Store the file that we will open later and try to parse.
    iabc_media(tune_chooser& the_tunes,preprocess& the_input);
    virtual ~iabc_media();

    // METHOD: parse(media& the_media)
    // DESCRIPTION:
    // parse the input file and tell the media what we've found so the
    // music can be realized somehow.
    virtual void parse(media &the_media);

    // METHOD: set_modified
    // DESCRIPTION:
    // The buffer has been modified.  Set up the last modified time as the current time.
    void set_modified(){my_last_modify_time = time(0);};

    // METHOD: get_start_line_of_tune
    // DESCRIPTION:
    // Return the line that the tune we just partsed started on.
    long get_start_line_of_current_tune();

    void reset_input(preprocess& the_new_input)
    {
        my_input_buffer = &the_new_input;
    }

    map<int,score_point> get_lines_to_score_points()
    {
        return my_lines_to_score_points;
    }

    map<score_point,int> get_score_points_to_lines()
    {
        return my_score_points_to_lines;
    }
private:
    // METHOD: present_all
    // DESCRIPTION:
    // We have parsed a file.  Now display all the music.
    void present_all();

    // METHOD: get_points
    // DESCRIPTION:
    // We have changed parts so we weren't reading the music in in order.
    // Get the points we saved the last time, and save the current point
    // of measure and chord for when we change back to this part/voice.
    void get_points(int the_part,score_point& the_measure_point,score_point& the_chord_point);
    void save_points(const score_point& the_measure_point,const score_point& the_chord_point);

    // METHOD: get_current_tune
    // DESCRIPTION:
    // Get the current tune selection from the GUI, and also save that in the
    // registry for next time we open the file.
    int update_current_tune();

    map<int,score_point> my_saved_measure_points;
    map<int,score_point> my_saved_chord_points;
    iabc_parse* my_parser;

    preprocess* my_input_buffer;
    time_t my_last_modify_time;
    time_t my_last_parse_time;

    // ATTRIBUTE: my_has_parsed
    // DESCRIPTION:
    // Indicates whether or not we have parsed a file already so we can
    // display different parts of a file without reloading the file, which
    // can take a long time if the file is big.
    bool my_has_parsed;

    // ATTRIBUTE: int my_number_of_tunes
    // DESCRIPTION:
    // The number of tunes in the parser, used to see if we need to
    // re-parse the file.
    int my_number_of_tunes;

    map<int,score_point> my_lines_to_score_points;
    map<score_point,int> my_score_points_to_lines;
};
}
#endif



