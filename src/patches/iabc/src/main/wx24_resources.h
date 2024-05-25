#ifndef wx_resources_h
#define wx_resources_h

#include "wx/wx.h"
#include "iabc/iabc_preferences.h"
#include "iabc/pitch.h"
#include "wx/spinctrl.h"

// FILE: wx24_resources
// SUBSYSTEM: GUI
// DESCRIPTION:
// Define most of the dialog boxes and similar gadgets that are defined in this
// program.
extern bool the_view_parameters_changed;
extern int the_transpose_on_tick_steps;

// CLASS: wxTextQuerySizer
// DESCRIPTION:
// A text widget in a dialog box.  Acts just like a wxTextCtrl combined with a sizer.
class wxTextQuerySizer
{
public:
    // METHOD:    ctor
    // DESCRIPTION:
    // Create the widget and bind it to the parent window
    wxTextQuerySizer(wxWindow* theParent,const char* theStaticPrompt,const char* theDefaultValue,const char* theName = "");
    
    // METHOD: ~ctor
    // DESCRIPTION:
    // release the resources
    virtual ~wxTextQuerySizer();
    
    // METHOD: wxSizer*()
    // DESCRIPTION:
    // Act just like a sizer to the client
    operator wxSizer*() {return mySizer;};

    // METHOD: SetValue
    // DESCRIPTION:
    // Set the value of the widget
    void SetValue(const wxString& s);
private:
    // ATTRIBUTE: wxStaticText* myStaticText
    // DESCRIPTION:
    // The label for the control
    wxStaticText* myStaticText;
    // ATTRIBUTE: wxTextCtrl* myTextCtrl
    // DESCRIPTION:
    // the control itself
    wxTextCtrl* myTextCtrl;
    // ATTRIBUTE: wxBoxSizer* mySizer
    // DESCRIPTION:
    // the parent sizer
    wxBoxSizer* mySizer;
};

// CLASS: wxBooleanQuerySizer
// DESCRIPTION:
// Act just like a sizer, allow the user to make boolean choices.  Implemented
// as a checkbox probably
class wxBooleanQuerySizer
{
public:
    wxBooleanQuerySizer(wxWindow* theParent,const char* theStaticPrompt,bool theDefaultValue,const char* theName = "");
    virtual ~wxBooleanQuerySizer();
    operator wxSizer*() {return mySizer;};
    void SetValue(bool b);
private:
    wxCheckBox* myCheckBox;
    wxBoxSizer* mySizer;
};

// CLASS: wxIntSpinSizer
// DESCRIPTION:
// Allows the user to choose a number with arrows up/down that increment
// /decrement the values.
class wxIntSpinSizer
{
public:
    wxIntSpinSizer(wxWindow* theParent,const char* theStaticPrompt,const char* theDefaultValue,const char* theName = "");
    virtual ~wxIntSpinSizer();
    operator wxSizer*() {return mySizer;};
    void SetValue(int i);
    int GetValue();
private:
    wxSpinCtrl* myControl;
    wxBoxSizer* mySizer;
};

// CLASS: wx_percentage_dialog : public wxDialog
// DESCRIPTION:
// Just displays progress for a long operation
class wx_percentage_dialog : public wxDialog
{
public:
    wx_percentage_dialog();
    virtual ~wx_percentage_dialog();
    void OnCancel(wxCommandEvent& event);
    
private:
    DECLARE_EVENT_TABLE();
};

// CLASS: wx_window_media_pp:public wxDialog
// DESCRIPTION:
// The dialog box that contains all of the display
// choices like page size, etc.
class wx_score_mode_db:public wxDialog
{
public:
    wx_score_mode_db();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);
private:
    wxBooleanQuerySizer* myScoreMode;
    wxBooleanQuerySizer* myPageBreakBetweenParts;
    wxGridSizer* my_sizer;
    DECLARE_EVENT_TABLE();
};

// CLASS: wx_window_media_pp:public wxDialog
// DESCRIPTION:
// The dialog box that contains all of the display
// choices like page size, etc.
class wx_window_media_pp:public wxDialog
{
public:
    wx_window_media_pp();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);
private:
    wxTextQuerySizer* myPageHeight;    
    wxTextQuerySizer* myPageWidth;    
    wxTextQuerySizer* myXScale;    
    wxTextQuerySizer* myYScale;    
    wxTextQuerySizer* myNoteScale;
    wxTextQuerySizer* myTitleSize;
    wxTextQuerySizer* myLeftMargin;
    wxTextQuerySizer* myRightMargin;
    wxTextQuerySizer* myTopMargin;
    wxTextQuerySizer* myBottomMargin;
    wxTextQuerySizer* myWordSize;
    wxTextQuerySizer* myCopyrightSize;
    wxTextQuerySizer* mySongwordSize;
    wxTextQuerySizer* myChordSize;
    wxBooleanQuerySizer* myChordBold;
    wxBooleanQuerySizer* myChordItalic;
    wxBooleanQuerySizer* myWordsItalic;
    wxGridSizer* my_sizer;
    DECLARE_EVENT_TABLE();
};

// CLASS: wx_abc_options_dialog
// DESCRIPTION:
// A dialog box that allows the user t ochoose some
// options that affect the parser.
class wx_abc_options_dialog:public wxDialog
{
public:
    wx_abc_options_dialog();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);
private:
    wxBooleanQuerySizer* my_no_brackets;
    wxBooleanQuerySizer* my_text_numbers;
    wxBooleanQuerySizer* my_barfly_compatibilty;
    wxGridSizer* my_sizer;
    DECLARE_EVENT_TABLE();
};

class wx_voice_options_dialog:public wxDialog
{
public:
    wx_voice_options_dialog(iabc::map<int,bool>& the_voice_on_map,
                 iabc::map<int,iabc::string>& the_voice_name_map);
    virtual ~wx_voice_options_dialog();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);
private:
    wxGridSizer* my_sizer;
    iabc::map<int,bool> my_voice_on_map;
    iabc::map<int,iabc::string> my_voice_name_map;
    DECLARE_EVENT_TABLE();
};

// CLASS: wx_midi_options_dialog
// DESCRIPTION:
// Allows the user to choose midi player, etc.
class wx_midi_options_dialog:public wxDialog
{
public:
    wx_midi_options_dialog();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);
    void OnChooseMidiDir(wxCommandEvent& event);
    void OnChooseMidiPlayer(wxCommandEvent& event);
private:
    wxTextQuerySizer* my_midi_directory;
    wxTextQuerySizer*  my_midi_player;
    wxTextQuerySizer*  my_midi_player_command;
    wxGridSizer* my_sizer;
    DECLARE_EVENT_TABLE();
};

class wx_transpose_dialog:public wxDialog
{
public:
    wx_transpose_dialog();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnInitDialog(wxInitDialogEvent& event);
private:
    wxIntSpinSizer* mySpinSizer;
    wxGridSizer* my_sizer;
    wxBooleanQuerySizer* my_diatonic;
    wxBooleanQuerySizer* my_refresh;
    DECLARE_EVENT_TABLE();
};

extern wx_percentage_dialog* global_progress_dialog;
extern wx_score_mode_db* global_score_mode_db;
extern wx_window_media_pp* global_window_media_pp;
extern wx_abc_options_dialog* global_abc_options_dialog;
extern wx_midi_options_dialog* global_midi_options_dialog;
extern wx_transpose_dialog* global_transpose_dialog;

#endif


