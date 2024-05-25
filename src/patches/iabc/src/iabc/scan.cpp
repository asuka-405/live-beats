/*
 * scan.cpp - A set of classes for handling regular expressions.
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
#include "iabc/scan.h"
#include "iabc/list.cpp"
#include "iabc/array.cpp"
#include "iabc/factory.cpp"

namespace iabc
{
;
static instance_counter<scan_state> scan_state_instance_count;

// Note that the order of these matters.  There is usually some ambiguity
// in a regular expression, and unless you make sure that every state
// knows about all other possible expressions its usually easiest
// to let the order of the states do some of the work for you.  For example
// '.' should match before literal since literal will look for a '.'.
// If you want a '.' literal you need "\.", otherwise '.' is a wildcard.

wildcard_state_creator& 
wildcard_state_creator::get_instance()
{
    static wildcard_state_creator the_instance;
    return the_instance;
}

range_state_creator&
range_state_creator::get_instance()
{
    static range_state_creator the_instance;
    return the_instance;
}

digit_state_creator& 
digit_state_creator::get_instance()
{
    static digit_state_creator the_instance;
    return the_instance;
}

ext_digit_state_creator& 
ext_digit_state_creator::get_instance()
{
    static ext_digit_state_creator the_instance;
    return the_instance;
}

word_state_creator& 
word_state_creator::get_instance()
{
    static word_state_creator the_instance;
    return the_instance;
}

non_whitespace_state_creator&
non_whitespace_state_creator::get_instance()
{
    static non_whitespace_state_creator the_instance;
    return the_instance;
}

line_state_creator&
line_state_creator::get_instance()
{
    static line_state_creator the_instance;
    return the_instance;
}

literal_state_creator&
literal_state_creator::get_instance()
{
    static literal_state_creator the_instance;
    return the_instance;
}

// This is just to get the linker to put the
// code in for us.  It doesn't need to be called
void 
state_initializers()
{
    range_state_creator::get_instance();
    digit_state_creator::get_instance();
    ext_digit_state_creator::get_instance();
    word_state_creator::get_instance();
    non_whitespace_state_creator::get_instance();
    line_state_creator::get_instance();
    wildcard_state_creator::get_instance();
    literal_state_creator::get_instance();
}

scan_state::scan_state(int the_group_number,bool the_is_greedy):
               my_group_number(the_group_number),
    my_is_one_or_more(false),
    my_is_zero_or_more(false),
    my_is_greedy(the_is_greedy),
    my_match_count(0),
    my_has_failed_further_matches(false),
    my_should_try_new_thing(false),
    my_start_index(0),
    my_end_index(0),
    my_string_ptr(0)
{
    ++scan_state_instance_count;
}

scan_state::~scan_state()
{
    // If we are modifying a previous state, delete that one
    // as well.
    --scan_state_instance_count;
}

void scan_state::check_state_integrity()
{
}

// Add 'this' to the static list of scan_state_creators,
// so that we can search them all when creating states.
scan_state_creator::scan_state_creator()
{
    add_at_end(all_creators(),this);
}

// Return true if the individual state matches this part of the string
bool 
scan_state::match(const char* the_string,int the_string_length,int& the_index,int the_length)
{
    int tmp_number_last_matched = my_match_count;
    my_string_ptr = the_string;
    my_start_index = the_index;
    my_end_index = my_start_index;
    bool tmp_success = true;
    bool tmp_match_multiple = (my_is_zero_or_more || my_is_one_or_more);

    // Greedy match multiple, second or more time through
    if ((my_is_greedy == true) && (tmp_number_last_matched > 0) && (my_should_try_new_thing))
    {
		--my_match_count;
        the_index += (my_match_count * this->private_get_length());
        // If 1 match, we can't get any shorter if we're one or more (+)
        if ((my_match_count == 1) && (my_is_one_or_more))
        {
            my_has_failed_further_matches = true;
        }
        if ((my_match_count == 0) && (my_is_zero_or_more))
        {
            my_has_failed_further_matches = true;
        }
    }
    // Greedy match multiple, first time through
    else if ((my_is_greedy == true) && (tmp_number_last_matched == 0) && (my_has_failed_further_matches == false))
    {
        tmp_success = match_output(the_string,the_string_length,the_index,the_length);
        bool tmp_current_match = tmp_success;
        while ((tmp_current_match) && (tmp_match_multiple))
        {
            ++my_match_count;
            tmp_current_match = 
                match_output(the_string,the_string_length,the_index,the_length);
        }
        // If there is 0 or 1 matches, we may be done already
        if ((my_match_count == 0) && (my_is_zero_or_more))
        {
            tmp_success = true;
            my_has_failed_further_matches = true;
        }
        else if ((my_match_count == 1) && (my_is_one_or_more))
        {
            my_has_failed_further_matches = true;
        }
    }
    // non-Greedy match multiple, kleene*, start with zero-length string
    else if ((my_is_greedy == false) && 
             (tmp_number_last_matched == 0) &&
             (my_is_zero_or_more == true))
    {
        // Magic - non-greedy my_match_count is actually 1 too big.  This allows
        // use to keep track of the case where we start with the 0 length
        // string on Kleene*
        ++my_match_count;
        tmp_success = true;
    }
    // Non-greedy match multiple
    else if ((my_is_greedy == false) && 
             ((tmp_number_last_matched > 0) || (my_should_try_new_thing))
              && (tmp_match_multiple))
    {
        int i;
        tmp_success = match_output(the_string,the_string_length,the_index,the_length);
        if (tmp_success == false)
        {
            my_has_failed_further_matches = true;
        }
        for (i = 0;i < (tmp_number_last_matched - 1) && tmp_success && tmp_match_multiple;++i)
        {
            if (match_output(the_string,the_string_length,the_index,the_length) == false)
            {
                my_has_failed_further_matches = true;
            }
        }
        if (my_has_failed_further_matches == false)
        {
            my_match_count++;
        }
    }
    // Just a normal match
    else if (tmp_match_multiple == false)
    {
        tmp_success = match_output(the_string,the_string_length,the_index,the_length);
    }

    // If this is a failure, but we are zero-or-more, return success anyway.
    if (my_is_zero_or_more)
    {
        tmp_success = true;
    }

    // Keep track of where the string started and ended
    my_end_index = the_index;

    return tmp_success;
}

bool
scan_state::is_there_anything_i_havent_tried()
{
    bool tmp_match_multiple = (my_is_zero_or_more | my_is_one_or_more);
    if ((tmp_match_multiple) &&
        (my_has_failed_further_matches == false))
    {
        return true;
    }

    return false;
}

void 
scan_state::reset()
{
    my_match_count = 0;
    my_has_failed_further_matches = false;
    my_start_index = my_end_index = 0;
}

// Consume the input string and create the state for this part of the string
scan_state* 
scan_state_creator::create_state(const string& the_string,int& the_index,
                                 int the_group_number,bool the_is_greedy)
{
    state_initializers();
    int tmp_start_index = the_index;

    // We ignore the result.  The predicate actually keeps track of which
    // element of the list found a match.
    find_state_pred tmp_pred(the_string,the_index,the_group_number,the_is_greedy);
    findit_predicate(all_creators(),tmp_pred);
    
    scan_state* tmp_ptr = tmp_pred.get_state();

    // We assume that if the scan state was created, the_index has changed.
    // So we check that here.  The assert would indicate a programming error.
    // It is sort of assumed that the input string for this program will be
    // fixed so there is no danger that it will be invalid.  This would not be
    // true if you were implementing an interpreter or compiler, so you should
    // handle this exception in that case.
    if (tmp_ptr)
    {
        if (tmp_start_index == the_index)
        {
            throw("inifinite loop detected in scan expression\n");
        }
    }

    return tmp_ptr;
}

string
scan_state::get_match()
{
    return string(&(my_string_ptr[my_start_index]),my_end_index - my_start_index);
}

instance_counter<regexp> regexp::class_instance_count;

regexp::~regexp()
{
    int i,j;
    --class_instance_count;
   for (i = 0;i < this->my_sub_expressions.get_size();++i)
    {
        delete my_sub_expressions[i];
    }

    for (i = 0;i < this->my_states.get_size(); ++i)
    {
        my_states[i]->check_state_integrity();
    }
    for (i = 0;i < this->my_states.get_size(); ++i)
    {
        for (j = i;j < this->my_states.get_size(); ++j)
        {
            my_states[j]->check_state_integrity();
        }
        delete my_states[i];
    }
}

// A regular expression created by a client.
regexp::regexp(const string& the_input,bool the_is_greedy)
:my_matched_flag(false),my_is_greedy(the_is_greedy),
    my_should_reset(false)
    
{
    ++class_instance_count;
    populate_machine(the_input);
}

// A sub-expression in a regular expression using '|' for 'or'
regexp::regexp(scan_state* the_first_state,string& the_input,
               int the_match_index,
               regexp::group_state the_state,
               bool the_is_greedy)
:my_matched_flag(false),my_is_greedy(the_is_greedy),
 my_should_reset(false)
{
    ++class_instance_count;
    my_states.expand(1);
    my_states[0] = the_first_state;
    populate_machine(the_input,the_match_index,the_state);
}

void regexp::reset()
{   ;
    int i;
    for (i = 0;i < my_states.get_size(); ++i)
    {
        my_states[i]->reset();
    }
	
    clear_all_strings();
    for (i = 0;i < my_sub_expressions.get_size();++i)
    {
        my_sub_expressions[i]->reset();
        my_sub_expressions[i]->clear_all_strings();
    }
	my_matched_flag = false;
}

// We have constructed a state machine from some input string that defines
// the language we understand.  Now run the machine and handle any
// errors.
bool
regexp::match(const char* the_string,int the_string_length)
{
    int i,j;
    bool tmp_success = true;
    int tmp_index = 0;
    int tmp_highest_variable_index = -1;
    int tmp_last_matched_string_index = 0;

    // If this is an old expression, reset all the states to 
    // clean up garbage from earlier failed match.
    if (my_should_reset)
    {
        reset();
    }

    my_should_reset = true;
    for (i = 0;i < my_states.get_size();++i)
    {
        scan_state* tmp_state = my_states[i];
        tmp_success = tmp_state->match(the_string,the_string_length,tmp_index,the_string_length);
        // Keep trying as long as there's a match
        if (tmp_success)
        {
            if (tmp_state->is_there_anything_i_havent_tried() == true)
            {
                tmp_highest_variable_index = i;
                tmp_last_matched_string_index = tmp_state->get_start_index();
            }
        }

        // If there was a match, then try an earlier variable-length
        // expression.
        else if (tmp_highest_variable_index >= 0)
        {
            int tmp_new_highest_variable_index = -1;
			tmp_index = tmp_last_matched_string_index;
            for (j = 0;j < i;++j)
            {
                tmp_state = my_states[j];
                if ((j < tmp_highest_variable_index) &&
                    (tmp_state->is_there_anything_i_havent_tried()))
                {
                    tmp_new_highest_variable_index = j;
                    tmp_last_matched_string_index = tmp_state->get_start_index();
                }
                else if (j == tmp_highest_variable_index)
                {
                    tmp_state->try_new_thing();
                }
                else if (j > tmp_highest_variable_index)
                {
                    tmp_state->reset();
                }
            }
            i = (tmp_highest_variable_index - 1);
            tmp_highest_variable_index = tmp_new_highest_variable_index;
        }
        // There are no matches, and no variable lenght expressions that we
        // haven't tried.  Give up.
        else
            break;
    }

    // If this is the last state, but I am part a sub-expression (due to 'or')
    // try to match one of the other regexps.
    if ((tmp_success) &&
         (my_sub_expressions.get_size() > 0))
    {
        tmp_success = match_sub_expression(the_string,the_string_length,tmp_index);
    }

    my_matched_flag = tmp_success;

    // We have matched the expression.  Now go through and construct the 
    // string out of the matches scan states and subexpressions.
    string tmp_matches;
    if (my_matched_flag)
    {
        my_matches.remove_all();
        my_matches.expand_to(1);
		my_matches[0] = "";
        int tmp_current_group = 0;
        for (i = 0;i < my_states.get_size();++i)
        {
            scan_state* tmp_state = my_states[i];
            if (tmp_state->get_group_number() != tmp_current_group)
            {
                tmp_current_group = tmp_state->get_group_number();
                my_matches.expand_to(tmp_current_group + 1);
            }
            if (tmp_current_group > 0)
            {
                my_matches[tmp_current_group] += tmp_state->get_match();
            }
            my_matches[0] += tmp_state->get_match();
        }

        for (i = 0;i < my_sub_expressions.get_size();++i)
        {
            regexp* tmp_expression = my_sub_expressions[i];
            if (tmp_expression->is_match() == true)
            {
                my_matches[0] += tmp_expression->my_matches[0];
                for (j = 1;j < tmp_expression->my_matches.get_size();++i)
                {
                    my_matches.expand(1);
                    my_matches[my_matches.get_size() - 1] = tmp_expression->my_matches[j];
                }
                break;
            }
        }
    }
	return my_matched_flag;
}

bool 
regexp::match_sub_expression(const char* the_string,int the_string_length,int& the_index)
{
    int i;
    const char* tmp_string = &(the_string[the_index]);
	int tmp_length = the_string_length - the_index;
    int tmp_original_index = the_index;

    // We have come to the end of our expression.  If there were subexpressions
    // as part of an '|' clause, go through all of them and try to match the
    // rest of the string.
    for (i = 0;i < my_sub_expressions.get_size(); ++i)
    {
        regexp* tmp_exp = my_sub_expressions[i];

        if (tmp_exp->match(&(the_string[the_index]),tmp_length))
        {
            // We found a match for the rest of the expression.  Add
            // the matched strings to our matched strings
            // for convenience of honoring the [] operator
            return true;
        }
    }

    return false;
}

void
regexp::clear_all_strings()
{
    int i;
    for (i = 0;i < my_matches.get_size();++i)
    {
		if (my_matches[i][0] != 0)
			my_matches[i] = "";
    }
}

// That was for running the state machine to match a string.  The rest of these
// regexp methods are the nitty-gritty of constructing said machine.  This
// method is in charge.
void 
regexp::populate_machine(const string& the_string,int the_match_index,regexp::group_state the_group_state)
{
    int tmp_index = 0;
    int tmp_state_index = my_states.get_size();
    my_valid_input_flag = true;
    scan_state* tmp_previous = 0;
    group_state tmp_group_state = the_group_state;
    int tmp_group_count = the_match_index;

    // We create our state machine by consuming the whole input string
    while (tmp_index < the_string.length())
    {
        list<scan_state*> tmp_list;

        // If this is a paren, we want to group the input on this
        // expression.
        if (the_string[tmp_index] == '(')
        {
			++tmp_group_count;
            if (tmp_group_state == regexp::no_group)
            {
                tmp_group_state = regexp::group;
            }
			++tmp_index;
            continue;
        }
        // If this is a paren, we want to group the input on this
        // expression.
        else if (the_string[tmp_index] == ')')
        {
            if (tmp_group_state == regexp::group)
            {
                tmp_group_state = regexp::no_group;
            }
            // Again, we are assuming here that the
            // input expressions are part of the program.  If this
            // is not the case, you'll need to handle this
            else
            {
                my_valid_input_flag = false;
                break;
            }
			++tmp_index;
            continue;
        }

        // Due to the use of '|' (or), there could be more than one next state.  So
        // we put all possible next states into a linked list.
        int tmp_current_group = 
            (tmp_group_state == regexp::no_group) ? -1 : tmp_group_count;
        put_next_state_in_list(tmp_list,the_string,tmp_index,tmp_current_group);

        // If this is a normal state (i.e. not an '|') then just add it to my array
        // of states.
        if (tmp_list.get_size() == 1)
        {
            my_states.expand(1);
            list<scan_state*>::iterator tmp_it(tmp_list.first());
            scan_state* tmp_next = (*tmp_it);
            tmp_it.delete_current();
            my_states[tmp_state_index] = tmp_next;
			tmp_state_index++;
			tmp_previous = tmp_next;
        }

        // Otherwise, if there are additional states, don't add it to my list.
        // instead, create another regular expression for each possible next
        // state.  We will try to match them all when the state machine is run.
        else if (tmp_list.get_size() > 1)
        {
            string tmp_string = the_string.mid(tmp_index,(the_string.length() - tmp_index));
            my_valid_input_flag = 
                create_additional_expressions(tmp_list,tmp_string,
                                              tmp_group_count,tmp_group_state);

            break;
        }
        // Otherwise the input string was not a valid regular expression.
        else
        {
            my_valid_input_flag = false;
			break;
        }
    }
}

void 
regexp::put_next_state_in_list(list<scan_state*>& the_list,const string& the_string,int& the_index,
                               int the_group_number)
{
    list<scan_state*>::iterator tmp_list(the_list.first());
    
    // Get the next state from the scan_state_creator based on the input string.
    scan_state* next_state = 
        scan_state_creator::create_state(the_string,the_index,the_group_number,my_is_greedy);

    // If we got one state, there may be more.  Check to see if the next
    // token is '|' and add additional states if it is.
    if (next_state)
    {
        // No matter what, we add the first state.
        tmp_list.add_after(next_state);
        tmp_list.next();
        if (the_string[the_index] == '*')
        {
            next_state->set_zero_or_more(true);
            ++the_index;
        }
        else if (the_string[the_index] == '+')
        {
            next_state->set_one_or_more(true);
            ++the_index;
        }

        // This decides if an additional state needs to be created for '|'
        while ((next_state) && (the_string[the_index] == '|'))
        {
			++the_index;
            next_state = scan_state_creator::create_state(
                the_string,the_index,the_group_number,my_is_greedy);
            if (next_state)
            {
                if (the_string[the_index] == '*')
                {
                    next_state->set_zero_or_more(true);
                    ++the_index;
                }
                else if (the_string[the_index] == '+')
                {
                    next_state->set_one_or_more(true);
                    ++the_index;
                }
                tmp_list.add_after(next_state);
                tmp_list.next();
            }
        }
    }
}

bool 
regexp::create_additional_expressions(list<scan_state*>& the_list,const string& the_string,
                                      int the_group_number,group_state the_group_state)
{
    // Create regular expressions for all states that were constructed due to
    // the strings after the '|' (or) clause.
    create_expression_predicate tmp_pred(my_sub_expressions,the_string,
                                         the_group_number,
										 the_group_state,
										 my_is_greedy);
    do_to_all(the_list,tmp_pred);

    return tmp_pred.my_is_valid;
}

void 
regexp::insert_match_in_string(int the_index,const string& the_match)
{
    if (my_matches.get_size() < the_index + 1 )
    {
        my_matches.expand((the_index + 1) - my_matches.get_size());
    }

    my_matches[the_index] += the_match;

	// regexp[0] is the full match, so save the match no matter
	// which group we're in
    if (the_index != 0)
    {
        my_matches[0] += the_match;
    }
}

// These are the individual states.  It's pretty much tedious if/then/else
// stuff.

bool 
wildcard_state::match_output(const char* the_string,int the_string_length,
							 int& the_index,int the_length)
{
    if (the_string[the_index] != (char)0)
    {
        ++the_index;
        return true;
    }
    return false;
}

scan_state* 
wildcard_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy)
{
    if ((the_string.length() > the_index) && (the_string[the_index] == '.'))
    {
		++the_index;
        return new wildcard_state(the_group_number,the_is_greedy);
    }

    return 0;
}

scan_state* 
literal_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy) 
{
	string tmp_input = "";

    // Match any escaped literal
    if ((the_string.length() > the_index + 1) &&
        (the_string[the_index] == '\\'))
    {
        tmp_input = string(the_string[the_index + 1]);
        the_index += 2;
    }
    // Match any number of literals in [] brackets
    else if ((the_string.length() > the_index + 2) &&
             (the_string[the_index] == '['))
    {
        ++the_index;
        while ((the_string[the_index] != '\0') &&
               (the_string[the_index] != ']'))
        {
            if (the_string[the_index] == '\\')
            {
                ++the_index;
            }
            tmp_input += the_string[the_index];
            ++the_index;
        }
        // Skip the last bracked
        if (the_string[the_index] == ']')
        {
            ++the_index;
        }
    }
    // Match a single of anything else
    else if (the_string.length() > the_index)
    {
        tmp_input = string(the_string[the_index]);
        the_index++;
    }
    else
    {
        return 0;        
    }

	return new literal_state(tmp_input,the_group_number,the_is_greedy);
}

bool
range_state::match_output(const char* the_string, int the_string_length,
                          int &the_index, int the_length)
{
    if ((the_string_length > the_index) &&
        (the_string[the_index] >= my_low) &&
        (the_string[the_index] <= my_high))
        {
        ++the_index;
        return true;
        }

    return false;
}

scan_state* 
range_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy)
{
    char tmp_low;
    char tmp_high;
    if ((the_string.length() >= the_index + 5) &&
        (the_string[the_index] == '[') &&
        (the_string[the_index + 2] == '-') &&
        (the_string[the_index + 4] == ']'))
        {
        tmp_low = the_string[the_index + 1];
        tmp_high = the_string[the_index + 3];
        the_index += 5;
        return new range_state(tmp_low,tmp_high,the_group_number,the_is_greedy);
        }

    return 0;
}

bool 
line_state::match_output(const char* the_string, int the_string_length,
                         int &the_index, int the_length)
{
    if ((the_index == 0) && (this->my_type == begin))
    {
        return true;
    }
    else if (my_type == end)
    {
        if ((the_index >= the_string_length - 1) ||
            (the_string[the_index] == '\n'))
        {
            return true;
        }
    }

	return false;
}

scan_state* 
line_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy)
{
    if (the_string[the_index] == '$')
    {
        ++the_index;
        return new line_state(line_state::end,the_group_number,the_is_greedy);
    }
    else if (the_string[the_index] == '^')
    {
        ++the_index;
		return new line_state(line_state::begin,the_group_number,the_is_greedy);
    }

    return 0;
}

bool 
digit_state::match_output(const char* the_string, int the_string_length,
                          int &the_index, int the_length)
{
    char c = the_string[the_index];
    if ((c >= '0') && (c <= '9')) 
        {
        ++the_index;
        return true;
        }
    return false;
}

bool 
ext_digit_state::match_output(const char* the_string, int the_string_length,
                          int &the_index, int the_length)
{
    char c = the_string[the_index];
    if ((c >= '0') && (c <= '9')) 
        {
        ++the_index;
        return true;
        }
    else if ((c == '+') || (c == '-'))
    {
        ++the_index;
        return true;
    }
    return false;
}

scan_state*
word_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy)
{
    if ((the_index + 1 <= the_string.length()) &&
        (the_string[the_index] == '\\') &&
        (the_string[the_index + 1] == 'w'))
        {
        the_index += 2;
        return new word_state(the_group_number,the_is_greedy);
        }

    return 0;
}

bool
word_state::match_output(const char* the_string,int the_string_length,int& the_index,int the_length)
{
    char c = the_string[the_index];
    if (((c >= 'A') && (c <= 'z')) ||
        ((c <= '0') && (c >= '9')) ||
         ((c == '_'))) 
        {
        ++the_index;
        return true;
        }
    return false;
}

scan_state* 
digit_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy) 
{
    if ((the_index + 1 <= the_string.length()) &&
        (the_string[the_index] == '\\') &&
        (the_string[the_index + 1] == 'd'))
        {
        the_index += 2;
        return new digit_state(the_group_number,the_is_greedy);
        }

    return 0;
}

scan_state*
ext_digit_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy)
{
    if ((the_index + 1 <= the_string.length()) &&
        (the_string[the_index] == '\\') &&
        (the_string[the_index + 1] == 'D'))
        {
        the_index += 2;
        return new digit_state(the_group_number,the_is_greedy);
        }

    return 0;
}

bool
non_whitespace_state::match_output(const char* the_string,int the_string_length,
                                   int& the_index,int the_length)
{
    char c = the_string[the_index];
    if ( c > ' ')
    {
        ++the_index;
        return true;
    }
    return false;
}

scan_state*
non_whitespace_state_creator::match_input(const string &the_string, int &the_index, 
                                    int the_group_number,bool the_is_greedy)
{
    if ((the_index + 1 <= the_string.length()) &&
        (the_string[the_index] == '\\') &&
        (the_string[the_index + 1] == 'W'))
        {
        the_index += 2;
        return new non_whitespace_state(the_group_number,the_is_greedy);
        }

    return 0;
}

bool 
literal_state::match_output(const char* the_string, int the_string_length,
                            int &the_index, int the_length)
{
    if (the_string_length >= the_index + my_input.length())
    {
        int i;
        bool tmp_match = true;
        for (i = 0;i < my_input.length();++i)
        {
            if (the_string[the_index + i] != my_input[i])
            {
                tmp_match = false;
                break;
            }
        }
    if (tmp_match == true)
        {
        the_index += my_input.length();
        return true;
        }
    }
    return false;
}
}

