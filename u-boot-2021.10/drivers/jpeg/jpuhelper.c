
#include <linux/string.h>
#include <malloc.h>
#include <common.h>

#include "jpuapi.h"
#include "jpuapifunc.h"
#include "jpuhelper.h"
#include "jpulog.h"
#include "jputable.h"
#include "regdefine.h"

extern int sscanf(const char *str, const char *format, ...);

extern int __must_check kstrtoint(const char *s, unsigned int base, int *res);

static int FillSdramBurst(BufInfo *pBufInfo, Uint64 targetAddr,
			  PhysicalAddress bsBufStartAddr,
			  PhysicalAddress bsBufEndAddr, Uint32 size,
			  int checkeos, int *streameos, int endian);
static int StoreYuvImageBurstFormat(Uint8 *dst, int picWidth, int picHeight,
				    unsigned long addrY, unsigned long addrCb,
				    unsigned long addrCr, int stride,
				    int interLeave, int format, int endian,
				    int packed);
static void ProcessEncodedBitstreamBurst(osal_file_t *fp,
					 unsigned long targetAddr,
					 PhysicalAddress bsBufStartAddr,
					 PhysicalAddress bsBufEndAddr, int size,
					 int endian);
static int LoadYuvImageBurstFormat(unsigned char *src, int picWidth,
				   int picHeight, unsigned long addrY,
				   unsigned long addrCb, unsigned long addrCr,
				   int stride, int interLeave, int format,
				   int endian, int packed);

// Figure A.6 - Zig-zag sequence of quantized DCT coefficients
const int InvScanTable[64] = {
	0,	1,  5,	6,  14, 15, 27, 28, 2,	4,  7,	13, 16, 26, 29, 42,
	3,	8,  12, 17, 25, 30, 41, 43, 9,	11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63
};

