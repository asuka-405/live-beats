#ifdef _MSC_VER
#include "stdafx.h"
#endif
#include <stdio.h>


#include "iabc/string.h"
#include "iabc/map.cpp"
#include "iabc/array.cpp"
#include "iabc/list.cpp"

namespace iabc
{
;

INSTANTIATE_MAP(int,string);
template class list<int>;

int do_main(int argc, char* argv[])
{
	array<char>*  ar = new array<char>;
	ar->add_to_end('a');
	ar->expand_to(10);
	delete ar;

	string* tmp_sar = new string[5];
	delete [] tmp_sar;

    list<int> ilist;
    list<int>::iterator tmp_iit = ilist.first();

    map<int,string>* plist = new map<int,string>;
    map<int,string>& str_list = (*plist);

    string one("one");
    string two("two");
    string three("three");

	string foo1;
	foo1 = one;
	foo1 += two;

    str_list.add_pair(1,one);
    str_list.add_pair(3,three);
    str_list.add_pair(2,two);

    map<int,string>::iterator it = 
        str_list.get_item(2);

    printf("\nexactly 2 is ");
    if (it)
    {
        printf("%s",(*it).value.access_char_array());
    }
    else
        printf("\n<null>,error!");

    it = str_list.get_item(2,lteq);
    printf("\nlteq 2 is ");
    if (it)
    {
        printf("%s",(*it).value.access_char_array());
    }
    else
        printf("\n<null>,error!");

    it = str_list.get_item(2,lt);
    printf("\nlt 2 is ");
    if (it)
    {
        printf("%s",(*it).value.access_char_array());
    }
    else
        printf("\n<null>,error!");

    printf("\nAfter copy");
    map<int,string> copy = str_list;

    it = copy.get_item(2,gt);
    printf("\ngt 2 is ");
    if (it)
    {
        printf("%s",(*it).value.access_char_array());
    }
    else
        printf("\n<null>,error!");

    it = copy.get_item(2,gteq);
    printf("\ngteq 2 is ");
    if (it)
    {
        printf("%s",(*it).value.access_char_array());
    }
    else
        printf("\n<null>,error!");

    it = copy.get_item(3,gt);
    if (it)
    {
        printf("\nerror!");
    }
    else
        printf("\nnothing >3");

    delete plist;

	return 0;
}
}

int main(int argc, char* argv[])
{
	return iabc::do_main(argc,argv);
}
