#ifndef iabcstring_h
#define iabcstring_h
// #include <iostream>
#ifdef GCC
typedef unsigned size_t;
#endif
#include "iabc/array.h"
#include "iabc/factory.h"

#define debug_strings 0
#if debug_strings
#include "iabc/list.h"
#endif
namespace iabc {
;

// iostream

//	Description:
//	A smart pointer to a char* used to hande text.  A deep
//	copy is always done so the emphasis is on effeciency of
//	programming rather than efficiency of the program.
class string 
{
  public:
    typedef enum precision_enum {precision=6} precision_enum;

      //	Description:
      //	Makes an empty string "\0"
      string ();

      
      //	Description:
      //	Construct a string from a char*
      //
      //	Example:
      //	string s="AB";  // "AB"
      string (const char* the_other);

      
      //	Description:
      //	copy constructor
      string (const string& the_other);

      
      //	Description:
      //	Construct a string representation of a signed integer
      //
      //	Example:
      //	string s=-100;  // "-100"
      string (long the_num);

      
      //	Description:
      //	Turn an unsigned long into a string of itself.
      //
      //	Example:
      //	string '
      string (unsigned long the_ul);

      
      //	Description:
      //	String from a double
      //
      //	Example:
      //	string s= -3.14159; //"-3.14159"
      string (double the_float);

      
      //	Description:
      //	Construct a string from a character
      //
      //	Example:
      //	string s='A';  // "A"
      string (char the_char);

      
      //	Description:
      //	Construct a string from an string that may not be NULL
      //	terminated, but we know its size.
      //
      //	Type:
      //	common
      //
      //	Parameters:
      //	char*
      //	int
      //
      //	Returns:
      //	NA
      string (const char* the_buf, long the_size);

    
      
      //	Description:
      //	Delete the representation.
      ~string ();


      // METHOD: chop
      // DESCRIPTION:
      // remove the last character
      void chop();
      
      //	Description:
      //	Assignment operator
      string& operator = (const string& the_other);

      
      string& operator = (const char* the_other);

      
      string& operator = (const long the_other);

      
      string& operator = (const double the_other);

      string& assign (const char* the_buf, int the_size);
      
      const char& operator [] (int the_num) const;
      char& operator [] (int the_num);

      
      //	Description:
      //	Emulate the BASIC 'Mid' function, which creates a string
      //	based on an offset into an existing string.
      //
      //	Example:
      //	string tmpS="1234567"; //'1234567'
      //
      //	string tmpT = tmpS.Mid(1,3);
      //	         //'234'
      string mid (int the_start, int the_length) const;

      //	Description:
      //	Reverses the string or characters
      void reverse ();

      //	Description:
      //	like strcmp.
      bool operator == (const string& the_other) const;

      
      bool operator != (const string& the_other) const;

      
      //	Description:
      //	Concatenation operator
      //
      //	Example:
      //	string s1="AB";  // AB
      //	string s2="CD"; // CD
      //	string s3 = s1 + s2;  //"ABCD"
      string operator + (string the_other) const;


      // Description:
      // Split on the pattern, like Perl does. e.g.
      // string s = ab:dd:ee;
      // s.split(":"); // returns array or 3 strings:
      //                     "ab" "dd" "ee"
      array<string> split (const string& the_pattern) const;

      //	Description:
      //	A = "ab";
      //	B = "cd";
      //	A += B;  // A = "abcd"
      string& operator += (const string& the_other);

      
      //	Description:
      //	Returns true if theOther is alphabetically less thanme.
      bool operator > (const string& the_other) const;
      bool operator >= (const string& the_other) const
          {return ((*this == the_other) || (*this > the_other));};

      
      //	Description:
      //	Returns true if theOther is alphabetically more than me.
      bool operator < (const string& the_other) const;
      bool operator <= (const string& the_other) const
          {return ((*this == the_other) || (*this < the_other));};

      
      //	Description:
      //	Converts the value of the string to a long.
      long as_long () const;

      
      //	Description:
      //	Converts to a double
      double as_double () const;

      // Description:
      // Converts to a bool from 'true' string or '0' or '1'.
      bool as_bool () const;
      
      //	Description:
      //	Return the length of the string.
      int length () const;

      //	Description:
      //	Convenience function to turn an unsigned long into hex
      //	string.
      //
      //	Example:
      //	string tmpS = string::ToHex(65535);
      //	 // "0x0000ffff"
      //
      //	Type:
      //	static
      //
      //	Parameters:
      //	unsigned long
      //
      //	Returns:
      //	string
      static string to_hex (unsigned long the_num);

      //	Description:
      //	Convert to lower case
      //
      //	Type:
      //	common
      //
      //	parameters:
      //	none
      //
      //	Returns:
      //	string& - itself
      string& to_lower ();

      
      //	Description:
      //	Convert to upper case
      //
      //	Type:
      //	common
      //
      //	parameters:
      //	none
      //
      //	Returns:
      //	string& - itself
      string& to_upper ();

      
      //	Description:
      //	Allocate and return a copy of myRep.  The client must
      //	delete the memory.
      char* allocate_char_array () const;

      
      //	Description:
      //	The standard output operator for a string.
      //
      //	Type:
      //	common
      //
      //	Parameters:
      //	none
      //
      //	Returns:
      //	the ostream&
      //friend std::ostream& operator << (
	  //std::ostream& os, 
	  //	  const iabc::string& the_string);

      
      //	Description:
      //	Read in data from the string and put it into the string
      //	for me.
      //
      //	Type:
      //	common
      //
      //	Parameters:
      //	none
      //
      //	Returns:
      //	void
      // friend std::istream& operator >> (std::istream& is, string& the_string);

      
      //	Description:
      //	Remove leading and trailing whitespace from the string.
      //
      //	Type:
      //	common
      //
      //	Parameters:
      //	none
      //
      //	Returns:
      //	void
      void strip_white_space ();

      
      //	Description:
      //	Returns a pointer to a char * representation of the
      //	string. This pointer is not guaranteed to remain valid
      //	after later operations on the string. This is
      //	intended for immediate consumption by code which will
      //	copy the contents of the string. The pointer must NOT be
      //	deleted by the caller. It points to a buffer maintained
      //	by the string object itself and the corresponding
      //	memory will be deleted by the string at the
      //	appropriate time.
      //	Example:
      //	printf("There are %d cards in terminal %s\n", tmpCount,
      //	tmpTermName.AsString());
      //
      //	Type:
      //	common
      //
      //	Parameters:
      //	none
      //
      //	Returns:
      //	const char const *
      const char * access_char_array () const{return (const char*)my_rep;};

      // METHOD: clear
      // DESCRIPTION:
      // Clear out the string
      void clear();
    // Data Members for Class Attributes
    static instance_counter<string> the_number_instances;
  public:
    // Additional Public Declarations
      
      

  protected:
    // Additional Protected Declarations
      
      

  private:
    // Data Members for Class Attributes
      void get_chars(size_t the_chars);
      void release_chars();
      array<char> my_rep;
      int my_size;
    // Additional Private Declarations
  private: 
    // Additional Implementation Declarations
      
      

};




// Class string 


//extern ostream& operator << (ostream& os, const string& theString);

#if debug_strings
extern list<string*>&
all_strings();
#endif

}

#endif

