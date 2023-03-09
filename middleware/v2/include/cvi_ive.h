/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_ive.h
 * Description:
 */

#ifndef __CVI_IVE_H__
#define __CVI_IVE_H__

#include "linux/cvi_type.h"
#include "linux/cvi_comm_ive.h"
#include "linux/cvi_comm_video.h"
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
extern __attribute__((weak)) void *__dso_handle;
#endif
#endif /* End of #ifdef __cplusplus */

CVI_S32 CVI_IVE_CompareIveImage(IVE_IMAGE_S *pstImage1, IVE_IMAGE_S *pstImage2);
CVI_S32 CVI_IVE_CompareIveMem(IVE_MEM_INFO_S *pstMem1, IVE_MEM_INFO_S *pstMem2);
CVI_S32 CVI_IVE_CompareIveData(IVE_DATA_S *pstData1, IVE_DATA_S *pstData2);
CVI_S32 CVI_IVE_CompareSADImage(IVE_IMAGE_S *pstImage1, IVE_IMAGE_S *pstImage2,
				IVE_SAD_MODE_E mode, CVI_BOOL isDMAhalf);

/**
 * @brief Create ive instance.
 * @return Ive instance handler.
 */
IVE_HANDLE CVI_IVE_CreateHandle();

/**
 * @brief Destroy ive instance.
 *
 * @param pIveHandle Ive instance handler.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_DestroyHandle(IVE_HANDLE pIveHandle);

/**
 * @brief IVE_IMAGE_S to VIDEO_FRAME_INFO_S.
 *
 * @param pstIISrc Input IVE_IMAGE_S.
 * @param pstVFIDst Output VIDEO_FRAME_INFO_S.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Image2VideoFrameInfo(IVE_IMAGE_S *pstIISrc, VIDEO_FRAME_INFO_S *pstVFIDst);

/**
 * @brief VIDEO_FRAME_INFO_S to IVE_IMAGE_S.
 *
 * @param pstVFISrc Input VIDEO_FRAME_INFO_S.
 * @param pstIIDst Output IVE_IMAGE_S.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_VideoFrameInfo2Image(VIDEO_FRAME_INFO_S *pstVFISrc, IVE_IMAGE_S *pstIIDst);

/**
 * @brief Set IVE_IMAGE_S content.
 *
 * @param pstImage IVE_IMAGE_S structure.
 * @param val input value.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ResetImage(IVE_IMAGE_S *pstImage, CVI_U8 val);

/**
 * @brief Create IVE_IMAGE_S structure, read from memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg IVE_IMAGE_S structure.
 * @param pBuffer Data array.
 * @param enType Set image format
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ReadImageArray(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg,
			       char *pBuffer, IVE_IMAGE_TYPE_E enType,
			       CVI_U16 u16Width, CVI_U16 u16Height);

/**
 * @brief Create IVE_IMAGE_S structure, read from file.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg IVE_IMAGE_S structure.
 * @param filename Read file name.
 * @param enType Set image format
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ReadRawImage(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg,
			  const char *filename, IVE_IMAGE_TYPE_E enType,
			  CVI_U16 u16Width, CVI_U16 u16Height);

/**
 * @brief Read an image from file system. Default is in the order of RGB.
 *
 * @param pIveHandle Ive instance handler.
 * @param filename Read png file name.
 * @param enType Type of the destination image.
 * @return IVE_IMAGE_S Return IVE_IMAGE_S.
 */
IVE_IMAGE_S CVI_IVE_ReadImage(IVE_HANDLE pIveHandle, const char *filename,
				IVE_IMAGE_TYPE_E enType);
