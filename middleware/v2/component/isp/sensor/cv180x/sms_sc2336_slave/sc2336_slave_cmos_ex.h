#ifndef __SC2336_SLAVE_CMOS_EX_H_
#define __SC2336_SLAVE_CMOS_EX_H_

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


enum sc2336_slave_linear_regs_e {
	LINEAR_SHS1_0_ADDR,
	LINEAR_SHS1_1_ADDR,
	LINEAR_SHS1_2_ADDR,
	LINEAR_AGAIN_0_ADDR,
	LINEAR_DGAIN_0_ADDR,
	LINEAR_DGAIN_1_ADDR,
	LINEAR_VMAX_0_ADDR,
	LINEAR_VMAX_1_ADDR,
	LINEAR_REGS_NUM
};


typedef enum _SC2336_SLAVE_MODE_E {
	SC2336_SLAVE_MODE_1080P30 = 0,
	SC2336_SLAVE_MODE_LINEAR_NUM,
	SC2336_SLAVE_MODE_NUM
} SC2336_SLAVE_MODE_E;

typedef struct _SC2336_SLAVE_STATE_S {
	CVI_U32		u32Sexp_MAX;
} SC2336_SLAVE_STATE_S;

typedef struct _SC2336_SLAVE_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	CVI_FLOAT f32MaxFps;
	CVI_FLOAT f32MinFps;
	CVI_U32 u32HtsDef;
	CVI_U32 u32VtsDef;
	SNS_ATTR_S stExp[2];
	SNS_ATTR_S stAgain[2];
	SNS_ATTR_S stDgain[2];
	CVI_U16 u16SexpMaxReg;
	char name[64];
} SC2336_SLAVE_MODE_S;

/****************************************************************************
 * external variables and functions                                         *
 ****************************************************************************/

extern ISP_SNS_STATE_S *g_pastSC2336_Slave[VI_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U g_aunSC2336_Slave_BusInfo[];
extern CVI_U16 g_au16SC2336_Slave_GainMode[];
extern CVI_U16 g_au16SC2336_Slave_L2SMode[];
extern const CVI_U8 sc2336_slave_i2c_addr;
extern const CVI_U32 sc2336_slave_addr_byte;
extern const CVI_U32 sc2336_slave_data_byte;
extern void sc2336_slave_init(VI_PIPE ViPipe);
extern void sc2336_slave_exit(VI_PIPE ViPipe);
extern void sc2336_slave_standby(VI_PIPE ViPipe);
extern void sc2336_slave_restart(VI_PIPE ViPipe);
extern int  sc2336_slave_write_register(VI_PIPE ViPipe, int addr, int data);
extern int  sc2336_slave_read_register(VI_PIPE ViPipe, int addr);
extern void sc2336_slave_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  sc2336_slave_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __SC2336_SLAVE_CMOS_EX_H_ */
