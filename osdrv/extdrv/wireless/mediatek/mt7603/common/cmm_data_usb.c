/*
   All functions in this file must be USB-depended, or you should out your function
	in other files.

*/

#ifdef RTMP_MAC_USB

#include	"rt_config.h"


NDIS_STATUS RTUSBFreeDescriptorRelease(RTMP_ADAPTER *pAd, UCHAR BulkOutPipeId)
{
	HT_TX_CONTEXT *pHTTXContext;
	unsigned long IrqFlags = 0;


	pHTTXContext = &pAd->TxContext[BulkOutPipeId];
	RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags);
	pHTTXContext->bCurWriting = FALSE;
	RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags);

	return NDIS_STATUS_SUCCESS;
}


/*
	========================================================================

	Routine	Description:
		This subroutine will scan through releative ring descriptor to find
		out avaliable free ring descriptor and compare with request size.

	Arguments:
		pAd	Pointer	to our adapter
		RingType	Selected Ring

	Return Value:
		NDIS_STATUS_FAILURE		Not enough free descriptor
		NDIS_STATUS_SUCCESS		Enough free descriptor

	Note:

	========================================================================
*/
NDIS_STATUS	RTUSBFreeDescRequest(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR BulkOutPipeId,
	IN UINT32 req_cnt)
{
	NDIS_STATUS	 Status = NDIS_STATUS_FAILURE;
	unsigned long IrqFlags = 0;
	HT_TX_CONTEXT *pHTTXContext;

#ifdef MULTI_WMM_SUPPORT
	if (BulkOutPipeId >= EDCA_WMM1_AC0_PIPE)
	{
		BulkOutPipeId -= EDCA_WMM1_AC0_PIPE;
		pHTTXContext = &pAd->TxContextWmm1[BulkOutPipeId];
		RTMP_IRQ_LOCK(&pAd->TxContextQueueWmm1Lock[BulkOutPipeId], IrqFlags);

		if ((pHTTXContext->CurWritePosition < pHTTXContext->NextBulkOutPosition) && ((pHTTXContext->CurWritePosition + req_cnt + LOCAL_TXBUF_SIZE) > pHTTXContext->NextBulkOutPosition))
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO,("RTUSBFreeD c1 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n",
						BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */

			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_WMM1_NORMAL << BulkOutPipeId));
		}
		else if ((pHTTXContext->CurWritePosition == 8) && (pHTTXContext->NextBulkOutPosition < (req_cnt + LOCAL_TXBUF_SIZE)))
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO,("RTUSBFreeD c2 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n",
						BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */
			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_WMM1_NORMAL << BulkOutPipeId));
		}
		else if (pHTTXContext->bCurWriting == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE,("RTUSBFreeD c3 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n", BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_WMM1_NORMAL << BulkOutPipeId));
		}
		else
		{
			Status = NDIS_STATUS_SUCCESS;
		}
		RTMP_IRQ_UNLOCK(&pAd->TxContextQueueWmm1Lock[BulkOutPipeId], IrqFlags);
	}
	else
#endif /* MULTI_WMM_SUPPORT */
	{
		/* get Tx Ring Resource & Dma Buffer address*/
#ifdef USE_BMC
		if (BulkOutPipeId == QID_BMC) {
			BulkOutPipeId = EDCA_BMC_PIPE;
		}
#endif
		pHTTXContext = &pAd->TxContext[BulkOutPipeId];
		RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags);
#ifdef USB_BULK_BUF_ALIGMENT
		if( ((pHTTXContext->CurWriteIdx< pHTTXContext->NextBulkIdx  ) &&   (pHTTXContext->NextBulkIdx - pHTTXContext->CurWriteIdx == 1))
			|| ((pHTTXContext->CurWriteIdx ==(BUF_ALIGMENT_RINGSIZE -1) ) &&  (pHTTXContext->NextBulkIdx == 0 )))
		{
			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));

		}
		else if (pHTTXContext->bCurWriting == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE,("BUF_ALIGMENT RTUSBFreeD c3 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n", BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
		}

#else
		if ((pHTTXContext->CurWritePosition < pHTTXContext->NextBulkOutPosition) && ((pHTTXContext->CurWritePosition + req_cnt + LOCAL_TXBUF_SIZE) > pHTTXContext->NextBulkOutPosition))
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO,("RTUSBFreeD c1 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n",
						BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */

			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
		}
		else if ((pHTTXContext->CurWritePosition == 8) && (pHTTXContext->NextBulkOutPosition < (req_cnt + LOCAL_TXBUF_SIZE)))
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO,("RTUSBFreeD c2 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n",
						BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */
			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
		}
		else if (pHTTXContext->bCurWriting == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE,("RTUSBFreeD c3 --> QueIdx=%d, CWPos=%ld, NBOutPos=%ld!\n", BulkOutPipeId, pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
			RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << BulkOutPipeId));
		}
#endif /* USB_BULK_BUF_ALIGMENT */
		else
		{
			Status = NDIS_STATUS_SUCCESS;
		}
		RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags);
	}

	return Status;
}


BOOLEAN	RTUSBNeedQueueBackForAgg(RTMP_ADAPTER *pAd, UCHAR BulkOutPipeId)
{
	HT_TX_CONTEXT *pHTTXContext;
	BOOLEAN needQueBack = FALSE;
	unsigned long   IrqFlags = 0;


	pHTTXContext = &pAd->TxContext[BulkOutPipeId];

	RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags);
	if ((pHTTXContext->IRPPending == TRUE)  /*&& (pAd->TxSwQueue[BulkOutPipeId].Number == 0) */)
	{
#if 0
		if ((pHTTXContext->CurWritePosition <= 8) &&
			(pHTTXContext->NextBulkOutPosition > 8 && (pHTTXContext->NextBulkOutPosition+MAX_AGGREGATION_SIZE) < MAX_TXBULK_LIMIT))
		{
			needQueBack = TRUE;
		}
		else if ((pHTTXContext->CurWritePosition < pHTTXContext->NextBulkOutPosition) &&
				 ((pHTTXContext->NextBulkOutPosition + MAX_AGGREGATION_SIZE) < MAX_TXBULK_LIMIT))
		{
			needQueBack = TRUE;
		}
#else
		if ((pHTTXContext->CurWritePosition < pHTTXContext->ENextBulkOutPosition) &&
			(((pHTTXContext->ENextBulkOutPosition+MAX_AGGREGATION_SIZE) < MAX_TXBULK_LIMIT) || (pHTTXContext->CurWritePosition > MAX_AGGREGATION_SIZE)))
		{
			needQueBack = TRUE;
		}
#endif
		else if ((pHTTXContext->CurWritePosition > pHTTXContext->ENextBulkOutPosition) &&
				 ((pHTTXContext->ENextBulkOutPosition + MAX_AGGREGATION_SIZE) < pHTTXContext->CurWritePosition))
		{
			needQueBack = TRUE;
		}
	}
	RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[BulkOutPipeId], IrqFlags);

	return needQueBack;

}


/*
	========================================================================

	Routine	Description:
		Calculates the duration which is required to transmit out frames
	with given size and specified rate.

	Arguments:
		pTxD		Pointer to transmit descriptor
		Ack			Setting for Ack requirement bit
		Fragment	Setting for Fragment bit
		RetryMode	Setting for retry mode
		Ifs			Setting for IFS gap
		Rate		Setting for transmit rate
		Service		Setting for service
		Length		Frame length
		TxPreamble  Short or Long preamble when using CCK rates
		QueIdx - 0-3, according to 802.11e/d4.4 June/2003

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	========================================================================
*/
VOID rlt_usb_write_txinfo(
	IN RTMP_ADAPTER *pAd,
	IN TXINFO_STRUC *pTxInfo,
	IN USHORT USBDMApktLen,
	IN BOOLEAN bWiv,
	IN UCHAR QueueSel,
	IN UCHAR NextValid,
	IN UCHAR TxBurst)
{
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		struct _TXINFO_NMAC_PKT *nmac_info = (struct _TXINFO_NMAC_PKT *)pTxInfo;

		nmac_info->pkt_80211 = 1;
		nmac_info->info_type = 0;
		nmac_info->d_port = 0;
		nmac_info->cso = 0;
		nmac_info->tso = 0;

		nmac_info->pkt_len = USBDMApktLen;
		nmac_info->QSEL = QueueSel;
		if (QueueSel != FIFO_EDCA)
			DBGPRINT(RT_DEBUG_TRACE, ("====> QueueSel != FIFO_EDCA <====\n"));
		nmac_info->next_vld = FALSE;	/*NextValid;   Need to check with Jan about this.*/
		nmac_info->tx_burst = TxBurst;
		nmac_info->wiv = bWiv;
#ifndef USB_BULK_BUF_ALIGMENT
		nmac_info->rsv0 = 0;		/* TxInfoSwLstRnd */
#else
		nmac_info->rsv0 = 0;		/* TxInfoSwLstRnd */
#endif /* USB_BULK_BUF_ALIGMENT */
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		struct _TXINFO_OMAC *omac_info = (struct _TXINFO_OMAC *)pTxInfo;

		omac_info->USBDMATxPktLen = USBDMApktLen;
		omac_info->QSEL = QueueSel;
		if (QueueSel != FIFO_EDCA)
			DBGPRINT(RT_DEBUG_TRACE, ("====> QueueSel != FIFO_EDCA <====\n"));
		omac_info->USBDMANextVLD = FALSE;	/*NextValid;   Need to check with Jan about this.*/
		omac_info->USBDMATxburst = TxBurst;
		omac_info->WIV = bWiv;

#ifdef CONFIG_CSO_SUPPORT
		if (IS_ASIC_CAP(pAd, fASIC_CAP_CSO))
		{
			omac_info->cso = 1;
			omac_info->uso = 0;
			omac_info->TCPOffset = 0;
			omac_info->IPOffset = 0;
		}
#endif /* CONFIG_CSO_SUPPORT */
#ifndef USB_BULK_BUF_ALIGMENT
		omac_info->SwUseLastRound = 0;
#else
		omac_info->bFragLasAlignmentsectiontRound = 0;
#endif /* USB_BULK_BUF_ALIGMENT */

	}
#endif /* RTMP_MAC */

}

#ifdef CONFIG_TSO_SUPPORT
static VOID rlt_usb_update_txinfo(
	IN RTMP_ADAPTER *pAd,
	IN TXINFO_STRUC *pTxInfo,
	IN TX_BLK *pTxBlk)
{
#ifdef RLT_MAC
#ifdef CONFIG_TSO_SUPPORT
	if (pTxBlk->naf_type) {
		struct _TXINFO_NMAC_PKT *nmac_info = (struct _TXINFO_NMAC_PKT *)pTxInfo;

		nmac_info->cso = (pTxBlk->naf_type & NAF_TYPE_CSO) ? 1 : 0;
		nmac_info->tso = (pTxBlk->naf_type & NAF_TYPE_TSO ? 1 : 0);
	}
#endif /* CONFIG_TSO_SUPPORT */
#endif /* RLT_MAC */
}
#endif /* CONFIG_TSO_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
VOID ComposePsPoll(RTMP_ADAPTER *pAd)
{
	TXINFO_STRUC *pTxInfo;
	TXWI_STRUC *pTxWI;
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	UCHAR *buf;
	USHORT data_len;
	MAC_TX_INFO mac_info;
#ifdef MT_MAC
	UCHAR *tmac_info;
	UINT tx_hw_hdr_len = pAd->chipCap.tx_hw_hdr_len;
	TMAC_TXD_L *pTxD = NULL;
#endif

	NdisZeroMemory(&pAd->PsPollFrame, sizeof (PSPOLL_FRAME));

	if (pAd->CommonCfg.bAPSDForcePowerSave)
		pAd->PsPollFrame.FC.PwrMgmt = PWR_SAVE;
	else
		pAd->PsPollFrame.FC.PwrMgmt = (RtmpPktPmBitCheck(pAd) == TRUE);

	pAd->PsPollFrame.FC.Type = FC_TYPE_CNTL;
	pAd->PsPollFrame.FC.SubType = SUBTYPE_PS_POLL;
	pAd->PsPollFrame.Aid = pAd->StaActive.Aid | 0xC000;
	COPY_MAC_ADDR(pAd->PsPollFrame.Bssid, pAd->CommonCfg.Bssid);
	COPY_MAC_ADDR(pAd->PsPollFrame.Ta, pAd->CurrentAddress);

	buf = &pAd->PsPollContext.TransferBuffer->field.WirelessPacket[0];
	RTMPZeroMemory(buf, 100);
	pTxInfo = (TXINFO_STRUC *)buf;
	pTxWI = (TXWI_STRUC *)&buf[TXINFO_SIZE];
	data_len = sizeof (PSPOLL_FRAME);
	rlt_usb_write_txinfo(pAd, pTxInfo, data_len + TXWISize + TSO_SIZE, TRUE,
						EpToQueue[MGMTPIPEIDX], FALSE, FALSE);

	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));

#ifdef MT_MAC
	tmac_info = &buf[0];
	mac_info.hdr_len = 16;
#endif

	mac_info.FRAG = FALSE;

	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = FALSE;
	mac_info.AMPDU = FALSE;

	mac_info.BM = 0;
	mac_info.Ack = TRUE;
	mac_info.NSeq = FALSE;
	mac_info.BASize = 0;

	mac_info.WCID = BSSID_WCID;
	mac_info.Length = data_len;
	mac_info.PID = 0;

	mac_info.TID = 0;
	mac_info.TxRate = (UCHAR) pAd->CommonCfg.MlmeTransmit.field.MCS;
	mac_info.Txopmode = IFS_BACKOFF;
	mac_info.Preamble = LONG_PREAMBLE;
