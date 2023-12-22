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
#include "sc223a_1L_cmos_ex.h"

#define SC223A_1L_CHIP_ID_HI_ADDR		0x3107
#define SC223A_1L_CHIP_ID_LO_ADDR		0x3108
#define SC223A_1L_CHIP_ID				0xcb3e

static void sc223a_1l_linear_1080p30_init(VI_PIPE ViPipe);

CVI_U8 sc223a_1l_i2c_addr = 0x30;        /* I2C Address of SC223A_1L */
const CVI_U32 sc223a_1l_addr_byte = 2;
const CVI_U32 sc223a_1l_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc223a_1l_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC223A_1L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc223a_1l_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc223a_1l_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc223a_1l_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	char buf[8];
	int idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc223a_1l_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc223a_1l_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc223a_1l_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc223a_1l_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc223a_1l_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc223a_1l_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc223a_1l_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc223a_1l_addr_byte + sc223a_1l_data_byte);
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

void sc223a_1l_standby(VI_PIPE ViPipe)
{
	sc223a_1l_write_register(ViPipe, 0x0100, 0x00); /* suspend */
}

void sc223a_1l_restart(VI_PIPE ViPipe)
{
	sc223a_1l_write_register(ViPipe, 0x0100, 0x01); /* resume */
}

void sc223a_1l_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC223A_1L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		sc223a_1l_write_register(ViPipe,
				g_pastSC223A_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC223A_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

void sc223a_1l_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc223a_1l_write_register(ViPipe, 0x3221, val);
}

int sc223a_1l_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc223a_1l_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc223a_1l_read_register(ViPipe, SC223A_1L_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc223a_1l_read_register(ViPipe, SC223A_1L_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC223A_1L_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}


void sc223a_1l_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;

	enWDRMode   = g_pastSC223A_1L[ViPipe]->enWDRMode;

	sc223a_1l_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_NONE) {
		sc223a_1l_linear_1080p30_init(ViPipe);
	} else {
		CVI_TRACE_SNS(CVI_DBG_ERR, "not surpport this WDR_MODE_E!\n");
	}

	g_pastSC223A_1L[ViPipe]->bInit = CVI_TRUE;
}

void sc223a_1l_exit(VI_PIPE ViPipe)
{
	sc223a_1l_i2c_exit(ViPipe);
}

