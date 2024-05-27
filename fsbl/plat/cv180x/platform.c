#include <cpu.h>
#include <mmio.h>
#include <debug.h>
#include <assert.h>
#include <errno.h>
#include <bl_common.h>
#include <platform.h>
#include <delay_timer.h>
#include <console.h>
#include <string.h>
#include <rom_api.h>

#include "ddr_pkg_info.h"
#include "rtc.h"

void panic_handler(void)
{
	void *ra;

	ATF_ERR = ATF_ERR_PLAT_PANIC;

	ra = __builtin_return_address(0);
	mmio_write_32(ATF_ERR_INFO0, ((uint64_t)ra) & 0xFFFFFFFFUL);

	ERROR("ra=0x%lx\n", (uint64_t)ra);

	__system_reset("panic", -1);
	__builtin_unreachable();
}

void __system_reset(const char *file, unsigned int line)
{
	ATF_ERR = ATF_ERR_PLAT_SYSTEM_RESET;
	ERROR("RESET:%s:%d\n", file, line);

	console_flush();

	ATF_STATE = ATF_STATE_RESET_WAIT;
	mdelay(5000);

	// enable rtc wdt reset
	mmio_write_32(0x050260E0, 0x0001); //enable rtc_core wathdog reset enable
	mmio_write_32(0x050260C8, 0x0001); //enable rtc_core power cycle   enable

	// sw delay 100us
	ATF_STATE = ATF_STATE_RESET_RTC_WAIT;
	udelay(100);

	// mmio_write_32(0x05025018,0x00FFFFFF); //Mercury rtcsys_rstn_src_sel
	mmio_write_32(0x050250AC, 0x00000000); //cv181x rtcsys_rstn_src_sel
	mmio_write_32(0x05025004, 0x0000AB18);
	mmio_write_32(0x05025008, 0x00400040); //enable rtc_ctrl wathdog reset enable

	// printf("Enable TOP_WDT\n");
	// mmio_write_32(0x03010004,0x00000000); //config watch dog 2.6ms
	// mmio_write_32(0x03010004,0x00000022); //config watch dog 166ms
	mmio_write_32(0x03010004, 0x00000066); //config watch dog 166ms
	mmio_write_32(0x0301001c, 0x00000020);
	mmio_write_32(0x0301000c, 0x00000076);
	mmio_write_32(0x03010000, 0x00000011);

	// ROM_PWR_CYC
	if (get_sw_info()->reset_type == 1) {
		ATF_ERR = ATF_ERR_PLAT_SYSTEM_PWR_CYC;
		// printf("Issue RTCSYS_PWR_CYC\n");

		// wait pmu state to ON
		while (mmio_read_32(0x050260D4) != 0x00000003) {
			;
		}

		mmio_write_32(0x05025008, 0x00080008);
	}

	while (1)
		;

	__builtin_unreachable();
}

void reset_c906l(uintptr_t reset_address)
{
	NOTICE("RSC.\n");

	mmio_clrbits_32(0x3003024, 1 << 6);

	mmio_setbits_32(SEC_SYS_BASE + 0x04, 1 << 13);
	mmio_write_32(SEC_SYS_BASE + 0x20, reset_address);
	mmio_write_32(SEC_SYS_BASE + 0x24, reset_address >> 32);

	mmio_setbits_32(0x3003024, 1 << 6);
}

void setup_dl_flag(void)
{
	uint32_t v = p_rom_api_get_boot_src();

	switch (v) {
	case BOOT_SRC_SD:
		mmio_write_32(BOOT_SOURCE_FLAG_ADDR, MAGIC_NUM_SD_DL);
		break;
	case BOOT_SRC_USB:
		mmio_write_32(BOOT_SOURCE_FLAG_ADDR, MAGIC_NUM_USB_DL);
		break;
	default:
		mmio_write_32(BOOT_SOURCE_FLAG_ADDR, v);
		break;
	}
}

void config_core_power(uint32_t low_period)
{
	/*
	 * low_period = 0x42; // 0.90V
	 * low_period = 0x48; // 0.93V
	 * low_period = 0x4F; // 0.96V
	 * low_period = 0x58; // 1.00V
	 * low_period = 0x5C; // 1.02V
	 * low_period = 0x62; // 1.05V
	 * low_period = 0x62; // 1.05V
	 */
	mmio_write_32(PWM0_BASE + PWM_HLPERIOD0, low_period);
	mmio_write_32(PWM0_BASE + PWM_PERIOD0, 0x64);
	mmio_write_32(PINMUX_BASE + 0xA4, 0x0); // set pinmux for pwm0
	mmio_write_32(PWM0_BASE + PWM_START, 0x1); // enable bit0:pwm0
	mmio_write_32(PWM0_BASE + PWM_OE, 0x1); // output enable bit0:pwm0
	mdelay(10);
}

