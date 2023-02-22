#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "n5_cmos_ex.h"
#include <pthread.h>
#include <signal.h>

const CVI_U8 n5_i2c_addr = 0x32;        /* I2C slave address of N5, SA0=0:0x32, SA0=1:0x33*/
const CVI_U32 n5_addr_byte = 1;
const CVI_U32 n5_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};
static pthread_t g_n5_thid;
unsigned char chn_mode[2] = {0xEE, 0xEE};

#define N5_TEST_PATTERN 1

int n5_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunN5_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
	syslog(LOG_DEBUG, "open %s\n", acDevFile);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, n5_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int n5_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int n5_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return 0;

	if (n5_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, n5_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, n5_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (n5_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int n5_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (n5_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (n5_data_byte == 2)
		buf[idx++] = (data >> 8) & 0xff;

	// add data byte 0
	buf[idx++] = data & 0xff;

	ret = write(g_fd[ViPipe], buf, n5_addr_byte + n5_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);

#if 0 // read back checing
	ret = n5_read_register(ViPipe, addr);
	if (ret != data)
		syslog(LOG_DEBUG, "i2c readback-check fail, 0x%x != 0x%x\n", ret, data);
#endif
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void n5_common_setting(VI_PIPE ViPipe)
{
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x00, 0x10);
	n5_write_register(ViPipe, 0x01, 0x10);
	n5_write_register(ViPipe, 0x18, 0x3f);
	n5_write_register(ViPipe, 0x19, 0x3f);
	n5_write_register(ViPipe, 0x22, 0x0b);
	n5_write_register(ViPipe, 0x23, 0x41);
	n5_write_register(ViPipe, 0x26, 0x0b);
	n5_write_register(ViPipe, 0x27, 0x41);
	n5_write_register(ViPipe, 0x54, 0x00);
	n5_write_register(ViPipe, 0xa0, 0x05);
	n5_write_register(ViPipe, 0xa1, 0x05);
	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x97, 0x00);
	n5_write_register(ViPipe, 0x97, 0x0f);
	n5_write_register(ViPipe, 0x7A, 0x0f);
	n5_write_register(ViPipe, 0xff, 0x05);
	n5_write_register(ViPipe, 0x00, 0xd0);
	n5_write_register(ViPipe, 0x01, 0x22);
	n5_write_register(ViPipe, 0x05, 0x04);
	n5_write_register(ViPipe, 0x08, 0x55);
	n5_write_register(ViPipe, 0x1b, 0x08);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x28, 0x80);
	n5_write_register(ViPipe, 0x2f, 0x00);
	n5_write_register(ViPipe, 0x30, 0xe0);
	n5_write_register(ViPipe, 0x31, 0x43);
	n5_write_register(ViPipe, 0x32, 0xa2);
	n5_write_register(ViPipe, 0x57, 0x00);
	n5_write_register(ViPipe, 0x58, 0x77);
	n5_write_register(ViPipe, 0x5b, 0x41);
	n5_write_register(ViPipe, 0x5c, 0x7C);
	n5_write_register(ViPipe, 0x5f, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0x7c, 0x01);
	n5_write_register(ViPipe, 0x7d, 0x80);
	n5_write_register(ViPipe, 0x80, 0x00);
	n5_write_register(ViPipe, 0x90, 0x01);
	n5_write_register(ViPipe, 0xa9, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
	n5_write_register(ViPipe, 0xb9, 0x72);
	n5_write_register(ViPipe, 0xd1, 0x00);
	n5_write_register(ViPipe, 0xd5, 0x80);
	n5_write_register(ViPipe, 0xff, 0x06);
	n5_write_register(ViPipe, 0x00, 0xd0);
	n5_write_register(ViPipe, 0x01, 0x22);
	n5_write_register(ViPipe, 0x05, 0x04);
	n5_write_register(ViPipe, 0x08, 0x55);
	n5_write_register(ViPipe, 0x1b, 0x08);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x28, 0x80);
	n5_write_register(ViPipe, 0x2f, 0x00);
	n5_write_register(ViPipe, 0x30, 0xe0);
	n5_write_register(ViPipe, 0x31, 0x43);
	n5_write_register(ViPipe, 0x32, 0xa2);
	n5_write_register(ViPipe, 0x57, 0x00);
	n5_write_register(ViPipe, 0x58, 0x77);
	n5_write_register(ViPipe, 0x5b, 0x41);
	n5_write_register(ViPipe, 0x5c, 0x7C);
	n5_write_register(ViPipe, 0x5f, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0x7c, 0x01);
	n5_write_register(ViPipe, 0x7d, 0x80);
	n5_write_register(ViPipe, 0x80, 0x00);
	n5_write_register(ViPipe, 0x90, 0x01);
	n5_write_register(ViPipe, 0xa9, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
	n5_write_register(ViPipe, 0xb9, 0x72);
	n5_write_register(ViPipe, 0xd1, 0x00);
	n5_write_register(ViPipe, 0xd5, 0x80);
	n5_write_register(ViPipe, 0xff, 0x09);
	n5_write_register(ViPipe, 0x50, 0x30);
	n5_write_register(ViPipe, 0x51, 0x6f);
	n5_write_register(ViPipe, 0x52, 0x67);
	n5_write_register(ViPipe, 0x53, 0x48);
	n5_write_register(ViPipe, 0x54, 0x30);
	n5_write_register(ViPipe, 0x55, 0x6f);
	n5_write_register(ViPipe, 0x56, 0x67);
	n5_write_register(ViPipe, 0x57, 0x48);
	n5_write_register(ViPipe, 0x96, 0x00);
	n5_write_register(ViPipe, 0x9e, 0x00);
	n5_write_register(ViPipe, 0xb6, 0x00);
	n5_write_register(ViPipe, 0xbe, 0x00);
	n5_write_register(ViPipe, 0xff, 0x0a);
	n5_write_register(ViPipe, 0x25, 0x10);
	n5_write_register(ViPipe, 0x27, 0x1e);
	n5_write_register(ViPipe, 0x30, 0xac);
	n5_write_register(ViPipe, 0x31, 0x78);
	n5_write_register(ViPipe, 0x32, 0x17);
	n5_write_register(ViPipe, 0x33, 0xc1);
	n5_write_register(ViPipe, 0x34, 0x40);
	n5_write_register(ViPipe, 0x35, 0x00);
	n5_write_register(ViPipe, 0x36, 0xc3);
	n5_write_register(ViPipe, 0x37, 0x0a);
	n5_write_register(ViPipe, 0x38, 0x00);
	n5_write_register(ViPipe, 0x39, 0x02);
	n5_write_register(ViPipe, 0x3a, 0x00);
	n5_write_register(ViPipe, 0x3b, 0xb2);
	n5_write_register(ViPipe, 0xa5, 0x10);
	n5_write_register(ViPipe, 0xa7, 0x1e);
	n5_write_register(ViPipe, 0xb0, 0xac);
	n5_write_register(ViPipe, 0xb1, 0x78);
	n5_write_register(ViPipe, 0xb2, 0x17);
	n5_write_register(ViPipe, 0xb3, 0xc1);
	n5_write_register(ViPipe, 0xb4, 0x40);
	n5_write_register(ViPipe, 0xb5, 0x00);
	n5_write_register(ViPipe, 0xb6, 0xc3);
	n5_write_register(ViPipe, 0xb7, 0x0a);
	n5_write_register(ViPipe, 0xb8, 0x00);
	n5_write_register(ViPipe, 0xb9, 0x02);
	n5_write_register(ViPipe, 0xba, 0x00);
	n5_write_register(ViPipe, 0xbb, 0xb2);
	n5_write_register(ViPipe, 0x77, 0x8F);
	n5_write_register(ViPipe, 0xF7, 0x8F);
	n5_write_register(ViPipe, 0xff, 0x13);
	n5_write_register(ViPipe, 0x07, 0x47);
	n5_write_register(ViPipe, 0x12, 0x04);
	n5_write_register(ViPipe, 0x1e, 0x1f);
	n5_write_register(ViPipe, 0x1f, 0x27);
	n5_write_register(ViPipe, 0x2e, 0x10);
	n5_write_register(ViPipe, 0x2f, 0xc8);
	n5_write_register(ViPipe, 0x30, 0x00);
	n5_write_register(ViPipe, 0x31, 0xff);
	n5_write_register(ViPipe, 0x32, 0x00);
	n5_write_register(ViPipe, 0x33, 0x00);
	n5_write_register(ViPipe, 0x3a, 0xff);
	n5_write_register(ViPipe, 0x3b, 0xff);
	n5_write_register(ViPipe, 0x3c, 0xff);
	n5_write_register(ViPipe, 0x3d, 0xff);
	n5_write_register(ViPipe, 0x3e, 0xff);
	n5_write_register(ViPipe, 0x3f, 0x0f);
	n5_write_register(ViPipe, 0x70, 0x00);
	n5_write_register(ViPipe, 0x72, 0x05);
	n5_write_register(ViPipe, 0x7A, 0xf0);
	n5_write_register(ViPipe, 0xff, 0x00); //8x8 color block test pattern
	n5_write_register(ViPipe, 0x78, 0xba);
	n5_write_register(ViPipe, 0xff, 0x05);
	n5_write_register(ViPipe, 0x2c, 0x08);
	n5_write_register(ViPipe, 0x6a, 0x80);
	n5_write_register(ViPipe, 0xff, 0x06);
	n5_write_register(ViPipe, 0x2c, 0x08);
	n5_write_register(ViPipe, 0x6a, 0x80);
}

void n5_set_chn_720h_ntsc(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0xa0);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x60);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)|(0x10<<chn));
	n5_write_register(ViPipe, 0x18+chn, 0x08);
	n5_write_register(ViPipe, 0x58+chn, 0x90);
	n5_write_register(ViPipe, 0x5c+chn, 0xbc);
	n5_write_register(ViPipe, 0x64+chn, 0xa1);
	n5_write_register(ViPipe, 0x89+chn, 0x10);
	n5_write_register(ViPipe, chn+0x8e, 0x2f);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x06);
	n5_write_register(ViPipe, 0x8c+chn, 0x86);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x90);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0x78);
	n5_write_register(ViPipe, 0x47, 0x04);
	n5_write_register(ViPipe, 0x50, 0x84);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0xb9);
}
void n5_set_chn_720h_pal(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0xdd);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x70);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x08);
	n5_write_register(ViPipe, 0x58+chn, 0x80);
	n5_write_register(ViPipe, 0x5c+chn, 0xbe);
	n5_write_register(ViPipe, 0x64+chn, 0xa0);
	n5_write_register(ViPipe, 0x89+chn, 0x10);
	n5_write_register(ViPipe, chn+0x8e, 0x2e);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x06);
	n5_write_register(ViPipe, 0x8c+chn, 0x86);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x90);
	n5_write_register(ViPipe, 0x25, 0xcc);
	n5_write_register(ViPipe, 0x2b, 0x78);
	n5_write_register(ViPipe, 0x47, 0x04);
	n5_write_register(ViPipe, 0x50, 0x84);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0xb9);
}

