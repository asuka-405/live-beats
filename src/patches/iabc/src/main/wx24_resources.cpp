#include "resource.h"

#include "resources.h"
#include "iabc/wx_percentage_meter.h"
#include "iabc/registry_defaults.h"
#include "wx/utils.h"
#include "wx24_resources.h"
#include <wx/wx.h>
#include "resources.cpp"
#include "iabc/abcrules.h"
#include "iabc/iabc_preferences.h"
#include "iabc/wx_pp_temp.cpp"
#include "iabc/drawtemp.cpp"
#include "iabc/map.cpp"
#include "iabc/unicode.h"

bool the_view_parameters_changed = false;
int the_transpose_on_tick_steps = 0;

wx_percentage_dialog*
global_progress_dialog = 0;

wx_window_media_pp*
global_window_media_pp = 0;

wx_score_mode_db*
global_score_mode_db = 0;

wx_abc_options_dialog* global_abc_options_dialog = 0;

wx_midi_options_dialog* global_midi_options_dialog = 0;

wx_transpose_dialog* global_transpose_dialog = 0;

BEGIN_EVENT_TABLE(wx_percentage_dialog, wxDialog)
	EVT_BUTTON(ID_PPCANCEL, wx_percentage_dialog::OnCancel)
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(wx_window_media_pp, wxDialog)
	EVT_BUTTON(ID_PPCANCEL, wx_window_media_pp::OnCancel)
	EVT_BUTTON(ID_PPOK, wx_window_media_pp::OnOK)
    EVT_INIT_DIALOG(wx_window_media_pp::OnInitDialog)
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(wx_score_mode_db, wxDialog)
	EVT_BUTTON(ID_PPCANCEL, wx_score_mode_db::OnCancel)
	EVT_BUTTON(ID_PPOK, wx_score_mode_db::OnOK)
    EVT_INIT_DIALOG(wx_score_mode_db::OnInitDialog)
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(wx_abc_options_dialog, wxDialog)
	EVT_BUTTON(ID_PPCANCEL, wx_abc_options_dialog::OnCancel)
	EVT_BUTTON(ID_PPOK, wx_abc_options_dialog::OnOK)
    EVT_INIT_DIALOG(wx_abc_options_dialog::OnInitDialog)
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(wx_midi_options_dialog, wxDialog)
    EVT_BUTTON(ID_PPCANCEL,wx_midi_options_dialog::OnCancel)
	EVT_BUTTON(ID_PPOK, wx_midi_options_dialog::OnOK)
    EVT_INIT_DIALOG(wx_midi_options_dialog::OnInitDialog)
    EVT_BUTTON (ID_CHOOSE_PLAYER_BUTTON,wx_midi_options_dialog::OnChooseMidiPlayer)
    EVT_BUTTON(ID_CHOOSE_DIR_BUTTON,wx_midi_options_dialog::OnChooseMidiDir)
END_EVENT_TABLE();

BEGIN_EVENT_TABLE(wx_transpose_dialog, wxDialog)
    EVT_BUTTON(ID_PPCANCEL,wx_transpose_dialog::OnCancel)
	EVT_BUTTON(ID_PPOK, wx_transpose_dialog::OnOK)
    EVT_INIT_DIALOG(wx_transpose_dialog::OnInitDialog)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wx_voice_options_dialog,wxDialog)
    EVT_BUTTON(ID_PPCANCEL,wx_voice_options_dialog::OnCancel)
    EVT_BUTTON(ID_PPOK,wx_voice_options_dialog::OnOK)
    EVT_INIT_DIALOG(wx_voice_options_dialog::OnInitDialog)
END_EVENT_TABLE()

void
LoadResources()
{
    global_midi_options_dialog = new wx_midi_options_dialog;
	global_abc_options_dialog = new wx_abc_options_dialog;
    global_transpose_dialog = new wx_transpose_dialog;
    global_progress_dialog = new wx_percentage_dialog;
    global_score_mode_db = new wx_score_mode_db;
    global_window_media_pp = new wx_window_media_pp;
}