void sys_switch_all_to_pll(void)
{
	// Switch all clocks to PLL
	mmio_write_32(0x03002030, 0x0); // REG_CLK_BYPASS_SEL0_REG
	mmio_write_32(0x03002034, 0x0); // REG_CLK_BYPASS_SEL1_REG
}

// #ifdef OD_CLK_SEL
void sys_pll_od(void)
{
	// OD clk setting
	uint32_t value;
	uint32_t byp0_value;

	uint32_t pll_syn_set[] = {
		614400000, // set apll synthesizer  98.304 M
		610080582, // set disp synthesizer  99 M
		610080582, // set cam0 synthesizer  99 M
		//586388132, // set cam1 synthesizer  103 M
		615164587, // set cam1 synthesizer  98.18181818 M
	};

	uint32_t pll_csr[] = {
		0x00208201, // set apll *16/2 (786.432 MHz)
		0x00188101, // set disp *12/1 (1188 MHz)
		// 0x00188101, // set cam0 *12/1 (1188 MHz)
		0x00308201, // set cam0 *24/2 (1188 MHz)
		//0x00148101, // set cam1 *10/1 (1030 MHz)
		0x00168101, // set cam1 *11/1 (1080 MHz)
	};

	NOTICE("PLLS/OD.\n");

	// set vddc for OD clock
	config_core_power(0x58); //1.00V

	// store byp0 value
	byp0_value = mmio_read_32(0x03002030);

	// switch clock to xtal
	mmio_write_32(0x03002030, 0xffffffff);
	mmio_write_32(0x03002034, 0x0000003f);

	//set mipipll = 900MHz
	mmio_write_32(0x03002808, 0x05488101);

	// set synthersizer clock
	mmio_write_32(REG_PLL_G2_SSC_SYN_CTRL, 0x3F); // enable synthesizer clock enable,
		// [0]: 1: MIPIMPLL(900)/1=900MHz,
		//      0: MIPIMPLL(900)/2=450MHz

	for (uint32_t i = 0; i < 4; i++) {
		mmio_write_32(REG_APLL_SSC_SYN_SET + 0x10 * i, pll_syn_set[i]); // set pll_syn_set

		value = mmio_read_32(REG_APLL_SSC_SYN_CTRL + 0x10 * i);
		value |= 1; // [0]: sw update (w1t: write one toggle)
		value &= ~(1 << 4); // [4]: bypass = 0
		mmio_write_32(REG_APLL_SSC_SYN_CTRL + 0x10 * i, value);

		mmio_write_32(REG_APLL0_CSR + 4 * i, pll_csr[i]); // set pll_csr
	}

	value = mmio_read_32(REG_PLL_G2_CTRL);
	value = value & (~0x00011111);
	mmio_write_32(REG_PLL_G2_CTRL, value); //clear all pll PD

#ifdef __riscv
	// set mpll = 1050MHz
	mmio_write_32(0x03002908, 0x05548101);

	// set div, src_mux of clk_c906_0: [20:16]div_factor=1, [9:8]clk_src = 3 (mpll), 1050/1 = 1050MHz
	mmio_write_32(0x03002130, 0x00010309);

	// set div, src_mux of clk_c906_1: [20:16]div_factor=1, [9:8]clk_src = 1 (a0pll), 786.432/1 = 786.432MHz
	mmio_write_32(0x03002138, 0x00010109);
#else
	// set mpll = 1000MHz
	mmio_write_32(0x03002908, 0x05508101);

	// set div, src_mux of clk_a53: [20:16]div_factor=1, [9:8]clk_src = 3 (mpll)
	mmio_write_32(0x03002040, 0x00010309);
#endif

	// set tpll = 1400MHz
	mmio_write_32(0x0300290C, 0x07708101);

	mmio_write_32(0x03002048, 0x00020109); //clk_cpu_axi0 = DISPPLL(1188) / 2
	mmio_write_32(0x03002054, 0x00020009); //clk_tpu = TPLL(1400) / 2 = 700MHz
	mmio_write_32(0x03002064, 0x00080009); //clk_emmc = FPLL(1500) / 8 = 187.5MHz
	mmio_write_32(0x03002088, 0x00080009); //clk_spi_nand = FPLL(1500) / 8 = 187.5MHz
	mmio_write_32(0x03002098, 0x00200009); //clk_sdma_aud0 = APLL(786.432) / 32 = 24.576MHz
	mmio_write_32(0x03002120, 0x000F0009); //clk_pwm_src = FPLL(1500) / 15 = 100MHz
	mmio_write_32(0x030020A8, 0x00010009); //clk_uart -> clk_cam0_200 = XTAL(25) / 1 = 25MHz
	mmio_write_32(0x030020E4, 0x00030209); //clk_axi_video_codec = CAM1PLL(1080) / 3 = 360MHz
	mmio_write_32(0x030020EC, 0x00020109); //clk_vc_src0 = MIPIPLL(900) / 2 = 450MHz
	mmio_write_32(0x030020C8, 0x00030009); //clk_axi_vip = MIPIPLL(900) / 3 = 300MHz
	mmio_write_32(0x030020D0, 0x00060309); //clk_src_vip_sys_0 = FPLL(1500) / 6 = 250MHz
	mmio_write_32(0x030020D8, 0x00040209); //clk_src_vip_sys_1 = DISPPLL(1188)/ 4 = 297MHz
	mmio_write_32(0x03002110, 0x00020209); //clk_src_vip_sys_2 = DISPPLL(1188) / 2 = 594MHz
	//mmio_write_32(0x03002140, 0x00020009); //clk_src_vip_sys_3 = MIPIPLL(900) / 2 = 450MHz
	mmio_write_32(0x03002144, 0x00030309); //clk_src_vip_sys_4 = FPLL(1500) / 3 = 500MHz

	// set hsperi clock to PLL (FPLL) div by 5  = 300MHz
	mmio_write_32(0x030020B8, 0x00050009); //--> CLK_AXI4

	// set rtcsys clock to PLL (FPLL) div by 5  = 300MHz
	mmio_write_32(0x0300212C, 0x00050009); // CLK_SRC_RTC_SYS_0

	// disable powerdown, mipimpll_d3_pd[2] = 0
	mmio_clrbits_32(0x030028A0, 0x4);

	// disable powerdown, cam0pll_d2_pd[1]/cam0pll_d3_pd[2] = 0
	mmio_clrbits_32(0x030028AC, 0x6);

	//wait for pll stable
	udelay(200);

#ifdef __riscv
	// set clk_sel_23: [23] clk_sel for clk_c906_0 = 1 (DIV_IN0_SRC_MUX)
	// set clk_sel_24: [24] clk_sel for clk_c906_1 = 1 (DIV_IN0_SRC_MUX)
	mmio_write_32(0x03002020, 0x01800000);
#else
	// set clk_sel_0: [0] clk_sel for clk_a53 = 1 (DIV_IN0_SRC_MUX)
	mmio_write_32(0x03002020, 0x00000001);
#endif

	// switch clock to PLL from xtal except clk_axi4 & clk_spi_nand
	byp0_value &= (1 << 8 | //clk_spi_nand
		       1 << 19 //clk_axi4
	);
	mmio_write_32(0x03002030, byp0_value); // REG_CLK_BYPASS_SEL0_REG
	mmio_write_32(0x03002034, 0x0); // REG_CLK_BYPASS_SEL1_REG
}
// #endif

