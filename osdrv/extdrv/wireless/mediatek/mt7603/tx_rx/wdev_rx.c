/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:

	Abstract:

	Revision History:
	Who 		When			What
	--------	----------		----------------------------------------------
*/


#include "rt_config.h"

#ifdef CONFIG_HOTSPOT
extern BOOLEAN hotspot_rx_handler(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry, RX_BLK *pRxBlk);
#endif /* CONFIG_HOTSPOT */

// TODO: shiang-usw, temporary put this function here, should remove to other place or re-write!
VOID Update_Rssi_Sample(
	IN RTMP_ADAPTER *pAd,
	IN RSSI_SAMPLE *pRssi,
	IN struct rx_signal_info *signal,
	IN USHORT phy_mode,
	IN USHORT bw)
{
	BOOLEAN bInitial = FALSE;
	UCHAR ant_idx;
	INT ant_max = 3;

	if (!(pRssi->AvgRssi[0] | pRssi->AvgRssiX8[0] | pRssi->LastRssi[0]))
		bInitial = TRUE;

	// TODO: shiang-usw, shall we check this here to reduce the for loop count?
	if (ant_max > pAd->Antenna.field.RxPath)
		ant_max = pAd->Antenna.field.RxPath;

	for (ant_idx = 0; ant_idx < 3; ant_idx++)
	{
		if (signal->raw_snr[ant_idx] != 0 && phy_mode != MODE_CCK)
		{
			pRssi->LastSnr[ant_idx] = ConvertToSnr(pAd, signal->raw_snr[ant_idx]);
			if (bInitial)
			{
				pRssi->AvgSnrX8[ant_idx] = pRssi->LastSnr[ant_idx] << 3;
				pRssi->AvgSnr[ant_idx] = pRssi->LastSnr[ant_idx];
			}
			else
				pRssi->AvgSnrX8[ant_idx] = (pRssi->AvgSnrX8[ant_idx] - pRssi->AvgSnr[ant_idx]) + pRssi->LastSnr[ant_idx];

			pRssi->AvgSnr[ant_idx] = (CHAR)(pRssi->AvgSnrX8[ant_idx] >> 3);
		}

		if (signal->raw_rssi[ant_idx] != 0)
		{
			pRssi->LastRssi[ant_idx] = ConvertToRssi(pAd, (struct raw_rssi_info *)(&signal->raw_rssi[0]), ant_idx);
#ifdef MT76x2
			if (IS_MT76x2(pAd)) {
				if (phy_mode == MODE_CCK)
					pRssi->LastRssi[ant_idx] -= 2;
				else if ((bw == BW_80) && (pRssi->LastRssi[ant_idx] < -75))
						pRssi->LastRssi[ant_idx] = (-92 + pRssi->LastSnr[ant_idx]);
			}
#endif /* MT76x2 */

			if (bInitial)
			{
				pRssi->AvgRssiX8[ant_idx] = pRssi->LastRssi[ant_idx] << 3;
				pRssi->AvgRssi[ant_idx] = pRssi->LastRssi[ant_idx];
			}
			else
				pRssi->AvgRssiX8[ant_idx] = (pRssi->AvgRssiX8[ant_idx] - pRssi->AvgRssi[ant_idx]) + pRssi->LastRssi[ant_idx];

			pRssi->AvgRssi[ant_idx] = (CHAR)(pRssi->AvgRssiX8[ant_idx] >> 3);
		}
	}
}

/* Compare CCMP PN value and return whether the pkt in rxblk is allowed.
 *
 * @pRxBlk: the rxblk to be checked
 */
static BOOLEAN check_rx_pkt_pn_allowed(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	MAC_TABLE_ENTRY *pEntry = NULL;

	if (unlikely(pRxBlk->wcid >= MAX_LEN_OF_MAC_TABLE)) {
		DBGPRINT(RT_DEBUG_OFF, ("invalid pRxBlk->wcid %u, bypass\n", pRxBlk->wcid));
		return TRUE;
	}

	pEntry = &pAd->MacTab.Content[pRxBlk->wcid];

	if (!pEntry->wdev) {
		DBGPRINT(RT_DEBUG_OFF, ("null w %u pEntry->wdev, bypass\n", pRxBlk->wcid));
		return TRUE;
	}

	if (pRxBlk->pRxInfo->Mcast || pRxBlk->pRxInfo->Bcast) {
		if (IS_CCMP_CIPHER(pRxBlk) || IS_TKIP_CIPHER(pRxBlk)) {
			UCHAR kid = pRxBlk->key_idx;
			if (unlikely(kid >= ARRAY_SIZE(pEntry->rx_ccmp_pn_bmc))) {
				DBGPRINT(RT_DEBUG_ERROR, ("invalid key id %u\n", kid));
				return TRUE;
			}

			if (likely(pRxBlk->ccmp_pn > pEntry->rx_ccmp_pn_bmc[kid])) {
				/* PN-0 is NOT allowed from now on */
				pEntry->rx_ccmp_pn_bmc_zero[kid] = FALSE;
				pEntry->rx_ccmp_pn_bmc[kid] = pRxBlk->ccmp_pn;
				DBGPRINT(RT_DEBUG_TRACE, ("wcid %d update rx bmc[%u] PN %llu\n",
					 pRxBlk->wcid, kid, pRxBlk->ccmp_pn));
				return TRUE;
			}

			/* Some APs initialize PN to 0, allow it only if first rx pkt */
			if (unlikely(pRxBlk->ccmp_pn == 0)) {
				if (pEntry->rx_ccmp_pn_bmc_zero[kid]) {
					pEntry->rx_ccmp_pn_bmc_zero[kid] = FALSE;
					return TRUE;
				}
			}

			DBGPRINT(RT_DEBUG_ERROR, ("wcid %d rx bmc[%u] %llu exp > %llu\n",
				 pRxBlk->wcid, kid, pRxBlk->ccmp_pn,
				 pEntry->rx_ccmp_pn_bmc[kid]));
			return FALSE;
		}
	}

	/* skip unicast check now */
	return TRUE;
}

#ifdef DOT11_N_SUPPORT
UINT deaggregate_AMSDU_announce(
	IN RTMP_ADAPTER *pAd,
	PNDIS_PACKET pPacket,
	IN UCHAR *pData,
	IN ULONG DataSize,
	IN UCHAR OpMode)
{
	USHORT PayloadSize;
	USHORT SubFrameSize;
	HEADER_802_3 *pAMSDUsubheader;
	UINT nMSDU;
	UCHAR Header802_3[14];
	UCHAR *pPayload, *pDA, *pSA, *pRemovedLLCSNAP, *pRA, *pTA;
	PNDIS_PACKET pClonePacket;
	struct wifi_dev *wdev;
	UCHAR wdev_idx = RTMP_GET_PACKET_WDEV(pPacket);
	UCHAR VLAN_Size;
	USHORT VLAN_VID = 0, VLAN_Priority = 0;
	PHEADER_802_11 pHeader;

	ASSERT(wdev_idx < WDEV_NUM_MAX);
	if (wdev_idx >= WDEV_NUM_MAX)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s():invalud wdev_idx(%d)\n", __FUNCTION__, wdev_idx));
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_SUCCESS);
		return 0;
	}

	wdev = pAd->wdev_list[wdev_idx];

	/* only MBssid support VLAN.*/
	VLAN_Size = (wdev->VLAN_VID != 0) ? LENGTH_802_1Q : 0;
	nMSDU = 0;

	/* Parse RA & TA in a AMSDU pkt */
	/* AMSDU format:
	 *         MAC Header             +         Sub-MSDU frame             +            Sub-MSDU frame            + ..... 
	 * FC + Duration + RA + TA  +  DA + SA + payload + padding  +  DA + SA + payload + padding    + .....
	 * each DA/SA in sub-MSDU frame should be same to RA/TA in MAC Header
	 */
	pHeader = (PHEADER_802_11) GET_OS_PKT_DATAPTR(pPacket);
	pRA = (PUCHAR) (pHeader + 4);/* skip FC and Duration field*/
	pTA = pRA + MAC_ADDR_LEN;

	while (DataSize > LENGTH_802_3)
	{
		nMSDU++;

		/*hex_dump("subheader", pData, 64);*/
		pAMSDUsubheader = (PHEADER_802_3)pData;
		/*pData += LENGTH_802_3;*/
		PayloadSize = pAMSDUsubheader->Octet[1] + (pAMSDUsubheader->Octet[0]<<8);
		SubFrameSize = PayloadSize + LENGTH_802_3;

		if ((DataSize < SubFrameSize) || (PayloadSize > 1518 ))
			break;

		/*DBGPRINT(RT_DEBUG_TRACE,("%d subframe: Size = %d\n",  nMSDU, PayloadSize));*/
		/* Subframe: DA + TA + payload + padding */
		pPayload = pData + LENGTH_802_3;
		pDA = pData;
		pSA = pData + MAC_ADDR_LEN;

		if (NdisEqualMemory(SNAP_802_1H, pDA, MAC_ADDR_LEN)) {
			DBGPRINT(RT_DEBUG_ERROR,
				("%s: AMSDU ATTACK-FakeLLC, drop it as pDA=%02x:%02x:%02x:%02x:%02x:%02x, SN(%d), nMSDU(%d)\n",
				__func__, PRINT_MAC(pDA), pHeader->Sequence,nMSDU));
			break;
		}

		if ((pHeader->FC.FrDs == 1) && !(NdisEqualMemory(pDA, pRA, MAC_ADDR_LEN))) {
                        DBGPRINT(RT_DEBUG_ERROR,
                                ("%s:AMSDU ATTACK-FrDs, pRA=%02x:%02x:%02x:%02x:%02x:%02x mismatch with pDA=%02x:%02x:%02x:%02x:%02x:%02x, SN(%d), nMSDU(%d)\n",
                               __func__, PRINT_MAC(pRA), PRINT_MAC(pDA), pHeader->Sequence,nMSDU));
                        break;
                }

		if ((pHeader->FC.ToDs == 1) && !(NdisEqualMemory(pSA, pTA, MAC_ADDR_LEN))) {
                        DBGPRINT(RT_DEBUG_ERROR,
                                ("%s:AMSDU ATTACK-ToDs, pSA=%02x:%02x:%02x:%02x:%02x:%02x mismatch with pTA=%02x:%02x:%02x:%02x:%02x:%02x, SN(%d), nMSDU(%d)\n",
                               __func__, PRINT_MAC(pSA), PRINT_MAC(pTA), pHeader->Sequence,nMSDU));
                        break;
                }

		/* convert to 802.3 header*/
		CONVERT_TO_802_3(Header802_3, pDA, pSA, pPayload, PayloadSize, pRemovedLLCSNAP);

#ifdef CONFIG_STA_SUPPORT
		if ((Header802_3[12] == 0x88) && (Header802_3[13] == 0x8E)
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT)
			&& (OpMode == OPMODE_STA)
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT */
		)
		{
			MLME_QUEUE_ELEM *Elem;

			os_alloc_mem(pAd, (UCHAR **)&Elem, sizeof(MLME_QUEUE_ELEM));
			if (Elem != NULL)
			{
				memmove(Elem->Msg+(LENGTH_802_11 + LENGTH_802_1_H), pPayload, PayloadSize);
				Elem->MsgLen = LENGTH_802_11 + LENGTH_802_1_H + PayloadSize;
				REPORT_MGMT_FRAME_TO_MLME(pAd, BSSID_WCID, Elem->Msg, Elem->MsgLen, 0, 0, 0, 0, OPMODE_STA);
				os_free_mem(NULL, Elem);
			}
		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT)
		if (OpMode == OPMODE_AP)
#else
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT */
		{
			if (pRemovedLLCSNAP)
			{
				pPayload -= (LENGTH_802_3 + VLAN_Size);
				PayloadSize += (LENGTH_802_3 + VLAN_Size);
				/*NdisMoveMemory(pPayload, &Header802_3, LENGTH_802_3);*/
			}
			else
			{
				pPayload -= VLAN_Size;
				PayloadSize += VLAN_Size;
			}

			WDEV_VLAN_INFO_GET(pAd, VLAN_VID, VLAN_Priority, wdev);

			RT_VLAN_8023_HEADER_COPY(pAd, VLAN_VID, VLAN_Priority,
									Header802_3, LENGTH_802_3, pPayload,
									TPID);
		}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
	        	if (pRemovedLLCSNAP)
	        	{
	    			pPayload -= LENGTH_802_3;
	    			PayloadSize += LENGTH_802_3;
	    			NdisMoveMemory(pPayload, &Header802_3[0], LENGTH_802_3);
	        	}
		}
#endif /* CONFIG_STA_SUPPORT */

		pClonePacket = ClonePacket(wdev->if_dev, pPacket, pPayload, PayloadSize);
		if (pClonePacket)
		{
			UCHAR opmode = pAd->OpMode;

#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT)
			opmode = OpMode;
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT */
			Announce_or_Forward_802_3_Packet(pAd, pClonePacket, RTMP_GET_PACKET_WDEV(pPacket), opmode);
		}


		/* A-MSDU has padding to multiple of 4 including subframe header.*/
		/* align SubFrameSize up to multiple of 4*/
		SubFrameSize = (SubFrameSize+3)&(~0x3);


		if (SubFrameSize > 1528 || SubFrameSize < 32)
			break;

		if (DataSize > SubFrameSize)
		{
			pData += SubFrameSize;
			DataSize -= SubFrameSize;
		}
		else
		{
			/* end of A-MSDU*/
			DataSize = 0;
		}
	}

	/* finally release original rx packet*/
	RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_SUCCESS);

	return nMSDU;
}


VOID Indicate_AMSDU_Packet(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk, UCHAR wdev_idx)
{
	//UINT nMSDU;
	if (check_rx_pkt_pn_allowed(pAd, pRxBlk) == FALSE) {
		DBGPRINT(RT_DEBUG_WARN, ("%s:drop packet by PN mismatch!\n", __FUNCTION__));
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

	RTMP_UPDATE_OS_PACKET_INFO(pAd, pRxBlk, wdev_idx);
	RTMP_SET_PACKET_WDEV(pRxBlk->pRxPacket, wdev_idx);
#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT)
	{
		struct rxd_base_struc *rx_base;
		rx_base = (struct rxd_base_struc *)pRxBlk->rmac_info;
		
		if ((rx_base->rxd_1.hdr_offset == 1) && (rx_base->rxd_1.payload_format != 0) && (rx_base->rxd_1.hdr_trans == 0)) {
			pRxBlk->pData += 2;
			pRxBlk->DataSize -= 2;
		}
	}
#endif	
	/*nMSDU =*/ deaggregate_AMSDU_announce(pAd, pRxBlk->pRxPacket, pRxBlk->pData, pRxBlk->DataSize, pRxBlk->OpMode);
}
#endif /* DOT11_N_SUPPORT */


VOID Announce_or_Forward_802_3_Packet(
	IN RTMP_ADAPTER *pAd,
	IN PNDIS_PACKET pPacket,
	IN UCHAR wdev_idx,
	IN UCHAR op_mode)
{
	INT to_os = FALSE;
	struct wifi_dev *wdev;

	ASSERT(wdev_idx < WDEV_NUM_MAX);
	if (wdev_idx >= WDEV_NUM_MAX) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():invalid wdev_idx(%d)!\n", __FUNCTION__, wdev_idx));
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		return;
	}
	wdev = pAd->wdev_list[wdev_idx];
	if (wdev->rx_pkt_foward)
		to_os = wdev->rx_pkt_foward(pAd, wdev, pPacket);

	if (to_os == TRUE)
		announce_802_3_packet(pAd, pPacket,op_mode);
	else {
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
	}
}


/* Normal legacy Rx packet indication*/
VOID Indicate_Legacy_Packet(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk, UCHAR wdev_idx)
{
	PNDIS_PACKET pRxPacket = pRxBlk->pRxPacket;
	UCHAR Header802_3[LENGTH_802_3];
	USHORT VLAN_VID = 0, VLAN_Priority = 0;
	UINT max_pkt_len = MAX_RX_PKT_LEN;
	UCHAR *pData = pRxBlk->pData;
	INT data_len = pRxBlk->DataSize;
	struct wifi_dev *wdev;
	UCHAR opmode = pAd->OpMode;

	if (check_rx_pkt_pn_allowed(pAd, pRxBlk) == FALSE) {
		DBGPRINT(RT_DEBUG_OFF, ("%s:drop packet by PN mismatch!\n", __func__));
		RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

	ASSERT(wdev_idx < WDEV_NUM_MAX);
	if (wdev_idx >= WDEV_NUM_MAX) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():invalid wdev_idx(%d)!\n", __FUNCTION__, wdev_idx));
		RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}
	wdev = pAd->wdev_list[wdev_idx];

//+++Add by shiang for debug
if (1) {
#ifdef HDR_TRANS_SUPPORT
	if (pRxBlk->bHdrRxTrans) {
		pData = pRxBlk->pTransData;
		data_len = pRxBlk->TransDataSize;
	}
#endif /* HDR_TRANS_SUPPORT */
//	hex_dump("Indicate_Legacy_Packet", pData, data_len);
//	hex_dump("802_11_hdr", (UCHAR *)pRxBlk->pHeader, LENGTH_802_11);
}
//---Add by shiang for debug

	/*
		1. get 802.3 Header
		2. remove LLC
			a. pointer pRxBlk->pData to payload
			b. modify pRxBlk->DataSize
	*/
#ifdef HDR_TRANS_SUPPORT
	if (pRxBlk->bHdrRxTrans) {
		max_pkt_len = 1514;
		pData = pRxBlk->pTransData;
		data_len = pRxBlk->TransDataSize;
	}
	else
#endif /* HDR_TRANS_SUPPORT */

	RTMP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(pRxBlk, Header802_3);
	//hex_dump("802_3_hdr", (UCHAR *)Header802_3, LENGTH_802_3);

	pData = pRxBlk->pData;
	data_len = pRxBlk->DataSize;


	if (data_len > max_pkt_len)
	{
		RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
DBGPRINT(RT_DEBUG_ERROR, ("%s():data_len(%d) > max_pkt_len(%d)!\n",
			__FUNCTION__, data_len, max_pkt_len));
		return;
	}

	STATS_INC_RX_PACKETS(pAd, wdev_idx);

#ifdef RTMP_MAC_USB
#ifdef DOT11_N_SUPPORT
	if (1)//pAd->CommonCfg.bDisableReordering == 0)
	{
		BA_REC_ENTRY *pBAEntry;
		ULONG Now32;
		UCHAR Wcid = pRxBlk->wcid;
		UCHAR TID = pRxBlk->TID;
		USHORT Idx;

#define REORDERING_PACKET_TIMEOUT		((100 * OS_HZ)/1000)	/* system ticks -- 100 ms*/

		if (Wcid < MAX_LEN_OF_MAC_TABLE)
		{
			Idx = pAd->MacTab.Content[Wcid].BARecWcidArray[TID];
			if (Idx != 0)
			{
				pBAEntry = &pAd->BATable.BARecEntry[Idx];
				/* update last rx time*/
				NdisGetSystemUpTime(&Now32);
				if ((pBAEntry->list.qlen > 0) &&
					 RTMP_TIME_AFTER((unsigned long)Now32, (unsigned long)(pBAEntry->LastIndSeqAtTimer+(REORDERING_PACKET_TIMEOUT)))
	   			)
				{
					DBGPRINT(RT_DEBUG_OFF, ("Indicate_Legacy_Packet():flush reordering_timeout_mpdus! pRxBlk->Flags=%d, pRxBlk->TID=%d, pRxInfo->AMPDU=%d!\n",
												pRxBlk->Flags, pRxBlk->TID, pRxBlk->pRxInfo->AMPDU));
					hex_dump("Dump the legacy Packet:", GET_OS_PKT_DATAPTR(pRxPacket), 64);
					ba_flush_reordering_timeout_mpdus(pAd, pBAEntry, Now32);
				}
			}
		}
	}
#endif /* DOT11_N_SUPPORT */
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_AP_SUPPORT
	WDEV_VLAN_INFO_GET(pAd, VLAN_VID, VLAN_Priority, wdev);
#endif /* CONFIG_AP_SUPPORT */

//+++Add by shiang for debug
if (0) {
	hex_dump("Before80211_2_8023", pData, data_len);
	hex_dump("header802_3", &Header802_3[0], LENGTH_802_3);
}
//---Add by shiang for debug

#ifdef HDR_TRANS_SUPPORT
	if (pRxBlk->bHdrRxTrans) {
		struct sk_buff *pOSPkt = RTPKT_TO_OSPKT(pRxPacket);

		pOSPkt->dev = get_netdev_from_bssid(pAd, wdev_idx);
		pOSPkt->data = pRxBlk->pTransData;
		pOSPkt->len = pRxBlk->TransDataSize;
		pOSPkt->tail = pOSPkt->data + pOSPkt->len;
		//printk("%s: rx trans ...%d\n", __FUNCTION__, __LINE__);
	}
	else
#endif /* HDR_TRANS_SUPPORT */
	{
		RT_80211_TO_8023_PACKET(pAd, VLAN_VID, VLAN_Priority,
							pRxBlk, Header802_3, wdev_idx, TPID);
	}

	/* pass this 802.3 packet to upper layer or forward this packet to WM directly*/
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef MAC_REPEATER_SUPPORT /* This should be moved to some where else */
		if (pRxBlk->pRxInfo->Bcast && (pAd->ApCfg.bMACRepeaterEn) && (pAd->ApCfg.MACRepeaterOuiMode != 1))
		{
			PUCHAR pPktHdr, pLayerHdr;

			pPktHdr = GET_OS_PKT_DATAPTR(pRxPacket);
			pLayerHdr = (pPktHdr + MAT_ETHER_HDR_LEN);

			/*For UDP packet, we need to check about the DHCP packet. */
			if (*(pLayerHdr + 9) == 0x11)
			{
				PUCHAR pUdpHdr;
				UINT16 srcPort, dstPort;
				BOOLEAN bHdrChanged = FALSE;

				pUdpHdr = pLayerHdr + 20;
				srcPort = OS_NTOHS(get_unaligned((PUINT16)(pUdpHdr)));
				dstPort = OS_NTOHS(get_unaligned((PUINT16)(pUdpHdr+2)));

				if (srcPort==67 && dstPort==68) /*It's a DHCP packet */
				{
					PUCHAR bootpHdr/*, dhcpHdr*/, pCliHwAddr;
					REPEATER_CLIENT_ENTRY *pReptEntry = NULL;

					bootpHdr = pUdpHdr + 8;
					//dhcpHdr = bootpHdr + 236;
					pCliHwAddr = (bootpHdr+28);
#if 0
					DBGPRINT(RT_DEBUG_ERROR, ("pCliHwAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
										pCliHwAddr[0], pCliHwAddr[1], pCliHwAddr[2],
										pCliHwAddr[3], pCliHwAddr[4], pCliHwAddr[5]));
#endif
					pReptEntry = RTMPLookupRepeaterCliEntry(pAd, FALSE, pCliHwAddr);
					if (pReptEntry)
						NdisMoveMemory(pCliHwAddr, pReptEntry->OriginalAddress, MAC_ADDR_LEN);
#if 0
					DBGPRINT(RT_DEBUG_ERROR, ("pCliHwAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
										pCliHwAddr[0], pCliHwAddr[1], pCliHwAddr[2],
										pCliHwAddr[3], pCliHwAddr[4], pCliHwAddr[5]));
#endif
					bHdrChanged = TRUE;
				}

				if (bHdrChanged == TRUE)
					NdisZeroMemory((pUdpHdr+6), 2); /*modify the UDP chksum as zero */
			}
		}
#endif /* MAC_REPEATER_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef P2P_SUPPORT
	if (RX_BLK_TEST_FLAG(pRxBlk, fRX_STA))
	{
		WDEV_VLAN_INFO_GET(pAd, VLAN_VID, VLAN_Priority, wdev);
	}
	opmode = pRxBlk->OpMode;
#endif /* P2P_SUPPORT */

//+++Add by shiang for debug
if (0) {
	hex_dump("After80211_2_8023", GET_OS_PKT_DATAPTR(pRxPacket), GET_OS_PKT_LEN(pRxPacket));
}
//---Add by shiang for debug
	Announce_or_Forward_802_3_Packet(pAd, pRxPacket, wdev->wdev_idx, opmode);
}


