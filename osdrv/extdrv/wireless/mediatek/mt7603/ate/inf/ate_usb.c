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
	ate_usb.c

	Abstract:

	Revision History:
	Who			When	    What
	--------	----------  ----------------------------------------------
	Name		Date	    Modification logs
*/

#ifdef RTMP_MAC_USB

#include "rt_config.h"

extern UCHAR EpToQueue[];
/* 802.11 MAC Header, Type:Data, Length:24bytes + 6 bytes QOS/HTC + 2 bytes padding */
extern UCHAR TemplateFrame[32];

INT TxDmaBusy(RTMP_ADAPTER *pAd)
{
	INT result;
	USB_DMA_CFG_STRUC UsbCfg;
	BOOLEAN is_busy;

	USB_CFG_READ(pAd, &UsbCfg.word);
#ifdef MT7601
	if (IS_MT7601(pAd))
		is_busy = UsbCfg.field_7601.TxBusy;
	else
#endif /* MT7601 */
#ifdef MT76x2
	if (IS_MT76x2(pAd))
		is_busy = UsbCfg.field_76xx.TxBusy;
	else
#endif /* MT76x2 */
		is_busy = (BOOLEAN)UsbCfg.field.TxBusy;

	result = (is_busy) ? TRUE : FALSE;

	return result;
}


INT RxDmaBusy(RTMP_ADAPTER *pAd)
{
	INT result;
	USB_DMA_CFG_STRUC UsbCfg;
	BOOLEAN is_busy;

	USB_CFG_READ(pAd, &UsbCfg.word);
#ifdef MT7601
	if (IS_MT7601(pAd))
		is_busy = UsbCfg.field_7601.RxBusy;
	else
#endif /* MT7601 */
#ifdef MT76x2
	if (IS_MT76x2(pAd))
		is_busy = UsbCfg.field_76xx.RxBusy;
	else
#endif /* MT76x2 */
		is_busy = (BOOLEAN)UsbCfg.field.RxBusy;

	result = (is_busy) ? TRUE : FALSE;

	return result;
}


VOID RtmpDmaEnable(RTMP_ADAPTER *pAd, INT Enable)
{
	BOOLEAN value;
	ULONG WaitCnt;
	USB_DMA_CFG_STRUC UsbCfg;
	
	value = Enable > 0 ? 1 : 0;

	/* check DMA is in busy mode. */
	WaitCnt = 0;

	while (TxDmaBusy(pAd) || RxDmaBusy(pAd))
	{
		RtmpusecDelay(10);
		if (WaitCnt++ > 100)
			break;
	}
#ifdef RELEASE_EXCLUDE
	/* Why not to clear USB DMA TX path first ??? */
#endif /* RELEASE_EXCLUDE */

	USB_CFG_READ(pAd, &UsbCfg.word);
#ifdef MT7601
	if (IS_MT7601(pAd)) {
		UsbCfg.field_7601.TxBulkEn = value;
		UsbCfg.field_7601.RxBulkEn = value;
	}
	else
#endif /* MT7601 */
#ifdef MT76x2
	if (IS_MT76x2(pAd)) {
		UsbCfg.field_76xx.TxBulkEn = value;
		UsbCfg.field_76xx.RxBulkEn = value;
	}
	else
#endif /* MT76x2 */
	{
		UsbCfg.field.TxBulkEn = value;
		UsbCfg.field.RxBulkEn = value;
	}

	USB_CFG_WRITE(pAd, UsbCfg.word);
	RtmpOsMsDelay(5);

	return;
}

