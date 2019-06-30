#include <chip/gpio.h>
#include <string.h>

#ifndef GPIO_NR
#define GPIO_NR             32
#endif

#define GPIO_FLAG_RESERVED          0
#define GPIO_FLAG_USED              1

typedef struct {
    gpio_chip_t* gpio_chip;
} gpio_desc_t;

static gpio_desc_t gpio_descs[GPIO_NR];


/**
 * @brief set gpio level.
 * @param gpio: gpio io index.
 * @param value: gpio level.
 * @return error code. If retval < 0, execute failed. If retval == 0, execute success.
 */
int gpio_set_level(int gpio, int value)
{
    ASSERT(gpio < GPIO_NR);

    gpio_chip_t* gpio_chip = gpio_descs[gpio].gpio_chip;
    if (!gpio_chip)
        return ENO_ERROR;
    int pin = gpio - gpio_chip->base;
    return gpio_chip->set_level(gpio_chip, pin, value);
}

/**
 * @brief get gpio level.
 * @param gpio: gpio io index.
 * @return gpio level. If retval < 0, execute failed. Otherwise gpio level.
 */
int gpio_get_level(int gpio)
{
    ASSERT(gpio < GPIO_NR);

    gpio_chip_t* gpio_chip = gpio_descs[gpio].gpio_chip;
    if (!gpio_chip)
        return ENO_ERROR;
    int pin = gpio - gpio_chip->base;
    return gpio_chip->get_level(gpio_chip, pin);
}


/**
 * @brief set gpio direction.
 * @param gpio: gpio io index.
 * @param value: gpio direction.
 * @return error code. If retval < 0, execute failed. If retval == 0, execute success.
 */
int gpio_set_dir(int gpio, int value)
{
    ASSERT(gpio < GPIO_NR);

    gpio_chip_t* gpio_chip = gpio_descs[gpio].gpio_chip;
    if (!gpio_chip)
        return ENO_ERROR;
    int pin = gpio - gpio_chip->base;
    return gpio_chip->set_dir(gpio_chip, pin, value);
}

/**
 * @brief get gpio direction.
 * @param gpio: gpio io index.
 * @return gpio direction. If retval < 0, execute failed. Otherwise gpio direction.
 */
int gpio_get_dir(int gpio)
{
    ASSERT(gpio < GPIO_NR);

    gpio_chip_t* gpio_chip = gpio_descs[gpio].gpio_chip;
    if (!gpio_chip)
        return ENO_ERROR;
    int pin = gpio - gpio_chip->base;
    return gpio_chip->get_dir(gpio_chip, pin);
}


/**
 * @brief add gpio chip to system
 * @param gpio_chip: gpio chip.
 * @return error code. If retval < 0, execute failed. Otherwise gpio direction.
 */
int gpiochip_add(gpio_chip_t* gpio_chip)
{
    for (int base = gpio_chip->base; base < gpio_chip->base + gpio_chip->ngpio; base++)
    {
        gpio_descs[base].gpio_chip = gpio_chip;
    }

    return 0;
}

int gpiolib_init(void)
{
    memset(gpio_descs, 0, sizeof(gpio_descs));

    return 0;
}