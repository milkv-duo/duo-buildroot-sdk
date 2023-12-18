/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
    sy_sta_ioctl.h

    Abstract:
    SigmaTel Wifi IOCTL header

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/
#ifndef __SY_STA_IOCTL_H__
#define __SY_STA_IOCTL_H__

#define WIFI_MAX_AP_RESULTS 30
#define WPS_STATUS_TABLE_BOUNDARY 0x100

struct _RTMP_ADAPTER;
typedef struct _RTMP_ADAPTER *PRTMP_ADAPTER;

extern int SYControlInit(IN PRTMP_ADAPTER pAd);
extern int SYControlExit(IN PRTMP_ADAPTER pAd);
extern void SYScanCompletedEventHdlr(IN PRTMP_ADAPTER pAd,
							  		 IN UCHAR wdev_idx);

DECLARE_TIMER_FUNCTION(SYWIFIAPScanTimerTimeout);
extern VOID SYWIFIAPScanTimerTimeout(
    		IN  PVOID SystemSpecific1, 
    		IN  PVOID FunctionContext, 
    		IN  PVOID SystemSpecific2, 
    		IN  PVOID SystemSpecific3);

#ifdef WSC_STA_SUPPORT
extern void SYWPSEndEventHdlr(IN PRTMP_ADAPTER pAd,
					   		  IN UCHAR wdev_idx);

extern void SYNextCanidatedEventHdlr(IN PRTMP_ADAPTER pAd,
							  		 IN UCHAR wdev_idx);

extern void SYWSCStatusFailEventHdlr(IN PRTMP_ADAPTER pAd,
							  		 IN UCHAR wdev_idx);

extern void SYSTALinkUpEventHdlr(IN PRTMP_ADAPTER pAd,
						  		 IN UCHAR wdev_idx);

extern DC_RETURN_CODE SYWriteNVRAM(DC_WIFI_MAC_CONFIGURATION *structP);

/* WPS definition */
#define SYROLE_ENROLLEE "1"
#define SYWSC_PIN_MODE  "1"
#define SYWSC_PBC_MODE  "2"
#define SYWSC_TRIGGER   "1"
#define SYWSC_STOP "1"
#define FOOL_PROOF_TIMEOUT (1000 * 5)

DECLARE_TIMER_FUNCTION(SYWIFIWPSTimerTimeout);
DECLARE_TIMER_FUNCTION(SYWIFIWPSFoolProofTimerTimeout);

extern VOID SYWIFIWPSTimerTimeout(
			IN  PVOID SystemSpecific1,
    		IN  PVOID FunctionContext,
    		IN  PVOID SystemSpecific2, 
    		IN  PVOID SystemSpecific3);

extern VOID SYWIFIWPSFoolProofTimerTimeout(
			IN  PVOID SystemSpecific1, 
    		IN  PVOID FunctionContext, 
    		IN  PVOID SystemSpecific2, 
    		IN  PVOID SystemSpecific3);

#endif /* WSC_STA_SUPPORT */


typedef struct _SY_WIFI_AP_RESULT
{
	WIFI_AP_RESULT_t *pSYAPScanResult;
	ULONG	IELength[WIFI_MAX_AP_RESULTS];
    UCHAR	IEs[WIFI_MAX_AP_RESULTS][MAX_VIE_LEN];	
}SY_WIFI_AP_RESULT, *PSY_WIFI_AP_RESULT;

typedef struct _SY_CONTROL
{
	BOOLEAN SYAPSiteSurveyOnly;
	DC_WIFI_MAC_CONFIGURATION CurrentWiFiMacConf;
	RTMP_NET_TASK_QUEUE SYNetTaskQueue;
	RALINK_TIMER_STRUCT SYWIFIAPScanTimer;
	UINT16 SYWiFiAPScanTimeOut;
	BOOLEAN SYWiFiAPScanTrigger;
	WifiAccessPointsCallback *pSYWiFiAPScanCallback;
	INT NumAPsFound;
	SY_WIFI_AP_RESULT SYWiFiAPResult;
	

#ifdef WSC_STA_SUPPORT
	RALINK_TIMER_STRUCT SYWIFIWPSTimer;
	UINT16 SYWiFiWPSTimeOut;
	RALINK_TIMER_STRUCT SYWIFIWPSFoolProofTimer;
	UINT16 SYWiFiWPSFoolProofTimeOut;
	BOOLEAN SYWiFiWPSTrigger;
	BOOLEAN	SYWiFiWPSSuccess;
	WPSCallback *pSYWiFiWPSCallback;
	BOOLEAN SYWPSPinMode;
	BOOLEAN StopWpsAPSearch;
	/* Semaphore for some wireless events */
	RTMP_OS_SEM SYWaitScanDoneSem;
	RTMP_OS_SEM SYWaitDoWPSPinSem;
	RTMP_NET_TASK_STRUCT WPSPinContinueTask;
	DC_WIFI_WPS_STATUS SYWPSStatus;
	DC_WIFI_IFACE_PARAMS SYWPSWiFiIfaceParams;
	BOOLEAN SYWPSStatusDone;
#endif /* WSC_STA_SUPPORT */
}SY_CONTROL, *PSY_CONTROL;

#endif /* __SY_STA_IOCTL_H__ */
