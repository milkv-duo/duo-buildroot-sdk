#include <mmio.h>
#include <reg_soc.h>
#include <ddr_sys.h>
#ifdef DDR2_3
#include <ddr3_1866_init.h>
#include <ddr2_1333_init.h>
#else
#include <ddr_init.h>
#endif
#include <bitwise_ops.h>
#include <delay_timer.h>
#include <cvx16_dram_cap_check.h>
#include <cvx16_pinmux.h>
#include <ddr_pkg_info.h>
#include <regconfig.h>
#include <phy_pll_init.h>
#include <platform_def.h>

#define opdelay(_x) udelay((_x)/1000)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
// #pragma GCC diagnostic ignored "-Wunused-variable"

uint32_t rddata;

/*
 * unused
void check_rd32(uintptr_t addr, uint32_t expected)
{
	// uint32_t rdata;
	// rdata = mmio_rd32(addr);
	// if (rdata != expected)
	//	mmio_wr32(TERM_SIM_ADDR, 0x0B0B0B0B); // fail
}
*/

void ddr_debug_num_write(void)
{
	// debug_seqnum = debug_seqnum+1 ;
	// mmio_wr32(4*(186 + PHY_BASE_ADDR)+CADENCE_PHYD,(debug_seqnum<<8));
	// if (debug_seqnum ==255){ ;
	// debug_seqnum1 = debug_seqnum1+1 ;
	// mmio_wr32(4*(186 + PHY_BASE_ADDR)+CADENCE_PHYD,(debug_seqnum1<<8));
	// debug_seqnum = 0 ;
	// } ;
}

void load_ddr_patch_set_data(void)
{
#ifdef DDR_SIM
	return;
#endif
	// NOTICE("ddr_patch_regs_count=%d\n", ddr_patch_regs_count);
#ifndef DDR2_3
	for (int i = 0; i < ddr_patch_regs_count; i++) {
		uint32_t addr = ddr_patch_regs[i].addr;
		uint32_t mask = ddr_patch_regs[i].mask;
		uint32_t val = ddr_patch_regs[i].val;
		uint32_t orig;

		orig = mmio_rd32(addr);
		orig &= ~mask;
		mmio_wr32(addr, orig | (val & mask));
	}
#else
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		for (int i = 0; i < ddr3_1866_patch_regs_count; i++) {
			uint32_t addr = ddr3_1866_patch_regs[i].addr;
			uint32_t mask = ddr3_1866_patch_regs[i].mask;
			uint32_t val = ddr3_1866_patch_regs[i].val;

			uint32_t orig;

			orig = mmio_rd32(addr);
			orig &= ~mask;
			mmio_wr32(addr, orig | (val & mask));
		}
	} else if (get_ddr_type() == DDR_TYPE_DDR2) {
		for (int i = 0; i < ddr2_1333_patch_regs_count; i++) {
			uint32_t addr = ddr2_1333_patch_regs[i].addr;
			uint32_t mask = ddr2_1333_patch_regs[i].mask;
			uint32_t val = ddr2_1333_patch_regs[i].val;

			uint32_t orig;

			orig = mmio_rd32(addr);
			orig &= ~mask;
			mmio_wr32(addr, orig | (val & mask));
		}
	} else {
		NOTICE("error ddr type.\n");
	}
#endif
}

void ddr_patch_set(void)
{
	load_ddr_patch_set_data();

#ifdef DDR3_DBG
	mmio_wr32(0x005C + PHYD_BASE_ADDR, 0x00FE0000);//wrlvl response only DQ0
#endif//DDR3_DBG
}

void cvx16_rdvld_train(void)
{
	uint32_t byte0_vld;
	uint32_t byte1_vld;
	uint32_t rdvld_offset;
	uint32_t bist_result;
	uint64_t err_data_odd;
	uint64_t err_data_even;

	cvx16_bist_wr_prbs_init();
	// cvx16_bist_wr_sram_init();

	byte0_vld = mmio_rd32(0x0B14 + PHYD_BASE_ADDR);
	byte1_vld = mmio_rd32(0x0B44 + PHYD_BASE_ADDR);
	rdvld_offset = mmio_rd32(0x0094 + PHYD_BASE_ADDR);
	rdvld_offset = get_bits_from_value(rdvld_offset, 3, 0);

	for (int i = 9; i > 1; i--) {
		byte0_vld = modified_bits_by_value(byte0_vld, i, 20, 16);
		mmio_wr32(0x0B14 + PHYD_BASE_ADDR, byte0_vld);
		byte1_vld = modified_bits_by_value(byte1_vld, i, 20, 16);
		mmio_wr32(0x0B44 + PHYD_BASE_ADDR, byte0_vld);
		cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);

		// KC_MSG(", bist_result = %x, err_data_odd = %x, err_data_even = %x\n",
		//         bist_result,err_data_odd,err_data_even);
		if (bist_result == 0) {
			KC_MSG("vld end = %x, sel = %x", i, i + 1 + rdvld_offset);
			i = i + 1 + rdvld_offset;
			byte0_vld = modified_bits_by_value(byte0_vld, i, 20, 16);
			mmio_wr32(0x0B14 + PHYD_BASE_ADDR, byte0_vld);
			byte1_vld = modified_bits_by_value(byte1_vld, i, 20, 16);
			mmio_wr32(0x0B44 + PHYD_BASE_ADDR, byte0_vld);
			break;
		}
	}
}

void ddr_sys_suspend(void)
{
	uartlog("cvx16_ddr_sub_suspend\n");
	// ddr_debug_wr32(0x3c);
	// ddr_debug_num_write();
	TJ_MSG("DDRC suspend start\n");

	cvx16_ddrc_suspend();
	TJ_MSG("DDRC suspend complete\n");

	// save phyd setting to sram
	cvx16_ddr_phyd_save(0x05026800);
	// cvx16_ddr_phya_pd
	cvx16_ddr_phya_pd();
	// virtual_pwr_off();
}

void ddr_sys_resume(void)
{
	uint8_t dram_cap_in_mbyte;
	// ddr_sub_resume1
	// cvx16_ddr_sub_resume1();
	// KC_MSG("ddr_sub_resume1\n");

	// pll_init
	pll_init();
	KC_MSG("pll_init_h finish\n");

	// ctrl_init
	ddrc_init();
	KC_MSG("2nd ctrl_init_h finish\n");

	// set axi QOS
	mmio_wr32(0x030001D8, 0x008899aa);
	mmio_wr32(0x030001DC, 0x00002244);

	// ddr_sub_resume2
	cvx16_ddr_sub_resume2();
	KC_MSG("ddr_sub_resume2\n");

	// pinmux
	//     cvx16_pinmux();
	//     KC_MSG("cvx16_pinmux finish\n");

	// ddr_sub_resume3
	cvx16_ddr_sub_resume3();
	KC_MSG("ddr_sub_resume3\n");

	// ctrl_init_h.ctrl_high_patch=1;
	//`uvm_send(ctrl_init_h);
	ctrl_init_high_patch();

	//    ctrl_init_detect_dram_size(&dram_cap_in_mbyte);
	//    KC_MSG("ctrl_init_detect_dram_size finish\n");

	// restory dram_cap_in_mbyte
	rddata = mmio_rd32(0x0208 + PHYD_BASE_ADDR);
	dram_cap_in_mbyte = rddata;

	ctrl_init_update_by_dram_size(dram_cap_in_mbyte);
	KC_MSG("ctrl_init_update_by_dram_size finish\n");

	KC_MSG("dram_cap_in_mbyte = %x\n", dram_cap_in_mbyte);
	cvx16_dram_cap_check(dram_cap_in_mbyte);
	KC_MSG("cvx16_dram_cap_check finish\n");

	// Write PCTRL.port_en = 1
	for (int i = 0; i < 4; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}

	//    ctrl_init_h.ctrl_high_patch=1;
	//    `uvm_send(ctrl_init_h);

	cvx16_clk_gating_enable();
}

void cvx16_ddr_sub_resume2(void)
{
	uartlog("%s\n", __func__);
	// ddr_debug_wr32(0x44);
	// ddr_debug_num_write();
	//  Program INIT0.skip_dram_init = 0b11
	rddata = mmio_rd32(cfg_base + 0xd0);
	rddata = modified_bits_by_value(rddata, 0x3, 31, 30);
	// rddata[31:30] = 0x3;
	mmio_wr32(cfg_base + 0xd0, rddata);
	// Program PWRCTL.selfref_sw = 0b1
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, 0x1, 5, 5);
	// rddata[5:5] = 0x1;
	mmio_wr32(cfg_base + 0x30, rddata);
	// Program DFIMISC.dfi_init_complete_en to 1b0
	rddata = mmio_rd32(cfg_base + 0x1b0);
	rddata = modified_bits_by_value(rddata, 0x0, 0, 0);
	// rddata[0:0] = 0x0;
	mmio_wr32(cfg_base + 0x1b0, rddata);
	// Remove the controller reset core_ddrc_rstn = 1b1 aresetn_n = 1b1
	rddata = mmio_rd32(0x0800a020);
	rddata = modified_bits_by_value(rddata, 0x0, 0, 0);
	// rddata[0] = 0;
	mmio_wr32(0x0800a020, rddata);
}

void cvx16_ddr_sub_resume3(void)
{
	uartlog("%s\n", __func__);
	// ddr_debug_wr32(0x45);
	// ddr_debug_num_write();
	// ddr_phyd_restore
	cvx16_ddr_phyd_restore(0x05026800);
	// setting_check
	cvx16_setting_check();
	KC_MSG("cvx16_setting_check  finish\n");

	// ddr_phy_power_on_seq1
	cvx16_ddr_phy_power_on_seq1();
	// ddr_phy_power_on_seq2
	cvx16_ddr_phy_power_on_seq2();
	// set_dfi_init_start
	cvx16_set_dfi_init_start();
	// set_dfi_init_complete
	cvx16_set_dfi_init_complete();
	// synp setting
	// deassert dfi_init_start, and enable the act on dfi_init_complete
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001b0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	// rddata[5] = 0b0;
	mmio_wr32(cfg_base + 0x000001b0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	KC_MSG("dfi_init_complete finish\n");

	// ddr_phy_power_on_seq3
	cvx16_ddr_phy_power_on_seq3();
	// Program SWCTL.sw_done = 0b0
	rddata = mmio_rd32(cfg_base + 0x320);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	// rddata[0:0] = 0x0;
	mmio_wr32(cfg_base + 0x320, rddata);
	// Program DFIMISC.dfi_init_complete_en to 1b1
	rddata = mmio_rd32(cfg_base + 0x1b0);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	// rddata[0:0] = 0x1;
	mmio_wr32(cfg_base + 0x1b0, rddata);
	// Program SWCTL.sw_done = 0b1
	rddata = mmio_rd32(cfg_base + 0x320);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	// rddata[0:0] = 0x1;
	mmio_wr32(cfg_base + 0x320, rddata);
	// Program PWRCTL.selfref_sw = 1b0
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	// rddata[5:5] = 0x0;
	mmio_wr32(cfg_base + 0x30, rddata);
	// Poll STAT.selfref_type = 2b00
	// do {
	//    rddata = mmio_rd32(cfg_base + 0x4);
	//} while (get_bits_from_value(rddata, 5, 4) != 0x0);
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		if (get_bits_from_value(rddata, 5, 4) == 0x0) {
			break;
		}
	}
	// Poll STAT.operating_mode for Normal Mode entry
	cvx16_polling_synp_normal_mode();
}

void cvx16_ddrc_suspend(void)
{
	uartlog("%s\n", __func__);
	// ddr_debug_wr32(0x40);
	// ddr_debug_num_write();
	//  Write 0 to PCTRL_n.port_en
	for (int i = 0; i < 4; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	// do {
	//    rddata = mmio_rd32(cfg_base+0x3fc);
	//} while (rddata != 0);
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		if (rddata == 0) {
			break;
		}
	}
	// Write 1 to PWRCTL.selfref_sw
	rddata = mmio_rd32(cfg_base + 0x30);
	// rddata[5] = 0x1;
	rddata = modified_bits_by_value(rddata, 1, 5, 5);
	mmio_wr32(cfg_base + 0x30, rddata);
// Poll STAT.selfref_type= 2b10
// Poll STAT.selfref_state = 0b10 (LPDDR4 only)
#ifndef LP4
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		if (get_bits_from_value(rddata, 5, 4) == 0x2) {
			break;
		}
	}
#else
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		if (get_bits_from_value(rddata, 9, 8) == 0x2) {
			break;
		}
	}
#endif
}

void cvx16_bist_wr_prbs_init(void)
{
	// reg [31:0] cmd[5:0];
	// reg [31:0] sram_st;
	// reg [31:0] sram_sp;
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	int i;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x23);
	ddr_debug_num_write();
	KC_MSG("bist_wr_prbs_init\n");

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	sram_st = 0;
	sram_sp = 511;
	// cmd queue
	//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
	//        repeat
	cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // W  1~17  prbs  repeat0
	cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // R  1~17  prbs  repeat0
	cmd[2] = 0; // NOP
	cmd[3] = 0; // NOP
	cmd[4] = 0; // NOP
	cmd[5] = 0; // NOP
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
#else
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
#endif
	uartlog("bist_wr_prbs_init done\n");
	KC_MSG("bist_wr_prbs_init done\n");
}

void cvx16_bist_wr_sram_init(void)
{
	const int byte_per_page = 256;
	int axi_per_page = byte_per_page / 64;
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	uint32_t fmax;
	uint32_t fmin;
	int i;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x23);
	ddr_debug_num_write();
	KC_MSG("bist_wr_sram_init\n");

	// bist clock enable, axi_len 8
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x000C000C);
	sram_st = 0;
	fmax = 15;
	fmin = 5;
	sram_st = 0;
	sram_sp = 9 * (fmin + fmax) * (fmax - fmin + 1) / 2 / 4 + (fmax - fmin + 1); // 8*f/4 -1
	KC_MSG("sram_sp = %x\n", sram_sp);

	// bist sso_period
	mmio_wr32(DDR_BIST_BASE + 0x24, (fmax << 8) + fmin);
	// cmd queue
	//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
	//        repeat
	cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (15 << 0); // W  1~17  sram  repeat0
	cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (15 << 0); // R  1~17  sram  repeat0
	//       NOP idle
	cmd[2] = 0; // NOP
	//       GOTO      addr_not_reset loop_cnt
	cmd[3] = (3 << 30) | (0 << 20) | (1 << 0); // GOTO
	cmd[4] = 0; // NOP
	cmd[5] = 0; // NOP
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step to 2KB
	mmio_wr32(DDR_BIST_BASE + 0x18, 2048 / axi_per_page / 16);
#else
	// TBD
#endif
	uartlog("bist_wr_sram_init done\n");
	KC_MSG("bist_wr_sram_init done\n");
}

void cvx16_bist_wrlvl_init(void)
{
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	int i;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x23);
	ddr_debug_num_write();
	KC_MSG_TR("%s\n", __func__);

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	sram_st = 0;
	// sram_sp = 511;
	sram_sp = 0;
	// cmd queue
	//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
	//        repeat
	cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // W  1~17  prbs  repeat0
	// cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
	//          (0 << 0); // R  1~17  prbs  repeat0
	cmd[1] = 0; // NOP
	cmd[2] = 0; // NOP
	cmd[3] = 0; // NOP
	cmd[4] = 0; // NOP
	cmd[5] = 0; // NOP
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
#else
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
#endif
	uartlog("bist_wr_prbs_init done\n");
	KC_MSG_TR("bist_wr_prbs_init done\n");
}

void cvx16_bist_rdglvl_init(void)
{
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	int i;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x231);
	ddr_debug_num_write();
	KC_MSG("%s\n", __func__);

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	sram_st = 0;
	// sram_sp = 511;
	sram_sp = 3;
	// cmd queue
	//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
	//        repeat
	// cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
	//          (0 << 0);  // W  1~17  prbs  repeat0
	cmd[0] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // R  1~17  prbs  repeat0
	// cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
	//          (0 << 0);  // R  1~17  prbs  repeat0
	cmd[1] = 0; // NOP
	cmd[2] = 0; // NOP
	cmd[3] = 0; // NOP
	cmd[4] = 0; // NOP
	cmd[5] = 0; // NOP
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
#else
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
#endif
	uartlog("%s done\n", __func__);
	KC_MSG("%s done\n", __func__);
}

void cvx16_bist_rdlvl_init(uint32_t mode)
{
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	uint32_t fmax;
	uint32_t fmin;
	// uint32_t sram_dq[1024];
	int i;

	// mode = 0x0  : MPR mode, DDR3 only.
	// mode = 0x1  : sram write/read continuous goto
	// mode = 0x2  : multi- bist write/read
	// mode = 0x12 : with Error enject,  multi- bist write/read
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x232);
	ddr_debug_num_write();
	KC_MSG("%s mode = %x\n", __func__, mode);

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	if (mode == 0x0) { // MPR mode
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 3;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		// cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		//          (0 << 0);  // W  1~17  prbs  repeat0
		cmd[0] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		// cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		//          (0 << 0);  // R  1~17  prbs  repeat0
		cmd[1] = 0; // NOP
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	} else if (mode == 0x1) { // sram write/read continuous goto
		fmax = 15;
		fmin = 5;
		sram_st = 0;
		sram_sp = 9 * (fmin + fmax) * (fmax - fmin + 1) / 2 / 4 + (fmax - fmin + 1); // 8*f/4 -1
		KC_MSG("sram_sp = %x\n", sram_sp);

		// bist sso_period
		mmio_wr32(DDR_BIST_BASE + 0x24, (fmax << 8) + fmin);
	#ifdef DDR_SIM
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0);  // W  1~17  sram  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0);  // R  1~17  sram  repeat0
		//       NOP idle
		cmd[2] = 1;
		//       GOTO      addr_not_reset loop_cnt
		cmd[3] = (3 << 30) | (0 << 20) | (1 << 0); //GOTO
		cmd[4] = 0;  // NOP
		cmd[5] = 0;  // NOP
	#else
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (511 << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (511 << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // R  1~17  prbs  repeat0
		cmd[2] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // W  1~17  sram  repeat0
		cmd[3] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // R  1~17  sram  repeat0
		//       GOTO      addr_not_reset loop_cnt
		cmd[4] = (3 << 30) | (0 << 20) | (1 << 0); // GOTO
		cmd[5] = 0; // NOP
	#endif
	} else if (mode == 0x2) { // multi- bist write/read
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 7;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	} else if (mode == 0x12) { // with Error enject,  multi- bist write/read
		//----for Error enject simulation only
		rddata = mmio_rd32(0x0084 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x1d, 4, 0); // param_phyd_pirdlvl_trig_lvl_start
		rddata = modified_bits_by_value(rddata, 0x1f, 12, 8); // param_phyd_pirdlvl_trig_lvl_end
		mmio_wr32(0x0084 + PHYD_BASE_ADDR, rddata);
		//----for Error enject simulation only
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 7;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	} else if (mode == 0x10) { // with Error enject,  multi- bist write/read
		//----for Error enject simulation only
		rddata = mmio_rd32(0x0084 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 4, 0); // param_phyd_pirdlvl_trig_lvl_start
		rddata = modified_bits_by_value(rddata, 3, 12, 8); // param_phyd_pirdlvl_trig_lvl_end
		mmio_wr32(0x0084 + PHYD_BASE_ADDR, rddata);
		//----for Error enject simulation only
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 3;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		// cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		//          (0 << 0);  // W  1~17  prbs  repeat0
		cmd[0] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		// cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		//          (0 << 0);  // R  1~17  prbs  repeat0
		cmd[1] = 0; //NOP
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	}
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
#else
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
#endif
	uartlog("%s\n", __func__);
	KC_MSG("%s\n", __func__);
}

void cvx16_bist_wdqlvl_init(uint32_t mode)
{
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	uint32_t fmax;
	uint32_t fmin;
	// uint32_t wdqlvl_vref_start; //unused
	// uint32_t wdqlvl_vref_end; //unused
	// uint32_t wdqlvl_vref_step; //unused
	int i;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x233);
	ddr_debug_num_write();
	KC_MSG("%s mode = %x\n", __func__, mode);

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	if (mode == 0x0) { // phyd pattern
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 3;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	} else if (mode == 0x1) { // bist write/read
		sram_st = 0;
		fmax = 15;
		fmin = 5;
		sram_st = 0;
		sram_sp = 9 * (fmin + fmax) * (fmax - fmin + 1) / 2 / 4 + (fmax - fmin + 1); // 8*f/4 -1
		KC_MSG("sram_sp = %x\n", sram_sp);

		// bist sso_period
		mmio_wr32(DDR_BIST_BASE + 0x24, (fmax << 8) + fmin);
	#ifdef DDR_SIM
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0);  // W  1~17  sram  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0);  // R  1~17  sram  repeat0
		//       NOP idle
		cmd[2] = 0;  // NOP
		//       GOTO      addr_not_reset loop_cnt
		cmd[3] = (3 << 30) | (0 << 20) | (1 << 0); //GOTO
		cmd[4] = 0;  // NOP
		cmd[5] = 0;  // NOP
	#else
		#ifdef ENABLE_FASTBOOT0
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (511 << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (511 << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  sram  repeat0
		cmd[3] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  sram  repeat0
		//       GOTO      addr_not_reset loop_cnt
		cmd[4] = 0; // GOTO
		cmd[5] = 0; // NOP
		#else
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (511 << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (511 << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // R  1~17  prbs  repeat0
		cmd[2] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // W  1~17  sram  repeat0
		cmd[3] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (6 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (2 << 0); // R  1~17  sram  repeat0
		//       GOTO      addr_not_reset loop_cnt
		cmd[4] = (3 << 30) | (0 << 20) | (1 << 0); // GOTO
		cmd[5] = 0; // NOP
		#endif
	#endif
	} else if (mode == 0x11) { // bist write/read
		//----for Error enject simulation only
		rddata = 0x00000000;
		rddata = modified_bits_by_value(rddata, 0x1, 6, 0); // param_phyd_dfi_wdqlvl_vref_start
		rddata = modified_bits_by_value(rddata, 0x3, 14, 8); // param_phyd_dfi_wdqlvl_vref_end
		rddata = modified_bits_by_value(rddata, 0x1, 19, 16); // param_phyd_dfi_wdqlvl_vref_step
		mmio_wr32(0x0190 + PHYD_BASE_ADDR, rddata);
		//----for Error enject simulation only
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 3;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	} else if (mode == 0x12) { // bist write/read
		//----for Error enject simulation only
		rddata = 0x00000000;
		rddata = modified_bits_by_value(rddata, 0x1d, 6, 0); // param_phyd_dfi_wdqlvl_vref_start
		rddata = modified_bits_by_value(rddata, 0x1f, 14, 8); // param_phyd_dfi_wdqlvl_vref_end
		rddata = modified_bits_by_value(rddata, 0x1, 19, 16); // param_phyd_dfi_wdqlvl_vref_step
		mmio_wr32(0x0190 + PHYD_BASE_ADDR, rddata);
		//----for Error enject simulation only
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 3;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	} else {
		sram_st = 0;
		// sram_sp = 511;
		sram_sp = 3;
		// cmd queue
		//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
		//        repeat
		cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // W  1~17  prbs  repeat0
		cmd[1] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (5 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
			 (0 << 0); // R  1~17  prbs  repeat0
		cmd[2] = 0; // NOP
		cmd[3] = 0; // NOP
		cmd[4] = 0; // NOP
		cmd[5] = 0; // NOP
	}
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
#else
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
#endif
	uartlog("%s done\n", __func__);
	KC_MSG("%s done\n", __func__);
}

void cvx16_bist_wdmlvl_init(void)
{
	uint32_t cmd[6];
	uint32_t sram_st;
	uint32_t sram_sp;
	uint32_t fmax;
	uint32_t fmin;
	int i;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x233);
	ddr_debug_num_write();
	KC_MSG("%s\n", __func__);

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	sram_st = 0;
	fmax = 15;
	fmin = 5;
	sram_st = 0;
	sram_sp = 9 * (fmin + fmax) * (fmax - fmin + 1) / 2 / 4 + (fmax - fmin + 1); // 8*f/4 -1
	KC_MSG("sram_sp = %x\n", sram_sp);

	// bist sso_period
	mmio_wr32(DDR_BIST_BASE + 0x24, (fmax << 8) + fmin);
	// cmd queue
	//        op_code         start              stop        pattern    dq_inv     dm_inv    dq_rotate
	//        repeat
	cmd[0] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (3 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // bist write pat 3 = 0x0f
	cmd[1] = (1 << 30) | (sram_st << 21) | (sram_sp << 12) | (7 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // bist write pat 7 = sso_8x1 DM toggle verison
	cmd[2] = (2 << 30) | (sram_st << 21) | (sram_sp << 12) | (7 << 9) | (0 << 8) | (0 << 7) | (0 << 4) |
		 (0 << 0); // bist read pat 7
	cmd[3] = 0; // NOP
	cmd[4] = 0; // NOP
	cmd[5] = 0; // NOP
	// write cmd queue
	for (i = 0; i < 6; i = i + 1) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	};
	// specified DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x000fffff);
#ifdef X16_MODE
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
#else
	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
#endif
	uartlog("%s done\n", __func__);
	KC_MSG("%s done\n", __func__);
}

void cvx16_bist_start_check(uint32_t *bist_result, uint64_t *err_data_odd, uint64_t *err_data_even)
{
	uint64_t err_data_even_l;
	uint64_t err_data_even_h;
	uint64_t err_data_odd_l;
	uint64_t err_data_odd_h;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x25);
	ddr_debug_num_write();
#ifdef X16_MODE
	// bist enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00030003);
	uartlog("bist start\n");
	KC_MSG("bist start\n");

#else
	// bist enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);
	uartlog("bist start\n");
	KC_MSG("bist start\n");

#endif
	// polling bist done
	//    while (get_bits_from_value(2)  == 0x0 = mmio_rd32(DDR_BIST_BASE + 0x80),2);
	while (1) {
		rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
		if (get_bits_from_value(rddata, 2, 2) == 1) {
			break;
		}
	}
	uartlog("bist done\n");
	KC_MSG("Read bist done %x ...\n", rddata);

	if (get_bits_from_value(rddata, 3, 3) == 1) {
		// opdelay(10);
		*bist_result = 0;
		uartlog("bist fail\n");
		uartlog("err data\n");
		// read err_data
		err_data_odd_l = mmio_rd32(DDR_BIST_BASE + 0x88);
		err_data_odd_h = mmio_rd32(DDR_BIST_BASE + 0x8c);
		err_data_even_l = mmio_rd32(DDR_BIST_BASE + 0x90);
		err_data_even_h = mmio_rd32(DDR_BIST_BASE + 0x94);
		*err_data_odd = err_data_odd_h << 32 | err_data_odd_l;
		*err_data_even = err_data_even_h << 32 | err_data_even_l;
	} else {
		// opdelay(10);
		*bist_result = 1;
		uartlog("bist pass\n");
		*err_data_odd = 0;
		*err_data_even = 0;
	}
	// bist disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00050000);
}

void cvx16_bist_tx_shift_delay(uint32_t shift_delay)
{
	uint32_t shift_tmp;
	uint32_t delay_tmp;
	// uint32_t oenz_shift_tmp; //unused
	uint32_t oenz_lead;
	uint32_t tdfi_phy_wrdata;
	// uint32_t dlie_sum_great; //unused
	uint32_t dlie_sub;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x26);
	ddr_debug_num_write();
	shift_tmp = get_bits_from_value(shift_delay, 13, 8);
	delay_tmp = get_bits_from_value(shift_delay, 6, 0);
	// tdfi_phy_wrdata
	rddata = mmio_rd32(0x00BC + PHYD_BASE_ADDR);
	tdfi_phy_wrdata = get_bits_from_value(rddata, 2, 0);
	oenz_lead = get_bits_from_value(rddata, 6, 3);
	KC_MSG("shift_delay = %x, oenz_lead = %x, tdfi_phy_wrdata = %x\n", shift_delay, oenz_lead,
	       tdfi_phy_wrdata);

	if ((shift_tmp + (tdfi_phy_wrdata << 1)) > oenz_lead) {
		dlie_sub = shift_tmp + (tdfi_phy_wrdata << 1) - oenz_lead;
	} else {
		dlie_sub = 0;
	}
	KC_MSG("dlie_sub = %x\n", dlie_sub);

	for (int i = 0; i < 2; i = i + 1) {
		rddata = (shift_tmp << 24) + (delay_tmp << 16) + (shift_tmp << 8) + delay_tmp;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		rddata = mmio_rd32(0x0A18 + i * 0x40 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, dlie_sub, 29, 24);
		mmio_wr32(0x0A18 + i * 0x40 + PHYD_BASE_ADDR, rddata);
	}
	cvx16_dll_sw_clr();
	KC_MSG("%s Fisish\n", __func__);
}

void cvx16_bist_rx_delay(uint32_t delay)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x27);
	ddr_debug_num_write();
	for (int i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B08 + i * 0x30 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, delay, 15, 8);
		rddata = modified_bits_by_value(rddata, delay, 23, 16);
		mmio_wr32(0x0B08 + i * 0x30 + PHYD_BASE_ADDR, rddata);
	}
	cvx16_dll_sw_clr();
	KC_MSG("%s Fisish\n", __func__);
}

void cvx16_bist_rx_deskew_delay(uint32_t delay)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x28);
	ddr_debug_num_write();
	for (int i = 0; i < 2; i = i + 1) {
		rddata = (delay << 24) + (delay << 16) + (delay << 8) + delay;
		mmio_wr32(0x0B00 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0B04 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		rddata = mmio_rd32(0x0B08 + i * 0x30 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, delay, 6, 0);
		mmio_wr32(0x0B08 + i * 0x30 + PHYD_BASE_ADDR, rddata);
	}
	cvx16_dll_sw_clr();
	KC_MSG("%s Fisish\n", __func__);
}

void cvx16_ca_shift_delay(uint32_t shift_delay)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x57);
	ddr_debug_num_write();
	rddata = (shift_delay << 16) + shift_delay;
	mmio_wr32(0x0900 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0904  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0908  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x090C  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0910  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0914  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0918  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x091C  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0920  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0924  + PHYD_BASE_ADDR,  rddata);
	// mmio_wr32(0x0928  + PHYD_BASE_ADDR,  rddata);
	mmio_wr32(0x092C + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0930  + PHYD_BASE_ADDR,  rddata);//CKE
	// mmio_wr32(0x0934  + PHYD_BASE_ADDR,  rddata);//cs
	// mmio_wr32(0x0938  + PHYD_BASE_ADDR,  rddata);//reset_n
	cvx16_dll_sw_clr();
	KC_MSG("%s Fisish\n", __func__);
}

void cvx16_cs_shift_delay(uint32_t shift_delay)
{
	uartlog("cvx16_cs_shift_delay\n");
	ddr_debug_wr32(0x57);
	ddr_debug_num_write();
	rddata = (shift_delay << 16) + shift_delay;
	// mmio_wr32(0x0900 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0904 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0908 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x090C + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0910 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0914 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0918 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x091C + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0920 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0924 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0928 + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x092C + PHYD_BASE_ADDR, rddata);
	// mmio_wr32(0x0930 + PHYD_BASE_ADDR, rddata); //cke
	mmio_wr32(0x0934 + PHYD_BASE_ADDR, rddata); //cs
	// mmio_wr32(0x0938 + PHYD_BASE_ADDR, rddata); //reset_n
	cvx16_dll_sw_clr();
	KC_MSG("cvx16_cs_shift_delay Fisish\n");
}

void cvx16_synp_mrw(uint32_t addr, uint32_t data)
{
	uint32_t init_dis_auto_zq;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x21);
	ddr_debug_num_write();
	// ZQCTL0.dis_auto_zq to 1.
	rddata = mmio_rd32(cfg_base + 0x180);
	// if (rddata[31] == 0b0) {
	if (get_bits_from_value(rddata, 31, 31) == 0) {
		init_dis_auto_zq = 0;
		// rddata[31] = 1;
		rddata = modified_bits_by_value(rddata, 1, 31, 31);
		mmio_wr32(cfg_base + 0x180, rddata);
		uartlog("non-lp4 Write ZQCTL0.dis_auto_zq to 1\n");
		// opdelay(256);
		uartlog("Wait tzqcs = 128 cycles\n");
	} else {
		init_dis_auto_zq = 1;
	}
	// Poll MRSTAT.mr_wr_busy until it is 0
	uartlog("Poll MRSTAT.mr_wr_busy until it is 0\n");
	rddata = 0;
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x18);
		if (get_bits_from_value(rddata, 0, 0) == 0) {
			break;
		}
	}
	uartlog("non-lp4 Poll MRSTAT.mr_wr_busy finish\n");
	// Write the MRCTRL0.mr_type, MRCTRL0.mr_addr, MRCTRL0.mr_rank and (for MRWs) MRCTRL1.mr_data
	// rddata[31:0]  = 0;
	// rddata[0]     = 0;       // mr_type  0:write   1:read
	// rddata[5:4]   = 1;       // mr_rank
	// rddata[15:12] = addr;    // mr_addr
	rddata = modified_bits_by_value(rddata, 0, 31, 0);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	rddata = modified_bits_by_value(rddata, 1, 5, 4);
	rddata = modified_bits_by_value(rddata, addr, 15, 12);
	mmio_wr32(cfg_base + 0x10, rddata);
	uartlog("non-lp4 Write the MRCTRL0\n");
	// rddata[31:0] = 0;
	// rddata[15:0] = data;     // mr_data
	rddata = modified_bits_by_value(rddata, 0, 31, 0);
	rddata = modified_bits_by_value(rddata, data, 31, 0);
	mmio_wr32(cfg_base + 0x14, rddata);
	uartlog("non-lp4 Write the MRCTRL1\n");
	// Write MRCTRL0.mr_wr to 1
	rddata = mmio_rd32(cfg_base + 0x10);
	// rddata[31] = 1;
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x10, rddata);
	uartlog("non-lp4 Write MRCTRL0.mr_wr to 1\n");
	if (init_dis_auto_zq == 0) {
		// ZQCTL0.dis_auto_zq to 0.
		rddata = mmio_rd32(cfg_base + 0x180);
		// rddata[31] = 0;
		rddata = modified_bits_by_value(rddata, 0, 31, 31);
		mmio_wr32(cfg_base + 0x180, rddata);
		uartlog("non-lp4 Write ZQCTL0.dis_auto_zq to 0\n");
	}
}

void cvx16_chg_pll_freq(void)
{
	uint32_t EN_PLL_SPEED_CHG;
	uint32_t CUR_PLL_SPEED;
	uint32_t NEXT_PLL_SPEED;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x04);
	ddr_debug_num_write();
	// Change PLL frequency
	// TOP_REG_RESETZ_DIV =0
	rddata = 0x00000000;
	mmio_wr32(0x04 + CV_DDR_PHYD_APB, rddata);
	// TOP_REG_RESETZ_DQS =0
	mmio_wr32(0x08 + CV_DDR_PHYD_APB, rddata);
	// TOP_REG_DDRPLL_MAS_RSTZ_DIV  =0
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 7, 7);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	uartlog("RSTZ_DIV=0\n");
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	EN_PLL_SPEED_CHG = get_bits_from_value(rddata, 0, 0);
	CUR_PLL_SPEED = get_bits_from_value(rddata, 5, 4);
	NEXT_PLL_SPEED = get_bits_from_value(rddata, 9, 8);
	KC_MSG("CUR_PLL_SPEED = %x, NEXT_PLL_SPEED = %x, EN_PLL_SPEED_CHG=%x\n", CUR_PLL_SPEED, NEXT_PLL_SPEED,
	       EN_PLL_SPEED_CHG);

	if (EN_PLL_SPEED_CHG) {
		if (NEXT_PLL_SPEED == 0) { // next clk_div40
			rddata = modified_bits_by_value(rddata, NEXT_PLL_SPEED, 5, 4);
			rddata = modified_bits_by_value(rddata, CUR_PLL_SPEED, 9, 8);
			mmio_wr32(0x4c + CV_DDR_PHYD_APB, rddata);
			cvx16_clk_div40();
			uartlog("clk_div40\n");
			KC_MSG("clk_div40\n");
		} else {
			if (NEXT_PLL_SPEED == 0x2) { // next clk normal
				rddata = modified_bits_by_value(rddata, NEXT_PLL_SPEED, 5, 4);
				rddata = modified_bits_by_value(rddata, CUR_PLL_SPEED, 9, 8);
				mmio_wr32(0x4c + CV_DDR_PHYD_APB, rddata);
				cvx16_clk_normal();
				uartlog("clk_normal\n");
				KC_MSG("clk_normal\n");
			} else {
				if (NEXT_PLL_SPEED == 0x1) { // next clk normal div_2
					rddata = modified_bits_by_value(rddata, NEXT_PLL_SPEED, 5, 4);
					rddata = modified_bits_by_value(rddata, CUR_PLL_SPEED, 9, 8);
					mmio_wr32(0x4c + CV_DDR_PHYD_APB, rddata);
					cvx16_clk_div2();
					uartlog("clk_div2\n");
					KC_MSG("clk_div2\n");
				}
			}
		}
		//         opdelay(100000);  //  1000ns
	}
	// TOP_REG_RESETZ_DIV  =1
	rddata = 0x00000001;
	mmio_wr32(0x04 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	// rddata[7]   = 1;    //TOP_REG_DDRPLL_MAS_RSTZ_DIV
	rddata = modified_bits_by_value(rddata, 1, 7, 7);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	uartlog("RSTZ_DIV=1\n");
	// rddata[0]   = 1;    //TOP_REG_RESETZ_DQS
	rddata = 0x00000001;
	mmio_wr32(0x08 + CV_DDR_PHYD_APB, rddata);
	uartlog("TOP_REG_RESETZ_DQS\n");
	KC_MSG("Wait for DRRPLL_SLV_LOCK=1...\n");

#ifdef REAL_LOCK
	rddata = modified_bits_by_value(rddata, 0, 15, 15);
	while (get_bits_from_value(rddata, 15, 15) == 0) {
		rddata = mmio_rd32(0x10 + CV_DDR_PHYD_APB);
		KC_MSG("REAL_LOCK.\n");

		opdelay(200);
	}
#else
	KC_MSG("check PLL lock...  pll init\n");

#endif
	//} Change PLL frequency
}

void cvx16_dll_cal(void)
{
	uint32_t EN_PLL_SPEED_CHG;
	uint32_t CUR_PLL_SPEED;
	uint32_t NEXT_PLL_SPEED;

	KC_MSG("Do DLLCAL ...\n");

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x2b);
	ddr_debug_num_write();
	// TOP_REG_EN_PLL_SPEED_CHG
	//     <= #RD (~pwstrb_mask[0] & TOP_REG_EN_PLL_SPEED_CHG) |  pwstrb_mask_pwdata[0];
	// TOP_REG_CUR_PLL_SPEED   [1:0]
	//     <= #RD (~pwstrb_mask[5:4] & TOP_REG_CUR_PLL_SPEED[1:0]) |  pwstrb_mask_pwdata[5:4];
	// TOP_REG_NEXT_PLL_SPEED  [1:0]
	//     <= #RD (~pwstrb_mask[9:8] & TOP_REG_NEXT_PLL_SPEED[1:0]) |  pwstrb_mask_pwdata[9:8];
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	EN_PLL_SPEED_CHG = get_bits_from_value(rddata, 0, 0);
	CUR_PLL_SPEED = get_bits_from_value(rddata, 5, 4);
	NEXT_PLL_SPEED = get_bits_from_value(rddata, 9, 8);
	KC_MSG("CUR_PLL_SPEED = %x, NEXT_PLL_SPEED = %x, EN_PLL_SPEED_CHG=%x\n", CUR_PLL_SPEED, NEXT_PLL_SPEED,
	       EN_PLL_SPEED_CHG);

	if (CUR_PLL_SPEED != 0) { // only do calibration and update when high speed
		// param_phyd_dll_rx_start_cal <= int_regin[1];
		// param_phyd_dll_tx_start_cal <= int_regin[17];
		rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 1, 1);
		rddata = modified_bits_by_value(rddata, 0, 17, 17);
		mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
		// param_phyd_dll_rx_start_cal <= int_regin[1];
		// param_phyd_dll_tx_start_cal <= int_regin[17];
		rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 1, 1);
		rddata = modified_bits_by_value(rddata, 1, 17, 17);
		mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
		rddata = 0x00000000;
		while (get_bits_from_value(rddata, 16, 16) == 0) {
			rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
		}
		KC_MSG("DLL lock !\n");

		uartlog("DLL lock\n");
		// opdelay(1000);
		uartlog("Do DLLUPD\n");
		// cvx16_dll_cal_status();
	} else { // stop calibration and update when low speed
		// param_phyd_dll_rx_start_cal <= int_regin[1];
		// param_phyd_dll_tx_start_cal <= int_regin[17];
		rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 1, 1);
		rddata = modified_bits_by_value(rddata, 0, 17, 17);
		mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	}
	KC_MSG("Do DLLCAL Finish\n");

	uartlog("Do DLLCAL Finish\n");
}

void cvx16_dll_cal_phyd_hw(void)
{
	KC_MSG("Do DLLCAL phyd_hw ...\n");

	uartlog("cvx16_dll_cal phyd_hw\n");
	ddr_debug_wr32(0x2b);
	ddr_debug_num_write();
	KC_MSG("DLLCAL HW mode cntr_mode 0\n");

	// param_phyd_dll_rx_sw_mode    [0]
	// param_phyd_dll_rx_start_cal  [1]
	// param_phyd_dll_rx_cntr_mode  [2]
	// param_phyd_dll_rx_hwrst_time [3]
	// param_phyd_dll_tx_sw_mode    [16]
	// param_phyd_dll_tx_start_cal  [17]
	// param_phyd_dll_tx_cntr_mode  [18]
	// param_phyd_dll_tx_hwrst_time [19]
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b0000, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b0000, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b0010, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b0010, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	while (1) {
		rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
		if ((get_bits_from_value(rddata, 16, 16) & get_bits_from_value(rddata, 0, 0)) != 0) {
			break;
		}
	}
	KC_MSG("DLL lock !\n");

	uartlog("DLL lock\n");
	// opdelay(1000);
	uartlog("Do DLLUPD\n");
	// cvx16_dll_cal_status();
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 17, 17);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	KC_MSG("Do DLLCAL cntr_mode 0 Finish\n");

	uartlog("Do DLLCAL cntr_mode 0 Finish\n");
	// opdelay(1000);
	KC_MSG("DLLCAL HW mode cntr_mode 1\n");

	// param_phyd_dll_rx_sw_mode    [0]
	// param_phyd_dll_rx_start_cal  [1]
	// param_phyd_dll_rx_cntr_mode  [2]
	// param_phyd_dll_rx_hwrst_time [3]
	// param_phyd_dll_tx_sw_mode    [16]
	// param_phyd_dll_tx_start_cal  [17]
	// param_phyd_dll_tx_cntr_mode  [18]
	// param_phyd_dll_tx_hwrst_time [19]
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b0000, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b0000, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b0110, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b0110, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	while (1) {
		rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
		if ((get_bits_from_value(rddata, 16, 16) & get_bits_from_value(rddata, 0, 0)) != 0) {
			break;
		}
	}
	KC_MSG("DLL lock !\n");

	uartlog("DLL lock\n");
	// opdelay(1000);
	uartlog("Do DLLUPD\n");
	// cvx16_dll_cal_status();
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 17, 17);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	KC_MSG("Do DLLCAL cntr_mode 1 Finish\n");

	uartlog("Do DLLCAL cntr_mode 1 Finish\n");
	// opdelay(1000);
	// param_phyd_dll_rx_sw_mode    [0]
	// param_phyd_dll_rx_start_cal  [1]
	// param_phyd_dll_rx_cntr_mode  [2]
	// param_phyd_dll_rx_hwrst_time [3]
	// param_phyd_dll_tx_sw_mode    [16]
	// param_phyd_dll_tx_start_cal  [17]
	// param_phyd_dll_tx_cntr_mode  [18]
	// param_phyd_dll_tx_hwrst_time [19]
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b0000, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b0000, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b1110, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b1110, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	while (1) {
		rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
		if ((get_bits_from_value(rddata, 16, 16) & get_bits_from_value(rddata, 0, 0)) != 0) {
			break;
		}
	}
	KC_MSG("DLL lock !\n");

	uartlog("DLL lock\n");
	// opdelay(1000);
	uartlog("Do DLLUPD\n");
	// cvx16_dll_cal_status();
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 17, 17);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	uartlog("Do DLLCAL cntr_mode hwrst_time 1 Finish\n");
}

void cvx16_dll_cal_phya_enautok(void)
{
	KC_MSG("Do DLLCAL enautok ...\n");

	uartlog("cvx16_dll_cal enautok\n");
	ddr_debug_wr32(0x2b);
	ddr_debug_num_write();
	KC_MSG("DLLCAL enautok\n");

	// param_phyd_dll_rx_sw_mode    [0]
	// param_phyd_dll_rx_start_cal  [1]
	// param_phyd_dll_rx_cntr_mode  [2]
	// param_phyd_dll_rx_hwrst_time [3]
	// param_phyd_dll_tx_sw_mode    [16]
	// param_phyd_dll_tx_start_cal  [17]
	// param_phyd_dll_tx_cntr_mode  [18]
	// param_phyd_dll_tx_hwrst_time [19]
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0b0000, 3, 0);
	rddata = modified_bits_by_value(rddata, 0b0000, 19, 16);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_ddrdll_enautok [0]
	// param_phya_reg_tx_ddrdll_enautok [16]
	rddata = mmio_rd32(0x0140 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	rddata = modified_bits_by_value(rddata, 1, 16, 16);
	mmio_wr32(0x0140 + PHYD_BASE_ADDR, rddata);
	while (1) {
		rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
		if ((get_bits_from_value(rddata, 16, 16) & get_bits_from_value(rddata, 0, 0)) != 0) {
			break;
		}
	}
	KC_MSG("DLL lock !\n");

	uartlog("DLL lock\n");
	// opdelay(1000);
	uartlog("Do DLLUPD\n");
	// cvx16_dll_cal_status();
	// param_phyd_dll_rx_start_cal <= int_regin[1];
	// param_phyd_dll_tx_start_cal <= int_regin[17];
	rddata = mmio_rd32(0x0040 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 17, 17);
	mmio_wr32(0x0040 + PHYD_BASE_ADDR, rddata);
	KC_MSG("Do DLLCAL enautok\n");

	uartlog("Do DLLCAL enautok Finish\n");
}

void cvx16_ddr_zqcal_isr8(void)
{
	uint32_t KP40_GOLDEN;
	uint32_t KN40_GOLDEN;
	uint32_t dram_class;
	// uint32_t wr_odt_en; //unused
	// uint32_t rtt_wr; //unused
	int i;
	// VDDQ_TXr       = 0.6;
	//------------------------------
	//  Init setting
	//------------------------------
	// ZQ clock on
	rddata = mmio_rd32(0x44 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 12, 12); // TOP_REG_CG_EN_ZQ
	mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
	// param_phyd_zqcal_hw_mode       <= `PI_SD int_regin[24];
	// sw mode
	// param_phyd_zqcal_hw_mode = 0
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_zqcal_done  <= `PI_SD int_regin[3];
	rddata = mmio_rd32(0x0158 + PHYD_BASE_ADDR);
	KC_MSG("Check ZQ Calibration param_phya_reg_zqcal_done = %x\n", get_bits_from_value(rddata, 14, 14));

	if (get_bits_from_value(rddata, 0, 0) == 0) { // initial ZQCAL
		rddata = mmio_rd32(0x0050 + PHYD_BASE_ADDR);
		// dram_class = rddata[11:8];  //DDR2:0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
		dram_class = get_bits_from_value(
			rddata, 3, 0); // DDR2:0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
		KC_MSG("dram_class = %x...\n", dram_class);

		KC_MSG("DDR2:0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011\n");

		//    ->ydh_zq_event;
		// a.
		// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
		rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 0, 31, 31);
		mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("TOP_REG_TX_ZQ_PD = 0...\n");

		// param_phya_reg_sel_zq_high_swing  <= `PI_SD int_regin[2];
		rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 2, 2);
		mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_sel_lpddr4_pmos_ph <= `PI_SD int_regin[2];
		rddata = mmio_rd32(0x0400 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 5, 5);
		mmio_wr32(0x0400 + PHYD_BASE_ADDR, rddata);
		//    ->ydh_zq_event;
		// b.
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 4, 0);
		rddata = modified_bits_by_value(rddata, 0x10, 12, 8);
		rddata = modified_bits_by_value(rddata, 0x10, 20, 16);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_zq_drvn[5:0] <= `PI_SD int_regin[20:16];
		// param_phya_reg_tx_zq_drvp[5:0] <= `PI_SD int_regin[28:24];
		rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x9, 20, 16);
		rddata = modified_bits_by_value(rddata, 0x9, 28, 24);
		mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
		//    ->ydh_zq_event;
		// c.
		// param_phya_reg_vref_pd  <= `PI_SD int_regin[9];
		//     REGREAD(4 + PHY_BASE_ADDR, rddata);
		//     rddata[9] = 0;
		//     REGWR  (4 + PHY_BASE_ADDR, rddata, 0);
		// param_phya_reg_vref_sel[4:0] <= `PI_SD int_regin[14:10];
		//     REGREAD(4 + PHY_BASE_ADDR, rddata);
		//     rddata[14:10] = 0b01111;
		//     REGWR  (4 + PHY_BASE_ADDR, rddata, 0);
		//    ->ydh_zq_event;
		// d.
		// param_phya_reg_tx_zq_en_test_aux <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_en_test_mux  <= `PI_SD int_regin[1];
		rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 1, 0);
		mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
		// TOP_REG_TX_SEL_GPIO
		//      <= #RD (~pwstrb_mask[7] & TOP_REG_TX_SEL_GPIO) |  pwstrb_mask_pwdata[7];
		// TOP_REG_TX_GPIO_OENZ
		//      <= #RD (~pwstrb_mask[6] & TOP_REG_TX_GPIO_OENZ) |  pwstrb_mask_pwdata[6];
		rddata = mmio_rd32(0x1c + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 1, 7, 6);
		mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
		//------------------------------
		// CMP offset Cal.
		//------------------------------
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x3, 4, 0);
		rddata = modified_bits_by_value(rddata, 0x10, 12, 8);
		rddata = modified_bits_by_value(rddata, 0x10, 20, 16);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		for (i = 0; i < 32; i = i + 1) {
			// opdelay(128);
			rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 24, 24)) { // param_phya_to_reg_zq_cmp_out
				rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 12, 8) < 0x1f) { // param_phya_reg_tx_zq_cmp_offset
					rddata = modified_bits_by_value(
						rddata, (get_bits_from_value(rddata, 12, 8) + 1), 12, 8);
					mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
				} else {
					KC_MSG("ZQ Calibration overflow - param_phya_reg_tx_zq_cmp_offset[4:0]: %x\n",
					       get_bits_from_value(rddata, 12, 8));
				}
			} else {
				rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 12, 8) > 0) {
					rddata = modified_bits_by_value(
						rddata, (get_bits_from_value(rddata, 12, 8) - 1), 12, 8);
					mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
				} else {
					KC_MSG("ZQ Calibration underflow - param_phya_reg_tx_zq_cmp_offset[4:0]: %x\n",
					       get_bits_from_value(rddata, 12, 8));
				}
			}
		}
		//------------------------------
		// ZQ PL
		//------------------------------
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x9, 4, 0);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		////param_phya_reg_tx_zq_drvn
		rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x9, 20, 16);
		mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
		for (i = 0; i < 32; i = i + 1) {
			//        repeat(128) @(posedge clk);
			// opdelay(128);
			rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 24, 24)) { // param_phya_to_reg_zq_cmp_out
				rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 20, 16) < 0x1f) {
					rddata = modified_bits_by_value(
						rddata, (get_bits_from_value(rddata, 20, 16) + 1), 20, 16);
					mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
				} else {
					KC_MSG("ZQ Calibration overflow - param_phya_reg_tx_zq_drvn[4:0]: %x\n",
					       get_bits_from_value(rddata, 20, 16));
				}
			} else {
				rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 20, 16) > 0) {
					rddata = modified_bits_by_value(
						rddata, (get_bits_from_value(rddata, 20, 16) - 1), 20, 16);
					mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
				} else {
					KC_MSG("ZQ Calibration underflow - param_phya_reg_tx_zq_drvn[4:0]: %x\n",
					       get_bits_from_value(rddata, 20, 16));
				}
			}
		}
		KN40_GOLDEN = get_bits_from_value(rddata, 20, 16);
		//------------------------------
		// ZQ PH
		//------------------------------
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x11, 4, 0);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		////param_phya_reg_tx_zq_drvp
		rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x9, 28, 24);
		mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
		for (i = 0; i < 32; i = i + 1) {
			// opdelay(128);
			rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 24, 24)) { // param_phya_to_reg_zq_cmp_out
				rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 28, 24) > 0) {
					rddata = modified_bits_by_value(
						rddata, (get_bits_from_value(rddata, 28, 24) - 1), 28, 24);
					mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
				} else {
					KC_MSG("ZQ Calibration underflow - param_phya_reg_tx_zq_drvp[4:0]: %x\n",
					       get_bits_from_value(rddata, 28, 24));
				}
			} else {
				rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 28, 24) < 0x1f) {
					rddata = modified_bits_by_value(
						rddata, (get_bits_from_value(rddata, 28, 24) + 1), 28, 24);
					mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
				} else {
				}
			}
		}
		KP40_GOLDEN = get_bits_from_value(rddata, 28, 24);
		//------------------------------
		// ZQ Complete
		//------------------------------
		uartlog("hw_done\n");
		// param_phya_reg_zqcal_done  <= `PI_SD int_regin[0];
		rddata = 0x00000001;
		mmio_wr32(0x0158 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 4, 0);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
		rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 1, 31, 31);
		mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("TOP_REG_TX_ZQ_PD = 1...\n");

		// ZQ clock off
		rddata = mmio_rd32(0x44 + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 0, 12, 12); // TOP_REG_CG_EN_ZQ
		mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("ZQ Complete ...\n");

		cvx16_zqcal_status();
	} //} initial ZQCAL
	else {
		KC_MSG("Not need ZQCAL\n");
	}
}

void cvx16_ddr_zqcal_hw_isr8(uint32_t hw_mode)
{
	uint32_t dram_class;
	// uint32_t wr_odt_en; //unused
	// uint32_t rtt_wr; //unused
	// int i; //unused

	uartlog("ddr_zqcal_hw_isr8\n");
	ddr_debug_wr32(0x2c);
	ddr_debug_num_write();
	// VDDQ_TXr        = 0.6;
	//------------------------------
	//  Init setting
	//------------------------------
	// ZQ clock on
	rddata = mmio_rd32(0x44 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 12, 12); // TOP_REG_CG_EN_ZQ
	mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
	// param_phyd_zqcal_hw_mode =1
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, hw_mode, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_zqcal_done  <= `PI_SD int_regin[3];
	rddata = mmio_rd32(0x0158 + PHYD_BASE_ADDR);
	KC_MSG("Check ZQ Calibration param_phya_reg_zqcal_done = %x\n", get_bits_from_value(rddata, 0, 0));

	if (get_bits_from_value(rddata, 0, 0) == 0) { // initial ZQCAL
		rddata = mmio_rd32(0x0050 + PHYD_BASE_ADDR);
		// dram_class = rddata[11:8];  //DDR2:0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
		dram_class = get_bits_from_value(
			rddata, 3, 0); // DDR2:0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
		KC_MSG("dram_class = %x...\n", dram_class);

		KC_MSG("DDR2:0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011\n");

		//    ->ydh_zq_event;
		// a.
		// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
		rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 0, 31, 31);
		mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("TOP_REG_TX_ZQ_PD = 0...\n");

		// param_phya_reg_sel_zq_high_swing  <= `PI_SD int_regin[2];
		rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 2, 2);
		mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_sel_lpddr4_pmos_ph <= `PI_SD int_regin[2];
		rddata = mmio_rd32(0x0400 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 5, 5);
		mmio_wr32(0x0400 + PHYD_BASE_ADDR, rddata);
		//    ->ydh_zq_event;
		// b.
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 4, 0);
		rddata = modified_bits_by_value(rddata, 0x10, 12, 8);
		rddata = modified_bits_by_value(rddata, 0x10, 20, 16);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_zq_drvn[5:0] <= `PI_SD int_regin[20:16];
		// param_phya_reg_tx_zq_drvp[5:0] <= `PI_SD int_regin[28:24];
		rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0x9, 20, 16);
		rddata = modified_bits_by_value(rddata, 0x9, 28, 24);
		mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
		//    ->ydh_zq_event;
		// c.
		// param_phya_reg_vref_pd  <= `PI_SD int_regin[9];
		//     REGREAD(4 + PHY_BASE_ADDR, rddata);
		//     rddata[9] = 0;
		//     REGWR  (4 + PHY_BASE_ADDR, rddata, 0);
		// param_phya_reg_vref_sel[4:0] <= `PI_SD int_regin[14:10];
		//     REGREAD(4 + PHY_BASE_ADDR, rddata);
		//     rddata[14:10] = 0b01111;
		//     REGWR  (4 + PHY_BASE_ADDR, rddata, 0);
		//    ->ydh_zq_event;
		// d.
		// param_phya_reg_tx_zq_en_test_aux <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_en_test_mux  <= `PI_SD int_regin[1];
		rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 1, 0);
		mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
		// TOP_REG_TX_SEL_GPIO
		//     <= #RD (~pwstrb_mask[7] & TOP_REG_TX_SEL_GPIO) |  pwstrb_mask_pwdata[7];
		// TOP_REG_TX_GPIO_OENZ
		//     <= #RD (~pwstrb_mask[6] & TOP_REG_TX_GPIO_OENZ) |  pwstrb_mask_pwdata[6];
		rddata = mmio_rd32(0x1c + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 1, 7, 6);
		mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
		//------------------------------
		// CMP offset Cal.
		//------------------------------
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 4, 0);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		KC_MSG("zq_cmp_en\n");

		//------------------------------
		// ZQ Complete
		//------------------------------
		uartlog("wait hw_done\n");
		// param_phyd_to_reg_zqcal_hw_done
		rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
		while (get_bits_from_value(rddata, 16, 16) == 0) {
			rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
			KC_MSG("wait param_phyd_to_reg_zqcal_hw_done ...\n");

			// opdelay(100);
		}
		uartlog("hw_done\n");
		// param_phya_reg_zqcal_done  <= `PI_SD int_regin[0];
		rddata = 0x00000001;
		mmio_wr32(0x0158 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
		// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
		// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
		// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
		// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
		// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
		// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
		rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 4, 0);
		mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
		// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
		rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 1, 31, 31);
		mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("TOP_REG_TX_ZQ_PD = 1...\n");

		// param_phyd_zqcal_hw_mode =0
		rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 18, 16);
		mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
		// ZQ clock off
		rddata = mmio_rd32(0x44 + CV_DDR_PHYD_APB);
		rddata = modified_bits_by_value(rddata, 0, 12, 12); // TOP_REG_CG_EN_ZQ
		mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("ZQ Complete ...\n");

		cvx16_zqcal_status();
	} else {
		KC_MSG("Not need ZQCAL\n");
	}
}

void cvx16_clk_normal(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x03);
	ddr_debug_num_write();
	KC_MSG("back to original frequency !!!\n\n");

	rddata = mmio_rd32(0x0c + CV_DDR_PHYD_APB);
	// rddata[13] TOP_REG_DDRPLL_SEL_LOW_SPEED 0
	// rddata[14] TOP_REG_DDRPLL_MAS_DIV_OUT_SEL 0
	rddata = modified_bits_by_value(rddata, 0, 13, 13);
	rddata = modified_bits_by_value(rddata, 0, 14, 14);
	mmio_wr32(0x0c + CV_DDR_PHYD_APB, rddata);
#ifdef SSC_EN
	//==============================================================
	// Enable SSC
	//==============================================================
	rddata = reg_set; // TOP_REG_SSC_SET
	mmio_wr32(0x54 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_span, 15, 0); // TOP_REG_SSC_SPAN
	mmio_wr32(0x58 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_step, 23, 0); // TOP_REG_SSC_STEP
	mmio_wr32(0x5C + 0x03002900, rddata);
	rddata = mmio_rd32(0x50 + 0x03002900);
	rddata = modified_bits_by_value(rddata, ~get_bits_from_value(rddata, 0, 0), 0, 0); // TOP_REG_SSC_SW_UP
	rddata = modified_bits_by_value(rddata, 1, 1, 1); // TOP_REG_SSC_EN_SSC
	rddata = modified_bits_by_value(rddata, 0, 3, 2); // TOP_REG_SSC_SSC_MODE
	rddata = modified_bits_by_value(rddata, 0, 4, 4); // TOP_REG_SSC_BYPASS
	rddata = modified_bits_by_value(rddata, 1, 5, 5); // extpulse
	rddata = modified_bits_by_value(rddata, 0, 6, 6); // ssc_syn_fix_div
	mmio_wr32(0x50 + 0x03002900, rddata);
	uartlog("SSC_EN\n");
#else
#ifdef SSC_BYPASS
	rddata = (reg_set & 0xfc000000) + 0x04000000; // TOP_REG_SSC_SET
	mmio_wr32(0x54 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_span, 15, 0); // TOP_REG_SSC_SPAN
	mmio_wr32(0x58 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_step, 23, 0); // TOP_REG_SSC_STEP
	mmio_wr32(0x5C + 0x03002900, rddata);
	rddata = mmio_rd32(0x50 + 0x03002900);
	rddata = modified_bits_by_value(rddata, ~get_bits_from_value(rddata, 0, 0), 0, 0); // TOP_REG_SSC_SW_UP
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // TOP_REG_SSC_EN_SSC
	rddata = modified_bits_by_value(rddata, 0, 3, 2); // TOP_REG_SSC_SSC_MODE
	rddata = modified_bits_by_value(rddata, 0, 4, 4); // TOP_REG_SSC_BYPASS
	rddata = modified_bits_by_value(rddata, 1, 5, 5); // TOP_REG_SSC_EXTPULSE
	rddata = modified_bits_by_value(rddata, 1, 6, 6); // ssc_syn_fix_div
	uartlog("SSC_BYPASS\n");
#else
	//==============================================================
	// SSC_EN =0
	//==============================================================
	uartlog("SSC_EN =0\n");
	rddata = reg_set; // TOP_REG_SSC_SET
	mmio_wr32(0x54 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_span, 15, 0); // TOP_REG_SSC_SPAN
	mmio_wr32(0x58 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_step, 23, 0); // TOP_REG_SSC_STEP
	mmio_wr32(0x5C + 0x03002900, rddata);
	rddata = mmio_rd32(0x50 + 0x03002900);
	rddata = modified_bits_by_value(rddata, ~get_bits_from_value(rddata, 0, 0), 0, 0); // TOP_REG_SSC_SW_UP
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // TOP_REG_SSC_EN_SSC
	rddata = modified_bits_by_value(rddata, 0, 3, 2); // TOP_REG_SSC_SSC_MODE
	rddata = modified_bits_by_value(rddata, 0, 4, 4); // TOP_REG_SSC_BYPASS
	rddata = modified_bits_by_value(rddata, 1, 5, 5); // TOP_REG_SSC_EXTPULSE
	rddata = modified_bits_by_value(rddata, 0, 6, 6); // ssc_syn_fix_div
	mmio_wr32(0x50 + 0x03002900, rddata);
	uartlog("SSC_OFF\n");
#endif // SSC_BYPASS
#endif // SSC_EN
	uartlog("back to original frequency\n");
}

void cvx16_clk_div2(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x01);
	ddr_debug_num_write();
	KC_MSG("div2 original frequency !!!\n\n");

	rddata = mmio_rd32(0x0c + CV_DDR_PHYD_APB);
	// rddata[14] = 1  ;  // TOP_REG_DDRPLL_MAS_DIV_OUT_SEL 1
	rddata = modified_bits_by_value(rddata, 1, 14, 14);
	mmio_wr32(0x0c + CV_DDR_PHYD_APB, rddata);
	uartlog("div2 original frequency\n");
}

void cvx16_INT_ISR_08(void)
{
	uint32_t EN_PLL_SPEED_CHG;
	uint32_t CUR_PLL_SPEED;
	uint32_t NEXT_PLL_SPEED;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x1c);
	ddr_debug_num_write();
	// param_phyd_clkctrl_init_complete   <= int_regin[0];
	rddata = 0x00000000;
	mmio_wr32(0x0118 + PHYD_BASE_ADDR, rddata);
	//----------------------------------------------------
	rddata = mmio_rd32(0x4c + CV_DDR_PHYD_APB);
	EN_PLL_SPEED_CHG = get_bits_from_value(rddata, 0, 0);
	CUR_PLL_SPEED = get_bits_from_value(rddata, 5, 4);
	NEXT_PLL_SPEED = get_bits_from_value(rddata, 9, 8);
	KC_MSG("CUR_PLL_SPEED = %x, NEXT_PLL_SPEED = %x, EN_PLL_SPEED_CHG=%x\n", CUR_PLL_SPEED, NEXT_PLL_SPEED,
	       EN_PLL_SPEED_CHG);

	//----------------------------------------------------
	cvx16_ddr_phy_power_on_seq2();
	cvx16_set_dfi_init_complete();
}

void cvx16_clk_div40(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x02);
	ddr_debug_num_write();
	KC_MSG("Enter low D40 frequency !!!\n\n");

	rddata = mmio_rd32(0x0c + CV_DDR_PHYD_APB);
	// TOP_REG_DDRPLL_SEL_LOW_SPEED =1
	rddata = modified_bits_by_value(rddata, 1, 13, 13);
	mmio_wr32(0x0c + CV_DDR_PHYD_APB, rddata);
	uartlog("Enter low D40 frequency\n");
}

void cvx16_ddr_phy_power_on_seq1(void)
{
	// power_seq_1
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x0e);
	ddr_debug_num_write();
	// RESETZ/CKE PD=0
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	// TOP_REG_TX_CA_PD_CKE0
	rddata = modified_bits_by_value(rddata, 0, 24, 24);
	// TOP_REG_TX_CA_PD_RESETZ
	rddata = modified_bits_by_value(rddata, 0, 30, 30);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("RESET PD !!!\n");

	// CA PD=0
	// All PHYA CA PD=0
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 31, 0);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("All PHYA CA PD=0 ...\n");

	// TOP_REG_TX_SEL_GPIO = 1 (DQ)
	rddata = mmio_rd32(0x1c + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 7, 7);
	mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_SEL_GPIO = 1\n");

	// DQ PD=0
	// TOP_REG_TX_BYTE0_PD
	// TOP_REG_TX_BYTE1_PD
	rddata = 0x00000000;
	mmio_wr32(0x00 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TX_BYTE PD=0 ...\n");

	// TOP_REG_TX_SEL_GPIO = 0 (DQ)
	rddata = mmio_rd32(0x1c + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 7, 7);
	mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_SEL_GPIO = 0\n");

	// power_seq_1
}

void cvx16_ddr_phy_power_on_seq2(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x0f);
	ddr_debug_num_write();
	// Change PLL frequency
	KC_MSG("Change PLL frequency if necessary ...\n");

	cvx16_chg_pll_freq();
	// OEN
	// param_phyd_sel_cke_oenz        <= `PI_SD int_regin[0];
	rddata = mmio_rd32(0x0154 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(0x0154 + PHYD_BASE_ADDR, rddata);
	// param_phyd_tx_ca_oenz          <= `PI_SD int_regin[0];
	// param_phyd_tx_ca_clk0_oenz     <= `PI_SD int_regin[8];
	// param_phyd_tx_ca_clk1_oenz     <= `PI_SD int_regin[16];
	rddata = 0x00000000;
	mmio_wr32(0x0130 + PHYD_BASE_ADDR, rddata);
	// Do DLLCAL if necessary
	KC_MSG("Do DLLCAL if necessary ...\n");

	cvx16_dll_cal();
	KC_MSG("Do DLLCAL done\n");

	//    KC_MSG("Do ZQCAL if necessary ...\n");

	// cvx16_ddr_zqcal_hw_isr8(0x7);//zqcal hw mode, bit0: offset_cal, bit1:pl_en, bit2:step2_en
	// KC_MSG("Do ZQCAL done\n");

	KC_MSG("cv181x without ZQ Calibration ...\n");

	// cvx16_ddr_zq240_cal();//zq240_cal
	// KC_MSG("Do cvx16_ddr_zq240_cal done\n");

	KC_MSG("cv181x without ZQ240 Calibration ...\n");

	// zq calculate variation
	//  zq_cal_var();
	KC_MSG("zq calculate variation not run\n");

	// CA PD =0
	// All PHYA CA PD=0
	rddata = 0x80000000;
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("All PHYA CA PD=0 ...\n");

	// BYTE PD =0
	rddata = 0x00000000;
	mmio_wr32(0x00 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TX_BYTE PD=0 ...\n");

	// power_on_2
}

void cvx16_ddr_phy_power_on_seq3(void)
{
	// power on 3
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x10);
	ddr_debug_num_write();
	// RESETYZ/CKE OENZ
	// param_phyd_sel_cke_oenz        <= `PI_SD int_regin[0];
	rddata = mmio_rd32(0x0154 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(0x0154 + PHYD_BASE_ADDR, rddata);
	// param_phyd_tx_ca_oenz          <= `PI_SD int_regin[0];
	// param_phyd_tx_ca_clk0_oenz     <= `PI_SD int_regin[8];
	// param_phyd_tx_ca_clk1_oenz     <= `PI_SD int_regin[16];
	rddata = 0x00000000;
	mmio_wr32(0x0130 + PHYD_BASE_ADDR, rddata);
	uartlog("[KC Info] --> ca_oenz  ca_clk_oenz !!!\n");

	// clock gated for power save
	// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline <= `PI_SD int_regin[0];
	// param_phya_reg_tx_byte1_en_extend_oenz_gated_dline <= `PI_SD int_regin[1];
	// param_phya_reg_tx_byte2_en_extend_oenz_gated_dline <= `PI_SD int_regin[2];
	// param_phya_reg_tx_byte3_en_extend_oenz_gated_dline <= `PI_SD int_regin[3];
	rddata = mmio_rd32(0x0204 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 18, 18);
	mmio_wr32(0x0204 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0224 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 18, 18);
	mmio_wr32(0x0224 + PHYD_BASE_ADDR, rddata);
	uartlog("[KC Info] --> en clock gated for power save !!!\n");

	// power on 3
}

void cvx16_wait_for_dfi_init_complete(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x13);
	ddr_debug_num_write();
	// synp setting
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x000001bc);
		//} while ((rddata & 0x00000001) != 1);
		if (get_bits_from_value(rddata, 0, 0) == 1) {
			break;
		}
	}
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001b0);
	rddata = modified_bits_by_value(rddata, 5, 5, 0);
	mmio_wr32(cfg_base + 0x000001b0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	KC_MSG("dfi_init_complete finish\n");
}

void cvx16_ctrlupd_short(void)
{
	// ctrlupd short
	uartlog("cvx16ctrlupd_short\n");
	ddr_debug_wr32(0x2a);
	ddr_debug_num_write();
	// for gate track
	// dis_auto_ctrlupd [31] =0, dis_auto_ctrlupd_srx [30] =0, ctrlupd_pre_srx [29] =1 @ 0x1a0
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	// original        rddata[31:29] = 0b001;
	rddata = modified_bits_by_value(rddata, 1, 31, 29);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	// dfi_t_ctrlupd_interval_min_x1024 [23:16] = 4, dfi_t_ctrlupd_interval_max_x1024 [7:0] = 8 @ 0x1a4
	rddata = mmio_rd32(cfg_base + 0x000001a4);
	// original        rddata[23:16] = 0x04;
	rddata = modified_bits_by_value(rddata, 4, 23, 16);
	// original        rddata[7:0] = 0x08;
	rddata = modified_bits_by_value(rddata, 8, 7, 0);
	mmio_wr32(cfg_base + 0x000001a4, rddata);
}

void cvx16_polling_dfi_init_start(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x11);
	ddr_debug_num_write();
	while (1) {
		rddata = mmio_rd32(0x3028 + PHYD_BASE_ADDR);
		if ((get_bits_from_value(rddata, 8, 8) == 1)) {
			break;
		}
	}
}

void cvx16_set_dfi_init_complete(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x48);
	ddr_debug_num_write();
#ifdef REAL_LOCK
	opdelay(20000);
#endif
	// rddata[8] = 1;
	rddata = 0x00000010;
	mmio_wr32(0x0120 + PHYD_BASE_ADDR, rddata);
	KC_MSG("set init_complete = 1 ...\n");

	// param_phyd_clkctrl_init_complete   <= int_regin[0];
	rddata = 0x00000001;
	mmio_wr32(0x0118 + PHYD_BASE_ADDR, rddata);
}

void cvx16_polling_synp_normal_mode(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x14);
	ddr_debug_num_write();
	// synp ctrl operating_mode
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x00000004);
		KC_MSG("operating_mode = %x\n", get_bits_from_value(rddata, 2, 0));

		if (get_bits_from_value(rddata, 2, 0) == 1) {
			break;
		}
	}
}

void cvx16_dfi_ca_park_prbs(uint32_t cap_enable)
{
	uint32_t dfi_ca_park_misc;
	uint32_t dfi_ca_park_retain_cycle;
	uint32_t dfi_ca_park_ca_ref;
	uint32_t dfi_ca_park_ca_park;

	// param_phyd_sw_dfi_phyupd_req =1
	rddata = 0x00000001;
	mmio_wr32(0x0174 + PHYD_BASE_ADDR, rddata);
	while (1) {
		// param_phyd_to_reg_dfi_phyupd_req  8   8
		// param_phyd_to_reg_dfi_phyupd_ack  9   9
		rddata = mmio_rd32(0x3030 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 9, 8) == 3) {
			break;
		}
	}
	// DDR3
	//   cfg_det_en = 0b1;
	//   cfg_cs_det_en = 0b1;
	//   cap_prbs_en = 0b1;
	//   cfg_cs_polarity = 0b1;
	//   cap_prbs_1t = 0b0;
	//   cfg_ca_reference = {0b0,0x0_ffff,0x7,0x0,0b1,0b0,0b1,0b1};
	//   cfg_cs_retain_cycle = 0b0000_0001;
	//   cfg_ca_retain_cycle = 0b0000_0000;
	//   cfg_ca_park_value = 0x3fff_ffff;
	if (cap_enable == 1) {
		dfi_ca_park_misc = 0x1B;
		mmio_wr32(DDR_TOP_BASE + 0x00, dfi_ca_park_misc);
		KC_MSG("dfi_ca_park_prbs enable = 1\n");
	} else {
		dfi_ca_park_misc = 0x0;
		mmio_wr32(DDR_TOP_BASE + 0x00, dfi_ca_park_misc);
		KC_MSG("dfi_ca_park_prbs enable = 0\n");
	}
	dfi_ca_park_retain_cycle = 0x01;
	mmio_wr32(DDR_TOP_BASE + 0x04, dfi_ca_park_retain_cycle);
	dfi_ca_park_ca_ref = 0x1ffffcb;
	mmio_wr32(DDR_TOP_BASE + 0x08, dfi_ca_park_ca_ref);
	dfi_ca_park_ca_park = 0x3fffffff;
	mmio_wr32(DDR_TOP_BASE + 0x0c, dfi_ca_park_ca_park);

	// param_phyd_sw_dfi_phyupd_req_clr =1
	rddata = 0x00000010;
	mmio_wr32(0x0174 + PHYD_BASE_ADDR, rddata);
}

void cvx16_wrlvl_req(void)
{
#ifndef DDR2
	uint32_t selfref_sw;
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
	uint32_t wr_odt_en;
	uint32_t rtt_wr;
	uint32_t rtt_nom;
	uint32_t port_num;

	// Note: training need ctrl_low_patch first
	mmio_wr32(0x005C + PHYD_BASE_ADDR, 0x00FE0000); //wrlvl response only DQ0
	// Write 0 to PCTRL_n.port_en, without port 0
	// port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// disable PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	selfref_sw = get_bits_from_value(rddata, 5, 5);
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	cvx16_clk_gating_disable();
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x2e);
	ddr_debug_num_write();
	KC_MSG_TR("%s\n", __func__);

	// save ctrl wr_odt_en
	rddata = mmio_rd32(cfg_base + 0x244);
	wr_odt_en = get_bits_from_value(rddata, 0, 0);
	// bist setting for dfi wrlvl
	cvx16_bist_wrlvl_init();
	// // RFSHCTL3.dis_auto_refresh =1
	// rddata = mmio_rd32(cfg_base + 0x60);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
	// mmio_wr32(cfg_base + 0x60, rddata);
#ifdef DDR3
	rtt_nom = 0;
	if (wr_odt_en == 1) {
		KC_MSG_TR("wr_odt_en =1 ...\n");

		// save rtt_wr
		rddata = mmio_rd32(cfg_base + 0xe0);
		rtt_wr = get_bits_from_value(rddata, 26, 25);
		if (rtt_wr != 0x0) {
			// disable rtt_wr
			rddata = modified_bits_by_value(rddata, 0, 26, 25);
			cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 16)); // MR2
			// set rtt_nom
			rddata = mmio_rd32(cfg_base + 0xdc);
			rtt_nom = modified_bits_by_value(rddata, 0, 9, 9); // rtt_nom[2]=0
			rtt_nom = modified_bits_by_value(rtt_nom, get_bits_from_value(rtt_wr, 1, 1), 6,
							 6); // rtt_nom[1]=rtt_wr[1]
			rtt_nom = modified_bits_by_value(rtt_nom, get_bits_from_value(rtt_wr, 0, 0), 2,
							 2); // rtt_nom[1]=rtt_wr[0]
			uartlog("dodt for wrlvl setting\n");
		}
	} else {
		uartlog("rtt_nom for wrlvl setting\n");
		KC_MSG_TR("wr_odt_en =0 ...\n");

		// set rtt_nom = 120ohm
		rddata = mmio_rd32(cfg_base + 0xdc);
		rtt_nom = modified_bits_by_value(rddata, 0, 9, 9); // rtt_nom[2]=0
		rtt_nom = modified_bits_by_value(rtt_nom, 1, 6, 6); // rtt_nom[1]=1
		rtt_nom = modified_bits_by_value(rtt_nom, 0, 2, 2); // rtt_nom[1]=0
		cvx16_synp_mrw(0x1, get_bits_from_value(rtt_nom, 15, 0));
	}
	rtt_nom = modified_bits_by_value(rtt_nom, 1, 7, 7); // Write leveling enable
	cvx16_synp_mrw(0x1, get_bits_from_value(rtt_nom, 15, 0));
	KC_MSG_TR("DDR3 MRS rtt_nom ...\n");

#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		rtt_nom = 0;
		if (wr_odt_en == 1) {
			KC_MSG_TR("wr_odt_en =1 ...\n");

			// save rtt_wr
			rddata = mmio_rd32(cfg_base + 0xe0);
			rtt_wr = get_bits_from_value(rddata, 26, 25);
			if (rtt_wr != 0x0) {
				// disable rtt_wr
				rddata = modified_bits_by_value(rddata, 0, 26, 25);
				cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 16)); // MR2
				// set rtt_nom
				rddata = mmio_rd32(cfg_base + 0xdc);
				rtt_nom = modified_bits_by_value(rddata, 0, 9, 9); // rtt_nom[2]=0
				rtt_nom = modified_bits_by_value(rtt_nom, get_bits_from_value(rtt_wr, 1, 1), 6,
								6); // rtt_nom[1]=rtt_wr[1]
				rtt_nom = modified_bits_by_value(rtt_nom, get_bits_from_value(rtt_wr, 0, 0), 2,
								2); // rtt_nom[1]=rtt_wr[0]
				uartlog("dodt for wrlvl setting\n");
			}
		} else {
			uartlog("rtt_nom for wrlvl setting\n");
			KC_MSG_TR("wr_odt_en =0 ...\n");

			// set rtt_nom = 120ohm
			rddata = mmio_rd32(cfg_base + 0xdc);
			rtt_nom = modified_bits_by_value(rddata, 0, 9, 9); // rtt_nom[2]=0
			rtt_nom = modified_bits_by_value(rtt_nom, 1, 6, 6); // rtt_nom[1]=1
			rtt_nom = modified_bits_by_value(rtt_nom, 0, 2, 2); // rtt_nom[1]=0
			cvx16_synp_mrw(0x1, get_bits_from_value(rtt_nom, 15, 0));
		}
		rtt_nom = modified_bits_by_value(rtt_nom, 1, 7, 7); // Write leveling enable
		cvx16_synp_mrw(0x1, get_bits_from_value(rtt_nom, 15, 0));
		KC_MSG_TR("DDR3 MRS rtt_nom ...\n");
	}
#endif
#ifdef DDR4
	rddata = mmio_rd32(cfg_base + 0xdc);
	rddata = modified_bits_by_value(rddata, 1, 7, 7); // Write leveling enable
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 0));
#endif
	rddata = mmio_rd32(0x0180 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_wrlvl_req
	rddata = modified_bits_by_value(rddata, wr_odt_en, 4, 4); // param_phyd_dfi_wrlvl_odt_en
	mmio_wr32(0x0180 + PHYD_BASE_ADDR, rddata);
	KC_MSG_TR("wait retraining finish ...\n");

	while (1) {
		//[0] param_phyd_dfi_wrlvl_done
		//[1] param_phyd_dfi_rdglvl_done
		//[2] param_phyd_dfi_rdlvl_done
		//[3] param_phyd_dfi_wdqlvl_done
		rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 0, 0) == 0x1) {
			// bist clock disable
			mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
			break;
		}
	}

	// RFSHCTL3.dis_auto_refresh =0
	rddata = mmio_rd32(cfg_base + 0x60);
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
	mmio_wr32(cfg_base + 0x60, rddata);

#ifdef DDR3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// rddata=modified_bits_by_value(rddata, 0, 7, 7); //Write leveling disable
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 0));
	rddata = mmio_rd32(cfg_base + 0xe0);
	cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 16)); // MR2
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x2e);
	ddr_debug_num_write();
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		rddata = mmio_rd32(cfg_base + 0xdc);
		// rddata=modified_bits_by_value(rddata, 0, 7, 7); //Write leveling disable
		cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 0));
		rddata = mmio_rd32(cfg_base + 0xe0);
		cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 16)); // MR2
		uartlog("%s\n", __func__);
		ddr_debug_wr32(0x2e);
		ddr_debug_num_write();
	}
#endif
#ifdef DDR4
	rddata = mmio_rd32(cfg_base + 0xdc);
	// rddata=modified_bits_by_value(rddata, 0, 7, 7); //Write leveling disable
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 0));
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x2e);
	ddr_debug_num_write();
#endif
	// // RFSHCTL3.dis_auto_refresh =0
	// rddata = mmio_rd32(cfg_base + 0x60);
	// rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
	// mmio_wr32(cfg_base + 0x60, rddata);
	// restore PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, selfref_sw, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 1 to PCTRL_n.port_en
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	// cvx16_wrlvl_status();
	cvx16_clk_gating_enable();
#endif // not DDR2
}

void cvx16_rdglvl_req(void)
{
	uint32_t selfref_sw;
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
#ifdef DDR3
	uint32_t ddr3_mpr_mode;
#endif //DDR3
#ifdef DDR2_3
		uint32_t ddr3_mpr_mode;
#endif
	uint32_t port_num;
	// Note: training need ctrl_low_patch first
	//  Write 0 to PCTRL_n.port_en, without port 0
	//  port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// disable PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	selfref_sw = get_bits_from_value(rddata, 5, 5);
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   // this register must not be set to 1
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	cvx16_clk_gating_disable();
	// RFSHCTL3.dis_auto_refresh =1
	// rddata = mmio_rd32(cfg_base + 0x60);
	// rddata=modified_bits_by_value(rddata, 1, 0, 0); //RFSHCTL3.dis_auto_refresh
	// mmio_wr32(cfg_base + 0x60, rddata);
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x2f);
	ddr_debug_num_write();
	KC_MSG_TR("%s\n", __func__);

#ifdef DDR3
	rddata = mmio_rd32(0x0184 + PHYD_BASE_ADDR);
	ddr3_mpr_mode = get_bits_from_value(rddata, 4, 4);
	if (ddr3_mpr_mode) {
		// RFSHCTL3.dis_auto_refresh =1
		rddata = mmio_rd32(cfg_base + 0x60);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
		mmio_wr32(cfg_base + 0x60, rddata);
		// MR3
		rddata = mmio_rd32(cfg_base + 0xe0);
		rddata = modified_bits_by_value(rddata, 1, 2, 2); // Dataflow from MPR
		cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
	}
#endif
#ifdef DDR2_3

	rddata = mmio_rd32(0x0184 + PHYD_BASE_ADDR);
	ddr3_mpr_mode = get_bits_from_value(rddata, 4, 4);
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		if (ddr3_mpr_mode) {
			// RFSHCTL3.dis_auto_refresh =1
			rddata = mmio_rd32(cfg_base + 0x60);
			rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
			mmio_wr32(cfg_base + 0x60, rddata);
			// MR3
			rddata = mmio_rd32(cfg_base + 0xe0);
			rddata = modified_bits_by_value(rddata, 1, 2, 2); // Dataflow from MPR
			cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
		}
	}
#endif
	// bist setting for dfi rdglvl
	cvx16_bist_rdglvl_init();
	rddata = mmio_rd32(0x0184 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_rdglvl_req
	mmio_wr32(0x0184 + PHYD_BASE_ADDR, rddata);
	KC_MSG_TR("wait retraining finish ...\n");

	while (1) {
		//[0] param_phyd_dfi_wrlvl_done
		//[1] param_phyd_dfi_rdglvl_done
		//[2] param_phyd_dfi_rdlvl_done
		//[3] param_phyd_dfi_wdqlvl_done
		rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 1, 1) == 0x1) {
			// bist clock disable
			mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
			break;
		}
	}
#ifdef DDR3
	if (ddr3_mpr_mode) {
		// MR3
		rddata = mmio_rd32(cfg_base + 0xe0);
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // Normal operation
		cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
		// RFSHCTL3.dis_auto_refresh =0
		rddata = mmio_rd32(cfg_base + 0x60);
		rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
		mmio_wr32(cfg_base + 0x60, rddata);
	}
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x2f);
	ddr_debug_num_write();
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		if (ddr3_mpr_mode) {
			// MR3
			rddata = mmio_rd32(cfg_base + 0xe0);
			rddata = modified_bits_by_value(rddata, 0, 2, 2); // Normal operation
			cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
			// RFSHCTL3.dis_auto_refresh =0
			rddata = mmio_rd32(cfg_base + 0x60);
			rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
			mmio_wr32(cfg_base + 0x60, rddata);
		}
		uartlog("%s\n", __func__);
		ddr_debug_wr32(0x2f);
		ddr_debug_num_write();
	}
#endif
	// RFSHCTL3.dis_auto_refresh =0
	// rddata = mmio_rd32(cfg_base + 0x60);
	// rddata=modified_bits_by_value(rddata, 0, 0, 0); //RFSHCTL3.dis_auto_refresh
	// mmio_wr32(cfg_base + 0x60, rddata);
	// restore PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, selfref_sw, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 1 to PCTRL_n.port_en
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	// cvx16_rdglvl_status();
	cvx16_clk_gating_enable();
}

void cvx16_rdlvl_req(uint32_t mode)
{
	uint32_t selfref_sw;
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
#ifdef DDR3
	uint32_t ddr3_mpr_mode;
#endif //DDR3
#ifdef DDR2_3
	uint32_t ddr3_mpr_mode;
#endif
	uint32_t port_num;
	uint32_t vref_training_en;
	// uint32_t code_neg; //unused
	// uint32_t code_pos; //unused
	// Note: training need ctrl_low_patch first
	// mode = 0x0  : MPR mode, DDR3 only.
	// mode = 0x1  : sram write/read continuous goto
	// mode = 0x2  : multi- bist write/read
	// mode = 0x10 : with Error enject,  multi- bist write/read
	// mode = 0x12 : with Error enject,  multi- bist write/read
	//  Write 0 to PCTRL_n.port_en, without port 0
	//  port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// disable PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	selfref_sw = get_bits_from_value(rddata, 5, 5);
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	cvx16_clk_gating_disable();
	//    //RFSHCTL3.dis_auto_refresh =1
	//    rddata = mmio_rd32(cfg_base + 0x60);
	//    rddata=modified_bits_by_value(rddata, 1, 0, 0); //RFSHCTL3.dis_auto_refresh
	//    mmio_wr32(cfg_base + 0x60, rddata);
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x30);
	ddr_debug_num_write();
	cvx16_dfi_ca_park_prbs(1);
	KC_MSG("%s\n", __func__);

	//deskew start from 0x20
	rddata = mmio_rd32(0x0080 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x20, 22, 16); //param_phyd_pirdlvl_deskew_start
	rddata = modified_bits_by_value(rddata, 0x1F, 30, 24); //param_phyd_pirdlvl_deskew_end
	mmio_wr32(0x0080 + PHYD_BASE_ADDR,  rddata);

	// save param_phyd_pirdlvl_vref_training_en
	rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
	vref_training_en = get_bits_from_value(rddata, 2, 2);
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // param_phyd_pirdlvl_rx_init_deskew_en
	rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_pirdlvl_vref_training_en
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // param_phyd_pirdlvl_rdvld_training_en = 0
	mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);
#ifdef DDR3
	rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
	ddr3_mpr_mode = get_bits_from_value(rddata, 4, 4);
	if (ddr3_mpr_mode) {
		// RFSHCTL3.dis_auto_refresh =1
		rddata = mmio_rd32(cfg_base + 0x60);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
		mmio_wr32(cfg_base + 0x60, rddata);
		// MR3
		rddata = mmio_rd32(cfg_base + 0xe0);
		rddata = modified_bits_by_value(rddata, 1, 2, 2); // Dataflow from MPR
		cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
	}
#endif
#ifdef DDR2_3
	rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
	ddr3_mpr_mode = get_bits_from_value(rddata, 4, 4);
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		if (ddr3_mpr_mode) {
			// RFSHCTL3.dis_auto_refresh =1
			rddata = mmio_rd32(cfg_base + 0x60);
			rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
			mmio_wr32(cfg_base + 0x60, rddata);
			// MR3
			rddata = mmio_rd32(cfg_base + 0xe0);
			rddata = modified_bits_by_value(rddata, 1, 2, 2); // Dataflow from MPR
			cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
		}
	}
#endif
	// bist setting for dfi rdglvl
	cvx16_bist_rdlvl_init(mode);
	rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_rdlvl_req
	mmio_wr32(0x0188 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dfi_rdlvl_req 1\n");

	KC_MSG("wait retraining finish ...\n");

	while (1) {
		//[0] param_phyd_dfi_wrlvl_done
		//[1] param_phyd_dfi_rdglvl_done
		//[2] param_phyd_dfi_rdlvl_done
		//[3] param_phyd_dfi_wdqlvl_done
		rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 2, 2) == 0x1) {
			break;
		}
	}
	if (vref_training_en == 0x1) {
		rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_pirdlvl_vref_training_en
		mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);
		// final training, keep rx trig_lvl
		KC_MSG("final training, keep rx trig_lvl\n");

		rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_rdlvl_req
		mmio_wr32(0x0188 + PHYD_BASE_ADDR, rddata);
		KC_MSG("dfi_rdlvl_req 2\n");

		KC_MSG("wait retraining finish ...\n");

		while (1) {
			//[0] param_phyd_dfi_wrlvl_done
			//[1] param_phyd_dfi_rdglvl_done
			//[2] param_phyd_dfi_rdlvl_done
			//[3] param_phyd_dfi_wdqlvl_done
			rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 2, 2) == 0x1) {
				break;
			}
		}
		rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, vref_training_en, 2, 2); // param_phyd_pirdlvl_vref_training_en
		mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);
	}

#ifdef DDR3
	if (ddr3_mpr_mode) {
		// MR3
		rddata = mmio_rd32(cfg_base + 0xe0);
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // Normal operation
		cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
		// RFSHCTL3.dis_auto_refresh =0
		rddata = mmio_rd32(cfg_base + 0x60);
		rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
		mmio_wr32(cfg_base + 0x60, rddata);
	}
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x30);
	ddr_debug_num_write();
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		if (ddr3_mpr_mode) {
			// MR3
			rddata = mmio_rd32(cfg_base + 0xe0);
			rddata = modified_bits_by_value(rddata, 0, 2, 2); // Normal operation
			cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
			// RFSHCTL3.dis_auto_refresh =0
			rddata = mmio_rd32(cfg_base + 0x60);
			rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
			mmio_wr32(cfg_base + 0x60, rddata);
		}
		uartlog("%s\n", __func__);
		ddr_debug_wr32(0x30);
		ddr_debug_num_write();
	}
#endif

	cvx16_rdvld_train();

	//    //RFSHCTL3.dis_auto_refresh =0
	//    rddata = mmio_rd32(cfg_base + 0x60);
	//    rddata=modified_bits_by_value(rddata, 0, 0, 0); //RFSHCTL3.dis_auto_refresh
	//    mmio_wr32(cfg_base + 0x60, rddata);
	// bist clock disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
	cvx16_dfi_ca_park_prbs(0);
	// restore PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, selfref_sw, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 1 to PCTRL_n.port_en
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	// cvx16_rdlvl_status();
	cvx16_clk_gating_enable();
}

void cvx16_rdlvl_sw_req(uint32_t mode)
{
	uint32_t selfref_sw;
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
#ifdef DDR3
	uint32_t ddr3_mpr_mode;
#endif //DDR3
#ifdef DDR2_3
	uint32_t ddr3_mpr_mode;
#endif
	uint32_t port_num;
	uint32_t vref_training_en;
	uint32_t byte0_pirdlvl_sw_upd_ack;
	uint32_t byte1_pirdlvl_sw_upd_ack;
	uint32_t rx_vref_sel;
	uint32_t byte0_data_rise_fail;
	uint32_t byte0_data_fall_fail;
	uint32_t byte1_data_rise_fail;
	uint32_t byte1_data_fall_fail;
	uint32_t dlie_code;
	uint32_t byte0_all_le_found;
	uint32_t byte0_all_te_found;
	uint32_t byte1_all_le_found;
	uint32_t byte1_all_te_found;
	// uint32_t code_neg; //unused
	// uint32_t code_pos; //unused
	uint32_t byte0_cur_pirdlvl_st;
	uint32_t byte1_cur_pirdlvl_st;
	uint32_t sw_upd_req_start;

	sw_upd_req_start = 0;
	// mode = 0x0  : MPR mode, DDR3 only.
	// mode = 0x1  : sram write/read continuous goto
	// mode = 0x2  : multi- bist write/read
	// mode = 0x10 : with Error enject,  multi- bist write/read
	// mode = 0x12 : with Error enject,  multi- bist write/read
	//  Write 0 to PCTRL_n.port_en, without port 0
	//  port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// disable PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	selfref_sw = get_bits_from_value(rddata, 5, 5);
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	cvx16_clk_gating_disable();
	//    //RFSHCTL3.dis_auto_refresh =1
	//    rddata = mmio_rd32(cfg_base + 0x60);
	//    rddata=modified_bits_by_value(rddata, 1, 0, 0); //RFSHCTL3.dis_auto_refresh
	//    mmio_wr32(cfg_base + 0x60, rddata);
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x30);
	ddr_debug_num_write();
	cvx16_dfi_ca_park_prbs(1);
	KC_MSG("%s\n", __func__);

	//deskew start from 0x20
	rddata = mmio_rd32(0x0080 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x20, 22, 16); //param_phyd_pirdlvl_deskew_start
	rddata = modified_bits_by_value(rddata, 0x1F, 30, 24); //param_phyd_pirdlvl_deskew_end
	mmio_wr32(0x0080 + PHYD_BASE_ADDR,  rddata);

	// save param_phyd_pirdlvl_vref_training_en
	rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
	vref_training_en = get_bits_from_value(rddata, 2, 2);
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // param_phyd_pirdlvl_rx_init_deskew_en
	rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_pirdlvl_vref_training_en
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // param_phyd_pirdlvl_rdvld_training_en = 0
	mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);

#ifdef DDR3
	rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
	ddr3_mpr_mode = get_bits_from_value(rddata, 4, 4);
	if (ddr3_mpr_mode) {
		// RFSHCTL3.dis_auto_refresh =1
		rddata = mmio_rd32(cfg_base + 0x60);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
		mmio_wr32(cfg_base + 0x60, rddata);
		// MR3
		rddata = mmio_rd32(cfg_base + 0xe0);
		rddata = modified_bits_by_value(rddata, 1, 2, 2); // Dataflow from MPR
		cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
	}
#endif
#ifdef DDR2_3

	rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
	ddr3_mpr_mode = get_bits_from_value(rddata, 4, 4);
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		if (ddr3_mpr_mode) {
			// RFSHCTL3.dis_auto_refresh =1
			rddata = mmio_rd32(cfg_base + 0x60);
			rddata = modified_bits_by_value(rddata, 1, 0, 0); // RFSHCTL3.dis_auto_refresh
			mmio_wr32(cfg_base + 0x60, rddata);
			// MR3
			rddata = mmio_rd32(cfg_base + 0xe0);
			rddata = modified_bits_by_value(rddata, 1, 2, 2); // Dataflow from MPR
			cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
		}
	}
#endif

	// bist setting for dfi rdglvl
	cvx16_bist_rdlvl_init(mode);
	// SW mode
	rddata = mmio_rd32(0x0090 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 9, 9); // param_phyd_pirdlvl_sw
	mmio_wr32(0x0090 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_rdlvl_req
	mmio_wr32(0x0188 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dfi_rdlvl_req 1\n");

	KC_MSG("wait retraining finish ...\n");

	while (1) {
		//[0] param_phyd_dfi_wrlvl_done
		//[1] param_phyd_dfi_rdglvl_done
		//[2] param_phyd_dfi_rdlvl_done
		//[3] param_phyd_dfi_wdqlvl_done
		rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 2, 2) == 0x1) {
			break;
		}
		while (1) {
			rddata = mmio_rd32(0x31B0 + PHYD_BASE_ADDR);
			byte0_pirdlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
			byte0_all_le_found = get_bits_from_value(rddata, 5, 5) & get_bits_from_value(rddata, 7, 7);
			byte0_all_te_found = get_bits_from_value(rddata, 4, 4) & get_bits_from_value(rddata, 6, 6);
			byte0_cur_pirdlvl_st = get_bits_from_value(rddata, 23, 20);
			rddata = mmio_rd32(0x31B0 + 0x40 + PHYD_BASE_ADDR);
			byte1_pirdlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
			byte1_all_le_found = get_bits_from_value(rddata, 5, 5) & get_bits_from_value(rddata, 7, 7);
			byte1_all_te_found = get_bits_from_value(rddata, 4, 4) & get_bits_from_value(rddata, 6, 6);
			byte1_cur_pirdlvl_st = get_bits_from_value(rddata, 23, 20);
			KC_MSG("=1 byte0_pirdlvl_sw_upd_ack = %x, byte1_pirdlvl_sw_upd_ack = %x\n",
			       byte0_pirdlvl_sw_upd_ack, byte1_pirdlvl_sw_upd_ack);

			if ((byte0_all_le_found == 0) && (byte0_all_te_found == 0) && (byte1_all_le_found == 0) &&
			    (byte1_all_te_found == 0)) {
				sw_upd_req_start = 0x1;
			} else {
				if ((byte0_all_le_found == 0x1) && (byte0_all_te_found == 0x1) &&
				    (byte1_all_le_found == 0x1) && (byte1_all_te_found == 0x1) &&
				    ((byte0_cur_pirdlvl_st == 0x0) && (byte1_cur_pirdlvl_st == 0x0))) {
					sw_upd_req_start = 0x0;
				}
			}
			// KC_MSG("sw_upd_req_start = %x\n", sw_upd_req_start);

			if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte0_all_le_found & byte0_all_te_found)) &&
			    ((byte1_pirdlvl_sw_upd_ack == 0x1) || (byte1_all_le_found & byte1_all_te_found))) {
				rddata = mmio_rd32(0x0B24 + PHYD_BASE_ADDR);
				rx_vref_sel = get_bits_from_value(rddata, 4, 0);
				rddata = mmio_rd32(0x0B08 + PHYD_BASE_ADDR);
				dlie_code = get_bits_from_value(rddata, 15, 8);
				rddata = mmio_rd32(0x31B4 + PHYD_BASE_ADDR);
				if (byte0_all_te_found) {
					byte0_data_rise_fail = 0xff;
					byte0_data_fall_fail = 0xff;
				} else {
					byte0_data_rise_fail = get_bits_from_value(rddata, 24, 16);
					byte0_data_fall_fail = get_bits_from_value(rddata, 8, 0);
				}
				rddata = mmio_rd32(0x31B4 + 0x40 + PHYD_BASE_ADDR);
				if (byte1_all_te_found) {
					byte1_data_rise_fail = 0xff;
					byte1_data_fall_fail = 0xff;
				} else {
					byte1_data_rise_fail = get_bits_from_value(rddata, 24, 16);
					byte1_data_fall_fail = get_bits_from_value(rddata, 8, 0);
				}
				if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte1_pirdlvl_sw_upd_ack == 0x1)) &&
				    ((byte0_cur_pirdlvl_st != 0x0) && (byte1_cur_pirdlvl_st != 0x0))) {
					SHMOO_MSG("vref = %02x, sw_rdq_training_start = %08x , ",
						  rx_vref_sel, dlie_code);
					SHMOO_MSG("err_data_rise/err_data_fall = %08x, %08x\n",
						  ((byte0_data_rise_fail & 0x000000FF) |
						   ((byte1_data_rise_fail & 0x000000FF) << 8)),
						   ((byte0_data_fall_fail & 0x000000FF) |
						   ((byte1_data_fall_fail & 0x000000FF) << 8)));
				}
				//if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte1_pirdlvl_sw_upd_ack == 0x1))
				//    || ((byte0_cur_pirdlvl_st != 0x0) && (byte1_cur_pirdlvl_st != 0x0))) {
				if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte1_pirdlvl_sw_upd_ack == 0x1)) &&
				    (sw_upd_req_start == 0x1)) {
					rddata = mmio_rd32(0x0090 + PHYD_BASE_ADDR);
					rddata = modified_bits_by_value(rddata, 1, 10,
									10); // param_phyd_pirdlvl_sw_upd_req
					mmio_wr32(0x0090 + PHYD_BASE_ADDR, rddata);
				}
				KC_MSG("byte0_all_le_found, byte0_all_te_found, ");
				KC_MSG("byte1_all_le_found, byte1_all_te_found");
				KC_MSG(" =%x %x %x %x\n",
				       byte0_all_le_found, byte0_all_te_found,
				       byte1_all_le_found, byte1_all_te_found);

				break;
			}
			if ((byte0_all_le_found == 0x1) && (byte0_all_te_found == 0x1) &&
			    (byte1_all_le_found == 0x1) && (byte1_all_te_found == 0x1)) {
				break;
			}

			rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 2, 2) == 0x1) {
				break;
			}
		}
	}
	if (vref_training_en == 0x1) {
		rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_pirdlvl_vref_training_en
		mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);
		// final training, keep rx trig_lvl
		KC_MSG("final training, keep rx trig_lvl\n");

		rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_rdlvl_req
		mmio_wr32(0x0188 + PHYD_BASE_ADDR, rddata);
		KC_MSG("dfi_rdlvl_req 2\n");

		KC_MSG("wait retraining finish ...\n");

		while (1) {
			rddata = mmio_rd32(0x31B0 + PHYD_BASE_ADDR);
			byte0_pirdlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
			byte0_all_le_found = get_bits_from_value(rddata, 5, 5) & get_bits_from_value(rddata, 7, 7);
			byte0_all_te_found = get_bits_from_value(rddata, 4, 4) & get_bits_from_value(rddata, 6, 6);
			byte0_cur_pirdlvl_st = get_bits_from_value(rddata, 23, 20);
			rddata = mmio_rd32(0x31B0 + 0x40 + PHYD_BASE_ADDR);
			byte1_pirdlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
			byte1_all_le_found = get_bits_from_value(rddata, 5, 5) & get_bits_from_value(rddata, 7, 7);
			byte1_all_te_found = get_bits_from_value(rddata, 4, 4) & get_bits_from_value(rddata, 6, 6);
			byte1_cur_pirdlvl_st = get_bits_from_value(rddata, 23, 20);
			KC_MSG("Wait all_found clear  ");
			KC_MSG("byte0_all_le_found | byte0_all_te_found = %x, ",
			       (byte0_all_le_found | byte0_all_te_found));
			KC_MSG("byte1_all_le_found | byte1_all_te_found = %x\n",
			       (byte1_all_le_found | byte1_all_te_found));

			if ((byte0_all_le_found == 0) && (byte0_all_te_found == 0) &&
			    (byte1_all_le_found == 0) && (byte1_all_te_found == 0)) {
				break;
			}
		}
		while (1) {
			//[0] param_phyd_dfi_wrlvl_done
			//[1] param_phyd_dfi_rdglvl_done
			//[2] param_phyd_dfi_rdlvl_done
			//[3] param_phyd_dfi_wdqlvl_done
			rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 2, 2) == 0x1) {
				break;
			}
			while (1) {
				rddata = mmio_rd32(0x31B0 + PHYD_BASE_ADDR);
				byte0_pirdlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
				byte0_all_le_found =
					get_bits_from_value(rddata, 5, 5) & get_bits_from_value(rddata, 7, 7);
				byte0_all_te_found =
					get_bits_from_value(rddata, 4, 4) & get_bits_from_value(rddata, 6, 6);
				byte0_cur_pirdlvl_st = get_bits_from_value(rddata, 23, 20);
				rddata = mmio_rd32(0x31B0 + 0x40 + PHYD_BASE_ADDR);
				byte1_pirdlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
				byte1_all_le_found =
					get_bits_from_value(rddata, 5, 5) & get_bits_from_value(rddata, 7, 7);
				byte1_all_te_found =
					get_bits_from_value(rddata, 4, 4) & get_bits_from_value(rddata, 6, 6);
				byte1_cur_pirdlvl_st = get_bits_from_value(rddata, 23, 20);
				KC_MSG("=1 byte0_pirdlvl_sw_upd_ack = %x, byte1_pirdlvl_sw_upd_ack = %x\n",
				       byte0_pirdlvl_sw_upd_ack, byte1_pirdlvl_sw_upd_ack);

				if ((byte0_all_le_found == 0) && (byte0_all_te_found == 0) &&
				    (byte1_all_le_found == 0) && (byte1_all_te_found == 0)) {
					sw_upd_req_start = 0x1;
				} else {
					if ((byte0_all_le_found == 0x1) && (byte0_all_te_found == 0x1) &&
					    (byte1_all_le_found == 0x1) && (byte1_all_te_found == 0x1) &&
					    ((byte0_cur_pirdlvl_st == 0x0) && (byte1_cur_pirdlvl_st == 0x0))) {
						sw_upd_req_start = 0x0;
					}
				}
				// KC_MSG("sw_upd_req_start = %x\n", sw_upd_req_start);

				if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte0_all_le_found & byte0_all_te_found)) &&
				    ((byte1_pirdlvl_sw_upd_ack == 0x1) || (byte1_all_le_found & byte1_all_te_found))) {
					//if ((byte0_pirdlvl_sw_upd_ack == 0x1) && (byte1_pirdlvl_sw_upd_ack == 0x1)) {
					rddata = mmio_rd32(0x0B24 + PHYD_BASE_ADDR);
					rx_vref_sel = get_bits_from_value(rddata, 4, 0);
					rddata = mmio_rd32(0x0B08 + PHYD_BASE_ADDR);
					dlie_code = get_bits_from_value(rddata, 15, 8);
					rddata = mmio_rd32(0x31B4 + PHYD_BASE_ADDR);
					if (byte0_all_te_found) {
						byte0_data_rise_fail = 0xff;
						byte0_data_fall_fail = 0xff;
					} else {
						byte0_data_rise_fail = get_bits_from_value(rddata, 24, 16);
						byte0_data_fall_fail = get_bits_from_value(rddata, 8, 0);
					}
					rddata = mmio_rd32(0x31B4 + 0x40 + PHYD_BASE_ADDR);
					if (byte1_all_te_found) {
						byte1_data_rise_fail = 0xff;
						byte1_data_fall_fail = 0xff;
					} else {
						byte1_data_rise_fail = get_bits_from_value(rddata, 24, 16);
						byte1_data_fall_fail = get_bits_from_value(rddata, 8, 0);
					}
					if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte1_pirdlvl_sw_upd_ack == 0x1)) &&
					    ((byte0_cur_pirdlvl_st != 0x0) && (byte1_cur_pirdlvl_st != 0x0))) {
						SHMOO_MSG("vref = %02x, sw_rdq_training_start = %08x , ",
							  rx_vref_sel, dlie_code);
						SHMOO_MSG("err_data_rise/err_data_fall = %08x, %08x\n",
						       ((byte0_data_rise_fail & 0x000000FF) |
							((byte1_data_rise_fail & 0x000000FF) << 8)),
						       ((byte0_data_fall_fail & 0x000000FF) |
							((byte1_data_fall_fail & 0x000000FF) << 8)));
					}
					// if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte1_pirdlvl_sw_upd_ack == 0x1))
					//     || ((byte0_cur_pirdlvl_st != 0x0) && (byte1_cur_pirdlvl_st != 0x0))) {
					if (((byte0_pirdlvl_sw_upd_ack == 0x1) || (byte1_pirdlvl_sw_upd_ack == 0x1)) &&
					    (sw_upd_req_start == 0x1)) {
						rddata = mmio_rd32(0x0090 + PHYD_BASE_ADDR);
						rddata = modified_bits_by_value(rddata, 1, 10,
										10); // param_phyd_pirdlvl_sw_upd_req
						mmio_wr32(0x0090 + PHYD_BASE_ADDR, rddata);
					}
					KC_MSG("byte0_all_le_found, byte0_all_te_found, ");
					KC_MSG("byte1_all_le_found, byte1_all_te_found");
					KC_MSG(" =%x %x %x %x\n",
					       byte0_all_le_found, byte0_all_te_found,
					       byte1_all_le_found, byte1_all_te_found);

					break;
				}
				if ((byte0_all_le_found == 0x1) && (byte0_all_te_found == 0x1) &&
				    (byte1_all_le_found == 0x1) && (byte1_all_te_found == 0x1)) {
					break;
				}

				rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
				if (get_bits_from_value(rddata, 2, 2) == 0x1) {
					break;
				}
			}
		}
		rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, vref_training_en, 2, 2); // param_phyd_pirdlvl_vref_training_en
		mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);
	}

#ifdef DDR3
	if (ddr3_mpr_mode) {
		// MR3
		rddata = mmio_rd32(cfg_base + 0xe0);
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // Normal operation
		cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
		// RFSHCTL3.dis_auto_refresh =0
		rddata = mmio_rd32(cfg_base + 0x60);
		rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
		mmio_wr32(cfg_base + 0x60, rddata);
	}
	uartlog("cvx16_rdlvl_req\n");
	ddr_debug_wr32(0x30);
	ddr_debug_num_write();
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		if (ddr3_mpr_mode) {
			// MR3
			rddata = mmio_rd32(cfg_base + 0xe0);
			rddata = modified_bits_by_value(rddata, 0, 2, 2); // Normal operation
			cvx16_synp_mrw(0x3, get_bits_from_value(rddata, 15, 0));
			// RFSHCTL3.dis_auto_refresh =0
			rddata = mmio_rd32(cfg_base + 0x60);
			rddata = modified_bits_by_value(rddata, 0, 0, 0); // RFSHCTL3.dis_auto_refresh
			mmio_wr32(cfg_base + 0x60, rddata);
		}
		uartlog("cvx16_rdlvl_req\n");
		ddr_debug_wr32(0x30);
		ddr_debug_num_write();
	}
#endif

	cvx16_rdvld_train();

	//    //RFSHCTL3.dis_auto_refresh =0
	//    rddata = mmio_rd32(cfg_base + 0x60);
	//    rddata=modified_bits_by_value(rddata, 0, 0, 0); //RFSHCTL3.dis_auto_refresh
	//    mmio_wr32(cfg_base + 0x60, rddata);
	// bist clock disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
	cvx16_dfi_ca_park_prbs(0);
	// restore PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, selfref_sw, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 1 to PCTRL_n.port_en
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	// cvx16_rdlvl_status();
	cvx16_clk_gating_enable();
}

void cvx16_wdqlvl_req(uint32_t data_mode, uint32_t lvl_mode)
{
	uint32_t selfref_sw;
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
	// uint32_t bist_data_mode; //unused
	uint32_t port_num;
	// Note: training need ctrl_low_patch first
	//  Write 0 to PCTRL_n.port_en, without port 0
	//  port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// disable PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	selfref_sw = get_bits_from_value(rddata, 5, 5);
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	cvx16_clk_gating_disable();
	uartlog("cvx16_wdqlvl_req\n");
	ddr_debug_wr32(0x31);
	ddr_debug_num_write();
	cvx16_dfi_ca_park_prbs(1);
	KC_MSG("cvx16_wdqlvl_req\n");

	// param_phyd_piwdqlvl_dq_mode
	//     <= #RD (~pwstrb_mask[12] & param_phyd_piwdqlvl_dq_mode) |  pwstrb_mask_pwdata[12];
	// param_phyd_piwdqlvl_dm_mode
	//     <= #RD (~pwstrb_mask[13] & param_phyd_piwdqlvl_dm_mode) |  pwstrb_mask_pwdata[13];
	rddata = mmio_rd32(0x00BC + PHYD_BASE_ADDR);
	// lvl_mode =0x0, wdmlvl
	// lvl_mode =0x1, wdqlvl
	// lvl_mode =0x2, wdqlvl and wdmlvl
	if (lvl_mode == 0x0) {
		rddata = modified_bits_by_value(rddata, 0, 12, 12); // param_phyd_piwdqlvl_dq_mode
		rddata = modified_bits_by_value(rddata, 1, 13, 13); // param_phyd_piwdqlvl_dm_mode
	} else if (lvl_mode == 0x1) {
		rddata = modified_bits_by_value(rddata, 1, 12, 12); // param_phyd_piwdqlvl_dq_mode
		rddata = modified_bits_by_value(rddata, 0, 13, 13); // param_phyd_piwdqlvl_dm_mode
	} else if (lvl_mode == 0x2) {
		rddata = modified_bits_by_value(rddata, 1, 12, 12); // param_phyd_piwdqlvl_dq_mode
		rddata = modified_bits_by_value(rddata, 1, 13, 13); // param_phyd_piwdqlvl_dm_mode
	}
	mmio_wr32(0x00BC + PHYD_BASE_ADDR, rddata);
	if (lvl_mode == 0x0) {
		rddata = mmio_rd32(cfg_base + 0xC);
		rddata = modified_bits_by_value(rddata, 1, 7, 7);
		mmio_wr32(cfg_base + 0xC, rddata);
		//        cvx16_bist_wdmlvl_init(sram_sp);
		cvx16_bist_wdmlvl_init();
	} else {
		// bist setting for dfi rdglvl
		// data_mode = 0x0 : phyd pattern
		// data_mode = 0x1 : bist read/write
		// data_mode = 0x11: with Error enject,  multi- bist write/read
		// data_mode = 0x12: with Error enject,  multi- bist write/read
		//         cvx16_bist_wdqlvl_init(data_mode, sram_sp);
		cvx16_bist_wdqlvl_init(data_mode);
	}
	uartlog("cvx16_wdqlvl_req\n");
	ddr_debug_wr32(0x31);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x018C + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_wdqlvl_req
	if (lvl_mode == 0x0) {
		rddata = modified_bits_by_value(rddata, 0, 10, 10); // param_phyd_dfi_wdqlvl_vref_train_en
	} else {
		rddata = modified_bits_by_value(rddata, 1, 10, 10); // param_phyd_dfi_wdqlvl_vref_train_en
	}
	if ((data_mode == 0x1) || (data_mode == 0x11) || (data_mode == 0x12)) {
		rddata = modified_bits_by_value(rddata, 1, 4, 4); // param_phyd_dfi_wdqlvl_bist_data_en
	} else {
		rddata = modified_bits_by_value(rddata, 0, 4, 4); // param_phyd_dfi_wdqlvl_bist_data_en
	}
	mmio_wr32(0x018C + PHYD_BASE_ADDR, rddata);
	KC_MSG("wait retraining finish ...\n");

	while (1) {
		//[0] param_phyd_dfi_wrlvl_done
		//[1] param_phyd_dfi_rdglvl_done
		//[2] param_phyd_dfi_rdlvl_done
		//[3] param_phyd_dfi_wdqlvl_done
		rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 3, 3) == 0x1) {
			break;
		}
	}
	rddata = mmio_rd32(cfg_base + 0xC);
	rddata = modified_bits_by_value(rddata, 0, 7, 7);
	mmio_wr32(cfg_base + 0xC, rddata);
	// bist clock disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
	cvx16_dfi_ca_park_prbs(0);
	// restore PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, selfref_sw, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 1 to PCTRL_n.port_en
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	// cvx16_wdqlvl_status();
	cvx16_clk_gating_enable();
}

void cvx16_wdqlvl_sw_req(uint32_t data_mode, uint32_t lvl_mode)
{
	uint32_t selfref_sw;
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
	// uint32_t bist_data_mode; //unused
	uint32_t port_num;
	uint32_t byte0_piwdqlvl_sw_upd_ack;
	uint32_t byte1_piwdqlvl_sw_upd_ack;
	uint32_t tx_vref_sel;
	uint32_t byte0_data_rise_fail;
	uint32_t byte0_data_fall_fail;
	uint32_t byte1_data_rise_fail;
	uint32_t byte1_data_fall_fail;
	uint32_t dlie_code;
	uint32_t byte0_all_le_found;
	uint32_t byte0_all_te_found;
	uint32_t byte1_all_le_found;
	uint32_t byte1_all_te_found;
	// uint32_t sram_sp;
	//  Write 0 to PCTRL_n.port_en, without port 0
	//  port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// disable PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	selfref_sw = get_bits_from_value(rddata, 5, 5);
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, 0, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	cvx16_clk_gating_disable();
	uartlog("cvx16_wdqlvl_sw_req\n");
	ddr_debug_wr32(0x31);
	ddr_debug_num_write();
	cvx16_dfi_ca_park_prbs(1);
	KC_MSG("cvx16_wdqlvl_sw_req\n");

	// param_phyd_piwdqlvl_dq_mode
	//     <= #RD (~pwstrb_mask[12] & param_phyd_piwdqlvl_dq_mode) |  pwstrb_mask_pwdata[12];
	// param_phyd_piwdqlvl_dm_mode
	//     <= #RD (~pwstrb_mask[13] & param_phyd_piwdqlvl_dm_mode) |  pwstrb_mask_pwdata[13];
	rddata = mmio_rd32(0x00BC + PHYD_BASE_ADDR);
	// lvl_mode =0x0, wdmlvl
	// lvl_mode =0x1, wdqlvl
	// lvl_mode =0x2, wdqlvl and wdmlvl
	if (lvl_mode == 0x0) {
		rddata = modified_bits_by_value(rddata, 0, 12, 12); // param_phyd_piwdqlvl_dq_mode
		rddata = modified_bits_by_value(rddata, 1, 13, 13); // param_phyd_piwdqlvl_dm_mode
	} else if (lvl_mode == 0x1) {
		rddata = modified_bits_by_value(rddata, 1, 12, 12); // param_phyd_piwdqlvl_dq_mode
		rddata = modified_bits_by_value(rddata, 0, 13, 13); // param_phyd_piwdqlvl_dm_mode
	} else if (lvl_mode == 0x2) {
		rddata = modified_bits_by_value(rddata, 1, 12, 12); // param_phyd_piwdqlvl_dq_mode
		rddata = modified_bits_by_value(rddata, 1, 13, 13); // param_phyd_piwdqlvl_dm_mode
	}
	mmio_wr32(0x00BC + PHYD_BASE_ADDR, rddata);
	if (lvl_mode == 0x0) {
		//        cvx16_bist_wdmlvl_init(sram_sp);
		cvx16_bist_wdmlvl_init();
	} else {
		// bist setting for dfi rdglvl
		// data_mode = 0x0 : phyd pattern
		// data_mode = 0x1 : bist read/write
		// data_mode = 0x11: with Error enject,  multi- bist write/read
		// data_mode = 0x12: with Error enject,  multi- bist write/read
		//         cvx16_bist_wdqlvl_init(data_mode, sram_sp);
		cvx16_bist_wdqlvl_init(data_mode);
	}
	uartlog("cvx16_wdqlvl_req sw\n");
	ddr_debug_wr32(0x31);
	ddr_debug_num_write();
	// SW mode
	rddata = mmio_rd32(0x00BC + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 7, 7); // param_phyd_piwdqlvl_sw
	mmio_wr32(0x00BC + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x018C + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_dfi_wdqlvl_req
	if (lvl_mode == 0x0) {
		rddata = modified_bits_by_value(rddata, 0, 10, 10); // param_phyd_dfi_wdqlvl_vref_train_en
	} else {
		rddata = modified_bits_by_value(rddata, 1, 10, 10); // param_phyd_dfi_wdqlvl_vref_train_en
	}
	if ((data_mode == 0x1) || (data_mode == 0x11) || (data_mode == 0x12)) {
		rddata = modified_bits_by_value(rddata, 1, 4, 4); // param_phyd_dfi_wdqlvl_bist_data_en
	} else {
		rddata = modified_bits_by_value(rddata, 0, 4, 4); // param_phyd_dfi_wdqlvl_bist_data_en
	}
	mmio_wr32(0x018C + PHYD_BASE_ADDR, rddata);
	KC_MSG("wait retraining finish ...\n");

	while (1) {
		//[0] param_phyd_dfi_wrlvl_done
		//[1] param_phyd_dfi_rdglvl_done
		//[2] param_phyd_dfi_rdlvl_done
		//[3] param_phyd_dfi_wdqlvl_done
		rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 3, 3) == 0x1) {
			break;
		}
		while (1) {
			rddata = mmio_rd32(0x32A4 + PHYD_BASE_ADDR);
			byte0_piwdqlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
			byte0_all_le_found = get_bits_from_value(rddata, 18, 18);
			byte0_all_te_found = get_bits_from_value(rddata, 17, 17);
			rddata = mmio_rd32(0x32E4 + PHYD_BASE_ADDR);
			byte1_piwdqlvl_sw_upd_ack = get_bits_from_value(rddata, 0, 0);
			byte1_all_le_found = get_bits_from_value(rddata, 18, 18);
			byte1_all_te_found = get_bits_from_value(rddata, 17, 17);
			KC_MSG("=1 byte0_piwdqlvl_sw_upd_ack = %x, byte1_piwdqlvl_sw_upd_ack = %x ",
			       byte0_piwdqlvl_sw_upd_ack, byte1_piwdqlvl_sw_upd_ack);
			KC_MSG("byte0_all_found = %x, byte1_all_found = %x\n",
			       (byte0_all_le_found & byte0_all_te_found),
			       (byte1_all_le_found & byte1_all_te_found));

			if (((byte0_piwdqlvl_sw_upd_ack == 0x1) || (byte0_all_le_found & byte0_all_te_found)) &&
			    ((byte1_piwdqlvl_sw_upd_ack == 0x1) || (byte1_all_le_found & byte1_all_te_found))) {
				rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
				tx_vref_sel = get_bits_from_value(rddata, 20, 16);
				rddata = mmio_rd32(0x32A8 + PHYD_BASE_ADDR);
				dlie_code = get_bits_from_value(rddata, 30, 18);
				if (byte0_all_te_found) {
					byte0_data_rise_fail = 0xff;
					byte0_data_fall_fail = 0xff;
				} else {
					byte0_data_rise_fail = get_bits_from_value(rddata, 17, 9);
					byte0_data_fall_fail = get_bits_from_value(rddata, 8, 0);
				}
				rddata = mmio_rd32(0x32E8 + PHYD_BASE_ADDR);
				if (byte1_all_te_found) {
					byte1_data_rise_fail = 0xff;
					byte1_data_fall_fail = 0xff;
				} else {
					byte1_data_rise_fail = get_bits_from_value(rddata, 17, 9);
					byte1_data_fall_fail = get_bits_from_value(rddata, 8, 0);
				}
				if ((byte0_piwdqlvl_sw_upd_ack == 0x1) || (byte1_piwdqlvl_sw_upd_ack == 0x1)) {
					SHMOO_MSG("vref = %02x, sw_wdq_training_start = %08x , ",
					       tx_vref_sel, dlie_code);

					SHMOO_MSG("err_data_rise/err_data_fall = %08x, %08x\n",
					       ((byte0_data_rise_fail & 0x000000FF) |
						((byte1_data_rise_fail & 0x000000FF) << 8)),
					       ((byte0_data_fall_fail & 0x000000FF) |
						((byte1_data_fall_fail & 0x000000FF) << 8)));
				}
				rddata = mmio_rd32(0x00BC + PHYD_BASE_ADDR);
				rddata = modified_bits_by_value(rddata, 1, 8, 8); // param_phyd_piwdqlvl_sw_upd_req
				mmio_wr32(0x00BC + PHYD_BASE_ADDR, rddata);
				KC_MSG("param_phyd_piwdqlvl_sw_upd_req = %x\n",
				       get_bits_from_value(rddata, 8, 8));

				KC_MSG("byte0_all_le_found, byte0_all_te_found, ");
				KC_MSG("byte1_all_le_found, byte1_all_te_found");
				KC_MSG("= %x %x %x %x\n",
				       byte0_all_le_found, byte0_all_te_found,
				       byte1_all_le_found, byte1_all_te_found);

				break;
			}
			if ((byte0_all_le_found == 0x1) && (byte0_all_te_found == 0x1) && (byte1_all_le_found == 0x1) &&
			    (byte1_all_te_found == 0x1)) {
				break;
			}
		}
	}
	// bist clock disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
	cvx16_dfi_ca_park_prbs(0);
	// restore PWRCTL.powerdown_en, PWRCTL.selfref_en
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, selfref_sw, 5, 5); // PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3); // PWRCTL.en_dfi_dram_clk_disable
	// rddata=modified_bits_by_value(rddata, 0, 2, 2); //PWRCTL.deeppowerdown_en, non-mDDR/non-LPDDR2/non-LPDDR3,
							   //this register must not be set to 1
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1); // PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0); // PWRCTL.selfref_en
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 1 to PCTRL_n.port_en
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	// cvx16_wdqlvl_status();
	cvx16_clk_gating_enable();
}

void cvx16_wrlvl_status(void)
{
#if defined(DBG_SHMOO) || defined(DDR_SIM)
	NOTICE("cvx16_wrlvl_status\n");
	ddr_debug_wr32(0x32);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x3100 + PHYD_BASE_ADDR);
	NOTICE("wrlvl_byte0_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	NOTICE("wrlvl_byte0_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	rddata = mmio_rd32(0x3104 + PHYD_BASE_ADDR);
	NOTICE("wrlvl_byte1_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	NOTICE("wrlvl_byte1_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	// rddata = mmio_rd32(0x3108 + PHYD_BASE_ADDR);
	// NOTICE("wrlvl_byte2_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	// NOTICE("wrlvl_byte2_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	// rddata = mmio_rd32(0x310C + PHYD_BASE_ADDR);
	// NOTICE("wrlvl_byte3_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	// NOTICE("wrlvl_byte3_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	rddata = mmio_rd32(0x3110 + PHYD_BASE_ADDR);
	NOTICE("wrlvl_byte0_status = %x\n", get_bits_from_value(rddata, 15, 0));
	NOTICE("wrlvl_byte1_status = %x\n", get_bits_from_value(rddata, 31, 16));

	// rddata = mmio_rd32(0x3114 + PHYD_BASE_ADDR);
	// NOTICE("wrlvl_byte2_status = %x\n", get_bits_from_value(rddata, 15, 0));
	// NOTICE("wrlvl_byte3_status = %x\n", get_bits_from_value(rddata, 31, 16));

	// RAW DLINE_UPD
	mmio_wr32(0x016C + PHYD_BASE_ADDR, 0xffffffff);
	rddata = mmio_rd32(0x0A14 + PHYD_BASE_ADDR);
	NOTICE("byte0 tx dqs shift/delay_dqsn/delay_dqsp = %x\n", rddata);

	rddata = mmio_rd32(0x0A34 + PHYD_BASE_ADDR);
	NOTICE("byte0 tx dqs raw delay_dqsn/delay_dqsp = %x\n", rddata);

	rddata = mmio_rd32(0x0A54 + PHYD_BASE_ADDR);
	NOTICE("byte1 tx dqs shift/delay_dqsn/delay_dqsp = %x\n", rddata);

	rddata = mmio_rd32(0x0A74 + PHYD_BASE_ADDR);
	NOTICE("byte1 tx dqs raw delay_dqsn/delay_dqsp = %x\n", rddata);

	// rddata = mmio_rd32(0x0A94 + PHYD_BASE_ADDR);
	// NOTICE("byte2 tx dqs shift/delay_dqsn/delay_dqsp = %x\n", rddata);

	// rddata = mmio_rd32(0x0AB4 + PHYD_BASE_ADDR);
	// NOTICE("byte2 tx dqs raw delay_dqsn/delay_dqsp = %x\n", rddata);

	// rddata = mmio_rd32(0x0AD4 + PHYD_BASE_ADDR);
	// NOTICE("byte3 tx dqs shift/delay_dqsn/delay_dqsp = %x\n", rddata);

	// rddata = mmio_rd32(0x0AE4 + PHYD_BASE_ADDR);
	// NOTICE("byte3 tx dqs raw delay_dqsn/delay_dqsp = %x\n", rddata);
#endif //DBG_SHMOO || DDR_SIM
}

void cvx16_rdglvl_status(void)
{
#if defined(DBG_SHMOO) || defined(DDR_SIM)
	NOTICE("cvx16_rdglvl_status\n");
	ddr_debug_wr32(0x33);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x3140 + PHYD_BASE_ADDR);
	NOTICE("rdglvl_byte0_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	NOTICE("rdglvl_byte0_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	rddata = mmio_rd32(0x3144 + PHYD_BASE_ADDR);
	NOTICE("rdglvl_byte1_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	NOTICE("rdglvl_byte1_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	// rddata = mmio_rd32(0x3148 + PHYD_BASE_ADDR);
	// NOTICE("rdglvl_byte2_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	// NOTICE("rdglvl_byte2_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	// rddata = mmio_rd32(0x314C + PHYD_BASE_ADDR);
	// NOTICE("rdglvl_byte3_hard0 = %x\n", get_bits_from_value(rddata, 13, 0));
	// NOTICE("rdglvl_byte3_hard1 = %x\n", get_bits_from_value(rddata, 29, 16));

	rddata = mmio_rd32(0x3150 + PHYD_BASE_ADDR);
	NOTICE("rdglvl_byte0_status = %x\n", get_bits_from_value(rddata, 15, 0));
	NOTICE("rdglvl_byte1_status = %x\n", get_bits_from_value(rddata, 31, 16));

	// rddata = mmio_rd32(0x3154 + PHYD_BASE_ADDR);
	// NOTICE("rdglvl_byte2_status = %x\n", get_bits_from_value(rddata, 15, 0));
	// NOTICE("rdglvl_byte3_status = %x\n", get_bits_from_value(rddata, 31, 16));

	rddata = mmio_rd32(0x0B0C + PHYD_BASE_ADDR);
	NOTICE("byte0 mask shift/delay = %x\n", rddata);

	rddata = mmio_rd32(0x0B3C + PHYD_BASE_ADDR);
	NOTICE("byte1 mask shift/delay = %x\n", rddata);

	// rddata = mmio_rd32(0x0B6C + PHYD_BASE_ADDR);
	// NOTICE("byte2 mask shift/delay = %x\n", rddata);

	// rddata = mmio_rd32(0x0B9C + PHYD_BASE_ADDR);
	// NOTICE("byte3 mask shift/delay = %x\n", rddata);

	// RAW DLINE_UPD
	mmio_wr32(0x016C + PHYD_BASE_ADDR, 0xffffffff);
	// raw
	rddata = mmio_rd32(0x0B20 + PHYD_BASE_ADDR);
	NOTICE("raw byte0 mask delay = %x\n", get_bits_from_value(rddata, 14, 8));

	rddata = mmio_rd32(0x0B50 + PHYD_BASE_ADDR);
	NOTICE("raw byte1 mask delay = %x\n", get_bits_from_value(rddata, 14, 8));

	// rddata = mmio_rd32(0x0B80 + PHYD_BASE_ADDR);
	// NOTICE("raw byte2 mask delay = %x\n", get_bits_from_value(rddata, 14, 8));

	// rddata = mmio_rd32(0x0BB0 + PHYD_BASE_ADDR);
	// NOTICE("raw byte3 mask delay = %x\n", get_bits_from_value(rddata, 14, 8));
#endif //DBG_SHMOO || DDR_SIM
}

void cvx16_rdlvl_status(void)
{
#if defined(DBG_SHMOO) || defined(DDR_SIM)
	uint32_t i;

	NOTICE("cvx16_rdlvl_status\n");
	ddr_debug_wr32(0x34);
	ddr_debug_num_write();
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x3180 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq0_rise_le = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq1_rise_le = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq2_rise_le = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq3_rise_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3184 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq4_rise_le = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq5_rise_le = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq6_rise_le = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq7_rise_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3188 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq8_rise_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x318C + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq0_rise_te = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq1_rise_te = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq2_rise_te = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq3_rise_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3190 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq4_rise_te = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq5_rise_te = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq6_rise_te = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq7_rise_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3194 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq8_rise_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x3198 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq0_fall_le = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq1_fall_le = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq2_fall_le = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq3_fall_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x319C + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq4_fall_le = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq5_fall_le = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq6_fall_le = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq7_fall_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x31A0 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq8_fall_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x31A4 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq0_fall_te = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq1_fall_te = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq2_fall_te = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq3_fall_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x31A8 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq4_fall_te = %x\n", i, get_bits_from_value(rddata, 7, 0));
		NOTICE("rdlvl_byte%x_dq5_fall_te = %x\n", i, get_bits_from_value(rddata, 15, 8));
		NOTICE("rdlvl_byte%x_dq6_fall_te = %x\n", i, get_bits_from_value(rddata, 23, 16));
		NOTICE("rdlvl_byte%x_dq7_fall_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x31AC + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_byte%x_dq8_fall_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x31B0 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_status0_byte%x = %x\n", i, rddata);

		rddata = mmio_rd32(0x31B4 + i * 0x40 + PHYD_BASE_ADDR);
		NOTICE("rdlvl_status1_byte%x = %x\n", i, rddata);
	}
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B00 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x deskewdq3210 = %x\n", i, rddata);

		rddata = mmio_rd32(0x0B04 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x deskewdq7654 = %x\n", i, rddata);

		rddata = mmio_rd32(0x0B08 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x rdqspos/neg/deskewdq8 = %x\n", i, rddata);
	}
	// rdvld
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B14 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x_rdvld = %x\n", i, get_bits_from_value(rddata, 20, 16));
	}
	// trig_lvl
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x trig_lvl_dq = %x, trig_lvl_dqs = %x\n", i, get_bits_from_value(rddata, 4, 0),
		       get_bits_from_value(rddata, 20, 16));
	}
	// RAW DLINE_UPD
	mmio_wr32(0x016C + PHYD_BASE_ADDR, 0xffffffff);
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B18 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x deskewdq3210_raw = %x\n", i, rddata);

		rddata = mmio_rd32(0x0B1C + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x deskewdq7654_raw = %x\n", i, rddata);

		rddata = mmio_rd32(0x0B20 + i * 0x30 + PHYD_BASE_ADDR);
		NOTICE("byte%x rdqspos/neg/mask/deskewdq8 = %x\n", i, rddata);
	}
#endif // DBG_SHMOO || DDR_SIM
}

void cvx16_wdqlvl_status(void)
{
#if defined(DBG_SHMOO) || defined(DDR_SIM)
	NOTICE("cvx16_wdqlvl_status\n");
	ddr_debug_wr32(0x35);
	ddr_debug_num_write();
	for (int i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x3280 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq0_le = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq1_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3284 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq2_le = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq3_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3288 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq4_le = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq5_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x328C + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq6_le = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq7_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3290 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq8_le = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq0_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3294 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq1_te = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq2_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3298 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq3_te = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq4_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x329C + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq5_te = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq6_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x32A0 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq7_te = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq8_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x32A4 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_status0 = %x\n", i, rddata);

		rddata = mmio_rd32(0x32A8 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_status1 = %x\n", i, rddata);
	}
	// wdq shift
	for (int i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0A00 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq0 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq1 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A04 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq2 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq3 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A08 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq4 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq5 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A0C + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq6 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));
		NOTICE("piwdqlvl_byte%x_dq7 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A10 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq8 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));
	}
	rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
	NOTICE("piwdqlvl tx vref = %x\n", get_bits_from_value(rddata, 20, 16));

	// RAW DLINE_UPD
	mmio_wr32(0x016C + PHYD_BASE_ADDR, 0xffffffff);
	for (int i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0A20 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq0 raw delay = %x\n", i, get_bits_from_value(rddata, 6, 0));
		NOTICE("piwdqlvl_byte%x_dq1 raw delay = %x\n", i, get_bits_from_value(rddata, 22, 16));

		rddata = mmio_rd32(0x0A24 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq2 raw delay = %x\n", i, get_bits_from_value(rddata, 6, 0));
		NOTICE("piwdqlvl_byte%x_dq3 raw delay = %x\n", i, get_bits_from_value(rddata, 22, 16));

		rddata = mmio_rd32(0x0A28 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq4 raw delay = %x\n", i, get_bits_from_value(rddata, 6, 0));
		NOTICE("piwdqlvl_byte%x_dq5 raw delay = %x\n", i, get_bits_from_value(rddata, 22, 16));

		rddata = mmio_rd32(0x0A2C + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq6 raw delay = %x\n", i, get_bits_from_value(rddata, 6, 0));
		NOTICE("piwdqlvl_byte%x_dq7 raw delay = %x\n", i, get_bits_from_value(rddata, 22, 16));

		rddata = mmio_rd32(0x0A30 + 0x40 * i + PHYD_BASE_ADDR);
		NOTICE("piwdqlvl_byte%x_dq8 raw delay = %x\n", i, get_bits_from_value(rddata, 6, 0));
	}
#endif //DBG_SHMOO || DDR_SIM
}

void cvx16_dll_cal_status(void)
{
#if defined(DBG_SHMOO) || defined(DDR_SIM)
	uint32_t err_cnt;
	uint32_t rx_dll_code;
	uint32_t tx_dll_code;

	NOTICE("cvx16_dll_cal_status\n");
	ddr_debug_wr32(0x37);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x3018 + PHYD_BASE_ADDR);
	NOTICE("param_phyd_to_reg_rx_dll_code0= %x ...\n", get_bits_from_value(rddata, 7, 0));
	NOTICE("param_phyd_to_reg_rx_dll_code1= %x ...\n", get_bits_from_value(rddata, 15, 8));
	NOTICE("param_phyd_to_reg_rx_dll_code2= %x ...\n", get_bits_from_value(rddata, 23, 16));
	NOTICE("param_phyd_to_reg_rx_dll_code3= %x ...\n", get_bits_from_value(rddata, 31, 24));

	rddata = mmio_rd32(0x301c + PHYD_BASE_ADDR);
	NOTICE("param_phyd_to_reg_rx_dll_max= %x ...\n", get_bits_from_value(rddata, 7, 0));
	NOTICE("param_phyd_to_reg_rx_dll_min= %x ...\n", get_bits_from_value(rddata, 15, 8));

	rddata = mmio_rd32(0x3020 + PHYD_BASE_ADDR);
	NOTICE("param_phyd_to_reg_tx_dll_code0= %x ...\n", get_bits_from_value(rddata, 7, 0));
	NOTICE("param_phyd_to_reg_tx_dll_code1= %x ...\n", get_bits_from_value(rddata, 15, 8));
	NOTICE("param_phyd_to_reg_tx_dll_code2= %x ...\n", get_bits_from_value(rddata, 23, 16));
	NOTICE("param_phyd_to_reg_tx_dll_code3= %x ...\n", get_bits_from_value(rddata, 31, 24));

	rddata = mmio_rd32(0x3024 + PHYD_BASE_ADDR);
	NOTICE("param_phyd_to_reg_tx_dll_max= %x ...\n", get_bits_from_value(rddata, 7, 0));
	NOTICE("param_phyd_to_reg_tx_dll_min= %x ...\n", get_bits_from_value(rddata, 15, 8));

	err_cnt = 0;
	rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
	rx_dll_code = get_bits_from_value(rddata, 15, 8);
	tx_dll_code = get_bits_from_value(rddata, 31, 24);
#ifdef _mem_freq_2133
	if (!((rx_dll_code > 0x26) && (rx_dll_code < 0x2b))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x26~0x2b --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x26) && (tx_dll_code < 0x2b))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x26~0x2b --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_3200
	if (!((rx_dll_code > 0x15) && (rx_dll_code < 0x22))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x15~0x22 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x15) && (tx_dll_code < 0x22))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x15~0x22 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_1866
	if (!((rx_dll_code > 0x2b) && (rx_dll_code < 0x30))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x2b~0x30 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x2b) && (tx_dll_code < 0x30))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x2b~0x30 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_1600
	if (!((rx_dll_code > 0x2a) && (rx_dll_code < 0x44))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x2a~0x44 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x2a) && (tx_dll_code < 0x44))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x2a~0x44 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_2400
	if (!((rx_dll_code > 0x21) && (rx_dll_code < 0x25))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x21~0x25 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x21) && (tx_dll_code < 0x25))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x21~0x25 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_2666
	if (!((rx_dll_code > 0x1e) && (rx_dll_code < 0x22))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x1e~0x22 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x1e) && (tx_dll_code < 0x22))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x1e~0x22 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_1333
	if (!((rx_dll_code > 0x3d) && (rx_dll_code < 0x44))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x3e~0x42 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x3d) && (tx_dll_code < 0x44))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x3e~0x42 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
#ifdef _mem_freq_1066
	if (!((rx_dll_code > 0x4e) && (rx_dll_code < 0x52))) {
		NOTICE("Info!! rx_dll_code dly_sel result fail, not 0x4e~0x52 --%x--\n", rx_dll_code);

		err_cnt = err_cnt + 1;
	}
	if (!((tx_dll_code > 0x4e) && (tx_dll_code < 0x52))) {
		NOTICE("Info!! tx_dll_code dly_sel result fail, not 0x4e~0x52 --%x--\n", tx_dll_code);

		err_cnt = err_cnt + 1;
	}
#endif
	/***************************************************************
	 *      FINISHED!
	 ***************************************************************/
	if (err_cnt != 0x0) {
		NOTICE("*****************************************\n");
		NOTICE("DLL_CAL ERR!!! err_cnt = %x...\n", err_cnt);
		NOTICE("*****************************************\n");
	} else {
		NOTICE("*****************************************\n");
		NOTICE("DLL_CAL PASS!!!\n");
		NOTICE("*****************************************\n");
	}
#endif //DBG_SHMOO || DDR_SIM
}

void cvx16_zqcal_status(void)
{
	uint32_t zq_drvn;
	uint32_t zq_drvp;

	uartlog("cvx16_zqcal_status\n");
	ddr_debug_wr32(0x36);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
	zq_drvn = get_bits_from_value(rddata, 20, 16);
	zq_drvp = get_bits_from_value(rddata, 28, 24);
	if ((zq_drvn >= 0x07) && (zq_drvn <= 0x9)) {
		KC_MSG("ZQ Complete ... param_phya_reg_tx_zq_drvn = %x\n", zq_drvn);
	} else {
		KC_MSG("ZQ Complete ... INFO! param_phya_reg_tx_zq_drvn != 0b01000 +- 1, read:value: %x\n",
		       zq_drvn);
	}
	if ((zq_drvp >= 0x07) && (zq_drvp <= 0x9)) {
		KC_MSG("ZQ Complete ... param_phya_reg_tx_zq_drvp = %x\n", zq_drvp);
	} else {
		KC_MSG("ZQ Complete ... INFO! param_phya_reg_tx_zq_drvp != 0b01000 +- 1, read value: %x\n",
		       zq_drvp);
	}
}

void cvx16_training_status(void)
{
	uint32_t i;

	uartlog("cvx16_training_status\n");
	// wrlvl
	rddata = mmio_rd32(0x0A14 + PHYD_BASE_ADDR);
	uartlog("byte0 tx dqs shift/delay_dqsn/delay_dqsp = %x\n", rddata);

	rddata = mmio_rd32(0x0A54 + PHYD_BASE_ADDR);
	uartlog("byte1 tx dqs shift/delay_dqsn/delay_dqsp = %x\n", rddata);

	// rdglvl
	rddata = mmio_rd32(0x0B0C + PHYD_BASE_ADDR);
	uartlog("byte0 mask shift/delay = %x\n", rddata);

	rddata = mmio_rd32(0x0B3C + PHYD_BASE_ADDR);
	uartlog("byte1 mask shift/delay = %x\n", rddata);

	// rdlvl
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x3180 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq0_rise_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq1_rise_le = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq2_rise_le = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq3_rise_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3184 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq4_rise_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq5_rise_le = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq6_rise_le = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq7_rise_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3188 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq8_rise_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x318C + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq0_rise_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq1_rise_te = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq2_rise_te = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq3_rise_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3190 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq4_rise_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq5_rise_te = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq6_rise_te = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq7_rise_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x3194 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq8_rise_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x3198 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq0_fall_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq1_fall_le = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq2_fall_le = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq3_fall_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x319C + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq4_fall_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq5_fall_le = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq6_fall_le = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq7_fall_le = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x31A0 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq8_fall_le = %x\n", i, get_bits_from_value(rddata, 7, 0));

		rddata = mmio_rd32(0x31A4 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq0_fall_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq1_fall_te = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq2_fall_te = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq3_fall_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x31A8 + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq4_fall_te = %x\n", i, get_bits_from_value(rddata, 7, 0));

		KC_MSG("rdlvl_byte%x_dq5_fall_te = %x\n", i, get_bits_from_value(rddata, 15, 8));

		KC_MSG("rdlvl_byte%x_dq6_fall_te = %x\n", i, get_bits_from_value(rddata, 23, 16));

		KC_MSG("rdlvl_byte%x_dq7_fall_te = %x\n", i, get_bits_from_value(rddata, 31, 24));

		rddata = mmio_rd32(0x31AC + i * 0x40 + PHYD_BASE_ADDR);
		KC_MSG("rdlvl_byte%x_dq8_fall_te = %x\n", i, get_bits_from_value(rddata, 7, 0));
	}
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B00 + i * 0x30 + PHYD_BASE_ADDR);
		uartlog("byte%x deskewdq3210 = %x\n", i, rddata);

		rddata = mmio_rd32(0x0B04 + i * 0x30 + PHYD_BASE_ADDR);
		uartlog("byte%x deskewdq7654 = %x\n", i, rddata);

		rddata = mmio_rd32(0x0B08 + i * 0x30 + PHYD_BASE_ADDR);
		uartlog("byte%x rdqspos/neg/deskewdq8 = %x\n", i, rddata);
	}
	// rdvld
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B14 + i * 0x30 + PHYD_BASE_ADDR);
		uartlog("byte%x_rdvld = %x\n", i, get_bits_from_value(rddata, 20, 16));
	}
	// trig_lvl
	for (i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR);
		uartlog("byte%x trig_lvl_dq = %x, trig_lvl_dqs = %x\n", i, get_bits_from_value(rddata, 4, 0),
		       get_bits_from_value(rddata, 20, 16));
	}
	// wdqlvl
	for (int i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x3280 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq0_le = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq1_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3284 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq2_le = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq3_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3288 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq4_le = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq5_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x328C + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq6_le = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq7_le = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3290 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq8_le = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq0_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3294 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq1_te = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq2_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x3298 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq3_te = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq4_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x329C + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq5_te = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq6_te = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x32A0 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq7_te = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq8_te = %x\n", i, get_bits_from_value(rddata, 29, 16));
	}
	// wdq shift
	for (int i = 0; i < 2; i = i + 1) {
		rddata = mmio_rd32(0x0A00 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq0 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq1 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A04 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq2 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq3 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A08 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq4 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq5 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A0C + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq6 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));

		KC_MSG("piwdqlvl_byte%x_dq7 shift/delay = %x\n", i, get_bits_from_value(rddata, 29, 16));

		rddata = mmio_rd32(0x0A10 + 0x40 * i + PHYD_BASE_ADDR);
		KC_MSG("piwdqlvl_byte%x_dq8 shift/delay = %x\n", i, get_bits_from_value(rddata, 13, 0));
	}
	rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
	KC_MSG("piwdqlvl tx vref = %x\n", get_bits_from_value(rddata, 20, 16));
}

void cvx16_setting_check(void)
{
	uint32_t dfi_tphy_wrlat;
	uint32_t dfi_tphy_wrdata;
	uint32_t dfi_t_rddata_en;
	uint32_t dfi_t_ctrl_delay;
	uint32_t dfi_t_wrdata_delay;
	uint32_t phy_reg_version;

	uartlog("cvx16_setting_check\n");
	ddr_debug_wr32(0x0a);
	ddr_debug_num_write();
	phy_reg_version = mmio_rd32(0x3000 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(cfg_base + 0x190);
	dfi_tphy_wrlat = get_bits_from_value(rddata, 5, 0); // DFITMG0.dfi_tphy_wrlat
	dfi_tphy_wrdata = get_bits_from_value(rddata, 13, 8); // DFITMG0.dfi_tphy_wrdata
	dfi_t_rddata_en = get_bits_from_value(rddata, 22, 16); // DFITMG0.dfi_t_rddata_en
	dfi_t_ctrl_delay = get_bits_from_value(rddata, 29, 24); // DFITMG0.dfi_t_ctrl_delay
	rddata = mmio_rd32(cfg_base + 0x194);
	dfi_t_wrdata_delay = get_bits_from_value(rddata, 20, 16); // DFITMG1.dfi_t_wrdata_delay
	KC_MSG("phy_reg_version = %x, dfi_t_ctrl_delay = %x, dfi_t_rddata_en = %x\n", phy_reg_version,
	       dfi_t_ctrl_delay, dfi_t_rddata_en);

	KC_MSG("dfi_tphy_wrlat  = %x, dfi_tphy_wrdata  = %x, dfi_t_wrdata_delay = %x\n", dfi_tphy_wrlat,
	       dfi_tphy_wrdata, dfi_t_wrdata_delay);

	if (dfi_t_ctrl_delay != 0x4) {
		KC_MSG("ERR !!! dfi_t_ctrl_delay not 0x4\n");
	}
#ifndef DDR2_3
#ifdef _mem_freq_2133
	if (dfi_tphy_wrlat != 0x6) {
		KC_MSG("ERR !!! dfi_tphy_wrlat not 0x6\n");
	}
	if (dfi_tphy_wrdata != 0x3) {
		KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
	}
	if (dfi_t_rddata_en != 0xa) {
		KC_MSG("ERR !!! dfi_t_rddata_en not 0xa\n");
	}
	if (dfi_t_wrdata_delay != 0x7) {
		KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
	}
#endif
#ifdef _mem_freq_1866
	if (dfi_tphy_wrlat != 0x5) {
		KC_MSG("ERR !!! dfi_tphy_wrlat not 0x5\n");
	}
	if (dfi_tphy_wrdata != 0x3) {
		KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
	}
	if (dfi_t_rddata_en != 0xa) {
		KC_MSG("ERR !!! dfi_t_rddata_en not 0xa\n");
	}
	if (dfi_t_wrdata_delay != 0x7) {
		KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
	}
#endif
#ifdef _mem_freq_1600
	if (dfi_tphy_wrlat != 0x4) {
		KC_MSG("ERR !!! dfi_tphy_wrlat not 0x4\n");
	}
	if (dfi_tphy_wrdata != 0x3) {
		KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
	}
	if (dfi_t_rddata_en != 0x8) {
		KC_MSG("ERR !!! dfi_t_rddata_en not 0x8\n");
	}
	if (dfi_t_wrdata_delay != 0x7) {
		KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
	}
#endif
#ifdef _mem_freq_1333
#ifdef ESMT_ETRON_1333
	if (dfi_tphy_wrlat != 0x4) {
		KC_MSG("ERR !!! dfi_tphy_wrlat not 0x4\n");
	}
	if (dfi_tphy_wrdata != 0x3) {
		KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
	}
	if (dfi_t_rddata_en != 0x7) {
		KC_MSG("ERR !!! dfi_t_rddata_en not 0x7\n");
	}
	if (dfi_t_wrdata_delay != 0x7) {
		KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
	}
#else
	if (dfi_tphy_wrlat != 0x2) {
		KC_MSG("ERR !!! dfi_tphy_wrlat not 0x2\n");
	}
	if (dfi_tphy_wrdata != 0x3) {
		KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
	}
	if (dfi_t_rddata_en != 0x5) {
		KC_MSG("ERR !!! dfi_t_rddata_en not 0x5\n");
	}
	if (dfi_t_wrdata_delay != 0x7) {
		KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
	}
#endif
#endif
#ifdef _mem_freq_1066
	if (dfi_tphy_wrlat != 0x2) {
		KC_MSG("ERR !!! dfi_tphy_wrlat not 0x2\n");
	}
	if (dfi_tphy_wrdata != 0x3) {
		KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
	}
	if (dfi_t_rddata_en != 0x5) {
		KC_MSG("ERR !!! dfi_t_rddata_en not 0x5\n");
	}
	if (dfi_t_wrdata_delay != 0x7) {
		KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
	}
#endif
#else
	if (get_ddr_type() == DDR_TYPE_DDR3) {//DDR3:1866

		if (dfi_tphy_wrlat != 0x5) {
			KC_MSG("ERR !!! dfi_tphy_wrlat not 0x5\n");
		}
		if (dfi_tphy_wrdata != 0x3) {
			KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
		}
		if (dfi_t_rddata_en != 0xa) {
			KC_MSG("ERR !!! dfi_t_rddata_en not 0xa\n");
		}
		if (dfi_t_wrdata_delay != 0x7) {
			KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
		}
	} else {
	#ifdef ESMT_ETRON_1333
		if (dfi_tphy_wrlat != 0x4) {
			KC_MSG("ERR !!! dfi_tphy_wrlat not 0x4\n");
		}
		if (dfi_tphy_wrdata != 0x3) {
			KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
		}
		if (dfi_t_rddata_en != 0x7) {
			KC_MSG("ERR !!! dfi_t_rddata_en not 0x7\n");
		}
		if (dfi_t_wrdata_delay != 0x7) {
			KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
		}
	#else
		if (dfi_tphy_wrlat != 0x2) {
			KC_MSG("ERR !!! dfi_tphy_wrlat not 0x2\n");
		}
		if (dfi_tphy_wrdata != 0x3) {
			KC_MSG("ERR !!! dfi_tphy_wrdata not 0x3\n");
		}
		if (dfi_t_rddata_en != 0x5) {
			KC_MSG("ERR !!! dfi_t_rddata_en not 0x5\n");
		}
		if (dfi_t_wrdata_delay != 0x7) {
			KC_MSG("ERR !!! dfi_t_wrdata_delay not 0x7\n");
		}
	#endif
	}
#endif
}

void cvx16_ddr_freq_change_htol(void)
{
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
#if defined(DDR3) || defined(DDR4) || defined(DDR2_3)
	uint32_t rtt_nom = 0;
#ifdef DDR4
	uint32_t rtt_park;
#endif //DDR4
	uint32_t rtt_wr = 0;
	uint32_t mwr_temp;
#endif //defined(DDR3) || defined(DDR4)
	uint32_t port_num;
	// Note: freq_change_htol need ctrl_low_patch first
	KC_MSG("HTOL Frequency Change Start\n");

	uartlog("cvx16_ddr_freq_change_htol\n");
	ddr_debug_wr32(0x38);
	ddr_debug_num_write();
	// TOP_REG_EN_PLL_SPEED_CHG =1, TOP_REG_NEXT_PLL_SPEED = 0b01, TOP_REG_CUR_PLL_SPEED=0b10
	rddata = mmio_rd32(0x4C + CV_DDR_PHYD_APB);
	// rddata[0]     = 1;        //TOP_REG_EN_PLL_SPEED_CHG
	// rddata[5:4]   = 0b10;    //TOP_REG_CUR_PLL_SPEED
	// rddata[9:8]   = 0b01;    //TOP_REG_NEXT_PLL_SPEED
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	rddata = modified_bits_by_value(rddata, 2, 5, 4);
	rddata = modified_bits_by_value(rddata, 1, 9, 8);
	mmio_wr32(0x4C + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_EN_PLL_SPEED_CHG = %x, TOP_REG_NEXT_PLL_SPEED = %x, TOP_REG_CUR_PLL_SPEED= %x ...\n",
	       get_bits_from_value(rddata, 0, 0), get_bits_from_value(rddata, 9, 8), get_bits_from_value(rddata, 5, 4));

	// clock gating disable
	cvx16_clk_gating_disable();
	// save lowpower setting
	rddata = mmio_rd32(cfg_base + 0x30);
	// en_dfi_dram_clk_disable = rddata[3];
	// powerdown_en            = rddata[1];
	// selfref_en              = rddata[0];
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 0 to PCTRL_n.port_en, without port 0
	// port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
	// 3. Set RFSHCTL3.dis_auto_refresh=1, to disable automatic refreshes
	rddata = mmio_rd32(cfg_base + 0x60);
	// rddata[0] = 0x1; //RFSHCTL3.dis_auto_refresh
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
// 5. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to disable RTT_NOM:
//    a. DDR3: Write ????to MR1[9], MR1[6] and MR1[2]
//    b. DDR4: Write ????to MR1[10:8]
// 6. For DDR4 only: Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to write ????to
//    MR5[8:6] to disable RTT_PARK
// 7. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to write ????to MR2[10:9], to
//    disable RTT_WR (and therefore disable dynamic ODT). This applies for both DDR3 and DDR4.
#ifdef DDR4
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// rtt_nom = rddata[10:8];
	rtt_nom = get_bits_from_value(rddata, 10, 8);
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 15, 11) << 11 | get_bits_from_value(rddata, 7, 0);
	// cvx16_synp_mrw(0x1,  {rddata[15:11], 0b000, rddata[7:0]});
	cvx16_synp_mrw(0x1, mwr_temp);
	uartlog("disable RTT_NOM DDR4: Write ????to MR1[10:8]\n");
	KC_MSG("disable RTT_NOM DDR4: Write ????to MR1[10:8]\n");

	// read mr5 @INIT6
	rddata = mmio_rd32(cfg_base + 0xe8);
	// rtt_park = rddata[8:6];
	rtt_park = get_bits_from_value(rddata, 8, 6);
	// cvx16_synp_mrw(0x5,  {rddata[15:9], 0b000, rddata[5:0]});
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 15, 9) << 9 | get_bits_from_value(rddata, 5, 0);
	cvx16_synp_mrw(0x5, mwr_temp);
	uartlog("write ????to MR5[8:6] to disable RTT_PARK\n");
	KC_MSG("write ????to MR5[8:6] to disable RTT_PARK\n");

	// read mr2 @INIT4
	rddata = mmio_rd32(cfg_base + 0xe0);
	// rtt_wr = rddata[25:24];
	rtt_wr = get_bits_from_value(rddata, 25, 24);
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 31, 26) << 10 | get_bits_from_value(rddata, 23, 16);
	// cvx16_synp_mrw(0x2,  {rddata[31:26], 0b00, rddata[23:16]});
	cvx16_synp_mrw(0x2, mwr_temp);
	uartlog("write ????to MR2[10:9], to disable RTT_WR\n");
	KC_MSG("write ????to MR2[10:9], to disable RTT_WR\n");

#endif
#ifdef DDR3
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// rtt_nom = {rddata[9], rddata[6], rddata[2]};
	rtt_nom = get_bits_from_value(rddata, 9, 9) << 3 | get_bits_from_value(rddata, 6, 6) |
		  get_bits_from_value(rddata, 2, 2);
	// cvx16_synp_mrw(0x1,  {rddata[15:10], 0b0, rddata[8:7], 0b0, rddata[5:3], 0b0, rddata[1:0]});
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 15, 10) << 10 | get_bits_from_value(rddata, 8, 7) << 7 |
		   get_bits_from_value(rddata, 5, 3) << 3 | get_bits_from_value(rddata, 1, 0);
	cvx16_synp_mrw(0x1, mwr_temp);
	uartlog("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");
	KC_MSG("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");

	// read mr2 @INIT4
	rddata = mmio_rd32(cfg_base + 0xe0);
	// rtt_wr = rddata[25:24];
	rtt_wr = get_bits_from_value(rddata, 25, 24);
	// cvx16_synp_mrw(0x2,  {rddata[31:26], 0b00, rddata[23:16]});
	cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 26) << 10 | get_bits_from_value(rddata, 23, 16));
	uartlog("write ????to MR2[10:9], to disable RTT_WR\n");
	KC_MSG("write ????to MR2[10:9], to disable RTT_WR\n");

#endif
#ifdef DDR2
	KC_MSG("DDR2\n");

#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		// read mr1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// rtt_nom = {rddata[9], rddata[6], rddata[2]};
		rtt_nom = get_bits_from_value(rddata, 9, 9) << 3 | get_bits_from_value(rddata, 6, 6) |
			get_bits_from_value(rddata, 2, 2);
		// cvx16_synp_mrw(0x1,  {rddata[15:10], 0b0, rddata[8:7], 0b0, rddata[5:3], 0b0, rddata[1:0]});
		mwr_temp = 0;
		mwr_temp = get_bits_from_value(rddata, 15, 10) << 10 | get_bits_from_value(rddata, 8, 7) << 7 |
			get_bits_from_value(rddata, 5, 3) << 3 | get_bits_from_value(rddata, 1, 0);
		cvx16_synp_mrw(0x1, mwr_temp);
		uartlog("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");
		KC_MSG("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");

		// read mr2 @INIT4
		rddata = mmio_rd32(cfg_base + 0xe0);
		// rtt_wr = rddata[25:24];
		rtt_wr = get_bits_from_value(rddata, 25, 24);
		// cvx16_synp_mrw(0x2,  {rddata[31:26], 0b00, rddata[23:16]});
		cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 26) << 10 | get_bits_from_value(rddata, 23, 16));
		uartlog("write ????to MR2[10:9], to disable RTT_WR\n");
		KC_MSG("write ????to MR2[10:9], to disable RTT_WR\n");
	}
	if (get_ddr_type() == DDR_TYPE_DDR2) {
		KC_MSG("DDR2\n");
	}
#endif
	// 20200206
	//  3. Set RFSHCTL3.dis_auto_refresh=1, to disable automatic refreshes
	rddata = mmio_rd32(cfg_base + 0x60);
	// rddata[0] = 0x1; //RFSHCTL3.dis_auto_refresh
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
// 8. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to disable the DLL. The timing of
// this MRS is automatically handled by the uMCTL2.
// a. DDR3: Write ????to MR1[0]
// b. DDR4: Write ????to MR1[0]
#ifdef DDR4
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// DDR4: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b0});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 1) << 1);
	uartlog("DDR4: Write ????to MR1[0]\n");
	KC_MSG("DDR4: Write ????to MR1[0]\n");

#endif
#ifdef DDR3
	rddata = mmio_rd32(cfg_base + 0x30);
	// rddata[3] = 0x0; //PWRCTL.en_dfi_dram_clk_disable
	// rddata[1] = 0x0; //PWRCTL.powerdown_en
	// rddata[0] = 0x0; //PWRCTL.selfref_en
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// DDR3: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	mwr_temp = modified_bits_by_value(rddata, 1, 0, 0);
	// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 1);
	cvx16_synp_mrw(0x1, mwr_temp);
#endif
#ifdef DDR2
	rddata = mmio_rd32(cfg_base + 0x30);
	// rddata[3] = 0x0; //PWRCTL.en_dfi_dram_clk_disable
	// rddata[1] = 0x0; //PWRCTL.powerdown_en
	// rddata[0] = 0x0; //PWRCTL.selfref_en
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	////read EMR1 @INIT3
	// rddata = mmio_rd32(cfg_base+0xdc);
	////DDR2: Write ????to EMR1
	////cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 1);
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		rddata = mmio_rd32(cfg_base + 0x30);
		// rddata[3] = 0x0; //PWRCTL.en_dfi_dram_clk_disable
		// rddata[1] = 0x0; //PWRCTL.powerdown_en
		// rddata[0] = 0x0; //PWRCTL.selfref_en
		rddata = modified_bits_by_value(rddata, 0, 3, 3);
		rddata = modified_bits_by_value(rddata, 0, 1, 1);
		rddata = modified_bits_by_value(rddata, 0, 0, 0);
		mmio_wr32(cfg_base + 0x30, rddata);
		// read mr1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// DDR3: Write ????to MR1[0]
		// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
		mwr_temp = modified_bits_by_value(rddata, 1, 0, 0);
		// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 1);
		cvx16_synp_mrw(0x1, mwr_temp);
	}
	if (get_ddr_type() == DDR_TYPE_DDR2) {
		rddata = mmio_rd32(cfg_base + 0x30);
		// rddata[3] = 0x0; //PWRCTL.en_dfi_dram_clk_disable
		// rddata[1] = 0x0; //PWRCTL.powerdown_en
		// rddata[0] = 0x0; //PWRCTL.selfref_en
		rddata = modified_bits_by_value(rddata, 0, 3, 3);
		rddata = modified_bits_by_value(rddata, 0, 1, 1);
		rddata = modified_bits_by_value(rddata, 0, 0, 0);
		mmio_wr32(cfg_base + 0x30, rddata);
		////read EMR1 @INIT3
		// rddata = mmio_rd32(cfg_base+0xdc);
		////DDR2: Write ????to EMR1
		////cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
		// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 1);
	}
#endif
	// 9. Put the SDRAM into self-refresh mode by setting PWRCTL.selfref_sw = 1, and polling STAT.operating_
	// mode to ensure the DDRC has entered self-refresh.
	// Write 1 to PWRCTL.selfref_sw
	rddata = mmio_rd32(cfg_base + 0x30);
	// rddata[5] = 0x1; //PWRCTL.selfref_sw
	// rddata[3] = 0x0; //PWRCTL.en_dfi_dram_clk_disable
	// rddata[1] = 0x0; //PWRCTL.powerdown_en
	// rddata[0] = 0x0; //PWRCTL.selfref_en
	rddata = modified_bits_by_value(rddata, 1, 5, 5);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
// Poll STAT.selfref_type= 2b10
// Poll STAT.selfref_state = 0b10 (LPDDR4 only)
#ifndef LP4
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.selfref_state = 0b10\n");

		if (get_bits_from_value(rddata, 5, 4) == 2) {
			break;
		}
	}
	// 11. Set the MSTR.dll_off_mode = 1.
	rddata = mmio_rd32(cfg_base + 0x0);
	// rddata[15] = 0x1;
	rddata = modified_bits_by_value(rddata, 1, 15, 15);
	mmio_wr32(cfg_base + 0x0, rddata);
#else
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.selfref_type= 2b10\n");

		if ((get_bits_from_value(rddata, 9, 8) == 0x2)) {
			break;
		}
	}
#endif
	// Change PLL frequency
	cvx16_chg_pll_freq();
	KC_MSG("cvx16_chg_pll_freq done ...\n");

	// test only when dll_cal is not use
	//     #ifdef DDR3
	//         //param_phyd_dll_sw_code    <= `PI_SD int_regin[23:16]
	//         //param_phyd_dll_sw_code_mode    <= `PI_SD int_regin[8];
	//         REGREAD(169 + PHY_BASE_ADDR, rddata);
	//         rddata[23:16] = 0x54;
	//         rddata[8] = 0b1;
	//         REGWR  (169 + PHY_BASE_ADDR, rddata, 0);
	uartlog("param_phyd_dll_sw_code\n");
	//        //param_phyd_dll_sw_clr          <= `PI_SD int_regin[7];
	//        rddata[7] = 0b1;
	//        REGWR  (169 + PHY_BASE_ADDR, rddata, 0);
	//        rddata[7] = 0b0;
	//        REGWR  (169 + PHY_BASE_ADDR, rddata, 0);
	//    #endif
	// dll_cal
	cvx16_dll_cal();
	KC_MSG("Do DLLCAL done ...\n");

	// refresh requirement
	// Program controller
	rddata = mmio_rd32(cfg_base + 0x64);
	// rddata[27:16] = rddata[27:17];
	rddata = modified_bits_by_value(rddata, get_bits_from_value(rddata, 27, 17), 27, 16);
	// rddata[9:0] = rddata[9:1] + rddata[0];
	rddata = modified_bits_by_value(rddata, get_bits_from_value(rddata, 9, 1) + get_bits_from_value(rddata, 0, 0),
					9, 0);
	mmio_wr32(cfg_base + 0x64, rddata);
	rddata = mmio_rd32(cfg_base + 0x68);
	// rddata[23:16] = rddata[23:17] + rddata[16];
	rddata = modified_bits_by_value(
		rddata, get_bits_from_value(rddata, 23, 17) + get_bits_from_value(rddata, 16, 16), 23, 16);
	mmio_wr32(cfg_base + 0x68, rddata);
//    // program phyupd_mask
//    rddata = mmio_rd32(0x0800a504);
//    //rddata[27:16] = rddata[27:17] + rddata[16];
//    rddata = modified_bits_by_value(rddata, get_bits_from_value(rddata, 27, 17) +
//                                    get_bits_from_value(rddata, 16, 16), 27, 16);
//    mmio_wr32(0x0800a504, rddata);
//    rddata = mmio_rd32(0x0800a500);
//    //rddata[31:16] = rddata[31:17];
//    rddata=modified_bits_by_value(rddata, get_bits_from_value(rddata, 31, 17), 31, 16);
//    mmio_wr32(0x0800a500, rddata);
#ifndef LP4
	// 9. Set MSTR.dll_off_mode = 0
	rddata = mmio_rd32(cfg_base + 0x0);
	// rddata[15] = 0x0;
	rddata = modified_bits_by_value(rddata, 0, 15, 15);
	mmio_wr32(cfg_base + 0x0, rddata);
#endif
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.selfref_type = 2b00\n");

		if (get_bits_from_value(rddata, 5, 4) == 0) {
			break;
		}
	}
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.operating_mode for Normal Mode entry\n");

		if (get_bits_from_value(rddata, 1, 0) == 1) {
			break;
		}
	}
#ifdef DDR4
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// DDR4: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 1) << 1 | 1);
	uartlog("DDR4: Write ????to MR1[0]\n");
	KC_MSG("DDR4: Write ????to MR1[0]\n");

	// read mr0 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// 15. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to reset the DLL explicitly by
	// writing to MR0, bit A8. The timing of this MRS is automatically handled by the uMCTL2
	// opdelay(1).2us; //wait tDLLK ??
	// cvx16_synp_mrw(0x0,  {rddata[31:25], 0b1, rddata[23:16]});
	cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 | get_bits_from_value(rddata, 23, 16));
	uartlog("15. Perform an MRS command\n");
	KC_MSG("15. Perform an MRS command\n");

	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// cvx16_synp_mrw(0x1,  {rddata[15:11], rtt_nom[2:0], rddata[7:0]});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 11) << 11 | get_bits_from_value(rtt_nom, 2, 0) << 8 |
				    get_bits_from_value(rddata, 7, 0));
	uartlog("enable RTT_NOM\n");
	KC_MSG("enable RTT_NOM\n");

	// read mr5 @INIT6
	rddata = mmio_rd32(cfg_base + 0xe8);
	// cvx16_synp_mrw(0x5,  {rddata[15:9], rtt_park[2:0], rddata[5:0]});
	cvx16_synp_mrw(0x5, get_bits_from_value(rddata, 15, 9) << 9 | get_bits_from_value(rtt_park, 2, 0) << 6 |
				    get_bits_from_value(rddata, 5, 0));
	uartlog("enable RTT_PARK\n");
	KC_MSG("enable RTT_PARK\n");

	// read mr2 @INIT4
	rddata = mmio_rd32(cfg_base + 0xe0);
	// cvx16_synp_mrw(0x2,  {rddata[31:26], rtt_wr[1:0], rddata[23:16]});
	cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 26) << 11 | get_bits_from_value(rtt_wr, 1, 0) << 8 |
				    get_bits_from_value(rddata, 23, 16));
	uartlog("enable RTT_WR\n");
	KC_MSG("enable RTT_WR\n");

#endif
#ifdef DDR3
	////read mr1 @INIT3
	// rddata = mmio_rd32(cfg_base+0xdc);
	////DDR3: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b0});
	// read mr2 @INIT4
	rddata = mmio_rd32(cfg_base + 0xe0);
	// cvx16_synp_mrw(0x2,  {rddata[31:26], rtt_wr[1:0], rddata[23:16]});
	cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 26) << 11 | get_bits_from_value(rtt_wr, 1, 0) << 8 |
				    get_bits_from_value(rddata, 23, 16));
	uartlog("enable RTT_WR\n");
	KC_MSG("enable RTT_WR\n");

	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// cvx16_synp_mrw(0x1,
	//      {rddata[15:10], rtt_nom[2], rddata[8:7], rtt_nom[1], rddata[5:3], rtt_nom[0], rddata[1], 0b0});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 10) << 10 | get_bits_from_value(rtt_nom, 2, 2) << 9 |
				    get_bits_from_value(rddata, 8, 7) << 7 | get_bits_from_value(rtt_nom, 1, 1) << 5 |
				    get_bits_from_value(rddata, 5, 3) << 3 | get_bits_from_value(rtt_nom, 0, 0) << 2 |
				    get_bits_from_value(rddata, 1, 1) << 1 | 0b0);
	uartlog("enable RTT_NOM\n");
	KC_MSG("enable RTT_NOM %x\n", rtt_nom);

	// read mr0 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// 15. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to reset the DLL explicitly by
	// writing to MR0, bit A8. The timing of this MRS is automatically handled by the uMCTL2
	// cvx16_synp_mrw(0x0,  {rddata[31:25], 0b1, rddata[23:16]});
	cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 | get_bits_from_value(rddata, 23, 16));
	KC_MSG("15. Perform an MRS command\n");

	// opdelay(2).2us; //don't remove. wait tDLLK ??
	opdelay(2200);
#endif
#ifdef DDR2
	// reset the DLL
	// DDR2: Value to write to MR register. Bit 8 is for DLL and the setting here is ignored.
	// The uMCTL2 sets this bit appropriately.
	// read EMR1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	////DDR2: Write ????to EMR1
	////cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 0);
	// rddata = mmio_rd32(cfg_base+0xdc);
	cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 | get_bits_from_value(rddata, 23, 16));
	KC_MSG("15. Perform an MRS command\n");

#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		////read mr1 @INIT3
		// rddata = mmio_rd32(cfg_base+0xdc);
		////DDR3: Write ????to MR1[0]
		// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b0});
		// read mr2 @INIT4
		rddata = mmio_rd32(cfg_base + 0xe0);
		// cvx16_synp_mrw(0x2,  {rddata[31:26], rtt_wr[1:0], rddata[23:16]});
		cvx16_synp_mrw(0x2, get_bits_from_value(rddata, 31, 26) << 11 | get_bits_from_value(rtt_wr, 1, 0) << 8 |
						get_bits_from_value(rddata, 23, 16));
		uartlog("enable RTT_WR\n");
		KC_MSG("enable RTT_WR\n");

		// read mr1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// cvx16_synp_mrw(0x1,
		//      {rddata[15:10], rtt_nom[2], rddata[8:7], rtt_nom[1], rddata[5:3], rtt_nom[0], rddata[1], 0b0});
		cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 10) << 10 |
					get_bits_from_value(rtt_nom, 2, 2) << 9 |
					get_bits_from_value(rddata, 8, 7) << 7 |
					get_bits_from_value(rtt_nom, 1, 1) << 5 |
					get_bits_from_value(rddata, 5, 3) << 3 |
					get_bits_from_value(rtt_nom, 0, 0) << 2 |
					get_bits_from_value(rddata, 1, 1) << 1 | 0b0);
		uartlog("enable RTT_NOM\n");
		KC_MSG("enable RTT_NOM %x\n", rtt_nom);

		// read mr0 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// 15. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to reset the DLL explicitly by
		// writing to MR0, bit A8. The timing of this MRS is automatically handled by the uMCTL2
		// cvx16_synp_mrw(0x0,  {rddata[31:25], 0b1, rddata[23:16]});
		cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 |
						get_bits_from_value(rddata, 23, 16));
		KC_MSG("15. Perform an MRS command\n");

		// opdelay(2).2us; //don't remove. wait tDLLK ??
		opdelay(2200);
	}
	if (get_ddr_type() == DDR_TYPE_DDR2) {
		// reset the DLL
		// DDR2: Value to write to MR register. Bit 8 is for DLL and the setting here is ignored.
		// The uMCTL2 sets this bit appropriately.
		// read EMR1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		////DDR2: Write ????to EMR1
		////cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
		// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 0);
		// rddata = mmio_rd32(cfg_base+0xdc);
		cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 |
						get_bits_from_value(rddata, 23, 16));
		KC_MSG("15. Perform an MRS command\n");
	}
#endif
	rddata = mmio_rd32(cfg_base + 0x60);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	uartlog("restore selfref_en powerdown_en\n");
#ifdef DDR4
	rddata = mmio_rd32(0x0b0c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, (get_bits_from_value(rddata, 6, 0) >> 1), 6, 0);
	mmio_wr32(0x0b0c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0b3c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, (get_bits_from_value(rddata, 6, 0) >> 1), 6, 0);
	mmio_wr32(0x0b3c + PHYD_BASE_ADDR, rddata);
	cvx16_dll_sw_upd();
#endif
	uartlog("rdlvl_gate\n");
	//        KC_MSG("pi_rdlvl_gate_req done ...\n");

	//    #endif
	cvx16_clk_gating_enable();
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	rddata = mmio_rd32(0x4C + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(0x4C + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_EN_PLL_SPEED_CHG = %x, TOP_REG_NEXT_PLL_SPEED = %x, TOP_REG_CUR_PLL_SPEED= %x ...\n",
	       get_bits_from_value(rddata, 0, 0), get_bits_from_value(rddata, 9, 8), get_bits_from_value(rddata, 5, 4));

	KC_MSG("HTOL Frequency Change Finished\n");
}

void cvx16_ddr_freq_change_ltoh(void)
{
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
#if defined(DDR3) || defined(DDR4) || defined(DDR2_3)
	uint32_t rtt_nom = 0;
#ifdef DDR4
	uint32_t rtt_park;
#endif //DDR4
	uint32_t mwr_temp;
#endif // defined(DDR3) || defined(DDR4)

	uint32_t port_num;

	// Note: freq_change_ltoh need ctrl_low_patch first
	KC_MSG("LTOH Frequency Change Start\n");

	uartlog("cvx16_ddr_freq_change_ltoh\n");
	ddr_debug_wr32(0x39);
	ddr_debug_num_write();
	// TOP_REG_EN_PLL_SPEED_CHG =1, TOP_REG_NEXT_PLL_SPEED = 0b01, TOP_REG_CUR_PLL_SPEED=0b10
	rddata = mmio_rd32(0x4C + CV_DDR_PHYD_APB);
	// rddata[0]     = 1;        //TOP_REG_EN_PLL_SPEED_CHG
	// rddata[5:4]   = 0b01;    //TOP_REG_CUR_PLL_SPEED
	// rddata[9:8]   = 0b10;    //TOP_REG_NEXT_PLL_SPEED
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	rddata = modified_bits_by_value(rddata, 1, 5, 4);
	rddata = modified_bits_by_value(rddata, 2, 9, 8);
	mmio_wr32(0x4C + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_EN_PLL_SPEED_CHG = %x, TOP_REG_NEXT_PLL_SPEED = %x, TOP_REG_CUR_PLL_SPEED= %x ...\n",
	       get_bits_from_value(rddata, 0, 0), get_bits_from_value(rddata, 9, 8), get_bits_from_value(rddata, 5, 4));

	// clock gating disable
	cvx16_clk_gating_disable();
	// save lowpower setting
	rddata = mmio_rd32(cfg_base + 0x30);
	// en_dfi_dram_clk_disable = rddata[3];
	//  powerdown_en            = rddata[1];
	//  selfref_en              = rddata[0];
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	// Write 0 to PCTRL_n.port_en, without port 0
	// port number = 0,1,2,3
	port_num = 0x4;
	for (int i = 1; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x0);
	}
	// Poll PSTAT.rd_port_busy_n = 0
	// Poll PSTAT.wr_port_busy_n = 0
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x3fc);
		KC_MSG("Poll PSTAT.rd_port_busy_n =0\n");

		if (rddata == 0) {
			break;
		}
	}
// 3. Set RFSHCTL3.dis_auto_refresh=1, to disable automatic refreshes
//   rddata = mmio_rd32(cfg_base + 0x60);
////rddata[0] = 0x1; //RFSHCTL3.dis_auto_refresh
//  rddata = modified_bits_by_value(rddata, 1, 0, 0);
//  mmio_wr32(cfg_base + 0x60, rddata);
// 5. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to disable RTT_NOM:
//    a. DDR3: Write ????to MR1[9], MR1[6] and MR1[2]
//    b. DDR4: Write ????to MR1[10:8]
// 6. For DDR4 only: Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to write ????to
//    MR5[8:6] to disable RTT_PARK
#ifdef DDR4
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// rtt_nom = rddata[10:8];
	rtt_nom = get_bits_from_value(rddata, 10, 8);
	// cvx16_synp_mrw(0x1,  {rddata[15:11], 0b000, rddata[7:0]});
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 15, 11) << 11 | get_bits_from_value(rddata, 7, 0);
	cvx16_synp_mrw(0x1, mwr_temp);
	uartlog("disable RTT_NOM DDR4: Write ????to MR1[10:8]\n");
	KC_MSG("disable RTT_NOM DDR4: Write ????to MR1[10:8]\n");

	// read mr5 @INIT6
	rddata = mmio_rd32(cfg_base + 0xe8);
	// rtt_park = rddata[8:6];
	rtt_park = get_bits_from_value(rddata, 8, 6);
	// cvx16_synp_mrw(0x5,  {rddata[15:9], 0b000, rddata[5:0]});
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 15, 9) << 9 | get_bits_from_value(rddata, 5, 0);
	uartlog("write ????to MR5[8:6] to disable RTT_PARK\n");
	KC_MSG("write ????to MR5[8:6] to disable RTT_PARK\n");

#endif
#ifdef DDR3
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// rtt_nom = {rddata[9], rddata[6], rddata[2]};
	rtt_nom = get_bits_from_value(rddata, 9, 9) << 3 | get_bits_from_value(rddata, 6, 6) |
		  get_bits_from_value(rddata, 2, 2);
	// cvx16_synp_mrw(0x1,  {rddata[15:10], 0b0, rddata[8:7], 0b0, rddata[5:3], 0b0, rddata[1:0]});
	mwr_temp = 0;
	mwr_temp = get_bits_from_value(rddata, 15, 10) << 10 | get_bits_from_value(rddata, 8, 7) << 7 |
		   get_bits_from_value(rddata, 5, 3) << 3 | get_bits_from_value(rddata, 1, 0);
	cvx16_synp_mrw(0x1, mwr_temp);
	uartlog("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");
	KC_MSG("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");

#endif
#ifdef DDR2
	KC_MSG("DDR2\n");

#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		// read mr1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// rtt_nom = {rddata[9], rddata[6], rddata[2]};
		rtt_nom = get_bits_from_value(rddata, 9, 9) << 3 | get_bits_from_value(rddata, 6, 6) |
			get_bits_from_value(rddata, 2, 2);
		// cvx16_synp_mrw(0x1,  {rddata[15:10], 0b0, rddata[8:7], 0b0, rddata[5:3], 0b0, rddata[1:0]});
		mwr_temp = 0;
		mwr_temp = get_bits_from_value(rddata, 15, 10) << 10 | get_bits_from_value(rddata, 8, 7) << 7 |
			get_bits_from_value(rddata, 5, 3) << 3 | get_bits_from_value(rddata, 1, 0);
		cvx16_synp_mrw(0x1, mwr_temp);
		uartlog("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");
		KC_MSG("disable RTT_NOM DDR3: Write ????to MR1[9], MR1[6] and MR1[2]\n");
	}
	if (get_ddr_type() == DDR_TYPE_DDR2)	{
		KC_MSG("DDR2\n");
	}
#endif
	// 20200206
	//  3. Set RFSHCTL3.dis_auto_refresh=1, to disable automatic refreshes
	rddata = mmio_rd32(cfg_base + 0x60);
	// rddata[0] = 0x1; //RFSHCTL3.dis_auto_refresh
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
// 8. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to disable the DLL. The timing of
// this MRS is automatically handled by the uMCTL2.
// a. DDR3: Write ????to MR1[0]
// b. DDR4: Write ????to MR1[0]
#ifdef DDR4
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// DDR4: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b0});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 1) << 1 | 0);
	uartlog("DDR4: Write ????to MR1[0]\n");
	KC_MSG("DDR4: Write ????to MR1[0]\n");

#endif
#ifdef DDR3
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// DDR3: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 1) << 1 | 1);
#endif
#ifdef DDR2
	KC_MSG("DDR2\n");

#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		// read mr1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// DDR3: Write ????to MR1[0]
		// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
		cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 1) << 1 | 1);
	}
	if (get_ddr_type() == DDR_TYPE_DDR2) {
		KC_MSG("DDR2\n");
	}
#endif
	// 9. Put the SDRAM into self-refresh mode by setting PWRCTL.selfref_sw = 1, and polling STAT.operating_
	// mode to ensure the DDRC has entered self-refresh.
	// Write 1 to PWRCTL.selfref_sw
	rddata = mmio_rd32(cfg_base + 0x30);
	// rddata[5] = 0x1; //PWRCTL.selfref_sw
	// rddata[3] = 0x1; //PWRCTL.en_dfi_dram_clk_disable
	// rddata[1] = 0x0; //PWRCTL.powerdown_en
	// rddata[0] = 0x0; //PWRCTL.selfref_en
	rddata = modified_bits_by_value(rddata, 1, 5, 5);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
// Poll STAT.selfref_type= 2b10
// Poll STAT.selfref_state = 0b10 (LPDDR4 only)
#ifndef LP4
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.selfref_state = 0b10\n");

		if (get_bits_from_value(rddata, 5, 4) == 2) {
			break;
		}
	}
	// 11. Set the MSTR.dll_off_mode = 1.
	rddata = mmio_rd32(cfg_base + 0x0);
	// rddata[15] = 0x1;
	rddata = modified_bits_by_value(rddata, 1, 15, 15);
	mmio_wr32(cfg_base + 0x0, rddata);
#else
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.selfref_type= 2b10\n");

		if (get_bits_from_value(rddata, 9, 8) == 2) {
			break;
		}
	}
#endif
	// Change PLL frequency
	cvx16_chg_pll_freq();
	KC_MSG("cvx16_chg_pll_freq done ...\n");

	cvx16_dll_cal();
	// refresh requirement
	// Program controller
	rddata = mmio_rd32(cfg_base + 0x64);
	// rddata[27:16] = {rddata[26:16], 0b0};
	// rddata[9:0] = {rddata[8:0], 0b0};
	rddata = modified_bits_by_value(rddata, get_bits_from_value(rddata, 26, 16) << 1, 27, 16);
	mmio_wr32(cfg_base + 0x64, rddata);
	rddata = mmio_rd32(cfg_base + 0x68);
	// rddata[23:16] = {rddata[22:16], 0b0};
	rddata = modified_bits_by_value(rddata, get_bits_from_value(rddata, 22, 16) << 1, 23, 16);
	mmio_wr32(cfg_base + 0x68, rddata);
//    // program phyupd_mask
//    rddata = mmio_rd32(0x0800a504);
//    //rddata[27:16] = {rddata[26:16],0b0};
//    rddata=modified_bits_by_value(rddata, get_bits_from_value(rddata, 26, 16)<<1, 27, 16);
//    mmio_wr32(0x0800a504, rddata);
//    rddata = mmio_rd32(0x0800a500);
//    //rddata[31:16] = {rddata[30:16],0b0};
//    rddata = modified_bits_by_value(rddata, get_bits_from_value(rddata, 30, 16)<<1, 31, 16);
//    mmio_wr32(0x0800a500, rddata);
#ifndef LP4
	// 9. Set MSTR.dll_off_mode = 0
	rddata = mmio_rd32(cfg_base + 0x0);
	// rddata[15] = 0x0;
	rddata = modified_bits_by_value(rddata, 0, 15, 15);
	mmio_wr32(cfg_base + 0x0, rddata);
#endif
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.selfref_type = 0b00\n");

		if (get_bits_from_value(rddata, 5, 4) == 0) {
			break;
		}
	}
	while (1) {
		rddata = mmio_rd32(cfg_base + 0x4);
		KC_MSG("Poll STAT.operating_mode for Normal Mode entry\n");

		if (get_bits_from_value(rddata, 1, 0) == 1) {
			break;
		}
	}
#ifdef DDR4
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// DDR4: Write 1 to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 1) << 1 | 1);
	uartlog("DDR4: Write 1 to MR1[0]\n");
	KC_MSG("DDR4: Write 1 to MR1[0]\n");

	// read mr0 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// 15. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to reset the DLL explicitly by
	// writing to MR0, bit A8. The timing of this MRS is automatically handled by the uMCTL2
	// opdelay(0).7us; //wait tDLLK ??
	// cvx16_synp_mrw(0x0,  {rddata[31:25], 0b1, rddata[23:16]});
	cvx16_synp_mrw(0x0,
		       ((get_bits_from_value(rddata, 31, 25) << 9) | (0b1 << 8) | get_bits_from_value(rddata, 23, 16)));
	uartlog("15. Perform an MRS command\n");
	KC_MSG("15. Perform an MRS command\n");

	// 17. Re-enable RTT_PARK (DDR4 only) and RTT_NOM by performing MRS commands (if required).
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// cvx16_synp_mrw(0x1,  {rddata[15:11], rtt_nom[2:0], rddata[7:0]});
	cvx16_synp_mrw(0x1, ((get_bits_from_value(rddata, 15, 11) << 11) | (get_bits_from_value(rtt_nom, 2, 0) << 8) |
			     get_bits_from_value(rddata, 7, 0)));
	uartlog("Re-enable RTT_NOM\n");
	KC_MSG("Re-enable RTT_NOM\n");

	// read mr5 @INIT6
	rddata = mmio_rd32(cfg_base + 0xe8);
	// cvx16_synp_mrw(0x5,  {rddata[15:9], rtt_park[2:0], rddata[5:0]});
	cvx16_synp_mrw(0x5, ((get_bits_from_value(rddata, 15, 9) << 9) | (get_bits_from_value(rtt_park, 2, 0) << 6) |
			     get_bits_from_value(rddata, 5, 0)));
	uartlog("Re-enable RTT_PARK\n");
	KC_MSG("Re-enable RTT_PARK\n");

#endif
#ifdef DDR3
	////read mr1 @INIT3
	// rddata = mmio_rd32(cfg_base+0xdc);
	////DDR3: Write ????to MR1[0]
	// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b0});
	//  17. Re-enable RTT_PARK (DDR4 only) and RTT_NOM by performing MRS commands (if required).
	// read mr1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	//cvx16_synp_mrw(0x1,
	//		{rddata[15:10], rtt_nom[2], rddata[8:7], rtt_nom[1], rddata[5:3], rtt_nom[0], rddata[1], 0b0});
	cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 10) << 10 | get_bits_from_value(rtt_nom, 2, 2) << 9 |
				    get_bits_from_value(rddata, 8, 7) << 7 | get_bits_from_value(rtt_nom, 1, 1) << 5 |
				    get_bits_from_value(rddata, 5, 3) << 3 | get_bits_from_value(rtt_nom, 0, 0) << 2 |
				    get_bits_from_value(rddata, 1, 1) << 1 | 0b0);
	uartlog("Re-enable RTT_NOM\n");
	KC_MSG("Re-enable RTT_NOM\n");

	// read mr0 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	// 15. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to reset the DLL explicitly by
	// writing to MR0, bit A8. The timing of this MRS is automatically handled by the uMCTL2
	// cvx16_synp_mrw(0x0,  {rddata[31:25], 0b1, rddata[23:16]});
	cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 | get_bits_from_value(rddata, 23, 16));
	opdelay(600); // don't remove. wait tDLLK ??
#endif
#ifdef DDR2
	// reset the DLL
	// DDR2: Value to write to MR register. Bit 8 is for DLL and the setting here is ignored.
	// The uMCTL2 sets this bit appropriately.
	// read EMR1 @INIT3
	rddata = mmio_rd32(cfg_base + 0xdc);
	////DDR2: Write ????to EMR1
	////cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
	// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 0);
	// rddata = mmio_rd32(cfg_base+0xdc);
	cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 | get_bits_from_value(rddata, 23, 16));
	KC_MSG("15. Perform an MRS command\n");

#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		////read mr1 @INIT3
		// rddata = mmio_rd32(cfg_base+0xdc);
		////DDR3: Write ????to MR1[0]
		// cvx16_synp_mrw(0x1,  {rddata[15:1], 0b0});
		//  17. Re-enable RTT_PARK (DDR4 only) and RTT_NOM by performing MRS commands (if required).
		// read mr1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		//cvx16_synp_mrw(0x1,
		//{rddata[15:10], rtt_nom[2], rddata[8:7], rtt_nom[1], rddata[5:3], rtt_nom[0], rddata[1], 0b0});
		cvx16_synp_mrw(0x1, get_bits_from_value(rddata, 15, 10) << 10 |
				get_bits_from_value(rtt_nom, 2, 2) << 9 |
				get_bits_from_value(rddata, 8, 7) << 7 |
				get_bits_from_value(rtt_nom, 1, 1) << 5 |
				get_bits_from_value(rddata, 5, 3) << 3 |
				get_bits_from_value(rtt_nom, 0, 0) << 2 |
				get_bits_from_value(rddata, 1, 1) << 1 | 0b0);
		uartlog("Re-enable RTT_NOM\n");
		KC_MSG("Re-enable RTT_NOM\n");

		// read mr0 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		// 15. Perform an MRS command (using MRCTRL0 and MRCTRL1 registers) to reset the DLL explicitly by
		// writing to MR0, bit A8. The timing of this MRS is automatically handled by the uMCTL2
		// cvx16_synp_mrw(0x0,  {rddata[31:25], 0b1, rddata[23:16]});
		cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 |
					get_bits_from_value(rddata, 23, 16));
		opdelay(600); // don't remove. wait tDLLK ??
	}
	if (get_ddr_type() == DDR_TYPE_DDR2)	{
		// reset the DLL
		// DDR2: Value to write to MR register. Bit 8 is for DLL and the setting here is ignored.
		// The uMCTL2 sets this bit appropriately.
		// read EMR1 @INIT3
		rddata = mmio_rd32(cfg_base + 0xdc);
		////DDR2: Write ????to EMR1
		////cvx16_synp_mrw(0x1,  {rddata[15:1], 0b1});
		// cvx16_synp_mrw(0x1,  get_bits_from_value(rddata, 15, 1)<<1 | 0);
		// rddata = mmio_rd32(cfg_base+0xdc);
		cvx16_synp_mrw(0x0, get_bits_from_value(rddata, 31, 25) << 9 | 0b1 << 8 |
					get_bits_from_value(rddata, 23, 16));
		KC_MSG("15. Perform an MRS command\n");
	}
#endif
	rddata = mmio_rd32(cfg_base + 0x60);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
	rddata = mmio_rd32(cfg_base + 0x30);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3);
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1);
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	uartlog("restore selfref_en powerdown_en\n");
#ifdef DDR4
	rddata = mmio_rd32(0x0b0c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, (get_bits_from_value(rddata, 6, 0) << 1), 6, 0);
	mmio_wr32(0x0b0c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0b3c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, (get_bits_from_value(rddata, 6, 0) << 1), 6, 0);
	mmio_wr32(0x0b3c + PHYD_BASE_ADDR, rddata);
	cvx16_dll_sw_upd();
#endif
	cvx16_clk_gating_disable();
	for (int i = 0; i < port_num; i++) {
		mmio_wr32(cfg_base + 0x490 + 0xb0 * i, 0x1);
	}
	rddata = mmio_rd32(0x4C + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(0x4C + CV_DDR_PHYD_APB, rddata);
	//KC_MSG("TOP_REG_EN_PLL_SPEED_CHG = %x, TOP_REG_NEXT_PLL_SPEED = %x, TOP_REG_CUR_PLL_SPEED= %x ...\n",
	//        rddata[0], rddata[9:8], rddata[5:4]);

	KC_MSG("TOP_REG_EN_PLL_SPEED_CHG = %x, TOP_REG_NEXT_PLL_SPEED = %x, TOP_REG_CUR_PLL_SPEED= %x ...\n",
	       get_bits_from_value(rddata, 0, 0), get_bits_from_value(rddata, 9, 8), get_bits_from_value(rddata, 5, 4));

	KC_MSG("LTOH Frequency Change Finished\n");
}

void cvx16_set_dq_vref(uint32_t vref)
{
#ifdef DDR4
	uint32_t en_dfi_dram_clk_disable;
	uint32_t powerdown_en;
	uint32_t selfref_en;
	uint32_t mr6_tmp;
#endif //DDR4
	uartlog("cvx16_set_dq_vref\n");
	ddr_debug_wr32(0x3b);
	ddr_debug_num_write();
#ifdef DDR4
	// save lowpower setting
	rddata = mmio_rd32(cfg_base + 0x30);
	// en_dfi_dram_clk_disable = rddata[3];
	//  powerdown_en            = rddata[1];
	//  selfref_en              = rddata[0];
	en_dfi_dram_clk_disable = get_bits_from_value(rddata, 3, 3);
	powerdown_en = get_bits_from_value(rddata, 1, 1);
	selfref_en = get_bits_from_value(rddata, 0, 0);
	rddata = modified_bits_by_value(rddata, 0, 3, 3);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	// dis_auto_refresh = 1
	rddata = mmio_rd32(cfg_base + 0x60);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
	// read mr6 @INIT7
	rddata = mmio_rd32(cfg_base + 0xec);
	rddata = modified_bits_by_value(rddata, vref, 6, 0);
	mr6_tmp = modified_bits_by_value(rddata, 1, 7, 7);
	cvx16_synp_mrw(0x6, mr6_tmp);
	uartlog("vrefDQ Training Enable\n");
	KC_MSG("vrefDQ Training Enable\n");

	opdelay(150);
	cvx16_synp_mrw(0x6, mr6_tmp);
	uartlog("vrefDQ set\n");
	opdelay(150);
	mr6_tmp = modified_bits_by_value(mr6_tmp, 0, 7, 7);
	cvx16_synp_mrw(0x6, mr6_tmp);
	uartlog("vrefDQ Training disable\n");
	KC_MSG("vrefDQ Training disable\n");

	opdelay(150);
	// dis_auto_refresh = 0
	rddata = mmio_rd32(cfg_base + 0x60);
	rddata = modified_bits_by_value(rddata, 0, 0, 0);
	mmio_wr32(cfg_base + 0x60, rddata);
	// restore
	rddata = mmio_rd32(cfg_base + 0x30);
	// rddata[5] = 0x0; //PWRCTL.selfref_sw
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	// rddata[3] = en_dfi_dram_clk_disable; //PWRCTL.en_dfi_dram_clk_disable
	rddata = modified_bits_by_value(rddata, en_dfi_dram_clk_disable, 3, 3);
	// rddata[1] = powerdown_en; //PWRCTL.powerdown_en
	rddata = modified_bits_by_value(rddata, powerdown_en, 1, 1);
	// rddata[0] = selfref_en; //PWRCTL.selfref_en
	rddata = modified_bits_by_value(rddata, selfref_en, 0, 0);
	mmio_wr32(cfg_base + 0x30, rddata);
	uartlog("restore selfref_en powerdown_en\n");
#endif
#ifdef DDR3
	// f0_param_phya_reg_tx_vref_sel	[20:16]
	rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, vref, 20, 16);
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
#endif
#ifdef DDR2
	// f0_param_phya_reg_tx_vref_sel	[20:16]
	rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, vref, 20, 16);
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		// f0_param_phya_reg_tx_vref_sel	[20:16]
		rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, vref, 20, 16);
		mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	}
	if (get_ddr_type() == DDR_TYPE_DDR2)	{
		// f0_param_phya_reg_tx_vref_sel	[20:16]
		rddata = mmio_rd32(0x0410 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, vref, 20, 16);
		mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	}
#endif
}

void cvx16_set_dfi_init_start(void)
{
	// synp setting
	// phy is ready for initial dfi_init_start request
	// set umctl2 to tigger dfi_init_start
	uartlog("cvx16_set_dfi_init_start\n");
	ddr_debug_wr32(0x0d);
	ddr_debug_num_write();
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001b0); // dfi_init_start @ rddata[5];
	rddata = modified_bits_by_value(rddata, 1, 5, 5);
	mmio_wr32(cfg_base + 0x000001b0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 1);
	KC_MSG("dfi_init_start finish\n");
}

void cvx16_ddr_phya_pd(void)
{
	uartlog("cvx16_ddr_phya_pd\n");
	ddr_debug_wr32(0x3d);
	ddr_debug_num_write();
	// ----------- PHY oen/pd reset ----------------
	// OEN
	// param_phyd_tx_ca_oenz         0
	// param_phyd_tx_ca_clk0_oenz    8
	// param_phyd_tx_ca_clk1_oenz    16
	rddata = 0x00010101;
	mmio_wr32(0x0130 + PHYD_BASE_ADDR, rddata);
	// PD
	// TOP_REG_TX_CA_PD_CA       22	0
	// TOP_REG_TX_CA_PD_CKE0     24	24
	// TOP_REG_TX_CLK_PD_CLK0    26	26
	// TOP_REG_TX_CA_PD_CSB0     28	28
	// TOP_REG_TX_CA_PD_RESETZ   30	30
	// TOP_REG_TX_ZQ_PD          31	31
	// rddata[31:0] = 0x947f_ffff;
	rddata = 0x947fffff;
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("All PHYA CA PD=0 ...\n");

	// TOP_REG_TX_BYTE0_PD	0
	// TOP_REG_TX_BYTE1_PD	1
	rddata = 0x00000003;
	mmio_wr32(0x00 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TX_BYTE PD=0 ...\n");

	// OEN
	// param_phyd_sel_cke_oenz        <= `PI_SD int_regin[0];
	mmio_wr32(0x0154 + PHYD_BASE_ADDR, rddata);
	// rddata[0] = 0b1;
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	mmio_wr32(0x0154 + PHYD_BASE_ADDR, rddata);
	uartlog("[KC Info] : CKE and RESETZ oenz\n");

	// PD
	// All PHYA PD=0
	rddata = 0xffffffff;
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	uartlog("[KC Info] : RESETZ CKE PD !!!\n\n");

	// PLL PD
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	// rddata[15]   = 1;    //TOP_REG_DDRPLL_PD
	rddata = modified_bits_by_value(rddata, 1, 15, 15);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	KC_MSG("PLL PD\n");

	// ----------- PHY oen/pd reset ----------------
}

void cvx16_ddr_phyd_save(uint32_t sram_base_addr)
{
	int sram_offset = 0;

	uartlog("cvx16_ddr_phyd_save\n");
	ddr_debug_wr32(0x46);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x0 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x4 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x8 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xc + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x10 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x14 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x18 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x1c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x20 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x24 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x28 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x2c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x40 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x44 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x48 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x4c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x50 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x54 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x58 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x5c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x60 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x64 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x68 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x70 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x74 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x80 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x84 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x88 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x8c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x90 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x94 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa0 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa4 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa8 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xac + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb0 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb4 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb8 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xbc + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xf0 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xf4 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xf8 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xfc + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x100 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x104 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x10c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x110 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x114 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x118 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x11c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x120 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x124 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x128 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x12c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x130 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x134 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x138 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x140 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x144 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x148 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x14c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x150 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x154 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x158 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x15c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x164 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x168 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x16c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x170 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x174 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x180 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x184 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x188 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x18c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x190 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x200 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x204 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x208 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x220 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x224 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x228 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x400 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x404 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x408 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x40c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x410 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x414 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x418 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x41c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x500 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x504 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x508 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x50c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x510 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x514 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x518 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x51c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x520 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x540 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x544 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x548 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x54c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x550 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x554 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x558 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x55c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x560 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x900 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	// rddata = mmio_rd32(0x904 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x908 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x90c + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x910 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x914 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x918 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x91c + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x920 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x924 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x928 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	rddata = mmio_rd32(0x92c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x930 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x934 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x938 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x940 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	// rddata = mmio_rd32(0x944 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x948 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x94c + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x950 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x954 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x958 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x95c + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x960 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x964 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x968 + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	// rddata = mmio_rd32(0x96c + PHYD_BASE_ADDR);
	// ddr_sram_wr32(sram_base_addr + sram_offset, rddata );
	//  sram_offset += 4;
	rddata = mmio_rd32(0x970 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x974 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x978 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x97c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0x980 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa00 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa04 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa08 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa0c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa10 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa14 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa18 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa1c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa20 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa24 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa28 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa2c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa30 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa34 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa38 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa3c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa40 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa44 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa48 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa4c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa50 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa54 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa58 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa5c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa60 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa64 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa68 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa6c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa70 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa74 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa78 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xa7c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb00 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb04 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb08 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb0c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb10 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb14 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb18 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb1c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb20 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb24 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb30 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb34 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb38 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb3c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb40 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb44 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb48 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb4c + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb50 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
	rddata = mmio_rd32(0xb54 + PHYD_BASE_ADDR);
	ddr_sram_wr32(sram_base_addr + sram_offset, rddata);
	sram_offset += 4;
}

void cvx16_ddr_phyd_restore(uint32_t sram_base_addr)
{
	uintptr_t temp = sram_base_addr;
	struct regconf *psave_phy_regs = (struct regconf *)temp;
	int i = 0;
	uint32_t addr;
	uint32_t val;

	while (1) {
		addr = psave_phy_regs[i].addr;
		val = psave_phy_regs[i].val;
		i++;

		if (addr != REG_GP_REG3) {
			if ((addr < (0x0 + PHYD_BASE_ADDR)) || (addr > (0xb54 + PHYD_BASE_ADDR)))
				break;
		}

		switch (addr) {
		case (0x118 + PHYD_BASE_ADDR):
			//reset param_phyd_clkctrl_init_complete
			val &= 0x00000000;
			break;
		case (0x130 + PHYD_BASE_ADDR):
			//ca oenz set by c-code
			continue;
		case (0x154 + PHYD_BASE_ADDR):
			val |= 0x00000001;
			break;
		case (0x204 + PHYD_BASE_ADDR):
			val &= 0xFFFBFFFF;
			break;
		case (0x224 + PHYD_BASE_ADDR):
			val &= 0xFFFBFFFF;
			break;
		}
		mmio_write_32(addr, val);
	}
}

void cvx16_dll_sw_upd(void)
{
	uartlog("cvx16_dll_sw_upd\n");
	ddr_debug_wr32(0x4B);
	ddr_debug_num_write();
	rddata = 0x1;
	mmio_wr32(0x0170 + PHYD_BASE_ADDR, rddata);
	while (1) {
		rddata = mmio_rd32(0x302C + PHYD_BASE_ADDR);
		if (rddata == 0xffffffff) {
			break;
		}
		KC_MSG("DLL SW UPD finish\n");
	}
}

void cvx16_bist_mask_shift_delay(uint32_t shift_delay, uint32_t en_lead)
{
	uint8_t shift_tmp;
	uint8_t delay_tmp;
	uint8_t dlie_sub;

	uartlog("cvx16_bist_mask_shift_delay\n");
	ddr_debug_wr32(0x4b);
	ddr_debug_num_write();
	//{shift_tmp, delay_tmp} = shift_delay;
	shift_tmp = get_bits_from_value(shift_delay, 12, 7);
	delay_tmp = get_bits_from_value(shift_delay, 6, 0);
	if (shift_tmp > en_lead) {
		dlie_sub = shift_tmp - en_lead;
	} else {
		dlie_sub = 0;
	}
	rddata = 0x00000000;
	rddata = shift_tmp << 8 | delay_tmp;
	mmio_wr32(0x0B0C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0B3C + PHYD_BASE_ADDR, rddata);
	rddata = 0x00000000;
	rddata = dlie_sub << 24 | dlie_sub << 8;
	mmio_wr32(0x0B10 + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0B40 + PHYD_BASE_ADDR, rddata);
	cvx16_dll_sw_clr();
	KC_MSG("cvx16_bist_mask_shift_delay Fisish\n");
}

void cvx16_set_dq_trig_lvl(uint32_t trig_lvl)
{
	uartlog("cvx16_set_dq_trig_lvl\n");
	ddr_debug_wr32(0x4c);
	ddr_debug_num_write();
	rddata = 0x00000000;
	rddata = trig_lvl << 16 | trig_lvl;
	mmio_wr32(0x0B24 + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0B54 + PHYD_BASE_ADDR, rddata);
}

void cvx16_pll_init(void)
{
	// opdelay(10);
	uartlog("pll_init\n");
	ddr_debug_wr32(0x00);
	ddr_debug_num_write();
	// TX_VREF_PD
	rddata = 0x00000000;
	mmio_wr32(0x28 + CV_DDR_PHYD_APB, rddata);
	// ZQ_240 OPTION
	rddata = 0x00080001;
	mmio_wr32(0x54 + CV_DDR_PHYD_APB, rddata);
#ifdef DDR3
// GPO setting
#ifdef _mem_freq_2133
	rddata = 0x01000808; // TOP_REG_TX_DDR3_GPO_IN =0
	mmio_wr32(0x58 + CV_DDR_PHYD_APB, rddata);
#else
	rddata = 0x01010808; // TOP_REG_TX_DDR3_GPO_IN =1
	mmio_wr32(0x58 + CV_DDR_PHYD_APB, rddata);
#endif
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		rddata = 0x01010808; // TOP_REG_TX_DDR3_GPO_IN =1
		mmio_wr32(0x58 + CV_DDR_PHYD_APB, rddata);
	}
#endif
#ifdef SSC_EN
	//==============================================================
	// Enable SSC
	//==============================================================
	rddata = reg_set; // TOP_REG_SSC_SET
	mmio_wr32(0x54 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_span, 15, 0); // TOP_REG_SSC_SPAN
	mmio_wr32(0x58 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_step, 23, 0); // TOP_REG_SSC_STEP
	mmio_wr32(0x5C + 0x03002900, rddata);
	KC_MSG("reg_step = %lx\n", reg_step);

	rddata = mmio_rd32(0x50 + 0x03002900);
	rddata = modified_bits_by_value(rddata, ~get_bits_from_value(rddata, 0, 0), 0, 0); // TOP_REG_SSC_SW_UP
	rddata = modified_bits_by_value(rddata, 1, 1, 1); // TOP_REG_SSC_EN_SSC
	rddata = modified_bits_by_value(rddata, 0, 3, 2); // TOP_REG_SSC_SSC_MODE
	rddata = modified_bits_by_value(rddata, 0, 4, 4); // TOP_REG_SSC_BYPASS
	rddata = modified_bits_by_value(rddata, 1, 5, 5); // extpulse
	rddata = modified_bits_by_value(rddata, 0, 6, 6); // ssc_syn_fix_div
	mmio_wr32(0x50 + 0x03002900, rddata);
	uartlog("SSC_EN\n");
#else
#ifdef SSC_BYPASS
	rddata = (reg_set & 0xfc000000) + 0x04000000; // TOP_REG_SSC_SET
	mmio_wr32(0x54 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_span, 15, 0); // TOP_REG_SSC_SPAN
	mmio_wr32(0x58 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_step, 23, 0); // TOP_REG_SSC_STEP
	mmio_wr32(0x5C + 0x03002900, rddata);
	rddata = mmio_rd32(0x50 + 0x03002900);
	rddata = modified_bits_by_value(rddata, ~get_bits_from_value(rddata, 0, 0), 0, 0); // TOP_REG_SSC_SW_UP
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // TOP_REG_SSC_EN_SSC
	rddata = modified_bits_by_value(rddata, 0, 3, 2); // TOP_REG_SSC_SSC_MODE
	rddata = modified_bits_by_value(rddata, 0, 4, 4); // TOP_REG_SSC_BYPASS
	rddata = modified_bits_by_value(rddata, 1, 5, 5); // TOP_REG_SSC_EXTPULSE
	rddata = modified_bits_by_value(rddata, 1, 6, 6); // ssc_syn_fix_div
	mmio_wr32(0x50 + 0x03002900, rddata);
	uartlog("SSC_BYPASS\n");
#else
	//==============================================================
	// SSC_EN =0
	//==============================================================
	uartlog("SSC_EN =0\n");
	rddata = reg_set; // TOP_REG_SSC_SET
	mmio_wr32(0x54 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_span, 15, 0); // TOP_REG_SSC_SPAN
	mmio_wr32(0x58 + 0x03002900, rddata);
	rddata = get_bits_from_value(reg_step, 23, 0); // TOP_REG_SSC_STEP
	mmio_wr32(0x5C + 0x03002900, rddata);
	rddata = mmio_rd32(0x50 + 0x03002900);
	rddata = modified_bits_by_value(rddata, ~get_bits_from_value(rddata, 0, 0), 0, 0); // TOP_REG_SSC_SW_UP
	rddata = modified_bits_by_value(rddata, 0, 1, 1); // TOP_REG_SSC_EN_SSC
	rddata = modified_bits_by_value(rddata, 0, 3, 2); // TOP_REG_SSC_SSC_MODE
	rddata = modified_bits_by_value(rddata, 0, 4, 4); // TOP_REG_SSC_BYPASS
	rddata = modified_bits_by_value(rddata, 1, 5, 5); // TOP_REG_SSC_EXTPULSE
	rddata = modified_bits_by_value(rddata, 0, 6, 6); // ssc_syn_fix_div
	mmio_wr32(0x50 + 0x03002900, rddata);
	uartlog("SSC_OFF\n");
#endif // SSC_BYPASS
#endif // SSC_EN
	// opdelay(1000);
	// DDRPLL setting
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	//[0]    = 1;      //TOP_REG_DDRPLL_EN_DLLCLK
	//[1]    = 1;      //TOP_REG_DDRPLL_EN_LCKDET
	//[2]    = 0;      //TOP_REG_DDRPLL_EN_TST
	//[5:3]  = 0b001; //TOP_REG_DDRPLL_ICTRL
	//[6]    = 0;      //TOP_REG_DDRPLL_MAS_DIV_SEL
	//[7]    = 0;      //TOP_REG_DDRPLL_MAS_RSTZ_DIV
	//[8]    = 1;      //TOP_REG_DDRPLL_SEL_4BIT
	//[10:9] = 0b01;  //TOP_REG_DDRPLL_SEL_MODE
	//[12:11]= 0b00;  //Rev
	//[13]   = 0;      //TOP_REG_DDRPLL_SEL_LOW_SPEED
	//[14]   = 0;      //TOP_REG_DDRPLL_MAS_DIV_OUT_SEL
	//[15]   = 0;      //TOP_REG_DDRPLL_PD
	rddata = modified_bits_by_value(rddata, 0x030b, 15, 0);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x10 + CV_DDR_PHYD_APB);
	//[7:0] = 0x0;   //TOP_REG_DDRPLL_TEST
	rddata = modified_bits_by_value(rddata, 0, 7, 0); // TOP_REG_DDRPLL_TEST
	mmio_wr32(0x10 + CV_DDR_PHYD_APB, rddata);
	//[0]   = 1;    //TOP_REG_RESETZ_DIV
	rddata = 0x1;
	mmio_wr32(0x04 + CV_DDR_PHYD_APB, rddata);
	uartlog("RSTZ_DIV=1\n");
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	//[7]   = 1;    //TOP_REG_DDRPLL_MAS_RSTZ_DIV
	rddata = modified_bits_by_value(rddata, 1, 7, 7);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	KC_MSG("Wait for DRRPLL LOCK=1... pll init\n");

	uartlog("Start DRRPLL LOCK pll init\n");
#ifdef REAL_LOCK
	while (1) {
		rddata = mmio_rd32(0x10 + CV_DDR_PHYD_APB);
		if (get_bits_from_value(rddata, 15, 15)) {
			break;
		}
	}
#else
	KC_MSG("check PLL lock...  pll init\n");

#endif
	uartlog("End DRRPLL LOCK=1... pll init\n");
	KC_MSG("PLL init finish !!!\n");
}

void cvx16_lb_0_phase40(void)
{
	uint32_t i, j;

	KC_MSG("DQ loop back test -IO internal loop back-\n");

	// Disable controller update for synopsys
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	uartlog("cvx16_lb_0\n");
	ddr_debug_wr32(0x4F);
	ddr_debug_num_write();
	cvx16_clk_gating_disable();
	KC_MSG("cvx16_clk_gating_disable\n");

	// param_phya_reg_sel_ddr4_mode    0
	// param_phya_reg_sel_lpddr3_mode  1
	// param_phya_reg_sel_lpddr4_mode  2
	// param_phya_reg_sel_ddr3_mode    3
	// param_phya_reg_sel_ddr2_mode    4
	rddata = 0x00000008;
	mmio_wr32(0x004C + PHYD_BASE_ADDR, rddata);
	// param_phyd_dram_class
	// DRAM class DDR2: 0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
	rddata = 0x00000006;
	mmio_wr32(0x0050 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dram_class = %x\n", rddata);

	// DDR tx vref on
	rddata = 0x00100000;
	// param_phya_reg_tx_vref_sel [20:16]
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_vrefca_sel [20:16]
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	KC_MSG("DDR tx vref on\n");

	// TOP_REG_TX_VREF_PD/TOP_REG_TX_VREFCA_PD
	rddata = 0x00000000;
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	KC_MSG("VREF_PD =0\n");

	// param_phya_reg_tx_zq_drvn,param_phya_reg_tx_zq_drvp
	rddata = 0x08080808;
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// CA DRVP/DRVN 0x08
	rddata = 0x08080808;
	mmio_wr32(0x097C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0980 + PHYD_BASE_ADDR, rddata);
	for (i = 0; i < 2; i = i + 1) {
		// DRVP/DRVN 0x08
		rddata = 0x08080808;
		mmio_wr32(0x0A38 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A3C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// Reset TX delay line to zero
		rddata = 0x06000600;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// trig_lvl_dq
		rddata = 0x00100010;
		mmio_wr32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		// sel_odt_center_tap
		rddata = mmio_rd32(0x0500 + i * 0x40 + PHYD_BASE_ADDR);
		// if odt on
		// rddata = modified_bits_by_value(rddata, 1, 10, 10 );
		mmio_wr32(0x0500 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_byte0_en_rx_awys_on   [0 :0 ]
		// param_phya_reg_tx_byte0_sel_en_rx_dly    [5 :4 ]
		// param_phya_reg_rx_byte0_sel_en_rx_gen_rst   [6 :6 ]
		// param_phya_reg_byte0_mask_oenz   [8 :8 ]
		// param_phya_reg_tx_byte0_en_mask   [10 :10 ]
		// param_phya_reg_rx_byte0_sel_cnt_mode   [13 :12 ]
		// param_phya_reg_tx_byte0_sel_int_loop_back    [14 :14 ]
		// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated   [17 :16 ]
		// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline   [18 :18 ]
		rddata = 0x00000440;
		mmio_wr32(0x0204 + i * 0x20 + PHYD_BASE_ADDR, rddata);
		KC_MSG("reg 0204 data = %x\n", rddata);
	}
	// ODT OFF
	rddata = 0x00000000;
	mmio_wr32(0x041C + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_en_ca_train_mode
	// rddata = mmio_rd32(0x0138 + PHYD_BASE_ADDR);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0 );
	// mmio_wr32(0x0138 + PHYD_BASE_ADDR,  rddata);
	rddata = 0x00404000;
	for (j = 0; j < 2; j = j + 1) {
		mmio_wr32(0x0B08 + j * 0x30 + PHYD_BASE_ADDR, rddata);
	}
	KC_MSG("rddata= %x\n", rddata);

	cvx16_dll_sw_upd();
	rddata = mmio_rd32(0x0100 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_lb_dq_en
	rddata = modified_bits_by_value(rddata, 0x400, 31, 16); // param_phyd_lb_sync_len
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	rddata = modified_bits_by_value(rddata, 1, 1, 1); // param_phyd_lb_dq_go
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	KC_MSG("wait for param_phyd_to_reg_lb_dq0_doing\n");

	KC_MSG("wait for param_phyd_to_reg_lb_dq1_doing\n");

	while (1) {
		// opdelay(1000);
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 1, 0) == 0x3) {
			break;
		}
	}
	KC_MSG("param_phyd_to_reg_lb_dq0_doing = 1\n");

	KC_MSG("param_phyd_to_reg_lb_dq1_doing = 1\n");

	uartlog("\n");

	KC_MSG("wait for param_phyd_to_reg_lb_dq0_syncfound\n");

	KC_MSG("wait for param_phyd_to_reg_lb_dq1_syncfound\n");

	while (1) {
		// opdelay(1000);
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 9, 8) == 0x3) {
			break;
		}
	}
	KC_MSG("param_phyd_to_reg_lb_dq0_syncfound = 1\n");

	KC_MSG("param_phyd_to_reg_lb_dq1_syncfound = 1\n");

	uartlog("\n");

	KC_MSG("wait for param_phyd_to_reg_lb_dq0_startfound\n");

	KC_MSG("wait for param_phyd_to_reg_lb_dq1_startfound\n");

	while (1) {
		// opdelay(1000);
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 17, 16) == 0x3) {
			break;
		}
	}
	KC_MSG("param_phyd_to_reg_lb_dq0_startfound = 1\n");

	KC_MSG("param_phyd_to_reg_lb_dq1_startfound = 1\n");

	// opdelay(1000);
	// Read param_phyd_to_reg_lb_dq_fail
	rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
	KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

	KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

	uartlog("\n");

	if (get_bits_from_value(rddata, 25, 24) != 0) {
		KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
	}
	// opdelay(1000);
	// Read param_phyd_to_reg_lb_dq_fail
	rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
	KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

	KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

	uartlog("\n");

	if (get_bits_from_value(rddata, 25, 24) != 0) {
		KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
	}
	// opdelay(1000);
	// Read param_phyd_to_reg_lb_dq_fail
	rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
	KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

	KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

	uartlog("\n");

	if (get_bits_from_value(rddata, 25, 24) != 0) {
		KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
	}
	uartlog("lb0 runing\n");
}

void cvx16_lb_0_external(void)
{
	uint32_t i, j;

	KC_MSG("DQ loop back test - with front-} (Drivier / Receiver)-\n");

	// Disable controller update for synopsys
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	uartlog("cvx16_lb_0_external\n");
	ddr_debug_wr32(0x4F);
	ddr_debug_num_write();
	cvx16_clk_gating_disable();
	KC_MSG("cvx16_clk_gating_disable\n");

	// param_phya_reg_sel_ddr4_mode    0
	// param_phya_reg_sel_lpddr3_mode  1
	// param_phya_reg_sel_lpddr4_mode  2
	// param_phya_reg_sel_ddr3_mode    3
	// param_phya_reg_sel_ddr2_mode    4
	rddata = 0x00000008;
	mmio_wr32(0x004C + PHYD_BASE_ADDR, rddata);
	// param_phyd_dram_class
	// DRAM class DDR2: 0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
	rddata = 0x00000006;
	mmio_wr32(0x0050 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dram_class = %x\n", rddata);

	// DDR tx vref on
	rddata = 0x00100000;
	// param_phya_reg_tx_vref_sel [20:16]
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_vrefca_sel [20:16]
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	KC_MSG("DDR tx vref on\n");

	// TOP_REG_TX_VREF_PD/TOP_REG_TX_VREFCA_PD
	rddata = 0x00000000;
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	KC_MSG("VREF_PD =0\n");

	// param_phya_reg_tx_zq_drvn,param_phya_reg_tx_zq_drvp
	rddata = 0x08080808;
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// CA DRVP/DRVN 0x08
	rddata = 0x08080808;
	mmio_wr32(0x097C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0980 + PHYD_BASE_ADDR, rddata);
	for (i = 0; i < 2; i = i + 1) {
		// DRVP/DRVN 0x08
		rddata = 0x08080808;
		mmio_wr32(0x0A38 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A3C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// Reset TX delay line to zero
		rddata = 0x06000600;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// trig_lvl_dq
		rddata = 0x00100010;
		mmio_wr32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		// sel_odt_center_tap
		rddata = mmio_rd32(0x0500 + i * 0x40 + PHYD_BASE_ADDR);
		// if odt on
		// rddata = modified_bits_by_value(rddata, 1, 10, 10 );
		mmio_wr32(0x0500 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_byte0_en_rx_awys_on   [0 :0 ]
		// param_phya_reg_tx_byte0_sel_en_rx_dly    [5 :4 ]
		// param_phya_reg_rx_byte0_sel_en_rx_gen_rst   [6 :6 ]
		// param_phya_reg_byte0_mask_oenz   [8 :8 ]
		// param_phya_reg_tx_byte0_en_mask   [10 :10 ]
		// param_phya_reg_rx_byte0_sel_cnt_mode   [13 :12 ]
		// param_phya_reg_tx_byte0_sel_int_loop_back    [14 :14 ]
		// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated   [17 :16 ]
		// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline   [18 :18 ]
		rddata = 0x00000440;
		mmio_wr32(0x0204 + i * 0x20 + PHYD_BASE_ADDR, rddata);
		KC_MSG("reg 0204 data = %x\n", rddata);
	}
	// ODT OFF
	rddata = 0x00000000;
	mmio_wr32(0x041C + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_en_ca_train_mode
	// rddata = mmio_rd32(0x0138 + PHYD_BASE_ADDR);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0 );
	// mmio_wr32(0x0138 + PHYD_BASE_ADDR,  rddata);
	rddata = 0x00000000;
	for (i = 0x00404000; i <= 0x00505000; i = i + 0x00101000) {
		if (i == 0x00808000) {
			rddata = 0x007f7f00;
		} else {
			rddata = i;
		}
		for (j = 0; j < 2; j = j + 1) {
			mmio_wr32(0x0B08 + j * 0x30 + PHYD_BASE_ADDR, rddata);
		}
		KC_MSG("rddata= %x\n", rddata);

		cvx16_dll_sw_upd();
		rddata = mmio_rd32(0x0100 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_lb_dq_en
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_lb_sw_en for CA
		rddata = modified_bits_by_value(rddata, 0x400, 31, 16); // param_phyd_lb_sync_len
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
		rddata = modified_bits_by_value(rddata, 1, 1, 1); // param_phyd_lb_dq_go
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
		KC_MSG("wait for param_phyd_to_reg_lb_dq0_doing\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq1_doing\n");

		while (1) {
			// opdelay(1000);
			rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 1, 0) == 0x3) {
				break;
			}
		}
		KC_MSG("param_phyd_to_reg_lb_dq0_doing = 1\n");

		KC_MSG("param_phyd_to_reg_lb_dq1_doing = 1\n");

		uartlog("\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq0_syncfound\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq1_syncfound\n");

		while (1) {
			// opdelay(1000);
			rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 9, 8) == 0x3) {
				break;
			}
		}
		KC_MSG("param_phyd_to_reg_lb_dq0_syncfound = 1\n");

		KC_MSG("param_phyd_to_reg_lb_dq1_syncfound = 1\n");

		uartlog("\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq0_startfound\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq1_startfound\n");

		while (1) {
			// opdelay(1000);
			rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 17, 16) == 0x3) {
				break;
			}
		}
		KC_MSG("param_phyd_to_reg_lb_dq0_startfound = 1\n");

		KC_MSG("param_phyd_to_reg_lb_dq1_startfound = 1\n");

		// opdelay(1000);
		// Read param_phyd_to_reg_lb_dq_fail
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

		KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

		uartlog("\n");

		if (get_bits_from_value(rddata, 25, 24) != 0) {
			KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
		}
		// opdelay(1000);
		// Read param_phyd_to_reg_lb_dq_fail
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

		KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

		uartlog("\n");

		if (get_bits_from_value(rddata, 25, 24) != 0) {
			KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
		}
		// opdelay(1000);
		// Read param_phyd_to_reg_lb_dq_fail
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

		KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

		uartlog("\n");

		if (get_bits_from_value(rddata, 25, 24) != 0) {
			KC_MSG("Error!!! DQ BIST Fail i Found ...\n");
		}
		rddata = mmio_rd32(0x0100 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 0, 0); // param_phyd_lb_dq_en
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_lb_sw_en for CA
		rddata = modified_bits_by_value(rddata, 0x400, 31, 16); // param_phyd_lb_sync_len
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
		rddata = modified_bits_by_value(rddata, 0, 1, 1); // param_phyd_lb_dq_go
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	}

	/***************************************************************
	 *      FINISHED!
	 ***************************************************************/
	// repeat(50) @(posedge axi3_ACLK);
	uartlog("PATTERN RAN TO COMPLETION\n");
}

void cvx16_lb_1_dq_set_highlow(void)
{
	uint32_t i;
	uint32_t pattern;

	KC_MSG("DQ set highlow test\n");

	// Disable controller update for synopsys
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	uartlog("cvx16_lb_1_dq_set_highlow\n");
	ddr_debug_wr32(0x50);
	ddr_debug_num_write();
	cvx16_clk_gating_disable();
	KC_MSG("cvx16_clk_gating_disable\n");

	// param_phya_reg_sel_ddr4_mode    0
	// param_phya_reg_sel_lpddr3_mode  1
	// param_phya_reg_sel_lpddr4_mode  2
	// param_phya_reg_sel_ddr3_mode    3
	// param_phya_reg_sel_ddr2_mode    4
	rddata = 0x00000008;
	mmio_wr32(0x004C + PHYD_BASE_ADDR, rddata);
	// param_phyd_dram_class
	// DRAM class DDR2: 0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
	rddata = 0x00000006;
	mmio_wr32(0x0050 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dram_class = %x\n", rddata);

	// DDR tx vref on
	rddata = 0x00100000;
	// param_phya_reg_tx_vref_sel [20:16]
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_vrefca_sel [20:16]
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	KC_MSG("DDR tx vref on\n");

	// TOP_REG_TX_VREF_PD/TOP_REG_TX_VREFCA_PD
	rddata = 0x00000000;
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	KC_MSG("VREF_PD =0\n");

	// param_phya_reg_tx_zq_drvn,param_phya_reg_tx_zq_drvp
	rddata = 0x08080808;
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// CA DRVP/DRVN 0x08
	rddata = 0x08080808;
	mmio_wr32(0x097C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0980 + PHYD_BASE_ADDR, rddata);
	for (i = 0; i < 2; i = i + 1) {
		// DRVP/DRVN 0x08
		rddata = 0x08080808;
		mmio_wr32(0x0A38 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A3C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// Reset TX delay line to zero
		rddata = 0x06000600;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// trig_lvl_dq
		rddata = 0x00100010;
		mmio_wr32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		// sel_odt_center_tap
		rddata = mmio_rd32(0x0500 + i * 0x40 + PHYD_BASE_ADDR);
		// if odt on
		// rddata = modified_bits_by_value(rddata, 1, 10, 10 );
		mmio_wr32(0x0500 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_byte0_en_rx_awys_on   [0 :0 ]
		// param_phya_reg_tx_byte0_sel_en_rx_dly    [5 :4 ]
		// param_phya_reg_rx_byte0_sel_en_rx_gen_rst   [6 :6 ]
		// param_phya_reg_byte0_mask_oenz   [8 :8 ]
		// param_phya_reg_tx_byte0_en_mask   [10 :10 ]
		// param_phya_reg_rx_byte0_sel_cnt_mode   [13 :12 ]
		// param_phya_reg_tx_byte0_sel_int_loop_back    [14 :14 ]
		// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated   [17 :16 ]
		// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline   [18 :18 ]
		rddata = 0x00000440;
		mmio_wr32(0x0204 + i * 0x20 + PHYD_BASE_ADDR, rddata);
		KC_MSG("reg 0204 data = %x\n", rddata);
	}
	// ODT OFF
	rddata = 0x00000000;
	mmio_wr32(0x041C + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_en_ca_train_mode
	// rddata = mmio_rd32(0x0138 + PHYD_BASE_ADDR);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0 );
	// mmio_wr32(0x0138 + PHYD_BASE_ADDR,  rddata);
	// param_phyd_lb_dq_en            [0:     0]
	// param_phyd_lb_dq_go            [1:     1]
	// param_phyd_lb_sw_en            [2:     2]
	// param_phyd_lb_sw_rx_en         [3:     3]
	// param_phyd_lb_sw_rx_mask       [4:     4]
	// param_phyd_lb_sw_odt_en        [5:     5]
	// param_phyd_lb_sw_ca_clkpattern [6:     6]
	// param_phyd_lb_sync_len         [31:   16]
	rddata = 0x0400001C;
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	// param_phyd_lb_sw_oenz_dout0      0
	// param_phyd_lb_sw_oenz_dout1      1
	// param_phyd_lb_sw_dqsn0           4
	// param_phyd_lb_sw_dqsn1           5
	// param_phyd_lb_sw_dqsp0           8
	// param_phyd_lb_sw_dqsp1           9
	// param_phyd_lb_sw_oenz_dqs_dout0  12
	// param_phyd_lb_sw_oenz_dqs_dout1  13
	rddata = 0x00003333;
	mmio_wr32(0x010C + PHYD_BASE_ADDR, rddata);
	// pattern all 0
	rddata = 0x00000000;
	mmio_wr32(0x0104 + PHYD_BASE_ADDR, rddata);
	// opdelay(1000);
	rddata = mmio_rd32(0x3404 + PHYD_BASE_ADDR);
	if (get_bits_from_value(rddata, 8, 0) != 0x000) {
		KC_MSG("Error!!! RX loop back din0[8:0] is not correct...\n");
	} else {
		KC_MSG("Pass!!! RX loop back din0[8:0] is Pass..\n");
	}
	if (get_bits_from_value(rddata, 24, 16) != 0x000) {
		KC_MSG("Error!!! RX loop back din1[8:0] is not correct...\n");
	} else {
		KC_MSG("Pass!!! RX loop back din1[8:0] is Pass..\n");
	}
	// pattern all 1
	rddata = 0x01ff01ff;
	mmio_wr32(0x0104 + PHYD_BASE_ADDR, rddata);
	// opdelay(1000);
	rddata = mmio_rd32(0x3404 + PHYD_BASE_ADDR);
	if (get_bits_from_value(rddata, 8, 0) != 0x1ff) {
		KC_MSG("Error!!! RX loop back din0[8:0] is not correct...\n");
	} else {
		KC_MSG("Pass!!! RX loop back din0[8:0] is Pass..\n");
	}
	if (get_bits_from_value(rddata, 24, 16) != 0x1ff) {
		KC_MSG("Error!!! RX loop back din1[8:0] is not correct...\n");
	} else {
		KC_MSG("Pass!!! RX loop back din1[8:0] is Pass..\n");
	}
	// pattern one hot
	pattern = 0x1;
	for (i = 0; i < 22; i = i + 1) {
		uartlog("[DBG] YD, i= %x, pattern =%x\n", i, pattern);

		rddata = modified_bits_by_value(rddata, get_bits_from_value(pattern, 8, 0), 8, 0);
		rddata = modified_bits_by_value(rddata, get_bits_from_value(pattern, 17, 9), 24, 16);
		mmio_wr32(0x0104 + PHYD_BASE_ADDR, rddata);
		if (i < 18) {
			rddata = mmio_rd32(0x010C + PHYD_BASE_ADDR);
			rddata = modified_bits_by_value(rddata, 0x3, 5, 4);
			rddata = modified_bits_by_value(rddata, 0x3, 9, 8);
			mmio_wr32(0x010C + PHYD_BASE_ADDR, rddata);
		} else {
			rddata = mmio_rd32(0x010C + PHYD_BASE_ADDR);
			rddata = modified_bits_by_value(rddata, get_bits_from_value(pattern, 19, 18), 5, 4);
			rddata = modified_bits_by_value(rddata, get_bits_from_value(pattern, 21, 20), 9, 8);
		}
		// opdelay(1000);
		rddata = mmio_rd32(0x3404 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 8, 0) != get_bits_from_value(pattern, 8, 0)) {
			KC_MSG("Error!!! RX loop back din0[8:0] is not correct...\n");
		} else {
			KC_MSG("Pass!!! RX loop back din0[8:0] is Pass..\n");
		}
		if (get_bits_from_value(rddata, 24, 16) != get_bits_from_value(pattern, 17, 9)) {
			KC_MSG("Error!!! RX loop back din1[8:0] is not correct...\n");
		} else {
			KC_MSG("Pass!!! RX loop back din1[8:0] is Pass..\n");
		}
		pattern = pattern << 1;
	}
	i = 0;
	pattern = 0x1;
	// param_phyd_lb_dq_en            [0:     0]
	// param_phyd_lb_dq_go            [1:     1]
	// param_phyd_lb_sw_en            [2:     2]
	// param_phyd_lb_sw_rx_en         [3:     3]
	// param_phyd_lb_sw_rx_mask       [4:     4]
	// param_phyd_lb_sw_odt_en        [5:     5]
	// param_phyd_lb_sw_ca_clkpattern [6:     6]
	// param_phyd_lb_sync_len         [31:   16]
	rddata = 0x04000000;
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	/***************************************************************
	 *      FINISHED!
	 ***************************************************************/
	uartlog("LB_1 PATTERN RAN TO COMPLETION\n");
}

void cvx16_lb_2_mux_demux(void)
{
	uint32_t i, j;

	KC_MSG("DQ loop back test 2 -DEMUX/MUX internal loop back-\n");

	// Disable controller update for synopsys
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	uartlog("cvx16_lb_2_mux_demux\n");
	ddr_debug_wr32(0x50);
	ddr_debug_num_write();
	cvx16_clk_gating_disable();
	KC_MSG("cvx16_clk_gating_disable\n");

	// param_phya_reg_sel_ddr4_mode    0
	// param_phya_reg_sel_lpddr3_mode  1
	// param_phya_reg_sel_lpddr4_mode  2
	// param_phya_reg_sel_ddr3_mode    3
	// param_phya_reg_sel_ddr2_mode    4
	rddata = 0x00000008;
	mmio_wr32(0x004C + PHYD_BASE_ADDR, rddata);
	// param_phyd_dram_class
	// DRAM class DDR2: 0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
	rddata = 0x00000006;
	mmio_wr32(0x0050 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dram_class = %x\n", rddata);

	// DDR tx vref on
	rddata = 0x00100000;
	// param_phya_reg_tx_vref_sel [20:16]
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_vrefca_sel [20:16]
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	KC_MSG("DDR tx vref on\n");

	// TOP_REG_TX_VREF_PD/TOP_REG_TX_VREFCA_PD
	rddata = 0x00000000;
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	KC_MSG("VREF_PD =0\n");

	// param_phya_reg_tx_zq_drvn,param_phya_reg_tx_zq_drvp
	rddata = 0x08080808;
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// CA DRVP/DRVN 0x08
	rddata = 0x08080808;
	mmio_wr32(0x097C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0980 + PHYD_BASE_ADDR, rddata);
	for (i = 0; i < 2; i = i + 1) {
		// DRVP/DRVN 0x08
		rddata = 0x08080808;
		mmio_wr32(0x0A38 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A3C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// Reset TX delay line to zero
		rddata = 0x06000600;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// trig_lvl_dq
		rddata = 0x00100010;
		mmio_wr32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		// sel_odt_center_tap
		rddata = mmio_rd32(0x0500 + i * 0x40 + PHYD_BASE_ADDR);
		// if odt on
		// rddata = modified_bits_by_value(rddata, 1, 10, 10 );
		mmio_wr32(0x0500 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_byte0_en_rx_awys_on   [0 :0 ]
		// param_phya_reg_tx_byte0_sel_en_rx_dly    [5 :4 ]
		// param_phya_reg_rx_byte0_sel_en_rx_gen_rst   [6 :6 ]
		// param_phya_reg_byte0_mask_oenz   [8 :8 ]
		// param_phya_reg_tx_byte0_en_mask   [10 :10 ]
		// param_phya_reg_rx_byte0_sel_cnt_mode   [13 :12 ]
		// param_phya_reg_tx_byte0_sel_int_loop_back    [14 :14 ]
		// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated   [17 :16 ]
		// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline   [18 :18 ]
		rddata = 0x00004440;
		mmio_wr32(0x0204 + i * 0x20 + PHYD_BASE_ADDR, rddata);
		KC_MSG("reg 0204 data = %x\n", rddata);
	}
	// ODT OFF
	rddata = 0x00000000;
	mmio_wr32(0x041C + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_en_ca_train_mode
	// rddata = mmio_rd32(0x0138 + PHYD_BASE_ADDR);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0 );
	// mmio_wr32(0x0138 + PHYD_BASE_ADDR,  rddata);
	rddata = 0x00000000;
	for (i = 0x00404000; i <= 0x00404000; i = i + 0x00101000) {
		if (i == 0x00808000) {
			rddata = 0x007f7f00;
		} else {
			rddata = i;
		}
		for (j = 0; j < 2; j = j + 1) {
			mmio_wr32(0x0B08 + j * 0x30 + PHYD_BASE_ADDR, rddata);
		}
		KC_MSG("rddata= %x\n", rddata);

		cvx16_dll_sw_upd();
		rddata = mmio_rd32(0x0100 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 0, 0); // param_phyd_lb_dq_en
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_lb_sw_en for CA
		rddata = modified_bits_by_value(rddata, 0x400, 31, 16); // param_phyd_lb_sync_len
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
		rddata = modified_bits_by_value(rddata, 1, 1, 1); // param_phyd_lb_dq_go
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
		KC_MSG("wait for param_phyd_to_reg_lb_dq0_doing\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq1_doing\n");

		while (1) {
			// opdelay(1000);
			rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 1, 0) == 0x3) {
				break;
			}
		}
		KC_MSG("param_phyd_to_reg_lb_dq0_doing = 1\n");

		KC_MSG("param_phyd_to_reg_lb_dq1_doing = 1\n");

		uartlog("\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq0_syncfound\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq1_syncfound\n");

		while (1) {
			// opdelay(1000);
			rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 9, 8) == 0x3) {
				break;
			}
		}
		KC_MSG("param_phyd_to_reg_lb_dq0_syncfound = 1\n");

		KC_MSG("param_phyd_to_reg_lb_dq1_syncfound = 1\n");

		uartlog("\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq0_startfound\n");

		KC_MSG("wait for param_phyd_to_reg_lb_dq1_startfound\n");

		while (1) {
			// opdelay(1000);
			rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
			if (get_bits_from_value(rddata, 17, 16) == 0x3) {
				break;
			}
		}
		KC_MSG("param_phyd_to_reg_lb_dq0_startfound = 1\n");

		KC_MSG("param_phyd_to_reg_lb_dq1_startfound = 1\n");

		// opdelay(1000);
		// Read param_phyd_to_reg_lb_dq_fail
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

		KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

		uartlog("\n");

		if (get_bits_from_value(rddata, 25, 24) != 0) {
			KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
		}
		// opdelay(1000);
		// Read param_phyd_to_reg_lb_dq_fail
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

		KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

		uartlog("\n");

		if (get_bits_from_value(rddata, 25, 24) != 0) {
			KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
		}
		// opdelay(1000);
		// Read param_phyd_to_reg_lb_dq_fail
		rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
		KC_MSG("param_phyd_to_reg_lb_dq0_fail = %x\n", get_bits_from_value(rddata, 24, 24));

		KC_MSG("param_phyd_to_reg_lb_dq1_fail = %x\n", get_bits_from_value(rddata, 25, 25));

		uartlog("\n");

		if (get_bits_from_value(rddata, 25, 24) != 0) {
			KC_MSG("Error!!! DQ BIST Fail is Found ...\n");
		}
		rddata = mmio_rd32(0x0100 + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 0, 0, 0); // param_phyd_lb_dq_en
		rddata = modified_bits_by_value(rddata, 0, 2, 2); // param_phyd_lb_sw_en for CA
		rddata = modified_bits_by_value(rddata, 0x400, 31, 16); // param_phyd_lb_sync_len
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
		rddata = modified_bits_by_value(rddata, 0, 1, 1); // param_phyd_lb_dq_go
		mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	}
	/***************************************************************
	 *      FINISHED!
	 ***************************************************************/
	// repeat(50) @(posedge axi3_ACLK);
	uartlog("PATTERN RAN TO COMPLETION\n");
}

void cvx16_lb_3_ca_set_highlow(void)
{
	uint32_t i;
	uint32_t pattern;
	uint32_t rddata_addr;
	uint32_t rddata_ctrl;

	KC_MSG("CA set highlow test\n");

	// Disable controller update for synopsys
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	uartlog("cvx16_lb_3_ca_set_highlow\n");
	ddr_debug_wr32(0x50);
	ddr_debug_num_write();
	cvx16_clk_gating_disable();
	KC_MSG("cvx16_clk_gating_disable\n");

	// param_phya_reg_sel_ddr4_mode    0
	// param_phya_reg_sel_lpddr3_mode  1
	// param_phya_reg_sel_lpddr4_mode  2
	// param_phya_reg_sel_ddr3_mode    3
	// param_phya_reg_sel_ddr2_mode    4
	rddata = 0x00000008;
	mmio_wr32(0x004C + PHYD_BASE_ADDR, rddata);
	// param_phyd_dram_class
	// DRAM class DDR2: 0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
	rddata = 0x00000006;
	mmio_wr32(0x0050 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dram_class = %x\n", rddata);

	// DDR tx vref on
	rddata = 0x00100000;
	// param_phya_reg_tx_vref_sel [20:16]
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_vrefca_sel [20:16]
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	KC_MSG("DDR tx vref on\n");

	// TOP_REG_TX_VREF_PD/TOP_REG_TX_VREFCA_PD
	rddata = 0x00000000;
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	KC_MSG("VREF_PD =0\n");

	// param_phya_reg_tx_zq_drvn,param_phya_reg_tx_zq_drvp
	rddata = 0x08080808;
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// CA DRVP/DRVN 0x08
	rddata = 0x08080808;
	mmio_wr32(0x097C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0980 + PHYD_BASE_ADDR, rddata);
	for (i = 0; i < 2; i = i + 1) {
		// DRVP/DRVN 0x08
		rddata = 0x08080808;
		mmio_wr32(0x0A38 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A3C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// Reset TX delay line to zero
		rddata = 0x06000600;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// trig_lvl_dq
		rddata = 0x00100010;
		mmio_wr32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		// sel_odt_center_tap
		rddata = mmio_rd32(0x0500 + i * 0x40 + PHYD_BASE_ADDR);
		// if odt on
		// rddata = modified_bits_by_value(rddata, 1, 10, 10 );
		mmio_wr32(0x0500 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_byte0_en_rx_awys_on   [0 :0 ]
		// param_phya_reg_tx_byte0_sel_en_rx_dly    [5 :4 ]
		// param_phya_reg_rx_byte0_sel_en_rx_gen_rst   [6 :6 ]
		// param_phya_reg_byte0_mask_oenz   [8 :8 ]
		// param_phya_reg_tx_byte0_en_mask   [10 :10 ]
		// param_phya_reg_rx_byte0_sel_cnt_mode   [13 :12 ]
		// param_phya_reg_tx_byte0_sel_int_loop_back    [14 :14 ]
		// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated   [17 :16 ]
		// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline   [18 :18 ]
		rddata = 0x00000440;
		mmio_wr32(0x0204 + i * 0x20 + PHYD_BASE_ADDR, rddata);
		KC_MSG("reg 0204 data = %x\n", rddata);
	}
	// ODT OFF
	rddata = 0x00000000;
	mmio_wr32(0x041C + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_en_ca_train_mode
	// rddata = mmio_rd32(0x0138 + PHYD_BASE_ADDR);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0 );
	// mmio_wr32(0x0138 + PHYD_BASE_ADDR,  rddata);
	// param_phyd_lb_dq_en            [0:     0]
	// param_phyd_lb_dq_go            [1:     1]
	// param_phyd_lb_sw_en            [2:     2]
	// param_phyd_lb_sw_rx_en         [3:     3]
	// param_phyd_lb_sw_rx_mask       [4:     4]
	// param_phyd_lb_sw_odt_en        [5:     5]
	// param_phyd_lb_sw_ca_clkpattern [6:     6]
	// param_phyd_lb_sync_len         [31:   16]
	rddata = 0x04000004;
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0134 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x1, 2, 2); // param_phya_reg_tx_ca_en_ca_loop_back
	mmio_wr32(0x0134 + PHYD_BASE_ADDR, rddata);
	// GPO
	// TOP_REG_TX_DDR3_GPO_DRVN	    [4:	    0]
	// TOP_REG_TX_DDR3_GPO_DRVP	    [12:	8]
	// TOP_REG_TX_DDR3_GPO_IN	    [16:	16]
	// TOP_REG_TX_DDR3_GPO_OENZ	    [17:	17]
	// TOP_REG_TX_DDR3_GPO_PD_GPO	[18:	18]
	// TOP_REG_TX_DDR3_GPO_SEL_GPIO	[19:	19]
	rddata = 0x01010808; // TOP_REG_TX_DDR3_GPO_IN =1
	mmio_wr32(0x58 + CV_DDR_PHYD_APB, rddata);
	// opdelay(1000);
	rddata = mmio_rd32(0x58 + CV_DDR_PHYD_APB);
	if (get_bits_from_value(rddata, 28, 28) != 0x1) {
		KC_MSG("Error!!! GPO test Fail\n");
	} else {
		KC_MSG("Pass!!! GPO test pass\n");
	}
	rddata = 0x01000808; // TOP_REG_TX_DDR3_GPO_IN =0
	mmio_wr32(0x58 + CV_DDR_PHYD_APB, rddata);
	// opdelay(1000);
	rddata = mmio_rd32(0x58 + CV_DDR_PHYD_APB);
	if (get_bits_from_value(rddata, 28, 28) != 0x0) {
		KC_MSG("Error!!! GPO test Fail\n");
	} else {
		KC_MSG("Pass!!! GPO test pass\n");
	}
	// pattern all 0
	rddata_addr = 0x00000000;
	mmio_wr32(0x0110 + PHYD_BASE_ADDR, rddata_addr); // param_phyd_lb_sw_ca_dout
	// param_phyd_lb_sw_clkn0_dout	0
	// param_phyd_lb_sw_clkp0_dout	4
	// param_phyd_lb_sw_cke0_dout	8
	// param_phyd_lb_sw_resetz_dout	12
	// param_phyd_lb_sw_csb0_dout	16
	rddata_ctrl = 0x00000000;
	mmio_wr32(0x0114 + PHYD_BASE_ADDR, rddata_ctrl);
	// opdelay(1000);
	rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
	if (rddata != rddata_addr) {
		KC_MSG("Error!!! CA BIST Fail is Found : CA[22:0] pin\n");
	} else {
		KC_MSG("Pass!!! CA[22:0] pin\n");
	}
	rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
	if (rddata != rddata_ctrl) {
		KC_MSG("Error!!! CA BIST Fail is Found : other pin [16:0] test\n");
	} else {
		KC_MSG("Pass!!! other pin is Pass..\n");
	}
	// pattern all 1
	rddata_addr = 0x007fffff;
	mmio_wr32(0x0110 + PHYD_BASE_ADDR, rddata_addr); // param_phyd_lb_sw_ca_dout
	// param_phyd_lb_sw_clkn0_dout	0
	// param_phyd_lb_sw_clkp0_dout	4
	// param_phyd_lb_sw_cke0_dout	8
	// param_phyd_lb_sw_resetz_dout	12
	// param_phyd_lb_sw_csb0_dout	16
	rddata_ctrl = 0x00010011;
	mmio_wr32(0x0114 + PHYD_BASE_ADDR, rddata_ctrl);
	// opdelay(1000);
	rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
	if (rddata != rddata_addr) {
		KC_MSG("Error!!! CA BIST Fail is Found : CA[22:0] pin\n");
	} else {
		KC_MSG("Pass!!! CA[22:0] pin\n");
	}
	rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
	if (rddata != rddata_ctrl) {
		KC_MSG("Error!!! CA BIST Fail is Found : other pin [8:0] test\n");
	} else {
		KC_MSG("Pass!!! other pin is Pass..\n");
	}
	// pattern 01
	rddata_addr = 0x00555555;
	mmio_wr32(0x0110 + PHYD_BASE_ADDR, rddata_addr); // param_phyd_lb_sw_ca_dout
	// param_phyd_lb_sw_clkn0_dout	0
	// param_phyd_lb_sw_clkp0_dout	4
	// param_phyd_lb_sw_cke0_dout	8
	// param_phyd_lb_sw_resetz_dout	12
	// param_phyd_lb_sw_csb0_dout	16
	rddata_ctrl = 0x00000010;
	mmio_wr32(0x0114 + PHYD_BASE_ADDR, rddata_ctrl);
	// opdelay(1000);
	rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
	if (rddata != rddata_addr) {
		KC_MSG("Error!!! CA BIST Fail is Found : CA[22:0] pin\n");
	} else {
		KC_MSG("Pass!!! CA[22:0] pin\n");
	}
	rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
	if (rddata != rddata_ctrl) {
		KC_MSG("Error!!! CA BIST Fail is Found : other pin [8:0] test\n");
	} else {
		KC_MSG("Pass!!! other pin is Pass..\n");
	}
	// pattern 10
	rddata_addr = 0x002aaaaa;
	mmio_wr32(0x0110 + PHYD_BASE_ADDR, rddata_addr); // param_phyd_lb_sw_ca_dout
	// param_phyd_lb_sw_clkn0_dout	0
	// param_phyd_lb_sw_clkp0_dout	4
	// param_phyd_lb_sw_cke0_dout	8
	// param_phyd_lb_sw_resetz_dout	12
	// param_phyd_lb_sw_csb0_dout	16
	rddata_ctrl = 0x00010001;
	mmio_wr32(0x0114 + PHYD_BASE_ADDR, rddata_ctrl);
	// opdelay(1000);
	rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
	if (rddata != rddata_addr) {
		KC_MSG("Error!!! CA BIST Fail is Found : CA[22:0] pin\n");
	} else {
		KC_MSG("Pass!!! CA[22:0] pin\n");
	}
	rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
	if (rddata != rddata_ctrl) {
		KC_MSG("Error!!! CA BIST Fail is Found : other pin [8:0] test\n");
	} else {
		KC_MSG("Pass!!! other pin is Pass..\n");
	}
	// pattern one hot
	pattern = 0x1;
	for (i = 0; i < 25; i = i + 1) {
		uartlog("[DBG] YD, i= %x, pattern =%x\n", i, pattern);

		rddata_addr = 0x00000000;
		rddata_addr = modified_bits_by_value(rddata_addr, get_bits_from_value(pattern, 22, 0), 22, 0);
		mmio_wr32(0x0110 + PHYD_BASE_ADDR, rddata_addr);
		// param_phyd_lb_sw_clkn0_dout   0
		// param_phyd_lb_sw_clkp0_dout   4
		// param_phyd_lb_sw_cke0_dout    8
		// param_phyd_lb_sw_resetz_dout  12
		// param_phyd_lb_sw_csb0_dout    16
		rddata_ctrl = 0x00000000;
		rddata_ctrl = modified_bits_by_value(rddata_ctrl, get_bits_from_value(pattern, 23, 23), 0, 0);
		rddata_ctrl = modified_bits_by_value(rddata_ctrl, get_bits_from_value(pattern, 24, 24), 4, 4);
		rddata_ctrl = modified_bits_by_value(rddata_ctrl, get_bits_from_value(pattern, 25, 25), 16, 16);
		mmio_wr32(0x0114 + PHYD_BASE_ADDR, rddata_ctrl);
		// opdelay(1000);
		rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
		if (rddata != rddata_addr) {
			KC_MSG("Error!!! CA BIST Fail is Found : CA[22:0] pin\n");
		} else {
			KC_MSG("Pass!!! CA[22:0] is Pass..\n");
		}
		rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
		if (rddata != rddata_ctrl) {
			KC_MSG("Error!!! CA BIST Fail is Found : other pin [8:0] test\n");
		} else {
			KC_MSG("Pass!!! other pin [8:0]\n");
		}
		pattern = pattern << 1;
	}
	i = 0;
	pattern = 0x1;
	// param_phyd_lb_dq_en            [0:     0]
	// param_phyd_lb_dq_go            [1:     1]
	// param_phyd_lb_sw_en            [2:     2]
	// param_phyd_lb_sw_rx_en         [3:     3]
	// param_phyd_lb_sw_rx_mask       [4:     4]
	// param_phyd_lb_sw_odt_en        [5:     5]
	// param_phyd_lb_sw_ca_clkpattern [6:     6]
	// param_phyd_lb_sync_len         [31:   16]
	rddata = 0x04000000;
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	/***************************************************************
	 *      FINISHED!
	 ***************************************************************/
	uartlog("LB_3 PATTERN RAN TO COMPLETION\n");
}

void cvx16_lb_4_ca_clk_pat(void)
{
	uint32_t i;
	// uint32_t pattern; //unused
	// uint32_t rddata_addr; //unused
	// uint32_t rddata_ctrl; //unused
	KC_MSG("%s test\n", __func__);

	// Disable controller update for synopsys
	mmio_wr32(cfg_base + 0x00000320, 0x00000000);
	rddata = mmio_rd32(cfg_base + 0x000001a0);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x000001a0, rddata);
	mmio_wr32(cfg_base + 0x00000320, 0x00000001);
	uartlog("cvx16_lb_4_ca_clk_pat\n");
	ddr_debug_wr32(0x50);
	ddr_debug_num_write();
	cvx16_clk_gating_disable();
	KC_MSG("cvx16_clk_gating_disable\n");

	// param_phya_reg_sel_ddr4_mode    0
	// param_phya_reg_sel_lpddr3_mode  1
	// param_phya_reg_sel_lpddr4_mode  2
	// param_phya_reg_sel_ddr3_mode    3
	// param_phya_reg_sel_ddr2_mode    4
	rddata = 0x00000008;
	mmio_wr32(0x004C + PHYD_BASE_ADDR, rddata);
	// param_phyd_dram_class
	// DRAM class DDR2: 0b0100, DDR3: 0b0110, DDR4: 0b1010, LPDDR3: 0b0111, LPDDR4: 0b1011
	rddata = 0x00000006;
	mmio_wr32(0x0050 + PHYD_BASE_ADDR, rddata);
	KC_MSG("dram_class = %x\n", rddata);

	// DDR tx vref on
	rddata = 0x00100000;
	// param_phya_reg_tx_vref_sel [20:16]
	mmio_wr32(0x0410 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_vrefca_sel [20:16]
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	KC_MSG("DDR tx vref on\n");

	// TOP_REG_TX_VREF_PD/TOP_REG_TX_VREFCA_PD
	rddata = 0x00000000;
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	KC_MSG("VREF_PD =0\n");

	// param_phya_reg_tx_zq_drvn,param_phya_reg_tx_zq_drvp
	rddata = 0x08080808;
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// CA DRVP/DRVN 0x08
	rddata = 0x08080808;
	mmio_wr32(0x097C + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0980 + PHYD_BASE_ADDR, rddata);
	for (i = 0; i < 2; i = i + 1) {
		// DRVP/DRVN 0x08
		rddata = 0x08080808;
		mmio_wr32(0x0A38 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A3C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// Reset TX delay line to zero
		rddata = 0x06000600;
		mmio_wr32(0x0A00 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A04 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A08 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A0C + i * 0x40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0A10 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// trig_lvl_dq
		rddata = 0x00100010;
		mmio_wr32(0x0B24 + i * 0x30 + PHYD_BASE_ADDR, rddata);
		// sel_odt_center_tap
		rddata = mmio_rd32(0x0500 + i * 0x40 + PHYD_BASE_ADDR);
		// if odt on
		// rddata = modified_bits_by_value(rddata, 1, 10, 10 );
		mmio_wr32(0x0500 + i * 0x40 + PHYD_BASE_ADDR, rddata);
		// param_phya_reg_tx_byte0_en_rx_awys_on   [0 :0 ]
		// param_phya_reg_tx_byte0_sel_en_rx_dly    [5 :4 ]
		// param_phya_reg_rx_byte0_sel_en_rx_gen_rst   [6 :6 ]
		// param_phya_reg_byte0_mask_oenz   [8 :8 ]
		// param_phya_reg_tx_byte0_en_mask   [10 :10 ]
		// param_phya_reg_rx_byte0_sel_cnt_mode   [13 :12 ]
		// param_phya_reg_tx_byte0_sel_int_loop_back    [14 :14 ]
		// param_phya_reg_rx_byte0_sel_dqs_dly_for_gated   [17 :16 ]
		// param_phya_reg_tx_byte0_en_extend_oenz_gated_dline   [18 :18 ]
		rddata = 0x00000440;
		mmio_wr32(0x0204 + i * 0x20 + PHYD_BASE_ADDR, rddata);
		KC_MSG("reg 0204 data = %x\n", rddata);
	}
	// ODT OFF
	rddata = 0x00000000;
	mmio_wr32(0x041C + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_rx_en_ca_train_mode
	// rddata = mmio_rd32(0x0138 + PHYD_BASE_ADDR);
	// rddata = modified_bits_by_value(rddata, 1, 0, 0 );
	// mmio_wr32(0x0138 + PHYD_BASE_ADDR,  rddata);
	// CKE/RESETN
	rddata = 0x00000067; // TOP_REG_TX_CA_SEL_GPIO_CKE0  [2] = 1
	mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
	rddata = 0x40000000; // TOP_REG_TX_CA_PD_RESETZ [30] = 1
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	// param_phyd_lb_dq_en            [0:     0]
	// param_phyd_lb_dq_go            [1:     1]
	// param_phyd_lb_sw_en            [2:     2]
	// param_phyd_lb_sw_rx_en         [3:     3]
	// param_phyd_lb_sw_rx_mask       [4:     4]
	// param_phyd_lb_sw_odt_en        [5:     5]
	// param_phyd_lb_sw_ca_clkpattern [6:     6]
	// param_phyd_lb_sync_len         [31:   16]
	rddata = 0x04000044;
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0134 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x1, 2, 2); // param_phya_reg_tx_ca_en_ca_loop_back
	mmio_wr32(0x0134 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_ca_en_tx_de     24
	// param_phya_reg_tx_clk0_en_tx_de   28
	// param_phya_reg_tx_csb_en_tx_de    30
	rddata = 0x51000000;
	mmio_wr32(0x0404 + PHYD_BASE_ADDR, rddata);
	// clock pattern
	// opdelay(1000);
	rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
	if (rddata != 0x007fffff) {
		KC_MSG("Error!!! CA BIST Fail is Found : CA[22:0] pin\n");
	} else {
		KC_MSG("Pass!!! CA[22:0] pin\n");
	}
	// param_phyd_lb_sw_clkn0_dout	0
	// param_phyd_lb_sw_clkp0_dout	4
	// param_phyd_lb_sw_cke0_dout	8
	// param_phyd_lb_sw_resetz_dout	12
	// param_phyd_lb_sw_csb0_dout	16
	rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
	if ((rddata & 0x00010011) != 0x00010011) {
		KC_MSG("Error!!! CA BIST Fail is Found : other pin [8:0] test\n");
	} else {
		KC_MSG("Pass!!! other pin is Pass..\n");
	}
	// param_phyd_lb_dq_en            [0:     0]
	// param_phyd_lb_dq_go            [1:     1]
	// param_phyd_lb_sw_en            [2:     2]
	// param_phyd_lb_sw_rx_en         [3:     3]
	// param_phyd_lb_sw_rx_mask       [4:     4]
	// param_phyd_lb_sw_odt_en        [5:     5]
	// param_phyd_lb_sw_ca_clkpattern [6:     6]
	// param_phyd_lb_sync_len         [31:   16]
	rddata = 0x04000000;
	mmio_wr32(0x0100 + PHYD_BASE_ADDR, rddata);
	/***************************************************************
	 *      FINISHED!
	 ***************************************************************/
	uartlog("LB_4 PATTERN RAN TO COMPLETION\n");
}

void cvx16_clk_gating_disable(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x4C);
	ddr_debug_num_write();
	// TOP_REG_CG_EN_PHYD_TOP      0
	// TOP_REG_CG_EN_CALVL         1
	// TOP_REG_CG_EN_WRLVL         2
	// N/A                         3
	// TOP_REG_CG_EN_WRDQ          4
	// TOP_REG_CG_EN_RDDQ          5
	// TOP_REG_CG_EN_PIGTLVL       6
	// TOP_REG_CG_EN_RGTRACK       7
	// TOP_REG_CG_EN_DQSOSC        8
	// TOP_REG_CG_EN_LB            9
	// TOP_REG_CG_EN_DLL_SLAVE     10 //0:a-on
	// TOP_REG_CG_EN_DLL_MST       11 //0:a-on
	// TOP_REG_CG_EN_ZQ            12
	// TOP_REG_CG_EN_PHY_PARAM     13 //0:a-on
	// TOP_REG_CG_EN_PHYA_GATED    14 //1: clock gating enable from controller
	//                                  0:from TOP_REG_DDRPLL_CLK_OUT_GATED_EN
	// 0b01001011110101
	rddata = 0x000012F5;
	mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
	rddata = 0x00000000;
	mmio_wr32(0x00F4 + PHYD_BASE_ADDR, rddata); // PHYD_SHIFT_GATING_EN
	rddata = mmio_rd32(cfg_base + 0x30); // phyd_stop_clk
	rddata = modified_bits_by_value(rddata, 0, 9, 9); //en_phyd_stop_clk
	//rddata = modified_bits_by_value(rddata, 0, 10, 10 ); //phyd_stop_clk_sel, 1: old, 0: new
	rddata = modified_bits_by_value(rddata, 0, 11, 11); //en_phya_stop_clk
	//rddata = modified_bits_by_value(rddata, 0, 12, 12 ); //phya_stop_clk_sel, 1: old, 0: new
	mmio_wr32(cfg_base + 0x30, rddata);
	rddata = mmio_rd32(cfg_base + 0x148); // dfi read/write clock gatting
	rddata = modified_bits_by_value(rddata, 0, 23, 23);
	rddata = modified_bits_by_value(rddata, 0, 31, 31);
	mmio_wr32(cfg_base + 0x148, rddata);
	KC_MSG("clk_gating_disable\n");

	// disable clock gating
	// mmio_wr32(0x0800_a000 + 0x14 , 0x00000fff);
	// KC_MSG("axi disable clock gating\n");
}

void cvx16_clk_gating_enable(void)
{
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x4D);
	ddr_debug_num_write();
	// TOP_REG_CG_EN_PHYD_TOP      0
	// TOP_REG_CG_EN_CALVL         1
	// TOP_REG_CG_EN_WRLVL         2
	// N/A                         3
	// TOP_REG_CG_EN_WRDQ          4
	// TOP_REG_CG_EN_RDDQ          5
	// TOP_REG_CG_EN_PIGTLVL       6
	// TOP_REG_CG_EN_RGTRACK       7
	// TOP_REG_CG_EN_DQSOSC        8
	// TOP_REG_CG_EN_LB            9
	// TOP_REG_CG_EN_DLL_SLAVE     10 //0:a-on
	// TOP_REG_CG_EN_DLL_MST       11 //0:a-on
	// TOP_REG_CG_EN_ZQ            12
	// TOP_REG_CG_EN_PHY_PARAM     13 //0:a-on
	// TOP_REG_CG_EN_PHYA_GATED    14 //1: clock gating enable from controller
	//                                  0:from TOP_REG_DDRPLL_CLK_OUT_GATED_EN
	// 0b10110010000001
	rddata = 0x00006C81;
	mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
	//    #ifdef _mem_freq_1333
	//    #ifdef DDR2
	rddata = mmio_rd32(cfg_base + 0x190);
	rddata = modified_bits_by_value(rddata, 6, 28, 24);
	mmio_wr32(cfg_base + 0x190, rddata);
	//    #endif
	rddata = 0x00030033;
	mmio_wr32(0x00F4 + PHYD_BASE_ADDR, rddata); // PHYD_SHIFT_GATING_EN
	rddata = mmio_rd32(cfg_base + 0x30); // phyd_stop_clk
	rddata = modified_bits_by_value(rddata, 1, 9, 9); //en_phyd_stop_clk
	rddata = modified_bits_by_value(rddata, 0, 10, 10); //phyd_stop_clk_sel, 1: old, 0: new
	rddata = modified_bits_by_value(rddata, 1, 11, 11); //en_phya_stop_clk
	rddata = modified_bits_by_value(rddata, 0, 12, 12); //phya_stop_clk_sel, 1: old, 0: new
	mmio_wr32(cfg_base + 0x30, rddata);
	rddata = mmio_rd32(cfg_base + 0x148); // dfi read/write clock gatting
	rddata = modified_bits_by_value(rddata, 1, 23, 23);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(cfg_base + 0x148, rddata);
	KC_MSG("clk_gating_enable\n");

	// disable clock gating
	// mmio_wr32(0x0800_a000 + 0x14 , 0x00000fff);
	// KC_MSG("axi disable clock gating\n");
}

void cvx16_dfi_phyupd_req(void)
{
	uint32_t ca_raw_upd;
	uint32_t byte0_wr_raw_upd;
	uint32_t byte1_wr_raw_upd;
	uint32_t byte0_wdqs_raw_upd;
	uint32_t byte1_wdqs_raw_upd;
	uint32_t byte0_rd_raw_upd;
	uint32_t byte1_rd_raw_upd;
	uint32_t byte0_rdg_raw_upd;
	uint32_t byte1_rdg_raw_upd;
	uint32_t byte0_rdqs_raw_upd;
	uint32_t byte1_rdqs_raw_upd;

	ca_raw_upd = 0x00000001 << 0;
	byte0_wr_raw_upd = 0x00000001 << 4;
	byte1_wr_raw_upd = 0x00000001 << 5;
	byte0_wdqs_raw_upd = 0x00000001 << 8;
	byte1_wdqs_raw_upd = 0x00000001 << 9;
	byte0_rd_raw_upd = 0x00000001 << 12;
	byte1_rd_raw_upd = 0x00000001 << 13;
	byte0_rdg_raw_upd = 0x00000001 << 16;
	byte1_rdg_raw_upd = 0x00000001 << 17;
	byte0_rdqs_raw_upd = 0x00000001 << 20;
	byte1_rdqs_raw_upd = 0x00000001 << 21;
	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x54);
	ddr_debug_num_write();
	// if ($test$plusargs("")) {
	//}
	// RAW DLINE_UPD
	rddata = ca_raw_upd;
	mmio_wr32(0x016C + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x0174 + PHYD_BASE_ADDR);
}

void cvx16_en_rec_vol_mode(void)
{
	uartlog("cvx16_en_rec_vol_mode\n");
	ddr_debug_wr32(0x54);
	ddr_debug_num_write();
#ifdef DDR2
	rddata = 0x00001001;
	mmio_wr32(0x0500 + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x0540 + PHYD_BASE_ADDR, rddata);
	KC_MSG("cvx16_en_rec_vol_mode done\n");

#endif
}

void cvx16_dll_sw_clr(void)
{
	uint32_t phyd_stop_clk;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x56);
	ddr_debug_num_write();
	phyd_stop_clk = mmio_rd32(cfg_base + 0x30); // phyd_stop_clk
	rddata = modified_bits_by_value(phyd_stop_clk, 0, 9, 9);
	mmio_wr32(cfg_base + 0x30, rddata);
	// param_phyd_sw_dfi_phyupd_req
	rddata = 0x00000101;
	mmio_wr32(0x0174 + PHYD_BASE_ADDR, rddata);
	while (1) {
		// param_phyd_to_reg_sw_phyupd_dline_done
		rddata = mmio_rd32(0x3030 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 24, 24) == 0x1) {
			break;
		}
	}
	mmio_wr32(cfg_base + 0x30, phyd_stop_clk);
}

void cvx16_reg_toggle(void)
{
	uartlog("cvx16_reg_toggle\n");
	ddr_debug_wr32(0x57);
	ddr_debug_num_write();
	rddata = mmio_rd32(0x0 + PHYD_BASE_ADDR);
	mmio_wr32(0x0 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x0 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x4 + PHYD_BASE_ADDR);
	mmio_wr32(0x4 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x4 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x8 + PHYD_BASE_ADDR);
	mmio_wr32(0x8 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x8 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xc + PHYD_BASE_ADDR);
	mmio_wr32(0xc + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xc + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x10 + PHYD_BASE_ADDR);
	mmio_wr32(0x10 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x10 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x14 + PHYD_BASE_ADDR);
	mmio_wr32(0x14 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x14 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x18 + PHYD_BASE_ADDR);
	mmio_wr32(0x18 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x18 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1c + PHYD_BASE_ADDR);
	mmio_wr32(0x1c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x20 + PHYD_BASE_ADDR);
	mmio_wr32(0x20 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x20 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x24 + PHYD_BASE_ADDR);
	mmio_wr32(0x24 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x24 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x28 + PHYD_BASE_ADDR);
	mmio_wr32(0x28 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x28 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x2c + PHYD_BASE_ADDR);
	mmio_wr32(0x2c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x2c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x40 + PHYD_BASE_ADDR);
	mmio_wr32(0x40 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x40 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x44 + PHYD_BASE_ADDR);
	mmio_wr32(0x44 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x44 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x48 + PHYD_BASE_ADDR);
	mmio_wr32(0x48 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x48 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x4c + PHYD_BASE_ADDR);
	mmio_wr32(0x4c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x4c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x50 + PHYD_BASE_ADDR);
	mmio_wr32(0x50 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x50 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x54 + PHYD_BASE_ADDR);
	mmio_wr32(0x54 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x54 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x58 + PHYD_BASE_ADDR);
	mmio_wr32(0x58 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x58 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x5c + PHYD_BASE_ADDR);
	mmio_wr32(0x5c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x5c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x60 + PHYD_BASE_ADDR);
	mmio_wr32(0x60 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x60 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x64 + PHYD_BASE_ADDR);
	mmio_wr32(0x64 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x64 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x68 + PHYD_BASE_ADDR);
	mmio_wr32(0x68 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x68 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x70 + PHYD_BASE_ADDR);
	mmio_wr32(0x70 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x70 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x74 + PHYD_BASE_ADDR);
	mmio_wr32(0x74 + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x74 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x80 + PHYD_BASE_ADDR);
	mmio_wr32(0x80 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x80 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x84 + PHYD_BASE_ADDR);
	mmio_wr32(0x84 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x84 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x88 + PHYD_BASE_ADDR);
	mmio_wr32(0x88 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x88 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x8c + PHYD_BASE_ADDR);
	mmio_wr32(0x8c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x8c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x90 + PHYD_BASE_ADDR);
	mmio_wr32(0x90 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x90 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x94 + PHYD_BASE_ADDR);
	mmio_wr32(0x94 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x94 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa0 + PHYD_BASE_ADDR);
	mmio_wr32(0xa0 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa0 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa4 + PHYD_BASE_ADDR);
	mmio_wr32(0xa4 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa4 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa8 + PHYD_BASE_ADDR);
	mmio_wr32(0xa8 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa8 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xac + PHYD_BASE_ADDR);
	mmio_wr32(0xac + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xac + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb0 + PHYD_BASE_ADDR);
	mmio_wr32(0xb0 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb0 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb4 + PHYD_BASE_ADDR);
	mmio_wr32(0xb4 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb4 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb8 + PHYD_BASE_ADDR);
	mmio_wr32(0xb8 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb8 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xbc + PHYD_BASE_ADDR);
	mmio_wr32(0xbc + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xbc + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xf8 + PHYD_BASE_ADDR);
	mmio_wr32(0xf8 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xf8 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xfc + PHYD_BASE_ADDR);
	mmio_wr32(0xfc + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xfc + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x100 + PHYD_BASE_ADDR);
	mmio_wr32(0x100 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x100 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x104 + PHYD_BASE_ADDR);
	mmio_wr32(0x104 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x104 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x10c + PHYD_BASE_ADDR);
	mmio_wr32(0x10c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x10c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x110 + PHYD_BASE_ADDR);
	mmio_wr32(0x110 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x110 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x114 + PHYD_BASE_ADDR);
	mmio_wr32(0x114 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x114 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x118 + PHYD_BASE_ADDR);
	mmio_wr32(0x118 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x118 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x11c + PHYD_BASE_ADDR);
	mmio_wr32(0x11c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x11c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x120 + PHYD_BASE_ADDR);
	mmio_wr32(0x120 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x120 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x124 + PHYD_BASE_ADDR);
	mmio_wr32(0x124 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x124 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x128 + PHYD_BASE_ADDR);
	mmio_wr32(0x128 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x128 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x12c + PHYD_BASE_ADDR);
	mmio_wr32(0x12c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x12c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x130 + PHYD_BASE_ADDR);
	mmio_wr32(0x130 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x130 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x134 + PHYD_BASE_ADDR);
	mmio_wr32(0x134 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x134 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x138 + PHYD_BASE_ADDR);
	mmio_wr32(0x138 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x138 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x140 + PHYD_BASE_ADDR);
	mmio_wr32(0x140 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x140 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x144 + PHYD_BASE_ADDR);
	mmio_wr32(0x144 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x144 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x148 + PHYD_BASE_ADDR);
	mmio_wr32(0x148 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x148 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x74 + PHYD_BASE_ADDR);
	mmio_wr32(0x74 + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x74 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x14c + PHYD_BASE_ADDR);
	mmio_wr32(0x14c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x14c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x150 + PHYD_BASE_ADDR);
	mmio_wr32(0x150 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x150 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x154 + PHYD_BASE_ADDR);
	mmio_wr32(0x154 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x154 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x158 + PHYD_BASE_ADDR);
	mmio_wr32(0x158 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x158 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x15c + PHYD_BASE_ADDR);
	mmio_wr32(0x15c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x15c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x168 + PHYD_BASE_ADDR);
	mmio_wr32(0x168 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x168 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x16c + PHYD_BASE_ADDR);
	mmio_wr32(0x16c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x16c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x170 + PHYD_BASE_ADDR);
	mmio_wr32(0x170 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x170 + PHYD_BASE_ADDR, rddata);
	// rddata = mmio_rd32(0x174 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x174 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x174 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x180 + PHYD_BASE_ADDR);
	mmio_wr32(0x180 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x180 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x184 + PHYD_BASE_ADDR);
	mmio_wr32(0x184 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x184 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x188 + PHYD_BASE_ADDR);
	mmio_wr32(0x188 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x188 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x18c + PHYD_BASE_ADDR);
	mmio_wr32(0x18c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x18c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x190 + PHYD_BASE_ADDR);
	mmio_wr32(0x190 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x190 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x200 + PHYD_BASE_ADDR);
	mmio_wr32(0x200 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x200 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x204 + PHYD_BASE_ADDR);
	mmio_wr32(0x204 + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x204 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x208 + PHYD_BASE_ADDR);
	mmio_wr32(0x208 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x208 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x220 + PHYD_BASE_ADDR);
	mmio_wr32(0x220 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x220 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x224 + PHYD_BASE_ADDR);
	mmio_wr32(0x224 + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x224 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x228 + PHYD_BASE_ADDR);
	mmio_wr32(0x228 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x228 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x400 + PHYD_BASE_ADDR);
	mmio_wr32(0x400 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x400 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x404 + PHYD_BASE_ADDR);
	mmio_wr32(0x404 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x404 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x408 + PHYD_BASE_ADDR);
	mmio_wr32(0x408 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x408 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x40c + PHYD_BASE_ADDR);
	mmio_wr32(0x40c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x40c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x410 + PHYD_BASE_ADDR);
	mmio_wr32(0x410 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x410 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x414 + PHYD_BASE_ADDR);
	mmio_wr32(0x414 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x414 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x418 + PHYD_BASE_ADDR);
	mmio_wr32(0x418 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x418 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x41c + PHYD_BASE_ADDR);
	mmio_wr32(0x41c + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x41c + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x500 + PHYD_BASE_ADDR);
	mmio_wr32(0x500 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x500 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x504 + PHYD_BASE_ADDR);
	mmio_wr32(0x504 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x504 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x508 + PHYD_BASE_ADDR);
	mmio_wr32(0x508 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x508 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x50c + PHYD_BASE_ADDR);
	mmio_wr32(0x50c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x50c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x510 + PHYD_BASE_ADDR);
	mmio_wr32(0x510 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x510 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x514 + PHYD_BASE_ADDR);
	mmio_wr32(0x514 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x514 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x518 + PHYD_BASE_ADDR);
	mmio_wr32(0x518 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x518 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x51c + PHYD_BASE_ADDR);
	mmio_wr32(0x51c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x51c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x520 + PHYD_BASE_ADDR);
	mmio_wr32(0x520 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x520 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x540 + PHYD_BASE_ADDR);
	mmio_wr32(0x540 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x540 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x544 + PHYD_BASE_ADDR);
	mmio_wr32(0x544 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x544 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x548 + PHYD_BASE_ADDR);
	mmio_wr32(0x548 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x548 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x54c + PHYD_BASE_ADDR);
	mmio_wr32(0x54c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x54c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x550 + PHYD_BASE_ADDR);
	mmio_wr32(0x550 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x550 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x554 + PHYD_BASE_ADDR);
	mmio_wr32(0x554 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x554 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x558 + PHYD_BASE_ADDR);
	mmio_wr32(0x558 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x558 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x55c + PHYD_BASE_ADDR);
	mmio_wr32(0x55c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x55c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x560 + PHYD_BASE_ADDR);
	mmio_wr32(0x560 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x560 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x900 + PHYD_BASE_ADDR);
	mmio_wr32(0x900 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x900 + PHYD_BASE_ADDR, rddata);
	// rddata = mmio_rd32(0x904 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x904 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x904 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x908 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x908 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x908 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x90c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x90c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x90c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x910 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x910 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x910 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x914 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x914 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x914 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x918 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x918 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x918 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x91c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x91c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x91c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x920 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x920 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x920 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x924 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x924 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x924 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x928 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x928 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x928 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x92c + PHYD_BASE_ADDR);
	mmio_wr32(0x92c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x92c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x930 + PHYD_BASE_ADDR);
	mmio_wr32(0x930 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x930 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x934 + PHYD_BASE_ADDR);
	mmio_wr32(0x934 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x934 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x938 + PHYD_BASE_ADDR);
	mmio_wr32(0x938 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x938 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x940 + PHYD_BASE_ADDR);
	mmio_wr32(0x940 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x940 + PHYD_BASE_ADDR, rddata);
	// rddata = mmio_rd32(0x944 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x944 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x944 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x948 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x948 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x948 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x94c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x94c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x94c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x950 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x950 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x950 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x954 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x954 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x954 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x958 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x958 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x958 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x95c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x95c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x95c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x960 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x960 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x960 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x964 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x964 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x964 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x968 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x968 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x968 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x96c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x96c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x96c + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x970 + PHYD_BASE_ADDR);
	mmio_wr32(0x970 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x970 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x974 + PHYD_BASE_ADDR);
	mmio_wr32(0x974 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x974 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x978 + PHYD_BASE_ADDR);
	mmio_wr32(0x978 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x978 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x97c + PHYD_BASE_ADDR);
	mmio_wr32(0x97c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x97c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x980 + PHYD_BASE_ADDR);
	mmio_wr32(0x980 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x980 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa00 + PHYD_BASE_ADDR);
	mmio_wr32(0xa00 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa00 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa04 + PHYD_BASE_ADDR);
	mmio_wr32(0xa04 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa04 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa08 + PHYD_BASE_ADDR);
	mmio_wr32(0xa08 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa08 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa0c + PHYD_BASE_ADDR);
	mmio_wr32(0xa0c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa0c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa10 + PHYD_BASE_ADDR);
	mmio_wr32(0xa10 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa10 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa14 + PHYD_BASE_ADDR);
	mmio_wr32(0xa14 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa14 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa18 + PHYD_BASE_ADDR);
	mmio_wr32(0xa18 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa18 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa1c + PHYD_BASE_ADDR);
	mmio_wr32(0xa1c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa1c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa20 + PHYD_BASE_ADDR);
	mmio_wr32(0xa20 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa20 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa24 + PHYD_BASE_ADDR);
	mmio_wr32(0xa24 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa24 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa28 + PHYD_BASE_ADDR);
	mmio_wr32(0xa28 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa28 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa2c + PHYD_BASE_ADDR);
	mmio_wr32(0xa2c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa2c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa30 + PHYD_BASE_ADDR);
	mmio_wr32(0xa30 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa30 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa34 + PHYD_BASE_ADDR);
	mmio_wr32(0xa34 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa34 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa38 + PHYD_BASE_ADDR);
	mmio_wr32(0xa38 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa38 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa3c + PHYD_BASE_ADDR);
	mmio_wr32(0xa3c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa3c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa40 + PHYD_BASE_ADDR);
	mmio_wr32(0xa40 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa40 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa44 + PHYD_BASE_ADDR);
	mmio_wr32(0xa44 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa44 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa48 + PHYD_BASE_ADDR);
	mmio_wr32(0xa48 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa48 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa4c + PHYD_BASE_ADDR);
	mmio_wr32(0xa4c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa4c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa50 + PHYD_BASE_ADDR);
	mmio_wr32(0xa50 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa50 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa54 + PHYD_BASE_ADDR);
	mmio_wr32(0xa54 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa54 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa58 + PHYD_BASE_ADDR);
	mmio_wr32(0xa58 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa58 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa5c + PHYD_BASE_ADDR);
	mmio_wr32(0xa5c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa5c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa60 + PHYD_BASE_ADDR);
	mmio_wr32(0xa60 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa60 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa64 + PHYD_BASE_ADDR);
	mmio_wr32(0xa64 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa64 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa68 + PHYD_BASE_ADDR);
	mmio_wr32(0xa68 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa68 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa6c + PHYD_BASE_ADDR);
	mmio_wr32(0xa6c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa6c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa70 + PHYD_BASE_ADDR);
	mmio_wr32(0xa70 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa70 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa74 + PHYD_BASE_ADDR);
	mmio_wr32(0xa74 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa74 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa78 + PHYD_BASE_ADDR);
	mmio_wr32(0xa78 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa78 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xa7c + PHYD_BASE_ADDR);
	mmio_wr32(0xa7c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xa7c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb00 + PHYD_BASE_ADDR);
	mmio_wr32(0xb00 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb00 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb04 + PHYD_BASE_ADDR);
	mmio_wr32(0xb04 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb04 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb08 + PHYD_BASE_ADDR);
	mmio_wr32(0xb08 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb08 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb0c + PHYD_BASE_ADDR);
	mmio_wr32(0xb0c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb0c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb10 + PHYD_BASE_ADDR);
	mmio_wr32(0xb10 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb10 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb14 + PHYD_BASE_ADDR);
	mmio_wr32(0xb14 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb14 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb18 + PHYD_BASE_ADDR);
	mmio_wr32(0xb18 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb18 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb1c + PHYD_BASE_ADDR);
	mmio_wr32(0xb1c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb1c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb20 + PHYD_BASE_ADDR);
	mmio_wr32(0xb20 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb20 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb24 + PHYD_BASE_ADDR);
	mmio_wr32(0xb24 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb24 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb30 + PHYD_BASE_ADDR);
	mmio_wr32(0xb30 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb30 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb34 + PHYD_BASE_ADDR);
	mmio_wr32(0xb34 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb34 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb38 + PHYD_BASE_ADDR);
	mmio_wr32(0xb38 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb38 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb3c + PHYD_BASE_ADDR);
	mmio_wr32(0xb3c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb3c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb40 + PHYD_BASE_ADDR);
	mmio_wr32(0xb40 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb40 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb44 + PHYD_BASE_ADDR);
	mmio_wr32(0xb44 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb44 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb48 + PHYD_BASE_ADDR);
	mmio_wr32(0xb48 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb48 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb4c + PHYD_BASE_ADDR);
	mmio_wr32(0xb4c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb4c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb50 + PHYD_BASE_ADDR);
	mmio_wr32(0xb50 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb50 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0xb54 + PHYD_BASE_ADDR);
	mmio_wr32(0xb54 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0xb54 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1400 + PHYD_BASE_ADDR);
	mmio_wr32(0x1400 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1400 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1404 + PHYD_BASE_ADDR);
	mmio_wr32(0x1404 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1404 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1408 + PHYD_BASE_ADDR);
	mmio_wr32(0x1408 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1408 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x140c + PHYD_BASE_ADDR);
	mmio_wr32(0x140c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x140c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1410 + PHYD_BASE_ADDR);
	mmio_wr32(0x1410 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1410 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1414 + PHYD_BASE_ADDR);
	mmio_wr32(0x1414 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1414 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1418 + PHYD_BASE_ADDR);
	mmio_wr32(0x1418 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1418 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x141c + PHYD_BASE_ADDR);
	mmio_wr32(0x141c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x141c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1500 + PHYD_BASE_ADDR);
	mmio_wr32(0x1500 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1500 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1504 + PHYD_BASE_ADDR);
	mmio_wr32(0x1504 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1504 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1508 + PHYD_BASE_ADDR);
	mmio_wr32(0x1508 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1508 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x150c + PHYD_BASE_ADDR);
	mmio_wr32(0x150c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x150c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1510 + PHYD_BASE_ADDR);
	mmio_wr32(0x1510 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1510 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1514 + PHYD_BASE_ADDR);
	mmio_wr32(0x1514 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1514 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1518 + PHYD_BASE_ADDR);
	mmio_wr32(0x1518 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1518 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x151c + PHYD_BASE_ADDR);
	mmio_wr32(0x151c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x151c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1520 + PHYD_BASE_ADDR);
	mmio_wr32(0x1520 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1520 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1540 + PHYD_BASE_ADDR);
	mmio_wr32(0x1540 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1540 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1544 + PHYD_BASE_ADDR);
	mmio_wr32(0x1544 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1544 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1548 + PHYD_BASE_ADDR);
	mmio_wr32(0x1548 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1548 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x154c + PHYD_BASE_ADDR);
	mmio_wr32(0x154c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x154c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1550 + PHYD_BASE_ADDR);
	mmio_wr32(0x1550 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1550 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1554 + PHYD_BASE_ADDR);
	mmio_wr32(0x1554 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1554 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1558 + PHYD_BASE_ADDR);
	mmio_wr32(0x1558 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1558 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x155c + PHYD_BASE_ADDR);
	mmio_wr32(0x155c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x155c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1560 + PHYD_BASE_ADDR);
	mmio_wr32(0x1560 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1560 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1900 + PHYD_BASE_ADDR);
	mmio_wr32(0x1900 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1900 + PHYD_BASE_ADDR, rddata);
	// rddata = mmio_rd32(0x1904 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1904 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1904 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1908 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1908 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1908 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x190c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x190c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x190c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1910 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1910 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1910 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1914 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1914 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1914 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1918 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1918 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1918 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x191c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x191c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x191c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1920 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1920 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1920 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1924 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1924 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1924 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1928 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1928 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1928 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x192c + PHYD_BASE_ADDR);
	mmio_wr32(0x192c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x192c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1930 + PHYD_BASE_ADDR);
	mmio_wr32(0x1930 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1930 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1934 + PHYD_BASE_ADDR);
	mmio_wr32(0x1934 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1934 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1938 + PHYD_BASE_ADDR);
	mmio_wr32(0x1938 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1938 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1940 + PHYD_BASE_ADDR);
	mmio_wr32(0x1940 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1940 + PHYD_BASE_ADDR, rddata);
	// rddata = mmio_rd32(0x1944 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1944 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1944 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1948 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1948 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1948 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x194c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x194c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x194c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1950 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1950 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1950 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1954 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1954 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1954 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1958 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1958 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1958 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x195c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x195c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x195c + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1960 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1960 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1960 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1964 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1964 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1964 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x1968 + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x1968 + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x1968 + PHYD_BASE_ADDR, rddata );
	// rddata = mmio_rd32(0x196c + PHYD_BASE_ADDR);
	// mmio_wr32    ( 0x196c + PHYD_BASE_ADDR, ~rddata );
	// mmio_wr32    ( 0x196c + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x1970 + PHYD_BASE_ADDR);
	mmio_wr32(0x1970 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1970 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1974 + PHYD_BASE_ADDR);
	mmio_wr32(0x1974 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1974 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1978 + PHYD_BASE_ADDR);
	mmio_wr32(0x1978 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1978 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x197c + PHYD_BASE_ADDR);
	mmio_wr32(0x197c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x197c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1980 + PHYD_BASE_ADDR);
	mmio_wr32(0x1980 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1980 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a00 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a00 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a00 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a04 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a04 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a04 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a08 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a08 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a08 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a0c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a0c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a0c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a10 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a10 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a10 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a14 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a14 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a14 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a18 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a18 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a18 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a1c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a1c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a1c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a20 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a20 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a20 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a24 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a24 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a24 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a28 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a28 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a28 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a2c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a2c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a2c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a30 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a30 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a30 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a34 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a34 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a34 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a38 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a38 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a38 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a3c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a3c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a3c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a40 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a40 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a40 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a44 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a44 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a44 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a48 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a48 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a48 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a4c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a4c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a4c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a50 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a50 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a50 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a54 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a54 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a54 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a58 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a58 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a58 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a5c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a5c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a5c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a60 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a60 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a60 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a64 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a64 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a64 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a68 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a68 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a68 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a6c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a6c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a6c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a70 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a70 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a70 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a74 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a74 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a74 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a78 + PHYD_BASE_ADDR);
	mmio_wr32(0x1a78 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a78 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1a7c + PHYD_BASE_ADDR);
	mmio_wr32(0x1a7c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1a7c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b00 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b00 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b00 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b04 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b04 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b04 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b08 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b08 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b08 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b0c + PHYD_BASE_ADDR);
	mmio_wr32(0x1b0c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b0c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b10 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b10 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b10 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b14 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b14 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b14 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b18 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b18 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b18 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b1c + PHYD_BASE_ADDR);
	mmio_wr32(0x1b1c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b1c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b20 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b20 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b20 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b24 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b24 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b24 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b30 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b30 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b30 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b34 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b34 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b34 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b38 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b38 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b38 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b3c + PHYD_BASE_ADDR);
	mmio_wr32(0x1b3c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b3c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b40 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b40 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b40 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b44 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b44 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b44 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b48 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b48 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b48 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b4c + PHYD_BASE_ADDR);
	mmio_wr32(0x1b4c + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b4c + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b50 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b50 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b50 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x1b54 + PHYD_BASE_ADDR);
	mmio_wr32(0x1b54 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x1b54 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x164 + PHYD_BASE_ADDR);
	mmio_wr32(0x164 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x164 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x41c + PHYD_BASE_ADDR);
	mmio_wr32(0x41c + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x41c + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x204 + PHYD_BASE_ADDR);
	mmio_wr32(0x204 + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x204 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x224 + PHYD_BASE_ADDR);
	mmio_wr32(0x224 + PHYD_BASE_ADDR, ~rddata);
	// mmio_wr32    ( 0x224 + PHYD_BASE_ADDR, rddata );
	rddata = mmio_rd32(0x3000 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3004 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3008 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x300c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3010 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3014 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3018 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x301c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3020 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3024 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3028 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x302c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3030 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3100 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3104 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3110 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3140 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3144 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3150 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3180 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3184 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3188 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x318c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3190 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3194 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3198 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x319c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31a0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31a4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31a8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31ac + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31b0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31b4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31c0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31c4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31c8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31cc + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31d0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31d4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31d8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31dc + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31e0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31e4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31e8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31ec + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31f0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x31f4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3280 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3284 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3288 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x328c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3290 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3294 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3298 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x329c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32a0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32a4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32a8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32c0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32c4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32c8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32cc + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32d0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32d4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32d8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32dc + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32e0 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32e4 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x32e8 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3370 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3380 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3400 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3404 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x340c + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3410 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3414 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3418 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x3444 + PHYD_BASE_ADDR);
	rddata = mmio_rd32(0x00 + CV_DDR_PHYD_APB);
	mmio_wr32(0x00 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x00 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x04 + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x08 + CV_DDR_PHYD_APB);
	mmio_wr32(0x08 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x08 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	rddata = mmio_rd32(0x10 + CV_DDR_PHYD_APB);
	mmio_wr32(0x10 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x10 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x1C + CV_DDR_PHYD_APB);
	mmio_wr32(0x1C + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x1C + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x20 + CV_DDR_PHYD_APB);
	mmio_wr32(0x20 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x20 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x24 + CV_DDR_PHYD_APB);
	mmio_wr32(0x24 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x24 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x28 + CV_DDR_PHYD_APB);
	mmio_wr32(0x28 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x28 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x44 + CV_DDR_PHYD_APB);
	mmio_wr32(0x44 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x44 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x48 + CV_DDR_PHYD_APB);
	mmio_wr32(0x48 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x48 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x4C + CV_DDR_PHYD_APB);
	mmio_wr32(0x4C + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x4C + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x54 + CV_DDR_PHYD_APB);
	mmio_wr32(0x54 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x54 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x58 + CV_DDR_PHYD_APB);
	mmio_wr32(0x58 + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x58 + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, ~rddata);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	rddata = mmio_rd32(0x00F0 + PHYD_BASE_ADDR);
	mmio_wr32(0x00F0 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x00F0 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x00F4 + PHYD_BASE_ADDR);
	mmio_wr32(0x00F4 + PHYD_BASE_ADDR, ~rddata);
	mmio_wr32(0x00F4 + PHYD_BASE_ADDR, rddata);
	mmio_wr32(0x00F4 + PHYD_BASE_ADDR, 0x00030033);
	mmio_wr32(0x092C + PHYD_BASE_ADDR, 0x00000008);
	for (int i = 0; i < 23; i++) {
		rddata = i << 24 | i << 16 | i << 8 | i;
		mmio_wr32(0x0000 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0004 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0008 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x000C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0010 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0014 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0018 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x001C + PHYD_BASE_ADDR, rddata);
		rddata = i << 28 | i << 24 | i << 20 | i << 16 | i << 12 | i << 8 | i << 4 | i;
		mmio_wr32(0x0020 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0024 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0028 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x002C + PHYD_BASE_ADDR, rddata);
	}
	for (int i = 0; i < 23; i++) {
		rddata = i << 24 | i << 16 | i << 8 | i;
		mmio_wr32(0x011C + PHYD_BASE_ADDR, rddata);
	}
	for (int i = 0; i < 128; i++) {
		rddata = i << 24 | i << 16 | i << 8 | i;
		mmio_wr32(0x0B0C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0B10 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0B14 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0B3C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0B40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x0B44 + PHYD_BASE_ADDR, rddata);
	}
	cvx16_dll_cal();
	for (int i = 0; i < 64; i++) {
		rddata = i << 24 | i << 16 | i << 8 | i;
		mmio_wr32(0x900 + PHYD_BASE_ADDR, rddata);
		// mmio_wr32( 0x904 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x908 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x90C + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x910 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x914 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x918 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x91C + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x920 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x924 + PHYD_BASE_ADDR, rddata );
		// mmio_wr32( 0x928 + PHYD_BASE_ADDR, rddata );
		mmio_wr32(0x92C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x930 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x934 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0x938 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA00 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA04 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA08 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA0C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA10 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA14 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA18 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA40 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA44 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA48 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA4C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA50 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA54 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xA58 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB00 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB04 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB08 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB0C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB10 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB30 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB34 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB38 + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB3C + PHYD_BASE_ADDR, rddata);
		mmio_wr32(0xB40 + PHYD_BASE_ADDR, rddata);
		cvx16_lb_1_dq_set_highlow();
		cvx16_lb_3_ca_set_highlow();
		cvx16_dll_sw_clr();
		KC_MSG("shift = %x\n", i);
	}
}

void cvx16_ana_test(void)
{
	uint32_t i;
	uint32_t j;

	uartlog("%s\n", __func__);
	ddr_debug_wr32(0x58);
	ddr_debug_num_write();
	//[31] TOP_REG_TX_ZQ_PD =0
	//[24] TOP_REG_TX_CA_PD_CKE0=0
	rddata = 0x7EFFFFFF;
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	//[1] TOP_REG_TX_CA_GPIO_OENZ=0
	//[2] TOP_REG_TX_CA_SEL_GPIO_CKE0=1
	rddata = 0x00000065;
	mmio_wr32(0x1C + CV_DDR_PHYD_APB, rddata);
	//[2] REG_DDRPLL_EN_TST=0b1
	rddata = mmio_rd32(0x0C + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 2, 2);
	mmio_wr32(0x0C + CV_DDR_PHYD_APB, rddata);
	//[15] param_phya_reg_tx_zq_en_test_mux
	rddata = mmio_rd32(0x014C + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 1, 1);
	mmio_wr32(0x014C + PHYD_BASE_ADDR, rddata);
	//[0] param_phya_reg_en_test
	rddata = mmio_rd32(0x0134 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	mmio_wr32(0x0134 + PHYD_BASE_ADDR, rddata);
	uartlog("start ana test\n");
	for (i = 1; i < 9; i = i + 1) {
		KC_MSG("param_phya_reg_zq_sel_test_out=%x\n", get_bits_from_value(i, 3, 0));

		// param_phya_reg_zq_sel_test_out0	7	4
		// param_phya_reg_zq_sel_test_out1	11	8
		rddata = mmio_rd32(0x014C + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, get_bits_from_value(i, 3, 0), 7, 4);
		rddata = modified_bits_by_value(rddata, get_bits_from_value(i, 3, 0), 11, 8);
		mmio_wr32(0x014C + PHYD_BASE_ADDR, rddata);
		for (j = 0; j <= 15; j = j + 1) {
			KC_MSG("param_phya_reg_rx_byte0_sel_test_in=%x\n", get_bits_from_value(j, 3, 0));

			// param_phya_reg_rx_byte0_sel_test_in0	19 16
			// param_phya_reg_rx_byte0_sel_test_in1	23 20
			rddata = mmio_rd32(0x0200 + PHYD_BASE_ADDR);
			rddata = modified_bits_by_value(rddata, get_bits_from_value(j, 3, 0), 19, 16);
			rddata = modified_bits_by_value(rddata, get_bits_from_value(j, 3, 0), 23, 20);
			// param_phya_reg_rx_byte1_sel_test_in0  19  16
			// param_phya_reg_rx_byte1_sel_test_in1  23  20
			mmio_wr32(0x0200 + PHYD_BASE_ADDR, rddata);
			rddata = mmio_rd32(0x0220 + PHYD_BASE_ADDR);
			rddata = modified_bits_by_value(rddata, get_bits_from_value(j, 3, 0), 19, 16);
			rddata = modified_bits_by_value(rddata, get_bits_from_value(j, 3, 0), 23, 20);
			mmio_wr32(0x0220 + PHYD_BASE_ADDR, rddata);
			// opdelay(1000);
		}
	}
	KC_MSG("%s Fisish\n", __func__);
}

void cvx16_ddr_zq240(void)
{
	//        KC_MSG("cv181x without ZQ240 Calibration ...\n");

	int i;
	int pre_zq240_cmp_out;

	KC_MSG("START ZQ240 Calibration ...\n");

	uartlog("ddr_zq240\n");
	ddr_debug_wr32(0x2c1);
	ddr_debug_num_write();
	// VDDQ_TXr        = 0.6;
	//------------------------------
	//  Init setting
	//------------------------------
	// param_phyd_zqcal_hw_mode =1
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 31, 31);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_ZQ_PD = 0...\n");

	// param_phya_reg_sel_zq_high_swing  <= `PI_SD int_regin[2];
	rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 2, 2);
	mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_sel_lpddr4_pmos_ph <= `PI_SD int_regin[2];
	rddata = mmio_rd32(0x0400 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	mmio_wr32(0x0400 + PHYD_BASE_ADDR, rddata);
	// b.
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 4, 0);
	rddata = modified_bits_by_value(rddata, 0x10, 12, 8);
	rddata = modified_bits_by_value(rddata, 0x10, 20, 16);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_zq_drvn[5:0] <= `PI_SD int_regin[20:16];
	// param_phya_reg_tx_zq_drvp[5:0] <= `PI_SD int_regin[28:24];
	rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x9, 20, 16);
	rddata = modified_bits_by_value(rddata, 0x9, 28, 24);
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// d.
	// param_phya_reg_tx_zq_en_test_aux <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_en_test_mux  <= `PI_SD int_regin[1];
	rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 1, 0);
	mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
	// TOP_REG_TX_SEL_GPIO         <= #RD (~pwstrb_mask[7] & TOP_REG_TX_SEL_GPIO) |  pwstrb_mask_pwdata[7];
	//  TOP_REG_TX_GPIO_OENZ        <= #RD (~pwstrb_mask[6] & TOP_REG_TX_GPIO_OENZ) |  pwstrb_mask_pwdata[6];
	rddata = mmio_rd32(0x1c + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 7, 6);
	mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
	//------------------------------
	// CMP offset Cal.
	//------------------------------
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	KC_MSG("zq_cmp_en\n");

	//------------------------------
	// ZQ Complete
	//------------------------------
	uartlog("wait hw_done\n");
	// param_phyd_to_reg_zqcal_hw_done
	while (1) {
		rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 16, 16) == 1) {
			break;
		}
		KC_MSG("wait param_phyd_to_reg_zqcal_hw_done ...\n");
	}
	// check param_phya_reg_tx_zq_cmp_offset
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	KC_MSG("reg_tx_zq_cmp_offset = %x\n", get_bits_from_value(rddata, 12, 8));

	uartlog("hw_done\n");
	KC_MSG("param_phyd_to_reg_zqcal_hw_done ...\n");

	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	// param_phyd_zqcal_hw_mode =0
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	//------------------------------
	// ZQ240
	//------------------------------
	KC_MSG("START ZQ240 Calibration - ZQ 240 ...\n");

	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	//         rddata=modified_bits_by_value(rddata, 0, 19, 16 );
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x15, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	rddata = modified_bits_by_value(rddata, 1, 19, 16);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	pre_zq240_cmp_out = 0x0;
	for (i = 0; i < 16; i = i + 1) {
		rddata = 0x00000001; // TOP_REG_ZQ_EN_ZQ_240_TRIM
		rddata = modified_bits_by_value(rddata, i, 19, 16); // TOP_REG_ZQ_TRIM_ZQ_240_TRIM
		mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
		// opdelay(128);
		KC_MSG("START ZQ240 Calibration ==> round %x...\n", i);

		rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
		// param_phya_to_reg_zq_cmp_out
		KC_MSG("START ZQ240 Calibration - cmp_out = %x\n", get_bits_from_value(rddata, 24, 24));

		if ((pre_zq240_cmp_out == 0x1) && (get_bits_from_value(rddata, 24, 24) == 0x0)) {
			KC_MSG("ZQ240 Calibration = %x\n", i);

			rddata = 0x00000001; // TOP_REG_ZQ_EN_ZQ_240
			rddata = modified_bits_by_value(rddata, i, 19, 16); // TOP_REG_ZQ_TRIM_ZQ_240
			mmio_wr32(0x54 + CV_DDR_PHYD_APB, rddata);
			i = 18;
		}
		pre_zq240_cmp_out = get_bits_from_value(rddata, 24, 24);
	}
	if ((pre_zq240_cmp_out == 0x0) || (i == 16)) {
		KC_MSG("Error !!! ZQ240 Calibration\n");
	}
	uartlog("zq240 done\n");
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	// TOP_REG_ZQ_EN_ZQ_240_TRIM =0
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_ZQ_PD = 1...\n");

	// opdelay(200);
	KC_MSG("ZQ240 Complete ...\n");
}

void cvx16_ddr_zq240_ate(void)
{
	//        KC_MSG("cv181x without ZQ240 Calibration ...\n");

	int i;
	int pre_zq240_cmp_out;

	KC_MSG("START ZQ240 Calibration ...\n");

	uartlog("ddr_zq240\n");
	ddr_debug_wr32(0x2c1);
	ddr_debug_num_write();
	// VDDQ_TXr        = 0.6;
	//------------------------------
	//  Init setting
	//------------------------------
	// param_phyd_zqcal_hw_mode =1
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 31, 31);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_ZQ_PD = 0...\n");

	// param_phya_reg_sel_zq_high_swing  <= `PI_SD int_regin[2];
	rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 2, 2);
	mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_sel_lpddr4_pmos_ph <= `PI_SD int_regin[2];
	rddata = mmio_rd32(0x0400 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 5, 5);
	mmio_wr32(0x0400 + PHYD_BASE_ADDR, rddata);
	// b.
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 4, 0);
	rddata = modified_bits_by_value(rddata, 0x10, 12, 8);
	rddata = modified_bits_by_value(rddata, 0x10, 20, 16);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	// param_phya_reg_tx_zq_drvn[5:0] <= `PI_SD int_regin[20:16];
	// param_phya_reg_tx_zq_drvp[5:0] <= `PI_SD int_regin[28:24];
	rddata = mmio_rd32(0x0148 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x9, 20, 16);
	rddata = modified_bits_by_value(rddata, 0x9, 28, 24);
	mmio_wr32(0x0148 + PHYD_BASE_ADDR, rddata);
	// d.
	// param_phya_reg_tx_zq_en_test_aux <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_en_test_mux  <= `PI_SD int_regin[1];
	rddata = mmio_rd32(0x014c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 1, 0);
	mmio_wr32(0x014c + PHYD_BASE_ADDR, rddata);
	// TOP_REG_TX_SEL_GPIO         <= #RD (~pwstrb_mask[7] & TOP_REG_TX_SEL_GPIO) |  pwstrb_mask_pwdata[7];
	//  TOP_REG_TX_GPIO_OENZ        <= #RD (~pwstrb_mask[6] & TOP_REG_TX_GPIO_OENZ) |  pwstrb_mask_pwdata[6];
	rddata = mmio_rd32(0x1c + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 7, 6);
	mmio_wr32(0x1c + CV_DDR_PHYD_APB, rddata);
	//------------------------------
	// CMP offset Cal.
	//------------------------------
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x15, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	KC_MSG("zq_cmp_en\n");

	//------------------------------
	// ZQ Complete
	//------------------------------
	uartlog("wait hw_done\n");
	// param_phyd_to_reg_zqcal_hw_done
	while (1) {
		rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
		if (get_bits_from_value(rddata, 16, 16) == 1) {
			break;
		}
		KC_MSG("wait param_phyd_to_reg_zqcal_hw_done ...\n");
	}
	// check param_phya_reg_tx_zq_cmp_offset
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	KC_MSG("reg_tx_zq_cmp_offset = %x\n", get_bits_from_value(rddata, 12, 8));

	uartlog("hw_done\n");
	KC_MSG("param_phyd_to_reg_zqcal_hw_done ...\n");

	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	// rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	// rddata=modified_bits_by_value(rddata, 0, 4, 0 );
	// mmio_wr32(0x0144 + PHYD_BASE_ADDR,  rddata);
	// param_phyd_zqcal_hw_mode =0
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	//------------------------------
	// ZQ240
	//------------------------------
	KC_MSG("START ZQ240 Calibration - ZQ 240 ...\n");

	// rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	// rddata=modified_bits_by_value(rddata, 1, 4, 0 );
	// mmio_wr32(0x0144 + PHYD_BASE_ADDR,  rddata);
	// rddata = mmio_rd32(0x50+CV_DDR_PHYD_APB);
	// rddata=modified_bits_by_value(rddata, 1, 0, 0 );
	//          rddata=modified_bits_by_value(rddata, 0, 19, 16 );
	// mmio_wr32(0x50+CV_DDR_PHYD_APB,  rddata);
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	// rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	// rddata=modified_bits_by_value(rddata, 0x15, 4, 0 );
	// mmio_wr32(0x0144 + PHYD_BASE_ADDR,  rddata);
	// rddata = mmio_rd32(0x50+CV_DDR_PHYD_APB);
	rddata = 0x00000000;
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	rddata = modified_bits_by_value(rddata, 1, 19, 16);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	pre_zq240_cmp_out = 0x0;
	for (i = 0; i < 16; i = i + 1) {
		rddata = 0x00000001; // TOP_REG_ZQ_EN_ZQ_240_TRIM
		rddata = modified_bits_by_value(rddata, i, 19, 16); // TOP_REG_ZQ_TRIM_ZQ_240_TRIM
		mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
		KC_MSG("START ZQ240 Calibration ==> round %x...\n", i);

		rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
		// param_phya_to_reg_zq_cmp_out
		KC_MSG("START ZQ240 Calibration - cmp_out = %x\n", get_bits_from_value(rddata, 24, 24));
	}
	uartlog("zq240 done\n");
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	// rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	// rddata=modified_bits_by_value(rddata, 0, 4, 0 );
	// mmio_wr32(0x0144 + PHYD_BASE_ADDR,  rddata);
	// TOP_REG_ZQ_EN_ZQ_240_TRIM =0
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_ZQ_PD = 1...\n");

	// opdelay(200);
	KC_MSG("ZQ240 Complete ...\n");
}

void cvx16_ddr_zq240_cal(void)
{
	//        KC_MSG("cv181x without ZQ240 Calibration ...\n");

	int i;
	int pre_zq240_cmp_out;

	KC_MSG("START ZQ240 Calibration ...\n");

	uartlog("ddr_zq240_cal\n");
	ddr_debug_wr32(0x2c1);
	ddr_debug_num_write();
	//------------------------------
	// ZQ240
	//------------------------------
	KC_MSG("START ZQ240 Calibration - ZQ 240 ...\n");

	// param_phyd_zqcal_hw_mode =0
	rddata = mmio_rd32(0x0074 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 18, 16);
	mmio_wr32(0x0074 + PHYD_BASE_ADDR, rddata);
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 31, 31);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_ZQ_PD = 0...\n");

	// param_phya_reg_tx_zq_cmp_en = 1
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	// TOP_REG_ZQ_EN_ZQ_240_TRIM = 1
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 0, 0);
	//         rddata=modified_bits_by_value(rddata, 0, 19, 16 );
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];            =1
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];            =1
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];            =1
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0x15, 4, 0);
#ifdef DDR4
	rddata = modified_bits_by_value(rddata, 0x10, 20, 16);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
#else
	// for zq pull high 60ohm
	rddata = modified_bits_by_value(rddata, 0x1A, 20, 16);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
#endif
	// rddata = mmio_rd32(0x50+CV_DDR_PHYD_APB);
	// rddata=modified_bits_by_value(rddata, 1, 0, 0 );
	// rddata=modified_bits_by_value(rddata, 1, 19, 16 );
	// mmio_wr32(0x50+CV_DDR_PHYD_APB,  rddata);
	pre_zq240_cmp_out = 0x0;
	for (i = 0; i < 16; i = i + 1) {
		rddata = 0x00000001; // TOP_REG_ZQ_EN_ZQ_240_TRIM
		rddata = modified_bits_by_value(rddata, i, 19, 16); // TOP_REG_ZQ_TRIM_ZQ_240_TRIM
		mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
		// opdelay(128);
		KC_MSG("START ZQ240 Calibration ==> round %x...\n", i);

		rddata = mmio_rd32(0x3440 + PHYD_BASE_ADDR);
		// param_phya_to_reg_zq_cmp_out
		KC_MSG("START ZQ240 Calibration - cmp_out = %x\n", get_bits_from_value(rddata, 24, 24));

		if ((pre_zq240_cmp_out == 0x1) && (get_bits_from_value(rddata, 24, 24) == 0x0)) {
			KC_MSG("ZQ240 Calibration = %x\n", i);

			rddata = 0x00000001; // TOP_REG_ZQ_EN_ZQ_240
			rddata = modified_bits_by_value(rddata, i, 19, 16); // TOP_REG_ZQ_TRIM_ZQ_240
			mmio_wr32(0x54 + CV_DDR_PHYD_APB, rddata);
			i = 18;
		}
		pre_zq240_cmp_out = get_bits_from_value(rddata, 24, 24);
	}
	if ((i == 16)) {
		KC_MSG("Error !!! ZQ240 Calibration\n");
	}
	uartlog("zq240 done\n");
	// param_phya_reg_tx_zq_cmp_en    <= `PI_SD int_regin[0];
	// param_phya_reg_tx_zq_cmp_offset_cal_en <= `PI_SD int_regin[1];
	// param_phya_reg_tx_zq_ph_en     <= `PI_SD int_regin[2];
	// param_phya_reg_tx_zq_pl_en     <= `PI_SD int_regin[3];
	// param_phya_reg_tx_zq_step2_en  <= `PI_SD int_regin[4];
	// param_phya_reg_tx_zq_cmp_offset[4:0] <= `PI_SD int_regin[12:8];
	// param_phya_reg_tx_zq_sel_vref[4:0] <= `PI_SD int_regin[20:16];
	rddata = mmio_rd32(0x0144 + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 0, 4, 0);
	mmio_wr32(0x0144 + PHYD_BASE_ADDR, rddata);
	// TOP_REG_ZQ_EN_ZQ_240_TRIM =0
	rddata = mmio_rd32(0x50 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 0, 1, 1);
	mmio_wr32(0x50 + CV_DDR_PHYD_APB, rddata);
	// TOP_REG_TX_ZQ_PD        <= #RD (~pwstrb_mask[31] & TOP_REG_TX_ZQ_PD) |  pwstrb_mask_pwdata[31];
	rddata = mmio_rd32(0x40 + CV_DDR_PHYD_APB);
	rddata = modified_bits_by_value(rddata, 1, 31, 31);
	mmio_wr32(0x40 + CV_DDR_PHYD_APB, rddata);
	KC_MSG("TOP_REG_TX_ZQ_PD = 1...\n");

	// opdelay(200);
	KC_MSG("ZQ240 Complete ...\n");
}

void ctrl_init_detect_dram_size(uint8_t *dram_cap_in_mbyte)
{
	uint8_t cap_in_mbyte = 0;
#ifdef DDR3
	uint32_t cmd[6];
	uint8_t i;

	// dram_cap_in_mbyte = 4;
	cap_in_mbyte = 4;

	for (i = 0; i < 6; i++)
		cmd[i] = 0x0;

	// Axsize = 3, axlen = 4, cgen
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x000e0006);

	// DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0xffffffff);

	// specified AXI address step
	mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);

	// write PRBS to 0x0 as background {{{
	// write 16 UI prbs
	cmd[0] = (1 << 30) + (0 << 21) + (3 << 12) + (5 << 9) + (0 << 8) + (0 << 0);

	for (i = 0; i < 6; i++) {
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
	}

	// bist_enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);

	// polling BIST done

	do {
		rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
	} while (get_bits_from_value(rddata, 2, 2) == 0);

	// bist disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010000);
	// }}}

	do {
		// *dram_cap_in_mbyte++;
		cap_in_mbyte++;
		uartlog("cap_in_mbyte =  %x\n", cap_in_mbyte);

		// write ~PRBS to (0x1 << *dram_cap_in_mbyte) {{{

		// DDR space
		mmio_wr32(DDR_BIST_BASE + 0x10, 1 << (cap_in_mbyte + 20 - 4));
		// write 16 UI ~prbs
		cmd[0] = (1 << 30) + (0 << 21) + (3 << 12) + (5 << 9) + (1 << 8) + (0 << 0);

		for (i = 0; i < 6; i++) {
			mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
		}

		// bist_enable
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);
		// polling BIST done

		do {
			rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
		} while (get_bits_from_value(rddata, 2, 2) == 0);

		// bist disable
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010000);
		// }}}

		// check PRBS at 0x0 {{{

		// DDR space
		mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
		cmd[0] = (2 << 30) + (0 << 21) + (3 << 12) + (5 << 9) + (0 << 8) + (0 << 0); // read 16 UI prbs

		for (i = 0; i < 6; i++) {
			mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
		}

		// bist_enable
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);
		// polling BIST done

		do {
			rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
		} while (get_bits_from_value(rddata, 2, 2) == 0);

		// bist disable
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010000);
		// }}}

	} while ((get_bits_from_value(rddata, 3, 3) == 0) && (cap_in_mbyte < 15)); // BIST fail stop the loop

#endif
#ifdef DDR2
	// fix size for DDR2
	cap_in_mbyte = 6;
#endif
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		uint32_t cmd[6];
		uint8_t i;

		// dram_cap_in_mbyte = 4;
		cap_in_mbyte = 4;

		for (i = 0; i < 6; i++)
			cmd[i] = 0x0;

		// Axsize = 3, axlen = 4, cgen
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x000e0006);

		// DDR space
		mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
		mmio_wr32(DDR_BIST_BASE + 0x14, 0xffffffff);

		// specified AXI address step
		mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);

		// write PRBS to 0x0 as background {{{

		cmd[0] = (1 << 30) + (0 << 21) + (3 << 12) + (5 << 9) + (0 << 8) + (0 << 0); // write 16 UI prbs

		for (i = 0; i < 6; i++) {
			mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
		}

		// bist_enable
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);

		// polling BIST done

		do {
			rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
		} while (get_bits_from_value(rddata, 2, 2) == 0);

		// bist disable
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010000);
		// }}}

		do {
			// *dram_cap_in_mbyte++;
			cap_in_mbyte++;
			uartlog("cap_in_mbyte =  %x\n", cap_in_mbyte);

			// write ~PRBS to (0x1 << *dram_cap_in_mbyte) {{{

			// DDR space
			mmio_wr32(DDR_BIST_BASE + 0x10, 1 << (cap_in_mbyte + 20 - 4));
			// write 16 UI ~prbs
			cmd[0] = (1 << 30) + (0 << 21) + (3 << 12) + (5 << 9) + (1 << 8) + (0 << 0);

			for (i = 0; i < 6; i++) {
				mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
			}

			// bist_enable
			mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);
			// polling BIST done

			do {
				rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
			} while (get_bits_from_value(rddata, 2, 2) == 0);

			// bist disable
			mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010000);
			// }}}

			// check PRBS at 0x0 {{{

			// DDR space
			mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
			// read 16 UI prbs
			cmd[0] = (2 << 30) + (0 << 21) + (3 << 12) + (5 << 9) + (0 << 8) + (0 << 0);

			for (i = 0; i < 6; i++) {
				mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);
			}

			// bist_enable
			mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);
			// polling BIST done

			do {
				rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
			} while (get_bits_from_value(rddata, 2, 2) == 0);

			// bist disable
			mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010000);
			// }}}

		} while ((get_bits_from_value(rddata, 3, 3) == 0) && (cap_in_mbyte < 15)); // BIST fail stop the loop
	}
	if (get_ddr_type() == DDR_TYPE_DDR2)	{
		// fix size for DDR2
		cap_in_mbyte = 6;
	}
#endif

	*dram_cap_in_mbyte = cap_in_mbyte;

	// save dram_cap_in_mbyte
	rddata = cap_in_mbyte;
	mmio_wr32(0x0208 + PHYD_BASE_ADDR, rddata);

	// cgen disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00040000);
}

#if defined(FULL_MEM_BIST) || defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)
uint32_t ddr_bist_all(uint32_t mode, uint32_t capacity, uint32_t x16_mode)
{
	uint32_t axi_len8;
	int loop;
	uint32_t cmd[6];
	uint64_t cap;
	uint32_t pattern;
	uint32_t bist_result = 1;
	uint32_t sram_sp;
	uint32_t fmax;
	uint32_t fmin;

	// mode--> 0/1/2 = prbs/sram/01
	// capacity--> 0/1/2/4/8/16 = 0.5/1/2/4/8/16 Gb
	// KC  KC_MSG("capacity=%x, x16_mode=%x\n", capacity, x16_mode);

	if (capacity == 0)
		cap = (2 << 9) << (20 - 3);
	else
		cap = capacity << (30 - 3);

	if (mode == 0) {
		//prbs
		pattern = 5;
		sram_sp = 511;
	} else if (mode == 1) {
		//sram
		pattern = 6;
		fmax = 15;
		fmin = 5;
		sram_sp = 511; //for all dram size, repeat sso pattern
		//sram_sp = 9 * (fmin + fmax) * (fmax - fmin + 1) / 2 / 4 + (fmax - fmin + 1); // 8*f/4 -1
		//KC_MSG("sram_sp = %x\n", sram_sp);

		// bist sso_period
		mmio_wr32(DDR_BIST_BASE + 0x24, (fmax << 8) + fmin);

	} else {
		pattern = 1;
		sram_sp = 511;
	}

	loop = cap / (512 * 4 * 16 * 4 / (2 << x16_mode)) - 1;

	// check xpi len
	rddata = mmio_rd32(cfg_base + 0xc);
	axi_len8 = FIELD_GET(rddata, 19, 19);
	KC_MSG("cap=%x, loop=%x, axi_len8=%x\n", cap, loop, axi_len8);

	// bist clock enable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00060006);
	// sram based cmd
	//        op_code     start       stop              pattern          dq_inv     dm_inv    dq_auto_rotate repeat
	cmd[0] = (1 << 30) | (0 << 21) | (sram_sp << 12) | (pattern << 9) | (0 << 8) | (0 << 7) | (0 << 4) | (15 << 0);
	cmd[1] = (2 << 30) | (0 << 21) | (sram_sp << 12) | (pattern << 9) | (0 << 8) | (0 << 7) | (0 << 4) | (15 << 0);
	cmd[3] = (1 << 30) | (0 << 21) | (sram_sp << 12) | (pattern << 9) | (1 << 8) | (0 << 7) | (0 << 4) | (15 << 0);
	cmd[4] = (2 << 30) | (0 << 21) | (sram_sp << 12) | (pattern << 9) | (1 << 8) | (0 << 7) | (0 << 4) | (15 << 0);
	//        op_code     addr_not_assert    goto_idx    loop
	cmd[2] = (3 << 30) | (1 << 20) | (0 << 16) | (loop << 0);
	cmd[5] = (3 << 30) | (1 << 20) | (3 << 16) | (loop << 0);

	// write cmd queue
	for (int i = 0; i < 6; i++)
		mmio_wr32(DDR_BIST_BASE + 0x40 + i * 4, cmd[i]);

	// DDR space
	mmio_wr32(DDR_BIST_BASE + 0x10, 0x00000000);
	mmio_wr32(DDR_BIST_BASE + 0x14, 0x00ffffff);
	// set AXI_LEN8
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00080000 | (axi_len8 << 3));
	// specified AXI address step
	if (x16_mode == 0x1) {
		if (axi_len8 == 0x1)
			mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
		else
			mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000002);
	} else {
		if (axi_len8 == 0x1)
			mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000008);
		else
			mmio_wr32(DDR_BIST_BASE + 0x18, 0x00000004);
	}
	// bist_enable & x16_en
	if (x16_mode == 0x1) {
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00030003);
	} else {
		mmio_wr32(DDR_BIST_BASE + 0x0, 0x00010001);
	}
	// polling bist done
	while (1) {
		rddata = mmio_rd32(DDR_BIST_BASE + 0x80);
		if (FIELD_GET(rddata, 2, 2) == 1)
			break;
	}

	//aspi_axi_sr(`SPI_REG_DDR_BIST + 0x80, ptest_aspi_spi_rdata, 0x0000_000C, "INFO: polling bist done and pass")
	if (FIELD_GET(rddata, 3, 2) == 1) {
		KC_MSG("bist_Pass\n");
		bist_result = 1 & bist_result;
	} else {
		KC_MSG("ERROR bist_fail\n");
		bist_result = 0;
	}
	// bist disable
	mmio_wr32(DDR_BIST_BASE + 0x0, 0x00050000);

	return bist_result;
}

uint32_t bist_all_dram(uint32_t mode, uint32_t capacity)
{
	// mode--> 0/1/2 = prbs/sram/01
	// capacity--> 0/1/2/4/8/16 = 0.5/1/2/4/8/16 Gb
	uint32_t bist_result;

	bist_result = ddr_bist_all(mode, capacity, 1);

	if (bist_result == 0)
		NOTICE("ERROR bist_fail!(%d, %d)\n", mode, capacity);
	else
		NOTICE("ALL Bist_Pass!(%d, %d)\n", mode, capacity);

	return bist_result;
}

void bist_all_dram_forever(uint32_t capacity)
{
	uint32_t count = 0;
	uint32_t bist_result = 1;
	// uint32_t sram_sp;

	// sso_8x1_c(5, 15, 0, 1, &sram_sp);
	// sso_8x1_c(5, 15, sram_sp, 1, &sram_sp);
	while (1) {
		NOTICE("%d\n", count++);
		bist_result &= bist_all_dram(0, capacity);
		bist_result &= bist_all_dram(1, capacity);
		bist_result &= bist_all_dram(2, capacity);
		if (bist_result == 0) {
			NOTICE("BIST stress test FAIL\n");
			break;
		}
	}
}
#endif // defined(FULL_MEM_BIST) || defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)

#if defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)
static void ddr_sys_init(void)
{
	KC_MSG("reset  !\n");
	mmio_wr32(DDR_TOP_BASE + 0x20, 0x1);

	KC_MSG("PLL INIT !\n");
	cvx16_pll_init();

	KC_MSG("DDRC_INIT !\n");
	// ddrc_init_ddr3_4g_2133();
	ddrc_init();

	// cvx16_ctrlupd_short();

	// release ddrc soft reset
	KC_MSG("releast reset  !\n");
	mmio_wr32(DDR_TOP_BASE + 0x20, 0x0);

	KC_MSG("phy_init!\n");
	phy_init();

	cvx16_setting_check();
	KC_MSG("cvx16_setting_check  finish");

	cvx16_pinmux();
	KC_MSG("cvx16_pinmux finish");

	ddr_patch_set();

	cvx16_en_rec_vol_mode();
	KC_MSG("cvx16_en_rec_vol_mode finish");

	// set_dfi_init_start
	cvx16_set_dfi_init_start();
	KC_MSG("set_dfi_init_start finish\n");

	// ddr_phy_power_on_seq1
	cvx16_ddr_phy_power_on_seq1();
	KC_MSG("ddr_phy_power_on_seq1 finish\n");

	// first dfi_init_start
	KC_MSG("first dfi_init_start\n");

	cvx16_polling_dfi_init_start();

	KC_MSG("cvx16_polling_dfi_init_start finish");

	cvx16_INT_ISR_08();
	KC_MSG("cvx16_INT_ISR_08 finish");

	// ddr_phy_power_on_seq3
	cvx16_ddr_phy_power_on_seq3();
	KC_MSG("ddr_phy_power_on_seq3 finish\n");

	// wait_for_dfi_init_complete
	cvx16_wait_for_dfi_init_complete();
	KC_MSG("wait_for_dfi_init_complete finish\n");

	// polling_synp_normal_mode
	cvx16_polling_synp_normal_mode();
	KC_MSG("polling_synp_normal_mode finish\n");
}

static void set_ca_vref(uint32_t vref)
{
	rddata = mmio_rd32(0x0414 + PHYD_BASE_ADDR);
	rddata = FIELD_SET(rddata, vref, 20, 16); //param_phya_reg_tx_vrefca_sel
	mmio_wr32(0x0414 + PHYD_BASE_ADDR, rddata);
	uartlog("vrefca = %08x\n", vref);
	//time.sleep(0.01);
	mdelay(10);
}
#endif // defined(SHMOO_CA) || defined(SHMOO_CS)

#if defined(DBG_SHMOO_CA)
static uint32_t bist_all_dram_calvl(uint32_t mode, uint32_t capacity, uint32_t shift_delay, uint32_t vrefca)
{
	// capacity--> 0/1/2/4/8/16 = 0.5/1/2/4/8/16 Gb
	uint32_t bist_result;
	uint32_t bist_err;

	bist_result = ddr_bist_all(mode, capacity, 1);

	if (bist_result == 0) {
		bist_err = 0xffffffff;
	} else {
		bist_err = 0x00000000;
	}
	SHMOO_MSG_CA("vref = %02x, sw_ca__training_start = %08x , err_data_rise/err_data_fall = %08x, %08x\n",
		  vrefca, shift_delay, bist_err, bist_err);

	return bist_result;
}
#endif // defined(DBG_SHMOO_CA)

#if defined(DBG_SHMOO_CS)
static uint32_t bist_all_dram_cslvl(uint32_t mode, uint32_t capacity, uint32_t shift_delay, uint32_t vrefca)
{
	// capacity--> 0/1/2/4/8/16 = 0.5/1/2/4/8/16 Gb
	uint32_t bist_result;
	uint32_t bist_err;

	bist_result = ddr_bist_all(mode, capacity, 1);

	if (bist_result == 0) {
		bist_err = 0xffffffff;
	} else {
		bist_err = 0x00000000;
	}
	SHMOO_MSG_CS("vref = %02x, sw_cs__training_start = %08x , err_data_rise/err_data_fall = %08x, %08x\n",
		  vrefca, shift_delay, bist_err, bist_err);

	return bist_result;
}
#endif // defined(DBG_SHMOO_CS)

#if defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)
static void bist_single(enum bist_mode mode)
{
	//uint32_t sram_sp;
	uint32_t bist_result;
	uint64_t err_data_odd;
	uint64_t err_data_even;

	if (mode == E_PRBS)
		cvx16_bist_wr_prbs_init();
	if (mode == E_SRAM) {
		cvx16_bist_wr_sram_init();
	}

	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);

	KC_MSG("bist_result=%x, err_data_odd=%x, err_data_even=%x\n", bist_result, err_data_odd, err_data_even);
}

void ddr_training(enum train_mode t_mode)
{
	uint32_t mode;
	uint32_t rddata;

	// sram_sp =0
	if (t_mode == E_WRLVL) {
		// wrlvl
		cvx16_wrlvl_req();
		// cvx16_wrlvl_status();
		bist_single(E_PRBS);
	}

	if (t_mode == E_RDGLVL) {
		// rdglvl
		cvx16_rdglvl_req();
		// cvx16_rdglvl_status();
		bist_single(E_PRBS);
	}

	if (t_mode == E_WDQLVL) {
		// wdqlvl
		mode = 1;
		// data_mode = 'h0 : phyd pattern
		// data_mode = 'h1 : bist read/write
		// data_mode = 'h11: with Error enject,  multi- bist write/read
		// data_mode = 'h12: with Error enject,  multi- bist write/read
		// lvl_mode  = 'h0 : wdmlvl
		// lvl_mode  = 'h1 : wdqlvl
		// lvl_mode  = 'h2 : wdqlvl and wdmlvl
		// cvx16_wdqlvl_req( data_mode,  lvl_mode,  sram_sp)
		if (mode == 0) {
			cvx16_wdqlvl_req(0, 2); // dq/dm
		} else {
			if (mode == 1) {
				cvx16_wdqlvl_req(1, 2); // dq/dm
				// cvx16_wdqlvl_req(1, 1); // dq
				// cvx16_wdqlvl_req(1, 0); // dm
			}
		}
		// cvx16_wdqlvl_status();
		bist_single(E_PRBS);
	}

	if (t_mode == E_RDLVL) {
		// rdlvl
		mode = 1;
		// mode = 'h0  : MPR mode, DDR3 only.
		// mode = 'h1  : sram write/read continuous goto
		// mode = 'h2  : multi- bist write/read
		// mode = 'h10 : with Error enject,  multi- bist write/read
		// mode = 'h12 : with Error enject,  multi- bist write/read

		if (mode == 0) {
			rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
			rddata = modified_bits_by_value(rddata, 1, 4, 4); // Dataflow from MPR
			mmio_wr32(0x0188 + PHYD_BASE_ADDR, rddata);
			cvx16_rdlvl_req(0);
			rddata = mmio_rd32(0x0188 + PHYD_BASE_ADDR);
			rddata = modified_bits_by_value(rddata, 0, 4, 4); // Dataflow from MPR
			mmio_wr32(0x0188 + PHYD_BASE_ADDR, rddata);
		} else {
			cvx16_rdlvl_req(mode);
		}
		// cvx16_rdlvl_status();
		bist_single(E_PRBS);
	}

	if (t_mode == E_WDQLVL_SW) {
		// wdqlvl_sw

		KC_MSG("wdqlvl_SW_M1_ALL\n");
		// data_mode = 'h0 : phyd pattern
		// data_mode = 'h1 : bist read/write
		// data_mode = 'h11: with Error enject,  multi- bist write/read
		// data_mode = 'h12: with Error enject,  multi- bist write/read
		// lvl_mode  = 'h0 : wdmlvl
		// lvl_mode  = 'h1 : wdqlvl
		// lvl_mode  = 'h2 : wdqlvl and wdmlvl
		// cvx16_wdqlvl_req(data_mode, lvl_mode)
		rddata = 0x00000000;
		rddata = modified_bits_by_value(rddata, 0x0c, 6, 0);  //param_phyd_dfi_wdqlvl_vref_start
		rddata = modified_bits_by_value(rddata, 0x13, 14, 8); //param_phyd_dfi_wdqlvl_vref_end
		rddata = modified_bits_by_value(rddata, 0x2, 19, 16); //param_phyd_dfi_wdqlvl_vref_step
		mmio_wr32(0x0190 + PHYD_BASE_ADDR, rddata);
		KC_MSG("cvx16_wdqlvl_sw_req\n");
		cvx16_wdqlvl_sw_req(1, 2);
		// cvx16_wdqlvl_status();
		KC_MSG("cvx16_wdqlvl_req dq/dm finish\n");
	}

	if (t_mode == E_RDLVL_SW) {
		// rdlvl_sw
		rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
		rddata = modified_bits_by_value(rddata, 1, 7, 4); //param_phyd_pirdlvl_capture_cnt
		mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);

		KC_MSG("SW mode 1, sram write/read continuous goto\n");
		cvx16_rdlvl_sw_req(1);
		// cvx16_rdlvl_status();
		KC_MSG("cvx16_rdlvl_req finish\n");
	}
}
#endif // defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)

#ifdef DBG_SHMOO_CA
void calvl_req(uint32_t capacity)
{
	uint32_t i, j;
	uint32_t vrefca_start = 0x02;
	uint32_t vrefca_end   = 0x1f;
	uint32_t vrefca_step  = 0x02;
	uint32_t shift_start  = 0x02;
	uint32_t shift_end    = 0x07;
	uint32_t delay_start  = 0x00;
	uint32_t delay_end    = 0x40;
	uint32_t delay_step   = 0x04;

	uartlog("=== calvl_req ===\n");
	// if DDR3_4G:
	//     capacity=4
	// if DDR3_2G:
	//     capacity=2
	// if DDR3_1G:
	//     capacity=1
	// if DDR3_DBG:
	//     capacity=4
	// if N25_DDR2_512:
	//     capacity=0

	// for i in range(vrefca_start, vrefca_end+1, vrefca_step):
	for (i = vrefca_start; i < (vrefca_end + 1); i += vrefca_step) {
		uint32_t vrefca_sel = i;
		uint32_t shift_delay_start = (shift_start << 7) + delay_start;
		uint32_t shift_delay_end = (shift_end << 7) + delay_end + 1;
		uint32_t bist_result = 0;

		uartlog("vrefca_start = %08x\n", i);
		// for j in range(shift_delay_start, shift_delay_end, delay_step):
		for (j = shift_delay_start; j < shift_delay_end; j += delay_step) {
			uint32_t shift_delay;

			if (bist_result == 0) {
				ddr_sys_init();
				ddr_training(E_WRLVL);
				ddr_training(E_RDGLVL);
			#ifdef DDR3_DBG
				ddr_training(E_RDLVL);
			#endif //DDR3_DBG
				ddr_training(E_WDQLVL);
				ddr_training(E_RDLVL);
				cvx16_clk_gating_disable();
				set_ca_vref(vrefca_sel);
			}
			// #mmio_wr32(0x0414 + PHYD_BASE_ADDR, vrefca_sel) #param_phya_reg_tx_vrefca_sel
			// #time.sleep(0.1)
			uartlog("shift_delay_before = %08x\n", j);

			shift_delay = ((get_bits_from_value(j, 12, 7)) << 8) + (get_bits_from_value(j, 6, 0));

			uartlog("shift_delay_after = %08x\n", shift_delay);
			cvx16_ca_shift_delay(shift_delay);
			// #bist_single_calvl(mode="prbs", shift_delay=j, vrefca=vrefca_sel)

		#if 1 //ca park 1
			cvx16_dfi_ca_park_prbs(1);
			bist_result = bist_all_dram_calvl(0, capacity, j, vrefca_sel); //0: prbs
			cvx16_dfi_ca_park_prbs(0);
		#else //ca park 0
			cvx16_dfi_ca_park_prbs(0);
			bist_result = bist_all_dram_calvl(0, capacity, j, vrefca_sel); //0: prbs
			cvx16_dfi_ca_park_prbs(0);
		#endif
		}
	}
	// re-init
	ddr_sys_init();
	ddr_training(E_WRLVL);
	ddr_training(E_RDGLVL);
#ifdef DDR3_DBG
	ddr_training(E_RDLVL);
#endif //DDR3_DBG
	ddr_training(E_WDQLVL);
	ddr_training(E_RDLVL);

	cvx16_clk_gating_enable();
}
#endif // DBG_SHMOO_CA

#ifdef DBG_SHMOO_CS
void cslvl_req(uint32_t capacity)
{
	uint32_t i, j;
	//TODO
	uint32_t vrefca_start = 0x02;
	uint32_t vrefca_end   = 0x1f;
	uint32_t vrefca_step  = 0x02;
	uint32_t shift_start  = 0x02;
	uint32_t shift_end    = 0x05;
	uint32_t delay_start  = 0x40;
	uint32_t delay_end    = 0x40;
	uint32_t delay_step   = 0x04;

	uartlog("=== cslvl_req ===\n");
	// if DDR3_4G:
	//     capacity=4
	// if DDR3_2G:
	//     capacity=2
	// if DDR3_1G:
	//     capacity=1
	// if DDR3_DBG:
	//     capacity=4
	// if N25_DDR2_512:
	//     capacity=0

	// for i in range(vrefca_start, vrefca_end+1, vrefca_step):
	for (i = vrefca_start; i < (vrefca_end + 1); i += vrefca_step) {
		uint32_t vrefca_sel = i;
		uint32_t shift_delay_start = (shift_start << 7) + delay_start;
		uint32_t shift_delay_end = (shift_end << 7) + delay_end + 1;
		uint32_t bist_result = 0;

		uartlog("vrefca_start = %08x\n", i);
		// for j in range(shift_delay_start, shift_delay_end, delay_step):
		for (j = shift_delay_start; j < shift_delay_end; j += delay_step) {
			uint32_t shift_delay;

			if (bist_result == 0) {
				ddr_sys_init();
				ddr_training(E_WRLVL);
				ddr_training(E_RDGLVL);
			#ifdef DDR3_DBG
				ddr_training(E_RDLVL);
			#endif //DDR3_DBG
				ddr_training(E_WDQLVL);
				ddr_training(E_RDLVL);
				cvx16_clk_gating_disable();
				set_ca_vref(vrefca_sel);
			}
			// #mmio_wr32(0x0414 + PHYD_BASE_ADDR, vrefca_sel) #param_phya_reg_tx_vrefca_sel
			// #time.sleep(0.1)
			uartlog("shift_delay_before = %08x\n", j);

			shift_delay = ((get_bits_from_value(j, 12, 7)) << 8) + (get_bits_from_value(j, 6, 0));

			uartlog("shift_delay_after = %08x\n", shift_delay);
			cvx16_cs_shift_delay(shift_delay);
			// #bist_single_calvl(mode="prbs", shift_delay=j, vrefca=vrefca_sel)

		#if 1 //ca park 1
			cvx16_dfi_ca_park_prbs(1);
			bist_result = bist_all_dram_cslvl(0, capacity, j, vrefca_sel); //0: prbs
			cvx16_dfi_ca_park_prbs(0);
		#else //ca park 0
			cvx16_dfi_ca_park_prbs(0);
			bist_result = bist_all_dram_cslvl(0, capacity, j, vrefca_sel); //0: prbs
			cvx16_dfi_ca_park_prbs(0);
		#endif
		}
	}
	// re-init
	ddr_sys_init();
	ddr_training(E_WRLVL);
	ddr_training(E_RDGLVL);
#ifdef DDR3_DBG
	ddr_training(E_RDLVL);
#endif //DDR3_DBG
	ddr_training(E_WDQLVL);
	ddr_training(E_RDLVL);

	cvx16_clk_gating_enable();
}
#endif // DBG_SHMOO_CS

#pragma GCC diagnostic pop
