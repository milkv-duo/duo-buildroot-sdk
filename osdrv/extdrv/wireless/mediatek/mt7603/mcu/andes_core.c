/*
 ***************************************************************************
 * MediaTek Inc.
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

	Module Name:
	andes_core.c
*/
#include "rt_config.h"

struct cmd_msg *AndesAllocCmdMsg(RTMP_ADAPTER *ad, unsigned int length)
{
	struct cmd_msg *msg = NULL;
	RTMP_CHIP_CAP *cap = &ad->chipCap;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	PNDIS_PACKET net_pkt = NULL;
#ifdef RTMP_USB_SUPPORT
	PURB urb = NULL;
#endif
	INT32 AllocateSize = cap->cmd_header_len + length + cap->cmd_padding_len;

#ifdef RTMP_SDIO_SUPPORT
	if (AllocateSize >= ad->BlockSize)
		AllocateSize = (AllocateSize + ((-AllocateSize) & (ad->BlockSize - 1)));
#endif

	net_pkt = RTMP_AllocateFragPacketBuffer(ad, AllocateSize);

	if (!net_pkt) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate net_pkt\n"));
		goto error0;
	}

	OS_PKT_RESERVE(net_pkt, cap->cmd_header_len);

	os_alloc_mem(NULL, (PUCHAR *)&msg, sizeof(*msg));

	if (!msg) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate cmd msg\n"));
		goto error1;
	}

	CMD_MSG_CB(net_pkt)->msg = msg;

	memset(msg, 0x00, sizeof(*msg));

#ifdef RTMP_USB_SUPPORT
	urb = RTUSB_ALLOC_URB(0);

	if (!urb) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate urb\n"));
		goto error2;
	}

	msg->urb = urb;

#ifdef USB_IOT_WORKAROUND2
	os_alloc_mem(NULL, (PUCHAR *)&(msg->usb_iot_w2_buf), AllocateSize+W2_IOT_MAX_PADDING_SIZE);

	if (!(msg->usb_iot_w2_buf)) {
		DBGPRINT(RT_DEBUG_ERROR, ("can not allocate urb\n"));
		goto error2;
	}

#endif	
#endif

	msg->priv = (void *)ad;
	msg->net_pkt = net_pkt;

	ctl->alloc_cmd_msg++;

	return msg;

#ifdef RTMP_USB_SUPPORT
error2:
#endif
	os_free_mem(NULL, msg);
error1:
	RTMPFreeNdisPacket(ad, net_pkt);
error0:
	return NULL;
}


VOID AndesInitCmdMsg(struct cmd_msg *msg, UINT16 pq_id, UINT8 cmd_type, UINT8 set_query, UINT8 ExtCmdType, BOOLEAN need_wait, UINT16 timeout, BOOLEAN need_retransmit, BOOLEAN need_rsp, UINT16 rsp_payload_len, char *rsp_payload, MSG_RSP_HANDLER rsp_handler)
{
	msg->pq_id = pq_id;
	msg->cmd_type = cmd_type;
	msg->set_query = set_query;
	msg->ext_cmd_type = ExtCmdType;
	msg->need_wait= need_wait;
	msg->timeout = timeout;

	if (need_wait) {
		RTMP_OS_INIT_COMPLETION(&msg->ack_done);
	}

	msg->need_retransmit = 0;

#ifdef RTMP_USB_SUPPORT
	if (need_retransmit)
		msg->retransmit_times = CMD_MSG_RETRANSMIT_TIMES;
#else
		msg->retransmit_times = 0;
#endif

	msg->need_rsp = need_rsp;
	msg->rsp_payload_len = rsp_payload_len;
	msg->rsp_payload = rsp_payload;
	msg->rsp_handler = rsp_handler;

#ifdef RTMP_SDIO_SUPPORT
	RTMP_OS_INIT_COMPLETION(&msg->tx_sdio_done);
#endif
}


VOID AndesAppendCmdMsg(struct cmd_msg *msg, char *data, unsigned int len)
{
	PNDIS_PACKET net_pkt = msg->net_pkt;

	if (data)
		memcpy(OS_PKT_TAIL_BUF_EXTEND(net_pkt, len), data, len);
}


VOID AndesFreeCmdMsg(struct cmd_msg *msg)
{
#if defined(RTMP_USB_SUPPORT) || defined(RTMP_SDIO_SUPPORT)
	PNDIS_PACKET net_pkt = msg->net_pkt;
#endif
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)(msg->priv);
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	if (msg->need_wait) {
		RTMP_OS_EXIT_COMPLETION(&msg->ack_done);
	}

#ifdef RTMP_SDIO_SUPPORT
        RTMP_OS_EXIT_COMPLETION(&msg->tx_sdio_done);
#endif


#ifdef RTMP_USB_SUPPORT
	RTUSB_FREE_URB(msg->urb);

#ifdef USB_IOT_WORKAROUND2
	os_free_mem(NULL, msg->usb_iot_w2_buf);
#endif
#endif

	os_free_mem(NULL, msg);

#if defined(RTMP_USB_SUPPORT) || defined(RTMP_SDIO_SUPPORT)
	if (net_pkt)
		RTMPFreeNdisPacket(ad, net_pkt);
#endif

	ctl->free_cmd_msg++;
}



VOID AndesForceFreeCmdMsg(struct cmd_msg *msg)
{
	PNDIS_PACKET net_pkt = msg->net_pkt;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)(msg->priv);
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	if (msg->need_wait) {
		RTMP_OS_EXIT_COMPLETION(&msg->ack_done);
	}

