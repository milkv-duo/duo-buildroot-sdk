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
#include "cv4001_cmos_ex.h"

#define CV4001_CHIP_ID_ADDR_H	0x3003
#define CV4001_CHIP_ID_ADDR_L	0x3002
#define CV4001_CHIP_ID			0x4001

static void cv4001_linear_1440p25_init(VI_PIPE ViPipe);
static void cv4001_wdr_1440p15_2to1_init(VI_PIPE ViPipe);

CVI_U8 cv4001_i2c_addr = 0x35;
const CVI_U32 cv4001_addr_byte = 2;
const CVI_U32 cv4001_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int cv4001_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunCV4001_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, cv4001_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int cv4001_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int cv4001_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (cv4001_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, cv4001_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, cv4001_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (cv4001_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int cv4001_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (cv4001_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}
	if (cv4001_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, cv4001_addr_byte + cv4001_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	ret = read(g_fd[ViPipe], buf, cv4001_addr_byte + cv4001_data_byte);
	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

void cv4001_standby(VI_PIPE ViPipe)
{
	cv4001_write_register(ViPipe, 0x3000, 0x1);

	printf("%s\n", __func__);
}

void cv4001_restart(VI_PIPE ViPipe)
{
	cv4001_write_register(ViPipe, 0x3000, 0x01);
	delay_ms(20);
	cv4001_write_register(ViPipe, 0x3000, 0x00);

	printf("%s\n", __func__);
}

void cv4001_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastCV4001[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		cv4001_write_register(ViPipe,
				g_pastCV4001[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastCV4001[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

int cv4001_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	usleep(50);
	if (cv4001_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = cv4001_read_register(ViPipe, CV4001_CHIP_ID_ADDR_H);
	nVal2 = cv4001_read_register(ViPipe, CV4001_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != CV4001_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void cv4001_init(VI_PIPE ViPipe)
{
	WDR_MODE_E        enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastCV4001[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastCV4001[ViPipe]->u8ImgMode;

	cv4001_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == CV4001_MODE_2560X1440P15_WDR) {
			cv4001_wdr_1440p15_2to1_init(ViPipe);
		}
	} else {
		if (u8ImgMode == CV4001_MODE_2560X1440P25) {
			cv4001_linear_1440p25_init(ViPipe);
		}
	}

	g_pastCV4001[ViPipe]->bInit = CVI_TRUE;
}

void cv4001_exit(VI_PIPE ViPipe)
{
	cv4001_i2c_exit(ViPipe);
}

static void cv4001_linear_1440p25_init(VI_PIPE ViPipe)
{
	delay_ms(10);

	//25fps 2lane
	cv4001_write_register(ViPipe, 0x3028, 0xB8);
	cv4001_write_register(ViPipe, 0x3029, 0x0B);
	cv4001_write_register(ViPipe, 0x302C, 0xE4);
	cv4001_write_register(ViPipe, 0x302D, 0x02);
	cv4001_write_register(ViPipe, 0x3048, 0x40);
	cv4001_write_register(ViPipe, 0x3049, 0x00);
	cv4001_write_register(ViPipe, 0x304A, 0x08);
	cv4001_write_register(ViPipe, 0x304B, 0x0A);
	cv4001_write_register(ViPipe, 0x3054, 0x28);
	cv4001_write_register(ViPipe, 0x3055, 0x00);
	cv4001_write_register(ViPipe, 0x3056, 0xA8);
	cv4001_write_register(ViPipe, 0x3057, 0x05);
	cv4001_write_register(ViPipe, 0x3020, 0x04);
	cv4001_write_register(ViPipe, 0x3908, 0x4A);
	cv4001_write_register(ViPipe, 0x3306, 0x03);
	cv4001_write_register(ViPipe, 0x343E, 0x00);
	cv4001_write_register(ViPipe, 0x3401, 0x01);
	cv4001_write_register(ViPipe, 0x3035, 0x01);
	cv4001_write_register(ViPipe, 0x3036, 0x01);
	cv4001_write_register(ViPipe, 0x343C, 0x01);
	cv4001_write_register(ViPipe, 0x362A, 0x00);
	cv4001_write_register(ViPipe, 0x3625, 0x01);
	cv4001_write_register(ViPipe, 0x35A4, 0x09);
	cv4001_write_register(ViPipe, 0x35A8, 0x09);
	cv4001_write_register(ViPipe, 0x35AE, 0x07);
	cv4001_write_register(ViPipe, 0x35AF, 0x07);
	cv4001_write_register(ViPipe, 0x34A2, 0x2C);
	cv4001_write_register(ViPipe, 0x3418, 0x9F);
	cv4001_write_register(ViPipe, 0x341A, 0x57);
	cv4001_write_register(ViPipe, 0x341C, 0x57);
	cv4001_write_register(ViPipe, 0x341E, 0x6F);
	cv4001_write_register(ViPipe, 0x341F, 0x01);
	cv4001_write_register(ViPipe, 0x3420, 0x57);
	cv4001_write_register(ViPipe, 0x3422, 0x9F);
	cv4001_write_register(ViPipe, 0x3424, 0x57);
	cv4001_write_register(ViPipe, 0x3426, 0x8F);
	cv4001_write_register(ViPipe, 0x3428, 0x47);
	cv4001_write_register(ViPipe, 0x3348, 0x00);
	cv4001_write_register(ViPipe, 0x3000, 0x00);
	cv4001_write_register(ViPipe, 0x3576, 0x06);
	cv4001_write_register(ViPipe, 0x350F, 0x18);
	cv4001_write_register(ViPipe, 0x3513, 0x07);
	cv4001_write_register(ViPipe, 0x3517, 0x07);
	cv4001_write_register(ViPipe, 0x351A, 0x05);
	cv4001_write_register(ViPipe, 0x351E, 0x0B);
	cv4001_write_register(ViPipe, 0x357A, 0x0B);
	cv4001_write_register(ViPipe, 0x3348, 0x00);
	cv4001_write_register(ViPipe, 0x316C, 0x64);
	cv4001_write_register(ViPipe, 0x3258, 0x02);
	cv4001_write_register(ViPipe, 0x3162, 0x01);
	cv4001_write_register(ViPipe, 0x3347, 0x01);
	cv4001_write_register(ViPipe, 0x3804, 0x0F);
	cv4001_write_register(ViPipe, 0x3871, 0x00);
	cv4001_write_register(ViPipe, 0x3244, 0x08);
	cv4001_write_register(ViPipe, 0x3270, 0x60);
	cv4001_write_register(ViPipe, 0x3271, 0x00);
	cv4001_write_register(ViPipe, 0x3272, 0x00);
	cv4001_write_register(ViPipe, 0x31AC, 0xC8);
	cv4001_write_register(ViPipe, 0x3890, 0x00);
	cv4001_write_register(ViPipe, 0x3894, 0x05);
	cv4001_write_register(ViPipe, 0x3690, 0x00);
	cv4001_write_register(ViPipe, 0x3898, 0x20);
	cv4001_write_register(ViPipe, 0x3899, 0x20);
	cv4001_write_register(ViPipe, 0x3583, 0x2f);
	cv4001_write_register(ViPipe, 0x3b75, 0x00);
	cv4001_write_register(ViPipe, 0x3b5E, 0x01);
	cv4001_write_register(ViPipe, 0x3a10, 0x06);
	cv4001_write_register(ViPipe, 0x3a11, 0x06);
	cv4001_write_register(ViPipe, 0x316C, 0x64);
	cv4001_write_register(ViPipe, 0x3000, 0x00);


	cv4001_default_reg_init(ViPipe);
	delay_ms(100);

	printf("ViPipe:%d,===CV4001 1440P 25fps 12bit LINEAR Init OK!===\n", ViPipe);
}

static void cv4001_wdr_1440p15_2to1_init(VI_PIPE ViPipe)
{
	cv4001_write_register(ViPipe, 0x3028, 0x08);
	cv4001_write_register(ViPipe, 0x3029, 0x17);
	cv4001_write_register(ViPipe, 0x302C, 0xF8);
	cv4001_write_register(ViPipe, 0x302D, 0x04);
	cv4001_write_register(ViPipe, 0x3908, 0x4B);
	cv4001_write_register(ViPipe, 0x3304, 0x01);
	cv4001_write_register(ViPipe, 0x3305, 0x02);
	cv4001_write_register(ViPipe, 0x3306, 0x01);
	cv4001_write_register(ViPipe, 0x343E, 0x00);
	cv4001_write_register(ViPipe, 0x3401, 0x03);
	cv4001_write_register(ViPipe, 0x3035, 0x01);
	cv4001_write_register(ViPipe, 0x3036, 0x01);
	cv4001_write_register(ViPipe, 0x3020, 0x04);
	cv4001_write_register(ViPipe, 0x3048, 0x44);
	cv4001_write_register(ViPipe, 0x3049, 0x00);
	cv4001_write_register(ViPipe, 0x304A, 0x00);
	cv4001_write_register(ViPipe, 0x304B, 0x0A);
	cv4001_write_register(ViPipe, 0x3054, 0x2C);
	cv4001_write_register(ViPipe, 0x3056, 0xA8);
	cv4001_write_register(ViPipe, 0x3057, 0x05);
	cv4001_write_register(ViPipe, 0x3030, 0x05);
	cv4001_write_register(ViPipe, 0x3060, 0x2C);
	cv4001_write_register(ViPipe, 0x3064, 0x12);
	cv4001_write_register(ViPipe, 0x3070, 0x1A);//62
	cv4001_write_register(ViPipe, 0x3071, 0x00);
	cv4001_write_register(ViPipe, 0x343C, 0x01);
	cv4001_write_register(ViPipe, 0x3930, 0x00);
	cv4001_write_register(ViPipe, 0x3040, 0x01);
	cv4001_write_register(ViPipe, 0x3044, 0x04);
	cv4001_write_register(ViPipe, 0x3046, 0xA0);
	cv4001_write_register(ViPipe, 0x3047, 0x05);
	cv4001_write_register(ViPipe, 0x362A, 0x00);
	cv4001_write_register(ViPipe, 0x3625, 0x01);
	cv4001_write_register(ViPipe, 0x35A4, 0x09);
	cv4001_write_register(ViPipe, 0x35A8, 0x09);
	cv4001_write_register(ViPipe, 0x35AE, 0x07);
	cv4001_write_register(ViPipe, 0x35AF, 0x07);
	cv4001_write_register(ViPipe, 0x34A2, 0x2C);
	cv4001_write_register(ViPipe, 0x3416, 0x0F);
	cv4001_write_register(ViPipe, 0x3418, 0x9F);
	cv4001_write_register(ViPipe, 0x341A, 0x57);
	cv4001_write_register(ViPipe, 0x341C, 0x57);
	cv4001_write_register(ViPipe, 0x341E, 0x6F);
	cv4001_write_register(ViPipe, 0x341F, 0x01);
	cv4001_write_register(ViPipe, 0x3420, 0x57);
	cv4001_write_register(ViPipe, 0x3422, 0x9F);
	cv4001_write_register(ViPipe, 0x3424, 0x57);
	cv4001_write_register(ViPipe, 0x3426, 0x8F);
	cv4001_write_register(ViPipe, 0x3428, 0x47);
	cv4001_write_register(ViPipe, 0x3348, 0x00);
	cv4001_write_register(ViPipe, 0x3000, 0x00);
	cv4001_write_register(ViPipe, 0x3220, 0x03);
	cv4001_write_register(ViPipe, 0x3347, 0x01);
	cv4001_write_register(ViPipe, 0x3348, 0x00);
	cv4001_write_register(ViPipe, 0x3804, 0x0F);
	cv4001_write_register(ViPipe, 0x3576, 0x06);
	cv4001_write_register(ViPipe, 0x350F, 0x18);
	cv4001_write_register(ViPipe, 0x3513, 0x07);
	cv4001_write_register(ViPipe, 0x3517, 0x07);
	cv4001_write_register(ViPipe, 0x351A, 0x05);
	cv4001_write_register(ViPipe, 0x351E, 0x0B);
	cv4001_write_register(ViPipe, 0x357A, 0x0B);
	cv4001_write_register(ViPipe, 0x3244, 0x08);
	cv4001_write_register(ViPipe, 0x3270, 0x60);
	cv4001_write_register(ViPipe, 0x3271, 0x00);
	cv4001_write_register(ViPipe, 0x3272, 0x00);
	cv4001_write_register(ViPipe, 0x3890, 0x01);
	cv4001_write_register(ViPipe, 0x3894, 0x05);
	cv4001_write_register(ViPipe, 0x3690, 0x00);
	cv4001_write_register(ViPipe, 0x3898, 0x20);
	cv4001_write_register(ViPipe, 0x3899, 0x20);
	cv4001_write_register(ViPipe, 0x389a, 0x20);
	cv4001_write_register(ViPipe, 0x389b, 0x20);
	cv4001_write_register(ViPipe, 0x389c, 0x20);
	cv4001_write_register(ViPipe, 0x389d, 0x15);
	cv4001_write_register(ViPipe, 0x389e, 0x05);
	cv4001_write_register(ViPipe, 0x3583, 0x2f);
	cv4001_write_register(ViPipe, 0x3b75, 0x00);
	cv4001_write_register(ViPipe, 0x3b5E, 0x01);
	cv4001_write_register(ViPipe, 0x3a10, 0x06);
	cv4001_write_register(ViPipe, 0x3a11, 0x06);
	cv4001_write_register(ViPipe, 0x316C, 0x64);
	cv4001_write_register(ViPipe, 0x3162, 0x01);
	cv4001_write_register(ViPipe, 0x3180, 0x00);
	cv4001_write_register(ViPipe, 0x3178, 0x40);
	cv4001_write_register(ViPipe, 0x3179, 0x00);

	cv4001_default_reg_init(ViPipe);
	delay_ms(100);

	printf("ViPipe:%d,===CV4001 1440P 15fps 12bit WDR2TO1 Init OK!===\n", ViPipe);
}

