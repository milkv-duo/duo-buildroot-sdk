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
#include "lt6911_cmos_ex.h"

#define LT6911_I2C_DEV 3
#define LT6911_I2C_BANK_ADDR 0xff

CVI_U8 lt6911_i2c_addr = 0x2b;
const CVI_U32 lt6911_addr_byte = 1;
const CVI_U32 lt6911_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int lt6911_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", LT6911_I2C_DEV);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", LT6911_I2C_DEV);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, lt6911_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int lt6911_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}


int lt6911_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	char buf[8];
	int idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (lt6911_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, lt6911_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, lt6911_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (lt6911_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "vipipe:%d i2c r 0x%x = 0x%x\n", ViPipe, addr, data);
	return data;
}

int lt6911_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (lt6911_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
	}
	buf[idx] = addr & 0xff;
	idx++;


	if (lt6911_data_byte == 2) {
		buf[idx] = (data >> 8) & 0xff;
		idx++;
	}
	buf[idx] = data & 0xff;
	idx++;

	ret = write(g_fd[ViPipe], buf, lt6911_addr_byte + lt6911_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	// ret = read(g_fd[ViPipe], buf, lt6911_addr_byte + lt6911_data_byte);
	syslog(LOG_DEBUG, "ViPipe:%d i2c w 0x%x 0x%x\n", ViPipe, addr, data);
	return CVI_SUCCESS;
}

static int lt6911_i2c_read(VI_PIPE ViPipe, int RegAddr)
{
	uint8_t bank = RegAddr >> 8;
	uint8_t addr = RegAddr & 0xff;

	lt6911_write_register(ViPipe, LT6911_I2C_BANK_ADDR, bank);
	return lt6911_read_register(ViPipe, addr);
}

static int lt6911_i2c_write(VI_PIPE ViPipe, int RegAddr, int data)
{
	uint8_t bank = RegAddr >> 8;
	uint8_t addr = RegAddr & 0xff;

	lt6911_write_register(ViPipe, LT6911_I2C_BANK_ADDR, bank);
	return lt6911_write_register(ViPipe, addr, data);
}

int lt6911_read(VI_PIPE ViPipe, int addr)
{
	int data = 0;

	lt6911_i2c_write(ViPipe, 0x80ee, 0x01);
	data = lt6911_i2c_read(ViPipe, addr);
	lt6911_i2c_write(ViPipe, 0x80ee, 0x00);
	return data;
}

int lt6911_write(VI_PIPE ViPipe, int addr, int data)
{
	lt6911_i2c_write(ViPipe, 0x80ee, 0x01);
	lt6911_i2c_write(ViPipe, addr, data);
	lt6911_i2c_write(ViPipe, 0x80ee, 0x00);
	return CVI_SUCCESS;
}

#define LT6911_CHIP_ID_ADDR_H	0xa000
#define LT6911_CHIP_ID_ADDR_L	0xa001
#define LT6911_CHIP_ID          0x1605

int  lt6911_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	usleep(50);
	if (lt6911_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = lt6911_read(ViPipe, LT6911_CHIP_ID_ADDR_H);
	nVal2 = lt6911_read(ViPipe, LT6911_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	printf("data:%02x %02x\n", nVal, nVal2);
	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != LT6911_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void lt6911_init(VI_PIPE ViPipe)
{
	lt6911_i2c_init(ViPipe);
}

void lt6911_exit(VI_PIPE ViPipe)
{
	lt6911_i2c_exit(ViPipe);
}