const int ScanTable[64] = {
	0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18,
	11, 4,  5,  12, 19, 26, 33, 40, 48, 41, 34, 27, 20,
	13, 6,  7,  14, 21, 28, 35, 42, 49, 56, 57, 50, 43,
	36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45,
	38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

JpgRet WriteJpgBsBufHelperStream(JpgDecHandle handle, BufInfo *pBufInfo,
				 PhysicalAddress paBsBufStart,
				 PhysicalAddress paBsBufEnd, int defaultsize,
				 int checkeos, int *pstreameos, int endian)
{
	JpgRet ret = JPG_RET_SUCCESS;
	int size = 0;
	int fillSize = 0;
	PhysicalAddress paRdPtr, paWrPtr;

	ret = JPU_DecGetBitstreamBuffer(handle, &paRdPtr, &paWrPtr, &size);
	JLOG(INFO, "rdPtr: 0x%lx, wrPtr: 0x%lx, size: %d\n", paRdPtr, paWrPtr, size);
	if (ret != JPG_RET_SUCCESS) {
		JLOG(ERR, "VPU_DecGetBitstreamBuffer failed Error code is 0x%x\n", ret);
		goto FILL_BS_ERROR;
	}

	if (size <= 0)
		return JPG_RET_SUCCESS;

	if (defaultsize) {
		if (size < defaultsize)
			fillSize = size;
		else
			fillSize = defaultsize;
	} else {
		fillSize = size;
	}

	JLOG(INFO, "Before fillSize: %d\n", fillSize);

	fillSize = FillSdramBurst(pBufInfo, paWrPtr, paBsBufStart, paBsBufEnd,
				  fillSize, checkeos, pstreameos, endian);

	if (fillSize == 0 && pBufInfo->fillendbs != 1)
		return ret;
	JLOG(INFO, "After fillSize :%d\n", fillSize);
	if (*pstreameos == 0) {
		ret = JPU_DecUpdateBitstreamBuffer(handle, fillSize);
		if (ret != JPG_RET_SUCCESS) {
			JLOG(ERR, "VPU_DecUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
			goto FILL_BS_ERROR;
		}

		if (pBufInfo->fillendbs == 1) {
			ret = JPU_DecUpdateBitstreamBuffer(handle, STREAM_END_SIZE);
			if (ret != JPG_RET_SUCCESS) {
				JLOG(ERR, "VPU_DecUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
				goto FILL_BS_ERROR;
			}

			//	pBufInfo->fillendbs = 1;
		}
	} else {
		if (!pBufInfo->fillendbs) {
			ret = JPU_DecUpdateBitstreamBuffer(handle, STREAM_END_SIZE);
			if (ret != JPG_RET_SUCCESS) {
				JLOG(ERR, "VPU_DecUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
				goto FILL_BS_ERROR;
			}
			pBufInfo->fillendbs = 1;
		}
	}

FILL_BS_ERROR:

	return ret;
}

JpgRet WriteJpgBsBufHelper(JpgDecHandle handle, BufInfo *pBufInfo,
			   PhysicalAddress paBsBufStart,
			   PhysicalAddress paBsBufEnd, int defaultsize,
			   int checkeos, int *pstreameos, int endian)
{
	JpgRet ret = JPG_RET_SUCCESS;
	int size = 0;
	int fillSize = 0;
	PhysicalAddress paRdPtr, paWrPtr;

	ret = JPU_DecGetBitstreamBuffer(handle, &paRdPtr, &paWrPtr, &size);

	JLOG(INFO, "rdPtr: 0x%lx, wrPtr: 0x%lx, size: %d\n", paRdPtr, paWrPtr, size);

	if (ret != JPG_RET_SUCCESS) {
		JLOG(ERR, "VPU_DecGetBitstreamBuffer failed Error code is 0x%x\n", ret);
		goto FILL_BS_ERROR;
	}

	if (size <= 0)
		return JPG_RET_SUCCESS;

	if (defaultsize) {
		if (size < defaultsize)
			fillSize = size;
		else
			fillSize = defaultsize;
	} else {
		fillSize = size;
	}

	fillSize = FillSdramBurst(pBufInfo, paWrPtr, paBsBufStart, paBsBufEnd,
				  fillSize, checkeos, pstreameos, endian);

	JLOG(INFO, "fillSize :%d\n", fillSize);

	if (*pstreameos == 0) {
		ret = JPU_DecUpdateBitstreamBuffer(handle, fillSize);
		if (ret != JPG_RET_SUCCESS) {
			JLOG(ERR, "VPU_DecUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
			goto FILL_BS_ERROR;
		}

		if ((pBufInfo->size - pBufInfo->point) <= 0) {
			ret = JPU_DecUpdateBitstreamBuffer(handle, STREAM_END_SIZE);
			if (ret != JPG_RET_SUCCESS) {
				JLOG(ERR, "VPU_DecUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
				goto FILL_BS_ERROR;
			}

			pBufInfo->fillendbs = 1;
		}
	} else {
		if (!pBufInfo->fillendbs) {
			ret = JPU_DecUpdateBitstreamBuffer(handle, STREAM_END_SIZE);
			if (ret != JPG_RET_SUCCESS) {
				JLOG(ERR, "VPU_DecUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
				goto FILL_BS_ERROR;
			}
			pBufInfo->fillendbs = 1;
		}
	}

FILL_BS_ERROR:

	return ret;
}

/**
 * Bitstream Read for encoders
 */
JpgRet ReadJpgBsBufHelper(JpgEncHandle handle, osal_file_t *bsFp,
			  PhysicalAddress paBsBufStart,
			  PhysicalAddress paBsBufEnd, int encHeaderSize,
			  int endian)
{
	JpgRet ret = JPG_RET_SUCCESS;
	int loadSize = 0;
	int stuffSize;
	PhysicalAddress paRdPtr, paWrPtr;
	int size = 0;

	ret = JPU_EncGetBitstreamBuffer(handle, &paRdPtr, &paWrPtr, &size);
	if (ret != JPG_RET_SUCCESS) {
		JLOG(ERR, "JPU_EncGetBitstreamBuffer failed Error code is 0x%x\n", ret);
		goto LOAD_BS_ERROR;
	}

	if (size > 0) {
		stuffSize = 0;
		if (encHeaderSize && (size + encHeaderSize) % 8) {
			stuffSize = (size + encHeaderSize) - ((size + encHeaderSize) / 8) * 8;
			stuffSize = 8 - stuffSize;
		}

		loadSize = size;

		if (loadSize > 0) {
			ProcessEncodedBitstreamBurst(bsFp, paRdPtr, paBsBufStart, paBsBufEnd, loadSize, endian);

			ret = JPU_EncUpdateBitstreamBuffer(handle, loadSize);
			if (ret != JPG_RET_SUCCESS) {
				JLOG(ERR, "VPU_EncUpdateBitstreamBuffer failed Error code is 0x%x\n", ret);
				goto LOAD_BS_ERROR;
			}
		}
	}

LOAD_BS_ERROR:

	return ret;
}

/******************************************************************************
 *    DPB Image Data Control
 ******************************************************************************/
int LoadYuvImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv,
			     PhysicalAddress addrY, PhysicalAddress addrCb,
			     PhysicalAddress addrCr, int picWidth,
			     int picHeight, int stride, int interleave,
			     int format, int endian, int packed)
{
	int frameSize = 0;

	switch (format) {
	case FORMAT_420:
		frameSize = picWidth * picHeight * 3 / 2;
		break;
	case FORMAT_224:
		frameSize = picWidth * picHeight * 4 / 2;
		break;
	case FORMAT_422:
		frameSize = picWidth * picHeight * 4 / 2;
		break;
	case FORMAT_444:
		frameSize = picWidth * picHeight * 6 / 2;
		break;
	case FORMAT_400:
		frameSize = picWidth * picHeight;
		break;
	}

	if (packed == PACKED_FORMAT_NONE) {
		frameSize = frameSize;
	} else if (packed == PACKED_FORMAT_444) {
		frameSize = picWidth * 3 * picHeight;
	} else { // PACKED_FORMAT_422_XXXX
		frameSize = picWidth * 2 * picHeight;
	}

	// Load source one picture image to encode to SDRAM frame buffer.
	if (!OSAL_FREAD(pYuv, 1, frameSize, yuvFp)) {
		if (!OSAL_FEOF(yuvFp))
			JLOG(ERR, "Yuv Data fread failed file handle is 0x%p\n", yuvFp);
		return 0;
	}

	LoadYuvImageBurstFormat(pYuv, picWidth, picHeight, addrY, addrCb,
				addrCr, stride, interleave, format, endian,
				packed);

	return 1;
}

int LoadYuvPartialImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv, PhysicalAddress addrY,
				    PhysicalAddress addrCb, PhysicalAddress addrCr, int picWidth, int picHeight,
					int picHeightPartial, int stride, int interleave, int format, int endian,
					int partPosIdx, int frameIdx, int packed)
{
	int LumaPicSize;
	int ChromaPicSize;
	int LumaPartialSize;
	int ChromaPartialSize;
	int pos;
	int divX, divY;
	int frameSize = 0;

	divX = format == FORMAT_420 || format == FORMAT_422 ? 2 : 1;
	divY = format == FORMAT_420 || format == FORMAT_224 ? 2 : 1;

	LumaPicSize = picWidth * picHeight;
	ChromaPicSize = LumaPicSize / divX / divY;

	LumaPartialSize = picWidth * picHeightPartial;
	ChromaPartialSize = LumaPartialSize / divX / divY;

	if (format == FORMAT_400)
		frameSize = LumaPicSize;
	else
		frameSize = LumaPicSize + ChromaPicSize * 2;
	// Load source one picture image to encode to SDRAM frame buffer.

	if (packed) {
		if (packed == PACKED_FORMAT_444) {
			LumaPicSize = picWidth * 3 * picHeight;
			LumaPartialSize = picWidth * 3 * picHeightPartial;
		} else {
			LumaPicSize = picWidth * 2 * picHeight;
			LumaPartialSize = picWidth * 2 * picHeightPartial;
		}
		frameSize = LumaPicSize;
		ChromaPicSize = 0;
		ChromaPartialSize = 0;
	}

	// Y
	OSAL_FSEEK(yuvFp, (frameIdx * frameSize), SEEK_SET);
	pos = LumaPartialSize * partPosIdx;
	OSAL_FSEEK(yuvFp, pos, SEEK_CUR);

	if (!OSAL_FREAD(pYuv, 1, LumaPartialSize, yuvFp)) {
		if (!OSAL_FEOF(yuvFp))
			JLOG(ERR, "Yuv Data fread failed file handle is 0x%p\n", yuvFp);
		return 0;
	}

	if (format != FORMAT_400 && packed == 0) {
		// Cb
		OSAL_FSEEK(yuvFp, (frameIdx * frameSize), SEEK_SET);
		pos = LumaPicSize + ChromaPartialSize * partPosIdx;
		OSAL_FSEEK(yuvFp, pos, SEEK_CUR);

		if (!OSAL_FREAD(pYuv + LumaPartialSize, 1, ChromaPartialSize,
				yuvFp)) {
			if (!OSAL_FEOF(yuvFp))
				JLOG(ERR, "Yuv Data fread failed file handle is 0x%p\n", yuvFp);
			return 0;
		}

		// Cr
		OSAL_FSEEK(yuvFp, (frameIdx * frameSize), SEEK_SET);
		pos = LumaPicSize + ChromaPicSize + ChromaPartialSize * partPosIdx;
		OSAL_FSEEK(yuvFp, pos, SEEK_CUR);

		if (!OSAL_FREAD(pYuv + LumaPartialSize + ChromaPartialSize, 1, ChromaPartialSize, yuvFp)) {
			if (!OSAL_FEOF(yuvFp))
				JLOG(ERR, "Yuv Data fread failed file handle is 0x%p\n", yuvFp);
			return 0;
		}
	}

	LoadYuvImageBurstFormat(pYuv, picWidth, picHeightPartial, addrY, addrCb,
				addrCr, stride, interleave, format, endian,
				packed);

	return 1;
}

int SaveYuvImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv,
			     PhysicalAddress addrY, PhysicalAddress addrCb,
			     PhysicalAddress addrCr, int picWidth,
			     int picHeight, int stride, int interLeave,
			     int format, int endian, int packed)
{
	int frameSize;

	frameSize = StoreYuvImageBurstFormat(pYuv, picWidth, picHeight, addrY,
					     addrCb, addrCr, stride, interLeave,
					     format, endian, packed);

	if (yuvFp) {
		if (!OSAL_FWRITE(pYuv, sizeof(Uint8), frameSize, yuvFp)) {
			JLOG(ERR, "Frame Data fwrite failed file handle is 0x%p\n", yuvFp);
			return 0;
		}
		OSAL_FFLUSH(yuvFp);
	}
	return 1;
}

int SaveYuvPartialImageHelperFormat(osal_file_t *yuvFp, Uint8 *pYuv, PhysicalAddress addrY,
				    PhysicalAddress addrCb, PhysicalAddress addrCr, int picWidth, int picHeight,
					int picHeightPartial, int stride, int interLeave, int format, int endian,
					int partPosIdx, int frameIdx, int packed)
{
	int LumaPicSize = 0;
	int ChromaPicSize = 0;
	int frameSize = 0;

	int LumaPartialSize = 0;
	int ChromaPartialSize = 0;

	int pos = 0;
	//    int divX, divY;

	//	divX = format == FORMAT_420 || format == FORMAT_422 ? 2 : 1;
	//	divY = format == FORMAT_420 || format == FORMAT_224 ? 2 : 1;

	switch (format) {
	case FORMAT_420:
		LumaPicSize = picWidth * ((picHeight + 1) / 2 * 2);
		ChromaPicSize = ((picWidth + 1) / 2) * ((picHeight + 1) / 2);
		frameSize = LumaPicSize + ChromaPicSize * 2;

		LumaPartialSize = picWidth * ((picHeightPartial + 1) / 2 * 2);
		ChromaPartialSize = ((picWidth + 1) / 2) * ((picHeightPartial + 1) / 2);
		if (interLeave)
			ChromaPartialSize = (((picWidth + 1) / 2) * 2) * ((picHeightPartial + 1) / 2);
		break;
	case FORMAT_224:
		LumaPicSize = picWidth * ((picHeight + 1) / 2 * 2);
		ChromaPicSize = picWidth * ((picHeight + 1) / 2);
		frameSize = LumaPicSize + ChromaPicSize * 2;

		LumaPartialSize = picWidth * ((picHeightPartial + 1) / 2 * 2);
		ChromaPartialSize = picWidth * ((picHeightPartial + 1) / 2);
		if (interLeave)
			ChromaPartialSize = picWidth * 2 * ((picHeightPartial + 1) / 2);
		break;
	case FORMAT_422:
		LumaPicSize = picWidth * picHeight;
		ChromaPicSize = ((picWidth + 1) / 2) * picHeight;
		frameSize = LumaPicSize + ChromaPicSize * 2;

		LumaPartialSize = picWidth * picHeightPartial;
		ChromaPartialSize = ((picWidth + 1) / 2) * picHeightPartial;
		if (interLeave)
			ChromaPartialSize = (((picWidth + 1) / 2) * 2) * picHeightPartial;
		break;
	case FORMAT_444:
		LumaPicSize = picWidth * picHeight;
		ChromaPicSize = picWidth * picHeight;
		frameSize = LumaPicSize + ChromaPicSize * 2;

		LumaPartialSize = picWidth * picHeightPartial;
		ChromaPartialSize = picWidth * picHeightPartial;
		if (interLeave)
			ChromaPartialSize = picWidth * 2 * picHeightPartial;
		break;
	case FORMAT_400:
		LumaPicSize = picWidth * picHeight;
		ChromaPicSize = 0;
		frameSize = LumaPicSize + ChromaPicSize * 2;

		LumaPartialSize = picWidth * picHeightPartial;
		ChromaPartialSize = 0;
		break;
	}

	if (packed) {
		if (packed == PACKED_FORMAT_444)
			picWidth *= 3;
		else
			picWidth *= 2;

		LumaPicSize = picWidth * picHeight;
		ChromaPicSize = 0;
		frameSize = LumaPicSize;
		LumaPartialSize = picWidth * picHeightPartial;
		ChromaPartialSize = 0;
	}

	StoreYuvImageBurstFormat(pYuv, picWidth, picHeightPartial, addrY,
				 addrCb, addrCr, stride, interLeave, format,
				 endian, packed);

	if (yuvFp) {
		// Y
		OSAL_FSEEK(yuvFp, (frameIdx * frameSize), SEEK_SET);
		pos = LumaPartialSize * partPosIdx;
		OSAL_FSEEK(yuvFp, pos, SEEK_CUR);
		if (!OSAL_FWRITE(pYuv, sizeof(Uint8), LumaPartialSize, yuvFp)) {
			JLOG(ERR, "Frame Data fwrite failed file handle is 0x%p\n", yuvFp);
			return 0;
		}

		if (packed) {
			OSAL_FFLUSH(yuvFp);
			return 1;
		}

		if (format != FORMAT_400) {
			// Cb
			OSAL_FSEEK(yuvFp, (frameIdx * frameSize), SEEK_SET);
			pos = LumaPicSize + ChromaPartialSize * partPosIdx;
			OSAL_FSEEK(yuvFp, pos, SEEK_CUR);
			if (!OSAL_FWRITE(pYuv + LumaPartialSize, sizeof(Uint8), ChromaPartialSize, yuvFp)) {
				JLOG(ERR, "Frame Data fwrite failed file handle is 0x%p\n", yuvFp);
				return 0;
			}

			if (interLeave) {
				OSAL_FFLUSH(yuvFp);
				return 1;
			}

			// Cr
			OSAL_FSEEK(yuvFp, (frameIdx * frameSize), SEEK_SET);
			pos = LumaPicSize + ChromaPicSize + ChromaPartialSize * partPosIdx;
			OSAL_FSEEK(yuvFp, pos, SEEK_CUR);
			if (!OSAL_FWRITE(pYuv + LumaPartialSize + ChromaPartialSize, sizeof(Uint8),
					 ChromaPartialSize, yuvFp)) {
				JLOG(ERR, "Frame Data fwrite failed file handle is 0x%p\n", yuvFp);
				return 0;
			}
		}
		OSAL_FFLUSH(yuvFp);
	}
	return 1;
}

/******************************************************************************
 *    JPEG specific Helper
 ******************************************************************************/
static int getTblElement(osal_file_t *fp, char *str)
{
	static int LineNum = 1;

	while (1) {
		if (!OSAL_FGETS(str, 256, fp))
			return 0;
		if ((str[0] != ';') && (str[0] != '/'))
			break;
		LineNum++;
	}
	return 1;
}

static int parseHuffmanTable(osal_file_t *fp, EncMjpgParam *param)
{
	int ret = 0;
	char sLine[256] = {
	    0,
	};
	unsigned int h[8] = {
	    0,
	};
	BYTE *huffBit;
	BYTE *huffVal;
	int i, j;

	huffBit = param->huffBits[DC_TABLE_INDEX0];
	huffVal = param->huffVal[DC_TABLE_INDEX0];

	for (i = 0; i < 2; i++) { // Luma DC BitLength
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);

		for (j = 0; j < 8; j++)
			*huffBit++ = (BYTE)h[j];
	}

	for (i = 0; i < (16 / 8); i++) { // Luma DC HuffValue
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);

		for (j = 0; j < 8; j++)
			*huffVal++ = (BYTE)h[j];
	}

	huffBit = param->huffBits[AC_TABLE_INDEX0];
	huffVal = param->huffVal[AC_TABLE_INDEX0];

	for (i = 0; i < (16 / 8); i++) { // Luma AC BitLength
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);
		for (j = 0; j < 8; j++)
			*huffBit++ = (BYTE)h[j];
	}

	for (i = 0; i < (162 / 8); i++) { // Luma DC HuffValue
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);
		for (j = 0; j < 8; j++)
			*huffVal++ = (BYTE)h[j];
	}

	if (getTblElement(fp, sLine) == 0)
		return 0;

	memset(h, 0x00, 8 * sizeof(unsigned int));
	ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2], &h[3],
		     &h[4], &h[5], &h[6], &h[7]);
	for (j = 0; j < 2; j++)
		*huffVal++ = (BYTE)h[j];

	huffBit = param->huffBits[DC_TABLE_INDEX1];
	huffVal = param->huffVal[DC_TABLE_INDEX1];

	for (i = 0; i < (16 / 8); i++) { // Chroma DC BitLength
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);
		for (j = 0; j < 8; j++)
			*huffBit++ = (BYTE)h[j];
	}

	for (i = 0; i < (16 / 8); i++) { // Chroma DC HuffValue
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);
		for (j = 0; j < 8; j++)
			*huffVal++ = (BYTE)h[j];
	}

	huffBit = param->huffBits[AC_TABLE_INDEX1];
	huffVal = param->huffVal[AC_TABLE_INDEX1];

	for (i = 0; i < (16 / 8); i++) { // Chroma DC BitLength
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);
		for (j = 0; j < 8; j++)
			*huffBit++ = (BYTE)h[j];
	}

	for (i = 0; i < (162 / 8); i++) {// Luma DC HuffValue
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);
		for (j = 0; j < 8; j++)
			*huffVal++ = (BYTE)h[j];
	}

	if (getTblElement(fp, sLine) == 0)
		return 0;

	memset(h, 0x00, 8 * sizeof(unsigned int));
	ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2], &h[3],
		     &h[4], &h[5], &h[6], &h[7]);
	for (j = 0; j < 2; j++)
		*huffVal++ = (BYTE)h[j];

	(void)ret;
	return 1;
}

