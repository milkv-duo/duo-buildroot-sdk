/*
 * Copyright (c) 2020 CVITEK
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

#include <dt-bindings/clock/cv1835-clock.h>

#define CV1835_CLK_FLAGS_ALL	(CLK_GET_RATE_NOCACHE)
// #define CV1835_CLK_FLAGS_ALL	(CLK_GET_RATE_NOCACHE | CLK_IS_CRITICAL)
// #define CV1835_CLK_FLAGS_ALL	(CLK_GET_RATE_NOCACHE | CLK_IGNORE_UNUSED)
#define CV1835_CLK_FLAGS_MUX	(CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT)

/* top_pll_g2 */
#define CV1835_CLK_PLL_G2_CTRL		0x800
#define CV1835_CLK_PLL_G2_STATUS	0x804
#define CV1835_CLK_MIPIMPLL_CSR		0x808
#define CV1835_CLK_A0PLL_CSR		0x80C
#define CV1835_CLK_DISPPLL_CSR		0x810
#define CV1835_CLK_CAM0PLL_CSR		0x814
#define CV1835_CLK_CAM1PLL_CSR		0x818
#define CV1835_CLK_PLL_G2_SSC_SYN_CTRL	0x840
#define CV1835_CLK_A0PLL_SSC_SYN_CTRL	0x850
#define CV1835_CLK_A0PLL_SSC_SYN_SET	0x854
#define CV1835_CLK_A0PLL_SSC_SYN_SPAN	0x858
#define CV1835_CLK_A0PLL_SSC_SYN_STEP	0x85C
#define CV1835_CLK_DISPPLL_SSC_SYN_CTRL	0x860
#define CV1835_CLK_DISPPLL_SSC_SYN_SET	0x864
#define CV1835_CLK_DISPPLL_SSC_SYN_SPAN	0x868
#define CV1835_CLK_DISPPLL_SSC_SYN_STEP	0x86C
#define CV1835_CLK_CAM0PLL_SSC_SYN_CTRL	0x870
#define CV1835_CLK_CAM0PLL_SSC_SYN_SET	0x874
#define CV1835_CLK_CAM0PLL_SSC_SYN_SPAN	0x878
#define CV1835_CLK_CAM0PLL_SSC_SYN_STEP	0x87C
#define CV1835_CLK_CAM1PLL_SSC_SYN_CTRL	0x880
#define CV1835_CLK_CAM1PLL_SSC_SYN_SET	0x884
#define CV1835_CLK_CAM1PLL_SSC_SYN_SPAN	0x888
#define CV1835_CLK_CAM1PLL_SSC_SYN_STEP	0x88C

/* top_pll_g6 */
#define CV1835_CLK_PLL_G6_CTRL		0x900
#define CV1835_CLK_PLL_G6_STATUS	0x904
#define CV1835_CLK_MPLL_CSR		0x908
#define CV1835_CLK_TPLL_CSR		0x90C
#define CV1835_CLK_FPLL_CSR		0x910
#define CV1835_CLK_PLL_G6_SSC_SYN_CTRL	0x940

/* clkgen */
#define CV1835_CLK_ENABLE0		0x000
#define CV1835_CLK_ENABLE1		0x004
#define CV1835_CLK_ENABLE2		0x008
#define CV1835_CLK_ENABLE3		0x00C
#define CV1835_CLK_ENABLE4		0x010
#define CV1835_CLK_SELECT		0x020
#define CV1835_CLK_BYPASS0		0x030
#define CV1835_CLK_BYPASS1		0x034
#define CV1835_CLK_A53_DIV0		0x040
#define CV1835_CLK_A53_DIV1		0x044
#define CV1835_CLK_CPU_AXI0_DIV0	0x048
#define CV1835_CLK_CPU_AXI0_DIV1	0x04C
#define CV1835_CLK_CPU_GIC_DIV		0x050
#define CV1835_CLK_TPU_AXI_DIV0		0x054
#define CV1835_CLK_TPU_AXI_DIV1		0x058
#define CV1835_CLK_TPU_FAB_DIV0		0x05C
#define CV1835_CLK_TPU_FAB_DIV1		0x060
#define CV1835_CLK_EMMC_DIV0		0x064
#define CV1835_CLK_EMMC_DIV1		0x068
#define CV1835_CLK_100K_EMMC_DIV	0x06C
#define CV1835_CLK_SD0_DIV0		0x070
#define CV1835_CLK_SD0_DIV1		0x074
#define CV1835_CLK_100K_SD0_DIV		0x078
#define CV1835_CLK_SD1_DIV0		0x07C
#define CV1835_CLK_SD1_DIV1		0x080
#define CV1835_CLK_100K_SD1_DIV		0x084
#define CV1835_CLK_SPI_NAND_DIV		0x088
#define CV1835_CLK_500M_ETH0_DIV	0x08C
#define CV1835_CLK_500M_ETH1_DIV	0x090
#define CV1835_CLK_GPIO_DB_DIV		0x094
#define CV1835_CLK_SDMA_AUD0_DIV	0x098
#define CV1835_CLK_SDMA_AUD1_DIV	0x09C
#define CV1835_CLK_SDMA_AUD2_DIV	0x0A0
#define CV1835_CLK_SDMA_AUD3_DIV	0x0A4
#define CV1835_CLK_187P5M_DIV		0x0A8
#define CV1835_CLK_125M_USB_DIV		0x0AC
#define CV1835_CLK_33K_USB_DIV		0x0B0
#define CV1835_CLK_12M_USB_DIV		0x0B4
#define CV1835_CLK_AXI4_DIV		0x0B8
#define CV1835_CLK_AXI6_DIV0		0x0BC
#define CV1835_CLK_AXI6_DIV1		0x0C0
#define CV1835_CLK_DSI_ESC_DIV		0x0C4
#define CV1835_CLK_AXI_VIP_DIV0		0x0C8
#define CV1835_CLK_AXI_VIP_DIV1		0x0CC
#define CV1835_CLK_SRC_VIP_SYS_0_DIV0	0x0D0
#define CV1835_CLK_SRC_VIP_SYS_0_DIV1	0x0D4
#define CV1835_CLK_SRC_VIP_SYS_1_DIV0	0x0D8
#define CV1835_CLK_SRC_VIP_SYS_1_DIV1	0x0DC
#define CV1835_CLK_DISP_SRC_VIP_DIV	0x0E0
#define CV1835_CLK_AXI_VIDEO_CODEC_DIV0	0x0E4
#define CV1835_CLK_AXI_VIDEO_CODEC_DIV1	0x0E8
#define CV1835_CLK_VC_SRC0_DIV0		0x0EC
#define CV1835_CLK_VC_SRC0_DIV1		0x0F0
#define CV1835_CLK_CAM0_DIV		0x0F4
#define CV1835_CLK_CAM1_DIV		0x0F8
#define CV1835_CLK_1M_DIV		0x0FC
#define CV1835_CLK_SPI_DIV		0x100
#define CV1835_CLK_I2C_DIV		0x104
#define CV1835_CLK_VC_SRC1_DIV0		0x108
#define CV1835_CLK_VC_SRC1_DIV1		0x10C
#define CV1835_CLK_SRC_VIP_SYS_2_DIV0	0x110
#define CV1835_CLK_SRC_VIP_SYS_2_DIV1	0x114
#define CV1835_CLK_AUDSRC_DIV		0x118
#define CV1835_CLK_VC_SRC2_DIV		0x11C
#define CV1835_CLK_PWM_SRC_DIV0		0x120
#define CV1835_CLK_PWM_SRC_DIV1		0x124

#define CV1835_VCSYS_BUS_IDLE		0x03000210
#define CV1835_BUS_IDLE_TIMEOUT_MS	1

/* PLL status register offset */
#define PLL_STATUS_MASK			0xFF
#define PLL_STATUS_OFFSET		0x04

/* G2 Synthesizer register offset */
#define G2_SSC_CTRL_MASK		0xFF
#define G2_SSC_CTRL_OFFSET		0x40
#define SSC_SYN_SET_MASK		0x0F
#define SSC_SYN_SET_OFFSET		0x04

#define to_cv1835_pll_clk(_hw) container_of(_hw, struct cv1835_pll_hw_clock, hw)
#define to_cv1835_div_clk(_hw) container_of(_hw, struct cv1835_div_hw_clock, hw)
#define to_cv1835_gate_clk(_hw) container_of(_hw, struct cv1835_gate_hw_clock, hw)

#define div_mask(width) ((1 << (width)) - 1)

static DEFINE_SPINLOCK(cv1835_clk_lock);

struct cv1835_clock_data {
	void __iomem *base;
	spinlock_t *lock;
	struct clk_hw_onecell_data hw_data;
};

struct cv1835_gate_clock {
	unsigned int	id;
	const char	*name;
	const char	*parent;
	u32		reg;
	s8		shift;
	u32		idle_reg;
	u32		idle_state;
	unsigned long	gate_flags;
};

struct cv1835_gate_hw_clock {
	struct cv1835_gate_clock gate;
	void __iomem *base;
	spinlock_t *lock;
	struct clk_hw hw;
};