/* Ralink Aggregation frame */
VOID Indicate_ARalink_Packet(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN RX_BLK *pRxBlk,
	IN UCHAR wdev_idx)
{
	UCHAR Header802_3[LENGTH_802_3];
	UINT16 Msdu2Size;
	UINT16 Payload1Size, Payload2Size;
	PUCHAR pData2;
	PNDIS_PACKET pPacket2 = NULL;
	USHORT VLAN_VID = 0, VLAN_Priority = 0;
	UCHAR opmode = pAd->OpMode;
	struct wifi_dev *wdev;

	ASSERT(wdev_idx < WDEV_NUM_MAX);
	if (wdev_idx >= WDEV_NUM_MAX) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Invalid wdev_idx(%d)\n", __FUNCTION__, wdev_idx));
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}
	wdev = pAd->wdev_list[wdev_idx];

	Msdu2Size = *(pRxBlk->pData) + (*(pRxBlk->pData+1) << 8);
	if ((Msdu2Size <= 1536) && (Msdu2Size < pRxBlk->DataSize))
	{
		/* skip two byte MSDU2 len */
		pRxBlk->pData += 2;
		pRxBlk->DataSize -= 2;
	}
	else
	{
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

	/* get 802.3 Header and  remove LLC*/
	RTMP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(pRxBlk, Header802_3);

	ASSERT(pRxBlk->pRxPacket);

	pAd->RalinkCounters.OneSecRxARalinkCnt++;
	Payload1Size = pRxBlk->DataSize - Msdu2Size;
	Payload2Size = Msdu2Size - LENGTH_802_3;

	pData2 = pRxBlk->pData + Payload1Size + LENGTH_802_3;
	pPacket2 = duplicate_pkt_vlan(wdev->if_dev,
							wdev->VLAN_VID, wdev->VLAN_Priority,
							(pData2-LENGTH_802_3), LENGTH_802_3,
							pData2, Payload2Size, TPID);

	if (!pPacket2)
	{
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

	/* update payload size of 1st packet*/
	pRxBlk->DataSize = Payload1Size;
	RT_80211_TO_8023_PACKET(pAd, VLAN_VID, VLAN_Priority,
							pRxBlk, Header802_3, wdev_idx, TPID);

#ifdef P2P_SUPPORT
	opmode = pRxBlk->OpMode;
#endif /* P2P_SUPPORT */
	Announce_or_Forward_802_3_Packet(pAd, pRxBlk->pRxPacket, wdev_idx, opmode);
	if (pPacket2)
		Announce_or_Forward_802_3_Packet(pAd, pPacket2, wdev_idx, opmode);
}


PNDIS_PACKET RTMPDeFragmentDataFrame(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	PNDIS_PACKET pRxPacket = pRxBlk->pRxPacket;
	UCHAR *pData = pRxBlk->pData;
	USHORT DataSize = pRxBlk->DataSize;
	PNDIS_PACKET pRetPacket = NULL;
	UCHAR *pFragBuffer = NULL;
	BOOLEAN bReassDone = FALSE;
	UINT HeaderRoom = 0;
	RXWI_STRUC *pRxWI = pRxBlk->pRxWI;
	UINT8 RXWISize = pAd->chipCap.RXWISize;
	MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
	PCIPHER_KEY pKey = NULL;

#ifdef MT_MAC
	if (pAd->chipCap.hif_type == HIF_MT)
		RXWISize = 0;
#endif /* MT_MAC */	

	ASSERT(pHeader);

	HeaderRoom = pData - (UCHAR *)pHeader;

	pKey = RTMPSwCipherKeySelection(pAd, pRxBlk->pData, pRxBlk, pEntry);

	DBGPRINT(RT_DEBUG_OFF,
		("%s: SN:%d,FN:%d,PN:%llu, Decrypted:%d, CipherErr:%d, pKey:%p\n",
		__func__, pHeader->Sequence, pHeader->Frag,
		pRxBlk->ccmp_pn, pRxBlk->pRxInfo->Decrypted,
		pRxBlk->pRxInfo->CipherErr, pKey));

	/* Re-assemble the fragmented packets*/
	if (pHeader->Frag == 0)
	{	/* Frag. Number is 0 : First frag or only one pkt*/
		/* the first pkt of fragment, record it.*/
		if (pHeader->FC.MoreFrag)
		{
			ASSERT(pAd->FragFrame.pFragPacket);
			pFragBuffer = GET_OS_PKT_DATAPTR(pAd->FragFrame.pFragPacket);
			/* Fix MT5396 crash issue when Rx fragmentation frame for Wi-Fi TGn 5.2.4 & 5.2.13 test items.
			    Copy RxWI content to pFragBuffer.
			*/
			//pAd->FragFrame.RxSize = DataSize + HeaderRoom;
			//NdisMoveMemory(pFragBuffer, pHeader, pAd->FragFrame.RxSize);
			pAd->FragFrame.RxSize = DataSize + HeaderRoom + RXWISize;
			NdisMoveMemory(pFragBuffer, pRxWI, RXWISize);
			NdisMoveMemory(pFragBuffer + RXWISize,	 pHeader, pAd->FragFrame.RxSize - RXWISize);
			pAd->FragFrame.Sequence = pHeader->Sequence;
			pAd->FragFrame.LastFrag = pHeader->Frag;	   /* Should be 0*/
			pAd->FragFrame.wcid = pRxBlk->wcid;   /* to tell from the fragment Buffer from which STA */
			if((pEntry->WepStatus == Ndis802_11TKIPEnable) ||
				(pEntry->WepStatus == Ndis802_11AESEnable) ||
				(pEntry->WepStatus == Ndis802_11TKIPAESMix)
#ifdef WPA3_SUPPORT
#ifdef CFG_SUPPORT_SUITB
				|| (pEntry->WepStatus == Ndis802_11GCMP256Enable)
#endif
#endif
				) {
				pAd->FragFrame.sec_on = TRUE;
				pAd->FragFrame.pKey = pKey;
				pAd->FragFrame.LastPN = pRxBlk->ccmp_pn;
			}
			DBGPRINT(RT_DEBUG_OFF, ("Fragment PACKET INIT, %d,%d,%llu - %d,%d,%llu.\n",
				pHeader->Sequence, pHeader->Frag, pRxBlk->ccmp_pn,
				pAd->FragFrame.Sequence, pAd->FragFrame.LastFrag, pAd->FragFrame.LastPN));
			ASSERT(pAd->FragFrame.LastFrag == 0);
			goto done;	/* end of processing this frame*/
		}
	}
	else
	{	/*Middle & End of fragment*/
		if ((pHeader->Sequence != pAd->FragFrame.Sequence) ||
			(pHeader->Frag != (pAd->FragFrame.LastFrag + 1)) ||
			((pAd->FragFrame.sec_on) &&
			(pRxBlk->ccmp_pn != (pAd->FragFrame.LastPN + 1))))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Fragment is not the same SN or out of FN/PN order,this frag %d,%d,%llu <--> last frag %d,%d,%llu\n\n",
				__func__, pHeader->Sequence, pHeader->Frag, pRxBlk->ccmp_pn,
				pAd->FragFrame.Sequence, pAd->FragFrame.LastFrag, pAd->FragFrame.LastPN));
			/* Fragment is not the same sequence or out of fragment number order*/
			/* Reset Fragment control blk*/
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Clear Frag Buffer, drop this frag\n",__func__));
			RESET_FRAGFRAME(pAd->FragFrame);
			goto done; /* give up this frame*/
		}
		/* Fix MT5396 crash issue when Rx fragmentation frame for Wi-Fi TGn 5.2.4 & 5.2.13 test items. */
		//else if ((pAd->FragFrame.RxSize + DataSize) > MAX_FRAME_SIZE)
		else if ((pAd->FragFrame.RxSize + DataSize) > MAX_FRAME_SIZE + RXWISize)
		{
			/* Fragment frame is too large, it exeeds the maximum frame size.*/
			/* Reset Fragment control blk*/
			RESET_FRAGFRAME(pAd->FragFrame);
			DBGPRINT(RT_DEBUG_ERROR, ("%s: Drop this frag, and clear frag buffer as this Fragment frame is too large, it exeeds the maximum frame size.\n", __func__));
			goto done;
		}


		/* Broadcom AP(BCM94704AGR) will send out LLC in fragment's packet, LLC only can accpet at first fragment.*/
		/* In this case, we will drop it.*/
		if (NdisEqualMemory(pData, SNAP_802_1H, sizeof(SNAP_802_1H)))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Find another LLC at Middle or End fragment(SN=%d, Frag=%d)\n", pHeader->Sequence, pHeader->Frag));
			goto done;
		}

		pFragBuffer = GET_OS_PKT_DATAPTR(pAd->FragFrame.pFragPacket);

		/* concatenate this fragment into the re-assembly buffer*/
		NdisMoveMemory((pFragBuffer + pAd->FragFrame.RxSize), pData, DataSize);
		pAd->FragFrame.RxSize  += DataSize;
		pAd->FragFrame.LastFrag = pHeader->Frag;	   /* Update fragment number*/
		pAd->FragFrame.LastPN = pRxBlk->ccmp_pn;

		/* Last fragment*/
		if (pHeader->FC.MoreFrag == FALSE)
			bReassDone = TRUE;
	}

done:
	/* always release rx fragmented packet*/
	RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);

	/* return defragmented packet if packet is reassembled completely*/
	/* otherwise return NULL*/
	if (bReassDone)
	{
		PNDIS_PACKET pNewFragPacket;

		/* allocate a new packet buffer for fragment*/
		pNewFragPacket = RTMP_AllocateFragPacketBuffer(pAd, RX_BUFFER_NORMSIZE);
		if (pNewFragPacket)
		{
			/* update RxBlk*/
			pRetPacket = pAd->FragFrame.pFragPacket;
			pAd->FragFrame.pFragPacket = pNewFragPacket;
			/* Fix MT5396 crash issue when Rx fragmentation frame for Wi-Fi TGn 5.2.4 & 5.2.13 test items. */
			//pRxBlk->pHeader = (PHEADER_802_11) GET_OS_PKT_DATAPTR(pRetPacket);
			//pRxBlk->pData = (UCHAR *)pRxBlk->pHeader + HeaderRoom;
			//pRxBlk->DataSize = pAd->FragFrame.RxSize - HeaderRoom;
			//pRxBlk->pRxPacket = pRetPacket;
			pRxBlk->pRxWI = (RXWI_STRUC *) GET_OS_PKT_DATAPTR(pRetPacket);
			pRxBlk->pHeader = (PHEADER_802_11) ((UCHAR *)pRxBlk->pRxWI + RXWISize);
			pRxBlk->pData = (UCHAR *)pRxBlk->pHeader + HeaderRoom;
			pRxBlk->DataSize = (USHORT)(pAd->FragFrame.RxSize - HeaderRoom - RXWISize);
			pRxBlk->pRxPacket = pRetPacket;
		}
		else
		{
			RESET_FRAGFRAME(pAd->FragFrame);
			DBGPRINT(RT_DEBUG_ERROR, ("%s:Clear Frag Buffer as Re-assemble done,but alloc NewFragBuffer fail\n", __func__));
		}
	}

	return pRetPacket;
}


VOID rx_eapol_frm_handle(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN RX_BLK *pRxBlk,
	IN UCHAR wdev_idx)
{
	UCHAR *pTmpBuf;
	BOOLEAN to_mlme = TRUE, to_daemon = FALSE;
	struct wifi_dev *wdev;

#if defined(WPA_SUPPLICANT_SUPPORT) && defined(CONFIG_AP_SUPPORT)
	STA_TR_ENTRY *tr_entry;
#endif

	ASSERT(wdev_idx < WDEV_NUM_MAX);
	if (wdev_idx >= WDEV_NUM_MAX)
		goto done;

	wdev = pAd->wdev_list[wdev_idx];
#ifdef MESH_SUPPORT
	if (wdev->wdev_type == WDEV_TYPE_WDS) {
		UINT mesh_header_len = GetMeshHederLen(pRxBlk->pData);

		/* skip mesh header */
		pRxBlk->pData += mesh_header_len;
		pRxBlk->DataSize -= mesh_header_len;
	}
#endif /* MESH_SUPPORT */

	if(pRxBlk->DataSize < (LENGTH_802_1_H + LENGTH_EAPOL_H))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("pkts size too small\n"));
		goto done;
	}
	else if (!RTMPEqualMemory(SNAP_802_1H, pRxBlk->pData, 6))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("no SNAP_802_1H parameter\n"));
		goto done;
	}
	else if (!RTMPEqualMemory(EAPOL, pRxBlk->pData+6, 2))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("no EAPOL parameter\n"));
		goto done;
	}
	else if(*(pRxBlk->pData+9) > EAPOLASFAlert)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Unknown EAP type(%d)\n", *(pRxBlk->pData+9)));
		goto done;
	}

#ifdef CONFIG_AP_SUPPORT
	if (pEntry && IS_ENTRY_CLIENT(pEntry))
	{
#ifdef EASY_CONFIG_SETUP
		if ((pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.bEnable) &&
			(pEntry->bRaAutoWpsCapable)
#ifdef WSC_AP_SUPPORT
			&& (pAd->ApCfg.MBSSID[pEntry->func_tb_idx].WscControl.WscConfMode == WSC_DISABLE)
#endif /* WSC_AP_SUPPORT */
		)
		{
			UCHAR *pData;
			PEAP_FRAME pEapFrame = NULL;

			/* Skip the EAP LLC header */
			pData = (UCHAR *)(pRxBlk->pData + LENGTH_802_1_H);
			if (pData)
				pEapFrame = (PEAP_FRAME)(pData + sizeof(IEEE8021X_FRAME));
			pData += (sizeof(IEEE8021X_FRAME) + sizeof(EAP_FRAME));

			if ((pData != NULL) && (pEapFrame != NULL) &&
				(pEapFrame->Code == EAP_CODE_RSP) &&
				(pEapFrame->Type == EAP_TYPE_WSC))
			{
				if (WscCheckWSCHeader(pData))
				{
					/* EAP-Rsp (Messages) */
					pData += sizeof(WSC_FRAME);
					if (pData && (pData + 9))
					{
						UCHAR MsgType = *(pData + 9);
						if (MsgType == 0x04) /* M1 */
							AutoProvisionDecodeExtData(pAd, pEntry->func_tb_idx, pEntry->Addr, pRxBlk->pData + (pRxBlk->DataSize -15));
					}
				}
			}
		}
#endif /* EASY_CONFIG_SETUP */

#ifdef HOSTAPD_SUPPORT
		if (pAd->ApCfg.MBSSID[pEntry->func_tb_idx].Hostapd == Hostapd_EXT)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Indicate_Legacy_Packet\n"));
			Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
			return;
		}
#endif/*HOSTAPD_SUPPORT*/
	}
#endif /* CONFIG_AP_SUPPORT */


#ifdef RT_CFG80211_SUPPORT
	if (pEntry)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("CFG80211 EAPOL Indicate_Legacy_Packet\n"));
		Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
		return;
	}
#endif /*RT_CFG80211_SUPPORT*/

	if (pEntry && IS_ENTRY_AP(pEntry))
	{
#ifdef WPA_SUPPLICANT_SUPPORT
		WPA_SUPPLICANT_INFO *sup_info = NULL;
		CIPHER_KEY *share_key = NULL;
		UCHAR BssIdx;

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			sup_info = &pAd->StaCfg.wpa_supplicant_info;
			share_key = &pAd->SharedKey[BSS0][0];
			BssIdx = BSS0;
		}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
		if (IS_ENTRY_APCLI(pEntry))
		{
			APCLI_STRUCT *apcli_entry = &pAd->ApCfg.ApCliTab[pEntry->func_tb_idx];

			sup_info = &apcli_entry->wpa_supplicant_info;
			share_key = &apcli_entry->SharedKey[0];
			BssIdx = pAd->ApCfg.BssidNum + MAX_MESH_NUM + pEntry->func_tb_idx;
		}
#endif /* APCLI_SUPPORT */		
#endif /* CONFIG_AP_SUPPORT */

		if ((sup_info != NULL) && (sup_info->WpaSupplicantUP))
		{
			INT eapcode = WpaCheckEapCode(pAd, pRxBlk->pData, pRxBlk->DataSize, LENGTH_802_1_H);
			struct wifi_dev *wdev = pEntry->wdev;

			/*
				All EAPoL frames have to pass to upper layer (ex. WPA_SUPPLICANT daemon)

				For dynamic WEP(802.1x+WEP) mode, if the received frame is EAP-SUCCESS packet, turn
				on the PortSecured variable
			*/
			to_mlme = FALSE;
			DBGPRINT(RT_DEBUG_TRACE, ("eapcode=%d\n",eapcode));

			if ((wdev->IEEE8021X == TRUE) && (wdev->WepStatus == Ndis802_11WEPEnabled) &&
				(eapcode == EAP_CODE_SUCCESS))
			{
				//UCHAR *Key;
				UCHAR CipherAlg;
				UCHAR idx = 0;

				DBGPRINT_RAW(RT_DEBUG_TRACE, ("Receive EAP-SUCCESS Packet\n"));
#ifdef CONFIG_AP_SUPPORT
				if (IS_ENTRY_APCLI(pEntry)) {
					tr_entry = &pAd->MacTab.tr_entry[pEntry->wcid];
					tr_entry->PortSecured=WPA_802_1X_PORT_SECURED;
					pEntry->PrivacyFilter=Ndis802_11PrivFilterAcceptAll;
				}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
				IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				{
					STA_PORT_SECURED(pAd);
				}
#endif /* CONFIG_STA_SUPPORT */

				if (sup_info->IEEE8021x_required_keys == FALSE)
				{
					idx = sup_info->DesireSharedKeyId;
					CipherAlg = sup_info->DesireSharedKey[idx].CipherAlg;
					//Key = sup_info->DesireSharedKey[idx].Key;

					if (sup_info->DesireSharedKey[idx].KeyLen > 0)
					{
						/* Set key material and cipherAlg to ASIC */
						RTMP_ASIC_SHARED_KEY_TABLE(pAd, BssIdx, idx,
											&sup_info->DesireSharedKey[idx]);

						/* STA doesn't need to set WCID attribute for group key */
						/* Assign pairwise key info */
						RTMP_SET_WCID_SEC_INFO(pAd, BssIdx, idx,
											CipherAlg, pEntry->wcid,
											SHAREDKEYTABLE);

#ifdef CONFIG_STA_SUPPORT
						IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
						{
							RTMP_IndicateMediaState(pAd, NdisMediaStateConnected);
							pAd->ExtraInfo = GENERAL_LINK_UP;
						}
#endif /* CONFIG_STA_SUPPORT */
						/* For Preventing ShardKey Table is cleared by remove key procedure. */
						share_key[idx].CipherAlg = CipherAlg;
						share_key[idx].KeyLen = sup_info->DesireSharedKey[idx].KeyLen;
						NdisMoveMemory(share_key[idx].Key,
										sup_info->DesireSharedKey[idx].Key,
										sup_info->DesireSharedKey[idx].KeyLen);
					}
				}
			}

#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			{
#ifdef WSC_STA_SUPPORT
				/* report EAP packets to MLME to check this packet is WPS packet or not */
				if (pAd->StaCfg.WscControl.WscState >= WSC_STATE_LINK_UP)
					to_mlme = TRUE;
#endif /* WSC_STA_SUPPORT */
			}
