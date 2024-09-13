#include <platform_def.h>
#include <reg_soc.h>
#include <phy_pll_init.h>
#include <ddr_sys.h>
#ifdef DDR2_3
#include <ddr3_1866_init.h>
#include <ddr2_1333_init.h>
#else
#include <ddr_init.h>
#endif
#include <mmio.h>
#include <bitwise_ops.h>
#include <cvx16_dram_cap_check.h>
#include <cvx16_pinmux.h>
#include <regconfig.h>
#include <console.h>
#include <ddr_pkg_info.h>

#define DO_BIST

#define REMAPPING_BASE 0
#define AXIMON_M1_WRITE	(REMAPPING_BASE + 0x0)
#define AXIMON_M1_READ	(REMAPPING_BASE + 0x80)
#define AXIMON_M2_WRITE	(REMAPPING_BASE + 0x100)
#define AXIMON_M2_READ	(REMAPPING_BASE + 0x180)
#define AXIMON_M3_WRITE	(REMAPPING_BASE + 0x200)
#define AXIMON_M3_READ	(REMAPPING_BASE + 0x280)
#define AXIMON_M4_WRITE	(REMAPPING_BASE + 0x300)
#define AXIMON_M4_READ	(REMAPPING_BASE + 0x380)
#define AXIMON_M5_WRITE	(REMAPPING_BASE + 0x400)
#define AXIMON_M5_READ	(REMAPPING_BASE + 0x480)
#define AXIMON_M6_WRITE	(REMAPPING_BASE + 0x500)
#define AXIMON_M6_READ	(REMAPPING_BASE + 0x580)

#define AXIMON_OFFSET_CYCLE 0x24
#define AXIMON_OFFSET_BYTECNTS 0x2C
#define AXIMON_OFFSET_LATENCYCNTS 0x34
#define AXIMON_OFFSET_HITCNTS 0x28
#define AXIMON_OFFSET_LAT_BIN_SIZE_SEL 0x50

#define AXIMON_START_REGVALUE 0x30001
#define AXIMON_STOP_REGVALUE 0x30002

static void axi_mon_latency_setting(uint32_t lat_bin_size_sel)
{
	uint32_t rdata;

	//for ddr3 1866: bin_size_sel=0d'5
	mmio_wr32((AXI_MON_BASE + AXIMON_M1_WRITE + AXIMON_OFFSET_LAT_BIN_SIZE_SEL), lat_bin_size_sel);
	mmio_wr32((AXI_MON_BASE + AXIMON_M1_READ + AXIMON_OFFSET_LAT_BIN_SIZE_SEL), lat_bin_size_sel);

	mmio_wr32((AXI_MON_BASE + AXIMON_M1_WRITE + 0x00), 0x01000100);//input clk sel
	rdata = mmio_rd32((AXI_MON_BASE + AXIMON_M1_WRITE + 0x04));//hit sel setting
	rdata = rdata & 0xfffffc00;
	rdata = rdata | 0x00000000;
	mmio_wr32((AXI_MON_BASE + AXIMON_M1_WRITE + 0x04), rdata);

	mmio_wr32((AXI_MON_BASE + AXIMON_M1_READ + 0x00), 0x01000100);
	rdata = mmio_rd32((AXI_MON_BASE + AXIMON_M1_READ + 0x04));
	rdata = rdata & 0xfffffc00;
	rdata = rdata | 0x00000000;
	mmio_wr32((AXI_MON_BASE + AXIMON_M1_READ + 0x04), rdata);

	mmio_wr32((AXI_MON_BASE + AXIMON_M5_WRITE + AXIMON_OFFSET_LAT_BIN_SIZE_SEL), lat_bin_size_sel);
	mmio_wr32((AXI_MON_BASE + AXIMON_M5_READ + AXIMON_OFFSET_LAT_BIN_SIZE_SEL), lat_bin_size_sel);

	mmio_wr32((AXI_MON_BASE + AXIMON_M5_WRITE + 0x00), 0x01000100);
	rdata = mmio_rd32((AXI_MON_BASE + AXIMON_M5_WRITE + 0x04));
	rdata = rdata & 0xfffffc00;
	rdata = rdata | 0x00000000;
	mmio_wr32((AXI_MON_BASE + AXIMON_M5_WRITE + 0x04), rdata);

	mmio_wr32((AXI_MON_BASE + AXIMON_M5_READ + 0x00), 0x01000100);
	rdata = mmio_rd32((AXI_MON_BASE + AXIMON_M5_READ + 0x04));
	rdata = rdata & 0xfffffc00;
	rdata = rdata | 0x00000000;
	mmio_wr32((AXI_MON_BASE + AXIMON_M5_READ + 0x04), rdata);

	//ERROR("mon cg en.\n");
	rdata = mmio_rd32((DDR_TOP_BASE+0x14));
	rdata = rdata | 0x00000100;
	mmio_wr32((DDR_TOP_BASE+0x14), rdata);
}

