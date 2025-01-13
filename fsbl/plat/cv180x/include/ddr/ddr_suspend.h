/* SPDX-License-Identifier: BSD-3-Clause */

#define SUSPEND_ENTRY __section(".suspend_entry")
#define SUSPEND_FUNC __section(".suspend_func")
#define SUSPEND_DATA __section(".suspend_data")

SUSPEND_ENTRY void ddr_suspend_entry(void);
static SUSPEND_FUNC void susp_udelay(uint32_t delay);
static SUSPEND_FUNC void ddr_sys_suspend_sus_res(void);
static SUSPEND_FUNC void cvx16_ddr_phya_pd_sus_res(void);
static SUSPEND_FUNC void cvx16_ddrc_suspend_sus_res(void);
static SUSPEND_FUNC void cvx16_ddr_phyd_save_sus_res(uint32_t);
static SUSPEND_FUNC void rtc_clr_ddr_pwrok(void);
static SUSPEND_FUNC void rtc_clr_rmio_pwrok(void);
// #define SUSPEND_USE_WDG_RST
#ifndef SUSPEND_USE_WDG_RST
static SUSPEND_FUNC void rtc_req_suspend(void);
#else
static SUSPEND_FUNC void rtc_req_wdg_rst(void);
#endif

struct reg {
	uint32_t addr;
	uint32_t val;
};
