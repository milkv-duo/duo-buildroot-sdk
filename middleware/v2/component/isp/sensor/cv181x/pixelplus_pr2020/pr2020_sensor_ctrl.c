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
#include <pthread.h>
#include <signal.h>

#ifdef ARCH_CV182X
#include <linux/cvi_vip_snsr.h>
#include "cvi_comm_video.h"
#else
#include <linux/vi_snsr.h>
#include <linux/cvi_comm_video.h>
#endif
#include "cvi_sns_ctrl.h"
#include "cvi_vi.h"
#include "pr2020_cmos_ex.h"

const CVI_U8 pr2020_i2c_addr = 0x5C;        /* I2C slave address of PR2020*/
const CVI_U32 pr2020_addr_byte = 1;
const CVI_U32 pr2020_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};
static pthread_t g_pr2020_thid;
static PR2020_MODE_E signal_type = PR2020_MODE_NONE;

#define PR2020_AUTO_DETECT 1

/*gpio*/
enum CVI_GPIO_NUM_E {
CVI_GPIOD_00 = 404,
CVI_GPIOD_01,   CVI_GPIOD_02,   CVI_GPIOD_03,   CVI_GPIOD_04,   CVI_GPIOD_05,
CVI_GPIOD_06,   CVI_GPIOD_07,   CVI_GPIOD_08,   CVI_GPIOD_09,   CVI_GPIOD_10,
CVI_GPIOD_11,
CVI_GPIOC_00 = 416,
CVI_GPIOC_01,   CVI_GPIOC_02,   CVI_GPIOC_03,   CVI_GPIOC_04,   CVI_GPIOC_05,
CVI_GPIOC_06,   CVI_GPIOC_07,   CVI_GPIOC_08,   CVI_GPIOC_09,   CVI_GPIOC_10,
CVI_GPIOC_11,   CVI_GPIOC_12,   CVI_GPIOC_13,   CVI_GPIOC_14,   CVI_GPIOC_15,
CVI_GPIOC_16,   CVI_GPIOC_17,   CVI_GPIOC_18,   CVI_GPIOC_19,   CVI_GPIOC_20,
CVI_GPIOC_21,   CVI_GPIOC_22,   CVI_GPIOC_23,   CVI_GPIOC_24,   CVI_GPIOC_25,
CVI_GPIOC_26,   CVI_GPIOC_27,   CVI_GPIOC_28,   CVI_GPIOC_29,   CVI_GPIOC_30,
CVI_GPIOC_31,
CVI_GPIOB_00 = 448,
CVI_GPIOB_01,   CVI_GPIOB_02,   CVI_GPIOB_03,   CVI_GPIOB_04,   CVI_GPIOB_05,
CVI_GPIOB_06,   CVI_GPIOB_07,   CVI_GPIOB_08,   CVI_GPIOB_09,   CVI_GPIOB_10,
CVI_GPIOB_11,   CVI_GPIOB_12,   CVI_GPIOB_13,   CVI_GPIOB_14,   CVI_GPIOB_15,
CVI_GPIOB_16,   CVI_GPIOB_17,   CVI_GPIOB_18,   CVI_GPIOB_19,   CVI_GPIOB_20,
CVI_GPIOB_21,   CVI_GPIOB_22,   CVI_GPIOB_23,   CVI_GPIOB_24,   CVI_GPIOB_25,
CVI_GPIOB_26,   CVI_GPIOB_27,   CVI_GPIOB_28,   CVI_GPIOB_29,   CVI_GPIOB_30,
CVI_GPIOB_31,
CVI_GPIOA_00 = 480,
CVI_GPIOA_01,   CVI_GPIOA_02,   CVI_GPIOA_03,   CVI_GPIOA_04,   CVI_GPIOA_05,
CVI_GPIOA_06,   CVI_GPIOA_07,   CVI_GPIOA_08,   CVI_GPIOA_09,   CVI_GPIOA_10,
CVI_GPIOA_11,   CVI_GPIOA_12,   CVI_GPIOA_13,   CVI_GPIOA_14,   CVI_GPIOA_15,
CVI_GPIOA_16,   CVI_GPIOA_17,   CVI_GPIOA_18,   CVI_GPIOA_19,   CVI_GPIOA_20,
CVI_GPIOA_21,   CVI_GPIOA_22,   CVI_GPIOA_23,   CVI_GPIOA_24,   CVI_GPIOA_25,
CVI_GPIOA_26,   CVI_GPIOA_27,   CVI_GPIOA_28,   CVI_GPIOA_29,   CVI_GPIOA_30,
CVI_GPIOA_31,
};

#define CVI_GPIO_MIN CVI_GPIOD_00
#define CVI_GPIO_MAX CVI_GPIOA_31

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

static int PR2020_GPIO_Export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR"/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

static int PR2020_GPIO_SetDirection(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/direction", gpio);
	if (access(buf, 0) == -1)
		PR2020_GPIO_Export(gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}
	//printf("mark %d , %s\n",out_flag, buf);
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);

	close(fd);
	return 0;
}

static int PR2020_GPIO_SetValue(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);
	if (access(buf, 0) == -1)
		PR2020_GPIO_Export(gpio);

	PR2020_GPIO_SetDirection(gpio, 1); //output

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}

	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);

	close(fd);
	return 0;
}

