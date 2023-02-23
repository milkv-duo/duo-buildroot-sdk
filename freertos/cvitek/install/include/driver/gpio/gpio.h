#ifndef __GPIO_H__
#define __GPIO_H__

#include <stddef.h>
#include <stdint.h>

#define CVI_GPIOA_BASE		0x03020000
#define CVI_GPIOB_BASE		0x03021000
#define CVI_GPIOC_BASE		0x03022000
#define CVI_GPIOD_BASE		0x03023000

enum of_gpio_flags {
	OF_GPIO_ACTIVE_LOW  = 0x1
};

int gpio_is_valid(int pin);
void gpio_direction_output(int pin, int val);

#endif
