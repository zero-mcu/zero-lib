#include <string.h>
#include <chip/w25qxx.h>
#include <bus/spi.h>

enum{
    WRITE_ENABLE                    = 0x06,
    WRITE_DISABLE                   = 0x04,
    READ_STATUS_REGISTER1           = 0x05,
    READ_STATUS_REGISTER2           = 0X35,
    WRITE_STATUS_REGISTER           = 0x01,
    PAGE_PROGRAM                    = 0x02,
    QUAD_PAGE_PROGRAM               = 0x32,
    BLOCK_ERASE_64K                 = 0xD8,
    BLOCK_ERASE_32k                 = 0x52,
    SECTOR_ERASE                    = 0x20,
    CHIP_ERASE                      = 0xC7,

    READ_DATA                       = 0x03,

    ERASE_SUSPEND                   = 0x75,
    ERASE_RESUME                    = 0x7A,
    POWER_DOWN                      = 0xB9,
    HIGH_PERFORMANCE_MODE           = 0xA3,
    MODE_BIT_RESET                  = 0xFF,

    MANUFACTURER_DEVICE_ID          = 0x90,
    READ_UNIQUE_ID                  = 0x48,
    JEDEC_ID                        = 0x9F,

    DUMMY_DATA                      = 0x00,

    STATUS_BUSY                     = 0x01,
    STATUS_WRITE_ENABLE             = 0x02,

    SECTOR_SIZE                     = 4 * 1024,
    BLOCK32K_SIZE                   = 32 * 1024,
    BLOCK64K_SIZE                   = 64 * 1024,
    PAGE_SIZE                       = 256,
};

ze_inline int _w25qxx_read_status(w25qxx_t* w25qxx, ze_u8_t status_id)
{
    ASSERT(w25qxx);

    int ret;
    ze_u8_t status;

    if ((ret = spi_write_then_read(w25qxx->spi, &status_id, 1, &status, 1)) < 0)
        return ret;

    return status;
}

int _w25qxx_write_enable(w25qxx_t* w25qxx)
{
    ASSERT(w25qxx);
    int ret;

    ze_u8_t write_enable = WRITE_ENABLE;

    if((ret = spi_write(w25qxx->spi, &write_enable, 1)) < 0)
        return ret;

    int status = 0;

    while(1)
    {
        status = _w25qxx_read_status(w25qxx, READ_STATUS_REGISTER1);
        if(((status & STATUS_BUSY) != STATUS_BUSY) && ((status & STATUS_WRITE_ENABLE) == STATUS_WRITE_ENABLE))
            break;
    }

    return ENO_OK;
}

int _w25qxx_write_disable(w25qxx_t* w25qxx)
{
    ASSERT(w25qxx);

    int ret;

    ze_u8_t write_enable = WRITE_DISABLE;

    if((ret = spi_write(w25qxx->spi, &write_enable, 1)) < 0)
        return ret;

    return ENO_OK;
}

ze_inline int _w25qxx_wait_idle(w25qxx_t* w25qxx, ze_millis_t timeout)
{
    ASSERT(w25qxx);

    ze_millis_t lasttime = sys_get_millis();

    while(1)
    {
        ze_u8_t status = _w25qxx_read_status(w25qxx, READ_STATUS_REGISTER1);
        if((status & STATUS_BUSY) != STATUS_BUSY)
            break;
        if(sys_get_millis() - lasttime > timeout)
            return ENO_TIMEOUT;
        sys_sleep_ms(1);
    }
    return ENO_OK;
}