void
UnloadResources()
{
    if (global_progress_dialog)
	    global_progress_dialog->Destroy();
    if (global_window_media_pp) 
        global_window_media_pp->Destroy();
    if (global_abc_options_dialog)
        global_abc_options_dialog->Destroy();
    if (global_midi_options_dialog)
        global_midi_options_dialog->Destroy();
    if (global_transpose_dialog)
        global_transpose_dialog->Destroy();
    if (global_score_mode_db)
        global_score_mode_db->Destroy();
}

wx_percentage_dialog::wx_percentage_dialog()
             :wxDialog(0, 
                       -1, 
                       wxString(_T("Percentage")))

{
    wxButton* button = new wxButton(this,ID_PPCANCEL,_T("Cancel"));
    button->SetLabel(_T("Cancel"));
    button->SetName(_T("cancel_button"));
	wxStaticText* tmp_caption = new wxStaticText(this,-1,_T("Caption"));
    tmp_caption->SetName(_T("caption"));
    wxStaticText* tmp_percent_label = new wxStaticText(this,-1,_T("Percentage"));
    tmp_percent_label->SetName(_T("percent_label"));
    wxGauge* tmp_gauge = new wxGauge(this,-1,100);
    tmp_gauge->SetName(_T("gauge2"));
    wxBoxSizer* r1 = new wxBoxSizer(wxHORIZONTAL);
    r1->Add(tmp_caption);
    wxBoxSizer* r2 = new wxBoxSizer(wxHORIZONTAL);
    r2->Add(tmp_percent_label);
    r2->Add(tmp_gauge);
    wxBoxSizer* r3 = new wxBoxSizer(wxHORIZONTAL);
    r3->Add(button);
    wxBoxSizer* top = new wxBoxSizer(wxVERTICAL);
    top->Add(r1);
    top->Add(r2);
    top->Add(r3);
    top->SetMinSize(100,-1);
    SetAutoLayout(TRUE);
    SetSizer(top);

    top->SetSizeHints(this);
    top->Fit(this);
}

wx_percentage_dialog::~wx_percentage_dialog()
{
}

void
wx_percentage_dialog::OnCancel(wxCommandEvent& wxUnused)
{
	wxButton *button = (wxButton *)wxFindWindowByName(_T("cancel_button"), this);

	iabc::wx_percentage_meter::static_set_cancelled(button);
}

wxTextQuerySizer::wxTextQuerySizer(wxWindow* theParent,const char* theStaticPrompt,const char* theDefaultValue,const char* theName)
{
    wxString tmpName;
    int x,y;
    myStaticText = new wxStaticText(theParent,-1,B2U(theStaticPrompt));
    myTextCtrl = new wxTextCtrl(theParent,-1,B2U(theDefaultValue),wxPoint(),wxSize(),0);
    if (B2U(theName) == wxString(_T(""))) 
    {
        tmpName = B2U(theStaticPrompt); 
    }
	else
	{
		tmpName = B2U(theName);
	}


    myTextCtrl->SetName(tmpName);
    mySizer = new wxBoxSizer(wxHORIZONTAL);
    mySizer->Add(myStaticText, 0, wxALIGN_LEFT | wxALL, 5);
    mySizer->Add(myTextCtrl, 1, wxGROW | wxALL, 5);
    myTextCtrl->GetTextExtent(B2U(theDefaultValue),&x,&y);
    mySizer->SetMinSize(200,y *2 + 5);
}

void 
wxTextQuerySizer::SetValue(const wxString& s)
{
    myTextCtrl->SetValue(s);
    this->mySizer->Layout();
}

wxTextQuerySizer::~wxTextQuerySizer()
{
	myTextCtrl->Destroy();
	myStaticText->Destroy();
}

wxBooleanQuerySizer::wxBooleanQuerySizer(wxWindow* theParent,const char* theStaticPrompt,bool theDefaultValue,const char* theName)
{
    myCheckBox = new wxCheckBox(theParent,-1,B2U(theStaticPrompt));
    myCheckBox->SetValue(theDefaultValue);
    myCheckBox->SetName(B2U(theName));
    mySizer = new wxBoxSizer(wxHORIZONTAL);
    mySizer->Add(myCheckBox, 1, wxGROW | wxALL, 5);
}