#ifdef RTMP_SDIO_SUPPORT
        RTMP_OS_EXIT_COMPLETION(&msg->tx_sdio_done);
#endif


#ifdef RTMP_USB_SUPPORT
	RTUSB_FREE_URB(msg->urb);
#ifdef USB_IOT_WORKAROUND2
	os_free_mem(NULL, msg->usb_iot_w2_buf);
#endif	
#endif

	os_free_mem(NULL, msg);

	if (net_pkt)
		RTMPFreeNdisPacket(ad, net_pkt);

	ctl->free_cmd_msg++;
}


BOOLEAN IsInbandCmdProcessing(RTMP_ADAPTER *ad)
{
	BOOLEAN ret = 0;

	return ret;
}


UCHAR GetCmdRspNum(RTMP_ADAPTER *ad)
{
	UCHAR Num = 0;

	return Num;
}


VOID AndesIncErrorCount(struct MCU_CTRL *ctl, enum cmd_msg_error_type type)
{
	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		switch (type) {
		case error_tx_kickout_fail:
			ctl->tx_kickout_fail_count++;
		break;
		case error_tx_timeout_fail:
			ctl->tx_timeout_fail_count++;
		break;
		case error_rx_receive_fail:
			ctl->rx_receive_fail_count++;
		break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("%s:unknown cmd_msg_error_type(%d)\n", __FUNCTION__, type));
		}
	}
}


static NDIS_SPIN_LOCK *AndesGetSpinLock(struct MCU_CTRL *ctl, DL_LIST *list)
{
	NDIS_SPIN_LOCK *lock = NULL;

	if (list == &ctl->txq)
		lock = &ctl->txq_lock;
	else if (list == &ctl->rxq)
		lock = &ctl->rxq_lock;
	else if (list == &ctl->ackq)
		lock = &ctl->ackq_lock;
	else if (list == &ctl->kickq)
		lock = &ctl->kickq_lock;
	else if (list == &ctl->tx_doneq)
		lock = &ctl->tx_doneq_lock;
	else if (list == &ctl->rx_doneq)
		lock = &ctl->rx_doneq_lock;
#ifdef RTMP_SDIO_SUPPORT
	else if (list == &ctl->txq_sdio)
		lock = &ctl->txq_sdio_lock;
#endif
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s:illegal list\n", __FUNCTION__));

	return lock;
}


static inline UCHAR AndesGetCmdMsgSeq(RTMP_ADAPTER *ad)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg;
	unsigned long flags;

	RTMP_SPIN_LOCK_IRQSAVE(&ctl->ackq_lock, &flags);
get_seq:
	ctl->cmd_seq >= 0xf ? ctl->cmd_seq = 1 : ctl->cmd_seq++;
	DlListForEach(msg, &ctl->ackq, struct cmd_msg, list) {
		if (msg->seq == ctl->cmd_seq) {
			DBGPRINT(RT_DEBUG_ERROR, ("command(seq: %d) is still running\n", ctl->cmd_seq));
			DBGPRINT(RT_DEBUG_ERROR, ("command response nums = %d\n", GetCmdRspNum(ad)));
			goto get_seq;
		}
	}
	RTMP_SPIN_UNLOCK_IRQRESTORE(&ctl->ackq_lock, &flags);

	return ctl->cmd_seq;
}


static VOID _AndesQueueTailCmdMsg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	msg->state = state;
	DlListAddTail(list, &msg->list);
}


VOID AndesQueueTailCmdMsg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	unsigned long flags;
	NDIS_SPIN_LOCK *lock;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)msg->priv;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	_AndesQueueTailCmdMsg(list, msg, state);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
}


static VOID _AndesQueueHeadCmdMsg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	msg->state = state;
	DlListAdd(list, &msg->list);
}


VOID AndesQueueHeadCmdMsg(DL_LIST *list, struct cmd_msg *msg,
										enum cmd_msg_state state)
{
	unsigned long flags;
	NDIS_SPIN_LOCK *lock;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)msg->priv;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	_AndesQueueHeadCmdMsg(list, msg, state);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
}


UINT32 AndesQueueLen(struct MCU_CTRL *ctl, DL_LIST *list)
{
	UINT32 qlen;
	unsigned long flags;
	NDIS_SPIN_LOCK *lock;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	qlen = DlListLen(list);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);

	return qlen;
}

/*Nobody uses it currently*/
#if 0
static INT32 AndesQueueEmpty(struct MCU_CTRL *ctl, DL_LIST *list)
{
	unsigned long flags;
	int is_empty;
	NDIS_SPIN_LOCK *lock;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	is_empty = DlListEmpty(list);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);

	return is_empty;
}
#endif

static VOID AndesQueueInit(struct MCU_CTRL *ctl, DL_LIST *list)
{

	unsigned long flags;
	NDIS_SPIN_LOCK *lock;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	DlListInit(list);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
}


VOID _AndesUnlinkCmdMsg(struct cmd_msg *msg, DL_LIST *list)
{
	if (!msg)
		return;

	DlListDel(&msg->list);
}


VOID AndesUnlinkCmdMsg(struct cmd_msg *msg, DL_LIST *list)
{
	unsigned long flags;
	NDIS_SPIN_LOCK *lock;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)msg->priv;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	struct cmd_msg *current_msg;
	DL_LIST *current_list;
	UCHAR bFound = FALSE;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	for (current_list = list;
			current_list && current_list->Next && (list != current_list->Next);
			current_list = current_list->Next) {
		current_msg = DlListEntry(current_list->Next, struct cmd_msg, list);
		if (current_msg == msg) {
			_AndesUnlinkCmdMsg(msg, list);
			bFound = TRUE;
			break;
		}
	}
	if (bFound == FALSE)
		DBGPRINT(RT_DEBUG_ERROR,
				("Cannot unlink msg %p. The msg is not in list\n", msg));
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
}