/**
 * @brief Create IVE_MEM_INFO_S structure, read from memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstMem IVE_MEM_INFO_S structure.
 * @param pBuffer Data array.
 * @param uSize Set ion alloc size.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ReadMemArray(IVE_HANDLE pIveHandle, IVE_MEM_INFO_S *pstMem,
			     char *pBuffer, CVI_U32 uSize);

/**
 * @brief Create IVE_MEM_INFO_S structure, read from file.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstMem IVE_MEM_INFO_S structure.
 * @param filename Read file name.
 * @param uSize Set ion alloc size.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ReadMem(IVE_HANDLE pIveHandle, IVE_MEM_INFO_S *pstMem,
			const char *filename, CVI_U32 uSize);

/**
 * @brief Create IVE_DATA_S structure, read from memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstData IVE_DATA_S structure.
 * @param pBuffer Data array.
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ReadDataArray(IVE_HANDLE pIveHandle, IVE_DATA_S *pstData,
			      char *pBuffer, CVI_U16 u16Width,
			      CVI_U16 u16Height);

/**
 * @brief Create IVE_DATA_S structure, read from file.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstData IVE_DATA_S structure.
 * @param filename Read file name.
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ReadData(IVE_HANDLE pIveHandle, IVE_DATA_S *pstData,
			 const char *filename, CVI_U16 u16Width,
			 CVI_U16 u16Height);

/**
 * @brief Create IVE_MEM_INFO_S structure, get ion memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstDataInfo IVE_MEM_INFO_S structure.
 * @param u32ByteSize Set ion alloc size.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CreateMemInfo(IVE_HANDLE pIveHandle, IVE_MEM_INFO_S *pstMemInfo,
			      CVI_U32 u32ByteSize);

/**
 * @brief Create IVE_DATA_S structure, get ion memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstDataInfo IVE_DATA_S structure.
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CreateDataInfo(IVE_HANDLE pIveHandle, IVE_DATA_S *pstDataInfo,
			       CVI_U16 u16Width, CVI_U16 u16Height);

/**
 * @brief Create IVE_IMAGE_S structure, get ion memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg IVE_IMAGE_S structure.
 * @param enType Set image format.
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CreateImage(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg,
			    IVE_IMAGE_TYPE_E enType, uint16_t u16Width,
			    uint16_t u16Height);

/**
 * @brief Create IVE_IMAGE_S structure, get ion cache memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg IVE_IMAGE_S structure.
 * @param enType Set image format.
 * @param u16Width Set width.
 * @param u16Height Set Height.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CreateImage_Cached(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg,
			    IVE_IMAGE_TYPE_E enType, uint16_t u32Width,
			    uint16_t u32Height);

/**
 * @brief Flush IVE_IMAGE_S cache memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg IVE_IMAGE_S structure.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_BufFlush(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg);

/**
 * @brief Invalidate IVE_IMAGE_S cache memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg IVE_IMAGE_S structure.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_BufRequest(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg);

/**
 * @brief Write to file.
 *
 * @param pIveHandle Ive instance handler.
 * @param filename Filename to save.
 * @param pstMem Save IVE_DATA_S content.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_WriteData(IVE_HANDLE pIveHandle, const char *filename,
			  IVE_DATA_S *pstData);

/**
 * @brief Write to raw data.
 *
 * @param pIveHandle Ive instance handler.
 * @param filename Filename to save.
 * @param pstMem Save IVE_IMAGE_S content.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_WriteImg(IVE_HANDLE pIveHandle, const char *filename,
			 IVE_IMAGE_S *pstImg);

/**
 * @brief Write to png file.
 *
 * @param pIveHandle Ive instance handler.
 * @param filename Filename to save.
 * @param pstMem Save IVE_IMAGE_S content.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_WriteImage(IVE_HANDLE pIveHandle, const char *filename,
			 IVE_IMAGE_S *pstImg);

/**
 * @brief Write to file.
 *
 * @param pIveHandle Ive instance handler.
 * @param filename Filename to save.
 * @param pstMem Save IVE_MEM_INFO_S content.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_WriteMem(IVE_HANDLE pIveHandle, const char *filename,
			 IVE_MEM_INFO_S *pstMem);

/**
 * @brief Free ION memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstImg Free struct in IVE_IMAGE_S.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_FreeI(IVE_HANDLE pIveHandle, IVE_IMAGE_S *pstImg);

/**
 * @brief Free ION memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstMem Free struct in IVE_MEM_INFO_S.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_FreeM(IVE_HANDLE pIveHandle, IVE_MEM_INFO_S *pstMem);

/**
 * @brief Free ION memory.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstData Free struct in IVE_DATA_S.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_SYS_FreeD(IVE_HANDLE pIveHandle, IVE_DATA_S *pstData);

/**
 * @brief Reset ive register to default.
 *
 * @param pIveHandle Ive instance handler.
 * @param select reset ive module noumber.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_RESET(IVE_HANDLE pIveHandle, int select);

/**
 * @brief Dump ive register log.
 *
 * @param pIveHandle Ive instance handler.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_DUMP(IVE_HANDLE pIveHandle);

/**
 * @brief Split the DiffFg of BgModel result to yuv images.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstDiffFg Input image type is IVE_IMAGE_TYPE_S16C1.
 * @param pstBgDiffFg Output image type is IVE_IMAGE_TYPE_S8C1.
 * @param pstFrmDiffFg Output image type is IVE_IMAGE_TYPE_S8C1
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_DiffFg_Split(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstDiffFg,
			     IVE_DST_IMAGE_S *pstBgDiffFg,
			     IVE_DST_IMAGE_S *pstFrmDiffFg);

/**
 * @brief Split the ChgSta of BgModel result to yuv images.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstChgSta Input image type is IVE_IMAGE_TYPE_U32C1.
 * @param pstChgStaImg Output image type is IVE_IMAGE_TYPE_U8C1.
 * @param pstChgStaFg Output image type is IVE_IMAGE_TYPE_U8C1
 * @param pstChStaLift Output image type is IVE_IMAGE_TYPE_U16C1.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_ChgSta_Split(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstChgSta,
			     IVE_DST_IMAGE_S *pstChgStaImg,
			     IVE_DST_IMAGE_S *pstChgStaFg,
			     IVE_DST_IMAGE_S *pstChStaLift);

/**
 * @brief Queries the completion status of an existing task.
 *
 * @param pIveHandle Ive instance handler.
 * @param pbFinidh Output Pointer to the task completion status. It cannot be null.
 * @param bBlock Input Flag indicating whether a task is blocked.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_QUERY(IVE_HANDLE pIveHandle, CVI_BOOL *pbFinish,
		      CVI_BOOL bBlock);

/**
 * @brief DMA one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input data.
 * @param pstDst Output dma result.
 * @param pstCtrl DMA parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_DMA(IVE_HANDLE pIveHandle, IVE_DATA_S *pstSrc,
		    IVE_DST_DATA_S *pstDst, IVE_DMA_CTRL_S *pstCtrl,
		    CVI_BOOL bInstant);

/**
 * @brief AND two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output result.
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_And(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
		    CVI_BOOL bInstant);

/**
 * @brief OR two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output result.
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Or(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		   IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
		   CVI_BOOL bInstant);

/**
 * @brief XOR two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output result.
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Xor(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
		    CVI_BOOL bInstant);

/**
 * @brief ADD two image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output result.
 * @param pstCtrl Add control parameter.
 * @param bInstant Dummy variable.
 * @return CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Add(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
		    IVE_ADD_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief Subtract two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output result.
 * @param pstCtrl Subtract control parameter.
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Sub(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
		    IVE_SUB_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief Threshold one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output thresh result.
 * @param pstCtrl Thresh parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Thresh(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		       IVE_DST_IMAGE_S *pstDst, IVE_THRESH_CTRL_S *pstCtrl,
		       CVI_BOOL bInstant);

/**
 * @brief Dilate one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output dilate result.
 * @param pstCtrl Dilate parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Dilate(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		       IVE_DST_IMAGE_S *pstDst, IVE_DILATE_CTRL_S *pstCtrl,
		       CVI_BOOL bInstant);

/**
 * @brief Erode one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output erode result.
 * @param pstCtrl Erode parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Erode(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		      IVE_DST_IMAGE_S *pstDst, IVE_ERODE_CTRL_S *pstCtrl,
		      CVI_BOOL bInstant);

/**
 * @brief MatchBgModel.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstCurImg Input image.
 * @param pstBgModel Input BgModel.
 * @param pstFgFlag Input FgFlag.
 * @param pstDiffFg Output DiffFg.
 * //@param pstFrmDiffFg Output FrmDiffFg.
 * @param pstStatData Output StatData.
 * @param pstCtrl erode parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_MatchBgModel(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstCurImg,
			     IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag,
			     IVE_DST_IMAGE_S *pstDiffFg,
			     IVE_DST_MEM_INFO_S *pstStatData,
			     IVE_MATCH_BG_MODEL_CTRL_S *pstCtrl,
			     CVI_BOOL bInstant);

/**
 * @brief UpdateBgModel.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstBgModel Input BgModel.
 * @param pstFgFlag Input FgFlag
 * @param pstBgImg Output BgImg
 * //@param pstChgStaImg Output ChgStaImg
 * //@param pstChgStaFg Output ChgStaFg
 * @param pstChgSta Output chg state.
 * @param pstStatData Output state data.
 * @param pstCtrl erode parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_UpdateBgModel(IVE_HANDLE pIveHandle, IVE_DATA_S *pstBgModel,
			      IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstBgImg,
			      IVE_DST_IMAGE_S *pstChgSta,
			      IVE_DST_MEM_INFO_S *pstStatData,
			      IVE_UPDATE_BG_MODEL_CTRL_S *pstCtrl,
			      CVI_BOOL bInstant);

/**
 * @brief GMM.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstFg Output Fg result.
 * @param pstBg Output Bg result.
 * @param pstCtrl GMM parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_GMM(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		    IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg,
		    IVE_MEM_INFO_S *pstModel, IVE_GMM_CTRL_S *pstCtrl,
		    CVI_BOOL bInstant);

/**
 * @brief GMM2.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstFactor Input image factor.
 * @param pstFg Output Fg result.
 * @param pstBg Output Bg result.
 * @param pstMatchModelInfo Output MatchModelInfo.
 * @param pstModel Output model
 * @param pstCtrl GMM2 parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_GMM2(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		     IVE_SRC_IMAGE_S *pstFactor, IVE_DST_IMAGE_S *pstFg,
		     IVE_DST_IMAGE_S *pstBg, IVE_DST_IMAGE_S *pstMatchModelInfo,
		     IVE_MEM_INFO_S *pstModel, IVE_GMM2_CTRL_S *pstCtrl,
		     CVI_BOOL bInstant);

/**
 * @brief Bernsen one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output erode result.
 * @param pstCtrl Bernsen parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Bernsen(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			IVE_DST_IMAGE_S *pstDst, IVE_BERNSEN_CTRL_S *pstCtrl,
			CVI_BOOL bInstant);

/**
 * @brief Filter one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output filter result.
 * @param pstCtrl Filter parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Filter(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		       IVE_DST_IMAGE_S *pstDst, IVE_FILTER_CTRL_S *pstCtrl,
		       CVI_BOOL bInstant);

/**
 * @brief Sobel one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDstH Output Horizontal result.
 * @param pstDstV Output Vertical result.
 * @param pstCtrl Sobel parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Sobel(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		      IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV,
		      IVE_SOBEL_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief MagAndAng one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDstMag Output Mag result.
 * @param pstDstAmg Output Ang result.
 * @param pstCtrl MagAndAng parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_MagAndAng(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			  IVE_DST_IMAGE_S *pstDstMag,
			  IVE_DST_IMAGE_S *pstDstAng,
			  IVE_MAG_AND_ANG_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief CSC one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output CSC result.
 * @param pstCtrl CSC parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CSC(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		    IVE_DST_IMAGE_S *pstDst, IVE_CSC_CTRL_S *pstCtrl,
		    CVI_BOOL bInstant);

/**
 * @brief FilterAndCSC one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output FilterAndCSC result.
 * @param pstCtrl FilterAndCSC parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_FilterAndCSC(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			     IVE_DST_IMAGE_S *pstDst,
			     IVE_FILTER_AND_CSC_CTRL_S *pstCtrl,
			     CVI_BOOL bInstant);

/**
 * @brief Histogram one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output Histogram result.
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Hist(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		     IVE_DST_MEM_INFO_S *pstDst, CVI_BOOL bInstant);

/**
 * @brief MAP one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output Map result.
 * @param pstCtrl Map parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Map(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		    IVE_SRC_MEM_INFO_S *pstMap, IVE_DST_IMAGE_S *pstDst,
		    IVE_MAP_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief NCC two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output NCC result.
 * @param pstCtrl NCC parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_NCC(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_MEM_INFO_S *pstDst,
		    CVI_BOOL bInstant);

/**
 * @brief Integrogram one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output Integrogram result.
 * @param pstCtrl Integ parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Integ(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		      IVE_DST_MEM_INFO_S *pstDst, IVE_INTEG_CTRL_S *pstCtrl,
		      CVI_BOOL bInstant);

/**
 * @brief LBP one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output LBP result.
 * @param pstCtrl LBP parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_LBP(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
		    IVE_DST_IMAGE_S *pstDst, IVE_LBP_CTRL_S *pstCtrl,
		    CVI_BOOL bInstant);

/**
 * @brief Thresh_S16 one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output result.
 * @param pstCtrl Thresh_S16 parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Thresh_S16(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			   IVE_DST_IMAGE_S *pstDst,
			   IVE_THRESH_S16_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief Thresh_U16 one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output result.
 * @param pstCtrl Thresh_U16 parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Thresh_U16(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			   IVE_DST_IMAGE_S *pstDst,
			   IVE_THRESH_U16_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief 16BitTo8Bit one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output result.
 * @param pstCtrl 16BitTo8Bit parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_16BitTo8Bit(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			    IVE_DST_IMAGE_S *pstDst,
			    IVE_16BIT_TO_8BIT_CTRL_S *pstCtrl,
			    CVI_BOOL bInstant);

/**
 * @brief OrdStatFilter one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output result.
 * @param pstCtrl OrdStatFilter parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_OrdStatFilter(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			      IVE_DST_IMAGE_S *pstDst,
			      IVE_ORD_STAT_FILTER_CTRL_S *pstCtrl,
			      CVI_BOOL bInstant);

/**
 * @brief CannyEdge one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstEdge Output result.
 * @param pstCtrl CannyHysEdge parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
//CVI_S32 CVI_IVE_CannyEdge(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
//			  IVE_DST_IMAGE_S *pstEdge,
//			  IVE_CANNY_HYS_EDGE_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief CannyEdge one input image and output the result.
 *
 * @param pstEdge Input/Output.
 * @param pstStack Input/Output.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CannyEdge(IVE_IMAGE_S *pstEdge, IVE_MEM_INFO_S *pstStack);

/**
 * @brief CannyHysEdge one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstEdge Output result.
 * @param pstStack Output result.
 * @param pstCtrl CannyHysEdge parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CannyHysEdge(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			     IVE_DST_IMAGE_S *pstEdge,
			     IVE_DST_MEM_INFO_S *pstStack,
			     IVE_CANNY_HYS_EDGE_CTRL_S *pstCtrl,
			     CVI_BOOL bInstant);

/**
 * @brief NormGrad one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDstH Output result.
 * @param pstDstV Output result.
 * @param pstDstHV Output result.
 * @param pstCtrl NormGrad parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_NormGrad(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			 IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV,
			 IVE_DST_IMAGE_S *pstDstHV,
			 IVE_NORM_GRAD_CTRL_S *pstCtrl, CVI_BOOL bInstant);

/**
 * @brief GradFg.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstBgDiffFg Input BgDiffFg image.
 * @param pstCurGrad Input CurGrad image.
 * @param pstBgGrad Input BgGrad image.
 * @param pstGradFg Output result.
 * @param pstCtrl GradFg parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_GradFg(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstBgDiffFg,
		       IVE_SRC_IMAGE_S *pstCurGrad, IVE_SRC_IMAGE_S *pstBgGrad,
		       IVE_DST_IMAGE_S *pstGradFg, IVE_GRAD_FG_CTRL_S *pstCtrl,
		       CVI_BOOL bInstant);

/**
 * @brief SAD two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image.
 * @param pstSrc2 Input image.
 * @param pstSad Output result.
 * @param pstCtrl SAD parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_SAD(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
		    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstSad,
		    IVE_DST_IMAGE_S *pstThr, IVE_SAD_CTRL_S *pstCtrl,
		    CVI_BOOL bInstant);

/**
 * @brief Resize one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param astSrc Input images.
 * @param astDst Output images.
 * @param pstCtrl Resize parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_Resize(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S astSrc[],
		       IVE_DST_IMAGE_S astDst[], IVE_RESIZE_CTRL_S *pstCtrl,
		       CVI_BOOL bInstant);

/**
 * @brief imgInToOdma one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output image.
 * @param pstCtrl imgInToOdma parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_imgInToOdma(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			    IVE_DST_IMAGE_S *pstDst, IVE_FILTER_CTRL_S *pstCtrl,
			    CVI_BOOL bInstant);

/**
 * @brief rgbPToYuvToErodeToDilate one input image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input images.
 * @param pstDst Output image.
 * @param pstDst2 Output image.
 * @param pstCtrl rgbPToYuvToErodeToDilate parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_rgbPToYuvToErodeToDilate(IVE_HANDLE pIveHandle,
					 IVE_SRC_IMAGE_S *pstSrc,
					 IVE_DST_IMAGE_S *pstDst1,
					 IVE_DST_IMAGE_S *pstDst2,
					 IVE_FILTER_CTRL_S *pstCtrl,
					 CVI_BOOL bInstant);

/**
 * @brief STCandiCorner image and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc Input image.
 * @param pstDst Output image.
 * @param pstCtrl STCandiCorner parameter
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_STCandiCorner(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
			      IVE_DST_IMAGE_S *pstDst,
			      IVE_ST_CANDI_CORNER_CTRL_S *pstCtrl,
			      CVI_BOOL bInstant);

/**
 * @brief Frame Diff Motion two images and output the result.
 *
 * @param pIveHandle Ive instance handler.
 * @param pstSrc1 Input image 1.
 * @param pstSrc2 Input image 2.
 * @param pstDst Output result.
 * @param pstCtrl FrameDiffMotion control parameter.
 * @param bInstant Dummy variable.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_FrameDiffMotion(IVE_HANDLE pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
				IVE_SRC_IMAGE_S *pstSrc2,
				IVE_DST_IMAGE_S *pstDst,
				IVE_FRAME_DIFF_MOTION_CTRL_S *pstCtrl,
				CVI_BOOL bInstant);
/**
 * @brief CMDQ.
 *
 * @param pIveHandle Ive instance handler.
 * @return CVI_S32 CVI_S32 Return CVI_SUCCESS if succeed.
 */
CVI_S32 CVI_IVE_CMDQ(IVE_HANDLE pIveHandle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_IVE_H__
