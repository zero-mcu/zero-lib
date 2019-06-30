#include <common/zero.h>
#include <bus/spi_bit.h>
#include <bus/spi.h>
#include <chip/gpio.h>

#ifdef SPI_DEBUG
#define spi_dbg(fmt, ...)   kernel_printf(fmt, ##__VA_ARGS__)
#else
#define spi_dbg(fmt, ...)
#endif

#define sck_set(algo, value) (algo->sck_set(algo, value))
#define mosi_set(algo, value) (algo->mosi_set(algo, value))
#define miso_get(algo)        (algo->miso_get(algo))

#define SCK_H(algo)          sck_set(algo, GPIO_LEVEL_HIGH)
#define SCK_L(algo)          sck_set(algo, GPIO_LEVEL_LOW)
#define MOSI_H(algo)          mosi_set(algo, GPIO_LEVEL_HIGH)
#define MOSI_L(algo)          mosi_set(algo, GPIO_LEVEL_LOW)
#define MISO_STATE(algo)      miso_get(algo)


ze_inline int spi_read_writeb(spi_bit_algorithm_t* algo, ze_u8_t wr_data, spi_mode_t mode)
{
    ze_u8_t rd_data = 0;

    delay_us(algo->delay_us);
    if(mode == SPI_MODE_0)
    {
        // CPOL=0, CPHA=0
        for(int i=0; i < 8; i++)
        {
            if(wr_data & 0x80)
                MOSI_H(algo);
            else
                MOSI_L(algo);
            wr_data <<= 1;
            delay_us(algo->delay_us);
            SCK_H(algo);
            delay_us(algo->delay_us);
            rd_data <<= 1;
            if(MISO_STATE(algo))
                rd_data |= 1;
            SCK_L(algo);
        }
    }
    else if(mode == SPI_MODE_1)
    {
        //CPOL=0, CPHA=1
        for(int i=0; i < 8; i++)
        {
            SCK_H(algo);

            if(wr_data & 0x80)
                MOSI_H(algo);
            else
                MOSI_L(algo);
            wr_data <<= 1;

            delay_us(algo->delay_us);
            rd_data <<= 1;
            if(MISO_STATE(algo))
                rd_data |= 1;

            SCK_L(algo);
            delay_us(algo->delay_us);

        }
    }
    else if(mode == SPI_MODE_2)
    {
        //CPOL=1, CPHA=0
        for(int i=0; i < 8; i++)
        {
            if(wr_data & 0x80)
                MOSI_H(algo);
            else
                MOSI_L(algo);
            wr_data <<= 1;
            delay_us(algo->delay_us);
            SCK_L(algo);
            delay_us(algo->delay_us);
            rd_data <<= 1;
            if(MISO_STATE(algo))
                rd_data |= 1;
            SCK_H(algo);
        }
    }
    else if(mode == SPI_MODE_3)
    {
        //CPOL=1, CPHA=1
        for(int i=0; i < 8; i++)
        {
            SCK_L(algo);

            if(wr_data & 0x80)
                MOSI_H(algo);
            else
                MOSI_L(algo);
            wr_data <<= 1;

            delay_us(algo->delay_us);

            SCK_H(algo);

            delay_us(algo->delay_us);

            rd_data <<= 1;
            if(MISO_STATE(algo))
                rd_data |= 1;
        }
    }
    delay_us(algo->delay_us);
    return rd_data;        
}

static int spi_bit_set_mode(spi_t* spi, spi_mode_t mode)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(spi->algo, spi_bit_algorithm_t, algo);

    spi_bit_algo->mode = mode;

    switch (mode)
    {
    case SPI_MODE_0:
    case SPI_MODE_1:
        SCK_L(spi_bit_algo);
        break;
    case SPI_MODE_2:
    case SPI_MODE_3:
        SCK_H(spi_bit_algo);
        break;
    }

    return 0;
}

static int spi_bit_get_mode(spi_t* spi, spi_mode_t* mode)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(spi->algo, spi_bit_algorithm_t, algo);

    return spi_bit_algo->mode;
}

static int spi_bit_set_speed(spi_t* spi, ze_u32_t speed)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(spi->algo, spi_bit_algorithm_t, algo);

    spi_bit_algo->delay_us = 1000000 / speed / 2;

    return 0;
}

static int spi_bit_get_speed(spi_t* spi, ze_u32_t* speed)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(spi->algo, spi_bit_algorithm_t, algo);
    
    *speed = 1000000 / spi_bit_algo->delay_us / 2;
    return 0;
}


static int spi_bit_write(spi_algorithm_t* algo, const ze_u8_t* wr_data, ze_size_t wr_len)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(algo, spi_bit_algorithm_t, algo);

    for(int i=0; i < wr_len; i++)
    {
        spi_read_writeb(spi_bit_algo, *wr_data++, spi_bit_algo->mode);
    }

    return wr_len;
}

static int spi_bit_read(spi_algorithm_t* algo, ze_u8_t* rd_data, ze_size_t rd_len)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(algo, spi_bit_algorithm_t, algo);

    for(int i=0; i < rd_len; i++)
    {
        *rd_data++ = spi_read_writeb(spi_bit_algo, 0xFF, spi_bit_algo->mode);
    }

    return rd_len;
}

static int spi_bit_sync_transfer(spi_algorithm_t* algo, const ze_u8_t *wr_data, ze_u8_t *rd_data, ze_size_t len)
{
    spi_bit_algorithm_t* spi_bit_algo = CONTAINER_OF(algo, spi_bit_algorithm_t, algo);

    for(int i=0; i < len; i++)
    {
        *rd_data++ = spi_read_writeb(spi_bit_algo, *wr_data++, spi_bit_algo->mode);
    }

    return len;
}

static int spi_bit_xfer(spi_t* spi, spi_msg_t* msgs, ze_size_t nmsg)
{
    int ret = 0;

    for (int i = 0; i < nmsg; i++)
    {
        spi_msg_t* msg = msgs + i;
        if (msg->flag == SPI_WRITE)
        {
            if ((ret = spi_bit_write(spi->algo, (const ze_u8_t*)msg->tx_buf, msg->tx_len)) < 0)
                return ret;
        }
        else if (msg->flag == SPI_READ)
        {
            if ((ret = spi_bit_read(spi->algo, msg->rx_buf, msg->rx_len)) < 0)
                return ret;
        }
        else if (msg->flag == SPI_RDWR)
        {
            if ((ret = spi_bit_sync_transfer(spi->algo, (const ze_u8_t*)msg->tx_buf, msg->rx_buf, msg->tx_len)) < 0)
                return ret;
        }
    }
    return ret;
}

void spi_bit_algorithm_init(spi_bit_algorithm_t* algo)
{
    ASSERT(algo);
    
    algo->algo.set_mode = spi_bit_set_mode;
    algo->algo.set_speed = spi_bit_set_speed;
    algo->algo.get_mode = spi_bit_get_mode;
    algo->algo.get_speed = spi_bit_get_speed;
    algo->algo.xfer = spi_bit_xfer;
}
