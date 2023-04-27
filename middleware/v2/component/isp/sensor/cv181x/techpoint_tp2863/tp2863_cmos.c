#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>
#include <errno.h>
#ifdef ARCH_CV182X
#include "cvi_type.h"
#include "cvi_comm_video.h"
#include <linux/cvi_vip_snsr.h>
#else
#include <linux/cvi_type.h>
#include <linux/cvi_comm_video.h>
#include <linux/vi_snsr.h>
#endif
#include "cvi_debug.h"
#include "cvi_comm_sns.h"
#include "cvi_sns_ctrl.h"
#include "cvi_ae_comm.h"
#include "cvi_awb_comm.h"
#include "cvi_ae.h"
#include "cvi_awb.h"
#include "cvi_isp.h"

#include "tp2863_cmos_ex.h"
#include "tp2863_cmos_param.h"

/****************************************************************************
 * global variables                                                         *
 ****************************************************************************/

ISP_SNS_COMMBUS_U g_aunTP2863_BusInfo[VI_MAX_PIPE_NUM] = {
	[0] = {.s8I2cDev = 0},
	[1 ... VI_MAX_PIPE_NUM - 1] = {.s8I2cDev = -1}};

ISP_SNS_STATE_S *g_pastTP2863[VI_MAX_PIPE_NUM] = {CVI_NULL};

#define TP2863_SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_pastTP2863[dev])
#define TP2863_SENSOR_SET_CTX(dev, pstCtx) (g_pastTP2863[dev] = pstCtx)
#define TP2863_SENSOR_RESET_CTX(dev) (g_pastTP2863[dev] = CVI_NULL)

#define TP2863_RES_IS_720P_25(w, h, f)      ((w) == 1280 && (h) == 720 && (f) == 25)
#define TP2863_RES_IS_720P_30(w, h, f)      ((w) == 1280 && (h) == 720 && (f) == 30)
#define TP2863_RES_IS_1080P_25(w, h, f)     ((w) == 1920 && (h) == 1080 && (f) == 25)
#define TP2863_RES_IS_1080P_30(w, h, f)     ((w) == 1920 && (h) == 1080 && (f) == 30)
#define TP2863_ID 0x2863
/****************************************************************************
 * local variables and functions                                            *
 ****************************************************************************/
static CVI_S32 cmos_get_wdr_size(VI_PIPE ViPipe, ISP_SNS_ISP_INFO_S *pstIspCfg)
{
	const TP2863_MODE_S *pstMode = CVI_NULL;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	TP2863_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstMode = &g_astTP2863_mode[pstSnsState->u8ImgMode];
	pstIspCfg->frm_num = 1;
	memcpy(&pstIspCfg->img_size[0], &pstMode->astImg[0], sizeof(ISP_WDR_SIZE_S));

	return CVI_SUCCESS;
}

static CVI_S32 cmos_get_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_SYNC_INFO_S *pstSnsSyncInfo)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;
	ISP_SNS_SYNC_INFO_S *pstCfg0 = CVI_NULL;

	CMOS_CHECK_POINTER(pstSnsSyncInfo);
	TP2863_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	pstCfg0 = &pstSnsState->astSyncInfo[0];
	cmos_get_wdr_size(ViPipe, &pstCfg0->ispCfg);
	memcpy(pstSnsSyncInfo, &pstSnsState->astSyncInfo[0], sizeof(ISP_SNS_SYNC_INFO_S));

	return CVI_SUCCESS;
}

