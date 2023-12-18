/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample/common/sample_common_vo.c
 * Description:
 *   Common sample code for video output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "sample_comm.h"

#include "mipi_tx.h"

#include "dsi_hx8394_evb.h"
#include "i80_st7789v.h"
#include "bt656_tp2803.h"
#include "bt1120_nvp6021.h"
#include "hw_mcu_st7789v3.h"
#include "lvds_lcm185x56.h"

static CVI_S32 sample_vo_i2c_file = -1;
static CVI_S32 sample_vo_i2c_slave_addr;

CVI_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, CVI_U32 *pu32W, CVI_U32 *pu32H, CVI_U32 *pu32Frm)
{
	switch (enIntfSync) {
	case VO_OUTPUT_PAL:
		*pu32W = 720;
		*pu32H = 576;
		*pu32Frm = 25;
		break;
	case VO_OUTPUT_NTSC:
		*pu32W = 720;
		*pu32H = 480;
		*pu32Frm = 30;
		break;
	case VO_OUTPUT_1080P24:
		*pu32W = 1920;
		*pu32H = 1080;
		*pu32Frm = 24;
		break;
	case VO_OUTPUT_1080P25:
		*pu32W = 1920;
		*pu32H = 1080;
		*pu32Frm = 25;
		break;
	case VO_OUTPUT_1080P30:
		*pu32W = 1920;
		*pu32H = 1080;
		*pu32Frm = 30;
		break;
	case VO_OUTPUT_720P50:
		*pu32W = 1280;
		*pu32H = 720;
		*pu32Frm = 50;
		break;
	case VO_OUTPUT_720P60:
		*pu32W = 1280;
		*pu32H = 720;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1080P50:
		*pu32W = 1920;
		*pu32H = 1080;
		*pu32Frm = 50;
		break;
	case VO_OUTPUT_1080P60:
		*pu32W = 1920;
		*pu32H = 1080;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_576P50:
		*pu32W = 720;
		*pu32H = 576;
		*pu32Frm = 50;
		break;
	case VO_OUTPUT_480P60:
		*pu32W = 720;
		*pu32H = 480;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_800x600_60:
		*pu32W = 800;
		*pu32H = 600;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1024x768_60:
		*pu32W = 1024;
		*pu32H = 768;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1280x1024_60:
		*pu32W = 1280;
		*pu32H = 1024;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1366x768_60:
		*pu32W = 1366;
		*pu32H = 768;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1440x900_60:
		*pu32W = 1440;
		*pu32H = 900;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1280x800_60:
		*pu32W = 1280;
		*pu32H = 800;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1600x1200_60:
		*pu32W = 1600;
		*pu32H = 1200;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1680x1050_60:
		*pu32W = 1680;
		*pu32H = 1050;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1920x1200_60:
		*pu32W = 1920;
		*pu32H = 1200;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_640x480_60:
		*pu32W = 640;
		*pu32H = 480;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_720x1280_60:
		*pu32W = 720;
		*pu32H = 1280;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_1080x1920_60:
		*pu32W = 1080;
		*pu32H = 1920;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_480x800_60:
		*pu32W = 480;
		*pu32H = 800;
		*pu32Frm = 60;
		break;
	case VO_OUTPUT_USER:
		*pu32W = 720;
		*pu32H = 576;
		*pu32Frm = 25;
		break;
	default:
		SAMPLE_PRT("vo enIntfSync %d not support!\n", enIntfSync);
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VO_FillIntfAttr(VO_PUB_ATTR_S *pstPubAttr)
{
	if (pstPubAttr == NULL) {
		SAMPLE_PRT("Error:argument can not be NULL\n");
		return CVI_FAILURE;
	}

	switch (pstPubAttr->enIntfType) {
	case VO_INTF_I80:
		pstPubAttr->sti80Cfg = stI80Cfg;
		break;
	case VO_INTF_HW_MCU:
		pstPubAttr->stMcuCfg = st7789v3Cfg;
		break;
	case VO_INTF_CVBS:
	case VO_INTF_YPBPR:
	case VO_INTF_VGA:
	case VO_INTF_BT656:
		pstPubAttr->stBtAttr = stTP2803Cfg;
		break;
	case VO_INTF_BT1120:
		pstPubAttr->stBtAttr = stNVP6021Cfg;
		break;
	case VO_INTF_LCD:
	case VO_INTF_LCD_18BIT:
	case VO_INTF_LCD_24BIT:
		pstPubAttr->stLvdsAttr = lvds_lcm185x56_cfg;
	case VO_INTF_LCD_30BIT:
	case VO_INTF_HDMI:
		break;
	case VO_INTF_MIPI:
	case VO_INTF_MIPI_SLAVE:
		//no need, MIPI-DSI is setup by mipi-tx
		break;
	default:
		break;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = SAMPLE_COMM_VO_FillIntfAttr(pstPubAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VO_SetPubAttr(VoDev, pstPubAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VO_Enable(VoDev);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VO_Disable(VoDev);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VO_SetVideoLayerAttr(VoLayer, pstLayerAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VO_EnableVideoLayer(VoLayer);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer)
{
	CVI_S32 s32Ret = CVI_SUCCESS;

	s32Ret = CVI_VO_DisableVideoLayer(VoLayer);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	return s32Ret;
}

CVI_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
	CVI_U32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32WndNum = 0;
	CVI_U32 u32Square = 0;
	CVI_U32 u32Row = 0;
	CVI_U32 u32Col = 0;
	CVI_U32 u32Width = 0;
	CVI_U32 u32Height = 0;
	VO_CHN_ATTR_S stChnAttr;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr;

	switch (enMode) {
	case VO_MODE_1MUX:
		u32WndNum = 1;
		u32Square = 1;
		break;
	case VO_MODE_2MUX:
		u32WndNum = 2;
		u32Square = 2;
		break;
	case VO_MODE_4MUX:
		u32WndNum = 4;
		u32Square = 2;
		break;
	case VO_MODE_8MUX:
		u32WndNum = 8;
		u32Square = 3;
		break;
	case VO_MODE_9MUX:
		u32WndNum = 9;
		u32Square = 3;
		break;
	case VO_MODE_16MUX:
		u32WndNum = 16;
		u32Square = 4;
		break;
	case VO_MODE_25MUX:
		u32WndNum = 25;
		u32Square = 5;
		break;
	case VO_MODE_36MUX:
		u32WndNum = 36;
		u32Square = 6;
		break;
	case VO_MODE_49MUX:
		u32WndNum = 49;
		u32Square = 7;
		break;
	case VO_MODE_64MUX:
		u32WndNum = 64;
		u32Square = 8;
		break;
	case VO_MODE_2X4:
		u32WndNum = 8;
		u32Square = 3;
		u32Row = 4;
		u32Col = 2;
		break;
	default:
		SAMPLE_PRT("Undefined VO_MODE(%d)!\n", enMode);
		return CVI_FAILURE;
	}

	s32Ret = CVI_VO_GetVideoLayerAttr(VoLayer, &stLayerAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}
	u32Width = stLayerAttr.stImageSize.u32Width;
	u32Height = stLayerAttr.stImageSize.u32Height;
	SAMPLE_PRT("u32Width:%d, u32Height:%d, u32Square:%d\n", u32Width, u32Height, u32Square);
	for (i = 0; i < u32WndNum; i++) {
		if (enMode == VO_MODE_1MUX || enMode == VO_MODE_2MUX || enMode == VO_MODE_4MUX ||
		    enMode == VO_MODE_8MUX || enMode == VO_MODE_9MUX || enMode == VO_MODE_16MUX ||
		    enMode == VO_MODE_25MUX || enMode == VO_MODE_36MUX || enMode == VO_MODE_49MUX ||
		    enMode == VO_MODE_64MUX) {
			stChnAttr.stRect.s32X = ALIGN_DOWN((u32Width / u32Square) * (i % u32Square), 2);
			stChnAttr.stRect.s32Y = ALIGN_DOWN((u32Height / u32Square) * (i / u32Square), 2);
			stChnAttr.stRect.u32Width = ALIGN_DOWN(u32Width / u32Square, 2);
			stChnAttr.stRect.u32Height = ALIGN_DOWN(u32Height / u32Square, 2);
			stChnAttr.u32Priority = 0;
		} else if (enMode == VO_MODE_2X4) {
			stChnAttr.stRect.s32X = ALIGN_DOWN((u32Width / u32Col) * (i % u32Col), 2);
			stChnAttr.stRect.s32Y = ALIGN_DOWN((u32Height / u32Row) * (i / u32Col), 2);
			stChnAttr.stRect.u32Width = ALIGN_DOWN(u32Width / u32Col, 2);
			stChnAttr.stRect.u32Height = ALIGN_DOWN(u32Height / u32Row, 2);
			stChnAttr.u32Priority = 0;
		}

		s32Ret = CVI_VO_SetChnAttr(VoLayer, i, &stChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}

		s32Ret = CVI_VO_EnableChn(VoLayer, i);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode)
{
	CVI_U32 i;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_U32 u32WndNum = 0;

	switch (enMode) {
	case VO_MODE_1MUX: {
		u32WndNum = 1;
		break;
	}
	case VO_MODE_2MUX: {
		u32WndNum = 2;
		break;
	}
	case VO_MODE_4MUX: {
		u32WndNum = 4;
		break;
	}
	case VO_MODE_8MUX: {
		u32WndNum = 8;
		break;
	}
	default:
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	for (i = 0; i < u32WndNum; i++) {
		s32Ret = CVI_VO_DisableChn(VoLayer, i);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
	}

	return s32Ret;
}

/*
 * Name : SAMPLE_COMM_VO_GetDefConfig
 * Desc : An instance of SAMPLE_VO_CONFIG_S, which allows you to use vo immediately.
 */
CVI_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
	if (pstVoConfig == NULL) {
		SAMPLE_PRT("Error:argument can not be NULL\n");
		return CVI_FAILURE;
	}

	pstVoConfig->VoDev             = SAMPLE_VO_DEV_UHD;

	pstVoConfig->stVoPubAttr.enIntfType = VO_INTF_MIPI;

	RECT_S stDefDispRect  = {0, 0, 1920, 1080};
	SIZE_S stDefImageSize = {1920, 1080};

	pstVoConfig->stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
	pstVoConfig->stDispRect    = stDefDispRect;
	pstVoConfig->stImageSize   = stDefImageSize;
	pstVoConfig->enPixFormat   = PIXEL_FORMAT_RGB_888_PLANAR;
	pstVoConfig->stVoPubAttr.u32BgColor = COLOR_10_RGB_BLUE;
	pstVoConfig->u32DisBufLen  = 3;
	pstVoConfig->enVoMode      = VO_MODE_1MUX;

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
	/*******************************************
	 * VO device VoDev# information declaration.
	 *******************************************/
	VO_DEV VoDev = 0;
	VO_LAYER VoLayer = 0;
	SAMPLE_VO_MODE_E enVoMode = 0;
	VO_VIDEO_LAYER_ATTR_S stLayerAttr = { 0 };
	CVI_S32 s32Ret = CVI_SUCCESS;

	if (pstVoConfig == NULL) {
		SAMPLE_PRT("Error:argument can not be NULL\n");
		return CVI_FAILURE;
	}
	VoDev = pstVoConfig->VoDev;
	VoLayer = pstVoConfig->VoDev;
	enVoMode = pstVoConfig->enVoMode;

	/********************************
	 * Set and start VO device VoDev#.
	 ********************************/
	s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &pstVoConfig->stVoPubAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartDev failed!\n");
		return s32Ret;
	}

	/******************************
	 * Set and start layer VoDev#.
	 ********************************/

	s32Ret = SAMPLE_COMM_VO_GetWH(pstVoConfig->stVoPubAttr.enIntfSync, &stLayerAttr.stDispRect.u32Width,
				      &stLayerAttr.stDispRect.u32Height, &stLayerAttr.u32DispFrmRt);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_GetWH failed!\n");
		SAMPLE_COMM_VO_StopDev(VoDev);
		return s32Ret;
	}
	stLayerAttr.enPixFormat = pstVoConfig->enPixFormat;

	stLayerAttr.stDispRect.s32X = 0;
	stLayerAttr.stDispRect.s32Y = 0;

	/******************************
	 * Set display rectangle if changed.
	 ********************************/
	if (memcmp(&pstVoConfig->stDispRect, &stLayerAttr.stDispRect, sizeof(RECT_S)) != 0)
		stLayerAttr.stDispRect = pstVoConfig->stDispRect;
	stLayerAttr.stImageSize.u32Width = stLayerAttr.stDispRect.u32Width;
	stLayerAttr.stImageSize.u32Height = stLayerAttr.stDispRect.u32Height;

	/******************************
	 * Set image size if changed.
	 ********************************/
	if (memcmp(&pstVoConfig->stImageSize, &stLayerAttr.stImageSize, sizeof(SIZE_S)) != 0)
		stLayerAttr.stImageSize = pstVoConfig->stImageSize;

	if (pstVoConfig->u32DisBufLen) {
		s32Ret = CVI_VO_SetDisplayBufLen(VoLayer, pstVoConfig->u32DisBufLen);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_VO_SetDisplayBufLen failed with %#x!\n", s32Ret);
			SAMPLE_COMM_VO_StopDev(VoDev);
			return s32Ret;
		}
	}

	s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_Start video layer failed!\n");
		SAMPLE_COMM_VO_StopDev(VoDev);
		return s32Ret;
	}

	/******************************
	 * start vo channels.
	 ********************************/
	s32Ret = SAMPLE_COMM_VO_StartChn(VoLayer, enVoMode);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_VO_StartChn failed!\n");
		SAMPLE_COMM_VO_StopLayer(VoLayer);
		SAMPLE_COMM_VO_StopDev(VoDev);
		return s32Ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig)
{
	VO_DEV VoDev = 0;
	VO_LAYER VoLayer = 0;
	SAMPLE_VO_MODE_E enVoMode = VO_MODE_BUTT;

	if (pstVoConfig == NULL) {
		SAMPLE_PRT("Error:argument can not be NULL\n");
		return CVI_FAILURE;
	}

	VoDev = pstVoConfig->VoDev;
	VoLayer = pstVoConfig->VoDev;
	enVoMode = pstVoConfig->enVoMode;

	SAMPLE_COMM_VO_StopChn(VoLayer, enVoMode);
	SAMPLE_COMM_VO_StopLayer(VoLayer);
	SAMPLE_COMM_VO_StopDev(VoDev);

	return CVI_SUCCESS;
}

CVI_VOID SAMPLE_COMM_VO_Exit(void)
{
	CVI_S32 i = 0, j = 0;

	for (i = 0; i < VO_MAX_LAYER_NUM; i++)
		for (j = 0; j < VO_MAX_CHN_NUM; j++)
			CVI_VO_DisableChn(i, j);

	for (i = 0; i < VO_MAX_LAYER_NUM; i++)
		CVI_VO_DisableVideoLayer(i);

	for (i = 0; i < VO_MAX_DEV_NUM; i++)
		CVI_VO_Disable(i);
}


#define SAMPLE_VO_I2C_WRITE(reg, val) \
	SAMPLE_COMM_I2C_Write(sample_vo_i2c_file, sample_vo_i2c_slave_addr, reg, val, 1, 1)

#define delay_ms(ms) usleep(ms * 1000)
#define MS7024_INTERNAL_PATTERN 1
#define MS7024_DVIN_576P50 2
#define MS7024_DVIN_480P60 3
CVI_S32 SAMPLE_COMM_VO_Init_BT656_MS7024(char *i2c_bus_str, uint8_t slave_addr, uint8_t selection)
{
	//init i2c
	sample_vo_i2c_file = SAMPLE_COMM_I2C_Open(i2c_bus_str);
	sample_vo_i2c_slave_addr = slave_addr;

	if (selection == MS7024_INTERNAL_PATTERN) {
		SAMPLE_VO_I2C_WRITE(0x04, 0x1c);
		SAMPLE_VO_I2C_WRITE(0x0e, 0x57);
		SAMPLE_VO_I2C_WRITE(0x0f, 0x01);
		SAMPLE_VO_I2C_WRITE(0x30, 0x02);
		SAMPLE_VO_I2C_WRITE(0x31, 0x4a);
		SAMPLE_VO_I2C_WRITE(0x32, 0x03);
		SAMPLE_VO_I2C_WRITE(0x33, 0x7a);
		SAMPLE_VO_I2C_WRITE(0x34, 0x00);
		SAMPLE_VO_I2C_WRITE(0x35, 0x07);
		SAMPLE_VO_I2C_WRITE(0x36, 0x02);
		SAMPLE_VO_I2C_WRITE(0x37, 0x27);
		SAMPLE_VO_I2C_WRITE(0x38, 0x00);
		SAMPLE_VO_I2C_WRITE(0x39, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x90, 0x02);
		SAMPLE_VO_I2C_WRITE(0x91, 0x00);
		SAMPLE_VO_I2C_WRITE(0x92, 0x00);
		SAMPLE_VO_I2C_WRITE(0x93, 0x00);
		SAMPLE_VO_I2C_WRITE(0x94, 0x00);
		SAMPLE_VO_I2C_WRITE(0x95, 0x00);
		SAMPLE_VO_I2C_WRITE(0x96, 0x00);
		SAMPLE_VO_I2C_WRITE(0x97, 0x00);
		SAMPLE_VO_I2C_WRITE(0x98, 0x00);
		SAMPLE_VO_I2C_WRITE(0x99, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9e, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9e, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9f, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa0, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa1, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa2, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa4, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa5, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa6, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa7, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa8, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa9, 0x47);
		SAMPLE_VO_I2C_WRITE(0xaa, 0x02);
		SAMPLE_VO_I2C_WRITE(0xab, 0x1d);
		SAMPLE_VO_I2C_WRITE(0xac, 0x15);
		SAMPLE_VO_I2C_WRITE(0xad, 0x89);
		SAMPLE_VO_I2C_WRITE(0x50, 0x00);
		SAMPLE_VO_I2C_WRITE(0x51, 0x16);
		SAMPLE_VO_I2C_WRITE(0x52, 0x0b);
		SAMPLE_VO_I2C_WRITE(0x53, 0x00);
		SAMPLE_VO_I2C_WRITE(0x54, 0x00);
		SAMPLE_VO_I2C_WRITE(0x55, 0x00);
		SAMPLE_VO_I2C_WRITE(0x56, 0x00);
		SAMPLE_VO_I2C_WRITE(0x57, 0x00);
		SAMPLE_VO_I2C_WRITE(0x58, 0x00);
		SAMPLE_VO_I2C_WRITE(0x59, 0x0d);
		SAMPLE_VO_I2C_WRITE(0x5a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);
		SAMPLE_VO_I2C_WRITE(0x21, 0x11);
		SAMPLE_VO_I2C_WRITE(0x22, 0x63);
		SAMPLE_VO_I2C_WRITE(0x23, 0x01);
		SAMPLE_VO_I2C_WRITE(0x24, 0x00);
		SAMPLE_VO_I2C_WRITE(0x25, 0x00);
		SAMPLE_VO_I2C_WRITE(0x26, 0x00);
		SAMPLE_VO_I2C_WRITE(0x27, 0xc1);
		SAMPLE_VO_I2C_WRITE(0x28, 0xc1);
		SAMPLE_VO_I2C_WRITE(0x29, 0x80);
		SAMPLE_VO_I2C_WRITE(0x2a, 0x84);
		SAMPLE_VO_I2C_WRITE(0x2b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x2c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x2d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x60, 0x03);
		SAMPLE_VO_I2C_WRITE(0x61, 0x00);
		SAMPLE_VO_I2C_WRITE(0x62, 0x01);
		SAMPLE_VO_I2C_WRITE(0x63, 0x00);
		SAMPLE_VO_I2C_WRITE(0x64, 0x20);
		SAMPLE_VO_I2C_WRITE(0x66, 0x00);
		SAMPLE_VO_I2C_WRITE(0x67, 0x40);
		SAMPLE_VO_I2C_WRITE(0x68, 0x00);
		SAMPLE_VO_I2C_WRITE(0x69, 0x20);
		SAMPLE_VO_I2C_WRITE(0x6a, 0x40);
		SAMPLE_VO_I2C_WRITE(0x6b, 0x60);
		SAMPLE_VO_I2C_WRITE(0x6c, 0x80);
		SAMPLE_VO_I2C_WRITE(0x6d, 0xa0);
		SAMPLE_VO_I2C_WRITE(0x6e, 0xc0);
		SAMPLE_VO_I2C_WRITE(0x6f, 0xe0);
		SAMPLE_VO_I2C_WRITE(0x70, 0xff);
		SAMPLE_VO_I2C_WRITE(0x71, 0x03);
		SAMPLE_VO_I2C_WRITE(0x72, 0x4b);
		SAMPLE_VO_I2C_WRITE(0x73, 0x40);
		SAMPLE_VO_I2C_WRITE(0x74, 0x40);
		SAMPLE_VO_I2C_WRITE(0x75, 0x40);
		SAMPLE_VO_I2C_WRITE(0x76, 0x40);
		SAMPLE_VO_I2C_WRITE(0x77, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x78, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x79, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x7a, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x7b, 0x02);
		SAMPLE_VO_I2C_WRITE(0x7c, 0x8c);
		SAMPLE_VO_I2C_WRITE(0x7d, 0xd4);
		SAMPLE_VO_I2C_WRITE(0x7e, 0x72);
		SAMPLE_VO_I2C_WRITE(0x7f, 0x00);
		SAMPLE_VO_I2C_WRITE(0x80, 0x00);
		SAMPLE_VO_I2C_WRITE(0x81, 0x00);
		SAMPLE_VO_I2C_WRITE(0x82, 0x04);
		SAMPLE_VO_I2C_WRITE(0x83, 0x00);
		SAMPLE_VO_I2C_WRITE(0x84, 0xff);
		SAMPLE_VO_I2C_WRITE(0x85, 0xce);
		SAMPLE_VO_I2C_WRITE(0x86, 0xb2);
		SAMPLE_VO_I2C_WRITE(0x87, 0x00);
		SAMPLE_VO_I2C_WRITE(0x88, 0x00);
		SAMPLE_VO_I2C_WRITE(0x89, 0x93);
		SAMPLE_VO_I2C_WRITE(0x8a, 0x06);
		SAMPLE_VO_I2C_WRITE(0x5f, 0x01);
		SAMPLE_VO_I2C_WRITE(0x2e, 0x10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x56);
		SAMPLE_VO_I2C_WRITE(0x2e, 0x11);
		SAMPLE_VO_I2C_WRITE(0x20, 0x50);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x56);
		SAMPLE_VO_I2C_WRITE(0x20, 0x50);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);

		// delay 100ms
		delay_ms(100);
		SAMPLE_VO_I2C_WRITE(0x05, 0x07);
		SAMPLE_VO_I2C_WRITE(0x06, 0x0f);
	} else if (selection == MS7024_DVIN_576P50) {
		SAMPLE_VO_I2C_WRITE(0x04, 0x1c);
		SAMPLE_VO_I2C_WRITE(0x0e, 0x57);
		SAMPLE_VO_I2C_WRITE(0x0f, 0x01);
		SAMPLE_VO_I2C_WRITE(0x30, 0x02);
		SAMPLE_VO_I2C_WRITE(0x31, 0x53);
		SAMPLE_VO_I2C_WRITE(0x32, 0x03);
		SAMPLE_VO_I2C_WRITE(0x33, 0x79);
		SAMPLE_VO_I2C_WRITE(0x34, 0x00);
		SAMPLE_VO_I2C_WRITE(0x35, 0x6d);
		SAMPLE_VO_I2C_WRITE(0x36, 0x02);
		SAMPLE_VO_I2C_WRITE(0x37, 0x2d);
		SAMPLE_VO_I2C_WRITE(0x38, 0x00);
		SAMPLE_VO_I2C_WRITE(0x39, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x90, 0x02);
		SAMPLE_VO_I2C_WRITE(0x91, 0x00);
		SAMPLE_VO_I2C_WRITE(0x92, 0x00);
		SAMPLE_VO_I2C_WRITE(0x93, 0x00);
		SAMPLE_VO_I2C_WRITE(0x94, 0x00);
		SAMPLE_VO_I2C_WRITE(0x95, 0x00);
		SAMPLE_VO_I2C_WRITE(0x96, 0x00);
		SAMPLE_VO_I2C_WRITE(0x97, 0x00);
		SAMPLE_VO_I2C_WRITE(0x98, 0x00);
		SAMPLE_VO_I2C_WRITE(0x99, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9e, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9e, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9f, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa0, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa1, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa2, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa4, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa5, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa6, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa7, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa8, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa9, 0x07);
		SAMPLE_VO_I2C_WRITE(0xaa, 0x02);
		SAMPLE_VO_I2C_WRITE(0xab, 0x05);
		SAMPLE_VO_I2C_WRITE(0xac, 0x15);
		SAMPLE_VO_I2C_WRITE(0xad, 0x89);
		SAMPLE_VO_I2C_WRITE(0x50, 0x20);
		SAMPLE_VO_I2C_WRITE(0x51, 0x16);
		SAMPLE_VO_I2C_WRITE(0x52, 0x0b);
		SAMPLE_VO_I2C_WRITE(0x53, 0x00);
		SAMPLE_VO_I2C_WRITE(0x54, 0x00);
		SAMPLE_VO_I2C_WRITE(0x55, 0x00);
		SAMPLE_VO_I2C_WRITE(0x56, 0x00);
		SAMPLE_VO_I2C_WRITE(0x57, 0x00);
		SAMPLE_VO_I2C_WRITE(0x58, 0x00);
		SAMPLE_VO_I2C_WRITE(0x59, 0x00);
		SAMPLE_VO_I2C_WRITE(0x5a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);
		SAMPLE_VO_I2C_WRITE(0x21, 0x11);
		SAMPLE_VO_I2C_WRITE(0x22, 0x63);
		SAMPLE_VO_I2C_WRITE(0x23, 0x01);
		SAMPLE_VO_I2C_WRITE(0x24, 0x00);
		SAMPLE_VO_I2C_WRITE(0x25, 0x00);
		SAMPLE_VO_I2C_WRITE(0x26, 0x00);
		SAMPLE_VO_I2C_WRITE(0x27, 0xc1);
		SAMPLE_VO_I2C_WRITE(0x28, 0xc1);
		SAMPLE_VO_I2C_WRITE(0x29, 0x80);
		SAMPLE_VO_I2C_WRITE(0x2a, 0x84);
		SAMPLE_VO_I2C_WRITE(0x2b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x2c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x2d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x60, 0xc3);
		SAMPLE_VO_I2C_WRITE(0x61, 0x00);
		SAMPLE_VO_I2C_WRITE(0x62, 0x01);
		SAMPLE_VO_I2C_WRITE(0x63, 0x00);
		SAMPLE_VO_I2C_WRITE(0x64, 0x20);
		SAMPLE_VO_I2C_WRITE(0x66, 0x00);
		SAMPLE_VO_I2C_WRITE(0x67, 0x40);
		SAMPLE_VO_I2C_WRITE(0x68, 0x00);
		SAMPLE_VO_I2C_WRITE(0x69, 0x20);
		SAMPLE_VO_I2C_WRITE(0x6a, 0x40);
		SAMPLE_VO_I2C_WRITE(0x6b, 0x60);
		SAMPLE_VO_I2C_WRITE(0x6c, 0x80);
		SAMPLE_VO_I2C_WRITE(0x6d, 0xa0);
		SAMPLE_VO_I2C_WRITE(0x6e, 0xc0);
		SAMPLE_VO_I2C_WRITE(0x6f, 0xe0);
		SAMPLE_VO_I2C_WRITE(0x70, 0xff);
		SAMPLE_VO_I2C_WRITE(0x71, 0x03);
		SAMPLE_VO_I2C_WRITE(0x72, 0x4b);
		SAMPLE_VO_I2C_WRITE(0x73, 0x40);
		SAMPLE_VO_I2C_WRITE(0x74, 0x40);
		SAMPLE_VO_I2C_WRITE(0x75, 0x40);
		SAMPLE_VO_I2C_WRITE(0x76, 0x40);
		SAMPLE_VO_I2C_WRITE(0x77, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x78, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x79, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x7a, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x7b, 0x02);
		SAMPLE_VO_I2C_WRITE(0x7c, 0x8c);
		SAMPLE_VO_I2C_WRITE(0x7d, 0xd4);
		SAMPLE_VO_I2C_WRITE(0x7e, 0x72);
		SAMPLE_VO_I2C_WRITE(0x7f, 0x00);
		SAMPLE_VO_I2C_WRITE(0x80, 0x00);
		SAMPLE_VO_I2C_WRITE(0x81, 0x00);
		SAMPLE_VO_I2C_WRITE(0x82, 0x04);
		SAMPLE_VO_I2C_WRITE(0x83, 0x00);
		SAMPLE_VO_I2C_WRITE(0x84, 0xff);
		SAMPLE_VO_I2C_WRITE(0x85, 0xce);
		SAMPLE_VO_I2C_WRITE(0x86, 0xb2);
		SAMPLE_VO_I2C_WRITE(0x87, 0x00);
		SAMPLE_VO_I2C_WRITE(0x88, 0x00);
		SAMPLE_VO_I2C_WRITE(0x89, 0x93);
		SAMPLE_VO_I2C_WRITE(0x8a, 0x06);
		SAMPLE_VO_I2C_WRITE(0x5f, 0x01);
		SAMPLE_VO_I2C_WRITE(0x2e, 0x10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x56);
		SAMPLE_VO_I2C_WRITE(0x2e, 0x11);
		SAMPLE_VO_I2C_WRITE(0x20, 0x50);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x56);
		SAMPLE_VO_I2C_WRITE(0x20, 0x50);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);

		// delay 100ms
		delay_ms(100);
		SAMPLE_VO_I2C_WRITE(0x05, 0x07);
		SAMPLE_VO_I2C_WRITE(0x06, 0x0f);
		} else if (selection == MS7024_DVIN_480P60) {
		SAMPLE_VO_I2C_WRITE(0x04, 0x1c);
		SAMPLE_VO_I2C_WRITE(0x0e, 0x57);
		SAMPLE_VO_I2C_WRITE(0x0f, 0x01);
		SAMPLE_VO_I2C_WRITE(0x30, 0x02);
		SAMPLE_VO_I2C_WRITE(0x31, 0x4a);
		SAMPLE_VO_I2C_WRITE(0x32, 0x03);
		SAMPLE_VO_I2C_WRITE(0x33, 0x7a);
		SAMPLE_VO_I2C_WRITE(0x34, 0x00);
		SAMPLE_VO_I2C_WRITE(0x35, 0x07);
		SAMPLE_VO_I2C_WRITE(0x36, 0x02);
		SAMPLE_VO_I2C_WRITE(0x37, 0x27);
		SAMPLE_VO_I2C_WRITE(0x38, 0x00);
		SAMPLE_VO_I2C_WRITE(0x39, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x3c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x90, 0x02);
		SAMPLE_VO_I2C_WRITE(0x91, 0x00);
		SAMPLE_VO_I2C_WRITE(0x92, 0x00);
		SAMPLE_VO_I2C_WRITE(0x93, 0x00);
		SAMPLE_VO_I2C_WRITE(0x94, 0x00);
		SAMPLE_VO_I2C_WRITE(0x95, 0x00);
		SAMPLE_VO_I2C_WRITE(0x96, 0x00);
		SAMPLE_VO_I2C_WRITE(0x97, 0x00);
		SAMPLE_VO_I2C_WRITE(0x98, 0x00);
		SAMPLE_VO_I2C_WRITE(0x99, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9e, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9e, 0x00);
		SAMPLE_VO_I2C_WRITE(0x9f, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa0, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa1, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa2, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa4, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa5, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa6, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa7, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa8, 0x00);
		SAMPLE_VO_I2C_WRITE(0xa9, 0x07);
		SAMPLE_VO_I2C_WRITE(0xaa, 0x02);
		SAMPLE_VO_I2C_WRITE(0xab, 0x05);
		SAMPLE_VO_I2C_WRITE(0xac, 0x15);
		SAMPLE_VO_I2C_WRITE(0xad, 0x89);
		SAMPLE_VO_I2C_WRITE(0x50, 0x20);
		SAMPLE_VO_I2C_WRITE(0x51, 0x16);
		SAMPLE_VO_I2C_WRITE(0x52, 0x0b);
		SAMPLE_VO_I2C_WRITE(0x53, 0x00);
		SAMPLE_VO_I2C_WRITE(0x54, 0x00);
		SAMPLE_VO_I2C_WRITE(0x55, 0x00);
		SAMPLE_VO_I2C_WRITE(0x56, 0x00);
		SAMPLE_VO_I2C_WRITE(0x57, 0x00);
		SAMPLE_VO_I2C_WRITE(0x58, 0x00);
		SAMPLE_VO_I2C_WRITE(0x59, 0x00);
		SAMPLE_VO_I2C_WRITE(0x5a, 0x00);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);
		SAMPLE_VO_I2C_WRITE(0x21, 0x11);
		SAMPLE_VO_I2C_WRITE(0x22, 0x63);
		SAMPLE_VO_I2C_WRITE(0x23, 0x01);
		SAMPLE_VO_I2C_WRITE(0x24, 0x00);
		SAMPLE_VO_I2C_WRITE(0x25, 0x00);
		SAMPLE_VO_I2C_WRITE(0x26, 0x00);
		SAMPLE_VO_I2C_WRITE(0x27, 0xc1);
		SAMPLE_VO_I2C_WRITE(0x28, 0xc1);
		SAMPLE_VO_I2C_WRITE(0x29, 0x80);
		SAMPLE_VO_I2C_WRITE(0x2a, 0x80);
		SAMPLE_VO_I2C_WRITE(0x2b, 0x00);
		SAMPLE_VO_I2C_WRITE(0x2c, 0x00);
		SAMPLE_VO_I2C_WRITE(0x2d, 0x00);
		SAMPLE_VO_I2C_WRITE(0x60, 0x03);
		SAMPLE_VO_I2C_WRITE(0x61, 0x00);
		SAMPLE_VO_I2C_WRITE(0x62, 0x01);
		SAMPLE_VO_I2C_WRITE(0x63, 0x00);
		SAMPLE_VO_I2C_WRITE(0x64, 0x20);
		SAMPLE_VO_I2C_WRITE(0x66, 0x00);
		SAMPLE_VO_I2C_WRITE(0x67, 0x40);
		SAMPLE_VO_I2C_WRITE(0x68, 0x00);
		SAMPLE_VO_I2C_WRITE(0x69, 0x20);
		SAMPLE_VO_I2C_WRITE(0x6a, 0x40);
		SAMPLE_VO_I2C_WRITE(0x6b, 0x60);
		SAMPLE_VO_I2C_WRITE(0x6c, 0x80);
		SAMPLE_VO_I2C_WRITE(0x6d, 0xa0);
		SAMPLE_VO_I2C_WRITE(0x6e, 0xc0);
		SAMPLE_VO_I2C_WRITE(0x6f, 0xe0);
		SAMPLE_VO_I2C_WRITE(0x70, 0xff);
		SAMPLE_VO_I2C_WRITE(0x71, 0x03);
		SAMPLE_VO_I2C_WRITE(0x72, 0x4b);
		SAMPLE_VO_I2C_WRITE(0x73, 0x40);
		SAMPLE_VO_I2C_WRITE(0x74, 0x40);
		SAMPLE_VO_I2C_WRITE(0x75, 0x40);
		SAMPLE_VO_I2C_WRITE(0x76, 0x40);
		SAMPLE_VO_I2C_WRITE(0x77, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x78, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x79, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x7a, 0x5b);
		SAMPLE_VO_I2C_WRITE(0x7b, 0x02);
		SAMPLE_VO_I2C_WRITE(0x7c, 0x8c);
		SAMPLE_VO_I2C_WRITE(0x7d, 0xd4);
		SAMPLE_VO_I2C_WRITE(0x7e, 0x72);
		SAMPLE_VO_I2C_WRITE(0x7f, 0x00);
		SAMPLE_VO_I2C_WRITE(0x80, 0x00);
		SAMPLE_VO_I2C_WRITE(0x81, 0x00);
		SAMPLE_VO_I2C_WRITE(0x82, 0x04);
		SAMPLE_VO_I2C_WRITE(0x83, 0x00);
		SAMPLE_VO_I2C_WRITE(0x84, 0xff);
		SAMPLE_VO_I2C_WRITE(0x85, 0xce);
		SAMPLE_VO_I2C_WRITE(0x86, 0xb2);
		SAMPLE_VO_I2C_WRITE(0x87, 0x00);
		SAMPLE_VO_I2C_WRITE(0x88, 0x00);
		SAMPLE_VO_I2C_WRITE(0x89, 0x93);
		SAMPLE_VO_I2C_WRITE(0x8a, 0x06);
		SAMPLE_VO_I2C_WRITE(0x5f, 0x01);
		SAMPLE_VO_I2C_WRITE(0x2e, 0x10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x56);
		SAMPLE_VO_I2C_WRITE(0x2e, 0x11);
		SAMPLE_VO_I2C_WRITE(0x20, 0x50);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x56);
		SAMPLE_VO_I2C_WRITE(0x20, 0x50);

		// delay 10ms
		delay_ms(10);
		SAMPLE_VO_I2C_WRITE(0x20, 0x59);

		// delay 100ms
		delay_ms(100);
		SAMPLE_VO_I2C_WRITE(0x05, 0x07);
		SAMPLE_VO_I2C_WRITE(0x06, 0x0f);
	}
	//deinit i2c
	SAMPLE_COMM_I2C_Close(sample_vo_i2c_file);
	return CVI_SUCCESS;
}

