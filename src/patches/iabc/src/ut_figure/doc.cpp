#include "doc.h"
#include <wx/wx.h>
#include "iabc/wx_winres.h"
#include "iabc/wx_dispatch.h"
#include "iabc/string.h"
#include "iabc/wx_winres.h"
#include "iabc/figure_factory.h"
#include "iabc/threads.h"
#include "iabc/event_log.h"

#define DOCVIEW_ABOUT   2

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp(void);
    bool OnInit(void);
    int OnExit(void);
    
    wxFrame *CreateChildFrame(wxDocument *doc, wxView *view, bool isCanvas);
    
protected:
    wxFrame* frame;
};

DECLARE_APP(MyApp)

namespace iabc
{
;

void
simple_figure::create_poly(polygon& the_poly)
{
    the_poly.moveto_relative(point(0,0));
    the_poly.curveto_relative(point(-25,25),point(-25,75),point(100,0));
    the_poly.curveto_relative(point(25,-25),point(25,-75),point(-100,0));
    the_poly.fill_poly();

    the_poly.moveto_relative(point(0,120));
    the_poly.lineto_relative(point(0,100));
    the_poly.lineto_relative(point(100,0));
    the_poly.lineto_relative(point(0,-100));
    the_poly.lineto_relative(point(-20,0));
    the_poly.lineto_relative(point(0,80));
    the_poly.lineto_relative(point(-60,0));
    the_poly.lineto_relative(point(0,-80));
    the_poly.lineto_relative(point(-20,0));
    the_poly.fill_poly();
}

void 
ut_figure::redraw(const rect &the_rect)
{
    my_clef1->dispatch_draw_self(*my_window,point(200,150),true);
    my_clef2->dispatch_draw_self(*my_window,point(100,100),true);
    my_clef3->dispatch_draw_self(*my_window,point(300,100),true);
    pen* tmp_pen = my_window->get_pen(0,0,0,1);
    if (tmp_pen)
    {
        tmp_pen->res_add_ref();
        line tmp_line(point(0,0),point(50,50));
        my_window->draw_line(*tmp_pen,tmp_line);
        tmp_line = line(point(0,50),point(50,0));
        my_window->draw_line(*tmp_pen,tmp_line);
        tmp_pen->res_remove_ref();

        simple_figure* tmp_figure = new simple_figure(my_window->pixels_per_inch());
		tmp_figure->add_ref();
        tmp_figure->dispatch_draw_self(*my_window,point(200,200));
        tmp_figure->remove_ref();
    }
}

ut_figure::ut_figure()
{
}

ut_figure::~ut_figure()
{
    my_clef1->remove_ref();
    my_clef2->remove_ref();
}

rect 
ut_figure::get_rect_from_page(int the_page)
{
    if (the_page == 1)
    {
        return my_window->get_visible_rect();
    }
    return rect();
}

void 
ut_figure::attach_canvas(canvas &the_canvas)
{
    window* tmp_window = dynamic_cast<window*>(&the_canvas);
    if (tmp_window)
    {
        tmp_window->res_add_ref();
        my_window = tmp_window;
    }
    this->my_clef1 = figure_factory::get_instance().get_figure((*my_window).pixels_per_inch(),
                                                               figure_factory::GCLEF);
    my_clef1->add_ref();
    my_clef1->rescale(scale(1.5,1.5));
    my_clef2 = figure_factory::get_instance().get_figure(
		(*my_window).pixels_per_inch(),
		figure_factory::QUARTER_REST);
    my_clef2->add_ref();
    my_clef2->rescale(scale(2.0,2.0));
    my_clef3 = figure_factory::get_instance().get_figure(
        (*my_window).pixels_per_inch(),
        figure_factory::EIGHTH_REST);
    my_clef3->add_ref();
    my_clef3->rescale(scale(2.0,2.0));
}

void 
ut_figure::detach_canvas(canvas &the_canvas)
{
    my_window->res_remove_ref();
}
}
// main.cpp : Defines the entry point for the application.
//
// CLASS: wx_view_frame
// DESCRIPTION:
// Each major window in wxWindows consists of a frame and a canvas.
// Define the frame.  The canvas is described in wx_winres.cpp.
namespace iabc
{
;
class wx_view_canvas;
class wx_view_frame: public wxFrame
{
    DECLARE_CLASS(wx_view_frame)
public:
    wx_view_canvas *canvas;
    wx_window* my_abc_window;

