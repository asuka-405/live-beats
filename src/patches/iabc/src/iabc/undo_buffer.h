/*
 * undo_buffer.h - implement basic undo functionality
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
#ifndef iabc_undo_buffer_h
#define iabc_undo_buffer_h
#include <wx/textctrl.h>
#include <wx/frame.h>
#include "iabc/string.h"
#include "iabc/list.h"
#define MAX_UNDO_EVENTS 800
namespace iabc{
class undo_event
{
public:
    undo_event();
    undo_event(const undo_event& o);
    undo_event& operator=(const undo_event& o);
    ~undo_event();
    long start_select;
    long end_select;
    bool is_modified;
    bool is_pre_save;
    wxKeyEvent* the_event;
    string the_buffer;
};

class undo_buffer
{
public:
    static void push(const undo_event& e);
    static undo_event pop();
    static int get_size();
    static void clear();
    static void mark_save();
private:
    static list<undo_event> my_buffer;
   
};
}
#endif
