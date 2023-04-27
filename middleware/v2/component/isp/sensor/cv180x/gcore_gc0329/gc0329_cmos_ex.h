#ifndef __GC0329_CMOS_EX_H_
#define __GC0329_CMOS_EX_H_

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

typedef enum _GC0329_MODE_E {
	GC0329_MODE_640X480P10 = 0,
	GC0329_MODE_NUM
} GC0329_SLAVE_MODE_E;

typedef struct _GC0329_MODE_S {
	ISP_WDR_SIZE_S stImg;
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp;
	SNS_ATTR_LARGE_S stAgain;
	SNS_ATTR_LARGE_S stDgain;
	char name[64];
} GC0329_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastGc0329[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunGc0329_BusInfo[];
extern CVI_U8 gc0329_i2c_addr;
extern const CVI_U32 gc0329_addr_byte;
extern const CVI_U32 gc0329_data_byte;
extern void gc0329_init(VI_PIPE ViPipe);
extern void gc0329_exit(VI_PIPE ViPipe);
extern void gc0329_standby(VI_PIPE ViPipe);
extern void gc0329_restart(VI_PIPE ViPipe);
extern int  gc0329_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  gc0329_read_register(VI_PIPE ViPipe, int addr);
extern void gc0329_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  gc0329_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __GC0329_CMOS_EX_H_ */

