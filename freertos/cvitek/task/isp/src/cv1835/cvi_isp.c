#include "linux/types.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS_POSIX.h"
#include <FreeRTOS_POSIX/errno.h>
#include "FreeRTOS_POSIX/pthread.h"
//#include <unistd.h> // for usleep
#include "linux/ioctl.h"
#include "linux/fs.h"
//#include <linux/fcntl.h>
//#include <sys/stat.h>

//#include "cvi_base.h"
//#include "cvi_isp.h"
//#include "isp_main.h"
//#include "isp_debug.h"
//#include "isp_defines.h"
//#include "isp_dpc.h"
//#include "isp_defaults.h"
//#include "isp_buf_ctrl.h"
//#include "isp_mock.h"
//#include "cvi_base.h"
//#include "cvi_sys.h"
//#include "isp_3a.h"
//#include "isp_version.h"
//#include "linux/cvi_vip.h"
//#include "linux/cvi_vip_isp.h"
//#include "linux/cvi_vip_tun_cfg.h"
//#include "ioctl_isp.h"
#include "cvi_sns_ctrl.h"
#include "linux/errno.h"

//#include "isp_ipc.h"
//#include "cvi_pqbin.h"

//#define	TUNING_FOR_DUAL_SENSOR_READY	0	// TODO@Kidd fix this

#define PRINT_ATTR_AUTO	0
#define PRINT_ATTR(attr, reg, min, max) ISP_DEBUG(LOG_DEBUG, "%s(%s) [%d-%d] = %d\n", #attr, #reg, min, max, attr)
#if 0
ISP_CTX_S g_astIspCtx[VI_MAX_PIPE_NUM] = { { 0 } };

ISP_PARAMETER_BUFFER g_param[VI_MAX_PIPE_NUM];
CVI_BOOL g_isp_debug_print_mpi = 1;
CVI_BOOL g_isp_debug_diff_only = 1;
CVI_BOOL g_isp_thread_run = CVI_FALSE;

// AE/AWB Debug callback function
ISP_AE_DEBUG_FUNC_S aeAlgoInternalLibReg[VI_MAX_PIPE_NUM] = {
	[0 ... VI_MAX_PIPE_NUM - 1].pfn_ae_dumplog = NULL,
	[0 ... VI_MAX_PIPE_NUM - 1].pfn_ae_setdumplogpath = NULL
};

ISP_AWB_DEBUG_FUNC_S awbAlgoInternalLibReg[VI_MAX_PIPE_NUM] = {
	[0 ... VI_MAX_PIPE_NUM - 1].pfn_awb_dumplog = NULL,
	[0 ... VI_MAX_PIPE_NUM - 1].pfn_awb_setdumplogpath = NULL
};

static CVI_S32 __subscribe_event(CVI_S32 fd, CVI_U32 event)
{
	struct v4l2_event_subscription sub;
	CVI_S32 ret = 0;

	memset(&sub, 0, sizeof(sub));
	sub.type = event;

	ret = ioctl(fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	if (ret != 0) {
		ISP_DEBUG(LOG_ERR, "IOCTL subscribe event fail, %d %s\n", errno, strerror(errno));
		return ret;
	}

	return ret;
}

static CVI_S32 __unsubscribe_event(CVI_S32 fd, CVI_U32 event)
{
	struct v4l2_event_subscription sub;
	CVI_S32 ret;

	memset(&sub, 0, sizeof(sub));
	sub.type = event;

	ret = ioctl(fd, VIDIOC_UNSUBSCRIBE_EVENT, &sub);
	if (ret != 0) {
		ISP_DEBUG(LOG_ERR, "IOCTL unsubscribe event fail, %d %s\n", errno, strerror(errno));
		return ret;
	}

	return ret;
}

static CVI_S32 check_enable_debug_message_to_stdout(void)
{
#define ENV_DEBUG_MESSAGE_TO_STDOUT		"ISP_ENABLE_STDOUT_DEBUG"
	char *pszEnv = getenv(ENV_DEBUG_MESSAGE_TO_STDOUT);
	char *pszEnd = NULL;

	if (pszEnv == NULL)
		return CVI_FAILURE;

	CVI_U32 u32Level = (CVI_U32)strtol(pszEnv, &pszEnd, 10);

	if (pszEnv == pszEnd) {
		ISP_DEBUG(LOG_INFO, "ISP_ENABLE_STDOUT_DEBUG invalid\n");
		return CVI_FAILURE;
	}

	gExportToStdout = (u32Level > 0) ? CVI_TRUE : CVI_FALSE;
	return CVI_SUCCESS;
}

static CVI_S32 apply_debug_level_from_environment(void)
{
#define ENV_DEBUG_LEVEL		"ISP_DEBUG_LEVEL"
	char *pszEnv = getenv(ENV_DEBUG_LEVEL);
	char *pszEnd = NULL;

	if (pszEnv == NULL)
		return CVI_FAILURE;

	CVI_UL ulLevel = strtol(pszEnv, &pszEnd, 10);

	if ((ulLevel > LOG_DEBUG) || (pszEnv == pszEnd)) {
		ISP_DEBUG(LOG_INFO, "ISP_DEBUG_LEVEL invalid\n");
		return CVI_FAILURE;
	}

	CVI_DEBUG_SetDebugLevel((int)ulLevel);
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_Init(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_DEV(ViPipe);

	apply_debug_level_from_environment();
	check_enable_debug_message_to_stdout();
#ifndef ISP_VERSION
#define ISP_VERSION "-"
#endif
	ISP_DEBUG(LOG_INFO, "ISP VERSION(%s)\n", ISP_VERSION);
	isp_global_init(ViPipe);
	isp_3aInfo_init(ViPipe);
	isp_param_global_init(ViPipe);

	for (CVI_U32 i = 0; i < AAA_TYPE_MAX; i++)
		isp_3aLib_init(ViPipe, i);

	/*Move here for check wdr mode.*/
	// BIN file will overwrite AE/AWB settings
	isp_iqParam_addr_initDefault(ViPipe);
	isp_sns_info_init(ViPipe);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_MemInit(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_DEV(ViPipe);

	ISP_CTX_S *pstIspCtx = NULL;
	struct vdev *d;

	d = get_dev_info(VDEV_TYPE_ISP, ViPipe);

	ISP_GET_CTX(ViPipe, pstIspCtx);
	pstIspCtx->ispDevFd = d->fd;

	if (!IS_VDEV_OPEN(d->state)) {
		ISP_DEBUG(LOG_ERR, "Pipe(%d) state(%d) incorrect\n", ViPipe, d->state);
		return -EBADF;
	}

	CVI_S32 fd = d->fd;

	if (fd == -1)
		return -EBADF;

	// TODO: get sns info to init

	// TODO: get mem
	//v4l2_qbuf(fd, V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE, &buf);
	isp_iqParam_addr_init(ViPipe);
	isp_buf_ctrl_tbl_init(ViPipe);
	isp_buf_ctrl_algo_buf_init(ViPipe);
	isp_tuningBuf_init(ViPipe);
	isp_drcHist_buf_init(ViPipe);

	return CVI_SUCCESS;
}

#ifdef ISP_RECEIVE_IPC_CMD
static CVI_S32 parse_isp_fifo_path_from_environment(char *fifo_path, CVI_U32 path_length)
{
	char *env = getenv(ENV_ISP_FIFO_IPC_NAME);

	if (env == NULL) {
		snprintf(fifo_path, path_length, DEFAULT_ISP_FIFO_IPC_FILENAME);
		return CVI_FAILURE;
	}

	snprintf(fifo_path, path_length, env);
	return CVI_SUCCESS;
}

static CVI_S32 initial_ipc_fifo(int *fifoFd, const char *fifo_path)
{
	if (access(fifo_path, F_OK) != 0)
		if (mkfifo(fifo_path, 0777) != 0) {
			ISP_DEBUG(LOG_ERR, "Create ISP FIFO file fail\n");
			return CVI_FAILURE;
		}

	*fifoFd = open(fifo_path, O_RDWR);
	if (*fifoFd == -1) {
		ISP_DEBUG(LOG_ERR, "Open ISP FIFO file fail\n");
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

static CVI_S32 release_ipc_fifo(const char *fifo_path)
{
	if (access(fifo_path, F_OK) == 0)
		if (unlink(fifo_path) != 0) {
			ISP_DEBUG(LOG_ERR, "Remove ISP FIFO file fail\n");
			return CVI_FAILURE;
		}

	return CVI_SUCCESS;
}

static CVI_S32 handle_fifo_command(const char *command)
{
	size_t ae_log_location_len = strlen(SET_AE_LOG_LOCATION_CMD);
	size_t awb_log_location_len = strlen(SET_AWB_LOG_LOCATION_CMD);

	if (strcmp(TRIGGER_AE_LOG_CMD, command) == 0) {
		for (int pipeId = 0; pipeId < VI_MAX_PIPE_NUM; ++pipeId) {
			if (aeAlgoInternalLibReg[pipeId].pfn_ae_dumplog != NULL) {
				aeAlgoInternalLibReg[pipeId].pfn_ae_dumplog();
			}
		}
	} else if (strcmp(TRIGGER_AWB_LOG_CMD, command) == 0) {
		for (int pipeId = 0; pipeId < VI_MAX_PIPE_NUM; ++pipeId) {
			if (awbAlgoInternalLibReg[pipeId].pfn_awb_dumplog != NULL) {
				awbAlgoInternalLibReg[pipeId].pfn_awb_dumplog();
			}
		}
	} else if (strncmp(SET_AE_LOG_LOCATION_CMD, command, ae_log_location_len) == 0) {
		for (int pipeId = 0; pipeId < VI_MAX_PIPE_NUM; ++pipeId) {
			if (aeAlgoInternalLibReg[pipeId].pfn_ae_dumplog != NULL) {
				aeAlgoInternalLibReg[pipeId].pfn_ae_setdumplogpath(command + ae_log_location_len);
			}
		}
	} else if (strncmp(SET_AWB_LOG_LOCATION_CMD, command, awb_log_location_len) == 0) {
		for (int pipeId = 0; pipeId < VI_MAX_PIPE_NUM; ++pipeId) {
			if (awbAlgoInternalLibReg[pipeId].pfn_awb_dumplog != NULL) {
				awbAlgoInternalLibReg[pipeId].pfn_awb_setdumplogpath(command + awb_log_location_len);
			}
		}
	}
}
#endif // ISP_RECEIVE_IPC_CMD

CVI_S32 CVI_ISP_AELibRegInternalCallBack(VI_PIPE ViPipe, ISP_AE_DEBUG_FUNC_S *pstRegister)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstRegister);

	aeAlgoInternalLibReg[ViPipe].pfn_ae_dumplog = pstRegister->pfn_ae_dumplog;
	aeAlgoInternalLibReg[ViPipe].pfn_ae_setdumplogpath = pstRegister->pfn_ae_setdumplogpath;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_AELibUnRegInternalCallBack(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);

	aeAlgoInternalLibReg[ViPipe].pfn_ae_dumplog = NULL;
	aeAlgoInternalLibReg[ViPipe].pfn_ae_setdumplogpath = NULL;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_AWBLibRegInternalCallBack(VI_PIPE ViPipe, ISP_AWB_DEBUG_FUNC_S *pstRegister)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstRegister);

	awbAlgoInternalLibReg[ViPipe].pfn_awb_dumplog = pstRegister->pfn_awb_dumplog;
	awbAlgoInternalLibReg[ViPipe].pfn_awb_setdumplogpath = pstRegister->pfn_awb_setdumplogpath;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_AWBLibUnRegInternalCallBack(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);

	awbAlgoInternalLibReg[ViPipe].pfn_awb_dumplog = NULL;
	awbAlgoInternalLibReg[ViPipe].pfn_awb_setdumplogpath = NULL;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_Run(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_DEV(ViPipe);

	ISP_CTX_S *pstIspCtx = NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);
	CVI_S32 fd = pstIspCtx->ispDevFd;

	if (fd == -1) {
		ISP_DEBUG(LOG_EMERG, "ISP%d fd state incorrect\n", fd);
		return -EBADF;
	}

	pstIspCtx->bIspRun = CVI_TRUE;
	__subscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE0_SOF);
	__subscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE1_SOF);
	__subscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE0_EOF);
	__subscribe_event(fd, V4L2_EVENT_CVI_VIP_POST_EOF);
	__subscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE1_EOF);
	__subscribe_event(fd, V4L2_EVENT_CVI_VIP_POST1_EOF);
	//isp_snsSync_cfg_set(ViPipe, 0);
	fd_set efds, readfds;
	struct timeval tv;
	CVI_S32 r, s32Ret = 0;
	CVI_S32 initStatus[2] = { 0 };
	int vi_pipe = 0;

#ifdef ISP_RECEIVE_IPC_CMD
	int ipc_fifo_fd = 0;
	char fifo_path[128] = {'\0'};
	FILE *fFifo = NULL;

	parse_isp_fifo_path_from_environment(fifo_path, 128);

	if (initial_ipc_fifo(&ipc_fifo_fd, fifo_path) != CVI_SUCCESS)
		ISP_DEBUG(LOG_EMERG, "Enable ISP ipc fail\n");

	if (ipc_fifo_fd)
		fFifo = fdopen(ipc_fifo_fd, "r");
#endif // ISP_RECEIVE_IPC_CMD

	g_isp_thread_run = CVI_TRUE;
	/* Timeout. */
	while (g_isp_thread_run) {
		int max_fd = fd;
		FD_ZERO(&efds);
		FD_ZERO(&readfds);
		FD_SET(fd, &efds);

#ifdef ISP_RECEIVE_IPC_CMD
		if (ipc_fifo_fd > 0) {
			FD_SET(ipc_fifo_fd, &readfds);
			max_fd = MAX(max_fd, ipc_fifo_fd);
		}
#endif // ISP_RECEIVE_IPC_CMD

		tv.tv_sec = 0;
		tv.tv_usec = 500 * 1000;
		r = select(max_fd + 1, &readfds, NULL, &efds, &tv);
		if (r == -1) {
			if (errno == EINTR)
				continue;
			//errno_exit("select");
			continue;
		}
		if (r == 0) {
			//fprintf(stderr, "%s: select timeout\n", __func__);
			continue;
		}

		if (FD_ISSET(fd, &efds)) {
			struct v4l2_event ev;

			r = ioctl(fd, VIDIOC_DQEVENT, &ev);
			switch (ev.type) {
			case V4L2_EVENT_CVI_VIP_PRE0_SOF:
			case V4L2_EVENT_CVI_VIP_PRE1_SOF:
				ISP_DEBUG(LOG_DEBUG, "SOF\n");
			break;

			case V4L2_EVENT_CVI_VIP_PRE0_EOF:
			case V4L2_EVENT_CVI_VIP_PRE1_EOF:
				if (ev.type == V4L2_EVENT_CVI_VIP_PRE0_EOF)
					vi_pipe = 0;
				else if (ev.type == V4L2_EVENT_CVI_VIP_PRE1_EOF)
					vi_pipe = 1;
				if (initStatus[vi_pipe] == 0) {
					isp_stsBuf_init(vi_pipe);
					isp_mlsc0Buf_init(vi_pipe);
					initStatus[vi_pipe] = 1;
				}
				s32Ret = isp_stsBuf_get(vi_pipe, ISP_EVENT_PRE_EOF);
				if (s32Ret != CVI_SUCCESS)
					ISP_DEBUG(LOG_ERR, "Get pre stsBuf error\n");
				break;
			case V4L2_EVENT_CVI_VIP_POST_EOF:
			case V4L2_EVENT_CVI_VIP_POST1_EOF:
				if (ev.type == V4L2_EVENT_CVI_VIP_POST_EOF)
					vi_pipe = 0;
				else if (ev.type == V4L2_EVENT_CVI_VIP_POST1_EOF)
					vi_pipe = 1;
				isp_run(vi_pipe);
				break;
			}
		}

#ifdef ISP_RECEIVE_IPC_CMD
		if (ipc_fifo_fd > 0)
			if (FD_ISSET(ipc_fifo_fd, &readfds)) {
				char szCmdCommand[128] = {'\0'};

				fscanf(fFifo, "%s", szCmdCommand);
				// printf("fifo ipc command : %s\n", szCmdCommand);

				handle_fifo_command(szCmdCommand);
			}
#endif // ISP_RECEIVE_IPC_CMD
	}

	__unsubscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE0_SOF);
	__unsubscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE1_SOF);
	__unsubscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE0_EOF);
	__unsubscribe_event(fd, V4L2_EVENT_CVI_VIP_POST_EOF);
	__unsubscribe_event(fd, V4L2_EVENT_CVI_VIP_PRE1_EOF);
	__unsubscribe_event(fd, V4L2_EVENT_CVI_VIP_POST1_EOF);
	ISP_DEBUG(LOG_INFO, "End\n");

#ifdef ISP_RECEIVE_IPC_CMD
	if (fFifo)
		fclose(fFifo);

	release_ipc_fifo(fifo_path);
#endif // ISP_RECEIVE_IPC_CMD

	pstIspCtx->bIspRun = CVI_FALSE;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_RunOnce(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_DEV(ViPipe);

	// TODO: implementation
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_Exit(VI_PIPE ViPipe)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_DEV(ViPipe);

	struct vdev *d;
	ISP_CTX_S *pstIspCtx = NULL;

	d = get_dev_info(VDEV_TYPE_ISP, ViPipe);
	ISP_GET_CTX(ViPipe, pstIspCtx);
	g_isp_thread_run = CVI_FALSE;
	int count = 40;

	while (pstIspCtx->bIspRun && (--count > 0))
		usleep(50 * 1000);

	if (pstIspCtx->bIspRun)
		ISP_DEBUG(LOG_INFO, "ISP%d state(%d) incorrect\n", ViPipe, d->state);
	/* TODO . open after check complete.*/
	isp_stsBuf_exit(ViPipe);
	isp_buf_ctrl_tbl_exit(ViPipe);
	isp_buf_ctrl_algo_buf_exit(ViPipe);
	isp_iqParam_addr_exit(ViPipe);
	isp_tuningBuf_exit(ViPipe);
	isp_drcHist_buf_exit(ViPipe);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetSnsSize(VI_PIPE ViPipe, SIZE_S *size)
{
	ISP_CTX_S *pstIspCtx = NULL;
	struct v4l2_format fmt;
	CVI_S32 fd;

	ISP_GET_CTX(ViPipe, pstIspCtx);
	fd = pstIspCtx->ispDevFd;

	ISP_CHECK_DEV(ViPipe);

	if (fd == -1)
		return -EBADF;

	if (v4l2_getfmt(fd, &fmt) != 0)
		return -1;

	size->u32Width = fmt.fmt.pix_mp.width;
	size->u32Height = fmt.fmt.pix_mp.height;

	return 0;
}

CVI_S32 CVI_ISP_SetPubAttr(VI_PIPE ViPipe, const ISP_PUB_ATTR_S *pstPubAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstPubAttr);

	ISP_CTX_S *pstIspCtx = NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);

	pstIspCtx->enBayer = pstPubAttr->enBayer;
	pstIspCtx->u8SnsWDRMode = pstPubAttr->enWDRMode;
	pstIspCtx->stSysRect.s32X = pstPubAttr->stWndRect.s32X;
	pstIspCtx->stSysRect.s32Y = pstPubAttr->stWndRect.s32Y;
	pstIspCtx->stSysRect.u32Width = pstPubAttr->stWndRect.u32Width;
	pstIspCtx->stSysRect.u32Height = pstPubAttr->stWndRect.u32Height;
	pstIspCtx->stSnsImageMode.f32Fps = pstPubAttr->f32FrameRate;
	pstIspCtx->stSnsImageMode.u16Width = pstPubAttr->stSnsSize.u32Width;
	pstIspCtx->stSnsImageMode.u16Height = pstPubAttr->stSnsSize.u32Height;
	pstIspCtx->stSnsImageMode.u8SnsMode = pstPubAttr->u8SnsMode;

	if (ViPipe == 0)
		CVI_PQBIN_SetSnsWDRMode(pstPubAttr->enWDRMode);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetPubAttr(VI_PIPE ViPipe, ISP_PUB_ATTR_S *pstPubAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstPubAttr);

	ISP_CTX_S *pstIspCtx = NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);

	pstPubAttr->enBayer = pstIspCtx->enBayer;
	pstPubAttr->enWDRMode = pstIspCtx->u8SnsWDRMode;
	pstPubAttr->stSnsSize.u32Width = pstIspCtx->stSnsImageMode.u16Width;
	pstPubAttr->stSnsSize.u32Height = pstIspCtx->stSnsImageMode.u16Height;
	pstPubAttr->stWndRect.s32X = pstIspCtx->stSysRect.s32X;
	pstPubAttr->stWndRect.s32Y = pstIspCtx->stSysRect.s32Y;
	pstPubAttr->stWndRect.u32Width = pstIspCtx->stSysRect.u32Width;
	pstPubAttr->stWndRect.u32Height = pstIspCtx->stSysRect.u32Height;
	pstPubAttr->f32FrameRate = pstIspCtx->stSnsImageMode.f32Fps;
	pstPubAttr->u8SnsMode = pstIspCtx->stSnsImageMode.u8SnsMode;

	return CVI_SUCCESS;
}