/* Nobody uses it currently */
#if 0
static VOID ATEWriteTxWI(
	IN	PRTMP_ADAPTER	pAd,
	IN	TXWI_STRUC *pTxWI,
	IN	BOOLEAN			FRAG,	
	IN	BOOLEAN			InsTimestamp,
	IN	BOOLEAN 		AMPDU,
	IN	BOOLEAN 		Ack,
	IN	BOOLEAN 		NSeq,		/* HW new a sequence. */
	IN	UCHAR			BASize,
	IN	UCHAR			WCID,
	IN	ULONG			Length,
	IN	UCHAR 			PID,
	IN	UCHAR			MIMOps,
	IN	UCHAR			Txopmode,	
	IN	BOOLEAN			CfAck,	
	IN	HTTRANSMIT_SETTING	Transmit)
{
#ifdef RELEASE_EXCLUDE
	/*
		Always use Long preamble before verifiation short preamble functionality works well.
		Todo: remove the following line if short preamble functionality works
	*/
#endif /* RELEASE_EXCLUDE */
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		struct  _TXWI_NMAC *txwi_n = (struct  _TXWI_NMAC *)pTxWI;

		txwi_n->FRAG = FRAG;
		txwi_n->TS = InsTimestamp;
		txwi_n->AMPDU = AMPDU;

		txwi_n->MIMOps = PWR_ACTIVE;
		txwi_n->MpduDensity = 4;
		txwi_n->ACK = Ack;
		txwi_n->txop = Txopmode;
		txwi_n->NSEQ = NSeq;
		txwi_n->BAWinSize = BASize;	

		txwi_n->wcid = WCID;
		txwi_n->MPDUtotalByteCnt = Length; 
		txwi_n->TxPktId = PID; 
		
		txwi_n->BW = Transmit.field.BW;
		txwi_n->ShortGI = Transmit.field.ShortGI;
		txwi_n->STBC= Transmit.field.STBC;
		
		txwi_n->MCS = Transmit.field.MCS;
		txwi_n->PHYMODE= Transmit.field.MODE;
		txwi_n->CFACK = CfAck;
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		struct  _TXWI_OMAC *txwi_o = (struct  _TXWI_OMAC *)pTxWI;

		txwi_o->FRAG= FRAG;
		txwi_o->TS = InsTimestamp;
		txwi_o->AMPDU = AMPDU;

		txwi_o->MIMOps = PWR_ACTIVE;
		txwi_o->MpduDensity = 4;
		txwi_o->ACK = Ack;
		txwi_o->txop = Txopmode;
		txwi_o->NSEQ = NSeq;
		txwi_o->BAWinSize = BASize;	

		txwi_o->wcid = WCID;
		txwi_o->MPDUtotalByteCnt = Length; 
		txwi_o->PacketId = PID; 
		
		txwi_o->BW = Transmit.field.BW;
		txwi_o->ShortGI = Transmit.field.ShortGI;
		txwi_o->STBC= Transmit.field.STBC;
		
		txwi_o->MCS = Transmit.field.MCS;
		txwi_o->PHYMODE= Transmit.field.MODE;
		txwi_o->CFACK = CfAck;
	}
#endif /* RTMP_MAC */

	return;
}


/*
========================================================================
	Routine	Description:
		Write TxInfo for ATE mode.
		
	Return Value:
		None
========================================================================
*/
static VOID ATEWriteTxInfo(
	IN	PRTMP_ADAPTER	pAd,
	IN	TXINFO_STRUC *pTxInfo,
	IN	USHORT		USBDMApktLen,
	IN	BOOLEAN		bWiv,
	IN	UCHAR			QueueSel,
	IN	UCHAR			NextValid,
	IN	UCHAR			TxBurst)
{
#if 0
	pTxInfo->TxInfoPktLen = USBDMApktLen;
	pTxInfo->TxInfoQSEL = QueueSel;

	if (QueueSel != FIFO_EDCA)
		DBGPRINT(RT_DEBUG_TRACE, ("======= QueueSel != FIFO_EDCA =======\n"));

	pTxInfo->TxInfoUDMANextVld = NextValid;
	pTxInfo->TxInfoUDMATxburst = TxBurst;
	pTxInfo->TxInfoWIV = bWiv;
#ifndef USB_BULK_BUF_ALIGMENT
	pTxInfo->TxInfoSwLstRnd = 0;
#else
	pTxInfo->bFragLasAlignmentsectiontRound = 0;
#endif /* USB_BULK_BUF_ALIGMENT */

	/* ATE doesn't support checksum offload. */
	pTxInfo->CSO = 1;
	pTxInfo->USO = 0;
	pTxInfo->TCPOffset = 0;
	pTxInfo->IPOffset = 0;
#else
//	rlt_usb_write_txinfo(pAd, pTxInfo, USBDMApktLen, bWiv, QueueSel, NextValid, TxBurst);
#endif
}
#endif


