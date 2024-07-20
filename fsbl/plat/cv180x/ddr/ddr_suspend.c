// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <delay_timer.h>
#include "mmio.h"
#include "reg_soc.h"
#include "phy_pll_init.h"
#include "ddr_sys.h"
#include "ddr_suspend.h"
#include "bitwise_ops.h"
// #include "regconfig.h"
#include "rtc.h"
#include "cvx16_dram_cap_check.h"
#ifdef DDR2_3
#include <ddr3_1866_init.h>
#include <ddr2_1333_init.h>
#else
#include <ddr_init.h>
#endif

void ddr_suspend_entry(void)
{
	ddr_sys_suspend_sus_res();
	rtc_clr_ddr_pwrok();
	rtc_clr_rmio_pwrok();
#ifndef SUSPEND_USE_WDG_RST
	rtc_req_suspend();
#else
	rtc_req_wdg_rst();
#endif
}

void ddr_sys_suspend_sus_res(void)
{
	cvx16_ddrc_suspend_sus_res();

	cvx16_ddr_phyd_save_sus_res(0x05026800);

	cvx16_ddr_phya_pd_sus_res();
}

void cvx16_ddrc_suspend_sus_res(void)
{
	uint32_t rddata;

	// Write 0 to PCTRL_n.port_en
	for (int i = 0; i < 4; i++)
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);

	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		if (rddata == 0)
			break;
	}

	//Write 1 to PWRCTL.selfref_sw
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = FIELD_SET(rddata, 1, 5, 5);
	mmio_wr32(cfg_base + 0x30, rddata);

    //Poll STAT.selfref_type= 2'b10
    //Poll STAT.selfref_state = 0b10 (LPDDR4 only)
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		if (FIELD_GET(rddata, 5, 4) == 0x2)
			break;
	}
}

void cvx16_ddr_phya_pd_sus_res(void)
{
	uint32_t rddata;
	// ----------- PHY oen/pd reset ----------------
	//OEN
	//param_phyd_tx_ca_oenz         0
	//param_phyd_tx_ca_clk0_oenz    8
	//param_phyd_tx_ca_clk1_oenz    16
	rddata = 0x00010101;
	mmio_wr32(0x0130 + PHYD_BASE_ADDR, rddata);
	//PD
	//TOP_REG_TX_CA_PD_CA       22	0
	//TOP_REG_TX_CA_PD_CKE0     24	24
	//TOP_REG_TX_CLK_PD_CLK0    26	26
	//TOP_REG_TX_CA_PD_CSB0     28	28
	//TOP_REG_TX_CA_PD_RESETZ   30	30
	//TOP_REG_TX_ZQ_PD          31	31
	//rddata[31:0] = 0x947f_ffff;
	rddata = 0x947fffff;
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);

	//TOP_REG_TX_BYTE0_PD	0
	//TOP_REG_TX_BYTE1_PD	1
	rddata = 0x00000003;
	mmio_wr32(0x00 + CV_DDR_PHYD_APB, rddata);

	//OEN
	//param_phyd_sel_cke_oenz        <= `PI_SD int_regin[0];
	rddata = mmio_rd32(0x0154 + PHYD_BASE_ADDR);
	//rddata[0] = 0b1;
	rddata = FIELD_SET(rddata, 1, 0, 0);
	mmio_wr32(0x0154 + PHYD_BASE_ADDR, rddata);

	//PD
	//All PHYA PD=0
	rddata = 0xffffffff;
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);

	//PLL PD
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	//rddata[15]   = 1;    //TOP_REG_DDRPLL_PD
	rddata = FIELD_SET(rddata, 1, 15, 15);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);

	// ----------- PHY oen/pd reset ----------------
	//reg_ddr_ssc_syn_src_en =0
	rddata = mmio_rd32(0x40 + 0x03002900);
	rddata = FIELD_SET(rddata, 0, 1, 1); //reg_ddr_ssc_syn_src_en
	mmio_wr32(0x40 + 0x03002900, rddata);
}

