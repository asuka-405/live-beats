/*
 * wx_pp.cpp - Property page helper functions.
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
#include "iabc/wx_pp.h"
#include "iabc/registry_defaults.h"
#include "iabc/drawtemp.cpp"
#include <wx/wx.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <iabc/unicode.h>

namespace iabc
{
;

// FUNCTION: db_to_global
// DESCRIPTION:
// helper functions to convert values from dialog boxes into
// globals of various types
void db_to_global(const wxCheckBox* the_input,bool& the_output)
{
    the_output = (*the_input).GetValue();
}

void db_to_global(const wxSpinCtrl* the_input,int& the_output)
{
    the_output = (*the_input).GetValue();
}

void db_to_global(const wxTextCtrl* the_input,double& the_output)
{
    wxString tmp_wx_string = (*the_input).GetValue();
    string tmp_string = (const char*)U2B(tmp_wx_string);
    the_output = tmp_string.as_double();
}

void db_to_global(const wxTextCtrl* the_input,string& the_output)
{
    wxString tmp_wx_string = the_input->GetValue();
    the_output = (const char*)(U2B(tmp_wx_string));
}

void db_to_global(const wxTextCtrl* the_input,int& the_output)
{
    wxString tmp_wx_string = the_input->GetValue();
    string tmp_string = (const char*)(U2B(tmp_wx_string));
    the_output = (int)tmp_string.as_long();
}

void reg_to_global(const string& the_input,bool& the_output)
{
    if (the_input == "true")
    {
        the_output = true;
    }
    else
    {
        the_output = false;
    }
}

void reg_to_global(const string& the_input,double& the_output)
{
    the_output = the_input.as_double();
}

void reg_to_global(const string& the_input,string& the_output)
{
    the_output = the_input;
}

void reg_to_global(const string& the_input,int& the_output)
{
    the_output = (int)the_input.as_long();
}

void global_to_reg(const bool& the_input,string& the_output)
{
    if (the_input == true)
    {
        the_output = "true";
    }
    else
    {
        the_output = "false";
    }
}

void global_to_reg(const double& the_input,string& the_output)
{
    the_output = string(the_input);
}

void global_to_reg(const string& the_input,string& the_output)
{
    the_output = the_input;
}

void global_to_reg(const int& the_input,string& the_output)
{
    the_output = string((long)(the_input));
}

void global_to_db(const bool& the_input,wxCheckBox* the_field)
{
    the_field->SetValue(the_input);
}

void global_to_db(const int& the_input,wxSpinCtrl* the_field)
{
    the_field->SetValue(the_input);
}

void global_to_db(const double& the_input,wxTextCtrl* the_field)
{
    string tmp_string(the_input);
    wxString tmp_wxstring = B2U(tmp_string.access_char_array());
    the_field->SetValue(tmp_wxstring);
}

void global_to_db(const string& the_input,wxTextCtrl* the_field)
{
    the_field->SetValue(B2U(the_input.access_char_array()));
}

void
threadsafe_db_shower::show()
{
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    draw_dialog_command::dispatch_draw_command(
        tmp_window,
        op_show,*this,true);
}

void
threadsafe_db_shower::hide()
{
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    draw_dialog_command::dispatch_draw_command(tmp_window,
        op_hide,*this,true);
}

void 
threadsafe_db_shower::draw_self(window& the_window,threadsafe_db_shower::operation the_operation)
{
    if (the_operation == op_show)
    {
        my_dialog->ShowModal();
    }
    else
    {
        // End modal probably takes care of the hide anyway.
        my_dialog->Hide();
    }
}

}
