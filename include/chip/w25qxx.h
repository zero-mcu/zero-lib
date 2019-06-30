#ifndef __W25QXX_H__
#define __W25QXX_H__
#include <common/zero.h>
#include <bus/spi.h>


#define MODULE_W25QXX(n)      MODULE_DEFINE(w25qxx, n)
#define IMPORT_W25QXX(n)      IMPORT_MODULE(w25qxx, n)
#define W25QXX(n)             MODULE(w25qxx, n)


typedef struct w25qxx {
    spi_t* spi;
} w25qxx_t;

void w25qxx_init(w25qxx_t* w25qxx, spi_t* spi);

int w25qxx_sector_erase(w25qxx_t* w25qxx, ze_u32_t address);

int w25qxx_block32_erase(w25qxx_t* w25qxx, ze_u32_t address);

int w25qxx_block64_erase(w25qxx_t* w25qxx, ze_u32_t address);

int w25qxx_write(w25qxx_t* w25qxx, ze_u32_t address, ze_u8_t* wr_data, ze_size_t wr_len);

int w25qxx_read(w25qxx_t* w25qxx, ze_u32_t address, ze_u8_t*rd_data, ze_size_t rd_len);

int w25qxx_read_manufacturer_device_id(w25qxx_t* w25qxx);

#endif