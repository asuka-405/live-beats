#include "threads.h"
#include "windows.h"

namespace iabc
{
class win32thread_mgr:public thread_mgr
{
public:
    win32thread_mgr(){};
    ~win32thread_mgr(){};

    virtual bool system_start(thread* the_thread,
                              unsigned long& the_system_id);
    virtual void sleep(int the_milliseconds){::Sleep(the_milliseconds);};
	void shutdown_application();
protected:
private:
    static unsigned int  WINAPI entry_point(LPVOID lpParameter);
    class win_thread_entry
    {    
    public:
        HANDLE handle;
        thread* thread_ptr;
    };

    static win_thread_entry win_thread_table[MAX_THREADS];
};
}
