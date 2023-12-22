/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rtusb_bulk.c

	Abstract:

	Revision History:
	Who			When		What
	--------	----------	----------------------------------------------
	Name		Date		Modification logs
	
*/

#include "rt_config.h"


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

	kernelTimeSlice(100);
	/*
		Creat TimerQ Thread, We need init timerQ related structure before create the timer thread.
	*/
	RtmpTimerQInit(pAd);

	pTask = &pAd->timerTask;
	RTMP_OS_TASK_INIT(pTask, "RtmpTimerTask", pAd, &pAd->RscTaskMemList);
	status = RtmpOSTaskAttach(pTask, RtmpTimerQThread, (ULONG)pTask);
	if (status == NDIS_STATUS_FAILURE) 
	{
		DBGPRINT (RT_DEBUG_ERROR, ("%s: unable to start RtmpTimerQThread\n", RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev)));
		return NDIS_STATUS_FAILURE;
	}

	/* Creat Command Thread */
	pTask = &pAd->cmdQTask;
	RTMP_OS_TASK_INIT(pTask, "RtmpCmdQTask", pAd, &pAd->RscTaskMemList);
	status = RtmpOSTaskAttach(pTask, RTUSBCmdThread, (ULONG)pTask);
	if (status == NDIS_STATUS_FAILURE) 
	{
		DBGPRINT (RT_DEBUG_ERROR, (": unable to start RTUSBCmdThread\n", RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev)));
		return NDIS_STATUS_FAILURE;
	}

#ifdef WSC_INCLUDED
	/* start the crediential write task first. */
	WscThreadInit(pAd);
#endif /* WSC_INCLUDED */
	
	return NDIS_STATUS_SUCCESS;
}


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
	CHECK_PID_LEGALITY(pTask->taskPID)
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
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->RTUSBBulkOutDataPacketComplete\n"));
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

	RTMP_IRQ_LOCK(&pAd->BulkInLock, IrqFlags);
	pRxContext->InUse = FALSE;
	pRxContext->IRPPending = FALSE;
	pRxContext->BulkInOffset += pUrb->actual_length;
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
							Status, pAd->NextRxBulkInIndex, pAd->NextRxBulkInReadIndex, pRxContext->pUrb->actual_length));

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
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	/* Create receive tasklet */
	RTMP_NET_TASK_INIT(&pObj->rx_done_task, rx_done_tasklet, (ULONG)pAd);
	RTMP_NET_TASK_INIT(&pObj->mgmt_dma_done_task, rtusb_mgmt_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac0_dma_done_task, rtusb_ac0_dma_done_tasklet, (unsigned long)pAd);
#ifdef CONFIG_ATE
	RTMP_NET_TASK_INIT(&pObj->ate_ac0_dma_done_task, rtusb_ate_ac0_dma_done_tasklet, (unsigned long)pAd);
#endif /* CONFIG_ATE */
	RTMP_NET_TASK_INIT(&pObj->ac1_dma_done_task, rtusb_ac1_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac2_dma_done_task, rtusb_ac2_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac3_dma_done_task, rtusb_ac3_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->hcca_dma_done_task, rtusb_hcca_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->tbtt_task, tbtt_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->null_frame_complete_task, rtusb_null_frame_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->pspoll_frame_complete_task, rtusb_pspoll_frame_done_tasklet, (unsigned long)pAd);

	return NDIS_STATUS_SUCCESS;
}


void RtmpNetTaskExit(IN RTMP_ADAPTER *pAd)
{
	POS_COOKIE pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	/*
	tasklet_kill(&pObj->rx_done_task);
	tasklet_kill(&pObj->mgmt_dma_done_task);
	tasklet_kill(&pObj->ac0_dma_done_task);
#ifdef CONFIG_ATE
	tasklet_kill(&pObj->ate_ac0_dma_done_task);
#endif 
	tasklet_kill(&pObj->ac1_dma_done_task);
	tasklet_kill(&pObj->ac2_dma_done_task);
	tasklet_kill(&pObj->ac3_dma_done_task);
	tasklet_kill(&pObj->hcca_dma_done_task);
	tasklet_kill(&pObj->tbtt_task);
	tasklet_kill(&pObj->null_frame_complete_task);
	tasklet_kill(&pObj->pspoll_frame_complete_task);
	*/
}

inline void usb_init_urb(pUSBHST_URB pURB)
{
	memset(pURB, 0, sizeof(*pURB));
}


/*
 * usb_alloc_urb - create a new URB for USB driver to use 
 * @iso_packets: number of iso packets for this URB
 * @mem_flags: not used for vxWorks
 * Create an urb for USB driver to use, initialze urb,
 * If no memory is availabe, NULL is returned
 * If the URB type is control, interrupt, or bulk, iso_packets are passed zero
 *
 * The driver must call usb_free_urb when it is finished with the URB
 */
