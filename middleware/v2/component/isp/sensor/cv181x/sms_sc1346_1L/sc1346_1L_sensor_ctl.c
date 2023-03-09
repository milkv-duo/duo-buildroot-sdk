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
#include "sc1346_1L_cmos_ex.h"

static void sc1346_1L_linear_720p30_init(VI_PIPE ViPipe);
static void sc1346_1L_linear_720p60_init(VI_PIPE ViPipe);

const CVI_U8 sc1346_1L_i2c_addr = 0x30;        /* I2C Address of SC1346_1L */
const CVI_U32 sc1346_1L_addr_byte = 2;
const CVI_U32 sc1346_1L_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc1346_1L_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC1346_1L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc1346_1L_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc1346_1L_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc1346_1L_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	char buf[8];
	int idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc1346_1L_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc1346_1L_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc1346_1L_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc1346_1L_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc1346_1L_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc1346_1L_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc1346_1L_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc1346_1L_addr_byte + sc1346_1L_data_byte);
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

void sc1346_1L_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			sc1346_1L_write_register(ViPipe, addr, data);
	}
}

void sc1346_1L_standby(VI_PIPE ViPipe)
{
	sc1346_1L_write_register(ViPipe, 0x0100, 0x00);
}

void sc1346_1L_restart(VI_PIPE ViPipe)
{
	sc1346_1L_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc1346_1L_write_register(ViPipe, 0x0100, 0x01);
}