int pr2020_sys_init(VI_PIPE ViPipe)
{
	(void) ViPipe;

#if 0
	//AHD_PWR_EN
	if (PR2020_GPIO_SetValue(CVI_GPIOA_00, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set power down gpio error!\n");
		return CVI_FAILURE;
	}
#endif
	//PR2K_RST
	if (PR2020_GPIO_SetValue(CVI_GPIOB_12, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set reset gpio error!\n");
		return CVI_FAILURE;
	}
#if 0
	//BACK_DET
	if (PR2020_GPIO_SetValue(CVI_GPIOD_01, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set back detect gpio error!\n");
		return CVI_FAILURE;
	}
#endif
	return CVI_SUCCESS;
}

int pr2020_i2c_init(VI_PIPE ViPipe)
{
	int ret;
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;

	u8DevNum = g_aunPr2020_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
	CVI_TRACE_SNS(CVI_DBG_INFO, "open %s\n", acDevFile);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, pr2020_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int pr2020_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int pr2020_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return 0;

	if (pr2020_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, pr2020_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, pr2020_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (pr2020_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	CVI_TRACE_SNS(CVI_DBG_INFO, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int pr2020_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (pr2020_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (pr2020_data_byte == 2)
		buf[idx++] = (data >> 8) & 0xff;

	// add data byte 0
	buf[idx++] = data & 0xff;

	ret = write(g_fd[ViPipe], buf, pr2020_addr_byte + pr2020_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	CVI_TRACE_SNS(CVI_DBG_INFO, "i2c w 0x%x 0x%x\n", addr, data);

#if 0 // read back checing
	ret = pr2020_read_register(ViPipe, addr);
	if (ret != data)
		CVI_TRACE_SNS(CVI_DBG_INFO, "i2c readback-check fail, 0x%x != 0x%x\n", ret, data);
#endif
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void pr2020_fw_init(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	pr2020_write_register(ViPipe, 0xFF, 0x00);
	pr2020_write_register(ViPipe, 0xD0, 0x30);
	pr2020_write_register(ViPipe, 0xD1, 0x08);
	pr2020_write_register(ViPipe, 0xD2, 0x21);
	pr2020_write_register(ViPipe, 0xD3, 0x00);
	pr2020_write_register(ViPipe, 0xD8, 0x37);
	pr2020_write_register(ViPipe, 0xD9, 0x08);

	pr2020_write_register(ViPipe, 0xFF, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xE4);//no-video data, 0xe4: black, 0xe5: blue
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0C);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xB2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0A, 0x02);
	pr2020_write_register(ViPipe, 0x0B, 0x14);
	pr2020_write_register(ViPipe, 0x0C, 0x04);
	pr2020_write_register(ViPipe, 0x0D, 0x08);
	pr2020_write_register(ViPipe, 0x0E, 0x5E);
	pr2020_write_register(ViPipe, 0x0F, 0x5E);
	pr2020_write_register(ViPipe, 0x10, 0x26);
}

void pr2020_set_cvbs_ntsc_60(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	//Page0 sys
	pr2020_write_register(ViPipe, 0xff, 0x00);
	pr2020_write_register(ViPipe, 0x10, 0x30);
	pr2020_write_register(ViPipe, 0x11, 0x07);
	pr2020_write_register(ViPipe, 0x12, 0x00);
	pr2020_write_register(ViPipe, 0x13, 0x00);
	pr2020_write_register(ViPipe, 0x14, 0x21);//b[1:0); => Select Camera Input. VinP(1), VinN(3), Differ(0).
	pr2020_write_register(ViPipe, 0x15, 0x44);
	pr2020_write_register(ViPipe, 0x16, 0x0d);
	pr2020_write_register(ViPipe, 0x40, 0x00);
	pr2020_write_register(ViPipe, 0x47, 0x36);
	pr2020_write_register(ViPipe, 0x4e, 0x3f);
	pr2020_write_register(ViPipe, 0x80, 0x56);
	pr2020_write_register(ViPipe, 0x81, 0x0e);
	pr2020_write_register(ViPipe, 0x82, 0x0d);
	pr2020_write_register(ViPipe, 0x84, 0x30);
	pr2020_write_register(ViPipe, 0x86, 0x20);
	pr2020_write_register(ViPipe, 0x87, 0x00);
	pr2020_write_register(ViPipe, 0x8a, 0x00);
	pr2020_write_register(ViPipe, 0x90, 0x00);
	pr2020_write_register(ViPipe, 0x91, 0x00);
	pr2020_write_register(ViPipe, 0x92, 0x00);
	pr2020_write_register(ViPipe, 0x94, 0xff);
	pr2020_write_register(ViPipe, 0x95, 0xff);
	pr2020_write_register(ViPipe, 0x96, 0xff);
	pr2020_write_register(ViPipe, 0xa0, 0x00);
	pr2020_write_register(ViPipe, 0xa1, 0x20);
	pr2020_write_register(ViPipe, 0xa4, 0x01);
	pr2020_write_register(ViPipe, 0xa5, 0xe3);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0x12);
	pr2020_write_register(ViPipe, 0xa8, 0x00);
	pr2020_write_register(ViPipe, 0xd0, 0x30);
	pr2020_write_register(ViPipe, 0xd1, 0x08);
	pr2020_write_register(ViPipe, 0xd2, 0x21);
	pr2020_write_register(ViPipe, 0xd3, 0x00);
	pr2020_write_register(ViPipe, 0xd8, 0x31);
	pr2020_write_register(ViPipe, 0xd9, 0x08);
	pr2020_write_register(ViPipe, 0xda, 0x21);
	pr2020_write_register(ViPipe, 0xe0, 0x3e);
	pr2020_write_register(ViPipe, 0xe1, 0x80);
	pr2020_write_register(ViPipe, 0xe2, 0x38);
	pr2020_write_register(ViPipe, 0xe3, 0x19);
	pr2020_write_register(ViPipe, 0xe4, 0x19);
	pr2020_write_register(ViPipe, 0xea, 0x01);
	pr2020_write_register(ViPipe, 0xeb, 0xff);
	pr2020_write_register(ViPipe, 0xf1, 0x44);
	pr2020_write_register(ViPipe, 0xf2, 0x01);

	//Page1 vdec
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xe4);
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0c);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xb2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0a, 0x02);
	pr2020_write_register(ViPipe, 0x0b, 0x14);
	pr2020_write_register(ViPipe, 0x0c, 0x04);
	pr2020_write_register(ViPipe, 0x0d, 0x08);
	pr2020_write_register(ViPipe, 0x0e, 0x5e);
	pr2020_write_register(ViPipe, 0x0f, 0x5e);
	pr2020_write_register(ViPipe, 0x10, 0x26);
	pr2020_write_register(ViPipe, 0x11, 0x00);
	pr2020_write_register(ViPipe, 0x12, 0x02);
	pr2020_write_register(ViPipe, 0x13, 0x90);
	pr2020_write_register(ViPipe, 0x14, 0xd0);
	pr2020_write_register(ViPipe, 0x15, 0x10);
	pr2020_write_register(ViPipe, 0x16, 0xf0);
	pr2020_write_register(ViPipe, 0x17, 0x00);
	pr2020_write_register(ViPipe, 0x18, 0x21);
	pr2020_write_register(ViPipe, 0x19, 0x4a);
	pr2020_write_register(ViPipe, 0x1a, 0x20);
	pr2020_write_register(ViPipe, 0x1b, 0x07);
	pr2020_write_register(ViPipe, 0x1c, 0x00);
	pr2020_write_register(ViPipe, 0x1d, 0x42);
	pr2020_write_register(ViPipe, 0x1e, 0x40);
	pr2020_write_register(ViPipe, 0x1f, 0xd0);
	pr2020_write_register(ViPipe, 0x20, 0x80);
	pr2020_write_register(ViPipe, 0x21, 0x70);
	pr2020_write_register(ViPipe, 0x22, 0x65);
	pr2020_write_register(ViPipe, 0x23, 0x83);
	pr2020_write_register(ViPipe, 0x24, 0x80);
	pr2020_write_register(ViPipe, 0x25, 0x80);
	pr2020_write_register(ViPipe, 0x26, 0x84);
	pr2020_write_register(ViPipe, 0x27, 0x80);
	pr2020_write_register(ViPipe, 0x28, 0x00);
	pr2020_write_register(ViPipe, 0x29, 0x5f);
	pr2020_write_register(ViPipe, 0x2a, 0x20);
	pr2020_write_register(ViPipe, 0x2b, 0x00);
	pr2020_write_register(ViPipe, 0x2c, 0x00);
	pr2020_write_register(ViPipe, 0x2d, 0x00);
	pr2020_write_register(ViPipe, 0x2e, 0x00);
	pr2020_write_register(ViPipe, 0x2f, 0x00);
	pr2020_write_register(ViPipe, 0x30, 0x00);
	pr2020_write_register(ViPipe, 0x31, 0x00);
	pr2020_write_register(ViPipe, 0x32, 0xc0);
	pr2020_write_register(ViPipe, 0x33, 0x14);
	pr2020_write_register(ViPipe, 0x34, 0x14);
	pr2020_write_register(ViPipe, 0x35, 0x80);
	pr2020_write_register(ViPipe, 0x36, 0x80);
	pr2020_write_register(ViPipe, 0x37, 0xe2);
	pr2020_write_register(ViPipe, 0x38, 0x41);
	pr2020_write_register(ViPipe, 0x39, 0x00);
	pr2020_write_register(ViPipe, 0x3a, 0xac);
	pr2020_write_register(ViPipe, 0x3b, 0x04);
	pr2020_write_register(ViPipe, 0x3c, 0x01);
	pr2020_write_register(ViPipe, 0x3d, 0x21);
	pr2020_write_register(ViPipe, 0x3e, 0x06);
	pr2020_write_register(ViPipe, 0x3f, 0xd5);
	pr2020_write_register(ViPipe, 0x40, 0x05);
	pr2020_write_register(ViPipe, 0x41, 0x55);
	pr2020_write_register(ViPipe, 0x42, 0x01);
	pr2020_write_register(ViPipe, 0x43, 0x30);
	pr2020_write_register(ViPipe, 0x44, 0x6a);
	pr2020_write_register(ViPipe, 0x45, 0x00);
	pr2020_write_register(ViPipe, 0x46, 0x06);
	pr2020_write_register(ViPipe, 0x47, 0x2b);
	pr2020_write_register(ViPipe, 0x48, 0xb9);
	pr2020_write_register(ViPipe, 0x49, 0x00);
	pr2020_write_register(ViPipe, 0x4a, 0x7b);
	pr2020_write_register(ViPipe, 0x4b, 0x60);
	pr2020_write_register(ViPipe, 0x4c, 0x00);
	pr2020_write_register(ViPipe, 0x4d, 0x47);
	pr2020_write_register(ViPipe, 0x4e, 0x02);
	pr2020_write_register(ViPipe, 0x4f, 0x20);
	pr2020_write_register(ViPipe, 0x50, 0x01);
	pr2020_write_register(ViPipe, 0x51, 0x28);
	pr2020_write_register(ViPipe, 0x52, 0x40);
	pr2020_write_register(ViPipe, 0x53, 0x0c);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
	pr2020_write_register(ViPipe, 0x55, 0x8d);
	pr2020_write_register(ViPipe, 0x70, 0x06);
	pr2020_write_register(ViPipe, 0x71, 0x08);
	pr2020_write_register(ViPipe, 0x72, 0x0a);
	pr2020_write_register(ViPipe, 0x73, 0x0c);
	pr2020_write_register(ViPipe, 0x74, 0x0e);
	pr2020_write_register(ViPipe, 0x75, 0x10);
	pr2020_write_register(ViPipe, 0x76, 0x12);
	pr2020_write_register(ViPipe, 0x77, 0x14);
	pr2020_write_register(ViPipe, 0x78, 0x06);
	pr2020_write_register(ViPipe, 0x79, 0x08);
	pr2020_write_register(ViPipe, 0x7a, 0x0a);
	pr2020_write_register(ViPipe, 0x7b, 0x0c);
	pr2020_write_register(ViPipe, 0x7c, 0x0e);
	pr2020_write_register(ViPipe, 0x7d, 0x10);
	pr2020_write_register(ViPipe, 0x7e, 0x12);
	pr2020_write_register(ViPipe, 0x7f, 0x14);
	pr2020_write_register(ViPipe, 0x80, 0x00);
	pr2020_write_register(ViPipe, 0x81, 0x09);
	pr2020_write_register(ViPipe, 0x82, 0x00);
	pr2020_write_register(ViPipe, 0x83, 0x07);
	pr2020_write_register(ViPipe, 0x84, 0x00);
	pr2020_write_register(ViPipe, 0x85, 0x17);
	pr2020_write_register(ViPipe, 0x86, 0x03);
	pr2020_write_register(ViPipe, 0x87, 0xe5);
	pr2020_write_register(ViPipe, 0x88, 0x04);
	pr2020_write_register(ViPipe, 0x89, 0x48);
	pr2020_write_register(ViPipe, 0x8a, 0x04);
	pr2020_write_register(ViPipe, 0x8b, 0x48);
	pr2020_write_register(ViPipe, 0x8c, 0x05);
	pr2020_write_register(ViPipe, 0x8d, 0xf0);
	pr2020_write_register(ViPipe, 0x8e, 0x05);
	pr2020_write_register(ViPipe, 0x8f, 0x47);
	pr2020_write_register(ViPipe, 0x90, 0x03);
	pr2020_write_register(ViPipe, 0x91, 0x13);
	pr2020_write_register(ViPipe, 0x92, 0x73);
	pr2020_write_register(ViPipe, 0x93, 0xe8);
	pr2020_write_register(ViPipe, 0x94, 0x0f);
	pr2020_write_register(ViPipe, 0x95, 0x5e);
	pr2020_write_register(ViPipe, 0x96, 0x03);
	pr2020_write_register(ViPipe, 0x97, 0xd0);
	pr2020_write_register(ViPipe, 0x98, 0x17);
	pr2020_write_register(ViPipe, 0x99, 0x34);
	pr2020_write_register(ViPipe, 0x9a, 0x13);
	pr2020_write_register(ViPipe, 0x9b, 0x56);
	pr2020_write_register(ViPipe, 0x9c, 0x0b);
	pr2020_write_register(ViPipe, 0x9d, 0x9a);
	pr2020_write_register(ViPipe, 0x9e, 0x09);
	pr2020_write_register(ViPipe, 0x9f, 0xab);
	pr2020_write_register(ViPipe, 0xa0, 0x01);
	pr2020_write_register(ViPipe, 0xa1, 0x74);
	pr2020_write_register(ViPipe, 0xa2, 0x01);
	pr2020_write_register(ViPipe, 0xa3, 0x6b);
	pr2020_write_register(ViPipe, 0xa4, 0x00);
	pr2020_write_register(ViPipe, 0xa5, 0xba);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0xa3);
	pr2020_write_register(ViPipe, 0xa8, 0x01);
	pr2020_write_register(ViPipe, 0xa9, 0x39);
	pr2020_write_register(ViPipe, 0xaa, 0x01);
	pr2020_write_register(ViPipe, 0xab, 0x39);
	pr2020_write_register(ViPipe, 0xac, 0x00);
	pr2020_write_register(ViPipe, 0xad, 0xc1);
	pr2020_write_register(ViPipe, 0xae, 0x00);
	pr2020_write_register(ViPipe, 0xaf, 0xc1);
	pr2020_write_register(ViPipe, 0xb0, 0x04);
	pr2020_write_register(ViPipe, 0xb1, 0xd4);
	pr2020_write_register(ViPipe, 0xb2, 0x07);
	pr2020_write_register(ViPipe, 0xb3, 0xda);
	pr2020_write_register(ViPipe, 0xb4, 0x00);
	pr2020_write_register(ViPipe, 0xb5, 0x17);
	pr2020_write_register(ViPipe, 0xb6, 0x08);
	pr2020_write_register(ViPipe, 0xb7, 0xe8);
	pr2020_write_register(ViPipe, 0xb8, 0xb0);
	pr2020_write_register(ViPipe, 0xb9, 0xce);
	pr2020_write_register(ViPipe, 0xba, 0x90);
	pr2020_write_register(ViPipe, 0xbb, 0x10);
	pr2020_write_register(ViPipe, 0xbc, 0x00);
	pr2020_write_register(ViPipe, 0xbd, 0x04);
	pr2020_write_register(ViPipe, 0xbe, 0x02);
	pr2020_write_register(ViPipe, 0xbf, 0xd0);
	pr2020_write_register(ViPipe, 0xc0, 0x00);
	pr2020_write_register(ViPipe, 0xc1, 0x10);
	pr2020_write_register(ViPipe, 0xc2, 0x00);
	pr2020_write_register(ViPipe, 0xc3, 0xf0);

	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0e);
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
}

void pr2020_set_cvbs_pal_50(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	//Page0 sys
	pr2020_write_register(ViPipe, 0xff, 0x00);
	pr2020_write_register(ViPipe, 0x10, 0x21);
	pr2020_write_register(ViPipe, 0x11, 0x07);
	pr2020_write_register(ViPipe, 0x12, 0x00);
	pr2020_write_register(ViPipe, 0x13, 0x00);
	pr2020_write_register(ViPipe, 0x14, 0x21);//b[1:0); => Select Camera Input. VinP(1), VinN(3), Differ(0).
	pr2020_write_register(ViPipe, 0x15, 0x44);
	pr2020_write_register(ViPipe, 0x16, 0x0d);
	pr2020_write_register(ViPipe, 0x40, 0x00);
	pr2020_write_register(ViPipe, 0x47, 0x36);
	pr2020_write_register(ViPipe, 0x4e, 0x3f);
	pr2020_write_register(ViPipe, 0x80, 0x56);
	pr2020_write_register(ViPipe, 0x81, 0x0e);
	pr2020_write_register(ViPipe, 0x82, 0x0d);
	pr2020_write_register(ViPipe, 0x84, 0x30);
	pr2020_write_register(ViPipe, 0x86, 0x20);
	pr2020_write_register(ViPipe, 0x87, 0x00);
	pr2020_write_register(ViPipe, 0x8a, 0x00);
	pr2020_write_register(ViPipe, 0x90, 0x00);
	pr2020_write_register(ViPipe, 0x91, 0x00);
	pr2020_write_register(ViPipe, 0x92, 0x00);
	pr2020_write_register(ViPipe, 0x94, 0xff);
	pr2020_write_register(ViPipe, 0x95, 0xff);
	pr2020_write_register(ViPipe, 0x96, 0xff);
	pr2020_write_register(ViPipe, 0xa0, 0x00);
	pr2020_write_register(ViPipe, 0xa1, 0x20);
	pr2020_write_register(ViPipe, 0xa4, 0x01);
	pr2020_write_register(ViPipe, 0xa5, 0xe3);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0x12);
	pr2020_write_register(ViPipe, 0xa8, 0x00);
	pr2020_write_register(ViPipe, 0xd0, 0x30);
	pr2020_write_register(ViPipe, 0xd1, 0x08);
	pr2020_write_register(ViPipe, 0xd2, 0x21);
	pr2020_write_register(ViPipe, 0xd3, 0x00);
	pr2020_write_register(ViPipe, 0xd8, 0x31);
	pr2020_write_register(ViPipe, 0xd9, 0x08);
	pr2020_write_register(ViPipe, 0xda, 0x21);
	pr2020_write_register(ViPipe, 0xe0, 0x3e);
	pr2020_write_register(ViPipe, 0xe1, 0x80);
	pr2020_write_register(ViPipe, 0xe2, 0x38);
	pr2020_write_register(ViPipe, 0xe3, 0x19);
	pr2020_write_register(ViPipe, 0xe4, 0x19);
	pr2020_write_register(ViPipe, 0xea, 0x01);
	pr2020_write_register(ViPipe, 0xeb, 0xff);
	pr2020_write_register(ViPipe, 0xf1, 0x44);
	pr2020_write_register(ViPipe, 0xf2, 0x01);

	//Page1 vdec
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xe4);
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0c);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xb2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0a, 0x02);
	pr2020_write_register(ViPipe, 0x0b, 0x14);
	pr2020_write_register(ViPipe, 0x0c, 0x04);
	pr2020_write_register(ViPipe, 0x0d, 0x08);
	pr2020_write_register(ViPipe, 0x0e, 0x5e);
	pr2020_write_register(ViPipe, 0x0f, 0x5e);
	pr2020_write_register(ViPipe, 0x10, 0x26);
	pr2020_write_register(ViPipe, 0x11, 0x00);
	pr2020_write_register(ViPipe, 0x12, 0x22);
	pr2020_write_register(ViPipe, 0x13, 0xe0);
	pr2020_write_register(ViPipe, 0x14, 0xd0);
	pr2020_write_register(ViPipe, 0x15, 0x16);
	pr2020_write_register(ViPipe, 0x16, 0x20);
	pr2020_write_register(ViPipe, 0x17, 0x00);
	pr2020_write_register(ViPipe, 0x18, 0x21);
	pr2020_write_register(ViPipe, 0x19, 0x4a);
	pr2020_write_register(ViPipe, 0x1a, 0x20);
	pr2020_write_register(ViPipe, 0x1b, 0x06);
	pr2020_write_register(ViPipe, 0x1c, 0x31);
	pr2020_write_register(ViPipe, 0x1d, 0x42);
	pr2020_write_register(ViPipe, 0x1e, 0x50);
	pr2020_write_register(ViPipe, 0x1f, 0xd0);
	pr2020_write_register(ViPipe, 0x20, 0x80);
	pr2020_write_register(ViPipe, 0x21, 0x75);
	pr2020_write_register(ViPipe, 0x22, 0x65);
	pr2020_write_register(ViPipe, 0x23, 0x83);
	pr2020_write_register(ViPipe, 0x24, 0x80);
	pr2020_write_register(ViPipe, 0x25, 0x80);
	pr2020_write_register(ViPipe, 0x26, 0x84);
	pr2020_write_register(ViPipe, 0x27, 0x80);
	pr2020_write_register(ViPipe, 0x28, 0x00);
	pr2020_write_register(ViPipe, 0x29, 0x5f);
	pr2020_write_register(ViPipe, 0x2a, 0x20);
	pr2020_write_register(ViPipe, 0x2b, 0x00);
	pr2020_write_register(ViPipe, 0x2c, 0x00);
	pr2020_write_register(ViPipe, 0x2d, 0x00);
	pr2020_write_register(ViPipe, 0x2e, 0x00);
	pr2020_write_register(ViPipe, 0x2f, 0x00);
	pr2020_write_register(ViPipe, 0x30, 0x00);
	pr2020_write_register(ViPipe, 0x31, 0x00);
	pr2020_write_register(ViPipe, 0x32, 0xc0);
	pr2020_write_register(ViPipe, 0x33, 0x20);
	pr2020_write_register(ViPipe, 0x34, 0x20);
	pr2020_write_register(ViPipe, 0x35, 0x10);
	pr2020_write_register(ViPipe, 0x36, 0x10);
	pr2020_write_register(ViPipe, 0x37, 0xc4);
	pr2020_write_register(ViPipe, 0x38, 0x42);
	pr2020_write_register(ViPipe, 0x39, 0x00);
	pr2020_write_register(ViPipe, 0x3a, 0xac);
	pr2020_write_register(ViPipe, 0x3b, 0x04);
	pr2020_write_register(ViPipe, 0x3c, 0x01);
	pr2020_write_register(ViPipe, 0x3d, 0x21);
	pr2020_write_register(ViPipe, 0x3e, 0x06);
	pr2020_write_register(ViPipe, 0x3f, 0xd5);
	pr2020_write_register(ViPipe, 0x40, 0x85);
	pr2020_write_register(ViPipe, 0x41, 0x55);
	pr2020_write_register(ViPipe, 0x42, 0x01);
	pr2020_write_register(ViPipe, 0x43, 0x31);
	pr2020_write_register(ViPipe, 0x44, 0x6a);
	pr2020_write_register(ViPipe, 0x45, 0x00);
	pr2020_write_register(ViPipe, 0x46, 0x07);
	pr2020_write_register(ViPipe, 0x47, 0xa4);
	pr2020_write_register(ViPipe, 0x48, 0xa5);
	pr2020_write_register(ViPipe, 0x49, 0x00);
	pr2020_write_register(ViPipe, 0x4a, 0x7b);
	pr2020_write_register(ViPipe, 0x4b, 0x60);
	pr2020_write_register(ViPipe, 0x4c, 0x00);
	pr2020_write_register(ViPipe, 0x4d, 0x47);
	pr2020_write_register(ViPipe, 0x4e, 0x02);
	pr2020_write_register(ViPipe, 0x4f, 0x20);
	pr2020_write_register(ViPipe, 0x50, 0x01);
	pr2020_write_register(ViPipe, 0x51, 0x28);
	pr2020_write_register(ViPipe, 0x52, 0x40);
	pr2020_write_register(ViPipe, 0x53, 0x0c);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
	pr2020_write_register(ViPipe, 0x55, 0x8d);
	pr2020_write_register(ViPipe, 0x70, 0x06);
	pr2020_write_register(ViPipe, 0x71, 0x08);
	pr2020_write_register(ViPipe, 0x72, 0x0a);
	pr2020_write_register(ViPipe, 0x73, 0x0c);
	pr2020_write_register(ViPipe, 0x74, 0x0e);
	pr2020_write_register(ViPipe, 0x75, 0x10);
	pr2020_write_register(ViPipe, 0x76, 0x12);
	pr2020_write_register(ViPipe, 0x77, 0x14);
	pr2020_write_register(ViPipe, 0x78, 0x06);
	pr2020_write_register(ViPipe, 0x79, 0x08);
	pr2020_write_register(ViPipe, 0x7a, 0x0a);
	pr2020_write_register(ViPipe, 0x7b, 0x0c);
	pr2020_write_register(ViPipe, 0x7c, 0x0e);
	pr2020_write_register(ViPipe, 0x7d, 0x10);
	pr2020_write_register(ViPipe, 0x7e, 0x12);
	pr2020_write_register(ViPipe, 0x7f, 0x14);
	pr2020_write_register(ViPipe, 0x80, 0x00);
	pr2020_write_register(ViPipe, 0x81, 0x09);
	pr2020_write_register(ViPipe, 0x82, 0x00);
	pr2020_write_register(ViPipe, 0x83, 0x07);
	pr2020_write_register(ViPipe, 0x84, 0x00);
	pr2020_write_register(ViPipe, 0x85, 0x17);
	pr2020_write_register(ViPipe, 0x86, 0x03);
	pr2020_write_register(ViPipe, 0x87, 0xe5);
	pr2020_write_register(ViPipe, 0x88, 0x05);
	pr2020_write_register(ViPipe, 0x89, 0x24);
	pr2020_write_register(ViPipe, 0x8a, 0x05);
	pr2020_write_register(ViPipe, 0x8b, 0x24);
	pr2020_write_register(ViPipe, 0x8c, 0x05);
	pr2020_write_register(ViPipe, 0x8d, 0xf0);
	pr2020_write_register(ViPipe, 0x8e, 0x05);
	pr2020_write_register(ViPipe, 0x8f, 0x47);
	pr2020_write_register(ViPipe, 0x90, 0x02);
	pr2020_write_register(ViPipe, 0x91, 0xb4);
	pr2020_write_register(ViPipe, 0x92, 0x73);
	pr2020_write_register(ViPipe, 0x93, 0xe8);
	pr2020_write_register(ViPipe, 0x94, 0x0f);
	pr2020_write_register(ViPipe, 0x95, 0x5e);
	pr2020_write_register(ViPipe, 0x96, 0x03);
	pr2020_write_register(ViPipe, 0x97, 0xd0);
	pr2020_write_register(ViPipe, 0x98, 0x17);
	pr2020_write_register(ViPipe, 0x99, 0x34);
	pr2020_write_register(ViPipe, 0x9a, 0x13);
	pr2020_write_register(ViPipe, 0x9b, 0x56);
	pr2020_write_register(ViPipe, 0x9c, 0x0b);
	pr2020_write_register(ViPipe, 0x9d, 0x9a);
	pr2020_write_register(ViPipe, 0x9e, 0x09);
	pr2020_write_register(ViPipe, 0x9f, 0xab);
	pr2020_write_register(ViPipe, 0xa0, 0x01);
	pr2020_write_register(ViPipe, 0xa1, 0x74);
	pr2020_write_register(ViPipe, 0xa2, 0x01);
	pr2020_write_register(ViPipe, 0xa3, 0x6b);
	pr2020_write_register(ViPipe, 0xa4, 0x00);
	pr2020_write_register(ViPipe, 0xa5, 0xba);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0xa3);
	pr2020_write_register(ViPipe, 0xa8, 0x01);
	pr2020_write_register(ViPipe, 0xa9, 0x39);
	pr2020_write_register(ViPipe, 0xaa, 0x01);
	pr2020_write_register(ViPipe, 0xab, 0x39);
	pr2020_write_register(ViPipe, 0xac, 0x00);
	pr2020_write_register(ViPipe, 0xad, 0xc1);
	pr2020_write_register(ViPipe, 0xae, 0x00);
	pr2020_write_register(ViPipe, 0xaf, 0xc1);
	pr2020_write_register(ViPipe, 0xb0, 0x05);
	pr2020_write_register(ViPipe, 0xb1, 0xcc);
	pr2020_write_register(ViPipe, 0xb2, 0x09);
	pr2020_write_register(ViPipe, 0xb3, 0x6d);
	pr2020_write_register(ViPipe, 0xb4, 0x00);
	pr2020_write_register(ViPipe, 0xb5, 0x17);
	pr2020_write_register(ViPipe, 0xb6, 0x08);
	pr2020_write_register(ViPipe, 0xb7, 0xe8);
	pr2020_write_register(ViPipe, 0xb8, 0xb0);
	pr2020_write_register(ViPipe, 0xb9, 0xce);
	pr2020_write_register(ViPipe, 0xba, 0x90);
	pr2020_write_register(ViPipe, 0xbb, 0x10);
	pr2020_write_register(ViPipe, 0xbc, 0x00);
	pr2020_write_register(ViPipe, 0xbd, 0x04);
	pr2020_write_register(ViPipe, 0xbe, 0x02);
	pr2020_write_register(ViPipe, 0xbf, 0xd0);
	pr2020_write_register(ViPipe, 0xc0, 0x00);
	pr2020_write_register(ViPipe, 0xc1, 0x16);
	pr2020_write_register(ViPipe, 0xc2, 0x01);
	pr2020_write_register(ViPipe, 0xc3, 0x20);

	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0e);
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
}

