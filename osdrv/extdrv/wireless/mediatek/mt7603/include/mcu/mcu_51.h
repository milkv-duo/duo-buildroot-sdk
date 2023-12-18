/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	mcu_51.h

	Abstract:

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/

#ifndef __MCU_51_H__
#define __MCU_51_H__

#include "mcu.h"

struct _RTMP_ADAPTER;

/*
 * cmd unit (8051)
 */
struct cmd_unit {
	UCHAR type;
	UCHAR token;
	UCHAR arg0;
	UCHAR arg1;
};

INT RtmpAsicEraseFirmware(
	struct _RTMP_ADAPTER *pAd);

NDIS_STATUS RtmpAsicLoadFirmware(
	struct _RTMP_ADAPTER *pAd);

NDIS_STATUS isMCUnotReady(
	struct _RTMP_ADAPTER *pAd);

NDIS_STATUS isMCUNeedToLoadFIrmware(
	struct _RTMP_ADAPTER *pAd);

BOOLEAN RtmpAsicCheckCommanOk(RTMP_ADAPTER *pAd, UCHAR Command);

INT RtmpAsicSendCommandToMcu(
	struct _RTMP_ADAPTER *pAd,
	UCHAR Command,
	UCHAR Token,
	UCHAR Arg0,
	UCHAR Arg1,
	BOOLEAN FlgIsNeedLocked);

#ifdef RTMP_PCI_SUPPORT
void m51_pci_fw_init(struct _RTMP_ADAPTER *ad);
#endif

#ifdef RTMP_USB_SUPPORT
void m51_usb_fw_init(struct _RTMP_ADAPTER *ad);
#endif

#endif
