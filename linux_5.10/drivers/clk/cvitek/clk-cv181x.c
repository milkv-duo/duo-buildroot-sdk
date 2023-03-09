/*
 * Copyright (c) 2021 CVITEK
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/syscore_ops.h>

#include <dt-bindings/clock/cv181x-clock.h>

#define CV181X_CLK_FLAGS_ALL	(CLK_GET_RATE_NOCACHE)
// #define CV181X_CLK_FLAGS_ALL	(CLK_GET_RATE_NOCACHE | CLK_IS_CRITICAL)
// #define CV181X_CLK_FLAGS_ALL	(CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED)
#define CV181X_CLK_FLAGS_MUX	(CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT)

/* top_pll_g2 */
#define REG_PLL_G2_CTRL			0x800
#define REG_PLL_G2_STATUS		0x804
#define REG_MIPIMPLL_CSR		0x808
#define REG_APLL0_CSR			0x80C
#define REG_DISPPLL_CSR			0x810
#define REG_CAM0PLL_CSR			0x814
#define REG_CAM1PLL_CSR			0x818
#define REG_PLL_G2_SSC_SYN_CTRL		0x840
#define REG_APLL_SSC_SYN_CTRL		0x850
#define REG_APLL_SSC_SYN_SET		0x854
#define REG_APLL_SSC_SYN_SPAN		0x858
#define REG_APLL_SSC_SYN_STEP		0x85C
#define REG_DISPPLL_SSC_SYN_CTRL	0x860
#define REG_DISPPLL_SSC_SYN_SET		0x864
#define REG_DISPPLL_SSC_SYN_SPAN	0x868
#define REG_DISPPLL_SSC_SYN_STEP	0x86C
#define REG_CAM0PLL_SSC_SYN_CTRL	0x870
#define REG_CAM0PLL_SSC_SYN_SET		0x874
#define REG_CAM0PLL_SSC_SYN_SPAN	0x878
#define REG_CAM0PLL_SSC_SYN_STEP	0x87C
#define REG_CAM1PLL_SSC_SYN_CTRL	0x880
#define REG_CAM1PLL_SSC_SYN_SET		0x884
#define REG_CAM1PLL_SSC_SYN_SPAN	0x888
#define REG_CAM1PLL_SSC_SYN_STEP	0x88C
#define REG_APLL_FRAC_DIV_CTRL		0x890
#define REG_APLL_FRAC_DIV_M		0x894
#define REG_APLL_FRAC_DIV_N		0x898
#define REG_MIPIMPLL_CLK_CSR		0x8A0
#define REG_A0PLL_CLK_CSR		0x8A4
#define REG_DISPPLL_CLK_CSR		0x8A8
#define REG_CAM0PLL_CLK_CSR		0x8AC
#define REG_CAM1PLL_CLK_CSR		0x8B0
#define REG_CLK_CAM0_SRC_DIV		0x8C0
#define REG_CLK_CAM1_SRC_DIV		0x8C4

/* top_pll_g6 */
#define REG_PLL_G6_CTRL			0x900
#define REG_PLL_G6_STATUS		0x904
#define REG_MPLL_CSR			0x908
#define REG_TPLL_CSR			0x90C
#define REG_FPLL_CSR			0x910
#define REG_PLL_G6_SSC_SYN_CTRL		0x940
#define REG_DPLL_SSC_SYN_CTRL		0x950
#define REG_DPLL_SSC_SYN_SET		0x954
#define REG_DPLL_SSC_SYN_SPAN		0x958
#define REG_DPLL_SSC_SYN_STEP		0x95C
#define REG_MPLL_SSC_SYN_CTRL		0x960
#define REG_MPLL_SSC_SYN_SET		0x964
#define REG_MPLL_SSC_SYN_SPAN		0x968
#define REG_MPLL_SSC_SYN_STEP		0x96C
#define REG_TPLL_SSC_SYN_CTRL		0x970
#define REG_TPLL_SSC_SYN_SET		0x974
#define REG_TPLL_SSC_SYN_SPAN		0x978
#define REG_TPLL_SSC_SYN_STEP		0x97C

/* clkgen */
#define REG_CLK_EN_0			0x000
#define REG_CLK_EN_1			0x004
#define REG_CLK_EN_2			0x008
#define REG_CLK_EN_3			0x00C
#define REG_CLK_EN_4			0x010
#define REG_CLK_SEL_0			0x020
#define REG_CLK_BYP_0			0x030
#define REG_CLK_BYP_1			0x034

#define REG_DIV_CLK_A53_0		0x040
#define REG_DIV_CLK_A53_1		0x044
#define REG_DIV_CLK_CPU_AXI0		0x048
#define REG_DIV_CLK_CPU_GIC		0x050
#define REG_DIV_CLK_TPU			0x054
#define REG_DIV_CLK_EMMC		0x064
#define REG_DIV_CLK_100K_EMMC		0x06C
#define REG_DIV_CLK_SD0			0x070
#define REG_DIV_CLK_100K_SD0		0x078
#define REG_DIV_CLK_SD1			0x07C
#define REG_DIV_CLK_100K_SD1		0x084
#define REG_DIV_CLK_SPI_NAND		0x088
#define REG_DIV_CLK_500M_ETH0		0x08C
#define REG_DIV_CLK_500M_ETH1		0x090
#define REG_DIV_CLK_GPIO_DB		0x094
#define REG_DIV_CLK_SDMA_AUD0		0x098
#define REG_DIV_CLK_SDMA_AUD1		0x09C
#define REG_DIV_CLK_SDMA_AUD2		0x0A0
#define REG_DIV_CLK_SDMA_AUD3		0x0A4
#define REG_DIV_CLK_CAM0_200		0x0A8
#define REG_DIV_CLK_AXI4		0x0B8
#define REG_DIV_CLK_AXI6		0x0BC
#define REG_DIV_CLK_DSI_ESC		0x0C4
#define REG_DIV_CLK_AXI_VIP		0x0C8
#define REG_DIV_CLK_SRC_VIP_SYS_0	0x0D0
#define REG_DIV_CLK_SRC_VIP_SYS_1	0x0D8
#define REG_DIV_CLK_DISP_SRC_VIP	0x0E0
#define REG_DIV_CLK_AXI_VIDEO_CODEC	0x0E4
#define REG_DIV_CLK_VC_SRC0		0x0EC
#define REG_DIV_CLK_1M			0x0FC
#define REG_DIV_CLK_SPI			0x100
#define REG_DIV_CLK_I2C			0x104
#define REG_DIV_CLK_SRC_VIP_SYS_2	0x110
#define REG_DIV_CLK_AUDSRC		0x118
#define REG_DIV_CLK_PWM_SRC_0		0x120
#define REG_DIV_CLK_AP_DEBUG		0x128
#define REG_DIV_CLK_RTCSYS_SRC_0	0x12C
#define REG_DIV_CLK_C906_0_0		0x130
#define REG_DIV_CLK_C906_0_1		0x134
#define REG_DIV_CLK_C906_1_0		0x138
#define REG_DIV_CLK_C906_1_1		0x13C
#define REG_DIV_CLK_SRC_VIP_SYS_3	0x140
#define REG_DIV_CLK_SRC_VIP_SYS_4	0x144

#define REG_PLL_G2_CSR_NUM		(REG_CAM1PLL_CSR / 4 - REG_MIPIMPLL_CSR / 4 + 1)
#define REG_PLL_G2_CSR_START		REG_MIPIMPLL_CSR

#define REG_PLL_G6_CSR_NUM		(REG_FPLL_CSR / 4 - REG_MPLL_CSR / 4 + 1)
#define REG_PLL_G6_CSR_START		REG_MPLL_CSR

#define REG_CLK_EN_NUM			(REG_CLK_EN_4 / 4 - REG_CLK_EN_0 / 4 + 1)
#define REG_CLK_EN_START		REG_CLK_EN_0

#define REG_CLK_SEL_NUM			(REG_CLK_SEL_0 / 4  - REG_CLK_SEL_0 / 4 + 1)
#define REG_CLK_SEL_START		REG_CLK_SEL_0

#define REG_CLK_BYP_NUM			(REG_CLK_BYP_1 / 4 - REG_CLK_BYP_0 / 4 + 1)
#define REG_CLK_BYP_START		REG_CLK_BYP_0

#define REG_CLK_DIV_NUM			(REG_DIV_CLK_SRC_VIP_SYS_4 / 4 - REG_DIV_CLK_A53_0 / 4 + 1)
#define REG_CLK_DIV_START		REG_DIV_CLK_A53_0

#define REG_CLK_G2_DIV_NUM		(REG_CLK_CAM1_SRC_DIV / 4 - REG_CLK_CAM0_SRC_DIV / 4 + 1)
#define REG_CLK_G2_DIV_START		REG_CLK_CAM0_SRC_DIV

#define CV181X_PLL_LOCK_TIMEOUT_MS	200

/* PLL status register offset */
#define PLL_STATUS_MASK			0xFF
#define PLL_STATUS_OFFSET		0x04

/* G2 Synthesizer register offset */
#define G2_SSC_CTRL_MASK		0xFF
#define G2_SSC_CTRL_OFFSET		0x40
#define SSC_SYN_SET_MASK		0x0F
#define SSC_SYN_SET_OFFSET		0x04

#define to_cv181x_pll_clk(_hw) container_of(_hw, struct cv181x_pll_hw_clock, hw)
#define to_cv181x_clk(_hw) container_of(_hw, struct cv181x_hw_clock, hw)

#define div_mask(width) ((1 << (width)) - 1)

static DEFINE_SPINLOCK(cv181x_clk_lock);

