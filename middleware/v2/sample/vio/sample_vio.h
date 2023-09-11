/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: sample_vio.h
 * Description:
 */

#ifndef __SAMPLE_VIO_H__
#define __SAMPLE_VIO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <linux/cvi_common.h>

CVI_S32 SAMPLE_IMX327_N5_VI_VPSS_ONLINE_TEST(void);
CVI_S32 SAMPLE_IMX327_N5_TEST(void);
CVI_S32 SAMPLE_PICO640_TEST(void);
CVI_S32 SAMPLE_MM308M2_TEST(void);
CVI_S32 SAMPLE_N5_TEST(void);
CVI_S32 SAMPLE_MCS369Q_TEST(void);
CVI_S32 SAMPLE_MCS369_TEST(void);
CVI_S32 SAMPLE_VIO_TWO_DEV_VO(void);
CVI_S32 SAMPLE_VI_VPSS_SLT(CVI_S32 frmNum, char *filename);
CVI_S32 SAMPLE_VIO_VoRotation(void);
CVI_S32 SAMPLE_VIO_ViVpssAspectRatio(void);
CVI_S32 SAMPLE_VIO_ViRotation(void);
CVI_S32 SAMPLE_VIO_VpssRotation(void);
CVI_S32 SAMPLE_VIO_VpssFileIO(SIZE_S stSize);
CVI_S32 SAMPLE_VIO_VpssCombine2File(SIZE_S stSize);
CVI_S32 SAMPLE_PR2100_TEST(void);
CVI_S32 SAMPLE_IMX335_PR2020_OFFLINE_ONLINE_TEST(void);
CVI_S32 SAMPLE_IMX307_PR2020_OFFLINE_ONLINE_TEST(void);
CVI_S32 SAMPLE_IMX307_ONTHEFLY_ONLINE_SC_TEST(void);
CVI_S32 SAMPLE_VO_LVDS_TEST(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_H__*/