void n5_set_chn_1920h_ntsc(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0xa0);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0xa0);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)|(0x10<<chn));
	n5_write_register(ViPipe, 0x18+chn, 0x08);
	n5_write_register(ViPipe, 0x58+chn, 0x2a);
	n5_write_register(ViPipe, 0x5c+chn, 0xbc);
	n5_write_register(ViPipe, 0x64+chn, 0x81);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x06);
	n5_write_register(ViPipe, 0x8c+chn, 0x06);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x90);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0x78);
	n5_write_register(ViPipe, 0x47, 0x04);
	n5_write_register(ViPipe, 0x50, 0x84);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0xb9);
}
void n5_set_chn_1920h_pal(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0xdd);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0xb0);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x08);
	n5_write_register(ViPipe, 0x58+chn, 0x42);
	n5_write_register(ViPipe, 0x5c+chn, 0xbe);
	n5_write_register(ViPipe, 0x64+chn, 0xa0);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x06);
	n5_write_register(ViPipe, 0x8c+chn, 0x06);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x90);
	n5_write_register(ViPipe, 0x25, 0xcc);
	n5_write_register(ViPipe, 0x2b, 0x78);
	n5_write_register(ViPipe, 0x47, 0x04);
	n5_write_register(ViPipe, 0x50, 0x84);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0xb9);
}