static struct cmd_msg *_AndesDequeueCmdMsg(DL_LIST *list)
{
	struct cmd_msg *msg;

	msg = DlListFirst(list, struct cmd_msg, list);

	_AndesUnlinkCmdMsg(msg, list);

	return msg;
}


struct cmd_msg *AndesDequeueCmdMsg(struct MCU_CTRL *ctl, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg;
	NDIS_SPIN_LOCK *lock;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	msg = _AndesDequeueCmdMsg(list);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);

	return msg;
}


#ifdef RTMP_SDIO_SUPPORT
VOID SdioKickOutCmdMsgComplete(struct cmd_msg *msg)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)msg->priv;
	struct MCU_CTRL *ctl = &pAd->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if (!msg->need_wait) {
		AndesUnlinkCmdMsg(msg, &ctl->kickq);
		DBGPRINT(RT_DEBUG_INFO, ("%s: msg state = %d\n", __FUNCTION__, msg->state));
		AndesQueueTailCmdMsg(&ctl->tx_doneq, msg, tx_done);
	}
	else
	{
		msg->state = wait_ack;
	}

	AndesBhSchedule(pAd);
}
#endif

#ifdef RTMP_PCI_SUPPORT
VOID PciKickOutCmdMsgComplete(PNDIS_PACKET net_pkt)
{
	struct cmd_msg *msg =CMD_MSG_CB(net_pkt)->msg;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)msg->priv;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if (!msg->need_wait) {
		AndesUnlinkCmdMsg(msg, &ctl->kickq);
		DBGPRINT(RT_DEBUG_INFO, ("%s: msg state = %d\n", __FUNCTION__, msg->state));
		AndesQueueTailCmdMsg(&ctl->tx_doneq, msg, tx_done);
	} else {
		if (msg->state != tx_done)
			msg->state = wait_ack;
	}

	AndesBhSchedule(ad);
}
#endif /* RTMP_PCI_SUPPORT */


#ifdef RTMP_USB_SUPPORT
static VOID UsbRxCmdMsgComplete(PURB urb)
{
	PNDIS_PACKET net_pkt = (PNDIS_PACKET)RTMP_OS_USB_CONTEXT_GET(urb);
	struct cmd_msg *msg = CMD_MSG_CB(net_pkt)->msg;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)msg->priv;
	POS_COOKIE pObj = (POS_COOKIE)ad->OS_Cookie;
	RTMP_CHIP_CAP *pChipCap = &ad->chipCap;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	enum cmd_msg_state state;
	unsigned long flags;
	int ret = 0;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	AndesUnlinkCmdMsg(msg, &ctl->rxq);

	OS_PKT_TAIL_BUF_EXTEND(net_pkt, RTMP_USB_URB_LEN_GET(urb));

	if (RTMP_USB_URB_STATUS_GET(urb) == 0) {
		state = rx_done;
	} else {
		state = rx_receive_fail;
		AndesIncErrorCount(ctl, error_rx_receive_fail);
		DBGPRINT(RT_DEBUG_ERROR, ("receive cmd msg fail(%d)\n", RTMP_USB_URB_STATUS_GET(urb)));
	}

	RTMP_SPIN_LOCK_IRQSAVE(&ctl->rx_doneq_lock, &flags);
	_AndesQueueTailCmdMsg(&ctl->rx_doneq, msg, state);
	RTMP_SPIN_UNLOCK_IRQRESTORE(&ctl->rx_doneq_lock, &flags);

	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		msg = AndesAllocCmdMsg(ad, 512);

		if (!msg) {
			return;
		}

		net_pkt = msg->net_pkt;

		RTUSB_FILL_BULK_URB(msg->urb, pObj->pUsb_Dev,
							usb_rcvbulkpipe(pObj->pUsb_Dev, pChipCap->CommandRspBulkInAddr),
							GET_OS_PKT_DATAPTR(net_pkt), 512, UsbRxCmdMsgComplete, net_pkt);

		AndesQueueTailCmdMsg(&ctl->rxq, msg, rx_start);

		ret = RTUSB_SUBMIT_URB(msg->urb);

		if (ret) {
			AndesUnlinkCmdMsg(msg, &ctl->rxq);
			AndesIncErrorCount(ctl, error_rx_receive_fail);
			DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __FUNCTION__, ret));
			AndesQueueTailCmdMsg(&ctl->rx_doneq, msg, rx_receive_fail);
		}

	}

	AndesBhSchedule(ad);
}


INT32 UsbRxCmdMsgSubmit(RTMP_ADAPTER *ad)
{
	RTMP_CHIP_CAP *pChipCap = &ad->chipCap;
	POS_COOKIE pObj = (POS_COOKIE)ad->OS_Cookie;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg = NULL;
	PNDIS_PACKET net_pkt = NULL;
	int ret = 0;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return ret;

	msg =  AndesAllocCmdMsg(ad, 512);

	if (!msg) {
		ret = NDIS_STATUS_RESOURCES;
		return ret;
	}

	net_pkt = msg->net_pkt;

	RTUSB_FILL_BULK_URB(msg->urb, pObj->pUsb_Dev,
						usb_rcvbulkpipe(pObj->pUsb_Dev, pChipCap->CommandRspBulkInAddr),
						GET_OS_PKT_DATAPTR(net_pkt), 512, UsbRxCmdMsgComplete, net_pkt);

	AndesQueueTailCmdMsg(&ctl->rxq, msg, rx_start);

	ret = RTUSB_SUBMIT_URB(msg->urb);

	if (ret) {
		AndesUnlinkCmdMsg(msg, &ctl->rxq);
		AndesIncErrorCount(ctl, error_rx_receive_fail);
		DBGPRINT(RT_DEBUG_ERROR, ("%s:submit urb fail(%d)\n", __FUNCTION__, ret));
		AndesQueueTailCmdMsg(&ctl->rx_doneq, msg, rx_receive_fail);
	}

	return ret;
}


