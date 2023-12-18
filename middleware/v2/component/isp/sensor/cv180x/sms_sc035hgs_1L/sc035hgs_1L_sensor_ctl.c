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
#include "sc035hgs_1L_cmos_ex.h"

static void sc035hgs_1L_linear_1296P30_init(VI_PIPE ViPipe);

const CVI_U8 sc035hgs_1L_i2c_addr = 0x32;        /* I2C Address of SC035HGS_1L */
const CVI_U32 sc035hgs_1L_addr_byte = 2;
const CVI_U32 sc035hgs_1L_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc035hgs_1L_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC035HGS_1L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc035hgs_1L_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc035hgs_1L_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc035hgs_1L_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc035hgs_1L_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc035hgs_1L_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc035hgs_1L_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc035hgs_1L_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}


int sc035hgs_1L_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc035hgs_1L_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc035hgs_1L_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc035hgs_1L_addr_byte + sc035hgs_1L_data_byte);
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

void sc035hgs_1L_standby(VI_PIPE ViPipe)
{
	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x00);
}

void sc035hgs_1L_restart(VI_PIPE ViPipe)
{
	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x01);
}

void sc035hgs_1L_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC035HGS_1L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastSC035HGS_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			sc035hgs_1L_write_register(ViPipe,
				g_pastSC035HGS_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC035HGS_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void sc035hgs_1L_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 val = 0;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		val |= 0x6;
		break;
	case ISP_SNS_FLIP:
		val |= 0x60;
		break;
	case ISP_SNS_MIRROR_FLIP:
		val |= 0x66;
		break;
	default:
		return;
	}

	sc035hgs_1L_write_register(ViPipe, 0x3221, val);
}

void sc035hgs_1L_init(VI_PIPE ViPipe)
{
	sc035hgs_1L_i2c_init(ViPipe);

	//linear mode only
	sc035hgs_1L_linear_1296P30_init(ViPipe);

	g_pastSC035HGS_1L[ViPipe]->bInit = CVI_TRUE;
}

void sc035hgs_1L_exit(VI_PIPE ViPipe)
{
	sc035hgs_1L_i2c_exit(ViPipe);
}