static int parseQMatrix(osal_file_t *fp, EncMjpgParam *param)
{
	int ret = 0;
	char sLine[256] = {
	    0,
	};
	int i, j;
	unsigned int h[8] = {
	    0,
	};
	BYTE temp_dc[64], temp_ac[64];
	BYTE *qTable_dc, *qTable_ac, *qTable;

	qTable = temp_dc;
	for (i = 0; i < (64 / 8); i++) {
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);

		for (j = 0; j < 8; j++)
			*qTable++ = (BYTE)h[j];
	}

	qTable = temp_ac;
	for (i = 0; i < (64 / 8); i++) {
		if (getTblElement(fp, sLine) == 0)
			return 0;

		memset(h, 0x00, 8 * sizeof(unsigned int));
		ret = sscanf(sLine, "%x %x %x %x %x %x %x %x", &h[0], &h[1], &h[2],
			     &h[3], &h[4], &h[5], &h[6], &h[7]);

		for (j = 0; j < 8; j++)
			*qTable++ = (BYTE)h[j];
	}

	qTable_dc = param->qMatTab[DC_TABLE_INDEX0];
	qTable_ac = param->qMatTab[AC_TABLE_INDEX0];

	for (i = 0; i < 64; i++) {
		qTable_dc[InvScanTable[i]] = temp_dc[i];
		qTable_ac[InvScanTable[i]] = temp_ac[i];
	}

	memcpy(param->qMatTab[DC_TABLE_INDEX1], param->qMatTab[DC_TABLE_INDEX0], 64);
	memcpy(param->qMatTab[AC_TABLE_INDEX1], param->qMatTab[AC_TABLE_INDEX0], 64);

	(void)ret;
	return 1;
}

