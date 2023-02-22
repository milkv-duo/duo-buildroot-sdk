#ifndef __SC200AI_CMOS_EX_H_
#define __SC200AI_CMOS_EX_H_

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


enum sc200ai_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_0_ADDR,
	LINEAR_AGAIN_1_ADDR,
	LINEAR_DGAIN_0_ADDR,
	LINEAR_DGAIN_1_ADDR,
	LINEAR_VMAX_0_ADDR,
	LINEAR_VMAX_1_ADDR,
	LINEAR_REGS_NUM
};

enum sc200ai_dol2_regs_e {
	WDR2_SHS1_0_ADDR,
	WDR2_SHS1_1_ADDR,
	WDR2_SHS1_2_ADDR,
	WDR2_SHS2_0_ADDR,
	WDR2_SHS2_1_ADDR,
	WDR2_SHS2_2_ADDR,
	WDR2_AGAIN1_0_ADDR,
	WDR2_AGAIN1_1_ADDR,
	WDR2_DGAIN1_0_ADDR,
	WDR2_DGAIN1_1_ADDR,
	WDR2_AGAIN2_0_ADDR,
	WDR2_AGAIN2_1_ADDR,
	WDR2_DGAIN2_0_ADDR,
	WDR2_DGAIN2_1_ADDR,
	WDR2_VMAX_0_ADDR,
	WDR2_VMAX_1_ADDR,
	WDR2_MAXSEXP_0_ADDR,
	WDR2_MAXSEXP_1_ADDR,
	WDR2_REGS_NUM
};

typedef enum _SC200AI_MODE_E {
	SC200AI_MODE_1080P30 = 0,
	SC200AI_MODE_LINEAR_NUM,
	SC200AI_MODE_1080P30_WDR = SC200AI_MODE_LINEAR_NUM,
	SC200AI_MODE_NUM
} SC200AI_MODE_E;

typedef struct _SC200AI_STATE_S {
	CVI_U32		u32Sexp_MAX;	/* (2*{16’h3e23,16’h3e24} – 'd10)/2 */
} SC200AI_STATE_S;

typedef struct _SC200AI_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	CVI_U16 u16SexpMaxReg;		/* {16’h3e23,16’h3e24} */
	char name[64];
} SC200AI_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC200AI[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC200AI_BusInfo[];
extern CVI_U16 g_au16SC200AI_GainMode[];
extern CVI_U16 g_au16SC200AI_L2SMode[];
extern const CVI_U8 sc200ai_i2c_addr;
extern const CVI_U32 sc200ai_addr_byte;
extern const CVI_U32 sc200ai_data_byte;
extern void sc200ai_init(VI_PIPE ViPipe);
extern void sc200ai_exit(VI_PIPE ViPipe);
extern void sc200ai_standby(VI_PIPE ViPipe);
extern void sc200ai_restart(VI_PIPE ViPipe);
extern int  sc200ai_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc200ai_read_register(VI_PIPE ViPipe, int addr);
extern void sc200ai_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  sc200ai_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC200AI_CMOS_EX_H_ */
