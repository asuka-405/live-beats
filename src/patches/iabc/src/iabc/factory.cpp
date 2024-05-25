#include "iabc/factory.h"

namespace iabc
{
;

template <class type>
int instance_counter<type>::class_instance_count = 0;

template <class type>
int instance_counter<type>::clock_in_count = 0;

template <class type>
int
instance_counter<type>::operator++(void)
{
    return (++instance_counter<type>::class_instance_count);
}
template <class type>
int
instance_counter<type>::operator++(int ignore)
{
    return (++instance_counter<type>::class_instance_count);
}
template <class type>
int 
instance_counter<type>::operator--(void)
{
    return (--instance_counter<type>::class_instance_count);
}
template <class type>
int 
instance_counter<type>::operator--(int ignore)
{
    return (--instance_counter<type>::class_instance_count);
}

template <class type>
int
instance_counter<type>::get_number_instances()
{
    return instance_counter<type>::class_instance_count;
}

template <class type>
void 
instance_counter<type>::clock_in()
{
    instance_counter<type>::clock_in_count = instance_counter<type>::class_instance_count;
}

template <class type>
void 
instance_counter<type>::clock_out()
{
    if (instance_counter<type>::class_instance_count > instance_counter<type>::clock_in_count)
    {
        throw ("Memory leak detected");
    }
}
}