static void load_ddr_patch_set_data(void)
{
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

static void ddr_patch_set(void)
{
	load_ddr_patch_set_data();

#ifdef DDR3_DBG
	mmio_wr32(0x005C + PHYD_BASE_ADDR, 0x00FE0000); //wrlvl response only DQ0
#endif //DDR3_DBG
}

static void axi_mon_start(uint32_t base_register)
{
	mmio_wr32((AXI_MON_BASE + base_register), AXIMON_START_REGVALUE);
}

void axi_mon_start_all(void)
{
	axi_mon_start(AXIMON_M1_WRITE);
	axi_mon_start(AXIMON_M1_READ);
	axi_mon_start(AXIMON_M2_WRITE);
	axi_mon_start(AXIMON_M2_READ);
	axi_mon_start(AXIMON_M3_WRITE);
	axi_mon_start(AXIMON_M3_READ);
	axi_mon_start(AXIMON_M4_WRITE);
	axi_mon_start(AXIMON_M4_READ);
	axi_mon_start(AXIMON_M5_WRITE);
	axi_mon_start(AXIMON_M5_READ);
	axi_mon_start(AXIMON_M6_WRITE);
	axi_mon_start(AXIMON_M6_READ);
}

void dump_axi_mon_reg(uint32_t base_register)
{
	uint i = 0;

	for (i = 0; i <= 0x7c; i = i+0x10) {
		ERROR("0x%08x: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n", AXI_MON_BASE + base_register+i,
				mmio_rd32(AXI_MON_BASE + base_register+i),
				mmio_rd32(AXI_MON_BASE + base_register+i+0x4),
				mmio_rd32(AXI_MON_BASE + base_register+i+0x8),
				mmio_rd32(AXI_MON_BASE + base_register+i+0xc));
	}
}

void ddr_sys_bring_up(void)
{
	uint32_t bist_result;
	uint64_t err_data_even, err_data_odd;
	uint8_t dram_cap_in_mbyte;

#if defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS) || defined(FULL_MEM_BIST) || defined(FULL_MEM_BIST_FOREVER)
	uint32_t cap = 1;

	// switch (get_ddr_capacity()) {
	// case DDR_CAPACITY_1G: cap = 1; break;
	// case DDR_CAPACITY_2G: cap = 2; break;
	// case DDR_CAPACITY_4G: cap = 4; break;
	// default: cap = 1;
	// }
	uartlog("cap=%d\n", cap);
#endif //FULL_MEM_BIST || FULL_MEM_BIST_FOREVER

	uartlog("%s pattern!\n", __func__);

	// mmio_wr32(PLLG6_BASE+top_pll_g6_reg_ddr_ssc_syn_src_en,
	//            mmio_rd32(PLLG6_BASE+top_pll_g6_reg_ddr_ssc_syn_src_en)
	//            &(~top_pll_g6_reg_ddr_ssc_syn_src_en_MASK)
	//            |0x1<<top_pll_g6_reg_ddr_ssc_syn_src_en_OFFSET);

#ifdef REAL_DDRPHY
	uartlog("PLL INIT !\n");
	pll_init();
#endif

	uartlog("DDRC_INIT !\n");
	ddrc_init();

	// cvx16_ctrlupd_short();

	// release ddrc soft reset
	uartlog("releast reset  !\n");
	mmio_wr32(DDR_TOP_BASE + 0x20, 0x0);

	// set axi QOS
	// M1 = 0xA (VIP realtime)
	// M2 = 0x8 (VIP offline)
	// M3 = 0x7 (CPU)
	// M4 = 0x0 (TPU)
	// M5 = 0x9 (Video codec)
	// M6 = 0x2 (high speed peri)
	mmio_wr32(0x030001D8, 0x007788aa);
	mmio_wr32(0x030001DC, 0x00002299);

#ifdef REAL_DDRPHY
	uartlog("phy_init !\n");
	phy_init();
#endif
	#ifdef ENABLE_FASTBOOT0
	mmio_wr32(0x080000a4, 0x02620504);
	#endif
	cvx16_setting_check();
	KC_MSG("cvx16_setting_check  finish\n");

	// pinmux
	cvx16_pinmux();
	KC_MSG("cvx16_pinmux finish\n");

	ddr_patch_set();

	cvx16_en_rec_vol_mode();
	KC_MSG("cvx16_en_rec_vol_mode finish\n");

	// set_dfi_init_start
	cvx16_set_dfi_init_start();
	KC_MSG("set_dfi_init_start finish\n");

	// ddr_phy_power_on_seq1
	cvx16_ddr_phy_power_on_seq1();
	KC_MSG("ddr_phy_power_on_seq1 finish\n");

	// first dfi_init_start
	KC_MSG("first dfi_init_start\n");
	cvx16_polling_dfi_init_start();
	KC_MSG("cvx16_polling_dfi_init_start finish\n");

	cvx16_INT_ISR_08();
	KC_MSG("cvx16_INT_ISR_08 finish\n");

	// ddr_phy_power_on_seq3
	cvx16_ddr_phy_power_on_seq3();
	KC_MSG("ddr_phy_power_on_seq3 finish\n");

	// wait_for_dfi_init_complete
	cvx16_wait_for_dfi_init_complete();
	KC_MSG("wait_for_dfi_init_complete finish\n");

	// polling_synp_normal_mode
	cvx16_polling_synp_normal_mode();
	KC_MSG("polling_synp_normal_mode finish\n");

#ifdef DO_BIST
	cvx16_bist_wr_prbs_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		ERROR("ERROR bist_fail\n");
		ERROR("bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
			  err_data_even);
	}
#endif

	ctrl_init_low_patch();
	KC_MSG("ctrl_low_patch finish\n");

	// cvx16_wrlvl_req
#ifndef DDR2
#ifdef DDR2_3
	if (get_ddr_type() == DDR_TYPE_DDR3) {
		cvx16_wrlvl_req();
		KC_MSG("cvx16_wrlvl_req finish\n");
	}
#else
	cvx16_wrlvl_req();
	KC_MSG("cvx16_wrlvl_req finish\n");
#endif
#endif

#ifdef DO_BIST
	cvx16_bist_wr_prbs_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		ERROR("ERROR bist_fail\n");
		ERROR("bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
			  err_data_even);
	}
