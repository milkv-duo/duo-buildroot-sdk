// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * CVITEK Bluetooth pin definition driver
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

struct cvi_bt_pin_dev {
	struct device *dev;
	unsigned int power_gpio;
};

static struct cvi_bt_pin_dev *bt_dev;

int cvi_get_bt_pwr_on_gpio(void)
{
	if (bt_dev) {
		if (bt_dev->power_gpio > 0)
			return bt_dev->power_gpio;

		pr_err("Bluetooth power pin is not available, plz check dts\n");
		return 0;
	}

	pr_err("Bluetooth power pin is not available, plz check bt_pin node in dts\n");
	return 0;
}
EXPORT_SYMBOL_GPL(cvi_get_bt_pwr_on_gpio);

static int cvi_bt_pin_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;

	dev_dbg(&pdev->dev, "%s, dev name=%s\n", __func__, dev_name(&pdev->dev));

	bt_dev = devm_kzalloc(&pdev->dev, sizeof(*bt_dev), GFP_KERNEL);
	if (!bt_dev)
		return -ENOMEM;

	if (np)
		bt_dev->power_gpio = of_get_named_gpio(np, "poweron-gpio", 0);

	return 0;
}

static const struct of_device_id cvi_bt_pin_id_match[] = {
	{
		.compatible = "cvitek,bt-pin",
	},
	{},
};

static struct platform_driver cvi_bt_pin_driver = {
	.driver = {
		.name = "cvi_bt_pin",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(cvi_bt_pin_id_match),
	},
	.probe = cvi_bt_pin_probe,
};

static int __init cvi_bt_pin_init(void)
{
	return platform_driver_register(&cvi_bt_pin_driver);
}

late_initcall(cvi_bt_pin_init);
