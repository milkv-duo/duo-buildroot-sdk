#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#ifdef ARCH_CV182X
#include <linux/cvi_vip_snsr.h>
#include "cvi_comm_video.h"
#else
#include <linux/vi_snsr.h>
#include <linux/cvi_comm_video.h>
#endif
#include <pthread.h>
#include <signal.h>
#include "cvi_sns_ctrl.h"
#include "tp2825_cmos_ex.h"

const CVI_U8 tp2825_i2c_addr = 0x45;        /* I2C slave address of tp2825, SA0=0:0x44, SA0=1:0x45*/
const CVI_U32 tp2825_addr_byte = 1;
const CVI_U32 tp2825_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};
static pthread_t g_tp2825_thid;


#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#define tp2825_BLUE_SCREEN 0


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
static int TP2825_GPIO_Export(unsigned int gpio)
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

static int TP2825_GPIO_SetDirection(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/direction", gpio);
	if (access(buf, 0) == -1)
		TP2825_GPIO_Export(gpio);

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
static int TP2825_GPIO_SetValue(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);
	if (access(buf, 0) == -1)
		TP2825_GPIO_Export(gpio);

	TP2825_GPIO_SetDirection(gpio, 1); //output

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

int tp2825_sys_init(VI_PIPE ViPipe)
{
	(void) ViPipe;

#ifdef AHD_PWR_EN
	if (PR2020_GPIO_SetValue(CVI_GPIOA_00, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set power down gpio error!\n");
		return CVI_FAILURE;
	}
#endif
	//PR2K_RST
	if (TP2825_GPIO_SetValue(CVI_GPIOB_12, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set reset gpio error!\n");
		return CVI_FAILURE;
	}
#ifdef BACK_DET
	if (PR2020_GPIO_SetValue(CVI_GPIOD_01, 1) != 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "set back detect gpio error!\n");
		return CVI_FAILURE;
	}
#endif
	return CVI_SUCCESS;
}

int tp2825_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;
	int ret;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;

	u8DevNum = g_auntp2825_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
	syslog(LOG_DEBUG, "open %s\n", acDevFile);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, tp2825_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int tp2825_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int tp2825_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return 0;

	if (tp2825_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, tp2825_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, tp2825_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (tp2825_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}
	printf("i2c r 0x%x = 0x%x\n", addr, data);
	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int tp2825_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (tp2825_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (tp2825_data_byte == 2)
		buf[idx++] = (data >> 8) & 0xff;

	// add data byte 0
	buf[idx++] = data & 0xff;

	ret = write(g_fd[ViPipe], buf, tp2825_addr_byte + tp2825_data_byte);
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

void tp2825_init_setting(VI_PIPE ViPipe, CVI_U8 mode)
{
	//unsigned char tmp;
	tp2825_write_register(ViPipe, 0x02, 0x50);//Mpage
	tp2825_write_register(ViPipe, 0x05, 0x00);
	tp2825_write_register(ViPipe, 0x06, 0x32);
	tp2825_write_register(ViPipe, 0x07, 0xC0);
	tp2825_write_register(ViPipe, 0x08, 0x00);
	tp2825_write_register(ViPipe, 0x09, 0x24);
	tp2825_write_register(ViPipe, 0x0A, 0x48);
	tp2825_write_register(ViPipe, 0x0B, 0xC0);
	tp2825_write_register(ViPipe, 0x0C, 0x03);
	tp2825_write_register(ViPipe, 0x0D, 0x50);
	tp2825_write_register(ViPipe, 0x0E, 0x00);
	tp2825_write_register(ViPipe, 0x0F, 0x00);
	tp2825_write_register(ViPipe, 0x10, 0x00);
	tp2825_write_register(ViPipe, 0x11, 0x40);
	tp2825_write_register(ViPipe, 0x12, 0x60);
	tp2825_write_register(ViPipe, 0x13, 0x00);
	tp2825_write_register(ViPipe, 0x14, 0x00);
	tp2825_write_register(ViPipe, 0x15, 0x23);
	tp2825_write_register(ViPipe, 0x16, 0x1B);
	tp2825_write_register(ViPipe, 0x17, 0x00);
	tp2825_write_register(ViPipe, 0x18, 0x38);
	tp2825_write_register(ViPipe, 0x19, 0xA0);
	tp2825_write_register(ViPipe, 0x1A, 0x5A);
	tp2825_write_register(ViPipe, 0x1B, 0x00);
	//PAGE0
	if (mode == TP2825_MODE_1440P_30P) {
		tp2825_write_register(ViPipe, 0x1C, 0x0C);
		tp2825_write_register(ViPipe, 0x1D, 0xE2);
	} else if (mode == TP2825_MODE_1440P_25P) {
		tp2825_write_register(ViPipe, 0x1C, 0x0F);
		tp2825_write_register(ViPipe, 0x1D, 0x76);
	}
	tp2825_write_register(ViPipe, 0x1E, 0x80);
	tp2825_write_register(ViPipe, 0x1F, 0x80);
	tp2825_write_register(ViPipe, 0x20, 0x50);
	tp2825_write_register(ViPipe, 0x21, 0x84);
	tp2825_write_register(ViPipe, 0x22, 0x36);
	tp2825_write_register(ViPipe, 0x23, 0x3C);
	tp2825_write_register(ViPipe, 0x24, 0x04);
	tp2825_write_register(ViPipe, 0x25, 0xFF);
	tp2825_write_register(ViPipe, 0x26, 0x05);
	tp2825_write_register(ViPipe, 0x27, 0xAD);
	tp2825_write_register(ViPipe, 0x28, 0x00);
	tp2825_write_register(ViPipe, 0x29, 0x48);
	tp2825_write_register(ViPipe, 0x2A, 0x30);
	tp2825_write_register(ViPipe, 0x2B, 0x60);
	tp2825_write_register(ViPipe, 0x2C, 0x2A);
	tp2825_write_register(ViPipe, 0x2D, 0x58);
	tp2825_write_register(ViPipe, 0x2E, 0x70);
	tp2825_write_register(ViPipe, 0x2F, 0x00);
	tp2825_write_register(ViPipe, 0x30, 0x74);
	tp2825_write_register(ViPipe, 0x31, 0x58);
	tp2825_write_register(ViPipe, 0x32, 0x9F);
	tp2825_write_register(ViPipe, 0x33, 0x60);
	tp2825_write_register(ViPipe, 0x34, 0x00);
	tp2825_write_register(ViPipe, 0x35, 0x15);
	tp2825_write_register(ViPipe, 0x36, 0xDC);
	tp2825_write_register(ViPipe, 0x37, 0x00);
	tp2825_write_register(ViPipe, 0x38, 0x40);
	tp2825_write_register(ViPipe, 0x39, 0x48);
	tp2825_write_register(ViPipe, 0x3A, 0x12);
	tp2825_write_register(ViPipe, 0x3B, 0x26);
	tp2825_write_register(ViPipe, 0x3C, 0x00);
	tp2825_write_register(ViPipe, 0x3D, 0x60);
	tp2825_write_register(ViPipe, 0x3E, 0x00);
	tp2825_write_register(ViPipe, 0x3F, 0x00);
	tp2825_write_register(ViPipe, 0x40, 0x00);
	tp2825_write_register(ViPipe, 0x41, 0x00);
	tp2825_write_register(ViPipe, 0x42, 0x00);
	tp2825_write_register(ViPipe, 0x43, 0x00);
	tp2825_write_register(ViPipe, 0x44, 0x00);
	tp2825_write_register(ViPipe, 0x45, 0x00);
	tp2825_write_register(ViPipe, 0x46, 0x00);
	tp2825_write_register(ViPipe, 0x47, 0x00);
	tp2825_write_register(ViPipe, 0x48, 0x00);
	tp2825_write_register(ViPipe, 0x49, 0x00);
	tp2825_write_register(ViPipe, 0x4A, 0x00);
	tp2825_write_register(ViPipe, 0x4B, 0x00);
	tp2825_write_register(ViPipe, 0x4C, 0x43);
	tp2825_write_register(ViPipe, 0x4D, 0x00);
	tp2825_write_register(ViPipe, 0x4E, 0x0D);
	tp2825_write_register(ViPipe, 0x4F, 0x00);
	tp2825_write_register(ViPipe, 0xF0, 0x00);
	tp2825_write_register(ViPipe, 0xF1, 0x00);
	tp2825_write_register(ViPipe, 0xF2, 0x00);
	tp2825_write_register(ViPipe, 0xF3, 0x00);
	tp2825_write_register(ViPipe, 0xF4, 0x20);
	tp2825_write_register(ViPipe, 0xF5, 0x10);
	tp2825_write_register(ViPipe, 0xF6, 0x00);
	tp2825_write_register(ViPipe, 0xF7, 0x00);
	tp2825_write_register(ViPipe, 0xF8, 0x00);
	tp2825_write_register(ViPipe, 0xF9, 0x00);
	tp2825_write_register(ViPipe, 0xFA, 0x03);
	tp2825_write_register(ViPipe, 0xFB, 0x00);
	tp2825_write_register(ViPipe, 0xFC, 0x00);

	printf("ViPipe:%d,===tp28251440P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

void tp2825_init(VI_PIPE ViPipe)
{
	if (tp2825_sys_init(ViPipe) != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "TP2825 sys init fail\n");
		return;
	}

	delay_ms(20);

	if (tp2825_i2c_init(ViPipe) != CVI_SUCCESS) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "TP2825 i2c init fail\n");
		return;
	}

	syslog(LOG_DEBUG, "Loading Techpoint tp2825 sensor\n");

	// check sensor chip id
	tp2825_write_register(ViPipe, 0x40, 0x0);
	if (tp2825_read_register(ViPipe, 0xfe) != 0x28 ||
		tp2825_read_register(ViPipe, 0xff) != 0x25) {
		syslog(LOG_DEBUG, "read tp2825 chip id fail\n");
		return;
	}

	if (g_pasttp2825[ViPipe]->u8ImgMode == TP2825_MODE_1440P_30P)
		syslog(LOG_DEBUG, "Techpoint tp2825 1440 30FPS\n");
	else
		syslog(LOG_DEBUG, "Techpoint tp2825 1440 25FPS\n");

	tp2825_init_setting(ViPipe, g_pasttp2825[ViPipe]->u8ImgMode);
#if tp2825_BLUE_SCREEN
	tp2825_write_register(ViPipe, 0x40, 0x00);
	tp2825_write_register(ViPipe, 0x2A, 0x34);
#endif
}

void tp2825_exit(VI_PIPE ViPipe)
{
	if (g_tp2825_thid)
		pthread_kill(g_tp2825_thid, SIGQUIT);

	tp2825_i2c_exit(ViPipe);
}