struct cv181x_clock_data {
	void __iomem *base;
	spinlock_t *lock;
	struct clk_hw_onecell_data hw_data;
#ifdef CONFIG_PM_SLEEP
	uint32_t clken_saved_regs[REG_CLK_EN_NUM];
	uint32_t clksel_saved_regs[REG_CLK_SEL_NUM];
	uint32_t clkbyp_saved_regs[REG_CLK_BYP_NUM];
	uint32_t clkdiv_saved_regs[REG_CLK_DIV_NUM];
	uint32_t g2_clkdiv_saved_regs[REG_CLK_G2_DIV_NUM];
	uint32_t pll_g2_csr_saved_regs[REG_PLL_G2_CSR_NUM];
	uint32_t a0pll_ssc_syn_set_saved_reg;
	uint32_t disppll_ssc_syn_set_saved_reg;
	uint32_t cam0pll_ssc_syn_set_saved_reg;
	uint32_t cam1pll_ssc_syn_set_saved_reg;
	uint32_t pll_g6_csr_saved_regs[REG_PLL_G6_CSR_NUM];
#endif /* CONFIG_PM_SLEEP */
};

struct cv181x_gate {
	u32		reg;
	s8		shift;
	unsigned long	flags;
};

struct cv181x_div {
	u32		reg;
	s8		shift;
	s8		width;
	s16		initval;
	unsigned long	flags;
};

struct cv181x_mux {
	u32		reg;
	s8		shift;
	s8		width;
	unsigned long	flags;
};

struct cv181x_hw_clock {
	unsigned int id;
	const char *name;
	struct clk_hw hw;
	void __iomem *base;
	spinlock_t *lock;

	struct cv181x_gate gate;
	struct cv181x_div div[2]; /* 0: DIV_IN0, 1: DIV_IN1 */
	struct cv181x_mux mux[3]; /* 0: bypass, 1: CLK_SEL, 2: CLK_SRC(DIV_IN0_SRC_MUX) */
};

struct cv181x_pll_clock {
	unsigned int	id;
	const char	*name;
	u32		reg_csr;
	u32		reg_ssc;
	s16		post_div_sel; /* -1: postdiv*/
	unsigned long	flags;
};

struct cv181x_pll_hw_clock {
	struct cv181x_pll_clock pll;
	void __iomem *base;
	spinlock_t *lock;
	struct clk_hw hw;
};

static const struct clk_ops cv181x_g6_pll_ops;
static const struct clk_ops cv181x_g2_pll_ops;
static const struct clk_ops cv181x_g2d_pll_ops;
static const struct clk_ops cv181x_clk_ops;

static struct cv181x_clock_data *clk_data;

static unsigned long cvi_clk_flags;

#define CV181X_CLK(_id, _name, _parents, _gate_reg, _gate_shift,		\
			_div_0_reg, _div_0_shift,			\
			_div_0_width, _div_0_initval,			\
			_div_1_reg, _div_1_shift,			\
			_div_1_width, _div_1_initval,			\
			_mux_0_reg, _mux_0_shift,			\
			_mux_1_reg, _mux_1_shift,			\
			_mux_2_reg, _mux_2_shift, _flags) {		\
		.id = _id,						\
		.name = _name,						\
		.gate.reg = _gate_reg,					\
		.gate.shift = _gate_shift,				\
		.div[0].reg = _div_0_reg,				\
		.div[0].shift = _div_0_shift,				\
		.div[0].width = _div_0_width,				\
		.div[0].initval = _div_0_initval,			\
		.div[1].reg = _div_1_reg,				\
		.div[1].shift = _div_1_shift,				\
		.div[1].width = _div_1_width,				\
		.div[1].initval = _div_1_initval,			\
		.mux[0].reg = _mux_0_reg,				\
		.mux[0].shift = _mux_0_shift,				\
		.mux[0].width = 1,					\
		.mux[1].reg = _mux_1_reg,				\
		.mux[1].shift = _mux_1_shift,				\
		.mux[1].width = 1,					\
		.mux[2].reg = _mux_2_reg,				\
		.mux[2].shift = _mux_2_shift,				\
		.mux[2].width = 2,					\
		.hw.init = CLK_HW_INIT_PARENTS(				\
				_name, _parents,			\
				&cv181x_clk_ops,				\
				_flags | CV181X_CLK_FLAGS_ALL),		\
	}


#define CLK_G6_PLL(_id, _name, _parent, _reg_csr, _flags) {		\
		.pll.id = _id,						\
		.pll.name = _name,					\
		.pll.reg_csr = _reg_csr,				\
		.pll.reg_ssc = 0,					\
		.pll.post_div_sel = -1,					\
		.hw.init = CLK_HW_INIT_PARENTS(_name, _parent,		\
					       &cv181x_g6_pll_ops,	\
					       _flags |			\
					       CV181X_CLK_FLAGS_ALL),	\
	}

#define CLK_G2_PLL(_id, _name, _parent, _reg_csr, _reg_ssc, _flags) {	\
		.pll.id = _id,						\
		.pll.name = _name,					\
		.pll.reg_csr = _reg_csr,				\
		.pll.reg_ssc = _reg_ssc,				\
		.pll.post_div_sel = -1,					\
		.hw.init = CLK_HW_INIT_PARENTS(_name, _parent,		\
					       &cv181x_g2_pll_ops,	\
					       _flags |			\
					       CV181X_CLK_FLAGS_ALL),	\
	}

#define CLK_G2D_PLL(_id, _name, _parent, _reg_csr, _reg_ssc,		\
			_post_div_sel, _flags) {			\
		.pll.id = _id,						\
		.pll.name = _name,					\
		.pll.reg_csr = _reg_csr,				\
		.pll.reg_ssc = _reg_ssc,				\
		.pll.post_div_sel = _post_div_sel,			\
		.hw.init = CLK_HW_INIT_PARENTS(_name, _parent,		\
					       &cv181x_g2d_pll_ops,	\
					       _flags |			\
					       CV181X_CLK_FLAGS_ALL),	\
	}

const char *const cv181x_pll_parent[] = {"osc"};
const char *const cv181x_frac_pll_parent[] = {"clk_mipimpll"};

/*
 * All PLL clocks are marked as CRITICAL, hence they are very crucial
 * for the functioning of the SoC
 */
static struct cv181x_pll_hw_clock cv181x_pll_clks[] = {
	CLK_G6_PLL(CV181X_CLK_MPLL, "clk_mpll", cv181x_pll_parent, REG_MPLL_CSR, 0),
	CLK_G6_PLL(CV181X_CLK_TPLL, "clk_tpll", cv181x_pll_parent, REG_TPLL_CSR, 0),
	CLK_G6_PLL(CV181X_CLK_FPLL, "clk_fpll", cv181x_pll_parent, REG_FPLL_CSR, 0),
	CLK_G2_PLL(CV181X_CLK_MIPIMPLL, "clk_mipimpll", cv181x_pll_parent, REG_MIPIMPLL_CSR, 0, 0),
	CLK_G2_PLL(CV181X_CLK_A0PLL, "clk_a0pll", cv181x_frac_pll_parent, REG_APLL0_CSR, REG_APLL_SSC_SYN_CTRL, 0),
	CLK_G2_PLL(CV181X_CLK_DISPPLL, "clk_disppll", cv181x_frac_pll_parent, REG_DISPPLL_CSR,
			REG_DISPPLL_SSC_SYN_CTRL, 0),
	CLK_G2_PLL(CV181X_CLK_CAM0PLL, "clk_cam0pll", cv181x_frac_pll_parent, REG_CAM0PLL_CSR, REG_CAM0PLL_SSC_SYN_CTRL,
		CLK_IGNORE_UNUSED),
	CLK_G2_PLL(CV181X_CLK_CAM1PLL, "clk_cam1pll", cv181x_frac_pll_parent, REG_CAM1PLL_CSR,
			REG_CAM1PLL_SSC_SYN_CTRL, 0),
	CLK_G2D_PLL(CV181X_CLK_MIPIMPLL_D3, "clk_mipimpll_d3", cv181x_pll_parent, REG_MIPIMPLL_CSR,
		0, 3, CLK_IGNORE_UNUSED),
	CLK_G2D_PLL(CV181X_CLK_CAM0PLL_D2, "clk_cam0pll_d2", cv181x_frac_pll_parent, REG_CAM0PLL_CSR,
		REG_CAM0PLL_SSC_SYN_CTRL, 2, CLK_IGNORE_UNUSED),
	CLK_G2D_PLL(CV181X_CLK_CAM0PLL_D3, "clk_cam0pll_d3", cv181x_frac_pll_parent, REG_CAM0PLL_CSR,
		REG_CAM0PLL_SSC_SYN_CTRL, 3, CLK_IGNORE_UNUSED),
};

/*
 * Clocks marked as CRITICAL are needed for the proper functioning
 * of the SoC.
 */
