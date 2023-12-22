/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

 	Module Name:
 	config.h
 
	Abstract:
    Miniport generic portion header file

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/

#ifndef	__CONFIG_H__
#define	__CONFIG_H__

#pragma once

#define ASIC_TARGET_MODE 0

#ifdef _USB
#include <wdm.h>
#include <usbdi.h>
#include <usbdlib.h>
#else
#include <ntddk.h>
#endif

#include "CommonDriverUI.h"
#include "Ioctl.h"
#include "crc32.h"
#include "define.h"
#include "driver.h"
#include "Dma.h"
#include "Eeprom.h"
#include "GUIDs.h"
#include "Register.h"
#include "RT2860.h"
#include "TxRx.h"
#include "ComPort.h"
#include "HwHalDefine.h"
#include "HwHalProcess.h"
#include "Hal.h"
#include "nic.h"

#ifdef _USB



#include "bulkpnp.h"
#include "bulkdev.h"
#include "bulkrwr.h"
#include "bulkwmi.h"
#include "bulkpwr.h"
#include "QaUsb.h"

#if DBG
#define BulkUsb_DbgPrint(level, _x_) DbgPrint _x_;
#else
#define BulkUsb_DbgPrint(level, _x_) DbgPrint _x_;
#endif//end DBG


typedef struct _IRP_COMPLETION_CONTEXT {

    RTMP_ADAPTER *DeviceExtension;

    PKEVENT Event;

} IRP_COMPLETION_CONTEXT, *PIRP_COMPLETION_CONTEXT;

#define IDLE_INTERVAL 5000
#define BULKUSB_MAX_TRANSFER_SIZE   256
#define BULKUSB_TEST_BOARD_TRANSFER_BUFFER_SIZE (64 *1024 )
#endif//end USB

typedef unsigned char BOOLEAN;

#endif	// __CONFIG_H__
