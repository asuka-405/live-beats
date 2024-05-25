/*
 * wx_list_box.h - wxWindows implementation of an explorer-type chooser.
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
#ifndef wx_list_box_h
#define wx_list_box_h
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include "wx/wx.h"
#include "iabc/drawtemp.h"
#include "iabc/wd_data.h"
#include "iabc/registry.h"

namespace iabc
{
;
// FILE: wx_property_page.h
// DESCRIPTION:
// These files all have to do with GUI artifacts used to get information
// from the user to an application.  
class wx_tree:public wxTreeCtrl
{
public:
    typedef enum enums
    {
        wx_tree_select = 1
    } enums;
    wx_tree(){++class_instance_count;};
    wx_tree(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& the_size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator &validator = wxDefaultValidator,
               const wxString& name = _T("TreeCtrl"));
    void OnActivate(wxTreeEvent& event);
    void set_selection_handler(selection_handler the_handler);

    // METHOD: add/remove ref
    // DESCRIPTION:
    // Don't do this because we will delete these explicitly since its a widows
    // resource.
    void add_ref(){};
    void remove_ref(){};
private:
    ~wx_tree();
    atomic_counter my_counter;
	DECLARE_DYNAMIC_CLASS(wx_tree)
    DECLARE_EVENT_TABLE()
    selection_handler my_handler;
    static int class_instance_count;
    wx_tree& operator=(const wx_tree& o);
    wx_tree(const wx_tree& o);
};

// CLASS: wx_list_box_control
// DESCRIPTION:
// A nested list of things, a widget that really everyone
// else calls a tree (and maybe someday I will too)
class wx_list_box_control
{
public:
    typedef enum list_box_operations
    {
        create,
        set_final
    } list_box_operations;

    friend class draw_command<wx_list_box_control,list_box_operations>;
    wx_list_box_control(const registry_entry& the_entry,
                        wx_tree& the_box);
    virtual void set_final_value();
    virtual ~wx_list_box_control();
    void add_ref();
    void remove_ref();
protected:
    void draw_self(window& w,list_box_operations the_operation);
    void wx_set_final_value();
    void wx_create();
    void initialize_if_required();
    virtual void populate_list_box() = 0;
    virtual void retreive_value_from_box() = 0;
    wx_tree* my_list_box;
    atomic_counter my_counter;
    bool my_should_initialize;
private:
    wx_list_box_control(const wx_list_box_control& o);
    wx_list_box_control& operator=(wx_list_box_control& o);
    static int class_instance_count;
};

typedef draw_command<wx_list_box_control,wx_list_box_control::list_box_operations> display_list_box_command;

}

#endif

