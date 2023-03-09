// SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause)
/*
 * platform.c - DesignWare HS OTG Controller platform driver
 *
 * Copyright (C) Matthijs Kooijman <matthijs@stdin.nl>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/of_device.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>
#include <linux/platform_data/s3c-hsotg.h>
#include <linux/reset.h>
#include <linux/of_gpio.h>

#include <linux/usb/of.h>

#include "core.h"
#include "hcd.h"
#include "debug.h"

#if IS_ENABLED(CONFIG_ARCH_CVITEK)
#include <linux/ctype.h>
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#endif
#endif

static const char dwc2_driver_name[] = "dwc2";

/*
 * Check the dr_mode against the module configuration and hardware
 * capabilities.
 *
 * The hardware, module, and dr_mode, can each be set to host, device,
 * or otg. Check that all these values are compatible and adjust the
 * value of dr_mode if possible.
 *
 *                      actual
 *    HW  MOD dr_mode   dr_mode
 *  ------------------------------
 *   HST  HST  any    :  HST
 *   HST  DEV  any    :  ---
 *   HST  OTG  any    :  HST
 *
 *   DEV  HST  any    :  ---
 *   DEV  DEV  any    :  DEV
 *   DEV  OTG  any    :  DEV
 *
 *   OTG  HST  any    :  HST
 *   OTG  DEV  any    :  DEV
 *   OTG  OTG  any    :  dr_mode
 */
static int dwc2_get_dr_mode(struct dwc2_hsotg *hsotg)
{
	enum usb_dr_mode mode;

	hsotg->dr_mode = usb_get_dr_mode(hsotg->dev);
	if (hsotg->dr_mode == USB_DR_MODE_UNKNOWN)
		hsotg->dr_mode = USB_DR_MODE_OTG;

	mode = hsotg->dr_mode;

	if (dwc2_hw_is_device(hsotg)) {
		if (IS_ENABLED(CONFIG_USB_DWC2_HOST)) {
			dev_err(hsotg->dev,
				"Controller does not support host mode.\n");
			return -EINVAL;
		}
		mode = USB_DR_MODE_PERIPHERAL;
	} else if (dwc2_hw_is_host(hsotg)) {
		if (IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL)) {
			dev_err(hsotg->dev,
				"Controller does not support device mode.\n");
			return -EINVAL;
		}
		mode = USB_DR_MODE_HOST;
	} else {
		if (IS_ENABLED(CONFIG_USB_DWC2_HOST))
			mode = USB_DR_MODE_HOST;
		else if (IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL))
			mode = USB_DR_MODE_PERIPHERAL;
	}

	if (mode != hsotg->dr_mode) {
		dev_warn(hsotg->dev,
			 "Configuration mismatch. dr_mode forced to %s\n",
			mode == USB_DR_MODE_HOST ? "host" : "device");

		hsotg->dr_mode = mode;
	}

	return 0;
}

static void __dwc2_disable_regulators(void *data)
{
	struct dwc2_hsotg *hsotg = data;

	regulator_bulk_disable(ARRAY_SIZE(hsotg->supplies), hsotg->supplies);
}

#if IS_ENABLED(CONFIG_ARCH_CVITEK)
static void dwc2_set_hw_id(struct dwc2_hsotg *hsotg, int is_dev)
{
	if (is_dev) {
		/* device */
		iowrite32((ioread32((void *)hsotg->cviusb.usb_pin_regs) & ~0x0000C0) | 0xC0,
				(void *)hsotg->cviusb.usb_pin_regs);
	} else {
		/* host */
		iowrite32((ioread32((void *)hsotg->cviusb.usb_pin_regs) & ~0x0000C0) | 0x40,
				(void *)hsotg->cviusb.usb_pin_regs);
	}
}
#endif

static int __dwc2_lowlevel_hw_enable(struct dwc2_hsotg *hsotg)
{
	struct platform_device *pdev = to_platform_device(hsotg->dev);
	int ret;
#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	struct cviusb_dev *cviusb = &hsotg->cviusb;
#endif

	ret = regulator_bulk_enable(ARRAY_SIZE(hsotg->supplies),
				    hsotg->supplies);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(&pdev->dev,
				       __dwc2_disable_regulators, hsotg);
	if (ret)
		return ret;

	if (hsotg->clk) {
		ret = clk_prepare_enable(hsotg->clk);
		if (ret)
			return ret;
	}

	/* Enable the clock here for init/resume process. */
#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	if (!IS_ERR(cviusb->clk_axi.clk_o)) {
		clk_prepare_enable(cviusb->clk_axi.clk_o);
		cviusb->clk_axi.is_on = 1;
		dev_info(hsotg->dev, "axi clk installed\n");
	}
	if (!IS_ERR(cviusb->clk_apb.clk_o)) {
		clk_prepare_enable(cviusb->clk_apb.clk_o);
		cviusb->clk_apb.is_on = 1;
		dev_info(hsotg->dev, "apb clk installed\n");
	}
	if (!IS_ERR(cviusb->clk_125m.clk_o)) {
		clk_prepare_enable(cviusb->clk_125m.clk_o);
		cviusb->clk_125m.is_on = 1;
		dev_info(hsotg->dev, "125m clk installed\n");
	}
	if (!IS_ERR(cviusb->clk_33k.clk_o)) {
		clk_prepare_enable(cviusb->clk_33k.clk_o);
		cviusb->clk_33k.is_on = 1;
		dev_info(hsotg->dev, "33k clk installed\n");
	}
	if (!IS_ERR(cviusb->clk_12m.clk_o)) {
		clk_prepare_enable(cviusb->clk_12m.clk_o);
		cviusb->clk_12m.is_on = 1;
		dev_info(hsotg->dev, "12m clk installed\n");
	}

	dwc2_set_hw_id(hsotg, hsotg->cviusb.id_override);
#endif

	if (hsotg->uphy) {
		ret = usb_phy_init(hsotg->uphy);
	} else if (hsotg->plat && hsotg->plat->phy_init) {
		ret = hsotg->plat->phy_init(pdev, hsotg->plat->phy_type);
	} else {
		ret = phy_power_on(hsotg->phy);
		if (ret == 0)
			ret = phy_init(hsotg->phy);
	}

	return ret;
}

