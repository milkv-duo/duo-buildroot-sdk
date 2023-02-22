#ifndef __F37P_CMOS_EX_H_
#define __F37P_CMOS_EX_H_

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

enum f37p_linear_regs_e {
	LINEAR_BLC_CTRL_CLOSE,
	LINEAR_EXP_H_DATA,
	LINEAR_EXP_L_DATA,
	LINEAR_AGAIN_DATA,
	LINEAR_VMAX_H_DATA,
	LINEAR_VMAX_L_DATA,
	LINEAR_BLC_CTRL_OPEN,
	LINEAR_REGS_NUM
};

typedef enum _F37P_MODE_E {
	F37P_MODE_1080P30 = 0,
	F37P_MODE_LINEAR_NUM,
	F37P_MODE_NUM
} F37P_MODE_E;

typedef struct _F37P_MODE_S {
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
} F37P_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastF37P[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunF37P_BusInfo[];
extern CVI_U8 f37p_i2c_addr;
extern const CVI_U32 f37p_addr_byte;
extern const CVI_U32 f37p_data_byte;
extern void f37p_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern void f37p_init(VI_PIPE ViPipe);
extern void f37p_exit(VI_PIPE ViPipe);
extern void f37p_standby(VI_PIPE ViPipe);
extern void f37p_restart(VI_PIPE ViPipe);
extern int  f37p_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  f37p_read_register(VI_PIPE ViPipe, int addr);
extern int  f37p_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __F37P_CMOS_EX_H_ */
