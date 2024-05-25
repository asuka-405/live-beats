/*
 * wx_list_box.cpp - wxWindows implementation of a explorer-type chooser.
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
#include "iabc/wx_list_box.h"
#include "iabc/drawtemp.cpp"
#include "iabc/list.cpp"
#include "iabc/map.cpp"

namespace iabc
{
;
IMPLEMENT_DYNAMIC_CLASS(wx_tree, wxTreeCtrl)

BEGIN_EVENT_TABLE(wx_tree, wxTreeCtrl)
	EVT_TREE_ITEM_ACTIVATED(wx_tree::wx_tree_select, wx_tree::OnActivate)
END_EVENT_TABLE()

int wx_tree::class_instance_count = 0;

wx_tree::wx_tree(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& the_size,
           long style,
           const wxValidator &validator,
           const wxString& name ):
wxTreeCtrl(parent,id,pos,the_size,style,validator,name)
{
    ++class_instance_count;
}
wx_tree::~wx_tree()
{
    --class_instance_count;
}

void 
wx_tree::set_selection_handler(selection_handler the_handler)
{
    my_handler = the_handler;
}

void 
wx_tree::OnActivate(wxTreeEvent& event)
{
    my_handler.select();
}

int wx_list_box_control::class_instance_count = 0;

wx_list_box_control::wx_list_box_control(const registry_entry& the_entry,
                                         wx_tree& the_box):
my_list_box(&the_box),my_should_initialize(true)
{
}

void 
wx_list_box_control::initialize_if_required()
{
    if (my_should_initialize)
    {
        my_should_initialize = false;
        window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
        display_list_box_command::dispatch_draw_command(tmp_window,
            create,*this,true);
    }
}

void
wx_list_box_control::add_ref()
{
    ++my_counter;
}

void
wx_list_box_control::remove_ref()
{
    if (--my_counter == 0)
    {
        delete this;
    }
}

void 
wx_list_box_control::set_final_value()
{
    initialize_if_required();
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    display_list_box_command::dispatch_draw_command(tmp_window,
		set_final,*this,true);
}

void 
wx_list_box_control::draw_self(window& w,
                               wx_list_box_control::list_box_operations the_operation)
{
    if (the_operation == wx_list_box_control::set_final)
    {
        wx_set_final_value();
    }
    else if (the_operation == wx_list_box_control::create)
    {
        wx_create();
    }
    else
        throw("Illegal choice in list box dispatch");
}

void
wx_list_box_control::wx_set_final_value()
{
    if (my_list_box)
    {
		// Retreive the value first since we may repopulate
		// the list box after the selection and this will wipe out
		// any selection the user entered.
        retreive_value_from_box();
        populate_list_box();
    }
}

wx_list_box_control::~wx_list_box_control()
{
    --class_instance_count;
    my_list_box->remove_ref();
}

void
wx_list_box_control::wx_create()
{
    ++class_instance_count;
    my_list_box->DeleteAllItems();
    my_list_box->AddRoot(_T("File"));
    my_list_box->add_ref();
    my_list_box->Show();
}

}
