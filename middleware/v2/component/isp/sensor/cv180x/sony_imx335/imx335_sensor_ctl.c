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
#include "imx335_cmos_ex.h"
static void imx335_wdr_5M30_2to1_init(VI_PIPE ViPipe);
static void imx335_wdr_4M30_2to1_init(VI_PIPE ViPipe);
static void imx335_wdr_4M30_1600p_2to1_init(VI_PIPE ViPipe);

static void imx335_linear_5M30_init(VI_PIPE ViPipe);
static void imx335_linear_4M30_init(VI_PIPE ViPipe);
static void imx335_linear_4M30_2l_init(VI_PIPE ViPipe);
static void imx335_linear_4M30_1600p_init(VI_PIPE ViPipe);

const CVI_U8 imx335_i2c_addr = 0x1A;

const CVI_U32 imx335_addr_byte = 2;
const CVI_U32 imx335_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int imx335_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunImx335_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, imx335_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int imx335_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int imx335_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (imx335_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, imx335_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, imx335_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (imx335_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}


int imx335_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (imx335_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (imx335_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, imx335_addr_byte + imx335_data_byte);
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

void imx335_standby(VI_PIPE ViPipe)
{
	imx335_write_register(ViPipe, 0x3000, 0x01); /* STANDBY */
	imx335_write_register(ViPipe, 0x3002, 0x01); /* XTMSTA */
}

void imx335_restart(VI_PIPE ViPipe)
{
	imx335_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
	//imx335_write_register(ViPipe, 0x304b, 0x0a);
}

void imx335_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastImx335[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		imx335_write_register(ViPipe,
				g_pastImx335[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastImx335[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

void imx335_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 u8Filp = 0;
	CVI_U8 u8Mirror = 0;

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		u8Mirror = 1;
		break;
	case ISP_SNS_FLIP:
		u8Filp = 1;
		break;
	case ISP_SNS_MIRROR_FLIP:
		u8Filp = 1;
		u8Mirror = 1;
		break;
	default:
		return;
	}

	imx335_write_register(ViPipe, 0x304e, u8Mirror);
	imx335_write_register(ViPipe, 0x304f, u8Filp);
	if (u8Filp != 0) {
		imx335_write_register(ViPipe, 0x3074, 0x10);
		imx335_write_register(ViPipe, 0x3075, 0x10);
		imx335_write_register(ViPipe, 0x3081, 0xfe);
		imx335_write_register(ViPipe, 0x3083, 0xfe);
		imx335_write_register(ViPipe, 0x30b6, 0xfa);
		imx335_write_register(ViPipe, 0x30b7, 0x01);
		imx335_write_register(ViPipe, 0x3116, 0x02);
		imx335_write_register(ViPipe, 0x3117, 0x00);
	} else {
		imx335_write_register(ViPipe, 0x3074, 0xb0);
		imx335_write_register(ViPipe, 0x3075, 0x00);
		imx335_write_register(ViPipe, 0x3081, 0x02);
		imx335_write_register(ViPipe, 0x3083, 0x02);
		imx335_write_register(ViPipe, 0x30b6, 0x00);
		imx335_write_register(ViPipe, 0x30b7, 0x00);
		imx335_write_register(ViPipe, 0x3116, 0x08);
		imx335_write_register(ViPipe, 0x3117, 0x00);
	}
}

int imx335_probe(VI_PIPE ViPipe)
{
	int nVal;

	usleep(100);
	if (imx335_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = imx335_read_register(ViPipe, 0x3000);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	return CVI_SUCCESS;
}

void imx335_init(VI_PIPE ViPipe)
{
	WDR_MODE_E        enWDRMode;
	CVI_U8            u8ImgMode;

	enWDRMode   = g_pastImx335[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastImx335[ViPipe]->u8ImgMode;

	imx335_i2c_init(ViPipe);

	if (enWDRMode == WDR_MODE_2To1_LINE) {
		if (u8ImgMode == IMX335_MODE_5M30_WDR)
			imx335_wdr_5M30_2to1_init(ViPipe);
		else if (u8ImgMode == IMX335_MODE_4M30_WDR)
			imx335_wdr_4M30_2to1_init(ViPipe);
		else if (u8ImgMode == IMX335_MODE_4M30_1600P_WDR)
			imx335_wdr_4M30_1600p_2to1_init(ViPipe);
		else{
		}
	} else {
		if (u8ImgMode == IMX335_MODE_5M30)
			imx335_linear_5M30_init(ViPipe);
		else if (u8ImgMode == IMX335_MODE_4M30)
			imx335_linear_4M30_init(ViPipe);
		else if (u8ImgMode == IMX335_MODE_4M30_2L)
			imx335_linear_4M30_2l_init(ViPipe);
		else if (u8ImgMode == IMX335_MODE_4M30_1600P)
			imx335_linear_4M30_1600p_init(ViPipe);
		else {
		}
	}
	g_pastImx335[ViPipe]->bInit = CVI_TRUE;
}

void imx335_exit(VI_PIPE ViPipe)
{
	imx335_i2c_exit(ViPipe);
}
static void imx335_linear_4M30_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01); // STANDBY
	imx335_write_register(ViPipe, 0x3002, 0x01); // XTMSTA

	imx335_write_register(ViPipe, 0x300C, 0x5B);
	imx335_write_register(ViPipe, 0x300D, 0x40);
	imx335_write_register(ViPipe, 0x3018, 0x04);// WINMODE[3:0] croping mode
	imx335_write_register(ViPipe, 0x302C, 0x3C);
	imx335_write_register(ViPipe, 0x302E, 0x20);
	imx335_write_register(ViPipe, 0x3056, 0xB4);//Y_OUT_SIZE[12:0]
	imx335_write_register(ViPipe, 0x3057, 0x05);
	imx335_write_register(ViPipe, 0x3074, 0xA8);
	imx335_write_register(ViPipe, 0x3075, 0x02);
	imx335_write_register(ViPipe, 0x3076, 0x68);
	imx335_write_register(ViPipe, 0x3077, 0x0B);
	imx335_write_register(ViPipe, 0x30C6, 0x12);
	imx335_write_register(ViPipe, 0x30CE, 0x64);
	imx335_write_register(ViPipe, 0x30D8, 0xE0);
	imx335_write_register(ViPipe, 0x30D9, 0x0E);
	imx335_write_register(ViPipe, 0x314C, 0xC0);
	imx335_write_register(ViPipe, 0x315A, 0x06);
	imx335_write_register(ViPipe, 0x316A, 0x7E);
	imx335_write_register(ViPipe, 0x319E, 0x02);
	imx335_write_register(ViPipe, 0x31A1, 0x00);
	imx335_write_register(ViPipe, 0x3288, 0x21);
	imx335_write_register(ViPipe, 0x328A, 0x02);
	imx335_write_register(ViPipe, 0x3414, 0x05);
	imx335_write_register(ViPipe, 0x3416, 0x18);
	imx335_write_register(ViPipe, 0x3648, 0x01);
	imx335_write_register(ViPipe, 0x364A, 0x04);
	imx335_write_register(ViPipe, 0x364C, 0x04);
	imx335_write_register(ViPipe, 0x3678, 0x01);
	imx335_write_register(ViPipe, 0x367C, 0x31);
	imx335_write_register(ViPipe, 0x367E, 0x31);
	imx335_write_register(ViPipe, 0x3706, 0x10);
	imx335_write_register(ViPipe, 0x3708, 0x03);
	imx335_write_register(ViPipe, 0x3714, 0x02);
	imx335_write_register(ViPipe, 0x3715, 0x02);
	imx335_write_register(ViPipe, 0x3716, 0x01);
	imx335_write_register(ViPipe, 0x3717, 0x03);
	imx335_write_register(ViPipe, 0x371C, 0x3D);
	imx335_write_register(ViPipe, 0x371D, 0x3F);
	imx335_write_register(ViPipe, 0x372C, 0x00);
	imx335_write_register(ViPipe, 0x372D, 0x00);
	imx335_write_register(ViPipe, 0x372E, 0x46);
	imx335_write_register(ViPipe, 0x372F, 0x00);
	imx335_write_register(ViPipe, 0x3730, 0x89);
	imx335_write_register(ViPipe, 0x3731, 0x00);
	imx335_write_register(ViPipe, 0x3732, 0x08);
	imx335_write_register(ViPipe, 0x3733, 0x01);
	imx335_write_register(ViPipe, 0x3734, 0xFE);
	imx335_write_register(ViPipe, 0x3735, 0x05);
	imx335_write_register(ViPipe, 0x3740, 0x02);
	imx335_write_register(ViPipe, 0x375D, 0x00);
	imx335_write_register(ViPipe, 0x375E, 0x00);
	imx335_write_register(ViPipe, 0x375F, 0x11);
	imx335_write_register(ViPipe, 0x3760, 0x01);
	imx335_write_register(ViPipe, 0x3768, 0x1B);
	imx335_write_register(ViPipe, 0x3769, 0x1B);
	imx335_write_register(ViPipe, 0x376A, 0x1B);
	imx335_write_register(ViPipe, 0x376B, 0x1B);
	imx335_write_register(ViPipe, 0x376C, 0x1A);
	imx335_write_register(ViPipe, 0x376D, 0x17);
	imx335_write_register(ViPipe, 0x376E, 0x0F);
	imx335_write_register(ViPipe, 0x3776, 0x00);
	imx335_write_register(ViPipe, 0x3777, 0x00);
	imx335_write_register(ViPipe, 0x3778, 0x46);
	imx335_write_register(ViPipe, 0x3779, 0x00);
	imx335_write_register(ViPipe, 0x377A, 0x89);
	imx335_write_register(ViPipe, 0x377B, 0x00);
	imx335_write_register(ViPipe, 0x377C, 0x08);
	imx335_write_register(ViPipe, 0x377D, 0x01);
	imx335_write_register(ViPipe, 0x377E, 0x23);
	imx335_write_register(ViPipe, 0x377F, 0x02);
	imx335_write_register(ViPipe, 0x3780, 0xD9);
	imx335_write_register(ViPipe, 0x3781, 0x03);
	imx335_write_register(ViPipe, 0x3782, 0xF5);
	imx335_write_register(ViPipe, 0x3783, 0x06);
	imx335_write_register(ViPipe, 0x3784, 0xA5);
	imx335_write_register(ViPipe, 0x3788, 0x0F);
	imx335_write_register(ViPipe, 0x378A, 0xD9);
	imx335_write_register(ViPipe, 0x378B, 0x03);
	imx335_write_register(ViPipe, 0x378C, 0xEB);
	imx335_write_register(ViPipe, 0x378D, 0x05);
	imx335_write_register(ViPipe, 0x378E, 0x87);
	imx335_write_register(ViPipe, 0x378F, 0x06);
	imx335_write_register(ViPipe, 0x3790, 0xF5);
	imx335_write_register(ViPipe, 0x3792, 0x43);
	imx335_write_register(ViPipe, 0x3794, 0x7A);
	imx335_write_register(ViPipe, 0x3796, 0xA1);
	imx335_write_register(ViPipe, 0x3A18, 0x7F);
	imx335_write_register(ViPipe, 0x3A1A, 0x37);
	imx335_write_register(ViPipe, 0x3A1C, 0x37);
	imx335_write_register(ViPipe, 0x3A1E, 0xF7);
	imx335_write_register(ViPipe, 0x3A1F, 0x00);
	imx335_write_register(ViPipe, 0x3A20, 0x3F);
	imx335_write_register(ViPipe, 0x3A22, 0x6F);
	imx335_write_register(ViPipe, 0x3A24, 0x3F);
	imx335_write_register(ViPipe, 0x3A26, 0x5F);
	imx335_write_register(ViPipe, 0x3A28, 0x2F);
	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00);	/* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00);	/* master mode start */
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe:%d,===IMX335 4M 30fps 12bit LINE Init OK!===\n", ViPipe);
}

static void imx335_linear_4M30_2l_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01); // STANDBY
	imx335_write_register(ViPipe, 0x3002, 0x01); // XTMSTA

	imx335_write_register(ViPipe, 0x300C, 0x5B); // BCWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x300D, 0x40); // CPWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x3018, 0x04); // WINMODE[3:0]
	imx335_write_register(ViPipe, 0x302C, 0x48); // HTRIMMING_START[11:0]
	imx335_write_register(ViPipe, 0x302E, 0x08); // HNUM[11:0]
	imx335_write_register(ViPipe, 0x3050, 0x00); // ADBIT[0]
	imx335_write_register(ViPipe, 0x3056, 0xA4); // Y_OUT_SIZE[12:0]
	imx335_write_register(ViPipe, 0x3057, 0x05); //
	imx335_write_register(ViPipe, 0x3074, 0xB8); // AREA3_ST_ADR_1[12:0]
	imx335_write_register(ViPipe, 0x3075, 0x02); //
	imx335_write_register(ViPipe, 0x3076, 0x48); // AREA3_WIDTH_1[12:0]
	imx335_write_register(ViPipe, 0x3077, 0x0B); //
	imx335_write_register(ViPipe, 0x30C6, 0x12); // BLACK_OFSET_ADR[12:0]
	imx335_write_register(ViPipe, 0x30CE, 0x64); // UNRD_LINE_MAX[12:0]
	imx335_write_register(ViPipe, 0x30D8, 0xD0); // UNREAD_ED_ADR[12:0]
	imx335_write_register(ViPipe, 0x30D9, 0x0E); //
	imx335_write_register(ViPipe, 0x315A, 0x02); // INCKSEL2[1:0]
	imx335_write_register(ViPipe, 0x316A, 0x7E); // INCKSEL4[1:0]
	imx335_write_register(ViPipe, 0x319D, 0x00); // MDBIT
	imx335_write_register(ViPipe, 0x31A1, 0x00); // XVS_DRV[1:0]
	imx335_write_register(ViPipe, 0x3288, 0x21); // -
	imx335_write_register(ViPipe, 0x328A, 0x02); // -
	imx335_write_register(ViPipe, 0x3414, 0x05); // -
	imx335_write_register(ViPipe, 0x3416, 0x18); // -
	imx335_write_register(ViPipe, 0x341C, 0xFF); // ADBIT1[8:0]
	imx335_write_register(ViPipe, 0x341D, 0x01); //
	imx335_write_register(ViPipe, 0x3648, 0x01); // -
	imx335_write_register(ViPipe, 0x364A, 0x04); // -
	imx335_write_register(ViPipe, 0x364C, 0x04); // -
	imx335_write_register(ViPipe, 0x3678, 0x01); // -
	imx335_write_register(ViPipe, 0x367C, 0x31); // -
	imx335_write_register(ViPipe, 0x367E, 0x31); // -
	imx335_write_register(ViPipe, 0x3706, 0x10); // -
	imx335_write_register(ViPipe, 0x3708, 0x03); // -
	imx335_write_register(ViPipe, 0x3714, 0x02); // -
	imx335_write_register(ViPipe, 0x3715, 0x02); // -
	imx335_write_register(ViPipe, 0x3716, 0x01); // -
	imx335_write_register(ViPipe, 0x3717, 0x03); // -
	imx335_write_register(ViPipe, 0x371C, 0x3D); // -
	imx335_write_register(ViPipe, 0x371D, 0x3F); // -
	imx335_write_register(ViPipe, 0x372C, 0x00); // -
	imx335_write_register(ViPipe, 0x372D, 0x00); // -
	imx335_write_register(ViPipe, 0x372E, 0x46); // -
	imx335_write_register(ViPipe, 0x372F, 0x00); // -
	imx335_write_register(ViPipe, 0x3730, 0x89); // -
	imx335_write_register(ViPipe, 0x3731, 0x00); // -
	imx335_write_register(ViPipe, 0x3732, 0x08); // -
	imx335_write_register(ViPipe, 0x3733, 0x01); // -
	imx335_write_register(ViPipe, 0x3734, 0xFE); // -
	imx335_write_register(ViPipe, 0x3735, 0x05); // -
	imx335_write_register(ViPipe, 0x3740, 0x02); // -
	imx335_write_register(ViPipe, 0x375D, 0x00); // -
	imx335_write_register(ViPipe, 0x375E, 0x00); // -
	imx335_write_register(ViPipe, 0x375F, 0x11); // -
	imx335_write_register(ViPipe, 0x3760, 0x01); // -
	imx335_write_register(ViPipe, 0x3768, 0x1A); // -
	imx335_write_register(ViPipe, 0x3769, 0x1A); // -
	imx335_write_register(ViPipe, 0x376A, 0x1A); // -
	imx335_write_register(ViPipe, 0x376B, 0x1A); // -
	imx335_write_register(ViPipe, 0x376C, 0x1A); // -
	imx335_write_register(ViPipe, 0x376D, 0x17); // -
	imx335_write_register(ViPipe, 0x376E, 0x0F); // -
	imx335_write_register(ViPipe, 0x3776, 0x00); // -
	imx335_write_register(ViPipe, 0x3777, 0x00); // -
	imx335_write_register(ViPipe, 0x3778, 0x46); // -
	imx335_write_register(ViPipe, 0x3779, 0x00); // -
	imx335_write_register(ViPipe, 0x377A, 0x89); // -
	imx335_write_register(ViPipe, 0x377B, 0x00); // -
	imx335_write_register(ViPipe, 0x377C, 0x08); // -
	imx335_write_register(ViPipe, 0x377D, 0x01); // -
	imx335_write_register(ViPipe, 0x377E, 0x23); // -
	imx335_write_register(ViPipe, 0x377F, 0x02); // -
	imx335_write_register(ViPipe, 0x3780, 0xD9); // -
	imx335_write_register(ViPipe, 0x3781, 0x03); // -
	imx335_write_register(ViPipe, 0x3782, 0xF5); // -
	imx335_write_register(ViPipe, 0x3783, 0x06); // -
	imx335_write_register(ViPipe, 0x3784, 0xA5); // -
	imx335_write_register(ViPipe, 0x3788, 0x0F); // -
	imx335_write_register(ViPipe, 0x378A, 0xD9); // -
	imx335_write_register(ViPipe, 0x378B, 0x03); // -
	imx335_write_register(ViPipe, 0x378C, 0xEB); // -
	imx335_write_register(ViPipe, 0x378D, 0x05); // -
	imx335_write_register(ViPipe, 0x378E, 0x87); // -
	imx335_write_register(ViPipe, 0x378F, 0x06); // -
	imx335_write_register(ViPipe, 0x3790, 0xF5); // -
	imx335_write_register(ViPipe, 0x3792, 0x43); // -
	imx335_write_register(ViPipe, 0x3794, 0x7A); // -
	imx335_write_register(ViPipe, 0x3796, 0xA1); // -
	imx335_write_register(ViPipe, 0x3A01, 0x01); // LANEMODE[2:0]
	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00);	/* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00);	/* master mode start */
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe:%d,===IMX335 4M 30fps 2L 10bit LINE Init OK!===\n", ViPipe);
}