#endif /* CONFIG_STA_SUPPORT */

			if ((pEntry->AuthMode == Ndis802_11AuthModeWPA) ||
				(pEntry->AuthMode == Ndis802_11AuthModeWPA2) ||
				(wdev->IEEE8021X == TRUE))
			{
				to_daemon = TRUE;
			}
		}
		else
#endif /* WPA_SUPPLICANT_SUPPORT */
		{
			to_mlme = TRUE;
			to_daemon = FALSE;
		}
	}

#ifdef CONFIG_AP_SUPPORT
	if (pEntry && IS_ENTRY_CLIENT(pEntry))
	{
#ifdef DOT1X_SUPPORT
		/* sent this frame to upper layer TCPIP */
		if ((pEntry->WpaState < AS_INITPMK) &&
			((pEntry->AuthMode == Ndis802_11AuthModeWPA) ||
			((pEntry->AuthMode == Ndis802_11AuthModeWPA2) && (pEntry->PMKID_CacheIdx == ENTRY_NOT_FOUND)) ||
			pAd->ApCfg.MBSSID[pEntry->func_tb_idx].wdev.IEEE8021X == TRUE))
		{
#ifdef WSC_AP_SUPPORT
			/* report EAP packets to MLME to check this packet is WPS packet or not */
			if ((pAd->ApCfg.MBSSID[pEntry->func_tb_idx].WscControl.WscConfMode != WSC_DISABLE) &&
				(!MAC_ADDR_EQUAL(pAd->ApCfg.MBSSID[pEntry->func_tb_idx].WscControl.EntryAddr, ZERO_MAC_ADDR)))
			{
				to_mlme = TRUE;
				pTmpBuf = pRxBlk->pData - LENGTH_802_11;
				// TODO: shiang-usw, why we need to change pHeader here??
				pRxBlk->pHeader = (PHEADER_802_11)pTmpBuf;
			}
#endif /* WSC_AP_SUPPORT */

#ifdef VXWORKS
			/*
				This is a patch for vxworks bridge issue
				VxWorks bridge doesn't forward EAPOL frame to upper layer application
				So, the EAPOL frames need to be masqueraded.
			*/
			{
				UCHAR RALINK_EAPOL[] = {0x0C, 0x43};

				NdisMoveMemory(pRxBlk->pData+6, RALINK_EAPOL, 2);
			}
#endif /* VXWORKS */

			to_daemon = TRUE;
			to_mlme = FALSE;
		}
		else
#endif /* DOT1X_SUPPORT */
		{
			/* sent this frame to WPA state machine */

			/*
				Check Addr3 (DA) is AP or not.
				If Addr3 is AP, forward this EAP packets to MLME
				If Addr3 is NOT AP, forward this EAP packets to upper layer or STA.
			*/
			if (wdev->wdev_type == WDEV_TYPE_AP) {
				ASSERT(wdev->func_idx < HW_BEACON_MAX_NUM);
				if (wdev->func_idx < HW_BEACON_MAX_NUM) {
					ASSERT(wdev == (&pAd->ApCfg.MBSSID[wdev->func_idx].wdev));
				}
			}

			// TODO: shiang-usw, why we check this here??
			if ((wdev->wdev_type == WDEV_TYPE_AP) &&
				(NdisEqualMemory(pRxBlk->pHeader->Addr3, pAd->ApCfg.MBSSID[wdev->func_idx].wdev.bssid, MAC_ADDR_LEN) == FALSE))
				to_daemon = TRUE;
			else
				to_mlme = TRUE;
		}
	}
#endif /* CONFIG_AP_SUPPORT */

	/*
	   Special DATA frame that has to pass to MLME
	   1. Cisco Aironet frames for CCX2. We need pass it to MLME for special process
	   2. EAPOL handshaking frames when driver supplicant enabled, pass to MLME for special process
	 */
	if (to_mlme)
	{
		pTmpBuf = pRxBlk->pData - LENGTH_802_11;
		NdisMoveMemory(pTmpBuf, pRxBlk->pHeader, LENGTH_802_11);
		REPORT_MGMT_FRAME_TO_MLME(pAd, pRxBlk->wcid,
							pTmpBuf,
							pRxBlk->DataSize + LENGTH_802_11,
							pRxBlk->rx_signal.raw_rssi[0],
							pRxBlk->rx_signal.raw_rssi[1],
							pRxBlk->rx_signal.raw_rssi[2],
							0,
							pRxBlk->OpMode);

		DBGPRINT_RAW(RT_DEBUG_TRACE,
			     ("!!! report EAPOL DATA to MLME (len=%d) !!!\n",
			      pRxBlk->DataSize));
	}

	if (to_daemon == TRUE)
	{
		Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
		return;
	}

done:
	RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
	return;
}


VOID Indicate_EAPOL_Packet(
	IN RTMP_ADAPTER *pAd,
	IN RX_BLK *pRxBlk,
	IN UCHAR wdev_idx)
{
	MAC_TABLE_ENTRY *pEntry = NULL;

	if (pRxBlk->wcid >= MAX_LEN_OF_MAC_TABLE)
	{
		DBGPRINT(RT_DEBUG_WARN, ("Indicate_EAPOL_Packet: invalid wcid.\n"));
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

	pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
	if (pEntry == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("Indicate_EAPOL_Packet: drop and release the invalid packet.\n"));
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

	rx_eapol_frm_handle(pAd, pEntry, pRxBlk, wdev_idx);
	return;
}

bool check_duplicated_mgmt_frame(HEADER_802_11 *pHeader)
{
	static DUPLICATED_FRAME duplicated_frame[15]={
		{-1,{0}},{-1,{0}},{-1,{0}},{-1,{0}},{-1,{0}},
		{-1,{0}},{-1,{0}},{-1,{0}},{-1,{0}},{-1,{0}},
		{-1,{0}},{-1,{0}},{-1,{0}},{-1,{0}},{-1,{0}}};
	INT current_sn = pHeader->Sequence;
	UINT16 retry = pHeader->FC.Retry;
	UINT16 mgmt_type = pHeader->FC.SubType;
	
 	if (mgmt_type >= 15) {
		DBGPRINT(RT_DEBUG_INFO, ("%s:: check duplicated mgmt frame fail(invilide mgmt subtype(%d)) \n",__FUNCTION__, mgmt_type));
		return FALSE;
	}
	
	if (MAC_ADDR_EQUAL(duplicated_frame[mgmt_type].prev_mgmt_src_addr, pHeader->Addr2) && retry == 1 
		&& duplicated_frame[mgmt_type].prev_mgmt_frame_sn == current_sn) {
		DBGPRINT(RT_DEBUG_TRACE,
		("%s:: Drop duplicated mgmt frame(subtype=%d, current_sn=%d, prev_sn=%d, retry=%d) \n",
			__FUNCTION__,
			mgmt_type, current_sn, duplicated_frame[mgmt_type].prev_mgmt_frame_sn, retry));
		return TRUE;
	} else {
		duplicated_frame[mgmt_type].prev_mgmt_frame_sn = current_sn;
		COPY_MAC_ADDR(duplicated_frame[mgmt_type].prev_mgmt_src_addr, pHeader->Addr2);
		return FALSE;
	}
}

// TODO: shiang-usw, modify the op_mode assignment for this function!!!
VOID dev_rx_mgmt_frm(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	PNDIS_PACKET pRxPacket = pRxBlk->pRxPacket;
	MAC_TABLE_ENTRY *pEntry = NULL;
	UCHAR op_mode = pRxBlk->OpMode;

DBGPRINT(RT_DEBUG_FPGA, ("-->%s()\n", __FUNCTION__));

#ifdef CFG_TDLS_SUPPORT
	if (CFG80211_HandleTdlsDiscoverRespFrame(pAd, pRxBlk, op_mode))
		goto done;
#endif /* CFG_TDLS_SUPPORT */

#ifdef RT_CFG80211_SUPPORT
	if (CFG80211_HandleP2pMgmtFrame(pAd, pRxBlk, op_mode))
		goto done;
#endif /* RT_CFG80211_SUPPORT */

#ifdef MESH_SUPPORT
	if (pHeader->FC.SubType == SUBTYPE_MULTIHOP) {
		MlmeHandleRxMeshFrame(pAd, pRxBlk);
		goto done;
	}
#endif /* MESH_SUPPORT */

#ifdef DOT11W_PMF_SUPPORT
	if (PMF_PerformRxFrameAction(pAd, pRxBlk) == FALSE)
		goto done;
#endif /* DOT11W_PMF_SUPPORT */

	if (pRxBlk->wcid < MAX_LEN_OF_MAC_TABLE)
		pEntry = &pAd->MacTab.Content[pRxBlk->wcid];

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;

		op_mode = OPMODE_AP;
#ifdef IDS_SUPPORT
		/*
			Check if a rogue AP impersonats our mgmt frame to spoof clients
			drop it if it's a spoofed frame
		*/
		if (RTMPSpoofedMgmtDetection(pAd, pHeader, pRxBlk))
			goto done;

		/* update sta statistics for traffic flooding detection later */
		RTMPUpdateStaMgmtCounter(pAd, pHeader->FC.SubType);
#endif /* IDS_SUPPORT */

		if (!pRxInfo->U2M)
		{
			if ((pHeader->FC.SubType != SUBTYPE_BEACON) && (pHeader->FC.SubType != SUBTYPE_PROBE_REQ))
			{
#ifdef WMM_ACM_SUPPORT
				if ((pHeader->FC.SubType == SUBTYPE_ACTION) &&
					(ACMP_IsBwAnnounceActionFrame(pAd, (VOID *)pHeader) != ACM_RTN_OK))
#endif /* WMM_ACM_SUPPORT */
					goto done;
			}
		}

		/* Software decrypt WEP data during shared WEP negotiation */
		if ((pHeader->FC.SubType == SUBTYPE_AUTH) &&
			(pHeader->FC.Wep == 1) && (pRxInfo->Decrypted == 0))
		{
			UCHAR *pMgmt = (PUCHAR)pHeader;
			UINT16 mgmt_len = pRxBlk->MPDUtotalByteCnt;

			if (!pEntry)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("ERROR: SW decrypt WEP data fails - the Entry is empty.\n"));
				goto done;
			}

			/* Skip 802.11 header */
			pMgmt += LENGTH_802_11;
			mgmt_len -= LENGTH_802_11;

			/* handle WEP decryption */
			if (RTMPSoftDecryptWEP(pAd,
								   &pAd->SharedKey[pEntry->func_tb_idx][pRxBlk->key_idx],
								   pMgmt,
								   &mgmt_len) == FALSE)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("ERROR: SW decrypt WEP data fails.\n"));
				goto done;
			}

#ifdef RT_BIG_ENDIAN
			/* swap 16 bit fields - Auth Alg No. field */
			*(USHORT *)pMgmt = SWAP16(*(USHORT *)pMgmt);

			/* swap 16 bit fields - Auth Seq No. field */
			*(USHORT *)(pMgmt + 2) = SWAP16(*(USHORT *)(pMgmt + 2));

			/* swap 16 bit fields - Status Code field */
			*(USHORT *)(pMgmt + 4) = SWAP16(*(USHORT *)(pMgmt + 4));
#endif /* RT_BIG_ENDIAN */

			DBGPRINT(RT_DEBUG_TRACE, ("Decrypt AUTH seq#3 successfully\n"));

			/* Update the total length */
			pRxBlk->DataSize -= (LEN_WEP_IV_HDR + LEN_ICV);
		}
	}
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#if defined(RT30xx) || defined(MT7601)
#ifdef ANT_DIVERSITY_SUPPORT
		RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;
#endif /* ANT_DIVERSITY_SUPPORT */
#endif /* defined(RT30xx) || defined(MT7601) */

		op_mode = OPMODE_STA;

#if defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
		/* CFG_TODO */
		op_mode = pRxBlk->OpMode;
#endif /* RT_CFG80211_P2P_SUPPORT */

		/* check if need to resend PS Poll when received packet with MoreData = 1 */
		if ((RtmpPktPmBitCheck(pAd) == TRUE) && (pHeader->FC.MoreData == 1)) {
			/* for UAPSD, all management frames will be VO priority */
			if (pAd->CommonCfg.bAPSDAC_VO == 0) {
				/* non-UAPSD delivery-enabled AC */
				RTMP_PS_POLL_ENQUEUE(pAd);
			}
		}

		/* TODO: if MoreData == 0, station can go to sleep */

		/* We should collect RSSI not only U2M data but also my beacon */
		if ((pHeader->FC.SubType == SUBTYPE_BEACON)
		    && (MAC_ADDR_EQUAL(&pAd->CommonCfg.Bssid, &pHeader->Addr2)))
		{
			if (pAd->RxAnt.EvaluatePeriod == 0)
			{
				Update_Rssi_Sample(pAd, &pAd->StaCfg.RssiSample, &pRxBlk->rx_signal, pRxBlk->rx_rate.field.MODE, pRxBlk->rx_rate.field.BW);

				pAd->StaCfg.wdev.LastSNR0 = (UCHAR) (pRxBlk->rx_signal.raw_snr[0]);
				pAd->StaCfg.wdev.LastSNR1 = (UCHAR) (pRxBlk->rx_signal.raw_snr[1]);
#ifdef DOT11N_SS3_SUPPORT
				pAd->StaCfg.wdev.LastSNR2 = (UCHAR) (pRxBlk->rx_signal.raw_snr[2]);
#endif /* DOT11N_SS3_SUPPORT */

#ifdef PRE_ANT_SWITCH
#if defined (RT2883) || defined (RT3883)
				if (IS_RT2883(pAd) || IS_RT3883(pAd))
					STASelectPktDetAntenna(pAd);
#endif /* defined (RT2883) || defined (RT3883) */
#endif /* PRE_ANT_SWITCH */
			}

#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
			if (pAd->chipCap.FreqCalibrationSupport)
			{
				if ((pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration == TRUE)
					&& (INFRA_ON(pAd))
					&& (pHeader->FC.Type == FC_TYPE_MGMT)
			    		&& (pHeader->FC.SubType == SUBTYPE_BEACON)
			    		&& (MAC_ADDR_EQUAL(&pAd->CommonCfg.Bssid, &pHeader->Addr2)))
				{
					pAd->FreqCalibrationCtrl.LatestFreqOffsetOverBeacon = GetFrequencyOffset(pAd, pRxBlk);
					pAd->FreqCalibrationCtrl.BeaconPhyMode = (UCHAR)(pRxBlk->rx_rate.field.MODE);

					DBGPRINT(RT_DEBUG_INFO,
						 ("%s: Beacon, Seq=%d, SA=%02x:%02x:%02x:%02x:%02x:%02x, freqOffset=%d, MCS=%d, BW=%d, PHYMODE=%d\n",
						  __FUNCTION__,
						  pHeader->Sequence, PRINT_MAC(pHeader->Addr2),
					  	((CHAR) (pRxBlk->rx_signal.freq_offset)),
					  	pRxBlk->rx_rate.field.MCS,
					  	pRxBlk->rx_rate.field.BW,
					  	pRxBlk->rx_rate.field.MODE));
				}
			}
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
		}

		if ((pHeader->FC.SubType == SUBTYPE_BEACON) && (ADHOC_ON(pAd)) && pEntry)
		{
			Update_Rssi_Sample(pAd, &pEntry->RssiSample, &pRxBlk->rx_signal, pRxBlk->rx_rate.field.MODE, pRxBlk->rx_rate.field.BW);
		}


#if defined(RT30xx) || defined(MT7601)
#ifdef ANT_DIVERSITY_SUPPORT
		/* collect rssi information for antenna diversity */
		if ((pAd->CommonCfg.bSWRxAntDiversity) &&
			(pAd->CommonCfg.RxAntDiversityCfg != ANT_DIVERSITY_DISABLE))
		{
			if ((pRxInfo->U2M)
			    || ((pHeader->FC.SubType == SUBTYPE_BEACON)
				&& (MAC_ADDR_EQUAL(&pAd->CommonCfg.Bssid, &pHeader->Addr2))))
			{
				if ((pAd->CommonCfg.RxAntDiversityCfg == ANT_HW_DIVERSITY_ENABLE) && pAd->chipCap.FlgSwBasedPPAD)
				{
					STARxCollectEvalRssi(pAd, pRxBlk, pRxInfo);	/* MT7601 SW based PPAD */
				}
				else
				{
					STA_COLLECT_RX_ANTENNA_AVERAGE_RSSI(pAd, ConvertToRssi(pAd, (struct raw_rssi_info *)(&pRxBlk->rx_signal.raw_rssi[0]), RSSI_IDX_0), 0);
					pAd->StaCfg.NumOfAvgRssiSample++;
				}
			}
		}
#endif /* ANT_DIVERSITY_SUPPORT */
#endif /* defined(RT30xx) || defined(MT7601) */
	}
#endif /* CONFIG_STA_SUPPORT */

	if (pRxBlk->DataSize > MAX_RX_PKT_LEN) {
		DBGPRINT(RT_DEBUG_TRACE, ("DataSize=%d\n", pRxBlk->DataSize));
		hex_dump("MGMT ???", (UCHAR *)pHeader, pRxBlk->pData - (UCHAR *) pHeader);
		goto done;
	}

#if defined(CONFIG_AP_SUPPORT) || defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
	if (pEntry && (pHeader->FC.SubType == SUBTYPE_ACTION))
	{
		/* only PM bit of ACTION frame can be set */
		if (((op_mode == OPMODE_AP) && IS_ENTRY_CLIENT(pEntry)) ||
			((op_mode == OPMODE_STA) && (IS_ENTRY_TDLS(pEntry))))
			RtmpPsIndicate(pAd, pHeader->Addr2, pRxBlk->wcid
							, (UCHAR)pHeader->FC.PwrMgmt);

		/*
			In IEEE802.11, 11.2.1.1 STA Power Management modes,
			The Power Managment bit shall not be set in any management
			frame, except an Action frame.

			In IEEE802.11e, 11.2.1.4 Power management with APSD,
			If there is no unscheduled SP in progress, the unscheduled SP
			begins when the QAP receives a trigger frame from a non-AP QSTA,
			which is a QoS data or QoS Null frame associated with an AC the
			STA has configured to be trigger-enabled.
			So a management action frame is not trigger frame.
		*/
	}
#endif /* defined(CONFIG_AP_SUPPORT) || defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */

	/* Signal in MLME_QUEUE isn't used, therefore take this item to save min SNR. */
	//if(pHeader->FC.SubType == SUBTYPE_BEACON)
		//printk("%02x:%02x:%02x:%02x:%02x:%02x=================================> pRxBlk->wcid: %d\n", PRINT_MAC(pHeader->Addr2), pRxBlk->wcid);
	/*check duplicated mgmt frame*/
	if(check_duplicated_mgmt_frame(pHeader) == TRUE)
	{
		goto done;
	}
	REPORT_MGMT_FRAME_TO_MLME(pAd, pRxBlk->wcid,
						pHeader,
						pRxBlk->DataSize,
						pRxBlk->rx_signal.raw_rssi[0],
						pRxBlk->rx_signal.raw_rssi[1],
						pRxBlk->rx_signal.raw_rssi[2],
						min(pRxBlk->rx_signal.raw_snr[0], pRxBlk->rx_signal.raw_snr[1]),
						op_mode);

#ifdef TXBF_SUPPORT
	if (pAd->chipCap.FlgHwTxBfCap)
	{
		pRxBlk->pData += LENGTH_802_11;
		pRxBlk->DataSize -= LENGTH_802_11;
		if (pHeader->FC.Order)
		{
			handleHtcField(pAd, pRxBlk);
			pRxBlk->pData += 4;
			pRxBlk->DataSize -= 4;
		}

		/* Check for compressed or non-compressed Sounding Response */
		if (((pHeader->FC.SubType == SUBTYPE_ACTION) || (pHeader->FC.SubType == SUBTYPE_ACTION_NO_ACK))
			&& (pRxBlk->pData[0] == CATEGORY_HT)
			&& ((pRxBlk->pData[1] == MIMO_N_BEACONFORM) || (pRxBlk ->pData[1] == MIMO_BEACONFORM))
		)
		{
			handleBfFb(pAd, pRxBlk);
		}
	}
#endif /* TXBF_SUPPORT */

done:

DBGPRINT(RT_DEBUG_FPGA, ("<--%s()\n", __FUNCTION__));

	RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
}


