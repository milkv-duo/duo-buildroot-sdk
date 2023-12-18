/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt_usb.c

	Abstract:

	Revision History:
	Who			When		What
	--------	----------	----------------------------------------------
	Name		Date		Modification logs
	
*/

#include "rt_config.h"

#define USB_HOST_CHANNEL_NUMS 10

/*	
 * Global variable for USB host channel 
 * may be allocated dynamically in the future depends on usb devices endpoint numbers
 */
static USB_HOST_CHANNEL USBHstChannel[USB_HOST_CHANNEL_NUMS]; 

#ifdef SIGMATEL_SDK
typedef struct _RTMP_URB_STRUCT_
{
	DC_URB *pOSURB;
	void (*callBack)(DC_URB_COMPLETION_STATUS status,
					 struct DC_URB_STRUCT *urb,
					 void *arg);
	void *callBackArg;
	int status;
	int bUrbEnqueueStatus;
	struct _RTMP_URB_STRUCT_ *next;
}RTMP_URB_STRUCT;

					
#define RTMP_DC_URB_MAX_NUM	64
#define RTMP_DC_URB_WAIT_QUEUE_NUM	(NUM_OF_TX_RING + 1) 
#define RTMP_DC_USB_RX_WAIT_QUEUE_IDX	NUM_OF_TX_RING

RTMP_URB_STRUCT RtmpDcUrbList[RTMP_DC_URB_MAX_NUM] = {0};
RTMP_URB_STRUCT *pRtmpDcUrbFreeList;
RTMP_URB_STRUCT *pRtmpDcUrbWaitList[RTMP_DC_URB_WAIT_QUEUE_NUM];
RTMP_URB_STRUCT *pRtmpDcUrbDoneList;

RTMP_OS_SEM *pRtmpDcUrbSema = NULL;
RTMP_OS_TASK RtmpDcUSBTask;
UINT32 RtmpDcUSBTaskStack[RTMP_TX_TASK_STACK_SIZE /sizeof(UINT32)];
TX_THREAD RTMP_DC_USB_TASK;

static int RtmpUrbPending = 0;
static int RtmpUrbSubmitIdx =0;
static int RtmpUrbWaitingCnt = 0;

#endif /* SIGMATEL_SDK */

void dump_urb(DC_URB* purb)
{
	dc_log_printf("DumpURB(0x%x):\n", (unsigned long)purb);
	dc_log_printf("  sz(URB):0x%d\n", sizeof(struct DC_URB_STRUCT));
	dc_log_printf("  pipe:0x%x\n", (unsigned long)purb->pipe);
	dc_log_printf("  endAddr:0x%x\n", (unsigned long)purb->pipe->bEndpointAddress);
	dc_log_printf("  status:%d\n", purb->status);
	dc_log_printf("  base:0x%x\n", (unsigned long)purb->base);
	dc_log_printf("  reqLen:0x%x\n", purb->requestedLength);
	dc_log_printf("  actLen:0x%x\n", purb->requestedLength - purb->length);
	dc_log_printf("  setBase:0x%x\n", (unsigned long)purb->setupBase);
	dc_log_printf("  startTime:0x%x\n", purb->tStart);
	dc_log_printf("  context:0x%x\n", (unsigned long)purb->completionCallbackArg);
	dc_log_printf("  complete:0x%x\n\n", (unsigned long)purb->completionCallback);

	hex_dump("URB buffer", purb->base, 64 /*(purb->requestedLength - purb->length)*/);
	
	return;
}

void dump_usbpipe(DC_USB_PIPE *pUSBPipe)
{
	UINT32 Index;
	
	DC_URB *curURB = pUSBPipe->urb_list;

	dc_log_printf("==BegUSBPipeDump==\n");	
	dc_log_printf("in_use : 0x%x\n", pUSBPipe->in_use);
	dc_log_printf("bDeviceAddr: 0x%x\n", pUSBPipe->bDeviceAddress);
	dc_log_printf("bEndpointAddress : 0x%x\n", pUSBPipe->bEndpointAddress);
	dc_log_printf("endpointAttr : 0x%x\n", pUSBPipe->bmEndpointAttributes);
	dc_log_printf("wMaxPacketSize : 0x%x\n", pUSBPipe->wMaxPacketSize);
	dc_log_printf("bInterval : 0x%x\n", pUSBPipe->bInterval);
	dc_log_printf("splitRequired : 0x%x\n", pUSBPipe->splitRequired);
	dc_log_printf("TThubPort : 0x%x\n", pUSBPipe->TThubPort);
	dc_log_printf("TThubAddress : 0x%x\n", pUSBPipe->TThubAddress);
	dc_log_printf("speed : 0x%x\n", pUSBPipe->speed);
	dc_log_printf("==EndUSBPipeDump==\n");

#if 0
	for	(Index = 0; !curURB; Index++)
	{
		curURB = curURB->next;
		dc_log_printf("cur urb address in pipe urb list = %x\n", (UINT32)curURB;
	}
#endif
}

/*
 * usb_alloc_urb - create a new URB for USB driver to use 
 * Create an urb for USB driver to use, initialze urb,
 * If no memory is availabe, NULL is returned
 * If the URB type is control, interrupt, or bulk, iso_packets are passed zero
 *
 * The driver must call usb_free_urb when it is finished with the URB
 */
DC_URB *usb_alloc_urb()
{
	DC_URB *pURB;
#if 1
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk;
	ra_dma_addr_t *pDMAAddr;
	
	pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_URB];
	RtmpDCRamBlockMemAlloc(pDcRamBlk, sizeof(DC_URB), (void **)(&pURB), &pDMAAddr);
#else
	pURB = kmalloc(sizeof(DC_URB),0);
#endif
	if (!pURB)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : URB meory insufficient\n", __FUNCTION__));
		return NULL;
	}

	NdisZeroMemory(pURB, sizeof(*pURB));

	return pURB;
}


/*
 * usb_free_urb - frees the memory used by an urb when all users of it are finished
 * pURB: pointer to the urb to free, maybe NULL
 * The transfer buffer associated with the urb is not freed, it must be freed elsewhere before usb_free_urb, 
 * otherwise the pTransferBuffer will losed, and memory leak.
 */
inline void usb_free_urb(DC_URB *pURB)
{
	RtmpDCRamBlockMemFree((void *)pURB);
}


/* 
 * usb_buffer_alloc - allocate dma-consistent(coherent) buffer
 * @dev: device : not used by threadX
 * @size: requested buffer size used in threadX!! (already pre-allocated at PSB in initialization)
 * @dma : not used in threadX because of not dma address field in URB, and the dma will be translate under host controller driver. 
 *
 * Return value is either null, or cpu-space pointer to buffer, 
 * When the buffe no longer used, use usb_buffer_free to release buffer space 
 */
void * usb_buffer_alloc(DC_USB_DEVICE *pUSBDev, unsigned int Size, ra_dma_addr_t **pDMAAddr)
{
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamBlk, *pDcRamBlkAligned = NULL;
	void *pVirtualAddress = NULL, *pTmpAddr, *pData_dma;

	if (Size == sizeof(HTTX_BUFFER))
	{
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_HTTX];
		pDcRamBlkAligned = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_HTTX_ALIGN];
		dc_log_printf("SelectHTTx,size=%d\n", Size);
	}
	else if (Size == MAX_RXBULK_SIZE)
	{
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_RX];
		dc_log_printf("SelectRx,size=%d\n", Size);
	}
	else if(Size == sizeof(TX_BUFFER))
	{
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_TX];
		dc_log_printf("SelectTx,size=%d\n", Size);
	}
	else
	{
		pDcRamBlk = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_CTRL_PIPE];
	}
	
	if (pDcRamBlk)
		RtmpDCRamBlockMemAlloc(pDcRamBlk, Size, &pVirtualAddress, &pTmpAddr);
	
	if (!pVirtualAddress)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : usb pool resource is not available\n", __FUNCTION__));	
		return NULL;
	}

	if (pDcRamBlkAligned)
	{
		RtmpDCRamBlockMemAlloc(pDcRamBlkAligned, Size, &pData_dma, &pTmpAddr);
		if (pData_dma == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s : usb dma resource is not available\n", __FUNCTION__));
			RtmpDCRamBlockMemFree(pVirtualAddress);
			pVirtualAddress = NULL;
		}
		*pDMAAddr = pData_dma;
		DBGPRINT(RT_DEBUG_ERROR, ("dataDma=0x%x,pDMAAddr=0x%x\n", pData_dma, *pDMAAddr));
	}
	else
	{
		*pDMAAddr = NULL;
	}

	
	/* Return virtual address */
	return pVirtualAddress;
}


