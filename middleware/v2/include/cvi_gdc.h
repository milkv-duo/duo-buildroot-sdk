/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_gdc.h
 * Description:
 *   gdc interfaces.
 */

#ifndef __CVI_GDC_H__
#define __CVI_GDC_H__

#include <linux/cvi_common.h>
#include <linux/cvi_comm_video.h>
#include <linux/cvi_comm_gdc.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Begin a gdc job,then add task into the job,gdc will finish all the task in the job.
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_BeginJob(GDC_HANDLE *phHandle);

/**
 * @brief End a job,all tasks in the job will be submmitted to gdc
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_EndJob(GDC_HANDLE hHandle);

/**
 * @brief Cancel a job ,then all tasks in the job will not be submmitted to gdc
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_CancelJob(GDC_HANDLE hHandle);

/**
 * @brief Add a fisheye task to a gdc job
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @param pstTask(In), GDC task attribute
 * @param pstFisheyeAttr(In), for further settings
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_AddCorrectionTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask
	, const FISHEYE_ATTR_S *pstFisheyeAttr);

/**
 * @brief Add a rotation task to a gdc job
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @param pstTask(In), GDC task attribute
 * @param enRotation(In), for further settings
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_AddRotationTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, ROTATION_E enRotation);

/**
 * @brief Add a affine task to a gdc job
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @param pstTask(In), GDC task attribute
 * @param pstAffineAttr(In), for further settings
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_AddAffineTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, const AFFINE_ATTR_S *pstAffineAttr);

/**
 * @brief Add a LDC task to a gdc job
 *
 * @param pstTask(In), GDC task attribute.
 * @param pstLDCAttr(In), GDC LDC attribute
 * @param enRotation(In), for further settings
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_AddLDCTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask
	, const LDC_ATTR_S *pstLDCAttr, ROTATION_E enRotation);

/**
 * @brief this function is abandoned, do not support yet.
 */
CVI_S32 CVI_GDC_AddCnvWarpTask(const float *pfmesh_data, GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask
	, const FISHEYE_ATTR_S *pstAffineAttr, bool *bReNew);

/**
 * @brief this function is abandoned, do not support yet.
 */
CVI_S32 CVI_GDC_AddCorrectionTaskCNV(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask
	, const FISHEYE_ATTR_S *pstFishEyeAttr, uint8_t *p_tbl, uint8_t *p_idl, uint32_t *tbl_param);

/**
 * @brief set meshsize for GDC
 *
 * @param nMeshHor(In), mesh counts horizontal
 * @param nMeshVer(In), mesh counts vertical
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_SetMeshSize(int nMeshHor, int nMeshVer);

/**
 * @brief gen GDC LDC(Lens Distortion Correction) mesh table
 *
 * @param u32Width(In), mesh region width
 * @param u32Height(In), mesh region height
 * @param pstLDCAttr(In), GDC LDC attribute
 * @param pu64PhyAddr(Out), mesh table physical addr in memory
 * @param ppVirAddr(Out), mesh table virtual addr in memory
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_GenLDCMesh(CVI_U32 u32Width, CVI_U32 u32Height, const LDC_ATTR_S *pstLDCAttr
	, const char *name, CVI_U64 *pu64PhyAddr, CVI_VOID **ppVirAddr);

CVI_S32 CVI_GDC_LoadLDCMesh(CVI_U32 u32Width, CVI_U32 u32Height, const char *fileNname
	, const char *tskName, CVI_U64 *pu64PhyAddr, CVI_VOID **ppVirAddr);

/**
 * @brief set GDC wrap buf attribute
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @param pstTask(In), GDC task attribute
 * @param pstBufWrap(In), GDC wrap buf attribute
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_SetBufWrapAttr(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, const DWA_BUF_WRAP_S *pstBufWrap);

/**
 * @brief get GDC wrap buf attribute
 *
 * @param hHandle(In), handle creat by CVI_GDC_BeginJob
 * @param pstTask(In), GDC task attribute
 * @param pstBufWrap(Out), GDC wrap buf attribute
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_GetBufWrapAttr(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, DWA_BUF_WRAP_S *pstBufWrap);

/**
 * @brief dump mesh data for GDC
 *
 * @param pMeshDumpAttr(In), mesh dump attribute
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_DumpMesh(MESH_DUMP_ATTR_S *pMeshDumpAttr);

/**
 * @brief load mesh for GDC, mesh data is load from user
 *
 * @param pMeshDumpAttr(In), mesh dump attribute
 * @return CVI_S32 Return CVI_SUCCESS if succeed
 */
CVI_S32 CVI_GDC_LoadMesh(MESH_DUMP_ATTR_S *pMeshDumpAttr);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_GDC_H__ */