INT32 UsbRxCmdMsgsReceive(RTMP_ADAPTER *ad)
{
	int ret = 0;
	int i;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	for (i = 0; (i < 1) && (AndesQueueLen(ctl, &ctl->rxq) < 1); i++) {
		ret = UsbRxCmdMsgSubmit(ad);
		if (ret)
			break;
	}

	return ret;
}


VOID UsbKickOutCmdMsgComplete(PURB urb)
{
	PNDIS_PACKET net_pkt = (PNDIS_PACKET)RTMP_OS_USB_CONTEXT_GET(urb);
	struct cmd_msg *msg = CMD_MSG_CB(net_pkt)->msg;
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)msg->priv;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	if (RTMP_USB_URB_STATUS_GET(urb) == -ENOENT
		|| RTMP_USB_URB_STATUS_GET(urb) == -ECONNRESET) {
		/* URB status is invalid and we should not access this URB anymore */
		DBGPRINT(RT_DEBUG_ERROR,
			("%s(): Invalid Urb status %d\n",
			__func__, RTMP_USB_URB_STATUS_GET(urb)));
			return;
	}

	OS_CLEAR_BIT(MCU_TX_IN_PROGRESS, &ctl->flags);
	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if (msg->state != wait_cmd_out_and_ack && msg->state != wait_cmd_out) {
		/* If the msg does not come from source state,
		* that means this msg was running into other states already.
		* Do nothing here!
		*/
		DBGPRINT(RT_DEBUG_ERROR,
			("%s(): Invalid TX status %d %lX %d\n",
			__func__, msg->state,
			(unsigned long int)virt_to_phys(msg), RTMP_USB_URB_STATUS_GET(msg->urb)));
		AndesBhSchedule(ad);
		return;
	}

	if (RTMP_USB_URB_STATUS_GET(urb) == 0) {
		if (!msg->need_wait) {
			AndesUnlinkCmdMsg(msg, &ctl->kickq);
			AndesQueueTailCmdMsg(&ctl->tx_doneq, msg, tx_done);
		} else {
			msg->state = wait_ack;
		}
	} else {
		if (!msg->need_wait) {
			AndesUnlinkCmdMsg(msg, &ctl->kickq);
			AndesQueueTailCmdMsg(&ctl->tx_doneq, msg, tx_kickout_fail);
			AndesIncErrorCount(ctl, error_tx_kickout_fail);
		} else {
			AndesUnlinkCmdMsg(msg, &ctl->ackq);
			msg->state = tx_kickout_fail;
			AndesIncErrorCount(ctl, error_tx_kickout_fail);
			RTMP_OS_COMPLETE(&msg->ack_done);
		}

		DBGPRINT(RT_DEBUG_ERROR, ("kick out cmd msg fail(%d)\n", RTMP_USB_URB_STATUS_GET(urb)));
	}

	AndesBhSchedule(ad);
}
#endif


VOID AndesRxProcessCmdMsg(RTMP_ADAPTER *ad, struct cmd_msg *rx_msg)
{
    RX_BLK RxBlk;

	memset(&RxBlk, 0x00, sizeof(RX_BLK));

#ifdef MT_MAC
	if (ad->chipCap.hif_type == HIF_MT)
	{
		/*rx_hw_hdr_len = */parse_rx_packet_type(ad, &RxBlk, rx_msg->net_pkt);
	}
#endif /* MT_MAC */

#ifdef RLT_MAC
	if (ad->chipCap.hif_type == HIF_RLT)
	{
		AndesRltRxProcessCmdMsg(ad, rx_msg);
	}
#endif /* RLT_MAC */

}


VOID AndesCmdMsgBh(unsigned long param)
{
	RTMP_ADAPTER *ad = (RTMP_ADAPTER *)param;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	struct cmd_msg *msg = NULL;

	while ((msg = AndesDequeueCmdMsg(ctl, &ctl->rx_doneq))) {
		switch (msg->state) {
			case rx_done:
				AndesRxProcessCmdMsg(ad, msg);
				AndesFreeCmdMsg(msg);
				break;
			case rx_receive_fail:
				AndesFreeCmdMsg(msg);
				break;
			default:				
				DBGPRINT(RT_DEBUG_ERROR, ("unknow msg state(%d)\n", msg->state));
				AndesFreeCmdMsg(msg);
				break;
		}
	}

	while ((msg = AndesDequeueCmdMsg(ctl, &ctl->tx_doneq))) {
		switch (msg->state) {
			case tx_done:
			case tx_kickout_fail:
			case tx_timeout_fail:
				AndesFreeCmdMsg(msg);
				break;
			default:				
				DBGPRINT(RT_DEBUG_ERROR, ("unknow msg state(%d)\n", msg->state));
				AndesFreeCmdMsg(msg);
				break;
		}
	}

	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
		AndesBhSchedule(ad);
#ifdef RTMP_USB_SUPPORT
		UsbRxCmdMsgsReceive(ad);
#endif
	}
}