static CVI_S32 cmos_set_image_mode(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{
	CVI_U8 u8SensorImageMode = 0;
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	CMOS_CHECK_POINTER(pstSensorImageMode);
	TP2863_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);

	u8SensorImageMode = pstSnsState->u8ImgMode;

	if (pstSensorImageMode->f32Fps <= 30)
	{
		if (pstSnsState->enWDRMode == WDR_MODE_NONE) {
			if (TP2863_RES_IS_720P_25(pstSensorImageMode->u16Width,
						pstSensorImageMode->u16Height,
						pstSensorImageMode->f32Fps)) {
				u8SensorImageMode = TP2863_MODE_720P_25P;
			} else if (TP2863_RES_IS_720P_30(pstSensorImageMode->u16Width,
						pstSensorImageMode->u16Height,
						pstSensorImageMode->f32Fps)) {
				u8SensorImageMode = TP2863_MODE_720P_30P;
			} else if (TP2863_RES_IS_1080P_25(pstSensorImageMode->u16Width,
						pstSensorImageMode->u16Height,
						pstSensorImageMode->f32Fps)) {
				u8SensorImageMode = TP2863_MODE_1080P_25P;
			} else if (TP2863_RES_IS_1080P_30(pstSensorImageMode->u16Width,
						pstSensorImageMode->u16Height,
						pstSensorImageMode->f32Fps)) {
				u8SensorImageMode = TP2863_MODE_1080P_30P;
			} else {
				CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
				       pstSensorImageMode->u16Width,
				       pstSensorImageMode->u16Height,
				       pstSensorImageMode->f32Fps,
				       pstSnsState->enWDRMode);
				return CVI_FAILURE;
			}
		}
		else {
			CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
			       pstSensorImageMode->u16Width,
			       pstSensorImageMode->u16Height,
			       pstSensorImageMode->f32Fps,
			       pstSnsState->enWDRMode);
			return CVI_FAILURE;
		}
	}
	else
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "Not support! Width:%d, Height:%d, Fps:%f, WDRMode:%d\n",
			pstSensorImageMode->u16Width,
			pstSensorImageMode->u16Height,
			pstSensorImageMode->f32Fps,
			pstSnsState->enWDRMode);
		return CVI_FAILURE;
	}

	if ((pstSnsState->bInit == CVI_TRUE) && (u8SensorImageMode == pstSnsState->u8ImgMode))
	{
		/* Don't need to switch SensorImageMode */
		return CVI_FAILURE;
	}

	pstSnsState->u8ImgMode = u8SensorImageMode;

	return CVI_SUCCESS;
}

static CVI_VOID sensor_global_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	TP2863_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER_VOID(pstSnsState);

	pstSnsState->bInit = CVI_FALSE;
	pstSnsState->u8ImgMode = TP2863_MODE_1080P_25P;
	pstSnsState->enWDRMode = WDR_MODE_NONE;
}