int SAMPLE_COMM_VO_MIPI_INIT(int fd, int devno, const struct dsc_instr *cmds, int size)
{
	CVI_S32 ret = CVI_SUCCESS;

	for (CVI_S32 i = 0; i < size; i++) {
		const struct dsc_instr *instr = &cmds[i];
		struct cmd_info_s cmd_info = {
			.devno = devno,
			.cmd_size = instr->size,
			.data_type = instr->data_type,
			.cmd = (void *)instr->data
		};

		ret = ioctl(fd, CVI_VIP_MIPI_TX_SET_CMD, &cmd_info);
		if (instr->delay)
			usleep(instr->delay * 1000);

		if (ret == -1) {
			perror("CVI_VIP_MIPI_TX_SET_CMD");
			SAMPLE_PRT("dsi init failed at %d instr.\n", i);
			return CVI_FAILURE;
		}
	}
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_VO_Init_MIPI_HX8394(void *pvData)
{
	int fd = open(MIPI_TX_NAME, O_RDWR | O_NONBLOCK, 0);

	SAMPLE_PRT("+\n");

	UNUSED(pvData);

	if (fd == -1) {
		SAMPLE_PRT("Cannot open '%s': %d, %s\n", MIPI_TX_NAME, errno, strerror(errno));
		return -1;
	}

	if (-1 == ioctl(fd, CVI_VIP_MIPI_TX_DISABLE, NULL)) {
		perror("CVI_VIP_MIPI_TX_DISABLE");
		return -1;
	}

	if (-1 == ioctl(fd, CVI_VIP_MIPI_TX_SET_DEV_CFG, &dev_cfg_hx8394_720x1280)) {
		perror("CVI_VIP_MIPI_TX_SET_DEV_CFG");
		return -1;
	}
	SAMPLE_COMM_VO_MIPI_INIT(fd, 0, dsi_init_cmds_hx8394_720x1280, ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280));
	if (-1 == ioctl(fd, CVI_VIP_MIPI_TX_SET_HS_SETTLE, &hs_timing_cfg_hx8394_720x1280)) {
		perror("CVI_VIP_MIPI_TX_SET_HS_SETTLE");
		return -1;
	}
	if (-1 == ioctl(fd, CVI_VIP_MIPI_TX_ENABLE, NULL)) {
		perror("CVI_VIP_MIPI_TX_ENABLE");
		return -1;
	}

	close(fd);
	return CVI_SUCCESS;
}
