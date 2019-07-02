#include <common/zero.h>


static ze_volatile ze_u32_t zero_ticks = 0;

/**
 * @brief get current system ticks.
 * @return current system ticks.
 */  
ze_tick_t sys_get_ticks(void)
{
    return zero_ticks;
}

/**
 * @brief get current system millis.
 * @return current system millis.
 */
ze_millis_t sys_get_millis(void)
{
    return zero_ticks * 1000 / ZERO_HZ;
}

/**
 * @brief delay us time.
 * @param us: us time to delay. 
 */
void delay_us(ze_usec_t us)
{
    int count_1us = (CPU_CLOCK_HZ / 1000000);

    for(int i = 0; i < us; i ++)
        for(int j = 0; j < count_1us; j ++);
}

#define sleep_us delay_us

/**
 * @brief delay millis time.
 * @param millis: milis time to delay. 
 */
void delay_ms(ze_millis_t millis)
{
    unsigned long timeout = zero_ticks + millis * ZERO_HZ / 1000;

    while(time_before(zero_ticks, timeout));
}

/**
 * @brief sleep millis time.
 * @param millis: millis time to delay. 
 */
void sys_sleep_ms(ze_millis_t millis)
{
#if CONFIG_USING_RTOS==1
#if (INCLUDE_xTaskGetSchedulerState == 1)
    if(xTaskGetSchedulerState()==taskSCHEDULER_NOT_STARTED)
    {
        delay_ms(millis);
    }
    else
    {
        osDelay(millis);
    }
#else
    osDelay(millis);
#endif
#else
    delay_ms(millis);
#endif
}

/**
 * @brief sleep second time.
 * @param sec: second time to delay. 
 */
void sys_sleep(float sec)
{
    ze_usec_t usec = (ze_usec_t)(sec * 1000000) % 1000;
    sys_sleep_ms((ze_millis_t)(sec * 1000));
    sys_sleep_us(usec);
}

/**
 * @brief increase system ticks.
 */
void sys_tick_inc(void)
{
    zero_ticks++;
#if CONFIG_USING_RTOS == 1
    osSystickHandler();
#endif
}

