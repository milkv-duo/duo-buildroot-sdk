#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "sample_comm.h"

/*gpio*/
enum CVI_GPIO_NUM_E {
CVI_GPIOE_00 = 352,
CVI_GPIOE_01,   CVI_GPIOE_02,   CVI_GPIOE_03,   CVI_GPIOE_04,   CVI_GPIOE_05,
CVI_GPIOE_06,   CVI_GPIOE_07,   CVI_GPIOE_08,   CVI_GPIOE_09,   CVI_GPIOE_10,
CVI_GPIOE_11,   CVI_GPIOE_12,   CVI_GPIOE_13,   CVI_GPIOE_14,   CVI_GPIOE_15,
CVI_GPIOE_16,   CVI_GPIOE_17,   CVI_GPIOE_18,   CVI_GPIOE_19,   CVI_GPIOE_20,
CVI_GPIOE_21,   CVI_GPIOE_22,   CVI_GPIOE_23,   CVI_GPIOE_24,   CVI_GPIOE_25,
CVI_GPIOE_26,   CVI_GPIOE_27,   CVI_GPIOE_28,   CVI_GPIOE_29,   CVI_GPIOE_30,
CVI_GPIOE_31,
CVI_GPIOD_00 = 384,
CVI_GPIOD_01,   CVI_GPIOD_02,   CVI_GPIOD_03,   CVI_GPIOD_04,   CVI_GPIOD_05,
CVI_GPIOD_06,   CVI_GPIOD_07,   CVI_GPIOD_08,   CVI_GPIOD_09,   CVI_GPIOD_10,
CVI_GPIOD_11,   CVI_GPIOD_12,   CVI_GPIOD_13,   CVI_GPIOD_14,   CVI_GPIOD_15,
CVI_GPIOD_16,   CVI_GPIOD_17,   CVI_GPIOD_18,   CVI_GPIOD_19,   CVI_GPIOD_20,
CVI_GPIOD_21,   CVI_GPIOD_22,   CVI_GPIOD_23,   CVI_GPIOD_24,   CVI_GPIOD_25,
CVI_GPIOD_26,   CVI_GPIOD_27,   CVI_GPIOD_28,   CVI_GPIOD_29,   CVI_GPIOD_30,
CVI_GPIOD_31,
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

static int SAMPLE_COMM_GPIO_Export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int SAMPLE_COMM_GPIO_Unexport(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);

	write(fd, buf, len);
	close(fd);
	return 0;
}

static int SAMPLE_COMM_GPIO_SetDirection(unsigned int gpio, unsigned int out_flag)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/direction", gpio);
	if (access(buf, 0) == -1)
		SAMPLE_COMM_GPIO_Export(gpio);

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

int SAMPLE_COMM_GPIO_SetValue(unsigned int gpio, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR"/gpio%d/value", gpio);
	if (access(buf, 0) == -1)
		SAMPLE_COMM_GPIO_Export(gpio);

	SAMPLE_COMM_GPIO_SetDirection(gpio, 1); //output

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

int SAMPLE_COMM_GPIO_GetValue(unsigned int gpio, unsigned int *value)
{
	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);

	if (access(buf, 0) == -1)
		SAMPLE_COMM_GPIO_Export(gpio);

	SAMPLE_COMM_GPIO_SetDirection(gpio, 0); //input

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/get-value");
		return fd;
	}

	read(fd, &ch, 1);

	if (ch != '0')
		*value = 1;
	else
		*value = 0;

	close(fd);
	return 0;
}