ze_inline int _w25qxx_page_write(w25qxx_t* w25qxx, ze_u32_t address, ze_u8_t* wr_data, ze_size_t wr_len)
{
    int length = 0;
    int ret;

    if((address & (PAGE_SIZE - 1)) + wr_len > PAGE_SIZE)
        length = PAGE_SIZE - (address & (PAGE_SIZE - 1));
    else
        length = wr_len;

    if((ret = _w25qxx_write_enable(w25qxx)) != ENO_OK)
        return ret;

    ze_u8_t cmd_and_address[] = {PAGE_PROGRAM, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    spi_msg_t msgs[2];
    msgs[0].tx_buf = cmd_and_address;
    msgs[0].tx_len = sizeof(cmd_and_address);
    msgs[0].flag = SPI_WRITE;
    msgs[1].tx_buf = wr_data;
    msgs[1].tx_len = wr_len;
    msgs[1].flag = SPI_WRITE;

    if((ret = spi_transfer(w25qxx->spi, msgs, 2)) < 0)
        return ret;

    if((ret = _w25qxx_write_disable(w25qxx)) != ENO_OK)
        return ret;

    return length;
}

int w25qxx_sector_erase(w25qxx_t* w25qxx, ze_u32_t address)
{
    ASSERT(w25qxx);
    ASSERT(address >= 0);
    ASSERT((address & (SECTOR_SIZE - 1)) == 0);

    int ret;

    if((ret = _w25qxx_write_enable(w25qxx)) != ENO_OK)
        return ret;

    ze_u8_t wr_data[] = {SECTOR_ERASE, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    if((ret = spi_write(w25qxx->spi, wr_data, sizeof(wr_data))) < 0)
        return ret;

    if((ret = _w25qxx_wait_idle(w25qxx, 100)) < 0)
        return ret;

    if((ret = _w25qxx_write_disable(w25qxx)) < 0)
        return ret;

    return ENO_OK;
}

int w25qxx_block32_erase(w25qxx_t* w25qxx, ze_u32_t address)
{
    ASSERT(w25qxx);
    ASSERT(address >= 0);
    ASSERT((address & (BLOCK32K_SIZE - 1)) == 0);

    int ret;

    if((ret = _w25qxx_write_enable(w25qxx)) != ENO_OK)
        return ret;

    ze_u8_t wr_data[] = {BLOCK_ERASE_32k, (address >> 16) & 0xFF, (address >> 7) & 0xFF, address & 0xFF};

    if((ret = spi_write(w25qxx->spi, wr_data, sizeof(wr_data))) < 0)
        return ret;

    if((ret = _w25qxx_wait_idle(w25qxx, 100)) != ENO_OK)
        return ret;

    if((ret = _w25qxx_write_disable(w25qxx)) != ENO_OK)
        return ret;

    return ENO_OK;
}

int w25qxx_block64_erase(w25qxx_t* w25qxx, ze_u32_t address)
{
    ASSERT(w25qxx);
    ASSERT(address >= 0);
    ASSERT((address & (BLOCK64K_SIZE - 1)) == 0);

    int ret;

    if((ret = _w25qxx_write_enable(w25qxx)) != ENO_OK)
        return ret;

    ze_u8_t wr_data[] = {BLOCK_ERASE_64K, (address >> 16) & 0xFF, (address >> 7) & 0xFF, address & 0xFF};

    if((ret = spi_write(w25qxx->spi, wr_data, sizeof(wr_data))) < 0)
        return ret;

    if((ret = _w25qxx_wait_idle(w25qxx, 100)) != ENO_OK)
        return ret;

    if((ret = _w25qxx_write_disable(w25qxx)) != ENO_OK)
        return ret;

    return ENO_OK;
}

int w25qxx_write(w25qxx_t* w25qxx, ze_u32_t address, ze_u8_t* wr_data, ze_size_t wr_len)
{
    ASSERT(w25qxx);
    ASSERT(address >= 0);
    ASSERT(wr_data);
    ASSERT(wr_len > 0);

    int length = wr_len;

    while(length > 0)
    {
        int ret = _w25qxx_page_write(w25qxx, address, wr_data, length);

        if(ret < 0)
            break;

        wr_data += ret;
        length -= ret;
        address += ret;
    }

    return ENO_OK;
}

int w25qxx_read(w25qxx_t* w25qxx, ze_u32_t address, ze_u8_t* rd_data, ze_size_t rd_len)
{
    ASSERT(w25qxx);
    ASSERT(address > 0);
    ASSERT(rd_data);
    ASSERT(rd_len > 0);

    int ret = 0;

    ze_u8_t wr_data[] = {READ_DATA, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};

    if ((ret = spi_write_then_read(w25qxx->spi, wr_data, sizeof(wr_data), rd_data, rd_len)) < 0)
        return ret;

    return ENO_OK;
}

int w25qxx_read_manufacturer_device_id(w25qxx_t* w25qxx)
{
    ASSERT(w25qxx);

    int ret;

    ze_u8_t wr_data[] = {MANUFACTURER_DEVICE_ID, DUMMY_DATA, DUMMY_DATA, DUMMY_DATA};

    ze_u16_t manufacturer_device_id;
    ze_u8_t rd_data[2];

    if ((ret = spi_write_then_read(w25qxx->spi, wr_data, sizeof(wr_data), rd_data, 2)) < 0)
        return ret;

    manufacturer_device_id = ((rd_data[0] << 8) | rd_data[1]);

    return manufacturer_device_id;
}

void w25qxx_init(w25qxx_t* w25qxx, spi_t* spi)
{
    ASSERT(w25qxx);
    ASSERT(spi);
    w25qxx->spi = spi;
}