/**
 * dwc2_lowlevel_hw_enable - enable platform lowlevel hw resources
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB platform resources (phy, clock, regulators)
 */
int dwc2_lowlevel_hw_enable(struct dwc2_hsotg *hsotg)
{
	int ret = __dwc2_lowlevel_hw_enable(hsotg);

	if (ret == 0)
		hsotg->ll_hw_enabled = true;
	return ret;
}

static int __dwc2_lowlevel_hw_disable(struct dwc2_hsotg *hsotg)
{
	struct platform_device *pdev = to_platform_device(hsotg->dev);
	int ret = 0;

	if (hsotg->uphy) {
		usb_phy_shutdown(hsotg->uphy);
	} else if (hsotg->plat && hsotg->plat->phy_exit) {
		ret = hsotg->plat->phy_exit(pdev, hsotg->plat->phy_type);
	} else {
		ret = phy_exit(hsotg->phy);
		if (ret == 0)
			ret = phy_power_off(hsotg->phy);
	}
	if (ret)
		return ret;

	if (hsotg->clk)
		clk_disable_unprepare(hsotg->clk);

	return 0;
}

/**
 * dwc2_lowlevel_hw_disable - disable platform lowlevel hw resources
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB platform resources (phy, clock, regulators)
 */
int dwc2_lowlevel_hw_disable(struct dwc2_hsotg *hsotg)
{
	int ret = __dwc2_lowlevel_hw_disable(hsotg);

	if (ret == 0)
		hsotg->ll_hw_enabled = false;
	return ret;
}

static int dwc2_lowlevel_hw_init(struct dwc2_hsotg *hsotg)
{
	int i, ret;

	hsotg->reset = devm_reset_control_get_optional(hsotg->dev, "dwc2");
	if (IS_ERR(hsotg->reset)) {
		ret = PTR_ERR(hsotg->reset);
		dev_err(hsotg->dev, "error getting reset control %d\n", ret);
		return ret;
	}

	reset_control_deassert(hsotg->reset);

	hsotg->reset_ecc = devm_reset_control_get_optional(hsotg->dev, "dwc2-ecc");
	if (IS_ERR(hsotg->reset_ecc)) {
		ret = PTR_ERR(hsotg->reset_ecc);
		dev_err(hsotg->dev, "error getting reset control for ecc %d\n", ret);
		return ret;
	}

	reset_control_deassert(hsotg->reset_ecc);

	/*
	 * Attempt to find a generic PHY, then look for an old style
	 * USB PHY and then fall back to pdata
	 */
	hsotg->phy = devm_phy_get(hsotg->dev, "usb2-phy");
	if (IS_ERR(hsotg->phy)) {
		ret = PTR_ERR(hsotg->phy);
		switch (ret) {
		case -ENODEV:
		case -ENOSYS:
			hsotg->phy = NULL;
			break;
		case -EPROBE_DEFER:
			return ret;
		default:
			dev_err(hsotg->dev, "error getting phy %d\n", ret);
			return ret;
		}
	}

	if (!hsotg->phy) {
		hsotg->uphy = devm_usb_get_phy(hsotg->dev, USB_PHY_TYPE_USB2);
		if (IS_ERR(hsotg->uphy)) {
			ret = PTR_ERR(hsotg->uphy);
			switch (ret) {
			case -ENODEV:
			case -ENXIO:
				hsotg->uphy = NULL;
				break;
			case -EPROBE_DEFER:
				return ret;
			default:
				dev_err(hsotg->dev, "error getting usb phy %d\n",
					ret);
				return ret;
			}
		}
	}

	hsotg->plat = dev_get_platdata(hsotg->dev);

#if !IS_ENABLED(CONFIG_ARCH_CVITEK)
	/* Clock */
	hsotg->clk = devm_clk_get_optional(hsotg->dev, "otg");
	if (IS_ERR(hsotg->clk)) {
		dev_err(hsotg->dev, "cannot get otg clock\n");
		return PTR_ERR(hsotg->clk);
	}
#endif
	/* Regulators */
	for (i = 0; i < ARRAY_SIZE(hsotg->supplies); i++)
		hsotg->supplies[i].supply = dwc2_hsotg_supply_names[i];

	ret = devm_regulator_bulk_get(hsotg->dev, ARRAY_SIZE(hsotg->supplies),
				      hsotg->supplies);
	if (ret) {
		if (ret != -EPROBE_DEFER)
			dev_err(hsotg->dev, "failed to request supplies: %d\n",
				ret);
		return ret;
	}
	return 0;
}

#if IS_ENABLED(CONFIG_ARCH_CVITEK)

#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)

#define PHY_REG(x)	(x)

#define REG014				PHY_REG(0x014)
#define REG014_UTMI_RESET		(1 << 8)
#define REG014_DMPULLDOWN		(1 << 7)
#define REG014_DPPULLDOWN		(1 << 6)
#define REG014_TERMSEL			(1 << 5)
#define REG014_XCVRSEL_MASK		(0x3 << 3)
#define REG014_XCVRSEL_SHIFT		3
#define REG014_OPMODE_MASK		(0x3 << 1)
#define REG014_OPMODE_SHIFT		1
#define REG014_UTMI_OVERRIDE		(1 << 0)

#define REG020				PHY_REG(0x020)
#define REG020_DP_DET			(1 << 17)
#define REG020_CHG_DET			(1 << 16)
#define REG020_VDM_SRC_EN		(1 << 5)
#define REG020_VDP_SRC_EN		(1 << 4)
#define REG020_DM_CMP_EN		(1 << 3)
#define REG020_DP_CMP_EN		(1 << 2)
#define REG020_DCD_EN			(1 << 1)
#define REG020_BC_EN			(1 << 0)

#ifdef CONFIG_PROC_FS

#define CVIUSB_ROLE_PROC_NAME "cviusb/otg_role"
#define CVIUSB_CHGDET_PROC_NAME "cviusb/chg_det"

