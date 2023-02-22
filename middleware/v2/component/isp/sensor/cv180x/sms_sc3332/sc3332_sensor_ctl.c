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
#include "sc3332_cmos_ex.h"

#define SC3332_CHIP_ID_HI_ADDR		0x3107
#define SC3332_CHIP_ID_LO_ADDR		0x3108
#define SC3332_CHIP_ID			0xcc44

static void sc3332_linear_1296P30_init(VI_PIPE ViPipe);

CVI_U8 sc3332_i2c_addr = 0x30;        /* I2C Address of SC3332 */
const CVI_U32 sc3332_addr_byte = 2;
const CVI_U32 sc3332_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc3332_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC3332_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc3332_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc3332_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc3332_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc3332_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc3332_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc3332_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc3332_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc3332_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc3332_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc3332_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc3332_addr_byte + sc3332_data_byte);
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

void sc3332_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			sc3332_write_register(ViPipe, addr, data);
	}
}

void sc3332_standby(VI_PIPE ViPipe)
{
	sc3332_write_register(ViPipe, 0x0100, 0x00);
}

void sc3332_restart(VI_PIPE ViPipe)
{
	sc3332_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc3332_write_register(ViPipe, 0x0100, 0x01);
}

void sc3332_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC3332[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastSC3332[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			sc3332_write_register(ViPipe,
				g_pastSC3332[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC3332[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void sc3332_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc3332_write_register(ViPipe, 0x3221, val);
}

int sc3332_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc3332_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc3332_read_register(ViPipe, SC3332_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc3332_read_register(ViPipe, SC3332_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC3332_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void sc3332_init(VI_PIPE ViPipe)
{
	sc3332_i2c_init(ViPipe);

	//linear mode only
	sc3332_linear_1296P30_init(ViPipe);

	g_pastSC3332[ViPipe]->bInit = CVI_TRUE;
}

void sc3332_exit(VI_PIPE ViPipe)
{
	sc3332_i2c_exit(ViPipe);
}

/* 1296P30 and 1296P25 */
static void sc3332_linear_1296P30_init(VI_PIPE ViPipe)
{
	sc3332_write_register(ViPipe, 0x0103, 0x01);
	sc3332_write_register(ViPipe, 0x36e9, 0x80);
	sc3332_write_register(ViPipe, 0x37f9, 0x80);
	sc3332_write_register(ViPipe, 0x301f, 0x01);
	sc3332_write_register(ViPipe, 0x30b8, 0x44);
	sc3332_write_register(ViPipe, 0x3253, 0x10);
	sc3332_write_register(ViPipe, 0x3301, 0x08);
	sc3332_write_register(ViPipe, 0x3302, 0xff);
	sc3332_write_register(ViPipe, 0x3305, 0x00);
	sc3332_write_register(ViPipe, 0x3306, 0x90);
	sc3332_write_register(ViPipe, 0x3308, 0x18);
	sc3332_write_register(ViPipe, 0x330a, 0x01);
	sc3332_write_register(ViPipe, 0x330b, 0xc0);
	sc3332_write_register(ViPipe, 0x330d, 0x70);
	sc3332_write_register(ViPipe, 0x330e, 0x30);
	sc3332_write_register(ViPipe, 0x3314, 0x15);
	sc3332_write_register(ViPipe, 0x3333, 0x10);
	sc3332_write_register(ViPipe, 0x3334, 0x40);
	sc3332_write_register(ViPipe, 0x335e, 0x06);
	sc3332_write_register(ViPipe, 0x335f, 0x0a);
	sc3332_write_register(ViPipe, 0x3364, 0x5e);
	sc3332_write_register(ViPipe, 0x337d, 0x0e);
	sc3332_write_register(ViPipe, 0x3390, 0x08);
	sc3332_write_register(ViPipe, 0x3391, 0x09);
	sc3332_write_register(ViPipe, 0x3392, 0x0f);
	sc3332_write_register(ViPipe, 0x3393, 0x10);
	sc3332_write_register(ViPipe, 0x3394, 0x80);
	sc3332_write_register(ViPipe, 0x3395, 0xff);
	sc3332_write_register(ViPipe, 0x33a2, 0x04);
	sc3332_write_register(ViPipe, 0x33ad, 0x2c);
	sc3332_write_register(ViPipe, 0x33b3, 0x48);
	sc3332_write_register(ViPipe, 0x33f8, 0x00);
	sc3332_write_register(ViPipe, 0x33f9, 0xc0);
	sc3332_write_register(ViPipe, 0x33fa, 0x00);
	sc3332_write_register(ViPipe, 0x33fb, 0xf0);
	sc3332_write_register(ViPipe, 0x33fc, 0x0b);
	sc3332_write_register(ViPipe, 0x33fd, 0x0f);
	sc3332_write_register(ViPipe, 0x349f, 0x03);
	sc3332_write_register(ViPipe, 0x34a6, 0x0b);
	sc3332_write_register(ViPipe, 0x34a7, 0x0f);
	sc3332_write_register(ViPipe, 0x34a8, 0x40);
	sc3332_write_register(ViPipe, 0x34a9, 0x30);
	sc3332_write_register(ViPipe, 0x34aa, 0x01);
	sc3332_write_register(ViPipe, 0x34ab, 0xf0);
	sc3332_write_register(ViPipe, 0x34ac, 0x02);
	sc3332_write_register(ViPipe, 0x34ad, 0x10);
	sc3332_write_register(ViPipe, 0x34f8, 0x1f);
	sc3332_write_register(ViPipe, 0x34f9, 0x30);
	sc3332_write_register(ViPipe, 0x3630, 0xf0);
	sc3332_write_register(ViPipe, 0x3631, 0x8c);
	sc3332_write_register(ViPipe, 0x3632, 0x78);
	sc3332_write_register(ViPipe, 0x3633, 0x33);
	sc3332_write_register(ViPipe, 0x363a, 0xcc);
	sc3332_write_register(ViPipe, 0x363c, 0x0f);
	sc3332_write_register(ViPipe, 0x363f, 0xc0);
	sc3332_write_register(ViPipe, 0x3641, 0x00);
	sc3332_write_register(ViPipe, 0x3650, 0x33); // LP 0x33
	sc3332_write_register(ViPipe, 0x3670, 0x5e);
	sc3332_write_register(ViPipe, 0x3674, 0xf0);
	sc3332_write_register(ViPipe, 0x3675, 0xf0);
	sc3332_write_register(ViPipe, 0x3676, 0xd0);
	sc3332_write_register(ViPipe, 0x3677, 0x87);
	sc3332_write_register(ViPipe, 0x3678, 0x8a);
	sc3332_write_register(ViPipe, 0x3679, 0x8d);
	sc3332_write_register(ViPipe, 0x367c, 0x08);
	sc3332_write_register(ViPipe, 0x367d, 0x0f);
	sc3332_write_register(ViPipe, 0x367e, 0x08);
	sc3332_write_register(ViPipe, 0x367f, 0x0f);
	sc3332_write_register(ViPipe, 0x3690, 0x74);
	sc3332_write_register(ViPipe, 0x3691, 0x78);
	sc3332_write_register(ViPipe, 0x3692, 0x78);
	sc3332_write_register(ViPipe, 0x3696, 0x33);
	sc3332_write_register(ViPipe, 0x3697, 0x33);
	sc3332_write_register(ViPipe, 0x3698, 0x45);
	sc3332_write_register(ViPipe, 0x369c, 0x0b);
	sc3332_write_register(ViPipe, 0x369d, 0x0f);
	sc3332_write_register(ViPipe, 0x36a0, 0x09);
	sc3332_write_register(ViPipe, 0x36a1, 0x0f);
	sc3332_write_register(ViPipe, 0x36b0, 0x88);
	sc3332_write_register(ViPipe, 0x36b1, 0x91);
	sc3332_write_register(ViPipe, 0x36b2, 0xa4);
	sc3332_write_register(ViPipe, 0x36b3, 0xcf);
	sc3332_write_register(ViPipe, 0x36b4, 0x09);
	sc3332_write_register(ViPipe, 0x36b5, 0x0b);
	sc3332_write_register(ViPipe, 0x36b6, 0x0f);
	sc3332_write_register(ViPipe, 0x370f, 0x01);
	sc3332_write_register(ViPipe, 0x3722, 0x05);
	sc3332_write_register(ViPipe, 0x3724, 0x31);
	sc3332_write_register(ViPipe, 0x3771, 0x09);
	sc3332_write_register(ViPipe, 0x3772, 0x05);
	sc3332_write_register(ViPipe, 0x3773, 0x05);
	sc3332_write_register(ViPipe, 0x377a, 0x0b);
	sc3332_write_register(ViPipe, 0x377b, 0x0f);
	sc3332_write_register(ViPipe, 0x3904, 0x04);
	sc3332_write_register(ViPipe, 0x3905, 0x8c);
	sc3332_write_register(ViPipe, 0x391d, 0x01);
	sc3332_write_register(ViPipe, 0x3922, 0x1f);
	sc3332_write_register(ViPipe, 0x3925, 0x0f);
	sc3332_write_register(ViPipe, 0x3926, 0x21);
	sc3332_write_register(ViPipe, 0x3933, 0x80);
	sc3332_write_register(ViPipe, 0x3934, 0x03);
	sc3332_write_register(ViPipe, 0x3937, 0x6f);
	sc3332_write_register(ViPipe, 0x39dc, 0x02);
	sc3332_write_register(ViPipe, 0x3e00, 0x00);
	sc3332_write_register(ViPipe, 0x3e01, 0x54);
	sc3332_write_register(ViPipe, 0x3e02, 0x00);
	sc3332_write_register(ViPipe, 0x440e, 0x02);
	sc3332_write_register(ViPipe, 0x4509, 0x28);
	sc3332_write_register(ViPipe, 0x450d, 0x32);
	sc3332_write_register(ViPipe, 0x5780, 0x66);
	sc3332_write_register(ViPipe, 0x578d, 0x40);
	sc3332_write_register(ViPipe, 0x36e9, 0x54);
	sc3332_write_register(ViPipe, 0x37f9, 0x27);

	sc3332_default_reg_init(ViPipe);

	sc3332_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC3332 1296P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