static struct cv181x_hw_clock cv181x_clks[] = {
	CV181X_CLK(CV181X_CLK_A53, "clk_a53",
		((const char *[]) {"osc", "clk_fpll", "clk_tpll", "clk_a0pll", "clk_mipimpll", "clk_mpll"}),
		REG_CLK_EN_0, 0,
		REG_DIV_CLK_A53_0, 16, 4, 1,
		REG_DIV_CLK_A53_1, 16, 4, 2,
		REG_CLK_BYP_0, 0,
		REG_CLK_SEL_0, 0,
		REG_DIV_CLK_A53_0, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CPU_AXI0, "clk_cpu_axi0",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_0, 1,
		REG_DIV_CLK_CPU_AXI0, 16, 4, 3,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 1,
		0, -1,
		REG_DIV_CLK_CPU_AXI0, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CPU_GIC, "clk_cpu_gic",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_0, 2,
		REG_DIV_CLK_CPU_GIC, 16, 4, 5,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 2,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_XTAL_A53, "clk_xtal_a53",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 3,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TPU, "clk_tpu",
		((const char *[]) {"osc", "clk_tpll", "clk_a0pll", "clk_mipimpll", "clk_fpll"}),
		REG_CLK_EN_0, 4,
		REG_DIV_CLK_TPU, 16, 4, 3,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 3,
		0, -1,
		REG_DIV_CLK_TPU, 8,
		0),
	CV181X_CLK(CV181X_CLK_TPU_FAB, "clk_tpu_fab",
		((const char *[]) {"osc", "clk_mipimpll"}),
		REG_CLK_EN_0, 5,
		0, -1, 0, 0,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 4,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_AHB_ROM, "clk_ahb_rom",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_0, 6,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_DDR_AXI_REG, "clk_ddr_axi_reg",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_0, 7,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_RTC_25M, "clk_rtc_25m",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 8,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TEMPSEN, "clk_tempsen",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 9,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SARADC, "clk_saradc",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 10,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_EFUSE, "clk_efuse",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 11,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_APB_EFUSE, "clk_apb_efuse",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 12,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_DEBUG, "clk_debug",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 13,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_XTAL_MISC, "clk_xtal_misc",
		((const char *[]) {"osc"}),
		REG_CLK_EN_0, 14,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI4_EMMC, "clk_axi4_emmc",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_0, 15,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_EMMC, "clk_emmc",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_0, 16,
		REG_DIV_CLK_EMMC, 16, 5, 15,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 5,
		0, -1,
		REG_DIV_CLK_EMMC, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_100K_EMMC, "clk_100k_emmc",
		((const char *[]) {"clk_1m"}),
		REG_CLK_EN_0, 17,
		REG_DIV_CLK_100K_EMMC, 16, 8, 10,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI4_SD0, "clk_axi4_sd0",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_0, 18,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SD0, "clk_sd0",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_0, 19,
		REG_DIV_CLK_SD0, 16, 5, 15,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 6,
		0, -1,
		REG_DIV_CLK_SD0, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_100K_SD0, "clk_100k_sd0",
		((const char *[]) {"clk_1m"}),
		REG_CLK_EN_0, 20,
		REG_DIV_CLK_100K_SD0, 16, 8, 10,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI4_SD1, "clk_axi4_sd1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_0, 21,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SD1, "clk_sd1",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_0, 22,
		REG_DIV_CLK_SD1, 16, 5, 15,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 7,
		0, -1,
		REG_DIV_CLK_SD1, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_100K_SD1, "clk_100k_sd1",
		((const char *[]) {"clk_1m"}),
		REG_CLK_EN_0, 23,
		REG_DIV_CLK_100K_SD1, 16, 8, 10,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SPI_NAND, "clk_spi_nand",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_0, 24,
		REG_DIV_CLK_SPI_NAND, 16, 5, 8,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 8,
		0, -1,
		REG_DIV_CLK_SPI_NAND, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_500M_ETH0, "clk_500m_eth0",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_0, 25,
		REG_DIV_CLK_500M_ETH0, 16, 4, 3,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 9,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI4_ETH0, "clk_axi4_eth0",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_0, 26,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_500M_ETH1, "clk_500m_eth1",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_0, 27,
		REG_DIV_CLK_500M_ETH1, 16, 4, 3,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 10,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI4_ETH1, "clk_axi4_eth1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_0, 28,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_GPIO, "clk_apb_gpio",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_0, 29,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_GPIO_INTR, "clk_apb_gpio_intr",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_0, 30,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_GPIO_DB, "clk_gpio_db",
		((const char *[]) {"clk_1m"}),
		REG_CLK_EN_0, 31,
		REG_DIV_CLK_GPIO_DB, 16, 16, 10,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AHB_SF, "clk_ahb_sf",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 0,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SDMA_AXI, "clk_sdma_axi",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 1,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SDMA_AUD0, "clk_sdma_aud0",
		((const char *[]) {"osc", "clk_a0pll", "a24k_clk"}),
		REG_CLK_EN_1, 2,
		REG_DIV_CLK_SDMA_AUD0, 16, 8, 18,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 11,
		0, -1,
		REG_DIV_CLK_SDMA_AUD0, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SDMA_AUD1, "clk_sdma_aud1",
		((const char *[]) {"osc", "clk_a0pll", "a24k_clk"}),
		REG_CLK_EN_1, 3,
		REG_DIV_CLK_SDMA_AUD1, 16, 8, 18,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 12,
		0, -1,
		REG_DIV_CLK_SDMA_AUD1, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SDMA_AUD2, "clk_sdma_aud2",
		((const char *[]) {"osc", "clk_a0pll", "a24k_clk"}),
		REG_CLK_EN_1, 4,
		REG_DIV_CLK_SDMA_AUD2, 16, 8, 18,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 13,
		0, -1,
		REG_DIV_CLK_SDMA_AUD2, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SDMA_AUD3, "clk_sdma_aud3",
		((const char *[]) {"osc", "clk_a0pll", "a24k_clk"}),
		REG_CLK_EN_1, 5,
		REG_DIV_CLK_SDMA_AUD3, 16, 8, 18,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 14,
		0, -1,
		REG_DIV_CLK_SDMA_AUD3, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2C, "clk_apb_i2c",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 6,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_WDT, "clk_apb_wdt",
		((const char *[]) {"osc"}),
		REG_CLK_EN_1, 7,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_PWM, "clk_pwm",
		((const char *[]) {"clk_pwm_src"}),
		REG_CLK_EN_1, 8,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_SPI0, "clk_apb_spi0",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 9,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_SPI1, "clk_apb_spi1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 10,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_SPI2, "clk_apb_spi2",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 11,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_SPI3, "clk_apb_spi3",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 12,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CAM0_200, "clk_cam0_200",
		((const char *[]) {"osc", "osc", "clk_disppll"}),
		REG_CLK_EN_1, 13,
		REG_DIV_CLK_CAM0_200, 16, 4, 1,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 16,
		0, -1,
		REG_DIV_CLK_CAM0_200, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_UART0, "clk_uart0",
		((const char *[]) {"clk_cam0_200"}),
		REG_CLK_EN_1, 14,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_UART0, "clk_apb_uart0",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 15,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_UART1, "clk_uart1",
		((const char *[]) {"clk_cam0_200"}),
		REG_CLK_EN_1, 16,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_UART1, "clk_apb_uart1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 17,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_UART2, "clk_uart2",
		((const char *[]) {"clk_cam0_200"}),
		REG_CLK_EN_1, 18,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_UART2, "clk_apb_uart2",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 19,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_UART3, "clk_uart3",
		((const char *[]) {"clk_cam0_200"}),
		REG_CLK_EN_1, 20,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_UART3, "clk_apb_uart3",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 21,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_UART4, "clk_uart4",
		((const char *[]) {"clk_cam0_200"}),
		REG_CLK_EN_1, 22,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_UART4, "clk_apb_uart4",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 23,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2S0, "clk_apb_i2s0",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 24,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2S1, "clk_apb_i2s1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 25,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2S2, "clk_apb_i2s2",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 26,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2S3, "clk_apb_i2s3",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 27,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI4_USB, "clk_axi4_usb",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 28,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_APB_USB, "clk_apb_usb",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_1, 29,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_125M_USB, "clk_125m_usb",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_1, 30,
		0, -1, 0, 0,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 17,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_33K_USB, "clk_33k_usb",
		((const char *[]) {"clk_1m"}),
		REG_CLK_EN_1, 31,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_12M_USB, "clk_12m_usb",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_2, 0,
		0, -1, 0, 0,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 18,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_AXI4, "clk_axi4",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_2, 1,
		REG_DIV_CLK_AXI4, 16, 4, 5,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 19,
		0, -1,
		REG_DIV_CLK_AXI4, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI6, "clk_axi6",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_2, 2,
		REG_DIV_CLK_AXI6, 16, 4, 15,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 20,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_DSI_ESC, "clk_dsi_esc",
		((const char *[]) {"osc", "clk_axi6"}),
		REG_CLK_EN_2, 3,
		REG_DIV_CLK_DSI_ESC, 16, 4, 5,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 21,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_AXI_VIP, "clk_axi_vip",
		((const char *[]) {"osc", "clk_mipimpll", "clk_cam0pll", "clk_disppll", "clk_fpll"}),
		REG_CLK_EN_2, 4,
		REG_DIV_CLK_AXI_VIP, 16, 4, 3,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 22,
		0, -1,
		REG_DIV_CLK_AXI_VIP, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_SRC_VIP_SYS_0, "clk_src_vip_sys_0",
		((const char *[]) {"osc", "clk_mipimpll", "clk_cam0pll", "clk_disppll", "clk_fpll"}),
		REG_CLK_EN_2, 5,
		REG_DIV_CLK_SRC_VIP_SYS_0, 16, 4, 6,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 23,
		0, -1,
		REG_DIV_CLK_SRC_VIP_SYS_0, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_SRC_VIP_SYS_1, "clk_src_vip_sys_1",
		((const char *[]) {"osc", "clk_mipimpll", "clk_cam0pll", "clk_disppll", "clk_fpll"}),
		REG_CLK_EN_2, 6,
		REG_DIV_CLK_SRC_VIP_SYS_1, 16, 4, 5,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 24,
		0, -1,
		REG_DIV_CLK_SRC_VIP_SYS_1, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_DISP_SRC_VIP, "clk_disp_src_vip",
		((const char *[]) {"osc", "clk_disppll"}),
		REG_CLK_EN_2, 7,
		REG_DIV_CLK_DISP_SRC_VIP, 16, 4, 8,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 25,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_AXI_VIDEO_CODEC, "clk_axi_video_codec",
		((const char *[]) {"osc", "clk_a0pll", "clk_mipimpll", "clk_cam1pll", "clk_fpll"}),
		REG_CLK_EN_2, 8,
		REG_DIV_CLK_AXI_VIDEO_CODEC, 16, 4, 2,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 26,
		0, -1,
		REG_DIV_CLK_AXI_VIDEO_CODEC, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_VC_SRC0, "clk_vc_src0",
		((const char *[]) {"osc", "clk_disppll", "clk_mipimpll", "clk_cam1pll", "clk_fpll"}),
		REG_CLK_EN_2, 9,
		REG_DIV_CLK_VC_SRC0, 16, 4, 2,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 27,
		0, -1,
		REG_DIV_CLK_VC_SRC0, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_H264C, "clk_h264c",
		((const char *[]) {"clk_axi_video_codec"}),
		REG_CLK_EN_2, 10,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_H265C, "clk_h265c",
		((const char *[]) {"clk_axi_video_codec"}),
		REG_CLK_EN_2, 11,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_JPEG, "clk_jpeg",
		((const char *[]) {"clk_axi_video_codec"}),
		REG_CLK_EN_2, 12,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_APB_JPEG, "clk_apb_jpeg",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_2, 13,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_APB_H264C, "clk_apb_h264c",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_2, 14,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_APB_H265C, "clk_apb_h265c",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_2, 15,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_CAM0, "clk_cam0",
		((const char *[]) {"clk_cam0pll", "clk_cam0pll_d2", "clk_cam0pll_d3", "clk_mipimpll_d3"}),
		REG_CLK_EN_2, 16,
		REG_CLK_CAM0_SRC_DIV, 16, 6, -1,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		REG_CLK_CAM0_SRC_DIV, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_CAM1, "clk_cam1",
		((const char *[]) {"clk_cam0pll", "clk_cam0pll_d2", "clk_cam0pll_d3", "clk_mipimpll_d3"}),
		REG_CLK_EN_2, 17,
		REG_CLK_CAM1_SRC_DIV, 16, 6, -1,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		REG_CLK_CAM1_SRC_DIV, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_CSI_MAC0_VIP, "clk_csi_mac0_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 18,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_CSI_MAC1_VIP, "clk_csi_mac1_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 19,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_ISP_TOP_VIP, "clk_isp_top_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 20,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_IMG_D_VIP, "clk_img_d_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 21,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_IMG_V_VIP, "clk_img_v_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 22,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SC_TOP_VIP, "clk_sc_top_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 23,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_SC_D_VIP, "clk_sc_d_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 24,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SC_V1_VIP, "clk_sc_v1_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 25,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SC_V2_VIP, "clk_sc_v2_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 26,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SC_V3_VIP, "clk_sc_v3_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 27,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_DWA_VIP, "clk_dwa_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 28,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_BT_VIP, "clk_bt_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 29,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_DISP_VIP, "clk_disp_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 30,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_DSI_MAC_VIP, "clk_dsi_mac_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_2, 31,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_LVDS0_VIP, "clk_lvds0_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_3, 0,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_LVDS1_VIP, "clk_lvds1_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_3, 1,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CSI0_RX_VIP, "clk_csi0_rx_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_3, 2,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CSI1_RX_VIP, "clk_csi1_rx_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_3, 3,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_PAD_VI_VIP, "clk_pad_vi_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_3, 4,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_1M, "clk_1m",
		((const char *[]) {"osc"}),
		REG_CLK_EN_3, 5,
		REG_DIV_CLK_1M, 16, 6, 25,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SPI, "clk_spi",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_3, 6,
		REG_DIV_CLK_SPI, 16, 6, 8,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 30,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_I2C, "clk_i2c",
		((const char *[]) {"osc", "clk_axi6"}),
		REG_CLK_EN_3, 7,
		REG_DIV_CLK_I2C, 16, 4, 1,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 31,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_PM, "clk_pm",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_3, 8,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER0, "clk_timer0",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 9,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER1, "clk_timer1",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 10,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER2, "clk_timer2",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 11,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER3, "clk_timer3",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 12,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER4, "clk_timer4",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 13,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER5, "clk_timer5",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 14,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER6, "clk_timer6",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 15,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_TIMER7, "clk_timer7",
		((const char *[]) {"clk_xtal_misc"}),
		REG_CLK_EN_3, 16,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2C0, "clk_apb_i2c0",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_3, 17,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2C1, "clk_apb_i2c1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_3, 18,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2C2, "clk_apb_i2c2",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_3, 19,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2C3, "clk_apb_i2c3",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_3, 20,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_I2C4, "clk_apb_i2c4",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_3, 21,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_WGN, "clk_wgn",
		((const char *[]) {"osc"}),
		REG_CLK_EN_3, 22,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_WGN0, "clk_wgn0",
		((const char *[]) {"osc"}),
		REG_CLK_EN_3, 23,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_WGN1, "clk_wgn1",
		((const char *[]) {"osc"}),
		REG_CLK_EN_3, 24,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_WGN2, "clk_wgn2",
		((const char *[]) {"osc"}),
		REG_CLK_EN_3, 25,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_KEYSCAN, "clk_keyscan",
		((const char *[]) {"osc"}),
		REG_CLK_EN_3, 26,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_AHB_SF1, "clk_ahb_sf1",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_3, 27,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_VC_SRC1, "clk_vc_src1",
		((const char *[]) {"osc", "clk_cam1pll"}),
		REG_CLK_EN_3, 28,
		0, -1, 0, 0,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 0,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_SRC_VIP_SYS_2, "clk_src_vip_sys_2",
		((const char *[]) {"osc", "clk_mipimpll", "clk_cam0pll", "clk_disppll", "clk_fpll"}),
		REG_CLK_EN_3, 29,
		REG_DIV_CLK_SRC_VIP_SYS_2, 16, 4, 2,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 1,
		0, -1,
		REG_DIV_CLK_SRC_VIP_SYS_2, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_PAD_VI1_VIP, "clk_pad_vi1_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_3, 30,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CFG_REG_VIP, "clk_cfg_reg_vip",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_3, 31,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CFG_REG_VC, "clk_cfg_reg_vc",
		((const char *[]) {"clk_axi6"}),
		REG_CLK_EN_4, 0,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_AUDSRC, "clk_audsrc",
		((const char *[]) {"osc", "clk_a0pll", "a24k_clk"}),
		REG_CLK_EN_4, 1,
		REG_DIV_CLK_AUDSRC, 16, 8, 18,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 2,
		0, -1,
		REG_DIV_CLK_AUDSRC, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_APB_AUDSRC, "clk_apb_audsrc",
		((const char *[]) {"clk_axi4"}),
		REG_CLK_EN_4, 2,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_VC_SRC2, "clk_vc_src2",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_4, 3,
		0, -1, 0, 0,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 3,
		0, -1,
		0, -1,
		0),
	CV181X_CLK(CV181X_CLK_PWM_SRC, "clk_pwm_src",
		((const char *[]) {"osc", "clk_fpll", "clk_disppll"}),
		REG_CLK_EN_4, 4,
		REG_DIV_CLK_PWM_SRC_0, 16, 6, 10,
		0, -1, 0, 0,
		REG_CLK_BYP_0, 15,
		0, -1,
		REG_DIV_CLK_PWM_SRC_0, 8,
		0),
	CV181X_CLK(CV181X_CLK_AP_DEBUG, "clk_ap_debug",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_4, 5,
		REG_DIV_CLK_AP_DEBUG, 16, 4, 5,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 4,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SRC_RTC_SYS_0, "clk_src_rtc_sys_0",
		((const char *[]) {"osc", "clk_fpll"}),
		REG_CLK_EN_4, 6,
		REG_DIV_CLK_RTCSYS_SRC_0, 16, 4, 5,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 5,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_PAD_VI2_VIP, "clk_pad_vi2_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 7,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CSI_BE_VIP, "clk_csi_be_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 8,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_VIP_IP0, "clk_vip_ip0",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 9,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_VIP_IP1, "clk_vip_ip1",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 10,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_VIP_IP2, "clk_vip_ip2",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 11,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_VIP_IP3, "clk_vip_ip3",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 12,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_C906_0, "clk_c906_0",
		((const char *[]) {"osc", "clk_fpll", "clk_tpll", "clk_a0pll", "clk_mipimpll", "clk_mpll"}),
		REG_CLK_EN_4, 13,
		REG_DIV_CLK_C906_0_0, 16, 4, 1,
		REG_DIV_CLK_C906_0_1, 16, 4, 2,
		REG_CLK_BYP_1, 6,
		REG_CLK_SEL_0, 23,
		REG_DIV_CLK_C906_0_0, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_C906_1, "clk_c906_1",
		((const char *[]) {"osc", "clk_fpll", "clk_tpll", "clk_a0pll", "clk_disppll", "clk_mpll"}),
		REG_CLK_EN_4, 14,
		REG_DIV_CLK_C906_1_0, 16, 4, 2,
		REG_DIV_CLK_C906_1_1, 16, 4, 2,
		REG_CLK_BYP_1, 7,
		REG_CLK_SEL_0, 24,
		REG_DIV_CLK_C906_1_0, 8,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_SRC_VIP_SYS_3, "clk_src_vip_sys_3",
		((const char *[]) {"osc", "clk_mipimpll", "clk_cam0pll", "clk_disppll", "clk_fpll"}),
		REG_CLK_EN_4, 15,
		REG_DIV_CLK_SRC_VIP_SYS_3, 16, 4, 2,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 8,
		0, -1,
		REG_DIV_CLK_SRC_VIP_SYS_3, 8,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_SRC_VIP_SYS_4, "clk_src_vip_sys_4",
		((const char *[]) {"osc", "clk_mipimpll", "clk_cam0pll", "clk_disppll", "clk_fpll"}),
		REG_CLK_EN_4, 16,
		REG_DIV_CLK_SRC_VIP_SYS_4, 16, 4, 3,
		0, -1, 0, 0,
		REG_CLK_BYP_1, 9,
		0, -1,
		REG_DIV_CLK_SRC_VIP_SYS_4, 8,
		0),
	CV181X_CLK(CV181X_CLK_IVE_VIP, "clk_ive_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 17,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_RAW_VIP, "clk_raw_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 18,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_OSDC_VIP, "clk_osdc_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 19,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
	CV181X_CLK(CV181X_CLK_CSI_MAC2_VIP, "clk_csi_mac2_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 20,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IGNORE_UNUSED),
	CV181X_CLK(CV181X_CLK_CAM0_VIP, "clk_cam0_vip",
		((const char *[]) {"clk_axi_vip"}),
		REG_CLK_EN_4, 21,
		0, -1, 0, 0,
		0, -1, 0, 0,
		0, -1,
		0, -1,
		0, -1,
		CLK_IS_CRITICAL),
};

static int __init cvi_clk_flags_setup(char *arg)
{
	int ret;
	unsigned long flags;

	ret = kstrtol(arg, 0, &flags);
	if (ret)
		return ret;

	cvi_clk_flags = flags;
	pr_info("cvi_clk_flags = 0x%lX\n", cvi_clk_flags);

	return 1;
}
__setup("cvi_clk_flags=", cvi_clk_flags_setup);

static unsigned long cv181x_pll_rate_calc(u32 regval, s16 post_div_sel, unsigned long parent_rate)
{
	u64 numerator;
	u32 predivsel, postdivsel, divsel;
	u32 denominator;

	predivsel = regval & 0x7f;
	postdivsel = post_div_sel < 0 ? (regval >> 8) & 0x7f : (u32)post_div_sel;
	divsel = (regval >> 17) & 0x7f;

	numerator = parent_rate * divsel;
	denominator = predivsel * postdivsel;
	do_div(numerator, denominator);

	return (unsigned long)numerator;
}

static unsigned long cv181x_g6_pll_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	struct cv181x_pll_hw_clock *pll_hw = to_cv181x_pll_clk(hw);
	unsigned long rate;
	u32 regval;

	regval = readl(pll_hw->base + pll_hw->pll.reg_csr);
	rate = cv181x_pll_rate_calc(regval, pll_hw->pll.post_div_sel, parent_rate);

	return rate;
}

static long cv181x_g6_pll_round_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long *prate)
{
	return rate;
}

