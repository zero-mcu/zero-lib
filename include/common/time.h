/**
 * @file time.h
 * @brief This is the difinition of time.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/05/30    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-05-30
 * @license Dual BSD/GPL
 */
#ifndef __COMMON_TIME_H__
#define __COMMON_TIME_H__
#include <common/types.h>

#ifdef __cplusplus
extern "C"{
#endif

#define time_after(unknow, known)    ((long)(known)-(long)(unknow)<0)
#define time_before(unknow, known)    ((long)(unknow)-(long)(known)<0)
#define time_after_eq(unknow, known)  ((long)(unknow)-(long)(known)>=0)
#define time_before_eq(unknow, known) ((long)(known)-(long)(unknow)>=0)

/**
 * @brief get current system ticks.
 * @return current system ticks.
 */  
ze_tick_t sys_get_ticks(void);

/**
 * @brief get current system millis.
 * @return current system millis.
 */
ze_millis_t sys_get_millis(void);

/**
 * @brief delay us time.
 * @param us: us time to delay. 
 */
void delay_us(ze_usec_t us);

#define sys_sleep_us delay_us

/**
 * @brief delay millis time.
 * @param millis: milis time to delay. 
 */
void delay_ms(ze_millis_t millis);

/**
 * @brief sleep millis time.
 * @param millis: millis time to delay. 
 */
void sys_sleep_ms(ze_millis_t millis);

/**
 * @brief sleep second time.
 * @param sec: second time to delay. 
 */
void sys_sleep(float sec);

/**
 * @brief increase system ticks.
 */
void sys_tick_inc(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __COMMON_TIME_H__