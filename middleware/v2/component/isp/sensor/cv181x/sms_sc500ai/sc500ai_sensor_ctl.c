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
#include "sc500ai_cmos_ex.h"

static void sc500ai_wdr_1620p30_2to1_init(VI_PIPE ViPipe);
static void sc500ai_linear_1620p30_init(VI_PIPE ViPipe);
static void sc500ai_wdr_1440p30_2to1_init(VI_PIPE ViPipe);
static void sc500ai_linear_1440p30_init(VI_PIPE ViPipe);

const CVI_U8 sc500ai_i2c_addr = 0x30;        /* I2C Address of SC500AI */
const CVI_U32 sc500ai_addr_byte = 2;
const CVI_U32 sc500ai_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc500ai_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC500AI_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc500ai_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc500ai_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc500ai_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc500ai_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc500ai_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc500ai_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc500ai_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc500ai_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc500ai_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc500ai_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc500ai_addr_byte + sc500ai_data_byte);
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

void sc500ai_standby(VI_PIPE ViPipe)
{
	sc500ai_write_register(ViPipe, 0x0100, 0x00);
}

void sc500ai_restart(VI_PIPE ViPipe)
{
	sc500ai_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc500ai_write_register(ViPipe, 0x0100, 0x01);
}

void sc500ai_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC500AI[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc500ai_write_register(ViPipe,
				g_pastSC500AI[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC500AI[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC500AI_CHIP_ID_HI_ADDR		0x3107
#define SC500AI_CHIP_ID_LO_ADDR		0x3108
#define SC500AI_CHIP_ID			0xce1f

void sc500ai_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc500ai_write_register(ViPipe, 0x3221, val);
}

static int sc500ai_init_ex(VI_PIPE ViPipe)
{
	CVI_U32 debounce = 0;
	int nVal, cnt = 0;
	CVI_U16 tmp = 0, tmpPrev = 0;

	while (debounce++ < 5) {
		nVal = sc500ai_read_register(ViPipe, 0x3109);
		if (nVal < 0) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "debounce read fail.\n");
			return nVal;
		}
		tmp = (nVal & 0xFF) << 8;
		if (tmp != tmpPrev)
			debounce = 0;
		tmpPrev = tmp;
		if (cnt++ > 20) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "debounce fail.\n");
			return nVal;
		}
	}
	if (tmp == 1) {
		sc500ai_write_register(ViPipe, 0x336d, 0x23);
	} else {
		sc500ai_write_register(ViPipe, 0x336d, 0x03);
	}

	debounce = 0;
	cnt = 0;
	while (debounce++ < 5) {
		nVal = sc500ai_read_register(ViPipe, 0x3040);
		if (nVal < 0) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "debounce read fail.\n");
			return nVal;
		}
		tmp = (nVal & 0xFF) << 8;
		if (tmp != tmpPrev)
			debounce = 0;
		tmpPrev = tmp;
		if (cnt++ > 20) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "debounce fail.\n");
			return nVal;
		}
	}
	if (tmp == 0) {
		sc500ai_write_register(ViPipe, 0x363c, 0x42);
	} else {
		sc500ai_write_register(ViPipe, 0x363c, 0x40);
	}

	return CVI_SUCCESS;
}

