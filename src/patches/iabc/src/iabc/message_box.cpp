#include "iabc/message_box.h"
#include <wx/choicdlg.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
#include "iabc/drawtemp.cpp"

wxFrame* theMessageFrame = 0;

extern int wxMessageBox(const wxString& message, const wxString& caption, long style,
                 wxWindow *parent, int WXUNUSED(x), int WXUNUSED(y) );
namespace iabc
{
;


void
message_box::display(const string& the_string)
{
  message_box tmp_box;
  tmp_box.private_display(the_string);
}
void
message_box::draw_self(window& w,const string& the_string)
{
    wxStatusBar* tmpBar = theMessageFrame->GetStatusBar();
    tmpBar->SetStatusText((wxString(the_string.access_char_array(),wxConvUTF8)),0);
    // ::wxMessageBox(the_string.access_char_array(),"Important Message",wxOK,0,0,0);
}

void
message_box::private_display(const string& the_string)
{
  window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));

  draw_text_command::dispatch_draw_command(tmp_window,
					   the_string,*this,true);
    
}
}