void n5_set_chn_720p_30(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x06);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x80);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x01);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x08);
	n5_write_register(ViPipe, 0x8c+chn, 0x08);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_720p_25(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x07);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x80);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x01);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x08);
	n5_write_register(ViPipe, 0x8c+chn, 0x08);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_1080p_30(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x02);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x78);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x01);
	n5_write_register(ViPipe, 0x89+chn, 0x10);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x00);
	n5_write_register(ViPipe, 0x8c+chn, 0x40);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0xb8, 0x39);

	//n5_write_register(ViPipe, 0xff, 0x13);
	//n5_write_register(ViPipe, 0x70, n5_read_register(ViPipe, 0x70)|(0x01<<chn));

}

void n5_set_chn_1080p_25(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x03);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x82);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x01);
	n5_write_register(ViPipe, 0x89+chn, 0x10);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x00);
	n5_write_register(ViPipe, 0x8c+chn, 0x40);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_1080p_60(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x02);
	n5_write_register(ViPipe, 0x85+chn, 0x0b);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x78);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x01);
	n5_write_register(ViPipe, 0x89+chn, 0x10);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x04);
	n5_write_register(ViPipe, 0x8c+chn, 0x44);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)|(0x01<<chn));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);

}

void n5_set_chn_1080p_50(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x03);
	n5_write_register(ViPipe, 0x85+chn, 0x0b);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x82);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x04);
	n5_write_register(ViPipe, 0x8c+chn, 0x44);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)|(0x01<<chn));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_960p_30(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x06);
	n5_write_register(ViPipe, 0x85+chn, 0x0b);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x48);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x00);
	n5_write_register(ViPipe, 0x8c+chn, 0x40);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_960p_25(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x07);
	n5_write_register(ViPipe, 0x85+chn, 0x0b);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x3f);
	n5_write_register(ViPipe, 0x58+chn, 0x40);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x00);
	n5_write_register(ViPipe, 0x8c+chn, 0x40);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)&(~(0x01<<chn)));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x00);
	n5_write_register(ViPipe, 0x7b, 0x11);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_2k_30(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x0e);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x1f);
	n5_write_register(ViPipe, 0x58+chn, 0x82);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x04);
	n5_write_register(ViPipe, 0x8c+chn, 0x44);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)|(0x01<<chn));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x10);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
}