VOID dev_rx_ctrl_frm(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	PNDIS_PACKET pRxPacket = pRxBlk->pRxPacket;

	switch (pHeader->FC.SubType)
	{
#ifdef DOT11_N_SUPPORT
		case SUBTYPE_BLOCK_ACK_REQ:
			{
				FRAME_BA_REQ *bar = (FRAME_BA_REQ *)pHeader;

#ifdef MT_MAC
				if ((pAd->chipCap.hif_type == HIF_MT) &&
				    (pRxBlk->wcid == RESERVED_WCID))
				{
					MAC_TABLE_ENTRY *pEntry = MacTableLookup(pAd, &pHeader->Addr2[0]);
					if (pEntry)
						pRxBlk->wcid = pEntry->wcid;
					else {
						DBGPRINT(RT_DEBUG_ERROR, ("%s(): Cannot found WCID of BAR packet!\n",
									__FUNCTION__));
					}
				}
#endif /* MT_MAC */

				CntlEnqueueForRecv(pAd, pRxBlk->wcid, (pRxBlk->MPDUtotalByteCnt),
									(PFRAME_BA_REQ)pHeader);

				if (bar->BARControl.Compressed == 0) {
					UCHAR tid = bar->BARControl.TID;
					BARecSessionTearDown(pAd, pRxBlk->wcid, tid, FALSE);
				}
			}
			break;
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
		case SUBTYPE_PS_POLL:
			/*CFG_TODO*/
			//IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			{
				USHORT Aid = pHeader->Duration & 0x3fff;
				PUCHAR pAddr = pHeader->Addr2;
				MAC_TABLE_ENTRY *pEntry;

#ifdef MT_MAC
				if ((pAd->chipCap.hif_type == HIF_MT) &&
				    (pRxBlk->wcid == RESERVED_WCID)) 
				{
					pEntry = MacTableLookup(pAd, &pHeader->Addr2[0]);
					if (pEntry)
						pRxBlk->wcid = pEntry->wcid;
					else {
						DBGPRINT(RT_DEBUG_ERROR, ("%s(): Cannot found WCID of PS-Poll packet!\n",
									__FUNCTION__));
					}
				}
#endif /* MT_MAC */


               //printk("dev_rx_ctrl_frm0 SUBTYPE_PS_POLL pRxBlk->wcid: %x pEntry->wcid:%x\n",pRxBlk->wcid,pEntry->wcid);
				if (pRxBlk->wcid < MAX_LEN_OF_MAC_TABLE) {
                                 //printk("dev_rx_ctrl_frm1 SUBTYPE_PS_POLL\n");
					pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
					if (pEntry->Aid == Aid)
						RtmpHandleRxPsPoll(pAd, pAddr, pRxBlk->wcid, FALSE);
					else {
						DBGPRINT(RT_DEBUG_ERROR, ("%s(): Aid mismatch(pkt:%d, Entry:%d)!\n",
									__FUNCTION__, Aid, pEntry->Aid));
					}
				}
			}
			break;
#endif /* CONFIG_AP_SUPPORT */

#ifdef WFA_VHT_PF
		case SUBTYPE_RTS:
			if (pAd->CommonCfg.vht_bw_signal && pRxBlk->wcid <= MAX_LEN_OF_MAC_TABLE)
			{
				PLCP_SERVICE_FIELD *srv_field;
				RTS_FRAME *rts = (RTS_FRAME *)pRxBlk->pHeader;

				if ((rts->Addr1[0] & 0x1) == 0x1) {
					srv_field = (PLCP_SERVICE_FIELD *)&pRxBlk->pRxWI->RXWI_N.bbp_rxinfo[15];
					if (srv_field->dyn_bw == 1) {
						DBGPRINT(RT_DEBUG_TRACE, ("%02x:%02x:%02x:%02x:%02x:%02x, WCID:%d, DYN,BW=%d\n",
									PRINT_MAC(rts->Addr1), pRxBlk->wcid, srv_field->cbw_in_non_ht));
					}
				}
			}
			break;

		case SUBTYPE_CTS:
			break;
#endif /* WFA_VHT_PF */

#ifdef DOT11_N_SUPPORT
		case SUBTYPE_BLOCK_ACK:
//+++Add by shiang for debug
// TODO: shiang-MT7603, remove this!
			{
				UCHAR *ptr, *ra, *ta;
				BA_CONTROL *ba_ctrl;
				DBGPRINT(RT_DEBUG_OFF, ("%s():BlockAck From WCID:%d\n", __FUNCTION__, pRxBlk->wcid));

				ptr = (PUCHAR)pRxBlk->pHeader;
				ptr += 4;
				ra = ptr;
				ptr += 6;
				ta = ptr;
				ptr += 6;
				ba_ctrl = (BA_CONTROL *)ptr;
				ptr += sizeof(BA_CONTROL);
				DBGPRINT(RT_DEBUG_OFF, ("\tRA=%02x:%02x:%02x:%02x:%02x:%02x, TA=%02x:%02x:%02x:%02x:%02x:%02x\n",
							PRINT_MAC(ra), PRINT_MAC(ta)));
				DBGPRINT(RT_DEBUG_OFF, ("\tBA Control: AckPolicy=%d, MTID=%d, Compressed=%d, TID_INFO=0x%x\n",
							ba_ctrl->ACKPolicy, ba_ctrl->MTID, ba_ctrl->Compressed, ba_ctrl->TID));
				if (ba_ctrl->ACKPolicy == 0 && ba_ctrl->Compressed == 1) {
					BASEQ_CONTROL *ba_seq;
					ba_seq = (BASEQ_CONTROL *)ptr;
					DBGPRINT(RT_DEBUG_OFF, ("\tBA StartingSeqCtrl:StartSeq=%d, FragNum=%d\n",
									ba_seq->field.StartSeq, ba_seq->field.FragNum));
					ptr += sizeof(BASEQ_CONTROL);
					DBGPRINT(RT_DEBUG_OFF, ("\tBA Bitmap:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
								*ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5), *(ptr+6), *(ptr+7)));
				}
			}
//---Add by shiang for debug
#endif /* DOT11_N_SUPPORT */
		case SUBTYPE_ACK:
		default:
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("ignore CNTL (subtype=%d)\n",
						pHeader->FC.SubType));
#endif /* RELEASE_EXCLUDE */
			break;
	}

	RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
}

#ifdef CONFIG_STA_SUPPORT
static VOID HandleMicErrorEvent(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	CIPHER_KEY *pWpaKey = &pAd->SharedKey[BSS0][pRxBlk->key_idx];

#ifdef WPA_SUPPLICANT_SUPPORT
        if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP)
        {       
		PHEADER_802_11 pHeader = pRxBlk->pHeader;
        	WpaSendMicFailureToWpaSupplicant(pAd->net_dev, pHeader->Addr2,
                                                (pWpaKey->Type == PAIRWISEKEY) ? TRUE : FALSE,
                                                (INT) pRxBlk->key_idx, NULL);
         }
         else
#endif /* WPA_SUPPLICANT_SUPPORT */
         	RTMPReportMicError(pAd, pWpaKey);

         RTMPSendWirelessEvent(pAd, IW_MIC_ERROR_EVENT_FLAG,
         			pAd->MacTab.Content[BSSID_WCID].Addr,
                                BSS0, 0);

}
#endif /* CONFIG_STA_SUPPORT */

/*
	========================================================================
	Routine Description:
		Check Rx descriptor, return NDIS_STATUS_FAILURE if any error found
	========================================================================
*/
static INT rtmp_chk_rx_err(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk, HEADER_802_11 *pHdr)
{
	RXINFO_STRUC *pRxInfo;
	if((pRxBlk == NULL) || (pRxBlk->pRxInfo == NULL))
		return NDIS_STATUS_FAILURE;

	pRxInfo = pRxBlk->pRxInfo;
	
#ifdef MT_MAC
	// TODO: shiang-MT7603
	if (pAd->chipCap.hif_type == HIF_MT) {
//+++Add by shiang for work-around, should remove it once we correctly configure the BSSID!
		// TODO: shiang-MT7603 work around!!
		RXD_BASE_STRUCT *rxd_base = (RXD_BASE_STRUCT *)pRxBlk->rmac_info;

		if (rxd_base->rxd_2.icv_err) {
			DBGPRINT(RT_DEBUG_OFF, ("ICV Error\n"));
			dump_rxblk(pAd, pRxBlk);
			return NDIS_STATUS_FAILURE;
		}
		if (rxd_base->rxd_2.cm && !rxd_base->rxd_2.null_frm && !rxd_base->rxd_2.ndata) {
			DBGPRINT(RT_DEBUG_INFO, ("CM\n"));
			//dump_rxblk(pAd, pRxBlk);
			//hex_dump("CMPkt",  (UCHAR *)rxd_base, rxd_base->rxd_0.rx_byte_cnt);
			return NDIS_STATUS_SUCCESS;
		}
		if (rxd_base->rxd_2.clm) {
			DBGPRINT(RT_DEBUG_OFF, ("CM Length Error\n"));
			return NDIS_STATUS_FAILURE;
		}
		if (rxd_base->rxd_2.tkip_mic_err) {
			DBGPRINT(RT_DEBUG_OFF, ("TKIP MIC Error\n"));
#ifdef CONFIG_STA_SUPPORT
			if (INFRA_ON(pAd))
				HandleMicErrorEvent(pAd, pRxBlk);
			return NDIS_STATUS_FAILURE;
#endif /* CONFIG_STA_SUPPORT */
		}
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			pRxBlk->pRxInfo->MyBss = 1;
#endif /* CONFIG_STA_SUPPORT */
	}
#endif /* MT_MAC */

	

	/* Phy errors & CRC errors*/
	if (pRxInfo->Crc) {
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			INT dBm = (pRxBlk->rx_signal.raw_rssi[0]) - pAd->BbpRssiToDbmDelta;

			/* Check RSSI for Noise Hist statistic collection.*/
			if (dBm <= -87)
				pAd->StaCfg.RPIDensity[0] += 1;
			else if (dBm <= -82)
				pAd->StaCfg.RPIDensity[1] += 1;
			else if (dBm <= -77)
				pAd->StaCfg.RPIDensity[2] += 1;
			else if (dBm <= -72)
				pAd->StaCfg.RPIDensity[3] += 1;
			else if (dBm <= -67)
				pAd->StaCfg.RPIDensity[4] += 1;
			else if (dBm <= -62)
				pAd->StaCfg.RPIDensity[5] += 1;
			else if (dBm <= -57)
				pAd->StaCfg.RPIDensity[6] += 1;
			else if (dBm > -57)
				pAd->StaCfg.RPIDensity[7] += 1;
		}
#endif /* CONFIG_STA_SUPPORT */

		return NDIS_STATUS_FAILURE;
	}

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Drop ToDs promiscous frame, it is opened due to CCX 2 channel load statistics*/
		if ((pRxBlk->DataSize < 14) || (pRxBlk->MPDUtotalByteCnt > MAX_AGGREGATION_SIZE))
		{
			/*
				min_len: CTS/ACK frame len = 10, but usually we filter it in HW,
						so here we drop packet with length < 14 Bytes.

				max_len:  Paul 04-03 for OFDM Rx length issue
			*/
			DBGPRINT_RAW(RT_DEBUG_ERROR, ("rx pkt len err(%d, %d)\n",
							pRxBlk->DataSize, pRxBlk->MPDUtotalByteCnt));
			return NDIS_STATUS_FAILURE;
		}

		if (pHdr)
		{
#ifndef CLIENT_WDS
			if (pHdr->FC.ToDs
#ifdef MESH_SUPPORT
				&& !MESH_ON(pAd)
#endif /* MESH_SUPPORT */
#ifdef RT_CFG80211_P2P_SUPPORT
				//CFG TODO
				/*&& !IS_ENTRY_CLIENT(pEntry)*/ && 0 
#endif /* RT_CFG80211_P2P_SUPPORT */
			)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s: Line(%d) (ToDs Packet not allow in STA Mode)\n", __FUNCTION__, __LINE__));
				return NDIS_STATUS_FAILURE;
			}
#endif /* CLIENT_WDS */
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	/* drop decyption fail frame*/
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (pRxInfo->CipherErr)
		{
			/*
				WCID equal to 255 mean MAC couldn't find any matched entry in Asic-MAC table.
				The incoming packet mays come from WDS or AP-Client link.
				We need them for further process. Can't drop the packet here.
			*/
			if ((pRxInfo->U2M) && (pRxBlk->wcid == 255)
#ifdef WDS_SUPPORT
				&& (pAd->WdsTab.Mode == WDS_LAZY_MODE)
#endif /* WDS_SUPPORT */
			)
				return NDIS_STATUS_SUCCESS;

			APRxErrorHandle(pAd, pRxBlk);

			/* Increase received error packet counter per BSS */
			if (pHdr->FC.FrDs == 0 &&
				pRxInfo->U2M &&
				pRxBlk->bss_idx < pAd->ApCfg.BssidNum)
			{
				BSS_STRUCT *pMbss = &pAd->ApCfg.MBSSID[pRxBlk->bss_idx];
				pMbss->RxDropCount ++;
				pMbss->RxErrorCount ++;
			}

#ifdef WDS_SUPPORT
#ifdef STATS_COUNT_SUPPORT
			if ((pHdr->FC.FrDs == 1) && (pHdr->FC.ToDs == 1) &&
				(pRxBlk->wcid <MAX_LEN_OF_MAC_TABLE))
			{
				MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[pRxBlk->wcid];

				if (IS_ENTRY_WDS(pEntry) && (pEntry->func_tb_idx < MAX_WDS_ENTRY))
					pAd->WdsTab.WdsEntry[pEntry->func_tb_idx].WdsCounter.RxErrorCount++;
			}
#endif /* STATS_COUNT_SUPPORT */
#endif /* WDS_SUPPORT */

			DBGPRINT(RT_DEBUG_INFO, ("%s(): pRxInfo:Crc=%d, CipherErr=%d, U2M=%d, Wcid=%d\n",
						__FUNCTION__, pRxInfo->Crc, pRxInfo->CipherErr, pRxInfo->U2M, pRxBlk->wcid));
			return NDIS_STATUS_FAILURE;
		}
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pRxInfo->Decrypted && pRxInfo->CipherErr)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT_RAW(RT_DEBUG_INFO,("CIPHER error = %d\n",pRxInfo->CipherErr));
#endif /* RELEASE_EXCLUDE */

			if (pRxInfo->CipherErr == 2)
				{DBGPRINT_RAW(RT_DEBUG_TRACE,("RxErr: ICV ok but MICErr"));}
			else if (pRxInfo->CipherErr == 1)
				{DBGPRINT_RAW(RT_DEBUG_TRACE,("RxErr: ICV Err"));}
			else if (pRxInfo->CipherErr == 3)
				DBGPRINT_RAW(RT_DEBUG_TRACE,("RxErr: Key not valid"));

			if (INFRA_ON(pAd) && pRxInfo->MyBss) {
				if ((pRxInfo->CipherErr & 1) == 1) {
					RTMPSendWirelessEvent(pAd, IW_ICV_ERROR_EVENT_FLAG,
										pAd->MacTab.Content[BSSID_WCID].Addr,
										BSS0, 0);
				}

				/* MIC Error*/
				if (pRxInfo->CipherErr == 2) {
					HandleMicErrorEvent(pAd, pRxBlk);
				}
			}

			DBGPRINT_RAW(RT_DEBUG_TRACE,("%s(): %d (len=%d, Mcast=%d, MyBss=%d, Wcid=%d, KeyId=%d)\n",
							__FUNCTION__, pRxInfo->CipherErr, pRxBlk->MPDUtotalByteCnt,
							pRxInfo->Mcast | pRxInfo->Bcast, pRxInfo->MyBss, pRxBlk->wcid,
							pRxBlk->key_idx));
#ifdef DBG
			dump_rxinfo(pAd, pRxInfo);
			dump_rmac_info(pAd, (UCHAR *)pRxBlk->pRxWI);
			if(pHdr)
			{
				hex_dump("ErrorPkt",  (UCHAR *)pHdr, pRxBlk->MPDUtotalByteCnt);
			}
#endif /* DBG */

			if (pHdr == NULL)
				return NDIS_STATUS_SUCCESS;

			return NDIS_STATUS_FAILURE;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	return NDIS_STATUS_SUCCESS;
}


BOOLEAN dev_rx_no_foward(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	return TRUE;
}


#ifdef CONFIG_ATE
INT ate_rx_done_handle(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{

#if defined(CONFIG_QA) || defined(HUAWEI_ATE)
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
#endif
#ifdef CONFIG_QA
	RXWI_STRUC *pRxWI = pRxBlk->pRxWI;
	RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;
#endif /* CONFIG_QA */
	ATE_CTRL *ATECtrl = &pAd->ATECtrl;
	ATE_OPERATION *ATEOp = ATECtrl->ATEOp;

#ifdef HUAWEI_ATE
	if (pAd->ate.bMACFilterEnable == TRUE)
	{
		if ((MAC_ADDR_EQUAL(pHeader->Addr1, pAd->ate.Filter))
			&& (MAC_ADDR_EQUAL(pHeader->Addr2, pAd->ate.Filter))
			&& (MAC_ADDR_EQUAL(pHeader->Addr3, pAd->ate.Filter)))
		{
			INC_COUNTER64(pAd->WlanCounters.ReceivedFragmentCount);
		}
	}
	else
#endif /* !HUAWEI_ATE */
		INC_COUNTER64(pAd->WlanCounters.ReceivedFragmentCount);

	ATECtrl->RxTotalCnt++;
	ATEOp->SampleRssi(pAd, pRxBlk);

#ifdef CONFIG_QA
	if ((ATECtrl->bQARxStart == TRUE) || (ATECtrl->Mode & ATE_RXFRAME))
	{
		/* GetPacketFromRxRing() has copy the endian-changed RxD if it is necessary. */
		ATE_QA_Statistics(pAd, pRxWI, pRxInfo, pHeader);
	}

#ifdef TXBF_SUPPORT
	/* Check sounding frame */
	if (pAd->chipCap.FlgHwTxBfCap)
	{
		pRxBlk->pData += LENGTH_802_11;
		pRxBlk->DataSize -= LENGTH_802_11;

		if (pHeader->FC.Type == FC_TYPE_MGMT)
		{

			if (pHeader->FC.Order) {
				pRxBlk->pData += 4;
				pRxBlk->DataSize -= 4;
			}

			if ((((pHeader->FC.SubType == SUBTYPE_ACTION) || (pHeader->FC.SubType == SUBTYPE_ACTION_NO_ACK))
				&&  (pRxBlk ->pData)[ 0] == CATEGORY_HT
				&&  ((pRxBlk ->pData)[ 1] == MIMO_N_BEACONFORM /*non-compressed beamforming report */
				|| (pRxBlk ->pData)[1] == MIMO_BEACONFORM)  )) /*compressed beamforming report */
			{
				/* sounding frame */
				if (pAd->ate.sounding == 1) {
					int i, Nc = ((pRxBlk ->pData)[2] & 0x3) + 1;
					pAd->ate.soundingSNR[0] = (CHAR)((pRxBlk ->pData)[8]);
					pAd->ate.soundingSNR[1] = (Nc<2)? 0: (CHAR)((pRxBlk ->pData)[9]);
					pAd->ate.soundingSNR[2] = (Nc<3)? 0: (CHAR)((pRxBlk ->pData)[10]);
					pAd->ate.sounding = 2;
					pAd->ate.soundingRespSize = pRxBlk->DataSize;
					for (i=0; i<pRxBlk->DataSize && i<MAX_SOUNDING_RESPONSE_SIZE; i++)
						pAd->ate.soundingResp[i] = pRxBlk->pData[i];
				}
			}
			/* Roger Debug : Fix Me */
			else
			{
				if (pHeader->FC.Order)
					DBGPRINT( RT_DEBUG_WARN, ("fcsubtype=%x\ndata[0]=%x\ndata[1]=%x\n",
							pHeader->FC.SubType, (pRxBlk ->pData)[0], (pRxBlk ->pData)[1]));
			}
		}
		else
		{
			if (pHeader->FC.Order)
			{
				PATE_INFO pATEInfo = &(pAd->ate);
				UCHAR CSI_Steering, NDP_Announce;

				//pRxBlk->pData
				//B0          B15 | B16     B17 | B18     B19 | B20  B21 | B22  B23 |        B24
				//Link Adaption |Calibration   |Calibration  |Reerved   |CSI/         |NDP
				//   Control       |Poition        |Sequence    |              |Steering    |Announcement
				pRxBlk->pData += 2;
				pRxBlk->DataSize -= 2;

				CSI_Steering = (pRxBlk->pData)[0] >> 6;
				NDP_Announce = (pRxBlk->pData)[1] & 0x1;

				DBGPRINT(RT_DEBUG_OFF, ("================== Sounding request info ==================\n"
					"HTC low byte    = 0x%x\n"
					"HTC high byte   = 0x%x\n"
					"CSI_Steering    = %d\n"
					"NDP Anouncement = %d\n",
					(pRxBlk->pData)[0], (pRxBlk->pData)[1],
					CSI_Steering, NDP_Announce));

				/* If sounding request packet i detected, send a null packet to AP immediately */
				COPY_MAC_ADDR(pATEInfo->Addr1, pHeader->Addr2);
				COPY_MAC_ADDR(pATEInfo->Addr2, pHeader->Addr1);
				COPY_MAC_ADDR(pATEInfo->Addr3, pAd->CommonCfg.Bssid);

				DBGPRINT(RT_DEBUG_OFF, ("=================== APSendNullFrame ===================\n"
		   			"From AP : \n"
		   			"pATEInfo->Addr1 = %x:%x:%x:%x:%x:%x\n"
		   			"pATEInfo->Addr2 = %x:%x:%x:%x:%x:%x\n"
		   			"pATEInfo->Addr3 = %x:%x:%x:%x:%x:%x\n"
					"=======================================================\n",
					pATEInfo->Addr1[0], pATEInfo->Addr1[1], pATEInfo->Addr1[2],
					pATEInfo->Addr1[3], pATEInfo->Addr1[4], pATEInfo->Addr1[5],
					pATEInfo->Addr2[0], pATEInfo->Addr2[2], pATEInfo->Addr2[2],
					pATEInfo->Addr2[3], pATEInfo->Addr2[4], pATEInfo->Addr2[5],
					pATEInfo->Addr3[0], pATEInfo->Addr3[1], pATEInfo->Addr3[2],
					pATEInfo->Addr3[3], pATEInfo->Addr3[4], pATEInfo->Addr3[5]));

				pATEInfo->TxCount= 1;
				pATEInfo->TxPower0 = 18;
				pATEInfo->TxPower1 = 18;
				pATEInfo->TxLength = 100;
				Set_ATE_Proc(pAd, "TXFRAME");
				Set_ATE_Proc(pAd, "RXFRAME");

				if (CSI_Steering >= 2 && NDP_Announce == TRUE)
					DBGPRINT(RT_DEBUG_OFF, ("================== Sounding request is found ==================\n"));
			}
		}
	}
#endif /* TXBF_SUPPORT */
#endif /* CONFIG_QA */

	return TRUE;
}
#endif /* CONFIG_ATE */


#ifdef MESH_SUPPORT
INT mesh_rx_foward_handle(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, PNDIS_PACKET pPacket)
{
	BOOLEAN bAnnounce, bDirectForward;
	PNDIS_PACKET pMeshForwardPacket = NULL;
	PNDIS_PACKET pForwardPacket = NULL;
	UCHAR *pHeader802_3;
	LONG RouteId;

	if (wdev->wdev_type != WDEV_TYPE_MESH)
		return FALSE;

	ASSERT(wdev->func_idx < MAX_MESH_LINKS);
	if (wdev->func_idx >= MAX_MESH_LINKS)
		return FALSE;

	pHeader802_3 = GET_OS_PKT_DATAPTR(pPacket);
	MeshDataPktProcess(pAd, pPacket, wdev->func_idx,
						&pMeshForwardPacket, &bDirectForward, &bAnnounce);

	if (bDirectForward)
	{
		if ((bDirectForward == TRUE) && (bAnnounce == TRUE))
			pForwardPacket = pMeshForwardPacket;
		else
			pForwardPacket = DuplicatePacket(wdev->if_dev, pPacket);

		if (pForwardPacket == NULL)
			return bAnnounce;

		RouteId = PathRouteIDSearch(pAd, pHeader802_3);
		if (RouteId == BMCAST_ROUTE_ID)
		{
			MeshClonePacket(pAd, pForwardPacket, MESH_FORWARD, wdev->func_idx);
			RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_SUCCESS);
		}
		else if (RouteId >= 0)
		{
			INT LinkId;

			LinkId = PathMeshLinkIDSearch(pAd, RouteId);
			if ((LinkId >= 0) && VALID_MESH_LINK_ID(LinkId))
			{
				RTMP_SET_PACKET_WDEV(pForwardPacket, pAd->MeshTab.wdev.wdev_idx);
				RTMP_SET_PACKET_MOREDATA(pForwardPacket, FALSE);
				RTMP_SET_PACKET_WCID(pForwardPacket,
										pAd->MeshTab.MeshLink[LinkId].Entry.MacTabMatchWCID);
				RTMP_SET_MESH_ROUTE_ID(pForwardPacket, RouteId);
				RTMP_SET_MESH_SOURCE(pForwardPacket, MESH_FORWARD);
				// TODO: shiang-usw, unify these two functions here!
				if (wdev->tx_pkt_handle)
					wdev->tx_pkt_handle(pAd, pForwardPacket);
				else
				{
					DBGPRINT(RT_DEBUG_ERROR, ("%s(): tx_pkt_handle not assigned!\n",
								__FUNCTION__));
					RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_FAILURE);
				}
			}
			else
			{
				RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_FAILURE);
			}
		}
		else
		{
			/* entity is not exist. start path discovery. */
			MeshCreatePreqAction(pAd, NULL, pHeader802_3);

			RELEASE_NDIS_PACKET(pAd, pForwardPacket, NDIS_STATUS_FAILURE);
		}

		RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
	}

	return bAnnounce;
}