static struct proc_dir_entry *cviusb_proc_dir;
static struct proc_dir_entry *cviusb_role_proc_entry;
static struct proc_dir_entry *cviusb_chgdet_proc_entry;

static u8 *sel_role[] = {
	"host",
	"device",
};

static int proc_role_show(struct seq_file *m, void *v)
{
	struct dwc2_hsotg *hsotg = (struct dwc2_hsotg *)m->private;

	seq_printf(m, "%s\n", sel_role[hsotg->cviusb.id_override]);

	return 0;
}

static int sel_role_hdler(struct dwc2_hsotg *hsotg, char const *input)
{
	u32 num;
	u8 str[80] = {0};
	u8 t = 0;
	u8 i, n;
	u8 *p;

	num = sscanf(input, "%s", str);
	if (num > 1) {
		return -EINVAL;
	}

	/* convert to lower case for following type compare */
	p = str;
	for (; *p; ++p)
		*p = tolower(*p);
	n = ARRAY_SIZE(sel_role);
	for (i = 0; i < n; i++) {
		if (!strcmp(str, sel_role[i])) {
			t = i;
			break;
		}
	}
	if (i == n)
		return -EINVAL;

	hsotg->cviusb.id_override = t;
	dwc2_set_hw_id(hsotg, t);

	return 0;
}

static ssize_t role_proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	char procdata[32] = {'\0'};
	struct dwc2_hsotg *hsotg = PDE_DATA(file_inode(file));

	if (user_buf == NULL || count >= sizeof(procdata)) {
		dev_err(hsotg->dev, "Invalid input value\n");
		return -EINVAL;
	}

	if (copy_from_user(procdata, user_buf, count)) {
		dev_err(hsotg->dev, "copy_from_user fail\n");
		return -EFAULT;
	}

	sel_role_hdler(hsotg, procdata);

	return count;
}

static int proc_role_open(struct inode *inode, struct file *file)
{
	struct dwc2_hsotg *hsotg = PDE_DATA(inode);

	return single_open(file, proc_role_show, hsotg);
}

static const struct proc_ops role_proc_ops = {
	.proc_open		= proc_role_open,
	.proc_read		= seq_read,
	.proc_write		= role_proc_write,
	.proc_lseek		= seq_lseek,
	.proc_release	= single_release,
};

#define TDCD_TIMEOUT_MAX	900	//ms
#define TDCD_TIMEOUT_MIN	300	//ms
#define TDCD_DBNC		10	//ms
#define TVDMSRC_EN		20	//ms
#define TVDPSRC_ON		40	//ms
#define TVDMSRC_ON		40	//ms

static u8 *dcd_en[] = {
	"dcd_off",
	"dcd_on",
};

static u8 *chg_port[CHGDET_NUM] = {
	"sdp",
	"dcp",
	"cdp",
};

static void utmi_chgdet_prepare(struct dwc2_hsotg *hsotg)
{
	struct cviusb_dev *cviusb = &hsotg->cviusb;

	cviusb_writel(REG014_UTMI_OVERRIDE |
			(REG014_OPMODE_MASK & (0x1 << REG014_OPMODE_SHIFT)) |
			(REG014_XCVRSEL_MASK & (0x1 << REG014_XCVRSEL_SHIFT)),
			cviusb->phy_regs + REG014);
}

static void utmi_reset(struct dwc2_hsotg *hsotg)
{
	struct cviusb_dev *cviusb = &hsotg->cviusb;

	cviusb_writel(0, cviusb->phy_regs + REG014);
}

static void dcd_det(struct dwc2_hsotg *hsotg)
{
	struct cviusb_dev *cviusb = &hsotg->cviusb;
	int cnt = 0;
	u32 dbnc = 0;

	/* 1. utmi prepare */
	utmi_chgdet_prepare(hsotg);
	/* 2. Enable bc and dcd*/
	cviusb_writel(REG020_BC_EN | REG020_DCD_EN, cviusb->phy_regs + REG020);
	/* 3. DCD det in 900ms */
	while (cnt++ < TDCD_TIMEOUT_MAX) {
		if (!(dwc2_readl(hsotg, DSTS) & BIT(22)))
			dbnc += 1;
		else
			dbnc = 0;
		if (dbnc >= TDCD_DBNC)
			break;
		usleep_range(1000, 1010);
	}
	/* 4. Disable bc dcd. */
	cviusb_writel(0, cviusb->phy_regs + REG020);
	/* 5. utmi reset */
	utmi_reset(hsotg);
	usleep_range(1000, 1010);
}

static int chg_det(struct dwc2_hsotg *hsotg)
{
	struct cviusb_dev *cviusb = &hsotg->cviusb;
	int cnt = 0;
	int det = 0;
	u32 reg;

	/* 1. Enable bc */
	cviusb_writel(REG020_BC_EN | REG020_VDP_SRC_EN | REG020_DM_CMP_EN,
			cviusb->phy_regs + REG020);
	/* need 2ms delay to avoid the unstable value on DM CMP. */
	usleep_range(2000, 2010);
	/* 2. Dm det in 40ms */
	while (cnt++ < TVDPSRC_ON) {
		reg = cviusb_readl(cviusb->phy_regs + REG020);
		if (reg & REG020_CHG_DET)
			det = 1;
		if (!det && cnt > TVDMSRC_EN)
			break;
		usleep_range(1000, 1010);
	}
	/* 3. Disable bc. */
	cviusb_writel(0, cviusb->phy_regs + REG020);

	return det;
}

static int cdp_det(struct dwc2_hsotg *hsotg)
{
	struct cviusb_dev *cviusb = &hsotg->cviusb;
	int cnt = 0;
	int det = 0;

	/* 1. Enable bc */
	cviusb_writel(REG020_BC_EN | REG020_VDM_SRC_EN | REG020_DP_CMP_EN,
			cviusb->phy_regs + REG020);
	usleep_range(1000, 1010);
	/* 2. Dp det in 40ms */
	while (cnt++ < TVDMSRC_ON) {
		if ((cviusb_readl(cviusb->phy_regs + REG020) & REG020_DP_DET))
			det = 1;
		/* 5ms for 2nd detection. */
		if (!det && cnt > 5)
			break;
		usleep_range(1000, 1010);
	}
	/* 3. Disable bc. */
	cviusb_writel(0, cviusb->phy_regs + REG020);

	return !det;
}

