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
 	Dma.c
 
	Abstract:
    DMA function routines.

	Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   08-21-2002    created

*/

#include "config.h"

#ifdef RTMP_PCI_SUPPORT


extern BOOLEAN					g_bDam4kBoundaryCheck;
extern ULONG	TXWI_SIZE;
USHORT g_SequenceNumber=0;
USHORT g_SequenceNumber_Ac0=0;
USHORT g_SequenceNumber_Ac1=0;
USHORT g_SequenceNumber_Ac2=0;
USHORT g_SequenceNumber_Ac3=0;
USHORT g_SequenceNumber_Ac4=0;
USHORT g_SequenceNumber_Ac5=0;
USHORT g_SequenceNumber_Ac6=0;
USHORT g_SequenceNumber_Ac7=0;
USHORT g_SequenceNumber_Ac8=0;
USHORT g_SequenceNumber_Ac9=0;
USHORT g_SequenceNumber_Ac10=0;
USHORT g_SequenceNumber_Ac11=0;
USHORT g_SequenceNumber_Mgmt=0;
USHORT g_SequenceNumber_Hcca=0;
//extern ULONG g_chPattern[10];
extern UCHAR g_FWSeqUI;


VOID InitDMA(RTMP_ADAPTER *pDevExt)
{
	ULONG value;

	//-------------------------------------------------------------------------------------
	//
	// Fill some register for Tx/Rx Control Register
	//


	//reset dma dtx
	RTMP_IO_READ32(pDevExt,RA_WPDMA_GLO_CFG,&value);
	value &= ~(TX_DMA_EN | RX_DMA_EN);
	RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, value );

	//value=	  RTMP_Real_IO_READ32(pDevExt,RA_SYS_CTRL);		
	//RTMP_Real_IO_WRITE32( pDevExt, RA_SYS_CTRL, (value | 0x00000008) );
	KeStallExecutionProcessor(10);
	//RTMP_Real_IO_WRITE32( pDevExt, RA_SYS_CTRL, (value & 0xFFFFFFF7) );
	
	RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_RST_IDX, 0x1003f);
	// 1. Set TX descriptor base registers
	
	// Set TX descriptor base registers
	value = pDevExt->Ac0RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR0, value);
	DBGPRINT(RT_DEBUG_TRACE,("initDMA :  ===>	ac 0 base= %x\n", value));
	
	value = pDevExt->Ac1RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR1, value);

	value = pDevExt->Ac2RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR2, value);

	value = pDevExt->Ac3RingMain[0].AllocPa.LowPart;
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR3, value);

	value = pDevExt->Ac4RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR4, value);

	value = pDevExt->Ac5RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR5, value);

	value = pDevExt->Ac6RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR6, value);
	
	value = pDevExt->Ac7RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR7, value);


	value = pDevExt->Ac8RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR8, value);

	value = pDevExt->Ac9RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR9, value);

	value = pDevExt->Ac10RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR10, value);

	value = pDevExt->Ac11RingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR11, value);	
	
	value = pDevExt->MgmtRingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR12, value);
	
	value = pDevExt->HccaRingMain[0].AllocPa.LowPart;	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_BASE_PTR13, value);

	
	//init dma cfg register
	//init ring size
	/*RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX0, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX1, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX2, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX3, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX4, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX5, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX6, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX7, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX8, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX9, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX0, 0);
	RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX1, 0);*/
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT0, AC0_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT1, AC1_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT2, AC2_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT3, AC3_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT4, AC4_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT5, AC5_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT6, AC6_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT7, AC7_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT8, AC8_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT9, AC9_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT10, AC10_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT11, AC11_RING_SIZE);	
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT12, MGMT_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_TX_MAX_CNT13, HCCA_RING_SIZE);
	
//	value = pDevExt->HccaRingMain[0].AllocPa.LowPart;	
//	RTMP_Real_IO_WRITE32(pDevExt, RA_HCCA_DSCP_BASE, value);

	// 1. Set RX descriptor base registers

	// Set RX descriptor base register
	value = pDevExt->Rx0RingMain[0].AllocPa.LowPart;
	RTMP_Real_IO_WRITE32(pDevExt, RA_RX_BASE_PTR0,(ULONG)value);
	value = pDevExt->Rx1RingMain[0].AllocPa.LowPart;
	RTMP_Real_IO_WRITE32(pDevExt, RA_RX_BASE_PTR1,(ULONG)value);
	
	RTMP_Real_IO_WRITE32(pDevExt, RA_RX_MAX_CNT0,RX0_RING_SIZE);
	RTMP_Real_IO_WRITE32(pDevExt, RA_RX_MAX_CNT1,RX1_RING_SIZE);	
	RTMP_IO_READ32(pDevExt,RA_WPDMA_GLO_CFG,&value);	
	//RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, value | TX_WB_DDONE | TX_DMA_EN | RX_DMA_EN);
	//RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, value | TX_WB_DDONE | TX_DMA_EN | RX_DMA_EN | TX_DMA_BIGENDIAN);
	
	//RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, value | TX_WB_DDONE | TX_DMA_EN | RX_DMA_EN);

	value &= 0xFFFFFBFF;//clearMULTI_DMA_EN
	//RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, value | TX_WB_DDONE | TX_DMA_EN |RX_DMA_EN | TX_BT_SIZE | FIFO_LITTLEENDIAN);
	RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, 0x52000850| TX_DMA_EN |RX_DMA_EN);
	
	DBGPRINT(RT_DEBUG_TRACE,("initDMA :  <===\n"));
	//value=0x500004D5;
	//RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, value );
	
	//RTMP_Real_IO_WRITE32(pDevExt,RA_INT_MASK, 0);				
	
	//-------------------------------------------------------------------------------------	
	
}

NTSTATUS 
ProbeDmaData(
	IN PDEVICE_OBJECT pDO,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	)
{
	NTSTATUS                ntStatus = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) pDO->DeviceExtension;
	PRXD_STRUC				RxDescriptor;
	ULONG					i,datalen=0;

//	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: ProbeDmaData  ===>\n");
	return STATUS_UNSUCCESSFUL ;
}

NTSTATUS 
GetDmaAdapter( 
	IN PDEVICE_OBJECT pDevObj 
	)
{
	NTSTATUS                ntStatus = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)	pDevObj->DeviceExtension;
	DEVICE_DESCRIPTION		dd;
	ULONG					NumberOfMapRegisters;

	RtlZeroMemory( &dd, sizeof(dd) );

	dd.Version = DEVICE_DESCRIPTION_VERSION;
	dd.Master = TRUE;
	dd.ScatterGather = TRUE;
	dd.DemandMode = FALSE;						//Not used for busmaster DMA
	dd.AutoInitialize = FALSE;
	dd.Dma32BitAddresses = TRUE;
	dd.IgnoreCount = TRUE;
	dd.Dma64BitAddresses = FALSE;

	dd.DmaChannel = ((ULONG) ~0);
	dd.InterfaceType = PCIBus;
	dd.DmaWidth = Width32Bits;
	dd.DmaSpeed = Compatible;
	dd.MaximumLength = (ULONG) -1;

	pDevExt->pDmaAdapter = IoGetDmaAdapter(pDevExt->pPhyDeviceObj, &dd, &NumberOfMapRegisters);
	
	if(!pDevExt->pDmaAdapter)
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;		
	else
		ntStatus = STATUS_SUCCESS;
	
	return ntStatus;
}