SUSPEND_DATA struct reg save_phy_regs[] = {
	{0x0 + PHYD_BASE_ADDR, 0x0},
	{0x4 + PHYD_BASE_ADDR, 0x0},
	{0x8 + PHYD_BASE_ADDR, 0x0},
	{0xc + PHYD_BASE_ADDR, 0x0},
	{0x10 + PHYD_BASE_ADDR, 0x0},
	{0x14 + PHYD_BASE_ADDR, 0x0},
	{0x18 + PHYD_BASE_ADDR, 0x0},
	{0x1c + PHYD_BASE_ADDR, 0x0},
	{0x20 + PHYD_BASE_ADDR, 0x0},
	{0x24 + PHYD_BASE_ADDR, 0x0},
	{0x28 + PHYD_BASE_ADDR, 0x0},
	{0x2c + PHYD_BASE_ADDR, 0x0},
	{0x40 + PHYD_BASE_ADDR, 0x0},
	{0x44 + PHYD_BASE_ADDR, 0x0},
	{0x48 + PHYD_BASE_ADDR, 0x0},
	{0x4c + PHYD_BASE_ADDR, 0x0},
	{0x50 + PHYD_BASE_ADDR, 0x0},
	{0x54 + PHYD_BASE_ADDR, 0x0},
	{0x58 + PHYD_BASE_ADDR, 0x0},
	{0x5c + PHYD_BASE_ADDR, 0x0},
	{0x60 + PHYD_BASE_ADDR, 0x0},
	{0x64 + PHYD_BASE_ADDR, 0x0},
	{0x68 + PHYD_BASE_ADDR, 0x0},
	{0x70 + PHYD_BASE_ADDR, 0x0},
	{0x74 + PHYD_BASE_ADDR, 0x0},
	{0x80 + PHYD_BASE_ADDR, 0x0},
	{0x84 + PHYD_BASE_ADDR, 0x0},
	{0x88 + PHYD_BASE_ADDR, 0x0},
	{0x8c + PHYD_BASE_ADDR, 0x0},
	{0x90 + PHYD_BASE_ADDR, 0x0},
	{0x94 + PHYD_BASE_ADDR, 0x0},
	{0xa0 + PHYD_BASE_ADDR, 0x0},
	{0xa4 + PHYD_BASE_ADDR, 0x0},
	{0xa8 + PHYD_BASE_ADDR, 0x0},
	{0xac + PHYD_BASE_ADDR, 0x0},
	{0xb0 + PHYD_BASE_ADDR, 0x0},
	{0xb4 + PHYD_BASE_ADDR, 0x0},
	{0xb8 + PHYD_BASE_ADDR, 0x0},
	{0xbc + PHYD_BASE_ADDR, 0x0},
	{0xf8 + PHYD_BASE_ADDR, 0x0},
	{0xfc + PHYD_BASE_ADDR, 0x0},
	{0x100 + PHYD_BASE_ADDR, 0x0},
	{0x104 + PHYD_BASE_ADDR, 0x0},
	{0x10c + PHYD_BASE_ADDR, 0x0},
	{0x110 + PHYD_BASE_ADDR, 0x0},
	{0x114 + PHYD_BASE_ADDR, 0x0},
	{0x118 + PHYD_BASE_ADDR, 0x0},
	{0x11c + PHYD_BASE_ADDR, 0x0},
	{0x120 + PHYD_BASE_ADDR, 0x0},
	{0x124 + PHYD_BASE_ADDR, 0x0},
	{0x128 + PHYD_BASE_ADDR, 0x0},
	{0x12c + PHYD_BASE_ADDR, 0x0},
	{0x130 + PHYD_BASE_ADDR, 0x0},
	{0x134 + PHYD_BASE_ADDR, 0x0},
	{0x138 + PHYD_BASE_ADDR, 0x0},
	{0x140 + PHYD_BASE_ADDR, 0x0},
	{0x144 + PHYD_BASE_ADDR, 0x0},
	{0x148 + PHYD_BASE_ADDR, 0x0},
	{0x14c + PHYD_BASE_ADDR, 0x0},
	{0x150 + PHYD_BASE_ADDR, 0x0},
	{0x154 + PHYD_BASE_ADDR, 0x0},
	{0x158 + PHYD_BASE_ADDR, 0x0},
	{0x15c + PHYD_BASE_ADDR, 0x0},
	{0x164 + PHYD_BASE_ADDR, 0x0},
	{0x168 + PHYD_BASE_ADDR, 0x0},
	{0x16c + PHYD_BASE_ADDR, 0x0},
	{0x170 + PHYD_BASE_ADDR, 0x0},
	{0x174 + PHYD_BASE_ADDR, 0x0},
	{0x180 + PHYD_BASE_ADDR, 0x0},
	{0x184 + PHYD_BASE_ADDR, 0x0},
	{0x188 + PHYD_BASE_ADDR, 0x0},
	{0x18c + PHYD_BASE_ADDR, 0x0},
	{0x190 + PHYD_BASE_ADDR, 0x0},
	{0x200 + PHYD_BASE_ADDR, 0x0},
	{0x204 + PHYD_BASE_ADDR, 0x0},
	{0x208 + PHYD_BASE_ADDR, 0x0},
	{0x220 + PHYD_BASE_ADDR, 0x0},
	{0x224 + PHYD_BASE_ADDR, 0x0},
	{0x228 + PHYD_BASE_ADDR, 0x0},
	{0x400 + PHYD_BASE_ADDR, 0x0},
	{0x404 + PHYD_BASE_ADDR, 0x0},
	{0x408 + PHYD_BASE_ADDR, 0x0},
	{0x40c + PHYD_BASE_ADDR, 0x0},
	{0x410 + PHYD_BASE_ADDR, 0x0},
	{0x414 + PHYD_BASE_ADDR, 0x0},
	{0x418 + PHYD_BASE_ADDR, 0x0},
	{0x41c + PHYD_BASE_ADDR, 0x0},
	{0x500 + PHYD_BASE_ADDR, 0x0},
	{0x504 + PHYD_BASE_ADDR, 0x0},
	{0x508 + PHYD_BASE_ADDR, 0x0},
	{0x50c + PHYD_BASE_ADDR, 0x0},
	{0x510 + PHYD_BASE_ADDR, 0x0},
	{0x514 + PHYD_BASE_ADDR, 0x0},
	{0x518 + PHYD_BASE_ADDR, 0x0},
	{0x51c + PHYD_BASE_ADDR, 0x0},
	{0x520 + PHYD_BASE_ADDR, 0x0},
	{0x540 + PHYD_BASE_ADDR, 0x0},
	{0x544 + PHYD_BASE_ADDR, 0x0},
	{0x548 + PHYD_BASE_ADDR, 0x0},
	{0x54c + PHYD_BASE_ADDR, 0x0},
	{0x550 + PHYD_BASE_ADDR, 0x0},
	{0x554 + PHYD_BASE_ADDR, 0x0},
	{0x558 + PHYD_BASE_ADDR, 0x0},
	{0x55c + PHYD_BASE_ADDR, 0x0},
	{0x560 + PHYD_BASE_ADDR, 0x0},
	{0x900 + PHYD_BASE_ADDR, 0x0},
	{0x904 + PHYD_BASE_ADDR, 0x0},
	{0x908 + PHYD_BASE_ADDR, 0x0},
	{0x90c + PHYD_BASE_ADDR, 0x0},
	{0x910 + PHYD_BASE_ADDR, 0x0},
	{0x914 + PHYD_BASE_ADDR, 0x0},
	{0x918 + PHYD_BASE_ADDR, 0x0},
	{0x91c + PHYD_BASE_ADDR, 0x0},
	{0x920 + PHYD_BASE_ADDR, 0x0},
	{0x924 + PHYD_BASE_ADDR, 0x0},
	{0x928 + PHYD_BASE_ADDR, 0x0},
	{0x92c + PHYD_BASE_ADDR, 0x0},
	{0x930 + PHYD_BASE_ADDR, 0x0},
	{0x934 + PHYD_BASE_ADDR, 0x0},
	{0x938 + PHYD_BASE_ADDR, 0x0},
	{0x940 + PHYD_BASE_ADDR, 0x0},
	{0x944 + PHYD_BASE_ADDR, 0x0},
	{0x948 + PHYD_BASE_ADDR, 0x0},
	{0x94c + PHYD_BASE_ADDR, 0x0},
	{0x950 + PHYD_BASE_ADDR, 0x0},
	{0x954 + PHYD_BASE_ADDR, 0x0},
	{0x958 + PHYD_BASE_ADDR, 0x0},
	{0x95c + PHYD_BASE_ADDR, 0x0},
	{0x960 + PHYD_BASE_ADDR, 0x0},
	{0x964 + PHYD_BASE_ADDR, 0x0},
	{0x968 + PHYD_BASE_ADDR, 0x0},
	{0x96c + PHYD_BASE_ADDR, 0x0},
	{0x970 + PHYD_BASE_ADDR, 0x0},
	{0x974 + PHYD_BASE_ADDR, 0x0},
	{0x978 + PHYD_BASE_ADDR, 0x0},
	{0x97c + PHYD_BASE_ADDR, 0x0},
	{0x980 + PHYD_BASE_ADDR, 0x0},
	{0xa00 + PHYD_BASE_ADDR, 0x0},
	{0xa04 + PHYD_BASE_ADDR, 0x0},
	{0xa08 + PHYD_BASE_ADDR, 0x0},
	{0xa0c + PHYD_BASE_ADDR, 0x0},
	{0xa10 + PHYD_BASE_ADDR, 0x0},
	{0xa14 + PHYD_BASE_ADDR, 0x0},
	{0xa18 + PHYD_BASE_ADDR, 0x0},
	{0xa1c + PHYD_BASE_ADDR, 0x0},
	{0xa20 + PHYD_BASE_ADDR, 0x0},
	{0xa24 + PHYD_BASE_ADDR, 0x0},
	{0xa28 + PHYD_BASE_ADDR, 0x0},
	{0xa2c + PHYD_BASE_ADDR, 0x0},
	{0xa30 + PHYD_BASE_ADDR, 0x0},
	{0xa34 + PHYD_BASE_ADDR, 0x0},
	{0xa38 + PHYD_BASE_ADDR, 0x0},
	{0xa3c + PHYD_BASE_ADDR, 0x0},
	{0xa40 + PHYD_BASE_ADDR, 0x0},
	{0xa44 + PHYD_BASE_ADDR, 0x0},
	{0xa48 + PHYD_BASE_ADDR, 0x0},
	{0xa4c + PHYD_BASE_ADDR, 0x0},
	{0xa50 + PHYD_BASE_ADDR, 0x0},
	{0xa54 + PHYD_BASE_ADDR, 0x0},
	{0xa58 + PHYD_BASE_ADDR, 0x0},
	{0xa5c + PHYD_BASE_ADDR, 0x0},
	{0xa60 + PHYD_BASE_ADDR, 0x0},
	{0xa64 + PHYD_BASE_ADDR, 0x0},
	{0xa68 + PHYD_BASE_ADDR, 0x0},
	{0xa6c + PHYD_BASE_ADDR, 0x0},
	{0xa70 + PHYD_BASE_ADDR, 0x0},
	{0xa74 + PHYD_BASE_ADDR, 0x0},
	{0xa78 + PHYD_BASE_ADDR, 0x0},
	{0xa7c + PHYD_BASE_ADDR, 0x0},
	{0xb00 + PHYD_BASE_ADDR, 0x0},
	{0xb04 + PHYD_BASE_ADDR, 0x0},
	{0xb08 + PHYD_BASE_ADDR, 0x0},
	{0xb0c + PHYD_BASE_ADDR, 0x0},
	{0xb10 + PHYD_BASE_ADDR, 0x0},
	{0xb14 + PHYD_BASE_ADDR, 0x0},
	{0xb18 + PHYD_BASE_ADDR, 0x0},
	{0xb1c + PHYD_BASE_ADDR, 0x0},
	{0xb20 + PHYD_BASE_ADDR, 0x0},
	{0xb24 + PHYD_BASE_ADDR, 0x0},
	{0xb30 + PHYD_BASE_ADDR, 0x0},
	{0xb34 + PHYD_BASE_ADDR, 0x0},
	{0xb38 + PHYD_BASE_ADDR, 0x0},
	{0xb3c + PHYD_BASE_ADDR, 0x0},
	{0xb40 + PHYD_BASE_ADDR, 0x0},
	{0xb44 + PHYD_BASE_ADDR, 0x0},
	{0xb48 + PHYD_BASE_ADDR, 0x0},
	{0xb4c + PHYD_BASE_ADDR, 0x0},
	{0xb50 + PHYD_BASE_ADDR, 0x0},
	{0xb54 + PHYD_BASE_ADDR, 0x0},
	{0xDEADBEEF, 0xDEADBEEF},
};