INT mesh_rx_pkt_allow(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	FRAME_CONTROL *pFmeCtrl = &pHeader->FC;
	MAC_TABLE_ENTRY *pEntry = NULL;
	INT hdr_len = 0;

	if (VALID_WCID(pRxBlk->wcid) {
		pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
		if (!IS_ENTRY_MESH(pEntry))
			pEntry = NULL;
	}

	if ((pFmeCtrl->FrDs == 1) && (pFmeCtrl->ToDs == 1))
	{
		/* handle MESH */
		if (pEntry)
		{
			pEntry = FindMeshEntry(pAd, pRxBlk->wcid, pHeader->Addr2);
			/* No valid mesh entry exist, so discard the incoming packet. */
			if (pEntry && MESH_ON(pAd))
			{
				RX_BLK_SET_FLAG(pRxBlk, fRX_MESH);
				hdr_len = LENGTH_802_11_WITH_ADDR4;
				*wdev_idx = pEntry->wdev->wdev_idx + MIN_NET_DEVICE_FOR_MESH;
			} else
				pEntry = NULL;
		}

		if (MeshLinkLookUp(pAd, pHeader->Addr2) != NULL)
			pEntry = NULL; /* Drop all packets for invaild Mesh Link. */
	}

	if (pEntry) {
#ifdef CONFIG_STA_SUPPORT
		/*
		   When the mesh-IF up, the STA own address would be set as my_bssid address.
		   If receiving an "encrypted" broadcast packet(its WEP bit as 1) and doesn't match my BSSID,
		   Asic pass to driver with "Decrypted" marked as 0 in pRxInfo.
		   The condition is below,
		   1. mesh IF is ON,
		   2. the addr2 of the received packet is STA's BSSID,
		   3. broadcast packet,
		   4. from DS packet,
		   5. Asic pass this packet to driver with "pRxInfo->Decrypted=0"
		 */
		if ((MAC_ADDR_EQUAL(pAd->CommonCfg.Bssid, pHeader->Addr2))
		    && (pRxInfo->Bcast || pRxInfo->Mcast)
		    && (pHeader->FC.FrDs == 1)
		    && (pHeader->FC.ToDs == 0)
		    && (pRxInfo->Decrypted == 0))
		{
			/* set this m-cast frame is my-bss. */
			hdr_len = LENGTH_802_11;
			pRxInfo->MyBss = 1;
		}
#endif /* CONFIG_STA_SUPPORT */

		return hdr_len;
	}
	else
		return FALSE;
}
#endif /* MESH_SUPPORT */


#if defined(CONFIG_STA_SUPPORT) || defined(APCLI_SUPPORT)
INT sta_rx_fwd_hnd(RTMP_ADAPTER *pAd, struct wifi_dev *wdev, PNDIS_PACKET pPacket)
{
	/*
		For STA, direct to OS and no need to forwad the packet to WM
	*/
	return TRUE; /* need annouce to upper layer */
}


INT sta_rx_pkt_allow(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	FRAME_CONTROL *pFmeCtrl = &pHeader->FC;
	MAC_TABLE_ENTRY *pEntry = NULL;
	INT hdr_len = FALSE;
	struct wifi_dev *wdev;

DBGPRINT(RT_DEBUG_INFO, ("-->%s():pRxBlk->wcid=%d\n", __FUNCTION__, pRxBlk->wcid));

	pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
	wdev = pEntry->wdev;

	if (wdev == NULL)
		DBGPRINT(RT_DEBUG_TRACE, ("wdev is NULL.\n"));

#ifdef CLIENT_WDS
	if ((pFmeCtrl->FrDs == 1) && (pFmeCtrl->ToDs == 1))
	{
		if ((pRxBlk->wcid < MAX_LEN_OF_MAC_TABLE)
			&& IS_ENTRY_CLIENT(pEntry))
		{
			RX_BLK_SET_FLAG(pRxBlk, fRX_WDS);
			hdr_len = LENGTH_802_11_WITH_ADDR4;
			pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
		}
	}
#endif /* CLIENT_WDS */

#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11V_WNM_SUPPORT
		RTMP_SET_WNM_DMS(pRxBlk->pRxPacket, 0);
		IS_WNM_DMS(pAd, pRxBlk->pRxPacket, pHeader);
#endif /* DOT11V_WNM_SUPPORT */
#ifdef QOS_DLS_SUPPORT
	if (RTMPRcvFrameDLSCheck(pAd, pHeader, pRxBlk->MPDUtotalByteCnt, pRxInfo)) {
DBGPRINT(RT_DEBUG_OFF, ("%s(): RTMPRcvFrameDLSCheck failed!pRxBlk->MPDUtotalByteCnt=%d\n",
			__FUNCTION__, pRxBlk->MPDUtotalByteCnt));
		return FALSE;
	}
#endif /* QOS_DLS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	ASSERT((pEntry != NULL));

	/* Drop not my BSS frames */
	if (pRxInfo->MyBss == 0) {
/* CFG_TODO: NEED CHECK for MT_MAC */	
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT)
		/* When the p2p-IF up, the STA own address would be set as my_bssid address.
		   If receiving an "encrypted" broadcast packet(its WEP bit as 1) and doesn't match my BSSID,
		   Asic pass to driver with "Decrypted" marked as 0 in pRxInfo.
		   The condition is below,
		   1. p2p IF is ON,
		   2. the addr2 of the received packet is STA's BSSID,
		   3. broadcast packet,
		   4. from DS packet,
		   5. Asic pass this packet to driver with "pRxInfo->Decrypted=0"
		 */
		 if (
#ifdef RT_CFG80211_P2P_SUPPORT
             TRUE /* The dummy device always present for CFG80211 application*/
#else
             (P2P_INF_ON(pAd))
#endif /* RT_CFG80211_P2P_SUPPORT */
			&& (MAC_ADDR_EQUAL(pAd->CommonCfg.Bssid, pHeader->Addr2)) &&
			(pRxInfo->Bcast || pRxInfo->Mcast) &&
			(pFmeCtrl->FrDs == 1) &&
			(pFmeCtrl->ToDs == 0) &&
			(pRxInfo->Decrypted == 0))
		{
			/* set this m-cast frame is my-bss. */
			pRxInfo->MyBss = 1;
		}
		else
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT */
		{
DBGPRINT(RT_DEBUG_OFF, ("%s():  Not my bss! pRxInfo->MyBss=%d\n", __FUNCTION__, pRxInfo->MyBss));
			return FALSE;
		}
	}

#ifdef RT3290
	// TODO: shiang, find out what's this??
	if (pRxInfo->MyBss)
	{
		// TODO: shiang, I mark this line due to I still didn't know what's this yet
		//pAd->Rssi[pAd->WlanFunCtrl.field.INV_TR_SW0] = pAd->StaCfg.RssiSample.AvgRssi[0];
	}
#endif /* RT3290 */

	pAd->RalinkCounters.RxCountSinceLastNULL++;
#ifdef UAPSD_SUPPORT
	if (wdev
	    && wdev->UapsdInfo.bAPSDCapable
	    && pAd->CommonCfg.APEdcaParm.bAPSDCapable
	    && (pHeader->FC.SubType & 0x08))
	{
		UCHAR *pData;
		DBGPRINT(RT_DEBUG_INFO, ("bAPSDCapable\n"));

		/* Qos bit 4 */
		pData = (PUCHAR) pHeader + LENGTH_802_11;
		if ((*pData >> 4) & 0x01)
		{
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)

			/* ccv EOSP frame so the peer can sleep */
			if (pEntry != NULL)
			{
				RTMP_PS_VIRTUAL_SLEEP(pEntry);
			}

			if (pAd->StaCfg.FlgPsmCanNotSleep == TRUE)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("tdls uapsd> Rcv EOSP frame but we can not sleep!\n"));
			}
			else
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
				{
				DBGPRINT(RT_DEBUG_INFO,
					("RxDone- Rcv EOSP frame, driver may fall into sleep\n"));
				pAd->CommonCfg.bInServicePeriod = FALSE;

#ifdef CONFIG_STA_SUPPORT
				/* Force driver to fall into sleep mode when rcv EOSP frame */
				if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
				{
					USHORT TbttNumToNextWakeUp;
					USHORT NextDtim = pAd->StaCfg.DtimPeriod;
					ULONG Now;

					NdisGetSystemUpTime(&Now);
					NextDtim -= (USHORT) (Now - pAd->StaCfg.LastBeaconRxTime) / pAd->CommonCfg.BeaconPeriod;

					TbttNumToNextWakeUp = pAd->StaCfg.DefaultListenCount;
					if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_RECEIVE_DTIM) && (TbttNumToNextWakeUp > NextDtim))
						TbttNumToNextWakeUp = NextDtim;

					DBGPRINT(RT_DEBUG_TRACE, ("%s::Hanmin_debug_3\n", __FUNCTION__));
					RTMP_SET_PSM_BIT(pAd, PWR_SAVE);
					/* if WMM-APSD is failed, try to disable following line */

					RTEnqueueInternalCmd(pAd, CMDTHREAD_FORCE_SLEEP_AUTO_WAKEUP, NULL, 0);
				}
#endif /* CONFIG_STA_SUPPORT */
			}
		}

		if ((pHeader->FC.MoreData) && (pAd->CommonCfg.bInServicePeriod)) {
			DBGPRINT(RT_DEBUG_TRACE, ("MoreData bit=1, Sending trigger frm again\n"));
		}
	}
#endif /* UAPSD_SUPPORT */

#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
	/* 1: PWR_SAVE, 0: PWR_ACTIVE */
	if (pEntry != NULL)
	{
		UCHAR OldPwrMgmt;

		OldPwrMgmt = RtmpPsIndicate(pAd, pHeader->Addr2, pEntry->wcid
									, (UCHAR)pFmeCtrl->PwrMgmt);
#ifdef UAPSD_SUPPORT
		RTMP_PS_VIRTUAL_TIMEOUT_RESET(pEntry);

		if (pFmeCtrl->PwrMgmt)
		{
			if ((CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_APSD_CAPABLE)) &&
				(pFmeCtrl->SubType & 0x08))
			{
				/*
					In IEEE802.11e, 11.2.1.4 Power management with APSD,
					If there is no unscheduled SP in progress, the unscheduled SP begins
					when the QAP receives a trigger frame from a non-AP QSTA, which is a
					QoS data or QoS Null frame associated with an AC the STA has
					configured to be trigger-enabled.

					In WMM v1.1, A QoS Data or QoS Null frame that indicates transition
					to/from Power Save Mode is not considered to be a Trigger Frame and
					the AP shall not respond with a QoS Null frame.
				*/
				/* Trigger frame must be QoS data or QoS Null frame */
				UCHAR  OldUP;

				if ((*(pRxBlk->pData+LENGTH_802_11) & 0x10) == 0)
				{
					/* this is not a EOSP frame */
					OldUP = (*(pRxBlk->pData+LENGTH_802_11) & 0x07);
					if (OldPwrMgmt == PWR_SAVE)
					{
						//hex_dump("trigger frame", pRxBlk->pData, 26);
						UAPSD_TriggerFrameHandle(pAd, pEntry, OldUP);
					}
				}
				else
				{
					DBGPRINT(RT_DEBUG_TRACE, ("This is a EOSP frame, not a trigger frame!\n"));
				}
			}
		}
#endif /* UAPSD_SUPPORT */
	}
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */

	/* Drop NULL, CF-ACK(no data), CF-POLL(no data), and CF-ACK+CF-POLL(no data) data frame */
	if ((pFmeCtrl->SubType & 0x04)) /* bit 2 : no DATA */ {
		DBGPRINT(RT_DEBUG_OFF, ("%s():  No DATA!\n", __FUNCTION__));
		return FALSE;
}

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
    if (pEntry && IS_ENTRY_APCLI(pEntry)) {
                RX_BLK_SET_FLAG(pRxBlk, fRX_AP);
				goto ret;	
    }
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	if (pAd->StaCfg.BssType == BSS_INFRA) {
		/* Infrastructure mode, check address 2 for BSSID */
		if (1
#ifdef QOS_DLS_SUPPORT
		    && (!pAd->CommonCfg.bDLSCapable)
#endif /* QOS_DLS_SUPPORT */
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
			    && (!IS_TDLS_SUPPORT(pAd))
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */

		)
		{
			if (!RTMPEqualMemory(&pHeader->Addr2, &pAd->MlmeAux.Bssid, 6)) {
DBGPRINT(RT_DEBUG_OFF, ("%s():  Infra-No my BSSID(Peer=>%02x:%02x:%02x:%02x:%02x:%02x, My=>%02x:%02x:%02x:%02x:%02x:%02x)!\n",
			__FUNCTION__, PRINT_MAC(pHeader->Addr2), PRINT_MAC(pAd->MlmeAux.Bssid)));
				return FALSE; /* Receive frame not my BSSID */
			}
		}
	}
	else
	{	/* Ad-Hoc mode or Not associated */

		/* Ad-Hoc mode, check address 3 for BSSID */
		if (!RTMPEqualMemory(&pHeader->Addr3, &pAd->CommonCfg.Bssid, 6)) {
DBGPRINT(RT_DEBUG_OFF, ("%s():  AdHoc-No my BSSID(Peer=>%02x:%02x:%02x:%02x:%02x:%02x, My=>%02x:%02x:%02x:%02x:%02x:%02x)!\n",
			__FUNCTION__, PRINT_MAC(pHeader->Addr3), PRINT_MAC(pAd->CommonCfg.Bssid)));
			return FALSE; /* Receive frame not my BSSID */
		}
	}
#endif /*CONFIG_STA_SUPPORT */

	if (pEntry) {
#ifdef WMM_ACM_SUPPORT
		ACMP_DataNullHandle(pAd, pEntry, pHeader);
#endif /* WMM_ACM_SUPPORT */
	}

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.BssType == BSS_INFRA) 
		{
			/* infra mode */
			RX_BLK_SET_FLAG(pRxBlk, fRX_AP);
#if defined(DOT11Z_TDLS_SUPPORT) || defined(QOS_DLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
			if ((pHeader->FC.FrDs == 0) && (pHeader->FC.ToDs == 0))
				RX_BLK_SET_FLAG(pRxBlk, fRX_DLS);
			else
#endif
				ASSERT(pRxBlk->wcid == BSSID_WCID);
		}
		else
		{
			/* ad-hoc mode */
			if ((pFmeCtrl->FrDs == 0) && (pFmeCtrl->ToDs == 0))
				RX_BLK_SET_FLAG(pRxBlk, fRX_ADHOC);
		}
	}
#endif /* CONFIG_STA_SUPPORT */


#ifndef WFA_VHT_PF
	// TODO: shiang@PF#2, is this atheros protection still necessary here???
	/* check Atheros Client */
	if ((pEntry->bIAmBadAtheros == FALSE) && (pRxInfo->AMPDU == 1)
	    && (pHeader->FC.Retry)) {
		pEntry->bIAmBadAtheros = TRUE;
#ifdef CONFIG_STA_SUPPORT
		pAd->CommonCfg.IOTestParm.bLastAtheros = TRUE;
		if (!STA_AES_ON(pAd))
			RTMP_UPDATE_PROTECT(pAd, 8 , ALLN_SETPROTECT, TRUE, FALSE);
#endif /* CONFIG_STA_SUPPORT */
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("Atheros Problem. Turn on RTS/CTS!!!\n"));
#endif /* RELEASE_EXCLUDE */
	}
#endif /* WFA_VHT_PF */

ret: 
	hdr_len = LENGTH_802_11;

	return hdr_len;
}
#endif /* defined(CONFIG_STA_SUPPORT) || defined(APCLI_SUPPORT) */


VOID rx_data_frm_announce(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry,
	IN RX_BLK *pRxBlk,
	IN struct wifi_dev *wdev)
{
	BOOLEAN eth_frame = FALSE;
	UINT data_len = pRxBlk->DataSize;
	UCHAR wdev_idx = wdev->wdev_idx;
#if defined(MESH_SUPPORT) && defined(CONFIG_STA_SUPPORT)
	STA_TR_ENTRY *tr_entry;
#endif /* defined(MESH_SUPPORT) && defined(CONFIG_STA_SUPPORT) */

	ASSERT(wdev_idx < WDEV_NUM_MAX);
	if (wdev_idx >= WDEV_NUM_MAX) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Invalid wdev_idx(%d)\n", __FUNCTION__, wdev_idx));
		RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
		return;
	}

#ifdef HDR_TRANS_SUPPORT
	if (pRxBlk->bHdrRxTrans) {
		eth_frame = TRUE;
		data_len = pRxBlk->TransDataSize;
	}