void InitTxDescriptor(PTXD_STRUC TxDescriptor)
{
	TxDescriptor->DDONE = 0;
}

NTSTATUS	AllocateDMAMemory(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS				Status = STATUS_SUCCESS;
	ULONG					BaseRegRequired;
	ULONG					DescriptorRequiredTotBytes;
	ULONG					RingBasePaHigh;
	ULONG					RingBasePaLow;
	PVOID					RingBaseVa;
	LONG					index;
	PTXD_STRUC				TxDescriptor;
	PRXD_STRUC				RxDescriptor;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: AllocateDMAMemory ===> \n"));
    
	pDevExt->DmaStatus = FALSE;


	//-----------------------------------------------------------------------------------------
	// RingMain
	
	// Calculate total base register number,
	/*DescriptorRequiredTotBytes = 	(AC0_RING_SIZE+AC0_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC1_RING_SIZE+AC1_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC2_RING_SIZE+AC2_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC3_RING_SIZE+AC3_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC4_RING_SIZE+AC4_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC5_RING_SIZE+AC5_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC6_RING_SIZE+AC6_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC7_RING_SIZE+AC7_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(MGMT_RING_SIZE+MGMT_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE +\
								(HCCA_RING_SIZE+HCCA_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE +\
								(RX_RING_SIZE)*RING_RX_DESCRIPTOR_SIZE;*/

		DescriptorRequiredTotBytes = 	(AC0_RING_SIZE+AC0_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC1_RING_SIZE+AC1_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC2_RING_SIZE+AC2_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC3_RING_SIZE+AC3_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC4_RING_SIZE+AC4_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC5_RING_SIZE+AC5_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC6_RING_SIZE+AC6_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC7_RING_SIZE+AC7_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC8_RING_SIZE+AC8_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC9_RING_SIZE+AC9_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC10_RING_SIZE+AC10_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(AC11_RING_SIZE+AC11_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE  + \
								(MGMT_RING_SIZE+MGMT_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE +\
								(HCCA_RING_SIZE+HCCA_TEMP_SIZE)*RING_TX_DESCRIPTOR_SIZE +\
								(RX0_RING_SIZE)*RING_RX_DESCRIPTOR_SIZE+\
								(RX1_RING_SIZE)*RING_RX_DESCRIPTOR_SIZE;

	do
	{		
		if(GetDmaAdapter(DeviceObject) != STATUS_SUCCESS)		break;
		
		if (DescriptorRequiredTotBytes > PAGE_SIZE)
		{
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Descriptor require more than one page memory\n"));
			Status = STATUS_UNSUCCESSFUL;
			break;
		}
		
		pDevExt->RingMain.AllocSize = DescriptorRequiredTotBytes;

		pDevExt->RingMain.AllocVa = 
			pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
				pDevExt->pDmaAdapter, 
				pDevExt->RingMain.AllocSize, 
				&pDevExt->RingMain.AllocPa, 
				TRUE
			);
		
		if (!pDevExt->RingMain.AllocVa)
		{
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
			Status = STATUS_UNSUCCESSFUL;
			break;
		}
		
		// Zero init this memory block
		RtlZeroMemory(pDevExt->RingMain.AllocVa, pDevExt->RingMain.AllocSize);
		
		// Save PA & VA for further operation
		RingBasePaHigh = pDevExt->RingMain.AllocPa.HighPart;
		RingBasePaLow  = pDevExt->RingMain.AllocPa.LowPart;
		RingBaseVa	= pDevExt->RingMain.AllocVa;
		
		//
		// Initialize Ac0 Ring and associated buffer memory
		//
		for (index = 0; index < AC0_RING_SIZE+AC0_TEMP_SIZE; index++)
		{
			// Init Tx Ring Size, Va, Pa variables
			pDevExt->Ac0RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac0RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac0RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac0RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Tx Buffer size & allocate share memory
			pDevExt->Ac0RingMain[index].DmaBuf.AllocSize = AC0_BUFFER_SIZE;
			

			pDevExt->Ac0RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac0RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac0RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac0RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac0RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac0RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac0RingMain[index].AllocVa);				
			
			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);

			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac0RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------

			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac0RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac0RingMain[index].AllocVa, pDevExt->Ac0RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac0RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac0 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac0RingMain[index].DmaBuf.AllocVa, pDevExt->Ac0RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac0RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac1 Ring and associated buffer memory
		//		
		for (index = 0; index < AC1_RING_SIZE+AC1_TEMP_SIZE; index++)
		{
			// Init Tx Ring Size, Va, Pa variables
			pDevExt->Ac1RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac1RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac1RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac1RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Tx Buffer size & allocate share memory
			pDevExt->Ac1RingMain[index].DmaBuf.AllocSize = AC1_BUFFER_SIZE;
			

			pDevExt->Ac1RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac1RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac1RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac1RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac1RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac1RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac1RingMain[index].AllocVa);	
				
			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);

			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac1RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------


			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac1RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac1RingMain[index].AllocVa, pDevExt->Ac1RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac1RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac1 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac1RingMain[index].DmaBuf.AllocVa, pDevExt->Ac1RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac1RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac2 Ring and associated buffer memory
		//		
		for (index = 0; index < AC2_RING_SIZE+AC2_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac2RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac2RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac2RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac2RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac2RingMain[index].DmaBuf.AllocSize = AC2_BUFFER_SIZE;
			
			pDevExt->Ac2RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac2RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac2RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac2RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac2RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac2RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac2RingMain[index].AllocVa);			
				
			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);

			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac2RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac2RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac2RingMain[index].AllocVa, pDevExt->Ac2RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac2RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac2 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac2RingMain[index].DmaBuf.AllocVa, pDevExt->Ac2RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac2RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac3 Ring and associated buffer memory
		//		
		for (index = 0; index < AC3_RING_SIZE+AC3_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac3RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac3RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac3RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac3RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac3RingMain[index].DmaBuf.AllocSize = AC3_BUFFER_SIZE;
			
			pDevExt->Ac3RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac3RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac3RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac3RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac3RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac3RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac3RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac3RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac3RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac3RingMain[index].AllocVa, pDevExt->Ac3RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac3RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac3 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac3RingMain[index].DmaBuf.AllocVa, pDevExt->Ac3RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac3RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac4 Ring and associated buffer memory
		//		
		for (index = 0; index < AC4_RING_SIZE+AC4_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac4RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac4RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac4RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac4RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac4RingMain[index].DmaBuf.AllocSize = AC4_BUFFER_SIZE;
			
			pDevExt->Ac4RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac4RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac4RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac4RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac4RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac4RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac4RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac4RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac4RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac4RingMain[index].AllocVa, pDevExt->Ac4RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac4RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac4 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac4RingMain[index].DmaBuf.AllocVa, pDevExt->Ac4RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac4RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac5 Ring and associated buffer memory
		//		
		for (index = 0; index < AC5_RING_SIZE+AC5_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac5RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac5RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac5RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac5RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac5RingMain[index].DmaBuf.AllocSize = AC5_BUFFER_SIZE;
			
			pDevExt->Ac5RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac5RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac5RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac5RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac5RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac5RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac5RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac5RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac5RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac5RingMain[index].AllocVa, pDevExt->Ac5RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac5RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac5 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac5RingMain[index].DmaBuf.AllocVa, pDevExt->Ac5RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac5RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac6 Ring and associated buffer memory
		//		
		for (index = 0; index < AC6_RING_SIZE+AC6_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac6RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac6RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac6RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac6RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac6RingMain[index].DmaBuf.AllocSize = AC6_BUFFER_SIZE;
			
			pDevExt->Ac6RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac6RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac6RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac6RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac6RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac6RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac6RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac6RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac6RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac6RingMain[index].AllocVa, pDevExt->Ac6RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac6RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac6 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac6RingMain[index].DmaBuf.AllocVa, pDevExt->Ac6RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac6RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Ac7 Ring and associated buffer memory
		//		
		for (index = 0; index < AC7_RING_SIZE+AC7_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac7RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac7RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac7RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac7RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac7RingMain[index].DmaBuf.AllocSize = AC7_BUFFER_SIZE;
			
			pDevExt->Ac7RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac7RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac7RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac7RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac7RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac7RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac7RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac7RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac7RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac7RingMain[index].AllocVa, pDevExt->Ac7RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac7RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac7 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac7RingMain[index].DmaBuf.AllocVa, pDevExt->Ac7RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac7RingMain[index].DmaBuf.AllocSize));
		}


		//
		// Initialize Ac8 Ring and associated buffer memory
		//		
		for (index = 0; index < AC8_RING_SIZE+AC8_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac8RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac8RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac8RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac8RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac8RingMain[index].DmaBuf.AllocSize = AC8_BUFFER_SIZE;
			
			pDevExt->Ac8RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac8RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac8RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac8RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac8RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac8RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac8RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac8RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac8RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac8RingMain[index].AllocVa, pDevExt->Ac8RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac8RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac8 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac8RingMain[index].DmaBuf.AllocVa, pDevExt->Ac8RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac8RingMain[index].DmaBuf.AllocSize));
		}
		

		//
		// Initialize Ac9 Ring and associated buffer memory
		//		
		for (index = 0; index < AC9_RING_SIZE+AC9_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac9RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac9RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac9RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac9RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac9RingMain[index].DmaBuf.AllocSize = AC9_BUFFER_SIZE;
			
			pDevExt->Ac9RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac9RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac9RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac9RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac9RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac9RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac9RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac9RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac9RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac9RingMain[index].AllocVa, pDevExt->Ac9RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac9RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac9 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac9RingMain[index].DmaBuf.AllocVa, pDevExt->Ac9RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac9RingMain[index].DmaBuf.AllocSize));
		}
		
		//
		// Initialize Ac10 Ring and associated buffer memory
		//		
		for (index = 0; index < AC10_RING_SIZE+AC10_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac10RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac10RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac10RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac10RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac10RingMain[index].DmaBuf.AllocSize = AC10_BUFFER_SIZE;
			
			pDevExt->Ac10RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac10RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac10RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac10RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac10RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac10RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac10RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac10RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac10RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac10RingMain[index].AllocVa, pDevExt->Ac10RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac10RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac10 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac10RingMain[index].DmaBuf.AllocVa, pDevExt->Ac10RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac10RingMain[index].DmaBuf.AllocSize));
		}
		
		//
		// Initialize Ac11 Ring and associated buffer memory
		//		
		for (index = 0; index < AC11_RING_SIZE+AC11_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->Ac11RingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->Ac11RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Ac11RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Ac11RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->Ac11RingMain[index].DmaBuf.AllocSize = AC11_BUFFER_SIZE;
			
			pDevExt->Ac11RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Ac11RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Ac11RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Ac11RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Ac11RingMain[index].DmaBuf.AllocVa,
				pDevExt->Ac11RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->Ac11RingMain[index].AllocVa);		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->Ac11RingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac11RingMain VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac11RingMain[index].AllocVa, pDevExt->Ac11RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac11RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Ac11 Buf VA = 0x%x, PA = 0x%x, ",
				pDevExt->Ac11RingMain[index].DmaBuf.AllocVa, pDevExt->Ac11RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Ac11RingMain[index].DmaBuf.AllocSize));
		}
		
		
		//
		// Initialize Mgmt Ring and associated buffer memory
		//		
		for (index = 0; index < MGMT_RING_SIZE+MGMT_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->MgmtRingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->MgmtRingMain[index].AllocVa = RingBaseVa;
			pDevExt->MgmtRingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->MgmtRingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->MgmtRingMain[index].DmaBuf.AllocSize = MGMT_BUFFER_SIZE;
			
			pDevExt->MgmtRingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->MgmtRingMain[index].DmaBuf.AllocSize, 
					&pDevExt->MgmtRingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->MgmtRingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->MgmtRingMain[index].DmaBuf.AllocVa,
				pDevExt->MgmtRingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->MgmtRingMain[index].AllocVa);				
		

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->MgmtRingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: MgmtRingMain VA = 0x%x, PA = 0x%x, ", pDevExt->MgmtRingMain[index].AllocVa, pDevExt->MgmtRingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->MgmtRingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Mgmt Buf VA = 0x%x, PA = 0x%x, ", pDevExt->MgmtRingMain[index].DmaBuf.AllocVa, pDevExt->MgmtRingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->MgmtRingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Hcca Ring and associated buffer memory
		//		
		for (index = 0; index < HCCA_RING_SIZE+HCCA_TEMP_SIZE; index++)
		{
			// Init Priority Ring Size, Va, Pa variables
			pDevExt->HccaRingMain[index].AllocSize = RING_TX_DESCRIPTOR_SIZE;
			pDevExt->HccaRingMain[index].AllocVa = RingBaseVa;
			pDevExt->HccaRingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->HccaRingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_TX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_TX_DESCRIPTOR_SIZE;
			
			// Setup Priority Buffer size & allocate share memory
			pDevExt->HccaRingMain[index].DmaBuf.AllocSize = HCCA_BUFFER_SIZE;
			
			pDevExt->HccaRingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->HccaRingMain[index].DmaBuf.AllocSize, 
					&pDevExt->HccaRingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->HccaRingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->HccaRingMain[index].DmaBuf.AllocVa,
				pDevExt->HccaRingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each TX descriptor
			TxDescriptor = (PTXD_STRUC)(pDevExt->HccaRingMain[index].AllocVa);	

			//--------------------------------------------------------------------
			
			InitTxDescriptor(TxDescriptor);
			
			//--------------------------------------------------------------------
			TxDescriptor->SDL0= 0;
		
			TxDescriptor->SDP0= pDevExt->HccaRingMain[index].DmaBuf.AllocPa.LowPart;
		
			//--------------------------------------------------------------------
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: HccaRingMain VA = 0x%x, PA = 0x%x, ", pDevExt->HccaRingMain[index].AllocVa, pDevExt->HccaRingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->HccaRingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Hcca Buf VA = 0x%x, PA = 0x%x, ", pDevExt->HccaRingMain[index].DmaBuf.AllocVa, pDevExt->HccaRingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->HccaRingMain[index].DmaBuf.AllocSize));
		}


		//
		// Initialize Rx0 Ring and associated buffer memory
		//		
		for (index = 0; index < RX0_RING_SIZE; index++)
		{
			// Init Rx Ring Size, Va, Pa variables
			pDevExt->Rx0RingMain[index].AllocSize = RING_RX_DESCRIPTOR_SIZE;
			pDevExt->Rx0RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Rx0RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Rx0RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_RX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_RX_DESCRIPTOR_SIZE;
			
			// Setup Rx Buffer size & allocate share memory
			pDevExt->Rx0RingMain[index].DmaBuf.AllocSize = RX_BUFFER_SIZE;
			

			pDevExt->Rx0RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Rx0RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Rx0RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Rx0RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Rx0RingMain[index].DmaBuf.AllocVa,
				pDevExt->Rx0RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each RX descriptor
			RxDescriptor					= (PRXD_STRUC)(pDevExt->Rx0RingMain[index].AllocVa);				
			RxDescriptor->SDL0	= RX_BUFFER_SIZE;
			RxDescriptor->SDP0	= pDevExt->Rx0RingMain[index].DmaBuf.AllocPa.LowPart;
			#ifdef RX_SCATTER
			RxDescriptor->SDP1	= RxDescriptor->SDP0 + 40; // header length equ
			#endif

			//++werner test->
			/*PULONG	ptr = (PULONG)RxDescriptor;
			 *(ptr+2) = 0xFFFFFFFF;
			 *(ptr+3) = 0xFFFFFFFF;
			DBGPRINT(RT_DEBUG_TRACE,("RX DMA = 0 x%X\n",  *(ptr));
			DBGPRINT(RT_DEBUG_TRACE,("RX DMA = 1 x%X\n", *(ptr+1));
			DBGPRINT(RT_DEBUG_TRACE,("RX DMA = 2 x%X\n",*(ptr+2));
			DBGPRINT(RT_DEBUG_TRACE,("RX DMA = 3 x%X\n", *(ptr+3));*/			
			//++werner test<-
			
			
			RxDescriptor->DDONE 	= 0x0;

			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Rx0RingMain VA = 0x%x, PA = 0x%x, ", pDevExt->Rx0RingMain[index].AllocVa, pDevExt->Rx0RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Rx0RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Rx0 Buf VA = 0x%x, PA = 0x%x, ", pDevExt->Rx0RingMain[index].DmaBuf.AllocVa, pDevExt->Rx0RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Rx0RingMain[index].DmaBuf.AllocSize));
		}

		//
		// Initialize Rx1 Ring and associated buffer memory
		//		
		for (index = 0; index < RX1_RING_SIZE; index++)
		{
			// Init Rx Ring Size, Va, Pa variables
			pDevExt->Rx1RingMain[index].AllocSize = RING_RX_DESCRIPTOR_SIZE;
			pDevExt->Rx1RingMain[index].AllocVa = RingBaseVa;
			pDevExt->Rx1RingMain[index].AllocPa.HighPart = RingBasePaHigh;
			pDevExt->Rx1RingMain[index].AllocPa.LowPart = RingBasePaLow;
			
			// Offset to next ring descriptor address
			RingBasePaLow += RING_RX_DESCRIPTOR_SIZE;		
			RingBaseVa = (PUCHAR) RingBaseVa + RING_RX_DESCRIPTOR_SIZE;
			
			// Setup Rx Buffer size & allocate share memory
			pDevExt->Rx1RingMain[index].DmaBuf.AllocSize = RX_BUFFER_SIZE;
			

			pDevExt->Rx1RingMain[index].DmaBuf.AllocVa = 
				pDevExt->pDmaAdapter->DmaOperations->AllocateCommonBuffer( 
					pDevExt->pDmaAdapter, 
					pDevExt->Rx1RingMain[index].DmaBuf.AllocSize, 
					&pDevExt->Rx1RingMain[index].DmaBuf.AllocPa, 
					TRUE
				);

			if (!pDevExt->Rx1RingMain[index].DmaBuf.AllocVa)
			{
				DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Failed to allocate a big buffer\n"));
				Status = STATUS_UNSUCCESSFUL;
				break;
			}
			
			// Zero init this memory block
			RtlZeroMemory(
				pDevExt->Rx1RingMain[index].DmaBuf.AllocVa,
				pDevExt->Rx1RingMain[index].DmaBuf.AllocSize);

			// Fill buffer address in each RX descriptor
			RxDescriptor					= (PRXD_STRUC)(pDevExt->Rx1RingMain[index].AllocVa);				
			RxDescriptor->SDL0	= RX_BUFFER_SIZE;
			RxDescriptor->SDP0	= pDevExt->Rx1RingMain[index].DmaBuf.AllocPa.LowPart;
			#ifdef RX_SCATTER
			RxDescriptor->SDP1	= RxDescriptor->SDP0 + 40; // header length equ
			#endif
			
			RxDescriptor->DDONE 	= 0x0;

			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Rx1RingMain VA = 0x%x, PA = 0x%x, ", pDevExt->Rx1RingMain[index].AllocVa, pDevExt->Rx1RingMain[index].AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Rx1RingMain[index].AllocSize));
			
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: Rx1 Buf VA = 0x%x, PA = 0x%x, ", pDevExt->Rx1RingMain[index].DmaBuf.AllocVa, pDevExt->Rx1RingMain[index].DmaBuf.AllocPa));
			DBGPRINT(RT_DEBUG_TRACE,(" size = 0x%x\n", pDevExt->Rx1RingMain[index].DmaBuf.AllocSize));
		}

		
		pDevExt->DmaStatus = TRUE;
		Status = STATUS_SUCCESS;
	}	while (FALSE);	

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: AllocateDMAMemory <=== \n\n"));
	return Status;
}