inline void usb_buffer_free(void *pUSBBufferAddr)
{
	if(pUSBBufferAddr)
		RtmpDCRamBlockMemFree(pUSBBufferAddr);
}


DC_USB_PIPE * GetUSBPipeFromEndpoint(UINT8 EndpointAddress)
{
	UINT32 Index;

	for (Index = 0; Index < USB_HOST_CHANNEL_NUMS; Index++)
	{
		if (USBHstChannel[Index].EndpointAddress == EndpointAddress)
		{
			return USBHstChannel[Index].pUSBPipe;
		}
	}

	DBGPRINT(RT_DEBUG_ERROR, ("%s : Can not find USBPipe for this EndpointAddress[%x]", __FUNCTION__, EndpointAddress));
	return NULL;
}


DC_USB_DEVICE *GetUSBDevFromUSBPipe(DC_USB_PIPE *pUSBPipe)
{
	UINT32 Index;
	DC_USB_DEVICE *pUSBDev = NULL;

	for (Index = 0; Index < USB_HOST_CHANNEL_NUMS; Index++)
	{
		if (USBHstChannel[Index].pUSBPipe == pUSBPipe)
		{
			return USBHstChannel[Index].pUSBDev;
		}
	}

	DBGPRINT(RT_DEBUG_ERROR, ("%s : Can not find USBDev for this USBPipe[%p]", __FUNCTION__, pUSBPipe));
	return NULL;
}


void ReleaseUSBHostChannel(UINT32 AllocatedChannelNums)
{
	UINT32 Index;

	for (Index = 0; Index < AllocatedChannelNums; Index++)
	{
		USBHstChannel[Index].pUSBDev = NULL;
		USBHstChannel[Index].EndpointAddress = NULL;
		dc_free(USBHstChannel[Index].pUSBPipe);
		USBHstChannel[Index].pUSBPipe = NULL;
	}
}


void SetUSBHostChannel(UINT32 ChannelIndex, DC_USB_DEVICE *pUSBDev, UINT8 EndpointAddress, DC_USB_PIPE *pUSBPipe)
{
	USBHstChannel[ChannelIndex].pUSBDev = pUSBDev;
	USBHstChannel[ChannelIndex].EndpointAddress = EndpointAddress;
	USBHstChannel[ChannelIndex].pUSBPipe = pUSBPipe;
}


/*
 * usb_control_msg - Builds a control urb, sends it off, and wait for completion until timeout value)
 * @USBDev: USB device info
 * @endpointaddress: USB device default pipe
 * @bRequest: USB message request type
 * @bmRequestType: USB message request
 * @wvalue: USB message value
 * @wIndex: USB
 * @pwLength : length of transferred data
 * @pBuffer: pointer to data buffer by user space (may not coherent)
 * @timeout: time to wait for message to complete
 * 
 *  !! Can not call this API in "INTERRUPT" context, otherwise lead to wait forever status
 */
INT32 usb_control_msg(DC_USB_DEVICE *pUSBDev, UINT32 endpointaddress, UINT8 bRequest, UINT8 bmRequestType, 
									UINT16 wValue, UINT16 wIndex, void *pBuffer, UINT16 wLength, int timeout)
{
	NDIS_STATUS Result = NDIS_STATUS_FAILURE;
	void *pDMAAddr = NULL;
	void *pSetupDMAAddr = NULL;

	/* To store the setup packet */
	pUSBHST_SETUP_PACKET pSetup = NULL;

	RTMP_DC_RAM_BLOCK_ELEMENT *cmdBuf;

	/* To store the USB request block */
	DC_URB *pURB;

	/* To send/receive usb buffer to/from device/host */
	UCHAR *pUSBBuffer = NULL;

	MUINT16 BytesMoved;

	/* Sanity check for usb device info */
	if (!pUSBDev)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Invalide Device Information.\n", __FUNCTION__));
		goto ERROR0;
	}

	/* Get DMA memory from pre-allocated USBHST_SETUP_PACKET PSB for cache coherent transfer */
	pSetup = RTUSB_URB_ALLOC_BUFFER(pUSBDev, sizeof(*pSetup), &pSetupDMAAddr);

	if (!pSetup)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficient Resource for USB buffer\n", __FUNCTION__));
		Result = NDIS_STATUS_FAILURE;
		goto ERROR0;
	}

	/* Allocate memory for USB request block for control pipe transfer*/
	pURB = RTUSB_ALLOC_URB(0);
	if (!pURB)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficient Resource for URB\n", __FUNCTION__));
		Result = NDIS_STATUS_FAILURE;
		goto ERROR1;
	}

	if (wLength > 0)
	{
		/* Get DMA memory from pre-allocated buffer for cache coherent transfer */
		pUSBBuffer = RTUSB_URB_ALLOC_BUFFER(pUSBDev, wLength, &pDMAAddr);
	}
	if ((wLength > 0) && !pUSBBuffer)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficient Resource for USB Buffer\n", __FUNCTION__));
		Result = NDIS_STATUS_FAILURE;
		goto ERROR2;
	}
	
	/* If Control endpoint for OUT transfer, Copy outside pBuffer to USB buffer for DMA cache coherent transfer */
	if (((bmRequestType & 0x80) == 0 ) && (wLength > 0)) {
		NdisMoveMemory(pUSBBuffer, (UCHAR *)pBuffer, wLength);

		/* Flush cache mapped to USBBuffer region to make sure any dirty data to USBBuffer DMA memory */
		dc_cache_flush(pUSBBuffer, pUSBBuffer + wLength);
	}
	
	/* Fill the  SETUP_PACKET with the information passed */
	USBHST_FILL_SETUP_PACKET(pSetup,
	                         bmRequestType,
	                         bRequest,
	                         wValue,
	                         wIndex,
	                         wLength);

	/* Convert the setup packet from CPU format to little endian format */
	/*OS_BUFFER_CPU_TO_LE(pSetup, sizeof(USBHST_SETUP_PACKET)); */

	/* Flush cache mapped to pSetup region to make sure any dirty data to pSetup DMA memory */
	dc_cache_flush(pSetup, pSetup + sizeof(USBHST_SETUP_PACKET));
	
	/* Synchronous URB transfer */
	Result = dc_uh_core_transfer(pUSBDev,
								 pUSBDev->pipe_ep0,
								 pURB,
								 pUSBBuffer,
								 wLength,
								 pSetup,
								 timeout,
								 FALSE, /* add_zero */
								 &BytesMoved);
	if (Result != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : dc_uh_core_transfer failed\n", __FUNCTION__));
		Result = NDIS_STATUS_FAILURE;
		goto ERROR3;
	}

	/* If Control endpoint for IN transfer, Copy USB buffer  to pBuffer, 
	   data do not swap from little endian to CPU format in this API */
	if (((bmRequestType & 0x80) != 0) && (wLength > 0) && (Result == DC_SUCCESS)) {

		/* Invalidate cache mapped to USBBuffer region to make sure CPU re-fetch from USBBuffer DMA memory */
		dc_cache_invalidate(pUSBBuffer, pUSBBuffer + wLength);
		NdisMoveMemory((UCHAR *)pBuffer, pUSBBuffer, wLength);
	}

#if 0
	cmdBuf = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_SHELLCMDBUF];
	dc_log_printf("CMD buf = %x", cmdBuf->baseAddr);
