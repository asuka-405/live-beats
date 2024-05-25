/////////////////////////////////////////////////////////////////////////////
// Name:        view.h
// Purpose:     View classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: view.h,v 1.2 2003/12/31 06:07:55 aaron_newman Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __VIEWSAMPLEH__
#define __VIEWSAMPLEH__

#include "wx/docview.h"
#include "iabc/wx_winres.h"

class DrawingView: public wxView
{
    DECLARE_DYNAMIC_CLASS(DrawingView)
private:
public:
    wxFrame *frame;
    iabc::wx_view_canvas *canvas;
    
    DrawingView(void) { canvas = (iabc::wx_view_canvas *) NULL; frame = (wxFrame *) NULL; };
    ~DrawingView(void) {};
    
    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = TRUE);
    
    void OnCut(wxCommandEvent& event);
    
    DECLARE_EVENT_TABLE()
};

#endif
