#include <common/zero.h>
#include <bus/i2c_bit.h>
#include <chip/gpio.h>


#define scl_set(algo, value)    (algo->scl_set(algo, value))
#define sda_set(algo, value)    (algo->sda_set(algo, value))
#define sda_get(algo)           (algo->sda_get(algo))

#define SCL_H(algo)            scl_set(algo, GPIO_LEVEL_HIGH)
#define SCL_L(algo)            scl_set(algo, GPIO_LEVEL_LOW)
#define SDA_H(algo)            sda_set(algo, GPIO_LEVEL_HIGH)
#define SDA_L(algo)            sda_set(algo, GPIO_LEVEL_LOW)
#define SDA_STATE(algo)        (GPIO_LEVEL_HIGH == sda_get(algo))

ze_inline int i2c_enable_input(i2c_bit_algorithm_t* algo)
{
    if (algo->sda_set_dir)
        return algo->sda_set_dir(algo, GPIO_DIR_INPUT);
    return 0;
}

ze_inline int i2c_enable_output(i2c_bit_algorithm_t* algo)
{
    if (algo->sda_set_dir)
        return algo->sda_set_dir(algo, GPIO_DIR_OUTPUT);

    return 0;
}

int i2c_bit_start(i2c_bit_algorithm_t* algo)
{
    i2c_enable_output(algo);
    SDA_H(algo);
    SCL_H(algo);
    delay_us(algo->delay_us);
    SDA_L(algo);
    delay_us(algo->delay_us);
    SCL_L(algo);
    delay_us(algo->delay_us);
    return 0;
}

int i2c_bit_stop(i2c_bit_algorithm_t* algo)
{
    i2c_enable_output(algo);
    SDA_L(algo);
    delay_us(algo->delay_us);
    SCL_H(algo);
    delay_us(algo->delay_us);
    SDA_H(algo);
    delay_us(algo->delay_us);
    return 0;
}

int i2c_bit_writeb(i2c_bit_algorithm_t* algo, ze_u8_t data, i2c_ack_t *ack)
{
    ze_u8_t temp = 0;
    i2c_enable_output(algo);
    for(int i = 0;i < 8;i++)
    {
        temp = data&0x80;
        data <<= 1;
        if(temp)
            SDA_H(algo);
        else
            SDA_L(algo);
        delay_us(algo->delay_us);
        SCL_H(algo);
        delay_us(algo->delay_us);
        SCL_L(algo);
        delay_us(algo->delay_us);
    }
    i2c_enable_input(algo);
    SDA_H(algo);
    delay_us(algo->delay_us);
    SCL_H(algo);
    delay_us(algo->delay_us);
    if(SDA_STATE(algo))
        *ack = I2C_NACK;
    else
        *ack = I2C_ACK;

    SCL_L(algo);
    delay_us(algo->delay_us);

    return 0;
}
int i2c_bit_readb(i2c_bit_algorithm_t* algo, ze_u8_t *data, i2c_ack_t ack)
{
    int temp = 0;

    SDA_H(algo);

    i2c_enable_input(algo);

    delay_us(algo->delay_us);
    for(int i=0;i<8;i++)
    {
        temp <<= 1;
        SCL_H(algo);
        delay_us(algo->delay_us);
        if(SDA_STATE(algo))
            temp |= 0x01;
        SCL_L(algo);
        delay_us(algo->delay_us);
    }

    i2c_enable_output(algo);

    if(ack == I2C_ACK)
        SDA_L(algo);
    else
        SDA_H(algo);
    SCL_H(algo);
    delay_us(algo->delay_us);
    SCL_L(algo);
    delay_us(algo->delay_us);
    *data = temp;

    return 0;
}


int i2c_bit_xfer(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_msg_t* msgs, ze_size_t nmsg)
{
    int ret = 0;
    i2c_ack_t ack;
    ze_u8_t* data;
    i2c_algorithm_t* algo = i2c->algo;
    i2c_bit_algorithm_t* i2c_bit_algo = CONTAINER_OF(algo, i2c_bit_algorithm_t, algo);

    for (int i = 0; i < nmsg; i++)
    {
        i2c_msg_t* msg = msgs + i;
        data = msg->data;
        if (msg->flag == I2C_WRITE)
        {
            if ((ret = i2c_bit_start(i2c_bit_algo)) != 0)
                goto i2c_error;
            if(i2c_bit_writeb(i2c_bit_algo, (I2C_ADDR(dev_addr) << 1) | 0x00, &ack)<0 || ack != I2C_ACK)
                goto i2c_error;
            for(int j = 0; j < msg->len; j++)
            {
                if(i2c_bit_writeb(i2c_bit_algo, *data ++, &ack) < 0 || ack != I2C_ACK)
                    goto i2c_error;
            }
        }
        else if (msg->flag == I2C_READ)
        {
            if(i2c_bit_start(i2c_bit_algo) < 0)
                goto i2c_error;
            if(i2c_bit_writeb(i2c_bit_algo, (I2C_ADDR(dev_addr) << 1) | 0x01, &ack)<0 || ack != I2C_ACK)
                goto i2c_error;
            for(int j = 0; j < msg->len; j++)
            {
                if(j == msg->len-1)
                {
                    if(i2c_bit_readb(i2c_bit_algo, data ++, I2C_NACK)<0)
                        goto i2c_error;
                }
                else
                {
                    if(i2c_bit_readb(i2c_bit_algo, data ++, I2C_ACK)<0)
                        goto i2c_error;
                }
            }
        }
        else if (msg->flag == I2C_APPEND)
        {
            for (int j = 0; j < msg->len; j++)
            {
                if (i2c_bit_writeb(i2c_bit_algo, *data++, &ack) || ack != I2C_ACK)
                    goto i2c_error;

            }
        }
    }

    i2c_bit_stop(i2c_bit_algo);

    return 0;
i2c_error:
    i2c_bit_stop(i2c_bit_algo);
    return ENO_ERROR;
}

void i2c_bit_algorithm_init(i2c_bit_algorithm_t* algo)
{
    ASSERT(algo);
    algo->algo.xfer = i2c_bit_xfer;
}