#ifdef RTMP_USB_SUPPORT
VOID AndesUsbUnlinkUrb(RTMP_ADAPTER *ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	NDIS_SPIN_LOCK *lock;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	DlListForEachSafe(msg, msg_tmp, list, struct cmd_msg, list) {
		RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
		if ((msg->state == wait_cmd_out_and_ack) || (msg->state == wait_cmd_out) ||
						(msg->state == tx_start) || (msg->state == rx_start) ||
						(msg->state == tx_retransmit))
			RTUSB_UNLINK_URB(msg->urb);
		RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	}
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
}
#endif


VOID AndesBhSchedule(RTMP_ADAPTER *ad)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags))
		return;

	if (((AndesQueueLen(ctl, &ctl->rx_doneq) > 0)
							|| (AndesQueueLen(ctl, &ctl->tx_doneq) > 0))
							&& OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
#ifndef WORKQUEUE_BH
		RTMP_NET_TASK_DATA_ASSIGN(&ctl->cmd_msg_task, (unsigned long)(ad));
		RTMP_OS_TASKLET_SCHE(&ctl->cmd_msg_task);
#else
		tasklet_hi_schedule(&ctl->cmd_msg_task);
#endif
	}
}


VOID AndesCleanupCmdMsg(RTMP_ADAPTER *ad, DL_LIST *list)
{
	unsigned long flags;
	struct cmd_msg *msg, *msg_tmp;
	NDIS_SPIN_LOCK *lock;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;

	lock = AndesGetSpinLock(ctl, list);

	RTMP_SPIN_LOCK_IRQSAVE(lock, &flags);
	DlListForEachSafe(msg, msg_tmp, list, struct cmd_msg, list) {
		_AndesUnlinkCmdMsg(msg, list);
		AndesFreeCmdMsg(msg);
	}
	DlListInit(list);
	RTMP_SPIN_UNLOCK_IRQRESTORE(lock, &flags);
}


#ifdef RTMP_PCI_SUPPORT
static VOID AndesCtrlPciInit(RTMP_ADAPTER *ad)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	
	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	ctl->cmd_seq = 0;
#ifndef WORKQUEUE_BH
	RTMP_OS_TASKLET_INIT(ad, &ctl->cmd_msg_task, AndesCmdMsgBh, (unsigned long)ad);
#else
	tasklet_init(&ctl->cmd_msg_task, AndesCmdMsgBh, (unsigned long)ad);
#endif
	NdisAllocateSpinLock(ad, &ctl->txq_lock);
	AndesQueueInit(ctl, &ctl->txq);
	NdisAllocateSpinLock(ad, &ctl->rxq_lock);
	AndesQueueInit(ctl, &ctl->rxq);
	NdisAllocateSpinLock(ad, &ctl->ackq_lock);
	AndesQueueInit(ctl, &ctl->ackq);
	NdisAllocateSpinLock(ad, &ctl->kickq_lock);
	AndesQueueInit(ctl, &ctl->kickq);
	NdisAllocateSpinLock(ad, &ctl->tx_doneq_lock);
	AndesQueueInit(ctl, &ctl->tx_doneq);
	NdisAllocateSpinLock(ad, &ctl->rx_doneq_lock);
	AndesQueueInit(ctl, &ctl->rx_doneq);
	ctl->tx_kickout_fail_count = 0;
	ctl->tx_timeout_fail_count = 0;
	ctl->rx_receive_fail_count = 0;
	ctl->alloc_cmd_msg = 0;
	ctl->free_cmd_msg = 0;
	OS_SET_BIT(MCU_INIT, &ctl->flags);
	ctl->ad = ad;
}
#endif


#ifdef RTMP_USB_SUPPORT
static VOID AndesCtrlUsbInit(RTMP_ADAPTER *ad)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	
	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	ctl->cmd_seq = 0;
	RTMP_OS_TASKLET_INIT(ad, &ctl->cmd_msg_task, AndesCmdMsgBh, (unsigned long)ad);
	NdisAllocateSpinLock(ad, &ctl->txq_lock);
	AndesQueueInit(ctl, &ctl->txq);
	NdisAllocateSpinLock(ad, &ctl->rxq_lock);
	AndesQueueInit(ctl, &ctl->rxq);
	NdisAllocateSpinLock(ad, &ctl->ackq_lock);
	AndesQueueInit(ctl, &ctl->ackq);
	NdisAllocateSpinLock(ad, &ctl->kickq_lock);
	AndesQueueInit(ctl, &ctl->kickq);
	NdisAllocateSpinLock(ad, &ctl->tx_doneq_lock);
	AndesQueueInit(ctl, &ctl->tx_doneq);
	NdisAllocateSpinLock(ad, &ctl->rx_doneq_lock);
	AndesQueueInit(ctl, &ctl->rx_doneq);
	ctl->tx_kickout_fail_count = 0;
	ctl->tx_timeout_fail_count = 0;
	ctl->rx_receive_fail_count = 0;
	ctl->alloc_cmd_msg = 0;
	ctl->free_cmd_msg = 0;
	ctl->ad = ad;
	OS_SET_BIT(MCU_INIT, &ctl->flags);
	UsbRxCmdMsgsReceive(ad);
}
#endif


