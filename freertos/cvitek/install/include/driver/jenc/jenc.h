/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __DRV_JENC_H__
#define __DRV_JENC_H__
#ifdef FREERTOS_BSP
#include "cvi_type.h"

int jpu_enc(CVI_U32 width, CVI_U32 height, CVI_U32 phySrcAddr, CVI_U32 phyDstAddr, CVI_U64 pts);

#else
#include <linux/cvi_type.h>
#endif

#define MAX_JPEG_NUM 16
typedef struct _JPEG_PIC {
	CVI_U32 width;
	CVI_U32 height;
	CVI_U32 addr;
	CVI_U32 size;
	CVI_U64 pts;
} JPEG_PIC;

#endif // end of __DRV_JENC_H__