#endif

	/* Continue to release allocated resource */
	Result = NDIS_STATUS_SUCCESS;
	
ERROR3:
	if (wLength != 0) 
	{
		RTUSB_URB_FREE_BUFFER(pUSBDev, wLength, pUSBBuffer, pDMAAddr);
	}

ERROR2:
	/* free USB request block */
	RTUSB_FREE_URB(pURB);

ERROR1:
	RTUSB_URB_FREE_BUFFER(pUSBDev, sizeof(*pSetup), pSetup, pSetupDMAAddr);
	
ERROR0:
	return (-Result); /* return negative result for multi-os support */
}


INT32 usb_submit_urb(DC_URB *pURB)
{
	NDIS_STATUS Result;
	DC_USB_DEVICE *pUSBDev = NULL;
	DC_USB_PIPE *pUSBPipe = pURB->pipe;

	pUSBDev = GetUSBDevFromUSBPipe(pUSBPipe);

	if (!pUSBDev)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Can not find USBDev from URB[%p]", __FUNCTION__, pURB));
		return NDIS_STATUS_FAILURE;
	}

	if ((((UINT32)pURB->base) & (RTMP_DC_USB_ALIGN_BYTE -1)) != 0 )
	{
		dc_log_printf("URB NotAligned");
		dump_urb(pURB);
	}

	/* Flush cache mapped to USBBuffer region to make sure any dirty data to USBBuffer DMA memory if OUT endpoint*/
	if ((pUSBPipe->bEndpointAddress & 0x80) == DC_USB20_ENDPOINT_OUT) /* Bit 7 of EndpointAddress field */
	{
		/* Flush cache mapped to USBBuffer region to make sure any dirty data to USBBuffer DMA memory */
		dc_cache_flush(pURB->base, pURB->base + pURB->requestedLength);
	}

	/* If transfer type is CONTROL, Flush Setup packet */
	if ((pUSBPipe->bmEndpointAttributes & 0x03) == DC_USB20_EP_CONTROL) /* Bit 0:1 of Attributes field */
	{
		/* Convert the setup packet from CPU format to little endian format */
		OS_BUFFER_CPU_TO_LE(pURB->setupBase, sizeof(USBHST_SETUP_PACKET));

		/* Flush cache mapped to pSetup region to make sure any dirty data to pSetup DMA memory */
		dc_cache_flush(pURB->setupBase, pURB->setupBase + sizeof(USBHST_SETUP_PACKET));
	}
	
	/* Asynchronous URB transfer */
	Result = dc_uh_core_transfer_start(pUSBDev,
									   pUSBPipe,
									   pURB,
									   pURB->base,
									   pURB->requestedLength,
									   pURB->setupBase,
									   pURB->timeoutMSec,
									   FALSE,
									   pURB->completionCallback,
									   pURB->completionCallbackArg);
	if (Result != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : usb_submit_urb failed!!\n", __FUNCTION__));
		Result = NDIS_STATUS_FAILURE;
	}
	else
	{
		Result = NDIS_STATUS_SUCCESS;
	}

	return Result;
}


/*
 * usb_unlink_urb : abort URBs transfer pending on a endpoint 
 * @pURB: pointer to URB previously submit 
 * 
 * Usually to dequeu host controller driver specific data structure for URBs, 
 * and call completion callback function
 */
NDIS_STATUS usb_unlink_urb(DC_URB *pURB)
{
	NDIS_STATUS Result;
	DC_USB_DEVICE *pUSBDev = NULL;
	DC_USB_PIPE *pUSBPipe = pURB->pipe;

	pUSBDev = GetUSBDevFromUSBPipe(pUSBPipe);

	if (!pUSBDev)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Can not find USBDev from URB[%p]", __FUNCTION__, pURB));
		return NDIS_STATUS_FAILURE;
	}

	/*
		This function is called by a Class Driver to cancel all the transfers pending for a 
		particular pipe, send a ¡§ClearFeature-Halt¡¨ to the endpoint on the device, and reset 
		the data toggle.
	*/
	Result = dc_uh_core_pipe_reset(pUSBDev, pUSBPipe);

	if (Result != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : dc_uh_core_pipe_reset failed!!\n", __FUNCTION__));
		Result = NDIS_STATUS_FAILURE;

	}
	else
	{
		Result = NDIS_STATUS_SUCCESS;
	}

	return Result;
}


static void rtusb_dataout_complete(unsigned long data)
{
	PRTMP_ADAPTER		pAd;
	purbb_t				pUrb;
	POS_COOKIE			pObj;
	PHT_TX_CONTEXT		pHTTXContext;
	UCHAR				BulkOutPipeId;
	NTSTATUS			Status;
	unsigned long		IrqFlags;


#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->rtusb_dataout_complete\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pHTTXContext	= (PHT_TX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pHTTXContext->pAd;
	pObj 			= (POS_COOKIE) pAd->OS_Cookie;
	Status			= pUrb->rtusb_urb_status;

	/* Store BulkOut PipeId */
	BulkOutPipeId = pHTTXContext->BulkOutPipeId;
	pAd->BulkOutDataOneSecCount++;

	/*DBGPRINT(RT_DEBUG_LOUD, ("Done-B(%d):I=0x%lx, CWPos=%ld, NBPos=%ld, ENBPos=%ld, bCopy=%d!\n", BulkOutPipeId, in_interrupt(), pHTTXContext->CurWritePosition, */
	/*		pHTTXContext->NextBulkOutPosition, pHTTXContext->ENextBulkOutPosition, pHTTXContext->bCopySavePad)); */

	RTMP_IRQ_LOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);
	pAd->BulkOutPending[BulkOutPipeId] = FALSE;
	pHTTXContext->IRPPending = FALSE;
	pAd->watchDogTxPendingCnt[BulkOutPipeId] = 0;
	
	if (Status == USB_ST_NOERROR)
	{	
		pAd->BulkOutComplete++;

		RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);
	
		pAd->Counters8023.GoodTransmits++;
		/*RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags); */
		FREE_HTTX_RING(pAd, BulkOutPipeId, pHTTXContext);			
		/*RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags); */

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			UAPSD_UnTagFrame(pAd, BulkOutPipeId, pHTTXContext->NextBulkOutPosition, pHTTXContext->ENextBulkOutPosition);
		}
#endif /* UAPSD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	}
	else	/* STATUS_OTHER */
	{
		PUCHAR	pBuf;
		
		pAd->BulkOutCompleteOther++;
		
		pBuf = &pHTTXContext->TransferBuffer->field.WirelessPacket[pHTTXContext->NextBulkOutPosition];
		
		if (!RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS |
									fRTMP_ADAPTER_HALT_IN_PROGRESS |
									fRTMP_ADAPTER_NIC_NOT_EXIST |
									fRTMP_ADAPTER_BULKOUT_RESET)))
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);
			pAd->bulkResetPipeid = BulkOutPipeId;
			pAd->bulkResetReq[BulkOutPipeId] = pAd->BulkOutReq;
		}
		RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);

		DBGPRINT_RAW(RT_DEBUG_ERROR, ("BulkOutDataPacket failed: ReasonCode=%d!\n", Status));
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("\t>>BulkOut Req=0x%lx, Complete=0x%lx, Other=0x%lx\n", pAd->BulkOutReq, pAd->BulkOutComplete, pAd->BulkOutCompleteOther));
		DBGPRINT_RAW(RT_DEBUG_ERROR, ("\t>>BulkOut Header:%x %x %x %x %x %x %x %x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7]));
		/*DBGPRINT_RAW(RT_DEBUG_ERROR, (">>BulkOutCompleteCancel=0x%x, BulkOutCompleteOther=0x%x\n", pAd->BulkOutCompleteCancel, pAd->BulkOutCompleteOther)); */
		
	}

	/* */
	/* bInUse = TRUE, means some process are filling TX data, after that must turn on bWaitingBulkOut */
	/* bWaitingBulkOut = TRUE, means the TX data are waiting for bulk out. */
	/* */
	/*RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags); */
	if ((pHTTXContext->ENextBulkOutPosition != pHTTXContext->CurWritePosition) && 
		(pHTTXContext->ENextBulkOutPosition != (pHTTXContext->CurWritePosition+8)) && 
		!RTUSB_TEST_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_FRAG << BulkOutPipeId)))
	{
		/* Indicate There is data avaliable */
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
	}
	/*RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags); */

	/* Always call Bulk routine, even reset bulk. */
	/* The protection of rest bulk should be in BulkOut routine */
	RTUSBKickBulkOut(pAd);
}


