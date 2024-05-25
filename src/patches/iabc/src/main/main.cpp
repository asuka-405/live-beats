// FILE: main.cpp 
// DESCRIPTION: 
// Defines the entry point for the application.

#include <wx/wx.h>
#include "iabc/factory.cpp"
#include "system_dirs.h"
#include "wx24_resources.h"
#include "iabc/wx_frame.h"
#include <iabc/unicode.h>

extern void LoadResources();

// CLASS: MyApp
// DESCRIPTION:
// Every application needs to create a derivation
// of hte wxApp class as an entry point.
// Define a new application
class MyApp: public wxApp
{
public:
    // METHOD: ctor
    // DESCRIPTION:
    // Create the main appplication window frame, and initialize
    // all of the pre-ordained dialog box objects.
    MyApp(void);

    // METHOD: OnInit
    // DESCRIPTION:
    // This is called when the main window is created.  We
    // make the window visible here.
    virtual bool OnInit(void);

    // METHOD: OnExit
    // DESCRIPTION:
    // Do some garbage collection and other cleanup, if you are debugging for
    // memory leaks this may be useful.
    int OnExit(void);

    // METHOD: MainLoop
    // DESCRIPTION:
    // Boilerplate stuff for the windows message pump.
    int MainLoop();
protected:
    // ATTRIBUTE: frame
    // DESCRIPTION:
    // the frame is the main application window.
    wxFrame* frame;
};

DECLARE_APP(MyApp)

#ifdef _MSC_VER
#include <windows.h>
extern void
win32_exception(_EXCEPTION_POINTERS* ep);
#define unlink _unlink
#endif

int
MyApp::MainLoop()
{
#ifdef _MSC_VER
    __try
    {
#endif
        wxApp::MainLoop();
#ifdef _MSC_VER
    }
    __except(win32_exception(GetExceptionInformation()))
    {
        ;
    }
#endif
    return 0;
}
IMPLEMENT_APP(MyApp)

// wxApp *wxCreateApp() { return new MyApp; }         \
// wxAppInitializer wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp); \
// MyApp& wxGetApp() { return *(MyApp *)wxTheApp; } \
// IMPLEMENT_WXWIN_MAIN

MyApp::MyApp(void)
{
    // Figure out Home, etc. before we start changing dirs and stuff.
    iabc::set_system_dirs(argv,argc);

    iabc::instance_counter<iabc::figure>::clock_in();
    iabc::instance_counter<iabc::text_figure>::clock_in();
    iabc::instance_counter<iabc::wd_command>::clock_in();
    iabc::instance_counter<iabc::page_contents>::clock_in();
    iabc::instance_counter<iabc::scan_state>::clock_in();
    iabc::instance_counter<iabc::regexp>::clock_in();
    iabc::instance_counter<iabc::string>::clock_in();

}

bool MyApp::OnInit(void)
{
    //// Give it an icon (this is ignored in MDI mode: uses resources)
    // We need to do this before we make the frame window.
    LoadResources();

    iabc::wx_view_frame* tmp_frame = 
        new iabc::wx_view_frame
        (B2U("iabc"),wxPoint(100,100),wxSize(500,400));

    tmp_frame->init();

    frame = tmp_frame;

#ifdef __WXMSW__
    frame->SetIcon(wxIcon(B2U("doc_icn")));
#endif


    frame->Centre(wxBOTH);
    SetTopWindow(frame);

    frame->Show(TRUE);
    frame->SetFocus();
    return TRUE;
}

int MyApp::OnExit(void)
{
    iabc::parser_state::gc();
    iabc::figure_factory::gc();
    iabc::thread_mgr::gc();
    iabc::registry::gc();
    iabc::instance_counter<iabc::figure>::clock_out();
    iabc::instance_counter<iabc::text_figure>::clock_out();
    iabc::instance_counter<iabc::wd_command>::clock_out();
    iabc::instance_counter<iabc::page_contents>::clock_out();
    iabc::instance_counter<iabc::scan_state>::clock_out();
    iabc::instance_counter<iabc::regexp>::clock_out();
    iabc::instance_counter<iabc::string>::clock_out();
    return 0;
}

