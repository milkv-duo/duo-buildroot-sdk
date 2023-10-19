#ifndef __CVI_SNS_CTRL_H__
#define __CVI_SNS_CTRL_H__

#include <cvi_vip_cif.h>
//#include <linux/cvi_vip_cif.h>
#include "cvi_type.h"
#include "cvi_debug.h"
#include "cvi_comm_3a.h"
#include "cvi_comm_isp.h"
#include "cvi_comm_vi.h"
#include "cvi_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct combo_dev_attr_s SNS_COMBO_DEV_ATTR_S;

typedef struct _SNS_ATTR_S {
	CVI_U16	u16Min;
	CVI_U16 u16Max;
	CVI_U16 u16Def;
	CVI_U16 u16Step;
} SNS_ATTR_S;

typedef struct _SNS_ATTR_LARGE_S {
	CVI_U32	u32Min;
	CVI_U32 u32Max;
	CVI_U32 u32Def;
	CVI_U32 u32Step;
} SNS_ATTR_LARGE_S;

typedef struct _ISP_SNS_STATE_S {
	CVI_BOOL     bInit;                  /* CVI_TRUE: Sensor init */
	CVI_BOOL     bSyncInit;              /* CVI_TRUE: Sync Reg init */
	CVI_U8       u8ImgMode;
	CVI_U8       u8Hdr;               /* CVI_TRUE: HDR enbale */
	WDR_MODE_E  enWDRMode;

	ISP_SNS_SYNC_INFO_S astSyncInfo[2]; /* [0]: Sensor reg info of cur-frame; [1]: Sensor reg info of pre-frame ; */

	CVI_U32      au32FL[2];              /* [0]: FullLines of cur-frame; [1]: Pre FullLines of pre-frame */
	CVI_U32      u32FLStd;               /* FullLines std */
	CVI_U32      au32WDRIntTime[4];
} ISP_SNS_STATE_S;

typedef enum _ISP_SNS_MIRRORFLIP_TYPE_E {
	ISP_SNS_NORMAL      = 0,
	ISP_SNS_MIRROR      = 1,
	ISP_SNS_FLIP        = 2,
	ISP_SNS_MIRROR_FLIP = 3,
	ISP_SNS_BUTT
} ISP_SNS_MIRRORFLIP_TYPE_E;

typedef enum _ISP_SNS_GAIN_MODE_E {
	SNS_GAIN_MODE_SHARE = 0,	/* gain setting for all wdr frames*/
	SNS_GAIN_MODE_WDR_2F,		/* separate gain for 2-frame wdr mode*/
	SNS_GAIN_MODE_WDR_3F,		/* separate gain for 3-frame wdr mode*/
	SNS_GAIN_MODE_ONLY_LEF		/* gain setting only apply to lef and sef is fixed to 1x */
} ISP_SNS_GAIN_MODE_E;

typedef enum _ISP_SNS_L2S_MODE_E {
	SNS_L2S_MODE_AUTO = 0,	/* sensor l2s distance varies by the inttime of sef. */
	SNS_L2S_MODE_FIX,	/* sensor l2s distance is fixed. */
} ISP_SNS_INTTIME_MODE_E;

typedef struct _RX_INIT_ATTR_S {
	CVI_U32 MipiDev;
	CVI_S16 as16LaneId[5];
	CVI_S8  as8PNSwap[5];
} RX_INIT_ATTR_S;

typedef struct _ISP_INIT_ATTR_S {
	CVI_U32 u32ExpTime;
	CVI_U32 u32AGain;
	CVI_U32 u32DGain;
	CVI_U32 u32ISPDGain;
	CVI_U32 u32Exposure;
	CVI_U32 u32LinesPer500ms;
	CVI_U32 u32PirisFNO;
	CVI_U16 u16WBRgain;
	CVI_U16 u16WBGgain;
	CVI_U16 u16WBBgain;
	CVI_U16 u16SampleRgain;
	CVI_U16 u16SampleBgain;
	CVI_U16 u16UseHwSync;
	ISP_SNS_GAIN_MODE_E enGainMode;
	ISP_SNS_INTTIME_MODE_E enL2SMode;
} ISP_INIT_ATTR_S;

