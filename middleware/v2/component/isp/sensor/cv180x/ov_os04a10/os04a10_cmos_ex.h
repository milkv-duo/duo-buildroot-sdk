#ifndef __OS04A10_CMOS_EX_H_
#define __OS04A10_CMOS_EX_H_

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


enum os04a10_linear_regs_e {
	LINEAR_HOLD_START = 0,
	LINEAR_EXP_0,
	LINEAR_EXP_1,
	LINEAR_AGAIN_0,
	LINEAR_AGAIN_1,
	LINEAR_DGAIN_0,
	LINEAR_DGAIN_1,
	LINEAR_DGAIN_2,
	LINEAR_HCG_0,
	LINEAR_HCG_1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_HOLD_END,
	LINEAR_LAUNCH_0,
	LINEAR_LAUNCH_1,
	LINEAR_REGS_NUM
};

enum os04a10_wdr2_regs_e {
	WDR2_HOLD_START = 0,
	WDR2_EXP1_0,
	WDR2_EXP1_1,
	WDR2_EXP2_0,
	WDR2_EXP2_1,
	WDR2_AGAIN1_0,
	WDR2_AGAIN1_1,
	WDR2_AGAIN2_0,
	WDR2_AGAIN2_1,
	WDR2_DGAIN1_0,
	WDR2_DGAIN1_1,
	WDR2_DGAIN1_2,
	WDR2_DGAIN2_0,
	WDR2_DGAIN2_1,
	WDR2_DGAIN2_2,
	WDR2_HCG_0,
	WDR2_HCG_1,
	WDR2_VTS_0,
	WDR2_VTS_1,
	WDR2_HOLD_END,
	WDR2_LAUNCH_0,
	WDR2_LAUNCH_1,
	WDR2_REGS_NUM
};

typedef enum _OS04A10_MODE_E {
	OS04A10_MODE_1440P30_12BIT = 0,
	OS04A10_MODE_LINEAR_NUM,
	OS04A10_MODE_1440P30_WDR = OS04A10_MODE_LINEAR_NUM,
	OS04A10_MODE_NUM
} OS04A10_MODE_E;

typedef struct _OS04A10_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OS04A10_STATE_S;

typedef struct _OS04A10_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
	CVI_U32 u32L2S_offset;
	CVI_U32 u32IspResTime;
	CVI_U32 u32VStart;
	CVI_U32 u32VEnd;
} OS04A10_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOs04a10[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOs04a10_BusInfo[];
extern CVI_U16 g_au16Os04a10_GainMode[];
extern CVI_U16 g_au16Os04a10_UseHwSync[VI_MAX_PIPE_NUM];
extern CVI_U8 os04a10_i2c_addr;
extern const CVI_U32 os04a10_addr_byte;
extern const CVI_U32 os04a10_data_byte;
extern void os04a10_init(VI_PIPE ViPipe);
extern void os04a10_exit(VI_PIPE ViPipe);
extern void os04a10_standby(VI_PIPE ViPipe);
extern void os04a10_restart(VI_PIPE ViPipe);
extern void os04a10_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  os04a10_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  os04a10_read_register(VI_PIPE ViPipe, int addr);
extern int  os04a10_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OS04A10_CMOS_EX_H_ */
