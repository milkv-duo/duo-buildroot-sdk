#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <time.h>

#include "sample_comm.h"
#include "fontmod.h"

#define  colorbar_bmp  "colorbar.bmp"
#define  dog_bmp    "dog.bmp"
#define  tiger_bmp  "tiger.bmp"
#define  tiger_8bitmode "tiger_8bitmode.bmp"
#define  test_bmp tiger_bmp
#define IsASCII(a)				(((a) >= 0x00 && (a) <= 0x7F) ? 1 : 0)
#define BYTE_BITS				8
#define NOASCII_CHARACTER_BYTES	2
#define OSD_LIB_FONT_W			24
#define OSD_LIB_FONT_H			24

SAMPLE_VI_CONFIG_S stViConfig;
CVI_CHAR *Path_BMP;
#define MAX_STR_LEN  (64)

void SAMPLE_REGION_Usage(char *sPrgNm)
{
	printf("Usage : %s <index>\n", sPrgNm);
	printf("index:\n");
	printf("\t 0)VPSS OSD.\n");
	printf("\t 1)VPSS OSDEX.\n");
	printf("\t 2)VPSS COVEREX.\n");
	printf("\t 3)VPSS COVER.\n");
	printf("\t 4)VPSS MOSAIC.\n");
	printf("\t 5)VO OSDEX.\n");
	printf("\t 6)VO COEREX.\n");
	printf("\t 7)VPSS 2-layer COEREX.\n");
	printf("\t 8)VPSS OSD TIME.\n");
	printf("\t 9)VPSS OSD MultiChn.\n");
	printf("\t 10)VPSS OSD 8bit mode OVERLAY.\n");
}

void SAMPLE_REGION_HandleSig(CVI_S32 signo)
{
	if (SIGINT == signo || SIGTERM == signo) {
		SAMPLE_COMM_All_ISP_Stop();
		SAMPLE_COMM_SYS_Exit();
		printf("\033[0;35mprogram termination abnormally!\033[0;39m\n");
	}
	exit(-1);
}

static int SAMPLE_REGION_GetFontMod(char *Character, uint8_t **FontMod, int *FontModLen)
{
	uint32_t offset = 0;
	uint32_t areacode = 0;
	uint32_t bitcode = 0;

	if (IsASCII(Character[0])) {
		areacode = 3;
		bitcode = (uint32_t)((uint8_t)Character[0] - 0x20);
	} else {
		areacode = (uint32_t)((uint8_t)Character[0] - 0xA0);
		bitcode = (uint32_t)((uint8_t)Character[1] - 0xA0);
	}
	offset = (94 * (areacode - 1) + (bitcode - 1)) * (OSD_LIB_FONT_W * OSD_LIB_FONT_H / 8);
	*FontMod = (uint8_t *)g_fontLib + offset;
	*FontModLen = OSD_LIB_FONT_W*OSD_LIB_FONT_H / 8;
	return CVI_SUCCESS;
}

static int SAMPLE_REGION_GetNonASCNum(char *string, int len)
{
	int i;
	int n = 0;

	for (i = 0; i < len; i++) {
		if (string[i] == '\0')
			break;
		if (!IsASCII(string[i])) {
			i++;
			n++;
		}
	}

	return n;
}

static void SAMPLE_REGION_GetTimeStr(const struct tm *pstTime, char *pazStr, int s32Len)
{
	time_t nowTime;
	struct tm stTime = {
		0,
	};

	if (!pstTime) {
		time(&nowTime);
		localtime_r(&nowTime, &stTime);
		pstTime = &stTime;
	}

	snprintf(pazStr, s32Len, "%04d-%02d-%02d %02d:%02d:%02d",
		pstTime->tm_year + 1900, pstTime->tm_mon + 1, pstTime->tm_mday,
		pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec);
}