/* 1296P30 and 1296P25 */
static void sc035hgs_1L_linear_1296P30_init(VI_PIPE ViPipe)
{
	sc035hgs_1L_write_register(ViPipe, 0x0103, 0x01);
	delay_ms(33);
	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x36e9, 0x80);
	sc035hgs_1L_write_register(ViPipe, 0x36f9, 0x80);
	sc035hgs_1L_write_register(ViPipe, 0x3000, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3001, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x300f, 0x0f);
	sc035hgs_1L_write_register(ViPipe, 0x3018, 0x13);
	sc035hgs_1L_write_register(ViPipe, 0x3019, 0xfe);
	sc035hgs_1L_write_register(ViPipe, 0x301c, 0x78);
	sc035hgs_1L_write_register(ViPipe, 0x301f, 0x62);
	sc035hgs_1L_write_register(ViPipe, 0x3031, 0x0a);
	sc035hgs_1L_write_register(ViPipe, 0x3037, 0x20);
	sc035hgs_1L_write_register(ViPipe, 0x303f, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x320c, 0x04);
	sc035hgs_1L_write_register(ViPipe, 0x320d, 0x70);
	sc035hgs_1L_write_register(ViPipe, 0x320e, 0x02);
	sc035hgs_1L_write_register(ViPipe, 0x320f, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x3217, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3218, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3220, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x3223, 0x48);
	sc035hgs_1L_write_register(ViPipe, 0x3226, 0x74);
	sc035hgs_1L_write_register(ViPipe, 0x3227, 0x07);
	sc035hgs_1L_write_register(ViPipe, 0x323b, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3250, 0xf0);
	sc035hgs_1L_write_register(ViPipe, 0x3251, 0x02);	// 0x1
	sc035hgs_1L_write_register(ViPipe, 0x3252, 0x02);	// 0xf8
	sc035hgs_1L_write_register(ViPipe, 0x3253, 0x08);
	sc035hgs_1L_write_register(ViPipe, 0x3254, 0x02);
	sc035hgs_1L_write_register(ViPipe, 0x3255, 0x07);
	sc035hgs_1L_write_register(ViPipe, 0x3304, 0x48);
	sc035hgs_1L_write_register(ViPipe, 0x3305, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3306, 0x98);
	sc035hgs_1L_write_register(ViPipe, 0x3309, 0x50);
	sc035hgs_1L_write_register(ViPipe, 0x330a, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x330b, 0x18);
	sc035hgs_1L_write_register(ViPipe, 0x330c, 0x18);
	sc035hgs_1L_write_register(ViPipe, 0x330f, 0x40);
	sc035hgs_1L_write_register(ViPipe, 0x3310, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x3314, 0x6b);
	sc035hgs_1L_write_register(ViPipe, 0x3315, 0x30);
	sc035hgs_1L_write_register(ViPipe, 0x3316, 0x68);
	sc035hgs_1L_write_register(ViPipe, 0x3317, 0x14);
	sc035hgs_1L_write_register(ViPipe, 0x3329, 0x5c);
	sc035hgs_1L_write_register(ViPipe, 0x332d, 0x5c);
	sc035hgs_1L_write_register(ViPipe, 0x332f, 0x60);
	sc035hgs_1L_write_register(ViPipe, 0x3335, 0x64);
	sc035hgs_1L_write_register(ViPipe, 0x3344, 0x64);
	sc035hgs_1L_write_register(ViPipe, 0x335b, 0x80);
	sc035hgs_1L_write_register(ViPipe, 0x335f, 0x80);
	sc035hgs_1L_write_register(ViPipe, 0x3366, 0x06);
	sc035hgs_1L_write_register(ViPipe, 0x3385, 0x31);
	sc035hgs_1L_write_register(ViPipe, 0x3387, 0x39);
	sc035hgs_1L_write_register(ViPipe, 0x3389, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x33b1, 0x03);
	sc035hgs_1L_write_register(ViPipe, 0x33b2, 0x06);
	sc035hgs_1L_write_register(ViPipe, 0x33bd, 0xe0);
	sc035hgs_1L_write_register(ViPipe, 0x33bf, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x3621, 0xa4);
	sc035hgs_1L_write_register(ViPipe, 0x3622, 0x05);
	sc035hgs_1L_write_register(ViPipe, 0x3624, 0x47);
	sc035hgs_1L_write_register(ViPipe, 0x3630, 0x4a);
	sc035hgs_1L_write_register(ViPipe, 0x3631, 0x58);
	sc035hgs_1L_write_register(ViPipe, 0x3633, 0x52);
	sc035hgs_1L_write_register(ViPipe, 0x3635, 0x03);
	sc035hgs_1L_write_register(ViPipe, 0x3636, 0x25);
	sc035hgs_1L_write_register(ViPipe, 0x3637, 0x8a);
	sc035hgs_1L_write_register(ViPipe, 0x3638, 0x0f);
	sc035hgs_1L_write_register(ViPipe, 0x3639, 0x08);
	sc035hgs_1L_write_register(ViPipe, 0x363a, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x363b, 0x48);
	sc035hgs_1L_write_register(ViPipe, 0x363c, 0x86);
	sc035hgs_1L_write_register(ViPipe, 0x363d, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x363e, 0xf8);
	sc035hgs_1L_write_register(ViPipe, 0x3640, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3641, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x36ea, 0x36);
	sc035hgs_1L_write_register(ViPipe, 0x36eb, 0x0e);
	sc035hgs_1L_write_register(ViPipe, 0x36ec, 0x0e);
	sc035hgs_1L_write_register(ViPipe, 0x36ed, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x36fa, 0x36);
	sc035hgs_1L_write_register(ViPipe, 0x36fb, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x36fc, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x36fd, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x3908, 0x91);
	sc035hgs_1L_write_register(ViPipe, 0x391b, 0x81);
	sc035hgs_1L_write_register(ViPipe, 0x3d08, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x3e01, 0x18);
	sc035hgs_1L_write_register(ViPipe, 0x3e02, 0xf0);
	sc035hgs_1L_write_register(ViPipe, 0x3e03, 0x2b);
	sc035hgs_1L_write_register(ViPipe, 0x3e06, 0x0c);
	sc035hgs_1L_write_register(ViPipe, 0x3f04, 0x03);
	sc035hgs_1L_write_register(ViPipe, 0x3f05, 0x80);
	sc035hgs_1L_write_register(ViPipe, 0x4500, 0x59);
	sc035hgs_1L_write_register(ViPipe, 0x4501, 0xc4);
	sc035hgs_1L_write_register(ViPipe, 0x4603, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x4800, 0x64);
	sc035hgs_1L_write_register(ViPipe, 0x4809, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x4810, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x4811, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x4837, 0x1c);
	sc035hgs_1L_write_register(ViPipe, 0x5011, 0x00);
	sc035hgs_1L_write_register(ViPipe, 0x5988, 0x02);
	sc035hgs_1L_write_register(ViPipe, 0x598e, 0x04);
	sc035hgs_1L_write_register(ViPipe, 0x598f, 0x4e);
	sc035hgs_1L_write_register(ViPipe, 0x36e9, 0x44);
	sc035hgs_1L_write_register(ViPipe, 0x36f9, 0x44);
	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x01);
	sc035hgs_1L_write_register(ViPipe, 0x4418, 0x0a);
	sc035hgs_1L_write_register(ViPipe, 0x4419, 0x80);

	sc035hgs_1L_default_reg_init(ViPipe);

	sc035hgs_1L_write_register(ViPipe, 0x0100, 0x01);
	delay_ms(7);
	sc035hgs_1L_write_register(ViPipe, 0x363d, 0x10);
	sc035hgs_1L_write_register(ViPipe, 0x4418, 0x0a);
	sc035hgs_1L_write_register(ViPipe, 0x4419, 0x80);

	delay_ms(100);

	printf("ViPipe:%d,===SC035HGS_1L 480P 120fps 12bit LINE Init OK!===3\n", ViPipe);
}
