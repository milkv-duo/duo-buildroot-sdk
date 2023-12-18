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
#include "cvi_sns_ctrl.h"
#include "sc2331_1L_cmos_ex.h"

#define SC2331_1L_CHIP_ID_HI_ADDR		0x3107
#define SC2331_1L_CHIP_ID_LO_ADDR		0x3108
#define SC2331_1L_CHIP_ID			0xcb5c

static void sc2331_1L_linear_1080p30_init(VI_PIPE ViPipe);

const CVI_U8 sc2331_1L_i2c_addr = 0x30;        /* I2C Address of SC2331_1L */
const CVI_U32 sc2331_1L_addr_byte = 2;
const CVI_U32 sc2331_1L_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int sc2331_1L_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunSC2331_1L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, sc2331_1L_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int sc2331_1L_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int sc2331_1L_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (sc2331_1L_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, sc2331_1L_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, sc2331_1L_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (sc2331_1L_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int sc2331_1L_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (sc2331_1L_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (sc2331_1L_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, sc2331_1L_addr_byte + sc2331_1L_data_byte);
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

void sc2331_1L_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			sc2331_1L_write_register(ViPipe, addr, data);
	}
}

void sc2331_1L_standby(VI_PIPE ViPipe)
{
	sc2331_1L_write_register(ViPipe, 0x0100, 0x00);
}

void sc2331_1L_restart(VI_PIPE ViPipe)
{
	sc2331_1L_write_register(ViPipe, 0x0100, 0x00);
	delay_ms(20);
	sc2331_1L_write_register(ViPipe, 0x0100, 0x01);
}

void sc2331_1L_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastSC2331_1L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		if (g_pastSC2331_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].bUpdate == CVI_TRUE) {
			sc2331_1L_write_register(ViPipe,
				g_pastSC2331_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastSC2331_1L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
		}
	}
}

void sc2331_1L_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
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

	sc2331_1L_write_register(ViPipe, 0x3221, val);
}


