/**
 * @file spi.h
 * @brief This is the difinition of spi.
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
#ifndef __SPI_H__
#define __SPI_H__
#include <common/zero.h>

#ifdef __cplusplus
extern "C"{
#endif

#define SPI_DECLEAR(n)     MODULE_DECLEAR(spi, n)
#define IMPORT_SPI(n)      IMPORT_MODULE(spi, n)
#define SPI(n)             MODULE(spi, n)

#define SPI_READ        0
#define SPI_WRITE       1
#define SPI_RDWR        2

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
} spi_mode_t;

typedef struct {
    void* private_data;
    struct spi_algorithm *algo;
} spi_t;

typedef struct {
    ze_u8_t flag;
    ze_u8_t* rx_buf;
    ze_size_t rx_len;
    ze_u8_t* tx_buf;
    ze_size_t tx_len;
} spi_msg_t;

typedef struct spi_algorithm {
    int (*set_speed)(spi_t* spi, ze_u32_t speed);
    int (*get_speed)(spi_t* spi, ze_u32_t* speed);
    int (*set_mode)(spi_t* spi, spi_mode_t mode);
    int (*get_mode)(spi_t* spi, spi_mode_t* mode);

    int (*xfer)(spi_t* spi, spi_msg_t* msgs, ze_size_t nmsg);
} spi_algorithm_t;

typedef struct {
    spi_t vspi;
    spi_t* spi;
    void (*cs_set)(int value);
} spi_exp_t;


void spi_init(spi_t* spi, spi_algorithm_t* algo);

int spi_set_mode(spi_t* spi, spi_mode_t mode);

int spi_get_mode(spi_t* spi, spi_mode_t* mode);

int spi_set_speed(spi_t* spi, ze_u32_t speed);

int spi_get_speed(spi_t* spi, ze_u32_t* speed);

int spi_transfer(spi_t* spi, spi_msg_t* msgs, ze_size_t nmsg);
/**
  * @brief This function is used to write data to spi.
  * @param spi: spi bus pointer.
  * @param wr_data: data to write.
  * @param wr_len: length of data to write.
  * @retval 0 for success, otherwise -1.
  */
int spi_write(spi_t* spi, const ze_u8_t *wr_data, ze_size_t wr_len);

int spi_read(spi_t* spi, ze_u8_t* rd_data, ze_size_t rd_len);

int spi_write_then_read(spi_t*spi, const ze_u8_t* wr_data, ze_size_t wr_len, ze_u8_t* rd_data, ze_size_t rd_len);

int spi_write_and_read(spi_t* spi, const ze_u8_t* wr_data, ze_u8_t* rd_data, ze_size_t len);

spi_t* spi_bind_cs(spi_exp_t* spi_exp, spi_t* spi, void (*cs_set)(int value));

#ifdef __cplusplus
}
#endif
#endif
