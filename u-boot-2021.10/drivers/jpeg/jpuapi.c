
#include "jpuapi.h"
#include "jpuapifunc.h"

#include "jpulog.h"
#include "dm/device.h"
#include "timer.h"

static unsigned char sJpuCompInfoTable[5][24] = {
	{00, 02, 02, 00, 00, 00, 01, 01, 01, 01, 01, 01,
	02, 01, 01, 01, 01, 01, 03, 00, 00, 00, 00, 00}, // 420
	{00, 02, 01, 00, 00, 00, 01, 01, 01, 01, 01, 01,
	02, 01, 01, 01, 01, 01, 03, 00, 00, 00, 00, 00}, // 422H
	{00, 01, 02, 00, 00, 00, 01, 01, 01, 01, 01, 01,
	02, 01, 01, 01, 01, 01, 03, 00, 00, 00, 00, 00}, // 422V
	{00, 01, 01, 00, 00, 00, 01, 01, 01, 01, 01, 01,
	02, 01, 01, 01, 01, 01, 03, 00, 00, 00, 00, 00}, // 444
	{00, 01, 01, 00, 00, 00, 01, 00, 00, 00, 00, 00,
	02, 00, 00, 00, 00, 00, 03, 00, 00, 00, 00, 00}, // 400
};

int JPU_IsBusy(void)
{
	Uint32 val;

	val = JpuReadReg(MJPEG_PIC_STATUS_REG);

	if ((val & (1 << INT_JPU_DONE)) || (val & (1 << INT_JPU_ERROR)))
		return 0;

	return 1;
}

void JPU_ClrStatus(Uint32 val)
{
	if (val != 0)
		JpuWriteReg(MJPEG_PIC_STATUS_REG, val);
}

Uint32 JPU_GetStatus(void)
{
	return JpuReadReg(MJPEG_PIC_STATUS_REG);
}

Uint32 JPU_IsInit(void)
{
	jpu_instance_pool_t *pjip;

	pjip = (jpu_instance_pool_t *)jdi_get_instance_pool();

	if (!pjip)
		return 0;

	return 1;
}

Uint32 JPU_WaitInterrupt(int timeout)
{
	Uint32 reason = 0;
#ifdef SUPPORT_INTERRUPT
	reason = jdi_wait_interrupt(timeout);
	BM_DBG_TRACE("reason = 0x%X\n", reason);
#else

	reason = jdi_wait_interrupt(timeout);
	BM_DBG_TRACE("reason = 0x%X\n", reason);

	JpgSetClockGate(1);

	if (reason != (Uint32)-1)
		reason = JpuReadReg(MJPEG_PIC_STATUS_REG);

	JpgSetClockGate(0);
#endif
	return reason;
}

int JPU_GetOpenInstanceNum(void)
{
	jpu_instance_pool_t *pjip;

	pjip = (jpu_instance_pool_t *)jdi_get_instance_pool();
	if (!pjip)
		return -1;

	return pjip->jpu_instance_num;
}

JpgRet JPU_Init(void)
{
	jpu_instance_pool_t *pjip;

	if (jdi_init() < 0)
		return JPG_RET_FAILURE;

	pjip = (jpu_instance_pool_t *)jdi_get_instance_pool();
	if (!pjip)
		return JPG_RET_FAILURE;

	InitJpgInstancePool();
	JPU_SWReset();
	return JPG_RET_SUCCESS;
}

void JPU_DeInit(void)
{
	jdi_release();
}