int sc2331_1L_probe(VI_PIPE ViPipe)
{
int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (sc2331_1L_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = sc2331_1L_read_register(ViPipe, SC2331_1L_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = sc2331_1L_read_register(ViPipe, SC2331_1L_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != SC2331_1L_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

// printf("======%d\n",ViPipe);
	return CVI_SUCCESS;
}



/* 1080P30 and 1080P25 */
static void sc2331_1L_linear_1080p30_init(VI_PIPE ViPipe)
{
	sc2331_1L_write_register(ViPipe, 0x0103, 0x01);
	sc2331_1L_write_register(ViPipe, 0x0100, 0x00);
	sc2331_1L_write_register(ViPipe, 0x36e9, 0x80);
	sc2331_1L_write_register(ViPipe, 0x37f9, 0x80);
	sc2331_1L_write_register(ViPipe, 0x3018, 0x1a);
	sc2331_1L_write_register(ViPipe, 0x3019, 0x0e);
	sc2331_1L_write_register(ViPipe, 0x301f, 0x20);
	printf("awei 20 hao 11:07 sc2331_1L\n");
	sc2331_1L_write_register(ViPipe, 0x3258, 0x0e);
	sc2331_1L_write_register(ViPipe, 0x3301, 0x06);
	sc2331_1L_write_register(ViPipe, 0x3302, 0x10);
	sc2331_1L_write_register(ViPipe, 0x3304, 0x68);
	sc2331_1L_write_register(ViPipe, 0x3306, 0x90);
	sc2331_1L_write_register(ViPipe, 0x3308, 0x18);
	sc2331_1L_write_register(ViPipe, 0x3309, 0x80);
	sc2331_1L_write_register(ViPipe, 0x330a, 0x01);
	sc2331_1L_write_register(ViPipe, 0x330b, 0x48);
	sc2331_1L_write_register(ViPipe, 0x330d, 0x18);
	sc2331_1L_write_register(ViPipe, 0x331c, 0x02);
	sc2331_1L_write_register(ViPipe, 0x331e, 0x59);
	sc2331_1L_write_register(ViPipe, 0x331f, 0x71);
	sc2331_1L_write_register(ViPipe, 0x3333, 0x10);
	sc2331_1L_write_register(ViPipe, 0x3334, 0x40);
	sc2331_1L_write_register(ViPipe, 0x3364, 0x56);
	sc2331_1L_write_register(ViPipe, 0x3390, 0x08);
	sc2331_1L_write_register(ViPipe, 0x3391, 0x09);
	sc2331_1L_write_register(ViPipe, 0x3392, 0x0b);
	sc2331_1L_write_register(ViPipe, 0x3393, 0x0a);
	sc2331_1L_write_register(ViPipe, 0x3394, 0x2a);
	sc2331_1L_write_register(ViPipe, 0x3395, 0x2a);
	sc2331_1L_write_register(ViPipe, 0x3396, 0x48);
	sc2331_1L_write_register(ViPipe, 0x3397, 0x49);
	sc2331_1L_write_register(ViPipe, 0x3398, 0x4b);
	sc2331_1L_write_register(ViPipe, 0x3399, 0x06);
	sc2331_1L_write_register(ViPipe, 0x339a, 0x0a);
	sc2331_1L_write_register(ViPipe, 0x339b, 0x30);
	sc2331_1L_write_register(ViPipe, 0x339c, 0x48);
	sc2331_1L_write_register(ViPipe, 0x33ad, 0x2c);
	sc2331_1L_write_register(ViPipe, 0x33ae, 0x38);
	sc2331_1L_write_register(ViPipe, 0x33b3, 0x40);
	sc2331_1L_write_register(ViPipe, 0x349f, 0x02);
	sc2331_1L_write_register(ViPipe, 0x34a6, 0x09);
	sc2331_1L_write_register(ViPipe, 0x34a7, 0x0f);
	sc2331_1L_write_register(ViPipe, 0x34a8, 0x30);
	sc2331_1L_write_register(ViPipe, 0x34a9, 0x28);
	sc2331_1L_write_register(ViPipe, 0x34f8, 0x5f);
	sc2331_1L_write_register(ViPipe, 0x34f9, 0x28);
	sc2331_1L_write_register(ViPipe, 0x3630, 0xc6);
	sc2331_1L_write_register(ViPipe, 0x3633, 0x33);
	sc2331_1L_write_register(ViPipe, 0x3637, 0x6b);
	sc2331_1L_write_register(ViPipe, 0x363c, 0xc1);
	sc2331_1L_write_register(ViPipe, 0x363e, 0xc2);
	sc2331_1L_write_register(ViPipe, 0x3670, 0x2e);
	sc2331_1L_write_register(ViPipe, 0x3674, 0xc5);
	sc2331_1L_write_register(ViPipe, 0x3675, 0xc7);
	sc2331_1L_write_register(ViPipe, 0x3676, 0xcb);
	sc2331_1L_write_register(ViPipe, 0x3677, 0x44);
	sc2331_1L_write_register(ViPipe, 0x3678, 0x48);
	sc2331_1L_write_register(ViPipe, 0x3679, 0x48);
	sc2331_1L_write_register(ViPipe, 0x367c, 0x08);
	sc2331_1L_write_register(ViPipe, 0x367d, 0x0b);
	sc2331_1L_write_register(ViPipe, 0x367e, 0x0b);
	sc2331_1L_write_register(ViPipe, 0x367f, 0x0f);
	sc2331_1L_write_register(ViPipe, 0x3690, 0x33);
	sc2331_1L_write_register(ViPipe, 0x3691, 0x33);
	sc2331_1L_write_register(ViPipe, 0x3692, 0x33);
	sc2331_1L_write_register(ViPipe, 0x3693, 0x84);
	sc2331_1L_write_register(ViPipe, 0x3694, 0x85);
	sc2331_1L_write_register(ViPipe, 0x3695, 0x8d);
	sc2331_1L_write_register(ViPipe, 0x3696, 0x9c);
	sc2331_1L_write_register(ViPipe, 0x369c, 0x0b);
	sc2331_1L_write_register(ViPipe, 0x369d, 0x0f);
	sc2331_1L_write_register(ViPipe, 0x369e, 0x09);
	sc2331_1L_write_register(ViPipe, 0x369f, 0x0b);
	sc2331_1L_write_register(ViPipe, 0x36a0, 0x0f);
	sc2331_1L_write_register(ViPipe, 0x36ec, 0x0c);
	sc2331_1L_write_register(ViPipe, 0x370f, 0x01);
	sc2331_1L_write_register(ViPipe, 0x3722, 0x05);
	sc2331_1L_write_register(ViPipe, 0x3724, 0x20);
	sc2331_1L_write_register(ViPipe, 0x3725, 0x91);
	sc2331_1L_write_register(ViPipe, 0x3771, 0x05);
	sc2331_1L_write_register(ViPipe, 0x3772, 0x05);
	sc2331_1L_write_register(ViPipe, 0x3773, 0x05);
	sc2331_1L_write_register(ViPipe, 0x377a, 0x0b);
	sc2331_1L_write_register(ViPipe, 0x377b, 0x0f);
	sc2331_1L_write_register(ViPipe, 0x3900, 0x19);
	sc2331_1L_write_register(ViPipe, 0x3905, 0xb8);
	sc2331_1L_write_register(ViPipe, 0x391b, 0x80);
	sc2331_1L_write_register(ViPipe, 0x391c, 0x04);
	sc2331_1L_write_register(ViPipe, 0x391d, 0x81);
	sc2331_1L_write_register(ViPipe, 0x3933, 0xc0);
	sc2331_1L_write_register(ViPipe, 0x3934, 0x08);
	sc2331_1L_write_register(ViPipe, 0x3940, 0x72);
	sc2331_1L_write_register(ViPipe, 0x3941, 0x00);
	sc2331_1L_write_register(ViPipe, 0x3942, 0x00);
	sc2331_1L_write_register(ViPipe, 0x3943, 0x09);
	sc2331_1L_write_register(ViPipe, 0x3946, 0x10);
	sc2331_1L_write_register(ViPipe, 0x3957, 0x86);
	sc2331_1L_write_register(ViPipe, 0x3e01, 0x8b);
	sc2331_1L_write_register(ViPipe, 0x3e02, 0xd0);
	sc2331_1L_write_register(ViPipe, 0x3e08, 0x00);
	sc2331_1L_write_register(ViPipe, 0x440e, 0x02);
	sc2331_1L_write_register(ViPipe, 0x4509, 0x28);
	sc2331_1L_write_register(ViPipe, 0x450d, 0x10);
	sc2331_1L_write_register(ViPipe, 0x4819, 0x09);
	sc2331_1L_write_register(ViPipe, 0x481b, 0x05);
	sc2331_1L_write_register(ViPipe, 0x481d, 0x14);
	sc2331_1L_write_register(ViPipe, 0x481f, 0x04);
	sc2331_1L_write_register(ViPipe, 0x4821, 0x0a);
	sc2331_1L_write_register(ViPipe, 0x4823, 0x05);
	sc2331_1L_write_register(ViPipe, 0x4825, 0x04);
	sc2331_1L_write_register(ViPipe, 0x4827, 0x05);
	sc2331_1L_write_register(ViPipe, 0x4829, 0x08);
	sc2331_1L_write_register(ViPipe, 0x5780, 0x66);
	sc2331_1L_write_register(ViPipe, 0x578d, 0x40);
	sc2331_1L_write_register(ViPipe, 0x5799, 0x06);
	sc2331_1L_write_register(ViPipe, 0x36e9, 0x20);
	sc2331_1L_write_register(ViPipe, 0x37f9, 0x27);

	sc2331_1L_default_reg_init(ViPipe);

	sc2331_1L_write_register(ViPipe, 0x0100, 0x01);

	// printf("ViPipe:%d,===SC2331_1L 1080P 30fps 10bit LINE Init OK!===\n", ViPipe);
}

void sc2331_1L_init(VI_PIPE ViPipe)
{
	sc2331_1L_i2c_init(ViPipe);

	//linear mode only
	sc2331_1L_linear_1080p30_init(ViPipe);

	g_pastSC2331_1L[ViPipe]->bInit = CVI_TRUE;
}

void sc2331_1L_exit(VI_PIPE ViPipe)
{
	sc2331_1L_i2c_exit(ViPipe);
}