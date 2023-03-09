// SPDX-License-Identifier: GPL-2.0
/* dwmac-cvitek.c - Bitmain DWMAC specific glue layer
 *
 * Copyright (c) 2019 Cvitek Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/stmmac.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/of_net.h>
#include <linux/of_gpio.h>
#include <linux/io.h>

#include "stmmac_platform.h"

struct cvitek_mac {
	struct device *dev;
	struct reset_control *rst;
	struct clk *clk_tx;
	struct clk *gate_clk_500m;
	struct clk *gate_clk_axi4;
	struct gpio_desc *reset;
};

static u64 bm_dma_mask = DMA_BIT_MASK(40);

static int bm_eth_reset_phy(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int phy_reset_gpio;
	u32	ephy_addr = 0x0;
	void __iomem *ephy_reg;

	if (!np)
		return 0;

	of_property_read_u32(np, "ephy_ctl_reg", &ephy_addr);

	if (ephy_addr) {
		ephy_reg = ioremap(ephy_addr, 0x10);
		writel(readl(ephy_reg) & 0xFFFFFFFC, ephy_reg);
		mdelay(2);
		iounmap(ephy_reg);
	}

	phy_reset_gpio = of_get_named_gpio(np, "phy-reset-gpios", 0);

	if (phy_reset_gpio < 0)
		return 0;

	if (gpio_request(phy_reset_gpio, "eth-phy-reset"))
		return 0;

	/* RESET_PU */
	gpio_direction_output(phy_reset_gpio, 0);
	mdelay(20);

	gpio_direction_output(phy_reset_gpio, 1);
	/* RC charging time */
	mdelay(60);

	return 0;
}

void bm_dwmac_exit(struct platform_device *pdev, void *priv)
{
	struct cvitek_mac *bsp_priv = priv;

	clk_disable_unprepare(bsp_priv->gate_clk_500m);
	clk_disable_unprepare(bsp_priv->gate_clk_axi4);
}

static int bm_dwmac_probe(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	struct cvitek_mac *bsp_priv = NULL;
	int ret;

	pdev->dev.dma_mask = &bm_dma_mask;
	pdev->dev.coherent_dma_mask = bm_dma_mask;

	bm_eth_reset_phy(pdev);

	ret = stmmac_get_platform_resources(pdev, &stmmac_res);
	if (ret)
		return ret;

	plat_dat = stmmac_probe_config_dt(pdev, &stmmac_res.mac);
	if (IS_ERR(plat_dat))
		return PTR_ERR(plat_dat);

	ret = stmmac_dvr_probe(&pdev->dev, plat_dat, &stmmac_res);
	if (ret)
		goto err_remove_config_dt;

	bsp_priv = devm_kzalloc(&pdev->dev, sizeof(*bsp_priv), GFP_KERNEL);
	if (!bsp_priv)
		return PTR_ERR(bsp_priv);

	bsp_priv->dev = &pdev->dev;

	/* clock setup */
	bsp_priv->gate_clk_500m = devm_clk_get(&pdev->dev, "clk_500m_eth");

	if (IS_ERR(bsp_priv->gate_clk_500m))
		dev_warn(&pdev->dev, "Cannot get clk_500m_eth!\n");
	else
		clk_prepare_enable(bsp_priv->gate_clk_500m);

	bsp_priv->gate_clk_axi4 = devm_clk_get(&pdev->dev, "clk_axi4_eth");

	if (IS_ERR(bsp_priv->gate_clk_axi4))
		dev_warn(&pdev->dev, "Cannot get gate_clk_axi4!\n");
	else
		clk_prepare_enable(bsp_priv->gate_clk_axi4);

	plat_dat->bsp_priv = bsp_priv;
	plat_dat->exit = bm_dwmac_exit;

	return 0;

err_remove_config_dt:
	stmmac_remove_config_dt(pdev, plat_dat);

	return ret;
}

static const struct of_device_id bm_dwmac_match[] = {
	{ .compatible = "cvitek,ethernet" },
	{ }
};
MODULE_DEVICE_TABLE(of, bm_dwmac_match);

static struct platform_driver bm_dwmac_driver = {
	.probe  = bm_dwmac_probe,
	.remove = stmmac_pltfr_remove,
	.driver = {
		.name           = "bm-dwmac",
		.pm		= &stmmac_pltfr_pm_ops,
		.of_match_table = bm_dwmac_match,
	},
};
module_platform_driver(bm_dwmac_driver);

MODULE_AUTHOR("Wei Huang<wei.huang01@bitmain.com>");
MODULE_DESCRIPTION("Cvitek DWMAC specific glue layer");
MODULE_LICENSE("GPL");
