#ifndef __LT6911_CMOS_EX_H_
#define __LT6911_CMOS_EX_H_

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

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif


enum lt6911_linear_regs_e {
	LINEAR_REGS_NUM
};


typedef enum _LT6911_MODE_E {
	LT6911_MODE_NONE,
	LT6911_MODE_NORMAL,
	LT6911_MODE_NUM
} LT6911_MODE_E;


typedef struct _LT6911_MODE_S {
	ISP_WDR_SIZE_S astImg[2];
	char name[64];
} LT6911_MODE_S;


extern CVI_U8 lt6911_i2c_addr;
extern const CVI_U32 lt6911_addr_byte;
extern const CVI_U32 lt6911_data_byte;
extern void lt6911_init(VI_PIPE ViPipe);
extern void lt6911_exit(VI_PIPE ViPipe);
extern void lt6911_standby(VI_PIPE ViPipe);
extern void lt6911_restart(VI_PIPE ViPipe);
extern int  lt6911_write(VI_PIPE ViPipe, int addr, int data);
extern int  lt6911_read(VI_PIPE ViPipe, int addr);
extern void lt6911_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
extern int  lt6911_probe(VI_PIPE ViPipe);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __LT6911_CMOS_EX_H_ */