void pr2020_set_720p_25(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	//Page0 sys
	pr2020_write_register(ViPipe, 0xff, 0x00);
	pr2020_write_register(ViPipe, 0x10, 0x82);
	pr2020_write_register(ViPipe, 0x11, 0x07);
	pr2020_write_register(ViPipe, 0x12, 0x00);
	pr2020_write_register(ViPipe, 0x13, 0x00);
	pr2020_write_register(ViPipe, 0x14, 0x21);//b[1:0); => Select Camera Input. VinP(1), VinN(3), Differ(0).
	pr2020_write_register(ViPipe, 0x15, 0x44);
	pr2020_write_register(ViPipe, 0x16, 0x0d);
	pr2020_write_register(ViPipe, 0x40, 0x00);
	pr2020_write_register(ViPipe, 0x47, 0x3a);
	pr2020_write_register(ViPipe, 0x4e, 0x3f);
	pr2020_write_register(ViPipe, 0x80, 0x56);
	pr2020_write_register(ViPipe, 0x81, 0x0e);
	pr2020_write_register(ViPipe, 0x82, 0x0d);
	pr2020_write_register(ViPipe, 0x84, 0x30);
	pr2020_write_register(ViPipe, 0x86, 0x20);
	pr2020_write_register(ViPipe, 0x87, 0x00);
	pr2020_write_register(ViPipe, 0x8a, 0x00);
	pr2020_write_register(ViPipe, 0x90, 0x00);
	pr2020_write_register(ViPipe, 0x91, 0x00);
	pr2020_write_register(ViPipe, 0x92, 0x00);
	pr2020_write_register(ViPipe, 0x94, 0xff);
	pr2020_write_register(ViPipe, 0x95, 0xff);
	pr2020_write_register(ViPipe, 0x96, 0xff);
	pr2020_write_register(ViPipe, 0xa0, 0x00);
	pr2020_write_register(ViPipe, 0xa1, 0x20);
	pr2020_write_register(ViPipe, 0xa4, 0x01);
	pr2020_write_register(ViPipe, 0xa5, 0xe3);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0x12);
	pr2020_write_register(ViPipe, 0xa8, 0x00);
	pr2020_write_register(ViPipe, 0xd0, 0x30);
	pr2020_write_register(ViPipe, 0xd1, 0x08);
	pr2020_write_register(ViPipe, 0xd2, 0x21);
	pr2020_write_register(ViPipe, 0xd3, 0x00);
	pr2020_write_register(ViPipe, 0xd8, 0x31);
	pr2020_write_register(ViPipe, 0xd9, 0x08);
	pr2020_write_register(ViPipe, 0xda, 0x21);
	pr2020_write_register(ViPipe, 0xe0, 0x39);
	pr2020_write_register(ViPipe, 0xe1, 0x90);
	pr2020_write_register(ViPipe, 0xe2, 0x38);
	pr2020_write_register(ViPipe, 0xe3, 0x19);
	pr2020_write_register(ViPipe, 0xe4, 0x19);
	pr2020_write_register(ViPipe, 0xea, 0x01);
	pr2020_write_register(ViPipe, 0xeb, 0xff);
	pr2020_write_register(ViPipe, 0xf1, 0x44);
	pr2020_write_register(ViPipe, 0xf2, 0x01);

	//Page1 vdec
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xe4);
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0c);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xb2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0a, 0x02);
	pr2020_write_register(ViPipe, 0x0b, 0x14);
	pr2020_write_register(ViPipe, 0x0c, 0x04);
	pr2020_write_register(ViPipe, 0x0d, 0x08);
	pr2020_write_register(ViPipe, 0x0e, 0x5e);
	pr2020_write_register(ViPipe, 0x0f, 0x5e);
	pr2020_write_register(ViPipe, 0x10, 0x26);
	pr2020_write_register(ViPipe, 0x11, 0x01);
	pr2020_write_register(ViPipe, 0x12, 0x45);
	pr2020_write_register(ViPipe, 0x13, 0x0c);
	pr2020_write_register(ViPipe, 0x14, 0x00);
	pr2020_write_register(ViPipe, 0x15, 0x1b);
	pr2020_write_register(ViPipe, 0x16, 0xd0);
	pr2020_write_register(ViPipe, 0x17, 0x00);
	pr2020_write_register(ViPipe, 0x18, 0x41);
	pr2020_write_register(ViPipe, 0x19, 0x46);
	pr2020_write_register(ViPipe, 0x1a, 0x22);
	pr2020_write_register(ViPipe, 0x1b, 0x05);
	pr2020_write_register(ViPipe, 0x1c, 0xea);
	pr2020_write_register(ViPipe, 0x1d, 0x45);
	pr2020_write_register(ViPipe, 0x1e, 0x4c);
	pr2020_write_register(ViPipe, 0x1f, 0x00);
	pr2020_write_register(ViPipe, 0x20, 0x80);
	pr2020_write_register(ViPipe, 0x21, 0x80);
	pr2020_write_register(ViPipe, 0x22, 0x90);
	pr2020_write_register(ViPipe, 0x23, 0x80);
	pr2020_write_register(ViPipe, 0x24, 0x80);
	pr2020_write_register(ViPipe, 0x25, 0x80);
	pr2020_write_register(ViPipe, 0x26, 0x84);
	pr2020_write_register(ViPipe, 0x27, 0x82);
	pr2020_write_register(ViPipe, 0x28, 0x00);
	pr2020_write_register(ViPipe, 0x29, 0x7d);
	pr2020_write_register(ViPipe, 0x2a, 0x00);
	pr2020_write_register(ViPipe, 0x2b, 0x00);
	pr2020_write_register(ViPipe, 0x2c, 0x00);
	pr2020_write_register(ViPipe, 0x2d, 0x00);
	pr2020_write_register(ViPipe, 0x2e, 0x00);
	pr2020_write_register(ViPipe, 0x2f, 0x00);
	pr2020_write_register(ViPipe, 0x30, 0x00);
	pr2020_write_register(ViPipe, 0x31, 0x00);
	pr2020_write_register(ViPipe, 0x32, 0xc0);
	pr2020_write_register(ViPipe, 0x33, 0x14);
	pr2020_write_register(ViPipe, 0x34, 0x14);
	pr2020_write_register(ViPipe, 0x35, 0x80);
	pr2020_write_register(ViPipe, 0x36, 0x80);
	pr2020_write_register(ViPipe, 0x37, 0xaa);
	pr2020_write_register(ViPipe, 0x38, 0x48);
	pr2020_write_register(ViPipe, 0x39, 0x08);
	pr2020_write_register(ViPipe, 0x3a, 0x27);
	pr2020_write_register(ViPipe, 0x3b, 0x02);
	pr2020_write_register(ViPipe, 0x3c, 0x01);
	pr2020_write_register(ViPipe, 0x3d, 0x23);
	pr2020_write_register(ViPipe, 0x3e, 0x02);
	pr2020_write_register(ViPipe, 0x3f, 0xc4);
	pr2020_write_register(ViPipe, 0x40, 0x05);
	pr2020_write_register(ViPipe, 0x41, 0x55);
	pr2020_write_register(ViPipe, 0x42, 0x01);
	pr2020_write_register(ViPipe, 0x43, 0x33);
	pr2020_write_register(ViPipe, 0x44, 0x6a);
	pr2020_write_register(ViPipe, 0x45, 0x00);
	pr2020_write_register(ViPipe, 0x46, 0x09);
	pr2020_write_register(ViPipe, 0x47, 0xe2);
	pr2020_write_register(ViPipe, 0x48, 0x01);
	pr2020_write_register(ViPipe, 0x49, 0x00);
	pr2020_write_register(ViPipe, 0x4a, 0x7b);
	pr2020_write_register(ViPipe, 0x4b, 0x60);
	pr2020_write_register(ViPipe, 0x4c, 0x00);
	pr2020_write_register(ViPipe, 0x4d, 0x4a);
	pr2020_write_register(ViPipe, 0x4e, 0x00);
	pr2020_write_register(ViPipe, 0x4f, 0x20);
	pr2020_write_register(ViPipe, 0x50, 0x01);
	pr2020_write_register(ViPipe, 0x51, 0x28);
	pr2020_write_register(ViPipe, 0x52, 0x40);
	pr2020_write_register(ViPipe, 0x53, 0x0c);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
	pr2020_write_register(ViPipe, 0x55, 0x8d);
	pr2020_write_register(ViPipe, 0x70, 0x06);
	pr2020_write_register(ViPipe, 0x71, 0x08);
	pr2020_write_register(ViPipe, 0x72, 0x0a);
	pr2020_write_register(ViPipe, 0x73, 0x0c);
	pr2020_write_register(ViPipe, 0x74, 0x0e);
	pr2020_write_register(ViPipe, 0x75, 0x10);
	pr2020_write_register(ViPipe, 0x76, 0x12);
	pr2020_write_register(ViPipe, 0x77, 0x14);
	pr2020_write_register(ViPipe, 0x78, 0x06);
	pr2020_write_register(ViPipe, 0x79, 0x08);
	pr2020_write_register(ViPipe, 0x7a, 0x0a);
	pr2020_write_register(ViPipe, 0x7b, 0x0c);
	pr2020_write_register(ViPipe, 0x7c, 0x0e);
	pr2020_write_register(ViPipe, 0x7d, 0x10);
	pr2020_write_register(ViPipe, 0x7e, 0x12);
	pr2020_write_register(ViPipe, 0x7f, 0x14);
	pr2020_write_register(ViPipe, 0x80, 0x00);
	pr2020_write_register(ViPipe, 0x81, 0x09);
	pr2020_write_register(ViPipe, 0x82, 0x00);
	pr2020_write_register(ViPipe, 0x83, 0x07);
	pr2020_write_register(ViPipe, 0x84, 0x00);
	pr2020_write_register(ViPipe, 0x85, 0x17);
	pr2020_write_register(ViPipe, 0x86, 0x03);
	pr2020_write_register(ViPipe, 0x87, 0xe5);
	pr2020_write_register(ViPipe, 0x88, 0x0a);
	pr2020_write_register(ViPipe, 0x89, 0x48);
	pr2020_write_register(ViPipe, 0x8a, 0x0a);
	pr2020_write_register(ViPipe, 0x8b, 0x48);
	pr2020_write_register(ViPipe, 0x8c, 0x0b);
	pr2020_write_register(ViPipe, 0x8d, 0xe0);
	pr2020_write_register(ViPipe, 0x8e, 0x05);
	pr2020_write_register(ViPipe, 0x8f, 0x47);
	pr2020_write_register(ViPipe, 0x90, 0x05);
	pr2020_write_register(ViPipe, 0x91, 0x69);
	pr2020_write_register(ViPipe, 0x92, 0x73);
	pr2020_write_register(ViPipe, 0x93, 0xe8);
	pr2020_write_register(ViPipe, 0x94, 0x0f);
	pr2020_write_register(ViPipe, 0x95, 0x5e);
	pr2020_write_register(ViPipe, 0x96, 0x07);
	pr2020_write_register(ViPipe, 0x97, 0x90);
	pr2020_write_register(ViPipe, 0x98, 0x17);
	pr2020_write_register(ViPipe, 0x99, 0x34);
	pr2020_write_register(ViPipe, 0x9a, 0x13);
	pr2020_write_register(ViPipe, 0x9b, 0x56);
	pr2020_write_register(ViPipe, 0x9c, 0x0b);
	pr2020_write_register(ViPipe, 0x9d, 0x9a);
	pr2020_write_register(ViPipe, 0x9e, 0x09);
	pr2020_write_register(ViPipe, 0x9f, 0xab);
	pr2020_write_register(ViPipe, 0xa0, 0x01);
	pr2020_write_register(ViPipe, 0xa1, 0x74);
	pr2020_write_register(ViPipe, 0xa2, 0x01);
	pr2020_write_register(ViPipe, 0xa3, 0x6b);
	pr2020_write_register(ViPipe, 0xa4, 0x00);
	pr2020_write_register(ViPipe, 0xa5, 0xba);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0xa3);
	pr2020_write_register(ViPipe, 0xa8, 0x01);
	pr2020_write_register(ViPipe, 0xa9, 0x39);
	pr2020_write_register(ViPipe, 0xaa, 0x01);
	pr2020_write_register(ViPipe, 0xab, 0x39);
	pr2020_write_register(ViPipe, 0xac, 0x00);
	pr2020_write_register(ViPipe, 0xad, 0xc1);
	pr2020_write_register(ViPipe, 0xae, 0x00);
	pr2020_write_register(ViPipe, 0xaf, 0xc1);
	pr2020_write_register(ViPipe, 0xb0, 0x0b);
	pr2020_write_register(ViPipe, 0xb1, 0x99);
	pr2020_write_register(ViPipe, 0xb2, 0x12);
	pr2020_write_register(ViPipe, 0xb3, 0xca);
	pr2020_write_register(ViPipe, 0xb4, 0x00);
	pr2020_write_register(ViPipe, 0xb5, 0x17);
	pr2020_write_register(ViPipe, 0xb6, 0x08);
	pr2020_write_register(ViPipe, 0xb7, 0xe8);
	pr2020_write_register(ViPipe, 0xb8, 0xb0);
	pr2020_write_register(ViPipe, 0xb9, 0xce);
	pr2020_write_register(ViPipe, 0xba, 0x90);
	pr2020_write_register(ViPipe, 0xbb, 0x00);
	pr2020_write_register(ViPipe, 0xbc, 0x00);
	pr2020_write_register(ViPipe, 0xbd, 0x04);
	pr2020_write_register(ViPipe, 0xbe, 0x05);
	pr2020_write_register(ViPipe, 0xbf, 0x00);
	pr2020_write_register(ViPipe, 0xc0, 0x00);
	pr2020_write_register(ViPipe, 0xc1, 0x12);
	pr2020_write_register(ViPipe, 0xc2, 0x02);
	pr2020_write_register(ViPipe, 0xc3, 0xd0);

	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0e);
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
}

