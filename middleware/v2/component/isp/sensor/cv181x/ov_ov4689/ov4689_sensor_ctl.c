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
#include "ov4689_cmos_ex.h"

static void ov4689_linear_1520p30_init(VI_PIPE ViPipe);

CVI_U8 ov4689_i2c_addr = 0x36;        /* I2C Address of OV4689 */
const CVI_U32 ov4689_addr_byte = 2;
const CVI_U32 ov4689_data_byte = 1;
static int g_fd[VI_MAX_PIPE_NUM] = {[0 ... (VI_MAX_PIPE_NUM - 1)] = -1};

int ov4689_i2c_init(VI_PIPE ViPipe)
{
	char acDevFile[16] = {0};
	CVI_U8 u8DevNum;

	if (g_fd[ViPipe] >= 0)
		return CVI_SUCCESS;
	int ret;

	u8DevNum = g_aunOv4689_BusInfo[ViPipe].s8I2cDev;
	snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

	g_fd[ViPipe] = open(acDevFile, O_RDWR, 0600);

	if (g_fd[ViPipe] < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Open /dev/i2c-%u error!\n", u8DevNum);
		return CVI_FAILURE;
	}

	ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, ov4689_i2c_addr);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return ret;
	}

	return CVI_SUCCESS;
}

int ov4689_i2c_exit(VI_PIPE ViPipe)
{
	if (g_fd[ViPipe] >= 0) {
		close(g_fd[ViPipe]);
		g_fd[ViPipe] = -1;
		return CVI_SUCCESS;
	}
	return CVI_FAILURE;
}

int ov4689_read_register(VI_PIPE ViPipe, int addr)
{
	int ret, data;
	CVI_U8 buf[8];
	CVI_U8 idx = 0;

	if (g_fd[ViPipe] < 0)
		return CVI_FAILURE;

	if (ov4689_addr_byte == 2)
		buf[idx++] = (addr >> 8) & 0xff;

	// add address byte 0
	buf[idx++] = addr & 0xff;

	ret = write(g_fd[ViPipe], buf, ov4689_addr_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	buf[0] = 0;
	buf[1] = 0;
	ret = read(g_fd[ViPipe], buf, ov4689_data_byte);
	if (ret < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "I2C_READ error!\n");
		return 0;
	}

	// pack read back data
	data = 0;
	if (ov4689_data_byte == 2) {
		data = buf[0] << 8;
		data += buf[1];
	} else {
		data = buf[0];
	}

	syslog(LOG_DEBUG, "i2c r 0x%x = 0x%x\n", addr, data);

	return data;
}

