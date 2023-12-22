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

extern ULONG	TXWI_SIZE;
//UCHAR	g_Pattern1[52];
//UCHAR	g_Pattern2[4000];
ULONG    g_ulTXPacketLength=0;

UCHAR	g_TxDPattern[32];
UCHAR	g_PayloadPattern[4000];

#ifdef RTMP_PCI_SUPPORT
NTSTATUS
RTPCIStartRX(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	)
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
	ULONG				type, Value;
	ULONG				channel, RX_CRX_IDX;
	KIRQL Irql;

	*ByteCount = 0;

	type = *(pIBuffer);								// type	0:stop	1:start	2:query
	channel = *(pIBuffer+1);						// Channel

	KeAcquireSpinLock(&pDevExt->RxRingMainLock, &Irql);

	if(type == RX_ACTION_RX_STOP)
	{
		DBGPRINT(RT_DEBUG_TRACE,("RTPCIStartRX() RX_ACTION_RX_STOP \n"));
//		Value = RTMP_Real_IO_READ32(pDevExt, RA_WPDMA_GLO_CFG); 
//		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, Value & (~RX_DMA_EN)) //disable rx dma
		pDevExt->bStartRx = FALSE;
		//Value =  RTMP_Real_IO_READ32(pDevExt, RA_INT_MASK);
		//RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, Value  & ~INT_Bit_ANY_RxDone );
	}
	else if( type == RX_ACTION_RX_START)
	{
		DBGPRINT(RT_DEBUG_TRACE,("RTPCIStartRX() RX_ACTION_RX_START \n"));

		//set RX0
		RTMP_IO_READ32(pDevExt, RA_FS_DRX_IDX0,&Value);
		
		if ( pDevExt->CurRx0Index  == Value)
		{
			if (pDevExt->CurRx0Index == 0)
				RX_CRX_IDX = RX0_RING_SIZE - 1;
			else
				RX_CRX_IDX = pDevExt->CurRx0Index -1;
			RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX0, RX_CRX_IDX);
		}

		//set RX1
		RTMP_IO_READ32(pDevExt, RA_FS_DRX_IDX1,&Value);
		
		if ( pDevExt->CurRx1Index  == Value)
		{
			if (pDevExt->CurRx1Index == 0)
				RX_CRX_IDX = RX1_RING_SIZE - 1;
			else
				RX_CRX_IDX = pDevExt->CurRx1Index -1;
			RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX1, RX_CRX_IDX);
		}

		pDevExt->bStartRx = TRUE;
//		Value = RTMP_Real_IO_READ32(pDevExt, RA_WPDMA_GLO_CFG); 
//		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG, Value |RX_DMA_EN); //Enable rx dma

//		Value =  RTMP_Real_IO_READ32(pDevExt, RA_INT_MASK );	
//		RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, Value  | INT_Bit_ANY_RxDone );
		//KeSetTimer(&pDevExt->m_IsrTimer, pDevExt->PollingInterval, &pDevExt->PollingDpc);
	}
	else if((type == RX_ACTION_QUERY) || (type == RX_ACTION_RESET))
	{// Query statistic counter and repeat conut
		DBGPRINT(RT_DEBUG_TRACE,("RTPCIStartRX() RX_ACTION_QUERY ||  RX_ACTION_RESET\n"));

		if ( type == RX_ACTION_RESET )
		{// Reset counter
			RtlZeroMemory(&pDevExt->WlanCounters, sizeof(COUNTER_802_11));

			RtlZeroMemory(&pDevExt->OtherCounters, sizeof(pDevExt->OtherCounters));

			/*pDevExt->OtherCounters.TransmittedCount = 0;
			pDevExt->OtherCounters.ReceivedCount = 0;
			pDevExt->OtherCounters.Ac0TxedCount = 0;
			pDevExt->OtherCounters.Ac1TxedCount = 0;
			pDevExt->OtherCounters.Ac2TxedCount = 0;
			pDevExt->OtherCounters.Ac3TxedCount = 0;
			pDevExt->OtherCounters.Ac4TxedCount = 0;
			pDevExt->OtherCounters.Ac5TxedCount = 0;
			pDevExt->OtherCounters.Ac6TxedCount = 0;
			pDevExt->OtherCounters.Ac7TxedCount = 0;
			pDevExt->OtherCounters.Ac8TxedCount = 0;
			pDevExt->OtherCounters.Ac9TxedCount = 0;
			pDevExt->OtherCounters.Ac10TxedCount = 0;
			pDevExt->OtherCounters.Ac11TxedCount = 0;			
			pDevExt->OtherCounters.MgmtTxedCount = 0;
			pDevExt->OtherCounters.HccaTxedCount = 0;

			pDevExt->OtherCounters.RSSI0 = 0;
			pDevExt->OtherCounters.RSSI1 = 0;
			pDevExt->OtherCounters.RSSI2 = 0;
			pDevExt->OtherCounters.SNR0 = 0;
			pDevExt->OtherCounters.SNR1 = 0;
			

			pDevExt->OtherCounters.UdpErr = 0;
			pDevExt->OtherCounters.TcpErr = 0;
			pDevExt->OtherCounters.IpErr = 0;
			pDevExt->RX0DataCount = 0;*/
		}

		if(pDevExt->bStartRx == TRUE)
		{
			LARGE_INTEGER	tmp;
			ULONG			TempVal;

			// Read RX OverFlow error count
			Value = 0;
			//Value = RTMP_Real_IO_READ32(pDevExt, RA_RX_STA_CNT2);
			TempVal = (Value & 0xffff0000)>>16;

			if(TempVal > 0)
			{
				tmp.LowPart = TempVal;
				tmp.HighPart = 0;
				pDevExt->WlanCounters.RXOverFlowCount = RtlLargeIntegerAdd(pDevExt->WlanCounters.RXOverFlowCount, tmp);

				if(pDevExt->WlanCounters.RXOverFlowCount.LowPart == 0)
					pDevExt->WlanCounters.RXOverFlowCount.HighPart++;
			}

			// Read PHY error count
			{
				//Value = RTMP_Real_IO_READ32(pDevExt, RA_RX_STA_CNT0);
				TempVal = (Value & 0xffff0000)>>16;

				if(TempVal > 0)
				{
					tmp.LowPart = TempVal;
					tmp.HighPart = 0;
					pDevExt->WlanCounters.PHYErrorCount = RtlLargeIntegerAdd(pDevExt->WlanCounters.PHYErrorCount, tmp);

					if(pDevExt->WlanCounters.PHYErrorCount.LowPart == 0)
						pDevExt->WlanCounters.PHYErrorCount.HighPart++;
				}

                           // Read CRC error count
				TempVal = (Value & 0x0000ffff);

				if(TempVal > 0)
				{
					tmp.LowPart = TempVal;
					tmp.HighPart = 0;
					pDevExt->WlanCounters.CRCErrorCount= RtlLargeIntegerAdd(pDevExt->WlanCounters.CRCErrorCount, tmp);

					if(pDevExt->WlanCounters.CRCErrorCount.LowPart == 0)
						pDevExt->WlanCounters.CRCErrorCount.HighPart++;
				}

				// Read CCA error count
				//Value = RTMP_Real_IO_READ32(pDevExt, RA_RX_STA_CNT1);
				TempVal = (Value & 0x0000ffff);

				if(TempVal > 0)
				{
					tmp.LowPart = TempVal;
					tmp.HighPart = 0;
					pDevExt->WlanCounters.FalseCCACount = RtlLargeIntegerAdd(pDevExt->WlanCounters.FalseCCACount, tmp);

					if(pDevExt->WlanCounters.FalseCCACount.LowPart == 0)
						pDevExt->WlanCounters.FalseCCACount.HighPart++;
				}
			}
		}

		RtlCopyMemory(UserBuffer, &pDevExt->WlanCounters, sizeof(COUNTER_802_11));
		RtlCopyMemory(UserBuffer + sizeof(COUNTER_802_11), &pDevExt->OtherCounters, sizeof(OTHER_STATISTICS));

		*ByteCount = sizeof(COUNTER_802_11) + sizeof(OTHER_STATISTICS);
	}

