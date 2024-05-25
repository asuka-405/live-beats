#ifndef iabc_key_sig_h
#define iabc_key_sig_h
#include "iabc/figure.h"
#include "iabc/figure_factory.h"
#include "iabc/pitch.h"
#include "iabc/note_info.h"
#include "iabc/staff_info.h"
#include "iabc/win_page.h"

namespace iabc
{
;

// ATTRIBUTES:
// Tables of the pitches for the various keys.
typedef pitch key_sig_pitches[7];
extern key_sig_pitches treble_sharps;
extern key_sig_pitches treble_flats;
extern key_sig_pitches base_sharps;
extern key_sig_pitches base_flats;

// CLASS: key_sig
// DESCRIPTION:
// Handle all the logic and resources associated with drawing a key
// signature on a staff.
class key_sig
{
public:
    // METHOD: key_sig (ctor)
    // DESCRIPTION:
    // Just populate variables.
    key_sig(const pitch& the_key,
            pitch::clef the_clef,
            const scale& the_scaling_factor,
            const pitch& the_last_key);
    ~key_sig();

    // METHOD: draw_self
    // DESCRIPTION:
    // draw yourself on the page, on the staff supplied.  Draw the ending bar if this is
    // an inline key change.
    virtual void draw_self(figure_container_if& the_page,
                           window& the_window,
                           const staff_info& the_staff,bool the_draw_bar = false); 

    pitch get_key(){return my_key;};
    int get_width(const size &the_ppi,bool the_draw_bar = false);
    int get_x_offset(const size& the_ppi);
private:
    // populates teh sharp or flat figure
    void make_figure(const size& the_size,bool the_draw_bar);

    bool is_sharp_key();
    bool is_flat_key();
    bool is_sharp_key(const pitch& the_pitch);
    bool is_flat_key(const pitch& the_pitch);
    bool is_last_sharp_key();
    bool is_last_flat_key();

    // returns the appropriate array based on key and clef
    key_sig_pitches* get_pitches(const pitch& the_pitch);
    
    // returns the appropriate array based on key and clef
    key_sig_pitches* get_natural_pitches();
    
    // METHOD: get_number_figures
    // DESCRIPTION:
    // gets the number of sharps or flats
    int get_number_figures(const pitch& the_pitch);

    // METHOD: get_number_naturals
    // DESCRIPTION:
    // If this is an inline key change, indicate how many naturals to put down
    // if we are going to a more-natural key.
    int get_number_naturals();

    figure* my_sharp;
    figure* my_flat;
    figure* my_natural;
    figure* my_bar;
    pitch my_key;
    pitch my_last_key;
    int my_horizontal_offset;
    pitch::clef my_clef;
    mutex my_mutex;
    scale my_scaling_factor;
};
}

#endif