NTSTATUS	FreeDMAMemory(
	IN PDEVICE_OBJECT DeviceObject)
{
	NTSTATUS				Status = STATUS_SUCCESS;
	LONG					index;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: FreeDMAMemory ===> \n"));

	if(pDevExt->pDmaAdapter != NULL)
	{

	//-----------------------------------------------------------------------------------------
	// RingMain
	
		if(pDevExt->RingMain.AllocVa)
		{
			pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
				pDevExt->pDmaAdapter,
				pDevExt->RingMain.AllocSize,
				pDevExt->RingMain.AllocPa,
				pDevExt->RingMain.AllocVa,
				TRUE);

			//
			// Release Ac0 Ring and associated buffer memory
			//
			for (index = 0; index < AC0_RING_SIZE; index++)
			{
				if(pDevExt->Ac0RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac0RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac0RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac0RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac1 Ring and associated buffer memory
			//
			for (index = 0; index < AC1_RING_SIZE; index++)
			{
				if(pDevExt->Ac1RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac1RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac1RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac1RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac2 Ring and associated buffer memory
			//
			for (index = 0; index < AC2_RING_SIZE; index++)
			{
				if(pDevExt->Ac2RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac2RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac2RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac2RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac3 Ring and associated buffer memory
			//
			for (index = 0; index < AC3_RING_SIZE; index++)
			{
				if(pDevExt->Ac3RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac3RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac3RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac3RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac4 Ring and associated buffer memory
			//
			for (index = 0; index < AC4_RING_SIZE; index++)
			{
				if(pDevExt->Ac4RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac4RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac4RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac4RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac5 Ring and associated buffer memory
			//
			for (index = 0; index < AC5_RING_SIZE; index++)
			{
				if(pDevExt->Ac5RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac5RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac5RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac5RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac6 Ring and associated buffer memory
			//
			for (index = 0; index < AC6_RING_SIZE; index++)
			{
				if(pDevExt->Ac6RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac6RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac6RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac6RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac7 Ring and associated buffer memory
			//
			for (index = 0; index < AC7_RING_SIZE; index++)
			{
				if(pDevExt->Ac7RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac7RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac7RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac7RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac8 Ring and associated buffer memory
			//
			for (index = 0; index < AC8_RING_SIZE; index++)
			{
				if(pDevExt->Ac8RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac8RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac8RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac8RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac9 Ring and associated buffer memory
			//
			for (index = 0; index < AC9_RING_SIZE; index++)
			{
				if(pDevExt->Ac9RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac9RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac9RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac9RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac10 Ring and associated buffer memory
			//
			for (index = 0; index < AC10_RING_SIZE; index++)
			{
				if(pDevExt->Ac10RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac10RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac10RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac10RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Ac11 Ring and associated buffer memory
			//
			for (index = 0; index < AC11_RING_SIZE; index++)
			{
				if(pDevExt->Ac11RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Ac11RingMain[index].DmaBuf.AllocSize,
						pDevExt->Ac11RingMain[index].DmaBuf.AllocPa,
						pDevExt->Ac11RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Mgmt Ring and associated buffer memory
			//
			for (index = 0; index < MGMT_RING_SIZE; index++)
			{
				if(pDevExt->MgmtRingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->MgmtRingMain[index].DmaBuf.AllocSize,
						pDevExt->MgmtRingMain[index].DmaBuf.AllocPa,
						pDevExt->MgmtRingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Hcca Ring and associated buffer memory
			//
			for (index = 0; index < HCCA_RING_SIZE; index++)
			{
				if(pDevExt->HccaRingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->HccaRingMain[index].DmaBuf.AllocSize,
						pDevExt->HccaRingMain[index].DmaBuf.AllocPa,
						pDevExt->HccaRingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}
			
			//
			// Release Rx0 Ring and associated buffer memory
			//
			for (index = 0; index < RX0_RING_SIZE; index++)
			{
				if(pDevExt->Rx0RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Rx0RingMain[index].DmaBuf.AllocSize,
						pDevExt->Rx0RingMain[index].DmaBuf.AllocPa,
						pDevExt->Rx0RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}

			//
			// Release Rx1 Ring and associated buffer memory
			//
			for (index = 0; index < RX1_RING_SIZE; index++)
			{
				if(pDevExt->Rx1RingMain[index].DmaBuf.AllocVa)
				{
					pDevExt->pDmaAdapter->DmaOperations->FreeCommonBuffer(
						pDevExt->pDmaAdapter,
						pDevExt->Rx1RingMain[index].DmaBuf.AllocSize,
						pDevExt->Rx1RingMain[index].DmaBuf.AllocPa,
						pDevExt->Rx1RingMain[index].DmaBuf.AllocVa,
						TRUE);
				}
			}
		}

		pDevExt->pDmaAdapter->DmaOperations->PutDmaAdapter(pDevExt->pDmaAdapter);
	}

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: FreeDMAMemory <=== \n"));
	return Status;
}


NTSTATUS
TransferDMADataBuffer(
	IN PDEVICE_OBJECT DeviceObject,
	IN PUCHAR pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
    )
{
	NTSTATUS				ntStatus = STATUS_SUCCESS;
	//ULONG					InBuf[5];	
	ULONG					idx, length, data,i;
	ULONG					SchReg4 =0,IsLoadFW = 0;
	FwTxD_STRUC			FwTxD;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;

	IsLoadFW = *(PULONG)(pIBuffer);//is loadfw or not
	//InBuf[0] = *(PULONG)(pIBuffer);							//bussum
	//InBuf[1] = *(PULONG)(pIBuffer+4);						//address
	idx		 = *(PULONG)(pIBuffer+8);						//offset
	length	 = *(PULONG)(pIBuffer+12);						//length
	data	 = *(PULONG)(pIBuffer+16);						//value			
	DBGPRINT(RT_DEBUG_TRACE,(" idx = %d ,length =%d\n",idx,length));	
	
	
	if(data == RING_TYPE_AC0)
	{
		DBGPRINT(RT_DEBUG_TRACE,(" TransferDMADataBuffer length = %x \n",length));		
		DBGPRINT(RT_DEBUG_TRACE,("AC0Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac0RingMain[idx].DmaBuf.AllocVa, (pIBuffer+ 20), length);

		if(IsLoadFW)
		{
			if(length>=sizeof(FwTxD))
			{
				RtlCopyMemory(&FwTxD, (pIBuffer+ 20), sizeof(FwTxD));
				g_FWSeqUI = (UCHAR)FwTxD.ucSeqNum;
			}
			else
				DBGPRINT(RT_DEBUG_TRACE,("%s IsLoadFW = %d , length is not enough length=%d,  FwTxD lenght=%d\n",__FUNCTION__,IsLoadFW,length,sizeof(FwTxD)));		

		}

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)              
		/*PUCHAR NSEQ = (PUCHAR)(((PUCHAR)(pDevExt->Ac0RingMain[idx].DmaBuf.AllocVa))+4);		
		if (((*NSEQ) & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software	           
			PUCHAR DataType = (PUCHAR)(((PUCHAR)(pDevExt->Ac0RingMain[idx].DmaBuf.AllocVa))+TXWI_SIZE);		     
			if ( ((*DataType) & 0x08) && ((*DataType) & 0x80) ) 
			{
				SEQdbg ("TransferDMADataBuffer ==> \n");
				IncSequenceNumber((UCHAR*)pDevExt->Ac0RingMain[idx].DmaBuf.AllocVa, 0);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac0RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}         
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac0RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}
		*/
		
	}
	else if(data == RING_TYPE_AC1)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC1Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*PUCHAR NSEQ = (PUCHAR)(((PUCHAR)(pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa))+4);
		if (((*NSEQ) & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software	           
			PUCHAR DataType = (PUCHAR)(((PUCHAR)(pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa))+TXWI_SIZE);		     
			if ( ((*DataType) & 0x08) && ((*DataType) & 0x80) ) 
			{
				SEQdbg ("TransferDMADataBuffer ==> \n");
				IncSequenceNumber((UCHAR*)pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa, 1);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}              
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac1RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}
		*/

	}
	else if(data == RING_TYPE_AC2)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC2Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa, 2);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac2RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}
		*/
	}
	else if(data == RING_TYPE_AC3)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC3Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa, 3);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac3RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC4)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC4Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa, 4);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac4RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC5)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC5Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa, 5);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac5RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC6)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC6Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa, 6);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac6RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC7)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC7Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa, 7);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC8)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC8Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac8RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac8RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac8RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac8RingMain[idx].DmaBuf.AllocVa, 8);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac8RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*?

		//IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC9)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC9Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac9RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac9RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac9RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac9RingMain[idx].DmaBuf.AllocVa, 9);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac9RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	
	else if(data == RING_TYPE_AC10)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC10Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac10RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac10RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac10RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac10RingMain[idx].DmaBuf.AllocVa, 10);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac10RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_AC11)
	{
		DBGPRINT(RT_DEBUG_TRACE,("AC11Ring"));
		RtlCopyMemory((PULONG)pDevExt->Ac11RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->Ac11RingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->Ac11RingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac11RingMain[idx].DmaBuf.AllocVa, 11);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->Ac11RingMain[idx].DmaBuf.AllocVa, 10);
			}
		}     
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/

		//IncSequenceNumber((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->Ac7RingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}		
		*/
	}
	else if(data == RING_TYPE_MGMT)
	{
		DBGPRINT(RT_DEBUG_TRACE,("MgmtRing"));
		RtlCopyMemory((PULONG)pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*PUCHAR NSEQ = (PUCHAR)(((PUCHAR)(pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa))+4);
		if (((*NSEQ) & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			PUCHAR DataType = (PUCHAR)(((PUCHAR)(pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa))+TXWI_SIZE);		     
			if ( ((*DataType) & 0x08) && ((*DataType) & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa, 8);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa, 10);
			}
		}       
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/	

		//IncSequenceNumber((UCHAR*)pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa);
		/*
		for (i =0 ;i< ( length - 16 -2);i++)
		{
			*((UCHAR*)pDevExt->MgmtRingMain[idx].DmaBuf.AllocVa + i + 24 + TXWI_SIZE) =(UCHAR)  (g_SequenceNumber   + i);
		}
		*/
	}
	else if(data == RING_TYPE_HCCA)
	{
		DBGPRINT(RT_DEBUG_TRACE,("HccaRing"));
		RtlCopyMemory((PULONG)pDevExt->HccaRingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);

		// TXWI.NSEQ indicates the inc sequence number by hardware (NSEQ=1) or software (NSEQ=0)
		/*UCHAR NSEQ = *((PUCHAR)(pDevExt->HccaRingMain[idx].DmaBuf.AllocVa)+4);
		if ((NSEQ & 0x02) == 0)
		{
			pDevExt->SetTXWI_NSEQ = 0;
			// QoS Data inc sequence number by software
			UCHAR DataType = *((PUCHAR)(pDevExt->HccaRingMain[idx].DmaBuf.AllocVa)+TXWI_SIZE);	
			if ( (DataType & 0x08) && (DataType & 0x80) ) 
			{
				IncSequenceNumber((UCHAR*)pDevExt->HccaRingMain[idx].DmaBuf.AllocVa, 9);
			}
			else
			{
				IncSequenceNumber((UCHAR*)pDevExt->HccaRingMain[idx].DmaBuf.AllocVa, 10);
			}
		}       
		else
		{
			pDevExt->SetTXWI_NSEQ = 1;
		}*/	
	}	
	else if(data == RING_TYPE_RX0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("Rx0Ring"));
		RtlCopyMemory((PULONG)pDevExt->Rx0RingMain[idx].DmaBuf.AllocVa, (pIBuffer+20), length);
	}

	return ntStatus;
}

NTSTATUS
TransferDMADescriptor(
	IN PDEVICE_OBJECT DeviceObject,
	IN PUCHAR pIBuffer,
	IN PUCHAR UserBuffer,
	IN ULONG BufferMaxSize,
	OUT PULONG ByteCount
	)
{
	NTSTATUS				ntStatus = STATUS_SUCCESS;
	ULONG					InBuf[5];	
	ULONG					idx, length, data;
	PTXD_STRUC				TxDescriptor = NULL;
	PRXD_STRUC				RxDescriptor;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) DeviceObject->DeviceExtension;
	PTXD_STRUC TxDescriptorIn;
		
	InBuf[0] = *(PULONG)(pIBuffer);							//busnum
	InBuf[1] = *(PULONG)(pIBuffer+4);						//address
	idx		 = *(PULONG)(pIBuffer+8);						//offset
	length	 = *(PULONG)(pIBuffer+12);						//length
	data	 = *(PULONG)(pIBuffer+16);						//value	// Ring type 0:TxRing	1:AtimRing	2:PrioRing	3:RxRing

	TxDescriptorIn = (PTXD_STRUC)(pIBuffer+20);
	DBGPRINT(RT_DEBUG_TRACE,("TransferDMADescriptor load descriptor:  ===>	idx = %x ,length= %x\n", idx,length));	
	if(RING_TYPE_AC0 == data)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac0RingMain[idx].AllocVa);	
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));	
		TxDescriptor->SDP0 = pDevExt->Ac0RingMain[idx].DmaBuf.AllocPa.LowPart;
		
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor: RING_TYPE_AC0 ===>	idx=%d SDP0 %x\n",idx, TxDescriptorIn->SDP0));
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  ===>	data %x = 0x%x SDL0\n",data, TxDescriptorIn->SDL0));
		//--------------------------------------------------------------------
	}
	else if(RING_TYPE_AC1 == data)
	{		
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac1RingMain[idx].AllocVa);
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));

		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac1RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC1===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC2 == data)
	{		
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac2RingMain[idx].AllocVa);
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));

		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac2RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor: RING_TYPE_AC2 ===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC3 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac3RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac3RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC3===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC4 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac4RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac4RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC4===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC5 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac5RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac5RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC5===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC6 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac6RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac6RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC6===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC7 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac7RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac7RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC7===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC8 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac8RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac8RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC8===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC9 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac9RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac9RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC9===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC10 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac10RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac10RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC10===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_AC11 == data)
	{				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac11RingMain[idx].AllocVa);		
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));
		
		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL=2;
		TxDescriptor->SDP0 = pDevExt->Ac11RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_AC11===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_MGMT == data)
	{	
		TxDescriptor = (PTXD_STRUC)(pDevExt->MgmtRingMain[idx].AllocVa);
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));

		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		//TxDescriptor->PBF_QSEL = 0;
		TxDescriptor->SDP0 = pDevExt->MgmtRingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_MGMT===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}
	else if(RING_TYPE_HCCA == data)
	{		
		TxDescriptor = (PTXD_STRUC)(pDevExt->HccaRingMain[idx].AllocVa);
		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));

		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		TxDescriptor->PBF_QSEL=1;
		TxDescriptor->SDP0 = pDevExt->HccaRingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_HCCA===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
		
	}	
	else if(RING_TYPE_RX0 == data)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Rx0RingMain[idx].AllocVa);

		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));

		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		
		TxDescriptor->SDP0 = pDevExt->Rx0RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------	
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_RX0===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
	}
	else if(RING_TYPE_RX1 == data)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Rx1RingMain[idx].AllocVa);

		RtlMoveMemory(TxDescriptor, TxDescriptorIn, sizeof(TXD_STRUC));

		//--------------------------------------------------------------------
		//TxDescriptor->SDL0 = TxDescriptorIn->SDL0;
		
		TxDescriptor->SDP0 = pDevExt->Rx1RingMain[idx].DmaBuf.AllocPa.LowPart;
		//TxDescriptor->PktLength = TxDescriptorIn->PktLength;
		//TxDescriptor->Is802dot11 = TxDescriptorIn->Is802dot11;
		//--------------------------------------------------------------------	
		DBGPRINT(RT_DEBUG_TRACE,("load descriptor:  RING_TYPE_RX1===>	data %x = 0x%x\n",data, TxDescriptorIn->SDL0));
	}

	if(TxDescriptor)
	{
	PULONG	ptr = (PULONG)TxDescriptor;
			Testdbg("\n");
			Testdbg("DMA D : \n");
			Testdbg("0x%08X\n", *ptr);
			Testdbg("0x%08X\n", *(ptr+1));
			Testdbg("0x%08X\n", *(ptr+2));
			Testdbg("0x%08X\n", *(ptr+3));

			Testdbg("DMA D TxDescriptor->SDP0 0x%08X\n", TxDescriptor->SDP0);
	}
		
	return ntStatus;
}

