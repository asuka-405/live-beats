#ifndef iabc_line_of_music_h
#define iabc_line_of_music_h
#include "iabc/wd_data.h"
#include "iabc/figure_factory.h"
#include "iabc/mutex.h"
#include "iabc/music_info.h"
#include "iabc/media.h"
#include "iabc/pitch.h"
#include "iabc/key_sig.h"
#include "iabc/staff_info.h"
#include "iabc/map.h"
#include "iabc/note_figure.h"
#include "iabc/music_info.h"
#include "iabc/beamer.h"
#include "iabc/slurer.h"
#include "iabc/text_figure.h"
#include "iabc/time_sig.h"
#ifdef _DEBUG
#define point_note_pair pnp_
#endif

#define DEFAULT_PAGE_WIDTH 8.5
namespace iabc
{
typedef part_point staff_point;
;

// CLASS: line_of_music
// DESCRIPTION:
// A line_of_music is a container for all the stuff that gets drawn on one line
// of music.  It also takes care of drawing fixtures of the staff (like
// the key signature) and the staff itself.
// Aside from that, the line_of_music does the horizontal justification that
// keeps the notes evenly spaced.
class line_of_music
{
	friend class figure_container<line_of_music>;
public:
    // METHOD: line_of_music (constructor)
    // ARGS:
    // window& - the window that I'm drawn on.
    // const scale& - the scale of the drawing stuff.
    // double page_width - the width of the page in inches
    // voice_info& - the starting part information (key, etc)
    // const point& - the upper left hand corner of the staff
    // int - the measure we're starting with.
    // bool - true if we should draw the 'part' symbol.
    line_of_music(window& the_page,
          const scale& the_scale,
          const scale& the_note_scale,
		  double page_width,
          const voice_info& the_info,
          const point& the_origin,
          int the_start_measure,
          bool the_should_draw_first);

    // METHOD: dtor
    // DESCRIPTION:
    // Free all the graphical resources we use to draw ourselves.
    ~line_of_music();

    void line_add_ref();
    void line_remove_ref(){if (--my_count == 0) delete this;};

    // METHOD: add_measure
    // DESCRIPTION:
    // Add another measure to this staff.  Future notes will be added to that measure.
    void add_measure(measure_feature::measure_feature_enum the_begin_break,
                     measure_feature::measure_feature_enum the_end_break,
                     const score_point& the_point,
                     const voice_info& the_info);

    // METHOD: add_nth_ending
    // DESCRIPTION:
    // Add an 1st, 2nd etc. ending bracket over the music between the given measures.
    void add_nth_ending(int the_start_measure,int the_end_measure,int the_ending);

    // METHOD: add_slur_from
    // DESCRIPTION:
    // add a slur from the previous point in the music.  The point in the music
    // where we're slurring to will be added later.
    void add_slur_from(const staff_point& my_point);

    // METHOD: add_slur_to
    // DESCRIPTION:
    // Finish the slur that was last added with add_slur_from
    void add_slur_to(const staff_point& my_point);

    // METHOD: get_start_measure
    // DESCRIPTION:
    // get the measure that we start with
    int get_start_measure() const {return my_start_measure;};
    
    // METHOD: get_end_measure
    // DESCRIPTION:
    // get the measure that we end with
    int get_end_measure() const {return my_end_measure;};
    
    // METHOD: get_number_measures
    // DESCRIPTION:
    // just subtract the from and the to.
    int get_number_measures() const {return 1 + (my_end_measure - my_start_measure);};

    // METHOD: render
    // DESCRIPTION:
    // This draws everything.
    void render();
    
    // METHOD: does_rect_overlap
    // DESCRIPTION:
    // Return true if the rectangle overlaps any of the regions of the
    // staff.  Used by the page manager (window_media) to draw page
    // fixtures without ruiing the music.
    bool does_rect_overlap(const rect& the_rect);

    // METHOD: add_note_figure
    // This adds a feature (note, rest, etc) to the staff at the given time.
    void add_note_figure(note_figure& the_figure,const staff_point& the_point);