JpgRet JPU_GetVersionInfo(Uint32 *versionInfo)
{

	if (JPU_IsInit() == 0) {
		return JPG_RET_NOT_INITIALIZED;
	}

	*versionInfo = API_VERSION;

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecOpen(JpgDecHandle *pHandle, JpgDecOpenParam *pop)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;

	ret = CheckJpgDecOpenParam(pop);
	if (ret != JPG_RET_SUCCESS) {
		return ret;
	}

	ret = GetJpgInstance(&pJpgInst);
	if (ret == JPG_RET_FAILURE) {
		*pHandle = 0;
		JpgLeaveLock();
		return JPG_RET_FAILURE;
	}

	*pHandle = pJpgInst;

	pDecInfo = &pJpgInst->JpgInfo.decInfo;
	memset(pDecInfo, 0x00, sizeof(JpgDecInfo));

	pDecInfo->streamWrPtr = pop->bitstreamBuffer;
	pDecInfo->streamRdPtr = pop->bitstreamBuffer;

	pDecInfo->streamBufStartAddr = pop->bitstreamBuffer;
	pDecInfo->streamBufSize = pop->bitstreamBufferSize;
	pDecInfo->streamBufEndAddr =
	    pop->bitstreamBuffer + pop->bitstreamBufferSize;
	JpuWriteReg(MJPEG_BBC_BAS_ADDR_REG, pDecInfo->streamBufStartAddr);
	JpuWriteReg(MJPEG_BBC_END_ADDR_REG, pDecInfo->streamBufEndAddr);
	JpuWriteReg(MJPEG_BBC_RD_PTR_REG, pDecInfo->streamRdPtr);
	JpuWriteReg(MJPEG_BBC_WR_PTR_REG, pDecInfo->streamWrPtr);
	JpuWriteReg(MJPEG_BBC_STRM_CTRL_REG, 0);

	pDecInfo->pBitStream = pop->pBitStream;
	pDecInfo->streamEndian = pop->streamEndian;
	pDecInfo->frameEndian = pop->frameEndian;
	pDecInfo->chroma_interleave = pop->chroma_interleave;
	pDecInfo->packedFormat = pop->packedFormat;
	pDecInfo->roiEnable = pop->roiEnable;
	pDecInfo->roiWidth = pop->roiWidth;
	pDecInfo->roiHeight = pop->roiHeight;
	pDecInfo->roiOffsetX = pop->roiOffsetX;
	pDecInfo->roiOffsetY = pop->roiOffsetY;

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecClose(JpgDecHandle handle)
{
	JpgInst *pJpgInst;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;

	JpgEnterLock();
	FreeJpgInstance(pJpgInst);
	JpgLeaveLock();

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecGetInitialInfo(JpgDecHandle handle, JpgDecInitialInfo *info)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS) {
		BM_DBG_ERR("CheckJpgInstValidity\n");
		return ret;
	}

	if (info == 0) {
		return JPG_RET_INVALID_PARAM;
	}
	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	if (JpegDecodeHeader(pDecInfo) <= 0) {
		BM_DBG_ERR("JpegDecodeHeader\n");
		return JPG_RET_FAILURE;
	}

	info->picWidth = pDecInfo->picWidth;
	info->picHeight = pDecInfo->picHeight;
	info->minFrameBufferCount = 1;
	info->sourceFormat = pDecInfo->format;
	info->ecsPtr = pDecInfo->ecsPtr;

	JLOG(ERR, "pDecInfo->format %d\n", pDecInfo->format);

	pDecInfo->initialInfoObtained = 1;
	pDecInfo->minFrameBufferNum = 1;

#ifdef MJPEG_ERROR_CONCEAL
	pDecInfo->curRstIdx = 0;
	pDecInfo->nextRstIdx = -1;
#endif

	if (pDecInfo->packedFormat == PACKED_FORMAT_444 &&
	    pDecInfo->format != FORMAT_444) {
		return JPG_RET_INVALID_PARAM;
	}

	if (pDecInfo->roiEnable) {
		pDecInfo->roiMcuWidth = pDecInfo->roiWidth / pDecInfo->mcuWidth;
		pDecInfo->roiMcuHeight = pDecInfo->roiHeight / pDecInfo->mcuHeight;
		pDecInfo->roiMcuOffsetX = pDecInfo->roiOffsetX / pDecInfo->mcuWidth;
		pDecInfo->roiMcuOffsetY = pDecInfo->roiOffsetY / pDecInfo->mcuHeight;

		if (pDecInfo->roiOffsetX > pDecInfo->alignedWidth ||
		    pDecInfo->roiOffsetY > pDecInfo->alignedHeight ||
		    pDecInfo->roiOffsetX + pDecInfo->roiWidth > pDecInfo->alignedWidth ||
		    pDecInfo->roiOffsetY + pDecInfo->roiHeight > pDecInfo->alignedHeight)
			return JPG_RET_INVALID_PARAM;

		if (((pDecInfo->roiOffsetX + pDecInfo->roiWidth) < pDecInfo->mcuWidth) ||
		    ((pDecInfo->roiOffsetY + pDecInfo->roiHeight) < pDecInfo->mcuHeight))
			return JPG_RET_INVALID_PARAM;

		info->roiFrameWidth = pDecInfo->roiMcuWidth * pDecInfo->mcuWidth;
		info->roiFrameHeight = pDecInfo->roiMcuHeight * pDecInfo->mcuHeight;
		info->roiFrameOffsetX = pDecInfo->roiMcuOffsetX * pDecInfo->mcuWidth;
		info->roiFrameOffsetY = pDecInfo->roiMcuOffsetY * pDecInfo->mcuHeight;
		info->roiMCUSize = pDecInfo->mcuWidth;
	}
	info->colorComponents = pDecInfo->compNum;

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecRegisterFrameBuffer(JpgDecHandle handle, FrameBuffer *bufArray,
				  int num, int strideY, int strideC)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	if (!pDecInfo->initialInfoObtained) {
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	if (bufArray == 0) {
		return JPG_RET_INVALID_FRAME_BUFFER;
	}

	if (num < pDecInfo->minFrameBufferNum) {
		return JPG_RET_INSUFFICIENT_FRAME_BUFFERS;
	}
	if (pDecInfo->usePartial && pDecInfo->bufNum == 0) {
		return JPG_RET_INSUFFICIENT_FRAME_BUFFERS;
	}
	if (pDecInfo->usePartial && num < pDecInfo->bufNum) {
		return JPG_RET_INSUFFICIENT_FRAME_BUFFERS;
	}

	if (!pDecInfo->roiEnable) {
		if (strideY < pDecInfo->picWidth >> 3 || strideY % 8 != 0)
			return JPG_RET_INVALID_STRIDE;
	}

	pDecInfo->frameBufPool = bufArray;
	pDecInfo->numFrameBuffers = num;
	pDecInfo->stride = strideY;
	pDecInfo->strideY = strideY;
	pDecInfo->strideC = strideC;

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecGetBitstreamBuffer(JpgDecHandle handle, PhysicalAddress *prdPrt,
				 PhysicalAddress *pwrPtr, int *size)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;
	PhysicalAddress rdPtr;
	PhysicalAddress wrPtr;
	int room;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	if (prdPrt == 0 || pwrPtr == 0 || size == 0) {
		return JPG_RET_INVALID_PARAM;
	}

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	if (GetJpgPendingInst() == pJpgInst) {
		rdPtr = JpuReadReg(MJPEG_BBC_RD_PTR_REG);
	} else {
		rdPtr = pDecInfo->streamRdPtr;
	}

	wrPtr = pDecInfo->streamWrPtr;

	if (wrPtr == pDecInfo->streamBufStartAddr) {
		if (pDecInfo->frameOffset == 0) {
			room = (pDecInfo->streamBufEndAddr -= pDecInfo->streamBufStartAddr);
		} else {
			room = (pDecInfo->frameOffset);
		}
	} else {
		room = (pDecInfo->streamBufEndAddr - wrPtr);
	}

	room = ((room >> 9) << 9); // multiple of 512

	*prdPrt = rdPtr;
	*pwrPtr = wrPtr;

	*size = room;

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecUpdateBitstreamBuffer(JpgDecHandle handle, int size)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	PhysicalAddress wrPtr;
	PhysicalAddress rdPtr;
	JpgRet ret;
	int val = 0;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;
	wrPtr = pDecInfo->streamWrPtr;

	if (size == 0) {
		val = (wrPtr - pDecInfo->streamBufStartAddr) / 256;
		if ((wrPtr - pDecInfo->streamBufStartAddr) % 256)
			val = val + 1;
		if (GetJpgPendingInst() == pJpgInst)
			JpuWriteReg(MJPEG_BBC_STRM_CTRL_REG, (1 << 31 | val));
		pDecInfo->streamEndflag = 1;
		return JPG_RET_SUCCESS;
	}

	JpgSetClockGate(1);

	wrPtr = pDecInfo->streamWrPtr;
	wrPtr += size;

	if (wrPtr == pDecInfo->streamBufEndAddr) {
		wrPtr = pDecInfo->streamBufStartAddr;
	}

	pDecInfo->streamWrPtr = wrPtr;

	if (GetJpgPendingInst() == pJpgInst) {
		rdPtr = JpuReadReg(MJPEG_BBC_RD_PTR_REG);

		if (rdPtr == (pDecInfo->streamBufEndAddr & 0xffffffff)) {
			JpuWriteReg(MJPEG_BBC_CUR_POS_REG, 0);
			JpuWriteReg(MJPEG_GBU_TT_CNT_REG, 0);
			JpuWriteReg(MJPEG_GBU_TT_CNT_REG + 4, 0);
		}

		JpuWriteReg(MJPEG_BBC_WR_PTR_REG, wrPtr);
		if (wrPtr == pDecInfo->streamBufStartAddr) {
			JpuWriteReg(MJPEG_BBC_END_ADDR_REG, pDecInfo->streamBufEndAddr);
		} else {
			JpuWriteReg(MJPEG_BBC_END_ADDR_REG, wrPtr);
		}
	} else {
		rdPtr = pDecInfo->streamRdPtr;
	}

	pDecInfo->streamRdPtr = rdPtr;

	JpgSetClockGate(0);
	return JPG_RET_SUCCESS;
}

JpgRet JPU_SWReset(void)
{
	Uint32 val;
	PhysicalAddress streamBufStartAddr;
	PhysicalAddress streamBufEndAddr;
	PhysicalAddress streamWrPtr;
	PhysicalAddress streamRdPtr;

	JpgEnterLock();

	streamBufStartAddr = JpuReadReg(MJPEG_BBC_BAS_ADDR_REG);
	streamBufEndAddr = JpuReadReg(MJPEG_BBC_END_ADDR_REG);
	streamWrPtr = JpuReadReg(MJPEG_BBC_RD_PTR_REG);
	streamRdPtr = JpuReadReg(MJPEG_BBC_WR_PTR_REG);

	JpuWriteReg(MJPEG_PIC_START_REG, (1 << JPG_START_INIT));

	do {
		val = JpuReadReg(MJPEG_PIC_START_REG);
	} while ((val & (1 << JPG_START_INIT)) == (1 << JPG_START_INIT));

	JpuWriteReg(MJPEG_BBC_BAS_ADDR_REG, streamBufStartAddr);
	JpuWriteReg(MJPEG_BBC_END_ADDR_REG, streamBufEndAddr);
	JpuWriteReg(MJPEG_BBC_RD_PTR_REG, streamRdPtr);
	JpuWriteReg(MJPEG_BBC_WR_PTR_REG, streamWrPtr);

	JpgLeaveLock();

	return JPG_RET_SUCCESS;
}

JpgRet JPU_HWReset(void)
{
	if (jdi_hw_reset() < 0)
		return JPG_RET_FAILURE;

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecIssueStop(JpgDecHandle handle)
{
	JpgInst *pJpgInst;
	//	JpgDecInfo * pDecInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	//	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	if (pJpgInst != GetJpgPendingInst()) {
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	JpgSetClockGate(1);
	JpuWriteReg(MJPEG_PIC_START_REG, 1 << JPG_START_STOP);
	JpgSetClockGate(0);
	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecCompleteStop(JpgDecHandle handle)
{
	JpgInst *pJpgInst;
	//	JpgDecInfo * pDecInfo;
	JpgRet ret;
	Uint32 val;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	//	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	if (pJpgInst != GetJpgPendingInst()) {
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	JpgSetClockGate(1);
	val = JpuReadReg(MJPEG_PIC_STATUS_REG);

	if (val & (1 << INT_JPU_BIT_BUF_STOP)) {
		SetJpgPendingInst(0);
		JpgSetClockGate(0);
	} else {
		JpgSetClockGate(0);
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecSetRdPtr(JpgDecHandle handle, PhysicalAddress addr, int updateWrPtr)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	JpgEnterLock();

	if (GetJpgPendingInst()) {
		JpgLeaveLock();
		return JPG_RET_FRAME_NOT_COMPLETE;
	}

	pDecInfo->streamRdPtr = addr;
	if (updateWrPtr)
		pDecInfo->streamWrPtr = addr;

	pDecInfo->frameOffset = addr - pDecInfo->streamBufStartAddr;
	pDecInfo->consumeByte = 0;

	JpuWriteReg(MJPEG_BBC_RD_PTR_REG, pDecInfo->streamRdPtr);

	JpgLeaveLock();

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecStartOneFrame(JpgDecHandle handle, JpgDecParam *param)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	Uint32 rotMir;
	JpgRet ret;
	Uint32 val;
	int i;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	if (pDecInfo->frameBufPool == 0) { // This means frame buffers have not been registered.
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	rotMir = 0;
	if (pDecInfo->rotationEnable) {
		rotMir |= 0x10; // Enable rotator
		switch (pDecInfo->rotationAngle) {
		case 0:
			rotMir |= 0x0;
			break;

		case 90:
			rotMir |= 0x1;
			break;

		case 180:
			rotMir |= 0x2;
			break;

		case 270:
			rotMir |= 0x3;
			break;
		}
	}

	if (pDecInfo->mirrorEnable) {
		rotMir |= 0x10; // Enable rotator
		switch (pDecInfo->mirrorDirection) {
		case MIRDIR_NONE:
			rotMir |= 0x0;
			break;

		case MIRDIR_VER:
			rotMir |= 0x4;
			break;

		case MIRDIR_HOR:
			rotMir |= 0x8;
			break;

		case MIRDIR_HOR_VER:
			rotMir |= 0xc;
			break;
		}
	}
	JpgEnterLock();

	if (GetJpgPendingInst()) {
		JpgLeaveLock();
		return JPG_RET_FRAME_NOT_COMPLETE;
	}

#ifdef MJPEG_ERROR_CONCEAL
	// Error Concealment
	if (pDecInfo->errInfo.bError) {
		// error conceal main function
		val = JpegDecodeConcealError(pDecInfo);
		if (val == -1) {
			// stream buffer wrap around in error cases.
			pDecInfo->frameOffset = 0;
			pDecInfo->nextOffset = 0;

			// end of stream
			if (pDecInfo->streamEndflag == 1) {
				pDecInfo->frameOffset = -1;
				SetJpgPendingInst(pJpgInst);
				return JPG_RET_EOS;
			}
			// request data
			JpgLeaveLock();
			return JPG_RET_BIT_EMPTY;
		}

		// init GBU
		JpuWriteReg(MJPEG_GBU_TT_CNT_REG, 0);
		JpuWriteReg(MJPEG_GBU_TT_CNT_REG + 4, 0);

		val = pDecInfo->huffAcIdx << 10 | pDecInfo->huffDcIdx << 7 | pDecInfo->userHuffTab << 6;
		val |= (JPU_CHECK_WRITE_RESPONSE_BVALID_SIGNAL << 2) | 0;
		JpuWriteReg(MJPEG_PIC_CTRL_REG, val);
		val = pDecInfo->mcuBlockNum << 16 | pDecInfo->compNum << 12;
		val |= pDecInfo->compInfo[0] << 8 | pDecInfo->compInfo[1] << 4 | pDecInfo->compInfo[2];
		JpuWriteReg(MJPEG_MCU_INFO_REG, val);
		JpuWriteReg(MJPEG_RST_INTVAL_REG, pDecInfo->rstIntval);

		JpgDecGramSetup(pDecInfo);

		JpuWriteReg(MJPEG_DPCM_DIFF_Y_REG, 0);
		JpuWriteReg(MJPEG_DPCM_DIFF_CB_REG, 0);
		JpuWriteReg(MJPEG_DPCM_DIFF_CR_REG, 0);

		JpuWriteReg(MJPEG_GBU_FF_RPTR_REG, pDecInfo->bitPtr);
		JpuWriteReg(MJPEG_GBU_CTRL_REG, 3);

		val = (pDecInfo->setPosX << 16) | (pDecInfo->setPosY);
		JpuWriteReg(MJPEG_PIC_SETMB_REG, val);
		JpuWriteReg(MJPEG_PIC_START_REG, (1 << JPG_START_PIC));

		SetJpgPendingInst(pJpgInst);
		return JPG_RET_SUCCESS;
	}
#endif

	if (pDecInfo->frameOffset < 0) {
		SetJpgPendingInst(pJpgInst);
		return JPG_RET_EOS;
	}

	val = JpegDecodeHeader(pDecInfo);
	if (val == 0) {
		JpgLeaveLock();
		return JPG_RET_FAILURE;
	}

	if (val == -2) { // wrap around case
		pDecInfo->frameOffset = 0;
		pDecInfo->ecsPtr = 0;
#ifdef MJPEG_ERROR_CONCEAL
		pDecInfo->nextOffset = 0;
#endif
		val = JpegDecodeHeader(pDecInfo);
		if (val == 0) {
			JpgLeaveLock();
			return JPG_RET_FAILURE;
		}
	}

	if (val == -1) { // stream empty case
		if (pDecInfo->streamEndflag == 1) {
			SetJpgPendingInst(pJpgInst);
			pDecInfo->frameOffset = -1;
			return JPG_RET_EOS;
		}
		JpgLeaveLock();
		return JPG_RET_BIT_EMPTY;
	}

	if (pDecInfo->streamRdPtr == pDecInfo->streamBufEndAddr) {
		JpuWriteReg(MJPEG_BBC_CUR_POS_REG, 0);
		JpuWriteReg(MJPEG_GBU_TT_CNT_REG, 0);
		JpuWriteReg(MJPEG_GBU_TT_CNT_REG + 4, 0);
	}

	JpuWriteReg(MJPEG_BBC_WR_PTR_REG, pDecInfo->streamWrPtr);
	if (pDecInfo->streamWrPtr == pDecInfo->streamBufStartAddr) {
		JpuWriteReg(MJPEG_BBC_END_ADDR_REG, pDecInfo->streamBufEndAddr);
	} else {
		JpuWriteReg(MJPEG_BBC_END_ADDR_REG, pDecInfo->streamWrPtr);
	}

	JLOG(INFO, "pDecInfo->streamBufStartAddr %p\n",
	     (void *)pDecInfo->streamBufStartAddr);

	JpuWriteReg(MJPEG_BBC_BAS_ADDR_REG, pDecInfo->streamBufStartAddr);

	if (pDecInfo->streamEndflag == 1) {
		val = JpuReadReg(MJPEG_BBC_STRM_CTRL_REG);
		if ((val & (1 << 31)) == 0) {
			val = (pDecInfo->streamWrPtr -
			       pDecInfo->streamBufStartAddr) /
			      256;
			if ((pDecInfo->streamWrPtr -
			     pDecInfo->streamBufStartAddr) %
			    256)
				val = val + 1;

			JpuWriteReg(MJPEG_BBC_STRM_CTRL_REG, (1 << 31 | val));
		}
	} else {
		JpuWriteReg(MJPEG_BBC_STRM_CTRL_REG, 0);
	}

	JpuWriteReg(MJPEG_GBU_TT_CNT_REG, 0);
	JpuWriteReg(MJPEG_GBU_TT_CNT_REG + 4, 0);
	JpuWriteReg(MJPEG_PIC_ERRMB_REG, 0);
	val = pDecInfo->huffAcIdx << 10 | pDecInfo->huffDcIdx << 7 | pDecInfo->userHuffTab << 6;
	val |= ((JPU_CHECK_WRITE_RESPONSE_BVALID_SIGNAL << 2) | pDecInfo->usePartial);
	JpuWriteReg(MJPEG_PIC_CTRL_REG, val);

	JpuWriteReg(MJPEG_PIC_SIZE_REG, (pDecInfo->alignedWidth << 16) | pDecInfo->alignedHeight);
	JpuWriteReg(MJPEG_ROT_INFO_REG, 0);
	JpuWriteReg(MJPEG_OP_INFO_REG, pDecInfo->lineNum << 16 | pDecInfo->bufNum << 3 | pDecInfo->busReqNum);
	val = pDecInfo->mcuBlockNum << 16 | pDecInfo->compNum << 12;
	val |= pDecInfo->compInfo[0] << 8 | pDecInfo->compInfo[1] << 4 | pDecInfo->compInfo[2];
	JpuWriteReg(MJPEG_MCU_INFO_REG, val);

	if (pDecInfo->chroma_interleave == 0)
		val = 0;
	else if (pDecInfo->chroma_interleave == 1)
		val = 2;
	else
		val = 3;
	if (pDecInfo->packedFormat == PACKED_FORMAT_NONE) {
		val |= (pDecInfo->frameEndian << 6) | (0 << 5) | (0 << 4);
		JpuWriteReg(MJPEG_DPB_CONFIG_REG, val);
	} else if (pDecInfo->packedFormat == PACKED_FORMAT_444) {
		val |= (pDecInfo->frameEndian << 6) | (1 << 5) | (0 << 4) | (0 << 2);
		JpuWriteReg(MJPEG_DPB_CONFIG_REG, val);
	} else {
		val |= (pDecInfo->frameEndian << 6) | (0 << 5) | (1 << 4) | ((pDecInfo->packedFormat - 1) << 2);
		JpuWriteReg(MJPEG_DPB_CONFIG_REG, val);
	}
	JpuWriteReg(MJPEG_RST_INTVAL_REG, pDecInfo->rstIntval);
	if (param) {
		if (param->scaleDownRatioWidth > 0)
			pDecInfo->iHorScaleMode = param->scaleDownRatioWidth;
		if (param->scaleDownRatioHeight > 0)
			pDecInfo->iVerScaleMode = param->scaleDownRatioHeight;
	}
	if (pDecInfo->iHorScaleMode | pDecInfo->iVerScaleMode)
		val = ((pDecInfo->iHorScaleMode & 0x3) << 2) |
		      ((pDecInfo->iVerScaleMode & 0x3)) | 0x10;
	else {
		val = 0;
	}
	JpuWriteReg(MJPEG_SCL_INFO_REG, val);

	if (pDecInfo->userHuffTab) {
		if (!JpgDecHuffTabSetUp(pDecInfo)) {
			JpgLeaveLock();
			return JPG_RET_INVALID_PARAM;
		}
	}

	if (!JpgDecQMatTabSetUp(pDecInfo)) {
		JpgLeaveLock();
		return JPG_RET_INVALID_PARAM;
	}

	JpgDecGramSetup(pDecInfo);

	JpuWriteReg(MJPEG_RST_INDEX_REG, 0); // RST index at the beginning.
	JpuWriteReg(MJPEG_RST_COUNT_REG, 0);

	JpuWriteReg(MJPEG_DPCM_DIFF_Y_REG, 0);
	JpuWriteReg(MJPEG_DPCM_DIFF_CB_REG, 0);
	JpuWriteReg(MJPEG_DPCM_DIFF_CR_REG, 0);

	JpuWriteReg(MJPEG_GBU_FF_RPTR_REG, pDecInfo->bitPtr);
	JpuWriteReg(MJPEG_GBU_CTRL_REG, 3);

	JpuWriteReg(MJPEG_ROT_INFO_REG, rotMir);

	if (rotMir & 1) {
		pDecInfo->format = (pDecInfo->format == FORMAT_422)
				       ? FORMAT_224
				       : (pDecInfo->format == FORMAT_224)
					     ? FORMAT_422
					     : pDecInfo->format;
	}

	if (rotMir & 0x10) {
		JpuWriteReg(MJPEG_DPB_BASE00_REG, pDecInfo->rotatorOutput.bufY);
		JpuWriteReg(MJPEG_DPB_BASE01_REG,
			    pDecInfo->rotatorOutput.bufCb);
		JpuWriteReg(MJPEG_DPB_BASE02_REG,
			    pDecInfo->rotatorOutput.bufCr);
	} else if (pDecInfo->usePartial) {
		PhysicalAddress addr;

		val = (pDecInfo->frameIdx % (pDecInfo->numFrameBuffers / pDecInfo->bufNum));
		for (i = 0; i < pDecInfo->bufNum; i++) {
			addr = pDecInfo->frameBufPool[(val * pDecInfo->bufNum) + i].bufY;
			JpuWriteReg(MJPEG_DPB_BASE00_REG + (i * 12), addr);
			addr = pDecInfo->frameBufPool[(val * pDecInfo->bufNum) + i].bufCb;
			JpuWriteReg(MJPEG_DPB_BASE01_REG + (i * 12), addr);
			addr = pDecInfo->frameBufPool[(val * pDecInfo->bufNum) + i].bufCr;
			JpuWriteReg(MJPEG_DPB_BASE02_REG + (i * 12), addr);
		}
	} else {
		val = (pDecInfo->frameIdx % pDecInfo->numFrameBuffers);
		JpuWriteReg(MJPEG_DPB_BASE00_REG, pDecInfo->frameBufPool[val].bufY);
		JpuWriteReg(MJPEG_DPB_BASE01_REG, pDecInfo->frameBufPool[val].bufCb);
		JpuWriteReg(MJPEG_DPB_BASE02_REG, pDecInfo->frameBufPool[val].bufCr);
	}

	if (pDecInfo->rotationEnable) {
		JpuWriteReg(MJPEG_DPB_YSTRIDE_REG, pDecInfo->rotatorStride);
		val = (pDecInfo->format == FORMAT_420 ||
		       pDecInfo->format == FORMAT_422 ||
		       pDecInfo->format == FORMAT_400)
			  ? 2
			  : 1;
		if (pDecInfo->chroma_interleave)
			JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, (pDecInfo->rotatorStride / (int)val) * 2);
		else
			JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, pDecInfo->rotatorStride / (int)val);
	} else {

		CVI_JPG_DBG("packedFormat = %d\n", pDecInfo->packedFormat);
		if (pDecInfo->packedFormat == PACKED_FORMAT_NONE && pDecInfo->format == FORMAT_420) {
			CVI_JPG_DBG("strideY = %d, strideC = %d\n", pDecInfo->strideY, pDecInfo->strideC);
			JpuWriteReg(MJPEG_DPB_YSTRIDE_REG, pDecInfo->strideY);
			JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, pDecInfo->strideC);
		} else {
			JpuWriteReg(MJPEG_DPB_YSTRIDE_REG, pDecInfo->stride);
			if (pDecInfo->chroma_interleave)
				JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, (pDecInfo->stride / (int)val) * 2);
			else
				JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, pDecInfo->stride / (int)val);
		}
	}
	if (pDecInfo->roiEnable) {
		JpuWriteReg(MJPEG_CLP_INFO_REG, 1);
		JpuWriteReg(MJPEG_CLP_BASE_REG, pDecInfo->roiOffsetX << 16 | pDecInfo->roiOffsetY); // pixel unit
		val = (pDecInfo->roiMcuWidth * pDecInfo->mcuWidth) << 16;
		val |= pDecInfo->roiMcuHeight * pDecInfo->mcuHeight;
		JpuWriteReg(MJPEG_CLP_SIZE_REG, val); // pixel Unit
	} else {
		JpuWriteReg(MJPEG_CLP_INFO_REG, 0);
	}

	if (pJpgInst->loggingEnable)
		jdi_log(JDI_LOG_CMD_PICRUN, 1);

	BM_DBG_FLOW("JPG_START_PIC\n");

#ifdef PROFILE_PERFORMANCE
	timer_meter_start();
#endif

	JpuWriteReg(MJPEG_PIC_STATUS_REG, JpuReadReg(MJPEG_PIC_STATUS_REG));

	JpuWriteReg(MJPEG_PIC_START_REG, (1 << JPG_START_PIC));

	SetJpgPendingInst(pJpgInst);

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecGetOutputInfo(JpgDecHandle handle, JpgDecOutputInfo *info)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;
	Uint32 val = 0;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return ret;
	}

	if (info == 0) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return JPG_RET_INVALID_PARAM;
	}

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;

	val = JpuReadReg(MJPEG_PIC_STATUS_REG);

	if (pJpgInst != GetJpgPendingInst()) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	if (pDecInfo->frameOffset < 0) {
		info->numOfErrMBs = 0;
		info->decodingSuccess = 1;
		info->indexFrameDisplay = -1;
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return JPG_RET_SUCCESS;
	}

	if (pDecInfo->roiEnable) {
		info->decPicWidth = pDecInfo->roiMcuWidth * pDecInfo->mcuWidth;
		info->decPicHeight =
		    pDecInfo->roiMcuHeight * pDecInfo->mcuHeight;
	} else {
		info->decPicWidth = pDecInfo->alignedWidth;
		info->decPicHeight = pDecInfo->alignedHeight;
	}

	info->decPicWidth >>= pDecInfo->iHorScaleMode;
	info->decPicHeight >>= pDecInfo->iVerScaleMode;

	info->indexFrameDisplay =
	    (pDecInfo->frameIdx % pDecInfo->numFrameBuffers);