#endif
	// cvx16_rdglvl_req
	cvx16_rdglvl_req();
	KC_MSG("cvx16_rdglvl_req finish\n");
#ifdef DO_BIST
	cvx16_bist_wr_prbs_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		ERROR("ERROR bist_fail\n");
		ERROR("bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
			  err_data_even);
	}
#endif

	//ERROR("AXI mon setting for latency histogram.\n");
	//axi_mon_set_lat_bin_size(0x5);

#ifdef DBG_SHMOO
	// DPHY WDQ
	// param_phyd_dfi_wdqlvl_vref_start [6:0]
	// param_phyd_dfi_wdqlvl_vref_end [14:8]
	// param_phyd_dfi_wdqlvl_vref_step [19:16]
	mmio_wr32(0x08000190, 0x00021E02);

	// param_phyd_piwdqlvl_dly_step[23:20]
	mmio_wr32(0x080000a4, 0x01220504);

	// write start   shift = 5  /  dline = 78
	mmio_wr32(0x080000a0, 0x0d400578);

	//write
	KC_MSG("wdqlvl_M1_ALL_DQ_DM\n");
	// data_mode = 'h0 : phyd pattern
	// data_mode = 'h1 : bist read/write
	// data_mode = 'h11: with Error enject,  multi- bist write/read
	// data_mode = 'h12: with Error enject,  multi- bist write/read
	// lvl_mode  = 'h0 : wdmlvl
	// lvl_mode  = 'h1 : wdqlvl
	// lvl_mode  = 'h2 : wdqlvl and wdmlvl
	// cvx16_wdqlvl_req(data_mode, lvl_mode)
	NOTICE("cvx16_wdqlvl_sw_req dq/dm\n"); console_getc();
	cvx16_wdqlvl_sw_req(1, 2);
	// cvx16_wdqlvl_status();
	KC_MSG("cvx16_wdqlvl_req dq/dm finish\n");

	NOTICE("cvx16_wdqlvl_sw_req dq\n"); console_getc();
	cvx16_wdqlvl_sw_req(1, 1);
	// cvx16_wdqlvl_status();
	KC_MSG("cvx16_wdqlvl_req dq finish\n");

	NOTICE("cvx16_wdqlvl_sw_req dm\n"); console_getc();
	cvx16_wdqlvl_sw_req(1, 0);
	// cvx16_wdqlvl_status();
	NOTICE("cvx16_wdqlvl_req dm finish\n");
