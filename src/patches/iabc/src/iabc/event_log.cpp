/*
 * event_log.cpp - Log concurrent events for debugging and post-mortem.
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
#include "iabc/event_log.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include "windows.h"
#endif

namespace iabc
{
;
// These are the strings that print out in the log files to describe
// the various events and their parameters.
base_event_data::event_string_class
windows_event_data::the_event_strings[windows_event_data::windows_events_last] =
{
    /*redraw_window */  {"redraw_window   ","       ul_x","ul_y","lr_x","lr_y"},
    /*vscroll_window,*/ {"vscroll_window  ","my_position","my_delta","redraw",""},
    /*hscroll_window */ {"hscroll_window  ","my_position","my_delta","redraw",""},
    /*set_scroll_point*/{"set_scroll_point","my_position","my_delta","redraw",""},
    /*create_window,*/  {"create_window   ","      sizex","sizey","posx","posy"},
    /*destroy_window, */{"destroy_window  ","","","",""},
    /* draw_line, */    {"draw_line       ","points","","",""},
    /* draw_text, */    {"draw_text       ","     x_pos","y_pos","length",""},
    /* draw_rect, */    {"draw_rect       ","    x_start","y_start","x_end","y_end"},
    /* draw_poly, */    {"draw_poly       ","points","","",""},
    /* create_font, */  {"create_font       ","family","point_size","style",""},
    /* move_sash  */  {"move_sash       ","this","pct","width","pos"},
    /* set_sash  */  {"set_sash ","vertical?","pct","",""},
    /* add_point  */  {"add_point ","x","y","",""}
};

base_event_data::event_string_class
dispatch_event_data::the_event_strings[dispatch_event_data::last] =
{
    /*enter */  {"enter   ","","","",""},
    /* same_thread,*/ {"same_thread","","","",""},
    /* blocking */ {"blocking","","","",""},
    /*unblocked*/{"unblocked","","","",""},
    /*ctor*/  {"ctor","","","",""},
    /*dtor */{"dtor","","","",""},
};

base_event_data::event_string_class
doc_event_data::the_event_strings[doc_event_data::last] =
{
    /* retreive_value_from_box */ {"retreive..   ","value","wx_index","this",""},
    /* select */             {"select","","","",""},
    /* remove_all_tunes */   {"rmv_all_tn","","","",""},
    /* populate_list_box */  {"populate_list_box","is_dirty","","",""},
    /* ctor */               {"ctor","","","",""},
    /* dtor */               {"dtor","","","",""},
    /* redraw */             {"redraw","lrx","lry","ulx","uly"},
    /* rerender_for_print */ {"rerend_4_prt","","","",""},
    /* print_page */         {"prt_page","the_page","","",""},
    /* print_event */        {"prt_event","can_restart","","",""},
    /* has_page  */          {"has_page","the_page","","",""},
    /* rescale */            {"rescale","","","",""},
    /* refresh_current_tune */{"refresh_tune","the_last_tune","","",""},
    /* choose_tune */         {"choose_tune","parsing","","",""},
    /* handle_selection */    {"handle_sel","","","",""},
    /* setup_pages */         {"setup_pages","","","",""},
    /* play */                {"play","","","",""},
    /* open_event */          {"open","","","",""},
    /* close_event */         {"close","","","",""},
    /* entry_point */         {"entry_point","start_sel","end_sel","",""},
    /* restart */             {"restart","my_can_restart","","",""},
    /* get_rect_from_page */  {"get_rect","","","",""},
    /* attach_canvas */       {"attach_canvas","","","",""},
    /* detach_canvas */       {"detach_canvas","","","",""},
    /* select_list  */        {"select_list","this","my_data","",""},
    /* selection_ctor */      {"selection_ctor","this","my_data","",""},
    /* selection_ctorp1 */      {"selection_ctorp1","this","my_data","",""},
    /* selection_cctor */     {"selection_cctor","this","my_data","",""},
    /* selection_dtor */      {"selection_dtor","this","my_data","",""},
    /* selection_opeq */      {"selection_opeq","this","my_data","o","old_data"},
    /* file_modify */         {"file_modify","file_time","buffer_time","",""},
    /* file_time_reset */     {"file_tim_rst","file_time","","",""},
    /* underlying */     {"underlying","","","",""},
    /* scan_lines */     {"scan_lines","count","","",""}
};