INT ATESetUpFrame(
	IN PRTMP_ADAPTER pAd,
	IN UINT32 TxIdx)
{

#if 0
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT pos = 0;
	PTX_CONTEXT	pNullContext;
	PUCHAR			pDest;
	HTTRANSMIT_SETTING	TxHTPhyMode;
	TXWI_STRUC *pTxWI;
	TXINFO_STRUC *pTxInfo;	
	UINT32			TransferBufferLength, OrgBufferLength = 0;
	UCHAR			padLen = 0;
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	UCHAR bw, sgi, stbc, mcs, phymode, frag, ts, ampdu, ack, nseq, basize, pid, txop, cfack;
	USHORT mpdu_len;
#ifdef CONFIG_QA
	PHEADER_802_11	pHeader80211 = NULL;
#endif /* CONFIG_QA */

	bw = sgi = stbc = mcs = phymode = frag = ts = ampdu = ack = nseq = basize = pid = txop = cfack = 0;
	mpdu_len = 0;

	if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		return -1;
	}

	/* We always use QID_AC_BE and FIFO_EDCA in ATE mode. */

	pNullContext = &(pAd->NullContext);
	ASSERT(pNullContext != NULL);
	
	if (pNullContext->InUse == FALSE)
	{
		/* set the in use bit */
		pNullContext->InUse = TRUE;
		NdisZeroMemory(&(pAd->NullFrame), sizeof(HEADER_802_11));
		
		/* fill 802.11 header */
#ifdef CONFIG_QA
		if (pATEInfo->bQATxStart == TRUE) 
		{
			pHeader80211 = NdisMoveMemory(&(pAd->NullFrame),
				pATEInfo->Header, pATEInfo->HLen);
		}
		else
#endif /* CONFIG_QA */
		{
			NdisMoveMemory(&(pAd->NullFrame), TemplateFrame,
			sizeof(HEADER_802_11));
		}

#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, (PUCHAR)&(pAd->NullFrame), DIR_READ, FALSE);
#endif /* RT_BIG_ENDIAN */

#ifdef CONFIG_QA
		if (pATEInfo->bQATxStart == TRUE) 
		{
			/* modify sequence number... */
			if (pATEInfo->TxDoneCount == 0)
			{
				pATEInfo->seq = pHeader80211->Sequence;
			}
			else
			{
				pHeader80211->Sequence = ++pATEInfo->seq;
			}
			/* We already got all the address fields from QA GUI. */
		}
		else
#endif /* CONFIG_QA */
		{
			COPY_MAC_ADDR(pAd->NullFrame.Addr1, pATEInfo->Addr1);
			COPY_MAC_ADDR(pAd->NullFrame.Addr2, pATEInfo->Addr2);
			COPY_MAC_ADDR(pAd->NullFrame.Addr3, pATEInfo->Addr3);
		}

		RTMPZeroMemory(&pAd->NullContext.TransferBuffer->field.WirelessPacket[0], TX_BUFFER_NORMSIZE);
		pTxInfo = (TXINFO_STRUC *)&pAd->NullContext.TransferBuffer->field.WirelessPacket[0];

#ifdef CONFIG_QA
		if (pATEInfo->bQATxStart == TRUE) 
		{
			/* Avoid to exceed the range of WirelessPacket[]. */
			ASSERT(pATEInfo->TxInfo.TxInfoPktLen <= (MAX_FRAME_SIZE - 34/* == 2312 */));
			NdisMoveMemory(pTxInfo, &(pATEInfo->TxInfo), sizeof(pATEInfo->TxInfo));
		}
		else
#endif /* CONFIG_QA */
		{
			/* Avoid to exceed the range of WirelessPacket[]. */
			ASSERT(pATEInfo->TxLength <= (MAX_FRAME_SIZE - 34/* == 2312 */));

			/* pTxInfo->TxInfoPktLen will be updated to include padding later */
			ATEWriteTxInfo(pAd, pTxInfo, (USHORT)(TXWISize + pATEInfo->TxLength)
			, TRUE, FIFO_EDCA, FALSE,  FALSE);
		}

		pTxWI = (TXWI_STRUC *)&pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE];

