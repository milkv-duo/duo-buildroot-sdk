/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2022. All rights reserved.
 *
 * File Name: cvi_ispd2.h
 * Description:
 */

#ifndef _CVI_ISPD2_H_
#define _CVI_ISPD2_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

void isp_daemon2_init(unsigned int port);
void isp_daemon2_uninit(void);

void isp_daemon2_enable_device_bind_control(int enable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _CVI_ISPD2_H_
