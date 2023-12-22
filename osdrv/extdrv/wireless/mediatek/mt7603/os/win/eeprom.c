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
 	Eeprom.c
 
	Abstract:
    Central header file to maintain all include files for all NDIS
    miniport driver routines.

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/

#include "config.h"


VOID RaiseClock(
	IN RTMP_ADAPTER *dx,
	IN OUT ULONG		*x)
{	
	*x = *x | EESK;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), *x);
	KeStallExecutionProcessor(1);
}


VOID LowerClock(
	IN RTMP_ADAPTER *dx,
	IN OUT ULONG		*x)
{	
	*x = *x & ~EESK;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), *x);
	KeStallExecutionProcessor(1);
}


USHORT ShiftInBits(
	IN RTMP_ADAPTER *dx)
{
	ULONG		x = 0xFFFFFFFF, i = 0;
	USHORT		d=0;
	
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~( EEDO | EEDI);

	for(i=0; i<16; i++)
	{
		d = d << 1;
		LowerClock(dx, &x);
		RaiseClock(dx, &x);	
		LowerClock(dx, &x);

		//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));

		x &= ~(EEDI);
		if(x & EEDO)
			d |= 1;
	}

	return d;
}

VOID ShiftOutBits(
	IN RTMP_ADAPTER *dx,
	IN USHORT		data,
	IN USHORT		count)
{
	ULONG		x = 0xFFFFFFF, mask;

	mask = 0x01 << (count - 1);
	
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EEDO | EEDI);

	do
	{
		x &= ~EEDI;
		if(data & mask)		x |= EEDI;

		//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

		LowerClock(dx, &x);
		RaiseClock(dx, &x);
		LowerClock(dx, &x);

		mask = mask >> 1;
	} while(mask);

	x &= ~EEDI;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);
}

USHORT ReadEEprom(
	IN RTMP_ADAPTER *dx,
	IN USHORT		Reg,
	IN USHORT		AddressSize)
{
	ULONG		x = 0xFFFFFFFF;
	ULONG		Addr;
	USHORT		data;

	// reset bits and set EECS
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EEDI | EEDO | EESK);
	x |= EECS;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

	// output the read_opcode and register number in that order    
	ShiftOutBits(dx, EEPROM_READ_OPCODE, 3);
	ShiftOutBits(dx, Reg, AddressSize);	

	// Now read the data (16 bits) in from the selected EEPROM word
	data = ShiftInBits(dx);

	EEpromCleanup(dx);

	return data;
}	//ReadEEprom


VOID WriteEEprom(
	IN RTMP_ADAPTER *dx,
	IN USHORT		Reg,
	IN USHORT		AddressSize,
	IN USHORT		Data)
{
	ULONG		x = 0xFFFFFFFF;
	USHORT		d=0;

	EWEN(dx);

	// reset bits and set EECS
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EEDI | EEDO | EESK);
	x |= EECS;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

	// output the read_opcode ,register number and data in that order    
	ShiftOutBits(dx, EEPROM_WRITE_OPCODE, 3);
	ShiftOutBits(dx, Reg, AddressSize);
	ShiftOutBits(dx, Data, 16);

	EEpromCleanup(dx);

	EWDS(dx);

	RTMPusecDelay(5000);
}	//WriteEEprom


USHORT GetEEpromSize(
	IN RTMP_ADAPTER *dx)
{
	ULONG		x = 0xFFFFFFFF, data;
	USHORT		size = 1;
	
	// select EEPROM, reset bits, set EECS
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EEDI | EEDO | EESK);
	x |= EECS;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

	// write the read opcode
	ShiftOutBits(dx, EEPROM_READ_OPCODE, 3);

	// experiment to discover the size of the eeprom.  request register zero
	// and wait for the eeprom to tell us it has accepted the entire address.
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));

	do
	{
		size *= 2;          // each bit of address doubles eeprom size
		x |= EEDO;          // set bit to detect "dummy zero"
		x &= ~EEDI;         // address consists of all zeros

		//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

		LowerClock(dx, &x);
		RaiseClock(dx, &x);
		LowerClock(dx, &x);

		// check for "dummy zero"
		//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
		if (size > EEPROM_MAX_SIZE)
		{
			size = 0;
			break;
		}
	}
	while (x & EEDO);

	// Now read the data (16 bits) in from the selected EEPROM word
	data = ShiftInBits(dx);

	EEpromCleanup(dx);

	return size;
}

VOID EEpromCleanup(
	IN RTMP_ADAPTER *dx)
{
	ULONG x = 0xFFFFFFFF;
	
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EECS | EEDI);
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

	LowerClock(dx, &x);	
	RaiseClock(dx, &x);
	LowerClock(dx, &x);	
}