static CVI_S32 sensor_rx_attr(VI_PIPE ViPipe, SNS_COMBO_DEV_ATTR_S *pstRxAttr)
{
	ISP_SNS_STATE_S *pstSnsState = CVI_NULL;

	TP2863_SENSOR_GET_CTX(ViPipe, pstSnsState);
	CMOS_CHECK_POINTER(pstSnsState);
	CMOS_CHECK_POINTER(pstRxAttr);

	memcpy(pstRxAttr, &tp2863_rx_attr, sizeof(*pstRxAttr));
	// CVI_TRACE_SNS(CVI_DBG_INFO, "get tp2863_rx0_attr\n");

	pstRxAttr->img_size.width = g_astTP2863_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Width;
	pstRxAttr->img_size.height = g_astTP2863_mode[pstSnsState->u8ImgMode].astImg[0].stSnsSize.u32Height;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_patch_rx_attr(RX_INIT_ATTR_S *pstRxInitAttr)
{
	SNS_COMBO_DEV_ATTR_S *pstRxAttr = &tp2863_rx_attr;
	int i;

	CMOS_CHECK_POINTER(pstRxInitAttr);

	if (pstRxInitAttr->MipiDev >= VI_MAX_DEV_NUM)
		return CVI_SUCCESS;

	pstRxAttr->devno = pstRxInitAttr->MipiDev;

	if (pstRxAttr->input_mode == INPUT_MODE_MIPI)
	{
		struct mipi_dev_attr_s *attr = &pstRxAttr->mipi_attr;

		for (i = 0; i < MIPI_LANE_NUM + 1; i++)
		{
			attr->lane_id[i] = pstRxInitAttr->as16LaneId[i];
			attr->pn_swap[i] = pstRxInitAttr->as8PNSwap[i];
		}
	}
	else
	{
		struct lvds_dev_attr_s *attr = &pstRxAttr->lvds_attr;

		for (i = 0; i < MIPI_LANE_NUM + 1; i++)
		{
			attr->lane_id[i] = pstRxInitAttr->as16LaneId[i];
			attr->pn_swap[i] = pstRxInitAttr->as8PNSwap[i];
		}
	}

	return CVI_SUCCESS;
}

static CVI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
	CMOS_CHECK_POINTER(pstSensorExpFunc);

	memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

	pstSensorExpFunc->pfn_cmos_sensor_init = tp2863_init;
	pstSensorExpFunc->pfn_cmos_sensor_exit = tp2863_exit;
	pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
	pstSensorExpFunc->pfn_cmos_set_image_mode = cmos_set_image_mode;
	pstSensorExpFunc->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

	return CVI_SUCCESS;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

static CVI_S32 tp2863_set_bus_info(VI_PIPE ViPipe, ISP_SNS_COMMBUS_U unSNSBusInfo)
{
	g_aunTP2863_BusInfo[ViPipe].s8I2cDev = unSNSBusInfo.s8I2cDev;

	return CVI_SUCCESS;
}

static CVI_S32 sensor_ctx_init(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	TP2863_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

	if (pastSnsStateCtx == CVI_NULL)
	{
		pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
		if (pastSnsStateCtx == CVI_NULL)
		{
			CVI_TRACE_SNS(CVI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
			return -ENOMEM;
		}
	}

	memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

	TP2863_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);

	return CVI_SUCCESS;
}

static CVI_VOID sensor_ctx_exit(VI_PIPE ViPipe)
{
	ISP_SNS_STATE_S *pastSnsStateCtx = CVI_NULL;

	TP2863_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
	SENSOR_FREE(pastSnsStateCtx);
	TP2863_SENSOR_RESET_CTX(ViPipe);
}

static CVI_S32 sensor_register_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
	(void)pstAeLib;
	(void)pstAwbLib;

	CVI_S32 s32Ret;
	ISP_SENSOR_REGISTER_S stIspRegister;
	ISP_SNS_ATTR_INFO_S stSnsAttrInfo;

	s32Ret = sensor_ctx_init(ViPipe);

	if (s32Ret != CVI_SUCCESS)
		return CVI_FAILURE;

	stSnsAttrInfo.eSensorId = TP2863_ID;

	s32Ret = cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
	s32Ret |= CVI_ISP_SensorRegCallBack(ViPipe, &stSnsAttrInfo, &stIspRegister);

	if (s32Ret != CVI_SUCCESS)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor register callback function failed!\n");
		return s32Ret;
	}

	return CVI_SUCCESS;
}

static CVI_S32 sensor_unregister_callback(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ALG_LIB_S *pstAwbLib)
{
	(void)pstAeLib;
	(void)pstAwbLib;

	CVI_S32 s32Ret;

	s32Ret = CVI_ISP_SensorUnRegCallBack(ViPipe, TP2863_ID);
	if (s32Ret != CVI_SUCCESS)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "sensor unregister callback function failed!\n");
		return s32Ret;
	}

	sensor_ctx_exit(ViPipe);

	return CVI_SUCCESS;
}

ISP_SNS_OBJ_S stSnsTP2863_Obj = {
	.pfnRegisterCallback = sensor_register_callback,
	.pfnUnRegisterCallback = sensor_unregister_callback,
	.pfnMirrorFlip = CVI_NULL,
	.pfnStandby = CVI_NULL,
	.pfnRestart = CVI_NULL,
	.pfnWriteReg = tp2863_write_register,
	.pfnReadReg = tp2863_read_register,
	.pfnSetBusInfo = tp2863_set_bus_info,
	.pfnSetInit = CVI_NULL,
	.pfnPatchRxAttr = sensor_patch_rx_attr,
	.pfnPatchI2cAddr = CVI_NULL,
	.pfnGetRxAttr = sensor_rx_attr,
	.pfnExpSensorCb = cmos_init_sensor_exp_function,
	.pfnExpAeCb = CVI_NULL,
	.pfnSnsProbe = CVI_NULL,
};
