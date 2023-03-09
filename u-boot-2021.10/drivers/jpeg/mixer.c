//#include <stdio.h>
#include "jpuapi.h"
#include "jpuhelper.h"
#include "jpulog.h"
#include "mixer.h"
#include "regdefine.h"
#include <linux/string.h>
#include <stdlib.h>

typedef struct {
	FRAME_BUF frameBuf[MAX_FRAME];
	jpu_buffer_t vb_base;
	int instIndex;
	int last_num;
	unsigned long last_addr;
} fb_context;

static fb_context s_fb[MAX_NUM_INSTANCE];

int AllocateFrameBuffer(int instIdx, int format, int strideY, int height,
			int frameBufNum, int pack, int strideC)
{
	unsigned int divX, divY;
	int i;
	unsigned int lum_size, chr_size;
	fb_context *fb;

	fb = &s_fb[instIdx];

	divX = format == FORMAT_420 || format == FORMAT_422 ? 2 : 1;
	divY = format == FORMAT_420 || format == FORMAT_224 ? 2 : 1;

	switch (format) {
	case FORMAT_420:
		height = (height + 1) / 2 * 2;
		strideY = (strideY + 1) / 2 * 2;
		break;
	case FORMAT_224:
		height = (height + 1) / 2 * 2;
		break;
	case FORMAT_422:
		strideY = (strideY + 1) / 2 * 2;
		break;
	case FORMAT_444:
		height = (height + 1) / 2 * 2;
		strideY = (strideY + 1) / 2 * 2;
		break;
	case FORMAT_400:
		height = (height + 1) / 2 * 2;
		strideY = (strideY + 1) / 2 * 2;
		break;
	}

	lum_size = (unsigned int)strideY * (unsigned int)height;

	if (pack)
		chr_size = 0;
	else {
		if (format == FORMAT_420)
			chr_size = strideC * height / divY;
		else
			chr_size = lum_size / divX / divY;
	}

	fb->vb_base.size = lum_size + chr_size * 2;
	fb->vb_base.size *= frameBufNum;

	if (jdi_allocate_dma_memory(&fb->vb_base) < 0) {
		JLOG(ERR, "Fail to allocate frame buffer size=%d\n", fb->vb_base.size);
		return 0;
	}

	fb->last_addr = fb->vb_base.phys_addr;

	for (i = fb->last_num; i < fb->last_num + frameBufNum; i++) {
		fb->frameBuf[i].Format = format;
		fb->frameBuf[i].Index = i;

		fb->frameBuf[i].vb_y.phys_addr = fb->last_addr;
		fb->frameBuf[i].vb_y.size = lum_size;

		fb->last_addr += fb->frameBuf[i].vb_y.size;
		fb->last_addr = ((fb->last_addr + 7) & ~7);

		if (chr_size) {
			fb->frameBuf[i].vb_cb.phys_addr = fb->last_addr;
			fb->frameBuf[i].vb_cb.size = chr_size;

			fb->last_addr += fb->frameBuf[i].vb_cb.size;
			fb->last_addr = ((fb->last_addr + 7) & ~7);

			fb->frameBuf[i].vb_cr.phys_addr = fb->last_addr;
			fb->frameBuf[i].vb_cr.size = chr_size;

			fb->last_addr += fb->frameBuf[i].vb_cr.size;
			fb->last_addr = ((fb->last_addr + 7) & ~7);
		}

		fb->frameBuf[i].strideY = strideY;
		if (format == FORMAT_420)
			fb->frameBuf[i].strideC = strideC;
		else
			fb->frameBuf[i].strideC = strideY / divX;
	}

	fb->last_num += frameBufNum;

	return 1;
}

int GetFrameBufBase(int instIdx)
{
	fb_context *fb;

	fb = &s_fb[instIdx];

	return fb->vb_base.phys_addr;
}

int GetFrameBufAllocSize(int instIdx)
{
	fb_context *fb;

	fb = &s_fb[instIdx];

	return (fb->last_addr - fb->vb_base.phys_addr);
}

FRAME_BUF *GetFrameBuffer(int instIdx, int index)
{
	fb_context *fb;

	fb = &s_fb[instIdx];
	return &fb->frameBuf[index];
}

FRAME_BUF *find_frame_buffer(int instIdx, PhysicalAddress addrY)
{
	int i;
	fb_context *fb;

	fb = &s_fb[instIdx];

	for (i = 0; i < MAX_FRAME; i++) {
		if (fb->frameBuf[i].vb_y.phys_addr == addrY) {
			return &fb->frameBuf[i];
		}
	}

	return NULL;
}

void clear_frame_buffer(int instIdx, int index)
{
}

void free_frame_buffer(int instIdx)
{
	fb_context *fb;

	fb = &s_fb[instIdx];

	fb->last_num = 0;
	fb->last_addr = -1;

	jdi_free_dma_memory(&fb->vb_base);
	fb->vb_base.base = 0;
	fb->vb_base.size = 0;
}

