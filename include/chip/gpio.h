/**
 * @file gpio.h
 * @brief This is the difinition of gpio.
 * @author yuanle
 * @version 0.1
 * @details Change history:
 * <Data>        |<Version>    |<Author>        |<Description>
 * -----------------------------------------------------------
 * 2019/06/10    |0.1          |yuanle          |Initial Version
 * -----------------------------------------------------------
 * @date 2019-06-10
 * @license Dual BSD/GPL
 */
#ifndef __GPIO_H__
#define __GPIO_H__
#include <common/zero.h>

#ifdef __cplusplus
extern "C"{
#endif

#define GPIO_DIR_OUTPUT         0
#define GPIO_DIR_INPUT          1

#define GPIO_LEVEL_LOW          0
#define GPIO_LEVEL_HIGH         1


typedef struct gpio_chip {
    int base;
    ze_u16_t ngpio;
    int (*set_level)(struct gpio_chip* chip, ze_base_t pin, int value);
    int (*get_level)(struct gpio_chip* chip, ze_base_t pin);
    int (*set_dir)(struct gpio_chip* chip, ze_base_t pin, int value);
    int (*get_dir)(struct gpio_chip* chip, ze_base_t pin);
} gpio_chip_t;


/**
 * @brief set gpio level.
 * @param gpio: gpio io index.
 * @param value: gpio level.
 * @return error code. If retval < 0, execute failed. If retval == 0, execute success.
 */
int gpio_set_level(int gpio, int value);

/**
 * @brief get gpio level.
 * @param gpio: gpio io index.
 * @return gpio level. If retval < 0, execute failed. Otherwise gpio level.
 */
int gpio_get_level(int gpio);


/**
 * @brief set gpio direction.
 * @param gpio: gpio io index.
 * @param value: gpio direction.
 * @return error code. If retval < 0, execute failed. If retval == 0, execute success.
 */
int gpio_set_dir(int gpio, int value);

/**
 * @brief get gpio direction.
 * @param gpio: gpio io index.
 * @return gpio direction. If retval < 0, execute failed. Otherwise gpio direction.
 */
int gpio_get_dir(int gpio);


/**
 * @brief add gpio chip to system
 * @param chip: gpio chip.
 * @return error code. If retval < 0, execute failed. Otherwise gpio direction.
 */
int gpiochip_add(gpio_chip_t* chip);

int gpiolib_init(void);

#ifdef __cplusplus
}
#endif
#endif