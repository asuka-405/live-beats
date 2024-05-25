/*
 * wx_pp_temp.h - template and helper functions for easy dialog box creation.
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
#ifndef wx_property_page_h
#define wx_property_page_h
#include "wx/wx.h"
#include "iabc/wx_pp.h"
#include "iabc/registry_defaults.h"

namespace iabc
{
;

template <class db_field_type,class global_type>
class db_translator
{
public:
    db_translator(global_type& the_global,reg::registry_default_value the_reg_entry,
                  db_field_type the_field);

    void populate_global_from_reg();
    void populate_reg_from_global();
    void populate_db_from_global();
    void populate_global_from_db();
private:
    global_type& my_global;
    db_field_type my_field;
    reg::registry_default_value my_reg_entry;
};

template <class global_type,class field_type>
extern void populate_global_from_reg1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry);
template <class global_type,class field_type>
extern void populate_reg_from_global1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry);
template <class global_type,class field_type>
extern void populate_db_from_global1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry);
template <class global_type,class field_type>
extern void populate_global_from_db1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry);

#define ON_DB_CREATE(the_sizer,sizer_type,the_prompt,the_global,field_type,field_name,reg_entry) \
    {the_sizer = new sizer_type(this,the_prompt,"xzzy",field_name); \
    field_type* tmp_field = (field_type*)wxFindWindowByName(B2U(field_name),this); \
    my_sizer->Add(*the_sizer,1,wxGROW|wxALL,5); \
    iabc::populate_global_from_reg1(the_global,tmp_field,reg_entry);}

#define ON_DB_COMMIT(the_global,field_type,field_name,reg_entry) \
    {field_type* tmp_field = (field_type*)wxFindWindowByName(B2U(field_name),this); \
    iabc::populate_global_from_db1(the_global,tmp_field,reg_entry); \
    iabc::populate_reg_from_global1(the_global,tmp_field,reg_entry);}

#define ON_DB_SHOW(the_global,field_type,field_name,reg_entry) \
    {field_type* tmp_field = (field_type*)wxFindWindowByName(B2U(field_name),this); \
    iabc::populate_db_from_global1(the_global,tmp_field,reg_entry);}
    
#define ADD_BUTTONS() \
    {wxButton* tmpOK = new wxButton(this,ID_PPOK,_T("OK")); \
    wxButton* tmpCANCEL = new wxButton(this,ID_PPCANCEL,B2U("Cancel")); \
    my_sizer->Add(tmpOK,0,wxALIGN_CENTER_HORIZONTAL); \
    my_sizer->Add(tmpCANCEL,0,wxALIGN_CENTER_HORIZONTAL); \
    SetAutoLayout(TRUE); \
    SetSizer(my_sizer); \
    my_sizer->SetSizeHints(this); \
    my_sizer->Fit(this); }
}
#endif

