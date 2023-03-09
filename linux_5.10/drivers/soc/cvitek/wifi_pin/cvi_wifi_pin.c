// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CVITEK Wifi pin definition driver
 *
 * Copyright 2020 CVITEK Inc.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>

struct cvi_wifi_pin_dev {
	struct device *dev;
	int power_gpio;
	int wakeup_gpio;
};

static struct cvi_wifi_pin_dev *wifi_dev;

int cvi_get_wifi_pwr_on_gpio(void)
{
	if (wifi_dev) {
		if (wifi_dev->power_gpio > 0)
			return wifi_dev->power_gpio;

		pr_err("Wifi power pin is not available, plz check dts\n");
		return 0;
	}

	pr_err("Wifi power pin is not available, plz check wifi_pin node in dts\n");
	return 0;
}
EXPORT_SYMBOL_GPL(cvi_get_wifi_pwr_on_gpio);

int cvi_get_wifi_wakeup_gpio(void)
{
	if (wifi_dev) {
		if (wifi_dev->wakeup_gpio > 0)
			return wifi_dev->wakeup_gpio;

		pr_err("Wifi wakeup pin is not available, plz check dts\n");
		return 0;
	}

	pr_err("Wifi wakeup pin is not available, plz check wifi_pin node in dts\n");
	return 0;
}
EXPORT_SYMBOL_GPL(cvi_get_wifi_wakeup_gpio);

static int cvi_wifi_pin_probe(struct platform_device *pdev)
{

	struct device_node *np = pdev->dev.of_node;

	dev_dbg(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	wifi_dev = devm_kzalloc(&pdev->dev, sizeof(*wifi_dev), GFP_KERNEL);
	if (!wifi_dev)
		return -ENOMEM;

	if (np) {
		wifi_dev->power_gpio = of_get_named_gpio(np, "poweron-gpio", 0);
		wifi_dev->wakeup_gpio = of_get_named_gpio(np, "wakeup-gpio", 0);
	}

	return 0;
}

static const struct of_device_id cvi_wifi_pin_id_match[] = {
	{
		.compatible = "cvitek,wifi-pin",
	},
	{},
};

static struct platform_driver cvi_wifi_pin_driver = {
	.driver = {
		.name = "cvi_wifi_pin",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(cvi_wifi_pin_id_match),
	},
	.probe = cvi_wifi_pin_probe,
};

static int __init cvi_wifi_pin_init(void)
{
	return platform_driver_register(&cvi_wifi_pin_driver);
}

late_initcall(cvi_wifi_pin_init);