void sc1346_1L_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC1346_1L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc1346_1L_write_register(ViPipe,
				g_pastSC1346_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC1346_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC1346_1L_CHIP_ID_HI_ADDR		0x3107
#define SC1346_1L_CHIP_ID_LO_ADDR		0x3108
#define SC1346_1L_CHIP_ID				0xda4d

int sc1346_1L_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc1346_1L_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc1346_1L_read_register(ViPipe, SC1346_1L_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc1346_1L_read_register(ViPipe, SC1346_1L_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC1346_1L_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc1346_1L_init(VI_PIPE ViPipe)
{
	CVI_U8 u8ImgMode = g_pastSC1346_1L[ViPipe]->u8ImgMode;

	sc1346_1L_i2c_init(ViPipe);

	if ((u8ImgMode == SC1346_1L_MODE_720P30) || (u8ImgMode == SC1346_1L_MODE_720P30_WDR))
		sc1346_1L_linear_720p30_init(ViPipe);
	else if ((u8ImgMode == SC1346_1L_MODE_720P60) || (u8ImgMode == SC1346_1L_MODE_720P60_WDR))
		sc1346_1L_linear_720p60_init(ViPipe);

	g_pastSC1346_1L[ViPipe]->bInit = CVI_TRUE;
}

void sc1346_1L_exit(VI_PIPE ViPipe)
{
	sc1346_1L_i2c_exit(ViPipe);
}

/* 720P30 */
static void sc1346_1L_linear_720p30_init(VI_PIPE ViPipe)
{
	sc1346_1L_write_register(ViPipe, 0x0103, 0x01);
	sc1346_1L_write_register(ViPipe, 0x0100, 0x00);
	sc1346_1L_write_register(ViPipe, 0x36e9, 0x80);
	sc1346_1L_write_register(ViPipe, 0x37f9, 0x80);
	sc1346_1L_write_register(ViPipe, 0x301f, 0x01);
	sc1346_1L_write_register(ViPipe, 0x3106, 0x05);
	sc1346_1L_write_register(ViPipe, 0x3301, 0x06);
	sc1346_1L_write_register(ViPipe, 0x3306, 0x50);
	sc1346_1L_write_register(ViPipe, 0x3308, 0x0a);
	sc1346_1L_write_register(ViPipe, 0x330a, 0x00);
	sc1346_1L_write_register(ViPipe, 0x330b, 0xda);
	sc1346_1L_write_register(ViPipe, 0x330e, 0x0a);
	sc1346_1L_write_register(ViPipe, 0x331e, 0x61);
	sc1346_1L_write_register(ViPipe, 0x331f, 0xa1);
	sc1346_1L_write_register(ViPipe, 0x3364, 0x1f);
	sc1346_1L_write_register(ViPipe, 0x3390, 0x09);
	sc1346_1L_write_register(ViPipe, 0x3391, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x3392, 0x1f);
	sc1346_1L_write_register(ViPipe, 0x3393, 0x30);
	sc1346_1L_write_register(ViPipe, 0x3394, 0x30);
	sc1346_1L_write_register(ViPipe, 0x3395, 0x30);
	sc1346_1L_write_register(ViPipe, 0x33ad, 0x10);
	sc1346_1L_write_register(ViPipe, 0x33b3, 0x40);
	sc1346_1L_write_register(ViPipe, 0x33f9, 0x50);
	sc1346_1L_write_register(ViPipe, 0x33fb, 0x80);
	sc1346_1L_write_register(ViPipe, 0x33fc, 0x09);
	sc1346_1L_write_register(ViPipe, 0x33fd, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x349f, 0x03);
	sc1346_1L_write_register(ViPipe, 0x34a6, 0x09);
	sc1346_1L_write_register(ViPipe, 0x34a7, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x34a8, 0x40);
	sc1346_1L_write_register(ViPipe, 0x34a9, 0x30);
	sc1346_1L_write_register(ViPipe, 0x34aa, 0x00);
	sc1346_1L_write_register(ViPipe, 0x34ab, 0xe8);
	sc1346_1L_write_register(ViPipe, 0x34ac, 0x01);
	sc1346_1L_write_register(ViPipe, 0x34ad, 0x0c);
	sc1346_1L_write_register(ViPipe, 0x3630, 0xe2);
	sc1346_1L_write_register(ViPipe, 0x3632, 0x76);
	sc1346_1L_write_register(ViPipe, 0x3633, 0x33);
	sc1346_1L_write_register(ViPipe, 0x3639, 0xf4);
	sc1346_1L_write_register(ViPipe, 0x3641, 0x00);
	sc1346_1L_write_register(ViPipe, 0x3670, 0x09);
	sc1346_1L_write_register(ViPipe, 0x3674, 0xe2);
	sc1346_1L_write_register(ViPipe, 0x3675, 0xea);
	sc1346_1L_write_register(ViPipe, 0x3676, 0xea);
	sc1346_1L_write_register(ViPipe, 0x367c, 0x09);
	sc1346_1L_write_register(ViPipe, 0x367d, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x3690, 0x22);
	sc1346_1L_write_register(ViPipe, 0x3691, 0x22);
	sc1346_1L_write_register(ViPipe, 0x3692, 0x22);
	sc1346_1L_write_register(ViPipe, 0x3698, 0x88);
	sc1346_1L_write_register(ViPipe, 0x3699, 0x90);
	sc1346_1L_write_register(ViPipe, 0x369a, 0xa1);
	sc1346_1L_write_register(ViPipe, 0x369b, 0xc3);
	sc1346_1L_write_register(ViPipe, 0x369c, 0x09);
	sc1346_1L_write_register(ViPipe, 0x369d, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x36a2, 0x09);
	sc1346_1L_write_register(ViPipe, 0x36a3, 0x0b);
	sc1346_1L_write_register(ViPipe, 0x36a4, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x36d0, 0x01);
	sc1346_1L_write_register(ViPipe, 0x370f, 0x01);
	sc1346_1L_write_register(ViPipe, 0x3722, 0x41);
	sc1346_1L_write_register(ViPipe, 0x3724, 0x41);
	sc1346_1L_write_register(ViPipe, 0x3725, 0xc1);
	sc1346_1L_write_register(ViPipe, 0x3728, 0x00);
	sc1346_1L_write_register(ViPipe, 0x37b0, 0x41);
	sc1346_1L_write_register(ViPipe, 0x37b1, 0x41);
	sc1346_1L_write_register(ViPipe, 0x37b2, 0x47);
	sc1346_1L_write_register(ViPipe, 0x37b3, 0x09);
	sc1346_1L_write_register(ViPipe, 0x37b4, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x3903, 0x40);
	sc1346_1L_write_register(ViPipe, 0x3904, 0x04);
	sc1346_1L_write_register(ViPipe, 0x3905, 0x8d);
	sc1346_1L_write_register(ViPipe, 0x3907, 0x00);
	sc1346_1L_write_register(ViPipe, 0x3908, 0x41);
	sc1346_1L_write_register(ViPipe, 0x391f, 0x41);
	sc1346_1L_write_register(ViPipe, 0x3933, 0x80);
	sc1346_1L_write_register(ViPipe, 0x3934, 0x02);
	sc1346_1L_write_register(ViPipe, 0x3937, 0x74);
	sc1346_1L_write_register(ViPipe, 0x3939, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x393a, 0xd4);
	sc1346_1L_write_register(ViPipe, 0x3e01, 0x2e);
	sc1346_1L_write_register(ViPipe, 0x3e02, 0xa0);
	sc1346_1L_write_register(ViPipe, 0x440e, 0x02);
	sc1346_1L_write_register(ViPipe, 0x4509, 0x20);
	sc1346_1L_write_register(ViPipe, 0x450d, 0x28);
	sc1346_1L_write_register(ViPipe, 0x5780, 0x66);
	sc1346_1L_write_register(ViPipe, 0x578d, 0x40);
	sc1346_1L_write_register(ViPipe, 0x36e9, 0x20);
	sc1346_1L_write_register(ViPipe, 0x37f9, 0x20);
	sc1346_1L_write_register(ViPipe, 0x0100, 0x01);

	sc1346_1L_default_reg_init(ViPipe);

	sc1346_1L_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC1346_1L 720P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

/* 720P60 */
static void sc1346_1L_linear_720p60_init(VI_PIPE ViPipe)
{
	sc1346_1L_write_register(ViPipe, 0x0103, 0x01);
	sc1346_1L_write_register(ViPipe, 0x0100, 0x00);
	sc1346_1L_write_register(ViPipe, 0x36e9, 0x80);
	sc1346_1L_write_register(ViPipe, 0x37f9, 0x80);
	sc1346_1L_write_register(ViPipe, 0x301f, 0x02);
	sc1346_1L_write_register(ViPipe, 0x3106, 0x05);
	sc1346_1L_write_register(ViPipe, 0x3301, 0x0b);
	sc1346_1L_write_register(ViPipe, 0x3303, 0x10);
	sc1346_1L_write_register(ViPipe, 0x3306, 0x50);
	sc1346_1L_write_register(ViPipe, 0x3308, 0x0a);
	sc1346_1L_write_register(ViPipe, 0x330a, 0x00);
	sc1346_1L_write_register(ViPipe, 0x330b, 0xda);
	sc1346_1L_write_register(ViPipe, 0x330e, 0x0a);
	sc1346_1L_write_register(ViPipe, 0x331e, 0x61);
	sc1346_1L_write_register(ViPipe, 0x331f, 0xa1);
	sc1346_1L_write_register(ViPipe, 0x3320, 0x04);
	sc1346_1L_write_register(ViPipe, 0x3327, 0x08);
	sc1346_1L_write_register(ViPipe, 0x3329, 0x09);
	sc1346_1L_write_register(ViPipe, 0x3364, 0x1f);
	sc1346_1L_write_register(ViPipe, 0x3390, 0x09);
	sc1346_1L_write_register(ViPipe, 0x3391, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x3392, 0x1f);
	sc1346_1L_write_register(ViPipe, 0x3393, 0x30);
	sc1346_1L_write_register(ViPipe, 0x3394, 0xff);
	sc1346_1L_write_register(ViPipe, 0x3395, 0xff);
	sc1346_1L_write_register(ViPipe, 0x33ad, 0x10);
	sc1346_1L_write_register(ViPipe, 0x33b3, 0x40);
	sc1346_1L_write_register(ViPipe, 0x33f9, 0x50);
	sc1346_1L_write_register(ViPipe, 0x33fb, 0x80);
	sc1346_1L_write_register(ViPipe, 0x33fc, 0x09);
	sc1346_1L_write_register(ViPipe, 0x33fd, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x349f, 0x03);
	sc1346_1L_write_register(ViPipe, 0x34a6, 0x09);
	sc1346_1L_write_register(ViPipe, 0x34a7, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x34a8, 0x40);
	sc1346_1L_write_register(ViPipe, 0x34a9, 0x30);
	sc1346_1L_write_register(ViPipe, 0x34aa, 0x00);
	sc1346_1L_write_register(ViPipe, 0x34ab, 0xe8);
	sc1346_1L_write_register(ViPipe, 0x34ac, 0x01);
	sc1346_1L_write_register(ViPipe, 0x34ad, 0x0c);
	sc1346_1L_write_register(ViPipe, 0x3630, 0xe2);
	sc1346_1L_write_register(ViPipe, 0x3632, 0x76);
	sc1346_1L_write_register(ViPipe, 0x3633, 0x33);
	sc1346_1L_write_register(ViPipe, 0x3639, 0xf4);
	sc1346_1L_write_register(ViPipe, 0x3641, 0x28);
	sc1346_1L_write_register(ViPipe, 0x3670, 0x09);
	sc1346_1L_write_register(ViPipe, 0x3674, 0xe2);
	sc1346_1L_write_register(ViPipe, 0x3675, 0xea);
	sc1346_1L_write_register(ViPipe, 0x3676, 0xea);
	sc1346_1L_write_register(ViPipe, 0x367c, 0x09);
	sc1346_1L_write_register(ViPipe, 0x367d, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x3690, 0x22);
	sc1346_1L_write_register(ViPipe, 0x3691, 0x22);
	sc1346_1L_write_register(ViPipe, 0x3692, 0x32);
	sc1346_1L_write_register(ViPipe, 0x3698, 0x88);
	sc1346_1L_write_register(ViPipe, 0x3699, 0x8f);
	sc1346_1L_write_register(ViPipe, 0x369a, 0xa0);
	sc1346_1L_write_register(ViPipe, 0x369b, 0xd1);
	sc1346_1L_write_register(ViPipe, 0x369c, 0x09);
	sc1346_1L_write_register(ViPipe, 0x369d, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x36a2, 0x09);
	sc1346_1L_write_register(ViPipe, 0x36a3, 0x0b);
	sc1346_1L_write_register(ViPipe, 0x36a4, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x36d0, 0x01);
	sc1346_1L_write_register(ViPipe, 0x36eb, 0x05);
	sc1346_1L_write_register(ViPipe, 0x36ec, 0x05);
	sc1346_1L_write_register(ViPipe, 0x370f, 0x01);
	sc1346_1L_write_register(ViPipe, 0x3722, 0x41);
	sc1346_1L_write_register(ViPipe, 0x3724, 0x41);
	sc1346_1L_write_register(ViPipe, 0x3725, 0xc1);
	sc1346_1L_write_register(ViPipe, 0x3727, 0x14);
	sc1346_1L_write_register(ViPipe, 0x3728, 0x00);
	sc1346_1L_write_register(ViPipe, 0x37b0, 0x21);
	sc1346_1L_write_register(ViPipe, 0x37b1, 0x21);
	sc1346_1L_write_register(ViPipe, 0x37b2, 0x37);
	sc1346_1L_write_register(ViPipe, 0x37b3, 0x09);
	sc1346_1L_write_register(ViPipe, 0x37b4, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x37fb, 0x31);
	sc1346_1L_write_register(ViPipe, 0x37fc, 0x01);
	sc1346_1L_write_register(ViPipe, 0x3903, 0x40);
	sc1346_1L_write_register(ViPipe, 0x3904, 0x04);
	sc1346_1L_write_register(ViPipe, 0x3905, 0x8d);
	sc1346_1L_write_register(ViPipe, 0x3907, 0x00);
	sc1346_1L_write_register(ViPipe, 0x3908, 0x41);
	sc1346_1L_write_register(ViPipe, 0x391f, 0x41);
	sc1346_1L_write_register(ViPipe, 0x3933, 0x80);
	sc1346_1L_write_register(ViPipe, 0x3934, 0x0a);
	sc1346_1L_write_register(ViPipe, 0x3935, 0x01);
	sc1346_1L_write_register(ViPipe, 0x3936, 0x55);
	sc1346_1L_write_register(ViPipe, 0x3937, 0x71);
	sc1346_1L_write_register(ViPipe, 0x3938, 0x72);
	sc1346_1L_write_register(ViPipe, 0x3939, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x393a, 0xef);
	sc1346_1L_write_register(ViPipe, 0x393b, 0x0f);
	sc1346_1L_write_register(ViPipe, 0x393c, 0xcd);
	sc1346_1L_write_register(ViPipe, 0x3e01, 0x2e);
	sc1346_1L_write_register(ViPipe, 0x3e02, 0x80);
	sc1346_1L_write_register(ViPipe, 0x440e, 0x02);
	sc1346_1L_write_register(ViPipe, 0x4509, 0x25);
	sc1346_1L_write_register(ViPipe, 0x450d, 0x28);
	sc1346_1L_write_register(ViPipe, 0x4819, 0x09);
	sc1346_1L_write_register(ViPipe, 0x481b, 0x05);
	sc1346_1L_write_register(ViPipe, 0x481d, 0x14);
	sc1346_1L_write_register(ViPipe, 0x481f, 0x04);
	sc1346_1L_write_register(ViPipe, 0x4821, 0x0a);
	sc1346_1L_write_register(ViPipe, 0x4823, 0x05);
	sc1346_1L_write_register(ViPipe, 0x4825, 0x04);
	sc1346_1L_write_register(ViPipe, 0x4827, 0x05);
	sc1346_1L_write_register(ViPipe, 0x4829, 0x08);
	sc1346_1L_write_register(ViPipe, 0x5780, 0x66);
	sc1346_1L_write_register(ViPipe, 0x578d, 0x40);
	sc1346_1L_write_register(ViPipe, 0x36e9, 0x20);
	sc1346_1L_write_register(ViPipe, 0x37f9, 0x20);
	sc1346_1L_write_register(ViPipe, 0x0100, 0x01);

	sc1346_1L_default_reg_init(ViPipe);

	sc1346_1L_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC1346_1L 720P 60fps 10bit LINE Init OK!===\n", ViPipe);

}