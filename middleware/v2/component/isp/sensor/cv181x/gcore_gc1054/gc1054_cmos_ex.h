#ifndef __GC1054_CMOS_EX_H_
#define __GC1054_CMOS_EX_H_

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

enum gc1054_linear_regs_e {
	LINEAR_EXP_PAGE = 0,		//fe
	LINEAR_EXP_H,			//03
	LINEAR_EXP_L,			//04
	LINEAR_GAIN_PAGE,		//fe
	LINEAR_AGAIN,			//b6
	LINEAR_VB_PAGE,			//fe
	LINEAR_VB_H,			//07
	LINEAR_VB_L,			//08
	LINEAR_FLIP_MIRROR_PAGE,	//fe
	LINEAR_FLIP_MIRROR,		//17
	LINEAR_REGS_NUM
};


typedef enum _GC1054_MODE_E {
	GC1054_MODE_1280X720P30 = 0,
	GC1054_MODE_NUM
} GC1054_SLAVE_MODE_E;

typedef struct _GC1054_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC1054_STATE_S;

typedef struct _GC1054_MODE_S {
	ISP_WDR_SIZE_S stImg;
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp;
	SNS_ATTR_LARGE_S stAgain;
	SNS_ATTR_LARGE_S stDgain;
	char name[64];
} GC1054_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc1054[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc1054_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc1054_MirrorFip[VI_MAX_PIPE_NUM];
extern const CVI_U8 gc1054_i2c_addr;
extern const CVI_U32 gc1054_addr_byte;
extern const CVI_U32 gc1054_data_byte;
extern void gc1054_init(VI_PIPE ViPipe);
extern void gc1054_exit(VI_PIPE ViPipe);
extern void gc1054_standby(VI_PIPE ViPipe);
extern void gc1054_restart(VI_PIPE ViPipe);
extern int  gc1054_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc1054_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC1054_CMOS_EX_H_ */