#ifdef JPU_FPGA_PLATFORM
//------------------------------------------------------------------------------
// MIXER REGISTER ADDRESS
//------------------------------------------------------------------------------
#define MIX_BASE 0x1000000
#define DISP_MIX 0x2000000

#define MIX_INT (MIX_BASE + 0x044)

#define MIX_STRIDE_Y (MIX_BASE + 0x144)
#define MIX_STRIDE_CB (MIX_BASE + 0x148)
#define MIX_STRIDE_CR (MIX_BASE + 0x14c)

#define MIX_ADDR_Y (MIX_BASE + 0x138)
#define MIX_ADDR_CB (MIX_BASE + 0x13C)
#define MIX_ADDR_CR (MIX_BASE + 0x140)

#define MIX_RUN (MIX_BASE + 0x120)

#define DISP_TOTAL_SAMPLE (DISP_MIX + 0x00C)
#define DISP_ACTIVE_SAMPLE (DISP_MIX + 0x010)
#define DISP_HSYNC_START_END (DISP_MIX + 0x014)
#define DISP_VSYNC_TOP_START (DISP_MIX + 0x018)
#define DISP_VSYNC_TOP_END (DISP_MIX + 0x01C)
#define DISP_VSYNC_BOT_START (DISP_MIX + 0x020)
#define DISP_VSYNC_BOT_END (DISP_MIX + 0x024)
#define DISP_ACTIVE_REGION_TOP (DISP_MIX + 0x02C)
#define DISP_ACTIVE_REGION_BOT (DISP_MIX + 0x030)

#define MIX_MIX_INTRPT (MIX_BASE + 0x0000)
#define MIX_SYNC_STATE (MIX_BASE + 0x0004)
#define MIX_SYNC_CTRL (MIX_BASE + 0x0008)
#define MIX_TOTAL_SAMPLE (MIX_BASE + 0x000c)
#define MIX_ACTIVE_SAMPLE (MIX_BASE + 0x0010)
#define MIX_HSYNC_START_END (MIX_BASE + 0x0014)
#define MIX_VSYNC_TOP_START (MIX_BASE + 0x0018)
#define MIX_VSYNC_TOP_END (MIX_BASE + 0x001c)
#define MIX_VSYNC_BOT_START (MIX_BASE + 0x0020)
#define MIX_VSYNC_BOT_END (MIX_BASE + 0x0024)
#define MIX_ACT_REGION_SAMPLE (MIX_BASE + 0x0028)
#define MIX_ACT_REGION_TOP (MIX_BASE + 0x002c)
#define MIX_ACT_REGION_BOT (MIX_BASE + 0x0030)
#define MIX_TOP_START (MIX_BASE + 0x0034)
#define MIX_BOT_START (MIX_BASE + 0x0038)
#define MIX_LINE_INC (MIX_BASE + 0x003c)
#define MIX_LATCH_PARAM_CTRL (MIX_BASE + 0x0040)
#define MIX_INTERRUPT (MIX_BASE + 0x0044)

#define MIX_LAYER_CTRL (MIX_BASE + 0x0100)
#define MIX_LAYER_ORDER (MIX_BASE + 0x0104)
#define MIX_BIG_ENDIAN (MIX_BASE + 0x0108)
#define MIX_L0_BG_COLOR (MIX_BASE + 0x0110)
#define MIX_L1_CTRL (MIX_BASE + 0x0120)
#define MIX_L1_LSIZE (MIX_BASE + 0x0124)
#define MIX_L1_SSIZE (MIX_BASE + 0x0128)
#define MIX_L1_LPOS (MIX_BASE + 0x012c)
#define MIX_L1_SPOS (MIX_BASE + 0x0130)
#define MIX_L1_BG_COLOR (MIX_BASE + 0x0134)
#define MIX_L1_Y_SADDR (MIX_BASE + 0x0138)
#define MIX_L1_CB_SADDR (MIX_BASE + 0x013c)
#define MIX_L1_CR_SADDR (MIX_BASE + 0x0140)
#define MIX_L1_Y_STRIDE (MIX_BASE + 0x0144)
#define MIX_L1_CB_STRIDE (MIX_BASE + 0x0148)
#define MIX_L1_CR_STRIDE (MIX_BASE + 0x014c)

