#ifndef iabc_staff_info_h
#define iabc_staff_info_h
#include "iabc/wd_data.h"
#include "iabc/pitch.h"

class feature;
namespace iabc
{
class staff_info
{
public:
    staff_info(){};
    staff_info(int the_horizontal_position,
               int the_vertical_position,
               const sizeloc& the_size,
               const pitch& the_center_pitch)
    {
        horizontal_position = the_horizontal_position;
        vertical_position = the_vertical_position;
        size = the_size;
        center_pitch = the_center_pitch;
    };
    staff_info(const staff_info& o)
    {
        horizontal_position = o.horizontal_position;
        vertical_position = o.vertical_position;
        size = o.size;
        center_pitch = o.center_pitch;
    };

    staff_info& operator=(const staff_info& o)
    {
        horizontal_position = o.horizontal_position;
        vertical_position = o.vertical_position;
        size = o.size;
        center_pitch = o.center_pitch;
    };

    ~staff_info(){};

    int horizontal_position;
    int vertical_position;
    sizeloc size;
    pitch center_pitch;
};
}
#endif

