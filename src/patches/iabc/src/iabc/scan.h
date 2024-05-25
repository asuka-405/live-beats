#ifndef IABC_SCAN_H
#define IABC_SCAN_H

#include "iabc/array.h"
#include "iabc/list.h"
#include "iabc/string.h"
#include "iabc/factory.h"

namespace iabc
{
;
// FILE: scan.h
// DESCRIPTION:
// All the class needed to implement a semi-functional, extremely slow
// regular expression scanner
//
// FUNCTION: state_initializers
// DESCRIPTION:
// In order to get all of the state machine creators created we need
// to explicitly reference them somewhere.  Do it here, for the linker's
// benefit.
void state_initializers();

// CLASS: scan_state
// DESCRIPTION:
// We will construct a state machine to implement our regular expression scanner.
// Each state in the machine will implement the following interface.
class scan_state
{
public:
    // METHOD: scan_state (constructor)
    // DESCRIPTION:
    // We construct a scan state with 2 variables:  the previous state,
    // which could be used for things like wildcard matches, and the group
    // number, which can be used to remember previous matches
    scan_state(int the_group_number,
               bool my_is_greedy);

    virtual ~scan_state();

    // METHOD: match
    // DESCRIPTION:
    // This is the main public module for a scan state.
    // Match the_string starting at the_index.  If this state matches the
    // string at the_index, increment the_index by the size of the string we
    // matched and return true.  Otherwise the match failed, return false
    // and leave the_index unchanged.
    bool match(const char* the_string,int the_string_length,
               int& the_index,int the_length = -1);

    // METHOD: get_length()
    // Some states have a variable size.  In this case we greedily try to match the
    // longest part of the string and if the match fails at some point we reduce the
    // size of the string to match and try again.  We do this until the size of the
    // string matches the minimum size, at which point we give up.
    int get_length()
    {
        return private_get_length();
    };

    // METHOD: get_group_number
    // DESCRIPTION:
    // Parts of a regular expression can be grouped.  The groups are assigend indices
    // and can be referenced if they are all matched.  States are assigned group numbers
    // when they are created.  Return the group number we were assigned.
    int get_group_number() {return my_group_number;};

    // METHOD: get_min_length
    // DESCRIPTION:
    // return the minimum that this state can match and be satisifed.  If the remainin
    // string is less than that, we know we're done.
    int get_min_length()
    {
        return private_get_min_length();
    };

    // METHOD: get_match
    // DESCRIPTION:
    // Return the matched string, or an emptry string if no match was found.
    string get_match();
    
    // METHOD:
    // DESCRIPTION:
    // This actually implements the match in the derived class
    virtual bool match_output(const char* the_string,int the_string_length,
                              int& the_index,int the_length = -1) = 0;

    // METHOD: reset
    // DESCRIPTION:
    // Some states have internal state that must be reset before another
    // expression can be matched.  This does that.
    virtual void reset();
    
    void check_state_integrity();
    void set_zero_or_more(bool the_value){my_is_zero_or_more = true;};
    void set_one_or_more(bool the_value){my_is_one_or_more = true;}
    bool is_there_anything_i_havent_tried();
    void try_new_thing(){my_should_try_new_thing = true;};
    int get_start_index() const {return my_start_index;};
protected:
    // METHOD: private_get_length
    // DESCRIPTION:
    // return the length of the string that this state has matched.
    virtual int private_get_length() = 0;
    virtual int private_get_min_length() = 0;

    int my_group_number;
    bool my_is_one_or_more;
    bool my_is_zero_or_more;
    bool my_is_greedy;
    int my_match_count;
    bool my_has_failed_further_matches;
    bool my_should_try_new_thing;
    int my_start_index;
    int my_end_index;
    const char* my_string_ptr;
private:
	scan_state& operator=(const scan_state&);
};

// CLASS: scan_state_creator
// DESCRIPTION:
// The state machine is constructed based on an input string.  So we have
// to scan the input string before we have a state machine.  This class will
// construct a scan state based on the input string.  For example, the '\d'
// input string will construct a digit-matching scan state.
class scan_state_creator
{
public:
    friend void state_initializers();
    scan_state_creator();
    virtual ~scan_state_creator(){};

    // METHOD: create_state
    // DESCRIPTION:
    // Static method.
    // Regexp can call this to create the next state in the input string.
    static scan_state* create_state(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);

    // METHOD: match_input
    // DESCRIPTION:
    // This will return a state if the next input token on the input string matches the
    // string the scan_state that I know how to create.
    // Note when implementing this method in a derived class, make sure that you increment
    // the_index if you create a state, or you will loop forever.
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy) = 0;
protected:
    