#ifdef MT_MAC
	/*mac_info.Type = FC_TYPE_CNTL;*/
	/*mac_info.SubType = SUBTYPE_PS_POLL;*/
	mac_info.q_idx = Q_IDX_AC0;
#endif /* MT_MAC */

	mac_info.SpeEn = 1;

	/* For  MT STA LP control, use H/W control mode for PM bit */
#if defined(CONFIG_STA_SUPPORT) && defined(CONFIG_PM_BIT_HW_MODE)
	mac_info.PsmBySw = 0;
#else
	mac_info.PsmBySw = 1;
#endif /* CONFIG_STA_SUPPORT && CONFIG_PM_BIT_HW_MODE */

	write_tmac_info(pAd, (UCHAR *)tmac_info, &mac_info, &pAd->CommonCfg.MlmeTransmit);

	/*This frame is PS-POLL, duration filed is replace by AID, and duration=1 means H/W won't overwtite this field */
	pTxD = (TMAC_TXD_L *)tmac_info;
	pTxD->txd_2.duration = 1;

#ifdef MT_MAC
	RTMPMoveMemory((VOID *)&buf[tx_hw_hdr_len], (VOID *)&pAd->PsPollFrame, data_len);
	/* Append 4 extra zero bytes. */
	pAd->PsPollContext.BulkOutSize = tx_hw_hdr_len + data_len + 4;
#else
	RTMPMoveMemory((VOID *)&buf[TXWISize + TXINFO_SIZE + TSO_SIZE], (VOID *)&pAd->PsPollFrame, data_len);
	/* Append 4 extra zero bytes. */
	pAd->PsPollContext.BulkOutSize = TXINFO_SIZE + TXWISize + TSO_SIZE + data_len + 4;
#endif
	pAd->PsPollContext.BulkOutSize = ( pAd->PsPollContext.BulkOutSize + 3) & (~3);
}
#endif /* CONFIG_STA_SUPPORT */


/* IRQL = DISPATCH_LEVEL */
VOID ComposeNullFrame(RTMP_ADAPTER *pAd)
{
	TXINFO_STRUC *pTxInfo;
	TXWI_STRUC *pTxWI;
	UCHAR *buf;
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	UINT8 tx_hw_hdr_len = pAd->chipCap.tx_hw_hdr_len;
	USHORT data_len = sizeof(pAd->NullFrame);
	MAC_TX_INFO mac_info;

	NdisZeroMemory(&pAd->NullFrame, data_len);
	pAd->NullFrame.FC.Type = FC_TYPE_DATA;
	pAd->NullFrame.FC.SubType = SUBTYPE_DATA_NULL;
	pAd->NullFrame.FC.ToDs = 1;
	COPY_MAC_ADDR(pAd->NullFrame.Addr1, pAd->CommonCfg.Bssid);
	COPY_MAC_ADDR(pAd->NullFrame.Addr2, pAd->CurrentAddress);
	COPY_MAC_ADDR(pAd->NullFrame.Addr3, pAd->CommonCfg.Bssid);
	buf = &pAd->NullContext.TransferBuffer->field.WirelessPacket[0];
	RTMPZeroMemory(buf, 100);
	pTxInfo = (TXINFO_STRUC *)buf;
	pTxWI = (TXWI_STRUC *)&buf[TXINFO_SIZE];
	rlt_usb_write_txinfo(pAd, pTxInfo,
			(USHORT)(data_len + TXWISize + TSO_SIZE), TRUE,
			EpToQueue[MGMTPIPEIDX], FALSE, FALSE);

	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));
	mac_info.FRAG = FALSE;

	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = FALSE;
	mac_info.AMPDU = FALSE;

	mac_info.BM = IS_BM_MAC_ADDR(pAd->NullFrame.Addr1);
	mac_info.Ack = TRUE;
	mac_info.NSeq = FALSE;
	mac_info.BASize = 0;

	mac_info.WCID = BSSID_WCID;
	mac_info.Length = data_len;
	mac_info.PID = 0;

	mac_info.TID = 0;
	mac_info.TxRate = (UCHAR)pAd->CommonCfg.MlmeTransmit.field.MCS;
	mac_info.Txopmode = IFS_BACKOFF;
	mac_info.Preamble = LONG_PREAMBLE;
#ifdef MT_MAC
	// TODO: shiang-MT7603
	mac_info.q_idx = Q_IDX_AC0;
#endif /* MT_MAC */

	mac_info.SpeEn = 1;

	write_tmac_info(pAd, (UCHAR *)pTxWI, &mac_info, &pAd->CommonCfg.MlmeTransmit);

#ifdef MT7603
	// TODO: shiang-7603
	RTMPMoveMemory((VOID *)&buf[tx_hw_hdr_len], (VOID *)&pAd->NullFrame, data_len);
	pAd->NullContext.BulkOutSize = tx_hw_hdr_len + data_len + 4;
#else
	RTMPMoveMemory((VOID *)&buf[TXWISize + TXINFO_SIZE], (VOID *)&pAd->NullFrame, data_len);
	pAd->NullContext.BulkOutSize = TXINFO_SIZE + TXWISize + TSO_SIZE + data_len + 4;
#endif /* MT7603 */
}

#ifdef MULTI_WMM_SUPPORT
static inline NDIS_STATUS RtmpUSBWmm1CanDoWrite(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR QueIdx,
	IN HT_TX_CONTEXT *pHTTXContext)
{
	NDIS_STATUS	canWrite = NDIS_STATUS_RESOURCES;

	if (((pHTTXContext->CurWritePosition) < pHTTXContext->NextBulkOutPosition) && (pHTTXContext->CurWritePosition + LOCAL_TXBUF_SIZE) > pHTTXContext->NextBulkOutPosition)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c1!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_WMM1_NORMAL << QueIdx));
	}
	else if ((pHTTXContext->CurWritePosition == 8) && (pHTTXContext->NextBulkOutPosition < LOCAL_TXBUF_SIZE))
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c2!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_WMM1_NORMAL << QueIdx));
	}
	else if (pHTTXContext->bCurWriting == TRUE)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c3!\n"));
	}
	else if ((pHTTXContext->ENextBulkOutPosition == 8)  && ((pHTTXContext->CurWritePosition + 7912 ) > MAX_TXBULK_LIMIT)  )
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c4!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_WMM1_NORMAL << QueIdx));
	}
	else
	{
		canWrite = NDIS_STATUS_SUCCESS;
	}


	return canWrite;
}
#endif /* MULTI_WMM_SUPPORT */

/*
	We can do copy the frame into pTxContext when match following conditions.
		=>
		=>
		=>
*/
static inline NDIS_STATUS RtmpUSBCanDoWrite(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR QueIdx,
	IN HT_TX_CONTEXT *pHTTXContext)
{
	NDIS_STATUS	canWrite = NDIS_STATUS_RESOURCES;

#ifdef USB_BULK_BUF_ALIGMENT
	if( ((pHTTXContext->CurWriteIdx< pHTTXContext->NextBulkIdx  ) &&   (pHTTXContext->NextBulkIdx - pHTTXContext->CurWriteIdx == 1))
		|| ((pHTTXContext->CurWriteIdx ==(BUF_ALIGMENT_RINGSIZE -1) ) &&  (pHTTXContext->NextBulkIdx == 0 )))
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite USB_BULK_BUF_ALIGMENT c1!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}
	else if (pHTTXContext->bCurWriting == TRUE)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite USB_BULK_BUF_ALIGMENT c3!!\n"));

	}
#else

	if (((pHTTXContext->CurWritePosition) < pHTTXContext->NextBulkOutPosition) && (pHTTXContext->CurWritePosition + LOCAL_TXBUF_SIZE) > pHTTXContext->NextBulkOutPosition)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c1!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}
	else if ((pHTTXContext->CurWritePosition == 8) && (pHTTXContext->NextBulkOutPosition < LOCAL_TXBUF_SIZE))
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c2!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}
	else if (pHTTXContext->bCurWriting == TRUE)
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c3!\n"));
	}
	else if ((pHTTXContext->ENextBulkOutPosition == 8)  && ((pHTTXContext->CurWritePosition + 7912 ) > MAX_TXBULK_LIMIT)  )
	{
		DBGPRINT(RT_DEBUG_ERROR,("RtmpUSBCanDoWrite c4!\n"));
		RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));
	}

#endif /* USB_BULK_BUF_ALIGMENT */
	else
	{
		canWrite = NDIS_STATUS_SUCCESS;
	}


	return canWrite;
}


USHORT RtmpUSB_WriteSubTxResource(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk,
	IN BOOLEAN bIsLast,
	OUT	USHORT *freeCnt)
{

	/* Dummy function. Should be removed in the future.*/
	return 0;

}

USHORT	RtmpUSB_WriteFragTxResource(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk,
	IN UCHAR fragNum,
	OUT	USHORT *freeCnt)
{
#ifndef MT_MAC
	HT_TX_CONTEXT	*pHTTXContext;
	USHORT			hwHdrLen;	/* The hwHdrLen consist of 802.11 header length plus the header padding length.*/
	UINT32			fillOffset;
	TXINFO_STRUC	*pTxInfo;
	TXWI_STRUC		*pTxWI;
	PUCHAR			pWirelessPacket = NULL;
	UCHAR			QueIdx;
	NDIS_STATUS		Status;
	unsigned long	IrqFlags = 0;
	UINT32			USBDMApktLen = 0, DMAHdrLen, padding;
#ifdef USB_BULK_BUF_ALIGMENT
	BOOLEAN			bLasAlignmentsectiontRound = FALSE;
#else
	BOOLEAN			TxQLastRound = FALSE;
#endif /* USB_BULK_BUF_ALIGMENT */
	UINT8 TXWISize = pAd->chipCap.TXWISize;


	/* get Tx Ring Resource & Dma Buffer address*/

	QueIdx = pTxBlk->QueIdx;

#ifdef USE_BMC
	if (pTxBlk->QueIdx == QID_BMC) {
		QueIdx = EDCA_BMC_PIPE;
	}
#endif

	pHTTXContext  = &pAd->TxContext[QueIdx];

	RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

	pHTTXContext  = &pAd->TxContext[QueIdx];
	fillOffset = pHTTXContext->CurWritePosition;

	if(fragNum == 0)
	{
		/* Check if we have enough space for this bulk-out batch.*/
		Status = RtmpUSBCanDoWrite(pAd, QueIdx, pHTTXContext);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			pHTTXContext->bCurWriting = TRUE;

#ifndef USB_BULK_BUF_ALIGMENT
			/* Reserve space for 8 bytes padding.*/
			if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
			{
				pHTTXContext->ENextBulkOutPosition += 8;
				pHTTXContext->CurWritePosition += 8;
				fillOffset += 8;
			}
#endif /* USB_BULK_BUF_ALIGMENT */
			pTxBlk->Priv = 0;
			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;
		}
		else
		{
			RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

			RELEASE_NDIS_PACKET(pAd, pTxBlk->pPacket, NDIS_STATUS_FAILURE);
			return(Status);
		}
	}
	else
	{
		/* For sub-sequent frames of this bulk-out batch. Just copy it to our bulk-out buffer.*/
		Status = ((pHTTXContext->bCurWriting == TRUE) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			fillOffset += pTxBlk->Priv;
		}
		else
		{
			RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

			RELEASE_NDIS_PACKET(pAd, pTxBlk->pPacket, NDIS_STATUS_FAILURE);
			return(Status);
		}
	}

	NdisZeroMemory((PUCHAR)(&pTxBlk->HeaderBuf[0]), TXINFO_SIZE);
	pTxInfo = (TXINFO_STRUC *)(&pTxBlk->HeaderBuf[0]);
	pTxWI= (TXWI_STRUC *)(&pTxBlk->HeaderBuf[TXINFO_SIZE]);

#ifdef USB_BULK_BUF_ALIGMENT
	INT idx=0;
	idx = pHTTXContext->CurWriteIdx;
	pWirelessPacket = &pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset];
#else
	pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];
#endif /* USB_BULK_BUF_ALIGMENT */


	/* copy TXWI + WLAN Header + LLC into DMA Header Buffer*/
	/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);*/
	hwHdrLen = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;

	/* Build our URB for USBD*/
	DMAHdrLen = TXWISize + hwHdrLen;
	USBDMApktLen = DMAHdrLen + pTxBlk->SrcBufLen;
	padding = (4 - (USBDMApktLen % 4)) & 0x03;	/* round up to 4 byte alignment*/
	USBDMApktLen += padding;

	pTxBlk->Priv += (TXINFO_SIZE + USBDMApktLen);

	/* For TxInfo, the length of USBDMApktLen = TXWI_SIZE + 802.11 header + payload*/
	rlt_usb_write_txinfo(pAd, pTxInfo, (USHORT)(USBDMApktLen), FALSE, FIFO_EDCA, FALSE /*NextValid*/,  FALSE);

	if (fragNum == pTxBlk->TotalFragNum)
	{
		pTxInfo->TxInfoUDMATxburst = 0;

#ifndef USB_BULK_BUF_ALIGMENT
		/*
			when CurWritePosition > 0x6000  mean that it is at the max bulk out  size,
			we CurWriteIdx must move to the next alignment section.
			Otherwirse,  CurWriteIdx will be moved to the next section at databulkout.


			(((pHTTXContext->CurWritePosition + 3906)& 0x00007fff) & 0xffff6000) == 0x00006000)
			we must make sure that the last fragNun packet just over the 0x6000
			otherwise it will error because the last frag packet will at the section but will not bulk out.
			ex:   when secoend packet writeresouce and it > 0x6000
				And the last packet writesource and it also > 0x6000  at this time CurWriteIdx++
				but when data bulk out , because at second packet it will > 0x6000 , the last packet will not bulk out.

		*/


		if ((pHTTXContext->CurWritePosition + pTxBlk->Priv + 3906)> MAX_TXBULK_LIMIT)
		{
			pTxInfo->TxInfoSwLstRnd = 1;
			TxQLastRound = TRUE;
		}
#endif /* USB_BULK_BUF_ALIGMENT */
	}
	else
	{
		pTxInfo->TxInfoUDMATxburst = 1;
	}

	NdisMoveMemory(pWirelessPacket, pTxBlk->HeaderBuf, TXINFO_SIZE + TXWISize + hwHdrLen);
