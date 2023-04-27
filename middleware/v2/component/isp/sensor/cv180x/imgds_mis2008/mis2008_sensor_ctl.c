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
#include "mis2008_cmos_ex.h"

static void mis2008_linear_1080p30_init(VI_PIPE ViPipe);

CVI_U8 mis2008_i2c_addr = 0x30;        /* I2C Address of MIS2008 */
const CVI_U32 mis2008_addr_byte = 2;
const CVI_U32 mis2008_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int mis2008_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunMIS2008_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, mis2008_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int mis2008_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int mis2008_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (mis2008_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, mis2008_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, mis2008_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (mis2008_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int mis2008_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (mis2008_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (mis2008_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, mis2008_addr_byte + mis2008_data_byte);
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

void mis2008_standby(VI_PIPE ViPipe)
{
	mis2008_write_register(ViPipe, 0x3006, 0x02);
}

void mis2008_restart(VI_PIPE ViPipe)
{
	mis2008_write_register(ViPipe, 0x3006, 0x01);
}

void mis2008_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastMIS2008[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		mis2008_write_register(ViPipe,
				g_pastMIS2008[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastMIS2008[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define MIS2008_CHIP_ID_HI_ADDR		0x3000
#define MIS2008_CHIP_ID_LO_ADDR		0x3001
#define MIS2008_CHIP_ID			0x2008

int mis2008_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	if (mis2008_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	delay_ms(5);

	nVal = mis2008_read_register(ViPipe, MIS2008_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = mis2008_read_register(ViPipe, MIS2008_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != MIS2008_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void mis2008_init(VI_PIPE ViPipe)
{
	mis2008_i2c_init(ViPipe);

	mis2008_linear_1080p30_init(ViPipe);

	g_pastMIS2008[ViPipe]->bInit = CVI_TRUE;
}

void mis2008_exit(VI_PIPE ViPipe)
{
	mis2008_i2c_exit(ViPipe);
}

/* 1080p30 */
static void mis2008_linear_1080p30_init(VI_PIPE ViPipe)
{
	/* [ParaList] */
	mis2008_write_register(ViPipe, 0x300a ,0x01);
	mis2008_write_register(ViPipe, 0x3006 ,0x02);
	mis2008_write_register(ViPipe, 0x3201 ,0x65);
	mis2008_write_register(ViPipe, 0x3200 ,0x04);
	mis2008_write_register(ViPipe, 0x3203 ,0x98); 
	mis2008_write_register(ViPipe, 0x3202 ,0x08);

	mis2008_write_register(ViPipe, 0x3205 ,0x04);
	mis2008_write_register(ViPipe, 0x3204 ,0x00);
	mis2008_write_register(ViPipe, 0x3207 ,0x43);
	mis2008_write_register(ViPipe, 0x3206 ,0x04);

	mis2008_write_register(ViPipe, 0x3209 ,0x04);
	mis2008_write_register(ViPipe, 0x3208 ,0x00);
	mis2008_write_register(ViPipe, 0x320b ,0x8b);
	mis2008_write_register(ViPipe, 0x320a ,0x07);

	//mis2008_write_register(ViPipe, 0x3007 ,0x00);
	mis2008_write_register(ViPipe, 0x3007 ,0x00);
	mis2008_write_register(ViPipe, 0x3300 ,0x21);
	mis2008_write_register(ViPipe, 0x3301 ,0x00);
	mis2008_write_register(ViPipe, 0x3302 ,0x02);
	mis2008_write_register(ViPipe, 0x3303 ,0x06);
	mis2008_write_register(ViPipe, 0x330d ,0x00);
	mis2008_write_register(ViPipe, 0x330b ,0x01);
	mis2008_write_register(ViPipe, 0x330f ,0x0f);
	mis2008_write_register(ViPipe, 0x3013 ,0x00);
	mis2008_write_register(ViPipe, 0x3637 ,0x1e);
	mis2008_write_register(ViPipe, 0x3900 ,0x07);
	mis2008_write_register(ViPipe, 0x2107 ,0x00);
	mis2008_write_register(ViPipe, 0x330e ,0x00);

	mis2008_write_register(ViPipe, 0x3900 ,0x07);
	mis2008_write_register(ViPipe, 0x2108 ,0x01);
	mis2008_write_register(ViPipe, 0x3c40 ,0x8c);
	mis2008_write_register(ViPipe, 0x3b01 ,0x3f);
	mis2008_write_register(ViPipe, 0x3b03 ,0x3f);
	mis2008_write_register(ViPipe, 0x3902 ,0x01);
	mis2008_write_register(ViPipe, 0x3904 ,0x00);
	mis2008_write_register(ViPipe, 0x3903 ,0x00);
	mis2008_write_register(ViPipe, 0x3906 ,0x1e);
	mis2008_write_register(ViPipe, 0x3905 ,0x00);
	mis2008_write_register(ViPipe, 0x3908 ,0x71);
	mis2008_write_register(ViPipe, 0x3907 ,0x10);
	mis2008_write_register(ViPipe, 0x390a ,0xff);
	mis2008_write_register(ViPipe, 0x3909 ,0x1f);
	mis2008_write_register(ViPipe, 0x390c ,0x67);
	mis2008_write_register(ViPipe, 0x390b ,0x03);
	mis2008_write_register(ViPipe, 0x390e ,0x77);
	mis2008_write_register(ViPipe, 0x390d ,0x00);
	mis2008_write_register(ViPipe, 0x3910 ,0x71);
	mis2008_write_register(ViPipe, 0x390f ,0x10);
	mis2008_write_register(ViPipe, 0x3912 ,0xff);
	mis2008_write_register(ViPipe, 0x3911 ,0x1f);
	mis2008_write_register(ViPipe, 0x3919 ,0x00);
	mis2008_write_register(ViPipe, 0x3918 ,0x00);
	mis2008_write_register(ViPipe, 0x391b ,0x91);
	mis2008_write_register(ViPipe, 0x391a ,0x01);
	mis2008_write_register(ViPipe, 0x3983 ,0x5a);
	mis2008_write_register(ViPipe, 0x3982 ,0x00);
	mis2008_write_register(ViPipe, 0x3985 ,0x0f);
	mis2008_write_register(ViPipe, 0x3984 ,0x00);
	mis2008_write_register(ViPipe, 0x391d ,0x00);
	mis2008_write_register(ViPipe, 0x391c ,0x00);
	mis2008_write_register(ViPipe, 0x391f ,0x65);
	mis2008_write_register(ViPipe, 0x391e ,0x10);
	mis2008_write_register(ViPipe, 0x3921 ,0xff);
	mis2008_write_register(ViPipe, 0x3920 ,0x1f);
	mis2008_write_register(ViPipe, 0x3923 ,0xff);
	mis2008_write_register(ViPipe, 0x3922 ,0x1f);
	mis2008_write_register(ViPipe, 0x3932 ,0x00);
	mis2008_write_register(ViPipe, 0x3931 ,0x00);
	mis2008_write_register(ViPipe, 0x3934 ,0x65);
	mis2008_write_register(ViPipe, 0x3933 ,0x01);
	mis2008_write_register(ViPipe, 0x393f ,0x6c);
	mis2008_write_register(ViPipe, 0x393e ,0x00);
	mis2008_write_register(ViPipe, 0x3941 ,0x67);
	mis2008_write_register(ViPipe, 0x3940 ,0x00);
	mis2008_write_register(ViPipe, 0x3943 ,0x55);
	mis2008_write_register(ViPipe, 0x3942 ,0x01);
	mis2008_write_register(ViPipe, 0x3945 ,0xc2);
	mis2008_write_register(ViPipe, 0x3944 ,0x02);
	mis2008_write_register(ViPipe, 0x3925 ,0x95);
	mis2008_write_register(ViPipe, 0x3924 ,0x00);
	mis2008_write_register(ViPipe, 0x3927 ,0xe1);
	mis2008_write_register(ViPipe, 0x3926 ,0x02);
	mis2008_write_register(ViPipe, 0x3947 ,0x74);
	mis2008_write_register(ViPipe, 0x3946 ,0x01);
	mis2008_write_register(ViPipe, 0x3949 ,0xda);
	mis2008_write_register(ViPipe, 0x3948 ,0x0e);
	mis2008_write_register(ViPipe, 0x394b ,0x42);
	mis2008_write_register(ViPipe, 0x394a ,0x03);
	mis2008_write_register(ViPipe, 0x394d ,0xf2);
	mis2008_write_register(ViPipe, 0x394c ,0x01);
	mis2008_write_register(ViPipe, 0x3913 ,0x01);
	mis2008_write_register(ViPipe, 0x3915 ,0x0f);
	mis2008_write_register(ViPipe, 0x3914 ,0x00);
	mis2008_write_register(ViPipe, 0x3917 ,0x67);
	mis2008_write_register(ViPipe, 0x3916 ,0x03);
	mis2008_write_register(ViPipe, 0x392a ,0x1e);
	mis2008_write_register(ViPipe, 0x3929 ,0x00);
	mis2008_write_register(ViPipe, 0x392c ,0x0f);
	mis2008_write_register(ViPipe, 0x392b ,0x00);
	mis2008_write_register(ViPipe, 0x392e ,0x0f);
	mis2008_write_register(ViPipe, 0x392d ,0x00);
	mis2008_write_register(ViPipe, 0x3930 ,0x6e);
	mis2008_write_register(ViPipe, 0x392f ,0x03);
	mis2008_write_register(ViPipe, 0x397f ,0x00);
	mis2008_write_register(ViPipe, 0x397e ,0x00);
	mis2008_write_register(ViPipe, 0x3981 ,0x77);
	mis2008_write_register(ViPipe, 0x3980 ,0x00);
	mis2008_write_register(ViPipe, 0x395d ,0x80);
	mis2008_write_register(ViPipe, 0x395c ,0x10);
	mis2008_write_register(ViPipe, 0x3962 ,0x9e);
	mis2008_write_register(ViPipe, 0x3961 ,0x10);
	mis2008_write_register(ViPipe, 0x3977 ,0x22);
	mis2008_write_register(ViPipe, 0x3976 ,0x00);
	mis2008_write_register(ViPipe, 0x3978 ,0x00);
	mis2008_write_register(ViPipe, 0x3979 ,0x04);
	mis2008_write_register(ViPipe, 0x396d ,0xc2);
	mis2008_write_register(ViPipe, 0x396c ,0x02);
	mis2008_write_register(ViPipe, 0x396f ,0xc2);
	mis2008_write_register(ViPipe, 0x396e ,0x02);
	mis2008_write_register(ViPipe, 0x3971 ,0xc2);
	mis2008_write_register(ViPipe, 0x3970 ,0x02);
	mis2008_write_register(ViPipe, 0x3973 ,0xc2);
	mis2008_write_register(ViPipe, 0x3972 ,0x02);

	mis2008_write_register(ViPipe, 0x3900 ,0x01);
	mis2008_write_register(ViPipe, 0x3600 ,0x00);
	mis2008_write_register(ViPipe, 0x3707 ,0x00);
	mis2008_write_register(ViPipe, 0x3708 ,0x80);
	mis2008_write_register(ViPipe, 0x3709 ,0x00);
	mis2008_write_register(ViPipe, 0x370a ,0x80);
	mis2008_write_register(ViPipe, 0x370b ,0x00);
	mis2008_write_register(ViPipe, 0x370c ,0x80);
	mis2008_write_register(ViPipe, 0x370d ,0x00);
	mis2008_write_register(ViPipe, 0x370e ,0x80);
	mis2008_write_register(ViPipe, 0x3006 ,0x00);


	mis2008_write_register(ViPipe, 0x3012 ,0x01);
	mis2008_write_register(ViPipe, 0x3600 ,0x13);
	mis2008_write_register(ViPipe, 0x3601 ,0x02);
	mis2008_write_register(ViPipe, 0x360e ,0x00);
	mis2008_write_register(ViPipe, 0x360f ,0x00);
	mis2008_write_register(ViPipe, 0x3610 ,0x02);
	mis2008_write_register(ViPipe, 0x3707 ,0x00);
	mis2008_write_register(ViPipe, 0x3708 ,0x40);
	mis2008_write_register(ViPipe, 0x3709 ,0x00);
	mis2008_write_register(ViPipe, 0x370a ,0x40);
	mis2008_write_register(ViPipe, 0x370b ,0x00);
	mis2008_write_register(ViPipe, 0x370c ,0x40);
	mis2008_write_register(ViPipe, 0x370d ,0x00);
	mis2008_write_register(ViPipe, 0x370e ,0x40);
	mis2008_write_register(ViPipe, 0x3800 ,0x01);
	mis2008_write_register(ViPipe, 0x3a03 ,0x03);
	mis2008_write_register(ViPipe, 0x3a02 ,0x0b);
	mis2008_write_register(ViPipe, 0x3a08 ,0x34);
	mis2008_write_register(ViPipe, 0x3a1b ,0x54);
	mis2008_write_register(ViPipe, 0x3a1e ,0x80);
	mis2008_write_register(ViPipe, 0x3100 ,0x04);
	mis2008_write_register(ViPipe, 0x3101 ,0x64);
	mis2008_write_register(ViPipe, 0x3a1c ,0x10);
	mis2008_write_register(ViPipe, 0x3a0C ,0x04);
	mis2008_write_register(ViPipe, 0x3a0D ,0x12);
	mis2008_write_register(ViPipe, 0x3a0E ,0x15);
	mis2008_write_register(ViPipe, 0x3a0F ,0x18);
	mis2008_write_register(ViPipe, 0x3a10 ,0x20);
	mis2008_write_register(ViPipe, 0x3a11 ,0x3c);

	 mis2008_default_reg_init(ViPipe);


	delay_ms(100);

	printf("ViPipe:%d,===MIS2008 1080P 30fps 12bit LINE Init OK!===\n", ViPipe);
}

