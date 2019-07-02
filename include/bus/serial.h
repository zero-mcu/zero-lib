/**
 * @file serial.h
 * @brief This is the difinition of serail.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/05/31    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-06-10
 * @license Dual BSD/GPL
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__
#include <common/zero.h>

#ifdef __cplusplus
extern "C"{
#endif

#define SERIAL_DECLEAR(n)       MODULE_DECLEAR(serial, n)
#define IMPORT_SERIAL(n)        IMPORT_MODULE(serial, n)
#define SERIAL(n)               MODULE(serial, n)


#define SERIAL_FLAG_IRQ_TX           0x01
#define SERIAL_FLAG_IRQ_RX           0x02
#define SERIAL_FLAG_DMA_TX           0x04
#define SERIAL_FLAG_DMA_RX           0x08
#define SERIAL_FLAG_INIT             0x10

#define SERIAL_EVENT_IRQ_TX           1
#define SERIAL_EVENT_IRQ_RX           2
#define SERIAL_EVENT_DMA_TX           3
#define SERIAL_EVENT_DMA_RX           4

#define SERIAL_RB_BUFSZ               100


#define SERIAL_CONFIG   {115200,SERIAL_DATABITS_8,SERIAL_STOPBITS_1,SERIAL_PARITY_NONE}

#define SERIAL_SET_CONFIG_DEFAULT(serial)       do {serial->config.baudrate = 115200;\
                                                    serial->config.databits = SERIAL_DATABITS_8;\
                                                    serial->config.stopbits = SERIAL_STOPBITS_1;\
                                                    serial->config.parity = SERIAL_PARITY_NONE;\
                                                } while(0)

#define SERIAL_PARITY_NONE      0
#define SERIAL_PARITY_EVEN      1
#define SERIAL_PARITY_ODD       2

#define SERIAL_DATABITS_7       7
#define SERIAL_DATABITS_8       8
#define SERIAL_DATABITS_9       8

#define SERIAL_STOPBITS_1       0
#define SERIAL_STOPBITS_1_5     1
#define SERIAL_STOPBITS_2       2

typedef struct {
    ze_u32_t baudrate;
    ze_u8_t parity;
    ze_u8_t databits;
    ze_u8_t stopbits;
} serial_config_t;

typedef struct serial_fifo {
    ze_u8_t *buffer;
    int put_index,get_index;
    int bufsz;
} serial_fifo_t;

typedef struct serial {
    ze_u8_t flags;
    list_t node;
    serial_config_t config;
    void *serial_rx;
    void *serial_tx;
    void *private_data;
    const struct serial_operations* ops;
} serial_t;

typedef struct serial_operations {
    int (*init)(serial_t* serial);
    int (*put_char)(serial_t* serial, int ch);
    int (*get_char)(serial_t* serial);
    int (*config)(serial_t* serial, int cmd, void* arg);
} serial_ops_t;

/**
 * @brief init serial device.
 * @param serial: pointer of serial device.
 * @param ops: serial device operations.
 */
void serial_init(serial_t* serial, const serial_ops_t* ops);

/**
 * @brief write data to serial device.
 * @param serial: pointer of serial device.
 * @param wr_data: data to be write.
 * @param wr_len: data length in bytes.
 * @return return number of bytes still not be written or return error code.
 */
int serial_write(serial_t* serial, const ze_u8_t* wr_data, ze_size_t wr_len);

/**
 * @brief read data from serial device.
 * @param serial: pointer of serial device.
 * @param rd_data: data be read
 * @param rd_len: data length in bytes.
 * @return return number of bytes still not be read or return error code.
 */
int serial_read(serial_t* serial, ze_u8_t* rd_data, ze_size_t rd_len);

/**
 * @brief set completion callback which will be called when data has been write.
 * @param serial: pointer of serial device.
 * @param complete: completion callback function.
 * @return error code. If retval < 0, execute failed. If retval == 0, execute success.
 */
int serial_set_completion(serial_t* serial, void (*complete)(void* data));

/**
 * @brief set rxindicate callback which will be called when data has been read.
 * @param serial: pointer of serial device.
 * @param rx_indicate: rx_indicate callback function.
 * @return error code. If retval < 0, execute failed. If retval == 0, execute success.
 */
int serial_set_rxindicate(serial_t* serial, void (*rx_indicate)(ze_size_t rd_len));

/**
 * @brief serial hardware interrupt.
 * @param serial: pointer of serial device.
 * @param event: serial interrupt event.
 */
void serial_hw_isr(serial_t* serial, ze_u8_t event);

#ifdef __cplusplus
}
#endif
#endif