void pr2020_set_720p_30(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	//Page0 sys
	pr2020_write_register(ViPipe, 0xff, 0x00);
	pr2020_write_register(ViPipe, 0x10, 0x92);
	pr2020_write_register(ViPipe, 0x11, 0x07);
	pr2020_write_register(ViPipe, 0x12, 0x00);
	pr2020_write_register(ViPipe, 0x13, 0x00);
	pr2020_write_register(ViPipe, 0x14, 0x21);//b[1:0); => Select Camera Input. VinP(1), VinN(3), Differ(0).
	pr2020_write_register(ViPipe, 0x15, 0x44);
	pr2020_write_register(ViPipe, 0x16, 0x0d);
	pr2020_write_register(ViPipe, 0x40, 0x00);
	pr2020_write_register(ViPipe, 0x47, 0x3a);
	pr2020_write_register(ViPipe, 0x4e, 0x3f);
	pr2020_write_register(ViPipe, 0x80, 0x56);
	pr2020_write_register(ViPipe, 0x81, 0x0e);
	pr2020_write_register(ViPipe, 0x82, 0x0d);
	pr2020_write_register(ViPipe, 0x84, 0x30);
	pr2020_write_register(ViPipe, 0x86, 0x20);
	pr2020_write_register(ViPipe, 0x87, 0x00);
	pr2020_write_register(ViPipe, 0x8a, 0x00);
	pr2020_write_register(ViPipe, 0x90, 0x00);
	pr2020_write_register(ViPipe, 0x91, 0x00);
	pr2020_write_register(ViPipe, 0x92, 0x00);
	pr2020_write_register(ViPipe, 0x94, 0xff);
	pr2020_write_register(ViPipe, 0x95, 0xff);
	pr2020_write_register(ViPipe, 0x96, 0xff);
	pr2020_write_register(ViPipe, 0xa0, 0x00);
	pr2020_write_register(ViPipe, 0xa1, 0x20);
	pr2020_write_register(ViPipe, 0xa4, 0x01);
	pr2020_write_register(ViPipe, 0xa5, 0xe3);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0x12);
	pr2020_write_register(ViPipe, 0xa8, 0x00);
	pr2020_write_register(ViPipe, 0xd0, 0x30);
	pr2020_write_register(ViPipe, 0xd1, 0x08);
	pr2020_write_register(ViPipe, 0xd2, 0x21);
	pr2020_write_register(ViPipe, 0xd3, 0x00);
	pr2020_write_register(ViPipe, 0xd8, 0x31);
	pr2020_write_register(ViPipe, 0xd9, 0x08);
	pr2020_write_register(ViPipe, 0xda, 0x21);
	pr2020_write_register(ViPipe, 0xe0, 0x39);
	pr2020_write_register(ViPipe, 0xe1, 0x90);
	pr2020_write_register(ViPipe, 0xe2, 0x38);
	pr2020_write_register(ViPipe, 0xe3, 0x19);
	pr2020_write_register(ViPipe, 0xe4, 0x19);
	pr2020_write_register(ViPipe, 0xea, 0x01);
	pr2020_write_register(ViPipe, 0xeb, 0xff);
	pr2020_write_register(ViPipe, 0xf1, 0x44);
	pr2020_write_register(ViPipe, 0xf2, 0x01);

	//Page1 vdec
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xe4);
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0c);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xb2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0a, 0x02);
	pr2020_write_register(ViPipe, 0x0b, 0x14);
	pr2020_write_register(ViPipe, 0x0c, 0x04);
	pr2020_write_register(ViPipe, 0x0d, 0x08);
	pr2020_write_register(ViPipe, 0x0e, 0x5e);
	pr2020_write_register(ViPipe, 0x0f, 0x5e);
	pr2020_write_register(ViPipe, 0x10, 0x26);
	pr2020_write_register(ViPipe, 0x11, 0x00);
	pr2020_write_register(ViPipe, 0x12, 0x45);
	pr2020_write_register(ViPipe, 0x13, 0xfc);
	pr2020_write_register(ViPipe, 0x14, 0x00);
	pr2020_write_register(ViPipe, 0x15, 0x18);
	pr2020_write_register(ViPipe, 0x16, 0xd0);
	pr2020_write_register(ViPipe, 0x17, 0x00);
	pr2020_write_register(ViPipe, 0x18, 0x41);
	pr2020_write_register(ViPipe, 0x19, 0x46);
	pr2020_write_register(ViPipe, 0x1a, 0x22);
	pr2020_write_register(ViPipe, 0x1b, 0x05);
	pr2020_write_register(ViPipe, 0x1c, 0xea);
	pr2020_write_register(ViPipe, 0x1d, 0x45);
	pr2020_write_register(ViPipe, 0x1e, 0x40);
	pr2020_write_register(ViPipe, 0x1f, 0x00);
	pr2020_write_register(ViPipe, 0x20, 0x80);
	pr2020_write_register(ViPipe, 0x21, 0x80);
	pr2020_write_register(ViPipe, 0x22, 0x90);
	pr2020_write_register(ViPipe, 0x23, 0x80);
	pr2020_write_register(ViPipe, 0x24, 0x80);
	pr2020_write_register(ViPipe, 0x25, 0x80);
	pr2020_write_register(ViPipe, 0x26, 0x84);
	pr2020_write_register(ViPipe, 0x27, 0x82);
	pr2020_write_register(ViPipe, 0x28, 0x00);
	pr2020_write_register(ViPipe, 0x29, 0x7b);
	pr2020_write_register(ViPipe, 0x2a, 0xa6);
	pr2020_write_register(ViPipe, 0x2b, 0x00);
	pr2020_write_register(ViPipe, 0x2c, 0x00);
	pr2020_write_register(ViPipe, 0x2d, 0x00);
	pr2020_write_register(ViPipe, 0x2e, 0x00);
	pr2020_write_register(ViPipe, 0x2f, 0x00);
	pr2020_write_register(ViPipe, 0x30, 0x00);
	pr2020_write_register(ViPipe, 0x31, 0x00);
	pr2020_write_register(ViPipe, 0x32, 0xc0);
	pr2020_write_register(ViPipe, 0x33, 0x14);
	pr2020_write_register(ViPipe, 0x34, 0x14);
	pr2020_write_register(ViPipe, 0x35, 0x80);
	pr2020_write_register(ViPipe, 0x36, 0x80);
	pr2020_write_register(ViPipe, 0x37, 0xaa);
	pr2020_write_register(ViPipe, 0x38, 0x48);
	pr2020_write_register(ViPipe, 0x39, 0x08);
	pr2020_write_register(ViPipe, 0x3a, 0x27);
	pr2020_write_register(ViPipe, 0x3b, 0x02);
	pr2020_write_register(ViPipe, 0x3c, 0x01);
	pr2020_write_register(ViPipe, 0x3d, 0x23);
	pr2020_write_register(ViPipe, 0x3e, 0x02);
	pr2020_write_register(ViPipe, 0x3f, 0xc4);
	pr2020_write_register(ViPipe, 0x40, 0x05);
	pr2020_write_register(ViPipe, 0x41, 0x55);
	pr2020_write_register(ViPipe, 0x42, 0x01);
	pr2020_write_register(ViPipe, 0x43, 0x33);
	pr2020_write_register(ViPipe, 0x44, 0x6a);
	pr2020_write_register(ViPipe, 0x45, 0x00);
	pr2020_write_register(ViPipe, 0x46, 0x09);
	pr2020_write_register(ViPipe, 0x47, 0xdc);
	pr2020_write_register(ViPipe, 0x48, 0xa0);
	pr2020_write_register(ViPipe, 0x49, 0x00);
	pr2020_write_register(ViPipe, 0x4a, 0x7b);
	pr2020_write_register(ViPipe, 0x4b, 0x60);
	pr2020_write_register(ViPipe, 0x4c, 0x00);
	pr2020_write_register(ViPipe, 0x4d, 0x4a);
	pr2020_write_register(ViPipe, 0x4e, 0x00);
	pr2020_write_register(ViPipe, 0x4f, 0x20);
	pr2020_write_register(ViPipe, 0x50, 0x01);
	pr2020_write_register(ViPipe, 0x51, 0x28);
	pr2020_write_register(ViPipe, 0x52, 0x40);
	pr2020_write_register(ViPipe, 0x53, 0x0c);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
	pr2020_write_register(ViPipe, 0x55, 0x8d);
	pr2020_write_register(ViPipe, 0x70, 0x06);
	pr2020_write_register(ViPipe, 0x71, 0x08);
	pr2020_write_register(ViPipe, 0x72, 0x0a);
	pr2020_write_register(ViPipe, 0x73, 0x0c);
	pr2020_write_register(ViPipe, 0x74, 0x0e);
	pr2020_write_register(ViPipe, 0x75, 0x10);
	pr2020_write_register(ViPipe, 0x76, 0x12);
	pr2020_write_register(ViPipe, 0x77, 0x14);
	pr2020_write_register(ViPipe, 0x78, 0x06);
	pr2020_write_register(ViPipe, 0x79, 0x08);
	pr2020_write_register(ViPipe, 0x7a, 0x0a);
	pr2020_write_register(ViPipe, 0x7b, 0x0c);
	pr2020_write_register(ViPipe, 0x7c, 0x0e);
	pr2020_write_register(ViPipe, 0x7d, 0x10);
	pr2020_write_register(ViPipe, 0x7e, 0x12);
	pr2020_write_register(ViPipe, 0x7f, 0x14);
	pr2020_write_register(ViPipe, 0x80, 0x00);
	pr2020_write_register(ViPipe, 0x81, 0x09);
	pr2020_write_register(ViPipe, 0x82, 0x00);
	pr2020_write_register(ViPipe, 0x83, 0x07);
	pr2020_write_register(ViPipe, 0x84, 0x00);
	pr2020_write_register(ViPipe, 0x85, 0x17);
	pr2020_write_register(ViPipe, 0x86, 0x03);
	pr2020_write_register(ViPipe, 0x87, 0xe5);
	pr2020_write_register(ViPipe, 0x88, 0x08);
	pr2020_write_register(ViPipe, 0x89, 0x91);
	pr2020_write_register(ViPipe, 0x8a, 0x08);
	pr2020_write_register(ViPipe, 0x8b, 0x91);
	pr2020_write_register(ViPipe, 0x8c, 0x0b);
	pr2020_write_register(ViPipe, 0x8d, 0xe0);
	pr2020_write_register(ViPipe, 0x8e, 0x05);
	pr2020_write_register(ViPipe, 0x8f, 0x47);
	pr2020_write_register(ViPipe, 0x90, 0x05);
	pr2020_write_register(ViPipe, 0x91, 0xa0);
	pr2020_write_register(ViPipe, 0x92, 0x73);
	pr2020_write_register(ViPipe, 0x93, 0xe8);
	pr2020_write_register(ViPipe, 0x94, 0x0f);
	pr2020_write_register(ViPipe, 0x95, 0x5e);
	pr2020_write_register(ViPipe, 0x96, 0x07);
	pr2020_write_register(ViPipe, 0x97, 0x90);
	pr2020_write_register(ViPipe, 0x98, 0x17);
	pr2020_write_register(ViPipe, 0x99, 0x34);
	pr2020_write_register(ViPipe, 0x9a, 0x13);
	pr2020_write_register(ViPipe, 0x9b, 0x56);
	pr2020_write_register(ViPipe, 0x9c, 0x0b);
	pr2020_write_register(ViPipe, 0x9d, 0x9a);
	pr2020_write_register(ViPipe, 0x9e, 0x09);
	pr2020_write_register(ViPipe, 0x9f, 0xab);
	pr2020_write_register(ViPipe, 0xa0, 0x01);
	pr2020_write_register(ViPipe, 0xa1, 0x74);
	pr2020_write_register(ViPipe, 0xa2, 0x01);
	pr2020_write_register(ViPipe, 0xa3, 0x6b);
	pr2020_write_register(ViPipe, 0xa4, 0x00);
	pr2020_write_register(ViPipe, 0xa5, 0xba);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0xa3);
	pr2020_write_register(ViPipe, 0xa8, 0x01);
	pr2020_write_register(ViPipe, 0xa9, 0x39);
	pr2020_write_register(ViPipe, 0xaa, 0x01);
	pr2020_write_register(ViPipe, 0xab, 0x39);
	pr2020_write_register(ViPipe, 0xac, 0x00);
	pr2020_write_register(ViPipe, 0xad, 0xc1);
	pr2020_write_register(ViPipe, 0xae, 0x00);
	pr2020_write_register(ViPipe, 0xaf, 0xc1);
	pr2020_write_register(ViPipe, 0xb0, 0x09);
	pr2020_write_register(ViPipe, 0xb1, 0xaa);
	pr2020_write_register(ViPipe, 0xb2, 0x0f);
	pr2020_write_register(ViPipe, 0xb3, 0xae);
	pr2020_write_register(ViPipe, 0xb4, 0x00);
	pr2020_write_register(ViPipe, 0xb5, 0x17);
	pr2020_write_register(ViPipe, 0xb6, 0x08);
	pr2020_write_register(ViPipe, 0xb7, 0xe8);
	pr2020_write_register(ViPipe, 0xb8, 0xb0);
	pr2020_write_register(ViPipe, 0xb9, 0xce);
	pr2020_write_register(ViPipe, 0xba, 0x90);
	pr2020_write_register(ViPipe, 0xbb, 0x00);
	pr2020_write_register(ViPipe, 0xbc, 0x00);
	pr2020_write_register(ViPipe, 0xbd, 0x04);
	pr2020_write_register(ViPipe, 0xbe, 0x05);
	pr2020_write_register(ViPipe, 0xbf, 0x00);
	pr2020_write_register(ViPipe, 0xc0, 0x00);
	pr2020_write_register(ViPipe, 0xc1, 0x18);
	pr2020_write_register(ViPipe, 0xc2, 0x02);
	pr2020_write_register(ViPipe, 0xc3, 0xd0);

	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0e);
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
}