static int cv181x_g6_pll_calc_csr(unsigned long parent_rate, unsigned long rate, u32 *csr)
{
	u64 numerator;
	u32 denominator;
	u32 divsel;		/* [23:17] DIV_SEL */
	u32 postdivsel = 1;	/* [14:8] POST_DIV_SEL */
	u32 ictrl = 7;		/* [26:24] ICTRL */
	u32 selmode = 1;	/* [16:15] SEL_MODE */
	u32 predivsel = 1;	/* [6:0] PRE_DIV_SEL */
	u32 vco_clks[] = {900, 1000, 1100, 1200, 1300, 1400, 1500, 1600};
	int i;

	for (i = 0; i < ARRAY_SIZE(vco_clks); i++) {
		if ((vco_clks[i] * 1000000) % rate == 0) {
			postdivsel = vco_clks[i] * 1000000 / rate;
			rate = vco_clks[i] * 1000000;
			pr_debug("rate=%ld, postdivsel=%d\n", rate, postdivsel);
			break;
		}
	}

	numerator = rate;
	denominator = parent_rate;

	do_div(numerator, denominator);

	divsel = (u32)numerator & 0x7f;
	*csr = (divsel << 17) | (postdivsel << 8) | (ictrl << 24) | (selmode << 15) | predivsel;

	pr_debug("csr=0x%08x\n", *csr);

	return 0;
}