pUSBHST_URB usb_alloc_urb(int iso_packets, int mem_flags)
{
	pUSBHST_URB pURB;
	pURB = kmalloc(sizeof(*pURB) + iso_packets * sizeof(USBHST_ISO_PACKET_DESC));

	if (!pURB) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Failed: URB meory insufficient.\n", __FUNCTION__));
		return NULL;
	}
	
	usb_init_urb(pURB);
	
	return pURB;
}


/*
 * usb_free_urb - frees the memory used by an urb when all users of it are finished
 * pURB: pointer to the urb to free, maybe NULL
 * The transfer buffer associated with the urb is not freed, it must be freed elsewhere before usb_free_urb, 
 * otherwise the pTransferBuffer will losed, and memory leak.
 */
inline void usb_free_urb(pUSBHST_URB pURB)
{
	kfree(pURB);
}


/* 
 * usb_buffer_alloc - allocate dma-consistent(coherent) buffer 
 * @dev: device the buffer will be used with (not used for vxWorks)
 * @size: requested buffer size
 * @mem_flags: not used for vxWorks
 * @dma : not used in vxWorks because of not dma address field in URB, and the dma will be translate under host controller driver. 
 *
 * Return value is either null, or cpu-space pointer to buffer, 
 * When the buffe no longer used, use usb_buffer_free to release buffer space 
 */
void * usb_buffer_alloc(pUSB_DEVICE_INFO pUSBDEV, unsigned int  size, unsigned mem_flags, ra_dma_addr_t *DMAAddr)
{
	void *pVirtualAddress = NULL;
	pVirtualAddress = (void *)OS_MALLOC(size);

	if(!pVirtualAddress)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s : usb pool source is not available.\n", __FUNCTION__));	
		return NULL;
	}

	if(NULL != DMAAddr) 
	{
		DMAAddr = (ra_dma_addr_t *)CACHE_DMA_VIRT_TO_PHYS(pVirtualAddress);
	}

	/* Return virtual address */
	return pVirtualAddress;
}


inline void usb_buffer_free(void *pUSBBufferAddr)
{
	OS_FREE(pUSBBufferAddr);
}


/* Release the event set by URB holder to synchorize control transfer */
USBHST_STATUS usb_ctrl_msg_complete(pUSBHST_URB pURB) /* Ptr to USB Request Block */
{
		
    /* Check if pContext is valid */
    if ((NULL == pURB) || (NULL == pURB->pContext)) 
    {
        DBGPRINT(RT_DEBUG_ERROR, ("%s : URB context is NULL.\n", __FUNCTION__));
        return USBHST_FAILURE;
    }

    /* Release the event */
 	semGive((SEM_ID)pURB->pContext);
	
	return USBHST_SUCCESS;
}


/*
 * usb_control_msg - Builds a control urb, sends it off, and wait for completion until timeout value)
 * @dev: USB device info
 * @pipe: USB device default pipe
 * @bmRequestType: USB message request type
 * @bRequest: USB message request
 * @wvalue: USB message value
 * @wIndex: USB
 * @pwLength : length of transferred data
 * @pBuffer: pointer to data buffer by user space (may not coherent)
 * @timeout: time to wait for message to complete
 * 
 *  !! Can not call this API in "INTERRUPT" context, otherwise lead to wait forever status
 */