/*TODO. Ctrl Param still need to be implement*/
CVI_S32 CVI_ISP_SetCtrlParam(VI_PIPE ViPipe, const ISP_CTRL_PARAM_S *pstIspCtrlParam)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstIspCtrlParam);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetCtrlParam(VI_PIPE ViPipe, ISP_CTRL_PARAM_S *pstIspCtrlParam)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstIspCtrlParam);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetFMWState(VI_PIPE ViPipe, const ISP_FMW_STATE_E enState)
{
	ISP_DEBUG(LOG_INFO, "+\n");

	ISP_CTX_S *pstIspCtx = NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);
	if (enState >= ISP_FMW_STATE_BUTT)
		return CVI_FAILURE;

	pstIspCtx->bFreezeFw = enState;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetFMWState(VI_PIPE ViPipe, ISP_FMW_STATE_E *penState)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(penState);

	ISP_CTX_S *pstIspCtx = NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);

	*penState = pstIspCtx->bFreezeFw;

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetModuleControl(VI_PIPE ViPipe, const ISP_MODULE_CTRL_U *punModCtrl)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(punModCtrl);

	// CVI_U64 bypassSetting = punModCtrl->u64Key;

	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_BNR, punModCtrl->bitBypassNR);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_DEMOSAIC, punModCtrl->bitBypassDemosaic);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_CCM, punModCtrl->bitBypassColorMatrix);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_DRC, punModCtrl->bitBypassDRC);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_FUSION, punModCtrl->bitBypassFSWDR);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_GAMMA, punModCtrl->bitBypassGamma);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_DEHAZE, punModCtrl->bitBypassDehaze);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_3DNR, punModCtrl->bitBypass3DNR);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_YNR, punModCtrl->bitBypassYnr);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_CNR, punModCtrl->bitBypassCnr);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_YEE, punModCtrl->bitBypassSharpen);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_HSV, punModCtrl->bitBypassHSV);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_3DLUT, punModCtrl->bitBypassCLUT);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_DCI, punModCtrl->bitBypassLdci);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_WBGAIN, punModCtrl->bitBypassWBGain);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_LSC_M, punModCtrl->bitBypassMeshShading);
	isp_iq_bypass_set(ViPipe, ISP_IQ_BLOCK_LSC_R, punModCtrl->bitBypassRadialShading);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetModuleControl(VI_PIPE ViPipe, ISP_MODULE_CTRL_U *punModCtrl)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(punModCtrl);

	// CVI_U64 bypassSetting = punModCtrl->u64Key;

	punModCtrl->bitBypassNR = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_BNR);
	punModCtrl->bitBypassDemosaic = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_DEMOSAIC);
	punModCtrl->bitBypassColorMatrix = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_CCM);
	punModCtrl->bitBypassDRC = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_DRC);
	punModCtrl->bitBypassFSWDR = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_FUSION);
	punModCtrl->bitBypassGamma = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_GAMMA);
	punModCtrl->bitBypassDehaze = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_DEHAZE);
	punModCtrl->bitBypass3DNR = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_3DNR);
	punModCtrl->bitBypassYnr = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_YNR);
	punModCtrl->bitBypassCnr = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_CNR);
	punModCtrl->bitBypassSharpen = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_YEE);
	punModCtrl->bitBypassHSV = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_HSV);
	punModCtrl->bitBypassCLUT = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_3DLUT);
	punModCtrl->bitBypassLdci = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_DCI);
	punModCtrl->bitBypassWBGain = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_WBGAIN);
	punModCtrl->bitBypassMeshShading = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_LSC_M);
	punModCtrl->bitBypassRadialShading = isp_iq_bypass_get(ViPipe, ISP_IQ_BLOCK_LSC_R);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetBindAttr(VI_PIPE ViPipe, const ISP_BIND_ATTR_S *pstBindAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstBindAttr);

	CVI_S32 *ret;
	CVI_S32 i;
	ISP_CTX_S *pstIspCtx = NULL;
	ALG_LIB_S bindInfo[AAA_TYPE_MAX] = { pstBindAttr->stAeLib,
		pstBindAttr->stAwbLib, pstBindAttr->stAfLib};

	ISP_GET_CTX(ViPipe, pstIspCtx);
	for (i = 0; i < AAA_TYPE_MAX; i++) {
		ret = &(pstIspCtx->activeLibIdx[i]);

		if (bindInfo[i].s32Id != -1 && bindInfo[i].acLibName != (CVI_U8 *)("\n")) {
			*ret = isp_3aLib_find(ViPipe, &(bindInfo[i]), i);
			// When can't find 3a register lib. Only show message for remind.
			if (*ret == ISP_3ALIB_FIND_FAIL) {
				ISP_DEBUG(LOG_INFO, "Find type %d Lib fail. Bind Lib NG %d\n", i, *ret);
			}
		}
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetBindAttr(VI_PIPE ViPipe, ISP_BIND_ATTR_S *pstBindAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstBindAttr);

	isp_3aLib_get(ViPipe, &(pstBindAttr->stAeLib), AAA_TYPE_AE);
	isp_3aLib_get(ViPipe, &(pstBindAttr->stAwbLib), AAA_TYPE_AWB);
	isp_3aLib_get(ViPipe, &(pstBindAttr->stAfLib), AAA_TYPE_AF);

	ISP_DEBUG(LOG_DEBUG, "pstBindAttr->stAeLib %s %d\n"
		, pstBindAttr->stAeLib.acLibName,  pstBindAttr->stAeLib.s32Id);
	ISP_DEBUG(LOG_DEBUG, "pstBindAttr->stAwbLib %s %d\n"
		, pstBindAttr->stAwbLib.acLibName,  pstBindAttr->stAwbLib.s32Id);
	ISP_DEBUG(LOG_DEBUG, "pstBindAttr->stAfLib %s %d\n"
		, pstBindAttr->stAfLib.acLibName,  pstBindAttr->stAfLib.s32Id);
	return CVI_SUCCESS;
}
#endif

static ISP_SENSOR_REGISTER_S tmpSnsRegRunc;

CVI_S32 CVI_ISP_SensorRegCallBack(VI_PIPE ViPipe, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
						ISP_SENSOR_REGISTER_S *pstRegister)
{
#if 1
	memcpy(&tmpSnsRegRunc, pstRegister, sizeof(ISP_SENSOR_REGISTER_S));
#else
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstSnsAttrInfo);

	CVI_S32 s32Ret;
	ISP_CTX_S *pstIspCtx = CVI_NULL;

//    ISP_CHECK_POINTER(pstRegister);
#if 0
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_sensor_init);
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_sensor_exit);
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_sensor_global_init);
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_set_image_mode);
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_set_wdr_mode);
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_get_isp_default);
	//ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_get_isp_black_level);
	ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_get_sns_reg_info);
	//ISP_CHECK_POINTER(pstRegister->stSnsExp.pfn_cmos_set_pixel_detect);
#endif
	ISP_GET_CTX(ViPipe, pstIspCtx);
	ISP_CHECK_POINTER(pstIspCtx);

	if (pstIspCtx->bSnsReg == CVI_TRUE)
		return CVI_FAILURE;

	if (pstRegister == CVI_NULL)
		s32Ret = sns_sensor_cb_reg(ViPipe, pstSnsAttrInfo);
	else
		s32Ret = isp_sensor_register(ViPipe, pstSnsAttrInfo, pstRegister);
	if (s32Ret != CVI_SUCCESS)
		return s32Ret;

	pstIspCtx->stBindAttr.sensorId = pstSnsAttrInfo->eSensorId;
	pstIspCtx->bSnsReg = CVI_TRUE;
#endif
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SensorUnRegCallBack(VI_PIPE ViPipe, SENSOR_ID SensorId)
{
#if 1
	(void)ViPipe;
	(void)SensorId;

	memset(&tmpSnsRegRunc, 0, sizeof(ISP_SENSOR_REGISTER_S));	
	return CVI_SUCCESS;
#else
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);

	SENSOR_ID sensorId = 0;
	CVI_S32 s32Ret;
	ISP_CTX_S *pstIspCtx = CVI_NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);

	isp_sensor_getId(ViPipe, &sensorId);

	if (pstIspCtx->stBindAttr.sensorId != sensorId) {
		ISP_DEBUG(LOG_ERR, "ViPipe %d current sensor is %d, unregister sensor is %d.\n",
			ViPipe, pstIspCtx->stBindAttr.sensorId, sensorId);
		return -EINVAL;
	}

	s32Ret = isp_sensor_unRegister(ViPipe);
	pstIspCtx->bSnsReg = CVI_FALSE;
	return s32Ret;
#endif
}

#if 0
CVI_S32 CVI_ISP_AELibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ISP_AE_REGISTER_S *pstRegister)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstRegister);
	ISP_CHECK_POINTER(pstAeLib);

	CVI_S32 ret = CVI_SUCCESS;

	ret = isp_3aLib_find(ViPipe, pstAeLib, AAA_TYPE_AE);
	if (ret != ISP_3ALIB_FIND_FAIL) {
		ISP_DEBUG(LOG_ERR, "This AE Lib already be registered\n");
		return -EINVAL;
	}

	ret = isp_3aLib_reg(ViPipe, pstAeLib, &(pstRegister->stAeExpFunc), AAA_TYPE_AE);
	if (ret != CVI_SUCCESS) {
		ISP_DEBUG(LOG_ERR, "This AE Lib register fail\n");
	}

	return ret;
}

CVI_S32 CVI_ISP_AWBLibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, ISP_AWB_REGISTER_S *pstRegister)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstRegister);
	ISP_CHECK_POINTER(pstAwbLib);

	CVI_S32 ret = CVI_SUCCESS;

	ret = isp_3aLib_find(ViPipe, pstAwbLib, AAA_TYPE_AWB);
	if (ret != ISP_3ALIB_FIND_FAIL) {
		ISP_DEBUG(LOG_ERR, "This AWB Lib already be registered\n");
		return -EINVAL;
	}

	ret = isp_3aLib_reg(ViPipe, pstAwbLib, &(pstRegister->stAwbExpFunc), AAA_TYPE_AWB);
	if (ret != CVI_SUCCESS) {
		ISP_DEBUG(LOG_ERR, "This AWB Lib register fail\n");
		isp_reg_lib_dump();
	}

	return ret;
}

CVI_S32 CVI_ISP_AELibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstAeLib);

	if (isp_3aLib_unreg(ViPipe, pstAeLib, AAA_TYPE_AE) != CVI_SUCCESS) {
		ISP_DEBUG(LOG_ERR, "This 3aLib unregister fail\n");
		isp_reg_lib_dump();
		return -EINVAL;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_AWBLibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstAwbLib);

	if (isp_3aLib_unreg(ViPipe, pstAwbLib, AAA_TYPE_AWB) != CVI_SUCCESS) {
		ISP_DEBUG(LOG_ERR, "This 3aLib unregister fail\n");
		isp_reg_lib_dump();
		return -EINVAL;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_AFLibRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAfLib, ISP_AF_REGISTER_S *pstRegister)
{
	CVI_S32 s32Ret = 0;

	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstRegister);
	ISP_CHECK_POINTER(pstAfLib);
	s32Ret = isp_3aLib_find(ViPipe, pstAfLib, AAA_TYPE_AF);
	if (s32Ret != ISP_3ALIB_FIND_FAIL) {
		ISP_DEBUG(LOG_ERR, "This AF Lib already be registered\n");
		return -EINVAL;
	}

	s32Ret = isp_3aLib_reg(ViPipe, pstAfLib, &(pstRegister->stAfExpFunc), AAA_TYPE_AF);
	if (s32Ret != 0) {
		ISP_DEBUG(LOG_ERR, "AF Lib register fail.\n");
		return CVI_FAILURE;
	}
	// isp_reg_lib_dump();
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_AFLibUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAfLib)
{
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstAfLib);

	if (isp_3aLib_unreg(ViPipe, pstAfLib, AAA_TYPE_AF) == -1) {
		ISP_DEBUG(LOG_ERR, "AF Lib unRegister fail.\n");
		return -EINVAL;
	}
	return 0;
}

CVI_S32 CVI_ISP_GetAEStatistics(VI_PIPE ViPipe, ISP_AE_STATISTICS_S *pstAeStat)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstAeStat);

	ISP_AA_STAT_INFO_S *stsInfo;
	CVI_U32 i, j, m;
	CVI_U32 col, row;
	ISP_CTX_S *pstIspCtx = CVI_NULL;
	ISP_STATISTICS_CTRL_U unStatKey;

	ISP_GET_CTX(ViPipe, pstIspCtx);
	/*TODO@CF. Need lock when get statistic data value.*/
	isp_3aInfo_get(ViPipe, &stsInfo);
	ISP_CHECK_POINTER(stsInfo);

	unStatKey = pstIspCtx->stsCfgInfo.unKey;
	/*TODO@CF. Need add sts enable check here.*/
	for (i = 0; i < ISP_CHANNEL_MAX_NUM; i++) {
		for (j = 0; j < AE_MAX_NUM; j++) {
			for (m = 0; m < MAX_HIST_BINS; m++)
				pstAeStat->au32FEHist1024Value[i][j][m] =
					stsInfo->aeStat1[j].au32HistogramMemArray[i][m];
			// Give global data only when Stat enable.
			if (unStatKey.bit1FEAeGloStat) {
				pstAeStat->au16FEGlobalAvg[i][j][0] = stsInfo->aeStat2[j].u16GlobalAvgB[i];
				pstAeStat->au16FEGlobalAvg[i][j][1] = stsInfo->aeStat2[j].u16GlobalAvgGr[i];
				pstAeStat->au16FEGlobalAvg[i][j][2] = stsInfo->aeStat2[j].u16GlobalAvgGb[i];
				pstAeStat->au16FEGlobalAvg[i][j][3] = stsInfo->aeStat2[j].u16GlobalAvgR[i];
			}
			// Give local data only when Stat enable.
			if (unStatKey.bit1FEAeLocStat) {
				for (col = 0; col < AE_ZONE_COLUMN; col++) {
					for (row = 0; row < AE_ZONE_ROW; row++) {
						pstAeStat->au16FEZoneAvg[i][j][row][col][0] =
							stsInfo->aeStat3[j].au16ZoneAvg[i][row][col][0];
						pstAeStat->au16FEZoneAvg[i][j][row][col][1] =
							stsInfo->aeStat3[j].au16ZoneAvg[i][row][col][1];
						pstAeStat->au16FEZoneAvg[i][j][row][col][2] =
							stsInfo->aeStat3[j].au16ZoneAvg[i][row][col][2];
						pstAeStat->au16FEZoneAvg[i][j][row][col][3] =
							stsInfo->aeStat3[j].au16ZoneAvg[i][row][col][3];
					}
				}
			}
		}
	}
#if 0
	for (i = 0; i < ISP_CHANNEL_MAX_NUM; i++) {
		for (j = 0; j < MAX_HIST_BINS; j++) {
			printf("pstAeStat->au32FEHist1024Value[%d][%d] %d\n", i, j,
				pstAeStat->au32FEHist1024Value[i][j]);
		}
		for (j = 0; j < AE_MAX_NUM; j++) {
			printf("pstAeStat->au16FEGlobalAvg[%d][%d][0] %d %d %d %d\n", i, j,
				pstAeStat->au16FEGlobalAvg[i][j][0], pstAeStat->au16FEGlobalAvg[i][j][1],
				pstAeStat->au16FEGlobalAvg[i][j][2], pstAeStat->au16FEGlobalAvg[i][j][3]);
			for (l = 0; l < AE_ZONE_COL; l++) {
				for (k = 0; k < AE_ZONE_ROW; k++) {
					printf("pstAeStat->au16FEZoneAvg[%d][%d][%d][0] %d %d %d %d\n", i, k, l,
						pstAeStat->au16FEZoneAvg[i][j][k][l][0],
						pstAeStat->au16FEZoneAvg[i][j][k][l][1],
						pstAeStat->au16FEZoneAvg[i][j][k][l][2],
						pstAeStat->au16FEZoneAvg[i][j][k][l][3]);
				}
			}
		}
	}
#endif
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetWBStatistics(VI_PIPE ViPipe, ISP_WB_STATISTICS_S *pstAwbStat)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstAwbStat);

	ISP_AA_STAT_INFO_S *stsInfo;
	CVI_U32 i;
	ISP_CTX_S *pstIspCtx = CVI_NULL;
	ISP_STATISTICS_CTRL_U unStatKey;

	ISP_GET_CTX(ViPipe, pstIspCtx);
	/*TODO@CF. Need lock when get statistic data value.*/
	isp_3aInfo_get(ViPipe, &stsInfo);
	ISP_CHECK_POINTER(stsInfo);

	unStatKey = pstIspCtx->stsCfgInfo.unKey;
	// HISI only has long exposure data. So only return LE now.
	// Global AWB info enable.
	if (unStatKey.bit1AwbStat1) {
		pstAwbStat->u16GlobalB = stsInfo->awbStat1[0].u16MeteringAwbAvgB;
		pstAwbStat->u16GlobalG = stsInfo->awbStat1[0].u16MeteringAwbAvgG;
		pstAwbStat->u16GlobalR = stsInfo->awbStat1[0].u16MeteringAwbAvgR;
		pstAwbStat->u16CountAll = stsInfo->awbStat1[0].u16MeteringAwbCountAll;
	}

	// Local AWB info enable.
	if (unStatKey.bit1AwbStat2) {
		for (i = 0; i < AWB_ZONE_NUM; i++) {
			pstAwbStat->au16ZoneAvgB[i] = stsInfo->awbZoneAvgB[0][i];
			pstAwbStat->au16ZoneAvgG[i] = stsInfo->awbZoneAvgG[0][i];
			pstAwbStat->au16ZoneAvgR[i] = stsInfo->awbZoneAvgR[0][i];
			pstAwbStat->au16ZoneCountAll[i] = stsInfo->awbZoneCount[0][i];
		}
	}
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetFocusStatistics(VI_PIPE ViPipe, ISP_AF_STATISTICS_S *pstAfStat)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstAfStat);

	ISP_AA_STAT_INFO_S *stsInfo;
	CVI_U32 row, col;
	CVI_U32 afWinXNum = AF_ZONE_COLUMN, afWinYNum = AF_ZONE_ROW;
	ISP_CTX_S *pstIspCtx = CVI_NULL;
	ISP_STATISTICS_CTRL_U unStatKey;

	/*TODO@CF. Need lock when get statistic data value.*/
	ISP_GET_CTX(ViPipe, pstIspCtx);
	isp_3aInfo_get(ViPipe, &stsInfo);
	ISP_CHECK_POINTER(stsInfo);

	unStatKey = pstIspCtx->stsCfgInfo.unKey;

	if (unStatKey.bit1FEAfStat) {
		for (row = 0; row < afWinYNum; row++) {
			for (col = 0; col < afWinXNum; col++) {
				pstAfStat->stFEAFStat.stZoneMetrics[row][col].u16HlCnt =
					stsInfo->afData.stZoneMetrics[row][col].u16HlCnt;
				pstAfStat->stFEAFStat.stZoneMetrics[row][col].u32v0 =
					stsInfo->afData.stZoneMetrics[row][col].u32v0;
				pstAfStat->stFEAFStat.stZoneMetrics[row][col].u64h0 =
					stsInfo->afData.stZoneMetrics[row][col].u64h0;
				pstAfStat->stFEAFStat.stZoneMetrics[row][col].u64h1 =
					stsInfo->afData.stZoneMetrics[row][col].u64h1;
			}
		}
	}
#if 0
	for (j = 2; j < 4; j++) {
		for (i = 2; i < 4; i++) {
			ISP_DEBUG(LOG_DEBUG, "pstAfStat->stFEAFStat.stZoneMetrics[%d][%d].u16HlCnt %d %d %ld %ld\n",
				i, j, pstAfStat->stFEAFStat.stZoneMetrics[i][j].u16HlCnt,
				pstAfStat->stFEAFStat.stZoneMetrics[i][j].u32v0,
				pstAfStat->stFEAFStat.stZoneMetrics[i][j].u64h0,
				pstAfStat->stFEAFStat.stZoneMetrics[i][j].u64h1);
		}
	}
#endif
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetMGStatistics(VI_PIPE ViPipe, ISP_MG_STATISTICS_S *pstMgStat)
{
	/*TODO@CF. Need check what is MGstatistics*/
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetStatisticsConfig(VI_PIPE ViPipe, const ISP_STATISTICS_CFG_S *pstStatCfg)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstStatCfg);

	ISP_CTX_S *pstIspCtx = NULL;

	ISP_GET_CTX(ViPipe, pstIspCtx);
	ISP_CHECK_VAL_RANGE(pstStatCfg->stWBCfg.u16ZoneRow, 0, AWB_ZONE_ORIG_ROW);
	ISP_CHECK_VAL_RANGE(pstStatCfg->stWBCfg.u16ZoneCol, 0, AWB_ZONE_ORIG_COLUMN);

	memcpy(&pstIspCtx->stsCfgInfo, pstStatCfg, sizeof(ISP_STATISTICS_CFG_S));

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_3AWIN);
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetStatisticsConfig(VI_PIPE ViPipe, ISP_STATISTICS_CFG_S *pstStatCfg)
{
	ISP_CTX_S *pstIspCtx = NULL;

	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstStatCfg);
	ISP_GET_CTX(ViPipe, pstIspCtx);

	memcpy(pstStatCfg, &pstIspCtx->stsCfgInfo, sizeof(ISP_STATISTICS_CFG_S));
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetModParam(const ISP_MOD_PARAM_S *pstModParam)
{
	ISP_DEBUG(LOG_INFO, "Only support bottom-half.!\n");
	ISP_CHECK_POINTER(pstModParam);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetModParam(ISP_MOD_PARAM_S *pstModParam)
{
	ISP_DEBUG(LOG_DEBUG, "always return bottom-half!\n");
	ISP_CHECK_POINTER(pstModParam);

	// BotHalf = 1 means botoom half.
	pstModParam->u32IntBotHalf = 1;

	return CVI_SUCCESS;
}

// For Tool get some parameter use at calculate IQ.
CVI_S32 CVI_ISP_QueryInnerStateInfo(VI_PIPE ViPipe, ISP_INNER_STATE_INFO_S *pstInnerStateInfo)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstInnerStateInfo);

	isp_internal_param_get(ViPipe, pstInnerStateInfo);

	for (CVI_U32 i = 0; i < pstInnerStateInfo->wdrHistBinNum; i++) {
		pstInnerStateInfo->wdrHistogramBefore[i] = pstInnerStateInfo->pWdrHistogram[i];
		pstInnerStateInfo->wdrHistogramAfter[i] = pstInnerStateInfo->pWdrHistogramResult[i];
	}

	return CVI_SUCCESS;
}

