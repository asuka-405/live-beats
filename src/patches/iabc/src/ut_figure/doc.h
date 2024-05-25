/////////////////////////////////////////////////////////////////////////////
// Name:        doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: doc.h,v 1.3 2005/04/24 10:26:16 aaron_newman Exp $
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

namespace iabc
{
class simple_figure:public figure
{
    // derivation draws the thing here.
public:
    simple_figure(const size& ppi):figure(ppi){};
protected:

    virtual void create_poly(polygon &poly);
    // Set the scale of the drawing surface.  Many of the drawing routines
    // we have use twips in a postscript-like coordinate system, but some
    // are in absolute pixels.  The default behavior is the postscript like
    // scaling.
    virtual void set_xform(polygon &poly) 
    {
        poly.set_x_zoom(1.0);
        poly.set_y_zoom(1.0);
    }
};
class ut_figure:public document
{
public:
    virtual void redraw(const rect &the_rect);
    ut_figure();
    virtual ~ut_figure();
    void attach_canvas(canvas &the_canvas);
    
    rect get_rect_from_page(int the_page);
    void detach_canvas(canvas &the_canvas);
    virtual void print_page(int the_page){return;};
    bool has_page(int the_page){return false;};
    semaphore my_exit_semaphore;
    window* my_window;
    figure* my_clef1;
    figure* my_clef2;
    figure* my_clef3;
    text_figure* my_figure;
};
}

class DrawingDocument: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(DrawingDocument)
private:
public:
    
    DrawingDocument(void);
    ~DrawingDocument(void);
    void StartABCDocument(iabc::window& the_window);
    void Redraw(){my_abc_doc->redraw(iabc::rect());};
private:
	iabc::ut_figure* my_abc_doc;
};



#endif
