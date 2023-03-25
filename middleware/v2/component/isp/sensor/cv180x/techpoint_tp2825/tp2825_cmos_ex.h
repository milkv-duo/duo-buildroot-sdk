#ifndef __tp2825_CMOS_EX_H_
#define __tp2825_CMOS_EX_H_

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
typedef enum _tp2825_MODE_E {
	TP2825_MODE_1440P_25P,
	TP2825_MODE_1440P_30P,
	TP2825_MODE_NUM
} tp2825_MODE_E;

typedef struct _tp2825_MODE_S {
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
} tp2825_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pasttp2825[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_auntp2825_BusInfo[];
extern const CVI_U8 tp2825_i2c_addr;
extern const CVI_U32 tp2825_addr_byte;
extern const CVI_U32 tp2825_data_byte;
extern void tp2825_init(VI_PIPE ViPipe);
extern void tp2825_exit(VI_PIPE ViPipe);
extern void tp2825_standby(VI_PIPE ViPipe);
extern void tp2825_restart(VI_PIPE ViPipe);
extern int  tp2825_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  tp2825_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __tp2825_CMOS_EX_H_ */