static void sc223a_1l_linear_1080p30_init(VI_PIPE ViPipe)
{
	sc223a_1l_write_register(ViPipe, 0x0100, 0x00);
	sc223a_1l_write_register(ViPipe, 0x36e9, 0x80);
	sc223a_1l_write_register(ViPipe, 0x37f9, 0x80);
	sc223a_1l_write_register(ViPipe, 0x3018, 0x1a);
	sc223a_1l_write_register(ViPipe, 0x3019, 0x02);
	sc223a_1l_write_register(ViPipe, 0x301f, 0x02);
	sc223a_1l_write_register(ViPipe, 0x30b8, 0x44);
	sc223a_1l_write_register(ViPipe, 0x3253, 0x0c);
	sc223a_1l_write_register(ViPipe, 0x3281, 0x80);
	sc223a_1l_write_register(ViPipe, 0x3301, 0x06);
	sc223a_1l_write_register(ViPipe, 0x3302, 0x12);
	sc223a_1l_write_register(ViPipe, 0x3306, 0x80);
	sc223a_1l_write_register(ViPipe, 0x3309, 0x60);
	sc223a_1l_write_register(ViPipe, 0x330a, 0x00);
	sc223a_1l_write_register(ViPipe, 0x330b, 0xe0);
	sc223a_1l_write_register(ViPipe, 0x330d, 0x20);
	sc223a_1l_write_register(ViPipe, 0x3314, 0x15);
	sc223a_1l_write_register(ViPipe, 0x331e, 0x41);
	sc223a_1l_write_register(ViPipe, 0x331f, 0x51);
	sc223a_1l_write_register(ViPipe, 0x3320, 0x0a);
	sc223a_1l_write_register(ViPipe, 0x3326, 0x0e);
	sc223a_1l_write_register(ViPipe, 0x3333, 0x10);
	sc223a_1l_write_register(ViPipe, 0x3334, 0x40);
	sc223a_1l_write_register(ViPipe, 0x335d, 0x60);
	sc223a_1l_write_register(ViPipe, 0x335e, 0x06);
	sc223a_1l_write_register(ViPipe, 0x335f, 0x08);
	sc223a_1l_write_register(ViPipe, 0x3364, 0x56);
	sc223a_1l_write_register(ViPipe, 0x337a, 0x06);
	sc223a_1l_write_register(ViPipe, 0x337b, 0x0e);
	sc223a_1l_write_register(ViPipe, 0x337c, 0x02);
	sc223a_1l_write_register(ViPipe, 0x337d, 0x0a);
	sc223a_1l_write_register(ViPipe, 0x3390, 0x03);
	sc223a_1l_write_register(ViPipe, 0x3391, 0x0f);
	sc223a_1l_write_register(ViPipe, 0x3392, 0x1f);
	sc223a_1l_write_register(ViPipe, 0x3393, 0x06);
	sc223a_1l_write_register(ViPipe, 0x3394, 0x06);
	sc223a_1l_write_register(ViPipe, 0x3395, 0x06);
	sc223a_1l_write_register(ViPipe, 0x3396, 0x48);
	sc223a_1l_write_register(ViPipe, 0x3397, 0x4b);
	sc223a_1l_write_register(ViPipe, 0x3398, 0x5f);
	sc223a_1l_write_register(ViPipe, 0x3399, 0x06);
	sc223a_1l_write_register(ViPipe, 0x339a, 0x06);
	sc223a_1l_write_register(ViPipe, 0x339b, 0x92);
	sc223a_1l_write_register(ViPipe, 0x339c, 0x92);
	sc223a_1l_write_register(ViPipe, 0x33a2, 0x04);
	sc223a_1l_write_register(ViPipe, 0x33a3, 0x0a);
	sc223a_1l_write_register(ViPipe, 0x33ad, 0x1c);
	sc223a_1l_write_register(ViPipe, 0x33af, 0x40);
	sc223a_1l_write_register(ViPipe, 0x33b1, 0x80);
	sc223a_1l_write_register(ViPipe, 0x33b3, 0x20);
	sc223a_1l_write_register(ViPipe, 0x349f, 0x02);
	sc223a_1l_write_register(ViPipe, 0x34a6, 0x48);
	sc223a_1l_write_register(ViPipe, 0x34a7, 0x4b);
	sc223a_1l_write_register(ViPipe, 0x34a8, 0x20);
	sc223a_1l_write_register(ViPipe, 0x34a9, 0x20);
	sc223a_1l_write_register(ViPipe, 0x34f8, 0x5f);
	sc223a_1l_write_register(ViPipe, 0x34f9, 0x10);
	sc223a_1l_write_register(ViPipe, 0x3616, 0xac);
	sc223a_1l_write_register(ViPipe, 0x3630, 0xc0);
	sc223a_1l_write_register(ViPipe, 0x3631, 0x86);
	sc223a_1l_write_register(ViPipe, 0x3632, 0x26);
	sc223a_1l_write_register(ViPipe, 0x3633, 0x32);
	sc223a_1l_write_register(ViPipe, 0x3637, 0x29);
	sc223a_1l_write_register(ViPipe, 0x363a, 0x84);
	sc223a_1l_write_register(ViPipe, 0x363b, 0x04);
	sc223a_1l_write_register(ViPipe, 0x363c, 0x08);
	sc223a_1l_write_register(ViPipe, 0x3641, 0x3a);
	sc223a_1l_write_register(ViPipe, 0x364f, 0x39);
	sc223a_1l_write_register(ViPipe, 0x3670, 0xce);
	sc223a_1l_write_register(ViPipe, 0x3674, 0xc0);
	sc223a_1l_write_register(ViPipe, 0x3675, 0xc0);
	sc223a_1l_write_register(ViPipe, 0x3676, 0xc0);
	sc223a_1l_write_register(ViPipe, 0x3677, 0x86);
	sc223a_1l_write_register(ViPipe, 0x3678, 0x8b);
	sc223a_1l_write_register(ViPipe, 0x3679, 0x8c);
	sc223a_1l_write_register(ViPipe, 0x367c, 0x4b);
	sc223a_1l_write_register(ViPipe, 0x367d, 0x5f);
	sc223a_1l_write_register(ViPipe, 0x367e, 0x4b);
	sc223a_1l_write_register(ViPipe, 0x367f, 0x5f);
	sc223a_1l_write_register(ViPipe, 0x3690, 0x62);
	sc223a_1l_write_register(ViPipe, 0x3691, 0x63);
	sc223a_1l_write_register(ViPipe, 0x3692, 0x63);
	sc223a_1l_write_register(ViPipe, 0x3699, 0x86);
	sc223a_1l_write_register(ViPipe, 0x369a, 0x92);
	sc223a_1l_write_register(ViPipe, 0x369b, 0xa4);
	sc223a_1l_write_register(ViPipe, 0x369c, 0x48);
	sc223a_1l_write_register(ViPipe, 0x369d, 0x4b);
	sc223a_1l_write_register(ViPipe, 0x36a2, 0x4b);
	sc223a_1l_write_register(ViPipe, 0x36a3, 0x4f);
	sc223a_1l_write_register(ViPipe, 0x36ec, 0x0c);
	sc223a_1l_write_register(ViPipe, 0x370f, 0x01);
	sc223a_1l_write_register(ViPipe, 0x3721, 0x6c);
	sc223a_1l_write_register(ViPipe, 0x3722, 0x09);
	sc223a_1l_write_register(ViPipe, 0x3724, 0x41);
	sc223a_1l_write_register(ViPipe, 0x3725, 0xc4);
	sc223a_1l_write_register(ViPipe, 0x37b0, 0x09);
	sc223a_1l_write_register(ViPipe, 0x37b1, 0x09);
	sc223a_1l_write_register(ViPipe, 0x37b2, 0x09);
	sc223a_1l_write_register(ViPipe, 0x37b3, 0x48);
	sc223a_1l_write_register(ViPipe, 0x37b4, 0x5f);
	sc223a_1l_write_register(ViPipe, 0x3900, 0x19);
	sc223a_1l_write_register(ViPipe, 0x3901, 0x02);
	sc223a_1l_write_register(ViPipe, 0x3905, 0xb8);
	sc223a_1l_write_register(ViPipe, 0x391b, 0x82);
	sc223a_1l_write_register(ViPipe, 0x391c, 0x00);
	sc223a_1l_write_register(ViPipe, 0x391f, 0x04);
	sc223a_1l_write_register(ViPipe, 0x3933, 0x81);
	sc223a_1l_write_register(ViPipe, 0x3934, 0x4c);
	sc223a_1l_write_register(ViPipe, 0x393f, 0xff);
	sc223a_1l_write_register(ViPipe, 0x3940, 0x73);
	sc223a_1l_write_register(ViPipe, 0x3942, 0x01);
	sc223a_1l_write_register(ViPipe, 0x3943, 0x4d);
	sc223a_1l_write_register(ViPipe, 0x3946, 0x20);
	sc223a_1l_write_register(ViPipe, 0x3957, 0x86);
	sc223a_1l_write_register(ViPipe, 0x3e01, 0x8c);
	sc223a_1l_write_register(ViPipe, 0x3e28, 0xc4);
	sc223a_1l_write_register(ViPipe, 0x440e, 0x02);
	sc223a_1l_write_register(ViPipe, 0x4501, 0xc0);
	sc223a_1l_write_register(ViPipe, 0x4509, 0x14);
	sc223a_1l_write_register(ViPipe, 0x450d, 0x11);
	sc223a_1l_write_register(ViPipe, 0x4518, 0x00);
	sc223a_1l_write_register(ViPipe, 0x451b, 0x0a);
	sc223a_1l_write_register(ViPipe, 0x4819, 0x09);
	sc223a_1l_write_register(ViPipe, 0x481b, 0x05);
	sc223a_1l_write_register(ViPipe, 0x481d, 0x14);
	sc223a_1l_write_register(ViPipe, 0x481f, 0x04);
	sc223a_1l_write_register(ViPipe, 0x4821, 0x0a);
	sc223a_1l_write_register(ViPipe, 0x4823, 0x05);
	sc223a_1l_write_register(ViPipe, 0x4825, 0x04);
	sc223a_1l_write_register(ViPipe, 0x4827, 0x05);
	sc223a_1l_write_register(ViPipe, 0x4829, 0x08);
	sc223a_1l_write_register(ViPipe, 0x501c, 0x00);
	sc223a_1l_write_register(ViPipe, 0x501d, 0x60);
	sc223a_1l_write_register(ViPipe, 0x501e, 0x00);
	sc223a_1l_write_register(ViPipe, 0x501f, 0x40);
	sc223a_1l_write_register(ViPipe, 0x5799, 0x06);
	sc223a_1l_write_register(ViPipe, 0x5ae0, 0xfe);
	sc223a_1l_write_register(ViPipe, 0x5ae1, 0x40);
	sc223a_1l_write_register(ViPipe, 0x5ae2, 0x38);
	sc223a_1l_write_register(ViPipe, 0x5ae3, 0x30);
	sc223a_1l_write_register(ViPipe, 0x5ae4, 0x28);
	sc223a_1l_write_register(ViPipe, 0x5ae5, 0x38);
	sc223a_1l_write_register(ViPipe, 0x5ae6, 0x30);
	sc223a_1l_write_register(ViPipe, 0x5ae7, 0x28);
	sc223a_1l_write_register(ViPipe, 0x5ae8, 0x3f);
	sc223a_1l_write_register(ViPipe, 0x5ae9, 0x34);
	sc223a_1l_write_register(ViPipe, 0x5aea, 0x2c);
	sc223a_1l_write_register(ViPipe, 0x5aeb, 0x3f);
	sc223a_1l_write_register(ViPipe, 0x5aec, 0x34);
	sc223a_1l_write_register(ViPipe, 0x5aed, 0x2c);
	sc223a_1l_write_register(ViPipe, 0x5aee, 0xfe);
	sc223a_1l_write_register(ViPipe, 0x5aef, 0x40);
	sc223a_1l_write_register(ViPipe, 0x5af4, 0x38);
	sc223a_1l_write_register(ViPipe, 0x5af5, 0x30);
	sc223a_1l_write_register(ViPipe, 0x5af6, 0x28);
	sc223a_1l_write_register(ViPipe, 0x5af7, 0x38);
	sc223a_1l_write_register(ViPipe, 0x5af8, 0x30);
	sc223a_1l_write_register(ViPipe, 0x5af9, 0x28);
	sc223a_1l_write_register(ViPipe, 0x5afa, 0x3f);
	sc223a_1l_write_register(ViPipe, 0x5afb, 0x34);
	sc223a_1l_write_register(ViPipe, 0x5afc, 0x2c);
	sc223a_1l_write_register(ViPipe, 0x5afd, 0x3f);
	sc223a_1l_write_register(ViPipe, 0x5afe, 0x34);
	sc223a_1l_write_register(ViPipe, 0x5aff, 0x2c);
	sc223a_1l_write_register(ViPipe, 0x36e9, 0x20);
	sc223a_1l_write_register(ViPipe, 0x37f9, 0x27);
	// sc223a_1l_write_register(ViPipe, 0x0100, 0x01);

	sc223a_1l_default_reg_init(ViPipe);

	sc223a_1l_write_register(ViPipe, 0x0100, 0x01);
	delay_ms(50);
	printf("ViPipe:%d,===SC223A_1L 1080P 30fps 10bit LINE Init OK!===\n", ViPipe);
}
