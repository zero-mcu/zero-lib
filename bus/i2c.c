#include <bus/i2c.h>

#ifdef I2C_DEBUG
#define I2C_DBG(fmt, ...)   print(fmt, ##__VA_ARGS__)
#else
#define I2C_DBG(fmt, ...)
#endif

void i2c_init(i2c_t* i2c, i2c_algorithm_t* algo)
{
    ASSERT(i2c);
    ASSERT(algo);

    i2c->algo = algo;
}

int i2c_write(i2c_t* i2c, i2c_dev_addr_t dev_addr, const ze_u8_t* data, ze_size_t len)
{
    ASSERT(i2c);
    ASSERT(data);
    ASSERT(len > 0);

    i2c_msg_t msg;

    msg.flag = I2C_WRITE;
    msg.data = (ze_u8_t*)data;
    msg.len = len;

    if (!i2c->algo || !i2c->algo->xfer)
        return ENO_ERROR;
    return i2c->algo->xfer(i2c, dev_addr, &msg, 1);
}

/**
  * @brief This function is used to read data from i2c bus.
  * @param i2c: i2c bus pointer.
  * @param dev_addr: i2c device address.
  * @param data: data to be read.
  * @param len: length of data.
  * @retval 0 for success, otherwise -1.
  */
int i2c_read(i2c_t* i2c, i2c_dev_addr_t dev_addr, ze_u8_t* data, ze_size_t len)
{
    ASSERT(i2c);
    ASSERT(data);
    ASSERT(len > 0);

    i2c_msg_t msg;

    msg.flag = I2C_READ;
    msg.data = data;
    msg.len = len;

    if (!i2c->algo || !i2c->algo->xfer)
        return ENO_ERROR;
    return i2c->algo->xfer(i2c, dev_addr, &msg, 1);
}

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
int i2c_client_write(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_addr_t address, const ze_u8_t* wr_data, ze_size_t wr_len)
{
    ASSERT(i2c);
    ASSERT(wr_data);
    ASSERT(wr_len);

    ze_u32_t addr = I2C_ADDR(address);

    i2c_msg_t msgs[2];
    msgs[0].flag = I2C_WRITE;
    msgs[0].data = (ze_u8_t*)&addr;
    msgs[0].len = I2C_ADDR_SIZE(address);

    msgs[1].flag = I2C_APPEND;
    msgs[1].data = (ze_u8_t*)wr_data;
    msgs[1].len = wr_len;

    if (!i2c->algo || !i2c->algo->xfer)
        return ENO_ERROR;
    return i2c->algo->xfer(i2c, dev_addr, msgs, 2);
}


int i2c_client_read(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_addr_t address, ze_u8_t* rd_data, ze_size_t rd_len)
{
    ASSERT(i2c);
    ASSERT(rd_data);
    ASSERT(rd_len);

    ze_u32_t addr = I2C_ADDR(address);

    i2c_msg_t msgs[2];

    msgs[0].flag = I2C_WRITE;
    msgs[0].data = (ze_u8_t*)&addr;
    msgs[0].len = I2C_ADDR_SIZE(address);
    msgs[1].flag = I2C_READ;
    msgs[1].data = rd_data;
    msgs[1].len = rd_len;

    if (!i2c->algo || !i2c->algo->xfer)
        return ENO_ERROR;
    return i2c->algo->xfer(i2c, dev_addr, msgs, 2);
}



int i2c_switch_enable_channel(i2c_switch_t* i2c_switch, int channel)
{
    ASSERT(i2c_switch);

    if (i2c_switch->enable_channel)
        return i2c_switch->enable_channel(i2c_switch, channel);

    return 0;
}

int i2c_switch_disable_channel(i2c_switch_t* i2c_switch, int channel)
{
    ASSERT(i2c_switch);

    if (i2c_switch->disable_channel)
        return i2c_switch->disable_channel(i2c_switch, channel);

    return 0;
}

static int i2c_mux_xfer(i2c_t* i2c, i2c_dev_addr_t dev_addr, i2c_msg_t* msg, ze_size_t nmsg)
{
    i2c_mux_t* mux = CONTAINER_OF(i2c, i2c_mux_t, vi2c);
    int ret = 0;

    while (mux->source)
        mux = mux->source;

    while (mux)
    {
        if ((ret = i2c_switch_enable_channel(mux->i2c_switch, mux->channel)) < 0)
            return ret;
        mux = mux->sink;
    }

    ret = i2c->algo->xfer(i2c, dev_addr, msg, nmsg);

    mux = CONTAINER_OF(i2c, i2c_mux_t, vi2c);
    
    while (mux)
    {
        if ((ret = i2c_switch_disable_channel(mux->i2c_switch, mux->channel)) < 0)
            return ret;
        mux = mux->sink;
    }

    return ret;
}

static i2c_algorithm_t i2c_mux_algo = {
    .xfer = i2c_mux_xfer,
};

i2c_t* i2c_mux_init(i2c_mux_t* i2c_mux, i2c_t* i2c, i2c_mux_t* source, i2c_switch_t* i2c_switch, int channel)
{
    ASSERT(i2c_mux);
    ASSERT(i2c);

    i2c_mux->sink = NULL;
    i2c_mux->source = source;
    source->sink = i2c_mux;
    i2c_mux->i2c = i2c;
    i2c_mux->i2c_switch = i2c_switch;
    i2c_mux->channel = channel;

    i2c_mux->vi2c.algo = &i2c_mux_algo;

    return &i2c_mux->vi2c;
}