static int SAMPLE_REGION_UpdateBitmap(RGN_HANDLE RgnHdl, char *szStr, BITMAP_S *pstBitmap, CVI_U32 *pu32Color)
{
	int s32Ret;
	uint32_t u32CanvasWidth, u32CanvasHeight, u32BgColor, u32Color;
	SIZE_S stFontSize;
	int s32StrLen = strnlen(szStr, MAX_STR_LEN);
	int NonASCNum = SAMPLE_REGION_GetNonASCNum(szStr, s32StrLen);

	u32CanvasWidth = OSD_LIB_FONT_W * (s32StrLen - NonASCNum * (NOASCII_CHARACTER_BYTES - 1));
	u32CanvasHeight = OSD_LIB_FONT_H;
	stFontSize.u32Width = OSD_LIB_FONT_W;
	stFontSize.u32Height = OSD_LIB_FONT_H;
	u32BgColor = 0x7fff;

	pstBitmap->u32Width = u32CanvasWidth;
	pstBitmap->u32Height = u32CanvasHeight;
	pstBitmap->pData = malloc(2 * (pstBitmap->u32Width) * (pstBitmap->u32Height));
	if (pstBitmap->pData == NULL)
		SAMPLE_PRT("malloc osd memroy err!\n");

	uint16_t *puBmData = (uint16_t *)pstBitmap->pData;
	uint32_t u32BmRow, u32BmCol;

	for (u32BmRow = 0; u32BmRow < u32CanvasHeight; ++u32BmRow) {
		int NonASCShow = 0;

		for (u32BmCol = 0; u32BmCol < u32CanvasWidth; ++u32BmCol) {
			int s32BmDataIdx = u32BmRow * pstBitmap->u32Width + u32BmCol;
			int s32CharIdx = u32BmCol / stFontSize.u32Width;
			int s32StringIdx = s32CharIdx + NonASCShow * (NOASCII_CHARACTER_BYTES - 1);

			if (NonASCNum > 0 && s32CharIdx > 0) {
				NonASCShow = SAMPLE_REGION_GetNonASCNum(szStr, s32StringIdx);
				s32StringIdx = s32CharIdx + NonASCShow * (NOASCII_CHARACTER_BYTES - 1);
			}
			int s32CharCol = (u32BmCol - (stFontSize.u32Width * s32CharIdx)) * OSD_LIB_FONT_W /
							stFontSize.u32Width;
			int s32CharRow = u32BmRow * OSD_LIB_FONT_H / stFontSize.u32Height;
			int s32HexOffset = s32CharRow * OSD_LIB_FONT_W / BYTE_BITS + s32CharCol / BYTE_BITS;
			int s32BitOffset = s32CharCol % BYTE_BITS;
			uint8_t *FontMod = NULL;
			int FontModLen = 0;

			if (SAMPLE_REGION_GetFontMod(&szStr[s32StringIdx], &FontMod, &FontModLen) == CVI_SUCCESS) {
				if (FontMod != NULL && s32HexOffset < FontModLen) {
					uint8_t temp = FontMod[s32HexOffset];

					u32Color = *(pu32Color + s32CharIdx);
					if ((temp >> ((BYTE_BITS - 1) - s32BitOffset)) & 0x1)
						puBmData[s32BmDataIdx] = (uint16_t)u32Color;
					else
						puBmData[s32BmDataIdx] = (uint16_t)u32BgColor;
					continue;
				}
			}
			SAMPLE_PRT("GetFontMod Fail\n");
			return CVI_FAILURE;
		}
	}

	s32Ret = CVI_RGN_SetBitMap(RgnHdl, pstBitmap);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("CVI_RGN_SetBitMap failed with %#x!\n", s32Ret);
		return CVI_FAILURE;
	}

	free(pstBitmap->pData);

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VI_VPSS_VO_START(CVI_VOID)
{
	MMF_VERSION_S stVersion;
	SAMPLE_INI_CFG_S	   stIniCfg = {0};

	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;
	SIZE_S stSizeOut = { .u32Width = 1280, .u32Height = 720 };

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.MipiDev[0]   = 0xFF,
		.u8UseMultiSns = 0,
	};

	CVI_SYS_GetVersion(&stVersion);
	SAMPLE_PRT("MMF Version:%s\n", stVersion.version);

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg))
		SAMPLE_PRT("Parse complete\n");

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_SYS_INIT(stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sys init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VPSS_INIT(0, stSize, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}
	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vi vpss failed!\n");
		goto VI_BIND_VPSS_FAILED;
	}
	s32Ret = SAMPLE_PLAT_VO_INIT();
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start  vi failed!\n");
		goto START_Vo_FAILED;
	}

	CVI_VO_SetChnRotation(0, 0, ROTATION_90);

	s32Ret = SAMPLE_COMM_VPSS_Bind_VO(0, 0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss vo failed!\n");
		goto VPSS_BIND_VO_FAILED;
	}
	return CVI_SUCCESS;

VPSS_BIND_VO_FAILED:
	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
START_Vo_FAILED:
	SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
VI_BIND_VPSS_FAILED:
START_VPSS_FAILED:
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}

CVI_VOID SAMPLE_REGION_VI_VPSS_VO_END(CVI_VOID)
{
	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_VPSS_UnBind_VO(0, 0, 0, 0);
	SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
	SAMPLE_COMM_SYS_Exit();
}

