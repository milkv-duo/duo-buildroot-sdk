#ifndef __F23_CMOS_EX_H_
#define __F23_CMOS_EX_H_

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


enum f23_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_0_DATA,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_1_DATA,
	LINEAR_AGAIN_ADDR,
	LINEAR_AGAIN_DATA,
	LINEAR_DGAIN_ADDR,
	LINEAR_DGAIN_DATA,
	LINEAR_VMAX_0_ADDR,
	LINEAR_VMAX_0_DATA,
	LINEAR_VMAX_1_ADDR,
	LINEAR_VMAX_1_DATA,
	LINEAR_REL,
	LINEAR_REGS_NUM
};

enum f23_dol2_regs_e {
	WDR2_SHS1_0_ADDR,
	WDR2_SHS1_0_DATA,
	WDR2_SHS1_1_ADDR,
	WDR2_SHS1_1_DATA,
	WDR2_SHS2_ADDR,
	WDR2_SHS2_DATA,
	WDR2_AGAIN_ADDR,
	WDR2_AGAIN_DATA,
	WDR2_DGAIN_ADDR,
	WDR2_DGAIN_DATA,
	WDR2_VMAX_0_ADDR,
	WDR2_VMAX_0_DATA,
	WDR2_VMAX_1_ADDR,
	WDR2_VMAX_1_DATA,
	WDR2_REL,
	WDR2_REGS_NUM
};

typedef enum _F23_MODE_E {
	F23_MODE_1080P30 = 0,
	F23_MODE_LINEAR_NUM,
	F23_MODE_1080P30_WDR = F23_MODE_LINEAR_NUM,
	F23_MODE_NUM
} F23_MODE_E;

typedef struct _F23_STATE_S {
	CVI_U32		u8SexpReg;
	CVI_U32		u32Sexp_MAX;
} F23_STATE_S;

typedef struct _F23_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	CVI_U8 u8DgainReg;
	char name[64];
} F23_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastF23[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunF23_BusInfo[];
extern CVI_U16 g_au16F23_GainMode[];
extern const CVI_U8 f23_i2c_addr;
extern const CVI_U32 f23_addr_byte;
extern const CVI_U32 f23_data_byte;
extern void f23_init(VI_PIPE ViPipe);
extern void f23_exit(VI_PIPE ViPipe);
extern void f23_standby(VI_PIPE ViPipe);
extern void f23_restart(VI_PIPE ViPipe);
extern int  f23_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  f23_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __F23_CMOS_EX_H_ */