#ifdef RT_BIG_ENDIAN
	RTMPFrameEndianChange(pAd, (PUCHAR)(pWirelessPacket + TXINFO_SIZE + TXWISize), DIR_WRITE, FALSE);
#endif /* RT_BIG_ENDIAN */
	pWirelessPacket += (TXINFO_SIZE + TXWISize + hwHdrLen);
	pHTTXContext->CurWriteRealPos += (TXINFO_SIZE + TXWISize + hwHdrLen);

	RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

	NdisMoveMemory(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);

	/*	Zero the last padding.*/
	pWirelessPacket += pTxBlk->SrcBufLen;
	NdisZeroMemory(pWirelessPacket, padding + 8);

	if (fragNum == pTxBlk->TotalFragNum)
	{
		RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

		/* Update the pHTTXContext->CurWritePosition. 3906 used to prevent the NextBulkOut is a A-RALINK/A-MSDU Frame.*/
		pHTTXContext->CurWritePosition += pTxBlk->Priv;
#ifndef USB_BULK_BUF_ALIGMENT
		if (TxQLastRound == TRUE)
			pHTTXContext->CurWritePosition = 8;
#endif /* USB_BULK_BUF_ALIGMENT */
#ifdef USB_BULK_BUF_ALIGMENT
		if(bLasAlignmentsectiontRound == TRUE)
		{
			CUR_WRITE_IDX_INC(pHTTXContext->CurWriteIdx, BUF_ALIGMENT_RINGSIZE);
			pHTTXContext->CurWritePosition = 0;
		}
#endif /* USB_BULK_BUF_ALIGMENT */

		pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

#ifdef UAPSD_SUPPORT
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
		UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#else
#ifdef CONFIG_AP_SUPPORT
#ifdef P2P_SUPPORT
		if (P2P_GO_ON(pAd))
#else
#ifdef RT_CFG80211_P2P_SUPPORT
        if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
#else
        IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
#endif /* P2P_SUPPORT */
		{
#ifdef USB_BULK_BUF_ALIGMENT
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWriteIdx);
#else
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#endif /* USB_BULK_BUF_ALIGMENT */
		}
#endif /* CONFIG_AP_SUPPORT */
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
#endif /* UAPSD_SUPPORT */

		/* Finally, set bCurWriting as FALSE*/
	pHTTXContext->bCurWriting = FALSE;

		RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

		/* succeed and release the skb buffer*/
		RELEASE_NDIS_PACKET(pAd, pTxBlk->pPacket, NDIS_STATUS_SUCCESS);
	}


	return(Status);
#else
	return 0;
#endif
}


USHORT RtmpUSB_WriteSingleTxResource(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk,
	IN BOOLEAN bIsLast,
	OUT	USHORT *freeCnt)
{
	HT_TX_CONTEXT *pHTTXContext;
	UINT32 fillOffset;
	TXINFO_STRUC *pTxInfo;
	//TXWI_STRUC *pTxWI;
	UCHAR *pWirelessPacket, *buf;
	UCHAR QueIdx;
	unsigned long	IrqFlags = 0;
	NDIS_STATUS Status;
	UINT32 hdr_copy_len, hdr_len, dma_len = 0, padding;
#ifndef USB_BULK_BUF_ALIGMENT
	BOOLEAN bTxQLastRound = FALSE;
#endif /* USB_BULK_BUF_ALIGMENT */
#if !defined(MT_MAC) || defined(CONFIG_TSO_SUPPORT) ||defined(RT_BIG_ENDIAN)
	UINT8 TXWISize = pAd->chipCap.TXWISize;
#endif
#ifdef CONFIG_TSO_SUPPORT
	TSO_INFO *tso_info;
	UCHAR *usb_frame_start, *usb_frame_end;
#endif /* CONFIG_TSO_SUPPORT */


	/* get Tx Ring Resource & Dma Buffer address*/
	QueIdx = pTxBlk->QueIdx;
#ifdef USE_BMC
    if (pTxBlk->QueIdx == QID_BMC) {
        QueIdx = EDCA_BMC_PIPE;
    }
#endif

#ifdef MULTI_WMM_SUPPORT
	if (pTxBlk->QueIdx >= EDCA_WMM1_AC0_PIPE)
	{
#ifdef USB_BULK_BUF_ALIGMENT
	INT idx=0;
#endif	
		QueIdx -= EDCA_WMM1_AC0_PIPE;
		RTMP_IRQ_LOCK(&pAd->TxContextQueueWmm1Lock[QueIdx], IrqFlags);
		pHTTXContext  = &pAd->TxContextWmm1[QueIdx];
		fillOffset = pHTTXContext->CurWritePosition;


#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_LOUD,("WS-B:I=0x%lx, CWPos=%ld, NBPos=%ld, ENBPos=%ld\n",
				in_interrupt(), pHTTXContext->CurWritePosition,
				pHTTXContext->NextBulkOutPosition,
				pHTTXContext->ENextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */

	/* Check ring full */
		Status = RtmpUSBWmm1CanDoWrite(pAd, QueIdx, pHTTXContext);
		if(Status == NDIS_STATUS_SUCCESS)
		{
			pHTTXContext->bCurWriting = TRUE;
			buf = &pTxBlk->HeaderBuf[0];
			pTxInfo = (TXINFO_STRUC *)buf;
			//pTxWI= (TXWI_STRUC *)&buf[TXINFO_SIZE];
#ifdef CONFIG_TSO_SUPPORT
			tso_info = (TSO_INFO *)&buf[TXINFO_SIZE + TXWISize];
#endif /* CONFIG_TSO_SUPPORT */

#ifndef USB_BULK_BUF_ALIGMENT
		/* Reserve space for 8 bytes padding.*/
			if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
			{
				pHTTXContext->ENextBulkOutPosition += 8;
				pHTTXContext->CurWritePosition += 8;
				fillOffset += 8;
			}
#endif /* USB_BULK_BUF_ALIGMENT */
			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

#ifdef USB_BULK_BUF_ALIGMENT
		idx = pHTTXContext->CurWriteIdx;
		pWirelessPacket = &pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset];
#else
			pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];
#endif /* USB_BULK_BUF_ALIGMENT */


		/* Build our URB for USBD */
#ifdef MT_MAC
			hdr_len = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
			if (pTxBlk->hw_rsv_len == 0) // Long
			{
				hdr_copy_len = sizeof(TMAC_TXD_L) + hdr_len;
			}
			else //Short
			{
				hdr_copy_len = sizeof(TMAC_TXD_S) + hdr_len;
				pTxBlk->HeaderBuf += pTxBlk->hw_rsv_len;
			}
   	     dma_len = hdr_copy_len + pTxBlk->SrcBufLen;
#else
			hdr_len = TXWISize + TSO_SIZE + pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
			hdr_copy_len = TXINFO_SIZE + hdr_len;
			dma_len = hdr_len + pTxBlk->SrcBufLen;
#endif /* MT_MAC */
			padding = (4 - (dma_len % 4)) & 0x03;	/* round up to 4 byte alignment*/
			dma_len += padding;

#ifdef MT_MAC
			pTxBlk->Priv = (dma_len);
#else
			pTxBlk->Priv = (TXINFO_SIZE + dma_len);
#endif /* MT_MAC */

		/* For TxInfo, the length of USBDMApktLen = TXWI_SIZE + TSO_SIZE + 802.11 header + payload */
			rlt_usb_write_txinfo(pAd, pTxInfo, (USHORT)(dma_len), FALSE, FIFO_EDCA, FALSE /*NextValid*/,  FALSE);

#ifdef CONFIG_TSO_SUPPORT
			rlt_tso_info_write(pAd, (TSO_INFO *)(&pTxBlk->HeaderBuf[TXINFO_SIZE + TXWISize]), pTxBlk, hdr_len);
			rlt_usb_update_txinfo(pAd, pTxInfo, pTxBlk);
			usb_frame_start = pWirelessPacket;
#endif /* CONFIG_TSO_SUPPORT */

#ifndef USB_BULK_BUF_ALIGMENT
			if ((pHTTXContext->CurWritePosition + 3906 + pTxBlk->Priv) > MAX_TXBULK_LIMIT)
			{
#ifdef MT_MAC
   	         TMAC_TXD_L *txd_l = (TMAC_TXD_L *)pTxBlk->HeaderBuf;
				TMAC_TXD_S *txd_s = (TMAC_TXD_S *)pTxBlk->HeaderBuf;
				TMAC_TXD_1 *txd_1 = &txd_s->txd_1;
				TMAC_TXD_7 *txd_7;

				if (txd_1->ft == TMI_FT_SHORT)
		            txd_7 = &txd_s->txd_7;
  		  	    else
   		     	    txd_7 = &txd_l->txd_7;

				txd_7->sw_field = 1;
#else
				pTxInfo->TxInfoSwLstRnd = 1;
#endif	/* MT_MAC */

				bTxQLastRound = TRUE;
			}
#endif /* USB_BULK_BUF_ALIGMENT */

			NdisMoveMemory(pWirelessPacket, pTxBlk->HeaderBuf, hdr_copy_len);
#ifdef RT_BIG_ENDIAN
			RTMPFrameEndianChange(pAd, (PUCHAR)(pWirelessPacket + TXINFO_SIZE + TXWISize + TSO_SIZE), DIR_WRITE, FALSE);
#endif /* RT_BIG_ENDIAN */
			pWirelessPacket += (hdr_copy_len);

		/* We unlock it here to prevent the first 8 bytes maybe over-writed issue.*/
		/*	1. First we got CurWritePosition but the first 8 bytes still not write to the pTxcontext.*/
		/*	2. An interrupt break our routine and handle bulk-out complete.*/
		/*	3. In the bulk-out compllete, it need to do another bulk-out, */
		/*			if the ENextBulkOutPosition is just the same as CurWritePosition, it will save the first 8 bytes from CurWritePosition,*/
		/*			but the payload still not copyed. the pTxContext->SavedPad[] will save as allzero. and set the bCopyPad = TRUE.*/
		/*	4. Interrupt complete.*/
		/*  5. Our interrupted routine go back and fill the first 8 bytes to pTxContext.*/
		/*	6. Next time when do bulk-out, it found the bCopyPad==TRUE and will copy the SavedPad[] to pTxContext->NextBulkOutPosition.*/
		/*		and the packet will wrong.*/
			pHTTXContext->CurWriteRealPos += hdr_copy_len;
#ifndef USB_BULK_BUF_ALIGMENT
			RTMP_IRQ_UNLOCK(&pAd->TxContextQueueWmm1Lock[QueIdx], IrqFlags);
#endif /* USB_BULK_BUF_ALIGMENT */

#ifdef TX_PKT_SG
		if (pTxBlk->pkt_info.BufferCount > 1) {
			INT i, len;
			void *data;
			PKT_SG_T *sg = &pTxBlk->pkt_info.sg_list[0];

			for (i = 0 ; i < pTxBlk->pkt_info.BufferCount; i++) {
				data = sg[i].data;
				len = sg[i].len;
				if (i == 0) {
					len -= ((ULONG)pTxBlk->pSrcBufData - (ULONG)sg[i].data);
					data = pTxBlk->pSrcBufData;
				}
				//DBGPRINT(RT_DEBUG_TRACE, ("%s:sg[%d]=0x%x, len=%d\n", __FUNCTION__, i, data, len));
				if (len <= 0) {
					DBGPRINT(RT_DEBUG_ERROR, ("%s():sg[%d] info error, sg.data=0x%x, sg.len=%d, pTxBlk->pSrcBufData=0x%x, pTxBlk->SrcBufLen=%d, data=0x%x, len=%d\n",
								__FUNCTION__, i, sg[i].data, sg[i].len, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, data, len));
					break;
				}
				NdisMoveMemory(pWirelessPacket, data, len);
				pWirelessPacket += len;
			}
		}
		else
#endif /* TX_PKT_SG */
			{
				NdisMoveMemory(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);
				pWirelessPacket += pTxBlk->SrcBufLen;
			}

#ifndef USB_BULK_BUF_ALIGMENT
			NdisZeroMemory(pWirelessPacket, padding + 8);
			RTMP_IRQ_LOCK(&pAd->TxContextQueueWmm1Lock[QueIdx], IrqFlags);
#endif /* USB_BULK_BUF_ALIGMENT */

#ifdef CONFIG_TSO_SUPPORT
			usb_frame_end = pWirelessPacket;
			//DBGPRINT(RT_DEBUG_TRACE, ("usb_frame_start=0x%x, usb_frame_end=0x%x\n", usb_frame_start, usb_frame_end));
			if ((pTxBlk->naf_type == NAF_TYPE_TSO) && ((usb_frame_end - usb_frame_start) > 0))
			{
				int dump_len;
				TXINFO_STRUC *info = (TXINFO_STRUC *)(usb_frame_start);
				TXWI_STRUC *wi = (TXWI_STRUC *)(usb_frame_start + sizeof(TXINFO_STRUC));
				TSO_INFO *soinfo = (TSO_INFO *)(usb_frame_start + sizeof(TXINFO_STRUC) + pAd->chipCap.TXWISize);
				UCHAR *hdr = (UCHAR *)(usb_frame_start + sizeof(TXINFO_STRUC) + pAd->chipCap.TXWISize + sizeof(TSO_INFO));

				dump_len = usb_frame_end - usb_frame_start;
				hex_dump("raw data of TXWI_TSO_WiFiPkt", usb_frame_start, dump_len > 100 ? 100: dump_len);
				dump_txinfo(pAd, info);
				dump_tmac_info(pAd, (UCHAR *)wi);
				dump_tsoinfo(soinfo);
				hex_dump("802.11 hdr", hdr, sizeof(HEADER_802_11));
			}
#endif /* CONFIG_TSO_SUPPORT */
			pHTTXContext->CurWritePosition += pTxBlk->Priv;
#ifdef UAPSD_SUPPORT
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
		UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#else
#ifdef CONFIG_AP_SUPPORT
#ifdef P2P_SUPPORT
		if (P2P_GO_ON(pAd))
#else
#ifdef RT_CFG80211_P2P_SUPPORT
        if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
#else
        IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
#endif /* P2P_SUPPORT */
		{
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
		}
#endif /* CONFIG_AP_SUPPORT */
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
#endif /* UAPSD_SUPPORT */

#ifdef USB_BULK_BUF_ALIGMENT
		/*
			when CurWritePosition > 0x6000  mean that it is at the max bulk out size,
			we CurWriteIdx must move to the next alignment section.
			Otherwirse,  CurWriteIdx will be moved to the next section at databulkout.

			Writingflag = TRUE ,mean that when we writing resource ,and databulkout happen,
			So we bulk out when this packet finish.
		*/
			if ( (pHTTXContext->CurWritePosition  & 0x00006000) == 0x00006000)
			{
				pHTTXContext->CurWritePosition = ((CUR_WRITE_IDX_INC(pHTTXContext->CurWriteIdx, BUF_ALIGMENT_RINGSIZE)) * 0x8000);
			}
#else
			if (bTxQLastRound)
				pHTTXContext->CurWritePosition = 8;
#endif /* USB_BULK_BUF_ALIGMENT */

			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;
			pHTTXContext->bCurWriting = FALSE;
		}


		RTMP_IRQ_UNLOCK(&pAd->TxContextQueueWmm1Lock[QueIdx], IrqFlags);
	}
	else