void 
wxBooleanQuerySizer::SetValue(bool b)
{
    myCheckBox->SetValue(b);
}

wxBooleanQuerySizer::~wxBooleanQuerySizer()
{
    myCheckBox->Destroy();
}

wxIntSpinSizer::wxIntSpinSizer(wxWindow* theParent,const char* theStaticPrompt,const char* theDefaultValue,const char* theName)
{
    myControl = new wxSpinCtrl(theParent,-1,B2U(theStaticPrompt));
    myControl->SetValue(B2U(theDefaultValue));
    myControl->SetName(B2U(theName));
    myControl->SetRange(-48,48);
    mySizer = new wxBoxSizer(wxHORIZONTAL);
    mySizer->Add(myControl,1,wxGROW|wxALL,5);
}

wxIntSpinSizer::~wxIntSpinSizer()
{
    myControl->Destroy();
}

void 
wxIntSpinSizer::SetValue(int i)
{
    myControl->SetValue(i);
}

int
wxIntSpinSizer::GetValue()
{
    return myControl->GetValue();
}

wx_score_mode_db::wx_score_mode_db()
             :wxDialog(0, 
                       -1, 
                       wxString(_T("ScoreMode")),
                       wxDefaultPosition,
                       wxSize(200,200),
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    my_sizer = new wxGridSizer(2,1,5,5);
    ON_DB_CREATE(myScoreMode,wxBooleanQuerySizer,"Score Mode",iabc::score_mode_on,wxCheckBox,"score_mode_on",reg::score_mode_on);
    ON_DB_CREATE(myPageBreakBetweenParts,wxBooleanQuerySizer,"Page Break (part mode)",iabc::page_break_between_parts,wxCheckBox,"page_break_between_parts",reg::page_break_between_parts);
    my_sizer->Add(5,5);
    ADD_BUTTONS();
}

void 
wx_score_mode_db::OnOK(wxCommandEvent& event)
{
    ON_DB_COMMIT(iabc::score_mode_on,wxCheckBox,"score_mode_on",reg::score_mode_on);
    ON_DB_COMMIT(iabc::page_break_between_parts,wxCheckBox,"page_break_between_parts",reg::page_break_between_parts);
    iabc::registry::get_instance().flush();
    the_view_parameters_changed = true;
    EndModal(wxID_OK);
}

void 
wx_score_mode_db::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void 
wx_score_mode_db::OnInitDialog(wxInitDialogEvent& event)
{
    ON_DB_SHOW(iabc::score_mode_on,wxCheckBox,"score_mode_on",reg::score_mode_on);
    ON_DB_SHOW(iabc::page_break_between_parts,wxCheckBox,"page_break_between_parts",reg::page_break_between_parts);
    my_sizer->Fit(this);
}

wx_window_media_pp::wx_window_media_pp()
             :wxDialog(0, 
                       -1, 
                       wxString(_T("Settings")),
                       wxDefaultPosition,
                       wxSize(200,200),
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)