CVI_S32 SAMPLE_REGION_VI_VPSS_VO(CVI_S32 HandleNum, RGN_TYPE_E enType, MMF_CHN_S *pstChn)
{
	CVI_S32 i;
	CVI_S32 s32Ret;
	CVI_S32 s32ExtRet;
	CVI_S32 MinHandle;
	PIXEL_FORMAT_E pixelFormat;

	s32Ret = SAMPLE_REGION_VI_VPSS_VO_START();
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	pixelFormat = PIXEL_FORMAT_ARGB_1555;
	s32Ret = SAMPLE_COMM_REGION_Create(HandleNum, enType, pixelFormat);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
		goto EXIT1;
	}
	s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum, enType, pstChn);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
		goto EXIT2;
	}

	if (enType == OVERLAY_RGN || enType == OVERLAYEX_RGN) {
		MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);

		for (i = MinHandle; i < MinHandle + HandleNum; i++) {
			//s32Ret = SAMPLE_COMM_REGION_SetBitMap(i, Path_BMP);
			s32Ret = SAMPLE_COMM_REGION_GetUpCanvas(i, Path_BMP);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("SAMPLE_COMM_REGION_GetUpCanvas failed!\n");
				goto EXIT2;
			}
		}
	}

	PAUSE();
EXIT2:
	s32ExtRet = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum, enType, pstChn);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_DetachFrmChn failed!\n");
EXIT1:
	s32ExtRet = SAMPLE_COMM_REGION_Destroy(HandleNum, enType);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");

	SAMPLE_REGION_VI_VPSS_VO_END();
	return s32Ret;
}

RGN_RGBQUARD_S g_stPaletteTable[16] = { //index0 ~ index15
		{255,   0,   0,   0}, {255, 255, 255, 255}, {255, 128, 128, 128},
		{255, 255,   0,   0}, {255,   0, 255,   0}, {255,   0,   0, 255},
		{255, 255, 255,   0}, {255, 255,   0, 255}, {255,   0, 255, 255},
		{255, 128,   0,   0}, {255,   0, 128,   0}, {255,   0,   0, 128},
		{255, 128, 128,   0}, {255, 128,   0, 128}, {255,   0, 128, 128},
};
CVI_S32 SAMPLE_REGION_VI_VPSS_VO_8BIT_MODE(CVI_S32 HandleNum, RGN_TYPE_E enType,
		MMF_CHN_S *pstChn, PIXEL_FORMAT_E pixelFormat)
{
	CVI_S32 i;
	CVI_S32 s32Ret;
	CVI_S32 MinHandle;
	RGN_PALETTE_S stPalette;

	s32Ret = SAMPLE_REGION_VI_VPSS_VO_START();
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	s32Ret = SAMPLE_COMM_REGION_Create(HandleNum, enType, pixelFormat);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
		goto EXIT1;
	}

	s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum, enType, pstChn);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
		goto EXIT2;
	}

	MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);

#ifdef _USE_CUSTOMER_PALETTE_
	/* Use customer's definition in COVER_RGN example. */
	if (enType == COVER_RGN) {
		for (i = 0; i < 16; i++) {
			CVI_U32 u32Pixel =
					((g_stPaletteTable[i].argbBlue | g_stPaletteTable[i].argbGreen << 8) |
					 (g_stPaletteTable[i].argbRed << 16 | g_stPaletteTable[i].argbAlpha << 24));
			printf("g_stPaletteTable index(%d) (0x%x).\n", i, u32Pixel);
		}
		CVI_RGN_SetChnPalette(MinHandle, pstChn, g_stPaletteTable, 16, RGN_COLOR_FMT_ARGB8888);
	}
#endif

	/* Use indexed palettes format of bmp file in OVERLAY example. */
	if (enType == OVERLAY_RGN) {
		for (i = MinHandle; i < MinHandle + HandleNum; i++) {
			s32Ret = SAMPLE_COMM_REGION_SetBitMap(i, Path_BMP, pixelFormat, false);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("SAMPLE_COMM_REGION_SetBitMap failed!\n");
				goto EXIT2;
			}
		}

#ifdef _SAMPLE_REGION_DEBUG_
		for (i = 0; i < 256 ; i++) {
			CVI_U32 u32Pixel =
					((overlay_palette[i].argbBlue | overlay_palette[i].argbGreen << 8) |
					 (overlay_palette[i].argbRed << 16 | overlay_palette[i].argbAlpha << 24));
			printf("overlay_palette index(%d) (0x%x).\n", i, u32Pixel);
		}
#endif
		stPalette.pstPaletteTable = (void *)overlay_palette;
		stPalette.lut_length = 256;
		stPalette.pixelFormat = RGN_COLOR_FMT_RGB888;
		CVI_RGN_SetChnPalette(MinHandle, pstChn, &stPalette);
	}

	PAUSE();
EXIT2:
	s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum, enType, pstChn);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
