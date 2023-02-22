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
#include "gc02m1_cmos_ex.h"
#define GC02M1_CHIP_ID_ADDR_H	0xf0
#define GC02M1_CHIP_ID_ADDR_L	0xf1
#define GC02M1_CHIP_ID		0x02e0

#define GC02M1_MIRROR_NORMAL    1
#define GC02M1_MIRROR_H         0
#define GC02M1_MIRROR_V         0
#define GC02M1_MIRROR_HV        0

#if GC02M1_MIRROR_NORMAL
#define GC02M1_MIRROR	        0x80
#elif GC02M1_MIRROR_H
#define GC02M1_MIRROR	        0x81
#elif GC02M1_MIRROR_V
#define GC02M1_MIRROR	        0x82
#elif GC02M1_MIRROR_HV
#define GC02M1_MIRROR	        0x83
#else
#define GC02M1_MIRROR	        0x80
#endif

static void gc02m1_linear_1200p30_init(VI_PIPE ViPipe);

CVI_U8 gc02m1_i2c_addr = 0x37;
const CVI_U32 gc02m1_addr_byte = 1;
const CVI_U32 gc02m1_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int gc02m1_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunGc02m1_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, gc02m1_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int gc02m1_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int gc02m1_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (gc02m1_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, gc02m1_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, gc02m1_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (gc02m1_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_INFO, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int gc02m1_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (gc02m1_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (gc02m1_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, gc02m1_addr_byte + gc02m1_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}

	ret = read(g_fd[ViPipe], buf, gc02m1_addr_byte + gc02m1_data_byte);
	syslog(LOG_INFO, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void gc02m1_standby(VI_PIPE ViPipe)
{
	int nVal;

	gc02m1_write_register(ViPipe, 0xfe, 0x00);

	nVal = gc02m1_read_register(ViPipe, 0x3e);
	nVal &= ~(0x1 << 7);
	nVal &= ~(0x1 << 4);
	gc02m1_write_register(ViPipe, 0x3e, nVal);
	gc02m1_write_register(ViPipe, 0xfc, 0x01);

	nVal = gc02m1_read_register(ViPipe, 0xf9);
	nVal |= (0x1 << 0);
	gc02m1_write_register(ViPipe, 0xf9, nVal);

	printf("gc02m1 standby\n");
}

void gc02m1_restart(VI_PIPE ViPipe)
{
	int nVal;

	nVal = gc02m1_read_register(ViPipe, 0xf9);
	nVal &= ~(0x1 << 0);
	gc02m1_write_register(ViPipe, 0xf9, nVal);

	usleep(1);
	gc02m1_write_register(ViPipe, 0xfc, 0x8e);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	nVal = gc02m1_read_register(ViPipe, 0x3e);
	nVal |= (0x1 << 7);
	nVal |= (0x1 << 4);
	gc02m1_write_register(ViPipe, 0x3e, nVal);

	printf("gc02m1 restart\n");
}

void gc02m1_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastGc02m1[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		gc02m1_write_register(ViPipe,
				g_pastGc02m1[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastGc02m1[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

int gc02m1_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	usleep(50);
	if (gc02m1_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = gc02m1_read_register(ViPipe, GC02M1_CHIP_ID_ADDR_H);
	nVal2 = gc02m1_read_register(ViPipe, GC02M1_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != GC02M1_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void gc02m1_init(VI_PIPE ViPipe)
{
	gc02m1_i2c_init(ViPipe);

	gc02m1_linear_1200p30_init(ViPipe);

	g_pastGc02m1[ViPipe]->bInit = CVI_TRUE;
}

void gc02m1_exit(VI_PIPE ViPipe)
{
	gc02m1_i2c_exit(ViPipe);
}

static void gc02m1_linear_1200p30_init(VI_PIPE ViPipe)
{
	usleep(10 * 1000);

	/*system*/
	gc02m1_write_register(ViPipe, 0xfc, 0x01);
	gc02m1_write_register(ViPipe, 0xf4, 0x41);
	gc02m1_write_register(ViPipe, 0xf5, 0xc0);
	gc02m1_write_register(ViPipe, 0xf6, 0x44);
	gc02m1_write_register(ViPipe, 0xf8, 0x32);
	gc02m1_write_register(ViPipe, 0xf9, 0x82);
	gc02m1_write_register(ViPipe, 0xfa, 0x00);
	gc02m1_write_register(ViPipe, 0xfd, 0x80);
	gc02m1_write_register(ViPipe, 0xfc, 0x81);
	gc02m1_write_register(ViPipe, 0xfe, 0x03);
	gc02m1_write_register(ViPipe, 0x01, 0x0b);
	gc02m1_write_register(ViPipe, 0xf7, 0x01);
	gc02m1_write_register(ViPipe, 0xfc, 0x80);
	gc02m1_write_register(ViPipe, 0xfc, 0x80);
	gc02m1_write_register(ViPipe, 0xfc, 0x80);
	gc02m1_write_register(ViPipe, 0xfc, 0x8e);
	/*CISCTL*/
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0x87, 0x09);
	gc02m1_write_register(ViPipe, 0xee, 0x72);
	gc02m1_write_register(ViPipe, 0xfe, 0x01);
	gc02m1_write_register(ViPipe, 0x8c, 0x90);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0x90, 0x00);
	gc02m1_write_register(ViPipe, 0x03, 0x04);
	gc02m1_write_register(ViPipe, 0x04, 0x7d);
	gc02m1_write_register(ViPipe, 0x41, 0x04);
	gc02m1_write_register(ViPipe, 0x42, 0xf4);
	gc02m1_write_register(ViPipe, 0x05, 0x04);
	gc02m1_write_register(ViPipe, 0x06, 0x48);
	gc02m1_write_register(ViPipe, 0x07, 0x00);
	gc02m1_write_register(ViPipe, 0x08, 0x18);
	gc02m1_write_register(ViPipe, 0x9d, 0x18);
	gc02m1_write_register(ViPipe, 0x09, 0x00);
	gc02m1_write_register(ViPipe, 0x0a, 0x02);
	gc02m1_write_register(ViPipe, 0x0d, 0x04);
	gc02m1_write_register(ViPipe, 0x0e, 0xbc);
	gc02m1_write_register(ViPipe, 0x17, GC02M1_MIRROR);
	gc02m1_write_register(ViPipe, 0x19, 0x04);
	gc02m1_write_register(ViPipe, 0x24, 0x00);
	gc02m1_write_register(ViPipe, 0x56, 0x20);
	gc02m1_write_register(ViPipe, 0x5b, 0x00);
	gc02m1_write_register(ViPipe, 0x5e, 0x01);
	/*analog Register width*/
	gc02m1_write_register(ViPipe, 0x21, 0x3c);
	gc02m1_write_register(ViPipe, 0x44, 0x20);
	gc02m1_write_register(ViPipe, 0xcc, 0x01);
	/*analog mode*/
	gc02m1_write_register(ViPipe, 0x1a, 0x04);
	gc02m1_write_register(ViPipe, 0x1f, 0x11);
	gc02m1_write_register(ViPipe, 0x27, 0x30);
	gc02m1_write_register(ViPipe, 0x2b, 0x00);
	gc02m1_write_register(ViPipe, 0x33, 0x00);
	gc02m1_write_register(ViPipe, 0x53, 0x90);
	gc02m1_write_register(ViPipe, 0xe6, 0x50);
	/*analog voltage*/
	gc02m1_write_register(ViPipe, 0x39, 0x07);
	gc02m1_write_register(ViPipe, 0x43, 0x04);
	gc02m1_write_register(ViPipe, 0x46, 0x2a);
	gc02m1_write_register(ViPipe, 0x7c, 0xa0);
	gc02m1_write_register(ViPipe, 0xd0, 0xbe);
	gc02m1_write_register(ViPipe, 0xd1, 0x60);
	gc02m1_write_register(ViPipe, 0xd2, 0x40);
	gc02m1_write_register(ViPipe, 0xd3, 0xf3);
	gc02m1_write_register(ViPipe, 0xde, 0x1d);
	/*analog current*/
	gc02m1_write_register(ViPipe, 0xcd, 0x05);
	gc02m1_write_register(ViPipe, 0xce, 0x6f);
	/*CISCTL RESET*/
	gc02m1_write_register(ViPipe, 0xfc, 0x88);
	gc02m1_write_register(ViPipe, 0xfe, 0x10);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xfc, 0x8e);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xfc, 0x88);
	gc02m1_write_register(ViPipe, 0xfe, 0x10);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xfc, 0x8e);
	gc02m1_write_register(ViPipe, 0xfe, 0x04);
	gc02m1_write_register(ViPipe, 0xe0, 0x01);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	/*ISP*/
	gc02m1_write_register(ViPipe, 0xfe, 0x01);
	gc02m1_write_register(ViPipe, 0x53, 0x44);
	gc02m1_write_register(ViPipe, 0x87, 0x53);
	gc02m1_write_register(ViPipe, 0x89, 0x03);
	/*Gain*/
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0xb0, 0x74);
	gc02m1_write_register(ViPipe, 0xb1, 0x04);
	gc02m1_write_register(ViPipe, 0xb2, 0x00);
	gc02m1_write_register(ViPipe, 0xb6, 0x00);
	gc02m1_write_register(ViPipe, 0xfe, 0x04);
	gc02m1_write_register(ViPipe, 0xd8, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x40);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x60);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0xc0);
	gc02m1_write_register(ViPipe, 0xc0, 0x2a);
	gc02m1_write_register(ViPipe, 0xc0, 0x80);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x40);
	gc02m1_write_register(ViPipe, 0xc0, 0xa0);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x90);
	gc02m1_write_register(ViPipe, 0xc0, 0x19);
	gc02m1_write_register(ViPipe, 0xc0, 0xc0);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0xD0);
	gc02m1_write_register(ViPipe, 0xc0, 0x2F);
	gc02m1_write_register(ViPipe, 0xc0, 0xe0);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x90);
	gc02m1_write_register(ViPipe, 0xc0, 0x39);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0x20);
	gc02m1_write_register(ViPipe, 0xc0, 0x04);
	gc02m1_write_register(ViPipe, 0xc0, 0x20);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0xe0);
	gc02m1_write_register(ViPipe, 0xc0, 0x0f);
	gc02m1_write_register(ViPipe, 0xc0, 0x40);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0xe0);
	gc02m1_write_register(ViPipe, 0xc0, 0x1a);
	gc02m1_write_register(ViPipe, 0xc0, 0x60);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0x20);
	gc02m1_write_register(ViPipe, 0xc0, 0x25);
	gc02m1_write_register(ViPipe, 0xc0, 0x80);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0xa0);
	gc02m1_write_register(ViPipe, 0xc0, 0x2c);
	gc02m1_write_register(ViPipe, 0xc0, 0xa0);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0xe0);
	gc02m1_write_register(ViPipe, 0xc0, 0x32);
	gc02m1_write_register(ViPipe, 0xc0, 0xc0);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0x20);
	gc02m1_write_register(ViPipe, 0xc0, 0x38);
	gc02m1_write_register(ViPipe, 0xc0, 0xe0);
	gc02m1_write_register(ViPipe, 0xc0, 0x01);
	gc02m1_write_register(ViPipe, 0xc0, 0x60);
	gc02m1_write_register(ViPipe, 0xc0, 0x3c);
	gc02m1_write_register(ViPipe, 0xc0, 0x00);
	gc02m1_write_register(ViPipe, 0xc0, 0x02);
	gc02m1_write_register(ViPipe, 0xc0, 0xa0);
	gc02m1_write_register(ViPipe, 0xc0, 0x40);
	gc02m1_write_register(ViPipe, 0xc0, 0x80);
	gc02m1_write_register(ViPipe, 0xc0, 0x02);
	gc02m1_write_register(ViPipe, 0xc0, 0x18);
	gc02m1_write_register(ViPipe, 0xc0, 0x5c);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0x9f, 0x10);
	/*BLK*/
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0x26, 0x20);
	gc02m1_write_register(ViPipe, 0xfe, 0x01);
	gc02m1_write_register(ViPipe, 0x40, 0x22);
	gc02m1_write_register(ViPipe, 0x46, 0x7f);
	gc02m1_write_register(ViPipe, 0x49, 0x0f);
	gc02m1_write_register(ViPipe, 0x4a, 0xf0);
	gc02m1_write_register(ViPipe, 0xfe, 0x04);
	gc02m1_write_register(ViPipe, 0x14, 0x80);
	gc02m1_write_register(ViPipe, 0x15, 0x80);
	gc02m1_write_register(ViPipe, 0x16, 0x80);
	gc02m1_write_register(ViPipe, 0x17, 0x80);
	/*ant _blooming*/
	gc02m1_write_register(ViPipe, 0xfe, 0x01);
	gc02m1_write_register(ViPipe, 0x41, 0x20);
	gc02m1_write_register(ViPipe, 0x4c, 0x00);
	gc02m1_write_register(ViPipe, 0x4d, 0x0c);
	gc02m1_write_register(ViPipe, 0x44, 0x08);
	gc02m1_write_register(ViPipe, 0x48, 0x03);
	/*Window 1600X1200*/
	gc02m1_write_register(ViPipe, 0xfe, 0x01);
	gc02m1_write_register(ViPipe, 0x90, 0x01);
	gc02m1_write_register(ViPipe, 0x91, 0x00);
	gc02m1_write_register(ViPipe, 0x92, 0x06);
	gc02m1_write_register(ViPipe, 0x93, 0x00);
	gc02m1_write_register(ViPipe, 0x94, 0x06);
	gc02m1_write_register(ViPipe, 0x95, 0x04);
	gc02m1_write_register(ViPipe, 0x96, 0xb0);
	gc02m1_write_register(ViPipe, 0x97, 0x06);
	gc02m1_write_register(ViPipe, 0x98, 0x40);
	/*mipi*/
	gc02m1_write_register(ViPipe, 0xfe, 0x03);
	gc02m1_write_register(ViPipe, 0x01, 0x23);
	gc02m1_write_register(ViPipe, 0x03, 0xce);
	gc02m1_write_register(ViPipe, 0x04, 0x48);
	gc02m1_write_register(ViPipe, 0x15, 0x00);
	gc02m1_write_register(ViPipe, 0x21, 0x10);
	gc02m1_write_register(ViPipe, 0x22, 0x05);
	gc02m1_write_register(ViPipe, 0x23, 0x20);
	gc02m1_write_register(ViPipe, 0x25, 0x20);
	gc02m1_write_register(ViPipe, 0x26, 0x08);
	gc02m1_write_register(ViPipe, 0x29, 0x06);
	gc02m1_write_register(ViPipe, 0x2a, 0x0a);
	gc02m1_write_register(ViPipe, 0x2b, 0x08);
	/*out*/
	gc02m1_write_register(ViPipe, 0xfe, 0x01);
	gc02m1_write_register(ViPipe, 0x8c, 0x10);
	gc02m1_write_register(ViPipe, 0xfe, 0x00);
	gc02m1_write_register(ViPipe, 0x3e, 0x90);

	gc02m1_default_reg_init(ViPipe);
	usleep(10 * 1000);

	printf("ViPipe:%d,===GC02M1 1200P 30fps 10bit LINEAR Init OK!===\n", ViPipe);
}
