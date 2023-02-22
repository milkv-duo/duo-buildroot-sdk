#ifndef __N5_CMOS_EX_H_
#define __N5_CMOS_EX_H_

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

typedef enum _N5_MODE_E {
	N5_MODE_H720_NT = 1,
	N5_MODE_H720_PAL,
	N5_MODE_720P_30P,
	N5_MODE_720P_25P,
	N5_MODE_960P_30P,
	N5_MODE_960P_25P,
	N5_MODE_1080P_30P,
	N5_MODE_1080P_25P,
	N5_MODE_1080P_60P,
	N5_MODE_1080P_50P,
	N5_MODE_4M_30P,
	N5_MODE_4M_25P,
	N5_MODE_8M_15P,
	N5_MODE_8M_12_5P,
	N5_MODE_NUM
} N5_MODE_E;

typedef enum _n5_outmode_sel {
	N5_OUTMODE_1MUX_SD = 0,
	N5_OUTMODE_1MUX_HD,
	N5_OUTMODE_1MUX_FHD,
	N5_OUTMODE_1MUX_FHD_HALF,
	N5_OUTMODE_2MUX_SD,
	N5_OUTMODE_2MUX_HD,
	N5_OUTMODE_2MUX_FHD,
	N5_OUTMODE_1MUX_BT1120S,
	N5_OUTMODE_2MUX_BT1120S_720P,
	N5_OUTMODE_2MUX_BT1120S_1080P,
	N5_OUTMODE_BUTT
} N5_OUTMODE_SEL;

typedef struct _N5_MODE_S {
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
} N5_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastN5[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunN5_BusInfo[];
extern const CVI_U8 n5_i2c_addr;
extern const CVI_U32 n5_addr_byte;
extern const CVI_U32 n5_data_byte;
extern void n5_init(VI_PIPE ViPipe);
extern void n5_exit(VI_PIPE ViPipe);
extern void n5_standby(VI_PIPE ViPipe);
extern void n5_restart(VI_PIPE ViPipe);
extern int  n5_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  n5_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __N5_CMOS_EX_H_ */
