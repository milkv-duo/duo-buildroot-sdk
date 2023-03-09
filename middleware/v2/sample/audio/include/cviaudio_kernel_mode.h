#ifndef __CVIAUDIO_KERNEL_MODE_H__
#define __CVIAUDIO_KERNEL_MODE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sched.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
/* usage from cvi audio header */
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include "cvi_comm_aio.h"
#include "cvi_audio.h"
#include "cviaudio_ioctl_cmd.h"
/* include the kernel command header*/
//#define CVIAUDIO_SHARE_MEM_SIZE	(0x32000)

extern CVI_S32 s32AudioCore;//control audio device id
//kernel mode api:cviaudio_core.ko insmod require
CVI_S32 CVI_AI_EnableExtSSp(CVI_S32 s32SampleRate, CVI_S32 s32PeriodSize,
				const AI_TALKVQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AI_GetFrameExtSsp(AUDIO_FRAME_S *pstFrm);
CVI_S32 CVI_AI_DisableExtSsp(void);
//block mode api:cviaudio_core.ko do not require
//BlkMode = BLOCK MODE -> call to rtos_cmdqu.ko directly
CVI_S32 CVI_AI_EnableExtSSp_BlkMode(CVI_S32 s32SampleRate, CVI_S32 s32PeriodSize,
				const AI_TALKVQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AI_GetFrameExtSsp_BlkMode(CVI_CHAR *datain,
					CVI_CHAR *dataout,
					CVI_S32 s32SizeInBytes,
					CVI_S32 *s32SizeOutBytes);
CVI_S32 CVI_AI_DisableExtSsp_BlkMode(void);

CVI_BOOL _cviaudio_check_audio_kernel_device(void);
CVI_S32 _cviaudio_open_cviaudio_device(void);

#ifdef __cplusplus
}
#endif
#endif