static void imx335_linear_4M30_1600p_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01); // STANDBY
	imx335_write_register(ViPipe, 0x3002, 0x01); // XTMSTA

	imx335_write_register(ViPipe, 0x300C, 0x5B);
	imx335_write_register(ViPipe, 0x300D, 0x40);
	imx335_write_register(ViPipe, 0x3018, 0x04);// WINMODE[3:0] croping mode
	imx335_write_register(ViPipe, 0x302C, 0x3C);
	imx335_write_register(ViPipe, 0x302E, 0x20);
	imx335_write_register(ViPipe, 0x3056, 0x54);//Y_OUT_SIZE[12:0]
	imx335_write_register(ViPipe, 0x3057, 0x06);
	imx335_write_register(ViPipe, 0x3074, 0x08);
	imx335_write_register(ViPipe, 0x3075, 0x02);
	imx335_write_register(ViPipe, 0x3076, 0xA8);
	imx335_write_register(ViPipe, 0x3077, 0x0C);
	imx335_write_register(ViPipe, 0x30C6, 0x12);
	imx335_write_register(ViPipe, 0x30CE, 0x64);
	imx335_write_register(ViPipe, 0x30D8, 0x80);
	imx335_write_register(ViPipe, 0x30D9, 0x0F);
	imx335_write_register(ViPipe, 0x315A, 0x02);
	imx335_write_register(ViPipe, 0x316A, 0x7E);
	imx335_write_register(ViPipe, 0x31A1, 0x00);
	imx335_write_register(ViPipe, 0x3288, 0x21);
	imx335_write_register(ViPipe, 0x328A, 0x02);
	imx335_write_register(ViPipe, 0x3414, 0x05);
	imx335_write_register(ViPipe, 0x3416, 0x18);
	imx335_write_register(ViPipe, 0x3648, 0x01);
	imx335_write_register(ViPipe, 0x364A, 0x04);
	imx335_write_register(ViPipe, 0x364C, 0x04);
	imx335_write_register(ViPipe, 0x3678, 0x01);
	imx335_write_register(ViPipe, 0x367C, 0x31);
	imx335_write_register(ViPipe, 0x367E, 0x31);
	imx335_write_register(ViPipe, 0x3706, 0x10);
	imx335_write_register(ViPipe, 0x3708, 0x03);
	imx335_write_register(ViPipe, 0x3714, 0x02);
	imx335_write_register(ViPipe, 0x3715, 0x02);
	imx335_write_register(ViPipe, 0x3716, 0x01);
	imx335_write_register(ViPipe, 0x3717, 0x03);
	imx335_write_register(ViPipe, 0x371C, 0x3D);
	imx335_write_register(ViPipe, 0x371D, 0x3F);
	imx335_write_register(ViPipe, 0x372C, 0x00);
	imx335_write_register(ViPipe, 0x372D, 0x00);
	imx335_write_register(ViPipe, 0x372E, 0x46);
	imx335_write_register(ViPipe, 0x372F, 0x00);
	imx335_write_register(ViPipe, 0x3730, 0x89);
	imx335_write_register(ViPipe, 0x3731, 0x00);
	imx335_write_register(ViPipe, 0x3732, 0x08);
	imx335_write_register(ViPipe, 0x3733, 0x01);
	imx335_write_register(ViPipe, 0x3734, 0xFE);
	imx335_write_register(ViPipe, 0x3735, 0x05);
	imx335_write_register(ViPipe, 0x3740, 0x02);
	imx335_write_register(ViPipe, 0x375D, 0x00);
	imx335_write_register(ViPipe, 0x375E, 0x00);
	imx335_write_register(ViPipe, 0x375F, 0x11);
	imx335_write_register(ViPipe, 0x3760, 0x01);
	imx335_write_register(ViPipe, 0x3768, 0x1B);
	imx335_write_register(ViPipe, 0x3769, 0x1B);
	imx335_write_register(ViPipe, 0x376A, 0x1B);
	imx335_write_register(ViPipe, 0x376B, 0x1B);
	imx335_write_register(ViPipe, 0x376C, 0x1A);
	imx335_write_register(ViPipe, 0x376D, 0x17);
	imx335_write_register(ViPipe, 0x376E, 0x0F);
	imx335_write_register(ViPipe, 0x3776, 0x00);
	imx335_write_register(ViPipe, 0x3777, 0x00);
	imx335_write_register(ViPipe, 0x3778, 0x46);
	imx335_write_register(ViPipe, 0x3779, 0x00);
	imx335_write_register(ViPipe, 0x377A, 0x89);
	imx335_write_register(ViPipe, 0x377B, 0x00);
	imx335_write_register(ViPipe, 0x377C, 0x08);
	imx335_write_register(ViPipe, 0x377D, 0x01);
	imx335_write_register(ViPipe, 0x377E, 0x23);
	imx335_write_register(ViPipe, 0x377F, 0x02);
	imx335_write_register(ViPipe, 0x3780, 0xD9);
	imx335_write_register(ViPipe, 0x3781, 0x03);
	imx335_write_register(ViPipe, 0x3782, 0xF5);
	imx335_write_register(ViPipe, 0x3783, 0x06);
	imx335_write_register(ViPipe, 0x3784, 0xA5);
	imx335_write_register(ViPipe, 0x3788, 0x0F);
	imx335_write_register(ViPipe, 0x378A, 0xD9);
	imx335_write_register(ViPipe, 0x378B, 0x03);
	imx335_write_register(ViPipe, 0x378C, 0xEB);
	imx335_write_register(ViPipe, 0x378D, 0x05);
	imx335_write_register(ViPipe, 0x378E, 0x87);
	imx335_write_register(ViPipe, 0x378F, 0x06);
	imx335_write_register(ViPipe, 0x3790, 0xF5);
	imx335_write_register(ViPipe, 0x3792, 0x43);
	imx335_write_register(ViPipe, 0x3794, 0x7A);
	imx335_write_register(ViPipe, 0x3796, 0xA1);
	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00);	/* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00);	/* master mode start */
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe:%d,===IMX335 4M 1660P 30fps 12bit LINE Init OK!===\n", ViPipe);
}

