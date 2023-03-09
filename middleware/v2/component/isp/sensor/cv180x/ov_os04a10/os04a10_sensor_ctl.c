#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#ifdef ARCH_CV182X
#include <linux/cvi_vip_snsr.h>
#include "cvi_comm_video.h"
#else
#include <linux/vi_snsr.h>
#include <linux/cvi_comm_video.h>
#endif
#include "cvi_sns_ctrl.h"
#include "os04a10_cmos_ex.h"

static void os04a10_wdr_1520p30_2to1_init(VI_PIPE ViPipe);
static void os04a10_linear_1520p30_12BIT_init(VI_PIPE ViPipe);

CVI_U8 os04a10_i2c_addr = 0x36;        /* I2C Address of OS04A10 */
const CVI_U32 os04a10_addr_byte = 2;
const CVI_U32 os04a10_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};
ISP_SNS_MIRRORFLIP_TYPE_E g_aeOs04a10_MirrorFip_Initial[VI_MAX_PIPE_NUM] = {
	ISP_SNS_MIRROR, ISP_SNS_MIRROR, ISP_SNS_MIRROR, ISP_SNS_MIRROR};

int os04a10_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunOs04a10_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, os04a10_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}
	return CVI_SUCCESS;
}

int os04a10_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int os04a10_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (os04a10_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, os04a10_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, os04a10_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (os04a10_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int os04a10_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (os04a10_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (os04a10_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, os04a10_addr_byte + os04a10_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}
/*
 *static void delay_ms(int ms)
 *{
 *	usleep(ms * 1000);
 *}
 */
void os04a10_standby(VI_PIPE ViPipe)
{
	os04a10_write_register(ViPipe, 0x0100, 0x00); /* STANDBY */
}

void os04a10_restart(VI_PIPE ViPipe)
{
	os04a10_write_register(ViPipe, 0x0100, 0x01); /* resume */
}

void os04a10_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;
	CVI_U32 start = 1;
	CVI_U32 end = g_pastOs04a10[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum - 3;

	for (i = start; i < end; i++) {
		os04a10_write_register(ViPipe,
				g_pastOs04a10[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOs04a10[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		CVI_TRACE_SNS(CVI_DBG_INFO, "i2c_addr:%#x, i2c_data:%#x\n",
			g_pastOs04a10[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
			g_pastOs04a10[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

ISP_SNS_MIRRORFLIP_TYPE_E aeSnsMirrorFlipMap[ISP_SNS_BUTT][ISP_SNS_BUTT] = {
	{ISP_SNS_NORMAL, ISP_SNS_MIRROR, ISP_SNS_FLIP, ISP_SNS_MIRROR_FLIP},
	{ISP_SNS_MIRROR, ISP_SNS_NORMAL, ISP_SNS_MIRROR_FLIP, ISP_SNS_FLIP},
	{ISP_SNS_FLIP, ISP_SNS_MIRROR_FLIP, ISP_SNS_NORMAL, ISP_SNS_MIRROR},
	{ISP_SNS_MIRROR_FLIP, ISP_SNS_FLIP, ISP_SNS_MIRROR, ISP_SNS_NORMAL}
};

#define OS04A10_ORIEN_ADDR (0x3820)
void os04a10_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 val = 0;
	CVI_U32 i = 0;

	for (i = 0; i < ISP_SNS_BUTT; i++) {
		if (g_aeOs04a10_MirrorFip_Initial[ViPipe] == aeSnsMirrorFlipMap[i][0]) {
			eSnsMirrorFlip = aeSnsMirrorFlipMap[i][eSnsMirrorFlip];
			break;
		}
	}

	val = os04a10_read_register(ViPipe, OS04A10_ORIEN_ADDR);
	val &= ~(0x3 << 1);

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		val |= 0x1<<1;
		break;
	case ISP_SNS_FLIP:
		val |= 0x1<<2;
		break;
	case ISP_SNS_MIRROR_FLIP:
		val |= 0x1<<1;
		val |= 0x1<<2;
		break;
	default:
		return;
	}

	os04a10_standby(ViPipe);
	os04a10_write_register(ViPipe, OS04A10_ORIEN_ADDR, val);
	usleep(1000*100);
	os04a10_restart(ViPipe);
}

#define OS04A10_CHIP_ID_ADDR_H		0x300A
#define OS04A10_CHIP_ID_ADDR_M		0x300B
#define OS04A10_CHIP_ID_ADDR_L		0x300C
#define OS04A10_CHIP_ID			0x530441

int os04a10_probe(VI_PIPE ViPipe)
{
	int nVal, nVal2, nVal3;

	usleep(500);
	if (os04a10_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = os04a10_read_register(ViPipe, OS04A10_CHIP_ID_ADDR_H);
	nVal2 = os04a10_read_register(ViPipe, OS04A10_CHIP_ID_ADDR_M);
	nVal3 = os04a10_read_register(ViPipe, OS04A10_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0 || nVal3 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 16) | ((nVal2 & 0xFF) << 8) | (nVal3 & 0xFF)) != OS04A10_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void os04a10_init(VI_PIPE ViPipe)
{
	WDR_MODE_E        enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastOs04a10[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastOs04a10[ViPipe]->u8ImgMode;

	os04a10_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == OS04A10_MODE_1440P30_WDR)
			os04a10_wdr_1520p30_2to1_init(ViPipe);
	} else {
		if (u8ImgMode == OS04A10_MODE_1440P30_12BIT)
			os04a10_linear_1520p30_12BIT_init(ViPipe);
	}
	g_pastOs04a10[ViPipe]->bInit = CVI_TRUE;
}

void os04a10_exit(VI_PIPE ViPipe)
{
	os04a10_i2c_exit(ViPipe);
}

static void os04a10_linear_1520p30_12BIT_init(VI_PIPE ViPipe)
{
	os04a10_write_register(ViPipe, 0x0103, 0x01);
	os04a10_write_register(ViPipe, 0x0109, 0x01);
	os04a10_write_register(ViPipe, 0x0104, 0x02);
	os04a10_write_register(ViPipe, 0x0102, 0x00);
	os04a10_write_register(ViPipe, 0x0305, 0x5c);
	os04a10_write_register(ViPipe, 0x0306, 0x00);
	os04a10_write_register(ViPipe, 0x0307, 0x00);
	os04a10_write_register(ViPipe, 0x0308, 0x05);
	os04a10_write_register(ViPipe, 0x030a, 0x01);
	os04a10_write_register(ViPipe, 0x0317, 0x0a);
	os04a10_write_register(ViPipe, 0x0322, 0x01);
	os04a10_write_register(ViPipe, 0x0323, 0x02);
	os04a10_write_register(ViPipe, 0x0324, 0x00);
	os04a10_write_register(ViPipe, 0x0325, 0xd0);
	os04a10_write_register(ViPipe, 0x0327, 0x05);
	os04a10_write_register(ViPipe, 0x0329, 0x02);
	os04a10_write_register(ViPipe, 0x032c, 0x02);
	os04a10_write_register(ViPipe, 0x032d, 0x02);
	os04a10_write_register(ViPipe, 0x032e, 0x02);
	os04a10_write_register(ViPipe, 0x300f, 0x11);
	os04a10_write_register(ViPipe, 0x3012, 0x41);
	os04a10_write_register(ViPipe, 0x3026, 0x10);
	os04a10_write_register(ViPipe, 0x3027, 0x08);
	os04a10_write_register(ViPipe, 0x302d, 0x24);
	os04a10_write_register(ViPipe, 0x3104, 0x01);
	os04a10_write_register(ViPipe, 0x3106, 0x11);
	os04a10_write_register(ViPipe, 0x3400, 0x00);
	os04a10_write_register(ViPipe, 0x3408, 0x05);
	os04a10_write_register(ViPipe, 0x340c, 0x0c);
	os04a10_write_register(ViPipe, 0x340d, 0xb0);
	os04a10_write_register(ViPipe, 0x3425, 0x51);
	os04a10_write_register(ViPipe, 0x3426, 0x10);
	os04a10_write_register(ViPipe, 0x3427, 0x14);
	os04a10_write_register(ViPipe, 0x3428, 0x10);
	os04a10_write_register(ViPipe, 0x3429, 0x10);
	os04a10_write_register(ViPipe, 0x342a, 0x10);
	os04a10_write_register(ViPipe, 0x342b, 0x04);
	os04a10_write_register(ViPipe, 0x3501, 0x02);
	os04a10_write_register(ViPipe, 0x3504, 0x08);
	os04a10_write_register(ViPipe, 0x3508, 0x01);
	os04a10_write_register(ViPipe, 0x3509, 0x00);
	os04a10_write_register(ViPipe, 0x350a, 0x01);
	os04a10_write_register(ViPipe, 0x3544, 0x08);
	os04a10_write_register(ViPipe, 0x3548, 0x01);
	os04a10_write_register(ViPipe, 0x3549, 0x00);
	os04a10_write_register(ViPipe, 0x3584, 0x08);
	os04a10_write_register(ViPipe, 0x3588, 0x01);
	os04a10_write_register(ViPipe, 0x3589, 0x00);
	os04a10_write_register(ViPipe, 0x3601, 0x70);
	os04a10_write_register(ViPipe, 0x3604, 0xe3);
	os04a10_write_register(ViPipe, 0x3605, 0xff);
	os04a10_write_register(ViPipe, 0x3606, 0x01);
	os04a10_write_register(ViPipe, 0x3608, 0xa8);
	os04a10_write_register(ViPipe, 0x360a, 0xd0);
	os04a10_write_register(ViPipe, 0x360b, 0x08);
	os04a10_write_register(ViPipe, 0x360e, 0xc8);
	os04a10_write_register(ViPipe, 0x360f, 0x66);
	os04a10_write_register(ViPipe, 0x3610, 0x89);
	os04a10_write_register(ViPipe, 0x3611, 0x8a);
	os04a10_write_register(ViPipe, 0x3612, 0x4e);
	os04a10_write_register(ViPipe, 0x3613, 0xbd);
	os04a10_write_register(ViPipe, 0x3614, 0x9b);
	os04a10_write_register(ViPipe, 0x362a, 0x0e);
	os04a10_write_register(ViPipe, 0x362b, 0x0e);
	os04a10_write_register(ViPipe, 0x362c, 0x0e);
	os04a10_write_register(ViPipe, 0x362d, 0x09);
	os04a10_write_register(ViPipe, 0x362e, 0x1a);
	os04a10_write_register(ViPipe, 0x362f, 0x34);
	os04a10_write_register(ViPipe, 0x3630, 0x67);
	os04a10_write_register(ViPipe, 0x3631, 0x7f);
	os04a10_write_register(ViPipe, 0x3638, 0x00);
	os04a10_write_register(ViPipe, 0x3643, 0x00);
	os04a10_write_register(ViPipe, 0x3644, 0x00);
	os04a10_write_register(ViPipe, 0x3645, 0x00);
	os04a10_write_register(ViPipe, 0x3646, 0x00);
	os04a10_write_register(ViPipe, 0x3647, 0x00);
	os04a10_write_register(ViPipe, 0x3648, 0x00);
	os04a10_write_register(ViPipe, 0x3649, 0x00);
	os04a10_write_register(ViPipe, 0x364a, 0x04);
	os04a10_write_register(ViPipe, 0x364c, 0x0e);
	os04a10_write_register(ViPipe, 0x364d, 0x0e);
	os04a10_write_register(ViPipe, 0x364e, 0x0e);
	os04a10_write_register(ViPipe, 0x364f, 0x0e);
	os04a10_write_register(ViPipe, 0x3650, 0xff);
	os04a10_write_register(ViPipe, 0x3651, 0xff);
	os04a10_write_register(ViPipe, 0x365a, 0x00);
	os04a10_write_register(ViPipe, 0x365b, 0x00);
	os04a10_write_register(ViPipe, 0x365c, 0x00);
	os04a10_write_register(ViPipe, 0x365d, 0x00);
	os04a10_write_register(ViPipe, 0x3661, 0x07);
	os04a10_write_register(ViPipe, 0x3662, 0x00);
	os04a10_write_register(ViPipe, 0x3663, 0x20);
	os04a10_write_register(ViPipe, 0x3665, 0x12);
	os04a10_write_register(ViPipe, 0x3667, 0xd4);
	os04a10_write_register(ViPipe, 0x3668, 0x80);
	os04a10_write_register(ViPipe, 0x366c, 0x00);
	os04a10_write_register(ViPipe, 0x366d, 0x00);
	os04a10_write_register(ViPipe, 0x366e, 0x00);
	os04a10_write_register(ViPipe, 0x366f, 0x00);
	os04a10_write_register(ViPipe, 0x3671, 0x08);
	os04a10_write_register(ViPipe, 0x3673, 0x2a);
	os04a10_write_register(ViPipe, 0x3681, 0x80);
	os04a10_write_register(ViPipe, 0x3700, 0x2d);
	os04a10_write_register(ViPipe, 0x3701, 0x22);
	os04a10_write_register(ViPipe, 0x3702, 0x25);
	os04a10_write_register(ViPipe, 0x3703, 0x28);
	os04a10_write_register(ViPipe, 0x3705, 0x00);
	os04a10_write_register(ViPipe, 0x3706, 0xf0);
	os04a10_write_register(ViPipe, 0x3707, 0x0a);
	os04a10_write_register(ViPipe, 0x3708, 0x36);
	os04a10_write_register(ViPipe, 0x3709, 0x57);
	os04a10_write_register(ViPipe, 0x370a, 0x03);
	os04a10_write_register(ViPipe, 0x370b, 0x15);
	os04a10_write_register(ViPipe, 0x3714, 0x01);
	os04a10_write_register(ViPipe, 0x3719, 0x24);
	os04a10_write_register(ViPipe, 0x371b, 0x1f);
	os04a10_write_register(ViPipe, 0x371c, 0x00);
	os04a10_write_register(ViPipe, 0x371d, 0x08);
	os04a10_write_register(ViPipe, 0x373f, 0x63);
	os04a10_write_register(ViPipe, 0x3740, 0x63);
	os04a10_write_register(ViPipe, 0x3741, 0x63);
	os04a10_write_register(ViPipe, 0x3742, 0x63);
	os04a10_write_register(ViPipe, 0x3743, 0x01);
	os04a10_write_register(ViPipe, 0x3756, 0xe7);
	os04a10_write_register(ViPipe, 0x3757, 0xe7);
	os04a10_write_register(ViPipe, 0x3762, 0x1c);
	os04a10_write_register(ViPipe, 0x376c, 0x10);
	os04a10_write_register(ViPipe, 0x3776, 0x05);
	os04a10_write_register(ViPipe, 0x3777, 0x22);
	os04a10_write_register(ViPipe, 0x3779, 0x60);
	os04a10_write_register(ViPipe, 0x377c, 0x48);
	os04a10_write_register(ViPipe, 0x3784, 0x06);
	os04a10_write_register(ViPipe, 0x3785, 0x0a);
	os04a10_write_register(ViPipe, 0x3790, 0x10);
	os04a10_write_register(ViPipe, 0x3793, 0x04);
	os04a10_write_register(ViPipe, 0x3794, 0x07);
	os04a10_write_register(ViPipe, 0x3796, 0x00);
	os04a10_write_register(ViPipe, 0x3797, 0x02);
	os04a10_write_register(ViPipe, 0x379c, 0x4d);
	os04a10_write_register(ViPipe, 0x37a1, 0x80);
	os04a10_write_register(ViPipe, 0x37bb, 0x88);
	os04a10_write_register(ViPipe, 0x37be, 0x48);
	os04a10_write_register(ViPipe, 0x37bf, 0x01);
	os04a10_write_register(ViPipe, 0x37c0, 0x01);
	os04a10_write_register(ViPipe, 0x37c4, 0x72);
	os04a10_write_register(ViPipe, 0x37c5, 0x72);
	os04a10_write_register(ViPipe, 0x37c6, 0x72);
	os04a10_write_register(ViPipe, 0x37ca, 0x21);
	os04a10_write_register(ViPipe, 0x37cc, 0x15);
	os04a10_write_register(ViPipe, 0x37cd, 0x90);
	os04a10_write_register(ViPipe, 0x37cf, 0x02);
	os04a10_write_register(ViPipe, 0x37d0, 0x00);
	os04a10_write_register(ViPipe, 0x37d1, 0xf0);
	os04a10_write_register(ViPipe, 0x37d2, 0x03);
	os04a10_write_register(ViPipe, 0x37d3, 0x15);
	os04a10_write_register(ViPipe, 0x37d4, 0x01);
	os04a10_write_register(ViPipe, 0x37d5, 0x00);
	os04a10_write_register(ViPipe, 0x37d6, 0x03);
	os04a10_write_register(ViPipe, 0x37d7, 0x15);
	os04a10_write_register(ViPipe, 0x37d8, 0x01);
	os04a10_write_register(ViPipe, 0x37dc, 0x00);
	os04a10_write_register(ViPipe, 0x37dd, 0x00);
	os04a10_write_register(ViPipe, 0x37da, 0x00);
	os04a10_write_register(ViPipe, 0x37db, 0x00);
	os04a10_write_register(ViPipe, 0x3800, 0x00);
	os04a10_write_register(ViPipe, 0x3801, 0x00);
	os04a10_write_register(ViPipe, 0x3802, 0x00);
	os04a10_write_register(ViPipe, 0x3803, 0x00);
	os04a10_write_register(ViPipe, 0x3804, 0x0a);
	os04a10_write_register(ViPipe, 0x3805, 0x8f);
	os04a10_write_register(ViPipe, 0x3806, 0x05);
	os04a10_write_register(ViPipe, 0x3807, 0xff);
	os04a10_write_register(ViPipe, 0x3808, 0x0a);
	os04a10_write_register(ViPipe, 0x3809, 0x80);
	os04a10_write_register(ViPipe, 0x380a, 0x05);
	os04a10_write_register(ViPipe, 0x380b, 0xf0);
	os04a10_write_register(ViPipe, 0x380c, 0x05);
	os04a10_write_register(ViPipe, 0x380d, 0xcc);
	os04a10_write_register(ViPipe, 0x380e, 0x09);
	os04a10_write_register(ViPipe, 0x380f, 0x80);
	os04a10_write_register(ViPipe, 0x3811, 0x08);
	os04a10_write_register(ViPipe, 0x3813, 0x08);
	os04a10_write_register(ViPipe, 0x3814, 0x01);
	os04a10_write_register(ViPipe, 0x3815, 0x01);
	os04a10_write_register(ViPipe, 0x3816, 0x01);
	os04a10_write_register(ViPipe, 0x3817, 0x01);
	os04a10_write_register(ViPipe, 0x381c, 0x00);
	os04a10_write_register(ViPipe, 0x3820, 0x02);
	os04a10_write_register(ViPipe, 0x3821, 0x00);
	os04a10_write_register(ViPipe, 0x3822, 0x14);
	os04a10_write_register(ViPipe, 0x3823, 0x18);
	os04a10_write_register(ViPipe, 0x3826, 0x00);
	os04a10_write_register(ViPipe, 0x3827, 0x00);
	os04a10_write_register(ViPipe, 0x3833, 0x40);
	os04a10_write_register(ViPipe, 0x384c, 0x05);
	os04a10_write_register(ViPipe, 0x384d, 0xc4);
	os04a10_write_register(ViPipe, 0x3858, 0x3c);
	os04a10_write_register(ViPipe, 0x3865, 0x02);
	os04a10_write_register(ViPipe, 0x3866, 0x00);
	os04a10_write_register(ViPipe, 0x3867, 0x00);
	os04a10_write_register(ViPipe, 0x3868, 0x02);
	os04a10_write_register(ViPipe, 0x3900, 0x13);
	os04a10_write_register(ViPipe, 0x3940, 0x13);
	os04a10_write_register(ViPipe, 0x3980, 0x13);
	os04a10_write_register(ViPipe, 0x3c01, 0x11);
	os04a10_write_register(ViPipe, 0x3c05, 0x00);
	os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
	os04a10_write_register(ViPipe, 0x3c12, 0x0d);
	os04a10_write_register(ViPipe, 0x3c19, 0x00);
	os04a10_write_register(ViPipe, 0x3c21, 0x00);
	os04a10_write_register(ViPipe, 0x3c3a, 0x10);
	os04a10_write_register(ViPipe, 0x3c3b, 0x18);
	os04a10_write_register(ViPipe, 0x3c3d, 0xc6);
	os04a10_write_register(ViPipe, 0x3c55, 0x08);
	os04a10_write_register(ViPipe, 0x3c5a, 0xe5);
	os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
	os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
	os04a10_write_register(ViPipe, 0x3d8c, 0x70);
	os04a10_write_register(ViPipe, 0x3d8d, 0x10);
	os04a10_write_register(ViPipe, 0x4000, 0xf9);
	os04a10_write_register(ViPipe, 0x4001, 0x2f);
	os04a10_write_register(ViPipe, 0x4004, 0x00);
	os04a10_write_register(ViPipe, 0x4005, 0x80);
	os04a10_write_register(ViPipe, 0x4008, 0x02);
	os04a10_write_register(ViPipe, 0x4009, 0x11);
	os04a10_write_register(ViPipe, 0x400a, 0x03);
	os04a10_write_register(ViPipe, 0x400b, 0x27);
	os04a10_write_register(ViPipe, 0x400e, 0x40);
	os04a10_write_register(ViPipe, 0x402e, 0x00);
	os04a10_write_register(ViPipe, 0x402f, 0x80);
	os04a10_write_register(ViPipe, 0x4030, 0x00);
	os04a10_write_register(ViPipe, 0x4031, 0x80);
	os04a10_write_register(ViPipe, 0x4032, 0x9f);
	os04a10_write_register(ViPipe, 0x4033, 0x80);
	os04a10_write_register(ViPipe, 0x4050, 0x00);
	os04a10_write_register(ViPipe, 0x4051, 0x07);
	os04a10_write_register(ViPipe, 0x4011, 0xbb);
	os04a10_write_register(ViPipe, 0x410f, 0x01);
	os04a10_write_register(ViPipe, 0x4288, 0xcf);
	os04a10_write_register(ViPipe, 0x4289, 0x00);
	os04a10_write_register(ViPipe, 0x428a, 0x46);
	os04a10_write_register(ViPipe, 0x430b, 0xff);
	os04a10_write_register(ViPipe, 0x430c, 0xff);
	os04a10_write_register(ViPipe, 0x430d, 0x00);
	os04a10_write_register(ViPipe, 0x430e, 0x00);
	os04a10_write_register(ViPipe, 0x4314, 0x04);
	os04a10_write_register(ViPipe, 0x4500, 0x18);
	os04a10_write_register(ViPipe, 0x4501, 0x18);
	os04a10_write_register(ViPipe, 0x4503, 0x10);
	os04a10_write_register(ViPipe, 0x4504, 0x00);
	os04a10_write_register(ViPipe, 0x4506, 0x32);
	os04a10_write_register(ViPipe, 0x4507, 0x02);
	os04a10_write_register(ViPipe, 0x4601, 0x30);
	os04a10_write_register(ViPipe, 0x4603, 0x00);
	os04a10_write_register(ViPipe, 0x460a, 0x50);
	os04a10_write_register(ViPipe, 0x460c, 0x60);
	os04a10_write_register(ViPipe, 0x4640, 0x62);
	os04a10_write_register(ViPipe, 0x4646, 0xaa);
	os04a10_write_register(ViPipe, 0x4647, 0x55);
	os04a10_write_register(ViPipe, 0x4648, 0x99);
	os04a10_write_register(ViPipe, 0x4649, 0x66);
	os04a10_write_register(ViPipe, 0x464d, 0x00);
	os04a10_write_register(ViPipe, 0x4654, 0x11);
	os04a10_write_register(ViPipe, 0x4655, 0x22);
	os04a10_write_register(ViPipe, 0x4800, 0x44);
	os04a10_write_register(ViPipe, 0x480e, 0x00);
	os04a10_write_register(ViPipe, 0x4810, 0xff);
	os04a10_write_register(ViPipe, 0x4811, 0xff);
	os04a10_write_register(ViPipe, 0x4813, 0x00);
	os04a10_write_register(ViPipe, 0x481f, 0x30);
	os04a10_write_register(ViPipe, 0x4837, 0x0d);
	os04a10_write_register(ViPipe, 0x484b, 0x27);
	os04a10_write_register(ViPipe, 0x4d00, 0x4d);
	os04a10_write_register(ViPipe, 0x4d01, 0x9d);
	os04a10_write_register(ViPipe, 0x4d02, 0xb9);
	os04a10_write_register(ViPipe, 0x4d03, 0x2e);
	os04a10_write_register(ViPipe, 0x4d04, 0x4a);
	os04a10_write_register(ViPipe, 0x4d05, 0x3d);
	os04a10_write_register(ViPipe, 0x4d09, 0x4f);
	os04a10_write_register(ViPipe, 0x5000, 0x7f);
	os04a10_write_register(ViPipe, 0x5001, 0x0d);
	os04a10_write_register(ViPipe, 0x5080, 0x00);
	os04a10_write_register(ViPipe, 0x50c0, 0x00);
	os04a10_write_register(ViPipe, 0x5100, 0x00);
	os04a10_write_register(ViPipe, 0x5200, 0x00);
	os04a10_write_register(ViPipe, 0x5201, 0x00);
	os04a10_write_register(ViPipe, 0x5202, 0x03);
	os04a10_write_register(ViPipe, 0x5203, 0xff);
	os04a10_write_register(ViPipe, 0x5780, 0x53);
	os04a10_write_register(ViPipe, 0x5782, 0x60);
	os04a10_write_register(ViPipe, 0x5783, 0xf0);
	os04a10_write_register(ViPipe, 0x5786, 0x01);
	os04a10_write_register(ViPipe, 0x5788, 0x60);
	os04a10_write_register(ViPipe, 0x5789, 0xf0);
	os04a10_write_register(ViPipe, 0x5792, 0x11);
	os04a10_write_register(ViPipe, 0x5793, 0x33);
	os04a10_write_register(ViPipe, 0x5857, 0xff);
	os04a10_write_register(ViPipe, 0x5858, 0xff);
	os04a10_write_register(ViPipe, 0x5859, 0xff);
	os04a10_write_register(ViPipe, 0x58d7, 0xff);
	os04a10_write_register(ViPipe, 0x58d8, 0xff);
	os04a10_write_register(ViPipe, 0x58d9, 0xff);

	os04a10_default_reg_init(ViPipe);
	os04a10_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===OS04A10 1520P 30fps 12bit LINE Init OK!===\n", ViPipe);
}

static void os04a10_wdr_1520p30_2to1_init(VI_PIPE ViPipe)
{
	os04a10_write_register(ViPipe, 0x0103, 0x01);
	os04a10_write_register(ViPipe, 0x0109, 0x01);
	os04a10_write_register(ViPipe, 0x0104, 0x02);
	os04a10_write_register(ViPipe, 0x0102, 0x00);
	os04a10_write_register(ViPipe, 0x0305, 0x3c);
	os04a10_write_register(ViPipe, 0x0306, 0x00);
	os04a10_write_register(ViPipe, 0x0307, 0x00);
	os04a10_write_register(ViPipe, 0x0308, 0x04);
	os04a10_write_register(ViPipe, 0x030a, 0x01);
	os04a10_write_register(ViPipe, 0x0317, 0x09);
	os04a10_write_register(ViPipe, 0x0322, 0x01);
	os04a10_write_register(ViPipe, 0x0323, 0x02);
	os04a10_write_register(ViPipe, 0x0324, 0x00);
	os04a10_write_register(ViPipe, 0x0325, 0x90);
	os04a10_write_register(ViPipe, 0x0327, 0x05);
	os04a10_write_register(ViPipe, 0x0329, 0x02);
	os04a10_write_register(ViPipe, 0x032c, 0x02);
	os04a10_write_register(ViPipe, 0x032d, 0x02);
	os04a10_write_register(ViPipe, 0x032e, 0x02);
	os04a10_write_register(ViPipe, 0x300f, 0x11);
	os04a10_write_register(ViPipe, 0x3012, 0x41);
	os04a10_write_register(ViPipe, 0x3026, 0x10);
	os04a10_write_register(ViPipe, 0x3027, 0x08);
	os04a10_write_register(ViPipe, 0x302d, 0x24);
	os04a10_write_register(ViPipe, 0x3104, 0x01);
	os04a10_write_register(ViPipe, 0x3106, 0x11);
	os04a10_write_register(ViPipe, 0x3400, 0x00);
	os04a10_write_register(ViPipe, 0x3408, 0x05);
	os04a10_write_register(ViPipe, 0x340c, 0x0c);
	os04a10_write_register(ViPipe, 0x340d, 0xb0);
	os04a10_write_register(ViPipe, 0x3425, 0x51);
	os04a10_write_register(ViPipe, 0x3426, 0x10);
	os04a10_write_register(ViPipe, 0x3427, 0x14);
	os04a10_write_register(ViPipe, 0x3428, 0x10);
	os04a10_write_register(ViPipe, 0x3429, 0x10);
	os04a10_write_register(ViPipe, 0x342a, 0x10);
	os04a10_write_register(ViPipe, 0x342b, 0x04);
	os04a10_write_register(ViPipe, 0x3501, 0x02);
	os04a10_write_register(ViPipe, 0x3504, 0x08);
	os04a10_write_register(ViPipe, 0x3508, 0x01);
	os04a10_write_register(ViPipe, 0x3509, 0x00);
	os04a10_write_register(ViPipe, 0x350a, 0x01);
	os04a10_write_register(ViPipe, 0x3544, 0x08);
	os04a10_write_register(ViPipe, 0x3548, 0x01);
	os04a10_write_register(ViPipe, 0x3549, 0x00);
	os04a10_write_register(ViPipe, 0x3584, 0x08);
	os04a10_write_register(ViPipe, 0x3588, 0x01);
	os04a10_write_register(ViPipe, 0x3589, 0x00);
	os04a10_write_register(ViPipe, 0x3601, 0x70);
	os04a10_write_register(ViPipe, 0x3604, 0xe3);
	os04a10_write_register(ViPipe, 0x3605, 0x7f);
	os04a10_write_register(ViPipe, 0x3606, 0x80);
	os04a10_write_register(ViPipe, 0x3608, 0xa8);
	os04a10_write_register(ViPipe, 0x360a, 0xd0);
	os04a10_write_register(ViPipe, 0x360b, 0x08);
	os04a10_write_register(ViPipe, 0x360e, 0xc8);
	os04a10_write_register(ViPipe, 0x360f, 0x66);
	os04a10_write_register(ViPipe, 0x3610, 0x89);
	os04a10_write_register(ViPipe, 0x3611, 0x8a);
	os04a10_write_register(ViPipe, 0x3612, 0x4e);
	os04a10_write_register(ViPipe, 0x3613, 0xbd);
	os04a10_write_register(ViPipe, 0x3614, 0x9b);
	os04a10_write_register(ViPipe, 0x362a, 0x0e);
	os04a10_write_register(ViPipe, 0x362b, 0x0e);
	os04a10_write_register(ViPipe, 0x362c, 0x0e);
	os04a10_write_register(ViPipe, 0x362d, 0x0e);
	os04a10_write_register(ViPipe, 0x362e, 0x1a);
	os04a10_write_register(ViPipe, 0x362f, 0x34);
	os04a10_write_register(ViPipe, 0x3630, 0x67);
	os04a10_write_register(ViPipe, 0x3631, 0x7f);
	os04a10_write_register(ViPipe, 0x3638, 0x00);
	os04a10_write_register(ViPipe, 0x3643, 0x00);
	os04a10_write_register(ViPipe, 0x3644, 0x00);
	os04a10_write_register(ViPipe, 0x3645, 0x00);
	os04a10_write_register(ViPipe, 0x3646, 0x00);
	os04a10_write_register(ViPipe, 0x3647, 0x00);
	os04a10_write_register(ViPipe, 0x3648, 0x00);
	os04a10_write_register(ViPipe, 0x3649, 0x00);
	os04a10_write_register(ViPipe, 0x364a, 0x04);
	os04a10_write_register(ViPipe, 0x364c, 0x0e);
	os04a10_write_register(ViPipe, 0x364d, 0x0e);
	os04a10_write_register(ViPipe, 0x364e, 0x0e);
	os04a10_write_register(ViPipe, 0x364f, 0x0e);
	os04a10_write_register(ViPipe, 0x3650, 0xff);
	os04a10_write_register(ViPipe, 0x3651, 0xff);
	os04a10_write_register(ViPipe, 0x365a, 0x00);
	os04a10_write_register(ViPipe, 0x365b, 0x00);
	os04a10_write_register(ViPipe, 0x365c, 0x00);
	os04a10_write_register(ViPipe, 0x365d, 0x00);
	os04a10_write_register(ViPipe, 0x3661, 0x07);
	os04a10_write_register(ViPipe, 0x3662, 0x02);
	os04a10_write_register(ViPipe, 0x3663, 0x20);
	os04a10_write_register(ViPipe, 0x3665, 0x12);
	os04a10_write_register(ViPipe, 0x3667, 0x54);
	os04a10_write_register(ViPipe, 0x3668, 0x80);
	os04a10_write_register(ViPipe, 0x366c, 0x00);
	os04a10_write_register(ViPipe, 0x366d, 0x00);
	os04a10_write_register(ViPipe, 0x366e, 0x00);
	os04a10_write_register(ViPipe, 0x366f, 0x00);
	os04a10_write_register(ViPipe, 0x3671, 0x09);
	os04a10_write_register(ViPipe, 0x3673, 0x2a);
	os04a10_write_register(ViPipe, 0x3681, 0x80);
	os04a10_write_register(ViPipe, 0x3700, 0x2d);
	os04a10_write_register(ViPipe, 0x3701, 0x22);
	os04a10_write_register(ViPipe, 0x3702, 0x25);
	os04a10_write_register(ViPipe, 0x3703, 0x20);
	os04a10_write_register(ViPipe, 0x3705, 0x00);
	os04a10_write_register(ViPipe, 0x3706, 0x72);
	os04a10_write_register(ViPipe, 0x3707, 0x0a);
	os04a10_write_register(ViPipe, 0x3708, 0x36);
	os04a10_write_register(ViPipe, 0x3709, 0x57);
	os04a10_write_register(ViPipe, 0x370a, 0x01);
	os04a10_write_register(ViPipe, 0x370b, 0x14);
	os04a10_write_register(ViPipe, 0x3714, 0x01);
	os04a10_write_register(ViPipe, 0x3719, 0x1f);
	os04a10_write_register(ViPipe, 0x371b, 0x16);
	os04a10_write_register(ViPipe, 0x371c, 0x00);
	os04a10_write_register(ViPipe, 0x371d, 0x08);
	os04a10_write_register(ViPipe, 0x373f, 0x63);
	os04a10_write_register(ViPipe, 0x3740, 0x63);
	os04a10_write_register(ViPipe, 0x3741, 0x63);
	os04a10_write_register(ViPipe, 0x3742, 0x63);
	os04a10_write_register(ViPipe, 0x3743, 0x01);
	os04a10_write_register(ViPipe, 0x3756, 0x9d);
	os04a10_write_register(ViPipe, 0x3757, 0x9d);
	os04a10_write_register(ViPipe, 0x3762, 0x1c);
	os04a10_write_register(ViPipe, 0x376c, 0x34);
	os04a10_write_register(ViPipe, 0x3776, 0x05);
	os04a10_write_register(ViPipe, 0x3777, 0x22);
	os04a10_write_register(ViPipe, 0x3779, 0x60);
	os04a10_write_register(ViPipe, 0x377c, 0x48);
	os04a10_write_register(ViPipe, 0x3784, 0x06);
	os04a10_write_register(ViPipe, 0x3785, 0x0a);
	os04a10_write_register(ViPipe, 0x3790, 0x10);
	os04a10_write_register(ViPipe, 0x3793, 0x04);
	os04a10_write_register(ViPipe, 0x3794, 0x07);
	os04a10_write_register(ViPipe, 0x3796, 0x00);
	os04a10_write_register(ViPipe, 0x3797, 0x02);
	os04a10_write_register(ViPipe, 0x379c, 0x4d);
	os04a10_write_register(ViPipe, 0x37a1, 0x80);
	os04a10_write_register(ViPipe, 0x37bb, 0x88);
	os04a10_write_register(ViPipe, 0x37be, 0x48);
	os04a10_write_register(ViPipe, 0x37bf, 0x01);
	os04a10_write_register(ViPipe, 0x37c0, 0x01);
	os04a10_write_register(ViPipe, 0x37c4, 0x72);
	os04a10_write_register(ViPipe, 0x37c5, 0x72);
	os04a10_write_register(ViPipe, 0x37c6, 0x72);
	os04a10_write_register(ViPipe, 0x37ca, 0x21);
	os04a10_write_register(ViPipe, 0x37cc, 0x13);
	os04a10_write_register(ViPipe, 0x37cd, 0x90);
	os04a10_write_register(ViPipe, 0x37cf, 0x02);
	os04a10_write_register(ViPipe, 0x37d0, 0x00);
	os04a10_write_register(ViPipe, 0x37d1, 0x72);
	os04a10_write_register(ViPipe, 0x37d2, 0x01);
	os04a10_write_register(ViPipe, 0x37d3, 0x14);
	os04a10_write_register(ViPipe, 0x37d4, 0x00);
	os04a10_write_register(ViPipe, 0x37d5, 0x6c);
	os04a10_write_register(ViPipe, 0x37d6, 0x00);
	os04a10_write_register(ViPipe, 0x37d7, 0xf7);
	os04a10_write_register(ViPipe, 0x37d8, 0x01);
	os04a10_write_register(ViPipe, 0x37dc, 0x00);
	os04a10_write_register(ViPipe, 0x37dd, 0x00);
	os04a10_write_register(ViPipe, 0x37da, 0x00);
	os04a10_write_register(ViPipe, 0x37db, 0x00);
	os04a10_write_register(ViPipe, 0x3800, 0x00);
	os04a10_write_register(ViPipe, 0x3801, 0x00);
	os04a10_write_register(ViPipe, 0x3802, 0x00);
	os04a10_write_register(ViPipe, 0x3803, 0x00);
	os04a10_write_register(ViPipe, 0x3804, 0x0a);
	os04a10_write_register(ViPipe, 0x3805, 0x8f);
	os04a10_write_register(ViPipe, 0x3806, 0x05);
	os04a10_write_register(ViPipe, 0x3807, 0xff);
	os04a10_write_register(ViPipe, 0x3808, 0x0a);
	os04a10_write_register(ViPipe, 0x3809, 0x80);
	os04a10_write_register(ViPipe, 0x380a, 0x05);
	os04a10_write_register(ViPipe, 0x380b, 0xf0);
	os04a10_write_register(ViPipe, 0x380c, 0x02);
	os04a10_write_register(ViPipe, 0x380d, 0xdc);
	os04a10_write_register(ViPipe, 0x380e, 0x06);
	os04a10_write_register(ViPipe, 0x380f, 0x58);
	os04a10_write_register(ViPipe, 0x3811, 0x08);
	os04a10_write_register(ViPipe, 0x3813, 0x08);
	os04a10_write_register(ViPipe, 0x3814, 0x01);
	os04a10_write_register(ViPipe, 0x3815, 0x01);
	os04a10_write_register(ViPipe, 0x3816, 0x01);
	os04a10_write_register(ViPipe, 0x3817, 0x01);
	os04a10_write_register(ViPipe, 0x381c, 0x08);
	os04a10_write_register(ViPipe, 0x3820, 0x03);
	os04a10_write_register(ViPipe, 0x3821, 0x00);
	os04a10_write_register(ViPipe, 0x3822, 0x14);
	os04a10_write_register(ViPipe, 0x3823, 0x18);
	os04a10_write_register(ViPipe, 0x3826, 0x00);
	os04a10_write_register(ViPipe, 0x3827, 0x00);
	os04a10_write_register(ViPipe, 0x3833, 0x41);
	os04a10_write_register(ViPipe, 0x384c, 0x02);
	os04a10_write_register(ViPipe, 0x384d, 0xdc);
	os04a10_write_register(ViPipe, 0x3858, 0x3c);
	os04a10_write_register(ViPipe, 0x3865, 0x02);
	os04a10_write_register(ViPipe, 0x3866, 0x00);
	os04a10_write_register(ViPipe, 0x3867, 0x00);
	os04a10_write_register(ViPipe, 0x3868, 0x02);
	os04a10_write_register(ViPipe, 0x3900, 0x13);
	os04a10_write_register(ViPipe, 0x3940, 0x13);
	os04a10_write_register(ViPipe, 0x3980, 0x13);
	os04a10_write_register(ViPipe, 0x3c01, 0x11);
	os04a10_write_register(ViPipe, 0x3c05, 0x00);
	os04a10_write_register(ViPipe, 0x3c0f, 0x1c);
	os04a10_write_register(ViPipe, 0x3c12, 0x0d);
	os04a10_write_register(ViPipe, 0x3c19, 0x00);
	os04a10_write_register(ViPipe, 0x3c21, 0x00);
	os04a10_write_register(ViPipe, 0x3c3a, 0x10);
	os04a10_write_register(ViPipe, 0x3c3b, 0x18);
	os04a10_write_register(ViPipe, 0x3c3d, 0xc6);
	os04a10_write_register(ViPipe, 0x3c55, 0x08);
	os04a10_write_register(ViPipe, 0x3c5a, 0x55);
	os04a10_write_register(ViPipe, 0x3c5d, 0xcf);
	os04a10_write_register(ViPipe, 0x3c5e, 0xcf);
	os04a10_write_register(ViPipe, 0x3d8c, 0x70);
	os04a10_write_register(ViPipe, 0x3d8d, 0x10);
	os04a10_write_register(ViPipe, 0x4000, 0xf9);
	os04a10_write_register(ViPipe, 0x4001, 0xef);
	os04a10_write_register(ViPipe, 0x4004, 0x00);
	os04a10_write_register(ViPipe, 0x4005, 0x40);
	os04a10_write_register(ViPipe, 0x4008, 0x02);
	os04a10_write_register(ViPipe, 0x4009, 0x11);
	os04a10_write_register(ViPipe, 0x400a, 0x06);
	os04a10_write_register(ViPipe, 0x400b, 0x40);
	os04a10_write_register(ViPipe, 0x400e, 0x40);
	os04a10_write_register(ViPipe, 0x402e, 0x00);
	os04a10_write_register(ViPipe, 0x402f, 0x40);
	os04a10_write_register(ViPipe, 0x4030, 0x00);
	os04a10_write_register(ViPipe, 0x4031, 0x40);
	os04a10_write_register(ViPipe, 0x4032, 0x0f);
	os04a10_write_register(ViPipe, 0x4033, 0x80);
	os04a10_write_register(ViPipe, 0x4050, 0x00);
	os04a10_write_register(ViPipe, 0x4051, 0x07);
	os04a10_write_register(ViPipe, 0x4011, 0xbb);
	os04a10_write_register(ViPipe, 0x410f, 0x01);
	os04a10_write_register(ViPipe, 0x4288, 0xce);
	os04a10_write_register(ViPipe, 0x4289, 0x00);
	os04a10_write_register(ViPipe, 0x428a, 0x46);
	os04a10_write_register(ViPipe, 0x430b, 0x0f);
	os04a10_write_register(ViPipe, 0x430c, 0xfc);
	os04a10_write_register(ViPipe, 0x430d, 0x00);
	os04a10_write_register(ViPipe, 0x430e, 0x00);
	os04a10_write_register(ViPipe, 0x4314, 0x04);
	os04a10_write_register(ViPipe, 0x4500, 0x18);
	os04a10_write_register(ViPipe, 0x4501, 0x18);
	os04a10_write_register(ViPipe, 0x4503, 0x10);
	os04a10_write_register(ViPipe, 0x4504, 0x00);
	os04a10_write_register(ViPipe, 0x4506, 0x32);
	os04a10_write_register(ViPipe, 0x4507, 0x03);
	os04a10_write_register(ViPipe, 0x4601, 0x30);
	os04a10_write_register(ViPipe, 0x4603, 0x00);
	os04a10_write_register(ViPipe, 0x460a, 0x50);
	os04a10_write_register(ViPipe, 0x460c, 0x60);
	os04a10_write_register(ViPipe, 0x4640, 0x62);
	os04a10_write_register(ViPipe, 0x4646, 0xaa);
	os04a10_write_register(ViPipe, 0x4647, 0x55);
	os04a10_write_register(ViPipe, 0x4648, 0x99);
	os04a10_write_register(ViPipe, 0x4649, 0x66);
	os04a10_write_register(ViPipe, 0x464d, 0x00);
	os04a10_write_register(ViPipe, 0x4654, 0x11);
	os04a10_write_register(ViPipe, 0x4655, 0x22);
	os04a10_write_register(ViPipe, 0x4800, 0x44);
	os04a10_write_register(ViPipe, 0x480e, 0x04);
	os04a10_write_register(ViPipe, 0x4810, 0xff);
	os04a10_write_register(ViPipe, 0x4811, 0xff);
	os04a10_write_register(ViPipe, 0x4813, 0x84);
	os04a10_write_register(ViPipe, 0x481f, 0x30);
	os04a10_write_register(ViPipe, 0x4837, 0x0e);
	os04a10_write_register(ViPipe, 0x484b, 0x67);
	os04a10_write_register(ViPipe, 0x4d00, 0x4d);
	os04a10_write_register(ViPipe, 0x4d01, 0x9d);
	os04a10_write_register(ViPipe, 0x4d02, 0xb9);
	os04a10_write_register(ViPipe, 0x4d03, 0x2e);
	os04a10_write_register(ViPipe, 0x4d04, 0x4a);
	os04a10_write_register(ViPipe, 0x4d05, 0x3d);
	os04a10_write_register(ViPipe, 0x4d09, 0x4f);
	os04a10_write_register(ViPipe, 0x5000, 0x1f);
	os04a10_write_register(ViPipe, 0x5001, 0x0c);
	os04a10_write_register(ViPipe, 0x5080, 0x00);
	os04a10_write_register(ViPipe, 0x50c0, 0x00);
	os04a10_write_register(ViPipe, 0x5100, 0x00);
	os04a10_write_register(ViPipe, 0x5200, 0x00);
	os04a10_write_register(ViPipe, 0x5201, 0x00);
	os04a10_write_register(ViPipe, 0x5202, 0x03);
	os04a10_write_register(ViPipe, 0x5203, 0xff);
	os04a10_write_register(ViPipe, 0x5780, 0x53);
	os04a10_write_register(ViPipe, 0x5782, 0x18);
	os04a10_write_register(ViPipe, 0x5783, 0x3c);
	os04a10_write_register(ViPipe, 0x5786, 0x01);
	os04a10_write_register(ViPipe, 0x5788, 0x18);
	os04a10_write_register(ViPipe, 0x5789, 0x3c);
	os04a10_write_register(ViPipe, 0x5792, 0x11);
	os04a10_write_register(ViPipe, 0x5793, 0x33);
	os04a10_write_register(ViPipe, 0x5857, 0xff);
	os04a10_write_register(ViPipe, 0x5858, 0xff);
	os04a10_write_register(ViPipe, 0x5859, 0xff);
	os04a10_write_register(ViPipe, 0x58d7, 0xff);
	os04a10_write_register(ViPipe, 0x58d8, 0xff);
	os04a10_write_register(ViPipe, 0x58d9, 0xff);

	os04a10_default_reg_init(ViPipe);
	os04a10_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===OS04A10 1520P 30fps 10bit 2to1 WDR Init OK!===\n", ViPipe);
}

