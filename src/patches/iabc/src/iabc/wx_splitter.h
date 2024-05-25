/*
 * wx_splitter.h - Derive a splitter class so that we can save the position changes
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
#ifndef wx_splitter_h
#define wx_splitter_h
#include <wx/wx.h>
#include <wx/splitter.h>

namespace iabc
{

class wx_splitter_notifier
{
public:
    typedef enum sash_direction{horizontal,vertical}sash_direction;

    wx_splitter_notifier(){};
    ~wx_splitter_notifier(){};
    virtual void notify_sash_change(double d) = 0;
};

class wx_splitter:public wxSplitterWindow
{
public:
    // Called when the sash position is about to be changed, return
    // FALSE from here to prevent the change from taking place.
    // Repositions sash to minimum position if pane would be too small.
    // newSashPosition here is always positive or zero.
    wx_splitter(wx_splitter_notifier& tmp_notifier,
                wxWindow *parent, wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_3D,
                     const wxString& name = wxT("splitter"));

    virtual bool OnSashPositionChange(int newSashPosition);
private:
    wx_splitter_notifier& my_notifier;
};
}

#endif
