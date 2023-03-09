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
#define BM1880_WIFI_EN_GPIO 493

int platform_wifi_power_on(void)
{
	int err = 0;

	err = gpio_request(BM1880_WIFI_EN_GPIO, "wifi_chip_en");
	if (err < 0) {
		RTW_INFO("request gpio for rtl8821cu failed!\n");
		return err;
	}
	gpio_direction_output(BM1880_WIFI_EN_GPIO, 1);/* enable wifi. */
	RTW_INFO("power on rtl8821cu.\n");
	msleep(500);
	RTW_INFO("[rtl8821cu] %s: new card, power on.\n", __func__);
	return err;
}

void platform_wifi_power_off(void)
{
	gpio_direction_output(BM1880_WIFI_EN_GPIO, 1);/* disable wifi. */
	RTW_INFO("power off rtl8821cu.\n");
	gpio_free(BM1880_WIFI_EN_GPIO);
	RTW_INFO("[rtl8821cu] %s: remove card, power off.\n", __func__);
}
