// ut_parse.cpp : Defines the entry point for the console application.
//

#ifndef GCC
#include "stdafx.h"
#endif
#include "iabc/iabc_parse.h"

#include "iabc/null_media.h"
#include "iabc/map.cpp"
#include "iabc/percentage_meter.h"
#include "iabc/scan.h"

bool globalUserAbortFlag = false;

namespace iabc
{
;

const char* get_current_thread_name()
{
    return "thread_id";
}

text_buf*
factory<text_buf,const string&>::create(const string& the_string)
{
    return new file_buf(the_string);
}

class null_tune_chooser:public tune_chooser
{
    // METHOD: get_choice
    // DESCRIPTION:
    // Abstract method that gets the current tune choice from the user.
public:
    virtual int get_choice() {return 1;};
    
    // METHOD: get_choice
    // DESCRIPTION:
    // Abstract method that clears the list for new parsing.
    virtual void remove_all_tunes() {};
};

class null_percentage_meter:public percentage_meter
{
public:
    null_percentage_meter(){};
	void update(percentage_meter::operation the_operation){};
};

percentage_meter* 
percentage_meter::create()
{
	percentage_meter* tmp_meter = new null_percentage_meter;
	return tmp_meter;
}

void 
percentage_meter::destroy(percentage_meter& the_meter)
{
    delete &(the_meter);
}

class null_source:public media_source
{
public:
    null_source(tune_chooser& the_chooser,
                const string& the_filename):my_filename(the_filename),
        media_source(the_chooser)
    {
    }
    virtual void parse(media &the_media);
private:
    string my_filename;
};

void
null_source::parse(media& the_media)
{
    media_source& tmp_this = (*this);
    preprocess tmp_preprocessor(my_filename); 
    iabc_parse the_parser(tmp_preprocessor,*this);
    tune tmp_tune;
    my_tunes->add_tune(tmp_tune);
    the_parser.parse(0);
}
}

void
iabc::message_box::display(const string& the_string)
{
	if (the_string.length())
		printf(" Message box: %s \n",the_string.access_char_array());
	else
		printf("Test case passed.\n");
};

int main(int argc, char* argv[])
{
	printf("Hello World!\n");

	iabc::regexp the_expression("^ *T: *(.+)");
	bool tmp_success = the_expression.match("T: My New Tune",3);
    if (tmp_success)
		printf("match s/b bbbb: %s\n",the_expression[0].access_char_array());

    using namespace iabc;
    null_tune_chooser tmp_chooser;
    measure_or_header* tmp_measure = new measure_or_header;
    delete tmp_measure;
    tmp_measure = new measure_or_header;
    delete tmp_measure;
    tmp_measure = new measure_or_header;
    delete tmp_measure;
    tmp_measure = new measure_or_header;
    delete tmp_measure;

	iabc::null_source tmp_parser(tmp_chooser,(string)"./sample.abc");
	iabc::null_media tmp_media;

    tmp_parser.parse(tmp_media);

	return 0;
}

