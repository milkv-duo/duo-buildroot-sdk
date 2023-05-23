#include "lt9611.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "cvi_sys.h"
#include <getopt.h>

#include "sample_comm.h"

#define LT9611_I2C_NAME 	"lt9611"
#define LT9611_I2C_ADDR      0x3B    /* 7bit addr */
#define LT9611_I2C_DEV       4

void Timer0_Delay1ms(uint32_t u32CNT)
{
    usleep(u32CNT*1000);
}

/******************************** debug *******************/
#define _pcr_mk_printf_
#define _mipi_Dphy_debug_
#define _htotal_stable_check_

/* pattern */
//#define _pattern_test_

//#define cec_on

#define _enable_read_edid_
/***********************************************************/
#define single_port_mipi  1
#define dual_port_mipi    2

//Do not support 3lane
#define lane_cnt_1   1
#define lane_cnt_2   2
#define lane_cnt_4   0

#define audio_i2s     0
#define audio_spdif   1

#define dsi           0
#define csi           1

#define Non_Burst_Mode_with_Sync_Events 0x00
#define Non_Burst_Mode_with_Sync_Pulses 0x01
#define Burst_Mode                      0x02

#define ac_mode     0
#define dc_mode     1

#define hdcp_disable 0
#define hdcp_enable 1

#define HPD_INTERRUPT_ENABLE            0x01
#define VID_CHG_INTERRUPT_ENABLE    0x02
#define CEC_INTERRUPT_ENABLE             0x03

#define RGB888 0x00
#define YUV422 0x01

#define DVI 0x00
#define HDMI 0x01

#define AR_ND 0x00
#define AR_4_3 0x01
#define AR_16_9 0x02


#define	MPEG_PKT_EN 0x01
#define	AIF_PKT_EN  0x02
#define SPD_PKT_EN	0x04
#define AVI_PKT_EN	0x08
#define UD1_PKT_EN	0x10
#define UD0_PKT_EN	0x20

#define CEC_SEND_DONE	0x01
#define CEC_REC_DATA	0x02
#define CEC_NACK	    0x04
#define CEC_ARB_LOST	0x08
#define CEC_ERROR_INITIATOR	0x10
#define CEC_ERROR_FOLLOWER	0x20
#define CEC_WAKE_UP 	0x40


#define	MSG_USRCNTRL_PRESS		0X44
#define	MSG_USRCNTRL_RELEASE	0X45
#define	MSG_OSD_GET				0x46
#define	MSG_OSD_SET				0x47
#define	MSG_PHYADDR_GET			0x83
#define	MSG_PHYADDR_SET			0x84
#define	MSG_STREAM_SET			0X86
#define	MSG_ACTIVESOURCE_GET	0X82
#define	MSG_VENDOR_GET			0x8C
#define	MSG_MENUSTATUS_GET		0X8D
#define	MSG_MENUSTATUS_SET		0X8E
#define	MSG_MEBUSTATUS_ACTIVE	0X00
#define	MSG_POWERSTATUS_GET		0X8F
#define	MSG_POWERSTATUS_SET		0X90
#define	MSG_POWERSTATUS_ON		0X00
#define	MSG_CECVERSION_GET		0x9F
#define	MSG_CECVERSION_SET		0X9E
#define	MSG_CECVERSION_1_4		0X05
#define	MSG_FEATURE_ABRT		0x00
#define	MSG_ABORT		        0xFF
#define	MSG_ABRT_REASON			0x00


#define CEC_ABORT_REASON_0 	0x00 //Unrecoanized opcode
#define CEC_ABORT_REASON_1 	0x01 //Not in correct mode to respond
#define CEC_ABORT_REASON_2 	0x02 //Cannot provide source
#define CEC_ABORT_REASON_3 	0x03 //Invalid operand
#define CEC_ABORT_REASON_4 	0x04 //Refused
#define CEC_ABORT_REASON_5 	0x05 //Unable to determine

typedef struct Lontium_IC_Mode{
    CVI_U8 mipi_port_cnt; //1 or 2
    CVI_U8 mipi_lane_cnt; //1 or 2 or 4
    bool mipi_mode;   //dsi or csi
    CVI_U8 video_mode;    //Non-Burst Mode with Sync Pulses; Non-Burst Mode with Sync Events
    bool audio_out;   //i2s or spdif
    bool hdmi_coupling_mode;//ac_mode or dc_mode
    bool hdcp_encryption; //hdcp_enable or hdcp_diable
    bool hdmi_mode;
        CVI_U8 input_color_space;  //RGB888 or YUV422
} __Lontium_IC_Mode;


typedef struct video_timing{
    CVI_U16 hfp;
        CVI_U16 hs;
        CVI_U16 hbp;
        CVI_U16 hact;
        CVI_U16 htotal;
        CVI_U16 vfp;
        CVI_U16 vs;
        CVI_U16 vbp;
        CVI_U16 vact;
        CVI_U16 vtotal;
        bool h_polarity;
        bool v_polarity;
        CVI_U16	vic;
        CVI_U8 aspact_ratio;  // 0=no data, 1=4:3, 2=16:9, 3=no data.
    CVI_U32 pclk_khz;
} __video_timing;

typedef struct cec_msg{
    CVI_U8 cec_status;
        CVI_U8 rx_data_buff[16];
        CVI_U8 tx_data_buff[18];
        CVI_U8 logical_address;
        CVI_U16 physical_address;
        CVI_U8 destintion;
        CVI_U8 retries_times;
        bool la_allocation_done;
        bool report_physical_address_done;
} __cec_msg;

typedef enum VideoFormat
{
	video_640x480_60Hz_vic1 = 1,       //vic 1
	video_720x480_60Hz_vic3,       //vic 2
	video_720x576_50Hz_vic,
	video_1280x720_60Hz_vic4,      //vic 3
	video_1920x1080_60Hz_vic16,    //vic 4

	video_1920x1080i_60Hz_169,  //vic 5
	video_720x480i_60Hz_43,     //vic 6
	video_720x480i_60Hz_169,    //vic 7
	video_720x240P_60Hz_43,     //vic 8
	video_720x240P_60Hz_169,    //vic 9

	video_1280x720_50Hz_vic,
	video_1280x720_30Hz_vic,

	video_3840x2160_30Hz_vic,
	video_3840x2160_25Hz_vic,
	video_3840x2160_24Hz_vic,

	video_3840x1080_60Hz_vic,
	video_1024x600_60Hz_vic,
	video_1080x1920_60Hz_vic,
	video_720x1280_60Hz_vic,
	video_1280x800_60Hz_vic,
	video_540x960_60Hz_vic,
	video_1366x768_60Hz_vic,

	video_2560x1600_60Hz_vic,
	video_2560x1440_60Hz_vic,
	video_2560x1080_60Hz_vic,

	video_1920x1080_50Hz_vic,
	video_1920x1080_30Hz_vic,
	video_1920x1080_25Hz_vic,
	video_1920x1080_24Hz_vic,

	video_2560x720_60Hz_vic,

	video_other,
	video_none
}__VideoFormat;

typedef enum
{
	I2S_2CH,
	I2S_8CH,
	SPDIF
} _Audio_Input_Mode;

#define     Audio_Input_Mode    I2S_2CH

typedef enum
{
	Input_RGB888,
	Input_RGB565,
	Input_YCbCr444,
	Input_YCbCr422_16BIT,
	Input_YCbCr422_20BIT,
	Input_YCbCr422_24BIT,
	Input_BT1120_16BIT,
	Input_BT1120_20BIT,
	Input_BT1120_24BIT,
	Input_BT656_8BIT,
	Input_BT656_10BIT,
	Input_BT656_12BIT
} _Video_Input_Mode;

#define Video_Input_Mode Input_RGB888

typedef enum
{
	Output_RGB888,
	Output_YCbCr444,
	Output_YCbCr422_16BIT,
	Output_YCbCr422_20BIT,
	Output_YCbCr422_24BIT
} _Video_Output_Mode;

#define Video_Output_Mode  Output_RGB888

struct Lontium_IC_Mode lt9611 = {
single_port_mipi, //mipi_port_cnt; //single_port_mipi or dual_port_mipi
lane_cnt_4,       //mipi_lane_cnt; //1 or 2 or 4
dsi,              //mipi_mode;     //dsi or csi
Burst_Mode,
audio_i2s,       //audio_out      //audio_i2s or audio_spdif       //hdmi_coupling_mode;//ac_mode or dc_mode
dc_mode,         //hdmi_coupling_mode;//ac_mode or dc_mode
hdcp_disable,    //hdcp_encryption //hdcp_enable or hdcp_disable
HDMI,            //HDMI or DVI
RGB888           //RGB888 or YUV422
};

#define MAX_CMD_OPTIONS	128
typedef enum _ARG_TYPE_ {
	ARG_INT = 0,
	ARG_UINT,
	ARG_STRING,
} ARG_TYPE;

typedef struct _optionExt_ {
	struct option opt;
	int type;
	int64_t min;
	int64_t max;
	const char *help;
} optionExt;

static optionExt long_option_ext[] = {
	{{"resolution",    optional_argument, NULL, 'r'},   ARG_STRING,    0,   0,
		"select hdmi display solution"},
	{{"help",      no_argument, NULL, 'h'},       ARG_STRING, 0,   0,
		"print usage."},
	{{NULL, 0, NULL, 0}, ARG_INT, 0, 0, "no param: just init the hdmi."}
};

static const char* s_hdmi_resolution_arr[] = {
	"1024x768_60HZ",
	"1280x720_60HZ",
	"1280x1024_60HZ",
	"1600x1200_60HZ",
	"1920x1080_30HZ",
	"1920x1080_60HZ",
};

static LT9611_VIDEO_FORMAT_E g_video_format = E_LT9611_VIDEO_1920x1080_60HZ;
//DTV 									      // hfp, hs, hbp,hact,htotal,vfp, vs, vbp,vact,vtotal, hfp, hs, hbp,hact,htotal,vfp, vs, vbp,vact,vtotal, h_polary, v_polary, vic, pclk_khz
static struct video_timing video_640x480_60Hz     ={16, 96,  48, 640,   800, 10,  2,  33, 480,   525, 0, 0, 1, AR_4_3, 25000};
static struct video_timing video_720x480_60Hz     ={16, 62,  60, 720,   858,  9,  6,  30, 480,   525, 0, 0, 2, AR_4_3, 27000};
static struct video_timing video_720x576_50Hz     ={12, 64,  68, 720,   864,  5,  5,  39, 576,   625, 0, 0, 17,AR_4_3, 27000};

static struct video_timing video_1280x720_60Hz    ={110,40, 220,1280,  1650,  5,  5,  20, 720,   750, 1, 1, 4, AR_16_9,74250};
static struct video_timing video_1280x720_50Hz    ={440,40, 220,1280,  1980,  5,  5,  20, 720,   750, 1, 1, 19,AR_16_9,74250};
static struct video_timing video_1280x720_30Hz    ={1760, 40, 220,1280,  3300,  5,  5,  20, 720, 750,   1,1,0, AR_16_9,74250};

static struct video_timing video_1920x1080_60Hz   ={88, 44, 148,1920,  2200,  4,  5,  36, 1080,  1125, 1,1,16,AR_16_9,148500};
static struct video_timing video_1920x1080_50Hz   ={528, 44, 148,1920, 2640,  4,  5,  36, 1080, 1125, 1,1,31,AR_16_9,148500};
static struct video_timing video_1920x1080_30Hz   ={88, 44, 148,1920,  2200,  4,  5,  36, 1080,  1125, 1,1,34,AR_16_9,74250};
static struct video_timing video_1920x1080_25Hz   ={528, 44, 148,1920,  2640,  4,  5,  36, 1080, 1125, 1,1,33,AR_16_9,74250};
static struct video_timing video_1920x1080_24Hz   ={638, 44, 148,1920,  2750,  4,  5,  36, 1080, 1125, 1,1,32,AR_16_9,74250};

static struct video_timing video_3840x2160_30Hz   ={176,88, 296,3840,  4400,  8,  10, 72, 2160, 2250, 1,1,95,AR_16_9, 297000};
static struct video_timing video_3840x2160_25Hz   ={1056,88, 296,3840,  5280,  8,  10, 72, 2160, 2250,1,1,94,AR_16_9, 297000};
static struct video_timing video_3840x2160_24Hz   ={1276,88, 296,3840,  5500,  8,  10, 72, 2160, 2250,1,1,93,AR_16_9,297000};

static struct video_timing video_1024x768_60Hz    ={24, 136, 160, 1024, 1344,  3,  6,  29, 768, 806,   0,0,0,AR_16_9,65000};
static struct video_timing video_1280x1024_60Hz   ={48, 112, 248, 1280, 1688,  1,  3,  38, 1024, 1066, 1,1,0,AR_16_9,108000};
static struct video_timing video_1600x1200_60Hz   ={64, 192, 304, 1600, 2160,  1,  3,  46, 1200, 1250, 1,1,0,AR_16_9,162000};

static struct video_timing video_1280x800_60Hz   ={28, 32, 100, 1280, 1440,  2,  6,  15, 800, 823, 0,0,0,AR_16_9,71000};

