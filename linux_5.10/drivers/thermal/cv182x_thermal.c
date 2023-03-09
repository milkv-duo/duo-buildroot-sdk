// SPDX-License-Identifier: GPL-2.0
/*
 * CVITEK CV182X thermal driver
 *
 * Copyright 2021 CVITEK Inc.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/thermal.h>
#include <linux/types.h>

#define tempsen_top_tempsen_version				0x0
#define tempsen_top_tempsen_ctrl				0x4
#define tempsen_top_tempsen_status				0x8
#define tempsen_top_tempsen_set					0xc
#define tempsen_top_tempsen_intr_en				0x10
#define tempsen_top_tempsen_intr_clr				0x14
#define tempsen_top_tempsen_intr_sta				0x18
#define tempsen_top_tempsen_intr_raw				0x1c
#define tempsen_top_tempsen_ch0_result				0x20
#define tempsen_top_tempsen_ch1_result				0x24
#define tempsen_top_tempsen_ch2_result				0x28
#define tempsen_top_tempsen_ch3_result				0x2c
#define tempsen_top_tempsen_ch0_temp_th				0x40
#define tempsen_top_tempsen_ch1_temp_th				0x44
#define tempsen_top_tempsen_ch2_temp_th				0x48
#define tempsen_top_tempsen_ch3_temp_th				0x4c
#define tempsen_top_Overheat_th					0x60
#define tempsen_top_tempsen_auto_cycle				0x64
#define tempsen_top_tempsen_auto_prediv				0x64
#define tempsen_top_tempsen_overheat_ctrl			0x68
#define tempsen_top_tempsen_overheat_countdown			0x6c
#define tempsen_top_tempsen_ch0_temp_th_cnt			0x70
#define tempsen_top_tempsen_ch1_temp_th_cnt			0x74
#define tempsen_top_tempsen_ch2_temp_th_cnt			0x78
#define tempsen_top_tempsen_ch3_temp_th_cnt			0x7c
#define tempsen_top_tempsen_test_force				0x80
#define tempsen_top_reg_ip_version				0x0
#define tempsen_top_reg_ip_version_OFFSET			0
#define tempsen_top_reg_ip_version_MASK				0xffffffff
#define tempsen_top_reg_tempsen_en				0x4
#define tempsen_top_reg_tempsen_en_OFFSET			0
#define tempsen_top_reg_tempsen_en_MASK				0x1
#define tempsen_top_reg_tempsen_sel				0x4
#define tempsen_top_reg_tempsen_sel_OFFSET			4
#define tempsen_top_reg_tempsen_sel_MASK			0xf0
#define tempsen_top_reg_tempsen_ovhl_cnt_to_irq			0x4
#define tempsen_top_reg_tempsen_ovhl_cnt_to_irq_OFFSET		16
#define tempsen_top_reg_tempsen_ovhl_cnt_to_irq_MASK		0xff0000
#define tempsen_top_reg_tempsen_udll_cnt_to_irq			0x4
#define tempsen_top_reg_tempsen_udll_cnt_to_irq_OFFSET		24
#define tempsen_top_reg_tempsen_udll_cnt_to_irq_MASK		0xff000000
#define tempsen_top_sta_tempsen_busy				0x8
#define tempsen_top_sta_tempsen_busy_OFFSET			0
#define tempsen_top_sta_tempsen_busy_MASK			0x1
#define tempsen_top_reg_tempsen_bgen				0xc
#define tempsen_top_reg_tempsen_bgen_OFFSET			0
#define tempsen_top_reg_tempsen_bgen_MASK			0x1
#define tempsen_top_reg_tempsen_chopen				0xc
#define tempsen_top_reg_tempsen_chopen_OFFSET			1
#define tempsen_top_reg_tempsen_chopen_MASK			0x2
#define tempsen_top_reg_tempsen_choppol				0xc
#define tempsen_top_reg_tempsen_choppol_OFFSET			2
#define tempsen_top_reg_tempsen_choppol_MASK			0x4
#define tempsen_top_reg_tempsen_clkpol				0xc
#define tempsen_top_reg_tempsen_clkpol_OFFSET			3
#define tempsen_top_reg_tempsen_clkpol_MASK			0x8
#define tempsen_top_reg_tempsen_chopsel				0xc
#define tempsen_top_reg_tempsen_chopsel_OFFSET			4
#define tempsen_top_reg_tempsen_chopsel_MASK			0x30
#define tempsen_top_reg_tempsen_accsel				0xc
#define tempsen_top_reg_tempsen_accsel_OFFSET			6
#define tempsen_top_reg_tempsen_accsel_MASK			0xc0
#define tempsen_top_reg_tempsen_cyc_clkdiv			0xc
#define tempsen_top_reg_tempsen_cyc_clkdiv_OFFSET		8
#define tempsen_top_reg_tempsen_cyc_clkdiv_MASK			0xff00
#define tempsen_top_reg_tempsen_tsel				0xc
#define tempsen_top_reg_tempsen_tsel_OFFSET			16
#define tempsen_top_reg_tempsen_tsel_MASK			0x30000
#define tempsen_top_sta_tempsen_intr_en				0x10
#define tempsen_top_sta_tempsen_intr_en_OFFSET			0
#define tempsen_top_sta_tempsen_intr_en_MASK			0xffffffff
#define tempsen_top_sta_tempsen_intr_clr			0x14
#define tempsen_top_sta_tempsen_intr_clr_OFFSET			0
#define tempsen_top_sta_tempsen_intr_clr_MASK			0xffffffff
#define tempsen_top_sta_tempsen_intr_sta			0x18
#define tempsen_top_sta_tempsen_intr_sta_OFFSET			0
#define tempsen_top_sta_tempsen_intr_sta_MASK			0xffffffff
#define tempsen_top_sta_tempsen_intr_raw			0x1c
#define tempsen_top_sta_tempsen_intr_raw_OFFSET			0
#define tempsen_top_sta_tempsen_intr_raw_MASK			0xffffffff
#define tempsen_top_sta_tempsen_ch0_result			0x20
#define tempsen_top_sta_tempsen_ch0_result_OFFSET		0
#define tempsen_top_sta_tempsen_ch0_result_MASK			0x1fff
#define tempsen_top_sta_tempsen_ch0_max_result			0x20
#define tempsen_top_sta_tempsen_ch0_max_result_OFFSET		16
#define tempsen_top_sta_tempsen_ch0_max_result_MASK		0x1fff0000
#define tempsen_top_clr_tempsen_ch0_max_result			0x20
#define tempsen_top_clr_tempsen_ch0_max_result_OFFSET		31
#define tempsen_top_clr_tempsen_ch0_max_result_MASK		0x80000000
#define tempsen_top_sta_tempsen_ch1_result			0x24
#define tempsen_top_sta_tempsen_ch1_result_OFFSET		0
#define tempsen_top_sta_tempsen_ch1_result_MASK			0x1fff
#define tempsen_top_sta_tempsen_ch1_max_result			0x24
#define tempsen_top_sta_tempsen_ch1_max_result_OFFSET		16
#define tempsen_top_sta_tempsen_ch1_max_result_MASK		0x1fff0000
#define tempsen_top_clr_tempsen_ch1_max_result			0x24
#define tempsen_top_clr_tempsen_ch1_max_result_OFFSET		31
#define tempsen_top_clr_tempsen_ch1_max_result_MASK		0x80000000
#define tempsen_top_sta_tempsen_ch2_result			0x28
#define tempsen_top_sta_tempsen_ch2_result_OFFSET		0
#define tempsen_top_sta_tempsen_ch2_result_MASK			0x1fff
#define tempsen_top_sta_tempsen_ch2_max_result			0x28
#define tempsen_top_sta_tempsen_ch2_max_result_OFFSET		16
#define tempsen_top_sta_tempsen_ch2_max_result_MASK		0x1fff0000
#define tempsen_top_clr_tempsen_ch2_max_result			0x28
#define tempsen_top_clr_tempsen_ch2_max_result_OFFSET		31
#define tempsen_top_clr_tempsen_ch2_max_result_MASK		0x80000000
#define tempsen_top_sta_tempsen_ch3_result			0x2c
#define tempsen_top_sta_tempsen_ch3_result_OFFSET		0
#define tempsen_top_sta_tempsen_ch3_result_MASK			0x1fff
#define tempsen_top_sta_tempsen_ch3_max_result			0x2c
#define tempsen_top_sta_tempsen_ch3_max_result_OFFSET		16
#define tempsen_top_sta_tempsen_ch3_max_result_MASK		0x1fff0000
#define tempsen_top_clr_tempsen_ch3_max_result			0x2c
#define tempsen_top_clr_tempsen_ch3_max_result_OFFSET		31
#define tempsen_top_clr_tempsen_ch3_max_result_MASK		0x80000000
#define tempsen_top_reg_tempsen_ch0_hi_th			0x40
#define tempsen_top_reg_tempsen_ch0_hi_th_OFFSET		0
#define tempsen_top_reg_tempsen_ch0_hi_th_MASK			0x1fff
#define tempsen_top_reg_tempsen_ch0_lo_th			0x40
#define tempsen_top_reg_tempsen_ch0_lo_th_OFFSET		16
#define tempsen_top_reg_tempsen_ch0_lo_th_MASK			0x1fff0000
#define tempsen_top_reg_tempsen_ch1_hi_th			0x44
#define tempsen_top_reg_tempsen_ch1_hi_th_OFFSET		0
#define tempsen_top_reg_tempsen_ch1_hi_th_MASK			0x1fff
#define tempsen_top_reg_tempsen_ch1_lo_th			0x44
#define tempsen_top_reg_tempsen_ch1_lo_th_OFFSET		16
#define tempsen_top_reg_tempsen_ch1_lo_th_MASK			0x1fff0000
#define tempsen_top_reg_tempsen_ch2_hi_th			0x48
#define tempsen_top_reg_tempsen_ch2_hi_th_OFFSET		0
#define tempsen_top_reg_tempsen_ch2_hi_th_MASK			0x1fff
#define tempsen_top_reg_tempsen_ch2_lo_th			0x48
#define tempsen_top_reg_tempsen_ch2_lo_th_OFFSET		16
#define tempsen_top_reg_tempsen_ch2_lo_th_MASK			0x1fff0000
#define tempsen_top_reg_tempsen_ch3_hi_th			0x4c
#define tempsen_top_reg_tempsen_ch3_hi_th_OFFSET		0
#define tempsen_top_reg_tempsen_ch3_hi_th_MASK			0x1fff
#define tempsen_top_reg_tempsen_ch3_lo_th			0x4c
#define tempsen_top_reg_tempsen_ch3_lo_th_OFFSET		16
#define tempsen_top_reg_tempsen_ch3_lo_th_MASK			0x1fff0000
#define tempsen_top_reg_tempsen_overheat_th			0x60
#define tempsen_top_reg_tempsen_overheat_th_OFFSET		0
#define tempsen_top_reg_tempsen_overheat_th_MASK		0x1fff
#define tempsen_top_reg_tempsen_auto_cycle			0x64
#define tempsen_top_reg_tempsen_auto_cycle_OFFSET		0
#define tempsen_top_reg_tempsen_auto_cycle_MASK			0xffffff
#define tempsen_top_reg_tempsen_auto_prediv			0x64
#define tempsen_top_reg_tempsen_auto_prediv_OFFSET		24
#define tempsen_top_reg_tempsen_auto_prediv_MASK		0xff000000
#define tempsen_top_reg_tempsen_overheat_cycle			0x68
#define tempsen_top_reg_tempsen_overheat_cycle_OFFSET		0
#define tempsen_top_reg_tempsen_overheat_cycle_MASK		0x3fffffff
#define tempsen_top_reg_overheat_reset_clr			0x68
#define tempsen_top_reg_overheat_reset_clr_OFFSET		30
#define tempsen_top_reg_overheat_reset_clr_MASK			0x40000000
#define tempsen_top_reg_overheat_reset_en			0x68
#define tempsen_top_reg_overheat_reset_en_OFFSET		31
#define tempsen_top_reg_overheat_reset_en_MASK			0x80000000
#define tempsen_top_sta_tempsen_overheat_countdown		0x6c
#define tempsen_top_sta_tempsen_overheat_countdown_OFFSET	0
#define tempsen_top_sta_tempsen_overheat_countdown_MASK		0x3fffffff
#define tempsen_top_sta_overheat_reset				0x6c
#define tempsen_top_sta_overheat_reset_OFFSET			31
#define tempsen_top_sta_overheat_reset_MASK			0x80000000
#define tempsen_top_sta_ch0_over_hi_temp_th_cnt			0x70
#define tempsen_top_sta_ch0_over_hi_temp_th_cnt_OFFSET		0
#define tempsen_top_sta_ch0_over_hi_temp_th_cnt_MASK		0xff
#define tempsen_top_sta_ch0_under_lo_temp_th_cnt		0x70
#define tempsen_top_sta_ch0_under_lo_temp_th_cnt_OFFSET		8
#define tempsen_top_sta_ch0_under_lo_temp_th_cnt_MASK		0xff00
#define tempsen_top_reg_ch0_temp_th_cnt_clr			0x70
#define tempsen_top_reg_ch0_temp_th_cnt_clr_OFFSET		16
#define tempsen_top_reg_ch0_temp_th_cnt_clr_MASK		0x10000
#define tempsen_top_sta_ch1_over_hi_temp_th_cnt			0x74
#define tempsen_top_sta_ch1_over_hi_temp_th_cnt_OFFSET		0
#define tempsen_top_sta_ch1_over_hi_temp_th_cnt_MASK		0xff
#define tempsen_top_sta_ch1_under_lo_temp_th_cnt		0x74
#define tempsen_top_sta_ch1_under_lo_temp_th_cnt_OFFSET		8
#define tempsen_top_sta_ch1_under_lo_temp_th_cnt_MASK		0xff00
#define tempsen_top_reg_ch1_temp_th_cnt_clr			0x74
#define tempsen_top_reg_ch1_temp_th_cnt_clr_OFFSET		16
#define tempsen_top_reg_ch1_temp_th_cnt_clr_MASK		0x10000
#define tempsen_top_sta_ch2_over_hi_temp_th_cnt			0x78
#define tempsen_top_sta_ch2_over_hi_temp_th_cnt_OFFSET		0
#define tempsen_top_sta_ch2_over_hi_temp_th_cnt_MASK		0xff
#define tempsen_top_sta_ch2_under_lo_temp_th_cnt		0x78
#define tempsen_top_sta_ch2_under_lo_temp_th_cnt_OFFSET		8
#define tempsen_top_sta_ch2_under_lo_temp_th_cnt_MASK		0xff00
#define tempsen_top_reg_ch2_temp_th_cnt_clr			0x78
#define tempsen_top_reg_ch2_temp_th_cnt_clr_OFFSET		16
#define tempsen_top_reg_ch2_temp_th_cnt_clr_MASK		0x10000
#define tempsen_top_sta_ch3_over_hi_temp_th_cnt			0x7c
#define tempsen_top_sta_ch3_over_hi_temp_th_cnt_OFFSET		0
#define tempsen_top_sta_ch3_over_hi_temp_th_cnt_MASK		0xff
#define tempsen_top_sta_ch3_under_lo_temp_th_cnt		0x7c
#define tempsen_top_sta_ch3_under_lo_temp_th_cnt_OFFSET		8
#define tempsen_top_sta_ch3_under_lo_temp_th_cnt_MASK		0xff00
#define tempsen_top_reg_ch3_temp_th_cnt_clr			0x7c
#define tempsen_top_reg_ch3_temp_th_cnt_clr_OFFSET		16
#define tempsen_top_reg_ch3_temp_th_cnt_clr_MASK		0x10000
#define tempsen_top_reg_tempsen_force_result			0x80
#define tempsen_top_reg_tempsen_force_result_OFFSET		0
#define tempsen_top_reg_tempsen_force_result_MASK		0x1fff
#define tempsen_top_reg_tempsen_force_valid			0x80
#define tempsen_top_reg_tempsen_force_valid_OFFSET		13
#define tempsen_top_reg_tempsen_force_valid_MASK		0x2000
#define tempsen_top_reg_tempsen_force_busy			0x80
#define tempsen_top_reg_tempsen_force_busy_OFFSET		14
#define tempsen_top_reg_tempsen_force_busy_MASK			0x4000
#define tempsen_top_reg_tempsen_force_en			0x80
#define tempsen_top_reg_tempsen_force_en_OFFSET			15
#define tempsen_top_reg_tempsen_force_en_MASK			0x8000

#define TEMPSEN_MASK(REG_NAME) tempsen_top_##REG_NAME##_MASK
#define TEMPSEN_OFFSET(REG_NAME) tempsen_top_##REG_NAME##_OFFSET
#define TEMPSEN_SET(BASE_ADDR, REG_NAME, VAL) \
	clrsetbits(BASE_ADDR + tempsen_top_##REG_NAME, \
	TEMPSEN_MASK(REG_NAME), (VAL) << TEMPSEN_OFFSET(REG_NAME))
#define TEMPSEN_GET(BASE_ADDR, REG_NAME) \
	((readl(BASE_ADDR + tempsen_top_##REG_NAME) & \
	TEMPSEN_MASK(REG_NAME)) >> TEMPSEN_OFFSET(REG_NAME))

static void __maybe_unused clrsetbits(void __iomem *reg, u32 clrval, u32 setval)
{
	u32 regval;

	regval = readl(reg);
	regval &= ~(clrval);
	regval |= setval;
	writel(regval, reg);
}

struct cv182x_thermal_zone {
	unsigned int ch;
	void __iomem *base;
	struct cv182x_thermal *ct;
};

struct cv182x_thermal {
	struct device *dev;
	void __iomem *base;
	struct clk *clk_tempsen;
};

static void cv182x_thermal_init(struct cv182x_thermal *ct)
{
	void __iomem *base = ct->base;
	u32 regval;

	/* clear all interrupt status */
	regval = TEMPSEN_GET(base, sta_tempsen_intr_raw);
	TEMPSEN_SET(base, sta_tempsen_intr_clr, regval);

	/* clear max result */
	TEMPSEN_SET(base, clr_tempsen_ch0_max_result, 1);
	TEMPSEN_SET(base, clr_tempsen_ch1_max_result, 1);

	/* set chop period to 3:1024T */
	TEMPSEN_SET(base, reg_tempsen_chopsel, 0x3);

	/* set acc period to 2:2048T*/
	TEMPSEN_SET(base, reg_tempsen_accsel, 0x2);

	/* set tempsen clock divider to 25M/(0x31+1)= 0.5M ,T=2us */
	TEMPSEN_SET(base, reg_tempsen_cyc_clkdiv, 0x31);

	/* set reg_tempsen_auto_cycle */
	TEMPSEN_SET(base, reg_tempsen_auto_cycle, 0x100000);

	/* enable tempsen channel */
	TEMPSEN_SET(base, reg_tempsen_sel, 0x1);
	TEMPSEN_SET(base, reg_tempsen_en, 1);
}