void n5_set_chn_2k_25(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x0f);
	n5_write_register(ViPipe, 0x85+chn, 0x00);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x1f);
	n5_write_register(ViPipe, 0x58+chn, 0x84);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x04);
	n5_write_register(ViPipe, 0x8c+chn, 0x44);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)|(0x01<<chn));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x10);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chn_4k_15(VI_PIPE ViPipe, CVI_U8 chn)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "%s chn=%d\n", __func__, chn);
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x02);
	n5_write_register(ViPipe, 0x85+chn, 0x08);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x1f);
	n5_write_register(ViPipe, 0x58+chn, 0x82);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x04);
	n5_write_register(ViPipe, 0x8c+chn, 0x44);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)|(0x01<<chn));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x10);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
}


void n5_set_chn_4k_12_5(VI_PIPE ViPipe, CVI_U8 chn)
{
	n5_write_register(ViPipe, 0xff, 0x00);
	n5_write_register(ViPipe, 0x08+chn, 0x00);
	n5_write_register(ViPipe, 0x34+chn, 0x00);
	n5_write_register(ViPipe, 0x81+chn, 0x01);
	n5_write_register(ViPipe, 0x85+chn, 0x08);
	n5_write_register(ViPipe, 0x54, n5_read_register(ViPipe, 0x54)&(~(0x10<<chn)));
	n5_write_register(ViPipe, 0x18+chn, 0x1f);
	n5_write_register(ViPipe, 0x58+chn, 0x7a);
	n5_write_register(ViPipe, 0x5c+chn, 0x80);
	n5_write_register(ViPipe, 0x64+chn, 0x00);
	n5_write_register(ViPipe, 0x89+chn, 0x00);
	n5_write_register(ViPipe, chn+0x8e, 0x00);
	n5_write_register(ViPipe, 0x30+chn, 0x12);
	n5_write_register(ViPipe, 0xa0+chn, 0x05);

	n5_write_register(ViPipe, 0xff, 0x01);
	n5_write_register(ViPipe, 0x84+chn, 0x04);
	n5_write_register(ViPipe, 0x8c+chn, 0x44);
	n5_write_register(ViPipe, 0xed, n5_read_register(ViPipe, 0xed)|(0x01<<chn));

	n5_write_register(ViPipe, 0xff, 0x05+chn);
	n5_write_register(ViPipe, 0x20, 0x84);
	n5_write_register(ViPipe, 0x25, 0xdc);
	n5_write_register(ViPipe, 0x2b, 0xa8);
	n5_write_register(ViPipe, 0x47, 0xee);
	n5_write_register(ViPipe, 0x50, 0xc6);
	n5_write_register(ViPipe, 0x69, 0x10);
	n5_write_register(ViPipe, 0x7b, 0x00);
	n5_write_register(ViPipe, 0xb8, 0x39);
}

