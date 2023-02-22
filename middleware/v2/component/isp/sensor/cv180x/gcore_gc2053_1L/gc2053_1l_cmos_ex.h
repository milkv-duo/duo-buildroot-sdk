#ifndef __GC2053_1L_CMOS_EX_H_
#define __GC2053_1L_CMOS_EX_H_

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

enum gc2053_1l_linear_regs_e {
	LINEAR_EXP_H = 0,//03
	LINEAR_EXP_L,	//04
	LINEAR_AGAIN_H, //b4
	LINEAR_AGAIN_L, //b3
	LINEAR_COL_AGAIN_H, //b8
	LINEAR_COL_AGAIN_L, //b9
	LINEAR_DGAIN_H,	//b1
	LINEAR_DGAIN_L, //b2
	LINEAR_VTS_H, //0x41  (frame length)
	LINEAR_VTS_L,//0x42
	LINEAR_REGS_NUM
};


typedef enum _GC2053_1L_MODE_E {
	GC2053_1L_MODE_1920X1080P30 = 0,
	GC2053_1L_MODE_NUM
} GC2053_1L_SLAVE_MODE_E;

typedef struct _GC2053_1L_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC2053_1L_STATE_S;

typedef struct _GC2053_1L_MODE_S {
	ISP_WDR_SIZE_S stImg;
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp;
	SNS_ATTR_LARGE_S stAgain;
	SNS_ATTR_LARGE_S stDgain;
	char name[64];
} GC2053_1L_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc2053_1l[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc2053_1l_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc2053_1l_MirrorFip[VI_MAX_PIPE_NUM];
extern CVI_U8 gc2053_1l_i2c_addr;
extern const CVI_U32 gc2053_1l_addr_byte;
extern const CVI_U32 gc2053_1l_data_byte;
extern void gc2053_1l_init(VI_PIPE ViPipe);
extern void gc2053_1l_exit(VI_PIPE ViPipe);
extern void gc2053_1l_standby(VI_PIPE ViPipe);
extern void gc2053_1l_restart(VI_PIPE ViPipe);
extern int  gc2053_1l_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc2053_1l_read_register(VI_PIPE ViPipe, int addr);
extern void gc2053_1l_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  gc2053_1l_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC2053_1L_CMOS_EX_H_ */

