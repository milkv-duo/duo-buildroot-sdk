#ifndef __OS08A20_CMOS_EX_H_
#define __OS08A20_CMOS_EX_H_

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


enum os08a20_linear_regs_e {
	LINEAR_HOLD1 = 0,
	LINEAR_HOLD2,
	LINEAR_HOLD3,
	LINEAR_EXP_0,
	LINEAR_EXP_1,
	LINEAR_AGAIN_0,
	LINEAR_AGAIN_1,
	LINEAR_DGAIN_0,
	LINEAR_DGAIN_1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_REL1,
	LINEAR_REL2,
	LINEAR_REL3,
	LINEAR_REGS_NUM
};

enum os08a20_wdr2_regs_e {
	WDR2_HOLD1 = 0,
	WDR2_HOLD2,
	WDR2_HOLD3,
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
	WDR2_REL1,
	WDR2_REL2,
	WDR2_REL3,
	WDR2_REGS_NUM
};

typedef enum _OS08A20_MODE_E {
	OS08A20_MODE_2592X1944P30 = 0,
	OS08A20_MODE_LINEAR_NUM,
	OS08A20_MODE_2592X1944P30_WDR = OS08A20_MODE_LINEAR_NUM,
	OS08A20_MODE_NUM
} OS08A20_MODE_E;

typedef struct _OS08A20_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OS08A20_STATE_S;

typedef struct _OS08A20_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	CVI_U16 u16L2sOffset;
	CVI_U16 u16TopBoundary;
	CVI_U16 u16BotBoundary;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	CVI_U32 u32L2S_MAX;
	char name[64];
} OS08A20_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOs08a20[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOs08a20_BusInfo[];
extern CVI_U16 g_au16Os08a20_GainMode[];
extern CVI_U16 g_au16Os08a20_L2SMode[VI_MAX_PIPE_NUM];
extern const CVI_U8 os08a20_i2c_addr;
extern const CVI_U32 os08a20_addr_byte;
extern const CVI_U32 os08a20_data_byte;
extern void os08a20_init(VI_PIPE ViPipe);
extern void os08a20_exit(VI_PIPE ViPipe);
extern void os08a20_standby(VI_PIPE ViPipe);
extern void os08a20_restart(VI_PIPE ViPipe);
extern int  os08a20_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  os08a20_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OS08A20_CMOS_EX_H_ */
