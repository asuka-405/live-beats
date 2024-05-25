#ifndef iabc_preprocess_h
#define iabc_preprocess_h
#if 0
#define _POSIX
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include "iabc/string.h"
#include <stdio.h>
#include "iabc/map.h"

#define NEWLINE '\n'
#undef getc

namespace iabc
{
;
// CLASS: text_buf
// DESCRIPTION:
// Abstract the behavior of a file so that it could work on any text
// buffer, such as a text editing window.
class text_buf
{
public:
    text_buf(){};
    virtual ~text_buf(){};

    typedef enum markers
    {
        eof = -1
    } markers;

    // METHOD: close
    // DESCRIPTION:
    // free up the resource so that it can be used by others.
    virtual void close() = 0;

    // METHOD: sync_contents_to_buffer
    // DESCRIPTION:
    // Make sure the contents of the text_file object match the contents of the
    // underlying buffer.
    virtual void sync_contents_to_buffer() = 0;

    // METHOD: buffer_setpos
    // DESCRIPTION:
    // set cursor position to l
    virtual void buffer_setpos(long l){};

    // METHOD: open
    // DESCRIPTION:
    // Open the text buffer for reading.
    static text_buf* open(const string& the_file);

    // METHOD: getpos
    // DESCRIPTION:
    // Return the current position of the pointer into the text buffer.
    virtual long getpos() = 0;

    // METHOD: getc
    // DESCRIPTION:
    // get the next character in the buffer.  Return text_buf::eof when the
    // last position is reached.
    virtual int getc() = 0;

    // METHOD: setpos
    // DESCRIPTION:
    // set the position of the next read to the_pos;
    virtual void setpos(long the_pos) = 0;

    // METHOD: refresh()
    // DESCRIPTION:
    // Refresh the text buffer with the persistent representation.
    virtual void refresh() = 0;
    
    // METHOD: get_last_modify_time
    // DESCRIPTION:
    // returns the last time the buffer was changed, so you'll know
    // if you need to reparse it.
    virtual time_t get_last_modify_time() const = 0;

    // METHOD: get_selection
    // DESCRIPTION:
    // For the real GUI application, we need to get and set the selection of the
    // text buffer so that the selection stays the same after redraw.
    virtual void get_selection(long& start,long& end){start=end=0;};

    // METHOD: set_selection
    // DESCRIPTION:
    // For the real GUI application, we need to get and set the selection of the
    // text buffer so that the selection stays the same after redraw.
    virtual void set_selection(long start,long end){};
};

// CLASS: file_buf
// DESCRIPTION:
// A stdio file implementation of text_buf.  Used for unit testing and
// development.
class file_buf:public text_buf
{
public:
    file_buf(const string& the_file):my_file(0),my_filename(the_file)
    {
        my_file = fopen(the_file.access_char_array(),"r");
    };

    virtual void sync_contents_to_buffer(){};

    // METHOD: close
    // DESCRIPTION:
    // free up the resource so that it can be used by others.
    virtual void close() 
    {
        if (my_file)
        {
            fclose(my_file);
            my_file = 0;
        }
    };

    // METHOD: getpos
    // DESCRIPTION:
    // Return the current position of the pointer into the text buffer.
    virtual long getpos() 
    {
        ::fpos_t tmp_pos;
        ::fgetpos(my_file,&tmp_pos);
#ifdef GCC
        return (long)(tmp_pos.__pos);
#else
        return (long)(tmp_pos);
#endif
    };
    // METHOD: getc
    // DESCRIPTION:
    // get the next character in the buffer.  Return text_buf::eof when the
    // last position is reached.
    virtual int getc() 
    {
        return ::getc(my_file);
    };

    // METHOD: setpos
    // DESCRIPTION:
    // set the position of the next read to the_pos;
    virtual void setpos(long the_pos) 
    {
        // NOTE: on Linux and Win32 long and int are the
        // same.  This should be written to be platform independent, somehow.
        fpos_t tmp_t;
#ifdef GCC
        tmp_t.__pos = the_pos;
#else
        tmp_t = the_pos;
#endif
        fsetpos(my_file,&tmp_t);

    };
    
    virtual time_t get_last_modify_time() const
    {
        struct stat tmp_stat;
        fstat(fileno(my_file),&tmp_stat);
        return (tmp_stat.st_mtime);
    };

    virtual void refresh()
    {
        if (my_file)
        {
            fclose(my_file);
        }
        my_file = fopen(my_filename.access_char_array(),"r");
    };
private:
    FILE* my_file;
    string my_filename;
};

// CLASS: preprocess
// DESCRIPTION:
// Preprocess an abc file.  Scan through the comments and return
// completed lines, including lines seperated with '\'.
//
// This class implements a 'fat' interface: there are functions here that control
// GUI elements if there is a GUI and do nothing if this is not a GUI (e.g. if this is
// one of the unit test programs.
class preprocess
{
public:
    preprocess(const string& the_file);
    string get_next_line();
    int get_line_number(){return my_line_number;};
    int get_lines_left();
    ~preprocess();

    // METHOD: set_line
    // DESCRIPTION:
    // Move to the position of the given line number.
    void set_line(long the_line);

    // METHOD: get_pos_from_line
    // DESCRIPTION:
    // Get the offset (for getpos, etc.) that we corresponds to
    // the beginning of the given line.
    long get_pos_from_line(long the_line);

    // METHOD: get_last_modify_time
    // DESCRIPTION:
    // return true if the file has changed since this was opened.
    time_t get_last_modify_time() const {return my_file->get_last_modify_time();};

    string get_filename() const {return my_filename;};
    
    // METHOD: index_file
    // DESCRIPTION:
    // Count the lines, and create the line to position map for the buffer.
    void index_file();

    long getpos() {return my_file->getpos();};
    void setpos(long l){my_file->setpos(l);};
    void buffer_setpos(long l){my_file->buffer_setpos(l);};

    void get_selection(long& start,long& end){my_file->get_selection(start,end);};
    void set_selection(long& start,long& end){my_file->set_selection(start,end);};
protected:
private:
    typedef enum file_states
    {
        normal,
        space,
        comment,
        done
    };
    void read_to_end();
    text_buf* my_file;
    string my_filename;
    int my_line_number;
    int my_total_lines;

    // ATTRIBUTE: my_lines_to_fpos
    // DESCRIPTION:
    // Keep track of which index is which line, so we can
    // get there fast.
    map<long,long> my_lines_to_fpos;
};
}

#endif


