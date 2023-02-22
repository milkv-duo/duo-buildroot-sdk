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
#include "sc401ai_cmos_ex.h"

static void sc401ai_linear_1440p30_init(VI_PIPE ViPipe);
static void sc401ai_linear_1296p30_init(VI_PIPE ViPipe);

CVI_U8 sc401ai_i2c_addr = 0x30;        /* I2C Address of SC401AI */
const CVI_U32 sc401ai_addr_byte = 2;
const CVI_U32 sc401ai_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc401ai_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC401AI_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc401ai_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc401ai_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc401ai_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc401ai_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc401ai_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc401ai_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc401ai_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc401ai_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc401ai_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc401ai_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc401ai_addr_byte + sc401ai_data_byte);
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

void sc401ai_standby(VI_PIPE ViPipe)
{
	sc401ai_write_register(ViPipe, 0x0100, 0x00);
}

void sc401ai_restart(VI_PIPE ViPipe)
{
	sc401ai_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc401ai_write_register(ViPipe, 0x0100, 0x01);
}

void sc401ai_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC401AI[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc401ai_write_register(ViPipe,
				g_pastSC401AI[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC401AI[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC401AI_CHIP_ID_HI_ADDR		0x3107
#define SC401AI_CHIP_ID_LO_ADDR		0x3108
#define SC401AI_CHIP_ID			0xcd2e

void sc401ai_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc401ai_write_register(ViPipe, 0x3221, val);
}

int sc401ai_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	delay_ms(4);
	if (sc401ai_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc401ai_read_register(ViPipe, SC401AI_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc401ai_read_register(ViPipe, SC401AI_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC401AI_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc401ai_init(VI_PIPE ViPipe)
{
	CVI_U8 u8ImgMode = g_pastSC401AI[ViPipe]->u8ImgMode;

	sc401ai_i2c_init(ViPipe);

	if (u8ImgMode == SC401AI_MODE_1440P30)
		sc401ai_linear_1440p30_init(ViPipe);
	else if (u8ImgMode == SC401AI_MODE_1296P30)
		sc401ai_linear_1296p30_init(ViPipe);

	g_pastSC401AI[ViPipe]->bInit = CVI_TRUE;
}

void sc401ai_exit(VI_PIPE ViPipe)
{
	sc401ai_i2c_exit(ViPipe);
}

/* 1440P30 and 1440P25 */
static void sc401ai_linear_1440p30_init(VI_PIPE ViPipe)
{
	sc401ai_write_register(ViPipe, 0x0103, 0x01);
	sc401ai_write_register(ViPipe, 0x0100, 0x00);
	sc401ai_write_register(ViPipe, 0x36e9, 0x80);
	sc401ai_write_register(ViPipe, 0x36f9, 0x80);
	sc401ai_write_register(ViPipe, 0x3018, 0x3a);
	sc401ai_write_register(ViPipe, 0x3019, 0x0c);
	sc401ai_write_register(ViPipe, 0x301c, 0x78);
	sc401ai_write_register(ViPipe, 0x301f, 0x05);
	sc401ai_write_register(ViPipe, 0x3208, 0x0a);
	sc401ai_write_register(ViPipe, 0x3209, 0x00);
	sc401ai_write_register(ViPipe, 0x320a, 0x05);
	sc401ai_write_register(ViPipe, 0x320b, 0xa0);
	sc401ai_write_register(ViPipe, 0x320e, 0x05);//vts
	sc401ai_write_register(ViPipe, 0x320f, 0xdc);
	sc401ai_write_register(ViPipe, 0x3214, 0x11);
	sc401ai_write_register(ViPipe, 0x3215, 0x11);
	sc401ai_write_register(ViPipe, 0x3223, 0x80);
	sc401ai_write_register(ViPipe, 0x3250, 0x00);
	sc401ai_write_register(ViPipe, 0x3253, 0x08);
	sc401ai_write_register(ViPipe, 0x3274, 0x01);
	sc401ai_write_register(ViPipe, 0x3301, 0x20);
	sc401ai_write_register(ViPipe, 0x3302, 0x18);
	sc401ai_write_register(ViPipe, 0x3303, 0x10);
	sc401ai_write_register(ViPipe, 0x3304, 0x50);
	sc401ai_write_register(ViPipe, 0x3306, 0x38);
	sc401ai_write_register(ViPipe, 0x3308, 0x18);
	sc401ai_write_register(ViPipe, 0x3309, 0x60);
	sc401ai_write_register(ViPipe, 0x330b, 0xc0);
	sc401ai_write_register(ViPipe, 0x330d, 0x10);
	sc401ai_write_register(ViPipe, 0x330e, 0x18);
	sc401ai_write_register(ViPipe, 0x330f, 0x04);
	sc401ai_write_register(ViPipe, 0x3310, 0x02);
	sc401ai_write_register(ViPipe, 0x331c, 0x04);
	sc401ai_write_register(ViPipe, 0x331e, 0x41);
	sc401ai_write_register(ViPipe, 0x331f, 0x51);
	sc401ai_write_register(ViPipe, 0x3320, 0x09);
	sc401ai_write_register(ViPipe, 0x3333, 0x10);
	sc401ai_write_register(ViPipe, 0x334c, 0x08);
	sc401ai_write_register(ViPipe, 0x3356, 0x09);
	sc401ai_write_register(ViPipe, 0x3364, 0x17);
	sc401ai_write_register(ViPipe, 0x338e, 0xfd);
	sc401ai_write_register(ViPipe, 0x3390, 0x08);
	sc401ai_write_register(ViPipe, 0x3391, 0x18);
	sc401ai_write_register(ViPipe, 0x3392, 0x38);
	sc401ai_write_register(ViPipe, 0x3393, 0x20);
	sc401ai_write_register(ViPipe, 0x3394, 0x20);
	sc401ai_write_register(ViPipe, 0x3395, 0x20);
	sc401ai_write_register(ViPipe, 0x3396, 0x08);
	sc401ai_write_register(ViPipe, 0x3397, 0x18);
	sc401ai_write_register(ViPipe, 0x3398, 0x38);
	sc401ai_write_register(ViPipe, 0x3399, 0x20);
	sc401ai_write_register(ViPipe, 0x339a, 0x20);
	sc401ai_write_register(ViPipe, 0x339b, 0x20);
	sc401ai_write_register(ViPipe, 0x339c, 0x20);
	sc401ai_write_register(ViPipe, 0x33ac, 0x10);
	sc401ai_write_register(ViPipe, 0x33ae, 0x18);
	sc401ai_write_register(ViPipe, 0x33af, 0x19);
	sc401ai_write_register(ViPipe, 0x360f, 0x01);
	sc401ai_write_register(ViPipe, 0x3620, 0x08);
	sc401ai_write_register(ViPipe, 0x3637, 0x25);
	sc401ai_write_register(ViPipe, 0x363a, 0x12);
	sc401ai_write_register(ViPipe, 0x3670, 0x0a);
	sc401ai_write_register(ViPipe, 0x3671, 0x07);
	sc401ai_write_register(ViPipe, 0x3672, 0x57);
	sc401ai_write_register(ViPipe, 0x3673, 0x5e);
	sc401ai_write_register(ViPipe, 0x3674, 0x84);
	sc401ai_write_register(ViPipe, 0x3675, 0x88);
	sc401ai_write_register(ViPipe, 0x3676, 0x8a);
	sc401ai_write_register(ViPipe, 0x367a, 0x58);
	sc401ai_write_register(ViPipe, 0x367b, 0x78);
	sc401ai_write_register(ViPipe, 0x367c, 0x58);
	sc401ai_write_register(ViPipe, 0x367d, 0x78);
	sc401ai_write_register(ViPipe, 0x3690, 0x33);
	sc401ai_write_register(ViPipe, 0x3691, 0x43);
	sc401ai_write_register(ViPipe, 0x3692, 0x34);
	sc401ai_write_register(ViPipe, 0x369c, 0x40);
	sc401ai_write_register(ViPipe, 0x369d, 0x78);
	sc401ai_write_register(ViPipe, 0x36ea, 0x39);
	sc401ai_write_register(ViPipe, 0x36eb, 0x0d);
	sc401ai_write_register(ViPipe, 0x36ec, 0x1c);
	sc401ai_write_register(ViPipe, 0x36ed, 0x24);
	sc401ai_write_register(ViPipe, 0x36fa, 0x39);
	sc401ai_write_register(ViPipe, 0x36fb, 0x33);
	sc401ai_write_register(ViPipe, 0x36fc, 0x10);
	sc401ai_write_register(ViPipe, 0x36fd, 0x14);
	sc401ai_write_register(ViPipe, 0x3908, 0x41);
	sc401ai_write_register(ViPipe, 0x396c, 0x0e);
	sc401ai_write_register(ViPipe, 0x3e00, 0x00);
	sc401ai_write_register(ViPipe, 0x3e01, 0xb6);
	sc401ai_write_register(ViPipe, 0x3e02, 0x00);
	sc401ai_write_register(ViPipe, 0x3e03, 0x0b);
	sc401ai_write_register(ViPipe, 0x3e08, 0x03);
	sc401ai_write_register(ViPipe, 0x3e09, 0x40);
	sc401ai_write_register(ViPipe, 0x3e1b, 0x2a);
	sc401ai_write_register(ViPipe, 0x4509, 0x30);
	sc401ai_write_register(ViPipe, 0x4819, 0x08);
	sc401ai_write_register(ViPipe, 0x481b, 0x05);
	sc401ai_write_register(ViPipe, 0x481d, 0x11);
	sc401ai_write_register(ViPipe, 0x481f, 0x04);
	sc401ai_write_register(ViPipe, 0x4821, 0x09);
	sc401ai_write_register(ViPipe, 0x4823, 0x04);
	sc401ai_write_register(ViPipe, 0x4825, 0x04);
	sc401ai_write_register(ViPipe, 0x4827, 0x04);
	sc401ai_write_register(ViPipe, 0x4829, 0x07);
	sc401ai_write_register(ViPipe, 0x57a8, 0xd0);
	sc401ai_write_register(ViPipe, 0x36e9, 0x14);
	sc401ai_write_register(ViPipe, 0x36f9, 0x14);
	sc401ai_write_register(ViPipe, 0x5001, 0x44);	//Support sid pull up

	sc401ai_default_reg_init(ViPipe);

	sc401ai_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC401AI 1440P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

/* 1440P30 and 1440P25 */
static void sc401ai_linear_1296p30_init(VI_PIPE ViPipe)
{
	sc401ai_write_register(ViPipe, 0x0103, 0x01);
	sc401ai_write_register(ViPipe, 0x0100, 0x00);
	sc401ai_write_register(ViPipe, 0x36e9, 0x80);
	sc401ai_write_register(ViPipe, 0x36f9, 0x80);
	sc401ai_write_register(ViPipe, 0x3018, 0x3a);
	sc401ai_write_register(ViPipe, 0x3019, 0x0c);
	sc401ai_write_register(ViPipe, 0x301c, 0x78);
	sc401ai_write_register(ViPipe, 0x301f, 0x05);
	sc401ai_write_register(ViPipe, 0x3200, 0x00);
	sc401ai_write_register(ViPipe, 0x3201, 0x80);
	sc401ai_write_register(ViPipe, 0x3202, 0x00);
	sc401ai_write_register(ViPipe, 0x3203, 0x48);
	sc401ai_write_register(ViPipe, 0x3204, 0x09);
	sc401ai_write_register(ViPipe, 0x3205, 0x87);
	sc401ai_write_register(ViPipe, 0x3206, 0x05);
	sc401ai_write_register(ViPipe, 0x3207, 0x5f);
	sc401ai_write_register(ViPipe, 0x3208, 0x09);
	sc401ai_write_register(ViPipe, 0x3209, 0x00);
	sc401ai_write_register(ViPipe, 0x320a, 0x05);
	sc401ai_write_register(ViPipe, 0x320b, 0x10);
	sc401ai_write_register(ViPipe, 0x320e, 0x07);
	sc401ai_write_register(ViPipe, 0x320f, 0x08);
	sc401ai_write_register(ViPipe, 0x3210, 0x00);
	sc401ai_write_register(ViPipe, 0x3211, 0x04);
	sc401ai_write_register(ViPipe, 0x3212, 0x00);
	sc401ai_write_register(ViPipe, 0x3213, 0x04);
	sc401ai_write_register(ViPipe, 0x3214, 0x11);
	sc401ai_write_register(ViPipe, 0x3215, 0x11);
	sc401ai_write_register(ViPipe, 0x3223, 0x80);
	sc401ai_write_register(ViPipe, 0x3250, 0x00);
	sc401ai_write_register(ViPipe, 0x3253, 0x08);
	sc401ai_write_register(ViPipe, 0x3274, 0x01);
	sc401ai_write_register(ViPipe, 0x3301, 0x20);
	sc401ai_write_register(ViPipe, 0x3302, 0x18);
	sc401ai_write_register(ViPipe, 0x3303, 0x10);
	sc401ai_write_register(ViPipe, 0x3304, 0x50);
	sc401ai_write_register(ViPipe, 0x3306, 0x38);
	sc401ai_write_register(ViPipe, 0x3308, 0x18);
	sc401ai_write_register(ViPipe, 0x3309, 0x60);
	sc401ai_write_register(ViPipe, 0x330b, 0xc0);
	sc401ai_write_register(ViPipe, 0x330d, 0x10);
	sc401ai_write_register(ViPipe, 0x330e, 0x18);
	sc401ai_write_register(ViPipe, 0x330f, 0x04);
	sc401ai_write_register(ViPipe, 0x3310, 0x02);
	sc401ai_write_register(ViPipe, 0x331c, 0x04);
	sc401ai_write_register(ViPipe, 0x331e, 0x41);
	sc401ai_write_register(ViPipe, 0x331f, 0x51);
	sc401ai_write_register(ViPipe, 0x3320, 0x09);
	sc401ai_write_register(ViPipe, 0x3333, 0x10);
	sc401ai_write_register(ViPipe, 0x334c, 0x08);
	sc401ai_write_register(ViPipe, 0x3356, 0x09);
	sc401ai_write_register(ViPipe, 0x3364, 0x17);
	sc401ai_write_register(ViPipe, 0x338e, 0xfd);
	sc401ai_write_register(ViPipe, 0x3390, 0x08);
	sc401ai_write_register(ViPipe, 0x3391, 0x18);
	sc401ai_write_register(ViPipe, 0x3392, 0x38);
	sc401ai_write_register(ViPipe, 0x3393, 0x20);
	sc401ai_write_register(ViPipe, 0x3394, 0x20);
	sc401ai_write_register(ViPipe, 0x3395, 0x20);
	sc401ai_write_register(ViPipe, 0x3396, 0x08);
	sc401ai_write_register(ViPipe, 0x3397, 0x18);
	sc401ai_write_register(ViPipe, 0x3398, 0x38);
	sc401ai_write_register(ViPipe, 0x3399, 0x20);
	sc401ai_write_register(ViPipe, 0x339a, 0x20);
	sc401ai_write_register(ViPipe, 0x339b, 0x20);
	sc401ai_write_register(ViPipe, 0x339c, 0x20);
	sc401ai_write_register(ViPipe, 0x33ac, 0x10);
	sc401ai_write_register(ViPipe, 0x33ae, 0x18);
	sc401ai_write_register(ViPipe, 0x33af, 0x19);
	sc401ai_write_register(ViPipe, 0x360f, 0x01);
	sc401ai_write_register(ViPipe, 0x3620, 0x08);
	sc401ai_write_register(ViPipe, 0x3637, 0x25);
	sc401ai_write_register(ViPipe, 0x363a, 0x12);
	sc401ai_write_register(ViPipe, 0x3670, 0x0a);
	sc401ai_write_register(ViPipe, 0x3671, 0x07);
	sc401ai_write_register(ViPipe, 0x3672, 0x57);
	sc401ai_write_register(ViPipe, 0x3673, 0x5e);
	sc401ai_write_register(ViPipe, 0x3674, 0x84);
	sc401ai_write_register(ViPipe, 0x3675, 0x88);
	sc401ai_write_register(ViPipe, 0x3676, 0x8a);
	sc401ai_write_register(ViPipe, 0x367a, 0x58);
	sc401ai_write_register(ViPipe, 0x367b, 0x78);
	sc401ai_write_register(ViPipe, 0x367c, 0x58);
	sc401ai_write_register(ViPipe, 0x367d, 0x78);
	sc401ai_write_register(ViPipe, 0x3690, 0x33);
	sc401ai_write_register(ViPipe, 0x3691, 0x43);
	sc401ai_write_register(ViPipe, 0x3692, 0x34);
	sc401ai_write_register(ViPipe, 0x369c, 0x40);
	sc401ai_write_register(ViPipe, 0x369d, 0x78);
	sc401ai_write_register(ViPipe, 0x36ea, 0x39);
	sc401ai_write_register(ViPipe, 0x36eb, 0x0d);
	sc401ai_write_register(ViPipe, 0x36ec, 0x1c);
	sc401ai_write_register(ViPipe, 0x36ed, 0x24);
	sc401ai_write_register(ViPipe, 0x36fa, 0x39);
	sc401ai_write_register(ViPipe, 0x36fb, 0x33);
	sc401ai_write_register(ViPipe, 0x36fc, 0x10);
	sc401ai_write_register(ViPipe, 0x36fd, 0x14);
	sc401ai_write_register(ViPipe, 0x3908, 0x41);
	sc401ai_write_register(ViPipe, 0x396c, 0x0e);
	sc401ai_write_register(ViPipe, 0x3e00, 0x00);
	sc401ai_write_register(ViPipe, 0x3e01, 0xb6);
	sc401ai_write_register(ViPipe, 0x3e02, 0x00);
	sc401ai_write_register(ViPipe, 0x3e03, 0x0b);
	sc401ai_write_register(ViPipe, 0x3e08, 0x03);
	sc401ai_write_register(ViPipe, 0x3e09, 0x40);
	sc401ai_write_register(ViPipe, 0x3e1b, 0x2a);
	sc401ai_write_register(ViPipe, 0x4509, 0x30);
	sc401ai_write_register(ViPipe, 0x4819, 0x08);
	sc401ai_write_register(ViPipe, 0x481b, 0x05);
	sc401ai_write_register(ViPipe, 0x481d, 0x11);
	sc401ai_write_register(ViPipe, 0x481f, 0x04);
	sc401ai_write_register(ViPipe, 0x4821, 0x09);
	sc401ai_write_register(ViPipe, 0x4823, 0x04);
	sc401ai_write_register(ViPipe, 0x4825, 0x04);
	sc401ai_write_register(ViPipe, 0x4827, 0x04);
	sc401ai_write_register(ViPipe, 0x4829, 0x07);
	sc401ai_write_register(ViPipe, 0x57a8, 0xd0);
	sc401ai_write_register(ViPipe, 0x36e9, 0x14);
	sc401ai_write_register(ViPipe, 0x36f9, 0x14);
	sc401ai_write_register(ViPipe, 0x5001, 0x44);	//Support sid pull up

	sc401ai_default_reg_init(ViPipe);

	sc401ai_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC401AI 1296P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