int jpgGetHuffTable(char *huffFileName, EncMjpgParam *param)
{
	char huffFilePath[256];
	osal_file_t *huffFp = NULL;

	if (huffFileName && huffFileName[0] != 0) {
		//	strcpy(huffFilePath, huffFileName);
		huffFp = OSAL_FOPEN(huffFilePath, "rt");
		if (!huffFp) {
			JLOG(ERR, "Can't open Huffman Table file %s\n", huffFilePath);
			return 0;
		}
		parseHuffmanTable(huffFp, param);
		OSAL_FCLOSE(huffFp);
	} else {
		// Rearrange and insert pre-defined Huffman table to deticated
		// variable.
		memcpy(param->huffBits[DC_TABLE_INDEX0], lumaDcBits, 16); // Luma DC BitLength
		memcpy(param->huffVal[DC_TABLE_INDEX0], lumaDcValue, 16); // Luma DC HuffValue

		memcpy(param->huffBits[AC_TABLE_INDEX0], lumaAcBits, 16); // Luma DC BitLength
		memcpy(param->huffVal[AC_TABLE_INDEX0], lumaAcValue, 162); // Luma DC HuffValue

		memcpy(param->huffBits[DC_TABLE_INDEX1], chromaDcBits, 16); // Chroma DC BitLength
		memcpy(param->huffVal[DC_TABLE_INDEX1], chromaDcValue, 16); // Chroma DC HuffValue

		memcpy(param->huffBits[AC_TABLE_INDEX1], chromaAcBits, 16); // Chroma AC BitLength
		memcpy(param->huffVal[AC_TABLE_INDEX1], chromaAcValue, 162); // Chorma AC HuffValue
	}

	return 1;
}

int jpgGetQMatrix(char *qMatFileName, EncMjpgParam *param)
{
	char qMatFilePath[256];
	osal_file_t *qMatFp = NULL;

	if (qMatFileName && qMatFileName[0] != 0) {
		// strcpy(qMatFilePath, qMatFileName);
		qMatFp = OSAL_FOPEN(qMatFilePath, "rt");
		if (!qMatFp) {
			JLOG(ERR, "Can't open Q Matrix file %s\n", qMatFilePath);
			return 0;
		}
		parseQMatrix(qMatFp, param);
		OSAL_FCLOSE(qMatFp);
	} else {
		// Rearrange and insert pre-defined Q-matrix to deticated
		// variable.
		memcpy(param->qMatTab[DC_TABLE_INDEX0], lumaQ2, 64);
		memcpy(param->qMatTab[AC_TABLE_INDEX0], chromaBQ2, 64);

		memcpy(param->qMatTab[DC_TABLE_INDEX1], param->qMatTab[DC_TABLE_INDEX0], 64);
		memcpy(param->qMatTab[AC_TABLE_INDEX1], param->qMatTab[AC_TABLE_INDEX0], 64);
	}

	return 1;
}

/******************************************************************************
 *    EncOpenParam Initialization
 ******************************************************************************/
int getJpgEncOpenParamDefault(JpgEncOpenParam *pEncOP,
			      EncConfigParam *pEncConfig)
{
	int ret;
	EncMjpgParam mjpgParam;

	memset(&mjpgParam, 0x00, sizeof(EncMjpgParam));

	pEncOP->picWidth = pEncConfig->picWidth;
	pEncOP->picHeight = pEncConfig->picHeight;
	pEncOP->sourceFormat = pEncConfig->mjpgChromaFormat;
	pEncOP->restartInterval = 0;
	pEncOP->chroma_interleave = pEncConfig->chroma_interleave;
	pEncOP->packedFormat = pEncConfig->packedFormat;
	mjpgParam.sourceFormat = pEncConfig->mjpgChromaFormat;
	ret = jpgGetHuffTable(pEncConfig->huffFileName, &mjpgParam);
	if (ret == 0)
		return ret;
	ret = jpgGetQMatrix(pEncConfig->qMatFileName, &mjpgParam);
	if (ret == 0)
		return ret;

	memcpy(pEncOP->huffVal, mjpgParam.huffVal, 4 * 162);
	memcpy(pEncOP->huffBits, mjpgParam.huffBits, 4 * 256);
	memcpy(pEncOP->qMatTab, mjpgParam.qMatTab, 4 * 64);

	return 1;
}

/**
 * To init EncOpenParam by CFG file
 * IN
 *   EncConfigParam *pEncConfig
 * OUT
 *   EncOpenParam *pEncOP
 *   char *srcYuvFileName
 */

int getJpgEncOpenParam(JpgEncOpenParam *pEncOP, EncConfigParam *pEncConfig, char *srcYuvFileName)
{
	ENC_CFG encCfg;
	int ret;
	EncMjpgParam mjpgParam;
	char cfgFileName[MAX_FILE_PATH];

	// Source YUV Image File to load
	// strcpy(srcYuvFileName, encCfg.SrcFileName);
	memset(&encCfg, 0x00, sizeof(ENC_CFG));
	memset(&mjpgParam, 0x00, sizeof(EncMjpgParam));

	if (strlen(pEncConfig->strCfgDir))
		sprintf(cfgFileName, "%s%s", pEncConfig->strCfgDir, pEncConfig->cfgFileName);
	// else
	// strcpy(cfgFileName, pEncConfig->cfgFileName);

	parseJpgCfgFile(&encCfg, cfgFileName);

	if (srcYuvFileName) {
		if (strlen(pEncConfig->strStmDir))
			sprintf(srcYuvFileName, "%s%s", pEncConfig->strStmDir, encCfg.SrcFileName);
		//	else
		//		strcpy(srcYuvFileName, encCfg.SrcFileName);
	}

	if (encCfg.FrmFormat == 0)
		pEncConfig->chroma_interleave = CBCR_SEPARATED;
	else if (encCfg.FrmFormat == 1)
		pEncConfig->chroma_interleave = CBCR_INTERLEAVE;
	else if (encCfg.FrmFormat == 2)
		pEncConfig->chroma_interleave = CRCB_INTERLEAVE;
	else
		pEncConfig->chroma_interleave = CBCR_SEPARATED;

	pEncConfig->packedFormat = (encCfg.FrmFormat - 2);
	if (pEncConfig->packedFormat < 0)
		pEncConfig->packedFormat = 0;

	pEncConfig->outNum = encCfg.NumFrame;

	if (pEncOP) {
		pEncOP->picWidth = encCfg.PicX;
		pEncOP->picHeight = encCfg.PicY;
		pEncOP->chroma_interleave = pEncConfig->chroma_interleave;
		pEncOP->packedFormat = pEncConfig->packedFormat;
		pEncOP->sourceFormat = encCfg.SrcFormat;
		pEncOP->restartInterval = encCfg.RstIntval;

		mjpgParam.sourceFormat = encCfg.SrcFormat;
		ret = jpgGetHuffTable(encCfg.HuffTabName, &mjpgParam);
		if (ret == 0)
			return ret;
		ret = jpgGetQMatrix(encCfg.QMatTabName, &mjpgParam);
		if (ret == 0)
			return ret;

		memcpy(pEncOP->huffVal, mjpgParam.huffVal, 4 * 162);
		memcpy(pEncOP->huffBits, mjpgParam.huffBits, 4 * 256);
		memcpy(pEncOP->qMatTab, mjpgParam.qMatTab, 4 * 64);
	}

	return 1;
}

//------------------------------------------------------------------------------
// ENCODE PARAMETER PARSE FUNCSIONS
//------------------------------------------------------------------------------
// Parameter parsing helper
static int GetValue(osal_file_t *fp, char *para, char *value)
{
	static int LineNum = 1;
	char lineStr[256];
	char paraStr[256];
	int ret = 0;

	OSAL_FSEEK(fp, 0, SEEK_SET);

	while (1) {
		if (!OSAL_FGETS(lineStr, 256, fp))
			return 0;
		ret = sscanf(lineStr, "%s %s", paraStr, value);
		if (paraStr[0] != ';') {
			if (strcmp(para, paraStr) == 0)
				return 1;
		}
		LineNum++;
	}

	(void)ret;
	return 1;
}