{
    my_sizer = new wxGridSizer(10,2,5,5);
    ON_DB_CREATE(myPageHeight,wxTextQuerySizer,"Page Height",iabc::window_media_default_height,wxTextCtrl,"page_height",reg::page_height_value);
    ON_DB_CREATE(myPageWidth,wxTextQuerySizer,"Page Width",iabc::window_media_default_width,wxTextCtrl,"page_width",reg::page_width_value);
    ON_DB_CREATE(myXScale,wxTextQuerySizer,"X Scale",iabc::window_media_default_xscale,wxTextCtrl,"x_scale",reg::xscale_value);
    ON_DB_CREATE(myYScale,wxTextQuerySizer,"Y Scale",iabc::window_media_default_yscale,wxTextCtrl,"y_scale",reg::yscale_value);
    ON_DB_CREATE(myNoteScale,wxTextQuerySizer,"Note Scale",iabc::window_media_note_scale,wxTextCtrl,"note_scale",reg::window_media_note_scale);
    ON_DB_CREATE(myTitleSize,wxTextQuerySizer,"Title Size",iabc::window_media_title_size,wxTextCtrl,"title_size",reg::window_media_title_size);
    ON_DB_CREATE(myLeftMargin,wxTextQuerySizer,"Left Margin",iabc::window_media_left_margin,wxTextCtrl,"left_margin",reg::window_media_left_margin);
    ON_DB_CREATE(myRightMargin,wxTextQuerySizer,"Right Margin",iabc::window_media_right_margin,wxTextCtrl,"right_margin",reg::window_media_right_margin);    
    ON_DB_CREATE(myTopMargin,wxTextQuerySizer,"Top Margin",iabc::window_media_top_margin,wxTextCtrl,"top_margin",reg::window_media_top_margin);
    ON_DB_CREATE(myBottomMargin,wxTextQuerySizer,"Bottom Margin",iabc::window_media_bottom_margin,wxTextCtrl,"bottom_margin",reg::window_media_bottom_margin); 
    ON_DB_CREATE(myWordSize,wxTextQuerySizer,"Word Size",iabc::window_media_word_size,wxTextCtrl,"word_size",reg::window_media_word_size);
    ON_DB_CREATE(myCopyrightSize,wxTextQuerySizer,"Copyright Size",iabc::window_media_copyright_size,wxTextCtrl,"copyright_size",reg::window_media_copyright_size);
    ON_DB_CREATE(mySongwordSize,wxTextQuerySizer,"Songword Size",iabc::window_media_songword_size,wxTextCtrl,"songword_size",reg::window_media_songword_size);
    ON_DB_CREATE(myChordSize,wxTextQuerySizer,"Chord Size",iabc::window_media_chord_size,wxTextCtrl,"chord_size",reg::window_media_chord_size);
    ON_DB_CREATE(myChordBold,wxBooleanQuerySizer,"Bold Chords",iabc::window_media_chord_bold,wxCheckBox,"bold_chords_box",reg::window_media_chord_bold);
    ON_DB_CREATE(myChordItalic,wxBooleanQuerySizer,"Italic Chords",iabc::window_media_chord_italic,wxCheckBox,"italic_chords_box",reg::window_media_chord_italic);
    ON_DB_CREATE(myWordsItalic,wxBooleanQuerySizer,"Italic Words",iabc::window_media_songword_italic,wxCheckBox,"italic_songwords_box",reg::window_media_songword_italic);
    my_sizer->Add(5,5);
    ADD_BUTTONS();
}

void 
wx_window_media_pp::OnOK(wxCommandEvent& event)
{
    ON_DB_COMMIT(iabc::window_media_default_height,wxTextCtrl,"page_height",reg::page_height_value);
    ON_DB_COMMIT(iabc::window_media_default_width,wxTextCtrl,"page_width",reg::page_width_value);
    ON_DB_COMMIT(iabc::window_media_default_xscale,wxTextCtrl,"x_scale",reg::xscale_value);
    ON_DB_COMMIT(iabc::window_media_default_yscale,wxTextCtrl,"y_scale",reg::yscale_value);
    ON_DB_COMMIT(iabc::window_media_top_margin,wxTextCtrl,"top_margin",reg::window_media_top_margin);
    ON_DB_COMMIT(iabc::window_media_bottom_margin,wxTextCtrl,"bottom_margin",reg::window_media_bottom_margin);
    ON_DB_COMMIT(iabc::window_media_title_size,wxTextCtrl,"title_size",reg::window_media_title_size);
    ON_DB_COMMIT(iabc::window_media_left_margin,wxTextCtrl,"left_margin",reg::window_media_left_margin);
    ON_DB_COMMIT(iabc::window_media_right_margin,wxTextCtrl,"right_margin",reg::window_media_right_margin);
    ON_DB_COMMIT(iabc::window_media_note_scale,wxTextCtrl,"note_scale",reg::window_media_note_scale);
    ON_DB_COMMIT(iabc::window_media_copyright_size,wxTextCtrl,"copyright_size",reg::window_media_copyright_size);
    ON_DB_COMMIT(iabc::window_media_chord_size,wxTextCtrl,"chord_size",reg::window_media_chord_size);
    ON_DB_COMMIT(iabc::window_media_chord_bold,wxCheckBox,"bold_chords_box",reg::window_media_chord_bold);
    ON_DB_COMMIT(iabc::window_media_chord_italic,wxCheckBox,"italic_chords_box",reg::window_media_chord_italic);
    ON_DB_COMMIT(iabc::window_media_songword_size,wxTextCtrl,"songword_size",reg::window_media_songword_size);
    ON_DB_COMMIT(iabc::window_media_songword_italic,wxCheckBox,"italic_songwords_box",reg::window_media_songword_italic);
    ON_DB_COMMIT(iabc::window_media_word_size,wxTextCtrl,"word_size",reg::window_media_word_size);
    iabc::registry::get_instance().flush();
    the_view_parameters_changed = true;
    EndModal(wxID_OK);
}

