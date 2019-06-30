/**
 * @file i2c.h
 * @brief This is the difinition of i2c.
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
#ifndef __BUS_I2C_H__
#define __BUS_I2C_H__
#include <common/zero.h>

#ifdef __cplusplus
extern "C"{
#endif

#define I2C_DECLEAR(n)     MODULE_DECLEAR(i2c, n)
#define IMPORT_I2C(n)      IMPORT_MODULE(i2c, n)
#define I2C(n)             MODULE(i2c, n)

#define I2C_FLAG_7BIT_ADDR                      (0)
#define I2C_FLAG_10BIT_ADDR                     (1 << 15)

#define I2C_MAKE_DEV_ADDR(_dev_addr, _flag)     ((i2c_dev_addr_t)((ze_u16_t)((_dev_addr & 0x7FFF) | _flag)))
#define I2C_MAKE_7BIT_ADDR(dev_addr)            I2C_MAKE_DEV_ADDR(dev_addr, I2C_FLAG_7BIT_ADDR)
#define I2C_MAKE_10BIT_ADDR(dev_addr)           I2C_MAKE_DEV_ADDR(dev_addr, I2C_FLAG_10BIT_ADDR)
#define I2C_IS_10BIT_ADDR(dev_addr)             (dev_addr.flag == I2C_FLAG_10BIT_ADDR)

#define I2C_DEV_ADDR(dev_addr)                  (dev_addr.addr)
#define I2C_DEV_ADDR_SIZE(dev_adr)              (dev_addr.flag == I2C_FLAG_10BIT_ADDR ? 2 : 1)

#define I2C_MAKE_ADDR(address, size)            ((i2c_addr_t)((size << 30) | address))
#define I2C_ADDR(address)                       (address.addr)
#define I2C_ADDR_SIZE(address)                  (address.size)

#define I2C_WRITE                             0
#define I2C_READ                              1
#define I2C_APPEND                            2


typedef union {
    struct {
        ze_u16_t addr           : 15;
        ze_u16_t flag           : 1;
    };
    ze_u16_t value;
} i2c_dev_addr_t;

typedef union {
    struct {
        ze_u32_t addr           : 30;
        ze_u32_t size           : 2;
    };
    ze_u32_t value;
} i2c_addr_t;

typedef enum {
    I2C_ACK = 0,
    I2C_NACK,
} i2c_ack_t;

typedef struct i2c {
    struct i2c_algorithm *algo;
    void* private_data;
} i2c_t;

typedef struct i2c_msg {
  ze_u8_t flag;
  ze_u8_t* data;
  ze_size_t len;
} i2c_msg_t;

typedef struct i2c_algorithm {
    void* private_data;
    int (*xfer)(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_msg_t* msg, ze_size_t nmsg);
} i2c_algorithm_t;

void i2c_init(i2c_t* i2c, i2c_algorithm_t* algo);

/**
  * @brief This function is used to write data to i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be write.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_write(i2c_t* i2c, i2c_dev_addr_t dev_addr, const ze_u8_t* data, ze_size_t len);

/**
  * @brief This function is used to read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be read.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_read(i2c_t* i2c, i2c_dev_addr_t dev_addr, ze_u8_t* data, ze_size_t len);

/**
  * @brief This function is used to write and read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address
  * @param address: slave device memory or register address.
  * @param flag: address option, 0 for 7 bit address, 1 for 10 bit address.
  * @param wr_data: data to be write.
  * @param wr_len: length of data to be write.
  * @retval 0 for success, otherwise -1.
  */
int i2c_address_write(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_addr_t address, const ze_u8_t* wr_data, ze_size_t wr_len);

/**
  * @brief This function is used to write and read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address
  * @param address: slave device memory or register address.
  * @param flag: address option, 0 for 7 bit address, 1 for 10 bit address.
  * @param wr_data: data to be write.
  * @param wr_len: length of data to be read.
  * @retval 0 for success, otherwise -1.
  */
int i2c_address_read(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_addr_t address, ze_u8_t* rd_data, ze_size_t rd_len);


typedef struct i2c_switch {
    int (*enable_channel)(struct i2c_switch* i2c_switch, int channel);
    int (*disable_channel)(struct i2c_switch* i2c_switch, int channel);
} i2c_switch_t;

typedef struct i2c_mux {
    struct i2c_mux* sink;
    struct i2c_mux* source;
    i2c_t* i2c;
    i2c_t vi2c;
    int channel;
    i2c_switch_t* i2c_switch;
} i2c_mux_t;

int i2c_switch_enable_channel(i2c_switch_t* i2c_switch, int channel);

int i2c_switch_disable_channel(i2c_switch_t* i2c_switch, int channel);

i2c_t* i2c_mux_init(i2c_mux_t* i2c_mux, i2c_t* i2c, i2c_mux_t* source, i2c_switch_t* i2c_switch, int channel);

#ifdef __cplusplus
}
#endif
#endif