int parseJpgCfgFile(ENC_CFG *pEncCfg, char *FileName)
{
	osal_file_t *Fp;
	int ret = 0;

	char sLine[256] = {
	    0,
	};

	Fp = OSAL_FOPEN(FileName, "rt");

	if (!Fp) {
		BM_DBG_ERR("   > ERROR: File not exist <%s>\n", FileName);
		return 0;
	}

	// source file name
	if (GetValue(Fp, "YUV_SRC_IMG", sLine) == 0)
		return 0;
	ret = sscanf(sLine, "%s", (char *)&pEncCfg->SrcFileName);

	// frame format
	// ; 0-planar, 1-NV12,NV16(CbCr interleave) 2-NV21,NV61(CbCr
	// alternative) ; 3-YUYV, 4-UYVY, 5-YVYU, 6-VYUY, 7-YUV packed
	// (444 only)
	if (GetValue(Fp, "FRAME_FORMAT", sLine) == 0)
		return 0;
	pEncCfg->FrmFormat = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->FrmFormat);

	// width
	if (GetValue(Fp, "PICTURE_WIDTH", sLine) == 0)
		return 0;
	pEncCfg->PicX = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->PicX);

	// height
	if (GetValue(Fp, "PICTURE_HEIGHT", sLine) == 0)
		return 0;
	pEncCfg->PicY = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->PicY);

	// frame_rate
	if (GetValue(Fp, "FRAME_RATE", sLine) == 0)
		return 0;
	{
		double frameRate = 0.0;
		int timeRes, timeInc;

		// frameRate = (double)(int)atoi(sLine);

		timeInc = 1;
		while ((int)frameRate != frameRate) {
			timeInc *= 10;
			frameRate *= 10;
		}
		timeRes = (int)frameRate;
		// divide 2 or 5
		if (timeInc % 2 == 0 && timeRes % 2 == 0)
			timeInc /= 2, timeRes /= 2;
		if (timeInc % 5 == 0 && timeRes % 5 == 0)
			timeInc /= 5, timeRes /= 5;

		if (timeRes == 2997 && timeInc == 100)
			timeRes = 30000, timeInc = 1001;
		pEncCfg->FrameRate = (timeInc - 1) << 16;
		pEncCfg->FrameRate |= timeRes;
	}

	// frame count
	if (GetValue(Fp, "FRAME_NUMBER_ENCODED", sLine) == 0)
		return 0;

	pEncCfg->NumFrame = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->NumFrame);

	if (GetValue(Fp, "VERSION_ID", sLine) == 0)
		return 0;

	pEncCfg->VersionID = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->VersionID);

	if (GetValue(Fp, "RESTART_INTERVAL", sLine) == 0)
		return 0;

	pEncCfg->RstIntval = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->RstIntval);

	if (GetValue(Fp, "IMG_FORMAT", sLine) == 0)
		return 0;

	pEncCfg->SrcFormat = (int)simple_strtol(sLine, NULL, 10);//sscanf(sLine, "%d", &pEncCfg->SrcFormat);

	if (GetValue(Fp, "QMATRIX_TABLE", sLine) == 0)
		return 0;

	ret = sscanf(sLine, "%s", (char *)&pEncCfg->QMatTabName);

	if (GetValue(Fp, "HUFFMAN_TABLE", sLine) == 0)
		return 0;

	ret = sscanf(sLine, "%s", (char *)&pEncCfg->HuffTabName);

	(void)ret;
	OSAL_FCLOSE(Fp);
	return 1;
}

int FillSdramBurst(BufInfo *pBufInfo, Uint64 targetAddr,
		   PhysicalAddress bsBufStartAddr, PhysicalAddress bsBufEndAddr,
		   Uint32 size, int checkeos, int *streameos, int endian)
{
	Uint8 *pBuf;
	int room;

	pBufInfo->count = 0;

	if (checkeos == 1 && pBufInfo->point >= pBufInfo->size) {
		*streameos = 1;
		return 0;
	}

	if ((pBufInfo->size - pBufInfo->point) < (int)size)
		pBufInfo->count = (pBufInfo->size - pBufInfo->point);
	else
		pBufInfo->count = size;

	pBuf = pBufInfo->buf + pBufInfo->point;

	if ((targetAddr + pBufInfo->count) > bsBufEndAddr) {
		room = bsBufEndAddr - targetAddr;
		JpuWriteMem(targetAddr, pBuf, room, endian);
		JpuWriteMem(bsBufStartAddr, pBuf + room, (pBufInfo->count - room), endian);
	} else {
		JpuWriteMem(targetAddr, pBuf, pBufInfo->count, endian);
	}

	pBufInfo->point += pBufInfo->count;
	return pBufInfo->count;
}

int StoreYuvImageBurstFormat(Uint8 *dst, int picWidth, int picHeight,
			     unsigned long addrY, unsigned long addrCb,
			     unsigned long addrCr, int stride, int interLeave,
			     int format, int endian, int packed)
{
	int size = 0;
	int y = 0, nY = 0, nCb = 0, nCr = 0;
	unsigned long addr = 0;
	int lumaSize = 0, chromaSize = 0, chromaStride = 0, chromaWidth = 0, chromaHeight = 0;

	Uint8 *puc;

	switch (format) {
	case FORMAT_420:
		nY = (picHeight + 1) / 2 * 2;
		nCb = (picHeight + 1) / 2;
		nCr = (picHeight + 1) / 2;
		chromaSize = ((picWidth + 1) / 2) * ((picHeight + 1) / 2);
		chromaStride = stride / 2;
		chromaWidth = (picWidth + 1) / 2;
		chromaHeight = nY;
		break;
	case FORMAT_224:
		nY = (picHeight + 1) / 2 * 2;
		nCb = (picHeight + 1) / 2;
		nCr = (picHeight + 1) / 2;
		chromaSize = (picWidth) * ((picHeight + 1) / 2);
		chromaStride = stride;
		chromaWidth = picWidth;
		chromaHeight = nY;
		break;
	case FORMAT_422:
		nY = picHeight;
		nCb = picHeight;
		nCr = picHeight;
		chromaSize = ((picWidth + 1) / 2) * picHeight;
		chromaStride = stride / 2;
		chromaWidth = (picWidth + 1) / 2;
		chromaHeight = nY * 2;
		break;
	case FORMAT_444:
		nY = picHeight;
		nCb = picHeight;
		nCr = picHeight;
		chromaSize = picWidth * picHeight;
		chromaStride = stride;
		chromaWidth = picWidth;
		chromaHeight = nY * 2;
		break;
	case FORMAT_400:
		nY = picHeight;
		nCb = 0;
		nCr = 0;
		chromaSize = 0;
		chromaStride = 0;
		chromaWidth = 0;
		chromaHeight = 0;
		break;
	}

	puc = dst;
	addr = addrY;

	if (packed) {
		if (packed == PACKED_FORMAT_444)
			picWidth *= 3;
		else
			picWidth *= 2;

		chromaSize = 0;
	}

	lumaSize = picWidth * nY;

	size = lumaSize + chromaSize * 2;

	if (picWidth == stride) {
		JpuReadMem(addr, (Uint8 *)(puc), lumaSize, endian);

		if (packed)
			return size;

		if (interLeave) {
			puc = dst + lumaSize;
			addr = addrCb;
			JpuReadMem(addr, (Uint8 *)(puc), chromaSize * 2,
				   endian);
		} else {
			puc = dst + lumaSize;
			addr = addrCb;
			JpuReadMem(addr, (Uint8 *)(puc), chromaSize, endian);

			puc = dst + lumaSize + chromaSize;
			addr = addrCr;
			JpuReadMem(addr, (Uint8 *)(puc), chromaSize, endian);
		}
	} else {
		for (y = 0; y < nY; ++y) {
			JpuReadMem(addr + stride * y, (Uint8 *)(puc + y * picWidth), picWidth, endian);
		}

		if (packed)
			return size;

		if (interLeave) {
			puc = dst + lumaSize;
			addr = addrCb;
			for (y = 0; y < (chromaHeight / 2); ++y) {
				JpuReadMem(addr + (chromaStride * 2) * y,
					   (Uint8 *)(puc + y * (chromaWidth * 2)),
					   (chromaWidth * 2), endian);
			}
		} else {
			puc = dst + lumaSize;
			addr = addrCb;
			for (y = 0; y < nCb; ++y) {
				JpuReadMem(addr + chromaStride * y,
					   (Uint8 *)(puc + y * chromaWidth),
					   chromaWidth, endian);
			}

			puc = dst + lumaSize + chromaSize;
			addr = addrCr;
			for (y = 0; y < nCr; ++y) {
				JpuReadMem(addr + chromaStride * y,
					   (Uint8 *)(puc + y * chromaWidth),
					   chromaWidth, endian);
			}
		}
	}

	return size;
}

