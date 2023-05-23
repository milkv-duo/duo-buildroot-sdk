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
#include "gc0312_cmos_ex.h"

static void gc0312_linear_480p20_init(VI_PIPE ViPipe);

CVI_U8 gc0312_i2c_addr = 0x21;
const CVI_U32 gc0312_addr_byte = 1;
const CVI_U32 gc0312_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int gc0312_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunGc0312_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, gc0312_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int gc0312_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int gc0312_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (gc0312_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, gc0312_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return ret;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, gc0312_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return ret;
	}

	// pack read back data
	data = 0;
	if (gc0312_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}


int gc0312_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (gc0312_addr_byte == 1) {
		buf[idx] = addr & 0xff;
		idx++;
	}
	if (gc0312_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, gc0312_addr_byte + gc0312_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
//	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);
	return CVI_SUCCESS;
}

static void delay_ms(int ms)
{
	usleep(ms * 1000);
}

#define GC0312_CHIP_ID 0xb310
#define GC0312_CHIP_ID_ADDR_H 0xf0
#define GC0312_CHIP_ID_ADDR_L 0xf1

int gc0312_probe(VI_PIPE ViPipe)
{
	int nVal;
	int nVal2;

	usleep(50);
	if (gc0312_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = gc0312_read_register(ViPipe, GC0312_CHIP_ID_ADDR_H);
	nVal2 = gc0312_read_register(ViPipe, GC0312_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != GC0312_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}
	printf("%d\n", ViPipe);
	return CVI_SUCCESS;
}

void gc0312_init(VI_PIPE ViPipe)
{
	gc0312_i2c_init(ViPipe);

	gc0312_linear_480p20_init(ViPipe);

	g_pastGc0312[ViPipe]->bInit = CVI_TRUE;
}

void gc0312_exit(VI_PIPE ViPipe)
{
	gc0312_i2c_exit(ViPipe);
}

static void gc0312_linear_480p20_init(VI_PIPE ViPipe)
{
	gc0312_write_register(ViPipe, 0xfe, 0xf0);
	gc0312_write_register(ViPipe, 0xfe, 0xf0);
	gc0312_write_register(ViPipe, 0xfe, 0x00);
	gc0312_write_register(ViPipe, 0xfc, 0x0e);
	gc0312_write_register(ViPipe, 0xfc, 0x0e);
	gc0312_write_register(ViPipe, 0xf2, 0x07);
	gc0312_write_register(ViPipe, 0xf3, 0x00);// output_disable
	gc0312_write_register(ViPipe, 0xf7, 0x1b);
	gc0312_write_register(ViPipe, 0xf8, 0x04);
	gc0312_write_register(ViPipe, 0xf9, 0x0e);
	gc0312_write_register(ViPipe, 0xfa, 0x11);

	/////////////////////////////////////////////////
	/////////////////  CISCTL reg	/////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x00, 0x2f);
	gc0312_write_register(ViPipe, 0x01, 0x0f);
	gc0312_write_register(ViPipe, 0x02, 0x04);
	gc0312_write_register(ViPipe, 0x03, 0x03);
	gc0312_write_register(ViPipe, 0x04, 0x50);
	gc0312_write_register(ViPipe, 0x09, 0x00);
	gc0312_write_register(ViPipe, 0x0a, 0x00);
	gc0312_write_register(ViPipe, 0x0b, 0x00);
	gc0312_write_register(ViPipe, 0x0c, 0x04);
	gc0312_write_register(ViPipe, 0x0d, 0x01);
	gc0312_write_register(ViPipe, 0x0e, 0xe8);
	gc0312_write_register(ViPipe, 0x0f, 0x02);
	gc0312_write_register(ViPipe, 0x10, 0x88);
	gc0312_write_register(ViPipe, 0x16, 0x00);
	gc0312_write_register(ViPipe, 0x17, 0x14);
	gc0312_write_register(ViPipe, 0x18, 0x1a);
	gc0312_write_register(ViPipe, 0x19, 0x14);
	gc0312_write_register(ViPipe, 0x1b, 0x48);
	gc0312_write_register(ViPipe, 0x1c, 0x6c);//1c travis 20140929
	gc0312_write_register(ViPipe, 0x1e, 0x6b);
	gc0312_write_register(ViPipe, 0x1f, 0x28);
	gc0312_write_register(ViPipe, 0x20, 0x8b);//89 travis 20140801
	gc0312_write_register(ViPipe, 0x21, 0x49);
	gc0312_write_register(ViPipe, 0x22, 0xd0);//b0 travis 20140929
	gc0312_write_register(ViPipe, 0x23, 0x04);
	gc0312_write_register(ViPipe, 0x24, 0x16);
	gc0312_write_register(ViPipe, 0x34, 0x20);

	/////////////////////////////////////////////////
	////////////////////   BLK	 ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x26, 0x23);
	gc0312_write_register(ViPipe, 0x28, 0xff);
	gc0312_write_register(ViPipe, 0x29, 0x00);
	gc0312_write_register(ViPipe, 0x32, 0x04);//00  travis 20140929
	gc0312_write_register(ViPipe, 0x33, 0x10);
	gc0312_write_register(ViPipe, 0x37, 0x20);
	gc0312_write_register(ViPipe, 0x38, 0x10);
	gc0312_write_register(ViPipe, 0x47, 0x80);
	gc0312_write_register(ViPipe, 0x4e, 0x66);
	gc0312_write_register(ViPipe, 0xa8, 0x02);
	gc0312_write_register(ViPipe, 0xa9, 0x80);

	/////////////////////////////////////////////////
	//////////////////	ISP reg   ///////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x40, 0xff);
	gc0312_write_register(ViPipe, 0x41, 0x21);
	gc0312_write_register(ViPipe, 0x42, 0xcf);
	gc0312_write_register(ViPipe, 0x44, 0x02);
	gc0312_write_register(ViPipe, 0x45, 0xa8);
	gc0312_write_register(ViPipe, 0x46, 0x02);
	gc0312_write_register(ViPipe, 0x4a, 0x11);
	gc0312_write_register(ViPipe, 0x4b, 0x01);
	gc0312_write_register(ViPipe, 0x4c, 0x20);
	gc0312_write_register(ViPipe, 0x4d, 0x05);
	gc0312_write_register(ViPipe, 0x4f, 0x01);
	gc0312_write_register(ViPipe, 0x50, 0x01);
	gc0312_write_register(ViPipe, 0x55, 0x01);
	gc0312_write_register(ViPipe, 0x56, 0xe0);
	gc0312_write_register(ViPipe, 0x57, 0x02);
	gc0312_write_register(ViPipe, 0x58, 0x80);

	/////////////////////////////////////////////////
	///////////////////   GAIN   ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x70, 0x70);
	gc0312_write_register(ViPipe, 0x5a, 0x84);
	gc0312_write_register(ViPipe, 0x5b, 0xc9);
	gc0312_write_register(ViPipe, 0x5c, 0xed);
	gc0312_write_register(ViPipe, 0x77, 0x74);
	gc0312_write_register(ViPipe, 0x78, 0x40);
	gc0312_write_register(ViPipe, 0x79, 0x5f);

	/////////////////////////////////////////////////
	///////////////////   DNDD  /////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x82, 0x14);
	gc0312_write_register(ViPipe, 0x83, 0x0b);
	gc0312_write_register(ViPipe, 0x89, 0xf0);

	/////////////////////////////////////////////////
	//////////////////   EEINTP  ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x8f, 0xaa);
	gc0312_write_register(ViPipe, 0x90, 0x8c);
	gc0312_write_register(ViPipe, 0x91, 0x90);
	gc0312_write_register(ViPipe, 0x92, 0x03);
	gc0312_write_register(ViPipe, 0x93, 0x03);
	gc0312_write_register(ViPipe, 0x94, 0x05);
	gc0312_write_register(ViPipe, 0x95, 0x65);
	gc0312_write_register(ViPipe, 0x96, 0xf0);

	/////////////////////////////////////////////////
	/////////////////////  ASDE  ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x00);

	gc0312_write_register(ViPipe, 0x9a, 0x20);
	gc0312_write_register(ViPipe, 0x9b, 0x80);
	gc0312_write_register(ViPipe, 0x9c, 0x40);
	gc0312_write_register(ViPipe, 0x9d, 0x80);

	gc0312_write_register(ViPipe, 0xa1, 0x30);
 	gc0312_write_register(ViPipe, 0xa2, 0x32);
	gc0312_write_register(ViPipe, 0xa4, 0x80);//30 travis 20140929
	gc0312_write_register(ViPipe, 0xa5, 0x28);//30 travis 20140929
	gc0312_write_register(ViPipe, 0xaa, 0x30);//10 travis 20140929
	gc0312_write_register(ViPipe, 0xac, 0x22);

	/////////////////////////////////////////////////
	///////////////////   GAMMA   ///////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x00);//default
	gc0312_write_register(ViPipe, 0xbf, 0x08);
	gc0312_write_register(ViPipe, 0xc0, 0x16);
	gc0312_write_register(ViPipe, 0xc1, 0x28);
	gc0312_write_register(ViPipe, 0xc2, 0x41);
	gc0312_write_register(ViPipe, 0xc3, 0x5a);
	gc0312_write_register(ViPipe, 0xc4, 0x6c);
	gc0312_write_register(ViPipe, 0xc5, 0x7a);
	gc0312_write_register(ViPipe, 0xc6, 0x96);
	gc0312_write_register(ViPipe, 0xc7, 0xac);
	gc0312_write_register(ViPipe, 0xc8, 0xbc);
	gc0312_write_register(ViPipe, 0xc9, 0xc9);
	gc0312_write_register(ViPipe, 0xca, 0xd3);
	gc0312_write_register(ViPipe, 0xcb, 0xdd);
	gc0312_write_register(ViPipe, 0xcc, 0xe5);
	gc0312_write_register(ViPipe, 0xcd, 0xf1);
	gc0312_write_register(ViPipe, 0xce, 0xfa);
	gc0312_write_register(ViPipe, 0xcf, 0xff);
	/////////////////////////////////////////////////
	///////////////////   YCP  //////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xd0, 0x40);
	gc0312_write_register(ViPipe, 0xd1, 0x34);
	gc0312_write_register(ViPipe, 0xd2, 0x34);
	gc0312_write_register(ViPipe, 0xd3, 0x40);
	gc0312_write_register(ViPipe, 0xd6, 0xf2);
	gc0312_write_register(ViPipe, 0xd7, 0x1b);
	gc0312_write_register(ViPipe, 0xd8, 0x18);
	gc0312_write_register(ViPipe, 0xdd, 0x03);

	/////////////////////////////////////////////////
	////////////////////   AEC   ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x01);
	gc0312_write_register(ViPipe, 0x05, 0x30);
	gc0312_write_register(ViPipe, 0x06, 0x75);
	gc0312_write_register(ViPipe, 0x07, 0x40);
	gc0312_write_register(ViPipe, 0x08, 0xb0);
	gc0312_write_register(ViPipe, 0x0a, 0xc5);
	gc0312_write_register(ViPipe, 0x0b, 0x11);
	gc0312_write_register(ViPipe, 0x0c, 0x00);
	gc0312_write_register(ViPipe, 0x12, 0x52);
	gc0312_write_register(ViPipe, 0x13, 0x38);
	gc0312_write_register(ViPipe, 0x18, 0x95);
	gc0312_write_register(ViPipe, 0x19, 0x96);
	gc0312_write_register(ViPipe, 0x1f, 0x20);
	gc0312_write_register(ViPipe, 0x20, 0xc0);
	gc0312_write_register(ViPipe, 0x3e, 0x40);
	gc0312_write_register(ViPipe, 0x3f, 0x57);
	gc0312_write_register(ViPipe, 0x40, 0x7d);
	gc0312_write_register(ViPipe, 0x03, 0x60);
	gc0312_write_register(ViPipe, 0x44, 0x02);

	/////////////////////////////////////////////////
	////////////////////   AWB   ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x01);
	gc0312_write_register(ViPipe, 0x1c, 0x91);
	gc0312_write_register(ViPipe, 0x21, 0x15);
	gc0312_write_register(ViPipe, 0x50, 0x80);
	gc0312_write_register(ViPipe, 0x56, 0x04);
	gc0312_write_register(ViPipe, 0x59, 0x08);
	gc0312_write_register(ViPipe, 0x5b, 0x02);
	gc0312_write_register(ViPipe, 0x61, 0x8d);
	gc0312_write_register(ViPipe, 0x62, 0xa7);
	gc0312_write_register(ViPipe, 0x63, 0xd0);
	gc0312_write_register(ViPipe, 0x65, 0x06);
	gc0312_write_register(ViPipe, 0x66, 0x06);
	gc0312_write_register(ViPipe, 0x67, 0x84);
	gc0312_write_register(ViPipe, 0x69, 0x08);
	gc0312_write_register(ViPipe, 0x6a, 0x25);
	gc0312_write_register(ViPipe, 0x6b, 0x01);
	gc0312_write_register(ViPipe, 0x6c, 0x00);
	gc0312_write_register(ViPipe, 0x6d, 0x02);
	gc0312_write_register(ViPipe, 0x6e, 0xf0);
	gc0312_write_register(ViPipe, 0x6f, 0x80);
	gc0312_write_register(ViPipe, 0x76, 0x80);
	gc0312_write_register(ViPipe, 0x78, 0xaf);
	gc0312_write_register(ViPipe, 0x79, 0x75);
	gc0312_write_register(ViPipe, 0x7a, 0x40);
	gc0312_write_register(ViPipe, 0x7b, 0x50);
	gc0312_write_register(ViPipe, 0x7c, 0x0c);


	gc0312_write_register(ViPipe, 0x90, 0xc9);//stable AWB
	gc0312_write_register(ViPipe, 0x91, 0xbe);
	gc0312_write_register(ViPipe, 0x92, 0xe2);
	gc0312_write_register(ViPipe, 0x93, 0xc9);
	gc0312_write_register(ViPipe, 0x95, 0x1b);
	gc0312_write_register(ViPipe, 0x96, 0xe2);
	gc0312_write_register(ViPipe, 0x97, 0x49);
	gc0312_write_register(ViPipe, 0x98, 0x1b);
	gc0312_write_register(ViPipe, 0x9a, 0x49);
	gc0312_write_register(ViPipe, 0x9b, 0x1b);
	gc0312_write_register(ViPipe, 0x9c, 0xc3);
	gc0312_write_register(ViPipe, 0x9d, 0x49);
	gc0312_write_register(ViPipe, 0x9f, 0xc7);
	gc0312_write_register(ViPipe, 0xa0, 0xc8);
	gc0312_write_register(ViPipe, 0xa1, 0x00);
	gc0312_write_register(ViPipe, 0xa2, 0x00);
	gc0312_write_register(ViPipe, 0x86, 0x00);
	gc0312_write_register(ViPipe, 0x87, 0x00);
	gc0312_write_register(ViPipe, 0x88, 0x00);
	gc0312_write_register(ViPipe, 0x89, 0x00);
	gc0312_write_register(ViPipe, 0xa4, 0xb9);
	gc0312_write_register(ViPipe, 0xa5, 0xa0);
	gc0312_write_register(ViPipe, 0xa6, 0xba);
	gc0312_write_register(ViPipe, 0xa7, 0x92);
	gc0312_write_register(ViPipe, 0xa9, 0xba);
	gc0312_write_register(ViPipe, 0xaa, 0x80);
	gc0312_write_register(ViPipe, 0xab, 0x9d);
	gc0312_write_register(ViPipe, 0xac, 0x7f);
	gc0312_write_register(ViPipe, 0xae, 0xbb);
	gc0312_write_register(ViPipe, 0xaf, 0x9d);
	gc0312_write_register(ViPipe, 0xb0, 0xc8);
	gc0312_write_register(ViPipe, 0xb1, 0x97);
	gc0312_write_register(ViPipe, 0xb3, 0xb7);
	gc0312_write_register(ViPipe, 0xb4, 0x7f);
	gc0312_write_register(ViPipe, 0xb5, 0x00);
	gc0312_write_register(ViPipe, 0xb6, 0x00);
	gc0312_write_register(ViPipe, 0x8b, 0x00);
	gc0312_write_register(ViPipe, 0x8c, 0x00);
	gc0312_write_register(ViPipe, 0x8d, 0x00);
	gc0312_write_register(ViPipe, 0x8e, 0x00);
	gc0312_write_register(ViPipe, 0x94, 0x55);
	gc0312_write_register(ViPipe, 0x99, 0xa6);
	gc0312_write_register(ViPipe, 0x9e, 0xaa);
	gc0312_write_register(ViPipe, 0xa3, 0x0a);
	gc0312_write_register(ViPipe, 0x8a, 0x00);
	gc0312_write_register(ViPipe, 0xa8, 0x55);
	gc0312_write_register(ViPipe, 0xad, 0x55);
	gc0312_write_register(ViPipe, 0xb2, 0x55);
	gc0312_write_register(ViPipe, 0xb7, 0x05);
	gc0312_write_register(ViPipe, 0x8f, 0x00);
	gc0312_write_register(ViPipe, 0xb8, 0xcb);
	gc0312_write_register(ViPipe, 0xb9, 0x9b);
	/////////////////////////////////////////////////
	////////////////////   CC    ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x01);

	gc0312_write_register(ViPipe, 0xd0, 0x38);//skin red
	gc0312_write_register(ViPipe, 0xd1, 0x00);
	gc0312_write_register(ViPipe, 0xd2, 0x02);
	gc0312_write_register(ViPipe, 0xd3, 0x04);
	gc0312_write_register(ViPipe, 0xd4, 0x38);
	gc0312_write_register(ViPipe, 0xd5, 0x12);
	gc0312_write_register(ViPipe, 0xd6, 0x30);
	gc0312_write_register(ViPipe, 0xd7, 0x00);
	gc0312_write_register(ViPipe, 0xd8, 0x0a);
	gc0312_write_register(ViPipe, 0xd9, 0x16);
	gc0312_write_register(ViPipe, 0xda, 0x39);
	gc0312_write_register(ViPipe, 0xdb, 0xf8);

	/////////////////////////////////////////////////
	////////////////////   LSC   ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x01);
	gc0312_write_register(ViPipe, 0xc1, 0x3c);
	gc0312_write_register(ViPipe, 0xc2, 0x50);
	gc0312_write_register(ViPipe, 0xc3, 0x00);
	gc0312_write_register(ViPipe, 0xc4, 0x40);
	gc0312_write_register(ViPipe, 0xc5, 0x30);
	gc0312_write_register(ViPipe, 0xc6, 0x30);
	gc0312_write_register(ViPipe, 0xc7, 0x10);
	gc0312_write_register(ViPipe, 0xc8, 0x00);
	gc0312_write_register(ViPipe, 0xc9, 0x00);
	gc0312_write_register(ViPipe, 0xdc, 0x20);
	gc0312_write_register(ViPipe, 0xdd, 0x10);
	gc0312_write_register(ViPipe, 0xdf, 0x00);
	gc0312_write_register(ViPipe, 0xde, 0x00);

	/////////////////////////////////////////////////
	///////////////////  Histogram	/////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x01, 0x10);
	gc0312_write_register(ViPipe, 0x0b, 0x31);
	gc0312_write_register(ViPipe, 0x0e, 0x50);
	gc0312_write_register(ViPipe, 0x0f, 0x0f);
	gc0312_write_register(ViPipe, 0x10, 0x6e);
	gc0312_write_register(ViPipe, 0x12, 0xa0);
	gc0312_write_register(ViPipe, 0x15, 0x60);
	gc0312_write_register(ViPipe, 0x16, 0x60);
	gc0312_write_register(ViPipe, 0x17, 0xe0);

	/////////////////////////////////////////////////
	//////////////	Measure Window	  ///////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xcc, 0x0c);
	gc0312_write_register(ViPipe, 0xcd, 0x10);
	gc0312_write_register(ViPipe, 0xce, 0xa0);
	gc0312_write_register(ViPipe, 0xcf, 0xe6);

	/////////////////////////////////////////////////
	/////////////////	dark sun   //////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0x45, 0xf7);
	gc0312_write_register(ViPipe, 0x46, 0xff);
	gc0312_write_register(ViPipe, 0x47, 0x15);
	gc0312_write_register(ViPipe, 0x48, 0x03);
	gc0312_write_register(ViPipe, 0x4f, 0x60);

	//////////////////banding//////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x00);
	gc0312_write_register(ViPipe, 0x05, 0x02);
	gc0312_write_register(ViPipe, 0x06, 0xd1); //HB
	gc0312_write_register(ViPipe, 0x07, 0x00);
	gc0312_write_register(ViPipe, 0x08, 0x22); //VB

	gc0312_write_register(ViPipe, 0xfe, 0x01);
	gc0312_write_register(ViPipe, 0x25, 0x00);   //anti-flicker step [11:8]
	gc0312_write_register(ViPipe, 0x26, 0x6a);   //anti-flicker step [7:0]

	gc0312_write_register(ViPipe, 0x27, 0x02);   //exp level 0  20fps
	gc0312_write_register(ViPipe, 0x28, 0x12);
	gc0312_write_register(ViPipe, 0x29, 0x03);   //exp level 1  12.50fps
	gc0312_write_register(ViPipe, 0x2a, 0x50);
	gc0312_write_register(ViPipe, 0x2b, 0x05);   //7.14fps
	gc0312_write_register(ViPipe, 0x2c, 0xcc);
	gc0312_write_register(ViPipe, 0x2d, 0x07);   //exp level 3  5.55fps
	gc0312_write_register(ViPipe, 0x2e, 0x74);
	gc0312_write_register(ViPipe, 0x3c, 0x20);
	gc0312_write_register(ViPipe, 0xfe, 0x00);

	/////////////////////////////////////////////////
	/////////////////////  DVP   ////////////////////
	/////////////////////////////////////////////////
	gc0312_write_register(ViPipe, 0xfe, 0x03);
	gc0312_write_register(ViPipe, 0x01, 0x00);
	gc0312_write_register(ViPipe, 0x02, 0x00);
	gc0312_write_register(ViPipe, 0x10, 0x00);
	gc0312_write_register(ViPipe, 0x15, 0x00);
	gc0312_write_register(ViPipe, 0xfe, 0x00);
	///////////////////OUTPUT//////////////////////
	gc0312_write_register(ViPipe, 0xf3, 0xff);// output_enable


	 delay_ms(50);

	printf("ViPipe:%d,===GC0312 480P 20fps YUV Init OK!===\n", ViPipe);
}

