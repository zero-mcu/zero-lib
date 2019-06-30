/**
 * @file syscfg.h
 * @brief This is the difinition of syscfg.
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
#ifndef __COMMON_CFG_H__
#define __COMMON_CFG_H__
#include <zerocfg.h>

#ifndef ZERO_HZ
#define ZERO_HZ       200
#endif

#ifndef CPU_CLOCK_HZ
#define CPU_CLOCK_HZ        72000000
#endif


#ifndef CONFIG_USING_ASSERT
#define CONFIG_USING_ASSERT             1
#endif

#ifndef CONFIG_USING_RTOS
#define CONFIG_USING_RTOS              1
#endif

#endif