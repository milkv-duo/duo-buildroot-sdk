#include <stdio.h>
#include "sample_comm.h"
#include "cvi_bin.h"
#include "stdlib.h"

static CVI_S32 _getFileSize(FILE *fp, CVI_U32 *size)
{
	CVI_S32 ret = CVI_SUCCESS;

	fseek(fp, 0L, SEEK_END);
	*size = ftell(fp);
	rewind(fp);

	return ret;
}

CVI_S32 SAMPLE_COMM_BIN_ReadParaFrombin(void)
{
	CVI_S32 ret = CVI_SUCCESS;
	FILE *fp = NULL;
	CVI_U8 *buf = NULL;
	CVI_CHAR binName[BIN_FILE_LENGTH] = { 0 };
	CVI_U32 u32TempLen = 0, u32FileSize = 0;

	ret = CVI_BIN_GetBinName(binName);
	if (ret != CVI_SUCCESS) {
		CVI_TRACE_SYS(CVI_DBG_WARN, "GetBinName(%s) fail\n", binName);
	}

	fp = fopen((const CVI_CHAR *)binName, "rb");
	if (fp == NULL) {
		CVI_TRACE_SYS(CVI_DBG_WARN, "Can't find bin(%s)\n", binName);
		ret = CVI_FAILURE;
		goto ERROR_HANDLER;
	} else {
		CVI_TRACE_SYS(CVI_DBG_WARN, "Bin exist (%s)\n", binName);
	}
	_getFileSize(fp, &u32FileSize);

	buf = (CVI_U8 *)malloc(u32FileSize);
	if (buf == NULL) {
		ret = CVI_FAILURE;
		CVI_TRACE_SYS(CVI_DBG_WARN, "Allocate memory fail\n");
		goto ERROR_HANDLER;
	}
	u32TempLen = fread(buf, u32FileSize, 1, fp);
	if (u32TempLen <= 0) {
		CVI_TRACE_SYS(CVI_DBG_WARN, "read data to buff fail!\n");
		ret = CVI_FAILURE;
		goto ERROR_HANDLER;
	}

	ret = CVI_BIN_ImportBinData(buf, (CVI_U32)u32FileSize);
	if (ret != CVI_SUCCESS) {
		CVI_TRACE_SYS(CVI_DBG_WARN, "CVI_BIN_ImportBinData error! value:(0x%x)\n", ret);
		goto ERROR_HANDLER;
	}

ERROR_HANDLER:
	if (fp != NULL) {
		fclose(fp);
	}
	if (buf != NULL) {
		free(buf);
	}

	return ret;
}

CVI_S32 SAMPLE_COMM_BIN_ReadBlockParaFrombin(enum CVI_BIN_SECTION_ID id)
{
	CVI_S32 ret = CVI_SUCCESS;
	FILE *fp = NULL;
	CVI_U8 *buf = NULL;
	CVI_CHAR binName[BIN_FILE_LENGTH] = {0};
	CVI_U32 u32file_size = 0;

	ret = CVI_BIN_GetBinName(binName);
	if (ret != CVI_SUCCESS) {
		CVI_TRACE_SYS(CVI_DBG_WARN, "GetBinName(%s) fail\n", binName);
	}

	fp = fopen((const CVI_CHAR *)binName, "rb");
	if (fp == NULL) {
		if (id == CVI_BIN_ID_VPSS) {
			CVI_TRACE_SYS(CVI_DBG_WARN, "Can't find bin(%s)\n", binName);
		} else if (id >= CVI_BIN_ID_ISP0 && id <= CVI_BIN_ID_ISP3) {
			CVI_TRACE_SYS(CVI_DBG_WARN, "Can't find bin(%s), use default parameters\n", binName);
		} else {
			CVI_TRACE_SYS(CVI_DBG_WARN, "Can't find bin(%s)\n", binName);
		}
		ret = CVI_FAILURE;
		goto ERROR_HANDLER;
	} else {
		CVI_TRACE_SYS(CVI_DBG_WARN, "Bin exist (%s)\n", binName);
	}
	_getFileSize(fp, &u32file_size);

	buf = (CVI_U8 *)malloc(u32file_size);
	if (buf == NULL) {
		ret = CVI_FAILURE;
		CVI_TRACE_SYS(CVI_DBG_WARN, "Allocate memory fail\n");
		goto ERROR_HANDLER;
	}
	fread(buf, u32file_size, 1, fp);

	if (id >= CVI_BIN_ID_ISP0 && id <= CVI_BIN_ID_ISP3) {
		ret = CVI_BIN_LoadParamFromBin(CVI_BIN_ID_HEADER, buf);
		if (ret != CVI_SUCCESS) {
			CVI_TRACE_SYS(CVI_DBG_WARN, "Bin Version not match, use default parameters\n");
			goto ERROR_HANDLER;
		}
	}

	ret = CVI_BIN_LoadParamFromBin(id, buf);

ERROR_HANDLER:
	if (fp != NULL) {
		fclose(fp);
	}
	if (buf != NULL) {
		free(buf);
	}

	return ret;
}