EXIT1:
	s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum, enType);
	if (s32Ret != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");

	SAMPLE_REGION_VI_VPSS_VO_END();
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_OSD(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 3;
	enType = OVERLAY_RGN;
	stChn.enModId = CVI_ID_VPSS;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	Path_BMP = test_bmp;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_OSDEX(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 3;
	enType = OVERLAYEX_RGN;
	stChn.enModId = CVI_ID_VPSS;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	Path_BMP = test_bmp;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_COVEREX(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 3;
	enType = COVEREX_RGN;
	stChn.enModId = CVI_ID_VPSS;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_COVER(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 3;
	enType = COVER_RGN;
	stChn.enModId = CVI_ID_VPSS;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_MOSAIC(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 3;
	enType = MOSAIC_RGN;
	stChn.enModId = CVI_ID_VPSS;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_OSD_8BIT_MODE(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;
	PIXEL_FORMAT_E pixelFormat;

	HandleNum = 3;
	enType = OVERLAY_RGN;
	stChn.enModId = CVI_ID_VPSS;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	pixelFormat = PIXEL_FORMAT_8BIT_MODE;
	Path_BMP = tiger_8bitmode;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO_8BIT_MODE(HandleNum, enType, &stChn, pixelFormat);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VO_OSD(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 1;
	enType = OVERLAY_RGN;
	stChn.enModId = CVI_ID_VO;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	Path_BMP = test_bmp;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VO_COVER(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn;

	HandleNum = 1;
	enType = COVER_RGN;
	stChn.enModId = CVI_ID_VO;
	stChn.s32DevId = 0;
	stChn.s32ChnId = 0;
	s32Ret = SAMPLE_REGION_VI_VPSS_VO(HandleNum, enType, &stChn);
	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VI_VPSS_VO_2LAYER(CVI_VOID)
{
	MMF_VERSION_S stVersion;
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	SAMPLE_VI_CONFIG_S stViConfig;

	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32ExtRet = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;
	SIZE_S stSizeOut = { .u32Width = 1280, .u32Height = 720 };

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.MipiDev[0]   = 0xFF,
		.u8UseMultiSns = 0,
	};

	CVI_SYS_GetVersion(&stVersion);
	SAMPLE_PRT("MMF Version:%s\n", stVersion.version);

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg))
		SAMPLE_PRT("Parse complete\n");

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_SYS_INIT(stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sys init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Create 2 vpss grp 0/1.
	 *         Bind vpss-grp-1 to vpss-grp-0 to have 2-layer region.
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VPSS_INIT(0, stSize, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}

	s32Ret = SAMPLE_PLAT_VPSS_INIT(1, stSizeOut, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}

	VPSS_CHN_ATTR_S stChnAttr;

	CVI_VPSS_GetChnAttr(0, 0, &stChnAttr);
	stChnAttr.bFlip = CVI_FALSE;
	stChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(0, 0, &stChnAttr);
#if 0
	CVI_VPSS_GetChnAttr(1, 0, &stChnAttr);
	stChnAttr.bFlip = CVI_FALSE;
	stChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(1, 0, &stChnAttr);
#endif
	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vi vpss failed!\n");
		goto VI_BIND_VPSS_FAILED;
	}
	s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(0, 0, 1);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss-vpss failed!\n");
		goto VPSS_BIND_VPSS_FAILED;
	}

	/************************************************
	 * step4:  VO init.
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VO_INIT();
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start  vi failed!\n");
		goto START_Vo_FAILED;
	}

	CVI_VO_SetChnRotation(0, 0, ROTATION_90);

	s32Ret = SAMPLE_COMM_VPSS_Bind_VO(1, 0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss vo failed!\n");
		goto VPSS_BIND_VO_FAILED;
	}

	/************************************************
	 * step5:  RGN attached to vpss-grp-0/1
	 ************************************************/
	CVI_S32 HandleNum;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn0, stChn1;
	RGN_HANDLE overlay_hdl, cover_hdl;
	RGN_CHN_ATTR_S stRgnChnAttr;
	PIXEL_FORMAT_E pixelFormat;

	// create cover at 1st layer.
	HandleNum = 1;
	enType = COVER_RGN;
	stChn0.enModId = CVI_ID_VPSS;
	stChn0.s32DevId = 0;
	stChn0.s32ChnId = 0;
	pixelFormat = PIXEL_FORMAT_ARGB_1555;

	s32Ret = SAMPLE_COMM_REGION_Create(HandleNum, enType, pixelFormat);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
		goto EXIT4;
	}
	s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum, enType, &stChn0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
		goto EXIT3;
	}
	cover_hdl = SAMPLE_COMM_REGION_GetMinHandle(enType);

	// create cover at 2nd layer.
	HandleNum = 1;
	enType = OVERLAY_RGN;
	stChn1.enModId = CVI_ID_VPSS;
	stChn1.s32DevId = 1;
	stChn1.s32ChnId = 0;
	Path_BMP = dog_bmp;
	s32Ret = SAMPLE_COMM_REGION_Create(HandleNum, enType, pixelFormat);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
		goto EXIT2;
	}
	s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum, enType, &stChn1);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
		goto EXIT1;
	}

	overlay_hdl = SAMPLE_COMM_REGION_GetMinHandle(enType);

	s32Ret = SAMPLE_COMM_REGION_GetUpCanvas(overlay_hdl, Path_BMP);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("SAMPLE_COMM_REGION_GetUpCanvas failed!\n");
		goto EXIT1;
	}

	PAUSE();

	SAMPLE_PRT("Change cover to green!\n");
	CVI_RGN_GetDisplayAttr(cover_hdl, &stChn0, &stRgnChnAttr);
	stRgnChnAttr.unChnAttr.stCoverChn.u32Color = 0x0000ff00;
	CVI_RGN_SetDisplayAttr(cover_hdl, &stChn0, &stRgnChnAttr);
	PAUSE();

	SAMPLE_PRT("Change cover to blue and smaller size!\n");
	CVI_RGN_GetDisplayAttr(cover_hdl, &stChn0, &stRgnChnAttr);
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Width = 50;
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 50;
	stRgnChnAttr.unChnAttr.stCoverChn.u32Color = 0x000000ff;
	CVI_RGN_SetDisplayAttr(cover_hdl, &stChn0, &stRgnChnAttr);
	PAUSE();

	SAMPLE_PRT("Change cover size bigger!\n");
	CVI_RGN_GetDisplayAttr(cover_hdl, &stChn0, &stRgnChnAttr);
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Width = 200;
	stRgnChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 200;
	CVI_RGN_SetDisplayAttr(cover_hdl, &stChn0, &stRgnChnAttr);
	PAUSE();