#endif /* MULTI_WMM_SUPPORT */
	{
#ifdef USB_BULK_BUF_ALIGMENT
	INT idx=0;
#endif
		if (QueIdx >= NUM_OF_TX_RING) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Invalid QueIdx (%d)\n"
					, __func__, QueIdx));
			return NDIS_STATUS_FAILURE;
		}
		RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);
		pHTTXContext  = &pAd->TxContext[QueIdx];
		fillOffset = pHTTXContext->CurWritePosition;


#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_LOUD,("WS-B:I=0x%lx, CWPos=%ld, NBPos=%ld, ENBPos=%ld\n",
				in_interrupt(), pHTTXContext->CurWritePosition,
				pHTTXContext->NextBulkOutPosition,
				pHTTXContext->ENextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */

		/* Check ring full */
		Status = RtmpUSBCanDoWrite(pAd, QueIdx, pHTTXContext);
		if(Status == NDIS_STATUS_SUCCESS)
		{
			pHTTXContext->bCurWriting = TRUE;
			buf = &pTxBlk->HeaderBuf[0];
			pTxInfo = (TXINFO_STRUC *)buf;
			//pTxWI= (TXWI_STRUC *)&buf[TXINFO_SIZE];
#ifdef CONFIG_TSO_SUPPORT
			tso_info = (TSO_INFO *)&buf[TXINFO_SIZE + TXWISize];
#endif /* CONFIG_TSO_SUPPORT */

#ifndef USB_BULK_BUF_ALIGMENT
			/* Reserve space for 8 bytes padding.*/
			if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
			{
				pHTTXContext->ENextBulkOutPosition += 8;
				pHTTXContext->CurWritePosition += 8;
				fillOffset += 8;
			}
#endif /* USB_BULK_BUF_ALIGMENT */
			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

#ifdef USB_BULK_BUF_ALIGMENT
			idx = pHTTXContext->CurWriteIdx;
			pWirelessPacket = &pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset];
#else
			pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];
#endif /* USB_BULK_BUF_ALIGMENT */


		/* Build our URB for USBD */
#ifdef MT_MAC
			hdr_len = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
			if (pTxBlk->hw_rsv_len == 0) // Long
			{
				hdr_copy_len = sizeof(TMAC_TXD_L) + hdr_len;
			}
			else //Short
			{
				hdr_copy_len = sizeof(TMAC_TXD_S) + hdr_len;
				pTxBlk->HeaderBuf += pTxBlk->hw_rsv_len;
			}
        	dma_len = hdr_copy_len + pTxBlk->SrcBufLen;
#else
			hdr_len = TXWISize + TSO_SIZE + pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
			hdr_copy_len = TXINFO_SIZE + hdr_len;
			dma_len = hdr_len + pTxBlk->SrcBufLen;
#endif /* MT_MAC */
			padding = (4 - (dma_len % 4)) & 0x03;	/* round up to 4 byte alignment*/
			dma_len += padding;

#ifdef MT_MAC
			pTxBlk->Priv = (dma_len);
#else
			pTxBlk->Priv = (TXINFO_SIZE + dma_len);
#endif /* MT_MAC */

			/* For TxInfo, the length of USBDMApktLen = TXWI_SIZE + TSO_SIZE + 802.11 header + payload */
			rlt_usb_write_txinfo(pAd, pTxInfo, (USHORT)(dma_len), FALSE, FIFO_EDCA, FALSE /*NextValid*/,  FALSE);

#ifdef CONFIG_TSO_SUPPORT
			rlt_tso_info_write(pAd, (TSO_INFO *)(&pTxBlk->HeaderBuf[TXINFO_SIZE + TXWISize]), pTxBlk, hdr_len);
			rlt_usb_update_txinfo(pAd, pTxInfo, pTxBlk);
			usb_frame_start = pWirelessPacket;
#endif /* CONFIG_TSO_SUPPORT */

#ifndef USB_BULK_BUF_ALIGMENT
			if ((pHTTXContext->CurWritePosition + 3906 + pTxBlk->Priv) > MAX_TXBULK_LIMIT)
			{
#ifdef MT_MAC
        	    TMAC_TXD_L *txd_l = (TMAC_TXD_L *)pTxBlk->HeaderBuf;
				TMAC_TXD_S *txd_s = (TMAC_TXD_S *)pTxBlk->HeaderBuf;
				TMAC_TXD_1 *txd_1 = &txd_s->txd_1;
				TMAC_TXD_7 *txd_7;

				if (txd_1->ft == TMI_FT_SHORT)
	        	    txd_7 = &txd_s->txd_7;
	    	    else
    	    	    txd_7 = &txd_l->txd_7;

				txd_7->sw_field = 1;
#else
				pTxInfo->TxInfoSwLstRnd = 1;
#endif	/* MT_MAC */

				bTxQLastRound = TRUE;
			}
#endif /* USB_BULK_BUF_ALIGMENT */

			NdisMoveMemory(pWirelessPacket, pTxBlk->HeaderBuf, hdr_copy_len);
#ifdef RT_BIG_ENDIAN
			RTMPFrameEndianChange(pAd, (PUCHAR)(pWirelessPacket + TXINFO_SIZE + TXWISize + TSO_SIZE), DIR_WRITE, FALSE);
#endif /* RT_BIG_ENDIAN */
			pWirelessPacket += (hdr_copy_len);

			/* We unlock it here to prevent the first 8 bytes maybe over-writed issue.*/
			/*	1. First we got CurWritePosition but the first 8 bytes still not write to the pTxcontext.*/
			/*	2. An interrupt break our routine and handle bulk-out complete.*/
			/*	3. In the bulk-out compllete, it need to do another bulk-out, */
			/*			if the ENextBulkOutPosition is just the same as CurWritePosition, it will save the first 8 bytes from CurWritePosition,*/
			/*			but the payload still not copyed. the pTxContext->SavedPad[] will save as allzero. and set the bCopyPad = TRUE.*/
			/*	4. Interrupt complete.*/
			/*  5. Our interrupted routine go back and fill the first 8 bytes to pTxContext.*/
			/*	6. Next time when do bulk-out, it found the bCopyPad==TRUE and will copy the SavedPad[] to pTxContext->NextBulkOutPosition.*/
			/*		and the packet will wrong.*/
			pHTTXContext->CurWriteRealPos += hdr_copy_len;
#ifndef USB_BULK_BUF_ALIGMENT
			RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);
#endif /* USB_BULK_BUF_ALIGMENT */

#ifdef TX_PKT_SG
			if (pTxBlk->pkt_info.BufferCount > 1) {
				INT i, len;
				void *data;
				PKT_SG_T *sg = &pTxBlk->pkt_info.sg_list[0];

				for (i = 0 ; i < pTxBlk->pkt_info.BufferCount; i++) {
					data = sg[i].data;
					len = sg[i].len;
					if (i == 0) {
						len -= ((ULONG)pTxBlk->pSrcBufData - (ULONG)sg[i].data);
						data = pTxBlk->pSrcBufData;
					}
					//DBGPRINT(RT_DEBUG_TRACE, ("%s:sg[%d]=0x%x, len=%d\n", __FUNCTION__, i, data, len));
					if (len <= 0) {
						DBGPRINT(RT_DEBUG_ERROR, ("%s():sg[%d] info error, sg.data=0x%x, sg.len=%d, pTxBlk->pSrcBufData=0x%x, pTxBlk->SrcBufLen=%d, data=0x%x, len=%d\n",
								__FUNCTION__, i, sg[i].data, sg[i].len, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, data, len));
						break;
					}
					NdisMoveMemory(pWirelessPacket, data, len);
					pWirelessPacket += len;
				}
			}
			else
#endif /* TX_PKT_SG */
			{
				NdisMoveMemory(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);
				pWirelessPacket += pTxBlk->SrcBufLen;
			}

#ifndef USB_BULK_BUF_ALIGMENT
			NdisZeroMemory(pWirelessPacket, padding + 8);
			RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);
#endif /* USB_BULK_BUF_ALIGMENT */

#ifdef CONFIG_TSO_SUPPORT
			usb_frame_end = pWirelessPacket;
			//DBGPRINT(RT_DEBUG_TRACE, ("usb_frame_start=0x%x, usb_frame_end=0x%x\n", usb_frame_start, usb_frame_end));
			if ((pTxBlk->naf_type == NAF_TYPE_TSO) && ((usb_frame_end - usb_frame_start) > 0))
			{
				int dump_len;
				TXINFO_STRUC *info = (TXINFO_STRUC *)(usb_frame_start);
				TXWI_STRUC *wi = (TXWI_STRUC *)(usb_frame_start + sizeof(TXINFO_STRUC));
				TSO_INFO *soinfo = (TSO_INFO *)(usb_frame_start + sizeof(TXINFO_STRUC) + pAd->chipCap.TXWISize);
				UCHAR *hdr = (UCHAR *)(usb_frame_start + sizeof(TXINFO_STRUC) + pAd->chipCap.TXWISize + sizeof(TSO_INFO));

				dump_len = usb_frame_end - usb_frame_start;
				hex_dump("raw data of TXWI_TSO_WiFiPkt", usb_frame_start, dump_len > 100 ? 100: dump_len);
				dump_txinfo(pAd, info);
				dump_tmac_info(pAd, (UCHAR *)wi);
				dump_tsoinfo(soinfo);
				hex_dump("802.11 hdr", hdr, sizeof(HEADER_802_11));
			}
#endif /* CONFIG_TSO_SUPPORT */
			pHTTXContext->CurWritePosition += pTxBlk->Priv;
#ifdef UAPSD_SUPPORT
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
#ifdef USB_BULK_BUF_ALIGMENT
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWriteIdx);
#else
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#endif /* USB_BULK_BUF_ALIGMENT */
#else
#ifdef CONFIG_AP_SUPPORT
#ifdef P2P_SUPPORT
			if (P2P_GO_ON(pAd))
#else

#ifdef RT_CFG80211_P2P_SUPPORT
            if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
#else
            IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
#endif /* P2P_SUPPORT */
			{
#ifdef USB_BULK_BUF_ALIGMENT
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWriteIdx);
#else
				UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#endif /* USB_BULK_BUF_ALIGMENT */
			}
#endif /* CONFIG_AP_SUPPORT */
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
#endif /* UAPSD_SUPPORT */
#ifdef USB_BULK_BUF_ALIGMENT
			/*
				when CurWritePosition > 0x6000  mean that it is at the max bulk out size,
				we CurWriteIdx must move to the next alignment section.
				Otherwirse,  CurWriteIdx will be moved to the next section at databulkout.

				Writingflag = TRUE ,mean that when we writing resource ,and databulkout happen,
				So we bulk out when this packet finish.
			*/
		if ( (pHTTXContext->CurWritePosition  & BULKOUT_SIZE) == BULKOUT_SIZE)
			{
			CUR_WRITE_IDX_INC(pHTTXContext->CurWriteIdx, BUF_ALIGMENT_RINGSIZE);
			pHTTXContext->CurWritePosition = 0;
			//printk("%d",BULKOUT_SIZE);
			}
