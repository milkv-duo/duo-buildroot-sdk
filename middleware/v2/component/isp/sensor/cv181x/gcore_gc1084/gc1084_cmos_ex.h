#ifndef __GC1084_CMOS_EX_H_
#define __GC1084_CMOS_EX_H_

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

enum gc1084_linear_regs_e {
	LINEAR_EXP_H,
	LINEAR_EXP_L,

	LINEAR_AGAIN_M,
	LINEAR_AGAIN_L,
	LINEAR_AGAIN_REG_0x031D_0x2E,
	LINEAR_AGAIN_H,
	LINEAR_AGAIN_REG_0x031D_0x28,
	LINEAR_COL_AGAIN_H,
	LINEAR_COL_AGAIN_L,
	LINEAR_AGAIN_REG_0x0155,

	LINEAR_DGAIN_H,
	LINEAR_DGAIN_L,

	LINEAR_VTS_H,
	LINEAR_VTS_L,

	LINEAR_FLIP_MIRROR1,
	LINEAR_FLIP_MIRROR2,

	LINEAR_REGS_NUM
};


typedef enum _GC1084_MODE_E {
	GC1084_MODE_1280X720P30 = 0,
	GC1084_MODE_NUM
} GC1084_SLAVE_MODE_E;

typedef struct _GC1084_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC1084_STATE_S;

typedef struct _GC1084_MODE_S {
	ISP_WDR_SIZE_S stImg;
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp;
	SNS_ATTR_LARGE_S stAgain;
	SNS_ATTR_LARGE_S stDgain;
	char name[64];
} GC1084_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc1084[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc1084_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc1084_MirrorFip[VI_MAX_PIPE_NUM];
extern const CVI_U8 gc1084_i2c_addr;
extern const CVI_U32 gc1084_addr_byte;
extern const CVI_U32 gc1084_data_byte;
extern void gc1084_init(VI_PIPE ViPipe);
extern void gc1084_exit(VI_PIPE ViPipe);
extern void gc1084_standby(VI_PIPE ViPipe);
extern void gc1084_restart(VI_PIPE ViPipe);
extern int  gc1084_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc1084_read_register(VI_PIPE ViPipe, int addr);
extern int  gc1084_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC1084_CMOS_EX_H_ */