void n5_set_chnmode(VI_PIPE ViPipe, CVI_U8 chn, CVI_U8 u8ImgMode)
{
	switch (u8ImgMode) {
	case N5_MODE_H720_NT:
		n5_set_chn_720h_ntsc(ViPipe, chn);
		break;
	case N5_MODE_H720_PAL:
		n5_set_chn_720h_pal(ViPipe, chn);
		break;
	case N5_MODE_720P_30P:
		n5_set_chn_720p_30(ViPipe, chn);
		break;
	case N5_MODE_720P_25P:
		n5_set_chn_720p_25(ViPipe, chn);
		break;
	case N5_MODE_1080P_30P:
		n5_set_chn_1080p_30(ViPipe, chn);
		break;
	case N5_MODE_1080P_25P:
		n5_set_chn_1080p_25(ViPipe, chn);
		break;
	case N5_MODE_960P_30P:
		n5_set_chn_960p_30(ViPipe, chn);
		break;
	case N5_MODE_960P_25P:
		n5_set_chn_960p_25(ViPipe, chn);
		break;
	case N5_MODE_4M_30P:
		n5_set_chn_2k_30(ViPipe, chn);
		break;
	case N5_MODE_4M_25P:
		n5_set_chn_2k_25(ViPipe, chn);
		break;
	case N5_MODE_8M_15P:
		n5_set_chn_4k_15(ViPipe, chn);
		break;
	case N5_MODE_8M_12_5P:
		n5_set_chn_4k_12_5(ViPipe, chn);
		break;
	default:
		n5_set_chn_1080p_25(ViPipe, chn);
		break;
	}
	chn_mode[chn] = u8ImgMode;
}