    // METHOD: add_crescendo
    // DESCRIPTION:
    // Add a creschendo/dimenuendo to the staff from/to the given point.
    void add_crescendo(const staff_point& the_start_point,
                        chord_info::dynamic the_start_dynamic,
                        const staff_point& the_stop_point,
                        chord_info::dynamic the_stop_dynamic);

    // METHOD: add_dynamic
    // DESCRIPTION:
    // This adds a dynamic change (pp, p, etc) to the list for later rendering
    void add_dynamic(const staff_point& the_point,chord_info::dynamic);

    // METHOD: get_high_point
    // DESCRIPTION:
    // Get the highest y coordinate, which is the lowest point on the screen tha this
    // staff takes up.
    int get_high_point() const {return my_high_point;};
    int get_low_point() const {return my_low_point;};

    // This is all staff_info stuff, so clients can draw themselves on me.
    int get_horizontal_position() const {return my_origin.x;};
    int get_vertical_position() const {return my_origin.y;};
    sizeloc get_size();
    size get_staff_size() const{return my_staff_size.my_size;};
    pitch get_center_pitch() const {return my_center_pitch;};

    void add_figures_to_page(page& the_page,const point& the_offset);

    bool has_rendered() const {return my_has_rendered;};

    // METHOD: add_y_offset
    // DESCRIPTION:
    // We decided this staff needs an offset different that the y
    // coordinate.  Change the y coordinate of the origin.
    void add_y_offset(int the_offset);

    void set_is_complete(bool the_is_complete = true){my_is_complete = the_is_complete;};
    bool get_is_complete () const {return my_is_complete;};

    // METHOD: get_voice_info
    // ATTRIBUTE:
    // Accessor so we can figure out which voice this staff has.
    voice_info& get_voice_info(int the_measure);

    // METHOD: get_lowest_score_point
    // DESCRIPTION:
    // return the lowest musical point on this staff, good for making hashtables.
    score_point get_lowest_score_point() const {return my_lowest_score_point;};
private:
    
    // METHOD: render_ornaments
    // DESCRIPTION:
    // We need to add the ornaments after we draw all the notes cause then
    // we know which way the beams go.  This should be called after the notes
    // are rendered or nothing will happen.
    void render_ornaments();

    // METHOD: operator staff_info()
    // DESCRIPTION:
    // A lot of fixtures need some basic information about a staff before
    // rendering themselves.  This is information about this staff for figures
    // to use when drawing themselves.  It also reflects my current position
    // horizontally as I'm drawing from left to right.
	operator staff_info();
    
    // METHOD: get_width_of_words
    // DESCRIPTION:
    // get the width of the words below the notes.  If they are wider
    // than the note head, use this for the width.
    int get_width_of_words(const chord_info& the_info);

    // METHOD: render_figures
    // DESCRIPTION:
    // render the notes into figures.
    void render_figures();

    // METHOD: render_words
    // DESCRIPTION:
    // render the words for the first time.
    void render_words();

    // METHOD: render_part
    // DESCRIPTION:
    // render the 'part' string, which is the letter marking
    // used at different places in the music.
    void render_part();

    // METHOD: render_dynamics
    // DESCRIPTION:
    // render the dynamics.
    void render_dynamics();

    // METHOD: draw_time_signature
    // DESCRIPTION:
    // draw the time signature on the point at the given location.  Update
    // the point based on my width.
    void draw_time_signature(point& the_origin);

    // METHOD: get_time_signature
    // DESCRIPTION:
    // return the time signature for the given measure.
    fraction get_time_signature(int the_measure);

    // METHOD: draw_beginning_repeat
    // DESCRIPTION:
    // If there is a repeat sign at the beginning of the measure, draw that.
    void draw_beginning_repeat(point& the_origin);

    // METHOD: render_bar_at
    // ARGS:  int the_measure before the bar
    //        key_sig* - inline key change, 0 if none
    //        time_sig* - inline time change, 0 if none
    // DESCRIPTION:
    // Draws the end-of-measure figures
    void render_bar_at(int the_measure,
                       key_sig* the_key_sig,
                       time_sig* the_time_sig);