VOID EWEN(
	IN RTMP_ADAPTER *dx)
{
	ULONG	x = 0xFFFFFFFF;
	USHORT	data;
	
	// reset bits and set EECS
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EEDI | EEDO | EESK);
	x |= EECS;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

	// kick a pulse
	LowerClock(dx, &x);
	RaiseClock(dx, &x);
	LowerClock(dx, &x);

	// output the read_opcode and six pulse in that order    
	ShiftOutBits(dx, EEPROM_EWEN_OPCODE, 5);
	ShiftOutBits(dx, 0, 8);

	EEpromCleanup(dx);
}

VOID EWDS(
	IN RTMP_ADAPTER *dx)
{
	ULONG	x = 0xFFFFFFFF;
	USHORT	data;
	
	// reset bits and set EECS
	//x = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));
	x &= ~(EEDI | EEDO | EESK);
	x |= EECS;
	//WRITE_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4), x);

	// kick a pulse
	LowerClock(dx, &x);
	RaiseClock(dx, &x);
	LowerClock(dx, &x);

	// output the read_opcode and six pulse in that order    
	ShiftOutBits(dx, EEPROM_EWDS_OPCODE, 5);
	ShiftOutBits(dx, 0, 8);

	EEpromCleanup(dx);
}


#ifdef RTMP_PCI_SUPPORT
void Eeprom_Init( 
	IN RTMP_ADAPTER *dx)
{
#if 0
	ULONG	data = 0;
	ULONG	EepromType=0;
	
	data = READ_Real_REGISTER_ULONG(((PULONG)dx->virtualaddr + RA_EE_CTRL/4));

	EepromType = (data & 0x30) >> 4;

//	if (IS_RT3290(dx->MACVersion))
		EepromType = 1; //RT3290 only use 93c66 ; MT7650	
	
	switch (EepromType)
	{
		case 0:
			dx->ADDRESSSIZE = 6; // 00=>Tyep:93c46, Size : 1Kbit = 2^6 * 16 bits
			break;

		case 1:
			dx->ADDRESSSIZE = 8; // 01=>Tyep:93c66, Size : 4Kbit = 2^8 * 16 bits
			break;

		case 2:		 	
			dx->ADDRESSSIZE = 10; // 10=>Tyep:93c86, Size : 16Kbit = 2^10 * 16 bits
			break;

		case 3:
			break;
	}	
#endif
	dx->ADDRESSSIZE = 8; // 01=>Tyep:93c66, Size : 4Kbit = 2^8 * 16 bits
}


NTSTATUS
BufferReadEEPROM(
	IN PDEVICE_OBJECT	pDevObj,
	IN PULONG			pIBuffer,
	IN PUCHAR			UserBuffer,
	IN ULONG				BufferMaxSize,
	OUT PULONG			ByteCount
    )
{
	NTSTATUS					ntStatus = STATUS_SUCCESS;
	ULONG					i, InBuf[5];
	USHORT					data=0;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) pDevObj->DeviceExtension;

	InBuf[0] = *(PULONG)(pIBuffer);				//offset
	InBuf[1] = *(pIBuffer+1);					//length
	InBuf[2] = *(pIBuffer+2);					//value

	*ByteCount = InBuf[1];

	for(i=0; i < InBuf[1]/2; i++)
	{
		data = ReadEEprom(pDevExt, (USHORT) (InBuf[0] + i), pDevExt->ADDRESSSIZE);

		RtlCopyMemory(UserBuffer + i*2, &data, sizeof(USHORT));
	}

	return ntStatus;
}   // BufferReadEEPROM


NTSTATUS
BufferWriteEEPROM(
	IN PDEVICE_OBJECT	pDevObj,
	IN PUCHAR			pIBuffer,
	IN PUCHAR			UserBuffer,
	IN ULONG				BufferMaxSize,
	OUT PULONG			ByteCount
    )
{
	NTSTATUS		ntStatus = STATUS_SUCCESS;
	ULONG		j, InBuf[5];
	USHORT		AddressSize, Data;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDevObj->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: BufferWriteEEPROM ===> \n"));

	InBuf[0] = *(PULONG)(pIBuffer+0);						//offset
	InBuf[1] = *(PULONG)(pIBuffer+4);						//length
	InBuf[2] = *(PULONG)(pIBuffer+8);						//value	

	AddressSize = pDevExt->ADDRESSSIZE;

	for(j = 0; j < InBuf[1]/2; j++)
	{
		Data = *(PUSHORT)(pIBuffer + 20 + j*2);

		WriteEEprom(pDevExt, (USHORT)(j+InBuf[0]), AddressSize, Data);
	}

	*ByteCount = 0;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: BufferWriteEEPROM <=== \n"));

	return ntStatus;
}   // BufferWriteEEPROM
#endif /* RTMP_PCI_SUPPORT */

