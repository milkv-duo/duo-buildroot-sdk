#ifndef __SC5336_2L_CMOS_EX_H_
#define __SC5336_2L_CMOS_EX_H_

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


enum SC5336_2l_linear_regs_e {
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

typedef enum _SC5336_2L_MODE_E {
	SC5336_2L_MODE_1620P30 = 0,
	SC5336_2L_MODE_1618P30 = 1,
	SC5336_2L_MODE_LINEAR_NUM,
	SC5336_2L_MODE_NUM
} SC5336_2L_MODE_E;

typedef struct _SC5336_2L_STATE_S {
	CVI_U32		u32Sexp_MAX;	/* (2*{16’h3e23,16’h3e24} – 'd10)/2 */
} SC5336_2L_STATE_S;

typedef struct _SC5336_2L_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	CVI_U16 u16SexpMaxReg;		/* {16’h3e23,16’h3e24} */
	char name[64];
} SC5336_2L_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC5336_2L[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC5336_2L_BusInfo[];
extern CVI_U16 g_au16SC5336_2L_GainMode[];
extern CVI_U16 g_au16SC5336_2L_L2SMode[];
extern const CVI_U8 SC5336_2l_i2c_addr;
extern const CVI_U32 SC5336_2l_addr_byte;
extern const CVI_U32 SC5336_2l_data_byte;
extern void SC5336_2l_init(VI_PIPE ViPipe);
extern void SC5336_2l_exit(VI_PIPE ViPipe);
extern void SC5336_2l_standby(VI_PIPE ViPipe);
extern void SC5336_2l_restart(VI_PIPE ViPipe);
extern int  SC5336_2l_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  SC5336_2l_read_register(VI_PIPE ViPipe, int addr);
extern void SC5336_2l_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  SC5336_2l_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC5336_2L_CMOS_EX_H_ */
