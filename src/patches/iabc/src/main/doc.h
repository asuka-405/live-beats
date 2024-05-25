/////////////////////////////////////////////////////////////////////////////
// Name:        doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: doc.h,v 1.2 2003/12/31 06:07:55 aaron_newman Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __DOCSAMPLEH__
#define __DOCSAMPLEH__

#include "wx/docview.h"
#include "iabc/winres.h"
#include "iabc/figure.h"
#include "iabc/text_figure.h"
#include "iabc/notetest.h"

class DrawingDocument: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(DrawingDocument)
private:
public:
    
    DrawingDocument(void);
    ~DrawingDocument(void);
    bool OnOpenDocument(const wxString& file);

    void StartABCDocument(iabc::window& the_window);
    void Redraw(){my_abc_doc->redraw(iabc::rect());};
private:
	iabc::notetest* my_abc_doc;
	iabc::window* my_window;
};



#endif
