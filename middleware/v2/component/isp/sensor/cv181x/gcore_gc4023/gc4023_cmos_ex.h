#ifndef __GC4023_CMOS_EX_H_
#define __GC4023_CMOS_EX_H_

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

enum gc4023_linear_regs_e {
	LINEAR_EXP_H,
	LINEAR_EXP_L,
	LINEAR_AGAIN_L,
	LINEAR_AGAIN_H,
	LINEAR_AGAIN_MAG1,
	LINEAR_AGAIN_MAG2,
	LINEAR_AGAIN_MAG3,
	LINEAR_COL_AGAIN_H,
	LINEAR_COL_AGAIN_L,
	LINEAR_VTS_H,
	LINEAR_VTS_L,
	LINEAR_MIRROR_FLIP,
	LINEAR_OTP_MIRROR_FLIP,
	LINEAR_REGS_NUM
};

typedef enum _GC4023_MODE_E {
	GC4023_MODE_2560X1440P30 = 0,
	GC4023_MODE_LINEAR_NUM,
	GC4023_MODE_NUM
} GC4023_MODE_E;

typedef struct _GC4023_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC4023_STATE_S;

typedef struct _GC4023_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} GC4023_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc4023[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc4023_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc4023_MirrorFip[VI_MAX_PIPE_NUM];
extern CVI_U8 gc4023_i2c_addr;
extern const CVI_U32 gc4023_addr_byte;
extern const CVI_U32 gc4023_data_byte;
extern void gc4023_init(VI_PIPE ViPipe);
extern void gc4023_exit(VI_PIPE ViPipe);
extern void gc4023_standby(VI_PIPE ViPipe);
extern void gc4023_restart(VI_PIPE ViPipe);
extern int  gc4023_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc4023_read_register(VI_PIPE ViPipe, int addr);
extern int  gc4023_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC4023_CMOS_EX_H_ */