#else
			if (bTxQLastRound)
				pHTTXContext->CurWritePosition = 8;
#endif /* USB_BULK_BUF_ALIGMENT */

			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;
			pHTTXContext->bCurWriting = FALSE;
		}


		RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);
	}

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_LOUD,("WS-A:Status=%d, Len=%d, CWPos=%ld, NBPos=%ld, ENBPos=%ld\n",
				Status, TXINFO_SIZE + dma_len, pHTTXContext->CurWritePosition,
				pHTTXContext->NextBulkOutPosition, pHTTXContext->ENextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */

	/* succeed and release the skb buffer*/
	RELEASE_NDIS_PACKET(pAd, pTxBlk->pPacket, NDIS_STATUS_SUCCESS);

	return(Status);
}


USHORT RtmpUSB_WriteMultiTxResource(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk,
	IN UCHAR frmNum,
	OUT USHORT *freeCnt)
{
	HT_TX_CONTEXT *pHTTXContext;
#ifdef MT_MAC
    UINT32 hdr_copy_len = 0;
#else
	USHORT hwHdrLen;	/* The hwHdrLen consist of 802.11 header length plus the header padding length.*/
	TXINFO_STRUC *pTxInfo;
	TXWI_STRUC *pTxWI;
	UINT8 TXWISize = pAd->chipCap.TXWISize;
#endif
	UINT32 fillOffset;
	UCHAR *pWirelessPacket = NULL;
	UCHAR QueIdx;
	NDIS_STATUS Status;
	unsigned long IrqFlags = 0;
#ifdef USB_BULK_BUF_ALIGMENT
	INT idx=0;
#endif

	/* get Tx Ring Resource & Dma Buffer address*/
	QueIdx = pTxBlk->QueIdx;

#ifdef USE_BMC
	if (pTxBlk->QueIdx == QID_BMC) {
		QueIdx = EDCA_BMC_PIPE;
	}
#endif

	pHTTXContext  = &pAd->TxContext[QueIdx];

	RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

	if(frmNum == 0)
	{
		/* Check if we have enough space for this bulk-out batch.*/
		Status = RtmpUSBCanDoWrite(pAd, QueIdx, pHTTXContext);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			pHTTXContext->bCurWriting = TRUE;

#ifndef MT_MAC
			pTxInfo = (TXINFO_STRUC *)(&pTxBlk->HeaderBuf[0]);
			pTxWI= (TXWI_STRUC *)(&pTxBlk->HeaderBuf[TXINFO_SIZE]);
#endif

#ifndef USB_BULK_BUF_ALIGMENT
			/* Reserve space for 8 bytes padding.*/
			if ((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition))
			{
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_LOUD, ("Colision, CWPos=%ld, ENBPos=%ld!\n", pHTTXContext->CurWritePosition, pHTTXContext->ENextBulkOutPosition));
#endif /* RELEASE_EXCLUDE */

				pHTTXContext->CurWritePosition += 8;
				pHTTXContext->ENextBulkOutPosition += 8;
			}
#endif /* USB_BULK_BUF_ALIGMENT */
			fillOffset = pHTTXContext->CurWritePosition;
			pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

#ifdef USB_BULK_BUF_ALIGMENT
			idx = pHTTXContext->CurWriteIdx;
			pWirelessPacket = &pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset];
#else
			pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];
#endif /* USB_BULK_BUF_ALIGMENT */


#ifdef MT_MAC
            if (pTxBlk->hw_rsv_len == 0) // Long
            {
                hdr_copy_len = sizeof(TMAC_TXD_L) + pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
            }
            else // short
            {
                hdr_copy_len = sizeof(TMAC_TXD_S) + pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;
                /* move pointer to short format start address */
                pTxBlk->HeaderBuf += pTxBlk->hw_rsv_len;
            }

            /* Copy TxD + 802.11 + 802.3 header(DA|SA|LLC|Len) */
            pTxBlk->Priv = hdr_copy_len;
#else
			/* Copy TXINFO + TXWI + WLAN Header + LLC into DMA Header Buffer*/

			if (pTxBlk->TxFrameType == TX_AMSDU_FRAME)
				/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen-AMSDU_SUBHEAD_LEN, 4)+AMSDU_SUBHEAD_LEN;*/
				hwHdrLen = pTxBlk->MpduHeaderLen-AMSDU_SUBHEAD_LEN + pTxBlk->HdrPadLen + AMSDU_SUBHEAD_LEN;
			else if (pTxBlk->TxFrameType == TX_RALINK_FRAME)
				/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen-ARALINK_HEADER_LEN, 4)+ARALINK_HEADER_LEN;*/
				hwHdrLen = pTxBlk->MpduHeaderLen-ARALINK_HEADER_LEN + pTxBlk->HdrPadLen + ARALINK_HEADER_LEN;
			else
				/*hwHdrLen = ROUND_UP(pTxBlk->MpduHeaderLen, 4);*/
				hwHdrLen = pTxBlk->MpduHeaderLen + pTxBlk->HdrPadLen;

			/* Update the pTxBlk->Priv.*/
			pTxBlk->Priv = TXINFO_SIZE + TXWISize + hwHdrLen;

			/*	pTxInfo->USBDMApktLen now just a temp value and will to correct latter.*/
			rlt_usb_write_txinfo(pAd, pTxInfo, (USHORT)(pTxBlk->Priv), FALSE, FIFO_EDCA, FALSE /*NextValid*/,  FALSE);
#endif

			NdisMoveMemory(pWirelessPacket, pTxBlk->HeaderBuf, pTxBlk->Priv);
#ifdef RT_BIG_ENDIAN
			RTMPFrameEndianChange(pAd, (PUCHAR)(pWirelessPacket+ TXINFO_SIZE + TXWISize), DIR_WRITE, FALSE);
#endif /* RT_BIG_ENDIAN */
			pHTTXContext->CurWriteRealPos += pTxBlk->Priv;
			pWirelessPacket += pTxBlk->Priv;
		}
	}
	else
	{	/* For sub-sequent frames of this bulk-out batch. Just copy it to our bulk-out buffer.*/

		Status = ((pHTTXContext->bCurWriting == TRUE) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			fillOffset =  (pHTTXContext->CurWritePosition + pTxBlk->Priv);
#ifdef USB_BULK_BUF_ALIGMENT
			idx = pHTTXContext->CurWriteIdx;
			pWirelessPacket = &pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset];
#else
			pWirelessPacket = &pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset];
#endif /* USB_BULK_BUF_ALIGMENT */

			/*hwHdrLen = pTxBlk->MpduHeaderLen;*/
			NdisMoveMemory(pWirelessPacket, pTxBlk->HeaderBuf, pTxBlk->MpduHeaderLen);
			pWirelessPacket += (pTxBlk->MpduHeaderLen);
			pTxBlk->Priv += pTxBlk->MpduHeaderLen;
		}
		else
		{	/* It should not happened now unless we are going to shutdown.*/
			DBGPRINT(RT_DEBUG_ERROR, ("WriteMultiTxResource():bCurWriting is FALSE when handle sub-sequent frames.\n"));
			Status = NDIS_STATUS_FAILURE;
		}
	}


	/*
		We unlock it here to prevent the first 8 bytes maybe over-write issue.
		1. First we got CurWritePosition but the first 8 bytes still not write to the pTxContext.
		2. An interrupt break our routine and handle bulk-out complete.
		3. In the bulk-out compllete, it need to do another bulk-out,
			if the ENextBulkOutPosition is just the same as CurWritePosition, it will save the first 8 bytes from CurWritePosition,
			but the payload still not copyed. the pTxContext->SavedPad[] will save as allzero. and set the bCopyPad = TRUE.
		4. Interrupt complete.
		5. Our interrupted routine go back and fill the first 8 bytes to pTxContext.
		6. Next time when do bulk-out, it found the bCopyPad==TRUE and will copy the SavedPad[] to pTxContext->NextBulkOutPosition.
			and the packet will wrong.
	*/
	RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR,("WriteMultiTxResource: CWPos = %ld, NBOutPos = %ld.\n", pHTTXContext->CurWritePosition, pHTTXContext->NextBulkOutPosition));
		goto done;
	}

	/* Copy the frame content into DMA buffer and update the pTxBlk->Priv*/
	NdisMoveMemory(pWirelessPacket, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen);
	pWirelessPacket += pTxBlk->SrcBufLen;
	pTxBlk->Priv += pTxBlk->SrcBufLen;

done:
	/* Release the skb buffer here*/
	RELEASE_NDIS_PACKET(pAd, pTxBlk->pPacket, NDIS_STATUS_SUCCESS);

	return(Status);
}


VOID RtmpUSB_FinalWriteTxResource(
	IN RTMP_ADAPTER	*pAd,
	IN TX_BLK *pTxBlk,
	IN USHORT totalMPDUSize,
	IN USHORT TxIdx)
{
	UCHAR			QueIdx;
	HT_TX_CONTEXT	*pHTTXContext;
	UINT32			fillOffset;
#ifndef MT_MAC
	TXINFO_STRUC	*pTxInfo;
#endif
#if !defined(MT_MAC) || defined(RLT_MAC) || defined(RTMP_MAC)
	TXWI_STRUC		*pTxWI;
#endif
	UINT32			USBDMApktLen, padding;
	unsigned long	IrqFlags = 0;
	PUCHAR			pWirelessPacket;
#ifdef MT_MAC
    TMAC_TXD_S      *txd_s;
    TMAC_TXD_L      *txd_l;
#endif
#ifdef USB_BULK_BUF_ALIGMENT
	INT idx=0;
#endif /* USB_BULK_BUF_ALIGMENT */

	QueIdx = pTxBlk->QueIdx;
	pHTTXContext  = &pAd->TxContext[QueIdx];
#ifdef USB_BULK_BUF_ALIGMENT
	idx = pHTTXContext->CurWriteIdx;
#endif /* USB_BULK_BUF_ALIGMENT */


	RTMP_IRQ_LOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);

	if (pHTTXContext->bCurWriting == TRUE)
	{
		fillOffset = pHTTXContext->CurWritePosition;
#ifndef USB_BULK_BUF_ALIGMENT
		if (((pHTTXContext->ENextBulkOutPosition == pHTTXContext->CurWritePosition) || ((pHTTXContext->ENextBulkOutPosition-8) == pHTTXContext->CurWritePosition))
			&& (pHTTXContext->bCopySavePad == TRUE))
			pWirelessPacket = (PUCHAR)(&pHTTXContext->SavedPad[0]);
		else
#endif /* USB_BULK_BUF_ALIGMENT */

#ifdef USB_BULK_BUF_ALIGMENT
			pWirelessPacket = (PUCHAR)(&pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset]);
#else
			pWirelessPacket = (PUCHAR)(&pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset]);
#endif /* USB_BULK_BUF_ALIGMENT */

		/* Update TxInfo->USBDMApktLen , */
		/*		the length = TXWI_SIZE + 802.11_hdr + 802.11_hdr_pad + payload_of_all_batch_frames + Bulk-Out-padding*/

#ifdef MT_MAC
        txd_s = (TMAC_TXD_S *)pWirelessPacket;
        txd_l = (TMAC_TXD_L *)pWirelessPacket;

		USBDMApktLen = pTxBlk->Priv;
#else
		pTxInfo = (TXINFO_STRUC *)(pWirelessPacket);

		/* Calculate the bulk-out padding*/
		USBDMApktLen = pTxBlk->Priv - TXINFO_SIZE;
#endif /* MT_MAC */
		padding = (4 - (USBDMApktLen % 4)) & 0x03;	/* round up to 4 byte alignment*/
		USBDMApktLen += padding;

#ifdef MT_MAC
        txd_s->txd_0.tx_byte_cnt = USBDMApktLen;
        pTxBlk->Priv += padding;
#else
		pTxInfo->TxInfoPktLen = USBDMApktLen;


		/*
			Update TXWI->TxWIMPDUByteCnt,
				the length = 802.11 header + payload_of_all_batch_frames
		*/
		pTxWI= (TXWI_STRUC *)(pWirelessPacket + TXINFO_SIZE);
#endif /* MT_MAC */
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
#ifdef MT7601
		if (IS_MT7601(pAd))
			pTxWI->TXWI_X.MPDUtotalByteCnt = totalMPDUSize;
		else
#endif /* MT7601 */
			pTxWI->TXWI_N.MPDUtotalByteCnt = totalMPDUSize;
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		pTxWI->TXWI_O.MPDUtotalByteCnt = totalMPDUSize;
#endif /* RTMP_MAC */

#ifndef MT_MAC
		/* Update the pHTTXContext->CurWritePosition*/

		pHTTXContext->CurWritePosition += (TXINFO_SIZE + USBDMApktLen);
#else
	    pHTTXContext->CurWritePosition += USBDMApktLen;
#endif
#ifdef USB_BULK_BUF_ALIGMENT
		/*
			when CurWritePosition > 0x6000  mean that it is at the max bulk out size,
			we CurWriteIdx must move to the next alignment section.
			Otherwirse,  CurWriteIdx will be moved to the next section at databulkout.

			Writingflag = TRUE ,mean that when we writing resource ,and databulkout happen,
			So we bulk out when this packet finish.
		*/
		if ( (pHTTXContext->CurWritePosition  & BULKOUT_SIZE) == BULKOUT_SIZE)
		{
			CUR_WRITE_IDX_INC(pHTTXContext->CurWriteIdx, BUF_ALIGMENT_RINGSIZE);
			pHTTXContext->CurWritePosition = 0;
		}
