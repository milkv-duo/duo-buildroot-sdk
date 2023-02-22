/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 */

#include <common.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <fdtdec.h>
#include <panel.h>
#include <regmap.h>
#include <syscon.h>
#include <asm/gpio.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <dm/uclass-internal.h>
#include <linux/kernel.h>

#include "vip_common.h"
#include "scaler.h"
#include "dsi_phy.h"

DECLARE_GLOBAL_DATA_PTR;

#define MHz 1000000

/*
 * Private information for cvi lvds
 *
 * @regs: lvds controller address
 * @panel: panel assined by device tree
 * @ref_clk: reference clock for lvds dsi pll
 * @sysclk: config clock for lvds dsi register
 * @pix_clk: pixel clock for vop->dsi data transmission
 * @phy_clk: lvds dphy output clock
 * @txbyte_clk: clock for dsi->dphy high speed data transmission
 * @txesc_clk: clock for tx esc mode
 */
struct cvi_lvds_priv {
	struct udevice *panel;
	u32 ref_clk;
	u32 sys_clk;
	u32 pix_clk;
	u32 phy_clk;
	u32 txbyte_clk;
	u32 txesc_clk;
};

static int cvi_lvds_read_timing(struct udevice *dev, struct display_timing *timing)
{
	int ret;

	ret = fdtdec_decode_display_timing(gd->fdt_blob, dev_of_offset(dev), 0, timing);
	if (ret) {
		debug("%s: Failed to decode display timing (ret=%d)\n",
		      __func__, ret);
		return -EINVAL;
	}

	return 0;
}

/*
 * This function is called by cvi_display_init() using cvi_lvds_enable() and
 * cvi_lvds_phy_enable() to initialize lvds controller and dphy. If success,
 * enable backlight.
 */
static int cvi_lvds_enable(struct udevice *dev, int panel_bpp, const struct display_timing *timing)
{
	int ret = 0;
	struct cvi_lvds_priv *priv = dev_get_priv(dev);

	/* Fill the lvds controller parameter */
	priv->ref_clk = 24 * MHz;
	priv->sys_clk = priv->ref_clk;
	priv->pix_clk = timing->pixelclock.typ;
	priv->phy_clk = priv->pix_clk * 6;
	priv->txbyte_clk = priv->phy_clk / 8;
	priv->txesc_clk = 20 * MHz;

#if 0
	/* Config  and enable lvds dsi according to timing */
	ret = rk_lvds_enable(dev, timing);
	if (ret) {
		debug("%s: rk_lvds_enable() failed (err=%d)\n",
		      __func__, ret);
		return ret;
	}

	/* Config and enable lvds phy */
	ret = rk_lvds_phy_enable(dev);
	if (ret) {
		debug("%s: rk_lvds_phy_enable() failed (err=%d)\n",
		      __func__, ret);
		return ret;
	}

	/* Enable backlight */
	ret = panel_enable_backlight(priv->panel);
	if (ret) {
		debug("%s: panel_enable_backlight() failed (err=%d)\n",
		      __func__, ret);
		return ret;
	}
#endif

	return ret;
}

static int cvi_lvds_ofdata_to_platdata(struct udevice *dev)
{
	struct cvi_lvds_priv *priv = dev_get_priv(dev);

	priv = priv;

#if 0
	priv->grf = syscon_get_first_range(ROCKCHIP_SYSCON_GRF);
	if (priv->grf <= 0) {
		debug("%s: Get syscon grf failed (ret=%llu)\n",
		      __func__, (u64)priv->grf);
		return  -ENXIO;
	}
#endif
	return 0;
}

/*
 * Probe function: check panel existence and readingit's timing. Then config
 * lvds dsi controller and enable it according to the timing parameter.
 */
static int cvi_lvds_probe(struct udevice *dev)
{
	int ret = 0;
	struct cvi_lvds_priv *priv = dev_get_priv(dev);

	priv = priv;

#if 0
	ret = uclass_get_device_by_phandle(UCLASS_PANEL, dev, "cvitek,panel",
					   &priv->panel);
	if (ret) {
		debug("%s: Can not find panel (err=%d)\n", __func__, ret);
		return ret;
	}
#endif

	return ret;
}

static const struct dm_display_ops cvi_lvds_ops = {
	.read_timing = cvi_lvds_read_timing,
	.enable = cvi_lvds_enable,
};

static const struct udevice_id cvi_lvds_ids[] = {
	{ .compatible = "cvitek,lvds" },
	{ }
};

U_BOOT_DRIVER(cvi_lvds) = {
	.name	= "cvi_lvds",
	.id	= UCLASS_DISPLAY,
	.of_match = cvi_lvds_ids,
	.ofdata_to_platdata = cvi_lvds_ofdata_to_platdata,
	.probe	= cvi_lvds_probe,
	.ops	= &cvi_lvds_ops,
	.priv_auto_alloc_size   = sizeof(struct cvi_lvds_priv),
};