#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			bw = pATEInfo->TxWI.TXWI_N.BW;
			sgi = pATEInfo->TxWI.TXWI_N.ShortGI;
			stbc = pATEInfo->TxWI.TXWI_N.STBC;
			mcs = pATEInfo->TxWI.TXWI_N.MCS;
			phymode = pATEInfo->TxWI.TXWI_N.PHYMODE;
			frag = pATEInfo->TxWI.TXWI_N.FRAG;
			ts = pATEInfo->TxWI.TXWI_N.TS;
			ampdu = pATEInfo->TxWI.TXWI_N.AMPDU;
			ack =pATEInfo->TxWI.TXWI_N.ACK;
			nseq = pATEInfo->TxWI.TXWI_N.NSEQ;
			basize = pATEInfo->TxWI.TXWI_N.BAWinSize;
			mpdu_len = pATEInfo->TxWI.TXWI_N.MPDUtotalByteCnt;
			pid = pATEInfo->TxWI.TXWI_N.TxPktId;
			txop = pATEInfo->TxWI.TXWI_N.txop;
			cfack = pATEInfo->TxWI.TXWI_N.CFACK;
		}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			bw = pATEInfo->TxWI.TXWI_O.BW;
			sgi = pATEInfo->TxWI.TXWI_O.ShortGI;
			stbc = pATEInfo->TxWI.TXWI_O.STBC;
			mcs = pATEInfo->TxWI.TXWI_O.MCS;
			phymode = pATEInfo->TxWI.TXWI_O.PHYMODE;
			frag = pATEInfo->TxWI.TXWI_O.FRAG;
			ts = pATEInfo->TxWI.TXWI_O.TS;
			ampdu = pATEInfo->TxWI.TXWI_O.AMPDU;
			ack =pATEInfo->TxWI.TXWI_O.ACK;
			nseq = pATEInfo->TxWI.TXWI_O.NSEQ;
			basize = pATEInfo->TxWI.TXWI_O.BAWinSize;
			mpdu_len = pATEInfo->TxWI.TXWI_O.MPDUtotalByteCnt;
			pid = pATEInfo->TxWI.TXWI_O.PacketId;
			txop = pATEInfo->TxWI.TXWI_O.txop;
			cfack = pATEInfo->TxWI.TXWI_O.CFACK;
		}
#endif /* RTMP_MAC */

		/* fill TxWI */
		if (pATEInfo->bQATxStart == TRUE) 
		{
			TxHTPhyMode.field.BW = bw;
			TxHTPhyMode.field.ShortGI = sgi;
			TxHTPhyMode.field.STBC = stbc;
			TxHTPhyMode.field.MCS = mcs;
			TxHTPhyMode.field.MODE = phymode;
			ATEWriteTxWI(pAd, pTxWI, frag, ts,
				ampdu, ack, nseq, 
				basize, BSSID_WCID,
				mpdu_len /* include 802.11 header */,
				pid,
				0, txop/*IFS_HTTXOP*/, cfack
				/*FALSE*/, TxHTPhyMode);
		}
		else
		{
			TxHTPhyMode.field.BW = bw;
			TxHTPhyMode.field.ShortGI = sgi;
			TxHTPhyMode.field.STBC = 0;
			TxHTPhyMode.field.MCS = mcs;
			TxHTPhyMode.field.MODE = phymode;

			ATEWriteTxWI(pAd, pTxWI,  FALSE, FALSE, FALSE, FALSE
				/* No ack required. */, FALSE, 0, BSSID_WCID, pATEInfo->TxLength,
				0, 0, IFS_HTTXOP, FALSE, TxHTPhyMode);
#ifdef RELEASE_EXCLUDE
			/* "MMPS_STATIC" instead of "MMPS_DYNAMIC" ??? */
#endif /* RELEASE_EXCLUDE */
		}

		RTMPMoveMemory(&pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE + TXWISize],
			&pAd->NullFrame, sizeof(HEADER_802_11));

		pDest = &(pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE + TXWISize + sizeof(HEADER_802_11)]);

		/* prepare frame payload */
