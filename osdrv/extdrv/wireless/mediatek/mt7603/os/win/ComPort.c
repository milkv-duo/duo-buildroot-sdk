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
VOID
ComPortInit(
	IN	PDEVICE_OBJECT	pDO
	)
{

}

NTSTATUS
RTPCIComWrite(
	IN	PDEVICE_OBJECT	pDO,
	IN	PUCHAR			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	
	)
{
	NTSTATUS                ntStatus = STATUS_SUCCESS;
	ULONG					i, InsertByteCount, RemainderCount, HandledCount;
	UARTTX_STRUC			uartTX;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)	pDO->DeviceExtension;

	InsertByteCount = *(pIBuffer + 0);

	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIComWrite --> InsertByteCount=%d\n", InsertByteCount));

	if(InsertByteCount < pDevExt->comTxBuffer.TxFreeCount)
	{
		// not wrap around
		if((pDevExt->comTxBuffer.TxInsertPointer + InsertByteCount) <= MAX_COM_BUFFER_SIZE)
		{
			RtlCopyMemory(
				(PUCHAR)pDevExt->comTxBuffer.pComTxBuf + pDevExt->comTxBuffer.TxInsertPointer, 
				(pIBuffer+20), 
				InsertByteCount
				);
			pDevExt->comTxBuffer.TxInsertPointer += InsertByteCount;
			if(pDevExt->comTxBuffer.TxInsertPointer >= MAX_COM_BUFFER_SIZE)
				pDevExt->comTxBuffer.TxInsertPointer = 0;
			pDevExt->comTxBuffer.TxFreeCount -= InsertByteCount;
		}
		else // wrap around
		{
			RtlCopyMemory(
				(PUCHAR)pDevExt->comTxBuffer.pComTxBuf + pDevExt->comTxBuffer.TxInsertPointer, 
				(pIBuffer + 20), 
				(MAX_COM_BUFFER_SIZE - pDevExt->comTxBuffer.TxInsertPointer)
				);
			HandledCount = (MAX_COM_BUFFER_SIZE - pDevExt->comTxBuffer.TxInsertPointer);
			RemainderCount = InsertByteCount - HandledCount;
			pDevExt->comTxBuffer.TxFreeCount -= HandledCount;
			pDevExt->comTxBuffer.TxInsertPointer = 0;

			RtlCopyMemory(
				(PUCHAR)pDevExt->comTxBuffer.pComTxBuf + pDevExt->comTxBuffer.TxInsertPointer, 
				(pIBuffer + 20 + HandledCount), 
				(RemainderCount)
				);
			pDevExt->comTxBuffer.TxInsertPointer += RemainderCount;
			pDevExt->comTxBuffer.TxFreeCount -= RemainderCount;
		}
	}
	else
	{
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIComWrite --> ComBuffer is full!!!\n"));
	}

	// Remove from buffer and transmit it
	uartTX.word = RTMP_Dummy_IO_READ32(pDevExt, UARTTX);
	for(i=0; i<uartTX.field.TxFrCnt; i++)
	{
		RTMP_Dummy_IO_WRITE32(pDevExt, 
						UARTTX, 
						*(pDevExt->comTxBuffer.pComTxBuf + pDevExt->comTxBuffer.TxRemovePointer));

		pDevExt->comTxBuffer.TxFreeCount++;
		pDevExt->comTxBuffer.TxRemovePointer++;
		if(pDevExt->comTxBuffer.TxRemovePointer == MAX_COM_BUFFER_SIZE)
			pDevExt->comTxBuffer.TxRemovePointer = 0;
		InsertByteCount--;
		if(InsertByteCount == 0)
			break;
	}

/*	RTMP_Dummy_IO_WRITE32(pDevExt, 
						0x18c, 
						0x98886088);
	RTMP_Dummy_IO_WRITE32(pDevExt, 
						0x184, 
						0x000106b6);
	RTMP_Dummy_IO_WRITE32(pDevExt, 
						0x180, 
						0x00000061);
	RTMP_Dummy_IO_WRITE32(pDevExt, 
						0x180, 
						0x00000062);
	RTMP_Dummy_IO_WRITE32(pDevExt, 
						0x180, 
						0x00000063);*/

	*ByteCount = 0;

	return	ntStatus;
}

