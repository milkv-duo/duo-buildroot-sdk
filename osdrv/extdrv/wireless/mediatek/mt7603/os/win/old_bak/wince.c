/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2005, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	wince.c

	Abstract:
	Routine for WinCE associated 

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
	Rory Chen   04-14-2005    created
*/

#include "rt_config.h"

#ifdef UNDER_CE

#if DBG
DBGPARAM dpCurSettings = {
    TEXT("RT61"), {
    TEXT("Errors"),TEXT("Warnings"),TEXT("Functions"),TEXT("Init"),
    TEXT("Interrupts"),TEXT("Receives"),TEXT("Transmits"),TEXT("Link"),
    TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),
    TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined") },
	ZONE_INIT_MASK | ZONE_ERROR_MASK
	};
#endif

/*
	========================================================================
	Routine Description:
		Entry point of Dynamic link library 

	Arguments:
		hDLL			Pointer to DLL
		dwReason		Specifies a flag indicating why the DLL entry-point function is being called
		lpReserved		Reserved

	Return Value:
		NTSTATUS

	Note:

	========================================================================
*/
BOOL __stdcall	DllEntry (
	HANDLE	hDLL,
	DWORD	dwReason,
	LPVOID	lpReserved)
{
	DBGPRINT(RT_DEBUG_TRACE, ("=====> RT61::DllEntry\n"));

    switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			DEBUGREGISTER (hDLL);
			DBGPRINT(RT_DEBUG_TRACE, ("DLL_PROCESS_ATTACH\n"));
			DisableThreadLibraryCalls((HMODULE) hDLL);
			break;

		case DLL_PROCESS_DETACH:
			DBGPRINT(RT_DEBUG_TRACE, ("DLL_PROCESS_DETACH\n"));
			break;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<===== RT61::DllEntry\n"));

	return (TRUE);
}

/*
	========================================================================
	Routine Description:
		Install an ISR to handle a particular interrupt.

	Arguments:
		pAd								Pointer to our adapter
		WrapperConfigurationContext		For use by NdisOpenConfiguration

	Return Value:
		NTSTATUS

	Note:

	========================================================================
*/
NDIS_STATUS LoadISR(
	IN PRTMP_ADAPTER   pAd,
	IN NDIS_HANDLE     WrapperConfigurationContext)
{
	NDIS_STATUS						Status = NDIS_STATUS_SUCCESS;
	DWORD							dwBusNumber;
	GIISR_INFO						Info;
	PHYSICAL_ADDRESS				physBusAddress;
	NDIS_HANDLE						ConfigHandle;
	NDIS_STRING						BusNumberStr = NDIS_STRING_CONST("BusNumber");
	NDIS_STRING						SysintrStr   = NDIS_STRING_CONST("Sysintr");
	PNDIS_CONFIGURATION_PARAMETER	ReturnedValue;

	Info.CheckPort	= TRUE;
	Info.PortIsIO	= FALSE;	/* MemoryMap space */
	Info.UseMaskReg	= FALSE;
	Info.PortSize	= sizeof(DWORD);
	Info.Mask		= 0xFFFFFFFFF;

	NdisOpenConfiguration(&Status, &ConfigHandle, WrapperConfigurationContext);

	if (Status != NDIS_STATUS_SUCCESS)
	{
		return Status;
	}

	/* Read PCI bus number from registry */
	NdisReadConfiguration(
		&Status,
		&ReturnedValue,
		ConfigHandle,
		&BusNumberStr,
		NdisParameterHexInteger);

	dwBusNumber = (DWORD)ReturnedValue->ParameterData.IntegerData;

	/* Read PCI sysintr from registry */
	NdisReadConfiguration(
		&Status,
		&ReturnedValue,
		ConfigHandle,
		&SysintrStr,
		NdisParameterHexInteger);

	Info.SysIntr = (DWORD)ReturnedValue->ParameterData.IntegerData;

	if (ConfigHandle)
	{
		NdisCloseConfiguration(ConfigHandle);
	}

	/* Be called by a driver to install an ISR to handle a particular interrupt */
	pAd->hGiISR = LoadIntChainHandler(TEXT("giisr.dll"), TEXT("ISRHandler"), (BYTE) (pAd->InterruptLevel));
	if (pAd->hGiISR == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("LoadIntChainHandler failed\n"));
		Status = NDIS_STATUS_FAILURE;
	}
	else
	{
		physBusAddress.HighPart = 0;
		physBusAddress.LowPart = pAd->MemPhysAddress.LowPart + 0x1C;
		
		/* Translates a bus-relative address to a system or logical address, */
		/* then creates a static, process independent, virtual address mapping for that location */
		if (!TransBusAddrToStatic(PCIBus, dwBusNumber, physBusAddress, Info.PortSize, &Info.PortIsIO, (PPVOID)&Info.PortAddr))
		{		
			DBGPRINT(RT_DEBUG_ERROR, ("TransBusAddrToStatic failed\n"));
			Status = NDIS_STATUS_FAILURE;
		}

		/* Be called from a driver to communicate with an interrupt handler(GiISR). */
		if (!KernelLibIoControl(pAd->hGiISR, IOCTL_GIISR_INFO, &Info, sizeof(Info), NULL, 0, NULL))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("KernelLibIoControl failed\n"));
			Status = NDIS_STATUS_FAILURE;
		}
	}

    return Status;
}

/*
	========================================================================
	Routine Description:
		Unload an existing interrupt handler

	Arguments:
		pAd		Pointer to our adapter

	Return Value:
		NTSTATUS

	Note:

	========================================================================
*/
NDIS_STATUS UnloadISR(
	IN PRTMP_ADAPTER   pAd)
{
    /* */
    /*  If the ISR has been loaded, unload it. */
    /* */
    if (pAd->hGiISR)
    {
        FreeIntChainHandler(pAd->hGiISR);
        pAd->hGiISR = NULL;
    }

    return NDIS_STATUS_SUCCESS;
}

/*
	========================================================================
	
	Routine Description:
		Implement this function to replace "RtlCopyUnicodeString" routine 
		in Ndis driver

	Arguments:
		DestinationString - Points to the destination string buffer. 
		SourceString 	  - Points to the source string buffer.
				
	Return Value:
		PUNICODE_STRING  DestinationString

	Note:
		It can not use RtlCopyUnicodeString in NDIS driver in WinCE
		
	========================================================================
*/
VOID	RTMPCopyUnicodeString(
	IN OUT PUNICODE_STRING	DestinationString,
	IN PUNICODE_STRING		SourceString)
{
	UINT	Index;

	if (SourceString->Length <= 0)
		return;

	if (SourceString->Length > 1)
	{
		for( Index = 0; (Index < SourceString->Length/sizeof(WCHAR)) && (Index < DestinationString->MaximumLength/sizeof(WCHAR)); Index++)
		{
			DestinationString->Buffer[Index] = SourceString->Buffer[Index];
		}
	}
	else
	{
		DestinationString->Buffer[0] = (WCHAR) SourceString->Buffer[0];
		Index = 1;
	}

	DestinationString->Length = Index * sizeof(WCHAR);

    /* NULL terminate the string if there's room. */
    if (DestinationString->Length <= (DestinationString->MaximumLength - sizeof(WCHAR)))
    {
        DestinationString->Buffer[Index] = 0;
    }
}

#endif /* UNDER_CE */
