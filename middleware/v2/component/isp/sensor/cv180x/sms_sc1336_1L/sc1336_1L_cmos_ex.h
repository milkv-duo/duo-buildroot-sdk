#ifndef __SC1336_1L_CMOS_EX_H_
#define __SC1336_1L_CMOS_EX_H_

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

enum sc1336_1l_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_ADDR0,
	LINEAR_AGAIN_ADDR1,
	LINEAR_AGAIN_FINE_ADDR,
	LINEAR_DGAIN_ADDR,
	LINEAR_DGAIN_FINE_ADDR,
	LINEAR_FLIP_MIRROR,
	LINEAR_VMAX_H_ADDR,
	LINEAR_VMAX_L_ADDR,
	LINEAR_REGS_NUM
};

enum sc1336_1l_dol2_regs_e {
	WDR_SHS1_0_ADDR,
	WDR_SHS1_1_ADDR,
	WDR_SHS1_2_ADDR,
	WDR_SHS2_0_ADDR,
	WDR_SHS2_1_ADDR,
	WDR_SHS2_2_ADDR,
	WDR_AGAIN1_ADDR0,
	WDR_AGAIN1_ADDR1,
	WDR_AGAIN1_FINE_ADDR,
	WDR_AGAIN2_ADDR0,
	WDR_AGAIN2_ADDR1,
	WDR_AGAIN2_FINE_ADDR,
	WDR_DGAIN1_ADDR,
	WDR_DGAIN1_FINE_ADDR,
	WDR_DGAIN2_ADDR,
	WDR_DGAIN2_FINE_ADDR,
	WDR_FLIP_MIRROR,
	WDR_VMAX_H_ADDR,
	WDR_VMAX_L_ADDR,
	WDR_REGS_NUM
};

typedef enum _SC1336_1L_MODE_E {
	SC1336_1L_MODE_720P30 = 0,
	SC1336_1L_MODE_720P60,
	SC1336_1L_MODE_LINEAR_NUM,
	SC1336_1L_MODE_720P30_WDR = SC1336_1L_MODE_LINEAR_NUM,
	SC1336_1L_MODE_720P60_WDR,
	SC1336_1L_MODE_NUM
} SC1336_1L_MODE_E;

typedef struct _SC1336_1L_STATE_S {
	CVI_U32		u32Sexp_MAX;	/* (2*{16’h3e23,16’h3e24} – 'd10)/2 */
} SC1336_1L_STATE_S;

typedef struct _SC1336_1L_MODE_S {
	ISP_WDR_SIZE_S stImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp;
	SNS_ATTR_LARGE_S stAgain;
	SNS_ATTR_LARGE_S stDgain;
	CVI_U16 u16SexpMaxReg;		/* {16’h3e23,16’h3e24} */
	char name[64];
} SC1336_1L_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC1336_1L[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC1336_1L_BusInfo[];
extern CVI_U16 g_au16SC1336_1L_GainMode[];
extern CVI_U16 g_au16SC1336_1L_L2SMode[];
extern const CVI_U8 sc1336_1l_i2c_addr;
extern const CVI_U32 sc1336_1l_addr_byte;
extern const CVI_U32 sc1336_1l_data_byte;
extern void sc1336_1l_init(VI_PIPE ViPipe);
extern void sc1336_1l_exit(VI_PIPE ViPipe);
extern void sc1336_1l_standby(VI_PIPE ViPipe);
extern void sc1336_1l_restart(VI_PIPE ViPipe);
extern int  sc1336_1l_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc1336_1l_read_register(VI_PIPE ViPipe, int addr);
extern int  sc1336_1l_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC1336_1L_CMOS_EX_H_ */
