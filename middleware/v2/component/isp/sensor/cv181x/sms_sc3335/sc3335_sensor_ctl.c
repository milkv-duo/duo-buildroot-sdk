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
#include "sc3335_cmos_ex.h"

#define SC3335_CHIP_ID_HI_ADDR		0x3107
#define SC3335_CHIP_ID_LO_ADDR		0x3108
#define SC3335_CHIP_ID			0xcc1a

static void sc3335_linear_1296P30_init(VI_PIPE ViPipe);

const CVI_U8 sc3335_i2c_addr = 0x30;        /* I2C Address of SC3335 */
const CVI_U32 sc3335_addr_byte = 2;
const CVI_U32 sc3335_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc3335_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC3335_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc3335_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc3335_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc3335_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc3335_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc3335_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc3335_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc3335_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc3335_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc3335_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc3335_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc3335_addr_byte + sc3335_data_byte);
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

void sc3335_standby(VI_PIPE ViPipe)
{
	sc3335_write_register(ViPipe, 0x0100, 0x00);
}

void sc3335_restart(VI_PIPE ViPipe)
{
	sc3335_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc3335_write_register(ViPipe, 0x0100, 0x01);
}

void sc3335_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC3335[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastSC3335[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			sc3335_write_register(ViPipe,
				g_pastSC3335[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC3335[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void sc3335_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc3335_write_register(ViPipe, 0x3221, val);
}


int sc3335_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	delay_ms(4);
	if (sc3335_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc3335_read_register(ViPipe, SC3335_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc3335_read_register(ViPipe, SC3335_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC3335_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void sc3335_init(VI_PIPE ViPipe)
{
	sc3335_i2c_init(ViPipe);

	//linear mode only
	sc3335_linear_1296P30_init(ViPipe);

	g_pastSC3335[ViPipe]->bInit = CVI_TRUE;
}

void sc3335_exit(VI_PIPE ViPipe)
{
	sc3335_i2c_exit(ViPipe);
}

/* 1296P30 and 1296P25 */
static void sc3335_linear_1296P30_init(VI_PIPE ViPipe)
{
	sc3335_write_register(ViPipe, 0x0103, 0x01);
	delay_ms(33);
	sc3335_write_register(ViPipe, 0x0100, 0x00);
	sc3335_write_register(ViPipe, 0x36e9, 0x80);
	sc3335_write_register(ViPipe, 0x36f9, 0x80);
	sc3335_write_register(ViPipe, 0x301f, 0x01);
	sc3335_write_register(ViPipe, 0x3253, 0x04);
	sc3335_write_register(ViPipe, 0x3301, 0x04);
	sc3335_write_register(ViPipe, 0x3302, 0x10);
	sc3335_write_register(ViPipe, 0x3304, 0x40);
	sc3335_write_register(ViPipe, 0x3306, 0x40);
	sc3335_write_register(ViPipe, 0x3309, 0x50);
	sc3335_write_register(ViPipe, 0x330b, 0xb6);
	sc3335_write_register(ViPipe, 0x330e, 0x29);
	sc3335_write_register(ViPipe, 0x3310, 0x06);
	sc3335_write_register(ViPipe, 0x3314, 0x96);
	sc3335_write_register(ViPipe, 0x331e, 0x39);
	sc3335_write_register(ViPipe, 0x331f, 0x49);
	sc3335_write_register(ViPipe, 0x3320, 0x09);
	sc3335_write_register(ViPipe, 0x3333, 0x10);
	sc3335_write_register(ViPipe, 0x334c, 0x01);
	sc3335_write_register(ViPipe, 0x3364, 0x17);
	sc3335_write_register(ViPipe, 0x3367, 0x01);
	sc3335_write_register(ViPipe, 0x3390, 0x04);
	sc3335_write_register(ViPipe, 0x3391, 0x08);
	sc3335_write_register(ViPipe, 0x3392, 0x38);
	sc3335_write_register(ViPipe, 0x3393, 0x05);
	sc3335_write_register(ViPipe, 0x3394, 0x09);
	sc3335_write_register(ViPipe, 0x3395, 0x16);
	sc3335_write_register(ViPipe, 0x33ac, 0x0c);
	sc3335_write_register(ViPipe, 0x33ae, 0x1c);
	sc3335_write_register(ViPipe, 0x3622, 0x16);
	sc3335_write_register(ViPipe, 0x3637, 0x22);
	sc3335_write_register(ViPipe, 0x363a, 0x1f);
	sc3335_write_register(ViPipe, 0x363c, 0x05);
	sc3335_write_register(ViPipe, 0x3670, 0x0e);
	sc3335_write_register(ViPipe, 0x3674, 0xb0);
	sc3335_write_register(ViPipe, 0x3675, 0x88);
	sc3335_write_register(ViPipe, 0x3676, 0x68);
	sc3335_write_register(ViPipe, 0x3677, 0x84);
	sc3335_write_register(ViPipe, 0x3678, 0x85);
	sc3335_write_register(ViPipe, 0x3679, 0x86);
	sc3335_write_register(ViPipe, 0x367c, 0x18);
	sc3335_write_register(ViPipe, 0x367d, 0x38);
	sc3335_write_register(ViPipe, 0x367e, 0x08);
	sc3335_write_register(ViPipe, 0x367f, 0x18);
	sc3335_write_register(ViPipe, 0x3690, 0x43);
	sc3335_write_register(ViPipe, 0x3691, 0x43);
	sc3335_write_register(ViPipe, 0x3692, 0x44);
	sc3335_write_register(ViPipe, 0x369c, 0x18);
	sc3335_write_register(ViPipe, 0x369d, 0x38);
	sc3335_write_register(ViPipe, 0x36ea, 0x3b);
	sc3335_write_register(ViPipe, 0x36eb, 0x0d);
	sc3335_write_register(ViPipe, 0x36ec, 0x1c);
	sc3335_write_register(ViPipe, 0x36ed, 0x24);
	sc3335_write_register(ViPipe, 0x36fa, 0x3b);
	sc3335_write_register(ViPipe, 0x36fb, 0x00);
	sc3335_write_register(ViPipe, 0x36fc, 0x10);
	sc3335_write_register(ViPipe, 0x36fd, 0x24);
	sc3335_write_register(ViPipe, 0x3908, 0x82);
	sc3335_write_register(ViPipe, 0x391f, 0x18);
	sc3335_write_register(ViPipe, 0x3e01, 0xa8);
	sc3335_write_register(ViPipe, 0x3e02, 0x20);
	sc3335_write_register(ViPipe, 0x3f09, 0x48);
	sc3335_write_register(ViPipe, 0x4505, 0x08);
	sc3335_write_register(ViPipe, 0x4509, 0x20);
	sc3335_write_register(ViPipe, 0x5799, 0x00);
	sc3335_write_register(ViPipe, 0x59e0, 0x60);
	sc3335_write_register(ViPipe, 0x59e1, 0x08);
	sc3335_write_register(ViPipe, 0x59e2, 0x3f);
	sc3335_write_register(ViPipe, 0x59e3, 0x18);
	sc3335_write_register(ViPipe, 0x59e4, 0x18);
	sc3335_write_register(ViPipe, 0x59e5, 0x3f);
	sc3335_write_register(ViPipe, 0x59e6, 0x06);
	sc3335_write_register(ViPipe, 0x59e7, 0x02);
	sc3335_write_register(ViPipe, 0x59e8, 0x38);
	sc3335_write_register(ViPipe, 0x59e9, 0x10);
	sc3335_write_register(ViPipe, 0x59ea, 0x0c);
	sc3335_write_register(ViPipe, 0x59eb, 0x10);
	sc3335_write_register(ViPipe, 0x59ec, 0x04);
	sc3335_write_register(ViPipe, 0x59ed, 0x02);
	sc3335_write_register(ViPipe, 0x36e9, 0x23);
	sc3335_write_register(ViPipe, 0x36f9, 0x23);

	sc3335_default_reg_init(ViPipe);

	sc3335_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(100);

	printf("ViPipe:%d,===SC3335 1296P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