    wx_view_frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    
    void OnAbout(wxCommandEvent& event);
    void OnPrint(wxCommandEvent& event);
    wx_view_canvas *CreateCanvas(wxFrame *parent);
    void OnCloseWindow(wxCloseEvent& the_event);
	void OnCloseMenu(wxCommandEvent& the_event);
    void OnWindowCreate(wxShowEvent& the_event);
    void OnDumpLog(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()
    wx_printout* my_last_printout;
    wxPrinter* my_last_printer;
    string my_last_file;
	iabc::ut_figure* my_abc_doc;
};

#define ON_DUMP_LOG (wxID_HIGHEST + 1)

IMPLEMENT_CLASS(wx_view_frame, wxFrame)
BEGIN_EVENT_TABLE(wx_view_frame, wxFrame)
    EVT_MENU(DOCVIEW_ABOUT, wx_view_frame::OnAbout)
    EVT_MENU(ON_DUMP_LOG, wx_view_frame::OnDumpLog)
    EVT_MENU(wxID_PRINT, wx_view_frame::OnPrint)
    EVT_MENU(wxID_EXIT, wx_view_frame::OnCloseMenu)
    EVT_CLOSE(wx_view_frame::OnCloseWindow)
    EVT_SHOW(wx_view_frame::OnWindowCreate)
END_EVENT_TABLE()

wx_view_frame::wx_view_frame(const wxString& title, const wxPoint& pos, const wxSize& size):
wxFrame((wxFrame *)NULL, -1, title, pos, size),
my_abc_window(0),canvas(0),my_last_printer(0),my_last_printout(0)
{
}

void wx_view_frame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
    (void)wxMessageBox("iabc (C) Aaron Newman 2002");
}

void wx_view_frame::OnCloseMenu(wxCommandEvent& the_event)
{
    wxCloseEvent other_event;
    OnCloseWindow(other_event);
}

void
wx_view_frame::OnCloseWindow(wxCloseEvent& the_event)
{
    dispatcher::end_app();
    thread::sleep(1000);
    thread_mgr::get_instance().shutdown_application();
	Destroy();
}

void
wx_view_frame::OnPrint(wxCommandEvent& event)
{
#if 0
    if (my_last_printout != 0)
    {
        delete my_last_printout;
        my_last_printout = 0;
    }
    if (my_last_printer != 0)
    {
        delete my_last_printer;
        my_last_printer = 0;
    }
    my_print_wxWindow = 
                   new wx_print_canvas(my_abc_);

    if ((my_abc_doc != 0) && (my_abc_window != 0))
    {
        my_last_printout = new wx_printout(*my_abc_doc,*my_abc_window);
        my_last_printer = new wxPrinter();
        my_last_printer->Print(this,my_last_printout);
    }
#endif
}

void wx_view_frame::OnDumpLog(wxCommandEvent& WXUNUSED(event) )
{
#ifndef GCC
    FILE* f = fopen("C:\\evlog.txt","w");
#else
    FILE* f = fopen("/tmp/evlog.txt","w");
#endif
    iabc::all_event_logs::print_logs(f);
    fclose(f);
}

// The displaying of the iabc files is done be a window media, which is
// derived from document.  The document is created or destroyed when
// the user exits the application or opens another window.
iabc::wx_view_canvas *wx_view_frame::CreateCanvas(wxFrame *parent)
{
    int width, height;
    parent->GetClientSize(&width, &height);
    
    // Non-retained canvas
	iabc::wx_view_canvas *canvas = new 
		iabc::wx_view_canvas(parent, wxPoint(0, 0), wxSize(width, height), 0);
    
    // Give it scrollbars
    canvas->SetScrollbars(20, 20, 50, 50);
    canvas->SetBackgroundColour(*wxWHITE);
    
    return canvas;
}

void
wx_view_frame::OnWindowCreate(wxShowEvent& the_event)
{
    if (canvas == 0)
    {
        canvas = CreateCanvas(this);
    #ifdef __X__
        // X seems to require a forced resize
        int x, y;
        GetSize(&x, &y);
        SetSize(-1, -1, x, y);
    #endif
        my_abc_window = new iabc::wx_window("Foo",*canvas);
        my_abc_doc = new iabc::ut_figure;
        wx_dispatcher::wx_set_default_canvas(*my_abc_window);
        canvas->set_window(my_abc_window);
        my_abc_window->res_add_ref();
        my_abc_window->add_document(my_abc_doc);
    }
}

}

wxApp *wxCreateApp() { return new MyApp; }         \
wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp); \
MyApp& wxGetApp() { return *(MyApp *)wxTheApp; } \
IMPLEMENT_WXWIN_MAIN

MyApp::MyApp(void)
{
}

bool MyApp::OnInit(void)
{
    //// Create the main frame window
    frame = new iabc::wx_view_frame
        ("iabc",wxPoint(100,100),wxSize(500,400));
    
    //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
    frame->SetIcon(wxIcon("doc_icn"));
#endif
    
    //// Make a menubar
    wxMenu *file_menu = new wxMenu;
    
    // file_menu->Append(wxID_NEW, "&New...");
    file_menu->Append(wxID_PRINT, "&Print...");
    
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "E&xit");
    
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(DOCVIEW_ABOUT, "&About");
    help_menu->Append(ON_DUMP_LOG, "&Dump Log");
    
    wxMenuBar *menu_bar = new wxMenuBar;
    
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(help_menu, "&Help");
    
    //// Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);
    
    frame->Centre(wxBOTH);
    SetTopWindow(frame);

    frame->Show(TRUE);
    frame->SetFocus();

    return TRUE;
}

int MyApp::OnExit(void)
{
    return 0;
}




