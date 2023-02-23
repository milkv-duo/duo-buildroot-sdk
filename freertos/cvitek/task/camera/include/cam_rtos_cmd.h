/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_rtos_camera.h
 * Description:
 */

#ifndef __CVITEK_RTOS_CAMERA_CMD_H__
#define __CVITEK_RTOS_CAMERA_CMD_H__

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*----------------------------------------------
 * The common data type
 *----------------------------------------------
 */

#ifndef loff_t
#define loff_t unsigned short
#define __user
#endif

struct cvi_cif_ioctl_parm {
	unsigned int devno;
	unsigned int cmd;
	unsigned long arg;
};

struct cvi_cif_dbg_hdler_param {
	const char __user *user_buf;
	size_t count;
	loff_t *ppos;
};

struct cvi_cam_parm {
	unsigned int ViPipe;
	unsigned long arg;
	unsigned long arg1;
	unsigned long arg2;
	unsigned long arg3;
	unsigned long arg4;
};

/*----------------------------------------------
 * definition of cmd
 *----------------------------------------------
 */

enum {
	RTOS_CMD_CAM_EXIT,

	RTOS_CMD_CIF_IOCTL = 10,
	RTOS_CMD_CIF_DBG_HDLER,
	RTOS_CMD_CIF_END,

	RTOS_CMD_CAM_REG_CB = 20,
	RTOS_CMD_CAM_UN_REG_CB,
	RTOS_CMD_CAM_SET_BUS_INFO,
	RTOS_CMD_CAM_STANDBY,
	RTOS_CMD_CAM_RESTART,
	RTOS_CMD_CAM_SET_ORIEN,
	RTOS_CMD_CAM_W_REG,
	RTOS_CMD_CAM_R_REG,
	RTOS_CMD_CAM_SET_INIT,
	RTOS_CMD_CAM_PATCH_RX_ATTR,
	RTOS_CMD_CAM_PATCH_I2C_ADDR,
	RTOS_CMD_CAM_GET_RX_ATTR,
	RTOS_CMD_CAM_PROBE,

	RTOS_CMD_CAM_EXP_SENSOR_INIT = 40,
	RTOS_CMD_CAM_EXP_SENSOR_EXIT,
	RTOS_CMD_CAM_EXP_GLOBAL_INIT,
	RTOS_CMD_CAM_SET_IMAGE_MODE,
	RTOS_CMD_CAM_SET_WDR_MODE,
	RTOS_CMD_CAM_GET_ISP_DEFAULT,
	RTOS_CMD_CAM_GET_BLC,
	RTOS_CMD_CAM_GET_REG_INFO,

	RTOS_CMD_CAM_GET_AE_DEFAULT = 60,
	RTOS_CMD_CAM_SET_FPS,
	RTOS_CMD_CAM_SET_SLOW_FPS,
	RTOS_CMD_CAM_UPDATE_INTTIME,
	RTOS_CMD_CAM_UPDATE_GAIN,
	RTOS_CMD_CAM_CALC_AGAIN,
	RTOS_CMD_CAM_CALC_DGAIN,
	RTOS_CMD_CAM_GET_MAX_INTTIME,
	RTOS_CMD_CAM_SET_FSWDR_ATTR,
} E_CVITEK_RTOS_CAMERA;




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVITEK_RTOS_CAMERA_CMD_H__ */
