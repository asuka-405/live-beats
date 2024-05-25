#ifndef iabc_semaphore_h
#define iabc_semaphore_h
#include "factory.h"

namespace iabc
{
class semaphore_if
{
public:
    semaphore_if(int the_initial_count = 1){};
    virtual ~semaphore_if(){};
    virtual void seize() = 0;
    virtual void release() = 0;
};

class semaphore:public semaphore_if
{
public:
    semaphore(int the_initial_count = 1);
    virtual ~semaphore(){if (my_if) delete my_if;};
    virtual void seize(){my_if->seize();};
    virtual void release(){my_if->release();};
private:
    semaphore_if* my_if;
};

}


#endif


