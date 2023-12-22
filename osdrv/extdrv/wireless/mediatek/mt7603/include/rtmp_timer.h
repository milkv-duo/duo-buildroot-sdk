/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2008, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
	rtmp_timer.h

    Abstract:
	Ralink Wireless Driver timer related data structures and delcarations
	
    Revision History:
	Who           When                What
	--------    ----------      ----------------------------------------------
	Name          Date                 Modification logs
	Shiang Tu    Aug-28-2008	init version
	
*/

#ifndef __RTMP_TIMER_H__
#define  __RTMP_TIMER_H__

#include "rtmp_os.h"

#ifdef __ECOS
#define DECLARE_TIMER_FUNCTION(_func)			\
	void rtmp_timer_##_func(cyg_handle_t alarm, cyg_addrword_t data)
#else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0))
#define DECLARE_TIMER_FUNCTION(_func)			\
	void rtmp_timer_##_func(OS_NDIS_MINIPORT_TIMER *timer)
#else
#define DECLARE_TIMER_FUNCTION(_func)			\
	void rtmp_timer_##_func(unsigned long data)
#endif
#endif /* __ECOS */

#define GET_TIMER_FUNCTION(_func)				\
	(PVOID)rtmp_timer_##_func

/* ----------------- Timer Related MARCO ---------------*/
/* In some os or chipset, we have a lot of timer functions and will read/write register, */
/*   it's not allowed in Linux USB sub-system to do it ( because of sleep issue when */
/*  submit to ctrl pipe). So we need a wrapper function to take care it. */

#ifdef RTMP_TIMER_TASK_SUPPORT
typedef VOID(
	*RTMP_TIMER_TASK_HANDLE) (
	IN PVOID SystemSpecific1,
	IN PVOID FunctionContext,
	IN PVOID SystemSpecific2,
	IN PVOID SystemSpecific3);
#endif /* RTMP_TIMER_TASK_SUPPORT */

typedef struct _RALINK_TIMER_STRUCT {
	RTMP_OS_TIMER TimerObj;	/* Ndis Timer object */
	BOOLEAN Valid;		/* Set to True when call RTMPInitTimer */
	BOOLEAN State;		/* True if timer cancelled */
	BOOLEAN PeriodicType;	/* True if timer is periodic timer */
	BOOLEAN Repeat;		/* True if periodic timer */
	ULONG TimerValue;	/* Timer value in milliseconds */
	ULONG cookie;		/* os specific object */
	void *pAd;
#ifdef RTMP_TIMER_TASK_SUPPORT
	RTMP_TIMER_TASK_HANDLE handle;
#endif				/* RTMP_TIMER_TASK_SUPPORT */
} RALINK_TIMER_STRUCT, *PRALINK_TIMER_STRUCT;


#ifdef RTMP_TIMER_TASK_SUPPORT
typedef struct _RTMP_TIMER_TASK_ENTRY_ {
	RALINK_TIMER_STRUCT *pRaTimer;
	struct _RTMP_TIMER_TASK_ENTRY_ *pNext;
} RTMP_TIMER_TASK_ENTRY;

#define TIMER_QUEUE_SIZE_MAX	128
typedef struct _RTMP_TIMER_TASK_QUEUE_ {
	unsigned int status;
	unsigned char *pTimerQPoll;
	RTMP_TIMER_TASK_ENTRY *pQPollFreeList;
	RTMP_TIMER_TASK_ENTRY *pQHead;
	RTMP_TIMER_TASK_ENTRY *pQTail;
} RTMP_TIMER_TASK_QUEUE;


INT RtmpTimerQThread(ULONG Context);

struct _RTMP_ADAPTER;
RTMP_TIMER_TASK_ENTRY *RtmpTimerQInsert(
	IN struct _RTMP_ADAPTER *pAd, 
	IN RALINK_TIMER_STRUCT *pTimer);

BOOLEAN RtmpTimerQRemove(
	IN struct _RTMP_ADAPTER *pAd, 
	IN RALINK_TIMER_STRUCT *pTimer);

void RtmpTimerQExit(struct _RTMP_ADAPTER *pAd);
void RtmpTimerQInit(struct _RTMP_ADAPTER *pAd);