int ov4689_write_register(VI_PIPE ViPipe, int addr, int data)
{
	CVI_U8 idx = 0;
	int ret;
	CVI_U8 buf[8];

	if (g_fd[ViPipe] < 0)
		return CVI_SUCCESS;

	if (ov4689_addr_byte == 2) {
		buf[idx] = (addr >> 8) & 0xff;
		idx++;
		buf[idx] = addr & 0xff;
		idx++;
	}

	if (ov4689_data_byte == 1) {
		buf[idx] = data & 0xff;
		idx++;
	}

	ret = write(g_fd[ViPipe], buf, ov4689_addr_byte + ov4689_data_byte);
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

void ov4689_standby(VI_PIPE ViPipe)
{
	ov4689_write_register(ViPipe, 0x0100, 0x00); /* STANDBY */
}

void ov4689_restart(VI_PIPE ViPipe)
{
	ov4689_write_register(ViPipe, 0x0100, 0x01); /* standby */
}

void ov4689_default_reg_init(VI_PIPE ViPipe)
{
	CVI_U32 i;
	CVI_U32 start = 1;
	CVI_U32 end = g_pastOv4689[ViPipe]->astSyncInfo[0].snsCfg.u32RegNum - 3;

	for (i = start; i < end; i++) {
		ov4689_write_register(ViPipe,
				g_pastOv4689[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32RegAddr,
				g_pastOv4689[ViPipe]->astSyncInfo[0].snsCfg.astI2cData[i].u32Data);
	}
}

#define OV4689_FLIP	0x3820
#define OV4689_MIRROR	0x3821
void ov4689_mirror_flip(VI_PIPE ViPipe, ISP_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
	CVI_U8 flip, mirror;

	flip = ov4689_read_register(ViPipe, OV4689_FLIP);
	mirror = ov4689_read_register(ViPipe, OV4689_MIRROR);
	flip &= ~(0x3 << 1);
	mirror &= ~(0x3 << 1);

	switch (eSnsMirrorFlip) {
	case ISP_SNS_NORMAL:
		break;
	case ISP_SNS_MIRROR:
		mirror |= 0x3 << 1;
		break;
	case ISP_SNS_FLIP:
		flip |= 0x3 << 1;
		break;
	case ISP_SNS_MIRROR_FLIP:
		flip |= 0x3 << 1;
		mirror |= 0x3 << 1;
		break;
	default:
		return;
	}

	ov4689_write_register(ViPipe, OV4689_FLIP, flip);
	ov4689_write_register(ViPipe, OV4689_MIRROR, mirror);
}

#define OV4689_CHIP_ID_ADDR_H		0x300A
#define OV4689_CHIP_ID_ADDR_L		0x300B
#define OV4689_CHIP_ID			0x4688

int ov4689_probe(VI_PIPE ViPipe)
{
	int nVal, nVal2;

	usleep(1000);
	if (ov4689_i2c_init(ViPipe) != CVI_SUCCESS)
		return CVI_FAILURE;

	nVal  = ov4689_read_register(ViPipe, OV4689_CHIP_ID_ADDR_H);
	nVal2 = ov4689_read_register(ViPipe, OV4689_CHIP_ID_ADDR_L);
	if (nVal < 0 || nVal2 < 0) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "read sensor id error.\n");
		return nVal;
	}

	if ((((nVal & 0xFF) << 8) | (nVal2 & 0xFF)) != OV4689_CHIP_ID) {
		CVI_TRACE_SNS(CVI_DBG_ERR, "Sensor ID Mismatch! Use the wrong sensor??\n");
		return CVI_FAILURE;
	}
	return CVI_SUCCESS;
}

void ov4689_init(VI_PIPE ViPipe)
{
	ov4689_i2c_init(ViPipe);

	delay_ms(10);

	ov4689_linear_1520p30_init(ViPipe);

	g_pastOv4689[ViPipe]->bInit = CVI_TRUE;
}

void ov4689_exit(VI_PIPE ViPipe)
{
	ov4689_i2c_exit(ViPipe);
}

