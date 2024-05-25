#ifndef iabc_event_log
#define iabc_event_log
#include <stdio.h>

// FILE: event_log.h
// DESCRIPTION:
// Log all kinds of events and changes in program state.  Not
// to be confused with events, which are a specific concurrent
// type.

// These sizes can be adjusted to determine how much memory the
// log files use.
#define WINDOWS_LOG_SIZE 500
#define PARSER_LOG_SIZE 500
#define SETTINGS_LOG_SIZE 200
#define PARSER_LOGBUF_SIZE 16000
#define SETTINGS_LOGBUF_SIZE 16000
#define DISPATCH_LOG_SIZE 500
#define FIGURE_LOG_SIZE 2000
#define EVENT_LOG_SIZE 500
#define CURSOR_LOG_SIZE 500
#define TEXT_LOG_SIZE 500
#define MIDI_LOG_SIZE 1000

#define LOG_FRACTION(f) (((unsigned short)f.num << 16) | ((unsigned short)f.den))
namespace iabc
{
;

// CLASS: base_event_data
// DESCRIPTION:
// Most events have this data in common so we have a
// class for it to save some typeing.
class base_event_data
{
public:
    // These are the parameters of the event.
    unsigned long param[4];
    // Timestamp for each event.
    unsigned long timestamp;
    // thread ID in which the event occured.
    const char* thread_id;
    // Strings that describe the events.
    class event_string_class
    {
    public:
        char* event_string[5];
    };
};

// CLASS: base_event_log
// DESCRIPTION:
// A container for event log data.  Methods used for
// printing, logging.  The data that is actually in
// the log is implementation-specific
class base_event_log
{
public:
    // METHOD: base_event_log
    // DESCRIPTION:
    // Initialize the common data.  the_size is the
    // number of events and the_num_params is the number
    // of parameters in each event.
    base_event_log(int the_size,int the_num_params);

    // METHOD:  get_first_event_index
    // DESCRIPTION:
    // When printing out the events, we use get_first_event_index to
    // get the index of the oldest event in the circular queue
    virtual int get_first_event_index();

    // METHOD:  get_last_event_index
    // DESCRIPTION:
    // When printing out the events, we use get_last_event_index to
    // get the index of the most recent event in the circular queue
    virtual int get_last_event_index();

    // METHOD: get_log_size
    // DESCRIPTION:
    // Return the number of events in this particular log.
    virtual int get_log_size();

    // METHOD: get_log_entry
    // DESCRIPTION:
    // Print the log entry into a character buffer in human-readable format.
    virtual void get_log_entry(int log_index,char* the_buffer,int buf_size) = 0;

    // METHOD: print_log
    // DESCRIPTION:
    // Dump the entire log file to the stream f.
    virtual void print_log(FILE* f) = 0;

    // METHOD: get_timestamp
    // DESCRIPTION:
    // return the timestamp for an event log.
    unsigned long get_timestamp();
protected:
    // ATTRIBUTE: my_size
    // DESCRIPTION:
    // the number of events in this log
    int my_size;

    // ATTRIBUTE: my_index
    // DESCRIPTION:
    // the current index into the circular buffer.
    int my_index;

    // ATTRIBUTE: my_wrapped
    // DESCRIPTION:
    // true if the event log has wrapped.
    bool my_wrapped;

    // ATTRIBUTE: my_num_params
    // DESCRIPTION:
    // The number of parameters in a single event.
    int my_num_params;
};

// CLASS: event_log_template
// DESCRIPTION:
// Most event logs are exactly the same except for the
// strings used to describe the numerical parameters and
// the number of parameters per event.  This template handles
// all of those types of events.
template <typename EventType,typename EnumT,int log_size>
class event_log_template:public base_event_log
{
public:
    // DESCRIPTION: ctor
    // The title is printed out before the entire log is dumped.
    event_log_template(const char* the_title);

    // METHOD: log_event
    // DESCRIPTION:
    // Store an event in the event log array.  The enumeration is
    // one of the types of events defined for EventType
    virtual void log_event(EnumT e,
               unsigned long ulp1 = 0,
               unsigned long ulp2 = 0,
               unsigned long ulp3 = 0,
               unsigned long ulp4 = 0);

    // METHOD: get_log_entry
    // DESCRIPTION:
    // Implementation of the interface described above.
    void get_log_entry(int log_index,char* the_buffer,int buf_size);