static int proc_chgdet_show(struct seq_file *m, void *v)
{
	struct dwc2_hsotg *hsotg = (struct dwc2_hsotg *)m->private;
	struct cviusb_dev *cviusb = &hsotg->cviusb;
	u32 reg;

	if (!hsotg->cviusb.id_override)
		return -EPERM;

	/* Disconnect the data line. */
	reg = dwc2_readl(hsotg, DCTL) | DCTL_SFTDISCON;
	dwc2_writel(hsotg, reg, DCTL);

	/* Run dcd detection or wait TDCD_TIMEOUT_MIN. */
	if (cviusb->dcd_dis)
		msleep(TDCD_TIMEOUT_MIN);
	else
		dcd_det(hsotg);
	/* Run chgdet */
	if (chg_det(hsotg)) {
		usleep_range(1000, 1010);
		if (cdp_det(hsotg))
			cviusb->chgdet = CHGDET_CDP;
		else
			cviusb->chgdet = CHGDET_DCP;
	} else
		cviusb->chgdet = CHGDET_SDP;

	/* Run dcpdet */
	seq_printf(m, "%s\n", chg_port[hsotg->cviusb.chgdet]);

	return 0;
}

static int dcd_en_hdler(struct dwc2_hsotg *hsotg, char const *input)
{
	u32 num;
	u8 str[80] = {0};
	u8 t = 0;
	u8 i, n;
	u8 *p;

	num = sscanf(input, "%s", str);
	if (num > 1) {
		return -EINVAL;
	}

	/* convert to lower case for following type compare */
	p = str;
	for (; *p; ++p)
		*p = tolower(*p);
	n = ARRAY_SIZE(dcd_en);
	for (i = 0; i < n; i++) {
		if (!strcmp(str, dcd_en[i])) {
			t = i;
			break;
		}
	}
	if (i == n)
		return -EINVAL;

	switch (t) {
	case 0:
		/* dcd off */
		hsotg->cviusb.dcd_dis = 1;
		break;
	case 1:
		/* dcd on */
		hsotg->cviusb.dcd_dis = 0;
		break;
	default:
		break;
	}

	return 0;
}

static ssize_t chgdet_proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	char procdata[32] = {'\0'};
	struct dwc2_hsotg *hsotg = PDE_DATA(file_inode(file));

	if (user_buf == NULL || count >= sizeof(procdata)) {
		dev_err(hsotg->dev, "Invalid input value\n");
		return -EINVAL;
	}

	if (copy_from_user(procdata, user_buf, count)) {
		dev_err(hsotg->dev, "copy_from_user fail\n");
		return -EFAULT;
	}

	dcd_en_hdler(hsotg, procdata);

	return count;
}

static int proc_chgdet_open(struct inode *inode, struct file *file)
{
	struct dwc2_hsotg *hsotg = PDE_DATA(inode);

	return single_open(file, proc_chgdet_show, hsotg);
}

static const struct proc_ops chgdet_proc_ops = {
	.proc_open		= proc_chgdet_open,
	.proc_read		= seq_read,
	.proc_write		= chgdet_proc_write,
	.proc_lseek		= seq_lseek,
	.proc_release	= single_release,
};

#endif

static int vbus_is_present(struct cviusb_dev *cviusb)
{
	if (gpio_is_valid(cviusb->vbus_pin))
		return gpio_get_value(cviusb->vbus_pin) ^
			cviusb->vbus_pin_inverted;

	/* No Vbus detection: Assume always present */
	return 1;
}

static irqreturn_t vbus_irq_handler(int irq, void *devid)
{
	struct cviusb_dev *cviusb = devid;
	struct dwc2_hsotg *hsotg = container_of(cviusb, struct dwc2_hsotg, cviusb);
	int vbus;

	/* do nothing if we are an A-device (vbus provider). */
	if (hsotg->op_state != OTG_STATE_B_PERIPHERAL)
		return IRQ_HANDLED;
	vbus = vbus_is_present(cviusb);
	dev_dbg(hsotg->dev, "vbus int = %d\n", vbus);
	return IRQ_WAKE_THREAD;
}

static irqreturn_t vbus_irq_thread(int irq, void *devid)
{
	struct cviusb_dev *cviusb = devid;
	struct dwc2_hsotg *hsotg = container_of(cviusb, struct dwc2_hsotg, cviusb);
	struct usb_gadget *gadget = &hsotg->gadget;
	int vbus;

	if (!gadget->udc)
		return IRQ_HANDLED;

	/* debounce */
	udelay(10);
	vbus = vbus_is_present(cviusb);
	if (cviusb->pre_vbus_status != vbus) {
		dev_dbg(hsotg->dev, "vbus thread = %d\n", vbus);
		usb_udc_vbus_handler(gadget, (vbus != 0));
		cviusb->pre_vbus_status = vbus;
	}
	return IRQ_HANDLED;
}
#endif	/* CONFIG_USB_DWC2_PERIPHERAL || CONFIG_USB_DWC2_DUAL_ROLE */

#endif	/* CONFIG_ARCH_CVITEK */

/**
 * dwc2_driver_remove() - Called when the DWC_otg core is unregistered with the
 * DWC_otg driver
 *
 * @dev: Platform device
 *
 * This routine is called, for example, when the rmmod command is executed. The
 * device may or may not be electrically present. If it is present, the driver
 * stops device processing. Any resources used on behalf of this device are
 * freed.
 */
