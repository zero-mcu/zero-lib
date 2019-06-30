/**
 * @file co_threads.h
 * @brief This is the difinition of co_thread.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/05/31    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-06-11
 * @license Dual BSD/GPL
 */
#ifndef __COMMON_CO_THREAD_H__
#define __COMMON_CO_THREAD_H__
#include <common/types.h>
#include <common/lists.h>

#ifdef __cplusplus
extern "C"{
#endif

enum {
    CO_THREAD_IDLE = 0,
    CO_THREAD_RUNNING,
    CO_THREAD_PENDING,
};

typedef struct {
    ze_u8_t flag;
    void* arg;
    void (*entry)(void* arg);
    slist_t node;
} co_thread_t;


void co_thread_add(co_thread_t* pthread, void (*entry)(void* arg), void* arg);

void co_thread_remove(co_thread_t* pthread);

void co_thread_start(co_thread_t* pthread);

void co_thread_stop(co_thread_t* pthread);

void co_thread_schedule(void);


#ifdef __cplusplus
} // extern "C"
#endif
#endif // __COMMON_CO_THREAD_H__