    // This predicate returns 'true' if the state passed in can use the string
    // supplied as input.  This can be used with findit_predicate.
    class find_state_pred
    {
    public:
        find_state_pred(const string& the_string,int& the_index,
                        int the_group,bool the_is_greedy)
        :my_string(the_string),my_index(&the_index),
            my_state(0),my_group(the_group),my_is_greedy(the_is_greedy){};
        bool operator()(scan_state_creator* the_creator)
        {
            if (my_state == 0)
            {
                my_state = the_creator->match_input(my_string,*my_index,my_group,my_is_greedy);
                if (my_state != 0)
                {
                    return true;
                }
            }
            return false;
        };
        scan_state* get_state(){return my_state;};
    private:
        string my_string;
		bool my_is_greedy;
		int* my_index;
        scan_state* my_state;
        int my_group;
    };
    // This does nothing but it is used so the linker puts the objects in the 
    // list.
    void init(){};
    static list<scan_state_creator*>& all_creators()
    {static list<scan_state_creator*> the_list;return the_list;};
};

// Now we have enough information to create our regular expression scanner, which
// is really just a container for the scan_state list, plus some twisted stuff.  
// First:
// We want to allow a | b | c to match either 'a' or 'b' or 'c'.  That
// means that if the next input symbol is '|', there will be more than one
// possible next state and so we try to match them all.  We do that by creating
// a regexp instance for each possible remaining state machine, and then executing
// that from this point on.
// Second:
// We want allow the match to be grouped using '()' in the input string.  Then we
// can identify the matched groups in addition to the full matched string.
// For example ".+(one)(two)(three)" applied to "junkonetwothree", rexexp[0] will
// contain junkonetwothree, regexp[1] will contain "one", regexp[2] will contain
// "two", etc.
// Third:
// Some states actually modify the previous state.  For example, '*' in the input
// string will match 0 or more of whatever the previous state was.  So we allow
// a look-ahead of one state when constructing the state machine.
class regexp
{
public:
    typedef array<scan_state*> scan_state_array;
    typedef array<string> match_array;
    typedef array<regexp*> regexp_array;
    
    static instance_counter<regexp> class_instance_count;
    typedef enum group_state
    {
        no_group,
        group
    } group_state;

    regexp(const string& the_input,bool the_greedy = true);
	~regexp();
    bool is_valid() const{return my_valid_input_flag;};
    bool is_match() const {return my_matched_flag;};

    // return true if the_string matches my regular expression.  In
    // that case we also save the match
    bool match(const char* the_string,int the_string_length);

    // Allow us to reuse a regexp more than one time by clearing out all
    // matched strings.
    void reset();

    // Return the matched string
    string operator[](int the_index)
    {
        if (my_matches.get_size() > the_index)
        {
            return my_matches[the_index];
        }
        return string();
    };

protected:
    // This is the special constructor we use if we are in an 'or' situation
    regexp(scan_state* the_first_state,
		string& the_input,
		int the_match_index,
		group_state the_group_state,
        bool the_is_greedy);
    
    // Populate the state machines based on the following input string
    void populate_machine(const string& the_string,int the_match_index = 0,
                          regexp::group_state = regexp::no_group);

    // Try to match the rest of the string with one of my_sub_expressions
    bool match_sub_expression(const char* the_string,int the_string_length,int& the_index);

    // Set all strings to the empty string.
    void clear_all_strings();

    // We can match several strings in one expression by grouping them
    // in ().  Put the string the state matched into the correct
    // array in my_matches.
    void insert_match_in_string(int index,const string& the_match);

    // This predicate is used when we create the additional state machines.
    // due to an 'or' condition.
    class create_expression_predicate
    {
    public:
        create_expression_predicate(array<regexp*>& the_others,
                                    const string& the_string,
                                    int the_group_number,
									group_state the_group_state,
									bool the_is_greedy):
            my_string(the_string),
            my_others(&the_others),
            my_is_valid(true),
            my_group_state(the_group_state),
			my_is_greedy(the_is_greedy),
            my_group_number(the_group_number){};
        void operator()(scan_state* the_state)
        {
            my_others->expand(1);
			regexp* tmp_reg = new regexp(the_state,my_string,
				my_group_number,my_group_state,my_is_greedy);
            if (tmp_reg->is_valid() == false)
            {
                my_is_valid = false;
            }
            (*my_others)[my_others->get_size() - 1] = tmp_reg;
        };
        string my_string;
        array<regexp*>* my_others;
        int my_group_number;
        group_state my_group_state;
        bool my_is_valid;
		bool my_is_greedy;
    };

	friend class regexp::create_expression_predicate;