void cvx16_ddr_phyd_save_sus_res(uint32_t RTC_SRAM_BASE)
{
	// struct reg *psave_phy_regs = (struct reg *)RTC_SRAM_BASE;
	uintptr_t temp = RTC_SRAM_BASE;
	struct reg *psave_phy_regs = (struct reg *)temp;
	int i;

	for (i = 0; i < ARRAY_SIZE(save_phy_regs) - 1; i++) {
		psave_phy_regs[i].addr = save_phy_regs[i].addr;
		psave_phy_regs[i].val = mmio_rd32(save_phy_regs[i].addr);
	}
}

void rtc_clr_ddr_pwrok(void)
{
	mmio_clrbits_32(REG_RTC_BASE + RTC_PG_REG, 0x00000001);
}

void rtc_clr_rmio_pwrok(void)
{
	mmio_clrbits_32(REG_RTC_BASE + RTC_PG_REG, 0x00000002);
}

#ifndef SUSPEND_USE_WDG_RST
void rtc_req_suspend(void)
{
	//info("Send suspend request\n");
	/* Enable power suspend wakeup source mask */
	mmio_write_32(REG_RTC_BASE + 0x3C, 0x1); // 1 = select prdata from 32K domain
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	if (mmio_read_32(RTC_INFO0) != MCU_FLAG)
		mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_WAKEUP, 0x3F);
	else
		mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_WAKEUP, 0x0);
	mmio_write_32(REG_RTC_BASE + RTC_EN_SUSPEND_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_SUSPEND_REQ) != 0x01)
		;
	while (1) {
		/* Send suspend request to RTC */
		mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, 0x00800080);
		mdelay(1);
	}
}
#else
void rtc_req_wdg_rst(void)
{
	uint32_t write_data = 0;

	write_data = mmio_rd32(REG_RTC_CTRL_BASE + 0x18); //rtcsys_rst_ctrl
	write_data = write_data | (0x01 << 24); //reg_rtcsys_reset_en
	mmio_wr32(REG_RTC_CTRL_BASE + 0x18, write_data); //
	mmio_wr32(REG_RTC_BASE + 0xE8, 0x04); // RTC_DB_REQ_WARM_RST
	mmio_wr32(REG_RTC_BASE + 0xE0, 0x01); // RTC_EN_WDG_RST_REQ
	mmio_wr32(REG_RTC_CTRL_BASE + 0x60, 0xA5);	  // write dummy register
	mmio_wr32(REG_RTC_CTRL_BASE + 0x04, 0xAB18);	  // rtc_ctrl0_unlockkey
	write_data = mmio_rd32(REG_RTC_CTRL_BASE + 0x08); // rtc_ctrl0
	//req_shdn         = rtc_ctrl0[0];
	//req_sw_thm_shdn  = rtc_ctrl0[1];
	//hw_thm_shdn_en   = rtc_ctrl0[2];
	//req_pwr_cyc      = rtc_ctrl0[3];
	//req_warm_rst     = rtc_ctrl0[4];
	//req_sw_wdg_rst   = rtc_ctrl0[5];
	//hw_wdg_rst_en    = rtc_ctrl0[6];
	//req_suspend      = rtc_ctrl0[7];
	write_data = 0xffff0000 | write_data | (0x01 << 5);
	// printf("[RTC] ----> Set req_sw_wdg_rst to 1 by register setting\n");
	mmio_wr32(REG_RTC_CTRL_BASE + 0x08, write_data); //rtc_ctrl0
}
#endif