static int dwc2_driver_remove(struct platform_device *dev)
{
	struct dwc2_hsotg *hsotg = platform_get_drvdata(dev);
	struct dwc2_gregs_backup *gr;
	int ret = 0;
#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	struct cviusb_dev *cviusb = &hsotg->cviusb;

	devm_free_irq(&dev->dev,
		      gpio_to_irq(hsotg->cviusb.vbus_pin),
		      (void *)&hsotg->cviusb);
#endif

	gr = &hsotg->gr_backup;

	/* Exit Hibernation when driver is removed. */
	if (hsotg->hibernated) {
		if (gr->gotgctl & GOTGCTL_CURMODE_HOST)
			ret = dwc2_exit_hibernation(hsotg, 0, 0, 1);
		else
			ret = dwc2_exit_hibernation(hsotg, 0, 0, 0);

		if (ret)
			dev_err(hsotg->dev,
				"exit hibernation failed.\n");
	}

	/* Exit Partial Power Down when driver is removed. */
	if (hsotg->in_ppd) {
		ret = dwc2_exit_partial_power_down(hsotg, 0, true);
		if (ret)
			dev_err(hsotg->dev,
				"exit partial_power_down failed\n");
	}

	/* Exit clock gating when driver is removed. */
	if (hsotg->params.power_down == DWC2_POWER_DOWN_PARAM_NONE &&
	    hsotg->bus_suspended) {
		if (dwc2_is_device_mode(hsotg))
			dwc2_gadget_exit_clock_gating(hsotg, 0);
		else
			dwc2_host_exit_clock_gating(hsotg, 0);
	}

	dwc2_debugfs_exit(hsotg);
	if (hsotg->hcd_enabled)
		dwc2_hcd_remove(hsotg);
	if (hsotg->gadget_enabled)
		dwc2_hsotg_remove(hsotg);

#if IS_ENABLED(CONFIG_USB_ROLE_SWITCH)
	dwc2_drd_exit(hsotg);
#endif

	if (hsotg->params.activate_stm_id_vb_detection)
		regulator_disable(hsotg->usb33d);

	if (hsotg->ll_hw_enabled)
		dwc2_lowlevel_hw_disable(hsotg);

	reset_control_assert(hsotg->reset);
	reset_control_assert(hsotg->reset_ecc);

	/* Disable the clock here for remove process. */
#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	if (cviusb->clk_axi.clk_o) {
		clk_disable_unprepare(cviusb->clk_axi.clk_o);
		dev_info(hsotg->dev, "axi clk disable\n");
		cviusb->clk_axi.is_on = 0;
	}
	if (cviusb->clk_apb.clk_o) {
		clk_disable_unprepare(cviusb->clk_apb.clk_o);
		dev_info(hsotg->dev, "apb clk disable\n");
		cviusb->clk_apb.is_on = 0;
	}
	if (cviusb->clk_125m.clk_o) {
		clk_disable_unprepare(cviusb->clk_125m.clk_o);
		dev_info(hsotg->dev, "125m clk disable\n");
		cviusb->clk_125m.is_on = 0;
	}
	if (cviusb->clk_33k.clk_o) {
		clk_disable_unprepare(cviusb->clk_33k.clk_o);
		dev_info(hsotg->dev, "33k clk disable\n");
		cviusb->clk_33k.is_on = 0;
	}
	if (cviusb->clk_12m.clk_o) {
		clk_disable_unprepare(cviusb->clk_12m.clk_o);
		dev_info(hsotg->dev, "12m clk disable\n");
		cviusb->clk_12m.is_on = 0;
	}

#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)

#ifdef CONFIG_PROC_FS
	proc_remove(cviusb_chgdet_proc_entry);
	proc_remove(cviusb_role_proc_entry);
	proc_remove(cviusb_proc_dir);
	cviusb_proc_dir = NULL;
	cviusb_role_proc_entry = NULL;
#endif	/* CONFIG_PROC_FS */

#endif	/* CONFIG_USB_DWC2_PERIPHERAL || CONFIG_USB_DWC2_DUAL_ROLE */

#endif

	return ret;
}

/**
 * dwc2_driver_shutdown() - Called on device shutdown
 *
 * @dev: Platform device
 *
 * In specific conditions (involving usb hubs) dwc2 devices can create a
 * lot of interrupts, even to the point of overwhelming devices running
 * at low frequencies. Some devices need to do special clock handling
 * at shutdown-time which may bring the system clock below the threshold
 * of being able to handle the dwc2 interrupts. Disabling dwc2-irqs
 * prevents reboots/poweroffs from getting stuck in such cases.
 */
static void dwc2_driver_shutdown(struct platform_device *dev)
{
	struct dwc2_hsotg *hsotg = platform_get_drvdata(dev);

	dwc2_disable_global_interrupts(hsotg);
	synchronize_irq(hsotg->irq);
}

/**
 * dwc2_check_core_endianness() - Returns true if core and AHB have
 * opposite endianness.
 * @hsotg:	Programming view of the DWC_otg controller.
 */
static bool dwc2_check_core_endianness(struct dwc2_hsotg *hsotg)
{
	u32 snpsid;

	snpsid = ioread32(hsotg->regs + GSNPSID);
	if ((snpsid & GSNPSID_ID_MASK) == DWC2_OTG_ID ||
	    (snpsid & GSNPSID_ID_MASK) == DWC2_FS_IOT_ID ||
	    (snpsid & GSNPSID_ID_MASK) == DWC2_HS_IOT_ID)
		return false;
	return true;
}

/**
 * Check core version
 *
 * @hsotg: Programming view of the DWC_otg controller
 *
 */
int dwc2_check_core_version(struct dwc2_hsotg *hsotg)
{
	struct dwc2_hw_params *hw = &hsotg->hw_params;

	/*
	 * Attempt to ensure this device is really a DWC_otg Controller.
	 * Read and verify the GSNPSID register contents. The value should be
	 * 0x45f4xxxx, 0x5531xxxx or 0x5532xxxx
	 */

	hw->snpsid = dwc2_readl(hsotg, GSNPSID);
	if ((hw->snpsid & GSNPSID_ID_MASK) != DWC2_OTG_ID &&
	    (hw->snpsid & GSNPSID_ID_MASK) != DWC2_FS_IOT_ID &&
	    (hw->snpsid & GSNPSID_ID_MASK) != DWC2_HS_IOT_ID) {
		dev_err(hsotg->dev, "Bad value for GSNPSID: 0x%08x\n",
			hw->snpsid);
		return -ENODEV;
	}

	dev_dbg(hsotg->dev, "Core Release: %1x.%1x%1x%1x (snpsid=%x)\n",
		hw->snpsid >> 12 & 0xf, hw->snpsid >> 8 & 0xf,
		hw->snpsid >> 4 & 0xf, hw->snpsid & 0xf, hw->snpsid);
	return 0;
}