#ifdef __ECOS
#define BUILD_TIMER_FUNCTION(_func)										\
void rtmp_timer_##_func(cyg_handle_t alarm, cyg_addrword_t data)					\
{																			\
	PRALINK_TIMER_STRUCT	_pTimer = (PRALINK_TIMER_STRUCT)data;				\
	RTMP_TIMER_TASK_ENTRY	*_pQNode;										\
	RTMP_ADAPTER			*_pAd;											\
																			\
	_pTimer->handle = _func;													\
	_pAd = (RTMP_ADAPTER *)_pTimer->pAd;										\
	_pQNode = RtmpTimerQInsert(_pAd, _pTimer); 								\
	if ((_pQNode == NULL) && (_pAd->TimerQ.status & RTMP_TASK_CAN_DO_INSERT))	\
		RTMP_OS_Add_Timer(&_pTimer->TimerObj, OS_HZ);               					\
}
#else /* !__ECOS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0))
#define BUILD_TIMER_FUNCTION(_func)										\
void rtmp_timer_##_func(OS_NDIS_MINIPORT_TIMER *timer)							\
{																			\
	PRALINK_TIMER_STRUCT	_pTimer = (PRALINK_TIMER_STRUCT)timer;				\
	RTMP_TIMER_TASK_ENTRY	*_pQNode;										\
	RTMP_ADAPTER			*_pAd;											\
																			\
	_pTimer->handle = _func;													\
	_pAd = (RTMP_ADAPTER *)_pTimer->pAd;										\
	_pQNode = RtmpTimerQInsert(_pAd, _pTimer); 								\
	if ((_pQNode == NULL) && (_pAd->TimerQ.status & RTMP_TASK_CAN_DO_INSERT))	\
		RTMP_OS_Add_Timer(&_pTimer->TimerObj, OS_HZ);               					\
}
#else
#define BUILD_TIMER_FUNCTION(_func)										\
void rtmp_timer_##_func(unsigned long data)										\
{																			\
	PRALINK_TIMER_STRUCT	_pTimer = (PRALINK_TIMER_STRUCT)data;				\
	RTMP_TIMER_TASK_ENTRY	*_pQNode;										\
	RTMP_ADAPTER			*_pAd;											\
																			\
	_pTimer->handle = _func;													\
	_pAd = (RTMP_ADAPTER *)_pTimer->pAd;										\
	_pQNode = RtmpTimerQInsert(_pAd, _pTimer); 								\
	if ((_pQNode == NULL) && (_pAd->TimerQ.status & RTMP_TASK_CAN_DO_INSERT))	\
		RTMP_OS_Add_Timer(&_pTimer->TimerObj, OS_HZ);               					\
}
#endif
#endif /* __ECOS */
#else /* !RTMP_TIMER_TASK_SUPPORT */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0))
#define BUILD_TIMER_FUNCTION(_func)										\
void rtmp_timer_##_func(OS_NDIS_MINIPORT_TIMER *timer)							\
{																			\
	PRALINK_TIMER_STRUCT	pTimer = (PRALINK_TIMER_STRUCT)timer;				\
																			\
	_func(NULL, (PVOID) pTimer->cookie, NULL, pTimer); 							\
	if (pTimer->Repeat)														\
		RTMP_OS_Add_Timer(&pTimer->TimerObj, pTimer->TimerValue);			\
}
#else
#define BUILD_TIMER_FUNCTION(_func)										\
void rtmp_timer_##_func(unsigned long data)										\
{																			\
	PRALINK_TIMER_STRUCT	pTimer = (PRALINK_TIMER_STRUCT) data;				\
																			\
	_func(NULL, (PVOID) pTimer->cookie, NULL, pTimer); 							\
	if (pTimer->Repeat)														\
		RTMP_OS_Add_Timer(&pTimer->TimerObj, pTimer->TimerValue);			\
}
#endif
#endif /* RTMP_TIMER_TASK_SUPPORT */

