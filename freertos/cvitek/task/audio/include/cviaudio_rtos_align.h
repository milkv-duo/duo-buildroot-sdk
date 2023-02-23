#ifndef __CVIAUDIO_KERNEL_ALIGN_H__
#define __CVIAUDIO_KERNEL_ALIGN_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "cvi_comm_aio.h"


#ifndef CVIAUDIO_BYTES_ALIGNMENT
#define CVIAUDIO_BYTES_ALIGNMENT  64
#endif

#ifndef CVIAUDIO_ALIGN
#define CVIAUDIO_ALIGN(x, a)      (((x) + ((a)-1)) & ~((a)-1))
#endif

#define ATTR __attribute__

#ifndef __packed
#define __packed ATTR((packed))
#endif

#ifndef __aligned
#define __aligned(x) ATTR((aligned(x)))
#endif

typedef struct _AI_AEC_CONFIG_S_RTOS AI_AEC_CONFIG_S_RTOS;
struct _AI_AEC_CONFIG_S_RTOS {
	CVI_U8 para_aec_filter_len;
	CVI_U8 para_aes_std_thrd;                 /* the threshold of STD/DTD, [0, 35] */
	CVI_U8 para_aes_supp_coeff;           /* the residual echo suppression level in AES, [0, 24] */
} __packed __aligned(0x4);

typedef struct _AUDIO_DELAY_CONFIG_S_RTOS AUDIO_DELAY_CONFIG_S_RTOS;
struct _AUDIO_DELAY_CONFIG_S_RTOS {
	/* the initial filter length of linear AEC to support up for echo tail, [1, 13] */
	CVI_U8 para_aec_init_filter_len;
	/* the digital gain target, [1, 12] */
	CVI_U8 para_dg_target;
	/* the delay sample for ref signal, [1, 3000] */
	CVI_U8 para_delay_sample;
} __packed __aligned(0x4);

typedef struct _AUDIO_AGC_CONFIG_S_RTOS AUDIO_AGC_CONFIG_S_RTOS;
struct _AUDIO_AGC_CONFIG_S_RTOS {
	/* the max boost gain for AGC release processing, [0, 3] */
	/* para_obj.para_agc_max_gain = 1; */
	CVI_U8 para_agc_max_gain;
	/* the gain level of target high of AGC, [0, 36] */
	/* para_obj.para_agc_target_high = 2; */
	CVI_U8 para_agc_target_high;
	/* the gain  level of target low of AGC, [0, 36] */
	/* para_obj.para_agc_target_low = 6; */
	CVI_U8 para_agc_target_low;
	/* speech-activated AGC functionality, [0, 1] */
	/* para_obj.para_agc_vad_enable = 1; */
	CVI_U8 para_agc_vad_ena;
} __packed __aligned(0x4);

typedef struct _AUDIO_ANR_CONFIG_S_RTOS AUDIO_ANR_CONFIG_S_RTOS;
struct _AUDIO_ANR_CONFIG_S_RTOS {
	/* the coefficient of NR priori SNR tracking, [0, 20] */
	/* para_obj.para_nr_snr_coeff = 15; */
	CVI_U8 para_nr_snr_coeff;
	/* the coefficient of NR noise tracking, [0, 14] */
	/* para_obj.para_nr_noise_coeff = 2; */
	//CVI_S8 para_nr_noise_coeff;
	CVI_U8 para_nr_init_sile_time;
} __packed __aligned(0x4);

typedef struct _AI_TALKVQE_CONFIG_S_RTOS AI_TALKVQE_CONFIG_S_RTOS;
struct  _AI_TALKVQE_CONFIG_S_RTOS {
	CVI_U8	para_client_config;
	CVI_U8	u32OpenMask;
	CVI_U16	s32WorkSampleRate;
	/* Sample Rate: 8KHz/16KHz. default: 8KHz*/
	//MIC IN VQE setting
	AI_AEC_CONFIG_S_RTOS     stAecCfg;
	AUDIO_ANR_CONFIG_S_RTOS  stAnrCfg;
	AUDIO_AGC_CONFIG_S_RTOS  stAgcCfg;
	AUDIO_DELAY_CONFIG_S_RTOS stAecDelayCfg;
	CVI_U8 s32RevMask;//turn this flag to default 0x11
	CVI_U8 para_notch_freq;//user can ignore this flag
	CVI_U8 customize[8];
} __packed __aligned(0x4);

#ifdef __cplusplus
}
#endif
#endif
