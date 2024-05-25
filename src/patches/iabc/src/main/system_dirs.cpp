#include "system_dirs.h"
#include <wx/wx.h>
#include <wx/dir.h>
#include "iabc/event_log.h"
#include "iabc/iabc_preferences.h"
#include "iabc/registry_defaults.h"

namespace iabc
{
;
string* home_dir_ptr = 0;
string* docs_dir_ptr = 0;
string* temp_dir_ptr = 0;
string* program_dir_ptr = 0;
string* new_file_ptr = 0;
void set_system_dirs(wxChar** argv,int argc)
{
    program_dir_ptr = new string;
    docs_dir_ptr = new string;
    temp_dir_ptr = new string;
    home_dir_ptr = new string;
    new_file_ptr = new string;

    program_dir = wxGetCwd().mb_str();
    wxSetWorkingDirectory((wxChar*)"..");
#ifdef WIN32
    home_dir = temp_dir = docs_dir = wxGetCwd().mb_str();
    docs_dir += (string) "\\doc";
    new_file = temp_dir + (string)"\\new.abc";
#else
    home_dir = getenv("HOME");
    temp_dir = "/tmp";
    docs_dir = "/usr/local/doc/iabc";
    new_file = temp_dir + (string)"/new.abc";
#endif

    // Try to read the midi temp directory from the registry, if its not there
    // fall back to the temp directory.
    midi_output_directory = reg::get_default(reg::midi_output_directory).get_value();
    if (midi_output_directory.length() == 0)
    {
        midi_output_directory = temp_dir;
        reg::set_new_default_value(reg::midi_output_directory,midi_output_directory);
    }

    global_settings_event_log.log_event(program_dir.access_char_array(),
                                        program_dir.length(),
                                        settings_event_data::set_program_dir_event);

    global_settings_event_log.log_event(temp_dir.access_char_array(),
                                        temp_dir.length(),
                                        settings_event_data::set_temp_dir_event);

    global_settings_event_log.log_event(docs_dir.access_char_array(),
                                        docs_dir.length(),
                                        settings_event_data::set_docs_dir_event);

    global_settings_event_log.log_event(home_dir.access_char_array(),
                                        home_dir.length(),
                                        settings_event_data::set_home_dir_event);

    global_settings_event_log.log_event(new_file.access_char_array(),
                                        new_file.length(),
                                        settings_event_data::set_new_file);



}

}