NTSTATUS 
DumpDMA(
	IN PDEVICE_OBJECT pDO,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	)
{
	NTSTATUS                ntStatus = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) pDO->DeviceExtension;
	PTXD_STRUC				TxDescriptor;
	PRXD_STRUC				RxDescriptor;
	ULONG					type,value;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DumpDMA  ===>\n"));

	*ByteCount = 0;

	type	= *(PULONG)(pIBuffer+3);						// Ring type
	value	 = *(PULONG)(pIBuffer+4);						// Ring index
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DumpDMA  ===>	Ring index = 0x%x\n", value));

	if(type == 0)
	{
		int i;
				
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac0RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac0RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac0RingMain[value].DmaBuf.AllocVa, AC0_BUFFER_SIZE);
		*ByteCount = AC0_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;

		//-----------
		// Debug
		for(i=26; i<1504-1; i++)
		{
			if( (UCHAR)(((UCHAR*)pDevExt->Ac0RingMain[value].DmaBuf.AllocVa)[i+1]) != (UCHAR)((UCHAR*)pDevExt->Ac0RingMain[value].DmaBuf.AllocVa)[i]+1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("INC: data[%d+1]!=data[%d]+1---------------\n", i, i));			
				DBGPRINT(RT_DEBUG_TRACE,("INC: data[%d+1]=0x%x\n", i, ((UCHAR*)pDevExt->Ac0RingMain[value].DmaBuf.AllocVa)[i+1]));							
				DBGPRINT(RT_DEBUG_TRACE,("INC: data[%d]=0x%x\n", i, ((UCHAR*)pDevExt->Ac0RingMain[value].DmaBuf.AllocVa)[i]));											
			}
		}
		
	}
	else if(type == RING_TYPE_AC1)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac1RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer,pDevExt->Ac1RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac1RingMain[value].DmaBuf.AllocVa, AC1_BUFFER_SIZE);
		*ByteCount = AC1_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_AC2)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac2RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac2RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac2RingMain[value].DmaBuf.AllocVa, AC2_BUFFER_SIZE);
		*ByteCount = AC2_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_AC3)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac3RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac3RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac3RingMain[value].DmaBuf.AllocVa, AC3_BUFFER_SIZE);
		*ByteCount = AC3_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_AC4)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac4RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac4RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac4RingMain[value].DmaBuf.AllocVa, AC4_BUFFER_SIZE);
		*ByteCount = AC4_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_AC5)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac5RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac5RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac5RingMain[value].DmaBuf.AllocVa, AC5_BUFFER_SIZE);
		*ByteCount = AC5_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_AC6)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac6RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac6RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac6RingMain[value].DmaBuf.AllocVa, AC6_BUFFER_SIZE);
		*ByteCount = AC6_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_AC7)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->Ac7RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->Ac7RingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->Ac7RingMain[value].DmaBuf.AllocVa, AC7_BUFFER_SIZE);
		*ByteCount = AC7_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_MGMT)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->MgmtRingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->MgmtRingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->MgmtRingMain[value].DmaBuf.AllocVa, MGMT_BUFFER_SIZE);
		*ByteCount = MGMT_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}
	else if(type == RING_TYPE_HCCA)
	{
		TxDescriptor = (PTXD_STRUC)(pDevExt->HccaRingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer, pDevExt->HccaRingMain[value].AllocVa, RING_TX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_TX_DESCRIPTOR_SIZE,pDevExt->HccaRingMain[value].DmaBuf.AllocVa, HCCA_BUFFER_SIZE);
		*ByteCount = HCCA_BUFFER_SIZE + RING_TX_DESCRIPTOR_SIZE;
	}	
	else if(type == RING_TYPE_RX0)
	{
		RxDescriptor = (PRXD_STRUC)(pDevExt->Rx0RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer,pDevExt->Rx0RingMain[value].AllocVa, RING_RX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_RX_DESCRIPTOR_SIZE, pDevExt->Rx0RingMain[value].DmaBuf.AllocVa, RX_BUFFER_SIZE);
		*ByteCount = RX_BUFFER_SIZE + RING_RX_DESCRIPTOR_SIZE;
	}	
	else if(type == RING_TYPE_RX1)
	{
		RxDescriptor = (PRXD_STRUC)(pDevExt->Rx1RingMain[value].AllocVa);
		RtlCopyMemory(UserBuffer,pDevExt->Rx1RingMain[value].AllocVa, RING_RX_DESCRIPTOR_SIZE);
		RtlCopyMemory(UserBuffer + RING_RX_DESCRIPTOR_SIZE, pDevExt->Rx1RingMain[value].DmaBuf.AllocVa, RX_BUFFER_SIZE);
		*ByteCount = RX_BUFFER_SIZE + RING_RX_DESCRIPTOR_SIZE;
	}	 

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: DumpDMA  <===\n"));
	return ntStatus;
}

