#ifndef __N6_CMOS_EX_H_
#define __N6_CMOS_EX_H_

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

typedef enum _N6_MODE_E {
	N6_MODE_H720_NT = 1,
	N6_MODE_H720_PAL,
	N6_MODE_720P_30P,
	N6_MODE_720P_25P,
	N6_MODE_960P_30P,
	N6_MODE_960P_25P,
	N6_MODE_1080P_30P,
	N6_MODE_1080P_25P,
	N6_MODE_1080P_60P,
	N6_MODE_1080P_50P,
	N6_MODE_4M_30P,
	N6_MODE_4M_25P,
	N6_MODE_8M_15P,
	N6_MODE_8M_12_5P,
	N6_MODE_NUM
} N6_MODE_E;

typedef enum _n6_outmode_sel {
	N6_OUTMODE_1MUX_SD = 0,
	N6_OUTMODE_1MUX_HD,
	N6_OUTMODE_1MUX_FHD,
	N6_OUTMODE_1MUX_FHD_HALF,
	N6_OUTMODE_2MUX_SD,
	N6_OUTMODE_2MUX_HD,
	N6_OUTMODE_2MUX_FHD,
	N6_OUTMODE_1MUX_BT1120S,
	N6_OUTMODE_2MUX_BT1120S_720P,
	N6_OUTMODE_2MUX_BT1120S_1080P,
	N6_OUTMODE_BUTT
} N6_OUTMODE_SEL;

typedef struct _N6_MODE_S {
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
} N6_MODE_S;

#define _MIPI_PORT0_
//#define _MIPI_PORT1_

#ifdef _MIPI_PORT0_
#define _MAR_BANK_ 0x20
#define _MTX_BANK_ 0x23
#else
#define _MAR_BANK_ 0x30
#define _MTX_BANK_ 0x33
#endif

#define CH0_NO		0
#define CH1_NO		1
#define NTPAL		0

#define VFMT_NTSC	0
#define VFMT_PAL	1

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastN6[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunN6_BusInfo[];
extern const CVI_U8 n6_i2c_addr;
extern const CVI_U32 n6_addr_byte;
extern const CVI_U32 n6_data_byte;
extern void n6_init(VI_PIPE ViPipe);
extern void n6_exit(VI_PIPE ViPipe);
extern void n6_standby(VI_PIPE ViPipe);
extern void n6_restart(VI_PIPE ViPipe);
extern int  n6_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  n6_read_register(VI_PIPE ViPipe, int addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __N6_CMOS_EX_H_ */
