#ifndef __GC4653_CMOS_EX_H_
#define __GC4653_CMOS_EX_H_

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

enum gc4653_linear_regs_e {
	LINEAR_EXP_H,       //0x0202 bit[13:8]
	LINEAR_EXP_L,       //0x0203
	LINEAR_AGAIN_L,     //0x02b3
	LINEAR_AGAIN_H,     //0x02b4 bit[10:8]
	LINEAR_COL_AGAIN_H, //0x02b8 bit[13:8]
	LINEAR_COL_AGAIN_L, //0x02b9
	LINEAR_AGAIN_MAG1,  //0x0515
	LINEAR_AGAIN_MAG2,  //0x0519
	LINEAR_AGAIN_MAG3,  //0x02d9
	LINEAR_DGAIN_H,     //0x020e bit[9:6]
	LINEAR_DGAIN_L,     //0x020f bit[5:0]
	LINEAR_VTS_H,       //0x0341 bit[13:8]
	LINEAR_VTS_L,       //0x0342
	LINEAR_FRAME_BUF_ON,//0x031D
	LINEAR_FLIP_MIRROR,
	LINEAR_FRAME_BUF_OFF,//0x031D
	LINEAR_REGS_NUM
};

typedef enum _GC4653_MODE_E {
	GC4653_MODE_2560X1440P30 = 0,
	GC4653_MODE_LINEAR_NUM,
	GC4653_MODE_NUM
} GC4653_MODE_E;

typedef struct _GC4653_STATE_S {
	CVI_U32		u32Sexp_MAX;
} GC4653_STATE_S;

typedef struct _GC4653_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} GC4653_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc4653[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc4653_BusInfo[];
extern ISP_SNS_MIRRORFLIP_TYPE_E g_aeGc4653_MirrorFip[VI_MAX_PIPE_NUM];
extern CVI_U8 gc4653_i2c_addr;
extern const CVI_U32 gc4653_addr_byte;
extern const CVI_U32 gc4653_data_byte;
extern void gc4653_init(VI_PIPE ViPipe);
extern void gc4653_exit(VI_PIPE ViPipe);
extern void gc4653_standby(VI_PIPE ViPipe);
extern void gc4653_restart(VI_PIPE ViPipe);
extern int  gc4653_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc4653_read_register(VI_PIPE ViPipe, int addr);
extern int  gc4653_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC4653_CMOS_EX_H_ */

