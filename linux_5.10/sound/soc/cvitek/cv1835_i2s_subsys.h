/* SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _CV1835_I2S_SUBSYS_H_
#define _CV1835_I2S_SUBSYS_H_

#define SUBSYS_I2S0     (0x1 << 0)
#define SUBSYS_I2S1     (0x1 << 1)
#define SUBSYS_I2S2     (0x1 << 2)
#define SUBSYS_I2S3     (0x1 << 3)

#define SCLK_IN_SEL				0x00
#define FS_IN_SEL				0x04
#define SDI_IN_SEL				0x08
#define SDO_OUT_SEL				0x0C
#define MULTI_SYNC				0x20
#define BCLK_OEN_SEL			0x30
#define BCLK_OUT_CTRL			0x34
#define AUDIO_PDM_CTRL			0x40
#define AUDIO_PHY_BYPASS1		0x50
#define AUDIO_PHY_BYPASS2		0x54
#define SYS_CLK_CTRL			0x70
#define I2S0_MASTER_CLK_CTRL0	0x80
#define I2S0_MASTER_CLK_CTRL1	0x84
#define I2S1_MASTER_CLK_CTRL0	0x90
#define I2S1_MASTER_CLK_CTRL1	0x94
#define I2S2_MASTER_CLK_CTRL0	0xA0
#define I2S2_MASTER_CLK_CTRL1	0xA4
#define I2S3_MASTER_CLK_CTRL0	0xB0
#define I2S3_MASTER_CLK_CTRL1	0xB4
#define SYS_LRCK_CTRL			0xC0

#define I2S_FRAME_SETTING_REG	0x04
#define I2S_ENABLE_REG			0x18
#define I2S_LCRK_MASTER_REG		0x2C
#define I2S_CLK_CTRL0_REG		0x60
#define I2S_CLK_CTRL1_REG		0x64
#define I2S_RESET_REG			0x1C
#define I2S_TX_STATUS_REG		0x48

#ifdef CONFIG_PM_SLEEP
struct subsys_reg_context {
	u32 sclk_in_sel;
	u32 fs_in_sel;
	u32 sdi_in_sel;
	u32 sdo_out_sel;
	u32 multi_sync;
	u32 bclk_oen_sel;
	u32 pdm_ctrl;
};
#endif

struct cvi_i2s_subsys_dev {
	void __iomem *subsys_base;
	struct device *dev;
	u32 master_id;
	u32 master_base;
#ifdef CONFIG_PM_SLEEP
	struct subsys_reg_context *reg_ctx;
#endif
};

#define	CVI_16384_MHZ	16384000   /* 16.384 Mhz */
#define	CVI_22579_MHZ	22579200   /* 22.5792 Mhz */
#define	CVI_24576_MHZ	24576000   /* 24.576 Mhz */

u32 i2s_subsys_query_master(void);
void i2s_master_clk_switch_on(bool on);
void i2s_set_master_clk(u32 clk_ctrl1);
void i2s_set_master_frame_setting(u32 frame_format);
void cv1835_set_mclk(u32 freq);
void cv182x_reset_dac(void);
void cv182x_reset_adc(void);
void cv182xa_reset_dac(void);
void cv182xa_reset_adc(void);

#endif