/**
 * dwc2_driver_probe() - Called when the DWC_otg core is bound to the DWC_otg
 * driver
 *
 * @dev: Platform device
 *
 * This routine creates the driver components required to control the device
 * (core, HCD, and PCD) and initializes the device. The driver components are
 * stored in a dwc2_hsotg structure. A reference to the dwc2_hsotg is saved
 * in the device private data. This allows the driver to access the dwc2_hsotg
 * structure on subsequent calls to driver methods for this device.
 */
static int dwc2_driver_probe(struct platform_device *dev)
{
	struct dwc2_hsotg *hsotg;
	struct resource *res;
	int retval;
#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	struct cviusb_dev *cviusb;
	enum of_gpio_flags	flags;
#endif

	hsotg = devm_kzalloc(&dev->dev, sizeof(*hsotg), GFP_KERNEL);
	if (!hsotg)
		return -ENOMEM;

	hsotg->dev = &dev->dev;

	/*
	 * Use reasonable defaults so platforms don't have to provide these.
	 */
	if (!dev->dev.dma_mask)
		dev->dev.dma_mask = &dev->dev.coherent_dma_mask;
	retval = dma_set_coherent_mask(&dev->dev, DMA_BIT_MASK(32));
	if (retval) {
		dev_err(&dev->dev, "can't set coherent DMA mask: %d\n", retval);
		return retval;
	}

	hsotg->regs = devm_platform_get_and_ioremap_resource(dev, 0, &res);
	if (IS_ERR(hsotg->regs))
		return PTR_ERR(hsotg->regs);

	dev_dbg(&dev->dev, "mapped PA %08lx to VA %p\n",
		(unsigned long)res->start, hsotg->regs);
#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	cviusb = &hsotg->cviusb;
	cviusb->usb_pin_regs = ioremap(0x03000048, 0x4);
	/* init as host mode */
	hsotg->cviusb.id_override = 0;
	res = platform_get_resource(dev, IORESOURCE_MEM, 1);
	cviusb->phy_regs = devm_ioremap_resource(&dev->dev, res);
	if (IS_ERR(cviusb->phy_regs))
		return PTR_ERR(cviusb->phy_regs);

	dev_dbg(&dev->dev, "mapped PA %08lx to VA %p\n",
		(unsigned long)res->start, cviusb->phy_regs);

	cviusb->clk_axi.clk_o = devm_clk_get(&dev->dev, "clk_axi");
	if (IS_ERR(cviusb->clk_axi.clk_o)) {
		dev_warn(&dev->dev, "Clock axi not found\n");
	}
	cviusb->clk_apb.clk_o = devm_clk_get(&dev->dev, "clk_apb");
	if (IS_ERR(cviusb->clk_apb.clk_o)) {
		dev_warn(&dev->dev, "Clock apb not found\n");
	}
	cviusb->clk_125m.clk_o = devm_clk_get(&dev->dev, "clk_125m");
	if (IS_ERR(cviusb->clk_125m.clk_o)) {
		dev_warn(&dev->dev, "Clock 125m not found\n");
	}
	cviusb->clk_33k.clk_o = devm_clk_get(&dev->dev, "clk_33k");
	if (IS_ERR(cviusb->clk_33k.clk_o)) {
		dev_warn(&dev->dev, "Clock 33k not found\n");
	}
	cviusb->clk_12m.clk_o = devm_clk_get(&dev->dev, "clk_12m");
	if (IS_ERR(cviusb->clk_12m.clk_o)) {
		dev_warn(&dev->dev, "Clock 12m not found\n");
	}
#endif
	retval = dwc2_lowlevel_hw_init(hsotg);
	if (retval)
		return retval;

	spin_lock_init(&hsotg->lock);

	hsotg->irq = platform_get_irq(dev, 0);
	if (hsotg->irq < 0)
		return hsotg->irq;

	dev_dbg(hsotg->dev, "registering common handler for irq%d\n",
		hsotg->irq);
	retval = devm_request_irq(hsotg->dev, hsotg->irq,
				  dwc2_handle_common_intr, IRQF_SHARED,
				  dev_name(hsotg->dev), hsotg);
	if (retval)
		return retval;

	hsotg->vbus_supply = devm_regulator_get_optional(hsotg->dev, "vbus");
	if (IS_ERR(hsotg->vbus_supply)) {
		retval = PTR_ERR(hsotg->vbus_supply);
		hsotg->vbus_supply = NULL;
		if (retval != -ENODEV)
			return retval;
	}

	retval = dwc2_lowlevel_hw_enable(hsotg);
	if (retval)
		return retval;

#if IS_ENABLED(CONFIG_ARCH_CVITEK)
	/* Not to eanble the GINtMSK till all isr are ready. */
	dwc2_writel(hsotg, 0, GINTMSK);
#endif
	hsotg->needs_byte_swap = dwc2_check_core_endianness(hsotg);

	retval = dwc2_get_dr_mode(hsotg);
	if (retval)
		goto error;

	hsotg->need_phy_for_wake =
		of_property_read_bool(dev->dev.of_node,
				      "snps,need-phy-for-wake");

	/*
	 * Before performing any core related operations
	 * check core version.
	 */
	retval = dwc2_check_core_version(hsotg);
	if (retval)
		goto error;

	/*
	 * Reset before dwc2_get_hwparams() then it could get power-on real
	 * reset value form registers.
	 */
	retval = dwc2_core_reset(hsotg, false);
	if (retval)
		goto error;

	/* Detect config values from hardware */
	retval = dwc2_get_hwparams(hsotg);
	if (retval)
		goto error;

	/*
	 * For OTG cores, set the force mode bits to reflect the value
	 * of dr_mode. Force mode bits should not be touched at any
	 * other time after this.
	 */
	dwc2_force_dr_mode(hsotg);

	retval = dwc2_init_params(hsotg);
	if (retval)
		goto error;

	if (hsotg->params.activate_stm_id_vb_detection) {
		u32 ggpio;

		hsotg->usb33d = devm_regulator_get(hsotg->dev, "usb33d");
		if (IS_ERR(hsotg->usb33d)) {
			retval = PTR_ERR(hsotg->usb33d);
			if (retval != -EPROBE_DEFER)
				dev_err(hsotg->dev,
					"failed to request usb33d supply: %d\n",
					retval);
			goto error;
		}
		retval = regulator_enable(hsotg->usb33d);
		if (retval) {
			dev_err(hsotg->dev,
				"failed to enable usb33d supply: %d\n", retval);
			goto error;
		}

		ggpio = dwc2_readl(hsotg, GGPIO);
		ggpio |= GGPIO_STM32_OTG_GCCFG_IDEN;
		ggpio |= GGPIO_STM32_OTG_GCCFG_VBDEN;
		dwc2_writel(hsotg, ggpio, GGPIO);
	}

#if IS_ENABLED(CONFIG_USB_ROLE_SWITCH)
	retval = dwc2_drd_init(hsotg);
	if (retval) {
		if (retval != -EPROBE_DEFER)
			dev_err(hsotg->dev, "failed to initialize dual-role\n");
		goto error_init;
	}
#endif

	if (hsotg->dr_mode != USB_DR_MODE_HOST) {
		retval = dwc2_gadget_init(hsotg);
		if (retval)
			goto error_drd;
		hsotg->gadget_enabled = 1;
	}

	/*
	 * If we need PHY for wakeup we must be wakeup capable.
	 * When we have a device that can wake without the PHY we
	 * can adjust this condition.
	 */
	if (hsotg->need_phy_for_wake)
		device_set_wakeup_capable(&dev->dev, true);

	hsotg->reset_phy_on_wake =
		of_property_read_bool(dev->dev.of_node,
				      "snps,reset-phy-on-wake");
	if (hsotg->reset_phy_on_wake && !hsotg->phy) {
		dev_warn(hsotg->dev,
			 "Quirk reset-phy-on-wake only supports generic PHYs\n");
		hsotg->reset_phy_on_wake = false;
	}

	if (hsotg->dr_mode != USB_DR_MODE_PERIPHERAL) {
		retval = dwc2_hcd_init(hsotg);
		if (retval) {
			if (hsotg->gadget_enabled)
				dwc2_hsotg_remove(hsotg);
			goto error_drd;
		}
		hsotg->hcd_enabled = 1;
	}

	platform_set_drvdata(dev, hsotg);
	hsotg->hibernated = 0;

	dwc2_debugfs_init(hsotg);

	/* Gadget code manages lowlevel hw on its own */
	if (hsotg->dr_mode == USB_DR_MODE_PERIPHERAL)
		dwc2_lowlevel_hw_disable(hsotg);

#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)
	/* Postponed adding a new gadget to the udc class driver list */
	if (hsotg->gadget_enabled) {
		retval = usb_add_gadget_udc(hsotg->dev, &hsotg->gadget);
		if (retval) {
			hsotg->gadget.udc = NULL;
			dwc2_hsotg_remove(hsotg);
			goto error_debugfs;
		}
	}
#endif /* CONFIG_USB_DWC2_PERIPHERAL || CONFIG_USB_DWC2_DUAL_ROLE */

#if IS_ENABLED(CONFIG_ARCH_CVITEK)

#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)

	hsotg->cviusb.vbus_pin = of_get_named_gpio_flags(dev->dev.of_node,
				"vbus-gpio", 0, &flags);
	hsotg->cviusb.vbus_pin_inverted = (flags & OF_GPIO_ACTIVE_LOW) ? 1 : 0;
	dev_dbg(hsotg->dev, "vbus_pin = %d, flags = %d\n",
			hsotg->cviusb.vbus_pin, hsotg->cviusb.vbus_pin_inverted);
	if (gpio_is_valid(hsotg->cviusb.vbus_pin)) {
		if (!devm_gpio_request(&dev->dev,
			hsotg->cviusb.vbus_pin, "cviusb-otg")) {
			irq_set_status_flags(gpio_to_irq(hsotg->cviusb.vbus_pin),
					IRQ_NOAUTOEN);
			retval = devm_request_threaded_irq(&dev->dev,
					gpio_to_irq(hsotg->cviusb.vbus_pin),
					vbus_irq_handler,
					vbus_irq_thread,
					IRQF_TRIGGER_RISING |
					IRQF_TRIGGER_FALLING,
					"cviusb-otg", (void *)&hsotg->cviusb);
			if (retval) {
				hsotg->cviusb.vbus_pin = -ENODEV;
				dev_err(hsotg->dev,
					"failed to request vbus irq\n");
			} else {
				hsotg->cviusb.pre_vbus_status = vbus_is_present(&hsotg->cviusb);
				enable_irq(gpio_to_irq(hsotg->cviusb.vbus_pin));
				dev_dbg(hsotg->dev,
					"enable vbus irq, vbus status = %d\n",
					hsotg->cviusb.pre_vbus_status);
			}
		} else {
			/* gpio_request fail so use -EINVAL for gpio_is_valid */
			hsotg->cviusb.vbus_pin = -EINVAL;
			dev_err(hsotg->dev, "request gpio fail!\n");
		}
	}

#ifdef CONFIG_PROC_FS
	cviusb_proc_dir = proc_mkdir("cviusb", NULL);
	cviusb_role_proc_entry = proc_create_data(CVIUSB_ROLE_PROC_NAME, 0644, NULL,
					  &role_proc_ops, hsotg);
	if (!cviusb_role_proc_entry)
		dev_err(&dev->dev, "cviusb: can't role procfs.\n");
	cviusb_chgdet_proc_entry = proc_create_data(CVIUSB_CHGDET_PROC_NAME, 0644, NULL,
					  &chgdet_proc_ops, hsotg);
	if (!cviusb_chgdet_proc_entry)
		dev_err(&dev->dev, "cviusb: can't chgdet procfs.\n");
#endif	/* CONFIG_PROC_FS */

#endif	/* CONFIG_USB_DWC2_PERIPHERAL || CONFIG_USB_DWC2_DUAL_ROLE */

#endif	/* CONFIG_ARCH_CVITEK */
	return 0;

#if IS_ENABLED(CONFIG_USB_DWC2_PERIPHERAL) || \
	IS_ENABLED(CONFIG_USB_DWC2_DUAL_ROLE)
error_debugfs:
	dwc2_debugfs_exit(hsotg);
	if (hsotg->hcd_enabled)
		dwc2_hcd_remove(hsotg);
#endif

error_drd:
#if IS_ENABLED(CONFIG_USB_ROLE_SWITCH)
	dwc2_drd_exit(hsotg);
#endif

error_init:
	if (hsotg->params.activate_stm_id_vb_detection)
		regulator_disable(hsotg->usb33d);
error:
	if (hsotg->dr_mode != USB_DR_MODE_PERIPHERAL)
		dwc2_lowlevel_hw_disable(hsotg);
	return retval;
}

