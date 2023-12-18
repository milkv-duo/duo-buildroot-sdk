#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <inttypes.h>

#include "sample_comm.h"
#include "mipi_tx.h"
#include "sample_panel.h"

static int fd;

#define MAX_OPTIONS	128
#define LANE_MAX_NUM   5

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

typedef enum {
	DSI_PANEL_3AML069LP01G,
	DSI_PANEL_GM8775C,
	DSI_PANEL_HX8394_EVB,
	DSI_PANEL_HX8399_1080P,
	DSI_PANEL_ICN9707,
	DSI_PANEL_ILI9881C,
	DSI_PANEL_ILI9881D,
	DSI_PANEL_JD9366AB,
	DSI_PANEL_LT9611_1920x1080_60,
	DSI_PANEL_LT9611_1920x1080_30,
	DSI_PANEL_LT9611_1280x720_60,
	DSI_PANEL_LT9611_1024x768_60,
	DSI_PANEL_LT9611_1280x1024_60,
	DSI_PANEL_LT9611_1600x1200_60,
	DSI_PANEL_NT35521,
	DSI_PANEL_OTA7290B_1920,
	DSI_PANEL_OTA7290B,
	DSI_PANEL_ST7701,
	LVDS_PANEL_LCM185X56,
	BT_PANEL_TP2803_BT656_1280x720_25FPS_72M,
	BT_PANEL_NVP6021_BT1120_1920x1080_25FPS_72M,
	I80_PANEL_ST7789V3_HW_MCU_240x320_60FPS,
	PANEL_MAX
} PANEL_MODEL;

typedef struct _inputPara_ {
	enum mipi_tx_lane_id    lane_id[LANE_MAX_NUM];
	bool                    lane_pn_swap[LANE_MAX_NUM];
	bool					lane_id_flag;
	bool					pn_swap_flag;
	PANEL_MODEL			panel_model;
} inputPara;

inputPara g_input_para = {
	.panel_model = DSI_PANEL_HX8394_EVB,
};

static struct panel_desc_s g_panel_desc = {
	.panel_mode = "HX8394_EVB",
	.panel_type = PANEL_MODE_DSI,
	.stdsicfg.dev_cfg = &dev_cfg_hx8394_720x1280,
	.stdsicfg.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280,
	.stdsicfg.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280,
	.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280)
};

static optionExt long_option_ext[] = {
	{{"panel",  optional_argument, NULL, 'm'},   ARG_STRING,   0,   0,
		"choose diaply panel model"},
	{{"laneid",    optional_argument, NULL, 'l'},   ARG_STRING,    0,   0,
		"laneid sequence by order"},
	{{"pnswap",    optional_argument, NULL, 'p'},   ARG_STRING,   0,   0,
		"pnswap sequence by order"},
	{{"dsi-control",     no_argument, NULL, 'd'}, ARG_STRING, 0,   0,
		"set/get dsi status or settings." },
	{{"help",      no_argument, NULL, 'h'},       ARG_STRING, 0,   0,
		"print usage."},
	{{NULL, 0, NULL, 0}, ARG_INT, 0, 0, "no param: just init the panel."}
};

static char *s_panel_model_type_arr[] = {
	"3AML069LP01G",
	"GM8775C",
	"HX8394_EVB",
	"HX8399_1080P",
	"ICN9707",
	"ILI9881C",
	"ILI9881D",
	"JD9366AB",
	"LT9611_1920x1080_60",
	"LT9611_1920x1080_30",
	"LT9611_1280x720_60",
	"LT9611_1024x768_60",
	"LT9611_1280x1024_60",
	"LT9611_1600x1200_60",
	"NT35521",
	"OTA7290B_1920",
	"OTA7290B",
	"ST7701",
	"LCM185X56",
	"TP2803_BT656_1280x720_25FPS_72M",
	"BT_PANEL_NVP6021_BT1120_1920x1080_25FPS_72M",
	"ST7789V3_HW_MCU_RGB565_240x320_60FPS",
};

void printdsiHelp(void)
{
	printf("\n// ------------------------dsi-control------------------------\n");
	printf(" 0: dcs send\n");
	printf(" 1: dcs get\n");
	printf(" 2: switch to lp\n");
	printf(" 3: switch to hs\n");
	printf(" 4: get hs settle settings\n");
	printf(" 5: set hs settle settings\n");
}

