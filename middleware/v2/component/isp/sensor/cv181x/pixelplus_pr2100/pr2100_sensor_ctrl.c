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
#include <linux/spi/spidev.h>
#include "cvi_sns_ctrl.h"
#include "pr2100_cmos_ex.h"
#include <pthread.h>
#include <signal.h>

static void pr2100_set_1080p(VI_PIPE ViPipe);
static void pr2100_set_1080p_2ch(VI_PIPE ViPipe);
static void pr2100_set_1080p_4ch(VI_PIPE ViPipe);

const CVI_U8 pr2100_master_i2c_addr = 0x5C;        /* I2C slave address of PR2100 master chip*/
const CVI_U8 pr2100_slave_i2c_addr = 0x5F;         /* I2C slave address of PR2100 slave chip*/
const CVI_U32 pr2100_addr_byte = 1;
const CVI_U32 pr2100_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};
static VI_PIPE slave_pipe = (VI_MAX_PIPE_NUM - 1);

#define PR2100_TEST_PATTERN 0
#define PR2100_SLAVE_TEST_PATTERN 0

#if (PR2100_TEST_PATTERN)
//1920x1080 offset  0x0   0x1FA400  0x278D00
// 0 : White        eb       80       80
// 1 : Yellow       d2       10       92
// 2 : Cyan         aa       a6       10
// 3 : Green        91       36       22
// 4 : Magenta      6a       ca       de
// 5 : Red          51       5a       f0
// 6 : Blue
// 7 : Black
// 8 : Color Bar
// 9 : Ramp
// 10 : Inverse Color Bar
// 11 : Combination Pattern
#define output_pattern_ch0 (0x8 | 0x80)	// color bar
#define output_pattern_ch1 (0x8 | 0xA0)	// reverse color bar
#define output_pattern_ch2 (0x8 | 0x80)	// color bar
#define output_pattern_ch3 (0x8 | 0xA0)	// reverse color bar
#else
#define output_pattern_ch0 (0x00)
#define output_pattern_ch1 (0x00)
#define output_pattern_ch2 (0x00)
#define output_pattern_ch3 (0x00)
#endif

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

static int PR2100_GPIO_Export(unsigned int gpio)
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

static int PR2100_GPIO_SetDirection(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/direction", gpio);
	if (access(buf, 0) == -1)
		PR2100_GPIO_Export(gpio);

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

static int PR2100_GPIO_SetValue(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);
	if (access(buf, 0) == -1)
		PR2100_GPIO_Export(gpio);

	PR2100_GPIO_SetDirection(gpio, 1); //output

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

int pr2100_sys_init(VI_PIPE ViPipe)
{
	(void) ViPipe;

	//CAM_PEN
	if (PR2100_GPIO_SetValue(CVI_GPIOA_06, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set power down gpio error!\n");
		return CVI_FAILURE;
	}
#if 0
	//SENSOR0_RSTN
	if (PR2100_GPIO_SetValue(CVI_GPIOD_07, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set reset gpio error!\n");
		return CVI_FAILURE;
	}

	//BACK_DET
	if (PR2100_GPIO_SetValue(CVI_GPIOD_01, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set back detect gpio error!\n");
		return CVI_FAILURE;
	}
#endif
	return CVI_SUCCESS;
}

int pr2100_i2c_init(VI_PIPE ViPipe, CVI_U8 i2c_addr)
{
	int ret;
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;

	u8DevNum = g_aunPr2100_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
	CVI_TRACE_SNS(CVI_DBG_INFO, "open %s\n", acDevFile);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int pr2100_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int pr2100_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return 0;

	if (pr2100_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, pr2100_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, pr2100_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (pr2100_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	CVI_TRACE_SNS(CVI_DBG_INFO, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int pr2100_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (pr2100_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (pr2100_data_byte == 2)
		buf[idx++] = (data >> 8) & 0xff;

	// add data byte 0
	buf[idx++] = data & 0xff;

	ret = write(g_fd[ViPipe], buf, pr2100_addr_byte + pr2100_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	CVI_TRACE_SNS(CVI_DBG_INFO, "i2c w 0x%x 0x%x\n", addr, data);

#if 0 // read back checing
	ret = pr2100_read_register(ViPipe, addr);
	if (ret != data)
		CVI_TRACE_SNS(CVI_DBG_INFO, "i2c readback-check fail, 0x%x != 0x%x\n", ret, data);
#endif
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void pr2100_init(VI_PIPE ViPipe)
{
	CVI_U8 u8ImgMode;

	u8ImgMode = g_pastPr2100[ViPipe]->u8ImgMode;
	if (ViPipe) // only init ch0
		return;

	if (pr2100_sys_init(ViPipe) != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "PR2100 sys init fail\n");
		return;
	}

	delay_ms(20);

	if (pr2100_i2c_init(ViPipe, pr2100_master_i2c_addr) != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "PR2100 master i2c init fail\n");
		return;
	}

	// check sensor chip id
	pr2100_write_register(ViPipe, 0xff, 0x00);
	if (((pr2100_read_register(ViPipe, 0xfc) << 8) |
	     (pr2100_read_register(ViPipe, 0xfd))) != 0x2100) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "PR2100 master chip id check fail\n");
		return;
	}

	CVI_TRACE_SNS(CVI_DBG_INFO, "Loading Pixelplus PR2100 sensor\n");

	switch (u8ImgMode) {
	case PR2100_MODE_1080P:
		pr2100_set_1080p(ViPipe);
		break;
	case PR2100_MODE_1080P_2CH:
		pr2100_set_1080p_2ch(ViPipe);
		break;
	case PR2100_MODE_1080P_4CH:
		g_aunPr2100_BusInfo[slave_pipe].s8I2cDev = g_aunPr2100_BusInfo[ViPipe].s8I2cDev;
		if (pr2100_i2c_init(slave_pipe, pr2100_slave_i2c_addr) != CVI_SUCCESS) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "PR2100 slave i2c init fail\n");
			break;
		}
		pr2100_write_register(slave_pipe, 0xff, 0x00);
		if (((pr2100_read_register(slave_pipe, 0xfc) << 8) |
		     (pr2100_read_register(slave_pipe, 0xfd))) != 0x2100) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "PR2100 slave chip id check fail\n");
			break;
		}
		pr2100_set_1080p_4ch(ViPipe);
		break;
	default:
		break;
	}

	// wait for signal to stabilize
	delay_ms(800);
	pr2100_write_register(ViPipe, 0xff, 0x00);//page0
}

void pr2100_exit(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "Exit Pixelplus PR2100 Sensor\n");

	pr2100_i2c_exit(ViPipe);

	if (g_pastPr2100[ViPipe]->u8ImgMode == PR2100_MODE_1080P_4CH)
		pr2100_i2c_exit(slave_pipe);
}

static void pr2100_set_1080p(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "1CH ViPipe=%d\n", ViPipe);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0xc0, 0x21);
	pr2100_write_register(ViPipe, 0xc1, 0x21);
	pr2100_write_register(ViPipe, 0xc2, 0x21);
	pr2100_write_register(ViPipe, 0xc3, 0x21);
	pr2100_write_register(ViPipe, 0xc4, 0x21);
	pr2100_write_register(ViPipe, 0xc5, 0x21);
	pr2100_write_register(ViPipe, 0xc6, 0x21);
	pr2100_write_register(ViPipe, 0xc7, 0x21);
	pr2100_write_register(ViPipe, 0xc8, 0x21);
	pr2100_write_register(ViPipe, 0xc9, 0x01);
	pr2100_write_register(ViPipe, 0xca, 0x01);
	pr2100_write_register(ViPipe, 0xcb, 0x01);
	pr2100_write_register(ViPipe, 0xd0, 0x06);
	pr2100_write_register(ViPipe, 0xd1, 0x23);
	pr2100_write_register(ViPipe, 0xd2, 0x21);
	pr2100_write_register(ViPipe, 0xd3, 0x44);
	pr2100_write_register(ViPipe, 0xd4, 0x06);
	pr2100_write_register(ViPipe, 0xd5, 0x23);
	pr2100_write_register(ViPipe, 0xd6, 0x21);
	pr2100_write_register(ViPipe, 0xd7, 0x44);
	pr2100_write_register(ViPipe, 0xd8, 0x06);
	pr2100_write_register(ViPipe, 0xd9, 0x22);
	pr2100_write_register(ViPipe, 0xda, 0x2c);
	pr2100_write_register(ViPipe, 0x11, 0x0f);
	pr2100_write_register(ViPipe, 0x12, 0x00);
	pr2100_write_register(ViPipe, 0x13, 0x00);
	pr2100_write_register(ViPipe, 0x14, 0x21);
	pr2100_write_register(ViPipe, 0x15, 0x44);
	pr2100_write_register(ViPipe, 0x16, 0x0d);
	pr2100_write_register(ViPipe, 0x31, 0x0f);
	pr2100_write_register(ViPipe, 0x32, 0x00);
	pr2100_write_register(ViPipe, 0x33, 0x00);
	pr2100_write_register(ViPipe, 0x34, 0x21);
	pr2100_write_register(ViPipe, 0x35, 0x44);
	pr2100_write_register(ViPipe, 0x36, 0x0d);
	pr2100_write_register(ViPipe, 0xf3, 0x06);
	pr2100_write_register(ViPipe, 0xf4, 0x66);
	pr2100_write_register(ViPipe, 0xff, 0x01);//Page1
	pr2100_write_register(ViPipe, 0x00, 0xe4);
	pr2100_write_register(ViPipe, 0x01, 0x61);
	pr2100_write_register(ViPipe, 0x02, 0x00);
	pr2100_write_register(ViPipe, 0x03, 0x56);
	pr2100_write_register(ViPipe, 0x04, 0x0c);
	pr2100_write_register(ViPipe, 0x05, 0x88);
	pr2100_write_register(ViPipe, 0x06, 0x04);
	pr2100_write_register(ViPipe, 0x07, 0xb2);
	pr2100_write_register(ViPipe, 0x08, 0x44);
	pr2100_write_register(ViPipe, 0x09, 0x34);
	pr2100_write_register(ViPipe, 0x0a, 0x02);
	pr2100_write_register(ViPipe, 0x0b, 0x14);
	pr2100_write_register(ViPipe, 0x0c, 0x04);
	pr2100_write_register(ViPipe, 0x0d, 0x08);
	pr2100_write_register(ViPipe, 0x0e, 0x5e);
	pr2100_write_register(ViPipe, 0x0f, 0x5e);
	pr2100_write_register(ViPipe, 0x2c, 0x00);
	pr2100_write_register(ViPipe, 0x2d, 0x00);
	pr2100_write_register(ViPipe, 0x2e, 0x00);
	pr2100_write_register(ViPipe, 0x2f, 0x00);
	pr2100_write_register(ViPipe, 0x30, 0x00);
	pr2100_write_register(ViPipe, 0x31, 0x00);
	pr2100_write_register(ViPipe, 0x32, 0xc0);
	pr2100_write_register(ViPipe, 0x3b, 0x02);
	pr2100_write_register(ViPipe, 0x3c, 0x01);
	pr2100_write_register(ViPipe, 0x4f, 0x00);
	pr2100_write_register(ViPipe, 0x51, 0x28);
	pr2100_write_register(ViPipe, 0x52, 0x40);
	pr2100_write_register(ViPipe, 0x53, 0x0c);
	pr2100_write_register(ViPipe, 0x54, 0x0f);
	pr2100_write_register(ViPipe, 0x55, 0x8d);
	pr2100_write_register(ViPipe, 0x70, 0x06);
	pr2100_write_register(ViPipe, 0x71, 0x08);
	pr2100_write_register(ViPipe, 0x72, 0x0a);
	pr2100_write_register(ViPipe, 0x73, 0x0c);
	pr2100_write_register(ViPipe, 0x74, 0x0e);
	pr2100_write_register(ViPipe, 0x75, 0x10);
	pr2100_write_register(ViPipe, 0x76, 0x12);
	pr2100_write_register(ViPipe, 0x77, 0x14);
	pr2100_write_register(ViPipe, 0x78, 0x06);
	pr2100_write_register(ViPipe, 0x79, 0x08);
	pr2100_write_register(ViPipe, 0x7a, 0x0a);
	pr2100_write_register(ViPipe, 0x7b, 0x0c);
	pr2100_write_register(ViPipe, 0x7c, 0x0e);
	pr2100_write_register(ViPipe, 0x7d, 0x10);
	pr2100_write_register(ViPipe, 0x7e, 0x12);
	pr2100_write_register(ViPipe, 0x7f, 0x14);
	pr2100_write_register(ViPipe, 0xff, 0x02);//Page2
	pr2100_write_register(ViPipe, 0x00, 0xe4);
	pr2100_write_register(ViPipe, 0x01, 0x61);
	pr2100_write_register(ViPipe, 0x02, 0x00);
	pr2100_write_register(ViPipe, 0x03, 0x56);
	pr2100_write_register(ViPipe, 0x04, 0x0c);
	pr2100_write_register(ViPipe, 0x05, 0x88);
	pr2100_write_register(ViPipe, 0x06, 0x04);
	pr2100_write_register(ViPipe, 0x07, 0xb2);
	pr2100_write_register(ViPipe, 0x08, 0x44);
	pr2100_write_register(ViPipe, 0x09, 0x34);
	pr2100_write_register(ViPipe, 0x0a, 0x02);
	pr2100_write_register(ViPipe, 0x0b, 0x14);
	pr2100_write_register(ViPipe, 0x0c, 0x04);
	pr2100_write_register(ViPipe, 0x0d, 0x08);
	pr2100_write_register(ViPipe, 0x0e, 0x5e);
	pr2100_write_register(ViPipe, 0x0f, 0x5e);
	pr2100_write_register(ViPipe, 0x2c, 0x00);
	pr2100_write_register(ViPipe, 0x2d, 0x00);
	pr2100_write_register(ViPipe, 0x2e, 0x00);
	pr2100_write_register(ViPipe, 0x2f, 0x00);
	pr2100_write_register(ViPipe, 0x30, 0x00);
	pr2100_write_register(ViPipe, 0x31, 0x00);
	pr2100_write_register(ViPipe, 0x32, 0xc0);
	pr2100_write_register(ViPipe, 0x3b, 0x02);
	pr2100_write_register(ViPipe, 0x3c, 0x01);
	pr2100_write_register(ViPipe, 0x4f, 0x00);
	pr2100_write_register(ViPipe, 0x51, 0x28);
	pr2100_write_register(ViPipe, 0x52, 0x40);
	pr2100_write_register(ViPipe, 0x53, 0x0c);
	pr2100_write_register(ViPipe, 0x54, 0x0f);
	pr2100_write_register(ViPipe, 0x55, 0x8d);
	pr2100_write_register(ViPipe, 0x70, 0x06);
	pr2100_write_register(ViPipe, 0x71, 0x08);
	pr2100_write_register(ViPipe, 0x72, 0x0a);
	pr2100_write_register(ViPipe, 0x73, 0x0c);
	pr2100_write_register(ViPipe, 0x74, 0x0e);
	pr2100_write_register(ViPipe, 0x75, 0x10);
	pr2100_write_register(ViPipe, 0x76, 0x12);
	pr2100_write_register(ViPipe, 0x77, 0x14);
	pr2100_write_register(ViPipe, 0x78, 0x06);
	pr2100_write_register(ViPipe, 0x79, 0x08);
	pr2100_write_register(ViPipe, 0x7a, 0x0a);
	pr2100_write_register(ViPipe, 0x7b, 0x0c);
	pr2100_write_register(ViPipe, 0x7c, 0x0e);
	pr2100_write_register(ViPipe, 0x7d, 0x10);
	pr2100_write_register(ViPipe, 0x7e, 0x12);
	pr2100_write_register(ViPipe, 0x7f, 0x14);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0x14, 0x21);
	pr2100_write_register(ViPipe, 0xff, 0x06);//Page6
	pr2100_write_register(ViPipe, 0x04, 0x50);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0xeb, 0x01);
	pr2100_write_register(ViPipe, 0xf0, 0x03);
	pr2100_write_register(ViPipe, 0xf1, 0xff);
	pr2100_write_register(ViPipe, 0xea, 0x00);
	// pr2100_write_register(ViPipe, 0xe3, 0x04);//Cb-Y-Cr-Y
	pr2100_write_register(ViPipe, 0xe3, 0xc4);//Y-Cb-Y-Cr
	pr2100_write_register(ViPipe, 0xff, 0x01);//Page1
	pr2100_write_register(ViPipe, 0x50, 0x21);
	pr2100_write_register(ViPipe, 0x4f, 0x00);
	pr2100_write_register(ViPipe, 0x50, 0x21);
	pr2100_write_register(ViPipe, 0x4f, 0x20);
	pr2100_write_register(ViPipe, 0xff, 0x02);//Page2
	pr2100_write_register(ViPipe, 0x50, 0x21);
	pr2100_write_register(ViPipe, 0x4f, 0x00);
	pr2100_write_register(ViPipe, 0x50, 0x21);
	pr2100_write_register(ViPipe, 0x4f, 0x20);
	pr2100_write_register(ViPipe, 0xff, 0x01);//Page1
	pr2100_write_register(ViPipe, 0xd1, 0x10);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0xe8, 0x00);
	pr2100_write_register(ViPipe, 0xe9, 0x10);
	pr2100_write_register(ViPipe, 0xff, 0x01);//Page1
	pr2100_write_register(ViPipe, 0xcd, 0x08);
	pr2100_write_register(ViPipe, 0x4f, 0x2c);
	pr2100_write_register(ViPipe, 0xff, 0x02);//Page2
	pr2100_write_register(ViPipe, 0xd1, 0x10);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0xe9, 0x00);
	pr2100_write_register(ViPipe, 0xe9, 0x00);
	pr2100_write_register(ViPipe, 0xff, 0x02);//Page2
	pr2100_write_register(ViPipe, 0xcd, 0x08);
	pr2100_write_register(ViPipe, 0x4f, 0x2c);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0xe4, 0x20);
	pr2100_write_register(ViPipe, 0xe5, 0x64);
	pr2100_write_register(ViPipe, 0xe6, 0x20);
	pr2100_write_register(ViPipe, 0xe7, 0x64);
	pr2100_write_register(ViPipe, 0xe2, 0x00);
	pr2100_write_register(ViPipe, 0xff, 0x06);//Page6
	pr2100_write_register(ViPipe, 0x04, 0x10);
	pr2100_write_register(ViPipe, 0x05, 0x04);
	pr2100_write_register(ViPipe, 0x06, 0x00);
	pr2100_write_register(ViPipe, 0x07, 0x00);
	pr2100_write_register(ViPipe, 0x08, 0xc9);
	pr2100_write_register(ViPipe, 0x36, 0x0f);
	pr2100_write_register(ViPipe, 0x37, 0x00);
	pr2100_write_register(ViPipe, 0x38, 0x0f);
	pr2100_write_register(ViPipe, 0x39, 0x00);
	pr2100_write_register(ViPipe, 0x3a, 0x0f);
	pr2100_write_register(ViPipe, 0x3b, 0x00);
	pr2100_write_register(ViPipe, 0x3c, 0x0f);
	pr2100_write_register(ViPipe, 0x3d, 0x00);
	pr2100_write_register(ViPipe, 0x46, 0x1e);
	pr2100_write_register(ViPipe, 0x47, 0x5e);
	pr2100_write_register(ViPipe, 0x48, 0x9e);
	pr2100_write_register(ViPipe, 0x49, 0xde);
	pr2100_write_register(ViPipe, 0x1c, 0x09);
	pr2100_write_register(ViPipe, 0x1d, 0x08);
	pr2100_write_register(ViPipe, 0x1e, 0x09);
	pr2100_write_register(ViPipe, 0x1f, 0x11);
	pr2100_write_register(ViPipe, 0x20, 0x0c);
	pr2100_write_register(ViPipe, 0x21, 0x28);
	pr2100_write_register(ViPipe, 0x22, 0x0b);
	pr2100_write_register(ViPipe, 0x23, 0x01);
	pr2100_write_register(ViPipe, 0x24, 0x12);
	pr2100_write_register(ViPipe, 0x25, 0x82);
	pr2100_write_register(ViPipe, 0x26, 0x11);
	pr2100_write_register(ViPipe, 0x27, 0x11);
	pr2100_write_register(ViPipe, 0x04, 0x50);
	pr2100_write_register(ViPipe, 0xff, 0x05);//Page5
	pr2100_write_register(ViPipe, 0x09, 0x00);
	pr2100_write_register(ViPipe, 0x0a, 0x03);
	pr2100_write_register(ViPipe, 0x0e, 0x80);
	pr2100_write_register(ViPipe, 0x0f, 0x10);
	pr2100_write_register(ViPipe, 0x11, 0x80);
	pr2100_write_register(ViPipe, 0x12, 0x6e);
	pr2100_write_register(ViPipe, 0x13, 0x00);
	pr2100_write_register(ViPipe, 0x14, 0x6e);
	pr2100_write_register(ViPipe, 0x15, 0x00);
	pr2100_write_register(ViPipe, 0x16, 0x00);
	pr2100_write_register(ViPipe, 0x17, 0x00);
	pr2100_write_register(ViPipe, 0x18, 0x00);
	pr2100_write_register(ViPipe, 0x19, 0x00);
	pr2100_write_register(ViPipe, 0x1a, 0x00);
	pr2100_write_register(ViPipe, 0x1b, 0x00);
	pr2100_write_register(ViPipe, 0x1c, 0x00);
	pr2100_write_register(ViPipe, 0x1d, 0x00);
	pr2100_write_register(ViPipe, 0x1e, 0x00);
	pr2100_write_register(ViPipe, 0x20, 0x88);
	pr2100_write_register(ViPipe, 0x21, 0x07);
	pr2100_write_register(ViPipe, 0x22, 0x80);
	pr2100_write_register(ViPipe, 0x23, 0x04);
	pr2100_write_register(ViPipe, 0x24, 0x38);
	pr2100_write_register(ViPipe, 0x25, 0x0f);
	pr2100_write_register(ViPipe, 0x26, 0x00);
	pr2100_write_register(ViPipe, 0x27, 0x0f);
	pr2100_write_register(ViPipe, 0x28, 0x00);
	pr2100_write_register(ViPipe, 0x29, 0x0b);
	pr2100_write_register(ViPipe, 0x2a, 0x40);
	pr2100_write_register(ViPipe, 0x30, 0x18);
	pr2100_write_register(ViPipe, 0x31, 0x07);
	pr2100_write_register(ViPipe, 0x32, 0x80);
	pr2100_write_register(ViPipe, 0x33, 0x04);
	pr2100_write_register(ViPipe, 0x34, 0x38);
	pr2100_write_register(ViPipe, 0x35, 0x0f);
	pr2100_write_register(ViPipe, 0x36, 0x00);
	pr2100_write_register(ViPipe, 0x37, 0x0f);
	pr2100_write_register(ViPipe, 0x38, 0x00);
	pr2100_write_register(ViPipe, 0x39, 0x07);
	pr2100_write_register(ViPipe, 0x3a, 0x80);
	pr2100_write_register(ViPipe, 0x40, 0x28);
	pr2100_write_register(ViPipe, 0x41, 0x07);
	pr2100_write_register(ViPipe, 0x42, 0x80);
	pr2100_write_register(ViPipe, 0x43, 0x04);
	pr2100_write_register(ViPipe, 0x44, 0x38);
	pr2100_write_register(ViPipe, 0x45, 0x0f);
	pr2100_write_register(ViPipe, 0x46, 0x00);
	pr2100_write_register(ViPipe, 0x47, 0x0f);
	pr2100_write_register(ViPipe, 0x48, 0x00);
	pr2100_write_register(ViPipe, 0x49, 0x03);
	pr2100_write_register(ViPipe, 0x4a, 0xc0);
	pr2100_write_register(ViPipe, 0x50, 0x38);
	pr2100_write_register(ViPipe, 0x51, 0x07);
	pr2100_write_register(ViPipe, 0x52, 0x80);
	pr2100_write_register(ViPipe, 0x53, 0x04);
	pr2100_write_register(ViPipe, 0x54, 0x38);
	pr2100_write_register(ViPipe, 0x55, 0x0f);
	pr2100_write_register(ViPipe, 0x56, 0x00);
	pr2100_write_register(ViPipe, 0x57, 0x0f);
	pr2100_write_register(ViPipe, 0x58, 0x00);
	pr2100_write_register(ViPipe, 0x59, 0x00);
	pr2100_write_register(ViPipe, 0x5a, 0x00);
	pr2100_write_register(ViPipe, 0x60, 0x05);
	pr2100_write_register(ViPipe, 0x61, 0x28);
	pr2100_write_register(ViPipe, 0x62, 0x05);
	pr2100_write_register(ViPipe, 0x63, 0x28);
	pr2100_write_register(ViPipe, 0x64, 0x05);
	pr2100_write_register(ViPipe, 0x65, 0x28);
	pr2100_write_register(ViPipe, 0x66, 0x05);
	pr2100_write_register(ViPipe, 0x67, 0x28);
	pr2100_write_register(ViPipe, 0x68, 0xff);
	pr2100_write_register(ViPipe, 0x69, 0xff);
	pr2100_write_register(ViPipe, 0x6a, 0xff);
	pr2100_write_register(ViPipe, 0x6b, 0xff);
	pr2100_write_register(ViPipe, 0x6c, 0xff);
	pr2100_write_register(ViPipe, 0x6d, 0xff);
	pr2100_write_register(ViPipe, 0x6e, 0xff);
	pr2100_write_register(ViPipe, 0x6f, 0xff);
	pr2100_write_register(ViPipe, 0x10, 0xb3);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0x80, 0x80);
	pr2100_write_register(ViPipe, 0x81, 0x0e);
	pr2100_write_register(ViPipe, 0x82, 0x0d);
	pr2100_write_register(ViPipe, 0x84, 0xf0);
	pr2100_write_register(ViPipe, 0x8a, 0x00);
	pr2100_write_register(ViPipe, 0x90, 0x00);
	pr2100_write_register(ViPipe, 0x91, 0x00);
	pr2100_write_register(ViPipe, 0x94, 0xff);
	pr2100_write_register(ViPipe, 0x95, 0xff);
	pr2100_write_register(ViPipe, 0xa0, 0x33);
	pr2100_write_register(ViPipe, 0xb0, 0x33);
	pr2100_write_register(ViPipe, 0xff, 0x01);//Page1
	pr2100_write_register(ViPipe, 0x80, 0x00);
	pr2100_write_register(ViPipe, 0x81, 0x09);
	pr2100_write_register(ViPipe, 0x82, 0x00);
	pr2100_write_register(ViPipe, 0x83, 0x07);
	pr2100_write_register(ViPipe, 0x84, 0x00);
	pr2100_write_register(ViPipe, 0x85, 0x17);
	pr2100_write_register(ViPipe, 0x86, 0x03);
	pr2100_write_register(ViPipe, 0x87, 0xe5);
	pr2100_write_register(ViPipe, 0x88, 0x05);
	pr2100_write_register(ViPipe, 0x89, 0x24);
	pr2100_write_register(ViPipe, 0x8a, 0x05);
	pr2100_write_register(ViPipe, 0x8b, 0x24);
	pr2100_write_register(ViPipe, 0x8c, 0x08);
	pr2100_write_register(ViPipe, 0x8d, 0xe8);
	pr2100_write_register(ViPipe, 0x8e, 0x05);
	pr2100_write_register(ViPipe, 0x8f, 0x47);
	pr2100_write_register(ViPipe, 0x90, 0x02);
	pr2100_write_register(ViPipe, 0x91, 0xb4);
	pr2100_write_register(ViPipe, 0x92, 0x73);
	pr2100_write_register(ViPipe, 0x93, 0xe8);
	pr2100_write_register(ViPipe, 0x94, 0x0f);
	pr2100_write_register(ViPipe, 0x95, 0x5e);
	pr2100_write_register(ViPipe, 0x96, 0x03);
	pr2100_write_register(ViPipe, 0x97, 0xd0);
	pr2100_write_register(ViPipe, 0x98, 0x17);
	pr2100_write_register(ViPipe, 0x99, 0x34);
	pr2100_write_register(ViPipe, 0x9a, 0x13);
	pr2100_write_register(ViPipe, 0x9b, 0x56);
	pr2100_write_register(ViPipe, 0x9c, 0x0b);
	pr2100_write_register(ViPipe, 0x9d, 0x9a);
	pr2100_write_register(ViPipe, 0x9e, 0x09);
	pr2100_write_register(ViPipe, 0x9f, 0xab);
	pr2100_write_register(ViPipe, 0xa0, 0x01);
	pr2100_write_register(ViPipe, 0xa1, 0x74);
	pr2100_write_register(ViPipe, 0xa2, 0x01);
	pr2100_write_register(ViPipe, 0xa3, 0x6b);
	pr2100_write_register(ViPipe, 0xa4, 0x00);
	pr2100_write_register(ViPipe, 0xa5, 0xba);
	pr2100_write_register(ViPipe, 0xa6, 0x00);
	pr2100_write_register(ViPipe, 0xa7, 0xa3);
	pr2100_write_register(ViPipe, 0xa8, 0x01);
	pr2100_write_register(ViPipe, 0xa9, 0x39);
	pr2100_write_register(ViPipe, 0xaa, 0x01);
	pr2100_write_register(ViPipe, 0xab, 0x39);
	pr2100_write_register(ViPipe, 0xac, 0x00);
	pr2100_write_register(ViPipe, 0xad, 0xc1);
	pr2100_write_register(ViPipe, 0xae, 0x00);
	pr2100_write_register(ViPipe, 0xaf, 0xc1);
	pr2100_write_register(ViPipe, 0xb0, 0x05);
	pr2100_write_register(ViPipe, 0xb1, 0xcc);
	pr2100_write_register(ViPipe, 0xb2, 0x09);
	pr2100_write_register(ViPipe, 0xb3, 0x6d);
	pr2100_write_register(ViPipe, 0xb4, 0x00);
	pr2100_write_register(ViPipe, 0xb5, 0x17);
	pr2100_write_register(ViPipe, 0xb6, 0x08);
	pr2100_write_register(ViPipe, 0xb7, 0xe8);
	pr2100_write_register(ViPipe, 0xb8, 0xb0);
	pr2100_write_register(ViPipe, 0xb9, 0xce);
	pr2100_write_register(ViPipe, 0xba, 0x90);
	pr2100_write_register(ViPipe, 0xbb, 0x00);
	pr2100_write_register(ViPipe, 0xbc, 0x00);
	pr2100_write_register(ViPipe, 0xbd, 0x04);
	pr2100_write_register(ViPipe, 0xbe, 0x07);
	pr2100_write_register(ViPipe, 0xbf, 0x80);
	pr2100_write_register(ViPipe, 0xc0, 0x00);
	pr2100_write_register(ViPipe, 0xc1, 0x00);
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	pr2100_write_register(ViPipe, 0xc4, output_pattern_ch0);
	pr2100_write_register(ViPipe, 0xc9, 0x00);
	pr2100_write_register(ViPipe, 0xca, 0x02);
	pr2100_write_register(ViPipe, 0xcb, 0x07);
	pr2100_write_register(ViPipe, 0xcc, 0x80);
	pr2100_write_register(ViPipe, 0xce, 0x20);
	pr2100_write_register(ViPipe, 0xcf, 0x04);
	pr2100_write_register(ViPipe, 0xd0, 0x38);
	pr2100_write_register(ViPipe, 0xd1, 0x00);
	pr2100_write_register(ViPipe, 0xd2, 0x00);
	pr2100_write_register(ViPipe, 0xd3, 0x00);
	pr2100_write_register(ViPipe, 0xff, 0x00);//Page0
	pr2100_write_register(ViPipe, 0x10, 0x83);
	pr2100_write_register(ViPipe, 0x12, 0x00);
	pr2100_write_register(ViPipe, 0xe0, 0x05);
	pr2100_write_register(ViPipe, 0xff, 0x01);//Page1
	pr2100_write_register(ViPipe, 0x10, 0x26);
	pr2100_write_register(ViPipe, 0x11, 0x00);
	pr2100_write_register(ViPipe, 0x12, 0x87);
	pr2100_write_register(ViPipe, 0x13, 0x24);
	pr2100_write_register(ViPipe, 0x14, 0x80);
	pr2100_write_register(ViPipe, 0x15, 0x2a);
	pr2100_write_register(ViPipe, 0x16, 0x38);
	pr2100_write_register(ViPipe, 0x17, 0x00);
	pr2100_write_register(ViPipe, 0x18, 0x80);
	pr2100_write_register(ViPipe, 0x19, 0x48);
	pr2100_write_register(ViPipe, 0x1a, 0x6c);
	pr2100_write_register(ViPipe, 0x1b, 0x05);
	pr2100_write_register(ViPipe, 0x1c, 0x61);
	pr2100_write_register(ViPipe, 0x1d, 0x07);
	pr2100_write_register(ViPipe, 0x1e, 0x7e);
	pr2100_write_register(ViPipe, 0x1f, 0x80);
	pr2100_write_register(ViPipe, 0x20, 0x80);
	pr2100_write_register(ViPipe, 0x21, 0x80);
	pr2100_write_register(ViPipe, 0x22, 0x90);
	pr2100_write_register(ViPipe, 0x23, 0x80);
	pr2100_write_register(ViPipe, 0x24, 0x80);
	pr2100_write_register(ViPipe, 0x25, 0x80);
	pr2100_write_register(ViPipe, 0x26, 0x84);
	pr2100_write_register(ViPipe, 0x27, 0x82);
	pr2100_write_register(ViPipe, 0x28, 0x00);
	pr2100_write_register(ViPipe, 0x29, 0xff);
	pr2100_write_register(ViPipe, 0x2a, 0xff);
	pr2100_write_register(ViPipe, 0x2b, 0x00);
	pr2100_write_register(ViPipe, 0x2c, 0x00);
	pr2100_write_register(ViPipe, 0x2e, 0x00);
	pr2100_write_register(ViPipe, 0x33, 0x14);
	pr2100_write_register(ViPipe, 0x34, 0x14);
	pr2100_write_register(ViPipe, 0x35, 0x80);
	pr2100_write_register(ViPipe, 0x36, 0x80);
	pr2100_write_register(ViPipe, 0x37, 0xad);
	pr2100_write_register(ViPipe, 0x38, 0x4b);
	pr2100_write_register(ViPipe, 0x39, 0x08);
	pr2100_write_register(ViPipe, 0x3a, 0x21);
	pr2100_write_register(ViPipe, 0x3b, 0x02);
	pr2100_write_register(ViPipe, 0x3d, 0x23);
	pr2100_write_register(ViPipe, 0x3e, 0x05);
	pr2100_write_register(ViPipe, 0x3f, 0xc8);
	pr2100_write_register(ViPipe, 0x40, 0x05);
	pr2100_write_register(ViPipe, 0x41, 0x55);
	pr2100_write_register(ViPipe, 0x42, 0x01);
	pr2100_write_register(ViPipe, 0x43, 0x38);
	pr2100_write_register(ViPipe, 0x44, 0x6a);
	pr2100_write_register(ViPipe, 0x45, 0x00);
	pr2100_write_register(ViPipe, 0x46, 0x14);
	pr2100_write_register(ViPipe, 0x47, 0xb0);
	pr2100_write_register(ViPipe, 0x48, 0xdf);
	pr2100_write_register(ViPipe, 0x49, 0x00);
	pr2100_write_register(ViPipe, 0x4a, 0x7b);
	pr2100_write_register(ViPipe, 0x4b, 0x60);
	pr2100_write_register(ViPipe, 0x4c, 0x00);
	pr2100_write_register(ViPipe, 0x4d, 0x26);
	pr2100_write_register(ViPipe, 0x4e, 0x00);
	pr2100_write_register(ViPipe, 0x50, 0x21);
	pr2100_write_register(ViPipe, 0x54, 0x0e);
	pr2100_write_register(ViPipe, 0x54, 0x0f);
}