#ifdef MJPEG_ERROR_CONCEAL
	info->consumedByte =
	    pDecInfo->gbuStartPtr + (JpuReadReg(MJPEG_GBU_TT_CNT_REG)) / 8;
#else
	info->consumedByte = (JpuReadReg(MJPEG_GBU_TT_CNT_REG)) / 8;
#endif
	pDecInfo->streamRdPtr = JpuReadReg(MJPEG_BBC_RD_PTR_REG);
	pDecInfo->consumeByte = info->consumedByte - 16 - pDecInfo->ecsPtr;
	info->bytePosFrameStart = pDecInfo->frameOffset;
	info->ecsPtr = pDecInfo->ecsPtr;

	pDecInfo->ecsPtr = 0;
	pDecInfo->frameIdx++;

	val = JpuReadReg(MJPEG_PIC_STATUS_REG);

	if (val & (1 << INT_JPU_DONE)) {
		info->decodingSuccess = 1;
		info->numOfErrMBs = 0;

#ifdef MJPEG_ERROR_CONCEAL
		pDecInfo->errInfo.bError = 0;
		pDecInfo->nextOffset = 0;
		pDecInfo->gbuStartPtr = 0;
#endif
	} else if (val & (1 << INT_JPU_ERROR)) {

		info->numOfErrMBs = JpuReadReg(MJPEG_PIC_ERRMB_REG);
		info->decodingSuccess = 0;

#ifdef MJPEG_ERROR_CONCEAL
		//			info->numOfErrMBs =
		// JpuReadReg(MJPEG_PIC_ERRMB_REG);
		pDecInfo->errInfo.bError = 1;
		pDecInfo->errInfo.errPosY = info->numOfErrMBs & 0xFFF;
		pDecInfo->errInfo.errPosX = (info->numOfErrMBs >> 12) & 0xFFF;

		// set search point to find next rstMarker from origin of frame
		// buffer by host
		pDecInfo->nextOffset = (info->consumedByte) & (~7);

		// prevent to find same position.
		if (pDecInfo->currOffset == pDecInfo->nextOffset)
			pDecInfo->nextOffset += JPU_GBU_SIZE;
#endif
	}

	if (val != 0)
		JpuWriteReg(MJPEG_PIC_STATUS_REG, val);

	if (pJpgInst->loggingEnable) {
		jdi_log(JDI_LOG_CMD_PICRUN, 0);
	}

	SetJpgPendingInst(0);
	JpgLeaveLock();

	return JPG_RET_SUCCESS;
}