void printHelp(char **argv)
{
	CVI_U32 idx;

	printf("// ------------------------help------------------------\n");
	printf("\noptional panel mode support list:\n");
	for (idx = 0; idx < sizeof(s_panel_model_type_arr) / sizeof(char *); idx++) {
		printf(" %s\n", s_panel_model_type_arr[idx]);
	}

	printf("\n.for mipi/lvds panel you can cfg lane seq or pnswap");
	printf("\nEX.\n");
	printf(" %s --panel=HX8394_EVB --laneid=1,2,0,3,4 --pnswap=0,0,0,0,0\n", argv[0]);
	printf("\n.for mipi panel You can also manually set the dsi by -d");
	printf("\nEX.\n");
	printf(" %s -d\n\n", argv[0]);

	for (idx = 0; idx < sizeof(long_option_ext) / sizeof(optionExt); idx++) {
		if (long_option_ext[idx].opt.name == NULL) {
			break;
		}

		printf("--%s\n", long_option_ext[idx].opt.name);
		printf("    %s\n", long_option_ext[idx].help);
	}

	printf("// ------------------------------------------------\n");
}

int dsi_init(int devno, const struct dsc_instr *cmds, int size)
{
	int ret;

	if (cmds == NULL) {
		return CVI_FAILURE;
	}

	for (int i = 0; i < size; i++) {
		const struct dsc_instr *instr = &cmds[i];
		struct cmd_info_s cmd_info = {
			.devno = devno,
			.cmd_size = instr->size,
			.data_type = instr->data_type,
			.cmd = (void *)instr->data
		};

		ret = mipi_tx_send_cmd(fd, &cmd_info);
		if (instr->delay)
			usleep(instr->delay * 1000);

		if (ret) {
			printf("dsi init failed at %d instr.\n", i);
			return ret;
		}
	}
	return ret;
}

