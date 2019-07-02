#include <common/zero.h>

void zero_init(void)
{

}

void zero_schedule(void)
{
#if CONFIG_USING_RTOS == 1
    osKernelStart();
#else
    for (;;)
    {
        co_thread_schedule();
    }
#endif  
}