static void rtusb_null_frame_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER	pAd;
	PTX_CONTEXT		pNullContext;
	purbb_t			pUrb;
	NTSTATUS		Status;
	unsigned long	irqFlag;

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->rtusb_null_frame_complete_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pNullContext	= (PTX_CONTEXT)pUrb->rtusb_urb_context;
	pAd 			= pNullContext->pAd;
	Status 			= pUrb->rtusb_urb_status;

	/* Reset Null frame context flags */
	RTMP_IRQ_LOCK(&pAd->BulkOutLock[0], irqFlag);
	pNullContext->IRPPending 	= FALSE;
	pNullContext->InUse 		= FALSE;
	pAd->BulkOutPending[0] = FALSE;
	pAd->watchDogTxPendingCnt[0] = 0;

	if (Status == USB_ST_NOERROR)
	{
		RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[0], irqFlag);
		
		RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
	}
	else	/* STATUS_OTHER */
	{
		if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET)))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("Bulk Out Null Frame Failed, ReasonCode=%d!\n", Status));
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);
			pAd->bulkResetPipeid = (MGMTPIPEIDX | BULKOUT_MGMT_RESET_FLAG);
			RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[0], irqFlag);
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);
		}
		else
		{
			RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[0], irqFlag);
		}
	}

	/* Always call Bulk routine, even reset bulk. */
	/* The protectioon of rest bulk should be in BulkOut routine */
	RTUSBKickBulkOut(pAd);
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---rtusb_null_frame_complete_tasklet\n"));
#endif /* RELEASE_EXCLUDE */
}


static void rtusb_pspoll_frame_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER	pAd;
	PTX_CONTEXT		pPsPollContext;
	purbb_t			pUrb;
	NTSTATUS		Status;


#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->rt2870_pspoll_frame_complete_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pPsPollContext	= (PTX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pPsPollContext->pAd;
	Status			= pUrb->rtusb_urb_status;

	/* Reset PsPoll context flags */
	pPsPollContext->IRPPending	= FALSE;
	pPsPollContext->InUse		= FALSE;
	pAd->watchDogTxPendingCnt[0] = 0;
	
	if (Status == USB_ST_NOERROR)
	{
		RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
	}
	else /* STATUS_OTHER */
	{
		if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET)))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("Bulk Out PSPoll Failed\n"));
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);
			pAd->bulkResetPipeid = (MGMTPIPEIDX | BULKOUT_MGMT_RESET_FLAG);
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);
		}
	}

	RTMP_SEM_LOCK(&pAd->BulkOutLock[0]);
	pAd->BulkOutPending[0] = FALSE;
	RTMP_SEM_UNLOCK(&pAd->BulkOutLock[0]);

	/* Always call Bulk routine, even reset bulk. */
	/* The protectioon of rest bulk should be in BulkOut routine */
	RTUSBKickBulkOut(pAd);
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---rt2870_pspoll_frame_complete_tasklet\n"));
#endif /* RELEASE_EXCLUDE */
}


/*
========================================================================
Routine Description:
    Handle received packets.

Arguments:
	data				- URB information pointer

Return Value:
    None

Note:
========================================================================
*/
static void rx_done_tasklet(unsigned long data)
{
	purbb_t 			pUrb;
	PRX_CONTEXT			pRxContext;
	PRTMP_ADAPTER		pAd;
	NTSTATUS			Status;
	unsigned int		IrqFlags;

	pUrb		= (purbb_t)data;
	pRxContext	= (PRX_CONTEXT)pUrb->rtusb_urb_context;
	pAd 		= pRxContext->pAd;
	Status = pUrb->rtusb_urb_status;

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->rx_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	/*DBGPRINT_RAW(RT_DEBUG_TRACE, ("rxDone, st=%d\n", Status)); */

	/* Invalidate cache for USB buffer zone of already received data from USB bus */
	RTUSB_CACHE_INVALIDATE(pUrb);

	/*dump_urb(pUrb); */
	
	RTMP_IRQ_LOCK(&pAd->BulkInLock, IrqFlags);
	pRxContext->InUse = FALSE;
	pRxContext->IRPPending = FALSE;
	pRxContext->BulkInOffset += (pUrb->requestedLength -pUrb->length);
	
	/*NdisInterlockedDecrement(&pAd->PendingRx); */
	pAd->PendingRx--;

	if (Status == USB_ST_NOERROR)
	{
		pAd->BulkInComplete++;
		pAd->NextRxBulkInPosition = 0;
		if (pRxContext->BulkInOffset)	/* As jan's comment, it may bulk-in success but size is zero. */
		{
			pRxContext->Readable = TRUE;
			INC_RING_INDEX(pAd->NextRxBulkInIndex, RX_RING_SIZE);
		}
		RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
	}
	else	 /* STATUS_OTHER */
	{
		pAd->BulkInCompleteFail++;
		/* Still read this packet although it may comtain wrong bytes. */
		pRxContext->Readable = FALSE;
		RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
		
		/* Parsing all packets. because after reset, the index will reset to all zero. */
		if ((!RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS |
									fRTMP_ADAPTER_BULKIN_RESET |
									fRTMP_ADAPTER_HALT_IN_PROGRESS |
									fRTMP_ADAPTER_NIC_NOT_EXIST))))
		{
			
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("Bulk In Failed. Status=%d, BIIdx=0x%x, BIRIdx=0x%x, actual_length= 0x%x\n", 
							Status, pAd->NextRxBulkInIndex, pAd->NextRxBulkInReadIndex, pRxContext->pUrb->requestedLength - pRxContext->pUrb->length));

			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKIN_RESET);
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_IN, NULL, 0);
		}
	}
	
	ASSERT((pRxContext->InUse == pRxContext->IRPPending));

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
	{
		/* If the driver is in ATE mode and Rx frame is set into here. */
		if (pAd->ContinBulkIn == TRUE)		
		{
			RTUSBBulkReceive(pAd);
		}
	}
	else
#endif /* CONFIG_ATE */
	RTUSBBulkReceive(pAd);

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---rx_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	return;

}


static void rtusb_mgmt_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER 	pAd;
	PTX_CONTEXT		pMLMEContext;
	int				index;
	PNDIS_PACKET	pPacket;
	purbb_t			pUrb;
	NTSTATUS		Status;
	unsigned long	IrqFlags;

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->rt2870_mgmt_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pMLMEContext	= (PTX_CONTEXT)pUrb->rtusb_urb_context;
	pAd 			= pMLMEContext->pAd;
	Status			= pUrb->rtusb_urb_status;
	index 			= pMLMEContext->SelfIdx;

	ASSERT((pAd->MgmtRing.TxDmaIdx == index));

	RTMP_IRQ_LOCK(&pAd->BulkOutLock[MGMTPIPEIDX], IrqFlags);

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		/* Qos Null frame with EOSP shall have valid Wcid value. reference RtmpUSBMgmtKickOut() API. */
		/* otherwise will be value of MCAST_WCID. */
		if ((pMLMEContext->Wcid != MCAST_WCID) && (pMLMEContext->Wcid < MAX_LEN_OF_MAC_TABLE))
		{
			MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[pMLMEContext->Wcid];

			UAPSD_SP_Close(pAd, pEntry);
			pMLMEContext->Wcid = MCAST_WCID;
		}
	}
