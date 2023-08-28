/*
 * CVITEK PHY drivers
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Copyright 2020 CVITEK, Inc.
 */

#include <config.h>
#include <common.h>
#include <linux/bitops.h>
#include <phy.h>
#include "mmio.h"

#define EPHY_EFUSE_VALID_BIT_BASE 0x03050120
#define EPHY_EFUSE_TXECHORC_FLAG 0x00000100 // bit 8
#define EPHY_EFUSE_TXITUNE_FLAG 0x00000200 // bit 9
#define EPHY_EFUSE_TXRXTERM_FLAG 0x00000800 // bit 11

#if defined(CVI_ETH_PHY_LOOPBACK)
static int cv182xa_set_phy_loopback(struct phy_device *phydev, int mode)
{
	return 0
}
#endif

static void cv182xa_ephy_init(void)
{
	uint32_t val = 0;

	// set rg_ephy_apb_rw_sel 0x0804@[0]=1/APB by using APB interface
	mmio_write_32(0x03009804, 0x0001);

	/* do this in board.c */
	// // Release 0x0800[0]=0/shutdown
	// mmio_write_32(0x03009800, 0x0900);

	// // Release 0x0800[2]=1/dig_rst_n, Let mii_reg can be accessabile
	// mmio_write_32(0x03009800, 0x0904);

	//mdelay(10);

	// ANA INIT (PD/EN), switch to MII-page5
	mmio_write_32(0x0300907c, 0x0500);
	// Release ANA_PD p5.0x10@[13:8] = 6'b001100
	mmio_write_32(0x03009040, 0x0c00);
	// Release ANA_EN p5.0x10@[7:0] = 8'b01111110
	mmio_write_32(0x03009040, 0x0c7e);

	// Wait PLL_Lock, Lock_Status p5.0x12@[15] = 1
	//mdelay(1);

	// Release 0x0800[1] = 1/ana_rst_n
	mmio_write_32(0x03009800, 0x0906);

	// ANA INIT
	// @Switch to MII-page5
	mmio_write_32(0x0300907c, 0x0500);

// Efuse register
	// Set Double Bias Current
	//Set rg_eth_txitune1  0x03009064 [15:8]
	//Set rg_eth_txitune0  0x03009064 [7:0]
	if ((mmio_read_32(EPHY_EFUSE_VALID_BIT_BASE) & EPHY_EFUSE_TXITUNE_FLAG) ==
		EPHY_EFUSE_TXITUNE_FLAG) {
		val = ((mmio_read_32(0x03051024) >> 24) & 0xFF) |
				(((mmio_read_32(0x03051024) >> 16) & 0xFF) << 8);
		mmio_clrsetbits_32(0x03009064, 0xFFFF, val);
	} else
		mmio_write_32(0x03009064, 0x5a5a);

	// Set Echo_I
	// Set rg_eth_txechoiadj 0x03009054  [15:8]
	if ((mmio_read_32(EPHY_EFUSE_VALID_BIT_BASE) & EPHY_EFUSE_TXECHORC_FLAG) ==
		EPHY_EFUSE_TXECHORC_FLAG) {
		mmio_clrsetbits_32(0x03009054, 0xFF00, ((mmio_read_32(0x03051024) >> 8) & 0xFF) << 8);
	} else
		mmio_write_32(0x03009054, 0x0000);

	//Set TX_Rterm & Echo_RC_Delay
	// Set rg_eth_txrterm_p1  0x03009058 [11:8]
	// Set rg_eth_txrterm     0x03009058  [7:4]
	// Set rg_eth_txechorcadj 0x03009058  [3:0]
	if ((mmio_read_32(EPHY_EFUSE_VALID_BIT_BASE) & EPHY_EFUSE_TXRXTERM_FLAG) ==
		EPHY_EFUSE_TXRXTERM_FLAG) {
		val = (((mmio_read_32(0x03051020) >> 28) & 0xF) << 4) |
				(((mmio_read_32(0x03051020) >> 24) & 0xF) << 8);
		mmio_clrsetbits_32(0x03009058, 0xFF0, val);
	} else
		mmio_write_32(0x03009058, 0x0bb0);

// ETH_100BaseT
	// Set Rise update
	mmio_write_32(0x0300905c, 0x0c10);

	// Set Falling phase
	mmio_write_32(0x03009068, 0x0003);

	// Set Double TX Bias Current
	mmio_write_32(0x03009054, 0x0000);

	// Switch to MII-page16
	mmio_write_32(0x0300907c, 0x1000);

	// Set MLT3 Positive phase code, Set MLT3 +0
	mmio_write_32(0x03009068, 0x1000);
	mmio_write_32(0x0300906c, 0x3020);
	mmio_write_32(0x03009070, 0x5040);
	mmio_write_32(0x03009074, 0x7060);

	// Set MLT3 +I
	mmio_write_32(0x03009058, 0x1708);
	mmio_write_32(0x0300905c, 0x3827);
	mmio_write_32(0x03009060, 0x5748);
	mmio_write_32(0x03009064, 0x7867);

	// Switch to MII-page17
	mmio_write_32(0x0300907c, 0x1100);

	// Set MLT3 Negative phase code, Set MLT3 -0
	mmio_write_32(0x03009040, 0x9080);
	mmio_write_32(0x03009044, 0xb0a0);
	mmio_write_32(0x03009048, 0xd0c0);
	mmio_write_32(0x0300904c, 0xf0e0);

	// Set MLT3 -I
	mmio_write_32(0x03009050, 0x9788);
	mmio_write_32(0x03009054, 0xb8a7);
	mmio_write_32(0x03009058, 0xd7c8);
	mmio_write_32(0x0300905c, 0xf8e7);

	// @Switch to MII-page5
	mmio_write_32(0x0300907c, 0x0500);

	// En TX_Rterm
	mmio_write_32(0x03009040, (0x0001 | mmio_read_32(0x03009040)));
	// change rx vcm
	mmio_write_32(0x0300904c, (0x820 | mmio_read_32(0x0300904c)));
//	Link Pulse
	// Switch to MII-page10
	mmio_write_32(0x0300907c, 0x0a00);
#if 1
	// Set Link Pulse
	mmio_write_32(0x03009040, 0x3e00);
	mmio_write_32(0x03009044, 0x7864);
	mmio_write_32(0x03009048, 0x6470);
	mmio_write_32(0x0300904c, 0x5f62);
	mmio_write_32(0x03009050, 0x5a5a);
	mmio_write_32(0x03009054, 0x5458);
	mmio_write_32(0x03009058, 0xb23a);
	mmio_write_32(0x0300905c, 0x94a0);
	mmio_write_32(0x03009060, 0x9092);
	mmio_write_32(0x03009064, 0x8a8e);
	mmio_write_32(0x03009068, 0x8688);
	mmio_write_32(0x0300906c, 0x8484);
	mmio_write_32(0x03009070, 0x0082);
#else 
	// from sean
	// Fix err: the status is still linkup when removed the network cable.
	mmio_write_32(0x03009040, 0x2000);
	mmio_write_32(0x03009044, 0x3832);
	mmio_write_32(0x03009048, 0x3132);
	mmio_write_32(0x0300904c, 0x2d2f);
	mmio_write_32(0x03009050, 0x2c2d);
	mmio_write_32(0x03009054, 0x1b2b);
	mmio_write_32(0x03009058, 0x94a0);
	mmio_write_32(0x0300905c, 0x8990);
	mmio_write_32(0x03009060, 0x8788);
	mmio_write_32(0x03009064, 0x8485);
	mmio_write_32(0x03009068, 0x8283);
	mmio_write_32(0x0300906c, 0x8182);
	mmio_write_32(0x03009070, 0x0081);
#endif
// TP_IDLE
	// Switch to MII-page11
	mmio_write_32(0x0300907c, 0x0b00);

// Set TP_IDLE
	mmio_write_32(0x03009040, 0x5252);
	mmio_write_32(0x03009044, 0x5252);
	mmio_write_32(0x03009048, 0x4B52);
	mmio_write_32(0x0300904c, 0x3D47);
	mmio_write_32(0x03009050, 0xAA99);
	mmio_write_32(0x03009054, 0x989E);
	mmio_write_32(0x03009058, 0x9395);
	mmio_write_32(0x0300905C, 0x9091);
	mmio_write_32(0x03009060, 0x8E8F);
	mmio_write_32(0x03009064, 0x8D8E);
	mmio_write_32(0x03009068, 0x8C8C);
	mmio_write_32(0x0300906C, 0x8B8B);
	mmio_write_32(0x03009070, 0x008A);

// ETH 10BaseT Data
	// Switch to MII-page13
	mmio_write_32(0x0300907c, 0x0d00);

	mmio_write_32(0x03009040, 0x1E0A);
	mmio_write_32(0x03009044, 0x3862);
	mmio_write_32(0x03009048, 0x1E62);
	mmio_write_32(0x0300904c, 0x2A08);
	mmio_write_32(0x03009050, 0x244C);
	mmio_write_32(0x03009054, 0x1A44);
	mmio_write_32(0x03009058, 0x061C);

	// Switch to MII-page14
	mmio_write_32(0x0300907c, 0x0e00);

	mmio_write_32(0x03009040, 0x2D30);
	mmio_write_32(0x03009044, 0x3470);
	mmio_write_32(0x03009048, 0x0648);
	mmio_write_32(0x0300904c, 0x261C);
	mmio_write_32(0x03009050, 0x3160);
	mmio_write_32(0x03009054, 0x2D5E);

	// Switch to MII-page15
	mmio_write_32(0x0300907c, 0x0f00);

	mmio_write_32(0x03009040, 0x2922);
	mmio_write_32(0x03009044, 0x366E);
	mmio_write_32(0x03009048, 0x0752);
	mmio_write_32(0x0300904c, 0x2556);
	mmio_write_32(0x03009050, 0x2348);
	mmio_write_32(0x03009054, 0x0C30);

	// Switch to MII-page16
	mmio_write_32(0x0300907c, 0x1000);

	mmio_write_32(0x03009040, 0x1E08);
	mmio_write_32(0x03009044, 0x3868);
	mmio_write_32(0x03009048, 0x1462);
	mmio_write_32(0x0300904c, 0x1A0E);
	mmio_write_32(0x03009050, 0x305E);
	mmio_write_32(0x03009054, 0x2F62);

// LED PAD MUX
	// mmio_write_32(0x030010e0, 0x05);
	// mmio_write_32(0x030010e4, 0x05);
	// //(SD1_CLK selphy)
	// mmio_write_32(0x050270b0, 0x11111111);
	// //(SD1_CMD selphy)
	// mmio_write_32(0x050270b4, 0x11111111);

// LED
	// Switch to MII-page1
	mmio_write_32(0x0300907c, 0x0100);

	// select LED_LNK/SPD/DPX out to LED_PAD
	mmio_write_32(0x03009068, (mmio_read_32(0x03009068) & ~0x0f00));

	/* do this in board.c */
	// // @Switch to MII-page0
	// mmio_write_32(0x0300907c, 0x0000);

	// // PHY_ID
	// mmio_write_32(0x03009008, 0x0043);
	// mmio_write_32(0x0300900c, 0x5649);

	// Switch to MII-page19
	mmio_write_32(0x0300907c, 0x1300);
	mmio_write_32(0x03009058, 0x0012);
	// set agc max/min swing
	mmio_write_32(0x0300905C, 0x6848);

	// Switch to MII-page18
	mmio_write_32(0x0300907c, 0x1200);
#if IS_ENABLED(CONFIG_TARGET_CVITEK_CV181X)
	/* mars LPF(8, 8, 8, 8) HPF(-8, 50(+32), -36, -8) */
	// lpf
	mmio_write_32(0x03009048, 0x0808);
	mmio_write_32(0x0300904C, 0x0808);
	// hpf
	mmio_write_32(0x03009050, 0x32f8);
	mmio_write_32(0x03009054, 0xf8dc);
#elif IS_ENABLED(CONFIG_TARGET_CVITEK_CV180X)
	/* phobos LPF:(1 8 23 23 8 1) HPF:(-4,58,-45,8,-5, 0) from sean PPT */
	// lpf
	mmio_write_32(0x03009048, 0x0801);
	mmio_write_32(0x0300904C, 0x1717);
	mmio_write_32(0x0300905C, 0x0108);
	// hpf
	mmio_write_32(0x03009050, 0x3afc);
	mmio_write_32(0x03009054, 0x08d3);
	mmio_write_32(0x03009060, 0x00fb);
#endif
	// Switch to MII-page0
	mmio_write_32(0x0300907c, 0x0000);
	// EPHY start auto-neg procedure
	mmio_write_32(0x03009800, 0x090e);

	// from jinyu.zhao
	/* EPHY is configured as half-duplex after reset, but we need force full-duplex */
	mmio_write_32(0x03009000, (mmio_read_32(0x03009000) | 0x100));

	// switch to MDIO control by ETH_MAC
	mmio_write_32(0x03009804, 0x0000);
}

