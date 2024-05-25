// ut_string.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "iabc/string.h"
#ifndef GCC
#include "stdafx.h"
#endif
#include "iabc/array.cpp"

namespace iabc
{
	void do_main();
}

int main(int argc, char* argv[])
{
	iabc::do_main();
	return 0;
}
namespace iabc
{
void do_main()
{
	string s1;
    s1 = "s1";
    printf("string from = const char* = %s",
           s1.access_char_array());

    string s1_copy(s1);
    printf("string from other string %s\n",s1_copy.access_char_array());

    string s2("s2");
    printf("string from const char* = %s\n",
           s2.access_char_array());

    string slong((long)65535);
    printf("string fromlong %s\n",slong.access_char_array());

    string sulong((unsigned long) 0xa5a5);
    printf("string from unsigned %s\n",sulong.access_char_array());

    string sdouble(3.14159);
    printf("string from double %s\n",sdouble.access_char_array());

    string schar('a');
    printf("string from char %s\n",schar.access_char_array());

    string tmp_buf("123456",3);
    printf("s/b 123 %s\n",tmp_buf.access_char_array());
    
    tmp_buf.chop();
    printf("s/b 12 %s\n",tmp_buf.access_char_array());

    printf("6 different ways to assign:\n");
    
    s1 = s2;
    printf("s/b s2 %s\n",s1.access_char_array());

    // Test where a string buffer is reused.
    s1 = " a long string";
    s1 = s2;
    printf("s/b s2 %s\n",s1.access_char_array());

    s1 = "s1";
    printf("s/b s1 %s\n",s1.access_char_array());

	s1.assign("abcdefg",1);
    printf("s/b a %s\n",s1.access_char_array());

	s1.assign("abcdefg",7);
    printf("s/b abcdefg %s\n",s1.access_char_array());

    s1 = (long)32767;
    printf("s/b 32767: %s\n",s1.access_char_array());

    s1 = 3.14159;
    printf("s/b 3.14159: %s\n",s1.access_char_array());

    s1[1] = '+';
    printf("Change the . to a +: %s\n",s1.access_char_array());

    printf("s/b '+': %c\n",s1[1]);

    s1[1] = '.';

    string smid1 = s1.mid(0,2);
    string smid2 = s1.mid(2,s1.length() - 2);
    printf("mid test, s/b 3.14159: %s%s\n",smid1.access_char_array(),
           smid2.access_char_array());

    s1 = "Heffalumps and woozles and other ferocious beasts\n";
    printf("Here's a kind of long string %s\n",s1.access_char_array());

    s2 = s1;
	s2.reverse();
    printf("Heres the same thing reversed: \n%s\n",
           s2.access_char_array());

    s1 = "s1";
    s2 = "s2";
    string s3 = s1 + s2;
    printf("Here's concatenation (+): %s + %s = %s\n",
           s1.access_char_array(),s2.access_char_array(),
           s3.access_char_array());

    string s2_copy(s2);
    bool s2_equality = (s2 == s2_copy);
    bool s1_equality = (s1 == s2);

    printf("%s == %s is %d, %s == %s is %d",
           s2_copy.access_char_array(),s2.access_char_array(),(int)s2_equality,
           s1.access_char_array(),s2.access_char_array(),(int)s1_equality);
    
    bool s2_inequality = (s2 != s2_copy);
    bool s1_inequality = (s1 != s2);

    printf("%s != %s is %d, %s != %s is %d",
           s2_copy.access_char_array(),s2.access_char_array(),(int)s2_inequality,
           s1.access_char_array(),s2.access_char_array(),(int)s1_inequality);

    s1 += s2;
    printf("s1 += s2 is %s\n",s1.access_char_array());

    string abc = "abc";
    string abc1 = abc;
    string def = "def";
    string def1 = def;

    bool agtd = (abc > def);
    bool dgta = (def > abc);
    printf("%s > %s = %d, %s > %s = %d",
           abc.access_char_array(),def.access_char_array(),(int)agtd,
           def.access_char_array(),abc.access_char_array(),(int)dgta);

    agtd = (abc >= abc);
    dgta = (def <= abc);
    printf("%s >= %s = %d, %s <= %s = %d\n",
           abc.access_char_array(),abc.access_char_array(),agtd,
           def.access_char_array(),abc.access_char_array(),dgta);


    string s100 = (long)100;
    string spi = 3.14159;

    printf("%s as long = %d,%s as double = %f\n",
           s100.access_char_array(),s100.as_long(),
		   spi.access_char_array(),spi.as_double());

    string hexnum = string::to_hex(0xbaadf00d);
    printf("baadf00d as hex is %s\n",hexnum.access_char_array());
    
    string SHOUT = "SHOUT";
    string shout = SHOUT.to_lower();
    printf("%s.to_lower() is %s\n",
           SHOUT.access_char_array(),shout.access_char_array());

    string whisper = "whisper";
    SHOUT = whisper.to_upper();
    printf("%s.to_upper() is %s\n",
           whisper.access_char_array(),SHOUT.access_char_array());

    array<char> ar1;
    ar1.expand(3);
    ar1[1] = 'i';
    ar1[0] = 'h';
    ar1[2] = (char)0;
	
	array<char> ar2 = ar1;

	char* c = ar1;
    printf("array, ar = %s\n",c);

}
}

