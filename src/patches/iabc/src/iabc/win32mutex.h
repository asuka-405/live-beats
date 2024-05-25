#ifndef win32mutex_h
#define win32mutex_h
// This code was written in 2001 by Aaron Newman.  I release it into the
// public domain.
#include "iabc/mutex.h"

#include "windows.h"

namespace iabc{

class win32lock;
class win32mutex:public mutex_if
{
    friend class win32lock;
public:
    win32mutex();
    virtual ~win32mutex();
    virtual void seize();
    virtual void release();
    virtual bool is_mine();
private:
    CRITICAL_SECTION my_cs;
    DWORD my_owner;
};


}
#endif