static void cv182x_thermal_uninit(struct cv182x_thermal *ct)
{
	void __iomem *base = ct->base;
	u32 regval;

	/* disable all tempsen channel */
	TEMPSEN_SET(base, reg_tempsen_sel, 0);
	TEMPSEN_SET(base, reg_tempsen_en, 0);

	/* clear all interrupt status */
	regval = TEMPSEN_GET(base, sta_tempsen_intr_raw);
	TEMPSEN_SET(base, sta_tempsen_intr_clr, regval);
}

static int calc_temp(uint32_t result)
{
	return ((result * 1000) * 716 / 2048 - 273000);

	/* Original calculation formula */
	// return ((result * 1000) / 2048 * 716 - 273000);
}

static int cv182x_read_temp(void *data, int *temperature)
{
	struct cv182x_thermal_zone *ctz = data;
	void __iomem *base = ctz->base;
	unsigned int ch = ctz->ch;
	u32 result;

	/* read temperature */
	switch (ch) {
	case 0:
		result = TEMPSEN_GET(base, sta_tempsen_ch0_result); break;
	default:
		result = 0;
	}
	*temperature = calc_temp(result);
	pr_debug("ch%d temp = %d mC(0x%x)\n", ch, *temperature, result);

	return 0;
}

static const struct thermal_zone_of_device_ops cv182x_thermal_ops = {
	.get_temp = cv182x_read_temp,
};

