/*
 * preprocess.cpp - Filters an abc file before the parser gets it.
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
#include "iabc/preprocess.h"
#include "iabc/factory.h"
#include "iabc/map.cpp"
#include "iabc/event_log.h"

namespace iabc
{
;
text_buf*
text_buf::open(const string& the_file)
{
    return factory<text_buf,const string&>::create(the_file);
}
#if 0
text_buf*
factory<text_buf,const string&>::create(const string& the_string)
{
    return new file_buf(the_string);
}
#endif
preprocess::~preprocess()
{
    if (my_file)
    {
        my_file->close();
        my_file = 0;
    }
}

preprocess::preprocess(const string& the_file):
my_filename(the_file),my_file(0),my_line_number(0),
    my_total_lines(0)
{
    my_file = text_buf::open(my_filename);
    index_file();
}

int
preprocess::get_lines_left()
{
    return my_total_lines - my_line_number;
}

string
preprocess::get_next_line()
{
    // Try to find a line with something interesting on it.
    bool tmp_nonwhite = false;

    string tmp_rv;
    if (my_file == 0)
    {
        my_file = text_buf::open(my_filename);
    }

    if (my_file == 0)
    {
        return tmp_rv;
    }

    preprocess::file_states tmp_state = preprocess::normal;
    while (tmp_state != preprocess::done)
    {
        int tmp_c = my_file->getc();

        // 2.0 standard, skip the backtick.
        while ((char)tmp_c == '`')
        {
            tmp_c = my_file->getc();
        }
        if ((tmp_c > ' ') && (tmp_c <= '~') && (tmp_c != '%'))
        {
            tmp_nonwhite = true;
        }

        if (tmp_c > 0)
        {
            int tmp_index = tmp_rv.length();

            // Eliminate duplicate spaces.  This speeds up the scanner some.
            if ((tmp_c <= ' ') &&
                (tmp_c != NEWLINE))
            {
                if (tmp_state != space)
                {
                    tmp_rv += (char)tmp_c;
                    tmp_state = space;
                }
            }
            else
            {
                if (tmp_state == space)
                    tmp_state = normal;
                tmp_rv += (char)tmp_c;
            }

            if (tmp_rv[tmp_index] == NEWLINE)
            {
                ++my_line_number;
                long tmp_pos = my_file->getpos();
                my_lines_to_fpos.add_pair(my_line_number,tmp_pos);
                if (tmp_nonwhite == false)
                {
                    tmp_state = normal;
                    tmp_rv = "";
                    continue;
                }

                tmp_state = done;
            }

            // We preprocess out comments
            else if (tmp_rv[tmp_index] == '%')
            {
                read_to_end();
                if (tmp_nonwhite == false)
                {
                    tmp_state = normal;
                    tmp_rv = "";
                    continue;
                }
                tmp_rv = tmp_rv.mid(0,tmp_rv.length() - 1);
                tmp_state = preprocess::done;
            }
        }
        else
        {
            tmp_state = done;
            my_line_number = my_total_lines;
        }
    }

    while ((tmp_rv.length() > 0) &&
           (tmp_rv[tmp_rv.length() - 1] <= ' '))
    {
        tmp_rv.chop();
    }
    tmp_rv += NEWLINE;
    return tmp_rv;
}

void
preprocess::read_to_end()
{
    int c = my_file->getc();
    while ((c > 0) && (c != NEWLINE))
    {
        c = my_file->getc();
    }
    my_line_number++;
}

void
preprocess::index_file()
{
    int tmp_lines = 0;
    long tmp_pos = 0;
    my_file->sync_contents_to_buffer();
    my_file->setpos(tmp_pos);

    int c = 0;
    while (c >= 0)
    {
        c = my_file->getc();
        tmp_pos = my_file->getpos();
        if (c == (int)NEWLINE)
        {
            ++tmp_lines;
            my_lines_to_fpos.add_pair(tmp_lines,tmp_pos);
        }
    }
    my_total_lines = tmp_lines;
    global_doc_event_log.log_event(doc_event_data::scan_lines,my_total_lines);
    my_file->setpos(0);
}

long
preprocess::get_pos_from_line(long the_line)
{
    long tmp_rv = 0;
    map<long,long>::iterator tmp_it =
        my_lines_to_fpos.get_item(the_line,lteq);
    if (tmp_it)
    {
        tmp_rv = (*tmp_it).value;
    }
    return tmp_rv;
}

void
preprocess::set_line(long the_line)
{
    map<long,long>::iterator tmp_it =
        my_lines_to_fpos.get_item(the_line,lteq);
    if (tmp_it)
    {
        my_file->setpos((*tmp_it).value);
        my_line_number = the_line;
    }
    else
    {
        my_file->setpos(0);
        my_line_number = 0;
    }
}

}
