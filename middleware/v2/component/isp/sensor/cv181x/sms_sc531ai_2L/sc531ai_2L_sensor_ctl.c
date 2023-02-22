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
#include "sc531ai_2L_cmos_ex.h"

static void sc531ai_2l_linear_1620p30_init(VI_PIPE ViPipe);

const CVI_U8 sc531ai_2l_i2c_addr = 0x30;        /* I2C Address of sc531AI_2L */
const CVI_U32 sc531ai_2l_addr_byte = 2;
const CVI_U32 sc531ai_2l_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc531ai_2l_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunsc531AI_2L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc531ai_2l_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc531ai_2l_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc531ai_2l_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc531ai_2l_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc531ai_2l_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc531ai_2l_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc531ai_2l_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc531ai_2l_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc531ai_2l_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc531ai_2l_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc531ai_2l_addr_byte + sc531ai_2l_data_byte);
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

void sc531ai_2l_standby(VI_PIPE ViPipe)
{
	sc531ai_2l_write_register(ViPipe, 0x0100, 0x00);
}

void sc531ai_2l_restart(VI_PIPE ViPipe)
{
	sc531ai_2l_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc531ai_2l_write_register(ViPipe, 0x0100, 0x01);
}

void sc531ai_2l_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastsc531AI_2L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc531ai_2l_write_register(ViPipe,
				g_pastsc531AI_2L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastsc531AI_2L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define sc531AI_2L_CHIP_ID_HI_ADDR		0x3107
#define sc531AI_2L_CHIP_ID_LO_ADDR		0x3108
#define sc531AI_2L_CHIP_ID			0x9e39

int sc531ai_2l_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc531ai_2l_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc531ai_2l_read_register(ViPipe, sc531AI_2L_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc531ai_2l_read_register(ViPipe, sc531AI_2L_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != sc531AI_2L_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc531ai_2l_init(VI_PIPE ViPipe)
{
	sc531ai_2l_i2c_init(ViPipe);

	sc531ai_2l_linear_1620p30_init(ViPipe);

	g_pastsc531AI_2L[ViPipe]->bInit = CVI_TRUE;
}

void sc531ai_2l_exit(VI_PIPE ViPipe)
{
	sc531ai_2l_i2c_exit(ViPipe);
}

/* 1620P30 */
static void sc531ai_2l_linear_1620p30_init(VI_PIPE ViPipe)
{
	sc531ai_2l_write_register(ViPipe, 0x0103, 0x01);
	sc531ai_2l_write_register(ViPipe, 0x0100, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x36e9, 0x80);
	sc531ai_2l_write_register(ViPipe, 0x37f9, 0x80);
	sc531ai_2l_write_register(ViPipe, 0x3018, 0x32);
	sc531ai_2l_write_register(ViPipe, 0x3019, 0x0c);
	sc531ai_2l_write_register(ViPipe, 0x301f, 0x69);
	sc531ai_2l_write_register(ViPipe, 0x3250, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x3251, 0x98);
	sc531ai_2l_write_register(ViPipe, 0x3253, 0x0c);
	sc531ai_2l_write_register(ViPipe, 0x325f, 0x20);
	sc531ai_2l_write_register(ViPipe, 0x3301, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3304, 0x50);
	sc531ai_2l_write_register(ViPipe, 0x3306, 0x88);
	sc531ai_2l_write_register(ViPipe, 0x3308, 0x14);
	sc531ai_2l_write_register(ViPipe, 0x3309, 0x70);
	sc531ai_2l_write_register(ViPipe, 0x330a, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x330b, 0xf8);
	sc531ai_2l_write_register(ViPipe, 0x330d, 0x10);
	sc531ai_2l_write_register(ViPipe, 0x330e, 0x42);
	sc531ai_2l_write_register(ViPipe, 0x331e, 0x41);
	sc531ai_2l_write_register(ViPipe, 0x331f, 0x61);
	sc531ai_2l_write_register(ViPipe, 0x3333, 0x10);
	sc531ai_2l_write_register(ViPipe, 0x335d, 0x60);
	sc531ai_2l_write_register(ViPipe, 0x335e, 0x06);
	sc531ai_2l_write_register(ViPipe, 0x335f, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3364, 0x56);
	sc531ai_2l_write_register(ViPipe, 0x3366, 0x01);
	sc531ai_2l_write_register(ViPipe, 0x337c, 0x02);
	sc531ai_2l_write_register(ViPipe, 0x337d, 0x0a);
	sc531ai_2l_write_register(ViPipe, 0x3390, 0x01);
	sc531ai_2l_write_register(ViPipe, 0x3391, 0x03);
	sc531ai_2l_write_register(ViPipe, 0x3392, 0x07);
	sc531ai_2l_write_register(ViPipe, 0x3393, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3394, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3395, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3396, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x3397, 0x48);
	sc531ai_2l_write_register(ViPipe, 0x3398, 0x4b);
	sc531ai_2l_write_register(ViPipe, 0x3399, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x339a, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x339b, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x339c, 0x1d);
	sc531ai_2l_write_register(ViPipe, 0x33a2, 0x04);
	sc531ai_2l_write_register(ViPipe, 0x33ae, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x33af, 0x50);
	sc531ai_2l_write_register(ViPipe, 0x33b1, 0x80);
	sc531ai_2l_write_register(ViPipe, 0x33b2, 0x48);
	sc531ai_2l_write_register(ViPipe, 0x33b3, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x349f, 0x02);
	sc531ai_2l_write_register(ViPipe, 0x34a6, 0x48);
	sc531ai_2l_write_register(ViPipe, 0x34a7, 0x4b);
	sc531ai_2l_write_register(ViPipe, 0x34a8, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x34a9, 0x18);
	sc531ai_2l_write_register(ViPipe, 0x34f8, 0x5f);
	sc531ai_2l_write_register(ViPipe, 0x34f9, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3632, 0x48);
	sc531ai_2l_write_register(ViPipe, 0x3633, 0x32);
	sc531ai_2l_write_register(ViPipe, 0x3637, 0x27);
	sc531ai_2l_write_register(ViPipe, 0x3638, 0xc1);
	sc531ai_2l_write_register(ViPipe, 0x363b, 0x20);
	sc531ai_2l_write_register(ViPipe, 0x363d, 0x02);
	sc531ai_2l_write_register(ViPipe, 0x3670, 0x09);
	sc531ai_2l_write_register(ViPipe, 0x3674, 0x8b);
	sc531ai_2l_write_register(ViPipe, 0x3675, 0xc6);
	sc531ai_2l_write_register(ViPipe, 0x3676, 0x8b);
	sc531ai_2l_write_register(ViPipe, 0x367c, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x367d, 0x48);
	sc531ai_2l_write_register(ViPipe, 0x3690, 0x32);
	sc531ai_2l_write_register(ViPipe, 0x3691, 0x43);
	sc531ai_2l_write_register(ViPipe, 0x3692, 0x33);
	sc531ai_2l_write_register(ViPipe, 0x3693, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x3694, 0x4b);
	sc531ai_2l_write_register(ViPipe, 0x3698, 0x85);
	sc531ai_2l_write_register(ViPipe, 0x3699, 0x8f);
	sc531ai_2l_write_register(ViPipe, 0x369a, 0xa0);
	sc531ai_2l_write_register(ViPipe, 0x369b, 0xc3);
	sc531ai_2l_write_register(ViPipe, 0x36a2, 0x49);
	sc531ai_2l_write_register(ViPipe, 0x36a3, 0x4b);
	sc531ai_2l_write_register(ViPipe, 0x36a4, 0x4f);
	sc531ai_2l_write_register(ViPipe, 0x36d0, 0x01);
	sc531ai_2l_write_register(ViPipe, 0x36ea, 0x0b);
	sc531ai_2l_write_register(ViPipe, 0x36eb, 0x04);
	sc531ai_2l_write_register(ViPipe, 0x36ec, 0x03);
	sc531ai_2l_write_register(ViPipe, 0x36ed, 0x14);
	sc531ai_2l_write_register(ViPipe, 0x370f, 0x01);
	sc531ai_2l_write_register(ViPipe, 0x3722, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x3728, 0x10);
	sc531ai_2l_write_register(ViPipe, 0x37b0, 0x03);
	sc531ai_2l_write_register(ViPipe, 0x37b1, 0x03);
	sc531ai_2l_write_register(ViPipe, 0x37b2, 0x83);
	sc531ai_2l_write_register(ViPipe, 0x37b3, 0x48);
	sc531ai_2l_write_register(ViPipe, 0x37b4, 0x49);
	sc531ai_2l_write_register(ViPipe, 0x37fa, 0x0b);
	sc531ai_2l_write_register(ViPipe, 0x37fb, 0x24);
	sc531ai_2l_write_register(ViPipe, 0x37fc, 0x01);
	sc531ai_2l_write_register(ViPipe, 0x37fd, 0x14);
	sc531ai_2l_write_register(ViPipe, 0x3901, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x3902, 0xc5);
	sc531ai_2l_write_register(ViPipe, 0x3904, 0x08);
	sc531ai_2l_write_register(ViPipe, 0x3905, 0x8c);
	sc531ai_2l_write_register(ViPipe, 0x3909, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x391d, 0x04);
	sc531ai_2l_write_register(ViPipe, 0x391f, 0x44);
	sc531ai_2l_write_register(ViPipe, 0x3926, 0x21);
	sc531ai_2l_write_register(ViPipe, 0x3929, 0x18);
	sc531ai_2l_write_register(ViPipe, 0x3933, 0x82);
	sc531ai_2l_write_register(ViPipe, 0x3934, 0x0a);
	sc531ai_2l_write_register(ViPipe, 0x3937, 0x5f);
	sc531ai_2l_write_register(ViPipe, 0x3939, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x393a, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x39dc, 0x02);
	sc531ai_2l_write_register(ViPipe, 0x3e01, 0xcd);
	sc531ai_2l_write_register(ViPipe, 0x3e02, 0xa0);
	sc531ai_2l_write_register(ViPipe, 0x440e, 0x02);
	sc531ai_2l_write_register(ViPipe, 0x4509, 0x20);
	sc531ai_2l_write_register(ViPipe, 0x4837, 0x14);
	sc531ai_2l_write_register(ViPipe, 0x5010, 0x10);
	sc531ai_2l_write_register(ViPipe, 0x5780, 0x66);
	sc531ai_2l_write_register(ViPipe, 0x578d, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x5799, 0x06);
	sc531ai_2l_write_register(ViPipe, 0x57ad, 0x00);
	sc531ai_2l_write_register(ViPipe, 0x5ae0, 0xfe);
	sc531ai_2l_write_register(ViPipe, 0x5ae1, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x5ae2, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5ae3, 0x2a);
	sc531ai_2l_write_register(ViPipe, 0x5ae4, 0x24);
	sc531ai_2l_write_register(ViPipe, 0x5ae5, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5ae6, 0x2a);
	sc531ai_2l_write_register(ViPipe, 0x5ae7, 0x24);
	sc531ai_2l_write_register(ViPipe, 0x5ae8, 0x3c);
	sc531ai_2l_write_register(ViPipe, 0x5ae9, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5aea, 0x28);
	sc531ai_2l_write_register(ViPipe, 0x5aeb, 0x3c);
	sc531ai_2l_write_register(ViPipe, 0x5aec, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5aed, 0x28);
	sc531ai_2l_write_register(ViPipe, 0x5aee, 0xfe);
	sc531ai_2l_write_register(ViPipe, 0x5aef, 0x40);
	sc531ai_2l_write_register(ViPipe, 0x5af4, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5af5, 0x2a);
	sc531ai_2l_write_register(ViPipe, 0x5af6, 0x24);
	sc531ai_2l_write_register(ViPipe, 0x5af7, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5af8, 0x2a);
	sc531ai_2l_write_register(ViPipe, 0x5af9, 0x24);
	sc531ai_2l_write_register(ViPipe, 0x5afa, 0x3c);
	sc531ai_2l_write_register(ViPipe, 0x5afb, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5afc, 0x28);
	sc531ai_2l_write_register(ViPipe, 0x5afd, 0x3c);
	sc531ai_2l_write_register(ViPipe, 0x5afe, 0x30);
	sc531ai_2l_write_register(ViPipe, 0x5aff, 0x28);
	sc531ai_2l_write_register(ViPipe, 0x36e9, 0x44);
	sc531ai_2l_write_register(ViPipe, 0x37f9, 0x44);

	sc531ai_2l_default_reg_init(ViPipe);

	sc531ai_2l_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===sc531AI_2L 1620P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