struct cv1835_mux_clock {
	unsigned int	id;
	const char	*name;
	const char      * const *parents;
	s8		num_parents;
	u32		reg;
	s8		shift;
	unsigned long	flags;
};

struct cv1835_div_clock {
	unsigned int	id;
	const char	*name;
	u32		reg;
	u8		shift;
	u8		width;
	u32		initval;
	const struct clk_div_table *table;
	unsigned long div_flags;
};

struct cv1835_div_hw_clock {
	struct cv1835_div_clock div;
	void __iomem *base;
	spinlock_t *lock;
	struct clk_hw hw;
};

struct cv1835_composite_clock {
	unsigned int	id;
	const char	*name;
	const char	*parent;
	const char      * const *parents;
	unsigned int	num_parents;
	unsigned long	flags;

	u32		gate_reg;
	u32		mux_reg;
	u32		div_reg;

	s8		gate_shift;
	s8		mux_shift;
	s8		div_shift;
	s8		div_width;
	s16		div_initval;
	const struct clk_div_table *table;
};

struct cv1835_pll_clock {
	unsigned int	id;
	const char	*name;
	u32		reg_csr;
	u32		reg_ssc;
	unsigned long	flags;
};

struct cv1835_pll_hw_clock {
	struct cv1835_pll_clock pll;
	void __iomem *base;
	spinlock_t *lock;
	struct clk_hw hw;
};

static const struct clk_ops cv1835_g6_pll_ops;
static const struct clk_ops cv1835_g2_pll_ops;
static const struct clk_ops cv1835_clk_div_ops;
static const struct clk_ops cv1835_clk_gate_ops;

static unsigned long cvi_clk_flags;

#define GATE_DIV(_id, _name, _parent, _gate_reg, _gate_shift, _div_reg,	\
			_div_shift, _div_width, _div_initval, _table,	\
			_flags) {					\
		.id = _id,						\
		.parent = _parent,					\
		.name = _name,						\
		.gate_reg = _gate_reg,					\
		.gate_shift = _gate_shift,				\
		.div_reg = _div_reg,					\
		.div_shift = _div_shift,				\
		.div_width = _div_width,				\
		.div_initval = _div_initval,				\
		.table = _table,					\
		.mux_shift = -1,					\
		.flags = _flags | CV1835_CLK_FLAGS_ALL,			\
	}

#define GATE_MUX(_id, _name, _parents, _gate_reg, _gate_shift,		\
			_mux_reg, _mux_shift, _flags) {			\
		.id = _id,						\
		.parents = _parents,					\
		.num_parents = ARRAY_SIZE(_parents),			\
		.name = _name,						\
		.gate_reg = _gate_reg,					\
		.gate_shift = _gate_shift,				\
		.div_shift = -1,					\
		.mux_reg = _mux_reg,					\
		.mux_shift = _mux_shift,				\
		.flags = _flags |					\
			 CV1835_CLK_FLAGS_ALL | CV1835_CLK_FLAGS_MUX,	\
	}

#define CLK_G6_PLL(_id, _name, _parent, _reg_csr, _flags) {		\
		.pll.id = _id,						\
		.pll.name = _name,					\
		.pll.reg_csr = _reg_csr,				\
		.pll.reg_ssc = 0,					\
		.hw.init = CLK_HW_INIT_PARENTS(_name, _parent,		\
					       &cv1835_g6_pll_ops,	\
					       _flags |			\
					       CV1835_CLK_FLAGS_ALL),	\
	}

#define CLK_G2_PLL(_id, _name, _parent, _reg_csr, _reg_ssc, _flags) {	\
		.pll.id = _id,						\
		.pll.name = _name,					\
		.pll.reg_csr = _reg_csr,				\
		.pll.reg_ssc = _reg_ssc,				\
		.hw.init = CLK_HW_INIT_PARENTS(_name, _parent,		\
					       &cv1835_g2_pll_ops,	\
					       _flags |			\
					       CV1835_CLK_FLAGS_ALL),	\
	}

#define CLK_DIV(_id, _name, _parent, _reg, _shift, _width, _initval,	\
			_table,	_flags) {				\
		.div.id = _id,						\
		.div.name = _name,					\
		.div.reg = _reg,					\
		.div.shift = _shift,					\
		.div.width = _width,					\
		.div.initval = _initval,				\
		.div.table = _table,					\
		.hw.init = CLK_HW_INIT(_name, _parent,			\
				       &cv1835_clk_div_ops,		\
				       _flags | CV1835_CLK_FLAGS_ALL),	\
	}

#define CLK_GATE(_id, _name, _parent, _reg, _shift, _flags) {		\
		.gate.id = _id,						\
		.gate.name = _name,					\
		.gate.reg = _reg,					\
		.gate.shift = _shift,					\
		.gate.idle_reg = 0,					\
		.gate.idle_state = 0,					\
		.hw.init = CLK_HW_INIT(_name, _parent,			\
				       &cv1835_clk_gate_ops,		\
				       _flags | CV1835_CLK_FLAGS_ALL),	\
	}

#define CLK_GATE_IDLE(_id, _name, _parent, _reg, _shift, _flags,	\
			_idle_reg, _idle_state) {			\
		.gate.id = _id,						\
		.gate.name = _name,					\
		.gate.reg = _reg,					\
		.gate.shift = _shift,					\
		.gate.idle_reg = _idle_reg,				\
		.gate.idle_state = _idle_state,				\
		.hw.init = CLK_HW_INIT(_name, _parent,			\
				       &cv1835_clk_gate_ops,		\
				       _flags | CV1835_CLK_FLAGS_ALL),	\
	}

const char *const cv1835_pll_parent[] = {"osc"};
const char *const cv1835_frac_pll_parent[] = {"clk_mipimpll"};

/*
 * All PLL clocks are marked as CRITICAL, hence they are very crucial
 * for the functioning of the SoC
 */
static struct cv1835_pll_hw_clock cv1835_pll_clks[] = {
	CLK_G6_PLL(CV1835_CLK_MPLL, "clk_mpll", cv1835_pll_parent,
		CV1835_CLK_MPLL_CSR, 0),
	CLK_G6_PLL(CV1835_CLK_TPLL, "clk_tpll", cv1835_pll_parent,
		CV1835_CLK_TPLL_CSR, 0),
	CLK_G6_PLL(CV1835_CLK_FPLL, "clk_fpll", cv1835_pll_parent,
		CV1835_CLK_FPLL_CSR, 0),
	CLK_G2_PLL(CV1835_CLK_MIPIMPLL, "clk_mipimpll", cv1835_pll_parent,
		CV1835_CLK_MIPIMPLL_CSR, 0, 0),
	CLK_G2_PLL(CV1835_CLK_A0PLL, "clk_a0pll", cv1835_frac_pll_parent,
		CV1835_CLK_A0PLL_CSR, CV1835_CLK_A0PLL_SSC_SYN_CTRL, 0),
	CLK_G2_PLL(CV1835_CLK_DISPPLL, "clk_disppll", cv1835_frac_pll_parent,
		CV1835_CLK_DISPPLL_CSR, CV1835_CLK_DISPPLL_SSC_SYN_CTRL, 0),
	CLK_G2_PLL(CV1835_CLK_CAM0PLL, "clk_cam0pll", cv1835_frac_pll_parent,
		CV1835_CLK_CAM0PLL_CSR, CV1835_CLK_CAM0PLL_SSC_SYN_CTRL, 0),
	CLK_G2_PLL(CV1835_CLK_CAM1PLL, "clk_cam1pll", cv1835_frac_pll_parent,
		CV1835_CLK_CAM1PLL_CSR, CV1835_CLK_CAM1PLL_SSC_SYN_CTRL, 0),
};

/*
 * Clocks marked as CRITICAL are needed for the proper functioning
 * of the SoC.
 */