/*
code struct video_timing video_2560x1080_60Hz   ={248,44, 148,2560,2720,  4,  5, 11, 1080,1100, 1,1,0,AR_16_9,198000};

//VESA-DMT
code struct video_timing video_800x600_60Hz     ={48, 128, 88, 800, 1056,  1,  4,  23, 600, 628,     1,1,0,AR_16_9,40000};
code struct video_timing video_1024x768_60Hz    ={24, 136, 160, 1024, 1344,  3,  6,  29, 768, 806,   0,0,0,AR_16_9,65000};
//code struct video_timing video_1280x768_60Hz    ={48, 32,   80, 1280, 1440,  3,  7,  12, 768, 790,   1,0,0,AR_16_9,68250}; //reduced blank
code struct video_timing video_1280x768_60Hz    ={64, 128,   192, 1280, 1664,  3,  7,  20, 768, 798,   0,1,0,AR_16_9,79500}; //Normal

//20190328
//code struct video_timing video_1280x800_60Hz   ={72, 128, 200, 1280, 1680,  3,  6,  22, 800, 831, 0,1,0,AR_16_9,83500};
code struct video_timing video_1280x800_60Hz   ={28, 32, 100, 1280, 1440,  2,  6,  15, 800, 823, 0,0,0,AR_16_9,71000};
code struct video_timing video_1280x960_60Hz   ={96, 112, 312, 1280, 1800,  1,  3,  36, 960, 1000, 1,1,0,AR_16_9,108000};

code struct video_timing video_1280x800_30Hz   ={72, 128, 200, 1280, 1680,  3,  6,  22, 800, 831, 0,1,0,AR_16_9,41750};
code struct video_timing video_1280x960_30Hz   ={96, 112, 312, 1280, 1800,  1,  3,  36, 960, 1000, 1,1,0,AR_16_9,54000};

code struct video_timing video_1280x1024_60Hz   ={48, 112, 248, 1280, 1688,  1,  3,  38, 1024, 1066, 1,1,0,AR_16_9,108000};
code struct video_timing video_1600x1200_60Hz   ={64, 192, 304, 1600, 2160,  1,  3,  46, 1200, 1250, 1,1,0,AR_16_9,162000};
//code struct video_timing video_1680x1050_60Hz   ={48, 32,   80, 1680, 1840,  3,  6,  21, 1050, 1080, 1,0,0,AR_16_9,119000}; //reduced blank
code struct video_timing video_1680x1050_60Hz   ={104, 176,   280, 1680, 2240,  3,  6,  30, 1050, 1089, 0,1,0,AR_16_9,146250};//Normal

code struct video_timing video_1280x720_30Hz    ={1760, 40, 220,1280,  3300,  5,  5,  20, 720, 750,   1,1,0, AR_16_9,74250};
code struct video_timing video_1024x600_60Hz    ={60,60, 100,1024,  1154,  2,  5, 10, 600, 617,      1,1,0,AR_16_9,34000};

code struct video_timing video_1366x768_60Hz    ={14, 56,  64,1366,  1500,  1,  3,  28, 768,  800,   1,1,0, AR_16_9,74000};

code struct video_timing video_2560x1440_50Hz   ={176,272, 448,2560,  3456,  3,  5, 36, 1440, 1484,  1,1,0, AR_16_9,256250};
code struct video_timing video_2560x1440_60Hz   ={48,32, 80,2560,  2720,  3,  5, 33, 1440, 1481,     1,1,0, AR_16_9,241500};

//20180731								        //hfp, hs, hbp,hact,htotal,vfp, vs, vbp,vact,vtotal, pclk_khz
code struct video_timing video_2560x1600_60Hz   ={48,32, 80,2560,  2720,  3,  6, 37, 1600, 1646,     1,1,0,AR_16_9,268500};

//VR timing
code struct video_timing video_3840x1080_60Hz   ={176,88, 296,3840,  4400,  4,  5,  36, 1080, 1125,   1,1,0, AR_16_9,297000};
code struct video_timing video_2560x720_60Hz    ={220,80, 440,2560,  3300,  5,  5,  20, 720,   750, 1, 1, 0, AR_16_9,148500};

//mipi panel resolution
code struct video_timing video_1080x1920_60Hz  ={  4,  5,  36, 1080, 1125, 88, 44, 148,1920,  2200,  1,1,0,AR_16_9,148500};
code struct video_timing video_720x1280_60Hz    ={  5,  5,  20, 720,   750, 110,40, 220,1280,  1650,  1,1,0,AR_16_9,74250};
code struct video_timing video_540x960_60Hz      ={30, 10, 30, 540, 610,  10,  10,  10, 960, 990,      1,1,0,AR_16_9,33500};
*/

//others
static struct video_timing video_1200x1920_60Hz    ={180,60, 160, 1200, 1600,  35, 10, 25, 1920, 1990, 1, 1, 0, AR_16_9,191040}; 
static struct video_timing video_1920x720_60Hz     ={88, 44, 148, 1920, 2200,  5,  5,  20, 720,  750,  1, 1, 0, AR_16_9,100000}; 


struct video_timing *video;
struct cec_msg lt9611_cec_msg = {0};

//static CVI_U8 CEC_RxData_Buff[16];
static CVI_U8 CEC_TxData_Buff[18];

CVI_U8 pcr_m;
bool flag_cec_data_received = 0;

CVI_U8 Sink_EDID[256];
bool Tx_HPD=0;

enum VideoFormat Video_Format;
static int lt9611_i2c_write_byte(CVI_U8 u8RegAddr, CVI_U8 u8Data);
static int lt9611_i2c_read_byte(CVI_U8 u8RegAddr);
static int fd = -1;

void LT9611_init_i2c(void)
{
    char temp[16] = {0};

    snprintf(temp, sizeof(temp), "/dev/i2c-%u", LT9611_I2C_DEV);

	fd = open(temp, O_RDWR);

	if(fd < 0)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "open %s error!!!", temp);
	}
}

static int lt9611_i2c_write_byte(CVI_U8 u8RegAddr, CVI_U8 u8Data)
{
	uint8_t temp[2] = {0};

	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msg;

	data.msgs = &msg;
	
	temp[0] = u8RegAddr;
	temp[1] = u8Data;

	data.msgs[0].addr = LT9611_I2C_ADDR;
	data.msgs[0].flags = 0;
	data.msgs[0].len = 2;
	data.msgs[0].buf = temp;

	data.nmsgs = 1;

	int ret = ioctl(fd, I2C_RDWR, (unsigned long) &data);

	if(ret < 0)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "lt9611_i2c_write_byte, addr: %x error!!!", u8RegAddr);
			return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

static int lt9611_i2c_read_byte(CVI_U8 u8RegAddr)
{
	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msg[2];
	
	uint8_t result = 0x00;

	data.msgs = &msg[0];

	data.msgs[0].addr = LT9611_I2C_ADDR;
	data.msgs[0].flags = 0;
	data.msgs[0].len = 1;
	data.msgs[0].buf = &u8RegAddr;

	data.msgs[1].addr = LT9611_I2C_ADDR;
	data.msgs[1].flags = I2C_M_RD;
	data.msgs[1].len = 1;
	data.msgs[1].buf = &result;

	data.nmsgs = 2;

	int ret = ioctl(fd, I2C_RDWR, (unsigned long) &data);

	if(ret < 0)
	{
		CVI_TRACE_LOG(CVI_DBG_ERR, "lt9611_i2c_read_byte, addr: %x error!!!", u8RegAddr);
			return -1;
	}

	return result;
}

void LT9611_Chip_ID(void)
{
    lt9611_i2c_write_byte(0xFF,0x80);
    lt9611_i2c_write_byte(0xee,0x01);
    CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611 ring Chip ID = 0x%x, 0x%x, 0x%x\n",lt9611_i2c_read_byte(0x00),
    lt9611_i2c_read_byte(0x01), lt9611_i2c_read_byte(0x02));
    lt9611_i2c_write_byte(0xFF,0x81);
    lt9611_i2c_write_byte(0x01,0x18); //sel xtal clock
    lt9611_i2c_write_byte(0xFF,0x80);
}

void LT9611_RST_PD_Init(void)
{
	/* power consumption for standby */
	lt9611_i2c_write_byte(0xFF,0x81);
	lt9611_i2c_write_byte(0x02,0x48);
	lt9611_i2c_write_byte(0x23,0x80);
	lt9611_i2c_write_byte(0x30,0x00);
	lt9611_i2c_write_byte(0x01,0x00); /* i2c stop work */
}

void LT9611_LowPower_mode(bool on)
{
	/* only hpd irq is working for low power consumption */
	/* 1.8V: 15 mA */
    if(on)
    {
        lt9611_i2c_write_byte(0xFF,0x81);
        lt9611_i2c_write_byte(0x02,0x49);
        lt9611_i2c_write_byte(0x23,0x80);
        lt9611_i2c_write_byte(0x30,0x00); //0x00 --> 0xc0, tx phy and clk can not power down, otherwise dc det don't work.

        lt9611_i2c_write_byte(0xff,0x80);
        lt9611_i2c_write_byte(0x11,0x0a);
    }
	else
	{
		lt9611_i2c_write_byte(0xFF,0x81);
		lt9611_i2c_write_byte(0x02,0x12);
		lt9611_i2c_write_byte(0x23,0x40);
		lt9611_i2c_write_byte(0x30,0xea);

		lt9611_i2c_write_byte(0xff,0x80);
		lt9611_i2c_write_byte(0x11,0xfa);
	}
}

void LT9611_System_Init(void)  //dsren
{
	lt9611_i2c_write_byte(0xFF,0x82);
	lt9611_i2c_write_byte(0x51,0x11);
	//Timer for Frequency meter
	lt9611_i2c_write_byte(0xFF,0x82);
	lt9611_i2c_write_byte(0x1b,0x69); //Timer 2
	lt9611_i2c_write_byte(0x1c,0x78);
	lt9611_i2c_write_byte(0xcb,0x69); //Timer 1
	lt9611_i2c_write_byte(0xcc,0x78);

	/*power consumption for work*/
	lt9611_i2c_write_byte(0xff,0x80);
	lt9611_i2c_write_byte(0x04,0xf0);
	lt9611_i2c_write_byte(0x06,0xf0);
	lt9611_i2c_write_byte(0x0a,0x80);
	lt9611_i2c_write_byte(0x0b,0x46); //csc clk
	lt9611_i2c_write_byte(0x0d,0xef);
	lt9611_i2c_write_byte(0x11,0xfa);
}

void LT9611_MIPI_Input_Analog(void)//xuxi
{
	//mipi mode
	lt9611_i2c_write_byte(0xff,0x81);
	lt9611_i2c_write_byte(0x06,0x60); //port A rx current
	lt9611_i2c_write_byte(0x07,0x3f); //eq
	lt9611_i2c_write_byte(0x08,0x3f); //eq
	lt9611_i2c_write_byte(0x09,0x09);

	lt9611_i2c_write_byte(0x0a,0xfe); //port A ldo voltage set
	lt9611_i2c_write_byte(0x0b,0xbf); //enable port A lprx

	lt9611_i2c_write_byte(0x11,0x60); //port B rx current
	lt9611_i2c_write_byte(0x12,0x3f); //eq
	lt9611_i2c_write_byte(0x13,0x3f); //eq
	lt9611_i2c_write_byte(0x15,0xfe); //port B ldo voltage set
	lt9611_i2c_write_byte(0x16,0xbf); //enable port B lprx

	lt9611_i2c_write_byte(0x1c,0x03); //PortA clk lane no-LP mode.
	lt9611_i2c_write_byte(0x20,0x03); //PortB clk lane no-LP mode.
}

void LT9611_MIPI_Input_Digtal(void) //weiguo
{
	CVI_U8 lanes;
	lanes = lt9611.mipi_lane_cnt;

	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_MIPI_Input_Digtal: lt9611 set mipi lanes = %d", lanes);

	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0x4f,0x80);    //[7] = Select ad_txpll_d_clk.
	lt9611_i2c_write_byte(0x50,0x10);

	lt9611_i2c_write_byte(0xff,0x83);
	lt9611_i2c_write_byte(0x00,lanes);
	lt9611_i2c_write_byte(0x02,0x08); //settle
	lt9611_i2c_write_byte(0x06,0x08); //settle

	if(1)//(P10) //single_port_mipi
	{
		lt9611_i2c_write_byte(0x0a,0x00); //1=dual_lr, 0=dual_en
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_MIPI_Input_Digtal: lt9611 set mipi ports = 1\n");
	}
	else    //dual_port_mipi
	{
	lt9611_i2c_write_byte(0x0a,0x03); //1=dual_lr, 0=dual_en
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_MIPI_Input_Digtal: lt9611 set mipi port = 2\n");
	}

#if 1
	if(lt9611.mipi_mode==csi)
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_MIPI_Input_Digtal: LT9611.mipi_mode = csi\n");
		lt9611_i2c_write_byte(0xff,0x83);
		lt9611_i2c_write_byte(0x08,0x10); //csi_en
		lt9611_i2c_write_byte(0x2c,0x40); //csi_sel

       	if(lt9611.input_color_space == RGB888)
    	{
    	   lt9611_i2c_write_byte(0xff,0x83);
    	   lt9611_i2c_write_byte(0x1c,0x01);
    	}
	}
	else
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_MIPI_Input_Digtal: LT9611.mipi_mode = dsi\n");
#endif
}