    // METHOD: get_log_entry
    // DESCRIPTION:
    // Implementation of the interface described above.
    virtual void print_log(FILE* f);
    const char* my_title;
    EventType the_event_array[log_size];
};

// CLASS: char_log_template
// DESCRIPTION:
// Some event logs have variable-sized events.  Each event generates
// some text that is variable size and some numerical events that are
// of fixed size.
template <typename EventType,typename EnumT,int log_size,int byte_size>
class char_log_template:public base_event_log
{
public:
    char_log_template(const char* the_title);

    // METHOD: log_event
    // DESCRIPTION:
    // Log the event.  The first parameter is the variable-sized part,
    // which is represented as a character string.
    virtual void log_event(const char* buf,
                           int buf_size,
                           EnumT e,
               unsigned long ulp1 = 0,
               unsigned long ulp2 = 0,
               unsigned long ulp3 = 0,
               unsigned long ulp4 = 0);

    // METHOD: get_log_entry
    // DESCRIPTION:
    // Implementation of the interface described above.
    virtual void get_log_entry(int log_index,char* the_buffer,int buf_size);

    // METHOD: get_log_entry
    // DESCRIPTION:
    // Implementation of the interface described above.
    virtual void print_log(FILE* f);
    const char* my_title;

    // ATTRIBUTE: the_event_array
    // DESCRIPTION:
    // the log of event data.
    EventType the_event_array[log_size];

    // ATTRIBUTE: my_byte_array
    // DESCRIPTION:
    // the array for the variable-sized character data.
    char my_byte_array[byte_size];
    int my_byte_array_index;
};

// CLASS: windows_event_data
// DESCRIPTION:
// A single windows event.  The interface allows the template class
// event_log_template to manage these events.
class windows_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        redraw_window,
        vscroll_window,
        hscroll_window,
        set_scroll_point,
        create_window,
        destroy_window,
        draw_line,
        draw_text,
        draw_rect,
        draw_poly,
        create_font,
        move_sash,
        set_sash,
        add_point,
        windows_events_last
    };
    // This is the number of events in the log for
    // this type
    typedef enum tag_log_size
    {
        log_size = WINDOWS_LOG_SIZE
    };

    base_event_data my_data;

    // This is the event type
    class_events my_event;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[windows_events_last];
};

// CLASS: dispatch_event_data
// DESCRIPTION:
// A single dispatch event.
class dispatch_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        enter,
        same_thread,
        blocking,
        unblocked,
        ctor,
        dtor,
        last
    };

    typedef enum tag_log_size {log_size = DISPATCH_LOG_SIZE};

    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

// CLASS: doc_event_data
// DESCRIPTION:
// The lifetime of a document and things that happen to it.
class doc_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        retreive_value_from_box,
        select,
        remove_all_tunes,
        populate_list_box,
        ctor,
        dtor,
        redraw,
        rerender_for_print,
        print_page,
        print_event,
        has_page,
        rescale,
        refresh_current_tune,
        choose_tune,
        handle_selection,
        setup_pages,
        play,
        open_event,
        close_event,
        entry_point,
        restart,
        get_rect_from_page,
        attach_canvas,
        detach_canvas,
        select_list,
        selection_ctor,
        selection_ctorp1,
        selection_cctor,
        selection_dtor,
        selection_opeq,
        file_modify,
        file_time_reset,
        underlying_file_changed,
        scan_lines,
        last
    };

    typedef enum tag_log_size
    {
        log_size = FIGURE_LOG_SIZE
    };
    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

// CLASS: figure_event_data
// DESCRIPTION:
// A figure (e.g. a picture of a note) has changed.
class figure_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        staff_create,
        gclef_create,
        fclef_create,
        ohead_create,
        chead_create,
        ustem_create,
        dstem_create,
        sharp_create,
        remove_ref,
        add_ref,
        render_poly,
        calc_size,
        calc_size1,
        rescale,
        draw_self,
        get_size,
        dispatch_draw_self,
        last
    };

    typedef enum tag_log_size
    {
        log_size = FIGURE_LOG_SIZE
    };
    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

// CLASS: event_event_data
// DESCRIPTION:
// Data to log a single event.
class event_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        add_handler,
        remove_handler,
        op_or,
        wait,
        add_handler_to_list,
        notify_handlers,
        populate_handler_list,
        set_event_source,
        handle_event,
        add_handler_to_chain,
        tear_down_chain,
        zero_all_refs,
        pass_event_to_first,
        add_all_refs,
        remove_all_refs,
        last
    };

    typedef enum tag_log_size {log_size = EVENT_LOG_SIZE};

    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

