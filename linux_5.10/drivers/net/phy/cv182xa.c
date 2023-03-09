// SPDX-License-Identifier: GPL-2.0-or-later
/* Driver for CVITEK PHYs */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/bitops.h>

#define CVI_INT_EVENTS \
	(CVI_LNK_STS_CHG_INT_MSK | CVI_MGC_PKT_DET_INT_MSK)

static int cv182xa_phy_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int cv182xa_phy_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int cv182xa_read_status(struct phy_device *phydev)
{
	int err = genphy_read_status(phydev);

	pr_debug("%s, speed=%d, duplex=%d, ", __func__, phydev->speed, phydev->duplex);
	pr_debug("pasue=%d, asym_pause=%d, autoneg=%d ", phydev->pause, phydev->asym_pause, phydev->autoneg);

	return err;
}

#if defined(CONFIG_CVITEK_PHY_UAPS)
/* Ultra Auto Power Saving mode */
static int cv182xa_phy_aps_enable(struct phy_device *phydev)
{
	return 0;
}
#endif

static int cv182xa_phy_config_aneg(struct phy_device *phydev)
{
	int ret;

#if defined(CONFIG_CVITEK_PHY_UAPS)
	cv182xa_phy_aps_enable(phydev); /* if phy not work, disable this function for try */
#endif

	ret = genphy_config_aneg(phydev);

	if (ret < 0)
		return ret;

	return 0;
}

static int cv182xa_phy_config_init(struct phy_device *phydev)
{
	return 0;
}

static struct phy_driver cv182xa_phy_driver[] = {
{
	.phy_id		= 0x00435649,
	.phy_id_mask	= 0xffffffff,
	.name		= "CVITEK CV182XA",
	.config_init	= cv182xa_phy_config_init,
	.config_aneg	= cv182xa_phy_config_aneg,
	.read_status	= cv182xa_read_status,
	/* IRQ related */
	.ack_interrupt	= cv182xa_phy_ack_interrupt,
	.config_intr	= cv182xa_phy_config_intr,
	.aneg_done	= genphy_aneg_done,
	.suspend	= genphy_suspend,
	.resume		= genphy_resume,
	.set_loopback   = genphy_loopback,
} };

module_phy_driver(cv182xa_phy_driver);

MODULE_DESCRIPTION("CV182XA EPHY driver");
MODULE_AUTHOR("Ethan Chen");
MODULE_LICENSE("GPL");

static struct mdio_device_id __maybe_unused cv182xa_tbl[] = {
	{ 0x00435649, 0xffffffff },
	{ }
};

MODULE_DEVICE_TABLE(mdio, cv182xa_tbl);
