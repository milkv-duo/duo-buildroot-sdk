#ifndef __SC301IOT_CMOS_EX_H_
#define __SC301IOT_CMOS_EX_H_

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


enum sc301iot_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_ADDR,
	LINEAR_DGAIN_0_ADDR,
	LINEAR_DGAIN_1_ADDR,
	LINEAR_VMAX_0_ADDR,
	LINEAR_VMAX_1_ADDR,
	LINEAR_REGS_NUM
};

typedef enum _SC301IOT_MODE_E {
	SC301IOT_MODE_1536P30 = 0,
	SC301IOT_MODE_NUM
} SC301IOT_MODE_E;

typedef struct _SC301IOT_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	char name[64];
} SC301IOT_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC301IOT[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC301IOT_BusInfo[];
extern CVI_U16 g_au16SC301IOT_GainMode[];
extern CVI_U16 g_au16SC301IOT_L2SMode[];
extern CVI_U8 sc301iot_i2c_addr;
extern const CVI_U32 sc301iot_addr_byte;
extern const CVI_U32 sc301iot_data_byte;
extern void sc301iot_init(VI_PIPE ViPipe);
extern void sc301iot_exit(VI_PIPE ViPipe);
extern void sc301iot_standby(VI_PIPE ViPipe);
extern void sc301iot_restart(VI_PIPE ViPipe);
extern int  sc301iot_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc301iot_read_register(VI_PIPE ViPipe, int addr);
extern void sc301iot_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  sc301iot_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC301IOT_CMOS_EX_H_ */