void ProcessEncodedBitstreamBurst(osal_file_t *fp, unsigned long targetAddr,
				  PhysicalAddress bsBufStartAddr,
				  PhysicalAddress bsBufEndAddr, int size,
				  int endian)
{
	Uint8 *val = 0;
	int room = 0;

	BM_DBG_TRACE("target addr :%lx\n", targetAddr);
	BM_DBG_TRACE("start :%lx, end:%lx, size:%d\n", bsBufStartAddr, bsBufEndAddr, size);

	val = (Uint8 *)OSAL_MALLOC(size);
	if ((targetAddr + size) > (unsigned long)bsBufEndAddr) {
		room = bsBufEndAddr - targetAddr;
		JpuReadMem(targetAddr, val, room, endian);
		JpuReadMem(bsBufStartAddr, val + room, (size - room), endian);
	} else {
		BM_DBG_TRACE("read target addr :%lx\n", targetAddr);
		JpuReadMem(targetAddr, val, size, endian);
	}

	if (fp) {
		OSAL_FWRITE(val, sizeof(Uint8), size, fp);
		OSAL_FFLUSH(fp);
	}

	OSAL_FREE(val);
}

int LoadYuvImageBurstFormat(Uint8 *src, int picWidth, int picHeight,
			    unsigned long addrY, unsigned long addrCb,
			    unsigned long addrCr, int stride, int interLeave,
			    int format, int endian, int packed)
{
	int y = 0, nY = 0, nCb = 0, nCr = 0;
	unsigned long addr = 0;
	int size = 0;
	int lumaSize = 0, chromaSize = 0, chromaStride = 0, chromaWidth = 0;
	Uint8 *puc = NULL;

	switch (format) {
	case FORMAT_420:
		nY = picHeight;
		nCb = picHeight / 2;
		nCr = picHeight / 2;
		chromaSize = picWidth * picHeight / 4;
		chromaStride = stride / 2;
		chromaWidth = picWidth / 2;
		break;
	case FORMAT_224:
		nY = picHeight;
		nCb = picHeight / 2;
		nCr = picHeight / 2;
		chromaSize = picWidth * picHeight / 2;
		chromaStride = stride;
		chromaWidth = picWidth;
		break;
	case FORMAT_422:
		nY = picHeight;
		nCb = picHeight;
		nCr = picHeight;
		chromaSize = picWidth * picHeight / 2;
		chromaStride = stride / 2;
		chromaWidth = picWidth / 2;
		break;
	case FORMAT_444:
		nY = picHeight;
		nCb = picHeight;
		nCr = picHeight;
		chromaSize = picWidth * picHeight;
		chromaStride = stride;
		chromaWidth = picWidth;
		break;
	case FORMAT_400:
		nY = picHeight;
		nCb = 0;
		nCr = 0;
		chromaSize = picWidth * picHeight / 4;
		chromaStride = stride / 2;
		chromaWidth = picWidth / 2;
		break;
	}

	puc = src;
	addr = addrY;

	if (packed) {
		if (packed == PACKED_FORMAT_444)
			picWidth *= 3;
		else
			picWidth *= 2;

		chromaSize = 0;
	}

	lumaSize = picWidth * nY;

	size = lumaSize + chromaSize * 2;

	if (picWidth == stride) { // for fast write
		JpuWriteMem(addr, (Uint8 *)(puc), lumaSize, endian);

		if (format == FORMAT_400)
			return size;

		if (packed)
			return size;

		if (interLeave) {
			Uint8 t0, t1, t2, t3, t4, t5, t6, t7;
			int i, height, width;
			int stride;
			Uint8 *pTemp;
			Uint8 *dstAddrCb;
			Uint8 *dstAddrCr;

			addr = addrCb;
			stride = chromaStride * 2;
			height = nCb;
			width = chromaWidth * 2;

			dstAddrCb = (Uint8 *)(puc + picWidth * nY);
			dstAddrCr = (Uint8 *)(dstAddrCb + chromaSize);

			pTemp = OSAL_MALLOC(width);
			if (!pTemp) {
				return 0;
			}

			for (y = 0; y < height; ++y) {
				for (i = 0; i < width; i += 8) {
					t0 = *dstAddrCb++;
					t2 = *dstAddrCb++;
					t4 = *dstAddrCb++;
					t6 = *dstAddrCb++;
					t1 = *dstAddrCr++;
					t3 = *dstAddrCr++;
					t5 = *dstAddrCr++;
					t7 = *dstAddrCr++;
					if (interLeave == CBCR_INTERLEAVE) {
						pTemp[i] = t0;
						pTemp[i + 1] = t1;
						pTemp[i + 2] = t2;
						pTemp[i + 3] = t3;
						pTemp[i + 4] = t4;
						pTemp[i + 5] = t5;
						pTemp[i + 6] = t6;
						pTemp[i + 7] = t7;
					} else { // CRCB_INTERLEAVE
						pTemp[i] = t1;
						pTemp[i + 1] = t0;
						pTemp[i + 2] = t3;
						pTemp[i + 3] = t2;
						pTemp[i + 4] = t5;
						pTemp[i + 5] = t4;
						pTemp[i + 6] = t7;
						pTemp[i + 7] = t6;
					}
				}
				JpuWriteMem(addr + stride * y, (unsigned char *)pTemp, width, endian);
			}

			OSAL_FREE(pTemp);
		} else {
			puc = src + lumaSize;
			addr = addrCb;
			JpuWriteMem(addr, (Uint8 *)puc, chromaSize, endian);

			puc = src + lumaSize + chromaSize;
			addr = addrCr;
			JpuWriteMem(addr, (Uint8 *)puc, chromaSize, endian);
		}
	} else {
		for (y = 0; y < nY; ++y) {
			JpuWriteMem(addr + stride * y, (Uint8 *)(puc + y * picWidth), picWidth, endian);
		}

		if (format == FORMAT_400)
			return size;

		if (packed)
			return size;

		if (interLeave == 1) {
			Uint8 t0, t1, t2, t3, t4, t5, t6, t7;
			int i, width, height, stride;
			Uint8 *pTemp;
			Uint8 *dstAddrCb;
			Uint8 *dstAddrCr;

			addr = addrCb;
			stride = chromaStride * 2;
			height = nCb / 2;
			width = chromaWidth * 2;

			dstAddrCb = (Uint8 *)(puc + picWidth * nY);
			dstAddrCr = (Uint8 *)(dstAddrCb + chromaSize);

			pTemp = OSAL_MALLOC((width + 7) & ~7);
			if (!pTemp) {
				return 0;
			}

			// it may be not occur that pic_width in not 8byte
			// alined.
			for (y = 0; y < height; ++y) {
				for (i = 0; i < width; i += 8) {
					t0 = *dstAddrCb++;
					t2 = *dstAddrCb++;
					t4 = *dstAddrCb++;
					t6 = *dstAddrCb++;
					t1 = *dstAddrCr++;
					t3 = *dstAddrCr++;
					t5 = *dstAddrCr++;
					t7 = *dstAddrCr++;

					if (interLeave == CBCR_INTERLEAVE) {
						pTemp[i] = t0;
						pTemp[i + 1] = t1;
						pTemp[i + 2] = t2;
						pTemp[i + 3] = t3;
						pTemp[i + 4] = t4;
						pTemp[i + 5] = t5;
						pTemp[i + 6] = t6;
						pTemp[i + 7] = t7;
					} else { // CRCB_INTERLEAVE
						pTemp[i] = t1;
						pTemp[i + 1] = t0;
						pTemp[i + 2] = t3;
						pTemp[i + 3] = t2;
						pTemp[i + 4] = t5;
						pTemp[i + 5] = t3;
						pTemp[i + 6] = t7;
						pTemp[i + 7] = t6;
					}

					JpuWriteMem(addr + stride * y, (unsigned char *)pTemp, stride, endian);
				}
			}

		} else {
			puc = src + lumaSize;
			addr = addrCb;
			for (y = 0; y < nCb; ++y) {
				JpuWriteMem(addr + chromaStride * y,
					    (Uint8 *)(puc + y * chromaWidth),
					    chromaWidth, endian);
			}

			puc = src + lumaSize + chromaSize;
			addr = addrCr;
			for (y = 0; y < nCr; ++y) {
				JpuWriteMem(addr + chromaStride * y,
					    (Uint8 *)(puc + y * chromaWidth),
					    chromaWidth, endian);
			}
		}
	}

	return size;
}

