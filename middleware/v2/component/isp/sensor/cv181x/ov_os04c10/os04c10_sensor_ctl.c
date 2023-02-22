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
#include "os04c10_cmos_ex.h"

#define HW_SYNC_AUTO 1

static void os04c10_wdr_1520p30_2to1_init(VI_PIPE ViPipe);
static void os04c10_linear_1520p30_init(VI_PIPE ViPipe);
static void os04c10_wdr_1440p30_2to1_init(VI_PIPE ViPipe);
static void os04c10_linear_1440p30_init(VI_PIPE ViPipe);


CVI_U8 os04c10_i2c_addr = 0x10;        /* I2C Address of OS04C10 */
const CVI_U32 os04c10_addr_byte = 2;
const CVI_U32 os04c10_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int os04c10_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunOs04c10_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, os04c10_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int os04c10_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int os04c10_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (os04c10_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, os04c10_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, os04c10_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (os04c10_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int os04c10_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (os04c10_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (os04c10_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, os04c10_addr_byte + os04c10_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void os04c10_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			os04c10_write_register(ViPipe, addr, data);
	}
}

void os04c10_standby(VI_PIPE ViPipe)
{
	os04c10_write_register(ViPipe, 0x0100, 0x00); /* STANDBY */
}

void os04c10_restart(VI_PIPE ViPipe)
{
	os04c10_write_register(ViPipe, 0x0100, 0x01); /* standby */
}

void os04c10_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;
	CVI_U32 start = 1;
	CVI_U32 end = g_pastOs04c10[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum - 3;

	for (i = start; i < end; i++) {
		os04c10_write_register(ViPipe,
				g_pastOs04c10[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOs04c10[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

void os04c10_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 orien1, orien2;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		orien1 = 0x80;
		orien2 = 0x24;
		break;
	case ISP_SNS_MIRROR:
		orien1 = 0x88;
		orien2 = 0x24;
		break;
	case ISP_SNS_FLIP:
		orien1 = 0xB0;
		orien2 = 0x04;
		break;
	case ISP_SNS_MIRROR_FLIP:
		orien1 = 0xB8;
		orien2 = 0x04;
		break;
	default:
		return;
	}

	os04c10_write_register(ViPipe, 0x3820, orien1);
	os04c10_write_register(ViPipe, 0x3716, orien2);
}
#define OS04C10_CHIP_ID_ADDR_H		0x300A
#define OS04C10_CHIP_ID_ADDR_M		0x300B
#define OS04C10_CHIP_ID_ADDR_L		0x300C
#define OS04C10_CHIP_ID			0x530443

int os04c10_probe(VI_PIPE ViPipe)
{
	int nVal, nVal2, nVal3;

	usleep(500);
	if (os04c10_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = os04c10_read_register(ViPipe, OS04C10_CHIP_ID_ADDR_H);
	nVal2 = os04c10_read_register(ViPipe, OS04C10_CHIP_ID_ADDR_M);
	nVal3 = os04c10_read_register(ViPipe, OS04C10_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0 || nVal3 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 16) | ((nVal2 & 0xFF) << 8) | (nVal3 & 0xFF)) != OS04C10_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void os04c10_init(VI_PIPE ViPipe)
{
	WDR_MODE_E        enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastOs04c10[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastOs04c10[ViPipe]->u8ImgMode;

	os04c10_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == OS04C10_MODE_2688X1520P30_WDR)
			os04c10_wdr_1520p30_2to1_init(ViPipe);
		else if (u8ImgMode == OS04C10_MODE_2560X1440P30_WDR)
			os04c10_wdr_1440p30_2to1_init(ViPipe);
		else {
		}
	} else {
		if (u8ImgMode == OS04C10_MODE_2688X1520P30)
			os04c10_linear_1520p30_init(ViPipe);
		else if (u8ImgMode == OS04C10_MODE_2560X1440P30)
			os04c10_linear_1440p30_init(ViPipe);
		else {
		}
	}
	g_pastOs04c10[ViPipe]->bInit = CVI_TRUE;
}

void os04c10_exit(VI_PIPE ViPipe)
{
	os04c10_i2c_exit(ViPipe);
}

/* 1520P30 and 1520P25 */
static void os04c10_linear_1520p30_init(VI_PIPE ViPipe)
{
	os04c10_write_register(ViPipe, 0x0100, 0x00);
	os04c10_write_register(ViPipe, 0x0103, 0x01);
	os04c10_write_register(ViPipe, 0x0301, 0xe4);
	os04c10_write_register(ViPipe, 0x0303, 0x01);
	os04c10_write_register(ViPipe, 0x0305, 0x6e);
	os04c10_write_register(ViPipe, 0x0306, 0x01);
	os04c10_write_register(ViPipe, 0x0307, 0x17);
	os04c10_write_register(ViPipe, 0x0323, 0x04);
	os04c10_write_register(ViPipe, 0x0324, 0x01);
	os04c10_write_register(ViPipe, 0x0325, 0x62);
	os04c10_write_register(ViPipe, 0x3012, 0x06);
	os04c10_write_register(ViPipe, 0x3013, 0x02);
	os04c10_write_register(ViPipe, 0x3016, 0x72);
	os04c10_write_register(ViPipe, 0x3021, 0x03);
	os04c10_write_register(ViPipe, 0x3106, 0x21);
	os04c10_write_register(ViPipe, 0x3107, 0xa1);
	os04c10_write_register(ViPipe, 0x3500, 0x00);
	os04c10_write_register(ViPipe, 0x3501, 0x06);
	os04c10_write_register(ViPipe, 0x3502, 0x1e);
	os04c10_write_register(ViPipe, 0x3503, 0x88);
	os04c10_write_register(ViPipe, 0x3508, 0x00);
	os04c10_write_register(ViPipe, 0x3509, 0x80);
	os04c10_write_register(ViPipe, 0x350a, 0x04);
	os04c10_write_register(ViPipe, 0x350b, 0x00);
	os04c10_write_register(ViPipe, 0x350c, 0x00);
	os04c10_write_register(ViPipe, 0x350d, 0x80);
	os04c10_write_register(ViPipe, 0x350e, 0x04);
	os04c10_write_register(ViPipe, 0x350f, 0x00);
	os04c10_write_register(ViPipe, 0x3510, 0x00);
	os04c10_write_register(ViPipe, 0x3511, 0x00);
	os04c10_write_register(ViPipe, 0x3512, 0x20);
	os04c10_write_register(ViPipe, 0x3624, 0x02);
	os04c10_write_register(ViPipe, 0x3625, 0x4c);
	os04c10_write_register(ViPipe, 0x3660, 0x00);
	os04c10_write_register(ViPipe, 0x3666, 0xa5);
	os04c10_write_register(ViPipe, 0x3667, 0xa5);
	os04c10_write_register(ViPipe, 0x366a, 0x60);
	os04c10_write_register(ViPipe, 0x3673, 0x0d);
	os04c10_write_register(ViPipe, 0x3672, 0x0d);
	os04c10_write_register(ViPipe, 0x3671, 0x0d);
	os04c10_write_register(ViPipe, 0x3670, 0x0d);
	os04c10_write_register(ViPipe, 0x3685, 0x00);
	os04c10_write_register(ViPipe, 0x3694, 0x0d);
	os04c10_write_register(ViPipe, 0x3693, 0x0d);
	os04c10_write_register(ViPipe, 0x3692, 0x0d);
	os04c10_write_register(ViPipe, 0x3691, 0x0d);
	os04c10_write_register(ViPipe, 0x3696, 0x4c);
	os04c10_write_register(ViPipe, 0x3697, 0x4c);
	os04c10_write_register(ViPipe, 0x3698, 0x40);
	os04c10_write_register(ViPipe, 0x3699, 0x80);
	os04c10_write_register(ViPipe, 0x369a, 0x18);
	os04c10_write_register(ViPipe, 0x369b, 0x1f);
	os04c10_write_register(ViPipe, 0x369c, 0x14);
	os04c10_write_register(ViPipe, 0x369d, 0x80);
	os04c10_write_register(ViPipe, 0x369e, 0x40);
	os04c10_write_register(ViPipe, 0x369f, 0x21);
	os04c10_write_register(ViPipe, 0x36a0, 0x12);
	os04c10_write_register(ViPipe, 0x36a1, 0x5d);
	os04c10_write_register(ViPipe, 0x36a2, 0x66);
	os04c10_write_register(ViPipe, 0x370a, 0x02);
	os04c10_write_register(ViPipe, 0x370e, 0x0c);
	os04c10_write_register(ViPipe, 0x3710, 0x00);
	os04c10_write_register(ViPipe, 0x3713, 0x00);
	os04c10_write_register(ViPipe, 0x3725, 0x02);
	os04c10_write_register(ViPipe, 0x372a, 0x03);
	os04c10_write_register(ViPipe, 0x3738, 0xce);
	os04c10_write_register(ViPipe, 0x3739, 0x10);
	os04c10_write_register(ViPipe, 0x3748, 0x02);
	os04c10_write_register(ViPipe, 0x374a, 0x02);
	os04c10_write_register(ViPipe, 0x374c, 0x02);
	os04c10_write_register(ViPipe, 0x374e, 0x02);
	os04c10_write_register(ViPipe, 0x3756, 0x00);
	os04c10_write_register(ViPipe, 0x3757, 0x0e);
	os04c10_write_register(ViPipe, 0x3767, 0x00);
	os04c10_write_register(ViPipe, 0x3771, 0x00);
	os04c10_write_register(ViPipe, 0x377b, 0x20);
	os04c10_write_register(ViPipe, 0x377c, 0x00);
	os04c10_write_register(ViPipe, 0x377d, 0x0c);
	os04c10_write_register(ViPipe, 0x3781, 0x03);
	os04c10_write_register(ViPipe, 0x3782, 0x00);
	os04c10_write_register(ViPipe, 0x3789, 0x14);
	os04c10_write_register(ViPipe, 0x3795, 0x02);
	os04c10_write_register(ViPipe, 0x379c, 0x00);
	os04c10_write_register(ViPipe, 0x379d, 0x00);
	os04c10_write_register(ViPipe, 0x37b8, 0x04);
	os04c10_write_register(ViPipe, 0x37ba, 0x03);
	os04c10_write_register(ViPipe, 0x37bb, 0x00);
	os04c10_write_register(ViPipe, 0x37bc, 0x04);
	os04c10_write_register(ViPipe, 0x37be, 0x08);
	os04c10_write_register(ViPipe, 0x37c4, 0x11);
	os04c10_write_register(ViPipe, 0x37c5, 0x80);
	os04c10_write_register(ViPipe, 0x37c6, 0x14);
	os04c10_write_register(ViPipe, 0x37c7, 0x08);
	os04c10_write_register(ViPipe, 0x37da, 0x11);
	os04c10_write_register(ViPipe, 0x381f, 0x08);
	os04c10_write_register(ViPipe, 0x3829, 0x03);
	os04c10_write_register(ViPipe, 0x3832, 0x00);
	os04c10_write_register(ViPipe, 0x3881, 0x00);
	os04c10_write_register(ViPipe, 0x3888, 0x04);
	os04c10_write_register(ViPipe, 0x388b, 0x00);
	os04c10_write_register(ViPipe, 0x3c80, 0x10);
	os04c10_write_register(ViPipe, 0x3c86, 0x00);
	os04c10_write_register(ViPipe, 0x3c9f, 0x01);
	os04c10_write_register(ViPipe, 0x3d85, 0x1b);
	os04c10_write_register(ViPipe, 0x3d8c, 0x71);
	os04c10_write_register(ViPipe, 0x3d8d, 0xe2);
	os04c10_write_register(ViPipe, 0x3f00, 0x0b);
	os04c10_write_register(ViPipe, 0x3f06, 0x04);
	os04c10_write_register(ViPipe, 0x400a, 0x01);
	os04c10_write_register(ViPipe, 0x400b, 0x50);
	os04c10_write_register(ViPipe, 0x400e, 0x08);
	os04c10_write_register(ViPipe, 0x4040, 0x00);
	os04c10_write_register(ViPipe, 0x4041, 0x07);
	os04c10_write_register(ViPipe, 0x4043, 0x7e);
	os04c10_write_register(ViPipe, 0x4045, 0x7e);
	os04c10_write_register(ViPipe, 0x4047, 0x7e);
	os04c10_write_register(ViPipe, 0x4049, 0x7e);
	os04c10_write_register(ViPipe, 0x4090, 0x04);
	os04c10_write_register(ViPipe, 0x40b0, 0x00);
	os04c10_write_register(ViPipe, 0x40b1, 0x00);
	os04c10_write_register(ViPipe, 0x40b2, 0x00);
	os04c10_write_register(ViPipe, 0x40b3, 0x00);
	os04c10_write_register(ViPipe, 0x40b4, 0x00);
	os04c10_write_register(ViPipe, 0x40b5, 0x00);
	os04c10_write_register(ViPipe, 0x40b7, 0x00);
	os04c10_write_register(ViPipe, 0x40b8, 0x00);
	os04c10_write_register(ViPipe, 0x40b9, 0x00);
	os04c10_write_register(ViPipe, 0x40ba, 0x00);
	os04c10_write_register(ViPipe, 0x4301, 0x00);
	os04c10_write_register(ViPipe, 0x4303, 0x00);
	os04c10_write_register(ViPipe, 0x4502, 0x04);
	os04c10_write_register(ViPipe, 0x4503, 0x00);
	os04c10_write_register(ViPipe, 0x4504, 0x06);
	os04c10_write_register(ViPipe, 0x4506, 0x00);
	os04c10_write_register(ViPipe, 0x4507, 0x64);
	os04c10_write_register(ViPipe, 0x4803, 0x00);
	os04c10_write_register(ViPipe, 0x480c, 0x32);
	os04c10_write_register(ViPipe, 0x480e, 0x00);
	os04c10_write_register(ViPipe, 0x4813, 0x00);
	os04c10_write_register(ViPipe, 0x4819, 0x70);
	os04c10_write_register(ViPipe, 0x481f, 0x30);
	os04c10_write_register(ViPipe, 0x4823, 0x3f);
	os04c10_write_register(ViPipe, 0x4825, 0x30);
	os04c10_write_register(ViPipe, 0x4833, 0x10);
	os04c10_write_register(ViPipe, 0x484b, 0x07);
	os04c10_write_register(ViPipe, 0x488b, 0x00);
	os04c10_write_register(ViPipe, 0x4d00, 0x04);
	os04c10_write_register(ViPipe, 0x4d01, 0xad);
	os04c10_write_register(ViPipe, 0x4d02, 0xbc);
	os04c10_write_register(ViPipe, 0x4d03, 0xa1);
	os04c10_write_register(ViPipe, 0x4d04, 0x1f);
	os04c10_write_register(ViPipe, 0x4d05, 0x4c);
	os04c10_write_register(ViPipe, 0x4d0b, 0x01);
	os04c10_write_register(ViPipe, 0x4e00, 0x2a);
	os04c10_write_register(ViPipe, 0x4e0d, 0x00);
	os04c10_write_register(ViPipe, 0x5001, 0x09);
	os04c10_write_register(ViPipe, 0x5004, 0x00);
	os04c10_write_register(ViPipe, 0x5080, 0x04);
	os04c10_write_register(ViPipe, 0x5036, 0x00);
	os04c10_write_register(ViPipe, 0x5180, 0x70);
	os04c10_write_register(ViPipe, 0x5181, 0x10);
	os04c10_write_register(ViPipe, 0x520a, 0x03);
	os04c10_write_register(ViPipe, 0x520b, 0x06);
	os04c10_write_register(ViPipe, 0x520c, 0x0c);
	os04c10_write_register(ViPipe, 0x580b, 0x0f);
	os04c10_write_register(ViPipe, 0x580d, 0x00);
	os04c10_write_register(ViPipe, 0x580f, 0x00);
	os04c10_write_register(ViPipe, 0x5820, 0x00);
	os04c10_write_register(ViPipe, 0x5821, 0x00);
	os04c10_write_register(ViPipe, 0x301c, 0xf8);
	os04c10_write_register(ViPipe, 0x301e, 0xb4);
	os04c10_write_register(ViPipe, 0x301f, 0xd0);
	os04c10_write_register(ViPipe, 0x3022, 0x61);
	os04c10_write_register(ViPipe, 0x3109, 0xe7);
	os04c10_write_register(ViPipe, 0x3600, 0x00);
	os04c10_write_register(ViPipe, 0x3610, 0x95);
	os04c10_write_register(ViPipe, 0x3611, 0x85);
	os04c10_write_register(ViPipe, 0x3613, 0x3a);
	os04c10_write_register(ViPipe, 0x3615, 0x60);
	os04c10_write_register(ViPipe, 0x3621, 0xb0);
	os04c10_write_register(ViPipe, 0x3620, 0x0c);
	os04c10_write_register(ViPipe, 0x3629, 0x00);
	os04c10_write_register(ViPipe, 0x3661, 0x04);
	os04c10_write_register(ViPipe, 0x3662, 0x10);
	os04c10_write_register(ViPipe, 0x3664, 0x70);
	os04c10_write_register(ViPipe, 0x3665, 0x00);
	os04c10_write_register(ViPipe, 0x3681, 0xa6);
	os04c10_write_register(ViPipe, 0x3682, 0x53);
	os04c10_write_register(ViPipe, 0x3683, 0x2a);
	os04c10_write_register(ViPipe, 0x3684, 0x15);
	os04c10_write_register(ViPipe, 0x3700, 0x2a);
	os04c10_write_register(ViPipe, 0x3701, 0x12);
	os04c10_write_register(ViPipe, 0x3703, 0x28);
	os04c10_write_register(ViPipe, 0x3704, 0x0e);
	os04c10_write_register(ViPipe, 0x3706, 0x9d);
	os04c10_write_register(ViPipe, 0x3709, 0x4a);
	os04c10_write_register(ViPipe, 0x370b, 0x48);
	os04c10_write_register(ViPipe, 0x370c, 0x01);
	os04c10_write_register(ViPipe, 0x370f, 0x04);
	os04c10_write_register(ViPipe, 0x3714, 0x24);
	os04c10_write_register(ViPipe, 0x3716, 0x24);
	os04c10_write_register(ViPipe, 0x3719, 0x11);
	os04c10_write_register(ViPipe, 0x371a, 0x1e);
	os04c10_write_register(ViPipe, 0x3720, 0x00);
	os04c10_write_register(ViPipe, 0x3724, 0x13);
	os04c10_write_register(ViPipe, 0x373f, 0xb0);
	os04c10_write_register(ViPipe, 0x3741, 0x9d);
	os04c10_write_register(ViPipe, 0x3743, 0x9d);
	os04c10_write_register(ViPipe, 0x3745, 0x9d);
	os04c10_write_register(ViPipe, 0x3747, 0x9d);
	os04c10_write_register(ViPipe, 0x3749, 0x48);
	os04c10_write_register(ViPipe, 0x374b, 0x48);
	os04c10_write_register(ViPipe, 0x374d, 0x48);
	os04c10_write_register(ViPipe, 0x374f, 0x48);
	os04c10_write_register(ViPipe, 0x3755, 0x10);
	os04c10_write_register(ViPipe, 0x376c, 0x00);
	os04c10_write_register(ViPipe, 0x378d, 0x3c);
	os04c10_write_register(ViPipe, 0x3790, 0x01);
	os04c10_write_register(ViPipe, 0x3791, 0x01);
	os04c10_write_register(ViPipe, 0x3798, 0x40);
	os04c10_write_register(ViPipe, 0x379e, 0x00);
	os04c10_write_register(ViPipe, 0x379f, 0x04);
	os04c10_write_register(ViPipe, 0x37a1, 0x10);
	os04c10_write_register(ViPipe, 0x37a2, 0x1e);
	os04c10_write_register(ViPipe, 0x37a8, 0x10);
	os04c10_write_register(ViPipe, 0x37a9, 0x1e);
	os04c10_write_register(ViPipe, 0x37ac, 0xa0);
	os04c10_write_register(ViPipe, 0x37b9, 0x01);
	os04c10_write_register(ViPipe, 0x37bd, 0x01);
	os04c10_write_register(ViPipe, 0x37bf, 0x26);
	os04c10_write_register(ViPipe, 0x37c0, 0x11);
	os04c10_write_register(ViPipe, 0x37c2, 0x04);
	os04c10_write_register(ViPipe, 0x37cd, 0x19);
	os04c10_write_register(ViPipe, 0x37d8, 0x02);
	os04c10_write_register(ViPipe, 0x37d9, 0x08);
	os04c10_write_register(ViPipe, 0x37e5, 0x02);
	os04c10_write_register(ViPipe, 0x3800, 0x00);
	os04c10_write_register(ViPipe, 0x3801, 0x00);
	os04c10_write_register(ViPipe, 0x3802, 0x00);
	os04c10_write_register(ViPipe, 0x3803, 0x00);
	os04c10_write_register(ViPipe, 0x3804, 0x0a);
	os04c10_write_register(ViPipe, 0x3805, 0x8f);
	os04c10_write_register(ViPipe, 0x3806, 0x05);
	os04c10_write_register(ViPipe, 0x3807, 0xff);
	os04c10_write_register(ViPipe, 0x3808, 0x0a);
	os04c10_write_register(ViPipe, 0x3809, 0x80);
	os04c10_write_register(ViPipe, 0x380a, 0x05);
	os04c10_write_register(ViPipe, 0x380b, 0xf0);
	os04c10_write_register(ViPipe, 0x380c, 0x08);
	os04c10_write_register(ViPipe, 0x380d, 0x5c);
	os04c10_write_register(ViPipe, 0x380e, 0x06);
	os04c10_write_register(ViPipe, 0x380f, 0x26);
	os04c10_write_register(ViPipe, 0x3811, 0x08);
	os04c10_write_register(ViPipe, 0x3813, 0x08);
	os04c10_write_register(ViPipe, 0x3814, 0x01);
	os04c10_write_register(ViPipe, 0x3815, 0x01);
	os04c10_write_register(ViPipe, 0x3816, 0x01);
	os04c10_write_register(ViPipe, 0x3817, 0x01);
	os04c10_write_register(ViPipe, 0x3820, 0x88);
	os04c10_write_register(ViPipe, 0x3821, 0x00);
	os04c10_write_register(ViPipe, 0x3880, 0x25);
	os04c10_write_register(ViPipe, 0x3882, 0x20);
	os04c10_write_register(ViPipe, 0x3c91, 0x0b);
	os04c10_write_register(ViPipe, 0x3c94, 0x45);
	os04c10_write_register(ViPipe, 0x4000, 0xf3);
	os04c10_write_register(ViPipe, 0x4001, 0x60);
	os04c10_write_register(ViPipe, 0x4003, 0x80);
	os04c10_write_register(ViPipe, 0x4008, 0x02);
	os04c10_write_register(ViPipe, 0x4009, 0x0d);
	os04c10_write_register(ViPipe, 0x4300, 0xff);
	os04c10_write_register(ViPipe, 0x4302, 0x0f);
	os04c10_write_register(ViPipe, 0x4305, 0x83);
	os04c10_write_register(ViPipe, 0x4505, 0x84);
	os04c10_write_register(ViPipe, 0x4809, 0x1e);
	os04c10_write_register(ViPipe, 0x480a, 0x04);
	os04c10_write_register(ViPipe, 0x4837, 0x23);
	os04c10_write_register(ViPipe, 0x4c00, 0x08);
	os04c10_write_register(ViPipe, 0x4c01, 0x08);
	os04c10_write_register(ViPipe, 0x4c04, 0x00);
	os04c10_write_register(ViPipe, 0x4c05, 0x00);
	os04c10_write_register(ViPipe, 0x5000, 0xf9);
	os04c10_write_register(ViPipe, 0x3624, 0x00);
	os04c10_write_register(ViPipe, 0x3016, 0x32);
	os04c10_write_register(ViPipe, 0x0306, 0x00);
	os04c10_write_register(ViPipe, 0x4837, 0x12);
	os04c10_write_register(ViPipe, 0x0305, 0x6a);
	os04c10_write_register(ViPipe, 0x0325, 0x54);
	os04c10_write_register(ViPipe, 0x3106, 0x25);
	os04c10_default_reg_init(ViPipe);

	if (!g_au16Os04c10_UseHwSync[ViPipe]) {
		/* freerun */
		os04c10_write_register(ViPipe, 0x3002, 0x21);
	} else {
		/* auto master */
#if HW_SYNC_AUTO
		os04c10_write_register(ViPipe, 0x3002, 0x22); // [1] vsync_oen, [0]: fsin_oen
		os04c10_write_register(ViPipe, 0x3690, 0x00); // [4]: 1'b0, 1st set vsync pulse
		os04c10_write_register(ViPipe, 0x383e, 0x00); // vscyn_rise_rcnt_pt[23:16]
		os04c10_write_register(ViPipe, 0x3818, 0x00); // Slave vsync pulse position cs [15:8]
		os04c10_write_register(ViPipe, 0x3819, 0x00); // Slave vsync pulse position cs [7:0],
							      // max is HTS/4
		os04c10_write_register(ViPipe, 0x381a, 0x00); // vscyn_rise_rcnt_pt[15:8]
		os04c10_write_register(ViPipe, 0x381b, 0x00); // vscyn_rise_rcnt_pt[7:0], max:
							      // VTS-12 for AHBIN 720p, (VTS -12)*2 for
							      // other formats
		os04c10_write_register(ViPipe, 0x3832, 0xf8); // default, 8'h08, [7:4] vsync pulse width
		os04c10_write_register(ViPipe, 0x368a, 0x04); // GPIO enable
#else
		/* manual master */
		os04c10_write_register(ViPipe, 0x3002, 0x22); // [1] vsync_oen, [0]: fsin_oen
		os04c10_write_register(ViPipe, 0x3690, 0x00); // [4]: 1'b0, 1st set vsync pulse
		os04c10_write_register(ViPipe, 0x383e, 0x00); // vscyn_rise_rcnt_pt[23:16]
		os04c10_write_register(ViPipe, 0x3818, 0x00); // Slave vsync pulse position cs [15:8]
		os04c10_write_register(ViPipe, 0x3819, 0x00); // Slave vsync pulse position cs [7:0],
							      // max is HTS/4
		os04c10_write_register(ViPipe, 0x381a, 0x00); // vscyn_rise_rcnt_pt[15:8]
		os04c10_write_register(ViPipe, 0x381b, 0x18); // vscyn_rise_rcnt_pt[7:0], max:
							      // VTS-12 for AHBIN 720p, (VTS -12)*2 for
							      // other formats
		os04c10_write_register(ViPipe, 0x3832, 0xf8); // default, 8'h08, [7:4] vsync pulse width
		os04c10_write_register(ViPipe, 0x368a, 0x04); // GPIO enable
#endif
	}
	os04c10_write_register(ViPipe, 0x0100, 0x01);

	usleep(50*1000);

	printf("ViPipe:%d,===OS04C10 1520P 30fps 12bit LINE Init OK!===\n", ViPipe);
}

static void os04c10_linear_1440p30_init(VI_PIPE ViPipe)
{
	os04c10_write_register(ViPipe, 0x0100, 0x00);
	os04c10_write_register(ViPipe, 0x0103, 0x01);
	os04c10_write_register(ViPipe, 0x0301, 0xe4);
	os04c10_write_register(ViPipe, 0x0303, 0x01);
	os04c10_write_register(ViPipe, 0x0305, 0x6e);
	os04c10_write_register(ViPipe, 0x0306, 0x01);
	os04c10_write_register(ViPipe, 0x0307, 0x17);
	os04c10_write_register(ViPipe, 0x0323, 0x04);
	os04c10_write_register(ViPipe, 0x0324, 0x01);
	os04c10_write_register(ViPipe, 0x0325, 0x62);
	os04c10_write_register(ViPipe, 0x3012, 0x06);
	os04c10_write_register(ViPipe, 0x3013, 0x02);
	os04c10_write_register(ViPipe, 0x3016, 0x72);
	os04c10_write_register(ViPipe, 0x3021, 0x03);
	os04c10_write_register(ViPipe, 0x3106, 0x21);
	os04c10_write_register(ViPipe, 0x3107, 0xa1);
	os04c10_write_register(ViPipe, 0x3500, 0x00);
	os04c10_write_register(ViPipe, 0x3501, 0x06);
	os04c10_write_register(ViPipe, 0x3502, 0x1e);
	os04c10_write_register(ViPipe, 0x3503, 0x88);
	os04c10_write_register(ViPipe, 0x3508, 0x00);
	os04c10_write_register(ViPipe, 0x3509, 0x80);
	os04c10_write_register(ViPipe, 0x350a, 0x04);
	os04c10_write_register(ViPipe, 0x350b, 0x00);
	os04c10_write_register(ViPipe, 0x350c, 0x00);
	os04c10_write_register(ViPipe, 0x350d, 0x80);
	os04c10_write_register(ViPipe, 0x350e, 0x04);
	os04c10_write_register(ViPipe, 0x350f, 0x00);
	os04c10_write_register(ViPipe, 0x3510, 0x00);
	os04c10_write_register(ViPipe, 0x3511, 0x00);
	os04c10_write_register(ViPipe, 0x3512, 0x20);
	os04c10_write_register(ViPipe, 0x3624, 0x02);
	os04c10_write_register(ViPipe, 0x3625, 0x4c);
	os04c10_write_register(ViPipe, 0x3660, 0x00);
	os04c10_write_register(ViPipe, 0x3666, 0xa5);
	os04c10_write_register(ViPipe, 0x3667, 0xa5);
	os04c10_write_register(ViPipe, 0x366a, 0x60);
	os04c10_write_register(ViPipe, 0x3673, 0x0d);
	os04c10_write_register(ViPipe, 0x3672, 0x0d);
	os04c10_write_register(ViPipe, 0x3671, 0x0d);
	os04c10_write_register(ViPipe, 0x3670, 0x0d);
	os04c10_write_register(ViPipe, 0x3685, 0x00);
	os04c10_write_register(ViPipe, 0x3694, 0x0d);
	os04c10_write_register(ViPipe, 0x3693, 0x0d);
	os04c10_write_register(ViPipe, 0x3692, 0x0d);
	os04c10_write_register(ViPipe, 0x3691, 0x0d);
	os04c10_write_register(ViPipe, 0x3696, 0x4c);
	os04c10_write_register(ViPipe, 0x3697, 0x4c);
	os04c10_write_register(ViPipe, 0x3698, 0x40);
	os04c10_write_register(ViPipe, 0x3699, 0x80);
	os04c10_write_register(ViPipe, 0x369a, 0x18);
	os04c10_write_register(ViPipe, 0x369b, 0x1f);
	os04c10_write_register(ViPipe, 0x369c, 0x14);
	os04c10_write_register(ViPipe, 0x369d, 0x80);
	os04c10_write_register(ViPipe, 0x369e, 0x40);
	os04c10_write_register(ViPipe, 0x369f, 0x21);
	os04c10_write_register(ViPipe, 0x36a0, 0x12);
	os04c10_write_register(ViPipe, 0x36a1, 0x5d);
	os04c10_write_register(ViPipe, 0x36a2, 0x66);
	os04c10_write_register(ViPipe, 0x370a, 0x02);
	os04c10_write_register(ViPipe, 0x370e, 0x0c);
	os04c10_write_register(ViPipe, 0x3710, 0x00);
	os04c10_write_register(ViPipe, 0x3713, 0x00);
	os04c10_write_register(ViPipe, 0x3725, 0x02);
	os04c10_write_register(ViPipe, 0x372a, 0x03);
	os04c10_write_register(ViPipe, 0x3738, 0xce);
	os04c10_write_register(ViPipe, 0x3739, 0x10);
	os04c10_write_register(ViPipe, 0x3748, 0x02);
	os04c10_write_register(ViPipe, 0x374a, 0x02);
	os04c10_write_register(ViPipe, 0x374c, 0x02);
	os04c10_write_register(ViPipe, 0x374e, 0x02);
	os04c10_write_register(ViPipe, 0x3756, 0x00);
	os04c10_write_register(ViPipe, 0x3757, 0x0e);
	os04c10_write_register(ViPipe, 0x3767, 0x00);
	os04c10_write_register(ViPipe, 0x3771, 0x00);
	os04c10_write_register(ViPipe, 0x377b, 0x20);
	os04c10_write_register(ViPipe, 0x377c, 0x00);
	os04c10_write_register(ViPipe, 0x377d, 0x0c);
	os04c10_write_register(ViPipe, 0x3781, 0x03);
	os04c10_write_register(ViPipe, 0x3782, 0x00);
	os04c10_write_register(ViPipe, 0x3789, 0x14);
	os04c10_write_register(ViPipe, 0x3795, 0x02);
	os04c10_write_register(ViPipe, 0x379c, 0x00);
	os04c10_write_register(ViPipe, 0x379d, 0x00);
	os04c10_write_register(ViPipe, 0x37b8, 0x04);
	os04c10_write_register(ViPipe, 0x37ba, 0x03);
	os04c10_write_register(ViPipe, 0x37bb, 0x00);
	os04c10_write_register(ViPipe, 0x37bc, 0x04);
	os04c10_write_register(ViPipe, 0x37be, 0x08);
	os04c10_write_register(ViPipe, 0x37c4, 0x11);
	os04c10_write_register(ViPipe, 0x37c5, 0x80);
	os04c10_write_register(ViPipe, 0x37c6, 0x14);
	os04c10_write_register(ViPipe, 0x37c7, 0x08);
	os04c10_write_register(ViPipe, 0x37da, 0x11);
	os04c10_write_register(ViPipe, 0x381f, 0x08);
	os04c10_write_register(ViPipe, 0x3829, 0x03);
	os04c10_write_register(ViPipe, 0x3832, 0x00);
	os04c10_write_register(ViPipe, 0x3881, 0x00);
	os04c10_write_register(ViPipe, 0x3888, 0x04);
	os04c10_write_register(ViPipe, 0x388b, 0x00);
	os04c10_write_register(ViPipe, 0x3c80, 0x10);
	os04c10_write_register(ViPipe, 0x3c86, 0x00);
	os04c10_write_register(ViPipe, 0x3c9f, 0x01);
	os04c10_write_register(ViPipe, 0x3d85, 0x1b);
	os04c10_write_register(ViPipe, 0x3d8c, 0x71);
	os04c10_write_register(ViPipe, 0x3d8d, 0xe2);
	os04c10_write_register(ViPipe, 0x3f00, 0x0b);
	os04c10_write_register(ViPipe, 0x3f06, 0x04);
	os04c10_write_register(ViPipe, 0x400a, 0x01);
	os04c10_write_register(ViPipe, 0x400b, 0x50);
	os04c10_write_register(ViPipe, 0x400e, 0x08);
	os04c10_write_register(ViPipe, 0x4040, 0x00);
	os04c10_write_register(ViPipe, 0x4041, 0x07);
	os04c10_write_register(ViPipe, 0x4043, 0x7e);
	os04c10_write_register(ViPipe, 0x4045, 0x7e);
	os04c10_write_register(ViPipe, 0x4047, 0x7e);
	os04c10_write_register(ViPipe, 0x4049, 0x7e);
	os04c10_write_register(ViPipe, 0x4090, 0x04);
	os04c10_write_register(ViPipe, 0x40b0, 0x00);
	os04c10_write_register(ViPipe, 0x40b1, 0x00);
	os04c10_write_register(ViPipe, 0x40b2, 0x00);
	os04c10_write_register(ViPipe, 0x40b3, 0x00);
	os04c10_write_register(ViPipe, 0x40b4, 0x00);
	os04c10_write_register(ViPipe, 0x40b5, 0x00);
	os04c10_write_register(ViPipe, 0x40b7, 0x00);
	os04c10_write_register(ViPipe, 0x40b8, 0x00);
	os04c10_write_register(ViPipe, 0x40b9, 0x00);
	os04c10_write_register(ViPipe, 0x40ba, 0x00);
	os04c10_write_register(ViPipe, 0x4301, 0x00);
	os04c10_write_register(ViPipe, 0x4303, 0x00);
	os04c10_write_register(ViPipe, 0x4502, 0x04);
	os04c10_write_register(ViPipe, 0x4503, 0x00);
	os04c10_write_register(ViPipe, 0x4504, 0x06);
	os04c10_write_register(ViPipe, 0x4506, 0x00);
	os04c10_write_register(ViPipe, 0x4507, 0x64);
	os04c10_write_register(ViPipe, 0x4803, 0x00);
	os04c10_write_register(ViPipe, 0x480c, 0x32);
	os04c10_write_register(ViPipe, 0x480e, 0x00);
	os04c10_write_register(ViPipe, 0x4813, 0x00);
	os04c10_write_register(ViPipe, 0x4819, 0x70);
	os04c10_write_register(ViPipe, 0x481f, 0x30);
	os04c10_write_register(ViPipe, 0x4823, 0x3f);
	os04c10_write_register(ViPipe, 0x4825, 0x30);
	os04c10_write_register(ViPipe, 0x4833, 0x10);
	os04c10_write_register(ViPipe, 0x484b, 0x07);
	os04c10_write_register(ViPipe, 0x488b, 0x00);
	os04c10_write_register(ViPipe, 0x4d00, 0x04);
	os04c10_write_register(ViPipe, 0x4d01, 0xad);
	os04c10_write_register(ViPipe, 0x4d02, 0xbc);
	os04c10_write_register(ViPipe, 0x4d03, 0xa1);
	os04c10_write_register(ViPipe, 0x4d04, 0x1f);
	os04c10_write_register(ViPipe, 0x4d05, 0x4c);
	os04c10_write_register(ViPipe, 0x4d0b, 0x01);
	os04c10_write_register(ViPipe, 0x4e00, 0x2a);
	os04c10_write_register(ViPipe, 0x4e0d, 0x00);
	os04c10_write_register(ViPipe, 0x5001, 0x09);
	os04c10_write_register(ViPipe, 0x5004, 0x00);
	os04c10_write_register(ViPipe, 0x5080, 0x04);
	os04c10_write_register(ViPipe, 0x5036, 0x00);
	os04c10_write_register(ViPipe, 0x5180, 0x70);
	os04c10_write_register(ViPipe, 0x5181, 0x10);
	os04c10_write_register(ViPipe, 0x520a, 0x03);
	os04c10_write_register(ViPipe, 0x520b, 0x06);
	os04c10_write_register(ViPipe, 0x520c, 0x0c);
	os04c10_write_register(ViPipe, 0x580b, 0x0f);
	os04c10_write_register(ViPipe, 0x580d, 0x00);
	os04c10_write_register(ViPipe, 0x580f, 0x00);
	os04c10_write_register(ViPipe, 0x5820, 0x00);
	os04c10_write_register(ViPipe, 0x5821, 0x00);
	os04c10_write_register(ViPipe, 0x301c, 0xf8);
	os04c10_write_register(ViPipe, 0x301e, 0xb4);
	os04c10_write_register(ViPipe, 0x301f, 0xd0);
	os04c10_write_register(ViPipe, 0x3022, 0x61);
	os04c10_write_register(ViPipe, 0x3109, 0xe7);
	os04c10_write_register(ViPipe, 0x3600, 0x00);
	os04c10_write_register(ViPipe, 0x3610, 0x95);
	os04c10_write_register(ViPipe, 0x3611, 0x85);
	os04c10_write_register(ViPipe, 0x3613, 0x3a);
	os04c10_write_register(ViPipe, 0x3615, 0x60);
	os04c10_write_register(ViPipe, 0x3621, 0xb0);
	os04c10_write_register(ViPipe, 0x3620, 0x0c);
	os04c10_write_register(ViPipe, 0x3629, 0x00);
	os04c10_write_register(ViPipe, 0x3661, 0x04);
	os04c10_write_register(ViPipe, 0x3662, 0x10);
	os04c10_write_register(ViPipe, 0x3664, 0x70);
	os04c10_write_register(ViPipe, 0x3665, 0x00);
	os04c10_write_register(ViPipe, 0x3681, 0xa6);
	os04c10_write_register(ViPipe, 0x3682, 0x53);
	os04c10_write_register(ViPipe, 0x3683, 0x2a);
	os04c10_write_register(ViPipe, 0x3684, 0x15);
	os04c10_write_register(ViPipe, 0x3700, 0x2a);
	os04c10_write_register(ViPipe, 0x3701, 0x12);
	os04c10_write_register(ViPipe, 0x3703, 0x28);
	os04c10_write_register(ViPipe, 0x3704, 0x0e);
	os04c10_write_register(ViPipe, 0x3706, 0x9d);
	os04c10_write_register(ViPipe, 0x3709, 0x4a);
	os04c10_write_register(ViPipe, 0x370b, 0x48);
	os04c10_write_register(ViPipe, 0x370c, 0x01);
	os04c10_write_register(ViPipe, 0x370f, 0x04);
	os04c10_write_register(ViPipe, 0x3714, 0x24);
	os04c10_write_register(ViPipe, 0x3716, 0x24);
	os04c10_write_register(ViPipe, 0x3719, 0x11);
	os04c10_write_register(ViPipe, 0x371a, 0x1e);
	os04c10_write_register(ViPipe, 0x3720, 0x00);
	os04c10_write_register(ViPipe, 0x3724, 0x13);
	os04c10_write_register(ViPipe, 0x373f, 0xb0);
	os04c10_write_register(ViPipe, 0x3741, 0x9d);
	os04c10_write_register(ViPipe, 0x3743, 0x9d);
	os04c10_write_register(ViPipe, 0x3745, 0x9d);
	os04c10_write_register(ViPipe, 0x3747, 0x9d);
	os04c10_write_register(ViPipe, 0x3749, 0x48);
	os04c10_write_register(ViPipe, 0x374b, 0x48);
	os04c10_write_register(ViPipe, 0x374d, 0x48);
	os04c10_write_register(ViPipe, 0x374f, 0x48);
	os04c10_write_register(ViPipe, 0x3755, 0x10);
	os04c10_write_register(ViPipe, 0x376c, 0x00);
	os04c10_write_register(ViPipe, 0x378d, 0x3c);
	os04c10_write_register(ViPipe, 0x3790, 0x01);
	os04c10_write_register(ViPipe, 0x3791, 0x01);
	os04c10_write_register(ViPipe, 0x3798, 0x40);
	os04c10_write_register(ViPipe, 0x379e, 0x00);
	os04c10_write_register(ViPipe, 0x379f, 0x04);
	os04c10_write_register(ViPipe, 0x37a1, 0x10);
	os04c10_write_register(ViPipe, 0x37a2, 0x1e);
	os04c10_write_register(ViPipe, 0x37a8, 0x10);
	os04c10_write_register(ViPipe, 0x37a9, 0x1e);
	os04c10_write_register(ViPipe, 0x37ac, 0xa0);
	os04c10_write_register(ViPipe, 0x37b9, 0x01);
	os04c10_write_register(ViPipe, 0x37bd, 0x01);
	os04c10_write_register(ViPipe, 0x37bf, 0x26);
	os04c10_write_register(ViPipe, 0x37c0, 0x11);
	os04c10_write_register(ViPipe, 0x37c2, 0x04);
	os04c10_write_register(ViPipe, 0x37cd, 0x19);
	os04c10_write_register(ViPipe, 0x37d8, 0x02);
	os04c10_write_register(ViPipe, 0x37d9, 0x08);
	os04c10_write_register(ViPipe, 0x37e5, 0x02);
	os04c10_write_register(ViPipe, 0x3800, 0x00);
	os04c10_write_register(ViPipe, 0x3801, 0x40);
	os04c10_write_register(ViPipe, 0x3802, 0x00);
	os04c10_write_register(ViPipe, 0x3803, 0x28);
	os04c10_write_register(ViPipe, 0x3804, 0x0a);
	os04c10_write_register(ViPipe, 0x3805, 0x4f);
	os04c10_write_register(ViPipe, 0x3806, 0x05);
	os04c10_write_register(ViPipe, 0x3807, 0xd7);
	os04c10_write_register(ViPipe, 0x3808, 0x0a);
	os04c10_write_register(ViPipe, 0x3809, 0x00);
	os04c10_write_register(ViPipe, 0x380a, 0x05);
	os04c10_write_register(ViPipe, 0x380b, 0xa0);
	os04c10_write_register(ViPipe, 0x380c, 0x08);
	os04c10_write_register(ViPipe, 0x380d, 0x5c);
	os04c10_write_register(ViPipe, 0x380e, 0x06);
	os04c10_write_register(ViPipe, 0x380f, 0x26);
	os04c10_write_register(ViPipe, 0x3811, 0x08);
	os04c10_write_register(ViPipe, 0x3813, 0x08);
	os04c10_write_register(ViPipe, 0x3814, 0x01);
	os04c10_write_register(ViPipe, 0x3815, 0x01);
	os04c10_write_register(ViPipe, 0x3816, 0x01);
	os04c10_write_register(ViPipe, 0x3817, 0x01);
	os04c10_write_register(ViPipe, 0x3820, 0x88);
	os04c10_write_register(ViPipe, 0x3821, 0x00);
	os04c10_write_register(ViPipe, 0x3880, 0x25);
	os04c10_write_register(ViPipe, 0x3882, 0x20);
	os04c10_write_register(ViPipe, 0x3c91, 0x0b);
	os04c10_write_register(ViPipe, 0x3c94, 0x45);
	os04c10_write_register(ViPipe, 0x4000, 0xf3);
	os04c10_write_register(ViPipe, 0x4001, 0x60);
	os04c10_write_register(ViPipe, 0x4003, 0x80);
	os04c10_write_register(ViPipe, 0x4008, 0x02);
	os04c10_write_register(ViPipe, 0x4009, 0x0d);
	os04c10_write_register(ViPipe, 0x4300, 0xff);
	os04c10_write_register(ViPipe, 0x4302, 0x0f);
	os04c10_write_register(ViPipe, 0x4305, 0x83);
	os04c10_write_register(ViPipe, 0x4505, 0x84);
	os04c10_write_register(ViPipe, 0x4809, 0x1e);
	os04c10_write_register(ViPipe, 0x480a, 0x04);
	os04c10_write_register(ViPipe, 0x4837, 0x23);
	os04c10_write_register(ViPipe, 0x4c00, 0x08);
	os04c10_write_register(ViPipe, 0x4c01, 0x08);
	os04c10_write_register(ViPipe, 0x4c04, 0x00);
	os04c10_write_register(ViPipe, 0x4c05, 0x00);
	os04c10_write_register(ViPipe, 0x5000, 0xe9);
	os04c10_write_register(ViPipe, 0x3624, 0x00);
	os04c10_write_register(ViPipe, 0x3016, 0x32);
	os04c10_write_register(ViPipe, 0x0306, 0x00);
	os04c10_write_register(ViPipe, 0x4837, 0x12);
	os04c10_write_register(ViPipe, 0x0305, 0x6a);
	os04c10_write_register(ViPipe, 0x0325, 0x54);
	os04c10_write_register(ViPipe, 0x3106, 0x25);
	os04c10_default_reg_init(ViPipe);

	if (!g_au16Os04c10_UseHwSync[ViPipe]) {
		/* freerun */
		os04c10_write_register(ViPipe, 0x3002, 0x21);
	} else {
		/* auto master */
		os04c10_write_register(ViPipe, 0x3002, 0x23); // [1] vsync_oen, [0]: fsin_oen
		os04c10_write_register(ViPipe, 0x3690, 0x00); // [4]: 1'b0, 1st set vsync pulse
		os04c10_write_register(ViPipe, 0x383e, 0x00); // vscyn_rise_rcnt_pt[23:16]
		os04c10_write_register(ViPipe, 0x3818, 0x00); // Slave vsync pulse position cs [15:8]
		os04c10_write_register(ViPipe, 0x3819, 0x00); // Slave vsync pulse position cs [7:0],
							      // max is HTS/4
		os04c10_write_register(ViPipe, 0x381a, 0x00); // vscyn_rise_rcnt_pt[15:8]
		os04c10_write_register(ViPipe, 0x381b, 0x18); // vscyn_rise_rcnt_pt[7:0], max:
							      // VTS-12 for AHBIN 720p, (VTS -12)*2 for
							      // other formats
		os04c10_write_register(ViPipe, 0x3832, 0xf8); // default, 8'h08, [7:4] vsync pulse width
		os04c10_write_register(ViPipe, 0x368a, 0x04); // GPIO enable
	}
	os04c10_write_register(ViPipe, 0x0100, 0x01);

	usleep(50*1000);

	printf("ViPipe:%d,===OS04C10 1440P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

static void os04c10_wdr_1520p30_2to1_init(VI_PIPE ViPipe)
{
	os04c10_write_register(ViPipe, 0x0100, 0x00);
	os04c10_write_register(ViPipe, 0x0103, 0x01);
	os04c10_write_register(ViPipe, 0x0301, 0x84);
	os04c10_write_register(ViPipe, 0x0303, 0x01);
	os04c10_write_register(ViPipe, 0x0305, 0x61);
	os04c10_write_register(ViPipe, 0x0306, 0x01);
	os04c10_write_register(ViPipe, 0x0307, 0x17);
	os04c10_write_register(ViPipe, 0x0323, 0x04);
	os04c10_write_register(ViPipe, 0x0324, 0x01);
	os04c10_write_register(ViPipe, 0x0325, 0x7a);
	os04c10_write_register(ViPipe, 0x3012, 0x06);
	os04c10_write_register(ViPipe, 0x3013, 0x02);
	os04c10_write_register(ViPipe, 0x3016, 0x72);
	os04c10_write_register(ViPipe, 0x3021, 0x03);
	os04c10_write_register(ViPipe, 0x3106, 0x21);
	os04c10_write_register(ViPipe, 0x3107, 0xa1);
	os04c10_write_register(ViPipe, 0x3500, 0x00);
	os04c10_write_register(ViPipe, 0x3501, 0x03);
	os04c10_write_register(ViPipe, 0x3502, 0x08);
	os04c10_write_register(ViPipe, 0x3503, 0x88);
	os04c10_write_register(ViPipe, 0x3508, 0x00);
	os04c10_write_register(ViPipe, 0x3509, 0x80);
	os04c10_write_register(ViPipe, 0x350a, 0x04);
	os04c10_write_register(ViPipe, 0x350b, 0x00);
	os04c10_write_register(ViPipe, 0x350c, 0x00);
	os04c10_write_register(ViPipe, 0x350d, 0x80);
	os04c10_write_register(ViPipe, 0x350e, 0x04);
	os04c10_write_register(ViPipe, 0x350f, 0x00);
	os04c10_write_register(ViPipe, 0x3510, 0x00);
	os04c10_write_register(ViPipe, 0x3511, 0x01);
	os04c10_write_register(ViPipe, 0x3512, 0x08);
	os04c10_write_register(ViPipe, 0x3624, 0x02);
	os04c10_write_register(ViPipe, 0x3625, 0x4c);
	os04c10_write_register(ViPipe, 0x3660, 0x04);
	os04c10_write_register(ViPipe, 0x3666, 0xa5);
	os04c10_write_register(ViPipe, 0x3667, 0xa5);
	os04c10_write_register(ViPipe, 0x366a, 0x54);
	os04c10_write_register(ViPipe, 0x3673, 0x0d);
	os04c10_write_register(ViPipe, 0x3672, 0x0d);
	os04c10_write_register(ViPipe, 0x3671, 0x0d);
	os04c10_write_register(ViPipe, 0x3670, 0x0d);
	os04c10_write_register(ViPipe, 0x3685, 0x00);
	os04c10_write_register(ViPipe, 0x3694, 0x0d);
	os04c10_write_register(ViPipe, 0x3693, 0x0d);
	os04c10_write_register(ViPipe, 0x3692, 0x0d);
	os04c10_write_register(ViPipe, 0x3691, 0x0d);
	os04c10_write_register(ViPipe, 0x3696, 0x4c);
	os04c10_write_register(ViPipe, 0x3697, 0x4c);
	os04c10_write_register(ViPipe, 0x3698, 0x40);
	os04c10_write_register(ViPipe, 0x3699, 0x80);
	os04c10_write_register(ViPipe, 0x369a, 0x18);
	os04c10_write_register(ViPipe, 0x369b, 0x1f);
	os04c10_write_register(ViPipe, 0x369c, 0x14);
	os04c10_write_register(ViPipe, 0x369d, 0x80);
	os04c10_write_register(ViPipe, 0x369e, 0x40);
	os04c10_write_register(ViPipe, 0x369f, 0x21);
	os04c10_write_register(ViPipe, 0x36a0, 0x12);
	os04c10_write_register(ViPipe, 0x36a1, 0x5d);
	os04c10_write_register(ViPipe, 0x36a2, 0x66);
	os04c10_write_register(ViPipe, 0x370a, 0x00);
	os04c10_write_register(ViPipe, 0x370e, 0x0c);
	os04c10_write_register(ViPipe, 0x3710, 0x00);
	os04c10_write_register(ViPipe, 0x3713, 0x00);
	os04c10_write_register(ViPipe, 0x3725, 0x02);
	os04c10_write_register(ViPipe, 0x372a, 0x03);
	os04c10_write_register(ViPipe, 0x3738, 0xce);
	os04c10_write_register(ViPipe, 0x3748, 0x00);
	os04c10_write_register(ViPipe, 0x374a, 0x00);
	os04c10_write_register(ViPipe, 0x374c, 0x00);
	os04c10_write_register(ViPipe, 0x374e, 0x00);
	os04c10_write_register(ViPipe, 0x3756, 0x00);
	os04c10_write_register(ViPipe, 0x3757, 0x00);
	os04c10_write_register(ViPipe, 0x3767, 0x00);
	os04c10_write_register(ViPipe, 0x3771, 0x00);
	os04c10_write_register(ViPipe, 0x377b, 0x28);
	os04c10_write_register(ViPipe, 0x377c, 0x00);
	os04c10_write_register(ViPipe, 0x377d, 0x0c);
	os04c10_write_register(ViPipe, 0x3781, 0x03);
	os04c10_write_register(ViPipe, 0x3782, 0x00);
	os04c10_write_register(ViPipe, 0x3789, 0x14);
	os04c10_write_register(ViPipe, 0x3795, 0x02);
	os04c10_write_register(ViPipe, 0x379c, 0x00);
	os04c10_write_register(ViPipe, 0x379d, 0x00);
	os04c10_write_register(ViPipe, 0x37b8, 0x04);
	os04c10_write_register(ViPipe, 0x37ba, 0x03);
	os04c10_write_register(ViPipe, 0x37bb, 0x00);
	os04c10_write_register(ViPipe, 0x37bc, 0x04);
	os04c10_write_register(ViPipe, 0x37be, 0x08);
	os04c10_write_register(ViPipe, 0x37c4, 0x11);
	os04c10_write_register(ViPipe, 0x37c5, 0x80);
	os04c10_write_register(ViPipe, 0x37c6, 0x14);
	os04c10_write_register(ViPipe, 0x37c7, 0x08);
	os04c10_write_register(ViPipe, 0x37da, 0x11);
	os04c10_write_register(ViPipe, 0x381f, 0x08);
	os04c10_write_register(ViPipe, 0x3829, 0x03);
	os04c10_write_register(ViPipe, 0x3881, 0x00);
	os04c10_write_register(ViPipe, 0x3888, 0x04);
	os04c10_write_register(ViPipe, 0x388b, 0x00);
	os04c10_write_register(ViPipe, 0x3c80, 0x10);
	os04c10_write_register(ViPipe, 0x3c86, 0x00);
	os04c10_write_register(ViPipe, 0x3c8c, 0x20);
	os04c10_write_register(ViPipe, 0x3c9f, 0x01);
	os04c10_write_register(ViPipe, 0x3d85, 0x1b);
	os04c10_write_register(ViPipe, 0x3d8c, 0x71);
	os04c10_write_register(ViPipe, 0x3d8d, 0xe2);
	os04c10_write_register(ViPipe, 0x3f00, 0x0b);
	os04c10_write_register(ViPipe, 0x3f06, 0x04);
	os04c10_write_register(ViPipe, 0x400a, 0x01);
	os04c10_write_register(ViPipe, 0x400b, 0x50);
	os04c10_write_register(ViPipe, 0x400e, 0x08);
	os04c10_write_register(ViPipe, 0x4043, 0x7e);
	os04c10_write_register(ViPipe, 0x4045, 0x7e);
	os04c10_write_register(ViPipe, 0x4047, 0x7e);
	os04c10_write_register(ViPipe, 0x4049, 0x7e);
	os04c10_write_register(ViPipe, 0x4090, 0x14);
	os04c10_write_register(ViPipe, 0x40b0, 0x00);
	os04c10_write_register(ViPipe, 0x40b1, 0x00);
	os04c10_write_register(ViPipe, 0x40b2, 0x00);
	os04c10_write_register(ViPipe, 0x40b3, 0x00);
	os04c10_write_register(ViPipe, 0x40b4, 0x00);
	os04c10_write_register(ViPipe, 0x40b5, 0x00);
	os04c10_write_register(ViPipe, 0x40b7, 0x00);
	os04c10_write_register(ViPipe, 0x40b8, 0x00);
	os04c10_write_register(ViPipe, 0x40b9, 0x00);
	os04c10_write_register(ViPipe, 0x40ba, 0x01);
	os04c10_write_register(ViPipe, 0x4301, 0x00);
	os04c10_write_register(ViPipe, 0x4303, 0x00);
	os04c10_write_register(ViPipe, 0x4502, 0x04);
	os04c10_write_register(ViPipe, 0x4503, 0x00);
	os04c10_write_register(ViPipe, 0x4504, 0x06);
	os04c10_write_register(ViPipe, 0x4506, 0x00);
	os04c10_write_register(ViPipe, 0x4507, 0x47);
	os04c10_write_register(ViPipe, 0x4803, 0x00);
	os04c10_write_register(ViPipe, 0x480c, 0x32);
	os04c10_write_register(ViPipe, 0x480e, 0x04);
	os04c10_write_register(ViPipe, 0x4813, 0xe4);
	os04c10_write_register(ViPipe, 0x4819, 0x70);
	os04c10_write_register(ViPipe, 0x481f, 0x30);
	os04c10_write_register(ViPipe, 0x4823, 0x3f);
	os04c10_write_register(ViPipe, 0x4825, 0x30);
	os04c10_write_register(ViPipe, 0x4833, 0x10);
	os04c10_write_register(ViPipe, 0x484b, 0x27);
	os04c10_write_register(ViPipe, 0x488b, 0x00);
	os04c10_write_register(ViPipe, 0x4d00, 0x04);
	os04c10_write_register(ViPipe, 0x4d01, 0xad);
	os04c10_write_register(ViPipe, 0x4d02, 0xbc);
	os04c10_write_register(ViPipe, 0x4d03, 0xa1);
	os04c10_write_register(ViPipe, 0x4d04, 0x1f);
	os04c10_write_register(ViPipe, 0x4d05, 0x4c);
	os04c10_write_register(ViPipe, 0x4d0b, 0x01);
	os04c10_write_register(ViPipe, 0x4e00, 0x2a);
	os04c10_write_register(ViPipe, 0x4e0d, 0x00);
	os04c10_write_register(ViPipe, 0x5001, 0x09);
	os04c10_write_register(ViPipe, 0x5004, 0x00);
	os04c10_write_register(ViPipe, 0x5080, 0x04);
	os04c10_write_register(ViPipe, 0x5036, 0x80);
	os04c10_write_register(ViPipe, 0x5180, 0x70);
	os04c10_write_register(ViPipe, 0x5181, 0x10);
	os04c10_write_register(ViPipe, 0x520a, 0x03);
	os04c10_write_register(ViPipe, 0x520b, 0x06);
	os04c10_write_register(ViPipe, 0x520c, 0x0c);
	os04c10_write_register(ViPipe, 0x580b, 0x0f);
	os04c10_write_register(ViPipe, 0x580d, 0x00);
	os04c10_write_register(ViPipe, 0x580f, 0x00);
	os04c10_write_register(ViPipe, 0x5820, 0x00);
	os04c10_write_register(ViPipe, 0x5821, 0x00);
	os04c10_write_register(ViPipe, 0x301c, 0xf8);
	os04c10_write_register(ViPipe, 0x301e, 0xb4);
	os04c10_write_register(ViPipe, 0x301f, 0xf0);
	os04c10_write_register(ViPipe, 0x3022, 0x01);
	os04c10_write_register(ViPipe, 0x3109, 0xe7);
	os04c10_write_register(ViPipe, 0x3600, 0x00);
	os04c10_write_register(ViPipe, 0x3610, 0x75);
	os04c10_write_register(ViPipe, 0x3611, 0x85);
	os04c10_write_register(ViPipe, 0x3613, 0x3a);
	os04c10_write_register(ViPipe, 0x3615, 0x60);
	os04c10_write_register(ViPipe, 0x3621, 0x90);
	os04c10_write_register(ViPipe, 0x3620, 0x0c);
	os04c10_write_register(ViPipe, 0x3629, 0x00);
	os04c10_write_register(ViPipe, 0x3661, 0x04);
	os04c10_write_register(ViPipe, 0x3664, 0x70);
	os04c10_write_register(ViPipe, 0x3665, 0x00);
	os04c10_write_register(ViPipe, 0x3681, 0xa6);
	os04c10_write_register(ViPipe, 0x3682, 0x53);
	os04c10_write_register(ViPipe, 0x3683, 0x2a);
	os04c10_write_register(ViPipe, 0x3684, 0x15);
	os04c10_write_register(ViPipe, 0x3700, 0x2a);
	os04c10_write_register(ViPipe, 0x3701, 0x12);
	os04c10_write_register(ViPipe, 0x3703, 0x28);
	os04c10_write_register(ViPipe, 0x3704, 0x0e);
	os04c10_write_register(ViPipe, 0x3706, 0x4a);
	os04c10_write_register(ViPipe, 0x3709, 0x4a);
	os04c10_write_register(ViPipe, 0x370b, 0xa2);
	os04c10_write_register(ViPipe, 0x370c, 0x01);
	os04c10_write_register(ViPipe, 0x370f, 0x04);
	os04c10_write_register(ViPipe, 0x3714, 0x24);
	os04c10_write_register(ViPipe, 0x3716, 0x24);
	os04c10_write_register(ViPipe, 0x3719, 0x11);
	os04c10_write_register(ViPipe, 0x371a, 0x1e);
	os04c10_write_register(ViPipe, 0x3720, 0x00);
	os04c10_write_register(ViPipe, 0x3724, 0x13);
	os04c10_write_register(ViPipe, 0x373f, 0xb0);
	os04c10_write_register(ViPipe, 0x3741, 0x4a);
	os04c10_write_register(ViPipe, 0x3743, 0x4a);
	os04c10_write_register(ViPipe, 0x3745, 0x4a);
	os04c10_write_register(ViPipe, 0x3747, 0x4a);
	os04c10_write_register(ViPipe, 0x3749, 0xa2);
	os04c10_write_register(ViPipe, 0x374b, 0xa2);
	os04c10_write_register(ViPipe, 0x374d, 0xa2);
	os04c10_write_register(ViPipe, 0x374f, 0xa2);
	os04c10_write_register(ViPipe, 0x3755, 0x10);
	os04c10_write_register(ViPipe, 0x376c, 0x00);
	os04c10_write_register(ViPipe, 0x378d, 0x30);
	os04c10_write_register(ViPipe, 0x3790, 0x4a);
	os04c10_write_register(ViPipe, 0x3791, 0xa2);
	os04c10_write_register(ViPipe, 0x3798, 0x40);
	os04c10_write_register(ViPipe, 0x379e, 0x00);
	os04c10_write_register(ViPipe, 0x379f, 0x04);
	os04c10_write_register(ViPipe, 0x37a1, 0x10);
	os04c10_write_register(ViPipe, 0x37a2, 0x1e);
	os04c10_write_register(ViPipe, 0x37a8, 0x10);
	os04c10_write_register(ViPipe, 0x37a9, 0x1e);
	os04c10_write_register(ViPipe, 0x37ac, 0xa0);
	os04c10_write_register(ViPipe, 0x37b9, 0x01);
	os04c10_write_register(ViPipe, 0x37bd, 0x01);
	os04c10_write_register(ViPipe, 0x37bf, 0x26);
	os04c10_write_register(ViPipe, 0x37c0, 0x11);
	os04c10_write_register(ViPipe, 0x37c2, 0x04);
	os04c10_write_register(ViPipe, 0x37cd, 0x19);
	os04c10_write_register(ViPipe, 0x37e0, 0x08);
	os04c10_write_register(ViPipe, 0x37e6, 0x04);
	os04c10_write_register(ViPipe, 0x37e5, 0x02);
	os04c10_write_register(ViPipe, 0x37e1, 0x0c);
	os04c10_write_register(ViPipe, 0x3737, 0x04);
	os04c10_write_register(ViPipe, 0x37d8, 0x02);
	os04c10_write_register(ViPipe, 0x37e2, 0x10);
	os04c10_write_register(ViPipe, 0x3739, 0x10);
	os04c10_write_register(ViPipe, 0x3662, 0x10);
	os04c10_write_register(ViPipe, 0x37e4, 0x20);
	os04c10_write_register(ViPipe, 0x37e3, 0x08);
	os04c10_write_register(ViPipe, 0x37d9, 0x08);
	os04c10_write_register(ViPipe, 0x4040, 0x00);
	os04c10_write_register(ViPipe, 0x4041, 0x07);
	os04c10_write_register(ViPipe, 0x4008, 0x02);
	os04c10_write_register(ViPipe, 0x4009, 0x0d);
	os04c10_write_register(ViPipe, 0x3800, 0x00);
	os04c10_write_register(ViPipe, 0x3801, 0x00);
	os04c10_write_register(ViPipe, 0x3802, 0x00);
	os04c10_write_register(ViPipe, 0x3803, 0x00);
	os04c10_write_register(ViPipe, 0x3804, 0x0a);
	os04c10_write_register(ViPipe, 0x3805, 0x8f);
	os04c10_write_register(ViPipe, 0x3806, 0x05);
	os04c10_write_register(ViPipe, 0x3807, 0xff);
	os04c10_write_register(ViPipe, 0x3808, 0x0a);
	os04c10_write_register(ViPipe, 0x3809, 0x80);
	os04c10_write_register(ViPipe, 0x380a, 0x05);
	os04c10_write_register(ViPipe, 0x380b, 0xf0);
	os04c10_write_register(ViPipe, 0x380c, 0x04);
	os04c10_write_register(ViPipe, 0x380d, 0x2e);
	os04c10_write_register(ViPipe, 0x380e, 0x06);
	os04c10_write_register(ViPipe, 0x380f, 0x92);
	os04c10_write_register(ViPipe, 0x3811, 0x08);
	os04c10_write_register(ViPipe, 0x3813, 0x08);
	os04c10_write_register(ViPipe, 0x3814, 0x01);
	os04c10_write_register(ViPipe, 0x3815, 0x01);
	os04c10_write_register(ViPipe, 0x3816, 0x01);
	os04c10_write_register(ViPipe, 0x3817, 0x01);
	os04c10_write_register(ViPipe, 0x3820, 0x88);
	os04c10_write_register(ViPipe, 0x3821, 0x04);
	os04c10_write_register(ViPipe, 0x3880, 0x25);
	os04c10_write_register(ViPipe, 0x3882, 0x20);
	os04c10_write_register(ViPipe, 0x3c91, 0x0b);
	os04c10_write_register(ViPipe, 0x3c94, 0x45);
	os04c10_write_register(ViPipe, 0x4000, 0xf3);
	os04c10_write_register(ViPipe, 0x4001, 0x60);
	os04c10_write_register(ViPipe, 0x4003, 0x40);
	os04c10_write_register(ViPipe, 0x4300, 0xff);
	os04c10_write_register(ViPipe, 0x4302, 0x0f);
	os04c10_write_register(ViPipe, 0x4305, 0x83);
	os04c10_write_register(ViPipe, 0x4505, 0x84);
	os04c10_write_register(ViPipe, 0x4809, 0x0e);
	os04c10_write_register(ViPipe, 0x480a, 0x04);
	os04c10_write_register(ViPipe, 0x4837, 0x14);
	os04c10_write_register(ViPipe, 0x4c00, 0x08);
	os04c10_write_register(ViPipe, 0x4c01, 0x08);
	os04c10_write_register(ViPipe, 0x4c04, 0x00);
	os04c10_write_register(ViPipe, 0x4c05, 0x00);
	os04c10_write_register(ViPipe, 0x5000, 0xf9);
	os04c10_write_register(ViPipe, 0x3624, 0x00);
	os04c10_write_register(ViPipe, 0x3016, 0x32);
	os04c10_write_register(ViPipe, 0x0306, 0x00);
	os04c10_write_register(ViPipe, 0x4837, 0x0a);
	os04c10_write_register(ViPipe, 0x0305, 0x5a);
	os04c10_write_register(ViPipe, 0x0325, 0x6b);
	os04c10_write_register(ViPipe, 0x3106, 0x25);
	/*  LCG-LCG */
	os04c10_write_register(ViPipe, 0x320d, 0x00);
	os04c10_write_register(ViPipe, 0x3208, 0x00);
	os04c10_write_register(ViPipe, 0x3698, 0x00);
	os04c10_write_register(ViPipe, 0x3699, 0x80);
	os04c10_write_register(ViPipe, 0x369a, 0x80);
	os04c10_write_register(ViPipe, 0x369b, 0x1f);
	os04c10_write_register(ViPipe, 0x369c, 0x1f);
	os04c10_write_register(ViPipe, 0x369d, 0x80);
	os04c10_write_register(ViPipe, 0x369e, 0x40);
	os04c10_write_register(ViPipe, 0x369f, 0x21);
	os04c10_write_register(ViPipe, 0x36a0, 0x12);
	os04c10_write_register(ViPipe, 0x36a1, 0xdd);
	os04c10_write_register(ViPipe, 0x370e, 0x00);
	os04c10_write_register(ViPipe, 0x3713, 0x04);
	os04c10_write_register(ViPipe, 0x379c, 0x00);
	os04c10_write_register(ViPipe, 0x379d, 0x00);
	os04c10_write_register(ViPipe, 0x37be, 0x26);
	os04c10_write_register(ViPipe, 0x37c7, 0xa8);
	os04c10_write_register(ViPipe, 0x3881, 0x00);
	os04c10_write_register(ViPipe, 0x3681, 0x80);
	os04c10_write_register(ViPipe, 0x3682, 0x40);
	os04c10_write_register(ViPipe, 0x3683, 0x21);
	os04c10_write_register(ViPipe, 0x3684, 0x12);
	os04c10_write_register(ViPipe, 0x370f, 0x00);
	os04c10_write_register(ViPipe, 0x379f, 0x04);
	os04c10_write_register(ViPipe, 0x37ac, 0xa0);
	os04c10_write_register(ViPipe, 0x37bf, 0x26);
	os04c10_write_register(ViPipe, 0x3880, 0x00);
	os04c10_write_register(ViPipe, 0x3208, 0x10);
	os04c10_write_register(ViPipe, 0x320d, 0x00);
	os04c10_write_register(ViPipe, 0x3208, 0xa0);

	os04c10_default_reg_init(ViPipe);

	if (!g_au16Os04c10_UseHwSync[ViPipe]) {
		/* freerun */
		os04c10_write_register(ViPipe, 0x3002, 0x21);
	} else {
		/* auto master */
#if HW_SYNC_AUTO
		os04c10_write_register(ViPipe, 0x3002, 0x22); // [1] vsync_oen, [0]: fsin_oen
		os04c10_write_register(ViPipe, 0x3690, 0x00); // [4]: 1'b0, 1st set vsync pulse
		os04c10_write_register(ViPipe, 0x383e, 0x00); // vscyn_rise_rcnt_pt[23:16]
		os04c10_write_register(ViPipe, 0x3818, 0x00); // Slave vsync pulse position cs [15:8]
		os04c10_write_register(ViPipe, 0x3819, 0x00); // Slave vsync pulse position cs [7:0],
							      // max is HTS/4
		os04c10_write_register(ViPipe, 0x381a, 0x00); // vscyn_rise_rcnt_pt[15:8]
		os04c10_write_register(ViPipe, 0x381b, 0x00); // vscyn_rise_rcnt_pt[7:0], max:
							      // VTS-12 for AHBIN 720p, (VTS -12)*2 for
							      // other formats
		os04c10_write_register(ViPipe, 0x3832, 0xf8); // default, 8'h08, [7:4] vsync pulse width
		os04c10_write_register(ViPipe, 0x368a, 0x04); // GPIO enable
#else
		/* manual master */
		os04c10_write_register(ViPipe, 0x3002, 0x22); // [1] vsync_oen, [0]: fsin_oen
		os04c10_write_register(ViPipe, 0x3690, 0x00); // [4]: 1'b0, 1st set vsync pulse
		os04c10_write_register(ViPipe, 0x383e, 0x00); // vscyn_rise_rcnt_pt[23:16]
		os04c10_write_register(ViPipe, 0x3818, 0x00); // Slave vsync pulse position cs [15:8]
		os04c10_write_register(ViPipe, 0x3819, 0x00); // Slave vsync pulse position cs [7:0],
							      // max is HTS/4
		os04c10_write_register(ViPipe, 0x381a, 0x00); // vscyn_rise_rcnt_pt[15:8]
		os04c10_write_register(ViPipe, 0x381b, 0x18); // vscyn_rise_rcnt_pt[7:0], max:
							      // VTS-12 for AHBIN 720p, (VTS -12)*2 for
							      // other formats
		os04c10_write_register(ViPipe, 0x3832, 0xf8); // default, 8'h08, [7:4] vsync pulse width
		os04c10_write_register(ViPipe, 0x368a, 0x04); // GPIO enable
#endif
	}

	os04c10_write_register(ViPipe, 0x0100, 0x01);

	usleep(50*1000);

	printf("===Os04c10 sensor 1520P30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}

static void os04c10_wdr_1440p30_2to1_init(VI_PIPE ViPipe)
{
	os04c10_write_register(ViPipe, 0x0100, 0x00);
	os04c10_write_register(ViPipe, 0x0103, 0x01);
	os04c10_write_register(ViPipe, 0x0301, 0x84);
	os04c10_write_register(ViPipe, 0x0303, 0x01);
	os04c10_write_register(ViPipe, 0x0305, 0x61);
	os04c10_write_register(ViPipe, 0x0306, 0x01);
	os04c10_write_register(ViPipe, 0x0307, 0x17);
	os04c10_write_register(ViPipe, 0x0323, 0x04);
	os04c10_write_register(ViPipe, 0x0324, 0x01);
	os04c10_write_register(ViPipe, 0x0325, 0x7a);
	os04c10_write_register(ViPipe, 0x3012, 0x06);
	os04c10_write_register(ViPipe, 0x3013, 0x02);
	os04c10_write_register(ViPipe, 0x3016, 0x72);
	os04c10_write_register(ViPipe, 0x3021, 0x03);
	os04c10_write_register(ViPipe, 0x3106, 0x21);
	os04c10_write_register(ViPipe, 0x3107, 0xa1);
	os04c10_write_register(ViPipe, 0x3500, 0x00);
	os04c10_write_register(ViPipe, 0x3501, 0x03);
	os04c10_write_register(ViPipe, 0x3502, 0x08);
	os04c10_write_register(ViPipe, 0x3503, 0x88);
	os04c10_write_register(ViPipe, 0x3508, 0x00);
	os04c10_write_register(ViPipe, 0x3509, 0x80);
	os04c10_write_register(ViPipe, 0x350a, 0x04);
	os04c10_write_register(ViPipe, 0x350b, 0x00);
	os04c10_write_register(ViPipe, 0x350c, 0x00);
	os04c10_write_register(ViPipe, 0x350d, 0x80);
	os04c10_write_register(ViPipe, 0x350e, 0x04);
	os04c10_write_register(ViPipe, 0x350f, 0x00);
	os04c10_write_register(ViPipe, 0x3510, 0x00);
	os04c10_write_register(ViPipe, 0x3511, 0x01);
	os04c10_write_register(ViPipe, 0x3512, 0x08);
	os04c10_write_register(ViPipe, 0x3624, 0x02);
	os04c10_write_register(ViPipe, 0x3625, 0x4c);
	os04c10_write_register(ViPipe, 0x3660, 0x04);
	os04c10_write_register(ViPipe, 0x3666, 0xa5);
	os04c10_write_register(ViPipe, 0x3667, 0xa5);
	os04c10_write_register(ViPipe, 0x366a, 0x54);
	os04c10_write_register(ViPipe, 0x3673, 0x0d);
	os04c10_write_register(ViPipe, 0x3672, 0x0d);
	os04c10_write_register(ViPipe, 0x3671, 0x0d);
	os04c10_write_register(ViPipe, 0x3670, 0x0d);
	os04c10_write_register(ViPipe, 0x3685, 0x00);
	os04c10_write_register(ViPipe, 0x3694, 0x0d);
	os04c10_write_register(ViPipe, 0x3693, 0x0d);
	os04c10_write_register(ViPipe, 0x3692, 0x0d);
	os04c10_write_register(ViPipe, 0x3691, 0x0d);
	os04c10_write_register(ViPipe, 0x3696, 0x4c);
	os04c10_write_register(ViPipe, 0x3697, 0x4c);
	os04c10_write_register(ViPipe, 0x3698, 0x40);
	os04c10_write_register(ViPipe, 0x3699, 0x80);
	os04c10_write_register(ViPipe, 0x369a, 0x18);
	os04c10_write_register(ViPipe, 0x369b, 0x1f);
	os04c10_write_register(ViPipe, 0x369c, 0x14);
	os04c10_write_register(ViPipe, 0x369d, 0x80);
	os04c10_write_register(ViPipe, 0x369e, 0x40);
	os04c10_write_register(ViPipe, 0x369f, 0x21);
	os04c10_write_register(ViPipe, 0x36a0, 0x12);
	os04c10_write_register(ViPipe, 0x36a1, 0x5d);
	os04c10_write_register(ViPipe, 0x36a2, 0x66);
	os04c10_write_register(ViPipe, 0x370a, 0x00);
	os04c10_write_register(ViPipe, 0x370e, 0x0c);
	os04c10_write_register(ViPipe, 0x3710, 0x00);
	os04c10_write_register(ViPipe, 0x3713, 0x00);
	os04c10_write_register(ViPipe, 0x3725, 0x02);
	os04c10_write_register(ViPipe, 0x372a, 0x03);
	os04c10_write_register(ViPipe, 0x3738, 0xce);
	os04c10_write_register(ViPipe, 0x3748, 0x00);
	os04c10_write_register(ViPipe, 0x374a, 0x00);
	os04c10_write_register(ViPipe, 0x374c, 0x00);
	os04c10_write_register(ViPipe, 0x374e, 0x00);
	os04c10_write_register(ViPipe, 0x3756, 0x00);
	os04c10_write_register(ViPipe, 0x3757, 0x00);
	os04c10_write_register(ViPipe, 0x3767, 0x00);
	os04c10_write_register(ViPipe, 0x3771, 0x00);
	os04c10_write_register(ViPipe, 0x377b, 0x28);
	os04c10_write_register(ViPipe, 0x377c, 0x00);
	os04c10_write_register(ViPipe, 0x377d, 0x0c);
	os04c10_write_register(ViPipe, 0x3781, 0x03);
	os04c10_write_register(ViPipe, 0x3782, 0x00);
	os04c10_write_register(ViPipe, 0x3789, 0x14);
	os04c10_write_register(ViPipe, 0x3795, 0x02);
	os04c10_write_register(ViPipe, 0x379c, 0x00);
	os04c10_write_register(ViPipe, 0x379d, 0x00);
	os04c10_write_register(ViPipe, 0x37b8, 0x04);
	os04c10_write_register(ViPipe, 0x37ba, 0x03);
	os04c10_write_register(ViPipe, 0x37bb, 0x00);
	os04c10_write_register(ViPipe, 0x37bc, 0x04);
	os04c10_write_register(ViPipe, 0x37be, 0x08);
	os04c10_write_register(ViPipe, 0x37c4, 0x11);
	os04c10_write_register(ViPipe, 0x37c5, 0x80);
	os04c10_write_register(ViPipe, 0x37c6, 0x14);
	os04c10_write_register(ViPipe, 0x37c7, 0x08);
	os04c10_write_register(ViPipe, 0x37da, 0x11);
	os04c10_write_register(ViPipe, 0x381f, 0x08);
	os04c10_write_register(ViPipe, 0x3829, 0x03);
	os04c10_write_register(ViPipe, 0x3881, 0x00);
	os04c10_write_register(ViPipe, 0x3888, 0x04);
	os04c10_write_register(ViPipe, 0x388b, 0x00);
	os04c10_write_register(ViPipe, 0x3c80, 0x10);
	os04c10_write_register(ViPipe, 0x3c86, 0x00);
	os04c10_write_register(ViPipe, 0x3c8c, 0x20);
	os04c10_write_register(ViPipe, 0x3c9f, 0x01);
	os04c10_write_register(ViPipe, 0x3d85, 0x1b);
	os04c10_write_register(ViPipe, 0x3d8c, 0x71);
	os04c10_write_register(ViPipe, 0x3d8d, 0xe2);
	os04c10_write_register(ViPipe, 0x3f00, 0x0b);
	os04c10_write_register(ViPipe, 0x3f06, 0x04);
	os04c10_write_register(ViPipe, 0x400a, 0x01);
	os04c10_write_register(ViPipe, 0x400b, 0x50);
	os04c10_write_register(ViPipe, 0x400e, 0x08);
	os04c10_write_register(ViPipe, 0x4043, 0x7e);
	os04c10_write_register(ViPipe, 0x4045, 0x7e);
	os04c10_write_register(ViPipe, 0x4047, 0x7e);
	os04c10_write_register(ViPipe, 0x4049, 0x7e);
	os04c10_write_register(ViPipe, 0x4090, 0x14);
	os04c10_write_register(ViPipe, 0x40b0, 0x00);
	os04c10_write_register(ViPipe, 0x40b1, 0x00);
	os04c10_write_register(ViPipe, 0x40b2, 0x00);
	os04c10_write_register(ViPipe, 0x40b3, 0x00);
	os04c10_write_register(ViPipe, 0x40b4, 0x00);
	os04c10_write_register(ViPipe, 0x40b5, 0x00);
	os04c10_write_register(ViPipe, 0x40b7, 0x00);
	os04c10_write_register(ViPipe, 0x40b8, 0x00);
	os04c10_write_register(ViPipe, 0x40b9, 0x00);
	os04c10_write_register(ViPipe, 0x40ba, 0x01);
	os04c10_write_register(ViPipe, 0x4301, 0x00);
	os04c10_write_register(ViPipe, 0x4303, 0x00);
	os04c10_write_register(ViPipe, 0x4502, 0x04);
	os04c10_write_register(ViPipe, 0x4503, 0x00);
	os04c10_write_register(ViPipe, 0x4504, 0x06);
	os04c10_write_register(ViPipe, 0x4506, 0x00);
	os04c10_write_register(ViPipe, 0x4507, 0x47);
	os04c10_write_register(ViPipe, 0x4803, 0x00);
	os04c10_write_register(ViPipe, 0x480c, 0x32);
	os04c10_write_register(ViPipe, 0x480e, 0x04);
	os04c10_write_register(ViPipe, 0x4813, 0xe4);
	os04c10_write_register(ViPipe, 0x4819, 0x70);
	os04c10_write_register(ViPipe, 0x481f, 0x30);
	os04c10_write_register(ViPipe, 0x4823, 0x3f);
	os04c10_write_register(ViPipe, 0x4825, 0x30);
	os04c10_write_register(ViPipe, 0x4833, 0x10);
	os04c10_write_register(ViPipe, 0x484b, 0x27);
	os04c10_write_register(ViPipe, 0x488b, 0x00);
	os04c10_write_register(ViPipe, 0x4d00, 0x04);
	os04c10_write_register(ViPipe, 0x4d01, 0xad);
	os04c10_write_register(ViPipe, 0x4d02, 0xbc);
	os04c10_write_register(ViPipe, 0x4d03, 0xa1);
	os04c10_write_register(ViPipe, 0x4d04, 0x1f);
	os04c10_write_register(ViPipe, 0x4d05, 0x4c);
	os04c10_write_register(ViPipe, 0x4d0b, 0x01);
	os04c10_write_register(ViPipe, 0x4e00, 0x2a);
	os04c10_write_register(ViPipe, 0x4e0d, 0x00);
	os04c10_write_register(ViPipe, 0x5001, 0x09);
	os04c10_write_register(ViPipe, 0x5004, 0x00);
	os04c10_write_register(ViPipe, 0x5080, 0x04);
	os04c10_write_register(ViPipe, 0x5036, 0x80);
	os04c10_write_register(ViPipe, 0x5180, 0x70);
	os04c10_write_register(ViPipe, 0x5181, 0x10);
	os04c10_write_register(ViPipe, 0x520a, 0x03);
	os04c10_write_register(ViPipe, 0x520b, 0x06);
	os04c10_write_register(ViPipe, 0x520c, 0x0c);
	os04c10_write_register(ViPipe, 0x580b, 0x0f);
	os04c10_write_register(ViPipe, 0x580d, 0x00);
	os04c10_write_register(ViPipe, 0x580f, 0x00);
	os04c10_write_register(ViPipe, 0x5820, 0x00);
	os04c10_write_register(ViPipe, 0x5821, 0x00);
	os04c10_write_register(ViPipe, 0x301c, 0xf8);
	os04c10_write_register(ViPipe, 0x301e, 0xb4);
	os04c10_write_register(ViPipe, 0x301f, 0xf0);
	os04c10_write_register(ViPipe, 0x3022, 0x01);
	os04c10_write_register(ViPipe, 0x3109, 0xe7);
	os04c10_write_register(ViPipe, 0x3600, 0x00);
	os04c10_write_register(ViPipe, 0x3610, 0x75);
	os04c10_write_register(ViPipe, 0x3611, 0x85);
	os04c10_write_register(ViPipe, 0x3613, 0x3a);
	os04c10_write_register(ViPipe, 0x3615, 0x60);
	os04c10_write_register(ViPipe, 0x3621, 0x90);
	os04c10_write_register(ViPipe, 0x3620, 0x0c);
	os04c10_write_register(ViPipe, 0x3629, 0x00);
	os04c10_write_register(ViPipe, 0x3661, 0x04);
	os04c10_write_register(ViPipe, 0x3664, 0x70);
	os04c10_write_register(ViPipe, 0x3665, 0x00);
	os04c10_write_register(ViPipe, 0x3681, 0xa6);
	os04c10_write_register(ViPipe, 0x3682, 0x53);
	os04c10_write_register(ViPipe, 0x3683, 0x2a);
	os04c10_write_register(ViPipe, 0x3684, 0x15);
	os04c10_write_register(ViPipe, 0x3700, 0x2a);
	os04c10_write_register(ViPipe, 0x3701, 0x12);
	os04c10_write_register(ViPipe, 0x3703, 0x28);
	os04c10_write_register(ViPipe, 0x3704, 0x0e);
	os04c10_write_register(ViPipe, 0x3706, 0x4a);
	os04c10_write_register(ViPipe, 0x3709, 0x4a);
	os04c10_write_register(ViPipe, 0x370b, 0xa2);
	os04c10_write_register(ViPipe, 0x370c, 0x01);
	os04c10_write_register(ViPipe, 0x370f, 0x04);
	os04c10_write_register(ViPipe, 0x3714, 0x24);
	os04c10_write_register(ViPipe, 0x3716, 0x24);
	os04c10_write_register(ViPipe, 0x3719, 0x11);
	os04c10_write_register(ViPipe, 0x371a, 0x1e);
	os04c10_write_register(ViPipe, 0x3720, 0x00);
	os04c10_write_register(ViPipe, 0x3724, 0x13);
	os04c10_write_register(ViPipe, 0x373f, 0xb0);
	os04c10_write_register(ViPipe, 0x3741, 0x4a);
	os04c10_write_register(ViPipe, 0x3743, 0x4a);
	os04c10_write_register(ViPipe, 0x3745, 0x4a);
	os04c10_write_register(ViPipe, 0x3747, 0x4a);
	os04c10_write_register(ViPipe, 0x3749, 0xa2);
	os04c10_write_register(ViPipe, 0x374b, 0xa2);
	os04c10_write_register(ViPipe, 0x374d, 0xa2);
	os04c10_write_register(ViPipe, 0x374f, 0xa2);
	os04c10_write_register(ViPipe, 0x3755, 0x10);
	os04c10_write_register(ViPipe, 0x376c, 0x00);
	os04c10_write_register(ViPipe, 0x378d, 0x30);
	os04c10_write_register(ViPipe, 0x3790, 0x4a);
	os04c10_write_register(ViPipe, 0x3791, 0xa2);
	os04c10_write_register(ViPipe, 0x3798, 0x40);
	os04c10_write_register(ViPipe, 0x379e, 0x00);
	os04c10_write_register(ViPipe, 0x379f, 0x04);
	os04c10_write_register(ViPipe, 0x37a1, 0x10);
	os04c10_write_register(ViPipe, 0x37a2, 0x1e);
	os04c10_write_register(ViPipe, 0x37a8, 0x10);
	os04c10_write_register(ViPipe, 0x37a9, 0x1e);
	os04c10_write_register(ViPipe, 0x37ac, 0xa0);
	os04c10_write_register(ViPipe, 0x37b9, 0x01);
	os04c10_write_register(ViPipe, 0x37bd, 0x01);
	os04c10_write_register(ViPipe, 0x37bf, 0x26);
	os04c10_write_register(ViPipe, 0x37c0, 0x11);
	os04c10_write_register(ViPipe, 0x37c2, 0x04);
	os04c10_write_register(ViPipe, 0x37cd, 0x19);
	os04c10_write_register(ViPipe, 0x37e0, 0x08);
	os04c10_write_register(ViPipe, 0x37e6, 0x04);
	os04c10_write_register(ViPipe, 0x37e5, 0x02);
	os04c10_write_register(ViPipe, 0x37e1, 0x0c);
	os04c10_write_register(ViPipe, 0x3737, 0x04);
	os04c10_write_register(ViPipe, 0x37d8, 0x02);
	os04c10_write_register(ViPipe, 0x37e2, 0x10);
	os04c10_write_register(ViPipe, 0x3739, 0x10);
	os04c10_write_register(ViPipe, 0x3662, 0x10);
	os04c10_write_register(ViPipe, 0x37e4, 0x20);
	os04c10_write_register(ViPipe, 0x37e3, 0x08);
	os04c10_write_register(ViPipe, 0x37d9, 0x08);
	os04c10_write_register(ViPipe, 0x4040, 0x00);
	os04c10_write_register(ViPipe, 0x4041, 0x07);
	os04c10_write_register(ViPipe, 0x4008, 0x02);
	os04c10_write_register(ViPipe, 0x4009, 0x0d);
	os04c10_write_register(ViPipe, 0x3800, 0x00);
	os04c10_write_register(ViPipe, 0x3801, 0x40);
	os04c10_write_register(ViPipe, 0x3802, 0x00);
	os04c10_write_register(ViPipe, 0x3803, 0x28);
	os04c10_write_register(ViPipe, 0x3804, 0x0a);
	os04c10_write_register(ViPipe, 0x3805, 0x4f);
	os04c10_write_register(ViPipe, 0x3806, 0x05);
	os04c10_write_register(ViPipe, 0x3807, 0xd7);
	os04c10_write_register(ViPipe, 0x3808, 0x0a);
	os04c10_write_register(ViPipe, 0x3809, 0x00);
	os04c10_write_register(ViPipe, 0x380a, 0x05);
	os04c10_write_register(ViPipe, 0x380b, 0xa0);
	os04c10_write_register(ViPipe, 0x380c, 0x04);
	os04c10_write_register(ViPipe, 0x380d, 0x2e);
	os04c10_write_register(ViPipe, 0x380e, 0x06);
	os04c10_write_register(ViPipe, 0x380f, 0x92);
	os04c10_write_register(ViPipe, 0x3811, 0x08);
	os04c10_write_register(ViPipe, 0x3813, 0x08);
	os04c10_write_register(ViPipe, 0x3814, 0x01);
	os04c10_write_register(ViPipe, 0x3815, 0x01);
	os04c10_write_register(ViPipe, 0x3816, 0x01);
	os04c10_write_register(ViPipe, 0x3817, 0x01);
	os04c10_write_register(ViPipe, 0x3820, 0x88);
	os04c10_write_register(ViPipe, 0x3821, 0x04);
	os04c10_write_register(ViPipe, 0x3880, 0x25);
	os04c10_write_register(ViPipe, 0x3882, 0x20);
	os04c10_write_register(ViPipe, 0x3c91, 0x0b);
	os04c10_write_register(ViPipe, 0x3c94, 0x45);
	os04c10_write_register(ViPipe, 0x4000, 0xf3);
	os04c10_write_register(ViPipe, 0x4001, 0x60);
	os04c10_write_register(ViPipe, 0x4003, 0x40);
	os04c10_write_register(ViPipe, 0x4300, 0xff);
	os04c10_write_register(ViPipe, 0x4302, 0x0f);
	os04c10_write_register(ViPipe, 0x4305, 0x83);
	os04c10_write_register(ViPipe, 0x4505, 0x84);
	os04c10_write_register(ViPipe, 0x4809, 0x0e);
	os04c10_write_register(ViPipe, 0x480a, 0x04);
	os04c10_write_register(ViPipe, 0x4837, 0x14);
	os04c10_write_register(ViPipe, 0x4c00, 0x08);
	os04c10_write_register(ViPipe, 0x4c01, 0x08);
	os04c10_write_register(ViPipe, 0x4c04, 0x00);
	os04c10_write_register(ViPipe, 0x4c05, 0x00);
	os04c10_write_register(ViPipe, 0x5000, 0xe9);
	os04c10_write_register(ViPipe, 0x3624, 0x00);
	os04c10_write_register(ViPipe, 0x3016, 0x32);
	os04c10_write_register(ViPipe, 0x0306, 0x00);
	os04c10_write_register(ViPipe, 0x4837, 0x0a);
	os04c10_write_register(ViPipe, 0x0305, 0x5a);
	os04c10_write_register(ViPipe, 0x0325, 0x6b);
	os04c10_write_register(ViPipe, 0x3106, 0x25);
	os04c10_default_reg_init(ViPipe);

	if (!g_au16Os04c10_UseHwSync[ViPipe]) {
		/* freerun */
		os04c10_write_register(ViPipe, 0x3002, 0x21);
	} else {
		/* auto master */
		os04c10_write_register(ViPipe, 0x3002, 0x22); // [1] vsync_oen, [0]: fsin_oen
		os04c10_write_register(ViPipe, 0x3690, 0x00); // [4]: 1'b0, 1st set vsync pulse
		os04c10_write_register(ViPipe, 0x383e, 0x00); // vscyn_rise_rcnt_pt[23:16]
		os04c10_write_register(ViPipe, 0x3818, 0x00); // Slave vsync pulse position cs [15:8]
		os04c10_write_register(ViPipe, 0x3819, 0x00); // Slave vsync pulse position cs [7:0],
							      // max is HTS/4
		os04c10_write_register(ViPipe, 0x381a, 0x00); // vscyn_rise_rcnt_pt[15:8]
		os04c10_write_register(ViPipe, 0x381b, 0x00); // vscyn_rise_rcnt_pt[7:0], max:
							      // VTS-12 for AHBIN 720p, (VTS -12)*2 for
							      // other formats
		os04c10_write_register(ViPipe, 0x3832, 0xf8); // default, 8'h08, [7:4] vsync pulse width
		os04c10_write_register(ViPipe, 0x368a, 0x04); // GPIO enable
	}

	os04c10_write_register(ViPipe, 0x0100, 0x01);

	usleep(50*1000);

	printf("===Os04c10 sensor 1440P30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}