#ifdef	SNIFF
	else if(type == RX_ACTION_SNIFF_START)
	{// RX_ACTION_SNIFF_START

		DBGPRINT(RT_DEBUG_TRACE,("RTPCIStartRX() RX_ACTION_SNIFF_START\n"));

		RtlZeroMemory(&pDevExt->WlanCounters, sizeof(COUNTER_802_11));
#ifdef	AUTO_RESPONDER
		{// only for Auto responder testing
		#if 0 //mark by simon	
			pDevExt->IntrMask = (~IRQ_MASK)&0xFFFFFFFF;
			RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, pDevExt->IntrMask|IRQ_MSK_SPECIAL_BITS);
		#endif
		}
#endif

		if(pDevExt->pPacketBuf)
			RtlZeroMemory(pDevExt->pPacketBuf,(RX_RING_PACKET_BUFFER * MAX_FRAME_SIZE));

		pDevExt->PacketBufferDriverIdx = 0;
		pDevExt->PacketBufferSnifferIdx = 0;
		pDevExt->PacketBufferWrapAround = FALSE;

		pDevExt->bStartSniff = TRUE;
		pDevExt->bStartRx = TRUE;
		//set RX0
		RTMP_IO_READ32(pDevExt,RA_FS_DRX_IDX0,&Value); 
		if ( pDevExt->CurRx0Index  == Value)
		{
		     if (pDevExt->CurRx0Index == 0)
                       RX_CRX_IDX = RX0_RING_SIZE - 1;
                   else
                       RX_CRX_IDX = pDevExt->CurRx0Index -1;
                   RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX0, RX_CRX_IDX);
		}


		//set RX1
		RTMP_IO_READ32(pDevExt,RA_FS_DRX_IDX1,&Value); 
		if ( pDevExt->CurRx1Index  == Value)
		{
		     if (pDevExt->CurRx1Index == 0)
                       RX_CRX_IDX = RX1_RING_SIZE - 1;
                   else
                       RX_CRX_IDX = pDevExt->CurRx1Index -1;
                   RTMP_Real_IO_WRITE32(pDevExt, RA_RX_CALC_IDX1, RX_CRX_IDX);
		}

//		Value = RTMP_Real_IO_READ32(pDevExt, RA_WPDMA_GLO_CFG); 
//		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG,Value |RX_DMA_EN); //Enable rx dma

//		Value =  RTMP_Real_IO_READ32(pDevExt, RA_INT_MASK);
//		RTMP_Real_IO_WRITE32(pDevExt,RA_INT_MASK, Value  | INT_Bit_ANY_RxDone );
	}
	else if(type == RX_ACTION_SNIFF_STOP)
	{// RX_ACTION_SNIFF_STOP
		DBGPRINT(RT_DEBUG_TRACE,("RTPCIStartRX() RX_ACTION_SNIFF_STOP\n"));

		pDevExt->bStartSniff = FALSE;
		pDevExt->ulTimestamp = 0;

		//---------------
		// BbpStopRx
		//RTMP_Real_IO_WRITE32(pDevExt, RA_MAC_CSR10, 0x00000018);

		/*marked for pseudo MAC-TX/RX-loopback*/
//		pDevExt->bStartRx = FALSE;
//		Value = RTMP_Real_IO_READ32(pDevExt, RA_WPDMA_GLO_CFG); 
//		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG,Value);
//		RTMP_Real_IO_WRITE32(pDevExt, RA_WPDMA_GLO_CFG,Value & ~RX_DMA_EN); //Enable rx dma
	}
#endif

	KeReleaseSpinLock(&pDevExt->RxRingMainLock, Irql);

	return	STATUS_SUCCESS;
}


NTSTATUS
RTPCIStartTX(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	)
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
	ULONG					type, index,value;
	PTXD_STRUC				TxDescriptor;	
	PTX_WI_STRUC			pTxWI;
	PTX_WI_STRUC			pLoopbackTxWI;
	PRXD_STRUC				RxDescriptor;
	ULONG					SchReg4;
