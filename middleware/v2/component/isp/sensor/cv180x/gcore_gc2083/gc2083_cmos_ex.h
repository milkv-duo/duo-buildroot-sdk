#ifndef __GC2083_CMOS_EX_H_
#define __GC2083_CMOS_EX_H_

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

enum gc2083_linear_regs_e {
	LINEAR_EXP_H,	//0x0d03
	LINEAR_EXP_L,	//0x0d04

	LINEAR_AGAIN_L,	//0x00d0
	LINEAR_AGAIN_MAG1,	//0x0155
	LINEAR_AGAIN_MAG2,	//0x0410
	LINEAR_AGAIN_MAG3,	//0x0411
	LINEAR_AGAIN_MAG4,	//0x0412
	LINEAR_AGAIN_MAG5,	//0x0413
	LINEAR_AGAIN_MAG6,	//0x0414
	LINEAR_AGAIN_MAG7,	//0x0415
	LINEAR_AGAIN_MAG8,	//0x0416
	LINEAR_AGAIN_MAG9,	//0x0417

	LINEAR_COL_AGAIN_H,	//0x00b8
	LINEAR_COL_AGAIN_L,	//0x00b9


	LINEAR_AGAIN_HOLD_2E,	// 0x031d
	LINEAR_AGAIN_H,	//0x0dc1
	LINEAR_AGAIN_HOLD_28,	// 0x031d

	LINEAR_DGAIN_H,	//0x00b1
	LINEAR_DGAIN_L,	//0x00b2

	LINEAR_VTS_H,	//0x0d41  (frame length)
	LINEAR_VTS_L,	//0x0d42

	LINEAR_REGS_NUM

};


typedef enum _GC2083_MODE_E {
	GC2083_MODE_1920X1080P30 = 0,
	GC2083_MODE_NUM
} GC2083_SLAVE_MODE_E;

typedef struct _GC2083_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC2083_STATE_S;

typedef struct _GC2083_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} GC2083_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc2083[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc2083_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc2083_MirrorFip[VI_MAX_PIPE_NUM];
extern CVI_U8 gc2083_i2c_addr;
extern const CVI_U32 gc2083_addr_byte;
extern const CVI_U32 gc2083_data_byte;
extern void gc2083_init(VI_PIPE ViPipe);
extern void gc2083_exit(VI_PIPE ViPipe);
extern void gc2083_standby(VI_PIPE ViPipe);
extern void gc2083_restart(VI_PIPE ViPipe);
extern int  gc2083_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc2083_read_register(VI_PIPE ViPipe, int addr);
extern void gc2083_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  gc2083_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC2083_CMOS_EX_H_ */