#else
		if ((pHTTXContext->CurWritePosition + 3906)> MAX_TXBULK_LIMIT)
		{	/* Add 3906 for prevent the NextBulkOut packet size is a A-RALINK/A-MSDU Frame.*/
#ifndef MT_MAC
			pTxInfo->TxInfoSwLstRnd = 1;
#else
        TMAC_TXD_1 *txd_1 = &txd_s->txd_1;
        TMAC_TXD_7 *txd_7;

        if (txd_1->ft == TMI_FT_SHORT)
            txd_7 = &txd_s->txd_7;
        else
            txd_7 = &txd_l->txd_7;

        txd_7->sw_field = 1;
#endif
        pHTTXContext->CurWritePosition = 8;
		}
#endif /* USB_BULK_BUF_ALIGMENT */

		pHTTXContext->CurWriteRealPos = pHTTXContext->CurWritePosition;

#ifdef UAPSD_SUPPORT
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
		UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#else
#ifdef CONFIG_AP_SUPPORT
#ifdef P2P_SUPPORT
		if (P2P_GO_ON(pAd))
#else
#ifdef RT_CFG80211_P2P_SUPPORT
        if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
#else
        IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
#endif /* P2P_SUPPORT */
		{
#ifdef USB_BULK_BUF_ALIGMENT
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWriteIdx);
#else
			UAPSD_TagFrame(pAd, pTxBlk->pPacket, pTxBlk->Wcid, pHTTXContext->CurWritePosition);
#endif /* USB_BULK_BUF_ALIGMENT */
		}
#endif /* CONFIG_AP_SUPPORT */
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
#endif /* UAPSD_SUPPORT */

#ifdef USB_BULK_BUF_ALIGMENT
		pWirelessPacket = (&pHTTXContext->TransferBuffer[idx]->field.WirelessPacket[fillOffset + pTxBlk->Priv]);
#else
		pWirelessPacket = (&pHTTXContext->TransferBuffer->field.WirelessPacket[fillOffset + pTxBlk->Priv]);
#endif /* USB_BULK_BUF_ALIGMENT */
		NdisZeroMemory(pWirelessPacket, padding + 8);


		/* Finally, set bCurWriting as FALSE*/
		pHTTXContext->bCurWriting = FALSE;

	}
	else
	{	/* It should not happened now unless we are going to shutdown.*/
		DBGPRINT(RT_DEBUG_ERROR, ("FinalWriteTxResource():bCurWriting is FALSE when handle last frames.\n"));
	}

	RTMP_IRQ_UNLOCK(&pAd->TxContextQueueLock[QueIdx], IrqFlags);
}


VOID RtmpUSBDataLastTxIdx(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR QueIdx,
	IN USHORT TxIdx)
{
	/* DO nothing for USB.*/
}


/*
	When can do bulk-out:
		1. TxSwFreeIdx < TX_RING_SIZE;
			It means has at least one Ring entity is ready for bulk-out, kick it out.
		2. If TxSwFreeIdx == TX_RING_SIZE
			Check if the CurWriting flag is FALSE, if it's FALSE, we can do kick out.

*/
VOID RtmpUSBDataKickOut(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk,
	IN UCHAR QueIdx)
{
#ifdef USE_BMC
    if (QueIdx == QID_BMC)
        RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << EDCA_BMC_PIPE));
    else
#endif
	   RTUSB_SET_BULK_FLAG(pAd, (fRTUSB_BULK_OUT_DATA_NORMAL << QueIdx));

	RTUSBKickBulkOut(pAd);

#ifdef USE_BMC
    if (QueIdx == QID_BMC) {
        struct wifi_dev *wdev = pAd->wdev_list[pTxBlk->wdev_idx];
        RTMP_UPDATE_BMC_CNT(pAd, wdev->func_idx);
    }
#endif

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<---RtmpUSBDataKickOut\n"));
#endif /* RELEASE_EXCLUDE */
}


/*
	Must be run in Interrupt context
	This function handle RT2870 specific TxDesc and cpu index update and kick the packet out.
 */
int RtmpUSBMgmtKickOut(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR QueIdx,
	IN PNDIS_PACKET pPacket,
	IN UCHAR *pSrcBufVA,
	IN UINT SrcBufLen)
{
	TXINFO_STRUC *pTxInfo;
	ULONG BulkOutSize;
	ULONG padLen;
	PUCHAR pDest;
	ULONG SwIdx;
	TX_CONTEXT *pMLMEContext = NULL;
    TX_CONTEXT *pBcnContext = NULL;
	ULONG IrqFlags = 0;

	RTMP_IRQ_LOCK(&pAd->MLMEBulkOutLock, IrqFlags);
#if defined(MT7603)
	if (IS_MT7603(pAd))
	{
		if (QueIdx == Q_IDX_BCN) {
			SwIdx = pAd->BcnRing.TxCpuIdx;
            pBcnContext = (PTX_CONTEXT)pAd->BcnRing.Cell[SwIdx].AllocVa;
		} else {
			SwIdx = pAd->MgmtRing.TxCpuIdx;
            pMLMEContext = (PTX_CONTEXT)pAd->MgmtRing.Cell[SwIdx].AllocVa;
		}
	}
    else
#endif /* MT7603 */
    {
        SwIdx = pAd->MgmtRing.TxCpuIdx;
        pMLMEContext = (PTX_CONTEXT)pAd->MgmtRing.Cell[SwIdx].AllocVa;
    }


	pTxInfo = (TXINFO_STRUC *)(pSrcBufVA);

	/* Build our URB for USBD*/
	BulkOutSize = (SrcBufLen + 3) & (~3);
	rlt_usb_write_txinfo(pAd, pTxInfo, (USHORT)(BulkOutSize - TXINFO_SIZE), TRUE, EpToQueue[MGMTPIPEIDX], FALSE,  FALSE);

	BulkOutSize += 4; /* Always add 4 extra bytes at every packet.*/

//+++Add by shiang for debug
#if 0
if (0) {
	DBGPRINT(RT_DEBUG_OFF, ("-->%s():shiang-6590, QueIdx=%d, SrcBufLen=%d\n", __FUNCTION__, QueIdx, SrcBufLen));
	dump_txinfo(pAd, pTxInfo);
	dump_tmac_info(pAd, (UCHAR *)(pSrcBufVA + TXINFO_SIZE));
}
#endif
//---Add by shiang for debug

/* WY , it cause Tx hang on Amazon_SE , Max said the padding is useless*/
	/* If BulkOutSize is multiple of BulkOutMaxPacketSize, add extra 4 bytes again.*/
/*	if ((BulkOutSize % pAd->BulkOutMaxPacketSize) == 0)*/
/*		BulkOutSize += 4;*/

	padLen = BulkOutSize - SrcBufLen;
	ASSERT((padLen <= RTMP_PKT_TAIL_PADDING));

	/* Now memzero all extra padding bytes.*/
	pDest = (PUCHAR)(pSrcBufVA + SrcBufLen);

#ifdef MT_MAC
    if (QueIdx == Q_IDX_BCN)
    	{
    		if(!pBcnContext)
    		{
				DBGPRINT(RT_DEBUG_ERROR, ("-->%s():Error! null pointer!\n", __FUNCTION__));
				return -1;
			}
        	pBcnContext->ppkt_tail_before_padding = GET_OS_PKT_DATATAIL(pPacket);
    	}
#endif

	OS_PKT_TAIL_BUF_EXTEND(pPacket, padLen);
	NdisZeroMemory(pDest, padLen);

#ifdef RELEASE_EXCLUDE
//	RTMP_IRQ_LOCK(&pAd->MLMEBulkOutLock, IrqFlags); //from EPSON project , move to above 
#endif /* RELEASE_EXCLUDE */

#ifdef MT_MAC
    if (QueIdx == Q_IDX_BCN)
        pAd->BcnRing.Cell[pAd->BcnRing.TxCpuIdx].pNdisPacket = pPacket;
    else
#endif
	   pAd->MgmtRing.Cell[pAd->MgmtRing.TxCpuIdx].pNdisPacket = pPacket;

#ifdef MT_MAC
	/* append four zero bytes padding */
	memset(OS_PKT_TAIL_BUF_EXTEND(pPacket, USB_END_PADDING), 0x00, USB_END_PADDING);
#endif

#ifdef MT_MAC
    if (QueIdx == Q_IDX_BCN) {
        pBcnContext->TransferBuffer = (PTX_BUFFER)(GET_OS_PKT_DATAPTR(pPacket));

        /* Length in TxInfo should be 8 less than bulkout size.*/
        pBcnContext->BulkOutSize = BulkOutSize;
        pBcnContext->InUse = TRUE;
        pBcnContext->bWaitingBulkOut = TRUE;

#ifdef UAPSD_SUPPORT
        pBcnContext->Wcid = MCAST_WCID;
#endif /* UAPSD_SUPPORT */
    }
    else
#endif /* MT_MAC */
    {
        pMLMEContext->TransferBuffer = (PTX_BUFFER)(GET_OS_PKT_DATAPTR(pPacket));

        /* Length in TxInfo should be 8 less than bulkout size.*/
        pMLMEContext->BulkOutSize = BulkOutSize;
        pMLMEContext->InUse = TRUE;
        pMLMEContext->bWaitingBulkOut = TRUE;

#ifdef UAPSD_SUPPORT
		/*
			If the packet is QoS Null frame, we mark the packet with its WCID;
			If not, we mark the packet with bc/mc WCID = 0.

			We will handle it in rtusb_mgmt_dma_done_tasklet().

			Even AP send a QoS Null frame but not EOSP frame in USB mode,
			then we will call UAPSD_SP_Close() and we will check
			pEntry->bAPSDFlagSPStart() so do not worry about it.
		*/
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
	if (RTMP_GET_PACKET_QOS_NULL(pPacket) != 0x00)
		pMLMEContext->Wcid = RTMP_GET_PACKET_WCID(pPacket);
	else
		pMLMEContext->Wcid = MCAST_WCID;
#else
#ifdef CONFIG_AP_SUPPORT
#ifdef P2P_SUPPORT
	if (P2P_GO_ON(pAd))
#else
#ifdef RT_CFG80211_P2P_SUPPORT
        if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
#else
        IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
#endif /* P2P_SUPPORT */
	{
		if (RTMP_GET_PACKET_QOS_NULL(pPacket) != 0x00)
			pMLMEContext->Wcid = RTMP_GET_PACKET_WCID(pPacket);
		else
			pMLMEContext->Wcid = MCAST_WCID;
	}
#endif /* CONFIG_AP_SUPPORT */
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
#endif /* UAPSD_SUPPORT */
    }
	/*hex_dump("RtmpUSBMgmtKickOut", &pMLMEContext->TransferBuffer->field.WirelessPacket[0], (pMLMEContext->BulkOutSize > 16 ? 16 : pMLMEContext->BulkOutSize));*/

/*
	pAd->RalinkCounters.KickTxCount++;
	pAd->RalinkCounters.OneSecTxDoneCount++;

	if (pAd->MgmtRing.TxSwFreeIdx == MGMT_RING_SIZE)
		needKickOut = TRUE;
*/

	/* Decrease the TxSwFreeIdx and Increase the TX_CTX_IDX*/
#ifdef MT_MAC
    if (QueIdx == Q_IDX_BCN) {
        pAd->BcnRing.TxSwFreeIdx--;
        INC_RING_INDEX(pAd->BcnRing.TxCpuIdx, BCN_RING_SIZE);
    }
    else
#endif
    {
        pAd->MgmtRing.TxSwFreeIdx--;
        INC_RING_INDEX(pAd->MgmtRing.TxCpuIdx, MGMT_RING_SIZE);
    }

	RTMP_IRQ_UNLOCK(&pAd->MLMEBulkOutLock, IrqFlags);

#ifdef MT_MAC
    if (QueIdx == Q_IDX_BCN)
        RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_BCN);
    else
#endif
    {
	   RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_MLME);
    }
	/*if (needKickOut)*/
	RTUSBKickBulkOut(pAd);

	return 0;
}


VOID RtmpUSBNullFrameKickOut(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR QueIdx,
	IN UCHAR *pNullFrame,
	IN UINT32 frameLen)
{
	if (pAd->NullContext.InUse == FALSE)
	{
		PTX_CONTEXT pNullContext;
		TXINFO_STRUC *pTxInfo;
#ifndef MT_MAC
		TXWI_STRUC *pTxWI;
#endif /* MT_MAC */
		UCHAR *pWirelessPkt;
		UINT8 TXWISize = pAd->chipCap.TXWISize;
		MAC_TX_INFO mac_info;
#ifdef MT_MAC
		UCHAR *tmac_info;
		UINT tx_hw_hdr_len = pAd->chipCap.tx_hw_hdr_len;
		PHEADER_802_11 pHeader_802_11;

		pHeader_802_11 = (HEADER_802_11 *)(pNullFrame);
#endif
		pNullContext = &(pAd->NullContext);

		/* Set the in use bit*/
		pNullContext->InUse = TRUE;
		pWirelessPkt = (PUCHAR)&pNullContext->TransferBuffer->field.WirelessPacket[0];

		RTMPZeroMemory(&pWirelessPkt[0], 100);
		pTxInfo = (TXINFO_STRUC *)&pWirelessPkt[0];
		rlt_usb_write_txinfo(pAd, pTxInfo, (USHORT)(frameLen + TXWISize + TSO_SIZE), TRUE, EpToQueue[MGMTPIPEIDX], FALSE,  FALSE);
#ifndef MT_MAC
		pTxWI = (TXWI_STRUC *)&pWirelessPkt[TXINFO_SIZE];
#endif /* !MT_MAC */
		NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));