#ifdef RTMP_SDIO_SUPPORT
static VOID AndesCtrlSdioInit(RTMP_ADAPTER *pAd)
{
	struct MCU_CTRL *ctl = &pAd->MCUCtrl;
	int ret = 0;

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	ctl->cmd_seq = 0;
	RTMP_OS_TASKLET_INIT(pAd, &ctl->cmd_msg_task, AndesCmdMsgBh, (unsigned long)pAd);
	NdisAllocateSpinLock(pAd, &ctl->txq_lock);
	AndesQueueInit(ctl, &ctl->txq);
	NdisAllocateSpinLock(pAd, &ctl->rxq_lock);
	AndesQueueInit(ctl, &ctl->rxq);
	NdisAllocateSpinLock(pAd, &ctl->ackq_lock);
	AndesQueueInit(ctl, &ctl->ackq);
	NdisAllocateSpinLock(pAd, &ctl->kickq_lock);
	AndesQueueInit(ctl, &ctl->kickq);
	NdisAllocateSpinLock(pAd, &ctl->tx_doneq_lock);
	AndesQueueInit(ctl, &ctl->tx_doneq);
	NdisAllocateSpinLock(pAd, &ctl->rx_doneq_lock);
	AndesQueueInit(ctl, &ctl->rx_doneq);
	NdisAllocateSpinLock(pAd, &ctl->txq_sdio_lock);
	AndesQueueInit(ctl, &ctl->txq_sdio);
	ctl->tx_kickout_fail_count = 0;
	ctl->tx_timeout_fail_count = 0;
	ctl->rx_receive_fail_count = 0;
	ctl->alloc_cmd_msg = 0;
	ctl->free_cmd_msg = 0;
	ctl->ad = pAd;
	OS_SET_BIT(MCU_INIT, &ctl->flags);
}
#endif


VOID AndesCtrlInit(RTMP_ADAPTER *pAd)
{
	struct MCU_CTRL *ctl = &pAd->MCUCtrl;

	if (!OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
#ifdef RTMP_PCI_SUPPORT
		AndesCtrlPciInit(pAd);
#endif

#ifdef RTMP_USB_SUPPORT
		AndesCtrlUsbInit(pAd);
#endif

#ifdef RTMP_SDIO_SUPPORT
		AndesCtrlSdioInit(pAd);
#endif
	}

	ctl->power_on = FALSE;
	ctl->dpd_on = FALSE;
	ctl->RxStream0 = 0;
	ctl->RxStream1 = 0;
}