    // We have received an 'or' expression.  Create an expression for each of the
    // possible next states.  Return 'true' if each of these expressions is valid
    bool create_additional_expressions(list<scan_state*>& the_list,const string& the_string,
                                       int the_group_number,group_state the_group_state);

    // Get the next state and put it into an empty list.  If there are multiple states
    // seperated by '|', there will be more than one possible state machine, so 
    // put all those states in the list.
    void put_next_state_in_list(list<scan_state*>& the_list,const string& the_string,int& the_index,
                                int the_group_number);

    regexp_array my_sub_expressions;
    match_array my_matches;
    scan_state_array my_states;
    bool my_valid_input_flag;
    bool my_matched_flag;
    bool my_should_reset;
    bool my_is_greedy;
    string the_input;
};

// below are all the states that this scanner knows about.  you can add as
// many as you want, just make sure that you don't introduce any ambiguity
// into your language or people will find you difficult...

// A range of characters is specified [a-z] to match 'a' through 'z'.
class range_state:public scan_state
{
public:
    range_state(int the_low,int the_high,int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy)
        ,my_low(the_low),my_high(the_high){};
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1);
    virtual int private_get_length() {return 1;};
    virtual int private_get_min_length() {return 1;};
protected:
    char my_low;
    char my_high;
};

class range_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static range_state_creator& get_instance();
protected:
private:
};

// A digit is just 0-9
class digit_state:public scan_state
{
public:
    digit_state(int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy){};
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1);
    virtual int private_get_length(){return 1;};
    virtual int private_get_min_length(){return get_length();};
};

// A digit is matched on \d
class digit_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static digit_state_creator& get_instance();
private:
};

// A ext digit is just 0-9 . + -
class ext_digit_state:public scan_state
{
public:
    ext_digit_state(int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy){};
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1);
    virtual int private_get_length(){return 1;};
    virtual int private_get_min_length(){return get_length();};
};

// A digit is matched on \d
class ext_digit_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static ext_digit_state_creator& get_instance();
private:
};

class word_state:public scan_state
{
public:
    word_state(int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy){};
    // METHOD:
    // DESCRIPTION:
    // match A-z or 0-9 or an _
    virtual bool match_output(const char *the_string, 
                              int the_string_length, 
                              int &the_index, int the_length=-1);
    virtual int private_get_length(){return 1;};
    virtual int private_get_min_length(){return get_length();};
};

class word_state_creator:public scan_state_creator
{
    // METHOD: match_input
    // DESCRIPTION:
    // Make \w match a word character.
public:
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    friend void state_initializers();
    static word_state_creator& get_instance();
};

class non_whitespace_state:public scan_state
{
    // This actually implements the match in the derived class
public:
    non_whitespace_state(int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy){};
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1); 
    virtual int private_get_length(){return 1;};
    virtual int private_get_min_length(){return get_length();};
};

class non_whitespace_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static non_whitespace_state_creator& get_instance();
private:

};
// match the beginning of a line or the end of a line
class line_state:public scan_state
{
public:
    typedef enum line_state_state
    {
        begin,
        end
    } line_state_state;

    line_state(line_state_state the_type,int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy),my_type(the_type){};
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1);
    virtual int private_get_length(){return 0;};
    virtual int private_get_min_length(){return 0;};
private:
    line_state_state my_type;
};

// We use $ to match the end of the line and ^ the start
class line_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static line_state_creator& get_instance();
private:
};

// Wildcards match any character.
class wildcard_state:public scan_state
{
public:
    wildcard_state(int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy){};
    virtual int private_get_length() {return 1;};
    virtual int private_get_min_length() {return 1;};
    
    // Match anything, as long as there is 1 character left in the string
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1);
};

// We represent a wild card as a '.'
class wildcard_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static wildcard_state_creator& get_instance();
private:
};

// A literal is just a fixed string of arbitrary characters.  To avoid confusing
// any special characters, this should probably be the last state.
class literal_state:public scan_state
{
public:
    literal_state(const string& the_input,int the_group_number,bool the_is_greedy):
        scan_state(the_group_number,the_is_greedy),my_input(the_input){};
    virtual bool match_output(const char* the_string, int the_string_length,
                              int &the_index, int the_length=-1);
    virtual int private_get_length(){return my_input.length();};
    virtual int private_get_min_length(){return get_length();};
    virtual ~literal_state(){};
protected:
    string my_input;
};

// Multi-strings can
// be enclosed in [], and special characters can be escaped with
// '\'
class literal_state_creator:public scan_state_creator
{
public:
    friend void state_initializers();
    virtual scan_state* match_input(const string& the_string,
                                    int& the_index,
                                    int the_group_number,
                                    bool the_is_greedy);
    static literal_state_creator& get_instance();
protected:
private:
};

}

#endif


