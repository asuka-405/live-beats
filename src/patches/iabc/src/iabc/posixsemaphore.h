#include "iabc/semaphore.h"
#include <semaphore.h>


namespace iabc
{

class posixsemaphore:public semaphore_if
{
public:
    posixsemaphore(int the_initial_count = 1);
    ~posixsemaphore();
    virtual void seize();
    virtual void release();
private:
    sem_t my_sem_handle;
};

}

