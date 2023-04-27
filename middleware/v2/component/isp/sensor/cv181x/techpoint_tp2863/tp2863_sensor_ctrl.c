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
#include "tp2863_cmos_ex.h"

const CVI_U8 tp2863_i2c_addr = 0x44; /* I2C slave address of TP2863, SA0=0:0x44, SA0=1:0x45*/
const CVI_U32 tp2863_addr_byte = 1;
const CVI_U32 tp2863_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ...(VI_MAX_PIPE_NUM - 1)] = -1};

#define TP2863_BLUE_SCREEN 0

enum
{
	CH_1 = 0,
	CH_2 = 1,
	CH_ALL = 4,
	MIPI_PAGE = 8,
};

enum
{
	STD_TVI, // TVI
	STD_HDA, // AHD
};

enum
{
	HD25,
	HD30, // 1280x720
	FHD25,
	FHD30, // 1920x1080
};

enum
{
	MIPI_1CH2LANE_297M,
	MIPI_1CH2LANE_594M,
	MIPI_2CH2LANE_594M,
	MIPI_2CH2LANE_297M,
};

int tp2863_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;
	int ret;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;

	u8DevNum = g_aunTP2863_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile), "/dev/i2c-%u", u8DevNum);
	syslog(LOG_DEBUG, "open %s\n", acDevFile);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/cvi_i2c_drv-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, tp2863_i2c_addr);
	if (ret < 0)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int tp2863_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0)
	{
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int tp2863_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return 0;

	if (tp2863_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, tp2863_addr_byte);
	if (ret < 0)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, tp2863_data_byte);
	if (ret < 0)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (tp2863_data_byte == 2)
	{
		data = buf[0] << 8;
		data += buf[1];
	}
	else
	{
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);
	return data;
}

int tp2863_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (tp2863_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	if (tp2863_data_byte == 2)
		buf[idx++] = (data >> 8) & 0xff;

	// add data byte 0
	buf[idx++] = data & 0xff;

	ret = write(g_fd[ViPipe], buf, tp2863_addr_byte + tp2863_data_byte);
	if (ret < 0)
	{
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_WRITE error!\n");
		return CVI_FAILURE;
	}
	syslog(LOG_DEBUG, "i2c w 0x%x 0x%x\n", addr, data);

#if 0 // read back checing
	ret = tp2863_read_register(ViPipe, addr);
	if (ret != data)
		syslog(LOG_DEBUG, "i2c readback-check fail, 0x%x != 0x%x\n", ret, data);
#endif
	return CVI_SUCCESS;
}