//	INTSRC_STRUC				IntSource;
	ULONG		Tmp_Length;
	int			i=0, j=0, k=0, retry = 10000;
	USHORT		PayloadLen=0;
	PUCHAR pPayload=0;

	int FirstTxCount = 0;
	int FillSingleRingNum=0;
	int bNeedBreak=0;
	int iRingNumber = 0;
	int iSendIndex = 0;
	ULONG iPattern2Location = 0;
	int iPrintTXByteCount=0;
	int pringMaxlen =0x60;
	int IsSaveRXVLog = 0;
	NTSTATUS ntstatus = 0;
	RX_V_GROUP1 group1;
	UNICODE_STRING     uniName;
    	OBJECT_ATTRIBUTES  objAttr;
	IO_STATUS_BLOCK    ioStatusBlock;

	DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() StartTx\n"));

	type = *(pIBuffer);								// type	0:stop	1:start	2:query	3:Just Kick Tx and disable Interrupt
	pDevExt->FrameType = *(pIBuffer+1);						// Channel
	pDevExt->IsSaveRXVLog = *(pIBuffer+2);//Is Save RXV Log
	pDevExt->MaxLength = *(pIBuffer+3);				// Max Length
	pDevExt->MinLength = *(pIBuffer+4);				// min Length
	pDevExt->RingType = *(pIBuffer+5);				// Ring Type	
	memset(&group1,0,sizeof(group1));

	//if(pDevExt->IsSaveRXVLog)
	//{
	/*	RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\WINDOWS\\Mediatek.csv");  // or L"\\SystemRoot\\example.txt"
	   	InitializeObjectAttributes(&objAttr, &uniName,
	                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
	                               NULL, NULL);

		if(KeGetCurrentIrql() != PASSIVE_LEVEL)
       	{
       		DBGPRINT(RT_DEBUG_TRACE,("%s create RXV log file fail !!!!\n",__FUNCTION__));
		}
		else
		{
			ntstatus = ZwCreateFile(&pDevExt->RXVFileHandle,
                             FILE_APPEND_DATA,
                            &objAttr, &ioStatusBlock, NULL,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_WRITE,
                            FILE_OPEN_IF, 
                            FILE_SYNCHRONOUS_IO_NONALERT,
                            NULL, 0);
		}


		WriteRXVToFile(pDevExt->RXVFileHandle,NULL,0);
	*/
	//}
       

	//if (pDevExt->RingType == 0x10)
	//	pDevExt->FirmwarePktCount = *(pIBuffer+2);			// Tx Repeat Count
	//else
		pDevExt->OtherCounters.TxRepeatCount = *(pIBuffer+2);			// Tx Repeat Count
	
	DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() StartTx pDevExt->OtherCounters.TxRepeatCount=%X, minlength=%X, pDevExt->RingType=%X\n",pDevExt->OtherCounters.TxRepeatCount, pDevExt->MinLength, pDevExt->RingType));		
	DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() type=%X\n",type));		
	if ( type == TX_START)
	{
		switch(pDevExt->FrameType)
		{
			case QA_COMBO_FRAME_TYPE_Qos_Data:
				break;
			default:
				;
		}

		// Use AC1 for firmware command
		/*if (pDevExt->RingType == 0x10)
		{
//			value = RTMP_Real_IO_READ32(pDevExt, RA_TX_DTX_IDX12);
			pDevExt->CurMgmtTxIndex =value;
			pDevExt->NextMgmtTxDoneIndex = value;
		}
		else
		{*/
			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX0,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc0TxIndex =value;
				pDevExt->NextAc0TxDoneIndex = value;
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() read RA_TX_DTX_IDX0 =0x%X\n",value));		

			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX1,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc1TxIndex =value;
				pDevExt->NextAc1TxDoneIndex = value;
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() read RA_TX_DTX_IDX1 =0x%X\n",value));		

			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX2,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc2TxIndex =value;
				pDevExt->NextAc2TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX3,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc3TxIndex =value;
				pDevExt->NextAc3TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX4,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc4TxIndex =value;
				pDevExt->NextAc4TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX5,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc5TxIndex =value;
				pDevExt->NextAc5TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX6,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc6TxIndex =value;
				pDevExt->NextAc6TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX7,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc7TxIndex =value;
				pDevExt->NextAc7TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX8,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc8TxIndex =value;
				pDevExt->NextAc8TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX9,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc9TxIndex =value;
				pDevExt->NextAc9TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX10,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc10TxIndex =value;
				pDevExt->NextAc10TxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX11,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurAc11TxIndex =value;
				pDevExt->NextAc11TxDoneIndex = value;
			}
			
			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX12,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurMgmtTxIndex =value;
				pDevExt->NextMgmtTxDoneIndex = value;
			}

			RTMP_IO_READ32(pDevExt, RA_TX_DTX_IDX13,&value);
			if(value!=0xFFFFFFFF)
			{//sometime HW error, may read value is 0xFFFFFFFF
				pDevExt->CurHccaTxIndex =value;
				pDevExt->NextHccaTxDoneIndex = value;
			}

			
			
			
			FirstTxCount = 0;
			FillSingleRingNum=0;
			bNeedBreak=0;
			if ( pDevExt->RingType&0x00003FFF )
			{
				if(pDevExt->RingType&0x00000001) FirstTxCount+=(AC0_RING_SIZE -1);
				if(pDevExt->RingType&0x00000002) FirstTxCount+=(AC1_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000004) FirstTxCount+=(AC2_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000008) FirstTxCount+=(AC3_RING_SIZE - 1);

				if(pDevExt->RingType&0x00000010) FirstTxCount+=(AC4_RING_SIZE -1);
				if(pDevExt->RingType&0x00000020) FirstTxCount+=(AC5_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000040) FirstTxCount+=(AC6_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000080) FirstTxCount+=(AC7_RING_SIZE - 1);

				if(pDevExt->RingType&0x00000100) FirstTxCount+=(AC8_RING_SIZE -1);
				if(pDevExt->RingType&0x00000200) FirstTxCount+=(AC9_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000400) FirstTxCount+=(AC10_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000800) FirstTxCount+=(AC11_RING_SIZE - 1);

				if(pDevExt->RingType&0x00001000) FirstTxCount+=(MGMT_RING_SIZE -1);
				if(pDevExt->RingType&0x00002000) FirstTxCount+=(HCCA_RING_SIZE - 1);
				
			}
			/*if ( pDevExt->RingType&0x0000000F )
			{
				if(pDevExt->RingType&0x00000001) FirstTxCount+=(AC0_RING_SIZE -1);
				if(pDevExt->RingType&0x00000002) FirstTxCount+=(AC1_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000004) FirstTxCount+=(AC2_RING_SIZE - 1);
				if(pDevExt->RingType&0x00000008) FirstTxCount+=(AC3_RING_SIZE - 1);
			}
			else if ( pDevExt->RingType&0x000F0000 )
			{
				if(pDevExt->RingType&0x00010000) FirstTxCount+=(AC4_RING_SIZE - 1);
				if(pDevExt->RingType&0x00020000) FirstTxCount+=(AC5_RING_SIZE - 1);
				if(pDevExt->RingType&0x00040000) FirstTxCount+=(AC6_RING_SIZE - 1);
				if(pDevExt->RingType&0x00080000) FirstTxCount+=(AC7_RING_SIZE - 1);
			}
			else	if(pDevExt->RingType&0x00000010) FirstTxCount+=(MGMT_RING_SIZE - 1);
			else	if(pDevExt->RingType&0x00000020) FirstTxCount+=(HCCA_RING_SIZE - 1);*/
			
			DBGPRINT(RT_DEBUG_TRACE,("1 FirstTxCount = %d\n", FirstTxCount));
		//}

		// Use AC8 for firmware command
		/*if (pDevExt->RingType == 0x10)
		{
			FirstTxCount = 7;
			FillSingleRingNum=0;
			bNeedBreak=0;
			
			//RING_TYPE_Mgmt
			if(pDevExt->RingType&0x00000010)
			{
				if(pDevExt->FirmwarePktCount == 0)
				{
					pDevExt->FirmwarePktCount = 0xFFFFFFFF;
				}
				else if(pDevExt->FirmwarePktCount > (ULONG)FirstTxCount)
				{
					pDevExt->FirmwarePktCount = pDevExt->FirmwarePktCount - (FirstTxCount);
				}
				else
				{
					FirstTxCount = pDevExt->FirmwarePktCount;
					pDevExt->FirmwarePktCount = 0;
				}
			
				if( (FirstTxCount - MGMT_RING_SIZE) > 0 )
				{
					FillSingleRingNum = MGMT_RING_SIZE - 1;
				}			
				else if((FirstTxCount - MGMT_RING_SIZE) == 0)
				{
					FillSingleRingNum = MGMT_RING_SIZE - 1;
					//bNeedBreak = 1;
				}
				else if((FirstTxCount - MGMT_RING_SIZE) < 0)
				{
					FillSingleRingNum = FirstTxCount;
					bNeedBreak = 1;			
				}

				for(i=0; i<FillSingleRingNum; i++)
				{
					DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_Mgmt Own[0x%02x] = 1", (pDevExt->CurMgmtTxIndex)%MGMT_RING_SIZE);
					TxDescriptor = (PTXD_STRUC)	pDevExt->MgmtRingMain[(pDevExt->CurMgmtTxIndex)%MGMT_RING_SIZE].AllocVa;
					
					TxDescriptor->DDONE= 0;									
					pDevExt->CurMgmtTxIndex++;
					if  (pDevExt->CurMgmtTxIndex >= MGMT_RING_SIZE)
						pDevExt->CurMgmtTxIndex = 0;
				}

				pDevExt->Mgmt_MAX_TX_PROCESS = FillSingleRingNum;
						
				FirstTxCount -= FillSingleRingNum;
				//if(bNeedBreak == 1)
				//{
				//	DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount);
				//	goto start_transmit;
				//}			

			}
		
			RTMP_Real_IO_WRITE32(pDevExt, RA_TX_CTX_IDX8, pDevExt->CurMgmtTxIndex );
			DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurMgmtTxIndex = %x\n", pDevExt->CurMgmtTxIndex);

//			value =  RTMP_Real_IO_READ32(pDevExt, RA_INT_MASK );
//			RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, value | INT_Bit_ANY_TxDone | INT_Bit_ANY_RxDone);	

			return	STATUS_SUCCESS;
		}
		else
		{*/		
			DBGPRINT(RT_DEBUG_TRACE,("TxRepeatCount = %d\n", pDevExt->OtherCounters.TxRepeatCount));
			if(pDevExt->OtherCounters.TxRepeatCount == 0)
			{
				pDevExt->OtherCounters.TxRepeatCount = 0xFFFFFFFF;
			}
			else if(pDevExt->OtherCounters.TxRepeatCount > (ULONG)FirstTxCount)
			{
				pDevExt->OtherCounters.TxRepeatCount = pDevExt->OtherCounters.TxRepeatCount - (FirstTxCount);
			}
			else
			{
				FirstTxCount = pDevExt->OtherCounters.TxRepeatCount;
				pDevExt->OtherCounters.TxRepeatCount = 0;
			}
		//}

		DBGPRINT(RT_DEBUG_TRACE,("2 FirstTxCount = %d\n", FirstTxCount));
		//RING_TYPE_AC0
		if(pDevExt->RingType&0x00000001)
		{
			PUCHAR			pData;	
			PHEADER_802_11	pHeader;

			iRingNumber = 0;			
			if( (FirstTxCount - AC0_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC0_RING_SIZE - 1;
			}
			else if((FirstTxCount - AC0_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC0_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC0_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum ; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC0 Own[0x%02x] = 1", (pDevExt->CurAc0TxIndex)%AC0_RING_SIZE));
				DBGPRINT(RT_DEBUG_TRACE,("TXT: pDevExt->FrameType = %d", pDevExt->FrameType));
				DBGPRINT(RT_DEBUG_TRACE,("TXT: TxDescriptor addr = %x", pDevExt->Ac0RingMain[(pDevExt->CurAc0TxIndex)%AC0_RING_SIZE].AllocVa));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac0RingMain[(pDevExt->CurAc0TxIndex)%AC0_RING_SIZE].AllocVa;

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;

					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE - 2;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }
					       break;

					default:
						;
				}

				TxDescriptor->DDONE= 0;

				pDevExt->CurAc0TxIndex++;
				if  (pDevExt->CurAc0TxIndex >= AC0_RING_SIZE)
					pDevExt->CurAc0TxIndex = 0;
			}

			pData = (PUCHAR) (pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa);
		       // Cast to 802.11 header for flags checking

			pDevExt->Ac0_MAX_TX_PROCESS = FillSingleRingNum;
	
			FirstTxCount -= FillSingleRingNum;	
			if(pDevExt->CurAc0TxIndex)
				iSendIndex = pDevExt->CurAc0TxIndex-1;
			else
				iSendIndex = AC0_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}
		}

		//RING_TYPE_AC1
		if(pDevExt->RingType&0x00000002)
		{
			iRingNumber = 1;
			if( (FirstTxCount - AC1_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC1_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC1_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC1_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC1_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}
			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC1 Own[0x%02x] = 1", (pDevExt->CurAc1TxIndex%AC1_RING_SIZE)  ));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac1RingMain[(pDevExt->CurAc1TxIndex)%AC1_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc1TxIndex++;
				if  (pDevExt->CurAc1TxIndex >= AC1_RING_SIZE)
					pDevExt->CurAc1TxIndex = 0;

			}

			pDevExt->Ac1_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc1TxIndex)
				iSendIndex = pDevExt->CurAc1TxIndex-1;
			else
				iSendIndex = AC1_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}			

		}

