#include "iabc/time_sig.h"
#include "iabc/figure_factory.h"
#include "iabc/text_figure.h"

namespace iabc
{
;

time_sig::time_sig(const scale& the_scale,
         const fraction& the_fraction,
         int the_eights_to_beat,
         bool the_use_symbols):
my_scale(the_scale),my_time(the_fraction),
    my_eighths_to_beat(the_eights_to_beat),
    my_use_symbols(the_use_symbols),
    my_width(0),my_num_figure(0),my_time_figure(0),
    my_den_figure(0)
{
}

time_sig::~time_sig()
{
    if (my_num_figure)
    {
        my_num_figure->remove_ref();
    }
    if (my_den_figure)
    {
        my_den_figure->remove_ref();
    }
    if (my_time_figure)
    {
        my_time_figure->remove_ref();
    }
}

void
time_sig::add_to_page(figure_container_if& the_page,window& the_window,
                      const staff_info& the_staff)
{
    make_figures(the_window);
    point tmp_middle(the_staff.horizontal_position,
                     the_staff.vertical_position + the_staff.size.my_size.y / 2);
    if (my_time_figure)
    {
        rect tmp_box = my_time_figure->get_bounding_box();
        tmp_middle.y -= tmp_box.height() / 2;
        the_page.add_figure(tmp_middle,*my_time_figure);
    }
    else
    {
        rect tmp_num_rect = my_num_figure->get_current_rect();
        rect tmp_den_rect = my_den_figure->get_current_rect();
		tmp_middle.y -= tmp_den_rect.height() / 2;
        tmp_middle.y -= my_den_figure->get_current_descent();
        tmp_middle.y -= my_num_figure->get_current_leading();
        the_page.add_text_figure(tmp_middle,*my_num_figure);
        tmp_middle.y += tmp_num_rect.height() / 2;
        the_page.add_text_figure(tmp_middle,*my_den_figure);
    }
}

int 
time_sig::get_width(window& the_window)
{
    make_figures(the_window);
    if (my_time_figure)
    {
        return my_time_figure->get_bounding_box().width();
    }
    else if (my_num_figure)
    {
        return my_num_figure->get_current_rect().width();
    }
	return 0;
}

int 
time_sig::get_x_offset(window& the_window)
{
    make_figures(the_window);
    if (my_time_figure)
    {
        return my_time_figure->get_bounding_box().ul.x;
    }
    else if (my_num_figure)
    {
        return -1 * my_num_figure->get_current_rect().width();
    }
	return 0;
}

void
time_sig::make_figures(window& the_window)
{
    size the_ppi = the_window.pixels_per_inch();
    if ((my_time_figure == 0) &&
        (my_time.num == 2) &&
        (my_time.den == 2,2) &&
        (my_use_symbols == true))
    {
        my_time_figure = figure_factory::get_instance().get_figure(
            the_ppi,figure_factory::CUT_TIME,my_scale);
        my_time_figure->add_ref();
        my_width = my_time_figure->get_width_in_pixels();
    }
    else if ((my_time_figure == 0) &&
             (my_time.num == 4) &&
             (my_time.den == 4) &&
              (my_use_symbols == true))
    {
        my_time_figure = figure_factory::get_instance().get_figure(
            the_ppi,figure_factory::COMMON_TIME,my_scale);
        my_time_figure->add_ref();
        my_width = my_time_figure->get_width_in_pixels();
    }
    else if (my_num_figure == 0) 
    {
        my_num_figure = new text_figure(font::serifs,
            (int)((double)16 * my_scale.y),the_window);
        my_den_figure = new text_figure(font::serifs,
            (int)((double)16 * my_scale.y),the_window);
		my_num_figure->add_ref();
		my_den_figure->add_ref();
        string tmp_num_string = string((long)my_time.num);
        string tmp_den_string = string((long)my_time.den);
        my_num_figure->set_string(tmp_num_string);
        my_den_figure->set_string(tmp_den_string);
    }
}
}