#endif /* UAPSD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	if (Status != USB_ST_NOERROR)
	{
		/*Bulk-Out fail status handle */
		if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET)))
		{
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("Bulk Out MLME Failed, Status=%d!\n", Status));
			/* TODO: How to handle about the MLMEBulkOut failed issue. Need to resend the mgmt pkt? */
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);
			pAd->bulkResetPipeid = (MGMTPIPEIDX | BULKOUT_MGMT_RESET_FLAG);
		}
	}

	pAd->BulkOutPending[MGMTPIPEIDX] = FALSE;
	RTMP_IRQ_UNLOCK(&pAd->BulkOutLock[MGMTPIPEIDX], IrqFlags);

	RTMP_IRQ_LOCK(&pAd->MLMEBulkOutLock, IrqFlags);
	/* Reset MLME context flags */
	pMLMEContext->IRPPending = FALSE;
	pMLMEContext->InUse = FALSE;
	pMLMEContext->bWaitingBulkOut = FALSE;
	pMLMEContext->BulkOutSize = 0;

	pPacket = pAd->MgmtRing.Cell[index].pNdisPacket;
	pAd->MgmtRing.Cell[index].pNdisPacket = NULL;

	/* Increase MgmtRing Index */
	INC_RING_INDEX(pAd->MgmtRing.TxDmaIdx, MGMT_RING_SIZE);
	pAd->MgmtRing.TxSwFreeIdx++;
	RTMP_IRQ_UNLOCK(&pAd->MLMEBulkOutLock, IrqFlags);

	/* No-matter success or fail, we free the mgmt packet. */
	if (pPacket)
		RTMPFreeNdisPacket(pAd, pPacket);

	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | 
								fRTMP_ADAPTER_HALT_IN_PROGRESS | 
								fRTMP_ADAPTER_NIC_NOT_EXIST))))
	{
		/* do nothing and return directly. */
	}
	else
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET) && 
			((pAd->bulkResetPipeid & BULKOUT_MGMT_RESET_FLAG) == BULKOUT_MGMT_RESET_FLAG))
		{	/* For Mgmt Bulk-Out failed, ignore it now. */
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);
		}
		else
		{

			/* Always call Bulk routine, even reset bulk. */
			/* The protectioon of rest bulk should be in BulkOut routine */
			if (pAd->MgmtRing.TxSwFreeIdx < MGMT_RING_SIZE /* pMLMEContext->bWaitingBulkOut == TRUE */)
			{
				RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_MLME);
			}
				RTUSBKickBulkOut(pAd);
			}
		}
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---rt2870_mgmt_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

}


static void rtusb_hcca_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER		pAd;
	PHT_TX_CONTEXT		pHTTXContext;
	UCHAR				BulkOutPipeId = 4;
	purbb_t				pUrb;

	
	DBGPRINT_RAW(RT_DEBUG_ERROR, ("--->hcca_dma_done_tasklet\n"));

	pUrb			= (purbb_t)data;
	pHTTXContext	= (PHT_TX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pHTTXContext->pAd;

	rtusb_dataout_complete((unsigned long)pUrb);

	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | 
								fRTMP_ADAPTER_HALT_IN_PROGRESS | 
								fRTMP_ADAPTER_NIC_NOT_EXIST)))) 
	{
		/* do nothing and return directly. */
	}
	else 
	{	
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET))
		{
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);	
		}
		else
		{	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
			if ((pAd->TxSwQueue[BulkOutPipeId].Number > 0) && 
				(pAd->DeQueueRunning[BulkOutPipeId] == FALSE) && 
				(pHTTXContext->bCurWriting == FALSE))
			{
				RTMPDeQueuePacket(pAd, FALSE, BulkOutPipeId, WCID_ALL, MAX_TX_PROCESS);
			}
			
			RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NORMAL);
			RTUSBKickBulkOut(pAd);
		}
	}
	
	DBGPRINT_RAW(RT_DEBUG_ERROR, ("<---hcca_dma_done_tasklet\n"));

		return;
}


static void rtusb_ac3_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER		pAd;
	PHT_TX_CONTEXT		pHTTXContext;
	UCHAR				BulkOutPipeId = 3;
	purbb_t				pUrb;
	
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->ac3_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pHTTXContext	= (PHT_TX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pHTTXContext->pAd;

	rtusb_dataout_complete((unsigned long)pUrb);

	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | 
								fRTMP_ADAPTER_HALT_IN_PROGRESS | 
								fRTMP_ADAPTER_NIC_NOT_EXIST)))) 
	{
		/* do nothing and return directly. */
	}
	else 
	{	
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET))
		{
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);	
		}
		else
		{	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
			if ((pAd->TxSwQueue[BulkOutPipeId].Number > 0) && 
				(pAd->DeQueueRunning[BulkOutPipeId] == FALSE) && 
				(pHTTXContext->bCurWriting == FALSE))
			{
				RTMPDeQueuePacket(pAd, FALSE, BulkOutPipeId, WCID_ALL, MAX_TX_PROCESS);
			}
			
			RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NORMAL<<3);
			RTUSBKickBulkOut(pAd);
		}
	}
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---ac3_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	return;
}


static void rtusb_ac2_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER		pAd;
	PHT_TX_CONTEXT		pHTTXContext;
	UCHAR				BulkOutPipeId = 2;
	purbb_t				pUrb;

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->ac2_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pHTTXContext	= (PHT_TX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pHTTXContext->pAd;

	rtusb_dataout_complete((unsigned long)pUrb);

	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | 
								fRTMP_ADAPTER_HALT_IN_PROGRESS | 
								fRTMP_ADAPTER_NIC_NOT_EXIST)))) 
	{
		/* do nothing and return directly. */
	}
	else 
	{	
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET))
		{
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);	
		}
		else
		{	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
			if ((pAd->TxSwQueue[BulkOutPipeId].Number > 0) && 
				(pAd->DeQueueRunning[BulkOutPipeId] == FALSE) && 
				(pHTTXContext->bCurWriting == FALSE))
			{
				RTMPDeQueuePacket(pAd, FALSE, BulkOutPipeId, WCID_ALL, MAX_TX_PROCESS);
			}
			
			RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NORMAL<<2);
			RTUSBKickBulkOut(pAd);
		}
	}
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---ac2_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	return;
}


static void rtusb_ac1_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER		pAd;
	PHT_TX_CONTEXT		pHTTXContext;
	UCHAR				BulkOutPipeId = 1;
	purbb_t				pUrb;

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->ac1_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pHTTXContext	= (PHT_TX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pHTTXContext->pAd;

	rtusb_dataout_complete((unsigned long)pUrb);

	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | 
								fRTMP_ADAPTER_HALT_IN_PROGRESS | 
								fRTMP_ADAPTER_NIC_NOT_EXIST)))) 
	{
		/* do nothing and return directly. */
	}
	else 
	{	
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET))
		{
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);	
		}
		else
		{	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
			if ((pAd->TxSwQueue[BulkOutPipeId].Number > 0) && 
				(pAd->DeQueueRunning[BulkOutPipeId] == FALSE) && 
				(pHTTXContext->bCurWriting == FALSE))
			{
				RTMPDeQueuePacket(pAd, FALSE, BulkOutPipeId, WCID_ALL, MAX_TX_PROCESS);
			}
			
			RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NORMAL<<1);
			RTUSBKickBulkOut(pAd);
		}
	}
#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---ac1_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	return;

}


static void rtusb_ac0_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER		pAd;
	PHT_TX_CONTEXT		pHTTXContext;
	UCHAR				BulkOutPipeId = 0;
	purbb_t				pUrb;
	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("--->rtusb_ac0_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	pUrb			= (purbb_t)data;
	pHTTXContext	= (PHT_TX_CONTEXT)pUrb->rtusb_urb_context;
	pAd				= pHTTXContext->pAd;

	rtusb_dataout_complete((unsigned long)pUrb);

		
	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS | 
								fRTMP_ADAPTER_HALT_IN_PROGRESS | 
								fRTMP_ADAPTER_NIC_NOT_EXIST)))) 
	{
		/* do nothing and return directly. */
	}
	else 
	{	
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET))
		{
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);	
		}
		else
		{	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
			if ((pAd->TxSwQueue[BulkOutPipeId].Number > 0) && 
				(pAd->DeQueueRunning[BulkOutPipeId] == FALSE) &&
				(pHTTXContext->bCurWriting == FALSE))
			{
				RTMPDeQueuePacket(pAd, FALSE, BulkOutPipeId, WCID_ALL, MAX_TX_PROCESS);
			}
			
			RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NORMAL);
			RTUSBKickBulkOut(pAd);
		}
	}

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<---rtusb_ac0_dma_done_tasklet\n"));
#endif /* RELEASE_EXCLUDE */

	return;

}


