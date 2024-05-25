#ifndef iabc_registry_defaults_h
#define iabc_registry_defaults_h

#include "iabc/registry.h"

namespace reg
{
;

// FILE: registry_defaults
// DESCRIPTION:
// Hard-code some values as defaults for things that the user might want to
// customize later.
class registry_default_value
{
public:
    const char* category;
    const char* key;
    const char* subkey;
    const char* value;
    iabc::registry_key as_key();
    iabc::registry_entry as_entry();
};

extern registry_default_value score_mode_on;
extern registry_default_value page_break_between_parts;

extern registry_default_value page_height_value; 
extern registry_default_value page_width_value;  
extern registry_default_value xscale_value;      
extern registry_default_value yscale_value;      
extern registry_default_value frame_width;
extern registry_default_value frame_height;
extern registry_default_value last_file;
extern registry_default_value last_tune;
extern registry_default_value v_no_bracket_default;
extern registry_default_value allow_text_voice_change;
extern registry_default_value allow_barfly_midi_commands;
extern registry_default_value midi_output_directory;
extern registry_default_value midi_player_name;
extern registry_default_value midi_player_command;
extern registry_default_value window_media_top_margin;
extern registry_default_value window_media_bottom_margin;
extern registry_default_value window_media_left_margin;
extern registry_default_value window_media_right_margin;
extern registry_default_value window_media_note_scale;
extern registry_default_value window_media_copyright_size;
extern registry_default_value window_media_title_size;
extern registry_default_value window_media_chord_size;
extern registry_default_value window_media_chord_bold;
extern registry_default_value window_media_chord_italic;
extern registry_default_value window_media_word_size;
extern registry_default_value window_media_songword_size;
extern registry_default_value window_media_songword_italic;
extern registry_default_value transpose_diatonic;
extern registry_default_value transpose_refresh;
extern registry_default_value transpose_steps;
extern registry_default_value xsash_default;
extern registry_default_value ysash_default;
extern registry_default_value show_tune_list;
extern registry_default_value text_buffer_font_name;
extern registry_default_value text_buffer_font_size;

// METHOD: get_default
// DESCRIPTION:
// Look in the global registry for the value.  If its not there use the hard-coded default 
// value.
extern iabc::registry_entry get_default(registry_default_value& the_value);

// METHOD: get_key_from_default
// DESCRIPTION:
// Convenience function to create a registry_key object from the strings in a 
// registry_default_value
extern iabc::registry_key get_key_from_default(const registry_default_value& the_value);

// METHOD: set_new_default_value
// DESCRIPTION:
// Replace the value in the global registry with the default value.
extern void set_new_default_value(const registry_default_value& the_key,const iabc::string& tmp_new_value);

}

#endif