void GetMcuUnitSize(int format, int *mcuWidth, int *mcuHeight)
{
	switch (format) {
	case FORMAT_420:
		*mcuWidth = 16;
		*mcuHeight = 16;
		break;
	case FORMAT_422:
		*mcuWidth = 16;
		*mcuHeight = 8;
		break;
	case FORMAT_224:
		*mcuWidth = 8;
		*mcuHeight = 16;
		break;
	default: // FORMAT_444,400
		*mcuWidth = 8;
		*mcuHeight = 8;
		break;
	}
}

unsigned int GetFrameBufSize(int framebufFormat, int picWidth, int picHeight)
{
	unsigned int framebufSize = 0;
	unsigned int framebufWidth, framebufHeight;

	if (framebufFormat == FORMAT_420 || framebufFormat == FORMAT_422)
		framebufWidth = ((picWidth + 15) / 16) * 16;
	else
		framebufWidth = ((picWidth + 7) / 8) * 8;

	if (framebufFormat == FORMAT_420 || framebufFormat == FORMAT_224)
		framebufHeight = ((picHeight + 15) / 16) * 16;
	else
		framebufHeight = ((picHeight + 7) / 8) * 8;

	switch (framebufFormat) {
	case FORMAT_420:
		framebufSize =
		    framebufWidth * ((framebufHeight + 1) / 2 * 2) +
		    ((framebufWidth + 1) / 2) * ((framebufHeight + 1) / 2) * 2;
		break;
	case FORMAT_224:
		framebufSize = framebufWidth * ((framebufHeight + 1) / 2 * 2) +
			       framebufWidth * ((framebufHeight + 1) / 2) * 2;
		break;
	case FORMAT_422:
		framebufSize = framebufWidth * framebufHeight +
			       ((framebufWidth + 1) / 2) * framebufHeight * 2;
		break;
	case FORMAT_444:
		framebufSize = framebufWidth * framebufHeight * 3;
		break;
	case FORMAT_400:
		framebufSize = framebufWidth * framebufHeight;
		break;
	}

	framebufSize = ((framebufSize + 7) & ~7);

	return framebufSize;
}

// inteleave : 0 (chroma separate mode), 1 (cbcr interleave mode), 2 (crcb
// interleave mode)
yuv2rgb_color_format
convert_jpuapi_format_to_yuv2rgb_color_format(int planar_format,
					      int pack_format, int interleave)
{
	yuv2rgb_color_format format = 0;

	if (!pack_format) {
		switch (planar_format) {
		case FORMAT_400:
			format = YUV400;
			break;
		case FORMAT_444:
			format = YUV444;
			break;
		case FORMAT_224:
		case FORMAT_422:
			format = YUV422;
			break;
		case FORMAT_420:
			if (interleave == 0)
				format = YUV420;
			else if (interleave == 1)
				format = NV12;
			else
				format = NV21;
			break;
		}
	} else {
		switch (pack_format) {
		case PACKED_FORMAT_422_YUYV:
			format = YUYV;
			break;
		case PACKED_FORMAT_422_UYVY:
			format = UYVY;
			break;
		case PACKED_FORMAT_422_YVYU:
			format = YVYU;
			break;
		case PACKED_FORMAT_422_VYUY:
			format = VYUY;
			break;
		case PACKED_FORMAT_444:
			format = YYY;
			break;
		}
	}

	return format;
}

void jpu_yuv2rgb(int width, int height, yuv2rgb_color_format format,
		 unsigned char *src, unsigned char *rgba, int cbcr_reverse)
{
#define jpu_clip(var) (((var) >= 255) ? 255 : ((var) <= 0) ? 0 : (var))
	int j, i;
	int c, d, e;

	unsigned char *line = rgba;
	unsigned char *cur = NULL;
	unsigned char *y = NULL;
	unsigned char *u = NULL;
	unsigned char *v = NULL;
	unsigned char *misc = NULL;

	int frame_size_y;
	int frame_size_uv;
	int t_width;

	frame_size_y = width * height;

	if (format == YUV444 || format == RGB_PLANAR)
		frame_size_uv = width * height;
	else if (format == YUV422)
		frame_size_uv = (width * height) >> 1;
	else if (format == YUV420 || format == NV12 || format == NV21)
		frame_size_uv = (width * height) >> 2;
	else
		frame_size_uv = 0;

	t_width = width;

	if (format == YUYV || format == YVYU || format == UYVY || format == VYUY) {
		misc = src;
	} else if (format == NV12 || format == NV21) {
		y = src;
		misc = src + frame_size_y;
	} else if (format == RGB32 || format == RGB24 || format == RGB16) {
		misc = src;
	} else {
		y = src;
		u = src + frame_size_y;
		v = src + frame_size_y + frame_size_uv;
	}

	if (format == YUV444) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				c = y[j * width + i] - 16;
				d = u[j * width + i] - 128;
				e = v[j * width + i] - 128;

				if (!cbcr_reverse) {
					d = u[j * width + i] - 128;
					e = v[j * width + i] - 128;
				} else {
					e = u[j * width + i] - 128;
					e = v[j * width + i] - 128;
				}
				(*cur) = jpu_clip((298 * c + 409 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c + 516 * d + 128) >> 8);
				cur++;
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	} else if (format == YUV422) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				c = y[j * width + i] - 16;
				d = u[j * (width >> 1) + (i >> 1)] - 128;
				e = v[j * (width >> 1) + (i >> 1)] - 128;

				if (!cbcr_reverse) {
					d = u[j * (width >> 1) + (i >> 1)] - 128;
					e = v[j * (width >> 1) + (i >> 1)] - 128;
				} else {
					e = u[j * (width >> 1) + (i >> 1)] - 128;
					d = v[j * (width >> 1) + (i >> 1)] - 128;
				}

				(*cur) = jpu_clip((298 * c + 409 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c + 516 * d + 128) >> 8);
				cur++;
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	} else if (format == YUYV || format == YVYU || format == UYVY || format == VYUY) {
		unsigned char *t = misc;

		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i += 2) {
				switch (format) {
				case YUYV:
					c = *(t) - 16;
					if (!cbcr_reverse) {
						d = *(t + 1) - 128;
						e = *(t + 3) - 128;
					} else {
						e = *(t + 1) - 128;
						d = *(t + 3) - 128;
					}
					break;
				case YVYU:
					c = *(t) - 16;
					if (!cbcr_reverse) {
						d = *(t + 3) - 128;
						e = *(t + 1) - 128;
					} else {
						e = *(t + 3) - 128;
						d = *(t + 1) - 128;
					}
					break;
				case UYVY:
					c = *(t + 1) - 16;
					if (!cbcr_reverse) {
						d = *(t) - 128;
						e = *(t + 2) - 128;
					} else {
						e = *(t) - 128;
						d = *(t + 2) - 128;
					}
					break;
				case VYUY:
					c = *(t + 1) - 16;
					if (!cbcr_reverse) {
						d = *(t + 2) - 128;
						e = *(t) - 128;
					} else {
						e = *(t + 2) - 128;
						d = *(t) - 128;
					}
					break;
				default: // like YUYV
					c = *(t) - 16;
					if (!cbcr_reverse) {
						d = *(t + 1) - 128;
						e = *(t + 3) - 128;
					} else {
						e = *(t + 1) - 128;
						d = *(t + 3) - 128;
					}
					break;
				}

				(*cur) = jpu_clip((298 * c + 409 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c + 516 * d + 128) >> 8);
				cur++;
				(*cur) = 0;
				cur++;

				switch (format) {
				case YUYV:
				case YVYU:
					c = *(t + 2) - 16;
					break;

				case VYUY:
				case UYVY:
					c = *(t + 3) - 16;
					break;
				default: // like YUYV
					c = *(t + 2) - 16;
					break;
				}

				(*cur) = jpu_clip((298 * c + 409 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c + 516 * d + 128) >> 8);
				cur++;
				(*cur) = 0;
				cur++;

				t += 4;
			}
			line += t_width << 2;
		}
	} else if (format == YUV420 || format == NV12 || format == NV21) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				c = y[j * width + i] - 16;
				if (format == YUV420) {
					if (!cbcr_reverse) {
						d = u[(j >> 1) * (width >> 1) + (i >> 1)] - 128;
						e = v[(j >> 1) * (width >> 1) + (i >> 1)] - 128;
					} else {
						e = u[(j >> 1) * (width >> 1) + (i >> 1)] - 128;
						d = v[(j >> 1) * (width >> 1) + (i >> 1)] - 128;
					}
				} else if (format == NV12) {
					if (!cbcr_reverse) {
						d = misc[(j >> 1) * width + (i >> 1 << 1)] - 128;
						e = misc[(j >> 1) * width + (i >> 1 << 1) + 1] - 128;
					} else {
						e = misc[(j >> 1) * width + (i >> 1 << 1)] - 128;
						d = misc[(j >> 1) * width + (i >> 1 << 1) + 1] - 128;
					}
				} else { // if (m_color == NV21)
					if (!cbcr_reverse) {
						d = misc[(j >> 1) * width + (i >> 1 << 1) + 1] - 128;
						e = misc[(j >> 1) * width + (i >> 1 << 1)] - 128;
					} else {
						e = misc[(j >> 1) * width + (i >> 1 << 1) + 1] - 128;
						d = misc[(j >> 1) * width + (i >> 1 << 1)] - 128;
					}
				}
				(*cur) = jpu_clip((298 * c + 409 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
				cur++;
				(*cur) = jpu_clip((298 * c + 516 * d + 128) >> 8);
				cur++;
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	} else if (format == RGB_PLANAR) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				(*cur) = y[j * width + i];
				cur++;
				(*cur) = u[j * width + i];
				cur++;
				(*cur) = v[j * width + i];
				cur++;
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	} else if (format == RGB32) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				(*cur) = misc[j * width * 4 + i];
				cur++; // R
				(*cur) = misc[j * width * 4 + i + 1];
				cur++; // G
				(*cur) = misc[j * width * 4 + i + 2];
				cur++; // B
				(*cur) = misc[j * width * 4 + i + 3];
				cur++; // A
			}
			line += t_width << 2;
		}
	} else if (format == RGB24) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				(*cur) = misc[j * width * 3 + i];
				cur++; // R
				(*cur) = misc[j * width * 3 + i + 1];
				cur++; // G
				(*cur) = misc[j * width * 3 + i + 2];
				cur++; // B
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	} else if (format == RGB16) {
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				int tmp = misc[j * width * 2 + i] << 8 | misc[j * width * 2 + i + 1];
				(*cur) = ((tmp >> 11) & 0x1F << 3);
				cur++; // R(5bit)
				(*cur) = ((tmp >> 5) & 0x3F << 2);
				cur++; // G(6bit)
				(*cur) = ((tmp) & 0x1F << 3);
				cur++; // B(5bit)
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	} else { // YYY
		for (j = 0; j < height; j++) {
			cur = line;
			for (i = 0; i < width; i++) {
				(*cur) = y[j * width + i];
				cur++;
				(*cur) = y[j * width + i];
				cur++;
				(*cur) = y[j * width + i];
				cur++;
				(*cur) = 0;
				cur++;
			}
			line += t_width << 2;
		}
	}
}