NTSTATUS
RTPCIComRead(
	IN	PDEVICE_OBJECT	pDO,
	IN	PULONG			pIBuffer,
    IN	PUCHAR			UserBuffer,
    IN	ULONG			BufferMaxSize,
    OUT PULONG			ByteCount	
	)
{
	NTSTATUS                ntStatus = STATUS_SUCCESS;
	ULONG					i, RemoveByteCount, RemainderCount, HandledCount, AvailDataCount;
	UARTRX_STRUC			uartRX;
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)	pDO->DeviceExtension;

	AvailDataCount = MAX_COM_BUFFER_SIZE - pDevExt->comRxBuffer.RxFreeCount;

//	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIComWrite --> AvailDataCount=%d\n", AvailDataCount);

	if(pDevExt->comRxBuffer.RxFreeCount != MAX_COM_BUFFER_SIZE)
	{
		if((pDevExt->comRxBuffer.RxRemovePointer + AvailDataCount) <= MAX_COM_BUFFER_SIZE)
		{
			RtlCopyMemory(
				(UserBuffer),
				(PUCHAR)pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxRemovePointer,
				(AvailDataCount)
				);
			pDevExt->comRxBuffer.RxRemovePointer += AvailDataCount;
			if(pDevExt->comRxBuffer.RxRemovePointer >= MAX_COM_BUFFER_SIZE)
				pDevExt->comRxBuffer.RxRemovePointer = 0;
			pDevExt->comRxBuffer.RxFreeCount = MAX_COM_BUFFER_SIZE;
		}
		else
		{
			RtlCopyMemory(
				(UserBuffer),
				(PUCHAR)pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxRemovePointer,
				(MAX_COM_BUFFER_SIZE - pDevExt->comRxBuffer.RxRemovePointer)
				);
			HandledCount = (MAX_COM_BUFFER_SIZE - pDevExt->comRxBuffer.RxRemovePointer);
			RemainderCount = AvailDataCount - HandledCount;
			pDevExt->comRxBuffer.RxRemovePointer = 0;
			pDevExt->comRxBuffer.RxFreeCount += HandledCount;

			RtlCopyMemory(
				(UserBuffer + HandledCount),
				(PUCHAR)pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxRemovePointer,
				(RemainderCount)
				);
			pDevExt->comRxBuffer.RxRemovePointer += RemainderCount;
			pDevExt->comRxBuffer.RxFreeCount += RemainderCount;
		
		}
	}

	*ByteCount = AvailDataCount;

	return	ntStatus;
}

VOID
RTPCIHandleUartTxThDoneInterrupt(
	IN PVOID pContext 
	)
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)	pContext;
	UARTTX_STRUC			uartTX;
	ULONG					i;

	DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: RTPCIHandleUartTxThDoneInterrupt ---> \n"));

	// Remove from buffer and transmit it
	uartTX.word = RTMP_Dummy_IO_READ32(pDevExt, UARTTX);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIComWrite --> uartTX.field.TxFrCnt=%d, wantTxnum=%d\n", uartTX.field.TxFrCnt,(MAX_COM_BUFFER_SIZE-pDevExt->comTxBuffer.TxFreeCount)));
	if(pDevExt->comTxBuffer.TxFreeCount != MAX_COM_BUFFER_SIZE)
	{
		for(i=0; i<uartTX.field.TxFrCnt; i++)
		{
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartTxThDoneInterrupt -->=%X\n", *(pDevExt->comTxBuffer.pComTxBuf + pDevExt->comTxBuffer.TxRemovePointer)));
			RTMP_Dummy_IO_WRITE32(pDevExt, 
							UARTTX, 
							*(pDevExt->comTxBuffer.pComTxBuf + pDevExt->comTxBuffer.TxRemovePointer));

			pDevExt->comTxBuffer.TxFreeCount++;
			pDevExt->comTxBuffer.TxRemovePointer++;
			if(pDevExt->comTxBuffer.TxRemovePointer == MAX_COM_BUFFER_SIZE)
				pDevExt->comTxBuffer.TxRemovePointer = 0;
			if(pDevExt->comTxBuffer.TxFreeCount == MAX_COM_BUFFER_SIZE)
				break;
		}
	}
}

