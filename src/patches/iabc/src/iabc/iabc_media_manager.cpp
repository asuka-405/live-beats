/*
 * iabc_media_manager.cpp - Manages an iabc_window and starts the parser (iabc_media).
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

#include "iabc/iabc_media_manager.h"
#include "iabc/winres.h"
#include "iabc/figure.h"
#include "iabc/wd_temp.cpp"
#include "iabc/staff.h"
#include "iabc/note_figure.h"
#include "iabc/array.cpp"
#include "iabc/map.cpp"
#include "iabc/list.cpp"
#include "iabc/preprocess.h"
#include "iabc/wx_dispatch.h"
#include "iabc/registry.h"
#include "iabc/midi_media.h"
#include "wx/listctrl.h"
#include "iabc/registry_defaults.h"
#include "iabc/unicode.h"

#define debug_print(c)
// extern void debug_print(const char*);
namespace iabc
{
;
wx_tune_list_control::wx_tune_list_control(const registry_entry& the_entry,
                                           wx_tree& the_box):
wx_list_box_control(the_entry,the_box),my_value(0)
{
}

void
wx_tune_list_control::retreive_value_from_box()
{
    int tmp_index = my_list_box->GetSelection();
    int tmp_int_value = 0;
    map<wxTreeItemId,int>::iterator tmp_it =
        my_id2index.get_item(tmp_index);
    if (tmp_it)
    {
        tmp_int_value = (*tmp_it).value;
    }
    my_value = (tmp_int_value);
    global_doc_event_log.log_event(doc_event_data::retreive_value_from_box,
                                   my_value,
                                   tmp_index,(unsigned long)this);
}

void
wx_tune_list_control::select(int the_index)
{
    global_doc_event_log.log_event(doc_event_data::select);
    wxTreeItemId tmp_id = (wxTreeItemId)(long)0;
    map<int,wxTreeItemId>::iterator tmp_it =
        my_index2id.get_item(the_index);
    if (tmp_it)
    {
        tmp_id = (*tmp_it).value;
        my_list_box->SelectItem(tmp_id);
    }
}

void
wx_tune_list_control::remove_all_tunes()
{
    my_value = 0;
    my_id2index.clear();
    my_index2id.clear();
    my_tunes.clear();
    is_dirty = true;
    global_doc_event_log.log_event(doc_event_data::remove_all_tunes);
}

void
wx_tune_list_control::populate_list_box()
{
    int i;
    global_doc_event_log.log_event(doc_event_data::populate_list_box,is_dirty);
    if (is_dirty)
    {
        int tmp_index = my_value;
        my_list_box->DeleteChildren(my_list_box->GetRootItem());
        for (i = 0;(is_dirty == true) && (i < get_size());++i)
        {
            tune& tmp_tune = (*this)[i];
            string tmp_string = (tmp_tune).title;
            tmp_string.chop();
            int tmp_id =
                my_list_box->AppendItem(my_list_box->GetRootItem(),B2U(tmp_string.access_char_array()));
            my_index2id.add_pair(i,tmp_id);
            my_id2index.add_pair(tmp_id,i);
        }
        select(tmp_index);
        is_dirty = false;
    }
}

iabc_media_manager::iabc_media_manager(wxWindow& the_parent,
                                       wx_tree& the_tune_list,
                                       wx_view_canvas& the_display_window,
                                       wx_text_buffer& the_text_buffer):
my_exit_semaphore(0),
my_parent_window(&the_parent),
my_printing_window(0),
my_print_wxWindow(0),
my_can_restart(false),
my_window_media(0),
my_source(0),
    my_last_redraw_time(0),
    my_view_canvas(&the_display_window),my_abc_window(0)
    ,my_list_box(&the_tune_list),
    my_last_printout(0),my_last_printer(0),my_text_buffer(0),
    my_midi_media(0),my_print_media(0),my_print_pages(0),
    my_last_text_position(0),my_is_changing_tunes(true)
,my_last_text_y(0)
,my_wx_text_buffer(the_text_buffer)
{
    global_doc_event_log.log_event(doc_event_data::ctor);
    my_list_box->add_ref();

    registry_entry tmp_tune_entry =
        registry_entry(reg::last_tune.as_key(),"1",true);

    // This does not appear on a page, but it is passed to the
    // iabc_media so we include it.
    my_tune_list = new
        wx_tune_list_control(tmp_tune_entry,
                             *my_list_box);
    my_tune_list->add_ref();
}

iabc_media_manager::~iabc_media_manager()
{
    global_doc_event_log.log_event(doc_event_data::dtor);
    close();
    if (my_list_box)
    {
        my_list_box->set_selection_handler(selection_handler());
        my_list_box->remove_ref();
        my_list_box = 0;
    }
    my_exit_semaphore.seize();
    if (my_source)
    {
        delete my_source;
    }
    if (my_text_buffer)
    {
        delete my_text_buffer;
    }
    delete my_tune_list;
    // Note: no need to destroy my_view_canvas,
    // close does that for us.
    if (my_midi_media)
    {
        delete my_midi_media;
    }
    cleanup_print_stuff();
}

void
iabc_media_manager::cleanup_print_stuff()
{
    if (my_print_media)
    {
        delete my_print_media;
        my_print_media = 0;
    }
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
    if (my_print_pages)
    {
        delete (my_print_pages);
        my_print_pages = 0;
    }
    if (my_printing_window != 0)
    {
        my_printing_window->res_remove_ref();
        my_printing_window = 0;
    }
    if (my_print_wxWindow != 0)
    {
        my_print_wxWindow->Destroy();
        my_print_wxWindow = 0;
    }
}

void
iabc_media_manager::redraw(const rect& the_rect)
{
    global_doc_event_log.log_event(doc_event_data::redraw,
                                   the_rect.ul.x,
                                   the_rect.ul.y,
                                   the_rect.lr.x,
                                   the_rect.lr.x);
    my_pages->refresh(the_rect);

    // After redrawing the document, set the focus back in the 
    // text window.
    my_wx_text_buffer.get_control()->SetFocus();
}

void
iabc_media_manager::rerender_for_print()
{
    scale tmp_print_scale(1.0,1.0);
    global_doc_event_log.log_event(doc_event_data::rerender_for_print);
    
    my_print_pages = new paged_window(point(0,0),*my_printing_window);
    my_print_media = new window_media(*my_print_pages,
                                *my_printing_window,my_page_size,tmp_print_scale);
    // my_print_source->parse(*my_print_media);
    my_source->parse(*my_print_media);
}

void
iabc_media_manager::print_page(int the_page)
{
    global_doc_event_log.log_event(doc_event_data::rerender_for_print,the_page);
    if (the_page == 1)
        rerender_for_print();
    if (my_print_pages)
    {
        my_print_pages->print_page(the_page);
    }
}

void
iabc_media_manager::print()
{
    global_doc_event_log.log_event(doc_event_data::print_event,
                                   my_can_restart);
    if (my_can_restart == true)
    {
        cleanup_print_stuff();

        page_settings tmp_settings = my_pages->get_page(1).get_settings();
        wxPrintData tmp_data;
        tmp_data.SetNoCopies(1);
        tmp_data.SetCollate(false);
        if (tmp_settings.width > tmp_settings.height)
        {
            tmp_data.SetOrientation(wxLANDSCAPE);
        }
        else
        {
            tmp_data.SetOrientation(wxPORTRAIT);
        }

        if (tmp_settings.width < 215.0 / 25.4)
        {
            tmp_data.SetPaperId(wxPAPER_A4);
        }
        else
        {
            tmp_data.SetPaperId(wxPAPER_LETTER);
        }
        my_print_wxWindow =
            new wx_print_canvas(my_parent_window);
        my_printing_window =
            new wx_printing_window("printing_window",*my_print_wxWindow);
        my_printing_window->res_add_ref();
        my_last_printout = new wx_printout(*this,*my_printing_window);
        wxPrintDialogData printDialogData(tmp_data);
        my_last_printer = new wxPrinter(&printDialogData);
        my_printing_window->set_printout(*my_last_printout);
        my_last_printer->Print(my_print_wxWindow,my_last_printout);
        my_printing_window->end_printing();
        cleanup_print_stuff();
    }
}

bool
iabc_media_manager::has_page(int the_page)
{
    bool tmp_rv = false;
    // wxWindows printing is kind of broken.  We need some page info
    // here, but the first time this is called we have no DC so how
    // can we know page info?  Get around this by just returning true if
    // this is the first page.  We render right before we print.
    if (my_pages)
    {
        tmp_rv = (my_pages->get_number_pages() >= the_page);
    }
    else
        tmp_rv = (the_page == 1);
    
    global_doc_event_log.log_event(doc_event_data::has_page,the_page,tmp_rv);
    return tmp_rv;
}

void
iabc_media_manager::rescale(const scale& the_scale)
{
    global_doc_event_log.log_event(doc_event_data::rescale);
    if ((my_scale.x != the_scale.x) ||
        (my_scale.y != the_scale.y))
    {
        if ((my_can_restart == true) &&
            (my_pages))
        {
            choose_tune();
        }
    }
}

void
iabc_media_manager::refresh_current_tune(long the_last_cursor)
{
    // If the text buffer has been modified, we re-parse the tune
    global_doc_event_log.log_event(doc_event_data::refresh_current_tune,the_last_cursor);
    my_last_text_position = the_last_cursor;
    if (my_text_buffer->get_last_modify_time() > my_last_redraw_time)
    {
        my_is_changing_tunes = false;
        choose_tune();
        my_list_box->set_selection_handler(selection_handler_dx<iabc_media_manager>::get_handler(*this));
    }
    // Else we just redraw the whole window, and also center around the current
    // part of the music.
    else if (my_abc_window)
    {
        int tmp_screen_line = get_screen_line_from_text_line(my_last_text_y);
        // int tmp_screen_line = get_screen_line_from_text_line(my_last_text_position);
        my_abc_window->set_center_of_y_scroll(tmp_screen_line);
    }
}

void
iabc_media_manager::choose_tune()
{
    global_doc_event_log.log_event(doc_event_data::choose_tune,my_is_parsing);
    if (my_is_parsing == false)
    {
        // Close the old document and open the new one.
        close();
        my_abc_window = new wx_scrolling_window("Foo",*my_view_canvas);
        my_view_canvas->set_window(my_abc_window);
        my_abc_window->res_add_ref();
        my_abc_window->add_document(this);

        // Index the file if there has been a change in the
        // underlying buffer or file since last parsing.
        if (my_last_redraw_time < my_text_buffer->get_last_modify_time())
        {
            my_text_buffer->index_file();
        }

        // Set it up so that we get notified when there is a change in selection.
        my_list_box->set_selection_handler(
            selection_handler_dx<iabc_media_manager>::get_handler(*this));

        // Start the parsing thread.
        restart();
    }
}

void
iabc_media_manager::handle_selection(selection_handler_if& the_handler)
{
    my_is_changing_tunes = true;
    global_doc_event_log.log_event(doc_event_data::handle_selection);

    choose_tune();
    my_list_box->set_selection_handler(selection_handler_dx<iabc_media_manager>::get_handler(*this));
}

void
iabc_media_manager::setup_pages()
{
    global_doc_event_log.log_event(doc_event_data::setup_pages);
    registry_entry tmp_default_width =
        reg::get_default(reg::page_width_value);

    registry_entry tmp_default_height =
        reg::get_default(reg::page_height_value);

    registry_entry tmp_default_xscale =
        reg::get_default(reg::xscale_value);

    registry_entry tmp_default_yscale =
        reg::get_default(reg::yscale_value);

    // string tmp_ys = my_pp->get_string_from_resource(pp_control::pp_y_scale);
    string tmp_ys = tmp_default_yscale.get_value();
    my_scale.y = tmp_ys.as_double();
    if (my_scale.y <= 0.0)
    {
        my_scale.y = 1.0;
    }

    tmp_ys = tmp_default_xscale.get_value();
    my_scale.x = tmp_ys.as_double();
    if (my_scale.x <= 0.0)
    {
        my_scale.x = 1.0;
    }

    tmp_ys = tmp_default_height.get_value();
    my_page_size.y = tmp_ys.as_double();

    tmp_ys = tmp_default_width.get_value();
    my_page_size.x = tmp_ys.as_double();

    if (my_abc_window)
    {
        my_pages = new paged_window(point(0,0),(*my_abc_window));
        page_settings tmp_settings;
        tmp_settings.width = my_page_size.x * my_scale.x;
        tmp_settings.height = my_page_size.y * my_scale.y;
        tmp_settings.top_margin = ((double)tmp_settings.top_margin * my_scale.y);
        tmp_settings.bottom_margin = ((double)tmp_settings.bottom_margin * my_scale.y);
        tmp_settings.left_margin = ((double)tmp_settings.left_margin * my_scale.x);
        tmp_settings.right_margin = ((double)tmp_settings.left_margin * my_scale.x);

        // Update the computed stuff.
        tmp_settings.update_settings();

        my_pages->change_settings(1,tmp_settings);
    }
}

void
iabc_media_manager::click(const point &the_point)
{
    int tmp_text_line =
        get_text_line_from_screen_line(the_point.y);
    my_last_text_position = tmp_text_line;
    my_last_text_y = tmp_text_line;
    // refresh_current_tune(tmp_text_line);
    wxTextCtrl* tmp_control = my_wx_text_buffer.get_control();
    long tmp_pos = tmp_control->XYToPosition(0,tmp_text_line - 1);
    tmp_control->SetInsertionPoint(tmp_pos);
    tmp_control->ShowPosition(tmp_pos);
    tmp_control->SetFocus();
}

score_point
iabc_media_manager::get_score_point_from_screen_position(int the_screen_line)
{
    score_point tmp_rv;
    score_point tmp_gt_value;
    score_point tmp_lteq_value;
    int tmp_gt_key = 0;
    int tmp_lteq_key = 0;
    int tmp_actual_key = 0;
    if ((my_source) &&
        (my_is_parsing == false) &&
        (my_window_media))
    {
        map<int,score_point> tmp_score_map =
            my_window_media->get_lines_to_score_points();

        // We search twice, once for the next music entry and
        // once for the previous music entry.  Whichever is closest
        // to the spot that was clicked on is the one we take.
        map<int,score_point>::iterator tmp_gt_it =
            tmp_score_map.get_item(the_screen_line,gteq);
        map<int,score_point>::iterator tmp_lteq_it =
            tmp_score_map.get_item(the_screen_line,lt);
        if (tmp_gt_it)
        {
            tmp_gt_key = (*tmp_gt_it).key;
            tmp_gt_value = (*tmp_gt_it).value;
        }
        if (tmp_lteq_it)
        {
            tmp_lteq_key = (*tmp_lteq_it).key;
            tmp_lteq_value = (*tmp_lteq_it).value;
        }

        // If there was a next and a previous entry found,
        // find the closest to the click
        if ((tmp_lteq_it) && (tmp_gt_it))
        {
            if ((the_screen_line - tmp_lteq_key) <
                (tmp_gt_key - the_screen_line))
            {
                tmp_actual_key = tmp_lteq_key;
                tmp_rv = tmp_lteq_value;
            }
            else
            {
                tmp_actual_key = tmp_gt_key;
                tmp_rv = tmp_gt_value;
            }
        }
        // Else, there was only one entry found, so use
        // that.
        else if (tmp_lteq_it)
        {
            tmp_rv = tmp_lteq_value;
            tmp_actual_key = tmp_lteq_key;
        }
        else if (tmp_gt_it)
        {
            tmp_rv = tmp_gt_value;
            tmp_actual_key = tmp_gt_key;
        }
    }
    global_cursor_event_log.log_event(cursor_event_data::click_gui_window,
                                      the_screen_line,
                                      tmp_actual_key);
    return tmp_rv;
}

score_point
iabc_media_manager::get_score_point_from_text_position(int the_text_line)
{
    score_point tmp_text_count;
    if ((my_source) &&
        (my_is_parsing == false) &&
        (my_window_media))
    {
        map<int,score_point> tmp_text_map =
            my_source->get_lines_to_score_points();
        map<int,score_point>::iterator tmp_text_it =
            tmp_text_map.get_item(the_text_line,lteq);
        if (tmp_text_it)
        {
            tmp_text_count = (*tmp_text_it).value;
        }
    }
    return tmp_text_count;
}

string
iabc_media_manager::transpose_selection(int the_start,int the_stop,const string& the_selection,int the_steps,bool the_diatonic)
{
    string tmp_rv;
    if ((my_source) &&
        (my_is_parsing == false) &&
        (my_window_media))
    {
        score_point tmp_text_count = get_score_point_from_text_position(the_start);
        voice_info tmp_info = my_source->get_voice_info(tmp_text_count);
        pitch tmp_key = tmp_info.my_key;
        int tmp_octave_offset = tmp_info.my_octave;
        parser_state& the_parser = parser_state::get_instance();
        pitch_rule tmp_rule;
        int i = 0;
        while (i < the_selection.length())
        {
            // Make sure that the parser has the current key, etc.
            the_parser.get_current_voice_info().my_voice_info = tmp_info;
            if (tmp_rule.match(the_selection,i))
            {
                pitch tmp_pitch = tmp_rule.get_value();
                if (the_diatonic == false)
                {
                    tmp_pitch = tmp_pitch.transpose(tmp_key,the_steps);
                }
                else
                {
                    tmp_pitch = tmp_pitch.transpose_diatonic(tmp_key,the_steps);
                }
                // If this voice has a value for the 'octave' voice_info, the printed
                // value should not reflect this.  So subtrace any octave offset due to
                // the voice_info octave field.
                tmp_pitch = pitch(tmp_pitch.get_letter(),tmp_pitch.get_accidental(),
                                  tmp_pitch.get_octave() - tmp_octave_offset);
                tmp_rv += tmp_pitch.abc_string(tmp_key);
            }
            else
            {
                tmp_rv += the_selection[i];
                ++i;
            }
            tmp_text_count = get_score_point_from_text_position(the_start + i);
            tmp_info = my_source->get_voice_info(tmp_text_count);
            tmp_key = tmp_info.my_key;
            tmp_rule.reset();
        }
    }
    return tmp_rv;
}

int
iabc_media_manager::get_text_position_from_score_point(const score_point& the_point)
{
    int tmp_rv = 0;
    map<score_point,int> tmp_line_map =
        my_source->get_score_points_to_lines();
    map<score_point,int>::iterator tmp_it =
        tmp_line_map.get_item(the_point,lteq);
    if (tmp_it)
    {
        tmp_rv = (*tmp_it).value;
    }
    return tmp_rv;
}

int
iabc_media_manager::get_text_line_from_screen_line(int the_screen_line)
{
    int tmp_rv = 0;
    score_point tmp_point = get_score_point_from_screen_position(the_screen_line);
    map<score_point,int> tmp_line_map = my_source->get_score_points_to_lines();
    map<score_point,int>::iterator tmp_it =
        tmp_line_map.get_item(tmp_point,gteq);
    if (tmp_it)
    {
        tmp_rv = (*tmp_it).value;
    }
    else
    {
        // If we can't find anything less than the current position, like
        // if the user clicks near the top, find something greater.
        tmp_it =
            tmp_line_map.get_item(tmp_point,lt);
        if (tmp_it)
        {
            tmp_rv = (*tmp_it).value;
        }
    }

    global_cursor_event_log.log_event(cursor_event_data::score_to_text,
                                      tmp_rv,
                                      tmp_point.measure,
                                      tmp_point.voice,
                                      LOG_FRACTION(tmp_point.beat));
    return tmp_rv;
}

int
iabc_media_manager::get_screen_line_from_text_line(int the_text_line)
{
    int tmp_screen_line = 0;
    score_point tmp_text_count = get_score_point_from_text_position(the_text_line);
    map<score_point,int> tmp_window_map =
        my_window_media->get_score_points_to_lines();
    map<score_point,int>::iterator tmp_window_it =
        tmp_window_map.get_item(tmp_text_count,lteq);
    if (tmp_window_it)
    {
        tmp_screen_line = (*tmp_window_it).value;
    }
    return tmp_screen_line;
}

void
iabc_media_manager::handle_tick(int the_text_line)
{
    if ((my_source) &&
        (my_is_parsing == false) &&
        (my_window_media))
    {
        // If the user has moved the cursor, log an event since that will cause other things
        // to happen
        if (my_last_text_y != the_text_line)
        {
            global_cursor_event_log.log_event(cursor_event_data::change_text_buf,
                                              the_text_line,my_last_text_y,
                                              my_last_text_position);
        }

        // Determine the line in the GUI music that relates to the line in the 
        // ABC text.
        my_last_text_y = the_text_line;
        int tmp_screen_line = get_screen_line_from_text_line(the_text_line);

        // If the text buffer has changed, let the parser know about it.
        if (my_text_buffer->get_last_modify_time() > my_last_redraw_time)
        {
            my_source->set_modified();
        }

        // If there's no change, avoid flicker and don't redraw
        if (my_last_gui_cursor_point.y != tmp_screen_line)
        {
            // We draw a little cursor pointer thingy to indicate where in the GUI music the text
            // relates to.  Update the position of that if either the user clicks on the GUI 
            // window or the text buffer line changes.
            global_cursor_event_log.log_event(cursor_event_data::change_cursor_pos,
                                              tmp_screen_line,my_last_gui_cursor_point.y);

            page tmp_page = my_pages->get_page(my_pages->get_page_number(my_last_gui_cursor_point));
            tmp_page.remove_text_figures(my_last_gui_cursor_point);

            point tmp_old_point = my_last_gui_cursor_point;
            my_last_gui_cursor_point = point(10,tmp_screen_line);
            tmp_page = my_pages->get_page(my_pages->get_page_number(my_last_gui_cursor_point));
            text_figure* tmp_text = new text_figure(
                font::fixed,17,
                *my_abc_window);
            tmp_text->add_ref();
            tmp_text->set_string(string(POINTER_CHAR));
            tmp_page.add_text_figure(my_last_gui_cursor_point,*tmp_text);
            rect tmp_new_rect = tmp_text->get_current_rect();
            tmp_text->remove_ref();

            // Now update the screen picture.
            tmp_page.redraw(tmp_new_rect.offset(my_last_gui_cursor_point));
            tmp_page.redraw(tmp_new_rect.offset(tmp_old_point));

			// Now set the focus back on the text window
			my_wx_text_buffer.get_control()->SetFocus();
        }
    }
}

void 
iabc_media_manager::set_voice_on_map(map<int,bool>& the_map)
{
    my_voice_on_map = the_map;
}

void
iabc_media_manager::play(const string& the_file,long the_last_cursor,const score_point& where_to_start_from)
{
    global_doc_event_log.log_event(doc_event_data::play);
    if ((my_source) &&
        (my_can_restart))
    {
        if (my_midi_media)
        {
            delete my_midi_media;
            my_midi_media = 0;
        }
        my_midi_media =
            new midi_media(the_file,where_to_start_from,this);

        // This creates the midi file and starts the player
        my_last_text_position = the_last_cursor;
        my_source->parse(*my_midi_media);
        my_text_buffer->setpos(my_last_text_position);
    }
}

void
iabc_media_manager::open(const string& the_file)
{
    global_doc_event_log.log_event(doc_event_data::open_event);
    close();

    // All the drawing routines interact with a generic window object, which
    // proxies to the wxWindow instance.  Create that with the actual wxWindow
    // as a parameter.
    my_abc_window = new wx_scrolling_window("Foo",*my_view_canvas);

    // The above line must come before this one, as this one expects
    // the global dispatcher to be there for it.
    my_text_buffer = new preprocess(the_file);

    // The iabc window and the specialized wxWindow that we use need to know
    // about each other, which is kind of ugly but that's the only way we can
    // handle things like scrolling.
    my_view_canvas->set_window(my_abc_window);
    my_abc_window->res_add_ref();

    // Now we can create the parser.
    my_tune_list->select(my_tune_list->get_choice());
    my_source = new iabc_media(*my_tune_list,*my_text_buffer);

    // Set it up so that we get notified when there is a change in selection.
    my_list_box->set_selection_handler(
        selection_handler_dx<iabc_media_manager>::get_handler(*this));

    // The window is the view, and the media source is the document.  Make sure they
    // know about each other so the source knows where to send the media.  NOTE:
    // as a side-effect this creates the window_media instance.
    my_abc_window->add_document(this);

    // And start the parsing thread.
    start();
}


void
iabc_media_manager::close()
{
    global_doc_event_log.log_event(doc_event_data::close_event);
    if (my_abc_window)
    {
        my_abc_window->shutdown();
        my_abc_window->res_remove_ref();
        my_abc_window = 0;
    }
    if (this->my_list_box)
    {
        my_list_box->set_selection_handler(selection_handler());
    }
}

void
iabc_media_manager::entry_point()
{
    my_is_parsing = true;
    long start_sel,end_sel;
    my_text_buffer->get_selection(start_sel,end_sel);
    global_doc_event_log.log_event(doc_event_data::entry_point,
                                   start_sel,end_sel);
    my_source->parse(*my_window_media);
    my_can_restart = true;
    my_is_parsing = false;

    // If we are changing tunes but the file is staying the same, scroll to the
    // start of the tune.
    if (my_is_changing_tunes == true)
    {
        my_last_text_position = my_source->get_start_line_of_current_tune();
        my_last_text_position = my_text_buffer->get_pos_from_line(my_last_text_position);
        my_text_buffer->buffer_setpos(my_last_text_position);
    }

    // If we are reloading the same tune, but the file has changed.
    if (my_is_changing_tunes == false)
    {
        // We used to set the cursor position here, now we just leave it alone.
        // Otherwise the selection seems to go away.  I think this is something 
        // that's changed since 2.4.2.
        int tmp_screen_line = get_screen_line_from_text_line(my_last_text_y);
        my_abc_window->set_center_of_y_scroll(tmp_screen_line);
        my_abc_window->refresh();
    }

    my_last_redraw_time = time(0);
}

void
iabc_media_manager::restart()
{
    global_doc_event_log.log_event(doc_event_data::restart,my_can_restart);
    if (my_can_restart == true)
    {
        my_can_restart = false;
        start();
    }
}

rect
iabc_media_manager::get_rect_from_page(int the_page)
{
    rect tmp_rv;
    global_doc_event_log.log_event(doc_event_data::get_rect_from_page);
    if (my_pages)
    {
        my_pages->get_page_rect(the_page);
    }
    return tmp_rv;
}

void
iabc_media_manager::attach_canvas(canvas &the_canvas)
{
    global_doc_event_log.log_event(doc_event_data::attach_canvas);
    setup_pages();
    my_window_media = new window_media(*my_pages,
                                *my_abc_window,my_page_size,my_scale,this);
}

void
iabc_media_manager::detach_canvas(canvas &the_canvas)
{
    global_doc_event_log.log_event(doc_event_data::detach_canvas);
    my_exit_semaphore.release();
    delete my_window_media;
    my_window_media = 0;
    if (my_pages)
    {
        delete my_pages;
        my_pages = 0;
    }
}
}