static int cv181x_g6_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	struct cv181x_pll_hw_clock *pll_hw = to_cv181x_pll_clk(hw);
	unsigned long flags = 0;
	int ret;
	u32 reg_g6_pll_status;
	u32 regval_csr;
	u32 regval_g6_pll_status;
	u32 g6_pll_update_status = 0;
	ktime_t timeout;

	reg_g6_pll_status = (pll_hw->pll.reg_csr & ~PLL_STATUS_MASK) + PLL_STATUS_OFFSET;

	if (pll_hw->lock)
		spin_lock_irqsave(pll_hw->lock, flags);
	else
		__acquire(pll_hw->lock);

	/* calculate csr register */
	ret = cv181x_g6_pll_calc_csr(parent_rate, rate, &regval_csr);
	if (ret < 0)
		return ret;

	/* csr register */
	writel(regval_csr, pll_hw->base + pll_hw->pll.reg_csr);

	if (pll_hw->pll.reg_csr == REG_MPLL_CSR)
		g6_pll_update_status = BIT(0);
	else if (pll_hw->pll.reg_csr == REG_TPLL_CSR)
		g6_pll_update_status = BIT(1);
	else if (pll_hw->pll.reg_csr == REG_FPLL_CSR)
		g6_pll_update_status = BIT(2);

	/* wait for pll setting updated */
	timeout = ktime_add_ms(ktime_get(), CV181X_PLL_LOCK_TIMEOUT_MS);
	while (1) {
		regval_g6_pll_status = readl(pll_hw->base + reg_g6_pll_status);
		if ((regval_g6_pll_status & g6_pll_update_status) == 0)
			break;

		if (ktime_after(ktime_get(), timeout)) {
			pr_err("timeout waiting for pll update, g6_pll_status = 0x%08x\n",
			       regval_g6_pll_status);
			break;
		}
		cpu_relax();
	}

	if (pll_hw->lock)
		spin_unlock_irqrestore(pll_hw->lock, flags);
	else
		__release(pll_hw->lock);

	return 0;
}

static const struct clk_ops cv181x_g6_pll_ops = {
	.recalc_rate = cv181x_g6_pll_recalc_rate,
	.round_rate = cv181x_g6_pll_round_rate,
	.set_rate = cv181x_g6_pll_set_rate,
};

static unsigned long cv181x_g2_pll_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	struct cv181x_pll_hw_clock *pll_hw = to_cv181x_pll_clk(hw);
	u32 reg_ssc_set;
	u32 reg_g2_ssc_ctrl;
	u32 regval_csr;
	u32 regval_ssc_set;
	u32 regval_g2_ssc_ctrl;
	u64 numerator;
	u32 denominator;
	unsigned long clk_ref;
	unsigned long rate;

	regval_csr = readl(pll_hw->base + pll_hw->pll.reg_csr);

	/* pll without synthesizer */
	if (pll_hw->pll.reg_ssc == 0) {
		clk_ref = parent_rate;
		goto rate_calc;
	}

	/* calculate synthesizer freq */
	reg_ssc_set = (pll_hw->pll.reg_ssc & ~SSC_SYN_SET_MASK) + SSC_SYN_SET_OFFSET;
	reg_g2_ssc_ctrl = (pll_hw->pll.reg_ssc & ~G2_SSC_CTRL_MASK) + G2_SSC_CTRL_OFFSET;

	regval_ssc_set = readl(pll_hw->base + reg_ssc_set);
	regval_g2_ssc_ctrl = readl(pll_hw->base + reg_g2_ssc_ctrl);

	/* bit0 sel_syn_clk */
	numerator = (regval_g2_ssc_ctrl & 0x1) ? parent_rate : (parent_rate >> 1);

	numerator <<= 26;
	denominator = regval_ssc_set;
	if (denominator)
		do_div(numerator, denominator);
	else
		pr_err("pll ssc_set is zero\n");

	clk_ref = numerator;

rate_calc:
	rate = cv181x_pll_rate_calc(regval_csr, pll_hw->pll.post_div_sel, clk_ref);

	return rate;
}

static const struct {
	unsigned long rate;
	u32 csr;
	u32 ssc_set;
} g2_pll_rate_lut[] = {
	// {.rate = 48000000, .csr = 0x00129201, .ssc_set = 629145600},
	// {.rate = 406425600, .csr = 0x010E9201, .ssc_set = 594430839},
	// {.rate = 417792000, .csr = 0x01109201, .ssc_set = 642509804},
	// {.rate = 768000000, .csr = 0x00108101, .ssc_set = 419430400},
	// {.rate = 832000000, .csr = 0x00108101, .ssc_set = 387166523},
	// {.rate = 1032000000, .csr = 0x00148101, .ssc_set = 390167814},
	// {.rate = 1050000000, .csr = 0x00168101, .ssc_set = 421827145},
	// {.rate = 1056000000, .csr = 0x00208100, .ssc_set = 412977625},
	// {.rate = 1125000000, .csr = 0x00168101, .ssc_set = 393705325},
	// {.rate = 1188000000, .csr = 0x00188101, .ssc_set = 610080582}, //postdiv=1
	{.rate = 1188000000, .csr = 0x00308201, .ssc_set = 610080582}, //postdiv=2
};

static int cv181x_g2_pll_get_setting_from_lut(unsigned long rate, u32 *csr,
					      u32 *ssc_set)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(g2_pll_rate_lut); i++) {
		if (rate == g2_pll_rate_lut[i].rate) {
			*csr = g2_pll_rate_lut[i].csr;
			*ssc_set = g2_pll_rate_lut[i].ssc_set;
			return 0;
		}
	}

	*csr = 0;
	*ssc_set = 0;
	return -ENOENT;
}

static long cv181x_g2_pll_round_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long *prate)
{
	return rate;
}

static int cv181x_g2_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	struct cv181x_pll_hw_clock *pll_hw = to_cv181x_pll_clk(hw);
	unsigned long flags = 0;
	int ret;
	u32 reg_ssc_set;
	u32 reg_ssc_ctrl;
	u32 reg_g2_pll_status;
	u32 regval_csr;
	u32 regval_ssc_set;
	u32 regval_ssc_ctrl;
	u32 regval_g2_pll_status;
	u32 g2_pll_update_status = 0;
	ktime_t timeout;

	/* pll without synthesizer */
	if (pll_hw->pll.reg_ssc == 0)
		return -ENOENT;

	ret = cv181x_g2_pll_get_setting_from_lut(rate, &regval_csr,
						 &regval_ssc_set);
	if (ret < 0)
		return ret;

	reg_ssc_set = (pll_hw->pll.reg_ssc & ~SSC_SYN_SET_MASK) + SSC_SYN_SET_OFFSET;
	reg_ssc_ctrl = pll_hw->pll.reg_ssc;
	reg_g2_pll_status = (pll_hw->pll.reg_csr & ~PLL_STATUS_MASK) + PLL_STATUS_OFFSET;

	if (pll_hw->lock)
		spin_lock_irqsave(pll_hw->lock, flags);
	else
		__acquire(pll_hw->lock);

	/* set synthersizer */
	writel(regval_ssc_set, pll_hw->base + reg_ssc_set);

	/* bit 0 toggle */
	regval_ssc_ctrl = readl(pll_hw->base + reg_ssc_ctrl);
	regval_ssc_ctrl ^= 0x00000001;
	writel(regval_ssc_ctrl, pll_hw->base + reg_ssc_ctrl);

	/* csr register */
	writel(regval_csr, pll_hw->base + pll_hw->pll.reg_csr);

	if (pll_hw->pll.reg_csr == REG_MIPIMPLL_CSR)
		g2_pll_update_status = BIT(0);
	else if (pll_hw->pll.reg_csr == REG_APLL0_CSR)
		g2_pll_update_status = BIT(1);
	else if (pll_hw->pll.reg_csr == REG_DISPPLL_CSR)
		g2_pll_update_status = BIT(2);
	else if (pll_hw->pll.reg_csr == REG_CAM0PLL_CSR)
		g2_pll_update_status = BIT(3);
	else if (pll_hw->pll.reg_csr == REG_CAM1PLL_CSR)
		g2_pll_update_status = BIT(4);

	/* wait for pll setting updated */
	timeout = ktime_add_ms(ktime_get(), CV181X_PLL_LOCK_TIMEOUT_MS);
	while (1) {
		regval_g2_pll_status = readl(pll_hw->base + reg_g2_pll_status);
		if ((regval_g2_pll_status & g2_pll_update_status) == 0)
			break;

		if (ktime_after(ktime_get(), timeout)) {
			pr_err("timeout waiting for pll update, g2_pll_status = 0x%08x\n",
			       regval_g2_pll_status);
			break;
		}
		cpu_relax();
	}

	if (pll_hw->lock)
		spin_unlock_irqrestore(pll_hw->lock, flags);
	else
		__release(pll_hw->lock);

	return 0;
}

