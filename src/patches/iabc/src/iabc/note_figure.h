/*
 * note_figure.h - Draws a note with stems and stuff.
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
#ifndef iabc_note_h
#define iabc_note_h
#include "iabc/music_info.h"
#include "iabc/figure.h"
#include "iabc/pitch.h"
#include "iabc/note_info.h"
#include "iabc/staff_info.h"
#include "iabc/window_figure.h"
#include "iabc/gm.h"
#include "iabc/beamer.h"
#include "iabc/slurer.h"
#include "iabc/text_figure.h"
#include "iabc/win_page.h"
#include "iabc/media.h"

namespace iabc
{
;
// CLASS: note_figure
// DESCRIPTION:
// Draws the actual note part.  Also updates beam group.
class note_figure
{
public:
    // METHOD: get_width;
    // DESCRIPTION:
    // Return the width of the rendered figure so that the staff can do
    // justification.
    int get_width();

    // METHOD: get_head_offset
    // DESCRIPTION:
    // Return the offset of the head, which isn't always in
    // the left hand side.
    int get_head_offset(){return my_head_offset;};

    // METHOD: ctor
    // DESCRIPTION:
    // none.
    note_figure(window& w,
                const chord_info& the_info,
                const pitch& the_key);
    // METHOD: dtor
    // DESCRIPTION:
    // Dereference all my stuff.
    ~note_figure();

    // METHOD: render
    // DESCRIPTION:
    // Actually draw self in correct thread.  Honor the_should_block,
    // which should be set the first time so drawing occurs properly.
    // Also activate beams and slurs and stuff.
    void render(figure_container_if& the_page,staff_info the_staff);

    // DESCRIPTION:
    // Allow the beamer to get set, so I can beam self when drawing self.
    void set_beamer(beamer* the_beamer);

    // DESCRIPTION:
    // Accessor functions.
    point get_origin() const{return my_origin;};
    fraction get_duration(){return my_note.get_duration();};
    chord_info get_note(){return my_note;};

    // METHOD: more accessors
    // DESCRIPTION:
    // We need to keep track of the  absolute bounding box once we've
    // rendered these so that we can draw other figures right next to
    // them.
    int get_high_point() const {return my_high_point;};
    int get_low_point() const {return my_low_point;};
    int get_absolute_left() const {return my_absolute_left;};
    int get_absolute_right() const {return my_absolute_right;};
    // Keep track of slurers.
    void add_to_slurer(slurer* the_slurer);
    void add_from_slurer(slurer* the_slurer);

    // METHOD: rescale
    // DESCRIPTION:
    // We are now on a different scale so all of our beamers and figures
    // are no good.  So get new ones.
    void rescale(const scale& the_scale);

    // METHOD: handle_ornaments
    // DESCRIPTION:
    // Handle any ornamentation on the note, like tenuto
    // or slurs.
    void handle_ornaments(figure_container_if& the_page,const staff_info& the_info);

    void set_accidentals(pitch::accidental_map& the_accidentals);
protected:
    // DESCRIPTION:
    // Figure out what type of note head to draw and get that from the factory.
    void set_head(const size& the_ppi);

    // DESCRIPTION:
    // Keep track of the high and low point of this note and beam group.
    void update_high_low(int the_possible_low,int the_possible_high);
    void update_left_right(figure* the_figure,const point& the_point);

    // DESCRIPTION:
    // Figure out what type of accidental to draw, and get that from the factory.
    void draw_accidentals(figure_container_if& the_page,staff_info the_staff);

    // figure out how many dots to put on the head
    void dot_notes(figure_container_if&,const point& my_point);
    void set_dot(const size& the_ppi);

    // METHOD: set_rest
    // DESCRIPTION:
    // Figure out which rest figure we need and get it from the factory.
    void set_rest(const fraction& the_duration,const size& the_ppi);

    // METHOD: get_num_dots
    // DESCRIPTION:
    // Reverse-engineer the number of dots on the note from its duration.
    int get_num_dots(const note_info& the_note) const;

    // METHOD:
    // DESCRIPTION:
    // Update the slurer for this channel with the location of the note.
    void handle_slurers(figure_container_if& the_page,const staff_info& the_staff);

    // ATTRIBUTE: my_head, etc.
    // DESCRIPTION:
    // A whole bunch of figures for rendering beams, etc.
    figure* my_head;
    figure* my_dot;
    figure::point_figure_pair my_ornaments;
    text_figure::point_text_pair my_ornament_text;
    text_figure* my_rest_count;
    figure* my_rest;
    text_figure* my_rest_number;
    beamer* my_beamer;
    slurer* my_to_slurer;
    slurer* my_from_slurer;

    // ATTRIBUTE: my_scale
    // DESCRIPTION:
    // We use this to determine what the scaling factor of all those figures is.

    scale my_scale;
    // DESCRIPTION:
    // The page on which we draw ourselves.
    window& my_window;

    // ATTRIBUTE: my_key
    // DESCRIPTION:
    // This tells us whether or not we need to draw accidentals.
    pitch my_key;

    // ATTRIBUTE: my_note
    // DESCRIPTION:
    // This is the chord that we are representing.
    chord_info my_note;

    // ATTRIBUTE: my_origin
    // DESCRIPTION:
    // Where we park.
    point my_origin;

    // ATTRIBUTE: my_accidental_width
    // DESCRIPTION:
    // We keep track of this so we can tell how big the whole
    // figure is.  This is used when justifying.
    int my_accidental_width;
    int my_head_offset;
    int my_high_point;
    int my_low_point;
    int my_absolute_right;
    int my_absolute_left;

    // ATTRIBUTE: need_to_beam
    // DESCRIPTION:
    // Keep track of whether or not we have drawn the beam and stuff.
    bool need_to_beam;
    bool need_to_slur;

    // ATTRIBUTE:
    // DESCRIPTION:
    // The array of accidentaly, maybe one for each note
    array<figure*> my_accidentals;

private:
    // Can't assigne these! too much stuff to keep track of.
    note_figure(const note_figure& o);
    note_figure& operator=(const note_figure& o);
};

}
#endif