    // METHOD: get_note_pixels
    // ARGS: note_figure& the_figure
    //       int the_measure - since different measures have different beats
    // DESCRIPTION:
    // We try to spread out the notes on a staff as much as possible.  If we
    // do this in a uniform way, this guarantees that we will always line up
    // parts from the same points of a score, if we can.
    // RETURNS:
    // The number of points that this note figure should take up.  This will
    // always be at least the width of the figure.
    // ASSUMPTIONS:
    // We assume that the get_leftover_pixels() has already been run, as this
    // will use my_rhs_overlap and my_spare_pixels
    int get_note_pixels(note_figure& the_figure,int the_measure);

    // METHOD: get_leftover_pixels
    // DESCRIPTION:
    // Go through all the figures in this staff, and figure out how many
    // pixels they would take up.  Fill in some class information about
    // how many pixels in all so we can justify the notes.
    void get_leftover_pixels();

    // METHOD: get_pixels_from_duration
    // DESCRIPTION:
    // Get the number of pixels a note of this duration shoudl take
    // up, given the number of pixles and number of beats in this
    // staff.
    int get_pixels_from_duration(const fraction& the_duration,int the_measure);

    // METHOD: count_measure_pixels
    // DESCRIPTION:
    // Count the numer of pixels taken by measure end things.  We use
    // this to justify the music
    int count_measure_pixels();

    // METHOD: draw_ledger_lines
    // DESCRIPTION:
    // Determine if we need ledger lines and draw them.
    void draw_ledger_lines(note_figure& the_figure);

    // METHOD: draw_little_numbers
    // DESCRIPTION:
    // Draws the little measure numbers on the left hand side
    void draw_little_numbers();

    // METHOD: get_barline_width
    // DESCRIPTION:
    // Get the width of the barline figure for the end of measure x.
    int get_barline_width(int the_measure);

    // METHOD: get_bar_width
    // DESCRIPTION:
    // Get the width of bar in pixels, once its been drawn.  Assumes that the
    // bar points have been populated already.
    int get_bar_width(int the_measure);

    // METHOD: get_bar_width
    // DESCRIPTION:
    // Get the width of bar in pixels, once its been drawn.
    int get_bar_point(int the_measure);

    // METHOD: create_chord_symbol
    // DESCRIPTION:
    // Create the text figure that goes above/(and somdeday below) the note and
    // compute its x coordinate.  The y location is computed when we have rendered
    // the chord and we know where its supposed to go.
    void create_chord_symbol(note_figure& the_note,const staff_point& the_point);

    // METHOD: render_chords
    // DESCRIPTION:
    // We figure out the chord symbols, but we can't actually render the chords
    // until we have drawn the figures and beamed them all.  Go through the 
    // my_chord_symbols and print them all out.
    void render_chords();

    // METHOD: render_voice_info
    // DESCRIPTION:
    // Add the description of the voice information to the staff.
    void render_voice_info();

    // METHOD:  get_top/bottom note
    // DESCRIPTION:
    // Get the top and bottom note on the staff to deterine if the
    // ledger lines are needed
    pitch get_top_note();
    pitch get_bottom_note();

    // METHOD: get_measure_end_figure
    // DESCRIPTION:
    // Get the appropriate figure for the give figure ending.
    figure* get_measure_end_figure(measure_feature::measure_feature_enum the_feature);

    // METHOD: get_height_of_first_note
    // DESCRIPTION:
    // We need to put something near the first beat of a measure, make sure it
    // doesn't run into a note.
    int get_height_of_first_note(int the_measure);

    // Things that make me special
    
    // ATTRIBUTE: my_figure_list
    // DESCRIPTION:
    // Render the figures into a list first so we can move them down
    // to a different page if we want to.
    map<point_ordinal,figure_array> my_figure_list;
    list<text_figure::point_text_pair> my_text_list;
    void add_text_figure(const point_ordinal& the_point,text_figure&);
    void add_figure(const point_ordinal& the_sort_point,figure&);

    // METHOD: cleanup
    // DESCRIPTION:
    // We create lots of intermediate things when creating the figures.  We don't 
    // need any of it now so release all the resources.
    void cleanup();

