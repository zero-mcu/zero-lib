/**
 * @file i2c_bit.h
 * @brief This is the difinition of i2c bit algorithm.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/06/16    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-06-16
 * @license Dual BSD/GPL
 */
#ifndef __I2C_BIT_H__
#define __I2C_BIT_H__

#include <bus/i2c.h>



#ifdef __cplusplus
extern "C"{
#endif

typedef struct i2c_bit_algorithm {
	i2c_algorithm_t algo;
	ze_usec_t delay_us;
	int (*scl_set)(struct i2c_bit_algorithm* algo, int value);
	int (*sda_set_dir)(struct i2c_bit_algorithm* algo, int value);
	int (*sda_set)(struct i2c_bit_algorithm* algo, int value);
	int (*sda_get)(struct i2c_bit_algorithm* algo);
} i2c_bit_algorithm_t;

void i2c_bit_algorithm_init(i2c_bit_algorithm_t* algo);

#ifdef __cplusplus
}
#endif
#endif