EXIT1:
	s32ExtRet = SAMPLE_COMM_REGION_DetachFrmChn(1, OVERLAY_RGN, &stChn1);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_DetachFrmChn failed!\n");
EXIT2:
	s32ExtRet = SAMPLE_COMM_REGION_Destroy(1, OVERLAY_RGN);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");

EXIT3:
	s32ExtRet = SAMPLE_COMM_REGION_DetachFrmChn(1, COVER_RGN, &stChn0);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_DetachFrmChn failed!\n");
EXIT4:
	s32ExtRet = SAMPLE_COMM_REGION_Destroy(1, COVER_RGN);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");

VPSS_BIND_VO_FAILED:
	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);

	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
START_Vo_FAILED:
	SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
VI_BIND_VPSS_FAILED:
VPSS_BIND_VPSS_FAILED:
START_VPSS_FAILED:
	SAMPLE_COMM_SYS_Exit();
	return s32Ret;
}


CVI_S32 SAMPLE_REGION_VPSS_OSD_TIME(CVI_VOID)
{
	CVI_S32 s32Ret;
	CVI_S32 s32ExtRet;
	CVI_S32 HandleNum;
	CVI_S32 MinHandle;
	RGN_TYPE_E enType;
	MMF_CHN_S stChn0;
	RGN_ATTR_S stRegion;
	RGN_CHN_ATTR_S stChnAttr;
	CVI_S32 i;
	char szStr[MAX_STR_LEN];
	int s32StrLen;

	s32Ret = SAMPLE_REGION_VI_VPSS_VO_START();
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	VPSS_CHN_ATTR_S stVpssChnAttr;

	CVI_VPSS_GetChnAttr(0, 0, &stVpssChnAttr);
	stVpssChnAttr.bFlip = CVI_FALSE;
	stVpssChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(0, 0, &stVpssChnAttr);

	SAMPLE_REGION_GetTimeStr(NULL, szStr, MAX_STR_LEN);
	s32StrLen = strnlen(szStr, MAX_STR_LEN);

	HandleNum = 2;
	enType = OVERLAY_RGN;
	stChn0.enModId = CVI_ID_VPSS;
	stChn0.s32DevId = 0;
	stChn0.s32ChnId = 0;
	MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);

	stRegion.enType = OVERLAY_RGN;
	stRegion.unAttr.stOverlay.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
	stRegion.unAttr.stOverlay.stSize.u32Height = OSD_LIB_FONT_H;
	stRegion.unAttr.stOverlay.stSize.u32Width = OSD_LIB_FONT_W * s32StrLen;
	stRegion.unAttr.stOverlay.u32BgColor = 0x7fff;
	stRegion.unAttr.stOverlay.u32CanvasNum = 1;
	for (i = MinHandle; i < MinHandle + HandleNum; i++) {
		s32Ret = CVI_RGN_Create(i, &stRegion);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			goto EXIT1;
		}
	}

	stChnAttr.bShow = CVI_TRUE;
	stChnAttr.enType = OVERLAY_RGN;
	stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Width = OSD_LIB_FONT_W;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.stInvColArea.u32Height = OSD_LIB_FONT_H;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.u32LumThresh = 128;
	stChnAttr.unChnAttr.stOverlayChn.stInvertColor.enChgMod = MORETHAN_LUM_THRESH;
	for (i = MinHandle; i < MinHandle + HandleNum; i++) {
		stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20 + 300 * (i - MinHandle);
		stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 20 + 300 * (i - MinHandle);
		if (i % 2 != 0)
			stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = CVI_TRUE;
		else
			stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = CVI_FALSE;
		s32Ret = CVI_RGN_AttachToChn(i, &stChn0, &stChnAttr);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
			goto EXIT2;
		}
	}

	BITMAP_S stBitmap;
	CVI_U32 *pu32Color = (CVI_U32 *)malloc(s32StrLen * sizeof(CVI_U32));

	while (CVI_TRUE) {
		for (i = MinHandle; i < MinHandle + HandleNum; i++) {
			memset(pu32Color, 0, sizeof(CVI_U32) * s32StrLen);
			CVI_RGN_GetDisplayAttr(i, &stChn0, &stChnAttr);
			if (stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn)
				CVI_RGN_Invert_Color(i, &stChn0, pu32Color);
			else
				memset(pu32Color, 0xffff, sizeof(CVI_U32) * s32StrLen);
			stBitmap.u32Width = stRegion.unAttr.stOverlay.stSize.u32Width;
			stBitmap.u32Height = stRegion.unAttr.stOverlay.stSize.u32Height;
			stBitmap.enPixelFormat = stRegion.unAttr.stOverlay.enPixelFormat;
			s32Ret = SAMPLE_REGION_UpdateBitmap(i, szStr, &stBitmap, pu32Color);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("SAMPLE_REGION_UpdateBitmap failed!\n");
				goto EXIT2;
			}
		}
		usleep(500 * 1000);
	}
	free(pu32Color);