    // ATTRIBUTE: my_is_complete
    // DESCRIPTION:
    // Indicate that all of the fixtures have been added to the staff, no more
    // should be added.
    bool my_is_complete;

    // ATTRIBUTE: my_offset
    // DESCRIPTION:
    // the offset where this staff is to be drawn.
    int my_y_offset;

    // ATTRIBUTE: my_start_measure
    // DESCRIPTION:
    // the measure number I start on.
    int my_start_measure;

    // ATTRIBUTE: my_end_measure
    // DESCRIPTION:
    // my last measure
    int my_end_measure;

    // ATTRIBUTE: my_should_center
    // DESCRIPTION:
    // keep track of whether or not we should try and justify
    bool my_should_center;

    // ATTRIBUTE: page my_window
    // DESCRIPTION:
    // where it all happens...
    window& my_window;
    
    // ATTRIBUTE: my_show_name
    // DESCRIPTION:
    // true if we need to show the name on top of the staff, i.e. we're in
    // score mode
    bool my_show_name;

    // ATTRIBUTE: map<staff_point,note_figure*> my_music
    // DESCRIPTION:
    // Map each point in the score with a figure to draw on the staff.
    // We keep 2 around, so we can re-render things fast if we already
    // know where stuff goes.  We also store the x location of the words since
    // we compute that at the same time as the note locations
    typedef struct point_note_pair
    {
        point the_note_point;
        int the_word_x;
        note_figure* the_figure;
    };
    map<staff_point,point_note_pair> my_music;

    map<staff_point,text_figure::point_text_array> my_words;
    
    // ATTRIBUTE: map<int,points>
    // DESCRIPTION:
    // Keep track of the places where the barlines go so we don't have to
    // keep recomputing those.
    map<int,int> my_bar_points;

    // ATTRIBUTE: fraction my_time_sig
    // DESCRIPTION:
    // Keep track of the time signature.
    map<int,time_sig*> my_time_sigs;

    // ATTRIBUTE: key_sig* my_key_sig
    // DESCRIPTION:
    // A key signature can draw itself on my.
    map<int,key_sig*> my_key_sigs;

    // ATTRIBUTE: point my_origin
    // DESCRIPTION:
    // We keep track of the UL part of ourselves, and then we increment the x part
    // as we render the staf from left to right.
    point my_origin;

    // ATTRIBUTE: point my_true_origin
    // DESCRIPTION:
    // This is the original origin.
    point my_true_origin;

    // ATTRIBUTE: array my_chord_symbols
    // DESCRIPTION:
    // We compute the x location of the chord symbols and the y location at 
    // different times, to avoid chords and beams colliding.  Save the figures
    // in an array and then render them all after the notes have been rendered.
    class note_text_point
    {
    public:
        note_figure* my_note_figure;
        text_figure* my_text_figure;
        point        my_point;
    };
    array<note_text_point> my_chord_symbols;

    // ATTRIBUTE: pitch my_center_pitch
    // DESCRIPTION:
    // Keep track of which is the center pitch for this cleff.  That is used to determmine
    // things like beam directions, etc.
    pitch my_center_pitch;
    
    // ATTRIBUTE: int my_pixels_for_notes
    // DESCRIPTION:
    // Keep track of how many pixels are left for notes, after drawing the key sig
    // and stuff.  This helps us justify correctly.
    int my_pixels_for_notes;

    // ATTRIBUTE: my_rhs_overlap
    // DESCRIPTION:
    // Keep track of how many pixels over the right edge we'll be going,
    // so we can 'borrow' from wider notes on a later pass.
    int my_rhs_overlap;

    // ATTRIBUTE: my_leftover_pixels
    // DESCRIPTION:
    // Keep track of how many 'spare' pixels we have for each note so we
    // can 'borrow' if there is overlap.
    int my_spare_pixels;
    
    // ATTRIBUTE: beamer* my_last_beamer
    // DESCRIPTION:
    // Keep track of the last beamer so that we can add notes to the current
    // beam group as they come in from the parser.
    beamer* my_last_beamer;
    
    // ATTRIBUTE: beamer* my_last_grace_beamer
    // DESCRIPTION:
    // We beam grace notes seperately, so keep track of those too.
    beamer* my_last_grace_beamer;

