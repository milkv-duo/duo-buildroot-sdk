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
#include "ov6211_cmos_ex.h"

static void ov6211_linear_400p120_init(VI_PIPE ViPipe);

CVI_U8 ov6211_i2c_addr = 0x60;        /* I2C Address of OV6211 */
const CVI_U32 ov6211_addr_byte = 2;
const CVI_U32 ov6211_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int ov6211_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunOv6211_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, ov6211_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int ov6211_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int ov6211_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (ov6211_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, ov6211_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, ov6211_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (ov6211_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;

}

int ov6211_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (ov6211_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (ov6211_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, ov6211_addr_byte + ov6211_data_byte);
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

void ov6211_standby(VI_PIPE ViPipe)
{
	ov6211_write_register(ViPipe, 0x0100, 0x00); /* standby */
}

void ov6211_restart(VI_PIPE ViPipe)
{
	ov6211_write_register(ViPipe, 0x0100, 0x01); /* restart */
}

void ov6211_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastOv6211[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastOv6211[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			ov6211_write_register(ViPipe,
				g_pastOv6211[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOv6211[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void ov6211_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 flip, mirror;

	flip = ov6211_read_register(ViPipe, 0x3820);
	mirror = ov6211_read_register(ViPipe, 0x3821);

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

	ov6211_write_register(ViPipe, 0x3820, flip);
	ov6211_write_register(ViPipe, 0x3821, mirror);
}

#define OV6211_CHIP_ID_ADDR_H		0x300A
#define OV6211_CHIP_ID_ADDR_L		0x300B
#define OV6211_CHIP_ID			0x6710

int ov6211_probe(VI_PIPE ViPipe)
{
	int nVal, nVal2;

	usleep(500);
	if (ov6211_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = ov6211_read_register(ViPipe, OV6211_CHIP_ID_ADDR_H);
	nVal2 = ov6211_read_register(ViPipe, OV6211_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | ((nVal2 & 0xFF) << 0)) != OV6211_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void ov6211_init(VI_PIPE ViPipe)
{
	ov6211_i2c_init(ViPipe);
	delay_ms(50);
	ov6211_linear_400p120_init(ViPipe);

	g_pastOv6211[ViPipe]->bInit = CVI_TRUE;
}

void ov6211_exit(VI_PIPE ViPipe)
{
	ov6211_i2c_exit(ViPipe);
}

/* 1944P30 and 1944P25 */
static void ov6211_linear_400p120_init(VI_PIPE ViPipe)
{
	ov6211_write_register(ViPipe, 0x0103, 0x01);
	ov6211_write_register(ViPipe, 0x0100, 0x00);
	ov6211_write_register(ViPipe, 0x3005, 0x00);
	ov6211_write_register(ViPipe, 0x3013, 0x12);
	ov6211_write_register(ViPipe, 0x3014, 0x04);
	ov6211_write_register(ViPipe, 0x3016, 0x10);
	ov6211_write_register(ViPipe, 0x3017, 0x00);
	ov6211_write_register(ViPipe, 0x3018, 0x00);
	ov6211_write_register(ViPipe, 0x301a, 0x00);
	ov6211_write_register(ViPipe, 0x301b, 0x00);
	ov6211_write_register(ViPipe, 0x301c, 0x00);
	ov6211_write_register(ViPipe, 0x3037, 0xf0);
	ov6211_write_register(ViPipe, 0x3080, 0x01);
	ov6211_write_register(ViPipe, 0x3081, 0x00);
	ov6211_write_register(ViPipe, 0x3082, 0x01);
	ov6211_write_register(ViPipe, 0x3098, 0x04);
	ov6211_write_register(ViPipe, 0x3099, 0x28);
	ov6211_write_register(ViPipe, 0x309a, 0x06);
	ov6211_write_register(ViPipe, 0x309b, 0x04);
	ov6211_write_register(ViPipe, 0x309c, 0x00);
	ov6211_write_register(ViPipe, 0x309d, 0x00);
	ov6211_write_register(ViPipe, 0x309e, 0x01);
	ov6211_write_register(ViPipe, 0x309f, 0x00);
	ov6211_write_register(ViPipe, 0x30b0, 0x0a);
	ov6211_write_register(ViPipe, 0x30b1, 0x02);
	ov6211_write_register(ViPipe, 0x30b2, 0x00);
	ov6211_write_register(ViPipe, 0x30b3, 0x32);
	ov6211_write_register(ViPipe, 0x30b4, 0x02);
	ov6211_write_register(ViPipe, 0x30b5, 0x05);
	ov6211_write_register(ViPipe, 0x3106, 0xd9);
	ov6211_write_register(ViPipe, 0x3500, 0x00);
	ov6211_write_register(ViPipe, 0x3501, 0x1b);
	ov6211_write_register(ViPipe, 0x3502, 0x20);
	ov6211_write_register(ViPipe, 0x3503, 0x07);
	ov6211_write_register(ViPipe, 0x3509, 0x10);
	ov6211_write_register(ViPipe, 0x350b, 0x10);
	ov6211_write_register(ViPipe, 0x3600, 0xfc);
	ov6211_write_register(ViPipe, 0x3620, 0xb7);
	ov6211_write_register(ViPipe, 0x3621, 0x05);
	ov6211_write_register(ViPipe, 0x3626, 0x31);
	ov6211_write_register(ViPipe, 0x3627, 0x40);
	ov6211_write_register(ViPipe, 0x3632, 0xa3);
	ov6211_write_register(ViPipe, 0x3633, 0x34);
	ov6211_write_register(ViPipe, 0x3634, 0x40);
	ov6211_write_register(ViPipe, 0x3636, 0x00);
	ov6211_write_register(ViPipe, 0x3660, 0x80);
	ov6211_write_register(ViPipe, 0x3662, 0x01);
	ov6211_write_register(ViPipe, 0x3664, 0xf0);
	ov6211_write_register(ViPipe, 0x366a, 0x10);
	ov6211_write_register(ViPipe, 0x366b, 0x06);
	ov6211_write_register(ViPipe, 0x3680, 0xf4);
	ov6211_write_register(ViPipe, 0x3681, 0x50);
	ov6211_write_register(ViPipe, 0x3682, 0x00);
	ov6211_write_register(ViPipe, 0x3708, 0x20);
	ov6211_write_register(ViPipe, 0x3709, 0x40);
	ov6211_write_register(ViPipe, 0x370d, 0x03);
	ov6211_write_register(ViPipe, 0x373b, 0x02);
	ov6211_write_register(ViPipe, 0x373c, 0x08);
	ov6211_write_register(ViPipe, 0x3742, 0x00);
	ov6211_write_register(ViPipe, 0x3744, 0x16);
	ov6211_write_register(ViPipe, 0x3745, 0x08);
	ov6211_write_register(ViPipe, 0x3781, 0xfc);
	ov6211_write_register(ViPipe, 0x3788, 0x00);
	ov6211_write_register(ViPipe, 0x3800, 0x00);
	ov6211_write_register(ViPipe, 0x3801, 0x04);
	ov6211_write_register(ViPipe, 0x3802, 0x00);
	ov6211_write_register(ViPipe, 0x3803, 0x04);
	ov6211_write_register(ViPipe, 0x3804, 0x01);
	ov6211_write_register(ViPipe, 0x3805, 0x9b);
	ov6211_write_register(ViPipe, 0x3806, 0x01);
	ov6211_write_register(ViPipe, 0x3807, 0x9b);
	ov6211_write_register(ViPipe, 0x3808, 0x01);
	ov6211_write_register(ViPipe, 0x3809, 0x90);
	ov6211_write_register(ViPipe, 0x380a, 0x01);
	ov6211_write_register(ViPipe, 0x380b, 0x90);
	ov6211_write_register(ViPipe, 0x380c, 0x05);
	ov6211_write_register(ViPipe, 0x380d, 0xf2);
	ov6211_write_register(ViPipe, 0x380e, 0x01);
	ov6211_write_register(ViPipe, 0x380f, 0xb6);
	ov6211_write_register(ViPipe, 0x3810, 0x00);
	ov6211_write_register(ViPipe, 0x3811, 0x04);
	ov6211_write_register(ViPipe, 0x3812, 0x00);
	ov6211_write_register(ViPipe, 0x3813, 0x04);
	ov6211_write_register(ViPipe, 0x3814, 0x11);
	ov6211_write_register(ViPipe, 0x3815, 0x11);
	ov6211_write_register(ViPipe, 0x3820, 0x00);
	ov6211_write_register(ViPipe, 0x3821, 0x00);
	ov6211_write_register(ViPipe, 0x382b, 0xfa);
	ov6211_write_register(ViPipe, 0x382f, 0x04);
	ov6211_write_register(ViPipe, 0x3832, 0x00);
	ov6211_write_register(ViPipe, 0x3833, 0x05);
	ov6211_write_register(ViPipe, 0x3834, 0x00);
	ov6211_write_register(ViPipe, 0x3835, 0x05);
	ov6211_write_register(ViPipe, 0x3882, 0x04);
	ov6211_write_register(ViPipe, 0x3883, 0x00);
	ov6211_write_register(ViPipe, 0x38a4, 0x10);
	ov6211_write_register(ViPipe, 0x38a5, 0x00);
	ov6211_write_register(ViPipe, 0x38b1, 0x03);
	ov6211_write_register(ViPipe, 0x3b80, 0x00);
	ov6211_write_register(ViPipe, 0x3b81, 0xa5);
	ov6211_write_register(ViPipe, 0x3b82, 0x10);
	ov6211_write_register(ViPipe, 0x3b83, 0x00);
	ov6211_write_register(ViPipe, 0x3b84, 0x08);
	ov6211_write_register(ViPipe, 0x3b85, 0x00);
	ov6211_write_register(ViPipe, 0x3b86, 0x01);
	ov6211_write_register(ViPipe, 0x3b87, 0x00);
	ov6211_write_register(ViPipe, 0x3b88, 0x00);
	ov6211_write_register(ViPipe, 0x3b89, 0x00);
	ov6211_write_register(ViPipe, 0x3b8a, 0x00);
	ov6211_write_register(ViPipe, 0x3b8b, 0x05);
	ov6211_write_register(ViPipe, 0x3b8c, 0x00);
	ov6211_write_register(ViPipe, 0x3b8d, 0x00);
	ov6211_write_register(ViPipe, 0x3b8e, 0x00);
	ov6211_write_register(ViPipe, 0x3b8f, 0x1a);
	ov6211_write_register(ViPipe, 0x3b94, 0x05);
	ov6211_write_register(ViPipe, 0x3b95, 0xf2);
	ov6211_write_register(ViPipe, 0x3b96, 0xf0);
	ov6211_write_register(ViPipe, 0x4004, 0x04);
	ov6211_write_register(ViPipe, 0x404e, 0x01);
	ov6211_write_register(ViPipe, 0x4801, 0x0f);
	ov6211_write_register(ViPipe, 0x4806, 0x0f);

	ov6211_write_register(ViPipe, 0x4818, 0x00);
	ov6211_write_register(ViPipe, 0x4819, 0xaa);
	ov6211_write_register(ViPipe, 0x482a, 0x08);

	ov6211_write_register(ViPipe, 0x481a, 0x00);
	ov6211_write_register(ViPipe, 0x481b, 0x4a);
	ov6211_write_register(ViPipe, 0x482b, 0x08);

	ov6211_write_register(ViPipe, 0x4837, 0x43);
	ov6211_write_register(ViPipe, 0x5a08, 0x00);
	ov6211_write_register(ViPipe, 0x5a01, 0x00);
	ov6211_write_register(ViPipe, 0x5a03, 0x00);
	ov6211_write_register(ViPipe, 0x5a04, 0x10);
	ov6211_write_register(ViPipe, 0x5a05, 0xa0);
	ov6211_write_register(ViPipe, 0x5a06, 0x0c);
	ov6211_write_register(ViPipe, 0x5a07, 0x78);

	ov6211_default_reg_init(ViPipe);

	ov6211_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(100);

	printf("ViPipe:%d,===OV6211 400P 120fps 10bit LINE Init OK!===\n", ViPipe);
}