int comparateYuv(Uint8 *pYuv, Uint8 *pRefYuv, int picWidth, int picHeight,
		 int stride, int interleave, int format, int endian, int packed)
{
	int size = 0;
	int y = 0, nY = 0, nCb = 0, nCr = 0;
	Uint8 *pRef = NULL;
	int lumaSize = 0, chromaSize = 0, chromaStride = 0, chromaWidth = 0, chromaHeight = 0;

	Uint8 *pOrg = NULL;

	switch (format) {
	case FORMAT_420:
		nY = (picHeight + 1) / 2 * 2;
		nCb =  (picHeight + 1) / 2;
		nCr = (picHeight + 1) / 2;
		chromaSize = ((picWidth + 1) / 2) * ((picHeight + 1) / 2);
		chromaStride = stride / 2;
		chromaWidth = (picWidth + 1) / 2;
		chromaHeight = nY;
		break;
	case FORMAT_224:
		nY = (picHeight + 1) / 2 * 2;
		nCb = (picHeight + 1) / 2;
		nCr = (picHeight + 1) / 2;
		chromaSize = (picWidth) * ((picHeight + 1) / 2);
		chromaStride = stride;
		chromaWidth = picWidth;
		chromaHeight = nY;
		break;
	case FORMAT_422:
		nY = picHeight;
		nCb = picHeight;
		nCr = picHeight;
		chromaSize = ((picWidth + 1) / 2) * picHeight;
		chromaStride = stride / 2;
		chromaWidth = (picWidth + 1) / 2;
		chromaHeight = nY * 2;
		break;
	case FORMAT_444:
		nY = picHeight;
		nCb = picHeight;
		nCr = picHeight;
		chromaSize = picWidth * picHeight;
		chromaStride = stride;
		chromaWidth = picWidth;
		chromaHeight = nY * 2;
		break;
	case FORMAT_400:
		nY = picHeight;
		nCb = 0;
		nCr = 0;
		chromaSize = 0;
		chromaStride = 0;
		chromaWidth = 0;
		chromaHeight = 0;
		break;
	}

	pRef = pRefYuv;
	pOrg = pYuv;

	if (packed) {
		if (packed == PACKED_FORMAT_444)
			picWidth *= 3;
		else
			picWidth *= 2;

		chromaSize = 0;
	}

	lumaSize = picWidth * nY;

	size = lumaSize + chromaSize * 2;

	if (picWidth == stride) {
		if (OSAL_MEMCMP(pRef, pOrg, size)) {
			JLOG(INFO, "pRef = 0x%lx, pOrg = 0x%lx, size:%d\n",
			     (unsigned long)pRef, (unsigned long)pOrg, size);
			return 1;
		} else
			return 0;
	} else {
		for (y = 0; y < nY; ++y) {
			if (OSAL_MEMCMP(pOrg + stride * y, pRef + y * picWidth, picWidth)) {
				JLOG(INFO, "pRef1 = 0x%lx, pOrg1 = 0x%lx\n",
				     (unsigned long)(pRef + y * picWidth),
				     (unsigned long)(pOrg + stride * y));
				return 1;
			}
		}

		if (packed)
			return 0;

		pRef = pRef + lumaSize;
		pOrg = pOrg + nY * stride;
		if (interleave) {
			for (y = 0; y < (chromaHeight / 2); ++y) {
				if (OSAL_MEMCMP(pOrg + (chromaStride * 2) * y,
						pRef + y * (chromaWidth * 2),
						(chromaWidth * 2))) {
					JLOG(INFO, "pRef1 = 0x%lx\n", (unsigned long)(pOrg + (chromaStride * 2) * y));
					JLOG(INFO, "pOrg2 = 0x%lx\n", (unsigned long)(pRef + y * (chromaWidth * 2)));
					return 1;
				}
			}
		} else {
			for (y = 0; y < nCb; ++y) {
				if (OSAL_MEMCMP(pOrg + chromaStride * y, pRef + y * chromaWidth, chromaWidth)) {
					JLOG(INFO,
					     "pRef3 = 0x%lx, pOrg3 = 0x%lx\n",
					     (unsigned long)(pOrg + chromaStride * y),
					     (unsigned long)(pRef + y * chromaWidth));
					return 1;
				}
			}

			pRef = pRef + chromaSize;
			pOrg = pOrg + nCb * chromaStride;
			for (y = 0; y < nCr; ++y) {
				if (OSAL_MEMCMP(pOrg + chromaStride * y, pRef + y * chromaWidth, chromaWidth)) {
					JLOG(INFO,
					     "pRef4 = 0x%lx, pOrg4 = 0x%lx\n",
					     (unsigned long)(pOrg + chromaStride * y),
					     (unsigned long)(pRef + y * chromaWidth));
					return 1;
				}
			}
		}
	}

	return 0;
}