void sys_pll_nd(void)
{
	// ND clk setting
	uint32_t value;
	uint32_t byp0_value;

	uint32_t pll_syn_set[] = {
		614400000, // set apll synthesizer  98.304 M
		610080582, // set disp synthesizer  99 M
		610080582, // set cam0 synthesizer  99 M
		615164587, // set cam1 synthesizer  98.18181818 M
	};

	uint32_t pll_csr[] = {
		0x00128201, // set apll *9/2 (442.368 MHz)
		0x00188101, // set disp *12/1 (1188 MHz)
		// 0x00188101, // set cam0 *12/1 (1188 MHz)
		0x00308201, // set cam0 *24/2 (1188 MHz)
		0x00168101, // set cam1 *11/1 (1080 MHz)
	};

	NOTICE("PLLS.\n");

	// store byp0 value
	byp0_value = mmio_read_32(0x03002030);

	// switch clock to xtal
	mmio_write_32(0x03002030, 0xffffffff);
	mmio_write_32(0x03002034, 0x0000003f);

	//set mipipll = 900MHz
	mmio_write_32(0x03002808, 0x05488101);

	// set synthersizer clock
	mmio_write_32(REG_PLL_G2_SSC_SYN_CTRL, 0x3F); // enable synthesizer clock enable,
		// [0]: 1: MIPIMPLL(900)/1=900MHz,
		//      0: MIPIMPLL(900)/2=450MHz

	for (uint32_t i = 0; i < 4; i++) {
		mmio_write_32(REG_APLL_SSC_SYN_SET + 0x10 * i, pll_syn_set[i]); // set pll_syn_set

		value = mmio_read_32(REG_APLL_SSC_SYN_CTRL + 0x10 * i);
		value |= 1; // [0]: sw update (w1t: write one toggle)
		value &= ~(1 << 4); // [4]: bypass = 0
		mmio_write_32(REG_APLL_SSC_SYN_CTRL + 0x10 * i, value);

		mmio_write_32(REG_APLL0_CSR + 4 * i, pll_csr[i]); // set pll_csr
	}

	value = mmio_read_32(REG_PLL_G2_CTRL);
	value = value & (~0x00011111);
	mmio_write_32(REG_PLL_G2_CTRL, value); //clear all pll PD

#ifdef __riscv
	// set mpll = 850MHz
	mmio_write_32(0x03002908, 0x00448101);

	// set div, src_mux of clk_c906_0: [20:16]div_factor=1, [9:8]clk_src = 3 (mpll), 850/1 = 850MHz
	mmio_write_32(0x03002130, 0x00010309);

	// set div, src_mux of clk_c906_1: [20:16]div_factor=2, [9:8]clk_src = 2 (disppll), 1188/2 = 594MHz
	mmio_write_32(0x03002138, 0x00020209);
#else
	// set mpll = 800MHz
	mmio_write_32(0x03002908, 0x00408101);

	// set div, src_mux of clk_a53: [20:16]div_factor=1, [9:8]clk_src = 3 (mpll)
	mmio_write_32(0x03002040, 0x00010309);
#endif

	// set tpll = 850MHz
	mmio_write_32(0x0300290C, 0x00448101);

	mmio_write_32(0x03002048, 0x00030009); //clk_cpu_axi0 = FPLL(1500) / 3
	mmio_write_32(0x03002054, 0x00030309); //clk_tpu = FPLL(1500) / 3 = 500MHz
	mmio_write_32(0x03002064, 0x00080009); //clk_emmc = FPLL(1500) / 8 = 187.5MHz
	mmio_write_32(0x03002088, 0x00080009); //clk_spi_nand = FPLL(1500) / 8 = 187.5MHz
	mmio_write_32(0x03002098, 0x00120009); //clk_sdma_aud0 = APLL(442.368) / 18 = 24.576MHz
	mmio_write_32(0x03002120, 0x000F0009); //clk_pwm_src = FPLL(1500) / 15 = 100MHz
	mmio_write_32(0x030020A8, 0x00010009); //clk_uart -> clk_cam0_200 = XTAL(25) / 1 = 25MHz
	mmio_write_32(0x030020E4, 0x00030109); //clk_axi_video_codec = MIPIPLL(900) / 3 = 300MHz
	mmio_write_32(0x030020EC, 0x00040309); //clk_vc_src0 = FPLL(1500) / 4 = 375MHz
	mmio_write_32(0x030020C8, 0x00030009); //clk_axi_vip = MIPIPLL(900) / 3 = 300MHz
	mmio_write_32(0x030020D0, 0x00060209); //clk_src_vip_sys_0 = DISPPLL(1188) / 6 = 198MHz
	mmio_write_32(0x030020D8, 0x00060209); //clk_src_vip_sys_1 = DISPPLL(1188) / 6 = 198MHz
	mmio_write_32(0x03002110, 0x00020209); //clk_src_vip_sys_2 = DISPPLL(1188) / 2 = 594MHz
	//mmio_write_32(0x03002140, 0x00030009); //clk_src_vip_sys_3 = MIPIPLL(900) / 3 = 300MHz
	mmio_write_32(0x03002144, 0x00030209); //clk_src_vip_sys_4 = DISPPLL(1188) / 3 = 396MHz

	// set hsperi clock to PLL (FPLL) div by 5  = 300MHz
	mmio_write_32(0x030020B8, 0x00050009); //--> CLK_AXI4

	// set rtcsys clock to PLL (FPLL) div by 5  = 300MHz
	mmio_write_32(0x0300212C, 0x00050009); // CLK_SRC_RTC_SYS_0

	// disable powerdown, mipimpll_d3_pd[2] = 0
	mmio_clrbits_32(0x030028A0, 0x4);

	// disable powerdown, cam0pll_d2_pd[1]/cam0pll_d3_pd[2] = 0
	mmio_clrbits_32(0x030028AC, 0x6);

	//wait for pll stable
	udelay(200);

#ifdef __riscv
	// set clk_sel_23: [23] clk_sel for clk_c906_0 = 1 (DIV_IN0_SRC_MUX)
	// set clk_sel_24: [24] clk_sel for clk_c906_1 = 1 (DIV_IN0_SRC_MUX)
	mmio_write_32(0x03002020, 0x01800000);
#else
	// set clk_sel_0: [0] clk_sel for clk_a53 = 1 (DIV_IN0_SRC_MUX)
	mmio_write_32(0x03002020, 0x00000001);
#endif

	// switch clock to PLL from xtal except clk_axi4 & clk_spi_nand
	byp0_value &= (1 << 8 | //clk_spi_nand
		       1 << 19 //clk_axi4
	);
	mmio_write_32(0x03002030, byp0_value); // REG_CLK_BYPASS_SEL0_REG
	mmio_write_32(0x03002034, 0x0); // REG_CLK_BYPASS_SEL1_REG
}

