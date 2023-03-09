#ifndef __CVI_SCENECOMM_H__
#define __CVI_SCENECOMM_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#if defined(ARCH_CV183X) || defined(ARCH_CV182X)
#include "cvi_type.h"
#elif defined(__CV180X__) || defined(__CV181X__)
#include <linux/cvi_type.h>
#endif // SOCs

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// color log macro define
#define NONE        "\033[m"
#define RED         "\033[0;32;31m"
#define GREEN       "\033[0;32;32m"
#define BLUE        "\033[0;32;34m"
#define YELLOW      "\033[1;33m"


/** Pointer Check */
#define CVI_SCENECOMM_CHECK_POINTER(p, errcode) \
	do { \
		if (!(p)) { \
			printf("null pointer\n"); \
			return errcode; \
		} \
	} while (0)

/** Return Result Check */
#define CVI_SCENECOMM_CHECK_RETURN(ret, errcode) \
	do { \
		if (ret != CVI_SUCCESS) { \
			printf(RED" ret[%08x] errcode[%x]"NONE"\n", ret, errcode); \
			return errcode; \
		} \
	} while (0)

#define CVI_SCENECOMM_CHECK(ret, errcode) \
	do { \
		if (ret != CVI_SUCCESS) { \
			printf(RED" ret[%08x] errcode[%x]"NONE"\n", ret, errcode); \
		} \
	} while (0)

#define CVI_MUTEX_INIT_LOCK(mutex)	pthread_mutex_init(&mutex, NULL)
#define CVI_MUTEX_LOCK(mutex)		pthread_mutex_lock(&mutex)
#define CVI_MUTEX_UNLOCK(mutex)		pthread_mutex_unlock(&mutex)
#define CVI_MUTEX_DESTROY(mutex)	pthread_mutex_destroy(&mutex)

#define CVI_SCENECOMM_ERR_BASE_ID (0x80000000L)
#define CVI_SCENECOMM_ERR_ID(err) \
	((CVI_S32)((CVI_SCENECOMM_ERR_BASE_ID) | (err)))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __CVI_SCENECOMM_H__