void TP2863_decoder_init(VI_PIPE ViPipe, unsigned char ch, unsigned char fmt, unsigned char std)
{
	unsigned char tmp;
	const unsigned char REG42_43[] = {0x01, 0x02, 0x00, 0x00, 0x03};
	const unsigned char MASK42_43[] = {0xfe, 0xfd, 0xff, 0xff, 0xfc};

	tp2863_write_register(ViPipe, 0x40, ch);
	tp2863_write_register(ViPipe, 0x06, 0x12); // default value
	tp2863_write_register(ViPipe, 0x50, 0x00); // VIN1/3
	tp2863_write_register(ViPipe, 0x51, 0x00);
	tp2863_write_register(ViPipe, 0x54, 0x03);

	if (HD25 == fmt)
	{
		tmp = tp2863_read_register(ViPipe, 0x42);
		tmp |= REG42_43[ch];
		tp2863_write_register(ViPipe, 0x42, tmp);

		tmp = tp2863_read_register(ViPipe, 0x43);
		tmp &= MASK42_43[ch];
		tp2863_write_register(ViPipe, 0x43, tmp);

		tp2863_write_register(ViPipe, 0x02, 0x42);
		tp2863_write_register(ViPipe, 0x07, 0xc0);
		tp2863_write_register(ViPipe, 0x0b, 0xc0);
		tp2863_write_register(ViPipe, 0x0c, 0x13);
		tp2863_write_register(ViPipe, 0x0d, 0x50);

		tp2863_write_register(ViPipe, 0x15, 0x13);
		tp2863_write_register(ViPipe, 0x16, 0x15);
		tp2863_write_register(ViPipe, 0x17, 0x00);
		tp2863_write_register(ViPipe, 0x18, 0x19);
		tp2863_write_register(ViPipe, 0x19, 0xd0);
		tp2863_write_register(ViPipe, 0x1a, 0x25);
		tp2863_write_register(ViPipe, 0x1c, 0x07); // 1280*720, 25fps
		tp2863_write_register(ViPipe, 0x1d, 0xbc); // 1280*720, 25fps

		tp2863_write_register(ViPipe, 0x20, 0x30);
		tp2863_write_register(ViPipe, 0x21, 0x84);
		tp2863_write_register(ViPipe, 0x22, 0x36);
		tp2863_write_register(ViPipe, 0x23, 0x3c);

		tp2863_write_register(ViPipe, 0x2b, 0x60);
		tp2863_write_register(ViPipe, 0x2c, 0x0a);
		tp2863_write_register(ViPipe, 0x2d, 0x30);
		tp2863_write_register(ViPipe, 0x2e, 0x70);

		tp2863_write_register(ViPipe, 0x30, 0x48);
		tp2863_write_register(ViPipe, 0x31, 0xbb);
		tp2863_write_register(ViPipe, 0x32, 0x2e);
		tp2863_write_register(ViPipe, 0x33, 0x90);

		tp2863_write_register(ViPipe, 0x35, 0x25);
		tp2863_write_register(ViPipe, 0x39, 0x08);

		if (STD_HDA == std)
		{
			tp2863_write_register(ViPipe, 0x02, 0x46);

			tp2863_write_register(ViPipe, 0x0d, 0x71);

			tp2863_write_register(ViPipe, 0x20, 0x40);
			tp2863_write_register(ViPipe, 0x21, 0x46);

			tp2863_write_register(ViPipe, 0x25, 0xfe);
			tp2863_write_register(ViPipe, 0x26, 0x01);

			tp2863_write_register(ViPipe, 0x2c, 0x3a);
			tp2863_write_register(ViPipe, 0x2d, 0x5a);
			tp2863_write_register(ViPipe, 0x2e, 0x40);

			tp2863_write_register(ViPipe, 0x30, 0x9e);
			tp2863_write_register(ViPipe, 0x31, 0x20);
			tp2863_write_register(ViPipe, 0x32, 0x10);
			tp2863_write_register(ViPipe, 0x33, 0x90);
		}
	}
	else if (HD30 == fmt)
	{
		tmp = tp2863_read_register(ViPipe, 0x42);
		tmp |= REG42_43[ch];
		tp2863_write_register(ViPipe, 0x42, tmp);

		tmp = tp2863_read_register(ViPipe, 0x43);
		tmp &= MASK42_43[ch];
		tp2863_write_register(ViPipe, 0x43, tmp);

		tp2863_write_register(ViPipe, 0x02, 0x42);
		tp2863_write_register(ViPipe, 0x07, 0xc0);
		tp2863_write_register(ViPipe, 0x0b, 0xc0);
		tp2863_write_register(ViPipe, 0x0c, 0x13);
		tp2863_write_register(ViPipe, 0x0d, 0x50);

		tp2863_write_register(ViPipe, 0x15, 0x13);
		tp2863_write_register(ViPipe, 0x16, 0x15);
		tp2863_write_register(ViPipe, 0x17, 0x00);
		tp2863_write_register(ViPipe, 0x18, 0x19);
		tp2863_write_register(ViPipe, 0x19, 0xd0);
		tp2863_write_register(ViPipe, 0x1a, 0x25);
		tp2863_write_register(ViPipe, 0x1c, 0x06); // 1280*720, 30fps
		tp2863_write_register(ViPipe, 0x1d, 0x72); // 1280*720, 30fps

		tp2863_write_register(ViPipe, 0x20, 0x30);
		tp2863_write_register(ViPipe, 0x21, 0x84);
		tp2863_write_register(ViPipe, 0x22, 0x36);
		tp2863_write_register(ViPipe, 0x23, 0x3c);

		tp2863_write_register(ViPipe, 0x2b, 0x60);
		tp2863_write_register(ViPipe, 0x2c, 0x0a);
		tp2863_write_register(ViPipe, 0x2d, 0x30);
		tp2863_write_register(ViPipe, 0x2e, 0x70);

		tp2863_write_register(ViPipe, 0x30, 0x48);
		tp2863_write_register(ViPipe, 0x31, 0xbb);
		tp2863_write_register(ViPipe, 0x32, 0x2e);
		tp2863_write_register(ViPipe, 0x33, 0x90);

		tp2863_write_register(ViPipe, 0x35, 0x25);
		tp2863_write_register(ViPipe, 0x39, 0x08);

		if (STD_HDA == std)
		{
			tp2863_write_register(ViPipe, 0x02, 0x46);

			tp2863_write_register(ViPipe, 0x0d, 0x70);

			tp2863_write_register(ViPipe, 0x20, 0x40);
			tp2863_write_register(ViPipe, 0x21, 0x46);

			tp2863_write_register(ViPipe, 0x25, 0xfe);
			tp2863_write_register(ViPipe, 0x26, 0x01);

			tp2863_write_register(ViPipe, 0x2c, 0x3a);
			tp2863_write_register(ViPipe, 0x2d, 0x5a);
			tp2863_write_register(ViPipe, 0x2e, 0x40);

			tp2863_write_register(ViPipe, 0x30, 0x9d);
			tp2863_write_register(ViPipe, 0x31, 0xca);
			tp2863_write_register(ViPipe, 0x32, 0x01);
			tp2863_write_register(ViPipe, 0x33, 0xd0);
		}
	}
	else if (FHD25 == fmt)
	{
		tmp = tp2863_read_register(ViPipe, 0x42);
		tmp &= MASK42_43[ch];
		tp2863_write_register(ViPipe, 0x42, tmp);

		tmp = tp2863_read_register(ViPipe, 0x43);
		tmp &= MASK42_43[ch];
		tp2863_write_register(ViPipe, 0x43, tmp);

		tp2863_write_register(ViPipe, 0x02, 0x40);
		tp2863_write_register(ViPipe, 0x07, 0xc0);
		tp2863_write_register(ViPipe, 0x0b, 0xc0);
		tp2863_write_register(ViPipe, 0x0c, 0x03);
		tp2863_write_register(ViPipe, 0x0d, 0x50);

		tp2863_write_register(ViPipe, 0x15, 0x03);
		tp2863_write_register(ViPipe, 0x16, 0xd2);
		tp2863_write_register(ViPipe, 0x17, 0x80);
		tp2863_write_register(ViPipe, 0x18, 0x29);
		tp2863_write_register(ViPipe, 0x19, 0x38);
		tp2863_write_register(ViPipe, 0x1a, 0x47);

		tp2863_write_register(ViPipe, 0x1c, 0x0a); // 1920*1080, 25fps
		tp2863_write_register(ViPipe, 0x1d, 0x50); //

		tp2863_write_register(ViPipe, 0x20, 0x30);
		tp2863_write_register(ViPipe, 0x21, 0x84);
		tp2863_write_register(ViPipe, 0x22, 0x36);
		tp2863_write_register(ViPipe, 0x23, 0x3c);

		tp2863_write_register(ViPipe, 0x2b, 0x60);
		tp2863_write_register(ViPipe, 0x2c, 0x0a);
		tp2863_write_register(ViPipe, 0x2d, 0x30);
		tp2863_write_register(ViPipe, 0x2e, 0x70);

		tp2863_write_register(ViPipe, 0x30, 0x48);
		tp2863_write_register(ViPipe, 0x31, 0xbb);
		tp2863_write_register(ViPipe, 0x32, 0x2e);
		tp2863_write_register(ViPipe, 0x33, 0x90);

		tp2863_write_register(ViPipe, 0x35, 0x05);
		tp2863_write_register(ViPipe, 0x39, 0x0C);

		if (STD_HDA == std)
		{
			tp2863_write_register(ViPipe, 0x02, 0x44);

			tp2863_write_register(ViPipe, 0x0d, 0x73);

			tp2863_write_register(ViPipe, 0x15, 0x01);
			tp2863_write_register(ViPipe, 0x16, 0xf0);
			tp2863_write_register(ViPipe, 0x18, 0x2a);
			tp2863_write_register(ViPipe, 0x20, 0x3c);
			tp2863_write_register(ViPipe, 0x21, 0x46);

			tp2863_write_register(ViPipe, 0x25, 0xfe);
			tp2863_write_register(ViPipe, 0x26, 0x0d);

			tp2863_write_register(ViPipe, 0x2c, 0x3a);
			tp2863_write_register(ViPipe, 0x2d, 0x54);
			tp2863_write_register(ViPipe, 0x2e, 0x40);

			tp2863_write_register(ViPipe, 0x30, 0xa5);
			tp2863_write_register(ViPipe, 0x31, 0x86);
			tp2863_write_register(ViPipe, 0x32, 0xfb);
			tp2863_write_register(ViPipe, 0x33, 0x60);
		}
	}
	else if (FHD30 == fmt)
	{
		tmp = tp2863_read_register(ViPipe, 0x42);
		tmp &= MASK42_43[ch];
		tp2863_write_register(ViPipe, 0x42, tmp);

		tmp = tp2863_read_register(ViPipe, 0x43);
		tmp &= MASK42_43[ch];
		tp2863_write_register(ViPipe, 0x43, tmp);

		tp2863_write_register(ViPipe, 0x02, 0x40);
		tp2863_write_register(ViPipe, 0x07, 0xc0);
		tp2863_write_register(ViPipe, 0x0b, 0xc0);
		tp2863_write_register(ViPipe, 0x0c, 0x03);
		tp2863_write_register(ViPipe, 0x0d, 0x50);

		tp2863_write_register(ViPipe, 0x15, 0x03);
		tp2863_write_register(ViPipe, 0x16, 0xd2);
		tp2863_write_register(ViPipe, 0x17, 0x80);
		tp2863_write_register(ViPipe, 0x18, 0x29);
		tp2863_write_register(ViPipe, 0x19, 0x38);
		tp2863_write_register(ViPipe, 0x1a, 0x47);
		tp2863_write_register(ViPipe, 0x1c, 0x08); // 1920*1080, 30fps
		tp2863_write_register(ViPipe, 0x1d, 0x98); //

		tp2863_write_register(ViPipe, 0x20, 0x30);
		tp2863_write_register(ViPipe, 0x21, 0x84);
		tp2863_write_register(ViPipe, 0x22, 0x36);
		tp2863_write_register(ViPipe, 0x23, 0x3c);

		tp2863_write_register(ViPipe, 0x2b, 0x60);
		tp2863_write_register(ViPipe, 0x2c, 0x0a);
		tp2863_write_register(ViPipe, 0x2d, 0x30);
		tp2863_write_register(ViPipe, 0x2e, 0x70);

		tp2863_write_register(ViPipe, 0x30, 0x48);
		tp2863_write_register(ViPipe, 0x31, 0xbb);
		tp2863_write_register(ViPipe, 0x32, 0x2e);
		tp2863_write_register(ViPipe, 0x33, 0x90);

		tp2863_write_register(ViPipe, 0x35, 0x05);
		tp2863_write_register(ViPipe, 0x39, 0x0C);

		if (STD_HDA == std)
		{
			tp2863_write_register(ViPipe, 0x02, 0x44);

			tp2863_write_register(ViPipe, 0x0d, 0x72);

			tp2863_write_register(ViPipe, 0x15, 0x01);
			tp2863_write_register(ViPipe, 0x16, 0xf0);
			tp2863_write_register(ViPipe, 0x18, 0x2a);

			tp2863_write_register(ViPipe, 0x20, 0x38);
			tp2863_write_register(ViPipe, 0x21, 0x46);

			tp2863_write_register(ViPipe, 0x25, 0xfe);
			tp2863_write_register(ViPipe, 0x26, 0x0d);

			tp2863_write_register(ViPipe, 0x2c, 0x3a);
			tp2863_write_register(ViPipe, 0x2d, 0x54);
			tp2863_write_register(ViPipe, 0x2e, 0x40);

			tp2863_write_register(ViPipe, 0x30, 0xa5);
			tp2863_write_register(ViPipe, 0x31, 0x95);
			tp2863_write_register(ViPipe, 0x32, 0xe0);
			tp2863_write_register(ViPipe, 0x33, 0x60);
		}
	}
}

