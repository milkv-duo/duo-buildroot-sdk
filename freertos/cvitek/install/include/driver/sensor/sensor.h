#ifndef __CV_IMAGE_SENSOR_H__
#define __CV_IMAGE_SENSOR_H__

#include <delay.h>
#include "cvi_comm_video.h"
#include "cvi_common.h"
#include "cvi_comm_vi.h"
#include "cvi_comm_isp.h"
#include "cvi_sns_ctrl.h"
#include "cif_uapi.h"
#include "gpio.h"
#include "sample_comm.h"

#define TPU_SRAM_BASE_ADDR      (0x0C020000)
#define TPU_SRAM_BASE_SIZE      (0x00010000)
#define RTC_SRAM_BASE_ADDR      (0x05200000)
#define RTC_SRAM_BASE_SIZE      (24 << 10)	// 24KB

//#define SENSOR_USE_I2C_DMA	0
#define CHECK_SENSOR_ID		1
#define SEN_CFG_ADDR            (RTC_SRAM_BASE_SIZE - 1024)
#define SEN_CFG_INI_ADDR        (RTC_SRAM_BASE_SIZE - 1024)

#define SYSDMA_LLP_ADDR         (RTC_SRAM_BASE_SIZE - 1024)
#define SEN_ITB_START_ADDR      (RTC_SRAM_BASE_SIZE - 1024)

#define SEN_MAX_LANE_NUM	(MIPI_LANE_NUM + 1) // 1C4D

#define DIV_0_TO_1(a)           ((0 == (a)) ? 1 : (a))
#define DIV_0_TO_1_FLOAT(a)     ((((a) < 1E-10) && ((a) > -1E-10)) ? 1 : (a))

/* Sensor resolutions */
#define SENSOR_RES_IS_2160P(w, h)      ((w) <= 3840 && (h) <= 2160)
#define SENSOR_RES_IS_1440P(w, h)      ((w) <= 2560 && (h) <= 1440)
#define SENSOR_RES_IS_1080P(w, h)      ((w) <= 1920 && (h) <= 1080)
#define SENSOR_RES_IS_720P(w, h)       ((w) <= 1280 && (h) <= 720)

#define delay_ms(a)		mdelay(a)

enum HDR_EXPOSURE_FRAME_ID {
	SHORT_EXP_ID,
	LONG_EXP_ID,
};

enum IMAGE_FORMAT_E {
	BAYER_RAW12 = 0,
	BAYER_RAW10,
	BAYER_RAW8,
	BAYER_FORMAT_NUM,
	YUV422_8B = BAYER_FORMAT_NUM,
	YUV422_10B,
};

/*
 * To indicate the 1st two pixel in the bayer_raw.
 */
enum DPHY_LANE_NUM {
	DPHY_1_DLANE	= 0,
	DPHY_2_DLANE	= 1,
	DPHY_4_DLANE	= 3,
	DPHY_8_DLANE	= 7
};

enum HDR_MODE_E {
	HDR_MODE_VC,	// CSI
	HDR_MODE_ID,	// CSI
	HDR_MODE_DT,	// CSI
	HDR_MODE_DOL,	// CSI
	HDR_MODE_PAT1,	// SLVDS
	HDR_MODE_PAT2,	// SLVDS
	HDR_MODE_LINEAR = 0xF
};

enum ISP_BAYER_TYPE {
	ISP_BAYER_TYPE_BG    = 0,
	ISP_BAYER_TYPE_GB,
	ISP_BAYER_TYPE_GR,
	ISP_BAYER_TYPE_RG,
//	ISP_BAYER_TYPE_MAX,
};

typedef struct _ISP_HDR_SIZE_S {
	RECT_S stWndRect;
	SIZE_S stSnsSize;
	// SIZE_S stMaxSize;
} ISP_HDR_SIZE_S;

typedef struct _SIMPLE_AE_CTRL_S {
	uint32_t seLuma;
	uint32_t seExp;
	uint32_t seGain;
	uint32_t leLuma;
	uint32_t leExp;
	uint32_t leGain;
	uint16_t rWb_Gain;
	uint16_t bWb_Gain;
} SIMPLE_AE_CTRL_S;