static void pr2100_set_1080p_2ch(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "2CH ViPipe=%d\n", ViPipe);

	pr2100_write_register(ViPipe, 0xFF, 0x00);
	pr2100_write_register(ViPipe, 0x10, 0x83);	//MAN_IFMT0
	pr2100_write_register(ViPipe, 0x11, 0x0f);	//MAN_EQ_DC_GN0
	pr2100_write_register(ViPipe, 0x12, 0x00);	//MAN_EQ_AC_GN0
	pr2100_write_register(ViPipe, 0x13, 0x00);	//VADC_EQ_BAND0
	pr2100_write_register(ViPipe, 0x14, 0x21);	//VADC_CTRL0_0
	pr2100_write_register(ViPipe, 0x15, 0x44);	//VADC_CTRL1_0
	pr2100_write_register(ViPipe, 0x16, 0x0d);	//VADC_CTRL2_0
	pr2100_write_register(ViPipe, 0x30, 0x83);	//MAN_IFMT1
	pr2100_write_register(ViPipe, 0x31, 0x0f);	//MAN_EQ_DC_GN1
	pr2100_write_register(ViPipe, 0x32, 0x00);	//MAN_EQ_AC_GN1
	pr2100_write_register(ViPipe, 0x33, 0x00);	//VADC_EQ_BAND1
	pr2100_write_register(ViPipe, 0x34, 0x21);	//VADC_CTRL0_1
	pr2100_write_register(ViPipe, 0x35, 0x44);	//VADC_CTRL1_1
	pr2100_write_register(ViPipe, 0x36, 0x0d);	//VADC_CTRL2_1
	pr2100_write_register(ViPipe, 0x80, 0x80);	//IRQ_CTRL
	pr2100_write_register(ViPipe, 0x81, 0x0e);	//IRQ_SYNC_PERIOD
	pr2100_write_register(ViPipe, 0x82, 0x0d);	//WAKE0_PERIOD
	pr2100_write_register(ViPipe, 0x84, 0xf0);	//IRQ_NOVID_MD
	pr2100_write_register(ViPipe, 0x8a, 0x00);	//WAKE1_PERIOD
	pr2100_write_register(ViPipe, 0x90, 0x00);	//IRQENA_WAKE
	pr2100_write_register(ViPipe, 0x91, 0x00);	//IRQENA_GPIO
	pr2100_write_register(ViPipe, 0x94, 0xff);	//IRQCLR_WAKE
	pr2100_write_register(ViPipe, 0x95, 0xff);	//IRQCLR_GPIO
	pr2100_write_register(ViPipe, 0xa0, 0x33);	//IRQENA_VFD0
	pr2100_write_register(ViPipe, 0xb0, 0x33);	//IRQENA_VFD1
	pr2100_write_register(ViPipe, 0xc0, 0x21);	//MPP_CTRL0
	pr2100_write_register(ViPipe, 0xc1, 0x21);	//MPP_CTRL1
	pr2100_write_register(ViPipe, 0xc2, 0x21);	//MPP_CTRL2
	pr2100_write_register(ViPipe, 0xc3, 0x21);	//MPP_CTRL3
	pr2100_write_register(ViPipe, 0xc4, 0x21);	//MPP_CTRL4
	pr2100_write_register(ViPipe, 0xc5, 0x21);	//MPP_CTRL5
	pr2100_write_register(ViPipe, 0xc6, 0x21);	//MPP_CTRL6
	pr2100_write_register(ViPipe, 0xc7, 0x21);	//MPP_CTRL7
	pr2100_write_register(ViPipe, 0xc8, 0x21);	//MPP_CTRL8
	pr2100_write_register(ViPipe, 0xc9, 0x01);	//MPP_CTRL9
	pr2100_write_register(ViPipe, 0xca, 0x01);	//MPP_CTRLA
	pr2100_write_register(ViPipe, 0xcb, 0x01);	//MPP_CTRLB
	pr2100_write_register(ViPipe, 0xd0, 0x06);	//PLL0_CON0
	pr2100_write_register(ViPipe, 0xd1, 0x23);	//PLL0_CON1
	pr2100_write_register(ViPipe, 0xd2, 0x21);	//PLL0_CON2
	pr2100_write_register(ViPipe, 0xd3, 0x44);	//PLL0_CON3
	pr2100_write_register(ViPipe, 0xd4, 0x06);	//PLL1_CON0
	pr2100_write_register(ViPipe, 0xd5, 0x23);	//PLL1_CON1
	pr2100_write_register(ViPipe, 0xd6, 0x21);	//PLL1_CON2
	pr2100_write_register(ViPipe, 0xd7, 0x44);	//PLL1_CON3
	pr2100_write_register(ViPipe, 0xd8, 0x06);	//PLL2_CON0
	pr2100_write_register(ViPipe, 0xd9, 0x22);	//PLL2_CON1
	pr2100_write_register(ViPipe, 0xda, 0x2c);	//PLL2_CON2
	pr2100_write_register(ViPipe, 0xe0, 0x05);	//LATCH_EN_CON0
	pr2100_write_register(ViPipe, 0xe1, 0x05);	//LATCH_EN_CON1
	pr2100_write_register(ViPipe, 0xe2, 0x00);	//OUT_FMT
	pr2100_write_register(ViPipe, 0xe3, 0xc4);	//CHID_NUM, Y-Cb-Y-Cr
	pr2100_write_register(ViPipe, 0xe4, 0x20);	//CH_SEL0
	pr2100_write_register(ViPipe, 0xe5, 0x64);	//CH_SEL1
	pr2100_write_register(ViPipe, 0xe6, 0x20);	//CH_SEL2
	pr2100_write_register(ViPipe, 0xe7, 0x64);	//CH_SEL3
	pr2100_write_register(ViPipe, 0xe8, 0x00);	//VDCKP_PHASE
	pr2100_write_register(ViPipe, 0xe9, 0x00);	//VDCKN_PHASE
	pr2100_write_register(ViPipe, 0xea, 0x00);	//CLK_PWDN
	pr2100_write_register(ViPipe, 0xeb, 0x01);	//MIPI_DATA_EN
	pr2100_write_register(ViPipe, 0xf0, 0x03);	//PAR_OE_M
	pr2100_write_register(ViPipe, 0xf1, 0xff);	//PAR_OE_L
	pr2100_write_register(ViPipe, 0xf3, 0x06);	//PAD_MPP_CTL
	pr2100_write_register(ViPipe, 0xf4, 0x66);	//PAD_VD_CTL

	pr2100_write_register(ViPipe, 0xFF, 0x01);
	pr2100_write_register(ViPipe, 0x00, 0xe4);	//VID_CON0
	pr2100_write_register(ViPipe, 0x01, 0x61);	//GAINFIT0
	pr2100_write_register(ViPipe, 0x02, 0x00);	//PEDFIT0
	pr2100_write_register(ViPipe, 0x03, 0x56);	//CLAMP_CON0
	pr2100_write_register(ViPipe, 0x04, 0x0c);	//HPLL_CON0_0
	pr2100_write_register(ViPipe, 0x05, 0x88);	//HPLL_CON1_0
	pr2100_write_register(ViPipe, 0x06, 0x04);	//HPLL_CON2_0
	pr2100_write_register(ViPipe, 0x07, 0xb2);	//HPLL_CON3_0
	pr2100_write_register(ViPipe, 0x08, 0x44);	//HPLL_CON4_0
	pr2100_write_register(ViPipe, 0x09, 0x34);	//HPLL_CON5_0
	pr2100_write_register(ViPipe, 0x0a, 0x02);	//HPLL_CON6_0
	pr2100_write_register(ViPipe, 0x0b, 0x14);	//STD_POS0_0
	pr2100_write_register(ViPipe, 0x0c, 0x04);	//STD_POS1_0
	pr2100_write_register(ViPipe, 0x0d, 0x08);	//STD_POS2_0
	pr2100_write_register(ViPipe, 0x0e, 0x5e);	//STD_SLICE0_0
	pr2100_write_register(ViPipe, 0x0f, 0x5e);	//STD_SLICE1_0
	pr2100_write_register(ViPipe, 0x10, 0x26);	//STD_SLICE2_0
	pr2100_write_register(ViPipe, 0x11, 0x00);	//HDELAY_MSB0
	pr2100_write_register(ViPipe, 0x12, 0x87);	//HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0x13, 0x24);	//HDELAY_LSB0
	pr2100_write_register(ViPipe, 0x14, 0x80);	//HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x15, 0x2a);	//VDELAY0
	pr2100_write_register(ViPipe, 0x16, 0x38);	//VACTIVE0
	pr2100_write_register(ViPipe, 0x17, 0x00);	//CBG_DELAY_MSB0
	pr2100_write_register(ViPipe, 0x18, 0x80);	//CBG_ACTIVE_MSB0
	pr2100_write_register(ViPipe, 0x19, 0x48);	//CBG_DELAY_LSB0
	pr2100_write_register(ViPipe, 0x1a, 0x6c);	//CBG_ACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x1b, 0x05);	//VSMASK_INV_STRT0
	pr2100_write_register(ViPipe, 0x1c, 0x61);	//VSMASK_INV_END0
	pr2100_write_register(ViPipe, 0x1d, 0x07);	//CBP_DELAY_MSB0
	pr2100_write_register(ViPipe, 0x1e, 0x7e);	//CBP_DELAY_LSB0
	pr2100_write_register(ViPipe, 0x1f, 0x80);	//HSCL_ACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x20, 0x80);	//CONT0
	pr2100_write_register(ViPipe, 0x21, 0x80);	//BRGT0
	pr2100_write_register(ViPipe, 0x22, 0x90);	//SAT0
	pr2100_write_register(ViPipe, 0x23, 0x80);	//HUE0
	pr2100_write_register(ViPipe, 0x24, 0x80);	//CB_GAIN0
	pr2100_write_register(ViPipe, 0x25, 0x80);	//CR_GAIN0
	pr2100_write_register(ViPipe, 0x26, 0x84);	//CB_OFF0
	pr2100_write_register(ViPipe, 0x27, 0x82);	//CR_OFF0
	pr2100_write_register(ViPipe, 0x28, 0x00);	//Y_OUT_GAIN0
	pr2100_write_register(ViPipe, 0x29, 0xff);	//DOWN_HSCL_MSB0
	pr2100_write_register(ViPipe, 0x2a, 0xff);	//DOWN_HSCL_LSB0
	pr2100_write_register(ViPipe, 0x2b, 0x00);	//MAN_HSCL_ADD0_0
	pr2100_write_register(ViPipe, 0x2c, 0x00);	//MAN_HSCL_ADD1_0
	pr2100_write_register(ViPipe, 0x2d, 0x00);	//MAN_HSCL_ADD2_0
	pr2100_write_register(ViPipe, 0x2e, 0x00);	//MAN_OUTFMT_ADD0_0
	pr2100_write_register(ViPipe, 0x2f, 0x00);	//MAN_OUTFMT_ADD1_0
	pr2100_write_register(ViPipe, 0x30, 0x00);	//MAN_OUTFMT_ADD2_0
	pr2100_write_register(ViPipe, 0x31, 0x00);	//MAN_COMB_MD0
	pr2100_write_register(ViPipe, 0x32, 0xc0);	//COMB_CRL_MD0
	pr2100_write_register(ViPipe, 0x33, 0x14);	//COMB_CVBS_CRL0_0
	pr2100_write_register(ViPipe, 0x34, 0x14);	//COMB_CVBS_CRL1_0
	pr2100_write_register(ViPipe, 0x35, 0x80);	//COMB_CVBS_CRL2_0
	pr2100_write_register(ViPipe, 0x36, 0x80);	//COMB_CVBS_CRL3_0
	pr2100_write_register(ViPipe, 0x37, 0xad);	//HD_Y_NOTCH_MD0
	pr2100_write_register(ViPipe, 0x38, 0x4b);	//HD_COMB_FLT_MD0
	pr2100_write_register(ViPipe, 0x39, 0x08);	//Y_DYN_PEAK_GN0
	pr2100_write_register(ViPipe, 0x3a, 0x21);	//HD_Y_LPF_MD0
	pr2100_write_register(ViPipe, 0x3b, 0x02);	//Y_HPF_CORE_MIN0
	pr2100_write_register(ViPipe, 0x3c, 0x01);	//Y_HPF_CORE_STEP0
	pr2100_write_register(ViPipe, 0x3d, 0x23);	//CORE_CON0
	pr2100_write_register(ViPipe, 0x3e, 0x05);	//MAN_CLPF_MD0
	pr2100_write_register(ViPipe, 0x3f, 0xc8);	//HD_CTI_CON0
	pr2100_write_register(ViPipe, 0x40, 0x05);	//C_LOOP_CON0_0
	pr2100_write_register(ViPipe, 0x41, 0x55);	//C_LOOP_CON1_0
	pr2100_write_register(ViPipe, 0x42, 0x01);	//C_LOOP_CON2_0
	pr2100_write_register(ViPipe, 0x43, 0x38);	//C_LOOP_CON3_0
	pr2100_write_register(ViPipe, 0x44, 0x6a);	//C_HUE_CVI0_0
	pr2100_write_register(ViPipe, 0x45, 0x00);	//C_HUE_CVI1_0
	pr2100_write_register(ViPipe, 0x46, 0x14);	//C_PHASE_REF0_0
	pr2100_write_register(ViPipe, 0x47, 0xb0);	//C_PHASE_REF1_0
	pr2100_write_register(ViPipe, 0x48, 0xdf);	//C_PHASE_REF2_0
	pr2100_write_register(ViPipe, 0x49, 0x00);	//G_GAIN_REF0_0
	pr2100_write_register(ViPipe, 0x4a, 0x7b);	//G_GAIN_REF1_0
	pr2100_write_register(ViPipe, 0x4b, 0x60);	//MAN_C_GAIN_VAL0_0
	pr2100_write_register(ViPipe, 0x4c, 0x00);	//MAN_C_GAIN_VAL1_0
	pr2100_write_register(ViPipe, 0x4d, 0x26);	//YC_DELAY0
	pr2100_write_register(ViPipe, 0x4e, 0x00);	//HD_HALF_MD0
	pr2100_write_register(ViPipe, 0x4f, 0x2c);	//OUTFMT_CON0
	pr2100_write_register(ViPipe, 0x50, 0x21);	//TST_EQ_POS0_0
	pr2100_write_register(ViPipe, 0x51, 0x28);	//TST_EQ_POS1_0
	pr2100_write_register(ViPipe, 0x52, 0x40);	//TST_EQ_POS2_0
	pr2100_write_register(ViPipe, 0x53, 0x0c);	//TST_VID_DEC0
	pr2100_write_register(ViPipe, 0x54, 0x0f);	//TST_VID_RST0
	pr2100_write_register(ViPipe, 0x55, 0x8d);	//PTZ_SLICE_LVL0
	pr2100_write_register(ViPipe, 0x70, 0x06);	//COMB_VCRL_VTH1_0
	pr2100_write_register(ViPipe, 0x71, 0x08);	//COMB_VCRL_VTH2_0
	pr2100_write_register(ViPipe, 0x72, 0x0a);	//COMB_VCRL_VTH3_0
	pr2100_write_register(ViPipe, 0x73, 0x0c);	//COMB_VCRL_VTH4_0
	pr2100_write_register(ViPipe, 0x74, 0x0e);	//COMB_VCRL_VTH5_0
	pr2100_write_register(ViPipe, 0x75, 0x10);	//COMB_VCRL_VTH6_0
	pr2100_write_register(ViPipe, 0x76, 0x12);	//COMB_VCRL_VTH7_0
	pr2100_write_register(ViPipe, 0x77, 0x14);	//COMB_VCRL_VTH8_0
	pr2100_write_register(ViPipe, 0x78, 0x06);	//COMB_VMIX_VTH1_0
	pr2100_write_register(ViPipe, 0x79, 0x08);	//COMB_VMIX_VTH2_0
	pr2100_write_register(ViPipe, 0x7a, 0x0a);	//COMB_VMIX_VTH3_0
	pr2100_write_register(ViPipe, 0x7b, 0x0c);	//COMB_VMIX_VTH4_0
	pr2100_write_register(ViPipe, 0x7c, 0x0e);	//COMB_VMIX_VTH5_0
	pr2100_write_register(ViPipe, 0x7d, 0x10);	//COMB_VMIX_VTH6_0
	pr2100_write_register(ViPipe, 0x7e, 0x12);	//COMB_VMIX_VTH7_0
	pr2100_write_register(ViPipe, 0x7f, 0x14);	//COMB_VMIX_VTH8_0
	pr2100_write_register(ViPipe, 0x80, 0x00);	//STD_VPOS_PVI_960MSB0
	pr2100_write_register(ViPipe, 0x81, 0x09);	//STD_VPOS_PVI_960LSB0
	pr2100_write_register(ViPipe, 0x82, 0x00);	//STD_VPOS_CVI_960MSB0
	pr2100_write_register(ViPipe, 0x83, 0x07);	//STD_VPOS_CVI_960LSB0
	pr2100_write_register(ViPipe, 0x84, 0x00);	//STD_VPOS_AHD_960MSB0
	pr2100_write_register(ViPipe, 0x85, 0x17);	//STD_VPOS_AHD_960LSB0
	pr2100_write_register(ViPipe, 0x86, 0x03);	//STD_VPOS_TVI_960MSB0
	pr2100_write_register(ViPipe, 0x87, 0xe5);	//STD_VPOS_TVI_960LSB0
	pr2100_write_register(ViPipe, 0x88, 0x05);	//STD_HPOS_PVI_960MSB0
	pr2100_write_register(ViPipe, 0x89, 0x24);	//STD_HPOS_PVI_960LSB0
	pr2100_write_register(ViPipe, 0x8a, 0x05);	//STD_HPOS_CVI_960MSB0
	pr2100_write_register(ViPipe, 0x8b, 0x24);	//STD_HPOS_CVI_960LSB0
	pr2100_write_register(ViPipe, 0x8c, 0x08);	//STD_HPOS_AHD_960MSB0
	pr2100_write_register(ViPipe, 0x8d, 0xe8);	//STD_HPOS_AHD_960LSB0
	pr2100_write_register(ViPipe, 0x8e, 0x05);	//STD_HPOS_TVI_960MSB0
	pr2100_write_register(ViPipe, 0x8f, 0x47);	//STD_HPOS_TVI_960LSB0
	pr2100_write_register(ViPipe, 0x90, 0x02);	//VSYNC_ACCUM_960MSB0
	pr2100_write_register(ViPipe, 0x91, 0xb4);	//VSYNC_ACCUM_960LSB0
	pr2100_write_register(ViPipe, 0x92, 0x73);	//VLINE_MAX_960_MSB0
	pr2100_write_register(ViPipe, 0x93, 0xe8);	//VLINE_MAX_960_LSB0
	pr2100_write_register(ViPipe, 0x94, 0x0f);	//SLICE_VSYNC0
	pr2100_write_register(ViPipe, 0x95, 0x5e);	//STD_SLICE_AHD0
	pr2100_write_register(ViPipe, 0x96, 0x03);	//HACT_SIZE_960_MSB0
	pr2100_write_register(ViPipe, 0x97, 0xd0);	//HACT_SIZE_960_LSB0
	pr2100_write_register(ViPipe, 0x98, 0x17);	//HPERIOD_960P25MSB0
	pr2100_write_register(ViPipe, 0x99, 0x34);	//HPERIOD_960P25LSB0
	pr2100_write_register(ViPipe, 0x9a, 0x13);	//HPERIOD_960P30MSB0
	pr2100_write_register(ViPipe, 0x9b, 0x56);	//HPERIOD_960P30LSB0
	pr2100_write_register(ViPipe, 0x9c, 0x0b);	//HPERIOD_960P50MSB0
	pr2100_write_register(ViPipe, 0x9d, 0x9a);	//HPERIOD_960P50LSB0
	pr2100_write_register(ViPipe, 0x9e, 0x09);	//HPERIOD_960P60MSB0
	pr2100_write_register(ViPipe, 0x9f, 0xab);	//HPERIOD_960P60LSB0
	pr2100_write_register(ViPipe, 0xa0, 0x01);	//VALY_STRT_960P25MSB0
	pr2100_write_register(ViPipe, 0xa1, 0x74);	//VALY_STRT_960P25LSB0
	pr2100_write_register(ViPipe, 0xa2, 0x01);	//VALY_STRT_960P30MSB0
	pr2100_write_register(ViPipe, 0xa3, 0x6b);	//VALY_STRT_960P30LSB0
	pr2100_write_register(ViPipe, 0xa4, 0x00);	//VALY_STRT_960P50MSB0
	pr2100_write_register(ViPipe, 0xa5, 0xba);	//VALY_STRT_960P50LSB0
	pr2100_write_register(ViPipe, 0xa6, 0x00);	//VALY_STRT_960P60MSB0
	pr2100_write_register(ViPipe, 0xa7, 0xa3);	//VALY_STRT_960P60LSB0
	pr2100_write_register(ViPipe, 0xa8, 0x01);	//PED_STRT_960P25MSB0
	pr2100_write_register(ViPipe, 0xa9, 0x39);	//PED_STRT_960P25LSB0
	pr2100_write_register(ViPipe, 0xaa, 0x01);	//PED_STRT_960P30MSB0
	pr2100_write_register(ViPipe, 0xab, 0x39);	//PED_STRT_960P30LSB0
	pr2100_write_register(ViPipe, 0xac, 0x00);	//PED_STRT_960P50MSB0
	pr2100_write_register(ViPipe, 0xad, 0xc1);	//PED_STRT_960P50LSB0
	pr2100_write_register(ViPipe, 0xae, 0x00);	//PED_STRT_960P60MSB0
	pr2100_write_register(ViPipe, 0xaf, 0xc1);	//PED_STRT_960P60LSB0
	pr2100_write_register(ViPipe, 0xb0, 0x05);	//COMB_MEM_960PMSB0
	pr2100_write_register(ViPipe, 0xb1, 0xcc);	//COMB_MEM_960PLSB0
	pr2100_write_register(ViPipe, 0xb2, 0x09);	//C_JIT_GAIN_960PMSB0
	pr2100_write_register(ViPipe, 0xb3, 0x6d);	//C_JIT_GAIN_960PLSB0
	pr2100_write_register(ViPipe, 0xb4, 0x00);	//STD_VPOS_AHD_MSB0
	pr2100_write_register(ViPipe, 0xb5, 0x17);	//STD_VPOS_AHD_LSB0
	pr2100_write_register(ViPipe, 0xb6, 0x08);	//STD_HPOS_AHD_MSB0
	pr2100_write_register(ViPipe, 0xb7, 0xe8);	//STD_HPOS_AHD_LSB0
	pr2100_write_register(ViPipe, 0xb8, 0xb0);	//STD_TVI_OFFSET_25F0
	pr2100_write_register(ViPipe, 0xb9, 0xce);	//STD_TVI_OFFSET_30F0
	pr2100_write_register(ViPipe, 0xba, 0x90);	//STD_TVI_OFFSET_960P0
	pr2100_write_register(ViPipe, 0xbb, 0x00);	//EXT_SYNC_CON0
	pr2100_write_register(ViPipe, 0xbc, 0x00);	//VOSYNC_HDELAY_MSB0
	pr2100_write_register(ViPipe, 0xbd, 0x04);	//VOSYNC_HDELAY_LSB0
	pr2100_write_register(ViPipe, 0xbe, 0x07);	//VOSYNC_HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xbf, 0x80);	//VOSYNC_HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xc0, 0x00);	//VOSYNC_VDELAY_MSB0
	pr2100_write_register(ViPipe, 0xc1, 0x00);	//VOSYNC_VDELAY_LSB0
	#if PR2100_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	#else
	pr2100_write_register(ViPipe, 0xc2, 0x04);	//VOSYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xc3, 0x39);	//VOSYNC_VACTIVE_LSB0
	#endif
	pr2100_write_register(ViPipe, 0xc4, output_pattern_ch0);
	pr2100_write_register(ViPipe, 0xc9, 0x00);	//VISYNC_HDELAY_MSB0
	pr2100_write_register(ViPipe, 0xca, 0x02);	//VISYNC_HDELAY_LSB0
	pr2100_write_register(ViPipe, 0xcb, 0x07);	//VISYNC_HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xcc, 0x80);	//VISYNC_HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xcd, 0x08);	//VISYNC_VDELAY_MSB0
	pr2100_write_register(ViPipe, 0xce, 0x20);	//VISYNC_VDELAY_LSB0
	pr2100_write_register(ViPipe, 0xcf, 0x04);	//VISYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xd1, 0x00);	//MIPI_DATA_FLD_CON_0
	pr2100_write_register(ViPipe, 0xd2, 0x00);	//MIPI_DATA_ODD_VAL_0
	pr2100_write_register(ViPipe, 0xd3, 0x00);	//MIPI_DATA_EVEN_VAL_0

	pr2100_write_register(ViPipe, 0xFF, 0x02);
	pr2100_write_register(ViPipe, 0x00, 0xe4);	//VID_CON1
	pr2100_write_register(ViPipe, 0x01, 0x61);	//GAINFIT1
	pr2100_write_register(ViPipe, 0x02, 0x00);	//PEDFIT1
	pr2100_write_register(ViPipe, 0x03, 0x56);	//CLAMP_CON1
	pr2100_write_register(ViPipe, 0x04, 0x0c);	//HPLL_CON0_1
	pr2100_write_register(ViPipe, 0x05, 0x88);	//HPLL_CON1_1
	pr2100_write_register(ViPipe, 0x06, 0x04);	//HPLL_CON2_1
	pr2100_write_register(ViPipe, 0x07, 0xb2);	//HPLL_CON3_1
	pr2100_write_register(ViPipe, 0x08, 0x44);	//HPLL_CON4_1
	pr2100_write_register(ViPipe, 0x09, 0x34);	//HPLL_CON5_1
	pr2100_write_register(ViPipe, 0x0a, 0x02);	//HPLL_CON6_1
	pr2100_write_register(ViPipe, 0x0b, 0x14);	//STD_POS0_1
	pr2100_write_register(ViPipe, 0x0c, 0x04);	//STD_POS1_1
	pr2100_write_register(ViPipe, 0x0d, 0x08);	//STD_POS2_1
	pr2100_write_register(ViPipe, 0x0e, 0x5e);	//STD_SLICE0_1
	pr2100_write_register(ViPipe, 0x0f, 0x5e);	//STD_SLICE1_1
	pr2100_write_register(ViPipe, 0x10, 0x26);	//STD_SLICE2_1
	pr2100_write_register(ViPipe, 0x11, 0x00);	//HDELAY_MSB1
	pr2100_write_register(ViPipe, 0x12, 0x87);	//HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0x13, 0x24);	//HDELAY_LSB1
	pr2100_write_register(ViPipe, 0x14, 0x80);	//HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x15, 0x2a);	//VDELAY1
	pr2100_write_register(ViPipe, 0x16, 0x38);	//VACTIVE1
	pr2100_write_register(ViPipe, 0x17, 0x00);	//CBG_DELAY_MSB1
	pr2100_write_register(ViPipe, 0x18, 0x80);	//CBG_ACTIVE_MSB1
	pr2100_write_register(ViPipe, 0x19, 0x48);	//CBG_DELAY_LSB1
	pr2100_write_register(ViPipe, 0x1a, 0x6c);	//CBG_ACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x1b, 0x05);	//VSMASK_INV_STRT1
	pr2100_write_register(ViPipe, 0x1c, 0x61);	//VSMASK_INV_END1
	pr2100_write_register(ViPipe, 0x1d, 0x07);	//CBP_DELAY_MSB1
	pr2100_write_register(ViPipe, 0x1e, 0x7e);	//CBP_DELAY_LSB1
	pr2100_write_register(ViPipe, 0x1f, 0x80);	//HSCL_ACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x20, 0x80);	//CONT1
	pr2100_write_register(ViPipe, 0x21, 0x80);	//BRGT1
	pr2100_write_register(ViPipe, 0x22, 0x90);	//SAT1
	pr2100_write_register(ViPipe, 0x23, 0x80);	//HUE1
	pr2100_write_register(ViPipe, 0x24, 0x80);	//CB_GAIN1
	pr2100_write_register(ViPipe, 0x25, 0x80);	//CR_GAIN1
	pr2100_write_register(ViPipe, 0x26, 0x84);	//CB_OFF1
	pr2100_write_register(ViPipe, 0x27, 0x82);	//CR_OFF1
	pr2100_write_register(ViPipe, 0x28, 0x00);	//Y_OUT_GAIN1
	pr2100_write_register(ViPipe, 0x29, 0xff);	//DOWN_HSCL_MSB1
	pr2100_write_register(ViPipe, 0x2a, 0xff);	//DOWN_HSCL_LSB1
	pr2100_write_register(ViPipe, 0x2b, 0x00);	//MAN_HSCL_ADD0_1
	pr2100_write_register(ViPipe, 0x2c, 0x00);	//MAN_HSCL_ADD1_1
	pr2100_write_register(ViPipe, 0x2d, 0x00);	//MAN_HSCL_ADD2_1
	pr2100_write_register(ViPipe, 0x2e, 0x00);	//MAN_OUTFMT_ADD0_1
	pr2100_write_register(ViPipe, 0x2f, 0x00);	//MAN_OUTFMT_ADD1_1
	pr2100_write_register(ViPipe, 0x30, 0x00);	//MAN_OUTFMT_ADD2_1
	pr2100_write_register(ViPipe, 0x31, 0x00);	//MAN_COMB_MD1
	pr2100_write_register(ViPipe, 0x32, 0xc0);	//COMB_CRL_MD1
	pr2100_write_register(ViPipe, 0x33, 0x14);	//COMB_CVBS_CRL0_1
	pr2100_write_register(ViPipe, 0x34, 0x14);	//COMB_CVBS_CRL1_1
	pr2100_write_register(ViPipe, 0x35, 0x80);	//COMB_CVBS_CRL2_1
	pr2100_write_register(ViPipe, 0x36, 0x80);	//COMB_CVBS_CRL3_1
	pr2100_write_register(ViPipe, 0x37, 0xad);	//HD_Y_NOTCH_MD1
	pr2100_write_register(ViPipe, 0x38, 0x4b);	//HD_COMB_FLT_MD1
	pr2100_write_register(ViPipe, 0x39, 0x08);	//Y_DYN_PEAK_GN1
	pr2100_write_register(ViPipe, 0x3a, 0x21);	//HD_Y_LPF_MD1
	pr2100_write_register(ViPipe, 0x3b, 0x02);	//Y_HPF_CORE_MIN1
	pr2100_write_register(ViPipe, 0x3c, 0x01);	//Y_HPF_CORE_STEP1
	pr2100_write_register(ViPipe, 0x3d, 0x23);	//CORE_CON1
	pr2100_write_register(ViPipe, 0x3e, 0x05);	//MAN_CLPF_MD1
	pr2100_write_register(ViPipe, 0x3f, 0xc8);	//HD_CTI_CON1
	pr2100_write_register(ViPipe, 0x40, 0x05);	//C_LOOP_CON0_1
	pr2100_write_register(ViPipe, 0x41, 0x55);	//C_LOOP_CON1_1
	pr2100_write_register(ViPipe, 0x42, 0x01);	//C_LOOP_CON2_1
	pr2100_write_register(ViPipe, 0x43, 0x38);	//C_LOOP_CON3_1
	pr2100_write_register(ViPipe, 0x44, 0x6a);	//C_HUE_CVI0_1
	pr2100_write_register(ViPipe, 0x45, 0x00);	//C_HUE_CVI1_1
	pr2100_write_register(ViPipe, 0x46, 0x14);	//C_PHASE_REF0_1
	pr2100_write_register(ViPipe, 0x47, 0xb0);	//C_PHASE_REF1_1
	pr2100_write_register(ViPipe, 0x48, 0xdf);	//C_PHASE_REF2_1
	pr2100_write_register(ViPipe, 0x49, 0x00);	//G_GAIN_REF0_1
	pr2100_write_register(ViPipe, 0x4a, 0x7b);	//G_GAIN_REF1_1
	pr2100_write_register(ViPipe, 0x4b, 0x60);	//MAN_C_GAIN_VAL0_1
	pr2100_write_register(ViPipe, 0x4c, 0x00);	//MAN_C_GAIN_VAL1_1
	pr2100_write_register(ViPipe, 0x4d, 0x26);	//YC_DELAY1
	pr2100_write_register(ViPipe, 0x4e, 0x00);	//HD_HALF_MD1
	pr2100_write_register(ViPipe, 0x4f, 0x2c);	//OUTFMT_CON1
	pr2100_write_register(ViPipe, 0x50, 0x21);	//TST_EQ_POS0_1
	pr2100_write_register(ViPipe, 0x51, 0x28);	//TST_EQ_POS1_1
	pr2100_write_register(ViPipe, 0x52, 0x40);	//TST_EQ_POS2_1
	pr2100_write_register(ViPipe, 0x53, 0x0c);	//TST_VID_DEC1
	pr2100_write_register(ViPipe, 0x54, 0x0f);	//TST_VID_RST1
	pr2100_write_register(ViPipe, 0x55, 0x8d);	//PTZ_SLICE_LVL1
	pr2100_write_register(ViPipe, 0x70, 0x06);	//COMB_VCRL_VTH1_1
	pr2100_write_register(ViPipe, 0x71, 0x08);	//COMB_VCRL_VTH2_1
	pr2100_write_register(ViPipe, 0x72, 0x0a);	//COMB_VCRL_VTH3_1
	pr2100_write_register(ViPipe, 0x73, 0x0c);	//COMB_VCRL_VTH4_1
	pr2100_write_register(ViPipe, 0x74, 0x0e);	//COMB_VCRL_VTH5_1
	pr2100_write_register(ViPipe, 0x75, 0x10);	//COMB_VCRL_VTH6_1
	pr2100_write_register(ViPipe, 0x76, 0x12);	//COMB_VCRL_VTH7_1
	pr2100_write_register(ViPipe, 0x77, 0x14);	//COMB_VCRL_VTH8_1
	pr2100_write_register(ViPipe, 0x78, 0x06);	//COMB_VMIX_VTH1_1
	pr2100_write_register(ViPipe, 0x79, 0x08);	//COMB_VMIX_VTH2_1
	pr2100_write_register(ViPipe, 0x7a, 0x0a);	//COMB_VMIX_VTH3_1
	pr2100_write_register(ViPipe, 0x7b, 0x0c);	//COMB_VMIX_VTH4_1
	pr2100_write_register(ViPipe, 0x7c, 0x0e);	//COMB_VMIX_VTH5_1
	pr2100_write_register(ViPipe, 0x7d, 0x10);	//COMB_VMIX_VTH6_1
	pr2100_write_register(ViPipe, 0x7e, 0x12);	//COMB_VMIX_VTH7_1
	pr2100_write_register(ViPipe, 0x7f, 0x14);	//COMB_VMIX_VTH8_1
	pr2100_write_register(ViPipe, 0x80, 0x00);	//STD_VPOS_PVI_960MSB1
	pr2100_write_register(ViPipe, 0x81, 0x09);	//STD_VPOS_PVI_960LSB1
	pr2100_write_register(ViPipe, 0x82, 0x00);	//STD_VPOS_CVI_960MSB1
	pr2100_write_register(ViPipe, 0x83, 0x07);	//STD_VPOS_CVI_960LSB1
	pr2100_write_register(ViPipe, 0x84, 0x00);	//STD_VPOS_AHD_960MSB1
	pr2100_write_register(ViPipe, 0x85, 0x17);	//STD_VPOS_AHD_960LSB1
	pr2100_write_register(ViPipe, 0x86, 0x03);	//STD_VPOS_TVI_960MSB1
	pr2100_write_register(ViPipe, 0x87, 0xe5);	//STD_VPOS_TVI_960LSB1
	pr2100_write_register(ViPipe, 0x88, 0x05);	//STD_HPOS_PVI_960MSB1
	pr2100_write_register(ViPipe, 0x89, 0x24);	//STD_HPOS_PVI_960LSB1
	pr2100_write_register(ViPipe, 0x8a, 0x05);	//STD_HPOS_CVI_960MSB1
	pr2100_write_register(ViPipe, 0x8b, 0x24);	//STD_HPOS_CVI_960LSB1
	pr2100_write_register(ViPipe, 0x8c, 0x08);	//STD_HPOS_AHD_960MSB1
	pr2100_write_register(ViPipe, 0x8d, 0xe8);	//STD_HPOS_AHD_960LSB1
	pr2100_write_register(ViPipe, 0x8e, 0x05);	//STD_HPOS_TVI_960MSB1
	pr2100_write_register(ViPipe, 0x8f, 0x47);	//STD_HPOS_TVI_960LSB1
	pr2100_write_register(ViPipe, 0x90, 0x02);	//VSYNC_ACCUM_960MSB1
	pr2100_write_register(ViPipe, 0x91, 0xb4);	//VSYNC_ACCUM_960LSB1
	pr2100_write_register(ViPipe, 0x92, 0x73);	//VLINE_MAX_960_MSB1
	pr2100_write_register(ViPipe, 0x93, 0xe8);	//VLINE_MAX_960_LSB1
	pr2100_write_register(ViPipe, 0x94, 0x0f);	//SLICE_VSYNC1
	pr2100_write_register(ViPipe, 0x95, 0x5e);	//STD_SLICE_AHD1
	pr2100_write_register(ViPipe, 0x96, 0x03);	//HACT_SIZE_960_MSB1
	pr2100_write_register(ViPipe, 0x97, 0xd0);	//HACT_SIZE_960_LSB1
	pr2100_write_register(ViPipe, 0x98, 0x17);	//HPERIOD_960P25MSB1
	pr2100_write_register(ViPipe, 0x99, 0x34);	//HPERIOD_960P25LSB1
	pr2100_write_register(ViPipe, 0x9a, 0x13);	//HPERIOD_960P30MSB1
	pr2100_write_register(ViPipe, 0x9b, 0x56);	//HPERIOD_960P30LSB1
	pr2100_write_register(ViPipe, 0x9c, 0x0b);	//HPERIOD_960P50MSB1
	pr2100_write_register(ViPipe, 0x9d, 0x9a);	//HPERIOD_960P50LSB1
	pr2100_write_register(ViPipe, 0x9e, 0x09);	//HPERIOD_960P60MSB1
	pr2100_write_register(ViPipe, 0x9f, 0xab);	//HPERIOD_960P60LSB1
	pr2100_write_register(ViPipe, 0xa0, 0x01);	//VALY_STRT_960P25MSB1
	pr2100_write_register(ViPipe, 0xa1, 0x74);	//VALY_STRT_960P25LSB1
	pr2100_write_register(ViPipe, 0xa2, 0x01);	//VALY_STRT_960P30MSB1
	pr2100_write_register(ViPipe, 0xa3, 0x6b);	//VALY_STRT_960P30LSB1
	pr2100_write_register(ViPipe, 0xa4, 0x00);	//VALY_STRT_960P50MSB1
	pr2100_write_register(ViPipe, 0xa5, 0xba);	//VALY_STRT_960P50LSB1
	pr2100_write_register(ViPipe, 0xa6, 0x00);	//VALY_STRT_960P60MSB1
	pr2100_write_register(ViPipe, 0xa7, 0xa3);	//VALY_STRT_960P60LSB1
	pr2100_write_register(ViPipe, 0xa8, 0x01);	//PED_STRT_960P25MSB1
	pr2100_write_register(ViPipe, 0xa9, 0x39);	//PED_STRT_960P25LSB1
	pr2100_write_register(ViPipe, 0xaa, 0x01);	//PED_STRT_960P30MSB1
	pr2100_write_register(ViPipe, 0xab, 0x39);	//PED_STRT_960P30LSB1
	pr2100_write_register(ViPipe, 0xac, 0x00);	//PED_STRT_960P50MSB1
	pr2100_write_register(ViPipe, 0xad, 0xc1);	//PED_STRT_960P50LSB1
	pr2100_write_register(ViPipe, 0xae, 0x00);	//PED_STRT_960P60MSB1
	pr2100_write_register(ViPipe, 0xaf, 0xc1);	//PED_STRT_960P60LSB1
	pr2100_write_register(ViPipe, 0xb0, 0x05);	//COMB_MEM_960PMSB1
	pr2100_write_register(ViPipe, 0xb1, 0xcc);	//COMB_MEM_960PLSB1
	pr2100_write_register(ViPipe, 0xb2, 0x09);	//C_JIT_GAIN_960PMSB1
	pr2100_write_register(ViPipe, 0xb3, 0x6d);	//C_JIT_GAIN_960PLSB1
	pr2100_write_register(ViPipe, 0xb4, 0x00);	//STD_VPOS_AHD_MSB1
	pr2100_write_register(ViPipe, 0xb5, 0x17);	//STD_VPOS_AHD_LSB1
	pr2100_write_register(ViPipe, 0xb6, 0x08);	//STD_HPOS_AHD_MSB1
	pr2100_write_register(ViPipe, 0xb7, 0xe8);	//STD_HPOS_AHD_LSB1
	pr2100_write_register(ViPipe, 0xb8, 0xb0);	//STD_TVI_OFFSET_25F1
	pr2100_write_register(ViPipe, 0xb9, 0xce);	//STD_TVI_OFFSET_30F1
	pr2100_write_register(ViPipe, 0xba, 0x90);	//STD_TVI_OFFSET_960P1
	pr2100_write_register(ViPipe, 0xbb, 0x00);	//EXT_SYNC_CON1
	pr2100_write_register(ViPipe, 0xbc, 0x00);	//VOSYNC_HDELAY_MSB1
	pr2100_write_register(ViPipe, 0xbd, 0x04);	//VOSYNC_HDELAY_LSB1
	pr2100_write_register(ViPipe, 0xbe, 0x07);	//VOSYNC_HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xbf, 0x80);	//VOSYNC_HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xc0, 0x00);	//VOSYNC_VDELAY_MSB1
	pr2100_write_register(ViPipe, 0xc1, 0x00);	//VOSYNC_VDELAY_LSB1
	#if PR2100_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	#else
	pr2100_write_register(ViPipe, 0xc2, 0x04);	//VOSYNC_VACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xc3, 0x39);	//VOSYNC_VACTIVE_LSB1
	#endif
	pr2100_write_register(ViPipe, 0xc4, output_pattern_ch1);
	pr2100_write_register(ViPipe, 0xc9, 0x00);	//VISYNC_HDELAY_MSB1
	pr2100_write_register(ViPipe, 0xca, 0x02);	//VISYNC_HDELAY_LSB1
	pr2100_write_register(ViPipe, 0xcb, 0x07);	//VISYNC_HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xcc, 0x80);	//VISYNC_HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xcd, 0x08);	//VISYNC_VDELAY_MSB1
	pr2100_write_register(ViPipe, 0xce, 0x20);	//VISYNC_VDELAY_LSB1
	pr2100_write_register(ViPipe, 0xcf, 0x04);	//VISYNC_VACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xd1, 0x00);	//MIPI_DATA_FLD_CON_1
	pr2100_write_register(ViPipe, 0xd2, 0x00);	//MIPI_DATA_ODD_VAL_1
	pr2100_write_register(ViPipe, 0xd3, 0x00);	//MIPI_DATA_EVEN_VAL_1

	pr2100_write_register(ViPipe, 0xFF, 0x05);
	pr2100_write_register(ViPipe, 0x09, 0x00);	//REF_CH_STRT
	pr2100_write_register(ViPipe, 0x0a, 0x03);	//REF_CH_VDLY
	pr2100_write_register(ViPipe, 0x0e, 0x80);	//VBLK_CODE
	pr2100_write_register(ViPipe, 0x0f, 0x10);	//VBLK_CODE
	pr2100_write_register(ViPipe, 0x10, 0xb3);	//MTX_CTRL
	pr2100_write_register(ViPipe, 0x11, 0x90);	//MTX_LONG_DLY_H
	pr2100_write_register(ViPipe, 0x12, 0x6e);	//MTX_LONG_DLY_L
	pr2100_write_register(ViPipe, 0x13, 0x00);	//MTX_SHORT_DLY_H
	pr2100_write_register(ViPipe, 0x14, 0x6e);	//MTX_SHORT_DLY_L
	pr2100_write_register(ViPipe, 0x15, 0x00);	//REF_OFFSET
	pr2100_write_register(ViPipe, 0x16, 0x00);	//REF_FE_HDLY
	pr2100_write_register(ViPipe, 0x17, 0x00);	//REF_FE_HDLY
	pr2100_write_register(ViPipe, 0x18, 0x00);	//REF_FS_HDLY
	pr2100_write_register(ViPipe, 0x19, 0x00);	//REF_FS_HDLY
	pr2100_write_register(ViPipe, 0x1a, 0x00);	//REF_HTOTAL
	pr2100_write_register(ViPipe, 0x1b, 0x00);	//REF_HTOTAL
	pr2100_write_register(ViPipe, 0x1c, 0x00);	//REF_VTOTAL
	pr2100_write_register(ViPipe, 0x1d, 0x00);	//REF_VTOTAL
	pr2100_write_register(ViPipe, 0x1e, 0x00);	//CHID_MD
	pr2100_write_register(ViPipe, 0x20, 0x88);	//MTX_CH0_CTRL
	pr2100_write_register(ViPipe, 0x21, 0x07);	//MTX_CH0_HSIZE
	pr2100_write_register(ViPipe, 0x22, 0x80);	//MTX_CH0_HSIZE
	pr2100_write_register(ViPipe, 0x23, 0x04);	//MTX_CH0_VSIZE
	pr2100_write_register(ViPipe, 0x24, 0x38);	//MTX_CH0_VSIZE
	pr2100_write_register(ViPipe, 0x25, 0x0f);	//CH0_FS_OS_H
	pr2100_write_register(ViPipe, 0x26, 0x00);	//CH0_FS_OS_L
	pr2100_write_register(ViPipe, 0x27, 0x0f);	//CH0_FE_OS_H
	pr2100_write_register(ViPipe, 0x28, 0x00);	//CH0_FE_OS_L
	pr2100_write_register(ViPipe, 0x29, 0x0b);	//REF_CH0_VS_OS
	pr2100_write_register(ViPipe, 0x2a, 0x40);	//REF_CH0_VS_OS
	pr2100_write_register(ViPipe, 0x30, 0x98);	//MTX_CH1_CTRL
	pr2100_write_register(ViPipe, 0x31, 0x07);	//MTX_CH1_HSIZE
	pr2100_write_register(ViPipe, 0x32, 0x80);	//MTX_CH1_HSIZE
	pr2100_write_register(ViPipe, 0x33, 0x04);	//MTX_CH1_VSIZE
	pr2100_write_register(ViPipe, 0x34, 0x38);	//MTX_CH1_VSIZE
	pr2100_write_register(ViPipe, 0x35, 0x0f);	//CH1_FS_OS_H
	pr2100_write_register(ViPipe, 0x36, 0x00);	//CH1_FS_OS_L
	pr2100_write_register(ViPipe, 0x37, 0x0f);	//CH1_FE_OS_H
	pr2100_write_register(ViPipe, 0x38, 0x00);	//CH1_FE_OS_L
	pr2100_write_register(ViPipe, 0x39, 0x07);	//REF_CH1_VS_OS
	pr2100_write_register(ViPipe, 0x3a, 0x80);	//REF_CH1_VS_OS
	pr2100_write_register(ViPipe, 0x40, 0x28);	//MTX_CH2_CTRL
	pr2100_write_register(ViPipe, 0x41, 0x07);	//MTX_CH2_HSIZE
	pr2100_write_register(ViPipe, 0x42, 0x80);	//MTX_CH2_HSIZE
	pr2100_write_register(ViPipe, 0x43, 0x04);	//MTX_CH2_VSIZE
	pr2100_write_register(ViPipe, 0x44, 0x38);	//MTX_CH2_VSIZE
	pr2100_write_register(ViPipe, 0x45, 0x0f);	//CH2_FS_OS_H
	pr2100_write_register(ViPipe, 0x46, 0x00);	//CH2_FS_OS_L
	pr2100_write_register(ViPipe, 0x47, 0x0f);	//CH2_FE_OS_H
	pr2100_write_register(ViPipe, 0x48, 0x00);	//CH2_FE_OS_L
	pr2100_write_register(ViPipe, 0x49, 0x03);	//REF_CH2_VS_OS
	pr2100_write_register(ViPipe, 0x4a, 0xc0);	//REF_CH2_VS_OS
	pr2100_write_register(ViPipe, 0x50, 0x38);	//MTX_CH3_CTRL
	pr2100_write_register(ViPipe, 0x51, 0x07);	//MTX_CH3_HSIZE
	pr2100_write_register(ViPipe, 0x52, 0x80);	//MTX_CH3_HSIZE
	pr2100_write_register(ViPipe, 0x53, 0x04);	//MTX_CH3_VSIZE
	pr2100_write_register(ViPipe, 0x54, 0x38);	//MTX_CH3_VSIZE
	pr2100_write_register(ViPipe, 0x55, 0x0f);	//CH3_FS_OS_H
	pr2100_write_register(ViPipe, 0x56, 0x00);	//CH3_FS_OS_L
	pr2100_write_register(ViPipe, 0x57, 0x0f);	//CH3_FE_OS_H
	pr2100_write_register(ViPipe, 0x58, 0x00);	//CH3_FE_OS_L
	pr2100_write_register(ViPipe, 0x59, 0x00);	//REF_CH3_VS_OS
	pr2100_write_register(ViPipe, 0x5a, 0x00);	//REF_CH3_VS_OS
	pr2100_write_register(ViPipe, 0x60, 0x05);	//CH0_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x61, 0x28);	//CH0_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x62, 0x05);	//CH1_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x63, 0x28);	//CH1_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x64, 0x05);	//CH2_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x65, 0x28);	//CH2_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x66, 0x05);	//CH3_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x67, 0x28);	//CH3_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x68, 0xff);	//CH0_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x69, 0xff);	//CH0_VRATE[7:0]
	pr2100_write_register(ViPipe, 0x6a, 0xff);	//CH1_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x6b, 0xff);	//CH1_VRATE[7:0]
	pr2100_write_register(ViPipe, 0x6c, 0xff);	//CH2_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x6d, 0xff);	//CH2_VRATE[7:0]
	pr2100_write_register(ViPipe, 0x6e, 0xff);	//CH3_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x6f, 0xff);	//CH3_VRATE[7:0]

	pr2100_write_register(ViPipe, 0xFF, 0x06);
	pr2100_write_register(ViPipe, 0x04, 0x10);	//MIPI_CONTROL_0
	pr2100_write_register(ViPipe, 0x05, 0x04);	//MIPI_CONTROL_1
	pr2100_write_register(ViPipe, 0x06, 0x00);	//MIPI_CONTROL_2
	pr2100_write_register(ViPipe, 0x07, 0x00);	//MIPI_CONTROL_3
	pr2100_write_register(ViPipe, 0x08, 0xc9);	//MIPI_CONTROL_4
	pr2100_write_register(ViPipe, 0x1c, 0x09);	//MIPI_T_LPX
	pr2100_write_register(ViPipe, 0x1d, 0x08);	//MIPI_T_CLK_PREPARE
	pr2100_write_register(ViPipe, 0x1e, 0x09);	//MIPI_T_HS_PREPARE
	pr2100_write_register(ViPipe, 0x1f, 0x11);	//MIPI_T_HS_ZERO
	pr2100_write_register(ViPipe, 0x20, 0x0c);	//MIPI_T_HS_TRAIL
	pr2100_write_register(ViPipe, 0x21, 0x28);	//MIPI_T_CLK_ZERO
	pr2100_write_register(ViPipe, 0x22, 0x0b);	//MIPI_T_CLK_TRAIL
	pr2100_write_register(ViPipe, 0x23, 0x01);	//MIPI_T_CLK_PRE
	pr2100_write_register(ViPipe, 0x24, 0x12);	//MIPI_T_CLK_POST
	pr2100_write_register(ViPipe, 0x25, 0x82);	//MIPI_T_WAKEUP
	pr2100_write_register(ViPipe, 0x26, 0x11);	//MIPI_T_HSEXIT
	pr2100_write_register(ViPipe, 0x27, 0x11);	//MIPI_T_CLK_HSEXIT
	pr2100_write_register(ViPipe, 0x36, 0x0f);	//MIPI_PKT_SIZE0_H
	pr2100_write_register(ViPipe, 0x37, 0x00);	//MIPI_PKT_SIZE0_L
	pr2100_write_register(ViPipe, 0x38, 0x0f);	//MIPI_PKT_SIZE1_H
	pr2100_write_register(ViPipe, 0x39, 0x00);	//MIPI_PKT_SIZE1_L
	pr2100_write_register(ViPipe, 0x3a, 0x0f);	//MIPI_PKT_SIZE2_H
	pr2100_write_register(ViPipe, 0x3b, 0x00);	//MIPI_PKT_SIZE2_L
	pr2100_write_register(ViPipe, 0x3c, 0x0f);	//MIPI_PKT_SIZE3_H
	pr2100_write_register(ViPipe, 0x3d, 0x00);	//MIPI_PKT_SIZE3_L
	pr2100_write_register(ViPipe, 0x46, 0x1e);	//MIPI_DATA_ID0
	pr2100_write_register(ViPipe, 0x47, 0x5e);	//MIPI_DATA_ID1
	pr2100_write_register(ViPipe, 0x48, 0x9e);	//MIPI_DATA_ID2
	pr2100_write_register(ViPipe, 0x49, 0xde);	//MIPI_DATA_ID3
	pr2100_write_register(ViPipe, 0x04, 0x50);	//MIPI_CONTROL_0
}

