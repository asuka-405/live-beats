#include "iabc/semaphore.h"

namespace iabc
{
semaphore::semaphore(int the_initial_count):
        my_if(factory<semaphore_if,int>::create(the_initial_count))
{
}
}