base_event_data::event_string_class
event_event_data::the_event_strings[event_event_data::last] =
{
    /*add_handler */  {"add_handler","source","handler","",""},
    /* remove_handler*/ {"remove_handler","source","handler","",""},
    /* op_or */ {"or","this","next","",""},
    /* wait */ {"wait","this","","",""},
    /* add_handler_to_list */ {"add_handler_to_list","source","handler","",""},
    /* notify_handlers */ {"notify_handlers","source","","",""},
    /*populate_handler_list*/{"populate_handler_list","source","","",""},
    /*set_event_source*/{"set_event_source","source","handler","",""},
    /*handle_event*/  {"handle_event","handler","","",""},
    /*add_handler_to_chain */{"add_2_chain","handler","next","last",""},
    /* tear_down_chain */ {"add_handler_to_list","handler","next","last","source"},
    /*zero_all_refs*/{"zero_all_refs","handler","","",""},
    /*set_event_source*/{"pass_event_to_first","handler","last","",""},
    /*add_all_refs*/  {"add_all_refs","handler","","",""},
    /*remove_all_refs */{"remove_all_refs","handler","","",""}
};

base_event_data::event_string_class
figure_event_data::the_event_strings[figure_event_data::last] =
{
    /*staff_create */  {"staff_create","this","","",""},
    /*gclef_create,*/ {"gclef_create","this","","",""},
    /*fclef_create */ {"fclef_create","this","","",""},
    /*ohead_create*/{"ohead_create","this","","",""},
    /*chead_create*/  {"chead_create","this","","",""},
    /*ustem_create */{"ustem_create","this","","",""},
    /*dstem_create */{"dstem_create","this","","",""},
    /*sharp_create */{"sharp_create","this","","",""},
    /*remove_ref */{"remove_ref","this","count","",""},
    /*add_ref */{"add_ref","this","count","",""},
    /*render_poly */{"render_poly","this","","",""},
    /*calc_size */{"calc_size","this","","",""},
    /*calc_size1 */{"calc_size1","szx","szy","locx","locy"},
    /*rescale */{"rescale","this","szx","szy",""},
    /*draw_self */{"draw_self","this","x","y",""},
    /*get_size */{"get_size","this","","",""},
    /*dispatch_draw */{"disp_draw","this","x","y","block"}
};

parser_event_data::event_string_class
parser_event_data::the_event_strings[parser_event_data::last] =
{
    /*words_rule */  {"words_rule","","","",""},
    /*measure_rule */  {"measure_rule","","","",""},
    /*header_rule */  {"header_rule","","","",""},
    /* match_parse_unit*/ {"match_parse_unit","","","",""},
    /* compare_parse_unit */ {"compare_parse_unit","","","",""}
};

settings_event_data::event_string_class
settings_event_data::the_event_strings[settings_event_data::last] =
{
    /*set_program_dir_event */  {"program_dir","","","",""},
    /*set_docs_dir_event */  {"docs_dir","","","",""},
    /*set_temp_dir_event */  {"temp_dir","","","",""},
    /*set_home_dir_event*/ {"home_dir","","","",""},
    /* set_filename,  */   {"set_fn","","","",""},
    /* set_ini_file,  */   {"set_ini","","","",""},
    /* new_file_ptr */    {"new_abc","","","",""}
};

base_event_data::event_string_class
cursor_event_data::the_event_strings[cursor_event_data::last] =
{
    /*store_screen_point */  {"store_screen_point","screeny","measure","voice","beat"},
    /*store_screen_point */  {"store_text_point","texty","measure","voice","beat"},
    /*change_cursor_pos */  {"change_cursor_pos","new_screeny","old_screeny","",""},
    /*change_text_buf */  {"change_text_pos","new_texty","old_texty","",""},
    /*click_gui_window */ {"click_gui_window","click_y","nearest_y","",""},
    /*score_to_text */ {"score_to_text","text_y","measure","voice","beat"}
};