NTSTATUS 
EraseDMA(
	IN PDEVICE_OBJECT pDO,
	IN PULONG pIBuffer,
    IN PUCHAR UserBuffer,
    IN ULONG BufferMaxSize,
    OUT PULONG ByteCount
	)
{
	NTSTATUS                ntStatus = STATUS_SUCCESS;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *) pDO->DeviceExtension;
	PRXD_STRUC				RxDescriptor;
	ULONG					i,value,datalen=0;

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: EraseDMA  ===>\n"));

	value	 = *(PULONG)(pIBuffer+16);						//value
	
	RtlZeroMemory(
				pDevExt->Rx0RingMain[value].AllocVa,
				pDevExt->Rx0RingMain[value].AllocSize);

	RxDescriptor					= (PRXD_STRUC)(pDevExt->Rx0RingMain[value].AllocVa);				
	RxDescriptor->SDP0 = pDevExt->Rx0RingMain[value].DmaBuf.AllocPa.LowPart;
	RxDescriptor->DDONE= 0x0;

	RtlZeroMemory(
				pDevExt->Rx0RingMain[value].DmaBuf.AllocVa,
				pDevExt->Rx0RingMain[value].DmaBuf.AllocSize);

	//++werner-->
	RtlZeroMemory(
				pDevExt->Rx1RingMain[value].AllocVa,
				pDevExt->Rx1RingMain[value].AllocSize);

	RxDescriptor					= (PRXD_STRUC)(pDevExt->Rx1RingMain[value].AllocVa);				
	RxDescriptor->SDP0 = pDevExt->Rx1RingMain[value].DmaBuf.AllocPa.LowPart;
	RxDescriptor->DDONE= 0x0;

	RtlZeroMemory(
				pDevExt->Rx1RingMain[value].DmaBuf.AllocVa,
				pDevExt->Rx1RingMain[value].DmaBuf.AllocSize);

	//++werner<--
	
	*ByteCount = 0; 

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: EraseDMA  <===\n"));
	return ntStatus;
}