JpgRet JPU_DecGiveCommand(JpgDecHandle handle, JpgCommand cmd, void *param)
{
	JpgInst *pJpgInst;
	JpgDecInfo *pDecInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pDecInfo = &pJpgInst->JpgInfo.decInfo;
	switch (cmd) {
	case ENABLE_JPG_ROTATION: {
		if (pDecInfo->roiEnable) {
			return JPG_RET_INVALID_PARAM;
		}

		if (pDecInfo->rotatorStride == 0) {
			return JPG_RET_ROTATOR_STRIDE_NOT_SET;
		}
		pDecInfo->rotationEnable = 1;
		break;
	}

	case DISABLE_JPG_ROTATION: {
		pDecInfo->rotationEnable = 0;
		break;
	}

	case ENABLE_JPG_MIRRORING: {
		if (pDecInfo->rotatorStride == 0) {
			return JPG_RET_ROTATOR_STRIDE_NOT_SET;
		}
		pDecInfo->mirrorEnable = 1;
		break;
	}
	case DISABLE_JPG_MIRRORING: {
		pDecInfo->mirrorEnable = 0;
		break;
	}
	case SET_JPG_MIRROR_DIRECTION: {

		JpgMirrorDirection mirDir;

		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}
		mirDir = *(JpgMirrorDirection *)param;
		if (!(mirDir >= MIRDIR_NONE && mirDir <= MIRDIR_HOR_VER)) {
			return JPG_RET_INVALID_PARAM;
		}
		pDecInfo->mirrorDirection = mirDir;

		break;
	}
	case SET_JPG_ROTATION_ANGLE: {
		int angle;

		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}

		angle = *(int *)param;
		if (angle != 0 && angle != 90 && angle != 180 && angle != 270) {
			return JPG_RET_INVALID_PARAM;
		}

		pDecInfo->rotationAngle = angle;
		break;
	}

	case SET_JPG_ROTATOR_OUTPUT: {
		FrameBuffer *frame;

		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}
		frame = (FrameBuffer *)param;
		pDecInfo->rotatorOutput = *frame;
		pDecInfo->rotatorOutputValid = 1;
		break;
	}

	case SET_JPG_ROTATOR_STRIDE: {
		int stride;

		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}
		stride = *(int *)param;
		if (stride % 8 != 0 || stride == 0) {
			return JPG_RET_INVALID_STRIDE;
		}

		if (pDecInfo->rotationAngle == 90 ||
		    pDecInfo->rotationAngle == 270) {
			if (pDecInfo->alignedHeight > stride) {
				return JPG_RET_INVALID_STRIDE;
			}
		} else {
			if (pDecInfo->alignedWidth > stride) {
				return JPG_RET_INVALID_STRIDE;
			}
		}

		pDecInfo->rotatorStride = stride;
		break;
	}
	case SET_JPG_SCALE_HOR: {
		int scale;

		scale = *(int *)param;
		if (pDecInfo->alignedWidth < 128 ||
		    pDecInfo->alignedHeight < 128) {
			if (scale) {
				return JPG_RET_INVALID_PARAM;
			}
		}

		pDecInfo->iHorScaleMode = scale;
		break;
	}
	case SET_JPG_SCALE_VER: {
		int scale;

		scale = *(int *)param;
		if (pDecInfo->alignedWidth < 128 ||
		    pDecInfo->alignedHeight < 128) {
			if (scale) {
				return JPG_RET_INVALID_PARAM;
			}
		}
		pDecInfo->iVerScaleMode = scale;
		break;
	}
	case SET_JPG_USE_PARTIAL_MODE: {
		int enable;

		enable = *(int *)param;
		pDecInfo->usePartial = enable;

		break;
	}
	case SET_JPG_PARTIAL_FRAME_NUM: {
		int frame;

		if (pDecInfo->stride != 0) {
			return JPG_RET_WRONG_CALL_SEQUENCE;
		}

		frame = *(int *)param;
		pDecInfo->bufNum = frame;

		break;
	}
	case SET_JPG_PARTIAL_LINE_NUM: {
		int line;

		if (pDecInfo->stride != 0) {
			return JPG_RET_WRONG_CALL_SEQUENCE;
		}
		line = *(int *)param;
		pDecInfo->lineNum = line;

		break;
	}
	case ENABLE_LOGGING: {
		pJpgInst->loggingEnable = 1;
	} break;
	case DISABLE_LOGGING: {
		pJpgInst->loggingEnable = 0;
	} break;
	default:
		return JPG_RET_INVALID_COMMAND;
	}
	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncOpen(JpgEncHandle *pHandle, JpgEncOpenParam *pop)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	JpgRet ret;
	int i;

	ret = CheckJpgEncOpenParam(pop);
	if (ret != JPG_RET_SUCCESS) {
		BM_DBG_ERR("CheckJpgEncOpenParam\n");
		return ret;
	}

	JpgEnterLock();
	ret = GetJpgInstance(&pJpgInst);
	if (ret == JPG_RET_FAILURE) {
		BM_DBG_ERR("GetJpgInstance\n");
		*pHandle = 0;
		JpgLeaveLock();
		return JPG_RET_FAILURE;
	}

	*pHandle = pJpgInst;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;
	memset(pEncInfo, 0x00, sizeof(JpgEncInfo));
	pEncInfo->openParam = *pop;
	pEncInfo->streamRdPtr = pop->bitstreamBuffer;
	pEncInfo->streamWrPtr = pop->bitstreamBuffer;

	JpuWriteReg(MJPEG_BBC_WR_PTR_REG, pEncInfo->streamWrPtr);
	pEncInfo->streamBufStartAddr = pop->bitstreamBuffer;
	pEncInfo->streamBufSize = pop->bitstreamBufferSize;
	pEncInfo->streamBufEndAddr =
	    pop->bitstreamBuffer + pop->bitstreamBufferSize;
	pEncInfo->streamEndian = pop->streamEndian;
	pEncInfo->frameEndian = pop->frameEndian;
	pEncInfo->chroma_interleave = pop->chroma_interleave;

	pEncInfo->format = pEncInfo->openParam.sourceFormat;
	pEncInfo->picWidth = pEncInfo->openParam.picWidth;
	pEncInfo->picHeight = pEncInfo->openParam.picHeight;
	// Picture size alignment
	if (pEncInfo->format == FORMAT_420 || pEncInfo->format == FORMAT_422)
		pEncInfo->alignedWidth = ((pEncInfo->picWidth + 15) / 16) * 16;
	else
		pEncInfo->alignedWidth = ((pEncInfo->picWidth + 7) / 8) * 8;

	if (pEncInfo->format == FORMAT_420 || pEncInfo->format == FORMAT_224)
		pEncInfo->alignedHeight =
		    ((pEncInfo->picHeight + 15) / 16) * 16;
	else
		pEncInfo->alignedHeight = ((pEncInfo->picHeight + 7) / 8) * 8;
	pEncInfo->rstIntval = pEncInfo->openParam.restartInterval;

	for (i = 0; i < 4; i++)
		pEncInfo->pHuffVal[i] = pEncInfo->openParam.huffVal[i];
	for (i = 0; i < 4; i++)
		pEncInfo->pHuffBits[i] = pEncInfo->openParam.huffBits[i];
	for (i = 0; i < 4; i++)
		pEncInfo->pQMatTab[i] = pEncInfo->openParam.qMatTab[i];

	pEncInfo->pCInfoTab[0] = sJpuCompInfoTable[/*2*/ pEncInfo->format];
	pEncInfo->pCInfoTab[1] = pEncInfo->pCInfoTab[0] + 6;
	pEncInfo->pCInfoTab[2] = pEncInfo->pCInfoTab[1] + 6;
	pEncInfo->pCInfoTab[3] = pEncInfo->pCInfoTab[2] + 6;

	if (pop->packedFormat == PACKED_FORMAT_444 &&
	    pEncInfo->format != FORMAT_444) {
		BM_DBG_ERR("PACKED_FORMAT_444\n");
		return JPG_RET_INVALID_PARAM;
	}
	pEncInfo->packedFormat = pop->packedFormat;

	JpgLeaveLock();

	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncClose(JpgEncHandle handle)
{
	JpgInst *pJpgInst;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	JpgEnterLock();

	if (GetJpgPendingInst()) {
		JpgLeaveLock();
		return JPG_RET_FRAME_NOT_COMPLETE;
	}

	pJpgInst = handle;
	JpuWriteReg(MJPEG_BBC_FLUSH_CMD_REG, 0);
	FreeJpgInstance(pJpgInst);
	JpgLeaveLock();

	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncGetInitialInfo(JpgEncHandle handle, JpgEncInitialInfo *info)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	if (info == 0) {
		return JPG_RET_INVALID_PARAM;
	}

	pJpgInst = handle;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	if (pEncInfo->initialInfoObtained) {
		return JPG_RET_CALLED_BEFORE;
	}

	JpgEnterLock();

	if (GetJpgPendingInst()) {
		JpgLeaveLock();
		return JPG_RET_FRAME_NOT_COMPLETE;
	}

	if (pEncInfo->format == FORMAT_400) {
		pEncInfo->compInfo[1] = 0;
		pEncInfo->compInfo[2] = 0;
	} else {
		pEncInfo->compInfo[1] = 5;
		pEncInfo->compInfo[2] = 5;
	}

	if (pEncInfo->format == FORMAT_400)
		pEncInfo->compNum = 1;
	else
		pEncInfo->compNum = 3;

	if (pEncInfo->format == FORMAT_420) {
		pEncInfo->mcuBlockNum = 6;
		pEncInfo->compInfo[0] = 10;
		pEncInfo->busReqNum = 2;
	} else if (pEncInfo->format == FORMAT_422) {
		pEncInfo->mcuBlockNum = 4;
		pEncInfo->busReqNum = 3;
		pEncInfo->compInfo[0] = 9;
	} else if (pEncInfo->format == FORMAT_224) {
		pEncInfo->mcuBlockNum = 4;
		pEncInfo->busReqNum = 3;
		pEncInfo->compInfo[0] = 6;
	} else if (pEncInfo->format == FORMAT_444) {
		pEncInfo->mcuBlockNum = 3;
		pEncInfo->compInfo[0] = 5;
		pEncInfo->busReqNum = 4;
	} else if (pEncInfo->format == FORMAT_400) {
		pEncInfo->mcuBlockNum = 1;
		pEncInfo->busReqNum = 4;
		pEncInfo->compInfo[0] = 5;
	}

	info->minFrameBufferCount = 0;
	info->colorComponents = pEncInfo->compNum;

	pEncInfo->initialInfo = *info;
	pEncInfo->initialInfoObtained = 1;

	JpgLeaveLock();
	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncGetBitstreamBuffer(JpgEncHandle handle, PhysicalAddress *prdPrt,
				 PhysicalAddress *pwrPtr, int *size)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	if (prdPrt == 0 || pwrPtr == 0 || size == 0) {
		return JPG_RET_INVALID_PARAM;
	}

	pJpgInst = handle;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	JpgSetClockGate(1);
	*prdPrt = pEncInfo->streamRdPtr;

	if (GetJpgPendingInst() == pJpgInst) {
		*pwrPtr = JpuReadReg(MJPEG_BBC_WR_PTR_REG);
	} else {
		*pwrPtr = pEncInfo->streamWrPtr;
	}
	*size = *pwrPtr - *prdPrt;
	JpgSetClockGate(0);

	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncUpdateBitstreamBuffer(JpgEncHandle handle, int size)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	PhysicalAddress rdPtr;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	JpgSetClockGate(1);
	rdPtr = pEncInfo->streamRdPtr;
	rdPtr += size;

	if (rdPtr == pEncInfo->streamBufEndAddr) {
		rdPtr = pEncInfo->streamBufStartAddr;
	}

	pEncInfo->streamRdPtr = pEncInfo->streamBufStartAddr;

	if (GetJpgPendingInst() == pJpgInst) {
		pEncInfo->streamWrPtr = JpuReadReg(MJPEG_BBC_WR_PTR_REG);
		JpuWriteReg(MJPEG_BBC_CUR_POS_REG, 0);
		JpuWriteReg(MJPEG_BBC_EXT_ADDR_REG,
			    pEncInfo->streamBufStartAddr);
		JpuWriteReg(MJPEG_BBC_RD_PTR_REG, pEncInfo->streamBufStartAddr);
		JpuWriteReg(MJPEG_BBC_WR_PTR_REG, pEncInfo->streamBufStartAddr);
	}
	JpgSetClockGate(0);
	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncIssueStop(JpgEncHandle handle)
{
	JpgInst *pJpgInst;
	//	JpgEncInfo *pEncInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	//	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	if (pJpgInst != GetJpgPendingInst()) {
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	JpgSetClockGate(1);
	JpuWriteReg(MJPEG_PIC_START_REG, 1 << JPG_START_STOP);
	JpgSetClockGate(0);
	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncCompleteStop(JpgEncHandle handle)
{
	JpgInst *pJpgInst;
	//	JpgEncInfo *pEncInfo;
	JpgRet ret;
	Uint32 val;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	//	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	if (pJpgInst != GetJpgPendingInst()) {
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	JpgSetClockGate(1);
	val = JpuReadReg(MJPEG_PIC_STATUS_REG);

	if (val & (1 << INT_JPU_BIT_BUF_STOP)) {
		SetJpgPendingInst(0);
		JpgSetClockGate(0);
	} else {
		JpgSetClockGate(0);
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncStartOneFrame(JpgEncHandle handle, JpgEncParam *param)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	FrameBuffer *pBasFrame;
	Uint32 rotMirEnable;
	Uint32 rotMirMode;
	JpgRet ret;
	Uint32 val;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	ret = CheckJpgEncParam(handle, param);
	if (ret != JPG_RET_SUCCESS) {
		return ret;
	}

	pBasFrame = param->sourceFrame;
	rotMirEnable = 0;
	rotMirMode = 0;
	if (pEncInfo->rotationEnable) {
		rotMirEnable = 0x10; // Enable rotator
		switch (pEncInfo->rotationAngle) {
		case 0:
			rotMirMode |= 0x0;
			break;

		case 90:
			rotMirMode |= 0x1;
			break;

		case 180:
			rotMirMode |= 0x2;
			break;

		case 270:
			rotMirMode |= 0x3;
			break;
		}
	}
	if (pEncInfo->mirrorEnable) {
		rotMirEnable = 0x10; // Enable rotator

		switch (pEncInfo->mirrorDirection) {
		case MIRDIR_NONE:
			rotMirMode |= 0x0;
			break;

		case MIRDIR_VER:
			rotMirMode |= 0x4;
			break;

		case MIRDIR_HOR:
			rotMirMode |= 0x8;
			break;

		case MIRDIR_HOR_VER:
			rotMirMode |= 0xc;
			break;
		}
	}

	JpgEnterLock();

	if (GetJpgPendingInst()) {
		JpgLeaveLock();
		return JPG_RET_FRAME_NOT_COMPLETE;
	}

	// off ROI enable due to not supported feature for encoder.
	JpuWriteReg(MJPEG_CLP_INFO_REG, 0);

	JpuWriteReg(MJPEG_BBC_BAS_ADDR_REG, pEncInfo->streamBufStartAddr);
	JpuWriteReg(MJPEG_BBC_END_ADDR_REG, pEncInfo->streamBufEndAddr);
	JpuWriteReg(MJPEG_BBC_WR_PTR_REG, pEncInfo->streamBufStartAddr);
	JpuWriteReg(MJPEG_BBC_RD_PTR_REG, pEncInfo->streamBufStartAddr);
	JpuWriteReg(MJPEG_BBC_CUR_POS_REG, 0);
	JpuWriteReg(MJPEG_BBC_DATA_CNT_REG, 256 / 4); // 64 * 4 byte == 32 * 8 byte
	JpuWriteReg(MJPEG_BBC_EXT_ADDR_REG, pEncInfo->streamBufStartAddr);
	JpuWriteReg(MJPEG_BBC_INT_ADDR_REG, 0);

	JpuWriteReg(MJPEG_GBU_BT_PTR_REG, 0);
	JpuWriteReg(MJPEG_GBU_WD_PTR_REG, 0);
	JpuWriteReg(MJPEG_GBU_BBSR_REG, 0);
	JpuWriteReg(MJPEG_GBU_CTRL_REG, 0);

	JpuWriteReg(MJPEG_GBU_BBER_REG, ((256 / 4) * 2) - 1);
	JpuWriteReg(MJPEG_GBU_BBIR_REG, 256 / 4); // 64 * 4 byte == 32 * 8 byte
	JpuWriteReg(MJPEG_GBU_BBHR_REG, 256 / 4); // 64 * 4 byte == 32 * 8 byte
	val = 0x18 | pEncInfo->usePartial | (JPU_CHECK_WRITE_RESPONSE_BVALID_SIGNAL << 2);
	JpuWriteReg(MJPEG_PIC_CTRL_REG, val);
	JpuWriteReg(MJPEG_SCL_INFO_REG, 0);
	if (pEncInfo->chroma_interleave == 0)
		val = 0;
	else if (pEncInfo->chroma_interleave == 1)
		val = 2;
	else
		val = 3;
	if (pEncInfo->packedFormat == PACKED_FORMAT_NONE) {
		val |= (pEncInfo->frameEndian << 6) | (0 << 5) | (0 << 4) | (0 << 2);
		JpuWriteReg(MJPEG_DPB_CONFIG_REG, val);
	} else if (pEncInfo->packedFormat == PACKED_FORMAT_444) {
		val |= (pEncInfo->frameEndian << 6) | (1 << 5) | (0 << 4) | (0 << 2);
		JpuWriteReg(MJPEG_DPB_CONFIG_REG, val);
	} else {
		val |= (pEncInfo->frameEndian << 6) | (0 << 5) | (1 << 4) | ((pEncInfo->packedFormat - 1) << 2);
		JpuWriteReg(MJPEG_DPB_CONFIG_REG, val);
	}

	JpuWriteReg(MJPEG_RST_INTVAL_REG, pEncInfo->rstIntval);
	JpuWriteReg(MJPEG_BBC_CTRL_REG, (pEncInfo->streamEndian << 1) | 1);
	val = pEncInfo->partiallineNum << 16 | pEncInfo->partialBufNum << 3 | pEncInfo->busReqNum;
	JpuWriteReg(MJPEG_OP_INFO_REG, val);

	// Load HUFFTab
	if (!JpgEncLoadHuffTab(pEncInfo)) {
		JpgLeaveLock();
		return JPG_RET_INVALID_PARAM;
	}

	// Load QMATTab
	if (!JpgEncLoadQMatTab(pEncInfo)) {
		JpgLeaveLock();
		return JPG_RET_INVALID_PARAM;
	}
	JpgEncEncodeHeader(handle, pEncInfo->paraSet);
	// although rotator is enable, this picture size must not be changed
	// from widh to height.
	JpuWriteReg(MJPEG_PIC_SIZE_REG, pEncInfo->alignedWidth << 16 | pEncInfo->alignedHeight);
	JpuWriteReg(MJPEG_ROT_INFO_REG, (rotMirEnable | rotMirMode));

	val = pEncInfo->mcuBlockNum << 16 | pEncInfo->compNum << 12;
	val |= pEncInfo->compInfo[0] << 8 | pEncInfo->compInfo[1] << 4 | pEncInfo->compInfo[2];
	JpuWriteReg(MJPEG_MCU_INFO_REG, val);

	// JpgEncGbuResetReg
	JpuWriteReg(MJPEG_GBU_CTRL_REG, pEncInfo->stuffByteEnable << 3); // stuffing "FF" data where frame end

	if (pEncInfo->usePartial) {
		int i;

		for (i = 0; i < pEncInfo->partialBufNum; i++) {
			JpuWriteReg(MJPEG_DPB_BASE00_REG + (i * 12), pBasFrame[i].bufY);
			JpuWriteReg(MJPEG_DPB_BASE01_REG + (i * 12), pBasFrame[i].bufCb);
			JpuWriteReg(MJPEG_DPB_BASE02_REG + (i * 12), pBasFrame[i].bufCr);
		}

	} else {
		JpuWriteReg(MJPEG_DPB_BASE00_REG, pBasFrame->bufY);
		JpuWriteReg(MJPEG_DPB_BASE01_REG, pBasFrame->bufCb);
		JpuWriteReg(MJPEG_DPB_BASE02_REG, pBasFrame->bufCr);
	}

	JpuWriteReg(MJPEG_DPB_YSTRIDE_REG, pBasFrame->stride);

	if (pEncInfo->format == FORMAT_420 || pEncInfo->format == FORMAT_422 || pEncInfo->format == FORMAT_400)
		val = 2;
	else
		val = 1;

	if (pEncInfo->chroma_interleave)
		JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, (pBasFrame->stride / (int)val) * 2);
	else
		JpuWriteReg(MJPEG_DPB_CSTRIDE_REG, pBasFrame->stride / (int)val);

	if (pJpgInst->loggingEnable)
		jdi_log(JDI_LOG_CMD_PICRUN, 1);

#ifdef PROFILE_PERFORMANCE
	timer_meter_start();
#endif

	JpuWriteReg(MJPEG_PIC_STATUS_REG, JpuReadReg(MJPEG_PIC_STATUS_REG));
	if (pEncInfo->usePartial)
		JpuWriteReg(MJPEG_PIC_START_REG, (1 << JPG_START_PIC) | (1 << JPG_START_PARTIAL));
	else
		JpuWriteReg(MJPEG_PIC_START_REG, (1 << JPG_START_PIC));

	SetJpgPendingInst(pJpgInst);
	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncGetOutputInfo(JpgEncHandle handle, JpgEncOutputInfo *info)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	Uint32 val;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return ret;
	}

	if (info == 0) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return JPG_RET_INVALID_PARAM;
	}

	pJpgInst = handle;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;

	if (pJpgInst != GetJpgPendingInst()) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	val = JpuReadReg(MJPEG_PIC_STATUS_REG);

	if ((val & 0x4) >> 2) {
		SetJpgPendingInst(0);
		JpgLeaveLock();
		return JPG_RET_WRONG_CALL_SEQUENCE;
	}

	if (val != 0)
		JpuWriteReg(MJPEG_PIC_STATUS_REG, val);

	info->bitstreamBuffer = pEncInfo->streamBufStartAddr;
	info->bitstreamSize = JpuReadReg(MJPEG_BBC_WR_PTR_REG) - pEncInfo->streamBufStartAddr;
	pEncInfo->streamWrPtr = JpuReadReg(MJPEG_BBC_WR_PTR_REG);

	JpuWriteReg(MJPEG_BBC_FLUSH_CMD_REG, 0);

	if (pJpgInst->loggingEnable)
		jdi_log(JDI_LOG_CMD_PICRUN, 0);

	SetJpgPendingInst(0);
	JpgLeaveLock();
	return JPG_RET_SUCCESS;
}

