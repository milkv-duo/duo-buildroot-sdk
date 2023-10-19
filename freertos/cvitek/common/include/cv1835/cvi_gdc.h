/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_gdc.h
 * Description:
 *   gdc interfaces.
 */

#ifndef __CVI_GDC_H__
#define __CVI_GDC_H__

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Begin a gdc job,then add task into the job,gdc will finish all the task in the job.
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_BeginJob(GDC_HANDLE *phHandle);

/* End a job,all tasks in the job will be submmitted to gdc
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_EndJob(GDC_HANDLE hHandle);

/* Cancel a job ,then all tasks in the job will not be submmitted to gdc
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_CancelJob(GDC_HANDLE hHandle);

/* Add a fisheye task to a gdc job
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @param pstTask: to describe what to do
 * @param pstFisheyeAttr: for further settings
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_AddCorrectionTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
				  const FISHEYE_ATTR_S *pstFisheyeAttr);

/* Add a rotation task to a gdc job
 *
 * @param phHandle: GDC_HANDLE *phHandle
 * @param pstTask: to describe what to do
 * @param enRotation: for further settings
 * @return Error code (0 if successful)
 */
CVI_S32 CVI_GDC_AddRotationTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, ROTATION_E enRotation);

CVI_S32 CVI_GDC_AddAffineTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, const AFFINE_ATTR_S *pstAffineAttr);

CVI_S32 CVI_GDC_AddLDCTask(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask, const LDC_ATTR_S *pstLDCAttr);

// color night vision
CVI_S32 CVI_GDC_AddCnvWarpTask(const float *pfmesh_data, GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
			       const FISHEYE_ATTR_S *pstAffineAttr, bool *bReNew);

CVI_S32 CVI_GDC_AddCorrectionTaskCNV(GDC_HANDLE hHandle, const GDC_TASK_ATTR_S *pstTask,
		const FISHEYE_ATTR_S *pstFishEyeAttr, uint8_t *p_tbl, uint8_t *p_idl, uint32_t *tbl_param);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CVI_GDC_H__ */
