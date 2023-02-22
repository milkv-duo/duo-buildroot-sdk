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
#include "f23_cmos_ex.h"

static void f23_wdr_1080p30_2to1_init(VI_PIPE ViPipe);
static void f23_linear_1080p30_init(VI_PIPE ViPipe);

const CVI_U8 f23_i2c_addr = 0x40;        /* I2C Address of F23 */
const CVI_U32 f23_addr_byte = 1;
const CVI_U32 f23_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int f23_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunF23_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, f23_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int f23_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int f23_read_register(VI_PIPE ViPipe, int addr)
{
	/* TODO:*/
	(void) ViPipe;
	(void) addr;

	return CVI_SUCCESS;
}


int f23_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (f23_addr_byte == 1) {
		buf[idx] = addr & 0xff;
		idx++;
	}
	if (f23_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, f23_addr_byte + f23_data_byte);
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

void f23_standby(VI_PIPE ViPipe)
{
	f23_write_register(ViPipe, 0x12, 0x40);
}

void f23_restart(VI_PIPE ViPipe)
{
	f23_write_register(ViPipe, 0x12, 0x40);
	delay_ms(20);
	f23_write_register(ViPipe, 0x12, 0x00);
}

void f23_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastF23[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		f23_write_register(ViPipe,
				g_pastF23[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastF23[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

void f23_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_BOOL          bInit;
	CVI_U8            u8ImgMode;

	bInit       = g_pastF23[ViPipe]->bInit;
	enWDRMode   = g_pastF23[ViPipe]->enWDRMode;
	u8ImgMode   = g_pastF23[ViPipe]->u8ImgMode;

	f23_i2c_init(ViPipe);

	/* When sensor first init, config all registers */
	if (bInit == CVI_FALSE) {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			if (u8ImgMode == F23_MODE_1080P30_WDR) {
				/* F23_MODE_1080P30_WDR */
				f23_wdr_1080p30_2to1_init(ViPipe);
			} else {
			}
		} else {
			f23_linear_1080p30_init(ViPipe);
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else {
		if (enWDRMode == WDR_MODE_2To1_LINE) {
			if (u8ImgMode == F23_MODE_1080P30_WDR) {
				/* F23_MODE_1080P30_WDR */
				f23_wdr_1080p30_2to1_init(ViPipe);
			} else {
			}
		} else {
			f23_linear_1080p30_init(ViPipe);
		}
	}
	g_pastF23[ViPipe]->bInit = CVI_TRUE;
}

void f23_exit(VI_PIPE ViPipe)
{
	f23_i2c_exit(ViPipe);
}

#ifdef FPGA_PORTING
static void f23_linear_1080p30_init(VI_PIPE ViPipe)
{
	f23_write_register(ViPipe, 0x12, 0x80);
	delay_ms(15);
	f23_write_register(ViPipe, 0x0E, 0x11);
	f23_write_register(ViPipe, 0x0F, 0x14);
	f23_write_register(ViPipe, 0x10, 0x40);
	f23_write_register(ViPipe, 0x11, 0x80);
	f23_write_register(ViPipe, 0x48, 0x05);
	f23_write_register(ViPipe, 0x96, 0xAA);
	f23_write_register(ViPipe, 0x94, 0xC0);
	f23_write_register(ViPipe, 0x97, 0x8D);
	f23_write_register(ViPipe, 0x96, 0x00);
	f23_write_register(ViPipe, 0x12, 0x40);
	f23_write_register(ViPipe, 0x48, 0x8A);
	f23_write_register(ViPipe, 0x48, 0x0A);
	f23_write_register(ViPipe, 0x0E, 0x11);
	f23_write_register(ViPipe, 0x0F, 0x14);

//	f23_write_register(ViPipe, 0x10, 0x20);
	f23_write_register(ViPipe, 0x10, 0x12);	// VCO for FPGA
	f23_write_register(ViPipe, 0x11, 0x80);
	f23_write_register(ViPipe, 0x0D, 0xA0);
	f23_write_register(ViPipe, 0x5F, 0x42);
	f23_write_register(ViPipe, 0x60, 0x2B);
	f23_write_register(ViPipe, 0x58, 0x12);
	f23_write_register(ViPipe, 0x57, 0x60);
	f23_write_register(ViPipe, 0x9D, 0x00);
//	f23_write_register(ViPipe, 0x20, 0x00);
//	f23_write_register(ViPipe, 0x21, 0x05);
	f23_write_register(ViPipe, 0x20, 0x00);
	f23_write_register(ViPipe, 0x21, 0x1e); // for FPGA, FrameW
	f23_write_register(ViPipe, 0x22, 0x65);
	f23_write_register(ViPipe, 0x23, 0x04);	// FrameH
	f23_write_register(ViPipe, 0x24, 0xC0);
	f23_write_register(ViPipe, 0x25, 0x38);
	f23_write_register(ViPipe, 0x26, 0x43);
	f23_write_register(ViPipe, 0x27, 0xC3);
	f23_write_register(ViPipe, 0x28, 0x19);
	f23_write_register(ViPipe, 0x29, 0x04);
	f23_write_register(ViPipe, 0x2C, 0x00);
	f23_write_register(ViPipe, 0x2D, 0x00);
	f23_write_register(ViPipe, 0x2E, 0x18);
	f23_write_register(ViPipe, 0x2F, 0x44);
	f23_write_register(ViPipe, 0x41, 0xC9);
	f23_write_register(ViPipe, 0x42, 0x13);
	f23_write_register(ViPipe, 0x46, 0x00);
	f23_write_register(ViPipe, 0x76, 0x60);
	f23_write_register(ViPipe, 0x77, 0x09);
	f23_write_register(ViPipe, 0x1D, 0x00);
	f23_write_register(ViPipe, 0x1E, 0x04);
	f23_write_register(ViPipe, 0x6C, 0x50); // 1-lane
//	f23_write_register(ViPipe, 0x6C, 0x40); // 2-lane

	f23_write_register(ViPipe, 0x68, 0x00);
	f23_write_register(ViPipe, 0x6E, 0x2C);
	f23_write_register(ViPipe, 0x70, 0x6C);
	f23_write_register(ViPipe, 0x71, 0x6D);
	f23_write_register(ViPipe, 0x72, 0x6A);
	f23_write_register(ViPipe, 0x73, 0x36);
	f23_write_register(ViPipe, 0x74, 0x02);
	f23_write_register(ViPipe, 0x78, 0x9E);
	f23_write_register(ViPipe, 0x89, 0x01);
	f23_write_register(ViPipe, 0x6B, 0x20);
	f23_write_register(ViPipe, 0x86, 0x40);
	f23_write_register(ViPipe, 0x2A, 0xB1);
	f23_write_register(ViPipe, 0x2B, 0x24);
	f23_write_register(ViPipe, 0x31, 0x08);
	f23_write_register(ViPipe, 0x32, 0x4F);
	f23_write_register(ViPipe, 0x33, 0x20);
	f23_write_register(ViPipe, 0x34, 0x5E);
	f23_write_register(ViPipe, 0x35, 0x5E);
	f23_write_register(ViPipe, 0x3A, 0xAF);
	f23_write_register(ViPipe, 0x56, 0x32);
	f23_write_register(ViPipe, 0x59, 0xBF);
	f23_write_register(ViPipe, 0x5A, 0x04);
	f23_write_register(ViPipe, 0x85, 0x5A);
	f23_write_register(ViPipe, 0x8A, 0x04);
	f23_write_register(ViPipe, 0x8F, 0x90);
	f23_write_register(ViPipe, 0x91, 0x13);
	f23_write_register(ViPipe, 0x5B, 0xA0);
	f23_write_register(ViPipe, 0x5C, 0xF0);
	f23_write_register(ViPipe, 0x5D, 0xFC);
	f23_write_register(ViPipe, 0x5E, 0x1F);
	f23_write_register(ViPipe, 0x62, 0x04);
	f23_write_register(ViPipe, 0x63, 0x0F);
	f23_write_register(ViPipe, 0x64, 0xC0);
	f23_write_register(ViPipe, 0x66, 0x44);
	f23_write_register(ViPipe, 0x67, 0x73);
	f23_write_register(ViPipe, 0x69, 0x7C);
	f23_write_register(ViPipe, 0x6A, 0x28);
	f23_write_register(ViPipe, 0x7A, 0xC0);
	f23_write_register(ViPipe, 0x4A, 0x05);
	f23_write_register(ViPipe, 0x7E, 0xCD);
	f23_write_register(ViPipe, 0x49, 0x10);
	f23_write_register(ViPipe, 0x50, 0x02);
	f23_write_register(ViPipe, 0x7B, 0x4A);
	f23_write_register(ViPipe, 0x7C, 0x0C);
	f23_write_register(ViPipe, 0x7F, 0x57);
	f23_write_register(ViPipe, 0x90, 0x00);
	f23_write_register(ViPipe, 0x8E, 0x00);
	f23_write_register(ViPipe, 0x8C, 0xFF);
	f23_write_register(ViPipe, 0x8D, 0xC7);
	f23_write_register(ViPipe, 0x8B, 0x01);
	f23_write_register(ViPipe, 0x0C, 0x40); // bit0: test pattern
	f23_write_register(ViPipe, 0x65, 0x02);
	f23_write_register(ViPipe, 0x80, 0x1A);
	f23_write_register(ViPipe, 0x81, 0xC0);
	f23_write_register(ViPipe, 0x19, 0x20);
	f23_write_register(ViPipe, 0x99, 0x0F);
	f23_write_register(ViPipe, 0x9B, 0x0F);

	//f23_default_reg_init(ViPipe);

	f23_write_register(ViPipe, 0x12, 0x00);
	f23_write_register(ViPipe, 0x48, 0x8A);
	f23_write_register(ViPipe, 0x48, 0x0A);

	printf("ViPipe:%d,===F23 1080P 30fps 10bit LINE FPGA Init OK!===\n", ViPipe);
}
#else
/* 1080P30 and 1080P25 */
static void f23_linear_1080p30_init(VI_PIPE ViPipe)
{
	f23_write_register(ViPipe, 0x12, 0x80);
	delay_ms(15);
	f23_write_register(ViPipe, 0x0E, 0x11);
	f23_write_register(ViPipe, 0x0F, 0x14);
	f23_write_register(ViPipe, 0x10, 0x40);
	f23_write_register(ViPipe, 0x11, 0x80);
	f23_write_register(ViPipe, 0x48, 0x05);
	f23_write_register(ViPipe, 0x96, 0xAA);
	f23_write_register(ViPipe, 0x94, 0xC0);
	f23_write_register(ViPipe, 0x97, 0x8D);
	f23_write_register(ViPipe, 0x96, 0x00);
	f23_write_register(ViPipe, 0x12, 0x40);
	f23_write_register(ViPipe, 0x48, 0x8A);
	f23_write_register(ViPipe, 0x48, 0x0A);
	f23_write_register(ViPipe, 0x0E, 0x11);
	f23_write_register(ViPipe, 0x0F, 0x14);

	f23_write_register(ViPipe, 0x10, 0x20);
//	f23_write_register(ViPipe, 0x10, 0x12);	// VCO for FPGA
	f23_write_register(ViPipe, 0x11, 0x80);
	f23_write_register(ViPipe, 0x0D, 0xA0); // for FPGA driving upto max
	f23_write_register(ViPipe, 0x5F, 0x42);
	f23_write_register(ViPipe, 0x60, 0x2B);
	f23_write_register(ViPipe, 0x58, 0x12);
	f23_write_register(ViPipe, 0x57, 0x60);
	f23_write_register(ViPipe, 0x9D, 0x00);
	f23_write_register(ViPipe, 0x20, 0x00);
	f23_write_register(ViPipe, 0x21, 0x05);
	// f23_write_register(ViPipe, 0x20, 0x00);
	// f23_write_register(ViPipe, 0x21, 0x1e); // for FPGA, FrameW
	f23_write_register(ViPipe, 0x22, 0x65);
	f23_write_register(ViPipe, 0x23, 0x04);	// FrameH
	f23_write_register(ViPipe, 0x24, 0xC0);
	f23_write_register(ViPipe, 0x25, 0x38);
	f23_write_register(ViPipe, 0x26, 0x43);
	f23_write_register(ViPipe, 0x27, 0xC3);
	f23_write_register(ViPipe, 0x28, 0x19);
	f23_write_register(ViPipe, 0x29, 0x04);
	f23_write_register(ViPipe, 0x2C, 0x00);
	f23_write_register(ViPipe, 0x2D, 0x00);
	f23_write_register(ViPipe, 0x2E, 0x18);
	f23_write_register(ViPipe, 0x2F, 0x44);
	f23_write_register(ViPipe, 0x41, 0xC9);
	f23_write_register(ViPipe, 0x42, 0x13);
	f23_write_register(ViPipe, 0x46, 0x00);
	f23_write_register(ViPipe, 0x76, 0x60);
	f23_write_register(ViPipe, 0x77, 0x09);
	f23_write_register(ViPipe, 0x1D, 0x00);
	f23_write_register(ViPipe, 0x1E, 0x04);
//	f23_write_register(ViPipe, 0x6C, 0x50); // 1-lane
	f23_write_register(ViPipe, 0x6C, 0x40); // 2-lane

	f23_write_register(ViPipe, 0x68, 0x00);
	f23_write_register(ViPipe, 0x6E, 0x2C);
	f23_write_register(ViPipe, 0x70, 0x6C);
	f23_write_register(ViPipe, 0x71, 0x6D);
	f23_write_register(ViPipe, 0x72, 0x6A);
	f23_write_register(ViPipe, 0x73, 0x36);
	f23_write_register(ViPipe, 0x74, 0x02);
	f23_write_register(ViPipe, 0x78, 0x9E);
	f23_write_register(ViPipe, 0x89, 0x01);
	f23_write_register(ViPipe, 0x6B, 0x20);
	f23_write_register(ViPipe, 0x86, 0x40);
	f23_write_register(ViPipe, 0x2A, 0xB1);
	f23_write_register(ViPipe, 0x2B, 0x24);
	f23_write_register(ViPipe, 0x31, 0x08);
	f23_write_register(ViPipe, 0x32, 0x4F);
	f23_write_register(ViPipe, 0x33, 0x20);
	f23_write_register(ViPipe, 0x34, 0x5E);
	f23_write_register(ViPipe, 0x35, 0x5E);
	f23_write_register(ViPipe, 0x3A, 0xAF);
	f23_write_register(ViPipe, 0x56, 0x32);
	f23_write_register(ViPipe, 0x59, 0xBF);
	f23_write_register(ViPipe, 0x5A, 0x04);
	f23_write_register(ViPipe, 0x85, 0x5A);
	f23_write_register(ViPipe, 0x8A, 0x04);
	f23_write_register(ViPipe, 0x8F, 0x90);
	f23_write_register(ViPipe, 0x91, 0x13);
	f23_write_register(ViPipe, 0x5B, 0xA0);
	f23_write_register(ViPipe, 0x5C, 0xF0);
	f23_write_register(ViPipe, 0x5D, 0xFC);
	f23_write_register(ViPipe, 0x5E, 0x1F);
	f23_write_register(ViPipe, 0x62, 0x04);
	f23_write_register(ViPipe, 0x63, 0x0F);
	f23_write_register(ViPipe, 0x64, 0xC0);
	f23_write_register(ViPipe, 0x66, 0x44);
	f23_write_register(ViPipe, 0x67, 0x73);
	f23_write_register(ViPipe, 0x69, 0x7C);
	f23_write_register(ViPipe, 0x6A, 0x28);
	f23_write_register(ViPipe, 0x7A, 0xC0);
	f23_write_register(ViPipe, 0x4A, 0x05);
	f23_write_register(ViPipe, 0x7E, 0xCD);
	f23_write_register(ViPipe, 0x49, 0x10);
	f23_write_register(ViPipe, 0x50, 0x02);
	f23_write_register(ViPipe, 0x7B, 0x4A);
	f23_write_register(ViPipe, 0x7C, 0x0C);
	f23_write_register(ViPipe, 0x7F, 0x57);
	f23_write_register(ViPipe, 0x90, 0x00);
	f23_write_register(ViPipe, 0x8E, 0x00);
	f23_write_register(ViPipe, 0x8C, 0xFF);
	f23_write_register(ViPipe, 0x8D, 0xC7);
	f23_write_register(ViPipe, 0x8B, 0x01);
	f23_write_register(ViPipe, 0x0C, 0x40); // bit0: test pattern
	f23_write_register(ViPipe, 0x65, 0x02);
	f23_write_register(ViPipe, 0x80, 0x1A);
	f23_write_register(ViPipe, 0x81, 0xC0);
	f23_write_register(ViPipe, 0x19, 0x20);
	f23_write_register(ViPipe, 0x99, 0x0F);
	f23_write_register(ViPipe, 0x9B, 0x0F);

	//f23_default_reg_init(ViPipe);

	f23_write_register(ViPipe, 0x12, 0x00);
	f23_write_register(ViPipe, 0x48, 0x8A);
	f23_write_register(ViPipe, 0x48, 0x0A);

	printf("ViPipe:%d,===F23 1080P 30fps 10bit LINE FPGA Init OK!===\n", ViPipe);
}
#endif

static void f23_wdr_1080p30_2to1_init(VI_PIPE ViPipe)
{
	(void) ViPipe;

	printf("===F23 sensor 1080P30fps HDR not support yet!=====\n");
}