static void imx335_linear_5M30_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01); // STANDBY
	imx335_write_register(ViPipe, 0x3002, 0x01); // XTMSTA

	imx335_write_register(ViPipe, 0x300C, 0x5B);
	imx335_write_register(ViPipe, 0x300D, 0x40);
	imx335_write_register(ViPipe, 0x315A, 0x02);
	imx335_write_register(ViPipe, 0x316A, 0x7E);
	imx335_write_register(ViPipe, 0x31A1, 0x00);
	imx335_write_register(ViPipe, 0x3288, 0x21);
	imx335_write_register(ViPipe, 0x328A, 0x02);
	imx335_write_register(ViPipe, 0x3414, 0x05);
	imx335_write_register(ViPipe, 0x3416, 0x18);
	imx335_write_register(ViPipe, 0x3648, 0x01);
	imx335_write_register(ViPipe, 0x364A, 0x04);
	imx335_write_register(ViPipe, 0x364C, 0x04);
	imx335_write_register(ViPipe, 0x3678, 0x01);
	imx335_write_register(ViPipe, 0x367C, 0x31);
	imx335_write_register(ViPipe, 0x367E, 0x31);
	imx335_write_register(ViPipe, 0x3706, 0x10);
	imx335_write_register(ViPipe, 0x3708, 0x03);
	imx335_write_register(ViPipe, 0x3714, 0x02);
	imx335_write_register(ViPipe, 0x3715, 0x02);
	imx335_write_register(ViPipe, 0x3716, 0x01);
	imx335_write_register(ViPipe, 0x3717, 0x03);
	imx335_write_register(ViPipe, 0x371C, 0x3D);
	imx335_write_register(ViPipe, 0x371D, 0x3F);
	imx335_write_register(ViPipe, 0x372C, 0x00);
	imx335_write_register(ViPipe, 0x372D, 0x00);
	imx335_write_register(ViPipe, 0x372E, 0x46);
	imx335_write_register(ViPipe, 0x372F, 0x00);
	imx335_write_register(ViPipe, 0x3730, 0x89);
	imx335_write_register(ViPipe, 0x3731, 0x00);
	imx335_write_register(ViPipe, 0x3732, 0x08);
	imx335_write_register(ViPipe, 0x3733, 0x01);
	imx335_write_register(ViPipe, 0x3734, 0xFE);
	imx335_write_register(ViPipe, 0x3735, 0x05);
	imx335_write_register(ViPipe, 0x3740, 0x02);
	imx335_write_register(ViPipe, 0x375D, 0x00);
	imx335_write_register(ViPipe, 0x375E, 0x00);
	imx335_write_register(ViPipe, 0x375F, 0x11);
	imx335_write_register(ViPipe, 0x3760, 0x01);
	imx335_write_register(ViPipe, 0x3768, 0x1B);
	imx335_write_register(ViPipe, 0x3769, 0x1B);
	imx335_write_register(ViPipe, 0x376A, 0x1B);
	imx335_write_register(ViPipe, 0x376B, 0x1B);
	imx335_write_register(ViPipe, 0x376C, 0x1A);
	imx335_write_register(ViPipe, 0x376D, 0x17);
	imx335_write_register(ViPipe, 0x376E, 0x0F);
	imx335_write_register(ViPipe, 0x3776, 0x00);
	imx335_write_register(ViPipe, 0x3777, 0x00);
	imx335_write_register(ViPipe, 0x3778, 0x46);
	imx335_write_register(ViPipe, 0x3779, 0x00);
	imx335_write_register(ViPipe, 0x377A, 0x89);
	imx335_write_register(ViPipe, 0x377B, 0x00);
	imx335_write_register(ViPipe, 0x377C, 0x08);
	imx335_write_register(ViPipe, 0x377D, 0x01);
	imx335_write_register(ViPipe, 0x377E, 0x23);
	imx335_write_register(ViPipe, 0x377F, 0x02);
	imx335_write_register(ViPipe, 0x3780, 0xD9);
	imx335_write_register(ViPipe, 0x3781, 0x03);
	imx335_write_register(ViPipe, 0x3782, 0xF5);
	imx335_write_register(ViPipe, 0x3783, 0x06);
	imx335_write_register(ViPipe, 0x3784, 0xA5);
	imx335_write_register(ViPipe, 0x3788, 0x0F);
	imx335_write_register(ViPipe, 0x378A, 0xD9);
	imx335_write_register(ViPipe, 0x378B, 0x03);
	imx335_write_register(ViPipe, 0x378C, 0xEB);
	imx335_write_register(ViPipe, 0x378D, 0x05);
	imx335_write_register(ViPipe, 0x378E, 0x87);
	imx335_write_register(ViPipe, 0x378F, 0x06);
	imx335_write_register(ViPipe, 0x3790, 0xF5);
	imx335_write_register(ViPipe, 0x3792, 0x43);
	imx335_write_register(ViPipe, 0x3794, 0x7A);
	imx335_write_register(ViPipe, 0x3796, 0xA1);
	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00); /* master mode start */
	CVI_TRACE_SNS(CVI_DBG_INFO, "ViPipe:%d,===IMX335 5M 30fps 12bit LINE Init OK!===\n", ViPipe);
}

