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
#include "gc0329_cmos_ex.h"

static void gc0329_linear_480p10_init(VI_PIPE ViPipe);

CVI_U8 gc0329_i2c_addr = 0x31;//0x78
const CVI_U32 gc0329_addr_byte = 1;
const CVI_U32 gc0329_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int gc0329_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunGc0329_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, gc0329_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int gc0329_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int gc0329_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (gc0329_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, gc0329_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, gc0329_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (gc0329_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}


int gc0329_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (gc0329_addr_byte == 1) {
		buf[idx] = addr & 0xff;
		idx++;
	}
	if (gc0329_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, gc0329_addr_byte + gc0329_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
//	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

#define GC0329_CHIP_ID_ADDR	0x00
#define GC0329_CHIP_ID		0xc0

int gc0329_probe(VI_PIPE ViPipe)
{
	int nVal;

	usleep(50);
	if (gc0329_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	gc0329_write_register(ViPipe, 0xfc, 0x16);
	nVal  = gc0329_read_register(ViPipe, GC0329_CHIP_ID_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((nVal & 0xFF) != GC0329_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}
	printf("%d\n", ViPipe);
	return CVI_SUCCESS;
}

void gc0329_init(VI_PIPE ViPipe)
{
	gc0329_i2c_init(ViPipe);

	gc0329_linear_480p10_init(ViPipe);

	g_pastGc0329[ViPipe]->bInit = CVI_TRUE;
}

void gc0329_exit(VI_PIPE ViPipe)
{
	gc0329_i2c_exit(ViPipe);
}

static void gc0329_linear_480p10_init(VI_PIPE ViPipe)
{
	gc0329_write_register(ViPipe, 0xfe, 0x80);
	gc0329_write_register(ViPipe, 0xfc, 0x16);
	gc0329_write_register(ViPipe, 0xfc, 0x16);
	gc0329_write_register(ViPipe, 0xfe, 0x00);
	gc0329_write_register(ViPipe, 0x70, 0x48);
	gc0329_write_register(ViPipe, 0x73, 0x90);
	gc0329_write_register(ViPipe, 0x74, 0x80);
	gc0329_write_register(ViPipe, 0x75, 0x80);
	gc0329_write_register(ViPipe, 0x76, 0x94);
	gc0329_write_register(ViPipe, 0x77, 0x62);
	gc0329_write_register(ViPipe, 0x78, 0x47);
	gc0329_write_register(ViPipe, 0x79, 0x40);

	gc0329_write_register(ViPipe, 0x03, 0x02);
	gc0329_write_register(ViPipe, 0x04, 0x40);

	gc0329_write_register(ViPipe, 0xfc, 0x16);
	gc0329_write_register(ViPipe, 0x09, 0x00);
	gc0329_write_register(ViPipe, 0x0a, 0x02);
	gc0329_write_register(ViPipe, 0x0b, 0x00);
	gc0329_write_register(ViPipe, 0x0c, 0x02);
	gc0329_write_register(ViPipe, 0x17, 0x14);
	gc0329_write_register(ViPipe, 0x19, 0x05);
	gc0329_write_register(ViPipe, 0x1b, 0x24);
	gc0329_write_register(ViPipe, 0x1c, 0x04);
	gc0329_write_register(ViPipe, 0x1e, 0x08);
	gc0329_write_register(ViPipe, 0x1f, 0x08);
	gc0329_write_register(ViPipe, 0x20, 0x01);
	gc0329_write_register(ViPipe, 0x21, 0x48);
	gc0329_write_register(ViPipe, 0x22, 0xba);
	gc0329_write_register(ViPipe, 0x23, 0x22);
	gc0329_write_register(ViPipe, 0x24, 0x16);

	gc0329_write_register(ViPipe, 0x26, 0xf7);
	gc0329_write_register(ViPipe, 0x28, 0x7f);
	gc0329_write_register(ViPipe, 0x29, 0x00);
	gc0329_write_register(ViPipe, 0x32, 0x00);
	gc0329_write_register(ViPipe, 0x33, 0x20);
	gc0329_write_register(ViPipe, 0x34, 0x20);
	gc0329_write_register(ViPipe, 0x35, 0x20);
	gc0329_write_register(ViPipe, 0x36, 0x20);

	gc0329_write_register(ViPipe, 0x3b, 0x04);
	gc0329_write_register(ViPipe, 0x3c, 0x04);
	gc0329_write_register(ViPipe, 0x3d, 0x04);
	gc0329_write_register(ViPipe, 0x3e, 0x04);

	gc0329_write_register(ViPipe, 0x40, 0xff);
	gc0329_write_register(ViPipe, 0x41, 0x24);
	gc0329_write_register(ViPipe, 0x42, 0xfa);
	gc0329_write_register(ViPipe, 0x46, 0x02);
	gc0329_write_register(ViPipe, 0x4b, 0xca);
	gc0329_write_register(ViPipe, 0x4d, 0x01);
	gc0329_write_register(ViPipe, 0x4f, 0x01);
	gc0329_write_register(ViPipe, 0x70, 0x48);

	gc0329_write_register(ViPipe, 0x80, 0x07);
	gc0329_write_register(ViPipe, 0x81, 0xc2);
	gc0329_write_register(ViPipe, 0x82, 0x90);
	gc0329_write_register(ViPipe, 0x83, 0x05);
	gc0329_write_register(ViPipe, 0x87, 0x40);

	gc0329_write_register(ViPipe, 0x90, 0x8c);
	gc0329_write_register(ViPipe, 0x92, 0x05);
	gc0329_write_register(ViPipe, 0x94, 0x05);
	gc0329_write_register(ViPipe, 0x95, 0x45);
	gc0329_write_register(ViPipe, 0x96, 0x88);

	gc0329_write_register(ViPipe, 0xfe, 0x01);
	gc0329_write_register(ViPipe, 0x18, 0x22);
	gc0329_write_register(ViPipe, 0xfe, 0x00);
	gc0329_write_register(ViPipe, 0x9c, 0x0a);
	gc0329_write_register(ViPipe, 0xa0, 0xaf);
	gc0329_write_register(ViPipe, 0xa2, 0xff);
	gc0329_write_register(ViPipe, 0xa4, 0x30);
	gc0329_write_register(ViPipe, 0xa5, 0x31);
	gc0329_write_register(ViPipe, 0xa7, 0x35);

	gc0329_write_register(ViPipe, 0xfe, 0x00);
	gc0329_write_register(ViPipe, 0xbf, 0x0b);
	gc0329_write_register(ViPipe, 0xc0, 0x1d);
	gc0329_write_register(ViPipe, 0xc1, 0x33);
	gc0329_write_register(ViPipe, 0xc2, 0x49);
	gc0329_write_register(ViPipe, 0xc3, 0x5d);
	gc0329_write_register(ViPipe, 0xc4, 0x6e);
	gc0329_write_register(ViPipe, 0xc5, 0x7c);
	gc0329_write_register(ViPipe, 0xc6, 0x99);
	gc0329_write_register(ViPipe, 0xc7, 0xaf);
	gc0329_write_register(ViPipe, 0xc8, 0xc2);
	gc0329_write_register(ViPipe, 0xc9, 0xd0);
	gc0329_write_register(ViPipe, 0xca, 0xda);
	gc0329_write_register(ViPipe, 0xcb, 0xe2);
	gc0329_write_register(ViPipe, 0xcc, 0xe7);
	gc0329_write_register(ViPipe, 0xcd, 0xf0);
	gc0329_write_register(ViPipe, 0xce, 0xf7);
	gc0329_write_register(ViPipe, 0xcf, 0xff);

	gc0329_write_register(ViPipe, 0xfe, 0x00);
	gc0329_write_register(ViPipe, 0x63, 0x00);
	gc0329_write_register(ViPipe, 0x64, 0x06);
	gc0329_write_register(ViPipe, 0x65, 0x0d);
	gc0329_write_register(ViPipe, 0x66, 0x1b);
	gc0329_write_register(ViPipe, 0x67, 0x2b);
	gc0329_write_register(ViPipe, 0x68, 0x3d);
	gc0329_write_register(ViPipe, 0x69, 0x50);
	gc0329_write_register(ViPipe, 0x6a, 0x60);
	gc0329_write_register(ViPipe, 0x6b, 0x80);
	gc0329_write_register(ViPipe, 0x6c, 0xa0);
	gc0329_write_register(ViPipe, 0x6d, 0xc0);
	gc0329_write_register(ViPipe, 0x6e, 0xe0);
	gc0329_write_register(ViPipe, 0x6f, 0xff);

	gc0329_write_register(ViPipe, 0xfe, 0x00);
	gc0329_write_register(ViPipe, 0xb3, 0x44);
	gc0329_write_register(ViPipe, 0xb4, 0xfd);
	gc0329_write_register(ViPipe, 0xb5, 0x02);
	gc0329_write_register(ViPipe, 0xb6, 0xfa);
	gc0329_write_register(ViPipe, 0xb7, 0x48);
	gc0329_write_register(ViPipe, 0xb8, 0xf0);
	gc0329_write_register(ViPipe, 0x50, 0x01);

	gc0329_write_register(ViPipe, 0xfe, 0x00);
	gc0329_write_register(ViPipe, 0xd0, 0x40);
	gc0329_write_register(ViPipe, 0xd1, 0x28);
	gc0329_write_register(ViPipe, 0xd2, 0x28);
	gc0329_write_register(ViPipe, 0xd3, 0x40);
	gc0329_write_register(ViPipe, 0xd5, 0x00);
	gc0329_write_register(ViPipe, 0xdd, 0x14);
	gc0329_write_register(ViPipe, 0xde, 0x34);

	gc0329_write_register(ViPipe, 0xfe, 0x01);
	gc0329_write_register(ViPipe, 0x10, 0x40);
	gc0329_write_register(ViPipe, 0x11, 0x21);
	gc0329_write_register(ViPipe, 0x12, 0x13);
	gc0329_write_register(ViPipe, 0x13, 0x50);
	gc0329_write_register(ViPipe, 0x17, 0xa8);
	gc0329_write_register(ViPipe, 0x1a, 0x21);
	gc0329_write_register(ViPipe, 0x20, 0x31);
	gc0329_write_register(ViPipe, 0x21, 0xc0);
	gc0329_write_register(ViPipe, 0x22, 0x60);
	gc0329_write_register(ViPipe, 0x3c, 0x50);
	gc0329_write_register(ViPipe, 0x3d, 0x40);
	gc0329_write_register(ViPipe, 0x3e, 0x45);

	gc0329_write_register(ViPipe, 0xfe, 0x01);
	gc0329_write_register(ViPipe, 0x06, 0x12);
	gc0329_write_register(ViPipe, 0x07, 0x06);
	gc0329_write_register(ViPipe, 0x08, 0x9c);
	gc0329_write_register(ViPipe, 0x09, 0xee);
	gc0329_write_register(ViPipe, 0x50, 0xfc);
	gc0329_write_register(ViPipe, 0x51, 0x28);
	gc0329_write_register(ViPipe, 0x52, 0x10);
	gc0329_write_register(ViPipe, 0x53, 0x20);
	gc0329_write_register(ViPipe, 0x54, 0x12);
	gc0329_write_register(ViPipe, 0x55, 0x16);
	gc0329_write_register(ViPipe, 0x56, 0x30);
	gc0329_write_register(ViPipe, 0x58, 0x60);
	gc0329_write_register(ViPipe, 0x59, 0x08);
	gc0329_write_register(ViPipe, 0x5a, 0x02);
	gc0329_write_register(ViPipe, 0x5b, 0x63);
	gc0329_write_register(ViPipe, 0x5c, 0x35);
	gc0329_write_register(ViPipe, 0x5d, 0x72);
	gc0329_write_register(ViPipe, 0x5e, 0x11);
	gc0329_write_register(ViPipe, 0x5f, 0x40);
	gc0329_write_register(ViPipe, 0x60, 0x40);
	gc0329_write_register(ViPipe, 0x61, 0xc8);
	gc0329_write_register(ViPipe, 0x62, 0xa0);
	gc0329_write_register(ViPipe, 0x63, 0x40);
	gc0329_write_register(ViPipe, 0x64, 0x50);
	gc0329_write_register(ViPipe, 0x65, 0x98);
	gc0329_write_register(ViPipe, 0x66, 0xfa);
	gc0329_write_register(ViPipe, 0x67, 0x80);
	gc0329_write_register(ViPipe, 0x68, 0x60);
	gc0329_write_register(ViPipe, 0x69, 0x90);
	gc0329_write_register(ViPipe, 0x6a, 0x40);
	gc0329_write_register(ViPipe, 0x6b, 0x39);
	gc0329_write_register(ViPipe, 0x6c, 0x30);
	gc0329_write_register(ViPipe, 0x6d, 0x60);
	gc0329_write_register(ViPipe, 0x6e, 0x41);
	gc0329_write_register(ViPipe, 0x70, 0x10);
	gc0329_write_register(ViPipe, 0x71, 0x00);
	gc0329_write_register(ViPipe, 0x72, 0x10);
	gc0329_write_register(ViPipe, 0x73, 0x40);
	gc0329_write_register(ViPipe, 0x80, 0x60);
	gc0329_write_register(ViPipe, 0x81, 0x50);
	gc0329_write_register(ViPipe, 0x82, 0x42);
	gc0329_write_register(ViPipe, 0x83, 0x40);
	gc0329_write_register(ViPipe, 0x84, 0x40);
	gc0329_write_register(ViPipe, 0x85, 0x40);
	gc0329_write_register(ViPipe, 0x74, 0x40);
	gc0329_write_register(ViPipe, 0x75, 0x58);
	gc0329_write_register(ViPipe, 0x76, 0x24);
	gc0329_write_register(ViPipe, 0x77, 0x40);
	gc0329_write_register(ViPipe, 0x78, 0x20);
	gc0329_write_register(ViPipe, 0x79, 0x60);
	gc0329_write_register(ViPipe, 0x7a, 0x58);
	gc0329_write_register(ViPipe, 0x7b, 0x20);
	gc0329_write_register(ViPipe, 0x7c, 0x30);
	gc0329_write_register(ViPipe, 0x7d, 0x35);
	gc0329_write_register(ViPipe, 0x7e, 0x10);
	gc0329_write_register(ViPipe, 0x7f, 0x08);

	gc0329_write_register(ViPipe, 0x9c, 0x00);
	gc0329_write_register(ViPipe, 0x9e, 0xc0);
	gc0329_write_register(ViPipe, 0x9f, 0x40);

	gc0329_write_register(ViPipe, 0xd0, 0x00);
	gc0329_write_register(ViPipe, 0xd2, 0x2c);
	gc0329_write_register(ViPipe, 0xd3, 0x80);

	gc0329_write_register(ViPipe, 0xfe, 0x01);
	gc0329_write_register(ViPipe, 0xc0, 0x0b);
	gc0329_write_register(ViPipe, 0xc1, 0x07);
	gc0329_write_register(ViPipe, 0xc2, 0x05);
	gc0329_write_register(ViPipe, 0xc6, 0x0b);
	gc0329_write_register(ViPipe, 0xc7, 0x07);
	gc0329_write_register(ViPipe, 0xc8, 0x05);
	gc0329_write_register(ViPipe, 0xba, 0x39);
	gc0329_write_register(ViPipe, 0xbb, 0x24);
	gc0329_write_register(ViPipe, 0xbc, 0x23);
	gc0329_write_register(ViPipe, 0xb4, 0x39);
	gc0329_write_register(ViPipe, 0xb5, 0x24);
	gc0329_write_register(ViPipe, 0xb6, 0x23);
	gc0329_write_register(ViPipe, 0xc3, 0x00);
	gc0329_write_register(ViPipe, 0xc4, 0x00);
	gc0329_write_register(ViPipe, 0xc5, 0x00);
	gc0329_write_register(ViPipe, 0xc9, 0x00);
	gc0329_write_register(ViPipe, 0xca, 0x00);
	gc0329_write_register(ViPipe, 0xcb, 0x00);
	gc0329_write_register(ViPipe, 0xbd, 0x2b);
	gc0329_write_register(ViPipe, 0xbe, 0x00);
	gc0329_write_register(ViPipe, 0xbf, 0x00);
	gc0329_write_register(ViPipe, 0xb7, 0x09);
	gc0329_write_register(ViPipe, 0xb8, 0x00);
	gc0329_write_register(ViPipe, 0xb9, 0x00);
	gc0329_write_register(ViPipe, 0xa8, 0x31);
	gc0329_write_register(ViPipe, 0xa9, 0x23);
	gc0329_write_register(ViPipe, 0xaa, 0x20);
	gc0329_write_register(ViPipe, 0xab, 0x31);
	gc0329_write_register(ViPipe, 0xac, 0x23);
	gc0329_write_register(ViPipe, 0xad, 0x20);
	gc0329_write_register(ViPipe, 0xae, 0x31);
	gc0329_write_register(ViPipe, 0xaf, 0x23);
	gc0329_write_register(ViPipe, 0xb0, 0x20);
	gc0329_write_register(ViPipe, 0xb1, 0x31);
	gc0329_write_register(ViPipe, 0xb2, 0x23);
	gc0329_write_register(ViPipe, 0xb3, 0x20);
	gc0329_write_register(ViPipe, 0xa4, 0x00);
	gc0329_write_register(ViPipe, 0xa5, 0x00);
	gc0329_write_register(ViPipe, 0xa6, 0x00);
	gc0329_write_register(ViPipe, 0xa7, 0x00);
	gc0329_write_register(ViPipe, 0xa1, 0x3c);
	gc0329_write_register(ViPipe, 0xa2, 0x50);
	gc0329_write_register(ViPipe, 0xfe, 0x00);

	gc0329_write_register(ViPipe, 0x05, 0x02);
	gc0329_write_register(ViPipe, 0x06, 0x2c);
	gc0329_write_register(ViPipe, 0x07, 0x00);
	gc0329_write_register(ViPipe, 0x08, 0xb8);
	gc0329_write_register(ViPipe, 0xfe, 0x01);
	gc0329_write_register(ViPipe, 0x29, 0x00);
	gc0329_write_register(ViPipe, 0x2a, 0x60);
	gc0329_write_register(ViPipe, 0x2b, 0x02);
	gc0329_write_register(ViPipe, 0x2c, 0xa0);
	gc0329_write_register(ViPipe, 0x2d, 0x03);
	gc0329_write_register(ViPipe, 0x2e, 0x00);
	gc0329_write_register(ViPipe, 0x2f, 0x03);
	gc0329_write_register(ViPipe, 0x30, 0xc0);
	gc0329_write_register(ViPipe, 0x31, 0x05);
	gc0329_write_register(ViPipe, 0x32, 0x40);
	gc0329_write_register(ViPipe, 0xfe, 0x00);

	gc0329_write_register(ViPipe, 0x44, 0xa2);
	gc0329_write_register(ViPipe, 0xf0, 0x07);
	gc0329_write_register(ViPipe, 0xf1, 0x01);

	 delay_ms(50);

	printf("ViPipe:%d,===GC0329 480P 10fps YUV Init OK!===\n", ViPipe);
}

