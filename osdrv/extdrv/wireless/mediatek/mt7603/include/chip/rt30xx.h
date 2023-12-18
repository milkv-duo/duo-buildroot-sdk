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
	rt30xx.h
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __RT30XX_H__
#define __RT30XX_H__


struct _RTMP_ADAPTER;

#include "rtmp_type.h"

extern REG_PAIR RT3020_RFRegTable[];
extern UCHAR NUM_RF_3020_REG_PARMS;

VOID RT30xx_Init(struct _RTMP_ADAPTER *pAd);

VOID RT30xx_ChipSwitchChannel(struct _RTMP_ADAPTER *pAd, UCHAR ch, BOOLEAN scan);

VOID RT30xx_ChipBBPAdjust(struct _RTMP_ADAPTER *pAd);

VOID RT30xx_ChipAGCInit(struct _RTMP_ADAPTER *pAd, UCHAR BandWidth);

VOID RT30xxHaltAction(struct _RTMP_ADAPTER *pAd);
VOID RT30xxSetRxAnt(struct _RTMP_ADAPTER *pAd, UCHAR Ant);
VOID PostBBPInitialization(struct _RTMP_ADAPTER *pAd);

VOID RTMPFilterCalibration(struct _RTMP_ADAPTER *pAd);
VOID RT30xxLoadRFNormalModeSetup(struct _RTMP_ADAPTER *pAd);
VOID RT30xxLoadRFSleepModeSetup(struct _RTMP_ADAPTER *pAd);
VOID RT30xxReverseRFSleepModeSetup(struct _RTMP_ADAPTER *pAd, BOOLEAN FlgIsInitState);

#ifdef MICROWAVE_OVEN_SUPPORT
VOID RT30xx_AsicMitigateMicrowave(
	IN struct _RTMP_ADAPTER *pAd);

VOID RT30xx_AsicMeasureFalseCCA(
	IN struct _RTMP_ADAPTER *pAd);
#endif /* MICROWAVE_OVEN_SUPPORT */

#endif /*__RT30XX_H__ */

