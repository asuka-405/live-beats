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
#include "iabc/wx_pp_temp.h"
//#include "iabc/wx_pp.h"

namespace iabc
{
;
template <class db_field_type,class global_type>
db_translator<db_field_type,global_type>::db_translator(global_type& the_global,reg::registry_default_value the_reg_entry,
                  db_field_type the_field):my_global(the_global),my_field(the_field),
    my_reg_entry(the_reg_entry)
{
}

template <class db_field_type,class global_type>
void 
db_translator<db_field_type,global_type>::populate_global_from_reg()
{
    iabc::registry_entry tmp_entry = 
        reg::get_default(my_reg_entry);
    reg_to_global(tmp_entry.get_value(),my_global);
}

template <class db_field_type,class global_type>
void 
db_translator<db_field_type,global_type>::populate_reg_from_global()
{
    iabc::registry& the_registry = iabc::globalRegistry;
    iabc::registry_entry tmp_entry = 
        reg::get_default(my_reg_entry);
    string tmp_value;
    global_to_reg(my_global,tmp_value);
    tmp_entry = tmp_value;
    the_registry.set_value(tmp_entry);
}

template <class db_field_type,class global_type>
void 
db_translator<db_field_type,global_type>::populate_db_from_global()
{
    global_to_db(my_global,my_field);
}

template <class db_field_type,class global_type>
void 
db_translator<db_field_type,global_type>::populate_global_from_db()
{
    db_to_global(my_field,my_global);
}

template <class global_type,class field_type>
void populate_global_from_reg1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry)
{
    db_translator<field_type,global_type> 
        tmp_translator(the_global,the_entry,the_field);

    tmp_translator.populate_global_from_reg();
}

template <class global_type,class field_type>
void populate_reg_from_global1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry)
{
    db_translator<field_type,global_type> 
        tmp_translator(the_global,the_entry,the_field);

    tmp_translator.populate_reg_from_global();
}

template <class global_type,class field_type>
void populate_db_from_global1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry)
{
    db_translator<field_type,global_type> 
        tmp_translator(the_global,the_entry,the_field);

    tmp_translator.populate_db_from_global();
}

template <class global_type,class field_type>
void populate_global_from_db1(global_type& the_global,field_type the_field,reg::registry_default_value the_entry)
{
    db_translator<field_type,global_type> 
        tmp_translator(the_global,the_entry,the_field);

    tmp_translator.populate_global_from_db();
}

}