CVI_S32 SAMPLE_MIPI_TX_ENABLE(void)
{
	CVI_S32 ret = 0;

	fd = open(MIPI_TX_NAME, O_RDWR | O_NONBLOCK, 0);
	if (fd == -1) {
		printf("Cannot open '%s': %d, %s\n", MIPI_TX_NAME, errno, strerror(errno));
		return CVI_FAILURE;
	}

	ret = mipi_tx_disable(fd);
	if (ret != CVI_SUCCESS) {
		printf("mipi_tx_disable fail!\n");
		return CVI_FAILURE;
	}

	ret = mipi_tx_cfg(fd, (struct combo_dev_cfg_s *)g_panel_desc.stdsicfg.dev_cfg);
	if (ret != CVI_SUCCESS) {
		printf("mipi_tx_cfg fail!\n");
		return CVI_FAILURE;
	}
	ret = dsi_init(0, g_panel_desc.stdsicfg.dsi_init_cmds, g_panel_desc.stdsicfg.dsi_init_cmds_size);
	if (ret != CVI_SUCCESS) {
		printf("dsi_init fail!\n");
		return CVI_FAILURE;
	}

	ret = mipi_tx_set_hs_settle(fd, g_panel_desc.stdsicfg.hs_timing_cfg);
	if (ret != CVI_SUCCESS) {
		printf("mipi_tx_set_hs_settle fail!\n");
		return CVI_FAILURE;
	}

	ret = mipi_tx_enable(fd);
	if (ret != CVI_SUCCESS) {
		printf("mipi_tx_enable fail!\n");
		return CVI_FAILURE;
	}

	printf("Init for MIPI-Driver-%s\n", g_panel_desc.panel_mode);

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_PANEL_ENABLE(void)
{
	CVI_S32 ret = 0;
	VO_DEV VoDev = 0;

	if (g_panel_desc.panel_type == PANEL_MODE_DSI) {
		ret = SAMPLE_MIPI_TX_ENABLE();
		if (ret != CVI_SUCCESS) {
			printf("SAMPLE_MIPI_TX_ENABLE fail!\n");
			return CVI_FAILURE;
		}
	} else {
		ret = CVI_VO_SetPubAttr(VoDev, &g_panel_desc.stVoPubAttr);
		if (ret != CVI_SUCCESS) {
			printf("failed with %#x!\n", ret);
			return CVI_FAILURE;
		}
		printf("Init for Driver-%s\n", g_panel_desc.panel_mode);
	}

	ret = CVI_VO_ShowPattern(VoDev, VO_PAT_COLORBAR);
	if (ret != CVI_SUCCESS) {
		printf("failed with %#x!\n", ret);
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

void SAMPLE_DSI_CONTROLE(void)
{
	CVI_U32 tmp;

	do {
		printdsiHelp();
		printf(" others: exit\n");
		scanf("%d", &tmp);
		if (tmp == 0) {
			struct cmd_info_s cmd_info;
			CVI_U8 data[16] = { 0 };
			int len = 0;

			printf("data size:\n");
			scanf("%d", &tmp);
			cmd_info.cmd_size = tmp;

			printf("data type: 0x\n");
			scanf("%x", &tmp);
			cmd_info.data_type = tmp;

			do {
				printf("data[%d]: 0x\n", len);
				scanf("%x", &tmp);
				data[len++] = tmp;
			} while (len < cmd_info.cmd_size);
			cmd_info.cmd = data;
			mipi_tx_send_cmd(fd, &cmd_info);
		} else if (tmp == 1) {
			struct get_cmd_info_s cmd_info;
			CVI_U8 data[4] = { 0 };

			printf("get data size:\n");
			scanf("%d", &tmp);
			cmd_info.get_data_size = tmp;

			printf("data type: 0x\n");
			scanf("%x", &tmp);
			cmd_info.data_type = tmp;

			printf("data param: 0x\n");
			scanf("%x", &tmp);
			cmd_info.data_param = tmp;

			cmd_info.get_data = data;
			mipi_tx_recv_cmd(fd, &cmd_info);
			printf("data[0]: %#x [1]: %#x [2]: %#x [3]: %#x\n"
				, cmd_info.get_data[0], cmd_info.get_data[1]
				, cmd_info.get_data[2], cmd_info.get_data[3]);
		} else if (tmp == 2) {
			mipi_tx_disable(fd);
		} else if (tmp == 3) {
			mipi_tx_enable(fd);
		} else if (tmp == 4) {
			struct hs_settle_s hs_cfg;

			mipi_tx_get_hs_settle(fd, &hs_cfg);
			printf("prepare(%d) zero(%d) trail(%d)\n",
				hs_cfg.prepare, hs_cfg.zero, hs_cfg.trail);
		} else if (tmp == 5) {
			struct hs_settle_s hs_cfg;

			printf("prepare:\n");
			scanf("%d", &tmp);
			hs_cfg.prepare = tmp;

			printf("zero:\n");
			scanf("%d", &tmp);
			hs_cfg.zero = tmp;

			printf("trail:\n");
			scanf("%d", &tmp);
			hs_cfg.trail = tmp;

			mipi_tx_set_hs_settle(fd, &hs_cfg);
		} else
			break;
	} while (1);
}

void SAMPLE_SET_PANEL_DESC(void)
{
	switch (g_input_para.panel_model) {
	case DSI_PANEL_ILI9881C:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_ili9881c_720x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_ili9881c_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_ili9881c_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ili9881c_720x1280);
		break;
	case DSI_PANEL_ILI9881D:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_ili9881d_720x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_ili9881d_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_ili9881d_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ili9881d_720x1280);
		break;
	case DSI_PANEL_JD9366AB:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_jd9366ab_800x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_jd9366ab_800x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_jd9366ab_800x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_jd9366ab_800x1280);
		break;
	case DSI_PANEL_NT35521:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_nt35521_800x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_nt35521_800x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_nt35521_800x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_nt35521_800x1280);
		break;
	case DSI_PANEL_OTA7290B:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_ota7290b_320x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_ota7290b_320x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_ota7290b_320x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ota7290b_320x1280);
		break;
	case DSI_PANEL_OTA7290B_1920:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_ota7290b_440x1920;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_ota7290b_440x1920;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_ota7290b_440x1920;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_ota7290b_440x1920);
		break;
	case DSI_PANEL_ICN9707:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_icn9707_480x1920;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_icn9707_480x1920;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_icn9707_480x1920;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_icn9707_480x1920);
		break;
	case DSI_PANEL_3AML069LP01G:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_3AML069LP01G_600x1024;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_3AML069LP01G_600x1024;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_3AML069LP01G_600x1024;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_3AML069LP01G_600x1024);
		break;
	case DSI_PANEL_ST7701:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_st7701_480x800;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_st7701_480x800;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_st7701_480x800;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_st7701_480x800);
		break;
	case DSI_PANEL_HX8399_1080P:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_hx8399_1080x1920;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_hx8399_1080x1920;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_hx8399_1080x1920;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8399_1080x1920);
		break;
	case DSI_PANEL_GM8775C:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_gm8775c;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_gm8775c;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_gm8775c;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_gm8775c);
		break;
	case DSI_PANEL_LT9611_1920x1080_60:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_lt9611_1920x1080_60Hz;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_lt9611;
		g_panel_desc.stdsicfg.dsi_init_cmds = NULL;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = 0;
		break;
	case DSI_PANEL_LT9611_1920x1080_30:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_lt9611_1920x1080_30Hz;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_lt9611;
		g_panel_desc.stdsicfg.dsi_init_cmds = NULL;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = 0;
		break;
	case DSI_PANEL_LT9611_1280x720_60:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_lt9611_1280x720_60Hz;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_lt9611;
		g_panel_desc.stdsicfg.dsi_init_cmds = NULL;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = 0;
		break;
	case DSI_PANEL_LT9611_1024x768_60:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_lt9611_1024x768_60Hz;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_lt9611;
		g_panel_desc.stdsicfg.dsi_init_cmds = NULL;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = 0;
		break;
	case DSI_PANEL_LT9611_1280x1024_60:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_lt9611_1280x1024_60Hz;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_lt9611;
		g_panel_desc.stdsicfg.dsi_init_cmds = NULL;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = 0;
		break;
	case DSI_PANEL_LT9611_1600x1200_60:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_lt9611_1600x1200_60Hz;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_lt9611;
		g_panel_desc.stdsicfg.dsi_init_cmds = NULL;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = 0;
		break;
	case DSI_PANEL_HX8394_EVB:
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_hx8394_720x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280);
		break;
	case LVDS_PANEL_LCM185X56:
		g_panel_desc.panel_type = PANEL_MODE_LVDS;
		g_panel_desc.stVoPubAttr.enIntfType = VO_INTF_LCD_24BIT;
		g_panel_desc.stVoPubAttr.enIntfSync = VO_OUTPUT_USER;
		VO_SYNC_INFO_S stLcm185x56_SyncInfo = {.bSynm = 1, .bIop = 1, .u16FrameRate = 60
		, .u16Vact = 768, .u16Vbb = 20, .u16Vfb = 10
		, .u16Hact = 1366, .u16Hbb = 100, .u16Hfb = 88
		, .u16Vpw = 2, .u16Hpw = 20, .bIdv = 0, .bIhs = 0, .bIvs = 0};
		g_panel_desc.stVoPubAttr.stSyncInfo = stLcm185x56_SyncInfo;
		g_panel_desc.stVoPubAttr.stLvdsAttr = lvds_lcm185x56_cfg;
		break;
	case BT_PANEL_TP2803_BT656_1280x720_25FPS_72M:
		g_panel_desc.panel_type = PANEL_MODE_BT;
		g_panel_desc.stVoPubAttr.enIntfType = VO_INTF_BT656;
		g_panel_desc.stVoPubAttr.enIntfSync = VO_OUTPUT_USER;
		VO_SYNC_INFO_S stTp2803_SyncInfo = {.bSynm = 1, .bIop = 1, .u16FrameRate = 25
		, .u16Vact = 720, .u16Vbb = 20, .u16Vfb = 5
		, .u16Hact = 1280, .u16Hbb = 200, .u16Hfb = 400
		, .u16Vpw = 5, .u16Hpw = 40, .bIdv = 0, .bIhs = 0, .bIvs = 0};
		g_panel_desc.stVoPubAttr.stSyncInfo = stTp2803_SyncInfo;
		g_panel_desc.stVoPubAttr.stBtAttr = stTP2803Cfg;
		break;
	case BT_PANEL_NVP6021_BT1120_1920x1080_25FPS_72M:
		g_panel_desc.panel_type = PANEL_MODE_BT;
		g_panel_desc.stVoPubAttr.enIntfType = VO_INTF_BT1120;
		g_panel_desc.stVoPubAttr.enIntfSync = VO_OUTPUT_USER;
		VO_SYNC_INFO_S stNvp6021_SyncInfo = {.bSynm = 1, .bIop = 1, .u16FrameRate = 25
		, .u16Vact = 1080, .u16Vbb = 36, .u16Vfb = 4
		, .u16Hact = 1920, .u16Hbb = 148, .u16Hfb = 528
		, .u16Vpw = 5, .u16Hpw = 44, .bIdv = 0, .bIhs = 0, .bIvs = 0};
		g_panel_desc.stVoPubAttr.stSyncInfo = stNvp6021_SyncInfo;
		g_panel_desc.stVoPubAttr.stBtAttr = stNVP6021Cfg;
		break;
	case I80_PANEL_ST7789V3_HW_MCU_240x320_60FPS:
		g_panel_desc.panel_type = PANEL_MODE_MCU;
		g_panel_desc.stVoPubAttr.enIntfType = VO_INTF_HW_MCU;
		g_panel_desc.stVoPubAttr.enIntfSync = VO_OUTPUT_USER;
		VO_SYNC_INFO_S st7789V3_SyncInfo = {.bSynm = 1, .bIop = 1, .u16FrameRate = 60
		, .u16Vact = 320, .u16Vbb = 0, .u16Vfb = 32
		, .u16Hact = 240, .u16Hbb = 0, .u16Hfb = 16
		, .u16Vpw = 2, .u16Hpw = 2, .bIdv = 0, .bIhs = 1, .bIvs = 1};
		g_panel_desc.stVoPubAttr.stSyncInfo = st7789V3_SyncInfo;
		g_panel_desc.stVoPubAttr.stMcuCfg = st7789v3Cfg;
		break;
	default:
		printf("default\n");
		g_panel_desc.panel_type = PANEL_MODE_DSI;
		g_panel_desc.stdsicfg.dev_cfg = &dev_cfg_hx8394_720x1280;
		g_panel_desc.stdsicfg.hs_timing_cfg = &hs_timing_cfg_hx8394_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds = dsi_init_cmds_hx8394_720x1280;
		g_panel_desc.stdsicfg.dsi_init_cmds_size = ARRAY_SIZE(dsi_init_cmds_hx8394_720x1280);
		break;
	}
	if (g_input_para.pn_swap_flag) {
		if (g_panel_desc.panel_type == PANEL_MODE_LVDS) {
			for (CVI_U32 i = 0; i < LANE_MAX_NUM; i++) {
				g_panel_desc.stVoPubAttr.stLvdsAttr.lane_pn_swap[i] =
				(enum VO_LVDS_LANE_ID)g_input_para.lane_pn_swap[i];
			}
		} else if (g_panel_desc.panel_type == PANEL_MODE_DSI) {
			for (CVI_U32 i = 0; i < LANE_MAX_NUM; i++) {
				g_panel_desc.stdsicfg.dev_cfg->lane_pn_swap[i] = g_input_para.lane_pn_swap[i];
			}
		}
	}
	if (g_input_para.lane_id_flag) {
		if (g_panel_desc.panel_type == PANEL_MODE_LVDS) {
			for (CVI_U32 i = 0; i < LANE_MAX_NUM; i++) {
				g_panel_desc.stVoPubAttr.stLvdsAttr.lane_id[i] =
				(enum VO_LVDS_LANE_ID)g_input_para.lane_id[i];
			}
		} else if (g_panel_desc.panel_type == PANEL_MODE_DSI) {
			for (CVI_U32 i = 0; i < LANE_MAX_NUM; i++) {
				g_panel_desc.stdsicfg.dev_cfg->lane_id[i] = g_input_para.lane_id[i];
			}
		}
	}
}

