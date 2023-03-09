/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_ive_ioctl.h
 * Description:
 */

#ifndef __CVI_IVE_IOCTL_H__
#define __CVI_IVE_IOCTL_H__

#include "cvi_comm_ive.h"
#include "cvi_errno.h"

struct cvi_ive_test_arg {
	IVE_IMAGE_TYPE_E enType;
	char *pAddr;
#ifdef __arm__
	__u32 padding;
#endif
	CVI_U16 u16Width;
	CVI_U16 u16Height;
};

struct cvi_ive_query_arg {
	IVE_HANDLE pIveHandle;
	CVI_BOOL *pbFinish;
#ifdef __arm__
	__u32 padding;
#endif
	CVI_BOOL bBlock;
};

struct cvi_ive_ioctl_add_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stDst;
	IVE_ADD_CTRL_S pstCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_and_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stDst;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_xor_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stDst;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_or_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stDst;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_sub_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stDst;
	IVE_SUB_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_erode_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_ERODE_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_dilate_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_DILATE_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_thresh_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_THRESH_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_match_bgmodel_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stCurImg;
	IVE_DATA_S stBgModel;
	IVE_IMAGE_S stFgFlag;
	IVE_DST_IMAGE_S stDiffFg;
	IVE_DST_MEM_INFO_S stStatData;
	IVE_MATCH_BG_MODEL_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_update_bgmodel_arg {
	IVE_HANDLE pIveHandle;
	IVE_DATA_S stBgModel;
	IVE_IMAGE_S stFgFlag;
	IVE_DST_IMAGE_S stBgImg;
	IVE_DST_IMAGE_S stChgSta;
	IVE_DST_MEM_INFO_S stStatData;
	IVE_UPDATE_BG_MODEL_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_gmm_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stFg;
	IVE_DST_IMAGE_S stBg;
	IVE_MEM_INFO_S stModel;
	IVE_GMM_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_gmm2_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_SRC_IMAGE_S stFactor;
	IVE_DST_IMAGE_S stFg;
	IVE_DST_IMAGE_S stBg;
	IVE_DST_IMAGE_S stInfo;
	IVE_MEM_INFO_S stModel;
	IVE_GMM2_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_dma_arg {
	IVE_HANDLE pIveHandle;
	IVE_DATA_S stSrc;
	IVE_DST_DATA_S stDst;
	IVE_DMA_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_bernsen_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_BERNSEN_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_filter_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_FILTER_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_sobel_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDstH;
	IVE_DST_IMAGE_S stDstV;
	IVE_SOBEL_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_maganang_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDstMag;
	IVE_DST_IMAGE_S stDstAng;
	IVE_MAG_AND_ANG_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_csc_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_CSC_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_hist_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_MEM_INFO_S stDst;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_filter_and_csc_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_FILTER_AND_CSC_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_map_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_SRC_MEM_INFO_S stMap;
	IVE_DST_IMAGE_S stDst;
	IVE_MAP_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_ncc_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_MEM_INFO_S stDst;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_integ_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_MEM_INFO_S stDst;
	IVE_INTEG_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_lbp_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_LBP_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_thresh_s16_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_THRESH_S16_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_thres_su16_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_THRESH_U16_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_16bit_to_8bit_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_16BIT_TO_8BIT_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_ord_stat_filter_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_ORD_STAT_FILTER_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_canny_hys_edge_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_DST_MEM_INFO_S stStack;
	IVE_CANNY_HYS_EDGE_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_norm_grad_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDstH;
	IVE_DST_IMAGE_S stDstV;
	IVE_DST_IMAGE_S stDstHV;
	IVE_NORM_GRAD_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_grad_fg_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stBgDiffFg;
	IVE_SRC_IMAGE_S stCurGrad;
	IVE_SRC_IMAGE_S stBgGrad;
	IVE_DST_IMAGE_S stGradFg;
	IVE_GRAD_FG_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_sad_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stSad;
	IVE_DST_IMAGE_S stThr;
	IVE_SAD_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_resize_arg {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S *astSrc;
#ifdef __arm__
		__u32 padding1;
#endif
	IVE_DST_IMAGE_S *astDst;
#ifdef __arm__
		__u32 padding2;
#endif
	IVE_RESIZE_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_rgbPToYuvToErodeToDilate {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst1;
	IVE_DST_IMAGE_S stDst2;
	IVE_FILTER_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_stcandicorner {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc;
	IVE_DST_IMAGE_S stDst;
	IVE_ST_CANDI_CORNER_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

struct cvi_ive_ioctl_md {
	IVE_HANDLE pIveHandle;
	IVE_SRC_IMAGE_S stSrc1;
	IVE_SRC_IMAGE_S stSrc2;
	IVE_DST_IMAGE_S stDst;
	IVE_FRAME_DIFF_MOTION_CTRL_S stCtrl;
	CVI_BOOL bInstant;
};

#define CVI_IVE_IOC_MAGIC 'v'
#define CVI_IVE_IOC_TEST _IOW(CVI_IVE_IOC_MAGIC, 0x00, unsigned long long)
#define CVI_IVE_IOC_DMA _IOW(CVI_IVE_IOC_MAGIC, 0x01, unsigned long long)
#define CVI_IVE_IOC_Add _IOW(CVI_IVE_IOC_MAGIC, 0x02, unsigned long long)
#define CVI_IVE_IOC_And _IOW(CVI_IVE_IOC_MAGIC, 0x03, unsigned long long)
#define CVI_IVE_IOC_Or _IOW(CVI_IVE_IOC_MAGIC, 0x04, unsigned long long)
#define CVI_IVE_IOC_Sub _IOW(CVI_IVE_IOC_MAGIC, 0x05, unsigned long long)
#define CVI_IVE_IOC_Xor _IOW(CVI_IVE_IOC_MAGIC, 0x06, unsigned long long)
#define CVI_IVE_IOC_Thresh _IOW(CVI_IVE_IOC_MAGIC, 0x07, unsigned long long)
#define CVI_IVE_IOC_Thresh_S16 _IOW(CVI_IVE_IOC_MAGIC, 0x08, unsigned long long)
#define CVI_IVE_IOC_Thresh_U16 _IOW(CVI_IVE_IOC_MAGIC, 0x09, unsigned long long)
#define CVI_IVE_IOC_16BitTo8Bit _IOW(CVI_IVE_IOC_MAGIC, 0x0a, unsigned long long)
#define CVI_IVE_IOC_CSC _IOW(CVI_IVE_IOC_MAGIC, 0x0b, unsigned long long)
#define CVI_IVE_IOC_GradFg _IOW(CVI_IVE_IOC_MAGIC, 0x0c, unsigned long long)
#define CVI_IVE_IOC_NormGrad _IOW(CVI_IVE_IOC_MAGIC, 0x0d, unsigned long long)
#define CVI_IVE_IOC_Filter _IOW(CVI_IVE_IOC_MAGIC, 0x0e, unsigned long long)
#define CVI_IVE_IOC_FilterAndCSC _IOW(CVI_IVE_IOC_MAGIC, 0x0f, unsigned long long)
#define CVI_IVE_IOC_Hist _IOW(CVI_IVE_IOC_MAGIC, 0x10, unsigned long long)
#define CVI_IVE_IOC_EqualizeHist _IOW(CVI_IVE_IOC_MAGIC, 0x11, unsigned long long)
#define CVI_IVE_IOC_Map _IOW(CVI_IVE_IOC_MAGIC, 0x12, unsigned long long)
#define CVI_IVE_IOC_NCC _IOWR(CVI_IVE_IOC_MAGIC, 0x13, unsigned long long)
#define CVI_IVE_IOC_OrdStatFilter _IOW(CVI_IVE_IOC_MAGIC, 0x14, unsigned long long)
#define CVI_IVE_IOC_Resize _IOW(CVI_IVE_IOC_MAGIC, 0x15, unsigned long long)
#define CVI_IVE_IOC_CannyHysEdge _IOW(CVI_IVE_IOC_MAGIC, 0x16, unsigned long long)
#define CVI_IVE_IOC_CannyEdge _IOW(CVI_IVE_IOC_MAGIC, 0x17, unsigned long long)
#define CVI_IVE_IOC_Integ _IOW(CVI_IVE_IOC_MAGIC, 0x18, unsigned long long)
#define CVI_IVE_IOC_LBP _IOW(CVI_IVE_IOC_MAGIC, 0x19, unsigned long long)
#define CVI_IVE_IOC_MagAndAng _IOW(CVI_IVE_IOC_MAGIC, 0x1a, unsigned long long)
#define CVI_IVE_IOC_STCandiCorner _IOW(CVI_IVE_IOC_MAGIC, 0x1b, unsigned long long)
#define CVI_IVE_IOC_STCorner _IOW(CVI_IVE_IOC_MAGIC, 0x1c, unsigned long long)
#define CVI_IVE_IOC_Sobel _IOW(CVI_IVE_IOC_MAGIC, 0x1d, unsigned long long)
#define CVI_IVE_IOC_CCL _IOW(CVI_IVE_IOC_MAGIC, 0x1e, unsigned long long)
#define CVI_IVE_IOC_Dilate _IOW(CVI_IVE_IOC_MAGIC, 0x1f, unsigned long long)
#define CVI_IVE_IOC_Erode _IOW(CVI_IVE_IOC_MAGIC, 0x20, unsigned long long)
#define CVI_IVE_IOC_MatchBgModel _IOWR(CVI_IVE_IOC_MAGIC, 0x21, unsigned long long)
#define CVI_IVE_IOC_UpdateBgModel _IOWR(CVI_IVE_IOC_MAGIC, 0x22, unsigned long long)
#define CVI_IVE_IOC_GMM _IOW(CVI_IVE_IOC_MAGIC, 0x23, unsigned long long)
#define CVI_IVE_IOC_GMM2 _IOW(CVI_IVE_IOC_MAGIC, 0x24, unsigned long long)
#define CVI_IVE_IOC_LKOpticalFlowPyr                                           \
	_IOW(CVI_IVE_IOC_MAGIC, 0x25, unsigned long long)
#define CVI_IVE_IOC_SAD _IOW(CVI_IVE_IOC_MAGIC, 0x26, unsigned long long)
#define CVI_IVE_IOC_Bernsen _IOW(CVI_IVE_IOC_MAGIC, 0x27, unsigned long long)
#define CVI_IVE_IOC_imgInToOdma _IOW(CVI_IVE_IOC_MAGIC, 0x28, unsigned long long)
#define CVI_IVE_IOC_rgbPToYuvToErodeToDilate                                   \
	_IOW(CVI_IVE_IOC_MAGIC, 0x29, unsigned long long)
#define CVI_IVE_IOC_MD _IOW(CVI_IVE_IOC_MAGIC, 0x2a, unsigned long long)
#define CVI_IVE_IOC_CMDQ _IOW(CVI_IVE_IOC_MAGIC, 0x2b, unsigned long long)
#define CVI_IVE_IOC_RESET _IOW(CVI_IVE_IOC_MAGIC, 0xF0, unsigned long long)
#define CVI_IVE_IOC_DUMP _IO(CVI_IVE_IOC_MAGIC, 0xF1)
#define CVI_IVE_IOC_QUERY _IOWR(CVI_IVE_IOC_MAGIC, 0xF2, unsigned long long)
#endif /* __CVI_IVE_IOCTL_H__ */
