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
	rt3090.h
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __RT3090_H__
#define __RT3090_H__

#ifdef RT3090

#ifndef RTMP_PCI_SUPPORT
#error "For RT3090, you should define the compile flag -DRTMP_PCI_SUPPORT"
#endif

#ifndef RTMP_MAC_PCI
#error "For RT3090, you should define the compile flag -DRTMP_MAC_PCI"
#endif

#ifndef RTMP_RF_RW_SUPPORT
#error "For RT3090, you should define the compile flag -DRTMP_RF_RW_SUPPORT"
#endif

#ifndef RT30xx
#error "For RT3090, you should define the compile flag -DRT30xx"
#endif

#ifdef CONFIG_STA_SUPPORT
#define PCIE_PS_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
#include "chip/rt30xx.h"

/*
  Device ID & Vendor ID, these values should match EEPROM value
*/
#if 0 /* os abl move to include/chip/chip_id.h */
#define NIC3090_PCIe_DEVICE_ID  0x3090		/* 1T/1R miniCard */
#define NIC3091_PCIe_DEVICE_ID  0x3091		/* 1T/2R miniCard */
#define NIC3092_PCIe_DEVICE_ID  0x3092		/* 2T/2R miniCard */
#endif /* 0 */

struct _RTMP_ADAPTER;

VOID NICInitRT3090RFRegisters(struct _RTMP_ADAPTER *pAd);

#endif /* RT3090 */

#endif /*__RT3090_H__ */

