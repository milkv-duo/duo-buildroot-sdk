/*
 * Media Bus API header
 *
 * Copyright (C) 2009, Guennadi Liakhovetski <g.liakhovetski@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_V4L2_MEDIABUS_H
#define __LINUX_V4L2_MEDIABUS_H

/**
 * struct v4l2_mbus_framefmt - frame format on the media bus
 * @width:	frame width
 * @height:	frame height
 * @code:	data format code (from enum v4l2_mbus_pixelcode)
 * @field:	used interlacing type (from enum v4l2_field)
 * @colorspace:	colorspace of the data (from enum v4l2_colorspace)
 * @ycbcr_enc:	YCbCr encoding of the data (from enum v4l2_ycbcr_encoding)
 * @quantization: quantization of the data (from enum v4l2_quantization)
 * @xfer_func:  transfer function of the data (from enum v4l2_xfer_func)
 */
struct v4l2_mbus_framefmt {
	__u32 width;
	__u32 height;
	__u32 code;
	__u32 field;
	__u32 colorspace;
	__u16 ycbcr_enc;
	__u16 quantization;
	__u16 xfer_func;
	__u16 reserved[11];
};

#endif