EXIT2:
	s32ExtRet = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum, enType, &stChn0);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_DetachFrmChn failed!\n");
EXIT1:
	s32ExtRet = SAMPLE_COMM_REGION_Destroy(HandleNum, enType);
	if (s32ExtRet != CVI_SUCCESS)
		SAMPLE_PRT("SAMPLE_COMM_REGION_Destroy failed!\n");

	SAMPLE_REGION_VI_VPSS_VO_END();

	return s32Ret;
}

CVI_S32 SAMPLE_REGION_VPSS_OSD_MULTICHN(CVI_VOID)
{
	MMF_VERSION_S stVersion;
	SAMPLE_INI_CFG_S	   stIniCfg = {0};
	SAMPLE_VI_CONFIG_S stViConfig;

	PIC_SIZE_E enPicSize;
	SIZE_S stSize;
	CVI_S32 s32Ret = CVI_SUCCESS;
	LOG_LEVEL_CONF_S log_conf;
	SIZE_S stSizeOut = { .u32Width = 1280, .u32Height = 720 };

	stIniCfg = (SAMPLE_INI_CFG_S) {
		.enSource  = VI_PIPE_FRAME_SOURCE_DEV,
		.devNum    = 1,
		.enSnsType[0] = SONY_IMX327_MIPI_2M_30FPS_12BIT,
		.enWDRMode[0] = WDR_MODE_NONE,
		.s32BusId[0]  = 3,
		.MipiDev[0]   = 0xFF,
		.u8UseMultiSns = 0,
	};

	CVI_SYS_GetVersion(&stVersion);
	SAMPLE_PRT("MMF Version:%s\n", stVersion.version);

	log_conf.enModId = CVI_ID_LOG;
	log_conf.s32Level = CVI_DBG_INFO;
	CVI_LOG_SetLevelConf(&log_conf);

	// Get config from ini if found.
	if (SAMPLE_COMM_VI_ParseIni(&stIniCfg))
		SAMPLE_PRT("Parse complete\n");

	/************************************************
	 * step1:  Config VI
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &stViConfig);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	/************************************************
	 * step2:  Get input size
	 ************************************************/
	s32Ret = SAMPLE_COMM_VI_GetSizeBySensor(stIniCfg.enSnsType[0], &enPicSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_VI_GetSizeBySensor failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "SAMPLE_COMM_SYS_GetPicSize failed with %#x\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_SYS_INIT(stSize);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "sys init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	s32Ret = SAMPLE_PLAT_VI_INIT(&stViConfig);
	if (s32Ret != CVI_SUCCESS) {
		CVI_TRACE_LOG(CVI_DBG_ERR, "vi init failed. s32Ret: 0x%x !\n", s32Ret);
		return s32Ret;
	}

	/************************************************
	 * step3:  Create 4 vpss grp 0/1/2/3.
	 *         Bind vpss-grp-1 to vpss-grp-0.
	 *         Bind vpss-grp-2 to vpss-grp-1.
	 *         Bind vpss-grp-3 to vpss-grp-2.
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VPSS_INIT(0, stSize, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}

	s32Ret = SAMPLE_PLAT_VPSS_INIT(1, stSizeOut, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}

	s32Ret = SAMPLE_PLAT_VPSS_INIT(2, stSizeOut, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}

	s32Ret = SAMPLE_PLAT_VPSS_INIT(3, stSizeOut, stSizeOut);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start vpss failed!\n");
		goto START_VPSS_FAILED;
	}

	VPSS_CHN_ATTR_S stChnAttr;

	CVI_VPSS_GetChnAttr(0, 0, &stChnAttr);
	stChnAttr.bFlip = CVI_FALSE;
	stChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(0, 0, &stChnAttr);

	CVI_VPSS_GetChnAttr(1, 0, &stChnAttr);
	stChnAttr.bFlip = CVI_FALSE;
	stChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(1, 0, &stChnAttr);

	CVI_VPSS_GetChnAttr(2, 0, &stChnAttr);
	stChnAttr.bFlip = CVI_FALSE;
	stChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(2, 0, &stChnAttr);

	CVI_VPSS_GetChnAttr(3, 0, &stChnAttr);
	stChnAttr.bFlip = CVI_FALSE;
	stChnAttr.bMirror = CVI_FALSE;
	CVI_VPSS_SetChnAttr(3, 0, &stChnAttr);

	s32Ret = SAMPLE_COMM_VI_Bind_VPSS(0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vi vpss failed!\n");
		goto VI_BIND_VPSS_FAILED;
	}
	s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(0, 0, 1);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss-vpss failed!\n");
		goto VPSS_BIND_VPSS_FAILED;
	}
	s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(1, 0, 2);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss-vpss failed!\n");
		goto VPSS_BIND_VPSS_FAILED;
	}
	s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(2, 0, 3);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss-vpss failed!\n");
		goto VPSS_BIND_VPSS_FAILED;
	}

	/************************************************
	 * step4:  VO init.
	 ************************************************/
	s32Ret = SAMPLE_PLAT_VO_INIT();
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Start  vi failed!\n");
		goto START_Vo_FAILED;
	}

	CVI_VO_SetChnRotation(0, 0, ROTATION_90);

	s32Ret = SAMPLE_COMM_VPSS_Bind_VO(3, 0, 0, 0);
	if (s32Ret != CVI_SUCCESS) {
		SAMPLE_PRT("Bind vpss vo failed!\n");
		goto VPSS_BIND_VO_FAILED;
	}

	/************************************************
	 * step5:  RGN attached to vpss-grp-0/1/2/3
	 ************************************************/
	MMF_CHN_S stChn[4];
	CVI_S32 i, j;

	stChn[0].enModId = CVI_ID_VPSS;
	stChn[0].s32DevId = 0;
	stChn[0].s32ChnId = 0;

	stChn[1].enModId = CVI_ID_VPSS;
	stChn[1].s32DevId = 1;
	stChn[1].s32ChnId = 0;

	stChn[2].enModId = CVI_ID_VPSS;
	stChn[2].s32DevId = 2;
	stChn[2].s32ChnId = 0;

	stChn[3].enModId = CVI_ID_VPSS;
	stChn[3].s32DevId = 3;
	stChn[3].s32ChnId = 0;

	Path_BMP = dog_bmp;

	RGN_ATTR_S stRgnOverlay;

	stRgnOverlay.enType = OVERLAY_RGN;
	stRgnOverlay.unAttr.stOverlay.enPixelFormat = PIXEL_FORMAT_ARGB_1555;
	stRgnOverlay.unAttr.stOverlay.stSize.u32Height = 100;
	stRgnOverlay.unAttr.stOverlay.stSize.u32Width = 100;
	stRgnOverlay.unAttr.stOverlay.u32BgColor = 0x0000;
	stRgnOverlay.unAttr.stOverlay.u32CanvasNum = 1;

	RGN_ATTR_S stRgnCover;

	stRgnCover.enType = COVER_RGN;

	RGN_CHN_ATTR_S stOverlayChnAttr;

	stOverlayChnAttr.bShow = CVI_TRUE;
	stOverlayChnAttr.enType = OVERLAY_RGN;
	stOverlayChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = CVI_FALSE;
	stOverlayChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;

	RGN_CHN_ATTR_S stCoverChnAttr;

	stCoverChnAttr.bShow = CVI_TRUE;
	stCoverChnAttr.enType = COVER_RGN;
	stCoverChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_RECT;
	stCoverChnAttr.unChnAttr.stCoverChn.stRect.u32Height = 100;
	stCoverChnAttr.unChnAttr.stCoverChn.stRect.u32Width = 100;
	stCoverChnAttr.unChnAttr.stCoverChn.u32Color = 0x00ffff;
	stCoverChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;
	stCoverChnAttr.unChnAttr.stCoverChn.u32Layer = 0;

	for (i = 0; i < 16; i++) {
		s32Ret = CVI_RGN_Create(i, &stRgnOverlay);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
		if (i < 8) {
			if (i < 4) {
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 20;
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = 180 * i + 30;
			} else {
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 160;
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = (i - 4) * 180 + 30;
			}

			s32Ret = CVI_RGN_AttachToChn(i, &stChn[0], &stOverlayChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		} else {
			if (i < 12) {
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 320;
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = (i - 8) * 180 + 30;
			} else {
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = 480;
				stOverlayChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = (i - 12) * 180 + 30;
			}
			s32Ret = CVI_RGN_AttachToChn(i, &stChn[1], &stOverlayChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		}

		s32Ret = SAMPLE_COMM_REGION_GetUpCanvas(i, Path_BMP);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("SAMPLE_COMM_REGION_GetUpCanvas failed!\n");
			return CVI_FAILURE;
		}
	}

	for (i = 16; i < 32; i++) {
		s32Ret = CVI_RGN_Create(i, &stRgnCover);
		if (s32Ret != CVI_SUCCESS) {
			SAMPLE_PRT("CVI_RGN_Create failed with %#x!\n", s32Ret);
			return CVI_FAILURE;
		}
		if (i < 24) {
			if (i < 20) {
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32X = 640;
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32Y = (i - 16) * 180 + 30;
			} else {
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32X = 800;
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32Y = (i - 20) * 180 + 30;
			}
			s32Ret = CVI_RGN_AttachToChn(i, &stChn[2], &stCoverChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		} else {
			if (i < 28) {
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32X = 960;
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32Y = (i - 24) * 180 + 30;
			} else {
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32X = 1120;
				stCoverChnAttr.unChnAttr.stCoverChn.stRect.s32Y = (i - 28) * 180 + 30;
			}

			s32Ret = CVI_RGN_AttachToChn(i, &stChn[3], &stCoverChnAttr);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_RGN_AttachToChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		}
	}

	PAUSE();

	for (j = 0; j < 4; j++) {
		for (i = j * 8; i < (j + 1) * 8; i++) {
			s32Ret = CVI_RGN_DetachFromChn(i, &stChn[j]);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_RGN_DetachFromChn failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}

			s32Ret = CVI_RGN_Destroy(i);
			if (s32Ret != CVI_SUCCESS) {
				SAMPLE_PRT("CVI_RGN_Destroy failed with %#x!\n", s32Ret);
				return CVI_FAILURE;
			}
		}
	}
	SAMPLE_COMM_VPSS_UnBind_VO(3, 0, 0, 0);
VPSS_BIND_VO_FAILED:
START_Vo_FAILED:
	SAMPLE_COMM_VI_UnBind_VPSS(0, 0, 0);
VPSS_BIND_VPSS_FAILED:
START_VPSS_FAILED:
VI_BIND_VPSS_FAILED:
	SAMPLE_COMM_VI_DestroyIsp(&stViConfig);
	SAMPLE_COMM_VI_DestroyVi(&stViConfig);
	SAMPLE_COMM_SYS_Exit();

	return s32Ret;
}

int main(int argc, char *argv[])
{
	CVI_S32 s32Ret = CVI_FAILURE;
	CVI_S32 s32Index;

	if (argc < 2 || argc > 2) {
		SAMPLE_REGION_Usage(argv[0]);
		return CVI_FAILURE;
	}

	if (!strncmp(argv[1], "-h", 2)) {
		SAMPLE_REGION_Usage(argv[0]);
		return CVI_SUCCESS;
	}

	signal(SIGINT, SAMPLE_REGION_HandleSig);
	signal(SIGTERM, SAMPLE_REGION_HandleSig);

	s32Index = atoi(argv[1]);
	switch (s32Index) {
	case 0:
		s32Ret = SAMPLE_REGION_VPSS_OSD();
		break;
	case 1:
		s32Ret = SAMPLE_REGION_VPSS_OSDEX();
		break;
	case 2:
		s32Ret = SAMPLE_REGION_VPSS_COVEREX();
		break;
	case 3:
		s32Ret = SAMPLE_REGION_VPSS_COVER();
		break;
	case 4:
		s32Ret = SAMPLE_REGION_VPSS_MOSAIC();
		break;
	case 5:
		s32Ret = SAMPLE_REGION_VO_OSD();
		break;
	case 6:
		s32Ret = SAMPLE_REGION_VO_COVER();
		break;
	case 7:
		s32Ret = SAMPLE_REGION_VI_VPSS_VO_2LAYER();
		break;
	case 8:
		s32Ret = SAMPLE_REGION_VPSS_OSD_TIME();
		break;
	case 9:
		s32Ret = SAMPLE_REGION_VPSS_OSD_MULTICHN();
		break;
	case 10:
		s32Ret = SAMPLE_REGION_VPSS_OSD_8BIT_MODE();
		break;
	default:
		SAMPLE_PRT("option, %d, is invaild!\n", s32Index);
		SAMPLE_REGION_Usage(argv[0]);
		s32Ret = CVI_FAILURE;
		break;
	}

	if (s32Ret == CVI_SUCCESS)
		SAMPLE_PRT("program exit normally!\n");
	else
		SAMPLE_PRT("program exit abnormally!\n");

	return s32Ret;
}
