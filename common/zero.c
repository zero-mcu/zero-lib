#include <common/zero.h>

void zero_init(void)
{

}

void zero_schedule(void)
{
    while(1)
    {
        co_thread_schedule();
    }
    
}