#ifndef iabc_percentage_meter_h
#define iabc_percentage_meter_h

#include "iabc/string.h"
#include "iabc/map.h"

namespace iabc
{
;
class percentage_meter
{
public:
    typedef enum operation
    {
        initialize_gui = 0,
        update_gui = 1,
        destroy_gui = 2
    } operation;
    bool is_cancelled(){return my_is_cancelled;};
    void set_message(const string& the_string);
    void set_percentage(double the_number);
    static percentage_meter* create();
    void add_ref(){};
    void remove_ref(){};
    static void destroy(percentage_meter& the_meter);
protected:
    virtual void update(operation the_op) = 0;
    string my_message;
    double my_value;
    bool my_visible;
    percentage_meter();
    virtual ~percentage_meter();
    bool my_is_cancelled;
};

}

#endif