static const struct clk_ops cv181x_g2_pll_ops = {
	.recalc_rate = cv181x_g2_pll_recalc_rate,
	.round_rate = cv181x_g2_pll_round_rate,
	.set_rate = cv181x_g2_pll_set_rate,
};

static const struct clk_ops cv181x_g2d_pll_ops = {
	.recalc_rate = cv181x_g2_pll_recalc_rate,
};

static struct clk_hw *cv181x_clk_register_pll(struct cv181x_pll_hw_clock *pll_clk,
					    void __iomem *sys_base)
{
	struct clk_hw *hw;
	struct clk_init_data init;
	int err;

	pll_clk->lock = &cv181x_clk_lock;
	pll_clk->base = sys_base;

	if (cvi_clk_flags) {
		/* copy clk_init_data for modification */
		memcpy(&init, pll_clk->hw.init, sizeof(init));

		init.flags |= cvi_clk_flags;
		pll_clk->hw.init = &init;
	}

	hw = &pll_clk->hw;

	err = clk_hw_register(NULL, hw);
	if (err)
		return ERR_PTR(err);

	return hw;
}

static void cv181x_clk_unregister_pll(struct clk_hw *hw)
{
	struct cv181x_pll_hw_clock *pll_hw = to_cv181x_pll_clk(hw);

	clk_hw_unregister(hw);
	kfree(pll_hw);
}

static int cv181x_clk_register_plls(struct cv181x_pll_hw_clock *clks,
				    int num_clks,
				    struct cv181x_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *pll_base = data->base;
	int i;

	for (i = 0; i < num_clks; i++) {
		struct cv181x_pll_hw_clock *cv181x_clk = &clks[i];

		hw = cv181x_clk_register_pll(cv181x_clk, pll_base);
		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, cv181x_clk->pll.name);
			goto err_clk;
		}

		data->hw_data.hws[clks[i].pll.id] = hw;

		clk_hw_register_clkdev(hw, cv181x_clk->pll.name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		cv181x_clk_unregister_pll(data->hw_data.hws[clks[i].pll.id]);

	return PTR_ERR(hw);
}

static int cv181x_clk_is_bypassed(struct cv181x_hw_clock *clk_hw)
{
	u32 val;
	void __iomem *reg_addr = clk_hw->base + clk_hw->mux[0].reg;

	if (clk_hw->mux[0].shift >= 0) {
		val = readl(reg_addr) >> clk_hw->mux[0].shift;
		val &= 0x1; //width
	} else {
		val = 0;
	}

	return val;
}

static int cv181x_clk_get_clk_sel(struct cv181x_hw_clock *clk_hw)
{
	u32 val;
	void __iomem *reg_addr = clk_hw->base + clk_hw->mux[1].reg;

	if (clk_hw->mux[1].shift >= 0) {
		val = readl(reg_addr) >> clk_hw->mux[1].shift;
		val &= 0x1; //width
		val ^= 0x1; //invert value
	} else {
		val = 0;
	}

	return val;
}

static int cv181x_clk_get_src_sel(struct cv181x_hw_clock *clk_hw)
{
	u32 val;
	void __iomem *reg_addr = clk_hw->base + clk_hw->mux[2].reg;

	if (clk_hw->mux[2].shift >= 0) {
		val = readl(reg_addr) >> clk_hw->mux[2].shift;
		val &= 0x3; //width
	} else {
		val = 0;
	}

	return val;
}

static unsigned long cv181x_clk_div_recalc_rate(struct clk_hw *hw,
					      unsigned long parent_rate)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	unsigned int clk_sel = cv181x_clk_get_clk_sel(clk_hw);
	void __iomem *reg_addr = clk_hw->base + clk_hw->div[clk_sel].reg;
	unsigned int val;
	unsigned long rate;

	if ((clk_hw->mux[0].shift >= 0) && cv181x_clk_is_bypassed(clk_hw))
		return parent_rate;

	if ((clk_hw->div[clk_sel].initval > 0) && !(readl(reg_addr) & BIT(3))) {
		val = clk_hw->div[clk_sel].initval;
	} else {
		val = readl(reg_addr) >> clk_hw->div[clk_sel].shift;
		val &= div_mask(clk_hw->div[clk_sel].width);
	}
	rate = divider_recalc_rate(hw, parent_rate, val, NULL,
				   clk_hw->div[clk_sel].flags,
				   clk_hw->div[clk_sel].width);

	return rate;
}

static long cv181x_clk_div_round_rate(struct clk_hw *hw, unsigned long rate,
				      unsigned long *prate)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	unsigned int clk_sel = cv181x_clk_get_clk_sel(clk_hw);

	if ((clk_hw->mux[0].shift >= 0) && cv181x_clk_is_bypassed(clk_hw))
		return DIV_ROUND_UP_ULL((u64)*prate, 1);

	return divider_round_rate(hw, rate, prate, NULL,
				  clk_hw->div[clk_sel].width, clk_hw->div[clk_sel].flags);
}

static long cv181x_clk_div_calc_round_rate(struct clk_hw *hw, unsigned long rate,
					 unsigned long *prate)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);

	if (clk_hw->div[0].shift > 0)
		return divider_round_rate(hw, rate, prate, NULL,
					clk_hw->div[0].width, clk_hw->div[0].flags);
	else
		return DIV_ROUND_UP_ULL((u64)*prate, 1);
}

static int cv181x_clk_div_determine_rate(struct clk_hw *hw,
				       struct clk_rate_request *req)
{
	struct clk_hw *current_parent;
	unsigned long parent_rate;
	unsigned long best_delta;
	unsigned long best_rate;
	u32 parent_count;
	long rate;
	u32 which;

	pr_debug("%s()_%d: req->rate=%ld\n", __func__, __LINE__, req->rate);

	parent_count = clk_hw_get_num_parents(hw);
	pr_debug("%s()_%d: parent_count=%d\n", __func__, __LINE__, parent_count);

	if ((parent_count < 2) || (clk_hw_get_flags(hw) & CLK_SET_RATE_NO_REPARENT)) {
		rate = cv181x_clk_div_round_rate(hw, req->rate, &req->best_parent_rate);
		if (rate < 0)
			return rate;

		req->rate = rate;
		return 0;
	}

	/* Unless we can do better, stick with current parent */
	current_parent = clk_hw_get_parent(hw);
	parent_rate = clk_hw_get_rate(current_parent);
	best_rate = cv181x_clk_div_calc_round_rate(hw, req->rate, &parent_rate);
	best_delta = abs(best_rate - req->rate);

	pr_debug("%s()_%d: parent_rate=%ld, best_rate=%ld, best_delta=%ld\n",
		 __func__, __LINE__, parent_rate, best_rate, best_delta);

	/* Check whether any other parent clock can produce a better result */
	for (which = 0; which < parent_count; which++) {
		struct clk_hw *parent = clk_hw_get_parent_by_index(hw, which);
		unsigned long delta;
		unsigned long other_rate;

		pr_debug("%s()_%d: idx=%d, parent_rate=%ld, best_rate=%ld, best_delta=%ld\n",
			 __func__, __LINE__, which, parent_rate, best_rate, best_delta);

		if (!parent)
			continue;

		if (parent == current_parent)
			continue;

		/* Not support CLK_SET_RATE_PARENT */
		parent_rate = clk_hw_get_rate(parent);
		other_rate = cv181x_clk_div_calc_round_rate(hw, req->rate, &parent_rate);
		delta = abs(other_rate - req->rate);
		pr_debug("%s()_%d: parent_rate=%ld, other_rate=%ld, delta=%ld\n",
			 __func__, __LINE__, parent_rate, other_rate, delta);
		if (delta < best_delta) {
			best_delta = delta;
			best_rate = other_rate;
			req->best_parent_hw = parent;
			req->best_parent_rate = parent_rate;
			pr_debug("%s()_%d: parent_rate=%ld, best_rate=%ld, best_delta=%ld\n",
				 __func__, __LINE__, parent_rate, best_rate, best_delta);
		}
	}

	req->rate = best_rate;

	return 0;
}

static int cv181x_clk_div_set_rate(struct clk_hw *hw, unsigned long rate,
				 unsigned long parent_rate)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	unsigned int clk_sel = cv181x_clk_get_clk_sel(clk_hw);
	void __iomem *reg_addr = clk_hw->base + clk_hw->div[clk_sel].reg;
	unsigned long flags = 0;
	int value;
	u32 val;

	pr_debug("%s()_%d:%s, rate=%ld, parent_rate=%ld\n", __func__, __LINE__, clk_hw->name, rate, parent_rate);

	if (clk_hw->div[clk_sel].shift < 0)
		pr_err("Error: %s: div[%d].shift = %d\n", __func__, clk_sel, clk_hw->div[clk_sel].shift);

	value = divider_get_val(rate, parent_rate, NULL,
				clk_hw->div[clk_sel].width,
				clk_hw->div[clk_sel].flags);
	if (value < 0)
		return value;

	if (clk_hw->lock)
		spin_lock_irqsave(clk_hw->lock, flags);
	else
		__acquire(clk_hw->lock);

	val = readl(reg_addr);
	val &= ~(div_mask(clk_hw->div[clk_sel].width) << clk_hw->div[clk_sel].shift);
	val |= (u32)value << clk_hw->div[clk_sel].shift;
	if (!(clk_hw->div[clk_sel].initval < 0))
		val |= BIT(3);
	writel(val, reg_addr);

	if (clk_hw->lock)
		spin_unlock_irqrestore(clk_hw->lock, flags);
	else
		__release(clk_hw->lock);

	return 0;
}