void sys_pll_init(void)
{
	sys_pll_nd();
	NOTICE("PLLE.\n");
}

void sys_pll_init_od_sel(void)
{
	uint8_t pkg = get_pkg();

	switch (pkg) {
	case PKG_QFN88:
		NOTICE("pkg QFN88\n");
		sys_pll_od();
		break;
	case PKG_QFN68:
		NOTICE("QFN68: OD UNSUPPORTED!\n");
		sys_pll_nd();
		break;
	default:
		NOTICE("unknown pkg=%d\n", pkg);
		sys_pll_od();
	}
	NOTICE("PLLE.\n");
}

void switch_rtc_mode_1st_stage(void)
{
	// TBD
}

void switch_rtc_mode_2nd_stage(void)
{
	uint32_t read_data;
	uint32_t write_data;
	// TBD
	// mdelay(50);
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0_STATUS0);
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	// reg_rtc_mode = rtc_ctrl0[10]
	write_data = 0x0C000000 | (read_data & 0xfffffbff);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data);
	//DA_SOC_READY = 1
	mmio_write_32(RTC_MACRO_BASE + 0x8C, 0x01);
	//DA_SOC_READY = 0
	mmio_write_32(RTC_MACRO_BASE + 0x8C, 0x00);
	read_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	// reg_clk32k_cg_en = rtc_ctrl0[11] -> 1
	write_data = 0x0C000000 | (read_data & 0xffffffff) | (0x1 << 11);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data); //rtc_ctrl0
	NOTICE("Use internal 32k\n");
}