/* define of Linux
typedef struct _SAMPLE_SENSOR_INFO_S {
	SAMPLE_SNS_TYPE_E enSnsType;
	CVI_S32 s32SnsId;
	CVI_S32 s32BusId;
	CVI_S32 s32SnsI2cAddr;
	combo_dev_t MipiDev;
	CVI_S16 as16LaneId[5];
	CVI_S8  as8PNSwap[5];
	CVI_U8  u8HwSync;
	SAMPLE_SENSOR_MCLK_ATTR_S stMclkAttr;
	CVI_U8 u8Orien;	// 0: normal, 1: mirror, 2: flip, 3: mirror and flip.
} SAMPLE_SENSOR_INFO_S;
*/
typedef struct _SENSOR_CFG_INI_HEADER {
	uint32_t magic_num;	// date for check
	uint16_t dev_num;	// totla sensor number
	uint16_t cfg_ofs;	// sizeof SENSOR_USR_CFG
} __attribute__((packed)) __attribute__((aligned(0x8))) SENSOR_CFG_INI_HEADER;

typedef struct _SENSOR_USR_CFG_INI {
	char name[64];		// sensor name

	uint8_t devno;		// support 0:sensor0, 1:sensor1, 2: sensor2
	uint8_t mclk_en;	// enable of mclk (0: driver default, 1: mclk0 enable, 2:maclk1 enable)
	uint8_t mclk;		// mclk clock
	// i2c info
	int8_t slave_id;	// slave id (slave addr[7:1]) 0: use default
	int8_t bus_id;		// i2c bus id
	// lane info
	uint8_t mipi_dev;	// MIPI phy number,
	uint8_t hs_settle;	// 0: use default
	int8_t lane_id[SEN_MAX_LANE_NUM];	// {clk, d0 ,d1, d2, d3}
	int8_t pn_swap[SEN_MAX_LANE_NUM];	// {clk, d0 ,d1, d2, d3}
	uint8_t u8HwSync;
	uint8_t u8Orien;	// 0: normal, 1: mirror, 2: flip, 3: mirror and flip.
	// reset-pin info
	uint32_t 		snsr_reset;	// sensor reset pin
	uint8_t 		reset_act;	// sensor reset low/high act fill by sensor driver
	uint32_t		reset_delay;	// delay us after reset
} __attribute__((packed)) __attribute__((aligned(0x8))) SENSOR_USR_CFG;

typedef struct _SENSOR_INFO {
	SENSOR_CFG_INI_HEADER	*header;
	SENSOR_USR_CFG		*cfg;		// load from flash
	ISP_SNS_OBJ_S 		*pstSnsObj;

	enum input_mode_e	vi_mode;	// fill by sensor name
	enum IMAGE_FORMAT_E	format;		// fill by sensor driver
	enum ISP_BAYER_TYPE	bayer_type;	// fill by sensor driver
	enum HDR_MODE_E		hdr_mode;	// fill by sensor name
	VI_WORK_MODE_E		chn;		// fill by sensor name
	enum DPHY_LANE_NUM	lane_num;	// fill by sensor name

	uint32_t		width;		// output width fill by sensor name
	uint32_t		height;		// output height fill by sensor name

	uint8_t 		init_ok;	// sensor init success
} SENSOR_INFO;

/*
 * Support Sensor Objects
 */
// extern ISP_SNS_OBJ_S stSnsImx327_Obj;

extern const char *snsr_type_name[SAMPLE_SNS_TYPE_BUTT];

/*
 * Sensor I2C API
 */
int sensor_i2c_init(VI_PIPE ViPipe, uint8_t u8I2cDev, uint16_t speed,
		uint8_t slave_addr, uint8_t alen, uint8_t dlen);
int sensor_i2c_exit(VI_PIPE ViPipe);
int sensor_read_register(VI_PIPE ViPipe, int addr);
int sensor_write_register(VI_PIPE ViPipe, int addr, int data);

/*
 * Sensor API
 */
extern int32_t cmos_set_sns_regs_info(VI_PIPE ViPipe, ISP_SNS_SYNC_INFO_S *pstSnsSyncInfo);

#endif //__CV_IMAGE_SENSOR_H__