#endif /* HDR_TRANS_SUPPORT */

	/* non-EAP frame */
	if (!RTMPCheckWPAframe(pAd, pEntry, pRxBlk, data_len, wdev_idx, eth_frame))
	{
#ifdef CONFIG_STA_SUPPORT
		// TODO: revise for APCLI about this checking
		if (pEntry->wdev->wdev_type == WDEV_TYPE_STA) {
			
			if (IS_ENTRY_APCLI(pEntry))
			{
				//printk("%s: RX \n", __FUNCTION__);
			}
			else /* drop all non-EAP DATA frame before peer's Port-Access-Control is secured */
			if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s:not EAP frame, drop pkt before linkup!\n", __func__));
				RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
				return;
			}

#ifdef DOT11Z_TDLS_SUPPORT
			if (TDLS_CheckTDLSframe(pAd, pRxBlk->pData, pRxBlk->DataSize)) {
				UCHAR *pTmpBuf;

				pTmpBuf = pRxBlk->pData - LENGTH_802_11;
				NdisMoveMemory(pTmpBuf, pRxBlk->pHeader, LENGTH_802_11);

				REPORT_MGMT_FRAME_TO_MLME(pAd, pRxBlk->wcid,
									pTmpBuf,
									pRxBlk->DataSize + LENGTH_802_11,
									pRxBlk->rx_signal.raw_rssi[0],
									pRxBlk->rx_signal.raw_rssi[1],
									pRxBlk->rx_signal.raw_rssi[2],
									0,
									OPMODE_STA);

				DBGPRINT_RAW(RT_DEBUG_TRACE,
					     ("!!! report TDLS Action DATA to MLME (len=%d) !!!\n",
					      pRxBlk->DataSize));

				RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket,
						    NDIS_STATUS_FAILURE);
				return;
			}
#endif /* DOT11Z_TDLS_SUPPORT */
#ifdef CFG_TDLS_SUPPORT
			cfg_tdls_rx_parsing(pAd, pRxBlk);
			//return;
#endif /* CFG_TDLS_SUPPORT */

		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef WAPI_SUPPORT
		/* report to upper layer if the received frame is WAI frame */
		if (RTMPCheckWAIframe(pRxBlk->pData, pRxBlk->DataSize)) {
			Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
			return;
		}
#endif /* WAPI_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
		/* drop all non-EAP DATA frame before peer's Port-Access-Control is secured */
		if ((pEntry->wdev->wdev_type == WDEV_TYPE_AP) &&
		    IS_ENTRY_CLIENT(pEntry) && (pEntry->PrivacyFilter == Ndis802_11PrivFilter8021xWEP))
		{
			/*
				If	1) no any EAP frame is received within 5 sec and
					2) an encrypted non-EAP frame from peer associated STA is received,
				AP would send de-authentication to this STA.
			 */
			if (pRxBlk->pHeader->FC.Wep &&
				pEntry->StaConnectTime > 5 && pEntry->WpaState < AS_AUTHENTICATION2)
			{
				DBGPRINT(RT_DEBUG_WARN, ("==> De-Auth this STA(%02x:%02x:%02x:%02x:%02x:%02x)\n",
							PRINT_MAC(pEntry->Addr)));
				MlmeDeAuthAction(pAd, pEntry, REASON_NO_LONGER_VALID, FALSE);
			}

			RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
			return;
		}
		/* To Fix security Vulnerability issue for RT MAC series */
		/* Drop Unencrypted non-EAPOL or WAI frames, such as Unencrypted DHCP pkt */
		if ((pEntry->wdev->wdev_type == WDEV_TYPE_AP) && IS_ENTRY_CLIENT(pEntry) &&
			(pEntry->WepStatus != Ndis802_11EncryptionDisabled) &&
			(pRxBlk->pHeader->FC.Wep == 0)) {
			DBGPRINT(RT_DEBUG_ERROR, ("==> De-Auth this STA(%02x:%02x:%02x:%02x:%02x:%02x) due to OPEN packet\n",
				PRINT_MAC(pEntry->Addr)));
			MlmeDeAuthAction(pAd, pEntry, REASON_UNSPECIFY, FALSE);

			RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
			return;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			NDIS_802_11_WEP_STATUS WepStatus = wdev->WepStatus;

			//CFG TODO: Apcli ??
			//ASSERT(wdev == &pAd->StaCfg.wdev);
#ifdef MESH_SUPPORT
			if (wdev->wdev_type == WDEV_TYPE_MESH) {
				tr_entry = &pAd->MacTab.tr_entry[pEntry->wcid];
				WepStatus = pEntry->WepStatus;
			}
#endif /* MESH_SUPPORT */

			/* drop all non-EAP DATA frame before peer's Port-Access-Control is secured */
			if (pRxBlk->pHeader->FC.Wep)
			{
				/* unsupported cipher suite */
				if (WepStatus == Ndis802_11EncryptionDisabled)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("%s:drop pkt as FC.Wep==1 while Ndis802_11EncryptionDisabled\n", __func__));
					RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
					return;
				}
			}
			else
			{
				/* encryption in-use but receive a non-EAPOL clear text frame, drop it */
				if (WepStatus != Ndis802_11EncryptionDisabled)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("%s:drop pkt as FC.Wep!=1 while Ndis802_11EncryptionEnabled\n", __func__));
					RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
					return;
				}
			}
		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef MESH_SUPPORT
		if (IS_ENTRY_MESH(pEntry)) {
			UINT32 MeshSeq = GetMeshSeq(pRxBlk->pData);
			PHEADER_802_11 pHeader = pRxBlk->pHeader;
			PUCHAR pMeshDA = pHeader->Addr3;
			PUCHAR pMeshSA = (PUCHAR)(pHeader->Octet);
			PUCHAR pSA = GetMeshAddr6(pRxBlk->pData);

			if (PktSigCheck(pAd, pHeader->Addr2, pMeshDA, pMeshSA, MeshSeq, FC_TYPE_DATA) == FALSE)
			{
				/* signature exist, it means it's a duplicate packet. drop it. */
				RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
				return;
			}

			if ((pMeshSA != NULL) && (pSA != NULL) && (pAd->MeshTab.OpMode & MESH_AP))
			{
				if (!MAC_ADDR_EQUAL(pMeshSA, pSA))
				{
					PMESH_PROXY_ENTRY pMeshProxyEntry = MeshProxyEntryTableLookUp(pAd, pSA);
					if (!pMeshProxyEntry)
						MeshProxyEntryTableInsert(pAd, pMeshSA, pSA);
				}
			}
		}
#endif /* MESH_SUPPORT */

#ifdef IGMP_SNOOP_SUPPORT
		if ((IS_ENTRY_CLIENT(pEntry) || IS_ENTRY_WDS(pEntry))
			&& (pAd->ApCfg.IgmpSnoopEnable)
			&& IS_MULTICAST_MAC_ADDR(pRxBlk->pHeader->Addr3))
		{
			PUCHAR pDA = pRxBlk->pHeader->Addr3;
			PUCHAR pSA = pRxBlk->pHeader->Addr2;
			PUCHAR pData = NdisEqualMemory(SNAP_802_1H, pRxBlk->pData, 6) ? (pRxBlk->pData + 6) : pRxBlk->pData;
			UINT16 protoType = OS_NTOHS(*((UINT16 *)(pData)));

			if (protoType == ETH_P_IP)
				IGMPSnooping(pAd, pDA, pSA, pData, wdev->if_dev);
			else if (protoType == ETH_P_IPV6)
				MLDSnooping(pAd, pDA, pSA,  pData, wdev->if_dev);
		}
#endif /* IGMP_SNOOP_SUPPORT */

#ifdef CONFIG_HOTSPOT
		if (pEntry->pMbss->HotSpotCtrl.HotSpotEnable) {
			if (hotspot_rx_handler(pAd, pEntry, pRxBlk) == TRUE)
				return;
		}
#endif /* CONFIG_HOTSPOT */

#ifdef CONFIG_AP_SUPPORT
#ifdef STATS_COUNT_SUPPORT
		if ((IS_ENTRY_CLIENT(pEntry)) && (pEntry->pMbss))
		{
			BSS_STRUCT *pMbss = pEntry->pMbss;
			UCHAR *pDA = pRxBlk->pHeader->Addr3;
			if (((*pDA) & 0x1) == 0x01) {
				if(IS_BROADCAST_MAC_ADDR(pDA))
					pMbss->bcPktsRx++;
				else
					pMbss->mcPktsRx++;
			} else
				pMbss->ucPktsRx++;
		}
#endif /* STATS_COUNT_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef DOT11_N_SUPPORT
		if (RX_BLK_TEST_FLAG(pRxBlk, fRX_AMPDU) /*&& (pAd->CommonCfg.bDisableReordering == 0)*/)
			Indicate_AMPDU_Packet(pAd, pRxBlk, wdev_idx);
		else if (RX_BLK_TEST_FLAG(pRxBlk, fRX_AMSDU))
			Indicate_AMSDU_Packet(pAd, pRxBlk, wdev_idx);
		else
#endif /* DOT11_N_SUPPORT */
		if (RX_BLK_TEST_FLAG(pRxBlk, fRX_ARALINK))
			Indicate_ARalink_Packet(pAd, pEntry, pRxBlk, wdev->wdev_idx);
		else
			Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
	}
	else
	{
		RX_BLK_SET_FLAG(pRxBlk, fRX_EAP);

#ifdef CONFIG_AP_SUPPORT
		/* Update the WPA STATE to indicate the EAP handshaking is started */
		if (IS_ENTRY_CLIENT(pEntry)) {
			if (pEntry->WpaState == AS_AUTHENTICATION)
			pEntry->WpaState = AS_AUTHENTICATION2;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef DOT11_N_SUPPORT
		if (RX_BLK_TEST_FLAG(pRxBlk, fRX_AMPDU)
			/*&& (pAd->CommonCfg.bDisableReordering == 0)*/)
		{
			Indicate_AMPDU_Packet(pAd, pRxBlk, wdev_idx);
		}
		else
#endif /* DOT11_N_SUPPORT */
		{
#ifdef CONFIG_HOTSPOT_R2
			UCHAR *pData = (UCHAR *)pRxBlk->pData;

			if (pEntry)
			{
				BSS_STRUCT *pMbss = pEntry->pMbss;
				if (NdisEqualMemory(SNAP_802_1H, pData, 6) ||
			        /* Cisco 1200 AP may send packet with SNAP_BRIDGE_TUNNEL*/
        			NdisEqualMemory(SNAP_BRIDGE_TUNNEL, pData, 6))
			    {
			        pData += 6;
			    }

				if (NdisEqualMemory(EAPOL, pData, 2))
    	    		pData += 2;

			    if ((*(pData+1) == EAPOLStart) && (pMbss->HotSpotCtrl.HotSpotEnable == 1) && (pMbss->wdev.AuthMode == Ndis802_11AuthModeWPA2) && (pEntry->hs_info.ppsmo_exist == 1))
				{
					UCHAR HS2_Header[4] = {0x50,0x6f,0x9a,0x12};
					memcpy(&pRxBlk->pData[pRxBlk->DataSize], HS2_Header, 4);
					memcpy(&pRxBlk->pData[pRxBlk->DataSize+4], &pEntry->hs_info, sizeof(struct _sta_hs_info));
					DBGPRINT(RT_DEBUG_OFF, ("rcv eapol start, %x:%x:%x:%x\n", pRxBlk->pData[pRxBlk->DataSize+4]
					, pRxBlk->pData[pRxBlk->DataSize+5], pRxBlk->pData[pRxBlk->DataSize+6], pRxBlk->pData[pRxBlk->DataSize+7]));
					pRxBlk->DataSize += 8;
				}
			}
#endif
			/* Determin the destination of the EAP frame */
			/*  to WPA state machine or upper layer */
			rx_eapol_frm_handle(pAd, pEntry, pRxBlk, wdev_idx);
		}
	}
}


#define SN_NQOS_INDEX 8
static INT rx_chk_duplicate_frame(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	FRAME_CONTROL *pFmeCtrl = &pHeader->FC;
	UCHAR up = pRxBlk->UserPriority;
	UCHAR wcid = pRxBlk->wcid;
	STA_TR_ENTRY *trEntry= NULL;
	INT sn = pHeader->Sequence;

	/*check if AMPDU frame ignore it, since AMPDU wil handle reorder problem and AMSDU is the set of AMPDU*/
	if(RX_BLK_TEST_FLAG(pRxBlk, fRX_AMPDU) || RX_BLK_TEST_FLAG(pRxBlk,fRX_AMSDU))
	{
		return NDIS_STATUS_SUCCESS;
	}

	/*check is vaild sta entry*/
	if(wcid >= MAX_LEN_OF_TR_TABLE)
	{
		return NDIS_STATUS_SUCCESS;
	}

	/*check sta tr entry is exist*/
	trEntry = &pAd->MacTab.tr_entry[wcid];
	if(!trEntry)
	{
		return NDIS_STATUS_SUCCESS;
	}
	/*check frame is QoS or Non-QoS frame*/
	if(!(pFmeCtrl->SubType & 0x08))
	{
		up = SN_NQOS_INDEX;
	}

	/*check is not retry frame or check sn is duplicate or not, update sn only*/
	if(!pFmeCtrl->Retry || trEntry->cacheSn[up] != sn)
	{
		/*update cache*/
		trEntry->cacheSn[up] = sn;
		return NDIS_STATUS_SUCCESS;
	}

	/* Middle/End of fragment */
	if (pHeader->Frag && pHeader->Frag != pAd->FragFrame.LastFrag)
	{
		return NDIS_STATUS_SUCCESS;
	}

	/*is duplicate frame, should return failed*/
	return NDIS_STATUS_FAILURE;
}



/*
 All Rx routines use RX_BLK structure to hande rx events
 It is very important to build pRxBlk attributes
  1. pHeader pointer to 802.11 Header
  2. pData pointer to payload including LLC (just skip Header)
  3. set payload size including LLC to DataSize
  4. set some flags with RX_BLK_SET_FLAG()
*/
// TODO: shiang-usw, FromWhichBSSID is replaced by "pRxBlk->wdev_idx"
// TODO:
// TODO: FromWhichBSSID = pEntry->apidx // For AP
// TODO: FromWhichBSSID = BSS0; // For STA
// TODO: FromWhichBSSID = pEntry->MatchMeshTabIdx + MIN_NET_DEVICE_FOR_MESH; // For Mesh
// TODO: FromWhichBSSID = pEntry->MatchWDSTabIdx + MIN_NET_DEVICE_FOR_WDS; // For WDS
// TODO: FromWhichBSSID = pEntry->MatchAPCLITabIdx + MIN_NET_DEVICE_FOR_APCLI; // For APCLI
// TODO: FromWhichBSSID = pEntry->apidx + MIN_NET_DEVICE_FOR_P2P_GO;  // For P2P
VOID dev_rx_data_frm(RTMP_ADAPTER *pAd, RX_BLK *pRxBlk)
{
	RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;
	HEADER_802_11 *pHeader = pRxBlk->pHeader;
	PNDIS_PACKET pRxPacket = pRxBlk->pRxPacket;
	BOOLEAN bFragment = FALSE;
	MAC_TABLE_ENTRY *pEntry = NULL;
	UCHAR wdev_idx = BSS0;
	FRAME_CONTROL *pFmeCtrl = &pHeader->FC;
	UCHAR UserPriority = 0;
	INT hdr_len = LENGTH_802_11;
	COUNTER_RALINK *pCounter = &pAd->RalinkCounters;
	UCHAR *pData;
	struct wifi_dev *wdev;
	BOOLEAN drop_err = TRUE;
#if defined(SOFT_ENCRYPT) || defined(ADHOC_WPA2PSK_SUPPORT)
	NDIS_STATUS status;
#endif /* defined(SOFT_ENCRYPT) || defined(ADHOC_WPA2PSK_SUPPORT) */

    DBGPRINT(RT_DEBUG_INFO, ("-->%s():pRxBlk->wcid=%d, pRxBlk->DataSize=%d\n",
                __FUNCTION__, pRxBlk->wcid, pRxBlk->DataSize));

//dump_rxblk(pAd, pRxBlk);

#ifdef CONFIG_STA_SUPPORT
#ifdef HDR_TRANS_SUPPORT
	if ( pRxBlk->bHdrRxTrans) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): HdrRxTrans frame!\n", __FUNCTION__));
		STAHandleRxDataFrame_Hdr_Trns(pAd, pRxBlk);
		return;
	}
#endif /* HDR_TRANS_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

//+++Add by shiang for debug
#if 0
if (!(pRxInfo->Mcast || pRxInfo->Bcast)){
	DBGPRINT(RT_DEBUG_OFF, ("-->%s(%d): Dump RxData FramePayload=%d\n", __FUNCTION__, __LINE__, pRxBlk->DataSize));
	dump_rxinfo(pAd, pRxInfo);
	hex_dump("DataFrameHeader", (UCHAR *)pHeader, sizeof(HEADER_802_11));
	hex_dump("DataFramePayload", pRxBlk->pData , (pRxBlk->DataSize > 128 ? 128 :pRxBlk->DataSize));
}

if (!(pRxInfo->Mcast || pRxInfo->Bcast)) {
		DBGPRINT(RT_DEBUG_TRACE, ("Dump the RxBlk info, MC/BC:%d,%d\n", pRxInfo->Mcast, pRxInfo->Bcast));
		dump_rxblk(pAd, pRxBlk);
		DBGPRINT(RT_DEBUG_TRACE, ("<==%s():Finish dump!\n", __FUNCTION__));
}
#endif

//	hex_dump("DataFrameHeader", (UCHAR *)pHeader, sizeof(HEADER_802_11));
//	hex_dump("DataFramePayload", pRxBlk->pData , (pRxBlk->DataSize > 128 ? 128 :pRxBlk->DataSize));
//---Add by shiangf for debug

	// TODO: shiang-usw, check wcid if we are repeater mode! when in Repeater mode, wcid is get by "A2" + "A1"
	if (VALID_WCID(pRxBlk->wcid))
	{
		pEntry = &pAd->MacTab.Content[pRxBlk->wcid];
#ifdef MT7603		
		if (MTK_REV_GTE(pAd, MT7603, MT7603E1)) 
		{
			if (NdisCmpMemory(pEntry->Addr, pHeader->Addr2, MAC_ADDR_LEN))
			{
				pEntry = MacTableLookup(pAd, pHeader->Addr2);
				if (pEntry)
					pRxBlk->wcid = pEntry->wcid;
			}
		}
#endif	/* MT7603 */		
	}
	else {
		/* IOT issue with Marvell test bed AP
		    Marvell AP ResetToOOB and do wps.
		    Because of AP send EAP Request too fast and without retransmit.
		    STA not yet add BSSID to WCID search table.
		    So, the EAP Request is dropped.
		    The patch lookup pEntry from MacTable.
		*/
		pEntry = MacTableLookup(pAd, pHeader->Addr2);
		if (pEntry)
			pRxBlk->wcid = pEntry->wcid;
	}

#ifdef CONFIG_STA_SUPPORT
	// TODO: shiang-usw, check this, may need to move to "wdev->rx_pkt_allowed"!
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef MT7601
#ifdef ANT_DIVERSITY_SUPPORT
		/*
			collect rssi information for ant diversity
			collect data packet's RSSI, accelerate RSSI collection speed
		*/
		if ((pAd->CommonCfg.RxAntDiversityCfg == ANT_HW_DIVERSITY_ENABLE) && pAd->chipCap.FlgSwBasedPPAD)
			STARxCollectEvalRssi(pAd, pRxBlk, pRxInfo);  /* MT7601 SW based PPAD */
#endif /* ANT_DIVERSITY_SUPPORT */
#endif /* MT7601 */
	}
