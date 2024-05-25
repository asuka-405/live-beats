#ifndef posixmutex_h
#define posixmutex_h
// This code was written in 2001 by Aaron Newman.  I release it into the
// public domain.
#include "iabc/mutex.h"

#include <pthread.h>

namespace iabc{

class posixmutex:public mutex_if
{
public:
    posixmutex();
    virtual ~posixmutex();
    virtual void seize();
    virtual void release();
    virtual bool is_mine();
private:
    pthread_mutex_t  my_mutex;
    pthread_t my_owner;
};

}
#endif
