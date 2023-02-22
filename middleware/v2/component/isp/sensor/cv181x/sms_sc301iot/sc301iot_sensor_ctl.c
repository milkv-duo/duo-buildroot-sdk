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
#include "sc301iot_cmos_ex.h"

static void sc301iot_linear_1536p30_init(VI_PIPE ViPipe);

CVI_U8 sc301iot_i2c_addr = 0x30;        /* I2C Address of SC301IOT */
const CVI_U32 sc301iot_addr_byte = 2;
const CVI_U32 sc301iot_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc301iot_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC301IOT_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc301iot_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc301iot_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc301iot_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	char buf[8];
	int idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc301iot_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc301iot_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc301iot_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc301iot_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc301iot_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc301iot_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc301iot_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc301iot_addr_byte + sc301iot_data_byte);
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

void sc301iot_standby(VI_PIPE ViPipe)
{
	sc301iot_write_register(ViPipe, 0x0100, 0x00);
}

void sc301iot_restart(VI_PIPE ViPipe)
{
	sc301iot_write_register(ViPipe, 0x0103, 0x01);
	delay_ms(1);
}

void sc301iot_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC301IOT[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc301iot_write_register(ViPipe,
				g_pastSC301IOT[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC301IOT[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC301IOT_CHIP_ID_HI_ADDR		0x3107
#define SC301IOT_CHIP_ID_LO_ADDR		0x3108
#define SC301IOT_CHIP_ID			0xcc40

void sc301iot_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 val = 0;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		val |= 0x6;
		break;
	case ISP_SNS_FLIP:
		val |= 0x60;
		break;
	case ISP_SNS_MIRROR_FLIP:
		val |= 0x66;
		break;
	default:
		return;
	}

	sc301iot_write_register(ViPipe, 0x3221, val);
}

int sc301iot_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc301iot_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc301iot_read_register(ViPipe, SC301IOT_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc301iot_read_register(ViPipe, SC301IOT_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC301IOT_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc301iot_init(VI_PIPE ViPipe)
{
	sc301iot_i2c_init(ViPipe);

	//linear mode only
	sc301iot_linear_1536p30_init(ViPipe);

	g_pastSC301IOT[ViPipe]->bInit = CVI_TRUE;
}

void sc301iot_exit(VI_PIPE ViPipe)
{
	sc301iot_i2c_exit(ViPipe);
}

static void sc301iot_linear_1536p30_init(VI_PIPE ViPipe)
{
	sc301iot_write_register(ViPipe, 0x0103, 0x01);
	sc301iot_write_register(ViPipe, 0x0100, 0x00);
	sc301iot_write_register(ViPipe, 0x36e9, 0x80);
	sc301iot_write_register(ViPipe, 0x37f9, 0x80);
	sc301iot_write_register(ViPipe, 0x301c, 0x78);
	sc301iot_write_register(ViPipe, 0x301f, 0x01);
	sc301iot_write_register(ViPipe, 0x30b8, 0x44);
	sc301iot_write_register(ViPipe, 0x3208, 0x08);
	sc301iot_write_register(ViPipe, 0x3209, 0x00);
	sc301iot_write_register(ViPipe, 0x320a, 0x06);
	sc301iot_write_register(ViPipe, 0x320b, 0x00);
	sc301iot_write_register(ViPipe, 0x320c, 0x04);
	sc301iot_write_register(ViPipe, 0x320d, 0x65);
	sc301iot_write_register(ViPipe, 0x320e, 0x06);
	sc301iot_write_register(ViPipe, 0x320f, 0x40);
	sc301iot_write_register(ViPipe, 0x3214, 0x11);
	sc301iot_write_register(ViPipe, 0x3215, 0x11);
	sc301iot_write_register(ViPipe, 0x3223, 0xd0);
	sc301iot_write_register(ViPipe, 0x3231, 0x01);
	sc301iot_write_register(ViPipe, 0x3253, 0x0c);
	sc301iot_write_register(ViPipe, 0x3274, 0x09);
	sc301iot_write_register(ViPipe, 0x3301, 0x08);
	sc301iot_write_register(ViPipe, 0x3306, 0x58);
	sc301iot_write_register(ViPipe, 0x3308, 0x08);
	sc301iot_write_register(ViPipe, 0x330a, 0x00);
	sc301iot_write_register(ViPipe, 0x330b, 0xe0);
	sc301iot_write_register(ViPipe, 0x330e, 0x10);
	sc301iot_write_register(ViPipe, 0x3314, 0x14);
	sc301iot_write_register(ViPipe, 0x331e, 0x55);
	sc301iot_write_register(ViPipe, 0x331f, 0x7d);
	sc301iot_write_register(ViPipe, 0x3333, 0x10);
	sc301iot_write_register(ViPipe, 0x3334, 0x40);
	sc301iot_write_register(ViPipe, 0x335e, 0x06);
	sc301iot_write_register(ViPipe, 0x335f, 0x08);
	sc301iot_write_register(ViPipe, 0x3364, 0x5e);
	sc301iot_write_register(ViPipe, 0x337c, 0x02);
	sc301iot_write_register(ViPipe, 0x337d, 0x0a);
	sc301iot_write_register(ViPipe, 0x3390, 0x01);
	sc301iot_write_register(ViPipe, 0x3391, 0x03);
	sc301iot_write_register(ViPipe, 0x3392, 0x07);
	sc301iot_write_register(ViPipe, 0x3393, 0x08);
	sc301iot_write_register(ViPipe, 0x3394, 0x08);
	sc301iot_write_register(ViPipe, 0x3395, 0x08);
	sc301iot_write_register(ViPipe, 0x3396, 0x08);
	sc301iot_write_register(ViPipe, 0x3397, 0x09);
	sc301iot_write_register(ViPipe, 0x3398, 0x1f);
	sc301iot_write_register(ViPipe, 0x3399, 0x08);
	sc301iot_write_register(ViPipe, 0x339a, 0x0a);
	sc301iot_write_register(ViPipe, 0x339b, 0x40);
	sc301iot_write_register(ViPipe, 0x339c, 0x88);
	sc301iot_write_register(ViPipe, 0x33a2, 0x04);
	sc301iot_write_register(ViPipe, 0x33ad, 0x0c);
	sc301iot_write_register(ViPipe, 0x33b1, 0x80);
	sc301iot_write_register(ViPipe, 0x33b3, 0x30);
	sc301iot_write_register(ViPipe, 0x33f9, 0x68);
	sc301iot_write_register(ViPipe, 0x33fb, 0x80);
	sc301iot_write_register(ViPipe, 0x33fc, 0x48);
	sc301iot_write_register(ViPipe, 0x33fd, 0x5f);
	sc301iot_write_register(ViPipe, 0x349f, 0x03);
	sc301iot_write_register(ViPipe, 0x34a6, 0x48);
	sc301iot_write_register(ViPipe, 0x34a7, 0x5f);
	sc301iot_write_register(ViPipe, 0x34a8, 0x30);
	sc301iot_write_register(ViPipe, 0x34a9, 0x30);
	sc301iot_write_register(ViPipe, 0x34aa, 0x00);
	sc301iot_write_register(ViPipe, 0x34ab, 0xf0);
	sc301iot_write_register(ViPipe, 0x34ac, 0x01);
	sc301iot_write_register(ViPipe, 0x34ad, 0x08);
	sc301iot_write_register(ViPipe, 0x34f8, 0x5f);
	sc301iot_write_register(ViPipe, 0x34f9, 0x10);
	sc301iot_write_register(ViPipe, 0x3630, 0xf0);
	sc301iot_write_register(ViPipe, 0x3631, 0x85);
	sc301iot_write_register(ViPipe, 0x3632, 0x74);
	sc301iot_write_register(ViPipe, 0x3633, 0x22);
	sc301iot_write_register(ViPipe, 0x3637, 0x4d);
	sc301iot_write_register(ViPipe, 0x3638, 0xcb);
	sc301iot_write_register(ViPipe, 0x363a, 0x8b);
	sc301iot_write_register(ViPipe, 0x363c, 0x08);
	sc301iot_write_register(ViPipe, 0x3640, 0x00);
	sc301iot_write_register(ViPipe, 0x3641, 0x38);
	sc301iot_write_register(ViPipe, 0x3670, 0x4e);
	sc301iot_write_register(ViPipe, 0x3674, 0xc0);
	sc301iot_write_register(ViPipe, 0x3675, 0xb0);
	sc301iot_write_register(ViPipe, 0x3676, 0xa0);
	sc301iot_write_register(ViPipe, 0x3677, 0x83);
	sc301iot_write_register(ViPipe, 0x3678, 0x87);
	sc301iot_write_register(ViPipe, 0x3679, 0x8a);
	sc301iot_write_register(ViPipe, 0x367c, 0x49);
	sc301iot_write_register(ViPipe, 0x367d, 0x4f);
	sc301iot_write_register(ViPipe, 0x367e, 0x48);
	sc301iot_write_register(ViPipe, 0x367f, 0x4b);
	sc301iot_write_register(ViPipe, 0x3690, 0x33);
	sc301iot_write_register(ViPipe, 0x3691, 0x33);
	sc301iot_write_register(ViPipe, 0x3692, 0x44);
	sc301iot_write_register(ViPipe, 0x3699, 0x8a);
	sc301iot_write_register(ViPipe, 0x369a, 0xa1);
	sc301iot_write_register(ViPipe, 0x369b, 0xc2);
	sc301iot_write_register(ViPipe, 0x369c, 0x48);
	sc301iot_write_register(ViPipe, 0x369d, 0x4f);
	sc301iot_write_register(ViPipe, 0x36a2, 0x4b);
	sc301iot_write_register(ViPipe, 0x36a3, 0x4f);
	sc301iot_write_register(ViPipe, 0x370f, 0x01);
	sc301iot_write_register(ViPipe, 0x3714, 0x80);
	sc301iot_write_register(ViPipe, 0x3722, 0x09);
	sc301iot_write_register(ViPipe, 0x3724, 0x41);
	sc301iot_write_register(ViPipe, 0x3725, 0xc1);
	sc301iot_write_register(ViPipe, 0x3728, 0x00);
	sc301iot_write_register(ViPipe, 0x3771, 0x09);
	sc301iot_write_register(ViPipe, 0x3772, 0x05);
	sc301iot_write_register(ViPipe, 0x3773, 0x05);
	sc301iot_write_register(ViPipe, 0x377a, 0x48);
	sc301iot_write_register(ViPipe, 0x377b, 0x49);
	sc301iot_write_register(ViPipe, 0x3905, 0x8d);
	sc301iot_write_register(ViPipe, 0x391d, 0x08);
	sc301iot_write_register(ViPipe, 0x3922, 0x1a);
	sc301iot_write_register(ViPipe, 0x3926, 0x21);
	sc301iot_write_register(ViPipe, 0x3933, 0x80);
	sc301iot_write_register(ViPipe, 0x3934, 0x0d);
	sc301iot_write_register(ViPipe, 0x3937, 0x6a);
	sc301iot_write_register(ViPipe, 0x3939, 0x00);
	sc301iot_write_register(ViPipe, 0x393a, 0x0e);
	sc301iot_write_register(ViPipe, 0x39dc, 0x02);
	sc301iot_write_register(ViPipe, 0x3e00, 0x00);
	sc301iot_write_register(ViPipe, 0x3e01, 0x63);
	sc301iot_write_register(ViPipe, 0x3e02, 0x80);
	sc301iot_write_register(ViPipe, 0x3e03, 0x0b);
	sc301iot_write_register(ViPipe, 0x3e1b, 0x2a);
	sc301iot_write_register(ViPipe, 0x4407, 0x34);
	sc301iot_write_register(ViPipe, 0x440e, 0x02);
	sc301iot_write_register(ViPipe, 0x5001, 0x40);
	sc301iot_write_register(ViPipe, 0x5007, 0x80);
	sc301iot_write_register(ViPipe, 0x36e9, 0x24);
	sc301iot_write_register(ViPipe, 0x37f9, 0x24);

	sc301iot_default_reg_init(ViPipe);

	sc301iot_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC301IOT 1536P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
