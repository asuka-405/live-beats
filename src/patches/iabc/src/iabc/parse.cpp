/*
 * parse.cpp - Generic parser and base classes for parser rules.
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
#include "iabc/parse.h"
#include "iabc/array.cpp"
#include "stdio.h"
#include "iabc/map.cpp"

namespace iabc
{
;
// WARNING:
// This is for debugging - but it will slow the program down
// a lot if you set it!
bool printrules = false;

int rule::the_instances = 0;

rule::rule(const char* the_string)
{
    ++the_instances;
    if (printrules || debug_strings)
    {
        my_description = the_string;
    }
}

rule::~rule()
{
    --the_instances;
}
// rule & rule: return a non_terminal
// containing the 2 rules in an array.
non_terminal
operator&(rule& r1,rule& r2)
{
	non_terminal tmp_t1(r1);
	non_terminal tmp_t2(r2);
    return non_terminal(tmp_t1 & tmp_t2);
}


non_terminal
operator&(rule& r1,const non_terminal& r2)
{
	non_terminal tmp_t1(r1);
	non_terminal tmp_r2(r2);
    tmp_t1.my_and_or_or = non_terminal::and1;

    // If this is the same 'and'-ness, make it the same level
    if (tmp_r2.my_and_or_or == non_terminal::and1)
    {
        tmp_r2.my_array.add_to_end(tmp_t1);
        return tmp_r2;
    }
    return non_terminal(tmp_t1 & r2);
}

// We do the same with the 'or' rules
non_terminal
operator|(rule& r1,rule& r2)
{
	non_terminal tmp_t1(r1);
	non_terminal tmp_t2(r2);
    return non_terminal(tmp_t1 | tmp_t2);
}

non_terminal::~non_terminal()
{
}

// Called by = operator and copy constructor
void
non_terminal::set_equal(const non_terminal& the_other)
{
    my_array = the_other.my_array;
    my_and_or_or = the_other.my_and_or_or;
    my_rule = the_other.my_rule;
    my_print = the_other.my_print;
}

non_terminal::non_terminal(rule& the_rule):
my_rule(0),my_and_or_or(non_terminal::terminal1),
    my_print(printrules),my_array(0,4)
{
    my_rule = &the_rule;
}

non_terminal::non_terminal(const non_terminal& the_other)
{
    set_equal(the_other);
}

// This combines 2 expressions into 1.
non_terminal::non_terminal(const non_terminal& the_first,
                 const non_terminal& the_second,
				 non_terminal::and_or_or the_type):
my_and_or_or(the_type),
    my_rule(0),my_print(printrules)
{
    my_array.expand(2);
    my_array[0] = the_first;
    my_array[1] = the_second;
}

// With an 'or' chain, we just tack the
// new expression on the end.
non_terminal 
non_terminal::operator|(const non_terminal& the_rule)
{
    return non_terminal(*this,the_rule,or1);
}

non_terminal 
non_terminal::operator&(const non_terminal& the_rule)
{
    return non_terminal(*this,the_rule,and1);
}

string
non_terminal::get_description() const
{
    // If I am a terminal, print out what I'm a terminal or.
    if (my_rule != 0)
    {
        return my_rule->get_description();
    }
    else
    {
        // otherwise, say what I consist of.
        int i;
        string tmp_s = "(";
        string tmp_symbol = ((my_and_or_or == and1) ? "_and_" : "_or_");
        tmp_s += my_array[0].get_description();
        for (i = 1;i < my_array.get_size(); ++i)
        {
            tmp_s += (string)" " + tmp_symbol + (string)" ";
            tmp_s += my_array[i].get_description();
        }
        tmp_s += ")\n";

        return tmp_s;
    }
}

void 
non_terminal::reset_to_default()
{
    non_terminal::reset_predicate tmp_pred;
    do_to_all(my_array,tmp_pred);
    if (this->my_rule != 0)
    {
        my_rule->reset();
    }
}

bool 
non_terminal::match(const string& the_string,int& the_index,non_terminal& the_term)
{
	static int match_depth = 0;
	match_depth++;
    int i;
	if (the_string.length() < the_index)
		return false;

    if (the_term.my_rule != 0)
    {
        // We have recursed down to the point where we are evaluating a terminal.
        // Return whether or not it matched.
        if (the_term.my_print)
        {
            printf("trying rule %s with %s and %d = '%c'\n",
                the_term.get_description().access_char_array(),
                   the_string.access_char_array(),the_index,
				   the_string[the_index]);
        }
		match_depth--;
        the_term.my_rule->pre_match(the_string,the_index);
        return the_term.my_rule->match(the_string,the_index);
    }

    // We are a complex expression.  Start parsing myself
    bool tmp_rv = false;
    int tmp_last_index = the_index;
    for (i = 0;i < the_term.my_array.get_size(); ++i)
    {
        non_terminal& tmp_rule = the_term.my_array[i];

        // try to match the first rule
        tmp_rv = match(the_string,the_index,tmp_rule);
        if (tmp_rv == false)
        {
            if (tmp_rule.my_print)
            {
                printf("No match for %s on %s %d.\n",tmp_rule.get_description().access_char_array(),
                       the_string.access_char_array(),the_index);
            }
			tmp_rule.reset_to_default();

            // if there was a failure, reset the index into the string
            // no matter what
            the_index = tmp_last_index;

            // if this is an 'and' and we've failed, then we're done.
            // Otherwise we continue.
            if (the_term.my_and_or_or == and1)
            {
				match_depth--;
                return tmp_rv;
            }
        }
        else
        {
            if (tmp_rule.my_print)
            {
                printf("Matched rule %s on %s %d.\n",
                       tmp_rule.get_description().access_char_array(),
                       the_string.access_char_array(),the_index);
            }
            // if this is an 'or' and we've succeeded then we're done
            if (the_term.my_and_or_or == or1)
            {
                if (the_term.my_print)
                {
                    printf("Match of 'or' rule %s succeeded with %s and %d\n",
                        the_term.get_description().access_char_array(),
                           the_string.access_char_array(),the_index);
                }
				match_depth--;
                return tmp_rv;
            }
        }
    }

	match_depth--;
	return tmp_rv;
}

compound_rule::compound_rule(const char* the_string):
rule(the_string),
my_should_create(true),
my_is_matched(false)
{
}

void
compound_rule::reset()
{
    my_is_matched = false;
}

bool 
compound_rule::match(const string &the_string, int &the_index)
{
    if (my_should_create == true)
    {
        my_complete_rule = create_complete_rule();
		my_should_create = false;
    }
    
    if (non_terminal::match(the_string,the_index,my_complete_rule) == true)
    {
        my_is_matched = true;
        convert(the_string,the_index);
    }
    else
    {
        my_is_matched = false;
    }

    return my_is_matched;
}

iterative_rule::iterative_rule(const char* the_string)
:my_syntax_error(false),
my_stop_please(false),
my_is_match(false),
rule(the_string)
{
}

bool 
iterative_rule::repeat_match(const string& the_string,int& the_index,int the_depth)
{
    return non_terminal::match(the_string,the_index,(*my_terminals[the_depth - 1]));
}

bool 
iterative_rule::match(const string &the_string, int &the_index)
{
    bool tmp_rv = false;
    
    int tmp_depth = 1;
    my_stop_please = false;
    int tmp_last_index = the_index;

    do
    {
        tmp_last_index = the_index;
        create_complete_rule(tmp_depth);
        tmp_rv = repeat_match(the_string,the_index,tmp_depth);

        if (tmp_rv == true)
        {
            convert(tmp_depth);
            tmp_depth++;
        }
    } 
	while ((tmp_rv) &&
           (my_stop_please == false) &&
           (my_syntax_error == false) &&
           (the_index > tmp_last_index));

    // Since this is a '1 or more' rule, return true if we have at
    // least 1 match.  A syntax error can be raised in 'convert' if
    // we see something that we recognize as being wrong.  If we just
    // don't recognize it than the match will fail.
	my_is_match = ((tmp_depth > 1) && (my_syntax_error == false));
    return my_is_match;
}

const char* literal_string = "literal: ";

// A literal is a terminal in the ABC language, such as the ':' delimiter
bool 
literal::match(const string& the_string,int& the_index)
{
    string tmp_string = the_string.mid(the_index,my_string.length());
    my_matched = (my_string == tmp_string);
    if (my_matched)
    {
        the_index += tmp_string.length();
    }
    return my_matched;
}

string
literal::get_description() const
{
    return (string)my_description + my_string;
}
void 
literal::reset()
{
    my_matched = false;
}

balanced_string_rule::balanced_string_rule(char the_quote_char):
my_quote(the_quote_char),
terminal((string(the_quote_char) + string("(.+)") + string(the_quote_char)),false)
{
}


//map<string,terminal::regexp_list>
//terminal::the_regexp_map;

const char* terminal_string = "terminal: ";

terminal::terminal(const string& the_pattern,bool the_greedy):
my_matched(false),my_input(the_pattern)
,rule(terminal_string)
{
    // So we have this fancy garbage collector and we're not using it.
    // The reason is that we generally create terminals as part of 
    // parse rules, and then we keep the parse rules around for the duration
    // or parsing, so we don't have an issue where terminals are being
    // created and destroyed all the time.  This would be useful if you
    // were doing a lot of arbitrary expression parsing, like in an
    // interpreter.
	//my_pattern = get_unused_exp(the_pattern);

	//if (my_pattern == 0)
	my_pattern = new regexp(the_pattern,the_greedy);
}

terminal::~terminal()
{
    delete my_pattern;
	//my_pattern->reset();
	//recycle_exp(*this);
}

#if 0
regexp*
terminal::get_unused_exp(const string& the_string)
{
    map<string,regexp_list>::iterator tmp_pair = 
		the_regexp_map.get_item(the_string,exact);

    if (tmp_pair)
    {
        regexp_list::iterator tmp_iterator = (*tmp_pair).value.first();
        if (tmp_iterator)
        {
            regexp* rv = *tmp_iterator;
            tmp_iterator.delete_current();
            return rv;
        }
    }

    return 0;
}

void 
terminal::recycle_exp(terminal& the_exp)
{
    map<string,regexp_list>::iterator tmp_pair = 
		the_regexp_map.get_item(the_exp.my_input,exact);

    if (tmp_pair)
    {
        regexp_list::iterator tmp_iterator = (*tmp_pair).value.first();
        tmp_iterator.add_before(the_exp.my_pattern);
    }
    else
    {
        regexp_list tmp_list;
        regexp_list::iterator tmp_it = tmp_list.first();
        tmp_it.add_after(the_exp.my_pattern);
        the_regexp_map.add_pair(the_exp.my_input,tmp_list);
    }
}
#endif

void
terminal::reset()
{
    my_pattern->reset();
    my_matched = false;
}

string 
terminal::get_description() const
{
    string tmp_rv = (string)my_description;
    tmp_rv += (string)" " + my_input;
    return tmp_rv;
}

// A terminal is any variable-sized regular expression.
bool
terminal::match(const string& the_string,int& the_index)
{
	const char* tmp_s = &((the_string.access_char_array())[the_index]);
    my_pattern->match(tmp_s,the_string.length() - the_index);
    the_index += (*my_pattern)[0].length();
    if (my_pattern->is_match())
    {
        my_matched = true;
        convert((*my_pattern)[0]);
    }
    else
    {
        my_matched = false;
    }

    return my_matched;
}

number_rule::number_rule(int the_default):
terminal("\\d+"),
my_number(the_default),
my_default(the_default)
{
}

void
number_rule::reset()
{
    my_number = my_default;
    terminal::reset();
}

nv_key_rule::nv_key_rule():terminal("([0-z]+)=")
{
}
void 
nv_key_rule::convert(const string& the_string)
{
	string tmp_match = (*my_pattern)[1];
	tmp_match.chop();
    my_value = tmp_match;
}

nv_rhs::nv_rhs()
#if printrules
:compound_rule("nv_rhs")
#endif
{
}

non_terminal 
nv_rhs::create_complete_rule()
{
  non_terminal tmp_qw = (my_quotes & my_word);
  return non_terminal(my_whitespace & tmp_qw);
}

void 
nv_rhs::convert(const string& the_string,int& the_index)
{
    if (my_quotes.is_match() == true)
    {
        my_value = my_quotes.get_value();
    }
    else
    {
        my_value = my_word.get_value();
    }
}

void 
nv_rhs::reset()
{
    my_quotes.reset();
    my_word.reset();
    my_whitespace.reset();
    my_value.clear();
}

nv_pair::nv_pair():my_is_match(false)
#if printrules
,rule("nv_pair")
#endif
{
    my_expression = new regexp(" *([0-z]+) *= *([!-~]+)");
}

nv_pair::~nv_pair()
{
    delete my_expression;
}

bool 
nv_pair::match(const string& the_string,int& the_index)
{
    bool tmp_rv = false;
    if (the_string.length() - the_index > 0)
        {
        tmp_rv = my_expression->match(&(the_string[the_index]),the_string.length() - the_index);
        if (tmp_rv)
            {
            the_index += (*my_expression)[0].length();
            my_key = (*my_expression)[1];
            my_value = (*my_expression)[2];
            }
        }
    return tmp_rv;
}

void 
nv_pair::reset()
{
    my_expression->reset();
}

}
