#ifndef __OV4689_CMOS_EX_H_
#define __OV4689_CMOS_EX_H_

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


enum ov4689_linear_regs_e {
	LINEAR_HOLD_START = 0,
	LINEAR_EXP_0,
	LINEAR_EXP_1,
	LINEAR_EXP_2,
	LINEAR_AGAIN_0,
	LINEAR_AGAIN_1,
	LINEAR_AGAIN_2,
	LINEAR_DGAIN_0,
	LINEAR_DGAIN_1,
	LINEAR_VTS_0,
	LINEAR_VTS_1,
	LINEAR_HOLD_END,
	LINEAR_LAUNCH_0,
	LINEAR_LAUNCH_1,
	LINEAR_REGS_NUM
};

typedef enum _OV4689_MODE_E {
	OV4689_MODE_2688X1520P30 = 0,
	OV4689_MODE_LINEAR_NUM,
	OV4689_MODE_NUM
} OV4689_MODE_E;

typedef struct _OV4689_STATE_S {
	CVI_U32		u32Sexp_MAX;
} OV4689_STATE_S;

typedef struct _OV4689_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	CVI_U16 u16L2sOffset;
	CVI_U16 u16TopBoundary;
	CVI_U16 u16BotBoundary;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_LARGE_S stAgain[2];
	SNS_ATTR_LARGE_S stDgain[2];
	CVI_U32 u32L2S_offset;
	CVI_U32 u32IspResTime;
	CVI_U32 u32HdrMargin;
	char name[64];
} OV4689_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastOv4689[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunOv4689_BusInfo[];
extern CVI_U16 g_au16Ov4689_GainMode[];
extern CVI_U16 g_au16Ov4689_UseHwSync[VI_MAX_PIPE_NUM];
extern CVI_U8 ov4689_i2c_addr;
extern const CVI_U32 ov4689_addr_byte;
extern const CVI_U32 ov4689_data_byte;
extern void ov4689_init(VI_PIPE ViPipe);
extern void ov4689_exit(VI_PIPE ViPipe);
extern void ov4689_standby(VI_PIPE ViPipe);
extern void ov4689_restart(VI_PIPE ViPipe);
extern int  ov4689_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  ov4689_read_register(VI_PIPE ViPipe, int addr);
extern void ov4689_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int ov4689_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __OV4689_CMOS_EX_H_ */
