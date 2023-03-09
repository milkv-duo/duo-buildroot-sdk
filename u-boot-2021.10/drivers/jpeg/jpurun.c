#include "jpuapi.h"
#include "jpuapifunc.h"
#include "jpuhelper.h"
#include "jpulog.h"
#include "jpurun.h"
#include "mixer.h"
#include "regdefine.h"
#include <stdlib.h>
#include "asm/cache.h"

#ifdef JPU_FPGA_PLATFORM
//#define ENC_SOURCE_FRAME_DISPLAY
#endif

#define NUM_FRAME_BUF MAX_FRAME
#define MAX_ROT_BUF_NUM 1
#define EXTRA_FRAME_BUFFER_NUM 0
#define ENC_SRC_BUF_NUM 1
//#define ROI_RANDOM_TEST
//#define TEST_JPEG_PERFORMANCE

#ifdef TEST_JPEG_PERFORMANCE
#undef __CONFIG_H__

#include "fw_config.h"
#include "system_common.h"
#include "dm/device.h"
#include "timer.h"
#endif

#define ALIGN_N_BIT(ADDR, BITS) ((((ADDR) + ((1 << (BITS)) - 1)) >> (BITS)) << (BITS))

int img_width, img_height;

int copy_to_dest_addr(Uint8 *pYuv, Uint8 *pRefYuv,
		      int picWidth, int picHeight, int strideY, int strideC,
		      int interleave, int format, int endian, int packed)
{
	int size = 0;
	int nY = 0;
	Uint8 *pRef = NULL;
	int lumaSize = 0, chromaSize = 0;

	Uint8 *pOrg = NULL;

	switch (format) {
	case FORMAT_420:
		nY = (picHeight + 1) / 2 * 2;
		chromaSize = ((picWidth + 1) / 2) * ((picHeight + 1) / 2);
		break;
	case FORMAT_224:
		nY = (picHeight + 1) / 2 * 2;
		chromaSize = (picWidth) * ((picHeight + 1) / 2);
		break;
	case FORMAT_422:
		nY = picHeight;
		chromaSize = ((picWidth + 1) / 2) * picHeight;
		break;
	case FORMAT_444:
		nY = picHeight;
		chromaSize = picWidth * picHeight;
		break;
	case FORMAT_400:
		nY = picHeight;
		chromaSize = 0;
		break;
	}

	pRef = pRefYuv;
	pOrg = pYuv;

	if (packed) {
		if (packed == PACKED_FORMAT_444)
			picWidth *= 3;
		else
			picWidth *= 2;

		lumaSize = picWidth * nY;
		chromaSize = 0;
	} else {
		if (format == FORMAT_420) {
			lumaSize = strideY * nY;
			chromaSize = strideC * ((picHeight + 1) / 2);
		} else {
			lumaSize = picWidth * nY;
		}
	}

	CVI_JPG_DBG("nY = %d, picHeight = %d\n", nY, picHeight);
	CVI_JPG_DBG("strideY = %d, lumaSize = 0x%X\n", strideY, lumaSize);
	CVI_JPG_DBG("strideC = %d, chromaSize = 0x%X\n", strideC, chromaSize);

	size = lumaSize + chromaSize * 2;

	CVI_JPG_DBG("size = 0x%X, lumaSize = 0x%X, chromaSize = 0x%X\n", size, lumaSize, chromaSize);

	CVI_JPG_DBG("y, pRef = %p, pOrg = %p\n", pRef, pOrg);
	OSAL_MEMCPY(pRef, pOrg, lumaSize);

	pOrg += lumaSize;
	pRef += lumaSize;
	pRef = (Uint8 *)ALIGN_N_BIT((unsigned long long)pRef, 12);
	CVI_JPG_DBG("u, pRef = %p, pOrg = %p\n", pRef, pOrg);
	OSAL_MEMCPY(pRef, pOrg, chromaSize);

	pOrg += chromaSize;
	pRef += chromaSize;
	pRef = (Uint8 *)ALIGN_N_BIT((unsigned long long)pRef, 12);
	CVI_JPG_DBG("v, pRef = %p, pOrg = %p\n", pRef, pOrg);
	OSAL_MEMCPY(pRef, pOrg, chromaSize);

	//flush_dcache_all();
	flush_dcache_range((unsigned long)pRefYuv, (unsigned long)pRefYuv + lumaSize + chromaSize + chromaSize);

	return 0;
}

