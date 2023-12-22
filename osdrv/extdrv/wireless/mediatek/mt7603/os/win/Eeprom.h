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

#ifndef	__Eeprom_H__
#define	__Eeprom_H__

#define EEPROM_MAX_SIZE			512

//-------------------------------------------------------------------------
// EEPROM bit definitions
//-------------------------------------------------------------------------
// EEPROM control register bits
#if 0
const USHORT	EERL	= 0x80;	   // Reload EEPROM content
const USHORT	EEDO	= 0x08;    // EEPROM DO
const USHORT	EEDI      = 0x04;    // EEPROM DI
const USHORT	EECS      = 0x02;    // EEPROM CS
const USHORT	EESK      = 0x01;    // EEPROM SK


// EEPROM OP Code
const USHORT	EEPROM_WRITE_OPCODE         = 05;
const USHORT	EEPROM_READ_OPCODE          = 06;
const USHORT	EEPROM_ERASE_OPCODE         = 07;	   // Erase
const USHORT	EEPROM_EWDS_OPCODE          = 16;      // Erase/write disable
const USHORT	EEPROM_EWEN_OPCODE          = 19;      // Erase/write enable
const USHORT	EEPROM_ERAL_OPCODE          = 18;      // Erase/write enable
#else
#define EERL	0x80	   // Reload EEPROM content
#define EEDO	0x08    // EEPROM DO
#define EEDI	0x04    // EEPROM DI
#define EECS	0x02    // EEPROM CS
#define EESK	0x01    // EEPROM SK


// EEPROM OP Code
#define EEPROM_WRITE_OPCODE	05
#define EEPROM_READ_OPCODE	06
#define EEPROM_ERASE_OPCODE	07	   // Erase
#define EEPROM_EWDS_OPCODE	16      // Erase/write disable
#define EEPROM_EWEN_OPCODE	19      // Erase/write enable
#define EEPROM_ERAL_OPCODE	18      // Erase/write enable
#endif


VOID RaiseClock(  IN RTMP_ADAPTER *dx, IN OUT ULONG *x );

VOID LowerClock( IN RTMP_ADAPTER *dx, IN OUT ULONG *x );

VOID EEpromCleanup( IN RTMP_ADAPTER *dx );

USHORT ShiftInBits( IN RTMP_ADAPTER *dx );

VOID ShiftOutBits( IN RTMP_ADAPTER *dx, IN USHORT data, IN USHORT count );

USHORT ReadEEprom( IN RTMP_ADAPTER *dx, IN USHORT Reg, IN USHORT AddressSize );

USHORT GetEEpromSize( IN RTMP_ADAPTER *dx );

VOID WriteEEprom( IN RTMP_ADAPTER *dx, IN USHORT Reg, IN USHORT AddressSize, IN USHORT Data );

VOID EWEN( IN RTMP_ADAPTER *dx );

VOID EWDS( IN RTMP_ADAPTER *dx );

void Eeprom_Init( IN RTMP_ADAPTER *dx );

NTSTATUS
BufferReadEEPROM(
	IN PDEVICE_OBJECT	pDevObj,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    );

NTSTATUS
BufferWriteEEPROM(
	IN PDEVICE_OBJECT	pDevObj,
	IN PUCHAR pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    );

#endif	// __Eeprom_H__