static void imx335_wdr_5M30_2to1_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01); /* STANDBY */
	imx335_write_register(ViPipe, 0x3002, 0x01); /* XTMSTA */
	imx335_write_register(ViPipe, 0x3003, 0x00);
	imx335_write_register(ViPipe, 0x3004, 0x00);
	imx335_write_register(ViPipe, 0x300C, 0x5B);  // BCWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x300D, 0x40);  // CPWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x3034, 0x13);  // HMAX[15:0]
	imx335_write_register(ViPipe, 0x3035, 0x01);  //
	imx335_write_register(ViPipe, 0x3048, 0x01);  // WDMODE[0]
	imx335_write_register(ViPipe, 0x3049, 0x01);  // WDSEL[1:0]
	imx335_write_register(ViPipe, 0x304A, 0x04);  // WD_SET1[2:0]
	imx335_write_register(ViPipe, 0x304B, 0x03);  // WD_SET2[3:0]
	imx335_write_register(ViPipe, 0x304C, 0x13);  // OPB_SIZE_V[5:0]
	imx335_write_register(ViPipe, 0x3050, 0x00);  // ADBIT[0]
	imx335_write_register(ViPipe, 0x3058, 0x68);  // SHR0[19:0]
	imx335_write_register(ViPipe, 0x3059, 0x01);  //
	imx335_write_register(ViPipe, 0x3068, 0x4A);  // RHS1[19:0]
	imx335_write_register(ViPipe, 0x315A, 0x02);  // INCKSEL2[1:0]
	imx335_write_register(ViPipe, 0x316A, 0x7E);  // INCKSEL4[1:0]
	imx335_write_register(ViPipe, 0x319D, 0x00);  // MDBIT
	imx335_write_register(ViPipe, 0x31A1, 0x00);  // XVS_DRV[1:0]
	imx335_write_register(ViPipe, 0x31D7, 0x01);  // XVSMSKCNT_INT[1:0]
	imx335_write_register(ViPipe, 0x3288, 0x21);  // -
	imx335_write_register(ViPipe, 0x328A, 0x02);  // -
	imx335_write_register(ViPipe, 0x3414, 0x05);  // -
	imx335_write_register(ViPipe, 0x3416, 0x18);  // -
	imx335_write_register(ViPipe, 0x341C, 0xFF);  // ADBIT1[8:0]
	imx335_write_register(ViPipe, 0x341D, 0x01);  //
	imx335_write_register(ViPipe, 0x3648, 0x01);  // -
	imx335_write_register(ViPipe, 0x364A, 0x04);  // -
	imx335_write_register(ViPipe, 0x364C, 0x04);  // -
	imx335_write_register(ViPipe, 0x3678, 0x01);  // -
	imx335_write_register(ViPipe, 0x367C, 0x31);  // -
	imx335_write_register(ViPipe, 0x367E, 0x31);  // -
	imx335_write_register(ViPipe, 0x3706, 0x10);  // -
	imx335_write_register(ViPipe, 0x3708, 0x03);  // -
	imx335_write_register(ViPipe, 0x3714, 0x02);  // -
	imx335_write_register(ViPipe, 0x3715, 0x02);  // -
	imx335_write_register(ViPipe, 0x3716, 0x01);  // -
	imx335_write_register(ViPipe, 0x3717, 0x03);  // -
	imx335_write_register(ViPipe, 0x371C, 0x3D);  // -
	imx335_write_register(ViPipe, 0x371D, 0x3F);  // -
	imx335_write_register(ViPipe, 0x372C, 0x00);  // -
	imx335_write_register(ViPipe, 0x372D, 0x00);  // -
	imx335_write_register(ViPipe, 0x372E, 0x46);  // -
	imx335_write_register(ViPipe, 0x372F, 0x00);  // -
	imx335_write_register(ViPipe, 0x3730, 0x89);  // -
	imx335_write_register(ViPipe, 0x3731, 0x00);  // -
	imx335_write_register(ViPipe, 0x3732, 0x08);  // -
	imx335_write_register(ViPipe, 0x3733, 0x01);  // -
	imx335_write_register(ViPipe, 0x3734, 0xFE);  // -
	imx335_write_register(ViPipe, 0x3735, 0x05);  // -
	imx335_write_register(ViPipe, 0x3740, 0x02);  // -
	imx335_write_register(ViPipe, 0x375D, 0x00);  // -
	imx335_write_register(ViPipe, 0x375E, 0x00);  // -
	imx335_write_register(ViPipe, 0x375F, 0x11);  // -
	imx335_write_register(ViPipe, 0x3760, 0x01);  // -
	imx335_write_register(ViPipe, 0x3768, 0x1B);  // -
	imx335_write_register(ViPipe, 0x3769, 0x1B);  // -
	imx335_write_register(ViPipe, 0x376A, 0x1B);  // -
	imx335_write_register(ViPipe, 0x376B, 0x1B);  // -
	imx335_write_register(ViPipe, 0x376C, 0x1A);  // -
	imx335_write_register(ViPipe, 0x376D, 0x17);  // -
	imx335_write_register(ViPipe, 0x376E, 0x0F);  // -
	imx335_write_register(ViPipe, 0x3776, 0x00);  // -
	imx335_write_register(ViPipe, 0x3777, 0x00);  // -
	imx335_write_register(ViPipe, 0x3778, 0x46);  // -
	imx335_write_register(ViPipe, 0x3779, 0x00);  // -
	imx335_write_register(ViPipe, 0x377A, 0x89);  // -
	imx335_write_register(ViPipe, 0x377B, 0x00);  // -
	imx335_write_register(ViPipe, 0x377C, 0x08);  // -
	imx335_write_register(ViPipe, 0x377D, 0x01);  // -
	imx335_write_register(ViPipe, 0x377E, 0x23);  // -
	imx335_write_register(ViPipe, 0x377F, 0x02);  // -
	imx335_write_register(ViPipe, 0x3780, 0xD9);  // -
	imx335_write_register(ViPipe, 0x3781, 0x03);  // -
	imx335_write_register(ViPipe, 0x3782, 0xF5);  // -
	imx335_write_register(ViPipe, 0x3783, 0x06);  // -
	imx335_write_register(ViPipe, 0x3784, 0xA5);  // -
	imx335_write_register(ViPipe, 0x3788, 0x0F);  // -
	imx335_write_register(ViPipe, 0x378A, 0xD9);  // -
	imx335_write_register(ViPipe, 0x378B, 0x03);  // -
	imx335_write_register(ViPipe, 0x378C, 0xEB);  // -
	imx335_write_register(ViPipe, 0x378D, 0x05);  // -
	imx335_write_register(ViPipe, 0x378E, 0x87);  // -
	imx335_write_register(ViPipe, 0x378F, 0x06);  // -
	imx335_write_register(ViPipe, 0x3790, 0xF5);  // -
	imx335_write_register(ViPipe, 0x3792, 0x43);  // -
	imx335_write_register(ViPipe, 0x3794, 0x7A);  // -
	imx335_write_register(ViPipe, 0x3796, 0xA1);  // -

	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00); /* master mode start */

	CVI_TRACE_SNS(CVI_DBG_INFO, "===Imx335 sensor 5M30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}
static void imx335_wdr_4M30_2to1_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01);/* STANDBY */
	imx335_write_register(ViPipe, 0x3002, 0x01);/* XTMSTA */

	imx335_write_register(ViPipe, 0x300C, 0x5B);// BCWAIT_TIME[CWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x300D, 0x40);// CPWAIT_TIME[PWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x3018, 0x04);// WINMODE[3:0]INMODE[3:0]
	imx335_write_register(ViPipe, 0x3034, 0x13);// HMAX[15:0]MAX[15:0]
	imx335_write_register(ViPipe, 0x3035, 0x01);// -
	imx335_write_register(ViPipe, 0x3048, 0x01);// WDMODE[0]DMODE[0]
	imx335_write_register(ViPipe, 0x3049, 0x01);// WDSEL[1:0]DSEL[1:0]
	imx335_write_register(ViPipe, 0x304A, 0x04);// WD_SET1[2:0]D_SET1[2:0]
	imx335_write_register(ViPipe, 0x304B, 0x03);// WD_SET2[3:0]D_SET2[3:0]
	imx335_write_register(ViPipe, 0x304C, 0x13);// OPB_SIZE_V[5PB_SIZE_V[5:0]
	imx335_write_register(ViPipe, 0x3050, 0x00);// ADBIT[0]DBIT[0]
	imx335_write_register(ViPipe, 0x3056, 0xB4);// Y_OUT_SIZE[1_OUT_SIZE[12:0]
	imx335_write_register(ViPipe, 0x3057, 0x05);// -
	imx335_write_register(ViPipe, 0x3058, 0x68);// SHR0[19:0]HR0[19:0]
	imx335_write_register(ViPipe, 0x3059, 0x01);// -
	imx335_write_register(ViPipe, 0x3068, 0x4A);// RHS1[19:0]HS1[19:0]
	imx335_write_register(ViPipe, 0x3074, 0xA8);// AREA3_ST_ADRREA3_ST_ADR_1[12:0]
	imx335_write_register(ViPipe, 0x3075, 0x02);// -
	imx335_write_register(ViPipe, 0x3076, 0x68);// AREA3_WIDTH_REA3_WIDTH_1[12:0]
	imx335_write_register(ViPipe, 0x3077, 0x0B);// -
	imx335_write_register(ViPipe, 0x30C6, 0x12);// BLACK_OFSET_LACK_OFSET_ADR[12:0]
	imx335_write_register(ViPipe, 0x30CE, 0x64);// UNRD_LINE_MANRD_LINE_MAX[12:0]
	imx335_write_register(ViPipe, 0x30D8, 0xE0);// UNREAD_ED_ADNREAD_ED_ADR[12:0]
	imx335_write_register(ViPipe, 0x30D9, 0x0E);// -
	imx335_write_register(ViPipe, 0x315A, 0x02);// INCKSEL2[1:0NCKSEL2[1:0]
	imx335_write_register(ViPipe, 0x316A, 0x7E);// INCKSEL4[1:0NCKSEL4[1:0]
	imx335_write_register(ViPipe, 0x319D, 0x00);// MDBITDBIT
	imx335_write_register(ViPipe, 0x31A1, 0x00);// XVS_DRV[1:0]CEN
	imx335_write_register(ViPipe, 0x31D7, 0x01);// XVSMSKCNT_INVS_DRV[1:0]
	imx335_write_register(ViPipe, 0x3288, 0x21);// -VSMSKCNT_INT[1:0]
	imx335_write_register(ViPipe, 0x328A, 0x02);// -
	imx335_write_register(ViPipe, 0x3414, 0x05);// -
	imx335_write_register(ViPipe, 0x3416, 0x18);// -
	imx335_write_register(ViPipe, 0x341C, 0xFF);// ADBIT1[8:0]
	imx335_write_register(ViPipe, 0x341D, 0x01);// DBIT1[8:0]
	imx335_write_register(ViPipe, 0x3648, 0x01);// -
	imx335_write_register(ViPipe, 0x364A, 0x04);// -
	imx335_write_register(ViPipe, 0x364C, 0x04);// -
	imx335_write_register(ViPipe, 0x3678, 0x01);// -
	imx335_write_register(ViPipe, 0x367C, 0x31);// -
	imx335_write_register(ViPipe, 0x367E, 0x31);// -
	imx335_write_register(ViPipe, 0x3706, 0x10);// -
	imx335_write_register(ViPipe, 0x3708, 0x03);// -
	imx335_write_register(ViPipe, 0x3714, 0x02);// -
	imx335_write_register(ViPipe, 0x3715, 0x02);// -
	imx335_write_register(ViPipe, 0x3716, 0x01);// -
	imx335_write_register(ViPipe, 0x3717, 0x03);// -
	imx335_write_register(ViPipe, 0x371C, 0x3D);// -
	imx335_write_register(ViPipe, 0x371D, 0x3F);// -
	imx335_write_register(ViPipe, 0x372C, 0x00);// -
	imx335_write_register(ViPipe, 0x372D, 0x00);// -
	imx335_write_register(ViPipe, 0x372E, 0x46);// -
	imx335_write_register(ViPipe, 0x372F, 0x00);// -
	imx335_write_register(ViPipe, 0x3730, 0x89);// -
	imx335_write_register(ViPipe, 0x3731, 0x00);// -
	imx335_write_register(ViPipe, 0x3732, 0x08);// -
	imx335_write_register(ViPipe, 0x3733, 0x01);// -
	imx335_write_register(ViPipe, 0x3734, 0xFE);// -
	imx335_write_register(ViPipe, 0x3735, 0x05);// -
	imx335_write_register(ViPipe, 0x3740, 0x02);// -
	imx335_write_register(ViPipe, 0x375D, 0x00);// -
	imx335_write_register(ViPipe, 0x375E, 0x00);// -
	imx335_write_register(ViPipe, 0x375F, 0x11);// -
	imx335_write_register(ViPipe, 0x3760, 0x01);// -
	imx335_write_register(ViPipe, 0x3768, 0x1B);// -
	imx335_write_register(ViPipe, 0x3769, 0x1B);// -
	imx335_write_register(ViPipe, 0x376A, 0x1B);// -
	imx335_write_register(ViPipe, 0x376B, 0x1B);// -
	imx335_write_register(ViPipe, 0x376C, 0x1A);// -
	imx335_write_register(ViPipe, 0x376D, 0x17);// -
	imx335_write_register(ViPipe, 0x376E, 0x0F);// -
	imx335_write_register(ViPipe, 0x3776, 0x00);// -
	imx335_write_register(ViPipe, 0x3777, 0x00);// -
	imx335_write_register(ViPipe, 0x3778, 0x46);// -
	imx335_write_register(ViPipe, 0x3779, 0x00);// -
	imx335_write_register(ViPipe, 0x377A, 0x89);// -
	imx335_write_register(ViPipe, 0x377B, 0x00);// -
	imx335_write_register(ViPipe, 0x377C, 0x08);// -
	imx335_write_register(ViPipe, 0x377D, 0x01);// -
	imx335_write_register(ViPipe, 0x377E, 0x23);// -
	imx335_write_register(ViPipe, 0x377F, 0x02);// -
	imx335_write_register(ViPipe, 0x3780, 0xD9);// -
	imx335_write_register(ViPipe, 0x3781, 0x03);// -
	imx335_write_register(ViPipe, 0x3782, 0xF5);// -
	imx335_write_register(ViPipe, 0x3783, 0x06);// -
	imx335_write_register(ViPipe, 0x3784, 0xA5);// -
	imx335_write_register(ViPipe, 0x3788, 0x0F);// -
	imx335_write_register(ViPipe, 0x378A, 0xD9);// -
	imx335_write_register(ViPipe, 0x378B, 0x03);// -
	imx335_write_register(ViPipe, 0x378C, 0xEB);// -
	imx335_write_register(ViPipe, 0x378D, 0x05);// -
	imx335_write_register(ViPipe, 0x378E, 0x87);// -
	imx335_write_register(ViPipe, 0x378F, 0x06);// -
	imx335_write_register(ViPipe, 0x3790, 0xF5);// -
	imx335_write_register(ViPipe, 0x3792, 0x43);// -
	imx335_write_register(ViPipe, 0x3794, 0x7A);// -
	imx335_write_register(ViPipe, 0x3796, 0xA1);// -

	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00); /* master mode start */

	CVI_TRACE_SNS(CVI_DBG_INFO, "===Imx335 sensor 4M30fps 10bit 2to1 WDR(60fps->30fps) init success!=====\n");
}
static void imx335_wdr_4M30_1600p_2to1_init(VI_PIPE ViPipe)
{
	delay_ms(4);
	imx335_write_register(ViPipe, 0x3000, 0x01);/* STANDBY */
	imx335_write_register(ViPipe, 0x3002, 0x01);/* XTMSTA */

	imx335_write_register(ViPipe, 0x300C, 0x5B);// BCWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x300D, 0x40);// CPWAIT_TIME[7:0]
	imx335_write_register(ViPipe, 0x3018, 0x04);// WINMODE[3:0]
	imx335_write_register(ViPipe, 0x302C, 0x3C);// HTRIMMING_START[11:0]
	imx335_write_register(ViPipe, 0x302E, 0x20);// HNUM[11:0]
	imx335_write_register(ViPipe, 0x3034, 0x13);// HMAX[15:0]
	imx335_write_register(ViPipe, 0x3035, 0x01);//
	imx335_write_register(ViPipe, 0x3048, 0x01);// WDMODE[0]
	imx335_write_register(ViPipe, 0x3049, 0x01);// WDSEL[1:0]
	imx335_write_register(ViPipe, 0x304A, 0x04);// WD_SET1[2:0]
	imx335_write_register(ViPipe, 0x304B, 0x03);// WD_SET2[3:0]
	imx335_write_register(ViPipe, 0x304C, 0x13);// OPB_SIZE_V[5:0]
	imx335_write_register(ViPipe, 0x3050, 0x00);// ADBIT[0]
	imx335_write_register(ViPipe, 0x3056, 0x54);// Y_OUT_SIZE[12:0]
	imx335_write_register(ViPipe, 0x3057, 0x06);//
	imx335_write_register(ViPipe, 0x3058, 0x68);// SHR0[19:0]
	imx335_write_register(ViPipe, 0x3059, 0x01);//
	imx335_write_register(ViPipe, 0x3068, 0x4A);// RHS1[19:0]
	imx335_write_register(ViPipe, 0x3074, 0x08);// AREA3_ST_ADR_1[12:0]
	imx335_write_register(ViPipe, 0x3075, 0x02);//
	imx335_write_register(ViPipe, 0x3076, 0xA8);// AREA3_WIDTH_1[12:0]
	imx335_write_register(ViPipe, 0x3077, 0x0C);//
	imx335_write_register(ViPipe, 0x30C6, 0x12);// BLACK_OFSET_ADR[12:0]
	imx335_write_register(ViPipe, 0x30CE, 0x64);// UNRD_LINE_MAX[12:0]
	imx335_write_register(ViPipe, 0x30D8, 0x80);// UNREAD_ED_ADR[12:0]
	imx335_write_register(ViPipe, 0x30D9, 0x0F);//
	imx335_write_register(ViPipe, 0x315A, 0x02);// INCKSEL2[1:0]
	imx335_write_register(ViPipe, 0x316A, 0x7E);// INCKSEL4[1:0]
	imx335_write_register(ViPipe, 0x319D, 0x00);// MDBIT
	imx335_write_register(ViPipe, 0x31A1, 0x00);// XVS_DRV[1:0]
	imx335_write_register(ViPipe, 0x31D7, 0x01);// XVSMSKCNT_INT[1:0]
	imx335_write_register(ViPipe, 0x3288, 0x21);//
	imx335_write_register(ViPipe, 0x328A, 0x02);//
	imx335_write_register(ViPipe, 0x3414, 0x05);//
	imx335_write_register(ViPipe, 0x3416, 0x18);//
	imx335_write_register(ViPipe, 0x341C, 0xFF);// ADBIT1[8:0]
	imx335_write_register(ViPipe, 0x341D, 0x01);//
	imx335_write_register(ViPipe, 0x3648, 0x01);//
	imx335_write_register(ViPipe, 0x364A, 0x04);//
	imx335_write_register(ViPipe, 0x364C, 0x04);//
	imx335_write_register(ViPipe, 0x3678, 0x01);//
	imx335_write_register(ViPipe, 0x367C, 0x31);//
	imx335_write_register(ViPipe, 0x367E, 0x31);//
	imx335_write_register(ViPipe, 0x3706, 0x10);//
	imx335_write_register(ViPipe, 0x3708, 0x03);//
	imx335_write_register(ViPipe, 0x3714, 0x02);//
	imx335_write_register(ViPipe, 0x3715, 0x02);//
	imx335_write_register(ViPipe, 0x3716, 0x01);//
	imx335_write_register(ViPipe, 0x3717, 0x03);//
	imx335_write_register(ViPipe, 0x371C, 0x3D);//
	imx335_write_register(ViPipe, 0x371D, 0x3F);//
	imx335_write_register(ViPipe, 0x372C, 0x00);//
	imx335_write_register(ViPipe, 0x372D, 0x00);//
	imx335_write_register(ViPipe, 0x372E, 0x46);//
	imx335_write_register(ViPipe, 0x372F, 0x00);//
	imx335_write_register(ViPipe, 0x3730, 0x89);//
	imx335_write_register(ViPipe, 0x3731, 0x00);//
	imx335_write_register(ViPipe, 0x3732, 0x08);//
	imx335_write_register(ViPipe, 0x3733, 0x01);//
	imx335_write_register(ViPipe, 0x3734, 0xFE);//
	imx335_write_register(ViPipe, 0x3735, 0x05);//
	imx335_write_register(ViPipe, 0x3740, 0x02);//
	imx335_write_register(ViPipe, 0x375D, 0x00);//
	imx335_write_register(ViPipe, 0x375E, 0x00);//
	imx335_write_register(ViPipe, 0x375F, 0x11);//
	imx335_write_register(ViPipe, 0x3760, 0x01);//
	imx335_write_register(ViPipe, 0x3768, 0x1B);//
	imx335_write_register(ViPipe, 0x3769, 0x1B);//
	imx335_write_register(ViPipe, 0x376A, 0x1B);//
	imx335_write_register(ViPipe, 0x376B, 0x1B);//
	imx335_write_register(ViPipe, 0x376C, 0x1A);//
	imx335_write_register(ViPipe, 0x376D, 0x17);//
	imx335_write_register(ViPipe, 0x376E, 0x0F);//
	imx335_write_register(ViPipe, 0x3776, 0x00);//
	imx335_write_register(ViPipe, 0x3777, 0x00);//
	imx335_write_register(ViPipe, 0x3778, 0x46);//
	imx335_write_register(ViPipe, 0x3779, 0x00);//
	imx335_write_register(ViPipe, 0x377A, 0x89);//
	imx335_write_register(ViPipe, 0x377B, 0x00);//
	imx335_write_register(ViPipe, 0x377C, 0x08);//
	imx335_write_register(ViPipe, 0x377D, 0x01);//
	imx335_write_register(ViPipe, 0x377E, 0x23);//
	imx335_write_register(ViPipe, 0x377F, 0x02);//
	imx335_write_register(ViPipe, 0x3780, 0xD9);//
	imx335_write_register(ViPipe, 0x3781, 0x03);//
	imx335_write_register(ViPipe, 0x3782, 0xF5);//
	imx335_write_register(ViPipe, 0x3783, 0x06);//
	imx335_write_register(ViPipe, 0x3784, 0xA5);//
	imx335_write_register(ViPipe, 0x3788, 0x0F);//
	imx335_write_register(ViPipe, 0x378A, 0xD9);//
	imx335_write_register(ViPipe, 0x378B, 0x03);//
	imx335_write_register(ViPipe, 0x378C, 0xEB);//
	imx335_write_register(ViPipe, 0x378D, 0x05);//
	imx335_write_register(ViPipe, 0x378E, 0x87);//
	imx335_write_register(ViPipe, 0x378F, 0x06);//
	imx335_write_register(ViPipe, 0x3790, 0xF5);//
	imx335_write_register(ViPipe, 0x3792, 0x43);//
	imx335_write_register(ViPipe, 0x3794, 0x7A);//
	imx335_write_register(ViPipe, 0x3796, 0xA1);//

	imx335_default_reg_init(ViPipe);

	imx335_write_register(ViPipe, 0x3000, 0x00); /* standby */
	delay_ms(20);
	imx335_write_register(ViPipe, 0x3002, 0x00); /* master mode start */

	CVI_TRACE_SNS(CVI_DBG_INFO, "===Imx335 sensor 4M1600P 30fps 10bit 2to1 WDR(60fps->30fps) init success!\n");
}