base_event_data::event_string_class
text_event_data::the_event_strings[text_event_data::last] =
{
    /*key_press */  {"key_press","key_code","shift","alt","control"},
    /*pop_undo */       {"pop_undo","key_code","buf_size","start_sel","modified"},
    /*cut */  {"cut","","","",""},
    /*paste */  {"paste","","","",""},
    /* copy, */ {"copy","","","",""},
    /*push_undo */  {"push_undo","key_code","buf_size","start_sel","is_modify"},
    /*clipboard_open */  {"clipboard_open","data_type","","",""}
};

base_event_data::event_string_class
midi_event_data::the_event_strings[midi_event_data::last] =
{
    /*play_note */  {"play_note","measure","voice","beatn","beatd"}
};

// The program needs to supply this function.  For iabc, this is
// in the derived threads class.  This is the only dependency outside
// of this file.
extern const char* get_current_thread_name();

base_event_log::base_event_log(int the_size,int the_num_params)
:my_size(the_size),
    my_num_params(the_num_params),
    my_index(0),my_wrapped(false)
{
}

int base_event_log::get_first_event_index()
{
    return (my_wrapped == false ? 0 : my_index);
}

int base_event_log::get_log_size()
{
    return (my_size);
}

#ifndef WIN32
typedef unsigned long long int __int64;
#endif

static __int64 static_ticks_per_second = 0;

// This is only for Linux on Intel.  For something
// else use clock() to get compilation.
#ifndef WIN32
__int64 os_ticks()
{
     unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}

 __int64 os_ticks_per_s()
{
     if (static_ticks_per_second == 0)
     {
         unsigned long long int tpu1 = os_ticks();
         struct timespec tv;
         tv.tv_sec = 1;
         tv.tv_nsec = 0;
         nanosleep(&tv,&tv);
         unsigned long long int tpu2 = os_ticks();
         unsigned long int tpu_delta = (unsigned long int)tpu1 - tpu2;
         static_ticks_per_second = tpu_delta;
     }
 }
#else
// This is used for Windows
__int64 os_ticks()
{
    __int64 li = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&li);
    return li;
}

 __int64 os_ticks_per_s()
 {
     if (static_ticks_per_second == 0)
     {
         __int64 li = 0;
         QueryPerformanceFrequency((LARGE_INTEGER*)&li);
         static_ticks_per_second = li;
     }
     return static_ticks_per_second;
 }
#endif


unsigned long
base_event_log::get_timestamp()
{
    __int64 li;
    __int64 s;
    __int64 ms;
    __int64 us;
    __int64 tps = (unsigned long)os_ticks_per_s();
    // If this machine understands the clock API, use it.
    // else fall back to clock.
    if (tps > 0)
    {
        li = os_ticks();
        s = li/tps;
        ms = ((li * 1000)/tps) % 1000;
        us = ((li * 1000000) / tps) % 1000;
        unsigned long tmp_rv =
            (unsigned long)(s * 1000000 + ms*1000 + us);
        return tmp_rv;
    }
    else
    {
        unsigned long tmp_ms = (unsigned long)clock();
        return tmp_ms;
    }
    return 0;
}

int base_event_log::get_last_event_index()
{
    // The last index if we wrapped is one less than then first one.  If
    // the index is 0 and we have wrapped, return the last index.  Otherwise,
    // return the current index-1.
    return (my_wrapped == false ? my_index :
            (my_index == 0 ? my_size - 1 :
                          my_index - 1));
}

template<typename EventType,typename EnumT,int log_size>
void
event_log_template<EventType,EnumT,log_size>::log_event(EnumT e,
               unsigned long ulp1,
               unsigned long ulp2,
               unsigned long ulp3,
               unsigned long ulp4)
{
    // Place the event data into the next event.
    the_event_array[my_index].my_data.timestamp = get_timestamp();
    the_event_array[my_index].my_event = e;
    the_event_array[my_index].my_data.param[0] = ulp1;
    the_event_array[my_index].my_data.param[1] = ulp2;
    the_event_array[my_index].my_data.param[2] = ulp3;
    the_event_array[my_index].my_data.param[3] = ulp4;
    the_event_array[my_index].my_data.thread_id = get_current_thread_name();

    // Update the index to the circular queue
    my_index++;
    if (my_index + 1 > my_size)
    {
        my_index = 0;
        my_wrapped = true;
    }
}