#ifdef RTMP_USB_SUPPORT
static VOID AndesCtrlUsbExit(RTMP_ADAPTER *ad)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	int ret = 0;
	int count = 0;

	if(ret != 0)
		DBGPRINT(RT_DEBUG_ERROR, ("%s:(%d) RTMP_SEM_EVENT_WAIT failed!\n",__FUNCTION__,ret));
	
	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	OS_CLEAR_BIT(MCU_INIT, &ctl->flags);
	while (OS_TEST_BIT(MCU_TX_IN_PROGRESS, &ctl->flags)) {
		DBGPRINT(RT_DEBUG_TRACE, ("cmd_tx count = %d\n", count));
		count++;
		if (count > 1000)
		break;
		RtmpOsMsDelay(2);
	}
	AndesUsbUnlinkUrb(ad, &ctl->txq);
	AndesUsbUnlinkUrb(ad, &ctl->kickq);
	AndesUsbUnlinkUrb(ad, &ctl->ackq);
	AndesUsbUnlinkUrb(ad, &ctl->rxq);
	RTMP_OS_TASKLET_KILL(&ctl->cmd_msg_task);
	AndesCleanupCmdMsg(ad, &ctl->txq);
	NdisFreeSpinLock(&ctl->txq_lock);
	AndesCleanupCmdMsg(ad, &ctl->ackq);
	NdisFreeSpinLock(&ctl->ackq_lock);
	AndesCleanupCmdMsg(ad, &ctl->rxq);
	NdisFreeSpinLock(&ctl->rxq_lock);
	AndesCleanupCmdMsg(ad, &ctl->kickq);
	NdisFreeSpinLock(&ctl->kickq_lock);
	AndesCleanupCmdMsg(ad, &ctl->tx_doneq);
	NdisFreeSpinLock(&ctl->tx_doneq_lock);
	AndesCleanupCmdMsg(ad, &ctl->rx_doneq);
	NdisFreeSpinLock(&ctl->rx_doneq_lock);
	DBGPRINT(RT_DEBUG_OFF, ("tx_kickout_fail_count = %ld\n", ctl->tx_kickout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("tx_timeout_fail_count = %ld\n", ctl->tx_timeout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("rx_receive_fail_count = %ld\n", ctl->rx_receive_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("alloc_cmd_msg = %ld\n", ctl->alloc_cmd_msg));
	DBGPRINT(RT_DEBUG_OFF, ("free_cmd_msg = %ld\n", ctl->free_cmd_msg));
}
#endif


#ifdef RTMP_PCI_SUPPORT
static VOID AndesCtrlPciExit(RTMP_ADAPTER *ad)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	
	RTMP_CLEAR_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	OS_CLEAR_BIT(MCU_INIT, &ctl->flags);
	RTMP_OS_TASKLET_KILL(&ctl->cmd_msg_task);
	AndesCleanupCmdMsg(ad, &ctl->txq);
	NdisFreeSpinLock(&ctl->txq_lock);
	AndesCleanupCmdMsg(ad, &ctl->ackq);
	NdisFreeSpinLock(&ctl->ackq_lock);
	AndesCleanupCmdMsg(ad, &ctl->rxq);
	NdisFreeSpinLock(&ctl->rxq_lock);
	AndesCleanupCmdMsg(ad, &ctl->kickq);
	NdisFreeSpinLock(&ctl->kickq_lock);
	AndesCleanupCmdMsg(ad, &ctl->tx_doneq);
	NdisFreeSpinLock(&ctl->tx_doneq_lock);
	AndesCleanupCmdMsg(ad, &ctl->rx_doneq);
	NdisFreeSpinLock(&ctl->rx_doneq_lock);
	DBGPRINT(RT_DEBUG_OFF, ("tx_kickout_fail_count = %ld\n", ctl->tx_kickout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("tx_timeout_fail_count = %ld\n", ctl->tx_timeout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("rx_receive_fail_count = %ld\n", ctl->rx_receive_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("alloc_cmd_msg = %ld\n", ctl->alloc_cmd_msg));
	DBGPRINT(RT_DEBUG_OFF, ("free_cmd_msg = %ld\n", ctl->free_cmd_msg));
}
#endif


#ifdef RTMP_SDIO_SUPPORT
static VOID AndesCtrlSdioExit(RTMP_ADAPTER *pAd)
{
	struct MCU_CTRL *ctl = &pAd->MCUCtrl;
	unsigned long flags;
	INT32 Ret;

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
	OS_CLEAR_BIT(MCU_INIT, &ctl->flags);
	RTMP_OS_TASKLET_KILL(&ctl->cmd_msg_task);
	AndesCleanupCmdMsg(pAd, &ctl->txq);
	NdisFreeSpinLock(&ctl->txq_lock);
	AndesCleanupCmdMsg(pAd, &ctl->ackq);
	NdisFreeSpinLock(&ctl->ackq_lock);
	AndesCleanupCmdMsg(pAd, &ctl->rxq);
	NdisFreeSpinLock(&ctl->rxq_lock);
	AndesCleanupCmdMsg(pAd, &ctl->kickq);
	NdisFreeSpinLock(&ctl->kickq_lock);
	AndesCleanupCmdMsg(pAd, &ctl->tx_doneq);
	NdisFreeSpinLock(&ctl->tx_doneq_lock);
	AndesCleanupCmdMsg(pAd, &ctl->rx_doneq);
	NdisFreeSpinLock(&ctl->rx_doneq_lock);
	AndesCleanupCmdMsg(pAd, &ctl->txq_sdio);
	NdisFreeSpinLock(&ctl->txq_sdio_lock);
	DBGPRINT(RT_DEBUG_OFF, ("tx_kickout_fail_count = %ld\n", ctl->tx_kickout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("tx_timeout_fail_count = %ld\n", ctl->tx_timeout_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("rx_receive_fail_count = %ld\n", ctl->rx_receive_fail_count));
	DBGPRINT(RT_DEBUG_OFF, ("alloc_cmd_msg = %ld\n", ctl->alloc_cmd_msg));
	DBGPRINT(RT_DEBUG_OFF, ("free_cmd_msg = %ld\n", ctl->free_cmd_msg));
}
#endif

VOID AndesCtrlExit(RTMP_ADAPTER *pAd)
{
	struct MCU_CTRL *ctl = &pAd->MCUCtrl;

	if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
#ifdef RTMP_PCI_SUPPORT
		AndesCtrlPciExit(pAd);
#endif

#ifdef RTMP_USB_SUPPORT
		AndesCtrlUsbExit(pAd);
#endif

#ifdef RTMP_SDIO_SUPPORT
		AndesCtrlSdioExit(pAd);
#endif
	}

	ctl->Stage = FW_NO_INIT;
	ctl->power_on = FALSE;
	ctl->dpd_on = FALSE;
}


static INT32 AndesDequeueAndKickOutCmdMsgs(RTMP_ADAPTER *ad)
{
	struct cmd_msg *msg = NULL;
	PNDIS_PACKET net_pkt = NULL;
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	int ret = NDIS_STATUS_SUCCESS;

	while ((msg = AndesDequeueCmdMsg(ctl, &ctl->txq)) != NULL) {
		if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {
			if (!msg->need_wait)
				AndesForceFreeCmdMsg(msg);
			continue;
		}

		if (AndesQueueLen(ctl, &ctl->ackq) > 0) {
			AndesQueueHeadCmdMsg(&ctl->txq, msg, msg->state);
			ret = NDIS_STATUS_FAILURE;
			continue;
		}

		net_pkt = msg->net_pkt;

		if (msg->state != tx_retransmit) {
			if (msg->need_wait)
				msg->seq = AndesGetCmdMsgSeq(ad);
			else
				msg->seq = 0;

            if (ad->chipOps.andes_fill_cmd_header != NULL)
                ad->chipOps.andes_fill_cmd_header(msg, net_pkt);
		}

#ifdef RTMP_USB_SUPPORT
		if (ad->chipOps.usb_kick_out_cmd_msg != NULL)
			ret = ad->chipOps.usb_kick_out_cmd_msg(ad, msg);
#endif

#ifdef RTMP_PCI_SUPPORT
		if (ad->chipOps.pci_kick_out_cmd_msg != NULL)
			ret = ad->chipOps.pci_kick_out_cmd_msg(ad, msg);
#endif

#ifdef RTMP_SDIO_SUPPORT
		if (ad->chipOps.sdio_kick_out_cmd_msg != NULL)
			ret = ad->chipOps.sdio_kick_out_cmd_msg(ad, msg);
#endif

		if (ret) {
			DBGPRINT(RT_DEBUG_ERROR, ("kick out msg fail\n"));
			break;
		}
	}

	AndesBhSchedule(ad);

	return ret;
}


#ifdef RTMP_SDIO_SUPPORT
INT32 AndesWaitForSdioCompleteTimeout(struct cmd_msg *msg, long timeout)
{
	int ret = 0;
	long expire = timeout ? RTMPMsecsToJiffies(timeout) : RTMPMsecsToJiffies(CMD_MSG_TIMEOUT);

	ret = RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&msg->tx_sdio_done, expire);

	return ret;
}
#endif

static INT32 AndesWaitForCompleteTimeout(struct cmd_msg *msg, long timeout)
{
	int ret = 0;
	long expire = timeout ? RTMPMsecsToJiffies(timeout) : RTMPMsecsToJiffies(CMD_MSG_TIMEOUT);

	ret = RTMP_OS_WAIT_FOR_COMPLETION_TIMEOUT(&msg->ack_done, expire);

	return ret;
}


INT32 AndesSendCmdMsg(PRTMP_ADAPTER ad, struct cmd_msg *msg)
{
	struct MCU_CTRL *ctl = &ad->MCUCtrl;
	int ret = 0;
	BOOLEAN need_wait = msg->need_wait;

	if(in_interrupt() && need_wait)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("BUG: %s is called from invalid context\n", __FUNCTION__));
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Command type = %x, Extension command type = %x\n", __FUNCTION__, msg->cmd_type, msg->ext_cmd_type));
		AndesForceFreeCmdMsg(msg);
		return NDIS_STATUS_FAILURE;
	}

	if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST)
				|| RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND)) {

		if (!RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD))
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Could not send in band command due to diable fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD\n", __FUNCTION__));
		else if (RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_NIC_NOT_EXIST))
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Could not send in band command due to fRTMP_ADAPTER_NIC_NOT_EXIST\n", __FUNCTION__));
		else if (RTMP_TEST_FLAG(ad, fRTMP_ADAPTER_SUSPEND))
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Could not send in band command due to fRTMP_ADAPTER_SUSPEND\n", __FUNCTION__));

		AndesForceFreeCmdMsg(msg);

		return NDIS_STATUS_FAILURE;
	}

	AndesQueueTailCmdMsg(&ctl->txq, msg, tx_start);