USBHST_STATUS usb_control_msg(pUSB_DEVICE_INFO dev, UINT32 endpointaddress, UINT8 bRequest, UINT8 bmRequestType, 
									UINT16 wValue, UINT16 wIndex, void *pBuffer, UINT16 wLength, int timeout)
{
	
	/* To store the setup packet */
    pUSBHST_SETUP_PACKET    pSetup = NULL;
	
    /* To store the USB request block */
    pUSBHST_URB             pURB = NULL;

	/* To send/receive usb buffer to/from device/host */
	UCHAR                    *USBBuffer = NULL;

    /* To store the Event created*/
    SEM_ID             EventId = OS_INVALID_EVENT_ID;

    /* To store the usb status returned on submission of request */
    USBHST_STATUS           Result = USBHST_FAILURE;

   	/* Check if pDeviceInfo is not null */
	if (!dev) 
	{
        DBGPRINT(RT_DEBUG_ERROR, ("%s : Invalide Device Information.\n", __FUNCTION__));
		Result = USBHST_INVALID_PARAMETER;
        goto ERROR0;
	}

	/* Allocate coherent memory for USBHST_SETUP_PACKET for DMA cache coherent transfer*/
    pSetup = (pUSBHST_SETUP_PACKET)OS_MALLOC(sizeof(USBHST_SETUP_PACKET));

 	/* Check if memory is allocated for pSetup */
    if (!pSetup) 
	{
    	DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficient Resource for Setup packet.\n", __FUNCTION__));
		Result = USBHST_INSUFFICIENT_RESOURCE; 
		goto ERROR0;
    }

    /* Fill the setup packet memory area with zero */
	NdisZeroMemory(pSetup, sizeof(*pSetup));
	
    /* Allocate memory for USB Request block */
	pURB = usb_alloc_urb(0, 0);

    /* Check if memory is allocated for USB request block */
    if (!pURB) 
	{
    	DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficient Resource for URB.\n", __FUNCTION__));
		Result = USBHST_INSUFFICIENT_RESOURCE;
		/* Free the setup packet memory area allocated */
		goto ERROR1;
    }

	if(wLength != 0)
	{
		/* no memory flag, no dma address for URB in vxWorks */
		USBBuffer = (UCHAR *)usb_buffer_alloc(dev, wLength, 0, NULL);
	}

	if ((!USBBuffer) && (wLength != 0)) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficient Resource for usb buffer.\n", __FUNCTION__));
		Result = USBHST_INSUFFICIENT_RESOURCE;
		goto ERROR2;
	}

	/* Fill the usbBuffer packet memory with zero */
	if (wLength != 0) 
	{
		NdisZeroMemory(USBBuffer, wLength);
	}
	
	/* If Control endpoint for OUT transfer, Copy outside pBuffer to USB buffer for DMA cache coherent usage */
	if (((bmRequestType & 0x80) == 0 ) && (wLength != 0)) 
	{
		NdisMoveMemory(USBBuffer, (UCHAR *)pBuffer, wLength);
	}
	
    /* Create transfer completion event for making a blocking call */
	EventId = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

    /* Check if the eventid created is valid */
	if (!EventId) 
	{
	    DBGPRINT(RT_DEBUG_ERROR, ("%s : Insufficent Resource for completion event.\n", __FUNCTION__));
		Result = USBHST_INSUFFICIENT_RESOURCE;
		goto ERROR3;
	}

	/* Fill the  SETUP_PACKET with the information passed */
	USBHST_FILL_SETUP_PACKET(pSetup,
	                         bmRequestType,
	                         bRequest,
	                         wValue,
	                         wIndex,
	                         wLength);

	/* Convert the setup packet from CPU format to little endian format */
	OS_BUFFER_CPU_TO_LE(pSetup, sizeof(USBHST_SETUP_PACKET));

	/* Populate the pUrb structure */
	USBHST_FILL_CONTROL_URB(pURB,
	                        dev->uDeviceHandle,
	                        0,
	                        USBBuffer,
	                        wLength,
	                        USBHST_SHORT_TRANSFER_OK,
	                        pSetup,
	                        usb_ctrl_msg_complete,
	                        EventId,
	                        USBHST_FAILURE);

	/* Enqueue URB to Host controller driver */
	Result = usbHstURBSubmit(pURB);
	
	if (Result != USBHST_SUCCESS) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : call usbHstURBSubmit not successfully.\n", __FUNCTION__));
		goto ERROR4;		
	}
	
	/* Wait for the completion of the event */
	if (OS_WAIT_TIMEOUT == semTake(EventId, timeout)) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Timeout Occurred, RequestType = 0x%x, Request = 0x%x, wValue = 0x%x, wIndex = 0x%x, wLength = 0x%x.\n", 
									__FUNCTION__, bmRequestType, bRequest, wValue, wIndex, wLength));

		/* Dequeue URB from Host controller driver */
		usbHstURBCancel(pURB);
		Result = USBHST_TIMEOUT;
		goto ERROR4;
	}
	   	
	/* Update the return status of URB */
	Result = pURB->nStatus;

	/* If Control endpoint for IN transfer, Copy USB buffer  to pBuffer */
	if (((bmRequestType & 0x80) != 0) && (wLength != 0) && (Result == USBHST_SUCCESS)) 
	{
		NdisMoveMemory((UCHAR *)pBuffer, USBBuffer, wLength);
	}
	   	
	/* Continue free resource allocated here */
	;
	   	
ERROR4:         
	/* Destroy the event */
	OS_DESTROY_EVENT(EventId);

ERROR3:
	/* Free USB buffer */
	if (wLength != 0) 
	{
    	usb_buffer_free((void *)USBBuffer);
	}

ERROR2:
	/* Free the USB request packet memory area allocated */
	usb_free_urb(pURB);	

ERROR1:
	/* Free the setup packet memory area allocated */
	OS_FREE(pSetup);

ERROR0:
	return Result;
}


/*
 * usb_unlink_urb : abort a URB transfer for a endpoint 
 * @pURB: pointer to URB previously submit 
 * 
 * Usually to dequeu host controller driver specific data structure for URB, 
 * and call completion callback function
 */
USBHST_STATUS usb_unlink_urb(pUSBHST_URB pURB)
{
	USBHST_STATUS Result = USBHST_FAILURE;
	
	if (!pURB) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : pURB[%p] is invalid.\n", __FUNCTION__, pURB));
		return USBHST_INVALID_PARAMETER;
	}
	
	/* Call USB core API to forward host controll driver to cancel URB*/
	Result = usbHstURBCancel(pURB);

	if (Result != USBHST_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : call usbHstURBCancel to unlink URB failed\n.", __FUNCTION__));
	}
	
	return Result;
}
