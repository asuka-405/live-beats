#include "iabc/registry_defaults.h"

using namespace iabc;

namespace reg
{
;

registry_default_value page_break_between_parts = {"score_mode","page_break_between_parts","","false"};

registry_default_value page_height_value ={"page_dimensions",  "height","","11.0"};
registry_default_value page_width_value  ={"page_dimensions",  "width", "","8.5"};
registry_default_value xscale_value      ={"page_dimensions",  "xscale","","1.0"};
registry_default_value yscale_value      ={"page_dimensions",  "yscale","","1.0"};
registry_default_value frame_width =      {"screen_dimensions","frame_width","","800"};
registry_default_value frame_height =     {"screen_dimensions","frame_height","","600"};
registry_default_value last_file    =     {"last_file","filename","",""};
registry_default_value last_tune =     {"last_file","tune_number","","1"};
registry_default_value v_no_bracket_default = {"abc","v_no_bracket_allowed","","false"};
registry_default_value allow_text_voice_change = {"abc","allow_text_voice_change","","false"};
registry_default_value allow_barfly_midi_commands = {"abc","allow_barfly_midi_commands","","false"};
registry_default_value midi_output_directory = {"midi","midi_directory","",""};
registry_default_value midi_player_name = {"midi","midi_player","",""};
registry_default_value midi_player_command = {"midi","midi_player_command","","%p %f"};
registry_default_value window_media_top_margin = {"page_dimensions", "top_margin","","1.5"};
registry_default_value window_media_bottom_margin = {"page_dimensions", "bottom_margin","","1.25"};
registry_default_value window_media_left_margin = {"page_dimensions", "left_margin","","0.7"};
registry_default_value window_media_right_margin = {"page_dimensions", "right_margin","","0.5"};
registry_default_value window_media_note_scale = {"page_dimensions", "note_scale","","1.0"};
registry_default_value window_media_copyright_size = {"page_dimensions", "copyright_size","","10"};
registry_default_value window_media_title_size = {"page_dimensions", "title_size","","14"};
registry_default_value window_media_chord_size = {"page_dimensions", "chord_size","","14"};
registry_default_value window_media_chord_bold = {"page_dimensions", "chord_bold","","true"};
registry_default_value window_media_chord_italic = {"page_dimensions", "chord_italic","","true"};
registry_default_value window_media_word_size = {"page_dimensions", "word_size","","8"};;
registry_default_value window_media_songword_size = {"page_dimensions", "songword_size","","12"};;
registry_default_value window_media_songword_italic = {"page_dimensions", "songword_italic","","true"};;
registry_default_value transpose_diatonic = {"transpose","diatonic","","false"};
registry_default_value transpose_refresh = {"transpose","refresh","","true"};
registry_default_value transpose_steps = {"transpose","steps","","2"};

registry_default_value xsash_default = {"layout","xsash","","0.6666"};
registry_default_value ysash_default = {"layout","ysash","","0.6666"};
registry_default_value show_tune_list = {"layout","show_tune_list","","true"};
registry_default_value score_mode_on = {"layout","score_mode","","false"};
registry_default_value text_buffer_font_name = {"layout","text_buffer_font_name","",""};
registry_default_value text_buffer_font_size = {"layout","text_buffer_font_size","","11"};

iabc::registry_key
registry_default_value::as_key()
{
	return iabc::registry_key((iabc::string)category,(iabc::string)key,(iabc::string)subkey);
}

iabc::registry_entry
registry_default_value::as_entry()
{
	return iabc::registry_entry(as_key(),(iabc::string)value);
}

iabc::registry_entry
get_default(registry_default_value& the_value)
{
    iabc::registry_key tmp_key(the_value.category,
		the_value.key);
    iabc::registry_entry tmp_entry = (globalRegistry).get_entry(tmp_key);
    if (tmp_entry.get_value().length() == 0)
    {
        tmp_entry = iabc::registry_entry(tmp_key,the_value.value,true);
        (globalRegistry).set_value(tmp_entry);
    }
    
    return tmp_entry;
}

iabc::registry_key
get_key_from_default(const registry_default_value& the_value)
{
	iabc::registry_key tmp_key(the_value.category,the_value.key);
	tmp_key.subkey = the_value.subkey;
	return tmp_key;
}

void
set_new_default_value(const registry_default_value& the_key,const string& tmp_new_value)
{
    iabc::registry_key tmp_key = get_key_from_default(the_key);
    iabc::registry_entry tmp_entry(tmp_key,tmp_new_value,true);
    iabc::registry::get_instance().set_value(tmp_entry);
    iabc::registry::get_instance().flush();
}
}

