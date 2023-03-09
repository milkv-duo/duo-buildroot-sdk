/* SPDX-License-Identifier: GPL-2.0-or-later
 * CV1835 DAC driver on CVITEK CV1835
 *
 * Copyright 2019 CVITEK
 *
 * Author: EthanChen
 *
 */

#ifndef __CV1835DAC_H__
#define __CV1835DAC_H__

#include <linux/clk.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <linux/miscdevice.h>

#define CVI_DAC_FS_REG				0x00
#define CVI_DAC_FS_16_32			0x0
#define CVI_DAC_FS_32_64			0x1
#define CVI_DAC_FS_64_128			0x2
#define CVI_DAC_FS_128_192			0x3

#define CVI_DAC_CTRL_REG			0x04
#define CVI_DAC_PWDAR_MASK			0x00000001
#define CVI_DAC_PWDAR_NORMAL		0xFFFFFFFE
#define CVI_DAC_PWDAR_DOWN			(1 << 0)
#define CVI_DAC_PWDAL_MASK			0x00000002
#define CVI_DAC_PWDAL_NORMAL		0xFFFFFFFD
#define CVI_DAC_PWDAL_DOWN			(1 << 1)
#define CVI_DAC_EN_REF_DISABLE		0xFFFFFFFB
#define CVI_DAC_EN_REF_ENABLE		(1 << 2)
#define CVI_DAC_DEN_ENABLE			0xFFFFFFF7
#define CVI_DAC_DEN_DISABLE			(1 << 3)
#define CVI_DAC_AEN_ENABLE			0xFFFFFFEF
#define CVI_DAC_AEN_DISABLE			(1 << 4)
#define CVI_DAC_STR_DISABLE			0xFFFFFFDF
#define CVI_DAC_STR_ENABLE			(1 << 5)
#define CVI_DAC_DISTHER_DISABLE		0xFFFFFFBF
#define CVI_DAC_DISTHER_ENABLE		(1 << 6)
#define CVI_DAC_MUTE_MODE			0xFFFFFCFF
#define CVI_DAC_DEMUTE_MODE			(0x3 << 8)
#define CVI_DAC_CKINV_NODELAY		0xFFFFFBFF
#define CVI_DAC_CKINV_DELAY			(1 << 10)
#define CVI_DAC_S_MASK				0xFFFF0FFF
#define CVI_DAC_S(v)				(v << 12)
#define CVI_DAC_ZCD_DISABLE			0xFFFEFFFF
#define CVI_DAC_ZCD_ENABLE			(1 << 16)

#define CVI_DAC_TEST_MODE_REG		0x08
#define CVI_DAC_TM_NORMAL			0x0
#define CVI_DAC_TM_DF				0x1 /* DAC digital filter test mode */
#define CVI_DAC_TM_SDM				0x2 /* DAC delta sigma modulator */

#define CVI_DAC_AIO_DA_REG			0x0C
#define CVI_DAV_AIO_ALL_GPIO		0x0
#define CVI_DAV_AIO_2CH				0x1

#define CVI_DAC_AS_LEVEL_REG	0x10

struct cvi1835dac {
	void __iomem *dac_base;
	struct clk *clk;
	struct device *dev;
	struct miscdevice miscdev;
};

extern bool cv1835dac_is_mute(struct snd_pcm_substream *substream);

extern struct proc_dir_entry *proc_audio_dir;

#endif  /* __CV1835DAC_H__ */