static void pr2100_set_1080p_4ch_slave(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "4CH Slave ViPipe=%d\n", ViPipe);
	//chip id : 0x5d-(ch:0)Set vdec [Camera:3(HDA), videoResolution:11(video_1920x1080p25)]

	pr2100_write_register(ViPipe, 0xFF, 0x00);
	pr2100_write_register(ViPipe, 0x10, 0x83);	//MAN_IFMT0
	pr2100_write_register(ViPipe, 0x11, 0x0f);	//MAN_EQ_DC_GN0
	pr2100_write_register(ViPipe, 0x12, 0x00);	//MAN_EQ_AC_GN0
	pr2100_write_register(ViPipe, 0x13, 0x00);	//VADC_EQ_BAND0
	pr2100_write_register(ViPipe, 0x14, 0x21);	//VADC_CTRL0_0
	pr2100_write_register(ViPipe, 0x15, 0x44);	//VADC_CTRL1_0
	pr2100_write_register(ViPipe, 0x16, 0x0d);	//VADC_CTRL2_0
	pr2100_write_register(ViPipe, 0x30, 0x83);	//MAN_IFMT1
	pr2100_write_register(ViPipe, 0x31, 0x0f);	//MAN_EQ_DC_GN1
	pr2100_write_register(ViPipe, 0x32, 0x00);	//MAN_EQ_AC_GN1
	pr2100_write_register(ViPipe, 0x33, 0x00);	//VADC_EQ_BAND1
	pr2100_write_register(ViPipe, 0x34, 0x21);	//VADC_CTRL0_1
	pr2100_write_register(ViPipe, 0x35, 0x44);	//VADC_CTRL1_1
	pr2100_write_register(ViPipe, 0x36, 0x0d);	//VADC_CTRL2_1
	pr2100_write_register(ViPipe, 0x80, 0x80);	//IRQ_CTRL
	pr2100_write_register(ViPipe, 0x81, 0x0e);	//IRQ_SYNC_PERIOD
	pr2100_write_register(ViPipe, 0x82, 0x0d);	//WAKE0_PERIOD
	pr2100_write_register(ViPipe, 0x84, 0xf0);	//IRQ_NOVID_MD
	pr2100_write_register(ViPipe, 0x8a, 0x00);	//WAKE1_PERIOD
	pr2100_write_register(ViPipe, 0x90, 0x00);	//IRQENA_WAKE
	pr2100_write_register(ViPipe, 0x91, 0x00);	//IRQENA_GPIO
	pr2100_write_register(ViPipe, 0x94, 0xff);	//IRQCLR_WAKE
	pr2100_write_register(ViPipe, 0x95, 0xff);	//IRQCLR_GPIO
	pr2100_write_register(ViPipe, 0xa0, 0x33);	//IRQENA_VFD0
	pr2100_write_register(ViPipe, 0xb0, 0x33);	//IRQENA_VFD1
	pr2100_write_register(ViPipe, 0xc0, 0x20);	//MPP_CTRL0
	pr2100_write_register(ViPipe, 0xc1, 0x20);	//MPP_CTRL1
	pr2100_write_register(ViPipe, 0xc2, 0x20);	//MPP_CTRL2
	pr2100_write_register(ViPipe, 0xc3, 0x20);	//MPP_CTRL3
	pr2100_write_register(ViPipe, 0xc4, 0x20);	//MPP_CTRL4
	pr2100_write_register(ViPipe, 0xc5, 0x20);	//MPP_CTRL5
	pr2100_write_register(ViPipe, 0xc6, 0x20);	//MPP_CTRL6
	pr2100_write_register(ViPipe, 0xc7, 0x20);	//MPP_CTRL7
	pr2100_write_register(ViPipe, 0xc8, 0x20);	//MPP_CTRL8
	pr2100_write_register(ViPipe, 0xc9, 0x04);	//MPP_CTRL9
	pr2100_write_register(ViPipe, 0xca, 0x04);	//MPP_CTRLA
	pr2100_write_register(ViPipe, 0xcb, 0x04);	//MPP_CTRLB
	pr2100_write_register(ViPipe, 0xd0, 0x06);	//PLL0_CON0
	pr2100_write_register(ViPipe, 0xd1, 0x23);	//PLL0_CON1
	pr2100_write_register(ViPipe, 0xd2, 0x21);	//PLL0_CON2
	pr2100_write_register(ViPipe, 0xd3, 0x44);	//PLL0_CON3
	pr2100_write_register(ViPipe, 0xd4, 0x06);	//PLL1_CON0
	pr2100_write_register(ViPipe, 0xd5, 0x23);	//PLL1_CON1
	pr2100_write_register(ViPipe, 0xd6, 0x21);	//PLL1_CON2
	pr2100_write_register(ViPipe, 0xd7, 0x61);	//PLL1_CON3
	pr2100_write_register(ViPipe, 0xd8, 0x86);	//PLL2_CON0
	pr2100_write_register(ViPipe, 0xd9, 0x23);	//PLL2_CON1
	pr2100_write_register(ViPipe, 0xda, 0x21);	//PLL2_CON2
	pr2100_write_register(ViPipe, 0xe0, 0x05);	//LATCH_EN_CON0
	pr2100_write_register(ViPipe, 0xe1, 0x05);	//LATCH_EN_CON1
	pr2100_write_register(ViPipe, 0xe2, 0x14);	//OUT_FMT
	pr2100_write_register(ViPipe, 0xe3, 0xc4);	//CHID_NUM, Y-Cb-Y-Cr
	pr2100_write_register(ViPipe, 0xe4, 0x20);	//CH_SEL0
	pr2100_write_register(ViPipe, 0xe5, 0x64);	//CH_SEL1
	pr2100_write_register(ViPipe, 0xe6, 0x02);	//CH_SEL2
	pr2100_write_register(ViPipe, 0xe7, 0x64);	//CH_SEL3
	pr2100_write_register(ViPipe, 0xe8, 0x08);	//VDCKP_PHASE
	pr2100_write_register(ViPipe, 0xe9, 0x08);	//VDCKN_PHASE
	pr2100_write_register(ViPipe, 0xea, 0x10);	//CLK_PWDN
	pr2100_write_register(ViPipe, 0xeb, 0x00);	//MIPI_DATA_EN
	pr2100_write_register(ViPipe, 0xf0, 0x02);	//PAR_OE_M
	pr2100_write_register(ViPipe, 0xf1, 0x00);	//PAR_OE_L
	pr2100_write_register(ViPipe, 0xf3, 0x04);	//PAD_MPP_CTL
	pr2100_write_register(ViPipe, 0xf4, 0x44);	//PAD_VD_CTL

	pr2100_write_register(ViPipe, 0xFF, 0x01);
	pr2100_write_register(ViPipe, 0x00, 0xe4);	//VID_CON0
	pr2100_write_register(ViPipe, 0x01, 0x61);	//GAINFIT0
	pr2100_write_register(ViPipe, 0x02, 0x00);	//PEDFIT0
	pr2100_write_register(ViPipe, 0x03, 0x56);	//CLAMP_CON0
	pr2100_write_register(ViPipe, 0x04, 0x0c);	//HPLL_CON0_0
	pr2100_write_register(ViPipe, 0x05, 0x88);	//HPLL_CON1_0
	pr2100_write_register(ViPipe, 0x06, 0x04);	//HPLL_CON2_0
	pr2100_write_register(ViPipe, 0x07, 0xb2);	//HPLL_CON3_0
	pr2100_write_register(ViPipe, 0x08, 0x44);	//HPLL_CON4_0
	pr2100_write_register(ViPipe, 0x09, 0x34);	//HPLL_CON5_0
	pr2100_write_register(ViPipe, 0x0a, 0x02);	//HPLL_CON6_0
	pr2100_write_register(ViPipe, 0x0b, 0x14);	//STD_POS0_0
	pr2100_write_register(ViPipe, 0x0c, 0x04);	//STD_POS1_0
	pr2100_write_register(ViPipe, 0x0d, 0x08);	//STD_POS2_0
	pr2100_write_register(ViPipe, 0x0e, 0x5e);	//STD_SLICE0_0
	pr2100_write_register(ViPipe, 0x0f, 0x5e);	//STD_SLICE1_0
	pr2100_write_register(ViPipe, 0x10, 0x26);	//STD_SLICE2_0
	pr2100_write_register(ViPipe, 0x11, 0x00);	//HDELAY_MSB0
	pr2100_write_register(ViPipe, 0x12, 0x87);	//HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0x13, 0x24);	//HDELAY_LSB0
	pr2100_write_register(ViPipe, 0x14, 0x80);	//HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x15, 0x2a);	//VDELAY0
	pr2100_write_register(ViPipe, 0x16, 0x38);	//VACTIVE0
	pr2100_write_register(ViPipe, 0x17, 0x00);	//CBG_DELAY_MSB0
	pr2100_write_register(ViPipe, 0x18, 0x80);	//CBG_ACTIVE_MSB0
	pr2100_write_register(ViPipe, 0x19, 0x48);	//CBG_DELAY_LSB0
	pr2100_write_register(ViPipe, 0x1a, 0x6c);	//CBG_ACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x1b, 0x05);	//VSMASK_INV_STRT0
	pr2100_write_register(ViPipe, 0x1c, 0x61);	//VSMASK_INV_END0
	pr2100_write_register(ViPipe, 0x1d, 0x07);	//CBP_DELAY_MSB0
	pr2100_write_register(ViPipe, 0x1e, 0x7e);	//CBP_DELAY_LSB0
	pr2100_write_register(ViPipe, 0x1f, 0x80);	//HSCL_ACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x20, 0x80);	//CONT0
	pr2100_write_register(ViPipe, 0x21, 0x80);	//BRGT0
	pr2100_write_register(ViPipe, 0x22, 0x90);	//SAT0
	pr2100_write_register(ViPipe, 0x23, 0x80);	//HUE0
	pr2100_write_register(ViPipe, 0x24, 0x80);	//CB_GAIN0
	pr2100_write_register(ViPipe, 0x25, 0x80);	//CR_GAIN0
	pr2100_write_register(ViPipe, 0x26, 0x84);	//CB_OFF0
	pr2100_write_register(ViPipe, 0x27, 0x82);	//CR_OFF0
	pr2100_write_register(ViPipe, 0x28, 0x00);	//Y_OUT_GAIN0
	pr2100_write_register(ViPipe, 0x29, 0xff);	//DOWN_HSCL_MSB0
	pr2100_write_register(ViPipe, 0x2a, 0xff);	//DOWN_HSCL_LSB0
	pr2100_write_register(ViPipe, 0x2b, 0x00);	//MAN_HSCL_ADD0_0
	pr2100_write_register(ViPipe, 0x2c, 0x00);	//MAN_HSCL_ADD1_0
	pr2100_write_register(ViPipe, 0x2d, 0x00);	//MAN_HSCL_ADD2_0
	pr2100_write_register(ViPipe, 0x2e, 0x00);	//MAN_OUTFMT_ADD0_0
	pr2100_write_register(ViPipe, 0x2f, 0x00);	//MAN_OUTFMT_ADD1_0
	pr2100_write_register(ViPipe, 0x30, 0x00);	//MAN_OUTFMT_ADD2_0
	pr2100_write_register(ViPipe, 0x31, 0x00);	//MAN_COMB_MD0
	pr2100_write_register(ViPipe, 0x32, 0xc0);	//COMB_CRL_MD0
	pr2100_write_register(ViPipe, 0x33, 0x14);	//COMB_CVBS_CRL0_0
	pr2100_write_register(ViPipe, 0x34, 0x14);	//COMB_CVBS_CRL1_0
	pr2100_write_register(ViPipe, 0x35, 0x80);	//COMB_CVBS_CRL2_0
	pr2100_write_register(ViPipe, 0x36, 0x80);	//COMB_CVBS_CRL3_0
	pr2100_write_register(ViPipe, 0x37, 0xad);	//HD_Y_NOTCH_MD0
	pr2100_write_register(ViPipe, 0x38, 0x4b);	//HD_COMB_FLT_MD0
	pr2100_write_register(ViPipe, 0x39, 0x08);	//Y_DYN_PEAK_GN0
	pr2100_write_register(ViPipe, 0x3a, 0x21);	//HD_Y_LPF_MD0
	pr2100_write_register(ViPipe, 0x3b, 0x02);	//Y_HPF_CORE_MIN0
	pr2100_write_register(ViPipe, 0x3c, 0x01);	//Y_HPF_CORE_STEP0
	pr2100_write_register(ViPipe, 0x3d, 0x23);	//CORE_CON0
	pr2100_write_register(ViPipe, 0x3e, 0x05);	//MAN_CLPF_MD0
	pr2100_write_register(ViPipe, 0x3f, 0xc8);	//HD_CTI_CON0
	pr2100_write_register(ViPipe, 0x40, 0x05);	//C_LOOP_CON0_0
	pr2100_write_register(ViPipe, 0x41, 0x55);	//C_LOOP_CON1_0
	pr2100_write_register(ViPipe, 0x42, 0x01);	//C_LOOP_CON2_0
	pr2100_write_register(ViPipe, 0x43, 0x38);	//C_LOOP_CON3_0
	pr2100_write_register(ViPipe, 0x44, 0x6a);	//C_HUE_CVI0_0
	pr2100_write_register(ViPipe, 0x45, 0x00);	//C_HUE_CVI1_0
	pr2100_write_register(ViPipe, 0x46, 0x14);	//C_PHASE_REF0_0
	pr2100_write_register(ViPipe, 0x47, 0xb0);	//C_PHASE_REF1_0
	pr2100_write_register(ViPipe, 0x48, 0xdf);	//C_PHASE_REF2_0
	pr2100_write_register(ViPipe, 0x49, 0x00);	//G_GAIN_REF0_0
	pr2100_write_register(ViPipe, 0x4a, 0x7b);	//G_GAIN_REF1_0
	pr2100_write_register(ViPipe, 0x4b, 0x60);	//MAN_C_GAIN_VAL0_0
	pr2100_write_register(ViPipe, 0x4c, 0x00);	//MAN_C_GAIN_VAL1_0
	pr2100_write_register(ViPipe, 0x4d, 0x26);	//YC_DELAY0
	pr2100_write_register(ViPipe, 0x4e, 0x00);	//HD_HALF_MD0
	pr2100_write_register(ViPipe, 0x4f, 0x2c);	//OUTFMT_CON0
	pr2100_write_register(ViPipe, 0x50, 0x21);	//TST_EQ_POS0_0
	pr2100_write_register(ViPipe, 0x51, 0x28);	//TST_EQ_POS1_0
	pr2100_write_register(ViPipe, 0x52, 0x40);	//TST_EQ_POS2_0
	pr2100_write_register(ViPipe, 0x53, 0x0c);	//TST_VID_DEC0
	pr2100_write_register(ViPipe, 0x54, 0x0f);	//TST_VID_RST0
	pr2100_write_register(ViPipe, 0x55, 0x8d);	//PTZ_SLICE_LVL0
	pr2100_write_register(ViPipe, 0x70, 0x06);	//COMB_VCRL_VTH1_0
	pr2100_write_register(ViPipe, 0x71, 0x08);	//COMB_VCRL_VTH2_0
	pr2100_write_register(ViPipe, 0x72, 0x0a);	//COMB_VCRL_VTH3_0
	pr2100_write_register(ViPipe, 0x73, 0x0c);	//COMB_VCRL_VTH4_0
	pr2100_write_register(ViPipe, 0x74, 0x0e);	//COMB_VCRL_VTH5_0
	pr2100_write_register(ViPipe, 0x75, 0x10);	//COMB_VCRL_VTH6_0
	pr2100_write_register(ViPipe, 0x76, 0x12);	//COMB_VCRL_VTH7_0
	pr2100_write_register(ViPipe, 0x77, 0x14);	//COMB_VCRL_VTH8_0
	pr2100_write_register(ViPipe, 0x78, 0x06);	//COMB_VMIX_VTH1_0
	pr2100_write_register(ViPipe, 0x79, 0x08);	//COMB_VMIX_VTH2_0
	pr2100_write_register(ViPipe, 0x7a, 0x0a);	//COMB_VMIX_VTH3_0
	pr2100_write_register(ViPipe, 0x7b, 0x0c);	//COMB_VMIX_VTH4_0
	pr2100_write_register(ViPipe, 0x7c, 0x0e);	//COMB_VMIX_VTH5_0
	pr2100_write_register(ViPipe, 0x7d, 0x10);	//COMB_VMIX_VTH6_0
	pr2100_write_register(ViPipe, 0x7e, 0x12);	//COMB_VMIX_VTH7_0
	pr2100_write_register(ViPipe, 0x7f, 0x14);	//COMB_VMIX_VTH8_0
	pr2100_write_register(ViPipe, 0x80, 0x00);	//STD_VPOS_PVI_960MSB0
	pr2100_write_register(ViPipe, 0x81, 0x09);	//STD_VPOS_PVI_960LSB0
	pr2100_write_register(ViPipe, 0x82, 0x00);	//STD_VPOS_CVI_960MSB0
	pr2100_write_register(ViPipe, 0x83, 0x07);	//STD_VPOS_CVI_960LSB0
	pr2100_write_register(ViPipe, 0x84, 0x00);	//STD_VPOS_AHD_960MSB0
	pr2100_write_register(ViPipe, 0x85, 0x17);	//STD_VPOS_AHD_960LSB0
	pr2100_write_register(ViPipe, 0x86, 0x03);	//STD_VPOS_TVI_960MSB0
	pr2100_write_register(ViPipe, 0x87, 0xe5);	//STD_VPOS_TVI_960LSB0
	pr2100_write_register(ViPipe, 0x88, 0x05);	//STD_HPOS_PVI_960MSB0
	pr2100_write_register(ViPipe, 0x89, 0x24);	//STD_HPOS_PVI_960LSB0
	pr2100_write_register(ViPipe, 0x8a, 0x05);	//STD_HPOS_CVI_960MSB0
	pr2100_write_register(ViPipe, 0x8b, 0x24);	//STD_HPOS_CVI_960LSB0
	pr2100_write_register(ViPipe, 0x8c, 0x08);	//STD_HPOS_AHD_960MSB0
	pr2100_write_register(ViPipe, 0x8d, 0xe8);	//STD_HPOS_AHD_960LSB0
	pr2100_write_register(ViPipe, 0x8e, 0x05);	//STD_HPOS_TVI_960MSB0
	pr2100_write_register(ViPipe, 0x8f, 0x47);	//STD_HPOS_TVI_960LSB0
	pr2100_write_register(ViPipe, 0x90, 0x02);	//VSYNC_ACCUM_960MSB0
	pr2100_write_register(ViPipe, 0x91, 0xb4);	//VSYNC_ACCUM_960LSB0
	pr2100_write_register(ViPipe, 0x92, 0x73);	//VLINE_MAX_960_MSB0
	pr2100_write_register(ViPipe, 0x93, 0xe8);	//VLINE_MAX_960_LSB0
	pr2100_write_register(ViPipe, 0x94, 0x0f);	//SLICE_VSYNC0
	pr2100_write_register(ViPipe, 0x95, 0x5e);	//STD_SLICE_AHD0
	pr2100_write_register(ViPipe, 0x96, 0x03);	//HACT_SIZE_960_MSB0
	pr2100_write_register(ViPipe, 0x97, 0xd0);	//HACT_SIZE_960_LSB0
	pr2100_write_register(ViPipe, 0x98, 0x17);	//HPERIOD_960P25MSB0
	pr2100_write_register(ViPipe, 0x99, 0x34);	//HPERIOD_960P25LSB0
	pr2100_write_register(ViPipe, 0x9a, 0x13);	//HPERIOD_960P30MSB0
	pr2100_write_register(ViPipe, 0x9b, 0x56);	//HPERIOD_960P30LSB0
	pr2100_write_register(ViPipe, 0x9c, 0x0b);	//HPERIOD_960P50MSB0
	pr2100_write_register(ViPipe, 0x9d, 0x9a);	//HPERIOD_960P50LSB0
	pr2100_write_register(ViPipe, 0x9e, 0x09);	//HPERIOD_960P60MSB0
	pr2100_write_register(ViPipe, 0x9f, 0xab);	//HPERIOD_960P60LSB0
	pr2100_write_register(ViPipe, 0xa0, 0x01);	//VALY_STRT_960P25MSB0
	pr2100_write_register(ViPipe, 0xa1, 0x74);	//VALY_STRT_960P25LSB0
	pr2100_write_register(ViPipe, 0xa2, 0x01);	//VALY_STRT_960P30MSB0
	pr2100_write_register(ViPipe, 0xa3, 0x6b);	//VALY_STRT_960P30LSB0
	pr2100_write_register(ViPipe, 0xa4, 0x00);	//VALY_STRT_960P50MSB0
	pr2100_write_register(ViPipe, 0xa5, 0xba);	//VALY_STRT_960P50LSB0
	pr2100_write_register(ViPipe, 0xa6, 0x00);	//VALY_STRT_960P60MSB0
	pr2100_write_register(ViPipe, 0xa7, 0xa3);	//VALY_STRT_960P60LSB0
	pr2100_write_register(ViPipe, 0xa8, 0x01);	//PED_STRT_960P25MSB0
	pr2100_write_register(ViPipe, 0xa9, 0x39);	//PED_STRT_960P25LSB0
	pr2100_write_register(ViPipe, 0xaa, 0x01);	//PED_STRT_960P30MSB0
	pr2100_write_register(ViPipe, 0xab, 0x39);	//PED_STRT_960P30LSB0
	pr2100_write_register(ViPipe, 0xac, 0x00);	//PED_STRT_960P50MSB0
	pr2100_write_register(ViPipe, 0xad, 0xc1);	//PED_STRT_960P50LSB0
	pr2100_write_register(ViPipe, 0xae, 0x00);	//PED_STRT_960P60MSB0
	pr2100_write_register(ViPipe, 0xaf, 0xc1);	//PED_STRT_960P60LSB0
	pr2100_write_register(ViPipe, 0xb0, 0x05);	//COMB_MEM_960PMSB0
	pr2100_write_register(ViPipe, 0xb1, 0xcc);	//COMB_MEM_960PLSB0
	pr2100_write_register(ViPipe, 0xb2, 0x09);	//C_JIT_GAIN_960PMSB0
	pr2100_write_register(ViPipe, 0xb3, 0x6d);	//C_JIT_GAIN_960PLSB0
	pr2100_write_register(ViPipe, 0xb4, 0x00);	//STD_VPOS_AHD_MSB0
	pr2100_write_register(ViPipe, 0xb5, 0x17);	//STD_VPOS_AHD_LSB0
	pr2100_write_register(ViPipe, 0xb6, 0x08);	//STD_HPOS_AHD_MSB0
	pr2100_write_register(ViPipe, 0xb7, 0xe8);	//STD_HPOS_AHD_LSB0
	pr2100_write_register(ViPipe, 0xb8, 0xb0);	//STD_TVI_OFFSET_25F0
	pr2100_write_register(ViPipe, 0xb9, 0xce);	//STD_TVI_OFFSET_30F0
	pr2100_write_register(ViPipe, 0xba, 0x90);	//STD_TVI_OFFSET_960P0
	pr2100_write_register(ViPipe, 0xbb, 0x00);	//EXT_SYNC_CON0
	pr2100_write_register(ViPipe, 0xbc, 0x00);	//VOSYNC_HDELAY_MSB0
	pr2100_write_register(ViPipe, 0xbd, 0x04);	//VOSYNC_HDELAY_LSB0
	pr2100_write_register(ViPipe, 0xbe, 0x07);	//VOSYNC_HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xbf, 0x80);	//VOSYNC_HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xc0, 0x00);	//VOSYNC_VDELAY_MSB0
	pr2100_write_register(ViPipe, 0xc1, 0x20);	//VOSYNC_VDELAY_LSB0
	#if PR2100_SLAVE_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	pr2100_write_register(ViPipe, 0xc4, 0x88);
	#else
	pr2100_write_register(ViPipe, 0xc2, 0x04);	//VOSYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xc3, 0x38);	//VOSYNC_VACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xc4, 0x00);
	#endif
	pr2100_write_register(ViPipe, 0xc5, 0x00);	//MAN_INFMT_ADD_LSB0
	pr2100_write_register(ViPipe, 0xc6, 0x00);	//C_PHASE_LOCK_RNG0_0
	pr2100_write_register(ViPipe, 0xc7, 0x00);	//C_PHASE_LOCK_RNG1_0
	pr2100_write_register(ViPipe, 0xc8, 0x00);	//C_PHASE_LOCK_RNG2_0
	pr2100_write_register(ViPipe, 0xc9, 0x00);	//VISYNC_HDELAY_MSB0
	pr2100_write_register(ViPipe, 0xca, 0x04);	//VISYNC_HDELAY_LSB0
	pr2100_write_register(ViPipe, 0xcb, 0x07);	//VISYNC_HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xcc, 0x80);	//VISYNC_HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xcd, 0x08);	//VISYNC_VDELAY_MSB0
	pr2100_write_register(ViPipe, 0xce, 0x20);	//VISYNC_VDELAY_LSB0
	pr2100_write_register(ViPipe, 0xcf, 0x04);	//VISYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xd1, 0x10);	//MIPI_DATA_FLD_CON_0

	pr2100_write_register(ViPipe, 0xFF, 0x02);
	pr2100_write_register(ViPipe, 0x00, 0xe4);	//VID_CON1
	pr2100_write_register(ViPipe, 0x01, 0x61);	//GAINFIT1
	pr2100_write_register(ViPipe, 0x02, 0x00);	//PEDFIT1
	pr2100_write_register(ViPipe, 0x03, 0x56);	//CLAMP_CON1
	pr2100_write_register(ViPipe, 0x04, 0x0c);	//HPLL_CON0_1
	pr2100_write_register(ViPipe, 0x05, 0x88);	//HPLL_CON1_1
	pr2100_write_register(ViPipe, 0x06, 0x04);	//HPLL_CON2_1
	pr2100_write_register(ViPipe, 0x07, 0xb2);	//HPLL_CON3_1
	pr2100_write_register(ViPipe, 0x08, 0x44);	//HPLL_CON4_1
	pr2100_write_register(ViPipe, 0x09, 0x34);	//HPLL_CON5_1
	pr2100_write_register(ViPipe, 0x0a, 0x02);	//HPLL_CON6_1
	pr2100_write_register(ViPipe, 0x0b, 0x14);	//STD_POS0_1
	pr2100_write_register(ViPipe, 0x0c, 0x04);	//STD_POS1_1
	pr2100_write_register(ViPipe, 0x0d, 0x08);	//STD_POS2_1
	pr2100_write_register(ViPipe, 0x0e, 0x5e);	//STD_SLICE0_1
	pr2100_write_register(ViPipe, 0x0f, 0x5e);	//STD_SLICE1_1
	pr2100_write_register(ViPipe, 0x10, 0x26);	//STD_SLICE2_1
	pr2100_write_register(ViPipe, 0x11, 0x00);	//HDELAY_MSB1
	pr2100_write_register(ViPipe, 0x12, 0x87);	//HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0x13, 0x24);	//HDELAY_LSB1
	pr2100_write_register(ViPipe, 0x14, 0x80);	//HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x15, 0x2a);	//VDELAY1
	pr2100_write_register(ViPipe, 0x16, 0x38);	//VACTIVE1
	pr2100_write_register(ViPipe, 0x17, 0x00);	//CBG_DELAY_MSB1
	pr2100_write_register(ViPipe, 0x18, 0x80);	//CBG_ACTIVE_MSB1
	pr2100_write_register(ViPipe, 0x19, 0x48);	//CBG_DELAY_LSB1
	pr2100_write_register(ViPipe, 0x1a, 0x6c);	//CBG_ACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x1b, 0x05);	//VSMASK_INV_STRT1
	pr2100_write_register(ViPipe, 0x1c, 0x61);	//VSMASK_INV_END1
	pr2100_write_register(ViPipe, 0x1d, 0x07);	//CBP_DELAY_MSB1
	pr2100_write_register(ViPipe, 0x1e, 0x7e);	//CBP_DELAY_LSB1
	pr2100_write_register(ViPipe, 0x1f, 0x80);	//HSCL_ACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x20, 0x80);	//CONT1
	pr2100_write_register(ViPipe, 0x21, 0x80);	//BRGT1
	pr2100_write_register(ViPipe, 0x22, 0x90);	//SAT1
	pr2100_write_register(ViPipe, 0x23, 0x80);	//HUE1
	pr2100_write_register(ViPipe, 0x24, 0x80);	//CB_GAIN1
	pr2100_write_register(ViPipe, 0x25, 0x80);	//CR_GAIN1
	pr2100_write_register(ViPipe, 0x26, 0x84);	//CB_OFF1
	pr2100_write_register(ViPipe, 0x27, 0x82);	//CR_OFF1
	pr2100_write_register(ViPipe, 0x28, 0x00);	//Y_OUT_GAIN1
	pr2100_write_register(ViPipe, 0x29, 0xff);	//DOWN_HSCL_MSB1
	pr2100_write_register(ViPipe, 0x2a, 0xff);	//DOWN_HSCL_LSB1
	pr2100_write_register(ViPipe, 0x2b, 0x00);	//MAN_HSCL_ADD0_1
	pr2100_write_register(ViPipe, 0x2c, 0x00);	//MAN_HSCL_ADD1_1
	pr2100_write_register(ViPipe, 0x2d, 0x00);	//MAN_HSCL_ADD2_1
	pr2100_write_register(ViPipe, 0x2e, 0x00);	//MAN_OUTFMT_ADD0_1
	pr2100_write_register(ViPipe, 0x2f, 0x00);	//MAN_OUTFMT_ADD1_1
	pr2100_write_register(ViPipe, 0x30, 0x00);	//MAN_OUTFMT_ADD2_1
	pr2100_write_register(ViPipe, 0x31, 0x00);	//MAN_COMB_MD1
	pr2100_write_register(ViPipe, 0x32, 0xc0);	//COMB_CRL_MD1
	pr2100_write_register(ViPipe, 0x33, 0x14);	//COMB_CVBS_CRL0_1
	pr2100_write_register(ViPipe, 0x34, 0x14);	//COMB_CVBS_CRL1_1
	pr2100_write_register(ViPipe, 0x35, 0x80);	//COMB_CVBS_CRL2_1
	pr2100_write_register(ViPipe, 0x36, 0x80);	//COMB_CVBS_CRL3_1
	pr2100_write_register(ViPipe, 0x37, 0xad);	//HD_Y_NOTCH_MD1
	pr2100_write_register(ViPipe, 0x38, 0x4b);	//HD_COMB_FLT_MD1
	pr2100_write_register(ViPipe, 0x39, 0x08);	//Y_DYN_PEAK_GN1
	pr2100_write_register(ViPipe, 0x3a, 0x21);	//HD_Y_LPF_MD1
	pr2100_write_register(ViPipe, 0x3b, 0x02);	//Y_HPF_CORE_MIN1
	pr2100_write_register(ViPipe, 0x3c, 0x01);	//Y_HPF_CORE_STEP1
	pr2100_write_register(ViPipe, 0x3d, 0x23);	//CORE_CON1
	pr2100_write_register(ViPipe, 0x3e, 0x05);	//MAN_CLPF_MD1
	pr2100_write_register(ViPipe, 0x3f, 0xc8);	//HD_CTI_CON1
	pr2100_write_register(ViPipe, 0x40, 0x05);	//C_LOOP_CON0_1
	pr2100_write_register(ViPipe, 0x41, 0x55);	//C_LOOP_CON1_1
	pr2100_write_register(ViPipe, 0x42, 0x01);	//C_LOOP_CON2_1
	pr2100_write_register(ViPipe, 0x43, 0x38);	//C_LOOP_CON3_1
	pr2100_write_register(ViPipe, 0x44, 0x6a);	//C_HUE_CVI0_1
	pr2100_write_register(ViPipe, 0x45, 0x00);	//C_HUE_CVI1_1
	pr2100_write_register(ViPipe, 0x46, 0x14);	//C_PHASE_REF0_1
	pr2100_write_register(ViPipe, 0x47, 0xb0);	//C_PHASE_REF1_1
	pr2100_write_register(ViPipe, 0x48, 0xdf);	//C_PHASE_REF2_1
	pr2100_write_register(ViPipe, 0x49, 0x00);	//G_GAIN_REF0_1
	pr2100_write_register(ViPipe, 0x4a, 0x7b);	//G_GAIN_REF1_1
	pr2100_write_register(ViPipe, 0x4b, 0x60);	//MAN_C_GAIN_VAL0_1
	pr2100_write_register(ViPipe, 0x4c, 0x00);	//MAN_C_GAIN_VAL1_1
	pr2100_write_register(ViPipe, 0x4d, 0x26);	//YC_DELAY1
	pr2100_write_register(ViPipe, 0x4e, 0x00);	//HD_HALF_MD1
	pr2100_write_register(ViPipe, 0x4f, 0x2c);	//OUTFMT_CON1
	pr2100_write_register(ViPipe, 0x50, 0x21);	//TST_EQ_POS0_1
	pr2100_write_register(ViPipe, 0x51, 0x28);	//TST_EQ_POS1_1
	pr2100_write_register(ViPipe, 0x52, 0x40);	//TST_EQ_POS2_1
	pr2100_write_register(ViPipe, 0x53, 0x0c);	//TST_VID_DEC1
	pr2100_write_register(ViPipe, 0x54, 0x0f);	//TST_VID_RST1
	pr2100_write_register(ViPipe, 0x55, 0x8d);	//PTZ_SLICE_LVL1
	pr2100_write_register(ViPipe, 0x70, 0x06);	//COMB_VCRL_VTH1_1
	pr2100_write_register(ViPipe, 0x71, 0x08);	//COMB_VCRL_VTH2_1
	pr2100_write_register(ViPipe, 0x72, 0x0a);	//COMB_VCRL_VTH3_1
	pr2100_write_register(ViPipe, 0x73, 0x0c);	//COMB_VCRL_VTH4_1
	pr2100_write_register(ViPipe, 0x74, 0x0e);	//COMB_VCRL_VTH5_1
	pr2100_write_register(ViPipe, 0x75, 0x10);	//COMB_VCRL_VTH6_1
	pr2100_write_register(ViPipe, 0x76, 0x12);	//COMB_VCRL_VTH7_1
	pr2100_write_register(ViPipe, 0x77, 0x14);	//COMB_VCRL_VTH8_1
	pr2100_write_register(ViPipe, 0x78, 0x06);	//COMB_VMIX_VTH1_1
	pr2100_write_register(ViPipe, 0x79, 0x08);	//COMB_VMIX_VTH2_1
	pr2100_write_register(ViPipe, 0x7a, 0x0a);	//COMB_VMIX_VTH3_1
	pr2100_write_register(ViPipe, 0x7b, 0x0c);	//COMB_VMIX_VTH4_1
	pr2100_write_register(ViPipe, 0x7c, 0x0e);	//COMB_VMIX_VTH5_1
	pr2100_write_register(ViPipe, 0x7d, 0x10);	//COMB_VMIX_VTH6_1
	pr2100_write_register(ViPipe, 0x7e, 0x12);	//COMB_VMIX_VTH7_1
	pr2100_write_register(ViPipe, 0x7f, 0x14);	//COMB_VMIX_VTH8_1
	pr2100_write_register(ViPipe, 0x80, 0x00);	//STD_VPOS_PVI_960MSB1
	pr2100_write_register(ViPipe, 0x81, 0x09);	//STD_VPOS_PVI_960LSB1
	pr2100_write_register(ViPipe, 0x82, 0x00);	//STD_VPOS_CVI_960MSB1
	pr2100_write_register(ViPipe, 0x83, 0x07);	//STD_VPOS_CVI_960LSB1
	pr2100_write_register(ViPipe, 0x84, 0x00);	//STD_VPOS_AHD_960MSB1
	pr2100_write_register(ViPipe, 0x85, 0x17);	//STD_VPOS_AHD_960LSB1
	pr2100_write_register(ViPipe, 0x86, 0x03);	//STD_VPOS_TVI_960MSB1
	pr2100_write_register(ViPipe, 0x87, 0xe5);	//STD_VPOS_TVI_960LSB1
	pr2100_write_register(ViPipe, 0x88, 0x05);	//STD_HPOS_PVI_960MSB1
	pr2100_write_register(ViPipe, 0x89, 0x24);	//STD_HPOS_PVI_960LSB1
	pr2100_write_register(ViPipe, 0x8a, 0x05);	//STD_HPOS_CVI_960MSB1
	pr2100_write_register(ViPipe, 0x8b, 0x24);	//STD_HPOS_CVI_960LSB1
	pr2100_write_register(ViPipe, 0x8c, 0x08);	//STD_HPOS_AHD_960MSB1
	pr2100_write_register(ViPipe, 0x8d, 0xe8);	//STD_HPOS_AHD_960LSB1
	pr2100_write_register(ViPipe, 0x8e, 0x05);	//STD_HPOS_TVI_960MSB1
	pr2100_write_register(ViPipe, 0x8f, 0x47);	//STD_HPOS_TVI_960LSB1
	pr2100_write_register(ViPipe, 0x90, 0x02);	//VSYNC_ACCUM_960MSB1
	pr2100_write_register(ViPipe, 0x91, 0xb4);	//VSYNC_ACCUM_960LSB1
	pr2100_write_register(ViPipe, 0x92, 0x73);	//VLINE_MAX_960_MSB1
	pr2100_write_register(ViPipe, 0x93, 0xe8);	//VLINE_MAX_960_LSB1
	pr2100_write_register(ViPipe, 0x94, 0x0f);	//SLICE_VSYNC1
	pr2100_write_register(ViPipe, 0x95, 0x5e);	//STD_SLICE_AHD1
	pr2100_write_register(ViPipe, 0x96, 0x03);	//HACT_SIZE_960_MSB1
	pr2100_write_register(ViPipe, 0x97, 0xd0);	//HACT_SIZE_960_LSB1
	pr2100_write_register(ViPipe, 0x98, 0x17);	//HPERIOD_960P25MSB1
	pr2100_write_register(ViPipe, 0x99, 0x34);	//HPERIOD_960P25LSB1
	pr2100_write_register(ViPipe, 0x9a, 0x13);	//HPERIOD_960P30MSB1
	pr2100_write_register(ViPipe, 0x9b, 0x56);	//HPERIOD_960P30LSB1
	pr2100_write_register(ViPipe, 0x9c, 0x0b);	//HPERIOD_960P50MSB1
	pr2100_write_register(ViPipe, 0x9d, 0x9a);	//HPERIOD_960P50LSB1
	pr2100_write_register(ViPipe, 0x9e, 0x09);	//HPERIOD_960P60MSB1
	pr2100_write_register(ViPipe, 0x9f, 0xab);	//HPERIOD_960P60LSB1
	pr2100_write_register(ViPipe, 0xa0, 0x01);	//VALY_STRT_960P25MSB1
	pr2100_write_register(ViPipe, 0xa1, 0x74);	//VALY_STRT_960P25LSB1
	pr2100_write_register(ViPipe, 0xa2, 0x01);	//VALY_STRT_960P30MSB1
	pr2100_write_register(ViPipe, 0xa3, 0x6b);	//VALY_STRT_960P30LSB1
	pr2100_write_register(ViPipe, 0xa4, 0x00);	//VALY_STRT_960P50MSB1
	pr2100_write_register(ViPipe, 0xa5, 0xba);	//VALY_STRT_960P50LSB1
	pr2100_write_register(ViPipe, 0xa6, 0x00);	//VALY_STRT_960P60MSB1
	pr2100_write_register(ViPipe, 0xa7, 0xa3);	//VALY_STRT_960P60LSB1
	pr2100_write_register(ViPipe, 0xa8, 0x01);	//PED_STRT_960P25MSB1
	pr2100_write_register(ViPipe, 0xa9, 0x39);	//PED_STRT_960P25LSB1
	pr2100_write_register(ViPipe, 0xaa, 0x01);	//PED_STRT_960P30MSB1
	pr2100_write_register(ViPipe, 0xab, 0x39);	//PED_STRT_960P30LSB1
	pr2100_write_register(ViPipe, 0xac, 0x00);	//PED_STRT_960P50MSB1
	pr2100_write_register(ViPipe, 0xad, 0xc1);	//PED_STRT_960P50LSB1
	pr2100_write_register(ViPipe, 0xae, 0x00);	//PED_STRT_960P60MSB1
	pr2100_write_register(ViPipe, 0xaf, 0xc1);	//PED_STRT_960P60LSB1
	pr2100_write_register(ViPipe, 0xb0, 0x05);	//COMB_MEM_960PMSB1
	pr2100_write_register(ViPipe, 0xb1, 0xcc);	//COMB_MEM_960PLSB1
	pr2100_write_register(ViPipe, 0xb2, 0x09);	//C_JIT_GAIN_960PMSB1
	pr2100_write_register(ViPipe, 0xb3, 0x6d);	//C_JIT_GAIN_960PLSB1
	pr2100_write_register(ViPipe, 0xb4, 0x00);	//STD_VPOS_AHD_MSB1
	pr2100_write_register(ViPipe, 0xb5, 0x17);	//STD_VPOS_AHD_LSB1
	pr2100_write_register(ViPipe, 0xb6, 0x08);	//STD_HPOS_AHD_MSB1
	pr2100_write_register(ViPipe, 0xb7, 0xe8);	//STD_HPOS_AHD_LSB1
	pr2100_write_register(ViPipe, 0xb8, 0xb0);	//STD_TVI_OFFSET_25F1
	pr2100_write_register(ViPipe, 0xb9, 0xce);	//STD_TVI_OFFSET_30F1
	pr2100_write_register(ViPipe, 0xba, 0x90);	//STD_TVI_OFFSET_960P1
	pr2100_write_register(ViPipe, 0xbb, 0x00);	//EXT_SYNC_CON1
	pr2100_write_register(ViPipe, 0xbc, 0x00);	//VOSYNC_HDELAY_MSB1
	pr2100_write_register(ViPipe, 0xbd, 0x04);	//VOSYNC_HDELAY_LSB1
	pr2100_write_register(ViPipe, 0xbe, 0x07);	//VOSYNC_HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xbf, 0x80);	//VOSYNC_HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xc0, 0x00);	//VOSYNC_VDELAY_MSB1
	pr2100_write_register(ViPipe, 0xc1, 0x20);	//VOSYNC_VDELAY_LSB1
	#if PR2100_SLAVE_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	pr2100_write_register(ViPipe, 0xc4, 0x88);
	#else
	pr2100_write_register(ViPipe, 0xc2, 0x04);	//VOSYNC_VACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xc3, 0x38);	//VOSYNC_VACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xc4, 0x00);
	#endif
	pr2100_write_register(ViPipe, 0xc5, 0x00);	//MAN_INFMT_ADD_LSB1
	pr2100_write_register(ViPipe, 0xc6, 0x00);	//C_PHASE_LOCK_RNG0_1
	pr2100_write_register(ViPipe, 0xc7, 0x00);	//C_PHASE_LOCK_RNG1_1
	pr2100_write_register(ViPipe, 0xc8, 0x00);	//C_PHASE_LOCK_RNG2_1
	pr2100_write_register(ViPipe, 0xc9, 0x00);	//VISYNC_HDELAY_MSB1
	pr2100_write_register(ViPipe, 0xca, 0x04);	//VISYNC_HDELAY_LSB1
	pr2100_write_register(ViPipe, 0xcb, 0x07);	//VISYNC_HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xcc, 0x80);	//VISYNC_HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xcd, 0x08);	//VISYNC_VDELAY_MSB1
	pr2100_write_register(ViPipe, 0xce, 0x20);	//VISYNC_VDELAY_LSB1
	pr2100_write_register(ViPipe, 0xcf, 0x04);	//VISYNC_VACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xd1, 0x10);	//MIPI_DATA_FLD_CON_1

	pr2100_write_register(ViPipe, 0xFF, 0x06);
	pr2100_write_register(ViPipe, 0x04, 0x30);	//MIPI_CONTROL_0
}

