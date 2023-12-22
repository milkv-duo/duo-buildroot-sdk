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
 	Eeprom.h
 
	Abstract:
    Eeprom function and definition header file

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/

#ifndef	__ComPort_H__
#define	__ComPort_H__


VOID 
ComPortInit(
	IN	PDEVICE_OBJECT	pDO);

NTSTATUS
RTPCIComWrite(
	IN	PDEVICE_OBJECT	pDO,
	IN	PUCHAR			pIBuffer,
	IN	PUCHAR			UserBuffer,
	IN	ULONG			BufferMaxSize,
	OUT PULONG			ByteCount	
	);

NTSTATUS
RTPCIComRead(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
	IN	PUCHAR			UserBuffer,
	IN	ULONG			BufferMaxSize,
	OUT PULONG			ByteCount	
	);

VOID
RTPCIHandleUartTxThDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTPCIHandleUartRxThDoneInterrupt(
	IN PVOID pContext 
	);

VOID
RTPCIHandleUartIdleThDoneInterrupt(
	IN PVOID pContext 
	);
#endif