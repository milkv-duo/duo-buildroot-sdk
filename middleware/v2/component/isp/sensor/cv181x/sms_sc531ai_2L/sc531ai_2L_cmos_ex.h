#ifndef __SC531AI_2L_CMOS_EX_H_
#define __SC531AI_2L_CMOS_EX_H_

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


enum sc531ai_2l_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_ADDR,
	LINEAR_DGAIN_ADDR,
	LINEAR_DGAIN_FINE_ADDR,
	LINEAR_FLIP_MIRROR,
	LINEAR_VMAX_H_ADDR,
	LINEAR_VMAX_L_ADDR,
	LINEAR_REGS_NUM
};

typedef enum _sc531AI_2L_MODE_E {
	sc531AI_2L_MODE_1620P30 = 0,
	sc531AI_2L_MODE_LINEAR_NUM,
	sc531AI_2L_MODE_NUM
} sc531AI_2L_MODE_E;

typedef struct _sc531AI_2L_STATE_S {
	CVI_U32		u32Sexp_MAX;
} sc531AI_2L_STATE_S;

typedef struct _sc531AI_2L_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	CVI_U16 u16SexpMaxReg;
	char name[64];
} sc531AI_2L_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastsc531AI_2L[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunsc531AI_2L_BusInfo[];
extern CVI_U16 g_au16sc531AI_2L_GainMode[];
extern CVI_U16 g_au16sc531AI_2L_L2SMode[];
extern const CVI_U8 sc531ai_2l_i2c_addr;
extern const CVI_U32 sc531ai_2l_addr_byte;
extern const CVI_U32 sc531ai_2l_data_byte;
extern void sc531ai_2l_init(VI_PIPE ViPipe);
extern void sc531ai_2l_exit(VI_PIPE ViPipe);
extern void sc531ai_2l_standby(VI_PIPE ViPipe);
extern void sc531ai_2l_restart(VI_PIPE ViPipe);
extern int  sc531ai_2l_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc531ai_2l_read_register(VI_PIPE ViPipe, int addr);
extern int  sc531ai_2l_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __sc531AI_2L_CMOS_EX_H_ */