//port:0 or 1
//muxmode: 1->1mux(default), 2->2mux
void n5_set_portmode(VI_PIPE ViPipe, CVI_U8 port, CVI_U8 muxmode, CVI_U8 is_bt601)
{
	CVI_U8 val_1xc8, val_1xca, val_0x54;
	// add delay for MUX2
	CVI_U8 clk_freq_array[4] = {0x83, 0x03, 0x43, 0x63}; //clk_freq: 0~3:37.125M/74.25M/148.5M/297M
	//CVI_U8 clk_freq_array[4] = {0x83, 0x03, 0x4f, 0x63}; //clk_freq: 0~3:37.125M/74.25M/148.5M/297M

	n5_write_register(ViPipe, 0xff, 0x00);
	val_0x54 = n5_read_register(ViPipe, 0x54);

	if ((muxmode == N5_OUTMODE_2MUX_SD) ||
	    (muxmode == N5_OUTMODE_2MUX_HD) ||
	    (muxmode == N5_OUTMODE_2MUX_FHD) ||
	    (muxmode == N5_OUTMODE_2MUX_BT1120S_720P) ||
	    (muxmode == N5_OUTMODE_2MUX_BT1120S_1080P))
		val_0x54 |= 0x01;
	else
		val_0x54 &= 0xFE;

	n5_write_register(ViPipe, 0x54, val_0x54);
	n5_write_register(ViPipe, 0xff, 0x01);
	val_1xc8 = n5_read_register(ViPipe, 0xc8);
	switch (muxmode) {
	case N5_OUTMODE_1MUX_SD:
		n5_write_register(ViPipe, 0xA0+port, 0x00);
		n5_write_register(ViPipe, 0xC0, 0x00);
		n5_write_register(ViPipe, 0xC2, 0x11);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[0]);
		break;
	case N5_OUTMODE_1MUX_HD:
		n5_write_register(ViPipe, 0xA0+port, 0x00);
		n5_write_register(ViPipe, 0xC0, 0x00);
		n5_write_register(ViPipe, 0xC2, 0x11);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[1]);
		break;
	case N5_OUTMODE_1MUX_FHD:
		n5_write_register(ViPipe, 0xA0+port, 0x00);
		n5_write_register(ViPipe, 0xC0, 0x00);
		n5_write_register(ViPipe, 0xC2, 0x11);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[2]);
		break;
	case N5_OUTMODE_1MUX_FHD_HALF:
		n5_write_register(ViPipe, 0xA0+port, 0x00);
		n5_write_register(ViPipe, 0xC0, 0x88);
		n5_write_register(ViPipe, 0xC2, 0x99);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[1]);
		break;
	case N5_OUTMODE_2MUX_SD:
		n5_write_register(ViPipe, 0xA0+port, 0x20);
		n5_write_register(ViPipe, 0xC0, 0x10);
		n5_write_register(ViPipe, 0xC2, 0x10);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		val_1xc8 |= (port == 1?0x20:0x02);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[1]);
		break;
	case N5_OUTMODE_2MUX_HD:
		n5_write_register(ViPipe, 0xA0+port, 0x20);
		n5_write_register(ViPipe, 0xC0, 0x10);
		n5_write_register(ViPipe, 0xC2, 0x10);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		val_1xc8 |= (port == 1?0x20:0x02);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[2]);
		break;
	case N5_OUTMODE_2MUX_FHD:
		n5_write_register(ViPipe, 0xA0+port, 0x00);
		n5_write_register(ViPipe, 0xC0, 0x10);
		n5_write_register(ViPipe, 0xC2, 0x10);
		val_1xc8 &= (port == 1?0x0F:0xF0);
		val_1xc8 |= (port == 1?0x20:0x02);
		n5_write_register(ViPipe, 0xC8, val_1xc8);
		n5_write_register(ViPipe, 0xCC+port, clk_freq_array[2]);
		break;
	case N5_OUTMODE_1MUX_BT1120S:
		n5_write_register(ViPipe, 0xA0, 0x00);
		n5_write_register(ViPipe, 0xA1, 0x00);
		n5_write_register(ViPipe, 0xC0, 0xCC);
		n5_write_register(ViPipe, 0xC1, 0xCC);
		n5_write_register(ViPipe, 0xC2, 0x44);
		n5_write_register(ViPipe, 0xC3, 0x44);
		n5_write_register(ViPipe, 0xC8, 0x00);
		n5_write_register(ViPipe, 0xCA, 0x33); //two ports are enabled
		n5_write_register(ViPipe, 0xCC, clk_freq_array[2]);
		break;
	case N5_OUTMODE_2MUX_BT1120S_720P:
		n5_write_register(ViPipe, 0xA0, 0x00);
		n5_write_register(ViPipe, 0xA1, 0x00);
		n5_write_register(ViPipe, 0xC0, 0xDC);  //C data
		n5_write_register(ViPipe, 0xC1, 0xDC);
		n5_write_register(ViPipe, 0xC2, 0x54); //Y data
		n5_write_register(ViPipe, 0xC3, 0x54);
		n5_write_register(ViPipe, 0xC8, 0x22);
		n5_write_register(ViPipe, 0xCA, 0x33); //two ports are enabled
		n5_write_register(ViPipe, 0xCC, clk_freq_array[1]);
		break;
	case N5_OUTMODE_2MUX_BT1120S_1080P:
		n5_write_register(ViPipe, 0xA0, 0x20);
		n5_write_register(ViPipe, 0xA1, 0x20);
		n5_write_register(ViPipe, 0xC0, 0xDC);  //C data
		n5_write_register(ViPipe, 0xC1, 0xDC);
		n5_write_register(ViPipe, 0xC2, 0x54); //Y data
		n5_write_register(ViPipe, 0xC3, 0x54);
		n5_write_register(ViPipe, 0xC8, 0x22);
		n5_write_register(ViPipe, 0xCA, 0x33); //two ports are enabled
		n5_write_register(ViPipe, 0xCC, clk_freq_array[1]);
		break;
	}
	if (is_bt601 == 1) {
		n5_write_register(ViPipe, 0xA8+port, 0x90+(port*0x10));	//h/v0 sync enabled
//		n5_write_register(ViPipe, 0xA9, 0xA0); //h/v1 sync enabled
//		n5_write_register(ViPipe, 0xBC, 0x10); //h/v0 swap enabled
//		n5_write_register(ViPipe, 0xBD, 0x10);
//		n5_write_register(ViPipe, 0xBE, 0x10); //h/v1 swap enabled
//		n5_write_register(ViPipe, 0xBF, 0x10);
	} else {
		n5_write_register(ViPipe, 0xA8, 0x00);
//		n5_write_register(ViPipe, 0xA9, 0x00); //h/v sync disable.
	}

	if (muxmode == N5_OUTMODE_2MUX_BT1120S_720P) {
		n5_write_register(ViPipe, 0xE4, 0x11);
		n5_write_register(ViPipe, 0xE5, 0x11);
	} else {
		n5_write_register(ViPipe, 0xE4, 0x00);
		n5_write_register(ViPipe, 0xE5, 0x00);
	}

	val_1xca = n5_read_register(ViPipe, 0xca);
	val_1xca |= (0x11<<port); //enable port
	n5_write_register(ViPipe, 0xCA, val_1xca);
}

