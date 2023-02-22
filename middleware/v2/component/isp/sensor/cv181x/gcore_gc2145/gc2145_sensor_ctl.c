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
#include "gc2145_cmos_ex.h"

static void gc2145_linear_1200p12_init(VI_PIPE ViPipe);

CVI_U8 gc2145_i2c_addr = 0x3C;//0x78
const CVI_U32 gc2145_addr_byte = 1;
const CVI_U32 gc2145_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int gc2145_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunGc2145_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, gc2145_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int gc2145_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int gc2145_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (gc2145_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, gc2145_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, gc2145_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (gc2145_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}


int gc2145_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (gc2145_addr_byte == 1) {
		buf[idx] = addr & 0xff;
		idx++;
	}
	if (gc2145_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, gc2145_addr_byte + gc2145_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	//syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

#define GC2145_CHIP_ID_ADDR_H	0xf0
#define GC2145_CHIP_ID_ADDR_L	0xf1
#define GC2145_CHIP_ID		0x2145

int gc2145_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	usleep(50);
	if (gc2145_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = gc2145_read_register(ViPipe, GC2145_CHIP_ID_ADDR_H);
	nVal2 = gc2145_read_register(ViPipe, GC2145_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != GC2145_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void gc2145_init(VI_PIPE ViPipe)
{
	gc2145_i2c_init(ViPipe);

	gc2145_linear_1200p12_init(ViPipe);

	g_pastGc2145[ViPipe]->bInit = CVI_TRUE;
}

void gc2145_exit(VI_PIPE ViPipe)
{
	gc2145_i2c_exit(ViPipe);
}

static void gc2145_linear_1200p12_init(VI_PIPE ViPipe)
{
	gc2145_write_register(ViPipe, 0xfe, 0xf0);
	gc2145_write_register(ViPipe, 0xfe, 0xf0);
	gc2145_write_register(ViPipe, 0xfe, 0xf0);
	gc2145_write_register(ViPipe, 0xfc, 0x06);
	gc2145_write_register(ViPipe, 0xf6, 0x00);
	gc2145_write_register(ViPipe, 0xf7, 0x1d);
	gc2145_write_register(ViPipe, 0xf8, 0x84);
	gc2145_write_register(ViPipe, 0xfa, 0x00);
	gc2145_write_register(ViPipe, 0xf9, 0xfe);
	gc2145_write_register(ViPipe, 0xf2, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x03, 0x04);
	gc2145_write_register(ViPipe, 0x04, 0xe2);
	gc2145_write_register(ViPipe, 0x09, 0x00);
	gc2145_write_register(ViPipe, 0x0a, 0x00);
	gc2145_write_register(ViPipe, 0x0b, 0x00);
	gc2145_write_register(ViPipe, 0x0c, 0x00);
	gc2145_write_register(ViPipe, 0x0d, 0x04);
	gc2145_write_register(ViPipe, 0x0e, 0xc0);
	gc2145_write_register(ViPipe, 0x0f, 0x06);
	gc2145_write_register(ViPipe, 0x10, 0x52);
	gc2145_write_register(ViPipe, 0x12, 0x2e);
	gc2145_write_register(ViPipe, 0x17, 0x14);
	gc2145_write_register(ViPipe, 0x18, 0x22);
	gc2145_write_register(ViPipe, 0x19, 0x0e);
	gc2145_write_register(ViPipe, 0x1a, 0x01);
	gc2145_write_register(ViPipe, 0x1b, 0x4b);
	gc2145_write_register(ViPipe, 0x1c, 0x07);
	gc2145_write_register(ViPipe, 0x1d, 0x10);
	gc2145_write_register(ViPipe, 0x1e, 0x88);
	gc2145_write_register(ViPipe, 0x1f, 0x78);
	gc2145_write_register(ViPipe, 0x20, 0x03);
	gc2145_write_register(ViPipe, 0x21, 0x40);
	gc2145_write_register(ViPipe, 0x22, 0xa0);
	gc2145_write_register(ViPipe, 0x24, 0x16);
	gc2145_write_register(ViPipe, 0x25, 0x01);
	gc2145_write_register(ViPipe, 0x26, 0x10);
	gc2145_write_register(ViPipe, 0x2d, 0x60);
	gc2145_write_register(ViPipe, 0x30, 0x01);
	gc2145_write_register(ViPipe, 0x31, 0x90);
	gc2145_write_register(ViPipe, 0x33, 0x06);
	gc2145_write_register(ViPipe, 0x34, 0x01);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x80, 0x7f);
	gc2145_write_register(ViPipe, 0x81, 0x26);
	gc2145_write_register(ViPipe, 0x82, 0xfa);
	gc2145_write_register(ViPipe, 0x83, 0x00);
	gc2145_write_register(ViPipe, 0x84, 0x00);
	gc2145_write_register(ViPipe, 0x86, 0x02);
	gc2145_write_register(ViPipe, 0x88, 0x03);
	gc2145_write_register(ViPipe, 0x89, 0x03);
	gc2145_write_register(ViPipe, 0x85, 0x08);
	gc2145_write_register(ViPipe, 0x8a, 0x00);
	gc2145_write_register(ViPipe, 0x8b, 0x00);
	gc2145_write_register(ViPipe, 0xb0, 0x55);
	gc2145_write_register(ViPipe, 0xc3, 0x00);
	gc2145_write_register(ViPipe, 0xc4, 0x80);
	gc2145_write_register(ViPipe, 0xc5, 0x90);
	gc2145_write_register(ViPipe, 0xc6, 0x3b);
	gc2145_write_register(ViPipe, 0xc7, 0x46);
	gc2145_write_register(ViPipe, 0xec, 0x06);
	gc2145_write_register(ViPipe, 0xed, 0x04);
	gc2145_write_register(ViPipe, 0xee, 0x60);
	gc2145_write_register(ViPipe, 0xef, 0x90);
	gc2145_write_register(ViPipe, 0xb6, 0x01);
	gc2145_write_register(ViPipe, 0x90, 0x01);
	gc2145_write_register(ViPipe, 0x91, 0x00);
	gc2145_write_register(ViPipe, 0x92, 0x00);
	gc2145_write_register(ViPipe, 0x93, 0x00);
	gc2145_write_register(ViPipe, 0x94, 0x00);
	gc2145_write_register(ViPipe, 0x95, 0x04);
	gc2145_write_register(ViPipe, 0x96, 0xb0);
	gc2145_write_register(ViPipe, 0x97, 0x06);
	gc2145_write_register(ViPipe, 0x98, 0x40);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x40, 0x42);
	gc2145_write_register(ViPipe, 0x41, 0x00);
	gc2145_write_register(ViPipe, 0x43, 0x5b);
	gc2145_write_register(ViPipe, 0x5e, 0x00);
	gc2145_write_register(ViPipe, 0x5f, 0x00);
	gc2145_write_register(ViPipe, 0x60, 0x00);
	gc2145_write_register(ViPipe, 0x61, 0x00);
	gc2145_write_register(ViPipe, 0x62, 0x00);
	gc2145_write_register(ViPipe, 0x63, 0x00);
	gc2145_write_register(ViPipe, 0x64, 0x00);
	gc2145_write_register(ViPipe, 0x65, 0x00);
	gc2145_write_register(ViPipe, 0x66, 0x20);
	gc2145_write_register(ViPipe, 0x67, 0x20);
	gc2145_write_register(ViPipe, 0x68, 0x20);
	gc2145_write_register(ViPipe, 0x69, 0x20);
	gc2145_write_register(ViPipe, 0x76, 0x00);
	gc2145_write_register(ViPipe, 0x6a, 0x08);
	gc2145_write_register(ViPipe, 0x6b, 0x08);
	gc2145_write_register(ViPipe, 0x6c, 0x08);
	gc2145_write_register(ViPipe, 0x6d, 0x08);
	gc2145_write_register(ViPipe, 0x6e, 0x08);
	gc2145_write_register(ViPipe, 0x6f, 0x08);
	gc2145_write_register(ViPipe, 0x70, 0x08);
	gc2145_write_register(ViPipe, 0x71, 0x08);
	gc2145_write_register(ViPipe, 0x76, 0x00);
	gc2145_write_register(ViPipe, 0x72, 0xf0);
	gc2145_write_register(ViPipe, 0x7e, 0x3c);
	gc2145_write_register(ViPipe, 0x7f, 0x00);
	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x48, 0x15);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x01, 0x04);
	gc2145_write_register(ViPipe, 0x02, 0xc0);
	gc2145_write_register(ViPipe, 0x03, 0x04);
	gc2145_write_register(ViPipe, 0x04, 0x90);
	gc2145_write_register(ViPipe, 0x05, 0x30);
	gc2145_write_register(ViPipe, 0x06, 0x90);
	gc2145_write_register(ViPipe, 0x07, 0x30);
	gc2145_write_register(ViPipe, 0x08, 0x80);
	gc2145_write_register(ViPipe, 0x09, 0x00);
	gc2145_write_register(ViPipe, 0x0a, 0x82);
	gc2145_write_register(ViPipe, 0x0b, 0x11);
	gc2145_write_register(ViPipe, 0x0c, 0x10);
	gc2145_write_register(ViPipe, 0x11, 0x10);
	gc2145_write_register(ViPipe, 0x13, 0x7b);
	gc2145_write_register(ViPipe, 0x17, 0x00);
	gc2145_write_register(ViPipe, 0x1c, 0x11);
	gc2145_write_register(ViPipe, 0x1e, 0x61);
	gc2145_write_register(ViPipe, 0x1f, 0x35);
	gc2145_write_register(ViPipe, 0x20, 0x40);
	gc2145_write_register(ViPipe, 0x22, 0x40);
	gc2145_write_register(ViPipe, 0x23, 0x20);
	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x0f, 0x04);
	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x12, 0x35);
	gc2145_write_register(ViPipe, 0x15, 0xb0);
	gc2145_write_register(ViPipe, 0x10, 0x31);
	gc2145_write_register(ViPipe, 0x3e, 0x28);
	gc2145_write_register(ViPipe, 0x3f, 0xb0);
	gc2145_write_register(ViPipe, 0x40, 0x90);
	gc2145_write_register(ViPipe, 0x41, 0x0f);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x90, 0x6c);
	gc2145_write_register(ViPipe, 0x91, 0x03);
	gc2145_write_register(ViPipe, 0x92, 0xcb);
	gc2145_write_register(ViPipe, 0x94, 0x33);
	gc2145_write_register(ViPipe, 0x95, 0x84);
	gc2145_write_register(ViPipe, 0x97, 0x65);
	gc2145_write_register(ViPipe, 0xa2, 0x11);
	gc2145_write_register(ViPipe, 0xfe, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x80, 0xc1);
	gc2145_write_register(ViPipe, 0x81, 0x08);
	gc2145_write_register(ViPipe, 0x82, 0x05);
	gc2145_write_register(ViPipe, 0x83, 0x08);
	gc2145_write_register(ViPipe, 0x84, 0x0a);
	gc2145_write_register(ViPipe, 0x86, 0xf0);
	gc2145_write_register(ViPipe, 0x87, 0x50);
	gc2145_write_register(ViPipe, 0x88, 0x15);
	gc2145_write_register(ViPipe, 0x89, 0xb0);
	gc2145_write_register(ViPipe, 0x8a, 0x30);
	gc2145_write_register(ViPipe, 0x8b, 0x10);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x21, 0x04);
	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0xa3, 0x50);
	gc2145_write_register(ViPipe, 0xa4, 0x20);
	gc2145_write_register(ViPipe, 0xa5, 0x40);
	gc2145_write_register(ViPipe, 0xa6, 0x80);
	gc2145_write_register(ViPipe, 0xab, 0x40);
	gc2145_write_register(ViPipe, 0xae, 0x0c);
	gc2145_write_register(ViPipe, 0xb3, 0x46);
	gc2145_write_register(ViPipe, 0xb4, 0x64);
	gc2145_write_register(ViPipe, 0xb6, 0x38);
	gc2145_write_register(ViPipe, 0xb7, 0x01);
	gc2145_write_register(ViPipe, 0xb9, 0x2b);
	gc2145_write_register(ViPipe, 0x3c, 0x04);
	gc2145_write_register(ViPipe, 0x3d, 0x15);
	gc2145_write_register(ViPipe, 0x4b, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x20);
	gc2145_write_register(ViPipe, 0xfe, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x10, 0x09);
	gc2145_write_register(ViPipe, 0x11, 0x0d);
	gc2145_write_register(ViPipe, 0x12, 0x13);
	gc2145_write_register(ViPipe, 0x13, 0x19);
	gc2145_write_register(ViPipe, 0x14, 0x27);
	gc2145_write_register(ViPipe, 0x15, 0x37);
	gc2145_write_register(ViPipe, 0x16, 0x45);
	gc2145_write_register(ViPipe, 0x17, 0x53);
	gc2145_write_register(ViPipe, 0x18, 0x69);
	gc2145_write_register(ViPipe, 0x19, 0x7d);
	gc2145_write_register(ViPipe, 0x1a, 0x8f);
	gc2145_write_register(ViPipe, 0x1b, 0x9d);
	gc2145_write_register(ViPipe, 0x1c, 0xa9);
	gc2145_write_register(ViPipe, 0x1d, 0xbd);
	gc2145_write_register(ViPipe, 0x1e, 0xcd);
	gc2145_write_register(ViPipe, 0x1f, 0xd9);
	gc2145_write_register(ViPipe, 0x20, 0xe3);
	gc2145_write_register(ViPipe, 0x21, 0xea);
	gc2145_write_register(ViPipe, 0x22, 0xef);
	gc2145_write_register(ViPipe, 0x23, 0xf5);
	gc2145_write_register(ViPipe, 0x24, 0xf9);
	gc2145_write_register(ViPipe, 0x25, 0xff);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0xc6, 0x20);
	gc2145_write_register(ViPipe, 0xc7, 0x2b);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x26, 0x0f);
	gc2145_write_register(ViPipe, 0x27, 0x14);
	gc2145_write_register(ViPipe, 0x28, 0x19);
	gc2145_write_register(ViPipe, 0x29, 0x1e);
	gc2145_write_register(ViPipe, 0x2a, 0x27);
	gc2145_write_register(ViPipe, 0x2b, 0x33);
	gc2145_write_register(ViPipe, 0x2c, 0x3b);
	gc2145_write_register(ViPipe, 0x2d, 0x45);
	gc2145_write_register(ViPipe, 0x2e, 0x59);
	gc2145_write_register(ViPipe, 0x2f, 0x69);
	gc2145_write_register(ViPipe, 0x30, 0x7c);
	gc2145_write_register(ViPipe, 0x31, 0x89);
	gc2145_write_register(ViPipe, 0x32, 0x98);
	gc2145_write_register(ViPipe, 0x33, 0xae);
	gc2145_write_register(ViPipe, 0x34, 0xc0);
	gc2145_write_register(ViPipe, 0x35, 0xcf);
	gc2145_write_register(ViPipe, 0x36, 0xda);
	gc2145_write_register(ViPipe, 0x37, 0xe2);
	gc2145_write_register(ViPipe, 0x38, 0xe9);
	gc2145_write_register(ViPipe, 0x39, 0xf3);
	gc2145_write_register(ViPipe, 0x3a, 0xf9);
	gc2145_write_register(ViPipe, 0x3b, 0xff);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0xd1, 0x32);
	gc2145_write_register(ViPipe, 0xd2, 0x32);
	gc2145_write_register(ViPipe, 0xd3, 0x40);
	gc2145_write_register(ViPipe, 0xd6, 0xf0);
	gc2145_write_register(ViPipe, 0xd7, 0x10);
	gc2145_write_register(ViPipe, 0xd8, 0xda);
	gc2145_write_register(ViPipe, 0xdd, 0x14);
	gc2145_write_register(ViPipe, 0xde, 0x86);
	gc2145_write_register(ViPipe, 0xed, 0x80);
	gc2145_write_register(ViPipe, 0xee, 0x00);
	gc2145_write_register(ViPipe, 0xef, 0x3f);
	gc2145_write_register(ViPipe, 0xd8, 0xd8);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x9f, 0x40);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0xc2, 0x14);
	gc2145_write_register(ViPipe, 0xc3, 0x0d);
	gc2145_write_register(ViPipe, 0xc4, 0x0c);
	gc2145_write_register(ViPipe, 0xc8, 0x15);
	gc2145_write_register(ViPipe, 0xc9, 0x0d);
	gc2145_write_register(ViPipe, 0xca, 0x0a);
	gc2145_write_register(ViPipe, 0xbc, 0x24);
	gc2145_write_register(ViPipe, 0xbd, 0x10);
	gc2145_write_register(ViPipe, 0xbe, 0x0b);
	gc2145_write_register(ViPipe, 0xb6, 0x25);
	gc2145_write_register(ViPipe, 0xb7, 0x16);
	gc2145_write_register(ViPipe, 0xb8, 0x15);
	gc2145_write_register(ViPipe, 0xc5, 0x00);
	gc2145_write_register(ViPipe, 0xc6, 0x00);
	gc2145_write_register(ViPipe, 0xc7, 0x00);
	gc2145_write_register(ViPipe, 0xcb, 0x00);
	gc2145_write_register(ViPipe, 0xcc, 0x00);
	gc2145_write_register(ViPipe, 0xcd, 0x00);
	gc2145_write_register(ViPipe, 0xbf, 0x07);
	gc2145_write_register(ViPipe, 0xc0, 0x00);
	gc2145_write_register(ViPipe, 0xc1, 0x00);
	gc2145_write_register(ViPipe, 0xb9, 0x00);
	gc2145_write_register(ViPipe, 0xba, 0x00);
	gc2145_write_register(ViPipe, 0xbb, 0x00);
	gc2145_write_register(ViPipe, 0xaa, 0x01);
	gc2145_write_register(ViPipe, 0xab, 0x01);
	gc2145_write_register(ViPipe, 0xac, 0x00);
	gc2145_write_register(ViPipe, 0xad, 0x05);
	gc2145_write_register(ViPipe, 0xae, 0x06);
	gc2145_write_register(ViPipe, 0xaf, 0x0e);
	gc2145_write_register(ViPipe, 0xb0, 0x0b);
	gc2145_write_register(ViPipe, 0xb1, 0x07);
	gc2145_write_register(ViPipe, 0xb2, 0x06);
	gc2145_write_register(ViPipe, 0xb3, 0x17);
	gc2145_write_register(ViPipe, 0xb4, 0x0e);
	gc2145_write_register(ViPipe, 0xb5, 0x0e);
	gc2145_write_register(ViPipe, 0xd0, 0x09);
	gc2145_write_register(ViPipe, 0xd1, 0x00);
	gc2145_write_register(ViPipe, 0xd2, 0x00);
	gc2145_write_register(ViPipe, 0xd6, 0x08);
	gc2145_write_register(ViPipe, 0xd7, 0x00);
	gc2145_write_register(ViPipe, 0xd8, 0x00);
	gc2145_write_register(ViPipe, 0xd9, 0x00);
	gc2145_write_register(ViPipe, 0xda, 0x00);
	gc2145_write_register(ViPipe, 0xdb, 0x00);
	gc2145_write_register(ViPipe, 0xd3, 0x0a);
	gc2145_write_register(ViPipe, 0xd4, 0x00);
	gc2145_write_register(ViPipe, 0xd5, 0x00);
	gc2145_write_register(ViPipe, 0xa4, 0x00);
	gc2145_write_register(ViPipe, 0xa5, 0x00);
	gc2145_write_register(ViPipe, 0xa6, 0x77);
	gc2145_write_register(ViPipe, 0xa7, 0x77);
	gc2145_write_register(ViPipe, 0xa8, 0x77);
	gc2145_write_register(ViPipe, 0xa9, 0x77);
	gc2145_write_register(ViPipe, 0xa1, 0x80);
	gc2145_write_register(ViPipe, 0xa2, 0x80);
	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0xdf, 0x0d);
	gc2145_write_register(ViPipe, 0xdc, 0x25);
	gc2145_write_register(ViPipe, 0xdd, 0x30);
	gc2145_write_register(ViPipe, 0xe0, 0x77);
	gc2145_write_register(ViPipe, 0xe1, 0x80);
	gc2145_write_register(ViPipe, 0xe2, 0x77);
	gc2145_write_register(ViPipe, 0xe3, 0x90);
	gc2145_write_register(ViPipe, 0xe6, 0x90);
	gc2145_write_register(ViPipe, 0xe7, 0xa0);
	gc2145_write_register(ViPipe, 0xe8, 0x90);
	gc2145_write_register(ViPipe, 0xe9, 0xa0);
	gc2145_write_register(ViPipe, 0xfe, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x4f, 0x00);
	gc2145_write_register(ViPipe, 0x4f, 0x00);
	gc2145_write_register(ViPipe, 0x4b, 0x01);
	gc2145_write_register(ViPipe, 0x4f, 0x00);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x71);
	gc2145_write_register(ViPipe, 0x4e, 0x01);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x91);
	gc2145_write_register(ViPipe, 0x4e, 0x01);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x70);
	gc2145_write_register(ViPipe, 0x4e, 0x01);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x90);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xb0);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8f);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x6f);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xaf);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xd0);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xf0);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xcf);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xef);
	gc2145_write_register(ViPipe, 0x4e, 0x02);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x6e);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8e);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xae);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xce);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x4d);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x6d);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8d);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xad);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xcd);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x4c);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x6c);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8c);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xac);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xcc);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xcb);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x4b);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x6b);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8b);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xab);
	gc2145_write_register(ViPipe, 0x4e, 0x03);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8a);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xaa);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xca);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xca);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xc9);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x8a);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0x89);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xa9);
	gc2145_write_register(ViPipe, 0x4e, 0x04);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x0b);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x0a);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xeb);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x01);
	gc2145_write_register(ViPipe, 0x4d, 0xea);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x09);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x29);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x2a);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x4a);
	gc2145_write_register(ViPipe, 0x4e, 0x05);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x8a);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x49);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x69);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x89);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xa9);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x48);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x68);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0x69);
	gc2145_write_register(ViPipe, 0x4e, 0x06);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xca);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xc9);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xe9);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x03);
	gc2145_write_register(ViPipe, 0x4d, 0x09);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xc8);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xe8);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xa7);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xc7);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x02);
	gc2145_write_register(ViPipe, 0x4d, 0xe7);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4c, 0x03);
	gc2145_write_register(ViPipe, 0x4d, 0x07);
	gc2145_write_register(ViPipe, 0x4e, 0x07);
	gc2145_write_register(ViPipe, 0x4f, 0x01);
	gc2145_write_register(ViPipe, 0x50, 0x80);
	gc2145_write_register(ViPipe, 0x51, 0xa8);
	gc2145_write_register(ViPipe, 0x52, 0x47);
	gc2145_write_register(ViPipe, 0x53, 0x38);
	gc2145_write_register(ViPipe, 0x54, 0xc7);
	gc2145_write_register(ViPipe, 0x56, 0x0e);
	gc2145_write_register(ViPipe, 0x58, 0x08);
	gc2145_write_register(ViPipe, 0x5b, 0x00);
	gc2145_write_register(ViPipe, 0x5c, 0x74);
	gc2145_write_register(ViPipe, 0x5d, 0x8b);
	gc2145_write_register(ViPipe, 0x61, 0xdb);
	gc2145_write_register(ViPipe, 0x62, 0xb8);
	gc2145_write_register(ViPipe, 0x63, 0x86);
	gc2145_write_register(ViPipe, 0x64, 0xc0);
	gc2145_write_register(ViPipe, 0x65, 0x04);
	gc2145_write_register(ViPipe, 0x67, 0xa8);
	gc2145_write_register(ViPipe, 0x68, 0xb0);
	gc2145_write_register(ViPipe, 0x69, 0x00);
	gc2145_write_register(ViPipe, 0x6a, 0xa8);
	gc2145_write_register(ViPipe, 0x6b, 0xb0);
	gc2145_write_register(ViPipe, 0x6c, 0xaf);
	gc2145_write_register(ViPipe, 0x6d, 0x8b);
	gc2145_write_register(ViPipe, 0x6e, 0x50);
	gc2145_write_register(ViPipe, 0x6f, 0x18);
	gc2145_write_register(ViPipe, 0x73, 0xf0);
	gc2145_write_register(ViPipe, 0x70, 0x0d);
	gc2145_write_register(ViPipe, 0x71, 0x60);
	gc2145_write_register(ViPipe, 0x72, 0x80);
	gc2145_write_register(ViPipe, 0x74, 0x01);
	gc2145_write_register(ViPipe, 0x75, 0x01);
	gc2145_write_register(ViPipe, 0x7f, 0x0c);
	gc2145_write_register(ViPipe, 0x76, 0x70);
	gc2145_write_register(ViPipe, 0x77, 0x58);
	gc2145_write_register(ViPipe, 0x78, 0xa0);
	gc2145_write_register(ViPipe, 0x79, 0x5e);
	gc2145_write_register(ViPipe, 0x7a, 0x54);
	gc2145_write_register(ViPipe, 0x7b, 0x58);
	gc2145_write_register(ViPipe, 0xfe, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0xc0, 0x01);
	gc2145_write_register(ViPipe, 0xc1, 0x44);
	gc2145_write_register(ViPipe, 0xc2, 0xfd);
	gc2145_write_register(ViPipe, 0xc3, 0x04);
	gc2145_write_register(ViPipe, 0xc4, 0xF0);
	gc2145_write_register(ViPipe, 0xc5, 0x48);
	gc2145_write_register(ViPipe, 0xc6, 0xfd);
	gc2145_write_register(ViPipe, 0xc7, 0x46);
	gc2145_write_register(ViPipe, 0xc8, 0xfd);
	gc2145_write_register(ViPipe, 0xc9, 0x02);
	gc2145_write_register(ViPipe, 0xca, 0xe0);
	gc2145_write_register(ViPipe, 0xcb, 0x45);
	gc2145_write_register(ViPipe, 0xcc, 0xec);
	gc2145_write_register(ViPipe, 0xcd, 0x48);
	gc2145_write_register(ViPipe, 0xce, 0xf0);
	gc2145_write_register(ViPipe, 0xcf, 0xf0);
	gc2145_write_register(ViPipe, 0xe3, 0x0c);
	gc2145_write_register(ViPipe, 0xe4, 0x4b);
	gc2145_write_register(ViPipe, 0xe5, 0xe0);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x9f, 0x40);
	gc2145_write_register(ViPipe, 0xfe, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0xf2, 0x0f);

	gc2145_write_register(ViPipe, 0xfe, 0x02);
	gc2145_write_register(ViPipe, 0x40, 0xbf);
	gc2145_write_register(ViPipe, 0x46, 0xcf);
	gc2145_write_register(ViPipe, 0xfe, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x05, 0x01);
	gc2145_write_register(ViPipe, 0x06, 0x56);
	gc2145_write_register(ViPipe, 0x07, 0x00);
	gc2145_write_register(ViPipe, 0x08, 0x32);
	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x25, 0x00);
	gc2145_write_register(ViPipe, 0x26, 0xfa);
	gc2145_write_register(ViPipe, 0x27, 0x04);
	gc2145_write_register(ViPipe, 0x28, 0xe2);
	gc2145_write_register(ViPipe, 0x29, 0x06);
	gc2145_write_register(ViPipe, 0x2a, 0xd6);
	gc2145_write_register(ViPipe, 0x2b, 0x07);
	gc2145_write_register(ViPipe, 0x2c, 0xd0);
	gc2145_write_register(ViPipe, 0x2d, 0x0b);
	gc2145_write_register(ViPipe, 0x2e, 0xb8);
	gc2145_write_register(ViPipe, 0xfe, 0x00);


	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0xfd, 0x00);
	gc2145_write_register(ViPipe, 0xf8, 0x82);
	gc2145_write_register(ViPipe, 0xfa, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x90, 0x01);
	gc2145_write_register(ViPipe, 0x91, 0x00);
	gc2145_write_register(ViPipe, 0x92, 0x00);
	gc2145_write_register(ViPipe, 0x93, 0x00);
	gc2145_write_register(ViPipe, 0x94, 0x00);
	gc2145_write_register(ViPipe, 0x95, 0x04);
	gc2145_write_register(ViPipe, 0x96, 0xb0);
	gc2145_write_register(ViPipe, 0x97, 0x06);
	gc2145_write_register(ViPipe, 0x98, 0x40);
	gc2145_write_register(ViPipe, 0x99, 0x11);
	gc2145_write_register(ViPipe, 0x9a, 0x06);
	gc2145_write_register(ViPipe, 0x9b, 0x00);
	gc2145_write_register(ViPipe, 0x9c, 0x00);
	gc2145_write_register(ViPipe, 0x9d, 0x00);
	gc2145_write_register(ViPipe, 0x9e, 0x00);
	gc2145_write_register(ViPipe, 0x9f, 0x00);
	gc2145_write_register(ViPipe, 0xa0, 0x00);
	gc2145_write_register(ViPipe, 0xa1, 0x00);
	gc2145_write_register(ViPipe, 0xa2, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0xec, 0x06);
	gc2145_write_register(ViPipe, 0xed, 0x04);
	gc2145_write_register(ViPipe, 0xee, 0x60);
	gc2145_write_register(ViPipe, 0xef, 0x90);
	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x74, 0x01);

	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x01, 0x04);
	gc2145_write_register(ViPipe, 0x02, 0xc0);
	gc2145_write_register(ViPipe, 0x03, 0x04);
	gc2145_write_register(ViPipe, 0x04, 0x90);
	gc2145_write_register(ViPipe, 0x05, 0x30);
	gc2145_write_register(ViPipe, 0x06, 0x90);
	gc2145_write_register(ViPipe, 0x07, 0x30);
	gc2145_write_register(ViPipe, 0x08, 0x80);
	gc2145_write_register(ViPipe, 0x0a, 0x82);
	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x21, 0x15);
	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x20, 0x15);

	gc2145_write_register(ViPipe, 0xfe, 0x00);
	gc2145_write_register(ViPipe, 0x05, 0x01);
	gc2145_write_register(ViPipe, 0x06, 0x2f);
	gc2145_write_register(ViPipe, 0x07, 0x00);
	gc2145_write_register(ViPipe, 0x08, 0x64);
	gc2145_write_register(ViPipe, 0xfe, 0x01);
	gc2145_write_register(ViPipe, 0x25, 0x00);
	gc2145_write_register(ViPipe, 0x26, 0xa0);
	gc2145_write_register(ViPipe, 0x27, 0x05);
	gc2145_write_register(ViPipe, 0x28, 0x00);
	gc2145_write_register(ViPipe, 0x29, 0x05);
	gc2145_write_register(ViPipe, 0x2a, 0x00);
	gc2145_write_register(ViPipe, 0x2b, 0x05);
	gc2145_write_register(ViPipe, 0x2c, 0x00);
	gc2145_write_register(ViPipe, 0x2d, 0x05);
	gc2145_write_register(ViPipe, 0x2e, 0x00);

	gc2145_write_register(ViPipe, 0xfe, 0x00);

	delay_ms(100);

	printf("ViPipe:%d,===GC2145 1200P 12fps YUV Init OK!===\n", ViPipe);
}

