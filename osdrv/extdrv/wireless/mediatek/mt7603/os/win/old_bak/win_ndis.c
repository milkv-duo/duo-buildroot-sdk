/*
  the following section should be defined in rt_winndis.h
  temporary put it here.

*/

#include "rt_config.h"

/*typedef	NDIS_MINIPORT_TIMER	RTMP_OS_TIMER; */

#define RTMP_INC_REF(_A)            NdisInterlockedIncrement(&(_A)->RefCount)
#define RTMP_DEC_REF(_A)            NdisInterlockedDecrement(&(_A)->RefCount); ASSERT(_A->RefCount >= 0)
#define RTMP_GET_REF(_A)            ((_A)->RefCount)

#define RTMP_INC_RCV_REF(_A)        ((_A)->RcvRefCount++)
#define RTMP_DEC_RCV_REF(_A)        ((_A)->RcvRefCount--)
#define RTMP_GET_RCV_REF(_A)        ((_A)->RcvRefCount)

/*#define RTMP_INC_SEND_REF(_A)       ((_A)->SendRefCount++) */
/*#define RTMP_DEC_SEND_REF(_A)       ((_A)->SendRefCount--) */
/*#define RTMP_GET_SEND_REF(_A)       ((_A)->SendRefCount) */

#define RTMP_OFFSET(field)          ((UINT)FIELD_OFFSET(RTMP_ADAPTER, field))
#define RTMP_SIZE(field)            sizeof(((PRTMP_ADAPTER)0)->field)

#define COMMON_CFG_OFFSET(field)    ((UINT)FIELD_OFFSET(RTMP_ADAPTER, CommonCfg) + (UINT)FIELD_OFFSET(COMMON_CONFIG, field))
#define STA_CFG_OFFSET(field)       ((UINT)FIELD_OFFSET(RTMP_ADAPTER, StaCfg) + (UINT)FIELD_OFFSET(STA_ADMIN_CONFIG, field))
#define AP_CFG_OFFSET(field)        ((UINT)FIELD_OFFSET(RTMP_ADAPTER, ApCfg) + (UINT)FIELD_OFFSET(AP_ADMIN_CONFIG, field))
#define COMMON_CFG_SIZE(field)      sizeof(((PCOMMON_CONFIG)0)->field)
#define STA_CFG_SIZE(field)         sizeof(((struct _STA_ADMIN_CONFIG *)0)->field)
#define AP_CFG_SIZE(field)          sizeof(((struct _AP_ADMIN_CONFIG *)0)->field)

#define INC_RING_INDEX(_idx, _RingSize)    \
{                                          \
    (_idx)++;                              \
    if ((_idx) >= (_RingSize)) _idx=0;     \
}



/*
  code segment in our souce tree, need to re-locate to proper place once
  our code need to support windows-based OS.
*/

#ifdef WIN_NDIS
#define	STRING_OFFSET(field)			((UINT) FIELD_OFFSET(LOCAL_STRINGS, field))
#define	STRING_SIZE(field)				sizeof(((PLOCAL_STRINGS)0)->field)
#define INIT_NDIS_STR(str, buf, len)	{(str).Length = 0;(str).MaximumLength = len;(str).Buffer = buf;}

/* */
/* Structure to first copy string values into */
/* */
typedef	struct		_LOCAL_STRINGS {
	NDIS_STRING		Ssid;				/* SSID network name */
	WCHAR			SsidBuf[64];
	NDIS_STRING		VendorDesc;			/* SSID network name */
	WCHAR			VendorDescBuf[64];
	NDIS_STRING 	Key0;				/* Key 0 in ascii format entered by user */
	WCHAR			KeyBuf0[64];
	NDIS_STRING 	Key1;				/* Key 1 in ascii format entered by user */
	WCHAR			KeyBuf1[64];
	NDIS_STRING 	Key2;				/* Key 2 in ascii format entered by user */
	WCHAR			KeyBuf2[64];
	NDIS_STRING 	Key3;				/* Key 3 in ascii format entered by user */
	WCHAR			KeyBuf3[64];
	NDIS_STRING 	MAC;				/* Local administration MAC Address */
	WCHAR			LocalMacAddr[64];
}	LOCAL_STRINGS, *PLOCAL_STRINGS;

LOCAL_STRINGS	LocalStrings;

