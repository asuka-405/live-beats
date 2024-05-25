/////////////////////////////////////////////////////////////////////////////
// Name:        view.cpp
// Purpose:     View classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: view.cpp,v 1.2 2003/12/31 06:07:55 aaron_newman Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

#include "iabc/wx_winres.h"
#include "iabc/wx_dispatch.h"
#include "iabc/notetest.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "docview.h"
#include "doc.h"
#include "view.h"
#include "iabc/wx_dispatch.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView)
BEGIN_EVENT_TABLE(DrawingView,wxView)
END_EVENT_TABLE()

// For drawing lines in a canvas
float xpos = -1;
float ypos = -1;

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    if (!singleWindowMode)
    {
        // Multiple windows
        frame = wxGetApp().CreateChildFrame(doc, this, TRUE);
        frame->SetTitle("<untitled>");
        
        canvas = GetMainFrame()->CreateCanvas(this, frame);
#ifdef __X__
        // X seems to require a forced resize
        int x, y;
        frame->GetSize(&x, &y);
        frame->SetSize(-1, -1, x, y);
#endif
        frame->Show(TRUE);
		iabc::wx_window* tmp_win = new iabc::wx_window("Foo",*canvas);
		tmp_win->res_add_ref();
        DrawingDocument* tmp_doc = (DrawingDocument*) doc;
        tmp_doc->StartABCDocument(*tmp_win);
    }
    else
    {
        // Single-window mode
        frame = GetMainFrame();
        canvas = GetMainFrame()->canvas;
        canvas->view = this;
        
        // Associate the appropriate frame with this view.
        SetFrame(frame);
        
        // Make sure the document manager knows that this is the
        // current view.
        Activate(TRUE);
        
        // Initialize the edit menu Undo and Redo items
        doc->GetCommandProcessor()->SetEditMenu(((MyFrame *)frame)->editMenu);
        doc->GetCommandProcessor()->Initialize();
    }
    
    return TRUE;
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void DrawingView::OnDraw(wxDC *dc)
{
    DrawingDocument* tmp_doc = (DrawingDocument*)GetDocument();
    dc->BeginDrawing();
    tmp_doc->Redraw();
    dc->EndDrawing();
}

void DrawingView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    DrawingDocument* tmp_doc = (DrawingDocument*)GetDocument();
    tmp_doc->Redraw();
    
    if (canvas)
        canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool DrawingView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close())
        return FALSE;
    
    // Clear the canvas in  case we're in single-window mode,
    // and the canvas stays.
    canvas->Clear();
    canvas->view = (wxView *) NULL;
    canvas = (iabc::wx_view_canvas *) NULL;
    
    wxString s(wxTheApp->GetAppName());
    if (frame)
        frame->SetTitle(s);
    
    SetFrame((wxFrame *) NULL);
    
    Activate(FALSE);
    
    if (deleteWindow && !singleWindowMode)
    {
        delete frame;
        return TRUE;
    }
    return TRUE;
}


