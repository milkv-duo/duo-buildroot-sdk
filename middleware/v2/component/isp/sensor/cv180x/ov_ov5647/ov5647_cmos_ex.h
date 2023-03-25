#ifndef __OV5647_CMOS_EX_H_
#define __OV5647_CMOS_EX_H_

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


enum ov5647_linear_regs_e {
	LINEAR_EXP_0,
	LINEAR_EXP_1,
	LINEAR_EXP_2,
	LINEAR_AGAIN_0,
	LINEAR_AGAIN_1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_REGS_NUM
};

typedef enum _OV5647_MODE_E {
	OV5647_MODE_1920X1080P30 = 0,
	OV5647_MODE_LINEAR_NUM,
	OV5647_MODE_NUM
} OV5647_MODE_E;

typedef struct _OV5647_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OV5647_STATE_S;

typedef struct _OV5647_MODE_S {
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
} OV5647_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOv5647[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOv5647_BusInfo[];
extern CVI_U16 g_au16Ov5647_GainMode[];
extern CVI_U16 g_au16Ov5647_UseHwSync[VI_MAX_PIPE_NUM];
extern CVI_U8 ov5647_i2c_addr;
extern const CVI_U32 ov5647_addr_byte;
extern const CVI_U32 ov5647_data_byte;
extern void ov5647_init(VI_PIPE ViPipe);
extern void ov5647_exit(VI_PIPE ViPipe);
extern void ov5647_standby(VI_PIPE ViPipe);
extern void ov5647_restart(VI_PIPE ViPipe);
extern int  ov5647_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  ov5647_read_register(VI_PIPE ViPipe, int addr);
extern void ov5647_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int ov5647_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OV5647_CMOS_EX_H_ */