RTMP_REG_ENTRY	NICRegTable[] = {
/*	reg value name						bRequired	Parameter Type			Offset in RTMP_ADAPTER			        Field size						    Default Value	Min			Max */
	{NDIS_STRING_CONST("OpMode"),				0,	NdisParameterInteger,	RTMP_OFFSET(OpMode),					RTMP_SIZE(OpMode),						OPMODE_STA,	0,			1			},
	{NDIS_STRING_CONST("CountryRegion"),		0,	NdisParameterInteger,	COMMON_CFG_OFFSET(CountryRegion),		COMMON_CFG_SIZE(CountryRegion),			0,			0,			7			},
	{NDIS_STRING_CONST("CountryRegionABand"),	0,	NdisParameterInteger,	COMMON_CFG_OFFSET(CountryRegionForABand),COMMON_CFG_SIZE(CountryRegionForABand),7,			0,			8			},
	{NDIS_STRING_CONST("WirelessMode"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(PhyMode),				COMMON_CFG_SIZE(PhyMode),				0,			0,			9			},
	{NDIS_STRING_CONST("Channel"),				0,	NdisParameterInteger,	COMMON_CFG_OFFSET(Channel),				COMMON_CFG_SIZE(Channel),				1,			1,			255			},
	{NDIS_STRING_CONST("RtsThresh"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(RtsThreshold),		COMMON_CFG_SIZE(RtsThreshold),			2347,		0,			2347		},
	{NDIS_STRING_CONST("FragThresh"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(FragmentThreshold),	COMMON_CFG_SIZE(FragmentThreshold),		2346,		256,		2346		},
	{NDIS_STRING_CONST("Encryption"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(WepStatus),			COMMON_CFG_SIZE(WepStatus),				0,			0,			7			},
	{NDIS_STRING_CONST("AuthenType"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(AuthMode), 			COMMON_CFG_SIZE(AuthMode),				0,			0,			6			},
	{NDIS_STRING_CONST("WEPKeyUse"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(DefaultKeyId),		COMMON_CFG_SIZE(DefaultKeyId),			0,			1,			4			},
	{NDIS_STRING_CONST("RDG"),					0,	NdisParameterInteger,	COMMON_CFG_OFFSET(bRdg),				COMMON_CFG_SIZE(bRdg),					1,			0,			1			},
	{NDIS_STRING_CONST("SSID"),					0,	NdisParameterString,	STRING_OFFSET(Ssid),					STRING_SIZE(Ssid),						0,			0,			1			},
	{NDIS_STRING_CONST("VendorDesc"),			0,	NdisParameterString,	STRING_OFFSET(VendorDesc),				STRING_SIZE(VendorDesc),				0,			0,			1			},
	{NDIS_STRING_CONST("WEPKey1"),				0,	NdisParameterString,	STRING_OFFSET(Key0),					STRING_SIZE(Key0),						0,			0,			1			},
	{NDIS_STRING_CONST("WEPKey2"),				0,	NdisParameterString,	STRING_OFFSET(Key1),					STRING_SIZE(Key1),						0,			0,			1			},
	{NDIS_STRING_CONST("WEPKey3"),				0,	NdisParameterString,	STRING_OFFSET(Key2),					STRING_SIZE(Key2),						0,			0,			1			},
	{NDIS_STRING_CONST("WEPKey4"),				0,	NdisParameterString,	STRING_OFFSET(Key3),					STRING_SIZE(Key3),						0,			0,			1			},
	{NDIS_STRING_CONST("NetworkAddress"),		0,	NdisParameterString,	STRING_OFFSET(MAC),						STRING_SIZE(MAC),						0,			0,			1			},

	{NDIS_STRING_CONST("BGProtection"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(UseBGProtection),		COMMON_CFG_SIZE(UseBGProtection),		0,			0,			2			},
	{NDIS_STRING_CONST("ShortSlot"), 			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(bUseShortSlotTime),	COMMON_CFG_SIZE(bUseShortSlotTime),		0,			0,			1			},
	{NDIS_STRING_CONST("FrameAggregation"),		0,	NdisParameterInteger,	COMMON_CFG_OFFSET(bAggregationCapable), COMMON_CFG_SIZE(bAggregationCapable),	1,			0,			1			},
	{NDIS_STRING_CONST("IEEE80211H"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(bIEEE80211H),			COMMON_CFG_SIZE(bIEEE80211H),			0,			0,			1			},
	{NDIS_STRING_CONST("TXBurst"),				0,	NdisParameterInteger,	COMMON_CFG_OFFSET(bEnableTxBurst),		COMMON_CFG_SIZE(bEnableTxBurst),		0,			0,			1			},
	{NDIS_STRING_CONST("WMMCapable"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(bWmmCapable),			COMMON_CFG_SIZE(bWmmCapable),			0,			0,			1			},
	{NDIS_STRING_CONST("BACapability"),			0,	NdisParameterInteger,	COMMON_CFG_OFFSET(BACapability),		COMMON_CFG_SIZE(BACapability),			0x07022040,	2,			0xffffffff	},
	{NDIS_STRING_CONST("TransmitSetting"),		0,	NdisParameterInteger,	COMMON_CFG_OFFSET(RegTransmitSetting),	COMMON_CFG_SIZE(RegTransmitSetting),	0x8219,		2,			0xffffffff	},

	{NDIS_STRING_CONST("Radio"),				0,	NdisParameterInteger,	STA_CFG_OFFSET(bSwRadio),				STA_CFG_SIZE(bSwRadio),					0,			0,			1			},
	{NDIS_STRING_CONST("ShowHiddenSSID"),		0,	NdisParameterInteger,	STA_CFG_OFFSET(bShowHiddenSSID),		STA_CFG_SIZE(bShowHiddenSSID),			0,			0,			1			},
	{NDIS_STRING_CONST("AutoReconnect"),		0,	NdisParameterInteger,	STA_CFG_OFFSET(bAutoReconnect),			STA_CFG_SIZE(bAutoReconnect),			1,			0,			1			},
     	{NDIS_STRING_CONST("NetworkType"),			0,	NdisParameterInteger,	STA_CFG_OFFSET(BssType),				STA_CFG_SIZE(BssType),					0,			0,			1			},
     	{NDIS_STRING_CONST("AdhocOfdm"),			0,	NdisParameterInteger,	STA_CFG_OFFSET(AdhocMode),				STA_CFG_SIZE(AdhocMode), 				0,			0,			2			},
     	{NDIS_STRING_CONST("PSMode"), 				0,	NdisParameterInteger,	STA_CFG_OFFSET(WindowsPowerMode),		STA_CFG_SIZE(WindowsPowerMode),			0,			0,			2			},
     	{NDIS_STRING_CONST("CCX2Control"),			0,	NdisParameterInteger,	STA_CFG_OFFSET(CCX2Control),			STA_CFG_SIZE(CCX2Control),				0,			0,			0xffffffff	},
     	{NDIS_STRING_CONST("NHFactor"), 			0,	NdisParameterInteger,	STA_CFG_OFFSET(NHFactor),				STA_CFG_SIZE(NHFactor),					3,			1,			20			},
	{NDIS_STRING_CONST("CLFactor"), 			0,	NdisParameterInteger,	STA_CFG_OFFSET(CLFactor),				STA_CFG_SIZE(CLFactor),					9,			1,			20			},

	{NDIS_STRING_CONST("AutoChannelSelect"),	0,	NdisParameterInteger,	AP_CFG_OFFSET(bAutoChannelAtBootup),	AP_CFG_SIZE(bAutoChannelAtBootup),		0,			0,			1			},
	{NDIS_STRING_CONST("DtimPeriod"),			0,	NdisParameterInteger,	AP_CFG_OFFSET(DtimPeriod),				AP_CFG_SIZE(DtimPeriod),				1,			1,			5			},

	{NDIS_STRING_CONST("TEST"), 				0,	NdisParameterInteger,	STA_CFG_OFFSET(WhqlTest),			    STA_CFG_SIZE(WhqlTest), 			    0,			0,			1			},		
};

#define	NIC_NUM_REG_PARAMS	(sizeof (NICRegTable) / sizeof(RTMP_REG_ENTRY))
#endif



#ifdef RTMP_MAC_PCI
#ifdef WIN_NDIS
/*
	========================================================================
	
	Routine Description:
		Find the adapter and get all the assigned resources

	Arguments:
		Adapter		Pointer to our adapter

	Return Value:
		NDIS_STATUS_SUCCESS
		NDIS_STATUS_ADAPTER_NOT_FOUND (event is logged as well)

	IRQL = PASSIVE_LEVEL

	Note:
	
	========================================================================
*/
NDIS_STATUS	RTMPFindAdapter(
	IN	PRTMP_ADAPTER	pAd,
	IN	NDIS_HANDLE		WrapperConfigurationContext
	)
{
	NDIS_STATUS			Status = NDIS_STATUS_ADAPTER_NOT_FOUND;
	ULONG				ErrorCode;
	ULONG				ErrorValue;

	ULONG				ulResult;
	UCHAR				buffer[NIC_PCI_HDR_LENGTH ];
	PPCI_COMMON_CONFIG	pPciConfig = (PPCI_COMMON_CONFIG) buffer;

	UCHAR				resBuf[NIC_RESOURCE_BUF_SIZE];
	PNDIS_RESOURCE_LIST	resList = (PNDIS_RESOURCE_LIST)resBuf;
	UINT				bufSize = NIC_RESOURCE_BUF_SIZE;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR	pResDesc;
	ULONG				index;
	BOOLEAN				bResPort = FALSE, bResInterrupt = FALSE, bResMemory = FALSE;

	DBGPRINT(RT_DEBUG_TRACE, ("---> RTMPFindAdapter\n"));

	do
	{
		/* */
		/* Find our adapter - read in the device and vendor IDs */
		/* */
		ulResult = NdisReadPciSlotInformation(
						pAd->AdapterHandle,
						0,          				/* ignored */
						FIELD_OFFSET(PCI_COMMON_CONFIG, VendorID),
						buffer,
						NIC_PCI_HDR_LENGTH );

		if (ulResult != NIC_PCI_HDR_LENGTH )
		{
			DBGPRINT(RT_DEBUG_ERROR,
				("NdisReadPciSlotInformation (PCI_COMMON_CONFIG) ulResult=%d\n", ulResult));

			ErrorCode  = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
			ErrorValue = ERRLOG_READ_PCI_SLOT_FAILED;

			break;
		}

		/* */
		/* Right type of adapter? */
		/* */
		if ((pPciConfig->VendorID != NIC_PCI_VENDOR_ID) ||
			((pPciConfig->DeviceID != NIC2860_PCI_DEVICE_ID) &&
			(pPciConfig->DeviceID != NIC2860_PCIe_DEVICE_ID)))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("VendorID or DeviceID doesn't match - %x/%x\n",
				pPciConfig->VendorID, pPciConfig->DeviceID));

			ErrorCode  = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
			ErrorValue = ERRLOG_VENDOR_DEVICE_NOMATCH;

			break;
		}

		/* save info from config space */
		pAd->RevsionID   = pPciConfig->RevisionID;
		pAd->VendorID    = pPciConfig->VendorID;
		pAd->DeviceID    = pPciConfig->DeviceID;
		pAd->SubVendorID = pPciConfig->u.type0.SubVendorID;
		pAd->SubSystemID = pPciConfig->u.type0.SubSystemID;

		DBGPRINT(RT_DEBUG_TRACE, ("Adapter found - VID=0x%04x, DevID=0x%04x, subVID=0x%04x, subSysID=0x%04x\n",
			pAd->VendorID, pAd->DeviceID, pAd->SubVendorID, pAd->SubSystemID));

		/* */
		/* Adapter is found. Now get the assigned resources */
		/* */
		NdisMQueryAdapterResources(
			&Status,
			WrapperConfigurationContext,
			resList,
			&bufSize);

		if (Status != NDIS_STATUS_SUCCESS)
		{
			ErrorCode = NDIS_ERROR_CODE_RESOURCE_CONFLICT;
			ErrorValue = ERRLOG_QUERY_ADAPTER_RESOURCES;
			break;
		}

		for (index=0; index < resList->Count; index++)
		{
			pResDesc = &resList->PartialDescriptors[index];

			switch(pResDesc->Type)
			{
				case CmResourceTypePort:		/* ResType_IO */
					pAd->IoBaseAddress = NdisGetPhysicalAddressLow(pResDesc->u.Port.Start);
					pAd->IoRange = pResDesc->u.Port.Length;
					bResPort = TRUE;

					DBGPRINT(RT_DEBUG_TRACE, ("IoBaseAddress = 0x%x\n", pAd->IoBaseAddress));
					DBGPRINT(RT_DEBUG_TRACE, ("IoRange = 0x%x\n", pAd->IoRange));
					break;

				case CmResourceTypeInterrupt:	/* ResType_IRQ */
					pAd->InterruptLevel = pResDesc->u.Interrupt.Level;
					bResInterrupt = TRUE;

					DBGPRINT(RT_DEBUG_TRACE, ("InterruptLevel = 0x%x\n", pAd->InterruptLevel));
					break;

				case CmResourceTypeMemory:		/* ResType_Mem */
					pAd->MemPhysAddress = pResDesc->u.Memory.Start;
					pAd->MemRange = pResDesc->u.Memory.Length;
					bResMemory = TRUE;

					DBGPRINT(RT_DEBUG_TRACE,
						("MemPhysAddress(Low) = 0x%0x\n", NdisGetPhysicalAddressLow(pAd->MemPhysAddress)));
					DBGPRINT(RT_DEBUG_TRACE,
						("MemPhysAddress(High) = 0x%0x\n", NdisGetPhysicalAddressHigh(pAd->MemPhysAddress)));
					DBGPRINT(RT_DEBUG_TRACE, ("MemRange = 0x%x\n", pAd->MemRange));
					break;
			}
		}

		if (!bResInterrupt || !bResMemory)
		{
			Status = NDIS_STATUS_RESOURCE_CONFLICT;
			ErrorCode = NDIS_ERROR_CODE_RESOURCE_CONFLICT;

			if (!bResInterrupt)
			{
				ErrorValue = ERRLOG_NO_INTERRUPT_RESOURCE;
			}
			else
			{
				ErrorValue = ERRLOG_NO_MEMORY_RESOURCE;
			}

			break;
		}

		Status = NDIS_STATUS_SUCCESS;
	}	while (FALSE);

	if (Status != NDIS_STATUS_SUCCESS)
	{
		/* Log error information */
		NdisWriteErrorLogEntry(
			pAd->AdapterHandle,
			ErrorCode,
			1,
			ErrorValue);
	}

	DBGPRINT_S(("<--- RTMPFindAdapter, Status=%x\n", Status));

	return Status;
}

/*
	========================================================================
	
	Routine Description:
		Free all the resources and RTMP_ADAPTER data block

	Arguments:
		Adapter			Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:

	========================================================================
*/
VOID	RTMPFreeAdapter(
	IN	PRTMP_ADAPTER	pAd)
{
	INT			index, num;

	DBGPRINT(RT_DEBUG_TRACE, ("--> RTMPFreeAdapter\n"));

	/* */
	/* No active and waiting sends */
	/* Send & Recv already checked in RTMPHalt routine */
	/* */

	/* Make sure no other pending operations */
	ASSERT(RTMP_GET_REF(pAd) == 0);

	/* */
	/* Free hardware resources */
	/* */

	/* Free Memory mapping PCI register & shared memory */
	if (pAd->CSRBaseAddress)
	{
		NdisMUnmapIoSpace(
			pAd->AdapterHandle,
			pAd->CSRBaseAddress,
			pAd->MemRange);
		pAd->CSRBaseAddress = NULL;
	}

	/* Free IO port resource */
	if (pAd->PortOffset)
	{
		NdisMDeregisterIoPortRange(
			pAd->AdapterHandle,
			pAd->IoBaseAddress,
			pAd->IoRange,
			pAd->PortOffset);
		pAd->PortOffset = NULL;
	}

	/* */
	/* Free common buffer share memory */
	/* Everything in reverse order */
	/* */

	/* */
	/* Free RX Ring related space */
	/* */
	for (index = RX_RING_SIZE - 1 ; index >= 0; index--)
	{
		if (pAd->RxRing[0].Cell[index].DmaBuf.AllocVa)
		{
			NdisMFreeSharedMemory(
				pAd->AdapterHandle,
				pAd->RxRing[0].Cell[index].DmaBuf.AllocSize,
				FALSE,
				pAd->RxRing[0].Cell[index].DmaBuf.AllocVa,
				pAd->RxRing[0].Cell[index].DmaBuf.AllocPa);
		}
	}
	NdisZeroMemory(pAd->RxRing[0].Cell, RX_RING_SIZE * sizeof(RTMP_DMACB));

	if (pAd->RxDescRing[0].AllocVa)
	{
		NdisMFreeSharedMemory(
			pAd->AdapterHandle,
			pAd->RxDescRing[0].AllocSize,
			FALSE,
			pAd->RxDescRing[0].AllocVa,
			pAd->RxDescRing[0].AllocPa);
	}
	NdisZeroMemory(&pAd->RxDescRing[0], sizeof(RTMP_DMABUF));

	/* */
	/* Free MGMT Ring related space */
	/* */
	if (pAd->MgmtDescRing.AllocVa)
	{
		NdisMFreeSharedMemory(
			pAd->AdapterHandle,
			pAd->MgmtDescRing.AllocSize,
			FALSE,
			pAd->MgmtDescRing.AllocVa,
			pAd->MgmtDescRing.AllocPa);
	}
	NdisZeroMemory(&pAd->MgmtDescRing, sizeof(RTMP_DMABUF));

	/* */
	/* Free TX Ring buffer */
	/* */
	for (num = 0; num < NUM_OF_TX_RING; num++)
	{
		if (pAd->TxBufSpace[num].AllocVa)
		{
			NdisMFreeSharedMemory(
				pAd->AdapterHandle,
				pAd->TxBufSpace[num].AllocSize,
				FALSE,
				pAd->TxBufSpace[num].AllocVa,
				pAd->TxBufSpace[num].AllocPa);
		}
		NdisZeroMemory(&pAd->TxBufSpace[num], sizeof(RTMP_DMABUF));
		
		if (pAd->TxDescRing[num].AllocVa)
		{
			NdisMFreeSharedMemory(
				pAd->AdapterHandle,
				pAd->TxDescRing[num].AllocSize,
				FALSE,
				pAd->TxDescRing[num].AllocVa,
				pAd->TxDescRing[num].AllocPa);
		}
		NdisZeroMemory(&pAd->TxDescRing[num], sizeof(RTMP_DMABUF));
	}

	/* */
	/* Free preallocated shared memory */
	/* */
	for (index = 0; index < NUM_OF_LOCAL_TXBUF; index++)
	{
		if (! pAd->LocalTxBuf[index].AllocVa)
			break;
		NdisMFreeSharedMemory(
			pAd->AdapterHandle,
			pAd->LocalTxBuf[index].AllocSize,
			TRUE,
			pAd->LocalTxBuf[index].AllocVa,
			pAd->LocalTxBuf[index].AllocPa);
	}


		/* */
		/* Free BA Rx reordering Ring and associated buffer memory */
		/* */
	
	for (num = 0; num < MAX_BARECI_SESSION; num++)
		{
		pAd->LocalRxReorderBuf[num].InUse = FALSE;
		for (index = 0; index < MAX_RX_REORDERBUF; index++)
				{
			pDmaBuf = &pAd->LocalRxReorderBuf[num].MAP_RXBuf[index];
			pDmaBuf->IsFull = FALSE;
				if (pDmaBuf->AllocVa)
				{
					NdisMFreeSharedMemory(
						pAd->AdapterHandle,
						pDmaBuf->AllocSize,
						FALSE,
						pDmaBuf->AllocVa,
						pDmaBuf->AllocPa);
				}
			}
		}
	
#ifndef UNDER_CE
	/* */
	/* Free map registers. This must be after all the shared memory is freed */
	/* */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_MAP_REGISTER))
	{
		NdisMFreeMapRegisters(pAd->AdapterHandle);
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_MAP_REGISTER);
	}
#endif

	for (num = 0; num < NUM_OF_TX_RING; num++)
		NdisFreeSpinLock(&pAd->TxSwQueueLock[num]);

	for (num = 0; num < NUM_OF_RX_RING; num++)
		NdisFreeSpinLock(&pAd->RxRingLock[num]);

	NdisFreeSpinLock(&pAd->MgmtRingLock);
#ifdef WIN_NDIS
	NdisFreeSpinLock(&pAd->TxRingLock);
	NdisFreeSpinLock(&pAd->LocalTxBufQueueLock);
#endif
	NdisFreeSpinLock(&pAd->irq_lock);
#if ME_98	
	NdisFreeSpinLock(&pAd->DispatchLock);
#endif

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
    UAPSD_Release(pAd);
#endif /* UAPSD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	/* free the pre-allocated NDI PACKET/BUFFER poll */
	if (pAd->FreeNdisPacketPoolHandle)
		NdisFreePacketPool(pAd->FreeNdisPacketPoolHandle);

	if (pAd->FreeNdisBufferPoolHandle)
		NdisFreeBufferPool(pAd->FreeNdisBufferPoolHandle);

	NdisFreeMemory(pAd->BeaconBuf, MAX_BEACON_SIZE);
	
	NdisFreeMemory(pAd, sizeof(RTMP_ADAPTER), 0);

	DBGPRINT(RT_DEBUG_TRACE, ("<-- RTMPFreeAdapter\n"));
}
#endif


/* Patch to enable cardbus controller pre-fetch mode */
/* */
/* IRQL = PASSIVE_LEVEL */
VOID RTMPPatchCardBus(
	IN	PRTMP_ADAPTER	pAdapter)
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
	ULONG	CardBus;

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPPatchCardBus-->\n"));
	/* Try to find the Cardbus controller we connected to. */
	/* We should not touch other Cardbus controller which we did not use */
	/* The cardbus controller should have bus number smaller than our device */
	/* We should skip Bus 0 & 1, since it is for PCI & VGA chipset. */
	/* Cardbus controller should start from bus = 2. */
	for (Bus = 0; Bus < MAX_PCI_BUS; Bus++)
	{
		for (Slot = 0; Slot < MAX_PCI_DEVICE; Slot++)
		{
			for (Func = 0; Func < MAX_FUNC_NUM; Func++)
			{
				Vendor    = RTMPReadCBConfig(Bus, Slot, Func, 0x00) & 0x0000FFFF;
				/* Skip non-exist deice right away */
				if (Vendor == UNKNOWN)
					continue;
				
				Device    = RTMPReadCBConfig(Bus, Slot, Func, 0x00) >> 16;
				Interrupt = RTMPReadCBConfig(Bus, Slot, Func, 0x3c) & 0x000000FF;
				Class     = RTMPReadCBConfig(Bus, Slot, Func, 0x08) >> 16;
				
				/* First of all, filter non-cardbus bridge and not matching INT line device */
				/* Ralink ASIC has to be modified for latency timer and cache line size too. */
				if (Interrupt != pAdapter->InterruptLevel)
					continue;
				/* Check the class for cardbus bridge, otherwise check RALINK device */
				if (Class != CARD_BRIDGE_CLASS)
				{
					if (Vendor != RALINK)
						continue;
					else
					{
						/* Found Ralink device before cardbus bridge indicate this is not in cardbus system */
						/* We should stop scanning and patch right away */
						DBGPRINT(RT_DEBUG_TRACE, ("This indicate this is not cardbus device!\n"));
						return;
					}
				}
				
				/* Second, Modify Latency timer and cache line size to 0x40, 0x08 respectly */
				Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x0c);
				CacheLine   = Configuration & 0x000000ff;
				PciLantency = (Configuration & 0x0000ff00) >> 8;
				if (CacheLine == 0)
					Configuration = (Configuration & 0xffffff00) | 0x08;
				if (PciLantency == 0)
					Configuration = (Configuration & 0xffff00ff) | 0xa800;
					
				RTMPWriteCBConfig(Bus, Slot, Func, 0x0c, Configuration);
				DBGPRINT(RT_DEBUG_TRACE, ("Configuration 0x0c is %x\n", Configuration));

				/* Read Subordinate bus number for our device, it must be there */
				Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x18);
				SubBus  = (Configuration & 0x00ff0000) >> 16;
				CardBus = (Configuration & 0x0000ff00) >> 8;
				
				/* Second check for prefecth bit modification requirement */
				switch (Vendor)
				{
					case	RICOH:
						/* Do prefectch configuration */
						/* Config register */
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x80);
						Configuration |= 0x01;
						RTMPWriteCBConfig(Bus, Slot, Func, 0x80, Configuration);
						DBGPRINT(RT_DEBUG_TRACE, ("RICOH 0x80 is %x\n", Configuration));
						break;
					
					case	O2MICRO:
						/* Do prefectch configuration */
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x94);
						/* Do prefectch configuration */
						Configuration |= 0x02;
						RTMPWriteCBConfig(Bus, Slot, Func, 0x94, Configuration);
						DBGPRINT(RT_DEBUG_TRACE, ("O2MICRO 6933 0x94 is %x\n", Configuration));
						/* Do unknown 6912 patch */
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0xd4);
						Configuration |= 0x02;
						RTMPWriteCBConfig(Bus, Slot, Func, 0xd4, Configuration);
						DBGPRINT(RT_DEBUG_TRACE, ("O2MICRO 6912 0xd4 is %x\n", Configuration));
						break;
						
					case	TI:
						/* Do prefectch configuration */
						/* Config register */
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x18);
						CardbusLantency = (Configuration & 0xff000000) >> 24;
						if (CardbusLantency == 0)
						{
							Configuration = (Configuration & 0x00ffffffff) | 0xb0000000;
							RTMPWriteCBConfig(Bus, Slot, Func, 0x18, Configuration);
							DBGPRINT(RT_DEBUG_TRACE, ("TI 0x18 is %x\n", Configuration));
						}
						break;
						
					case	ENE:
						/* Do patch for ENE cardbus issue found on ASUS notebook */
						/* Change offset address 0xCB to 0x1f */
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0xC8);
						Configuration = (Configuration & 0x00ffffff) | 0x1f000000;
						RTMPWriteCBConfig(Bus, Slot, Func, 0xC8, Configuration);
						DBGPRINT(RT_DEBUG_TRACE, ("ENE 0xc8 is %x\n", Configuration));
						/* */
						/* ENE need to path with following code by Max 2005/07 */
						/* */
						Configuration = RTMPReadCBConfig(Bus, Slot, Func, 0x80);
						Configuration |= 0x01;
						RTMPWriteCBConfig(Bus, Slot, Func, 0x80, Configuration);
						DBGPRINT(RT_DEBUG_TRACE, ("ENE 0x80 is %x\n", Configuration));
					default:
						break;
				}
				/* Go to config ralink device after setup cardbus controller */
				RTMPPatchRalinkCardBus(pAdapter, SubBus);
				RTMPPatchRalinkCardBus(pAdapter, CardBus);
			}
		}
	}
	DBGPRINT(RT_DEBUG_TRACE, ("<---RTMPPatchCardBus\n"));
}

