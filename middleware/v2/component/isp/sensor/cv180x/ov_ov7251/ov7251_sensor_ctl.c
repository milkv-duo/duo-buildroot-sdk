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
#include "ov7251_cmos_ex.h"

static void ov7251_linear_480p120_init(VI_PIPE ViPipe);

CVI_U8 ov7251_i2c_addr = 0x60;        /* I2C Address of OV7251 */
const CVI_U32 ov7251_addr_byte = 2;
const CVI_U32 ov7251_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int ov7251_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunOv7251_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, ov7251_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int ov7251_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int ov7251_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (ov7251_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, ov7251_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, ov7251_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (ov7251_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;

}

int ov7251_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (ov7251_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (ov7251_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, ov7251_addr_byte + ov7251_data_byte);
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

void ov7251_standby(VI_PIPE ViPipe)
{
	ov7251_write_register(ViPipe, 0x0100, 0x00); /* standby */
}

void ov7251_restart(VI_PIPE ViPipe)
{
	ov7251_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	ov7251_write_register(ViPipe, 0x0100, 0x01); /* restart */
}

void ov7251_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastOv7251[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastOv7251[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			ov7251_write_register(ViPipe,
				g_pastOv7251[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOv7251[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void ov7251_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 flip, mirror;

	flip = ov7251_read_register(ViPipe, 0x3820);
	mirror = ov7251_read_register(ViPipe, 0x3821);

	flip &= ~(0x1 << 2);
	mirror &= ~(0x1 << 2);

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		mirror |= 0x1 << 2;
		break;
	case ISP_SNS_FLIP:
		flip |= 0x1 << 2;
		break;
	case ISP_SNS_MIRROR_FLIP:
		flip |= 0x1 << 2;
		mirror |= 0x1 << 2;
		break;
	default:
		return;
	}

	ov7251_write_register(ViPipe, 0x3820, flip);
	ov7251_write_register(ViPipe, 0x3821, mirror);
}

#define OV7251_CHIP_ID_ADDR_H		0x300A
#define OV7251_CHIP_ID_ADDR_L		0x300B
#define OV7251_CHIP_ID			0x7750

int ov7251_probe(VI_PIPE ViPipe)
{
	int nVal, nVal2;

	if (ov7251_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	delay_ms(5);

	nVal  = ov7251_read_register(ViPipe, OV7251_CHIP_ID_ADDR_H);
	nVal2 = ov7251_read_register(ViPipe, OV7251_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | ((nVal2 & 0xFF) << 0)) != OV7251_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void ov7251_init(VI_PIPE ViPipe)
{
	ov7251_i2c_init(ViPipe);

	ov7251_linear_480p120_init(ViPipe);

	g_pastOv7251[ViPipe]->bInit = CVI_TRUE;
}

void ov7251_exit(VI_PIPE ViPipe)
{
	ov7251_i2c_exit(ViPipe);
}

/* 1944P30 and 1944P25 */
static void ov7251_linear_480p120_init(VI_PIPE ViPipe)
{
	ov7251_write_register(ViPipe, 0x0103, 0x01);
	ov7251_write_register(ViPipe, 0x0100, 0x00);
	ov7251_write_register(ViPipe, 0x3012, 0xc0);
	ov7251_write_register(ViPipe, 0x3013, 0xd2);
	ov7251_write_register(ViPipe, 0x3016, 0x10);
	ov7251_write_register(ViPipe, 0x3017, 0x00);
	ov7251_write_register(ViPipe, 0x3018, 0x00);
	ov7251_write_register(ViPipe, 0x301a, 0x00);
	ov7251_write_register(ViPipe, 0x301b, 0x00);
	ov7251_write_register(ViPipe, 0x301c, 0x00);
	ov7251_write_register(ViPipe, 0x3023, 0x05);
	ov7251_write_register(ViPipe, 0x3099, 0x32);
	ov7251_write_register(ViPipe, 0x30b3, 0x64);
	ov7251_write_register(ViPipe, 0x30b4, 0x03);
	ov7251_write_register(ViPipe, 0x30b5, 0x05);
	ov7251_write_register(ViPipe, 0x3106, 0xda);
	ov7251_write_register(ViPipe, 0x3501, 0x1f);
	ov7251_write_register(ViPipe, 0x3502, 0x80);
	ov7251_write_register(ViPipe, 0x3503, 0x07);
	ov7251_write_register(ViPipe, 0x3600, 0x1c);
	ov7251_write_register(ViPipe, 0x3602, 0x62);
	ov7251_write_register(ViPipe, 0x3620, 0xb7);
	ov7251_write_register(ViPipe, 0x3622, 0x04);
	ov7251_write_register(ViPipe, 0x3626, 0x21);
	ov7251_write_register(ViPipe, 0x3627, 0x30);
	ov7251_write_register(ViPipe, 0x3630, 0x44);
	ov7251_write_register(ViPipe, 0x3631, 0x35);
	ov7251_write_register(ViPipe, 0x3634, 0x60);
	ov7251_write_register(ViPipe, 0x3663, 0x70);
	ov7251_write_register(ViPipe, 0x3669, 0x1a);
	ov7251_write_register(ViPipe, 0x3673, 0x01);
	ov7251_write_register(ViPipe, 0x3674, 0xef);
	ov7251_write_register(ViPipe, 0x3675, 0x03);
	ov7251_write_register(ViPipe, 0x3705, 0xc1);
	ov7251_write_register(ViPipe, 0x3757, 0xb3);
	ov7251_write_register(ViPipe, 0x37a8, 0x01);
	ov7251_write_register(ViPipe, 0x37a9, 0xc0);
	ov7251_write_register(ViPipe, 0x380d, 0xa0);
	ov7251_write_register(ViPipe, 0x380f, 0x1a);
	ov7251_write_register(ViPipe, 0x3811, 0x04);
	ov7251_write_register(ViPipe, 0x3813, 0x05);
	ov7251_write_register(ViPipe, 0x3820, 0x40);
	ov7251_write_register(ViPipe, 0x382f, 0x0e);
	ov7251_write_register(ViPipe, 0x3835, 0x0c);
	ov7251_write_register(ViPipe, 0x3b80, 0x00);
	ov7251_write_register(ViPipe, 0x3c01, 0x63);
	ov7251_write_register(ViPipe, 0x3c07, 0x06);
	ov7251_write_register(ViPipe, 0x3c0c, 0x01);
	ov7251_write_register(ViPipe, 0x3c0d, 0xd0);
	ov7251_write_register(ViPipe, 0x3c0e, 0x02);
	ov7251_write_register(ViPipe, 0x3c0f, 0x0a);
	ov7251_write_register(ViPipe, 0x4001, 0x42);
	ov7251_write_register(ViPipe, 0x404e, 0x01);
	ov7251_write_register(ViPipe, 0x4501, 0x48);
	ov7251_write_register(ViPipe, 0x4601, 0x4e);
	ov7251_write_register(ViPipe, 0x4801, 0x0f);
	ov7251_write_register(ViPipe, 0x4819, 0xaa);
	ov7251_write_register(ViPipe, 0x4823, 0x3e);
	ov7251_write_register(ViPipe, 0x4a47, 0x7f);
	ov7251_write_register(ViPipe, 0x4a49, 0xf0);
	ov7251_write_register(ViPipe, 0x4a4b, 0x30);
	ov7251_write_register(ViPipe, 0x5001, 0x80);

	ov7251_default_reg_init(ViPipe);

	ov7251_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(100);

	printf("ViPipe:%d,===OV7251 480P 120fps 10bit LINE Init OK!===\n", ViPipe);
}

