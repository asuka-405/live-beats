#ifndef iabc_drawtemp_h
#define iabc_drawtemp_h
#include "iabc/wd_data.h"
#include "iabc/winres.h"
#include "iabc/dispatch.h"

namespace iabc
{
template <typename command_type,typename param_type>
class draw_command:public wd_command
{
public:
    static void dispatch_draw_command(window& w,
                                      param_type p,
                                      command_type& the_instance,
                                      bool blocking = false)
    {
		wd_data_dx<param_type> data(p);
        wd_command* tmp_cmd = draw_command<command_type,param_type>::get_draw_command(w,the_instance);
        tmp_cmd->command_add_ref();
        dispatcher* disp = dispatcher::get_dispatcher();
        disp->add_ref();
		disp->dispatch(*tmp_cmd,data,blocking);
        disp->remove_ref();
        tmp_cmd->command_remove_ref();
    };
protected:
    draw_command(window& w,command_type& the_command_instance):my_window(w),
        my_instance(&the_command_instance),wd_command(w){my_instance->add_ref();};
    ~draw_command(){my_instance->remove_ref();};
    static draw_command<command_type,param_type>*
        get_draw_command(window& w,command_type& the_command_instance)
	{return new draw_command<command_type,param_type>(w,the_command_instance);};
    virtual void do_command(wd_data param);
    virtual void do_command_dx(const param_type& param){my_instance->draw_self(my_window,param);};
private:
    command_type* my_instance;
    window& my_window;
};

}

#endif



