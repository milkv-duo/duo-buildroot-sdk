#ifndef __OV7251_CMOS_EX_H_
#define __OV7251_CMOS_EX_H_

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


enum ov7251_linear_regs_e {
	LINEAR_EXP_0 = 0,
	LINEAR_EXP_1,
	LINEAR_EXP_2,
	LINEAR_AGAIN0,
	LINEAR_AGAIN1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_REGS_NUM
};

typedef enum _OV7251_MODE_E {
	OV7251_MODE_640X480P120 = 0,
	OV7251_MODE_LINEAR_NUM,
	OV7251_MODE_NUM
} OV7251_MODE_E;

typedef struct _OV7251_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OV7251_STATE_S;

typedef struct _OV7251_MODE_S {
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
} OV7251_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOv7251[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOv7251_BusInfo[];
extern CVI_U16 g_au16Ov7251_GainMode[];
extern CVI_U16 g_au16Ov7251_L2SMode[VI_MAX_PIPE_NUM];
extern CVI_U8 ov7251_i2c_addr;
extern const CVI_U32 ov7251_addr_byte;
extern const CVI_U32 ov7251_data_byte;
extern void ov7251_init(VI_PIPE ViPipe);
extern void ov7251_exit(VI_PIPE ViPipe);
extern void ov7251_standby(VI_PIPE ViPipe);
extern void ov7251_restart(VI_PIPE ViPipe);
extern int  ov7251_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  ov7251_read_register(VI_PIPE ViPipe, int addr);
extern void ov7251_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int ov7251_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OV7251_CMOS_EX_H_ */
