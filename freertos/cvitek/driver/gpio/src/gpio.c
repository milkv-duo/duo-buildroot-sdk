#include <stdio.h>
#ifdef RUN_IN_SRAM
#include "system_common.h"
#endif

#include "mmio.h"
#include "gpio.h"


int gpio_is_valid(int pin)
{
	switch ((pin >> 8)) {
		case 0xA:
		case 0xB:
		case 0xC:
		case 0xD:
			return 1;
		default:
			return 0;
	}
}

void gpio_direction_output(int pin, int val)
{
	uint32_t gpio_base;

	switch ((pin >> 8)) {
		case 0xB:
			gpio_base = CVI_GPIOB_BASE;
			break;
		case 0xC:
			gpio_base = CVI_GPIOC_BASE;
			break;
		case 0xD:
			gpio_base = CVI_GPIOD_BASE;
			break;
		case 0xA:
			gpio_base = CVI_GPIOA_BASE;
			break;
		default:
			return;
	}

	pin &= 0xff;
	// GPIO_OE
	mmio_write_32(gpio_base + 4, 1 << pin);
	// GPIO_O
	mmio_write_32(gpio_base, val << pin);
}