#endif /* CONFIG_STA_SUPPORT */


	/*
		if FrameCtrl.type == DATA
			FromDS = 1, ToDS = 1 : send from WDS/MESH
			FromDS = 1, ToDS = 0 : send from STA
			FromDS = 0, ToDS = 1 : send from AP
			FromDS = 0, ToDS = 0 : AdHoc / TDLS

			if pRxBlk->wcid == VALID,
				directly assign to the pEntry[WCID]->wdev->rx

			if pRxBlk->wcid == INVALD,
				FromDS = 1, ToDS = 1 : WDS/MESH Rx
				FromDS = 1, ToDS = 0 : drop
				FromDS = 0, ToDS = 1 : drop
				FromDS = 0, ToDS = 0 : AdHoc/TDLS Rx
	*/
	if (pEntry && pEntry->wdev && pEntry->wdev->rx_pkt_allowed)
		hdr_len = pEntry->wdev->rx_pkt_allowed(pAd, pRxBlk);
	else {
		if (pEntry) {
			DBGPRINT(RT_DEBUG_INFO, ("invalid hdr_len, wdev=%p! ", pEntry->wdev));
			if (pEntry->wdev) {
				DBGPRINT(RT_DEBUG_INFO, ("rx_pkt_allowed=%p!", pEntry->wdev->rx_pkt_allowed));
			}
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		}
		else
		{
#ifdef CONFIG_AP_SUPPORT
#if defined(WDS_SUPPORT) || defined(CLIENT_WDS)
			if ((pFmeCtrl->FrDs == 1) && (pFmeCtrl->ToDs == 1))
			{
				if (MAC_ADDR_EQUAL(pHeader->Addr1, pAd->CurrentAddress))
					pEntry = FindWdsEntry(pAd, pRxBlk->wcid, pHeader->Addr2, pRxBlk->rx_rate.field.MODE);
			}
#endif /* defined(WDS_SUPPORT) || defined(CLIENT_WDS) */

			/* check if Class2 or 3 error */
			if ((pFmeCtrl->FrDs == 0) && (pFmeCtrl->ToDs == 1))
			{
				APChkCls2Cls3Err(pAd, pRxBlk->wcid, pHeader);
			}
#endif /* CONFIG_AP_SUPPORT */
		}

		goto drop;
	}

	wdev = pEntry->wdev;
	wdev_idx = wdev->wdev_idx;
	DBGPRINT(RT_DEBUG_INFO, ("%s(): wcid=%d, wdev_idx=%d, pRxBlk->Flags=0x%x, fRX_AP/STA/ADHOC=0x%x/0x%x/0x%x, Type/SubType=%d/%d, FrmDS/ToDS=%d/%d\n",
                __FUNCTION__, pEntry->wcid, wdev->wdev_idx,
                pRxBlk->Flags,
                RX_BLK_TEST_FLAG(pRxBlk, fRX_AP),
                RX_BLK_TEST_FLAG(pRxBlk, fRX_STA),
                RX_BLK_TEST_FLAG(pRxBlk, fRX_ADHOC),
                pHeader->FC.Type, pHeader->FC.SubType,
                pHeader->FC.FrDs, pHeader->FC.ToDs));

   	/* Gather PowerSave information from all valid DATA frames. IEEE 802.11/1999 p.461 */
   	/* must be here, before no DATA check */
	pData = (UCHAR *)pHeader;

	if (wdev->rx_ps_handle)
		wdev->rx_ps_handle(pAd, pRxBlk);

	/*
		update RxBlk->pData, DataSize, 802.11 Header, QOS, HTC, Hw Padding
	*/
	pData = (UCHAR *)pHeader;

	/* 1. skip 802.11 HEADER */
	pData += hdr_len;
	pRxBlk->DataSize -= hdr_len;

	/* 2. QOS */
	if (pFmeCtrl->SubType & 0x08)
	{
		UserPriority = *(pData) & 0x0f;

#ifdef CONFIG_AP_SUPPORT
#ifdef WMM_ACM_SUPPORT
		ACMP_DATA_NULL_HANDLE(pAd, pEntry, pHeader, UserPriority);
#endif /* WMM_ACM_SUPPORT */

		/* count packets priroity more than BE */
		detect_wmm_traffic(pAd, UserPriority, 0);
#endif /* CONFIG_AP_SUPPORT */

		/* bit 7 in QoS Control field signals the HT A-MSDU format */
		if ((*pData) & 0x80)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s:receive AMSDU pkt!\n", __func__));
			RX_BLK_SET_FLAG(pRxBlk, fRX_AMSDU);
			pCounter->RxAMSDUCount.u.LowPart++;
		}

#ifdef DOT11_N_SUPPORT
		if (pRxInfo->BA)
		{
			RX_BLK_SET_FLAG(pRxBlk, fRX_AMPDU);

			/* incremented by the number of MPDUs */
			/* received in the A-MPDU when an A-MPDU is received. */
			pCounter->MPDUInReceivedAMPDUCount.u.LowPart ++;
		}
		else
		{
			if (pAd->MacTab.Content[pRxBlk->wcid].BARecWcidArray[pRxBlk->TID] != 0)
				RX_BLK_SET_FLAG(pRxBlk, fRX_AMPDU);
		}
#endif /* DOT11_N_SUPPORT */

		/* skip QOS contorl field */
		pData += 2;
		pRxBlk->DataSize -= 2;
	}
	pRxBlk->UserPriority = UserPriority;

	/*check if duplicate frame, ignore it and then drop*/
	if(rx_chk_duplicate_frame(pAd,pRxBlk) == NDIS_STATUS_FAILURE)
	{
		DBGPRINT(RT_DEBUG_INFO, ("%s(): duplication frame, drop it!\n", __FUNCTION__));
		goto drop;
	}

#ifdef CONFIG_STA_SUPPORT
	/*
		check if need to resend PS Poll when received packet with MoreData = 1
		a).only for unicast packet
		b).for TDLS power save, More Data bit is not used
	*/
	if (IS_ENTRY_AP(pEntry) && (pRxInfo->U2M))
	{
		if ((RtmpPktPmBitCheck(pAd) == TRUE) && (pHeader->FC.MoreData == 1))
		{
			if ((((UserPriority == 0) || (UserPriority == 3)) && pAd->CommonCfg.bAPSDAC_BE == 0) ||
	    			(((UserPriority == 1) || (UserPriority == 2)) && pAd->CommonCfg.bAPSDAC_BK == 0) ||
				(((UserPriority == 4) || (UserPriority == 5)) && pAd->CommonCfg.bAPSDAC_VI == 0) ||
				(((UserPriority == 6) || (UserPriority == 7)) && pAd->CommonCfg.bAPSDAC_VO == 0))
			{
				/* non-UAPSD delivery-enabled AC */
				RTMP_PS_POLL_ENQUEUE(pAd);
			}
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	/* 3. Order bit: A-Ralink or HTC+ */
	if (pFmeCtrl->Order)
	{
#ifdef AGGREGATION_SUPPORT
		// TODO: shiang-MT7603, fix me, because now we don't have rx_rate.field.MODE can refer
		if ((pRxBlk->rx_rate.field.MODE <= MODE_OFDM) &&
			(CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_AGGREGATION_CAPABLE)))
		{
			RX_BLK_SET_FLAG(pRxBlk, fRX_ARALINK);
		}
		else
#endif /* AGGREGATION_SUPPORT */
		{
#ifdef DOT11_N_SUPPORT
#ifdef TXBF_SUPPORT
			if (pAd->chipCap.FlgHwTxBfCap && (pFmeCtrl->SubType & 0x08))
				handleHtcField(pAd, pRxBlk);
#endif /* TXBF_SUPPORT */

			/* skip HTC control field */
			pData += 4;
			pRxBlk->DataSize -= 4;
#endif /* DOT11_N_SUPPORT */
		}
	}

	/* Drop NULL, CF-ACK(no data), CF-POLL(no data), and CF-ACK+CF-POLL(no data) data frame */
	if (pFmeCtrl->SubType & 0x04) /* bit 2 : no DATA */
	{
		DBGPRINT(RT_DEBUG_INFO, ("%s(): Null/QosNull frame!\n", __FUNCTION__));

		drop_err = FALSE;
		goto drop;
	}

	/* 4. skip HW padding */
	if (pRxInfo->L2PAD)
	{
		/* just move pData pointer because DataSize excluding HW padding */
		RX_BLK_SET_FLAG(pRxBlk, fRX_PAD);
		pData += 2;
	}

#ifdef RTMP_MAC_USB
#ifdef CONFIG_CSO_SUPPORT
	if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_CSO))
	{
		if (pRxInfo->vlan_taged_tcp_sum_err)
			RTMP_SET_TCP_CHKSUM_FAIL(pRxPacket, TRUE);
	}
#endif /* CONFIG_CSO_SUPPORT */
#endif /* RTMP_MAC_USB */

	pRxBlk->pData = pData;

#if defined(SOFT_ENCRYPT) || defined(ADHOC_WPA2PSK_SUPPORT)
	/* Use software to decrypt the encrypted frame if necessary.
	   If a received "encrypted" unicast packet(its WEP bit as 1)
	   and it's passed to driver with "Decrypted" marked as 0 in RxInfo.
	*/
	if (pAd->chipCap.hif_type != HIF_MT)
	{
	if ((pHeader->FC.Wep == 1) && (pRxInfo->Decrypted == 0))
	{
#ifdef HDR_TRANS_SUPPORT
		if ( pRxBlk->bHdrRxTrans) {
			status = RTMPSoftDecryptionAction(pAd,
								 	(PUCHAR)pHeader,
									 UserPriority,
									 &pEntry->PairwiseKey,
								 	 pRxBlk->pTransData + 14,
									 &(pRxBlk->TransDataSize));
		}
		else
#endif /* HDR_TRANS_SUPPORT */
		{
			CIPHER_KEY *pSwKey = &pEntry->PairwiseKey;

#ifdef CONFIG_STA_SUPPORT
			if (IS_ENTRY_AP(pEntry)) {
				pSwKey = RTMPSwCipherKeySelection(pAd,
						       			pRxBlk->pData, pRxBlk,
						       			pEntry);

				/* Cipher key table selection */
				if (!pSwKey) {
					DBGPRINT(RT_DEBUG_INFO, ("No vaild cipher key for SW decryption!!!\n"));
					RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
					return;
				}
			}
#endif /* CONFIG_STA_SUPPORT */

			status = RTMPSoftDecryptionAction(pAd,
								 	(PUCHAR)pHeader,
									 UserPriority,
									 pSwKey,
								 	 pRxBlk->pData,
									 &(pRxBlk->DataSize));
		}

		if ( status != NDIS_STATUS_SUCCESS)
		{
			RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
			return;
		}
		/* Record the Decrypted bit as 1 */
		pRxInfo->Decrypted = 1;
	}
	}
#endif /* SOFT_ENCRYPT || ADHOC_WPA2PSK_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
#ifdef TDLS_AUTOLINK_SUPPORT
	if (pAd->StaCfg.TdlsInfo.TdlsAutoLink)
	{
		if (!RX_BLK_TEST_FLAG(pRxBlk, fRX_DLS))
			TDLS_AutoSetupByRcvFrame(pAd, pHeader);
	}
#endif /* TDLS_AUTOLINK_SUPPORT */
#endif /* DOT11Z_TDLS_SUPPORT */

#ifdef SMART_ANTENNA
	if (RTMP_SA_WORK_ON(pAd))
		sa_pkt_radio_info_update(pAd, pRxBlk, pEntry);
#endif /* SMART_ANTENNA */

#ifdef DOT11_N_SUPPORT
#ifndef DOT11_VHT_AC
#ifndef WFA_VHT_PF
// TODO: shiang@PF#2, is this atheros protection still necessary here????
	/* check Atheros Client */
	if (!pEntry->bIAmBadAtheros && (pFmeCtrl->Retry) &&
		(pRxBlk->rx_rate.field.MODE < MODE_VHT) &&
		(pRxInfo->AMPDU == 1) && (pAd->CommonCfg.bHTProtect == TRUE)
	)
	{
		if (pAd->CommonCfg.IOTestParm.bRTSLongProtOn == FALSE)
			RTMP_UPDATE_PROTECT(pAd, 8 , ALLN_SETPROTECT, FALSE, FALSE);
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("Atheros Problem. Turn on RTS/CTS!!!\n"));
#endif /* RELEASE_EXCLUDE */
		pEntry->bIAmBadAtheros = TRUE;

		if (pEntry->WepStatus != Ndis802_11WEPDisabled)
			pEntry->MpduDensity = 6;
	}
#endif /* WFA_VHT_PF */
#endif /* DOT11_VHT_AC */
#endif /* DOT11_N_SUPPORT */


#ifdef CONFIG_AP_SUPPORT
#ifdef RT30xx
#ifdef ANT_DIVERSITY_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if ((pAd->CommonCfg.bSWRxAntDiversity)
 			&& (pAd->CommonCfg.RxAntDiversityCfg != ANT_DIVERSITY_DISABLE))
	   	{
			INT RssiSample;
#ifdef MT7601
			Err!!!
			/* MT7601 using different RSSI foruma. */
#endif /* MT7601 */
			RssiSample = ConvertToRssi(pAd, (struct raw_rssi_info *)(&pRxBlk->rx_signal.raw_rssi[0]), RSSI_IDX_0);
			AP_COLLECT_RX_ANTENNA_AVERAGE_RSSI(pAd, RssiSample, 0, 0);
	   	}
	}
#endif /* ANT_DIVERSITY_SUPPORT */
#endif /* RT30xx */
#endif /* CONFIG_AP_SUPPORT */

#ifdef RELEASE_EXCLUDE
   	DBGPRINT(RT_DEBUG_INFO, ("Rcv packet to IF(ra%d)\n", wdev_idx));
#endif /* RELEASE_EXCLUDE */

	/* update rssi sample */
	Update_Rssi_Sample(pAd, &pEntry->RssiSample, &pRxBlk->rx_signal, pRxBlk->rx_rate.field.MODE, pRxBlk->rx_rate.field.BW);
	pEntry->NoDataIdleCount = 0;
	// TODO: shiang-usw,  remove upper setting becasue we need to migrate to tr_entry!
	pAd->MacTab.tr_entry[pEntry->wcid].NoDataIdleCount = 0;

#ifdef CONFIG_STA_SUPPORT
	/* Case I  Process Broadcast & Multicast data frame */
DBGPRINT(RT_DEBUG_INFO, ("%s():pRxInfo->Bcast =%d, pRxInfo->Mcast=%d, pRxBlk->Flags=0x%x, fRX_AP/STA/ADHOC=0x%x/0x%x/0x%x\n",
			__FUNCTION__, pRxInfo->Bcast, pRxInfo->Mcast, pRxBlk->Flags,
			RX_BLK_TEST_FLAG(pRxBlk, fRX_AP),
			RX_BLK_TEST_FLAG(pRxBlk, fRX_STA),
			RX_BLK_TEST_FLAG(pRxBlk, fRX_ADHOC)));

	if (pRxInfo->Bcast || pRxInfo->Mcast) {
		/* Drop Mcast/Bcast frame with fragment bit on */
		if (pFmeCtrl->MoreFrag) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): drop frag B/Mast!\n", __FUNCTION__));
			RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
			return;
		}
		
		if (((pFmeCtrl->Wep == 1) && (pEntry->WepStatus == Ndis802_11EncryptionDisabled)) ||
			 ((pFmeCtrl->Wep == 0) && (pEntry->WepStatus != Ndis802_11EncryptionDisabled))) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s: drop B/Mcast as FC.Wep(%d) mismatch with pEntry->WepStatus(%d)!\n", __FUNCTION__, pFmeCtrl->Wep, pEntry->WepStatus));
                        RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
                        return;
		}

		/* Filter out Bcast frame which AP relayed for us */
		if (pFmeCtrl->FrDs
		    && MAC_ADDR_EQUAL(pHeader->Addr3, pAd->CurrentAddress)) {
			DBGPRINT(RT_DEBUG_INFO, ("%s(): pFmeCtrl->FrDs!\n", __FUNCTION__));
			RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
			return;
		}

#ifdef STATS_COUNT_SUPPORT
		INC_COUNTER64(pAd->WlanCounters.MulticastReceivedFrameCount);
#endif /* STATS_COUNT_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
#ifdef WFD_SUPPORT
		if ((pAd->StaCfg.WfdCfg.bWfdEnable) &&
			pRxInfo->Bcast &&
			TDLS_CheckTDLSframe(pAd, pRxBlk->pData, pRxBlk->DataSize))
		{
			UCHAR *pTmpBuf = pRxBlk->pData - LENGTH_802_11;

			NdisMoveMemory(pTmpBuf, pRxBlk->pHeader, LENGTH_802_11);
			REPORT_MGMT_FRAME_TO_MLME(pAd, pRxBlk->wcid,
						  pTmpBuf,
						  pRxBlk->DataSize + LENGTH_802_11,
						  pRxBlk->rx_signal.raw_rssi[0],
						  pRxBlk->rx_signal.raw_rssi[1],
						  pRxBlk->rx_signal.raw_rssi[2],
						  0,
						  OPMODE_STA);

			DBGPRINT_RAW(RT_DEBUG_INFO,
					 ("!!! report TDLS Action DATA to MLME (len=%d) !!!\n",
					  pRxBlk->DataSize));

			RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);
			return;
		}
#endif /* WFD_SUPPORT */
#endif /* DOT11Z_TDLS_SUPPORT */

		Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
#ifdef RELEASE_EXCLUDE
		DBGPRINT_RAW(RT_DEBUG_INFO, ("!!! report BCAST DATA to LLC (len=%d) !!!\n", pRxBlk->DataSize));
#endif /* RELEASE_EXCLUDE */
		return;
	}
#endif /* CONFIG_STA_SUPPORT */

	if (pRxInfo->U2M)
	{
#ifdef CONFIG_AP_SUPPORT
		Update_Rssi_Sample(pAd, &pAd->ApCfg.RssiSample, &pRxBlk->rx_signal, pRxBlk->rx_rate.field.MODE, pRxBlk->rx_rate.field.BW);
		pAd->ApCfg.NumOfAvgRssiSample ++;
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		Update_Rssi_Sample(pAd, &pAd->StaCfg.RssiSample, &pRxBlk->rx_signal, pRxBlk->rx_rate.field.MODE, pRxBlk->rx_rate.field.BW);
		pAd->RalinkCounters.OneSecRxOkDataCnt++;

#ifdef STA_LP_PHASE_1_SUPPORT
		/* there's packet sent to me, keep awake for 1200ms */
		if (pAd->CountDowntoPsm < 12)
			pAd->CountDowntoPsm = 12;
#else
#ifdef RTMP_MAC_USB
		/* there's packet sent to me, keep awake for 1200ms */
		if (pAd->CountDowntoPsm < 12)
			pAd->CountDowntoPsm = 12;
#endif /* RTMP_MAC_USB */
#endif
#endif /* CONFIG_STA_SUPPORT */

		pEntry->LastRxRate = (ULONG)(pRxBlk->rx_rate.word);

#ifdef TXBF_SUPPORT
		if (pRxBlk->rx_rate.field.ShortGI)
			pEntry->OneSecRxSGICount++;
		else
			pEntry->OneSecRxLGICount++;
#endif /* TXBF_SUPPORT */

#ifdef DBG_DIAGNOSE
		if (pAd->DiagStruct.inited) {
			struct dbg_diag_info *diag_info;
			diag_info = &pAd->DiagStruct.diag_info[pAd->DiagStruct.ArrayCurIdx];
			diag_info->RxDataCnt++;
#ifdef DBG_RX_MCS
			if (pRxBlk->rx_rate.field.MODE == MODE_HTMIX ||
				pRxBlk->rx_rate.field.MODE == MODE_HTGREENFIELD) {
				if (pRxBlk->rx_rate.field.MCS < MAX_MCS_SET)
					diag_info->RxMcsCnt_HT[pRxBlk->rx_rate.field.MCS]++;
			}
#ifdef DOT11_VHT_AC
			if (pRxBlk->rx_rate.field.MODE == MODE_VHT) {
				INT mcs_idx = ((pRxBlk->rx_rate.field.MCS >> 4) * 10) +
								(pRxBlk->rx_rate.field.MCS & 0xf);
				if (mcs_idx < MAX_VHT_MCS_SET)
					diag_info->RxMcsCnt_VHT[mcs_idx]++;
			}
#endif /* DOT11_VHT_AC */
#endif /* DBG_RX_MCS */
		}
#endif /* DBG_DIAGNOSE */
	}
#ifdef CONFIG_STA_SUPPORT
#ifdef XLINK_SUPPORT
	else if (IS_ENTRY_AP(pEntry))
	{
		if (pAd->StaCfg.PSPXlink) {
			Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);
			return;
		} else
			goto drop;
	}
#endif /* XLINK_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	wdev->LastSNR0 = (UCHAR)(pRxBlk->rx_signal.raw_snr[0]);
	wdev->LastSNR1 = (UCHAR)(pRxBlk->rx_signal.raw_snr[1]);
#ifdef DOT11N_SS3_SUPPORT
	wdev->LastSNR2 = (UCHAR)(pRxBlk->rx_signal.raw_snr[2]);
#endif /* DOT11N_SS3_SUPPORT */
	pEntry->freqOffset = (CHAR)(pRxBlk->rx_signal.freq_offset);
	pEntry->freqOffsetValid = TRUE;

#if defined(RT2883) || defined(RT3883)
	if (IS_RT2883(pAd) || IS_RT3883(pAd))
	{
		pEntry->BF_SNR[0] = BF_SNR_OFFSET + pRxBlk->rx_signal.bf_snr[0];
		pEntry->BF_SNR[1] = BF_SNR_OFFSET + pRxBlk->rx_signal.bf_snr[1];
		pEntry->BF_SNR[2] = BF_SNR_OFFSET + pRxBlk->rx_signal.bf_snr[2];
#ifdef CONFIG_STA_SUPPORT
		if (pRxInfo->U2M) {
#ifdef PRE_ANT_SWITCH
			STASelectPktDetAntenna(pAd);
#endif /* PRE_ANT_SWITCH */
		}
#endif /* CONFIG_STA_SUPPORT */
	}
#endif /* defined(RT2883) || defined(RT3883) */

	if (!((pHeader->Frag == 0) && (pFmeCtrl->MoreFrag == 0)))
	{
		/*
			re-assemble the fragmented packets, return complete
			frame (pRxPacket) or NULL
		*/
		bFragment = TRUE;
		pRxPacket = RTMPDeFragmentDataFrame(pAd, pRxBlk);
	}

	if (pRxPacket)
	{
		/*
			process complete frame which encrypted by TKIP,
			Minus MIC length and calculate the MIC value
		*/
		if (bFragment && (pFmeCtrl->Wep) && (pEntry->WepStatus == Ndis802_11TKIPEnable))
		{
			pRxBlk->DataSize -= 8;
			if (rtmp_chk_tkip_mic(pAd, pEntry, pRxBlk) == FALSE)
				return;
		}

#ifdef CONFIG_AP_SUPPORT
		pEntry->RxBytes += pRxBlk->MPDUtotalByteCnt;
		INC_COUNTER64(pEntry->RxPackets);
#endif /* CONFIG_AP_SUPPORT */
        pAd->RxTotalByteCnt += pRxBlk->MPDUtotalByteCnt;

#ifdef IKANOS_VX_1X0
		RTMP_SET_PACKET_WDEV(pRxPacket, wdev_idx);
#endif /* IKANOS_VX_1X0 */

#ifdef MAC_REPEATER_SUPPORT
		if (IS_ENTRY_APCLI(pEntry))
			RTMP_SET_PACKET_WCID(pRxPacket, pRxBlk->wcid);
#endif /* MAC_REPEATER_SUPPORT */

		rx_data_frm_announce(pAd, pEntry, pRxBlk, wdev);
	}
	else
	{
		/*
			just return because RTMPDeFragmentDataFrame() will release rx
			packet, if packet is fragmented
		*/
	}

    DBGPRINT(RT_DEBUG_INFO, ("<--%s(): Success!\n", __FUNCTION__));

	return;