static void cv181x_clk_gate_endisable(struct clk_hw *hw, int enable)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	void __iomem *reg_addr = clk_hw->base + clk_hw->gate.reg;
	unsigned long flags = 0;
	u32 reg;

	if (clk_hw->gate.shift < 0)
		pr_err("Error: %s: gate.shift = %d\n", __func__, clk_hw->gate.shift);

	if (clk_hw->lock)
		spin_lock_irqsave(clk_hw->lock, flags);
	else
		__acquire(clk_hw->lock);

	reg = readl(reg_addr);

	if (enable)
		reg |= BIT(clk_hw->gate.shift);
	else
		reg &= ~BIT(clk_hw->gate.shift);

	writel(reg, reg_addr);

	if (clk_hw->lock)
		spin_unlock_irqrestore(clk_hw->lock, flags);
	else
		__release(clk_hw->lock);
}

static int cv181x_clk_gate_enable(struct clk_hw *hw)
{
	cv181x_clk_gate_endisable(hw, 1);

	return 0;
}

static void cv181x_clk_gate_disable(struct clk_hw *hw)
{
	cv181x_clk_gate_endisable(hw, 0);
}

static int cv181x_clk_gate_is_enabled(struct clk_hw *hw)
{
	u32 reg;
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	void __iomem *reg_addr = clk_hw->base + clk_hw->gate.reg;

	if (clk_hw->gate.shift < 0)
		pr_err("Error: %s: gate.shift = %d\n", __func__, clk_hw->gate.shift);

	reg = readl(reg_addr);

	reg &= BIT(clk_hw->gate.shift);

	if (clk_hw_get_flags(hw) & CLK_IGNORE_UNUSED)
		return __clk_get_enable_count(hw->clk) ? (reg ? 1 : 0) : 0;
	else
		return reg ? 1 : 0;
}

static u8 cv181x_clk_mux_get_parent(struct clk_hw *hw)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	u8 clk_sel = cv181x_clk_get_clk_sel(clk_hw);
	u8 src_sel = cv181x_clk_get_src_sel(clk_hw);
	u8 parent_idx = 0;

	/*
	 * | 0     | 1     | 2     | 3     | 4     | 5     |
	 * +-------+-------+-------+-------+-------+-------+
	 * | XTAL  | DIV_1 | src_0 | src_1 | src_2 | src_3 |
	 * | XTAL  | src_0 | src_1 | src_2 | src_3 |       |
	 * | DIV_1 | src_0 | src_1 | src_2 | src_3 |       |
	 * | src_0 | src_1 | src_2 | src_3 |       |       |
	 * +-------+-------+-------+-------+-------+-------+
	 */

	if (clk_hw->mux[0].shift >= 0) {
		// clk with bypass reg
		if (cv181x_clk_is_bypassed(clk_hw)) {
			parent_idx = 0;
		} else {
			if (clk_hw->mux[1].shift >= 0) {
				// clk with clk_sel reg
				if (clk_sel) {
					parent_idx = 1;
				} else {
					parent_idx = src_sel + 2;
				}
			} else {
				// clk without clk_sel reg
				parent_idx = src_sel + 1;
			}
		}
	} else {
		// clk without bypass reg
		if (clk_hw->mux[1].shift >= 0) {
			// clk with clk_sel reg
			if (clk_sel) {
				parent_idx = 0;
			} else {
				parent_idx = src_sel + 1;
			}
		} else {
			//clk without clk_sel reg
			parent_idx = src_sel;
		}
	}

	return parent_idx;
}

static int cv181x_clk_mux_set_parent(struct clk_hw *hw, u8 index)
{
	struct cv181x_hw_clock *clk_hw = to_cv181x_clk(hw);
	unsigned long flags = 0;
	void __iomem *reg_addr;
	unsigned int reg;

	if (clk_hw->lock)
		spin_lock_irqsave(clk_hw->lock, flags);
	else
		__acquire(clk_hw->lock);

	/*
	 * | 0     | 1     | 2     | 3     | 4     | 5     |
	 * +-------+-------+-------+-------+-------+-------+
	 * | XTAL  | DIV_1 | src_0 | src_1 | src_2 | src_3 |
	 * | XTAL  | src_0 | src_1 | src_2 | src_3 |       |
	 * | DIV_1 | src_0 | src_1 | src_2 | src_3 |       |
	 * | src_0 | src_1 | src_2 | src_3 |       |       |
	 * +-------+-------+-------+-------+-------+-------+
	 */

	if (index == 0) {
		if (clk_hw->mux[0].shift >= 0) {
			// set bypass
			reg_addr = clk_hw->base + clk_hw->mux[0].reg;
			reg = readl(reg_addr);
			reg |= 1 << clk_hw->mux[0].shift;
			writel(reg, reg_addr);
			goto unlock_release;
		} else if (clk_hw->mux[1].shift >= 0) {
			// set clk_sel to DIV_1
			reg_addr = clk_hw->base + clk_hw->mux[1].reg;
			reg = readl(reg_addr);
			reg &= ~(1 << clk_hw->mux[1].shift);
			writel(reg, reg_addr);
			goto unlock_release;
		}
	} else if (index == 1) {
		if (clk_hw->mux[0].shift >= 0) {
			// clear bypass
			reg_addr = clk_hw->base + clk_hw->mux[0].reg;
			reg = readl(reg_addr);
			reg &= ~(0x1 << clk_hw->mux[0].shift);
			writel(reg, reg_addr);

			if (clk_hw->mux[1].shift >= 0) {
				// set clk_sel to DIV_1
				reg_addr = clk_hw->base + clk_hw->mux[1].reg;
				reg = readl(reg_addr);
				reg &= ~(1 << clk_hw->mux[1].shift);
				writel(reg, reg_addr);
				goto unlock_release;
			} else {
				index--;
			}
		} else if (clk_hw->mux[1].shift >= 0) {
			// set clk_sel to DIV_0
			reg_addr = clk_hw->base + clk_hw->mux[1].reg;
			reg = readl(reg_addr);
			reg |= 1 << clk_hw->mux[1].shift;
			writel(reg, reg_addr);
			index--;
		}
	} else {
		if (clk_hw->mux[0].shift >= 0) {
			// clear bypass
			reg_addr = clk_hw->base + clk_hw->mux[0].reg;
			reg = readl(reg_addr);
			reg &= ~(0x1 << clk_hw->mux[0].shift);
			writel(reg, reg_addr);
			index--;
		}

		if (clk_hw->mux[1].shift >= 0) {
			// set clk_sel to DIV_0
			reg_addr = clk_hw->base + clk_hw->mux[1].reg;
			reg = readl(reg_addr);
			reg |= 1 << clk_hw->mux[1].shift;
			writel(reg, reg_addr);
			index--;
		}
	}

	if (index < 0) {
		pr_err("index is negative(%d)\n", index);
		goto unlock_release;
	}

	// set src_sel reg
	reg_addr = clk_hw->base + clk_hw->mux[2].reg;
	reg = readl(reg_addr);
	reg &= ~(0x3 << clk_hw->mux[2].shift); // clear bits
	reg |= (index & 0x3) << clk_hw->mux[2].shift; //set bits
	writel(reg, reg_addr);

unlock_release:
	if (clk_hw->lock)
		spin_unlock_irqrestore(clk_hw->lock, flags);
	else
		__release(clk_hw->lock);

	return 0;
}

static const struct clk_ops cv181x_clk_ops = {
	// gate
	.enable = cv181x_clk_gate_enable,
	.disable = cv181x_clk_gate_disable,
	.is_enabled = cv181x_clk_gate_is_enabled,

	// div
	.recalc_rate = cv181x_clk_div_recalc_rate,
	.round_rate = cv181x_clk_div_round_rate,
	.determine_rate = cv181x_clk_div_determine_rate,
	.set_rate = cv181x_clk_div_set_rate,

	//mux
	.get_parent = cv181x_clk_mux_get_parent,
	.set_parent = cv181x_clk_mux_set_parent,
};

static struct clk_hw *cv181x_register_clk(struct cv181x_hw_clock *cv181x_clk,
					  void __iomem *sys_base)
{
	struct clk_hw *hw;
	struct clk_init_data init;
	int err;

	cv181x_clk->gate.flags = 0;
	cv181x_clk->div[0].flags = CLK_DIVIDER_ONE_BASED | CLK_DIVIDER_ALLOW_ZERO |
				 CLK_DIVIDER_ROUND_CLOSEST;
	cv181x_clk->div[1].flags = CLK_DIVIDER_ONE_BASED | CLK_DIVIDER_ALLOW_ZERO |
				 CLK_DIVIDER_ROUND_CLOSEST;
	cv181x_clk->mux[0].flags = 0; /* clk byp */
	cv181x_clk->mux[1].flags = 0; /* clk sel */
	cv181x_clk->mux[2].flags = 0; /* div_0 src_sel */
	cv181x_clk->base = sys_base;
	cv181x_clk->lock = &cv181x_clk_lock;

	if (cvi_clk_flags) {
		/* copy clk_init_data for modification */
		memcpy(&init, cv181x_clk->hw.init, sizeof(init));

		init.flags |= cvi_clk_flags;
		cv181x_clk->hw.init = &init;
	}

	hw = &cv181x_clk->hw;
	err = clk_hw_register(NULL, hw);
	if (err) {
		return ERR_PTR(err);
	}

	return hw;
}

