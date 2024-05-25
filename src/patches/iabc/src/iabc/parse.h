#ifndef iabc_parse_h
#define iabc_parse_h

#include "iabc/scan.h"

// Define this if you want to debug the parser in a 
// console app
extern bool printrules;

// abc is a really lousy language for parsing, really.  There is no concept
// of a 'block' and so pretty much any rule can show up anywhere.  This 
// makes parsing large files extremely slow.
namespace iabc
{

// A rule is a rule in a parser.  There are essentially 2 types of rules:
// terminals and non-terminals.  Rules can be and'd and or'd together to 
// produce more complex rules.
class rule
{
public:
    rule(const char* the_string = "rule: ");
    virtual ~rule();

    // return true if the_string[the_index] matches this rule
    virtual bool match(const string& the_string,int& the_index) = 0;

    // This can produce verbose strings for debugging purposes
    virtual string get_description() const{return my_description;};

    // METHOD: pre_match
    // DESCRIPTION:
    // This is useful for debugging, since the match method of most
    // of the classes will be overridden, this will allow you to
    // debug other classes by overriding this method.
    virtual void pre_match(const string& the_string,int the_index){};

    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset() = 0;
protected:
    // for debugging
    static int the_instances;
    const char* my_description;
private:
};

// CLASS: null_rule
// DESCRIPTION:
// A null rule always matches.  It is handy when optimizing 
// the sequence of rules in an 'or' and making optional params.
class null_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    null_rule():rule("null_rule"){};
    virtual ~null_rule(){};
    virtual bool match(const string &the_string, int &the_index) 
    {
        return true;
    }
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset() {
    };
};

// CLASS: loser_rule
// DESCRIPTION:
// A loser rule is like a null rule, except that it always fails.
class loser_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    virtual bool match(const string &the_string, int &the_index) 
    {
        return false;
    }
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    virtual void reset() {
    }
};

// A literal is a literal string, like '['.  A literal
// is like a terminal (below), but there's really nothing
// to 'convert'
extern const char* literal_string;

class literal:public rule
{
public:
    literal(const string& the_string):
        my_matched(false),
        my_string(the_string)
        ,rule(literal_string)
             {};
    virtual ~literal(){};
    virtual bool match(const string& the_string,int& the_index);
    bool is_match(){return my_matched;};
    virtual void reset();
    virtual string get_description() const;
protected:
    string my_string;
    bool my_matched;
};

// CLASS: terminal
// DESCRIPTION:
// A terminal is a regular expression, like a variable name
class terminal:public rule
{
public:
    // Match will try to match the regular expression in my_pattern.  If
    // there is a match, it will call convert with the matched string,
    // which will then be converted to some type of value.
    bool match(const string& the_string,int& the_index);

    // The derived class provides the expression in the costructor,
    // and then defines the 'convert' function.  The base class will
    // call this method if there is a match.
    virtual void convert(const string& the_string) = 0;
    terminal(const string& the_pattern,bool the_greedy = true);
    virtual ~terminal();
    bool is_match() const{return my_matched;};
    virtual void reset();
    virtual string get_description() const;
protected:
    regexp* my_pattern;
    
    // Keep a list of regexp so we don't have to keep making new ones...
    typedef list<regexp*> regexp_list;
    // static map<string,regexp_list> the_regexp_map;
    // static regexp* get_unused_exp(const string& the_string);
    // static void recycle_exp(terminal& the_exp);
    bool my_matched;
    string my_input;
};

// CLASS: non_terminal
// DESCRIPTION:
// A non-terminal rule is made up of other terminal or non-terminal rules.
// We want to overload the & and | operators to use them in the usual way so
// we don't make non_terminal be of type 'rule'.  Instead we make non_terminal
// its own class that can be constructed with a rule or another non_terminal.
class non_terminal
{
public:
    friend class rule;
    non_terminal():my_rule(0),my_and_or_or(and1),my_array(0,4){};
    non_terminal(rule& the_rule);
    non_terminal(const non_terminal& the_other);
    ~non_terminal();
    string get_description() const;

    // An 'or' rule match will return true if any one of the rules in
    // the 'or' chain is a match.
    // Usage:  return match(the_string,the_index,rule1 | rule2 | rule3);
    non_terminal operator|(const non_terminal& the_rule);
    // An 'and' rule match will return true if and only if all of the rules
    // in the 'and' chain match.
    // Usage:  return match(the_string,the_index,rule1 & rule2 & rule3);
    non_terminal operator&(const non_terminal& the_rule);
    static bool match(const string& the_string,int& the_index,non_terminal& the_term);
    
