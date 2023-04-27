#ifndef __TP2863_CMOS_EX_H_
#define __TP2863_CMOS_EX_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
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

typedef enum _TP2863_MODE_E
{
	TP2863_MODE_1080P_30P,
	TP2863_MODE_1080P_25P,
	TP2863_MODE_720P_30P,
	TP2863_MODE_720P_25P,
	TP2863_MODE_NUM
} TP2863_MODE_E;
typedef struct _TP2863_MODE_S
{
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
} TP2863_MODE_S;
/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/
extern ISP_SNS_STATE_S *g_pastTP2863[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunTP2863_BusInfo[];
extern const CVI_U8 tp2863_i2c_addr;
extern const CVI_U32 tp2863_addr_byte;
extern const CVI_U32 tp2863_data_byte;
extern void tp2863_init(VI_PIPE ViPipe);
extern void tp2863_exit(VI_PIPE ViPipe);
extern void tp2863_standby(VI_PIPE ViPipe);
extern void tp2863_restart(VI_PIPE ViPipe);
extern int tp2863_write_register(VI_PIPE ViPipe, int addr, int data);
extern int tp2863_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __TP2863_CMOS_EX_H_ */