/* CVITEK cv182xa */
static int cv182xa_config(struct phy_device *phydev)
{
	//phy_reset(phydev);

	cv182xa_ephy_init();/* config cvitek cv182xa eth internal phy on ASIC board */

	genphy_config(phydev);

#if defined(CVI_ETH_PHY_LOOPBACK)
	cv182xa_set_phy_loopback(phydev, LOOPBACK_PCS2MAC);
#endif

	return 0;
}

static int cv182xa_parse_status(struct phy_device *phydev)
{
	int mii_reg;

	mii_reg = phy_read(phydev, MDIO_DEVAD_NONE, MII_BMSR);

	if (mii_reg & (BMSR_100FULL | BMSR_100HALF))
		phydev->speed = SPEED_100;
	else
		phydev->speed = SPEED_10;

	if (mii_reg & (BMSR_10FULL | BMSR_100FULL))
		phydev->duplex = DUPLEX_FULL;
	else
		phydev->duplex = DUPLEX_HALF;

	return 0;
}

static int cv182xa_startup(struct phy_device *phydev)
{
	int ret;

	/* Read the Status (2x to make sure link is right) */
	ret = genphy_update_link(phydev);
	if (ret)
		return ret;

	return cv182xa_parse_status(phydev);
}

/* Support for cv182xa PHYs */
static struct phy_driver cv182xa_driver = {
	.name = "CVITEK CV182XA",
	.uid = 0x00435649,
	.mask = 0xffffffff,
	.features = PHY_BASIC_FEATURES,
	.config = &cv182xa_config,
	.startup = &cv182xa_startup,
	.shutdown = &genphy_shutdown,
};

int phy_cvitek_init(void)
{
	phy_register(&cv182xa_driver);

	return 0;
}
