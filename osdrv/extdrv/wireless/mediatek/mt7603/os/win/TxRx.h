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
 	TxRx.h
 
	Abstract:
    TxRx function and definition header file

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   11-19-2002    created

*/

#ifndef	__TxRx_H__
#define	__TxRx_H__

#ifdef	SNIFF
#define RX_ACTION_SNIFF_START		4
#define RX_ACTION_SNIFF_STOP		5
#endif

#define HW_BEACON_BASE0              0x7800
#define DefaultBeaconLength               59 // 59 bytes

#define PAYLOAD_RANDOM_OFFSET 20

NTSTATUS
RTPCIStartRX(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	
	);

NTSTATUS
RTPCIStartTX(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	
	);

VOID	NICIssueReset(
	IN PVOID pContext
	);

#ifdef	SNIFF
NTSTATUS
RTPCIStartSniffer(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	
	);
#endif

NTSTATUS
EnableBeacon(
    IN	PDEVICE_OBJECT	pDO,
    IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	
    );

NTSTATUS
DisableBeacon(
    IN	PDEVICE_OBJECT	pDO,   
    OUT PULONG			ByteCount	
    );
    
#endif