void
wx_window_media_pp::OnInitDialog(wxInitDialogEvent& event)
{
    ON_DB_SHOW(iabc::window_media_default_height,wxTextCtrl,"page_height",reg::page_height_value);
    ON_DB_SHOW(iabc::window_media_default_width,wxTextCtrl,"page_width",reg::page_width_value);
    ON_DB_SHOW(iabc::window_media_default_xscale,wxTextCtrl,"x_scale",reg::xscale_value);
    ON_DB_SHOW(iabc::window_media_default_yscale,wxTextCtrl,"y_scale",reg::yscale_value);
    ON_DB_SHOW(iabc::window_media_top_margin,wxTextCtrl,"top_margin",reg::window_media_top_margin);
    ON_DB_SHOW(iabc::window_media_bottom_margin,wxTextCtrl,"bottom_margin",reg::window_media_bottom_margin);
    ON_DB_SHOW(iabc::window_media_title_size,wxTextCtrl,"title_size",reg::window_media_title_size);
    ON_DB_SHOW(iabc::window_media_left_margin,wxTextCtrl,"left_margin",reg::window_media_left_margin);
    ON_DB_SHOW(iabc::window_media_right_margin,wxTextCtrl,"right_margin",reg::window_media_right_margin);
    ON_DB_SHOW(iabc::window_media_note_scale,wxTextCtrl,"note_scale",reg::window_media_note_scale);
    ON_DB_SHOW(iabc::window_media_copyright_size,wxTextCtrl,"copyright_size",reg::window_media_copyright_size);
    ON_DB_SHOW(iabc::window_media_chord_size,wxTextCtrl,"chord_size",reg::window_media_chord_size);
    ON_DB_SHOW(iabc::window_media_chord_bold,wxCheckBox,"bold_chords_box",reg::window_media_chord_bold);
    ON_DB_SHOW(iabc::window_media_chord_italic,wxCheckBox,"italic_chords_box",reg::window_media_chord_italic);
    ON_DB_SHOW(iabc::window_media_songword_size,wxTextCtrl,"songword_size",reg::window_media_songword_size);
    ON_DB_SHOW(iabc::window_media_songword_italic,wxCheckBox,"italic_songwords_box",reg::window_media_songword_italic);
    ON_DB_SHOW(iabc::window_media_word_size,wxTextCtrl,"word_size",reg::window_media_word_size);
    my_sizer->Fit(this);
}

void 
wx_window_media_pp::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

wx_voice_options_dialog::wx_voice_options_dialog(
    iabc::map<int,bool>& the_voice_on_map,
    iabc::map<int,iabc::string>& the_voice_name_map):
    my_voice_name_map(the_voice_name_map),
    my_voice_on_map(the_voice_on_map),
    wxDialog(0, 
              -1, 
              wxString(_T("Settings")),
              wxDefaultPosition,
              wxSize(200,200),
              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    int num_rows = my_voice_on_map.get_size();
    int tmp_widgets = 0;
    my_sizer = new wxGridSizer(num_rows,3,5,5);
    iabc::map<int,bool>::iterator tmp_value_it = 
        my_voice_on_map.least();
    iabc::map<int,iabc::string>::iterator tmp_name_it = 
        my_voice_name_map.least();
    while (tmp_value_it && tmp_name_it)
    {
        bool tmp_value = (*tmp_value_it).value;
        iabc::string& tmp_name = (*tmp_name_it).value;            
        wxBooleanQuerySizer* tmp_sizer = new wxBooleanQuerySizer(this,
                                                                  tmp_name.access_char_array(),
                                                                  tmp_value,
                                                                  tmp_name.access_char_array());
        my_sizer->Add(*tmp_sizer,1,wxGROW|wxALL,5);
        tmp_value_it = tmp_value_it.get_item((*tmp_value_it).key,iabc::gt);
        tmp_name_it = tmp_name_it.get_item((*tmp_name_it).key,iabc::gt);
        ++tmp_widgets;
    }

    tmp_widgets = tmp_widgets % 3;
    while (tmp_widgets)
    {
        my_sizer->Add(5,5);
        --tmp_widgets;
    }
    ADD_BUTTONS();
}

