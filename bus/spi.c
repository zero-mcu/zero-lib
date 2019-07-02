#include <string.h>
#include <bus/spi.h>
#include <chip/gpio.h>

int spi_set_mode(spi_t* spi, spi_mode_t mode)
{
    ASSERT(spi);

    if (spi->algo->set_mode)
        return spi->algo->set_mode(spi, mode);

    return 0;
}

int spi_set_speed(spi_t* spi, ze_u32_t speed)
{
    ASSERT(spi);

    if (spi->algo->set_speed)
        return spi->algo->set_speed(spi, speed);

    return 0;
}

int spi_transfer(spi_t* spi, spi_msg_t* msgs, ze_size_t nmsg)
{
    spi_algorithm_t* algo = spi->algo;

    if (algo->xfer)
        return algo->xfer(spi, msgs, nmsg);
    return 0;
}

int spi_write(spi_t* spi, const ze_u8_t *wr_data, ze_size_t wr_len)
{
    ASSERT(spi);
    ASSERT(wr_data);

    spi_msg_t msg;

    msg.tx_buf = (ze_u8_t*)wr_data;
    msg.tx_len = wr_len;
    msg.flag = SPI_WRITE;

    return spi_transfer(spi, &msg, 1);
}

int spi_read(spi_t* spi, ze_u8_t *rd_data, ze_size_t rd_len)
{
    ASSERT(spi);
    ASSERT(rd_data);

    spi_msg_t msg;

    msg.rx_buf = rd_data;
    msg.rx_len = rd_len;
    msg.flag = SPI_READ;

    return spi_transfer(spi, &msg, 1);
}

int spi_write_then_read(spi_t* spi, const ze_u8_t *wr_data, ze_size_t wr_len, ze_u8_t *rd_data, ze_size_t rd_len)
{
    ASSERT(spi);
    ASSERT(wr_data);
    ASSERT(rd_data);

    spi_msg_t msgs[2];

    msgs[0].tx_buf = (ze_u8_t*)wr_data;
    msgs[0].tx_len = wr_len;
    msgs[0].flag = SPI_WRITE;
    msgs[1].rx_buf = rd_data;
    msgs[1].rx_len = rd_len;
    msgs[1].flag = SPI_READ;

    return spi_transfer(spi, msgs, 2);
}

int spi_write_and_read(spi_t* spi, const ze_u8_t *wr_data, ze_u8_t *rd_data, ze_size_t len)
{
    ASSERT(spi);
    ASSERT(wr_data);
    ASSERT(rd_data);

    spi_msg_t msg;

    msg.tx_buf = (ze_u8_t*)wr_data;
    msg.tx_len = len;
    msg.rx_buf = rd_data;
    msg.rx_len = len;
    msg.flag = SPI_RDWR;

    return spi_transfer(spi, &msg, 1);
}


void spi_init(spi_t* spi, spi_algorithm_t* algo)
{
    ASSERT(spi);
    ASSERT(spi->algo);

    spi->algo = algo;
}


static int spi_exp_set_speed(spi_t* spi, ze_u32_t speed)
{
    int ret = 0;
    spi_exp_t* spi_exp = CONTAINER_OF(spi, spi_exp_t, vspi);

    ret = spi_set_speed(spi_exp->spi, speed);

    return ret;
}

static int spi_exp_get_speed(spi_t* spi, ze_u32_t* speed)
{
    int ret = 0;
    spi_exp_t* spi_exp = CONTAINER_OF(spi, spi_exp_t, vspi);

    ret = spi_get_speed(spi_exp->spi, speed);

    return ret;
}

static int spi_exp_set_mode(spi_t* spi, spi_mode_t mode)
{
    int ret = 0;
    spi_exp_t* spi_exp = CONTAINER_OF(spi, spi_exp_t, vspi);

    ret = spi_set_mode(spi_exp->spi, mode);

    return ret;
}

static int spi_exp_get_mode(spi_t* spi, spi_mode_t* mode)
{
    int ret = 0;
    spi_exp_t* spi_exp = CONTAINER_OF(spi, spi_exp_t, vspi);

    ret = spi_get_mode(spi_exp->spi, mode);

    return ret;
}

static int spi_exp_xfer(spi_t* spi, spi_msg_t* msgs, ze_size_t nmsg)
{
    int ret = 0;
    spi_exp_t* spi_exp = CONTAINER_OF(spi, spi_exp_t, vspi);

    spi_exp->cs_set(GPIO_LEVEL_LOW);

    ret = spi_transfer(spi_exp->spi, msgs, nmsg);

    spi_exp->cs_set(GPIO_LEVEL_HIGH);

    return ret;
}

static spi_algorithm_t spi_exp_algo = {
    .set_speed = spi_exp_set_speed,
    .get_speed = spi_exp_get_speed,
    .set_mode = spi_exp_set_mode,
    .get_mode = spi_exp_get_mode,
    .xfer = spi_exp_xfer,
};

spi_t* spi_bind_cs(spi_exp_t* spi_exp, spi_t* spi, void (*cs_set)(int value))
{
    ASSERT(spi_exp);
    ASSERT(spi);
    ASSERT(cs_set);

    spi_exp->spi = spi;
    spi_exp->vspi.algo = &spi_exp_algo;
    spi_exp->cs_set = cs_set;

    return &spi_exp->vspi;
}