int SetMixerDecOutFrame(FRAME_BUF *pFrame, int width, int height)
{
	int staX, staY;
	int div;

	staX = (MAX_DISPLAY_WIDTH - width) / 2;
	if (height > MAX_DISPLAY_HEIGHT)
		staY = 0;
	else
		staY = (MAX_DISPLAY_HEIGHT - height) / 2;
	if (staX % 16)
		staX = (staX + 15) / 16 * 16;

	JpuWriteReg(MIX_L0_BG_COLOR, (0 << 16) | (0x80 << 8) | 0x80);

	JpuWriteReg(MIX_L1_LSIZE, (height << 12) | width);
	JpuWriteReg(MIX_L1_SSIZE, (height << 12) | width);
	JpuWriteReg(MIX_L1_LPOS, (staY << 12) | staX);

	div = (pFrame->Format == FORMAT_420 || pFrame->Format == FORMAT_422 ||
	       pFrame->Format == FORMAT_400)
		  ? 2
		  : 1;

	JpuWriteReg(MIX_STRIDE_Y, width);
	JpuWriteReg(MIX_STRIDE_CB, width / div);
	JpuWriteReg(MIX_STRIDE_CR, width / div);

	JpuWriteReg(MIX_ADDR_Y, pFrame->vb_y.phys_addr);
	JpuWriteReg(MIX_ADDR_CB, pFrame->vb_cb.phys_addr);
	JpuWriteReg(MIX_ADDR_CR, pFrame->vb_cr.phys_addr);

	JpuWriteReg(DISP_HSYNC_START_END,
		    ((0x7d7 - 40) << 12) | (0x82f - 40)); // horizontal center
	JpuWriteReg(DISP_ACTIVE_REGION_TOP, ((0x014 - 2) << 12) | (0x230 - 2));
	JpuWriteReg(DISP_ACTIVE_REGION_BOT, ((0x247 - 2) << 12) | (0x463 - 2));

	JpuWriteReg(MIX_LAYER_CTRL, 0x3); // backgroup on
	JpuWriteReg(MIX_RUN, 0x92);	  // on, vdec, from sdram

	return 1;
}

int SetMixerDecOutLayer(int instIdx, int index, int width, int height)
{
	FRAME_BUF *pFrame;
	int staX, staY;
	int div;

	pFrame = GetFrameBuffer(instIdx, index);

	staX = (MAX_DISPLAY_WIDTH - width) / 2;
	if (height > MAX_DISPLAY_HEIGHT)
		staY = 0;
	else
		staY = (MAX_DISPLAY_HEIGHT - height) / 2;
	if (staX % 16)
		staX = (staX + 15) / 16 * 16;

	JpuWriteReg(MIX_L0_BG_COLOR, (0 << 16) | (0x80 << 8) | 0x80);

	JpuWriteReg(MIX_L1_LSIZE, (height << 12) | width);
	JpuWriteReg(MIX_L1_SSIZE, (height << 12) | width);
	JpuWriteReg(MIX_L1_LPOS, (staY << 12) | staX);

	div = (pFrame->Format == FORMAT_420 || pFrame->Format == FORMAT_422 ||
	       pFrame->Format == FORMAT_400)
		  ? 2
		  : 1;

	JpuWriteReg(MIX_STRIDE_Y, width);
	JpuWriteReg(MIX_STRIDE_CB, width / div);
	JpuWriteReg(MIX_STRIDE_CR, width / div);

	JpuWriteReg(MIX_ADDR_Y, pFrame->vb_y.phys_addr);
	JpuWriteReg(MIX_ADDR_CB, pFrame->vb_cb.phys_addr);
	JpuWriteReg(MIX_ADDR_CR, pFrame->vb_cr.phys_addr);

	JpuWriteReg(DISP_HSYNC_START_END, ((0x7d7 - 40) << 12) | (0x82f - 40)); // horizontal center
	JpuWriteReg(DISP_ACTIVE_REGION_TOP, ((0x014 - 2) << 12) | (0x230 - 2));
	JpuWriteReg(DISP_ACTIVE_REGION_BOT, ((0x247 - 2) << 12) | (0x463 - 2));

	JpuWriteReg(MIX_LAYER_CTRL, 0x3); // backgroup on
	JpuWriteReg(MIX_RUN, 0x92);	  // on, vdec, from sdram

	return 1;
}

void wait_mixer_int(void)
{
	int data;

	return;

	if (JpuReadReg(MIX_INT) == 1)
		JpuWriteReg(MIX_INT, 0);

	while (1) {
		data = JpuReadReg(MIX_INT);
		if (data & 1)
			break;
	}
	JpuWriteReg(MIX_INT, 0);
}

#endif

#ifdef PLATFORM_LINUX
#include <linux/fb.h>
#define FBDEV_FILENAME "/dev/fb0"

typedef struct {
	int s_fd;
	unsigned char *s_scr_ptr;
	unsigned char *s_rgb_ptr;
	unsigned long s_product;
	int s_fb_stride;
	int s_fb_height;
	int s_fb_width;
	int s_fb_bpp;
} sw_mixer_context_t;
#endif // PLATFORM_LINUX

int sw_mixer_open(int instIdx, int width, int height)
{
	return 0;
}

int sw_mixer_draw(int instIdx, int x, int y, int width, int height,
		  int planar_format, int pack_format, int interleave,
		  unsigned char *pbImage)
{
	return 0;
}

void sw_mixer_close(int instIdx)
{
}