#ifdef CONFIG_ATE
static void rtusb_ate_ac0_dma_done_tasklet(unsigned long data)
{
	PRTMP_ADAPTER pAd;
	PTX_CONTEXT pNullContext;
	UCHAR BulkOutPipeId;
	NTSTATUS Status;
	ULONG IrqFlags;
	ULONG OldValue;
	purbb_t pURB;

	pURB = (purbb_t)data;
	pNullContext = (PTX_CONTEXT)pURB->rtusb_urb_context;
	pAd = pNullContext->pAd;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("--->ATE_RTUSBBulkOutDataPacketComplete\n"));
#endif /* RELEASE_EXCLUDE */
	
	/* Reset Null frame context flags */
	pNullContext->IRPPending = FALSE;
	pNullContext->InUse = FALSE;
	Status = pURB->rtusb_urb_status;

	/* Store BulkOut PipeId. */
	BulkOutPipeId = pNullContext->BulkOutPipeId;
	pAd->BulkOutDataOneSecCount++;
	
	if (Status == USB_ST_NOERROR)
	{	
#ifdef CONFIG_QA
		if ((ATE_ON(pAd)) && (pAd->ate.bQATxStart == TRUE))
		{
			if (pAd->ate.QID == BulkOutPipeId)
			{
				/*
					Let Rx can have a chance to break in during Tx process,
					especially for loopback mode in QA ATE.

					To trade off between tx performance and loopback mode integrity.

					Q   : Now Rx is handled by tasklet, do we still need this delay ?
					Ans : Even tasklet is used, Rx/Tx < 1 if we do not delay for a while right here.
				*/
				RtmpusecDelay(500);
				pAd->ate.TxDoneCount++;
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("pAd->ate.TxDoneCount == %d\n", pAd->ate.TxDoneCount));
#endif /* RELEASE_EXCLUDE */
				pAd->RalinkCounters.KickTxCount++;
				ASSERT(pAd->ate.QID == 0);
				pAd->ate.TxAc0++;
			}
		}
#endif /* CONFIG_QA */
		pAd->BulkOutComplete++;

		pAd->Counters8023.GoodTransmits++;

		RTMPDeQueuePacket(pAd, TRUE, BulkOutPipeId, WCID_ALL, MAX_TX_PROCESS);

	}
	else
	{
		pAd->BulkOutCompleteOther++;

		DBGPRINT(RT_DEBUG_ERROR, ("BulkOutDataPacket Failed STATUS_OTHER = 0x%x . \n", Status));
		DBGPRINT(RT_DEBUG_ERROR, (">>BulkOutReq=0x%lx, BulkOutComplete=0x%lx\n", pAd->BulkOutReq, pAd->BulkOutComplete));

		if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET)))
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET);

			/* In 28xx, RT_OID_USB_RESET_BULK_OUT ==> CMDTHREAD_RESET_BULK_OUT */
			RTEnqueueInternalCmd(pAd, CMDTHREAD_RESET_BULK_OUT, NULL, 0);

			/* check */
			BULK_OUT_LOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);
			pAd->BulkOutPending[BulkOutPipeId] = FALSE;
			pAd->bulkResetPipeid = BulkOutPipeId;
			BULK_OUT_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);

			return;
		}
	}

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->pAd = 0x%lx\n", (ULONG)&pNullContext->pAd));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->pUrb = 0x%lx\n", (ULONG)&pNullContext->pUrb));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->TransferBuffer = 0x%lx\n", (ULONG)&pNullContext->TransferBuffer));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->BulkOutPipeId = %d\n", pNullContext->BulkOutPipeId));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->BulkOutSize = %ld\n", pNullContext->BulkOutSize));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->InUse = %d\n", (pNullContext->InUse==TRUE)));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->bWaitingBulkOut = %d\n", (pNullContext->bWaitingBulkOut==TRUE)));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->IRPPending = %d\n", (pNullContext->IRPPending==TRUE)));
	DBGPRINT(RT_DEBUG_OFF, ("pNullContext->LastOne = %d\n", (pNullContext->LastOne==TRUE)));
#endif /* RELEASE_EXCLUDE */
	

	if (atomic_read(&pAd->BulkOutRemained) > 0)
	{			
		atomic_dec(&pAd->BulkOutRemained);
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("Bulk Out Remained = %d\n", atomic_read(&pAd->BulkOutRemained)));
#endif /* RELEASE_EXCLUDE */
	}
	
	/* 1st - Transmit Success */
	OldValue = pAd->WlanCounters.TransmittedFragmentCount.u.LowPart;
	pAd->WlanCounters.TransmittedFragmentCount.u.LowPart++;

	if (pAd->WlanCounters.TransmittedFragmentCount.u.LowPart < OldValue)
	{
		pAd->WlanCounters.TransmittedFragmentCount.u.HighPart++;
	}
	
	if (((pAd->ContinBulkOut == TRUE ) ||(atomic_read(&pAd->BulkOutRemained) > 0))
		&& (pAd->ate.Mode & ATE_TXFRAME))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("Continue to BulkOut ! \n"));
#endif /* RELEASE_EXCLUDE */
		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);
	}	
	else
	{
		RTUSB_CLEAR_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);
#ifdef CONFIG_QA
		pAd->ate.TxStatus = 0;
#endif /* CONFIG_QA */
	}

	BULK_OUT_LOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);
	pAd->BulkOutPending[BulkOutPipeId] = FALSE;
	BULK_OUT_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);	

	/* Always call Bulk routine, even reset bulk. */
	/* The protection of rest bulk should be in BulkOut routine. */
	RTUSBKickBulkOut(pAd);
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<---ATE_RTUSBBulkOutDataPacketComplete\n"));
#endif /* RELEASE_EXCLUDE */

}
#endif /* CONFIG_ATE */


NDIS_STATUS RtmpNetTaskInit(
	IN RTMP_ADAPTER *pAd)
{
	UINT32 Result;
	
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;

	/* Create RTNetTask Queue and related service thread */
	Result = RTMPCreateNetTaskQueue("RTNetTask", RTMP_TX_TASK_PRIORITY, TX_NO_TIME_SLICE, &pObj->NetTaskQueue);

	if (Result != NDIS_STATUS_SUCCESS)
	{
		return Result;
	}
		
	/* Init Net task for RTNetTask Queue */
	RTMP_NET_TASK_INIT(&pObj->rx_done_task, rx_done_tasklet, (ULONG)pAd);
	RTMP_NET_TASK_INIT(&pObj->mgmt_dma_done_task, rtusb_mgmt_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac0_dma_done_task, rtusb_ac0_dma_done_tasklet, (unsigned long)pAd);
#ifdef CONFIG_ATE
	RTMP_NET_TASK_INIT(&pObj->ate_ac0_dma_done_task, rtusb_ate_ac0_dma_done_tasklet, (unsigned long)pAd);
#endif
	RTMP_NET_TASK_INIT(&pObj->ac1_dma_done_task, rtusb_ac1_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac2_dma_done_task, rtusb_ac2_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac3_dma_done_task, rtusb_ac3_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->hcca_dma_done_task, rtusb_hcca_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->tbtt_task, tbtt_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->null_frame_complete_task, rtusb_null_frame_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->pspoll_frame_complete_task, rtusb_pspoll_frame_done_tasklet, (unsigned long)pAd);

	return NDIS_STATUS_SUCCESS;
}


