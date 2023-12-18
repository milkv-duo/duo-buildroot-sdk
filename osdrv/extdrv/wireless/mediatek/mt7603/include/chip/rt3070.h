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
	rt3070.h
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __RT3070_H__
#define __RT3070_H__

#ifdef RT3070

struct _RTMP_ADAPTER;
struct _RSSI_SAMPLE;

#ifndef RTMP_USB_SUPPORT
#error "For RT3070, you should define the compile flag -DRTMP_USB_SUPPORT"
#endif

#ifndef RTMP_MAC_USB
#error "For RT3070, you should define the compile flag -DRTMP_MAC_USB"
#endif

#ifndef RTMP_RF_RW_SUPPORT
#error "For RT3070, you should define the compile flag -DRTMP_RF_RW_SUPPORT"
#endif

#ifndef RT30xx
#error "For RT3070, you should define the compile flag -DRT30xx"
#endif

#include "chip/rt30xx.h"

/*
    Device ID & Vendor ID, these values should match EEPROM value
*/

VOID RT3070_PowerTuning(struct _RTMP_ADAPTER *pAd, struct _RSSI_SAMPLE *pRssi);

VOID NICInitRT3070RFRegisters(struct _RTMP_ADAPTER *pAd);

#endif /* RT3070 */

#endif /* __RT3070_H__ */