void TP2863_mipi_out(VI_PIPE ViPipe, unsigned char output)
{
	// mipi setting
	tp2863_write_register(ViPipe, 0x40, MIPI_PAGE); // MIPI page
	tp2863_write_register(ViPipe, 0x02, 0x78);
	tp2863_write_register(ViPipe, 0x03, 0x70);
	tp2863_write_register(ViPipe, 0x04, 0x70);
	tp2863_write_register(ViPipe, 0x05, 0x70);
	tp2863_write_register(ViPipe, 0x06, 0x70);

	tp2863_write_register(ViPipe, 0x13, 0xef);
	tp2863_write_register(ViPipe, 0x20, 0x00);
	tp2863_write_register(ViPipe, 0x21, 0x22);
	tp2863_write_register(ViPipe, 0x22, 0x20);
	tp2863_write_register(ViPipe, 0x23, 0x9e);

	if (MIPI_1CH2LANE_297M == output)
	{
		tp2863_write_register(ViPipe, 0x21, 0x12);
		tp2863_write_register(ViPipe, 0x14, 0x41);
		tp2863_write_register(ViPipe, 0x15, 0x02);
		tp2863_write_register(ViPipe, 0x2a, 0x04);
		tp2863_write_register(ViPipe, 0x2b, 0x03);
		tp2863_write_register(ViPipe, 0x2c, 0x09);
		tp2863_write_register(ViPipe, 0x2e, 0x02);
		tp2863_write_register(ViPipe, 0x10, 0xa0);
		tp2863_write_register(ViPipe, 0x10, 0x20);
	}
	else if (MIPI_1CH2LANE_594M == output)
	{
		tp2863_write_register(ViPipe, 0x21, 0x12);
		tp2863_write_register(ViPipe, 0x14, 0x00); // clk
		tp2863_write_register(ViPipe, 0x15, 0x01); // clk
		tp2863_write_register(ViPipe, 0x2a, 0x08); // LPX
		tp2863_write_register(ViPipe, 0x2b, 0x08); // PREP
		tp2863_write_register(ViPipe, 0x2c, 0x10); // TRAIL/HS ZERO
		tp2863_write_register(ViPipe, 0x2e, 0x0a);
		tp2863_write_register(ViPipe, 0x10, 0xa0);
		tp2863_write_register(ViPipe, 0x10, 0x20);
	}
	else if (MIPI_2CH2LANE_297M == output)
	{
		tp2863_write_register(ViPipe, 0x21, 0x22);
		tp2863_write_register(ViPipe, 0x14, 0x41);
		tp2863_write_register(ViPipe, 0x15, 0x02);
		tp2863_write_register(ViPipe, 0x2a, 0x04);
		tp2863_write_register(ViPipe, 0x2b, 0x03);
		tp2863_write_register(ViPipe, 0x2c, 0x09);
		tp2863_write_register(ViPipe, 0x2e, 0x02);
		tp2863_write_register(ViPipe, 0x10, 0xa0);
		tp2863_write_register(ViPipe, 0x10, 0x20);
	}
	else if (MIPI_2CH2LANE_594M == output)
	{
		tp2863_write_register(ViPipe, 0x21, 0x22);
		tp2863_write_register(ViPipe, 0x14, 0x00);
		tp2863_write_register(ViPipe, 0x15, 0x01);
		tp2863_write_register(ViPipe, 0x2a, 0x08);
		tp2863_write_register(ViPipe, 0x2b, 0x08);
		tp2863_write_register(ViPipe, 0x2c, 0x10);
		tp2863_write_register(ViPipe, 0x2e, 0x0a);
		tp2863_write_register(ViPipe, 0x10, 0xa0);
		tp2863_write_register(ViPipe, 0x10, 0x20);
	}

	/* Enable MIPI CSI2 output */
	tp2863_write_register(ViPipe, 0x28, 0x02);
	tp2863_write_register(ViPipe, 0x28, 0x00);
}