DECLARE_TIMER_FUNCTION(MlmePeriodicExecTimer);
DECLARE_TIMER_FUNCTION(MlmeRssiReportExec);
DECLARE_TIMER_FUNCTION(AsicRxAntEvalTimeout);
DECLARE_TIMER_FUNCTION(APSDPeriodicExec);
DECLARE_TIMER_FUNCTION(EnqueueStartForPSKExec);
#ifdef CONFIG_STA_SUPPORT
#ifdef ADHOC_WPA2PSK_SUPPORT
DECLARE_TIMER_FUNCTION(Adhoc_WpaRetryExec);
#endif /* ADHOC_WPA2PSK_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef DOT11W_PMF_SUPPORT
DECLARE_TIMER_FUNCTION(PMF_SAQueryTimeOut);
DECLARE_TIMER_FUNCTION(PMF_SAQueryConfirmTimeOut);
#endif /* DOT11W_PMF_SUPPORT */

#ifdef RTMP_MAC_USB
DECLARE_TIMER_FUNCTION(BeaconUpdateExec);
#ifdef CONFIG_MULTI_CHANNEL
DECLARE_TIMER_FUNCTION(EDCA_ActionTimeout);
DECLARE_TIMER_FUNCTION(HCCA_ActionTimeout);
#endif /* CONFIG_MULTI_CHANNEL */
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_AP_SUPPORT
DECLARE_TIMER_FUNCTION(APDetectOverlappingExec);

#ifdef DOT11N_DRAFT3
DECLARE_TIMER_FUNCTION(Bss2040CoexistTimeOut);
#endif /* DOT11N_DRAFT3 */

DECLARE_TIMER_FUNCTION(GREKEYPeriodicExec);
DECLARE_TIMER_FUNCTION(CMTimerExec);
DECLARE_TIMER_FUNCTION(WPARetryExec);
#ifdef AP_SCAN_SUPPORT
DECLARE_TIMER_FUNCTION(APScanTimeout);
#endif /* AP_SCAN_SUPPORT */
DECLARE_TIMER_FUNCTION(APQuickResponeForRateUpExec);

#ifdef IDS_SUPPORT
DECLARE_TIMER_FUNCTION(RTMPIdsPeriodicExec);
#endif /* IDS_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
DECLARE_TIMER_FUNCTION(FT_KDP_InfoBroadcast);
#endif /* DOT11R_FT_SUPPORT */

#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
DECLARE_TIMER_FUNCTION(BeaconTimeout);
DECLARE_TIMER_FUNCTION(ScanTimeout);
DECLARE_TIMER_FUNCTION(AuthTimeout);
DECLARE_TIMER_FUNCTION(AssocTimeout);
DECLARE_TIMER_FUNCTION(ReassocTimeout);
DECLARE_TIMER_FUNCTION(DisassocTimeout);
DECLARE_TIMER_FUNCTION(LinkDownExec);
DECLARE_TIMER_FUNCTION(StaQuickResponeForRateUpExec);
DECLARE_TIMER_FUNCTION(WpaDisassocApAndBlockAssoc);

#ifdef RTMP_PCI_SUPPORT
DECLARE_TIMER_FUNCTION(PsPollWakeExec);
DECLARE_TIMER_FUNCTION(RadioOnExec);
#endif /* RTMP_PCI_SUPPORT */
#ifdef QOS_DLS_SUPPORT
DECLARE_TIMER_FUNCTION(DlsTimeoutAction);
#endif /* QOS_DLS_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
DECLARE_TIMER_FUNCTION(TDLS_OffChExpired);
DECLARE_TIMER_FUNCTION(TDLS_BaseChExpired);
DECLARE_TIMER_FUNCTION(TDLS_LinkTimeoutAction);
DECLARE_TIMER_FUNCTION(TDLS_ChannelSwitchTimeAction);
DECLARE_TIMER_FUNCTION(TDLS_ChannelSwitchTimeOutAction);
DECLARE_TIMER_FUNCTION(TDLS_DisablePeriodChannelSwitchAction);
#endif /* DOT11Z_TDLS_SUPPORT */
#ifdef CFG_TDLS_SUPPORT
#ifdef UAPSD_SUPPORT
DECLARE_TIMER_FUNCTION(cfg_tdls_PTITimeoutAction);
#endif /*UAPSD_SUPPORT*/
#endif /* CFG_TDLS_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
DECLARE_TIMER_FUNCTION(FT_OTA_AuthTimeout);
DECLARE_TIMER_FUNCTION(FT_OTD_TimeoutAction);
#endif /* DOT11R_FT_SUPPORT */