// TODO@Kidd should add ISP API to calculate settings of radio shadding
// and remove CVI_ISP_SetRadioShadingAttr() / CVI_ISP_GetRadioShadingAttr()
CVI_S32 CVI_ISP_SetMeshShadingAttr(VI_PIPE ViPipe, const ISP_MeshShading_ATTR_S *pstMeshShadingAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstMeshShadingAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_MESHSHADING_LSC_ENABLE, pstMeshShadingAttr->Enable);
	isp_param_set(ViPipe, ISP_MESHSHADING_ENOPTYPE, pstMeshShadingAttr->enOpType);

	// manual attributes
	isp_param_set(ViPipe, ISP_MESHSHADING_MANUAL_LSC_STRENGTH, pstMeshShadingAttr->stManual.MeshStr);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		isp_param_set(ViPipe, ISP_MESHSHADING_AUTO_LSC_STRENGTH + i, pstMeshShadingAttr->stAuto.MeshStr[i]);

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_LSC_M);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetMeshShadingAttr(VI_PIPE ViPipe, ISP_MeshShading_ATTR_S *pstMeshShadingAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstMeshShadingAttr);

	CLEAR(*pstMeshShadingAttr);
	// non-auto parameters
	isp_param_get(ViPipe, ISP_MESHSHADING_LSC_ENABLE, (void *)(uintptr_t)&(pstMeshShadingAttr->Enable));
	isp_param_get(ViPipe, ISP_MESHSHADING_ENOPTYPE, (void *)(uintptr_t)&(pstMeshShadingAttr->enOpType));

	// manual parameters
	isp_param_get(ViPipe, ISP_MESHSHADING_MANUAL_LSC_STRENGTH,
		(void *)(uintptr_t)&(pstMeshShadingAttr->stManual.MeshStr));

	// auto parameters
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_MESHSHADING_AUTO_LSC_STRENGTH + i,
				(void *)(uintptr_t)&(pstMeshShadingAttr->stAuto.MeshStr[i]));
	}
	// TODO should add fixed parameters?
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetMeshShadingGainLutAttr(VI_PIPE ViPipe
	, const ISP_MeshShadingGainLut_ATTR_S *pstMeshShadingGainLutAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstMeshShadingGainLutAttr);

	memcpy(&g_param[ViPipe].mlscLUT, pstMeshShadingGainLutAttr, sizeof(*pstMeshShadingGainLutAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_LSC_M);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetMeshShadingGainLutAttr(VI_PIPE ViPipe, ISP_MeshShadingGainLut_ATTR_S *pstMeshShadingGainLutAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstMeshShadingGainLutAttr);

	memcpy(pstMeshShadingGainLutAttr, &g_param[ViPipe].mlscLUT, sizeof(*pstMeshShadingGainLutAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetRadialShadingAttr(VI_PIPE ViPipe, const ISP_RadialShading_ATTR_S *pstRadialShadingAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstRadialShadingAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_MESHSHADING_LSCR_ENABLE, pstRadialShadingAttr->Enable);
	isp_param_set(ViPipe, ISP_MESHSHADING_LSCR_CENTERX, pstRadialShadingAttr->CenterX);
	isp_param_set(ViPipe, ISP_MESHSHADING_LSCR_CENTERY, pstRadialShadingAttr->CenterY);

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_LSC_R);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetRadialShadingAttr(VI_PIPE ViPipe, ISP_RadialShading_ATTR_S *pstRadialShadingAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstRadialShadingAttr);

	// non-auto parameters
	isp_param_get(ViPipe, ISP_MESHSHADING_LSCR_ENABLE, (void *)(uintptr_t)&(pstRadialShadingAttr->Enable));
	isp_param_get(ViPipe, ISP_MESHSHADING_LSCR_CENTERX, (void *)(uintptr_t)&(pstRadialShadingAttr->CenterX));
	isp_param_get(ViPipe, ISP_MESHSHADING_LSCR_CENTERY, (void *)(uintptr_t)&(pstRadialShadingAttr->CenterY));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetRadialShadingGainLutAttr(VI_PIPE ViPipe,
			const ISP_RadialShadingGainLut_ATTR_S *pstRadialShadingGainLutAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstRadialShadingGainLutAttr);

	// non-auto attributes
	// TODO@Kidd interplace by color temperature
	for (CVI_U32 i = 0; i < ISP_RLSC_WINDOW_SIZE; i++) {
		isp_param_set(ViPipe, ISP_MESHSHADING_LOW_CT_LSCR_GAIN_LUT_0 + i,
				pstRadialShadingGainLutAttr->RLscGainLut[ISP_RLSC_COLOR_TEMPERATURE_LOW][i]);
		isp_param_set(ViPipe, ISP_MESHSHADING_HIGH_CT_LSCR_GAIN_LUT_0 + i,
				pstRadialShadingGainLutAttr->RLscGainLut[ISP_RLSC_COLOR_TEMPERATURE_HIGH][i]);
	}
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_LSC_R);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetRadialShadingGainLutAttr(VI_PIPE ViPipe,
			ISP_RadialShadingGainLut_ATTR_S *pstRadialShadingGainLutAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstRadialShadingGainLutAttr);

	// non-auto parameters
	// TODO interplace by color temperature
	for (CVI_S32 i = 0; i < ISP_RLSC_WINDOW_SIZE; i++) {
		isp_param_get(ViPipe, ISP_MESHSHADING_LOW_CT_LSCR_GAIN_LUT_0 + i,
		(void *)(uintptr_t)&(pstRadialShadingGainLutAttr->RLscGainLut[ISP_RLSC_COLOR_TEMPERATURE_LOW][i]));
		isp_param_get(ViPipe, ISP_MESHSHADING_HIGH_CT_LSCR_GAIN_LUT_0 + i,
		(void *)(uintptr_t)&(pstRadialShadingGainLutAttr->RLscGainLut[ISP_RLSC_COLOR_TEMPERATURE_HIGH][i]));
	}

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintDemosaicAttr(const ISP_Demosaic_ATTR_S *pstDemosaicAttr)
{
	// non-auto attributes
	PRINT_ATTR(pstDemosaicAttr->Enable, reg_cfa_enable, 0, 1);
	PRINT_ATTR(pstDemosaicAttr->TuningMode, reg_cfa_out_sel, 0, 1);

	// manual attributes
	PRINT_ATTR(pstDemosaicAttr->stManual.CoarseEdgeThr, reg_cfa_edgee_thd, 0, 4095);
	PRINT_ATTR(pstDemosaicAttr->stManual.CoarseStr, reg_cfa_edge_tol, 0, 4095);
	PRINT_ATTR(pstDemosaicAttr->stManual.FineEdgeThr, reg_cfa_sige_thd, 0, 4095);
	PRINT_ATTR(pstDemosaicAttr->stManual.FineStr, (reg_cfa_gsig_tol, reg_cfa_rbsig_tol), 0, 4095);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_S32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstDemosaicAttr->stAuto.CoarseEdgeThr[i], reg_cfa_edgee_thd, 0, 4095);
		PRINT_ATTR(pstDemosaicAttr->stAuto.CoarseStr[i], reg_cfa_edge_tol, 0, 4095);
		PRINT_ATTR(pstDemosaicAttr->stAuto.FineEdgeThr[i], reg_cfa_sige_thd, 0, 4095);
		PRINT_ATTR(pstDemosaicAttr->stAuto.FineStr[i], (reg_cfa_gsig_tol, reg_cfa_rbsig_tol), 0, 4095);
	}
	#endif
}

CVI_S32 CVI_ISP_SetDemosaicAttr(VI_PIPE ViPipe, const ISP_Demosaic_ATTR_S *pstDemosaicAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDemosaicAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_DEMOSAIC_ENABLE, pstDemosaicAttr->Enable);
	isp_param_set(ViPipe, ISP_DEMOSAIC_ENOPTYPE, pstDemosaicAttr->enOpType);
	isp_param_set(ViPipe, ISP_DEMOSAIC_OUTSEL, pstDemosaicAttr->TuningMode);

	// manual attributes
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_COARSEEDGETHR, pstDemosaicAttr->stManual.CoarseEdgeThr);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_COARSESTR, pstDemosaicAttr->stManual.CoarseStr);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_FINEEDGETHR, pstDemosaicAttr->stManual.FineEdgeThr);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_FINESTR, pstDemosaicAttr->stManual.FineStr);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_COARSEEDGETHR + i, pstDemosaicAttr->stAuto.CoarseEdgeThr[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_COARSESTR + i, pstDemosaicAttr->stAuto.CoarseStr[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_FINEEDGETHR + i, pstDemosaicAttr->stAuto.FineEdgeThr[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_FINESTR + i, pstDemosaicAttr->stAuto.FineStr[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicAttr(pstDemosaicAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DEMOSAIC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDemosaicAttr(VI_PIPE ViPipe, ISP_Demosaic_ATTR_S *pstDemosaicAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDemosaicAttr);

	CLEAR(*pstDemosaicAttr);
	// non-auto parameters
	isp_param_get(ViPipe, ISP_DEMOSAIC_ENABLE, (void *)(uintptr_t)&(pstDemosaicAttr->Enable));
	isp_param_get(ViPipe, ISP_DEMOSAIC_ENOPTYPE, (void *)(uintptr_t)&(pstDemosaicAttr->enOpType));
	isp_param_get(ViPipe, ISP_DEMOSAIC_OUTSEL, (void *)(uintptr_t)&(pstDemosaicAttr->TuningMode));

	// manual parameters
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_COARSEEDGETHR,
		(void *)(uintptr_t)&(pstDemosaicAttr->stManual.CoarseEdgeThr));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_COARSESTR, (void *)(uintptr_t)&(pstDemosaicAttr->stManual.CoarseStr));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_FINEEDGETHR,
		(void *)(uintptr_t)&(pstDemosaicAttr->stManual.FineEdgeThr));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_FINESTR, (void *)(uintptr_t)&(pstDemosaicAttr->stManual.FineStr));

	// auto parameters
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_COARSEEDGETHR + i
			, (void *)(uintptr_t)&(pstDemosaicAttr->stAuto.CoarseEdgeThr[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_COARSESTR + i
			, (void *)(uintptr_t)&(pstDemosaicAttr->stAuto.CoarseStr[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_FINEEDGETHR + i
			, (void *)(uintptr_t)&(pstDemosaicAttr->stAuto.FineEdgeThr[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_FINESTR + i
			, (void *)(uintptr_t)&(pstDemosaicAttr->stAuto.FineStr[i]));
	}

	// TODO should add fixed parameters?
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicAttr(pstDemosaicAttr);

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintDemosaicDemoireAttr(const ISP_DemosaicDemoire_ATTR_S *pstDemosaicDemoireAttr)
{
	// non-auto attributes

	// manual attributes
	PRINT_ATTR(pstDemosaicDemoireAttr->stManual.DetailSmoothEnable, reg_cfa_moire_enable, 0, 1);
	PRINT_ATTR(pstDemosaicDemoireAttr->stManual.DetailSmoothStr, reg_cfa_moire_strth, 0, 255);
	PRINT_ATTR(pstDemosaicDemoireAttr->stManual.EdgeWgtStr, reg_cfa_moire_wght_gain, 0, 255);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_U8 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstDemosaicDemoireAttr->stAuto.DetailSmoothEnable[i], reg_cfa_moire_enable, 0, 1);
		PRINT_ATTR(pstDemosaicDemoireAttr->stAuto.DetailSmoothStr[i], reg_cfa_moire_strth, 0, 255);
		PRINT_ATTR(pstDemosaicDemoireAttr->stAuto.EdgeWgtStr[i], reg_cfa_moire_wght_gain, 0, 255);
	}
	#endif
}

CVI_S32 CVI_ISP_SetDemosaicDemoireAttr(VI_PIPE ViPipe, const ISP_DemosaicDemoire_ATTR_S *pstDemosaicDemoireAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDemosaicDemoireAttr);

	// non-auto attributes

	// manual attributes
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_DEMOIREENABLE, pstDemosaicDemoireAttr->stManual.DetailSmoothEnable);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_DEMOIRESTR, pstDemosaicDemoireAttr->stManual.DetailSmoothStr);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_EDGEWGTSTR, pstDemosaicDemoireAttr->stManual.EdgeWgtStr);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_DEMOIREENABLE + i
			, pstDemosaicDemoireAttr->stAuto.DetailSmoothEnable[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_DEMOIRESTR + i
			, pstDemosaicDemoireAttr->stAuto.DetailSmoothStr[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_EDGEWGTSTR + i
			, pstDemosaicDemoireAttr->stAuto.EdgeWgtStr[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicDemoireAttr(pstDemosaicDemoireAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DEMOSAIC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDemosaicDemoireAttr(VI_PIPE ViPipe, ISP_DemosaicDemoire_ATTR_S *pstDemosaicDemoireAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDemosaicDemoireAttr);

	// non-auto parameters

	// manual parameters
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_DEMOIREENABLE
		, (void *)(uintptr_t)&(pstDemosaicDemoireAttr->stManual.DetailSmoothEnable));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_DEMOIRESTR
		, (void *)(uintptr_t)&(pstDemosaicDemoireAttr->stManual.DetailSmoothStr));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_EDGEWGTSTR
		, (void *)(uintptr_t)&(pstDemosaicDemoireAttr->stManual.EdgeWgtStr));

	// auto parameters
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_DEMOIREENABLE + i
			, (void *)(uintptr_t)&(pstDemosaicDemoireAttr->stAuto.DetailSmoothEnable[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_DEMOIRESTR + i
			, (void *)(uintptr_t)&(pstDemosaicDemoireAttr->stAuto.DetailSmoothStr[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_EDGEWGTSTR + i
			, (void *)(uintptr_t)&(pstDemosaicDemoireAttr->stAuto.EdgeWgtStr[i]));
	}

	// TODO should add fixed parameters?
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicDemoireAttr(pstDemosaicDemoireAttr);

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintDemosaicFilterAttr(const ISP_DemosaicFilter_ATTR_S *pstDemosaicFilterAttr)
{
	// non-auto attributes

	// manual attributes
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.DetailWgtThr
		, (reg_cfa_moire_diffthd_min, reg_cfa_moire_sadthd_min), 0, 4095);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.DetailWgtSlope
		, (reg_cfa_moire_diffthd_slope, reg_cfa_moire_sadthd_slope), 0, 255);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.DetailWgtMin
		, (reg_cfa_moire_diffw_low, reg_cfa_moire_sadw_low), 0, 255);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.DetailWgtMax
		, (reg_cfa_moire_diffw_high, reg_cfa_moire_sadw_high), 0, 255);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.LumaWgtThr, reg_cfa_moire_lumathd_min, 0, 4095);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.LumaWgtSlope, reg_cfa_moire_lumathd_slope, 0, 255);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.LumaWgtMin, reg_cfa_moire_lumaw_low, 0, 255);
	PRINT_ATTR(pstDemosaicFilterAttr->stManual.LumaWgtMax, reg_cfa_moire_lumaw_high, 0, 255);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.DetailWgtThr[i]
			, (reg_cfa_moire_diffthd_min, reg_cfa_moire_sadthd_min), 0, 4095);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.DetailWgtSlope[i]
			, (reg_cfa_moire_diffthd_slope, reg_cfa_moire_sadthd_slope), 0, 255);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.DetailWgtMin[i]
			, (reg_cfa_moire_diffw_low, reg_cfa_moire_sadw_low), 0, 255);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.DetailWgtMax[i]
			, (reg_cfa_moire_diffw_high, reg_cfa_moire_sadw_high), 0, 255);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.LumaWgtThr[i], reg_cfa_moire_lumathd_min, 0, 4095);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.LumaWgtSlope[i], reg_cfa_moire_lumathd_slope, 0, 255);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.LumaWgtMin[i], reg_cfa_moire_lumaw_low, 0, 255);
		PRINT_ATTR(pstDemosaicFilterAttr->stAuto.LumaWgtMax[i], reg_cfa_moire_lumaw_high, 0, 255);
	}
	#endif
}

CVI_S32 CVI_ISP_SetDemosaicFilterAttr(VI_PIPE ViPipe, const ISP_DemosaicFilter_ATTR_S *pstDemosaicFilterAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDemosaicFilterAttr);

	// non-auto attributes

	// manual attributes
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFTHR, pstDemosaicFilterAttr->stManual.DetailWgtThr);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFSLOPE, pstDemosaicFilterAttr->stManual.DetailWgtSlope);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFLOWOFFSET, pstDemosaicFilterAttr->stManual.DetailWgtMin);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFHIGHOFFSET, pstDemosaicFilterAttr->stManual.DetailWgtMax);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_LUMATHRMIN, pstDemosaicFilterAttr->stManual.LumaWgtThr);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_LUMATHRSLOPE, pstDemosaicFilterAttr->stManual.LumaWgtSlope);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_LUMAWLOW, pstDemosaicFilterAttr->stManual.LumaWgtMin);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_LUMAWHIGH, pstDemosaicFilterAttr->stManual.LumaWgtMax);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_DIFFTHR + i, pstDemosaicFilterAttr->stAuto.DetailWgtThr[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_DIFFSLOPE + i, pstDemosaicFilterAttr->stAuto.DetailWgtSlope[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_DIFFLOWOFFSET + i
			, pstDemosaicFilterAttr->stAuto.DetailWgtMin[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_DIFFHIGHOFFSET + i
			, pstDemosaicFilterAttr->stAuto.DetailWgtMax[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_LUMATHRMIN + i
			, pstDemosaicFilterAttr->stAuto.LumaWgtThr[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_LUMATHRSLOPE + i
			, pstDemosaicFilterAttr->stAuto.LumaWgtSlope[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_LUMAWLOW + i, pstDemosaicFilterAttr->stAuto.LumaWgtMin[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_LUMAWHIGH + i, pstDemosaicFilterAttr->stAuto.LumaWgtMax[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicFilterAttr(pstDemosaicFilterAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DEMOSAIC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDemosaicFilterAttr(VI_PIPE ViPipe, ISP_DemosaicFilter_ATTR_S *pstDemosaicFilterAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDemosaicFilterAttr);

	// non-auto parameters

	// manual parameters
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFTHR,
		(void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.DetailWgtThr));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFSLOPE,
		(void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.DetailWgtSlope));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFLOWOFFSET,
		      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.DetailWgtMin));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_DIFFHIGHOFFSET,
		      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.DetailWgtMax));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_LUMATHRMIN,
		(void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.LumaWgtThr));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_LUMATHRSLOPE,
		      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.LumaWgtSlope));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_LUMAWLOW,
		(void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.LumaWgtMin));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_LUMAWHIGH,
		(void *)(uintptr_t)&(pstDemosaicFilterAttr->stManual.LumaWgtMax));

	// auto parameters
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_DIFFTHR + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.DetailWgtThr[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_DIFFSLOPE + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.DetailWgtSlope[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_DIFFLOWOFFSET + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.DetailWgtMin[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_DIFFHIGHOFFSET + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.DetailWgtMax[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_LUMATHRMIN + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.LumaWgtThr[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_LUMATHRSLOPE + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.LumaWgtSlope[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_LUMAWLOW + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.LumaWgtMin[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_LUMAWHIGH + i,
			      (void *)(uintptr_t)&(pstDemosaicFilterAttr->stAuto.LumaWgtMax[i]));
	}

	// TODO should add fixed parameters?
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicFilterAttr(pstDemosaicFilterAttr);

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintDemosaicEEAttr(const ISP_DemosaicEE_ATTR_S *pstDemosaicEEAttr)
{
	CVI_S32 j = 0;

	// non-auto attributes

	// manual attributes
	PRINT_ATTR(pstDemosaicEEAttr->stManual.EdgeEnhanceEnable, reg_rgbee_enable, 0, 1);
	PRINT_ATTR(pstDemosaicEEAttr->stManual.OverShtGain, reg_rgbee_osgain, 0, 1023);
	PRINT_ATTR(pstDemosaicEEAttr->stManual.UnderShtGain, reg_rgbee_usgain, 0, 1023);
	PRINT_ATTR(pstDemosaicEEAttr->stManual.NoiseSuppressStr
		, (reg_rgbee_np_lut00 - reg_rgbee_np_lut32), 0, 255);
	for (j = 0; j < 33; j++)
		PRINT_ATTR(pstDemosaicEEAttr->stManual.HFGainTable[j]
			, (reg_rgbee_ac_lut00 - reg_rgbee_ac_lut32), 0, 255);
	for (j = 0; j < 33; j++)
		PRINT_ATTR(pstDemosaicEEAttr->stManual.GainTable[j]
			, (reg_rgbee_edge_lut00 - reg_rgbee_edge_lut32), 0, 255);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstDemosaicEEAttr->stAuto.EdgeEnhanceEnable[i], reg_rgbee_enable, 0, 1);
		PRINT_ATTR(pstDemosaicEEAttr->stAuto.OverShtGain[i], reg_rgbee_osgain, 0, 1023);
		PRINT_ATTR(pstDemosaicEEAttr->stAuto.UnderShtGain[i], reg_rgbee_usgain, 0, 1023);
		PRINT_ATTR(pstDemosaicEEAttr->stAuto.NoiseSuppressStr[i]
			, (reg_rgbee_np_lut00 - reg_rgbee_np_lut32), 0, 255);
		for (j = 0; j < 33; j++)
			PRINT_ATTR(pstDemosaicEEAttr->stAuto.HFGainTable[j][i]
				, (reg_rgbee_ac_lut00 - reg_rgbee_ac_lut32), 0, 255);
		for (j = 0; j < 33; j++)
			PRINT_ATTR(pstDemosaicEEAttr->stAuto.GainTable[j][i]
				, (reg_rgbee_edge_lut00 - reg_rgbee_edge_lut32), 0, 255);
	}
	#endif
}

CVI_S32 CVI_ISP_SetDemosaicEEAttr(VI_PIPE ViPipe, const ISP_DemosaicEE_ATTR_S *pstDemosaicEEAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDemosaicEEAttr);

	CVI_S32 i = 0;
	CVI_S32 j = 0;
	CVI_S32 offset = 0;

	// non-auto attributes

	// manual attributes
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_EDGEENHANCEENABLE, pstDemosaicEEAttr->stManual.EdgeEnhanceEnable);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_OVERSHTGAIN, pstDemosaicEEAttr->stManual.OverShtGain);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_UNDERSHTGAIN, pstDemosaicEEAttr->stManual.UnderShtGain);
	isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_NOISESUPPRESSSTR
		, pstDemosaicEEAttr->stManual.NoiseSuppressStr);
	for (j = 0; j < 33; j++)
		isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_HFGAINTABLE_0 + j,
			      pstDemosaicEEAttr->stManual.HFGainTable[j]);
	for (j = 0; j < 33; j++)
		isp_param_set(ViPipe, ISP_DEMOSAIC_MANUAL_GAINTABLE_0 + j, pstDemosaicEEAttr->stManual.GainTable[j]);

	// auto attributes
	for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_EDGEENHANCEENABLE + i,
			      pstDemosaicEEAttr->stAuto.EdgeEnhanceEnable[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_OVERSHTGAIN + i, pstDemosaicEEAttr->stAuto.OverShtGain[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_UNDERSHTGAIN + i, pstDemosaicEEAttr->stAuto.UnderShtGain[i]);
		isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_NOISESUPPRESSSTR + i,
			      pstDemosaicEEAttr->stAuto.NoiseSuppressStr[i]);
		for (j = 0; j < 33; j++) {
			offset = ISP_DEMOSAIC_AUTO_HFGAINTABLE_1 - ISP_DEMOSAIC_AUTO_HFGAINTABLE_0;
			isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_HFGAINTABLE_0 + offset * j + i,
				      pstDemosaicEEAttr->stAuto.HFGainTable[j][i]);
		}
		for (j = 0; j < 33; j++) {
			offset = ISP_DEMOSAIC_AUTO_GAINTABLE_1 - ISP_DEMOSAIC_AUTO_GAINTABLE_0;
			isp_param_set(ViPipe, ISP_DEMOSAIC_AUTO_GAINTABLE_0 + offset * j + i,
				      pstDemosaicEEAttr->stAuto.GainTable[j][i]);
		}
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicEEAttr(pstDemosaicEEAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DEMOSAIC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDemosaicEEAttr(VI_PIPE ViPipe, ISP_DemosaicEE_ATTR_S *pstDemosaicEEAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDemosaicEEAttr);

	CVI_S32 i = 0;
	CVI_S32 j = 0;
	CVI_S32 offset = 0;

	// non-auto parameters

	// manual parameters
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_EDGEENHANCEENABLE,
		      (void *)(uintptr_t)&(pstDemosaicEEAttr->stManual.EdgeEnhanceEnable));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_OVERSHTGAIN,
		(void *)(uintptr_t)&(pstDemosaicEEAttr->stManual.OverShtGain));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_UNDERSHTGAIN,
		(void *)(uintptr_t)&(pstDemosaicEEAttr->stManual.UnderShtGain));
	isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_NOISESUPPRESSSTR,
		      (void *)(uintptr_t)&(pstDemosaicEEAttr->stManual.NoiseSuppressStr));
	for (j = 0; j < 33; j++) {
		isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_HFGAINTABLE_0 + j,
			      (void *)(uintptr_t)&(pstDemosaicEEAttr->stManual.HFGainTable[j]));
	}
	for (j = 0; j < 33; j++) {
		isp_param_get(ViPipe, ISP_DEMOSAIC_MANUAL_GAINTABLE_0 + j,
			      (void *)(uintptr_t)&(pstDemosaicEEAttr->stManual.GainTable[j]));
	}

	// auto parameters
	for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_EDGEENHANCEENABLE + i
			, (void *)(uintptr_t)&(pstDemosaicEEAttr->stAuto.EdgeEnhanceEnable[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_OVERSHTGAIN + i
			, (void *)(uintptr_t)&(pstDemosaicEEAttr->stAuto.OverShtGain[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_UNDERSHTGAIN + i
			, (void *)(uintptr_t)&(pstDemosaicEEAttr->stAuto.UnderShtGain[i]));
		isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_NOISESUPPRESSSTR + i
			, (void *)(uintptr_t)&(pstDemosaicEEAttr->stAuto.NoiseSuppressStr[i]));
		for (j = 0; j < 33; j++) {
			offset = ISP_DEMOSAIC_AUTO_HFGAINTABLE_1 - ISP_DEMOSAIC_AUTO_HFGAINTABLE_0;
			isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_HFGAINTABLE_0 + offset * j + i
				, (void *)(uintptr_t)&(pstDemosaicEEAttr->stAuto.HFGainTable[j][i]));
		}
		for (j = 0; j < 33; j++) {
			offset = ISP_DEMOSAIC_AUTO_GAINTABLE_1 - ISP_DEMOSAIC_AUTO_GAINTABLE_0;
			isp_param_get(ViPipe, ISP_DEMOSAIC_AUTO_GAINTABLE_0 + offset * j + i
				, (void *)(uintptr_t)&(pstDemosaicEEAttr->stAuto.GainTable[j][i]));
		}
	}

	// TODO should add fixed parameters?
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDemosaicEEAttr(pstDemosaicEEAttr);

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintNRAttr(const ISP_NR_ATTR_S *pstNRAttr)
{
	// non-auto attributes
	PRINT_ATTR(pstNRAttr->Enable, reg_bnr_enable, 0, 1);
	PRINT_ATTR(pstNRAttr->enOpType, reg_na, 0, 1);

	// manual attributes
	PRINT_ATTR(pstNRAttr->stManual.WindowType
		, (reg_bnr_weight_intra_0, reg_bnr_weight_intra_1, reg_bnr_weight_intra_2
		, reg_bnr_weight_norm_intra1, reg_bnr_weight_norm_intra2), 0, 11);
	PRINT_ATTR(pstNRAttr->stManual.DetailSmoothMode, reg_bnr_flag_neighbor_max_weight, 0, 1);
	PRINT_ATTR(pstNRAttr->stManual.NoiseSuppressStr, reg_bnr_ns_gain, 0, 255);
	PRINT_ATTR(pstNRAttr->stManual.FilterType, reg_bnr_weight_lut_h, 0, 255);
	PRINT_ATTR(pstNRAttr->stManual.NoiseSuppressStrMode, reg_bnr_strength_mode, 0, 255);


	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstNRAttr->stAuto.WindowType[i]
			, (reg_bnr_weight_intra_0, reg_bnr_weight_intra_1, reg_bnr_weight_intra_2
			, reg_bnr_weight_norm_intra1, reg_bnr_weight_norm_intra2), 0, 11);
		PRINT_ATTR(pstNRAttr->stAuto.DetailSmoothMode[i], reg_bnr_flag_neighbor_max_weight, 0, 1);
		PRINT_ATTR(pstNRAttr->stAuto.NoiseSuppressStr[i], reg_bnr_ns_gain, 0, 255);
		PRINT_ATTR(pstNRAttr->stAuto.FilterType[i], reg_bnr_weight_lut_h, 0, 255);
		PRINT_ATTR(pstNRAttr->stAuto.NoiseSuppressStrMode[i], reg_bnr_strength_mode, 0, 255);
	}
	#endif
}

