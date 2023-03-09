/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <display.h>
#include <dm.h>
#include <regmap.h>
#include <video.h>
//#include <asm/hardware.h>
#include <asm/io.h>
#include <dt-structs.h>
#include <asm/gpio.h>
//#include <asm-generic/gpio.h>
#include "vip_common.h"
#include "scaler.h"
#include "dsi_phy.h"

DECLARE_GLOBAL_DATA_PTR;

struct cvi_vo_priv {
	phys_addr_t regs_sc;
	phys_addr_t regs_vip;
	phys_addr_t regs_dphy;
	struct disp_ctrl_gpios ctrl_gpios;
};

static int cvi_vo_bind(struct udevice *dev)
{
	//struct video_uc_plat *plat = dev_get_uclass_plat(dev);
	//plat->size = 4 * (CONFIG_VIDEO_CVITEK_MAX_XRES *
	//CONFIG_VIDEO_CVITEK_MAX_YRES);

	return 0;
}

static int cvi_vo_ofdata_to_platdata(struct udevice *dev)
{
	struct cvi_vo_priv *priv = dev_get_priv(dev);

	priv->regs_sc = devfdt_get_addr_name(dev, "sc");
	if (priv->regs_sc == FDT_ADDR_T_NONE) {
		printf("%s: Get VO sc address failed (ret=%llu)\n", __func__, (u64)priv->regs_sc);
		return -ENXIO;
	}
	priv->regs_vip = devfdt_get_addr_name(dev, "vip_sys");
	if (priv->regs_vip == FDT_ADDR_T_NONE) {
		printf("%s: Get dsi address failed (ret=%llu)\n", __func__, (u64)priv->regs_vip);
		return -ENXIO;
	}
	priv->regs_dphy = devfdt_get_addr_name(dev, "dphy");
	if (priv->regs_dphy == FDT_ADDR_T_NONE) {
		printf("%s: Get MIPI dsi address failed (ret=%llu)\n", __func__, (u64)priv->regs_dphy);
		return -ENXIO;
	}
	debug("%s: base(sc)=%#llx base(vip)=%#llx base(dphy)=%#llx\n", __func__
	     , priv->regs_sc, priv->regs_vip, priv->regs_dphy);
	return 0;
}

static int cvi_vo_probe(struct udevice *dev)
{
	//struct video_uc_platdata *plat = dev_get_uclass_platdata(dev);
	//const void *blob = gd->fdt_blob;
	struct cvi_vo_priv *priv = dev_get_priv(dev);
	int ret = 0;

	/* Before relocation we don't need to do anything */
	if (!(gd->flags & GD_FLG_RELOC))
		return 0;

	sclr_set_base_addr((void *)priv->regs_sc);
	vip_set_base_addr((void *)priv->regs_vip);
	dphy_set_base_addr((void *)priv->regs_dphy);

#ifdef BOOTLOGO_ISP_RESET
	vip_isp_clk_reset();
#endif

	sclr_ctrl_init();
	sclr_ctrl_set_disp_src(false);

	ret = gpio_request_by_name(dev, "reset-gpio", 0, &priv->ctrl_gpios.disp_reset_gpio,
				   GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (ret) {
		printf("%s: Warning: cannot get reset GPIO: ret=%d\n", __func__, ret);
		if (ret != -ENOENT)
			return ret;
	}
	ret = gpio_request_by_name(dev, "pwm-gpio", 0, &priv->ctrl_gpios.disp_pwm_gpio,
				   GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (ret) {
		printf("%s: Warning: cannot get pwm GPIO: ret=%d\n", __func__, ret);
		if (ret != -ENOENT)
			return ret;
	}
	ret = gpio_request_by_name(dev, "power-ct-gpio", 0, &priv->ctrl_gpios.disp_power_ct_gpio,
				   GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (ret) {
		printf("%s: Warning: cannot get power GPIO: ret=%d\n", __func__, ret);
		if (ret != -ENOENT)
			return ret;
	}
	set_disp_ctrl_gpios(&priv->ctrl_gpios);

	video_set_flush_dcache(dev, 1);

	return ret;
}

static const struct udevice_id cvi_vo_ids[] = {
	{ .compatible = "cvitek,vo" },
	{ }
};

static const struct video_ops cvi_vo_ops = {
};

U_BOOT_DRIVER(cvi_vo) = {
	.name	= "vo",
	.id	= UCLASS_VIDEO,
	.of_match = cvi_vo_ids,
	.ops	= &cvi_vo_ops,
	.bind	= cvi_vo_bind,
	.probe	= cvi_vo_probe,
	.of_to_plat = cvi_vo_ofdata_to_platdata,
	.priv_auto = sizeof(struct cvi_vo_priv),
};