wx_voice_options_dialog::~wx_voice_options_dialog()
{
}

void 
wx_voice_options_dialog::OnOK(wxCommandEvent& event)
{
    iabc::map<int,iabc::string>::iterator tmp_name_it = 
        my_voice_name_map.least();
    while (tmp_name_it)
    {
        iabc::string tmp_name = (*tmp_name_it).value;
        int tmp_key = (*tmp_name_it).key;
        wxString tmp_wx_string(B2U(tmp_name.access_char_array()));
        wxCheckBox* tmp_field = (wxCheckBox*)wxFindWindowByName(tmp_wx_string,this);
        if (tmp_field)
        {
            my_voice_on_map.add_pair(tmp_key,tmp_field->GetValue());
        }
        tmp_name_it = tmp_name_it.get_item((*tmp_name_it).key,iabc::gt);
    }
    EndModal(wxID_OK);
}
    
void 
wx_voice_options_dialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void 
wx_voice_options_dialog::OnInitDialog(wxInitDialogEvent& event)
{
    my_sizer->Fit(this);
}

void
wx_abc_options_dialog::OnInitDialog(wxInitDialogEvent& event)
{
    ON_DB_SHOW(iabc::allow_deprecated_voice_change,wxCheckBox,"v_no_brackets",reg::v_no_bracket_default);
    ON_DB_SHOW(iabc::allow_text_voice_change,wxCheckBox,"v_text_numbers",reg::allow_text_voice_change);
    ON_DB_SHOW(iabc::allow_barfly_midi_commands,wxCheckBox,"v_barfly_compatibility",reg::allow_barfly_midi_commands);
}

wx_abc_options_dialog::wx_abc_options_dialog()
             :wxDialog(0, 
                       -1, 
                       wxString(_T("Settings")),
                       wxDefaultPosition,
                       wxSize(),
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    my_sizer = new wxGridSizer(10,2,5,5);
    ON_DB_CREATE(my_no_brackets,wxBooleanQuerySizer,"Allow V: without brackets (barfly)",iabc::allow_deprecated_voice_change,wxCheckBox,"v_no_brackets",reg::v_no_bracket_default);
    ON_DB_CREATE(my_text_numbers,wxBooleanQuerySizer,"Allow Text Numbers",iabc::allow_text_voice_change,wxCheckBox,"v_text_numbers",reg::allow_text_voice_change);
    ON_DB_CREATE(my_barfly_compatibilty,wxBooleanQuerySizer,"Allow Barfly program field",iabc::allow_barfly_midi_commands,wxCheckBox,"v_barfly_compatibility",reg::allow_barfly_midi_commands);
    my_sizer->Add(5,5);
    ADD_BUTTONS();
}

void
wx_abc_options_dialog::OnOK(wxCommandEvent& event)
{
    ON_DB_COMMIT(iabc::allow_deprecated_voice_change,wxCheckBox,"v_no_brackets",reg::v_no_bracket_default);
    ON_DB_COMMIT(iabc::allow_text_voice_change,wxCheckBox,"v_text_numbers",reg::allow_text_voice_change);
    ON_DB_COMMIT(iabc::allow_barfly_midi_commands,wxCheckBox,"v_barfly_compatibility",reg::allow_barfly_midi_commands);
    iabc::registry::get_instance().flush();
    EndModal(wxID_OK);
}

void 
wx_abc_options_dialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