void LT9611_Video_Check(void) //dsren
{
#if 1
	CVI_U8 mipi_video_format=0x00;
	CVI_U16 h_act, h_act_a ,h_act_b, v_act,v_tal;
	CVI_U16 h_total_sysclk;

	lt9611_i2c_write_byte(0xff,0x82); // top video check module
    h_total_sysclk = lt9611_i2c_read_byte(0x86);
    h_total_sysclk = (h_total_sysclk<<8) + lt9611_i2c_read_byte(0x87);
    CVI_TRACE_LOG(CVI_DBG_INFO, "\33[32m\n");
    CVI_TRACE_LOG(CVI_DBG_INFO, "-----------------------------------------------------------------------------\n");
    CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Video_Check: h_total_sysclk = %d",h_total_sysclk);

	v_act=lt9611_i2c_read_byte(0x82);
	v_act=(v_act<<8)+lt9611_i2c_read_byte(0x83);
	v_tal=lt9611_i2c_read_byte(0x6c);
	v_tal=(v_tal<<8)+lt9611_i2c_read_byte(0x6d);

	lt9611_i2c_write_byte(0xff,0x83);
	h_act_a = lt9611_i2c_read_byte(0x82);
	h_act_a = (h_act_a<<8)+lt9611_i2c_read_byte(0x83);

	h_act_b = lt9611_i2c_read_byte(0x86);
	h_act_b =(h_act_b<<8)+lt9611_i2c_read_byte(0x87);


	if(lt9611.input_color_space==YUV422)
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Video_Check: lt9611.input_color_space = YUV422\n");
		h_act_a /= 2;
		h_act_b /= 2;
	}
	else if(lt9611.input_color_space==RGB888)
	{
	  	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Video_Check: lt9611.input_color_space = RGB888\n");
	   	h_act_a /= 3;
		h_act_b /= 3;
	}

	mipi_video_format=lt9611_i2c_read_byte(0x88);

	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Video_Check: h_act_a, h_act_b, v_act, v_tal: %d, %d, %d, %d, ",h_act_a, h_act_b, v_act, v_tal);
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Video_Check: mipi_video_format: 0x%x",mipi_video_format);

	if(0)//(P10 == 0) //dual port.
		h_act = h_act_a + h_act_b;
	else
		h_act = h_act_a;

	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Video_Check: Video_Format =\n");
///////////////////////formate detect///////////////////////////////////

//DTV
	if((h_act==video_640x480_60Hz.hact)&&(v_act==video_640x480_60Hz.vact))
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_640x480_60Hz \n");
		Video_Format=video_640x480_60Hz_vic1;
		video = &video_640x480_60Hz;
	}
	else if((h_act==(video_720x480_60Hz.hact))&&(v_act==video_720x480_60Hz.vact))
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_720x480_60Hz \n");
		Video_Format=video_720x480_60Hz_vic3;
		video = &video_720x480_60Hz;
	}

	else if((h_act==(video_720x576_50Hz.hact))&&(v_act==video_720x576_50Hz.vact))
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_720x576_50Hz \n");
		Video_Format=video_720x576_50Hz_vic;
		video = &video_720x576_50Hz;
	}

	else if((h_act==video_1280x720_60Hz.hact) && (v_act==video_1280x720_60Hz.vact))
	{
		if(h_total_sysclk < 630)
		{
				CVI_TRACE_LOG(CVI_DBG_INFO, " video_1280x720_60Hz \n");
				Video_Format=video_1280x720_60Hz_vic4;
				video = &video_1280x720_60Hz;
		}
		else if(h_total_sysclk < 750)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1280x720_50Hz \n");
			Video_Format=video_1280x720_50Hz_vic;
			video = &video_1280x720_50Hz;
		}
		else if(h_total_sysclk < 1230)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1280x720_30Hz \n");
			Video_Format=video_1280x720_30Hz_vic;
			video = &video_1280x720_30Hz;
		}
	}

	else if((h_act==(video_1280x800_60Hz.hact))&&(v_act==video_1280x800_60Hz.vact))
	{
		if(h_total_sysclk < 560)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1280x800_60Hz \n");
			Video_Format = video_other;
			video = &video_1280x800_60Hz;
		}
	}

	else if((h_act==video_1920x1080_60Hz.hact) && (v_act==video_1920x1080_60Hz.vact))//1080P
	{
		if(h_total_sysclk < 430)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1920x1080_60Hz \n");
			Video_Format=video_1920x1080_60Hz_vic16;
			video = &video_1920x1080_60Hz;
		}

		else if(h_total_sysclk < 510)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1920x1080_50Hz \n");
			Video_Format=video_1920x1080_50Hz_vic;
			video = &video_1920x1080_50Hz;
		}

		else if(h_total_sysclk < 830)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1920x1080_30Hz \n");
			Video_Format=video_1920x1080_30Hz_vic;
			video = &video_1920x1080_30Hz;
		}

		else if(h_total_sysclk < 980)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1920x1080_25Hz \n");
			Video_Format=video_1920x1080_25Hz_vic;
			video = &video_1920x1080_25Hz;
		}

		else if(h_total_sysclk < 1030)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_1920x1080_24Hz \n");
			Video_Format=video_1920x1080_24Hz_vic;
			video = &video_1920x1080_24Hz;
		}
	}
	else if((h_act==video_3840x2160_30Hz.hact) && (v_act==video_3840x2160_30Hz.vact)) //2160P
	{
		if(h_total_sysclk < 430)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_3840x2160_30Hz \n");
			Video_Format=video_3840x2160_30Hz_vic;
			video = &video_3840x2160_30Hz;
		}
		else if(h_total_sysclk < 490)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_3840x2160_25Hz \n");
			Video_Format=video_3840x2160_25Hz_vic;
			video = &video_3840x2160_25Hz;
		}
		else if(h_total_sysclk < 520)
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, " video_3840x2160_24Hz \n");
			Video_Format=video_3840x2160_24Hz_vic;
			video = &video_3840x2160_24Hz;
		}
	}
	else if((h_act==(video_1024x768_60Hz.hact))&&(v_act==video_1024x768_60Hz.vact))
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_1024x768_60Hz \n");
		Video_Format = video_other;
		video = &video_1024x768_60Hz;
	}
	else if((h_act==(video_1280x1024_60Hz.hact))&&(v_act==video_1280x1024_60Hz.vact))
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_1280x1024_60Hz \n");
		Video_Format = video_other;
		video = &video_1280x1024_60Hz;
	}
	else if((h_act==(video_1600x1200_60Hz.hact))&&(v_act==video_1600x1200_60Hz.vact))
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_1600x1200_60Hz \n");
		Video_Format = video_other;
		video = &video_1600x1200_60Hz;
	}
	else if((h_act==video_1200x1920_60Hz.hact)&&(v_act==video_1200x1920_60Hz.vact))//&&
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_1200x1920_60Hz \n");
		Video_Format = video_other;
		video = &video_1200x1920_60Hz;
	}
	else if((h_act==video_1920x720_60Hz.hact)&&(v_act==video_1920x720_60Hz.vact))//&&
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, " video_1920x720_60Hz \n");
		Video_Format = video_other;
		video = &video_1920x720_60Hz;
	}
	else
	{
		Video_Format = video_none;
		CVI_TRACE_LOG(CVI_DBG_INFO, " unknown video format \n");
	}
	CVI_TRACE_LOG(CVI_DBG_INFO, "-----------------------------------------------------------------------------\n");
#endif

}


void LT9611_MIPI_Video_Timing(struct video_timing *video_format) //weiguo
{
	lt9611_i2c_write_byte(0xff,0x83);
	lt9611_i2c_write_byte(0x0d,(CVI_U8)(video_format->vtotal/256));
	lt9611_i2c_write_byte(0x0e,(CVI_U8)(video_format->vtotal%256));//vtotal
	lt9611_i2c_write_byte(0x0f,(CVI_U8)(video_format->vact/256));
	lt9611_i2c_write_byte(0x10,(CVI_U8)(video_format->vact%256));  //vactive
	lt9611_i2c_write_byte(0x11,(CVI_U8)(video_format->htotal/256));
	lt9611_i2c_write_byte(0x12,(CVI_U8)(video_format->htotal%256));//htotal
	lt9611_i2c_write_byte(0x13,(CVI_U8)(video_format->hact/256));
	lt9611_i2c_write_byte(0x14,(CVI_U8)(video_format->hact%256)); //hactive
	lt9611_i2c_write_byte(0x15,(CVI_U8)(video_format->vs%256));   //vsa
	lt9611_i2c_write_byte(0x16,(CVI_U8)(video_format->hs%256));   //hsa
	lt9611_i2c_write_byte(0x17,(CVI_U8)(video_format->vfp%256));  //vfp
	lt9611_i2c_write_byte(0x18,(CVI_U8)((video_format->vs+video_format->vbp)%256));  //vss
	lt9611_i2c_write_byte(0x19,(CVI_U8)(video_format->hfp%256));  //hfp
	lt9611_i2c_write_byte(0x1a,(CVI_U8)(((video_format->hfp/256)<<4)+(video_format->hs+video_format->hbp)/256));   //20180901
	lt9611_i2c_write_byte(0x1b,(CVI_U8)((video_format->hs+video_format->hbp)%256));  //hss
}

void LT9611_MIPI_Pcr(struct video_timing *video_format) //weiguo
{
	CVI_U8 POL;
	CVI_U16 hact;
	hact = video_format->hact;
	POL = (video_format-> h_polarity)*0x02 + (video_format-> v_polarity);
	POL = ~POL;
	POL &= 0x03;

	lt9611_i2c_write_byte(0xff,0x83);

//    if(P10 == 0) //dual port.
	if(0) //dual port.
	{
		hact = (hact>>2);
		hact += 0x50;

		hact = (0x3e0>hact ? hact:0x3e0);

		lt9611_i2c_write_byte(0x0b,(CVI_U8)(hact >>8)); //vsync mode
		lt9611_i2c_write_byte(0x0c,(CVI_U8)hact); //=1/4 hact
		//hact -=0x40;
		lt9611_i2c_write_byte(0x48,(CVI_U8)(hact >>8)); //de mode delay
		lt9611_i2c_write_byte(0x49,(CVI_U8)(hact)); //
	}
	else
	{
		lt9611_i2c_write_byte(0x0b,0x01); //vsync read delay(reference value)
		lt9611_i2c_write_byte(0x0c,0x10); //

		lt9611_i2c_write_byte(0x48,0x00); //de mode delay
		lt9611_i2c_write_byte(0x49,0x81); //=1/4 hact
	}

	/* stage 1 */
	lt9611_i2c_write_byte(0x21,0x4a); //bit[3:0] step[11:8]
	//lt9611_i2c_write_byte(0x22,0x40);//step[7:0]

	lt9611_i2c_write_byte(0x24,0x71); //bit[7:4]v/h/de mode; line for clk stb[11:8]
	lt9611_i2c_write_byte(0x25,0x30); //line for clk stb[7:0]

	lt9611_i2c_write_byte(0x2a,0x01); //clk stable in

	/* stage 2 */
	lt9611_i2c_write_byte(0x4a,0x40); //offset //0x10
	lt9611_i2c_write_byte(0x1d,(0x10|POL)); //PCR de mode step setting.

	/* MK limit */

	switch(Video_Format)
	{
		case video_3840x1080_60Hz_vic:
		case video_3840x2160_30Hz_vic:
		case video_3840x2160_25Hz_vic:
		case video_3840x2160_24Hz_vic:
		case video_2560x1600_60Hz_vic:
 		case video_2560x1440_60Hz_vic:
 		case video_2560x1080_60Hz_vic:
		break;
		case video_1920x1080_60Hz_vic16:
		case video_1920x1080_30Hz_vic:
		case video_1280x720_60Hz_vic4:
		case video_1280x720_30Hz_vic:
		break;
		case video_720x480_60Hz_vic3:
		case video_640x480_60Hz_vic1:

			lt9611_i2c_write_byte(0xff,0x83);
			lt9611_i2c_write_byte(0x0b,0x02);
			lt9611_i2c_write_byte(0x0c,0x40);
			lt9611_i2c_write_byte(0x48,0x01);
			lt9611_i2c_write_byte(0x49,0x10);
			lt9611_i2c_write_byte(0x24,0x70);
			lt9611_i2c_write_byte(0x25,0x80);
			lt9611_i2c_write_byte(0x2a,0x10);
			lt9611_i2c_write_byte(0x2b,0x80);
			lt9611_i2c_write_byte(0x23,0x28);  //
			lt9611_i2c_write_byte(0x4a,0x10);
			lt9611_i2c_write_byte(0x1d,0xf3);  //

			CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_MIPI_Pcr: 640x480_60Hz\n");
		break;

		case video_540x960_60Hz_vic:
		case video_1024x600_60Hz_vic:
			lt9611_i2c_write_byte(0x24,0x70); //bit[7:4]v/h/de mode; line for clk stb[11:8]
			lt9611_i2c_write_byte(0x25,0x80); //line for clk stb[7:0]
			lt9611_i2c_write_byte(0x2a,0x10); //clk stable in

			/* stage 2 */
			//lt9611_i2c_write_byte(0x23,0x04); //pcr h mode step
			//lt9611_i2c_write_byte(0x4a,0x10); //offset //0x10
			lt9611_i2c_write_byte(0x1d,0xf0); //PCR de mode step setting.
		break;

		default: break;
	}
	LT9611_MIPI_Video_Timing(video);

	//lt9611_i2c_write_byte(0xff,0x83);
	//lt9611_i2c_write_byte(0x26,pcr_m);

	lt9611_i2c_write_byte(0xff,0x80);
	lt9611_i2c_write_byte(0x11,0x5a); //Pcr reset
	//lt9611_i2c_write_byte(0x11,0xfa);
	lt9611_i2c_write_byte(0x11,0xf8);
}

