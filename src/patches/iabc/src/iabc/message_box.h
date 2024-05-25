#ifndef iabc_message_box_h
#define iabc_message_box_h
#include "iabc/string.h"
#include "iabc/winres.h"
#include "iabc/drawtemp.h"

namespace iabc
{
;

class message_box
{
public:
    friend class draw_command<message_box,string>;
    static void display(const string& the_string);
  void add_ref(){};
  void remove_ref(){};
 private:
    void private_display(const string& the_string);
    void draw_self(window& w,const string& the_string); 
};

typedef draw_command<message_box,string> draw_text_command;

}

#endif