/* 1080P30 */
static void ov4689_linear_1520p30_init(VI_PIPE ViPipe)
{
	ov4689_write_register(ViPipe, 0x0103, 0x01);
	ov4689_write_register(ViPipe, 0x3638, 0x00);
	ov4689_write_register(ViPipe, 0x0300, 0x00);
	ov4689_write_register(ViPipe, 0x0302, 0x2a);
	ov4689_write_register(ViPipe, 0x0303, 0x00);
	ov4689_write_register(ViPipe, 0x0304, 0x03);
	ov4689_write_register(ViPipe, 0x030b, 0x00);
	ov4689_write_register(ViPipe, 0x030d, 0x1e);
	ov4689_write_register(ViPipe, 0x030e, 0x04);
	ov4689_write_register(ViPipe, 0x030f, 0x01);
	ov4689_write_register(ViPipe, 0x0312, 0x01);
	ov4689_write_register(ViPipe, 0x031e, 0x00);
	ov4689_write_register(ViPipe, 0x3000, 0x20);
	ov4689_write_register(ViPipe, 0x3002, 0x00);
	ov4689_write_register(ViPipe, 0x3018, 0x32);
	ov4689_write_register(ViPipe, 0x3020, 0x93);
	ov4689_write_register(ViPipe, 0x3021, 0x03);
	ov4689_write_register(ViPipe, 0x3022, 0x01);
	ov4689_write_register(ViPipe, 0x3031, 0x0a);
	ov4689_write_register(ViPipe, 0x303f, 0x0c);
	ov4689_write_register(ViPipe, 0x3305, 0xf1);
	ov4689_write_register(ViPipe, 0x3307, 0x04);
	ov4689_write_register(ViPipe, 0x3309, 0x29);
	ov4689_write_register(ViPipe, 0x3500, 0x00);
	ov4689_write_register(ViPipe, 0x3501, 0x60);
	ov4689_write_register(ViPipe, 0x3502, 0x00);
	ov4689_write_register(ViPipe, 0x3503, 0x04);
	ov4689_write_register(ViPipe, 0x3504, 0x00);
	ov4689_write_register(ViPipe, 0x3505, 0x00);
	ov4689_write_register(ViPipe, 0x3506, 0x00);
	ov4689_write_register(ViPipe, 0x3507, 0x00);
	ov4689_write_register(ViPipe, 0x3508, 0x00);
	ov4689_write_register(ViPipe, 0x3509, 0x80);
	ov4689_write_register(ViPipe, 0x350a, 0x00);
	ov4689_write_register(ViPipe, 0x350b, 0x00);
	ov4689_write_register(ViPipe, 0x350c, 0x00);
	ov4689_write_register(ViPipe, 0x350d, 0x00);
	ov4689_write_register(ViPipe, 0x350e, 0x00);
	ov4689_write_register(ViPipe, 0x350f, 0x80);
	ov4689_write_register(ViPipe, 0x3510, 0x00);
	ov4689_write_register(ViPipe, 0x3511, 0x00);
	ov4689_write_register(ViPipe, 0x3512, 0x00);
	ov4689_write_register(ViPipe, 0x3513, 0x00);
	ov4689_write_register(ViPipe, 0x3514, 0x00);
	ov4689_write_register(ViPipe, 0x3515, 0x80);
	ov4689_write_register(ViPipe, 0x3516, 0x00);
	ov4689_write_register(ViPipe, 0x3517, 0x00);
	ov4689_write_register(ViPipe, 0x3518, 0x00);
	ov4689_write_register(ViPipe, 0x3519, 0x00);
	ov4689_write_register(ViPipe, 0x351a, 0x00);
	ov4689_write_register(ViPipe, 0x351b, 0x80);
	ov4689_write_register(ViPipe, 0x351c, 0x00);
	ov4689_write_register(ViPipe, 0x351d, 0x00);
	ov4689_write_register(ViPipe, 0x351e, 0x00);
	ov4689_write_register(ViPipe, 0x351f, 0x00);
	ov4689_write_register(ViPipe, 0x3520, 0x00);
	ov4689_write_register(ViPipe, 0x3521, 0x80);
	ov4689_write_register(ViPipe, 0x3522, 0x08);
	ov4689_write_register(ViPipe, 0x3524, 0x08);
	ov4689_write_register(ViPipe, 0x3526, 0x08);
	ov4689_write_register(ViPipe, 0x3528, 0x08);
	ov4689_write_register(ViPipe, 0x352a, 0x08);
	ov4689_write_register(ViPipe, 0x3602, 0x00);
	ov4689_write_register(ViPipe, 0x3603, 0x40);
	ov4689_write_register(ViPipe, 0x3604, 0x02);
	ov4689_write_register(ViPipe, 0x3605, 0x00);
	ov4689_write_register(ViPipe, 0x3606, 0x00);
	ov4689_write_register(ViPipe, 0x3607, 0x00);
	ov4689_write_register(ViPipe, 0x3609, 0x12);
	ov4689_write_register(ViPipe, 0x360a, 0x40);
	ov4689_write_register(ViPipe, 0x360c, 0x08);
	ov4689_write_register(ViPipe, 0x360f, 0xe5);
	ov4689_write_register(ViPipe, 0x3608, 0x8f);
	ov4689_write_register(ViPipe, 0x3611, 0x00);
	ov4689_write_register(ViPipe, 0x3613, 0xf7);
	ov4689_write_register(ViPipe, 0x3616, 0x58);
	ov4689_write_register(ViPipe, 0x3619, 0x99);
	ov4689_write_register(ViPipe, 0x361b, 0x60);
	ov4689_write_register(ViPipe, 0x361c, 0x7a);
	ov4689_write_register(ViPipe, 0x361e, 0x79);
	ov4689_write_register(ViPipe, 0x361f, 0x02);
	ov4689_write_register(ViPipe, 0x3632, 0x00);
	ov4689_write_register(ViPipe, 0x3633, 0x10);
	ov4689_write_register(ViPipe, 0x3634, 0x10);
	ov4689_write_register(ViPipe, 0x3635, 0x10);
	ov4689_write_register(ViPipe, 0x3636, 0x15);
	ov4689_write_register(ViPipe, 0x3646, 0x86);
	ov4689_write_register(ViPipe, 0x364a, 0x0b);
	ov4689_write_register(ViPipe, 0x3700, 0x17);
	ov4689_write_register(ViPipe, 0x3701, 0x22);
	ov4689_write_register(ViPipe, 0x3703, 0x10);
	ov4689_write_register(ViPipe, 0x370a, 0x37);
	ov4689_write_register(ViPipe, 0x3705, 0x00);
	ov4689_write_register(ViPipe, 0x3706, 0x63);
	ov4689_write_register(ViPipe, 0x3709, 0x3c);
	ov4689_write_register(ViPipe, 0x370b, 0x01);
	ov4689_write_register(ViPipe, 0x370c, 0x30);
	ov4689_write_register(ViPipe, 0x3710, 0x24);
	ov4689_write_register(ViPipe, 0x3711, 0x0c);
	ov4689_write_register(ViPipe, 0x3716, 0x00);
	ov4689_write_register(ViPipe, 0x3720, 0x28);
	ov4689_write_register(ViPipe, 0x3729, 0x7b);
	ov4689_write_register(ViPipe, 0x372a, 0x84);
	ov4689_write_register(ViPipe, 0x372b, 0xbd);
	ov4689_write_register(ViPipe, 0x372c, 0xbc);
	ov4689_write_register(ViPipe, 0x372e, 0x52);
	ov4689_write_register(ViPipe, 0x373c, 0x0e);
	ov4689_write_register(ViPipe, 0x373e, 0x33);
	ov4689_write_register(ViPipe, 0x3743, 0x10);
	ov4689_write_register(ViPipe, 0x3744, 0x88);
	ov4689_write_register(ViPipe, 0x3745, 0xc0);
	ov4689_write_register(ViPipe, 0x374a, 0x43);
	ov4689_write_register(ViPipe, 0x374c, 0x00);
	ov4689_write_register(ViPipe, 0x374e, 0x23);
	ov4689_write_register(ViPipe, 0x3751, 0x7b);
	ov4689_write_register(ViPipe, 0x3752, 0x84);
	ov4689_write_register(ViPipe, 0x3753, 0xbd);
	ov4689_write_register(ViPipe, 0x3754, 0xbc);
	ov4689_write_register(ViPipe, 0x3756, 0x52);
	ov4689_write_register(ViPipe, 0x375c, 0x00);
	ov4689_write_register(ViPipe, 0x3760, 0x00);
	ov4689_write_register(ViPipe, 0x3761, 0x00);
	ov4689_write_register(ViPipe, 0x3762, 0x00);
	ov4689_write_register(ViPipe, 0x3763, 0x00);
	ov4689_write_register(ViPipe, 0x3764, 0x00);
	ov4689_write_register(ViPipe, 0x3767, 0x04);
	ov4689_write_register(ViPipe, 0x3768, 0x04);
	ov4689_write_register(ViPipe, 0x3769, 0x08);
	ov4689_write_register(ViPipe, 0x376a, 0x08);
	ov4689_write_register(ViPipe, 0x376b, 0x20);
	ov4689_write_register(ViPipe, 0x376c, 0x00);
	ov4689_write_register(ViPipe, 0x376d, 0x00);
	ov4689_write_register(ViPipe, 0x376e, 0x00);
	ov4689_write_register(ViPipe, 0x3773, 0x00);
	ov4689_write_register(ViPipe, 0x3774, 0x51);
	ov4689_write_register(ViPipe, 0x3776, 0xbd);
	ov4689_write_register(ViPipe, 0x3777, 0xbd);
	ov4689_write_register(ViPipe, 0x3781, 0x18);
	ov4689_write_register(ViPipe, 0x3783, 0x25);
	ov4689_write_register(ViPipe, 0x3798, 0x1b);
	ov4689_write_register(ViPipe, 0x3800, 0x00);
	ov4689_write_register(ViPipe, 0x3801, 0x08);
	ov4689_write_register(ViPipe, 0x3802, 0x00);
	ov4689_write_register(ViPipe, 0x3803, 0x04);
	ov4689_write_register(ViPipe, 0x3804, 0x0a);
	ov4689_write_register(ViPipe, 0x3805, 0x97);
	ov4689_write_register(ViPipe, 0x3806, 0x05);
	ov4689_write_register(ViPipe, 0x3807, 0xfb);
	ov4689_write_register(ViPipe, 0x3808, 0x0a);
	ov4689_write_register(ViPipe, 0x3809, 0x80);
	ov4689_write_register(ViPipe, 0x380a, 0x05);
	ov4689_write_register(ViPipe, 0x380b, 0xf0);
	ov4689_write_register(ViPipe, 0x380c, 0x0A);
	ov4689_write_register(ViPipe, 0x380d, 0x18);
	ov4689_write_register(ViPipe, 0x380e, 0x06);
	ov4689_write_register(ViPipe, 0x380f, 0x12);
	ov4689_write_register(ViPipe, 0x3810, 0x00);
	ov4689_write_register(ViPipe, 0x3811, 0x08);
	ov4689_write_register(ViPipe, 0x3812, 0x00);
	ov4689_write_register(ViPipe, 0x3813, 0x04);
	ov4689_write_register(ViPipe, 0x3814, 0x01);
	ov4689_write_register(ViPipe, 0x3815, 0x01);
	ov4689_write_register(ViPipe, 0x3819, 0x01);
	ov4689_write_register(ViPipe, 0x3820, 0x00);
	ov4689_write_register(ViPipe, 0x3821, 0x00);
	ov4689_write_register(ViPipe, 0x3829, 0x00);
	ov4689_write_register(ViPipe, 0x382a, 0x01);
	ov4689_write_register(ViPipe, 0x382b, 0x01);
	ov4689_write_register(ViPipe, 0x382d, 0x7f);
	ov4689_write_register(ViPipe, 0x3830, 0x04);
	ov4689_write_register(ViPipe, 0x3836, 0x01);
	ov4689_write_register(ViPipe, 0x3837, 0x00);
	ov4689_write_register(ViPipe, 0x3841, 0x02);
	ov4689_write_register(ViPipe, 0x3846, 0x08);
	ov4689_write_register(ViPipe, 0x3847, 0x07);
	ov4689_write_register(ViPipe, 0x3d85, 0x36);
	ov4689_write_register(ViPipe, 0x3d8c, 0x71);
	ov4689_write_register(ViPipe, 0x3d8d, 0xcb);
	ov4689_write_register(ViPipe, 0x3f0a, 0x00);
	ov4689_write_register(ViPipe, 0x4000, 0xf1);
	ov4689_write_register(ViPipe, 0x4001, 0x40);
	ov4689_write_register(ViPipe, 0x4002, 0x04);
	ov4689_write_register(ViPipe, 0x4003, 0x14);
	ov4689_write_register(ViPipe, 0x400e, 0x00);
	ov4689_write_register(ViPipe, 0x4011, 0x00);
	ov4689_write_register(ViPipe, 0x401a, 0x00);
	ov4689_write_register(ViPipe, 0x401b, 0x00);
	ov4689_write_register(ViPipe, 0x401c, 0x00);
	ov4689_write_register(ViPipe, 0x401d, 0x00);
	ov4689_write_register(ViPipe, 0x401f, 0x00);
	ov4689_write_register(ViPipe, 0x4020, 0x00);
	ov4689_write_register(ViPipe, 0x4021, 0x10);
	ov4689_write_register(ViPipe, 0x4022, 0x07);
	ov4689_write_register(ViPipe, 0x4023, 0xcf);
	ov4689_write_register(ViPipe, 0x4024, 0x09);
	ov4689_write_register(ViPipe, 0x4025, 0x60);
	ov4689_write_register(ViPipe, 0x4026, 0x09);
	ov4689_write_register(ViPipe, 0x4027, 0x6f);
	ov4689_write_register(ViPipe, 0x4028, 0x00);
	ov4689_write_register(ViPipe, 0x4029, 0x02);
	ov4689_write_register(ViPipe, 0x402a, 0x06);
	ov4689_write_register(ViPipe, 0x402b, 0x04);
	ov4689_write_register(ViPipe, 0x402c, 0x02);
	ov4689_write_register(ViPipe, 0x402d, 0x02);
	ov4689_write_register(ViPipe, 0x402e, 0x0e);
	ov4689_write_register(ViPipe, 0x402f, 0x04);
	ov4689_write_register(ViPipe, 0x4302, 0xff);
	ov4689_write_register(ViPipe, 0x4303, 0xff);
	ov4689_write_register(ViPipe, 0x4304, 0x00);
	ov4689_write_register(ViPipe, 0x4305, 0x00);
	ov4689_write_register(ViPipe, 0x4306, 0x00);
	ov4689_write_register(ViPipe, 0x4308, 0x02);
	ov4689_write_register(ViPipe, 0x4500, 0x6c);
	ov4689_write_register(ViPipe, 0x4501, 0xc4);
	ov4689_write_register(ViPipe, 0x4502, 0x40);
	ov4689_write_register(ViPipe, 0x4503, 0x01);
	ov4689_write_register(ViPipe, 0x4601, 0xA7);
	ov4689_write_register(ViPipe, 0x4800, 0x04);
	ov4689_write_register(ViPipe, 0x4813, 0x08);
	ov4689_write_register(ViPipe, 0x481f, 0x40);
	ov4689_write_register(ViPipe, 0x4829, 0x78);
	ov4689_write_register(ViPipe, 0x4837, 0x10);
	ov4689_write_register(ViPipe, 0x4b00, 0x2a);
	ov4689_write_register(ViPipe, 0x4b0d, 0x00);
	ov4689_write_register(ViPipe, 0x4d00, 0x04);
	ov4689_write_register(ViPipe, 0x4d01, 0x42);
	ov4689_write_register(ViPipe, 0x4d02, 0xd1);
	ov4689_write_register(ViPipe, 0x4d03, 0x93);
	ov4689_write_register(ViPipe, 0x4d04, 0xf5);
	ov4689_write_register(ViPipe, 0x4d05, 0xc1);
	ov4689_write_register(ViPipe, 0x5000, 0xf3);
	ov4689_write_register(ViPipe, 0x5001, 0x11);
	ov4689_write_register(ViPipe, 0x5004, 0x00);
	ov4689_write_register(ViPipe, 0x500a, 0x00);
	ov4689_write_register(ViPipe, 0x500b, 0x00);
	ov4689_write_register(ViPipe, 0x5032, 0x00);
	ov4689_write_register(ViPipe, 0x5040, 0x00);
	ov4689_write_register(ViPipe, 0x5050, 0x0c);
	ov4689_write_register(ViPipe, 0x5500, 0x00);
	ov4689_write_register(ViPipe, 0x5501, 0x10);
	ov4689_write_register(ViPipe, 0x5502, 0x01);
	ov4689_write_register(ViPipe, 0x5503, 0x0f);
	ov4689_write_register(ViPipe, 0x8000, 0x00);
	ov4689_write_register(ViPipe, 0x8001, 0x00);
	ov4689_write_register(ViPipe, 0x8002, 0x00);
	ov4689_write_register(ViPipe, 0x8003, 0x00);
	ov4689_write_register(ViPipe, 0x8004, 0x00);
	ov4689_write_register(ViPipe, 0x8005, 0x00);
	ov4689_write_register(ViPipe, 0x8006, 0x00);
	ov4689_write_register(ViPipe, 0x8007, 0x00);
	ov4689_write_register(ViPipe, 0x8008, 0x00);
	ov4689_write_register(ViPipe, 0x3638, 0x00);

	ov4689_default_reg_init(ViPipe);

	ov4689_write_register(ViPipe, 0x0100, 0x01);

	delay_ms(100);

	printf("ViPipe:%d,===OV4689 1520P 30fps 10bit LINE Init OK!===\n", ViPipe);
}