drop:
#ifdef CONFIG_AP_SUPPORT
	/* Increase received error packet counter per BSS */
	if (pFmeCtrl->FrDs == 0 &&
		pRxInfo->U2M &&
		pRxBlk->bss_idx < pAd->ApCfg.BssidNum)
	{
		pAd->ApCfg.MBSSID[pRxBlk->bss_idx].RxDropCount ++;
		if (drop_err == TRUE)
			pAd->ApCfg.MBSSID[pRxBlk->bss_idx].RxErrorCount ++;
	}
#endif /* CONFIG_AP_SUPPORT */
	//DBGPRINT(RT_DEBUG_OFF, ("%s():release packet!\n", __FUNCTION__));

	RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);

    DBGPRINT(RT_DEBUG_INFO, ("<--%s(): Drop!\n", __FUNCTION__));

	return;
}


/*
		========================================================================
		Routine Description:
			Process RxDone interrupt, running in DPC level

		Arguments:
			pAd    Pointer to our adapter

		Return Value:
			None

		Note:
			This routine has to maintain Rx ring read pointer.
	========================================================================
*/
#undef MAX_RX_PROCESS_CNT
#define MAX_RX_PROCESS_CNT	(256)

BOOLEAN rtmp_rx_done_handle(RTMP_ADAPTER *pAd)
{
#ifdef RTMP_MAC_PCI
	UINT32 RxProcessed = 0; 
#endif
    UINT32 RxPending = 0;
	BOOLEAN bReschedule = FALSE;
	PNDIS_PACKET pRxPacket;
	HEADER_802_11 *pHeader;
	RX_BLK rxblk, *pRxBlk = NULL;
	BSS_STRUCT *pMbss;
	struct wifi_dev *wdev;

	DBGPRINT(RT_DEBUG_FPGA, ("-->%s():\n", __FUNCTION__));

#ifdef LINUX
#ifdef RTMP_RBUS_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
	{
#if defined(CONFIG_RA_CLASSIFIER) ||defined(CONFIG_RA_CLASSIFIER_MODULE)
#if defined(CONFIG_RALINK_EXTERNAL_TIMER)
		classifier_cur_cycle = (*((UINT32 *)(0xB0000D08))&0x0FFFF);
#else
		classifier_cur_cycle = read_c0_count();
#endif /* CONFIG_RALINK_EXTERNAL_TIMER */
#endif /* CONFIG_RA_CLASSIFIER */
	}
#endif /* RTMP_RBUS_SUPPORT */
#endif /* LINUX */

	/* process whole rx ring */
	while (1)
	{
		if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RADIO_OFF |
								fRTMP_ADAPTER_RESET_IN_PROGRESS |
#if 0
/* Fix Rx Ring FULL lead DMA Busy, when DUT is in reset stage */
								fRTMP_ADAPTER_HALT_IN_PROGRESS |
#endif
								fRTMP_ADAPTER_NIC_NOT_EXIST)) ||
				(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
			)
			&& (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_POLL_IDLE)))
		{
			break;
		}

#ifdef RTMP_MAC_PCI
#ifdef UAPSD_SUPPORT
		UAPSD_TIMING_RECORD_INDEX(RxProcessed);
#endif /* UAPSD_SUPPORT */

		if (RxProcessed++ > MAX_RX_PROCESS_CNT)
		{
			bReschedule = TRUE;
			break;
		}

#ifdef UAPSD_SUPPORT
		/* static rate also need NICUpdateFifoStaCounters() function. */
		/*if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED)) */
		UAPSD_MR_SP_SUSPEND(pAd);
#endif /* UAPSD_SUPPORT */

		/*
			Note:

			Can not take off the NICUpdateFifoStaCounters(); Or the
			FIFO overflow rate will be high, i.e. > 3%
			(see the rate by "iwpriv ra0 show stainfo")

			Based on different platform, try to find the best value to
			replace '4' here (overflow rate target is about 0%).
		*/
		if (++pAd->FifoUpdateDone >= FIFO_STAT_READ_PERIOD)
		{
			NICUpdateFifoStaCounters(pAd);
			pAd->FifoUpdateDone = 0;
		}
#endif /* RTMP_MAC_PCI */

		/*
			1. allocate a new data packet into rx ring to replace received packet
				then processing the received packet
			2. the callee must take charge of release of packet
			3. As far as driver is concerned, the rx packet must
				a. be indicated to upper layer or
				b. be released if it is discarded
		*/

#ifdef RELEASE_EXCLUDE
		/* for 5VT: Time for GetPacketFromRxRing is 25us */
#endif /* RELEASE_EXCLUDE */
		NdisZeroMemory(&rxblk,sizeof(RX_BLK));

		pRxBlk = &rxblk;
		pRxPacket = GetPacketFromRxRing(pAd, pRxBlk, &bReschedule, &RxPending, 0);

		if (RX_BLK_TEST_FLAG(pRxBlk, fRX_CMD_RSP)) {
			RX_BLK_CLEAR_FLAG(pRxBlk, fRX_CMD_RSP);
			continue;
		}

      	if (RX_BLK_TEST_FLAG(pRxBlk, fRX_RETRIEVE)) {
         	RX_BLK_CLEAR_FLAG(pRxBlk, fRX_RETRIEVE);
         	continue;
      	}
		
		if (pRxPacket == NULL)
			break;

#if 1
		/* Fix Rx Ring FULL lead DMA Busy, when DUT is in reset stage */
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
		{
			if (pRxPacket)
			{
				RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
				continue;
			}
		}
#endif

		/* get rx descriptor and data buffer */
		pHeader = rxblk.pHeader;
		if (!pHeader) {
			DBGPRINT(RT_DEBUG_ERROR,
				 ("%s(): null rxblk.pHeader!\n", __func__));
			continue;
		}

#ifdef MT_MAC
		if (pAd->chipCap.hif_type == HIF_MT) {
                   if ((rxblk.DataSize == 0) && (pRxPacket)) {
                      RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
                      DBGPRINT(RT_DEBUG_INFO, ("%s():Packet Length is zero!\n", __FUNCTION__));
	              continue;
		   }

		}
#endif /* MT_MAC */

#ifdef RLT_MAC
#ifdef CONFIG_ANDES_SUPPORT
#ifdef RTMP_PCI_SUPPORT
		if ((pAd->chipCap.hif_type == HIF_RLT) &&
			(pRxBlk->pRxFceInfo->info_type == CMD_PACKET))
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s: Receive command packet.\n", __FUNCTION__));
			continue;
		}
#endif /* RTMP_PCI_SUPPORT */
#endif /* CONFIG_ANDES_SUPPORT */

		// TODO: shiang-6590, handle packet from other ports
		if (pAd->chipCap.hif_type == HIF_RLT)
		{
			RXINFO_STRUC *pRxInfo = rxblk.pRxInfo;
			RXFCE_INFO *pFceInfo = rxblk.pRxFceInfo;

#ifdef HDR_TRANS_SUPPORT
			if ((pFceInfo->info_type == 0) && (pFceInfo->pkt_80211 == 0) &&
				(pRxInfo->hdr_trans_ip_sum_err == 1))
			{
				pRxBlk->bHdrRxTrans = pRxBlk->pRxInfo->hdr_trans_ip_sum_err;
				pRxBlk->bHdrVlanTaged = pRxBlk->pRxInfo->vlan_taged_tcp_sum_err;
				if (IS_MT7601(pAd))
					pRxBlk->pTransData = (UCHAR *)pHeader +  38; /* 36 byte + 802.3 padding */
				else
					pRxBlk->pTransData = (UCHAR *)pHeader +  36; /* 36 byte RX Wifi Info */
				pRxBlk->TransDataSize = pRxBlk->MPDUtotalByteCnt;
			} else
#endif /* HDR_TRANS_SUPPORT */
			if ((pFceInfo->info_type != 0) || (pFceInfo->pkt_80211 != 1))
			{
				RXD_STRUC *pRxD = (RXD_STRUC *)&pRxBlk->hw_rx_info[0];

				DBGPRINT(RT_DEBUG_OFF, ("==>%s(): GetFrameFromOtherPorts!\n", __FUNCTION__));
				hex_dump("hw_rx_info", &rxblk.hw_rx_info[0], sizeof(rxblk.hw_rx_info));
				DBGPRINT(RT_DEBUG_TRACE, ("Dump the RxD, RxFCEInfo and RxInfo:\n"));
				hex_dump("RxD", (UCHAR *)pRxD, sizeof(RXD_STRUC));
#ifdef RTMP_MAC_PCI
				dump_rxd(pAd, pRxD);
#endif /* RTMP_MAC_PCI */
				dumpRxFCEInfo(pAd, pFceInfo);
				dump_rxinfo(pAd, pRxInfo);
				hex_dump("RxFrame", (UCHAR *)GET_OS_PKT_DATAPTR(pRxPacket), (pFceInfo->pkt_len));
				DBGPRINT(RT_DEBUG_OFF, ("<==\n"));
				RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
				continue;
			}
		}
#endif /* RLT_MAC */

#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, (UCHAR *)pHeader, DIR_READ, TRUE);
		// TODO: shiang-usw, following endian swap move the GetPacketFromRxRing()
		//RTMPWIEndianChange(pAd , (UCHAR *)pRxWI, TYPE_RXWI);
#endif /* RT_BIG_ENDIAN */

//+++Add by shiang for debug
#if 1
		if (1 || (pHeader->FC.Type == FC_TYPE_DATA)) {
			if (!pRxBlk->pRxInfo) {
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): pRxBlk->pRxInfo is NULL!\n", __FUNCTION__));
				RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
				continue;
			}

#if 0
#ifdef RLT_MAC
			if (pAd->chipCap.hif_type == HIF_RLT) {
				RXWI_STRUC *pRxWI = (RXWI_STRUC *)GET_OS_PKT_DATAPTR(pRxPacket);

#ifdef RT_BIG_ENDIAN
				RTMPWIEndianChange(pAd , (UCHAR *)pRxWI, TYPE_RXWI);
#endif /* RT_BIG_ENDIAN */
				DBGPRINT(RT_DEBUG_TRACE, ("Dump the RxWI:\n"));
				dump_rmac_info(pAd, (UCHAR *)pRxWI);

				dumpRxFCEInfo(pAd, rxblk.pRxFceInfo);
			}
#endif /* RLT_MAC */
#endif
			//DBGPRINT(RT_DEBUG_TRACE, ("%s():Dump the RxBlk info\n", __FUNCTION__));
			//dump_rxblk(pAd, pRxBlk);
			//hex_dump("RxPacket", (UCHAR *)pHeader, pRxBlk->MPDUtotalByteCnt);
			//DBGPRINT(RT_DEBUG_TRACE, ("%s():==>Finish dump!\n", __FUNCTION__));
		}
#endif
//---Add by shiang for debug

#ifdef DBG_CTRL_SUPPORT
#ifdef INCLUDE_DEBUG_QUEUE
		if (pAd->CommonCfg.DebugFlags & DBF_DBQ_RXWI)
			dbQueueEnqueueRxFrame(GET_OS_PKT_DATAPTR(pRxPacket),
										(UCHAR *)pHeader,
										pAd->CommonCfg.DebugFlags);
#endif /* INCLUDE_DEBUG_QUEUE */
#endif /* DBG_CTRL_SUPPORT */

#ifdef RT_CFG80211_SUPPORT
#ifdef RT_CFG80211_P2P_SUPPORT
	if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd) &&
		(NdisEqualMemory(pAd->cfg80211_ctrl.P2PCurrentAddress, pHeader->Addr1, MAC_ADDR_LEN) ||
		(pHeader->FC.SubType == SUBTYPE_PROBE_REQ)))
	{
		SET_PKT_OPMODE_AP(&rxblk);
	}
    	else if (RTMP_CFG80211_VIF_P2P_CLI_ON(pAd) &&
        	(((pHeader->FC.SubType == SUBTYPE_BEACON || pHeader->FC.SubType == SUBTYPE_PROBE_RSP) &&
        	   NdisEqualMemory(pAd->ApCfg.ApCliTab[MAIN_MBSSID].CfgApCliBssid, pHeader->Addr2, MAC_ADDR_LEN)) ||
        	(pHeader->FC.SubType == SUBTYPE_PROBE_REQ) ||
        	 NdisEqualMemory(pAd->ApCfg.ApCliTab[MAIN_MBSSID].MlmeAux.Bssid, pHeader->Addr2, MAC_ADDR_LEN)))
	{
		/* 
		   1. Beacon & ProbeRsp for Connecting & Tracking 
                   2. ProbeReq for P2P Search
                   3. Any Packet's Addr2 Equals MlmeAux.Bssid when connected   
                 */
#if 0
		if (NdisEqualMemory(pAd->ApCfg.ApCliTab[MAIN_MBSSID].CfgApCliBssid, pHeader->Addr2, MAC_ADDR_LEN))
		{
		//	if (pHeader->FC.SubType == SUBTYPE_BEACON)
		//		printk("==============> SUBTYPE_BEACON\n");
		}
#endif		
		SET_PKT_OPMODE_AP(&rxblk);
	}	
	else
#endif /* RT_CFG80211_P2P_SUPPORT */
	{
		//YF
		//todo: Bind to pAd->net_dev
		if (RTMP_CFG80211_HOSTAPD_ON(pAd))
			SET_PKT_OPMODE_AP(&rxblk);
		else
			SET_PKT_OPMODE_STA(&rxblk);
		
	}
#endif /* RT_CFG80211_SUPPORT */

#if 0
	if (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP)
		SET_PKT_OPMODE_AP(&rxblk);
	else
		SET_PKT_OPMODE_STA(&rxblk);
#endif

#ifdef CFG80211_MULTI_STA
		if (RTMP_CFG80211_MULTI_STA_ON(pAd, pAd->cfg80211_ctrl.multi_sta_net_dev) &&
	        (((pHeader->FC.SubType == SUBTYPE_BEACON || pHeader->FC.SubType == SUBTYPE_PROBE_RSP) &&
	        NdisEqualMemory(pAd->ApCfg.ApCliTab[MAIN_MBSSID].CfgApCliBssid, pHeader->Addr2, MAC_ADDR_LEN)) ||
	        (pHeader->FC.SubType == SUBTYPE_PROBE_REQ) ||
	        NdisEqualMemory(pAd->ApCfg.ApCliTab[MAIN_MBSSID].MlmeAux.Bssid, pHeader->Addr2, MAC_ADDR_LEN)))
        {
                SET_PKT_OPMODE_AP(&rxblk);
        }
        else
        {
                SET_PKT_OPMODE_STA(&rxblk);
        }

#endif /* CFG80211_MULTI_STA */

		/* Increase Total receive byte counter after real data received no mater any error or not */
		pAd->RalinkCounters.ReceivedByteCount += rxblk.DataSize;
		pAd->RalinkCounters.OneSecReceivedByteCount += rxblk.DataSize;
		pAd->RalinkCounters.RxCount++;
		pAd->RalinkCounters.OneSecRxCount++;

#ifdef CONFIG_ATE
		if (ATE_ON(pAd)) {
			ate_rx_done_handle(pAd, pRxBlk);

			RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_SUCCESS);
			continue;
		}
#endif /* CONFIG_ATE */

#ifdef CONFIG_SNIFFER_SUPPORT
		if (MONITOR_ON(pAd) && pAd->monitor_ctrl.current_monitor_mode != MONITOR_MODE_OFF)
		{
			PNDIS_PACKET	pClonePacket;
			PNDIS_PACKET    pTmpRxPacket;
			struct wifi_dev *wdev;
			
			UCHAR wdev_idx = RTMP_GET_PACKET_WDEV(rxblk.pRxPacket);
			
			ASSERT(wdev_idx < WDEV_NUM_MAX);
			if (wdev_idx >= WDEV_NUM_MAX) {
				DBGPRINT(RT_DEBUG_ERROR, ("%s():invalid wdev_idx(%d)!\n", __FUNCTION__, wdev_idx));
				RELEASE_NDIS_PACKET(pAd, rxblk.pRxPacket, NDIS_STATUS_FAILURE);
				return 0;
			}
			
			wdev = pAd->wdev_list[wdev_idx];

			if(pAd->monitor_ctrl.current_monitor_mode == MONITOR_MODE_REGULAR_RX)
			{	

				/* only report Probe_Req */
				if((pHeader->FC.Type == FC_TYPE_MGMT) && (pHeader->FC.SubType == SUBTYPE_PROBE_REQ))	
		    	{					
					pTmpRxPacket = rxblk.pRxPacket;
					pClonePacket = ClonePacket(wdev->if_dev, rxblk.pRxPacket, rxblk.pData, rxblk.DataSize);
					rxblk.pRxPacket = pClonePacket;
					STA_MonPktSend(pAd, &rxblk);
					
					rxblk.pRxPacket = pTmpRxPacket;	
				}
			}
			else if(pAd->monitor_ctrl.current_monitor_mode == MONITOR_MODE_FULL)
			{
					pTmpRxPacket = rxblk.pRxPacket;
					pClonePacket = ClonePacket(wdev->if_dev, rxblk.pRxPacket, rxblk.pData, rxblk.DataSize);
					rxblk.pRxPacket = pClonePacket;
					STA_MonPktSend(pAd, &rxblk);
					//RELEASE_NDIS_PACKET(pAd, rxblk.pRxPacket , NDIS_STATUS_SUCCESS);
					rxblk.pRxPacket = pTmpRxPacket;	
			}

		}		
#endif /* CONFIG_SNIFFER_SUPPORT */


#ifdef STATS_COUNT_SUPPORT
		INC_COUNTER64(pAd->WlanCounters.ReceivedFragmentCount);
#endif /* STATS_COUNT_SUPPORT */

		pMbss = &pAd->ApCfg.MBSSID[CFG_GO_BSSID_IDX];
		wdev = &pMbss->wdev;

		if ((pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP && (wdev->Hostapd != Hostapd_CFG))
#ifdef RT_CFG80211_P2P_SUPPORT
			&& (!RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
#endif //RT_CFG80211_P2P_SUPPORT
			)
		{
			/*Drop receive request since hardware is in reset state*/
			RELEASE_NDIS_PACKET(pAd,pRxPacket,NDIS_STATUS_FAILURE);
			continue;
		}


		/* Check for all RxD errors */
		if (rtmp_chk_rx_err(pAd, pRxBlk, pHeader) != NDIS_STATUS_SUCCESS)
		{
			pAd->Counters8023.RxErrors++;

			RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
			DBGPRINT(RT_DEBUG_OFF, ("%s(): drop pkt as CheckRxError!\n", __FUNCTION__));
			continue;
		}


		// TODO: shiang-usw, for P2P, we original has following code, need to check it and merge to correct place!!!
#if 0
		if (P2P_GO_ON(pAd) || P2P_CLI_ON(pAd))
		{
			if (NdisEqualMemory(pAd->P2PCurrentAddress, pHeader->Addr1, MAC_ADDR_LEN) ||
			    (pHeader->FC.SubType == SUBTYPE_PROBE_REQ) ||
			    (pHeader->FC.SubType == SUBTYPE_BEACON
			    && NdisEqualMemory(pAd->ApCfg.ApCliTab[0].CfgApCliBssid, pHeader->Addr2, MAC_ADDR_LEN)) ||
			    NdisEqualMemory(pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid, pHeader->Addr2, MAC_ADDR_LEN))
				bReschedule = APHandleRxDonePacket(pAd, pRxPacket, pRxBlk);
			else
				bReschedule = STAHandleRxDonePacket(pAd, pRxPacket, pRxBlk);
		}
		else
			bReschedule = STAHandleRxDonePacket(pAd, pRxPacket, pRxBlk);
#endif

		switch (pHeader->FC.Type)
		{
			case FC_TYPE_DATA:
				dev_rx_data_frm(pAd, &rxblk);
				break;

			case FC_TYPE_MGMT:
				dev_rx_mgmt_frm(pAd, &rxblk);
				break;

			case FC_TYPE_CNTL:
				dev_rx_ctrl_frm(pAd, &rxblk);
				break;

			default:
				RELEASE_NDIS_PACKET(pAd, pRxPacket, NDIS_STATUS_FAILURE);
				break;
		}
	}

#ifdef UAPSD_SUPPORT
#ifdef CONFIG_AP_SUPPORT

#ifdef RT_CFG80211_P2P_SUPPORT
        if (pRxBlk && IS_PKT_OPMODE_AP(pRxBlk))
#else	
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */ 
	{
		/* dont remove the function or UAPSD will fail */
		UAPSD_MR_SP_RESUME(pAd);
		UAPSD_SP_CloseInRVDone(pAd);
	}
#endif /* CONFIG_AP_SUPPORT */
#endif /* UAPSD_SUPPORT */

	return bReschedule;
}

