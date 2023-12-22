/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	misc.h

	Abstract:

	Handling Misc Problem

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Sean Wang	2009-08-12		Create
	John Li		2009-12-23		Modified
*/
#ifdef RELEASE_EXCLUDE
	/* Handling Bluetooth Coexistence Problem */
#endif /* RELEASE_EXCLUDE */

#ifdef BT_COEXISTENCE_SUPPORT


#ifndef __MISC_H
#define __MISC_H

#include "misc_cmm.h"

#define IN
#define OUT
#define INOUT

#define CYRSTALL_SHARED			0x01
#define WIFI_2040_SWITCH_THRESHOLD 	BUSY_4
#define HISTORY_RECORD_NUM 		5
#ifdef RTMP_USB_SUPPORT
#define CHECK_TIME_INTERVAL 		10 /*unit : 100ms, must follow timer interval */
#define IDLE_STATE_THRESHOLD 	30
#define DETECT_TIMEOUT			100
#endif /* RTMP_USB_SUPPORT */
#ifdef RTMP_PCI_SUPPORT
#define CHECK_TIME_INTERVAL 		100   /*unit : 10ms, must follow timer interval */
#ifdef RELEASE_EXCLUDE
/* when > IDLE_STATE_THRESHOLD, we treat that Bluetooth is no busy */
#endif /* RELEASE_EXCLUDE */
#define IDLE_STATE_THRESHOLD 	10
#define DETECT_TIMEOUT			10
#endif /* RTMP_PCI_SUPPORT */

#ifdef DOT11N_DRAFT3
BOOLEAN WifiThroughputOverLimit(
	IN	PRTMP_ADAPTER	pAd,
	IN  UCHAR WifiThroughputLimit);
#endif /* DOT11N_DRAFT3 */

BUSY_DEGREE CheckBusy(
	IN PLONG History, 
	IN UCHAR HistorySize);

VOID Adjust(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN			bIssue4020, 
	IN ULONG			NoBusyTimeCount);

VOID TxPowerDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				Rssi,
	INOUT CHAR			*pDeltaPowerByBbpR1, 
	INOUT CHAR			*pDeltaPwr);

struct _RTMP_RA_LEGACY_TB;
VOID McsDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				CurrRateIdx, 
	IN struct _RTMP_RA_LEGACY_TB *pCurrTxRate, 
	INOUT CHAR			*pUpRateIdx, 
	INOUT CHAR			*pDownRateIdx);

VOID McsDown2(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR			MCS3, 
	IN UCHAR			MCS4, 
	IN UCHAR			MCS5, 
	IN UCHAR			MCS6, 
	INOUT UCHAR			*pTxRateIdx);

UCHAR TxBaSizeDown(RTMP_ADAPTER *pAd, UCHAR old_basize);
UCHAR TxBaDensityDown(RTMP_ADAPTER *pAd, UCHAR mpdu_density);

#ifdef RELEASE_EXCLUDE
VOID ReadParametersFromFile(
	IN PRTMP_ADAPTER pAd,
	RTMP_STRING *tmpbuf,
	RTMP_STRING *pBuffer);
#endif /* RELEASE_EXCLUDE */

VOID MiscInit(
	IN PRTMP_ADAPTER pAd);

VOID MiscUserCfgInit(
	IN PRTMP_ADAPTER pAd);

#ifdef RTMP_USB_SUPPORT
NTSTATUS DetectExecAtCmdThread(
	IN PRTMP_ADAPTER pAd, 
	IN PCmdQElmt CMDQelmt);
#endif /* RTMP_USB_SUPPORT */

VOID DetectExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

#endif /* __MISC_H */
#endif /* BT_COEXISTENCE_SUPPORT */