void LT9611_PLL(struct video_timing *video_format) //zhangzhichun
{
	CVI_U32 pclk;
	CVI_U8 pll_lock_flag, cal_done_flag, band_out;
	CVI_U8 hdmi_post_div;
	CVI_U8 i;
	pclk = video_format->pclk_khz;
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_PLL: set rx pll = %d", pclk);

	lt9611_i2c_write_byte(0xff,0x81);
	lt9611_i2c_write_byte(0x23,0x40); //Enable LDO and disable PD
	lt9611_i2c_write_byte(0x24,0x62); //0x62, LG25UM58 issue, 20180824
	//lt9611_i2c_write_byte(0x25,0x80); //pre-divider
	lt9611_i2c_write_byte(0x25,0xc2); //pre-divider
	lt9611_i2c_write_byte(0x26,0x55);
	lt9611_i2c_write_byte(0x2c,0x37);
	//lt9611_i2c_write_byte(0x2d,0x99); //txpll_divx_set&da_txpll_freq_set
	//lt9611_i2c_write_byte(0x2e,0x01);
	lt9611_i2c_write_byte(0x2f,0x01);
	//lt9611_i2c_write_byte(0x26,0x55);
	lt9611_i2c_write_byte(0x27,0x66);
	lt9611_i2c_write_byte(0x28,0x88);

	lt9611_i2c_write_byte(0x2a,0x20); //for U3.

	if(pclk > 150000)
	{
	lt9611_i2c_write_byte(0x2d,0x88);
		hdmi_post_div = 0x01;
	}
	else if(pclk > 70000)
	{
		lt9611_i2c_write_byte(0x2d,0x99);
		hdmi_post_div = 0x02;
	}
	else
	{
		lt9611_i2c_write_byte(0x2d,0xaa); //0xaa
		hdmi_post_div = 0x04;
	}

	//pcr_m = (CVI_U8)((pclk*5*hdmi_post_div)/27000);
	//pcr_m --;

	pcr_m = 20*hdmi_post_div;


	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_PLL: pcr_m = 0x%x, hdmi_post_div = %d", pcr_m, hdmi_post_div); //Hex

	lt9611_i2c_write_byte(0xff,0x83);
	lt9611_i2c_write_byte(0x2d,0x40); //M up limit
	lt9611_i2c_write_byte(0x31,0x08); //M down limit
	lt9611_i2c_write_byte(0x26,0x80|pcr_m); /* fixed M is to let pll locked*/

	pclk = pclk / 2;
	lt9611_i2c_write_byte(0xff,0x82);     //13.5M
	lt9611_i2c_write_byte(0xe3,pclk/65536);
	pclk = pclk % 65536;
	lt9611_i2c_write_byte(0xe4,pclk/256);
	lt9611_i2c_write_byte(0xe5,pclk%256);

	lt9611_i2c_write_byte(0xde,0x20); // pll cal en, start calibration
	lt9611_i2c_write_byte(0xde,0xe0);

	lt9611_i2c_write_byte(0xff,0x80);
	lt9611_i2c_write_byte(0x11,0x5a); /* Pcr clk reset */
	lt9611_i2c_write_byte(0x11,0xfa);
	lt9611_i2c_write_byte(0x16,0xf2); /* pll cal digital reset */
	lt9611_i2c_write_byte(0x18,0xdc); /* pll analog reset */
	lt9611_i2c_write_byte(0x18,0xfc);
	lt9611_i2c_write_byte(0x16,0xf3); /*start calibration*/

	/* pll lock status */
	for(i = 0; i < 6 ; i++)
	{
        lt9611_i2c_write_byte(0xff,0x80);
        lt9611_i2c_write_byte(0x16,0xe3); /* pll lock logic reset */
        lt9611_i2c_write_byte(0x16,0xf3);
        lt9611_i2c_write_byte(0xff,0x82);
        cal_done_flag = lt9611_i2c_read_byte(0xe7);
        band_out = lt9611_i2c_read_byte(0xe6);
        pll_lock_flag = lt9611_i2c_read_byte(0x15);

		if((pll_lock_flag & 0x80)&&(cal_done_flag & 0x80)&&(band_out != 0xff))
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_PLL: HDMI pll locked band out: 0x%x", band_out);
			break;
		}
		else
		{
            lt9611_i2c_write_byte(0xff,0x80);
            lt9611_i2c_write_byte(0x11,0x5a); /* Pcr clk reset */
            lt9611_i2c_write_byte(0x11,0xfa);
            lt9611_i2c_write_byte(0x16,0xf2); /* pll cal digital reset */
            lt9611_i2c_write_byte(0x18,0xdc); /* pll analog reset */
            lt9611_i2c_write_byte(0x18,0xfc);
            lt9611_i2c_write_byte(0x16,0xf3); /*start calibration*/
			CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_PLL: HDMI pll unlocked, reset pll\n");
		}
	}
}

void LT9611_HDMI_TX_Phy(void) //xyji
{
	lt9611_i2c_write_byte(0xff,0x81);
	lt9611_i2c_write_byte(0x30,0x6a);
	if(lt9611.hdmi_coupling_mode==ac_mode)
	{
		lt9611_i2c_write_byte(0x31,0x73); //DC: 0x44, AC:0x73
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_TX_Phy: AC couple\n");
  	}
	else //lt9611.hdmi_coupling_mode==dc_mode
	{
		lt9611_i2c_write_byte(0x31,0x44);
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_TX_Phy: DC couple\n");
	}
		lt9611_i2c_write_byte(0x32,0x4a);
		lt9611_i2c_write_byte(0x33,0x0b);
		lt9611_i2c_write_byte(0x34,0x00);
		lt9611_i2c_write_byte(0x35,0x00);
		lt9611_i2c_write_byte(0x36,0x00);
		lt9611_i2c_write_byte(0x37,0x44);
		lt9611_i2c_write_byte(0x3f,0x0f);
		lt9611_i2c_write_byte(0x40,0x98); //clk swing
		lt9611_i2c_write_byte(0x41,0x98); //D0 swing
		lt9611_i2c_write_byte(0x42,0x98); //D1 swing
		lt9611_i2c_write_byte(0x43,0x98); //D2 swing
		lt9611_i2c_write_byte(0x44,0x0a);
}

void LT9611_HDCP_Init(void) //luodexing
{
	lt9611_i2c_write_byte(0xff,0x85);
	lt9611_i2c_write_byte(0x07,0x1f);
	lt9611_i2c_write_byte(0x13,0xfe);// [7]=force_hpd, [6]=force_rsen, [5]=vsync_pol, [4]=hsync_pol,
	                              // [3]=hdmi_mode, [2]=no_accs_when_rdy, [1]=skip_wt_hdmi
	lt9611_i2c_write_byte(0x17,0x0f);// [7]=ri_short_read, [3]=sync_pol_mode, [2]=srm_chk_done,
	                              // [1]=bksv_srm_pass, [0]=ksv_list_vld
	lt9611_i2c_write_byte(0x15,0x05);
	//lt9611_i2c_write_byte(0x15,0x65);// [7]=key_ddc_st_sel, [6]=tx_hdcp_en,[5]=tx_auth_en, [4]=tx_re_auth
}

void LT9611_HDCP_Enable(void) //luodexing
{
	lt9611_i2c_write_byte(0xff,0x80);
	lt9611_i2c_write_byte(0x14,0x7f);
	lt9611_i2c_write_byte(0x14,0xff);
	lt9611_i2c_write_byte(0xff,0x85);
	lt9611_i2c_write_byte(0x15,0x01); //disable HDCP
	lt9611_i2c_write_byte(0x15,0x71); //enable HDCP
	lt9611_i2c_write_byte(0x15,0x65); //enable HDCP
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDCP_Enable: On\n");
}

void LT9611_HDCP_Disable(void) //luodexing
{
	lt9611_i2c_write_byte(0xff,0x85);
	lt9611_i2c_write_byte(0x15,0x45); //enable HDCP
}

void LT9611_HDMI_Out_Enable(void) //dsren
{
	lt9611_i2c_write_byte(0xff,0x81);
	lt9611_i2c_write_byte(0x23,0x40);

	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0xde,0x20);
	lt9611_i2c_write_byte(0xde,0xe0);

	lt9611_i2c_write_byte(0xff,0x80);
	lt9611_i2c_write_byte(0x18,0xdc); /* txpll sw rst */
	lt9611_i2c_write_byte(0x18,0xfc);
	lt9611_i2c_write_byte(0x16,0xf1); /* txpll calibration rest */
	lt9611_i2c_write_byte(0x16,0xf3);

	lt9611_i2c_write_byte(0x11,0x5a); //Pcr reset
	lt9611_i2c_write_byte(0x11,0xf8);

	lt9611_i2c_write_byte(0xff,0x81);
	lt9611_i2c_write_byte(0x30,0xea);

	if(lt9611.hdcp_encryption == hdcp_enable)
	{
		LT9611_HDCP_Enable();
	}
	else
	{
		LT9611_HDCP_Disable();
	}

	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_Out_Enable\n");
}

void LT9611_HDMI_Out_Disable(void) //dsren
{
	lt9611_i2c_write_byte(0xff,0x81);
	lt9611_i2c_write_byte(0x30,0x00); /* Txphy PD */
	lt9611_i2c_write_byte(0x23,0x80); /* Txpll PD */
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_Out_Disable\n");
	LT9611_HDCP_Disable();
}

void LT9611_HDMI_TX_Digital(struct video_timing *video_format) //dsren
{
	//bool hdmi_mode = lt9611->hdmi_mode;
	CVI_U8 VIC = video_format->vic;
	CVI_U8 AR = video_format->aspact_ratio;
	CVI_U8 pb0,pb2,pb4;
	CVI_U8 infoFrame_en;

	infoFrame_en = (AIF_PKT_EN|AVI_PKT_EN|SPD_PKT_EN);
	//MPEG_PKT_EN,AIF_PKT_EN,SPD_PKT_EN,AVI_PKT_EN,UD0_PKT_EN,UD1_PKT_EN
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_TX_Digital: infoFrame_en = 0x%x",infoFrame_en);

	pb2 =  (AR<<4) + 0x08;
	pb4 =  VIC;

	pb0 = ((pb2 + pb4) <= 0x5f)?(0x5f - pb2 - pb4):(0x15f - pb2 - pb4);

	lt9611_i2c_write_byte(0xff,0x82);
	if(lt9611.hdmi_mode == HDMI)
		{
			lt9611_i2c_write_byte(0xd6,0x8e); //sync polarity
			CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_TX_Digital: HMDI mode = HDMI\n");
		}
	else if(lt9611.hdmi_mode == DVI)
		{
			lt9611_i2c_write_byte(0xd6,0x0e); //sync polarity
			CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDMI_TX_Digital: HMDI mode = DVI\n");
		}

	if(lt9611.audio_out==audio_i2s)
	{
		lt9611_i2c_write_byte(0xd7,0x04);
	}

	if(lt9611.audio_out==audio_spdif)
	{
		lt9611_i2c_write_byte(0xd7,0x80);
	}

	//AVI
	lt9611_i2c_write_byte(0xff,0x84);
	lt9611_i2c_write_byte(0x43,pb0);   //AVI_PB0

	//lt9611_i2c_write_byte(0x44,0x10);//AVI_PB1
	lt9611_i2c_write_byte(0x45,pb2);  //AVI_PB2
	lt9611_i2c_write_byte(0x47,pb4);   //AVI_PB4

	lt9611_i2c_write_byte(0xff,0x84);
	lt9611_i2c_write_byte(0x10,0x02); //data iland
	lt9611_i2c_write_byte(0x12,0x64); //act_h_blank

	//VS_IF, 4k 30hz need send VS_IF packet.
	if(VIC == 95)
	{
		lt9611_i2c_write_byte(0xff,0x84);
		lt9611_i2c_write_byte(0x3d,infoFrame_en|UD0_PKT_EN); //UD1 infoframe enable //revise on 20200715

		lt9611_i2c_write_byte(0x74,0x81);  //HB0
		lt9611_i2c_write_byte(0x75,0x01);  //HB1
		lt9611_i2c_write_byte(0x76,0x05);  //HB2
		lt9611_i2c_write_byte(0x77,0x49);  //PB0
		lt9611_i2c_write_byte(0x78,0x03);  //PB1
		lt9611_i2c_write_byte(0x79,0x0c);  //PB2
		lt9611_i2c_write_byte(0x7a,0x00);  //PB3
		lt9611_i2c_write_byte(0x7b,0x20);  //PB4
		lt9611_i2c_write_byte(0x7c,0x01);  //PB5
	}
	else
	{
		lt9611_i2c_write_byte(0xff,0x84);
		lt9611_i2c_write_byte(0x3d,infoFrame_en); //UD1 infoframe enable
	}

	if(infoFrame_en&&SPD_PKT_EN)
	{
		lt9611_i2c_write_byte(0xff,0x84);
		lt9611_i2c_write_byte(0xc0,0x83);  //HB0
		lt9611_i2c_write_byte(0xc1,0x01);  //HB1
		lt9611_i2c_write_byte(0xc2,0x19);  //HB2

		lt9611_i2c_write_byte(0xc3,0x00);  //PB0
		lt9611_i2c_write_byte(0xc4,0x01);  //PB1
		lt9611_i2c_write_byte(0xc5,0x02);  //PB2
		lt9611_i2c_write_byte(0xc6,0x03);  //PB3
		lt9611_i2c_write_byte(0xc7,0x04);  //PB4
		lt9611_i2c_write_byte(0xc8,0x00);  //PB5
	}
}

