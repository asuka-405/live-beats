#include "iabc/simple.h"
#include "iabc/figure.h"
#include "iabc/win_page.h"
#include "iabc/staff_info.h"

namespace iabc
{
class time_sig 
{
public:
    // METHOD: ctor
    // ARGS: scale - the scale we are to render on.
    //       fraction - the value of the numerator/denom for the
    //                   time signature
    //       int - indicates how many 1/8ths to a beat, default 2
    //       bool - indicates whether or not we use the cut time
    //              or  common time symbols.
    time_sig(const scale& the_scale,
             const fraction& the_fraction = fraction(4,4),
             int the_eights_to_beat = 2,
             bool the_use_symbols = true);
    ~time_sig();
    void add_to_page(figure_container_if& w,window& the_page,const staff_info& the_origin);
    int get_width(window& the_window);
    int get_x_offset(window& the_window);
    fraction get_time() const{return my_time;};
private:
    void make_figures(window& the_window);
    fraction my_time;
    int my_eighths_to_beat;
    bool my_use_symbols;
    scale my_scale;
    figure* my_time_figure;
    text_figure* my_num_figure;
    text_figure* my_den_figure;
    int my_width;
    int my_offset;
};
}
