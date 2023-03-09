#ifndef __CVI_SCENE_LOADPARAM_H__
#define __CVI_SCENE_LOADPARAM_H__

#include "cvi_scene.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SCENE_MAX_VIDEOMODE		(7)

typedef struct _SCENE_VIDEO_MODE_S {
	CVI_SCENE_MODE_S astVideoMode[SCENE_MAX_VIDEOMODE];
} CVI_SCENE_VIDEO_MODE_S;

CVI_S32 CVI_SCENE_CreateParam(CVI_CHAR *pszDirname,
	CVI_SCENE_PARAM_S *pstSceneParam, CVI_SCENE_VIDEO_MODE_S *pstVideoMode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_SCENE_LOADPARAM_H__
