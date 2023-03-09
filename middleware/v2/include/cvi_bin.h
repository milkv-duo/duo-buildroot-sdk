#ifndef __CVI_BIN_H__
#define __CVI_BIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "stdio.h"
#include <linux/cvi_type.h>
#include <linux/cvi_comm_video.h>

// ++++++++ If you want to change these interfaces, please contact the isp team. ++++++++
#define BIN_FILE_LENGTH	256

#define CVI_BIN_NULL_POINT  0xCB000001
#define CVI_BIN_REG_ATTR_ERR  0xCB000002
#define CVI_BIN_MALLOC_ERR  0xCB000003
#define CVI_BIN_CHIP_ERR  0xCB000004
#define CVI_BIN_CRC_ERR  0xCB000005
#define CVI_BIN_SIZE_ERR  0xCB000006 /*size of inputing data is error.*/
#define CVI_BIN_LEBLE_ERR  0xCB000007
#define CVI_BIN_DATA_ERR  0xCB000008 /*data abnormal*/
#define CVI_BIN_SECURITY_SOLUTION_FAILED  0xCB00000A
#define CVI_BIN_COMPRESS_ERROR  0xCB00000B /*json compress buffer fail.*/
#define CVI_BIN_UNCOMPRESS_ERROR  0xCB00000C /*json uncompress fail.*/
#define CVI_BIN_SAPCE_ERR  0xCB00000D /*Input buffer space isn't enough.*/
#define CVI_BIN_JSON_ERR  0xCB00000E /*json is inexistence in current bin file.*/
#define CVI_BIN_UPDATE_ERROR  0xCB00000F /*Update bin para fail automatically*/
#define CVI_BIN_FINDBINFILE_ERROR  0xCB000010 /*can't find bin file*/
#define CVI_BIN_NOJSON_ERROR  0xCB000011 /*invalid json in current bin file*/
#define CVI_BIN_JSONHANLE_ERROR  0xCB000012 /*creat json handle fail*/
#define CVI_BIN_ID_ERROR  0xCB000013 /*invalid id error*/
#define CVI_BIN_READ_ERROR  0xCB000014 /*read para from file fail*/
#define CVI_BIN_FILE_ERROR  0xCB000015 /*PQbin file is invalid.*/
#define CVI_BIN_SENSORNUM_ERROR  0xCB000016 /*Sensor number exceeds specified sensor number in the current bin file.*/

enum CVI_BIN_SECTION_ID {
	CVI_BIN_ID_MIN = 0,
	CVI_BIN_ID_HEADER = CVI_BIN_ID_MIN,
	CVI_BIN_ID_ISP0,
	CVI_BIN_ID_ISP1,
	CVI_BIN_ID_ISP2,
	CVI_BIN_ID_ISP3,
	CVI_BIN_ID_VPSS,
	CVI_BIN_ID_VDEC,
	CVI_BIN_ID_VENC,
	CVI_BIN_ID_VO,
	CVI_BIN_ID_MAX
};

enum CVI_BIN_CREATMODE {
	CVI_BIN_AUTO = 0,
	CVI_BIN_MANUAL,
	CVI_BIN_MODE_MAX
};

typedef struct {
	CVI_UCHAR Author[32];
	CVI_UCHAR Desc[1024];
	CVI_UCHAR Time[32];
} CVI_BIN_EXTRA_S;

typedef struct {
	CVI_U32 u32InitSize;
	CVI_U32 u32CompreSize;
} CVI_JSON_INFO;

typedef struct _CVI_JSON_HEADER {
	CVI_JSON_INFO size[CVI_BIN_ID_MAX];
} CVI_JSON_HEADER;

typedef struct _CVI_BIN_HEADER {
	CVI_U32 chipId;
	CVI_BIN_EXTRA_S extraInfo;
	CVI_U32 size[CVI_BIN_ID_MAX];
} CVI_BIN_HEADER;

CVI_S32 CVI_BIN_GetBinExtraAttr(FILE *fp, CVI_BIN_EXTRA_S *extraInfo);
CVI_S32 CVI_BIN_SaveParamToBin(FILE *fp, CVI_BIN_EXTRA_S *extraInfo);
CVI_S32 CVI_BIN_LoadParamFromBin(enum CVI_BIN_SECTION_ID id, CVI_U8 *buf);
CVI_S32 CVI_BIN_SetBinName(WDR_MODE_E wdrMode, const CVI_CHAR *binName);
CVI_S32 CVI_BIN_GetBinName(CVI_CHAR *binName);

/* CVI_BIN_LoadParamFromBinEx:
 *   get bin data from buffer
 *
 * [in]	buf: input buf
 *   u32DataLength:length of bin data
 * [Out]void
 * return: please refer to CVI_BIN_ImportBinData
 */
CVI_S32 CVI_BIN_LoadParamFromBinEx(enum CVI_BIN_SECTION_ID id, CVI_U8 *buf, CVI_U32 u32DataLength);
/* CVI_BIN_GetBinTotalLen:
 *   Get length of bin data
 *
 * [in]void
 * [Out]void
 * return: length of bin data
 */
CVI_U32 CVI_BIN_GetBinTotalLen(void);
/* CVI_BIN_ExportBinData:
 *   get bin data from buffer
 *
 * [in]	pu8Buffer:save bin data
 *      u32DataLength:length of bin data
 * [Out]void.
 * return: 0: Success;
 *		error codes:
 *		0xCB000001:input pointer is null.
 *		0xCB000003: malloc fail.
 *		0xCB000008: data error.
 *		0xCB00000A: security solution fail.
 *		0xCB00000B: json compress fail.
 *		0xCB00000D: Input buffer space isn't enough.
 *		0xCB000012: creat json handle fail.
 *		0xCB000013: invalid id error.
 */
CVI_S32 CVI_BIN_ExportBinData(CVI_U8 *pu8Buffer, CVI_U32 u32DataLength);
/* CVI_BIN_ImportBinData:
 *   set bin data from buffer
 *
 * [in]	pu8Buffer:save bin data
 *		u32DataLength:length of bin data
 * [Out]void
 * return: 0: Success;
 *		error codes:
 *		0xCB000001: input pointer is null.
 *		0xCB000003: malloc fail.
 *		0xCB000006: size of inputing data is error.
 *		0xCB000008: data error.
 *		0xCB00000C: json uncompress fail.
 *		0xCB00000E: json inexistence fail.
 *		0xCB00000F: update bin para from json fail automatically.
 *		0xCB000010: can't find bin file.
 *		0xCB000011: invalid json in current bin file.
 *		0xCB000012: creat json handle fail.
 *		0xCB000013: invalid id error.
 *		0xCB000014: read para from file fail.
 *		0xCB000015: current PQbin file is invalid.
 *		0xCB000016: Sensor number exceeds specified sensor number in bin file.
 */
CVI_S32 CVI_BIN_ImportBinData(CVI_U8 *pu8Buffer, CVI_U32 u32DataLength);
// -------- If you want to change these interfaces, please contact the isp team. --------

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