CVI_S32 CVI_ISP_SetNRAttr(VI_PIPE ViPipe, const ISP_NR_ATTR_S *pstNRAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstNRAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_NR_ENABLE, pstNRAttr->Enable);
	isp_param_set(ViPipe, ISP_NR_ENOPTYPE, pstNRAttr->enOpType);

	// manual attributes
	isp_param_set(ViPipe, ISP_NR_MANUAL_WINDOWTYPE, pstNRAttr->stManual.WindowType);
	isp_param_set(ViPipe, ISP_NR_MANUAL_DETAILSMOOTHMODE, pstNRAttr->stManual.DetailSmoothMode);
	isp_param_set(ViPipe, ISP_NR_MANUAL_NOISESUPPRESSSTR, pstNRAttr->stManual.NoiseSuppressStr);
	isp_param_set(ViPipe, ISP_NR_MANUAL_FILTERTYPE, pstNRAttr->stManual.FilterType);
	isp_param_set(ViPipe, ISP_NR_MANUAL_NOISESUPPRESSSTRMODE, pstNRAttr->stManual.NoiseSuppressStrMode);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_NR_AUTO_WINDOWTYPE + i, pstNRAttr->stAuto.WindowType[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_DETAILSMOOTHMODE + i, pstNRAttr->stAuto.DetailSmoothMode[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_NOISESUPPRESSSTR + i, pstNRAttr->stAuto.NoiseSuppressStr[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_FILTERTYPE + i, pstNRAttr->stAuto.FilterType[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_NOISESUPPRESSSTRMODE + i, pstNRAttr->stAuto.NoiseSuppressStrMode[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintNRAttr(pstNRAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_BNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetNRAttr(VI_PIPE ViPipe, ISP_NR_ATTR_S *pstNRAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstNRAttr);

	CLEAR(*pstNRAttr);
	// non-auto attributes
	isp_param_get(ViPipe, ISP_NR_ENABLE, (void *)(uintptr_t)&(pstNRAttr->Enable));
	isp_param_get(ViPipe, ISP_NR_ENOPTYPE, (void *)(uintptr_t)&(pstNRAttr->enOpType));

	// manual attributes
	isp_param_get(ViPipe, ISP_NR_MANUAL_WINDOWTYPE, (void *)(uintptr_t)&(pstNRAttr->stManual.WindowType));
	isp_param_get(ViPipe, ISP_NR_MANUAL_DETAILSMOOTHMODE,
		(void *)(uintptr_t)&(pstNRAttr->stManual.DetailSmoothMode));
	isp_param_get(ViPipe, ISP_NR_MANUAL_NOISESUPPRESSSTR,
		(void *)(uintptr_t)&(pstNRAttr->stManual.NoiseSuppressStr));
	isp_param_get(ViPipe, ISP_NR_MANUAL_FILTERTYPE, (void *)(uintptr_t)&(pstNRAttr->stManual.FilterType));
	isp_param_get(ViPipe, ISP_NR_MANUAL_NOISESUPPRESSSTRMODE,
		(void *)(uintptr_t)&(pstNRAttr->stManual.NoiseSuppressStrMode));

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_NR_AUTO_WINDOWTYPE + i,
			(void *)(uintptr_t)&(pstNRAttr->stAuto.WindowType[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_DETAILSMOOTHMODE + i,
			      (void *)(uintptr_t)&(pstNRAttr->stAuto.DetailSmoothMode[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_NOISESUPPRESSSTR + i,
			      (void *)(uintptr_t)&(pstNRAttr->stAuto.NoiseSuppressStr[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_FILTERTYPE + i,
			(void *)(uintptr_t)&(pstNRAttr->stAuto.FilterType[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_NOISESUPPRESSSTRMODE + i,
			      (void *)(uintptr_t)&(pstNRAttr->stAuto.NoiseSuppressStrMode[i]));
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintNRAttr(pstNRAttr);

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintNRFilterAttr(const ISP_NRFilter_ATTR_S *pstNRFilterAttr)
{
	// non-auto attributes
	PRINT_ATTR(pstNRFilterAttr->TuningMode, reg_bnr_out_sel, 0, 15);

	// manual attributes
	PRINT_ATTR(pstNRFilterAttr->stManual.NrLscRatio, reg_bnr_lsc_ratio, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[0], reg_bnr_intensity_sel_0, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[1], reg_bnr_intensity_sel_1, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[2], reg_bnr_intensity_sel_2, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[3], reg_bnr_intensity_sel_3, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[4], reg_bnr_intensity_sel_4, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[5], reg_bnr_intensity_sel_5, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[6], reg_bnr_intensity_sel_6, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.LumaStr[7], reg_bnr_intensity_sel_7, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.VarThr, reg_bnr_var_th, 0, 1023);
	PRINT_ATTR(pstNRFilterAttr->stManual.CoringWgtLF, reg_bnr_res_ratio_k_smooth, 0, 256);
	PRINT_ATTR(pstNRFilterAttr->stManual.CoringWgtHF, reg_bnr_res_ratio_k_texture, 0, 256);
	PRINT_ATTR(pstNRFilterAttr->stManual.NonDirFiltStr, reg_bnr_weight_smooth, 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.VhDirFiltStr, (reg_bnr_weight_v, reg_bnr_weight_h), 0, 31);
	PRINT_ATTR(pstNRFilterAttr->stManual.AaDirFiltStr
		, (reg_bnr_weight_d35, reg_bnr_weight_d135), 0, 31);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_S32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstNRFilterAttr->stAuto.NrLscRatio[i], reg_bnr_lsc_ratio, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[0][i], reg_bnr_intensity_sel_0, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[1][i], reg_bnr_intensity_sel_1, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[2][i], reg_bnr_intensity_sel_2, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[3][i], reg_bnr_intensity_sel_3, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[4][i], reg_bnr_intensity_sel_4, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[5][i], reg_bnr_intensity_sel_5, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[6][i], reg_bnr_intensity_sel_6, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.LumaStr[7][i], reg_bnr_intensity_sel_7, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.VarThr[i], reg_bnr_var_th, 0, 1023);
		PRINT_ATTR(pstNRFilterAttr->stAuto.CoringWgtLF[i], reg_bnr_res_ratio_k_smooth, 0, 256);
		PRINT_ATTR(pstNRFilterAttr->stAuto.CoringWgtHF[i], reg_bnr_res_ratio_k_texture, 0, 256);
		PRINT_ATTR(pstNRFilterAttr->stAuto.NonDirFiltStr[i], reg_bnr_weight_smooth, 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.VhDirFiltStr[i], (reg_bnr_weight_v, reg_bnr_weight_h), 0, 31);
		PRINT_ATTR(pstNRFilterAttr->stAuto.AaDirFiltStr[i]
			, (reg_bnr_weight_d35, reg_bnr_weight_d135), 0, 31);
	}
	#endif
}

CVI_S32 CVI_ISP_SetNRFilterAttr(VI_PIPE ViPipe, const ISP_NRFilter_ATTR_S *pstNRFilterAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstNRFilterAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_NR_TUNINGMODE, pstNRFilterAttr->TuningMode);

	// manual attributes
	isp_param_set(ViPipe, ISP_NR_MANUAL_NRLSCRATIO, pstNRFilterAttr->stManual.NrLscRatio);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR0, pstNRFilterAttr->stManual.LumaStr[0]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR1, pstNRFilterAttr->stManual.LumaStr[1]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR2, pstNRFilterAttr->stManual.LumaStr[2]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR3, pstNRFilterAttr->stManual.LumaStr[3]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR4, pstNRFilterAttr->stManual.LumaStr[4]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR5, pstNRFilterAttr->stManual.LumaStr[5]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR6, pstNRFilterAttr->stManual.LumaStr[6]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_LUMASTR7, pstNRFilterAttr->stManual.LumaStr[7]);
	isp_param_set(ViPipe, ISP_NR_MANUAL_VARTHR, pstNRFilterAttr->stManual.VarThr);
	isp_param_set(ViPipe, ISP_NR_MANUAL_CORINGWGTLF, pstNRFilterAttr->stManual.CoringWgtLF);
	isp_param_set(ViPipe, ISP_NR_MANUAL_CORINGWGTHF, pstNRFilterAttr->stManual.CoringWgtHF);
	isp_param_set(ViPipe, ISP_NR_MANUAL_NONDIRFILTSTR, pstNRFilterAttr->stManual.NonDirFiltStr);
	isp_param_set(ViPipe, ISP_NR_MANUAL_VHDIRFILTSTR, pstNRFilterAttr->stManual.VhDirFiltStr);
	isp_param_set(ViPipe, ISP_NR_MANUAL_AADIRFILTSTR, pstNRFilterAttr->stManual.AaDirFiltStr);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_NR_AUTO_NRLSCRATIO + i, pstNRFilterAttr->stAuto.NrLscRatio[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR0 + i, pstNRFilterAttr->stAuto.LumaStr[0][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR1 + i, pstNRFilterAttr->stAuto.LumaStr[1][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR2 + i, pstNRFilterAttr->stAuto.LumaStr[2][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR3 + i, pstNRFilterAttr->stAuto.LumaStr[3][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR4 + i, pstNRFilterAttr->stAuto.LumaStr[4][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR5 + i, pstNRFilterAttr->stAuto.LumaStr[5][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR6 + i, pstNRFilterAttr->stAuto.LumaStr[6][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_LUMASTR7 + i, pstNRFilterAttr->stAuto.LumaStr[7][i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_VARTHR + i, pstNRFilterAttr->stAuto.VarThr[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_CORINGWGTLF + i, pstNRFilterAttr->stAuto.CoringWgtLF[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_CORINGWGTHF + i, pstNRFilterAttr->stAuto.CoringWgtHF[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_NONDIRFILTSTR + i, pstNRFilterAttr->stAuto.NonDirFiltStr[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_VHDIRFILTSTR + i, pstNRFilterAttr->stAuto.VhDirFiltStr[i]);
		isp_param_set(ViPipe, ISP_NR_AUTO_AADIRFILTSTR + i, pstNRFilterAttr->stAuto.AaDirFiltStr[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintNRFilterAttr(pstNRFilterAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_BNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetNRFilterAttr(VI_PIPE ViPipe, ISP_NRFilter_ATTR_S *pstNRFilterAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstNRFilterAttr);

	// non-auto attributes
	isp_param_get(ViPipe, ISP_NR_TUNINGMODE, (void *)(uintptr_t)&(pstNRFilterAttr->TuningMode));

	// manual attributes
	isp_param_get(ViPipe, ISP_NR_MANUAL_NRLSCRATIO, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.NrLscRatio));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR0, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[0]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR1, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[1]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR2, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[2]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR3, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[3]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR4, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[4]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR5, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[5]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR6, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[6]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_LUMASTR7, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.LumaStr[7]));
	isp_param_get(ViPipe, ISP_NR_MANUAL_VARTHR, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.VarThr));
	isp_param_get(ViPipe, ISP_NR_MANUAL_CORINGWGTLF, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.CoringWgtLF));
	isp_param_get(ViPipe, ISP_NR_MANUAL_CORINGWGTHF, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.CoringWgtHF));
	isp_param_get(ViPipe, ISP_NR_MANUAL_NONDIRFILTSTR,
		(void *)(uintptr_t)&(pstNRFilterAttr->stManual.NonDirFiltStr));
	isp_param_get(ViPipe, ISP_NR_MANUAL_VHDIRFILTSTR, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.VhDirFiltStr));
	isp_param_get(ViPipe, ISP_NR_MANUAL_AADIRFILTSTR, (void *)(uintptr_t)&(pstNRFilterAttr->stManual.AaDirFiltStr));

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_NR_AUTO_NRLSCRATIO + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.NrLscRatio[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR0 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[0][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR1 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[1][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR2 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[2][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR3 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[3][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR4 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[4][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR5 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[5][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR6 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[6][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_LUMASTR7 + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.LumaStr[7][i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_VARTHR + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.VarThr[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_CORINGWGTLF + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.CoringWgtLF[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_CORINGWGTHF + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.CoringWgtHF[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_NONDIRFILTSTR + i,
			      (void *)(uintptr_t)&(pstNRFilterAttr->stAuto.NonDirFiltStr[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_VHDIRFILTSTR + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.VhDirFiltStr[i]));
		isp_param_get(ViPipe, ISP_NR_AUTO_AADIRFILTSTR + i,
			(void *)(uintptr_t)&(pstNRFilterAttr->stAuto.AaDirFiltStr[i]));
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintNRFilterAttr(pstNRFilterAttr);

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintRLSCAttr(const ISP_RLSC_ATTR_S *pstRLSCAttr)
{
	// non-auto attributes
	PRINT_ATTR(pstRLSCAttr->RlscEnable, reg_cbns_enable, 0, 1);
	PRINT_ATTR(pstRLSCAttr->RlscCenterX, reg_cbns_centerx, 0, 4095);
	PRINT_ATTR(pstRLSCAttr->RlscCenterY, reg_cbns_centery, 0, 2047);

	// manual attributes
	PRINT_ATTR(pstRLSCAttr->stManual.RadialStr, reg_cbns_strnth, 0, 4095);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_S32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		PRINT_ATTR(pstRLSCAttr->stAuto.RadialStr[i], reg_cbns_strnth, 0, 4095);
	#endif
}

CVI_S32 CVI_ISP_SetRLSCAttr(VI_PIPE ViPipe, const ISP_RLSC_ATTR_S *pstRLSCAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstRLSCAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_NR_RLSCENABLE, pstRLSCAttr->RlscEnable);
	isp_param_set(ViPipe, ISP_NR_RLSCCENTERX, pstRLSCAttr->RlscCenterX);
	isp_param_set(ViPipe, ISP_NR_RLSCCENTERY, pstRLSCAttr->RlscCenterY);

	// manual attributes
	isp_param_set(ViPipe, ISP_NR_MANUAL_RADIALSTR, pstRLSCAttr->stManual.RadialStr);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		isp_param_set(ViPipe, ISP_NR_AUTO_RADIALSTR + i, pstRLSCAttr->stAuto.RadialStr[i]);

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintRLSCAttr(pstRLSCAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_BNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetRLSCAttr(VI_PIPE ViPipe, ISP_RLSC_ATTR_S *pstRLSCAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstRLSCAttr);

	// non-auto attributes
	isp_param_get(ViPipe, ISP_NR_RLSCENABLE, (void *)(uintptr_t)&(pstRLSCAttr->RlscEnable));
	isp_param_get(ViPipe, ISP_NR_RLSCCENTERX, (void *)(uintptr_t)&(pstRLSCAttr->RlscCenterX));
	isp_param_get(ViPipe, ISP_NR_RLSCCENTERY, (void *)(uintptr_t)&(pstRLSCAttr->RlscCenterY));

	// manual attributes
	isp_param_get(ViPipe, ISP_NR_MANUAL_RADIALSTR, (void *)(uintptr_t)&(pstRLSCAttr->stManual.RadialStr));

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		isp_param_get(ViPipe, ISP_NR_AUTO_RADIALSTR + i,
			(void *)(uintptr_t)&(pstRLSCAttr->stAuto.RadialStr[i]));

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintRLSCAttr(pstRLSCAttr);

	return CVI_SUCCESS;
}

//-----------------------------------------------------------------------------
//	Dehaze
//-----------------------------------------------------------------------------
static void CVI_ISP_PrintDehazeAttr(const ISP_DEHAZE_ATTR_S *pstDehazeAttr)
{
	// non-auto attributes
	PRINT_ATTR(pstDehazeAttr->Enable, reg_dehaze_enable, 0, 2);
	PRINT_ATTR(pstDehazeAttr->enOpType, reg_na, 0, 2);
	PRINT_ATTR(pstDehazeAttr->CumulativeThr, reg_dehaze_cum_th, 0, 16383);
	PRINT_ATTR(pstDehazeAttr->MinTransMapValue, reg_dehaze_tmap_min, 0, 8192);

	// manual attributes
	PRINT_ATTR(pstDehazeAttr->stManual.Strength, reg_dehaze_w, 0, 100);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_S32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		PRINT_ATTR(pstDehazeAttr->stAuto.Strength[i], reg_dehaze_w, 0, 100);
	#endif
}

CVI_S32 CVI_ISP_SetDehazeAttr(VI_PIPE ViPipe, const ISP_DEHAZE_ATTR_S *pstDehazeAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDehazeAttr);

	isp_param_set(ViPipe, ISP_DEHAZE_ENABLE, pstDehazeAttr->Enable);
	isp_param_set(ViPipe, ISP_DEHAZE_ENOPTYPE, pstDehazeAttr->enOpType);
	isp_param_set(ViPipe, ISP_DEHAZE_MANUAL_CUMULATIVETHR, pstDehazeAttr->CumulativeThr);
	isp_param_set(ViPipe, ISP_DEHAZE_MANUAL_MINTRANSMAPVALUE, pstDehazeAttr->MinTransMapValue);

	isp_param_set(ViPipe, ISP_DEHAZE_MANUAL_STRENGTH, pstDehazeAttr->stManual.Strength);
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		isp_param_set(ViPipe, ISP_DEHAZE_AUTO_STRENGTH + i, pstDehazeAttr->stAuto.Strength[i]);

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDehazeAttr(pstDehazeAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DEHAZE);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDehazeAttr(VI_PIPE ViPipe, ISP_DEHAZE_ATTR_S *pstDehazeAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDehazeAttr);

	CLEAR(*pstDehazeAttr);
	isp_param_get(ViPipe, ISP_DEHAZE_ENABLE, (void *)(uintptr_t)&(pstDehazeAttr->Enable));
	isp_param_get(ViPipe, ISP_DEHAZE_ENOPTYPE, (void *)(uintptr_t)&(pstDehazeAttr->enOpType));
	isp_param_get(ViPipe, ISP_DEHAZE_MANUAL_CUMULATIVETHR, (void *)(uintptr_t)&(pstDehazeAttr->CumulativeThr));
	isp_param_get(ViPipe, ISP_DEHAZE_MANUAL_MINTRANSMAPVALUE,
		(void *)(uintptr_t)&(pstDehazeAttr->MinTransMapValue));

	isp_param_get(ViPipe, ISP_DEHAZE_MANUAL_STRENGTH, (void *)(uintptr_t)&(pstDehazeAttr->stManual.Strength));
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++)
		isp_param_get(ViPipe, ISP_DEHAZE_AUTO_STRENGTH + i,
			(void *)(uintptr_t)&(pstDehazeAttr->stAuto.Strength[i]));

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintDehazeAttr(pstDehazeAttr);

	return CVI_SUCCESS;
}

//-----------------------------------------------------------------------------
//	YNR
//-----------------------------------------------------------------------------
static CVI_BOOL is_value_in_array(CVI_S32 value, CVI_S32 *array, CVI_U32 length)
{
	CVI_U32 i;

	for (i = 0; i < length; i++)
		if (array[i] == value)
			break;

	return i != length;
}

CVI_S32 CVI_ISP_SetYNRAttr(VI_PIPE ViPipe, const ISP_YNR_ATTR_S *pstYNRAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstYNRAttr);

	CVI_S32 TuningModeList[] = {8, 11, 12, 13, 14, 15};
#if 0
	CVI_S32 i = 0;

	// non-auto attributes
	isp_param_set(ViPipe, ISP_YNR_ENABLE, pstYNRAttr->Enable);
	isp_param_set(ViPipe, ISP_YNR_ENOPTYPE, pstYNRAttr->enOpType);
	if (is_value_in_array(pstYNRAttr->TuningMode, TuningModeList, ARRAY_SIZE(TuningModeList)))
		isp_param_set(ViPipe, ISP_YNR_MANUAL_TUNINGMODE, pstYNRAttr->TuningMode);
	else
		ISP_DEBUG(LOG_WARNING, "tuning moode only accept values in 8, 11, 12, 13, 14, 15\n");

	// manual attributes
	isp_param_set(ViPipe, ISP_YNR_MANUAL_WINDOWTYPE, pstYNRAttr->stManual.WindowType);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_DETAILSMOOTHMODE, pstYNRAttr->stManual.DetailSmoothMode);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_NOISESUPPRESSSTR, pstYNRAttr->stManual.NoiseSuppressStr);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_FILTERTYPE, pstYNRAttr->stManual.FilterType);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_NOISESUPPRESSSTRMODE, pstYNRAttr->stManual.NoiseSuppressStrMode);

	// auto attributes
	for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_YNR_AUTO_WINDOWTYPE + i, pstYNRAttr->stAuto.WindowType[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_DETAILSMOOTHMODE + i, pstYNRAttr->stAuto.DetailSmoothMode[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_FILTERTYPE + i, pstYNRAttr->stAuto.NoiseSuppressStr[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_NOISESUPPRESSSTR + i, pstYNRAttr->stAuto.FilterType[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_NOISESUPPRESSSTRMODE + i,
			      pstYNRAttr->stAuto.NoiseSuppressStrMode[i]);
	}

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_YNR);

	return CVI_SUCCESS;
#endif
	if (!is_value_in_array(pstYNRAttr->TuningMode, TuningModeList, ARRAY_SIZE(TuningModeList)))
		ISP_DEBUG(LOG_WARNING, "tuning moode only accept values in 8, 11, 12, 13, 14, 15\n");
	memcpy(&g_param[ViPipe].YNR, pstYNRAttr, sizeof(*pstYNRAttr));

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DCI);
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetYNRAttr(VI_PIPE ViPipe, ISP_YNR_ATTR_S *pstYNRAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstYNRAttr);

#if 0
	CVI_S32 i = 0;

	CLEAR(*pstYNRAttr);
	// non-auto attributes
	isp_param_get(ViPipe, ISP_YNR_ENABLE, (void *)(uintptr_t)&(pstYNRAttr->Enable));
	isp_param_get(ViPipe, ISP_YNR_ENOPTYPE, (void *)(uintptr_t)&(pstYNRAttr->enOpType));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_TUNINGMODE, (void *)(uintptr_t)&(pstYNRAttr->TuningMode));

	// manual attributes
	isp_param_get(ViPipe, ISP_YNR_MANUAL_WINDOWTYPE, (void *)(uintptr_t)&(pstYNRAttr->stManual.WindowType));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_DETAILSMOOTHMODE,
		(void *)(uintptr_t)&(pstYNRAttr->stManual.DetailSmoothMode));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_NOISESUPPRESSSTR,
		(void *)(uintptr_t)&(pstYNRAttr->stManual.NoiseSuppressStr));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_FILTERTYPE, (void *)(uintptr_t)&(pstYNRAttr->stManual.FilterType));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_NOISESUPPRESSSTRMODE,
		      (void *)(uintptr_t)&(pstYNRAttr->stManual.NoiseSuppressStrMode));

	// auto attributes
	for (i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_YNR_AUTO_WINDOWTYPE + i,
			(void *)(uintptr_t)&(pstYNRAttr->stAuto.WindowType[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_DETAILSMOOTHMODE + i,
			      (void *)(uintptr_t)&(pstYNRAttr->stAuto.DetailSmoothMode[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_FILTERTYPE + i,
			(void *)(uintptr_t)&(pstYNRAttr->stAuto.NoiseSuppressStr[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_NOISESUPPRESSSTR + i,
			(void *)(uintptr_t)&(pstYNRAttr->stAuto.FilterType[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_NOISESUPPRESSSTRMODE + i,
			      (void *)(uintptr_t)&(pstYNRAttr->stAuto.NoiseSuppressStrMode[i]));
	}

	return CVI_SUCCESS;
#endif
	memcpy(pstYNRAttr, &g_param[ViPipe].YNR, sizeof(*pstYNRAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetYNRMotionNRAttr(VI_PIPE ViPipe, const ISP_YNRMotionNR_ATTR_S *pstYNRMotionNRAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstYNRMotionNRAttr);

	// non-auto attributes

	// manual attributes
	isp_param_set(ViPipe, ISP_YNR_MANUAL_MOTIONTHR, pstYNRMotionNRAttr->stManual.MotionThr);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_MOTIONNRPOSGAIN, pstYNRMotionNRAttr->stManual.MotionNrPosGain);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_MOTIONNRNEGGAIN, pstYNRMotionNRAttr->stManual.MotionNrNegGain);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_YNR_AUTO_MOTIONTHR + i, pstYNRMotionNRAttr->stAuto.MotionThr[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_MOTIONNRPOSGAIN + i, pstYNRMotionNRAttr->stAuto.MotionNrPosGain[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_MOTIONNRNEGGAIN + i, pstYNRMotionNRAttr->stAuto.MotionNrNegGain[i]);
	}

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_YNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetYNRMotionNRAttr(VI_PIPE ViPipe, ISP_YNRMotionNR_ATTR_S *pstYNRMotionNRAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstYNRMotionNRAttr);

	// non-auto attributes

	// manual attributes
	isp_param_get(ViPipe, ISP_YNR_MANUAL_MOTIONTHR, (void *)(uintptr_t)&(pstYNRMotionNRAttr->stManual.MotionThr));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_MOTIONNRPOSGAIN,
		(void *)(uintptr_t)&(pstYNRMotionNRAttr->stManual.MotionNrPosGain));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_MOTIONNRNEGGAIN,
		(void *)(uintptr_t)&(pstYNRMotionNRAttr->stManual.MotionNrNegGain));

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_YNR_AUTO_MOTIONTHR + i,
			(void *)(uintptr_t)&(pstYNRMotionNRAttr->stAuto.MotionThr[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_MOTIONNRPOSGAIN + i,
			      (void *)(uintptr_t)&(pstYNRMotionNRAttr->stAuto.MotionNrPosGain[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_MOTIONNRNEGGAIN + i,
			      (void *)(uintptr_t)&(pstYNRMotionNRAttr->stAuto.MotionNrNegGain[i]));
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetYNRFilterAttr(VI_PIPE ViPipe, const ISP_YNRFilter_ATTR_S *pstYNRFilterAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstYNRFilterAttr);

	// non-auto attributes

	// manual attributes
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR0, pstYNRFilterAttr->stManual.LumaStr[0]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR1, pstYNRFilterAttr->stManual.LumaStr[1]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR2, pstYNRFilterAttr->stManual.LumaStr[2]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR3, pstYNRFilterAttr->stManual.LumaStr[3]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR4, pstYNRFilterAttr->stManual.LumaStr[4]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR5, pstYNRFilterAttr->stManual.LumaStr[5]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR6, pstYNRFilterAttr->stManual.LumaStr[6]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_LUMASTR7, pstYNRFilterAttr->stManual.LumaStr[7]);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_VARTHR, pstYNRFilterAttr->stManual.VarThr);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_CORINGWGTLF, pstYNRFilterAttr->stManual.CoringWgtLF);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_CORINGWGTHF, pstYNRFilterAttr->stManual.CoringWgtHF);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_NONDIRFILTSTR, pstYNRFilterAttr->stManual.NonDirFiltStr);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_VHDIRFILTSTR, pstYNRFilterAttr->stManual.VhDirFiltStr);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_AADIRFILTSTR, pstYNRFilterAttr->stManual.AaDirFiltStr);
	isp_param_set(ViPipe, ISP_YNR_MANUAL_FILTERMODE, pstYNRFilterAttr->stManual.FilterMode);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR0 + i, pstYNRFilterAttr->stAuto.LumaStr[0][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR1 + i, pstYNRFilterAttr->stAuto.LumaStr[1][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR2 + i, pstYNRFilterAttr->stAuto.LumaStr[2][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR3 + i, pstYNRFilterAttr->stAuto.LumaStr[3][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR4 + i, pstYNRFilterAttr->stAuto.LumaStr[4][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR5 + i, pstYNRFilterAttr->stAuto.LumaStr[5][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR6 + i, pstYNRFilterAttr->stAuto.LumaStr[6][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_LUMASTR7 + i, pstYNRFilterAttr->stAuto.LumaStr[7][i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_VARTHR + i, pstYNRFilterAttr->stAuto.VarThr[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_CORINGWGTLF + i, pstYNRFilterAttr->stAuto.CoringWgtLF[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_CORINGWGTHF + i, pstYNRFilterAttr->stAuto.CoringWgtHF[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_NONDIRFILTSTR + i, pstYNRFilterAttr->stAuto.NonDirFiltStr[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_VHDIRFILTSTR + i, pstYNRFilterAttr->stAuto.VhDirFiltStr[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_AADIRFILTSTR + i, pstYNRFilterAttr->stAuto.AaDirFiltStr[i]);
		isp_param_set(ViPipe, ISP_YNR_AUTO_FILTERMODE + i, pstYNRFilterAttr->stAuto.FilterMode[i]);
	}

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_YNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetYNRFilterAttr(VI_PIPE ViPipe, ISP_YNRFilter_ATTR_S *pstYNRFilterAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstYNRFilterAttr);

	// non-auto attributes

	// manual attributes
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR0, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[0]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR1, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[1]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR2, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[2]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR3, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[3]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR4, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[4]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR5, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[5]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR6, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[6]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_LUMASTR7, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.LumaStr[7]));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_VARTHR, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.VarThr));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_CORINGWGTLF, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.CoringWgtLF));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_CORINGWGTHF, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.CoringWgtHF));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_NONDIRFILTSTR,
		(void *)(uintptr_t)&(pstYNRFilterAttr->stManual.NonDirFiltStr));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_VHDIRFILTSTR,
		(void *)(uintptr_t)&(pstYNRFilterAttr->stManual.VhDirFiltStr));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_AADIRFILTSTR,
		(void *)(uintptr_t)&(pstYNRFilterAttr->stManual.AaDirFiltStr));
	isp_param_get(ViPipe, ISP_YNR_MANUAL_FILTERMODE, (void *)(uintptr_t)&(pstYNRFilterAttr->stManual.FilterMode));

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR0 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[0][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR1 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[1][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR2 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[2][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR3 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[3][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR4 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[4][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR5 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[5][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR6 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[6][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_LUMASTR7 + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.LumaStr[7][i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_VARTHR + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.VarThr[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_CORINGWGTLF + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.CoringWgtLF[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_CORINGWGTHF + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.CoringWgtHF[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_NONDIRFILTSTR + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.NonDirFiltStr[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_VHDIRFILTSTR + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.VhDirFiltStr[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_AADIRFILTSTR + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.AaDirFiltStr[i]));
		isp_param_get(ViPipe, ISP_YNR_AUTO_FILTERMODE + i,
			(void *)(uintptr_t)&(pstYNRFilterAttr->stAuto.FilterMode[i]));
	}

	return CVI_SUCCESS;
}

//-----------------------------------------------------------------------------
//	CNR
//-----------------------------------------------------------------------------
static void CVI_ISP_PrintCNRAttr(const ISP_CNR_ATTR_S *pstCNRAttr)
{
	// non-auto attributes
	PRINT_ATTR(pstCNRAttr->Enable, reg_cnr_enable, 0, 1);

	// manual attributes
	PRINT_ATTR(pstCNRAttr->stManual.CnrStr, reg_cnr_strength_mode, 0, 255);
	PRINT_ATTR(pstCNRAttr->stManual.NoiseSuppressStr, reg_cnr_diff_shift_val, 0, 255);
	PRINT_ATTR(pstCNRAttr->stManual.NoiseSuppressGain, reg_cnr_diff_gain, 0, 7);
	PRINT_ATTR(pstCNRAttr->stManual.FilterType, reg_cnr_weight_lut_inter_block, 0, 16);
	PRINT_ATTR(pstCNRAttr->stManual.MotionNrStr, reg_cnr_ratio, 0, 255);
	PRINT_ATTR(pstCNRAttr->stManual.LumaWgt, reg_cnr_fusion_intensity_weight, 0, 8);
	PRINT_ATTR(pstCNRAttr->stManual.DetailSmoothMode, reg_cnr_flag_neighbor_max_weight, 0, 1);

	// auto attributes
	#if PRINT_ATTR_AUTO
	for (CVI_S32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstCNRAttr->stAuto.CnrStr[i], reg_cnr_strength_mode, 0, 255);
		PRINT_ATTR(pstCNRAttr->stAuto.NoiseSuppressStr[i], reg_cnr_diff_shift_val, 0, 255);
		PRINT_ATTR(pstCNRAttr->stAuto.NoiseSuppressGain[i], reg_cnr_diff_gain, 0, 7);
		PRINT_ATTR(pstCNRAttr->stAuto.FilterType[i], reg_cnr_weight_lut_inter_block, 0, 16);
		PRINT_ATTR(pstCNRAttr->stAuto.MotionNrStr[i], reg_cnr_ratio, 0, 255);
		PRINT_ATTR(pstCNRAttr->stAuto.LumaWgt[i], reg_cnr_fusion_intensity_weight, 0, 8);
		PRINT_ATTR(pstCNRAttr->stAuto.DetailSmoothMode[i], reg_cnr_flag_neighbor_max_weight, 0, 1);
	}
	#endif
}

CVI_S32 CVI_ISP_SetCNRAttr(VI_PIPE ViPipe, const ISP_CNR_ATTR_S *pstCNRAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstCNRAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_CNR_ENABLE, pstCNRAttr->Enable);
	isp_param_set(ViPipe, ISP_CNR_ENOPTYPE, pstCNRAttr->enOpType);

	// manual attributes
	isp_param_set(ViPipe, ISP_CNR_MANUAL_CNRSTR, pstCNRAttr->stManual.CnrStr);
	isp_param_set(ViPipe, ISP_CNR_MANUAL_NOISESUPPRESSSTR, pstCNRAttr->stManual.NoiseSuppressStr);
	isp_param_set(ViPipe, ISP_CNR_MANUAL_NOISESUPPRESSGAIN, pstCNRAttr->stManual.NoiseSuppressGain);
	isp_param_set(ViPipe, ISP_CNR_MANUAL_FILTERTYPE, pstCNRAttr->stManual.FilterType);
	isp_param_set(ViPipe, ISP_CNR_MANUAL_MOTIONNRSTR, pstCNRAttr->stManual.MotionNrStr);
	isp_param_set(ViPipe, ISP_CNR_MANUAL_LUMAWGT, pstCNRAttr->stManual.LumaWgt);
	isp_param_set(ViPipe, ISP_CNR_MANUAL_DETAILSMOOTHMODE, pstCNRAttr->stManual.DetailSmoothMode);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_CNR_AUTO_CNRSTR + i, pstCNRAttr->stAuto.CnrStr[i]);
		isp_param_set(ViPipe, ISP_CNR_AUTO_NOISESUPPRESSSTR + i, pstCNRAttr->stAuto.NoiseSuppressStr[i]);
		isp_param_set(ViPipe, ISP_CNR_AUTO_NOISESUPPRESSGAIN + i, pstCNRAttr->stAuto.NoiseSuppressGain[i]);
		isp_param_set(ViPipe, ISP_CNR_AUTO_FILTERTYPE + i, pstCNRAttr->stAuto.FilterType[i]);
		isp_param_set(ViPipe, ISP_CNR_AUTO_MOTIONNRSTR + i, pstCNRAttr->stAuto.MotionNrStr[i]);
		isp_param_set(ViPipe, ISP_CNR_AUTO_LUMAWGT + i, pstCNRAttr->stAuto.LumaWgt[i]);
		isp_param_set(ViPipe, ISP_CNR_AUTO_DETAILSMOOTHMODE + i, pstCNRAttr->stAuto.DetailSmoothMode[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintCNRAttr(pstCNRAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_CNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetCNRAttr(VI_PIPE ViPipe, ISP_CNR_ATTR_S *pstCNRAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstCNRAttr);

	CLEAR(*pstCNRAttr);
	// non-auto parameters
	isp_param_get(ViPipe, ISP_CNR_ENABLE, (void *)(uintptr_t)&(pstCNRAttr->Enable));
	isp_param_get(ViPipe, ISP_CNR_ENOPTYPE, (void *)(uintptr_t)&(pstCNRAttr->enOpType));

	// manual parameters
	isp_param_get(ViPipe, ISP_CNR_MANUAL_CNRSTR, (void *)(uintptr_t)&(pstCNRAttr->stManual.CnrStr));
	isp_param_get(ViPipe, ISP_CNR_MANUAL_NOISESUPPRESSSTR,
		(void *)(uintptr_t)&(pstCNRAttr->stManual.NoiseSuppressStr));
	isp_param_get(ViPipe, ISP_CNR_MANUAL_NOISESUPPRESSGAIN,
		(void *)(uintptr_t)&(pstCNRAttr->stManual.NoiseSuppressGain));
	isp_param_get(ViPipe, ISP_CNR_MANUAL_FILTERTYPE, (void *)(uintptr_t)&(pstCNRAttr->stManual.FilterType));
	isp_param_get(ViPipe, ISP_CNR_MANUAL_MOTIONNRSTR, (void *)(uintptr_t)&(pstCNRAttr->stManual.MotionNrStr));
	isp_param_get(ViPipe, ISP_CNR_MANUAL_LUMAWGT, (void *)(uintptr_t)&(pstCNRAttr->stManual.LumaWgt));
	isp_param_get(ViPipe, ISP_CNR_MANUAL_DETAILSMOOTHMODE,
		(void *)(uintptr_t)&(pstCNRAttr->stManual.DetailSmoothMode));

	// auto parameters
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_CNR_AUTO_CNRSTR + i, (void *)(uintptr_t)&(pstCNRAttr->stAuto.CnrStr[i]));
		isp_param_get(ViPipe, ISP_CNR_AUTO_NOISESUPPRESSSTR + i,
			      (void *)(uintptr_t)&(pstCNRAttr->stAuto.NoiseSuppressStr[i]));
		isp_param_get(ViPipe, ISP_CNR_AUTO_NOISESUPPRESSGAIN + i,
			      (void *)(uintptr_t)&(pstCNRAttr->stAuto.NoiseSuppressGain[i]));
		isp_param_get(ViPipe, ISP_CNR_AUTO_FILTERTYPE + i,
			(void *)(uintptr_t)&(pstCNRAttr->stAuto.FilterType[i]));
		isp_param_get(ViPipe, ISP_CNR_AUTO_MOTIONNRSTR + i,
			(void *)(uintptr_t)&(pstCNRAttr->stAuto.MotionNrStr[i]));
		isp_param_get(ViPipe, ISP_CNR_AUTO_LUMAWGT + i, (void *)(uintptr_t)&(pstCNRAttr->stAuto.LumaWgt[i]));
		isp_param_get(ViPipe, ISP_CNR_AUTO_DETAILSMOOTHMODE + i,
			      (void *)(uintptr_t)&(pstCNRAttr->stAuto.DetailSmoothMode[i]));
	}

	// TODO should add fixed parameters?
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintCNRAttr(pstCNRAttr);

	return CVI_SUCCESS;
}

//-----------------------------------------------------------------------------
//	LocalCac
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_SetCACAttr(VI_PIPE ViPipe, const ISP_CAC_ATTR_S *pstCACAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstCACAttr);

	isp_param_set(ViPipe, ISP_CAC_ENABLE, pstCACAttr->Enable);
	isp_param_set(ViPipe, ISP_CAC_ENOPTYPE, pstCACAttr->enOpType);
	isp_param_set(ViPipe, ISP_CAC_TUNINGMODE, pstCACAttr->TuningMode);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_VARTHR, pstCACAttr->VarThr);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_PURPLEDETRANGE, pstCACAttr->PurpleDetRange);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_DEPURPLESTR, pstCACAttr->stManual.DePurpleStr);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_PURPLECB, pstCACAttr->PurpleCb);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_PURPLECR, pstCACAttr->PurpleCr);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_GREENCB, pstCACAttr->GreenCb);
	isp_param_set(ViPipe, ISP_CAC_MANUAL_GREENCR, pstCACAttr->GreenCr);
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_CAC_AUTO_DEPURPLESTR + i, pstCACAttr->stAuto.DePurpleStr[i]);
	}
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_CAC);
	isp_iq_auto_set(ViPipe, ISP_IQ_BLOCK_CAC, pstCACAttr->enOpType);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetCACAttr(VI_PIPE ViPipe, ISP_CAC_ATTR_S *pstCACAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstCACAttr);

	CLEAR(*pstCACAttr);
	isp_param_get(ViPipe, ISP_CAC_ENABLE, (void *)(uintptr_t)&(pstCACAttr->Enable));
	isp_param_get(ViPipe, ISP_CAC_ENOPTYPE, (void *)(uintptr_t)&(pstCACAttr->enOpType));
	isp_param_get(ViPipe, ISP_CAC_TUNINGMODE, (void *)(uintptr_t)&(pstCACAttr->TuningMode));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_VARTHR, (void *)(uintptr_t)&(pstCACAttr->VarThr));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_PURPLEDETRANGE, (void *)(uintptr_t)&(pstCACAttr->PurpleDetRange));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_DEPURPLESTR, (void *)(uintptr_t)&(pstCACAttr->stManual.DePurpleStr));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_PURPLECB, (void *)(uintptr_t)&(pstCACAttr->PurpleCb));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_PURPLECR, (void *)(uintptr_t)&(pstCACAttr->PurpleCr));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_GREENCB, (void *)(uintptr_t)&(pstCACAttr->GreenCb));
	isp_param_get(ViPipe, ISP_CAC_MANUAL_GREENCR, (void *)(uintptr_t)&(pstCACAttr->GreenCr));
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_CAC_AUTO_DEPURPLESTR + i,
			      (void *)(uintptr_t)&(pstCACAttr->stAuto.DePurpleStr[i]));
	}

	return CVI_SUCCESS;
}

const CVI_U16 defaultGamma[GAMMA_NODE_NUM] = {
	   0,   21,   55,  101,  154,  212,  271,  334,  406,  483,  560,  634,  702,  765,  830,  895,
	 959, 1023, 1084, 1143, 1198, 1248, 1294, 1334, 1369, 1403, 1437, 1469, 1500, 1529, 1558, 1586,
	1613, 1640, 1665, 1690, 1714, 1738, 1761, 1784, 1806, 1828, 1850, 1871, 1892, 1913, 1935, 1955,
	1975, 1995, 2013, 2031, 2049, 2066, 2083, 2100, 2116, 2131, 2147, 2162, 2176, 2191, 2206, 2220,
	2234, 2249, 2263, 2277, 2291, 2306, 2321, 2335, 2349, 2364, 2378, 2392, 2406, 2419, 2433, 2446,
	2459, 2472, 2486, 2498, 2511, 2524, 2537, 2549, 2562, 2574, 2587, 2599, 2611, 2624, 2636, 2648,
	2660, 2672, 2683, 2695, 2707, 2718, 2730, 2741, 2752, 2764, 2775, 2786, 2797, 2808, 2819, 2829,
	2840, 2851, 2861, 2872, 2883, 2893, 2903, 2914, 2924, 2934, 2944, 2954, 2964, 2974, 2983, 2993,
	3003, 3012, 3022, 3032, 3041, 3051, 3060, 3070, 3079, 3088, 3098, 3107, 3117, 3126, 3135, 3144,
	3154, 3163, 3172, 3181, 3190, 3199, 3208, 3217, 3225, 3234, 3243, 3252, 3261, 3270, 3279, 3288,
	3297, 3305, 3314, 3324, 3333, 3342, 3351, 3360, 3369, 3378, 3387, 3397, 3406, 3415, 3424, 3433,
	3442, 3452, 3461, 3470, 3479, 3488, 3497, 3506, 3515, 3524, 3533, 3541, 3550, 3559, 3567, 3576,
	3585, 3593, 3602, 3611, 3619, 3628, 3636, 3645, 3653, 3661, 3670, 3678, 3687, 3695, 3703, 3712,
	3720, 3728, 3737, 3745, 3753, 3761, 3769, 3777, 3786, 3794, 3802, 3810, 3818, 3826, 3834, 3842,
	3850, 3858, 3866, 3874, 3881, 3889, 3897, 3905, 3913, 3921, 3928, 3936, 3945, 3953, 3961, 3970,
	3978, 3987, 3995, 4004, 4012, 4020, 4028, 4036, 4044, 4051, 4058, 4065, 4072, 4078, 4084, 4089
};

const CVI_U16 sRGBGamma[GAMMA_NODE_NUM] = {
	0,  203,  347,  452,  538,  613,  679,  739,  794,  845,  894,  939,  982, 1023, 1062, 1099,
	1136, 1170, 1204, 1237, 1268, 1299, 1329, 1358, 1386, 1414, 1441, 1467, 1493, 1518, 1543, 1567,
	1591, 1615, 1638, 1660, 1683, 1704, 1726, 1747, 1768, 1789, 1809, 1829, 1849, 1868, 1888, 1907,
	1926, 1944, 1962, 1981, 1998, 2016, 2034, 2051, 2068, 2085, 2102, 2119, 2135, 2151, 2168, 2184,
	2199, 2215, 2231, 2246, 2261, 2277, 2292, 2307, 2321, 2336, 2351, 2365, 2379, 2394, 2408, 2422,
	2436, 2449, 2463, 2477, 2490, 2504, 2517, 2530, 2543, 2557, 2570, 2582, 2595, 2608, 2621, 2633,
	2646, 2658, 2670, 2683, 2695, 2707, 2719, 2731, 2743, 2755, 2767, 2778, 2790, 2802, 2813, 2825,
	2836, 2847, 2859, 2870, 2881, 2892, 2903, 2914, 2925, 2936, 2947, 2958, 2969, 2979, 2990, 3001,
	3011, 3022, 3032, 3043, 3053, 3063, 3074, 3084, 3094, 3104, 3114, 3124, 3134, 3144, 3154, 3164,
	3174, 3184, 3194, 3203, 3213, 3223, 3232, 3242, 3252, 3261, 3271, 3280, 3289, 3299, 3308, 3317,
	3327, 3336, 3345, 3354, 3363, 3372, 3382, 3391, 3400, 3409, 3418, 3426, 3435, 3444, 3453, 3462,
	3471, 3479, 3488, 3497, 3505, 3514, 3523, 3531, 3540, 3548, 3557, 3565, 3574, 3582, 3590, 3599,
	3607, 3615, 3624, 3632, 3640, 3648, 3656, 3665, 3673, 3681, 3689, 3697, 3705, 3713, 3721, 3729,
	3737, 3745, 3753, 3761, 3769, 3776, 3784, 3792, 3800, 3807, 3815, 3823, 3831, 3838, 3846, 3854,
	3861, 3869, 3876, 3884, 3891, 3899, 3906, 3914, 3921, 3929, 3936, 3944, 3951, 3958, 3966, 3973,
	3980, 3988, 3995, 4002, 4009, 4017, 4024, 4031, 4038, 4045, 4053, 4060, 4067, 4074, 4081, 4088
};

CVI_S32 CVI_ISP_SetGammaAttr(VI_PIPE ViPipe, const ISP_GAMMA_ATTR_S *pstGammaAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstGammaAttr);

	memcpy(&g_param[ViPipe].gamma, pstGammaAttr, sizeof(*pstGammaAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_GAMMA);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetGammaCurveType(VI_PIPE ViPipe, const ISP_GAMMA_CURVE_TYPE_E curveType)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);

	g_param[ViPipe].gamma.enCurveType = curveType;
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_GAMMA);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetGammaAttr(VI_PIPE ViPipe, ISP_GAMMA_ATTR_S *pstGammaAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstGammaAttr);

	const CVI_U16 *gammaCurve;

	pstGammaAttr->Enable = g_param[ViPipe].gamma.Enable;
	pstGammaAttr->enCurveType = g_param[ViPipe].gamma.enCurveType;

	switch (pstGammaAttr->enCurveType) {
	case ISP_GAMMA_CURVE_DEFAULT:
		gammaCurve = defaultGamma;
		break;
	case ISP_GAMMA_CURVE_SRGB:
		gammaCurve = sRGBGamma;
		break;
	case ISP_GAMMA_CURVE_USER_DEFINE:
		gammaCurve = g_param[ViPipe].gamma.Table;
		break;
	default:
		gammaCurve = g_param[ViPipe].gamma.Table;
		break;
	}
	memcpy(pstGammaAttr->Table, gammaCurve, sizeof(CVI_U16) * GAMMA_NODE_NUM);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetAutoGammaAttr(VI_PIPE ViPipe, const ISP_AUTO_GAMMA_ATTR_S *pstGammaAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstGammaAttr);

	memcpy(&g_param[ViPipe].autoGamma, pstGammaAttr, sizeof(*pstGammaAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_GAMMA);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetAutoGammaAttr(VI_PIPE ViPipe, ISP_AUTO_GAMMA_ATTR_S *pstGammaAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstGammaAttr);

	memcpy(pstGammaAttr, &g_param[ViPipe].autoGamma, sizeof(*pstGammaAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetDCIAttr(VI_PIPE ViPipe, const ISP_DCI_ATTR_S *pstDCIAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDCIAttr);

	memcpy(&g_param[ViPipe].dci, pstDCIAttr, sizeof(*pstDCIAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DCI);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDCIAttr(VI_PIPE ViPipe, ISP_DCI_ATTR_S *pstDCIAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDCIAttr);

	memcpy(pstDCIAttr, &g_param[ViPipe].dci, sizeof(*pstDCIAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetDPCAttr(VI_PIPE ViPipe, const ISP_DPC_ATTR_S *pstDPCAttr)
{
	isp_param_set(ViPipe, ISP_DPC_ENOPTYPE, pstDPCAttr->enOpType);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_ENABLE, pstDPCAttr->stManual.dpc_enable);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_STATICDPC_ENABLE, pstDPCAttr->stManual.dpc_staticdpc_enable);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_CLUSTER_SIZE, pstDPCAttr->stManual.dpc_cluster_size);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_R_BRIGHT_PIXEL_RATIO, pstDPCAttr->stManual.dpc_r_bright_pixel_ratio);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_G_BRIGHT_PIXEL_RATIO, pstDPCAttr->stManual.dpc_g_bright_pixel_ratio);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_B_BRIGHT_PIXEL_RATIO, pstDPCAttr->stManual.dpc_b_bright_pixel_ratio);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_R_DARK_PIXEL_RATIO, pstDPCAttr->stManual.dpc_r_dark_pixel_ratio);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_G_DARK_PIXEL_RATIO, pstDPCAttr->stManual.dpc_g_dark_pixel_ratio);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_B_DARK_PIXEL_RATIO, pstDPCAttr->stManual.dpc_b_dark_pixel_ratio);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_R_DARK_PIXEL_MINDIFF, pstDPCAttr->stManual.dpc_r_dark_pixel_mindiff);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_G_DARK_PIXEL_MINDIFF, pstDPCAttr->stManual.dpc_g_dark_pixel_mindiff);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_B_DARK_PIXEL_MINDIFF, pstDPCAttr->stManual.dpc_b_dark_pixel_mindiff);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_R, pstDPCAttr->stManual.dpc_flat_thre_r);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_G, pstDPCAttr->stManual.dpc_flat_thre_g);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_B, pstDPCAttr->stManual.dpc_flat_thre_b);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_MIN_G, pstDPCAttr->stManual.dpc_flat_thre_min_g);
	isp_param_set(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_MIN_RB, pstDPCAttr->stManual.dpc_flat_thre_min_rb);

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_ENABLE + i, pstDPCAttr->stAuto.dpc_enable[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_STATICDPC_ENABLE + i,
			      pstDPCAttr->stAuto.dpc_staticdpc_enable[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_CLUSTER_SIZE + i, pstDPCAttr->stAuto.dpc_cluster_size[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_R_BRIGHT_PIXEL_RATIO + i,
			      pstDPCAttr->stAuto.dpc_r_bright_pixel_ratio[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_G_BRIGHT_PIXEL_RATIO + i,
			      pstDPCAttr->stAuto.dpc_g_bright_pixel_ratio[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_B_BRIGHT_PIXEL_RATIO + i,
			      pstDPCAttr->stAuto.dpc_b_bright_pixel_ratio[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_R_DARK_PIXEL_RATIO + i,
			      pstDPCAttr->stAuto.dpc_r_dark_pixel_ratio[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_G_DARK_PIXEL_RATIO + i,
			      pstDPCAttr->stAuto.dpc_g_dark_pixel_ratio[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_B_DARK_PIXEL_RATIO + i,
			      pstDPCAttr->stAuto.dpc_b_dark_pixel_ratio[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_R_DARK_PIXEL_MINDIFF + i,
			      pstDPCAttr->stAuto.dpc_r_dark_pixel_mindiff[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_G_DARK_PIXEL_MINDIFF + i,
			      pstDPCAttr->stAuto.dpc_g_dark_pixel_mindiff[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_B_DARK_PIXEL_MINDIFF + i,
			      pstDPCAttr->stAuto.dpc_b_dark_pixel_mindiff[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_R + i, pstDPCAttr->stAuto.dpc_flat_thre_r[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_G + i, pstDPCAttr->stAuto.dpc_flat_thre_g[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_B + i, pstDPCAttr->stAuto.dpc_flat_thre_b[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_MIN_G + i, pstDPCAttr->stAuto.dpc_flat_thre_min_g[i]);
		isp_param_set(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_MIN_RB + i,
			      pstDPCAttr->stAuto.dpc_flat_thre_min_rb[i]);
	}
	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDPCAttr(VI_PIPE ViPipe, ISP_DPC_ATTR_S *pstDPCAttr)
{
	CLEAR(*pstDPCAttr);
	isp_param_get(ViPipe, ISP_DPC_ENOPTYPE, (void *)(uintptr_t)&(pstDPCAttr->enOpType));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_ENABLE, (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_enable));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_STATICDPC_ENABLE,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_staticdpc_enable));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_CLUSTER_SIZE,
		(void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_cluster_size));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_R_BRIGHT_PIXEL_RATIO,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_r_bright_pixel_ratio));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_G_BRIGHT_PIXEL_RATIO,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_g_bright_pixel_ratio));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_B_BRIGHT_PIXEL_RATIO,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_b_bright_pixel_ratio));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_R_DARK_PIXEL_RATIO,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_r_dark_pixel_ratio));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_G_DARK_PIXEL_RATIO,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_g_dark_pixel_ratio));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_B_DARK_PIXEL_RATIO,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_b_dark_pixel_ratio));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_R_DARK_PIXEL_MINDIFF,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_r_dark_pixel_mindiff));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_G_DARK_PIXEL_MINDIFF,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_g_dark_pixel_mindiff));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_B_DARK_PIXEL_MINDIFF,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_b_dark_pixel_mindiff));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_R,
		(void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_flat_thre_r));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_G,
		(void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_flat_thre_g));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_B,
		(void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_flat_thre_b));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_MIN_G,
		(void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_flat_thre_min_g));
	isp_param_get(ViPipe, ISP_DPC_MANUAL_DPC_FLAT_THRE_MIN_RB,
		      (void *)(uintptr_t)&(pstDPCAttr->stManual.dpc_flat_thre_min_rb));

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_ENABLE + i,
			(void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_enable[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_STATICDPC_ENABLE + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_staticdpc_enable[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_CLUSTER_SIZE + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_cluster_size[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_R_BRIGHT_PIXEL_RATIO + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_r_bright_pixel_ratio[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_G_BRIGHT_PIXEL_RATIO + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_g_bright_pixel_ratio[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_B_BRIGHT_PIXEL_RATIO + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_b_bright_pixel_ratio[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_R_DARK_PIXEL_RATIO + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_r_dark_pixel_ratio[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_G_DARK_PIXEL_RATIO + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_g_dark_pixel_ratio[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_B_DARK_PIXEL_RATIO + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_b_dark_pixel_ratio[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_R_DARK_PIXEL_MINDIFF + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_r_dark_pixel_mindiff[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_G_DARK_PIXEL_MINDIFF + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_g_dark_pixel_mindiff[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_B_DARK_PIXEL_MINDIFF + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_b_dark_pixel_mindiff[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_R + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_flat_thre_r[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_G + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_flat_thre_g[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_B + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_flat_thre_b[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_MIN_G + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_flat_thre_min_g[i]));
		isp_param_get(ViPipe, ISP_DPC_AUTO_DPC_FLAT_THRE_MIN_RB + i,
			      (void *)(uintptr_t)&(pstDPCAttr->stAuto.dpc_flat_thre_min_rb[i]));
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetSharpenAttr(VI_PIPE ViPipe, const ISP_SHARPEN_ATTR_S *pstSharpenAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstSharpenAttr);

	memcpy(&g_param[ViPipe].Sharpen, pstSharpenAttr, sizeof(*pstSharpenAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_YEE);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetSharpenAttr(VI_PIPE ViPipe, ISP_SHARPEN_ATTR_S *pstSharpenAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstSharpenAttr);

	memcpy(pstSharpenAttr, &g_param[ViPipe].Sharpen, sizeof(*pstSharpenAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetTNRAttr(VI_PIPE ViPipe, const ISP_TNR_ATTR_S *pstTNRAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstTNRAttr);

	memcpy(&g_param[ViPipe].TNR, pstTNRAttr, sizeof(*pstTNRAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_3DNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetTNRAttr(VI_PIPE ViPipe, ISP_TNR_ATTR_S *pstTNRAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstTNRAttr);

	memcpy(pstTNRAttr, &g_param[ViPipe].TNR, sizeof(*pstTNRAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetTNRNoiseModelAttr(VI_PIPE ViPipe, const ISP_TNRNoiseModel_ATTR_S *pstTNRNoiseModelAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstTNRNoiseModelAttr);

	memcpy(&g_param[ViPipe].TnrNoiseModel, pstTNRNoiseModelAttr, sizeof(*pstTNRNoiseModelAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_3DNR);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetTNRNoiseModelAttr(VI_PIPE ViPipe, ISP_TNRNoiseModel_ATTR_S *pstTNRNoiseModelAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstTNRNoiseModelAttr);

	memcpy(pstTNRNoiseModelAttr, &g_param[ViPipe].TnrNoiseModel, sizeof(*pstTNRNoiseModelAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetTNRLumaMotionAttr(VI_PIPE ViPipe, const ISP_TNRLumaMotion_ATTR_S *pstTNRLumaMotionAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstTNRLumaMotionAttr);

	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN0, pstTNRLumaMotionAttr->stManual.L2mIn0[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN1, pstTNRLumaMotionAttr->stManual.L2mIn0[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN2, pstTNRLumaMotionAttr->stManual.L2mIn0[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN3, pstTNRLumaMotionAttr->stManual.L2mIn0[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT0, pstTNRLumaMotionAttr->stManual.L2mOut0[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT1, pstTNRLumaMotionAttr->stManual.L2mOut0[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT2, pstTNRLumaMotionAttr->stManual.L2mOut0[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT3, pstTNRLumaMotionAttr->stManual.L2mOut0[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN0, pstTNRLumaMotionAttr->stManual.L2mIn1[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN1, pstTNRLumaMotionAttr->stManual.L2mIn1[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN2, pstTNRLumaMotionAttr->stManual.L2mIn1[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN3, pstTNRLumaMotionAttr->stManual.L2mIn1[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT0, pstTNRLumaMotionAttr->stManual.L2mOut1[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT1, pstTNRLumaMotionAttr->stManual.L2mOut1[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT2, pstTNRLumaMotionAttr->stManual.L2mOut1[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT3, pstTNRLumaMotionAttr->stManual.L2mOut1[3]);

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN0 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn0[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN1 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn0[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN2 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn0[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN3 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn0[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT0 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut0[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT1 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut0[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT2 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut0[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT3 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut0[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN0 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn1[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN1 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn1[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN2 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn1[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN3 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mIn1[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT0 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut1[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT1 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut1[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT2 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut1[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT3 + i,
			      pstTNRLumaMotionAttr->stAuto.L2mOut1[3][i]);
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetTNRLumaMotionAttr(VI_PIPE ViPipe, ISP_TNRLumaMotion_ATTR_S *pstTNRLumaMotionAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstTNRLumaMotionAttr);

	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN0,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn0[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN1,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn0[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN2,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn0[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_IN3,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn0[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT0,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut0[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT1,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut0[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT2,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut0[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_L2M_OUT3,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut0[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN0,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn1[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN1,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn1[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN2,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn1[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_IN3,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mIn1[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT0,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut1[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT1,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut1[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT2,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut1[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_L2M_OUT3,
		      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stManual.L2mOut1[3]));

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN0 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn0[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN1 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn0[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN2 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn0[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_IN3 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn0[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT0 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut0[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT1 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut0[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT2 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut0[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_L2M_OUT3 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut0[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN0 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn1[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN1 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn1[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN2 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn1[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_IN3 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mIn1[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT0 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut1[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT1 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut1[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT2 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut1[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_L2M_OUT3 + i,
			      (void *)(uintptr_t)&(pstTNRLumaMotionAttr->stAuto.L2mOut1[3][i]));
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetTNRGhostAttr(VI_PIPE ViPipe, const ISP_TNRGhost_ATTR_S *pstTNRGhostAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstTNRGhostAttr);

	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN0, pstTNRGhostAttr->stManual.PrtctIn0[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN1, pstTNRGhostAttr->stManual.PrtctIn0[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN2, pstTNRGhostAttr->stManual.PrtctIn0[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN3, pstTNRGhostAttr->stManual.PrtctIn0[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT0, pstTNRGhostAttr->stManual.PrtctOut0[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT1, pstTNRGhostAttr->stManual.PrtctOut0[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT2, pstTNRGhostAttr->stManual.PrtctOut0[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT3, pstTNRGhostAttr->stManual.PrtctOut0[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN0, pstTNRGhostAttr->stManual.PrtctIn1[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN1, pstTNRGhostAttr->stManual.PrtctIn1[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN2, pstTNRGhostAttr->stManual.PrtctIn1[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN3, pstTNRGhostAttr->stManual.PrtctIn1[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_OUT0, pstTNRGhostAttr->stManual.PrtctOut1[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_OUT1, pstTNRGhostAttr->stManual.PrtctOut1[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_CPSE, pstTNRGhostAttr->stManual.PrtctOut1[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_OUT3, pstTNRGhostAttr->stManual.PrtctOut1[3]);

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN0 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn0[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN1 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn0[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN2 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn0[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN3 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn0[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT0 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut0[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT1 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut0[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT2 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut0[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT3 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut0[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN0 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn1[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN1 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn1[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN2 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn1[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN3 + i,
			      pstTNRGhostAttr->stAuto.PrtctIn1[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_OUT0 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut1[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_OUT1 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut1[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_CPSE + i, pstTNRGhostAttr->stAuto.PrtctOut1[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_OUT3 + i,
			      pstTNRGhostAttr->stAuto.PrtctOut1[3][i]);
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetTNRGhostAttr(VI_PIPE ViPipe, ISP_TNRGhost_ATTR_S *pstTNRGhostAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstTNRGhostAttr);

	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN0,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn0[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN1,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn0[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN2,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn0[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_IN3,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn0[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT0,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut0[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT1,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut0[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT2,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut0[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH0_PRTCT_OUT3,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut0[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN0,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn1[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN1,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn1[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN2,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn1[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_IN3,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctIn1[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_OUT0,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut1[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_OUT1,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut1[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_CPSE, (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut1[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_TNR_PATH1_PRTCT_OUT3,
		      (void *)(uintptr_t)&(pstTNRGhostAttr->stManual.PrtctOut1[3]));

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN0 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn0[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN1 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn0[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN2 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn0[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_IN3 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn0[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT0 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut0[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT1 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut0[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT2 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut0[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH0_PRTCT_OUT3 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut0[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN0 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn1[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN1 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn1[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN2 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn1[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_IN3 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctIn1[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_OUT0 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut1[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_OUT1 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut1[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_CPSE + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut1[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_TNR_PATH1_PRTCT_OUT3 + i,
			      (void *)(uintptr_t)&(pstTNRGhostAttr->stAuto.PrtctOut1[3][i]));
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetTNRMtPrtAttr(VI_PIPE ViPipe, const ISP_TNRMtPrt_ATTR_S *pstTNRMtPrtAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstTNRMtPrtAttr);

	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_EN, pstTNRMtPrtAttr->LowMtPrtEn);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_LEVEL, pstTNRMtPrtAttr->stManual.LowMtPrtLevel);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN0, pstTNRMtPrtAttr->stManual.LowMtPrtIn[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN1, pstTNRMtPrtAttr->stManual.LowMtPrtIn[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN2, pstTNRMtPrtAttr->stManual.LowMtPrtIn[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN3, pstTNRMtPrtAttr->stManual.LowMtPrtIn[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT0, pstTNRMtPrtAttr->stManual.LowMtPrtOut[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT1, pstTNRMtPrtAttr->stManual.LowMtPrtOut[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT2, pstTNRMtPrtAttr->stManual.LowMtPrtOut[2]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT3, pstTNRMtPrtAttr->stManual.LowMtPrtOut[3]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_SLOPE0, pstTNRMtPrtAttr->stManual.LowMtPrtSlope0[0]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_SLOPE1, pstTNRMtPrtAttr->stManual.LowMtPrtSlope0[1]);
	isp_param_set(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_SLOPE2, pstTNRMtPrtAttr->stManual.LowMtPrtSlope0[2]);

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_LEVEL + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtLevel[i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN0 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtIn[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN1 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtIn[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN2 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtIn[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN3 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtIn[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT0 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtOut[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT1 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtOut[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT2 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtOut[2][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT3 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtOut[3][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_SLOPE0 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtSlope0[0][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_SLOPE1 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtSlope0[1][i]);
		isp_param_set(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_SLOPE2 + i,
			pstTNRMtPrtAttr->stAuto.LowMtPrtSlope0[2][i]);
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetTNRMtPrtAttr(VI_PIPE ViPipe, ISP_TNRMtPrt_ATTR_S *pstTNRMtPrtAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstTNRMtPrtAttr);

	isp_param_get(ViPipe, ISP_CLUT_HSV3DLUT_ENABLE, (void *)(uintptr_t)&(pstTNRMtPrtAttr->LowMtPrtEn));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_EN, (void *)(uintptr_t)&(pstTNRMtPrtAttr->LowMtPrtEn));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_LEVEL,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtLevel));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN0,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtIn[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN1,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtIn[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN2,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtIn[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_IN3,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtIn[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT0,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtOut[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT1,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtOut[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT2,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtOut[2]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_OUT3,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtOut[3]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_SLOPE0,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtSlope0[0]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_SLOPE1,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtSlope0[1]));
	isp_param_get(ViPipe, ISP_TNR_MANUAL_LOW_MTPRT_SLOPE2,
		(void *)(uintptr_t)&(pstTNRMtPrtAttr->stManual.LowMtPrtSlope0[2]));

	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_LEVEL + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtLevel[i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN0 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtIn[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN1 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtIn[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN2 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtIn[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_IN3 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtIn[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT0 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtOut[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT1 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtOut[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT2 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtOut[2][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_OUT3 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtOut[3][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_SLOPE0 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtSlope0[0][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_SLOPE1 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtSlope0[1][i]));
		isp_param_get(ViPipe, ISP_TNR_AUTO_LOW_MTPRT_SLOPE2 + i,
			(void *)(uintptr_t)&(pstTNRMtPrtAttr->stAuto.LowMtPrtSlope0[2][i]));
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetCLutAttr(VI_PIPE ViPipe, const ISP_CLUT_ATTR_S *pstCLutAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstCLutAttr);

	isp_param_set(ViPipe, ISP_CLUT_HSV3DLUT_ENABLE, pstCLutAttr->hsv3dlut_enable);
	isp_param_set(ViPipe, ISP_CLUT_HSV3DLUT_H_CLAMP_WRAP_OPT, pstCLutAttr->hsv3dlut_h_clamp_wrap_opt);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetCLutAttr(VI_PIPE ViPipe, ISP_CLUT_ATTR_S *pstCLutAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstCLutAttr);

	isp_param_get(ViPipe, ISP_CLUT_HSV3DLUT_ENABLE, (void *)(uintptr_t)&(pstCLutAttr->hsv3dlut_enable));
	isp_param_get(ViPipe, ISP_CLUT_HSV3DLUT_H_CLAMP_WRAP_OPT,
		(void *)(uintptr_t)&(pstCLutAttr->hsv3dlut_h_clamp_wrap_opt));

	return CVI_SUCCESS;
}

static CVI_S32 CVI_ISP_PrintBlackLevelAttr(const ISP_BLACKLEVEL_ATTR_S *pstBlackLevelAttr)
{
	ISP_CHECK_POINTER(pstBlackLevelAttr);

	// non-auto attributes
	PRINT_ATTR(pstBlackLevelAttr->Enable, reg_blc_enable, 0, 1);
	PRINT_ATTR(pstBlackLevelAttr->enOpType, nan, 0, 1);

	// manual attributes
	PRINT_ATTR(pstBlackLevelAttr->stManual.OffsetR, reg_blc_offset_r, 0, 1023);
	PRINT_ATTR(pstBlackLevelAttr->stManual.OffsetGr, reg_blc_offset_gr, 0, 1023);
	PRINT_ATTR(pstBlackLevelAttr->stManual.OffsetGb, reg_blc_offset_gb, 0, 1023);
	PRINT_ATTR(pstBlackLevelAttr->stManual.OffsetB, reg_blc_offset_b, 0, 1023);
	PRINT_ATTR(pstBlackLevelAttr->stManual.GainR, reg_blc_gain_r, 0, 65535);
	PRINT_ATTR(pstBlackLevelAttr->stManual.GainGr, reg_blc_gain_gr, 0, 65535);
	PRINT_ATTR(pstBlackLevelAttr->stManual.GainGb, reg_blc_gain_gb, 0, 65535);
	PRINT_ATTR(pstBlackLevelAttr->stManual.GainB, reg_blc_gain_b, 0, 65535);

	// auto attributes
	for (CVI_U8 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstBlackLevelAttr->stAuto.OffsetR[i], reg_blc_offset_r, 0, 1023);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.OffsetGr[i], reg_blc_offset_gr, 0, 1023);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.OffsetGb[i], reg_blc_offset_gb, 0, 1023);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.OffsetB[i], reg_blc_offset_b, 0, 1023);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.GainR[i], reg_blc_gain_r, 0, 65535);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.GainGr[i], reg_blc_gain_gr, 0, 65535);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.GainGb[i], reg_blc_gain_gb, 0, 65535);
		PRINT_ATTR(pstBlackLevelAttr->stAuto.GainB[i], reg_blc_gain_b, 0, 65535);
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetBlackLevelAttr(VI_PIPE ViPipe, const ISP_BLACKLEVEL_ATTR_S *pstBlackLevelAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstBlackLevelAttr);

	// non-auto attributes
	isp_param_set(ViPipe, ISP_BLACKLEVEL_BLC_ENABLE, pstBlackLevelAttr->Enable);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_BLC_OPTYPE, pstBlackLevelAttr->enOpType);

	// manual attributes
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_R, pstBlackLevelAttr->stManual.OffsetR);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_GR, pstBlackLevelAttr->stManual.OffsetGr);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_GB, pstBlackLevelAttr->stManual.OffsetGb);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_B, pstBlackLevelAttr->stManual.OffsetB);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_R, pstBlackLevelAttr->stManual.GainR);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_GR, pstBlackLevelAttr->stManual.GainGr);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_GB, pstBlackLevelAttr->stManual.GainGb);
	isp_param_set(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_B, pstBlackLevelAttr->stManual.GainB);

	// auto attributes
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_R + i, pstBlackLevelAttr->stAuto.OffsetR[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_GR + i, pstBlackLevelAttr->stAuto.OffsetGr[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_GB + i, pstBlackLevelAttr->stAuto.OffsetGb[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_B + i, pstBlackLevelAttr->stAuto.OffsetB[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_R + i, pstBlackLevelAttr->stAuto.GainR[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_GR + i, pstBlackLevelAttr->stAuto.GainGr[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_GB + i, pstBlackLevelAttr->stAuto.GainGb[i]);
		isp_param_set(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_B + i, pstBlackLevelAttr->stAuto.GainB[i]);
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintBlackLevelAttr(pstBlackLevelAttr);

	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_BLC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetBlackLevelAttr(VI_PIPE ViPipe, ISP_BLACKLEVEL_ATTR_S *pstBlackLevelAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstBlackLevelAttr);

	// non-auto parameters
	CLEAR(*pstBlackLevelAttr);
	isp_param_get(ViPipe, ISP_BLACKLEVEL_BLC_ENABLE, (void *)&(pstBlackLevelAttr->Enable));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_BLC_OPTYPE, (void *)&(pstBlackLevelAttr->enOpType));

	// manual parameters
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_R, (void *)&(pstBlackLevelAttr->stManual.OffsetR));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_GR, (void *)&(pstBlackLevelAttr->stManual.OffsetGr));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_GB, (void *)&(pstBlackLevelAttr->stManual.OffsetGb));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_OFFSET_B, (void *)&(pstBlackLevelAttr->stManual.OffsetB));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_R, (void *)&(pstBlackLevelAttr->stManual.GainR));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_GR, (void *)&(pstBlackLevelAttr->stManual.GainGr));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_GB, (void *)&(pstBlackLevelAttr->stManual.GainGb));
	isp_param_get(ViPipe, ISP_BLACKLEVEL_MANUAL_BLC_GAIN_B, (void *)&(pstBlackLevelAttr->stManual.GainB));

	// auto parameters
	for (CVI_U32 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_R + i
				, (void *)&(pstBlackLevelAttr->stAuto.OffsetR[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_GR + i
				, (void *)&(pstBlackLevelAttr->stAuto.OffsetGr[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_GB + i
				, (void *)&(pstBlackLevelAttr->stAuto.OffsetGb[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_OFFSET_B + i
				, (void *)&(pstBlackLevelAttr->stAuto.OffsetB[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_R + i
				, (void *)&(pstBlackLevelAttr->stAuto.GainR[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_GR + i
				, (void *)&(pstBlackLevelAttr->stAuto.GainGr[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_GB + i
				, (void *)&(pstBlackLevelAttr->stAuto.GainGb[i]));
		isp_param_get(ViPipe, ISP_BLACKLEVEL_AUTO_BLC_GAIN_B + i
				, (void *)&(pstBlackLevelAttr->stAuto.GainB[i]));
	}

	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintBlackLevelAttr(pstBlackLevelAttr);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetColorToneAttr(VI_PIPE ViPipe, const ISP_COLORTONE_ATTR_S *pstColorToneAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstColorToneAttr);

	isp_param_set(ViPipe, ISP_COLORTONE_WBG_ENABLE, pstColorToneAttr->wbg_enable);
	isp_param_set(ViPipe, ISP_COLORTONE_U16REDCASTGAIN, pstColorToneAttr->u16RedCastGain);
	isp_param_set(ViPipe, ISP_COLORTONE_U16GREENCASTGAIN, pstColorToneAttr->u16GreenCastGain);
	isp_param_set(ViPipe, ISP_COLORTONE_U16BLUECASTGAIN, pstColorToneAttr->u16BlueCastGain);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_WBGAIN);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetColorToneAttr(VI_PIPE ViPipe, ISP_COLORTONE_ATTR_S *pstColorToneAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstColorToneAttr);

	isp_param_get(ViPipe, ISP_COLORTONE_WBG_ENABLE, (void *)(uintptr_t)&(pstColorToneAttr->wbg_enable));
	isp_param_get(ViPipe, ISP_COLORTONE_U16REDCASTGAIN, (void *)(uintptr_t)&(pstColorToneAttr->u16RedCastGain));
	isp_param_get(ViPipe, ISP_COLORTONE_U16GREENCASTGAIN, (void *)(uintptr_t)&(pstColorToneAttr->u16GreenCastGain));
	isp_param_get(ViPipe, ISP_COLORTONE_U16BLUECASTGAIN, (void *)(uintptr_t)&(pstColorToneAttr->u16BlueCastGain));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetSaturationAttr(VI_PIPE ViPipe, const ISP_Saturation_ATTR_S *pstSaturationAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstSaturationAttr);

	memcpy(&g_param[ViPipe].Saturation, pstSaturationAttr, sizeof(*pstSaturationAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_CCM);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetSaturationAttr(VI_PIPE ViPipe, ISP_Saturation_ATTR_S *pstSaturationAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstSaturationAttr);

	memcpy(pstSaturationAttr, &g_param[ViPipe].Saturation, sizeof(*pstSaturationAttr));

	return CVI_SUCCESS;
}

static void CVI_ISP_PrintCCMAttr(const ISP_CCM_ATTR_S *pstCCMAttr)
{
	CVI_U8 table = 0;

	// non-auto attributes
	PRINT_ATTR(pstCCMAttr->Enable, reg_ccm_enable, 0, 1);
	PRINT_ATTR(pstCCMAttr->enOpType, reg_ccm_enable, 0, 1);

	PRINT_ATTR(pstCCMAttr->stManual.SatEnable, reg_na, 0, 1);

	PRINT_ATTR(pstCCMAttr->stAuto.ISOActEnable, reg_na, 0, 1);
	PRINT_ATTR(pstCCMAttr->stAuto.TempActEnable, reg_na, 0, 1);
	//high to low, (D50, TL84, A) or (10K, D65, D50, TL84, A)
	// INIT_ATTR(pstCCMAttr->stAuto.CCMTabNum, 0, 7);
	PRINT_ATTR(pstCCMAttr->stAuto.CCMTabNum, reg_na, 3, 3);
	PRINT_ATTR(pstCCMAttr->stAuto.CCMTab[0].ColorTemp, reg_na, 5000, 5000);	//D50
	PRINT_ATTR(pstCCMAttr->stAuto.CCMTab[1].ColorTemp, reg_na, 4100, 4100);	//TL84
	PRINT_ATTR(pstCCMAttr->stAuto.CCMTab[2].ColorTemp, reg_na, 2800, 2800);;	//A
	for (table = 0; table < pstCCMAttr->stAuto.CCMTabNum; table++) {
		for (CVI_U8 cell = 0; cell < 9; cell++)
			PRINT_ATTR(pstCCMAttr->stAuto.CCMTab[table].CCM[cell], reg_na, 0, (rand() % 1025) / 3);
	}

	PRINT_ATTR(pstCCMAttr->stManual.SatEnable, reg_na, 0, 1);
	for (CVI_U8 cell = 0; cell < 9; cell++)
		PRINT_ATTR(pstCCMAttr->stManual.CCM[cell], reg_ccm_xx, 0, 256);

	// manual attributes

	// auto attributes
}

CVI_S32 CVI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_CCM_ATTR_S *pstCCMAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstCCMAttr);

	memcpy(&g_param[ViPipe].CMM, pstCCMAttr, sizeof(*pstCCMAttr));
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintCCMAttr(pstCCMAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_CCM);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_CCM_ATTR_S *pstCCMAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstCCMAttr);

	memcpy(pstCCMAttr, &g_param[ViPipe].CMM, sizeof(*pstCCMAttr));
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintCCMAttr(pstCCMAttr);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetHSVAttr(VI_PIPE ViPipe, const ISP_HSV_ATTR_S *pstHSVAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstHSVAttr);

	memcpy(&g_param[ViPipe].HSV, pstHSVAttr, sizeof(*pstHSVAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_HSV);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetHSVAttr(VI_PIPE ViPipe, ISP_HSV_ATTR_S *pstHSVAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstHSVAttr);

	memcpy(pstHSVAttr, &g_param[ViPipe].HSV, sizeof(*pstHSVAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetDPDynamicAttr(VI_PIPE ViPipe, const ISP_DPDynamic_ATTR_S *pstDPCDynamicAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDPCDynamicAttr);

	memcpy(&g_param[ViPipe].DPDynamic, pstDPCDynamicAttr, sizeof(*pstDPCDynamicAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DPC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDPDynamicAttr(VI_PIPE ViPipe, ISP_DPDynamic_ATTR_S *pstDPCDynamicAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDPCDynamicAttr);

	memcpy(pstDPCDynamicAttr, &g_param[ViPipe].DPDynamic, sizeof(*pstDPCDynamicAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetDPStaticAttr(VI_PIPE ViPipe, const ISP_DPStatic_ATTR_S *pstDPStaticAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDPStaticAttr);

	memcpy(&g_param[ViPipe].DPStatic, pstDPStaticAttr, sizeof(*pstDPStaticAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDPStaticAttr(VI_PIPE ViPipe, ISP_DPStatic_ATTR_S *pstDPStaticAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDPStaticAttr);

	memcpy(pstDPStaticAttr, &g_param[ViPipe].DPStatic, sizeof(*pstDPStaticAttr));

	return CVI_SUCCESS;
}

static CVI_VOID crop_image(VI_PIPE ViPipe, CVI_U8 *src, CVI_U8 *dst)
{
	ISP_SNS_SYNC_INFO_S snsCropInfo;

	if ((src == NULL) || (dst == NULL)) {
		ISP_DEBUG(LOG_ERR, "input error src(%p), dst(%p)\n", src, dst);
	}

	isp_sensor_get_crop_info(ViPipe, &snsCropInfo);

	// Two pixel packed with three bytes in raw12 format

	// ofs_x = pixel
	CVI_U32 ofs_x = ((snsCropInfo.ispCfg.img_size[0].stWndRect.s32X * 3) >> 1);
	// ofs_y = line.
	CVI_U32 ofs_y = snsCropInfo.ispCfg.img_size[0].stWndRect.s32Y;
	CVI_U32 img_w = ((snsCropInfo.ispCfg.img_size[0].stWndRect.u32Width * 3) >> 1);
	CVI_U32 img_h = snsCropInfo.ispCfg.img_size[0].stWndRect.u32Height;
	CVI_U32 sns_w = ((snsCropInfo.ispCfg.img_size[0].stSnsSize.u32Width * 3) >> 1);
	CVI_U32 sns_h = snsCropInfo.ispCfg.img_size[0].stSnsSize.u32Height;

	ISP_DEBUG(LOG_DEBUG, "ofs_x/y(%d, %d), img_w/h(%d, %d), sns_w/h(%d, %d)\n",
		ofs_x, ofs_y, img_w, img_h, sns_w, sns_h);

	CVI_U8 *addr_src = src + ofs_y * sns_w + ofs_x;
	CVI_U8 *addr_dst = dst;

	for (CVI_U32 i = 0 ; i < img_h ; i++) {
		memcpy(addr_dst, addr_src, img_w);
		addr_src += sns_w;
		addr_dst += img_w;
	}
}

CVI_S32 ISP_GetRawBuffer(VI_PIPE ViPipe, CVI_U8 *bayerBuffer
	, BAYER_FORMAT_E *bayerFormat, CVI_BOOL saveFileEn)
{
	VIDEO_FRAME_INFO_S stVideoFrame;
	VI_DUMP_ATTR_S attr;

	memset(&stVideoFrame, 0, sizeof(stVideoFrame));

	stVideoFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;

	// Set Attr
	attr.bEnable = 1;
	attr.u32Depth = 0;
	attr.enDumpType = VI_DUMP_TYPE_RAW;
	CVI_VI_SetPipeDumpAttr(ViPipe, &attr);

	// Check Attr effective
	attr.bEnable = 0;
	attr.enDumpType = VI_DUMP_TYPE_IR;
	CVI_VI_GetPipeDumpAttr(ViPipe, &attr);

	if ((attr.bEnable != 1) || (attr.enDumpType != VI_DUMP_TYPE_RAW)) {
		ISP_DEBUG(LOG_ERR, "Enable(%d), DumpType(%d)\n", attr.bEnable, attr.enDumpType);
	}

	CVI_VI_GetPipeFrame(ViPipe, &stVideoFrame, 100);

	size_t image_size = stVideoFrame.stVFrame.u32Length[0];

	if (attr.enDumpType == VI_DUMP_TYPE_RAW) {
		stVideoFrame.stVFrame.pu8VirAddr[0]
			= CVI_SYS_Mmap(stVideoFrame.stVFrame.u64PhyAddr[0], image_size);
		ISP_DEBUG(LOG_DEBUG, "paddr(0x%llx) vaddr(0x%llx)\n",
			stVideoFrame.stVFrame.u64PhyAddr[0], stVideoFrame.stVFrame.pu8VirAddr[0]);

		unsigned char *ptr = malloc(image_size);

		memcpy(ptr, (const void *)stVideoFrame.stVFrame.pu8VirAddr[0], image_size);
		crop_image(ViPipe, ptr, bayerBuffer);

		CVI_SYS_Munmap((void *)stVideoFrame.stVFrame.pu8VirAddr[0], image_size);

		if (saveFileEn) {
			char img_name[128] = {0,}, order_id[8] = {0,};
			FILE *output;
			struct timeval tv1;

			switch (stVideoFrame.stVFrame.enBayerFormat) {
			case BAYER_FORMAT_BG:
				snprintf(order_id, sizeof(order_id), "BG");
				break;
			case BAYER_FORMAT_GB:
				snprintf(order_id, sizeof(order_id), "GB");
				break;
			case BAYER_FORMAT_GR:
				snprintf(order_id, sizeof(order_id), "GR");
				break;
			case BAYER_FORMAT_RG:
				snprintf(order_id, sizeof(order_id), "RG");
				break;
			}

			gettimeofday(&tv1, NULL);

			snprintf(img_name, sizeof(img_name), "./vi_%d_byrid_%s_w_%d_h_%d_tv_%d_%d.raw",
					ViPipe, order_id,
					stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height,
					tv1.tv_sec, tv1.tv_usec);

			ISP_DEBUG(LOG_DEBUG, "dump image %s\n", img_name);

			output = fopen(img_name, "wb");

			fwrite(ptr, image_size, 1, output);
			fclose(output);
		}
		free(ptr);
	}

	CVI_VI_ReleasePipeFrame(ViPipe, &stVideoFrame);

	return 0;
}

CVI_S32 ISP_SetDPCalibrate(VI_PIPE ViPipe, const ISP_DPCalib_ATTR_S *pstDPCalibAttr)
{
	ISP_CTX_S *pstIspCtx = NULL;

	ISP_CHECK_PIPE(ViPipe);
	ISP_GET_CTX(ViPipe, pstIspCtx);

	ISP_DPCalib_ATTR_S attr = *pstDPCalibAttr;
	CVI_U16 width = pstIspCtx->stSnsImageMode.u16Width;
	CVI_U16 height = pstIspCtx->stSnsImageMode.u16Height;
	CVI_U8 *bayerBuffer = calloc(1, width * height * 3 / 2);	//12bit
	CVI_U16 *detected = calloc(1, sizeof(CVI_U16) * width * height);

	if ((!bayerBuffer) || (!detected)) {
		ISP_DEBUG(LOG_ERR, "Calloc failed, bayerBuffer(%p), detected(%d)\n",
			bayerBuffer, detected);
	}

	BAYER_FORMAT_E bayerFormat;
	CVI_U32 bad_type = NORMAL_PIXEL;

	CVI_U32 reg_image_cnt = 0;

	ISP_DEBUG(LOG_DEBUG, "width(%u), height(%d)\n", width, height);

	if (attr.EnableDetect) {

		// Calibrate by `StaticDPType`
		// until `TimeLimit`
		time_t endTime = time(0) + attr.TimeLimit;
		time_t currTime = time(0);

		// TODO@Kidd get noise level from mpi
		// TODO@Kidd move to isp_test.c
		if (attr.StaticDPType == ISP_STATIC_DP_BRIGHT) {
			attr.StartThresh = (WHITE_LVL - SD_NUM * NOISE_LVL);
			bad_type = WHITE_BAD_PIXEL;
		} else {
			attr.StartThresh = (BLACK_LVL + SD_NUM * NOISE_LVL);
			bad_type = BLACK_BAD_PIXEL;
		}

		// Detect
		do  {
			// get frame
			ISP_GetRawBuffer(ViPipe, bayerBuffer, &bayerFormat, attr.saveFileEn);

			// detect bad pixel
			Bad_Pixel_Detection(bayerBuffer, width, height, detected, attr.StartThresh, attr.StaticDPType);

			// update threshold
			attr.FinishThresh = attr.StartThresh;

			// next loop
			reg_image_cnt++;
			currTime = time(0);
		} while (currTime < endTime);

		// screening the detection result
		// TODO@Kidd sorting by Confidence, select first #STATIC_DP_COUNT_MAX and write to `Table`
		attr.Count = 0;
		for (CVI_U32 i = 0; i < height; i++) {
			for (CVI_U32 j = 0; j < width; j++) {
				// 90% are detected as bad, than this pixel is bad
				CVI_U16 idx = i * width + j;

				if (detected[idx] > (reg_image_cnt * BAD_RATIO)) {

					detected[idx] = bad_type;
					if (attr.Count > attr.CountMax) {
						ISP_DEBUG(LOG_ERR, "Bad pixel count overflow\n");
						attr.Status = ISP_STATUS_TIMEOUT;
						goto ERROR;
					}
					attr.Table[attr.Count] = ((i << 16) + j);
					attr.Count++;
				} else {
					detected[idx] = NORMAL_PIXEL;
				}
			}
		}
	} // end of detect

ERROR:
	memcpy(&g_param[ViPipe].DPCalib, &attr, sizeof(attr));
	free(detected);
	free(bayerBuffer);

	return 0;
}

#define PARAL_DPC
#ifdef PARAL_DPC
pthread_mutex_t mutex_dpc[VI_MAX_PIPE_NUM];
pthread_t dpcThread[VI_MAX_PIPE_NUM];
#endif

typedef struct {
	VI_PIPE ViPipe;
	const ISP_DPCalib_ATTR_S pstDPCalibAttr;
} ISP_Calibrate;
ISP_Calibrate calibrate[VI_MAX_PIPE_NUM];
void *ISP_DPCalibrateThread(void *param)
{
	ISP_Calibrate *calibrate = (ISP_Calibrate *)param;
	VI_PIPE ViPipe = calibrate->ViPipe;
	const ISP_DPCalib_ATTR_S *pstDPCalibAttr = &(calibrate->pstDPCalibAttr);

#ifdef PARAL_DPC
	pthread_detach(pthread_self());
	pthread_mutex_lock(&mutex_dpc[ViPipe]);
#endif

	ISP_SetDPCalibrate(ViPipe, pstDPCalibAttr);

#ifdef PARAL_DPC
	pthread_mutex_unlock(&mutex_dpc[ViPipe]);
	pthread_exit(NULL);
#endif
	return 0;
}

CVI_S32 CVI_ISP_SetDPCalibrate(VI_PIPE ViPipe, const ISP_DPCalib_ATTR_S *pstDPCalibAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDPCalibAttr);

	calibrate[ViPipe].ViPipe = ViPipe;
	memcpy(&(calibrate[ViPipe].pstDPCalibAttr), pstDPCalibAttr, sizeof(ISP_DPCalib_ATTR_S));

#ifdef PARAL_DPC
	struct sched_param param;
	pthread_attr_t attr;

	param.sched_priority = 80;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setschedparam(&attr, &param);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_create(&dpcThread[ViPipe], &attr, ISP_DPCalibrateThread, (void *)&calibrate[ViPipe]);
#else
	ISP_DPCalibrateThread(&calibrate[ViPipe]);
#endif

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDPCalibrate(VI_PIPE ViPipe, ISP_DPCalib_ATTR_S *pstDPCalibAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstDPCalibAttr);

#ifdef PARAL_DPC
	pthread_mutex_lock(&mutex_dpc[ViPipe]);
#endif

	memcpy(pstDPCalibAttr, &g_param[ViPipe].DPCalib, sizeof(*pstDPCalibAttr));

#ifdef PARAL_DPC
	pthread_mutex_unlock(&mutex_dpc[ViPipe]);
#endif

	return CVI_SUCCESS;
}

#define ISP_DIFF_VALUE(ATTR) do {\
	diff = attr1->ATTR != attr2->ATTR;\
	if (!diffOnly || diff) {\
		if (diff)\
			printf("\033[0;32;31m");\
		printf("%s %d, %d\n", #ATTR, attr1->ATTR, attr2->ATTR);\
		if (diff)\
			printf("\033[m");\
	} \
} while (0)

#define ISP_DIFF_ARRAY(ATTR, fmt, size) do {\
	for (CVI_U32 i; i < (size); i++) {\
		diff = attr1->ATTR[i] != attr2->ATTR[i];\
		if (!diffOnly || diff) {\
			if (diff)\
				ISP_DEBUG(LOG_DEBUG, "\033[0;32;31m");\
			ISP_DEBUG(LOG_DEBUG, "%s[%u] " fmt ", " fmt "\n", #ATTR, i, attr1->ATTR[i], attr2->ATTR[i]);\
			if (diff)\
				ISP_DEBUG(LOG_DEBUG, "\033[m");\
		} \
	} \
} while (0)

#define ISP_DEBUG_PRINT_DIFF(ATTR) do {\
	if (g_isp_debug_print_mpi) {\
		typeof(pst##ATTR) old;\
		CVI_ISP_Print##ATTR(ViPipe,  &old, pst##ATTR, g_isp_debug_diff_only);\
	} \
} while (0)

CVI_S32 CVI_ISP_PrintCrosstalkAttr(VI_PIPE ViPipe, const ISP_Crosstalk_ATTR_S *attr1,
				       const ISP_Crosstalk_ATTR_S *attr2, CVI_BOOL diffOnly)
{
	CVI_BOOL diff;

	// non-auto
	ISP_DIFF_VALUE(Enable);
	ISP_DIFF_ARRAY(GrGbDiffThreSec, "%u", 4);
	ISP_DIFF_ARRAY(FlatThre, "%u", 4);
	ISP_DIFF_VALUE(enOpType);

	// manual
	ISP_DIFF_VALUE(stManual.Strength);

	// auto
	ISP_DIFF_ARRAY(stAuto.Strength, "%u", ISP_AUTO_ISO_STRENGTH_NUM);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetCrosstalkAttr(VI_PIPE ViPipe, const ISP_Crosstalk_ATTR_S *pstCrosstalkAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstCrosstalkAttr);

	// ISP_DEBUG_PRINT_DIFF(CrosstalkAttr);

	memcpy(&g_param[ViPipe].Crosstalk, pstCrosstalkAttr, sizeof(*pstCrosstalkAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_CROSSTALK);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetCrosstalkAttr(VI_PIPE ViPipe, ISP_Crosstalk_ATTR_S *pstCrosstalkAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(pstCrosstalkAttr);

	memcpy(pstCrosstalkAttr, &g_param[ViPipe].Crosstalk, sizeof(*pstCrosstalkAttr));

	return CVI_SUCCESS;
}

//-----------------------------------------------------------------------------
//	FSWDR
//-----------------------------------------------------------------------------
static CVI_S32 CVI_ISP_PrintFSWDRAttr(const ISP_FSWDR_ATTR_S *pstFSWDRAttr)
{
	ISP_CHECK_POINTER(pstFSWDRAttr);

	// non-auto attributes
	PRINT_ATTR(pstFSWDRAttr->Enable, reg_fs_enable, 0, 1);
	PRINT_ATTR(pstFSWDRAttr->MotionCompEnable, reg_fs_mc_enable, 0, 1);
	PRINT_ATTR(pstFSWDRAttr->TuningMode, reg_fs_out_sel, 0, 3);

	// manual attributes
	PRINT_ATTR(pstFSWDRAttr->stManual.MergeMode, reg_mmap_mrg_mode, 0, 1);
	PRINT_ATTR(pstFSWDRAttr->stManual.MergeModeAlpha, reg_mmap_mrg_alph, 0, 1);
	PRINT_ATTR(pstFSWDRAttr->stManual.WDRCombineShortThr, reg_fs_luma_thd_l, 0, 4095);
	PRINT_ATTR(pstFSWDRAttr->stManual.WDRCombineLongThr, reg_fs_luma_thd_h, 0, 4095);
	PRINT_ATTR(pstFSWDRAttr->stManual.WDRCombineMinWeight, reg_fs_wgt_min, 0, 255);
	PRINT_ATTR(pstFSWDRAttr->stManual.WDRCombineMaxWeight, reg_fs_wgt_max, 0, 255);

	// auto attributes
	for (CVI_U8 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstFSWDRAttr->stAuto.MergeMode[i], reg_mmap_mrg_mode, 0, 1);
		PRINT_ATTR(pstFSWDRAttr->stAuto.MergeModeAlpha[i], reg_mmap_mrg_alph, 0, 1);
		PRINT_ATTR(pstFSWDRAttr->stAuto.WDRCombineShortThr[i], reg_fs_luma_thd_l, 0, 4095);
		PRINT_ATTR(pstFSWDRAttr->stAuto.WDRCombineLongThr[i], reg_fs_luma_thd_h, 0, 4095);
		PRINT_ATTR(pstFSWDRAttr->stAuto.WDRCombineMinWeight[i], reg_fs_wgt_min, 0, 255);
		PRINT_ATTR(pstFSWDRAttr->stAuto.WDRCombineMaxWeight[i], reg_fs_wgt_max, 0, 255);
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetFSWDRAttr(VI_PIPE ViPipe, const ISP_FSWDR_ATTR_S *pstFSWDRAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstFSWDRAttr);

	memcpy(&g_param[ViPipe].FSWDR, pstFSWDRAttr, sizeof(*pstFSWDRAttr));
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintFSWDRAttr(pstFSWDRAttr);
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_FUSION);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetFSWDRAttr(VI_PIPE ViPipe, ISP_FSWDR_ATTR_S *pstFSWDRAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstFSWDRAttr);

	memcpy(pstFSWDRAttr, &g_param[ViPipe].FSWDR, sizeof(*pstFSWDRAttr));
	ISP_DEBUG(LOG_DEBUG, "\n");
	CVI_ISP_PrintFSWDRAttr(pstFSWDRAttr);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_PrintWDRExposureAttr(const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExposureAttr)
{
	ISP_CHECK_POINTER(pstWDRExposureAttr);

	// non-auto attributes
	PRINT_ATTR(pstWDRExposureAttr->au32ExpRatio[0], reg_fs_ls_gain, 64, 16383);

	// manual attributes

	// auto attributes
	//for (CVI_U8 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
	//}

	return 0;
}


//-----------------------------------------------------------------------------
//	DRC
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_PrintDRCAttr(const ISP_DRC_ATTR_S *pstDRCAttr)
{
	ISP_CHECK_POINTER(pstDRCAttr);

	// non-auto attributes
	PRINT_ATTR(pstDRCAttr->Enable, reg_ltm_enable, 0, 1);
	PRINT_ATTR(pstDRCAttr->NLDetailEnhanceEn, reg_ltm_dark_lce_en, 0, 1);
	PRINT_ATTR(pstDRCAttr->TuningMode, reg_ltm_dbg_enable, 0, 4);
	PRINT_ATTR(pstDRCAttr->ToneCurveSelect, reg_ltm_deflt_lut[769], 0, 1);
	for (CVI_U32 i = 0; i < 769; i++)
		PRINT_ATTR(pstDRCAttr->CurveUserDefine[i], reg_na, 0, 4096);

	PRINT_ATTR(pstDRCAttr->DRangeFltScale, (reg_ltm_de_rng, reg_ltm_be_rng), 0, 15);
	PRINT_ATTR(pstDRCAttr->DRangeFltCoef, (reg_ltm_de_dist_wgt[11], reg_ltm_be_dist_wgt[11]), 0, 31);
	PRINT_ATTR(pstDRCAttr->SpatialFltScale, (reg_ltm_lmap0_lp_rng, reg_ltm_lmap1_lp_rng), 0, 15);
	PRINT_ATTR(pstDRCAttr->SpatialFltCoef, (reg_ltm_lmap0_lp_diff_wgt[30], reg_ltm_lmap1_lp_diff_wgt[30]), 0, 31);

	// manual attributes
	PRINT_ATTR(pstDRCAttr->stManual.NLDetailDarkGain[4], reg_ltm_dar_lce_gain_0, 0, 4095);
	PRINT_ATTR(pstDRCAttr->stManual.NLDetailDarkAmplitude[4], reg_ltm_dar_lce_diff_thd_0, 0, 104875);
	PRINT_ATTR(pstDRCAttr->stManual.NLDetailBrightGain[4], reg_ltm_bri_lce_up_gain_0, 0, 4095);
	PRINT_ATTR(pstDRCAttr->stManual.NLDetailBrightAmplitude[4], reg_ltm_bri_lce_up_thd_0, 0, 104875);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastDarkMinThrd, reg_ltm_dar_in_thd_l, 0, 4095);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastDarkMaxThrd, reg_ltm_dar_in_thd_h, 0, 4095);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastDarkMinWeight, reg_ltm_dar_out_thd_l, 0, 255);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastDarkMaxWeight, reg_ltm_dar_out_thd_h, 0, 255);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastBrightMinThrd, reg_ltm_bri_in_thd_l, 0, 4095);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastBrightMaxThrd, reg_ltm_bri_in_thd_h, 0, 4095);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastBrightMinWeight, reg_ltm_bri_out_thd_l, 0, 255);
	PRINT_ATTR(pstDRCAttr->stManual.ContrastBrightMaxWeight, reg_ltm_bri_out_thd_h, 0, 255);

	// auto attributes
	for (CVI_U8 i = 0; i < ISP_AUTO_ISO_STRENGTH_NUM; i++) {
		PRINT_ATTR(pstDRCAttr->stAuto.NLDetailDarkGain[4][i], reg_ltm_dar_lce_gain_0, 0, 4095);
		PRINT_ATTR(pstDRCAttr->stAuto.NLDetailDarkAmplitude[4][i], reg_ltm_dar_lce_diff_thd_0, 0, 104875);
		PRINT_ATTR(pstDRCAttr->stAuto.NLDetailBrightGain[4][i], reg_ltm_bri_lce_up_gain_0, 0, 4095);
		PRINT_ATTR(pstDRCAttr->stAuto.NLDetailBrightAmplitude[4][i], reg_ltm_bri_lce_up_thd_0, 0, 104875);
	}

	for (CVI_U8 i = 0; i < ISP_AUTO_LV_NUM; i++) {
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastDarkMinThrd, reg_ltm_dar_in_thd_l, 0, 4095);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastDarkMaxThrd, reg_ltm_dar_in_thd_h, 0, 4095);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastDarkMinWeight, reg_ltm_dar_out_thd_l, 0, 255);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastDarkMaxWeight, reg_ltm_dar_out_thd_h, 0, 255);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastBrightMinThrd, reg_ltm_bri_in_thd_l, 0, 4095);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastBrightMaxThrd, reg_ltm_bri_in_thd_h, 0, 4095);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastBrightMinWeight, reg_ltm_bri_out_thd_l, 0, 255);
		PRINT_ATTR(pstDRCAttr->stAuto.ContrastBrightMaxWeight, reg_ltm_bri_out_thd_h, 0, 255);
	}

	return 0;
}

CVI_S32 CVI_ISP_SetDRCAttr(VI_PIPE ViPipe, const ISP_DRC_ATTR_S *pstDRCAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstDRCAttr);

	memcpy(&g_param[ViPipe].DRC, pstDRCAttr, sizeof(*pstDRCAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_DRC);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetDRCAttr(VI_PIPE ViPipe, ISP_DRC_ATTR_S *pstDRCAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstDRCAttr);

	memcpy(pstDRCAttr, &g_param[ViPipe].DRC, sizeof(*pstDRCAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_SetMonoAttr(VI_PIPE ViPipe, const ISP_MONO_ATTR_S *pstMonoAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstMonoAttr);

	memcpy(&g_param[ViPipe].mono, pstMonoAttr, sizeof(*pstMonoAttr));
	isp_iq_invalid_set(ViPipe, ISP_IQ_BLOCK_MONO);

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetMonoAttr(VI_PIPE ViPipe, ISP_MONO_ATTR_S *pstMonoAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstMonoAttr);

	memcpy(pstMonoAttr, &g_param[ViPipe].mono, sizeof(*pstMonoAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_PrintNoiseProfileAttr(const ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr)
{
	ISP_CHECK_POINTER(pstNoiseProfileAttr);

	for (CVI_U32 i = 0; i < NOISE_PROFILE_ISO_NUM; i++) {
		for (CVI_U32 j = 0; j < NOISE_PROFILE_CHANNEL_NUM; j++) {
			for (CVI_U32 k = 0; k < NOISE_PROFILE_LEVEL_NUM; k++) {
				PRINT_ATTR(pstNoiseProfileAttr->CalibrationCoef[i][j][k], reg_na, 0, 4096);
			}
		}
	}

	return 0;
}

CVI_S32 CVI_ISP_SetNoiseProfileAttr(VI_PIPE ViPipe, const ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr)
{
	ISP_DEBUG(LOG_INFO, "+\n");
	ISP_CHECK_POINTER(pstNoiseProfileAttr);

	memcpy(&g_param[ViPipe].np, pstNoiseProfileAttr, sizeof(*pstNoiseProfileAttr));

	return CVI_SUCCESS;
}

CVI_S32 CVI_ISP_GetNoiseProfileAttr(VI_PIPE ViPipe, ISP_CMOS_NOISE_CALIBRATION_S *pstNoiseProfileAttr)
{
	ISP_DEBUG(LOG_DEBUG, "+\n");
	ISP_CHECK_POINTER(pstNoiseProfileAttr);

	memcpy(pstNoiseProfileAttr, &g_param[ViPipe].np, sizeof(*pstNoiseProfileAttr));

	return CVI_SUCCESS;
}

//-----------------------------------------------------------------------------
//	tuning
//-----------------------------------------------------------------------------
CVI_S32 CVI_ISP_GetTuningAttrSize(VI_PIPE ViPipe, CVI_U32 *Size)
{
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(Size);

	return sizeof(g_param[ViPipe]);
}

CVI_S32 CVI_ISP_SetTuningAttr(VI_PIPE ViPipe, const CVI_U8 *TuningParamBuffer)
{
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(TuningParamBuffer);

	memcpy(&g_param[ViPipe], TuningParamBuffer, sizeof(g_param[ViPipe]));
	isp_iq_invalid_set_all(ViPipe);

	return 0;
}

CVI_S32 CVI_ISP_GetTuningAttr(VI_PIPE ViPipe, CVI_U8 *TuningParamBuffer)
{
	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(TuningParamBuffer);

	memcpy(TuningParamBuffer, &g_param[ViPipe], sizeof(g_param[ViPipe]));
	return 0;
}
#endif

#define MIPI_DEV_NODE "cvi-mipi-rx"
#define MIPI_CHECK_OPEN(s32Ret) do {\
	if (fd_mipi < 0) {\
		s32Ret = mipi_open_dev(); \
		if (s32Ret != CVI_SUCCESS) \
			return s32Ret; \
	} \
} while (0)

#define VI_MAX_DEV_NUM            2
#define ISP_CHECK_PIPE(pipe)

CVI_S32 fd_mipi = -1;

CVI_S32 mipi_open_dev(CVI_VOID)
{
	fd_mipi = open(MIPI_DEV_NODE, 0);
	if (fd_mipi < 0) {
		return CVI_FAILURE;
	}

	return CVI_SUCCESS;
}

CVI_S32 CVI_MIPI_SetMipiReset(CVI_S32 devno, CVI_U32 reset)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32Devno = devno;

	MIPI_CHECK_OPEN(s32Ret);
//	printf("Func %s %d %d fd_mipi %d\n", __func__, devno, reset, fd_mipi);
	if (reset == 0) {
		if (ioctl(fd_mipi, CVI_MIPI_UNRESET_MIPI, (uintptr_t)&s32Devno) < 0) {
			//ISP_DEBUG(LOG_ERR, "CVI_MIPI_UNRESET_MIPI - %d NG\n", s32Devno);
			return errno;
		}
	} else {
		if (ioctl(fd_mipi, CVI_MIPI_RESET_MIPI, (uintptr_t)&s32Devno) < 0)	{
			//ISP_DEBUG(LOG_ERR, "CVI_MIPI_RESET_MIPI - %d NG\n", s32Devno);
			return errno;
		}
	}
	return s32Ret;
}

CVI_S32 CVI_MIPI_SetSensorClock(CVI_S32 devno, CVI_U32 enable)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32Devno = devno;

	MIPI_CHECK_OPEN(s32Ret);
	if (enable == 0) {
		if (ioctl(fd_mipi, CVI_MIPI_DISABLE_SENSOR_CLOCK, (uintptr_t)&s32Devno) < 0) {
			//ISP_DEBUG(LOG_ERR, "CVI_MIPI_DISABLE_SENSOR_CLOCK - %d NG\n", s32Devno);
			return errno;
		}
	} else {
		if (ioctl(fd_mipi, CVI_MIPI_ENABLE_SENSOR_CLOCK, (uintptr_t)&s32Devno) < 0) {
			//ISP_DEBUG(LOG_ERR, "CVI_MIPI_ENABLE_SENSOR_CLOCK - %d NG\n", s32Devno);
			return errno;
		}
	}
	return s32Ret;
}

CVI_S32 CVI_MIPI_SetSensorReset(CVI_S32 devno, CVI_U32 reset)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32Devno = devno;

	MIPI_CHECK_OPEN(s32Ret);
//	printf("Func %s %d %d fd_mipi %d\n", __func__, devno, reset, fd_mipi);
	if (reset == 0) {
		if (ioctl(fd_mipi, CVI_MIPI_UNRESET_SENSOR, (uintptr_t)&s32Devno) < 0)	{
			//ISP_DEBUG(LOG_ERR, "CVI_MIPI_DISABLE_SENSOR_CLOCK - %d NG\n", s32Devno);
			return errno;
		}
	} else {
		if (ioctl(fd_mipi, CVI_MIPI_RESET_SENSOR, (uintptr_t)&s32Devno) < 0) {
			//ISP_DEBUG(LOG_ERR, "CVI_MIPI_RESET_SENSOR - %d NG\n", s32Devno);
			return errno;
		}
	}
	return s32Ret;
}

CVI_S32 CVI_MIPI_SetMipiAttr(CVI_S32 ViPipe, const CVI_VOID *devAttr)
{
	CVI_S32 s32Ret = 0;
	SNS_COMBO_DEV_ATTR_S *comboAttr;

	ISP_CHECK_PIPE(ViPipe);
	ISP_CHECK_POINTER(devAttr);
	MIPI_CHECK_OPEN(s32Ret);
	comboAttr = (SNS_COMBO_DEV_ATTR_S *)devAttr;
	if (ioctl(fd_mipi, CVI_MIPI_SET_DEV_ATTR, (uintptr_t)comboAttr) < 0) {
		//ISP_DEBUG(LOG_ERR, "CVI_MIPI_SET_DEV_ATTR NG\n");
		return errno;
	}
	return s32Ret;
}

CVI_S32 CVI_MIPI_SetClkEdge(CVI_S32 devno, CVI_U32 is_up)
{
	CVI_S32 s32Ret = CVI_SUCCESS;
	CVI_S32 s32Devno = devno;
	struct clk_edge_s clk;

	MIPI_CHECK_OPEN(s32Ret);
	clk.devno = devno;
	clk.edge = is_up ? CLK_UP_EDGE : CLK_DOWN_EDGE;
//	ISP_DEBUG(LOG_DEBUG, "%d %d fd_mipi %d\n", devno, reset, fd_mipi);
	if (ioctl(fd_mipi, CVI_MIPI_SET_OUTPUT_CLK_EDGE, (uintptr_t)&clk) < 0)	{
		//ISP_DEBUG(LOG_ERR, "CVI_MIPI_SET_OUTPUT_CLK_EDGE, - %d NG\n", s32Devno);
		return errno;
	}
	return s32Ret;
}