// CLASS: parser_event_data
// DESCRIPTION:
// A single parser event.
class parser_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        words_rule,
        measure_rule,
        header_rule,
        match_parse_unit,
        compare_parse_unit,
        last
    };

    typedef enum tag_log_size {log_size = PARSER_LOG_SIZE};
    typedef enum tag_buffer_size {buf_size = PARSER_LOGBUF_SIZE};

    // ATTRIBUTE: my_event
    // This is enumeration that describes the event.
    class_events my_event;

    // ATTRIBUTE: my_data
    // DESCRIPTION:
    // This is some data that is common to all events
    base_event_data my_data;

    char* my_string;
    unsigned char my_string_length;
    // Strings that describe the events.
    class event_string_class
    {
    public:
        char* event_string[5];
    };

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static event_string_class the_event_strings[last];
};

// CLASS: parser_event_data
// DESCRIPTION:
// A single parser event.
class settings_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        set_program_dir_event,
        set_docs_dir_event,
        set_temp_dir_event,
        set_home_dir_event,
        set_filename,
        set_ini_file,
        set_new_file,
        last
    };

    typedef enum tag_log_size {log_size = SETTINGS_LOG_SIZE};
    typedef enum tag_buffer_size {buf_size = SETTINGS_LOGBUF_SIZE};

    // ATTRIBUTE: my_event
    // This is enumeration that describes the event.
    class_events my_event;

    // ATTRIBUTE: my_data
    // DESCRIPTION:
    // This is some data that is common to all events
    base_event_data my_data;

    char* my_string;
    unsigned char my_string_length;
    // Strings that describe the events.
    class event_string_class
    {
    public:
        char* event_string[5];
    };

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static event_string_class the_event_strings[last];
};


class cursor_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        store_screen_point,
        store_text_point,
        change_cursor_pos,
        change_text_buf,
        click_gui_window,
        score_to_text,
        last
    };

    typedef enum tag_log_size {log_size = CURSOR_LOG_SIZE};

    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

class text_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        key_press,
        pop_undo,
        cut,
        paste,
        copy,
        push_undo,
        clipboard_open,
        last
    };

    typedef enum tag_log_size {log_size = CURSOR_LOG_SIZE};

    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

class midi_event_data
{
public:
    // These are the kinds of events that can be logged
    // for this event.
    typedef enum class_events
    {
        play_note,
        last
    };

    typedef enum tag_log_size {log_size = MIDI_LOG_SIZE};

    // This is the event type
    class_events my_event;

    base_event_data my_data;

    // ATTRIBUTE: the_event_strings
    // DESCRIPTION:
    // The strings that describe the events and their parameters.
    static base_event_data::event_string_class the_event_strings[last];
};

// The actual event logs are instantiations of the
// templates classes with the event data as a paramter,
// along with some size information for the static arrays.
typedef event_log_template<windows_event_data,
    windows_event_data::class_events,
    windows_event_data::log_size> windows_event_log;
typedef event_log_template<dispatch_event_data,
    dispatch_event_data::class_events,
    dispatch_event_data::log_size> dispatch_event_log;
typedef event_log_template<figure_event_data,
    figure_event_data::class_events,
    figure_event_data::log_size> figure_event_log;
typedef char_log_template<parser_event_data,
    parser_event_data::class_events,
    parser_event_data::log_size,
    parser_event_data::buf_size> parser_event_log;
typedef char_log_template<settings_event_data,
    settings_event_data::class_events,
    settings_event_data::log_size,
    settings_event_data::buf_size> settings_event_log;
typedef event_log_template<event_event_data,
    event_event_data::class_events,
    event_event_data::log_size> event_event_log;
typedef event_log_template<text_event_data,
    text_event_data::class_events,
    text_event_data::log_size> text_event_log;
typedef event_log_template<cursor_event_data,
    cursor_event_data::class_events,
    cursor_event_data::log_size> cursor_event_log;
typedef event_log_template<midi_event_data,
    midi_event_data::class_events,
    midi_event_data::log_size> midi_event_log;
typedef event_log_template<doc_event_data,
    doc_event_data::class_events,
    doc_event_data::log_size> doc_event_log;

// This allows references to the log files by just including
// this file.
extern windows_event_log global_windows_event_log;
extern dispatch_event_log global_dispatch_event_log;
extern figure_event_log global_figure_event_log;
extern parser_event_log global_parser_event_log;
extern settings_event_log global_settings_event_log;
extern event_event_log global_event_event_log;
extern cursor_event_log global_cursor_event_log;
extern text_event_log global_text_event_log;
extern midi_event_log global_midi_event_log;
extern doc_event_log global_doc_event_log;

// This is used by 'main' when the 'dump logs' menu
// item is chosen.
class all_event_logs
{
public:
    static void print_logs(FILE* f);
};

}
#endif


