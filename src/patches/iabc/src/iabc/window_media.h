/*
 * window_media.h - A piece of music that can be displayed in a window.
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
#ifndef iabc_window_media_h
#define iabc_window_media_h

#include "iabc/media.h"
#include "iabc/winres.h"
#include "iabc/key_sig.h"
#include "iabc/line_of_music.h"
#include "iabc/win_page.h"
#include "iabc/map.h"

namespace iabc
{
typedef map<score_point,line_of_music*> staff_map;
typedef map<int,line_of_music*> staff_page_map;

// CLASS: music_window_iterator
// DESCRIPTION:
// Iterate through the music and populate the graphical interface.  We use
// a template because the iteration actually uses different clients depending
// on whether we're in score mode or not, but everything else is the same.
template<typename score_iterator,typename parent_class>
class music_window_iterator
{
public:
    music_window_iterator(parent_class& the_parent,
                          paged_window& the_pages,
                          int& the_current_page);
    void render_music(score_iterator& the_music);
private:
    parent_class* my_parent;
    int* my_current_page;
    paged_window* my_pages;
};

template<typename score_iterator,typename parent_class>
int
execute_music_iterator(score_iterator& the_iterator,parent_class the_parent);

// CLASS:
// window_media
// DERIVES FROM:
// media, document
// DESCRIPTION:
// There will be other media types, such as: midi device output,
// printer output, midi file output,
// but for now the only one is window output.  As other media types are
// added they will be moved into their own files.  This accepts musical
// features from a media source and renders them on some type of window
// device.
class window_media:public media
{
public:
    window_media(paged_window& the_pages,
                 window& the_window,
                 const scale& the_page_size,
                 const scale& the_scale,
                 voice_map_source* the_voice_map_source = 0);
    virtual ~window_media();

    friend class music_window_iterator<map<score_point,music_info >::iterator,
        window_media >;

    friend class music_window_iterator<map<score_time,music_info >::iterator,
        window_media >;

    // METHOD: setup
    // DESCRIPTION:
    // This is supposed to be where the media gets set up to receive
    // the music, but it doesn't really apply to a window getting
    // music.
    // This didn't really work....
    virtual bool setup(){return true;};

    // METHOD: complete
    // DESCRIPTION:
    // This is called when all the music has been added to the media.
    // It's a sign for the media to start drawing itself.  Here we just
    // render the music one page at a time.
    virtual void complete();

    map<int,score_point> my_saved_measure_points;
    map<int,score_point> my_saved_chord_points;

    map<score_point,int> get_score_points_to_lines()
    {
        return my_score_points_to_lines;
    };
    map<int,score_point> get_lines_to_score_points()
    {
        return my_lines_to_score_points;
    };
protected:
    class dynamic_on_beat
    {
    public:
        score_point the_beat;
        chord_info::dynamic the_dynamic;
    };
    map<int,dynamic_on_beat> my_last_crescendo_for_voice;

    // METHOD:  consolidate_long_rests
    // DESCRIPTION:
    // If we are in part mode, consolitdate all the rests into blocks where possible.
    void consolidate_long_rests();

    // METHOD: sort_staffs_in_y_order
    // DESCRIPTION:
    // Sort the staffs in order from lowest to highest on the
    // screen
    void sort_staffs_in_y_order(map<int,line_of_music*>&);

    // METHOD: adjust_staff_height
    // DESCRIPTION:
    // Move all the staffs so that they don't overlap one another.
    void adjust_staff_height(map<int,line_of_music*>,map<int,line_of_music*>&);

    // METHOD: add_margins_stuff
    // DESCRIPTION:
    // Add the header and footer to the given page.
    void add_margins_stuff(int the_page);

    // METHOD: render_music_feature
    // ARGS:  music_feature* - the feature to render
    //        const score_point& - the place to render it
    // DESCRIPTION:
    // Make the musical feature come alive on the window.  Usually this creates a note
    // figure and puts it on the correct line_of_music.
    virtual void render_music_feature(const chord_info& the_feature,const score_point& the_part);

    // METHOD: render_measure_feature
    // ARGS:  measure_feature* - the feature of the measure to render
    //        const score_point& - the place to render it
    // DESCRIPTION:
    // render a feature that revolves around a measure, such as a bar line or repeat sign.
    virtual void render_measure_feature(const measure_feature& the_feature,const score_point& the_part);

private:
    // METHOD: transpose_chord
    // DESCRIPTION:
    // Transpose the note up or down the given number of half-steps
    chord_info transpose_chord(const chord_info& the_info,int the_steps,const pitch& the_key);

    // METHOD: transpose_key
    // DESCRIPTION:
    // Transpose the note up or down the given number of half-steps
    pitch transpose_key(const pitch& the_pitch,int the_steps);

    // METHOD: render_music
    // DESCRIPTION:
    // We've read in the music, and stored it, so now its time to render the music.
    // RETURNS:
    // the number of the next page; 0 if this is the last page.
    void render_music();

    // METHOD: render_words
    // DESCRIPTION:
    // Handle the big 'W'.  Just draw the words centered in the window at the
    // end of the song part.
    void render_words();

    // METHOD: render_all_staffs
    // DESCRIPTION:
    // We've put together a bunch of staffs and added the music to them, now
    // go and do the drawing of them all at once.
    void render_all_staffs();

    // RETURNS:
    // the number of the next page; 0 if this is the last page.
    // METHOD: set_dynamics
    // DESCRIPTION:
    // Set the dynamics in the staff, if they've changed.
    void set_dynamics(const chord_info& the_chord,
                               const score_point& the_current_beat);

    // METHOD: add_figures_to_page
    // DESCRIPTION:
    // Add the staffs to the pages, compensating for the offset and
    // adding new pages when necessary.
    void add_figures_to_page();

    // METHOD: get_staves_per_system
    // DESCRIPTION:
    // If we are in score mode, return the number of voices.
    // Otherwise, return 1.
    int get_staves_per_system();

    // METHOD: add_music_feature_to_staff
    // DESCRIPTION:
    // Create a note figure from a note info and add it to the staff.
    void create_note_figures(const chord_info& the_feature,const score_point& the_part);


    // METHOD: add_measure
    // ARGS: const measure_feature& the_feature
    //       score_point&  the place to add the measure
    // DESCRIPTION:
    // We have rendered all the notes in the measure, now complete the
    // measure by adding the figure that goes at the end, and the figure
    // that starts the next measure (e.g. a begin repeat) to the appropriate
    // staff.
    void add_measure(const measure_feature& the_feature,
                     const score_point& the_point);

    // METHOD: get_or_create_line_of_music
    // ARGS: score_point& - the place in the music that maps to this staff
    // DESCRIPTION:
    // We keep a map of places in the music to the staff objects that go with it.  Look up
    // the staff in the map that is less than or = to this point in the score.  If this is
    // the first such request there are no staffs for this voice,
    // create the initial staff and return it.
    line_of_music* get_or_create_line_of_music(const score_point& the_point);

    // METHOD: get_previous_staff
    // DESCRIPTION:
    // To keep the staffs from running into one another, compare the points
    // to the previous staff.
    line_of_music* get_previous_staff(const line_of_music& the_staff);

    // METHOD: add_nth_endings
    // DESCRIPTION:
    // Figure out which nth endings go on the staff.
    void add_nth_endings(line_of_music& the_staff);

    void add_ref(){};
    void remove_ref(){};

    // METHOD: get_points
    // DESCRIPTION:
    // We have changed parts so we weren't reading the music in in order.
    // Get the points we saved the last time, and save the current point
    // of measure and chord for when we change back to this part/voice.
    void get_count_by_voice(int the_part,score_point& the_measure_point,score_point& the_chord_point);
    void save_count_by_voice(const score_point& the_measure_point,const score_point& the_chord_point);

    // ATTRIBUTE:
    // scale my_page_size
    // scale my_page_scale
    // DESCRIPTION:
    // x and y scale and page size
    scale my_page_size;
    scale my_scale;
    
    // ATTRIBUTE: my_staff_names[]
    // DESCRIPTION:
    // an array names that contain the voice header name information
    // (e.g. trumpet2).  There may be one of these for each page.
    array<string> my_staff_names;

    // METHOD: get_vertical_offset
    // ARGS: line_of_music* - the staff to get the offset of.
    // DESCRIPTION:
    // We have a line_of_music.  We want to see what the offset is info this window so that we
    // can put it in the right place.
    int get_vertical_offset(const score_point&,line_of_music* the_staff);

    // METHOD: create_new_staff
    // DESCRIPTION:
    // Handle the work of creating a new staff instance given a
    // point of music.  We assume that the client wants to actually create the
    // line_of_music if they call this function; no check is made to see if it
    // already exists.
    line_of_music* create_new_staff(const score_point& the_point);

    // METHOD: in_score_mode
    // DESCRIPTION:
    // Return true if we are in score mode.
    bool in_score_mode() const {return (my_score_map != 0);};

    bool should_draw_part(const score_point& the_point);

    //
    // ATTRIBUTE: window* my_window
    // window* my_window
    // DESCRIPTION:
    // This is where we do our thing...
    window* my_window;

    // ATTRIBUTE: rect my_visible_rect
    // DESCRIPTION:
    // Used to determine whether or not a thing is in the window range
    rect my_visible_rect;

    // ATTRIBUTE:
    // staff_map my_staves
    // DESCRIPTION:
    // A map of staffs to parts/measures
    staff_map my_staves;

    // ATTRIBUTE:
    // staff_page_map my_staves_y
    // DESCRIPTION:
    // Keep track of the staffs as we draw them so we can
    // space them properly
    staff_page_map my_staves_y;

    // ATTRIBUTE: my_staffs_to_render
    // DESCRIPTION:
    // Keep track of the staffs and then render them all at once.
    array<line_of_music*> my_staffs_to_render;

    // ATTRIBUTE: my_staffs_to_render
    // DESCRIPTION:
    // Keep track of the staffs that we have started so far.
    array<line_of_music*> my_started_staffs;

    // Keep a map of
    // These keep us safe from danger.
    atomic_counter my_count;

    // ATTRIBUTE: my_pages
    // DESCRIPTION:
    // These keep track of the pages.
    paged_window* my_pages;
    int my_current_page;

    // ATTRIBUTE: my_last_rendered_score_point
    // DESCRIPTION:
    // The last point I rendered, used to update the system a little at a time.
    point my_last_rendered_point;

    // ATTRIBUTE: my_music_map
    // DESCRIPTION:
    // Keep track of all the music in one map so we don't have to
    // keep switching maps.
    map<score_point,music_info>* my_music_map;

    // ATTRIBUTE: my_score_map
    // DESCRIPTION:
    // This is the map we use if we are displaying in score mode.
    map<score_time,music_info>* my_score_map;

    // ATTRIBUTE: my_lines_to_score_point
    // DESCRIPTION:
    // Keep track of which screen lines the staff is on so we
    // can do GUI magic later.
    map<score_point,int> my_score_points_to_lines;
    map<int,score_point> my_lines_to_score_points;

    chord_info::dynamic my_last_dynamic;

    window_media(const window_media& o);
    window_media& operator=(const window_media& o);
    static int class_instance_count;
};

typedef draw_command<window_media,bool*> draw_media_command;

}

#endif

