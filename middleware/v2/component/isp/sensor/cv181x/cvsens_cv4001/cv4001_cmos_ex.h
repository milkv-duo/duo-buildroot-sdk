#ifndef __CV4001_CMOS_EX_H_
#define __CV4001_CMOS_EX_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef ARCH_CV182X
#include <linux/cvi_vip_cif.h>
#include <linux/cvi_vip_snsr.h>
#include "cvi_type.h"
#else
#include <linux/cif_uapi.h>
#include <linux/vi_snsr.h>
#include <linux/cvi_type.h>
#endif
#include "cvi_sns_ctrl.h"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

enum cv4001_linear_regs_e {
	LINEAR_EXP_0,       //0x3062 bit[19:16]
	LINEAR_EXP_1,       //0x3061
	LINEAR_EXP_2,       //0x3060
	LINEAR_AGAIN,       //0x3180 bit[7:0]
	LINEAR_DGAIN_H,     //0x3179 bit[15:8]
	LINEAR_DGAIN_L,     //0x3178 bit[7:0]
	LINEAR_VTS_0,       //0x302A bit[19:16]
	LINEAR_VTS_1,       //0x3029
	LINEAR_VTS_2,       //0x3028
	LINEAR_FLIP_MIRROR, //0x3034
	LINEAR_REGS_NUM
};

enum cv4001_wdr2_regs_e {
	WDR2_EXP1_0,
	WDR2_EXP1_1,
	WDR2_EXP1_2,
	WDR2_EXP2_0,
	WDR2_EXP2_1,
	WDR2_EXP2_2,
	WDR2_AGAIN1,
	WDR2_AGAIN2,
	WDR2_DGAIN1_H,
	WDR2_DGAIN1_L,
	WDR2_DGAIN2_H,
	WDR2_DGAIN2_L,
	WDR2_VTS_0,
	WDR2_VTS_1,
	WDR2_VTS_2,
	WDR2_FLIP_MIRROR,
	WDR2_REGS_NUM
};

typedef enum _CV4001_MODE_E {
	CV4001_MODE_2560X1440P25 = 0,
	CV4001_MODE_LINEAR_NUM,
	CV4001_MODE_2560X1440P15_WDR = CV4001_MODE_LINEAR_NUM,
	CV4001_MODE_NUM
} CV4001_MODE_E;

typedef struct _CV4001_STATE_S {
	CVI_U32		u32Sexp_MAX;
} CV4001_STATE_S;

typedef struct _CV4001_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	CVI_U32 u32IspResTime;
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} CV4001_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastCV4001[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunCV4001_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeCV4001_MirrorFip[VI_MAX_PIPE_NUM];
extern CVI_U8 cv4001_i2c_addr;
extern const CVI_U32 cv4001_addr_byte;
extern const CVI_U32 cv4001_data_byte;
extern void cv4001_init(VI_PIPE ViPipe);
extern void cv4001_exit(VI_PIPE ViPipe);
extern void cv4001_standby(VI_PIPE ViPipe);
extern void cv4001_restart(VI_PIPE ViPipe);
extern int  cv4001_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  cv4001_read_register(VI_PIPE ViPipe, int addr);
extern int  cv4001_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __CV4001_CMOS_EX_H_ */