CVI_S32 SAMPLE_SET_PANEL_MODEL(char *pinput_str)
{
	CVI_S32 i = 0;
	bool is_find = false;
	PANEL_MODEL panel_model = DSI_PANEL_HX8394_EVB;

	for (i = 0; i < PANEL_MAX; i++) {
		if (strcmp(pinput_str, s_panel_model_type_arr[i]) == 0) {
			is_find = true;
			break;
		}
	}

	if (is_find) {
		panel_model = (PANEL_MODEL)i;
	} else {
		return CVI_FAILURE;
	}

	g_input_para.panel_model = panel_model;
	g_panel_desc.panel_mode = s_panel_model_type_arr[i];
	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_SET_LANEID(char *pLaneid)
{
	CVI_S32 lane_id[] = {0, 0, 0, 0, 0};

	if (pLaneid == NULL)
		return CVI_FAILURE;

	CVI_S32 n = sscanf(pLaneid, "%02d,%02d,%02d,%02d,%02d",
		&lane_id[0], &lane_id[1], &lane_id[2], &lane_id[3], &lane_id[4]);

	if (n != sizeof(lane_id)/sizeof(CVI_S32)) {
		return CVI_FAILURE;
	}
	g_input_para.lane_id_flag = true;
	for (CVI_U32 i = 0; i < sizeof(lane_id)/sizeof(CVI_S32); i++) {
		if (lane_id[i] < -1 || lane_id[i] > 5) {
			return CVI_FAILURE;
		}
		g_input_para.lane_id[i] = lane_id[i];
	}

	return CVI_SUCCESS;
}

CVI_S32 SAMPLE_SET_PNSWAP(char *pPnswap)
{
	CVI_U32 pnswap[] = {0, 0, 0, 0, 0};

	if (pPnswap == NULL)
		return CVI_FAILURE;

	CVI_S32 n = sscanf(pPnswap, "%02d,%02d,%02d,%02d,%02d",
		&pnswap[0], &pnswap[1], &pnswap[2], &pnswap[3], &pnswap[4]);

	if (n != sizeof(pnswap)/sizeof(CVI_U32)) {
		return CVI_FAILURE;
	}
	g_input_para.pn_swap_flag = true;
	for (CVI_U32 i = 0; i < sizeof(pnswap)/sizeof(CVI_U32); i++) {
		if ((pnswap[i] != 0) && (pnswap[i] != 1)) {
			return CVI_FAILURE;
		}
		g_input_para.lane_pn_swap[i] = (bool)(pnswap[i]);
	}

	return CVI_SUCCESS;
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printHelp(argv);
		return CVI_SUCCESS;
	}

	struct option long_options[MAX_OPTIONS + 1];
	CVI_S32 ch, idx, ret;

	memset((void *)long_options, 0, sizeof(long_options));

	for (idx = 0; idx < MAX_OPTIONS; idx++) {
		if (long_option_ext[idx].opt.name == NULL)
			break;

		if (idx >= MAX_OPTIONS) {
			printf("too many options\n");
			return -1;
		}

		memcpy(&long_options[idx], &long_option_ext[idx].opt, sizeof(struct option));
	}

	optind = 0;
	while ((ch = getopt_long(argc, argv, "dh", long_options, &idx)) != -1) {
		switch (ch) {
		case 'l':
			ret = SAMPLE_SET_LANEID(optarg);
			if (ret != CVI_SUCCESS) {
				printf("invalid laneid parameter\n");
				return ret;
			}
			break;
		case 'p':
			ret = SAMPLE_SET_PNSWAP(optarg);
			if (ret != CVI_SUCCESS) {
				printf("invalid pnswap parameter\n");
				return ret;
			}
			break;
		case 'm':
			ret = SAMPLE_SET_PANEL_MODEL(optarg);
			if (ret != CVI_SUCCESS) {
				printf("invalid input panel model\n");
				return ret;
			}
			break;
		case 'd':
			if (argc > 2) {
				printf("usage:%s -d. -d can't use in the same time with other command\n", argv[0]);
				return CVI_FAILURE;
			}
			SAMPLE_DSI_CONTROLE();
			break;
		case 'h':
			printHelp(argv);
			return CVI_SUCCESS;
		default:
			printf("ch = %c\n", ch);
			printHelp(argv);
			return CVI_FAILURE;
		}
	}

	if (optind < argc) {
		printHelp(argv);
	}

	SAMPLE_SET_PANEL_DESC();
	SAMPLE_PANEL_ENABLE();

	while (1) {
		sleep(1);
	}

	return CVI_SUCCESS;
}