void pr2020_set_1080p_25(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	//Page0 sys
	pr2020_write_register(ViPipe, 0xff, 0x00);
	pr2020_write_register(ViPipe, 0x10, 0x83);
	pr2020_write_register(ViPipe, 0x11, 0x07);
	pr2020_write_register(ViPipe, 0x12, 0x00);
	pr2020_write_register(ViPipe, 0x13, 0x00);
	pr2020_write_register(ViPipe, 0x14, 0x21);//b[1:0} => Select Camera Input. VinP(1), VinN(3), Differ(0).
	pr2020_write_register(ViPipe, 0x15, 0x44);
	pr2020_write_register(ViPipe, 0x16, 0x0d);
	pr2020_write_register(ViPipe, 0x40, 0x00);
	pr2020_write_register(ViPipe, 0x47, 0x3a);
	pr2020_write_register(ViPipe, 0x4e, 0x3f);
	pr2020_write_register(ViPipe, 0x80, 0x56);
	pr2020_write_register(ViPipe, 0x81, 0x0e);
	pr2020_write_register(ViPipe, 0x82, 0x0d);
	pr2020_write_register(ViPipe, 0x84, 0x30);
	pr2020_write_register(ViPipe, 0x86, 0x20);
	pr2020_write_register(ViPipe, 0x87, 0x00);
	pr2020_write_register(ViPipe, 0x8a, 0x00);
	pr2020_write_register(ViPipe, 0x90, 0x00);
	pr2020_write_register(ViPipe, 0x91, 0x00);
	pr2020_write_register(ViPipe, 0x92, 0x00);
	pr2020_write_register(ViPipe, 0x94, 0xff);
	pr2020_write_register(ViPipe, 0x95, 0xff);
	pr2020_write_register(ViPipe, 0x96, 0xff);
	pr2020_write_register(ViPipe, 0xa0, 0x00);
	pr2020_write_register(ViPipe, 0xa1, 0x20);
	pr2020_write_register(ViPipe, 0xa4, 0x01);
	pr2020_write_register(ViPipe, 0xa5, 0xe3);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0x12);
	pr2020_write_register(ViPipe, 0xa8, 0x00);
	pr2020_write_register(ViPipe, 0xd0, 0x30);
	pr2020_write_register(ViPipe, 0xd1, 0x08);
	pr2020_write_register(ViPipe, 0xd2, 0x21);
	pr2020_write_register(ViPipe, 0xd3, 0x00);
	pr2020_write_register(ViPipe, 0xd8, 0x30);
	pr2020_write_register(ViPipe, 0xd9, 0x08);
	pr2020_write_register(ViPipe, 0xda, 0x21);
	pr2020_write_register(ViPipe, 0xe0, 0x35);
	pr2020_write_register(ViPipe, 0xe1, 0x80);//[6] 0:cb-y-cr-y 1:y-cb-y-cr
	pr2020_write_register(ViPipe, 0xe2, 0x18);
	pr2020_write_register(ViPipe, 0xe3, 0x00);
	pr2020_write_register(ViPipe, 0xe4, 0x00);
	pr2020_write_register(ViPipe, 0xea, 0x01);
	pr2020_write_register(ViPipe, 0xeb, 0xff);
	pr2020_write_register(ViPipe, 0xf1, 0x44);
	pr2020_write_register(ViPipe, 0xf2, 0x01);

	//Page1 vdec
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xe4);
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0c);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xb2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0a, 0x02);
	pr2020_write_register(ViPipe, 0x0b, 0x14);
	pr2020_write_register(ViPipe, 0x0c, 0x04);
	pr2020_write_register(ViPipe, 0x0d, 0x08);
	pr2020_write_register(ViPipe, 0x0e, 0x5e);
	pr2020_write_register(ViPipe, 0x0f, 0x5e);
	pr2020_write_register(ViPipe, 0x10, 0x26);
	pr2020_write_register(ViPipe, 0x11, 0x00);
	pr2020_write_register(ViPipe, 0x12, 0x87);
	pr2020_write_register(ViPipe, 0x13, 0x24);
	pr2020_write_register(ViPipe, 0x14, 0x80);
	pr2020_write_register(ViPipe, 0x15, 0x2a);
	pr2020_write_register(ViPipe, 0x16, 0x38);
	pr2020_write_register(ViPipe, 0x17, 0x00);
	pr2020_write_register(ViPipe, 0x18, 0x80);
	pr2020_write_register(ViPipe, 0x19, 0x48);
	pr2020_write_register(ViPipe, 0x1a, 0x6c);
	pr2020_write_register(ViPipe, 0x1b, 0x05);
	pr2020_write_register(ViPipe, 0x1c, 0x61);
	pr2020_write_register(ViPipe, 0x1d, 0x07);
	pr2020_write_register(ViPipe, 0x1e, 0x7e);
	pr2020_write_register(ViPipe, 0x1f, 0x80);
	pr2020_write_register(ViPipe, 0x20, 0x80);
	pr2020_write_register(ViPipe, 0x21, 0x80);
	pr2020_write_register(ViPipe, 0x22, 0x90);
	pr2020_write_register(ViPipe, 0x23, 0x80);
	pr2020_write_register(ViPipe, 0x24, 0x80);
	pr2020_write_register(ViPipe, 0x25, 0x80);
	pr2020_write_register(ViPipe, 0x26, 0x84);
	pr2020_write_register(ViPipe, 0x27, 0x82);
	pr2020_write_register(ViPipe, 0x28, 0x00);
	pr2020_write_register(ViPipe, 0x29, 0xff);
	pr2020_write_register(ViPipe, 0x2a, 0xff);
	pr2020_write_register(ViPipe, 0x2b, 0x00);
	pr2020_write_register(ViPipe, 0x2c, 0x00);
	pr2020_write_register(ViPipe, 0x2d, 0x00);
	pr2020_write_register(ViPipe, 0x2e, 0x00);
	pr2020_write_register(ViPipe, 0x2f, 0x00);
	pr2020_write_register(ViPipe, 0x30, 0x00);
	pr2020_write_register(ViPipe, 0x31, 0x00);
	pr2020_write_register(ViPipe, 0x32, 0xc0);
	pr2020_write_register(ViPipe, 0x33, 0x14);
	pr2020_write_register(ViPipe, 0x34, 0x14);
	pr2020_write_register(ViPipe, 0x35, 0x80);
	pr2020_write_register(ViPipe, 0x36, 0x80);
	pr2020_write_register(ViPipe, 0x37, 0xad);
	pr2020_write_register(ViPipe, 0x38, 0x4b);
	pr2020_write_register(ViPipe, 0x39, 0x08);
	pr2020_write_register(ViPipe, 0x3a, 0x21);
	pr2020_write_register(ViPipe, 0x3b, 0x02);
	pr2020_write_register(ViPipe, 0x3c, 0x01);
	pr2020_write_register(ViPipe, 0x3d, 0x23);
	pr2020_write_register(ViPipe, 0x3e, 0x05);
	pr2020_write_register(ViPipe, 0x3f, 0xc8);
	pr2020_write_register(ViPipe, 0x40, 0x05);
	pr2020_write_register(ViPipe, 0x41, 0x55);
	pr2020_write_register(ViPipe, 0x42, 0x01);
	pr2020_write_register(ViPipe, 0x43, 0x38);
	pr2020_write_register(ViPipe, 0x44, 0x6a);
	pr2020_write_register(ViPipe, 0x45, 0x00);
	pr2020_write_register(ViPipe, 0x46, 0x14);
	pr2020_write_register(ViPipe, 0x47, 0xb0);
	pr2020_write_register(ViPipe, 0x48, 0xdf);
	pr2020_write_register(ViPipe, 0x49, 0x00);
	pr2020_write_register(ViPipe, 0x4a, 0x7b);
	pr2020_write_register(ViPipe, 0x4b, 0x60);
	pr2020_write_register(ViPipe, 0x4c, 0x00);
	pr2020_write_register(ViPipe, 0x4d, 0x26);
	pr2020_write_register(ViPipe, 0x4e, 0x00);
	pr2020_write_register(ViPipe, 0x4f, 0x20);//RK:0x24
	pr2020_write_register(ViPipe, 0x50, 0x01);
	pr2020_write_register(ViPipe, 0x51, 0x28);
	pr2020_write_register(ViPipe, 0x52, 0x40);
	pr2020_write_register(ViPipe, 0x53, 0x0c);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
	pr2020_write_register(ViPipe, 0x55, 0x8d);
	pr2020_write_register(ViPipe, 0x70, 0x06);
	pr2020_write_register(ViPipe, 0x71, 0x08);
	pr2020_write_register(ViPipe, 0x72, 0x0a);
	pr2020_write_register(ViPipe, 0x73, 0x0c);
	pr2020_write_register(ViPipe, 0x74, 0x0e);
	pr2020_write_register(ViPipe, 0x75, 0x10);
	pr2020_write_register(ViPipe, 0x76, 0x12);
	pr2020_write_register(ViPipe, 0x77, 0x14);
	pr2020_write_register(ViPipe, 0x78, 0x06);
	pr2020_write_register(ViPipe, 0x79, 0x08);
	pr2020_write_register(ViPipe, 0x7a, 0x0a);
	pr2020_write_register(ViPipe, 0x7b, 0x0c);
	pr2020_write_register(ViPipe, 0x7c, 0x0e);
	pr2020_write_register(ViPipe, 0x7d, 0x10);
	pr2020_write_register(ViPipe, 0x7e, 0x12);
	pr2020_write_register(ViPipe, 0x7f, 0x14);
	pr2020_write_register(ViPipe, 0x80, 0x00);
	pr2020_write_register(ViPipe, 0x81, 0x09);
	pr2020_write_register(ViPipe, 0x82, 0x00);
	pr2020_write_register(ViPipe, 0x83, 0x07);
	pr2020_write_register(ViPipe, 0x84, 0x00);
	pr2020_write_register(ViPipe, 0x85, 0x17);
	pr2020_write_register(ViPipe, 0x86, 0x03);
	pr2020_write_register(ViPipe, 0x87, 0xe5);
	pr2020_write_register(ViPipe, 0x88, 0x05);
	pr2020_write_register(ViPipe, 0x89, 0x24);
	pr2020_write_register(ViPipe, 0x8a, 0x05);
	pr2020_write_register(ViPipe, 0x8b, 0x24);
	pr2020_write_register(ViPipe, 0x8c, 0x08);
	pr2020_write_register(ViPipe, 0x8d, 0xe8);
	pr2020_write_register(ViPipe, 0x8e, 0x05);
	pr2020_write_register(ViPipe, 0x8f, 0x47);
	pr2020_write_register(ViPipe, 0x90, 0x02);
	pr2020_write_register(ViPipe, 0x91, 0xb4);
	pr2020_write_register(ViPipe, 0x92, 0x73);
	pr2020_write_register(ViPipe, 0x93, 0xe8);
	pr2020_write_register(ViPipe, 0x94, 0x0f);
	pr2020_write_register(ViPipe, 0x95, 0x5e);
	pr2020_write_register(ViPipe, 0x96, 0x03);
	pr2020_write_register(ViPipe, 0x97, 0xd0);
	pr2020_write_register(ViPipe, 0x98, 0x17);
	pr2020_write_register(ViPipe, 0x99, 0x34);
	pr2020_write_register(ViPipe, 0x9a, 0x13);
	pr2020_write_register(ViPipe, 0x9b, 0x56);
	pr2020_write_register(ViPipe, 0x9c, 0x0b);
	pr2020_write_register(ViPipe, 0x9d, 0x9a);
	pr2020_write_register(ViPipe, 0x9e, 0x09);
	pr2020_write_register(ViPipe, 0x9f, 0xab);
	pr2020_write_register(ViPipe, 0xa0, 0x01);
	pr2020_write_register(ViPipe, 0xa1, 0x74);
	pr2020_write_register(ViPipe, 0xa2, 0x01);
	pr2020_write_register(ViPipe, 0xa3, 0x6b);
	pr2020_write_register(ViPipe, 0xa4, 0x00);
	pr2020_write_register(ViPipe, 0xa5, 0xba);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0xa3);
	pr2020_write_register(ViPipe, 0xa8, 0x01);
	pr2020_write_register(ViPipe, 0xa9, 0x39);
	pr2020_write_register(ViPipe, 0xaa, 0x01);
	pr2020_write_register(ViPipe, 0xab, 0x39);
	pr2020_write_register(ViPipe, 0xac, 0x00);
	pr2020_write_register(ViPipe, 0xad, 0xc1);
	pr2020_write_register(ViPipe, 0xae, 0x00);
	pr2020_write_register(ViPipe, 0xaf, 0xc1);
	pr2020_write_register(ViPipe, 0xb0, 0x05);
	pr2020_write_register(ViPipe, 0xb1, 0xcc);
	pr2020_write_register(ViPipe, 0xb2, 0x09);
	pr2020_write_register(ViPipe, 0xb3, 0x6d);
	pr2020_write_register(ViPipe, 0xb4, 0x00);
	pr2020_write_register(ViPipe, 0xb5, 0x17);
	pr2020_write_register(ViPipe, 0xb6, 0x08);
	pr2020_write_register(ViPipe, 0xb7, 0xe8);
	pr2020_write_register(ViPipe, 0xb8, 0xb0);
	pr2020_write_register(ViPipe, 0xb9, 0xce);
	pr2020_write_register(ViPipe, 0xba, 0x90);
	pr2020_write_register(ViPipe, 0xbb, 0x00);
	pr2020_write_register(ViPipe, 0xbc, 0x00);
	pr2020_write_register(ViPipe, 0xbd, 0x04);
	pr2020_write_register(ViPipe, 0xbe, 0x07);
	pr2020_write_register(ViPipe, 0xbf, 0x80);
	pr2020_write_register(ViPipe, 0xc0, 0x00);
	pr2020_write_register(ViPipe, 0xc1, 0x20);
	pr2020_write_register(ViPipe, 0xc2, 0x04);
	pr2020_write_register(ViPipe, 0xc3, 0x38);

	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0e);
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
}

