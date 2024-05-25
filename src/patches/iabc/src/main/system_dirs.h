#include <stdio.h>
#include "iabc/string.h"
#include <wx/string.h>

namespace iabc
{
;
void set_system_dirs(wxChar** argv,int argc);
extern string* home_dir_ptr;
extern string* docs_dir_ptr;
extern string* temp_dir_ptr;
extern string* program_dir_ptr;
extern string* new_file_ptr;
#define home_dir (*home_dir_ptr)
#define docs_dir (*docs_dir_ptr)
#define temp_dir (*temp_dir_ptr)
#define program_dir (*program_dir_ptr)
#define new_file (*new_file_ptr)

}
