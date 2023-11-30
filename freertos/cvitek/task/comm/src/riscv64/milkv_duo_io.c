#include "milkv_duo_io.h"

void duo_led_control(int enable)
{
	*(uint32_t*)(GPIO2 | GPIO_SWPORTA_DDR) = 1 << 24;

	if (enable) {
		*(uint32_t*)(GPIO2 | GPIO_SWPORTA_DR) = 1 << 24;
	} else {
		*(uint32_t*)(GPIO2 | GPIO_SWPORTA_DR) = 0;
	}
}