void set_rtc_en_registers(void)
{
	uint32_t write_data;
	uint32_t read_data;

	read_data = mmio_read_32(REG_RTC_BASE + RTC_ST_ON_REASON);
	NOTICE("st_on_reason=%x\n", read_data);
	read_data = mmio_read_32(REG_RTC_BASE + RTC_ST_OFF_REASON);
	NOTICE("st_off_reason=%x\n", read_data);

	mmio_write_32(REG_RTC_BASE + RTC_EN_SHDN_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_SHDN_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_WARM_RST_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_PWR_CYC_REQ) != 0x01)
		;
	mmio_write_32(REG_RTC_BASE + RTC_EN_WDT_RST_REQ, 0x01);
	while (mmio_read_32(REG_RTC_BASE + RTC_EN_WDT_RST_REQ) != 0x01)
		;
	mmio_setbits_32(REG_RTC_CTRL_BASE + RTC_POR_RST_CTRL, 0X1);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0_UNLOCKKEY, 0xAB18);
	write_data = mmio_read_32(REG_RTC_CTRL_BASE + RTC_CTRL0);
	write_data = 0xffff0000 | write_data | (0x1 << 11) | (0x01 << 6);
	mmio_write_32(REG_RTC_CTRL_BASE + RTC_CTRL0, write_data);
	mmio_clrbits_32(REG_RTC_BASE + RTC_EN_PWR_VBAT_DET, BIT(2));
}

void apply_analog_trimming_data(void)
{
}