#ifdef CONFIG_QA
		if (pATEInfo->bQATxStart == TRUE) 
		{
			/* copy the pattern one by one to the frame payload */
			if ((pATEInfo->PLen != 0) && (pATEInfo->DLen != 0))
			{
				for (pos = 0; pos < pATEInfo->DLen; pos += pATEInfo->PLen)
				{
					RTMPMoveMemory(pDest, pATEInfo->Pattern, pATEInfo->PLen);
					pDest += pATEInfo->PLen;
				}
			}
			TransferBufferLength = TXINFO_SIZE + TXWISize + mpdu_len;
		}
		else
#endif /* CONFIG_QA */
		{
		    for (pos = 0; pos < (pATEInfo->TxLength - sizeof(HEADER_802_11)); pos++)
		    {
		    		if ( pATEInfo->bFixedPayload )
		    		{
					/* default payload is 0xA5 */
					*pDest = pATEInfo->Payload;
		    		} 
				else
				{
					*pDest = RandomByte(pAd);
		    		}
				pDest += 1;
		    }
			TransferBufferLength = TXINFO_SIZE + TXWISize + pATEInfo->TxLength;
		}

		OrgBufferLength = TransferBufferLength;
		TransferBufferLength = (TransferBufferLength + 3) & (~3);

		/* Always add 4 extra bytes at every packet. */
		padLen = TransferBufferLength - OrgBufferLength + 4;/* 4 == last packet padding */

		/* 
			RTMP_PKT_TAIL_PADDING == 11.
			[11 == 3(max 4 byte padding) + 4(last packet padding) + 4(MaxBulkOutsize align padding)]		
		*/
		ASSERT((padLen <= (RTMP_PKT_TAIL_PADDING - 4/* 4 == MaxBulkOutsize alignment padding */)));

		/* Now memzero all extra padding bytes. */
		NdisZeroMemory(pDest, padLen);
		pDest += padLen;

		/* Update pTxInfo->TxInfoPktLen to include padding. */
		pTxInfo->TxInfoPktLen = TransferBufferLength - TXINFO_SIZE;

		TransferBufferLength += 4;

		/* If TransferBufferLength is multiple of 64, add extra 4 bytes again. */
		if ((TransferBufferLength % pAd->BulkOutMaxPacketSize) == 0)
		{
			NdisZeroMemory(pDest, 4);
			TransferBufferLength += 4;
		}

		/* Fill out frame length information for global Bulk out arbitor. */
		pAd->NullContext.BulkOutSize = TransferBufferLength;
	}

#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange(pAd, (PUCHAR)pTxWI, TYPE_TXWI);
	RTMPFrameEndianChange(pAd, (((PUCHAR)pTxInfo) + TXWISize + TXINFO_SIZE), DIR_WRITE, FALSE);
	RTMPDescriptorEndianChange((PUCHAR)pTxInfo, TYPE_TXINFO);
#endif /* RT_BIG_ENDIAN */

	return 0;
#endif
	return 0;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:
		None

	Note:
	
========================================================================
*/
VOID ATE_RTUSBBulkOutDataPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BulkOutPipeId)
{
	PTX_CONTEXT		pNullContext = &(pAd->NullContext);
	PURB			pUrb;
	INT			ret = 0;
	ULONG			IrqFlags = 0;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("--->ATE_RTUSBBulkOutDataPacket \n"));
