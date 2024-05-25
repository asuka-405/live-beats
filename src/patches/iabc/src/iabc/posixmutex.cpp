// This code was written in 2001 by Aaron Newman.  I release it into the
// public domain.
#include "iabc/mutex.h"
#include "iabc/posixmutex.h"
#include <pthread.h>

namespace iabc{

template<>
mutex_if* factory<mutex_if,int>::create()
{
	return new posixmutex;
}

posixmutex::posixmutex():my_owner(0)
{
    pthread_mutex_init (&my_mutex,0);
}

posixmutex::~posixmutex()
{
   pthread_mutex_destroy (&my_mutex);
}

void 
posixmutex::seize()
{
    pthread_t tmp_thread = pthread_self();
    if (tmp_thread != my_owner)
    {
        pthread_mutex_lock (&my_mutex);
        my_owner = pthread_self();
    }
}

void 
posixmutex::release()
{
    pthread_t tmp_thread = pthread_self();
    if (my_owner == tmp_thread)
    {
        my_owner = 0;
        pthread_mutex_unlock (&my_mutex);
    }
    
}

bool 
posixmutex::is_mine()
{
  if (my_owner)
    return (my_owner == pthread_self());
  else
    return (false);
}

}