#else //DBG_SHMOO
	// cvx16_wdqlvl_req
	KC_MSG("wdqlvl_M1_ALL_DQ_DM\n");
	// sso_8x1_c(5, 15, 0, 1, &sram_sp);//mode = write, input int fmin = 5, input int fmax = 15,
					    //input int sram_st = 0, output int sram_sp

	// data_mode = 'h0 : phyd pattern
	// data_mode = 'h1 : bist read/write
	// data_mode = 'h11: with Error enject,  multi- bist write/read
	// data_mode = 'h12: with Error enject,  multi- bist write/read
	// lvl_mode  = 'h0 : wdmlvl
	// lvl_mode  = 'h1 : wdqlvl
	// lvl_mode  = 'h2 : wdqlvl and wdmlvl
	// cvx16_wdqlvl_req(data_mode, lvl_mode);
	cvx16_wdqlvl_req(1, 2);
	KC_MSG("cvx16_wdqlvl_req dq/dm finish\n");

	cvx16_wdqlvl_req(1, 1);
	KC_MSG("cvx16_wdqlvl_req dq finish\n");

	cvx16_wdqlvl_req(1, 0);
	KC_MSG("cvx16_wdqlvl_req dm finish\n");

#ifdef DO_BIST
	cvx16_bist_wr_prbs_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		KC_MSG("ERROR bist_fail\n");
	}
#endif
#endif //!DBG_SHMOO

#ifdef DBG_SHMOO
	// param_phyd_pirdlvl_dly_step [3:0]
	// param_phyd_pirdlvl_vref_step [11:8]
	mmio_wr32(0x08000088, 0x0A010212);

	//read
	NOTICE("cvx16_rdlvl_req start\n"); console_getc();
	NOTICE("SW mode 1, sram write/read continuous goto\n");
	cvx16_rdlvl_sw_req(1);
	// cvx16_rdlvl_status();
	NOTICE("cvx16_rdlvl_req finish\n");
#else //DBG_SHMOO
	// cvx16_rdlvl_req
	// mode = 'h0  : MPR mode, DDR3 only.
	// mode = 'h1  : sram write/read continuous goto
	// mode = 'h2  : multi- bist write/read
	// mode = 'h10 : with Error enject,  multi- bist write/read
	// mode = 'h12 : with Error enject,  multi- bist write/read
	rddata = mmio_rd32(0x008c + PHYD_BASE_ADDR);
	rddata = modified_bits_by_value(rddata, 1, 7, 4); // param_phyd_pirdlvl_capture_cnt
	mmio_wr32(0x008c + PHYD_BASE_ADDR, rddata);

	KC_MSG("mode multi- bist write/read\n");
	// cvx16_rdlvl_req(2); // mode multi- PRBS bist write/read
	cvx16_rdlvl_req(1); // mode multi- SRAM bist write/read
	KC_MSG("cvx16_rdlvl_req finish\n");
#ifdef DO_BIST
	cvx16_bist_wr_prbs_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		KC_MSG("ERROR bist_fail\n");
	}
#endif
#endif //!DBG_SHMOO