#if 1
		//RING_TYPE_AC2
		if(pDevExt->RingType&0x00000004)
		{
			iRingNumber = 2;
			if( (FirstTxCount - AC2_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC2_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC2_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC2_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC2_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC2 Own[0x%02x] = 1", (pDevExt->CurAc2TxIndex)%AC2_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac2RingMain[(pDevExt->CurAc2TxIndex)%AC2_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;		
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc2TxIndex++;
				if  (pDevExt->CurAc2TxIndex >= AC2_RING_SIZE)
					pDevExt->CurAc2TxIndex = 0;

			}

			pDevExt->Ac2_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;
			if(pDevExt->CurAc2TxIndex)
				iSendIndex = pDevExt->CurAc2TxIndex-1;
			else
				iSendIndex = AC2_RING_SIZE-1;
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}			

		}

		//RING_TYPE_AC3
		if(pDevExt->RingType&0x00000008)
		{
			iRingNumber = 3;
			if( (FirstTxCount - AC3_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC3_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC3_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC3_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC3_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC3 Own[0x%02x] = 1", (pDevExt->CurAc3TxIndex)%AC3_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac3RingMain[(pDevExt->CurAc3TxIndex)%AC3_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc3TxIndex++;
				if  (pDevExt->CurAc3TxIndex >= AC3_RING_SIZE)
					pDevExt->CurAc3TxIndex = 0;

			}

			pDevExt->Ac3_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;
			
			if(pDevExt->CurAc3TxIndex)
				iSendIndex = pDevExt->CurAc3TxIndex-1;
			else
				iSendIndex = AC3_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}
		}

		//RING_TYPE_AC4
		//if(pDevExt->RingType&0x00010000)
		if(pDevExt->RingType&0x00000010)
		{
			iRingNumber = 4;
			if( (FirstTxCount - AC4_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC4_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC4_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC4_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC4_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC4 Own[0x%02x] = 1", (pDevExt->CurAc4TxIndex)%AC4_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac4RingMain[(pDevExt->CurAc4TxIndex)%AC4_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc4TxIndex++;
				if  (pDevExt->CurAc4TxIndex >= AC4_RING_SIZE)
					pDevExt->CurAc4TxIndex = 0;

			}

			pDevExt->Ac4_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc4TxIndex)
				iSendIndex = pDevExt->CurAc4TxIndex-1;
			else
				iSendIndex = AC4_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}
		}

		//RING_TYPE_AC5
		//if(pDevExt->RingType&0x00020000)
		if(pDevExt->RingType&0x00000020)
		{
			iRingNumber = 5;
			if( (FirstTxCount - AC5_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC5_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC5_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC5_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC5_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC5 Own[0x%02x] = 1", (pDevExt->CurAc5TxIndex)%AC5_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac5RingMain[(pDevExt->CurAc5TxIndex)%AC5_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc5TxIndex++;
				if  (pDevExt->CurAc5TxIndex >= AC5_RING_SIZE)
					pDevExt->CurAc5TxIndex = 0;

			}

			pDevExt->Ac5_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc5TxIndex)
				iSendIndex = pDevExt->CurAc5TxIndex-1;
			else
				iSendIndex = AC5_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}
		}

		//RING_TYPE_AC6
		//if(pDevExt->RingType&0x00040000)
		if(pDevExt->RingType&0x00000040)
		{
			iRingNumber = 6;
			if( (FirstTxCount - AC6_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC6_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC6_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC6_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC6_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC6 Own[0x%02x] = 1", (pDevExt->CurAc6TxIndex)%AC6_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac6RingMain[(pDevExt->CurAc6TxIndex)%AC6_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc6TxIndex++;
				if  (pDevExt->CurAc6TxIndex >= AC6_RING_SIZE)
					pDevExt->CurAc6TxIndex = 0;

			}

			pDevExt->Ac6_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc6TxIndex)
				iSendIndex = pDevExt->CurAc6TxIndex-1;
			else
				iSendIndex = AC6_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}
		}

		//RING_TYPE_AC7
		//if(pDevExt->RingType&0x00080000)
		if(pDevExt->RingType&0x00000080)
		{	
			iRingNumber = 7;
			if( (FirstTxCount - AC7_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC7_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC7_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC7_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC7_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC7 Own[0x%02x] = 1", (pDevExt->CurAc7TxIndex)%AC7_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac7RingMain[(pDevExt->CurAc7TxIndex)%AC7_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc7TxIndex++;
				if  (pDevExt->CurAc7TxIndex >= AC7_RING_SIZE)
					pDevExt->CurAc7TxIndex = 0;

			}

			pDevExt->Ac7_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc7TxIndex)
				iSendIndex = pDevExt->CurAc7TxIndex-1;
			else
				iSendIndex = AC7_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}
		}

		//RING_TYPE_AC8
		//if(pDevExt->RingType&0x00080000)
		if(pDevExt->RingType&0x00000100)
		{
			iRingNumber = 8;
			if( (FirstTxCount - AC8_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC8_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC8_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC8_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC8_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC8 Own[0x%02x] = 1", (pDevExt->CurAc8TxIndex)%AC8_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac8RingMain[(pDevExt->CurAc8TxIndex)%AC8_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac8RingMain[pDevExt->CurAc8TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac8RingMain[pDevExt->CurAc8TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac8RingMain[pDevExt->CurAc8TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc8TxIndex++;
				if  (pDevExt->CurAc8TxIndex >= AC8_RING_SIZE)
					pDevExt->CurAc8TxIndex = 0;

			}

			pDevExt->Ac8_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc8TxIndex)
				iSendIndex = pDevExt->CurAc8TxIndex-1;
			else
				iSendIndex = AC8_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac8RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}

		}


		//RING_TYPE_AC9
		//if(pDevExt->RingType&0x00080000)
		if(pDevExt->RingType&0x00000200)
		{
			iRingNumber = 9;
			if( (FirstTxCount - AC9_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC9_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC9_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC9_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC9_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC9 Own[0x%02x] = 1", (pDevExt->CurAc9TxIndex)%AC9_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac9RingMain[(pDevExt->CurAc9TxIndex)%AC9_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac9RingMain[pDevExt->CurAc9TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac9RingMain[pDevExt->CurAc9TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac9RingMain[pDevExt->CurAc9TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc9TxIndex++;
				if  (pDevExt->CurAc9TxIndex >= AC9_RING_SIZE)
					pDevExt->CurAc9TxIndex = 0;

			}

			pDevExt->Ac9_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc9TxIndex)
				iSendIndex = pDevExt->CurAc9TxIndex-1;
			else
				iSendIndex = AC9_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac9RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}

		}

		//RING_TYPE_AC10
		if(pDevExt->RingType&0x00000400)
		{
			iRingNumber = 10;
			if( (FirstTxCount - AC10_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC10_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC10_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC10_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC10_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC10 Own[0x%02x] = 1", (pDevExt->CurAc10TxIndex)%AC10_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac10RingMain[(pDevExt->CurAc10TxIndex)%AC10_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac10RingMain[pDevExt->CurAc10TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac10RingMain[pDevExt->CurAc10TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac10RingMain[pDevExt->CurAc10TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc10TxIndex++;
				if  (pDevExt->CurAc10TxIndex >= AC10_RING_SIZE)
					pDevExt->CurAc10TxIndex = 0;

			}

			pDevExt->Ac10_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc10TxIndex)
				iSendIndex = pDevExt->CurAc10TxIndex-1;
			else
				iSendIndex = AC10_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac10RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}

		}

		//RING_TYPE_AC11
		//if(pDevExt->RingType&0x00080000)
		if(pDevExt->RingType&0x00000800)
		{
			iRingNumber = 11;
			if( (FirstTxCount - AC11_RING_SIZE) > 0 )
			{
				FillSingleRingNum = AC11_RING_SIZE - 1;
			}			
			else if((FirstTxCount - AC11_RING_SIZE) == 0)
			{
				FillSingleRingNum = AC11_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - AC11_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_AC11 Own[0x%02x] = 1", (pDevExt->CurAc11TxIndex)%AC11_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->Ac11RingMain[(pDevExt->CurAc11TxIndex)%AC11_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac11RingMain[pDevExt->CurAc11TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->Ac11RingMain[pDevExt->CurAc11TxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->Ac11RingMain[pDevExt->CurAc11TxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurAc11TxIndex++;
				if  (pDevExt->CurAc11TxIndex >= AC11_RING_SIZE)
					pDevExt->CurAc11TxIndex = 0;

			}

			pDevExt->Ac11_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurAc11TxIndex)
				iSendIndex = pDevExt->CurAc11TxIndex-1;
			else
				iSendIndex = AC11_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->Ac11RingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}

		}


		//RING_TYPE_AC12, MGMT
		if(pDevExt->RingType&0x00001000)
		{
			iRingNumber = 12;
			if( (FirstTxCount - MGMT_RING_SIZE) > 0 )
			{
				FillSingleRingNum = MGMT_RING_SIZE - 1;
			}			
			else if((FirstTxCount - MGMT_RING_SIZE) == 0)
			{
				FillSingleRingNum = MGMT_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - MGMT_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_MGMT Own[0x%02x] = 1", (pDevExt->CurMgmtTxIndex)%MGMT_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->MgmtRingMain[(pDevExt->CurMgmtTxIndex)%MGMT_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurMgmtTxIndex++;
				if  (pDevExt->CurMgmtTxIndex >= MGMT_RING_SIZE)
					pDevExt->CurMgmtTxIndex = 0;

			}

			pDevExt->Mgmt_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurMgmtTxIndex)
				iSendIndex = pDevExt->CurMgmtTxIndex-1;
			else
				iSendIndex = MGMT_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}

		}


		
		//RING_TYPE_AC13, HCCA
		if(pDevExt->RingType&0x00002000)
		{
			iRingNumber = 13;
			if( (FirstTxCount - HCCA_RING_SIZE) > 0 )
			{
				FillSingleRingNum = HCCA_RING_SIZE - 1;
			}			
			else if((FirstTxCount - HCCA_RING_SIZE) == 0)
			{
				FillSingleRingNum = HCCA_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - HCCA_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}

			DBGPRINT(RT_DEBUG_TRACE,("FillSingleRingNum = %d\n", FillSingleRingNum));
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_HCCA Own[0x%02x] = 1", (pDevExt->CurHccaTxIndex)%HCCA_RING_SIZE));
				TxDescriptor = (PTXD_STRUC)	pDevExt->HccaRingMain[(pDevExt->CurHccaTxIndex)%HCCA_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0));
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurHccaTxIndex++;
				if  (pDevExt->CurHccaTxIndex >= HCCA_RING_SIZE)
					pDevExt->CurHccaTxIndex = 0;

			}

			pDevExt->Hcca_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;

			if(pDevExt->CurHccaTxIndex)
				iSendIndex = pDevExt->CurHccaTxIndex-1;
			else
				iSendIndex = HCCA_RING_SIZE-1;
			
			pLoopbackTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[iSendIndex].DmaBuf.AllocVa;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount));
				goto start_transmit;
			}

		}



