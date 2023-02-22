#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cvi_scene_decode.h"

#define MAX_TEMP_DATA (1024)
static CVI_S64 as64TempData[MAX_TEMP_DATA];

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

static CVI_U32 SCENE_GetNumbersInOneLine(const char * const pszInputLine)
{
	const char  *pszVRBegin = pszInputLine;
	const char  *pszVREnd = pszVRBegin;
	CVI_CHAR    szPart[64] = {0};
	CVI_U32     u32PartCount = 0;
	CVI_U32     u32WholeCount = 0;
	CVI_U32     u32Length = strlen(pszInputLine);
	CVI_U32     u32DataCount = 0;

	memset(as64TempData, 0, sizeof(CVI_S64) * MAX_TEMP_DATA);
	while (pszVREnd != NULL) {
		if (u32WholeCount >= u32Length) {
			break;
		}

		while ((*pszVREnd != '|') && (*pszVREnd != '\0') && (*pszVREnd != ',')) {
			pszVREnd++;
			u32PartCount++;
			u32WholeCount++;
		}

		memcpy(szPart, pszVRBegin, u32PartCount);

		as64TempData[u32DataCount] = strtol(szPart, NULL, 10);
//		printf("%d, as64TempData[%d]=%d, %s\n", u32DataCount, u32DataCount, as64TempData[u32DataCount], szPart);
		memset(szPart, 0, 64);
		u32PartCount = 0;
		pszVREnd++;
		pszVRBegin = pszVREnd;
		u32WholeCount++;
		u32DataCount++;
	}

	return u32DataCount;
}

CVI_S32 SCENE_DecodeS32Array(const char * const pszSrc, CVI_S32 *pas32Dest, CVI_U32 u32DestLength)
{
	if ((!pszSrc) || (!pas32Dest)) {
		return CVI_FAILURE;
	}

	CVI_U32 u32Count = SCENE_GetNumbersInOneLine(pszSrc);

	if (u32Count != u32DestLength) {
		printf("Data Length Miss Match : src(%d) != dest(%d)\nSource ==> %s\n",
			u32Count, u32DestLength, pszSrc);
	}

	for (CVI_U32 u32Idx = 0; u32Idx < MIN(u32Count, u32DestLength); ++u32Idx) {
		pas32Dest[u32Idx] = (CVI_S32)as64TempData[u32Idx];
	}

	return CVI_SUCCESS;
}

CVI_S32 SCENE_DecodeU8Array(const char * const pszSrc, CVI_U8 *pau8Dest, CVI_U32 u32DestLength)
{
	if ((!pszSrc) || (!pau8Dest)) {
		return CVI_FAILURE;
	}

	CVI_U32 u32Count = SCENE_GetNumbersInOneLine(pszSrc);

	if (u32Count != u32DestLength) {
		printf("Data Length Miss Match : src(%d) != dest(%d)\nSource ==> %s\n",
			u32Count, u32DestLength, pszSrc);
	}

	for (CVI_U32 u32Idx = 0; u32Idx < MIN(u32Count, u32DestLength); ++u32Idx) {
		pau8Dest[u32Idx] = (CVI_U8)as64TempData[u32Idx];
	}

	return CVI_SUCCESS;
}

CVI_S32 SCENE_DecodeU16Array(const char * const pszSrc, CVI_U16 *pau16Dest, CVI_U32 u32DestLength)
{
	if ((!pszSrc) || (!pau16Dest)) {
		return CVI_FAILURE;
	}

	CVI_U32 u32Count = SCENE_GetNumbersInOneLine(pszSrc);

	if (u32Count != u32DestLength) {
		printf("Data Length Miss Match : src(%d) != dest(%d)\nSource ==> %s\n",
			u32Count, u32DestLength, pszSrc);
	}

	for (CVI_U32 u32Idx = 0; u32Idx < MIN(u32Count, u32DestLength); ++u32Idx) {
		pau16Dest[u32Idx] = (CVI_U16)as64TempData[u32Idx];
	}

	return CVI_SUCCESS;
}

