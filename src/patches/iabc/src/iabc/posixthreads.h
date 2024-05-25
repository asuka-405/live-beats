#include "threads.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>


namespace iabc
{
class posixthread_mgr:public thread_mgr
{
public:
    posixthread_mgr(){};
    ~posixthread_mgr(){};

    virtual bool system_start(thread* the_thread,
                              unsigned long& the_system_id);
    virtual void sleep(int the_milliseconds);
    void shutdown_application();
protected:
private:
    static void*  entry_point(void* the_this);
};
}