void pr2020_set_1080p_30(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe=%d\n", ViPipe);

	//Page0 sys
	pr2020_write_register(ViPipe, 0xff, 0x00);
	pr2020_write_register(ViPipe, 0x10, 0x93);
	pr2020_write_register(ViPipe, 0x11, 0x07);
	pr2020_write_register(ViPipe, 0x12, 0x00);
	pr2020_write_register(ViPipe, 0x13, 0x00);
	pr2020_write_register(ViPipe, 0x14, 0x21);//b[1:0} => Select Camera Input. VinP(1), VinN(3), Differ(0).
	pr2020_write_register(ViPipe, 0x15, 0x44);
	pr2020_write_register(ViPipe, 0x16, 0x0d);
	pr2020_write_register(ViPipe, 0x40, 0x00);
	pr2020_write_register(ViPipe, 0x47, 0x3a);
	pr2020_write_register(ViPipe, 0x4e, 0x3f);
	pr2020_write_register(ViPipe, 0x80, 0x56);
	pr2020_write_register(ViPipe, 0x81, 0x0e);
	pr2020_write_register(ViPipe, 0x82, 0x0d);
	pr2020_write_register(ViPipe, 0x84, 0x30);
	pr2020_write_register(ViPipe, 0x86, 0x20);
	pr2020_write_register(ViPipe, 0x87, 0x00);
	pr2020_write_register(ViPipe, 0x8a, 0x00);
	pr2020_write_register(ViPipe, 0x90, 0x00);
	pr2020_write_register(ViPipe, 0x91, 0x00);
	pr2020_write_register(ViPipe, 0x92, 0x00);
	pr2020_write_register(ViPipe, 0x94, 0xff);
	pr2020_write_register(ViPipe, 0x95, 0xff);
	pr2020_write_register(ViPipe, 0x96, 0xff);
	pr2020_write_register(ViPipe, 0xa0, 0x00);
	pr2020_write_register(ViPipe, 0xa1, 0x20);
	pr2020_write_register(ViPipe, 0xa4, 0x01);
	pr2020_write_register(ViPipe, 0xa5, 0xe3);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0x12);
	pr2020_write_register(ViPipe, 0xa8, 0x00);
	pr2020_write_register(ViPipe, 0xd0, 0x30);
	pr2020_write_register(ViPipe, 0xd1, 0x08);
	pr2020_write_register(ViPipe, 0xd2, 0x21);
	pr2020_write_register(ViPipe, 0xd3, 0x00);
	pr2020_write_register(ViPipe, 0xd8, 0x30);
	pr2020_write_register(ViPipe, 0xd9, 0x08);
	pr2020_write_register(ViPipe, 0xda, 0x21);
	pr2020_write_register(ViPipe, 0xe0, 0x35);
	pr2020_write_register(ViPipe, 0xe1, 0x80);
	pr2020_write_register(ViPipe, 0xe2, 0x18);
	pr2020_write_register(ViPipe, 0xe3, 0x00);
	pr2020_write_register(ViPipe, 0xe4, 0x00);
	pr2020_write_register(ViPipe, 0xea, 0x01);
	pr2020_write_register(ViPipe, 0xeb, 0xff);
	pr2020_write_register(ViPipe, 0xf1, 0x44);
	pr2020_write_register(ViPipe, 0xf2, 0x01);

	//Page1 vdec
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x00, 0xe4);
	pr2020_write_register(ViPipe, 0x01, 0x61);
	pr2020_write_register(ViPipe, 0x02, 0x00);
	pr2020_write_register(ViPipe, 0x03, 0x57);
	pr2020_write_register(ViPipe, 0x04, 0x0c);
	pr2020_write_register(ViPipe, 0x05, 0x88);
	pr2020_write_register(ViPipe, 0x06, 0x04);
	pr2020_write_register(ViPipe, 0x07, 0xb2);
	pr2020_write_register(ViPipe, 0x08, 0x44);
	pr2020_write_register(ViPipe, 0x09, 0x34);
	pr2020_write_register(ViPipe, 0x0a, 0x02);
	pr2020_write_register(ViPipe, 0x0b, 0x14);
	pr2020_write_register(ViPipe, 0x0c, 0x04);
	pr2020_write_register(ViPipe, 0x0d, 0x08);
	pr2020_write_register(ViPipe, 0x0e, 0x5e);
	pr2020_write_register(ViPipe, 0x0f, 0x5e);
	pr2020_write_register(ViPipe, 0x10, 0x26);
	pr2020_write_register(ViPipe, 0x11, 0x00);
	pr2020_write_register(ViPipe, 0x12, 0x87);
	pr2020_write_register(ViPipe, 0x13, 0x2c);
	pr2020_write_register(ViPipe, 0x14, 0x80);
	pr2020_write_register(ViPipe, 0x15, 0x28);
	pr2020_write_register(ViPipe, 0x16, 0x38);
	pr2020_write_register(ViPipe, 0x17, 0x00);
	pr2020_write_register(ViPipe, 0x18, 0x80);
	pr2020_write_register(ViPipe, 0x19, 0x48);
	pr2020_write_register(ViPipe, 0x1a, 0x6c);
	pr2020_write_register(ViPipe, 0x1b, 0x05);
	pr2020_write_register(ViPipe, 0x1c, 0x61);
	pr2020_write_register(ViPipe, 0x1d, 0x07);
	pr2020_write_register(ViPipe, 0x1e, 0x7e);
	pr2020_write_register(ViPipe, 0x1f, 0x80);
	pr2020_write_register(ViPipe, 0x20, 0x80);
	pr2020_write_register(ViPipe, 0x21, 0x80);
	pr2020_write_register(ViPipe, 0x22, 0x90);
	pr2020_write_register(ViPipe, 0x23, 0x80);
	pr2020_write_register(ViPipe, 0x24, 0x80);
	pr2020_write_register(ViPipe, 0x25, 0x80);
	pr2020_write_register(ViPipe, 0x26, 0x84);
	pr2020_write_register(ViPipe, 0x27, 0x82);
	pr2020_write_register(ViPipe, 0x28, 0x00);
	pr2020_write_register(ViPipe, 0x29, 0xff);
	pr2020_write_register(ViPipe, 0x2a, 0xff);
	pr2020_write_register(ViPipe, 0x2b, 0x00);
	pr2020_write_register(ViPipe, 0x2c, 0x00);
	pr2020_write_register(ViPipe, 0x2d, 0x00);
	pr2020_write_register(ViPipe, 0x2e, 0x00);
	pr2020_write_register(ViPipe, 0x2f, 0x00);
	pr2020_write_register(ViPipe, 0x30, 0x00);
	pr2020_write_register(ViPipe, 0x31, 0x00);
	pr2020_write_register(ViPipe, 0x32, 0xc0);
	pr2020_write_register(ViPipe, 0x33, 0x14);
	pr2020_write_register(ViPipe, 0x34, 0x14);
	pr2020_write_register(ViPipe, 0x35, 0x80);
	pr2020_write_register(ViPipe, 0x36, 0x80);
	pr2020_write_register(ViPipe, 0x37, 0xad);
	pr2020_write_register(ViPipe, 0x38, 0x4b);
	pr2020_write_register(ViPipe, 0x39, 0x08);
	pr2020_write_register(ViPipe, 0x3a, 0x21);
	pr2020_write_register(ViPipe, 0x3b, 0x02);
	pr2020_write_register(ViPipe, 0x3c, 0x01);
	pr2020_write_register(ViPipe, 0x3d, 0x23);
	pr2020_write_register(ViPipe, 0x3e, 0x05);
	pr2020_write_register(ViPipe, 0x3f, 0xc8);
	pr2020_write_register(ViPipe, 0x40, 0x05);
	pr2020_write_register(ViPipe, 0x41, 0x55);
	pr2020_write_register(ViPipe, 0x42, 0x01);
	pr2020_write_register(ViPipe, 0x43, 0x38);
	pr2020_write_register(ViPipe, 0x44, 0x6a);
	pr2020_write_register(ViPipe, 0x45, 0x00);
	pr2020_write_register(ViPipe, 0x46, 0x14);
	pr2020_write_register(ViPipe, 0x47, 0xb2);
	pr2020_write_register(ViPipe, 0x48, 0xbc);
	pr2020_write_register(ViPipe, 0x49, 0x00);
	pr2020_write_register(ViPipe, 0x4a, 0x7b);
	pr2020_write_register(ViPipe, 0x4b, 0x60);
	pr2020_write_register(ViPipe, 0x4c, 0x00);
	pr2020_write_register(ViPipe, 0x4d, 0x26);
	pr2020_write_register(ViPipe, 0x4e, 0x00);
	pr2020_write_register(ViPipe, 0x4f, 0x20);//RK:0x24
	pr2020_write_register(ViPipe, 0x50, 0x01);
	pr2020_write_register(ViPipe, 0x51, 0x28);
	pr2020_write_register(ViPipe, 0x52, 0x40);
	pr2020_write_register(ViPipe, 0x53, 0x0c);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
	pr2020_write_register(ViPipe, 0x55, 0x8d);
	pr2020_write_register(ViPipe, 0x70, 0x06);
	pr2020_write_register(ViPipe, 0x71, 0x08);
	pr2020_write_register(ViPipe, 0x72, 0x0a);
	pr2020_write_register(ViPipe, 0x73, 0x0c);
	pr2020_write_register(ViPipe, 0x74, 0x0e);
	pr2020_write_register(ViPipe, 0x75, 0x10);
	pr2020_write_register(ViPipe, 0x76, 0x12);
	pr2020_write_register(ViPipe, 0x77, 0x14);
	pr2020_write_register(ViPipe, 0x78, 0x06);
	pr2020_write_register(ViPipe, 0x79, 0x08);
	pr2020_write_register(ViPipe, 0x7a, 0x0a);
	pr2020_write_register(ViPipe, 0x7b, 0x0c);
	pr2020_write_register(ViPipe, 0x7c, 0x0e);
	pr2020_write_register(ViPipe, 0x7d, 0x10);
	pr2020_write_register(ViPipe, 0x7e, 0x12);
	pr2020_write_register(ViPipe, 0x7f, 0x14);
	pr2020_write_register(ViPipe, 0x80, 0x00);
	pr2020_write_register(ViPipe, 0x81, 0x09);
	pr2020_write_register(ViPipe, 0x82, 0x00);
	pr2020_write_register(ViPipe, 0x83, 0x07);
	pr2020_write_register(ViPipe, 0x84, 0x00);
	pr2020_write_register(ViPipe, 0x85, 0x17);
	pr2020_write_register(ViPipe, 0x86, 0x03);
	pr2020_write_register(ViPipe, 0x87, 0xe5);
	pr2020_write_register(ViPipe, 0x88, 0x04);
	pr2020_write_register(ViPipe, 0x89, 0x48);
	pr2020_write_register(ViPipe, 0x8a, 0x04);
	pr2020_write_register(ViPipe, 0x8b, 0x48);
	pr2020_write_register(ViPipe, 0x8c, 0x08);
	pr2020_write_register(ViPipe, 0x8d, 0xe8);
	pr2020_write_register(ViPipe, 0x8e, 0x05);
	pr2020_write_register(ViPipe, 0x8f, 0x47);
	pr2020_write_register(ViPipe, 0x90, 0x03);
	pr2020_write_register(ViPipe, 0x91, 0x13);
	pr2020_write_register(ViPipe, 0x92, 0x73);
	pr2020_write_register(ViPipe, 0x93, 0xe8);
	pr2020_write_register(ViPipe, 0x94, 0x0f);
	pr2020_write_register(ViPipe, 0x95, 0x5e);
	pr2020_write_register(ViPipe, 0x96, 0x03);
	pr2020_write_register(ViPipe, 0x97, 0xd0);
	pr2020_write_register(ViPipe, 0x98, 0x17);
	pr2020_write_register(ViPipe, 0x99, 0x34);
	pr2020_write_register(ViPipe, 0x9a, 0x13);
	pr2020_write_register(ViPipe, 0x9b, 0x56);
	pr2020_write_register(ViPipe, 0x9c, 0x0b);
	pr2020_write_register(ViPipe, 0x9d, 0x9a);
	pr2020_write_register(ViPipe, 0x9e, 0x09);
	pr2020_write_register(ViPipe, 0x9f, 0xab);
	pr2020_write_register(ViPipe, 0xa0, 0x01);
	pr2020_write_register(ViPipe, 0xa1, 0x74);
	pr2020_write_register(ViPipe, 0xa2, 0x01);
	pr2020_write_register(ViPipe, 0xa3, 0x6b);
	pr2020_write_register(ViPipe, 0xa4, 0x00);
	pr2020_write_register(ViPipe, 0xa5, 0xba);
	pr2020_write_register(ViPipe, 0xa6, 0x00);
	pr2020_write_register(ViPipe, 0xa7, 0xa3);
	pr2020_write_register(ViPipe, 0xa8, 0x01);
	pr2020_write_register(ViPipe, 0xa9, 0x39);
	pr2020_write_register(ViPipe, 0xaa, 0x01);
	pr2020_write_register(ViPipe, 0xab, 0x39);
	pr2020_write_register(ViPipe, 0xac, 0x00);
	pr2020_write_register(ViPipe, 0xad, 0xc1);
	pr2020_write_register(ViPipe, 0xae, 0x00);
	pr2020_write_register(ViPipe, 0xaf, 0xc1);
	pr2020_write_register(ViPipe, 0xb0, 0x04);
	pr2020_write_register(ViPipe, 0xb1, 0xd4);
	pr2020_write_register(ViPipe, 0xb2, 0x07);
	pr2020_write_register(ViPipe, 0xb3, 0xda);
	pr2020_write_register(ViPipe, 0xb4, 0x00);
	pr2020_write_register(ViPipe, 0xb5, 0x17);
	pr2020_write_register(ViPipe, 0xb6, 0x08);
	pr2020_write_register(ViPipe, 0xb7, 0xe8);
	pr2020_write_register(ViPipe, 0xb8, 0xb0);
	pr2020_write_register(ViPipe, 0xb9, 0xce);
	pr2020_write_register(ViPipe, 0xba, 0x90);
	pr2020_write_register(ViPipe, 0xbb, 0x00);
	pr2020_write_register(ViPipe, 0xbc, 0x00);
	pr2020_write_register(ViPipe, 0xbd, 0x04);
	pr2020_write_register(ViPipe, 0xbe, 0x07);
	pr2020_write_register(ViPipe, 0xbf, 0x80);
	pr2020_write_register(ViPipe, 0xc0, 0x00);
	pr2020_write_register(ViPipe, 0xc1, 0x20);
	pr2020_write_register(ViPipe, 0xc2, 0x04);
	pr2020_write_register(ViPipe, 0xc3, 0x38);

	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0e);
	pr2020_write_register(ViPipe, 0xff, 0x01);
	pr2020_write_register(ViPipe, 0x54, 0x0f);
}