#ifdef RTMP_MAC_USB
DECLARE_TIMER_FUNCTION(RtmpUsbStaAsicForceWakeupTimeout);
#endif /* RTMP_MAC_USB */

#ifdef STA_EASY_CONFIG_SETUP
DECLARE_TIMER_FUNCTION(AutoProvisionScanTimeOutAction);
DECLARE_TIMER_FUNCTION(InfraConnectionTimeout);
DECLARE_TIMER_FUNCTION(AdhocConnectionTimeout);
#endif /* STA_EASY_CONFIG_SETUP */
#endif /* CONFIG_STA_SUPPORT */

#ifdef TXBF_SUPPORT
DECLARE_TIMER_FUNCTION(eTxBfProbeTimerExec);
#endif // TXBF_SUPPORT //

#ifdef WSC_INCLUDED
DECLARE_TIMER_FUNCTION(WscEAPOLTimeOutAction);
DECLARE_TIMER_FUNCTION(Wsc2MinsTimeOutAction);
DECLARE_TIMER_FUNCTION(WscUPnPMsgTimeOutAction);
DECLARE_TIMER_FUNCTION(WscM2DTimeOutAction);
DECLARE_TIMER_FUNCTION(WscPBCTimeOutAction);
DECLARE_TIMER_FUNCTION(WscScanTimeOutAction);
DECLARE_TIMER_FUNCTION(WscProfileRetryTimeout);
#ifdef WSC_LED_SUPPORT
DECLARE_TIMER_FUNCTION(WscLEDTimer);
DECLARE_TIMER_FUNCTION(WscSkipTurnOffLEDTimer);
#endif /* WSC_LED_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
DECLARE_TIMER_FUNCTION(WscUpdatePortCfgTimeout);
#ifdef WSC_V2_SUPPORT
DECLARE_TIMER_FUNCTION(WscSetupLockTimeout);
#endif /* WSC_V2_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
#ifdef IWSC_SUPPORT
DECLARE_TIMER_FUNCTION(IWSC_T1TimerAction);
DECLARE_TIMER_FUNCTION(IWSC_T2TimerAction);
DECLARE_TIMER_FUNCTION(IWSC_EntryTimerAction);
DECLARE_TIMER_FUNCTION(IWSC_DevQueryAction);
#endif /* IWSC_SUPPORT */
#endif /* WSC_INCLUDED */

#ifdef MESH_SUPPORT
DECLARE_TIMER_FUNCTION(MeshPathReqTimeoutAction);
#endif /* MESH_SUPPORT */

#ifdef CONFIG_HOTSPOT
#ifdef CONFIG_STA_SUPPORT
DECLARE_TIMER_FUNCTION(GASResponseTimeout); 
DECLARE_TIMER_FUNCTION(GASCBDelayTimeout);
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
DECLARE_TIMER_FUNCTION(PostReplyTimeout); 
#endif /* CONFIG_AP_SUPPORT */
#endif /* CONFIG_HOTSPOT */

#ifdef WMM_ACM_SUPPORT
DECLARE_TIMER_FUNCTION(ACMP_TR_TC_ReqCheck);
DECLARE_TIMER_FUNCTION(ACMP_TR_STM_Check);
DECLARE_TIMER_FUNCTION(ACMP_TR_TC_General);
DECLARE_TIMER_FUNCTION(ACMP_CMD_Timer_Data_Simulation);
#endif /* WMM_ACM_SUPPORT */

#ifdef P2P_SUPPORT
DECLARE_TIMER_FUNCTION(P2PCTWindowTimer);
DECLARE_TIMER_FUNCTION(P2pSwNoATimeOut);
DECLARE_TIMER_FUNCTION(P2pPreAbsenTimeOut);
DECLARE_TIMER_FUNCTION(P2pWscTimeOut);
DECLARE_TIMER_FUNCTION(P2pReSendTimeOut);
DECLARE_TIMER_FUNCTION(P2pCliReConnectTimeOut);
#endif /* P2P_SUPPORT */

#ifdef CONFIG_ATE
DECLARE_TIMER_FUNCTION(ATEPeriodicExec);
#endif /* CONFIG_ATE */


#endif /* __RTMP_TIMER_H__ */