#ifdef DBG_SHMOO_CA
	//CA training
	NOTICE("\n===== calvl_req =====\n"); console_getc();
	// sso_8x1_c(5, 15, sram_sp, 1, &sram_sp_1);
	calvl_req(cap);
#endif //DBG_SHMOO_CA

#ifdef DBG_SHMOO_CS
	//CS training
	NOTICE("\n===== cslvl_req =====\n"); console_getc();
	// sso_8x1_c(5, 15, sram_sp, 1, &sram_sp_1);
	cslvl_req(cap);
#endif // DBG_SHMOO_CS

#ifdef DBG_SHMOO
	cvx16_dll_cal_status();
	cvx16_wrlvl_status();
	cvx16_rdglvl_status();
	cvx16_rdlvl_status();
	cvx16_wdqlvl_status();
#endif // DBG_SHMOO

	// ctrl_high_patch
	ctrl_init_high_patch();

	ctrl_init_detect_dram_size(&dram_cap_in_mbyte);
	KC_MSG("ctrl_init_detect_dram_size finish\n");

	ctrl_init_update_by_dram_size(dram_cap_in_mbyte);
	KC_MSG("ctrl_init_update_by_dram_size finish\n");

	KC_MSG("dram_cap_in_mbyte = %x\n", dram_cap_in_mbyte);
	cvx16_dram_cap_check(dram_cap_in_mbyte);
	KC_MSG("cvx16_dram_cap_check finish\n");

	// clk_gating_enable
	cvx16_clk_gating_enable();
	KC_MSG("cvx16_clk_gating_enable finish\n");

#ifdef DO_BIST
	cvx16_bist_wr_prbs_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		KC_MSG("ERROR prbs bist_fail\n");
		NOTICE("DDR BIST FAIL\n");
		while (1) {
		}
	}

	cvx16_bist_wr_sram_init();
	cvx16_bist_start_check(&bist_result, &err_data_odd, &err_data_even);
	KC_MSG(", bist_result = %x, err_data_odd = %lx, err_data_even = %lx\n", bist_result, err_data_odd,
	       err_data_even);
	if (bist_result == 0) {
		KC_MSG("ERROR sram bist_fail\n");
		NOTICE("DDR BIST FAIL\n");
		while (1) {
		}
	}
	NOTICE("DDR BIST PASS\n");
#endif

#ifdef FULL_MEM_BIST
	//full memory
	// sso_8x1_c(5, 15, 0, 1, &sram_sp);
	// sso_8x1_c(5, 15, sram_sp, 1, &sram_sp);

	NOTICE("====FULL_MEM_BIST====\n");
	bist_result = bist_all_dram(0, cap);
	if (bist_result == 0) {
		NOTICE("bist_all_dram(prbs): ERROR bist_fail\n");
	} else {
		NOTICE("bist_all_dram(prbs): BIST PASS\n");
	}

	bist_result = bist_all_dram(1, cap);
	if (bist_result == 0) {
		NOTICE("bist_all_dram(sram): ERROR bist_fail\n");
	} else {
		NOTICE("bist_all_dram(sram): BIST PASS\n");
	}

	bist_result = bist_all_dram(2, cap);
	if (bist_result == 0) {
		NOTICE("bist_all_dram(01): ERROR bist_fail\n");
	} else {
		NOTICE("bist_all_dram(01): BIST PASS\n");
	}

	NOTICE("===== BIST END ======\n");
#endif //FULL_MEM_BIST

#ifdef FULL_MEM_BIST_FOREVER
	NOTICE("Press any key to start stress test\n"); console_getc();
	bist_all_dram_forever(cap);
#endif //FULL_MEM_BIST_FOREVER

	//ERROR("AXI mon setting for latency histogram.\n");
	axi_mon_latency_setting(0x5);

	//ERROR("AXI mon 0 register dump before start.\n");
	//dump_axi_mon_reg(AXIMON_M1_WRITE);
	//ERROR("AXI mon 1 register dump before start.\n");
	//dump_axi_mon_reg(AXIMON_M1_READ);

	axi_mon_start_all();

#if defined(DBG_SHMOO) || defined(DBG_SHMOO_CA) || defined(DBG_SHMOO_CS)
	while (1)
		;
#endif

}