typedef struct _ISP_SNS_OBJ_S {
	CVI_S32 (*pfnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *, ALG_LIB_S *);
	CVI_S32 (*pfnUnRegisterCallback)(VI_PIPE ViPipe, ALG_LIB_S *, ALG_LIB_S *);
	CVI_S32 (*pfnSetBusInfo)(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo);
	CVI_VOID (*pfnStandby)(VI_PIPE ViPipe);
	CVI_VOID (*pfnRestart)(VI_PIPE ViPipe);
	CVI_VOID (*pfnMirrorFlip)(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip);
	CVI_S32 (*pfnWriteReg)(VI_PIPE ViPipe, CVI_S32 s32Addr, CVI_S32 s32Data);
	CVI_S32 (*pfnReadReg)(VI_PIPE ViPipe, CVI_S32 s32Addr);
	CVI_S32 (*pfnSetInit)(VI_PIPE ViPipe, ISP_INIT_ATTR_S *);
	CVI_S32 (*pfnPatchRxAttr)(RX_INIT_ATTR_S *);
	CVI_VOID (*pfnPatchI2cAddr)(CVI_S32 s32I2cAddr);
	CVI_S32 (*pfnGetRxAttr)(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *);
	CVI_S32 (*pfnExpSensorCb)(ISP_SENSOR_EXP_FUNC_S *);
	CVI_S32 (*pfnExpAeCb)(AE_SENSOR_EXP_FUNC_S *);
} ISP_SNS_OBJ_S;

extern ISP_SNS_OBJ_S stSnsImx327_fpga_Obj;
extern ISP_SNS_OBJ_S stSnsImx327_Obj;
extern ISP_SNS_OBJ_S stSnsOs08a20_Obj;
extern ISP_SNS_OBJ_S stSnsOs08a20_Slave_Obj;
extern ISP_SNS_OBJ_S stSnsF23_Obj;
extern ISP_SNS_OBJ_S stSnsF35_Obj;
extern ISP_SNS_OBJ_S stSnsF35_Slave_Obj;
extern ISP_SNS_OBJ_S stSnsH65_Obj;
extern ISP_SNS_OBJ_S stSnsImx327_2l_Obj;
extern ISP_SNS_OBJ_S stSnsImx290_2l_Obj;
extern ISP_SNS_OBJ_S stSnsImx327_Slave_Obj;
extern ISP_SNS_OBJ_S stSnsImx327_Sublvds_Obj;
extern ISP_SNS_OBJ_S stSnsImx307_Obj;
extern ISP_SNS_OBJ_S stSnsImx307_2l_Obj;
extern ISP_SNS_OBJ_S stSnsImx307_Slave_Obj;
extern ISP_SNS_OBJ_S stSnsPICO640_Obj;
extern ISP_SNS_OBJ_S stSnsPICO384_Obj;
extern ISP_SNS_OBJ_S stSnsImx307_Sublvds_Obj;
extern ISP_SNS_OBJ_S stSnsMCS369Q_Obj;
extern ISP_SNS_OBJ_S stSnsMM308M2_Obj;
extern ISP_SNS_OBJ_S stSnsN5_Obj;
extern ISP_SNS_OBJ_S stSnsSC3335_Obj;
extern ISP_SNS_OBJ_S stSnsSC3335_Slave_Obj;
extern ISP_SNS_OBJ_S stSnsImx335_Obj;
extern ISP_SNS_OBJ_S stSnsPR2020_Obj;
extern ISP_SNS_OBJ_S stSnsPR2100_Obj;
extern ISP_SNS_OBJ_S stSnsImx334_Obj;
extern ISP_SNS_OBJ_S stSnsSC8238_Obj;
extern ISP_SNS_OBJ_S stSnsMCS369_Obj;
extern ISP_SNS_OBJ_S stSnsSC4210_Obj;
extern ISP_SNS_OBJ_S stSnsSC200AI_Obj;
extern ISP_SNS_OBJ_S stSnsN6_Obj;

#define CMOS_CHECK_POINTER(ptr)\
	do {\
		if (ptr == CVI_NULL) {\
			syslog(LOG_ERR, "Null Pointer!\n");\
			return CVI_ERR_VI_INVALID_NULL_PTR;\
		} \
	} while (0)

#define CMOS_CHECK_POINTER_VOID(ptr)\
	do {\
		if (ptr == CVI_NULL) {\
			syslog(LOG_ERR, "Null Pointer!\n");\
			return;\
		} \
	} while (0)

#define SENSOR_FREE(ptr)\
	do {\
		if (ptr != CVI_NULL) {\
			free(ptr);\
			ptr = CVI_NULL;\
		} \
	} while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_SNS_CTRL_H__ */
