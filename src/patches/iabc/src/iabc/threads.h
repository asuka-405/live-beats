#ifndef iabc_threads_h
#define iabc_threads_h
#include "iabc/factory.h"
#include "iabc/mutex.h"
#include "iabc/semaphore.h"

#define MAX_THREADS 100

namespace iabc 
{
class thread_mgr;
class thread
{
friend class thread_mgr;
public:
    thread(const char* the_label=0);
    virtual ~thread();
    bool start();
    virtual void entry_point(){};
    static thread* this_thread();
    static thread& get_system_thread();
    static void sleep(int the_milliseconds);
    void add_ref(void);
    void remove_ref(void);
    const char* get_label(){return my_label;};
protected:
    semaphore my_thread_sem;
    atomic_counter my_thread_ref;
    unsigned long my_system_id;
    const char* my_label;
};

// This is used for event logging, its nice to make
// it easy for non-threaded apps to use it
extern const char* get_current_thread_name();

class thread_mgr
{
    friend class thread;
public:
    static thread_mgr& get_instance();
    static void gc();
    virtual void shutdown_application() = 0;
    class thread_table
    {
    public:
        thread* thread_ptr;
    };

protected:
    bool start(thread* the_thread);
	static unsigned long get_system_thread_id();
    unsigned long get_system_id(thread* the_thread);
    virtual bool system_start(
                              thread* the_thread,
                              unsigned long& the_system_id) = 0;
    static void entry_point(void* p);
    virtual void sleep(int the_milliseconds) = 0;
    static thread* this_thread();
    thread_mgr();
    virtual ~thread_mgr();
    mutex my_mutex;
    thread_table my_table[MAX_THREADS];
};

}
#endif