int sc500ai_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	delay_ms(4);
	if (sc500ai_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc500ai_read_register(ViPipe, SC500AI_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc500ai_read_register(ViPipe, SC500AI_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC500AI_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc500ai_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_BOOL          bInit;
	CVI_U8            u8ImgMode;

	bInit       = g_pastSC500AI[ViPipe]->bInit;
	enWDRMode   = g_pastSC500AI[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastSC500AI[ViPipe]->u8ImgMode;

	sc500ai_i2c_init(ViPipe);

	/* When sensor first init, config all registers */
	if (bInit == CVI_FALSE) {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			if (u8ImgMode == SC500AI_MODE_1620P30_WDR) {
				/* SC500AI_MODE_1620P30_WDR */
				sc500ai_wdr_1620p30_2to1_init(ViPipe);
			} else if (u8ImgMode == SC500AI_MODE_1440P30_WDR) {
				/* SC500AI_MODE_1620P30_WDR */
				sc500ai_wdr_1440p30_2to1_init(ViPipe);
			} else {
			}
		} else {
			if (u8ImgMode == SC500AI_MODE_1620P30) {
				sc500ai_linear_1620p30_init(ViPipe);
			} else if (u8ImgMode == SC500AI_MODE_1440P30) {
				/* SC500AI_MODE_1440P30 */
				sc500ai_linear_1440p30_init(ViPipe);
			} else {
			}
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			if (u8ImgMode == SC500AI_MODE_1620P30_WDR) {
				/* SC500AI_MODE_1620P30_WDR */
				sc500ai_wdr_1620p30_2to1_init(ViPipe);
			} else if (u8ImgMode == SC500AI_MODE_1440P30_WDR) {
				/* SC500AI_MODE_1620P30_WDR */
				sc500ai_wdr_1440p30_2to1_init(ViPipe);
			} else {
			}
		} else {
			if (u8ImgMode == SC500AI_MODE_1620P30) {
				sc500ai_linear_1620p30_init(ViPipe);
			} else if (u8ImgMode == SC500AI_MODE_1440P30) {
				/* SC500AI_MODE_1440P30 */
				sc500ai_linear_1440p30_init(ViPipe);
			} else {
			}
		}
	}
	g_pastSC500AI[ViPipe]->bInit = CVI_TRUE;
}

void sc500ai_exit(VI_PIPE ViPipe)
{
	sc500ai_i2c_exit(ViPipe);
}

/* 1620P30 and 1620P25 */
static void sc500ai_linear_1620p30_init(VI_PIPE ViPipe)
{
	sc500ai_write_register(ViPipe, 0x0103, 0x01);
	sc500ai_write_register(ViPipe, 0x0100, 0x00);
	sc500ai_write_register(ViPipe, 0x36e9, 0x80);
	sc500ai_write_register(ViPipe, 0x36f9, 0x80);
	sc500ai_write_register(ViPipe, 0x301f, 0x01);
	sc500ai_write_register(ViPipe, 0x3253, 0x0a);
	sc500ai_write_register(ViPipe, 0x3301, 0x0a);
	sc500ai_write_register(ViPipe, 0x3302, 0x18);
	sc500ai_write_register(ViPipe, 0x3303, 0x10);
	sc500ai_write_register(ViPipe, 0x3304, 0x60);
	sc500ai_write_register(ViPipe, 0x3306, 0x60);
	sc500ai_write_register(ViPipe, 0x3308, 0x10);
	sc500ai_write_register(ViPipe, 0x3309, 0x70);
	sc500ai_write_register(ViPipe, 0x330a, 0x00);
	sc500ai_write_register(ViPipe, 0x330b, 0xf0);
	sc500ai_write_register(ViPipe, 0x330d, 0x18);
	sc500ai_write_register(ViPipe, 0x330e, 0x20);
	sc500ai_write_register(ViPipe, 0x330f, 0x02);
	sc500ai_write_register(ViPipe, 0x3310, 0x02);
	sc500ai_write_register(ViPipe, 0x331c, 0x04);
	sc500ai_write_register(ViPipe, 0x331e, 0x51);
	sc500ai_write_register(ViPipe, 0x331f, 0x61);
	sc500ai_write_register(ViPipe, 0x3320, 0x09);
	sc500ai_write_register(ViPipe, 0x3333, 0x10);
	sc500ai_write_register(ViPipe, 0x334c, 0x08);
	sc500ai_write_register(ViPipe, 0x3356, 0x09);
	sc500ai_write_register(ViPipe, 0x3364, 0x17);
	sc500ai_write_register(ViPipe, 0x336d, 0x03);
	sc500ai_write_register(ViPipe, 0x3390, 0x08);
	sc500ai_write_register(ViPipe, 0x3391, 0x18);
	sc500ai_write_register(ViPipe, 0x3392, 0x38);
	sc500ai_write_register(ViPipe, 0x3393, 0x0a);
	sc500ai_write_register(ViPipe, 0x3394, 0x20);
	sc500ai_write_register(ViPipe, 0x3395, 0x20);
	sc500ai_write_register(ViPipe, 0x3396, 0x08);
	sc500ai_write_register(ViPipe, 0x3397, 0x18);
	sc500ai_write_register(ViPipe, 0x3398, 0x38);
	sc500ai_write_register(ViPipe, 0x3399, 0x0a);
	sc500ai_write_register(ViPipe, 0x339a, 0x20);
	sc500ai_write_register(ViPipe, 0x339b, 0x20);
	sc500ai_write_register(ViPipe, 0x339c, 0x20);
	sc500ai_write_register(ViPipe, 0x33ac, 0x10);
	sc500ai_write_register(ViPipe, 0x33ae, 0x10);
	sc500ai_write_register(ViPipe, 0x33af, 0x19);
	sc500ai_write_register(ViPipe, 0x360f, 0x01);
	sc500ai_write_register(ViPipe, 0x3622, 0x03);
	sc500ai_write_register(ViPipe, 0x363a, 0x1f);
	sc500ai_write_register(ViPipe, 0x363c, 0x40);
	sc500ai_write_register(ViPipe, 0x3651, 0x7d);
	sc500ai_write_register(ViPipe, 0x3670, 0x0a);
	sc500ai_write_register(ViPipe, 0x3671, 0x07);
	sc500ai_write_register(ViPipe, 0x3672, 0x17);
	sc500ai_write_register(ViPipe, 0x3673, 0x1e);
	sc500ai_write_register(ViPipe, 0x3674, 0x82);
	sc500ai_write_register(ViPipe, 0x3675, 0x64);
	sc500ai_write_register(ViPipe, 0x3676, 0x66);
	sc500ai_write_register(ViPipe, 0x367a, 0x48);
	sc500ai_write_register(ViPipe, 0x367b, 0x78);
	sc500ai_write_register(ViPipe, 0x367c, 0x58);
	sc500ai_write_register(ViPipe, 0x367d, 0x78);
	sc500ai_write_register(ViPipe, 0x3690, 0x34);
	sc500ai_write_register(ViPipe, 0x3691, 0x34);
	sc500ai_write_register(ViPipe, 0x3692, 0x54);
	sc500ai_write_register(ViPipe, 0x369c, 0x48);
	sc500ai_write_register(ViPipe, 0x369d, 0x78);
	sc500ai_write_register(ViPipe, 0x36ec, 0x1a);
	sc500ai_write_register(ViPipe, 0x3904, 0x04);
	sc500ai_write_register(ViPipe, 0x3908, 0x41);
	sc500ai_write_register(ViPipe, 0x391d, 0x04);
	sc500ai_write_register(ViPipe, 0x39c2, 0x30);
	sc500ai_write_register(ViPipe, 0x3e01, 0xcd);
	sc500ai_write_register(ViPipe, 0x3e02, 0xc0);
	sc500ai_write_register(ViPipe, 0x3e16, 0x00);
	sc500ai_write_register(ViPipe, 0x3e17, 0x80);
	sc500ai_write_register(ViPipe, 0x4500, 0x88);
	sc500ai_write_register(ViPipe, 0x4509, 0x20);
	sc500ai_write_register(ViPipe, 0x481b, 0x50);	// hs-trail
	sc500ai_write_register(ViPipe, 0x5799, 0x00);
	sc500ai_write_register(ViPipe, 0x59e0, 0x60);
	sc500ai_write_register(ViPipe, 0x59e1, 0x08);
	sc500ai_write_register(ViPipe, 0x59e2, 0x3f);
	sc500ai_write_register(ViPipe, 0x59e3, 0x18);
	sc500ai_write_register(ViPipe, 0x59e4, 0x18);
	sc500ai_write_register(ViPipe, 0x59e5, 0x3f);
	sc500ai_write_register(ViPipe, 0x59e7, 0x02);
	sc500ai_write_register(ViPipe, 0x59e8, 0x38);
	sc500ai_write_register(ViPipe, 0x59e9, 0x20);
	sc500ai_write_register(ViPipe, 0x59ea, 0x0c);
	sc500ai_write_register(ViPipe, 0x59ec, 0x08);
	sc500ai_write_register(ViPipe, 0x59ed, 0x02);
	sc500ai_write_register(ViPipe, 0x59ee, 0xa0);
	sc500ai_write_register(ViPipe, 0x59ef, 0x08);
	sc500ai_write_register(ViPipe, 0x59f4, 0x18);
	sc500ai_write_register(ViPipe, 0x59f5, 0x10);
	sc500ai_write_register(ViPipe, 0x59f6, 0x0c);
	sc500ai_write_register(ViPipe, 0x59f9, 0x02);
	sc500ai_write_register(ViPipe, 0x59fa, 0x18);
	sc500ai_write_register(ViPipe, 0x59fb, 0x10);
	sc500ai_write_register(ViPipe, 0x59fc, 0x0c);
	sc500ai_write_register(ViPipe, 0x59ff, 0x02);
	sc500ai_write_register(ViPipe, 0x36e9, 0x1c);
	sc500ai_write_register(ViPipe, 0x36f9, 0x24);
	sc500ai_init_ex(ViPipe);

	sc500ai_default_reg_init(ViPipe);

	sc500ai_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC500AI 1620P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

static void sc500ai_wdr_1620p30_2to1_init(VI_PIPE ViPipe)
{
	sc500ai_write_register(ViPipe, 0x0103, 0x01);
	sc500ai_write_register(ViPipe, 0x0100, 0x00);
	sc500ai_write_register(ViPipe, 0x36e9, 0x80);
	sc500ai_write_register(ViPipe, 0x36f9, 0x80);
	sc500ai_write_register(ViPipe, 0x301f, 0x06);
	sc500ai_write_register(ViPipe, 0x3106, 0x01);
	sc500ai_write_register(ViPipe, 0x320e, 0x0c);
	sc500ai_write_register(ViPipe, 0x320f, 0xe4);
	sc500ai_write_register(ViPipe, 0x3220, 0x53);
	sc500ai_write_register(ViPipe, 0x3250, 0xff);
	sc500ai_write_register(ViPipe, 0x3253, 0x0a);
	sc500ai_write_register(ViPipe, 0x3301, 0x0b);
	sc500ai_write_register(ViPipe, 0x3302, 0x20);
	sc500ai_write_register(ViPipe, 0x3303, 0x10);
	sc500ai_write_register(ViPipe, 0x3304, 0x70);
	sc500ai_write_register(ViPipe, 0x3306, 0x50);
	sc500ai_write_register(ViPipe, 0x3308, 0x18);
	sc500ai_write_register(ViPipe, 0x3309, 0x80);
	sc500ai_write_register(ViPipe, 0x330a, 0x00);
	sc500ai_write_register(ViPipe, 0x330b, 0xe8);
	sc500ai_write_register(ViPipe, 0x330d, 0x30);
	sc500ai_write_register(ViPipe, 0x330e, 0x30);
	sc500ai_write_register(ViPipe, 0x330f, 0x02);
	sc500ai_write_register(ViPipe, 0x3310, 0x02);
	sc500ai_write_register(ViPipe, 0x331c, 0x08);
	sc500ai_write_register(ViPipe, 0x331e, 0x61);
	sc500ai_write_register(ViPipe, 0x331f, 0x71);
	sc500ai_write_register(ViPipe, 0x3320, 0x11);
	sc500ai_write_register(ViPipe, 0x3333, 0x10);
	sc500ai_write_register(ViPipe, 0x334c, 0x10);
	sc500ai_write_register(ViPipe, 0x3356, 0x11);
	sc500ai_write_register(ViPipe, 0x3364, 0x17);
	sc500ai_write_register(ViPipe, 0x336d, 0x03);
	sc500ai_write_register(ViPipe, 0x3390, 0x08);
	sc500ai_write_register(ViPipe, 0x3391, 0x18);
	sc500ai_write_register(ViPipe, 0x3392, 0x38);
	sc500ai_write_register(ViPipe, 0x3393, 0x0a);
	sc500ai_write_register(ViPipe, 0x3394, 0x0a);
	sc500ai_write_register(ViPipe, 0x3395, 0x12);
	sc500ai_write_register(ViPipe, 0x3396, 0x08);
	sc500ai_write_register(ViPipe, 0x3397, 0x18);
	sc500ai_write_register(ViPipe, 0x3398, 0x38);
	sc500ai_write_register(ViPipe, 0x3399, 0x0a);
	sc500ai_write_register(ViPipe, 0x339a, 0x0a);
	sc500ai_write_register(ViPipe, 0x339b, 0x0a);
	sc500ai_write_register(ViPipe, 0x339c, 0x12);
	sc500ai_write_register(ViPipe, 0x33ac, 0x10);
	sc500ai_write_register(ViPipe, 0x33ae, 0x20);
	sc500ai_write_register(ViPipe, 0x33af, 0x21);
	sc500ai_write_register(ViPipe, 0x360f, 0x01);
	sc500ai_write_register(ViPipe, 0x3621, 0xe8);
	sc500ai_write_register(ViPipe, 0x3622, 0x06);
	sc500ai_write_register(ViPipe, 0x3630, 0x82);
	sc500ai_write_register(ViPipe, 0x3633, 0x33);
	sc500ai_write_register(ViPipe, 0x3634, 0x64);
	sc500ai_write_register(ViPipe, 0x3637, 0x50);
	sc500ai_write_register(ViPipe, 0x363a, 0x1f);
	sc500ai_write_register(ViPipe, 0x363c, 0x40);
	sc500ai_write_register(ViPipe, 0x3651, 0x7d);
	sc500ai_write_register(ViPipe, 0x3670, 0x0a);
	sc500ai_write_register(ViPipe, 0x3671, 0x06);
	sc500ai_write_register(ViPipe, 0x3672, 0x16);
	sc500ai_write_register(ViPipe, 0x3673, 0x17);
	sc500ai_write_register(ViPipe, 0x3674, 0x82);
	sc500ai_write_register(ViPipe, 0x3675, 0x62);
	sc500ai_write_register(ViPipe, 0x3676, 0x44);
	sc500ai_write_register(ViPipe, 0x367a, 0x48);
	sc500ai_write_register(ViPipe, 0x367b, 0x78);
	sc500ai_write_register(ViPipe, 0x367c, 0x48);
	sc500ai_write_register(ViPipe, 0x367d, 0x58);
	sc500ai_write_register(ViPipe, 0x3690, 0x34);
	sc500ai_write_register(ViPipe, 0x3691, 0x34);
	sc500ai_write_register(ViPipe, 0x3692, 0x54);
	sc500ai_write_register(ViPipe, 0x369c, 0x48);
	sc500ai_write_register(ViPipe, 0x369d, 0x78);
	sc500ai_write_register(ViPipe, 0x36ea, 0x35);
	sc500ai_write_register(ViPipe, 0x36eb, 0x04);
	sc500ai_write_register(ViPipe, 0x36ec, 0x0a);
	sc500ai_write_register(ViPipe, 0x36ed, 0x14);
	sc500ai_write_register(ViPipe, 0x36fa, 0x35);
	sc500ai_write_register(ViPipe, 0x36fb, 0x04);
	sc500ai_write_register(ViPipe, 0x36fc, 0x00);
	sc500ai_write_register(ViPipe, 0x36fd, 0x16);
	sc500ai_write_register(ViPipe, 0x3904, 0x04);
	sc500ai_write_register(ViPipe, 0x3908, 0x41);
	sc500ai_write_register(ViPipe, 0x391f, 0x10);
	sc500ai_write_register(ViPipe, 0x39c2, 0x30);
	sc500ai_write_register(ViPipe, 0x3e00, 0x01);
	sc500ai_write_register(ViPipe, 0x3e01, 0x82);
	sc500ai_write_register(ViPipe, 0x3e02, 0x00);
	sc500ai_write_register(ViPipe, 0x3e04, 0x18);
	sc500ai_write_register(ViPipe, 0x3e05, 0x20);
	sc500ai_write_register(ViPipe, 0x3e23, 0x00);
	sc500ai_write_register(ViPipe, 0x3e24, 0xc6);
	sc500ai_write_register(ViPipe, 0x4500, 0x88);
	sc500ai_write_register(ViPipe, 0x4509, 0x20);
	sc500ai_write_register(ViPipe, 0x4800, 0x04);
	sc500ai_write_register(ViPipe, 0x4837, 0x15);
	sc500ai_write_register(ViPipe, 0x4853, 0xfd);
	sc500ai_write_register(ViPipe, 0x481b, 0x50);	// hs-trail
	sc500ai_write_register(ViPipe, 0x36e9, 0x44);
	sc500ai_write_register(ViPipe, 0x36f9, 0x44);
	sc500ai_write_register(ViPipe, 0x0100, 0x01);
	sc500ai_init_ex(ViPipe);

	sc500ai_default_reg_init(ViPipe);

	sc500ai_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(50);

	printf("===SC500AI sensor 1620P30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}

/* 1440P30 and 1440P25 */
static void sc500ai_linear_1440p30_init(VI_PIPE ViPipe)
{
	sc500ai_write_register(ViPipe, 0x0103, 0x01);
	sc500ai_write_register(ViPipe, 0x0100, 0x00);
	sc500ai_write_register(ViPipe, 0x36e9, 0x80);
	sc500ai_write_register(ViPipe, 0x36f9, 0x80);
	sc500ai_write_register(ViPipe, 0x301f, 0x03);
	sc500ai_write_register(ViPipe, 0x3200, 0x00);
	sc500ai_write_register(ViPipe, 0x3201, 0xa0);
	sc500ai_write_register(ViPipe, 0x3202, 0x00);
	sc500ai_write_register(ViPipe, 0x3203, 0x5a);
	sc500ai_write_register(ViPipe, 0x3204, 0x0a);
	sc500ai_write_register(ViPipe, 0x3205, 0xa7);
	sc500ai_write_register(ViPipe, 0x3206, 0x06);
	sc500ai_write_register(ViPipe, 0x3207, 0x01);
	sc500ai_write_register(ViPipe, 0x3208, 0x0a);
	sc500ai_write_register(ViPipe, 0x3209, 0x00);
	sc500ai_write_register(ViPipe, 0x320a, 0x05);
	sc500ai_write_register(ViPipe, 0x320b, 0xa0);
	sc500ai_write_register(ViPipe, 0x320c, 0x05);
	sc500ai_write_register(ViPipe, 0x320d, 0x78);
	sc500ai_write_register(ViPipe, 0x320e, 0x05);
	sc500ai_write_register(ViPipe, 0x320f, 0xdc);
	sc500ai_write_register(ViPipe, 0x3210, 0x00);
	sc500ai_write_register(ViPipe, 0x3211, 0x04);
	sc500ai_write_register(ViPipe, 0x3212, 0x00);
	sc500ai_write_register(ViPipe, 0x3213, 0x04);
	sc500ai_write_register(ViPipe, 0x3253, 0x0a);
	sc500ai_write_register(ViPipe, 0x3301, 0x08);
	sc500ai_write_register(ViPipe, 0x3302, 0x18);
	sc500ai_write_register(ViPipe, 0x3303, 0x10);
	sc500ai_write_register(ViPipe, 0x3304, 0x4c);
	sc500ai_write_register(ViPipe, 0x3306, 0x44);
	sc500ai_write_register(ViPipe, 0x3308, 0x10);
	sc500ai_write_register(ViPipe, 0x3309, 0x58);
	sc500ai_write_register(ViPipe, 0x330a, 0x00);
	sc500ai_write_register(ViPipe, 0x330b, 0xd8);
	sc500ai_write_register(ViPipe, 0x330d, 0x14);
	sc500ai_write_register(ViPipe, 0x330e, 0x20);
	sc500ai_write_register(ViPipe, 0x330f, 0x02);
	sc500ai_write_register(ViPipe, 0x3310, 0x02);
	sc500ai_write_register(ViPipe, 0x331c, 0x04);
	sc500ai_write_register(ViPipe, 0x331e, 0x3d);
	sc500ai_write_register(ViPipe, 0x331f, 0x49);
	sc500ai_write_register(ViPipe, 0x3320, 0x09);
	sc500ai_write_register(ViPipe, 0x3333, 0x10);
	sc500ai_write_register(ViPipe, 0x334c, 0x08);
	sc500ai_write_register(ViPipe, 0x3356, 0x09);
	sc500ai_write_register(ViPipe, 0x3364, 0x17);
	sc500ai_write_register(ViPipe, 0x336d, 0x03);
	sc500ai_write_register(ViPipe, 0x3390, 0x08);
	sc500ai_write_register(ViPipe, 0x3391, 0x18);
	sc500ai_write_register(ViPipe, 0x3392, 0x38);
	sc500ai_write_register(ViPipe, 0x3393, 0x08);
	sc500ai_write_register(ViPipe, 0x3394, 0x20);
	sc500ai_write_register(ViPipe, 0x3395, 0x20);
	sc500ai_write_register(ViPipe, 0x3396, 0x08);
	sc500ai_write_register(ViPipe, 0x3397, 0x18);
	sc500ai_write_register(ViPipe, 0x3398, 0x38);
	sc500ai_write_register(ViPipe, 0x3399, 0x08);
	sc500ai_write_register(ViPipe, 0x339a, 0x20);
	sc500ai_write_register(ViPipe, 0x339b, 0x20);
	sc500ai_write_register(ViPipe, 0x339c, 0x20);
	sc500ai_write_register(ViPipe, 0x33ac, 0x10);
	sc500ai_write_register(ViPipe, 0x33ae, 0x10);
	sc500ai_write_register(ViPipe, 0x33af, 0x19);
	sc500ai_write_register(ViPipe, 0x360f, 0x01);
	sc500ai_write_register(ViPipe, 0x3622, 0x03);
	sc500ai_write_register(ViPipe, 0x363a, 0x1f);
	sc500ai_write_register(ViPipe, 0x363c, 0x40);
	sc500ai_write_register(ViPipe, 0x3651, 0x7d);
	sc500ai_write_register(ViPipe, 0x3670, 0x0a);
	sc500ai_write_register(ViPipe, 0x3671, 0x07);
	sc500ai_write_register(ViPipe, 0x3672, 0x17);
	sc500ai_write_register(ViPipe, 0x3673, 0x1e);
	sc500ai_write_register(ViPipe, 0x3674, 0x82);
	sc500ai_write_register(ViPipe, 0x3675, 0x64);
	sc500ai_write_register(ViPipe, 0x3676, 0x66);
	sc500ai_write_register(ViPipe, 0x367a, 0x48);
	sc500ai_write_register(ViPipe, 0x367b, 0x78);
	sc500ai_write_register(ViPipe, 0x367c, 0x58);
	sc500ai_write_register(ViPipe, 0x367d, 0x78);
	sc500ai_write_register(ViPipe, 0x3690, 0x34);
	sc500ai_write_register(ViPipe, 0x3691, 0x34);
	sc500ai_write_register(ViPipe, 0x3692, 0x54);
	sc500ai_write_register(ViPipe, 0x369c, 0x48);
	sc500ai_write_register(ViPipe, 0x369d, 0x78);
	sc500ai_write_register(ViPipe, 0x36ea, 0x39);
	sc500ai_write_register(ViPipe, 0x36eb, 0x0e);
	sc500ai_write_register(ViPipe, 0x36ec, 0x1a);
	sc500ai_write_register(ViPipe, 0x36ed, 0x34);
	sc500ai_write_register(ViPipe, 0x36fa, 0x32);
	sc500ai_write_register(ViPipe, 0x36fb, 0x0e);
	sc500ai_write_register(ViPipe, 0x36fc, 0x10);
	sc500ai_write_register(ViPipe, 0x36fd, 0x14);
	sc500ai_write_register(ViPipe, 0x3904, 0x04);
	sc500ai_write_register(ViPipe, 0x3908, 0x41);
	sc500ai_write_register(ViPipe, 0x391d, 0x04);
	sc500ai_write_register(ViPipe, 0x39c2, 0x30);
	sc500ai_write_register(ViPipe, 0x3e01, 0xbb);
	sc500ai_write_register(ViPipe, 0x3e02, 0x00);
	sc500ai_write_register(ViPipe, 0x3e16, 0x00);
	sc500ai_write_register(ViPipe, 0x3e17, 0x80);
	sc500ai_write_register(ViPipe, 0x4500, 0x88);
	sc500ai_write_register(ViPipe, 0x4509, 0x20);
	sc500ai_write_register(ViPipe, 0x481b, 0x50);	// hs-trail
	sc500ai_write_register(ViPipe, 0x4837, 0x2a);
	sc500ai_write_register(ViPipe, 0x5799, 0x00);
	sc500ai_write_register(ViPipe, 0x59e0, 0x60);
	sc500ai_write_register(ViPipe, 0x59e1, 0x08);
	sc500ai_write_register(ViPipe, 0x59e2, 0x3f);
	sc500ai_write_register(ViPipe, 0x59e3, 0x18);
	sc500ai_write_register(ViPipe, 0x59e4, 0x18);
	sc500ai_write_register(ViPipe, 0x59e5, 0x3f);
	sc500ai_write_register(ViPipe, 0x59e7, 0x02);
	sc500ai_write_register(ViPipe, 0x59e8, 0x38);
	sc500ai_write_register(ViPipe, 0x59e9, 0x20);
	sc500ai_write_register(ViPipe, 0x59ea, 0x0c);
	sc500ai_write_register(ViPipe, 0x59ec, 0x08);
	sc500ai_write_register(ViPipe, 0x59ed, 0x02);
	sc500ai_write_register(ViPipe, 0x59ee, 0xa0);
	sc500ai_write_register(ViPipe, 0x59ef, 0x08);
	sc500ai_write_register(ViPipe, 0x59f4, 0x18);
	sc500ai_write_register(ViPipe, 0x59f5, 0x10);
	sc500ai_write_register(ViPipe, 0x59f6, 0x0c);
	sc500ai_write_register(ViPipe, 0x59f9, 0x02);
	sc500ai_write_register(ViPipe, 0x59fa, 0x18);
	sc500ai_write_register(ViPipe, 0x59fb, 0x10);
	sc500ai_write_register(ViPipe, 0x59fc, 0x0c);
	sc500ai_write_register(ViPipe, 0x59ff, 0x02);
	sc500ai_write_register(ViPipe, 0x36e9, 0x44);
	sc500ai_write_register(ViPipe, 0x36f9, 0x44);
	sc500ai_init_ex(ViPipe);

	sc500ai_default_reg_init(ViPipe);

	sc500ai_write_register(ViPipe, 0x0100, 0x01);

	printf("ViPipe:%d,===SC500AI 1440P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

static void sc500ai_wdr_1440p30_2to1_init(VI_PIPE ViPipe)
{
	sc500ai_write_register(ViPipe, 0x0103, 0x01);
	sc500ai_write_register(ViPipe, 0x0100, 0x00);
	sc500ai_write_register(ViPipe, 0x36e9, 0x80);
	sc500ai_write_register(ViPipe, 0x36f9, 0x80);
	sc500ai_write_register(ViPipe, 0x301f, 0x12);
	sc500ai_write_register(ViPipe, 0x3106, 0x01);
	sc500ai_write_register(ViPipe, 0x3200, 0x00);
	sc500ai_write_register(ViPipe, 0x3201, 0xa0);
	sc500ai_write_register(ViPipe, 0x3202, 0x00);
	sc500ai_write_register(ViPipe, 0x3203, 0x5a);
	sc500ai_write_register(ViPipe, 0x3204, 0x0a);
	sc500ai_write_register(ViPipe, 0x3205, 0xa7);
	sc500ai_write_register(ViPipe, 0x3206, 0x06);
	sc500ai_write_register(ViPipe, 0x3207, 0x01);
	sc500ai_write_register(ViPipe, 0x3208, 0x0a);
	sc500ai_write_register(ViPipe, 0x3209, 0x00);
	sc500ai_write_register(ViPipe, 0x320a, 0x05);
	sc500ai_write_register(ViPipe, 0x320b, 0xa0);
	sc500ai_write_register(ViPipe, 0x320c, 0x05);
	sc500ai_write_register(ViPipe, 0x320d, 0xdc);
	sc500ai_write_register(ViPipe, 0x320e, 0x0b);
	sc500ai_write_register(ViPipe, 0x320f, 0xb8);
	sc500ai_write_register(ViPipe, 0x3210, 0x00);
	sc500ai_write_register(ViPipe, 0x3211, 0x04);
	sc500ai_write_register(ViPipe, 0x3212, 0x00);
	sc500ai_write_register(ViPipe, 0x3213, 0x04);
	sc500ai_write_register(ViPipe, 0x3220, 0x53);
	sc500ai_write_register(ViPipe, 0x3250, 0xff);
	sc500ai_write_register(ViPipe, 0x3253, 0x0a);
	sc500ai_write_register(ViPipe, 0x3301, 0x0b);
	sc500ai_write_register(ViPipe, 0x3302, 0x20);
	sc500ai_write_register(ViPipe, 0x3303, 0x10);
	sc500ai_write_register(ViPipe, 0x3304, 0x60);
	sc500ai_write_register(ViPipe, 0x3306, 0x40);
	sc500ai_write_register(ViPipe, 0x3308, 0x18);
	sc500ai_write_register(ViPipe, 0x3309, 0x80);
	sc500ai_write_register(ViPipe, 0x330a, 0x01);
	sc500ai_write_register(ViPipe, 0x330b, 0x04);
	sc500ai_write_register(ViPipe, 0x330d, 0x28);
	sc500ai_write_register(ViPipe, 0x330e, 0x30);
	sc500ai_write_register(ViPipe, 0x330f, 0x02);
	sc500ai_write_register(ViPipe, 0x3310, 0x02);
	sc500ai_write_register(ViPipe, 0x331c, 0x08);
	sc500ai_write_register(ViPipe, 0x331e, 0x51);
	sc500ai_write_register(ViPipe, 0x331f, 0x71);
	sc500ai_write_register(ViPipe, 0x3320, 0x11);
	sc500ai_write_register(ViPipe, 0x3333, 0x10);
	sc500ai_write_register(ViPipe, 0x334c, 0x10);
	sc500ai_write_register(ViPipe, 0x3356, 0x11);
	sc500ai_write_register(ViPipe, 0x3364, 0x17);
	sc500ai_write_register(ViPipe, 0x336d, 0x03);
	sc500ai_write_register(ViPipe, 0x3390, 0x08);
	sc500ai_write_register(ViPipe, 0x3391, 0x18);
	sc500ai_write_register(ViPipe, 0x3392, 0x38);
	sc500ai_write_register(ViPipe, 0x3393, 0x0a);
	sc500ai_write_register(ViPipe, 0x3394, 0x0a);
	sc500ai_write_register(ViPipe, 0x3395, 0x12);
	sc500ai_write_register(ViPipe, 0x3396, 0x08);
	sc500ai_write_register(ViPipe, 0x3397, 0x18);
	sc500ai_write_register(ViPipe, 0x3398, 0x38);
	sc500ai_write_register(ViPipe, 0x3399, 0x0a);
	sc500ai_write_register(ViPipe, 0x339a, 0x0a);
	sc500ai_write_register(ViPipe, 0x339b, 0x0a);
	sc500ai_write_register(ViPipe, 0x339c, 0x12);
	sc500ai_write_register(ViPipe, 0x33ac, 0x10);
	sc500ai_write_register(ViPipe, 0x33ae, 0x20);
	sc500ai_write_register(ViPipe, 0x33af, 0x21);
	sc500ai_write_register(ViPipe, 0x360f, 0x01);
	sc500ai_write_register(ViPipe, 0x3621, 0xe8);
	sc500ai_write_register(ViPipe, 0x3622, 0x06);
	sc500ai_write_register(ViPipe, 0x3630, 0x82);
	sc500ai_write_register(ViPipe, 0x3633, 0x33);
	sc500ai_write_register(ViPipe, 0x3634, 0x64);
	sc500ai_write_register(ViPipe, 0x3637, 0x50);
	sc500ai_write_register(ViPipe, 0x363a, 0x1f);
	sc500ai_write_register(ViPipe, 0x363c, 0x40);
	sc500ai_write_register(ViPipe, 0x3651, 0x7d);
	sc500ai_write_register(ViPipe, 0x3670, 0x0a);
	sc500ai_write_register(ViPipe, 0x3671, 0x06);
	sc500ai_write_register(ViPipe, 0x3672, 0x16);
	sc500ai_write_register(ViPipe, 0x3673, 0x17);
	sc500ai_write_register(ViPipe, 0x3674, 0x82);
	sc500ai_write_register(ViPipe, 0x3675, 0x62);
	sc500ai_write_register(ViPipe, 0x3676, 0x44);
	sc500ai_write_register(ViPipe, 0x367a, 0x48);
	sc500ai_write_register(ViPipe, 0x367b, 0x78);
	sc500ai_write_register(ViPipe, 0x367c, 0x48);
	sc500ai_write_register(ViPipe, 0x367d, 0x58);
	sc500ai_write_register(ViPipe, 0x3690, 0x34);
	sc500ai_write_register(ViPipe, 0x3691, 0x34);
	sc500ai_write_register(ViPipe, 0x3692, 0x54);
	sc500ai_write_register(ViPipe, 0x369c, 0x48);
	sc500ai_write_register(ViPipe, 0x369d, 0x78);
	sc500ai_write_register(ViPipe, 0x36ea, 0x31);
	sc500ai_write_register(ViPipe, 0x36eb, 0x04);
	sc500ai_write_register(ViPipe, 0x36ec, 0x0a);
	sc500ai_write_register(ViPipe, 0x36ed, 0x24);
	sc500ai_write_register(ViPipe, 0x36fa, 0x31);
	sc500ai_write_register(ViPipe, 0x36fb, 0x04);
	sc500ai_write_register(ViPipe, 0x36fc, 0x00);
	sc500ai_write_register(ViPipe, 0x36fd, 0x26);
	sc500ai_write_register(ViPipe, 0x3904, 0x04);
	sc500ai_write_register(ViPipe, 0x3908, 0x41);
	sc500ai_write_register(ViPipe, 0x391f, 0x10);
	sc500ai_write_register(ViPipe, 0x39c2, 0x30);
	sc500ai_write_register(ViPipe, 0x3e00, 0x01);
	sc500ai_write_register(ViPipe, 0x3e01, 0x5e);
	sc500ai_write_register(ViPipe, 0x3e02, 0x00);
	sc500ai_write_register(ViPipe, 0x3e04, 0x15);
	sc500ai_write_register(ViPipe, 0x3e05, 0xe0);
	sc500ai_write_register(ViPipe, 0x3e23, 0x00);
	sc500ai_write_register(ViPipe, 0x3e24, 0xb4);
	sc500ai_write_register(ViPipe, 0x4500, 0x88);
	sc500ai_write_register(ViPipe, 0x4509, 0x20);
	sc500ai_write_register(ViPipe, 0x4800, 0x24);
	sc500ai_write_register(ViPipe, 0x4837, 0x18);
	sc500ai_write_register(ViPipe, 0x4853, 0xfd);
	sc500ai_write_register(ViPipe, 0x481b, 0x50);	// hs-trail
	sc500ai_write_register(ViPipe, 0x36e9, 0x40);
	sc500ai_write_register(ViPipe, 0x36f9, 0x40);
	sc500ai_init_ex(ViPipe);

	sc500ai_default_reg_init(ViPipe);

	sc500ai_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(50);

	printf("===SC500AI sensor 1440P30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}