void LT9611_CSC(void)
{
   	if(lt9611.input_color_space == YUV422)
	{
	   lt9611_i2c_write_byte(0xff,0x82);
	   lt9611_i2c_write_byte(0xb9,0x18);
	   CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_CSC: Ypbpr 422 to RGB888\n");
	}
}


void LT9611_Audio_Init(void) //sujin
{
#if 1
	if(lt9611.audio_out==audio_i2s)
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "Audio inut = I2S 2ch\n");

		lt9611_i2c_write_byte(0xff,0x84);
		lt9611_i2c_write_byte(0x06,0x08);
		lt9611_i2c_write_byte(0x07,0x10);

		//48K sampling frequency
		lt9611_i2c_write_byte(0x0f,0x2b); //0x2b: 48K, 0xab:96K
		lt9611_i2c_write_byte(0x34,0xd4); //CTS_N 20180823 0xd5: sclk = 32fs, 0xd4: sclk = 64fs

		lt9611_i2c_write_byte(0x35,0x00); // N value = 6144
		lt9611_i2c_write_byte(0x36,0x18);
		lt9611_i2c_write_byte(0x37,0x00);
	}

	if(lt9611.audio_out==audio_spdif)
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "Audio inut = SPDIF\n");

		lt9611_i2c_write_byte(0xff,0x84);
		lt9611_i2c_write_byte(0x06,0x0c);
		lt9611_i2c_write_byte(0x07,0x10);

		lt9611_i2c_write_byte(0x34,0xd4); //CTS_N
	}
#endif
}


void LT9611_Read_EDID(void) //luodexing
{
#ifdef _enable_read_edid_
	CVI_U8 i,j, edid_data;
	CVI_U8 extended_flag = 00;

	memset(Sink_EDID,0,sizeof Sink_EDID);

	lt9611_i2c_write_byte(0xff,0x85);
	//lt9611_i2c_write_byte(0x02,0x0a); //I2C 100K
	lt9611_i2c_write_byte(0x03,0xc9);
	lt9611_i2c_write_byte(0x04,0xa0); //0xA0 is EDID device address
	lt9611_i2c_write_byte(0x05,0x00); //0x00 is EDID offset address
	lt9611_i2c_write_byte(0x06,0x20); //length for read
	lt9611_i2c_write_byte(0x14,0x7f);

	for(i=0;i < 8;i++) // block 0 & 1
	{
		lt9611_i2c_write_byte(0x05,i*32); //0x00 is EDID offset address
		lt9611_i2c_write_byte(0x07,0x36);
		lt9611_i2c_write_byte(0x07,0x34); //0x31
		lt9611_i2c_write_byte(0x07,0x37); //0x37
		Timer0_Delay1ms(5); // wait 5ms for reading edid data.
		if(lt9611_i2c_read_byte(0x40)&0x02) //KEY_DDC_ACCS_DONE=1
		{
			if(lt9611_i2c_read_byte(0x40)&0x50)//DDC No Ack or Abitration lost
			{
				CVI_TRACE_LOG(CVI_DBG_INFO, "read edid failed: no ack\n");
				goto end;
			}
			else
			{
				for(j=0; j<32; j++)
					{
						edid_data = lt9611_i2c_read_byte(0x83);
						Sink_EDID[i*32+j]= edid_data; // write edid data to Sink_EDID[];
						if((i == 3)&&( j == 30))
						{
							extended_flag = edid_data & 0x03;
						}
						CVI_TRACE_LOG(CVI_DBG_INFO, "%x,",edid_data);
					}
				if(i == 3)
				{
					if(extended_flag < 1) //no block 1, stop reading edid.
					{
						goto end;
					}
				}
			}
		}
		else
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, "read edid failed: accs not done\n");
			goto end;
		}
	}

	if(extended_flag < 2) //no block 2, stop reading edid.
	{
		goto end;
	}

	for(i=0;i< 8;i++) //  // block 2 & 3
	{
		lt9611_i2c_write_byte(0x05,i*32); //0x00 is EDID offset address
		lt9611_i2c_write_byte(0x07,0x76); //0x31
		lt9611_i2c_write_byte(0x07,0x74); //0x31
		lt9611_i2c_write_byte(0x07,0x77); //0x37
		Timer0_Delay1ms(5); // wait 5ms for reading edid data.
		if(lt9611_i2c_read_byte(0x40)&0x02) //KEY_DDC_ACCS_DONE=1
		{
			if(lt9611_i2c_read_byte(0x40)&0x50)//DDC No Ack or Abitration lost
			{
				CVI_TRACE_LOG(CVI_DBG_INFO, "read edid failed: no ack\n");
				goto end;
			}
			else
			{
				for(j=0; j<32; j++)
					{
						edid_data = lt9611_i2c_read_byte(0x83);
						//Sink_EDID[256+i*32+j]= edid_data; // write edid data to Sink_EDID[];
						CVI_TRACE_LOG(CVI_DBG_INFO, "%x,",edid_data);
					}
				if(i == 3)
				{
					if(extended_flag < 3) //no block 1, stop reading edid.
					{
						goto end;
					}
				}
			}
		}
		else
		{
			CVI_TRACE_LOG(CVI_DBG_INFO, "read edid failed: accs not done\n");
			goto end;
		}
	}
end:
	lt9611_i2c_write_byte(0x03,0xc2);
	lt9611_i2c_write_byte(0x07,0x1f);
#endif
}

void LT9611_load_hdcp_key(void) //luodexing
{
	lt9611_i2c_write_byte(0xff,0x85);
	lt9611_i2c_write_byte(0x00,0x85);
	//lt9611_i2c_write_byte(0x02,0x0a); //I2C 100K
	lt9611_i2c_write_byte(0x03,0xc0);
	lt9611_i2c_write_byte(0x03,0xc3);
	lt9611_i2c_write_byte(0x04,0xa0); //0xA0 is eeprom device address
	lt9611_i2c_write_byte(0x05,0x00); //0x00 is eeprom offset address
	lt9611_i2c_write_byte(0x06,0x20); //length for read
	lt9611_i2c_write_byte(0x14,0xff);

	lt9611_i2c_write_byte(0x07,0x11); //0x31
	lt9611_i2c_write_byte(0x07,0x17); //0x37
	Timer0_Delay1ms(50); // wait 5ms for loading key.

	//CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_load_hdcp_key: 0x%02bx",lt9611_i2c_read_byte(0x40));

	if((lt9611_i2c_read_byte(0x40)&0x81) == 0x81)
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_load_hdcp_key: external key valid\n");
	}
	else
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_load_hdcp_key: external key unvalid, using internal test key!\n");
	}

	lt9611_i2c_write_byte(0x03,0xc2);
	lt9611_i2c_write_byte(0x07,0x1f);
}

bool LT9611_Get_HPD_Status(void)
{
	CVI_U8 reg_825e;
	lt9611_i2c_write_byte(0xff,0x82);
	reg_825e = lt9611_i2c_read_byte(0x5e);

	if((reg_825e&0x04) == 0x04) //20200727
	{
		//Tx_HPD=1;
		return 1;
	}
	else
	{
		//Tx_HPD=0;
		return 0;
	}
	}


//CEC: start
void LT9611_HDMI_CEC_ON(bool enable)
{
  if(enable){
	/* cec init */
		lt9611_i2c_write_byte(0xff, 0x80);
		lt9611_i2c_write_byte(0x0d, 0xff);
		lt9611_i2c_write_byte(0x15, 0xf1); //reset cec logic
		lt9611_i2c_write_byte(0x15, 0xf9);
		lt9611_i2c_write_byte(0xff, 0x86);
		lt9611_i2c_write_byte(0xfe, 0xa5); //clk div
	}
	else{
		lt9611_i2c_write_byte(0xff, 0x80);
		lt9611_i2c_write_byte(0x15, 0xf1);
	}
}

void lt9611_cec_logical_reset(void)
{
    lt9611_i2c_write_byte(0xff, 0x80);
    lt9611_i2c_write_byte(0x15, 0xf1); //reset cec logic
	lt9611_i2c_write_byte(0x15, 0xf9);
}


void lt9611_cec_msg_set_logical_address(struct cec_msg *cec_msg)
{
    CVI_U8 logical_address;
/*
    0xf8, 0xf7   //Register
    0x00, 0x01,  //LA 0
    0x00, 0x02,  //LA 1
    0x00, 0x03,  //LA 2
    0x00, 0x04,  //LA 3
    0x00, 0x10,  //LA 4
    0x00, 0x20,  //LA 5
    0x00, 0x30,  //LA 6
    0x00, 0x40,  //LA 7
    0x01, 0x00,  //LA 8
    0x02, 0x00,  //LA 9
    0x03, 0x00,  //LA 10
    0x04, 0x00,  //LA 11
    0x10, 0x00,  //LA 12
    0x20, 0x00,  //LA 13
    0x30, 0x00,  //LA 14
    0x40, 0x00	 //LA 15
*/

	if(!cec_msg->la_allocation_done)
	{
		logical_address = 15;
	}
	else
	{
		logical_address = cec_msg->logical_address;
	}

    if(logical_address > 15)
    {
        CVI_TRACE_LOG(CVI_DBG_ERR, " LA error!\n");
        return;
    }

    lt9611_i2c_write_byte(0xff, 0x86);

    switch(logical_address) {
    case 0: 
    lt9611_i2c_write_byte(0xf7, 0x01);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 1:
    lt9611_i2c_write_byte(0xf7, 0x02);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 2:
    lt9611_i2c_write_byte(0xf7, 0x03);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 3:
    lt9611_i2c_write_byte(0xf7, 0x04);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 4:
    lt9611_i2c_write_byte(0xf7, 0x10);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 5:
    lt9611_i2c_write_byte(0xf7, 0x20);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 6:
    lt9611_i2c_write_byte(0xf7, 0x30);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 7:
    lt9611_i2c_write_byte(0xf7, 0x40);
    lt9611_i2c_write_byte(0xf8, 0x00);
    break;

    case 8:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x01);
    break;

    case 9:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x02);
    break;

    case 10:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x03);
    break;

    case 11:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x04);
    break;

    case 12:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x10);
    break;

    case 13:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x20);
    break;

    case 14:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x30);
    break;

    case 15:
    lt9611_i2c_write_byte(0xf7, 0x00);
    lt9611_i2c_write_byte(0xf8, 0x40);
    break;

    default: break;
    }

}


static int do_checksum(const unsigned char *x, CVI_U8 len)
{
	unsigned char check = x[len];
	unsigned char sum = 0;
	int i;

	CVI_TRACE_LOG(CVI_DBG_INFO, "Checksum: 0x%x", check);

	for (i = 0; i < len; i++)
		sum += x[i];

	if ((unsigned char)(check + sum) != 0) {
		CVI_TRACE_LOG(CVI_DBG_INFO, " (should be 0x%x)\n", -sum & 0xff);
		return 0;
	}

	CVI_TRACE_LOG(CVI_DBG_INFO, " (valid)\n");
	return 1;
}


int lt9611_parse_physical_address(struct cec_msg *cec_msg, CVI_U8 *edid) // parse edid data from edid.
{
//    int ret = 0;
	int version;
	int offset = 0;
    int offset_d = 0;
    int tag_code;
    CVI_U16 physical_address;

    version = edid[0x81];
    offset_d = edid[0x82];

    if(!do_checksum(edid, 255))
	{
		return 0; //prase_physical_address fail.
	}

	if (version < 3)
	{
		return 0; //prase_physical_address fail.
	}

    if (offset_d < 5)
	{
		return 0; //prase_physical_address fail.
	}

    tag_code = (edid[0x84 + offset] & 0xe0)>>5;

    while(tag_code != 0x03){
        if((edid[0x84 + offset]&0x1f) == 0 )
            return 0;
        offset += edid[0x84 + offset]&0x1f;
        offset++;

        if(offset > (offset_d - 4))
            return 0;

        tag_code = (edid[0x84 + offset] & 0xe0)>>5;

        }

    CVI_TRACE_LOG(CVI_DBG_INFO, "vsdb: 0x%x,0x%x,0x%x", edid[0x84 + offset],edid[0x85 + offset],edid[0x86 + offset]);

    if((edid[0x84 + offset + 1] == 0x03)&&
        (edid[0x84 + offset + 2] == 0x0c)&&
        (edid[0x84 + offset + 3] == 0x00))
        {
            physical_address = edid[0x84 + offset + 4];
            physical_address = (physical_address<<8) + edid[0x84 + offset + 5];
            cec_msg->physical_address = physical_address;
            CVI_TRACE_LOG(CVI_DBG_INFO, "prase physical address success! %x",physical_address);
            return 1;
        }
	return 0;
}