VOID
RTPCIHandleUartRxThDoneInterrupt(
	IN PVOID pContext 
	)
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)	pContext;
	UARTRX_STRUC			uartRX;
	ULONG					i;
	UCHAR					data;

	uartRX.word = RTMP_Dummy_IO_READ32(pDevExt, UARTRX);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartIdleThDoneInterrupt --> RxByteCnt=%d\n", uartRX.field.RxByteCnt));

	if((uartRX.field.RxDv == 1) && (uartRX.field.RxByteCnt <= pDevExt->comRxBuffer.RxFreeCount))
	{
		data = (UCHAR)uartRX.field.RXD;
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartIdleThDoneInterrupt --> RXD=%x\n", uartRX.field.RXD));
		RtlCopyMemory(
				pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxInsertPointer, 
				&data, 
				sizeof(UCHAR)
				);

		pDevExt->comRxBuffer.RxInsertPointer++;
		if(pDevExt->comRxBuffer.RxInsertPointer >= MAX_COM_BUFFER_SIZE)
			pDevExt->comRxBuffer.RxInsertPointer = 0;
		pDevExt->comRxBuffer.RxFreeCount--;

		for(i=0; i<uartRX.field.RxByteCnt; i++)
		{
			uartRX.word = RTMP_Dummy_IO_READ32(pDevExt, UARTRX);
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartIdleThDoneInterrupt --> RXD=%x\n", uartRX.field.RXD));
			data = (UCHAR)uartRX.field.RXD;

			RtlCopyMemory(
					pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxInsertPointer, 
					&data, 
					sizeof(UCHAR)
					);

			pDevExt->comRxBuffer.RxInsertPointer++;
			if(pDevExt->comRxBuffer.RxInsertPointer >= MAX_COM_BUFFER_SIZE)
				pDevExt->comRxBuffer.RxInsertPointer = 0;
			pDevExt->comRxBuffer.RxFreeCount--;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: pComRxBuf not enough or RxDv=0!!! \n"));
	}
}

VOID
RTPCIHandleUartIdleThDoneInterrupt(
	IN PVOID pContext 
	)
{
	RTMP_ADAPTER *pDevExt = (RTMP_ADAPTER *)	pContext;
	UARTRX_STRUC			uartRX;
	ULONG					i;
	UCHAR					data;

	uartRX.word = RTMP_Dummy_IO_READ32(pDevExt, UARTRX);
	DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartIdleThDoneInterrupt --> RxByteCnt=%d\n", uartRX.field.RxByteCnt));

	if((uartRX.field.RxDv == 1) && (uartRX.field.RxByteCnt <= pDevExt->comRxBuffer.RxFreeCount))
	{
		data = (UCHAR)uartRX.field.RXD;
		DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartIdleThDoneInterrupt --> RXD=%x\n", uartRX.field.RXD));
		RtlCopyMemory(
				pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxInsertPointer, 
				&data, 
				sizeof(UCHAR)
				);

		pDevExt->comRxBuffer.RxInsertPointer++;
		if(pDevExt->comRxBuffer.RxInsertPointer >= MAX_COM_BUFFER_SIZE)
			pDevExt->comRxBuffer.RxInsertPointer = 0;
		pDevExt->comRxBuffer.RxFreeCount--;

		for(i=0; i<uartRX.field.RxByteCnt; i++)
		{
			uartRX.word = RTMP_Dummy_IO_READ32(pDevExt, UARTRX);
			DBGPRINT(RT_DEBUG_TRACE,("PciDumpr: RTPCIHandleUartIdleThDoneInterrupt --> RXD=%x\n", uartRX.field.RXD));
			data = (UCHAR)uartRX.field.RXD;

			RtlCopyMemory(
					pDevExt->comRxBuffer.pComRxBuf + pDevExt->comRxBuffer.RxInsertPointer, 
					&data, 
					sizeof(UCHAR)
					);

			pDevExt->comRxBuffer.RxInsertPointer++;
			if(pDevExt->comRxBuffer.RxInsertPointer >= MAX_COM_BUFFER_SIZE)
				pDevExt->comRxBuffer.RxInsertPointer = 0;
			pDevExt->comRxBuffer.RxFreeCount--;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE,("\nPciDumpr: pComRxBuf not enough or RxDv=0!!! \n"));
	}
}

#endif /* RTMP_PCI_SUPPORT */

