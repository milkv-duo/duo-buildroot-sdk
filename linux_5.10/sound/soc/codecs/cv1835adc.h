/* SPDX-License-Identifier: GPL-2.0-or-later
 * CV1835 ADC driver on CVITEK CV1835
 *
 * Copyright 2019 CVITEK
 *
 * Author: EthanChen
 *
 */

#ifndef __CV1835ADC_H__
#define __CV1835ADC_H__

#include <linux/clk.h>
#include <linux/miscdevice.h>

#define CVI_ADC_GSEL_REG			0x00
#define CVI_ADC_GSELR_MASK			0xFFFFFFE0
#define CVI_ADC_GSELR(v)			(v << 0)
#define CVI_ADC_GSELL_MASK			0xFFFFE0FF
#define CVI_ADC_GSELL(v)			(v << 8)
#define CVI_ADC_GSELR_MIC_MASK		0xFFF8FFFF
#define CVI_ADC_GSELR_MIC(v)		(v << 16)
#define CVI_ADC_GSELL_MIC_MASK		0xF8FFFFFF
#define CVI_ADC_GSELL_MIC(v)		(v << 24)

#define CVI_ADC_CTRL_REG			0x04
#define CVI_ADC_INSELR_LINE			0xFFFFFFFE	/* bit[0] == 0*/
#define CVI_ADC_INSELR_MIC			(1 << 0)
#define CVI_ADC_INSELL_LINE			0xFFFFFFFD	/* bit[1] == 0*/
#define CVI_ADC_INSELL_MIC			(1 << 1)
#define CVI_ADC_POWER_MASK			0xFFFCFF0C	/* use to reset power on/off related bits */
#define CVI_ADC_ENADR_POWER_DOWN	0xFFFFFFFB	/* bit[2] == 0*/
#define CVI_ADC_ENADR_NORMAL		(1 << 2)
#define CVI_ADC_ENADL_POWER_DOWN	0xFFFFFFF7	/* bit[3] == 0*/
#define CVI_ADC_ENADL_NORMAL		(1 << 3)
#define CVI_ADC_VREF_POWER_DOWN		0xFFFFFFEF	/* bit[4] == 0*/
#define CVI_ADC_VERF_NORMAL			(1 << 4)
#define CVI_ADC_ZCD_DISABLE			0xFFFFFFDF	/* bit[5] == 0*/
#define CVI_ADC_ZCD_ENABLE			(1 << 5)
#define CVI_ADC_HPR_BYPASS			0xFFFFFFBF	/* bit[6] == 0*/
#define CVI_ADC_HPR_ENABLE			(1 << 6)
#define CVI_ADC_HPL_BYPASS			0xFFFFFF7F	/* bit[7] == 0*/
#define CVI_ADC_HPL_ENABLE			(1 << 7)
#define CVI_ADC_OVRS_MASK			0xFFFCFFFF
#define CVI_ADC_OVRS(v)				(v << 16)

#define CVI_ADC_OVTOP_REG			0x08

#define CVI_ADC_TEST_MODE_REG		0x0C
#define CVI_ADC_TM_NORMAL			0x0
#define CVI_ADC_TM_READ_ROM			0x2
#define CVI_ADC_TM_R_PCM			0x6
#define CVI_ADC_TM_L_PCM			0x7

#define CVI_ADC_RSEL_REG			0x10

#define CVI_ADC_FS_SEL_REG			0x14 /* ADMCLK/LRCK ratio */
#define CVI_ADC_RATIO_256			0x0
#define CVI_ADC_RATIO_512			0x1
#define CVI_ADC_RATIO_1024			0x2

#define CVI_ADC_DAGC_CTRL_REG		0x20 /* digital AGC control */
#define CVI_ADC_DAGC_DISABLE		0xFFFFFFFE /* bit[0] == 0 */
#define CVI_ADC_DAGC_ENABLE			(1 << 0)
#define CVI_ADC_DAGC_RMS_MODE		0xFFFFFFFD /* bit[1] == 1 */
#define CVI_ADC_DAGC_PEAK_MODE		(1 << 1)
#define CVI_ADC_DAGC_ZCD_DISABLE	0xFFFFFFFB /* bit[2] == 0 */
#define CVI_ADC_DAGC_ZCD_ENABLE		(1 << 2)

#define CVI_ADC_DAGC_THOLD_REG		0x24
#define CVI_ADC_DAGC_THOLD_MASK		0xFFFFFF81
#define CVI_ADC_DAGC_THOLD(v)		(v << 0) /* maximum is 0x7f, minimum is 0x01 */
#define CVI_ADC_DAGC_UPDATE_THOLD_MASK	0x8000FFFF
#define CVI_ADC_DAGC_UPDATE_THOLD(v)	(v << 16) /* maximum is 0x7fff */

#define CVI_ADC_DAGC_ATTACKTIME_REG		0x28

#define CVI_ADC_DAGC_SRC_DM_REG			0x2C
#define CVI_SDC_DAGC_UPDATE_FREQ_MASK	0xFFFFF000
#define CVI_SDC_DAGC_UPDATE_FREQ(v)		(v << 0)
#define CVI_ADC_DAGC_SRC_DM_MASK		0xFFFFCFFF
#define CVI_ADC_DAGC_SRC_DM_MONO_L		(0 << 12)

#define CVI_ADC_DAGC_PGAG_REG			0x30
#define CVI_ADC_DAGC_PGAG_R				0x0000001F
#define CVI_ADC_DAGC_PGAG_L				0x00001F00

#define CVI_ADC_ADCO_REG				0x34

struct cvi1835adc {
	void __iomem *adc_base;
	struct clk *clk;
	struct device *dev;
	struct miscdevice miscdev;
};

extern struct proc_dir_entry *proc_audio_dir;

#endif  /* __CV1835ADC_H__ */
