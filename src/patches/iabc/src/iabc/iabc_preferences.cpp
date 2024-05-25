#include "iabc/iabc_preferences.h"

namespace iabc
{
;
// score vs. part mode options.
bool score_mode_on = false;
bool page_break_between_parts = false;

// Handle iabc display preferences
double window_media_default_width = 8.5;
double window_media_default_height = 11.0;
double window_media_default_xscale = 1.0;
double window_media_default_yscale = 1.0;
double window_media_top_margin = 1.5;
double window_media_bottom_margin = 1.25;
double window_media_left_margin = 0.7;
double window_media_right_margin = 0.5;
int window_media_title_size = 14;
int window_media_copyright_size = 12;
int window_media_chord_size = 14;
bool window_media_chord_bold = true;
bool window_media_chord_italic = true;
int window_media_word_size = 8;
int window_media_songword_size = 10;
bool window_media_songword_italic = true;
double window_media_note_scale = 1.0;

string midi_output_directory;
string midi_player_name;
string midi_player_command;

// Options selectable from the GUI
bool allow_deprecated_voice_change = false;
bool allow_text_voice_change = false;
bool allow_barfly_midi_commands = false;
bool allow_local_header_values = false;

int transpose_steps = 0;
bool  transpose_diatonic = false;
bool  transpose_refresh = false;
}


