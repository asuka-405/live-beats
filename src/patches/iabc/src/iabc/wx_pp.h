/*
 * wx_pp.h - wxWindows helper functions for dialog boxes (property pages).
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
#ifndef iabc_pp_h
#define iabc_pp_h

#include "iabc/string.h"
#include "iabc/list.h"
#include "iabc/mutex.h"
#include "iabc/drawtemp.h"
#include "iabc/registry.h"
#include "iabc/registry_defaults.h"
#include <wx/wx.h>
#include "wx/spinctrl.h"

// FILE: pp.h
// DESCRIPTION:
// Stuff to handle property pages.  A property page
// is just a way to get preferences from the user.

namespace iabc
{
;

// Some functions that we use to convert to/from
// registry or dialog box values to the global
// that determine user preferences.
extern void db_to_global(const wxCheckBox*,bool& the_output);
extern void db_to_global(const wxSpinCtrl*,int& the_output);
extern void db_to_global(const wxTextCtrl*,double& the_output);
extern void db_to_global(const wxTextCtrl* the_input,string& the_output);
extern void db_to_global(const wxTextCtrl* the_input,int& the_output);
extern void global_to_db(const bool& the_input ,wxCheckBox* the_field);
extern void global_to_db(const double& the_input,wxTextCtrl* the_field);
extern void global_to_db(const int& the_input,wxSpinCtrl* the_field);
extern void global_to_db(const string& the_input,wxTextCtrl* the_field);
extern void reg_to_global(const string& the_input,bool& the_output);
extern void reg_to_global(const string& the_input,double& the_output);
extern void reg_to_global(const string& the_input,string& the_output);
extern void reg_to_global(const string& the_input,int& the_output);
extern void global_to_reg(const bool& the_input,string& the_output);
extern void global_to_reg(const double& the_input,string& the_output);
extern void global_to_reg(const string& the_input,string& the_output);
extern void global_to_reg(const int& the_input,string& the_output);

class threadsafe_db_shower
{
public:
    typedef enum operation
    {
        op_show,op_hide
    } operation;
    friend class draw_command<threadsafe_db_shower,operation>;
    threadsafe_db_shower(wxDialog* the_dialog,bool the_block = true):
        my_dialog(the_dialog),
        my_blocking(the_block){};
    void show();
    void hide();
    void add_ref(){};
    void remove_ref(){};
private:
    void draw_self(window& the_window,threadsafe_db_shower::operation the_operation);
    wxDialog* my_dialog;
    bool my_blocking;
};

typedef draw_command<threadsafe_db_shower,threadsafe_db_shower::operation> draw_dialog_command;

}

#endif


