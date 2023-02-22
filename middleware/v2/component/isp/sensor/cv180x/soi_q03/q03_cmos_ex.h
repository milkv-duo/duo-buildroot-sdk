#ifndef __Q03_CMOS_EX_H_
#define __Q03_CMOS_EX_H_

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

enum q03_linear_regs_e {
	LINEAR_SHS1_0_DATA,
	LINEAR_SHS1_1_DATA,
	LINEAR_AGAIN_DATA,
	LINEAR_VMAX_0_DATA,
	LINEAR_VMAX_1_DATA,
	LINEAR_REGS_NUM
};

typedef enum _Q03_MODE_E {
	Q03_MODE_1296P30 = 0,
	Q03_MODE_LINEAR_NUM,
	Q03_MODE_1296P30_WDR = Q03_MODE_LINEAR_NUM,
	Q03_MODE_NUM
} Q03_MODE_E;

typedef struct _Q03_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	CVI_U8 u8DgainReg;
	CVI_U32 u32L2S_MAX;
	char name[64];
} Q03_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastQ03[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunQ03_BusInfo[];
extern CVI_U16 g_au16Q03_GainMode[];
extern CVI_U16 g_au16Q03_L2SMode[];
extern CVI_U8 q03_i2c_addr;
extern const CVI_U32 q03_addr_byte;
extern const CVI_U32 q03_data_byte;
extern void q03_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern void q03_init(VI_PIPE ViPipe);
extern void q03_exit(VI_PIPE ViPipe);
extern void q03_standby(VI_PIPE ViPipe);
extern void q03_restart(VI_PIPE ViPipe);
extern int  q03_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  q03_read_register(VI_PIPE ViPipe, int addr);
extern int  q03_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __Q03_CMOS_EX_H_ */
