#ifndef __MIS2008_1L_CMOS_EX_H_
#define __MIS2008_1L_CMOS_EX_H_

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


enum mis2008_1l_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_ADDR,
	LINEAR_DGAIN_ADDR,
	LINEAR_D_FINEGAIN_ADDR,
	LINEAR_VMAX_0_ADDR,
	LINEAR_VMAX_1_ADDR,
	LINEAR_FLIP_MIRROR_ADDR,
	LINEAR_REGS_NUM
};

typedef enum _MIS2008_1L_MODE_E {
	MIS2008_1L_MODE_1080P30 = 0,
	MIS2008_1L_MODE_NUM
} MIS2008_1L_MODE_E;

typedef struct _MIS2008_1L_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_LARGE_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} MIS2008_1L_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastMIS2008_1L[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunMIS2008_1L_BusInfo[];
extern CVI_U16 g_au16MIS2008_1L_GainMode[];
extern CVI_U16 g_au16MIS2008_1L_L2SMode[];
extern CVI_U8 mis2008_1l_i2c_addr;
extern const CVI_U32 mis2008_1l_addr_byte;
extern const CVI_U32 mis2008_1l_data_byte;
extern void mis2008_1l_init(VI_PIPE ViPipe);
extern void mis2008_1l_exit(VI_PIPE ViPipe);
extern void mis2008_1l_standby(VI_PIPE ViPipe);
extern void mis2008_1l_restart(VI_PIPE ViPipe);
extern int  mis2008_1l_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  mis2008_1l_read_register(VI_PIPE ViPipe, int addr);
extern void mis2008_1l_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  mis2008_1l_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __MIS2008_1L_CMOS_EX_H_ */