static void cv181x_unregister_clk(struct clk_hw *hw)
{
	struct cv181x_hw_clock *cv181x_clk = to_cv181x_clk(hw);

	clk_hw_unregister(hw);
	kfree(cv181x_clk);
}
static int cv181x_register_clks(struct cv181x_hw_clock *clks,
				int num_clks,
				struct cv181x_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *sys_base = data->base;
	unsigned int i;

	for (i = 0; i < num_clks; i++) {
		struct cv181x_hw_clock *cv181x_clk = &clks[i];

		hw = cv181x_register_clk(cv181x_clk, sys_base);

		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, cv181x_clk->name);
			goto err_clk;
		}
		data->hw_data.hws[clks[i].id] = hw;
		clk_hw_register_clkdev(hw, cv181x_clk->name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		cv181x_unregister_clk(data->hw_data.hws[clks[i].id]);

	return PTR_ERR(hw);
}

static const struct of_device_id cvi_clk_match_ids_tables[] = {
	{
		.compatible = "cvitek,cv181x-clk",
	},
	{}
};

#ifdef CONFIG_PM_SLEEP
static int cv181x_clk_suspend(void)
{
	memcpy_fromio(clk_data->clken_saved_regs,
		      clk_data->base + REG_CLK_EN_START,
		      REG_CLK_EN_NUM * 4);

	memcpy_fromio(clk_data->clksel_saved_regs,
		      clk_data->base + REG_CLK_SEL_START,
		      REG_CLK_SEL_NUM * 4);

	memcpy_fromio(clk_data->clkbyp_saved_regs,
		      clk_data->base + REG_CLK_BYP_START,
		      REG_CLK_BYP_NUM * 4);

	memcpy_fromio(clk_data->clkdiv_saved_regs,
		      clk_data->base + REG_CLK_DIV_START,
		      REG_CLK_DIV_NUM * 4);

	memcpy_fromio(clk_data->g2_clkdiv_saved_regs,
		      clk_data->base + REG_CLK_G2_DIV_START,
		      REG_CLK_G2_DIV_NUM * 4);

	memcpy_fromio(clk_data->pll_g2_csr_saved_regs,
		      clk_data->base + REG_PLL_G2_CSR_START,
		      REG_PLL_G2_CSR_NUM * 4);

	memcpy_fromio(clk_data->pll_g6_csr_saved_regs,
		      clk_data->base + REG_PLL_G6_CSR_START,
		      REG_PLL_G6_CSR_NUM * 4);

	clk_data->a0pll_ssc_syn_set_saved_reg =
		readl(clk_data->base + REG_APLL_SSC_SYN_SET);

	clk_data->disppll_ssc_syn_set_saved_reg =
		readl(clk_data->base + REG_DISPPLL_SSC_SYN_SET);

	clk_data->cam0pll_ssc_syn_set_saved_reg =
		readl(clk_data->base + REG_CAM0PLL_SSC_SYN_SET);

	clk_data->cam1pll_ssc_syn_set_saved_reg =
		readl(clk_data->base + REG_CAM1PLL_SSC_SYN_SET);

	return 0;
}

static void cv181x_clk_resume(void)
{
	uint32_t regval;

	/* switch clock to xtal */
	writel(0xffffffff, clk_data->base + REG_CLK_BYP_0);
	writel(0x0000000f, clk_data->base + REG_CLK_BYP_1);

	memcpy_toio(clk_data->base + REG_CLK_EN_START,
		    clk_data->clken_saved_regs,
		    REG_CLK_EN_NUM * 4);

	memcpy_toio(clk_data->base + REG_CLK_SEL_START,
		    clk_data->clksel_saved_regs,
		    REG_CLK_SEL_NUM * 4);

	memcpy_toio(clk_data->base + REG_CLK_DIV_START,
		    clk_data->clkdiv_saved_regs,
		    REG_CLK_DIV_NUM * 4);

	memcpy_toio(clk_data->base + REG_CLK_G2_DIV_START,
		    clk_data->g2_clkdiv_saved_regs,
		    REG_CLK_G2_DIV_NUM * 4);

	memcpy_toio(clk_data->base + REG_PLL_G6_CSR_START,
		    clk_data->pll_g6_csr_saved_regs,
		    REG_PLL_G6_CSR_NUM * 4);

	/* wait for pll setting updated */
	while (readl(clk_data->base + REG_PLL_G6_STATUS) & 0x7) {
	}

	/* A0PLL */
	if (clk_data->a0pll_ssc_syn_set_saved_reg !=
	    readl(clk_data->base + REG_APLL_SSC_SYN_SET)) {
		pr_debug("%s: update A0PLL\n", __func__);
		writel(clk_data->a0pll_ssc_syn_set_saved_reg,
		       clk_data->base + REG_APLL_SSC_SYN_SET);

		/* toggle software update */
		regval = readl(clk_data->base + REG_APLL_SSC_SYN_CTRL);
		regval ^= 1;
		writel(regval, clk_data->base + REG_APLL_SSC_SYN_CTRL);
	}

	/* DISPPLL */
	if (clk_data->disppll_ssc_syn_set_saved_reg !=
	    readl(clk_data->base + REG_DISPPLL_SSC_SYN_SET)) {
		pr_debug("%s: update DISPPLL\n", __func__);
		writel(clk_data->disppll_ssc_syn_set_saved_reg,
		       clk_data->base + REG_DISPPLL_SSC_SYN_SET);

		/* toggle software update */
		regval = readl(clk_data->base + REG_DISPPLL_SSC_SYN_CTRL);
		regval ^= 1;
		writel(regval, clk_data->base + REG_DISPPLL_SSC_SYN_CTRL);
	}

	/* CAM0PLL */
	if (clk_data->cam0pll_ssc_syn_set_saved_reg !=
	    readl(clk_data->base + REG_CAM0PLL_SSC_SYN_SET)) {
		pr_debug("%s: update CAM0PLL\n", __func__);
		writel(clk_data->cam0pll_ssc_syn_set_saved_reg,
		       clk_data->base + REG_CAM0PLL_SSC_SYN_SET);

		/* toggle software update */
		regval = readl(clk_data->base + REG_CAM0PLL_SSC_SYN_CTRL);
		regval ^= 1;
		writel(regval, clk_data->base + REG_CAM0PLL_SSC_SYN_CTRL);
	}

	/* CAM1PLL */
	if (clk_data->cam1pll_ssc_syn_set_saved_reg !=
	    readl(clk_data->base + REG_CAM1PLL_SSC_SYN_SET)) {
		pr_debug("%s: update CAM1PLL\n", __func__);
		writel(clk_data->cam1pll_ssc_syn_set_saved_reg,
		       clk_data->base + REG_CAM1PLL_SSC_SYN_SET);

		/* toggle software update */
		regval = readl(clk_data->base + REG_CAM1PLL_SSC_SYN_CTRL);
		regval ^= 1;
		writel(regval, clk_data->base + REG_CAM1PLL_SSC_SYN_CTRL);
	}

	memcpy_toio(clk_data->base + REG_PLL_G2_CSR_START,
		    clk_data->pll_g2_csr_saved_regs,
		    REG_PLL_G2_CSR_NUM * 4);

	/* wait for pll setting updated */
	while (readl(clk_data->base + REG_PLL_G2_STATUS) & 0x1F) {
	}

	memcpy_toio(clk_data->base + REG_CLK_BYP_START,
		    clk_data->clkbyp_saved_regs,
		    REG_CLK_BYP_NUM * 4);
}

static struct syscore_ops cv181x_clk_syscore_ops = {
	.suspend = cv181x_clk_suspend,
	.resume = cv181x_clk_resume,
};
#endif /* CONFIG_PM_SLEEP */

static void __init cvi_clk_init(struct device_node *node)
{
	int num_clks;
	int i;
	int ret = 0;
	int of_num_clks;
	struct clk *clk;

	of_num_clks = of_clk_get_parent_count(node);
	for (i = 0; i < of_num_clks; i++) {
		clk = of_clk_get(node, i);
		clk_register_clkdev(clk, __clk_get_name(clk), NULL);
		clk_put(clk);
	}

	num_clks = ARRAY_SIZE(cv181x_pll_clks) +
		   ARRAY_SIZE(cv181x_clks);

	clk_data = kzalloc(sizeof(struct cv181x_clock_data) +
			   sizeof(struct clk_hw) * num_clks,
			   GFP_KERNEL);
	if (!clk_data) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < num_clks; i++)
		clk_data->hw_data.hws[i] = ERR_PTR(-ENOENT);

	clk_data->hw_data.num = num_clks;

	clk_data->lock = &cv181x_clk_lock;

	clk_data->base = of_iomap(node, 0);
	if (!clk_data->base) {
		pr_err("Failed to map address range for cvitek,cv181x-clk node\n");
		return;
	}

	cv181x_clk_register_plls(cv181x_pll_clks,
			       ARRAY_SIZE(cv181x_pll_clks),
			       clk_data);

	cv181x_register_clks(cv181x_clks,
			   ARRAY_SIZE(cv181x_clks),
			   clk_data);


	/* register clk-provider */
	ret = of_clk_add_hw_provider(node, of_clk_hw_onecell_get, &clk_data->hw_data);
	if (ret)
		pr_err("Unable to add hw clk provider\n");

	/* force enable clocks */
	// clk_prepare_enable(clk_data->hw_data.hws[CV181X_CLK_DSI_MAC_VIP]->clk);
	// clk_prepare_enable(clk_data->hw_data.hws[CV181X_CLK_DISP_VIP]->clk);
	// clk_prepare_enable(clk_data->hw_data.hws[CV181X_CLK_BT_VIP]->clk);
	// clk_prepare_enable(clk_data->hw_data.hws[CV181X_CLK_SC_TOP_VIP]->clk);

#ifdef CONFIG_PM_SLEEP
	register_syscore_ops(&cv181x_clk_syscore_ops);
#endif

	if (!ret)
		return;

out:
	pr_err("%s failed error number %d\n", __func__, ret);
}
CLK_OF_DECLARE(cvi_clk, "cvitek,cv181x-clk", cvi_clk_init);