JpgRet JPU_EncGiveCommand(JpgEncHandle handle, JpgCommand cmd, void *param)
{
	JpgInst *pJpgInst;
	JpgEncInfo *pEncInfo;
	JpgRet ret;

	ret = CheckJpgInstValidity(handle);
	if (ret != JPG_RET_SUCCESS)
		return ret;

	pJpgInst = handle;
	pEncInfo = &pJpgInst->JpgInfo.encInfo;
	switch (cmd) {
	case ENABLE_JPG_ROTATION: {
		pEncInfo->rotationEnable = 1;
	} break;
	case DISABLE_JPG_ROTATION: {
		pEncInfo->rotationEnable = 0;
	} break;
	case ENABLE_JPG_MIRRORING: {
		pEncInfo->mirrorEnable = 1;
	} break;
	case DISABLE_JPG_MIRRORING: {
		pEncInfo->mirrorEnable = 0;
	} break;
	case SET_JPG_MIRROR_DIRECTION: {
		JpgMirrorDirection mirDir;

		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}
		mirDir = *(JpgMirrorDirection *)param;
		if (!(mirDir >= MIRDIR_NONE && mirDir <= MIRDIR_HOR_VER)) {
			return JPG_RET_INVALID_PARAM;
		}
		pEncInfo->mirrorDirection = mirDir;
	} break;
	case SET_JPG_ROTATION_ANGLE: {
		int angle;

		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}
		angle = *(int *)param;
		if (angle != 0 && angle != 90 && angle != 180 && angle != 270) {
			return JPG_RET_INVALID_PARAM;
		}
		if (pEncInfo->initialInfoObtained && (angle == 90 || angle == 270)) {
			return JPG_RET_INVALID_PARAM;
		}
		pEncInfo->rotationAngle = angle;
	} break;
	case ENC_JPG_GET_HEADER: {
		if (param == 0) {
			return JPG_RET_INVALID_PARAM;
		}

		pEncInfo->paraSet = (JpgEncParamSet *)param;
		break;
	}
	case SET_JPG_USE_PARTIAL_MODE: {
		int enable;

		enable = *(int *)param;
		pEncInfo->usePartial = enable;

		break;
	}
	case SET_JPG_PARTIAL_FRAME_NUM: {
		int frame;

		frame = *(int *)param;
		pEncInfo->partialBufNum = frame;

		break;
	}
	case SET_JPG_PARTIAL_LINE_NUM: {
		int line;

		line = *(int *)param;
		pEncInfo->partiallineNum = line;

		break;
	}
	case SET_JPG_ENCODE_NEXT_LINE: {
		JpuWriteReg(MJPEG_PIC_START_REG, (1 << JPG_START_PARTIAL));
		break;
	}
	case SET_JPG_USE_STUFFING_BYTE_FF: {
		int enable;

		enable = *(int *)param;
		pEncInfo->stuffByteEnable = enable;
		break;
	}
	case ENABLE_LOGGING: {
		pJpgInst->loggingEnable = 1;
		break;
	}
	case DISABLE_LOGGING: {
		pJpgInst->loggingEnable = 0;
		break;
	}
	default:
		return JPG_RET_INVALID_COMMAND;
	}

	return JPG_RET_SUCCESS;
}
