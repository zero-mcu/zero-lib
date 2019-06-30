/**
 * @file gavin.h
 * @brief This is the difinition of gavin.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/05/31    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-05-31
 * @license Dual BSD/GPL
 */
#ifndef __COMMON_ZERO_H__
#define __COMMON_ZERO_H__


#include <common/syscfg.h>
#include <common/types.h>
#include <common/assert.h>
#include <common/errno.h>
#include <common/lists.h>
#include <common/string.h>
#include <common/time.h>
#include <common/co_thread.h>
#if CONFIG_USING_RTOS == 1
#include <cmsis_os.h>
#endif


#ifdef __cplusplus
extern "C"{
#endif

void zero_init(void);

void zero_schedule(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // __COMMON_ZERO_H__