/*pwm*/
#define SYSFS_PWM_DIR "/sys/class/pwm/pwmchip0/"
int SAMPLE_COMM_PWM_SetParm(int chn, int period, int duty_cycle)
{
	int fd;
	char buf[MAX_BUF], buf1[MAX_BUF];

	if (!((chn >= 0) && (chn <= 3))) {
		printf("pwm chanel 0 ~ 3\n");
		return -1;
	}

	snprintf(buf, sizeof(buf), SYSFS_PWM_DIR"/pwm%d/period", chn);
	if (access(buf, 0) == -1) {
		fd = open(SYSFS_PWM_DIR"/export", O_WRONLY);
		if (fd < 0) {
			printf("open export error\n");
			return -1;
		}
		if (chn == 0)
			write(fd, "0", strlen("0"));
		else if (chn == 1)
			write(fd, "1", strlen("1"));
		else if (chn == 2)
			write(fd, "2", strlen("2"));
		else if (chn == 3)
			write(fd, "3", strlen("3"));

		close(fd);
	}

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("open period error\n");
		return -1;
	}
	snprintf(buf1, sizeof(buf1), "%d", period);
	write(fd, buf1, sizeof(buf1));
	close(fd);

	snprintf(buf, sizeof(buf), SYSFS_PWM_DIR"/pwm%d/duty_cycle", chn);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("open duty_cycle error\n");
		return -1;
	}
	snprintf(buf1, sizeof(buf1), "%d", duty_cycle);
	write(fd, buf1, sizeof(buf1));
	close(fd);
	return 0;
}
int SAMPLE_COMM_PWM_Enable(int chn, int en)
{
	int fd;
	char buf[MAX_BUF];

	if (!((chn >= 0) && (chn <= 3))) {
		printf("pwm chanel 0 ~ 3\n");
		return -1;
	}
	snprintf(buf, sizeof(buf), SYSFS_PWM_DIR"/pwm%d/enable", chn);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("open period error\n");
		return -1;
	}

	if (en)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);

	close(fd);
	return 0;
}

/*i2c*/
CVI_S32 SAMPLE_COMM_I2C_Write(CVI_S32 file, CVI_U16 addr, CVI_U16 reg, CVI_U16 val, CVI_U16 reg_w, CVI_U16 val_w)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];
	int ret;
	unsigned char temp[4];

	messages[0].addr = addr;
	messages[0].flags = 0;
	messages[0].len = reg_w + val_w;

	switch (reg_w) {
	case 1:
		temp[0] = reg;
		switch (val_w) {
		case 1:
			temp[1] = val;
			break;
		case 2:
			temp[1] = val >> 8;
			temp[2] = val;
			break;
		default:
			printf("No support of this value width\n");
		}
		break;
	case 2:
		temp[0] = reg >> 8;
		temp[1] = reg;
		switch (val_w) {
		case 1:
			temp[2] = val;
			break;
		case 2:
			temp[2] = val >> 8;
			temp[3] = val;
			break;
		default:
			printf("No support of this value width\n");
		}
		break;
	default:
		printf("No support of this register width\n");
		return CVI_FAILURE;
	}

	messages[0].buf = temp;
	/* Send the request to the kernel and get the result back */
	packets.msgs = messages;
	packets.nmsgs = 1;
	ret = ioctl(file, I2C_RDWR, &packets);

	if (ret < 0) {
		perror("Unable to send data");
		return ret;
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_I2C_Read(CVI_S32 file, CVI_U16 addr, CVI_U16 reg, CVI_U16 reg_w, CVI_U8 *r_val)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];
	int ret;
	unsigned char temp[2];

	messages[0].addr = addr;
	messages[0].flags = 0;
	messages[0].len = reg_w;

	switch (reg_w) {
	case 1:
		temp[0] = reg;
		break;
	case 2:
		temp[0] = reg >> 8;
		temp[1] = reg;
		break;
	default:
		printf("No support of this register width\n");
		return CVI_FAILURE;
	}

	messages[0].buf = temp;
	/* The data will get returned in this structure */
	messages[1].addr = addr;
	/* | I2C_M_NOSTART */
	messages[1].flags = I2C_M_RD;
	messages[1].len = 1;
	messages[1].buf = r_val;

	/* Send the request to the kernel and get the result back */
	packets.msgs = messages;
	packets.nmsgs = 2;
	ret = ioctl(file, I2C_RDWR, &packets);

	if (ret < 0) {
		perror("Unable to send data");
		return ret;
	}

	//printf("get val=%x\n", *r_val);

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_COMM_I2C_Open(CVI_CHAR *i2c_bus)
{
	//eg. i2c_bus = "dev/i2c-2"
	return open(i2c_bus, O_RDWR);
}

CVI_S32 SAMPLE_COMM_I2C_Close(CVI_S32 i2c_file)
{
	return close(i2c_file);
}
