#ifndef __CVI_SCENE_H__
#define __CVI_SCENE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
#include "cvi_type.h"
#elif defined(__CV181X__) || defined(__CV180X__)
#include <linux/cvi_type.h>
#endif // SOCs

#include "cvi_scene_setparam.h"

#define CVI_SCENE_PIPE_MAX_NUM			(8)

/** error code define */
#define CVI_SCENE_EINVAL				CVI_SCENECOMM_ERR_ID(1)
#define CVI_SCENE_ENOTINIT				CVI_SCENECOMM_ERR_ID(2)
#define CVI_SCENE_ENONPTR				CVI_SCENECOMM_ERR_ID(3)
#define CVI_SCENE_EOUTOFRANGE			CVI_SCENECOMM_ERR_ID(4)
#define CVI_SCENE_EINTER				CVI_SCENECOMM_ERR_ID(5)
#define CVI_SCENE_EINITIALIZED			CVI_SCENECOMM_ERR_ID(6)

typedef struct _SCENE_PIPE_ATTR_S {
	CVI_BOOL    bEnable;
	CVI_HANDLE  MainPipeHdl;
	CVI_HANDLE  VcapPipeHdl;
	CVI_HANDLE  PipeChnHdl;
	CVI_U8      u8PipeParamIndex;
} CVI_SCENE_PIPE_ATTR_S;

typedef enum _SCENE_PIPE_MODE_E {
	CVI_SCENE_PIPE_MODE_LINEAR = 0,
	CVI_SCENE_PIPE_MODE_WDR
} CVI_SCENE_PIPE_MODE_E;

typedef struct _SCENE_MODE_S {
	CVI_SCENE_PIPE_MODE_E enPipeMode;
	CVI_SCENE_PIPE_ATTR_S astPipeAttr[CVI_SCENE_PIPE_MAX_NUM];
} CVI_SCENE_MODE_S;

typedef struct _SCENE_PARAM_S {
	CVI_SCENE_PIPE_PARAM_S astPipeParam[CVI_SCENE_PIPETYPE_NUM];
} CVI_SCENE_PARAM_S;

CVI_S32 CVI_SCENE_Init(const CVI_SCENE_PARAM_S *pstSceneParam);
CVI_S32 CVI_SCENE_Deinit(void);
CVI_S32 CVI_SCENE_SetSceneMode(const CVI_SCENE_MODE_S *pstSceneMode);
CVI_S32 CVI_SCENE_GetSceneMode(CVI_SCENE_MODE_S *pstSceneMode);
CVI_S32 CVI_SCENE_Pause(CVI_BOOL bEnable);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_SCENE_H__
