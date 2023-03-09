// SPDX-License-Identifier: GPL-2.0-or-later
/* Driver for CVITEK PHYs */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/bitops.h>

#define CVI_ETH_PHY_ADC_CTRL		16
#define CVI_ETH_PHY_INT_STS			16
#define CVI_ETH_AUTO_CALIBRATE		16
#define CVI_ETH_PHY_INT_MASK		17
#define CVI_ETH_PHY_APS				18
#define CVI_ETH_PHY_UAPS			19
#define CVI_ETH_PHY_LB_CTL			18
#define CVI_ETH_PHY_AFE_TX			20
#define CVI_ETH_PHY_AFE_DRV2		21
#define CVI_ETH_PHY_CP				23
#define CVI_ETH_PHY_ADC_OP_BIAS		24
#define CVI_ETH_PHY_STATUS			25
#define CVI_ETH_PHY_RX_SIGNAL		25
#define CVI_ETH_PHY_2_MISC			28
#define CVI_ETH_PHY_TXDATA_CTRL		24
#define CVI_ETH_PHY_PAGE_SEL		31

#define CVI_ETH_PHY_PAGE_0		0x0000
#define CVI_ETH_PHY_PAGE_1		0x0100
#define CVI_ETH_PHY_PAGE_2		0x0200
#define CVI_ETH_PHY_PAGE_3		0x0300
#define CVI_ETH_PHY_PAGE_6		0x0600
#define CVI_ETH_PHY_PAGE_8		0x0800
#define CVI_ETH_PHY_PAGE_9		0x0900

#define CVI_ETH_PHY_LOOPBACK
#define LOOPBACK_XMII2MAC		0x8000
#define LOOPBACK_PCS2MAC		0x2000
#define LOOPBACK_PMA2MAC		0x1000
#define LOOPBACK_RMII2PHY		0x0080

#define CVI_LNK_STS_CHG_INT_MSK		BIT(15)
#define CVI_MGC_PKT_DET_INT_MSK		BIT(14)
#define CVI_TX_LPI_RCV_INT_MSK		BIT(13)
#define CVI_RX_LPI_RCV_INT_MSK		BIT(12)

#define CVI_UAPS_ENABLE				BIT(15)

#define CVI_CKSELIN					BIT(13)

#define CVI_INT_EVENTS \
	(CVI_LNK_STS_CHG_INT_MSK | CVI_MGC_PKT_DET_INT_MSK)

static int cvi_phy_config_intr(struct phy_device *phydev)
{
	int rc = phy_write(phydev, CVI_ETH_PHY_INT_MASK,
			((phydev->interrupts == PHY_INTERRUPT_ENABLED)
			? CVI_INT_EVENTS
			: 0));
	return rc < 0 ? rc : 0;
}

static int cvi_phy_ack_interrupt(struct phy_device *phydev)
{
	int err;

	pr_debug("%s, clear interrupts\n", __func__);
	err = phy_write(phydev, CVI_ETH_PHY_INT_STS, 0xf000);

	return err < 0 ? err : 0;
}

static int cvi_read_status(struct phy_device *phydev)
{
	int err = genphy_read_status(phydev);

	pr_debug("%s, speed=%d, duplex=%d, ", __func__, phydev->speed, phydev->duplex);
	pr_debug("pasue=%d, asym_pause=%d, autoneg=%d ", phydev->pause, phydev->asym_pause, phydev->autoneg);
	pr_debug("cvi_Status=0x%04x\n", phy_read(phydev, CVI_ETH_PHY_STATUS));

	return err;
}

#if defined(CONFIG_CVITEK_PHY_UAPS)
/* Ultra Auto Power Saving mode */
static int cvi_phy_aps_enable(struct phy_device *phydev)
{
	int val;

	pr_info("Enable CVITEK ethernet phy ultra auto power saving mode\n");
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_1);

	val = phy_read(phydev, CVI_ETH_PHY_UAPS);
	phy_write(phydev, CVI_ETH_PHY_UAPS, CVI_UAPS_ENABLE | val);
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_0);

	return 0;
}
#endif

static int cvi_phy_config_aneg(struct phy_device *phydev)
{
	int ret;

	/* switch to page 1 */
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_1);
	/* Disable APS */
	phy_write(phydev, CVI_ETH_PHY_APS, 0x4824);
	/* switch to page 2 */
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_2);
	/* PHYAFE TRX optimization */
	phy_write(phydev, CVI_ETH_PHY_TXDATA_CTRL, 0x0000);
	/* switch to page 6 */
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_6);
	/* PHYAFE ADC optimization */
	phy_write(phydev, CVI_ETH_PHY_ADC_CTRL, 0x555b);
	/* PHYAFE TX optimization */
	phy_write(phydev, CVI_ETH_PHY_AFE_TX, 0x508f);
	/* enable a_CLKSELIN to avoid CRC error and increase  10M/100M amplitude*/
	phy_write(phydev, CVI_ETH_PHY_AFE_DRV2, 0x3030);
	/* PHYAFE CP current optimization */
	phy_write(phydev, CVI_ETH_PHY_CP, 0x0575);
	/* PHYAFE ADC OP BIAS optimization */
	phy_write(phydev, CVI_ETH_PHY_ADC_OP_BIAS, 0x0000);
	/* PHYAFE RX signal detector level optimization */
	phy_write(phydev, CVI_ETH_PHY_RX_SIGNAL, 0x0408);
	/* Enable PD control optimization */
	phy_write(phydev, CVI_ETH_PHY_2_MISC, 0x8880);

	/* switch to page 8 */
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_8);
	/* Stop auto-calibrate */
	phy_write(phydev, CVI_ETH_AUTO_CALIBRATE, 0x0844);

#if defined(CONFIG_CVITEK_PHY_UAPS)
	cvi_phy_aps_enable(phydev); /* if phy not work, disable this function for try */
#endif
	/* switch back to page 0 */
	phy_write(phydev, CVI_ETH_PHY_PAGE_SEL, CVI_ETH_PHY_PAGE_0);

	ret = genphy_config_aneg(phydev);

	if (ret < 0)
		return ret;

	return 0;
}

static int cvi_phy_config_init(struct phy_device *phydev)
{
	int ret;

	ret = genphy_read_abilities(phydev);
	if (ret < 0)
		return ret;

	return 0;
}

static struct phy_driver cvitek_phy_driver[] = {
{
	.phy_id		= 0x00441400,
	.phy_id_mask	= 0xfffffff0,
	.name		= "CVITEK CVI18XX",
    .get_features	= genphy_read_abilities,
	.read_status	= cvi_read_status,
	/* IRQ related */
	.ack_interrupt	= cvi_phy_ack_interrupt,
	.config_intr	= cvi_phy_config_intr,
	.aneg_done	= genphy_aneg_done,
	.suspend	= genphy_suspend,
	.resume		= genphy_resume,
	.set_loopback   = genphy_loopback,
} };

module_phy_driver(cvitek_phy_driver);

MODULE_DESCRIPTION("CVITEK PHY driver");
MODULE_AUTHOR("Ethan Chen");
MODULE_LICENSE("GPL");

static struct mdio_device_id __maybe_unused cvitek_tbl[] = {
	{ 0x00441400, 0xfffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, cvitek_tbl);
