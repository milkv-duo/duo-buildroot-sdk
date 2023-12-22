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
 	Driver.c
 
	Abstract:
    Central header file to maintain all include files for all NDIS
    miniport driver routines.

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/


#define INITGUID		// initialize PCIDUMPR_GUID in this module

#include "config.h"
#include "FW7603.h"
#include "FW7636RomPatch.h"
#include "FW7636.h"

#include <Ntstrsafe.h>
#ifdef _USB
#include "bulkusr.h"
#endif

ULONG	RTDebugLevel = RT_DEBUG_TRACE;
#define RETRY_INTERVAL    900*1000 //5s
BOOLEAN					g_bDam4kBoundaryCheck = FALSE;
ULONG	TXWI_SIZE;
//ULONG g_chPattern[10];//
//extern UCHAR	g_Pattern1[52];
//extern UCHAR	g_Pattern2[4000];
UCHAR g_FWSeqUI 		= 0;//fw download
UCHAR g_FWSeqMCU 		= 0;//fw download
UCHAR g_FWRspStatus 	= 0;//fw download

//UCHAR g_PacketCMDSeqUI		= 0;//fw download
UCHAR g_PacketCMDSeqMCU 		= 0;//fw download
UCHAR g_PacketCMDRspData[PKTCMD_EVENT_BUFFER];

extern UCHAR	g_TxDPattern[32];
extern UCHAR	g_PayloadPattern[4000];
extern ULONG    g_ulTXPacketLength;
UCHAR FirmwareImage4K[] = {
    0x02, 0x0f, 0x1c, 0x02, 0x0e, 0x0f, 0x8f, 0x50, 0xd2, 0x59, 0x22, 0x02, 0x0c, 0x31, 0x8f, 0x54, 
    0xd2, 0x58, 0x22, 0x02, 0x0b, 0x23, 0x02, 0x0e, 0x31, 0xc3, 0x22, 0x02, 0x0b, 0xe2, 0x90, 0x04, 
    0x14, 0xe0, 0x20, 0xe7, 0x03, 0x02, 0x04, 0x91, 0x90, 0x70, 0x12, 0xe0, 0xf5, 0x56, 0x90, 0x04, 
    0x04, 0xe0, 0x12, 0x0d, 0xc5, 0x00, 0xc8, 0x30, 0x00, 0x9f, 0x31, 0x00, 0x81, 0x35, 0x00, 0x78, 
    0x36, 0x00, 0xd5, 0x40, 0x00, 0xec, 0x41, 0x01, 0x03, 0x50, 0x01, 0x48, 0x51, 0x01, 0x51, 0x52, 
    0x01, 0x51, 0x53, 0x01, 0x51, 0x54, 0x01, 0x8d, 0x55, 0x01, 0xea, 0x56, 0x02, 0x3d, 0x70, 0x02, 
    0x63, 0x71, 0x02, 0x8c, 0x72, 0x03, 0x37, 0x73, 0x03, 0x5b, 0x74, 0x04, 0x05, 0x80, 0x04, 0x75, 
    0x83, 0x04, 0x2c, 0x91, 0x00, 0x00, 0x04, 0x91, 0x90, 0x70, 0x11, 0xe0, 0xf5, 0x3c, 0x02, 0x04, 
    0x8b, 0xe5, 0x55, 0xb4, 0x02, 0x0f, 0xe5, 0x58, 0x30, 0xe0, 0x06, 0x90, 0x01, 0x0d, 0x74, 0x08, 
    0xf0, 0x7d, 0x01, 0x80, 0x02, 0x7d, 0x02, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x02, 0x04, 0x8b, 0x20, 
    0x02, 0x03, 0x30, 0x03, 0x0a, 0x7d, 0x02, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x02, 0x04, 0x8b, 0xe5, 
    0x25, 0xd3, 0x94, 0x01, 0x40, 0x0c, 0x90, 0x01, 0x0c, 0xe0, 0x44, 0x02, 0xf0, 0xa3, 0xe0, 0x44, 
    0x04, 0xf0, 0x85, 0x56, 0x41, 0xd2, 0x02, 0x22, 0x90, 0x70, 0x11, 0xe0, 0xb4, 0x5a, 0x03, 0xc2, 
    0x4f, 0x22, 0xd2, 0x4f, 0x22, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x50, 0x03, 0x02, 0x04, 0x91, 0x90, 
    0x01, 0x0c, 0xe0, 0x44, 0x02, 0xf0, 0xa3, 0xe0, 0x44, 0x04, 0xf0, 0x22, 0xe5, 0x25, 0xd3, 0x94, 
    0x01, 0x50, 0x03, 0x02, 0x04, 0x91, 0x90, 0x01, 0x0c, 0xe0, 0x54, 0xfd, 0xf0, 0xa3, 0xe0, 0x54, 
    0xfb, 0xf0, 0x22, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x07, 0xe5, 0x55, 0x60, 0x03, 0x02, 0x04, 
    0x91, 0x90, 0x70, 0x10, 0xe0, 0x54, 0x7f, 0xff, 0xbf, 0x0a, 0x0d, 0x90, 0x70, 0x11, 0xe0, 0xb4, 
    0x08, 0x06, 0x75, 0x4e, 0x01, 0x75, 0x4f, 0x84, 0x90, 0x70, 0x10, 0xe0, 0x54, 0x7f, 0xff, 0xbf, 
    0x02, 0x12, 0x90, 0x70, 0x11, 0xe0, 0x64, 0x08, 0x60, 0x04, 0xe0, 0xb4, 0x20, 0x06, 0x75, 0x4e, 
    0x03, 0x75, 0x4f, 0x20, 0xe4, 0xf5, 0x27, 0x22, 0x90, 0x70, 0x11, 0xe0, 0x24, 0xff, 0x92, 0x47, 
    0x22, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x07, 0xe5, 0x55, 0x60, 0x03, 0x02, 0x03, 0x42, 0x90, 
    0x04, 0x04, 0xe0, 0x25, 0xe0, 0x24, 0x5d, 0xf5, 0x57, 0x90, 0x70, 0x10, 0xe0, 0xff, 0x74, 0x47, 
    0x25, 0x57, 0xf8, 0xc6, 0xef, 0xc6, 0x90, 0x70, 0x11, 0xe0, 0xff, 0x74, 0x48, 0x25, 0x57, 0xf8, 
    0xc6, 0xef, 0xc6, 0xe4, 0xfd, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x02, 0x04, 0x8b, 0xe5, 0x25, 0xd3, 
    0x94, 0x01, 0x40, 0x07, 0xe5, 0x55, 0x60, 0x03, 0x02, 0x03, 0x42, 0xe5, 0x47, 0x64, 0x07, 0x60, 
    0x1d, 0xe5, 0x47, 0x64, 0x08, 0x60, 0x17, 0xe5, 0x47, 0x64, 0x09, 0x60, 0x11, 0xe5, 0x47, 0x64, 
    0x0a, 0x60, 0x0b, 0xe5, 0x47, 0x64, 0x0b, 0x60, 0x05, 0xe5, 0x47, 0xb4, 0x0c, 0x08, 0x90, 0x70, 
    0x11, 0xe0, 0x54, 0x0f, 0xf5, 0x3a, 0xe5, 0x47, 0xb4, 0x09, 0x08, 0xe5, 0x3a, 0xb4, 0x03, 0x03, 
    0xe4, 0xf5, 0x46, 0xe5, 0x47, 0xb4, 0x0a, 0x08, 0xe5, 0x3a, 0xb4, 0x01, 0x03, 0xe4, 0xf5, 0x46, 
    0xe4, 0xfd, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0xd2, 0x04, 0x22, 0x90, 0x70, 0x11, 0xe0, 0xf4, 0xff, 
    0x90, 0x70, 0x10, 0xe0, 0x5f, 0xff, 0x90, 0x70, 0x11, 0xe0, 0x55, 0x27, 0x4f, 0x90, 0x70, 0x18, 
    0xf0, 0x90, 0x70, 0x11, 0xe0, 0x90, 0x70, 0x19, 0xf0, 0xe4, 0xfd, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 
    0x30, 0x15, 0x03, 0xd2, 0x14, 0x22, 0x90, 0x70, 0x18, 0xe0, 0xf5, 0x27, 0x90, 0x02, 0x29, 0xe0, 
    0xff, 0x90, 0x70, 0x19, 0xe0, 0xfe, 0xef, 0x5e, 0x90, 0x02, 0x29, 0xf0, 0x30, 0x47, 0x04, 0xaf, 
    0x27, 0x80, 0x04, 0xe5, 0x27, 0xf4, 0xff, 0x90, 0x02, 0x28, 0xef, 0xf0, 0x22, 0xe5, 0x25, 0xd3, 
    0x94, 0x01, 0x40, 0x07, 0xe5, 0x55, 0x60, 0x03, 0x02, 0x03, 0x42, 0x90, 0x70, 0x10, 0xe0, 0xfe, 
    0x90, 0x70, 0x11, 0xe0, 0xfd, 0xed, 0xf8, 0xe6, 0xf5, 0x57, 0xfd, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 
    0x02, 0x04, 0x8b, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x07, 0xe5, 0x55, 0x60, 0x03, 0x02, 0x03, 
    0x42, 0x90, 0x70, 0x10, 0xe0, 0xfe, 0x90, 0x70, 0x11, 0xe0, 0xfd, 0xed, 0xf5, 0x82, 0x8e, 0x83, 
    0xe0, 0xf5, 0x57, 0xfd, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x02, 0x04, 0x8b, 0x90, 0x10, 0x00, 0xe0, 
    0xf5, 0x57, 0xe4, 0xf5, 0x58, 0xf5, 0x59, 0x90, 0x10, 0x03, 0xe0, 0xb4, 0x28, 0x05, 0x75, 0x58, 
    0x01, 0x80, 0x3c, 0x90, 0x10, 0x03, 0xe0, 0xb4, 0x30, 0x05, 0x75, 0x58, 0x02, 0x80, 0x30, 0x90, 
    0x10, 0x03, 0xe0, 0xb4, 0x33, 0x05, 0x75, 0x58, 0x04, 0x80, 0x24, 0x90, 0x10, 0x03, 0xe0, 0xb4, 
    0x35, 0x0c, 0x90, 0x10, 0x02, 0xe0, 0xb4, 0x72, 0x05, 0x75, 0x58, 0x08, 0x80, 0x11, 0x90, 0x10, 
    0x03, 0xe0, 0xb4, 0x35, 0x0a, 0x90, 0x10, 0x02, 0xe0, 0xb4, 0x93, 0x03, 0x75, 0x58, 0x10, 0xe5, 
    0x58, 0x30, 0xe1, 0x19, 0x90, 0x05, 0x08, 0xe0, 0x44, 0x01, 0xf0, 0xfd, 0x90, 0x05, 0x05, 0xe0, 
    0x54, 0xfb, 0xf0, 0x44, 0x04, 0xf0, 0xed, 0x54, 0xfe, 0x90, 0x05, 0x08, 0xf0, 0xe4, 0xf5, 0x4e, 
    0xf5, 0x4f, 0x75, 0x3a, 0xff, 0xf5, 0x25, 0x90, 0x05, 0xa4, 0x74, 0x11, 0xf0, 0xa3, 0x74, 0xff, 
    0xf0, 0xa3, 0x74, 0x03, 0xf0, 0xd2, 0x4f, 0x90, 0x01, 0x0d, 0xe0, 0x44, 0x40, 0xf0, 0x75, 0x3c, 
    0xff, 0xad, 0x57, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x90, 0x70, 0x34, 0x74, 0x31, 0xf0, 0xa3, 0x74, 
    0x26, 0xf0, 0xc2, 0x17, 0x02, 0x04, 0x8b, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x0b, 0xe5, 0x55, 
    0x60, 0x07, 0x7d, 0x03, 0xaf, 0x56, 0x02, 0x0d, 0xeb, 0x90, 0x70, 0x10, 0xe0, 0x24, 0xff, 0x92, 
    0x93, 0xe4, 0xfd, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x02, 0x04, 0x8b, 0x90, 0x10, 0x00, 0xe0, 0x90, 
    0x10, 0x2c, 0xf0, 0x90, 0x10, 0x2f, 0x74, 0x40, 0xf0, 0x90, 0x70, 0x11, 0xe0, 0xf5, 0x57, 0xe0, 
    0x54, 0x80, 0x90, 0x70, 0x32, 0xf0, 0x90, 0x70, 0x10, 0xe0, 0xff, 0x90, 0x70, 0x11, 0xe0, 0xd3, 
    0x9f, 0x40, 0x35, 0x90, 0x70, 0x33, 0xe5, 0x57, 0xf0, 0x90, 0x70, 0x10, 0xe0, 0xff, 0x90, 0x70, 
    0x33, 0xe0, 0xc3, 0x9f, 0x40, 0x57, 0xe0, 0xff, 0x90, 0x70, 0x32, 0xe0, 0x4f, 0x90, 0x05, 0x00, 
    0xf0, 0xa3, 0x74, 0x11, 0xf0, 0xa3, 0x74, 0x01, 0xf0, 0x74, 0x03, 0xf0, 0xff, 0x12, 0x0e, 0xf8, 
    0x90, 0x70, 0x33, 0xe0, 0x14, 0xf0, 0x80, 0xd1, 0x90, 0x70, 0x33, 0xe5, 0x57, 0xf0, 0x90, 0x70, 
    0x10, 0xe0, 0xff, 0x90, 0x70, 0x33, 0xe0, 0xd3, 0x9f, 0x50, 0x22, 0xe0, 0xff, 0x90, 0x70, 0x32, 
    0xe0, 0x4f, 0x90, 0x05, 0x00, 0xf0, 0xa3, 0x74, 0x11, 0xf0, 0xa3, 0x74, 0x01, 0xf0, 0x74, 0x03, 
    0xf0, 0xff, 0x12, 0x0e, 0xf8, 0x90, 0x70, 0x33, 0xe0, 0x04, 0xf0, 0x80, 0xd1, 0x90, 0x10, 0x00, 
    0xe0, 0x90, 0x10, 0x2c, 0xf0, 0x90, 0x10, 0x2f, 0x74, 0x7f, 0xf0, 0xe4, 0xfd, 0xaf, 0x56, 0x12, 
    0x0d, 0xeb, 0x02, 0x04, 0x8b, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x0d, 0xe5, 0x55, 0x60, 0x09, 
    0x7d, 0x03, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x80, 0x72, 0x90, 0x70, 0x10, 0xe0, 0x24, 0xff, 0x92, 
    0x4a, 0xd2, 0x05, 0xad, 0x57, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x80, 0x5f, 0x90, 0x70, 0x11, 0xe0, 
    0x24, 0xff, 0x92, 0x17, 0x90, 0x70, 0x10, 0xe0, 0xf5, 0x5d, 0xad, 0x57, 0xaf, 0x56, 0x12, 0x0d, 
    0xeb, 0x90, 0x04, 0x14, 0x74, 0x80, 0xf0, 0x30, 0x17, 0x13, 0x90, 0x10, 0x00, 0xe0, 0x90, 0x10, 
    0x2c, 0xf0, 0x90, 0x10, 0x2f, 0xe0, 0x54, 0xf0, 0xf5, 0x57, 0x45, 0x5d, 0xf0, 0xe4, 0x90, 0x70, 
    0x13, 0xf0, 0xe5, 0x56, 0xf4, 0x60, 0x2a, 0x90, 0x70, 0x25, 0xe0, 0x44, 0x01, 0xf0, 0x90, 0x02, 
    0x2c, 0x74, 0xff, 0xf0, 0x22, 0xe4, 0xf5, 0x25, 0xd2, 0x4f, 0x90, 0x70, 0x10, 0xe0, 0xf4, 0x60, 
    0x03, 0xe0, 0xf5, 0x25, 0xad, 0x57, 0xaf, 0x56, 0x12, 0x0d, 0xeb, 0x90, 0x04, 0x14, 0x74, 0x80, 
    0xf0, 0x22, 0xc2, 0xaf, 0x90, 0x04, 0x14, 0xe0, 0x54, 0x0e, 0x60, 0x04, 0xd2, 0x18, 0x80, 0x08, 
    0xe5, 0x4e, 0x45, 0x4f, 0x24, 0xff, 0x92, 0x18, 0xd2, 0xaf, 0x90, 0x04, 0x14, 0xe0, 0xa2, 0xe4, 
    0x92, 0x19, 0x74, 0x1e, 0xf0, 0xe5, 0x5f, 0x54, 0x0f, 0xf5, 0x2d, 0xe5, 0x2a, 0x70, 0x13, 0x30, 
    0x18, 0x05, 0xe5, 0x5f, 0x20, 0xe5, 0x0b, 0x30, 0x19, 0x19, 0xe5, 0x5f, 0x54, 0x30, 0xff, 0xbf, 
    0x30, 0x11, 0xe5, 0x2a, 0x70, 0x05, 0x75, 0x2a, 0x0c, 0x80, 0x02, 0x15, 0x2a, 0xd2, 0x6c, 0xd2, 
    0x6d, 0x80, 0x0f, 0xe5, 0x5f, 0x30, 0xe6, 0x06, 0xc2, 0x6c, 0xd2, 0x6d, 0x80, 0x04, 0xd2, 0x6c, 
    0xc2, 0x6d, 0xe5, 0x47, 0x64, 0x03, 0x70, 0x21, 0x30, 0x4b, 0x06, 0xc2, 0x6c, 0xd2, 0x6d, 0x80, 
    0x18, 0xe5, 0x2a, 0x70, 0x03, 0x30, 0x4c, 0x11, 0xc2, 0x4c, 0xe5, 0x2a, 0x70, 0x05, 0x75, 0x2a, 
    0x07, 0x80, 0x02, 0x15, 0x2a, 0xd2, 0x6c, 0xd2, 0x6d, 0xe5, 0x47, 0xb4, 0x09, 0x14, 0xe5, 0x44, 
    0x20, 0xe3, 0x0b, 0xe5, 0x3a, 0x64, 0x02, 0x60, 0x05, 0xe5, 0x3a, 0xb4, 0x03, 0x04, 0xc2, 0x6c, 
    0xd2, 0x6d, 0xe5, 0x47, 0xb4, 0x0a, 0x13, 0xe5, 0x3a, 0xb4, 0x01, 0x06, 0xc2, 0x6c, 0xd2, 0x6d, 
    0x80, 0x08, 0xe5, 0x3a, 0x70, 0x04, 0xd2, 0x6c, 0xc2, 0x6d, 0x20, 0x69, 0x07, 0xe5, 0x5e, 0x20, 
    0xe0, 0x02, 0xb2, 0x68, 0x20, 0x6b, 0x07, 0xe5, 0x5e, 0x20, 0xe1, 0x02, 0xb2, 0x6a, 0x20, 0x6d, 
    0x07, 0xe5, 0x5e, 0x20, 0xe2, 0x02, 0xb2, 0x6c, 0x75, 0x2e, 0x40, 0x20, 0x69, 0x04, 0xa2, 0x68, 
    0x80, 0x26, 0x30, 0x68, 0x06, 0xe5, 0x46, 0xa2, 0xe2, 0x80, 0x1d, 0xe5, 0x5e, 0x20, 0xe0, 0x04, 
    0x7f, 0x01, 0x80, 0x02, 0x7f, 0x00, 0xe5, 0x46, 0x54, 0xf0, 0xfe, 0xbe, 0xf0, 0x04, 0x7e, 0x01, 
    0x80, 0x02, 0x7e, 0x00, 0xee, 0x6f, 0x24, 0xff, 0x92, 0x73, 0x92, 0x72, 0x20, 0x6b, 0x04, 0xa2, 
    0x6a, 0x80, 0x26, 0x30, 0x6a, 0x06, 0xe5, 0x46, 0xa2, 0xe2, 0x80, 0x1d, 0xe5, 0x5e, 0x20, 0xe1, 
    0x04, 0x7f, 0x01, 0x80, 0x02, 0x7f, 0x00, 0xe5, 0x46, 0x54, 0xf0, 0xfe, 0xbe, 0xf0, 0x04, 0x7e, 
    0x01, 0x80, 0x02, 0x7e, 0x00, 0xee, 0x6f, 0x24, 0xff, 0x92, 0x75, 0x92, 0x74, 0x20, 0x6d, 0x04, 
    0xa2, 0x6c, 0x80, 0x26, 0xe5, 0x47, 0x64, 0x0a, 0x70, 0x22, 0x30, 0x6c, 0x06, 0xe5, 0x46, 0xa2, 
    0xe3, 0x80, 0x17, 0xe5, 0x3a, 0xb4, 0x01, 0x06, 0xe5, 0x46, 0xa2, 0xe3, 0x80, 0x34, 0xe5, 0x46, 
    0x20, 0xe4, 0x03, 0x30, 0xe5, 0x03, 0xd3, 0x80, 0x01, 0xc3, 0x80, 0x26, 0x30, 0x6c, 0x06, 0xe5, 
    0x46, 0xa2, 0xe2, 0x80, 0x1d, 0xe5, 0x5e, 0x20, 0xe2, 0x04, 0x7f, 0x01, 0x80, 0x02, 0x7f, 0x00, 
    0xe5, 0x46, 0x54, 0xf0, 0xfe, 0xbe, 0xf0, 0x04, 0x7e, 0x01, 0x80, 0x02, 0x7e, 0x00, 0xee, 0x6f, 
    0x24, 0xff, 0x92, 0x71, 0x92, 0x70, 0x90, 0x10, 0x00, 0xe0, 0x90, 0x10, 0x2c, 0xf0, 0x90, 0x10, 
    0x03, 0xe0, 0xc3, 0x94, 0x30, 0x40, 0x19, 0xe0, 0x64, 0x32, 0x60, 0x14, 0xa2, 0x71, 0x92, 0x77, 
    0xa2, 0x70, 0x92, 0x76, 0xe5, 0x2e, 0x13, 0x13, 0x54, 0x3f, 0xf5, 0x2e, 0xc2, 0x77, 0xd2, 0x76, 
    0x30, 0x17, 0x0d, 0x53, 0x2e, 0xf0, 0xe5, 0x2e, 0x45, 0x5d, 0x90, 0x10, 0x2f, 0xf0, 0x80, 0x06, 
    0x90, 0x10, 0x2f, 0xe5, 0x2e, 0xf0, 0xe5, 0x47, 0x64, 0x06, 0x70, 0x47, 0x90, 0x02, 0x28, 0xe0, 
    0x30, 0x47, 0x03, 0xff, 0x80, 0x02, 0xf4, 0xff, 0x8f, 0x27, 0x90, 0x02, 0x29, 0xe0, 0x54, 0xfe, 
    0xf0, 0xe5, 0x43, 0xc4, 0x54, 0x0f, 0x14, 0x60, 0x0c, 0x24, 0xfe, 0x60, 0x0c, 0x24, 0x03, 0x70, 
    0x13, 0xc2, 0x38, 0x80, 0x0f, 0xd2, 0x38, 0x80, 0x0b, 0xe5, 0x46, 0x30, 0xe2, 0x03, 0xd3, 0x80, 
    0x01, 0xc3, 0x92, 0x38, 0x30, 0x47, 0x05, 0xaf, 0x27, 0x02, 0x07, 0xe8, 0xe5, 0x27, 0xf4, 0xff, 
    0x02, 0x07, 0xe8, 0xe5, 0x47, 0x64, 0x07, 0x60, 0x0f, 0xe5, 0x47, 0x64, 0x08, 0x60, 0x09, 0xe5, 
    0x47, 0x64, 0x09, 0x60, 0x03, 0x02, 0x07, 0x56, 0x90, 0x02, 0x28, 0xe0, 0x30, 0x47, 0x03, 0xff, 
    0x80, 0x02, 0xf4, 0xff, 0x8f, 0x27, 0x90, 0x02, 0x29, 0xe0, 0x54, 0xfc, 0xf0, 0xe5, 0x3a, 0x14, 
    0x60, 0x22, 0x14, 0x60, 0x25, 0x14, 0x60, 0x2d, 0x24, 0xfc, 0x60, 0x49, 0x24, 0xf9, 0x60, 0x14, 
    0x24, 0x0e, 0x70, 0x50, 0xe5, 0x46, 0x13, 0x13, 0x54, 0x3f, 0x75, 0xf0, 0x03, 0x84, 0xe5, 0xf0, 
    0x24, 0xff, 0x80, 0x3a, 0xd2, 0x39, 0xc2, 0x38, 0x80, 0x3e, 0xe5, 0x46, 0x30, 0xe2, 0x03, 0xd3, 
    0x80, 0x1d, 0xc3, 0x80, 0x1a, 0xe5, 0x46, 0x30, 0xe2, 0x0d, 0x54, 0x38, 0xc3, 0x94, 0x30, 0x50, 
    0x06, 0x7e, 0x00, 0x7f, 0x01, 0x80, 0x04, 0x7e, 0x00, 0x7f, 0x00, 0xee, 0x4f, 0x24, 0xff, 0x92, 
    0x38, 0xc2, 0x39, 0x80, 0x13, 0xe5, 0x46, 0x30, 0xe2, 0x03, 0xd3, 0x80, 0x01, 0xc3, 0x92, 0x39, 
    0xc2, 0x38, 0x80, 0x04, 0xc2, 0x38, 0xc2, 0x39, 0x30, 0x47, 0x04, 0xaf, 0x27, 0x80, 0x04, 0xe5, 
    0x27, 0xf4, 0xff, 0x02, 0x07, 0xe8, 0xe5, 0x47, 0x64, 0x0c, 0x60, 0x09, 0xe5, 0x47, 0x64, 0x0b, 
    0x60, 0x03, 0x02, 0x07, 0xed, 0x90, 0x02, 0x28, 0xe0, 0x30, 0x47, 0x03, 0xff, 0x80, 0x02, 0xf4, 
    0xff, 0x8f, 0x27, 0x90, 0x02, 0x29, 0xe0, 0x54, 0xfd, 0xf0, 0xe5, 0x3a, 0x14, 0x60, 0x20, 0x14, 
    0x60, 0x21, 0x14, 0x60, 0x2b, 0x24, 0xfc, 0x60, 0x45, 0x24, 0xf9, 0x60, 0x12, 0x24, 0x0e, 0x70, 
    0x4a, 0xe5, 0x46, 0x13, 0x13, 0x54, 0x3f, 0x75, 0xf0, 0x03, 0x84, 0xe5, 0xf0, 0x80, 0x29, 0xd2, 
    0x39, 0x80, 0x3a, 0xe5, 0x46, 0x30, 0xe2, 0x03, 0xd3, 0x80, 0x01, 0xc3, 0x92, 0x39, 0x80, 0x2d, 
    0xe5, 0x46, 0x30, 0xe2, 0x0d, 0x54, 0x38, 0xc3, 0x94, 0x30, 0x50, 0x06, 0x7e, 0x00, 0x7f, 0x01, 
    0x80, 0x04, 0x7e, 0x00, 0x7f, 0x00, 0xee, 0x4f, 0x24, 0xff, 0x92, 0x39, 0x80, 0x0f, 0xe5, 0x46, 
    0x30, 0xe2, 0x03, 0xd3, 0x80, 0x01, 0xc3, 0x92, 0x39, 0x80, 0x02, 0xc2, 0x39, 0x30, 0x47, 0x04, 
    0xaf, 0x27, 0x80, 0x04, 0xe5, 0x27, 0xf4, 0xff, 0x90, 0x02, 0x28, 0xef, 0xf0, 0x22, 0xe4, 0xf5, 
    0x30, 0xc2, 0xaf, 0xe5, 0x51, 0x14, 0x60, 0x47, 0x14, 0x60, 0x65, 0x24, 0x02, 0x60, 0x03, 0x02, 
    0x09, 0x49, 0xd2, 0x59, 0x75, 0x55, 0x01, 0x90, 0x02, 0x08, 0xe0, 0x54, 0xfe, 0xf0, 0xe0, 0x20, 
    0xe1, 0x23, 0x90, 0x04, 0x34, 0xe0, 0xb4, 0x02, 0x1c, 0xa3, 0xe0, 0xb4, 0x02, 0x17, 0xa3, 0xe0, 
    0xb4, 0x02, 0x12, 0x7f, 0x20, 0x12, 0x00, 0x06, 0x90, 0x10, 0x04, 0xe0, 0x54, 0xf3, 0xf0, 0x75, 
    0x51, 0x01, 0x02, 0x09, 0x49, 0xe5, 0x50, 0x70, 0x06, 0x75, 0x30, 0x03, 0x02, 0x09, 0x49, 0x90, 
    0x12, 0x00, 0xe0, 0x54, 0x03, 0x70, 0x12, 0x7f, 0x20, 0x12, 0x00, 0x06, 0x90, 0x02, 0x08, 0xe0, 
    0x54, 0xfb, 0xf0, 0x75, 0x51, 0x02, 0x02, 0x09, 0x49, 0xe5, 0x50, 0x70, 0x03, 0x02, 0x09, 0x44, 
    0x90, 0x02, 0x08, 0xe0, 0x30, 0xe3, 0x03, 0x02, 0x09, 0x40, 0x90, 0x04, 0x37, 0xe0, 0x64, 0x22, 
    0x60, 0x03, 0x02, 0x09, 0x40, 0x90, 0x12, 0x04, 0x74, 0x0a, 0xf0, 0xe5, 0x58, 0x30, 0xe3, 0x1c, 
    0x90, 0x00, 0x02, 0xe0, 0x30, 0xe0, 0x15, 0xe4, 0x90, 0x05, 0x00, 0xf0, 0xa3, 0x74, 0x08, 0xf0, 
    0xa3, 0x74, 0x01, 0xf0, 0x74, 0x03, 0xf0, 0x7f, 0x01, 0x12, 0x0e, 0xe9, 0x90, 0x13, 0x28, 0xe0, 
    0x90, 0x70, 0x1a, 0xf0, 0x90, 0x13, 0x29, 0xe0, 0x90, 0x70, 0x1b, 0xf0, 0x90, 0x13, 0x2b, 0xe0, 
    0x90, 0x70, 0x22, 0xf0, 0x90, 0x13, 0x28, 0xe0, 0x54, 0xf0, 0xf0, 0xa3, 0xe0, 0x54, 0xf0, 0xf0, 
    0x90, 0x13, 0x2b, 0xe0, 0x54, 0xcc, 0xf0, 0xe5, 0x58, 0x30, 0xe3, 0x17, 0xe5, 0x25, 0x70, 0x13, 
    0xe5, 0x3c, 0xf4, 0x90, 0x13, 0x2a, 0x60, 0x05, 0xe0, 0x54, 0xf3, 0x80, 0x11, 0xe0, 0x54, 0xfb, 
    0xf0, 0x80, 0x14, 0xe5, 0x3c, 0xf4, 0x90, 0x13, 0x2a, 0x60, 0x08, 0xe0, 0x54, 0xf2, 0x45, 0x3c, 
    0xf0, 0x80, 0x04, 0xe0, 0x54, 0xfa, 0xf0, 0x90, 0x04, 0x01, 0xe0, 0x44, 0x10, 0xf0, 0x75, 0x8c, 
    0x80, 0xe0, 0x54, 0xfd, 0xf0, 0x90, 0x12, 0x04, 0xe0, 0x44, 0x04, 0xf0, 0xe5, 0x58, 0x30, 0xe0, 
    0x06, 0x90, 0x01, 0x0d, 0xe0, 0xf5, 0x24, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x17, 0x20, 0x02, 
    0x14, 0x20, 0x03, 0x11, 0x30, 0x4f, 0x0e, 0x90, 0x01, 0x0d, 0xe0, 0x54, 0xfb, 0xf0, 0x90, 0x01, 
    0x0c, 0xe0, 0x54, 0xfd, 0xf0, 0x75, 0x30, 0x01, 0x75, 0x55, 0x02, 0xe4, 0xf5, 0x51, 0x80, 0x09, 
    0xe5, 0x50, 0x70, 0x05, 0x75, 0x30, 0x03, 0xf5, 0x51, 0xe5, 0x30, 0x60, 0x15, 0xc2, 0x01, 0xe4, 
    0xf5, 0x51, 0xc2, 0x59, 0xad, 0x30, 0xaf, 0x40, 0x12, 0x0d, 0x42, 0xe5, 0x30, 0xb4, 0x03, 0x02, 
    0xd2, 0x03, 0xd2, 0xaf, 0x22, 0xc2, 0xaf, 0x30, 0x01, 0x0e, 0xe4, 0xf5, 0x51, 0xc2, 0x59, 0xc2, 
    0x01, 0x7d, 0x02, 0xaf, 0x40, 0x12, 0x0d, 0x42, 0xe5, 0x52, 0x14, 0x60, 0x48, 0x14, 0x60, 0x25, 
    0x24, 0x02, 0x60, 0x03, 0x02, 0x0a, 0x5b, 0xe5, 0x25, 0xd3, 0x94, 0x01, 0x40, 0x11, 0x90, 0x01, 
    0x0c, 0xe0, 0x44, 0x02, 0xf0, 0xa3, 0xe0, 0x44, 0x04, 0xf0, 0x7f, 0x0a, 0x12, 0x0e, 0xe9, 0x75, 
    0x52, 0x02, 0x75, 0x55, 0x03, 0xe5, 0x58, 0x30, 0xe0, 0x06, 0x90, 0x01, 0x0d, 0xe5, 0x24, 0xf0, 
    0x90, 0x12, 0x04, 0xe0, 0x54, 0xfb, 0xf0, 0x7f, 0x20, 0x12, 0x00, 0x0e, 0x75, 0x52, 0x01, 0x75, 
    0x55, 0x03, 0x02, 0x0a, 0x5b, 0xe5, 0x54, 0x60, 0x03, 0x02, 0x0a, 0x5b, 0x90, 0x04, 0x01, 0xe0, 
    0x44, 0x0e, 0xf0, 0xe0, 0x54, 0xef, 0xf0, 0xe4, 0xf5, 0x8c, 0xe5, 0x58, 0x54, 0x18, 0x60, 0x1e, 
    0x90, 0x70, 0x1a, 0xe0, 0x90, 0x13, 0x28, 0xf0, 0x90, 0x70, 0x1b, 0xe0, 0x90, 0x13, 0x29, 0xf0, 
    0xa3, 0x74, 0x05, 0xf0, 0x90, 0x70, 0x22, 0xe0, 0x90, 0x13, 0x2b, 0xf0, 0x80, 0x11, 0x90, 0x13, 
    0x28, 0xe0, 0x44, 0x0f, 0xf0, 0xa3, 0xe0, 0x44, 0x0f, 0xf0, 0xa3, 0xe0, 0x44, 0x05, 0xf0, 0x90, 
    0x12, 0x04, 0x74, 0x03, 0xf0, 0xe5, 0x58, 0x30, 0xe3, 0x1d, 0x90, 0x00, 0x02, 0xe0, 0x30, 0xe0, 
    0x16, 0x90, 0x05, 0x00, 0x74, 0xe2, 0xf0, 0xa3, 0x74, 0x08, 0xf0, 0xa3, 0x74, 0x01, 0xf0, 0x74, 
    0x03, 0xf0, 0x7f, 0x01, 0x12, 0x0e, 0xe9, 0x90, 0x02, 0x08, 0xe0, 0x44, 0x05, 0xf0, 0x90, 0x10, 
    0x04, 0xe0, 0x44, 0x0c, 0xf0, 0xe4, 0xf5, 0x52, 0xf5, 0x55, 0x30, 0x02, 0x09, 0xc2, 0x02, 0x7d, 
    0x01, 0xaf, 0x41, 0x12, 0x0d, 0x42, 0x30, 0x03, 0x02, 0xc2, 0x03, 0xd2, 0xaf, 0x22, 0xc2, 0x4b, 
    0xc2, 0x4c, 0xe5, 0x44, 0x12, 0x0d, 0xc5, 0x0a, 0x80, 0x00, 0x0b, 0x0e, 0x04, 0x0b, 0x0a, 0x08, 
    0x0a, 0xea, 0x10, 0x0a, 0x94, 0x20, 0x0a, 0xb4, 0x60, 0x0a, 0xc5, 0xa0, 0x00, 0x00, 0x0b, 0x10, 
    0x85, 0x48, 0x43, 0x85, 0x4a, 0x42, 0x85, 0x4c, 0x5e, 0xe5, 0x47, 0x64, 0x06, 0x60, 0x03, 0x02, 
    0x0b, 0x10, 0x80, 0x1b, 0xe5, 0x48, 0xc4, 0x54, 0x0f, 0xf5, 0x43, 0xe5, 0x4a, 0xc4, 0x54, 0x0f, 
    0xf5, 0x42, 0xe5, 0x4c, 0xc4, 0x54, 0x0f, 0xf5, 0x5e, 0xe5, 0x47, 0x64, 0x06, 0x70, 0x61, 0x53, 
    0x43, 0x0f, 0x80, 0x5c, 0x85, 0x49, 0x43, 0x85, 0x4b, 0x42, 0x85, 0x4d, 0x5e, 0xe5, 0x47, 0x64, 
    0x06, 0x70, 0x4d, 0x80, 0x1b, 0xe5, 0x49, 0xc4, 0x54, 0x0f, 0xf5, 0x43, 0xe5, 0x4b, 0xc4, 0x54, 
    0x0f, 0xf5, 0x42, 0xe5, 0x4d, 0xc4, 0x54, 0x0f, 0xf5, 0x5e, 0xe5, 0x47, 0x64, 0x06, 0x70, 0x30, 
    0xe5, 0x43, 0x54, 0x0f, 0x44, 0x10, 0xf5, 0x43, 0x80, 0x26, 0xe5, 0x47, 0x64, 0x04, 0x60, 0x05, 
    0xe5, 0x47, 0xb4, 0x05, 0x06, 0x43, 0x5e, 0x04, 0x75, 0x42, 0x09, 0xe5, 0x47, 0xb4, 0x06, 0x10, 
    0xe5, 0x43, 0x54, 0x0f, 0x44, 0x30, 0xf5, 0x43, 0x80, 0x06, 0xd2, 0x4b, 0x80, 0x02, 0xd2, 0x4c, 
    0xe4, 0xf5, 0x2a, 0xe5, 0x42, 0xc4, 0x54, 0xf0, 0xff, 0xe5, 0x43, 0x54, 0x0f, 0x4f, 0xf5, 0x5f, 
    0xd2, 0x60, 0x22, 0xc0, 0xe0, 0xc0, 0xf0, 0xc0, 0x83, 0xc0, 0x82, 0xc0, 0xd0, 0x75, 0xd0, 0x18, 
    0xc2, 0xaf, 0x30, 0x45, 0x03, 0x12, 0x0f, 0x57, 0x90, 0x04, 0x16, 0xe0, 0x30, 0xe3, 0x03, 0x74, 
    0x08, 0xf0, 0x90, 0x04, 0x14, 0xe0, 0x20, 0xe7, 0x03, 0x02, 0x0b, 0xd0, 0x74, 0x80, 0xf0, 0x90, 
    0x70, 0x12, 0xe0, 0xf5, 0x2f, 0x90, 0x04, 0x04, 0xe0, 0x24, 0xcf, 0x60, 0x30, 0x14, 0x60, 0x42, 
    0x24, 0xe2, 0x60, 0x47, 0x14, 0x60, 0x55, 0x24, 0x21, 0x70, 0x60, 0xe5, 0x55, 0x24, 0xfe, 0x60, 
    0x07, 0x14, 0x60, 0x08, 0x24, 0x02, 0x70, 0x08, 0x7d, 0x01, 0x80, 0x28, 0x7d, 0x02, 0x80, 0x24, 
    0x90, 0x70, 0x10, 0xe0, 0xf5, 0x50, 0x85, 0x2f, 0x40, 0xd2, 0x01, 0x80, 0x3e, 0xe5, 0x55, 0x64, 
    0x03, 0x60, 0x04, 0xe5, 0x55, 0x70, 0x04, 0x7d, 0x02, 0x80, 0x09, 0x85, 0x2f, 0x41, 0xd2, 0x02, 
    0x80, 0x29, 0xad, 0x55, 0xaf, 0x2f, 0x12, 0x0d, 0xeb, 0x80, 0x20, 0x90, 0x70, 0x10, 0xe0, 0xf5, 
    0x47, 0x90, 0x70, 0x11, 0xe0, 0xf5, 0x44, 0x12, 0x0f, 0x48, 0x80, 0x06, 0x90, 0x70, 0x10, 0xe0, 
    0xf5, 0x45, 0xe4, 0xfd, 0xaf, 0x2f, 0x12, 0x0d, 0xeb, 0xd2, 0x04, 0x90, 0x70, 0x13, 0xe4, 0xf0, 
    0x90, 0x70, 0x13, 0xe4, 0xf0, 0xd2, 0xaf, 0xd0, 0xd0, 0xd0, 0x82, 0xd0, 0x83, 0xd0, 0xf0, 0xd0, 
    0xe0, 0x32, 0xc0, 0xe0, 0xc0, 0xf0, 0xc0, 0x83, 0xc0, 0x82, 0xc0, 0xd0, 0xe8, 0xc0, 0xe0, 0xe9, 
    0xc0, 0xe0, 0xea, 0xc0, 0xe0, 0xeb, 0xc0, 0xe0, 0xec, 0xc0, 0xe0, 0xed, 0xc0, 0xe0, 0xee, 0xc0, 
    0xe0, 0xef, 0xc0, 0xe0, 0xc2, 0xaf, 0x30, 0x45, 0x03, 0x12, 0x0f, 0x60, 0xd2, 0xaf, 0xd0, 0xe0, 
    0xff, 0xd0, 0xe0, 0xfe, 0xd0, 0xe0, 0xfd, 0xd0, 0xe0, 0xfc, 0xd0, 0xe0, 0xfb, 0xd0, 0xe0, 0xfa, 
    0xd0, 0xe0, 0xf9, 0xd0, 0xe0, 0xf8, 0xd0, 0xd0, 0xd0, 0x82, 0xd0, 0x83, 0xd0, 0xf0, 0xd0, 0xe0, 
    0x32, 0xc0, 0xe0, 0xc0, 0xf0, 0xc0, 0x83, 0xc0, 0x82, 0xc0, 0xd0, 0x75, 0xd0, 0x10, 0xc2, 0xaf, 
    0x30, 0x45, 0x03, 0x12, 0x0f, 0x5a, 0x30, 0x58, 0x0a, 0xe5, 0x54, 0x60, 0x04, 0x15, 0x54, 0x80, 
    0x02, 0xc2, 0x58, 0x30, 0x59, 0x0a, 0xe5, 0x50, 0x60, 0x04, 0x15, 0x50, 0x80, 0x02, 0xc2, 0x59, 
    0xd5, 0x53, 0x07, 0x30, 0x60, 0x04, 0x15, 0x46, 0xd2, 0x04, 0x30, 0x45, 0x03, 0x12, 0x0f, 0x5d, 
    0xc2, 0x8d, 0xd2, 0xaf, 0xd0, 0xd0, 0xd0, 0x82, 0xd0, 0x83, 0xd0, 0xf0, 0xd0, 0xe0, 0x32, 0x90, 
    0x70, 0x2a, 0xe0, 0x30, 0xe1, 0x43, 0xc2, 0xaf, 0x90, 0x70, 0x28, 0xe0, 0x90, 0x10, 0x1c, 0xf0, 
    0x90, 0x70, 0x29, 0xe0, 0x90, 0x10, 0x1d, 0xf0, 0x90, 0x70, 0x2a, 0xe0, 0x90, 0x10, 0x1e, 0xf0, 
    0x90, 0x10, 0x1c, 0xe0, 0xf5, 0x30, 0x90, 0x10, 0x1e, 0xe0, 0x20, 0xe1, 0xf3, 0x90, 0x10, 0x1c, 
    0xe0, 0x90, 0x70, 0x28, 0xf0, 0x90, 0x10, 0x1d, 0xe0, 0x90, 0x70, 0x29, 0xf0, 0x90, 0x10, 0x1e, 
    0xe0, 0x90, 0x70, 0x2a, 0xf0, 0xc2, 0x05, 0xd2, 0xaf, 0x22, 0x12, 0x0e, 0x8c, 0x30, 0x45, 0x03, 
    0x12, 0x0f, 0x51, 0x30, 0x01, 0x06, 0x20, 0x09, 0x03, 0x12, 0x0f, 0x4b, 0x30, 0x02, 0x06, 0x20, 
    0x0a, 0x03, 0x12, 0x0f, 0x4e, 0x30, 0x03, 0x06, 0x20, 0x0b, 0x03, 0x12, 0x0f, 0x4e, 0x30, 0x04, 
    0x06, 0x20, 0x0c, 0x03, 0x12, 0x00, 0x16, 0x20, 0x13, 0x09, 0x20, 0x11, 0x06, 0xe5, 0x2b, 0x45, 
    0x2c, 0x60, 0x03, 0xd3, 0x80, 0x01, 0xc3, 0x92, 0xa9, 0x12, 0x0e, 0xd5, 0x80, 0xbf, 0x30, 0x14, 
    0x30, 0x90, 0x70, 0x19, 0xe0, 0x55, 0x27, 0xff, 0x90, 0x70, 0x18, 0xe0, 0x4f, 0xf5, 0x27, 0x90, 
    0x02, 0x29, 0xe0, 0xff, 0x90, 0x70, 0x19, 0xe0, 0xfe, 0xef, 0x5e, 0x90, 0x02, 0x29, 0xf0, 0x30, 
    0x47, 0x04, 0xaf, 0x27, 0x80, 0x04, 0xe5, 0x27, 0xf4, 0xff, 0x90, 0x02, 0x28, 0xef, 0xf0, 0xc2, 
    0x14, 0x22, 0xef, 0xf4, 0x60, 0x2d, 0xe4, 0xfe, 0x74, 0x14, 0x2e, 0xf5, 0x82, 0xe4, 0x34, 0x70, 
    0xf5, 0x83, 0xe0, 0xb4, 0xff, 0x19, 0x74, 0x14, 0x2e, 0xf5, 0x82, 0xe4, 0x34, 0x70, 0xf5, 0x83, 
    0xef, 0xf0, 0x74, 0x1c, 0x2e, 0xf5, 0x82, 0xe4, 0x34, 0x70, 0xf5, 0x83, 0xed, 0xf0, 0x22, 0x0e, 
    0xbe, 0x04, 0xd5, 0x22, 0xe5, 0x47, 0xb4, 0x0b, 0x10, 0x90, 0x02, 0x29, 0xe0, 0x54, 0xeb, 0xf0, 
    0xe5, 0x27, 0x54, 0xeb, 0x45, 0x45, 0xf5, 0x27, 0x22, 0xe4, 0x90, 0x02, 0x29, 0xf0, 0x30, 0x47, 
    0x04, 0xaf, 0x45, 0x80, 0x04, 0xe5, 0x45, 0xf4, 0xff, 0x90, 0x02, 0x28, 0xef, 0xf0, 0x22, 0xc2, 
    0x43, 0xd2, 0x45, 0xe4, 0xf5, 0x20, 0xf5, 0x21, 0xf5, 0x53, 0xf5, 0x46, 0xf5, 0x2b, 0xf5, 0x2c, 
    0xc2, 0x42, 0xf5, 0x51, 0xf5, 0x52, 0xf5, 0x55, 0x90, 0x04, 0x18, 0x74, 0x80, 0xf0, 0x90, 0x04, 
    0x1a, 0x74, 0x08, 0xf0, 0x22, 0xd0, 0x83, 0xd0, 0x82, 0xf8, 0xe4, 0x93, 0x70, 0x12, 0x74, 0x01, 
    0x93, 0x70, 0x0d, 0xa3, 0xa3, 0x93, 0xf8, 0x74, 0x01, 0x93, 0xf5, 0x82, 0x88, 0x83, 0xe4, 0x73, 
    0x74, 0x02, 0x93, 0x68, 0x60, 0xef, 0xa3, 0xa3, 0xa3, 0x80, 0xdf, 0xef, 0xf4, 0x60, 0x1f, 0xe4, 
    0xfe, 0x12, 0x0f, 0x28, 0xe0, 0xb4, 0xff, 0x12, 0x12, 0x0f, 0x28, 0xef, 0xf0, 0x74, 0x1c, 0x2e, 
    0xf5, 0x82, 0xe4, 0x34, 0x70, 0xf5, 0x83, 0xed, 0xf0, 0x22, 0x0e, 0xbe, 0x04, 0xe3, 0x22, 0xc0, 
    0xe0, 0xc0, 0xf0, 0xc0, 0x83, 0xc0, 0x82, 0xc0, 0xd0, 0x75, 0xd0, 0x08, 0xc2, 0xaf, 0x30, 0x45, 
    0x03, 0x12, 0x0f, 0x54, 0xd2, 0xaf, 0xd0, 0xd0, 0xd0, 0x82, 0xd0, 0x83, 0xd0, 0xf0, 0xd0, 0xe0, 
    0x32, 0xd2, 0x15, 0xe5, 0x47, 0x24, 0xf5, 0x60, 0x0b, 0x24, 0xcb, 0x60, 0x07, 0x24, 0x40, 0x70, 
    0x06, 0xc2, 0x15, 0x22, 0x12, 0x0d, 0x74, 0x12, 0x04, 0x92, 0xc2, 0x15, 0xc2, 0xaf, 0xc2, 0x04, 
    0xd2, 0xaf, 0x22, 0xe5, 0x53, 0x70, 0x1a, 0x30, 0x60, 0x09, 0xb2, 0x4d, 0x30, 0x4d, 0x04, 0x05, 
    0x46, 0xc2, 0x04, 0xe5, 0x4f, 0x45, 0x4e, 0x60, 0x08, 0xe5, 0x4f, 0x15, 0x4f, 0x70, 0x02, 0x15, 
    0x4e, 0x22, 0x90, 0x10, 0x1c, 0xed, 0xf0, 0xa3, 0xef, 0xf0, 0xa3, 0x74, 0x0a, 0xf0, 0x90, 0x10, 
    0x1c, 0xe0, 0xf5, 0x58, 0x90, 0x10, 0x1e, 0xe0, 0x20, 0xe1, 0xf3, 0x22, 0xc2, 0xaf, 0x12, 0x0e, 
    0xff, 0x12, 0x0d, 0x9f, 0x12, 0x0e, 0xbe, 0xe4, 0xf5, 0x22, 0xf5, 0x47, 0x90, 0x04, 0x00, 0x74, 
    0x80, 0xf0, 0xd2, 0xaf, 0x22, 0x90, 0x10, 0x1d, 0xef, 0xf0, 0xa3, 0x74, 0x0b, 0xf0, 0x90, 0x10, 
    0x1c, 0xe0, 0xf5, 0x58, 0x90, 0x10, 0x1e, 0xe0, 0x20, 0xe1, 0xf3, 0xaf, 0x58, 0x22, 0x75, 0x89, 
    0x02, 0xe4, 0xf5, 0x8c, 0xf5, 0x8a, 0xf5, 0x88, 0xf5, 0xb8, 0xf5, 0xe8, 0x75, 0x90, 0x18, 0xd2, 
    0x8c, 0x75, 0xa8, 0x05, 0x22, 0x30, 0x45, 0x03, 0x12, 0x0f, 0x63, 0xe5, 0x20, 0x70, 0x03, 0x20, 
    0x10, 0x03, 0x30, 0x11, 0x03, 0x43, 0x87, 0x01, 0x22, 0xce, 0xef, 0xce, 0xee, 0x60, 0x08, 0x7f, 
    0xff, 0x12, 0x0e, 0xf8, 0x1e, 0x80, 0xf5, 0x22, 0xef, 0x60, 0x03, 0x1f, 0x80, 0xfa, 0x22, 0x22, 
    0xc0, 0x26, 0x74, 0x0f, 0xc0, 0xe0, 0xc0, 0x82, 0xc0, 0x83, 0x75, 0x26, 0x0a, 0x22, 0xc0, 0x26, 
    0x74, 0x0f, 0xc0, 0xe0, 0xc0, 0x82, 0xc0, 0x83, 0x75, 0x26, 0x18, 0x22, 0x78, 0x7f, 0xe4, 0xf6, 
    0xd8, 0xfd, 0x75, 0x81, 0x5f, 0x02, 0x0c, 0xca, 0x74, 0x14, 0x2e, 0xf5, 0x82, 0xe4, 0x34, 0x70, 
    0xf5, 0x83, 0x22, 0xef, 0x90, 0x0f, 0x3c, 0x93, 0x90, 0x0f, 0x00, 0x73, 0x0a, 0x18, 0x30, 0x05, 
    0x06, 0x20, 0x0d, 0x03, 0x12, 0x0c, 0x7f, 0x22, 0x02, 0x0a, 0x5e, 0x02, 0x07, 0xee, 0x02, 0x09, 
    0x65, 0x02, 0x0f, 0x3e, 0x02, 0x0f, 0x67, 0x02, 0x00, 0x1e, 0x02, 0x0f, 0x68, 0x02, 0x0e, 0x53, 
    0x02, 0x0f, 0x69, 0x02, 0x0d, 0x0e, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0xac, 0x12, 
};


#define FIRMWAREIMAGE_LENGTH_4K                     (sizeof (FirmwareImage4K) / sizeof(UCHAR))
#define FIRMWARE_MAJOR_VERSION_4K     0
#define FIRMWARE_MINOR_VERSION_4K     34

const unsigned short ccitt_16Table[] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};
#define ByteCRC16(v, crc) \
	(unsigned short)((crc << 8) ^  ccitt_16Table[((crc >> 8) ^ (v)) & 255])

unsigned char BitReverse(unsigned char x)
{
	int i;
	unsigned char Temp=0;
	for(i=0; ; i++)
	{
		if(x & 0x80)	Temp |= 0x80;
		if(i==7)		break;
		x	<<= 1;
		Temp >>= 1;
	}
	return Temp;
}

// Unify all delay routine by using NdisStallExecution
__inline    VOID    RTMPusecDelay(
    IN      ULONG   usec)
{
	ULONG   i;

	for (i = 0; i < (usec / 50); i++)
		KeStallExecutionProcessor(50);

	if (usec % 50)
		KeStallExecutionProcessor(usec % 50);
}

//#pragma code_seg("INIT") // start INIT section

//++
// Function:	DriverEntry
//
// Description:
//		Initializes the driver.
//
// Arguments:
//		pDriverObject - Passed from I/O Manager
//		pRegistryPath - UNICODE_STRING pointer to
//						registry info (service key)
//						for this driver
//
// Return value:
//		NTSTATUS signaling success or failure
//--
NTSTATUS DriverEntry (
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath	
	) 
{
	ULONG ulDeviceNumber = 0;
	NTSTATUS status = STATUS_SUCCESS;	

	DBGPRINT(RT_DEBUG_TRACE, ("====> DriverEntry\n"));
	DBGPRINT(RT_DEBUG_TRACE,("================================================\n"));
	DBGPRINT(RT_DEBUG_TRACE,("Build Date: "__DATE__" Time: "__TIME__"\n"));
	DBGPRINT(RT_DEBUG_TRACE,("================================================\n"));

#ifdef _USB

	//pDriverObject->MajorFunction[IRP_MJ_CLEANUP]        = BulkUsb_DispatchClean;
	//pDriverObject->MajorFunction[IRP_MJ_READ]           =
   	//pDriverObject->MajorFunction[IRP_MJ_WRITE]          = BulkUsb_DispatchReadWrite;
	pDriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = BulkUsb_DispatchSysCtrl;
#endif

	// Announce other driver entry points
	pDriverObject->DriverUnload = DriverUnload;

	pDriverObject->DriverStartIo = StartIo;

	// Announce the PNP AddDevice entry point
	pDriverObject->DriverExtension->AddDevice =	AddDevice;

	// Announce the PNP Major Function entry point
	pDriverObject->MajorFunction[IRP_MJ_PNP] = DispPnp;

	// This includes Dispatch routines for Create, Write & Read
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
       pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;

	pDriverObject->MajorFunction[IRP_MJ_POWER] = Wdm2Power; //DispPower;

	// Notice that no device objects are created by DriverEntry.
	// Instead, we await the PnP call to AddDevice

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DriverEntry <=== \n"));

	return status;
}
//#pragma code_seg() // end INIT section

//++
// Function:	AddDevice
//
// Description:
//	Called by the PNP Manager when a new device is
//	detected on a bus.  The responsibilities include
//	creating an FDO, device name, and symbolic link.
//
// Arguments:
//	pDriverObject - Passed from PNP Manager
//	pdo		    - pointer to Physcial Device Object
//				 passed from PNP Manager
//
// Return value:
//	NTSTATUS signaling success or failure
//--
NTSTATUS AddDevice (	IN PDRIVER_OBJECT pDriverObject,
						IN PDEVICE_OBJECT pDo	) 
{
	NTSTATUS status = 0;
	PDEVICE_OBJECT pfdo;
	RTMP_ADAPTER *pDevExt;
	static int ulDeviceNumber = 0;
	UCHAR				resBuf[CM_RESOURCE_BUF_SIZE];
	PCM_RESOURCE_LIST	resList = (PCM_RESOURCE_LIST)resBuf;
	ULONG				bufSize = CM_RESOURCE_BUF_SIZE;
	PCM_FULL_RESOURCE_DESCRIPTOR  	pResDesc;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR	pPartialResDesc;
	ULONG				ResultbutSize;
	ULONG				index,inner;
#ifdef _USB
	RTMP_ADAPTER *deviceExtension;
    	POWER_STATE       state;
    	KIRQL             oldIrql;

	UNREFERENCED_PARAMETER( oldIrql );
#endif

	g_FWSeqUI = 0;
	g_FWSeqMCU = 0;
	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: sys minusPCICSR0 AddDevice ===> \n"));

//	DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: AddDevice; current DeviceNumber = %d\n",
//				ulDeviceNumber);


#ifdef _USB
	// TODO: FILE_DEVICE_SECURE_OPEN or FILE_AUTOGENERATED_DEVICE_NAME
	//
	status = IoCreateDevice(
					pDriverObject,                   // our driver object
					sizeof(RTMP_ADAPTER),//sizeof(MTKDRV_EXTENSION),       // extension size for us
					NULL,                           // name for this device
					FILE_DEVICE_UNKNOWN,
					FILE_DEVICE_SECURE_OPEN, //FILE_AUTOGENERATED_DEVICE_NAME, // device characteristics
					FALSE,                          // Not exclusive
					&pfdo);   
	//RtlZeroMemory(pfdo->DeviceExtension, sizeof(MTKDRV_EXTENSION));
	RtlZeroMemory(pfdo->DeviceExtension, sizeof(RTMP_ADAPTER));

    //
    // Initialize the device extension
    //

    deviceExtension = (RTMP_ADAPTER *) pfdo->DeviceExtension;
    deviceExtension->FunctionalDeviceObject = pfdo;
    deviceExtension->PhysicalDeviceObject = pDo;
    RtlZeroMemory(&deviceExtension->OtherCounters, sizeof(deviceExtension->OtherCounters));
    // default buffer mode
    deviceExtension->EfuseMode= MODE_BUFFER;
    RtlZeroMemory(&deviceExtension->EfuseContent, EFUSESIZE);
    deviceExtension->IsTxSetFrequency = FALSE;
	//FW
	deviceExtension->LoadingFWCount = 0;
	RtlZeroMemory(&deviceExtension->FWImage, MAXFIRMWARESIZE);
	deviceExtension->FWSize = 0;
	deviceExtension->IsFWImageInUse = FALSE;
	deviceExtension->IsUISetFW = FALSE;
	deviceExtension->bIsWaitFW = FALSE;
	deviceExtension->MACVersion = 0;
	deviceExtension->FwCmdSeqNum = 0;
	deviceExtension->RomPatchSemStatus = 0xFF;
	
    pfdo->Flags |= DO_DIRECT_IO;

    //
    // initialize the device state lock and set the device state
    //

    KeInitializeSpinLock(&deviceExtension->DevStateLock);
    INITIALIZE_PNP_STATE(deviceExtension);

    //
    //initialize OpenHandleCount
    //
    deviceExtension->OpenHandleCount = 0;

    //
    // Initialize the selective suspend variables
    //
    KeInitializeSpinLock(&deviceExtension->IdleReqStateLock);
    deviceExtension->IdleReqPend = 0;
    deviceExtension->PendingIdleIrp = NULL;

    //
    // Hold requests until the device is started
    //

    deviceExtension->QueueState = HoldRequests;

    //
    // Initialize the queue and the queue spin lock
    //

    InitializeListHead(&deviceExtension->NewRequestsQueue);
    KeInitializeSpinLock(&deviceExtension->QueueLock);

    //
    // Initialize the remove event to not-signaled.
    //

    KeInitializeEvent(&deviceExtension->RemoveEvent, 
                      SynchronizationEvent, 
                      FALSE);

    //
    // Initialize the stop event to signaled.
    // This event is signaled when the OutstandingIO becomes 1
    //

    KeInitializeEvent(&deviceExtension->StopEvent, 
                      SynchronizationEvent, 
                      TRUE);

    //
    // OutstandingIo count biased to 1.
    // Transition to 0 during remove device means IO is finished.
    // Transition to 1 means the device can be stopped
    //

    deviceExtension->OutStandingIO = 1;
    KeInitializeSpinLock(&deviceExtension->IOCountLock);

    //
    // Delegating to WMILIB
    //:TODO:   don't do this???
    //ntStatus = BulkUsb_WmiRegistration(deviceExtension);

    //if(!NT_SUCCESS(ntStatus)) {
//	 DBGPRINT(RT_DEBUG_TRACE,"BulkUsb_WmiRegistration failed with %X\n", ntStatus);
    //    IoDeleteDevice(deviceObject);
    //    return ntStatus;
    //}

    //
    // set the flags as underlying PDO
    //

    if(pDo->Flags & DO_POWER_PAGABLE) {

        pfdo->Flags |= DO_POWER_PAGABLE;
    }

    //
    // Typically, the function driver for a device is its 
    // power policy owner, although for some devices another 
    // driver or system component may assume this role. 
    // Set the initial power state of the device, if known, by calling 
    // PoSetPowerState.
    // 

    deviceExtension->DevPower = PowerDeviceD0;
    deviceExtension->SysPower = PowerSystemWorking;

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState(pfdo, DevicePowerState, state);

    //
    // attach our driver to device stack
    // The return value of IoAttachDeviceToDeviceStack is the top of the
    // attachment chain.  This is where all the IRPs should be routed.
    //

    deviceExtension->TopOfStackDeviceObject = 
                IoAttachDeviceToDeviceStack(pfdo,
                                            pDo);
    //:TODO:   don't do this???
    //if(NULL == deviceExtension->TopOfStackDeviceObject) {
//
    //    BulkUsb_WmiDeRegistration(deviceExtension);
    //    IoDeleteDevice(deviceObject);
    //    return STATUS_NO_SUCH_DEVICE;
    //}
        
    //
    // Register device interfaces
    //

    status = IoRegisterDeviceInterface(deviceExtension->PhysicalDeviceObject, 
                                         &GUID_QA_MT7603U, 
                                         NULL, 
                                         &deviceExtension->InterfaceName);

    if(!NT_SUCCESS(status)) {

        //BulkUsb_WmiDeRegistration(deviceExtension); //:TODO:   don't do this???
        IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
        IoDeleteDevice(pfdo);
        return status;
    }

	//event for load fw
	KeInitializeEvent(&deviceExtension->LoadFWEvent,
                      SynchronizationEvent,     // auto-clearing event
                      FALSE);                   // event initially non-signalled
	deviceExtension->bLoadingFW = FALSE;
	//event for fw bulkout
	KeInitializeEvent(&deviceExtension->WaitFWEvent,
                      SynchronizationEvent,     // auto-clearing event
                      FALSE);                   // event initially non-signalled
	
	//event for Rom Patch bulkout
	KeInitializeEvent(&deviceExtension->RomPatchEvent,
                      SynchronizationEvent,     // auto-clearing event
                      FALSE);                   // event initially non-signalled					  
	
#else
	//PCIe
	// Now create the device
	status = IoCreateDevice( 
						pDriverObject,
						sizeof(RTMP_ADAPTER),
						NULL,
						FILE_DEVICE_UNKNOWN,
						0, 
						FALSE,
						&pfdo 
						);

	if (!NT_SUCCESS(status))
		return status;

	// Choose to use BUFFERED_IO
	pfdo->Flags |= DO_BUFFERED_IO;

	// Initialize the Device Extension
	pDevExt = (RTMP_ADAPTER *)pfdo->DeviceExtension;
	pDevExt->pDeviceObject = pfdo;	// back pointer
	
	pDevExt->pPhyDeviceObj = pDo;
	
	pDevExt->DeviceNumber = ulDeviceNumber;
	pDevExt->pIntObj = NULL;
	pDevExt->state = Stopped;
	pDevExt->EfuseMode = MODE_BUFFER;
	RtlZeroMemory(&pDevExt->EfuseContent, EFUSESIZE);
	pDevExt->IsTxSetFrequency = FALSE;
	pDevExt->bRxEnable[BULKIN_PIPENUMBER_0] = FALSE;
	pDevExt->bRxEnable[BULKIN_PIPENUMBER_1] = FALSE;
	pDevExt->bBulkOutRunning = FALSE;

	pDevExt->ulBulkInRunning[0] = 0;
	pDevExt->ulBulkInRunning[1] = 0;
	
	//-----------------------------------------	
	// Initialize Sniffer Spin Lock
	KeInitializeSpinLock(&pDevExt->SnifferSpinLock);

	//-----------------------------------------	
	// Initialize MPDU Spin Lock
	KeInitializeSpinLock(&pDevExt->MPDUSpinLock);
	
	//-----------------------------------------	
	// Initialize Rx Spin Lock
	KeInitializeSpinLock(&pDevExt->RxSpinLock[0]);
	KeInitializeSpinLock(&pDevExt->RxSpinLock[1]);
	
	//-----------------------------------------	
	// Initialize IoCtrl Semaphore
	KeInitializeSemaphore(&pDevExt->IoCtrlSemaphore, 1, 1);
	//--<

	
	// Register and enable our device interface
	status = IoRegisterDeviceInterface(pDo, &GUID_QA_MT7603E, NULL, &pDevExt->ustrSymLinkName);
	if( !NT_SUCCESS(status))
	{
		IoDeleteDevice(pfdo);
		return status;
	}

	// Form the symbolic link name
	IoSetDeviceInterfaceState(&pDevExt->ustrSymLinkName, TRUE);

	// Pile this new fdo on top of the existing lower stack
	pDevExt->pLowerDevice =		// downward pointer
		IoAttachDeviceToDeviceStack( pfdo, pDo);

	if (!pDevExt->pLowerDevice) 
	{
		// if it fails now, must delete Device object
		IoDeleteDevice( pfdo );
		return status;
	}

	status=IoGetDeviceProperty(
			pDo,
			DevicePropertyBootConfigurationTranslated ,
			bufSize,
			resList,			
			&ResultbutSize
    );
	if (status==STATUS_BUFFER_TOO_SMALL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: STATUS_BUFFER_TOO_SMALL \n") );
	}
	else if (status==STATUS_INVALID_PARAMETER_2)
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: STATUS_INVALID_PARAMETER_2 \n") );
	}
	else if (status==STATUS_INVALID_DEVICE_REQUEST)
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: STATUS_INVALID_DEVICE_REQUEST \n") );
	}
	else if( NT_SUCCESS(status))	
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: STATUS=%d \n",status));
	}
	else if( !NT_SUCCESS(status))	
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: !NT_SUCCESS(status)=%d \n",status));
	}
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: resList->Count=%d \n",resList->Count));

	// We need a DpcForIsr registration
	IoInitializeDpcRequest( pfdo, RTPCIDpcForIsr );

	

	// Made it
	ulDeviceNumber++;

	status = STATUS_SUCCESS;
#endif

	//  Clear the Device Initializing bit since the FDO was created
	//  outside of DriverEntry.
	pfdo->Flags &= ~DO_DEVICE_INITIALIZING;
	

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: AddDevice <=== \n"));

	return status;
}


#ifdef RTMP_PCI_SUPPORT
NTSTATUS DispPower(IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp ) 
{
	// obtain current IRP stack location
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)(pDO->DeviceExtension);
	PIO_STACK_LOCATION pIrpStack;
	pIrpStack = IoGetCurrentIrpStackLocation( pIrp );

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DispPower IRP: %d\n", pIrpStack->MinorFunction));

	PoStartNextPowerIrp(pIrp);
	IoSkipCurrentIrpStackLocation(pIrp);

	return PoCallDriver(pDevExt->pLowerDevice, pIrp);
}

NTSTATUS PassDownPnP( IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp ) 
{	
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;

	IoSkipCurrentIrpStackLocation( pIrp );
	
	return IoCallDriver(pDevExt->pLowerDevice, pIrp);
}


NTSTATUS HandleStopDevice(	IN PDEVICE_OBJECT pDO,
							IN PIRP pIrp ) 
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: HandleStopDevice ===> \n"));

	pDevExt->state = Stopped;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: HandleStopDevice <=== \n"));
	return PassDownPnP(pDO, pIrp);
}
#endif /* RTMP_PCI_SUPPORT */


#ifdef _USB
NTSTATUS DispPnpUSB(IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp)
{
	 PIO_STACK_LOCATION irpStack;
    RTMP_ADAPTER *deviceExtension;
//    KEVENT             startDeviceEvent;
    NTSTATUS           ntStatus = 0;
	//
    // initialize variables
    //

    irpStack = IoGetCurrentIrpStackLocation(pIrp);
    deviceExtension = (RTMP_ADAPTER *)pDO->DeviceExtension;
	//
	// since the device is removed, fail the Irp.
	//
	DBGPRINT(RT_DEBUG_TRACE,("USB %s  MinorFunction=0x%x ==>\n",__FUNCTION__,irpStack->MinorFunction));
	if(USBRemoved == deviceExtension->DeviceState) 
	{

	        ntStatus = STATUS_DELETE_PENDING;

	        pIrp->IoStatus.Status = ntStatus;
	        pIrp->IoStatus.Information = 0;

	        IoCompleteRequest(pIrp, IO_NO_INCREMENT);

		return ntStatus;
	}
	BulkUsb_IoIncrement(deviceExtension);
	if(irpStack->MinorFunction == IRP_MN_START_DEVICE) 
	{
        	ASSERT(deviceExtension->IdleReqPend == 0);
	}
	else 
	{
		/*if(deviceExtension->SSEnable) 
		{
	            CancelSelectSuspend(deviceExtension);
		}*/
	}

	switch(irpStack->MinorFunction) 
	{

    		case IRP_MN_START_DEVICE:
			
		        ntStatus = HandleStartDevice(pDO, pIrp);

		        break;

		case IRP_MN_QUERY_STOP_DEVICE:

		        //
		        // if we cannot stop the device, we fail the query stop irp
		        //

		        ntStatus = CanStopDevice(pDO, pIrp);

		        if(NT_SUCCESS(ntStatus)) {

		            ntStatus = HandleQueryStopDevice(pDO, pIrp);

		            return ntStatus;
		        }
		        break;

		case IRP_MN_CANCEL_STOP_DEVICE:

		        ntStatus = HandleCancelStopDevice(pDO, pIrp);

		        //BulkUsb_DbgPrint(3, ("BulkUsb_DispatchPnP::IRP_MN_CANCEL_STOP_DEVICE::"));
		        BulkUsb_IoDecrement(deviceExtension);

		        return ntStatus;
		     
		case IRP_MN_STOP_DEVICE:

#ifdef RTMP_PCI_SUPPORT
			// TODO: How about USB device???
		        ntStatus = HandleStopDevice(pDO, pIrp);
#endif /* RTMP_PCI_SUPPORT */
			
		        //BulkUsb_DbgPrint(3, ("BulkUsb_DispatchPnP::IRP_MN_STOP_DEVICE::"));
		        BulkUsb_IoDecrement(deviceExtension);

		        return ntStatus;

		case IRP_MN_QUERY_REMOVE_DEVICE:

		        //
		        // if we cannot remove the device, we fail the query remove irp
		        //
		        ntStatus = HandleQueryRemoveDevice(pDO, pIrp);

		        return ntStatus;

		case IRP_MN_CANCEL_REMOVE_DEVICE:

		        ntStatus = HandleCancelRemoveDevice(pDO, pIrp);

		        //BulkUsb_DbgPrint(3, ("BulkUsb_DispatchPnP::IRP_MN_CANCEL_REMOVE_DEVICE::"));
		        BulkUsb_IoDecrement(deviceExtension);

		        return ntStatus;

		case IRP_MN_SURPRISE_REMOVAL:

		        ntStatus = HandleSurpriseRemoval(pDO, pIrp);

		        //BulkUsb_DbgPrint(3, ("BulkUsb_DispatchPnP::IRP_MN_SURPRISE_REMOVAL::"));
		        BulkUsb_IoDecrement(deviceExtension);

		        return ntStatus;

		case IRP_MN_REMOVE_DEVICE:

		        ntStatus = HandleRemoveDevice(pDO, pIrp);

		        return ntStatus;

		case IRP_MN_QUERY_CAPABILITIES:

		        ntStatus = HandleQueryCapabilities(pDO, pIrp);

		        break;

		default:

		        IoSkipCurrentIrpStackLocation(pIrp);

		        ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, pIrp);

		       DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchPnP::default::"));
		        BulkUsb_IoDecrement(deviceExtension);

		        return ntStatus;

		    } // switch

	//
	// complete request 
	//

	    pIrp->IoStatus.Status = ntStatus;
	    pIrp->IoStatus.Information = 0;

	    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	//
	// decrement count
	//
	   DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchPnP::"));
	    BulkUsb_IoDecrement(deviceExtension);

	return ntStatus;

}
#endif /* _USB */


#ifdef RTMP_PCI_SUPPORT
NTSTATUS DispPnpPCI(	IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp ) 
{
	// obtain current IRP stack location
	PIO_STACK_LOCATION pIrpStack;
	pIrpStack = IoGetCurrentIrpStackLocation( pIrp );

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Received PNP IRP: %d\n",
				pIrpStack->MinorFunction));


	switch (pIrpStack->MinorFunction) 
	{
	case IRP_MN_START_DEVICE:
		return HandleStartDevice(pDO, pIrp );
	case IRP_MN_STOP_DEVICE:
		return HandleStopDevice( pDO, pIrp );
	case IRP_MN_REMOVE_DEVICE:
		return HandleRemoveDevice( pDO, pIrp );
	default:
		// if not supported here, just pass it down
		return PassDownPnP(pDO, pIrp);
	}

	// all paths from the switch statement will "return"
	// the results of the handler invoked
}
#endif /* RTMP_PCI_SUPPORT */


NTSTATUS DispPnp(	IN PDEVICE_OBJECT pDO,
					IN PIRP pIrp ) 
{
#ifdef _USB
	return DispPnpUSB(pDO, pIrp);
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT
	return DispPnpPCI(pDO, pIrp);
#endif /* RTMP_PCI_SUPPORT */

	return 0;
}


NTSTATUS HandleStartDevice(	IN PDEVICE_OBJECT pDO,
							IN PIRP pIrp ) 
{
#ifdef _USB
    KIRQL             oldIrql;
    KEVENT            startDeviceEvent;
    NTSTATUS          ntStatus = 0;
    RTMP_ADAPTER *deviceExtension;
    LARGE_INTEGER     dueTime;

    //
    // initialize variables
    //
    deviceExtension = (RTMP_ADAPTER *) pDO->DeviceExtension;
    deviceExtension->UsbConfigurationDescriptor = NULL;
    deviceExtension->UsbInterface = NULL;
    deviceExtension->PipeContext = NULL;

    //
    // We cannot touch the device (send it any non pnp irps) until a
    // start device has been passed down to the lower drivers.
    // first pass the Irp down
    //

    KeInitializeEvent(&startDeviceEvent, NotificationEvent, FALSE);

    IoCopyCurrentIrpStackLocationToNext(pIrp);

    IoSetCompletionRoutine(pIrp, 
                           IrpCompletionRoutine, 
                           (PVOID)&startDeviceEvent, 
                           TRUE, 
                           TRUE, 
                           TRUE);

    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, pIrp);

    if(ntStatus == STATUS_PENDING) {

        KeWaitForSingleObject(&startDeviceEvent, 
                              Executive, 
                              KernelMode, 
                              FALSE, 
                              NULL);

        ntStatus = pIrp->IoStatus.Status;
    }
    if(!NT_SUCCESS(ntStatus)) 
    {
        BulkUsb_DbgPrint(1, ("Lower drivers failed this Irp\n"));
        return ntStatus;
    }

    //
    // Read the device descriptor, configuration descriptor 
    // and select the interface descriptors
    //

    ntStatus = ReadandSelectDescriptors(pDO);

    if(!NT_SUCCESS(ntStatus)) 
    {

        //BulkUsb_DbgPrint(1, ("ReadandSelectDescriptors failed\n"));
        return ntStatus;
    }

     //
    // enable the symbolic links for system components to open
    // handles to the device
    //

    ntStatus = IoSetDeviceInterfaceState(&deviceExtension->InterfaceName, 
                                         TRUE);

    if(!NT_SUCCESS(ntStatus)) {

        BulkUsb_DbgPrint(1, ("IoSetDeviceInterfaceState:enable:failed\n"));
        return ntStatus;
    }

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

    SET_NEW_PNP_STATE(deviceExtension, USBWorking);
    deviceExtension->QueueState = AllowRequests;

    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

    //
    // initialize wait wake outstanding flag to false.
    // and issue a wait wake.
    
    deviceExtension->FlagWWOutstanding = 0;
    deviceExtension->FlagWWCancel = 0;
    deviceExtension->WaitWakeIrp = NULL;
    
    /*if(deviceExtension->WaitWakeEnable) {

        IssueWaitWake(deviceExtension);
    }*/

    ProcessQueuedRequests(deviceExtension);

    if(WinXpOrBetter == deviceExtension->WdmVersion) {


        //deviceExtension->SSEnable = deviceExtension->SSRegistryEnable;

        //
        // set timer.for selective suspend requests
        //

       /* if(deviceExtension->SSEnable) {

            dueTime.QuadPart = -10000 * IDLE_INTERVAL;               // 5000 ms

            KeSetTimerEx(&deviceExtension->Timer, 
                         dueTime,
                         IDLE_INTERVAL,                              // 5000 ms
                         &deviceExtension->DeferredProcCall);

            deviceExtension->FreeIdleIrpCount = 0;
        }*/
    }
	ntStatus = NICInit(deviceExtension);
	DBGPRINT(RT_DEBUG_TRACE, ("HandleStartDevice - ends\n"));
	return ntStatus;
#else
//PCIe

	PCM_RESOURCE_LIST				pResourceList;
	PCM_FULL_RESOURCE_DESCRIPTOR	pFullDescriptor;
	PCM_PARTIAL_RESOURCE_LIST		pPartialList;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR pPartialDescriptor;
	NTSTATUS						status;
	int								i, AddressCount = 0;
	// The stack location contains the Parameter info
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: HandleStartDevice ===> \n"));
	
	pResourceList = pIrpStack->Parameters.StartDevice.AllocatedResourcesTranslated;
	pFullDescriptor = pResourceList->List;
	pPartialList = &pFullDescriptor->PartialResourceList;
	for (i=0; i<(int)pPartialList->Count; i++)
	{
		pPartialDescriptor = &pPartialList->PartialDescriptors[i];
		
		switch (pPartialDescriptor->Type) 
		{
		case CmResourceTypeInterrupt:
			DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: CmResourceTypeInterrupt \n"));
			
			pDevExt->IRQL = (KIRQL)	pPartialDescriptor->u.Interrupt.Level;
			pDevExt->InterruptLevel = pPartialDescriptor->u.Interrupt.Level;
			pDevExt->Vector = pPartialDescriptor->u.Interrupt.Vector;
			pDevExt->Affinity = pPartialDescriptor->u.Interrupt.Affinity;			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Claiming Interrupt Resources: "
					 "InterruptLevel=%d, IRQ=%d Vector=0x%03X Affinity=%X\n",
					 pDevExt->InterruptLevel ,pDevExt->IRQL, pDevExt->Vector, pDevExt->Affinity));
#ifdef MSI
			{
			//MSI
				// deviceExtension is a pointer to the driver's device extension. 
				//     deviceExtension->IntObj is a PKINTERRUPT.
				// deviceInterruptService is a pointer to the driver's InterruptService routine.
				// IntResource is a CM_PARTIAL_RESOURCE_DESCRIPTOR structure of either type CmResourceTypeInterrupt or CmResourceTypeMessageInterrupt.
				// PhysicalDeviceObject is a pointer to the device's PDO. 
				// ServiceContext is a pointer to driver-specified context for the ISR.

				RtlZeroMemory( &pDevExt->params, sizeof(IO_CONNECT_INTERRUPT_PARAMETERS) );
				pDevExt->params.Version = CONNECT_FULLY_SPECIFIED;
				pDevExt->params.FullySpecified.PhysicalDeviceObject = pDevExt->pPhyDeviceObj;
				pDevExt->params.FullySpecified.InterruptObject = (PKINTERRUPT*)&pDevExt->pConnectionContext;
				pDevExt->params.FullySpecified.ServiceRoutine = Isr;
				pDevExt->params.FullySpecified.ServiceContext = pDevExt;
				pDevExt->params.FullySpecified.FloatingSave = FALSE;
				pDevExt->params.FullySpecified.SpinLock = NULL;

				if (pPartialDescriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) 
				{
					// The resource is for a message-based interrupt. Use the u.MessageInterrupt.Translated member of IntResource.

					pDevExt->params.FullySpecified.Vector = pPartialDescriptor->u.MessageInterrupt.Translated.Vector;
					pDevExt->params.FullySpecified.Irql = (KIRQL)pPartialDescriptor->u.MessageInterrupt.Translated.Level;
					pDevExt->params.FullySpecified.SynchronizeIrql = (KIRQL)pPartialDescriptor->u.MessageInterrupt.Translated.Level;
					pDevExt->params.FullySpecified.ProcessorEnableMask = pPartialDescriptor->u.MessageInterrupt.Translated.Affinity;
				} 
				else 
				{
					// The resource is for a line-based interrupt. Use the u.Interrupt member of IntResource.

					pDevExt->params.FullySpecified.Vector = pPartialDescriptor->u.Interrupt.Vector;
					pDevExt->params.FullySpecified.Irql = (KIRQL)pPartialDescriptor->u.Interrupt.Level;
					pDevExt->params.FullySpecified.SynchronizeIrql = (KIRQL)pPartialDescriptor->u.Interrupt.Level;
					pDevExt->params.FullySpecified.ProcessorEnableMask = pPartialDescriptor->u.Interrupt.Affinity;
				}

				pDevExt->params.FullySpecified.InterruptMode = (pPartialDescriptor->Flags & CM_RESOURCE_INTERRUPT_LATCHED ? Latched : LevelSensitive);
				pDevExt->params.FullySpecified.ShareVector = (BOOLEAN)(pPartialDescriptor->ShareDisposition == CmResourceShareShared);
			}
#endif
			break;

		case CmResourceTypeDma:
			DBGPRINT(RT_DEBUG_TRACE,("\n\n\nPciDumpr: CmResourceTypeDma\n\n\n"));
			break;

		case CmResourceTypePort:
			DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: CmResourceTypePort \n"));			
			pDevExt->ExtendPciAddress.PciAddress[AddressCount].TranslateAddress = pPartialDescriptor->u.Port.Start.LowPart;
			pDevExt->ExtendPciAddress.PciAddress[AddressCount].ShareMemorySize = pPartialDescriptor->u.Port.Length;
			AddressCount ++;
			DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: pDevExt->ExtendPciAddress.PciAddress[AddressCount].TranslateAddress = %x\n",pDevExt->ExtendPciAddress.PciAddress[AddressCount].TranslateAddress));
			DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: pDevExt->ExtendPciAddress.PciAddress[AddressCount].ShareMemorySize = %x\n",pDevExt->ExtendPciAddress.PciAddress[AddressCount].ShareMemorySize));
			break;

		case CmResourceTypeMemory:			
			{
			PHYSICAL_ADDRESS PhyAddress = pPartialDescriptor->u.Memory.Start;

			DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: CmResourceTypeMemory  call MmMapIoSpace\n"));			
			
			pDevExt->MemorySize = pPartialDescriptor->u.Memory.Length;
			
			pDevExt->virtualaddr = (PULONG)MmMapIoSpace(PhyAddress, pDevExt->MemorySize, MmNonCached);

			DBGPRINT(RT_DEBUG_TRACE,("\nPhyAddress = 0x%08x \n",PhyAddress));
			DBGPRINT(RT_DEBUG_TRACE,("\npDevExt->MemorySize = 0x%08x \n",pDevExt->MemorySize));
			DBGPRINT(RT_DEBUG_TRACE,("\npDevExt->virtualaddr = 0x%08x \n",pDevExt->virtualaddr));
			

			pDevExt->ExtendPciAddress.PciAddress[AddressCount].TranslateAddress = pPartialDescriptor->u.Memory.Start.LowPart;
			pDevExt->ExtendPciAddress.PciAddress[AddressCount].ShareMemorySize = pPartialDescriptor->u.Memory.Length;
			pDevExt->ExtendPciAddress.PciAddress[AddressCount].MapIoAddress = *pDevExt->virtualaddr;

			AddressCount ++;
			}
			break;
		}
	}
	pDevExt->ExtendPciAddress.Count = AddressCount;	
	
	// Create & connect to an Interrupt object
	if (pDevExt->IRQL == 0 )
	{
		return STATUS_BIOS_FAILED_TO_CONNECT_INTERRUPT;	
	}

#ifdef MSI
	if (pDevExt->bConnectedInterrupt == FALSE)
	{
		status = IoConnectInterruptEx(&pDevExt->params);
		
		//Save the type of interrup connected. We'll use this later when we need to disconnect from the interrupt.
		pDevExt->TypeOfInterruptVersion = pDevExt->params.Version;

		pDevExt->bConnectedInterrupt = TRUE;
	}
#else
	{
		status = IoConnectInterrupt(
				&pDevExt->pIntObj,				// the Interrupt object
				Isr,							// our ISR
				pDevExt,						// Service Context
				NULL,							// no spin lock
				pDevExt->Vector,				// vector
				pDevExt->IRQL,					// DIRQL
				pDevExt->IRQL,					// DIRQL
				LevelSensitive,					// Latched or LevelSensitive
				TRUE,							// Shared?
				pDevExt->Affinity,				// processors in an MP set
				FALSE							// save FP registers? 
				);
	}
#endif

	if(status == STATUS_INVALID_PARAMETER)
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: STATUS_INVALID_PARAMETER \n"));
	}

	if(status == STATUS_INSUFFICIENT_RESOURCES)
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: STATUS_INSUFFICIENT_RESOURCES \n"));
	}

	if (!NT_SUCCESS(status)) 
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Interrupt connection failure: %X\n", status));
		return status;
	}
	
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Interrupt successfully connected\n"));

	AllocateDMAMemory(pDO);

#ifdef SNIFF
	pDevExt->pPacketBuf = ExAllocatePoolWithTag(
											NonPagedPool,
											(RX_RING_PACKET_BUFFER * MAX_FRAME_SIZE),
											'yroR');
	if(pDevExt->pPacketBuf)
		RtlZeroMemory(pDevExt->pPacketBuf,(RX_RING_PACKET_BUFFER * MAX_FRAME_SIZE));
	else
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Error! PacketBuf not allocated\n"));	
#endif

	// init comTxBuffer
	pDevExt->comTxBuffer.pComTxBuf = (PUCHAR)ExAllocatePoolWithTag(
											NonPagedPool,
											MAX_COM_BUFFER_SIZE,
											'yroR');
	if(pDevExt->comTxBuffer.pComTxBuf)
		RtlZeroMemory(pDevExt->comTxBuffer.pComTxBuf, MAX_COM_BUFFER_SIZE);
	else
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Error! pComTxBuf not allocated\n"));

	pDevExt->comTxBuffer.TxInsertPointer = 0;
	pDevExt->comTxBuffer.TxRemovePointer = 0;
	pDevExt->comTxBuffer.TxFreeCount	 = MAX_COM_BUFFER_SIZE;

	// init comRxBuffer
	pDevExt->comRxBuffer.pComRxBuf = (PUCHAR)ExAllocatePoolWithTag(
											NonPagedPool,
											MAX_COM_BUFFER_SIZE,
											'yroR');
	if(pDevExt->comRxBuffer.pComRxBuf)
		RtlZeroMemory(pDevExt->comRxBuffer.pComRxBuf, MAX_COM_BUFFER_SIZE);
	else
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Error! pComRxBuf not allocated\n"));

	pDevExt->comRxBuffer.RxInsertPointer = 0;
	pDevExt->comRxBuffer.RxRemovePointer = 0;
	pDevExt->comRxBuffer.RxFreeCount	 = MAX_COM_BUFFER_SIZE;

	{
		LARGE_INTEGER  CurrentTime;

		KeQuerySystemTime(&CurrentTime);
		LfsrInit(pDO, CurrentTime.LowPart);
	}

	pDevExt->state = Started;	
	//RTMPPatchCardBus(pDevExt);
		
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: HandleStartDevice <=== \n\n\n"));	
	return PassDownPnP(pDO, pIrp);

#endif
}

NTSTATUS
ReadWriteConfigSpace(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG          ReadOrWrite, // 0 for read 1 for write
    IN PVOID          Buffer,
    IN ULONG          Offset,
    IN ULONG          Length
    )
{
    KEVENT event;
    NTSTATUS status;
    PIRP irp;
    IO_STATUS_BLOCK ioStatusBlock;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_OBJECT targetObject;

    PAGED_CODE();
    KeInitializeEvent( &event, NotificationEvent, FALSE );
    targetObject = IoGetAttachedDeviceReference( DeviceObject );
    irp = IoBuildSynchronousFsdRequest( IRP_MJ_PNP,
                                        targetObject,
                                        NULL,
                                        0,
                                        NULL,
                                        &event,
                                        &ioStatusBlock );
    if (irp == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto End;
    }
    irpStack = IoGetNextIrpStackLocation( irp );
    if (ReadOrWrite == 0) {
        irpStack->MinorFunction = IRP_MN_READ_CONFIG;
    }else {
        irpStack->MinorFunction = IRP_MN_WRITE_CONFIG;
    }
    irpStack->Parameters.ReadWriteConfig.WhichSpace = PCI_WHICHSPACE_CONFIG;
    irpStack->Parameters.ReadWriteConfig.Buffer = Buffer;
    irpStack->Parameters.ReadWriteConfig.Offset = Offset;
    irpStack->Parameters.ReadWriteConfig.Length = Length;
    // Initialize the status to error in case the bus driver does not 
    // set it correctly.
    irp->IoStatus.Status = STATUS_NOT_SUPPORTED ;
    status = IoCallDriver( targetObject, irp );
    if (status == STATUS_PENDING) {
        KeWaitForSingleObject( &event, Executive, KernelMode, FALSE, NULL );
        status = ioStatusBlock.Status;
    }
End:
    // Done with reference
    ObDereferenceObject( targetObject );
    return status;
}


NTSTATUS HandleRemoveDevice(IN PDEVICE_OBJECT pDO,
							IN PIRP pIrp ) 
{
#ifdef _USB
	KIRQL             oldIrql;
//    KEVENT            event;
    ULONG             requestCount;
    NTSTATUS          ntStatus;
    RTMP_ADAPTER *deviceExtension;

   DBGPRINT(RT_DEBUG_TRACE, ("HandleRemoveDevice - begins\n"));

    //
    // initialize variables
    //

    deviceExtension = (RTMP_ADAPTER *) pDO->DeviceExtension;

    //
    // The Plug & Play system has dictated the removal of this device.  We
    // have no choice but to detach and delete the device object.
    // (If we wanted to express an interest in preventing this removal,
    // we should have failed the query remove IRP).
    //

    if(USBSurpriseRemoved != deviceExtension->DeviceState) {

        //
        // we are here after QUERY_REMOVE
        //

        KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

        deviceExtension->QueueState = FailRequests;
        
        KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);

        /*if(deviceExtension->WaitWakeEnable) {
        
            CancelWaitWake(deviceExtension);
        }*/

        if(WinXpOrBetter == deviceExtension->WdmVersion) {

            /*if(deviceExtension->SSEnable) {

                //
                // Cancel the timer so that the DPCs are no longer fired.
                // we do not need DPCs because the device has been removed
                //            
                KeCancelTimer(&deviceExtension->Timer);

                deviceExtension->SSEnable = 0;

                //
                // make sure that if a DPC was fired before we called cancel timer,
                // then the DPC and work-time have run to their completion
                //
                KeWaitForSingleObject(&deviceExtension->NoDpcWorkItemPendingEvent, 
                                      Executive, 
                                      KernelMode, 
                                      FALSE, 
                                      NULL);

                //
                // make sure that the selective suspend request has been completed.
                //  
                KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent, 
                                      Executive, 
                                      KernelMode, 
                                      FALSE, 
                                      NULL);
            }*/
        }
	UsbBulkMemoryFree(pDO);

        ProcessQueuedRequests(deviceExtension);

        ntStatus = IoSetDeviceInterfaceState(&deviceExtension->InterfaceName, 
                                             FALSE);

        if(!NT_SUCCESS(ntStatus)) {

            BulkUsb_DbgPrint(1, ("IoSetDeviceInterfaceState::disable:failed\n"));
        }

        RtlFreeUnicodeString(&deviceExtension->InterfaceName);

        BulkUsb_WmiDeRegistration(deviceExtension);

        BulkUsb_AbortPipes(pDO);
    }

    KeAcquireSpinLock(&deviceExtension->DevStateLock, &oldIrql);

    SET_NEW_PNP_STATE(deviceExtension, USBRemoved);
    
    KeReleaseSpinLock(&deviceExtension->DevStateLock, oldIrql);
    
    //
    // need 2 decrements
    //

   DBGPRINT(RT_DEBUG_TRACE, ("HandleRemoveDevice::"));
    requestCount = BulkUsb_IoDecrement(deviceExtension);

    ASSERT(requestCount > 0);

   DBGPRINT(RT_DEBUG_TRACE, ("HandleRemoveDevice::"));
    requestCount = BulkUsb_IoDecrement(deviceExtension);

    KeWaitForSingleObject(&deviceExtension->RemoveEvent, 
                          Executive, 
                          KernelMode, 
                          FALSE, 
                          NULL);

    ReleaseMemory(pDO);
    //
    // We need to send the remove down the stack before we detach,
    // but we don't need to wait for the completion of this operation
    // (and to register a completion routine).
    //

    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;

    IoSkipCurrentIrpStackLocation(pIrp);
    ntStatus = IoCallDriver(deviceExtension->TopOfStackDeviceObject, pIrp);

    //
    // Detach the FDO from the device stack
    //
    IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
    IoDeleteDevice(pDO);

   DBGPRINT(RT_DEBUG_TRACE, ("HandleRemoveDevice - ends\n"));

    return ntStatus;
#else

	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: HandleRemoveDevice ===> \n"));

//	if (IS_RT3290(pDevExt->MACVersion))
		RTMPEnableWlan(pDevExt, FALSE); // For MT7650
	
	if(pDevExt->DmaStatus == TRUE)
	{
		FreeDMAMemory(pDO);
		pDevExt->DmaStatus = FALSE;
	}

#ifdef SNIFF
	if(pDevExt->pPacketBuf)
		ExFreePool(pDevExt->pPacketBuf);
#endif

	if (pDevExt->state == Started) 
	{		
#ifdef MSI
		if (pDevExt->bConnectedInterrupt == TRUE)
		{
			IO_DISCONNECT_INTERRUPT_PARAMETERS discParams;
			RtlZeroMemory(&discParams, sizeof(IO_DISCONNECT_INTERRUPT_PARAMETERS));
			discParams.Version = pDevExt->TypeOfInterruptVersion;
			discParams.ConnectionContext.Generic = pDevExt->pConnectionContext;
			IoDisconnectInterruptEx(&discParams);
			pDevExt->bConnectedInterrupt = FALSE;
		}
#else
		if (pDevExt->pIntObj)		
		{
//			DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: PCI Interrrupt disable! \n"));
			
			IoDisconnectInterrupt( pDevExt->pIntObj );			//disconnect interrupt
		}
#endif
		if(pDevExt->virtualaddr)
		{
			MmUnmapIoSpace(pDevExt->virtualaddr,pDevExt->MemorySize);
			pDevExt->virtualaddr = NULL;
		}
	}

	// disable device interface
	IoSetDeviceInterfaceState(&pDevExt->ustrSymLinkName, FALSE);
	RtlFreeUnicodeString(&pDevExt->ustrSymLinkName);	
		
	if (pDevExt->pLowerDevice)		IoDetachDevice(pDevExt->pLowerDevice);	// unattach from stack
	
	IoDeleteDevice( pDO );								// Delete the device

	pDevExt->state = Removed;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: HandleRemoveDevice <=== \n"));
	return PassDownPnP( pDO, pIrp );

#endif//USB and PCIe
}


VOID StartIo(
				IN PDEVICE_OBJECT pDO,
				IN PIRP pIrp )
{
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: StartIo ===> \n"));
}
//++
// Function:	DriverUnload
//
// Description:
//		Stops & Deletes devices controlled by this driver.
//		Stops interrupt processing (if any)
//		Releases kernel resources consumed by driver
//
// Arguments:
//		pDriverObject - Passed from I/O Manager
//
// Return value:
//		None
//--

VOID DriverUnload ( IN PDRIVER_OBJECT	pDriverObject ) 
{
#ifdef _USB
 	DBGPRINT(RT_DEBUG_TRACE,("USB DriverUnload==>\n"));
 	/*PUNICODE_STRING registryPath;
	UNREFERENCED_PARAMETER( pDriverObject );
	PAGED_CODE();
	registryPath = &Globals.BulkUsb_RegistryPath;

	if(registryPath->Buffer) 
	{
       	ExFreePool(registryPath->Buffer);
       	registryPath->Buffer = NULL;
	}*/

	
#else
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DriverUnload\n"));

#endif
}

//++
// Function:	DispatchCreate
//
// Description:
//		Handles call from Win32 CreateFile request
//		Does nothing
//
// Arguments:
//		pDevObj - Passed from I/O Manager
//		pIrp - Passed from I/O Manager
//
// Return value:
//		NTSTATUS - success or failure code
//--
//LONG *g_pTestSSEnable;
NTSTATUS DispatchCreate (	IN PDEVICE_OBJECT	pDO,
							IN PIRP				pIrp ) 
{	
#ifdef _USB
	ULONG                       i;
       NTSTATUS                    status = STATUS_SUCCESS;
       PFILE_OBJECT                fileObject;
       RTMP_ADAPTER *deviceExtension;
       PIO_STACK_LOCATION          irpStack;
       PBULKUSB_PIPE_CONTEXT       pipeContext;
       PUSBD_INTERFACE_INFORMATION interface;
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)pDO->DeviceExtension;
	LARGE_INTEGER CurrentTime;
       PAGED_CODE();

	//
       // initialize variables
       //
       irpStack = IoGetCurrentIrpStackLocation(pIrp);
       fileObject = irpStack->FileObject;
       deviceExtension = (RTMP_ADAPTER *) pDO->DeviceExtension;
	DBGPRINT(RT_DEBUG_TRACE,("USB: DispatchCreate\n"));
	if(deviceExtension->DeviceState != USBWorking) 
	{
	        status = STATUS_INVALID_DEVICE_STATE;
       	 goto BulkUsb_DispatchCreate_Exit;
   	}

	if(deviceExtension->UsbInterface) 
	{
        	interface = deviceExtension->UsbInterface;
    	}
    	else 
	{
	      status = STATUS_INVALID_DEVICE_STATE;
	      goto BulkUsb_DispatchCreate_Exit;
	}

	//
       // FsContext is Null for the device
       //
       if(fileObject) 
	{        
        	fileObject->FsContext = NULL; 
       }
       else 
	{
	        status = STATUS_INVALID_PARAMETER;
	        goto BulkUsb_DispatchCreate_Exit;
       }

	//-->
	// init Variable, RTMP_ADAPTER has been initialized in AddDevice function. Just set variable which not zero.
	dx->ulSetNorFlashSizeType = NOR_FLASH_SIZE_TYPE_8_16_BIT;	

	dx->FwReqSequence = 1;

	//-----------------------------------------
	// init Sniffer group	
	dx->RxAggregate = TRUE;	

	KeQuerySystemTime(&CurrentTime);
	LfsrInit(dx, CurrentTime.LowPart);
	//-----------------------------------------	
	// Initialize Bulk Memory///::TODO
	//g_pTestSSEnable = &deviceExtension->SSEnable;
	
#if 0
	if(UsbBulkMemoryAlloc(dx) == STATUS_INSUFFICIENT_RESOURCES)
		goto BulkUsb_DispatchCreate_Exit;	

	dx->bBulkOutRunning = FALSE;

	dx->ulBulkInRunning[0] = 0;
	dx->ulBulkInRunning[1] = 0;
	
	//-----------------------------------------	
	// Initialize Sniffer Spin Lock
	KeInitializeSpinLock(&dx->SnifferSpinLock);

	//-----------------------------------------	
	// Initialize MPDU Spin Lock
	KeInitializeSpinLock(&dx->MPDUSpinLock);
	
	//-----------------------------------------	
	// Initialize Rx Spin Lock
	KeInitializeSpinLock(&dx->RxSpinLock[0]);
	KeInitializeSpinLock(&dx->RxSpinLock[1]);
	
	//-----------------------------------------	
	// Initialize IoCtrl Semaphore
	KeInitializeSemaphore(&dx->IoCtrlSemaphore, 1, 1);
	//--<
#endif
	if(0 == fileObject->FileName.Length)
	{
	        //
	        // opening a device as opposed to pipe.
	        //
	        status = STATUS_SUCCESS;

	        InterlockedIncrement(&deviceExtension->OpenHandleCount);

	        //
	        // the device is idle if it has no open handles or pending PnP Irps
	        // since we just received an open handle request, cancel idle req.
	        //
	        /*if(deviceExtension->SSEnable) 
		 {        
	            CancelSelectSuspend(deviceExtension);
	        }*/

	        goto BulkUsb_DispatchCreate_Exit;
       }
    
      pipeContext = BulkUsb_PipeWithName(pDO, &fileObject->FileName);
  
      if(pipeContext == NULL) 
      {
  
          status = STATUS_INVALID_PARAMETER;
          goto BulkUsb_DispatchCreate_Exit;
      }
  
  	status = STATUS_INVALID_PARAMETER;
  
      for(i=0; i<interface->NumberOfPipes; i++) 
      {
  
          if(pipeContext == &deviceExtension->PipeContext[i]) 
  	{
  
              //
              // found a match
              //
              //BulkUsb_DbgPrint(3, ("open pipe %d\n", i));
  
              fileObject->FsContext = &interface->Pipes[i];
              
              ASSERT(fileObject->FsContext);
  
              pipeContext->PipeOpen = TRUE;
  
              status = STATUS_SUCCESS;
  
              //
              // increment OpenHandleCounts
              //
              InterlockedIncrement(&deviceExtension->OpenHandleCount);
  
              //
              // the device is idle if it has no open handles or pending PnP Irps
              // since we just received an open handle request, cancel idle req.
              //
             /* if(deviceExtension->SSEnable) 
  	    {
  
                  CancelSelectSuspend(deviceExtension);
              }*/
          }
      }
	

BulkUsb_DispatchCreate_Exit:

    pIrp->IoStatus.Status = status;
    pIrp->IoStatus.Information = 0;

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return status;
#else//PCIe
	NTSTATUS status = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
	DbgPrint("DispatchCreate");
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DispatchCreate ===>\n"));	
	
	if (pDevExt->state != Started)		
		status = STATUS_DEVICE_REMOVED;

	DriverInit(pDO); // 050624 From DispatchCreate move here

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;	
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DispatchCreate <===\n"));
	return status;

#endif
}

//++
// Function:	DispatchClose
//
// Description:
//		Handles call from Win32 CreateHandle request
//		For this driver, frees any buffer
//
// Arguments:
//		pDevObj - Passed from I/O Manager
//		pIrp - Passed from I/O Manager
//
// Return value:
//		NTSTATUS - success or failure code
//--

NTSTATUS DispatchClose (	IN PDEVICE_OBJECT	pDO,
							IN PIRP				pIrp ) 
{
#ifdef _USB
	NTSTATUS               ntStatus;
    PFILE_OBJECT           fileObject;
    RTMP_ADAPTER *deviceExtension;
    PIO_STACK_LOCATION     irpStack;
    PBULKUSB_PIPE_CONTEXT  pipeContext;
    PUSBD_PIPE_INFORMATION pipeInformation;
    
    PAGED_CODE();

    //
    // initialize variables
    //
    irpStack = IoGetCurrentIrpStackLocation(pIrp);
    fileObject = irpStack->FileObject;
    pipeContext = NULL;
    pipeInformation = NULL;
    deviceExtension = (RTMP_ADAPTER *) pDO->DeviceExtension;

   DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchClose - begins\n"));
    	
    if(fileObject && fileObject->FsContext) {

        pipeInformation = (PUSBD_PIPE_INFORMATION)fileObject->FsContext;

        if(0 != fileObject->FileName.Length) {

            pipeContext = BulkUsb_PipeWithName(pDO, 
                                               &fileObject->FileName);
        }

        if(pipeContext && pipeContext->PipeOpen) {
            
            pipeContext->PipeOpen = FALSE;
        }
    }

    //
    // set ntStatus to STATUS_SUCCESS 
    //
    ntStatus = STATUS_SUCCESS;

    pIrp->IoStatus.Status = ntStatus;
    pIrp->IoStatus.Information = 0;

    //UsbBulkMemoryFree(pDO);

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    InterlockedDecrement(&deviceExtension->OpenHandleCount);

   DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchClose - ends\n"));

    return ntStatus;
#endif /* _USB */

#ifdef RTMP_PCI_SUPPORT
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DispatchClose ===>\n"));

	if (pDevExt->deviceBuffer != NULL) {
		ExFreePool(pDevExt->deviceBuffer);
		pDevExt->deviceBuffer = NULL;
		pDevExt->deviceBufferSize = 0;
	}

	//CSR1: System control register host ready disable
//	WRITE_Dummy_REGISTER_ULONG(((PULONG)pDevExt->virtualaddr + CSR1 / 4),0);

	// Disable interrupt

	pDevExt->IntrMask = (IRQ_MASK)&0xFFFFFFFF;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, pDevExt->IntrMask);//|IRQ_MSK_SPECIAL_BITS);		

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;	
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );

//	 KeCancelTimer(&pDevExt->m_IsrTimer);
	NICIssueReset(pDevExt);
	
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DispatchClose <===\n"));
	return STATUS_SUCCESS;
#endif /* RTMP_PCI_SUPPORT */
}


//++
// Function:	DispatchCancel
//
// Description:
//		Handles canceled IRP
//
// Arguments:
//		pDevObj - Passed from I/O Manager
//		pIrp - Passed from I/O Manager
//
// Return value:
//		NTSTATUS - success or failuer code
//--

VOID DispatchCancel (	IN PDEVICE_OBJECT	pDevObj,
						IN PIRP				pIrp )
{
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IRP Canceled\n"));
	
	pIrp->IoStatus.Status = STATUS_CANCELLED;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );

	IoStartNextPacket( pDevObj, TRUE );
}


#ifdef _USB
NTSTATUS DispatchDeviceControlUSB(IN PDEVICE_OBJECT DeviceObject,
									IN PIRP Irp)
{
	PIO_STACK_LOCATION  irpStack;
	PULONG              pIBuffer;
	ULONG               inputBufferLength;
	PVOID               ioBuffer;
	ULONG               outputBufferLength;
	ULONG               ioControlCode;
	ULONG               byteCount = 0;
	NTSTATUS            ntStatus;

	    ULONG              code;
	    ULONG              info;
           ULONG InputLength = 0;
           ULONG OutputLength = 0;
	    RTMP_ADAPTER *deviceExtension;
	    //RTMP_ADAPTER *dx = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;

	    //
	    // initialize variables
	    //
	    info = 0;
	    irpStack = IoGetCurrentIrpStackLocation(Irp);
	    code = irpStack->Parameters.DeviceIoControl.IoControlCode;
	    InputLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
           OutputLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
	    deviceExtension = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

	    DBGPRINT(RT_DEBUG_TRACE,("DeviceIoControl: Control code %x InputLength %d OutputLength %d\n",
				code, InputLength, OutputLength));

	    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
	    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
	    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
	    DBGPRINT(RT_DEBUG_TRACE,("USB: DispatchDeviceControl\n"));
	    if(deviceExtension->DeviceState != USBWorking) {

	        BulkUsb_DbgPrint(1, ("Invalid device state\n"));

	        Irp->IoStatus.Status = ntStatus = STATUS_INVALID_DEVICE_STATE;
	        Irp->IoStatus.Information = info;

	        IoCompleteRequest(Irp, IO_NO_INCREMENT);
	        return ntStatus;
	    }

          DBGPRINT(RT_DEBUG_TRACE, ("BulkUsb_DispatchDevCtrl::"));
	    BulkUsb_IoIncrement(deviceExtension);

	    //
	    // It is true that the client driver cancelled the selective suspend
	    // request in the dispatch routine for create.
	    // But there is no guarantee that it has indeed been completed.
	    // so wait on the NoIdleReqPendEvent and proceed only if this event
	    // is signalled.
	    //
	   DBGPRINT(RT_DEBUG_TRACE, ("Waiting on the IdleReqPendEvent\n"));
	    
	    //
	    // make sure that the selective suspend request has been completed.
	    //

	    /*if(deviceExtension->SSEnable) {

	        KeWaitForSingleObject(&deviceExtension->NoIdleReqPendEvent, 
	                              Executive, 
	                              KernelMode, 
	                              FALSE, 
	                              NULL);
	    }*/

	    switch(code) {

    	    case IOCTL_BULKUSB_RESET_PIPE:
	    {
	        PFILE_OBJECT           fileObject;
	        PUSBD_PIPE_INFORMATION pipe;

	        pipe = NULL;
	        fileObject = NULL;

	        //
	        // FileObject is the address of the kernel file object to
	        // which the IRP is directed. Drivers use the FileObject
	        // to correlate IRPs in a queue.
	        //
	        fileObject = irpStack->FileObject;

	        if(fileObject == NULL) {

	            ntStatus = STATUS_INVALID_PARAMETER;

	            break;
	        }

	        pipe = (PUSBD_PIPE_INFORMATION) fileObject->FsContext;

	        if(pipe == NULL) {

	            ntStatus = STATUS_INVALID_PARAMETER;
	        }
	        else {
	            
	            ntStatus = BulkUsb_ResetPipe(DeviceObject, pipe);
	        }

	        break;
	    }

	    case IOCTL_BULKUSB_GET_CONFIG_DESCRIPTOR:
	    {
	     		ULONG length;

		        if(deviceExtension->UsbConfigurationDescriptor) 
			{

		            length = deviceExtension->UsbConfigurationDescriptor->wTotalLength;

		            if(outputBufferLength >= length) 
				{

		                RtlCopyMemory(ioBuffer,
		                              deviceExtension->UsbConfigurationDescriptor,
		                              length);

		                info = length;

		                ntStatus = STATUS_SUCCESS;
			       }
			       else 
				{
			                
			             ntStatus = STATUS_BUFFER_TOO_SMALL;
			        }
		        }
		        else 
			{
		            
		            ntStatus = STATUS_UNSUCCESSFUL;
		        }

	        break;
	    }


		case IOCTL_BULKUSB_RESET_DEVICE:
	       {
		 DBGPRINT(RT_DEBUG_TRACE,("%s: IOCTL_BULKUSB_RESET_DEVICE\n",__FUNCTION__));
	        ntStatus = BulkUsb_ResetDevice(DeviceObject);

	        break;
		}

		case IOCTL_USBQA_USB_MAC_READ_LONG:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG Offset = pInBuf[0];					//addr
			DBGPRINT(RT_DEBUG_TRACE,("%s: IOCTL_USBQA_USB_MAC_READ_LONG, don't use this now!!!!!!!\n",__FUNCTION__));
			info = 0;
			//USBHwHal_ReadMacRegister(deviceExtension, (USHORT)Offset, &pOutBuf[0]);
			info += 4;
			
			ntStatus = STATUS_SUCCESS;
	    	}
		break;

		case IOCTL_USBQA_USB_MAC_READ_LONG47:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG Offset = pInBuf[0];					//addr
			//DBGPRINT(RT_DEBUG_TRACE,"%s: IOCTL_USBQA_USB_MAC_READ_LONG47\n",__FUNCTION__);
			info = 0;
			USBHwHal_ReadMacRegister47(deviceExtension, (USHORT)Offset, &pOutBuf[0]);
			info += 4;
			
			ntStatus = STATUS_SUCCESS;
	    	}
		break;

		case IOCTL_USBQA_USB_MAC_READ_LONG63:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG HighByte = pInBuf[0];					//addr
			ULONG LowByte =  pInBuf[1];	
			ULONG Length =  pInBuf[2];	
			info = 0;
			//DBGPRINT(RT_DEBUG_TRACE,"%s: IOCTL_USBQA_USB_MAC_READ_LONG63 pInBuf[0]=0x%08x, HighByte=0x%08x, LowByte=0x%08x\n",__FUNCTION__,pInBuf[0],HighByte,LowByte);
			USBHwHal_ReadMacRegister63(deviceExtension, (USHORT)HighByte,(USHORT)LowByte, &pOutBuf[0],Length);
			info += Length;
			
			ntStatus = STATUS_SUCCESS;
	    	}
		break;


		case IOCTL_USBQA_USB_MAC_WRITE_LONG66:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG HighByte = pInBuf[0];					//addr
			ULONG LowByte =  pInBuf[1];
		
			ULONG Value = pInBuf[2];	
			info = 0;
			//DBGPRINT(RT_DEBUG_TRACE,"%s: IOCTL_USBQA_USB_MAC_WRITE_LONG66 pInBuf[0]=0x%08x, HighByte=0x%08x, LowByte=0x%08x\n",__FUNCTION__,pInBuf[0],HighByte,LowByte);
			USBHwHal_WriteMacRegister66(deviceExtension,  (USHORT)HighByte, (USHORT)LowByte, Value);//convert to  (USHORT) otherwise can't pass free build.

			info = 0;
			ntStatus = STATUS_SUCCESS;
		}
		break;

		///////	Set Start Rx0
		case IOCTL_USBQA_START_RX0:
		{
			KIRQL Irql;
			USHORT* pInBuf = (USHORT*)ioBuffer;
			USHORT* pOutBuf = (USHORT*)ioBuffer;

			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_USBQA_START_RX0\n"));

			if(!deviceExtension->bRxEnable[BULKIN_PIPENUMBER_0])
			{//now rx0 is stop can start.
				deviceExtension->bRxEnable[BULKIN_PIPENUMBER_0] = TRUE;

				deviceExtension->BulkInRemained[BULKIN_PIPENUMBER_0] = *pInBuf; 

				if (deviceExtension->BulkInRemained[BULKIN_PIPENUMBER_0] == 0)
					deviceExtension->ContinBulkIn[BULKIN_PIPENUMBER_0] = TRUE;
				else
					deviceExtension->ContinBulkIn[BULKIN_PIPENUMBER_0] = FALSE;
				
				info = 0;

				KeAcquireSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_0], &Irql);
				if(deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_0] > 0)
				{
					//KIRQL Irql;
					//KeAcquireSpinLock(&dx->RxSpinLock[0], &Irql);
					deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_0] ++;
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_0], Irql);
				}
				else
				{
					//KIRQL Irql;
					//KeAcquireSpinLock(&dx->RxSpinLock[0], &Irql);
					deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_0] ++;
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_0], Irql);

					RtlZeroMemory(&deviceExtension->ReceivedCount[BULKIN_PIPENUMBER_0], sizeof(LARGE_INTEGER));

					//Kick bulk in
					UsbDoBulkInTransfer(deviceExtension,BULKIN_PIPENUMBER_0);
				}

			}

			
			info = 0;
			ntStatus = STATUS_SUCCESS;
		}
		break;

		///////	Set Start Rx1
		case IOCTL_USBQA_START_RX1:
		{
			KIRQL Irql;
			USHORT* pInBuf = (USHORT*)ioBuffer;
			USHORT* pOutBuf = (USHORT*)ioBuffer;

			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_USBQA_START_RX1\n"));

			if(!deviceExtension->bRxEnable[BULKIN_PIPENUMBER_1] )
			{//now rx1 is stop can start.
				deviceExtension->bRxEnable[BULKIN_PIPENUMBER_1] = TRUE;

				deviceExtension->BulkInRemained[BULKIN_PIPENUMBER_1] = *pInBuf; 

				if (deviceExtension->BulkInRemained[BULKIN_PIPENUMBER_1] == 0)
					deviceExtension->ContinBulkIn[BULKIN_PIPENUMBER_1] = TRUE;
				else
					deviceExtension->ContinBulkIn[BULKIN_PIPENUMBER_1] = FALSE;
				
				info = 0;

				KeAcquireSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_1], &Irql);
				if(deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_1] > 0)
				{
					//KIRQL Irql;
					//KeAcquireSpinLock(&dx->RxSpinLock[BULKIN_PIPENUMBER_1], &Irql);
					deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_1] ++;
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_1], Irql);
				}
				else
				{
					//KIRQL Irql;
					//KeAcquireSpinLock(&dx->RxSpinLock[BULKIN_PIPENUMBER_1], &Irql);
					deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_1] ++;
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_1], Irql);

					RtlZeroMemory(&deviceExtension->ReceivedCount[BULKIN_PIPENUMBER_1], sizeof(LARGE_INTEGER));

					//Kick bulk in
					UsbDoBulkInTransfer(deviceExtension,BULKIN_PIPENUMBER_1);
				}

			}

			

			info = 0;
			ntStatus = STATUS_SUCCESS;
		}
		break;

		///////	Set Stop Rx0
		case IOCTL_USBQA_STOP_RX0:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_USBQA_STOP_RX0\n"));

			if(deviceExtension->bRxEnable[BULKIN_PIPENUMBER_0] )
			{//now rx0 is enable can stop.
				KIRQL Irql;

				deviceExtension->bRxEnable[BULKIN_PIPENUMBER_0] = FALSE;

				deviceExtension->BulkInRemained[BULKIN_PIPENUMBER_0] = 0;
				deviceExtension->ContinBulkIn[BULKIN_PIPENUMBER_0] = FALSE;

				info = 0;

				KeAcquireSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_0], &Irql);
				deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_0] --;
				//KeReleaseSpinLock(&dx->RxSpinLock[0], Irql);

				if(deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_0] > 0)
				{
					DBGPRINT(RT_DEBUG_TRACE,("%s  pipe RX %d ulBulkInRunning %d \n",__FUNCTION__,BULKIN_PIPENUMBER_0,deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_0]));
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_0], Irql);				
				}
				else
				{
					UsbCancelBulkInIrp(DeviceObject,BULKIN_PIPENUMBER_0);
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_0], Irql);
				}

			}

			info = 0;	
			ntStatus = STATUS_SUCCESS;
		}
		break;

		///////	Set Stop Rx1
		case IOCTL_USBQA_STOP_RX1:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_USBQA_STOP_RX1\n"));
			
			if(deviceExtension->bRxEnable[BULKIN_PIPENUMBER_1])
			{//now rx1 is enable can stop.
				KIRQL Irql;

				deviceExtension->bRxEnable[BULKIN_PIPENUMBER_1] = FALSE;

				deviceExtension->BulkInRemained[BULKIN_PIPENUMBER_1] = 0;
				deviceExtension->ContinBulkIn[BULKIN_PIPENUMBER_1] = FALSE;

				info = 0;

				KeAcquireSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_1], &Irql);
				deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_1] --;
				//KeReleaseSpinLock(&dx->RxSpinLock[0], Irql);

				if(deviceExtension->ulBulkInRunning[BULKIN_PIPENUMBER_1] > 0)
				{
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_1], Irql);				
				}
				else
				{
					UsbCancelBulkInIrp(DeviceObject,BULKIN_PIPENUMBER_1);
					KeReleaseSpinLock(&deviceExtension->RxSpinLock[BULKIN_PIPENUMBER_1], Irql);
				}


			}		

			info = 0;	
			ntStatus = STATUS_SUCCESS;
		}
		break;

		///////	Setup Bulk Out Packet
		case IOCTL_USBQA_SETUP_BULK_OUT_PACKET:
		{
			PTX_WI_STRUC pTxWI;
	
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG txPipeNum = pInBuf[0];
			ULONG Length = pInBuf[1]&0X0000FFFF;//refernece TX D, txbytecount
			//UCHAR* pDataX = (UCHAR*)&pInBuf[2];
			ULONG* pDataX = &pInBuf[1];

			DBGPRINT(RT_DEBUG_TRACE,("%s IOCTL_USBQA_SETUP_BULK_OUT_PACKET\n",__FUNCTION__));
			DBGPRINT(RT_DEBUG_TRACE,("%s txPipeNum = %d, tx length= %d\n",__FUNCTION__, txPipeNum,Length));

			/*************************************************************/
			// driver must pad 4 bytes 0 at the ending of  frame
			deviceExtension->TxBufferLength[txPipeNum]     = Length +4;
			RtlZeroMemory(deviceExtension->TxBuffer[txPipeNum],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
			RtlMoveMemory(deviceExtension->TxBuffer[txPipeNum], pDataX, Length);
			/*************************************************************/

			/*************************************************************/
			// For Inc Length Test...
			//RtlZeroMemory(deviceExtension->TxBufferForIncLenTest,  USB_BULK_BUFFER_SIZE );
			//RtlMoveMemory(deviceExtension->TxBufferForIncLenTest, pDataX, deviceExtension->TxBufferLength[txPipeNum]);
			
			//pTxWI = ( PTX_WI_STRUC ) (dx->TxBuffer[txPipeNum] + sizeof(TX_INF_STRUC));
			//pTxWI = ( PTX_WI_STRUC ) (dx->TxBuffer[txPipeNum] );

			deviceExtension->bMinLen = 44;
			//dx->bMaxLen = pTxWI->MPDU_SIZE;
			deviceExtension->bCurrlength= deviceExtension->bMinLen-1;
			/*************************************************************/

			DBGPRINT(RT_DEBUG_TRACE,("dx->TxBufferLength[%d] = %d",txPipeNum, deviceExtension->TxBufferLength[txPipeNum]));
			info = 0;	
			ntStatus = STATUS_SUCCESS;
	
		}
		break;

		///////	Set Start Bulk Out
		case IOCTL_USBQA_START_BULK_OUT:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG BulkOutRemained = pInBuf[0];
			ULONG BulkOutWhichPipeNeedRun = pInBuf[1];
			ULONG BulkOutTxType = pInBuf[2];
			//ULONG NextVLD = pInBuf[3];  // if use nextvld  

			ULONG temp;

			DBGPRINT(RT_DEBUG_TRACE,("%s IOCTL_USBQA_START_BULK_OUT\n",__FUNCTION__));
			DBGPRINT(RT_DEBUG_TRACE,("%s BulkOutRemained = %d\n",__FUNCTION__, BulkOutRemained));
			DBGPRINT(RT_DEBUG_TRACE,("%s BulkOutWhichPipeNeedRun = 0x%x\n",__FUNCTION__, BulkOutWhichPipeNeedRun));
			DBGPRINT(RT_DEBUG_TRACE,("%s dx->bBulkOutRunning = %d\n",__FUNCTION__ ,deviceExtension->bBulkOutRunning));

			RtlZeroMemory(deviceExtension->RXMPDUBuffer, USB_BULK_BUFFER_SIZE);

			deviceExtension->Seq_dbg = (BOOLEAN)pInBuf[4];
			deviceExtension->BulkOutRemained = BulkOutRemained;
			//dx->bNextVLD = NextVLD;

			if (deviceExtension->BulkOutRemained == 0)
			{
				deviceExtension->BulkOutRemained = 0xFFFFFFFF;
				deviceExtension->ContinBulkOut = TRUE;
			}
			else
				deviceExtension->ContinBulkOut = FALSE;
			
			deviceExtension->BulkOutWhichPipeNeedRun = BulkOutWhichPipeNeedRun;
			deviceExtension->BulkOutTxType = BulkOutTxType;

			info = 0;

			RtlZeroMemory(&deviceExtension->TransmittedCount, sizeof(ULONG));
			RtlZeroMemory(&deviceExtension->TxBulkCount[0], sizeof(ULONG));
			RtlZeroMemory(&deviceExtension->TxBulkCount[1], sizeof(ULONG));
			RtlZeroMemory(&deviceExtension->TxBulkCount[2], sizeof(ULONG));
			RtlZeroMemory(&deviceExtension->TxBulkCount[3], sizeof(ULONG));
			RtlZeroMemory(&deviceExtension->TxBulkCount[4], sizeof(ULONG));
			RtlZeroMemory(&deviceExtension->TxBulkCount[5], sizeof(ULONG));

			if(deviceExtension->bBulkOutRunning == TRUE)
			{
				;
			}
			else
			{
				deviceExtension->bBulkOutRunning = TRUE;

				DBGPRINT(RT_DEBUG_TRACE,("BulkOutWhichPipeNeedRun = %d", BulkOutWhichPipeNeedRun));
				//Kick bulk out
				if(BulkOutWhichPipeNeedRun & 0x0001)
					UsbDoBulkOutTransfer(deviceExtension,0);	

				if(BulkOutWhichPipeNeedRun & 0x0002)
					UsbDoBulkOutTransfer(deviceExtension,1);

				if(BulkOutWhichPipeNeedRun & 0x0004)
					UsbDoBulkOutTransfer(deviceExtension,2);

				if(BulkOutWhichPipeNeedRun & 0x0008)
					UsbDoBulkOutTransfer(deviceExtension,3);

				if(BulkOutWhichPipeNeedRun & 0x0010)
					UsbDoBulkOutTransfer(deviceExtension,4);

				if(BulkOutWhichPipeNeedRun & 0x0020)
					UsbDoBulkOutTransfer(deviceExtension,5);
			}
		
			info = 0;	
			ntStatus = STATUS_SUCCESS;
		}
		break;


		case IOCTL_USBQA_STOP_TX:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			deviceExtension->BulkOutRemained = 0;
			deviceExtension->ContinBulkOut = FALSE;
			DebugPrint("IOCTL_USBQA_STOP_TX\n");			

			// Wait currnet bulkout complete
			RTMPusecDelay(1000);			

			info = 0;	
			ntStatus = STATUS_SUCCESS;

		}
		break;

		case IOCTL_QA_GET_FWD_SEQNUM:
		{
			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FW_STATUS g_FWSeqMCU= %d\n", g_FWSeqMCU));
			
			RtlCopyMemory(ioBuffer, &g_FWSeqMCU,sizeof(g_FWSeqMCU));
			info = sizeof(g_FWSeqMCU);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			ntStatus = Irp->IoStatus.Status;

		}
		break;

		case IOCTL_QA_GET_FWD_STATUS:
		{
			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FWD_STATUS g_FWRspStatus= %d\n", g_FWRspStatus));
			
			RtlCopyMemory(ioBuffer, &g_FWRspStatus,sizeof(g_FWRspStatus));
			info = sizeof(g_FWRspStatus);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			ntStatus = Irp->IoStatus.Status;

		}
		break;

		case IOCTL_QA_GET_PACKETCMD_SEQNUM:
		{
			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_PACKETCMD_SEQNUM g_PacketCMDSeqMCU= %d\n", g_PacketCMDSeqMCU));
			
			RtlCopyMemory(ioBuffer, &g_PacketCMDSeqMCU,sizeof(g_PacketCMDSeqMCU));
			info = sizeof(g_PacketCMDSeqMCU);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			ntStatus = Irp->IoStatus.Status;

		}
		break;

		case IOCTL_QA_GET_PACKETCMD_RESPONSE:
		{
			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FW_STATUS g_FWSeqMCU= %d\n", g_FWSeqMCU));
			
			RtlCopyMemory(ioBuffer, &g_PacketCMDRspData,sizeof(g_PacketCMDRspData));
			info = sizeof(g_PacketCMDRspData);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			ntStatus = Irp->IoStatus.Status;

		}
		break;

		///////	Write Eeprom Registers
		case IOCTL_USBQA_USB_EEPROM_WRITE:
		{

			USHORT* pInBuf = (USHORT*)ioBuffer;
			USHORT* pOutBuf = (USHORT*)ioBuffer;

			USHORT Offset = pInBuf[0];					//addr
			USHORT Length = pInBuf[1];					//length

			USHORT* pValueX = &pInBuf[2];					//value ...
			UCHAR* pOneByte = (UCHAR*)&pInBuf[2];
			int i;

			DBGPRINT(RT_DEBUG_TRACE,("DeviceIoControl: IOCTL_USBQA_USB_EEPROM_WRITE\n"));
			info = 0;
#if 1
			for(i=0; i<Length; i++)
			{
				HwHal_USBWriteEeRegisters(deviceExtension, Offset+i, 1, (USHORT*)(pOneByte+i));
			}
#else
			for(i=0; i<Length; i+=2)
			{
				HwHal_WriteEeRegisters(dx, Offset+i, 2, &pValueX[i/2]);
			}
#endif
			
			info = 0;
			ntStatus = STATUS_SUCCESS;
			
	    	}
		break;

		///////	Read Eeprom Registers
		case IOCTL_USBQA_USB_EEPROM_READ:
		{

			USHORT* pInBuf = (USHORT*)ioBuffer;
			USHORT* pOutBuf = (USHORT*)ioBuffer;

			USHORT Offset = pInBuf[0];					//addr
			USHORT Length = pInBuf[1];					//length
			int i;

			//USHORT* pValueX = &pInBuf[2];					//value ...
			DBGPRINT(RT_DEBUG_TRACE,("DeviceIoControl: IOCTL_USBQA_USB_EEPROM_READ\n"));
			info = 0;
			for(i=0; i<Length; i+=2)
			{
				HwHal_USBReadEeRegisters(deviceExtension, Offset+i, 2, &pOutBuf[i/2]);
				info += 2;
			}
			
			ntStatus = STATUS_SUCCESS;

	    	}
		break;

		//0: BufferMode, 1: Efuse, 2: EEPROM
		case IOCTL_SET_EFUSE_MODE:
		{
			UCHAR* pInBuf = (UCHAR*)ioBuffer;

			deviceExtension->EfuseMode = pInBuf[0];

			// pcie pDevExt->EfuseMode = pInBuf[0];
			
			ntStatus = STATUS_SUCCESS;
	    	}
		break;

		case IOCTL_WRITE_EEPROM_BUFFER:
		{			
			ULONG* pInBuf = (ULONG*)ioBuffer;			
			ULONG addr = (ULONG)pInBuf[0];
			UCHAR size = (UCHAR)pInBuf[1];
			UCHAR idx = 0;
			UCHAR idx2 = 0;	
			DBGPRINT(RT_DEBUG_TRACE,("WRITE_EEPROM_BUFFER, addr = 0x%x, size = %d\n", addr, size ));
		
			if (size == 16)
			{
				
				for (idx=0; idx<4; idx++)
				{
					DBGPRINT(RT_DEBUG_TRACE,("0x%x \n", pInBuf[2+idx] ));
					deviceExtension->EfuseContent[addr+idx*4] = (UCHAR)(pInBuf[2+idx]);
					deviceExtension->EfuseContent[addr+idx*4+1] = (UCHAR)(pInBuf[2+idx] >> 8);
					deviceExtension->EfuseContent[addr+idx*4+2] = (UCHAR)(pInBuf[2+idx] >> 16);
					deviceExtension->EfuseContent[addr+idx*4+3] = (UCHAR)(pInBuf[2+idx] >> 24);
					DBGPRINT(RT_DEBUG_TRACE,("EfuseContent[0x%x] = 0x%x \n", addr+idx*4,  (UCHAR)(pInBuf[2+idx] )));
					DBGPRINT(RT_DEBUG_TRACE,("EfuseContent[0x%x] = 0x%x \n", addr+idx*4+1, (UCHAR)(pInBuf[2+idx] >> 8)));
					DBGPRINT(RT_DEBUG_TRACE,("EfuseContent[0x%x] = 0x%x \n", addr+idx*4+2, (UCHAR)(pInBuf[2+idx] >> 16)));
					DBGPRINT(RT_DEBUG_TRACE,("EfuseContent[0x%x] = 0x%x \n", addr+idx*4+3, (UCHAR)(pInBuf[2+idx] >> 24)));
				}
				
			}
			else
			{
			
				DBGPRINT(RT_DEBUG_TRACE,("size != 16 \n"));	
				
			}
			ntStatus = STATUS_SUCCESS;
		}
				
		break;
		//(write Firmware image to driver) 
		case IOCTL_WRITE_FW:
		{
			//pInBuf[0]: address
			//pInBuf[1]: size
			//pInBuf[2]: buffer
			ULONG* pInBuf = (ULONG*)ioBuffer;			
			ULONG addr = (ULONG)pInBuf[0];
			ULONG size = (ULONG)pInBuf[1];
			ULONG idx = 0;			
			DBGPRINT(RT_DEBUG_TRACE,("WRITE FW image, addr = 0x%x, size = %d\n", addr, size ));
			if(deviceExtension->FWSize + size > MAXFIRMWARESIZE)
			{
				deviceExtension->IsUISetFW = FALSE;
				ntStatus = STATUS_INSUFFICIENT_RESOURCES;
			}
			else
			{		
				deviceExtension->FWSize += size;
				if (size == 16)
				{					
					for (idx=0; idx<4; idx++)
					{
						DBGPRINT(RT_DEBUG_TRACE,("0x%x \n", pInBuf[2+idx] ));						
						deviceExtension->FWImage[addr+idx*4] = (UCHAR)(pInBuf[2+idx]);
						deviceExtension->FWImage[addr+idx*4+1] = (UCHAR)(pInBuf[2+idx] >> 8);
						deviceExtension->FWImage[addr+idx*4+2] = (UCHAR)(pInBuf[2+idx] >> 16);
						deviceExtension->FWImage[addr+idx*4+3] = (UCHAR)(pInBuf[2+idx] >> 24);				
					}
				}
				else
				{			
					UCHAR RemainSize = size;
					DBGPRINT(RT_DEBUG_TRACE,("size != 16 sholud be last data, %d bytes, addr = 0x%x\n", size, addr));
					idx = 0;
					do
					{
						
						if(RemainSize > 4)
						{
							deviceExtension->FWImage[addr+idx*4] = (UCHAR)(pInBuf[2+idx]);
							deviceExtension->FWImage[addr+idx*4+1] = (UCHAR)(pInBuf[2+idx] >> 8);
							deviceExtension->FWImage[addr+idx*4+2] = (UCHAR)(pInBuf[2+idx] >> 16);
							deviceExtension->FWImage[addr+idx*4+3] = (UCHAR)(pInBuf[2+idx] >> 24);				
							RemainSize -= 4;
							idx++;
						}
						else
						{
							UCHAR Count = 0;
							for(;Count<RemainSize;Count++)
							{
								deviceExtension->FWImage[addr+idx*4+Count] = (UCHAR)(pInBuf[2+idx] >> Count*8);
								DBGPRINT(RT_DEBUG_TRACE,("Last %dth byte = 0x%x\n", RemainSize-Count,deviceExtension->FWImage[addr+idx*4+Count]));
							}
							RemainSize = 0;
						}
					}while(RemainSize > 0);
					for(idx = 0; idx<size; idx++)
					{
						DBGPRINT(RT_DEBUG_TRACE,("Last %dth byte = 0x%x\n", size-idx,deviceExtension->FWImage[addr+idx]));
					}
				}
				deviceExtension->IsUISetFW = TRUE;
				ntStatus = STATUS_SUCCESS;
			}

		}
		break;
		case IOCTL_CLEAR_FW:
		{			
			deviceExtension->FWSize = 0;
			RtlZeroMemory(&deviceExtension->FWImage, MAXFIRMWARESIZE);
			ntStatus = STATUS_SUCCESS;
		}
		break;
		//(Trigger FW download) 
		case IOCTL_DOWNLOAD_FW:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG FWsize = (ULONG)pInBuf[0];
			ULONG Type = (ULONG)pInBuf[1];
			if(!deviceExtension->IsFWImageInUse && ((FWsize == deviceExtension->FWSize)  || !(deviceExtension->IsUISetFW)))
			{//make sure FW size is correct, or IsUISetFW to check if the fw has been set
				deviceExtension->IsFWImageInUse = TRUE;
				switch(Type)
				{
					case MT7603_RAM:
					{
						if(!IS_MT7603(deviceExtension))
						{
							ntStatus = STATUS_UNSUCCESSFUL;
							DBGPRINT(RT_DEBUG_ERROR,("Load 7603 RAM but not 7603!!!\n"));							
						}
						else
						{
							DBGPRINT(RT_DEBUG_TRACE,("IOCTL load 7603 RAM\n"));
							ntStatus = NICLoadFirmware(deviceExtension);
						}
					}
					break;
					case MT7636_RAM:
					{					
						if(!IS_MT7636(deviceExtension))
						{
							ntStatus = STATUS_UNSUCCESSFUL;
							DBGPRINT(RT_DEBUG_ERROR,("Load 7636 RAM but not 7636!!!\n"));
						}
						else
						{
							DBGPRINT(RT_DEBUG_TRACE,("IOCTL load 7636 RAM\n"));
							ntStatus = NICLoadFirmware(deviceExtension);
							//need to write CR after load 7636 FW
							mt7603_DoOtherConfig(deviceExtension);
						}
					}
					break;
					case MT7636_ROM_PATCH:
					{
						if(!IS_MT7636(deviceExtension))
						{
							ntStatus = STATUS_UNSUCCESSFUL;
							DBGPRINT(RT_DEBUG_ERROR,("Load 7636 ROM Patch but not 7636!!!\n"));							
						}
						else
						{
							DBGPRINT(RT_DEBUG_TRACE,("IOCTL load 7636 ROM\n"));
							ntStatus = NICLoadRomPatch(deviceExtension);							
						}
					}
					break;
					default:
						DBGPRINT(RT_DEBUG_WARN,("Type not implemented, type = %d\n", Type));
						ntStatus = STATUS_NOT_IMPLEMENTED;
				}
				deviceExtension->IsFWImageInUse = FALSE;				
			}
			else
			{
				ntStatus = STATUS_INSUFFICIENT_RESOURCES;
				DBGPRINT(RT_DEBUG_ERROR,("FW size does not match, command size = %d, image size = %d!!!\n", FWsize, deviceExtension->FWSize));
			}
				
			//Clear Image
			RtlZeroMemory(&deviceExtension->FWImage, MAXFIRMWARESIZE);
			deviceExtension->FWSize = 0;
		}
		break;

		case IOCTL_HAL_CMD:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			ULONG BytesTxd = 0;
			ULONG iCmd = pInBuf[0];					//addr
			ULONG iType = pInBuf[1];
			//ULONG Length = pInBuf[1];					//length
			ULONG temp = 0;
			UCHAR TempChar = 0;
			int i;

			DBGPRINT(RT_DEBUG_TRACE,("iCmd = %d\n", iCmd));
			switch(iCmd)
			{
				case CMD_ASIC_TOPINIT:
					mt_asic_top_init(deviceExtension);					
					break;

				case CMD_HIF_INIT:
					mt_hif_sys_init(deviceExtension);
					break;

				case CMD_TMAC_INIT:
					asic_set_tmac_info_template(deviceExtension);
					break;

				case CMD_INITMAC:
					mt7603_init_mac_cr(deviceExtension);
					//SetConfigMIBCounter(DeviceObject,STATISTICS_DEFAULT_EN_COUNTS,0, TRUE);
					break;

				case CMD_CONTI_TX_START:
					StartContinuesTXTest(deviceExtension, (UCHAR)pInBuf[1], (UCHAR)pInBuf[2], (UCHAR)pInBuf[3], (UCHAR)pInBuf[4], (UCHAR)pInBuf[5]);
					break;

				case CMD_CONTI_TX_STOP:
					StopContinuTXTest(deviceExtension);
					break;

				case CMD_CONTI_TONE_START:
					StartTXToneTest(deviceExtension,pInBuf[1]);
					break;

				case CMD_CONTI_TONE_STOP:
					StopContinuTXTest(deviceExtension);
					break;

				case CMD_GET_MAC_MDRDY:
					GetStaticNumber(deviceExtension,&temp);
					RtlCopyMemory(ioBuffer, &temp,sizeof(temp));

					//pOutBuf[0] = temp;
					//DBGPRINT(RT_DEBUG_TRACE,("temp = %d, pOutBuf[0]=%d\n", temp,pOutBuf[0]));
					BytesTxd = 4;	
					break;

				case  CMD_SET_TXPOWER:
					DBGPRINT(RT_DEBUG_TRACE,("%d ,%d ,%d \n",pInBuf[1], pInBuf[2], pInBuf[3]));
					TxSetFramePower(deviceExtension, pInBuf[1], pInBuf[2], (UCHAR)pInBuf[3]);
					break;

				case CMD_SET_AIFS://(IPG)
					for(i=AIFS_0;i<=AIFS_14;i++)//set all rings
					{
						SetAIFS(deviceExtension,i,pInBuf[1]);
					}
					break;

				case CMD_FREQ_OFFSET:
					TxSetFrequencyOffset (deviceExtension, pInBuf[1]);
					break;

				case CMD_TXPOWER_RANGE:
					TxConfigPowerRange(deviceExtension, (UCHAR)pInBuf[1],(UCHAR) pInBuf[2]);
					break;				

				case CMD_GET_DRIVER_RX_COUNT:
					RtlCopyMemory(ioBuffer, &deviceExtension->OtherCounters.Rx0ReceivedCount,sizeof(deviceExtension->OtherCounters.Rx0ReceivedCount));
					BytesTxd = sizeof(deviceExtension->OtherCounters.Rx0ReceivedCount);
					break;

				case CMD_GET_RXSNR:
					pOutBuf[0] = deviceExtension->OtherCounters.SNR0;
					pOutBuf[1] = deviceExtension->OtherCounters.SNR1;
					BytesTxd = 8;
					break;

				case CMD_GET_RCPI:
					DBGPRINT(RT_DEBUG_TRACE,("RCPI0 %d ,RCPI1 %d\n", deviceExtension->OtherCounters.RCPI0, deviceExtension->OtherCounters.RCPI1));
					pOutBuf[0] = deviceExtension->OtherCounters.RCPI0;
					pOutBuf[1] = deviceExtension->OtherCounters.RCPI1;
					BytesTxd = 8;
					break;

				case CMD_GET_FREQOFFSETFROM_RX:
					DBGPRINT(RT_DEBUG_TRACE,("FreqOffstFromRX %d \n", deviceExtension->OtherCounters.FreqOffstFromRX));
					pOutBuf[0] = deviceExtension->OtherCounters.FreqOffstFromRX;
					BytesTxd = 4;
					break;

				case CMD_GET_TX_COUNT:
					DBGPRINT(RT_DEBUG_TRACE,("CMD_GET_TX_COUNT %d \n", deviceExtension->OtherCounters.Ac0TxedCount));
					pOutBuf[0] = deviceExtension->OtherCounters.Ac0TxedCount;
					BytesTxd = sizeof(deviceExtension->OtherCounters.Ac0TxedCount);
					break;

				case CMD_SET_TSSI:
					DBGPRINT(RT_DEBUG_TRACE,("CMD_SET_TSSI\n"));
					SetTSSI(deviceExtension, pInBuf[1], (UCHAR)pInBuf[2]);
					BytesTxd = 0;
					break;	
				
				case CMD_SET_DPD:
					DBGPRINT(RT_DEBUG_TRACE,("CMD_SET_DPD\n"));
					SetDPD(deviceExtension, pInBuf[1], (UCHAR)pInBuf[2]);
					BytesTxd = 0;
					break;	

				case CMD_GET_MACFCSERRORCNT_COUNT:
					GetMACFCSErrCnt(deviceExtension, &pInBuf[0]);
					BytesTxd = 4;
					break;

				case CMD_GET_PHY_MDRDYCOUNT:
					GetPHYMdrdyErrCnt(deviceExtension, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_PHY_FCSERRCOUNT:
					GetPHYFCSErrCnt(deviceExtension, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_PD:
					GetPhyPD(deviceExtension, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_CCK_SIG_SFD:
					GetPhyCCK_SIG_SFD(deviceExtension, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_OFDM_ERR:
					GetPhyOFDMErr(deviceExtension, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_RSSI:
					GetPhyRSSI(deviceExtension, &pInBuf[0], &pInBuf[1], &pInBuf[2], &pInBuf[3]);
					BytesTxd = 32;
					break;
				
				case CMD_TMR_SETTING:
					SetTMR(deviceExtension, (UCHAR)pInBuf[1]); 
					BytesTxd = 0;
					break;

				case CMD_GET_SECURITY_RESULT:
					pInBuf[0] = deviceExtension->SecurityFail;
					BytesTxd = 4;
					break;


			}				

			ntStatus = STATUS_SUCCESS;
			info= BytesTxd;

		}
		break;

		

		case IOCTL_RESET_TXRX_COUNTER:
			DBGPRINT(RT_DEBUG_TRACE,("%s IOCTL_RESET_TXRX_COUNTER\n",__FUNCTION__));
			RtlZeroMemory(&deviceExtension->OtherCounters, sizeof(deviceExtension->OtherCounters));
			deviceExtension->SecurityFail = 0;
			ntStatus = STATUS_SUCCESS;
			break;

		

		 default :

	        ntStatus = STATUS_INVALID_DEVICE_REQUEST;

	        break;
    }
   
    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = info;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    //BulkUsb_DbgPrint(3, ("BulkUsb_DispatchDevCtrl::"));
    BulkUsb_IoDecrement(deviceExtension);
    DBGPRINT(RT_DEBUG_TRACE,("%s <<---\n",__FUNCTION__));
    return ntStatus;

//#endif
}

#else

NTSTATUS DispatchDeviceControlPCI(IN PDEVICE_OBJECT DeviceObject,
									IN PIRP Irp )
{
	PIO_STACK_LOCATION  irpStack;
	PULONG              pIBuffer;
	ULONG               inputBufferLength;
	PVOID               ioBuffer;
	ULONG               outputBufferLength;
	ULONG               ioControlCode;
	ULONG               byteCount = 0;
	NTSTATUS            ntStatus;
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;

//	DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: DispatchDeviceControl ===>\n");	

	//
	// Set default values in the IRP.  Note that this driver will not queue
	// any IRPs (i.e. pend requests).
	//

	Irp->IoStatus.Status      = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	//
	// Get a pointer to the current location in the Irp. This is where
	// the function codes and parameters are located.
	//

	irpStack = IoGetCurrentIrpStackLocation(Irp);

	//
	// Get the pointer to the output buffer and it's length.  The IOCTLs for
	// this driver have been defined as METHOD_BUFFERED, so we can use the
	// buffers directly (i.e. there is no MDL).
	//

	pIBuffer = (PULONG)Irp->AssociatedIrp.SystemBuffer;
	ioBuffer = Irp->AssociatedIrp.SystemBuffer;

	inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
	outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

	ASSERT(irpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL);

	switch (irpStack->Parameters.DeviceIoControl.IoControlCode) 
	{	
		case IOCTL_QA_GET_PACKETCMD_SEQNUM://USB and PCIe
		{
			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FW_STATUS g_FWSeqMCU= %d\n", g_FWSeqMCU));
			
			RtlCopyMemory(ioBuffer, &g_PacketCMDSeqMCU,sizeof(g_PacketCMDSeqMCU));
			
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(g_PacketCMDSeqMCU);		
		}
		break;

		case IOCTL_QA_GET_PACKETCMD_RESPONSE://USB and PCIe
		{
			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FW_STATUS g_FWSeqMCU= %d\n", g_FWSeqMCU));
			
			RtlCopyMemory(ioBuffer, &g_PacketCMDRspData,sizeof(g_PacketCMDRspData));
			

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(g_PacketCMDRspData);

		}
		break;
		
	
		case IOCTL_QA_GET_FWD_SEQNUM://USB and PCIe
		{
			ULONG BytesTxd;
			ULONG status;

			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FW_STATUS g_FWSeqMCU= %d\n", g_FWSeqMCU));

			RtlCopyMemory(ioBuffer, &g_FWSeqMCU,sizeof(g_FWSeqMCU));
			BytesTxd = sizeof(g_FWSeqMCU);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;
			status = Irp->IoStatus.Status;

		}
		break;

		case IOCTL_QA_GET_FWD_STATUS:
		{
			ULONG BytesTxd;
			ULONG status;

			DBGPRINT(RT_DEBUG_TRACE,("IOCTL_QA_GET_FWD_STATUS g_FWRspStatus= %d\n", g_FWRspStatus));

			RtlCopyMemory(ioBuffer, &g_FWRspStatus,sizeof(g_FWRspStatus));
			BytesTxd = sizeof(g_FWRspStatus);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;
			status = Irp->IoStatus.Status;

		}
		break;


			


		///////	DMA 4K Boundary Test	
		case IOCTL_PCIDUMPR_DMA_4K_BOUNDARY_TEST:
		{
			ULONG BytesTxd;
			ULONG status;
			
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			
			ULONG	b4kBoundaryCheck = (*pInBuf);
			DBGPRINT(RT_DEBUG_TRACE,("b4kBoundaryCheck = %d\n", b4kBoundaryCheck));

			g_bDam4kBoundaryCheck = (BOOLEAN)b4kBoundaryCheck;

			BytesTxd = 0;	
			status = STATUS_SUCCESS;	

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;		
		}
		break;

		//0: BufferMode, 1: Efuse, 2: EEPROM
		case IOCTL_SET_EFUSE_MODE:
		{
			UCHAR* pInBuf = (UCHAR*)ioBuffer;			

			pAd->EfuseMode = pInBuf[0];
			
			ntStatus = STATUS_SUCCESS;
	    	}
		break;

		case IOCTL_WRITE_EEPROM_BUFFER:
		{			
			ULONG* pInBuf = (ULONG*)ioBuffer;			
			ULONG addr = (ULONG)pInBuf[0];
			UCHAR size = (UCHAR)pInBuf[1];
			UCHAR idx = 0;
			UCHAR idx2 = 0;	
			DBGPRINT(RT_DEBUG_TRACE,("kun WRITE_EEPROM_BUFFER, addr = 0x%x, size = %d\n", addr, size ));
		
			if (size == 16)
			{
				for (idx=0; idx<4; idx++)
				{
					DBGPRINT(RT_DEBUG_TRACE,("kun 0x%x \n", pInBuf[2+idx] ));
					pAd->EfuseContent[addr+idx*4] = (UCHAR)(pInBuf[2+idx]);
					pAd->EfuseContent[addr+idx*4+1] = (UCHAR)(pInBuf[2+idx] >> 8);
					pAd->EfuseContent[addr+idx*4+2] = (UCHAR)(pInBuf[2+idx] >> 16);
					pAd->EfuseContent[addr+idx*4+3] = (UCHAR)(pInBuf[2+idx] >> 24);
					DBGPRINT(RT_DEBUG_TRACE,("[kun] EfuseContent[0x%x] = 0x%x \n", addr+idx*4,  (UCHAR)(pInBuf[2+idx] )));
					DBGPRINT(RT_DEBUG_TRACE,("[kun] EfuseContent[0x%x] = 0x%x \n", addr+idx*4+1, (UCHAR)(pInBuf[2+idx] >> 8)));
					DBGPRINT(RT_DEBUG_TRACE,("[kun] EfuseContent[0x%x] = 0x%x \n", addr+idx*4+2, (UCHAR)(pInBuf[2+idx] >> 16)));
					DBGPRINT(RT_DEBUG_TRACE,("[kun] EfuseContent[0x%x] = 0x%x \n", addr+idx*4+3, (UCHAR)(pInBuf[2+idx] >> 24)));
				}
				
			/*
				for (idx=0; idx<4; idx++)
				{
				
					RtlCopyMemory(
						&pAd->EfuseContent[addr+idx*4],
						(PUCHAR)pInBuf[2+idx],				 
						size
						);
						
				}//RtlCopyMemory((PULONG)pAd->Ac1RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);
				*/
				/*
				for (idx2 = 0; idx2<16;idx2++)
				{
					DBGPRINT(RT_DEBUG_TRACE,("kun 0x%x \n", pInBuf[2+idx]));						
				}
				*/
				
			}
			else
			{
			
				DBGPRINT(RT_DEBUG_TRACE,("[kun] size != 16 \n"));	
				/*
				UCHAR DWNumbers = size/4;
				UCHAR DWMods = size%4;
				for (idx=0; idx<DWNumbers; idx++)
				{
				
					RtlCopyMemory(
						&pAd->EfuseContent[addr+idx*4],			 
						(PVOID)pInBuf[2+idx],				 
						size
						);					

						
				}
				for (idx=0; idx<DWMods; idx++)
				{
					pAd->EfuseContent[addr+DWNumbers*4+idx]  = (UCHAR)(pInBuf[DWNumbers]>>(8*idx)); 				
				}
				*/
			}	
			/*
			DBGPRINT(RT_DEBUG_TRACE,("=====efuse content=====\n" ));
			for (idx = 0; idx<EFUSESIZE;idx++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("0x%02x \n", pAd->EfuseContent[idx]));	
				if(EFUSESIZE%16 == 0)
					DBGPRINT(RT_DEBUG_TRACE,("\n"));
			}
			DBGPRINT(RT_DEBUG_TRACE,("=====efuse content=====\n" ));
			*/
			ntStatus = STATUS_SUCCESS;
			

		}

		break;

		case IOCTL_HAL_CMD:
		{
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			ULONG status = STATUS_SUCCESS;
			ULONG BytesTxd = 0;
			ULONG iCmd = pInBuf[0];					//addr
			ULONG iType = pInBuf[1];
			//ULONG Length = pInBuf[1];					//length
			ULONG temp = 0;
			UCHAR TempChar = 0;
			DBGPRINT(RT_DEBUG_TRACE,("iCmd = %d\n", iCmd));
			switch(iCmd)
			{
				case CMD_ASIC_TOPINIT:
					mt_asic_top_init(pAd);
					break;

				case CMD_HIF_INIT:
					mt_hif_sys_init(pAd);
					break;

				case CMD_TMAC_INIT:
					asic_set_tmac_info_template(pAd);
					break;

				case CMD_INITMAC:
					mt7603_init_mac_cr(pAd);
					//SetConfigMIBCounter(pAd,STATISTICS_DEFAULT_EN_COUNTS,0, TRUE);
					break;

				case CMD_CONTI_TX_START:
					StartContinuesTXTest(pAd, (UCHAR)pInBuf[1], (UCHAR)pInBuf[2], (UCHAR)pInBuf[3], (UCHAR)pInBuf[4], (UCHAR)pInBuf[5]);
					break;

				case CMD_CONTI_TX_STOP:
					StopContinuTXTest(pAd);					
					break;

				case CMD_CONTI_TONE_START:
					StartTXToneTest(pAd, iType);
					break;

				case CMD_CONTI_TONE_STOP:
					StopTXToneTest(pAd);									
					break;

				case CMD_GET_MAC_MDRDY:
					GetStaticNumber(pAd,&temp);
					RtlCopyMemory(ioBuffer, &temp,sizeof(temp));

					//pOutBuf[0] = temp;
					//DBGPRINT(RT_DEBUG_TRACE,("temp = %d, pOutBuf[0]=%d\n", temp,pOutBuf[0]));
					BytesTxd = 4;	
					break;

				case  CMD_SET_TXPOWER:
					DBGPRINT(RT_DEBUG_TRACE,("%d ,%d ,%d \n",pInBuf[1], pInBuf[2], pInBuf[3]));
					TxSetFramePower(pAd, pInBuf[1], pInBuf[2], (UCHAR)pInBuf[3]);
					break;

				case CMD_SET_AIFS://(IPG)
					{
						int i;
						for(i=AIFS_0;i<=AIFS_14;i++)//set all rings
						{
							SetAIFS(pAd,i,pInBuf[1]);
						}
					}
					break;

				case CMD_FREQ_OFFSET:
					TxSetFrequencyOffset (pAd, pInBuf[1]);
					break;

				case CMD_TXPOWER_RANGE:
					TxConfigPowerRange(pAd, (UCHAR)pInBuf[1],(UCHAR) pInBuf[2]);
					break;				

				case CMD_GET_DRIVER_RX_COUNT:
					RtlCopyMemory(ioBuffer, &pAd->OtherCounters.Rx0ReceivedCount,sizeof(pAd->OtherCounters.Rx0ReceivedCount));
					BytesTxd = sizeof(pAd->OtherCounters.Rx0ReceivedCount);
					BytesTxd = 4;
					break;

				case CMD_GET_RXSNR:
					pOutBuf[0] = pAd->OtherCounters.SNR0;
					pOutBuf[1] = pAd->OtherCounters.SNR1;
					BytesTxd = 8;
					break;

				case CMD_GET_RCPI:
					DBGPRINT(RT_DEBUG_TRACE,("RCPI0 %d ,RCPI1 %d \n", pAd->OtherCounters.RCPI0, pAd->OtherCounters.RCPI1));
					pOutBuf[0] = pAd->OtherCounters.RCPI0;
					pOutBuf[1] = pAd->OtherCounters.RCPI1;
					BytesTxd = 8;
					break;

				case CMD_GET_FREQOFFSETFROM_RX:
					DBGPRINT(RT_DEBUG_TRACE,("FreqOffstFromRX %d \n", pAd->OtherCounters.FreqOffstFromRX));
					pOutBuf[0] = pAd->OtherCounters.FreqOffstFromRX;
					BytesTxd = 4;
					break;

				case CMD_GET_TX_COUNT:
					DBGPRINT(RT_DEBUG_TRACE,("CMD_GET_TX_COUNT %d \n", pAd->OtherCounters.Ac0TxedCount));
					pOutBuf[0] = pAd->OtherCounters.Ac0TxedCount;
					BytesTxd = sizeof(pAd->OtherCounters.Ac0TxedCount);
					break;

				case CMD_SET_TSSI:
					DBGPRINT(RT_DEBUG_TRACE,("CMD_SET_TSSI\n"));
					SetTSSI(pAd, pInBuf[1], (UCHAR)pInBuf[2]);
					BytesTxd = 0;
					break;	
				
				case CMD_SET_DPD:
					DBGPRINT(RT_DEBUG_TRACE,("CMD_SET_DPD\n"));
					SetDPD(pAd, pInBuf[1], (UCHAR)pInBuf[2]);
					BytesTxd = 0;
					break;	

				case CMD_GET_MACFCSERRORCNT_COUNT:
					GetMACFCSErrCnt(pAd, &pInBuf[0]);
					BytesTxd = 4;
					break;

				case CMD_GET_PHY_MDRDYCOUNT:
					GetPHYMdrdyErrCnt(pAd, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_PHY_FCSERRCOUNT:
					GetPHYFCSErrCnt(pAd, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_PD:
					GetPhyPD(pAd, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_CCK_SIG_SFD:
					GetPhyCCK_SIG_SFD(pAd, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_OFDM_ERR:
					GetPhyOFDMErr(pAd, &pInBuf[0], &pInBuf[1]);
					BytesTxd = 8;
					break;

				case CMD_GET_RSSI:
					GetPhyRSSI(pAd, &pInBuf[0], &pInBuf[1], &pInBuf[2], &pInBuf[3]);
					BytesTxd = 32;
					break;

				case CMD_TMR_SETTING:
					SetTMR(pAd, (UCHAR)pInBuf[1]);
					BytesTxd = 0;
					break;

				case CMD_GET_SECURITY_RESULT:
					 pInBuf[0] = pAd->SecurityFail;
					BytesTxd = 4;
					break;

			}
			
			status = STATUS_SUCCESS;	

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;

		}
		break;
		
		///////	Set Channel
		case IOCTL_PCIQA_SELECT_CHANNEL:
		{
			ULONG BytesTxd;
			ULONG status;
			
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			
			ULONG	Channel = (*pInBuf);
			DBGPRINT(RT_DEBUG_TRACE,("channel = %d\n", Channel));

			BytesTxd = 0;			
			HwHal_SetChannel(pAd, Channel);

			BytesTxd = 0;	
			status = STATUS_SUCCESS;	

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;
			
		}
		break;

		case IOCTL_QA_GET_FW_CMD_RSP:
		{
			ULONG BytesTxd;
			ULONG status;
			PFW_RXD_STRUC pFwReceiveBuffer;

			Testdbg("IOCTL_QA_GET_FW_CMD_RSP\n");

			pFwReceiveBuffer = (PFW_RXD_STRUC) &pAd->FwReceiveBuffer;
			Testdbg("pFwReceiveBuffer->CMD_SEQ = %d\n", pFwReceiveBuffer->CMD_SEQ);
			Testdbg("pAd->FwReqSequence = %d\n", pAd->FwReqSequence);
			
			if ( pFwReceiveBuffer->CMD_SEQ == pAd->FwReqSequence )
			{
				RtlCopyMemory(ioBuffer, &pAd->FwReceiveBuffer, pFwReceiveBuffer->PktLength + sizeof(FW_RXD_STRUC));

				BytesTxd = pFwReceiveBuffer->PktLength + sizeof(FW_RXD_STRUC);
			}
			else
			{
				BytesTxd = 0;
			}

			Irp->IoStatus.Status = STATUS_SUCCESS;

			Irp->IoStatus.Information = BytesTxd;
			status = Irp->IoStatus.Status;
		}
		break;
			
		///////	Write Rf Register
		case IOCTL_PCIQA_RF_WRITE:
		{
			ULONG BytesTxd;
			ULONG status;
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			ULONG Value = pInBuf[0];					//Value
		
			BytesTxd = 0;
			HwHal_WriteRfRegister(pAd, Value);

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;
		
		}
		break;


		///////	Read Mac Registers
		case IOCTL_PCIQA_PCI_MAC_MULTI_READ:
		{
			ULONG BytesTxd;
			ULONG status;

			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG Offset = pInBuf[0];					//addr
			ULONG Length = pInBuf[1];					//length

			//USHORT* pValueX = &pInBuf[2];					//value ...			

			BytesTxd = 0;

			HwHal_ReadMacRegisters(pAd, Offset, Length, pOutBuf);

			BytesTxd = Length;
			status = STATUS_SUCCESS;
			
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;			
	    	}
		break;
		
		///////	Write Mac Registers
		case IOCTL_PCIQA_PCI_MAC_MULTI_WRITE:
		{
			ULONG BytesTxd;
			ULONG status;
			
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;


			ULONG Offset = pInBuf[0];					//addr
			ULONG Length = pInBuf[1];					//length

			ULONG* pValueX = &pInBuf[2];					//value ...			

			BytesTxd = 0;

			HwHal_WriteMacRegisters(pAd, Offset, Length, pValueX);

			BytesTxd = 0;
			status = STATUS_SUCCESS;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;			
	    	}
		break;

		///////	Read Eeprom Registers
		case IOCTL_PCIQA_PCI_EEPROM_READ:
		{
			ULONG BytesTxd;
			ULONG status;
			
			USHORT* pInBuf = (USHORT*)ioBuffer;
			USHORT* pOutBuf = (USHORT*)ioBuffer;

			USHORT Offset = pInBuf[0];					//addr
			USHORT Length = pInBuf[1];					//length

			//USHORT* pValueX = &pInBuf[2];					//value ...			

			BytesTxd = 0;

			HwHal_ReadEeRegisters(pAd, Offset, Length, pOutBuf);
			BytesTxd = Length;
			
			status = STATUS_SUCCESS;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;	
	    	}
		break;
		
		///////	Write Eeprom Registers
		case IOCTL_PCIQA_PCI_EEPROM_WRITE:
		{
			ULONG BytesTxd;
			ULONG status;
			
			USHORT* pInBuf = (USHORT*)ioBuffer;
			USHORT* pOutBuf = (USHORT*)ioBuffer;

			USHORT Offset = pInBuf[0];					//addr
			USHORT Length = pInBuf[1];					//length

			USHORT* pValueX = &pInBuf[2];					//value ...			

			BytesTxd = 0;

			HwHal_WriteEeRegisters(pAd, Offset, Length, pValueX);
			
			BytesTxd = 0;
			status = STATUS_SUCCESS;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;		
	    	}
		break;

		///////	Read Bbp Register
		case IOCTL_PCIQA_PCI_BBP_READ:
		{
			ULONG BytesTxd;
			ULONG status;
			
			UCHAR* pInBuf = (UCHAR*)ioBuffer;
			UCHAR* pOutBuf = (UCHAR*)ioBuffer;

			UCHAR Id = pInBuf[0];					//Id

			//UCHAR* pValue = &pOutBuf[1];					//pValue		

			BytesTxd = 0;
			HwHal_ReadBbpRegister(pAd, Id, pOutBuf);
			
			BytesTxd = 1;	
			status = STATUS_SUCCESS;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;		
	    	}
		break;
		
		///////	Write Bbp Register
		case IOCTL_PCIQA_PCI_BBP_WRITE:
		{
			ULONG BytesTxd;
			ULONG status;
			
			UCHAR* pInBuf = (UCHAR*)ioBuffer;
			UCHAR* pOutBuf = (UCHAR*)ioBuffer;


			UCHAR Id = pInBuf[0];					//Id
			UCHAR Value = pInBuf[1];						//Value
		

			BytesTxd = 0;
			HwHal_WriteBbpRegister(pAd, Id, Value);

			BytesTxd = 0;
			status = STATUS_SUCCESS;

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;		
	    	}
		break;

		///////	Set Rx Antenna
		case IOCTL_PCIQA_RX_ANTENNA_SELECTED:
		{
			ULONG BytesTxd;
			ULONG status;
			
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			
			ULONG	Antenna = *pInBuf;
			DBGPRINT(RT_DEBUG_TRACE,("Rx Antenna =%d\n", Antenna));

			BytesTxd = 0;					
			HwHal_SetRxAntenna(pAd, Antenna);

			BytesTxd = 0;	
			status = STATUS_SUCCESS;	

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;			
		}
		break;

		///////	Set Tx Antenna
		case IOCTL_PCIQA_TX_ANTENNA_SELECTED:
		{
			ULONG BytesTxd;
			ULONG status;
			
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;

			ULONG	Antenna = *pInBuf;
			DBGPRINT(RT_DEBUG_TRACE,("Rx Antenna =%d\n", Antenna));
			
			BytesTxd = 0;					
			HwHal_SetTxAntenna(pAd, Antenna);

			BytesTxd = 0;	
			status = STATUS_SUCCESS;		

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;			
		}
		break;

		///////	Set Rf Antenna
		case IOCTL_PCIQA_RF_ANTENNA_SELECTED:
		{
			ULONG BytesTxd;
			ULONG status;
			
			ULONG* pInBuf = (ULONG*)ioBuffer;
			ULONG* pOutBuf = (ULONG*)ioBuffer;
			
			ULONG	Antenna = *pInBuf;
			DBGPRINT(RT_DEBUG_TRACE,("Rx Antenna =%d\n", Antenna));

			BytesTxd = 0;					
			HwHal_SetRfAntenna(pAd, Antenna);

			BytesTxd = 0;	
			status = STATUS_SUCCESS;	

			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = BytesTxd;			
		}
		break;
		
		//--------------------------------------------------------------
			
	        case IOCTL_PCIDUMPR_DUMP_PCI_DATA:
		{
			ULONG Bus, length, propertyAddress;
			ULONG Function, Device;

			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_DUMP_PCI_DATA \n"));
			
			IoGetDeviceProperty(pAd->pPhyDeviceObj,
						DevicePropertyBusNumber,
						sizeof(ULONG),
						(PVOID)&Bus,
						&length);

			IoGetDeviceProperty(pAd->pPhyDeviceObj,
						DevicePropertyAddress,
						sizeof(ULONG),
						(PVOID)&propertyAddress,
						&length);

			Function = (propertyAddress) & 0x0000FFFF;
			Device = ((propertyAddress) >> 16) & 0x0000FFFF;

			RtlCopyMemory(pIBuffer, &Bus, sizeof(ULONG));
			RtlCopyMemory(pIBuffer + 1, &Function, sizeof(ULONG));
			RtlCopyMemory(pIBuffer + 2, &Device, sizeof(ULONG));

	        
			//
		        // Scan the PCI slots and dump the PCI configuration data to the
		        // user's buffer.
		        //
				ReadWriteConfigSpace(DeviceObject, 
										0, // 0 for read 1 for write
										pIBuffer + 3,
										0,
										256
										);

		        //
		        // If everything was OK, then update the returned data byte count.
		        //
				byteCount = 3 * sizeof(ULONG) + 256 + 64;

		        if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
		        }	       		
		}
		 break;

		 case IOCTL_READ_PCI_ConfigSpace:
		 {
			ULONG	Bus, length, propertyAddress;
			ULONG	Function, Device;

		 	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_READ_PCI_ConfigSpace \n"));
			
			IoGetDeviceProperty(pAd->pPhyDeviceObj,
						DevicePropertyBusNumber,
						sizeof(ULONG),
						(PVOID)&Bus,
						&length);

			IoGetDeviceProperty(pAd->pPhyDeviceObj,
						DevicePropertyAddress,
						sizeof(ULONG),
						(PVOID)&propertyAddress,
						&length);

			Function = (propertyAddress) & 0x0000FFFF;
			Device = ((propertyAddress) >> 16) & 0x0000FFFF;

			RtlCopyMemory(pIBuffer, &Bus, sizeof(ULONG));
			RtlCopyMemory(pIBuffer + 1, &Function, sizeof(ULONG));
			RtlCopyMemory(pIBuffer + 2, &Device, sizeof(ULONG));

	        
			//
		        // Scan the PCI slots and dump the PCI configuration data to the
		        // user's buffer.
		        //
			if ( pAd->OSMajorVersion == 5) // XP
			{
				PULONG				MmVirtualAddr;
				PHYSICAL_ADDRESS		QueryPhyAddr;
				QueryPhyAddr.LowPart = pAd->ConfigurationSpacePhyAddr.LowPart;
				QueryPhyAddr.LowPart += (Bus*32*8*0x1000) + (Device*8*0x1000) + (Function*0x1000);
				QueryPhyAddr.LowPart += *(pIBuffer + 3);

				MmVirtualAddr = (PULONG)MmMapIoSpace(QueryPhyAddr, 4, MmCached);

				*(pIBuffer + 3) = *MmVirtualAddr;
				
				MmUnmapIoSpace((PVOID)MmVirtualAddr, 4);
			}
			else
			{
					ReadWriteConfigSpace(DeviceObject, 
												0, // 0 for read 1 for write
												ioBuffer,
												*(pIBuffer + 3),
												4
												);
			}
		        //
		        // If everything was OK, then update the returned data byte count.
		        //
			byteCount = sizeof(ULONG);

		        if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
		        }
		 }
		 break;

		case IOCTL_WRITE_PCI_ConfigSpace:
		{
			ULONG	Bus, length, propertyAddress;
			ULONG	Function, Device;

			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_WRITE_PCI_ConfigSpace \n"));
			
			IoGetDeviceProperty(pAd->pPhyDeviceObj,
						DevicePropertyBusNumber,
						sizeof(ULONG),
						(PVOID)&Bus,
						&length);

			IoGetDeviceProperty(pAd->pPhyDeviceObj,
						DevicePropertyAddress,
						sizeof(ULONG),
						(PVOID)&propertyAddress,
						&length);

			Function = (propertyAddress) & 0x0000FFFF;
			Device = ((propertyAddress) >> 16) & 0x0000FFFF;

			RtlCopyMemory(pIBuffer, &Bus, sizeof(ULONG));
			RtlCopyMemory(pIBuffer + 1, &Function, sizeof(ULONG));
			RtlCopyMemory(pIBuffer + 2, &Device, sizeof(ULONG));

	        
			//
		        // Scan the PCI slots and dump the PCI configuration data to the
		        // user's buffer.
		        //
		        if ( pAd->OSMajorVersion == 5) // XP
			{
				PULONG				MmVirtualAddr;
				PHYSICAL_ADDRESS		QueryPhyAddr;
				QueryPhyAddr.LowPart = pAd->ConfigurationSpacePhyAddr.LowPart;
				QueryPhyAddr.LowPart += (Bus*32*8*0x1000) + (Device*8*0x1000) + (Function*0x1000);
				QueryPhyAddr.LowPart += *(pIBuffer + 3);

				MmVirtualAddr = (PULONG)MmMapIoSpace(QueryPhyAddr, 4, MmCached);

				*MmVirtualAddr = *(pIBuffer + 3);

				MmUnmapIoSpace((PVOID)MmVirtualAddr, 4);
			}
			else
			{
				ReadWriteConfigSpace(DeviceObject, 
											1, // 0 for read 1 for write
											((PULONG)ioBuffer + 4),
											*(pIBuffer + 3),
											4
											);
			}

		        //
		        // If everything was OK, then update the returned data byte count.
		        //
			byteCount = 0;

		        if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
		        }
		}
		break;
		
		case IOCTL_PCIDUMPR_GET_PCI_ADDRESS :
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_GET_PCI_ADDRESS \n"));

			RtlCopyMemory(ioBuffer,&pAd->ExtendPciAddress, sizeof(EXTENDED_PCI_ADDRESS));
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(EXTENDED_PCI_ADDRESS);
		break;

		//
		// Read Memry in defined format
		//
		case IOCTL_PCIDUMPR_READ_MEMORY:
			//DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_READ_MEMORY \n"));

			Irp->IoStatus.Status = ReadMemory(DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS) 
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		//
		// Write Memry in defined format
		//
		case IOCTL_PCIDUMPR_WRITE_MEMORY:
//			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_WRITE_MEMORY \n"));

			Irp->IoStatus.Status = WriteMemory(DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		//
		// Read Memry in defined length
		//

		case IOCTL_PCIDUMPR_BUFFER_READ_MEMORY:
//			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_BUFFER_READ_MEMORY \n"));

			Irp->IoStatus.Status = BufferReadMemory(DeviceObject,
												pIBuffer,
												(PUCHAR)ioBuffer,
												outputBufferLength,
												&byteCount
												);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		//
		// Write Memry in defined length
		//

		case IOCTL_PCIDUMPR_BUFFER_WRITE_MEMORY:
//			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_BUFFER_WRITE_MEMORY \n"));

			Irp->IoStatus.Status = BufferWriteMemory(pIBuffer,
												(PUCHAR)ioBuffer,
												outputBufferLength,
												&byteCount
												);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;


		case IOCTL_PCIDUMPR_READ_BLOCK_EEPROM:
//			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_READ_BLOCK_EEPROM \n"));

			Irp->IoStatus.Status = BufferReadEEPROM(DeviceObject,
												pIBuffer,
												(PUCHAR)ioBuffer,
												outputBufferLength,
												&byteCount
												);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS) 
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		//
		// Write Memry in defined length
		//

		case IOCTL_PCIDUMPR_WRITE_BLOCK_EEPROM:
//			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_WRITE_BLOCK_EEPROM \n"));

			Irp->IoStatus.Status = BufferWriteEEPROM(DeviceObject,
												(PUCHAR)pIBuffer,
												(PUCHAR)ioBuffer,
												outputBufferLength,
												&byteCount
												);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_PCIDUMPR_PROBE_DMA_DATA:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_GEN_DMA \n"));

			Irp->IoStatus.Status = ProbeDmaData(DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS) 
			{
				Irp->IoStatus.Information = byteCount;
			}
		    break;

		case IOCTL_PCIDUMPR_TRANSFER_DMA_DATA_BUFFER:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_TRANSFER_DMA_DATA_BUFFER \n"));

			Irp->IoStatus.Status = TransferDMADataBuffer(
													DeviceObject,
													(PUCHAR)pIBuffer,
													(PUCHAR)ioBuffer,
													outputBufferLength,
													&byteCount
													);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS) 
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;


		case IOCTL_PCIDUMPR_TRANSFER_DMA_DESCRIPTOR:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_TRANSFER_DMA_DESCRIPTOR \n"));

			Irp->IoStatus.Status = TransferDMADescriptor(
													DeviceObject,
													(PUCHAR)pIBuffer,
													(PUCHAR)ioBuffer,
													outputBufferLength,
													&byteCount
													);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_PCIDUMPR_DUMP_DMA:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_DUMP_DMA \n"));

			Irp->IoStatus.Status = DumpDMA(DeviceObject,
										pIBuffer,
										(PUCHAR)ioBuffer,
										outputBufferLength,
										&byteCount
										);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS) 
			{
				Irp->IoStatus.Information = byteCount;
			}

		break;

		case IOCTL_PCIDUMPR_ERASE_DMA:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_ERASE_DMA \n"));

			Irp->IoStatus.Status = EraseDMA(DeviceObject,
										pIBuffer,
										(PUCHAR)ioBuffer,
										outputBufferLength,
										&byteCount
										);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_PCIDUMPR_START_TX:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_START_TX \n"));

			Irp->IoStatus.Status = RTPCIStartTX(DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_PCIDUMPR_START_RX:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_START_RX \n"));

			Irp->IoStatus.Status = RTPCIStartRX(
											DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount	
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;


		case IOCTL_PCIDUMPR_COM_WRITE:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_COM_WRITE \n"));

			Irp->IoStatus.Status = RTPCIComWrite(
											DeviceObject,
											(PUCHAR)pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount	
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_PCIDUMPR_COM_READ:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_COM_READ \n"));

			Irp->IoStatus.Status = RTPCIComRead(
											DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount	
											);

			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

#ifdef	SNIFF
		case IOCTL_PCIDUMPR_SNIFFER:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_SNIFFER \n"));

			Irp->IoStatus.Status = RTPCIStartSniffer(
												DeviceObject,
												pIBuffer,
												(PUCHAR)ioBuffer,
												outputBufferLength,
												&byteCount	
												);
			//
			// If everything was OK, then update the returned data byte count.
			//

			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;	    
#endif

		case IOCTL_ENABLE_BEACON:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_ENABLE_BEACON \n"));

			Irp->IoStatus.Status = EnableBeacon(
											DeviceObject,
											pIBuffer,
											(PUCHAR)ioBuffer,
											outputBufferLength,
											&byteCount	
											);
			//
			// If everything was OK, then update the returned data byte count.
			//
			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_DISABLE_BEACON:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_DISABLE_BEACON \n"));

			Irp->IoStatus.Status = DisableBeacon(
											DeviceObject,									
											&byteCount	
											);
			//
			// If everything was OK, then update the returned data byte count.
			//
			if (Irp->IoStatus.Status == STATUS_SUCCESS)
			{
				Irp->IoStatus.Information = byteCount;
			}
		break;

		case IOCTL_SET_TCPIP_PAYLOAD:
			{
				ULONG* pInBuf = (ULONG*)ioBuffer;

				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_SET_TCPIP_PAYLOAD \n"));
					
				pAd->SetTcpIpPayload = *pInBuf;
				
				//
				// If everything was OK, then update the returned data byte count.
				//
				Irp->IoStatus.Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = 0;
			}
		break;

		case IOCTL_PCIDUMPR_GET_FIRMWARE_VERSION:
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_PCIDUMPR_GET_FIRMWARE_VERSION \n"));

			RtlCopyMemory(ioBuffer,&pAd->FirmwareVersion, sizeof(ULONG));
			Irp->IoStatus.Status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(ULONG);
			
		break;

		case IOCTL_ENABLE_ANTENNA_DIVERSITY:
			{
				ULONG* pInBuf = (ULONG*)ioBuffer;

				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: IOCTL_ENABLE_ANTENNA_DIVERSITY \n"));
				
				pAd->AntennaDiversity = 1;
				pAd->SelAnt = *pInBuf;
				Irp->IoStatus.Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = 0;
			}
		break;

		default:
			Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: unknown IRP_MJ_DEVICE_CONTROL\n"));
		break;
	}

	//
	// As soon as IoCompleteRequest is called, the status field of that IRP
	// is inaccessible.  So fill it in before the call to IoCompleteRequest.
	//

	ntStatus = Irp->IoStatus.Status;

	IoCompleteRequest(				// indicates the caller has completed all processing for a given I/O request and is returning the given IRP to the I/O Manager.
					Irp,			// Points to the IRP to be completed. 
					IO_NO_INCREMENT	// This value is IO_NO_INCREMENT if the original thread requested an operation the driver could complete quickly 
					);

	//
	// We never have pending operation so always return the status code.
	//

//	DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: DispatchDeviceControl <===\n");	

	return ntStatus;
}
#endif // _USB //


/*++

Routine Description:

    Process the IOCTLs sent to this device.

Arguments:

    DeviceObject - Target device object.

    Irp - IRP_MJ_DEVICE_CONTROL

Return Value:

    NTSTATUS

--*/
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject,
									IN PIRP Irp )

{	
#ifdef _USB
	return DispatchDeviceControlUSB(DeviceObject, Irp);
#else
	return DispatchDeviceControlPCI(DeviceObject, Irp);
#endif 
}   // PciDumprDeviceControl

VOID
CsampPollingTimerDpc(
    IN PKDPC Dpc,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
)
 /*++
     Routine Description:
  
         CustomTimerDpc routine to process Irp that are
         waiting in the PendingIrpQueue
  
     Arguments:
  
         DeviceObject    - pointer to DPC object
         Context         - pointer to device object
         SystemArgument1 -  undefined
         SystemArgument2 -  undefined
  
     Return Value:
--*/
{
#if 0
    PDEVICE_OBJECT pDO =  ( PDEVICE_OBJECT)Context;
    RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;

     RTMP_Real_IO_WRITE32(pDevExt, RA_MCU_CMD, 0x2);		
     DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: timer on\n");	
     KeSetTimer(&pDevExt->m_IsrTimer, pDevExt->PollingInterval,
                      &pDevExt->PollingDpc);
#endif
}

//PCIe
BOOLEAN
AsicSetDevMac(
    IN	PDEVICE_OBJECT	pDO,
    IN PUCHAR addr)
{
	/*ULONG val;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Set OwnMac=%02x:%02x:%02x:%02x:%02x:%02x\n",
				__FUNCTION__, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]));
#ifdef _USB
	RTMP_ADAPTER *dx = (RTMP_ADAPTER *)pDO->DeviceExtension;	
#endif

	
	// TODO: shiang-7603, now only configure OWN MAC Address 0
	val = (addr[0]) | (addr[1]<<8) |  (addr[2]<<16) | (addr[3]<<24);
#ifdef _USB
	USBHwHal_WriteMacRegister(dx,RMAC_OMA0R0,val);
#else
	RTMP_Real_IO_WRITE32(pDevExt,RMAC_OMA0R0,val);
#endif

	val = addr[4] | (addr[5]<<8) |  (1 <<16);

#ifdef _USB
	USBHwHal_WriteMacRegister(dx,RMAC_OMA0R1,val);
#else
	RTMP_Real_IO_WRITE32(pDevExt,RMAC_OMA0R1,val);
#endif
	*/
	return TRUE;
}


#ifndef _USB



NTSTATUS
DriverInit(
    IN PDEVICE_OBJECT pDO
    )
{
	NTSTATUS				ntStatus = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
	ULONG				value, data = 0, MAC_Value;
	DBGPRINT(RT_DEBUG_TRACE,("DriverInit====> \n"));	
	//------------------------------------------------------------------
	// Initialize Adapter's variables	

	RtlZeroMemory(&pDevExt->WlanCounters, sizeof(COUNTER_802_11));
	RtlZeroMemory(&pDevExt->OtherCounters, sizeof(OTHER_STATISTICS));
	
	pDevExt->CurRx0Index = 0;
	pDevExt->CurRx1Index = 0;
	
	pDevExt->CurAc0TxIndex = 0;
	pDevExt->NextAc0TxDoneIndex = 0;
	pDevExt->CurAc1TxIndex = 0;
	pDevExt->NextAc1TxDoneIndex = 0;
	pDevExt->CurAc2TxIndex = 0;
	pDevExt->NextAc2TxDoneIndex = 0;
	pDevExt->CurAc3TxIndex = 0;
	pDevExt->NextAc3TxDoneIndex = 0;
	pDevExt->CurAc4TxIndex = 0;
	pDevExt->NextAc4TxDoneIndex = 0;
	pDevExt->CurAc5TxIndex = 0;
	pDevExt->NextAc5TxDoneIndex = 0;
	pDevExt->CurAc6TxIndex = 0;
	pDevExt->NextAc6TxDoneIndex = 0;
	pDevExt->CurAc7TxIndex = 0;
	pDevExt->NextAc7TxDoneIndex = 0;
	pDevExt->CurMgmtTxIndex = 0; // AC8
	pDevExt->NextMgmtTxDoneIndex = 0; // AC8
	pDevExt->CurHccaTxIndex = 0; // AC9
	pDevExt->NextHccaTxDoneIndex = 0; // AC9	

	pDevExt->Ac0_MAX_TX_PROCESS = 0;
	pDevExt->Ac1_MAX_TX_PROCESS = 0;
	pDevExt->Ac2_MAX_TX_PROCESS = 0;
	pDevExt->Ac3_MAX_TX_PROCESS = 0;
	pDevExt->Mgmt_MAX_TX_PROCESS = 0;
	pDevExt->Hcca_MAX_TX_PROCESS = 0;

	g_bDam4kBoundaryCheck = FALSE;
		
#ifdef	SNIFF
	pDevExt->bStartSniff = FALSE;
	pDevExt->PacketBufferDriverIdx = 0;
	pDevExt->PacketBufferSnifferIdx = 0;
	pDevExt->PacketBufferWrapAround = FALSE;
#endif
	
	pDevExt->bStartTx = FALSE;
	pDevExt->bStartRx = FALSE;

	pDevExt->AntennaDiversity = 0;
	pDevExt->SelAnt= 1;
	pDevExt->IsSaveRXVLog = 0 ;
	pDevExt->RXVFileHandle = 0 ;
	//pDevExt->RX0DataCount = 0;
	RtlZeroMemory(&pDevExt->OtherCounters, sizeof(pDevExt->OtherCounters));

	pDevExt->IsFwRsp = FALSE;

	{
		int		Index = 0,i;
		ULONG	MacCsr12;
		ULONG	TempReg, MacValue;
		WPDMA_GLO_CFG_STRUC	GloCfg;

		//
		// Make sure MAC gets ready.
		//
		DBGPRINT(RT_DEBUG_TRACE,("DriverEntry Make sure MAC gets ready. \n"));
		Index = 0;
		/*do --werner
		{
//			pDevExt->MACVersion = RTMP_Real_IO_READ32(pDevExt, RA_ASIC_VERSION);

			if ((pDevExt->MACVersion != 0x00) && (pDevExt->MACVersion != 0xFFFFFFFF))
				break;

			RTMPusecDelay(100);
		} while (Index++ < 1000);*/


		//
#if 1
		//1reset
		//RTMP_IO_READ32(pDevExt, 0x730,&TempReg);
		TempReg = 0;
		// check common register if FW has been loaded
		if((TempReg & 0x01) == 0)
		{			
			//RTMP_IO_READ32(pDevExt, RA_MISC_CTRL,&TempReg);
			TempReg |= 0x80000;
			//RTMP_Real_IO_WRITE32(pDevExt, RA_MISC_CTRL, TempReg);//--werner
			RTMPusecDelay(100);
			TempReg &= (~0x80000);
			//RTMP_Real_IO_WRITE32(pDevExt, RA_MISC_CTRL, TempReg);//--werner
		}	
#endif	
		Index = 0;
		/*do --werner
		{
//			pDevExt->MACVersion = RTMP_Real_IO_READ32(pDevExt, RA_ASIC_VERSION);

			if ((pDevExt->MACVersion != 0x00) && (pDevExt->MACVersion != 0xFFFFFFFF))
				break;

			RTMPusecDelay(100);
		} while (Index++ < 1000);

		DBGPRINT(RT_DEBUG_TRACE,"RA_ASIC_VERSION  [ Ver:Rev=0x%08x]\n", pDevExt->MACVersion);*/

		TXWI_SIZE = GetTXWISize (pDevExt);
		Testdbg("RA_ASIC_VERSION : 0x%08X, TXWI_SIZE = %d\n", pDevExt->MACVersion, TXWI_SIZE);

		//DBGPRINT(RT_DEBUG_TRACE,"QA_Driver: FUNCTION: %s, LINE%d\n",__FUNCTION__,__LINE__);

		//DebugPrintWhereAmI();

		//RTMPEnableWlan(pDevExt, TRUE);	// For MT7650
#if 0
		if ( IS_MT7610(pDevExt->MACVersion) )
		{
			//------------------------------------------------------------------
			// Disable FW CPU before init the DMA pointer
			MAC_Value = RTMP_Real_IO_READ32(pDevExt, RESET_CTL);
			MAC_Value &= 0xFFFFFDFF; // bit9=0 to disable FW CPU
			RTMP_Real_IO_WRITE32(pDevExt, RESET_CTL, MAC_Value);
			RTMPusecDelay(50);
		}
#endif
		// To fix driver disable/enable hang issue when radio off
		//RTMP_Real_IO_WRITE32(pDevExt, RA_PWR_PIN_CFG, 0x2);

		/*
		// Disable DMA.
//		GloCfg.word = RTMP_Real_IO_READ32(pDevExt, RA_WPDMA_GLO_CFG);
		GloCfg.word &= 0xff0;
		GloCfg.field.EnTXWriteBackDDONE = 0;
		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, GloCfg.word);		

		// 3. Set DMA global configuration except TX_DMA_EN and RX_DMA_EN bits:
		Index = 0;
		do
		{
//			GloCfg.word = RTMP_Real_IO_READ32(pDevExt, RA_WPDMA_GLO_CFG);
			if ((GloCfg.field.TxDMABusy == 0)  && (GloCfg.field.RxDMABusy == 0))
				break;
			
			RTMPusecDelay(1000);
			Index++;
		} while(Index < 100);
		DBGPRINT(RT_DEBUG_TRACE,"<== DMA offset 0x208 = 0x%x\n", GloCfg.word);
		Testdbg("<== DMA offset 0x208 = 0x%x\n", GloCfg.word);
		GloCfg.word &= 0xff0;
		GloCfg.field.EnTXWriteBackDDONE = 0;
		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, GloCfg.word);
		
		// asic simulation sequence put this ahead before loading firmware.
		// pbf hardware reset
		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_RST_IDX, 0x1003f);	// 0x10000 for reset rx, 0x3f resets all 6 tx rings.
		RTMP_Real_IO_WRITE32(pDevExt, RA_SYS_CTRL, 0x00080C0C);
		RTMPusecDelay(100);
		RTMP_Real_IO_WRITE32(pDevExt, RA_SYS_CTRL, 0x00080C00);
		RTMPusecDelay(1000);

		//
		// Before program BBP, we need to wait BBP/RF get wake up.
		//
		Index = 0;
		do
		{
//			MacCsr12 = RTMP_Real_IO_READ32(pDevExt, RA_MAC_STATUS_REG);

			if ((MacCsr12 & 0x03) == 0)	// if BB.RF is stable
				break;
			
			DBGPRINT(RT_DEBUG_TRACE,"Check RA_MAC_STATUS_REG  = Busy = %x\n", MacCsr12);
			RTMPusecDelay(1000);
		} while (Index++ < 100);

		*/
	}

	
	// DMA initialization
	InitDMA(pDevExt);

	// Initialize Asic
	//Eeprom_Init(pDevExt);

	// Set Delay INT CFG
	//RTMP_Real_IO_WRITE32(pDevExt, RA_DELAY_INT_CFG, DELAY_VALUE);	

	// Enable interrupt			
	pDevExt->IntrMask = IRQ_MASK;
	RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, pDevExt->IntrMask|IRQ_MSK_SPECIAL_BITS);		

	// MAC TX/RX Enable	
	/*RTMP_Real_IO_WRITE32(pDevExt, RA_MAC_SYS_CTRL, 0xC);		

	// Enable bit1 for RX Queue in RA_PBF_CFG	
//	value = RTMP_Real_IO_READ32(pDevExt, RA_PBF_CFG);
	data = value | 0x00000002;
	RTMP_Real_IO_WRITE32(pDevExt, RA_PBF_CFG, data);
	*/

	// Disable L0s
	{
		ULONG	Bus = 0, length = 0, propertyAddress = 0;
		ULONG	Function = 0, Device = 0, Value = 0;

		//
		// Scan the PCI slots and dump the PCI configuration data to the
		// user's buffer.
		//
	    ReadWriteConfigSpace(pDO, 
								0, // 0 for read 1 for write
								&Value,
								0x80,
								4
								);
		Value &= 0xfffffefc;
		ReadWriteConfigSpace(pDO, 
								1, // 0 for read 1 for write
								&Value,
								0x80,
								4
								);
	}
	//DebugPrintWhereAmI();
{
	USHORT	Bus = 0, Slot = 0, Func = 0;
	ULONG	SubBus = 0, MTKSubBus=0, length = 0;
	ULONG	Vendor = 0;
	ULONG	DeviceID = 0;
	ULONG	Class = 0;
	ULONG	Configuration = 0;
	ULONG	offset = 0, i=0;

	IoGetDeviceProperty(pDevExt->pPhyDeviceObj,
						DevicePropertyBusNumber,
						sizeof(ULONG),
						(PVOID)&MTKSubBus,
						&length);
	//DebugPrintWhereAmI();
	// 2.Find PCI host, and its subbus is the same as Ralink's bus.
	for (Bus = 0; Bus < MAX_PCI_BUS; Bus++)
	{
		//DebugPrintWhereAmI();
		for (Slot = 0; Slot < MAX_PCI_DEVICE; Slot++)
		{
			//DebugPrintWhereAmI();
			for (Func = 0; Func < MAX_FUNC_NUM; Func++)
			{
				Vendor    = RTMPReadCBConfig(Bus, Slot, Func, 0x00);
				DeviceID = Vendor & 0xFF000000;
				Vendor    = Vendor & 0x0000FFFF;
				// Skip non-exist deice right away
				if (Vendor == UNKNOWN)
					continue;
				// AMD PCI host seems to has problem. So only support Intel PCI bus .
				// If Find Intel Vendor ID. Always use toggle.
				if (Vendor == PCIBUS_INTEL_VENDOR)
				{
//					bToggle = TRUE;
//					RTMP_SET_PSFLAG(pAd, fRTMP_PS_TOGGLE_L1);
//					pAd->HostVendor = Vendor;
				}
				else if ((Vendor == PCIBUS_AMD_VENDOR1) || (Vendor == PCIBUS_ATI_VENDOR1) || (Vendor == PCIBUS_NVIDIA_VENDOR1))
				{
//					pAd->HostVendor = Vendor;
					//Verified Aspire 8530 AMD NB (S3/S4/CBoot/WBoot/Chariot). 
					// So use L1 Toggle method in this NB.
					// L1 toggle for AMD, default should be Off, unless the flag is set
//					if (pAd->StaCfg.PSControl.field.AMDNewPSOn == TRUE)
//					{
//						bToggle = TRUE;
//						RTMP_SET_PSFLAG(pAd, fRTMP_PS_TOGGLE_L1);
//						DBGPRINT(RT_DEBUG_TRACE, ("PSM : Aspire 8530 AMD NB. Use L1 Toggle. \n"));
//					}
				}
				//DebugPrintWhereAmI();
				Class     = RTMPReadCBConfig(Bus, Slot, Func, 0x08) >> 16;
				SubBus    = RTMPReadCBConfig(Bus, Slot, Func, 0x18);
				SubBus &= 0xff00;
				SubBus = (SubBus >> 8);
		
				if ((Class == CARD_PCIBRIDGEPCI_CLASS) && (SubBus == MTKSubBus))
				{
 //					pAd->HBus = Bus;
//					pAd->HSubBus = SubBus;
//					pAd->HSlot = Slot;
//					pAd->HFunc = Func;
					// find LinkControl offset according to PCI spec.
					offset = 0x34;
					i = 0;
					do 
					{
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, offset);
						if ((Configuration&0xff) == 0x10)
						{	
							offset += 0x10;
//							pAd->LnkCtrlOffset = offset;
//							LinkControlSetting.field.HostLinkCOffset = offset;
//							LinkControlSetting.field.HostFunc = Func;
//							PciSetting.field.HostBus = Bus;
//							PciSetting.field.HostSlot = Slot;
//							PciHostSetting.field.VendorId = Vendor;
							DeviceID = DeviceID>>16;
//							PciHostSetting.field.DeviceId = DeviceID;
							// Save those bus information to Registry.
//							RTMPSetDisableFindHostDeviceFlag(pAd, PciSetting.word, LinkControlSetting.word, PciHostSetting.word);
							Configuration = RTMPReadCBConfig(Bus, Slot, Func, offset);
							Configuration &= 0xfffffefc;
							Configuration |= (0x0);

							// Force L1 for test
//							if (pAd->StaCfg.PSControl.field.rt30xxForceASPMTest == 1)
//								bToggle = TRUE;
#if 1
							RTMPWriteCBConfig(Bus, Slot, Func, offset, Configuration);
#else
							if ((pAd->DeviceID == NIC2860_PCIe_DEVICE_ID) ||
								(pAd->DeviceID == NIC2790_PCIe_DEVICE_ID))
							{
								RTMPWriteCBConfig(Bus, Slot, Func, offset, Configuration);
								DBGPRINT(RT_DEBUG_TRACE, ("PSM :Write Host offset 0x%x = 0x%x\n", offset, Configuration));
								DBGPRINT(RT_DEBUG_TRACE, ("PSM :<=== HBus = %d, HSubBus = %d, HSlot = %d, HFunc = %d. \n", pAd->HBus, pAd->HSubBus, pAd->HSlot, pAd->HFunc));
							}
							else if  (IS_DEV_RT3xxx(pAd)	&& (bToggle == FALSE))
							{
								// Before thorough test in AMD platforms, set to L0 first in AMD platform.
								RTMPWriteCBConfig(Bus, Slot, Func, offset, Configuration);
								pAd->Rt3xxHostLinkCtrl = Configuration;
								DBGPRINT(RT_DEBUG_TRACE, ("PSM : rt30xx AMD Write Host offset 0x%x = 0x%x, LnkCtrlOffset = %x \n", offset, Configuration, pAd->LnkCtrlOffset));
								DBGPRINT(RT_DEBUG_TRACE, ("PSM : rt30xx AMD <=== HBus = %d, HSubBus = %d, HSlot = %d, HFunc = %d. \n", pAd->HBus, pAd->HSubBus, pAd->HSlot, pAd->HFunc));
							}
#endif
							break;
						}
						else if (offset == 0x34)
						{
							offset = Configuration&0xff;
						}
						else
						{
							offset = (Configuration&0xff00) >> 8;
						}
						i++;
					}while (i < 10);
					break;
				}
			}
		}
	}
}
//DebugPrintWhereAmI();
	{
		////////////////////////////////////////////
		// Read MCFG table
		////////////////////////////////////////////
	        HANDLE				KeyHandle=NULL;
	        OBJECT_ATTRIBUTES	ObjectAttributes;
		UNICODE_STRING		UnicodeUSBPRINTRegPath;
		UNICODE_STRING		ValueName;
		PKEY_VALUE_PARTIAL_INFORMATION	KeyValueInformation = NULL;
		ULONG				ResultLength1, ResultLength2, iIdx=0;
		const char			szBuf[8]={0}, MCFG[] = {"MCFG"};
		const char* Ptr = NULL;

		RtlInitUnicodeString(&UnicodeUSBPRINTRegPath, L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Services\\mssmbios\\Data");
		RtlInitUnicodeString(&ValueName, L"AcpiData");

		InitializeObjectAttributes(&ObjectAttributes, &UnicodeUSBPRINTRegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	        ntStatus = ZwOpenKey(&KeyHandle, KEY_QUERY_VALUE, &ObjectAttributes);
//DebugPrintWhereAmI();
		ntStatus = ZwQueryValueKey(
								KeyHandle,
								&ValueName,
								KeyValuePartialInformation,
								KeyValueInformation,
								sizeof(KEY_VALUE_PARTIAL_INFORMATION),
								&ResultLength1
								);
//DebugPrintWhereAmI();
		KeyValueInformation = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, ResultLength1, 'MgeR');

		ntStatus = ZwQueryValueKey(
								KeyHandle,
								&ValueName,
								KeyValuePartialInformation,
								KeyValueInformation,
								ResultLength1,
								&ResultLength2
								);

		Ptr = (const char*)KeyValueInformation->Data;

		for (iIdx=0; iIdx<ResultLength1-sizeof(MCFG); iIdx++)
		{
			memcpy((void*)szBuf, (const void*)(Ptr + iIdx), strlen(MCFG));
			if ( strcmp(szBuf, MCFG) == 0 )
			{
				pDevExt->ConfigurationSpacePhyAddr.LowPart = *((ULONG*)(Ptr + iIdx + 0x2C));
				pDevExt->ConfigurationSpacePhyAddr.HighPart = *((ULONG*)(Ptr + iIdx + 0x30));
//				DbgPrint("pDevExt->ConfigurationSpacePhyAddr.LowPart  = 0x%08X", pDevExt->ConfigurationSpacePhyAddr.LowPart);
//				DbgPrint("pDevExt->ConfigurationSpacePhyAddr.HighPart  = 0x%08X", pDevExt->ConfigurationSpacePhyAddr.HighPart);
				break;
			}
		}
	//DebugPrintWhereAmI();	
		ExFreePoolWithTag(KeyValueInformation, 'MgeR');
		ntStatus = ZwClose(KeyHandle);
	}
//DebugPrintWhereAmI();
	{
		////////////////////////////////////////////
		// Read MCFG table
		////////////////////////////////////////////
		RTL_OSVERSIONINFOW	OSVersionInfo;
		OSVersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

		if ( RtlGetVersion((RTL_OSVERSIONINFOW*)&OSVersionInfo) == STATUS_SUCCESS)
			pDevExt->OSMajorVersion = OSVersionInfo.dwMajorVersion;
		else
			pDevExt->OSMajorVersion = 5; // XP
	}

	return ntStatus;
}
#endif


#ifdef RTMP_PCI_SUPPORT
NTSTATUS
BufferReadMemory(
	IN PDEVICE_OBJECT DeviceObject,
    IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    )
/*++

Routine Description:
	Read memory from assign location in defined format

Arguments:

    UserBuffer - Pointer to the user's storage location for the PCI configuration
                 data.

    BufferMaxSize - The maximum size of the user's storage area.

    ByteCount - The running count of the number of bytes currently stored in the
                user's buffer.  At the end of the searching, this is how many
                bytes to be returned to the caller.


Return Value:

    STATUS_SUCCESS if successful.
    Various NTSTATUS values if unsuccessful.

--*/
{
    NTSTATUS	ntStatus = STATUS_SUCCESS;
	ULONG		InBuf[5];
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;

	InBuf[0] = *(pIBuffer+0);					//offset
	InBuf[1] = *(pIBuffer+1);					//length
	InBuf[2] = *(pIBuffer+2);					//value

	READ_REGISTER_BUFFER_ULONG((PULONG)pDevExt->virtualaddr + InBuf[0], (PULONG)UserBuffer, InBuf[1]/4);

	DBGPRINT(RT_DEBUG_TRACE,("IOCTL: BufferReadMemory()\n"));
	
	*ByteCount = InBuf[1];
    return ntStatus;
}   // BufferReadMemory
#endif /* RTMP_PCI_SUPPORT */


NTSTATUS
BufferWriteMemory(
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    )
/*++

Routine Description:

  	Read memory from assign location in defined length

Arguments:

    UserBuffer - Pointer to the user's storage location for the PCI configuration
                 data.

    BufferMaxSize - The maximum size of the user's storage area.

    ByteCount - The running count of the number of bytes currently stored in the
                user's buffer.  At the end of the searching, this is how many
                bytes to be returned to the caller.


Return Value:

    STATUS_SUCCESS if successful.
    Various NTSTATUS values if unsuccessful.

--*/
{
    NTSTATUS		ntStatus = STATUS_SUCCESS;
    ULONG			InBuf[5], Reg, Buf;
	
	InBuf[0] = *(pIBuffer);							//busnum
	InBuf[1] = *(pIBuffer+1);						//address
	InBuf[2] = *(pIBuffer+2);						//offset
	InBuf[3] = *(pIBuffer+3);						//length
	InBuf[4] = *(pIBuffer+4);						//value	

	Reg = InBuf[1]+InBuf[2];
	WRITE_REGISTER_BUFFER_ULONG((PULONG) &Reg, (PULONG)(pIBuffer+5), InBuf[3]/4);

//	DBGPRINT(RT_DEBUG_TRACE,"IOCTL: BufferWriteMemory()\n");
	
	*ByteCount = 0;
    return ntStatus;
}   // BufferWriteMemory


#ifdef RTMP_PCI_SUPPORT
NTSTATUS
ReadMemory(
	IN PDEVICE_OBJECT DeviceObject,
    IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    )
/*++

Routine Description:
	Read memory from assign location in defined format

Arguments:

    UserBuffer - Pointer to the user's storage location for the PCI configuration
                 data.

    BufferMaxSize - The maximum size of the user's storage area.

    ByteCount - The running count of the number of bytes currently stored in the
                user's buffer.  At the end of the searching, this is how many
                bytes to be returned to the caller.


Return Value:

    STATUS_SUCCESS if successful.
    Various NTSTATUS values if unsuccessful.

--*/
{
    NTSTATUS	ntStatus = STATUS_SUCCESS;
	ULONG		InBuf[128];
	ULONG		data[128];
	ULONG temp = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;
	RtlZeroMemory(data,sizeof(data));
	InBuf[0] = *(pIBuffer+0);			//offset
	InBuf[1] = *(pIBuffer+1);			//length
	InBuf[2] = *(pIBuffer+2);			//value
//	Testdbg("IOCTL: ReadMemory_4B[0x%08x] = 0x%08x\n", InBuf[0]), InBuf[2];	
	if(InBuf[0] &0xFF000000)
	{
		DBGPRINT(RT_DEBUG_TRACE,("IOCTL: Error ReadMemory[0x%08x] = 0x%08x !!!!!\n", InBuf[0], InBuf[2]));
		*ByteCount = 0;
   		return ntStatus;
	}

	if(InBuf[1] == 1)
	{
		*data = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0]);
//		DBGPRINT(RT_DEBUG_TRACE,"IOCTL: ReadMemory_1B[0x%02x] = 0x%02x\n", (UCHAR)InBuf[0], (UCHAR)data);				
	}	
	else if(InBuf[1] == 2)
	{
		if ( (InBuf[0] % 2) == 0 )
		{
			*data = READ_REGISTER_USHORT((PUSHORT)pDevExt->virtualaddr + (InBuf[0]/2));
//			DBGPRINT(RT_DEBUG_TRACE,"IOCTL: ReadMemory_2B[0x%04x] = 0x%04x\n", (USHORT)InBuf[0], (USHORT)data);
		}
		else
		{
			ULONG	tmp;
			*data	= 0;
			tmp = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0]);
			*data |= tmp;
			tmp = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0] + 1);
			*data |= tmp<<8;
		}
	}
	else if(InBuf[1] == 4)
	{
		if ( (InBuf[0] % 4) == 0 )
		{
			*data = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + (InBuf[0]/4));
//			DBGPRINT(RT_DEBUG_TRACE,"IOCTL: ReadMemory_4B[0x%08x] = 0x%08x\n", InBuf[0]), data;
	//		Testdbg("IOCTL: ReadMemory_4B[0x%08x] = 0x%08x\n", InBuf[0]), InBuf[2];
		}
		else
		{
			ULONG	tmp;
			*data	= 0;
			tmp = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0]);
			*data |= tmp;
			tmp = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0] + 1);
			*data |= tmp<<8;
			tmp = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0] + 2);
			*data |= tmp<<16;
			tmp = READ_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + InBuf[0] + 3);
			*data |= tmp<<24;
		}
	}
	/*else if(InBuf[1] == 8)
	{
		if ( (InBuf[0] % 4) == 0 )
		{
			data[0] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + (InBuf[0]/4));
			data[1] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + ((4+InBuf[0])/4));
		}
		
	}
	else if(InBuf[1] == 12)
	{
		if ( (InBuf[0] % 4) == 0 )
		{
			data[0] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + (InBuf[0]/4));
			data[1] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + ((4+InBuf[0])/4));
			data[2] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + ((8+InBuf[0])/4));
		}
		
	}
	else if(InBuf[1] == 16)
	{
		if ( (InBuf[0] % 4) == 0 )
		{
			temp = InBuf[1];
			temp = temp/4;
			for (ULONG i=0 ;i<temp ;i++)
			{
				data[i] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + ((i*4+InBuf[0])/4));
			}			
		}
		
	}*/
	else
	{
		if ( (InBuf[0] % 4) == 0 )
		{
			ULONG i;
			
			temp = InBuf[1];
			temp = temp/4;
			for (i=0 ;i<temp ;i++)
			{
				data[i] = READ_REGISTER_ULONG((PULONG)pDevExt->virtualaddr + ((i*4+InBuf[0])/4));
			}			
		}
	}


	RtlCopyMemory(UserBuffer,data,InBuf[1]);

    *ByteCount = InBuf[1];
	return ntStatus;
}   // ReadMemory


NTSTATUS
WriteMemory(
	IN PDEVICE_OBJECT DeviceObject,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    )
/*++

Routine Description:
  	Read memory from assign location in defined format

Arguments:
    UserBuffer - Pointer to the user's storage location for the PCI configuration
                 data.
    BufferMaxSize - The maximum size of the user's storage area.
    ByteCount - The running count of the number of bytes currently stored in the
                user's buffer.  At the end of the searching, this is how many
                bytes to be returned to the caller.


Return Value:
    STATUS_SUCCESS if successful.
    Various NTSTATUS values if unsuccessful.

--*/
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    ULONG InBuf[5];
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)DeviceObject->DeviceExtension;
	
	InBuf[0] = *(pIBuffer + 0);			//offset
	InBuf[1] = *(pIBuffer + 1);			//length
	InBuf[2] = *(pIBuffer + 2);			//value

	if(InBuf[0] &0xFF000000)
	{
		DBGPRINT(RT_DEBUG_TRACE,("IOCTL: Error WriteMemory[0x%08x] = 0x%08x !!!!!\n", InBuf[0], InBuf[2]));
		*ByteCount = 0;
   		return ntStatus;
	}

	if(InBuf[1] == 1)
	{
		WRITE_Real_REGISTER_UCHAR(((PUCHAR)pDevExt->virtualaddr + InBuf[0]), (UCHAR)InBuf[2]);
//		DBGPRINT(RT_DEBUG_TRACE,"IOCTL: WriteMemory_1B[0x%02x] = 0x%02x\n", (UCHAR)InBuf[0], (UCHAR)InBuf[2]);				
	}	
	else if(InBuf[1] == 2)
	{
		WRITE_Real_REGISTER_USHORT(((PUSHORT)pDevExt->virtualaddr + InBuf[0]), (USHORT)InBuf[2]);
//		DBGPRINT(RT_DEBUG_TRACE,"IOCTL: WriteMemory_2B[0x%04x] = 0x%04x\n", (USHORT)InBuf[0], (USHORT)InBuf[2]);				
	}
	else if(InBuf[1] == 4)
	{
		WRITE_Real_REGISTER_ULONG(((PULONG)pDevExt->virtualaddr + InBuf[0]), (ULONG)InBuf[2]);
//		DBGPRINT(RT_DEBUG_TRACE,"IOCTL: WriteMemory_4B[0x%08x] = 0x%08x\n", InBuf[0], InBuf[2]);		
	}
	
	*ByteCount = 0;
    return ntStatus;
}   // WriteMemory


ULONG g_CntIsr = 0;
ULONG g_CntDpc = 0;

BOOLEAN Isr (IN PKINTERRUPT pIntObj,
			IN PVOID pServiceContext ) //msi
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pServiceContext;
//	INTSRC_STRUC	IntSource;
	BOOLEAN			RetValue = FALSE;//TRUE;
	ULONG			MaskValue, IntSourceValue, Tmp;
	
	// Disable interrupt

	DBGPRINT(RT_DEBUG_TRACE,("Isr()  ==>\n"));

	RTMP_IO_READ32(pDevExt, RA_INT_MASK,&MaskValue);
	RTMP_IO_READ32(pDevExt, RA_INT_STATUS,&IntSourceValue);
	DBGPRINT(RT_DEBUG_TRACE,("IntSourceValue %x\n",IntSourceValue));
	DBGPRINT(RT_DEBUG_TRACE,("MaskValue %x\n",MaskValue));
	MaskValue &= IRQ_MSK_SPECIAL_BITS;
	if ( (MaskValue&IRQ_MSK_SPECIAL_BITS) != IRQ_MSK_SPECIAL_BITS )
	{
		DBGPRINT(RT_DEBUG_TRACE,("MaskValue&IRQ_MSK_SPECIAL_BITS = %x return FALSE\n",MaskValue&IRQ_MSK_SPECIAL_BITS));
		return FALSE;
	}
	
	RTMP_IO_READ32(pDevExt, RA_INT_STATUS,&IntSourceValue);
	if ( IntSourceValue == 0 )
	{
		DBGPRINT(RT_DEBUG_TRACE,("IntSourceValue = %x ,return FALSE\n",MaskValue));
		return FALSE;
	}
	
	//pDevExt->IntrMask = (IRQ_MASK)&0xFFFFFFFF;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, 0 /*pDevExt->IntrMask & ~IRQ_MSK_SPECIAL_BITS*/);
	RTMP_IO_READ32(pDevExt, RA_INT_MASK,&MaskValue);
	
	//
	// Get the interrupt sources & saved to local variable
	//

	RTMP_IO_READ32(pDevExt, RA_INT_STATUS,&pDevExt->IntSource );
	RTMP_Real_IO_WRITE32(pDevExt, RA_INT_STATUS, pDevExt->IntSource);
	RTMP_IO_READ32(pDevExt, RA_INT_STATUS,&Tmp);
	
	DBGPRINT(RT_DEBUG_TRACE,("HWM_R[RA_INT_STATUS]  = %08x \n", pDevExt->IntSource));
	
	if(!(pDevExt->IntSource & IRQ_MASK))
	{
		RetValue = FALSE;
		
		pDevExt->IntrMask =0;	
		RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, pDevExt->IntrMask|IRQ_MSK_SPECIAL_BITS);
		RTMP_IO_READ32(pDevExt, RA_INT_STATUS,&Tmp);
		
		goto EXIT;
	}

	//Queue DPC routine
	RetValue = TRUE;
	IoRequestDpc(pDevExt->pDeviceObject, NULL, pDevExt);			

EXIT:
		g_CntIsr++;
		DBGPRINT(RT_DEBUG_TRACE,("g_CntIsr = 0x%08x\n", g_CntIsr));
		DBGPRINT(RT_DEBUG_TRACE,("g_CntDpc = 0x%08x\n", g_CntDpc));		
		
		DBGPRINT(RT_DEBUG_TRACE,("Isr()  <==\n"));

	return RetValue;
}


//++
// Function:
//		RTPCIDpcForIsr
//
// Description:
//		This function performs the low-IRQL
//		post-processing of I/O requests
//
// Arguments:
//		Pointer to a DPC object
//		Pointer to the Device object
//		Pointer to the IRP for this request
//		Pointer to the Device Extension
//
// Return Value:
//		(None)
//--
VOID
RTPCIDpcForIsr(	IN PKDPC pDpc,
			IN PDEVICE_OBJECT pDevObj,
			IN PIRP pIrp,
			IN PVOID pContext )
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	ULONG	IntMaskValue;
//	INTSRC_STRUC	IntSource;

	DBGPRINT(RT_DEBUG_TRACE,("RTPCIDpcForIsr()  ==>\n"));

	//
	// Get the interrupt sources & saved to local variable
	//
	
	//IntSource.word = RTMP_Real_IO_READ32(pDevExt, RA_INT_STATUS);
	
	DBGPRINT(RT_DEBUG_TRACE,("HWM_R[RA_INT_STATUS]  = %08x \n",pDevExt->IntSource));
	
	//RTMP_Real_IO_WRITE32(pDevExt, RA_INT_STATUS, IntSource.word);		

	//
	// Handle interrupt, walk through all bits
	// Should start from highest priority interrupt
	// The priority can be adjust by altering processing if statement
	//
	// If required spinlock, each interrupt service routine has to acquire and release itself.
	//
	
	if (pDevExt->IntSource&INT_Bit_ANY_TxDone)	
	{
		
		if (pDevExt->IntSource&INT_Bit_Ac0TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc0TxRingTxDoneInterrupt Check bit[4]\n"));
			RTMPHandleAc0TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac1TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc1TxRingTxDoneInterrupt Check bit[5]\n"));
			RTMPHandleAc1TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac2TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc2TxRingTxDoneInterrupt Check bit[6]\n"));
			RTMPHandleAc2TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac3TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc3TxRingTxDoneInterrupt Check bit[7]\n"));
			RTMPHandleAc3TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac4TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc4TxRingTxDoneInterrupt Check bit[8]\n"));
			RTMPHandleAc4TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac5TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc5TxRingTxDoneInterrupt Check bit[9]\n"));
			RTMPHandleAc5TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac6TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc6TxRingTxDoneInterrupt Check bit[10]\n"));
			RTMPHandleAc6TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac7TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc7TxRingTxDoneInterrupt Check bit[11]\n"));
			RTMPHandleAc7TxRingTxDoneInterrupt(pDevExt);
		}

		//++werner-->
		if (pDevExt->IntSource&INT_Bit_Ac8TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc8TxRingTxDoneInterrupt Check bit[12]\n"));
			//RTMPHandleAc7TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac9TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc9TxRingTxDoneInterrupt Check bit[13]\n"));
			//RTMPHandleAc7TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac10TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc10TxRingTxDoneInterrupt Check bit[14]\n"));
			//RTMPHandleAc7TxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_Ac11TxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleAc11TxRingTxDoneInterrupt Check bit[15]\n"));
			//RTMPHandleAc7TxRingTxDoneInterrupt(pDevExt);
		}
		//++werner<--

		if (pDevExt->IntSource&INT_Bit_MgmtTxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleMgmtTxRingTxDoneInterrupt Check bit[16]\n"));
			RTMPHandleMgmtTxRingTxDoneInterrupt(pDevExt);
		}

		if (pDevExt->IntSource&INT_Bit_HccaTxDone)	
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTMPHandleHccaTxRingTxDoneInterrupt Check bit[17]\n"));
			RTMPHandleHccaTxRingTxDoneInterrupt(pDevExt);
		}	
	 }


	if (pDevExt->IntSource & INT_Bit_ANY_RxDone)
	{
		if(pDevExt->IntSource &INT_Bit_RX0TxDone)
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTPCIHandleRx0DoneInterrupt\n"));
			RTPCIHandleRx0DoneInterrupt(pDevExt);
		}

		if(pDevExt->IntSource &INT_Bit_RX1TxDone)
		{
			DBGPRINT(RT_DEBUG_TRACE,("====> RTPCIHandleRx1DoneInterrupt\n"));
			RTPCIHandleRx1DoneInterrupt(pDevExt);
		}
		
		
	}

	
	//
	// Re-enable the interrupt (disabled in Isr)
	//

	pDevExt->IntrMask = 0;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, pDevExt->IntrMask|IRQ_MSK_SPECIAL_BITS);
	RTMP_IO_READ32(pDevExt, RA_INT_MASK,&IntMaskValue);

	g_CntDpc++;
	DBGPRINT(RT_DEBUG_TRACE,("g_CntIsr = 0x%08x\n", g_CntIsr));
	DBGPRINT(RT_DEBUG_TRACE,("g_CntDpc = 0x%08x\n", g_CntDpc));		

	DBGPRINT(RT_DEBUG_TRACE,("TransmittedCount = %d\n", pDevExt->OtherCounters.TransmittedCount));	
	
	DBGPRINT(RT_DEBUG_TRACE,("RTPCIDpcForIsr()  <==\n"));
/*	
	if ( pDevExt->WlanCounters.CRCErrorCount .LowPart != 0)
	{
			DBGPRINT(RT_DEBUG_TRACE,"error stop\n");
			RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, 0);		
	}
*/	
	
}

VOID
 RTPCIHandleRx1DoneInterrupt(
			IN PVOID pContext )
{
	UCHAR				Count=0;
	PUCHAR				pData;
	PRXD_STRUC			pRxD, pSnifRxD;
	PRXINFO_STRUC		pRxInfo;
	PHEADER_802_11		pHeader;
	NTSTATUS				Status = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	ULONG				RX_CNT, RX_DRX_IDX, RX_CRX_IDX, value;
	USHORT				SeqNum;
	int					i;
	LONG				length;
	ULONG				TmpValue;
	int					Temp;
	ULONG				SchReg4;
	PULONG		pTemULONGptr;
	PUCHAR		pTemUCHARptr;
	BOOLEAN			bPrintReceiveData;
	FwCMDRspTxD_STRUC FwCMDRspTxD;
	//	BOOLEAN					bPayload_Error = FALSE;
	ULONG *pUlong = NULL;
	RX_INFO_DW0 RxInfoDW0;
	RX_V_GROUP3 RxVG3;
	CMD_CH_PRIVILEGE_T ChannelPayload;
	RXV			rxv;
	RXV_HEADER rxv_header;
	char			rxvPrintBuffer[1024];
	UNICODE_STRING     uniName;
    	OBJECT_ATTRIBUTES  objAttr;
	IO_STATUS_BLOCK    ioStatusBlock;

	KeAcquireSpinLockAtDpcLevel(&pDevExt->RxRingMainLock);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck: RTPCIHandleRx1DoneInterrupt()\n"));
	//Testdbg("PciDumpr: RXCheck: RTPCIHandleRxDoneInterrupt()\n");

	RTMP_IO_READ32(pDevExt, RA_FS_DRX_IDX1,&RX_DRX_IDX);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RX1Check:DTX = %d\n",RX_DRX_IDX));

	if (RX_DRX_IDX > pDevExt->CurRx1Index) 
		RX_CNT = RX_DRX_IDX -pDevExt->CurRx1Index;
	else
		RX_CNT = RX_DRX_IDX + RX0_RING_SIZE - pDevExt->CurRx1Index;

	do
	{
		PULONG	ptr;
		
		//====================================================//
		// Processing FW response
		//====================================================//
		/*{
			PFW_RXD_STRUC	pFwRxD;
			pFwRxD = (PFW_RXD_STRUC) pDevExt->Rx1RingMain[pDevExt->CurRx1Index].AllocVa;

			if (pFwRxD->DDONE != 1)
			{
				break;
			}
			
			if ( pFwRxD->INFO_TYPE == INFO_TYPE_RSP_EVT )
			{
				Testdbg("1111pFwRxD->INFO_TYPE == INFO_TYPE_RSP_EVT\n");
				Testdbg("1111pFwRxD->PktLength = %d, sizeof(FW_RXD_STRUC) = %d\n", pFwRxD->PktLength, sizeof(FW_RXD_STRUC));
				RtlCopyMemory(&pDevExt->FwReceiveBuffer, (UCHAR*)pFwRxD, pFwRxD->PktLength + sizeof(FW_RXD_STRUC));

				pDevExt->IsFwRsp = TRUE;
			}
		}*/
		//====================================================//
		
		pRxD = (PRXD_STRUC) pDevExt->Rx1RingMain[pDevExt->CurRx1Index].AllocVa;

		ptr = (PULONG)pRxD;
		DBGPRINT(RT_DEBUG_TRACE,("RX1 RXD 0 0x%08X\n", *ptr));
		DBGPRINT(RT_DEBUG_TRACE,("RX1 RXD 1 0x%08X\n", *(ptr+1)));
		DBGPRINT(RT_DEBUG_TRACE,("RX1 RXD 2 0x%08X\n", *(ptr+2)));
		DBGPRINT(RT_DEBUG_TRACE,("RX1 RXD 3 0x%08X\n", *(ptr+3)));

		if (pRxD->DDONE != 1)
		{
			DBGPRINT(RT_DEBUG_TRACE,("RxD->DDONE != 1 !!!!!!\n"));
			//break;
		}

		do
		{
			PRX_WI_STRUC pRxWI;
			
			if ( pDevExt->IsFwRsp == TRUE )
			{
				pDevExt->IsFwRsp = FALSE;
				break;
			}
			
			pData	= (PUCHAR) (pDevExt->Rx1RingMain[pDevExt->CurRx1Index].DmaBuf.AllocVa);

			pRxInfo = (PRXINFO_STRUC) pData;
			bPrintReceiveData = FALSE;
			// Cast to 802.11 header for flags checking

			//pHeader	= (PHEADER_802_11) ((PUCHAR)pData+sizeof(RXINFO_STRUC)+GetRXWISize(pDevExt));//--werner	
			pHeader	= (PHEADER_802_11) ((PUCHAR)pData);//++werner	
			pRxWI = (PRX_WI_STRUC) (pData+sizeof(RXINFO_STRUC)); 
			pDevExt->OtherCounters.ReceivedCount++;
			//++werner-> dump receive data
			
			RTMP_IO_READ32(pDevExt, RA_SCHEDULER_REG4,&SchReg4);
			if(SchReg4&0x00000020)
			{
				PTX_WI_STRUC pTXWI;
				
				//DBGPRINT(RT_DEBUG_TRACE,"ReceivedCount=%d\n", pDevExt->OtherCounters.ReceivedCount);
				DBGPRINT(RT_DEBUG_TRACE,("ReceivedCount=%d\n", pDevExt->OtherCounters.ReceivedCount));

				//compare tx data and rx data 
				pTXWI=  (PTX_WI_STRUC) ((PUCHAR)pData);
				//compare length
				if(pRxD->SDL0!=g_ulTXPacketLength)
				{
					DBGPRINT(RT_DEBUG_TRACE,("Packet compare error, RX data length=%d  PacketLength=%d\n", pRxD->SDL0,g_ulTXPacketLength));
					bPrintReceiveData = TRUE;
				}
				
				pTemULONGptr = (PULONG)pHeader;

				//compare TXD first, skip DW0
				if(memcmp((pTemULONGptr+1),(g_TxDPattern+4),sizeof(TX_WI_STRUC)-4)!=0)
				{
					DBGPRINT(RT_DEBUG_TRACE,("TXD compare ,fail\n"));
					bPrintReceiveData = TRUE;
				}

				//compare payload g_PayloadPattern
				if(memcmp((pTemULONGptr+TXWI_SIZE/4),g_PayloadPattern,g_ulTXPacketLength-TXWI_SIZE)!=0)
				{
					DBGPRINT(RT_DEBUG_TRACE,("Payload compare ,fail\n"));
					bPrintReceiveData = TRUE;
				}


				//++werner-> dump receive data			
				Temp = pRxD->SDL0;

				if(bPrintReceiveData)
				{
					PULONG	ptr;
					
					Temp = pRxD->SDL0;
					DBGPRINT(RT_DEBUG_TRACE,("pRxD->SDL0= %d = 0x%x  Temp=%d\n", pRxD->SDL0, pRxD->SDL0,Temp));
					for (i = 0; i < Temp/4; i++)
					{
						ptr = (PULONG)pHeader;
						DBGPRINT(RT_DEBUG_TRACE,("Receive 4byte %d   0x%08X\n",i, *(ptr+i)));
					}
					//print TXD
					Temp = sizeof(g_TxDPattern);
					for (i = 0; i < Temp/4; i++)
					{
						ptr = (PULONG)g_TxDPattern;
						DBGPRINT(RT_DEBUG_TRACE,("g_TxDPattern  %d   0x%08X\n",i, *(ptr+i)));
					}

					//print g_PayloadPattern, payload.
					Temp = g_ulTXPacketLength - sizeof(g_TxDPattern);
					for (i = 0; i < Temp/4; i++)
					{
						ptr = (PULONG)g_PayloadPattern;
						DBGPRINT(RT_DEBUG_TRACE,("g_PayloadPattern  %d   0x%08X\n",i, *(ptr+i)));
					}

					
				}
				else
					DBGPRINT(RT_DEBUG_TRACE,("Rx1 Receive compare success\n"));

		
				//++werner<- dump receive data
			}
			

			

			///////////////////////////////////////////////////////////
			// Check for all RxD errors
			if (pRxInfo->CrcErr)
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTPCIHandleRx1DoneInterrupt CrcErr=%d ,break\n",pRxInfo->CrcErr));
				break; // give up this frame
			}

			if (pRxInfo->IcvErr || pRxInfo->MicErr)
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTPCIHandleRx1DoneInterrupt IcvErr=%d ,MicErr=%d ,break\n",pRxInfo->IcvErr,pRxInfo->MicErr));
				break;
			}

			// Check Last Section
			if (pRxD->LS0 != 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTPCIHandleRx1DoneInterrupt pRxD->LS0 =%d != 1,break \n",pRxD->LS0));
				break; // give up this frame
			}
			
//			if (pRxD->SDL0 < (GetRXWISize(pDevExt)+sizeof(RXINFO_STRUC)))
//			{
//				break; // give up this frame
//			}
			//////////////////////////////////////////////////////////////
#if 0
			length =  pRxWI->MPDU_SIZE;

			length = length - 24 - 1;
			DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: RXCheck payload length= %x\n", length);		
			if ( ( length < 2600) && (length > 0))
			{
				for ( i=0;i< length;i++)
				{
					if ((UCHAR) *(pData + GetRXWISize(pDevExt) + 24 + i + 1)  != ((UCHAR)(*(pData + GetRXWISize(pDevExt) + 24 + i ) + 1) ))
					{
						bPayload_Error = TRUE;
						DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: RXCheck payload byte error idx = %d \n",i);
						break;
					}
				}
			}
			else
			{
				bPayload_Error = TRUE;
			}
#endif

			//check if this packet is FW download rsp or packet cmd
			Temp = pRxD->SDL0;
			DBGPRINT(RT_DEBUG_TRACE,("%s SDL0 = %d, sizeof(FwCMDRspTxD) = %d\n",__FUNCTION__,Temp,sizeof(FwCMDRspTxD)));
			pUlong = (ULONG*)pData;			
			for (i=0; i<Temp/4; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("%s RX DW%d  0x%08x\n",__FUNCTION__,i,*(pUlong+i)));
			}

			if(pRxD->SDL0>=sizeof(FwCMDRspTxD) )
			{
				memcpy(&FwCMDRspTxD,pData,sizeof(FwCMDRspTxD));
				if(0xE000==FwCMDRspTxD.FwEventTxD.u2PacketType)
				{	
					if(FwCMDRspTxD.FwEventTxD.ucSeqNum <= MAX_FW_DOWNLOAD_SEQ)
					{//fw download seq num
						g_FWSeqMCU = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
						g_FWRspStatus = (UCHAR)FwCMDRspTxD.ucStatus;
						DBGPRINT(RT_DEBUG_TRACE,("%s FW download  get seqnum = %d, from MCU  ,g_FWRspStatus =%d\n",__FUNCTION__,g_FWSeqMCU,g_FWRspStatus));

						//switch channel event seq is 0, so detect here.
						if (FW_PKT_CMD_CH_PRIVILEGE_EVENT==FwCMDRspTxD.FwEventTxD.ucEID)
						{
							if ( (sizeof(FwCMDRspTxD)+sizeof(CMD_CH_PRIVILEGE_T)) == FwCMDRspTxD.FwEventTxD.u2RxByteCount)
							{
								pData += sizeof(FwCMDRspTxD);
								memcpy(&ChannelPayload,pData,sizeof(ChannelPayload));	
								g_PacketCMDSeqMCU = ChannelPayload.ucTokenID;

								DBGPRINT(RT_DEBUG_TRACE,("%s Get switchchannel seqnum =%d \n",__FUNCTION__, g_PacketCMDSeqMCU));
							}
							else
							{
								DBGPRINT(RT_DEBUG_TRACE,("%s u2RxByteCount =%d, != sizeof(FwCMDRspTxD)+sizeof(CMD_CH_PRIVILEGE_T), error  !!!\n",__FUNCTION__, FwCMDRspTxD.FwEventTxD.u2RxByteCount));
							}

						}
					} 
					else
					{//packet cmd
						g_PacketCMDSeqMCU  = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
						DBGPRINT(RT_DEBUG_TRACE,("%s Packet CMD get seqnum = %d, from MCU, u2RxByteCount=%d\n",__FUNCTION__,g_PacketCMDSeqMCU,FwCMDRspTxD.FwEventTxD.u2RxByteCount ));
						if( (FwCMDRspTxD.FwEventTxD.u2RxByteCount-sizeof(FwTxD_STRUC))<= sizeof(g_PacketCMDRspData))
						{
							memset(g_PacketCMDRspData,0,sizeof(g_PacketCMDRspData));
							memcpy(g_PacketCMDRspData,pData,FwCMDRspTxD.FwEventTxD.u2RxByteCount);
						}
						else
						{
							DBGPRINT(RT_DEBUG_TRACE,("%s u2RxByteCount=%d > sizeof(g_PacketCMDRspData)(%d), error!!!!!\n",__FUNCTION__,FwCMDRspTxD.FwEventTxD.u2RxByteCount,sizeof(g_PacketCMDRspData)));
						}
						
					}
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE,("u2PacketType =0x%X not from FW(MCU) \n",FwCMDRspTxD.FwEventTxD.u2PacketType));
				}
			}

			//check if the packet is RX vector
#ifndef _USB//always PCIe function here
			if(pRxD->SDL0>4)
			{
				HandleRXVector(pData,pDevExt);				
			}
#endif
			//
			// Do RxD release operation	for	all	failure	frames
			//
			if(Status == STATUS_SUCCESS)
			{
				// pData : Pointer skip	the	first 24 bytes,	802.11 HEADER
				//
				// Start of	main loop to parse receiving frames.
				// The sequence	will be	Type first,	then subtype...
				//			            
				DBGPRINT(RT_DEBUG_TRACE,("pRxInfo->CrcErr = %d\n", pRxInfo->CrcErr));
				if(!pRxInfo->CrcErr)
				{			
					/*pDevExt->OtherCounters.RSSI0 = pRxWI->RSSI_0;
					pDevExt->OtherCounters.RSSI1 = pRxWI->RSSI_1;
					pDevExt->OtherCounters.RSSI2 = pRxWI->RSSI_2;
					pDevExt->OtherCounters.SNR0 = pRxWI->SNR_0;
					pDevExt->OtherCounters.SNR1 = pRxWI->SNR_1;

					pDevExt->OtherCounters.UdpErr = pRxD->UDPerr;
					pDevExt->OtherCounters.TcpErr = pRxD->TCPerr;
					pDevExt->OtherCounters.IpErr = pRxD->IPerr;*/

					DBGPRINT(RT_DEBUG_TRACE,("pHeader->Controlhead.Frame.Type = %d\n", pHeader->Controlhead.Frame.Type));
					switch (pHeader->Controlhead.Frame.Type)
					{
						case BTYPE_DATA:
							if (pRxInfo->U2M)
							{
								INC_COUNTER(pDevExt->WlanCounters.U2MDataCount);
							}
							else	// ( !U2M || Mcast || Bcast )
							{
								INC_COUNTER(pDevExt->WlanCounters.OtherDataCount);
							}
							break;

						case BTYPE_MGMT:
						case BTYPE_CNTL:
							if(pHeader->Controlhead.Frame.Subtype == SUBTYPE_BEACON)
							{
								INC_COUNTER(pDevExt->WlanCounters.BeaconCount);
							}
							else
							{
								INC_COUNTER(pDevExt->WlanCounters.othersCount);
							}
							break;

						default	:
							DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck:  Unknow data type Error\n"));
							break;
					}
				}	 
			}

// Sniffer Function
#ifdef	SNIFF
			if(pDevExt->bStartSniff == TRUE)
			{
				if((pDevExt->PacketBufferDriverIdx >= RX_RING_PACKET_BUFFER) && (pDevExt->PacketBufferWrapAround == FALSE))
				{
					pDevExt->PacketBufferDriverIdx = 0;
					pDevExt->PacketBufferWrapAround = TRUE;

					DBGPRINT(RT_DEBUG_TRACE,("RxDone ===> Wrap Around!\n"));
				}
				else if((pDevExt->PacketBufferDriverIdx >= RX_RING_PACKET_BUFFER) && (pDevExt->PacketBufferWrapAround == TRUE))
				{
					pDevExt->PacketBufferDriverIdx = 0;
					pDevExt->PacketBufferWrapAround = TRUE;

					DBGPRINT(RT_DEBUG_TRACE,("RxDone ===> Error!!! Buffer Full!\n"));
				}

				pSnifRxD = (PRXD_STRUC) ((PUCHAR)pDevExt->pPacketBuf + pDevExt->PacketBufferDriverIdx * MAX_FRAME_SIZE);


				RtlCopyMemory((PUCHAR)pDevExt->pPacketBuf + (pDevExt->PacketBufferDriverIdx * MAX_FRAME_SIZE), 
								pRxD, RING_RX_DESCRIPTOR_SIZE);						// Descriptor
				RtlCopyMemory((PUCHAR)pDevExt->pPacketBuf + (pDevExt->PacketBufferDriverIdx * MAX_FRAME_SIZE) + RING_RX_DESCRIPTOR_SIZE,
								pData, pRxD->SDL0);

				DBGPRINT(RT_DEBUG_TRACE,("RxDone ===> pDevExt->PacketBufferdriverIdx = %d\n",pDevExt->PacketBufferDriverIdx));

				pDevExt->PacketBufferDriverIdx++;
			}
#endif
		}while (FALSE);

		pRxD->DDONE= 0;
		pRxD->SDL0 = 8192;
		pRxD->LS0 = 0;

		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck: SetHw pRxD->DDONE = 0\n"));

		RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX1, pDevExt->CurRx1Index);
		pDevExt->CurRx1Index++;

		if (pDevExt->CurRx1Index >= RX1_RING_SIZE)
		{
			pDevExt->CurRx1Index = 0;
		}

		Count++;

		RTMP_IO_READ32(pDevExt, RA_RX_CALC_IDX1,&TmpValue);

	}while (Count < RX_CNT);

	KeReleaseSpinLockFromDpcLevel(&pDevExt->RxRingMainLock);

	DBGPRINT(RT_DEBUG_TRACE,("%s()<--\n",__FUNCTION__));
}


VOID
RTPCIHandleRx0DoneInterrupt(
			IN PVOID pContext )
{
	UCHAR				Count=0;
	PUCHAR				pData;
	PRXD_STRUC			pRxD, pSnifRxD;
	PRXINFO_STRUC		pRxInfo;
	PHEADER_802_11		pHeader;
	NTSTATUS				Status = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	ULONG				RX_CNT, RX_DRX_IDX, RX_CRX_IDX, value;
	USHORT				SeqNum;
	int					i;
	LONG				length;
	ULONG				TmpValue;
	int					Temp;
	ULONG				SchReg4;
	BOOLEAN					bPrintReceiveData;
//	BOOLEAN					bPayload_Error = FALSE;
	PULONG		pTemULONGptr;
	PUCHAR		pTemUCHARptr;
	ULONG iPattern2Location;
	FwCMDRspTxD_STRUC FwCMDRspTxD;
	RX_V_GROUP1 group1;
	memset(&group1,0,sizeof(group1));
	
	/*if(pDevExt->IsSaveRXVLog)//test
	{
		//RX_V_GROUP1 group1;
		WriteRXVToFile(pDevExt->RXVFileHandle,group1);
	}*/

	KeAcquireSpinLockAtDpcLevel(&pDevExt->RxRingMainLock);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck: RTPCIHandleRx0DoneInterrupt()\n"));
	//Testdbg("PciDumpr: RXCheck: RTPCIHandleRxDoneInterrupt()\n");

	RTMP_IO_READ32(pDevExt, RA_FS_DRX_IDX0,&RX_DRX_IDX);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck:DTX = %d\n",RX_DRX_IDX));
	//Testdbg("PciDumpr: RXCheck:DTX = %d\n",RX_DRX_IDX);
	if (RX_DRX_IDX > pDevExt->CurRx0Index) 
		RX_CNT = RX_DRX_IDX -pDevExt->CurRx0Index;
	else
		RX_CNT = RX_DRX_IDX + RX0_RING_SIZE - pDevExt->CurRx0Index;

	do
	{
		PULONG	ptr;
		
		bPrintReceiveData = FALSE;
		//====================================================//
		// Processing FW response
		//====================================================//
		{
			PFW_RXD_STRUC	pFwRxD;
			pFwRxD = (PFW_RXD_STRUC) pDevExt->Rx0RingMain[pDevExt->CurRx0Index].AllocVa;

			if (pFwRxD->DDONE != 1)
			{
				break;
			}
			
			if ( pFwRxD->INFO_TYPE == INFO_TYPE_RSP_EVT )
			{
				Testdbg("1111pFwRxD->INFO_TYPE == INFO_TYPE_RSP_EVT\n");
				Testdbg("1111pFwRxD->PktLength = %d, sizeof(FW_RXD_STRUC) = %d\n", pFwRxD->PktLength, sizeof(FW_RXD_STRUC));
				RtlCopyMemory(&pDevExt->FwReceiveBuffer, (UCHAR*)pFwRxD, pFwRxD->PktLength + sizeof(FW_RXD_STRUC));

				pDevExt->IsFwRsp = TRUE;
			}
		}
		//====================================================//
		
		pRxD = (PRXD_STRUC) pDevExt->Rx0RingMain[pDevExt->CurRx0Index].AllocVa;
		ptr = (PULONG)pRxD;
		DBGPRINT(RT_DEBUG_TRACE,("RX0 RXD 0 0x%08X\n", *ptr));
		DBGPRINT(RT_DEBUG_TRACE,("RX0 RXD 1 0x%08X\n", *(ptr+1)));
		DBGPRINT(RT_DEBUG_TRACE,("RX0 RXD 2 0x%08X\n", *(ptr+2)));
		DBGPRINT(RT_DEBUG_TRACE,("RX0 RXD 3 0x%08X\n", *(ptr+3)));	

		if(*ptr&0x70000000)
		{
			pDevExt->OtherCounters.Rx0ReceivedCount++;
			DBGPRINT(RT_DEBUG_TRACE,("Driver Rx Count = %d\n",pDevExt->OtherCounters.Rx0ReceivedCount));
		}

		if (pRxD->DDONE != 1)
		{
			break;
		}

		do
		{
			PRX_WI_STRUC pRxWI;
			PTX_WI_STRUC pTXWI;
			
			if ( pDevExt->IsFwRsp == TRUE )
			{
				pDevExt->IsFwRsp = FALSE;
				break;
			}
			
			pData	= (PUCHAR) (pDevExt->Rx0RingMain[pDevExt->CurRx0Index].DmaBuf.AllocVa);

			pRxInfo = (PRXINFO_STRUC) pData;

			// Cast to 802.11 header for flags checking

			//pHeader	= (PHEADER_802_11) ((PUCHAR)pData+sizeof(RXINFO_STRUC)+GetRXWISize(pDevExt));//--werner	
			pHeader	= (PHEADER_802_11) ((PUCHAR)pData);//++werner	
			pRxWI = (PRX_WI_STRUC) (pData+sizeof(RXINFO_STRUC)); 

			pDevExt->OtherCounters.ReceivedCount++;
			//++werner-> dump receive data
			
			RTMP_IO_READ32(pDevExt, RA_SCHEDULER_REG4,&SchReg4);
			if(SchReg4&0x00000020)//loopback mode
			{	
				//DBGPRINT(RT_DEBUG_TRACE,"ReceivedCount=%d\n", pDevExt->OtherCounters.ReceivedCount);
				DBGPRINT(RT_DEBUG_TRACE,("ReceivedCount=%d\n", pDevExt->OtherCounters.ReceivedCount));
				//compare tx data and rx data 
				pTXWI=  (PTX_WI_STRUC) ((PUCHAR)pData);
				//compare length
				if(pRxD->SDL0!=g_ulTXPacketLength)
				{
					DBGPRINT(RT_DEBUG_TRACE,("Packet compare error, RX data length=%d  PacketLength=%d\n", pRxD->SDL0,g_ulTXPacketLength));
					bPrintReceiveData = TRUE;
				}
				
				pTemULONGptr = (PULONG)pHeader;

				//compare TXD first, skip DW0
				if(memcmp((pTemULONGptr+1),(g_TxDPattern+4),sizeof(TX_WI_STRUC)-4)!=0)
				{
					DBGPRINT(RT_DEBUG_TRACE,("TXD compare ,fail\n"));
					bPrintReceiveData = TRUE;
				}

				//compare payload g_PayloadPattern
				if(memcmp((pTemULONGptr+TXWI_SIZE/4),g_PayloadPattern,g_ulTXPacketLength-TXWI_SIZE)!=0)
				{
					DBGPRINT(RT_DEBUG_TRACE,("Payload compare ,fail\n"));
					bPrintReceiveData = TRUE;
				}
				
				if(bPrintReceiveData)
				{
					Temp = pRxD->SDL0;
					DBGPRINT(RT_DEBUG_TRACE,("pRxD->SDL0= %d = 0x%x  Temp=%d\n", pRxD->SDL0, pRxD->SDL0,Temp));
					for (i = 0; i < Temp/4; i++)
					{
						PULONG	ptr = (PULONG)pHeader;
						DBGPRINT(RT_DEBUG_TRACE,("Receive 4byte %d   0x%08X\n",i, *(ptr+i)));
					}
					//print TXD
					Temp = sizeof(g_TxDPattern);
					for (i = 0; i < Temp/4; i++)
					{
						PULONG	ptr = (PULONG)g_TxDPattern;
						DBGPRINT(RT_DEBUG_TRACE,("g_TxDPattern  %d   0x%08X\n",i, *(ptr+i)));
					}

					//print g_PayloadPattern, payload.
					Temp = g_ulTXPacketLength - sizeof(g_TxDPattern);
					for (i = 0; i < Temp/4; i++)
					{
						PULONG	ptr = (PULONG)g_PayloadPattern;
						DBGPRINT(RT_DEBUG_TRACE,("g_PayloadPattern  %d   0x%08X\n",i, *(ptr+i)));
					}

					
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE,("Rx0 Receive compare success\n"));
					//for test
					Temp = pRxD->SDL0;
					DBGPRINT(RT_DEBUG_TRACE,("pRxD->SDL0= %d = 0x%x  Temp=%d\n", pRxD->SDL0, pRxD->SDL0,Temp));
					for (i = 0; i < Temp/4; i++)
					{
						PULONG	ptr = (PULONG)pHeader;
						DBGPRINT(RT_DEBUG_TRACE,("Receive 4byte %d   0x%08X\n",i, *(ptr+i)));
					}
					//print TXD
					Temp = sizeof(g_TxDPattern);
					for (i = 0; i < Temp/4; i++)
					{
						PULONG	ptr = (PULONG)g_TxDPattern;
						DBGPRINT(RT_DEBUG_TRACE,("g_TxDPattern  %d   0x%08X\n",i, *(ptr+i)));
					}

					//print g_PayloadPattern, payload.
					Temp = g_ulTXPacketLength - sizeof(g_TxDPattern);
					for (i = 0; i < Temp/4; i++)
					{
						PULONG	ptr = (PULONG)g_PayloadPattern;
						DBGPRINT(RT_DEBUG_TRACE,("g_PayloadPattern  %d   0x%08X\n",i, *(ptr+i)));
					}

				}
							
				
			}
			else
			{//normal receive data
				Temp = pRxD->SDL0;
				if(Temp>=80)
				{
					Temp = 80;
				}
				DBGPRINT(RT_DEBUG_TRACE,("=======Dump receive packet===Max 20 dword======\n"));
				DBGPRINT(RT_DEBUG_TRACE,("receive length=%d\n", Temp));
				ptr = (PULONG)pHeader;
				for (i = 0; i < Temp/4; i++)
				{					
					DBGPRINT(RT_DEBUG_TRACE,("Receive 4byte %d   0x%08X\n",i, *(ptr+i)));					
				}
				DBGPRINT(RT_DEBUG_TRACE,("=======================================\n"));

#ifndef _USB//always PCIe function here
				CheckSecurityResult(ptr,pDevExt);
#endif
			}
			//++werner<- dump receive data

			///////////////////////////////////////////////////////////
			// Check for all RxD errors
			/*if (pRxInfo->CrcErr)
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTPCIHandleRx0DoneInterrupt CrcErr=%d ,break\n",pRxInfo->CrcErr));
				break; // give up this frame
			}

			if (pRxInfo->IcvErr || pRxInfo->MicErr)
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTPCIHandleRx0DoneInterrupt IcvErr=%d ,MicErr=%d ,break\n",pRxInfo->IcvErr,pRxInfo->MicErr));
				break;
			}*/

			// Check Last Section
			if (pRxD->LS0 != 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("RTPCIHandleRx0DoneInterrupt pRxD->LS0 =%d != 1,break \n",pRxD->LS0));
				break; // give up this frame
			}
			
//			if (pRxD->SDL0 < (GetRXWISize(pDevExt)+sizeof(RXINFO_STRUC)))
//			{
//				break; // give up this frame
//			}

			//check if this packet is FW download rsp or packet cmd
			if(pRxD->SDL0==sizeof(FwCMDRspTxD) )
			{
				memcpy(&FwCMDRspTxD,pData,sizeof(FwCMDRspTxD));
				if(0xE000==FwCMDRspTxD.FwEventTxD.u2PacketType)
				{	
					if(FwCMDRspTxD.FwEventTxD.ucSeqNum <= MAX_FW_DOWNLOAD_SEQ)
					{//fw download seq num
						g_FWSeqMCU = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
						g_FWRspStatus = (UCHAR)FwCMDRspTxD.ucStatus;
						DBGPRINT(RT_DEBUG_TRACE,("%s FW download  get seqnum = %d, from MCU  ,g_FWRspStatus =%d\n",__FUNCTION__,g_FWSeqMCU,g_FWRspStatus));
					}
					else
					{//packet cmd
						g_PacketCMDSeqMCU  = (UCHAR)FwCMDRspTxD.FwEventTxD.ucSeqNum;
						DBGPRINT(RT_DEBUG_TRACE,("%s Packet CMD get seqnum = %d, from MCU, u2RxByteCount=%d\n",__FUNCTION__,g_PacketCMDSeqMCU,FwCMDRspTxD.FwEventTxD.u2RxByteCount ));
						if( (FwCMDRspTxD.FwEventTxD.u2RxByteCount-sizeof(FwTxD_STRUC))<= sizeof(g_PacketCMDRspData))
						{
							memset(g_PacketCMDRspData,0,sizeof(g_PacketCMDRspData));
							memcpy(g_PacketCMDRspData,pData+sizeof(FwTxD_STRUC),FwCMDRspTxD.FwEventTxD.u2RxByteCount-sizeof(FwTxD_STRUC));
						}
						else
						{
							DBGPRINT(RT_DEBUG_TRACE,("%s u2RxByteCount-12 =0%d > sizeof  g_PacketCMDRspData, error!!!!!\n",__FUNCTION__,FwCMDRspTxD.FwEventTxD.u2RxByteCount-12));
						}
						
					}
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE,("u2PacketType =0x%X error \n",FwCMDRspTxD.FwEventTxD.u2PacketType));
				}
			}
			



			//////////////////////////////////////////////////////////////
#if 0
			length =  pRxWI->MPDU_SIZE;

			length = length - 24 - 1;
			DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: RXCheck payload length= %x\n", length);		
			if ( ( length < 2600) && (length > 0))
			{
				for ( i=0;i< length;i++)
				{
					if ((UCHAR) *(pData + GetRXWISize(pDevExt) + 24 + i + 1)  != ((UCHAR)(*(pData + GetRXWISize(pDevExt) + 24 + i ) + 1) ))
					{
						bPayload_Error = TRUE;
						DBGPRINT(RT_DEBUG_TRACE,"PciDumpr: RXCheck payload byte error idx = %d \n",i);
						break;
					}
				}
			}
			else
			{
				bPayload_Error = TRUE;
			}
#endif
			//
			// Do RxD release operation	for	all	failure	frames
			//
			if(Status == STATUS_SUCCESS)
			{
				// pData : Pointer skip	the	first 24 bytes,	802.11 HEADER
				//
				// Start of	main loop to parse receiving frames.
				// The sequence	will be	Type first,	then subtype...
				//			            
				DBGPRINT(RT_DEBUG_TRACE,("pRxInfo->CrcErr = %d\n", pRxInfo->CrcErr));
				if(!pRxInfo->CrcErr)
				{			
					pDevExt->OtherCounters.RSSI0 = pRxWI->RSSI_0;
					pDevExt->OtherCounters.RSSI1 = pRxWI->RSSI_1;
					pDevExt->OtherCounters.RSSI2 = pRxWI->RSSI_2;
					pDevExt->OtherCounters.SNR0 = pRxWI->SNR_0 -16;
					pDevExt->OtherCounters.SNR1 = pRxWI->SNR_1 -16;

					pDevExt->OtherCounters.UdpErr = pRxD->UDPerr;
					pDevExt->OtherCounters.TcpErr = pRxD->TCPerr;
					pDevExt->OtherCounters.IpErr = pRxD->IPerr;

					DBGPRINT(RT_DEBUG_TRACE,("pHeader->Controlhead.Frame.Type = %d\n", pHeader->Controlhead.Frame.Type));
					switch (pHeader->Controlhead.Frame.Type)
					{
						case BTYPE_DATA:
							if (pRxInfo->U2M)
							{
								INC_COUNTER(pDevExt->WlanCounters.U2MDataCount);
							}
							else	// ( !U2M || Mcast || Bcast )
							{
								INC_COUNTER(pDevExt->WlanCounters.OtherDataCount);
							}
							break;

						case BTYPE_MGMT:
						case BTYPE_CNTL:
							if(pHeader->Controlhead.Frame.Subtype == SUBTYPE_BEACON)
							{
								INC_COUNTER(pDevExt->WlanCounters.BeaconCount);
							}
							else
							{
								INC_COUNTER(pDevExt->WlanCounters.othersCount);
							}
							break;

						default	:
							DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck:  Unknow data type Error\n"));
							break;
					}
				}	 
			}

// Sniffer Function
#ifdef	SNIFF
			if(pDevExt->bStartSniff == TRUE)
			{
				if((pDevExt->PacketBufferDriverIdx >= RX_RING_PACKET_BUFFER) && (pDevExt->PacketBufferWrapAround == FALSE))
				{
					pDevExt->PacketBufferDriverIdx = 0;
					pDevExt->PacketBufferWrapAround = TRUE;

					DBGPRINT(RT_DEBUG_TRACE,("RxDone ===> Wrap Around!\n"));
				}
				else if((pDevExt->PacketBufferDriverIdx >= RX_RING_PACKET_BUFFER) && (pDevExt->PacketBufferWrapAround == TRUE))
				{
					pDevExt->PacketBufferDriverIdx = 0;
					pDevExt->PacketBufferWrapAround = TRUE;

					DBGPRINT(RT_DEBUG_TRACE,("RxDone ===> Error!!! Buffer Full!\n"));
				}

				pSnifRxD = (PRXD_STRUC) ((PUCHAR)pDevExt->pPacketBuf + pDevExt->PacketBufferDriverIdx * MAX_FRAME_SIZE);

				//jliao [040816 - P4]
				//pRxD->High32TSF = RTMP_Dummy_IO_READ32(pDevExt, CSR17);		// TSF value
				//pRxD->Low32TSF = RTMP_Dummy_IO_READ32(pDevExt, CSR16);		// TSF vlaue

				RtlCopyMemory((PUCHAR)pDevExt->pPacketBuf + (pDevExt->PacketBufferDriverIdx * MAX_FRAME_SIZE), 
								pRxD, RING_RX_DESCRIPTOR_SIZE);						// Descriptor
				RtlCopyMemory((PUCHAR)pDevExt->pPacketBuf + (pDevExt->PacketBufferDriverIdx * MAX_FRAME_SIZE) + RING_RX_DESCRIPTOR_SIZE,
								pData, pRxD->SDL0);

				DBGPRINT(RT_DEBUG_TRACE,("RxDone ===> pDevExt->PacketBufferdriverIdx = %d\n",pDevExt->PacketBufferDriverIdx));

				pDevExt->PacketBufferDriverIdx++;
			}
#endif
		}while (FALSE);

		pRxD->DDONE= 0;
		pRxD->SDL0 = 8192;
		pRxD->LS0 = 0;

		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RXCheck: SetHw pRxD->DDONE = 0\n"));

		RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX0, pDevExt->CurRx0Index);
		pDevExt->CurRx0Index++;

		if (pDevExt->CurRx0Index >= RX0_RING_SIZE)
		{
			pDevExt->CurRx0Index = 0;
		}

		Count++;

		RTMP_IO_READ32(pDevExt, RA_RX_CALC_IDX0,&TmpValue);

	}while (Count < RX_CNT);

	KeReleaseSpinLockFromDpcLevel(&pDevExt->RxRingMainLock);
}

VOID
RTMPHandleAc0TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc0RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX0, TX_DTX_IDX0, FREE, Tmp_Length;
	PULONG				pulTemp;


	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc0TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc0TxIndex= %d, NextAc0TxDoneIndex= %d\n", pDevExt->CurAc0TxIndex,pDevExt->NextAc0TxDoneIndex));	

		Testdbg("\n");
		Testdbg("PciDumpr:  CurAc0TxIndex= %d, NextAc0TxDoneIndex= %d\n", pDevExt->CurAc0TxIndex,pDevExt->NextAc0TxDoneIndex);	


		pTxD = (PTXD_STRUC)	pDevExt->Ac0RingMain[pDevExt->NextAc0TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
		{
			Testdbg("pTxD->DDONE != 1\n");			
			Testdbg("pDevExt->Ac0_MAX_TX_PROCESS = %d\n", pDevExt->Ac0_MAX_TX_PROCESS);
			Testdbg("pDevExt->CurAc0TxIndex = %d\n", pDevExt->CurAc0TxIndex);
			Testdbg("pDevExt->NextAc0TxDoneIndex = %d\n", pDevExt->NextAc0TxDoneIndex);
			break;
		}

//		Testdbg("pDevExt->CurAc0TxIndex = %d\n", pDevExt->CurAc0TxIndex);
//		Testdbg("pDevExt->NextAc0TxDoneIndex = %d\n", pDevExt->NextAc0TxDoneIndex);
		if(pTxD->DDONE)
		{
			pTxD->DDONE = 0;
			DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok, set DDONE=0\n"));
		}
		else
		{			
			DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE isn't trueok, may error happen\n"));	
		}
		
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac0TxedCount++;		
		DBGPRINT(RT_DEBUG_TRACE,("TXT: TransmittedCount=%d  Ac0TxedCount=%d\n",pDevExt->OtherCounters.TransmittedCount,pDevExt->OtherCounters.Ac0TxedCount));
		DBGPRINT(RT_DEBUG_TRACE,("Count = %d  ,  pDevExt->Ac0_MAX_TX_PROCESS = %d\n", Count, pDevExt->Ac0_MAX_TX_PROCESS));
		//Testdbg ("Count = %d  ,  pDevExt->Ac0_MAX_TX_PROCESS = %d\n", Count, pDevExt->Ac0_MAX_TX_PROCESS);
		RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX0,&TX_CTX_IDX0);
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  RA_TX_CTX_IDX0 = %d\n", TX_CTX_IDX0));
		//Testdbg ("PciDumpr:  RA_TX_CTX_IDX0 = %d\n", TX_CTX_IDX0);
		RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX0,&TX_DTX_IDX0);
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  RA_TX_DTX_IDX0 = %d\n", TX_DTX_IDX0));
		//Testdbg ("PciDumpr:  RA_TX_DTX_IDX0 = %d\n", TX_DTX_IDX0);
		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
			//Testdbg("TXT: transmit next packet\n");
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                                         
			if (TX_DTX_IDX0 > pDevExt->CurAc0TxIndex) 
				FREE = TX_DTX_IDX0 -pDevExt->CurAc0TxIndex -1;
			else
				FREE = TX_DTX_IDX0 + AC0_RING_SIZE -pDevExt->CurAc0TxIndex -1;

			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextAc0TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac0RingMain[j].DmaBuf.AllocVa, 0);
					}
					else
					{
						int j=pDevExt->NextAc0TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac0RingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}
			
			SEQdbg ("FREE = %d\n", FREE);
			if (  FREE >=1 )
			{					       
                            
				pTxD = (PTXD_STRUC)	pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					//Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pAc0RingTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
					//pAc0RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc0RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc0RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				if(pDevExt->FrameType == 17)		// Random length
				{				   
					DBGPRINT(RT_DEBUG_TRACE,("%s() Random length\n",__FUNCTION__));
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc0RingTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
					//pAc0RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc0RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc0RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					DBGPRINT(RT_DEBUG_TRACE,("%s()Inc length\n",__FUNCTION__));
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	
					
					pTxD->SDL0 = pDevExt->CurLength;	

					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}

					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc0RingTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
					//pAc0RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc0RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					Testdbg("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0);
					Testdbg("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length);
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc0RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;
                            
				pDevExt->CurAc0TxIndex++;
				if(pDevExt->CurAc0TxIndex >= AC0_RING_SIZE)				
					pDevExt->CurAc0TxIndex = 0;
				DBGPRINT(RT_DEBUG_TRACE,("CurAc0TxIndex++ = %d\n", pDevExt->CurAc0TxIndex));
				Testdbg ("CurAc0TxIndex++ = %d\n", pDevExt->CurAc0TxIndex);

			}
			
			DBGPRINT(RT_DEBUG_TRACE,("Write CurAc0TxIndex = %d\n", pDevExt->NextAc0TxDoneIndex));
			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX0,	pDevExt->CurAc0TxIndex  );			
			
		}

		pDevExt->NextAc0TxDoneIndex++;
		if (pDevExt->NextAc0TxDoneIndex >= AC0_RING_SIZE)
			pDevExt->NextAc0TxDoneIndex = 0;
		DBGPRINT(RT_DEBUG_TRACE,("NextAc0TxDoneIndex++ = %d\n", pDevExt->NextAc0TxDoneIndex));
		Testdbg ("NextAc0TxDoneIndex++ = %d\n", pDevExt->NextAc0TxDoneIndex);

	}while(++Count < pDevExt->Ac0_MAX_TX_PROCESS);
}

VOID
RTMPHandleAc1TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc1RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX1, TX_DTX_IDX1, FREE, Tmp_Length;
	PULONG				pulTemp;


	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc1TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc1TxIndex= %d, NextAc1TxDoneIndex= %d\n", pDevExt->CurAc1TxIndex,pDevExt->NextAc1TxDoneIndex));	

		Testdbg("\n");
		Testdbg("PciDumpr:  CurAc1TxIndex= %d, NextAc1TxDoneIndex= %d\n", pDevExt->CurAc1TxIndex,pDevExt->NextAc1TxDoneIndex);	


		pTxD = (PTXD_STRUC)	pDevExt->Ac1RingMain[pDevExt->NextAc1TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
		{
//			Testdbg("pTxD->DDONE != 1\n");			
//			Testdbg("pDevExt->Ac1_MAX_TX_PROCESS = %d\n", pDevExt->Ac1_MAX_TX_PROCESS);
//			Testdbg("pDevExt->CurAc1TxIndex = %d\n", pDevExt->CurAc1TxIndex);
//			Testdbg("pDevExt->NextAc1TxDoneIndex = %d\n", pDevExt->NextAc1TxDoneIndex);
			break;
		}

//		Testdbg("pDevExt->CurAc1TxIndex = %d\n", pDevExt->CurAc1TxIndex);
//		Testdbg("pDevExt->NextAc1TxDoneIndex = %d\n", pDevExt->NextAc1TxDoneIndex);
		
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		Testdbg("TXT:tx descriptor DDONE ok\n");
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac1TxedCount++;				
		DBGPRINT(RT_DEBUG_TRACE,("Count = %d  ,  pDevExt->Ac1_MAX_TX_PROCESS = %d\n", Count, pDevExt->Ac1_MAX_TX_PROCESS));
		Testdbg ("Count = %d  ,  pDevExt->Ac1_MAX_TX_PROCESS = %d\n", Count, pDevExt->Ac1_MAX_TX_PROCESS);
		RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX1,&TX_CTX_IDX1);
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  RA_TX_CTX_IDX1 = %d\n", TX_CTX_IDX1));
		Testdbg ("PciDumpr:  RA_TX_CTX_IDX1 = %d\n", TX_CTX_IDX1);
		RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX1,&TX_DTX_IDX1);
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  RA_TX_DTX_IDX1 = %d\n", TX_DTX_IDX1));
		Testdbg ("PciDumpr:  RA_TX_DTX_IDX1 = %d\n", TX_DTX_IDX1);

		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
			Testdbg("TXT: transmit next packet\n");
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)
				pDevExt->OtherCounters.TxRepeatCount--;
  
			if (TX_DTX_IDX1 > pDevExt->CurAc1TxIndex) 
				FREE = TX_DTX_IDX1 -pDevExt->CurAc1TxIndex -1;
			else
				FREE = TX_DTX_IDX1 + AC1_RING_SIZE -pDevExt->CurAc1TxIndex -1;

			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextAc1TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac1RingMain[j].DmaBuf.AllocVa, 0);
					}
					else
					{
						int j=pDevExt->NextAc1TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac1RingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}
			
			SEQdbg ("FREE = %d\n", FREE);
			if (  FREE >=1 )
			{					       
                            
				pTxD = (PTXD_STRUC)	pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pAc1RingTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
					//pAc1RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc1RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc1RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				if(pDevExt->FrameType == 17)		// Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc1RingTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
					//pAc1RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc1RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc1RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	
					
					pTxD->SDL0 = pDevExt->CurLength;	

					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}

					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc1RingTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
					//pAc1RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc1RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					Testdbg("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0);
					Testdbg("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length);
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc1RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;
                            
				pDevExt->CurAc1TxIndex++;
				if(pDevExt->CurAc1TxIndex >= AC1_RING_SIZE)				
					pDevExt->CurAc1TxIndex = 0;
				DBGPRINT(RT_DEBUG_TRACE,("CurAc1TxIndex++ = %d\n", pDevExt->CurAc1TxIndex));
				Testdbg ("CurAc1TxIndex++ = %d\n", pDevExt->CurAc1TxIndex);

			}
			
			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX1,	pDevExt->CurAc1TxIndex  );			
			
		}

		pDevExt->NextAc1TxDoneIndex++;
		if (pDevExt->NextAc1TxDoneIndex >= AC1_RING_SIZE)
			pDevExt->NextAc1TxDoneIndex = 0;
		DBGPRINT(RT_DEBUG_TRACE,("NextAc1TxDoneIndex++ = %d\n", pDevExt->NextAc1TxDoneIndex));
		Testdbg ("NextAc1TxDoneIndex++ = %d\n", pDevExt->NextAc1TxDoneIndex);

	}while(++Count < pDevExt->Ac1_MAX_TX_PROCESS);
}

VOID
RTMPHandleAc2TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc2RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX2, TX_DTX_IDX2, FREE, Tmp_Length;
	PULONG				pulTemp;
	
	DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc2TxRingTxDoneInterrupt()\n"));
	do
	{
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc2TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc2TxIndex= %d, NextAc2TxDoneIndex= %d\n", pDevExt->CurAc2TxIndex,pDevExt->NextAc2TxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->Ac2RingMain[pDevExt->NextAc2TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac2TxedCount++;	
		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                    
		       if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
			       if (pDevExt->SetTXWI_NSEQ == 0)
                    	       {
                                   if(pDevExt->FrameType == 21)
                                   {
                                        int j=pDevExt->NextAc2TxDoneIndex;
                                        IncSequenceNumber((UCHAR*)pDevExt->Ac2RingMain[j].DmaBuf.AllocVa, 2);
                                   }
		                     else
		                     {
		                          int j=pDevExt->NextAc2TxDoneIndex;
                                        IncSequenceNumber((UCHAR*)pDevExt->Ac2RingMain[j].DmaBuf.AllocVa, 10);
		                      }
                            }
			}
 

			RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX2,&TX_CTX_IDX2);
			RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX2,&TX_DTX_IDX2);
			if (TX_DTX_IDX2 > pDevExt->CurAc2TxIndex) 
				FREE = TX_DTX_IDX2 -pDevExt->CurAc2TxIndex -1;
			else
				FREE = TX_DTX_IDX2+ AC2_RING_SIZE -pDevExt->CurAc2TxIndex -1;
			
			if (  FREE >=1)
			{

				pTxD = (PTXD_STRUC)	pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].AllocVa;
				
				if((pDevExt->FrameType == 17) || (pDevExt->RingType&0x01000000))		// Random length
				{
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc2RingTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].DmaBuf.AllocVa;
					//pAc2RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc2RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc2RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)
						pDevExt->CurLength = pDevExt->MinLength;					
					
					pTxD->SDL0 = pDevExt->CurLength;				
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc2RingTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].DmaBuf.AllocVa;
					//pAc2RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc2RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc2RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;

				pDevExt->CurAc2TxIndex++;
				if(pDevExt->CurAc2TxIndex >= AC2_RING_SIZE)				
					pDevExt->CurAc2TxIndex = 0;
				
			}
			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX2,	pDevExt->CurAc2TxIndex  );
			
		}

		pDevExt->NextAc2TxDoneIndex++;
		if (pDevExt->NextAc2TxDoneIndex >= AC2_RING_SIZE)
			pDevExt->NextAc2TxDoneIndex = 0;

	}while(++Count < pDevExt->Ac2_MAX_TX_PROCESS);
}

VOID
RTMPHandleAc3TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc3RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX3, TX_DTX_IDX3, FREE, Tmp_Length;
	PULONG				pulTemp;
	
	DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc3TxRingTxDoneInterrupt()\n"));
	do
	{
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc3TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc3TxIndex= %d, NextAc3TxDoneIndex= %d\n", pDevExt->CurAc3TxIndex,pDevExt->NextAc3TxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->Ac3RingMain[pDevExt->NextAc3TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac3TxedCount++;	
		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                     
			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
			       if (pDevExt->SetTXWI_NSEQ == 0)
                    	       {
                                   if(pDevExt->FrameType == 21)
                                   {
                                        int j=pDevExt->NextAc3TxDoneIndex;
                                        IncSequenceNumber((UCHAR*)pDevExt->Ac3RingMain[j].DmaBuf.AllocVa, 3);
                                   }
		                     else
		                     {
		                          int j=pDevExt->NextAc3TxDoneIndex;
                                        IncSequenceNumber((UCHAR*)pDevExt->Ac3RingMain[j].DmaBuf.AllocVa, 10);
		                      }
                            }
			}


			RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX3,&TX_CTX_IDX3);
			RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX3,&TX_DTX_IDX3);
			if (TX_DTX_IDX3 > pDevExt->CurAc3TxIndex) 
				FREE = TX_DTX_IDX3 -pDevExt->CurAc3TxIndex -1;
			else
				FREE = TX_DTX_IDX3+ AC3_RING_SIZE -pDevExt->CurAc3TxIndex -1;
			
			if (  FREE >=1)
			{

				pTxD = (PTXD_STRUC)	pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].AllocVa;

				if((pDevExt->FrameType == 17) || (pDevExt->RingType&0x01000000))		// Random length
				{
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc3RingTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].DmaBuf.AllocVa;
					//pAc3RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc3RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc3RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;					

					pTxD->SDL0 = pDevExt->CurLength;				
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc3RingTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].DmaBuf.AllocVa;
					//pAc3RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc3RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc3RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;

				pDevExt->CurAc3TxIndex++;
				if(pDevExt->CurAc3TxIndex >= AC3_RING_SIZE)				
					pDevExt->CurAc3TxIndex = 0;
				
			}
			
			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX3,	pDevExt->CurAc3TxIndex  );
			
		}

		pDevExt->NextAc3TxDoneIndex++;
		if (pDevExt->NextAc3TxDoneIndex >= AC3_RING_SIZE)
			pDevExt->NextAc3TxDoneIndex = 0;

	}while(++Count < pDevExt->Ac3_MAX_TX_PROCESS);
}

VOID
RTMPHandleAc4TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc4RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX4, TX_DTX_IDX4, FREE, Tmp_Length;
	PULONG				pulTemp;

	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc4TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc4TxIndex= %d, NextAc4TxDoneIndex= %d\n", pDevExt->CurAc4TxIndex,pDevExt->NextAc4TxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->Ac4RingMain[pDevExt->NextAc4TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		DBGPRINT(RT_DEBUG_TRACE,("pDevExt->OtherCounters.TransmittedCount = %d\n", pDevExt->OtherCounters.TransmittedCount));
		DBGPRINT(RT_DEBUG_TRACE,("pDevExt->OtherCounters.TxRepeatCount = %d\n", pDevExt->OtherCounters.TxRepeatCount));
		
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac4TxedCount++;
		RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX4,&TX_CTX_IDX4);
		RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX4,&TX_DTX_IDX4);

		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                                         
			if (TX_DTX_IDX4 > pDevExt->CurAc4TxIndex) 
				FREE = TX_DTX_IDX4 -pDevExt->CurAc4TxIndex -1;
			else
				FREE = TX_DTX_IDX4 + AC4_RING_SIZE -pDevExt->CurAc4TxIndex -1;

			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextAc4TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac4RingMain[j].DmaBuf.AllocVa, 4);
					}
					else
					{
						int j=pDevExt->NextAc4TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac4RingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}

			SEQdbg ("FREE = %d\n", FREE);
			if ( FREE >= 1 )
			{
				pTxD = (PTXD_STRUC)	pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pAc4RingTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
					//pAc4RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc4RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc4RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				if(pDevExt->FrameType == 17)		// Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc4RingTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
					//pAc4RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc4RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc4RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	

					pTxD->SDL0 = pDevExt->CurLength;	
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc4RingTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
					//pAc4RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc4RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc4RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}

				pTxD->DDONE = 0;

				pDevExt->CurAc4TxIndex++;
				if(pDevExt->CurAc4TxIndex >= AC4_RING_SIZE)
					pDevExt->CurAc4TxIndex = 0;
			}

			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX4,	pDevExt->CurAc4TxIndex);
		}

		pDevExt->NextAc4TxDoneIndex++;
		if (pDevExt->NextAc4TxDoneIndex >= AC4_RING_SIZE)
			pDevExt->NextAc4TxDoneIndex = 0;

	}while(++Count < pDevExt->Ac4_MAX_TX_PROCESS);
}


VOID
RTMPHandleAc5TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc5RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX5, TX_DTX_IDX5, FREE, Tmp_Length;
	PULONG				pulTemp;

	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc5TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc5TxIndex= %d, NextAc5TxDoneIndex= %d\n", pDevExt->CurAc5TxIndex,pDevExt->NextAc5TxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->Ac5RingMain[pDevExt->NextAc5TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac5TxedCount++;
		RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX5,&TX_CTX_IDX5);
		RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX5,&TX_DTX_IDX5);

		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                                         
			if (TX_DTX_IDX5 > pDevExt->CurAc5TxIndex) 
				FREE = TX_DTX_IDX5 -pDevExt->CurAc5TxIndex -1;
			else
				FREE = TX_DTX_IDX5 + AC5_RING_SIZE -pDevExt->CurAc5TxIndex -1;

			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextAc5TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac5RingMain[j].DmaBuf.AllocVa, 5);
					}
					else
					{
						int j=pDevExt->NextAc5TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac5RingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}

			SEQdbg ("FREE = %d\n", FREE);
			if ( FREE >= 1 )
			{
				pTxD = (PTXD_STRUC)	pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pAc5RingTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
					//pAc5RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc5RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc5RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				if(pDevExt->FrameType == 17)		// Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc5RingTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
					//pAc5RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc5RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc5RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	

					pTxD->SDL0 = pDevExt->CurLength;	
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc5RingTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
					//pAc5RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc5RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc5RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;

				pDevExt->CurAc5TxIndex++;
				if(pDevExt->CurAc5TxIndex >= AC5_RING_SIZE)				
					pDevExt->CurAc5TxIndex = 0;
			}
			
			RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX5,	pDevExt->CurAc5TxIndex  );			
			
		}

		pDevExt->NextAc5TxDoneIndex++;
		if (pDevExt->NextAc5TxDoneIndex >= AC5_RING_SIZE)
			pDevExt->NextAc5TxDoneIndex = 0;

	}while(++Count < pDevExt->Ac5_MAX_TX_PROCESS);
}

VOID
RTMPHandleAc6TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc6RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX6, TX_DTX_IDX6, FREE, Tmp_Length;
	PULONG				pulTemp;

	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc6TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc6TxIndex= %d, NextAc6TxDoneIndex= %d\n", pDevExt->CurAc6TxIndex,pDevExt->NextAc6TxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->Ac6RingMain[pDevExt->NextAc6TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac6TxedCount++;
		RTMP_IO_READ32(pDevExt, RA_TX_CTX_IDX6,&TX_CTX_IDX6);
		RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX6,&TX_DTX_IDX6);

		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                                         
			if (TX_DTX_IDX6 > pDevExt->CurAc6TxIndex) 
				FREE = TX_DTX_IDX6 -pDevExt->CurAc6TxIndex -1;
			else
				FREE = TX_DTX_IDX6 + AC6_RING_SIZE -pDevExt->CurAc6TxIndex -1;

			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextAc6TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac6RingMain[j].DmaBuf.AllocVa, 6);
					}
					else
					{
						int j=pDevExt->NextAc6TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac6RingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}

			SEQdbg ("FREE = %d\n", FREE);
			if ( FREE >= 1 )
			{
				pTxD = (PTXD_STRUC)	pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pAc6RingTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
					//pAc6RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc6RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc6RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				if(pDevExt->FrameType == 17)		// Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc6RingTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
					//pAc6RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc6RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc6RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	

					pTxD->SDL0 = pDevExt->CurLength;	
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc6RingTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
					//pAc6RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc6RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc6RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;

				pDevExt->CurAc6TxIndex++;
				if(pDevExt->CurAc6TxIndex >= AC6_RING_SIZE)				
					pDevExt->CurAc6TxIndex = 0;
			}
			
			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX6,	pDevExt->CurAc6TxIndex  );			
			
		}

		pDevExt->NextAc6TxDoneIndex++;
		if (pDevExt->NextAc6TxDoneIndex >= AC6_RING_SIZE)
			pDevExt->NextAc6TxDoneIndex = 0;

	}while(++Count < pDevExt->Ac6_MAX_TX_PROCESS);
}

VOID
RTMPHandleAc7TxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pAc7RingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				TX_CTX_IDX7, TX_DTX_IDX7, FREE, Tmp_Length;
	PULONG				pulTemp;

	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleAc7TxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurAc7TxIndex= %d, NextAc7TxDoneIndex= %d\n", pDevExt->CurAc7TxIndex,pDevExt->NextAc7TxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->Ac7RingMain[pDevExt->NextAc7TxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.Ac7TxedCount++;
		RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX7,&TX_CTX_IDX7);
		RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX7,&TX_DTX_IDX7);

		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                                         
			if (TX_DTX_IDX7 > pDevExt->CurAc7TxIndex) 
				FREE = TX_DTX_IDX7 -pDevExt->CurAc7TxIndex -1;
			else
				FREE = TX_DTX_IDX7 + AC7_RING_SIZE -pDevExt->CurAc7TxIndex -1;

			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextAc7TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[j].DmaBuf.AllocVa, 7);
					}
					else
					{
						int j=pDevExt->NextAc7TxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}

			SEQdbg ("FREE = %d\n", FREE);
			if ( FREE >= 1 )
			{
				pTxD = (PTXD_STRUC)	pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pAc7RingTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
					//pAc7RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc7RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc7RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				if(pDevExt->FrameType == 17)		// Random length
				{
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc7RingTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
					//pAc7RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc7RingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc7RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	

					pTxD->SDL0 = pDevExt->CurLength;	
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pAc7RingTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
					//pAc7RingTxWI->MPDU_SIZE = Tmp_Length;
					pAc7RingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pAc7RingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;

				pDevExt->CurAc7TxIndex++;
				if(pDevExt->CurAc7TxIndex >= AC7_RING_SIZE)				
					pDevExt->CurAc7TxIndex = 0;
			}
			
			RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX7,	pDevExt->CurAc7TxIndex  );			
			
		}

		pDevExt->NextAc7TxDoneIndex++;
		if (pDevExt->NextAc7TxDoneIndex >= AC7_RING_SIZE)
			pDevExt->NextAc7TxDoneIndex = 0;

	}while(++Count < pDevExt->Ac7_MAX_TX_PROCESS);
}

VOID
RTMPHandleMgmtTxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pMgmtRingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				MGN_CTX_IDX, MGN_DTX_IDX, FREE, Tmp_Length;
	PULONG				pulTemp;

	do
	{  	       	      
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleMgmtTxRingTxDoneInterrupt()\n"));
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurMgmtTxIndex= %d, NextMgmtTxDoneIndex= %d\n", pDevExt->CurMgmtTxIndex, pDevExt->NextMgmtTxDoneIndex));	

		pTxD = (PTXD_STRUC)	pDevExt->MgmtRingMain[pDevExt->NextMgmtTxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
		{
			break;
		}
		
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		Testdbg("TXT:tx descriptor DDONE ok\n");
//		pDevExt->OtherCounters.TransmittedCount++;
		
//		pDevExt->OtherCounters.MgmtTxedCount++;

		RTMP_IO_READ32(pDevExt, RA_TX_CTX_IDX8,&MGN_CTX_IDX);
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  RA_TX_CTX_IDX8 = %d\n", MGN_CTX_IDX));
		RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX8,&MGN_DTX_IDX);
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  RA_TX_DTX_IDX8 = %d\n", MGN_DTX_IDX));
		
		//if(pDevExt->OtherCounters.TxRepeatCount > 0)
		if(pDevExt->FirmwarePktCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
			Testdbg("TXT: transmit next packet\n");
  			//if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)
			//	pDevExt->OtherCounters.TxRepeatCount--;
			if(pDevExt->FirmwarePktCount != 0xFFFFFFFF)
				pDevExt->FirmwarePktCount--;
  
			if (MGN_DTX_IDX > pDevExt->CurMgmtTxIndex) 
				FREE = MGN_DTX_IDX -pDevExt->CurMgmtTxIndex -1;
			else
				FREE = MGN_DTX_IDX + MGMT_RING_SIZE -pDevExt->CurMgmtTxIndex -1;

			//if (pDevExt->OtherCounters.TxRepeatCount > 0)
			if(pDevExt->FirmwarePktCount > 0)
			{			      
				if (pDevExt->SetTXWI_NSEQ == 0)
				{
					if(pDevExt->FrameType == 21)
					{
						int j=pDevExt->NextMgmtTxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->MgmtRingMain[j].DmaBuf.AllocVa, 8);
					}
					else
					{
						int j=pDevExt->NextMgmtTxDoneIndex;
						IncSequenceNumber((UCHAR*)pDevExt->MgmtRingMain[j].DmaBuf.AllocVa, 10);
					}
				}
			}
			
			SEQdbg ("FREE = %d\n", FREE);
			if ( FREE >= 1 )
			{
				pTxD = (PTXD_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].AllocVa;

				if (pDevExt->RingType&0x01000000) // QoS Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE - 2;
					DBGPRINT(RT_DEBUG_TRACE,("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0));
					Testdbg("pTxD->SDL0->SDL0 : %d===========\n", pTxD->SDL0);
					pMgmtRingTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
					//pMgmtRingTxWI->MPDU_SIZE = Tmp_Length;
					pMgmtRingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pMgmtRingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}

				if(pDevExt->FrameType == 17)		// Random length
				{				       
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pMgmtRingTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
					//pMgmtRingTxWI->MPDU_SIZE = Tmp_Length;
					pMgmtRingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pMgmtRingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)	
						pDevExt->CurLength = pDevExt->MinLength;	
					
					pTxD->SDL0 = pDevExt->CurLength;	

					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}

					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pMgmtRingTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
					//pMgmtRingTxWI->MPDU_SIZE = Tmp_Length;
					pMgmtRingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					Testdbg("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0);
					Testdbg("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length);
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pMgmtRingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;
                            
				pDevExt->CurMgmtTxIndex++;
				if(pDevExt->CurMgmtTxIndex >= MGMT_RING_SIZE)				
					pDevExt->CurMgmtTxIndex = 0;
				DBGPRINT(RT_DEBUG_TRACE,("CurMgmtTxIndex++ = %d\n", pDevExt->CurMgmtTxIndex));
				Testdbg ("CurMgmtTxIndex++ = %d\n", pDevExt->CurMgmtTxIndex);

			}
			
			RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX8,	 pDevExt->CurMgmtTxIndex  );			
			
		}

		pDevExt->NextMgmtTxDoneIndex++;
		if (pDevExt->NextMgmtTxDoneIndex >= MGMT_RING_SIZE)
			pDevExt->NextMgmtTxDoneIndex = 0;
		DBGPRINT(RT_DEBUG_TRACE,("NextMgmtTxDoneIndex++ = %d\n", pDevExt->NextMgmtTxDoneIndex));
		Testdbg ("NextMgmtTxDoneIndex++ = %d\n", pDevExt->NextMgmtTxDoneIndex);

	}while(++Count < pDevExt->Mgmt_MAX_TX_PROCESS);
}

VOID
RTMPHandleHccaTxRingTxDoneInterrupt(
	IN PVOID pContext )
{
	PTXD_STRUC			pTxD;
	PTX_WI_STRUC		pHccaRingTxWI;
	UCHAR				Count = 0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;
	UCHAR				i;
	ULONG				HCCA_CTX_IDX, HCCA_DTX_IDX, FREE, Tmp_Length;
	PULONG				pulTemp;
	
	DBGPRINT(RT_DEBUG_TRACE,("TXT: RTMPHandleHccaTxRingTxDoneInterrupt()\n"));
	do
	{
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  CurTxIndex= %d, NextHccaTxDoneIndex= %d\n", pDevExt->CurHccaTxIndex,pDevExt->NextHccaTxDoneIndex));	
		pTxD = (PTXD_STRUC)	pDevExt->HccaRingMain[pDevExt->NextHccaTxDoneIndex].AllocVa;
		if ((pTxD->DDONE != DESC_DMA_DONE))
			break;
		pTxD->DDONE = 0;
		DBGPRINT(RT_DEBUG_TRACE,("TXT:tx descriptor DDONE ok\n"));
		pDevExt->OtherCounters.TransmittedCount++;
		
		pDevExt->OtherCounters.HccaTxedCount++;	
		if(pDevExt->OtherCounters.TxRepeatCount > 0)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TXT: transmit next packet\n"));
  			if(pDevExt->OtherCounters.TxRepeatCount != 0xFFFFFFFF)				
				pDevExt->OtherCounters.TxRepeatCount--;
                     
			if (pDevExt->OtherCounters.TxRepeatCount > 0)
			{			      
			       if (pDevExt->SetTXWI_NSEQ == 0)
                    	       {
                                   if(pDevExt->FrameType == 21)
                                   {
                                        int j=pDevExt->NextHccaTxDoneIndex;
                                        IncSequenceNumber((UCHAR*)pDevExt->HccaRingMain[j].DmaBuf.AllocVa, 9);
                                   }
		                     else
		                     {
		                          int j=pDevExt->NextHccaTxDoneIndex;
                                        IncSequenceNumber((UCHAR*)pDevExt->HccaRingMain[j].DmaBuf.AllocVa, 10);
		                      }
                            }
			}


			RTMP_IO_READ32(pDevExt,RA_TX_CTX_IDX9,&HCCA_CTX_IDX);
			RTMP_IO_READ32(pDevExt,RA_TX_DTX_IDX9,&HCCA_DTX_IDX);
			if (HCCA_DTX_IDX> pDevExt->CurHccaTxIndex) 
				FREE = HCCA_DTX_IDX -pDevExt->CurHccaTxIndex -1;
			else
				FREE = HCCA_DTX_IDX+ HCCA_RING_SIZE -pDevExt->CurHccaTxIndex -1;
			
			if (  FREE >=1)
			{

				pTxD = (PTXD_STRUC)	pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].AllocVa;
				
				if((pDevExt->FrameType == 17) || (pDevExt->RingType&0x01000000))		// Random length
				{
					pTxD->SDL0 = (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
					if(pTxD->SDL0< 50)	
					{
						pTxD->SDL0 += 50;
					}
					pDevExt->CurLength = pTxD->SDL0;
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pHccaRingTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
					//pHccaRingTxWI->MPDU_SIZE = Tmp_Length;
					pHccaRingTxWI->TXByteCount = Tmp_Length;
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pHccaRingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				else if(pDevExt->FrameType == 18)		// Inc length
				{
					pDevExt->CurLength++;
					if(pDevExt->CurLength > pDevExt->MaxLength)
						pDevExt->CurLength = pDevExt->MinLength;					
					pTxD->SDL0 = pDevExt->CurLength;		
					
					if(pTxD->SDL0 < 44)
					{
						pTxD->SDL0 = 44;
					}
					
					Tmp_Length =  pTxD->SDL0 - TXWI_SIZE;
					pHccaRingTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
					//pHccaRingTxWI->MPDU_SIZE = Tmp_Length;
					pHccaRingTxWI->TXByteCount = Tmp_Length;
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len: SDL0 = %x\n",pTxD->SDL0));
					DBGPRINT(RT_DEBUG_TRACE,("Isr-->Inc_len:WI MSDU = %x\n",Tmp_Length));
					if ( pDevExt->SetTcpIpPayload )
					{
						pulTemp = (PULONG)pHccaRingTxWI;
						pulTemp += (TXWI_SIZE + 8);
						*pulTemp = (*pulTemp & 0xFFFF0000);
						*pulTemp = *pulTemp | (( Tmp_Length & 0x0000FFFF ) - 32); // Total length in IP header is mpdu_total_length - wifi_header_length(24 bytes) - llc_length(8bytes).
					}
				}
				
				pTxD->DDONE = 0;

				pDevExt->CurHccaTxIndex++;
				if(pDevExt->CurHccaTxIndex >= HCCA_RING_SIZE)				
					pDevExt->CurHccaTxIndex = 0;
				
			}
			
			RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX9,	pDevExt->CurHccaTxIndex  );
			
		}

		pDevExt->NextHccaTxDoneIndex++;
		if (pDevExt->NextHccaTxDoneIndex >= HCCA_RING_SIZE)
			pDevExt->NextHccaTxDoneIndex = 0;

	}while(++Count < pDevExt->Hcca_MAX_TX_PROCESS);
}


VOID LfsrInit(
    IN PDEVICE_OBJECT	pDevObj, 
    IN ULONG Seed) 
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDevObj->DeviceExtension;

    if (Seed == 0) 
        pDevExt->ShiftReg = 1;
    else 
        pDevExt->ShiftReg = Seed;
}
#endif /* RTMP_PCI_SUPPORT */


UCHAR RandomByte(RTMP_ADAPTER *pDevExt) 
{
    ULONG i;
    UCHAR R, Result;

    R = 0;

    for (i = 0; i < 8; i++) 
    {
        if (pDevExt->ShiftReg & 0x00000001) 
        {
            pDevExt->ShiftReg = ((pDevExt->ShiftReg ^ LFSR_MASK) >> 1) | 0x80000000;
            Result = 1;
        } 
        else 
        {
            pDevExt->ShiftReg = pDevExt->ShiftReg >> 1;
            Result = 0;
        }
        R = (R << 1) | Result;
    }

    return R;
}


USHORT RandomShort(RTMP_ADAPTER *pDevExt) 
{
    ULONG i;
    USHORT R, Result;

    R = 0;

    for (i = 0; i < 16; i++) 
    {
        if (pDevExt->ShiftReg & 0x00000001) 
        {
            pDevExt->ShiftReg = ((pDevExt->ShiftReg ^ LFSR_MASK) >> 1) | 0x80000000;
            Result = 1;
        } 
        else 
        {
            pDevExt->ShiftReg = pDevExt->ShiftReg >> 1;
            Result = 0;
        }
        R = (R << 1) | Result;
    }

    return R;
}


#ifdef RTMP_PCI_SUPPORT
VOID
RTPCIHandleDecryptionDoneInterrupt(
			IN PVOID pContext )
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;

}

VOID
RTPCIHandleEncryptionDoneInterrupt(
			IN PVOID pContext )
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;

}

#if 1
VOID RTMPPatchCardBus(RTMP_ADAPTER *pAdapter)
{
	USHORT	Bus;
	USHORT	Slot;
	USHORT	Func;
	ULONG	Configuration;
	ULONG	Vendor;
	ULONG	Device;
	ULONG	Class;
	ULONG	Interrupt;
	ULONG	PciLantency;
	ULONG	CacheLine;
	ULONG	CardbusLantency;
	ULONG	SubBus;
	
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTMPPatchCardBus!\n"));

	// Try to find the Cardbus controller we connected to.
	// We should not touch other Cardbus controller which we did not use
	// The cardbus controller should have bus number smaller than our device
	// We should skip Bus 0 & 1, since it is for PCI & VGA chipset.
	// Cardbus controller should start from bus = 2.
	for (Bus = 0; Bus < MAX_PCI_BUS; Bus++)
	{
		for (Slot = 0; Slot < MAX_PCI_DEVICE; Slot++)
		{
			for (Func = 0; Func < MAX_FUNC_NUM; Func++)
			{
				Vendor    = RTMPReadCBConfig(Bus, Slot, Func, 0x00) & 0x0000FFFF;
				// Skip non-exist deice right away
				if (Vendor == UNKNOWN)
					continue;
				
				Device    = RTMPReadCBConfig(Bus, Slot, Func, 0x00) >> 16;
				Interrupt = RTMPReadCBConfig(Bus, Slot, Func, 0x3c) & 0x000000FF;
				Class     = RTMPReadCBConfig(Bus, Slot, Func, 0x08) >> 16;
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Vendor=%x, Device=%x, Interrupt=%x, Class=%x!\n",Vendor,Device,Interrupt,Class));
				
				// First of all, filter non-cardbus bridge and not matching INT line device
				// Ralink ASIC has to be modified for latency timer and cache line size too.
//				if (Interrupt != pAdapter->InterruptLevel)
//					continue;

				// Check the class for cardbus bridge, otherwise check RALINK device	
				if (Class != CARD_BRIDGE_CLASS)
				{
					if (Vendor != RALINK)
						continue;
					else
					{
						// Found Ralink device before cardbus bridge indicate this is not in cardbus system
						// We should stop scanning and patch right away
						DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: This indicate this is not cardbus device!\n"));
						continue;
					}
				}
				
				// Second, Modify Latency timer and cache line size to 0x40, 0x08 respectly
				Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x0c);
				CacheLine   = Configuration & 0x000000ff;
				PciLantency = (Configuration & 0x0000ff00) >> 8;
				if (CacheLine == 0)
					Configuration = (Configuration & 0xffffff00) | 0x08;
				if (PciLantency == 0)
					Configuration = (Configuration & 0xffff00ff) | 0xa800;
					
				RTMPWriteCBConfig(Bus, Slot, Func, 0x0c, Configuration);
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Configuration 0x0c is %x!\n",Configuration));

				// Read Subordinate bus number for our device, it must be there
				Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x18);
				SubBus = (Configuration & 0x00ff0000) >> 16;
				
				// Second check for prefecth bit modification requirement
				switch (Vendor)
				{					
					case	RICOH:
						// Do prefectch configuration
						// Config register
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x80);
						Configuration |= 0x01;
						RTMPWriteCBConfig(Bus, Slot, Func, 0x80, Configuration);						
						DBGPRINT(RT_DEBUG_TRACE,("RICOH 0x80 is %x\n", Configuration));
						break;
					case	O2MICRO:						
						// Do prefectch configuration
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x94);
						// Do prefectch configuration						
						Configuration |= 0x02;
						RTMPWriteCBConfig(Bus, Slot, Func, 0x94, Configuration);					
						DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: O2MICRO 6933 0x94  is  %x!\n",Configuration));
						// Do unknown 6912 patch
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0xd4);
						Configuration |= 0x02;
						RTMPWriteCBConfig(Bus, Slot, Func, 0xd4, Configuration);					
						DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: O2MICRO 6912 0xD4  is  %x!\n",Configuration));
						break;
						
					case	TI:
						// Do prefectch configuration
						// Config register
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x18);
						CardbusLantency = (Configuration & 0xff000000) >> 24;
						if (CardbusLantency == 0)
						{
							Configuration = (Configuration & 0x00ffffffff) | 0xb0000000;
							RTMPWriteCBConfig(Bus, Slot, Func, 0x18, Configuration);						
							DBGPRINT(RT_DEBUG_TRACE,("TI 0x18 is %x\n", Configuration));
						}
						break;
						
					case	ENE:
						// Do patch for ENE cardbus issue found on ASUS notebook
						// Change offset address 0xCB to 0x1f
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0xC8);
						Configuration = (Configuration & 0x00ffffff) | 0x1f000000;
						RTMPWriteCBConfig(Bus, Slot, Func, 0xC8, Configuration);					
						DBGPRINT(RT_DEBUG_TRACE,("ENE 0xc8 is %x\n", Configuration));
						break;
					default:
						break;						
				}
				// Go to config ralink device after setup cardbus controller
				RTMPPatchRalinkCardBus(pAdapter, SubBus);
			}
		}
	}
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr:  <---RTMPPatchCardBus\n"));
}

// IRQL = PASSIVE_LEVEL
VOID	RTMPPatchRalinkCardBus(
	IN	RTMP_ADAPTER *pAdapter,
	IN	ULONG			Bus)
{
	USHORT	Slot;
	ULONG	Configuration;
	ULONG	Vendor;
	ULONG	PciLantency;
	ULONG	CacheLine;
	
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTMPPatchRalinkCardBus!\n"));
	for (Slot = 0; Slot < MAX_PCI_DEVICE; Slot++)
	{
		// There is only one function for Ralink device which is 0
		Vendor	  = RTMPReadCBConfig(Bus, Slot, 0, 0x00) & 0x0000FFFF;
		if (Vendor != RALINK)
			continue;
		// Second, Modify Latency timer and cache line size to 0x40, 0x08 respectly
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTMPPatchRalinkCardBus, Vendor %x\n",Vendor));
		Configuration = RTMPReadCBConfig(Bus, Slot, 0, 0x0c);
		CacheLine	= Configuration & 0x000000ff;
		PciLantency = (Configuration & 0x0000ff00) >> 8;
		if (CacheLine == 0)
			Configuration = (Configuration & 0xffffff00) | 0x08;
		if (PciLantency == 0)
			Configuration = (Configuration & 0xffff00ff) | 0xa800;
				
		RTMPWriteCBConfig(Bus, Slot, 0, 0x0c, Configuration);
	}
}

//
// This routine will read a long (32 bits) from configuration registers.
//
// IRQL = PASSIVE_LEVEL
ULONG	RTMPReadCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset)
{
	ULONG	Value;
	ULONG	ConfigAddr;

	ConfigAddr = (Bus << 16) | (Slot << 11) | (Func << 8) | Offset | 0x80000000L;
	WRITE_PORT_ULONG((PULONG)PCI_CFG_ADDR_PORT, ConfigAddr);
	KeStallExecutionProcessor(10);
	Value = READ_PORT_ULONG((PULONG)PCI_CFG_DATA_PORT);

	return(Value);
}

//
// This routine will write a long (32 bits) from configuration registers.
//
// IRQL = PASSIVE_LEVEL
VOID	RTMPWriteCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset,
	IN	ULONG	Value)
{
	ULONG	ConfigAddr;

	ConfigAddr = (Bus << 16) | (Slot << 11) | (Func << 8) | Offset | 0x80000000L;
	
	WRITE_PORT_ULONG((PULONG)PCI_CFG_ADDR_PORT, (ULONG)ConfigAddr);
	KeStallExecutionProcessor(10);
	WRITE_PORT_ULONG((PULONG)PCI_CFG_DATA_PORT, Value);
}
	
#endif
#endif /* RTMP_PCI_SUPPORT */

/*
	========================================================================
	
	Routine Description:
		Be called in HandleDeviceStart to init some settings

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS		Succeed
		NDIS_STATUS_FAILURE			Failed

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/

//USB
NTSTATUS NICInit(
	IN RTMP_ADAPTER *pAdapter)
{
	NTSTATUS 				ntStatus = STATUS_SUCCESS;
	NTSTATUS 				CRStatus = STATUS_SUCCESS;
	U3DMA_WLCFG         		U3DMAWLCFG;
	ULONG BulkInNum = 0;

	//To get Chip ID
	CRStatus = USBHwHal_ReadMacRegister(pAdapter, 0x80000008, &pAdapter->MACVersion);
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Read Mac Register CR_BASE_HIF failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s MacValue = 0x%x\n", __FUNCTION__, pAdapter->MACVersion));
	}

	//deviceExtension->MACVersion = 0x76360000;

	if(IS_MT7603(pAdapter))
	{
		DBGPRINT(RT_DEBUG_TRACE,("####################################  MT7603  ############################################\n"));
	}
	else if(IS_MT7636(pAdapter))
	{
		DBGPRINT(RT_DEBUG_TRACE,("####################################  MT7636  ############################################\n"));
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Not support Nic MacValue = 0x%x\n", __FUNCTION__, pAdapter->MACVersion));
	}
	do
	{
		UCHAR BulkInNum = 0;
		ntStatus = UsbBulkMemoryAlloc(pAdapter);
		if(ntStatus == STATUS_INSUFFICIENT_RESOURCES)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("===================UsbBulkMemoryAlloc   STATUS_INSUFFICIENT_RESOURCES===================\n"));
			break;
		}		
		
		//return STATUS_SUCCESS;

	
//	
//		goto BulkUsb_DispatchCreate_Exit;	
	}while(FALSE);
	// Enable UDMA Tx/Rx	
	CRStatus = USBHwHal_ReadMacRegister(pAdapter, UDMA_WLCFG_0, &U3DMAWLCFG.word);
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Read Mac Register UDMA_WLCFG_0 failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}
	    
	DBGPRINT(RT_DEBUG_TRACE,("1 U3DMAWLCFG = %x\n", U3DMAWLCFG.word));
	U3DMAWLCFG.Default.WL_TX_EN = 1;
	U3DMAWLCFG.Default.WL_RX_EN = 1;
	U3DMAWLCFG.Default.WL_RX_MPSZ_PAD0 = 1;
	DBGPRINT(RT_DEBUG_TRACE,("2 U3DMAWLCFG = %x\n", U3DMAWLCFG.word));	
	CRStatus = USBHwHal_WriteMacRegister(pAdapter, UDMA_WLCFG_0, U3DMAWLCFG.word);
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Write Mac Register UDMA_WLCFG_0 failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}

	#if 1	//load by driver self need to start rx first
		
	for (BulkInNum = 0; BulkInNum<MAX_RX_BULK_PIPE_NUM; BulkInNum++)
	{		
		if(!pAdapter->bRxEnable[BulkInNum])
		{//now rx0 is stop can start.				
			KIRQL Irql;				
			pAdapter->bRxEnable[BulkInNum] = TRUE;

			pAdapter->ContinBulkIn[BulkInNum] = TRUE;				

			KeAcquireSpinLock(&pAdapter->RxSpinLock[BulkInNum], &Irql);
			if(pAdapter->ulBulkInRunning[BulkInNum] > 0)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("=================== ulBulkInRunning[BULKIN_PIPENUMBER_0] > 0===================\n"));
				//KIRQL Irql;
				//KeAcquireSpinLock(&dx->RxSpinLock[0], &Irql);
				pAdapter->ulBulkInRunning[BulkInNum] ++;
				KeReleaseSpinLock(&pAdapter->RxSpinLock[BulkInNum], Irql);
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, ("=================== UsbDoBulkInTransfer===================\n"));
			
				pAdapter->ulBulkInRunning[BulkInNum] ++;
				KeReleaseSpinLock(&pAdapter->RxSpinLock[BulkInNum], Irql);

				RtlZeroMemory(&pAdapter->ReceivedCount[BulkInNum], sizeof(LARGE_INTEGER));
				//Kick bulk in
				UsbDoBulkInTransfer(pAdapter,BulkInNum);
			}

		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("===================IS   pAdapter->bRxEnable[BULKIN_PIPENUMBER_0]===================\n"));
		}
		
	}
#endif	

#if 0
	//driver download FW
	//only 7636 need to download rom patch
	if(IS_MT7636(pAdapter))
	{
		ntStatus = NICLoadRomPatch(pAdapter);
		if (ntStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s NICLoadRomPatch failed\n", __FUNCTION__));
			return STATUS_UNSUCCESSFUL;
		}
	}
	ntStatus = NICLoadFirmware(pAdapter);
	if (ntStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s NICLoadFirmware failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}
#endif
	return ntStatus;
}
/*
	========================================================================
	
	Routine Description:
		Used in load by driver self not by UI

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS         firmware image load ok
		NDIS_STATUS_FAILURE         image not found

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/
NTSTATUS NICLoadRomPatch(
	IN RTMP_ADAPTER *pAdapter)
{
	NTSTATUS 				LoadStatus = STATUS_SUCCESS;
	NTSTATUS 				CRStatus = STATUS_SUCCESS;
	PUCHAR 					pRomPatchImage = NULL;
	ULONG  					RomPatchLength  = 0;
	UINT16					BreakCount = 0;
	SCHEDULER_REGISTER4 	SchedulerRegister4, backupSchedulerRegister4;

	//UDMA Tx/Rx already been set in init
	//step 1  power on Wi-Fi sys
	LoadStatus = ROMPatchPowerOnWiFiSys(pAdapter);
	if(LoadStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s ROMPatchPowerOnWiFiSys failed\n", __FUNCTION__));
		return LoadStatus;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s ROMPatchPowerOnWiFiSys succeed\n", __FUNCTION__));
	}
  
	//step 2 Set DMA scheduler to Bypass mode.
	CRStatus = USBHwHal_ReadMacRegister(pAdapter, SCH_REG4, &SchedulerRegister4.word);
	
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Read Mac Register SCH_REG4 failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}
	    
	DBGPRINT(RT_DEBUG_TRACE,("1 SchedulerRegister4 = %x\n", SchedulerRegister4.word));
	backupSchedulerRegister4.word = SchedulerRegister4.word;
	SchedulerRegister4.Default.BypassMode = 1;
	SchedulerRegister4.Default.ForceQid = 8;
	DBGPRINT(RT_DEBUG_TRACE,("2 SchedulerRegister4 = %x\n", SchedulerRegister4.word));	
	CRStatus = USBHwHal_WriteMacRegister(pAdapter, SCH_REG4, SchedulerRegister4.word);
	
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Write Mac Register SCH_REG4 failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}

	//step 3 Polling 0x81021250 = 0x01, and then can download patch
	do
	{
		ULONG CRValue = 0;
		CRStatus = USBHwHal_ReadMacRegister(pAdapter, SW_SYNC, &CRValue);		
		if (CRValue == 0x1)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Ready to download Rom Patch !!!!!\n"));
			break;
		}
		else if(CRValue == 0x3)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! WiFi is already running no need to load Rom Patch!!!!!\n"));
			return STATUS_SUCCESS;
		}		
		        
		RTMPusecDelay(100000);// 1 ms		        

		BreakCount ++;
		if (BreakCount > 100)
		{
			DBGPRINT(RT_DEBUG_ERROR,("Polling ready to download Rom Patch failed 0x81021250 = 0x%x\n", CRValue));
			break;
		}
		        
	} while(1);	
	//step 4 download ROM patch image check semaphore
	LoadStatus = FirmwareCommnadGetPatchSemaphore(pAdapter);
	if (LoadStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareCommnadGetPatchSemaphore failed. !!!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}
	//step 5 if RomPatchSemStatus != 0x2 => no need to download rom patch
	if(pAdapter->RomPatchSemStatus == 0x2)
	{
		if(pAdapter->IsUISetFW)
		{
			LoadStatus = LoadROMPatchProcess(pAdapter, pAdapter->FWImage, pAdapter->FWSize);
		}
		else
		{
			LoadStatus = LoadROMPatchProcess(pAdapter, FW7636_ROMPATCH, FW7636_ROMPATCH_LENGTH);
		}
		if (LoadStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load ROM Patch failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!!!! No need to load ROM sem = 0x%x !!!!!\n", pAdapter->RomPatchSemStatus));
	}	
	// step 6 Set DMA scheduler to original mode.
	SchedulerRegister4.Default.BypassMode = backupSchedulerRegister4.Default.BypassMode;
	CRStatus = USBHwHal_WriteMacRegister(pAdapter, SCH_REG4, SchedulerRegister4.word);
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Set DMA scheduler to original mode failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}	
	return LoadStatus;
}

/*
	========================================================================
	
	Routine Description:
		Used in load by driver self not by UI

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS         firmware image load ok
		NDIS_STATUS_FAILURE         image not found

	IRQL = PASSIVE_LEVEL
		
	========================================================================
*/
NTSTATUS NICLoadFirmware(
    IN RTMP_ADAPTER *pAdapter)
{
	NTSTATUS 				LoadStatus = STATUS_SUCCESS;
	NTSTATUS 				CRStatus = STATUS_SUCCESS;
	PUCHAR 					pFirmwareImage = NULL;
	ULONG  					FWImageLength  = 0;
	PFW_BIN_INFO_STRUC_7603 pFwInfo = NULL;
	TOP_MISC_CONTROLS2 	TopMiscControls2;
	U3DMA_WLCFG         		U3DMAWLCFG;
	SCHEDULER_REGISTER4 	SchedulerRegister4, backupSchedulerRegister4;
	ULONG                  		BreakCount = 0;
	ULONG					FileLength = 0;
	FW_BIN_INFO_STRUC_7636 ILMBinInfo, DLMBinInfo;
	ULONG					MACValue = 0;
	BOOLEAN					bIsReDownload = FALSE;

	RtlZeroMemory(&ILMBinInfo, sizeof(FW_BIN_INFO_STRUC_7636));
	//FW Image choose
	//IsUISetFW: Load from UI
	//!IsUISetFW: Load from default mt7603.h 
	if(pAdapter->IsUISetFW)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s Load FW from UI, Image size = %d should be the same as FWSize = %d\n", __FUNCTION__, sizeof(pAdapter->FWImage), pAdapter->FWSize));
		pFirmwareImage = pAdapter->FWImage;	
		FileLength = pAdapter->FWSize;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s Load FW from driver\n", __FUNCTION__));
		if(IS_MT7603(pAdapter))
		{
			pFirmwareImage = FW7603;	
			FileLength = FW7603_LENGTH;
		}
		else if(IS_MT7636(pAdapter))
		{
			pFirmwareImage = FW7636;	
			FileLength = FW7636_LENGTH;
		}
	}
	//DBGPRINT(RT_DEBUG_TRACE, ("%s: Load the internal firmware file, Length = %d\n", __FUNCTION__, FW7603_LENGTH));
//	
	
	if(IS_MT7603(pAdapter))
	{
		// check if need to reload FW
		CRStatus = USBHwHal_ReadMacRegister(pAdapter, TOP_MISC2, &TopMiscControls2.word);
		if (CRStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s Read Mac Register TOP_MISC2 failed\n", __FUNCTION__));
			return STATUS_UNSUCCESSFUL;
		}
		if (TopMiscControls2.Default.FwIsRunning)
		{
			bIsReDownload = TRUE;
		}
		else
		{
			bIsReDownload = FALSE;
		}
			
		if(FileLength <= FW_INFO_LENGTH_7603)
		{
			DBGPRINT(RT_DEBUG_ERROR,("invliad firmware, size < 64 bytes\n"));
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		
	}	
	else if(IS_MT7636(pAdapter))
	{
		ULONG CRValue = 0;
		// check if need to reload FW
		CRStatus = USBHwHal_ReadMacRegister(pAdapter, SW_SYNC, &CRValue);
		if (CRStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s Read Mac Register TOP_MISC2 failed\n", __FUNCTION__));
			return STATUS_UNSUCCESSFUL;
		}
		if (CRValue == 3)
		{
			bIsReDownload = TRUE;
		}
		else
		{
			bIsReDownload = FALSE;
		}
	}
	DBGPRINT(RT_DEBUG_ERROR,("Need to redownload, %d\n", bIsReDownload));
//USB and PCIe
	// Set DMA scheduler to Bypass mode.
	CRStatus = USBHwHal_ReadMacRegister(pAdapter, SCH_REG4, &SchedulerRegister4.word);
	
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Read Mac Register SCH_REG4 failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}
	    
	DBGPRINT(RT_DEBUG_TRACE,("1 SchedulerRegister4 = %x\n", SchedulerRegister4.word));
	backupSchedulerRegister4.Default.BypassMode = SchedulerRegister4.Default.BypassMode;
	SchedulerRegister4.Default.BypassMode = 1;
	SchedulerRegister4.Default.ForceQid = 8;
	DBGPRINT(RT_DEBUG_TRACE,("2 SchedulerRegister4 = %x\n", SchedulerRegister4.word));	
	CRStatus = USBHwHal_WriteMacRegister(pAdapter, SCH_REG4, SchedulerRegister4.word);
	
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Write Mac Register SCH_REG4 failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}
//~USB and PCIe
	//step 2 check if need to restart download, ram to rom
	if (bIsReDownload)
	{
		LoadStatus = FirmwareCommnadRestartDownloadFW(pAdapter);
		if (LoadStatus != STATUS_SUCCESS)
		{               
			DBGPRINT(RT_DEBUG_ERROR,("%s FirmwareCommnadRestartDownloadFW failed\n", __FUNCTION__));
			return STATUS_UNSUCCESSFUL;
		}
	}
	
	if(IS_MT7603(pAdapter))
	{
		
		DBGPRINT(RT_DEBUG_TRACE,("TOP_MISC2 = %x\n", TopMiscControls2.word));	
		pAdapter->BulkOutRemained = 0;
		pAdapter->ContinBulkOut = FALSE;	

		BreakCount = 0;//reset count to 0
		do
		{
			ULONG CRValue = 0;
			CRStatus = USBHwHal_ReadMacRegister(pAdapter, TOP_MISC2, &TopMiscControls2.word);
			if (TopMiscControls2.Default.ReadyToLoadFW)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("!!!!! [7603] Ready to download ram !!!!!\n"));
				break;
			}
		        
			RTMPusecDelay(100000);// 1 ms		        

			BreakCount ++;
			if (BreakCount > 100)
			{
				DBGPRINT(RT_DEBUG_ERROR,("!!!!! TOP is not ready to load FW. !!!!!\n"));
				break;
			}		        
		} while(1);
		pFwInfo           = (PFW_BIN_INFO_STRUC_7603)(pFirmwareImage + (FW7603_LENGTH - sizeof(FW_BIN_INFO_STRUC_7603)));
		FWImageLength    = pFwInfo->ImageLength + 4;  // 4 is CRC length
		DBGPRINT(RT_DEBUG_TRACE,("%s: FileLength = 0x%08X, FWImageLength = 0x%08X\n", __FUNCTION__, FileLength, FWImageLength));
		// Send command packet to FW to set "start to load FW"	
		
		LoadStatus = FirmwareCommnadStartToLoadFW(pAdapter, FWImageLength, 0x00100000,0x80000000);
		if (LoadStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareCommnadStartToLoadFW failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}

		pAdapter->bLoadingFW = TRUE;

		// Start to load FW
		LoadStatus = FirmwareScatters(pAdapter, pFirmwareImage, FWImageLength);
		if (LoadStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareScatters failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}

		pAdapter->bLoadingFW = FALSE;
	}
	else if(IS_MT7636(pAdapter))
	{
		//step 3 Polling 0x81021250 = 0x01, and then can download FW
		BreakCount = 0;//reset count to 0
		do
		{
			ULONG CRValue = 0;
			CRStatus = USBHwHal_ReadMacRegister(pAdapter, SW_SYNC, &CRValue);		
			if (CRValue == 0x1)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Ready to download Rom Patch !!!!!\n"));
				break;
			}
		        
			RTMPusecDelay(100000);// 1 ms		        

			BreakCount ++;
			if (BreakCount > 100)
			{
				DBGPRINT(RT_DEBUG_ERROR,("Polling ready to download Rom Patch failed 0x81021250 = 0x%x\n", CRValue));
				break;
			}
		        
		} while(1);
		BreakCount = 0;//reset count to 0

		// download ILM RAM image
		//pFirmwareImage = FW7636;
		LoadStatus = LoadFwImageProcess(pAdapter, pFirmwareImage, FileLength, ILM);
		if (LoadStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load ILM failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load ILM succeed. !!!!!\n"));
		}

		// download DLM RAM image
		//pFirmwareImage = FW7636;
		LoadStatus = LoadFwImageProcess(pAdapter, pFirmwareImage, FileLength, DLM);
		if (LoadStatus != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load DLM failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load DLM Succeed. !!!!!\n"));
		}		
	}

	// FW Starting
	// difference of 7603 and 7636 difference in the function FirmwareCommnadStartToRunFW
	LoadStatus = FirmwareCommnadStartToRunFW(pAdapter);

	if (LoadStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareCommnadStartToRunFW failed. !!!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	// Set DMA scheduler to original mode.
	SchedulerRegister4.Default.BypassMode = backupSchedulerRegister4.Default.BypassMode;
	CRStatus = USBHwHal_WriteMacRegister(pAdapter, SCH_REG4, SchedulerRegister4.word);
	if (CRStatus != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s Set DMA scheduler to original mode failed\n", __FUNCTION__));
		return STATUS_UNSUCCESSFUL;
	}

	if(IS_MT7603(pAdapter))
	{
		// Polling 0x80021134 [1] = 1, and then can read/write CRs
		do
		{
			CRStatus = USBHwHal_ReadMacRegister(pAdapter, TOP_MISC2, &TopMiscControls2.word);		
			if (TopMiscControls2.Default.FwIsRunning == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load firmware succeed !!!!!\n"));
				break;
			}
	        
			RTMPusecDelay(100000);// 1 ms	        

			BreakCount ++;
			if (BreakCount > 100)
			{
				DBGPRINT(RT_DEBUG_ERROR,("Polling FW ready bit failed.\n"));
				break;
			}
	        
		} while(1);
	}

	else if(IS_MT7636(pAdapter))	
	{	
		BreakCount = 0;		
		// Polling 0x81021250 = 0x03, and then can read/write CRs
		do
		{
			ULONG CRValue = 0;
			CRStatus = USBHwHal_ReadMacRegister(pAdapter, SW_SYNC, &CRValue);				
			if (CRValue == 0x3)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Load firmware succeed !!!!!\n"));
				break;
			}
	        
			RTMPusecDelay(100000);// 1 ms	        

			BreakCount ++;
			if (BreakCount > 100)
			{
				DBGPRINT(RT_DEBUG_ERROR,("Polling FW ready bit failed 0x81021250 = 0x%x\n", CRValue));
				break;
			}

		} while(1);
	}	

	pAdapter->LoadingFWCount ++;
	DBGPRINT(RT_DEBUG_TRACE, ("####################### pAd->LoadingFWCount = %d ###################\n", pAdapter->LoadingFWCount));

	return STATUS_SUCCESS;
}

NTSTATUS FirmwareCommnadRestartDownloadFW(
    IN  RTMP_ADAPTER   *pAdapter
    )
{
	NTSTATUS		Status = STATUS_SUCCESS;
    	NTSTATUS		FWStatus = STATUS_SUCCESS;    
	FIRMWARE_TXDSCR CmdRestartDownloadFwRequest;
	LARGE_INTEGER       TimeOut1Second;

	TimeOut1Second.QuadPart = -(1000* 10000);

	// Fill setting
	RtlZeroMemory(&CmdRestartDownloadFwRequest, sizeof(FIRMWARE_TXDSCR));
	CmdRestartDownloadFwRequest.PQ_ID = 0x8000; //P_IDX[31] = 0x1, Q_IDX[31 : 27] = 0x0
	CmdRestartDownloadFwRequest.CID = 0xEF; // The ID of CMD w/ target address/length request
	CmdRestartDownloadFwRequest.PktTypeID = 0xA0; //bit 15 = 1'b1, bit[14:13] = 0x1
	CmdRestartDownloadFwRequest.Length = sizeof(FIRMWARE_TXDSCR);   

	RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
	
	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(FIRMWARE_TXDSCR);
	RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &CmdRestartDownloadFwRequest, sizeof(FIRMWARE_TXDSCR));

	if(pAdapter->bBulkOutRunning == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
		FWStatus = STATUS_UNSUCCESSFUL;	
	}
	else
	{
		pAdapter->bBulkOutRunning = TRUE;
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
		//Kick bulk out
		Status = UsbDoBulkOutTransfer(pAdapter,BULKOUT_PIPENUMBER_0);	

		do
		{        		

			DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadRestartDownloadFW - wait to get bulk in ###\n"));

			pAdapter->bIsWaitFW = TRUE;
			Status = KeWaitForSingleObject(&pAdapter->WaitFWEvent,
	                                Executive,
	                                KernelMode,
	                                FALSE,
	                                &TimeOut1Second);

			if (Status == STATUS_IO_TIMEOUT)
				DBGPRINT(RT_DEBUG_TRACE, ("!!!!!FirmwareCommnadRestartDownloadFW time out !!!!!\n"));

			if (Status != STATUS_SUCCESS)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("!!!!!FirmwareCommnadRestartDownloadFW Status != STATUS_SUCCESS (%d) !!!!!\n", Status));
				FWStatus = STATUS_UNSUCCESSFUL;
				break;
			}

			DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadRestartDownloadFW - got CMD response ###\n"));
	    
		}while (FALSE);

		RTMPusecDelay(1000);// 1 ms
	}
	return FWStatus;
}

NTSTATUS FirmwareCommnadStartToLoadFW(
	IN  RTMP_ADAPTER    *pAdapter,
	IN  ULONG		ImageLength,
	IN  ULONG		Address,
	IN  ULONG		DataMode
	)
{
	NTSTATUS	Status = STATUS_SUCCESS;
	NTSTATUS	FWStatus = STATUS_SUCCESS;
	CMD_START_TO_LOAD_FW_REQUEST    CmdStartToLoadFwRequest;
	LARGE_INTEGER       TimeOut1Second;
	PFW_BIN_INFO_STRUC_7636 pFWInfo7636 = NULL;
	PFW_BIN_INFO_STRUC_7603 pFWInfo7603 = NULL;
	
	TimeOut1Second.QuadPart = -(1000* 10000);	

	DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadStartToLoadFW ###\n"));

	DBGPRINT(RT_DEBUG_TRACE,("=== FW Info ===\n"));
	DBGPRINT(RT_DEBUG_TRACE,("=== Address = 0x%x ===\n", Address));
	DBGPRINT(RT_DEBUG_TRACE,("=== Length = %d ===\n", ImageLength));

	// Fill setting
	RtlZeroMemory(&CmdStartToLoadFwRequest, sizeof(CMD_START_TO_LOAD_FW_REQUEST));
	CmdStartToLoadFwRequest.FwTxD.PQ_ID = 0x8000; //P_IDX[31] = 0x1, Q_IDX[31 : 27] = 0x0
	CmdStartToLoadFwRequest.FwTxD.CID = 0x01; // The ID of CMD w/ target address/length request
	CmdStartToLoadFwRequest.FwTxD.PktTypeID = 0xA0; //bit 15 = 1'b1, bit[14:13] = 0x1
	CmdStartToLoadFwRequest.FwTxD.Length = sizeof(CMD_START_TO_LOAD_FW_REQUEST);    
	CmdStartToLoadFwRequest.FwTxD.SeqNum = SEQ_CMD_FW_STARTREQ;
	CmdStartToLoadFwRequest.Address = Address;   
	CmdStartToLoadFwRequest.Length = ImageLength;  // FW    size
	CmdStartToLoadFwRequest.DataMode = DataMode;//0x80000000;
	DBGPRINT(RT_DEBUG_TRACE,("=== DataMode = 0x%x ===\n", CmdStartToLoadFwRequest.DataMode));
	RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
	
	/*************************************************************/
	// Setup packet
	// driver must pad 4 bytes 0 at the ending of  frame
	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(CMD_START_TO_LOAD_FW_REQUEST) +4;	
	RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &CmdStartToLoadFwRequest, sizeof(CMD_START_TO_LOAD_FW_REQUEST));
	/*************************************************************/

	/*************************************************************/

	DBGPRINT(RT_DEBUG_TRACE,("dx->TxBufferLength[%d] = %d",BULKOUT_PIPENUMBER_0, pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0]));

	/*************************************************************/

	if(pAdapter->bBulkOutRunning == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
		FWStatus = STATUS_UNSUCCESSFUL;	
	}
	else
	{
		pAdapter->bBulkOutRunning = TRUE;
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
		//Kick bulk out
		Status = UsbDoBulkOutTransfer(pAdapter, BULKOUT_PIPENUMBER_0);	

		DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadStartToLoadFW - wait to get bulk in ###\n"));
		pAdapter->bIsWaitFW = TRUE;
		Status = KeWaitForSingleObject(&pAdapter->WaitFWEvent,
                                Executive,
                                KernelMode,
                                FALSE,
                                &TimeOut1Second);		

		if (Status != STATUS_SUCCESS)
		{
			FWStatus = STATUS_UNSUCCESSFUL;			
        	}
	}
	/*************************************************************/

	return FWStatus;
}

NTSTATUS FirmwareScatters(
	IN  RTMP_ADAPTER *pAdapter,
	IN  PUCHAR          pFirmwareImage,
	IN  ULONG           ImageLength
	)
{
	NTSTATUS Status = STATUS_SUCCESS;	
	ULONG sentLen = 0, remainLen = 0, i = 0;
	ULONG MacValue=0;	
	ULONG busyCount = 0;
	PUCHAR          pCopyPointer;
	FIRMWARE_TXDSCR FwTxD;
	LARGE_INTEGER       TimeOut1Second;
	ULONG  UnitSize = 14336;//14336 in 7603

	TimeOut1Second.QuadPart = -(2000* 10000);
        
	sentLen = 0;
	remainLen = ImageLength;
	pCopyPointer = pFirmwareImage;

	DBGPRINT(RT_DEBUG_TRACE,("### FirmwareScatters ###\n"));


	//
	// end out 14k bytes to Andes till end of firmware
	//      14336
    
	while (remainLen > 0)
	{
		RTMPusecDelay(100000);// 1 ms
		if(remainLen > UnitSize)
		{
			sentLen = UnitSize;  // copy 14k bytes at most
			remainLen = remainLen - UnitSize;
		}
		else
		{
			sentLen = remainLen;
			remainLen = 0;
		}
	
		DBGPRINT(RT_DEBUG_TRACE,("copying the %d-th Kbytes....sentLen %d \n",++i, sentLen));
		DBGPRINT(RT_DEBUG_TRACE,("Total FileLength %d len remain %d \n", ImageLength, remainLen));                
		
		RtlZeroMemory(&FwTxD, sizeof(FIRMWARE_TXDSCR));
		FwTxD.PQ_ID = 0xC000; //P_IDX[31] = 0x1, Q_IDX[30 : 27] = 0x0
		FwTxD.CID = 0xEE; // The ID of CMD w/ target address/length request
		FwTxD.PktTypeID = 0xA0; //bit 15 = 1'b1, bit[14:13] = 0x1
		FwTxD.Length = sizeof(FIRMWARE_TXDSCR) + sentLen;
		RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
        	
       	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(FIRMWARE_TXDSCR) + sentLen +4 ;
		
		RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &FwTxD, sizeof(FIRMWARE_TXDSCR));
		RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0] + sizeof(FIRMWARE_TXDSCR), pCopyPointer, sentLen);		

		pCopyPointer += sentLen;
		
		if(pAdapter->bBulkOutRunning == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
			Status = STATUS_UNSUCCESSFUL;
			break;
		}
		else
		{
			pAdapter->bBulkOutRunning = TRUE;
			DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
			//Kick bulk out
			Status = UsbDoBulkOutTransfer(pAdapter, BULKOUT_PIPENUMBER_0);	
			Status = KeWaitForSingleObject(&pAdapter->LoadFWEvent,
                                Executive,
                                KernelMode,
                                FALSE,
                                &TimeOut1Second);
			if (Status != STATUS_SUCCESS || pAdapter->bLoadingFW == FALSE)
			{				
				DBGPRINT(RT_DEBUG_TRACE,("%s  wait for event LoadFWEvent failed status = %d\n", __FUNCTION__, Status));
				break;
			}
		}		

		RTMPusecDelay(100);// 1 ms

	}
    
	return Status;
}

NTSTATUS FirmwareCommnadStartToRunFW(
	IN  RTMP_ADAPTER *pAdapter
	)
{
	NTSTATUS		Status = STATUS_SUCCESS;
	NTSTATUS		FWStatus = STATUS_SUCCESS;	
	CMD_START_FW_REQUEST    CmdStartFwRequest;
	LARGE_INTEGER       TimeOut1Second;
	

	TimeOut1Second.QuadPart = -(500* 10000);

	DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadStartToRunFW ###\n"));

	// Fill setting
	RtlZeroMemory(&CmdStartFwRequest, sizeof(CMD_START_FW_REQUEST));
	CmdStartFwRequest.FwTxD.PQ_ID = 0x8000; //P_IDX[31] = 0x1, Q_IDX[31 : 27] = 0x0
	CmdStartFwRequest.FwTxD.CID = 0x02; // The ID of CMD w/ target address/length request
	CmdStartFwRequest.FwTxD.PktTypeID = 0xA0; //bit 15 = 1'b1, bit[14:13] = 0x1
	CmdStartFwRequest.FwTxD.Length = sizeof(CMD_START_FW_REQUEST);  
	CmdStartFwRequest.FwTxD.SeqNum = SEQ_CMD_FW_STARTTORUN;
	if(IS_MT7603(pAdapter))
	{
	CmdStartFwRequest.Override = 0x1;
		CmdStartFwRequest.Address = 0x00100000;//0x00100000;
	}
	else if(IS_MT7636(pAdapter))
	{
		CmdStartFwRequest.Override = 0x0;
		//Override =0 => ignore Address
	}
	
	
	RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
	
	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(CMD_START_FW_REQUEST);
	RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &CmdStartFwRequest, sizeof(CMD_START_FW_REQUEST));
    
	if(pAdapter->bBulkOutRunning == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
		FWStatus = STATUS_UNSUCCESSFUL;	
	}
	else
	{
		pAdapter->bBulkOutRunning = TRUE;
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
		//Kick bulk out
		Status = UsbDoBulkOutTransfer(pAdapter,BULKOUT_PIPENUMBER_0);	
		pAdapter->bIsWaitFW = TRUE;
		Status = KeWaitForSingleObject(&pAdapter->WaitFWEvent,
                                Executive,
                                KernelMode,
                                FALSE,
                                &TimeOut1Second);		

		if (Status != STATUS_SUCCESS)
		{
			FWStatus = STATUS_UNSUCCESSFUL;			
        	}
	}

	RTMPusecDelay(1000);// 1 ms

	DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadStartToRunFW - got CMD response ###\n"));        
        
	return Status;
}

//
// Load Andes ROM PATCH
// MT7636 FPGA
//
NTSTATUS LoadROMPatchProcess(
	IN RTMP_ADAPTER *pAdapter,
	IN PUCHAR	pFileName,
	UINT32		FileLength
	)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PUCHAR pFirmwareImage = pFileName;
	ULONG  FWImageLength  = 0;
	BOOLEAN bLoadExternalFW = FALSE;
	PROM_PATCH_BIN_HEADER_STRUC	pROMInfo = NULL;
	ROMPATCH_CAL_CHECKSUM_DATA	CalChksumData;
	//checksume return by FW
	USHORT	FWChecksum = 0, ImgChecksum = 0;	
	UCHAR BreakCount = 0;
	
	if(FileLength <= ROM_PATCH_INFO_LENGTH)
	{
		DBGPRINT(RT_DEBUG_ERROR,("invalid ROM patch, size < 30 bytes\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	pROMInfo = (PROM_PATCH_BIN_HEADER_STRUC)(pFirmwareImage);
	FWImageLength = FileLength - ROM_PATCH_INFO_LENGTH;
	ImgChecksum	 =  pROMInfo->Checksum;

	DBGPRINT(RT_DEBUG_TRACE,("%s: FileLength = 0x%08X, ROM patch Length = 0x%08X\n", __FUNCTION__, FileLength, FWImageLength));
	DBGPRINT(RT_DEBUG_TRACE,("pROMInfo->Checksum = 0x%04X\n", pROMInfo->Checksum));	
	
	do
	{
		// Send command packet to FW to set "start to load FW"
		Status = FirmwareCommnadStartToLoadROMPatch(pAdapter, ROM_PATCH_ADDRESS, FWImageLength);
		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! LoadROMPatchProcess failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}

		RTMPusecDelay(1000);

		pAdapter->bLoadingFW = TRUE;

		// Start to load FW
		Status = FirmwareScatters(pAdapter, pFirmwareImage+ROM_PATCH_INFO_LENGTH, FWImageLength);
		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareScatters failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}

		pAdapter->bLoadingFW = FALSE;


	} while(FALSE);	

	// send CMD patch finish
	Status = FirmwareCommnadLoadROMPatchFinish(pAdapter);
	if (Status != STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareCommnadLoadROMPatchFinish failed. !!!!!\n"));
		return STATUS_UNSUCCESSFUL;
	}

	// request FW to calculate download ROM image checksum
	CalChksumData.Address = ROM_PATCH_ADDRESS;
	CalChksumData.Length = FWImageLength;
	USBROMPatchCalChecksum(pAdapter, &CalChksumData, sizeof(ROMPATCH_CAL_CHECKSUM_DATA));
	USBROMPatchGetChecksum(pAdapter, &FWChecksum, sizeof(FWChecksum));

	DBGPRINT(RT_DEBUG_ERROR,("FWChecksum = 0x%04X, ImgChecksum = 0x%04X\n", FWChecksum, ImgChecksum));

	if(FWChecksum != ImgChecksum)
	{
		Status = STATUS_UNSUCCESSFUL;
	}	
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Rom Patch download succeed !!!!!\n"));
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));

	return Status;
}
	// ===============================================
	// Load Firmware 7636 
	// ===============================================
	//  Andes FW loading architecture
	//
	//  Bin file format
	//    +--------------------------+
	//    |       FW ILM part        	| 4N or (16N-4) bytes
	//    +--------------------------+
	//    |       ILM CRC       	| 4 bytes
	//    +--------------------------+
	//    |       FW DLM part		| 4N or (16N-4) bytes
	//    +--------------------------+		
	//    |       DLM CRC       	| 4 bytes
	//    +--------------------------+	
	//    |       ILM Tailer (Info)	| 36 bytes
	//    +--------------------------+		
	//    |       DLM Tailer (Info)	| 36 bytes
	//    +--------------------------+		
//
// Load Andes ROM patch code
// MT7603 FPGA
//
NTSTATUS LoadFwImageProcess(
	IN RTMP_ADAPTER *pAdapter,
	IN PUCHAR	pFileName,
	IN UINT32	FileLength,
	IN UCHAR	Type
	)
{
	NTSTATUS Status = STATUS_SUCCESS;
	//UINT32 FileLength = 0;
	PUCHAR pFirmwareImage = pFileName;
	ULONG  FWImageLength  = 0;
	PFW_BIN_INFO_STRUC_7636 pFwInfo = NULL;
	ULONG  BreakCount = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: file length = %d\n", __FUNCTION__,FileLength));
	
	if(FileLength <= FW_INFO_LENGTH_7636)
	{
		DBGPRINT(RT_DEBUG_ERROR,("invliad firmware, size < 36 bytes\n"));
		return STATUS_UNSUCCESSFUL;
	}
	//ILM
	if(ILM == Type)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s  ===ILM===\n", __FUNCTION__));
		pFwInfo = (PFW_BIN_INFO_STRUC_7636)(pFirmwareImage + (FileLength - 2*sizeof(FW_BIN_INFO_STRUC_7636) ));
		
	}
	//DLM
	else if(DLM == Type)
	{
		PFW_BIN_INFO_STRUC_7636 ILMInfo = NULL;
		DBGPRINT(RT_DEBUG_TRACE,("%s  ===DLM===\n", __FUNCTION__));
		ILMInfo = (PFW_BIN_INFO_STRUC_7636)(pFirmwareImage + (FileLength - 2*sizeof(FW_BIN_INFO_STRUC_7636) ));
	
	pFwInfo           = (PFW_BIN_INFO_STRUC_7636)(pFirmwareImage + (FileLength - sizeof(FW_BIN_INFO_STRUC_7636)));
		pFirmwareImage = pFirmwareImage + ILMInfo->ImageLength + 4;
	}		
	FWImageLength    = pFwInfo->ImageLength + 4;  // 4 is CRC length

	DBGPRINT(RT_DEBUG_TRACE,("%s: FileLength = %d, FWImageLength = %d\n", __FUNCTION__, FileLength, FWImageLength));	
	DBGPRINT(RT_DEBUG_TRACE,("=== FW Info ===\n"));
	DBGPRINT(RT_DEBUG_TRACE,("%s: Address = 0x%x\n", __FUNCTION__, pFwInfo->Address));
	DBGPRINT(RT_DEBUG_TRACE,("=== Address = 0x%x ===\n", pFwInfo->Address));
	DBGPRINT(RT_DEBUG_TRACE,("=== Length = %d ===\n", FWImageLength));
	DBGPRINT(RT_DEBUG_TRACE,("=== Encryption = %d ===\n", pFwInfo->FeatureSet.Encryption));
	DBGPRINT(RT_DEBUG_TRACE,("=== Date = 0x%x ===\n", pFwInfo->Date));
	DBGPRINT(RT_DEBUG_TRACE,("=== Version = 0x%x ===\n", pFwInfo->Version));
	do
	{
		ULONG CRValue = 0;
		Status = USBHwHal_ReadMacRegister(pAdapter, SW_SYNC, &CRValue);		
		if (CRValue == 0x1)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! Ready to download Ram !!!!!\n"));
			break;
		}
	        
		RTMPusecDelay(100000);// 1 ms		        
		BreakCount ++;
		if (BreakCount > 100)
		{
			DBGPRINT(RT_DEBUG_ERROR,("Polling ready to download Ram failed 0x81021250 = 0x%x\n", CRValue));
			break;
		}
		        
	} while(1);
	

	do
	{
		ULONG DataMode = 0x80000000;//parameter for start to load fw
		if (IS_MT7636(pAdapter) && pFwInfo->FeatureSet.Encryption ==1)		
		{
			DBGPRINT(RT_DEBUG_TRACE,("=== Encryption = %d ===\n", pFwInfo->FeatureSet.Encryption));
			DBGPRINT(RT_DEBUG_TRACE,("=== KEY_INDEX = %d ===\n", pFwInfo->FeatureSet.KEY_INDEX));			
			DataMode |= pFwInfo->FeatureSet.Encryption;
			DataMode |= (pFwInfo->FeatureSet.KEY_INDEX << 1);
			DataMode |= 0x8;//turn on bit 3			
		}		
		// Send command packet to FW to set "start to load FW"
		Status = FirmwareCommnadStartToLoadFW(pAdapter, FWImageLength,  pFwInfo->Address, DataMode);
		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareCommnadStartToLoadFW failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}

		RTMPusecDelay(1000);
		pAdapter->bLoadingFW = TRUE;

		// Start to load FW
		Status = FirmwareScatters(pAdapter, pFirmwareImage, FWImageLength);
		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!!!! FirmwareScatters failed. !!!!!\n"));
			return STATUS_UNSUCCESSFUL;
		}

		pAdapter->bLoadingFW = FALSE;

	} while(FALSE);
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));
	
	return Status;
}

//
// MT7636 firmware command start to load patch
//
NTSTATUS FirmwareCommnadStartToLoadROMPatch(
	IN	RTMP_ADAPTER	*pAdapter,
	IN	ULONG			Address,
	IN	ULONG			Length
	)
{
	NTSTATUS 			Status = STATUS_SUCCESS;
	NTSTATUS 			FWStatus = STATUS_SUCCESS;
	CMD_START_TO_LOAD_FW_REQUEST	CmdStartToLoadFwRequest;
	LARGE_INTEGER		TimeOut1Second;
	UINT32				BreakCount = 0;

	TimeOut1Second.QuadPart = -(500* 10000);

	DBGPRINT(RT_DEBUG_TRACE,("### %s ###\n", __FUNCTION__));

	// Fill setting
	RtlZeroMemory(&CmdStartToLoadFwRequest, sizeof(CMD_START_TO_LOAD_FW_REQUEST));
	CmdStartToLoadFwRequest.FwTxD.PQ_ID = 0x8000; //P_IDX[31] = 0x1, Q_IDX[31 : 27] = 0x0
	CmdStartToLoadFwRequest.FwTxD.CID = 0x05; // The ID of CMD w/ target address/length request
	CmdStartToLoadFwRequest.FwTxD.PktTypeID = 0xA0; //bit 15 = 1'b1, bit[14:13] = 0x1
	CmdStartToLoadFwRequest.FwTxD.Length = sizeof(CMD_START_TO_LOAD_FW_REQUEST);	
	CmdStartToLoadFwRequest.FwTxD.SeqNum = SEQ_CMD_ROM_PATCH_STARTREQ;
	CmdStartToLoadFwRequest.Address = Address;	
	CmdStartToLoadFwRequest.Length = Length;  // 4 is CRC length;
	CmdStartToLoadFwRequest.DataMode = 0x80000000;			// 0: plan mode
	
	DBGPRINT(RT_DEBUG_TRACE,("CmdStartToLoadFwRequest.Address=0x%x, CmdStartToLoadFwRequest.Length=0x%x, CmdStartToLoadFwRequest.DataMode = 0x%x\n", 

		CmdStartToLoadFwRequest.Address, CmdStartToLoadFwRequest.Length, CmdStartToLoadFwRequest.DataMode));
	
	RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
	/*************************************************************/
	// Setup packet
	// driver must pad 4 bytes 0 at the ending of  frame
	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(CMD_START_TO_LOAD_FW_REQUEST) +4;	
	RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &CmdStartToLoadFwRequest, sizeof(CMD_START_TO_LOAD_FW_REQUEST));
	/*************************************************************/
	DBGPRINT(RT_DEBUG_TRACE,("dx->TxBufferLength[%d] = %d",BULKOUT_PIPENUMBER_0, pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0]));

	/*************************************************************/

	while(1)
	{
		if(pAdapter->bBulkOutRunning == FALSE)
			break;
		RTMPusecDelay(100);
		if( 1000 == BreakCount++)
			break;
	}

	if(pAdapter->bBulkOutRunning == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
		FWStatus = STATUS_UNSUCCESSFUL;	
	}
	else
	{
		pAdapter->bBulkOutRunning = TRUE;
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
		//Kick bulk out
		Status = UsbDoBulkOutTransfer(pAdapter,BULKOUT_PIPENUMBER_0);	

		DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadStartToLoadROMPatch - wait to get bulk in ###\n"));
		pAdapter->bIsWaitFW = TRUE;
		Status = KeWaitForSingleObject(&pAdapter->WaitFWEvent,
                                Executive,
                                KernelMode,
                                FALSE,
                                &TimeOut1Second);
		

		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("### FirmwareCommnadStartToLoadROMPatch- timeout ###\n"));
			FWStatus = STATUS_UNSUCCESSFUL;			
        	}
	}
	/*************************************************************/
	return FWStatus;
}

NTSTATUS FirmwareCommnadGetPatchSemaphore(
	IN	RTMP_ADAPTER	*pAdapter
	)
{
	NTSTATUS 			Status = STATUS_SUCCESS;
	CMD_GET_PATCH_SEMAPHORE	CmdGetPatchSem;
	LARGE_INTEGER		TimeOut1Second;

	TimeOut1Second.QuadPart = -(500* 10000);
	RtlZeroMemory(&CmdGetPatchSem, sizeof(CMD_GET_PATCH_SEMAPHORE));
	CmdGetPatchSem.FwTxD.PQ_ID 	= 0x8000; //P_IDX[31] = 0x1, Q_IDX[31 : 27] = 0x0
	CmdGetPatchSem.FwTxD.Length 	= 0x10;
	CmdGetPatchSem.FwTxD.CID 	= 0x10;
	CmdGetPatchSem.FwTxD.PktTypeID 	= 0xA0;
	CmdGetPatchSem.FwTxD.SeqNum= SEQ_CMD_ROM_PATCH_SEMAPHORE;
	CmdGetPatchSem.ucGetSemaphore	= 0x1;

	RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
	/*************************************************************/
	// Setup packet
	// driver must pad 4 bytes 0 at the ending of  frame
	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(CMD_GET_PATCH_SEMAPHORE) + 4;	
	RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &CmdGetPatchSem, sizeof(CMD_GET_PATCH_SEMAPHORE));
	/*************************************************************/
	/*************************************************************/
	
	if(pAdapter->bBulkOutRunning == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
		Status = STATUS_UNSUCCESSFUL;	
	}
	else
	{
		pAdapter->bBulkOutRunning = TRUE;
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
		//Kick bulk out
		Status = UsbDoBulkOutTransfer(pAdapter,BULKOUT_PIPENUMBER_0);	

		DBGPRINT(RT_DEBUG_TRACE,("### FirmwareCommnadGetPatchSemaphore- wait to get bulk in ###\n"));
		Status = KeWaitForSingleObject(&pAdapter->RomPatchEvent,
                                Executive,
                                KernelMode,
                                FALSE,
                                &TimeOut1Second);		

		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("### FirmwareCommnadGetPatchSemaphore- timeout ###\n"));	
			Status = STATUS_UNSUCCESSFUL;			
        	}
		else
		{			
			DBGPRINT(RT_DEBUG_TRACE,("%s get seqnum = %d, from MCU  ,RspStatus =%d\n",__FUNCTION__,g_FWSeqMCU, pAdapter->RomPatchSemStatus));
		}
	}
	/*************************************************************/

	return Status;
}

//
// MT7636 firmware command patch finish
//
NTSTATUS FirmwareCommnadLoadROMPatchFinish(
	IN	RTMP_ADAPTER	*pAdapter
	)
{
	NTSTATUS 			Status = STATUS_SUCCESS;
	NTSTATUS 			FWStatus = STATUS_SUCCESS;	
	FIRMWARE_TXDSCR	CmdRomPatchFinish;
	LARGE_INTEGER		TimeOut1Second;

	TimeOut1Second.QuadPart = -(500* 10000);

	DBGPRINT(RT_DEBUG_TRACE,("### %s ###\n", __FUNCTION__));

	// Fill setting
	RtlZeroMemory(&CmdRomPatchFinish, sizeof(FIRMWARE_TXDSCR));
	CmdRomPatchFinish.PQ_ID = 0x8000; //P_IDX[31] = 0x1, Q_IDX[31 : 27] = 0x0
	CmdRomPatchFinish.CID = 0x07; // The ID of CMD w/ target address/length request
	CmdRomPatchFinish.PktTypeID = 0xA0; //bit 15 = 1'b1, bit[14:13] = 0x1
	CmdRomPatchFinish.Length = sizeof(FIRMWARE_TXDSCR);	
	CmdRomPatchFinish.SeqNum = SEQ_CMD_ROM_PATCH_FINISH;

	RtlZeroMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0],  USB_BULK_BUFFER_COUNT*USB_BULK_BUFFER_SIZE );
	/*************************************************************/
	// Setup packet
	// driver must pad 4 bytes 0 at the ending of  frame
	pAdapter->TxBufferLength[BULKOUT_PIPENUMBER_0] = sizeof(FIRMWARE_TXDSCR) + 4;	
	RtlMoveMemory(pAdapter->TxBuffer[BULKOUT_PIPENUMBER_0], &CmdRomPatchFinish, sizeof(FIRMWARE_TXDSCR));
	/*************************************************************/
	/*************************************************************/
	
	if(pAdapter->bBulkOutRunning == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d] is still running\n", BULKOUT_PIPENUMBER_0));
		FWStatus = STATUS_UNSUCCESSFUL;	
	}
	else
	{
		pAdapter->bBulkOutRunning = TRUE;
		DBGPRINT(RT_DEBUG_TRACE,("BulkOut [%d]\n", BULKOUT_PIPENUMBER_0));
		//Kick bulk out
		Status = UsbDoBulkOutTransfer(pAdapter,BULKOUT_PIPENUMBER_0);	

		DBGPRINT(RT_DEBUG_TRACE,("### FirmwareLoadROMPatchFinish- wait to get bulk in ###\n"));
		pAdapter->bIsWaitFW = TRUE;
		Status = KeWaitForSingleObject(&pAdapter->WaitFWEvent,
                                Executive,
                                KernelMode,
                                FALSE,
                                &TimeOut1Second);
		

		if (Status != STATUS_SUCCESS)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("### FirmwareCommnadLoadROMPatchFinish- timeout ###\n"));
			FWStatus = STATUS_UNSUCCESSFUL;			
        	}
	}
	/*************************************************************/
	
	return FWStatus;
}

NTSTATUS	ROMPatchPowerOnWiFiSys(
	IN	RTMP_ADAPTER	*pAdapter)
{
	NTSTATUS	ntStatus = STATUS_SUCCESS;
	PUCHAR		pData = 0;	
	USHORT UrbSize;
	PURB urb;
	ULONG		Length = sizeof(UCHAR);
	DBGPRINT(RT_DEBUG_TRACE, ("-->%s\n", __FUNCTION__));	

	// Allocate memory for URB
	UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s No URB memory\n",__FUNCTION__));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_OUT,	// Direction out
		DEVICE_VENDOR_REQUEST_OUT,						// Reserved bits
		0x04,						// Request
		0x0,					// Value
		0x1,			// index
		NULL, NULL, 0,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( pAdapter, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);


	return ntStatus;
}

NTSTATUS	USBROMPatchCalChecksum(
	IN	RTMP_ADAPTER	*pAdapter,
	IN	PVOID			pData,
	IN	USHORT			Length)
{
	NTSTATUS	ntStatus = 0;
	USHORT UrbSize;
	PURB urb;
	DBGPRINT(RT_DEBUG_TRACE, ("-->%s\n", __FUNCTION__));	

	// Allocate memory for URB
	UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s No URB memory\n",__FUNCTION__));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_OUT,	// Direction out
		DEVICE_VENDOR_REQUEST_OUT,						// Reserved bits
		0x01,						// Request
		0x20,					// Value
		0x0,			// index
		pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( pAdapter, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

	return ntStatus;
}

NTSTATUS	USBROMPatchGetChecksum(
	IN	RTMP_ADAPTER	*pAdapter,
	IN	PVOID			pData,
	IN	USHORT			Length)
{
	NTSTATUS	ntStatus = 0;	
	// Allocate memory for URB
	USHORT UrbSize = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
	PURB urb = (PURB)ExAllocatePool(NonPagedPool, UrbSize);
	DBGPRINT(RT_DEBUG_TRACE, ("-->%s\n", __FUNCTION__));
	if( urb==NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE,("No URB memory\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	// Build URB to send vendor control request on Default pipe
	UsbBuildVendorRequest(urb,
		URB_FUNCTION_VENDOR_DEVICE, UrbSize,
		USBD_TRANSFER_DIRECTION_IN | USBD_SHORT_TRANSFER_OK,	// Direction out
		DEVICE_VENDOR_REQUEST_IN,						// Reserved bits
		0x01,						// Request
		0x21,					// Value
		0x0,			// index
		(USHORT*)pData, NULL, Length,				// Output data
		NULL);

	// Call the USB driver
	ntStatus = CallUSBDI( pAdapter, urb, IOCTL_INTERNAL_USB_SUBMIT_URB, 0);
	// Check statuses
	if( !NT_SUCCESS(ntStatus) || !USBD_SUCCESS( urb->UrbHeader.Status))
	{
		DBGPRINT(RT_DEBUG_TRACE,("status %x URB status %x\n", ntStatus, urb->UrbHeader.Status));
		ntStatus = STATUS_UNSUCCESSFUL;
	}
	ExFreePool(urb);

	return ntStatus;
}


/*
	========================================================================
	
	Routine Description:
		Enable Wlan function. this action will enable wlan clock so that chip can accept command. So MUST put in the 
		very beginning of Initialization. And put in the very LAST in the Halt function.

	Arguments:
		pAd						Pointer to our adapter

	Return Value:
		None

	IRQL <= DISPATCH_LEVEL
	
	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
VOID RTMPEnableWlan(
	IN	RTMP_ADAPTER *pDevExt,
	IN BOOLEAN bOn)
{
	WLAN_FUN_CTRL_STRUC 	WlanFunCtrl;
	CMB_CTRL_STRUC			CmbCtrl;
	UCHAR		index;

	
	//RTMP_Real_IO_READ32(pDevExt, WLAN_FUN_CTRL,&WlanFunCtrl.word);
	if (bOn == TRUE)
	{
		WlanFunCtrl.WLAN_EN = 1;
		WlanFunCtrl.WLAN_CLK_EN = 1;
	}
	else
	{
		WlanFunCtrl.WLAN_EN = 0;
		WlanFunCtrl.WLAN_CLK_EN = 0;
	}
	//RTMP_Real_IO_WRITE32(pDevExt, WLAN_FUN_CTRL, WlanFunCtrl.word);

	RTMPusecDelay(1);
	index = 0;
	CmbCtrl.word = 0;
	/*if (bOn == TRUE)//--werner 20130701
	{
		do 
		{
//			CmbCtrl.word = RTMP_Real_IO_READ32(pDevExt, CMB_CTRL);

			if (CmbCtrl.PLL_LD == 1)
				break;

			RTMPusecDelay(10);
		} while (index++ < 1000);

		DBGPRINT(RT_DEBUG_TRACE,"[ CMB_CTRL =0x%08x]. index = %d.\n", pDevExt->MACVersion, index);
	}*/
}

ULONG GetTXWISize (IN	RTMP_ADAPTER *pAdapter)
{
	ULONG TXWISize = 0;

	/*if ( IS_MT7662(pAdapter->MACVersion) || IS_MT7650(pAdapter->MACVersion) || IS_MT7630(pAdapter->MACVersion) || IS_MT7610(pAdapter->MACVersion) || IS_MT7603(pAdapter->MACVersion))
	{
		TXWISize= sizeof(TX_WI_STRUC);		
	}
	else
	{
		TXWISize = sizeof(TX_WI_STRUC) - sizeof(ULONG);
	}*/
	TXWISize = sizeof(TX_WI_STRUC);
	DBGPRINT(RT_DEBUG_TRACE,("TXWISize = %d.\n", TXWISize));
	return TXWISize;
}

ULONG GetRXWISize (IN RTMP_ADAPTER *pAdapter)
{
	ULONG RXWISize = 0;

	/*if ( IS_MT7662(pAdapter->MACVersion) || IS_MT7650(pAdapter->MACVersion) || IS_MT7630(pAdapter->MACVersion) || IS_MT7610(pAdapter->MACVersion) || IS_MT7603(pAdapter->MACVersion))
	{
		RXWISize= sizeof(RX_WI_STRUC);
	}
	else
	{
		// The old RXWI structure does not include the RSSI_ANT0 field, the BFSNR_0 field, the BFSNR_1 field and the BFSNR_2 field.
		RXWISize = sizeof(RX_WI_STRUC) - 3*sizeof(ULONG);
	}*/
	RXWISize = sizeof(RX_WI_STRUC);
	DBGPRINT(RT_DEBUG_TRACE,("RXWISize = %d.\n", RXWISize));
	return RXWISize;
}
VOID WriteRXVToFile(
    IN	HANDLE   	handle,
    IN	PCHAR buffer,
    IN	ULONG buffersize)
{
	NTSTATUS ntstatus = 0;
	IO_STATUS_BLOCK    ioStatusBlock;
	size_t  cb;
	CHAR bufferT[1024];

	DBGPRINT(RT_DEBUG_TRACE,("-->\n"));

	if(NT_SUCCESS(ntstatus))
	{
	    ntstatus = RtlStringCbPrintfA(bufferT, sizeof(bufferT), "This is %d test1234,234\r\n", 0x0);
	    if(NT_SUCCESS(ntstatus)) 
	    {
	      	    ntstatus = RtlStringCbLengthA(bufferT, sizeof(bufferT), &cb);
	           if(NT_SUCCESS(ntstatus)) 
		    {
	                ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
		          		       bufferT, cb, NULL, NULL);
	           }
	    }
	    ZwClose(handle);
	}	
	DBGPRINT(RT_DEBUG_TRACE,("<--\n"));
}
/*
	========================================================================
	
	Routine Description:
		Compare two memory block

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		1:			memory are equal
		0:			memory are not equal

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
ULONG	RTMPEqualMemory(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index = 0;

	pMem1 = (PUCHAR) pSrc1;
	pMem2 = (PUCHAR) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] != pMem2[Index])
		{
			break;
		}
	}

	if ((Index == Length))
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

/*
	========================================================================
	
	Routine Description:
		Compare two memory block

	Arguments:
		pSrc1		Pointer to first memory address
		pSrc2		Pointer to second memory address
		
	Return Value:
		0:			memory is equal
		1:			pSrc1 memory is larger
		2:			pSrc2 memory is larger

	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
ULONG	RTMPCompareMemory(
	IN	PVOID	pSrc1,
	IN	PVOID	pSrc2,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index = 0;

	pMem1 = (PUCHAR) pSrc1;
	pMem2 = (PUCHAR) pSrc2;

	for (Index = 0; Index < Length; Index++)
	{
		if (pMem1[Index] > pMem2[Index])
			return (1);
		else if (pMem1[Index] < pMem2[Index])
			return (2);
	}

	// Equal
	return (0);
}

/*
	========================================================================
	
	Routine Description:
		Zero out memory block

	Arguments:
		pSrc1		Pointer to memory address
		Length		Size

	Return Value:
		None
		
	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	RTMPZeroMemory(
	IN	PVOID	pSrc,
	IN	ULONG	Length)
{
	PUCHAR	pMem;
	ULONG	Index = 0;

	pMem = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem[Index] = 0x00;
	}
}

/*
	========================================================================
	
	Routine Description:
		Copy data from memory block 1 to memory block 2

	Arguments:
		pDest		Pointer to destination memory address
		pSrc		Pointer to source memory address
		Length		Copy size
		
	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	Note:
		
	========================================================================
*/
VOID	RTMPMoveMemory(
	OUT	PVOID	pDest,
	IN	PVOID	pSrc,
	IN	ULONG	Length)
{
	PUCHAR	pMem1;
	PUCHAR	pMem2;
	ULONG	Index;

	ASSERT((Length==0) || (pDest && pSrc));

	pMem1 = (PUCHAR) pDest;
	pMem2 = (PUCHAR) pSrc;

	for (Index = 0; Index < Length; Index++)
	{
		pMem1[Index] = pMem2[Index];
	}
}
/*
	========================================================================
	
	Routine Description:
		handle RX vector, print 
		

	Arguments:
		pDest		Pointer to destination memory address
		pSrc		Pointer to source memory address
		Length		Copy size
		
	Return Value:
		None


	
	Note:
		
	========================================================================
*/
VOID HandleRXVector(
	IN PUCHAR   pData,
	IN	RTMP_ADAPTER *pAdapter
	)
{
	RX_INFO_DW0 RxInfoDW0;
	RXV_HEADER rxv_header;
	TX_STATUS tx_status;
	ULONG Temp = 0;
	char			rxvPrintBuffer[1024] = "";
	RXV			rxv;
	ULONG			i = 0;
	ULONG		TmpValue = 0;

	DBGPRINT(RT_DEBUG_TRACE,("%s --->>\n",__FUNCTION__));

	RtlZeroMemory(&RxInfoDW0, sizeof(RxInfoDW0));
	RtlZeroMemory(&rxv_header, sizeof(rxv_header));
	RtlZeroMemory(&rxv, sizeof(rxv));
	memcpy(&RxInfoDW0,pData,sizeof(RX_INFO_DW0));
	if (RX_PKT_TYPE_RX_NORMAL==RxInfoDW0.PKT_TYPE || RX_PKT_TYPE_RX_DUP_FRB==RxInfoDW0.PKT_TYPE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Get RX_PKT_TYPE_RX_TXRXV or RX_PKT_TYPE_RX_DUP_FRB packet\n"));
	}
	else if (RX_PKT_TYPE_RX_DUP_FRB==RxInfoDW0.PKT_TYPE)
	{
		DBGPRINT(RT_DEBUG_TRACE,("TODO RX_PKT_TYPE_RX_DUP_FRB type\n"));
	}
	else if (RX_PKT_TYPE_RX_TXRXV==RxInfoDW0.PKT_TYPE)
	{
		memcpy(&rxv_header,pData,sizeof(rxv_header));
		DBGPRINT(RT_DEBUG_TRACE,("RXV_CNT = %d\n",rxv_header.RXV_CNT));
		Temp = rxv_header.RXV_CNT;
		pData+=sizeof(rxv_header);
		RtlZeroMemory(rxvPrintBuffer, sizeof(rxvPrintBuffer));
		for(i=0;i<Temp;i++)
		{
			memcpy(&rxv,pData,sizeof(rxv));
			RtlStringCbPrintfA(rxvPrintBuffer, sizeof(rxvPrintBuffer), 
							"txrate %d,HT_stbc %d,HT_adcode %d,\
HT_extitf %d,txmode %d,frmode %d,\
VHTA1_B22 %d,HT_aggregation %d,HT_shortgi %d,\
HT_smooth %d,HT_no_sound %d,VHTA2_B8_B1 %d,\
VHTA1_B5_B4 %d,\
Length %d,VHTA1_B16_B6 %d,\
VHTA1_B21_B17 %d,OFDM_FreqTrans_DET %d,ACI_DETx %d,\
SEL_ANT %d,RCPI0 %d,FAGC0_EQ_CAL %d,\
FGAC0_CAL_GAIN %d,RCPI1 %d,FAGC1_EQ_CAL %d,\
FGAC1_CAL_GAIN %d,\
IB_RSSIx %d,WB_RSSIx %d,FAGC_LPF_GAINx %d,\
IB_RSSI1 %d,FAGC_LPF_GAIN1 %d,\
FAGC_LNA_GAIN0 %d,FAGC_LNA_GAIN1 %d,cagc_state %d,\
FOE %d, LTF_PROC_TIME %d,LTF_SNR0 %d,\
NF0 %d,NF1 %d,WB_RSSI1 %d\r\n",
							rxv.vector1.RXV1.txrate,rxv.vector1.RXV1.HT_stbc,rxv.vector1.RXV1.HT_adcode,
							rxv.vector1.RXV1.HT_extitf,rxv.vector1.RXV1.txmode,rxv.vector1.RXV1.frmode,
							rxv.vector1.RXV1.VHTA1_B22,rxv.vector1.RXV1.HT_aggregation,rxv.vector1.RXV1.HT_shortgi,
							rxv.vector1.RXV1.HT_smooth,rxv.vector1.RXV1.HT_no_sound,rxv.vector1.RXV1.VHTA2_B8_B1,
							rxv.vector1.RXV1.VHTA1_B5_B4/*first cycle end*/,
							rxv.vector1.RXV2.Length,rxv.vector1.RXV2.VHTA1_B16_B6,
							rxv.vector1.RXV3.VHTA1_B21_B17,rxv.vector1.RXV3.OFDM_FreqTrans_DET,rxv.vector1.RXV3.ACI_DETx,
							rxv.vector1.RXV3.SEL_ANT,rxv.vector1.RXV3.RCPI0,rxv.vector1.RXV3.FAGC0_EQ_CAL,
							rxv.vector1.RXV3.FGAC0_CAL_GAIN, rxv.vector1.RXV3.RCPI1, rxv.vector1.RXV3.FAGC1_EQ_CAL,
							rxv.vector1.RXV3.FGAC1_CAL_GAIN,
							rxv.vector1.RXV4.IB_RSSIx, rxv.vector1.RXV4.WB_RSSIx ,rxv.vector1.RXV4.FAGC_LPF_GAINx,
							rxv.vector1.RXV4.IB_RSSI1 ,rxv.vector1.RXV4.FAGC_LPF_GAIN1,
							rxv.vector1.RXV5.FAGC_LNA_GAIN0 , rxv.vector1.RXV5.FAGC_LNA_GAIN1, rxv.vector1.RXV5.cagc_state,
							rxv.vector1.RXV5.FOE , rxv.vector1.RXV5.LTF_PROC_TIME , rxv.vector1.RXV5.LTF_SNR0,
							rxv.vector1.RXV6.NF0 ,rxv.vector1.RXV6.NF1 ,rxv.vector1.RXV6.WB_RSSI1);

						DBGPRINT(RT_DEBUG_TRACE,("%s",rxvPrintBuffer));
						RtlZeroMemory(rxvPrintBuffer, sizeof(rxvPrintBuffer));
						RtlStringCbPrintfA(rxvPrintBuffer, sizeof(rxvPrintBuffer),
							"RXValidIndicator %d,NSTSFiled %d,cagc_state0 %d,\
Reserved %d,\
Prim_ITFR_ENV %d,Sec_ITFR_EVN %d,Sec40_ITFR_EVN %d,\
BT_EVN %d,OFDM_BPSK_LQ %d,Reserved %d,\
Reserved1 %d,Capacity_LQ %d,BTD_NOTCH_LOC %d,\
OFDM_LTF_SNR1 %d,Reserved2 %d,DYN_BANDWIDTH_IN_NOT_HT %d,\
CH_BANDWIDTH_IN_NOT_HT %d,\
3th cycle Reserved2 %d,OFDM_CE_RMSD_Id %d,OFDM_CE_GIC_ENB %d,\
OFDM_CE_LTF_COMB %d,OFDM_DEW_MODE_DET %d,FCS_ERR %d\r\n",
							rxv.vector1.RXV6.RXValidIndicator ,rxv.vector1.RXV6.NSTSFiled ,rxv.vector1.RXV6.cagc_state0,
							rxv.vector1.RXV6.Reserved,
							rxv.vector2.FirstCycle.Prim_ITFR_ENV,rxv.vector2.FirstCycle.Sec_ITFR_EVN,rxv.vector2.FirstCycle.Sec40_ITFR_EVN,
							rxv.vector2.FirstCycle.BT_EVN ,rxv.vector2.FirstCycle.OFDM_BPSK_LQ ,rxv.vector2.FirstCycle.Reserved,
							rxv.vector2.SecondCycle.Reserved1 ,rxv.vector2.SecondCycle.Capacity_LQ ,rxv.vector2.SecondCycle.BTD_NOTCH_LOC,
							rxv.vector2.SecondCycle.OFDM_LTF_SNR1 , rxv.vector2.SecondCycle.Reserved2 , rxv.vector2.SecondCycle.DYN_BANDWIDTH_IN_NOT_HT,
							rxv.vector2.SecondCycle.CH_BANDWIDTH_IN_NOT_HT,
							rxv.vector2.ThreeCycle.Reserved2,rxv.vector2.ThreeCycle.OFDM_CE_RMSD_Id ,rxv.vector2.ThreeCycle.OFDM_CE_GIC_ENB,
							rxv.vector2.ThreeCycle.OFDM_CE_LTF_COMB, rxv.vector2.ThreeCycle.OFDM_DEW_MODE_DET, rxv.vector2.ThreeCycle.FCS_ERR);

						DBGPRINT(RT_DEBUG_TRACE,("%s",rxvPrintBuffer));

						pAdapter->OtherCounters.SNR0 = rxv.vector1.RXV5.LTF_SNR0;
						pAdapter->OtherCounters.SNR1 = rxv.vector2.SecondCycle.OFDM_LTF_SNR1;
						pAdapter->OtherCounters.RCPI0 = rxv.vector1.RXV3.RCPI0;
						pAdapter->OtherCounters.RCPI1 = rxv.vector1.RXV3.RCPI1;

						if(1==rxv.vector1.RXV1.txmode)
						{//OFDM
							pAdapter->OtherCounters.FreqOffstFromRX = rxv.vector1.RXV5.FOE;
						}
						else if(0==rxv.vector1.RXV1.txmode)
						{//CCK 
							RtlCopyMemory(&TmpValue,&rxv.vector1.RXV5,	sizeof(rxv.vector1.RXV5));
							pAdapter->OtherCounters.FreqOffstFromRX =((TmpValue&=0x0001FF80)>>7);
						}
							
						if(i<Temp-1)
							pData+=sizeof(rxv);

		}
	}
	else if (RX_PKT_TYPE_RX_TXS==RxInfoDW0.PKT_TYPE)
	{
		memcpy(&rxv_header,pData,sizeof(rxv_header));
		DBGPRINT(RT_DEBUG_TRACE,("RXV_CNT = %d\n",rxv_header.RXV_CNT));
		Temp = rxv_header.RXV_CNT;
		pData+=sizeof(rxv_header);
		RtlZeroMemory(rxvPrintBuffer, sizeof(rxvPrintBuffer));
		for(i=0;i<Temp;i++)
		{
			memcpy(&tx_status,pData,sizeof(tx_status));
			RtlStringCbPrintfA(rxvPrintBuffer, sizeof(rxvPrintBuffer), 
							"TXSFM %d, TXPower %d,TSSI %d\r\n",
							tx_status.DW0.TXSFM,tx_status.DW2.TXPower,tx_status.DW4.TSSI);

			DBGPRINT(RT_DEBUG_TRACE,("%s",rxvPrintBuffer));
			pAdapter->OtherCounters.TXPower = tx_status.DW2.TXPower;
			pAdapter->OtherCounters.TSSI = tx_status.DW4.TSSI;			
		}

	}
	else if (RX_PKT_TYPE_RX_TMR==RxInfoDW0.PKT_TYPE)
	{	//7603 RX format 3.1 Timing Measurement Report Frame Format
		ULONG *pValue = (ULONG*)pData;//DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) DW %d 0x%08X\n",j, *(ptr+j)));
		ULONG IorR = *pValue;
		int i;
		
		for(i=0;i<=6;i++)
		{
			DBGPRINT(RT_DEBUG_TRACE,("TMR DW %d 0x%08X\n",i, *(pValue+i)));
		}			

		DBGPRINT(RT_DEBUG_TRACE, ("TMR Report: ir = %d\n", IorR & (0x1<<28)));
		DBGPRINT(RT_DEBUG_TRACE, ("DWORD_0: ByteCnt=%d, NC=%d, TMF=%d, toa_vld=%d, tod_vld=%d\n",
                            					(*pValue) & 0x0000FFFF,  //[0:15]
                            					((*pValue) & 0x00040000) >> 18, //[18]
                            					((*pValue) & 0x00080000) >> 19,//[19]
                            					((*pValue) & 0x04000000) >> 26,//[26]
                            					((*pValue) & 0x08000000) >> 27));//[27]
		if( IorR & (0x1<<28))
		{
			DBGPRINT(RT_DEBUG_TRACE,("%s Get TMR Responder\n",__FUNCTION__));

			DBGPRINT(RT_DEBUG_TRACE, ("TYPE=%x,SUB_TYPE=%x\n",
									((*pValue) & 0x00F00000) >> 20,
									((*pValue) & 0x03000000) >> 24));
			DBGPRINT(RT_DEBUG_TRACE, ("DWORD_2: TA[0:15]=%x, SN=%x\n", 
									(*(pValue+2)) & 0x0000FFFF,
									(*(pValue+2)) >>16));
			DBGPRINT(RT_DEBUG_TRACE, ("DWORD_3: TA[16:47]=%x\n", *(pValue+3)));
		}
		DBGPRINT(RT_DEBUG_TRACE, ("DWORD_4: TOD[0:31]=0x%x\n", *(pValue+4)));
		DBGPRINT(RT_DEBUG_TRACE, ("DWORD_6: TOD[32:47]=0x%x\n", (*(pValue+6)) & 0x0000FFFF));

		DBGPRINT(RT_DEBUG_TRACE, ("DWORD_5: TOA[0:31]=0x%x\n", *(pValue+5)));
		DBGPRINT(RT_DEBUG_TRACE, ("DWORD_6: TOA[32:47]=0x%x\n", (*(pValue+6)) >>16));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("Unknow Type, warning\n",__FUNCTION__));
	}


	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<\n",__FUNCTION__));
}
//pData id rx packet head.
VOID CheckSecurityResult(
	IN PULONG   pData,
	IN	RTMP_ADAPTER *pAdapter
	)
{
	RX_INFO_DW0 RxInfoDW0;
	DBGPRINT(RT_DEBUG_TRACE,("%s --->>\n",__FUNCTION__));
	
	memcpy(&RxInfoDW0,pData,sizeof(RX_INFO_DW0));
	if(RxInfoDW0.RxByteCount<=12)
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s, RxByteCount = %d, <12, return !! \n",__FUNCTION__,RxInfoDW0.RxByteCount));
		return;
	}	

	if(RxInfoDW0.PKT_TYPE==RX_PKT_TYPE_RX_NORMAL || RxInfoDW0.PKT_TYPE==RX_PKT_TYPE_RX_DUP_FRB)
	{
		
		if(*(pData+2) & 0x007C00FF)
		{
			DBGPRINT(RT_DEBUG_TRACE,("SecurityFail\n"));
			if(*(pData+7) & 0x00000000)
			{//for SLT security, only set source mac 00:00:00:00:00:00
				DBGPRINT(RT_DEBUG_TRACE,("Set SecurityFail on\n"));
				pAdapter->SecurityFail = TRUE;
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE,("from %08X ,%08X\n",*(pData+6),*(pData+7)));//print source MAC address
				DBGPRINT(RT_DEBUG_TRACE,("Don't set SecurityFail\n"));
			}
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("%s, RxInfoDW0.PKT_TYPE = %d,  don't check SecurityFail !! \n",__FUNCTION__,RxInfoDW0.PKT_TYPE));
	}

	
						


	DBGPRINT(RT_DEBUG_TRACE,("%s ---<<\n",__FUNCTION__));
}