#if (!N5_TEST_PATTERN)
void n5_device_auto_detect(VI_PIPE ViPipe)
{
	//CVI_U8 vloss=0xFF;
	CVI_U8 val_5xf0 = 0xEE, val_13x70;
	CVI_U8 pre_vfc[2] = {0xEE, 0xEE};
	CVI_U8 ch = 0, val_0xa8, ch_prevloss[2] = {0x01, 0x01}, ch_vloss[2] = {0xff, 0xff};

#define NC_VIVO_CH_FORMATDEF_UNKNOWN 0
	while (1) {
		//standard rutine of auto detection
		n5_write_register(ViPipe, 0xff, 0x00);
		val_0xa8 = n5_read_register(ViPipe,  0xa8);
		for (ch = 0; ch < 2; ch++) {
			ch_vloss[ch] = (val_0xa8 >> ch) & 0x01;
			n5_write_register(ViPipe, 0xff, 0x05+ch);
			val_5xf0 = n5_read_register(ViPipe,  0xf0);
			//if(0xFF == val_5xf0 || (0x0F == (val_5xf0&0x0F)) || (0xF0 == (val_5xf0&0xF0)) ) //no video
			if (ch_vloss[ch] == 0 && ch_prevloss[ch] == 1) {
				if (val_5xf0 == 0xFF) {
					//printk("1pre_vfc[%d]=%2x val_5xf0=%2x\n",ch, pre_vfc[ch], val_5xf0);
					if (chn_mode[ch] != NC_VIVO_CH_FORMATDEF_UNKNOWN) {
						n5_set_chnmode(ViPipe, ch, NC_VIVO_CH_FORMATDEF_UNKNOWN);
						n5_write_register(ViPipe, 0xff, 0x05+ch);
						n5_write_register(ViPipe, 0xB8, 0xB8);
						n5_write_register(ViPipe, 0xff, 0x13);
						val_13x70 = n5_read_register(ViPipe,  0x70);
						val_13x70 &= (~(0x01<<ch));
						n5_write_register(ViPipe, 0x70, val_13x70);
						pre_vfc[ch] = val_5xf0;
					}
				} else if (val_5xf0 != pre_vfc[ch]) {
					//printk("2pre_vfc[%d]=%2x val_5xf0=%2x\n",ch, pre_vfc[ch], val_5xf0);
					n5_write_register(ViPipe, 0xff, 0x13);
					n5_write_register(ViPipe, 0x70,
							  n5_read_register(ViPipe, 0x70)|(0x01<<ch));
					//msleep(100);
					n5_write_register(ViPipe, 0x71,
							  n5_read_register(ViPipe, 0x71)|(0x01<<ch));

					switch (val_5xf0) {
					case 0x00:
						n5_set_chnmode(ViPipe, ch, N5_MODE_H720_NT);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_SD, 0);
						break;
					case 0x10:
						n5_set_chnmode(ViPipe, ch, N5_MODE_H720_PAL);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_SD, 0);
						break;
					case 0x20:
						n5_set_chnmode(ViPipe, ch, N5_MODE_720P_30P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_HD, 0);
						break;
					case 0x21:
						n5_set_chnmode(ViPipe, ch, N5_MODE_720P_25P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_HD, 0);
						break;
					case 0x30:
						n5_set_chnmode(ViPipe, ch, N5_MODE_1080P_30P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x31:
						n5_set_chnmode(ViPipe, ch, N5_MODE_1080P_25P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x38:
						n5_set_chnmode(ViPipe, ch, N5_MODE_1080P_60P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x39:
						n5_set_chnmode(ViPipe, ch, N5_MODE_1080P_50P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x50:
						n5_set_chnmode(ViPipe, ch, N5_MODE_4M_30P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x51:
						n5_set_chnmode(ViPipe, ch, N5_MODE_4M_25P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x80:
						n5_set_chnmode(ViPipe, ch, N5_MODE_8M_15P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0x82:
						n5_set_chnmode(ViPipe, ch, N5_MODE_8M_12_5P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0xA0:
						n5_set_chnmode(ViPipe, ch, N5_MODE_960P_30P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					case 0xA1:
						n5_set_chnmode(ViPipe, ch, N5_MODE_960P_25P);
						n5_set_portmode(ViPipe, 0, N5_OUTMODE_1MUX_FHD, 0);
						break;
					default:
						syslog(LOG_DEBUG, "vfc[0x%2x] not supported\n", val_5xf0);
						break;
					}
					pre_vfc[ch] = val_5xf0;
					ch_prevloss[ch] = ch_vloss[ch];
				}
			} else if (ch_vloss[ch] == 1 && ch_prevloss[ch] == 0) {
				if (chn_mode[ch] != NC_VIVO_CH_FORMATDEF_UNKNOWN) {
					n5_set_chnmode(ViPipe, ch, NC_VIVO_CH_FORMATDEF_UNKNOWN);
					n5_write_register(ViPipe, 0xff, 0x05+ch);
					n5_write_register(ViPipe, 0xB8, 0xB8);
					n5_write_register(ViPipe, 0xff, 0x13);
					val_13x70 = n5_read_register(ViPipe,  0x70);
					val_13x70 &= (~(0x01<<ch));
					n5_write_register(ViPipe, 0x70, val_13x70);
					pre_vfc[ch] = val_5xf0;
				}
				ch_prevloss[ch] = ch_vloss[ch];
			}
		}

		sleep(1);
	}
}
#endif

void n5_init(VI_PIPE ViPipe)
{
	n5_i2c_init(ViPipe);

	syslog(LOG_DEBUG, "Loading Nextchip N5 sensor\n");

	// check sensor chip id
	n5_write_register(ViPipe, 0xFF, 0x0);
	if (n5_read_register(ViPipe, 0xf4) != 0xe0) {
		syslog(LOG_DEBUG, "read N5 chip id fail\n");
		return;
	}

	n5_common_setting(ViPipe);

	// set device config
	n5_set_chnmode(ViPipe, 0, N5_MODE_720P_25P);
	n5_set_chnmode(ViPipe, 1, N5_MODE_720P_25P);
	n5_set_portmode(ViPipe, 0, N5_OUTMODE_2MUX_HD, 0/*is_bt601*/);

	// wait for the sensor signal to stabilize
	delay_ms(300);
#if N5_TEST_PATTERN
	// test_mode
	n5_write_register(ViPipe, 0xFF, 0x5);
	n5_write_register(ViPipe, 0x2c, 0x8);
	n5_write_register(ViPipe, 0xFF, 0x6);
	n5_write_register(ViPipe, 0x2c, 0x8);
	n5_write_register(ViPipe, 0xFF, 0x7);
	n5_write_register(ViPipe, 0x2c, 0x8);
	n5_write_register(ViPipe, 0xFF, 0x8);
	n5_write_register(ViPipe, 0x2c, 0x8);
	n5_write_register(ViPipe, 0xFF, 0x0);
	n5_write_register(ViPipe, 0x78, 0xaa);
	n5_write_register(ViPipe, 0x79, 0xaa);
#else
	{
		CVI_U32 ret;

		ret = pthread_create(&g_n5_thid, NULL, n5_device_auto_detect, ViPipe);
		if (ret == 0)
			syslog(LOG_DEBUG, "N5 auto detect function fail!\n");

	}
#endif
}

void n5_exit(VI_PIPE ViPipe)
{
	if (g_n5_thid)
		pthread_kill(g_n5_thid, SIGQUIT);

	n5_i2c_exit(ViPipe);
}