#ifdef MT_MAC
        tmac_info = &pWirelessPkt[0];
        if (pHeader_802_11->FC.Type == FC_TYPE_DATA) {
            switch (pHeader_802_11->FC.SubType) {
                case SUBTYPE_DATA_NULL:
                    mac_info.hdr_len = 24;
                    break;
                case SUBTYPE_QOS_NULL:
                    mac_info.hdr_len = 26;
                    break;
                default:
                    break;
            }
        }
#endif

		mac_info.FRAG = FALSE;

		mac_info.CFACK = FALSE;
		mac_info.InsTimestamp = FALSE;
		mac_info.AMPDU = FALSE;

		mac_info.BM = 0;
		mac_info.Ack = TRUE;
		mac_info.NSeq = FALSE;
		mac_info.BASize = 0;

		mac_info.WCID = BSSID_WCID;
		mac_info.Length = frameLen;
		mac_info.PID = 0;

#ifdef CONFIG_STA_SUPPORT
#if defined(MT_MAC) && (defined(STA_LP_PHASE_1_SUPPORT) || defined(STA_LP_PHASE_2_SUPPORT))
		if (pHeader_802_11->FC.PwrMgmt == PWR_ACTIVE)
		{
			mac_info.PID = PID_NULL_FRAME_PWR_ACTIVE;
		}
		else
		{
			mac_info.PID = PID_NULL_FRAME_PWR_SAVE;
		}

		if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)) && RTMPEnterPsmNullBitStatus(&pAd->StaCfg.PwrMgmt))
		{
			//mac_info.TxS2Host = 1;
			//TxSTypeCtl(pAd, mac_info.PID, TXS_FORMAT0, FALSE, TRUE);
			DBGPRINT(RT_DEBUG_TRACE, ("%s(line=%d)\n", __FUNCTION__, __LINE__));
			RTMPClearEnterPsmNullBit(&pAd->StaCfg.PwrMgmt);
			TxSTypeCtlPerPkt(pAd, mac_info.PID, TXS_FORMAT0, FALSE, TRUE, FALSE, TXS_DUMP_REPEAT); 
		}
		else if(pAd->cfg80211_ctrl.TxStatusInUsed == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s(line=%d), Enable tx status.\n", __FUNCTION__, __LINE__));
			TxSTypeCtlPerPkt(pAd, mac_info.PID, TXS_FORMAT0, FALSE, TRUE, FALSE, TXS_DUMP_REPEAT); 		
		}
		else
		{
			/* In scan initial phase, driver needs not handle PID_NULL_FRAME_PWR_SAVE 
			since driver will wakeup H/W later at ScanNextChannel() to do scan */
			//mac_info.TxS2Host = 0;
			//TxSTypeCtl(pAd, mac_info.PID, TXS_FORMAT0, FALSE, FALSE);
			DBGPRINT(RT_DEBUG_TRACE, ("%s(line=%d)\n", __FUNCTION__, __LINE__));
			TxSTypeCtlPerPkt(pAd, mac_info.PID, TXS_FORMAT0, FALSE, FALSE, FALSE, TXS_DUMP_REPEAT); 
		}
#endif /* MT_MAC */
#endif /* CONFIG_STA_SUPPORT */

		mac_info.TID = 0;
		mac_info.TxRate = (UCHAR)pAd->CommonCfg.MlmeTransmit.field.MCS;
		mac_info.Txopmode = IFS_HTTXOP;
		mac_info.Preamble = LONG_PREAMBLE;
#ifdef MT_MAC

		/* For  MT STA LP control, use H/W control mode for PM bit */
#if defined(CONFIG_STA_SUPPORT) && defined(CONFIG_PM_BIT_HW_MODE)
		mac_info.PsmBySw = 0;
#else
		mac_info.PsmBySw = 1;
#endif /* CONFIG_STA_SUPPORT && CONFIG_PM_BIT_HW_MODE */


		// TODO: shiang-MT7603
/*		mac_info.q_idx = Q_IDX_AC0;*/
/*change null frame with mgn*/
		mac_info.q_idx = Q_IDX_AC4;

		write_tmac_info(pAd, (UCHAR *)tmac_info, &mac_info, &pAd->CommonCfg.MlmeTransmit);
        RTMPMoveMemory(&pWirelessPkt[tx_hw_hdr_len], pNullFrame, frameLen);
        pAd->NullContext.BulkOutSize =  tx_hw_hdr_len + frameLen + 4;
#else
		write_tmac_info(pAd, (UCHAR *)pTxWI, &mac_info, &pAd->CommonCfg.MlmeTransmit);
#ifdef RT_BIG_ENDIAN
		RTMPWIEndianChange(pAd, (PUCHAR)pTxWI, TYPE_TXWI);
#endif /* RT_BIG_ENDIAN */
		RTMPMoveMemory(&pWirelessPkt[TXWISize + TXINFO_SIZE + TSO_SIZE], pNullFrame, frameLen);
#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, (PUCHAR)&pWirelessPkt[TXINFO_SIZE + TXWISize + TSO_SIZE], DIR_WRITE, FALSE);
#endif /* RT_BIG_ENDIAN */
		pAd->NullContext.BulkOutSize =  TXINFO_SIZE + TXWISize + TSO_SIZE + frameLen + 4;
#endif /* MT_MAC */

		pAd->NullContext.BulkOutSize = ( pAd->NullContext.BulkOutSize + 3) & (~3);

#ifdef MT_MAC
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Send NULL Frame @%d Mbps...(PwrMgmt, PID)(%d, %d)\n", __FUNCTION__, RateIdToMbps[pAd->CommonCfg.TxRate], pHeader_802_11->FC.PwrMgmt, mac_info.PID));
#else
		DBGPRINT(RT_DEBUG_TRACE, ("%s - Send NULL Frame @%d Mbps...\n", __FUNCTION__, RateIdToMbps[pAd->CommonCfg.TxRate]));
#endif

		DBGPRINT(RT_DEBUG_TRACE, ("%s - Send NULL Frame @%d Mbps...\n", __FUNCTION__, RateIdToMbps[pAd->CommonCfg.TxRate]));
		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_NULL);

		pAd->Sequence = (pAd->Sequence+1) & MAXSEQ;

		/* Kick bulk out */
		RTUSBKickBulkOut(pAd);
	}

}

/*
========================================================================
Routine Description:
    Get a received packet.

Arguments:
	pAd					device control block
	pSaveRxD			receive descriptor information
	*pbReschedule		need reschedule flag
	*pRxPending			pending received packet flag

Return Value:
    the recieved packet

Note:
========================================================================
*/
PNDIS_PACKET GetPacketFromRxRing(
	RTMP_ADAPTER *pAd,
	RX_BLK *pRxBlk,
	BOOLEAN *pbReschedule,
	UINT32 *pRxPending,
	UCHAR RxRingNo)
{
	RX_CONTEXT *pRxContext;
	PNDIS_PACKET pNetPkt = NULL;
	UCHAR *pData;
	ULONG ThisFrameLen, RxBufferLength, valid_len;
#if defined(RLT_MAC) || defined(RTMP_MAC)
	RXWI_STRUC *pRxWI;
#endif
	UINT8 RXWISize = pAd->chipCap.RXWISize;
	RXINFO_STRUC *pRxInfo = NULL;
#ifdef RLT_MAC
	RXFCE_INFO *pRxFceInfo;
#endif /* RLT_MAC */
#ifdef MT_MAC
	UINT8 rx_hw_hdr_len = pAd->chipCap.RXWISize;
	UINT32 padding = 0;
	static int i = 0;
#endif
    union rmac_rxd_0 *rxd_0;
    struct rxd_base_struc *rx_base;

	pRxContext = &pAd->RxContext[pAd->RxBulkInReadIndex];

	pRxBlk->Flags = 0;
	RxBufferLength = pRxContext->BulkInOffset - pRxContext->ReadPosition;

#ifdef MT_MAC
	valid_len = 8;
#else
	valid_len = RXDMA_FIELD_SIZE + RXWISize + sizeof(RXINFO_STRUC);
#endif /* MT_MAC */

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		valid_len += sizeof(RXFCE_INFO);
#endif /* RLT_MAC */

	if (RxBufferLength < valid_len)
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("%s(): Remaining RxBufLen(%ld) small than MinSize(%ld)!\n", __FUNCTION__, RxBufferLength, valid_len));
#endif /* RELEASE_EXCLUDE */
		goto label_null;
	}

	pData = &pRxContext->TransferBuffer[pRxContext->ReadPosition];
//+++Add by shiang for debug
if (0) {
	hex_dump("GetPacketFromRxRing", pData, (RxBufferLength > 7000 ? 7000 : RxBufferLength));
}
//---Add by shiang for debug

#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange(pAd, pData, TYPE_RXWI);
#endif /* RT_BIG_ENDIAN */

	/* The RXDMA field is 4 bytes, now just use the first 2 bytes. The Length including the (RXWI + MSDU + Padding) */
	ThisFrameLen = *pData + (*(pData + 1) << 8);

#ifndef MT_MAC
	if (ThisFrameLen == 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("BIRIdx(%d): RXDMALen is zero.[%ld], BulkInBufLen = %ld)\n",
								pAd->NextRxBulkInReadIndex, ThisFrameLen, pRxContext->BulkInOffset));
		goto label_null;
	}
	if ((ThisFrameLen & 0x3) != 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("BIRIdx(%d): RXDMALen not multiple of 4.[%ld], BulkInBufLen = %ld)\n",
								pAd->NextRxBulkInReadIndex, ThisFrameLen, pRxContext->BulkInOffset));
		goto label_null;
	}

	if ((ThisFrameLen + 8) > RxBufferLength)	/* 8 for (RXDMA_FIELD_SIZE + sizeof(RXINFO_STRUC))*/
	{
		DBGPRINT(RT_DEBUG_TRACE,("BIRIdx(%d):FrameLen(0x%lx) outranges. BulkInLen=0x%lx, remaining RxBufLen=0x%lx, ReadPos=0x%lx\n",
						pAd->NextRxBulkInReadIndex, ThisFrameLen, pRxContext->BulkInOffset, RxBufferLength, pRxContext->ReadPosition));

		/* error frame. finish this loop*/
		goto label_null;
	}

	pData += RXDMA_FIELD_SIZE;
#endif /* MT_MAC */

#ifdef RLT_MAC
#ifdef MT7601
	if (IS_MT7601(pAd)) {
		struct _RXWI_XMAC *rxwi_x;
		pRxInfo = (RXINFO_STRUC *)pData;
		pRxFceInfo = (RXFCE_INFO *)(pData + ThisFrameLen);
		pData += RAL_RXINFO_SIZE;
		pRxWI = (RXWI_STRUC *)pData;
		rxwi_x = (struct _RXWI_XMAC *)pData;
		pRxBlk->pRxWI = (RXWI_STRUC *)pData;
		pRxBlk->MPDUtotalByteCnt = rxwi_x->MPDUtotalByteCnt;
		pRxBlk->wcid = rxwi_x->wcid;
		pRxBlk->key_idx = rxwi_x->key_idx;
		pRxBlk->bss_idx = rxwi_x->bss_idx;
		pRxBlk->TID = rxwi_x->tid;
		pRxBlk->DataSize = rxwi_x->MPDUtotalByteCnt;
		pRxBlk->rx_rate.field.MODE = rxwi_x->phy_mode;
		pRxBlk->rx_rate.field.MCS = rxwi_x->mcs;
		pRxBlk->rx_rate.field.BW = rxwi_x->bw;
		pRxBlk->rx_rate.field.STBC = rxwi_x->stbc;
		pRxBlk->rx_rate.field.ShortGI = rxwi_x->sgi;
		pRxBlk->rssi[0] = rxwi_x->SNR2; /* RSSI */
		pRxBlk->rssi[1] = rxwi_x->SNR1; /* Antenna ID */
		pRxBlk->rssi[2] = rxwi_x->bw; /* BW */
		pRxBlk->snr[0] = rxwi_x->SNR0;
		pRxBlk->snr[1] = 0;
		pRxBlk->snr[2] = 0;
		pRxBlk->freq_offset = rxwi_x->FOFFSET;
	} else