static int __maybe_unused dwc2_suspend(struct device *dev)
{
	struct dwc2_hsotg *dwc2 = dev_get_drvdata(dev);
	bool is_device_mode = dwc2_is_device_mode(dwc2);
	int ret = 0;

	if (is_device_mode)
		dwc2_hsotg_suspend(dwc2);

	dwc2_drd_suspend(dwc2);

	if (dwc2->params.activate_stm_id_vb_detection) {
		unsigned long flags;
		u32 ggpio, gotgctl;

		/*
		 * Need to force the mode to the current mode to avoid Mode
		 * Mismatch Interrupt when ID detection will be disabled.
		 */
		dwc2_force_mode(dwc2, !is_device_mode);

		spin_lock_irqsave(&dwc2->lock, flags);
		gotgctl = dwc2_readl(dwc2, GOTGCTL);
		/* bypass debounce filter, enable overrides */
		gotgctl |= GOTGCTL_DBNCE_FLTR_BYPASS;
		gotgctl |= GOTGCTL_BVALOEN | GOTGCTL_AVALOEN;
		/* Force A / B session if needed */
		if (gotgctl & GOTGCTL_ASESVLD)
			gotgctl |= GOTGCTL_AVALOVAL;
		if (gotgctl & GOTGCTL_BSESVLD)
			gotgctl |= GOTGCTL_BVALOVAL;
		dwc2_writel(dwc2, gotgctl, GOTGCTL);
		spin_unlock_irqrestore(&dwc2->lock, flags);

		ggpio = dwc2_readl(dwc2, GGPIO);
		ggpio &= ~GGPIO_STM32_OTG_GCCFG_IDEN;
		ggpio &= ~GGPIO_STM32_OTG_GCCFG_VBDEN;
		dwc2_writel(dwc2, ggpio, GGPIO);

		regulator_disable(dwc2->usb33d);
	}

	if (dwc2->ll_hw_enabled &&
	    (is_device_mode || dwc2_host_can_poweroff_phy(dwc2))) {
		ret = __dwc2_lowlevel_hw_disable(dwc2);
		dwc2->phy_off_for_suspend = true;
	}

	return ret;
}

