#ifndef __CVI_SCENE_INNER_H__
#define __CVI_SCENE_INNER_H__

#include "cvi_scene.h"
#include <pthread.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define TIME_NORMAL_THREAD			(300000)
#define TIME_LUMINANCE_THREAD		(100000)
#define TIME_NOTLINEAR_THREAD		(100000)

typedef enum _SCENE_THREAD_TYPE_E {
	SCENE_THREAD_TYPE_NORMAL = 0,
	SCENE_THREAD_TYPE_LUMINANCE,
	SCENE_THREAD_TYPE_NOTLINEAR,
	SCENE_THREAD_PIPE_TYPE_BUTT
} SCENE_THREAD_TYPE_E;

typedef struct _SCENE_THREAD_S {
	CVI_BOOL    bThreadFlag;
	pthread_t   pThread;
} SCENE_THREAD_S;

typedef struct _SCENE_MAINPIPE_STATE_S {
	CVI_HANDLE  MainPipeHdl;
	CVI_U32     u32SubPipeNum;
	CVI_HANDLE  aSubPipeHdl[CVI_SCENE_PIPE_MAX_NUM];

	CVI_BOOL    bFixedAEMatrix;
	CVI_BOOL    bDISEnable;

	CVI_U64     u64LastNormalExposure;
	CVI_U32     u32LastNormalIso;
	CVI_S32     s32LastNormalLV;
	CVI_U64     u64LastLuminanceExposure;
	CVI_U32     u32LastLuminanceISO;
	CVI_S32     s32LastLuminanceLV;
	CVI_U64     u64LastNotLinearExposure;
	CVI_U32     u32LastNotLinearISO;
	CVI_S32     s32LastNotLinearLV;
	CVI_U32     u32Iso;
	CVI_U64     u64Exposure;
	CVI_S32     s32LV;
	CVI_U32     u32ActRation;
	CVI_U32     u32HDRBrightRatio;
} SCENE_MAINPIPE_STATE_S;

typedef struct _SCENE_STATE_S {
	CVI_BOOL    bSceneInit;
	CVI_BOOL    bPause;

	SCENE_THREAD_S stThreadNormal;
	SCENE_THREAD_S stThreadLuminance;
	SCENE_THREAD_S stThreadNotLinear;

	pthread_attr_t stThreadNormalAttr;
	pthread_attr_t stThreadLuminanceAttr;
	pthread_attr_t stThreadNotLinearAttr;

	CVI_U32     u32MainPipeNum;
	SCENE_MAINPIPE_STATE_S astMainPipe[CVI_SCENE_PIPE_MAX_NUM];
} SCENE_STATE_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_SCENE_INNER_H__