//-------------------------------------------------------------------------------------------------------


void InitSequenceNumber()
{
	g_SequenceNumber=0;
	g_SequenceNumber_Ac0=0;
	g_SequenceNumber_Ac1=0;
	g_SequenceNumber_Ac2=0;
	g_SequenceNumber_Ac3=0;
	g_SequenceNumber_Ac4=0;
	g_SequenceNumber_Ac5=0;
	g_SequenceNumber_Ac6=0;
	g_SequenceNumber_Ac7=0;
	g_SequenceNumber_Ac8=0;
	g_SequenceNumber_Ac9=0;
	g_SequenceNumber_Ac10=0;
	g_SequenceNumber_Ac11=0;	
	g_SequenceNumber_Mgmt=0;
	g_SequenceNumber_Hcca=0;
}

void IncSequenceNumber(UCHAR* pDesBuffer, USHORT RingNum )
{
       /*USHORT* pSequenceNumber = (USHORT*)(pDesBuffer + (TXWI_SIZE+2+2+6+6+6)); //WI length+FC+Durattion+addr1+addr2+addr3
       //AsicDBGPRINT(RT_DEBUG_TRACE,("SN: *pSequenceNumber = 0x%04x\n", *pSequenceNumber);		

	if (RingNum == RING_TYPE_AC0)
	{
	    SEQdbg ("SN: g_SequenceNumber_Ac0 = 0x%04x\n", g_SequenceNumber_Ac0);
	    *pSequenceNumber = g_SequenceNumber_Ac0;
	    g_SequenceNumber_Ac0 = g_SequenceNumber_Ac0 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC1)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac1;
	    g_SequenceNumber_Ac1 = g_SequenceNumber_Ac1 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC2)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac2;
	    g_SequenceNumber_Ac2 = g_SequenceNumber_Ac2 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC3)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac3;
	    g_SequenceNumber_Ac3 = g_SequenceNumber_Ac3 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC4)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac4;
	    g_SequenceNumber_Ac4 = g_SequenceNumber_Ac4 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC5)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac5;
	    g_SequenceNumber_Ac5 = g_SequenceNumber_Ac5 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC6)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac6;
	    g_SequenceNumber_Ac6 = g_SequenceNumber_Ac6 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC7)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac7;
	    g_SequenceNumber_Ac7 = g_SequenceNumber_Ac7 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC8)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac8;
	    g_SequenceNumber_Ac8 = g_SequenceNumber_Ac8 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC9)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac9;
	    g_SequenceNumber_Ac9 = g_SequenceNumber_Ac9 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC10)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac10;
	    g_SequenceNumber_Ac10 = g_SequenceNumber_Ac10 + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_AC11)
	{
	    *pSequenceNumber = g_SequenceNumber_Ac11;
	    g_SequenceNumber_Ac11 = g_SequenceNumber_Ac11 + 16; //lower 4bits are fragment field
	}	
	else if (RingNum == RING_TYPE_MGMT)
	{
	    *pSequenceNumber = g_SequenceNumber_Mgmt;
	    g_SequenceNumber_Mgmt = g_SequenceNumber_Mgmt + 16; //lower 4bits are fragment field
	}
	else if (RingNum == RING_TYPE_HCCA)
	{
	    *pSequenceNumber = g_SequenceNumber_Hcca;
	    g_SequenceNumber_Hcca = g_SequenceNumber_Hcca + 16; //lower 4bits are fragment field
	}
	else if (RingNum == 10)
	{
	    *pSequenceNumber = g_SequenceNumber;
	    g_SequenceNumber = g_SequenceNumber + 16; //lower 4bits are fragment field
	}*/

}


