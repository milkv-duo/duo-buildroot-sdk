#ifndef __SC401AI_CMOS_EX_H_
#define __SC401AI_CMOS_EX_H_

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


enum sc401ai_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_ADDR,
	LINEAR_A_FINEGAIN_ADDR,
	LINEAR_DGAIN_ADDR,
	LINEAR_D_FINEGAIN_ADDR,
	LINEAR_VMAX_0_ADDR,
	LINEAR_VMAX_1_ADDR,
	LINEAR_REGS_NUM
};

typedef enum _SC401AI_MODE_E {
	SC401AI_MODE_1440P30 = 0,
	SC401AI_MODE_1296P30,
	SC401AI_MODE_NUM
} SC401AI_MODE_E;

typedef struct _SC401AI_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_LARGE_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} SC401AI_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC401AI[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC401AI_BusInfo[];
extern CVI_U16 g_au16SC401AI_GainMode[];
extern CVI_U16 g_au16SC401AI_L2SMode[];
extern CVI_U8 sc401ai_i2c_addr;
extern const CVI_U32 sc401ai_addr_byte;
extern const CVI_U32 sc401ai_data_byte;
extern void sc401ai_init(VI_PIPE ViPipe);
extern void sc401ai_exit(VI_PIPE ViPipe);
extern void sc401ai_standby(VI_PIPE ViPipe);
extern void sc401ai_restart(VI_PIPE ViPipe);
extern int  sc401ai_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc401ai_read_register(VI_PIPE ViPipe, int addr);
extern void sc401ai_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  sc401ai_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC401AI_CMOS_EX_H_ */