void lt9611_hdmi_cec_read(struct cec_msg *cec_msg) // transfer cec msg from LT9611 regisrer to rx_buffer.
{
    CVI_U8 size, i;
    lt9611_i2c_write_byte(0xff, 0x86);
    lt9611_i2c_write_byte(0xf5, 0x01); //lock rx data buff
    size = lt9611_i2c_read_byte(0xd3);
    cec_msg->rx_data_buff[0] = size;
    //CVI_TRACE_LOG(CVI_DBG_INFO, "cec rec: \n");
    for(i = 1;i<= size; i++)
    {
        cec_msg->rx_data_buff[i] = lt9611_i2c_read_byte(0xd3 + i);
        //CVI_TRACE_LOG(CVI_DBG_INFO, "0x%02bx, ",cec_msg->rx_data_buff[i]);
    }
    lt9611_i2c_write_byte(0xf5, 0x00); //unlock rx data buff
}


void lt9611_hdmi_cec_write(struct cec_msg *cec_msg)// send cec msg which is in tx_buffer.
{
    CVI_U8 size, i;
    size = cec_msg->tx_data_buff[0];
    cec_msg->retries_times = 0;

    lt9611_i2c_write_byte(0xff, 0x86);
    lt9611_i2c_write_byte(0xf5, 0x01); //lock rx data buff
    lt9611_i2c_write_byte(0xf4, size);
    for(i = 0;i<= size; i++)
    {
        lt9611_i2c_write_byte(0xe4+i, cec_msg->tx_data_buff[1+i]);
    }
	lt9611_i2c_write_byte(0xf9, 0x03); //start send msg
	Timer0_Delay1ms(25*i); //wait HDMI
    lt9611_i2c_write_byte(0xf5, 0x00); //unlock rx data buff
}



void lt9611_cec_msg_write_demo(void) //for debug
{
      CEC_TxData_Buff[0] = 0x05; //data counter to be send
      CEC_TxData_Buff[1] = 0x40; //first cec data(in spec, sender id = 0x05
      							 //receiver id = 0x00
      CEC_TxData_Buff[2] = 0x84; //second cec data(in spec, it is opcode =0x84)
      CEC_TxData_Buff[3] = 0x10; //parameter of opcode
      CEC_TxData_Buff[4] = 0x00; //parameter of opcode
      CEC_TxData_Buff[5] = 0x05; //parameter of opcode

      //lt9611_hdmi_cec_write(CEC_TxData_Buff);
}

void lt9611_broad_cast_demo(void) // for debug
{
      CEC_TxData_Buff[0] = 0x05; //data counter to be send
      CEC_TxData_Buff[1] = 0x40; //first cec data(in spec, sender id = 0x05,
      							 //receiver id = 0x00
      CEC_TxData_Buff[2] = 0x84; //second cec data(in spec, it is opcode =0x84)
      CEC_TxData_Buff[3] = 0x10; //parameter of opcode
      CEC_TxData_Buff[4] = 0x00; //parameter of opcode
      CEC_TxData_Buff[5] = 0x05; //parameter of opcode

      //lt9611_hdmi_cec_write(CEC_TxData_Buff);
}


void lt9611_cec_la_allocation(struct cec_msg *cec_msg) //polling  logical address.
{
    CVI_U8 logical_address;

    logical_address = cec_msg->logical_address;
    cec_msg->tx_data_buff[0] = 0x01; //data counter to be send
    cec_msg->tx_data_buff[1] = (logical_address<<4)|logical_address;
                                     //first cec data(in spec, sender id = 0x04,
      							     //receiver id = 0x04;
    lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_report_physical_address(struct cec_msg *cec_msg) // report physical address.
{
	cec_msg->tx_data_buff[0] = 0x05; //data counter to be send
	cec_msg->tx_data_buff[1] = (cec_msg->logical_address<<4)|0x0f;
								//first cec data([7:4]=initiator ;[7:4]= destintion)
	cec_msg->tx_data_buff[2] = 0x84; //opcode
	cec_msg->tx_data_buff[3] = (CVI_U8)(cec_msg->physical_address>>8); //parameter of opcode
	cec_msg->tx_data_buff[4] = (CVI_U8)(cec_msg->physical_address); //parameter of opcode
	cec_msg->tx_data_buff[5] = 0x04; //device type = playback device

	//CVI_TRACE_LOG(CVI_DBG_INFO, "PA:%bx, %bx",cec_msg->tx_data_buff[3],cec_msg->tx_data_buff[4]);
	lt9611_hdmi_cec_write(cec_msg);
}



void lt9611_cec_menu_activate(struct cec_msg *cec_msg) // report physical address.
{
        cec_msg->tx_data_buff[0] = 0x04; //data counter to be send
        cec_msg->tx_data_buff[1] = (cec_msg->logical_address<<4)|cec_msg->destintion;
                                    //first cec data([7:4]=initiator ;[7:4]= destintion)
        cec_msg->tx_data_buff[2] = 0x8e; //opcode
        cec_msg->tx_data_buff[3] = 0x00; //parameter of opcode
        //CVI_TRACE_LOG(CVI_DBG_INFO, "PA:%bx, %bx",cec_msg->tx_data_buff[3],cec_msg->tx_data_buff[4]);
        lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_feature_abort(struct cec_msg *cec_msg, CVI_U8 reason)// report feature abort 
{
        cec_msg->tx_data_buff[0] = 0x03; //data counter to be send
        cec_msg->tx_data_buff[1] = (cec_msg->logical_address<<4)|cec_msg->destintion;
                                         //first cec data([7:4]=initiator ;[7:4]= destintion)
        cec_msg->tx_data_buff[2] = 0x00; //opcode
        cec_msg->tx_data_buff[3] = reason; //parameter1 of opcode

        lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_frame_retransmission(struct cec_msg *cec_msg)
{
    if(cec_msg->retries_times < 5){
        lt9611_i2c_write_byte(0xff, 0x86);
        lt9611_i2c_write_byte(0xf9, 0x02);
        lt9611_i2c_write_byte(0xf9, 0x03);   //start send msg
    }
    cec_msg->retries_times ++;
}


void lt9611_cec_device_polling(struct cec_msg *cec_msg)
{
    static CVI_U8 i;
    if(!cec_msg->la_allocation_done)
        {
            cec_msg->tx_data_buff[0] = 0x01; //data counter to be send
            cec_msg->tx_data_buff[1] = i;    //first cec data(in spec, sender id = 0x04,
                  					         //receiver id = 0x04;
            lt9611_hdmi_cec_write(cec_msg);
            if(i >13 )
            cec_msg->la_allocation_done = 1;
            (i>13)?(i=0):(i++);
        }
}

void lt9611_cec_msg_tx_handle(struct cec_msg *cec_msg)
{
    CVI_U8 cec_status;
    CVI_U8 header;
    CVI_U8 opcode;
    CVI_U8 i;
    cec_status = cec_msg ->cec_status;

//    if( cec_msg ->send_msg_done) //There is no tx msg to be handled
//        return;

    if(cec_status&CEC_ERROR_INITIATOR){
        CVI_TRACE_LOG(CVI_DBG_INFO, "CEC_ERROR_INITIATOR.\n");
        lt9611_cec_logical_reset();
        return;
        }

    if(cec_status&CEC_ARB_LOST){
        CVI_TRACE_LOG(CVI_DBG_INFO, "CEC_ARB_LOST.\n"); //lost arbitration
        return;
        }

    if(cec_status&(CEC_SEND_DONE|CEC_NACK|CEC_ERROR_FOLLOWER)) do {

        CVI_TRACE_LOG(CVI_DBG_INFO, "tx_date: \n");
        for(i = 0; i < cec_msg->tx_data_buff[0]; i++)
            CVI_TRACE_LOG(CVI_DBG_INFO, "0x%x, ",cec_msg->tx_data_buff[i + 1]);


        if(cec_status&CEC_SEND_DONE)
            CVI_TRACE_LOG(CVI_DBG_INFO, "CEC_SEND_DONE >>\n");
        if(cec_status&CEC_NACK)
            CVI_TRACE_LOG(CVI_DBG_INFO, "NACK >>\n");

        header = cec_msg ->tx_data_buff[1];

        if((header == 0x44)||(header == 0x88)||(header == 0xbb)) //logical address allocation
        {
            if(cec_status&CEC_NACK){
            cec_msg ->logical_address = header&0x0f;
            CVI_TRACE_LOG(CVI_DBG_INFO, "la_allocation_done.\n");
            lt9611_cec_msg_set_logical_address(cec_msg);
            lt9611_cec_report_physical_address(cec_msg);
            }

            if(cec_status&CEC_SEND_DONE){

                if(cec_msg->logical_address == 4)// go to next la.
                    cec_msg->logical_address = 8;
                else if(cec_msg->logical_address == 8)// go to next la.
                    cec_msg->logical_address = 11;
                else if(cec_msg->logical_address == 11) // go to next la.
                    cec_msg->logical_address = 4;

                lt9611_cec_la_allocation(cec_msg);
            }

            break;
        }

        if(cec_status&(CEC_NACK|CEC_ERROR_FOLLOWER)){
            lt9611_cec_frame_retransmission(cec_msg);
        }

        if(cec_msg ->tx_data_buff[0] < 2)  //check tx data length
            break;

        opcode = cec_msg ->tx_data_buff[2];
        if(opcode == 0x84){
              cec_msg ->report_physical_address_done = 1;
              CVI_TRACE_LOG(CVI_DBG_INFO, "report_physical_address.\n");
        }

        if(opcode == 0x00){
              CVI_TRACE_LOG(CVI_DBG_INFO, "feature abort\n");
        }

        } while (0);
}


void lt9611_cec_msg_rx_parse(struct cec_msg *cec_msg)
{
    CVI_U8 cec_status;
    CVI_U8 header;
    CVI_U8 opcode;
    CVI_U8 initiator;
    CVI_U8 destintion;
    CVI_U8 i;
    cec_status = cec_msg ->cec_status;

//    if( cec_msg ->parse_msg_done) //There is no Rx msg to be parsed
//        return;

    if(cec_status&CEC_ERROR_FOLLOWER){
        CVI_TRACE_LOG(CVI_DBG_INFO, "CEC_ERROR_FOLLOWER.\n");
        return;
        }

    if(!(cec_status&CEC_REC_DATA)){
        return;
        }

    lt9611_hdmi_cec_read(&lt9611_cec_msg);

    if(cec_msg ->rx_data_buff[0] < 1) //check rx data length
        return;

    CVI_TRACE_LOG(CVI_DBG_INFO, "rx_date: \n");
    for(i = 0; i < cec_msg->rx_data_buff[0]; i++)
    CVI_TRACE_LOG(CVI_DBG_INFO, "0x%x, ",cec_msg->rx_data_buff[i + 1]);

    CVI_TRACE_LOG(CVI_DBG_INFO, "parse <<\n");
    header = cec_msg ->rx_data_buff[1];
    destintion = header&0x0f;
    initiator = (header&0xf0) >> 4;
    //cec_msg ->parse_msg_done = 1;

    if(header == 0x4f){
       CVI_TRACE_LOG(CVI_DBG_INFO, "lt9611 broadcast msg.\n");
        }

    if(cec_msg ->rx_data_buff[0] < 2) //check rx data length
        return;

    opcode = cec_msg ->rx_data_buff[2];

// CECT 12 Invalid Msg Tests
if((header&0x0f) == 0x0f){
    if((opcode == 0x00)||
        (opcode == 0x83)||
        (opcode == 0x8e)||
        (opcode == 0x90)||
        (opcode == 0xff)){
        CVI_TRACE_LOG(CVI_DBG_INFO, "Invalid msg, destination address error\n"); //these msg should not be broadcast msg, but they do.
        return;
    }
}else{
    if((opcode == 0x84)||
        (opcode == 0x84)||
        (opcode == 0x84)){
        CVI_TRACE_LOG(CVI_DBG_INFO, "Invalid msg, destination address error\n"); //these msg should be broadcast msg, but they not.
        return;
    }
}

    if(opcode == 0xff) //abort
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "abort.\n");
        if(destintion == 0x0f) //ignor broadcast abort msg.
            return;
        cec_msg ->destintion = initiator;
        lt9611_cec_feature_abort(cec_msg, CEC_ABORT_REASON_0);
    }

    if(opcode == 0x83) //give physical address
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "give physical address.\n");
        lt9611_cec_report_physical_address(cec_msg);
    }

    if(opcode == 0x90) //report power status
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "report power status.\n");
        if(cec_msg ->rx_data_buff[0] < 3) {
            CVI_TRACE_LOG(CVI_DBG_INFO, "<error:parameters missing\n");
            return; //parameters missing, ignor this msg.
        }
    }

    if(opcode == 0x8e) //menu status
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "menu status.\n");
        if(cec_msg ->rx_data_buff[0] < 3) {
            CVI_TRACE_LOG(CVI_DBG_INFO, "<error:parameters missing\n");
            return; //parameters missing, ignor this msg.
        }
    }

    if(opcode == 0x00) //feature abort
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "feature abort.\n");
        if(cec_msg ->rx_data_buff[0] < 3) {
            CVI_TRACE_LOG(CVI_DBG_INFO, "<error:parameters missing\n");
            return; //parameters missing, ignor this msg.
        }
    }

    if(opcode == 0x9e) //cec version
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "cec version.\n");
        if(cec_msg ->rx_data_buff[0] < 3) {
            CVI_TRACE_LOG(CVI_DBG_INFO, "<error:parameters missing\n");
            return; //parameters missing, ignor this msg.
        }
    }

    if(opcode == 0x84) //report physical address
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "report physical address.\n");
        if(cec_msg ->rx_data_buff[0] < 5) {
            CVI_TRACE_LOG(CVI_DBG_INFO, "<error:parameters missing\n");
            return; //parameters missing, ignor this msg.
        }
    }

    if(opcode == 0x86) //set stream path
    {
        CVI_TRACE_LOG(CVI_DBG_INFO, "set stream path.\n");
        if(cec_msg ->rx_data_buff[0] < 4) {
            CVI_TRACE_LOG(CVI_DBG_INFO, "<error:parameters missing\n");
            return; //parameters missing, ignor this msg.
        }
        lt9611_cec_report_physical_address(cec_msg);
        Timer0_Delay1ms(120);
        lt9611_cec_menu_activate(cec_msg);
    }
}

