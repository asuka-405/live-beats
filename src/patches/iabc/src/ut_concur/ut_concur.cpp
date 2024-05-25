// concur.cpp : Defines the entry point for the console application.
//

#ifndef GCC
#include "stdafx.h"
#endif
#include "iabc/threads.h"
#include "iabc/event.h"
#include "iabc/event_log.h"
#include "iabc/list.cpp"
#include <stdio.h>
namespace iabc
{

class a_thread:public thread
{
public:
    a_thread(int num,const char* the_label):my_num(num),a_source(*sticky_event_source::create())
		,thread(the_label)
		{a_source.event_source_add_ref();};
    ~a_thread(){a_source.event_source_remove_ref();};
    void entry_point();
    event_handler* get_handler(){return sticky_event_handler::create(a_source);};
private:
    int my_num;
    sticky_event_source& a_source;
};

void a_thread::entry_point()
{
	thread::sleep(my_num);
	a_source.set();
}

}

int main(int argc, char* argv[])
{
	using namespace iabc;
	a_thread* a = new a_thread(24000,"a");
    a->add_ref();
    a_thread* b = new a_thread(24012,"b");
    a_thread* c = new a_thread(2403,"c");
    b->add_ref();
    c->add_ref();
    event_handler* h1 = a->get_handler();
    h1->event_handler_add_ref();
    event_handler* h2 = b->get_handler();
    h2->event_handler_add_ref();
    event_handler* h3 = c->get_handler();
    h3->event_handler_add_ref();
    a->start();
    b->start();
    c->start();
    event_handler::wait(*h1 | *h2 | *h3);
    h1->event_handler_remove_ref();
    h2->event_handler_remove_ref();
    h3->event_handler_remove_ref();
    a->remove_ref();
    b->remove_ref();
    c->remove_ref();
	thread_mgr::get_instance().shutdown_application();
	return 0;
}