template<typename EventType,typename EnumT,int log_size,int byte_size>
void
char_log_template<EventType,EnumT,log_size,byte_size>::log_event(
    const char* the_buf,
    int the_buf_size,
    EnumT e,
               unsigned long ulp1,
               unsigned long ulp2,
               unsigned long ulp3,
               unsigned long ulp4)
{
    // Store the 'normal' event data.
    unsigned long tmp_timestamp = get_timestamp();
    the_event_array[my_index].my_data.timestamp = tmp_timestamp;
    the_event_array[my_index].my_event = e;
    the_event_array[my_index].my_data.param[0] = ulp1;
    the_event_array[my_index].my_data.param[1] = ulp2;
    the_event_array[my_index].my_data.param[2] = ulp3;
    the_event_array[my_index].my_data.param[3] = ulp4;
    the_event_array[my_index].my_data.thread_id = get_current_thread_name();

    // Now store the variable-sized part.
    // First see if we need to wrap the buffer for variable-sized data.
    if (the_buf_size + my_byte_array_index + sizeof(unsigned long) >= byte_size)
    {
        my_byte_array_index = 0;
    }

    // We copy the timestamp into the character buffer.  We compare the timestamp
    // to the value in the buffer to see if another event entry has overwritten
    // our own, when we print the event log.
    memcpy(&my_byte_array[my_byte_array_index],&tmp_timestamp,sizeof(unsigned long));

    // And save a pointer into the character array in the event.
    the_event_array[my_index].my_string = &(my_byte_array[my_byte_array_index]);
    the_event_array[my_index].my_string_length = the_buf_size;
    my_byte_array_index += sizeof(unsigned long);

    // Now copy the actual character data into the buffer.
    int i;
    int tmp_actual = 0;
    for (i = 0;i < the_buf_size;++my_byte_array_index,++i)
    {
        char c = the_buf[i];
        // Strip out non-printable chars.
        if (c >= ' ')
        {
            my_byte_array[my_byte_array_index] = c;
            ++tmp_actual;
        }
    }

    // Since we strip off the non-printable chars, we need to adjust the length
    my_byte_array_index += tmp_actual;

    // Now advance the index in the event buffer.
    my_index++;
    if (my_index + 1 > my_size)
    {
        my_index = 0;
        my_wrapped = true;
    }
}

template<typename EventType,typename EnumT,int log_size,int byte_size>
void
char_log_template<EventType,EnumT,log_size,byte_size>::get_log_entry(int log_index,char* the_buffer,int buf_size)
{
    char* tmp_sprint_buffer = the_buffer;
    EventType* p = &(the_event_array[log_index]);
    tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                 "\n%10x",
                                 p->my_data.timestamp);

    tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                 " %10s ",
                                 EventType::the_event_strings[p->my_event].event_string[0]);

    // Check the timestamp in the log with the timestamp in the
    // event to make sure that the data in the circular buffer was
    // not overwritten.
    unsigned long* tmp_ts = (unsigned long*)(p->my_string);
    if (*tmp_ts == p->my_data.timestamp)
    {
        p->my_string += sizeof(unsigned long);
        memcpy(tmp_sprint_buffer,p->my_string,p->my_string_length);
        tmp_sprint_buffer += p->my_string_length;
        tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                     " ");
    }
    else
    {
        sprintf(tmp_sprint_buffer,"(wrapped) ");
    }
}

template<typename EventType,typename EnumT,int log_size>
event_log_template<EventType,EnumT,log_size>::event_log_template(const char* the_title)
:base_event_log(log_size,4),my_title(the_title)
{
}