#if (PR2020_AUTO_DETECT)
void pr2000_chip_init(VI_PIPE ViPipe)
{
	CVI_U8 lockstatus = 0;
	CVI_U8 detvideo = 0;
	CVI_U8 temp = 0;

	lockstatus = pr2020_read_register(ViPipe, 0x01);
	detvideo = pr2020_read_register(ViPipe, 0x00);
	temp = pr2020_read_register(ViPipe, 0x10);
	CVI_TRACE_SNS(CVI_DBG_INFO, "detvideo = 0x%2x, lockstatus = 0x%2x, signal_type = %d, temp = 0x%2x!!!\n",
			detvideo, lockstatus, signal_type, temp);
	if (((lockstatus & 0x18) == 0x18) && ((detvideo & 0x08) == 0x08)) { //camera plug in
		//for test start
		if ((detvideo & 0x03) == 0x00) { //NTSC
			if (signal_type != PR2020_MODE_720H_NTSC) {//shold set reg again
				pr2020_write_register(ViPipe, 0xff, 0x00);
				temp = pr2020_read_register(ViPipe, 0x11);
				if (temp != 0x00)
					pr2020_write_register(ViPipe, 0x11, 0x00);
				delay_ms(200);
				pr2020_set_cvbs_ntsc_60(ViPipe);
				signal_type = PR2020_MODE_720H_NTSC;
				CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 download NTSC reg!\n");
			}
		} else if ((detvideo & 0x03) == 0x01) { //PAL
			if (signal_type != PR2020_MODE_720H_PAL) {//shold set reg again
				pr2020_write_register(ViPipe, 0xff, 0x00);
				temp = pr2020_read_register(ViPipe, 0x11);
				if (temp != 0x00)
					pr2020_write_register(ViPipe, 0x11, 0x00);
				delay_ms(200);
				pr2020_set_cvbs_pal_50(ViPipe);
				signal_type = PR2020_MODE_720H_PAL;
				CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 download PAL reg!\n");
			}
		} else if ((detvideo & 0x03) == 0x02) { //720p
			if ((detvideo & 0x30) == 0x00) { //25fps
				if (signal_type != PR2020_MODE_720P_25) { //shold set reg again
					pr2020_write_register(ViPipe, 0xff, 0x00);
					temp = pr2020_read_register(ViPipe, 0x11);
					if (temp != 0x00)
						pr2020_write_register(ViPipe, 0x11, 0x00);
					delay_ms(200);
					pr2020_set_720p_25(ViPipe);
					signal_type = PR2020_MODE_720P_25;
					CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 download 720P 25fps reg!\n");
				}
			} else if ((detvideo & 0x30) == 0x10) { //30fps
				if (signal_type != PR2020_MODE_720P_30) { //shold set reg again
					pr2020_write_register(ViPipe, 0xff, 0x00);
					temp = pr2020_read_register(ViPipe, 0x11);
					if (temp != 0x00)
						pr2020_write_register(ViPipe, 0x11, 0x00);
					delay_ms(200);
					pr2020_set_720p_30(ViPipe);
					signal_type = PR2020_MODE_720P_30;
					CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 download 720P 30fps reg!\n");
				}
			} else if ((detvideo & 0x30) == 0x20) { //50fps
				signal_type = PR2020_MODE_NONE;
				CVI_TRACE_SNS(CVI_DBG_INFO, "detect video fmt is 720P 50fps\n");
			} else if ((detvideo & 0x30) == 0x30) { //60fps
				signal_type = PR2020_MODE_NONE;
				CVI_TRACE_SNS(CVI_DBG_INFO, "detect video fmt is 720P 60fps\n");
			}
		} else if ((detvideo & 0x03) == 0x03) { //1080p
			if ((detvideo & 0x30) == 0x00) { //25fps
				if (signal_type != PR2020_MODE_1080P_25) { //shold set reg again
					pr2020_write_register(ViPipe, 0xff, 0x00);
					temp = pr2020_read_register(ViPipe, 0x11);
					if (temp != 0x00)
						pr2020_write_register(ViPipe, 0x11, 0x00);
					delay_ms(200);
					pr2020_set_1080p_25(ViPipe);
					signal_type = PR2020_MODE_1080P_25;
					CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 download full hd 1080p 25fps reg!\n");
				}
			} else if ((detvideo & 0x30) == 0x10) { //30fps
				if (signal_type != PR2020_MODE_1080P_30) { //shold set reg again
					pr2020_write_register(ViPipe, 0xff, 0x00);
					temp = pr2020_read_register(ViPipe, 0x11);
					if (temp != 0x00)
						pr2020_write_register(ViPipe, 0x11, 0x00);
					delay_ms(200);
					pr2020_set_1080p_30(ViPipe);
					signal_type = PR2020_MODE_1080P_30;
					CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 download full hd 1080p 30fps reg!\n");
				}
			}
		} else {
			CVI_TRACE_SNS(CVI_DBG_INFO, "detect nothing!!!\n");
			signal_type = PR2020_MODE_NONE;
		}
		pr2020_write_register(ViPipe, 0xff, 0x00);
#if 0
		//for test end
		//read camera plug and signal state
		//mdelay(100);
		CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 read reg 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x\n",
				pr2020_read_register(ViPipe, 0xff),
				pr2020_read_register(ViPipe, 0x00),
				pr2020_read_register(ViPipe, 0x01),
				pr2020_read_register(ViPipe, 0x10),
				pr2020_read_register(ViPipe, 0x11));

#endif
	} else {
		pr2020_write_register(ViPipe, 0xff, 0x00);
		pr2020_write_register(ViPipe, 0x11, 0x00);
		signal_type = PR2020_MODE_NONE;
		CVI_TRACE_SNS(CVI_DBG_INFO, "pr2020 has no signal!\n");
	}
}