static const struct of_device_id cv182x_thermal_of_match[] = {
	{
		.compatible = "cvitek,cv182x-thermal",
	},
	{},
};
MODULE_DEVICE_TABLE(of, cv182x_thermal_of_match);

static int cv182x_thermal_probe(struct platform_device *pdev)
{
	struct cv182x_thermal *ct;
	struct cv182x_thermal_zone *ctz;
	struct resource *res;
	struct thermal_zone_device *tz;
	int i;

	ct = devm_kzalloc(&pdev->dev, sizeof(*ct), GFP_KERNEL);
	if (!ct)
		return -ENOMEM;

	ct->clk_tempsen = devm_clk_get(&pdev->dev, "clk_tempsen");
	if (IS_ERR(ct->clk_tempsen)) {
		dev_err(&pdev->dev, "failed to get clk_tempsen\n");
		return PTR_ERR(ct->clk_tempsen);
	}

	/* enable clk_tempsen */
	clk_prepare_enable(ct->clk_tempsen);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ct->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(ct->base)) {
		dev_err(&pdev->dev, "failed to map tempsen registers\n");
		return PTR_ERR(ct->base);
	}

	ct->dev = &pdev->dev;

	cv182x_thermal_init(ct);

	platform_set_drvdata(pdev, ct);

	for (i = 0; i < 1; i++) {
		ctz = devm_kzalloc(&pdev->dev, sizeof(*ctz), GFP_KERNEL);
		if (!ctz)
			return -ENOMEM;

		ctz->base = ct->base;
		ctz->ct = ct;
		ctz->ch = i;
		tz = devm_thermal_zone_of_sensor_register(&pdev->dev, i, ctz,
							  &cv182x_thermal_ops);
		if (IS_ERR(tz)) {
			dev_err(&pdev->dev, "failed to register thermal zone %d\n", i);
			return PTR_ERR(tz);
		}
	}

	return 0;
}