static void pr2100_set_1080p_4ch(VI_PIPE ViPipe)
{
	CVI_TRACE_SNS(CVI_DBG_INFO, "4CH Master ViPipe=%d\n", ViPipe);

	//chip id : 0x5c-(ch:0)Set vdec [Camera:3(HDA), videoResolution:11(video_1920x1080p25)]
	pr2100_write_register(ViPipe, 0xFF, 0x00);
	pr2100_write_register(ViPipe, 0x10, 0x83);	//MAN_IFMT0
	pr2100_write_register(ViPipe, 0x11, 0x0f);	//MAN_EQ_DC_GN0
	pr2100_write_register(ViPipe, 0x12, 0x00);	//MAN_EQ_AC_GN0
	pr2100_write_register(ViPipe, 0x13, 0x00);	//VADC_EQ_BAND0
	pr2100_write_register(ViPipe, 0x14, 0x21);	//VADC_CTRL0_0
	pr2100_write_register(ViPipe, 0x15, 0x44);	//VADC_CTRL1_0
	pr2100_write_register(ViPipe, 0x16, 0x0d);	//VADC_CTRL2_0
	pr2100_write_register(ViPipe, 0x30, 0x83);	//MAN_IFMT1
	pr2100_write_register(ViPipe, 0x31, 0x0f);	//MAN_EQ_DC_GN1
	pr2100_write_register(ViPipe, 0x32, 0x00);	//MAN_EQ_AC_GN1
	pr2100_write_register(ViPipe, 0x33, 0x00);	//VADC_EQ_BAND1
	pr2100_write_register(ViPipe, 0x34, 0x21);	//VADC_CTRL0_1
	pr2100_write_register(ViPipe, 0x35, 0x44);	//VADC_CTRL1_1
	pr2100_write_register(ViPipe, 0x36, 0x0d);	//VADC_CTRL2_1
	pr2100_write_register(ViPipe, 0x80, 0x80);	//IRQ_CTRL
	pr2100_write_register(ViPipe, 0x81, 0x0e);	//IRQ_SYNC_PERIOD
	pr2100_write_register(ViPipe, 0x82, 0x0d);	//WAKE0_PERIOD
	pr2100_write_register(ViPipe, 0x84, 0xf0);	//IRQ_NOVID_MD
	pr2100_write_register(ViPipe, 0x8a, 0x00);	//WAKE1_PERIOD
	pr2100_write_register(ViPipe, 0x90, 0x00);	//IRQENA_WAKE
	pr2100_write_register(ViPipe, 0x91, 0x00);	//IRQENA_GPIO
	pr2100_write_register(ViPipe, 0x94, 0xff);	//IRQCLR_WAKE
	pr2100_write_register(ViPipe, 0x95, 0xff);	//IRQCLR_GPIO
	pr2100_write_register(ViPipe, 0xa0, 0x33);	//IRQENA_VFD0
	pr2100_write_register(ViPipe, 0xb0, 0x33);	//IRQENA_VFD1
	pr2100_write_register(ViPipe, 0xc0, 0x21);	//MPP_CTRL0
	pr2100_write_register(ViPipe, 0xc1, 0x21);	//MPP_CTRL1
	pr2100_write_register(ViPipe, 0xc2, 0x21);	//MPP_CTRL2
	pr2100_write_register(ViPipe, 0xc3, 0x21);	//MPP_CTRL3
	pr2100_write_register(ViPipe, 0xc4, 0x21);	//MPP_CTRL4
	pr2100_write_register(ViPipe, 0xc5, 0x21);	//MPP_CTRL5
	pr2100_write_register(ViPipe, 0xc6, 0x21);	//MPP_CTRL6
	pr2100_write_register(ViPipe, 0xc7, 0x21);	//MPP_CTRL7
	pr2100_write_register(ViPipe, 0xc8, 0x21);	//MPP_CTRL8
	pr2100_write_register(ViPipe, 0xc9, 0x01);	//MPP_CTRL9
	pr2100_write_register(ViPipe, 0xca, 0x01);	//MPP_CTRLA
	pr2100_write_register(ViPipe, 0xcb, 0x01);	//MPP_CTRLB
	pr2100_write_register(ViPipe, 0xd0, 0x06);	//PLL0_CON0
	pr2100_write_register(ViPipe, 0xd1, 0x23);	//PLL0_CON1
	pr2100_write_register(ViPipe, 0xd2, 0x21);	//PLL0_CON2
	pr2100_write_register(ViPipe, 0xd3, 0x44);	//PLL0_CON3
	pr2100_write_register(ViPipe, 0xd4, 0x06);	//PLL1_CON0
	pr2100_write_register(ViPipe, 0xd5, 0x23);	//PLL1_CON1
	pr2100_write_register(ViPipe, 0xd6, 0x21);	//PLL1_CON2
	pr2100_write_register(ViPipe, 0xd7, 0x44);	//PLL1_CON3
	pr2100_write_register(ViPipe, 0xd8, 0x06);	//PLL2_CON0
	pr2100_write_register(ViPipe, 0xd9, 0x22);	//PLL2_CON1
	pr2100_write_register(ViPipe, 0xda, 0x2c);	//PLL2_CON2
	pr2100_write_register(ViPipe, 0xe0, 0x05);	//LATCH_EN_CON0
	pr2100_write_register(ViPipe, 0xe1, 0x05);	//LATCH_EN_CON1
	pr2100_write_register(ViPipe, 0xe2, 0x00);	//OUT_FMT
	pr2100_write_register(ViPipe, 0xe3, 0xc4);	//CHID_NUM, Y-Cb-Y-Cr
	pr2100_write_register(ViPipe, 0xe4, 0x20);	//CH_SEL0
	pr2100_write_register(ViPipe, 0xe5, 0x64);	//CH_SEL1
	pr2100_write_register(ViPipe, 0xe6, 0x20);	//CH_SEL2
	pr2100_write_register(ViPipe, 0xe7, 0x64);	//CH_SEL3
	pr2100_write_register(ViPipe, 0xe8, 0x00);	//VDCKP_PHASE
	pr2100_write_register(ViPipe, 0xe9, 0x00);	//VDCKN_PHASE
	pr2100_write_register(ViPipe, 0xea, 0x00);	//CLK_PWDN
	pr2100_write_register(ViPipe, 0xeb, 0x01);	//MIPI_DATA_EN
	pr2100_write_register(ViPipe, 0xf0, 0x03);	//PAR_OE_M
	pr2100_write_register(ViPipe, 0xf1, 0xff);	//PAR_OE_L
	pr2100_write_register(ViPipe, 0xf3, 0x06);	//PAD_MPP_CTL
	pr2100_write_register(ViPipe, 0xf4, 0x66);	//PAD_VD_CTL

	pr2100_write_register(ViPipe, 0xFF, 0x01);
	pr2100_write_register(ViPipe, 0x00, 0xe4);	//VID_CON0
	pr2100_write_register(ViPipe, 0x01, 0x61);	//GAINFIT0
	pr2100_write_register(ViPipe, 0x02, 0x00);	//PEDFIT0
	pr2100_write_register(ViPipe, 0x03, 0x56);	//CLAMP_CON0
	pr2100_write_register(ViPipe, 0x04, 0x0c);	//HPLL_CON0_0
	pr2100_write_register(ViPipe, 0x05, 0x88);	//HPLL_CON1_0
	pr2100_write_register(ViPipe, 0x06, 0x04);	//HPLL_CON2_0
	pr2100_write_register(ViPipe, 0x07, 0xb2);	//HPLL_CON3_0
	pr2100_write_register(ViPipe, 0x08, 0x44);	//HPLL_CON4_0
	pr2100_write_register(ViPipe, 0x09, 0x34);	//HPLL_CON5_0
	pr2100_write_register(ViPipe, 0x0a, 0x02);	//HPLL_CON6_0
	pr2100_write_register(ViPipe, 0x0b, 0x14);	//STD_POS0_0
	pr2100_write_register(ViPipe, 0x0c, 0x04);	//STD_POS1_0
	pr2100_write_register(ViPipe, 0x0d, 0x08);	//STD_POS2_0
	pr2100_write_register(ViPipe, 0x0e, 0x5e);	//STD_SLICE0_0
	pr2100_write_register(ViPipe, 0x0f, 0x5e);	//STD_SLICE1_0
	pr2100_write_register(ViPipe, 0x10, 0x26);	//STD_SLICE2_0
	pr2100_write_register(ViPipe, 0x11, 0x00);	//HDELAY_MSB0
	pr2100_write_register(ViPipe, 0x12, 0x87);	//HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0x13, 0x24);	//HDELAY_LSB0
	pr2100_write_register(ViPipe, 0x14, 0x80);	//HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x15, 0x2a);	//VDELAY0
	pr2100_write_register(ViPipe, 0x16, 0x38);	//VACTIVE0
	pr2100_write_register(ViPipe, 0x17, 0x00);	//CBG_DELAY_MSB0
	pr2100_write_register(ViPipe, 0x18, 0x80);	//CBG_ACTIVE_MSB0
	pr2100_write_register(ViPipe, 0x19, 0x48);	//CBG_DELAY_LSB0
	pr2100_write_register(ViPipe, 0x1a, 0x6c);	//CBG_ACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x1b, 0x05);	//VSMASK_INV_STRT0
	pr2100_write_register(ViPipe, 0x1c, 0x61);	//VSMASK_INV_END0
	pr2100_write_register(ViPipe, 0x1d, 0x07);	//CBP_DELAY_MSB0
	pr2100_write_register(ViPipe, 0x1e, 0x7e);	//CBP_DELAY_LSB0
	pr2100_write_register(ViPipe, 0x1f, 0x80);	//HSCL_ACTIVE_LSB0
	pr2100_write_register(ViPipe, 0x20, 0x80);	//CONT0
	pr2100_write_register(ViPipe, 0x21, 0x80);	//BRGT0
	pr2100_write_register(ViPipe, 0x22, 0x90);	//SAT0
	pr2100_write_register(ViPipe, 0x23, 0x80);	//HUE0
	pr2100_write_register(ViPipe, 0x24, 0x80);	//CB_GAIN0
	pr2100_write_register(ViPipe, 0x25, 0x80);	//CR_GAIN0
	pr2100_write_register(ViPipe, 0x26, 0x84);	//CB_OFF0
	pr2100_write_register(ViPipe, 0x27, 0x82);	//CR_OFF0
	pr2100_write_register(ViPipe, 0x28, 0x00);	//Y_OUT_GAIN0
	pr2100_write_register(ViPipe, 0x29, 0xff);	//DOWN_HSCL_MSB0
	pr2100_write_register(ViPipe, 0x2a, 0xff);	//DOWN_HSCL_LSB0
	pr2100_write_register(ViPipe, 0x2b, 0x00);	//MAN_HSCL_ADD0_0
	pr2100_write_register(ViPipe, 0x2c, 0x00);	//MAN_HSCL_ADD1_0
	pr2100_write_register(ViPipe, 0x2d, 0x00);	//MAN_HSCL_ADD2_0
	pr2100_write_register(ViPipe, 0x2e, 0x00);	//MAN_OUTFMT_ADD0_0
	pr2100_write_register(ViPipe, 0x2f, 0x00);	//MAN_OUTFMT_ADD1_0
	pr2100_write_register(ViPipe, 0x30, 0x00);	//MAN_OUTFMT_ADD2_0
	pr2100_write_register(ViPipe, 0x31, 0x00);	//MAN_COMB_MD0
	pr2100_write_register(ViPipe, 0x32, 0xc0);	//COMB_CRL_MD0
	pr2100_write_register(ViPipe, 0x33, 0x14);	//COMB_CVBS_CRL0_0
	pr2100_write_register(ViPipe, 0x34, 0x14);	//COMB_CVBS_CRL1_0
	pr2100_write_register(ViPipe, 0x35, 0x80);	//COMB_CVBS_CRL2_0
	pr2100_write_register(ViPipe, 0x36, 0x80);	//COMB_CVBS_CRL3_0
	pr2100_write_register(ViPipe, 0x37, 0xad);	//HD_Y_NOTCH_MD0
	pr2100_write_register(ViPipe, 0x38, 0x4b);	//HD_COMB_FLT_MD0
	pr2100_write_register(ViPipe, 0x39, 0x08);	//Y_DYN_PEAK_GN0
	pr2100_write_register(ViPipe, 0x3a, 0x21);	//HD_Y_LPF_MD0
	pr2100_write_register(ViPipe, 0x3b, 0x02);	//Y_HPF_CORE_MIN0
	pr2100_write_register(ViPipe, 0x3c, 0x01);	//Y_HPF_CORE_STEP0
	pr2100_write_register(ViPipe, 0x3d, 0x23);	//CORE_CON0
	pr2100_write_register(ViPipe, 0x3e, 0x05);	//MAN_CLPF_MD0
	pr2100_write_register(ViPipe, 0x3f, 0xc8);	//HD_CTI_CON0
	pr2100_write_register(ViPipe, 0x40, 0x05);	//C_LOOP_CON0_0
	pr2100_write_register(ViPipe, 0x41, 0x55);	//C_LOOP_CON1_0
	pr2100_write_register(ViPipe, 0x42, 0x01);	//C_LOOP_CON2_0
	pr2100_write_register(ViPipe, 0x43, 0x38);	//C_LOOP_CON3_0
	pr2100_write_register(ViPipe, 0x44, 0x6a);	//C_HUE_CVI0_0
	pr2100_write_register(ViPipe, 0x45, 0x00);	//C_HUE_CVI1_0
	pr2100_write_register(ViPipe, 0x46, 0x14);	//C_PHASE_REF0_0
	pr2100_write_register(ViPipe, 0x47, 0xb0);	//C_PHASE_REF1_0
	pr2100_write_register(ViPipe, 0x48, 0xdf);	//C_PHASE_REF2_0
	pr2100_write_register(ViPipe, 0x49, 0x00);	//G_GAIN_REF0_0
	pr2100_write_register(ViPipe, 0x4a, 0x7b);	//G_GAIN_REF1_0
	pr2100_write_register(ViPipe, 0x4b, 0x60);	//MAN_C_GAIN_VAL0_0
	pr2100_write_register(ViPipe, 0x4c, 0x00);	//MAN_C_GAIN_VAL1_0
	pr2100_write_register(ViPipe, 0x4d, 0x26);	//YC_DELAY0
	pr2100_write_register(ViPipe, 0x4e, 0x00);	//HD_HALF_MD0
	pr2100_write_register(ViPipe, 0x4f, 0x2c);	//OUTFMT_CON0
	pr2100_write_register(ViPipe, 0x50, 0x21);	//TST_EQ_POS0_0
	pr2100_write_register(ViPipe, 0x51, 0x28);	//TST_EQ_POS1_0
	pr2100_write_register(ViPipe, 0x52, 0x40);	//TST_EQ_POS2_0
	pr2100_write_register(ViPipe, 0x53, 0x0c);	//TST_VID_DEC0
	pr2100_write_register(ViPipe, 0x54, 0x0f);	//TST_VID_RST0
	pr2100_write_register(ViPipe, 0x55, 0x8d);	//PTZ_SLICE_LVL0
	pr2100_write_register(ViPipe, 0x70, 0x06);	//COMB_VCRL_VTH1_0
	pr2100_write_register(ViPipe, 0x71, 0x08);	//COMB_VCRL_VTH2_0
	pr2100_write_register(ViPipe, 0x72, 0x0a);	//COMB_VCRL_VTH3_0
	pr2100_write_register(ViPipe, 0x73, 0x0c);	//COMB_VCRL_VTH4_0
	pr2100_write_register(ViPipe, 0x74, 0x0e);	//COMB_VCRL_VTH5_0
	pr2100_write_register(ViPipe, 0x75, 0x10);	//COMB_VCRL_VTH6_0
	pr2100_write_register(ViPipe, 0x76, 0x12);	//COMB_VCRL_VTH7_0
	pr2100_write_register(ViPipe, 0x77, 0x14);	//COMB_VCRL_VTH8_0
	pr2100_write_register(ViPipe, 0x78, 0x06);	//COMB_VMIX_VTH1_0
	pr2100_write_register(ViPipe, 0x79, 0x08);	//COMB_VMIX_VTH2_0
	pr2100_write_register(ViPipe, 0x7a, 0x0a);	//COMB_VMIX_VTH3_0
	pr2100_write_register(ViPipe, 0x7b, 0x0c);	//COMB_VMIX_VTH4_0
	pr2100_write_register(ViPipe, 0x7c, 0x0e);	//COMB_VMIX_VTH5_0
	pr2100_write_register(ViPipe, 0x7d, 0x10);	//COMB_VMIX_VTH6_0
	pr2100_write_register(ViPipe, 0x7e, 0x12);	//COMB_VMIX_VTH7_0
	pr2100_write_register(ViPipe, 0x7f, 0x14);	//COMB_VMIX_VTH8_0
	pr2100_write_register(ViPipe, 0x80, 0x00);	//STD_VPOS_PVI_960MSB0
	pr2100_write_register(ViPipe, 0x81, 0x09);	//STD_VPOS_PVI_960LSB0
	pr2100_write_register(ViPipe, 0x82, 0x00);	//STD_VPOS_CVI_960MSB0
	pr2100_write_register(ViPipe, 0x83, 0x07);	//STD_VPOS_CVI_960LSB0
	pr2100_write_register(ViPipe, 0x84, 0x00);	//STD_VPOS_AHD_960MSB0
	pr2100_write_register(ViPipe, 0x85, 0x17);	//STD_VPOS_AHD_960LSB0
	pr2100_write_register(ViPipe, 0x86, 0x03);	//STD_VPOS_TVI_960MSB0
	pr2100_write_register(ViPipe, 0x87, 0xe5);	//STD_VPOS_TVI_960LSB0
	pr2100_write_register(ViPipe, 0x88, 0x05);	//STD_HPOS_PVI_960MSB0
	pr2100_write_register(ViPipe, 0x89, 0x24);	//STD_HPOS_PVI_960LSB0
	pr2100_write_register(ViPipe, 0x8a, 0x05);	//STD_HPOS_CVI_960MSB0
	pr2100_write_register(ViPipe, 0x8b, 0x24);	//STD_HPOS_CVI_960LSB0
	pr2100_write_register(ViPipe, 0x8c, 0x08);	//STD_HPOS_AHD_960MSB0
	pr2100_write_register(ViPipe, 0x8d, 0xe8);	//STD_HPOS_AHD_960LSB0
	pr2100_write_register(ViPipe, 0x8e, 0x05);	//STD_HPOS_TVI_960MSB0
	pr2100_write_register(ViPipe, 0x8f, 0x47);	//STD_HPOS_TVI_960LSB0
	pr2100_write_register(ViPipe, 0x90, 0x02);	//VSYNC_ACCUM_960MSB0
	pr2100_write_register(ViPipe, 0x91, 0xb4);	//VSYNC_ACCUM_960LSB0
	pr2100_write_register(ViPipe, 0x92, 0x73);	//VLINE_MAX_960_MSB0
	pr2100_write_register(ViPipe, 0x93, 0xe8);	//VLINE_MAX_960_LSB0
	pr2100_write_register(ViPipe, 0x94, 0x0f);	//SLICE_VSYNC0
	pr2100_write_register(ViPipe, 0x95, 0x5e);	//STD_SLICE_AHD0
	pr2100_write_register(ViPipe, 0x96, 0x03);	//HACT_SIZE_960_MSB0
	pr2100_write_register(ViPipe, 0x97, 0xd0);	//HACT_SIZE_960_LSB0
	pr2100_write_register(ViPipe, 0x98, 0x17);	//HPERIOD_960P25MSB0
	pr2100_write_register(ViPipe, 0x99, 0x34);	//HPERIOD_960P25LSB0
	pr2100_write_register(ViPipe, 0x9a, 0x13);	//HPERIOD_960P30MSB0
	pr2100_write_register(ViPipe, 0x9b, 0x56);	//HPERIOD_960P30LSB0
	pr2100_write_register(ViPipe, 0x9c, 0x0b);	//HPERIOD_960P50MSB0
	pr2100_write_register(ViPipe, 0x9d, 0x9a);	//HPERIOD_960P50LSB0
	pr2100_write_register(ViPipe, 0x9e, 0x09);	//HPERIOD_960P60MSB0
	pr2100_write_register(ViPipe, 0x9f, 0xab);	//HPERIOD_960P60LSB0
	pr2100_write_register(ViPipe, 0xa0, 0x01);	//VALY_STRT_960P25MSB0
	pr2100_write_register(ViPipe, 0xa1, 0x74);	//VALY_STRT_960P25LSB0
	pr2100_write_register(ViPipe, 0xa2, 0x01);	//VALY_STRT_960P30MSB0
	pr2100_write_register(ViPipe, 0xa3, 0x6b);	//VALY_STRT_960P30LSB0
	pr2100_write_register(ViPipe, 0xa4, 0x00);	//VALY_STRT_960P50MSB0
	pr2100_write_register(ViPipe, 0xa5, 0xba);	//VALY_STRT_960P50LSB0
	pr2100_write_register(ViPipe, 0xa6, 0x00);	//VALY_STRT_960P60MSB0
	pr2100_write_register(ViPipe, 0xa7, 0xa3);	//VALY_STRT_960P60LSB0
	pr2100_write_register(ViPipe, 0xa8, 0x01);	//PED_STRT_960P25MSB0
	pr2100_write_register(ViPipe, 0xa9, 0x39);	//PED_STRT_960P25LSB0
	pr2100_write_register(ViPipe, 0xaa, 0x01);	//PED_STRT_960P30MSB0
	pr2100_write_register(ViPipe, 0xab, 0x39);	//PED_STRT_960P30LSB0
	pr2100_write_register(ViPipe, 0xac, 0x00);	//PED_STRT_960P50MSB0
	pr2100_write_register(ViPipe, 0xad, 0xc1);	//PED_STRT_960P50LSB0
	pr2100_write_register(ViPipe, 0xae, 0x00);	//PED_STRT_960P60MSB0
	pr2100_write_register(ViPipe, 0xaf, 0xc1);	//PED_STRT_960P60LSB0
	pr2100_write_register(ViPipe, 0xb0, 0x05);	//COMB_MEM_960PMSB0
	pr2100_write_register(ViPipe, 0xb1, 0xcc);	//COMB_MEM_960PLSB0
	pr2100_write_register(ViPipe, 0xb2, 0x09);	//C_JIT_GAIN_960PMSB0
	pr2100_write_register(ViPipe, 0xb3, 0x6d);	//C_JIT_GAIN_960PLSB0
	pr2100_write_register(ViPipe, 0xb4, 0x00);	//STD_VPOS_AHD_MSB0
	pr2100_write_register(ViPipe, 0xb5, 0x17);	//STD_VPOS_AHD_LSB0
	pr2100_write_register(ViPipe, 0xb6, 0x08);	//STD_HPOS_AHD_MSB0
	pr2100_write_register(ViPipe, 0xb7, 0xe8);	//STD_HPOS_AHD_LSB0
	pr2100_write_register(ViPipe, 0xb8, 0xb0);	//STD_TVI_OFFSET_25F0
	pr2100_write_register(ViPipe, 0xb9, 0xce);	//STD_TVI_OFFSET_30F0
	pr2100_write_register(ViPipe, 0xba, 0x90);	//STD_TVI_OFFSET_960P0
	pr2100_write_register(ViPipe, 0xbb, 0x00);	//EXT_SYNC_CON0
	pr2100_write_register(ViPipe, 0xbc, 0x00);	//VOSYNC_HDELAY_MSB0
	pr2100_write_register(ViPipe, 0xbd, 0x04);	//VOSYNC_HDELAY_LSB0
	pr2100_write_register(ViPipe, 0xbe, 0x07);	//VOSYNC_HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xbf, 0x80);	//VOSYNC_HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xc0, 0x00);	//VOSYNC_VDELAY_MSB0
	pr2100_write_register(ViPipe, 0xc1, 0x00);	//VOSYNC_VDELAY_LSB0
	#if PR2100_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	#else
	pr2100_write_register(ViPipe, 0xc2, 0x04);	//VOSYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xc3, 0x39);	//VOSYNC_VACTIVE_LSB0
	#endif
	pr2100_write_register(ViPipe, 0xc4, output_pattern_ch0);
	pr2100_write_register(ViPipe, 0xc9, 0x00);	//VISYNC_HDELAY_MSB0
	pr2100_write_register(ViPipe, 0xca, 0x02);	//VISYNC_HDELAY_LSB0
	pr2100_write_register(ViPipe, 0xcb, 0x07);	//VISYNC_HACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xcc, 0x80);	//VISYNC_HACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xcd, 0x08);	//VISYNC_VDELAY_MSB0
	pr2100_write_register(ViPipe, 0xce, 0x20);	//VISYNC_VDELAY_LSB0
	pr2100_write_register(ViPipe, 0xcf, 0x04);	//VISYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xd1, 0x10);	//MIPI_DATA_FLD_CON_0
	pr2100_write_register(ViPipe, 0xd2, 0x00);	//MIPI_DATA_ODD_VAL_0
	pr2100_write_register(ViPipe, 0xd3, 0x00);	//MIPI_DATA_EVEN_VAL_0
	#if PR2100_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xcf, 0x8c);	//VISYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xcd, 0x40);	//VISYNC_VDELAY_MSB0
	#endif
	pr2100_write_register(ViPipe, 0xFF, 0x02);
	pr2100_write_register(ViPipe, 0x00, 0xe4);	//VID_CON1
	pr2100_write_register(ViPipe, 0x01, 0x61);	//GAINFIT1
	pr2100_write_register(ViPipe, 0x02, 0x00);	//PEDFIT1
	pr2100_write_register(ViPipe, 0x03, 0x56);	//CLAMP_CON1
	pr2100_write_register(ViPipe, 0x04, 0x0c);	//HPLL_CON0_1
	pr2100_write_register(ViPipe, 0x05, 0x88);	//HPLL_CON1_1
	pr2100_write_register(ViPipe, 0x06, 0x04);	//HPLL_CON2_1
	pr2100_write_register(ViPipe, 0x07, 0xb2);	//HPLL_CON3_1
	pr2100_write_register(ViPipe, 0x08, 0x44);	//HPLL_CON4_1
	pr2100_write_register(ViPipe, 0x09, 0x34);	//HPLL_CON5_1
	pr2100_write_register(ViPipe, 0x0a, 0x02);	//HPLL_CON6_1
	pr2100_write_register(ViPipe, 0x0b, 0x14);	//STD_POS0_1
	pr2100_write_register(ViPipe, 0x0c, 0x04);	//STD_POS1_1
	pr2100_write_register(ViPipe, 0x0d, 0x08);	//STD_POS2_1
	pr2100_write_register(ViPipe, 0x0e, 0x5e);	//STD_SLICE0_1
	pr2100_write_register(ViPipe, 0x0f, 0x5e);	//STD_SLICE1_1
	pr2100_write_register(ViPipe, 0x10, 0x26);	//STD_SLICE2_1
	pr2100_write_register(ViPipe, 0x11, 0x00);	//HDELAY_MSB1
	pr2100_write_register(ViPipe, 0x12, 0x87);	//HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0x13, 0x24);	//HDELAY_LSB1
	pr2100_write_register(ViPipe, 0x14, 0x80);	//HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x15, 0x2a);	//VDELAY1
	pr2100_write_register(ViPipe, 0x16, 0x38);	//VACTIVE1
	pr2100_write_register(ViPipe, 0x17, 0x00);	//CBG_DELAY_MSB1
	pr2100_write_register(ViPipe, 0x18, 0x80);	//CBG_ACTIVE_MSB1
	pr2100_write_register(ViPipe, 0x19, 0x48);	//CBG_DELAY_LSB1
	pr2100_write_register(ViPipe, 0x1a, 0x6c);	//CBG_ACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x1b, 0x05);	//VSMASK_INV_STRT1
	pr2100_write_register(ViPipe, 0x1c, 0x61);	//VSMASK_INV_END1
	pr2100_write_register(ViPipe, 0x1d, 0x07);	//CBP_DELAY_MSB1
	pr2100_write_register(ViPipe, 0x1e, 0x7e);	//CBP_DELAY_LSB1
	pr2100_write_register(ViPipe, 0x1f, 0x80);	//HSCL_ACTIVE_LSB1
	pr2100_write_register(ViPipe, 0x20, 0x80);	//CONT1
	pr2100_write_register(ViPipe, 0x21, 0x80);	//BRGT1
	pr2100_write_register(ViPipe, 0x22, 0x90);	//SAT1
	pr2100_write_register(ViPipe, 0x23, 0x80);	//HUE1
	pr2100_write_register(ViPipe, 0x24, 0x80);	//CB_GAIN1
	pr2100_write_register(ViPipe, 0x25, 0x80);	//CR_GAIN1
	pr2100_write_register(ViPipe, 0x26, 0x84);	//CB_OFF1
	pr2100_write_register(ViPipe, 0x27, 0x82);	//CR_OFF1
	pr2100_write_register(ViPipe, 0x28, 0x00);	//Y_OUT_GAIN1
	pr2100_write_register(ViPipe, 0x29, 0xff);	//DOWN_HSCL_MSB1
	pr2100_write_register(ViPipe, 0x2a, 0xff);	//DOWN_HSCL_LSB1
	pr2100_write_register(ViPipe, 0x2b, 0x00);	//MAN_HSCL_ADD0_1
	pr2100_write_register(ViPipe, 0x2c, 0x00);	//MAN_HSCL_ADD1_1
	pr2100_write_register(ViPipe, 0x2d, 0x00);	//MAN_HSCL_ADD2_1
	pr2100_write_register(ViPipe, 0x2e, 0x00);	//MAN_OUTFMT_ADD0_1
	pr2100_write_register(ViPipe, 0x2f, 0x00);	//MAN_OUTFMT_ADD1_1
	pr2100_write_register(ViPipe, 0x30, 0x00);	//MAN_OUTFMT_ADD2_1
	pr2100_write_register(ViPipe, 0x31, 0x00);	//MAN_COMB_MD1
	pr2100_write_register(ViPipe, 0x32, 0xc0);	//COMB_CRL_MD1
	pr2100_write_register(ViPipe, 0x33, 0x14);	//COMB_CVBS_CRL0_1
	pr2100_write_register(ViPipe, 0x34, 0x14);	//COMB_CVBS_CRL1_1
	pr2100_write_register(ViPipe, 0x35, 0x80);	//COMB_CVBS_CRL2_1
	pr2100_write_register(ViPipe, 0x36, 0x80);	//COMB_CVBS_CRL3_1
	pr2100_write_register(ViPipe, 0x37, 0xad);	//HD_Y_NOTCH_MD1
	pr2100_write_register(ViPipe, 0x38, 0x4b);	//HD_COMB_FLT_MD1
	pr2100_write_register(ViPipe, 0x39, 0x08);	//Y_DYN_PEAK_GN1
	pr2100_write_register(ViPipe, 0x3a, 0x21);	//HD_Y_LPF_MD1
	pr2100_write_register(ViPipe, 0x3b, 0x02);	//Y_HPF_CORE_MIN1
	pr2100_write_register(ViPipe, 0x3c, 0x01);	//Y_HPF_CORE_STEP1
	pr2100_write_register(ViPipe, 0x3d, 0x23);	//CORE_CON1
	pr2100_write_register(ViPipe, 0x3e, 0x05);	//MAN_CLPF_MD1
	pr2100_write_register(ViPipe, 0x3f, 0xc8);	//HD_CTI_CON1
	pr2100_write_register(ViPipe, 0x40, 0x05);	//C_LOOP_CON0_1
	pr2100_write_register(ViPipe, 0x41, 0x55);	//C_LOOP_CON1_1
	pr2100_write_register(ViPipe, 0x42, 0x01);	//C_LOOP_CON2_1
	pr2100_write_register(ViPipe, 0x43, 0x38);	//C_LOOP_CON3_1
	pr2100_write_register(ViPipe, 0x44, 0x6a);	//C_HUE_CVI0_1
	pr2100_write_register(ViPipe, 0x45, 0x00);	//C_HUE_CVI1_1
	pr2100_write_register(ViPipe, 0x46, 0x14);	//C_PHASE_REF0_1
	pr2100_write_register(ViPipe, 0x47, 0xb0);	//C_PHASE_REF1_1
	pr2100_write_register(ViPipe, 0x48, 0xdf);	//C_PHASE_REF2_1
	pr2100_write_register(ViPipe, 0x49, 0x00);	//G_GAIN_REF0_1
	pr2100_write_register(ViPipe, 0x4a, 0x7b);	//G_GAIN_REF1_1
	pr2100_write_register(ViPipe, 0x4b, 0x60);	//MAN_C_GAIN_VAL0_1
	pr2100_write_register(ViPipe, 0x4c, 0x00);	//MAN_C_GAIN_VAL1_1
	pr2100_write_register(ViPipe, 0x4d, 0x26);	//YC_DELAY1
	pr2100_write_register(ViPipe, 0x4e, 0x00);	//HD_HALF_MD1
	pr2100_write_register(ViPipe, 0x4f, 0x2c);	//OUTFMT_CON1
	pr2100_write_register(ViPipe, 0x50, 0x21);	//TST_EQ_POS0_1
	pr2100_write_register(ViPipe, 0x51, 0x28);	//TST_EQ_POS1_1
	pr2100_write_register(ViPipe, 0x52, 0x40);	//TST_EQ_POS2_1
	pr2100_write_register(ViPipe, 0x53, 0x0c);	//TST_VID_DEC1
	pr2100_write_register(ViPipe, 0x54, 0x0f);	//TST_VID_RST1
	pr2100_write_register(ViPipe, 0x55, 0x8d);	//PTZ_SLICE_LVL1
	pr2100_write_register(ViPipe, 0x70, 0x06);	//COMB_VCRL_VTH1_1
	pr2100_write_register(ViPipe, 0x71, 0x08);	//COMB_VCRL_VTH2_1
	pr2100_write_register(ViPipe, 0x72, 0x0a);	//COMB_VCRL_VTH3_1
	pr2100_write_register(ViPipe, 0x73, 0x0c);	//COMB_VCRL_VTH4_1
	pr2100_write_register(ViPipe, 0x74, 0x0e);	//COMB_VCRL_VTH5_1
	pr2100_write_register(ViPipe, 0x75, 0x10);	//COMB_VCRL_VTH6_1
	pr2100_write_register(ViPipe, 0x76, 0x12);	//COMB_VCRL_VTH7_1
	pr2100_write_register(ViPipe, 0x77, 0x14);	//COMB_VCRL_VTH8_1
	pr2100_write_register(ViPipe, 0x78, 0x06);	//COMB_VMIX_VTH1_1
	pr2100_write_register(ViPipe, 0x79, 0x08);	//COMB_VMIX_VTH2_1
	pr2100_write_register(ViPipe, 0x7a, 0x0a);	//COMB_VMIX_VTH3_1
	pr2100_write_register(ViPipe, 0x7b, 0x0c);	//COMB_VMIX_VTH4_1
	pr2100_write_register(ViPipe, 0x7c, 0x0e);	//COMB_VMIX_VTH5_1
	pr2100_write_register(ViPipe, 0x7d, 0x10);	//COMB_VMIX_VTH6_1
	pr2100_write_register(ViPipe, 0x7e, 0x12);	//COMB_VMIX_VTH7_1
	pr2100_write_register(ViPipe, 0x7f, 0x14);	//COMB_VMIX_VTH8_1
	pr2100_write_register(ViPipe, 0x80, 0x00);	//STD_VPOS_PVI_960MSB1
	pr2100_write_register(ViPipe, 0x81, 0x09);	//STD_VPOS_PVI_960LSB1
	pr2100_write_register(ViPipe, 0x82, 0x00);	//STD_VPOS_CVI_960MSB1
	pr2100_write_register(ViPipe, 0x83, 0x07);	//STD_VPOS_CVI_960LSB1
	pr2100_write_register(ViPipe, 0x84, 0x00);	//STD_VPOS_AHD_960MSB1
	pr2100_write_register(ViPipe, 0x85, 0x17);	//STD_VPOS_AHD_960LSB1
	pr2100_write_register(ViPipe, 0x86, 0x03);	//STD_VPOS_TVI_960MSB1
	pr2100_write_register(ViPipe, 0x87, 0xe5);	//STD_VPOS_TVI_960LSB1
	pr2100_write_register(ViPipe, 0x88, 0x05);	//STD_HPOS_PVI_960MSB1
	pr2100_write_register(ViPipe, 0x89, 0x24);	//STD_HPOS_PVI_960LSB1
	pr2100_write_register(ViPipe, 0x8a, 0x05);	//STD_HPOS_CVI_960MSB1
	pr2100_write_register(ViPipe, 0x8b, 0x24);	//STD_HPOS_CVI_960LSB1
	pr2100_write_register(ViPipe, 0x8c, 0x08);	//STD_HPOS_AHD_960MSB1
	pr2100_write_register(ViPipe, 0x8d, 0xe8);	//STD_HPOS_AHD_960LSB1
	pr2100_write_register(ViPipe, 0x8e, 0x05);	//STD_HPOS_TVI_960MSB1
	pr2100_write_register(ViPipe, 0x8f, 0x47);	//STD_HPOS_TVI_960LSB1
	pr2100_write_register(ViPipe, 0x90, 0x02);	//VSYNC_ACCUM_960MSB1
	pr2100_write_register(ViPipe, 0x91, 0xb4);	//VSYNC_ACCUM_960LSB1
	pr2100_write_register(ViPipe, 0x92, 0x73);	//VLINE_MAX_960_MSB1
	pr2100_write_register(ViPipe, 0x93, 0xe8);	//VLINE_MAX_960_LSB1
	pr2100_write_register(ViPipe, 0x94, 0x0f);	//SLICE_VSYNC1
	pr2100_write_register(ViPipe, 0x95, 0x5e);	//STD_SLICE_AHD1
	pr2100_write_register(ViPipe, 0x96, 0x03);	//HACT_SIZE_960_MSB1
	pr2100_write_register(ViPipe, 0x97, 0xd0);	//HACT_SIZE_960_LSB1
	pr2100_write_register(ViPipe, 0x98, 0x17);	//HPERIOD_960P25MSB1
	pr2100_write_register(ViPipe, 0x99, 0x34);	//HPERIOD_960P25LSB1
	pr2100_write_register(ViPipe, 0x9a, 0x13);	//HPERIOD_960P30MSB1
	pr2100_write_register(ViPipe, 0x9b, 0x56);	//HPERIOD_960P30LSB1
	pr2100_write_register(ViPipe, 0x9c, 0x0b);	//HPERIOD_960P50MSB1
	pr2100_write_register(ViPipe, 0x9d, 0x9a);	//HPERIOD_960P50LSB1
	pr2100_write_register(ViPipe, 0x9e, 0x09);	//HPERIOD_960P60MSB1
	pr2100_write_register(ViPipe, 0x9f, 0xab);	//HPERIOD_960P60LSB1
	pr2100_write_register(ViPipe, 0xa0, 0x01);	//VALY_STRT_960P25MSB1
	pr2100_write_register(ViPipe, 0xa1, 0x74);	//VALY_STRT_960P25LSB1
	pr2100_write_register(ViPipe, 0xa2, 0x01);	//VALY_STRT_960P30MSB1
	pr2100_write_register(ViPipe, 0xa3, 0x6b);	//VALY_STRT_960P30LSB1
	pr2100_write_register(ViPipe, 0xa4, 0x00);	//VALY_STRT_960P50MSB1
	pr2100_write_register(ViPipe, 0xa5, 0xba);	//VALY_STRT_960P50LSB1
	pr2100_write_register(ViPipe, 0xa6, 0x00);	//VALY_STRT_960P60MSB1
	pr2100_write_register(ViPipe, 0xa7, 0xa3);	//VALY_STRT_960P60LSB1
	pr2100_write_register(ViPipe, 0xa8, 0x01);	//PED_STRT_960P25MSB1
	pr2100_write_register(ViPipe, 0xa9, 0x39);	//PED_STRT_960P25LSB1
	pr2100_write_register(ViPipe, 0xaa, 0x01);	//PED_STRT_960P30MSB1
	pr2100_write_register(ViPipe, 0xab, 0x39);	//PED_STRT_960P30LSB1
	pr2100_write_register(ViPipe, 0xac, 0x00);	//PED_STRT_960P50MSB1
	pr2100_write_register(ViPipe, 0xad, 0xc1);	//PED_STRT_960P50LSB1
	pr2100_write_register(ViPipe, 0xae, 0x00);	//PED_STRT_960P60MSB1
	pr2100_write_register(ViPipe, 0xaf, 0xc1);	//PED_STRT_960P60LSB1
	pr2100_write_register(ViPipe, 0xb0, 0x05);	//COMB_MEM_960PMSB1
	pr2100_write_register(ViPipe, 0xb1, 0xcc);	//COMB_MEM_960PLSB1
	pr2100_write_register(ViPipe, 0xb2, 0x09);	//C_JIT_GAIN_960PMSB1
	pr2100_write_register(ViPipe, 0xb3, 0x6d);	//C_JIT_GAIN_960PLSB1
	pr2100_write_register(ViPipe, 0xb4, 0x00);	//STD_VPOS_AHD_MSB1
	pr2100_write_register(ViPipe, 0xb5, 0x17);	//STD_VPOS_AHD_LSB1
	pr2100_write_register(ViPipe, 0xb6, 0x08);	//STD_HPOS_AHD_MSB1
	pr2100_write_register(ViPipe, 0xb7, 0xe8);	//STD_HPOS_AHD_LSB1
	pr2100_write_register(ViPipe, 0xb8, 0xb0);	//STD_TVI_OFFSET_25F1
	pr2100_write_register(ViPipe, 0xb9, 0xce);	//STD_TVI_OFFSET_30F1
	pr2100_write_register(ViPipe, 0xba, 0x90);	//STD_TVI_OFFSET_960P1
	pr2100_write_register(ViPipe, 0xbb, 0x00);	//EXT_SYNC_CON1
	pr2100_write_register(ViPipe, 0xbc, 0x00);	//VOSYNC_HDELAY_MSB1
	pr2100_write_register(ViPipe, 0xbd, 0x04);	//VOSYNC_HDELAY_LSB1
	pr2100_write_register(ViPipe, 0xbe, 0x07);	//VOSYNC_HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xbf, 0x80);	//VOSYNC_HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xc0, 0x00);	//VOSYNC_VDELAY_MSB1
	pr2100_write_register(ViPipe, 0xc1, 0x00);	//VOSYNC_VDELAY_LSB1
	#if PR2100_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xc2, 0x44);
	pr2100_write_register(ViPipe, 0xc3, 0x38);
	#else
	pr2100_write_register(ViPipe, 0xc2, 0x04);	//VOSYNC_VACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xc3, 0x39);	//VOSYNC_VACTIVE_LSB1
	#endif
	pr2100_write_register(ViPipe, 0xc4, output_pattern_ch1);
	pr2100_write_register(ViPipe, 0xc9, 0x00);	//VISYNC_HDELAY_MSB1
	pr2100_write_register(ViPipe, 0xca, 0x02);	//VISYNC_HDELAY_LSB1
	pr2100_write_register(ViPipe, 0xcb, 0x07);	//VISYNC_HACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xcc, 0x80);	//VISYNC_HACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xcd, 0x08);	//VISYNC_VDELAY_MSB1
	pr2100_write_register(ViPipe, 0xce, 0x20);	//VISYNC_VDELAY_LSB1
	pr2100_write_register(ViPipe, 0xcf, 0x04);	//VISYNC_VACTIVE_MSB1
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB1
	pr2100_write_register(ViPipe, 0xd1, 0x10);	//MIPI_DATA_FLD_CON_1
	pr2100_write_register(ViPipe, 0xd2, 0x00);	//MIPI_DATA_ODD_VAL_1
	pr2100_write_register(ViPipe, 0xd3, 0x00);	//MIPI_DATA_EVEN_VAL_1
	#if PR2100_TEST_PATTERN
	pr2100_write_register(ViPipe, 0xcf, 0x8c);	//VISYNC_VACTIVE_MSB0
	pr2100_write_register(ViPipe, 0xd0, 0x38);	//VISYNC_VACTIVE_LSB0
	pr2100_write_register(ViPipe, 0xcd, 0x40);	//VISYNC_VDELAY_MSB0
	#endif
	pr2100_write_register(ViPipe, 0xFF, 0x05);
	pr2100_write_register(ViPipe, 0x09, 0x00);	//REF_CH_STRT
	pr2100_write_register(ViPipe, 0x0a, 0x03);	//REF_CH_VDLY
	pr2100_write_register(ViPipe, 0x0e, 0x80);	//VBLK_CODE
	pr2100_write_register(ViPipe, 0x0f, 0x10);	//VBLK_CODE
	pr2100_write_register(ViPipe, 0x10, 0xb3);	//MTX_CTRL
	pr2100_write_register(ViPipe, 0x11, 0xb0);	//MTX_LONG_DLY_H
	pr2100_write_register(ViPipe, 0x12, 0x6e);	//MTX_LONG_DLY_L
	pr2100_write_register(ViPipe, 0x13, 0x00);	//MTX_SHORT_DLY_H
	pr2100_write_register(ViPipe, 0x14, 0x6e);	//MTX_SHORT_DLY_L
	pr2100_write_register(ViPipe, 0x15, 0x00);	//REF_OFFSET
	pr2100_write_register(ViPipe, 0x16, 0x00);	//REF_FE_HDLY
	pr2100_write_register(ViPipe, 0x17, 0x00);	//REF_FE_HDLY
	pr2100_write_register(ViPipe, 0x18, 0x00);	//REF_FS_HDLY
	pr2100_write_register(ViPipe, 0x19, 0x00);	//REF_FS_HDLY
	pr2100_write_register(ViPipe, 0x1a, 0x00);	//REF_HTOTAL
	pr2100_write_register(ViPipe, 0x1b, 0x00);	//REF_HTOTAL
	pr2100_write_register(ViPipe, 0x1c, 0x00);	//REF_VTOTAL
	pr2100_write_register(ViPipe, 0x1d, 0x00);	//REF_VTOTAL
	pr2100_write_register(ViPipe, 0x1e, 0x00);	//CHID_MD
	pr2100_write_register(ViPipe, 0x20, 0x88);	//MTX_CH0_CTRL
	pr2100_write_register(ViPipe, 0x21, 0x07);	//MTX_CH0_HSIZE
	pr2100_write_register(ViPipe, 0x22, 0x80);	//MTX_CH0_HSIZE
	pr2100_write_register(ViPipe, 0x23, 0x04);	//MTX_CH0_VSIZE
	pr2100_write_register(ViPipe, 0x24, 0x38);	//MTX_CH0_VSIZE
	pr2100_write_register(ViPipe, 0x25, 0x0f);	//CH0_FS_OS_H
	pr2100_write_register(ViPipe, 0x26, 0x00);	//CH0_FS_OS_L
	pr2100_write_register(ViPipe, 0x27, 0x0f);	//CH0_FE_OS_H
	pr2100_write_register(ViPipe, 0x28, 0x00);	//CH0_FE_OS_L
	pr2100_write_register(ViPipe, 0x29, 0x0b);	//REF_CH0_VS_OS
	pr2100_write_register(ViPipe, 0x2a, 0x40);	//REF_CH0_VS_OS
	pr2100_write_register(ViPipe, 0x30, 0x98);	//MTX_CH1_CTRL
	pr2100_write_register(ViPipe, 0x31, 0x07);	//MTX_CH1_HSIZE
	pr2100_write_register(ViPipe, 0x32, 0x80);	//MTX_CH1_HSIZE
	pr2100_write_register(ViPipe, 0x33, 0x04);	//MTX_CH1_VSIZE
	pr2100_write_register(ViPipe, 0x34, 0x38);	//MTX_CH1_VSIZE
	pr2100_write_register(ViPipe, 0x35, 0x0f);	//CH1_FS_OS_H
	pr2100_write_register(ViPipe, 0x36, 0x00);	//CH1_FS_OS_L
	pr2100_write_register(ViPipe, 0x37, 0x0f);	//CH1_FE_OS_H
	pr2100_write_register(ViPipe, 0x38, 0x00);	//CH1_FE_OS_L
	pr2100_write_register(ViPipe, 0x39, 0x07);	//REF_CH1_VS_OS
	pr2100_write_register(ViPipe, 0x3a, 0x80);	//REF_CH1_VS_OS
	pr2100_write_register(ViPipe, 0x40, 0xa8);	//MTX_CH2_CTRL
	pr2100_write_register(ViPipe, 0x41, 0x07);	//MTX_CH2_HSIZE
	pr2100_write_register(ViPipe, 0x42, 0x80);	//MTX_CH2_HSIZE
	pr2100_write_register(ViPipe, 0x43, 0x04);	//MTX_CH2_VSIZE
	pr2100_write_register(ViPipe, 0x44, 0x38);	//MTX_CH2_VSIZE
	pr2100_write_register(ViPipe, 0x45, 0x0f);	//CH2_FS_OS_H
	pr2100_write_register(ViPipe, 0x46, 0x00);	//CH2_FS_OS_L
	pr2100_write_register(ViPipe, 0x47, 0x0f);	//CH2_FE_OS_H
	pr2100_write_register(ViPipe, 0x48, 0x00);	//CH2_FE_OS_L
	pr2100_write_register(ViPipe, 0x49, 0x03);	//REF_CH2_VS_OS
	pr2100_write_register(ViPipe, 0x4a, 0xc0);	//REF_CH2_VS_OS
	pr2100_write_register(ViPipe, 0x50, 0xb8);	//MTX_CH3_CTRL
	pr2100_write_register(ViPipe, 0x51, 0x07);	//MTX_CH3_HSIZE
	pr2100_write_register(ViPipe, 0x52, 0x80);	//MTX_CH3_HSIZE
	pr2100_write_register(ViPipe, 0x53, 0x04);	//MTX_CH3_VSIZE
	pr2100_write_register(ViPipe, 0x54, 0x38);	//MTX_CH3_VSIZE
	pr2100_write_register(ViPipe, 0x55, 0x0f);	//CH3_FS_OS_H
	pr2100_write_register(ViPipe, 0x56, 0x00);	//CH3_FS_OS_L
	pr2100_write_register(ViPipe, 0x57, 0x0f);	//CH3_FE_OS_H
	pr2100_write_register(ViPipe, 0x58, 0x00);	//CH3_FE_OS_L
	pr2100_write_register(ViPipe, 0x59, 0x00);	//REF_CH3_VS_OS
	pr2100_write_register(ViPipe, 0x5a, 0x00);	//REF_CH3_VS_OS
	pr2100_write_register(ViPipe, 0x60, 0x05);	//CH0_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x61, 0x28);	//CH0_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x62, 0x05);	//CH1_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x63, 0x28);	//CH1_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x64, 0x05);	//CH2_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x65, 0x28);	//CH2_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x66, 0x05);	//CH3_HWIDTH[13:8]
	pr2100_write_register(ViPipe, 0x67, 0x28);	//CH3_HWIDTH [7:0]
	pr2100_write_register(ViPipe, 0x68, 0xff);	//CH0_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x69, 0xff);	//CH0_VRATE[7:0]
	pr2100_write_register(ViPipe, 0x6a, 0xff);	//CH1_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x6b, 0xff);	//CH1_VRATE[7:0]
	pr2100_write_register(ViPipe, 0x6c, 0xff);	//CH2_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x6d, 0xff);	//CH2_VRATE[7:0]
	pr2100_write_register(ViPipe, 0x6e, 0xff);	//CH3_VRATE[15:8]
	pr2100_write_register(ViPipe, 0x6f, 0xff);	//CH3_VRATE[7:0]

	pr2100_write_register(ViPipe, 0xFF, 0x06);
	pr2100_write_register(ViPipe, 0x04, 0x10);	//MIPI_CONTROL_0
	pr2100_write_register(ViPipe, 0x05, 0x04);	//MIPI_CONTROL_1
	pr2100_write_register(ViPipe, 0x06, 0x00);	//MIPI_CONTROL_2
	pr2100_write_register(ViPipe, 0x07, 0x00);	//MIPI_CONTROL_3
	pr2100_write_register(ViPipe, 0x08, 0xc9);	//MIPI_CONTROL_4
	pr2100_write_register(ViPipe, 0x1c, 0x09);	//MIPI_T_LPX
	pr2100_write_register(ViPipe, 0x1d, 0x08);	//MIPI_T_CLK_PREPARE
	pr2100_write_register(ViPipe, 0x1e, 0x09);	//MIPI_T_HS_PREPARE
	pr2100_write_register(ViPipe, 0x1f, 0x11);	//MIPI_T_HS_ZERO
	pr2100_write_register(ViPipe, 0x20, 0x0c);	//MIPI_T_HS_TRAIL
	pr2100_write_register(ViPipe, 0x21, 0x28);	//MIPI_T_CLK_ZERO
	pr2100_write_register(ViPipe, 0x22, 0x0b);	//MIPI_T_CLK_TRAIL
	pr2100_write_register(ViPipe, 0x23, 0x01);	//MIPI_T_CLK_PRE
	pr2100_write_register(ViPipe, 0x24, 0x12);	//MIPI_T_CLK_POST
	pr2100_write_register(ViPipe, 0x25, 0x82);	//MIPI_T_WAKEUP
	pr2100_write_register(ViPipe, 0x26, 0x11);	//MIPI_T_HSEXIT
	pr2100_write_register(ViPipe, 0x27, 0x11);	//MIPI_T_CLK_HSEXIT
	pr2100_write_register(ViPipe, 0x36, 0x0f);	//MIPI_PKT_SIZE0_H
	pr2100_write_register(ViPipe, 0x37, 0x00);	//MIPI_PKT_SIZE0_L
	pr2100_write_register(ViPipe, 0x38, 0x0f);	//MIPI_PKT_SIZE1_H
	pr2100_write_register(ViPipe, 0x39, 0x00);	//MIPI_PKT_SIZE1_L
	pr2100_write_register(ViPipe, 0x3a, 0x0f);	//MIPI_PKT_SIZE2_H
	pr2100_write_register(ViPipe, 0x3b, 0x00);	//MIPI_PKT_SIZE2_L
	pr2100_write_register(ViPipe, 0x3c, 0x0f);	//MIPI_PKT_SIZE3_H
	pr2100_write_register(ViPipe, 0x3d, 0x00);	//MIPI_PKT_SIZE3_L
	pr2100_write_register(ViPipe, 0x46, 0x1e);	//MIPI_DATA_ID0
	pr2100_write_register(ViPipe, 0x47, 0x5e);	//MIPI_DATA_ID1
	pr2100_write_register(ViPipe, 0x48, 0x9e);	//MIPI_DATA_ID2
	pr2100_write_register(ViPipe, 0x49, 0xde);	//MIPI_DATA_ID3
	pr2100_write_register(ViPipe, 0x04, 0x50);	//MIPI_CONTROL_0

	pr2100_set_1080p_4ch_slave(slave_pipe);
}
