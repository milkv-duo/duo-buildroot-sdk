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
#include "sc5336_2L_cmos_ex.h"

static void SC5336_2l_linear_1620p30_init(VI_PIPE ViPipe);

const CVI_U8 SC5336_2l_i2c_addr = 0x30;        /* I2C Address of SC5336_2L */
const CVI_U32 SC5336_2l_addr_byte = 2;
const CVI_U32 SC5336_2l_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int SC5336_2l_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC5336_2L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, SC5336_2l_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int SC5336_2l_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int SC5336_2l_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (SC5336_2l_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, SC5336_2l_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, SC5336_2l_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (SC5336_2l_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int SC5336_2l_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (SC5336_2l_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (SC5336_2l_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, SC5336_2l_addr_byte + SC5336_2l_data_byte);
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

void SC5336_2l_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			SC5336_2l_write_register(ViPipe, addr, data);
	}
}

void SC5336_2l_standby(VI_PIPE ViPipe)
{
	SC5336_2l_write_register(ViPipe, 0x0100, 0x00);
}

void SC5336_2l_restart(VI_PIPE ViPipe)
{
	SC5336_2l_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	SC5336_2l_write_register(ViPipe, 0x0100, 0x01);
}

void SC5336_2l_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC5336_2L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		SC5336_2l_write_register(ViPipe,
				g_pastSC5336_2L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC5336_2L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define SC5336_2L_CHIP_ID_HI_ADDR		0x3107
#define SC5336_2L_CHIP_ID_LO_ADDR		0x3108
#define SC5336_2L_CHIP_ID				0xce50

void SC5336_2l_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	SC5336_2l_write_register(ViPipe, 0x3221, val);
}

int SC5336_2l_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (SC5336_2l_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = SC5336_2l_read_register(ViPipe, SC5336_2L_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = SC5336_2l_read_register(ViPipe, SC5336_2L_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC5336_2L_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void SC5336_2l_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_BOOL          bInit;

	bInit       = g_pastSC5336_2L[ViPipe]->bInit;
	enWDRMode   = g_pastSC5336_2L[ViPipe]->enWDRMode;

	SC5336_2l_i2c_init(ViPipe);

	/* When sensor first init, config all registers */
	if (bInit == CVI_FALSE) {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "not surpport this WDR_MODE_E!\n");
		} else {
			SC5336_2l_linear_1620p30_init(ViPipe);
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			CVI_TRACE_SNS(CVI_DBG_ERR, "not surpport this WDR_MODE_E!\n");
		} else {
			SC5336_2l_linear_1620p30_init(ViPipe);
		}
	}
	g_pastSC5336_2L[ViPipe]->bInit = CVI_TRUE;
}

void SC5336_2l_exit(VI_PIPE ViPipe)
{
	SC5336_2l_i2c_exit(ViPipe);
}

/* 1620P30 and 1620P25 */
static void SC5336_2l_linear_1620p30_init(VI_PIPE ViPipe)
{
	SC5336_2l_write_register(ViPipe, 0x0103, 0x01);
	SC5336_2l_write_register(ViPipe, 0x36e9, 0x80);
	SC5336_2l_write_register(ViPipe, 0x37f9, 0x80);

	if (0x00 == SC5336_2l_read_register(ViPipe, 0x3040)) {
		// 2880 * 1618
		SC5336_2l_write_register(ViPipe, 0x301f, 0x03);
		SC5336_2l_write_register(ViPipe, 0x3207, 0x5b);
		SC5336_2l_write_register(ViPipe, 0x320b, 0x52);
	} else if (0x03 == SC5336_2l_read_register(ViPipe, 0x3040)) {
		// 2880 * 1620
		SC5336_2l_write_register(ViPipe, 0x301f, 0x01);
	}

	SC5336_2l_write_register(ViPipe, 0x320e, 0x07);
	SC5336_2l_write_register(ViPipe, 0x320f, 0x08);
	SC5336_2l_write_register(ViPipe, 0x3213, 0x04);
	SC5336_2l_write_register(ViPipe, 0x3241, 0x00);
	SC5336_2l_write_register(ViPipe, 0x3243, 0x01);
	SC5336_2l_write_register(ViPipe, 0x3248, 0x02);
	SC5336_2l_write_register(ViPipe, 0x3249, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x3253, 0x10);
	SC5336_2l_write_register(ViPipe, 0x3258, 0x0c);
	SC5336_2l_write_register(ViPipe, 0x3301, 0x0a);
	SC5336_2l_write_register(ViPipe, 0x3305, 0x00);
	SC5336_2l_write_register(ViPipe, 0x3306, 0x58);
	SC5336_2l_write_register(ViPipe, 0x3308, 0x08);
	SC5336_2l_write_register(ViPipe, 0x3309, 0xb0);
	SC5336_2l_write_register(ViPipe, 0x330a, 0x00);
	SC5336_2l_write_register(ViPipe, 0x330b, 0xc8);
	SC5336_2l_write_register(ViPipe, 0x3314, 0x14);
	SC5336_2l_write_register(ViPipe, 0x331f, 0xa1);
	SC5336_2l_write_register(ViPipe, 0x3321, 0x10);
	SC5336_2l_write_register(ViPipe, 0x3327, 0x14);
	SC5336_2l_write_register(ViPipe, 0x3328, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x3329, 0x0e);
	SC5336_2l_write_register(ViPipe, 0x3333, 0x10);
	SC5336_2l_write_register(ViPipe, 0x3334, 0x40);
	SC5336_2l_write_register(ViPipe, 0x3356, 0x10);
	SC5336_2l_write_register(ViPipe, 0x3364, 0x5e);
	SC5336_2l_write_register(ViPipe, 0x338f, 0x80);
	SC5336_2l_write_register(ViPipe, 0x3390, 0x09);
	SC5336_2l_write_register(ViPipe, 0x3391, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x3392, 0x0f);
	SC5336_2l_write_register(ViPipe, 0x3393, 0x10);
	SC5336_2l_write_register(ViPipe, 0x3394, 0x16);
	SC5336_2l_write_register(ViPipe, 0x3395, 0x98);
	SC5336_2l_write_register(ViPipe, 0x3396, 0x08);
	SC5336_2l_write_register(ViPipe, 0x3397, 0x09);
	SC5336_2l_write_register(ViPipe, 0x3398, 0x0f);
	SC5336_2l_write_register(ViPipe, 0x3399, 0x0a);
	SC5336_2l_write_register(ViPipe, 0x339a, 0x18);
	SC5336_2l_write_register(ViPipe, 0x339b, 0x60);
	SC5336_2l_write_register(ViPipe, 0x339c, 0xff);
	SC5336_2l_write_register(ViPipe, 0x33ad, 0x0c);
	SC5336_2l_write_register(ViPipe, 0x33ae, 0x5c);
	SC5336_2l_write_register(ViPipe, 0x33af, 0x52);
	SC5336_2l_write_register(ViPipe, 0x33b1, 0xa0);
	SC5336_2l_write_register(ViPipe, 0x33b2, 0x38);
	SC5336_2l_write_register(ViPipe, 0x33b3, 0x18);
	SC5336_2l_write_register(ViPipe, 0x33f8, 0x00);
	SC5336_2l_write_register(ViPipe, 0x33f9, 0x60);
	SC5336_2l_write_register(ViPipe, 0x33fa, 0x00);
	SC5336_2l_write_register(ViPipe, 0x33fb, 0x80);
	SC5336_2l_write_register(ViPipe, 0x33fc, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x33fd, 0x1f);
	SC5336_2l_write_register(ViPipe, 0x349f, 0x03);
	SC5336_2l_write_register(ViPipe, 0x34a6, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x34a7, 0x1f);
	SC5336_2l_write_register(ViPipe, 0x34a8, 0x08);
	SC5336_2l_write_register(ViPipe, 0x34a9, 0x08);
	SC5336_2l_write_register(ViPipe, 0x34aa, 0x00);
	SC5336_2l_write_register(ViPipe, 0x34ab, 0xd0);
	SC5336_2l_write_register(ViPipe, 0x34ac, 0x00);
	SC5336_2l_write_register(ViPipe, 0x34ad, 0xf0);
	SC5336_2l_write_register(ViPipe, 0x34f8, 0x3f);
	SC5336_2l_write_register(ViPipe, 0x34f9, 0x08);
	SC5336_2l_write_register(ViPipe, 0x3630, 0xc0);
	SC5336_2l_write_register(ViPipe, 0x3631, 0x83);
	SC5336_2l_write_register(ViPipe, 0x3632, 0x54);
	SC5336_2l_write_register(ViPipe, 0x3633, 0x33);
	SC5336_2l_write_register(ViPipe, 0x3638, 0xcf);
	SC5336_2l_write_register(ViPipe, 0x363f, 0xc0);
	SC5336_2l_write_register(ViPipe, 0x3641, 0x38);
	SC5336_2l_write_register(ViPipe, 0x3670, 0x56);
	SC5336_2l_write_register(ViPipe, 0x3674, 0xc0);
	SC5336_2l_write_register(ViPipe, 0x3675, 0xa0);
	SC5336_2l_write_register(ViPipe, 0x3676, 0xa0);
	SC5336_2l_write_register(ViPipe, 0x3677, 0x83);
	SC5336_2l_write_register(ViPipe, 0x3678, 0x86);
	SC5336_2l_write_register(ViPipe, 0x3679, 0x8a);
	SC5336_2l_write_register(ViPipe, 0x367c, 0x08);
	SC5336_2l_write_register(ViPipe, 0x367d, 0x0f);
	SC5336_2l_write_register(ViPipe, 0x367e, 0x08);
	SC5336_2l_write_register(ViPipe, 0x367f, 0x0f);
	SC5336_2l_write_register(ViPipe, 0x3696, 0x23);
	SC5336_2l_write_register(ViPipe, 0x3697, 0x33);
	SC5336_2l_write_register(ViPipe, 0x3698, 0x34);
	SC5336_2l_write_register(ViPipe, 0x36a0, 0x09);
	SC5336_2l_write_register(ViPipe, 0x36a1, 0x0f);
	SC5336_2l_write_register(ViPipe, 0x36b0, 0x85);
	SC5336_2l_write_register(ViPipe, 0x36b1, 0x8a);
	SC5336_2l_write_register(ViPipe, 0x36b2, 0x95);
	SC5336_2l_write_register(ViPipe, 0x36b3, 0xa6);
	SC5336_2l_write_register(ViPipe, 0x36b4, 0x09);
	SC5336_2l_write_register(ViPipe, 0x36b5, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x36b6, 0x0f);
	SC5336_2l_write_register(ViPipe, 0x36ea, 0x0c);
	SC5336_2l_write_register(ViPipe, 0x370f, 0x01);
	SC5336_2l_write_register(ViPipe, 0x3721, 0x6c);
	SC5336_2l_write_register(ViPipe, 0x3722, 0x89);
	SC5336_2l_write_register(ViPipe, 0x3724, 0x21);
	SC5336_2l_write_register(ViPipe, 0x3725, 0xb4);
	SC5336_2l_write_register(ViPipe, 0x3727, 0x14);
	SC5336_2l_write_register(ViPipe, 0x3771, 0x89);
	SC5336_2l_write_register(ViPipe, 0x3772, 0x89);
	SC5336_2l_write_register(ViPipe, 0x3773, 0xc5);
	SC5336_2l_write_register(ViPipe, 0x377a, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x377b, 0x1f);
	SC5336_2l_write_register(ViPipe, 0x37fa, 0x0c);
	SC5336_2l_write_register(ViPipe, 0x3900, 0x0d);
	SC5336_2l_write_register(ViPipe, 0x3901, 0x00);
	SC5336_2l_write_register(ViPipe, 0x3904, 0x04);
	SC5336_2l_write_register(ViPipe, 0x3905, 0x8c);
	SC5336_2l_write_register(ViPipe, 0x391d, 0x04);
	SC5336_2l_write_register(ViPipe, 0x391f, 0x49);
	SC5336_2l_write_register(ViPipe, 0x3926, 0x21);
	SC5336_2l_write_register(ViPipe, 0x3933, 0x80);
	SC5336_2l_write_register(ViPipe, 0x3934, 0x0a);
	SC5336_2l_write_register(ViPipe, 0x3935, 0x00);
	SC5336_2l_write_register(ViPipe, 0x3936, 0xff);
	SC5336_2l_write_register(ViPipe, 0x3937, 0x75);
	SC5336_2l_write_register(ViPipe, 0x3938, 0x74);
	SC5336_2l_write_register(ViPipe, 0x393c, 0x1e);
	SC5336_2l_write_register(ViPipe, 0x39dc, 0x02);
	SC5336_2l_write_register(ViPipe, 0x3e00, 0x00);
	SC5336_2l_write_register(ViPipe, 0x3e01, 0x70);
	SC5336_2l_write_register(ViPipe, 0x3e02, 0x00);
	SC5336_2l_write_register(ViPipe, 0x3e09, 0x00);
	SC5336_2l_write_register(ViPipe, 0x440d, 0x10);
	SC5336_2l_write_register(ViPipe, 0x440e, 0x02);
	SC5336_2l_write_register(ViPipe, 0x450d, 0x18);
	SC5336_2l_write_register(ViPipe, 0x4819, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x481b, 0x06);
	SC5336_2l_write_register(ViPipe, 0x481d, 0x17);
	SC5336_2l_write_register(ViPipe, 0x481f, 0x05);
	SC5336_2l_write_register(ViPipe, 0x4821, 0x0b);
	SC5336_2l_write_register(ViPipe, 0x4823, 0x06);
	SC5336_2l_write_register(ViPipe, 0x4825, 0x05);
	SC5336_2l_write_register(ViPipe, 0x4827, 0x05);
	SC5336_2l_write_register(ViPipe, 0x4829, 0x09);
	SC5336_2l_write_register(ViPipe, 0x5780, 0x66);
	SC5336_2l_write_register(ViPipe, 0x5787, 0x08);
	SC5336_2l_write_register(ViPipe, 0x5788, 0x03);
	SC5336_2l_write_register(ViPipe, 0x5789, 0x00);
	SC5336_2l_write_register(ViPipe, 0x578a, 0x08);
	SC5336_2l_write_register(ViPipe, 0x578b, 0x03);
	SC5336_2l_write_register(ViPipe, 0x578c, 0x00);
	SC5336_2l_write_register(ViPipe, 0x578d, 0x40);
	SC5336_2l_write_register(ViPipe, 0x5790, 0x08);
	SC5336_2l_write_register(ViPipe, 0x5791, 0x04);
	SC5336_2l_write_register(ViPipe, 0x5792, 0x01);
	SC5336_2l_write_register(ViPipe, 0x5793, 0x08);
	SC5336_2l_write_register(ViPipe, 0x5794, 0x04);
	SC5336_2l_write_register(ViPipe, 0x5795, 0x01);
	SC5336_2l_write_register(ViPipe, 0x5799, 0x46);
	SC5336_2l_write_register(ViPipe, 0x57aa, 0x2a);
	SC5336_2l_write_register(ViPipe, 0x5ae0, 0xfe);
	SC5336_2l_write_register(ViPipe, 0x5ae1, 0x40);
	SC5336_2l_write_register(ViPipe, 0x5ae2, 0x38);
	SC5336_2l_write_register(ViPipe, 0x5ae3, 0x30);
	SC5336_2l_write_register(ViPipe, 0x5ae4, 0x0c);
	SC5336_2l_write_register(ViPipe, 0x5ae5, 0x38);
	SC5336_2l_write_register(ViPipe, 0x5ae6, 0x30);
	SC5336_2l_write_register(ViPipe, 0x5ae7, 0x28);
	SC5336_2l_write_register(ViPipe, 0x5ae8, 0x3f);
	SC5336_2l_write_register(ViPipe, 0x5ae9, 0x34);
	SC5336_2l_write_register(ViPipe, 0x5aea, 0x2c);
	SC5336_2l_write_register(ViPipe, 0x5aeb, 0x3f);
	SC5336_2l_write_register(ViPipe, 0x5aec, 0x34);
	SC5336_2l_write_register(ViPipe, 0x5aed, 0x2c);
	SC5336_2l_write_register(ViPipe, 0x36e9, 0x44);
	SC5336_2l_write_register(ViPipe, 0x37f9, 0x44);
	if (0x00 == SC5336_2l_read_register(ViPipe, 0x3040)) {
		SC5336_2l_write_register(ViPipe, 0x3258, 0x0c);
		SC5336_2l_write_register(ViPipe, 0x3249, 0x0b);
		SC5336_2l_write_register(ViPipe, 0x3934, 0x0a);
		SC5336_2l_write_register(ViPipe, 0x3935, 0x00);
		SC5336_2l_write_register(ViPipe, 0x3937, 0x75);
	} else if (0x03 == SC5336_2l_read_register(ViPipe, 0x3040)) {
		SC5336_2l_write_register(ViPipe, 0x3258, 0x08);
		SC5336_2l_write_register(ViPipe, 0x3249, 0x07);
		SC5336_2l_write_register(ViPipe, 0x3934, 0x05);
		SC5336_2l_write_register(ViPipe, 0x3935, 0x07);
		SC5336_2l_write_register(ViPipe, 0x3937, 0x74);
	}
	SC5336_2l_write_register(ViPipe, 0x0100, 0x01);
	SC5336_2l_default_reg_init(ViPipe);

	SC5336_2l_write_register(ViPipe, 0x0100, 0x01);

	if (0x00 == SC5336_2l_read_register(ViPipe, 0x3040)) {
		printf("ViPipe:%d,===SC5336_2L 1618P 30fps 10bit LINE Init OK!===\n", ViPipe);
	} else if (0x03 == SC5336_2l_read_register(ViPipe, 0x3040)) {
		printf("ViPipe:%d,===SC5336_2L 1620P 30fps 10bit LINE Init OK!===\n", ViPipe);
	}
}