void TP2863_reg_init(VI_PIPE ViPipe, CVI_U8 u8ImgMode)
{
	if (u8ImgMode == TP2863_MODE_1080P_30P)
	{
		TP2863_decoder_init(ViPipe, CH_1, FHD30, STD_HDA);
		TP2863_mipi_out(ViPipe, MIPI_1CH2LANE_297M);
		printf("Techpoint TP2863 1080 FHD30\n");
	}
	else if (u8ImgMode == TP2863_MODE_1080P_25P)
	{
		TP2863_decoder_init(ViPipe, CH_1, FHD25, STD_HDA);
		TP2863_mipi_out(ViPipe, MIPI_1CH2LANE_297M);
		printf("Techpoint TP2863 720 FHD25\n");
	}
	else if (u8ImgMode == TP2863_MODE_720P_30P)
	{
		TP2863_decoder_init(ViPipe, CH_1, HD30, STD_HDA);
		TP2863_mipi_out(ViPipe, MIPI_1CH2LANE_297M);
		printf("Techpoint TP2863 720 HD30\n");
	}
	else if (u8ImgMode == TP2863_MODE_720P_25P)
	{
		TP2863_decoder_init(ViPipe, CH_1, HD25, STD_HDA);
		TP2863_mipi_out(ViPipe, MIPI_1CH2LANE_297M);
		printf("Techpoint TP2863 720 HD25\n");
	}
}

void tp2863_init(VI_PIPE ViPipe)
{
	tp2863_i2c_init(ViPipe);

	syslog(LOG_DEBUG, "Loading Techpoint TP2863 sensor\n");

	// check sensor chip id
	tp2863_write_register(ViPipe, 0x40, 0x0);
	if (tp2863_read_register(ViPipe, 0xfe) != 0x28 ||
		tp2863_read_register(ViPipe, 0xff) != 0x63)
	{
		syslog(LOG_DEBUG, "read TP2863 chip id fail\n");
		return;
	}

	TP2863_reg_init(ViPipe, g_pastTP2863[ViPipe]->u8ImgMode);

	usleep(500 * 1000);

#if TP2863_BLUE_SCREEN
	tp2863_write_register(ViPipe, 0x40, 0x00);
	tp2863_write_register(ViPipe, 0x2A, 0x3C);
#endif
}

void tp2863_exit(VI_PIPE ViPipe)
{
	tp2863_i2c_exit(ViPipe);
}