/******************************************************************************
 *
 * Copyright(c) 2018 bitmain.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include <drv_types.h>
#include <linux/gpio.h>

int platform_wifi_power_on(void)
{
	int err = 0;
	RTW_INFO("*** power on rtl8821fu ***\n");
	return err;
}

void platform_wifi_power_off(void)
{
	RTW_INFO("*** power off rtl8821fu ***\n");
}
