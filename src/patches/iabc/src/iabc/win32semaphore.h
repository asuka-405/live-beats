#include "semaphore.h"
#include "windows.h"

namespace iabc
{

class win32semaphore:public semaphore_if
{
public:
    win32semaphore(int the_initial_count = 1);
    ~win32semaphore();
    virtual void seize();
    virtual void release();
private:
    HANDLE my_sem_handle;
};

}

