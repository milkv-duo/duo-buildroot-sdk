/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "delay.h"
#include "mmio.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "comm.h"
#include "cvi_type.h"
#include "cvi_math.h"

// #include "cam_task.h"
#include "cam_rtos_cmd.h"

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_sns_ctrl.h"
#include "cvi_comm_isp.h"
#include "cvi_comm_vi.h"
#include "sample_comm.h"
#include "cvi_vi.h"
#include "cif_uapi.h"

#include "sensor.h"
#include "cvi_i2c.h"

static QueueHandle_t gCameraQueHandle;
static QueueHandle_t gQueueCameraCmdqu;

void prvCameraRunTask(void *pvParameters)
{
	ISP_SNS_OBJ_S *pstSnsObj = NULL;
	struct cvi_cam_parm *cam_parm = NULL;
	uint32_t ViPipe = 0;
	ISP_SENSOR_EXP_FUNC_S stSnsrSensorFunc;
	AE_SENSOR_EXP_FUNC_S stSnsrAeFunc;

	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;
	cmdqu_t rtos_cmdq;
	printf("%s:%d run +\n", __FUNCTION__, __LINE__);

	gCameraQueHandle = main_GetMODHandle(E_QUEUE_CAMERA);
	gQueueCameraCmdqu = main_GetMODHandle(E_QUEUE_CMDQU);

	for (;;) {
		xQueueReceive(gCameraQueHandle, &rtos_cmdq, portMAX_DELAY);
		printf("prvCameraRunTask ip=%d, cmd=%d, para=%x\n",
			rtos_cmdq.ip_id, rtos_cmdq.cmd_id, rtos_cmdq.param_ptr);

		if (rtos_cmdq.ip_id != IP_CAMERA) {
			printf("Get CMD != IP_CAMERA\n");
			rtos_cmdq.ip_id = -1;
			continue;
		}

		cam_parm = (struct cvi_cam_parm *) rtos_cmdq.param_ptr;
		ViPipe = cam_parm->ViPipe;
		pstSnsObj = (ISP_SNS_OBJ_S *) SAMPLE_COMM_ISP_GetSnsObj(ViPipe);
		if (!pstSnsObj) {
			printf("[RTOS] SAMPLE_COMM_ISP_GetSnsObj = NULL\n");
			continue;
		}

		if (rtos_cmdq.cmd_id > RTOS_CMD_CIF_END) {
			pstSnsObj->pfnExpSensorCb(&stSnsrSensorFunc);
			pstSnsObj->pfnExpAeCb(&stSnsrAeFunc);
		}

		switch (rtos_cmdq.cmd_id) {
		case RTOS_CMD_CIF_IOCTL:
		{
			struct cvi_cif_ioctl_parm *cif_parm = (struct cvi_cif_ioctl_parm *) rtos_cmdq.param_ptr;
			cif_ioctl(cif_parm->devno, cif_parm->cmd, cif_parm->arg);
			break;
		}
		case RTOS_CMD_CAM_REG_CB:
			pstSnsObj->pfnRegisterCallback(ViPipe,
				(ALG_LIB_S *) cam_parm->arg, (ALG_LIB_S *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_UN_REG_CB:
			pstSnsObj->pfnUnRegisterCallback(ViPipe,
				(ALG_LIB_S *) cam_parm->arg, (ALG_LIB_S *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_SET_BUS_INFO:
		{
			ISP_SNS_COMMBUS_U *pbi = (ISP_SNS_COMMBUS_U *) cam_parm->arg;
			pstSnsObj->pfnSetBusInfo(ViPipe, *pbi);
			break;
		}
		case RTOS_CMD_CAM_STANDBY:
			pstSnsObj->pfnStandby(ViPipe);
			break;
		case RTOS_CMD_CAM_RESTART:
			pstSnsObj->pfnRestart(ViPipe);
			break;
		case RTOS_CMD_CAM_SET_ORIEN:
			pstSnsObj->pfnMirrorFlip(ViPipe, (ISP_SNS_MIRRORFLIP_TYPE_E) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_W_REG:
			pstSnsObj->pfnWriteReg(ViPipe, (CVI_S32) cam_parm->arg, (CVI_S32) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_R_REG:
			pstSnsObj->pfnReadReg(ViPipe, (CVI_S32) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_SET_INIT:
			pstSnsObj->pfnSetInit(ViPipe, (ISP_INIT_ATTR_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_PATCH_RX_ATTR:
			pstSnsObj->pfnPatchRxAttr((RX_INIT_ATTR_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_PATCH_I2C_ADDR:
			pstSnsObj->pfnPatchI2cAddr((CVI_S32) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_GET_RX_ATTR:
			pstSnsObj->pfnGetRxAttr(ViPipe, (SNS_COMBO_DEV_ATTR_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_PROBE:
			pstSnsObj->pfnSnsProbe(ViPipe);
			break;

		/* ISP_SENSOR_EXP_FUNC_S */
		case RTOS_CMD_CAM_EXP_SENSOR_INIT:
			stSnsrSensorFunc.pfn_cmos_sensor_init(ViPipe);
			break;
		case RTOS_CMD_CAM_EXP_SENSOR_EXIT:
			stSnsrSensorFunc.pfn_cmos_sensor_exit(ViPipe);
			break;
		case RTOS_CMD_CAM_EXP_GLOBAL_INIT:
			stSnsrSensorFunc.pfn_cmos_sensor_global_init(ViPipe);
			break;
		case RTOS_CMD_CAM_SET_IMAGE_MODE:
			stSnsrSensorFunc.pfn_cmos_set_image_mode(ViPipe,
				(ISP_CMOS_SENSOR_IMAGE_MODE_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_SET_WDR_MODE:
			stSnsrSensorFunc.pfn_cmos_set_wdr_mode(ViPipe, (CVI_U8) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_GET_ISP_DEFAULT:
			stSnsrSensorFunc.pfn_cmos_get_isp_default(ViPipe, (ISP_CMOS_DEFAULT_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_GET_BLC:
			stSnsrSensorFunc.pfn_cmos_get_isp_black_level(ViPipe,
				(ISP_CMOS_BLACK_LEVEL_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_GET_REG_INFO:
			stSnsrSensorFunc.pfn_cmos_get_sns_reg_info(ViPipe,
				(ISP_SNS_SYNC_INFO_S *) cam_parm->arg);
			break;

		/* AE_SENSOR_EXP_FUNC_S */
		case RTOS_CMD_CAM_GET_AE_DEFAULT:
			stSnsrAeFunc.pfn_cmos_get_ae_default(ViPipe, (AE_SENSOR_DEFAULT_S *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_SET_FPS:
			stSnsrAeFunc.pfn_cmos_fps_set(ViPipe, (CVI_FLOAT) cam_parm->arg,
				(AE_SENSOR_DEFAULT_S *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_SET_SLOW_FPS:
			stSnsrAeFunc.pfn_cmos_slow_framerate_set(ViPipe, (CVI_U32) cam_parm->arg,
				(AE_SENSOR_DEFAULT_S *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_UPDATE_INTTIME:
			stSnsrAeFunc.pfn_cmos_inttime_update(ViPipe, (CVI_U32 *) cam_parm->arg);
			break;
		case RTOS_CMD_CAM_UPDATE_GAIN:
			stSnsrAeFunc.pfn_cmos_gains_update(ViPipe,
				(CVI_U32 *) cam_parm->arg, (CVI_U32 *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_CALC_AGAIN:
			stSnsrAeFunc.pfn_cmos_again_calc_table(ViPipe,
				(CVI_U32 *) cam_parm->arg, (CVI_U32 *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_CALC_DGAIN:
			stSnsrAeFunc.pfn_cmos_dgain_calc_table(ViPipe,
				(CVI_U32 *) cam_parm->arg, (CVI_U32 *) cam_parm->arg1);
			break;
		case RTOS_CMD_CAM_GET_MAX_INTTIME:
			stSnsrAeFunc.pfn_cmos_get_inttime_max(ViPipe, (CVI_U16) cam_parm->arg,
				(CVI_U32 *) cam_parm->arg1, (CVI_U32 *) cam_parm->arg2,
				(CVI_U32 *) cam_parm->arg3, (CVI_U32 *) cam_parm->arg4);
			break;
		case RTOS_CMD_CAM_SET_FSWDR_ATTR:
			stSnsrAeFunc.pfn_cmos_ae_fswdr_attr_set(ViPipe, (AE_FSWDR_ATTR_S *) cam_parm->arg);
			break;
		}
		rtos_cmdq.ip_id = -1;
	}
}
