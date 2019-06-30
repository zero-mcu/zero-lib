/**
 * @file spi_bit.h
 * @brief This is the difinition of spi_bit.
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
#ifndef __SPI_BIT_H__
#define __SPI_BIT_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <bus/spi.h>

typedef struct spi_bit_algorithm {
    spi_algorithm_t algo;
    spi_mode_t mode;
    int delay_us;
    int (*mosi_set)(struct spi_bit_algorithm* algo, int value);
    int (*miso_get)(struct spi_bit_algorithm* algo);
    int (*sck_set)(struct spi_bit_algorithm* algo, int value);
} spi_bit_algorithm_t;

void spi_bit_algorithm_init(spi_bit_algorithm_t* algo);

#ifdef __cplusplus
}
#endif
#endif