CVI_S32 SCENE_DecodeU32Array(const char * const pszSrc, CVI_U32 *pau32Dest, CVI_U32 u32DestLength)
{
	if ((!pszSrc) || (!pau32Dest)) {
		return CVI_FAILURE;
	}

	CVI_U32 u32Count = SCENE_GetNumbersInOneLine(pszSrc);

	if (u32Count != u32DestLength) {
		printf("Data Length Miss Match : src(%d) != dest(%d)\nSource ==> %s\n",
			u32Count, u32DestLength, pszSrc);
	}

	for (CVI_U32 u32Idx = 0; u32Idx < MIN(u32Count, u32DestLength); ++u32Idx) {
		pau32Dest[u32Idx] = (CVI_U32)as64TempData[u32Idx];
	}

	return CVI_SUCCESS;
}

CVI_S32 SCENE_DecodeU64Array(const char * const pszSrc, CVI_U64 *pau64Dest, CVI_U32 u32DestLength)
{
	if ((!pszSrc) || (!pau64Dest)) {
		return CVI_FAILURE;
	}

	CVI_U32 u32Count = SCENE_GetNumbersInOneLine(pszSrc);

	if (u32Count != u32DestLength) {
		printf("Data Length Miss Match : src(%d) != dest(%d)\nSource ==> %s\n",
			u32Count, u32DestLength, pszSrc);
	}

	for (CVI_U32 u32Idx = 0; u32Idx < MIN(u32Count, u32DestLength); ++u32Idx) {
		pau64Dest[u32Idx] = (CVI_U64)as64TempData[u32Idx];
	}

	return CVI_SUCCESS;
}

CVI_VOID SCENE_PrintS8Array(const char * const pszSrc, const CVI_S8 * const pas8Data, CVI_U32 u32DataLength)
{
	printf("In : %s\n", pszSrc);
	for (CVI_U32 u32Idx = 0; u32Idx < u32DataLength; ++u32Idx) {
		printf("Out : %2d - %u\n", u32Idx, pas8Data[u32Idx]);
	}
	printf("\n");
}

CVI_VOID SCENE_PrintS32Array(const char * const pszSrc, const CVI_S32 * const pas32Data, CVI_U32 u32DataLength)
{
	printf("In : %s\n", pszSrc);
	for (CVI_U32 u32Idx = 0; u32Idx < u32DataLength; ++u32Idx) {
		printf("Out : %2d - %u\n", u32Idx, pas32Data[u32Idx]);
	}
	printf("\n");
}

CVI_VOID SCENE_PrintU8Array(const char * const pszSrc, const CVI_U8 * const pau8Data, CVI_U32 u32DataLength)
{
	printf("In : %s\n", pszSrc);
	for (CVI_U32 u32Idx = 0; u32Idx < u32DataLength; ++u32Idx) {
		printf("Out : %2d - %u\n", u32Idx, pau8Data[u32Idx]);
	}
	printf("\n");
}

CVI_VOID SCENE_PrintU16Array(const char * const pszSrc, const CVI_U16 * const pau16Data, CVI_U32 u32DataLength)
{
	printf("In : %s\n", pszSrc);
	for (CVI_U32 u32Idx = 0; u32Idx < u32DataLength; ++u32Idx) {
		printf("Out : %2d - %u\n", u32Idx, pau16Data[u32Idx]);
	}
	printf("\n");
}

CVI_VOID SCENE_PrintU32Array(const char * const pszSrc, const CVI_U32 * const pau32Data, CVI_U32 u32DataLength)
{
	printf("In : %s\n", pszSrc);
	for (CVI_U32 u32Idx = 0; u32Idx < u32DataLength; ++u32Idx) {
		printf("Out : %2d - %u\n", u32Idx, pau32Data[u32Idx]);
	}
	printf("\n");
}