#endif /* RELEASE_EXCLUDE */

	ASSERT(BulkOutPipeId == 0);

	/* Build up the frame first. */
	BULK_OUT_LOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);

	if (pAd->BulkOutPending[BulkOutPipeId] == TRUE)
	{
		BULK_OUT_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);
		return;
	}

	pAd->BulkOutPending[BulkOutPipeId] = TRUE;
	BULK_OUT_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);

	/* Increase total transmit byte counter. */
	pAd->RalinkCounters.OneSecTransmittedByteCount +=  pNullContext->BulkOutSize; 
	pAd->RalinkCounters.TransmittedByteCount +=  pNullContext->BulkOutSize;

	/* Clear ATE frame bulk out flag. */
	RTUSB_CLEAR_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

	/* Init Tx context descriptor. */
	pNullContext->IRPPending = TRUE;
	RTUSBInitTxDesc(pAd, pNullContext, BulkOutPipeId,
		(usb_complete_t)RTUSBBulkOutDataPacketComplete);
	pUrb = pNullContext->pUrb;

	if ((ret = RTUSB_SUBMIT_URB(pUrb))!=0)
	{
		DBGPRINT_ERR(("ATE_RTUSBBulkOutDataPacket: Submit Tx URB failed %d\n", ret));
		return;
	}

	pAd->BulkOutReq++;
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<---ATE_RTUSBBulkOutDataPacket \n"));
#endif /* RELEASE_EXCLUDE */	

	return;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:
		None

	Note:
	
========================================================================
*/
VOID ATE_RTUSBCancelPendingBulkInIRP(
	IN	PRTMP_ADAPTER	pAd)
{
	PRX_CONTEXT		pRxContext = NULL;
	UINT			rx_ring_index;

	DBGPRINT(RT_DEBUG_TRACE, ("--->ATE_RTUSBCancelPendingBulkInIRP\n"));

	for (rx_ring_index = 0; rx_ring_index < (RX_RING_SIZE); rx_ring_index++)
	{
		pRxContext = &(pAd->RxContext[rx_ring_index]);
		RTUSB_UNLINK_URB(pRxContext->pUrb);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<---ATE_RTUSBCancelPendingBulkInIRP\n"));

	return;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:
		None

	Note:
	
========================================================================
*/
VOID ATEResetBulkIn(
	IN PRTMP_ADAPTER	pAd)
{
	if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT_ERR(("ATE : BulkIn IRP Pending!!!\n"));
		ATE_RTUSBCancelPendingBulkInIRP(pAd);
		RtmpOsMsDelay(100);
	}

	return;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:

	Note:
	
========================================================================
*/
INT ATEResetBulkOut(
	IN PRTMP_ADAPTER	pAd)
{

#if 0
	PATE_INFO pATEInfo = &(pAd->ate);
	PTX_CONTEXT	pNullContext = &(pAd->NullContext);
	INT ret=0;

	pNullContext->IRPPending = TRUE;

	/*
		If driver is still in ATE TXFRAME mode, 
		keep on transmitting ATE frames.
	*/
	DBGPRINT(RT_DEBUG_TRACE, ("pATEInfo->Mode == %d\npAd->ContinBulkOut == %d\npAd->BulkOutRemained == %d\n",
		pATEInfo->Mode, pAd->ContinBulkOut, atomic_read(&pAd->BulkOutRemained)));

	if ((pATEInfo->Mode == ATE_TXFRAME) && ((pAd->ContinBulkOut == TRUE) || (atomic_read(&pAd->BulkOutRemained) > 0)))
    {
		DBGPRINT(RT_DEBUG_TRACE, ("After CMDTHREAD_RESET_BULK_OUT, continue to bulk out frames !\n"));

		/* Init Tx context descriptor. */
		RTUSBInitTxDesc(pAd, pNullContext, 0/* pAd->bulkResetPipeid */, (usb_complete_t)RTUSBBulkOutDataPacketComplete);
		
		if ((ret = RTUSB_SUBMIT_URB(pNullContext->pUrb))!=0)
		{
			DBGPRINT_ERR(("ATE_RTUSBBulkOutDataPacket: Submit Tx URB failed %d\n", ret));
		}

		pAd->BulkOutReq++;
	}

	return ret;
#endif
	return 0;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:

	IRQL = 
	
	Note:
	
========================================================================
*/
VOID RTUSBRejectPendingPackets(
	IN	PRTMP_ADAPTER	pAd)
{

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("--->RejectPendingPackets\n"));
#endif /* RELEASE_EXCLUDE */

	rtmp_tx_swq_exit(pAd, WCID_ALL);

#ifdef RELEASE_EXCLUDE
	DBGPRINT_RAW(RT_DEBUG_INFO, ("<---RejectPendingPackets\n"));
#endif /* RELEASE_EXCLUDE */

}

#endif /* RTMP_MAC_USB */

