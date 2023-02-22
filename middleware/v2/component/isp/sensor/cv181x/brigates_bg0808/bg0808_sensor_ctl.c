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
#include "bg0808_cmos_ex.h"

#define BG0808_CHIP_ID_HI_ADDR		0x0000
#define BG0808_CHIP_ID_LO_ADDR		0x0001
#define BG0808_CHIP_ID			0x0808
static void bg0808_linear_1080P30_init(VI_PIPE ViPipe);
static void bg0808_wdr_1080P30_2to1_init(VI_PIPE ViPipe);
CVI_U8 bg0808_i2c_addr = 0x32;        /* I2C Address of BG0808 */
const CVI_U32 bg0808_addr_byte = 2;
const CVI_U32 bg0808_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int bg0808_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunBG0808_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, bg0808_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int bg0808_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

int bg0808_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (bg0808_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, bg0808_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, bg0808_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (bg0808_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int bg0808_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (bg0808_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (bg0808_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, bg0808_addr_byte + bg0808_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);

	return CVI_SUCCESS;
}

void bg0808_standby(VI_PIPE ViPipe)
{
	(void) ViPipe;
	CVI_TRACE_SNS(CVI_DBG_NOTICE, "unsupport standby.\n");
}

void bg0808_restart(VI_PIPE ViPipe)
{
	(void) ViPipe;
	CVI_TRACE_SNS(CVI_DBG_NOTICE, "unsupport restart.\n");
}

void bg0808_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastBG0808[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastBG0808[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			bg0808_write_register(ViPipe,
				g_pastBG0808[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastBG0808[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void bg0808_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 val = 0;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		val |= 0x2;
		break;
	case ISP_SNS_FLIP:
		val |= 0x1;
		break;
	case ISP_SNS_MIRROR_FLIP:
		val |= 0x3;
		break;
	default:
		return;
	}

	bg0808_write_register(ViPipe, 0x0020, val);
	bg0808_write_register(ViPipe, 0x001d, 0x02);
}

int bg0808_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	delay_ms(4);
	if (bg0808_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = bg0808_read_register(ViPipe, BG0808_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = bg0808_read_register(ViPipe, BG0808_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != BG0808_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void bg0808_init(VI_PIPE ViPipe)
{
	WDR_MODE_E        enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastBG0808[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastBG0808[ViPipe]->u8ImgMode;

	if (g_fd[ViPipe] < 0)
		bg0808_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == BG0808_MODE_1920X1080P30_WDR) {
			bg0808_wdr_1080P30_2to1_init(ViPipe);
		} else {
		}
	} else {
		bg0808_linear_1080P30_init(ViPipe);
	}

	g_pastBG0808[ViPipe]->bInit = CVI_TRUE;
}

void bg0808_exit(VI_PIPE ViPipe)
{
	bg0808_i2c_exit(ViPipe);
}

/* 1080P30 and 1080P25 */
static void bg0808_linear_1080P30_init(VI_PIPE ViPipe)
{
	delay_ms(20);
	bg0808_write_register(ViPipe, 0x00b1, 0x55);
	bg0808_write_register(ViPipe, 0x00cc, 0x07);
	bg0808_write_register(ViPipe, 0x00e4, 0x14);
	bg0808_write_register(ViPipe, 0x00fa, 0x7f);
	bg0808_write_register(ViPipe, 0x0391, 0x84);
	bg0808_write_register(ViPipe, 0x0390, 0x06);
	bg0808_write_register(ViPipe, 0x0398, 0x15);

	bg0808_write_register(ViPipe, 0x03b2, 0x00);//hs trail, default 0x3c
	bg0808_write_register(ViPipe, 0x03b3, 0x78);

	bg0808_write_register(ViPipe, 0x03c3, 0x10);
	bg0808_write_register(ViPipe, 0x0392, 0x00);
	bg0808_write_register(ViPipe, 0x0045, 0x01);
	bg0808_write_register(ViPipe, 0x0046, 0x03);
	bg0808_write_register(ViPipe, 0x0074, 0x00);
	bg0808_write_register(ViPipe, 0x0075, 0x04);
	bg0808_write_register(ViPipe, 0x00ae, 0x0a);
	bg0808_write_register(ViPipe, 0x00d0, 0x02);
	bg0808_write_register(ViPipe, 0x00b2, 0x05);
	bg0808_write_register(ViPipe, 0x0120, 0x01);
	bg0808_write_register(ViPipe, 0x01a5, 0x07);
	bg0808_write_register(ViPipe, 0x007c, 0x04);
	bg0808_write_register(ViPipe, 0x00a8, 0x0f);
	bg0808_write_register(ViPipe, 0x0030, 0x30);
	bg0808_write_register(ViPipe, 0x0032, 0x01);
	bg0808_write_register(ViPipe, 0x0033, 0xed);
	bg0808_write_register(ViPipe, 0x0034, 0x1e);
	bg0808_write_register(ViPipe, 0x002c, 0x00);
	bg0808_write_register(ViPipe, 0x002d, 0x2b);
	bg0808_write_register(ViPipe, 0x002e, 0x02);
	bg0808_write_register(ViPipe, 0x000e, 0x08);
	bg0808_write_register(ViPipe, 0x000f, 0x98);
	bg0808_write_register(ViPipe, 0x0010, 0x04);
	bg0808_write_register(ViPipe, 0x0011, 0x65);
	bg0808_write_register(ViPipe, 0x0070, 0x00);
	bg0808_write_register(ViPipe, 0x0071, 0x01);
	bg0808_write_register(ViPipe, 0x0069, 0x01);
	bg0808_write_register(ViPipe, 0x004b, 0x02);
	bg0808_write_register(ViPipe, 0x004c, 0x02);
	bg0808_write_register(ViPipe, 0x0049, 0x00);
	bg0808_write_register(ViPipe, 0x004a, 0x08);
	bg0808_write_register(ViPipe, 0x004d, 0x0b);
	bg0808_write_register(ViPipe, 0x004e, 0x01);
	bg0808_write_register(ViPipe, 0x004f, 0x00);
	bg0808_write_register(ViPipe, 0x0050, 0x2a);
	bg0808_write_register(ViPipe, 0x0051, 0x00);
	bg0808_write_register(ViPipe, 0x0052, 0x4a);
	bg0808_write_register(ViPipe, 0x0055, 0x04);
	bg0808_write_register(ViPipe, 0x0056, 0x00);
	bg0808_write_register(ViPipe, 0x0057, 0x1e);
	bg0808_write_register(ViPipe, 0x0084, 0x00);
	bg0808_write_register(ViPipe, 0x0085, 0xb9);
	bg0808_write_register(ViPipe, 0x0058, 0x02);
	bg0808_write_register(ViPipe, 0x005a, 0x00);
	bg0808_write_register(ViPipe, 0x005b, 0x4a);
	bg0808_write_register(ViPipe, 0x005c, 0x00);
	bg0808_write_register(ViPipe, 0x005d, 0x4a);
	bg0808_write_register(ViPipe, 0x0086, 0x01);
	bg0808_write_register(ViPipe, 0x0087, 0xda);
	bg0808_write_register(ViPipe, 0x0088, 0x02);
	bg0808_write_register(ViPipe, 0x006f, 0x02);
	bg0808_write_register(ViPipe, 0x005e, 0x01);
	bg0808_write_register(ViPipe, 0x0043, 0x02);
	bg0808_write_register(ViPipe, 0x0044, 0x02);
	bg0808_write_register(ViPipe, 0x0039, 0x00);
	bg0808_write_register(ViPipe, 0x003a, 0xa0);
	bg0808_write_register(ViPipe, 0x0037, 0x00);
	bg0808_write_register(ViPipe, 0x0038, 0x80);
	bg0808_write_register(ViPipe, 0x006d, 0x0d);
	bg0808_write_register(ViPipe, 0x0064, 0x15);
	bg0808_write_register(ViPipe, 0x00a1, 0x0c);
	bg0808_write_register(ViPipe, 0x00aa, 0x0c);
	bg0808_write_register(ViPipe, 0x00bc, 0x02);
	bg0808_write_register(ViPipe, 0x0046, 0x00);
	//blc
	bg0808_write_register(ViPipe, 0x0130, 0x00);// blc, default 0x18
	bg0808_write_register(ViPipe, 0x0131, 0x18);
	bg0808_write_register(ViPipe, 0x001d, 0x01);

	bg0808_default_reg_init(ViPipe);
	delay_ms(20);

	printf("ViPipe:%d,===BG0808 1080P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
static void bg0808_wdr_1080P30_2to1_init(VI_PIPE ViPipe)
{
	delay_ms(20);
	bg0808_write_register(ViPipe, 0x00b1, 0x55);
	bg0808_write_register(ViPipe, 0x00cc, 0x07);
	bg0808_write_register(ViPipe, 0x00e4, 0x14);
	bg0808_write_register(ViPipe, 0x00fa, 0x7f);
	bg0808_write_register(ViPipe, 0x0391, 0x84);
	bg0808_write_register(ViPipe, 0x0390, 0x06);
	bg0808_write_register(ViPipe, 0x03c3, 0x10);
	bg0808_write_register(ViPipe, 0x0392, 0x00);

	bg0808_write_register(ViPipe, 0x03b2, 0x00);//hs trail, default 0x3c
	bg0808_write_register(ViPipe, 0x03b3, 0x78);

	bg0808_write_register(ViPipe, 0x0045, 0x01);
	bg0808_write_register(ViPipe, 0x0046, 0x03);
	bg0808_write_register(ViPipe, 0x0074, 0x00);
	bg0808_write_register(ViPipe, 0x0075, 0x04);
	bg0808_write_register(ViPipe, 0x00ae, 0x0a);
	bg0808_write_register(ViPipe, 0x00d0, 0x02);
	bg0808_write_register(ViPipe, 0x00b2, 0x05);
	bg0808_write_register(ViPipe, 0x0120, 0x01);
	bg0808_write_register(ViPipe, 0x01a5, 0x07);
	bg0808_write_register(ViPipe, 0x007c, 0x04);
	bg0808_write_register(ViPipe, 0x00a8, 0x0f);
	bg0808_write_register(ViPipe, 0x0032, 0x00);
	bg0808_write_register(ViPipe, 0x0033, 0x35);
	bg0808_write_register(ViPipe, 0x0034, 0x00);
	bg0808_write_register(ViPipe, 0x002c, 0x00);
	bg0808_write_register(ViPipe, 0x002d, 0x13);
	bg0808_write_register(ViPipe, 0x002e, 0x00);
	bg0808_write_register(ViPipe, 0x000e, 0x11);
	bg0808_write_register(ViPipe, 0x000f, 0x30);
	bg0808_write_register(ViPipe, 0x0010, 0x04);
	bg0808_write_register(ViPipe, 0x0011, 0x65);
	bg0808_write_register(ViPipe, 0x0070, 0x00);
	bg0808_write_register(ViPipe, 0x0071, 0x01);
	bg0808_write_register(ViPipe, 0x0069, 0x01);
	bg0808_write_register(ViPipe, 0x004b, 0x04);
	bg0808_write_register(ViPipe, 0x004c, 0x04);
	bg0808_write_register(ViPipe, 0x0049, 0x00);
	bg0808_write_register(ViPipe, 0x004a, 0x0f);
	bg0808_write_register(ViPipe, 0x004d, 0x14);
	bg0808_write_register(ViPipe, 0x004e, 0x01);
	bg0808_write_register(ViPipe, 0x004f, 0x00);
	bg0808_write_register(ViPipe, 0x0050, 0x53);
	bg0808_write_register(ViPipe, 0x0051, 0x00);
	bg0808_write_register(ViPipe, 0x0052, 0x93);
	bg0808_write_register(ViPipe, 0x0055, 0x08);
	bg0808_write_register(ViPipe, 0x0056, 0x00);
	bg0808_write_register(ViPipe, 0x0057, 0x3b);
	bg0808_write_register(ViPipe, 0x0084, 0x01);
	bg0808_write_register(ViPipe, 0x0085, 0x72);
	bg0808_write_register(ViPipe, 0x0058, 0x04);
	bg0808_write_register(ViPipe, 0x005a, 0x00);
	bg0808_write_register(ViPipe, 0x005b, 0x93);
	bg0808_write_register(ViPipe, 0x005c, 0x00);
	bg0808_write_register(ViPipe, 0x005d, 0x93);
	bg0808_write_register(ViPipe, 0x0086, 0x03);
	bg0808_write_register(ViPipe, 0x0087, 0x8e);
	bg0808_write_register(ViPipe, 0x0088, 0x04);
	bg0808_write_register(ViPipe, 0x006f, 0x04);
	bg0808_write_register(ViPipe, 0x005e, 0x01);
	bg0808_write_register(ViPipe, 0x0043, 0x04);
	bg0808_write_register(ViPipe, 0x0044, 0x04);
	bg0808_write_register(ViPipe, 0x0039, 0x01);
	bg0808_write_register(ViPipe, 0x003a, 0x40);
	bg0808_write_register(ViPipe, 0x0037, 0x01);
	bg0808_write_register(ViPipe, 0x0038, 0x00);
	bg0808_write_register(ViPipe, 0x006d, 0x0e);
	bg0808_write_register(ViPipe, 0x0064, 0x16);
	bg0808_write_register(ViPipe, 0x0021, 0x01);
	bg0808_write_register(ViPipe, 0x0041, 0x01);
	bg0808_write_register(ViPipe, 0x0042, 0x40);
	bg0808_write_register(ViPipe, 0x003f, 0x01);
	bg0808_write_register(ViPipe, 0x0040, 0x00);
	bg0808_write_register(ViPipe, 0x0098, 0x00);
	bg0808_write_register(ViPipe, 0x0099, 0x88);
	bg0808_write_register(ViPipe, 0x009a, 0x01);
	bg0808_write_register(ViPipe, 0x009b, 0x88);
	bg0808_write_register(ViPipe, 0x009c, 0x01);
	bg0808_write_register(ViPipe, 0x009d, 0x72);
	bg0808_write_register(ViPipe, 0x009e, 0x03);
	bg0808_write_register(ViPipe, 0x009f, 0x8e);
	bg0808_write_register(ViPipe, 0x00a1, 0x0c);
	bg0808_write_register(ViPipe, 0x00a3, 0x0c);
	bg0808_write_register(ViPipe, 0x00aa, 0x0c);
	bg0808_write_register(ViPipe, 0x00ab, 0x0c);
	bg0808_write_register(ViPipe, 0x00bc, 0x02);
	bg0808_write_register(ViPipe, 0x0046, 0x00);
	//blc
	bg0808_write_register(ViPipe, 0x0130, 0x00);// blc, default 0x18
	bg0808_write_register(ViPipe, 0x0131, 0x18);
	bg0808_write_register(ViPipe, 0x001d, 0x01);

	bg0808_default_reg_init(ViPipe);
	delay_ms(20);

	printf("ViPipe:%d,===BG0808 1080P 30fps 10bit WDR2TO1 Init OK!===\n", ViPipe);
}

