#ifndef __GC02M1_CMOS_EX_H_
#define __GC02M1_CMOS_EX_H_

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

enum gc02m1_linear_regs_e {
	LINEAR_PAGE_0,
	LINEAR_EXP_H,
	LINEAR_EXP_L,
	LINEAR_AGAIN,
	LINEAR_VTS_H,
	LINEAR_VTS_L,
	LINEAR_MIRROR_FLIP,

	LINEAR_REGS_NUM
};

typedef enum _GC02M1_MODE_E {
	GC02M1_MODE_1600X1200P30 = 0,
	GC02M1_MODE_LINEAR_NUM,
	GC02M1_MODE_NUM
} GC02M1_MODE_E;

typedef struct _GC02M1_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC02M1_STATE_S;

typedef struct _GC02M1_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} GC02M1_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc02m1[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc02m1_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc02m1_MirrorFip[VI_MAX_PIPE_NUM];
extern CVI_U8 gc02m1_i2c_addr;
extern const CVI_U32 gc02m1_addr_byte;
extern const CVI_U32 gc02m1_data_byte;
extern void gc02m1_init(VI_PIPE ViPipe);
extern void gc02m1_exit(VI_PIPE ViPipe);
extern void gc02m1_standby(VI_PIPE ViPipe);
extern void gc02m1_restart(VI_PIPE ViPipe);
extern int  gc02m1_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc02m1_read_register(VI_PIPE ViPipe, int addr);
extern int  gc02m1_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC02M1_CMOS_EX_H_ */