    // We have failed in an 'or' rule.  Notify all rules under me that the current
    // attempt at a match have failed and reset themselves.
    void reset_to_default();
    // Except for calling the match function, clients really shouldn't
    // have to call the terminal operators explicitly.  We provide these
    // operators to do the conversion automatically.
    friend non_terminal
    operator&(rule& r1,rule& r2);
    
    friend non_terminal
    operator&(rule& r1,const non_terminal& r2);
    friend non_terminal
    operator|(rule& r1,rule& r2);
private:
    void set_equal(const non_terminal& the_other);
    typedef enum and_or_or
    {
        and1,
        or1,
        terminal1
    } and_or_or;

    class reset_predicate
    {
    public:
        void operator()(non_terminal& o){o.reset_to_default();};
    };

    // When constructing and or or chains, we can create a new rule out of
    // 'this' rule and then next one
    non_terminal(const non_terminal& the_first,
                 const non_terminal& the_second,
                 and_or_or the_type);

    and_or_or my_and_or_or;

    array<non_terminal> my_array;
    rule* my_rule;
    bool my_print;
};

// CLASS: compound_rule
// DESCRIPTION:
// Some rules are made up of other rules.  When constructing these
// rules its more efficient if you just set up your parse tree
// one time during the lifetime of the rule, and then just keep
// resetting it everytime you use it.
class compound_rule:public rule
{
public:
    compound_rule(const char* the_string="");

    // METHOD: match
    // DESCRIPTION:
    // Construct the compound rule if we have to, and then run it.
    // Call convert() if there's a match and return true.
    bool match(const string &the_string, int &the_index);

    // METHOD: is_matched
    // DESCRIPTION:
    // return true if I matched last time I tried
    bool is_matched() const {return my_is_matched;};

    // METHOD: convert
    // DESCRIPTION:
    // We have found a match for the rule, which means we have matched a
    // expression in the language.  Do whatever that expression tells us to.
    virtual void convert(const string& the_string,int& the_index) = 0;
    virtual void reset();

protected:
    // METHOD: create_complete_rule
    // DESCRIPTION:
    // We create the compound rule using or and 'and' only once during the 
    // lifetime of the object to speed up the parse - its wasteful to keep
    // creating and deleting instances.
    virtual non_terminal create_complete_rule() = 0;
private:
    // ATTRIBUTE: the composite rule that I represent
    non_terminal my_complete_rule;

    // ATTRIBUTE:
    // set to true if I should create the compound rule.
    bool my_should_create;

    // ATTRIBUTE:
    // set to indicate that this rule has succeeded the last
    // time it was tried.
    bool my_is_matched;
};

// We use these templates to manage arrays of rules that are
// called recursively.  This one deletes arrays in the destructors
// of the rules.
template <class ruletp>
void
delete_rule_array(array< ruletp >& the_array)
{
    for (int i = 0;i < the_array.get_size(); ++i)
    {
        delete the_array[i];
    }
}

template <class ruletp>
void
clear_rule_array(array< ruletp >& the_array)
{
    for (int i = 0;i < the_array.get_size(); ++i)
    {
        (*the_array[i]).reset();
    }
}

// This one adds another level to the array if we are recursing to
// a new level, and also resets all the rule arrays if this is
// the first pass through this string.
template <class ruletp,class rulet>
void
setup_rule_array(int the_depth,array< ruletp >& the_array)
{
    int tmp_array_size = the_array.get_size();
    if (tmp_array_size == the_depth - 1)
    {
        the_array.expand(1);
        the_array[the_depth - 1] = new (rulet);
    }
    if (the_depth == 1)
    {
		for (int i = 0;i < the_array.get_size(); ++i)
		{
            the_array[i]->reset();
		}
    }
}


// CLASS: iterative_rule
// DESCRIPTION:
// A '1 or more' compound rule.  Calls repeat_match until the match
// fails, and return true if there was at least 1 match and
// no syntax_error was raised.
class iterative_rule:public rule
{
    // return true if the_string[the_index] matches this rule
public:
    iterative_rule(const char* the_string = "");
    virtual ~iterative_rule(){};

    // METHOD: match
    // DESCRIPTION:
    // Try repeat_match 1 more more times, then call convert
    // if there is a match.  Keep going until the match fails.
    virtual bool match(const string &the_string, int &the_index);
    
    // METHOD: reset
    // Note:  all other reset-ing is handled by the derived class in
    // the create_complete_rule part.
    virtual void reset(){my_syntax_error = false;};

    bool is_match() const {return my_is_match;};
protected:
    // CAREFUL EVERYBODY....
    // the_iteration is 1-indexed, arrays are 0 indexed.  You've been warned.