void lt9611_cec_msg_init(struct cec_msg *cec_msg)
{
    LT9611_HDMI_CEC_ON(1);
    cec_msg->physical_address = 0x2000;
    cec_msg->logical_address = 4;
    cec_msg->report_physical_address_done = 0;
    cec_msg->la_allocation_done = 0;
    lt9611_cec_msg_set_logical_address(cec_msg);
}


//CEC: end

/////////////////////////////////////////////////////////////
//These function for debug: start
/////////////////////////////////////////////////////////////
void LT9611_Frequency_Meter_Byte_Clk(void)
{
	CVI_U8 temp;
	CVI_U32 reg=0x00;

    CVI_TRACE_LOG(CVI_DBG_INFO, "\33[32m\n");
	/* port A byte clk meter */
	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0xc7,0x03); //PortA
	Timer0_Delay1ms(50);
	temp = lt9611_i2c_read_byte(0xcd);
	if((temp&0x60)==0x60) /* clk stable */
	{
		reg = (CVI_U32)(temp&0x0f)*65536;
		temp = lt9611_i2c_read_byte(0xce);
		reg = reg + (CVI_U16)temp*256;
		temp = lt9611_i2c_read_byte(0xcf);
		reg = reg + temp;
		CVI_TRACE_LOG(CVI_DBG_INFO, "port A byte clk = %d",reg);
	}
	else /* clk unstable */
	  	CVI_TRACE_LOG(CVI_DBG_INFO, "port A byte clk unstable\n");

	/* port A reference clock meter */
	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0xc7,0x0a);
	Timer0_Delay1ms(50);
	temp = lt9611_i2c_read_byte(0xcd);
	if((temp&0x60)==0x60) /* clk stable */
	{
		reg = (CVI_U32)(temp&0x0f)*65536;
		temp = lt9611_i2c_read_byte(0xce);
		reg =reg + (CVI_U16)temp*256;
		temp = lt9611_i2c_read_byte(0xcf);
		reg = reg + temp;
		CVI_TRACE_LOG(CVI_DBG_INFO, "port A reference clock = %d",reg);
	}
	else /* clk unstable */
	       CVI_TRACE_LOG(CVI_DBG_INFO, "port A reference clock unstable\n");
}

void LT9611_Htotal_Sysclk(void)
{
#ifdef _htotal_stable_check_
	CVI_U16 reg;
	CVI_U8 loopx;
	for(loopx = 0; loopx < 10; loopx++)
	{
		lt9611_i2c_write_byte(0xff,0x82);
		reg = lt9611_i2c_read_byte(0x86);
		reg = reg*256+lt9611_i2c_read_byte(0x87);
		CVI_TRACE_LOG(CVI_DBG_INFO, "Htotal_Sysclk = %d", reg);
		//printdec_u32(reg);
	}
#endif
}
void LT9611_Pcr_MK_Print(void)
{
	#ifdef _pcr_mk_printf_
	CVI_U8 loopx;
	for(loopx = 0; loopx < 8; loopx++)
	{
		lt9611_i2c_write_byte(0xff,0x83);
		CVI_TRACE_LOG(CVI_DBG_INFO, "M:0x%x",lt9611_i2c_read_byte(0x97));
		CVI_TRACE_LOG(CVI_DBG_INFO, " 0x%x",lt9611_i2c_read_byte(0xb4));
		CVI_TRACE_LOG(CVI_DBG_INFO, " 0x%x",lt9611_i2c_read_byte(0xb5));
		CVI_TRACE_LOG(CVI_DBG_INFO, " 0x%x",lt9611_i2c_read_byte(0xb6));
		CVI_TRACE_LOG(CVI_DBG_INFO, " 0x%x",lt9611_i2c_read_byte(0xb7));
		Timer0_Delay1ms(1000);
	}
	#endif
}

void LT9611_Dphy_debug(void)
{
	#ifdef _mipi_Dphy_debug_
	CVI_U8 temp;

	lt9611_i2c_write_byte(0xff,0x83);
	temp = lt9611_i2c_read_byte(0xbc);
	if(temp == 0x55)
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane PN is right\n");
	else
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane PN error 0x83bc = 0x%x",temp);

	temp = lt9611_i2c_read_byte(0x99);
	if(temp == 0xb8)
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 0 sot right \n");
	else
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 0 sot error = 0x%x",temp);

	temp = lt9611_i2c_read_byte(0x9b);
	if(temp == 0xb8)
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 1 sot right \n");
	else
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 1 sot error = 0x%x",temp);

	temp = lt9611_i2c_read_byte(0x9d);
	if(temp == 0xb8)
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 2 sot right \n");
	else
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 2 sot error = 0x%x",temp);

	temp = lt9611_i2c_read_byte(0x9f);
	if(temp == 0xb8)
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 3 sot right \n");
	else
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 3 sot error = 0x%x",temp);

	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 0 settle = 0x%x",lt9611_i2c_read_byte(0x98));
	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 1 settle = 0x%x",lt9611_i2c_read_byte(0x9a));
 	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 2 settle = 0x%x",lt9611_i2c_read_byte(0x9c));
 	CVI_TRACE_LOG(CVI_DBG_INFO, "port A lane 3 settle = 0x%x",lt9611_i2c_read_byte(0x9e));

	#endif
}

/////////////////////////////////////////////////////////////
//These function for debug: end
/////////////////////////////////////////////////////////////


void LT9611_IRQ_Init(void) //dsren
{
	//int hpd interrupt
	lt9611_i2c_write_byte(0xff,0x82);
	//lt9611_i2c_write_byte(0x10,0x00); //Output low level active;
	lt9611_i2c_write_byte(0x58,0x0a); //Det HPD 0x0a --> 0x08 20200727
	lt9611_i2c_write_byte(0x59,0x80); //HPD debounce width

	//intial vid change interrupt
	lt9611_i2c_write_byte(0x9e,0xf7);
}

void LT9611_Globe_Interrupts(bool on)
{
if(on)
{
    lt9611_i2c_write_byte(0xff,0x81);
    lt9611_i2c_write_byte(0x51,0x10); //hardware mode irq pin out
}
else
{
    lt9611_i2c_write_byte(0xff,0x81); //software mode irq pin out = 1;
    lt9611_i2c_write_byte(0x51,0x30);
}

}

void LT9611_Enable_Interrupts(CVI_U8 interrupts, bool on)
{
     if(interrupts == HPD_INTERRUPT_ENABLE)
     	{
        if(on)
        {
		       lt9611_i2c_write_byte(0xff,0x82);
				   lt9611_i2c_write_byte(0x07,0xff); //clear3
				   lt9611_i2c_write_byte(0x07,0x3f); //clear3
				   lt9611_i2c_write_byte(0x03,0x3f); //mask3  //Tx_det
				   CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Enable_Interrupts: hpd_irq_enable\n");
        }
	     else
	     	{
		       lt9611_i2c_write_byte(0xff,0x82);
				   lt9611_i2c_write_byte(0x07,0xff); //clear3
				   lt9611_i2c_write_byte(0x03,0xff); //mask3  //Tx_det
				   CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Enable_Interrupts: hpd_irq_disable\n");
	     	}
     	}

	 if(interrupts == VID_CHG_INTERRUPT_ENABLE)
     	{
         if(on)
           	{
		        lt9611_i2c_write_byte(0xff,0x82);
				lt9611_i2c_write_byte(0x9e,0xff); //clear vid chk irq
	   			lt9611_i2c_write_byte(0x9e,0xf7);
				lt9611_i2c_write_byte(0x04,0xff); //clear0
				lt9611_i2c_write_byte(0x04,0xfe); //clear0
				lt9611_i2c_write_byte(0x00,0xfe); //mask0 vid_chk_IRQ
				CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Enable_Interrupts: vid_chg_irq_enable\n");
           	}
	     else
	     	{
		        lt9611_i2c_write_byte(0xff,0x82);
				lt9611_i2c_write_byte(0x04,0xff); //clear0
				lt9611_i2c_write_byte(0x00,0xff); //mask0 vid_chk_IRQ
				CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Enable_Interrupts: vid_chg_irq_disable\n");
	     	}
     	}
	 if(interrupts == CEC_INTERRUPT_ENABLE)
	 	{
			if(on)
				{
					lt9611_i2c_write_byte(0xff, 0x86);
					lt9611_i2c_write_byte(0xfa, 0x00); //cec interrup mask
				    lt9611_i2c_write_byte(0xfc, 0x7f); //cec irq clr
				    lt9611_i2c_write_byte(0xfc, 0x00);

					/* cec irq init */
					lt9611_i2c_write_byte(0xff, 0x82);
					lt9611_i2c_write_byte(0x01, 0x7f); //mask bit[7]
					lt9611_i2c_write_byte(0x05, 0xff); //clr bit[7]
					lt9611_i2c_write_byte(0x05, 0x7f);
				}
			else
				{
					lt9611_i2c_write_byte(0xff, 0x86);
					lt9611_i2c_write_byte(0xfa, 0xff); //cec interrup mask
				    lt9611_i2c_write_byte(0xfc, 0x7f); //cec irq clr

					/* cec irq init */
					lt9611_i2c_write_byte(0xff, 0x82);
					lt9611_i2c_write_byte(0x01, 0xff); //mask bit[7]
					lt9611_i2c_write_byte(0x05, 0xff); //clr bit[7]
				}
	 	}
}

void LT9611_HDP_Interrupt_Handle(void)
{
     Tx_HPD = LT9611_Get_HPD_Status();

     lt9611_i2c_write_byte(0xff,0x82);
     lt9611_i2c_write_byte(0x07,0xff); //clear3
     lt9611_i2c_write_byte(0x07,0x3f); //clear3

     if(Tx_HPD)
     	{
	     	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDP_Interrupt_Handle: HDMI connected\n");
		 	LT9611_LowPower_mode(0);
			LT9611_Enable_Interrupts(VID_CHG_INTERRUPT_ENABLE, 1);
			Timer0_Delay1ms(100);
			LT9611_Read_EDID();
#ifdef cec_on
            lt9611_parse_physical_address(&lt9611_cec_msg, Sink_EDID);
            lt9611_cec_la_allocation(&lt9611_cec_msg);
#endif

	LT9611_Video_Check();
	if(Video_Format != video_none)
		{
				LT9611_PLL(video);
				LT9611_MIPI_Pcr(video);
				LT9611_HDMI_TX_Digital(video);
				LT9611_HDMI_Out_Enable();
		}
	else
		{
			LT9611_HDMI_Out_Disable();
			CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDP_Interrupt_Handle: no mipi video, disable hdmi output\n");
		}
	}
	else
	{
		CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_HDP_Interrupt_Handle: HDMI disconnected\n");
		LT9611_Enable_Interrupts(VID_CHG_INTERRUPT_ENABLE, 0);
		//LT9611_LowPower_mode(1);
#ifdef cec_on
		lt9611_cec_msg_init(&lt9611_cec_msg);
#endif
	}
}

void LT9611_Vid_Chg_Interrupt_Handle(void)
{
	CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Vid_Chg_Interrupt_Handle: \n");
	#if 1
	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0x9e,0xff); //clear vid chk irq
	lt9611_i2c_write_byte(0x9e,0xf7);

	lt9611_i2c_write_byte(0x04,0xff); //clear0 irq
	lt9611_i2c_write_byte(0x04,0xfe);
	#endif
	//Timer0_Delay1ms(100);
	LT9611_Video_Check();

	if(Video_Format != video_none)
	{
		LT9611_PLL(video);
		LT9611_MIPI_Pcr(video);
		LT9611_HDMI_TX_Digital(video);
		LT9611_HDMI_Out_Enable();
		//LT9611_HDCP_Enable();
	}
	else
	{
		//CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_Vid_Chg_Interrupt_Handle: no mipi video\n");
		LT9611_HDMI_Out_Disable();
	}
}