wx_midi_options_dialog::wx_midi_options_dialog()
             :wxDialog(0, 
                       -1, 
                       wxString(_T("Settings")),
                       wxDefaultPosition,
                       wxSize(),
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    my_sizer = new wxGridSizer(10,2,5,5);
    ON_DB_CREATE(my_midi_directory,wxTextQuerySizer,"Midi Directory",iabc::midi_output_directory,wxTextCtrl,"midi_directory",reg::midi_output_directory);
    wxButton* tmpMidiDirButton = new wxButton(this,ID_CHOOSE_DIR_BUTTON,_T("..."));
    my_sizer->Add(tmpMidiDirButton,0,wxALIGN_CENTER,10);
    ON_DB_CREATE(my_midi_player,wxTextQuerySizer,"Midi Player",iabc::midi_player_name,wxTextCtrl,"midi_player",reg::midi_player_name);
    wxButton* tmpMidiPlayerButton = new wxButton(this,ID_CHOOSE_PLAYER_BUTTON,_T("..."));
    my_sizer->Add(tmpMidiPlayerButton,0,wxALIGN_CENTER,10);
    ON_DB_CREATE(my_midi_player_command,wxTextQuerySizer,"Midi Player Command",iabc::midi_player_command,wxTextCtrl,"midi_player_command",reg::midi_player_command);
    my_sizer->Add(5,5);
    ADD_BUTTONS();
    }
void
wx_midi_options_dialog::OnInitDialog(wxInitDialogEvent& event)
{
    ON_DB_SHOW(iabc::midi_output_directory,wxTextCtrl,"midi_directory",reg::midi_output_directory);
    ON_DB_SHOW(iabc::midi_player_name,wxTextCtrl,"midi_player",reg::midi_player_name);
    ON_DB_SHOW(iabc::midi_player_command,wxTextCtrl,"midi_player_command",reg::midi_player_command);
}

void
wx_midi_options_dialog::OnChooseMidiDir(wxCommandEvent& event)
{
    wxDirDialog tmpDialog(this);
    tmpDialog.SetMessage(_T("MIDI Output Directory"));
    wxTextCtrl* tmp_field = (wxTextCtrl*)wxFindWindowByName(_T("midi_directory"),this);
    tmpDialog.SetPath(tmp_field->GetValue());
    tmpDialog.ShowModal();
    tmp_field->SetValue(tmpDialog.GetPath());
}

void
wx_midi_options_dialog::OnChooseMidiPlayer(wxCommandEvent& event)
{
    wxFileDialog tmpDialog(this);
    tmpDialog.SetMessage((wxChar*)"MIDI Output Directory");
    wxTextCtrl* tmp_field = (wxTextCtrl*)wxFindWindowByName((wxChar*)"midi_player",this);
    tmpDialog.SetPath(tmp_field->GetValue());
    tmpDialog.ShowModal();
    tmp_field->SetValue(tmpDialog.GetPath());
}

void
wx_midi_options_dialog::OnOK(wxCommandEvent& event)
{
    ON_DB_COMMIT(iabc::midi_output_directory,wxTextCtrl,"midi_directory",reg::midi_output_directory);
    ON_DB_COMMIT(iabc::midi_player_name,wxTextCtrl,"midi_player",reg::midi_player_name);
    ON_DB_COMMIT(iabc::midi_player_command,wxTextCtrl,"midi_player_command",reg::midi_player_command);

    EndModal(wxID_OK);
    iabc::registry::get_instance().flush();
}

void
wx_midi_options_dialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

wx_transpose_dialog::wx_transpose_dialog()
    :wxDialog(0, 
          -1, 
          wxString(_T("Transpose")),
          wxDefaultPosition,
          wxSize(),
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)