#endif
	if (pAd->chipCap.hif_type == HIF_RLT) {
		struct _RXWI_NMAC *rxwi_n;
		pRxInfo = (RXINFO_STRUC *)pData;
		pRxFceInfo = (RXFCE_INFO *)(pData + ThisFrameLen);
		pData += RAL_RXINFO_SIZE;
		pRxWI = (RXWI_STRUC *)pData;
		rxwi_n = (struct _RXWI_NMAC *)pData;;
		pRxBlk->pRxWI = (RXWI_STRUC *)pData;
		pRxBlk->MPDUtotalByteCnt = rxwi_n->MPDUtotalByteCnt;
		pRxBlk->wcid = rxwi_n->wcid;
		pRxBlk->key_idx = rxwi_n->key_idx;
		pRxBlk->bss_idx = rxwi_n->bss_idx;
		pRxBlk->TID = rxwi_n->tid;
		pRxBlk->DataSize = rxwi_n->MPDUtotalByteCnt;
		pRxBlk->rx_rate.field.MODE = rxwi_n->phy_mode;
		pRxBlk->rx_rate.field.MCS = rxwi_n->mcs;
		pRxBlk->rx_rate.field.ldpc = rxwi_n->ldpc;
		pRxBlk->rx_rate.field.BW = rxwi_n->bw;
		pRxBlk->rx_rate.field.STBC = rxwi_n->stbc;
		pRxBlk->rx_rate.field.ShortGI = rxwi_n->sgi;
		pRxBlk->rx_signal.raw_rssi[0] = rxwi_n->rssi[0];
		pRxBlk->rx_signal.raw_rssi[1] = rxwi_n->rssi[1];
		pRxBlk->rx_signal.raw_rssi[2] = rxwi_n->rssi[2];
		pRxBlk->rx_signal.raw_snr[0] = rxwi_n->bbp_rxinfo[0];
		pRxBlk->rx_signal.raw_snr[1] = rxwi_n->bbp_rxinfo[1];
		pRxBlk->rx_signal.raw_snr[2] = rxwi_n->bbp_rxinfo[2];
		pRxBlk->rx_signal.freq_offset = rxwi_n->bbp_rxinfo[4];
		pRxBlk->ldpc_ex_sym = rxwi_n->ldpc_ex_sym;
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		struct _RXWI_OMAC *rxwi_o;
		pRxInfo = (RXINFO_STRUC *)(pData + ThisFrameLen);
		pRxWI = (RXWI_STRUC *)pData;

		rxwi_o = (struct _RXWI_OMAC *)(pData);
		pRxBlk->pRxWI = (RXWI_STRUC *)(pData);
		pRxBlk->MPDUtotalByteCnt = rxwi_o->MPDUtotalByteCnt;
		pRxBlk->wcid = rxwi_o->wcid;
		pRxBlk->key_idx = rxwi_o->key_idx;
		pRxBlk->bss_idx = rxwi_o->bss_idx;
		pRxBlk->TID = rxwi_o->tid;
		pRxBlk->DataSize = rxwi_o->MPDUtotalByteCnt;

		pRxBlk->rx_rate.field.MODE = rxwi_o->phy_mode;
		pRxBlk->rx_rate.field.MCS = rxwi_o->mcs;
		pRxBlk->rx_rate.field.BW = rxwi_o->bw;
		pRxBlk->rx_rate.field.STBC = rxwi_o->stbc;
		pRxBlk->rx_rate.field.ShortGI = rxwi_o->sgi;
		pRxBlk->rssi[0] = rxwi_o->RSSI0;
		pRxBlk->rssi[1] = rxwi_o->RSSI1;
		pRxBlk->rssi[2] = rxwi_o->RSSI2;
		pRxBlk->snr[0] = rxwi_o->SNR0;
		pRxBlk->snr[1] = rxwi_o->SNR1;
		pRxBlk->snr[2] = rxwi_o->SNR2;
		pRxBlk->freq_offset = rxwi_o->FOFFSET;
	}
#endif /* RTMP_MAC */

#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT) {
		//union rmac_rxd_0 *rxd_0;
		rxd_0 = (union rmac_rxd_0 *)pData;

		/* allocate a rx packet */
		pNetPkt = RTMP_AllocateFragPacketBuffer(pAd, ThisFrameLen);

		if (pNetPkt == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s():Cannot Allocate sk buffer for this Bulk-In buffer!\n", __FUNCTION__));
				goto label_null;
		}

        /* copy the rx packet*/
#if 0
		hex_dump("pData rx data:", (char *)pData, ThisFrameLen);
#endif
		RTMP_USB_PKT_COPY(get_netdev_from_bssid(pAd, BSS0), pNetPkt, ThisFrameLen, pData);


		rxd_0 = (union rmac_rxd_0 *)(GET_OS_PKT_DATAPTR(pNetPkt));
		rx_base = (struct rxd_base_struc *)rxd_0;

		if ((rx_base->rxd_0.rx_byte_cnt < 10 )||(rx_base->rxd_0.rx_byte_cnt > MAX_AGGREGATION_SIZE) )
		{
			if (i < 5)
			{
				DBGPRINT(RT_DEBUG_ERROR,("%s:rx urb check rx_base->rxd_0.rx_byte_cnt %d\n", 
					__FUNCTION__, rx_base->rxd_0.rx_byte_cnt));
			}
			i++;
			RELEASE_NDIS_PACKET(pAd, pNetPkt, NDIS_STATUS_SUCCESS);
			return NULL;
		}


		rx_hw_hdr_len = (UINT8)parse_rx_packet_type(pAd, pRxBlk, pNetPkt);
		if(rx_hw_hdr_len == INVALID_RX_PACKET_LEN)
		{
			RELEASE_NDIS_PACKET(pAd, pNetPkt, NDIS_STATUS_FAILURE);
			DBGPRINT(RT_DEBUG_ERROR,("%s():Invalid Rx packet lenght, release buff and return!\n", __FUNCTION__));
			return NULL;
		}

		if (RX_BLK_TEST_FLAG(pRxBlk, fRX_CMD_RSP))
		{
			RELEASE_NDIS_PACKET(pAd, pNetPkt, NDIS_STATUS_SUCCESS);
			// Only one RX_CMD_RSP exist in one RxContext, update ReadPosition
			// to prevent the same RxContext being handled again.
			pRxContext->ReadPosition += pRxContext->BulkInOffset;
			return NULL;
		}

		ThisFrameLen -= rx_hw_hdr_len;
	}
#endif /* MT_MAC */

#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange(pAd, pData, TYPE_RXWI);
#endif /* RT_BIG_ENDIAN */
	if (pRxBlk->MPDUtotalByteCnt > ThisFrameLen)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():pRxWIMPDUtotalByteCount(%d) large than RxDMALen(%ld)(RxBlk.Flags = 0x%x)\n",
									__FUNCTION__, pRxBlk->MPDUtotalByteCnt, ThisFrameLen, pRxBlk->Flags));
		if (pNetPkt && (!RX_BLK_TEST_FLAG(pRxBlk, fRX_RETRIEVE)))
			RELEASE_NDIS_PACKET(pAd, pNetPkt, NDIS_STATUS_SUCCESS);
		goto label_null;
	}
#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange(pAd, pData, TYPE_RXWI);
#endif /* RT_BIG_ENDIAN */


	if (pAd->chipCap.hif_type != HIF_MT)
	{
		/* allocate a rx packet*/
		pNetPkt = RTMP_AllocateFragPacketBuffer(pAd, ThisFrameLen);
		if (pNetPkt == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s():Cannot Allocate sk buffer for this Bulk-In buffer!\n", __FUNCTION__));
			goto label_null;
		}

		/* copy the rx packet*/
		RTMP_USB_PKT_COPY(get_netdev_from_bssid(pAd, BSS0), pNetPkt, ThisFrameLen, pData);
	}

#ifdef RT_BIG_ENDIAN
	RTMPDescriptorEndianChange((PUCHAR)pRxInfo, TYPE_RXINFO);
#endif /* RT_BIG_ENDIAN */

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		NdisMoveMemory((VOID *)&pRxBlk->hw_rx_info[0], (VOID *)pRxFceInfo, sizeof(RXFCE_INFO));
		pRxBlk->pRxFceInfo = (RXFCE_INFO *)&pRxBlk->hw_rx_info[0];
	}
#endif /* RLT_MAC */

#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT)
	{
		/* update next packet read position.*/
		padding = (4 - ((ThisFrameLen + rx_hw_hdr_len) % 4)) & 0x03;  /* round up to 4 byte alignment*/
		pRxContext->ReadPosition += (ThisFrameLen + rx_hw_hdr_len + padding + 4); // 4:CSO
	}
	else
#endif /* MT_MAC */
	{
		if(pRxInfo)
		{
			NdisMoveMemory(&pRxBlk->hw_rx_info[RXINFO_OFFSET], pRxInfo, RAL_RXINFO_SIZE);
		}
		pRxBlk->pRxInfo = (RXINFO_STRUC *)&pRxBlk->hw_rx_info[RXINFO_OFFSET];

		/* update next packet read position.*/
		pRxContext->ReadPosition += (ThisFrameLen + RXDMA_FIELD_SIZE + RAL_RXINFO_SIZE);	/* 8 for (RXDMA_FIELD_SIZE + sizeof(RXINFO_STRUC))*/
	}

	pRxBlk->pRxPacket = pNetPkt;
	pRxBlk->pData = (UCHAR *)GET_OS_PKT_DATAPTR(pNetPkt);
#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT)
	{
		pRxBlk->pHeader = (HEADER_802_11 *)(pRxBlk->pData);
	}
	else
#endif /* MT_MAC */
	{
		pRxBlk->pHeader = (HEADER_802_11 *)(pRxBlk->pData + RXWISize);
	}

	pRxBlk->Flags = 0;

	return pNetPkt;

label_null:
	if (pNetPkt)
		RELEASE_NDIS_PACKET(pAd, pNetPkt, NDIS_STATUS_SUCCESS);
	return NULL;
}


#ifdef CONFIG_STA_SUPPORT
VOID RtmpUsbStaAsicForceWakeupTimeout(
	IN PVOID SystemSpecific1,
	IN PVOID FunctionContext,
	IN PVOID SystemSpecific2,
	IN PVOID SystemSpecific3)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<-- RtmpUsbStaAsicForceWakeupTimeout\n"));
#endif /* RELEASE_EXCLUDE */


	if (pAd && pAd->Mlme.AutoWakeupTimerRunning)
	{
#ifdef MT7601
		if ( IS_MT7601(pAd) )
		{
			if ( !OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) )
			{
				RTMPSetTimer(&pAd->Mlme.AutoWakeupTimer, AUTO_WAKEUP_TIMEOUT);
				return;
			}

			ASIC_RADIO_ON(pAd, MLME_RADIO_ON);
		}
		else
#endif /* MT7601 */
		{
			RTUSBBulkReceive(pAd);

			AsicSendCommandToMcu(pAd, 0x31, 0xff, 0x00, 0x02, FALSE);
		}
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
		pAd->Mlme.AutoWakeupTimerRunning = FALSE;
	}
}


VOID RT28xxUsbStaAsicForceWakeup(RTMP_ADAPTER *pAd, BOOLEAN bFromTx)
{
#if defined(STA_LP_PHASE_1_SUPPORT) || defined(STA_LP_PHASE_2_SUPPORT)
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): -->\n", __FUNCTION__));
	RTMPOffloadPm(pAd, BSSID_WCID, PM4, EXIT_PM_STATE);	
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): <--\n", __FUNCTION__));
#else	
	BOOLEAN	Canceled;

	if (pAd->Mlme.AutoWakeupTimerRunning)
	{
		RTMPCancelTimer(&pAd->Mlme.AutoWakeupTimer, &Canceled);
		pAd->Mlme.AutoWakeupTimerRunning = FALSE;
	}

#ifdef MT7601
	if ( IS_MT7601(pAd) )
	{
		ASIC_RADIO_ON(pAd, DOT11_RADIO_ON);
	}
	else
#endif /* MT7601 */
	AsicSendCommandToMcu(pAd, 0x31, 0xff, 0x00, 0x02, FALSE);

	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);
#endif /* STA_LP_PHASE_1_SUPPORT || STA_LP_PHASE_2_SUPPORT  */	
}


VOID RT28xxUsbStaAsicSleepThenAutoWakeup(
	IN PRTMP_ADAPTER pAd,
	IN USHORT TbttNumToNextWakeUp)
{
#if defined(STA_LP_PHASE_1_SUPPORT) || defined(STA_LP_PHASE_2_SUPPORT)
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): -->\n", __FUNCTION__));
	RTMPOffloadPm(pAd, BSSID_WCID, PM4, ENTER_PM_STATE);	
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): <--\n", __FUNCTION__));
#else
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s(): TbttNumToNextWakeUp=%d 1\n",
				__FUNCTION__, TbttNumToNextWakeUp));
#endif /* RELEASE_EXCLUDE */

	/* Not going to sleep if in the Count Down Time*/
	if (pAd->CountDowntoPsm > 0)
		return;

	/* we have decided to SLEEP, so at least do it for a BEACON period.*/
	if (TbttNumToNextWakeUp == 0)
		TbttNumToNextWakeUp = 1;

	RTMPSetTimer(&pAd->Mlme.AutoWakeupTimer, AUTO_WAKEUP_TIMEOUT);
	pAd->Mlme.AutoWakeupTimerRunning = TRUE;

#ifdef MT7601
	if ( IS_MT7601(pAd) )
	{
		ASIC_RADIO_OFF(pAd, DOT11_RADIO_OFF);
	}
	else
#endif /* MT7601 */
	{
		AsicSendCommandToMcu(pAd, 0x30, 0xff, 0xff, 0x02, FALSE);   /* send POWER-SAVE command to MCU. Timeout 40us.*/

		/* cancel bulk-in IRPs prevent blocking CPU enter C3.*/
		if((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			RTUSBCancelPendingBulkInIRP(pAd);
			/* resend bulk-in IRPs to receive beacons after a period of (pAd->CommonCfg.BeaconPeriod - 40) ms*/
		}
	}

	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_DOZE);
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<-- AsicSleepThenAutoWakeup, TbttNumToNextWakeUp=%d \n", TbttNumToNextWakeUp));
#endif /* RELEASE_EXCLUDE */
#endif /* STA_LP_PHASE_1_SUPPORT || STA_LP_PHASE_2_SUPPORT */
}
#endif /* CONFIG_STA_SUPPORT */

#endif /* RTMP_MAC_USB */