/* IRQL = PASSIVE_LEVEL */
VOID RTMPPatchRalinkCardBus(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	ULONG			Bus)
{
	USHORT	Slot;
	ULONG	Configuration;
	ULONG	Vendor;
	ULONG	PciLantency;
	ULONG	CacheLine;
	
	for (Slot = 0; Slot < MAX_PCI_DEVICE; Slot++)
	{
		/* There is only one function for Ralink device which is 0 */
		Vendor	  = RTMPReadCBConfig(Bus, Slot, 0, 0x00) & 0x0000FFFF;
		if (Vendor != RALINK)
			continue;
		/* Second, Modify Latency timer and cache line size to 0x40, 0x08 respectly */
		Configuration = RTMPReadCBConfig(Bus, Slot, 0, 0x0c);
		CacheLine	= Configuration & 0x000000ff;
		PciLantency = (Configuration & 0x0000ff00) >> 8;
		if (CacheLine == 0)
			Configuration = (Configuration & 0xffffff00) | 0x08;
		if (PciLantency == 0)
			Configuration = (Configuration & 0xffff00ff) | 0xa800;
				
		RTMPWriteCBConfig(Bus, Slot, 0, 0x0c, Configuration);
		DBGPRINT(RT_DEBUG_TRACE, ("Patch Ralink Cardbus 0x0c is %x\n", Configuration));
	}
}

/* */
/* This routine will read a long (32 bits) from configuration registers. */
/* */
/* IRQL = PASSIVE_LEVEL */
ULONG RTMPReadCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset)
{
	ULONG	Value;
	ULONG	ConfigAddr;

	ConfigAddr = (Bus << 16) | (Slot << 11) | (Func << 8) | Offset | 0x80000000L;
	
	NdisRawWritePortUlong(PCI_CFG_ADDR_PORT, ConfigAddr);
	RtmpusecDelay(10);
	NdisRawReadPortUlong (PCI_CFG_DATA_PORT, &Value);

	return(Value);
}

/* */
/* This routine will write a long (32 bits) from configuration registers. */
/* */
/* IRQL = PASSIVE_LEVEL */
VOID RTMPWriteCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset,
	IN	ULONG	Value)
{
	ULONG	ConfigAddr;

	ConfigAddr = (Bus << 16) | (Slot << 11) | (Func << 8) | Offset | 0x80000000L;
	
	NdisRawWritePortUlong(PCI_CFG_ADDR_PORT, ConfigAddr);
	RtmpusecDelay(10);
	NdisRawWritePortUlong(PCI_CFG_DATA_PORT, Value);
}

#endif /* RTMP_MAC_PCI */

