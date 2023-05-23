#ifndef __SC2331_1L_CMOS_EX_H_
#define __SC2331_1L_CMOS_EX_H_

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


enum sc2331_1L_linear_regs_e {
	LINEAR_EXP_H_ADDR,
	LINEAR_EXP_M_ADDR,
	LINEAR_EXP_L_ADDR,
	LINEAR_AGAIN_H_ADDR,
	LINEAR_AGAIN_L_ADDR,
	LINEAR_DGAIN_H_ADDR,
	LINEAR_DGAIN_L_ADDR,
	LINEAR_VMAX_H_ADDR,
	LINEAR_VMAX_L_ADDR,
	LINEAR_REGS_NUM
};

typedef enum _SC2331_1L_MODE_E {
	SC2331_1L_MODE_1920X1080P30 = 0,
	SC2331_1L_MODE_LINEAR_NUM,
	SC2331_1L_MODE_NUM
} SC2331_1L_MODE_E;

typedef struct _SC2331_1L_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	char name[64];
} SC2331_1L_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC2331_1L[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC2331_1L_BusInfo[];
extern CVI_U16 g_au16SC2331_1L_GainMode[];
extern CVI_U16 g_au16SC2331_1L_L2SMode[];
extern const CVI_U8 sc2331_1L_i2c_addr;
extern const CVI_U32 sc2331_1L_addr_byte;
extern const CVI_U32 sc2331_1L_data_byte;
extern void sc2331_1L_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern void sc2331_1L_init(VI_PIPE ViPipe);
extern void sc2331_1L_exit(VI_PIPE ViPipe);
extern void sc2331_1L_standby(VI_PIPE ViPipe);
extern void sc2331_1L_restart(VI_PIPE ViPipe);
extern int  sc2331_1L_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc2331_1L_read_register(VI_PIPE ViPipe, int addr);
extern int  sc2331_1L_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC2331_1L_CMOS_EX_H_ */