inline void RtmpNetTaskExit(IN RTMP_ADAPTER *pAd)
{
	POS_COOKIE pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	RTMPDestroyNetTaskQueue(&pObj->NetTaskQueue);	
}


/*
========================================================================
Routine Description:
    Create kernel threads & tasklets.

Arguments:
    *net_dev			Pointer to wireless net device interface

Return Value:
	NDIS_STATUS_SUCCESS
	NDIS_STATUS_FAILURE

Note:
========================================================================
*/
NDIS_STATUS	 RtmpMgmtTaskInit(
	IN RTMP_ADAPTER *pAd)
{
	RTMP_OS_TASK *pTask;
	NDIS_STATUS status;

	/*
		Creat TimerQ Thread, We need init timerQ related structure before create the timer thread.
	*/
	RtmpTimerQInit(pAd);

	pTask = &pAd->timerTask;
	dc_log_printf("TimerTask=0x%x", (ULONG)pTask);
	RTMP_OS_TASK_INIT(pTask, "RtTimerTask", pAd, &pAd->RscTaskMemList);
	status = RtmpOSTaskAttach(pTask, RtmpTimerQThread, (ULONG)pTask);
	if (status == NDIS_STATUS_FAILURE) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: unable to start RtmpTimerQThread\n", RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev)));
		return NDIS_STATUS_FAILURE;
	}
	

	/* Creat Command Thread */
	pTask = &pAd->cmdQTask;
	dc_log_printf("CmdTask=0x%x", (ULONG)pTask, &pAd->RscTaskMemList);
	RTMP_OS_TASK_INIT(pTask, "RtCmdQTask", pAd);
	status = RtmpOSTaskAttach(pTask, RTUSBCmdThread, (ULONG)pTask);
	if (status == NDIS_STATUS_FAILURE) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: unable to start RTUSBCmdThread\n", RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev)));
		return NDIS_STATUS_FAILURE;
	}

#ifdef WSC_INCLUDED
	/* start the crediential write task first. */
	WscThreadInit(pAd);
#endif /* WSC_INCLUDED */

	return NDIS_STATUS_SUCCESS;
}



/*
========================================================================
Routine Description:
    Close kernel threads.

Arguments:
	*pAd				the raxx interface data pointer

Return Value:
    NONE

Note:
========================================================================
*/
VOID RtmpMgmtTaskExit(
	IN RTMP_ADAPTER *pAd)
{
	INT			ret;
	RTMP_OS_TASK	*pTask;
	
	/* Sleep 50 milliseconds so pending io might finish normally */
	RtmpusecDelay(50000);

	/* We want to wait until all pending receives and sends to the */
	/* device object. We cancel any */
	/* irps. Wait until sends and receives have stopped. */
	RTUSBCancelPendingIRPs(pAd);

	/* We need clear timerQ related structure before exits of the timer thread. */
	RtmpTimerQExit(pAd);

	/* Terminate cmdQ thread */
	pTask = &pAd->cmdQTask;
#ifdef KTHREAD_SUPPORT
	if (pTask->kthread_task)
#else
	CHECK_PID_LEGALITY(pTask->taskPID)
#endif
	{
		NdisAcquireSpinLock(&pAd->CmdQLock);
		pAd->CmdQ.CmdQState = RTMP_TASK_STAT_STOPED;
		NdisReleaseSpinLock(&pAd->CmdQLock);
		/*RTUSBCMDUp(pAd); */
		ret = RtmpOSTaskKill(pTask);
		if (ret == NDIS_STATUS_FAILURE)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s: kill task(%s) failed!\n", 
					RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev), pTask->taskName));
		}
		pAd->CmdQ.CmdQState = RTMP_TASK_STAT_UNKNOWN;
	}

	/* Terminate timer thread */
	pTask = &pAd->timerTask;
	ret = RtmpOSTaskKill(pTask);
	if (ret == NDIS_STATUS_FAILURE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: kill task(%s) failed!\n", 
					RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev), pTask->taskName));
	}

#ifdef WSC_INCLUDED
	WscThreadExit(pAd);
#endif /* WSC_INCLUDED */

}


#ifdef SIGMATEL_SDK
static void RtmpDcUSBComplete(
	IN DC_URB_COMPLETION_STATUS status,
	IN struct DC_URB_STRUCT *urb,
	IN void *arg)
{
	RTMP_URB_STRUCT *pUrbEntry, *pListHead;


	pUrbEntry = (RTMP_URB_STRUCT *)arg;
	urb->completionCallback = pUrbEntry->callBack;
	urb->completionCallbackArg = pUrbEntry->callBackArg;

	/*dc_log_printf("UrbComplete"); */
	NdisZeroMemory(pUrbEntry, sizeof(*pUrbEntry));
	if (pRtmpDcUrbFreeList == NULL)
	{
		pRtmpDcUrbFreeList = pUrbEntry;
	}
	else
	{
		pUrbEntry->next = pRtmpDcUrbFreeList;
		pRtmpDcUrbFreeList = pUrbEntry;
	}
	RtmpUrbPending = FALSE;
	
	RTMP_SEM_EVENT_UP(pRtmpDcUrbSema);

	if (urb->completionCallback)
		urb->completionCallback(status, urb, pUrbEntry->callBackArg);
	/*dc_log_printf("callBackDone\n"); */
}


DC_RETURN_CODE RtmpDcUSBTaskFunc(IN ULONG Context)
{
	RTMP_OS_TASK *pTask;
	DC_RETURN_CODE retCode = DC_SUCCESS;
	int status, epIdx = 0;
	UINT32 irqFlags;
	RTMP_URB_STRUCT *pUrbEntry;
	NDIS_STATUS submitStatus = NDIS_STATUS_FAILURE;
	dc_log_printf("Into RtmpDcUSBTask\n");
	pTask = (RTMP_OS_TASK *)Context;
	DC_URB *pUrb;
	UCHAR  *pPMKBuf, *pSSIDStr;
	INT		ssidLen;
	PRTMP_ADAPTER pAd = NULL;

	while(1)
	{

		RTMP_SEM_EVENT_WAIT(pRtmpDcUrbSema, status);
		if (status != TX_SUCCESS)
		{
			dc_log_printf("%s:waitingFailed\n", pTask->taskName);
			continue;
		}

		pAd = (RTMP_ADAPTER *)RT_WIFI_DevCtrl;
		pPMKBuf = pAd->StaCfg.PMK;
		pSSIDStr = (PUCHAR)pAd->StaCfg.WscControl.WscProfile.Profile[0].SSID.Ssid;
		ssidLen = pAd->StaCfg.WscControl.WscProfile.Profile[0].SSID.SsidLength;
		DBGPRINT(RT_DEBUG_TRACE, ("aaa : %x, %s, %s, %s\n", (UINT32)pAd, pAd->StaCfg.WscControl.WscProfile.Profile[0].Key, pAd->StaCfg.WpaPassPhrase, pSSIDStr));
		RT_CfgSetWPAPSKKey(pAd, pAd->StaCfg.WscControl.WscProfile.Profile[0].Key, pSSIDStr, ssidLen, pPMKBuf);
		DBGPRINT(RT_DEBUG_TRACE, ("bbb : %x, %s, %s, %s\n", (UINT32)pAd, pAd->StaCfg.WscControl.WscProfile.Profile[0].Key, pAd->StaCfg.WpaPassPhrase, pSSIDStr));
		
#if 0
		/*dc_log_printf("WakeUp,Urb=%d", RtmpUrbWaitingCnt); */
		irqFlags = tx_interrupt_control(TX_INT_DISABLE);
		if (RtmpUrbPending == TRUE)
		{
			tx_interrupt_control(irqFlags);
			dc_log_printf("%s:UrbPending\n", pTask->taskName);
			continue;
		}
		
		/* We can do URB submit now */
		epIdx = RtmpUrbSubmitIdx;
		do
		{
			INC_RING_INDEX(epIdx, RTMP_DC_URB_WAIT_QUEUE_NUM);
			if(pRtmpDcUrbWaitList[epIdx])
			{
				pUrbEntry = pRtmpDcUrbWaitList[epIdx];
				pRtmpDcUrbWaitList[epIdx] = pUrbEntry->next;

				RtmpUrbPending = TRUE;
				RtmpUrbWaitingCnt--;
				/*dc_log_printf("[Dq:0x%x,Len=%d]", pUrbEntry->pOSURB, pUrbEntry->pOSURB->requestedLength); */
				/*if(pUrbEntry->next || RtmpUrbWaitingCnt) */
				/*	dc_log_printf("StillHasUrbInQ[0x%x, %d]", pUrbEntry->next, RtmpUrbWaitingCnt); */
				
				tx_interrupt_control(irqFlags);

				submitStatus = usb_submit_urb(pUrbEntry->pOSURB);
				if (submitStatus == NDIS_STATUS_FAILURE)
				{
					dc_log_printf("submit URB failed\n");
					irqFlags = tx_interrupt_control(TX_INT_DISABLE);
					RtmpUrbPending = FALSE;
					pUrbEntry->bUrbEnqueueStatus = DC_URB_UNKNOWN_ERROR;
					if (pUrbEntry->callBack != NULL)
						pUrbEntry->callBack(DC_URB_UNKNOWN_ERROR, pUrbEntry->callBackArg, pUrbEntry->callBackArg);
					tx_interrupt_control(irqFlags);
				}
				else
				{
					/*dc_log_printf("SMDone,ep:%d,urb:%x", epIdx, pUrbEntry->pOSURB); */
				}

				RtmpUrbSubmitIdx = epIdx;

				if(0)
				{
					DC_USB_DEVICE *pUSBDev = NULL;
					DC_USB_PIPE *pUSBPipe = pUrbEntry->pOSURB->pipe;
					int id;
					struct DC_SGTL_USB_HC *oc;
						
					pUSBDev = GetUSBDevFromUSBPipe(pUSBPipe);

					id = pUSBDev->hcd->id;
					oc = (struct DC_SGTL_USB_HC *)&sgtl_host_controllers[id];
					
					irqFlags = tx_interrupt_control(TX_INT_DISABLE);
					oc->timer.tx_timer_internal.tx_timer_internal_timeout_function(id);
					tx_interrupt_control(irqFlags);
				}
				break;
			}
		}while(	epIdx != RtmpUrbSubmitIdx);
#endif
	}

	return retCode;
}