//-------------------------------------------------------------------------------------------------------

USHORT PayloadLenRandom(RTMP_ADAPTER *pDevExt, USHORT min, USHORT max, TXD_STRUC* pTxD)
{
	USHORT rInMax, RetValue;
	
	rInMax = max - (min+1);
	RetValue =  ((RandomShort(pDevExt)%rInMax))+min;
	pDevExt->CurLength = RetValue;

	return 	RetValue;
}

#endif /* RTMP_PCI_SUPPORT */

VOID PayloadDataRandom(RTMP_ADAPTER *pDevExt, USHORT StartOffset, USHORT Length, UCHAR* pDesBuffer)
{
	int i;
	UCHAR *pucTemp;
	UCHAR ucInitValue;

	pucTemp = pDesBuffer + StartOffset;
	ucInitValue = RandomByte(pDevExt);
	
	for(i=0; i<Length; i++)
	{
		*pucTemp = (UCHAR)(ucInitValue+i);
		DBGPRINT(RT_DEBUG_TRACE,("SCT: 0x%x\n", *pucTemp));
		pucTemp++;
	}

	DBGPRINT(RT_DEBUG_TRACE,("SCT: Length = %d\n", Length));
	DBGPRINT(RT_DEBUG_TRACE,("SCT: ====\n"));
}