    // METHOD: create_complete_rule
    // DESCRIPTION:
    // We are iterating through the_iteration and we need to make sure that the
    // arrays have been created up to this level, if we have never been at this level
    // before.  We also need to make sure we've reset the rules at this level before
    // we try the match.
    virtual void create_complete_rule(int the_iteration) = 0;

    // METHOD: repeat_match
    // DESCRIPTION:
    // We have matched on the_iteration.  Extract any information we need from the 
    // rules before going on to the next step.
    virtual void convert(int the_iteration) = 0;

    // METHOD: repeat_match
    // DESCRIPTION:
    // This is where we call the actual match.
    bool repeat_match(const string& the_string,int& the_index,int the_iteration);
    
    // ATTRIBUTE: my_terminals
    // DESCRIPTION:
    // These are the non-terminal rules that make up this rule.  They are an array
    // such that the n'th iteration refers to the n-1th rule.  The array is populated
    // by calls to create_complete_rule.
    array<non_terminal*> my_terminals;
    bool my_syntax_error;
    bool my_stop_please;
    bool my_is_match;
private:
};

// Here are some simple rules.  We only define some
// generically useful things here; the rules for the 
// language are defined in the rules file.

// This rule is for a repeated literal, like '/////'
class repeat_rule:public terminal
{
public:
    repeat_rule(const string& the_pattern):terminal(string(the_pattern + "+")){};
    
    // A repeated literal string is distinguished by how many times its repeated.
    virtual void convert(const string &the_string)
    {my_count = the_string.length();};

    int get_value(){return my_count;};
protected:
    int my_count;
};

// A number rule is a terminal that represents a number
class number_rule:public terminal
{
public:
    number_rule(int the_default = 0);
    virtual void convert(const string &the_string) 
    {
        string tmp_string = (*my_pattern)[0];
        my_number = (int)tmp_string.as_long();
    }
    int get_value(){return my_number;};
    void reset();
protected:
    int my_number;
    int my_default;
};

// CLASS: quoted_string_rule
// DESCRIPTION:
// This matches a string delemited by matching characters.  Probably
// the most famous of these is a quoted string.
class balanced_string_rule:public terminal
{
public:
    // The default implementation is a balanced string rule
    balanced_string_rule(char the_quote_char = '\"'); 
    virtual void convert(const string &the_string) 
    {
        my_value = (*my_pattern)[1];
    }
    string get_value(){return my_value;};
private:
    string my_value;
    char my_quote;
};

#define quoted_string_rule balanced_string_rule

// CLASS: word_rule
// DESCRIPTION:
// A word, in other words not spaces
class word_rule:public terminal
{
public:
    word_rule():terminal("([0-z]+)"){};
    void convert(const string& the_string)
    {
        my_value = (*my_pattern)[1];
    }
    string get_value(){return my_value;};
private:
    string my_value;
};

// This can be used to skip spaces in an expression.
class whitespace:public terminal
{
    // The derived class provides the expression in the costructor,
    // and then defines the 'convert' function.  The base class will
    // call this method if there is a match.
public:
    whitespace():terminal(" +"){};
    virtual void convert(const string &the_string){};
};

class optional_whitespace:public terminal
{
    // The derived class provides the expression in the costructor,
    // and then defines the 'convert' function.  The base class will
    // call this method if there is a match.
public:
    optional_whitespace():terminal(" *"){};
    virtual void convert(const string &the_string){};
};

// CLASS: key_rule
// DESCRIPTION:
// key = something, chop off the '=' part
class nv_key_rule:public terminal
{
public:
    nv_key_rule();
    void convert(const string& the_string);
    string get_value(){return my_value;};
private:
    string my_value;
};

// CLASS: nv_rhs
// DESCRIPTION:
// name-value right hand side, either a word or a 
// quoted string.
class nv_rhs:public compound_rule
{
public:
    nv_rhs();
    non_terminal create_complete_rule();
    void convert(const string& the_string,int& the_index);
    string get_value(){return my_value;};
    void reset();
private:
    optional_whitespace my_whitespace;
    quoted_string_rule my_quotes;
    word_rule my_word;
    string my_value;
};

// CLASS: nv_pair
// DESCRIPTION:
// name-value pair, for example:
// NAME = VALUE
class nv_pair:public rule
{
public:
    virtual bool match(const string &the_string, int &the_index);
    // We have failed to match an 'or' rule, so we need to reset
    // to our default values.
    nv_pair();
    virtual ~nv_pair();
    void convert(const string& the_string,int& the_index);
    virtual void reset();
    string get_key(){return my_key;};
    string get_value(){return my_value;};
private:
    string my_key;
    string my_value;
    regexp* my_expression;
    bool my_is_match;
};


}
#endif