static int __maybe_unused dwc2_resume(struct device *dev)
{
	struct dwc2_hsotg *dwc2 = dev_get_drvdata(dev);
	int ret = 0;

	if (dwc2->phy_off_for_suspend && dwc2->ll_hw_enabled) {
		ret = __dwc2_lowlevel_hw_enable(dwc2);
		if (ret)
			return ret;
	}
	dwc2->phy_off_for_suspend = false;

	if (dwc2->params.activate_stm_id_vb_detection) {
		unsigned long flags;
		u32 ggpio, gotgctl;

		ret = regulator_enable(dwc2->usb33d);
		if (ret)
			return ret;

		ggpio = dwc2_readl(dwc2, GGPIO);
		ggpio |= GGPIO_STM32_OTG_GCCFG_IDEN;
		ggpio |= GGPIO_STM32_OTG_GCCFG_VBDEN;
		dwc2_writel(dwc2, ggpio, GGPIO);

		/* ID/VBUS detection startup time */
		usleep_range(5000, 7000);

		spin_lock_irqsave(&dwc2->lock, flags);
		gotgctl = dwc2_readl(dwc2, GOTGCTL);
		gotgctl &= ~GOTGCTL_DBNCE_FLTR_BYPASS;
		gotgctl &= ~(GOTGCTL_BVALOEN | GOTGCTL_AVALOEN |
			     GOTGCTL_BVALOVAL | GOTGCTL_AVALOVAL);
		dwc2_writel(dwc2, gotgctl, GOTGCTL);
		spin_unlock_irqrestore(&dwc2->lock, flags);
	}

	/* Need to restore FORCEDEVMODE/FORCEHOSTMODE */
	dwc2_force_dr_mode(dwc2);

	dwc2_drd_resume(dwc2);

	if (dwc2_is_device_mode(dwc2))
		ret = dwc2_hsotg_resume(dwc2);

	return ret;
}

static const struct dev_pm_ops dwc2_dev_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(dwc2_suspend, dwc2_resume)
};

static struct platform_driver dwc2_platform_driver = {
	.driver = {
		.name = dwc2_driver_name,
		.of_match_table = dwc2_of_match_table,
		.pm = &dwc2_dev_pm_ops,
	},
	.probe = dwc2_driver_probe,
	.remove = dwc2_driver_remove,
	.shutdown = dwc2_driver_shutdown,
};

module_platform_driver(dwc2_platform_driver);

MODULE_DESCRIPTION("DESIGNWARE HS OTG Platform Glue");
MODULE_AUTHOR("Matthijs Kooijman <matthijs@stdin.nl>");
MODULE_LICENSE("Dual BSD/GPL");
