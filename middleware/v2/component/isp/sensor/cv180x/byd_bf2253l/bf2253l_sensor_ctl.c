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
#include "bf2253l_cmos_ex.h"

static void bf2253l_linear_1200p10_init(VI_PIPE ViPipe);

const CVI_U8 bf2253l_i2c_addr = 0x6e;        /* I2C Address of BF2253L */
const CVI_U32 bf2253l_addr_byte = 1;
const CVI_U32 bf2253l_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int bf2253l_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunBF2253L_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, bf2253l_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int bf2253l_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int bf2253l_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	char buf[8];
	int idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (bf2253l_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, bf2253l_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, bf2253l_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (bf2253l_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int bf2253l_write_register(VI_PIPE ViPipe, int addr, int data)
{
	int idx = 0;
	int ret;
	char buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (bf2253l_addr_byte == 1) {
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (bf2253l_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, bf2253l_addr_byte + bf2253l_data_byte);
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

void bf2253l_prog(VI_PIPE ViPipe, int *rom)
{
	int i = 0;

	while (1) {
		int lookup = rom[i++];
		int addr = (lookup >> 16) & 0xFFFF;
		int data = lookup & 0xFFFF;

		if (addr == 0xFFFE)
			delay_ms(data);
		else if (addr != 0xFFFF)
			bf2253l_write_register(ViPipe, addr, data);
	}
}

void bf2253l_standby(VI_PIPE ViPipe)
{
	bf2253l_write_register(ViPipe, 0xe0, 0x01);
}

void bf2253l_restart(VI_PIPE ViPipe)
{
	bf2253l_write_register(ViPipe, 0xe0, 0x01);
	delay_ms(20);
	bf2253l_write_register(ViPipe, 0xe0, 0x00);
}

void bf2253l_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;

	for (i = 0; i < g_pastBF2253L[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum; i++) {
		bf2253l_write_register(ViPipe,
				g_pastBF2253L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastBF2253L[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define BF2253L_CHIP_ID_HI_ADDR		0xfc
#define BF2253L_CHIP_ID_LO_ADDR		0xfd
#define BF2253L_CHIP_ID			0x2253

int bf2253l_probe(VI_PIPE ViPipe)
{
	int nVal;
	CVI_U16 chip_id;

	usleep(4*1000);
	if (bf2253l_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal = bf2253l_read_register(ViPipe, BF2253L_CHIP_ID_HI_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id = (nVal & 0xFF) << 8;
	nVal = bf2253l_read_register(ViPipe, BF2253L_CHIP_ID_LO_ADDR);
	if (nVal < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}
	chip_id |= (nVal & 0xFF);

	if (chip_id != BF2253L_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}


void bf2253l_init(VI_PIPE ViPipe)
{
	WDR_MODE_E       enWDRMode;
	CVI_BOOL          bInit;

	bInit       = g_pastBF2253L[ViPipe]->bInit;
	enWDRMode   = g_pastBF2253L[ViPipe]->enWDRMode;

	bf2253l_i2c_init(ViPipe);

	/* When sensor first init, config all registers */
	if (bInit == CVI_FALSE) {
		if (enWDRMode == WDR_MODE_NONE) {
			bf2253l_linear_1200p10_init(ViPipe);
		}
	}
	/* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
	else {
		if (enWDRMode == WDR_MODE_NONE) {
			bf2253l_linear_1200p10_init(ViPipe);
		}
	}
	g_pastBF2253L[ViPipe]->bInit = CVI_TRUE;
}

void bf2253l_exit(VI_PIPE ViPipe)
{
	bf2253l_i2c_exit(ViPipe);
}

/* 1200P10 and 1200P25 */
static void bf2253l_linear_1200p10_init(VI_PIPE ViPipe)
{
	bf2253l_write_register(ViPipe, 0xe1, 0x06);
	bf2253l_write_register(ViPipe, 0xe2, 0x06);
	bf2253l_write_register(ViPipe, 0xe3, 0x0e);
	bf2253l_write_register(ViPipe, 0xe4, 0x40);
	bf2253l_write_register(ViPipe, 0xe5, 0x67);
	bf2253l_write_register(ViPipe, 0xe6, 0x02);
	bf2253l_write_register(ViPipe, 0xe8, 0x84);
	bf2253l_write_register(ViPipe, 0x01, 0x14);
	bf2253l_write_register(ViPipe, 0x03, 0x98);
	bf2253l_write_register(ViPipe, 0x27, 0x21);
	bf2253l_write_register(ViPipe, 0x29, 0x20);
	bf2253l_write_register(ViPipe, 0x59, 0x10);
	bf2253l_write_register(ViPipe, 0x5a, 0x10);
	bf2253l_write_register(ViPipe, 0x5c, 0x11);
	bf2253l_write_register(ViPipe, 0x5d, 0x73);
	bf2253l_write_register(ViPipe, 0x6a, 0x2f);
	bf2253l_write_register(ViPipe, 0x6b, 0x0e);
	bf2253l_write_register(ViPipe, 0x6c, 0x7e);
	bf2253l_write_register(ViPipe, 0x6f, 0x10);
	bf2253l_write_register(ViPipe, 0x70, 0x08);
	bf2253l_write_register(ViPipe, 0x71, 0x05);
	bf2253l_write_register(ViPipe, 0x72, 0x10);
	bf2253l_write_register(ViPipe, 0x73, 0x08);
	bf2253l_write_register(ViPipe, 0x74, 0x05);
	bf2253l_write_register(ViPipe, 0x75, 0x06);
	bf2253l_write_register(ViPipe, 0x76, 0x20);
	bf2253l_write_register(ViPipe, 0x77, 0x03);
	bf2253l_write_register(ViPipe, 0x78, 0x0e);
	bf2253l_write_register(ViPipe, 0x79, 0x08);
	bf2253l_write_register(ViPipe, 0xe0, 0x00);
	bf2253l_default_reg_init(ViPipe);

	printf("ViPipe:%d,===BF2253L 1200P 10fps 10bit LINE Init OK!===\n", ViPipe);
}
