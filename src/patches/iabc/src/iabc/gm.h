#ifndef iabc_gm
#define iabc_gm
#include "iabc/winres.h"
#include "iabc/note_info.h"
#include "iabc/staff_info.h"
#include "iabc/figure.h"
#include "iabc/window_figure.h"

namespace iabc
{
;
// FILE: gm.h
// DESCRIPTION:
// Geometry Management. Stand-alone functions that compute coordinates of 
// things in relation to a staff, but didn't belong any place else.
// Basically these functions will compute the correct spot to place a note
// given some basic information about the note and the staff.

// FUNCTION: place_note
// ARGS: window& - the window
//       note_info& - the pitch and duration of the note to be placed.
//       staff_info - information about the geometry of the staff itself
//       figure& - the figure to be placed.
// DESCRIPTION:
// Compute the coordinate to place the figure, if you want it to represent
// the_note place on the_staff.
extern point place_note(window& w,
                            const note_info& the_note,
                            staff_info the_staff);


// FUNCTION: note_height
// DESCRIPTION:
// Find the height of the note, if the staff height is the_staff_heigh and the
// center of the staff is center_line.  Value is offset from the center of 
// the staff.
extern int
note_height(const pitch& the_pitch,int the_staff_height,const pitch& center_line);

// FUNCTION: get_head_vertical
// DESCRIPTION:
// Get the vertical placement of the given pitch, if the given staff is
// the_staff_height.
extern int get_head_vertical(int the_staff_height,
                          const pitch& the_pitch,
                          const pitch& the_middle_pitch
                          );



}

#endif