int jpeg_decode_helper(DecConfigParam *param)
{
	JpgDecHandle handle = {0};
	JpgDecOpenParam decOP = {0};
	JpgDecInitialInfo initialInfo = {0};
	JpgDecOutputInfo outputInfo = {0};
	JpgDecParam decParam = {0};
	JpgRet ret = JPG_RET_SUCCESS;
	FrameBuffer frameBuf[NUM_FRAME_BUF];
	jpu_buffer_t vbStream = {0};
	BufInfo bufInfo = {0};
	FRAME_BUF * pFrame[NUM_FRAME_BUF];
	Uint32 framebufWidth = 0, framebufHeight = 0;
	Uint32 framebufStrideY = 0, framebufStrideC = 0;
	Uint32 framebufFormat = FORMAT_420;
	int dispWidth = 0, dispHeight = 0;
	int i = 0, frameIdx = 0, ppIdx = 0, saveIdx = 0, totalNumofErrMbs = 0, streameos = 0, dispImage = 0;
	int suc = 1;
	Uint8 *pRefYuvBuf = NULL;
	int needFrameBufCount = 0, regFrameBufCount = 0;
	int rotEnable = 0;
	int int_reason = 0;
	int instIdx;
	int partPosIdx = 0;
	int partBufIdx = 0;
	int partMaxIdx = 0;
	int partialHeight = 0;
	int jpeg_done = 0;

	DecConfigParam decConfig;

	memcpy(&decConfig, param, sizeof(DecConfigParam));
	memset(&pFrame, 0x00, sizeof(FRAME_BUF *) * NUM_FRAME_BUF);
	memset(&frameBuf, 0x00, sizeof(FrameBuffer) * NUM_FRAME_BUF);

	instIdx = decConfig.instNum;
	if (decConfig.usePartialMode && decConfig.roiEnable) {
		JLOG(ERR, "Invalid operation mode : partial and ROI mode can not be worked\n");
		goto ERR_DEC_INIT;
	}

	if (decConfig.packedFormat && decConfig.roiEnable) {
		JLOG(ERR, "Invalid operation mode : packed mode and ROI mode can not be worked\n");
		goto ERR_DEC_INIT;
	}

	if ((decConfig.iHorScaleMode || decConfig.iVerScaleMode) && decConfig.roiEnable) {
		JLOG(ERR, "Invalid operation mode : Scaler mode and ROI mode can not be worked\n");
		goto ERR_DEC_INIT;
	}

	if (decConfig.useRot && decConfig.roiEnable) {
		JLOG(ERR, "Invalid operation mode : Rotator mode and ROI mode can not be worked\n");
		goto ERR_DEC_INIT;
	}

	if (!decConfig.yuvFileName)
		dispImage = 0;
	else
		dispImage = 1;

	decConfig.comparatorFlag = param->comparatorFlag;

	pRefYuvBuf = (Uint8 *)decConfig.yuv_addr;
	bufInfo.buf = (Uint8 *)decConfig.bs_addr;
	bufInfo.size = decConfig.size;
	bufInfo.point = 0;

	ret = JPU_Init();

	if (ret != JPG_RET_SUCCESS && ret != JPG_RET_CALLED_BEFORE) {
		suc = 0;
		JLOG(ERR, "JPU_Init failed Error code is 0x%x\n", ret);
		goto ERR_DEC_INIT;
	}

	// Open an instance and get initial information for decoding.

	vbStream.size = STREAM_BUF_SIZE;
	if (jdi_allocate_dma_memory(&vbStream) < 0) {
		JLOG(ERR, "fail to allocate bitstream buffer\n");
		goto ERR_DEC_INIT;
	}

	decOP.streamEndian = decConfig.StreamEndian;
	decOP.frameEndian = decConfig.FrameEndian;
	decOP.chroma_interleave = (CbCrInterLeave)decConfig.chroma_interleave;
	decOP.bitstreamBuffer = vbStream.phys_addr;
	decOP.bitstreamBufferSize = vbStream.size;
	decOP.pBitStream = (BYTE *)vbStream.virt_addr; // set virtual address mapped of physical address
	decOP.packedFormat = decConfig.packedFormat;
	decOP.roiEnable = decConfig.roiEnable;
	decOP.roiOffsetX = decConfig.roiOffsetX;
	decOP.roiOffsetY = decConfig.roiOffsetY;
	decOP.roiWidth = decConfig.roiWidth;
	decOP.roiHeight = decConfig.roiHeight;

	decParam.scaleDownRatioWidth = decConfig.iHorScaleMode;
	decParam.scaleDownRatioHeight = decConfig.iVerScaleMode;

	JLOG(INFO, "scale ratio:%d, %d\n", decParam.scaleDownRatioWidth, decParam.scaleDownRatioHeight);

	ret = JPU_DecOpen(&handle, &decOP);

	if (ret != JPG_RET_SUCCESS) {
		JLOG(ERR, "JPU_DecOpen failed Error code is 0x%x\n", ret);
		goto ERR_DEC_INIT;
	}

	// JPU_DecGiveCommand(handle, ENABLE_LOGGING, NULL);

	if (decConfig.useRot)
		rotEnable = 1;
	else
		rotEnable = 0;

	ret = WriteJpgBsBufHelper(handle, &bufInfo, decOP.bitstreamBuffer,
				  decOP.bitstreamBuffer + decOP.bitstreamBufferSize,
				  0, 0, &streameos, decOP.streamEndian);

	//flush_dcache_all();
	flush_dcache_range(decOP.bitstreamBuffer, decOP.bitstreamBuffer + decOP.bitstreamBufferSize);

	if (ret != JPG_RET_SUCCESS) {
		suc = 0;
		JLOG(ERR, "WriteBsBufHelper failed Error code is 0x%x\n", ret);
		goto ERR_DEC_OPEN;
	}

	ret = JPU_DecGetInitialInfo(handle, &initialInfo);

	if (ret != JPG_RET_SUCCESS) {
		suc = 0;
		JLOG(ERR, "JPU_DecGetInitialInfo failed Error code is 0x%x, inst=%d\n", ret, instIdx);
		goto ERR_DEC_OPEN;
	}

	if (decConfig.usePartialMode) {
		// disable Rotator, Scaler
		rotEnable = 0;
		decConfig.iHorScaleMode = 0;
		decConfig.iVerScaleMode = 0;
		partialHeight = (initialInfo.sourceFormat == FORMAT_420 || initialInfo.sourceFormat == FORMAT_224) ?
				16 : 8;

		partMaxIdx = ((initialInfo.picHeight + 15) & ~15) / partialHeight;

		if (partMaxIdx < decConfig.partialBufNum)
			decConfig.partialBufNum = partMaxIdx;
	}
	img_width = initialInfo.picWidth;
	img_height = initialInfo.picHeight;

	JLOG(INFO, "init info width: %d, height = %d\n", initialInfo.picWidth, initialInfo.picHeight);

	if (initialInfo.sourceFormat == FORMAT_420 || initialInfo.sourceFormat == FORMAT_422)
		framebufWidth = ((initialInfo.picWidth + 15) / 16) * 16;
	else
		framebufWidth = ((initialInfo.picWidth + 7) / 8) * 8;

	if (initialInfo.sourceFormat == FORMAT_420 || initialInfo.sourceFormat == FORMAT_224)
		framebufHeight = ((initialInfo.picHeight + 15) / 16) * 16;
	else
		framebufHeight = ((initialInfo.picHeight + 7) / 8) * 8;

	if (decConfig.roiEnable) {
		framebufWidth = initialInfo.roiFrameWidth;
		framebufHeight = initialInfo.roiFrameHeight;
	}

	// scaler constraint when conformance test is disable
	if (framebufWidth < 128 || framebufHeight < 128) {
		if (decConfig.iHorScaleMode || decConfig.iVerScaleMode)
			JLOG(WARN,
			     "Invalid operation mode : Not supported resolution with Scaler, width=%d, height=%d\n",
			     framebufWidth, framebufHeight);
		decConfig.iHorScaleMode = 0;
		decConfig.iVerScaleMode = 0;
	}

	JLOG(INFO, "* Dec InitialInfo =>\n instance #%d,\n minframeBuffercount: %u\n",
	     instIdx, initialInfo.minFrameBufferCount);
	JLOG(INFO, "picWidth: %u\n picHeight: %u\n roiWidth: %u\n rouHeight: %u\n",
	     initialInfo.picWidth, initialInfo.picHeight, initialInfo.roiFrameWidth, initialInfo.roiFrameHeight);

	if (decConfig.usePartialMode) {
		JLOG(INFO, "Partial Mode Enable\n ");
		JLOG(INFO, "Num of Buffer for Partial : %d\n ", decConfig.partialBufNum);
		JLOG(INFO, "Num of Line for Partial   : %d\n ", partialHeight);
	}

	framebufFormat = initialInfo.sourceFormat;
	framebufWidth >>= decConfig.iHorScaleMode;
	framebufHeight >>= decConfig.iVerScaleMode;

	if (decConfig.iHorScaleMode || decConfig.iVerScaleMode) {
		framebufHeight = ((framebufHeight + 1) / 2) * 2;
		framebufWidth = ((framebufWidth + 1) / 2) * 2;
	}

	dispWidth = (decConfig.rot_angle == 90 || decConfig.rot_angle == 270) ? framebufHeight : framebufWidth;
	dispHeight = (decConfig.rot_angle == 90 || decConfig.rot_angle == 270) ? framebufWidth : framebufHeight;

	if (decConfig.rot_angle == 90 || decConfig.rot_angle == 270) {
		framebufStrideY = framebufHeight;
		framebufHeight = framebufWidth;
		framebufFormat = (framebufFormat == FORMAT_422) ? FORMAT_224 :
				 (framebufFormat == FORMAT_224) ? FORMAT_422 : framebufFormat;
	} else {
		framebufStrideY = framebufWidth;
	}

	framebufStrideC = framebufStrideY / 2;
	printf("framebufStrideY = %d, framebufStrideC = %d\n", framebufStrideY, framebufStrideC);

	if (decConfig.iHorScaleMode || decConfig.iVerScaleMode)
		framebufStrideY = ((framebufStrideY + 15) / 16) * 16;

	if (decOP.packedFormat >= PACKED_FORMAT_422_YUYV && decOP.packedFormat <= PACKED_FORMAT_422_VYUY) {
		framebufStrideY = framebufStrideY * 2;
		framebufFormat = FORMAT_422;
		if (decConfig.rot_angle == 90 || decConfig.rot_angle == 270)
			framebufFormat = FORMAT_224;

	} else if (decOP.packedFormat == PACKED_FORMAT_444) {
		framebufStrideY = framebufStrideY * 3;
		framebufFormat = FORMAT_444;
	} else if (decOP.packedFormat == PACKED_FORMAT_NONE) {
#ifdef ALIGN_32
		if (framebufFormat == FORMAT_420) {
			framebufStrideY = ALIGN_X(framebufStrideY, 32);
			framebufStrideC = ALIGN_X(framebufStrideY / 2, 32);
		}
#endif
	}

	JLOG(INFO, "framebufStrideY = %d, framebufHeight = %d\n", framebufStrideY, framebufHeight);
	JLOG(INFO, "framebufFormat = %d, packedFormat = %d\n", framebufFormat, decOP.packedFormat);
	JLOG(INFO, "display width: %d, height = %d\n", dispWidth, dispHeight);

	// Allocate frame buffer
	regFrameBufCount = initialInfo.minFrameBufferCount + EXTRA_FRAME_BUFFER_NUM;

	if (decConfig.usePartialMode) {
		if (decConfig.partialBufNum > 4)
			decConfig.partialBufNum = 4;

		regFrameBufCount *= decConfig.partialBufNum;
	}

	needFrameBufCount = regFrameBufCount;

	AllocateFrameBuffer(instIdx, framebufFormat, framebufStrideY,
			    framebufHeight, needFrameBufCount, 0, framebufStrideC);

	JpgEnterLock();

	for (i = 0; i < needFrameBufCount; ++i) {
		pFrame[i] = GetFrameBuffer(instIdx, i);
		frameBuf[i].bufY = pFrame[i]->vb_y.phys_addr;
		JLOG(INFO, "frame buffer Y addr: 0x%lx\n", frameBuf[i].bufY);
		frameBuf[i].bufCb = pFrame[i]->vb_cb.phys_addr;
		JLOG(INFO, "frame buffer Cb addr: 0x%lx\n", frameBuf[i].bufCb);

		if (decOP.chroma_interleave == CBCR_SEPARATED) {
			frameBuf[i].bufCr = pFrame[i]->vb_cr.phys_addr;
			JLOG(INFO, "frame buffer Cr addr: 0x%lx\n", frameBuf[i].bufCr);
		}

		if (dispImage) {
			clear_frame_buffer(instIdx, i);
			JLOG(INFO, ".");
		}
	}

	JpgLeaveLock();

	ret = JPU_DecGiveCommand(handle, SET_JPG_USE_PARTIAL_MODE, &decConfig.usePartialMode);
	if (ret != JPG_RET_SUCCESS) {
		suc = 0;
		JLOG(ERR, "JPU_DecGiveCommand[SET_JPG_USE_PARTIAL_MODE] failed Error code is 0x%x\n", ret);
		goto ERR_DEC_OPEN;
	}

	ret = JPU_DecGiveCommand(handle, SET_JPG_PARTIAL_FRAME_NUM, &decConfig.partialBufNum);

	if (ret != JPG_RET_SUCCESS) {
		suc = 0;
		JLOG(ERR, "JPU_DecGiveCommand[SET_JPG_PARTIAL_FRAME_NUM] failed Error code is 0x%x\n", ret);
		goto ERR_DEC_OPEN;
	}

	ret = JPU_DecGiveCommand(handle, SET_JPG_PARTIAL_LINE_NUM, &partialHeight);

	if (ret != JPG_RET_SUCCESS) {
		suc = 0;
		JLOG(ERR, "JPU_DecGiveCommand[SET_JPG_PARTIAL_LINE_NUM] failed Error code is 0x%x\n", ret);
		goto ERR_DEC_OPEN;
	}

	// Register frame buffers requested by the decoder.
	ret = JPU_DecRegisterFrameBuffer(handle, frameBuf, regFrameBufCount, framebufStrideY, framebufStrideC);
	if (ret != JPG_RET_SUCCESS) {
		suc = 0;
		JLOG(ERR, "JPU_DecRegisterFrameBuffer failed Error code is 0x%x\n", ret);
		goto ERR_DEC_OPEN;
	}

	ppIdx = 0;

	while (1) {
#ifdef TEST_JPEG_PERFORMANCE
		u64 now = timer_get_tick(); // get_timer(0);
		u64 time_consume = 0;
#endif
		if (rotEnable) {
			JPU_DecGiveCommand(handle, SET_JPG_ROTATION_ANGLE, &decConfig.rot_angle);
			JPU_DecGiveCommand(handle, SET_JPG_MIRROR_DIRECTION, &decConfig.mirDir);
			JPU_DecGiveCommand(handle, SET_JPG_ROTATOR_OUTPUT, &frameBuf[ppIdx]);
			JPU_DecGiveCommand(handle, SET_JPG_ROTATOR_STRIDE, &framebufStrideY);
			JPU_DecGiveCommand(handle, ENABLE_JPG_ROTATION, 0);
			JPU_DecGiveCommand(handle, ENABLE_JPG_MIRRORING, 0);
		}

		JPU_DecGiveCommand(handle, SET_JPG_SCALE_HOR, &decConfig.iHorScaleMode);
		JPU_DecGiveCommand(handle, SET_JPG_SCALE_VER, &decConfig.iVerScaleMode);

		if (decConfig.usePartialMode) {
			partPosIdx = 0;
			partBufIdx = 0;
			outputInfo.decodingSuccess = 0;
			JPU_SWReset();
		}

		// Start decoding a frame.
		ret = JPU_DecStartOneFrame(handle, &decParam);

		if (ret != JPG_RET_SUCCESS && ret != JPG_RET_EOS) {
			if (ret == JPG_RET_BIT_EMPTY) {
				ret = WriteJpgBsBufHelper(handle, &bufInfo, decOP.bitstreamBuffer,
							  decOP.bitstreamBuffer + decOP.bitstreamBufferSize,
							  STREAM_FILL_SIZE, 0, &streameos, decOP.streamEndian);

				if (ret != JPG_RET_SUCCESS) {
					suc = 0;
					JLOG(ERR, "WriteBsBufHelper failed Error code is 0x%x\n", ret);
					goto ERR_DEC_OPEN;
				}
				continue;
			}

			suc = 0;
			JLOG(ERR, "JPU_DecStartOneFrame failed Error code is 0x%x\n", ret);
			goto ERR_DEC_OPEN;
		}

		if (ret == JPG_RET_EOS)
			goto JPU_END_OF_STREAM;

		while (1) {
			int_reason = JPU_WaitInterrupt(JPU_INTERRUPT_TIMEOUT_MS);

			if (int_reason == -1) {
				JLOG(ERR, "Error : timeout happened\n");
				JPU_SWReset();
				break;
			}

			if (decConfig.usePartialMode && (int_reason & 0xf0)) {
				partBufIdx = ((partPosIdx) % decConfig.partialBufNum);

				if ((1 << partBufIdx) & ((int_reason & 0xf0) >> 4)) {
					printf("DECODED : PARTIAL BUFFER IDX %d / POS %d / MAX POS %d / INT_REASON=0x%x\n",
					       partBufIdx, partPosIdx + 1, partMaxIdx, int_reason);

					if (dispImage) {
//						p_disp_frame = FindFrameBuffer(instIdx,
// frameBuf[partBufIdx].bufY);
#ifdef JPU_FPGA_PLATFORM
						set_mixer_dec_out_frame(p_disp_frame, framebufStrideY, partialHeight);
#endif
					}

					partPosIdx++;
					JPU_ClrStatus((1 << (INT_JPU_PARIAL_BUF0_EMPTY + partBufIdx)));

					continue;
				} else {
					suc = 0;
					JLOG(ERR,
					     "Invalid partial interrupt : expected reason =0x%x, actual reason=0x%x\n",
					     (1 << partBufIdx), ((int_reason & 0xF0) >> 4));
					goto ERR_DEC_OPEN;
				}
			}

			if (int_reason & (1 << INT_JPU_DONE)) {
				// Must catch PIC_DONE interrupt before catching EMPTY interrupt
				// Do no clear INT_JPU_DONE and INT_JPU_ERROR interrupt.
				// these will be cleared in JPU_DecGetOutputInfo.
				JLOG(INFO, "jpeg done\n");
				jpeg_done = 1;
				break;
			}

			if (int_reason & (1 << INT_JPU_ERROR)) {
				// Must catch PIC_DONE interrupt before catching EMPTY interrupt
				// Do no clear INT_JPU_DONE and INT_JPU_ERROR interrupt.
				// these will be cleared in JPU_DecGetOutputInfo.
				break;
			}

			if (int_reason & (1 << INT_JPU_BIT_BUF_EMPTY)) {
				ret = WriteJpgBsBufHelper(handle, &bufInfo, decOP.bitstreamBuffer,
							  decOP.bitstreamBuffer + decOP.bitstreamBufferSize,
							  STREAM_FILL_SIZE, 0, &streameos, decOP.streamEndian);
				printf("write buffer in empty intterupt!\n\n");
				if (ret != JPG_RET_SUCCESS) {
					suc = 0;
					JLOG(ERR,
					     "WriteBsBufHelper failed Error code is 0x%x\n",
					     ret);
					goto ERR_DEC_OPEN;
				}
				JPU_ClrStatus((1 << INT_JPU_BIT_BUF_EMPTY));
			}

			if (int_reason & (1 << INT_JPU_BIT_BUF_STOP)) {
				ret = JPU_DecCompleteStop(handle);
				if (ret != JPG_RET_SUCCESS) {
					suc = 0;
					JLOG(ERR, "JPU_DecCompleteStop failed Error code is 0x%x\n", ret);
					goto ERR_DEC_OPEN;
				}
				JPU_ClrStatus((1 << INT_JPU_BIT_BUF_STOP));
				break;
			}

			if (int_reason & (1 << INT_JPU_PARIAL_OVERFLOW))
				JPU_ClrStatus((1 << INT_JPU_PARIAL_OVERFLOW));
		}

JPU_END_OF_STREAM:
		ret = JPU_DecGetOutputInfo(handle, &outputInfo);

		if (ret != JPG_RET_SUCCESS) {
			suc = 0;
			JLOG(ERR, "JPU_DecGetOutputInfo failed Error code is 0x%x\n", ret);
			goto ERR_DEC_OPEN;
		}

#ifdef TEST_JPEG_PERFORMANCE
		time_consume = timer_get_tick();
		JLOG(INFO, "time_consume: %ld\n", (time_consume - now) / TIMER_CNT_US);
#endif

#ifdef MJPEG_ERROR_CONCEAL
		if (outputInfo.numOfErrMBs) {
			int err_rst_idx, errPosX, errPosY;

			err_rst_idx = (outputInfo.numOfErrMBs & 0x0F000000) >> 24;
			errPosX = (outputInfo.numOfErrMBs & 0x00FFF000) >> 12;
			errPosY = (outputInfo.numOfErrMBs & 0x00000FFF);
			JLOG(ERR,
			     "Error restart Idx : %d, MCU x:%d, y:%d, in Frame : %d\n",
			     err_rst_idx, errPosX, errPosY, frameIdx);
			continue;
		}
#endif

		if (outputInfo.decodingSuccess == 0)
			JLOG(ERR, "JPU_DecGetOutputInfo decode fail framdIdx %d\n", frameIdx);

		JLOG(INFO,
		     "#%d:%d, indexFrameDisplay %d || consumedByte %d || ppIdx %d || frameStart=0x%x || ecsStart=0x%x\n",
		     instIdx, frameIdx, outputInfo.indexFrameDisplay, outputInfo.consumedByte,
		     ppIdx, outputInfo.bytePosFrameStart, outputInfo.bytePosFrameStart + outputInfo.ecsPtr);

		JLOG(INFO, "rdPtr=0x%x || wrPtr=0x%x || pos=%d\n",
		     JpuReadReg(MJPEG_BBC_RD_PTR_REG),
		     JpuReadReg(MJPEG_BBC_WR_PTR_REG),
		     JpuReadReg(MJPEG_BBC_CUR_POS_REG));

		if (outputInfo.indexFrameDisplay == -1)
			break;

		// YUV Dump Done when partial buffer is all displayed.
		int_reason = JPU_GetStatus();

		if (decConfig.usePartialMode && !(int_reason & 0xF0))
			goto SKIP_BUF_DUMP;

		// indexFrameDisplay points to the frame buffer, among ones
		// registered, which holds the output of the decoder.
		if (dispImage) {
#ifdef JPU_FPGA_PLATFORM
			if (frameIdx)
				wait_mixer_int();
#endif
			if (!rotEnable) {
#ifdef JPU_FPGA_PLATFORM
				set_mixer_dec_out_frame(p_disp_frame, outputInfo.decPicWidth, outputInfo.decPicHeight);
#endif
			} else {
#ifdef JPU_FPGA_PLATFORM
				set_mixer_dec_out_frame(p_disp_frame,
							(decConfig.rot_angle == 90 || decConfig.rot_angle == 270)
							? outputInfo.decPicHeight : outputInfo.decPicWidth,
							(decConfig.rot_angle == 90 || decConfig.rot_angle == 270)
							? outputInfo.decPicWidth : outputInfo.decPicHeight);
#endif

				ppIdx = (ppIdx - regFrameBufCount + 1) % MAX_ROT_BUF_NUM;
			}
		} else if (decConfig.comparatorFlag != 0) {
			JLOG(INFO, "compare yuv, frameBuf[%d].bufY = 0x%lx\n", saveIdx, frameBuf[saveIdx].bufY);
			JLOG(INFO, "width:%d, height:%d\n", dispWidth, dispHeight);

			saveIdx = ppIdx;
			if (comparateYuv((Uint8 *)frameBuf[saveIdx].bufY, pRefYuvBuf,
					 dispWidth, dispHeight, framebufStrideY, (int)decOP.chroma_interleave,
					 framebufFormat, 0, decOP.packedFormat) != 0) {
				suc = 0;
			}

			JLOG(INFO, "compare result: %d\n", 1 - suc);

			if (suc)
				JLOG(ERR, "compare pass\n");

			ppIdx = (ppIdx - regFrameBufCount + 1) % MAX_ROT_BUF_NUM;
		}

		copy_to_dest_addr((Uint8 *)frameBuf[saveIdx].bufY, pRefYuvBuf,
				  dispWidth, dispHeight, framebufStrideY, framebufStrideC,
				  (int)decOP.chroma_interleave, framebufFormat, 0, decOP.packedFormat);

SKIP_BUF_DUMP:
		if (outputInfo.numOfErrMBs) {
			int err_rst_idx, errPosX, errPosY;

			err_rst_idx = (outputInfo.numOfErrMBs & 0x0F000000) >> 24;
			errPosX = (outputInfo.numOfErrMBs & 0x00FFF000) >> 12;
			errPosY = (outputInfo.numOfErrMBs & 0x00000FFF);
			JLOG(ERR,
			     "Error restart Idx : %d, MCU x:%d, y:%d, in Frame : %d\n",
			     err_rst_idx, errPosX, errPosY, frameIdx);
		}
		frameIdx++;

		if (decConfig.outNum && frameIdx == decConfig.outNum)
			break;
	}

	if (totalNumofErrMbs) {
		suc = 0;
		JLOG(ERR, "Total Num of Error MBs : %d\n", totalNumofErrMbs);
	}

ERR_DEC_OPEN:
	// Now that we are done with decoding, close the open instance.
	ret = JPU_DecClose(handle);

	JLOG(INFO, "Dec End. Tot Frame %d\n", frameIdx);

ERR_DEC_INIT:
	free_frame_buffer(instIdx);

	jdi_free_dma_memory(&vbStream);

	JPU_DeInit();

	if (jpeg_done == 1) {
		JLOG(INFO, "decode test done\n");
	}

	return suc;
}

int get_jpeg_size(int *width_addr, int *height_addr)
{
	*width_addr = img_width;
	*height_addr = img_height;

	return 0;
}