template<typename EventType,typename EnumT,int log_size>
void
event_log_template<EventType,EnumT,log_size>::get_log_entry(int log_index,char* the_buffer,int buf_size)
{
    char* tmp_sprint_buffer = the_buffer;
    EventType* p = &(the_event_array[log_index]);
    tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                 "\n%10x",
                                 p->my_data.timestamp);

    tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                 " %10s",
                                 EventType::the_event_strings[p->my_event].event_string[0]);

    tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                 " %10s",
                                 p->my_data.thread_id);

    for (int i = 0;i < my_num_params;++i)
    {
        if (strcmp(EventType::the_event_strings[p->my_event].event_string[i + 1],""))
        {
            tmp_sprint_buffer += sprintf(tmp_sprint_buffer,
                                         " %6s = %4x,",
                                         EventType::the_event_strings[p->my_event].event_string[i + 1]
                                                   ,p->my_data.param[i]);
        }
    }
}


template<typename EventType,typename EnumT,int log_size,int byte_size>
char_log_template<EventType,EnumT,log_size,byte_size>::char_log_template(const char* the_title)
:base_event_log(log_size,4),my_title(the_title)
{
}

template<typename EventType,typename EnumT,int log_size,int byte_size>
void
char_log_template<EventType,EnumT,log_size,byte_size>::print_log(FILE* f)
{
    char sbuf[1024];
    fprintf(f,"\n ==== %s ===",my_title);
    int tmp_index = get_first_event_index();
    int tmp_first_index = tmp_index;
    int tmp_end = (my_wrapped == false ? get_last_event_index() : my_size);

    for (;(f != 0) && (tmp_index < tmp_end);++tmp_index)
    {
        if ((tmp_index - tmp_first_index) % 80 == 0)
        {
            fprintf(f,"\n timestmp   event           String");
        }
        get_log_entry(tmp_index,sbuf,1024);
        fprintf(f,"%s",sbuf);
    }

    if (my_wrapped)
    {
        tmp_index = 0;
        tmp_end = get_last_event_index();
        for (;(f != 0) && (tmp_index < tmp_end);++tmp_index)
        {
            get_log_entry(tmp_index,sbuf,1024);
            fprintf(f,"%s",sbuf);
        }
    }
}

template<typename EventType,typename EnumT,int log_size>
void event_log_template<EventType,EnumT,log_size>::print_log(FILE* f)
{
    char sbuf[1024];
    fprintf(f,"\n ==== %s ===",my_title);
    int tmp_index = get_first_event_index();
    int tmp_first_index = tmp_index;
    int tmp_end = (my_wrapped == false ? get_last_event_index() : my_size);

    for (;(f != 0) && (tmp_index < tmp_end);++tmp_index)
    {
        if ((tmp_index - tmp_first_index) % 80 == 0)
        {
            fprintf(f,"\n timestmp   event           thread           p1                  p2");
        }
        get_log_entry(tmp_index,sbuf,1024);
        fprintf(f,"%s",sbuf);
    }

    if (my_wrapped)
    {
        tmp_index = 0;
        tmp_end = get_last_event_index();
        for (;(f != 0) && (tmp_index < tmp_end);++tmp_index)
        {
            get_log_entry(tmp_index,sbuf,1024);
            fprintf(f,"%s",sbuf);
        }
    }
}

// Intantiate the event logs.
windows_event_log global_windows_event_log("Windows Event Log");
dispatch_event_log global_dispatch_event_log("Dispatch Events Log");
figure_event_log global_figure_event_log("Figure Event Log");
parser_event_log global_parser_event_log("Parser Event Log");
settings_event_log global_settings_event_log("Settings Event Log");
event_event_log global_event_event_log("Event Event Log");
text_event_log global_text_event_log("Text Event Log");
cursor_event_log global_cursor_event_log("Cursor Event Log");
midi_event_log global_midi_event_log("Midi Event Log");
doc_event_log global_doc_event_log("Document Event Log");

void all_event_logs::print_logs(FILE* f)
{
    global_settings_event_log.print_log(f);
    global_windows_event_log.print_log(f);
    global_dispatch_event_log.print_log(f);
    global_figure_event_log.print_log(f);
    global_parser_event_log.print_log(f);
    global_event_event_log.print_log(f);
    global_text_event_log.print_log(f);
    global_cursor_event_log.print_log(f);
    global_midi_event_log.print_log(f);
    global_doc_event_log.print_log(f);
}

}