static int cv182x_thermal_remove(struct platform_device *pdev)
{
	struct cv182x_thermal *ct = platform_get_drvdata(pdev);

	cv182x_thermal_uninit(ct);

	clk_disable_unprepare(ct->clk_tempsen);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int cv182x_thermal_suspend(struct device *dev)
{
	struct cv182x_thermal *ct = dev_get_drvdata(dev);

	cv182x_thermal_uninit(ct);

	clk_disable_unprepare(ct->clk_tempsen);

	return 0;
}

static int cv182x_thermal_resume(struct device *dev)
{
	struct cv182x_thermal *ct = dev_get_drvdata(dev);

	/* enable clk_tempsen */
	clk_prepare_enable(ct->clk_tempsen);

	cv182x_thermal_init(ct);

	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(cv182x_thermal_pm_ops,
			 cv182x_thermal_suspend, cv182x_thermal_resume);

static struct platform_driver cv182x_thermal_driver = {
	.probe = cv182x_thermal_probe,
	.remove = cv182x_thermal_remove,
	.driver = {
		.name = "cv182x-thermal",
		.pm = &cv182x_thermal_pm_ops,
		.of_match_table = cv182x_thermal_of_match,
	},
};

module_platform_driver(cv182x_thermal_driver);
MODULE_DESCRIPTION("CV182X thermal driver");