retransmit:
	AndesDequeueAndKickOutCmdMsgs(ad);

	/* Wait for response */
	if (need_wait) {
		enum cmd_msg_state state;
		if (!AndesWaitForCompleteTimeout(msg, msg->timeout)) {
			ret = NDIS_STATUS_FAILURE;
#if 0
			if(DlListEmpty(&ad->SdioWorkTaskList))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("&ad->SdioWorkTaskList is emplty\n"));
			}
#endif
			DBGPRINT(RT_DEBUG_ERROR, ("command (%x), ext_cmd_type (%x), timeout(%dms)\n", msg->cmd_type, msg->ext_cmd_type, CMD_MSG_TIMEOUT));
			DBGPRINT(RT_DEBUG_ERROR, ("txq qlen = %d\n", AndesQueueLen(ctl, &ctl->txq)));
			DBGPRINT(RT_DEBUG_ERROR, ("rxq qlen = %d\n", AndesQueueLen(ctl, &ctl->rxq)));
			DBGPRINT(RT_DEBUG_ERROR, ("kickq qlen = %d\n", AndesQueueLen(ctl, &ctl->kickq)));
			DBGPRINT(RT_DEBUG_ERROR, ("ackq qlen = %d\n", AndesQueueLen(ctl, &ctl->ackq)));
			DBGPRINT(RT_DEBUG_ERROR, ("tx_doneq.qlen = %d\n", AndesQueueLen(ctl, &ctl->tx_doneq)));
			DBGPRINT(RT_DEBUG_ERROR, ("rx_done qlen = %d\n", AndesQueueLen(ctl, &ctl->rx_doneq)));
			if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
				if (msg->state == wait_cmd_out_and_ack) {
#ifndef RTMP_USB_SUPPORT
					AndesUnlinkCmdMsg(msg, &ctl->ackq);
#else
					RTUSB_UNLINK_URB(msg->urb);
#endif
				} else if (msg->state == wait_ack) {
					AndesUnlinkCmdMsg(msg, &ctl->ackq);
				}
			}

			DBGPRINT(RT_DEBUG_ERROR, ("%s: msg state = %d\n", __FUNCTION__, msg->state));
			AndesIncErrorCount(ctl, error_tx_timeout_fail);
			state = tx_timeout_fail;
			if (msg->retransmit_times > 0)
				msg->retransmit_times--;
#ifdef MT_PS
			else
			{
				/* timeout process */
				if ((msg->cmd_type == EXT_CID) &&
					(msg->ext_cmd_type == EXT_CMD_PS_RETRIEVE_START))
				{
					MtPsSendToken(ad, msg->wcid);
				}
			}
#endif /* MT_PS */
			DBGPRINT(RT_DEBUG_ERROR, ("msg->retransmit_times = %d\n", msg->retransmit_times));
		} else {
			if (msg->state == tx_kickout_fail) {
				state = tx_kickout_fail;
				msg->retransmit_times--;
			} else {
				state = tx_done;
				msg->retransmit_times = 0;
			}
		}

		if (OS_TEST_BIT(MCU_INIT, &ctl->flags)) {
			if (msg->need_retransmit && (msg->retransmit_times > 0)) {
				RTMP_OS_EXIT_COMPLETION(&msg->ack_done);
				RTMP_OS_INIT_COMPLETION(&msg->ack_done);
				state = tx_retransmit;
				AndesQueueHeadCmdMsg(&ctl->txq, msg, state);
				goto retransmit;
			} else {
				DBGPRINT(RT_DEBUG_INFO, ("%s: msg state = %d\n", __FUNCTION__, state));
				AndesQueueTailCmdMsg(&ctl->tx_doneq, msg, state);
			}
		} else {
			AndesFreeCmdMsg(msg);
		}
	}

	return ret;
}

