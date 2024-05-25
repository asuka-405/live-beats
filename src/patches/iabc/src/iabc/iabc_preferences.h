#include "iabc/string.h"

namespace iabc
{
;

// Midi options
extern string midi_output_directory;
extern string midi_player_name;
extern string midi_player_command;

extern bool score_mode_on;
extern bool page_break_between_parts;

// Window look and feel options
extern double window_media_default_width;
extern double window_media_default_height;
extern double window_media_top_margin;
extern double window_media_bottom_margin;
extern double window_media_left_margin;
extern double window_media_right_margin;
extern double window_media_default_xscale;
extern double window_media_default_yscale;
extern double window_media_note_scale;
extern int window_media_copyright_size;
extern int window_media_title_size;
extern int window_media_chord_size;
extern bool window_media_chord_bold;
extern bool window_media_chord_italic;
extern int window_media_word_size;
extern int window_media_songword_size;
extern bool window_media_songword_italic;

// Options selectable from the GUI
extern bool allow_deprecated_voice_change;
extern bool allow_text_voice_change;
extern bool allow_barfly_midi_commands;
extern bool allow_local_header_values;

extern bool transpose_diatonic;
extern bool transpose_refresh;
extern int transpose_steps;
}