static void *pr2020_device_auto_detect(void *arg)
{
	VI_PIPE ViPipe = *(CVI_U8 *)arg;
	PR2020_MODE_E signal_type_old = signal_type;

	free(arg);
	while (1) {
		delay_ms(500); //500ms
		pr2000_chip_init(ViPipe); //do it day and night
		if (signal_type_old != signal_type) {
			signal_type_old = signal_type;
			CVI_VI_Trig_AHD(ViPipe, 1);
		}
	}
	return NULL;
}
#endif

void pr2020_init(VI_PIPE ViPipe)
{
	if (pr2020_sys_init(ViPipe) != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "PR2020 sys init fail\n");
		return;
	}

	delay_ms(20);

	if (pr2020_i2c_init(ViPipe) != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "PR2020 i2c init fail\n");
		return;
	}

	// check sensor chip id
	pr2020_write_register(ViPipe, 0xff, 0x00);
	if (((pr2020_read_register(ViPipe, 0xfc) << 8) | (pr2020_read_register(ViPipe, 0xfd))) != 0x2000) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read PR2020 chip id fail\n");
		return;
	}

	CVI_TRACE_SNS(CVI_DBG_INFO, "Loading Pixelplus PR2020 sensor\n");

	pr2020_write_register(ViPipe, 0xff, 0x00);//reset
	pr2020_write_register(ViPipe, 0x11, 0x00);

	pr2020_fw_init(ViPipe);

	signal_type = g_pastPr2020[ViPipe]->u8ImgMode;
	if (signal_type == PR2020_MODE_720P_25) {
		pr2020_set_720p_25(ViPipe);
	} else if (signal_type == PR2020_MODE_720P_30) {
		pr2020_set_720p_30(ViPipe);
	} else if (signal_type == PR2020_MODE_1080P_25) {
		pr2020_set_1080p_25(ViPipe);
	} else if (signal_type == PR2020_MODE_1080P_30) {
		pr2020_set_1080p_30(ViPipe);
	}
	CVI_TRACE_SNS(CVI_DBG_INFO, "signal_type=%d\n", signal_type);
	// wait for signal to stabilize
	delay_ms(800);
	pr2020_write_register(ViPipe, 0xff, 0x00);//page0
#if PR2020_AUTO_DETECT
	CVI_U8 *arg = malloc(sizeof(*arg));

	*arg = ViPipe;
	if (pthread_create(&g_pr2020_thid, NULL, pr2020_device_auto_detect, arg) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "PR2020 auto detect function fail!\n");
	}
#endif
}

void pr2020_exit(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "Exit Pixelplus PR2020 Sensor\n");

	if (g_pr2020_thid)
		pthread_kill(g_pr2020_thid, SIGQUIT);

	pr2020_i2c_exit(ViPipe);
}
