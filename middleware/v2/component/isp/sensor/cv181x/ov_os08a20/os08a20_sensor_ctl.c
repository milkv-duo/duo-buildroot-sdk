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
#include "os08a20_cmos_ex.h"

static void os08a20_wdr_1944p30_2to1_init(VI_PIPE ViPipe);
static void os08a20_linear_1944p30_init(VI_PIPE ViPipe);

const CVI_U8 os08a20_i2c_addr = 0x36;        /* I2C Address of OS08A20 */
const CVI_U32 os08a20_addr_byte = 2;
const CVI_U32 os08a20_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int os08a20_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunOs08a20_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, os08a20_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int os08a20_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int os08a20_read_register(VI_PIPE ViPipe, int addr)
{
	/* TODO:*/
	(void) ViPipe;
	(void) addr;

	return CVI_SUCCESS;
}


int os08a20_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (os08a20_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (os08a20_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, os08a20_addr_byte + os08a20_data_byte);
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

void os08a20_standby(VI_PIPE ViPipe)
{
	os08a20_write_register(ViPipe, 0x0100, 0x00); /* STANDBY */
}

void os08a20_restart(VI_PIPE ViPipe)
{
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
}

void os08a20_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	/* by pass the group hold and clear registers since it won't update when stream is off. */
	for (i = 3; i < g_pastOs08a20[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum - 3; i++) {
		os08a20_write_register(ViPipe,
				g_pastOs08a20[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOs08a20[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

void os08a20_init(VI_PIPE ViPipe)
{
	WDR_MODE_E        enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastOs08a20[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastOs08a20[ViPipe]->u8ImgMode;

	os08a20_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == OS08A20_MODE_2592X1944P30_WDR)
			os08a20_wdr_1944p30_2to1_init(ViPipe);
		else {
		}
	} else {
		if (u8ImgMode == OS08A20_MODE_2592X1944P30)
			os08a20_linear_1944p30_init(ViPipe);
		else {
		}
	}
	g_pastOs08a20[ViPipe]->bInit = CVI_TRUE;
}

void os08a20_exit(VI_PIPE ViPipe)
{
	os08a20_i2c_exit(ViPipe);
}

/* 1944P30 and 1944P25 */
static void os08a20_linear_1944p30_init(VI_PIPE ViPipe)
{
	os08a20_write_register(ViPipe, 0x0100, 0x00);
	os08a20_write_register(ViPipe, 0x0103, 0x01);
	os08a20_write_register(ViPipe, 0x0303, 0x01);
	os08a20_write_register(ViPipe, 0x0305, 0x5a);
	os08a20_write_register(ViPipe, 0x0306, 0x00);
	os08a20_write_register(ViPipe, 0x0308, 0x03);
	os08a20_write_register(ViPipe, 0x0309, 0x04);
	os08a20_write_register(ViPipe, 0x032a, 0x00);
	os08a20_write_register(ViPipe, 0x300f, 0x11);
	os08a20_write_register(ViPipe, 0x3010, 0x01);
	os08a20_write_register(ViPipe, 0x3011, 0x04);
	os08a20_write_register(ViPipe, 0x3012, 0x41);
	os08a20_write_register(ViPipe, 0x3016, 0xf0);
	os08a20_write_register(ViPipe, 0x301e, 0x98);
	os08a20_write_register(ViPipe, 0x3031, 0xa9);
	os08a20_write_register(ViPipe, 0x3103, 0x92);
	os08a20_write_register(ViPipe, 0x3104, 0x01);
	os08a20_write_register(ViPipe, 0x3106, 0x10);
	os08a20_write_register(ViPipe, 0x3400, 0x04);
	os08a20_write_register(ViPipe, 0x3025, 0x03);
	os08a20_write_register(ViPipe, 0x3425, 0x01);
	os08a20_write_register(ViPipe, 0x3428, 0x01);
	os08a20_write_register(ViPipe, 0x3406, 0x08);
	os08a20_write_register(ViPipe, 0x3408, 0x03);
	os08a20_write_register(ViPipe, 0x340c, 0xff);
	os08a20_write_register(ViPipe, 0x340d, 0xff);
	os08a20_write_register(ViPipe, 0x031e, 0x09);
	os08a20_write_register(ViPipe, 0x3501, 0x08);
	os08a20_write_register(ViPipe, 0x3502, 0xe5);
	os08a20_write_register(ViPipe, 0x3505, 0x83);
	os08a20_write_register(ViPipe, 0x3508, 0x00);
	os08a20_write_register(ViPipe, 0x3509, 0x80);
	os08a20_write_register(ViPipe, 0x350a, 0x04);
	os08a20_write_register(ViPipe, 0x350b, 0x00);
	os08a20_write_register(ViPipe, 0x350c, 0x00);
	os08a20_write_register(ViPipe, 0x350d, 0x80);
	os08a20_write_register(ViPipe, 0x350e, 0x04);
	os08a20_write_register(ViPipe, 0x350f, 0x00);
	os08a20_write_register(ViPipe, 0x3600, 0x00);
	os08a20_write_register(ViPipe, 0x3603, 0x2c);
	os08a20_write_register(ViPipe, 0x3605, 0x50);
	os08a20_write_register(ViPipe, 0x3609, 0xb5);
	os08a20_write_register(ViPipe, 0x3610, 0x39);
	os08a20_write_register(ViPipe, 0x360c, 0x01);
	os08a20_write_register(ViPipe, 0x3628, 0xa4);
	os08a20_write_register(ViPipe, 0x362d, 0x10);
	os08a20_write_register(ViPipe, 0x3660, 0x43);
	os08a20_write_register(ViPipe, 0x3661, 0x06);
	os08a20_write_register(ViPipe, 0x3662, 0x00);
	os08a20_write_register(ViPipe, 0x3663, 0x28);
	os08a20_write_register(ViPipe, 0x3664, 0x0d);
	os08a20_write_register(ViPipe, 0x366a, 0x38);
	os08a20_write_register(ViPipe, 0x366b, 0xa0);
	os08a20_write_register(ViPipe, 0x366d, 0x00);
	os08a20_write_register(ViPipe, 0x366e, 0x00);
	os08a20_write_register(ViPipe, 0x3680, 0x00);
	os08a20_write_register(ViPipe, 0x36c0, 0x00);
	os08a20_write_register(ViPipe, 0x3701, 0x02);
	os08a20_write_register(ViPipe, 0x373b, 0x02);
	os08a20_write_register(ViPipe, 0x373c, 0x02);
	os08a20_write_register(ViPipe, 0x3736, 0x02);
	os08a20_write_register(ViPipe, 0x3737, 0x02);
	os08a20_write_register(ViPipe, 0x3705, 0x00);
	os08a20_write_register(ViPipe, 0x3706, 0x39);
	os08a20_write_register(ViPipe, 0x370a, 0x00);
	os08a20_write_register(ViPipe, 0x370b, 0x98);
	os08a20_write_register(ViPipe, 0x3709, 0x49);
	os08a20_write_register(ViPipe, 0x3714, 0x21);
	os08a20_write_register(ViPipe, 0x371c, 0x00);
	os08a20_write_register(ViPipe, 0x371d, 0x08);
	os08a20_write_register(ViPipe, 0x3740, 0x1b);
	os08a20_write_register(ViPipe, 0x3741, 0x04);
	os08a20_write_register(ViPipe, 0x375e, 0x0b);
	os08a20_write_register(ViPipe, 0x3760, 0x10);
	os08a20_write_register(ViPipe, 0x3776, 0x10);
	os08a20_write_register(ViPipe, 0x3781, 0x02);
	os08a20_write_register(ViPipe, 0x3782, 0x04);
	os08a20_write_register(ViPipe, 0x3783, 0x02);
	os08a20_write_register(ViPipe, 0x3784, 0x08);
	os08a20_write_register(ViPipe, 0x3785, 0x08);
	os08a20_write_register(ViPipe, 0x3788, 0x01);
	os08a20_write_register(ViPipe, 0x3789, 0x01);
	os08a20_write_register(ViPipe, 0x3797, 0x04);
	os08a20_write_register(ViPipe, 0x3762, 0x11);
	os08a20_write_register(ViPipe, 0x3800, 0x00);
	os08a20_write_register(ViPipe, 0x3801, 0x00);
	os08a20_write_register(ViPipe, 0x3802, 0x00);
	os08a20_write_register(ViPipe, 0x3803, 0x0c);
	os08a20_write_register(ViPipe, 0x3804, 0x0e);
	os08a20_write_register(ViPipe, 0x3805, 0xff);
	os08a20_write_register(ViPipe, 0x3806, 0x08);
	os08a20_write_register(ViPipe, 0x3807, 0x6f);
	os08a20_write_register(ViPipe, 0x3808, 0x0f);
	os08a20_write_register(ViPipe, 0x3809, 0x00);
	os08a20_write_register(ViPipe, 0x380a, 0x08);
	os08a20_write_register(ViPipe, 0x380b, 0x70);
	os08a20_write_register(ViPipe, 0x380c, 0x04);
	os08a20_write_register(ViPipe, 0x380d, 0x0c);
	os08a20_write_register(ViPipe, 0x380e, 0x09);
	os08a20_write_register(ViPipe, 0x380f, 0x0a);
	os08a20_write_register(ViPipe, 0x3813, 0x10);
	os08a20_write_register(ViPipe, 0x3814, 0x01);
	os08a20_write_register(ViPipe, 0x3815, 0x01);
	os08a20_write_register(ViPipe, 0x3816, 0x01);
	os08a20_write_register(ViPipe, 0x3817, 0x01);
	os08a20_write_register(ViPipe, 0x381c, 0x00);
	os08a20_write_register(ViPipe, 0x3820, 0x00);
	os08a20_write_register(ViPipe, 0x3821, 0x04);
	os08a20_write_register(ViPipe, 0x3823, 0x08);
	os08a20_write_register(ViPipe, 0x3826, 0x00);
	os08a20_write_register(ViPipe, 0x3827, 0x08);
	os08a20_write_register(ViPipe, 0x382d, 0x08);
	os08a20_write_register(ViPipe, 0x3832, 0x02);
	os08a20_write_register(ViPipe, 0x3833, 0x00);
	os08a20_write_register(ViPipe, 0x383c, 0x48);
	os08a20_write_register(ViPipe, 0x383d, 0xff);
	os08a20_write_register(ViPipe, 0x3d85, 0x0b);
	os08a20_write_register(ViPipe, 0x3d84, 0x40);
	os08a20_write_register(ViPipe, 0x3d8c, 0x63);
	os08a20_write_register(ViPipe, 0x3d8d, 0xd7);
	os08a20_write_register(ViPipe, 0x4000, 0xf8);
	os08a20_write_register(ViPipe, 0x4001, 0x2b);
	os08a20_write_register(ViPipe, 0x4004, 0x00);
	os08a20_write_register(ViPipe, 0x4005, 0x40);
	os08a20_write_register(ViPipe, 0x400a, 0x01);
	os08a20_write_register(ViPipe, 0x400f, 0xa0);
	os08a20_write_register(ViPipe, 0x4010, 0x12);
	os08a20_write_register(ViPipe, 0x4018, 0x00);
	os08a20_write_register(ViPipe, 0x4008, 0x02);
	os08a20_write_register(ViPipe, 0x4009, 0x0d);
	os08a20_write_register(ViPipe, 0x401a, 0x58);
	os08a20_write_register(ViPipe, 0x4050, 0x00);
	os08a20_write_register(ViPipe, 0x4051, 0x01);
	os08a20_write_register(ViPipe, 0x4028, 0x2f);
	os08a20_write_register(ViPipe, 0x4052, 0x00);
	os08a20_write_register(ViPipe, 0x4053, 0x80);
	os08a20_write_register(ViPipe, 0x4054, 0x00);
	os08a20_write_register(ViPipe, 0x4055, 0x80);
	os08a20_write_register(ViPipe, 0x4056, 0x00);
	os08a20_write_register(ViPipe, 0x4057, 0x80);
	os08a20_write_register(ViPipe, 0x4058, 0x00);
	os08a20_write_register(ViPipe, 0x4059, 0x80);
	os08a20_write_register(ViPipe, 0x430b, 0xff);
	os08a20_write_register(ViPipe, 0x430c, 0xff);
	os08a20_write_register(ViPipe, 0x430d, 0x00);
	os08a20_write_register(ViPipe, 0x430e, 0x00);
	os08a20_write_register(ViPipe, 0x4501, 0x18);
	os08a20_write_register(ViPipe, 0x4502, 0x00);
	os08a20_write_register(ViPipe, 0x4643, 0x00);
	os08a20_write_register(ViPipe, 0x4640, 0x01);
	os08a20_write_register(ViPipe, 0x4641, 0x04);
	os08a20_write_register(ViPipe, 0x4800, 0x64);
	os08a20_write_register(ViPipe, 0x4809, 0x2b);
	os08a20_write_register(ViPipe, 0x4813, 0x90);
	os08a20_write_register(ViPipe, 0x4817, 0x04);
	os08a20_write_register(ViPipe, 0x4833, 0x18);
	os08a20_write_register(ViPipe, 0x4837, 0x0b);
	os08a20_write_register(ViPipe, 0x483b, 0x00);
	os08a20_write_register(ViPipe, 0x484b, 0x03);
	os08a20_write_register(ViPipe, 0x4850, 0x7c);
	os08a20_write_register(ViPipe, 0x4852, 0x06);
	os08a20_write_register(ViPipe, 0x4856, 0x58);
	os08a20_write_register(ViPipe, 0x4857, 0xaa);
	os08a20_write_register(ViPipe, 0x4862, 0x0a);
	os08a20_write_register(ViPipe, 0x4869, 0x18);
	os08a20_write_register(ViPipe, 0x486a, 0xaa);
	os08a20_write_register(ViPipe, 0x486e, 0x03);
	os08a20_write_register(ViPipe, 0x486f, 0x55);
	os08a20_write_register(ViPipe, 0x4875, 0xf0);
	os08a20_write_register(ViPipe, 0x5000, 0x89);
	os08a20_write_register(ViPipe, 0x5001, 0x42);
	os08a20_write_register(ViPipe, 0x5004, 0x40);
	os08a20_write_register(ViPipe, 0x5005, 0x00);
	os08a20_write_register(ViPipe, 0x5180, 0x00);
	os08a20_write_register(ViPipe, 0x5181, 0x10);
	os08a20_write_register(ViPipe, 0x580b, 0x03);
	os08a20_write_register(ViPipe, 0x4d00, 0x03);
	os08a20_write_register(ViPipe, 0x4d01, 0xc9);
	os08a20_write_register(ViPipe, 0x4d02, 0xbc);
	os08a20_write_register(ViPipe, 0x4d03, 0xc6);
	os08a20_write_register(ViPipe, 0x4d04, 0x4a);
	os08a20_write_register(ViPipe, 0x4d05, 0x25);
	os08a20_write_register(ViPipe, 0x4700, 0x2b);
	os08a20_write_register(ViPipe, 0x4e00, 0x2b);
	os08a20_write_register(ViPipe, 0x3501, 0x09);
	os08a20_write_register(ViPipe, 0x3502, 0x01);
	os08a20_write_register(ViPipe, 0x0305, 0x57);
	os08a20_write_register(ViPipe, 0x380c, 0x08);
	os08a20_write_register(ViPipe, 0x380d, 0x18);
	os08a20_write_register(ViPipe, 0x380e, 0x09);
	os08a20_write_register(ViPipe, 0x380f, 0x0a);
	os08a20_write_register(ViPipe, 0x3501, 0x09);
	os08a20_write_register(ViPipe, 0x3502, 0x02);
	os08a20_write_register(ViPipe, 0x3808, 0x0a);
	os08a20_write_register(ViPipe, 0x3809, 0x20);
	os08a20_write_register(ViPipe, 0x380a, 0x07);
	os08a20_write_register(ViPipe, 0x380b, 0x98);

	os08a20_default_reg_init(ViPipe);

	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */

	delay_ms(50);

	printf("ViPipe:%d,===OS08A20 1944P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

static void os08a20_wdr_1944p30_2to1_init(VI_PIPE ViPipe)
{
	os08a20_write_register(ViPipe, 0x0100, 0x00);
	os08a20_write_register(ViPipe, 0x0103, 0x01);
	os08a20_write_register(ViPipe, 0x0303, 0x01);
	os08a20_write_register(ViPipe, 0x0305, 0x5a);
	os08a20_write_register(ViPipe, 0x0306, 0x00);
	os08a20_write_register(ViPipe, 0x0308, 0x03);
	os08a20_write_register(ViPipe, 0x0309, 0x04);
	os08a20_write_register(ViPipe, 0x032a, 0x00);
	os08a20_write_register(ViPipe, 0x300f, 0x11);
	os08a20_write_register(ViPipe, 0x3010, 0x01);
	os08a20_write_register(ViPipe, 0x3011, 0x04);
	os08a20_write_register(ViPipe, 0x3012, 0x41);
	os08a20_write_register(ViPipe, 0x3016, 0xf0);
	os08a20_write_register(ViPipe, 0x301e, 0x98);
	os08a20_write_register(ViPipe, 0x3031, 0xa9);
	os08a20_write_register(ViPipe, 0x3103, 0x92);
	os08a20_write_register(ViPipe, 0x3104, 0x01);
	os08a20_write_register(ViPipe, 0x3106, 0x10);
	os08a20_write_register(ViPipe, 0x340c, 0xff);
	os08a20_write_register(ViPipe, 0x340d, 0xff);
	os08a20_write_register(ViPipe, 0x031e, 0x09);
	os08a20_write_register(ViPipe, 0x3501, 0x08);
	os08a20_write_register(ViPipe, 0x3502, 0xe5);
	os08a20_write_register(ViPipe, 0x3505, 0x83);
	os08a20_write_register(ViPipe, 0x3508, 0x00);
	os08a20_write_register(ViPipe, 0x3509, 0x80);
	os08a20_write_register(ViPipe, 0x350a, 0x04);
	os08a20_write_register(ViPipe, 0x350b, 0x00);
	os08a20_write_register(ViPipe, 0x350c, 0x00);
	os08a20_write_register(ViPipe, 0x350d, 0x80);
	os08a20_write_register(ViPipe, 0x350e, 0x04);
	os08a20_write_register(ViPipe, 0x350f, 0x00);
	os08a20_write_register(ViPipe, 0x3600, 0x00);
	os08a20_write_register(ViPipe, 0x3603, 0x2c);
	os08a20_write_register(ViPipe, 0x3605, 0x50);
	os08a20_write_register(ViPipe, 0x3609, 0xb5);
	os08a20_write_register(ViPipe, 0x3610, 0x39);
	os08a20_write_register(ViPipe, 0x360c, 0x01);
	os08a20_write_register(ViPipe, 0x3628, 0xa4);
	os08a20_write_register(ViPipe, 0x362d, 0x10);
	os08a20_write_register(ViPipe, 0x3660, 0x42);
	os08a20_write_register(ViPipe, 0x3661, 0x07);
	os08a20_write_register(ViPipe, 0x3662, 0x00);
	os08a20_write_register(ViPipe, 0x3663, 0x28);
	os08a20_write_register(ViPipe, 0x3664, 0x0d);
	os08a20_write_register(ViPipe, 0x366a, 0x38);
	os08a20_write_register(ViPipe, 0x366b, 0xa0);
	os08a20_write_register(ViPipe, 0x366d, 0x00);
	os08a20_write_register(ViPipe, 0x366e, 0x00);
	os08a20_write_register(ViPipe, 0x3680, 0x00);
	os08a20_write_register(ViPipe, 0x36c0, 0x00);
	os08a20_write_register(ViPipe, 0x3701, 0x02);
	os08a20_write_register(ViPipe, 0x373b, 0x02);
	os08a20_write_register(ViPipe, 0x373c, 0x02);
	os08a20_write_register(ViPipe, 0x3736, 0x02);
	os08a20_write_register(ViPipe, 0x3737, 0x02);
	os08a20_write_register(ViPipe, 0x3705, 0x00);
	os08a20_write_register(ViPipe, 0x3706, 0x39);
	os08a20_write_register(ViPipe, 0x370a, 0x00);
	os08a20_write_register(ViPipe, 0x370b, 0x98);
	os08a20_write_register(ViPipe, 0x3709, 0x49);
	os08a20_write_register(ViPipe, 0x3714, 0x21);
	os08a20_write_register(ViPipe, 0x371c, 0x00);
	os08a20_write_register(ViPipe, 0x371d, 0x08);
	os08a20_write_register(ViPipe, 0x3740, 0x1b);
	os08a20_write_register(ViPipe, 0x3741, 0x04);
	os08a20_write_register(ViPipe, 0x375e, 0x0b);
	os08a20_write_register(ViPipe, 0x3760, 0x10);
	os08a20_write_register(ViPipe, 0x3776, 0x10);
	os08a20_write_register(ViPipe, 0x3781, 0x02);
	os08a20_write_register(ViPipe, 0x3782, 0x04);
	os08a20_write_register(ViPipe, 0x3783, 0x02);
	os08a20_write_register(ViPipe, 0x3784, 0x08);
	os08a20_write_register(ViPipe, 0x3785, 0x08);
	os08a20_write_register(ViPipe, 0x3788, 0x01);
	os08a20_write_register(ViPipe, 0x3789, 0x01);
	os08a20_write_register(ViPipe, 0x3797, 0x04);
	os08a20_write_register(ViPipe, 0x3762, 0x11);
	os08a20_write_register(ViPipe, 0x3800, 0x00);
	os08a20_write_register(ViPipe, 0x3801, 0x00);
	os08a20_write_register(ViPipe, 0x3802, 0x00);
	os08a20_write_register(ViPipe, 0x3803, 0x0c);
	os08a20_write_register(ViPipe, 0x3804, 0x0e);
	os08a20_write_register(ViPipe, 0x3805, 0xff);
	os08a20_write_register(ViPipe, 0x3806, 0x08);
	os08a20_write_register(ViPipe, 0x3807, 0x6f);
	os08a20_write_register(ViPipe, 0x3808, 0x0a);
	os08a20_write_register(ViPipe, 0x3809, 0x20);
	os08a20_write_register(ViPipe, 0x380a, 0x07);
	os08a20_write_register(ViPipe, 0x380b, 0x98);
	os08a20_write_register(ViPipe, 0x380c, 0x04);
	os08a20_write_register(ViPipe, 0x380d, 0x0c);
	os08a20_write_register(ViPipe, 0x380e, 0x09);
	os08a20_write_register(ViPipe, 0x380f, 0x0a);
	os08a20_write_register(ViPipe, 0x3813, 0x10);
	os08a20_write_register(ViPipe, 0x3814, 0x01);
	os08a20_write_register(ViPipe, 0x3815, 0x01);
	os08a20_write_register(ViPipe, 0x3816, 0x01);
	os08a20_write_register(ViPipe, 0x3817, 0x01);
	os08a20_write_register(ViPipe, 0x381c, 0x08);
	os08a20_write_register(ViPipe, 0x3820, 0x00);
	os08a20_write_register(ViPipe, 0x3821, 0x24);
	os08a20_write_register(ViPipe, 0x3823, 0x08);
	os08a20_write_register(ViPipe, 0x3826, 0x00);
	os08a20_write_register(ViPipe, 0x3827, 0x08);
	os08a20_write_register(ViPipe, 0x382d, 0x08);
	os08a20_write_register(ViPipe, 0x3832, 0x02);
	os08a20_write_register(ViPipe, 0x3833, 0x01);
	os08a20_write_register(ViPipe, 0x383c, 0x48);
	os08a20_write_register(ViPipe, 0x383d, 0xff);
	os08a20_write_register(ViPipe, 0x3d85, 0x0b);
	os08a20_write_register(ViPipe, 0x3d84, 0x40);
	os08a20_write_register(ViPipe, 0x3d8c, 0x63);
	os08a20_write_register(ViPipe, 0x3d8d, 0xd7);
	os08a20_write_register(ViPipe, 0x4000, 0xf8);
	os08a20_write_register(ViPipe, 0x4001, 0x2b);
	os08a20_write_register(ViPipe, 0x4004, 0x00);
	os08a20_write_register(ViPipe, 0x4005, 0x40);
	os08a20_write_register(ViPipe, 0x400a, 0x01);
	os08a20_write_register(ViPipe, 0x400f, 0xa0);
	os08a20_write_register(ViPipe, 0x4010, 0x12);
	os08a20_write_register(ViPipe, 0x4018, 0x00);
	os08a20_write_register(ViPipe, 0x4008, 0x02);
	os08a20_write_register(ViPipe, 0x4009, 0x0d);
	os08a20_write_register(ViPipe, 0x401a, 0x58);
	os08a20_write_register(ViPipe, 0x4050, 0x00);
	os08a20_write_register(ViPipe, 0x4051, 0x01);
	os08a20_write_register(ViPipe, 0x4028, 0x2f);
	os08a20_write_register(ViPipe, 0x4052, 0x00);
	os08a20_write_register(ViPipe, 0x4053, 0x80);
	os08a20_write_register(ViPipe, 0x4054, 0x00);
	os08a20_write_register(ViPipe, 0x4055, 0x80);
	os08a20_write_register(ViPipe, 0x4056, 0x00);
	os08a20_write_register(ViPipe, 0x4057, 0x80);
	os08a20_write_register(ViPipe, 0x4058, 0x00);
	os08a20_write_register(ViPipe, 0x4059, 0x80);
	os08a20_write_register(ViPipe, 0x430b, 0xff);
	os08a20_write_register(ViPipe, 0x430c, 0xff);
	os08a20_write_register(ViPipe, 0x430d, 0x00);
	os08a20_write_register(ViPipe, 0x430e, 0x00);
	os08a20_write_register(ViPipe, 0x4501, 0x18);
	os08a20_write_register(ViPipe, 0x4502, 0x00);
	os08a20_write_register(ViPipe, 0x4643, 0x00);
	os08a20_write_register(ViPipe, 0x4640, 0x01);
	os08a20_write_register(ViPipe, 0x4641, 0x04);
	os08a20_write_register(ViPipe, 0x4800, 0x64);
	os08a20_write_register(ViPipe, 0x4809, 0x2b);
	os08a20_write_register(ViPipe, 0x4813, 0x98);
	os08a20_write_register(ViPipe, 0x4817, 0x04);
	os08a20_write_register(ViPipe, 0x4833, 0x18);
	os08a20_write_register(ViPipe, 0x4837, 0x0b);
	os08a20_write_register(ViPipe, 0x483b, 0x00);
	os08a20_write_register(ViPipe, 0x484b, 0x03);
	os08a20_write_register(ViPipe, 0x4850, 0x7c);
	os08a20_write_register(ViPipe, 0x4852, 0x06);
	os08a20_write_register(ViPipe, 0x4856, 0x58);
	os08a20_write_register(ViPipe, 0x4857, 0xaa);
	os08a20_write_register(ViPipe, 0x4862, 0x0a);
	os08a20_write_register(ViPipe, 0x4869, 0x18);
	os08a20_write_register(ViPipe, 0x486a, 0xaa);
	os08a20_write_register(ViPipe, 0x486e, 0x07);
	os08a20_write_register(ViPipe, 0x486f, 0x55);
	os08a20_write_register(ViPipe, 0x4875, 0xf0);
	os08a20_write_register(ViPipe, 0x5000, 0x89);
	os08a20_write_register(ViPipe, 0x5001, 0x42);
	os08a20_write_register(ViPipe, 0x5004, 0x40);
	os08a20_write_register(ViPipe, 0x5005, 0x00);
	os08a20_write_register(ViPipe, 0x5180, 0x00);
	os08a20_write_register(ViPipe, 0x5181, 0x10);
	os08a20_write_register(ViPipe, 0x580b, 0x03);
	os08a20_write_register(ViPipe, 0x4d00, 0x03);
	os08a20_write_register(ViPipe, 0x4d01, 0xc9);
	os08a20_write_register(ViPipe, 0x4d02, 0xbc);
	os08a20_write_register(ViPipe, 0x4d03, 0xc6);
	os08a20_write_register(ViPipe, 0x4d04, 0x4a);
	os08a20_write_register(ViPipe, 0x4d05, 0x25);
	os08a20_write_register(ViPipe, 0x4700, 0x2b);
	os08a20_write_register(ViPipe, 0x4e00, 0x2b);
	os08a20_write_register(ViPipe, 0x3501, 0x08);
	os08a20_write_register(ViPipe, 0x3502, 0xe1);
	os08a20_write_register(ViPipe, 0x3511, 0x00);
	os08a20_write_register(ViPipe, 0x3512, 0x20);
	os08a20_write_register(ViPipe, 0x3833, 0x01);
	os08a20_write_register(ViPipe, 0x0305, 0x57);
	os08a20_write_register(ViPipe, 0x0325, 0x46);
	os08a20_write_register(ViPipe, 0x380e, 0x09);
	os08a20_write_register(ViPipe, 0x380f, 0x2a);
	os08a20_write_register(ViPipe, 0x4028, 0x4f);
	os08a20_write_register(ViPipe, 0x4029, 0x1f);
	os08a20_write_register(ViPipe, 0x402a, 0x7f);
	os08a20_write_register(ViPipe, 0x402b, 0x01);

	os08a20_default_reg_init(ViPipe);

	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */
	os08a20_write_register(ViPipe, 0x0100, 0x01); /* standby */

	delay_ms(50);

	printf("===Os08a20 sensor 1944P30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}

