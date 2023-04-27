#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "sample_comm.h"
#include "sample_vio_tp2863_tp2803.h"
#include "cvi_sys.h"
#include <linux/cvi_type.h>

const CVI_U8 tp2803_master_i2c_addr = 0x45;        /* I2C slave address of tp2803 master chip*/
const CVI_U32 tp2803_addr_byte = 1;
const CVI_U32 tp2803_data_byte = 1;
static int i2c_fd = -1;

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

static int tp2803_GPIO_Export(unsigned int gpio)
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

static int tp2803_GPIO_SetDirection(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/direction", gpio);
	if (access(buf, 0) == -1)
		tp2803_GPIO_Export(gpio);

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

static int tp2803_GPIO_SetValue(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);
	if (access(buf, 0) == -1)
		tp2803_GPIO_Export(gpio);

	tp2803_GPIO_SetDirection(gpio, 1); //output

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

int tp2803_i2c_init(VI_PIPE ViPipe, CVI_U8 i2c_addr)
{
	UNUSED(ViPipe);
	int ret;
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum = 1;

	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);
	printf("open %s\n", acDevFile);

	i2c_fd = open(acDevFile, O_RDWR, 0600);
	if (i2c_fd < 0) {
		printf("Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(i2c_fd, I2C_SLAVE_FORCE, i2c_addr);
	if (ret < 0) {
		printf("I2C_SLAVE_FORCE error!\n");
		close(i2c_fd);
		i2c_fd = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int tp2803_read_register(VI_PIPE ViPipe, int addr)
{
	UNUSED(ViPipe);
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (tp2803_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(i2c_fd, buf, tp2803_addr_byte);
	if (ret < 0) {
		printf("I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(i2c_fd, buf, tp2803_data_byte);
	if (ret < 0) {
		printf("I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (tp2803_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	printf("i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int tp2803_write_register(VI_PIPE ViPipe, int addr, int data)
{
	UNUSED(ViPipe);
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (tp2803_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (tp2803_data_byte == 2)
		buf[idx++] = (data >> 8) & 0xff;

	// add data byte 0
	buf[idx++] = data & 0xff;

	ret = write(i2c_fd, buf, tp2803_addr_byte + tp2803_data_byte);
	if (ret < 0) {
		printf("I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	printf("i2c w 0x%x 0x%x\n", addr, data);

#if 0 // read back checing
	ret = tp2803_read_register(ViPipe, addr);
	if (ret != data)
		printf("i2c readback-check fail, 0x%x != 0x%x\n", ret, data);
#endif
	return CVI_SUCCESS;
}

int tp2803_gpio_init(VI_PIPE ViPipe)
{
	(void) ViPipe;

	//VO_RESET
	if (tp2803_GPIO_SetValue(CVI_GPIOB_13, 0) != 0) {
		printf("set power down gpio error!\n");
		return CVI_FAILURE;
	}

	usleep(100 * 1000);

	if (tp2803_GPIO_SetValue(CVI_GPIOB_13, 1) != 0) {
		printf("set power down gpio error!\n");
		return CVI_FAILURE;
	}

	usleep(100 * 1000);

	return CVI_SUCCESS;
}

void tp2803_reg_init()
{
	//AHD 720P 25FPS
	tp2803_write_register(0, 0xFF, 0x00); //Page 00
	tp2803_write_register(0, 0x00, 0x00);
	tp2803_write_register(0, 0x01, 0x00);
	tp2803_write_register(0, 0x02, 0x9B);
	tp2803_write_register(0, 0x03, 0x62);
	tp2803_write_register(0, 0x04, 0x00);
	tp2803_write_register(0, 0x05, 0x6C);
	tp2803_write_register(0, 0x06, 0x00);
	tp2803_write_register(0, 0x07, 0xC1);
	tp2803_write_register(0, 0x08, 0x76);
	tp2803_write_register(0, 0x09, 0x76);
	tp2803_write_register(0, 0x0A, 0x76);
	tp2803_write_register(0, 0x0B, 0x15);
	tp2803_write_register(0, 0x0C, 0x04);
	tp2803_write_register(0, 0x0D, 0xF0);
	tp2803_write_register(0, 0x0E, 0x82);
	tp2803_write_register(0, 0x0F, 0x80);
	tp2803_write_register(0, 0x10, 0x17);
	tp2803_write_register(0, 0x11, 0x80);
	tp2803_write_register(0, 0x12, 0x01);
	tp2803_write_register(0, 0x13, 0x3C);
	tp2803_write_register(0, 0x14, 0x38);
	tp2803_write_register(0, 0x15, 0x39);
	tp2803_write_register(0, 0x16, 0xEB);
	tp2803_write_register(0, 0x17, 0x10);
	tp2803_write_register(0, 0x18, 0x10);
	tp2803_write_register(0, 0x19, 0xF0);
	tp2803_write_register(0, 0x1A, 0x10);
	tp2803_write_register(0, 0x1B, 0xA4);
	tp2803_write_register(0, 0x1C, 0x55);
	tp2803_write_register(0, 0x1D, 0x76);
	tp2803_write_register(0, 0x1E, 0x80);
	tp2803_write_register(0, 0x1F, 0x00);
	tp2803_write_register(0, 0x20, 0x28);
	tp2803_write_register(0, 0x21, 0xC4);
	tp2803_write_register(0, 0x22, 0x44);
	tp2803_write_register(0, 0x23, 0x44);
	tp2803_write_register(0, 0x24, 0x86);
	tp2803_write_register(0, 0x25, 0x40);
	tp2803_write_register(0, 0x26, 0x00);
	tp2803_write_register(0, 0x27, 0x56);
	tp2803_write_register(0, 0x28, 0x00);
	tp2803_write_register(0, 0x29, 0x34);
	tp2803_write_register(0, 0x2A, 0x19);
	tp2803_write_register(0, 0x2B, 0x5e);
	tp2803_write_register(0, 0x2C, 0x60);
	tp2803_write_register(0, 0x2D, 0x00);
	tp2803_write_register(0, 0x2E, 0x00);
	tp2803_write_register(0, 0x2F, 0x00);
	tp2803_write_register(0, 0x30, 0x01);
	tp2803_write_register(0, 0x31, 0x00);
	tp2803_write_register(0, 0x32, 0x00);
	tp2803_write_register(0, 0x33, 0x00);
	tp2803_write_register(0, 0x34, 0x00);
	tp2803_write_register(0, 0x35, 0x00);
	tp2803_write_register(0, 0x36, 0x00);
	tp2803_write_register(0, 0x37, 0x00);
	tp2803_write_register(0, 0x38, 0x00);
	tp2803_write_register(0, 0x39, 0x00);
	tp2803_write_register(0, 0x3A, 0x00);
	tp2803_write_register(0, 0x3B, 0x90);
	tp2803_write_register(0, 0x3C, 0x10);
	tp2803_write_register(0, 0x3D, 0x80);
	tp2803_write_register(0, 0x3E, 0x18);
	tp2803_write_register(0, 0x3F, 0x00);
	tp2803_write_register(0, 0x45, 0x41);
	tp2803_write_register(0, 0xF0, 0x00);
	tp2803_write_register(0, 0xF1, 0x08);
	tp2803_write_register(0, 0xF2, 0x1e);
	tp2803_write_register(0, 0xF3, 0x4a);
	tp2803_write_register(0, 0xF4, 0x41);
	tp2803_write_register(0, 0xF5, 0x00);

	//AHD 720P 30FPS
	// tp2803_write_register(0, 0xFF, 0x00);
	// tp2803_write_register(0, 0x00, 0x00);
	// tp2803_write_register(0, 0x01, 0x00);
	// tp2803_write_register(0, 0x02, 0xCB);
	// tp2803_write_register(0, 0x03, 0x62);
	// tp2803_write_register(0, 0x04, 0x00);
	// tp2803_write_register(0, 0x05, 0x6C);
	// tp2803_write_register(0, 0x06, 0x00);
	// tp2803_write_register(0, 0x07, 0xC1);
	// tp2803_write_register(0, 0x08, 0x76);
	// tp2803_write_register(0, 0x09, 0x76);
	// tp2803_write_register(0, 0x0A, 0x76);
	// tp2803_write_register(0, 0x0B, 0x00);
	// tp2803_write_register(0, 0x0C, 0x04);
	// tp2803_write_register(0, 0x0D, 0xF0);
	// tp2803_write_register(0, 0x0E, 0x82);
	// tp2803_write_register(0, 0x0F, 0x40);
	// tp2803_write_register(0, 0x10, 0x06);
	// tp2803_write_register(0, 0x11, 0x3E);
	// tp2803_write_register(0, 0x12, 0x00);
	// tp2803_write_register(0, 0x13, 0x3C);
	// tp2803_write_register(0, 0x14, 0x38);
	// tp2803_write_register(0, 0x15, 0x39);
	// tp2803_write_register(0, 0x16, 0xEB);
	// tp2803_write_register(0, 0x17, 0x10);
	// tp2803_write_register(0, 0x18, 0x10);
	// tp2803_write_register(0, 0x19, 0xF0);
	// tp2803_write_register(0, 0x1A, 0x10);
	// tp2803_write_register(0, 0x1B, 0xA4);
	// tp2803_write_register(0, 0x1C, 0x55);
	// tp2803_write_register(0, 0x1D, 0x76);
	// tp2803_write_register(0, 0x1E, 0x80);
	// tp2803_write_register(0, 0x1F, 0x00);
	// tp2803_write_register(0, 0x20, 0x28);
	// tp2803_write_register(0, 0x21, 0xAE);
	// tp2803_write_register(0, 0x22, 0x14);
	// tp2803_write_register(0, 0x23, 0x7A);
	// tp2803_write_register(0, 0x24, 0x86);
	// tp2803_write_register(0, 0x25, 0x40);
	// tp2803_write_register(0, 0x26, 0x00);
	// tp2803_write_register(0, 0x27, 0x56);
	// tp2803_write_register(0, 0x28, 0x00);
	// tp2803_write_register(0, 0x29, 0x34);
	// tp2803_write_register(0, 0x2A, 0x19);
	// tp2803_write_register(0, 0x2B, 0x5e);
	// tp2803_write_register(0, 0x2C, 0x60);
	// tp2803_write_register(0, 0x2D, 0x00);
	// tp2803_write_register(0, 0x2E, 0x00);
	// tp2803_write_register(0, 0x2F, 0x00);
	// tp2803_write_register(0, 0x30, 0x01);
	// tp2803_write_register(0, 0x31, 0x00);
	// tp2803_write_register(0, 0x32, 0x00);
	// tp2803_write_register(0, 0x33, 0x00);
	// tp2803_write_register(0, 0x34, 0x00);
	// tp2803_write_register(0, 0x35, 0x00);
	// tp2803_write_register(0, 0x36, 0x00);
	// tp2803_write_register(0, 0x37, 0x00);
	// tp2803_write_register(0, 0x38, 0x00);
	// tp2803_write_register(0, 0x39, 0x00);
	// tp2803_write_register(0, 0x3A, 0x00);
	// tp2803_write_register(0, 0x3B, 0xD0);
	// tp2803_write_register(0, 0x3C, 0x10);
	// tp2803_write_register(0, 0x3D, 0x80);
	// tp2803_write_register(0, 0x3E, 0x18);
	// tp2803_write_register(0, 0x3F, 0x00);
	// tp2803_write_register(0, 0x45, 0x40);
	// tp2803_write_register(0, 0xF0, 0x00);
	// tp2803_write_register(0, 0xF1, 0x08);
	// tp2803_write_register(0, 0xF2, 0x1e);
	// tp2803_write_register(0, 0xF3, 0x4a);
	// tp2803_write_register(0, 0xF4, 0x41);
	// tp2803_write_register(0, 0xF5, 0x00);
}

int main(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);

	CVI_S32 s32Ret = CVI_FAILURE;

	signal(SIGINT, SAMPLE_VIO_HandleSig);
	signal(SIGTERM, SAMPLE_VIO_HandleSig);

	s32Ret = SAMPLE_VIO();
	if (s32Ret == CVI_SUCCESS){
		SAMPLE_PRT("sample_vio exit success!\n");
	}else{
		SAMPLE_PRT("sample_vio exit abnormally!\n");
		return s32Ret;
	}

	//output tp2803 mclk
	SNS_MCLK_S mclk;
	mclk.u8Cam = 0;
	mclk.enFreq = MCLK_FREQ_27M;
	CVI_MIPI_SetSnsMclk(&mclk);

	//gpio reset
	tp2803_gpio_init(0);

	//i2c init
	tp2803_i2c_init(0, tp2803_master_i2c_addr);

	// check sensor chip id
	if (tp2803_read_register(0, 0xfe) != 0x29 ||
		tp2803_read_register(0, 0xff) != 0x12)
	{
		printf("read tp2803 chip id fail\n");
		return CVI_FAILURE;
	}

	//tp2803 mode init
	tp2803_reg_init();

	while(1)
	{
		usleep(1000 * 1000);
	}

	return s32Ret;
}