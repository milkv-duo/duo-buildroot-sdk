/* SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __CV1835_IOCTL_H__
#define __CV1835_IOCTL_H__

enum cv1835_codec_fs {
	ACODEC_FS_8000  =   0x1,
	ACODEC_FS_11025 =   0x2,
	ACODEC_FS_12000 =   0x3,
	ACODEC_FS_16000 =   0x4,
	ACODEC_FS_22050 =   0x5,
	ACODEC_FS_24000 =   0x6,
	ACODEC_FS_32000 =   0x7,
	ACODEC_FS_44100 =   0x8,
	ACODEC_FS_48000 =   0x9,
	ACODEC_FS_64000 =   0xa,
	ACODEC_FS_96000 =   0xb,
	ACODEC_FS_192000 =  0xc,
};

enum cv1835_mixer {
	CVI_MIXER_LINEIN = 0x0,
	CVI_MIXER_MIC_IN = 0x1,
};

enum cv1835_codec_cmd {
	CVI_SOFT_RESET_CTRL = 0x0,

	CVI_SET_INPUT_VOL,
	CVI_SET_OUTPUT_VOL,
	CVI_GET_INPUT_VOL,
	CVI_GET_OUTPUT_VOL,

	CVI_SET_I2S1_FS,
	CVI_SET_MIXER_MIC,
	CVI_SEL_DAC_CLK, /* not support */
	CVI_SEL_ADC_CLK, /* not support */
	CVI_SEL_ANA_MCLK, /* not support */
	CVI_SET_GAIN_MICL,
	CVI_SET_GAIN_MICR,
	CVI_SET_DACL_VOL,
	CVI_SET_DACR_VOL,
	CVI_SET_ADCL_VOL,
	CVI_SET_ADCR_VOL,
	CVI_SET_MICL_MUTE,
	CVI_SET_MICR_MUTE,
	CVI_SET_DACL_MUTE,
	CVI_SET_DACR_MUTE,
	CVI_BOOSTL_ENABLE, /* not support */
	CVI_BOOSTR_ENABLE, /* not support */

	CVI_GET_GAIN_MICL,
	CVI_GET_GAIN_MICR,
	CVI_GET_DACL_VOL,
	CVI_GET_DACR_VOL,
	CVI_GET_ADCL_VOL,
	CVI_GET_ADCR_VOL,

	CVI_SET_PD_DACL,
	CVI_SET_PD_DACR,
	CVI_SET_PD_ADCL,
	CVI_SET_PD_ADCR,
	CVI_SET_PD_LINEINL,
	CVI_SET_PD_LINEINR,

	CVI_SET_DAC_DE_EMPHASIS,
	CVI_SET_ADC_HP_FILTER,

	CVI_SET_I2S1_DATAWIDTH, /* not support */

	CVI_DACL_SEL_TRACK, /* not support */
	CVI_DACR_SEL_TRACK, /* not support */
	CVI_ADCL_SEL_TRACK, /* not support */
	CVI_ADCR_SEL_TRACK, /* not support */
};

struct cvi_vol_ctrl {
	/* volume control, adc range: 0x00~0x1f, 0x17F:mute. dac range: 0x00~0x0f, 0x0f:mute */
	unsigned int vol_ctrl;
	/* adc/dac mute control, 1:mute, 0:unmute */
	unsigned int vol_ctrl_mute;
};

#define ACODEC_SOFT_RESET_CTRL _IO('A', CVI_SOFT_RESET_CTRL)

#define ACODEC_SET_INPUT_VOL _IOWR('A', CVI_SET_INPUT_VOL, unsigned int)
#define ACODEC_SET_OUTPUT_VOL _IOWR('A', CVI_SET_OUTPUT_VOL, unsigned int)
#define ACODEC_GET_INPUT_VOL _IOWR('A', CVI_GET_INPUT_VOL, unsigned int)
#define ACODEC_GET_OUTPUT_VOL _IOWR('A', CVI_GET_OUTPUT_VOL, unsigned int)

#define ACODEC_SET_I2S1_FS _IOWR('A', CVI_SET_I2S1_FS, unsigned int)
#define ACODEC_SET_MIXER_MIC _IOWR('A', CVI_SET_MIXER_MIC, unsigned int)
#define ACODEC_SET_GAIN_MICL _IOWR('A', CVI_SET_GAIN_MICL, unsigned int)
#define ACODEC_SET_GAIN_MICR _IOWR('A', CVI_SET_GAIN_MICR, unsigned int)
#define ACODEC_SET_DACL_VOL _IOWR('A', CVI_SET_DACL_VOL, struct cvi_vol_ctrl)
#define ACODEC_SET_DACR_VOL _IOWR('A', CVI_SET_DACR_VOL, struct cvi_vol_ctrl)
#define ACODEC_SET_ADCL_VOL _IOWR('A', CVI_SET_ADCL_VOL, struct cvi_vol_ctrl)
#define ACODEC_SET_ADCR_VOL _IOWR('A', CVI_SET_ADCR_VOL, struct cvi_vol_ctrl)

#define ACODEC_SET_MICL_MUTE _IOWR('A', CVI_SET_MICL_MUTE, unsigned int)
#define ACODEC_SET_MICR_MUTE _IOWR('A', CVI_SET_MICR_MUTE, unsigned int)
#define ACODEC_SET_DACL_MUTE _IOWR('A', CVI_SET_DACL_MUTE, unsigned int)
#define ACODEC_SET_DACR_MUTE _IOWR('A', CVI_SET_DACR_MUTE, unsigned int)


#define ACODEC_GET_GAIN_MICL _IOWR('A', CVI_GET_GAIN_MICL, unsigned int)
#define ACODEC_GET_GAIN_MICR _IOWR('A', CVI_GET_GAIN_MICR, unsigned int)
#define ACODEC_GET_DACL_VOL _IOWR('A', CVI_GET_DACL_VOL, struct cvi_vol_ctrl)
#define ACODEC_GET_DACR_VOL _IOWR('A', CVI_GET_DACR_VOL, struct cvi_vol_ctrl)
#define ACODEC_GET_ADCL_VOL _IOWR('A', CVI_GET_ADCL_VOL, struct cvi_vol_ctrl)
#define ACODEC_GET_ADCR_VOL _IOWR('A', CVI_GET_ADCR_VOL, struct cvi_vol_ctrl)


#define ACODEC_SET_PD_DACL _IOWR('A', CVI_SET_PD_DACL, unsigned int)
#define ACODEC_SET_PD_DACR _IOWR('A', CVI_SET_PD_DACR, unsigned int)
#define ACODEC_SET_PD_ADCL _IOWR('A', CVI_SET_PD_ADCL, unsigned int)
#define ACODEC_SET_PD_ADCR _IOWR('A', CVI_SET_PD_ADCR, unsigned int)
#define ACODEC_SET_PD_LINEINL _IOWR('A', CVI_SET_PD_LINEINL, unsigned int)
#define ACODEC_SET_PD_LINEINR _IOWR('A', CVI_SET_PD_LINEINR, unsigned int)

#define ACODEC_SET_DAC_DE_EMPHASIS _IOWR('A', CVI_SET_DAC_DE_EMPHASIS, unsigned int)
#define ACODEC_SET_ADC_HP_FILTER _IOWR('A', CVI_SET_ADC_HP_FILTER, unsigned int)

#endif  /* __CV1835_IOCTL_H__ */
