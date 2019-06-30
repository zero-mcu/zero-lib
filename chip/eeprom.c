#include <chip/eeprom.h>

int eeprom_write(eeprom_t* eeprom, ze_u32_t addr, const ze_u8_t* wr_data, ze_size_t wr_len)
{
    ASSERT(eeprom);
    ASSERT(wr_data);
    ASSERT(eeprom->ops);
    ASSERT(eeprom->ops->write);

    return eeprom->ops->write(eeprom, addr, wr_data, wr_len);
}

int eeprom_read(eeprom_t* eeprom, ze_u32_t addr, ze_u8_t* rd_data, ze_size_t rd_len)
{
    ASSERT(eeprom);
    ASSERT(rd_data);
    ASSERT(eeprom->ops);
    ASSERT(eeprom->ops->read);

    return eeprom->ops->read(eeprom, addr, rd_data, rd_len);
}

void eeprom_init(eeprom_t* eeprom, const eeprom_ops_t* ops)
{
    ASSERT(eeprom);
    ASSERT(ops);

    eeprom->ops = ops;
}