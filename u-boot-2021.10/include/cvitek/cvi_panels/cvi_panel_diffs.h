/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2021. All rights reserved.
 *
 * File Name: cvi_panel_diffs.h
 * Description:
 */

#ifndef __CVI_PANEL_DIFFS_H__
#define __CVI_PANEL_DIFFS_H__

#ifdef CONFIG_DISPLAY_CVITEK_I80
#define CVI_JPEG ""
#define START_VO "startvo 0 65536 0;"
#elif defined(CONFIG_DISPLAY_CVITEK_LVDS)
#define CVI_JPEG "cvi_jpeg_dec " LOGO_READ_ADDR " " LOGO_RESERVED_ADDR " " LOGOSIZE ";"
#define START_VO "startvo 0 2048 0;"
#else
#define CVI_JPEG "cvi_jpeg_dec " LOGO_READ_ADDR " " LOGO_RESERVED_ADDR " " LOGOSIZE ";"
#define START_VO "startvo 0 8192 0;"
#endif

#define START_VL "startvl 0 " LOGO_READ_ADDR " " LOGO_RESERVED_ADDR " " LOGOSIZE " " VO_ALIGNMENT ";"
#define SET_VO_BG "setvobg 0 0xffffffff;"

#endif