void lt9611_cec_msg_Interrupt_Handle(struct cec_msg *cec_msg)
{
	CVI_U8 cec_status;

	lt9611_i2c_write_byte(0xff,0x86);
	cec_status = lt9611_i2c_read_byte(0xd2) ;

	cec_msg->cec_status = cec_status;
	CVI_TRACE_LOG(CVI_DBG_INFO, "IRQ cec_status: 0x%x",cec_status);

	lt9611_i2c_write_byte(0xff, 0x86);
	lt9611_i2c_write_byte(0xfc, 0x7f); //cec irq clr
	lt9611_i2c_write_byte(0xfc, 0x00);

	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0x05,0xff); //clear3
	lt9611_i2c_write_byte(0x05,0x7f); //clear3

	lt9611_cec_msg_tx_handle(cec_msg);
	lt9611_cec_msg_rx_parse(cec_msg);
}


/////////////////////////////////////////////////////////////
//These function for Pattern output: start
/////////////////////////////////////////////////////////////
void LT9611_pattern_gcm(struct video_timing *video_format)
{
	CVI_U8 POL;
	POL = (video_format-> h_polarity)*0x10 + (video_format-> v_polarity)*0x20;
	POL = ~POL;
	POL &= 0x30;

	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0xa3,(CVI_U8)((video_format->hs+video_format->hbp)/256));//de_delay
	lt9611_i2c_write_byte(0xa4,(CVI_U8)((video_format->hs+video_format->hbp)%256));
	lt9611_i2c_write_byte(0xa5,(CVI_U8)((video_format->vs+video_format->vbp)%256));//de_top
	lt9611_i2c_write_byte(0xa6,(CVI_U8)(video_format->hact/256));
	lt9611_i2c_write_byte(0xa7,(CVI_U8)(video_format->hact%256));  //de_cnt
	lt9611_i2c_write_byte(0xa8,(CVI_U8)(video_format->vact/256));
	lt9611_i2c_write_byte(0xa9,(CVI_U8)(video_format->vact%256));  //de_line
	lt9611_i2c_write_byte(0xaa,(CVI_U8)(video_format->htotal/256));
	lt9611_i2c_write_byte(0xab,(CVI_U8)(video_format->htotal%256));//htotal
	lt9611_i2c_write_byte(0xac,(CVI_U8)(video_format->vtotal/256));
	lt9611_i2c_write_byte(0xad,(CVI_U8)(video_format->vtotal%256));//vtotal
	lt9611_i2c_write_byte(0xae,(CVI_U8)(video_format->hs/256));
	lt9611_i2c_write_byte(0xaf,(CVI_U8)(video_format->hs%256));    //hvsa
	lt9611_i2c_write_byte(0xb0,(CVI_U8)(video_format->vs%256));    //vsa

    lt9611_i2c_write_byte(0x47,(CVI_U8)(POL|0x07));  //sync polarity

}

void LT9611_pattern_pixel_clk(struct video_timing *video_format)
{
	CVI_U32 pclk;
	pclk = video_format->pclk_khz;
	CVI_TRACE_LOG(CVI_DBG_INFO, "set pixel clk = %d", pclk);
	//printdec_u32(pclk); //Dec

	lt9611_i2c_write_byte(0xff,0x83);
	lt9611_i2c_write_byte(0x2d,0x50);

	if(pclk == 297000)
	{
		lt9611_i2c_write_byte(0x26,0xb6);
		lt9611_i2c_write_byte(0x27,0xf0);
	}
	if(pclk == 148500)
	{
		lt9611_i2c_write_byte(0x26,0xb7);
	}
	if(pclk == 74250)
	{
		lt9611_i2c_write_byte(0x26,0x9c);
	}
	lt9611_i2c_write_byte(0xff,0x80);
	lt9611_i2c_write_byte(0x11,0x5a); //Pcr reset
	lt9611_i2c_write_byte(0x11,0xfa);
}

void LT9611_pattern_en(void)
{
	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0x4f,0x80);    //[7] = Select ad_txpll_d_clk.
	lt9611_i2c_write_byte(0x50,0x20);
}

void LT9611_pattern(void)
{
	//DTV
	//video = &video_640x480_60Hz;
	//video = &video_720x480_60Hz;
	//video = &video_1280x720_60Hz;
	video = &video_1920x1080_60Hz;
	//video = &video_3840x2160_30Hz;

	//DMT
	//video = &video_1024x600_60Hz;
	//video = &video_1024x600_60Hz;
	//video = &video_1280x800_60Hz;

	//video = &video_1920x1080_30Hz;

	LT9611_Chip_ID();
	LT9611_System_Init();
	LT9611_pattern_en();
	LT9611_PLL(video);
	LT9611_pattern_gcm(video);

	LT9611_HDMI_TX_Digital(video);
	LT9611_HDMI_TX_Phy();

	LT9611_Pcr_MK_Print();

#if 0
	//Audio pattern
	lt9611_i2c_write_byte(0xff,0x82);
	lt9611_i2c_write_byte(0xd6,0x8c);
	lt9611_i2c_write_byte(0xd7,0x06); //sync polarity

	lt9611_i2c_write_byte(0xff,0x84);
	lt9611_i2c_write_byte(0x06,0x0c);
	lt9611_i2c_write_byte(0x07,0x10);
	lt9611_i2c_write_byte(0x16,0x01);

	lt9611_i2c_write_byte(0x34,0xd4); //CTS_N
#endif
    LT9611_Audio_Init();
	LT9611_HDCP_Init();
	LT9611_load_hdcp_key();

    //LT9611_Read_EDID();
	LT9611_HDMI_Out_Enable();
    //LT9611_HDMI_CEC_ON(1);
	//lt9611_cec_msg_set_logical_address();

	//while(1)
	//{
	//	lt9611_cec_msg_write_demo();
	//}
}

/////////////////////////////////////////////////////////////
//These function for debug: end
/////////////////////////////////////////////////////////////


void LT9611_Init(void)
{
	LT9611_Chip_ID();
	LT9611_System_Init();

	LT9611_MIPI_Input_Analog();
	LT9611_MIPI_Input_Digtal();

	Timer0_Delay1ms(1000);
	LT9611_Video_Check();

	LT9611_PLL(video);
	LT9611_MIPI_Pcr(video);

	LT9611_Audio_Init();
	LT9611_CSC();
	LT9611_HDCP_Init();
	LT9611_HDMI_TX_Digital(video);
	LT9611_HDMI_TX_Phy();

	LT9611_IRQ_Init();;
	lt9611_cec_msg_init(&lt9611_cec_msg);

	LT9611_Enable_Interrupts(HPD_INTERRUPT_ENABLE, 1);
	LT9611_Enable_Interrupts(VID_CHG_INTERRUPT_ENABLE, 0);
	LT9611_Enable_Interrupts(CEC_INTERRUPT_ENABLE, 1);

	LT9611_Frequency_Meter_Byte_Clk();
	LT9611_Dphy_debug();
	LT9611_Htotal_Sysclk();
	LT9611_Pcr_MK_Print();

	CVI_TRACE_LOG(CVI_DBG_INFO, "==========================LT9611 Initial End===============================\n");
	Timer0_Delay1ms(200); //HPD have debounce, wait HPD irq.
	LT9611_HDP_Interrupt_Handle();
}

void LT9611_IRQ_Task(void)
{
	CVI_U8 irq_flag3;
	CVI_U8 irq_flag0;
	CVI_U8 irq_flag1;

	//CVI_TRACE_LOG(CVI_DBG_INFO, "LT9611_IRQ_Task :IRQ Task\n");

	lt9611_i2c_write_byte(0xff,0x82);

	irq_flag0=lt9611_i2c_read_byte(0x0c);
	irq_flag1=lt9611_i2c_read_byte(0x0d);
  	irq_flag3=lt9611_i2c_read_byte(0x0f);

	if((irq_flag1&0x80) == 0x80)
  {
		lt9611_cec_msg_Interrupt_Handle(&lt9611_cec_msg);
	}

	if(irq_flag3&0xc0)   //HPD interrupt
	{
		LT9611_HDP_Interrupt_Handle();
	}

	if(irq_flag0&0x01) //vid_chk
	{
		LT9611_Vid_Chg_Interrupt_Handle();
  }
}

static void __upadte_video(LT9611_VIDEO_FORMAT_E enVideoFormat)
{
    switch(enVideoFormat)
    {
    case E_LT9611_VIDEO_1024x768_60HZ:
        Video_Format = video_other;
        video = &video_1024x768_60Hz;
        break;
    case E_LT9611_VIDEO_1280x720_60HZ:
        Video_Format = video_1280x720_60Hz_vic4;
        video = &video_1280x720_60Hz;
        break;
    case E_LT9611_VIDEO_1280x1024_60HZ:
        Video_Format = video_other;
        video = &video_1280x1024_60Hz;
        break;
    case E_LT9611_VIDEO_1600x1200_60HZ:
        Video_Format = video_other;
        video = &video_1600x1200_60Hz;
        break;
    case E_LT9611_VIDEO_1920x1080_30HZ:
        Video_Format=video_1920x1080_30Hz_vic;
        video = &video_1920x1080_30Hz;
        break;
    case E_LT9611_VIDEO_1920x1080_60HZ:
        Video_Format = video_1920x1080_60Hz_vic16;
        video = &video_1920x1080_60Hz;
        break;
    default:
        CVI_TRACE_LOG(CVI_DBG_INFO, "ERROR: unknown video format...\n");
        break;
    }
}

void lt9611_init(LT9611_VIDEO_FORMAT_E enVideoFormat)
{
	LT9611_init_i2c();
    LT9611_Chip_ID();
    LT9611_System_Init();

    LT9611_MIPI_Input_Analog();
    LT9611_MIPI_Input_Digtal();

    __upadte_video(enVideoFormat);

    LT9611_PLL(video);
    LT9611_MIPI_Pcr(video);

    LT9611_Audio_Init();

    LT9611_HDCP_Init();
    LT9611_HDMI_TX_Digital(video);
    LT9611_HDMI_TX_Phy();
    LT9611_HDMI_Out_Enable();
}

void lt9611_set_video_format(LT9611_VIDEO_FORMAT_E enVideoFormat)
{
    __upadte_video(enVideoFormat);
    LT9611_HDMI_Out_Disable();

    usleep(1000000);

    LT9611_PLL(video);
    LT9611_MIPI_Pcr(video);
    LT9611_HDMI_TX_Digital(video);

    LT9611_HDMI_Out_Enable();
}


CVI_S32 lt9611_get_video_format(char* pinput_str)
{
	CVI_S32 i = 0;
	bool is_find = false;
	for(int i=0;i<E_LT9611_VIDEO_MAX;i++)
	{
		if(strcmp(pinput_str, s_hdmi_resolution_arr[i]) == 0)
		{
			is_find = true;
			break;
		}
	}
	if(!is_find)
	{
		return CVI_FAILURE;
	}
	g_video_format = (LT9611_VIDEO_FORMAT_E)i;
	return CVI_SUCCESS;
}

void printHdmiHelp(char **argv)
{
	CVI_U32 idx;

	printf("// ------------------------------------------------\n");
	printf("%s --resolution=select hdmi display solution\n", argv[0]);
	printf("\noptional resolution list by lt9611:\n");
	for(CVI_S32 i=0;i<E_LT9611_VIDEO_MAX;i++)
	{
		printf(" %s\n", s_hdmi_resolution_arr[i]);
	}
	printf("\nEX.\n");
	printf(" %s -h\n", argv[0]);
	printf(" %s --resolution=1920x1080_30HZ\n", argv[0]);
	printf(" %s -d\n", argv[0]);
	printf("// ------------------------------------------------\n");

	for (idx = 0; idx < sizeof(long_option_ext) / sizeof(optionExt); idx++) {
		if (long_option_ext[idx].opt.name == NULL)
			break;

		printf("--%s\n", long_option_ext[idx].opt.name);
		printf("    %s\n", long_option_ext[idx].help);
	}
}

int main(int argc, char *argv[])
{
	if(argc == 1) {
		lt9611_init(g_video_format);
		return CVI_SUCCESS;
	}

	struct option long_options[MAX_CMD_OPTIONS + 1];
	CVI_S32 ch, idx, ret;

	memset((void *)long_options, 0, sizeof(long_options));

	for (idx = 0; idx < MAX_CMD_OPTIONS; idx++) {
		if (long_option_ext[idx].opt.name == NULL)
			break;

		if (idx >= MAX_CMD_OPTIONS) {
			CVI_TRACE_LOG(CVI_DBG_ERR, "too many options\n");
			return -1;
		}

		memcpy(&long_options[idx], &long_option_ext[idx].opt, sizeof(struct option));
	}

	optind = 0;
	while ((ch = getopt_long(argc, argv, "h", long_options, &idx)) != -1) {
		switch (ch) {
			case 'r':
				ret = lt9611_get_video_format(optarg);
				if(ret != CVI_SUCCESS) {
					CVI_TRACE_LOG(CVI_DBG_ERR, "invalid resolution parameter\n");
					return ret;
				}
				break;
			case 'h':
				printHdmiHelp(argv);
				return CVI_SUCCESS;
			default:
				CVI_TRACE_LOG(CVI_DBG_INFO, "ch = %c\n", ch);
				printHdmiHelp(argv);
				break;
			}
	}

	if(optind < argc){
		printHdmiHelp(argv);
	}

	lt9611_init(g_video_format);

	return CVI_SUCCESS;
}



