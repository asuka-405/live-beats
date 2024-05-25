/////////////////////////////////////////////////////////////////////////////
// Name:        doc.cpp
// Purpose:     Implements document functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: doc.cpp,v 1.2 2003/12/31 06:07:55 aaron_newman Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/txtstrm.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "doc.h"
#include "view.h"
#include "iabc/figure_factory.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingDocument, wxDocument)

DrawingDocument::DrawingDocument(void):
my_abc_doc(0),my_window(0)
{
}

DrawingDocument::~DrawingDocument(void)
{
    // doodleSegments.DeleteContents(TRUE);
}

void 
DrawingDocument::StartABCDocument(iabc::window& the_window)
{
	my_window = &the_window;
    if (my_abc_doc)
    {
        the_window.res_add_ref();
        the_window.add_document(my_abc_doc);
        my_abc_doc->start();
    }
}


bool 
DrawingDocument::OnOpenDocument(const wxString& file)
{
    iabc::string tmp_string = (const char*)file;
    my_abc_doc = new iabc::notetest(tmp_string);
    if (my_window)
    {
        my_window->res_add_ref();
        my_window->add_document(my_abc_doc);
        my_abc_doc->start();
    }
	return true;
}

