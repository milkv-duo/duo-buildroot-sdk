#ifndef __BG0808_CMOS_EX_H_
#define __BG0808_CMOS_EX_H_

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


enum bg0808_linear_regs_e {
	LINEAR_EXP_H_ADDR,//0x0c
	LINEAR_EXP_L_ADDR,//0x0d
	LINEAR_AGAIN_ADDR,//0x00a2
	LINEAR_CLAMP_MODE,//0x73  need change when again changed
	LINEAR_DGAIN_H_ADDR,//0x00c0
	LINEAR_DGAIN_L_ADDR,//0x00c1
	LINEAR_VMAX_H_ADDR,//0x0010
	LINEAR_VMAX_L_ADDR,//0x0011
	LINEAR_SHADOW_ADDR,//0x001d
	LINEAR_REGS_NUM
};
enum bg0808_wdr_regs_e {
	WDR_EXP_H_ADDR,
	WDR_EXP_L_ADDR,
	WDR_SEXP_H_ADDR,//0x22
	WDR_SEXP_L_ADDR,//0x23
	WDR_AGAIN_ADDR,//0x00a2
	WDR_SAGAIN_ADDR,//0x00a4
	WDR_CLAMP_MODE,//0x73  need change when again changed
	WDR_DGAIN_H_ADDR,
	WDR_DGAIN_L_ADDR,
	WDR_SDGAIN_H_ADDR,//0x00c2
	WDR_SDGAIN_L_ADDR,//0x00c3
	WDR_VMAX_H_ADDR,
	WDR_VMAX_L_ADDR,
	WDR_SHADOW_ADDR,
	WDR_REGS_NUM
};

typedef enum _BG0808_MODE_E {
	BG0808_MODE_1920X1080P30 = 0,
	BG0808_MODE_LINEAR_NUM,
	BG0808_MODE_1920X1080P30_WDR = BG0808_MODE_LINEAR_NUM,
	BG0808_MODE_NUM
} BG0808_MODE_E;

typedef struct _BG0808_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	CVI_U32 u32IspResTime;
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	char name[64];
} BG0808_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastBG0808[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunBG0808_BusInfo[];
extern CVI_U16 g_au16BG0808_GainMode[];
extern CVI_U16 g_au16BG0808_L2SMode[];
extern CVI_U8 bg0808_i2c_addr;
extern const CVI_U32 bg0808_addr_byte;
extern const CVI_U32 bg0808_data_byte;
extern void bg0808_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern void bg0808_init(VI_PIPE ViPipe);
extern void bg0808_exit(VI_PIPE ViPipe);
extern void bg0808_standby(VI_PIPE ViPipe);
extern void bg0808_restart(VI_PIPE ViPipe);
extern int  bg0808_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  bg0808_read_register(VI_PIPE ViPipe, int addr);
extern int  bg0808_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __BG0808_CMOS_EX_H_ */
