#ifndef iabc_window_feature_h
#define iabc_window_feature_h
#include "iabc/music_info.h"
#include "iabc/wd_data.h"
#include "iabc/window_figure.h"

namespace iabc
{
class window_feature:public feature
{
public:
    window_feature(const fraction& the_duration,int the_channel):
        feature(the_duration,the_channel){};
    virtual ~window_feature(){};
    virtual window_figure get_figure(window& w,const staff_info the_origin) = 0;
protected:
    window_figure* my_note_figure;
};

}
#endif