#if 0
		if(pDevExt->RingType&0x00000010)
		{
	
			if( (FirstTxCount - MGMT_RING_SIZE) > 0 )
			{
				FillSingleRingNum = MGMT_RING_SIZE - 1;
			}			
			else if((FirstTxCount - MGMT_RING_SIZE) == 0)
			{
				FillSingleRingNum = MGMT_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - MGMT_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_Mgmt Own[0x%02x] = 1", (pDevExt->CurMgmtTxIndex)%MGMT_RING_SIZE);
				TxDescriptor = (PTXD_STRUC)	pDevExt->MgmtRingMain[(pDevExt->CurMgmtTxIndex)%MGMT_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
						pTxWI->MPDU_SIZE = Tmp_Length;						
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0);
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
						pTxWI->MPDU_SIZE = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
						    pTxWI->MPDU_SIZE = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurMgmtTxIndex++;
				if  (pDevExt->CurMgmtTxIndex >= MGMT_RING_SIZE)
					pDevExt->CurMgmtTxIndex = 0;

			}

			pDevExt->Mgmt_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount);
				goto start_transmit;
			}			

		}
#endif
		/*if(pDevExt->RingType&0x00000020)
		{
	
			if( (FirstTxCount - HCCA_RING_SIZE) > 0 )
			{
				FillSingleRingNum = HCCA_RING_SIZE - 1;
			}			
			else if((FirstTxCount - HCCA_RING_SIZE) == 0)
			{
				FillSingleRingNum = HCCA_RING_SIZE - 1;
				//bNeedBreak = 1;
			}
			else if((FirstTxCount - HCCA_RING_SIZE) < 0)
			{
				FillSingleRingNum = FirstTxCount;
				bNeedBreak = 1;			
			}
			for(i=0; i<FillSingleRingNum; i++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXT: RING_TYPE_Hcca Own[0x%02x] = 1", (pDevExt->CurHccaTxIndex)%HCCA_RING_SIZE);
				TxDescriptor = (PTXD_STRUC)	pDevExt->HccaRingMain[(pDevExt->CurHccaTxIndex)%HCCA_RING_SIZE].AllocVa;			

				switch(pDevExt->FrameType)
				{
					case QA_COMBO_FRAME_TYPE_User1_Random_length:
						TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						if ( TxDescriptor->SDL0 < 50)
						{
							TxDescriptor->SDL0 += 50;
						}
						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						DBGPRINT(RT_DEBUG_TRACE,("Random_len: SDL0 = %x\n",TxDescriptor->SDL0);
						break;
						
					case QA_COMBO_FRAME_TYPE_User2_Inc_length:		// Inc length
						pDevExt->CurLength = pDevExt->MinLength + i;		
						TxDescriptor->SDL0  =  pDevExt->CurLength;
						if(TxDescriptor->SDL0 < 44)	
						{
							TxDescriptor->SDL0 = 44;
						}

						Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
						//pTxWI->MPDU_SIZE = Tmp_Length;	
						pTxWI->TXByteCount = Tmp_Length;
						break;
						
					case QA_COMBO_FRAME_TYPE_User4_Data_CRC:						
						break;

					case QA_COMBO_FRAME_TYPE_Qos_Data:	
					       if(pDevExt->RingType&0x01000000) 
					       {
					           TxDescriptor->SDL0= (((RandomByte(pDevExt) << 8) + RandomByte(pDevExt)) % 1500);
						    if ( TxDescriptor->SDL0 < 50)
						    {
							    TxDescriptor->SDL0 += 50;
						    }
						    Tmp_Length =  TxDescriptor->SDL0 - TXWI_SIZE;
						    pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;
						    //pTxWI->MPDU_SIZE = Tmp_Length;
						    pTxWI->TXByteCount = Tmp_Length;
					       }						   
					       break;
						   
					default:
						;
				
				}
				
				TxDescriptor->DDONE= 0;									
				pDevExt->CurHccaTxIndex++;
				if  (pDevExt->CurHccaTxIndex >= HCCA_RING_SIZE)
					pDevExt->CurHccaTxIndex = 0;

			}

			pDevExt->Hcca_MAX_TX_PROCESS = FillSingleRingNum;
					
			FirstTxCount -= FillSingleRingNum;
			if(bNeedBreak == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE,("FirstTxCount need cnt to zero, check value = 0x%x", FirstTxCount);
				goto start_transmit;
			}			

		}*/