static struct cv1835_gate_hw_clock cv1835_gate_clks[] = {
	CLK_GATE(CV1835_CLK_XTAL_A53, "clk_xtal_a53", "osc",
		CV1835_CLK_ENABLE0, 3, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AHB_ROM, "clk_ahb_rom", "clk_div_axi4",
		CV1835_CLK_ENABLE0, 6, 0),
	CLK_GATE(CV1835_CLK_DDR_AXI_REG, "clk_ddr_axi_reg", "clk_mux_axi6",
		CV1835_CLK_ENABLE0, 7, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_RTC_25M, "clk_rtc_25m", "osc",
		CV1835_CLK_ENABLE0, 8, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TEMPSEN, "clk_tempsen", "osc",
		CV1835_CLK_ENABLE0, 9, 0),
	CLK_GATE(CV1835_CLK_SARADC, "clk_saradc", "osc",
		CV1835_CLK_ENABLE0, 10, 0),
	CLK_GATE(CV1835_CLK_EFUSE, "clk_efuse", "osc",
		CV1835_CLK_ENABLE0, 11, 0),
	CLK_GATE(CV1835_CLK_APB_EFUSE, "clk_apb_efuse", "osc",
		CV1835_CLK_ENABLE0, 12, 0),
	CLK_GATE(CV1835_CLK_XTAL_MISC, "clk_xtal_misc", "osc",
		CV1835_CLK_ENABLE0, 14, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AXI4_EMMC, "clk_axi4_emmc", "clk_div_axi4",
		CV1835_CLK_ENABLE0, 15, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AXI4_SD0, "clk_axi4_sd0", "clk_div_axi4",
		CV1835_CLK_ENABLE0, 18, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AXI4_SD1, "clk_axi4_sd1", "clk_div_axi4",
		CV1835_CLK_ENABLE0, 21, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AXI4_ETH0, "clk_axi4_eth0", "clk_div_axi4",
		CV1835_CLK_ENABLE0, 26, 0),
	CLK_GATE(CV1835_CLK_AXI4_ETH1, "clk_axi4_eth1", "clk_div_axi4",
		CV1835_CLK_ENABLE0, 28, 0),
	CLK_GATE(CV1835_CLK_APB_GPIO, "clk_apb_gpio", "clk_mux_axi6",
		CV1835_CLK_ENABLE0, 29, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_GPIO_INTR, "clk_apb_gpio_intr", "clk_mux_axi6",
		CV1835_CLK_ENABLE0, 30, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AHB_SF, "clk_ahb_sf", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 0, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_SDMA_AXI, "clk_sdma_axi", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 1, 0),
	CLK_GATE(CV1835_CLK_APB_I2C, "clk_apb_i2c", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 6, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_WDT, "clk_apb_wdt", "osc",
		CV1835_CLK_ENABLE1, 7, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_SPI0, "clk_apb_spi0", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 9, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_SPI1, "clk_apb_spi1", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 10, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_SPI2, "clk_apb_spi2", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 11, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_SPI3, "clk_apb_spi3", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 12, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_187P5M, "clk_187p5m", "clk_div_187p5m",
		CV1835_CLK_ENABLE1, 13, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_UART0, "clk_apb_uart0", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 15, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_UART1, "clk_apb_uart1", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 17, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_UART2, "clk_apb_uart2", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 19, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_UART3, "clk_apb_uart3", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 21, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_UART4, "clk_apb_uart4", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 23, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2S0, "clk_apb_i2s0", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 24, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2S1, "clk_apb_i2s1", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 25, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2S2, "clk_apb_i2s2", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 26, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2S3, "clk_apb_i2s3", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 27, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AXI4_USB, "clk_axi4_usb", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 28, 0),
	CLK_GATE(CV1835_CLK_APB_USB, "clk_apb_usb", "clk_div_axi4",
		CV1835_CLK_ENABLE1, 29, 0),
	CLK_GATE(CV1835_CLK_AXI4, "clk_axi4", "clk_div_axi4",
		CV1835_CLK_ENABLE2, 1, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_AXI6, "clk_axi6", "clk_mux_axi6",
		CV1835_CLK_ENABLE2, 2, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_H264C, "clk_h264c", "clk_axi_video_codec",
		CV1835_CLK_ENABLE2, 10, 0),
	CLK_GATE(CV1835_CLK_H265C, "clk_h265c", "clk_axi_video_codec",
		CV1835_CLK_ENABLE2, 11, 0),
	CLK_GATE(CV1835_CLK_JPEG, "clk_jpeg", "clk_axi_video_codec",
		CV1835_CLK_ENABLE2, 12, 0),
	CLK_GATE(CV1835_CLK_APB_JPEG, "clk_apb_jpeg", "clk_axi6",
		CV1835_CLK_ENABLE2, 13, 0),
	CLK_GATE(CV1835_CLK_APB_H264C, "clk_apb_h264c", "clk_axi6",
		CV1835_CLK_ENABLE2, 14, 0),
	CLK_GATE(CV1835_CLK_APB_H265C, "clk_apb_h265c", "clk_axi6",
		CV1835_CLK_ENABLE2, 15, 0),
	CLK_GATE(CV1835_CLK_CSI_MAC0_VIP, "clk_csi_mac0_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 18, 0),
	CLK_GATE(CV1835_CLK_CSI_MAC1_VIP, "clk_csi_mac1_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 19, 0),
	CLK_GATE(CV1835_CLK_ISP_TOP_VIP, "clk_isp_top_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 20, 0),
	CLK_GATE(CV1835_CLK_IMG_D_VIP, "clk_img_d_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 21, 0),
	CLK_GATE(CV1835_CLK_IMG_V_VIP, "clk_img_v_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 22, 0),
	CLK_GATE(CV1835_CLK_SC_TOP_VIP, "clk_sc_top_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 23, 0),
	CLK_GATE(CV1835_CLK_SC_D_VIP, "clk_sc_d_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 24, 0),
	CLK_GATE(CV1835_CLK_SC_V1_VIP, "clk_sc_v1_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 25, 0),
	CLK_GATE(CV1835_CLK_SC_V2_VIP, "clk_sc_v2_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 26, 0),
	CLK_GATE(CV1835_CLK_SC_V3_VIP, "clk_sc_v3_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 27, 0),
	CLK_GATE(CV1835_CLK_DWA_VIP, "clk_dwa_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 28, 0),
	CLK_GATE(CV1835_CLK_BT_VIP, "clk_bt_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 29, 0),
	CLK_GATE(CV1835_CLK_DISP_VIP, "clk_disp_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 30, 0),
	CLK_GATE(CV1835_CLK_DSI_MAC_VIP, "clk_dsi_mac_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE2, 31, 0),
	CLK_GATE(CV1835_CLK_LVDS0_VIP, "clk_lvds0_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE3, 0, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_LVDS1_VIP, "clk_lvds1_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE3, 1, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_CSI0_RX_VIP, "clk_csi0_rx_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE3, 2, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_CSI1_RX_VIP, "clk_csi1_rx_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE3, 3, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_PAD_VI_VIP, "clk_pad_vi_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE3, 4, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_1M, "clk_1m", "clk_div_1m",
		CV1835_CLK_ENABLE3, 5, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_PM, "clk_pm", "clk_mux_axi6",
		CV1835_CLK_ENABLE3, 8, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER0, "clk_timer0", "osc",
		CV1835_CLK_ENABLE3, 9, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER1, "clk_timer1", "osc",
		CV1835_CLK_ENABLE3, 10, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER2, "clk_timer2", "osc",
		CV1835_CLK_ENABLE3, 11, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER3, "clk_timer3", "osc",
		CV1835_CLK_ENABLE3, 12, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER4, "clk_timer4", "osc",
		CV1835_CLK_ENABLE3, 13, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER5, "clk_timer5", "osc",
		CV1835_CLK_ENABLE3, 14, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER6, "clk_timer6", "osc",
		CV1835_CLK_ENABLE3, 15, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_TIMER7, "clk_timer7", "osc",
		CV1835_CLK_ENABLE3, 16, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2C0, "clk_apb_i2c0", "clk_div_axi4",
		CV1835_CLK_ENABLE3, 17, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2C1, "clk_apb_i2c1", "clk_div_axi4",
		CV1835_CLK_ENABLE3, 18, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2C2, "clk_apb_i2c2", "clk_div_axi4",
		CV1835_CLK_ENABLE3, 19, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2C3, "clk_apb_i2c3", "clk_div_axi4",
		CV1835_CLK_ENABLE3, 20, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_APB_I2C4, "clk_apb_i2c4", "clk_div_axi4",
		CV1835_CLK_ENABLE3, 21, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_WGN, "clk_wgn", "osc",
		CV1835_CLK_ENABLE3, 22, 0),
	CLK_GATE(CV1835_CLK_WGN0, "clk_wgn0", "osc",
		CV1835_CLK_ENABLE3, 23, 0),
	CLK_GATE(CV1835_CLK_WGN1, "clk_wgn1", "osc",
		CV1835_CLK_ENABLE3, 24, 0),
	CLK_GATE(CV1835_CLK_WGN2, "clk_wgn2", "osc",
		CV1835_CLK_ENABLE3, 25, 0),
	CLK_GATE(CV1835_CLK_KEYSCAN, "clk_keyscan", "osc",
		CV1835_CLK_ENABLE3, 26, 0),
	CLK_GATE(CV1835_CLK_AHB_SF1, "clk_ahb_sf1", "clk_div_axi4",
		CV1835_CLK_ENABLE3, 27, 0),
	CLK_GATE(CV1835_CLK_PAD_VI1_VIP, "clk_pad_vi1_vip", "clk_axi_vip",
		CV1835_CLK_ENABLE3, 30, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_CFG_REG_VIP, "clk_cfg_reg_vip", "clk_mux_axi6",
		CV1835_CLK_ENABLE3, 31, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_CFG_REG_VC, "clk_cfg_reg_vc", "clk_mux_axi6",
		CV1835_CLK_ENABLE4, 0, 0),
	CLK_GATE(CV1835_CLK_APB_AUDSRC, "clk_apb_audsrc", "clk_div_axi4",
		CV1835_CLK_ENABLE4, 2, CLK_IS_CRITICAL),
	CLK_GATE(CV1835_CLK_PWM_SRC, "clk_pwm_src", "clk_mux_pwm_src",
		CV1835_CLK_ENABLE4, 4, 0),
	CLK_GATE_IDLE(CV1835_CLK_AXI_VIDEO_CODEC, "clk_axi_video_codec", "clk_mux_axi_video_codec",
		CV1835_CLK_ENABLE2, 8, 0, CV1835_VCSYS_BUS_IDLE, 0x7F),
};

static const char * const clk_axi6_parents[] = { "clk_div_1_axi6", "clk_div_0_axi6" };
static const char * const clk_a53_parents[] = { "clk_div_1_a53", "clk_div_0_a53" };
static const char * const clk_cpu_axi0_parents[] = { "clk_div_1_cpu_axi0", "clk_div_0_cpu_axi0" };
static const char * const clk_tpu_axi_parents[] = { "clk_div_1_tpu_axi", "clk_div_0_tpu_axi" };
static const char * const clk_tpu_fab_parents[] = { "clk_div_1_tpu_fab", "clk_div_0_tpu_fab" };
static const char * const clk_debug_parents[] = { "clk_mux_axi6", "osc" };
static const char * const clk_emmc_parents[] = { "clk_div_1_emmc", "clk_div_0_emmc" };
static const char * const clk_sd0_parents[] = { "clk_div_1_sd0", "clk_div_0_sd0" };
static const char * const clk_sd1_parents[] = { "clk_div_1_sd1", "clk_div_0_sd1" };
static const char * const clk_apb_pwm_parents[] = { "clk_mux_axi6", "clk_mux_pwm_src" };
static const char * const clk_uart0_parents[] = { "clk_div_187p5m", "osc" };
static const char * const clk_uart1_parents[] = { "clk_div_187p5m", "osc" };
static const char * const clk_uart2_parents[] = { "clk_div_187p5m", "osc" };
static const char * const clk_uart3_parents[] = { "clk_div_187p5m", "osc" };
static const char * const clk_uart4_parents[] = { "clk_div_187p5m", "osc" };
static const char * const clk_axi_vip_parents[] = { "clk_div_1_axi_vip", "clk_div_0_axi_vip" };
static const char * const clk_src_vip_sys_0_parents[] = { "clk_div_1_src_vip_sys_0", "clk_div_0_src_vip_sys_0" };
static const char * const clk_src_vip_sys_1_parents[] = { "clk_div_1_src_vip_sys_1", "clk_div_0_src_vip_sys_1" };
static const char * const clk_axi_video_codec_parents[] = { "clk_div_1_axi_video_codec", "clk_div_0_axi_video_codec" };
static const char * const clk_vc_src0_parents[] = { "clk_div_1_vc_src0", "clk_div_0_vc_src0" };
static const char * const clk_vc_src1_parents[] = { "clk_div_1_vc_src1", "clk_div_0_vc_src1" };
static const char * const clk_src_vip_sys_2_parents[] = { "clk_div_1_src_vip_sys_2", "clk_div_0_src_vip_sys_2" };
static const char * const clk_pwm_src_parents[] = { "clk_div_1_pwm_src", "clk_div_0_pwm_src" };
static const char * const clk_axi_video_codec_1_parents[] = { "clk_mipimpll", "clk_cam1pll" };

static const struct cv1835_mux_clock cv1835_mux_clks[] = {
	{ CV1835_CLK_MUX_AXI6, "clk_mux_axi6", clk_axi6_parents, 2,
	   CV1835_CLK_SELECT, 14, 0 },
	{ CV1835_CLK_MUX_PWM_SRC, "clk_mux_pwm_src", clk_pwm_src_parents, 2,
	   CV1835_CLK_SELECT, 22, 0 },
	{ CV1835_CLK_MUX_AXI_VIDEO_CODEC, "clk_mux_axi_video_codec", clk_axi_video_codec_parents, 2,
	   CV1835_CLK_SELECT, 18, 0 },
	// ECO bit for clk_axi_video_codec divider1
	{ CV1835_CLK_MUX_AXI_VIDEO_CODEC_1, "clk_mux_axi_video_codec_1", clk_axi_video_codec_1_parents, 2,
	   CV1835_CLK_SELECT, 31, 0 },
};

/*
 * Clocks marked as CRITICAL are needed for the proper functioning
 * of the SoC.
 */
static struct cv1835_div_hw_clock cv1835_div_clks[] = {
	CLK_DIV(CV1835_CLK_DIV_0_A53, "clk_div_0_a53", "clk_mpll",
		CV1835_CLK_A53_DIV0, 16, 5, 1, NULL, CLK_SET_RATE_PARENT),
	CLK_DIV(CV1835_CLK_DIV_1_A53, "clk_div_1_a53", "clk_fpll",
		CV1835_CLK_A53_DIV1, 16, 5, 2, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_CPU_AXI0, "clk_div_0_cpu_axi0", "clk_mipimpll",
		CV1835_CLK_CPU_AXI0_DIV0, 16, 5, 2, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_CPU_AXI0, "clk_div_1_cpu_axi0", "clk_fpll",
		CV1835_CLK_CPU_AXI0_DIV1, 16, 5, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_TPU_AXI, "clk_div_0_tpu_axi", "clk_tpll",
		CV1835_CLK_TPU_AXI_DIV0, 16, 5, 1, NULL, CLK_SET_RATE_PARENT),
	CLK_DIV(CV1835_CLK_DIV_1_TPU_AXI, "clk_div_1_tpu_axi", "clk_fpll",
		CV1835_CLK_TPU_AXI_DIV1, 16, 5, 2, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_TPU_FAB, "clk_div_0_tpu_fab", "clk_mipimpll",
		CV1835_CLK_TPU_FAB_DIV0, 16, 5, 2, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_TPU_FAB, "clk_div_1_tpu_fab", "clk_fpll",
		CV1835_CLK_TPU_FAB_DIV1, 16, 5, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_EMMC, "clk_div_0_emmc", "clk_fpll",
		CV1835_CLK_EMMC_DIV0, 16, 5, 15, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_EMMC, "clk_div_1_emmc", "clk_disppll",
		CV1835_CLK_EMMC_DIV1, 16, 5, 12, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_SD0, "clk_div_0_sd0", "clk_fpll",
		CV1835_CLK_SD0_DIV0, 16, 5, 15, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_SD0, "clk_div_1_sd0", "clk_disppll",
		CV1835_CLK_SD0_DIV1, 16, 5, 12, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_SD1, "clk_div_0_sd1", "clk_fpll",
		CV1835_CLK_SD1_DIV0, 16, 5, 15, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_SD1, "clk_div_1_sd1", "clk_disppll",
		CV1835_CLK_SD1_DIV1, 16, 5, 12, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_AXI4, "clk_div_axi4", "clk_fpll",
		CV1835_CLK_AXI4_DIV, 16, 5, 5, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_AXI6, "clk_div_0_axi6", "clk_fpll",
		CV1835_CLK_AXI6_DIV0, 16, 5, 15, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_AXI6, "clk_div_1_axi6", "clk_mpll",
		CV1835_CLK_AXI6_DIV1, 16, 5, 10, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_AXI_VIP, "clk_div_0_axi_vip", "clk_mipimpll",
		CV1835_CLK_AXI_VIP_DIV0, 16, 5, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_AXI_VIP, "clk_div_1_axi_vip", "clk_fpll",
		CV1835_CLK_AXI_VIP_DIV1, 16, 5, 4, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_SRC_VIP_SYS_0, "clk_div_0_src_vip_sys_0", "clk_fpll",
		CV1835_CLK_SRC_VIP_SYS_0_DIV0, 16, 5, 6, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_SRC_VIP_SYS_0, "clk_div_1_src_vip_sys_0", "clk_mipimpll",
		CV1835_CLK_SRC_VIP_SYS_0_DIV1, 16, 5, 5, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_SRC_VIP_SYS_1, "clk_div_0_src_vip_sys_1", "clk_fpll",
		CV1835_CLK_SRC_VIP_SYS_1_DIV0, 16, 5, 5, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_SRC_VIP_SYS_1, "clk_div_1_src_vip_sys_1", "clk_mipimpll",
		CV1835_CLK_SRC_VIP_SYS_1_DIV1, 16, 5, 4, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_AXI_VIDEO_CODEC, "clk_div_0_axi_video_codec", "clk_fpll",
		CV1835_CLK_AXI_VIDEO_CODEC_DIV0, 16, 4, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_AXI_VIDEO_CODEC, "clk_div_1_axi_video_codec", "clk_mux_axi_video_codec_1",
		CV1835_CLK_AXI_VIDEO_CODEC_DIV1, 16, 4, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_VC_SRC0, "clk_div_0_vc_src0", "clk_a0pll",
		CV1835_CLK_VC_SRC0_DIV0, 16, 4, 1, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_VC_SRC0, "clk_div_1_vc_src0", "clk_fpll",
		CV1835_CLK_VC_SRC0_DIV1, 16, 4, 4, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1M, "clk_div_1m", "osc",
		CV1835_CLK_1M_DIV, 16, 6, 25, NULL, CLK_IS_CRITICAL),
	CLK_DIV(CV1835_CLK_DIV_0_VC_SRC1, "clk_div_0_vc_src1", "clk_cam1pll",
		CV1835_CLK_VC_SRC1_DIV0, 16, 4, 2, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_VC_SRC1, "clk_div_1_vc_src1", "clk_fpll",
		CV1835_CLK_VC_SRC1_DIV1, 16, 4, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_SRC_VIP_SYS_2, "clk_div_0_src_vip_sys_2", "clk_mipimpll",
		CV1835_CLK_SRC_VIP_SYS_2_DIV0, 16, 5, 3, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_SRC_VIP_SYS_2, "clk_div_1_src_vip_sys_2", "clk_fpll",
		CV1835_CLK_SRC_VIP_SYS_2_DIV1, 16, 5, 4, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_0_PWM_SRC, "clk_div_0_pwm_src", "clk_disppll",
		CV1835_CLK_PWM_SRC_DIV0, 16, 6, 8, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_1_PWM_SRC, "clk_div_1_pwm_src", "clk_cam0pll",
		CV1835_CLK_PWM_SRC_DIV1, 16, 6, 8, NULL, 0),
	CLK_DIV(CV1835_CLK_DIV_187P5M, "clk_div_187p5m", "clk_fpll",
	  CV1835_CLK_187P5M_DIV, 16, 5, 8, NULL, 0),
};

/*
 * Clocks marked as CRITICAL are all needed for the proper functioning
 * of the SoC.
 */
static struct cv1835_composite_clock cv1835_composite_clks[] = {
	GATE_MUX(CV1835_CLK_A53, "clk_a53", clk_a53_parents,
		CV1835_CLK_ENABLE0, 0, CV1835_CLK_SELECT, 0, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_CPU_AXI0, "clk_cpu_axi0", clk_cpu_axi0_parents,
		CV1835_CLK_ENABLE0, 1, CV1835_CLK_SELECT, 1, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_TPU_AXI, "clk_tpu_axi", clk_tpu_axi_parents,
		CV1835_CLK_ENABLE0, 4, CV1835_CLK_SELECT, 2, 0),
	GATE_MUX(CV1835_CLK_TPU_FAB, "clk_tpu_fab", clk_tpu_fab_parents,
		CV1835_CLK_ENABLE0, 5, CV1835_CLK_SELECT, 3, 0),
	GATE_MUX(CV1835_CLK_DEBUG, "clk_debug", clk_debug_parents,
		CV1835_CLK_ENABLE0, 13, CV1835_CLK_SELECT, 4, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_EMMC, "clk_emmc", clk_emmc_parents,
		CV1835_CLK_ENABLE0, 16, CV1835_CLK_SELECT, 5, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_SD0, "clk_sd0", clk_sd0_parents,
		CV1835_CLK_ENABLE0, 19, CV1835_CLK_SELECT, 6, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_SD1, "clk_sd1", clk_sd1_parents,
		CV1835_CLK_ENABLE0, 22, CV1835_CLK_SELECT, 7, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_APB_PWM, "clk_apb_pwm", clk_apb_pwm_parents,
		CV1835_CLK_ENABLE1, 8, CV1835_CLK_SELECT, 8, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_UART0, "clk_uart0", clk_uart0_parents,
		CV1835_CLK_ENABLE1, 14, CV1835_CLK_SELECT, 9, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_UART1, "clk_uart1", clk_uart1_parents,
		CV1835_CLK_ENABLE1, 16, CV1835_CLK_SELECT, 10, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_UART2, "clk_uart2", clk_uart2_parents,
		CV1835_CLK_ENABLE1, 18, CV1835_CLK_SELECT, 11, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_UART3, "clk_uart3", clk_uart3_parents,
		CV1835_CLK_ENABLE1, 20, CV1835_CLK_SELECT, 12, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_UART4, "clk_uart4", clk_uart4_parents,
		CV1835_CLK_ENABLE1, 22, CV1835_CLK_SELECT, 13, CLK_IS_CRITICAL),
	GATE_MUX(CV1835_CLK_AXI_VIP, "clk_axi_vip", clk_axi_vip_parents,
		CV1835_CLK_ENABLE2, 4, CV1835_CLK_SELECT, 15, 0),
	GATE_MUX(CV1835_CLK_SRC_VIP_SYS_0, "clk_src_vip_sys_0", clk_src_vip_sys_0_parents,
		CV1835_CLK_ENABLE2, 5, CV1835_CLK_SELECT, 16, 0),
	GATE_MUX(CV1835_CLK_SRC_VIP_SYS_1, "clk_src_vip_sys_1", clk_src_vip_sys_1_parents,
		CV1835_CLK_ENABLE2, 6, CV1835_CLK_SELECT, 17, 0),
	GATE_MUX(CV1835_CLK_VC_SRC0, "clk_vc_src0", clk_vc_src0_parents,
		CV1835_CLK_ENABLE2, 9, CV1835_CLK_SELECT, 19, 0),
	GATE_MUX(CV1835_CLK_VC_SRC1, "clk_vc_src1", clk_vc_src1_parents,
		CV1835_CLK_ENABLE3, 28, CV1835_CLK_SELECT, 20, 0),
	GATE_MUX(CV1835_CLK_SRC_VIP_SYS_2, "clk_src_vip_sys_2", clk_src_vip_sys_2_parents,
		CV1835_CLK_ENABLE3, 29, CV1835_CLK_SELECT, 21, 0),
	GATE_DIV(CV1835_CLK_CPU_GIC, "clk_cpu_gic", "clk_fpll",
		CV1835_CLK_ENABLE0, 2, CV1835_CLK_CPU_GIC_DIV, 16, 5, 5, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_100K_EMMC, "clk_100k_emmc", "clk_div_1m",
		CV1835_CLK_ENABLE0, 17, CV1835_CLK_100K_EMMC_DIV, 16, 8, 10, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_100K_SD0, "clk_100k_sd0", "clk_div_1m",
		CV1835_CLK_ENABLE0, 20, CV1835_CLK_100K_SD0_DIV, 16, 8, 10, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_100K_SD1, "clk_100k_sd1", "clk_div_1m",
		CV1835_CLK_ENABLE0, 23, CV1835_CLK_100K_SD1_DIV, 16, 8, 10, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_SPI_NAND, "clk_spi_nand", "clk_fpll",
		CV1835_CLK_ENABLE0, 24, CV1835_CLK_SPI_NAND_DIV, 16, 5, 8, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_500M_ETH0, "clk_500m_eth0", "clk_fpll",
		CV1835_CLK_ENABLE0, 25, CV1835_CLK_500M_ETH0_DIV, 16, 5, 3, NULL, 0),
	GATE_DIV(CV1835_CLK_500M_ETH1, "clk_500m_eth1", "clk_fpll",
		CV1835_CLK_ENABLE0, 27, CV1835_CLK_500M_ETH1_DIV, 16, 5, 3, NULL, 0),
	GATE_DIV(CV1835_CLK_GPIO_DB, "clk_gpio_db", "clk_div_1m",
		CV1835_CLK_ENABLE0, 31, CV1835_CLK_GPIO_DB_DIV, 16, 16, 10, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_SDMA_AUD0, "clk_sdma_aud0", "clk_a0pll",
		CV1835_CLK_ENABLE1, 2, CV1835_CLK_SDMA_AUD0_DIV, 16, 8, 18, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_SDMA_AUD1, "clk_sdma_aud1", "clk_a0pll",
		CV1835_CLK_ENABLE1, 3, CV1835_CLK_SDMA_AUD1_DIV, 16, 8, 18, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_SDMA_AUD2, "clk_sdma_aud2", "clk_a0pll",
		CV1835_CLK_ENABLE1, 4, CV1835_CLK_SDMA_AUD2_DIV, 16, 8, 18, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_SDMA_AUD3, "clk_sdma_aud3", "clk_a0pll",
		CV1835_CLK_ENABLE1, 5, CV1835_CLK_SDMA_AUD3_DIV, 16, 8, 18, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_125M_USB, "clk_125m_usb", "clk_fpll",
		CV1835_CLK_ENABLE1, 30, CV1835_CLK_125M_USB_DIV, 16, 5, 12, NULL, 0),
	GATE_DIV(CV1835_CLK_33K_USB, "clk_33k_usb", "clk_div_1m",
		CV1835_CLK_ENABLE1, 31, CV1835_CLK_33K_USB_DIV, 16, 9, 1, NULL, 0),
	GATE_DIV(CV1835_CLK_12M_USB, "clk_12m_usb", "clk_fpll",
		CV1835_CLK_ENABLE2, 0, CV1835_CLK_12M_USB_DIV, 16, 7, 125, NULL, 0),
	GATE_DIV(CV1835_CLK_DSI_ESC, "clk_dsi_esc", "clk_mux_axi6",
		CV1835_CLK_ENABLE2, 3, CV1835_CLK_DSI_ESC_DIV, 16, 5, 5, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_DISP_SRC_VIP, "clk_disp_src_vip", "clk_disppll",
		CV1835_CLK_ENABLE2, 7, CV1835_CLK_DISP_SRC_VIP_DIV, 16, 6, 8, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_CAM0, "clk_cam0", "clk_cam0pll",
		CV1835_CLK_ENABLE2, 16, CV1835_CLK_CAM0_DIV, 16, 6, 14, NULL, 0),
	GATE_DIV(CV1835_CLK_CAM1, "clk_cam1", "clk_cam1pll",
		CV1835_CLK_ENABLE2, 17, CV1835_CLK_CAM1_DIV, 16, 6, 14, NULL, 0),
	GATE_DIV(CV1835_CLK_SPI, "clk_spi", "clk_fpll",
		CV1835_CLK_ENABLE3, 6, CV1835_CLK_SPI_DIV, 16, 6, 8, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_I2C, "clk_i2c", "clk_mux_axi6",
		CV1835_CLK_ENABLE3, 7, CV1835_CLK_I2C_DIV, 16, 4, 1, NULL, 0),
	GATE_DIV(CV1835_CLK_AUDSRC, "clk_audsrc", "clk_a0pll",
		CV1835_CLK_ENABLE4, 1, CV1835_CLK_AUDSRC_DIV, 16, 8, 18, NULL, CLK_IS_CRITICAL),
	GATE_DIV(CV1835_CLK_VC_SRC2, "clk_vc_src2", "clk_fpll",
		CV1835_CLK_ENABLE4, 3, CV1835_CLK_VC_SRC2_DIV, 16, 4, 3, NULL, 0),
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

static unsigned long cv1835_pll_rate_calc(u32 regval, unsigned long parent_rate)
{
	u64 numerator;
	u32 predivsel, postdivsel, divsel;
	u32 denominator;

	predivsel = regval & 0x7f;
	postdivsel = (regval >> 8) & 0x7f;
	divsel = (regval >> 17) & 0x7f;

	numerator = parent_rate * divsel;
	denominator = predivsel * postdivsel;
	do_div(numerator, denominator);

	return (unsigned long)numerator;
}

static unsigned long cv1835_g6_pll_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	struct cv1835_pll_hw_clock *pll_hw = to_cv1835_pll_clk(hw);
	unsigned long rate;
	u32 regval;

	regval = readl(pll_hw->base + pll_hw->pll.reg_csr);
	rate = cv1835_pll_rate_calc(regval, parent_rate);

	return rate;
}

static long cv1835_g6_pll_round_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long *prate)
{
	return rate;
}

static int cv1835_g6_pll_calc_csr(unsigned long parent_rate, unsigned long rate, u32 *csr)
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

static int cv1835_g6_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	struct cv1835_pll_hw_clock *pll_hw = to_cv1835_pll_clk(hw);
	unsigned long flags = 0;
	int ret;
	u32 reg_g6_pll_status;
	u32 regval_csr;
	u32 regval_g6_pll_status;

	reg_g6_pll_status = (pll_hw->pll.reg_csr & ~PLL_STATUS_MASK) + PLL_STATUS_OFFSET;

	if (pll_hw->lock)
		spin_lock_irqsave(pll_hw->lock, flags);
	else
		__acquire(pll_hw->lock);

	/* calculate csr register */
	ret = cv1835_g6_pll_calc_csr(parent_rate, rate, &regval_csr);
	if (ret < 0)
		return ret;

	/* csr register */
	writel(regval_csr, pll_hw->base + pll_hw->pll.reg_csr);

	/* wait for pll setting updated */
	do {
		regval_g6_pll_status = readl(pll_hw->base + reg_g6_pll_status);
	} while (regval_g6_pll_status & 0x07);

	if (pll_hw->lock)
		spin_unlock_irqrestore(pll_hw->lock, flags);
	else
		__release(pll_hw->lock);

	return 0;
}

static const struct clk_ops cv1835_g6_pll_ops = {
	.recalc_rate = cv1835_g6_pll_recalc_rate,
	.round_rate = cv1835_g6_pll_round_rate,
	.set_rate = cv1835_g6_pll_set_rate,
};

static unsigned long cv1835_g2_pll_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	struct cv1835_pll_hw_clock *pll_hw = to_cv1835_pll_clk(hw);
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
	do_div(numerator, denominator);

	clk_ref = numerator;

rate_calc:
	rate = cv1835_pll_rate_calc(regval_csr, clk_ref);

	return rate;
}

static const struct {
	unsigned long rate;
	u32 csr;
	u32 ssc_set;
} g2_pll_rate_lut[] = {
	{.rate = 48000000, .csr = 0x00129201, .ssc_set = 419430400},
	{.rate = 406425600, .csr = 0x01108201, .ssc_set = 396287226},
	{.rate = 417792000, .csr = 0x01108201, .ssc_set = 385505882},
	{.rate = 768000000, .csr = 0x00108101, .ssc_set = 419430400},
	{.rate = 832000000, .csr = 0x00108101, .ssc_set = 387166523},
	{.rate = 1032000000, .csr = 0x00148101, .ssc_set = 390167814},
	{.rate = 1050000000, .csr = 0x00168101, .ssc_set = 421827145},
	{.rate = 1056000000, .csr = 0x00208100, .ssc_set = 412977625},
	{.rate = 1125000000, .csr = 0x00168101, .ssc_set = 393705325},
	{.rate = 1188000000, .csr = 0x00188101, .ssc_set = 406720388},
};

static int cv1835_g2_pll_get_setting_from_lut(unsigned long rate, u32 *csr,
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

static long cv1835_g2_pll_round_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long *prate)
{
	return rate;
}

static int cv1835_g2_pll_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	struct cv1835_pll_hw_clock *pll_hw = to_cv1835_pll_clk(hw);
	unsigned long flags = 0;
	int ret;
	u32 reg_ssc_set;
	u32 reg_ssc_ctrl;
	u32 reg_g2_pll_status;
	u32 regval_csr;
	u32 regval_ssc_set;
	u32 regval_ssc_ctrl;
	u32 regval_g2_pll_status;

	/* pll without synthesizer */
	if (pll_hw->pll.reg_ssc == 0)
		return -ENOENT;

	ret = cv1835_g2_pll_get_setting_from_lut(rate, &regval_csr,
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

	/* wait for pll setting updated */
	do {
		regval_g2_pll_status = readl(pll_hw->base + reg_g2_pll_status);
	} while (regval_g2_pll_status & 0x1F);

	if (pll_hw->lock)
		spin_unlock_irqrestore(pll_hw->lock, flags);
	else
		__release(pll_hw->lock);

	return 0;
}

static const struct clk_ops cv1835_g2_pll_ops = {
	.recalc_rate = cv1835_g2_pll_recalc_rate,
	.round_rate = cv1835_g2_pll_round_rate,
	.set_rate = cv1835_g2_pll_set_rate,
};

static struct clk_hw *cv1835_clk_register_pll(struct cv1835_pll_hw_clock *pll_clk,
					      void __iomem *sys_base)
{
	struct clk_hw *hw;
	struct clk_init_data init;
	int err;

	pll_clk->lock = &cv1835_clk_lock;
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

static void cv1835_clk_unregister_pll(struct clk_hw *hw)
{
	struct cv1835_pll_hw_clock *pll_hw = to_cv1835_pll_clk(hw);

	clk_hw_unregister(hw);
	kfree(pll_hw);
}

static int cv1835_clk_register_plls(struct cv1835_pll_hw_clock *clks,
				    int num_clks,
				    struct cv1835_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *pll_base = data->base;
	int i;

	for (i = 0; i < num_clks; i++) {
		struct cv1835_pll_hw_clock *cv1835_clk = &clks[i];

		hw = cv1835_clk_register_pll(cv1835_clk, pll_base);
		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, cv1835_clk->pll.name);
			goto err_clk;
		}

		data->hw_data.hws[clks[i].pll.id] = hw;

		clk_hw_register_clkdev(hw, cv1835_clk->pll.name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		cv1835_clk_unregister_pll(data->hw_data.hws[clks[i].pll.id]);

	return PTR_ERR(hw);
}

static int cv1835_clk_register_mux(const struct cv1835_mux_clock *clks,
				   int num_clks,
				   struct cv1835_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *sys_base = data->base;
	int i;

	for (i = 0; i < num_clks; i++) {
		hw = clk_hw_register_mux(NULL, clks[i].name,
					 clks[i].parents,
					 clks[i].num_parents,
					 clks[i].flags | cvi_clk_flags,
					 sys_base + clks[i].reg,
					 clks[i].shift, 1, 0,
					 &cv1835_clk_lock);
		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, clks[i].name);
			goto err_clk;
		}

		data->hw_data.hws[clks[i].id] = hw;

		clk_hw_register_clkdev(hw, clks[i].name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		clk_hw_unregister_mux(data->hw_data.hws[clks[i].id]);

	return PTR_ERR(hw);
}

static unsigned long cv1835_clk_div_recalc_rate(struct clk_hw *hw,
						unsigned long parent_rate)
{
	struct cv1835_div_hw_clock *div_hw = to_cv1835_div_clk(hw);
	struct cv1835_div_clock *div = &div_hw->div;
	void __iomem *reg_addr = div_hw->base + div->reg;
	unsigned int val;
	unsigned long rate;

	if (!(readl(reg_addr) & BIT(3))) {
		val = div->initval;
	} else {
		val = readl(reg_addr) >> div->shift;
		val &= div_mask(div->width);
	}

	rate = divider_recalc_rate(hw, parent_rate, val, div->table,
				   div->div_flags, div->width);

	return rate;
}

static long cv1835_clk_div_round_rate(struct clk_hw *hw, unsigned long rate,
				      unsigned long *prate)
{
	struct cv1835_div_hw_clock *div_hw = to_cv1835_div_clk(hw);
	struct cv1835_div_clock *div = &div_hw->div;

	return divider_round_rate(hw, rate, prate, div->table,
				  div->width, div->div_flags);
}


static int cv1835_clk_div_determine_rate(struct clk_hw *hw,
					 struct clk_rate_request *req)
{
	req->rate = cv1835_clk_div_round_rate(hw, min(req->rate, req->max_rate),
					      &req->best_parent_rate);
	return 0;
}

static int cv1835_clk_div_set_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long parent_rate)
{
	struct cv1835_div_hw_clock *div_hw = to_cv1835_div_clk(hw);
	struct cv1835_div_clock *div = &div_hw->div;
	void __iomem *reg_addr = div_hw->base + div->reg;
	unsigned long flags = 0;
	int value;
	u32 val;

	value = divider_get_val(rate, parent_rate, div->table,
				div->width, div_hw->div.div_flags);
	if (value < 0)
		return value;

	if (div_hw->lock)
		spin_lock_irqsave(div_hw->lock, flags);
	else
		__acquire(div_hw->lock);

	val = readl(reg_addr);
	val &= ~(div_mask(div->width) << div_hw->div.shift);
	val |= (u32)value << div->shift;
	val |= BIT(3);
	writel(val, reg_addr);

	if (div_hw->lock)
		spin_unlock_irqrestore(div_hw->lock, flags);
	else
		__release(div_hw->lock);

	return 0;
}

static const struct clk_ops cv1835_clk_div_ops = {
	.recalc_rate = cv1835_clk_div_recalc_rate,
	.round_rate = cv1835_clk_div_round_rate,
	.determine_rate = cv1835_clk_div_determine_rate,
	.set_rate = cv1835_clk_div_set_rate,
};

static struct clk_hw *cv1835_clk_register_div(struct cv1835_div_hw_clock *div_clk,
					      void __iomem *sys_base)
{
	struct clk_hw *hw;
	struct clk_init_data init;
	int err;

	div_clk->div.div_flags = CLK_DIVIDER_ONE_BASED | CLK_DIVIDER_ALLOW_ZERO |
				 CLK_DIVIDER_ROUND_CLOSEST;
	div_clk->base = sys_base;
	div_clk->lock = &cv1835_clk_lock;

	if (cvi_clk_flags) {
		/* copy clk_init_data for modification */
		memcpy(&init, div_clk->hw.init, sizeof(init));

		init.flags |= cvi_clk_flags;
		div_clk->hw.init = &init;
	}

	hw = &div_clk->hw;
	err = clk_hw_register(NULL, hw);
	if (err)
		return ERR_PTR(err);

	return hw;
}

static void cv1835_clk_unregister_div(struct clk_hw *hw)
{
	struct cv1835_div_hw_clock *div_hw = to_cv1835_div_clk(hw);

	clk_hw_unregister(hw);
	kfree(div_hw);
}

static int cv1835_clk_register_divs(struct cv1835_div_hw_clock *clks,
				    int num_clks,
				    struct cv1835_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *sys_base = data->base;
	unsigned int i;

	for (i = 0; i < num_clks; i++) {
		struct cv1835_div_hw_clock *cv1835_clk = &clks[i];

		hw = cv1835_clk_register_div(cv1835_clk, sys_base);
		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, cv1835_clk->div.name);
			goto err_clk;
		}

		data->hw_data.hws[clks[i].div.id] = hw;

		clk_hw_register_clkdev(hw, cv1835_clk->div.name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		cv1835_clk_unregister_div(data->hw_data.hws[clks[i].div.id]);

	return PTR_ERR(hw);
}

static void cv1835_clk_gate_endisable(struct clk_hw *hw, int enable)
{
	struct cv1835_gate_hw_clock *gate_hw = to_cv1835_gate_clk(hw);
	struct cv1835_gate_clock *gate = &gate_hw->gate;
	void __iomem *reg_addr = gate_hw->base + gate->reg;
	unsigned long flags = 0;
	u32 reg;

	if (gate_hw->lock)
		spin_lock_irqsave(gate_hw->lock, flags);
	else
		__acquire(gate_hw->lock);

	reg = readl(reg_addr);

	if (enable)
		reg |= BIT(gate->shift);
	else
		reg &= ~BIT(gate->shift);

	writel(reg, reg_addr);

	if (gate_hw->lock)
		spin_unlock_irqrestore(gate_hw->lock, flags);
	else
		__release(gate_hw->lock);
}

static int cv1835_clk_gate_enable(struct clk_hw *hw)
{
	cv1835_clk_gate_endisable(hw, 1);

	return 0;
}

static void cv1835_clk_gate_disable(struct clk_hw *hw)
{
	struct cv1835_gate_hw_clock *gate_hw = to_cv1835_gate_clk(hw);
	struct cv1835_gate_clock *gate = &gate_hw->gate;

	/* bypass clk disable if need to wait bus idle*/
	if (gate->idle_reg)
		return;

	cv1835_clk_gate_endisable(hw, 0);
}

static int cv1835_clk_gate_is_enabled(struct clk_hw *hw)
{
	u32 reg;
	struct cv1835_gate_hw_clock *gate_hw = to_cv1835_gate_clk(hw);
	struct cv1835_gate_clock *gate = &gate_hw->gate;
	void __iomem *reg_addr = gate_hw->base + gate->reg;

	reg = readl(reg_addr);

	reg &= BIT(gate->shift);

	return reg ? 1 : 0;
}

static void cv1835_clk_gate_unprepare(struct clk_hw *hw)
{
	struct cv1835_gate_hw_clock *gate_hw = to_cv1835_gate_clk(hw);
	struct cv1835_gate_clock *gate = &gate_hw->gate;
	void __iomem *idle_reg;
	u32 idle_state;
	ktime_t timeout;

	/* wait for axi bus idle */
	if (gate->idle_reg) {
		idle_reg = ioremap(gate->idle_reg, 4);
		if (WARN_ON(!idle_reg))
			return;

		timeout = ktime_add_ms(ktime_get(), CV1835_BUS_IDLE_TIMEOUT_MS);
		while ((idle_state = readl(idle_reg)) != gate->idle_state) {
			if (ktime_after(ktime_get(), timeout)) {
				pr_err("%s: wait for idle timeout\n", clk_hw_get_name(hw));
				break;
			}
			cpu_relax();
		}

		iounmap(idle_reg);

		if (idle_state == gate->idle_state) {
			cv1835_clk_gate_endisable(hw, 0);
		}
	}
}

static const struct clk_ops cv1835_clk_gate_ops = {
	.enable = cv1835_clk_gate_enable,
	.disable = cv1835_clk_gate_disable,
	.is_enabled = cv1835_clk_gate_is_enabled,
	.unprepare = cv1835_clk_gate_unprepare,
};

static struct clk_hw *cv1835_clk_register_gate(struct cv1835_gate_hw_clock *gate_clk,
					       void __iomem *sys_base)
{

	struct clk_hw *hw;
	struct clk_init_data init;
	int err;

	gate_clk->gate.gate_flags = 0;
	gate_clk->base = sys_base;
	gate_clk->lock = &cv1835_clk_lock;

	if (cvi_clk_flags) {
		/* copy clk_init_data for modification */
		memcpy(&init, gate_clk->hw.init, sizeof(init));

		init.flags |= cvi_clk_flags;
		gate_clk->hw.init = &init;
	}

	hw = &gate_clk->hw;
	err = clk_hw_register(NULL, hw);
	if (err) {
		return ERR_PTR(err);
	}

	return hw;
}

static void cv1835_clk_unregister_gate(struct clk_hw *hw)
{
	struct cv1835_gate_hw_clock *gate_hw = to_cv1835_gate_clk(hw);

	clk_hw_unregister(hw);
	kfree(gate_hw);
}

static int cv1835_clk_register_gates(struct cv1835_gate_hw_clock *clks,
				     int num_clks,
				     struct cv1835_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *sys_base = data->base;
	unsigned int i;

	for (i = 0; i < num_clks; i++) {
		struct cv1835_gate_hw_clock *cv1835_clk = &clks[i];

		hw = cv1835_clk_register_gate(cv1835_clk, sys_base);

		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, cv1835_clk->gate.name);
			goto err_clk;
		}

		data->hw_data.hws[clks[i].gate.id] = hw;
		clk_hw_register_clkdev(hw, cv1835_clk->gate.name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		cv1835_clk_unregister_gate(data->hw_data.hws[clks[i].gate.id]);

	return PTR_ERR(hw);

}

static struct clk_hw *cv1835_clk_register_composite(struct cv1835_composite_clock *clks,
						    void __iomem *sys_base)
{
	struct clk_hw *hw;
	struct clk_mux *mux = NULL;
	struct cv1835_gate_hw_clock *gate_hws = NULL;
	struct cv1835_div_hw_clock *div_hws = NULL;
	struct clk_hw *mux_hw = NULL, *gate_hw = NULL, *div_hw = NULL;
	const struct clk_ops *mux_ops = NULL, *gate_ops = NULL, *div_ops = NULL;
	const char * const *parent_names;
	const char *parent;
	int num_parents;
	int ret;

	if (clks->mux_shift >= 0) {
		mux = kzalloc(sizeof(*mux), GFP_KERNEL);
		if (!mux)
			return ERR_PTR(-ENOMEM);

		mux->reg = sys_base + clks->mux_reg;
		mux->mask = 1;
		mux->shift = clks->mux_shift;
		mux_hw = &mux->hw;
		mux_ops = &clk_mux_ops;
		mux->lock = &cv1835_clk_lock;

		parent_names = clks->parents;
		num_parents = clks->num_parents;
	} else {
		parent = clks->parent;
		parent_names = &parent;
		num_parents = 1;
	}

	if (clks->gate_shift >= 0) {
		gate_hws = kzalloc(sizeof(*gate_hws), GFP_KERNEL);
		if (!gate_hws) {
			ret = -ENOMEM;
			goto err_out;
		}

		gate_hws->base = sys_base;
		gate_hws->gate.reg = clks->gate_reg;
		gate_hws->gate.shift = clks->gate_shift;
		gate_hws->gate.gate_flags = 0;
		gate_hws->lock = &cv1835_clk_lock;

		gate_hw = &gate_hws->hw;
		gate_ops = &cv1835_clk_gate_ops;
	}

	if (clks->div_shift >= 0) {
		div_hws = kzalloc(sizeof(*div_hws), GFP_KERNEL);
		if (!div_hws) {
			ret = -ENOMEM;
			goto err_out;
		}

		div_hws->base = sys_base;
		div_hws->div.reg = clks->div_reg;
		div_hws->div.shift = clks->div_shift;
		div_hws->div.width = clks->div_width;
		div_hws->div.table = clks->table;
		div_hws->div.initval = clks->div_initval;
		div_hws->lock = &cv1835_clk_lock;
		div_hws->div.div_flags = CLK_DIVIDER_ONE_BASED |
					 CLK_DIVIDER_ROUND_CLOSEST |
					 CLK_DIVIDER_ALLOW_ZERO;

		div_hw = &div_hws->hw;
		div_ops = &cv1835_clk_div_ops;
	}

	hw = clk_hw_register_composite(NULL, clks->name, parent_names,
				       num_parents, mux_hw, mux_ops, div_hw,
				       div_ops, gate_hw, gate_ops,
				       clks->flags | cvi_clk_flags);

	if (IS_ERR(hw)) {
		ret = PTR_ERR(hw);
		goto err_out;
	}

	return hw;

err_out:
	kfree(div_hws);
	kfree(gate_hws);
	kfree(mux);

	return ERR_PTR(ret);
}

static void cv1835_clk_unregister_composite(struct clk_hw *hw)
{
	struct clk_composite *composite;

	composite = to_clk_composite(hw);

	clk_hw_unregister(hw);
	kfree(composite);
}

static int cv1835_clk_register_composites(struct cv1835_composite_clock *clks,
					  int num_clks,
					  struct cv1835_clock_data *data)
{
	struct clk_hw *hw;
	void __iomem *sys_base = data->base;
	int i;

	for (i = 0; i < num_clks; i++) {
		struct cv1835_composite_clock *cv1835_clk = &clks[i];

		hw = cv1835_clk_register_composite(cv1835_clk, sys_base);
		if (IS_ERR(hw)) {
			pr_err("%s: failed to register clock %s\n",
			       __func__, cv1835_clk->name);
			goto err_clk;
		}

		data->hw_data.hws[clks[i].id] = hw;

		clk_hw_register_clkdev(hw, cv1835_clk->name, NULL);
	}

	return 0;

err_clk:
	while (i--)
		cv1835_clk_unregister_composite(data->hw_data.hws[clks[i].id]);

	return PTR_ERR(hw);
}

static const struct of_device_id cvi_clk_match_ids_tables[] = {
	{
		.compatible = "cvitek,cv1835-clk",
	},
	{}
};

static void __init cvi_clk_init(struct device_node *node)
{
	struct cv1835_clock_data *clk_data;
	static void __iomem *base;
	int num_clks;
	int i;
	int ret = 0;

	num_clks = ARRAY_SIZE(cv1835_pll_clks) +
		   ARRAY_SIZE(cv1835_div_clks) +
		   ARRAY_SIZE(cv1835_mux_clks) +
		   ARRAY_SIZE(cv1835_composite_clks) +
		   ARRAY_SIZE(cv1835_gate_clks);

	clk_data = kzalloc(sizeof(struct cv1835_clock_data) +
			   sizeof(struct clk_hw) * num_clks,
			   GFP_KERNEL);
	if (!clk_data) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < num_clks; i++)
		clk_data->hw_data.hws[i] = ERR_PTR(-ENOENT);

	clk_data->hw_data.num = num_clks;

	clk_data->lock = &cv1835_clk_lock;

	base = of_iomap(node, 0);
	if (!base) {
		pr_err("Failed to map address range for cvitek,cv1835-clk node\n");
		return;
	}

	clk_data->base = base;

	cv1835_clk_register_plls(cv1835_pll_clks,
				 ARRAY_SIZE(cv1835_pll_clks),
				 clk_data);

	cv1835_clk_register_divs(cv1835_div_clks,
				 ARRAY_SIZE(cv1835_div_clks),
				 clk_data);

	cv1835_clk_register_mux(cv1835_mux_clks,
				ARRAY_SIZE(cv1835_mux_clks),
				clk_data);

	cv1835_clk_register_composites(cv1835_composite_clks,
				       ARRAY_SIZE(cv1835_composite_clks),
				       clk_data);

	cv1835_clk_register_gates(cv1835_gate_clks,
				  ARRAY_SIZE(cv1835_gate_clks),
				  clk_data);

	/* register clk-provider */
	ret = of_clk_add_hw_provider(node, of_clk_hw_onecell_get, &clk_data->hw_data);
	if (ret)
		pr_err("Unable to add hw clk provider\n");

	/* force enable clocks */
	clk_prepare_enable(clk_data->hw_data.hws[CV1835_CLK_DSI_MAC_VIP]->clk);
	clk_prepare_enable(clk_data->hw_data.hws[CV1835_CLK_DISP_VIP]->clk);
	clk_prepare_enable(clk_data->hw_data.hws[CV1835_CLK_BT_VIP]->clk);
	clk_prepare_enable(clk_data->hw_data.hws[CV1835_CLK_SC_TOP_VIP]->clk);

#if defined(CONFIG_COMMON_CLK_CV1832_TPU_500M)
	/* switch clk_tpu_axi's parent to div1 */
	ret = clk_set_parent(clk_data->hw_data.hws[CV1835_CLK_TPU_AXI]->clk,
			     clk_data->hw_data.hws[CV1835_CLK_DIV_1_TPU_AXI]->clk);
	if (ret)
		pr_err("Set clk_tpu_axi's parent to div1 failed\n");

	/* Set clk_tpu_axi max rate to 500M */
	ret = clk_set_max_rate(clk_data->hw_data.hws[CV1835_CLK_TPU_AXI]->clk, 500000000);
	if (ret)
		pr_err("Set clk_tpu_axi max rate to 500M failed\n");

	/* Set clk_tpu_axi rate to 500M */
	ret = clk_set_rate(clk_data->hw_data.hws[CV1835_CLK_TPU_AXI]->clk, 500000000);
	if (ret)
		pr_err("Set clk_tpu_axi rate to 500M failed\n");

	/* switch clk_tpu_fab's parent to div1 */
	ret = clk_set_parent(clk_data->hw_data.hws[CV1835_CLK_TPU_FAB]->clk,
			     clk_data->hw_data.hws[CV1835_CLK_DIV_1_TPU_FAB]->clk);
	if (ret)
		pr_err("Set clk_tpu_fab's parent to div1 failed\n");

	/* Set clk_tpu_fab rate to 300M */
	ret = clk_set_rate(clk_data->hw_data.hws[CV1835_CLK_TPU_FAB]->clk, 300000000);
	if (ret)
		pr_err("Set clk_tpu_fab rate to 300M failed\n");
#endif

	if (!ret)
		return;

out:
	pr_err("%s failed error number %d\n", __func__, ret);
}
CLK_OF_DECLARE(cvi_clk, "cvitek,cv1835-clk", cvi_clk_init);
