#ifndef __OS04C10_CMOS_EX_H_
#define __OS04C10_CMOS_EX_H_

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


enum os04c10_linear_regs_e {
	LINEAR_HOLD_START = 0,
	LINEAR_EXP_0,
	LINEAR_EXP_1,
	LINEAR_AGAIN_0,
	LINEAR_AGAIN_1,
	LINEAR_DGAIN_0,
	LINEAR_DGAIN_1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_HCG,
	LINEAR_HOLD_END,
	LINEAR_LAUNCH_0,
	LINEAR_LAUNCH_1,
	LINEAR_REGS_NUM
};

enum os04c10_wdr2_regs_e {
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
	WDR2_DGAIN2_0,
	WDR2_DGAIN2_1,
	WDR2_VTS_0,
	WDR2_VTS_1,
	WDR2_HOLD_END,
	WDR2_LAUNCH_0,
	WDR2_LAUNCH_1,
	WDR2_GGAIN_0,
	WDR2_GGAIN_1,
	WDR2_REGS_NUM
};

typedef enum _OS04C10_MODE_E {
	OS04C10_MODE_2688X1520P30 = 0,
	OS04C10_MODE_2560X1440P30,
	OS04C10_MODE_LINEAR_NUM,
	OS04C10_MODE_2688X1520P30_WDR = OS04C10_MODE_LINEAR_NUM,
	OS04C10_MODE_2560X1440P30_WDR,
	OS04C10_MODE_NUM
} OS04C10_MODE_E;

typedef struct _OS04C10_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OS04C10_STATE_S;

typedef struct _OS04C10_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	CVI_U16 u16L2sOffset;
	CVI_U16 u16TopBoundary;
	CVI_U16 u16BotBoundary;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	CVI_U32 u32L2S_offset;
	CVI_U32 u32IspResTime;
	CVI_U32 u32HdrMargin;
	char name[64];
} OS04C10_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOs04c10[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOs04c10_BusInfo[];
extern CVI_U16 g_au16Os04c10_GainMode[];
extern CVI_U16 g_au16Os04c10_UseHwSync[VI_MAX_PIPE_NUM];
extern CVI_U8 os04c10_i2c_addr;
extern const CVI_U32 os04c10_addr_byte;
extern const CVI_U32 os04c10_data_byte;
extern void os04c10_init(VI_PIPE ViPipe);
extern void os04c10_exit(VI_PIPE ViPipe);
extern void os04c10_standby(VI_PIPE ViPipe);
extern void os04c10_restart(VI_PIPE ViPipe);
extern int  os04c10_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  os04c10_read_register(VI_PIPE ViPipe, int addr);
extern void os04c10_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int os04c10_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OS04C10_CMOS_EX_H_ */