    // ATTRIBUTE: slurer* my_slurer
    // DESCRIPTION:
    // There is one slur object per staff that contains all slurs on that staff.
    // This should allow us to do complex things with slurs like slur around
    // notes.  For now we just try to draw it as best we can.
    slurer* my_slurer;
    
    // ATTRIBUTE: double my_scale_factor
    // DESCRIPTION: 
    // a constant scaling the drawing routines so we can fit more/less stuff on
    // the screen.
    scale my_scale_factor;
    scale my_note_scale;

    // ATTRIBUTE: my_rect_map
    // DESCRIPTION:
    // When we add figures to the staff, we want to keep them from bumping
    // into the important things like notes.  So make an array of all the rectangles
    // of the figures that we have drawn.
    typedef array<rect> rectangle_array;
    map<int,rectangle_array> my_rect_map;
    void insert_rect_into_map(const rect& the_rect);

    // ATTRIBUTE: double my_page_width
    // DESCRIPTION:
    // Page width in inches
    double my_width;

    // ATTRIBUTE:  A whole bunch of figures that I know how to draw.
    map<int,figure*> my_barlines;

    // ATTRIBUTE: my_begin_barlines
    // DESCRIPTION:
    // If a measure begins with a repeat, and its the first in a line,
    // this is what we draw.
    figure* my_begin_barline;

    // CLASS: nth_ending_info
    // DESCRIPTION:
    // This is all the stuff that is required to draw an nth ending.
    // We populate the number ending and begin/end type when we 
    // add the measure to the staff, and the other stuff gets
    // populated after we have rendered all the measures.
    class nth_ending_info
    {
    public:
        int my_number_ending;
        int my_start_measure;
        int my_end_measure;
        point my_text_point;
        text_figure* my_number_figure;
        point my_bracket_point;
        figure* my_bracket_figure;
    };
    map<int,nth_ending_info> my_nth_ending_map;

    // METHOD: render_nth_endings
    // DESCRIPTION:
    // Make the figures that constitue nth endings.
    void render_nth_endings();

    // ATTRIBUTE: dynamic_marking
    // DESCRIPTION:
    // Keep track of dynamics that we need to spell out,
    // and crescendos.
    class dynamic_marking
    {
    public:
        chord_info::dynamic the_start_dynamic;
        chord_info::dynamic the_stop_dynamic;
        staff_point the_start_point;
        staff_point the_stop_point;
    };

    map<staff_point,dynamic_marking> my_dynamics_info;
    map<staff_point,chord_info::dynamic> my_dynamic_changes;

    // We keep track of this so that the application can form
    // a hashtable that matches the parser's hashtable.
    score_point my_lowest_score_point;
    int my_voice_index;
    bool my_first_notes_added;

    figure* my_clef;
    int my_clef_x;
    figure* my_staff;
    sizeloc my_staff_size;

    fraction my_beats;
    atomic_counter my_count;

    // ATTRIBUTE:  my_has_rendered
    // DESCRIPTION:
    // Return true, this will let a client know that no more music should
    // go on this page.
    bool my_has_rendered;

    // ATTRIBUTE: my_draw_part_first
    // DESCRIPTION:
    // If the first measure is the first measure in a new part,
    // we need to be told that so that we can draw it.
    bool my_draw_part_first;

    // ATTRIBUTE:  my_voice_info_map
    // DESCRIPTION:
    // General stuff about the current key, staff, etc.  Mapped per
    // measure.
    map<int,voice_info> my_voice_info_map;

    // ATTRIBUTE: my_hi/lo point
    // DESCRIPTION:
    // Keep track of the high and low point so we can adjust our size
    // for vertical justification.
    int my_high_point;
    int my_low_point;
    void update_high_low(int the_possible_high,int the_possible_low);

    // ATTRIBUTE: my_accidentals
    // DESCRIPTION:
    // keep track of a running total of the accidentals
    array<pitch::accidental_map> my_accidentals;

    // ATTRIBUTE: my_accidentals
    // DESCRIPTION:
    // Keep track of which accidentals are being used in which measure
    static int static_staff_count;
};

}
#endif