{
    my_sizer = new wxGridSizer(2,2,5,5);
    ON_DB_CREATE(mySpinSizer,wxIntSpinSizer,"Steps",iabc::transpose_steps,wxSpinCtrl,"transpose_steps",reg::transpose_steps);
    ON_DB_CREATE(my_diatonic,wxBooleanQuerySizer,"Diatonic",iabc::transpose_diatonic,wxCheckBox,"transpose_diatonic",reg::transpose_diatonic);
    ON_DB_CREATE(my_refresh,wxBooleanQuerySizer,"Refresh music view",iabc::transpose_refresh,wxCheckBox,"transpose_refresh",reg::transpose_refresh);

    // my_sizer->Add(*mySpinSizer,1,wxGROW|wxALL,5); 
    my_sizer->Add(5,5);
    ADD_BUTTONS();
}
void wx_transpose_dialog::OnOK(wxCommandEvent& event)
{
    ON_DB_COMMIT(iabc::transpose_steps,wxSpinCtrl,"transpose_steps",reg::transpose_steps);
    ON_DB_COMMIT(iabc::transpose_diatonic,wxCheckBox,"transpose_diatonic",reg::transpose_diatonic);
    ON_DB_COMMIT(iabc::transpose_refresh,wxCheckBox,"transpose_refresh",reg::transpose_refresh);
    EndModal(wxID_OK);
    iabc::registry::get_instance().flush();
}

void 
wx_transpose_dialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void 
wx_transpose_dialog::OnInitDialog(wxInitDialogEvent& event)
{
    ON_DB_SHOW(iabc::transpose_steps,wxSpinCtrl,"transpose_steps",reg::transpose_steps);
    ON_DB_SHOW(iabc::transpose_diatonic,wxCheckBox,"transpose_diatonic",reg::transpose_diatonic);
    ON_DB_SHOW(iabc::transpose_refresh,wxCheckBox,"transpose_refresh",reg::transpose_refresh);
}

namespace iabc
{
class percentage_meter_creator
{
public:
    friend class draw_command<percentage_meter_creator,int>;
    percentage_meter_creator();
    void add_ref(){};
    void remove_ref(){};
    void draw_self(window& w,int);
    wx_percentage_meter* get_meter(){return my_meter;};
    wx_percentage_meter* my_meter;
};

class percentage_meter_destroyer
{
public:
    friend class draw_command<percentage_meter_destroyer,int>;
    percentage_meter_destroyer(percentage_meter& the_victim);
    void add_ref(){};
    void remove_ref(){};
    void draw_self(window& w,percentage_meter* the_victim);
};

typedef draw_command<percentage_meter_creator,int> pmc_dispatcher;
typedef draw_command<percentage_meter_destroyer,percentage_meter*> pmd_dispatcher;

percentage_meter_creator::percentage_meter_creator()
{
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    int tmp_ignore = 0;
    pmc_dispatcher::dispatch_draw_command(tmp_window,
		tmp_ignore,*this,true);
}

void 
percentage_meter_creator::draw_self(window& w,int)
{
	wxDialog* tmp_dialog = global_progress_dialog;
	wxButton* tmp_button =  (wxButton *)wxFindWindowByName(_T("cancel_button"), tmp_dialog);
	wxStaticText* tmp_caption = (wxStaticText*)wxFindWindowByName(_T("caption"),tmp_dialog);
	wxStaticText* tmp_percent_label = (wxStaticText*)wxFindWindowByName(_T("percent_label"),tmp_dialog);
	wxGauge* tmp_gauge = (wxGauge*)wxFindWindowByName(_T("gauge2"),tmp_dialog);
	my_meter = new wx_percentage_meter(
		*tmp_button,*tmp_caption,*tmp_percent_label,*tmp_gauge,*tmp_dialog);
}

percentage_meter_destroyer::percentage_meter_destroyer(percentage_meter& the_victim)
{
    window& tmp_window = caster<window>::get_resource(*(wd_command::get_default_canvas()));
    int tmp_ignore = 0;
    pmd_dispatcher::dispatch_draw_command(tmp_window,
		&the_victim,*this,true);
}

void
percentage_meter_destroyer::draw_self(window& w,percentage_meter* the_meter)
{
	wx_percentage_meter* tmp_meter = dynamic_cast<wx_percentage_meter*>(the_meter);
    delete tmp_meter;
}

percentage_meter* 
percentage_meter::create()
{
    percentage_meter_creator tmp_creator;
	return tmp_creator.get_meter();
}

void 
percentage_meter::destroy(percentage_meter& the_meter)
{
    percentage_meter_destroyer tmp_destroyer(the_meter);

}
}