/*
	This functon used to create a Ralink wrapper URB which contant the DC_URB, 
	and insert this URB into the corresponding RtmpDcUrbWaitingList depends on the
	number endpoint.

		pRtmpDcUrbWaitList[0] for endpoint 0, i.e., the control pipe.
		pRtmpDcUrbWaitList[1~4] for bulk out endpint 1~4, i.e., the AC0~AC3

	We don't take care those URBs for Bulk-In access, because we only has one 
	active bulk-in URB at one time.

	For the contentation of BulkIn and BulkOut/Ctrl access, we left it to the system
	USB core system to take care that.
 */
int RtmpDcUSB_Enqueue(DC_URB *pOSURB)
{
	RTMP_URB_STRUCT *pUrbEntry = NULL, *pEntryList;
	UINT32 irqFlags;
	int status = NDIS_STATUS_FAILURE;
	DC_URB *pUrb;
	DC_USB_PIPE *pUSBPipe = pOSURB->pipe;
	UINT8 bEPAddr;


	bEPAddr = pUSBPipe->bEndpointAddress;
	if (( bEPAddr & 0x80) == DC_USB20_ENDPOINT_IN) /* Bit 7 of EndpointAddress field */
	{
		/*dc_log_printf("BulkIn Access, directly pass!"); */
		return usb_submit_urb(pOSURB);
	}
	
	irqFlags = tx_interrupt_control(TX_INT_DISABLE);
	if (pRtmpDcUrbFreeList)
	{
		pUrbEntry = pRtmpDcUrbFreeList;
		pRtmpDcUrbFreeList = pRtmpDcUrbFreeList->next;
	}

	if (pUrbEntry)
	{
		pUrb = pOSURB;
		pUrbEntry->pOSURB = pOSURB;
		pUrbEntry->callBack = pOSURB->completionCallback;
		pUrbEntry->callBackArg = pOSURB->completionCallbackArg;
		pUrbEntry->bUrbEnqueueStatus = 0;
		pUrbEntry->status = 0;
		pUrbEntry->next = NULL;

		pUrb->completionCallback = RtmpDcUSBComplete;
		pUrb->completionCallbackArg = pUrbEntry;
		
		if (pRtmpDcUrbWaitList[bEPAddr] == NULL)
		{
			pRtmpDcUrbWaitList[bEPAddr] = pUrbEntry;
		}
		else
		{
			pEntryList = pRtmpDcUrbWaitList[bEPAddr];
			while(pEntryList->next != NULL)
				pEntryList = pEntryList->next;
			pEntryList->next = pUrbEntry;
		}

		{
			RtmpUrbWaitingCnt++;
			/*dc_log_printf("[Eq:%x,EP:%d,#:%d]", pOSURB, bEPAddr, RtmpUrbWaitingCnt); */
#if 0
			pEntryList = pRtmpDcUrbWaitList[bEPAddr];
			while(pEntryList!= NULL)
			{
				dc_log_printf("UrbList[0x%x:%d],nxt=0x%x", pEntryList->pOSURB, pEntryList->pOSURB->requestedLength, pEntryList->next);
				pEntryList = pEntryList->next;
			}
#endif
		}
		status = NDIS_STATUS_SUCCESS;
	}
	tx_interrupt_control(irqFlags);

	if (status == NDIS_STATUS_SUCCESS)
		RTMP_SEM_EVENT_UP(pRtmpDcUrbSema);

	return status;
}


DC_RETURN_CODE RtmpDcUSBTaskInit(void)
{
	DC_RETURN_CODE retCode;
	int idx = 0;
	RTMP_URB_STRUCT *pList;

	/* Init the free list of wrapper urb structures */
	memset(&RtmpDcUrbList[0], 0, sizeof(RtmpDcUrbList));
	pRtmpDcUrbFreeList = &RtmpDcUrbList[0];
	while(idx < (RTMP_DC_URB_MAX_NUM -1))
	{
		pList = &RtmpDcUrbList[idx];
		idx++;
		pList->next = &RtmpDcUrbList[idx];
	}
	
	/* Init waiting list, 0 for CtrlPipe, 1~ 4 for AC0~AC3 */
	for (idx = 0; idx < NUM_OF_TX_RING; idx++)
		pRtmpDcUrbWaitList[idx] = NULL;

	/* Init the urb complete list */
	pRtmpDcUrbDoneList = NULL;

	/* Init the URB mgmt  thread and kick USB Submit Mgmt Task to start */
	memset(&RtmpDcUSBTask, 0, sizeof(RtmpDcUSBTask));
	RtmpDcUSBTask.taskPID = &RTMP_DC_USB_TASK;
	NdisMoveMemory(&RtmpDcUSBTask.taskName[0], "RtmpDcUSB", sizeof("RtmpDcUSB")); 	
	RTMP_SEM_EVENT_INIT_LOCKED(&(RtmpDcUSBTask.taskSema), NULL);
	pRtmpDcUrbSema = &(RtmpDcUSBTask.taskSema);
	retCode = tx_thread_create(&RTMP_DC_USB_TASK,
							"RtmpDcUSBTask",
							RtmpDcUSBTaskFunc,
							&RtmpDcUSBTask,
							RtmpDcUSBTaskStack,
							sizeof(RtmpDcUSBTaskStack),
							RTMP_TX_TASK_PRIORITY,
							RTMP_TX_TASK_PRIORITY,
							1,
							TX_AUTO_START);
	if (retCode != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Create USBTask failed!\n"));
	}

	return retCode;
}

#endif /* SIGMATEL_SDK */

