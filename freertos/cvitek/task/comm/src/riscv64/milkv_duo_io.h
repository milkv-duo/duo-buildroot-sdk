#include <stdio.h>
/*
 * The blue LED on Duo is GPIOC24.
 * The datasheet page 536 says:
 * Configure register GPIO_SWPORTA_DDR and set GPIO as input or output.
 * When the output pin is configured, write the output value to the
 * GPIO_SWPORTA_DR register to control the GPIO output level.
 */

#define GPIO2 0x03022000
#define GPIO_SWPORTA_DR 0x000
#define GPIO_SWPORTA_DDR 0x004

void duo_led_control(int enable);