#endif
start_transmit:
//werner debug->
		for(i=0;i<=RING_TYPE_HCCA;i++)
		{
			PULONG	ptr;

			DBGPRINT(RT_DEBUG_TRACE,("Ring %d  ===========================\n",i));
			if(RING_TYPE_AC0==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac0RingMain[pDevExt->CurAc0TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC1==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac1RingMain[pDevExt->CurAc1TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC2==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac2RingMain[pDevExt->CurAc2TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC3==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac3RingMain[pDevExt->CurAc3TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC4==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac4RingMain[pDevExt->CurAc4TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC5==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac5RingMain[pDevExt->CurAc5TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC6==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac6RingMain[pDevExt->CurAc6TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC7==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac7RingMain[pDevExt->CurAc7TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC8==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac8RingMain[pDevExt->CurAc8TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC9==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac9RingMain[pDevExt->CurAc9TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC10==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac10RingMain[pDevExt->CurAc10TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_AC11==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->Ac11RingMain[pDevExt->CurAc11TxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_MGMT==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->MgmtRingMain[pDevExt->CurMgmtTxIndex].DmaBuf.AllocVa;
			else if(RING_TYPE_HCCA==i)
				pTxWI = (PTX_WI_STRUC) pDevExt->HccaRingMain[pDevExt->CurHccaTxIndex].DmaBuf.AllocVa;

			ptr = (PULONG)pTxWI;
			DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) TXByteCount=0x%08X= %d\n", pTxWI->TXByteCount,pTxWI->TXByteCount));	
			iPrintTXByteCount = pTxWI->TXByteCount;
			
			//if(iPrintTXByteCount>sizeof(TX_WI_STRUC))
			if(iPrintTXByteCount>pringMaxlen)
			{
				iPrintTXByteCount = pringMaxlen;//don't print too much, only print TXD
			}
			
			for(j=0 ; j<iPrintTXByteCount/4 +1 ; j++)
			{
				DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) DW %d 0x%08X\n",j, *(ptr+j)));
			}
			//DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) 0 0x%08X\n", *ptr));
			//DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) 1 0x%08X\n", *(ptr+1)));
			//DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) 2 0x%08X\n", *(ptr+2)));
			//DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) 3 0x%08X\n", *(ptr+3)));	
			//DBGPRINT(RT_DEBUG_TRACE,("TXD(TXWI) TXByteCount=0x%08X= %d\n", pTxWI->TXByteCount,pTxWI->TXByteCount));		
		}

		RTMP_IO_READ32(pDevExt, RA_SCHEDULER_REG4,&SchReg4);
		if(SchReg4&0x00000020)
		{
			PUCHAR	ptr;
			
			memset(g_TxDPattern,0,sizeof(g_TxDPattern));
			memset(g_PayloadPattern,0,sizeof(g_PayloadPattern));

			
			//if(pLoopbackTxWI->FT)
			//{
			//	iPattern2Location = 56;
			//}
			//else
			//{
			//	iPattern2Location = 36;
			//}
		
			ptr = (PUCHAR)pLoopbackTxWI;

			if(1==pLoopbackTxWI->FT)
			{//tx is long format
				memcpy(g_TxDPattern,ptr,sizeof(g_TxDPattern));
				if(pLoopbackTxWI->TXByteCount>sizeof(g_TxDPattern))
				{
					memcpy(g_PayloadPattern,ptr+sizeof(g_TxDPattern),pLoopbackTxWI->TXByteCount-sizeof(g_TxDPattern));
				}
				g_ulTXPacketLength = pLoopbackTxWI->TXByteCount;	
			}
			else
			{//short format
				memcpy(g_TxDPattern,ptr,8);
				memcpy(g_TxDPattern+8,ptr+8,4);
				
				if(pLoopbackTxWI->TXByteCount>= 12)
				{
					memcpy(g_PayloadPattern,ptr+12,pLoopbackTxWI->TXByteCount-12);
					g_ulTXPacketLength = pLoopbackTxWI->TXByteCount+20;//return data will be long format, add 4 DW
				}
				else
					DBGPRINT(RT_DEBUG_TRACE,("%s TXByteCount <12, error!!!!!!! \n",__FUNCTION__, *ptr));
				
				
			}
			
						
		}
//werner debug<-


		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() StartTx\n"));							
		// Enable TX DMA master function
		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX0,pDevExt->CurAc0TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc0TxIndex = %x\n",pDevExt->CurAc0TxIndex));
		
		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX1,pDevExt->CurAc1TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc1TxIndex = %x\n",pDevExt->CurAc1TxIndex));
		
		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX2,pDevExt->CurAc2TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc2TxIndex = %x\n",pDevExt->CurAc2TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX3,pDevExt->CurAc3TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc3TxIndex = %x\n",pDevExt->CurAc3TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX4,pDevExt->CurAc4TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc4TxIndex = %x\n",pDevExt->CurAc4TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX5,pDevExt->CurAc5TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc5TxIndex = %x\n",pDevExt->CurAc5TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX6,pDevExt->CurAc6TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc6TxIndex = %x\n",pDevExt->CurAc6TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX7,pDevExt->CurAc7TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc7TxIndex = %x\n",pDevExt->CurAc7TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX8,pDevExt->CurAc8TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc8TxIndex = %x\n",pDevExt->CurAc8TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX9,pDevExt->CurAc9TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc9TxIndex = %x\n",pDevExt->CurAc9TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX10,pDevExt->CurAc10TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc10TxIndex = %x\n",pDevExt->CurAc10TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX11,pDevExt->CurAc11TxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurAc11TxIndex = %x\n",pDevExt->CurAc11TxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX12,pDevExt->CurMgmtTxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurMgmtTxIndex = %x\n",pDevExt->CurMgmtTxIndex));

		RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX13,pDevExt->CurHccaTxIndex );
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() pDevExt->CurHccaTxIndex = %x\n",pDevExt->CurHccaTxIndex));

//		value =  RTMP_Real_IO_READ32(pDevExt, RA_INT_MASK );
//		RTMP_Real_IO_WRITE32(pDevExt, RA_INT_MASK, value | INT_Bit_ANY_TxDone | INT_Bit_ANY_RxDone);	

	}
	else if(type == TX_STOP)
	{
		DBGPRINT(RT_DEBUG_TRACE,("TXT: RTPCIStartTX() StopTx\n"));

		// Disable TX DMA master function
		//value = RTMP_Real_IO_READ32(pDevExt,RA_WPDMA_GLO_CFG );
		/*while (( value & ( TX_DMA_BUSY))  != 0)
		{
//			value = RTMP_Real_IO_READ32(pDevExt,RA_WPDMA_GLO_CFG );
			retry--;
			if (retry == 0 ) 
			{
				break;
				DBGPRINT(RT_DEBUG_TRACE,("TXT: StopTx fail still in busy\n");
			}
		}*/

		//RTMP_Real_IO_WRITE32(pDevExt,RA_WPDMA_GLO_CFG ,value &  ~TX_DMA_EN );
		//RTMP_Real_IO_WRITE32(pDevExt,RA_TX_CTX_IDX0,0);
		//disable interrupt
		//value =  RTMP_Real_IO_READ32(pDevExt,RA_INT_MASK );
		pDevExt->OtherCounters.TxRepeatCount = 0;
		//RTMP_Real_IO_WRITE32(pDevExt,RA_INT_MASK, 0 );

	}

	return	STATUS_SUCCESS;
}


VOID	NICIssueReset(
	IN PVOID pContext)
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pContext;

	DBGPRINT(RT_DEBUG_TRACE,("--> NICIssueReset\n"));

	// Disable Rx, register value supposed will remain after reset
//	RTMP_Dummy_IO_WRITE32(pDevExt, RXCSR0, 0x01);
	// Issue reset and clear from reset state
//	RTMP_Dummy_IO_WRITE32(pDevExt, CSR1, 0x01);
//	RTMP_Dummy_IO_WRITE32(pDevExt, CSR1, 0x00);

	DBGPRINT(RT_DEBUG_TRACE,("<-- NICIssueReset\n"));
}


#ifdef	SNIFF
NTSTATUS
RTPCIStartSniffer(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	)
{
	ULONG				Count=0;
	PRXD_STRUC			pSnifRxD;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
#if 1 //mark by simon
	do
	{
		if((pDevExt->PacketBufferSnifferIdx > pDevExt->PacketBufferDriverIdx) && (pDevExt->PacketBufferSnifferIdx >= RX_RING_PACKET_BUFFER)
			&& (pDevExt->PacketBufferWrapAround == TRUE))
		{
			pDevExt->PacketBufferSnifferIdx = 0;
			pDevExt->PacketBufferWrapAround = FALSE;
		}
		else if(pDevExt->PacketBufferSnifferIdx == pDevExt->PacketBufferDriverIdx)
		{
			break;
		}

		pSnifRxD = (PRXD_STRUC)((PUCHAR)pDevExt->pPacketBuf + pDevExt->PacketBufferSnifferIdx * MAX_FRAME_SIZE);

		RtlCopyMemory(UserBuffer + (Count * MAX_FRAME_SIZE), 
			                 (PUCHAR)pDevExt->pPacketBuf + pDevExt->PacketBufferSnifferIdx * MAX_FRAME_SIZE, 
			                 (RING_RX_DESCRIPTOR_SIZE + pSnifRxD->SDL0));

		pDevExt->PacketBufferSnifferIdx++;
		Count++;
	}while(Count < SNIFFER_COUNT);

	if(Count > 0)
	{
		DBGPRINT(RT_DEBUG_TRACE,("RTPCIStartSniffer ===> PacketBufferSnifferIdx = %d, Count = %d\n",pDevExt->PacketBufferSnifferIdx - 1, Count));
	}

	*ByteCount = MAX_FRAME_SIZE * Count;
#endif
	return	STATUS_SUCCESS;
}
#endif


NTSTATUS
EnableBeacon(
    IN	PDEVICE_OBJECT	pDO,
    IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	)
{
    RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
    PCHAR ptr;
	ULONG i;

    ptr = (PCHAR)(pIBuffer);
    DBGPRINT(RT_DEBUG_TRACE,("Beacon TXWI : ===============\n"));
    DBGPRINT(RT_DEBUG_TRACE,("pDevExt->virtualaddr : 0x%08x\n", (PUCHAR)pDevExt->virtualaddr));
    DBGPRINT(RT_DEBUG_TRACE,("pDevExt->virtualaddr + HW_BEACON_BASE0 : 0x%08x\n", (PUCHAR)pDevExt->virtualaddr + HW_BEACON_BASE0));
    for (i=0; i<TXWI_SIZE; i++)
    {                	
	 WRITE_Real_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + HW_BEACON_BASE0 + i, *ptr);
        ptr ++;
    }
    DBGPRINT(RT_DEBUG_TRACE,("===============\n"));
		

    // 59 bytes Beacon Frame (Default Length) 
    DBGPRINT(RT_DEBUG_TRACE,("Beacon Frame : ===============\n"));
    for (i=0; i < DefaultBeaconLength; i++)
    {     
	 WRITE_Real_REGISTER_UCHAR((PUCHAR)pDevExt->virtualaddr + HW_BEACON_BASE0 + TXWI_SIZE + i, *ptr);
        ptr ++;
    }    
    DBGPRINT(RT_DEBUG_TRACE,("===============\n"));


    //
    // For Wi-Fi faily generated beacons between participating stations. 
    // Set TBTT phase adaptive adjustment step to 8us (default 16us)
    // 	
    //RTMP_Real_IO_WRITE32(pDevExt, RA_TBTT_SYNC_CFG, 0x00001008);
    // Enable BEACON
    //RTMP_Real_IO_WRITE32(pDevExt, RA_BCN_TIME_CFG, 0x001D0640);

    *ByteCount = 0;

    return	STATUS_SUCCESS;
	
}


NTSTATUS
DisableBeacon(
    IN	PDEVICE_OBJECT	pDO,   
    OUT PULONG			ByteCount	)
{
    RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)pDO->DeviceExtension;
    
    // Disable BEACON
   // RTMP_Real_IO_WRITE32(pDevExt, RA_BCN_TIME_CFG, 0x00000640);

    *ByteCount = 0;

    return	STATUS_SUCCESS;
	
}
#endif /* RTMP_PCI_SUPPORT */

