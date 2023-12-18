#include "rt_config.h"

#define ADHOC_ENTRY_BEACON_LOST_TIME    (2*OS_HZ)       /* 2 sec */ /* we re-add the ad-hoc peer into our mac table */
#define ADHOC_BEACON_LOST_TIME          (8*OS_HZ)       /* 8 sec */ /* we deauth the ad-hoc peer */

BOOLEAN Adhoc_AddPeerfromBeacon(RTMP_ADAPTER *pAd, BCN_IE_LIST *bcn_ie_list, 
				  NDIS_802_11_VARIABLE_IEs *pVIE, USHORT LenVIE)
{
	struct wifi_dev *wdev = &pAd->StaCfg.wdev;
	ULONG Now;
	NdisGetSystemUpTime(&Now);

	//YF-DEBUG-ADHOC--------------------------------------------------
	if (ADHOC_ON(pAd) && (CAP_IS_IBSS_ON(bcn_ie_list->CapabilityInfo)))
        {
        	UCHAR MaxSupportedRateIn500Kbps = 0;
                UCHAR idx = 0;
                MAC_TABLE_ENTRY *pEntry = NULL;
#ifdef IWSC_SUPPORT
                PWSC_CTRL pWpsCtrl = &pAd->StaCfg.WscControl;
#endif /* IWSC_SUPPORT */

                MaxSupportedRateIn500Kbps = dot11_max_sup_rate(bcn_ie_list->SupRateLen,
                                 &bcn_ie_list->SupRate[0], bcn_ie_list->ExtRateLen, &bcn_ie_list->ExtRate[0]);

                /* look up the existing table */
                pEntry = MacTableLookup(pAd, bcn_ie_list->Addr2);

                /*
                   Ad-hoc mode is using MAC address as BA session. 
                   So we need to continuously find newly joined adhoc station 
                   by receiving beacon. To prevent always check this, 
                   we use wcid == RESERVED_WCID to recognize it 
                   as newly joined adhoc station.
                 */
                 
		if ((ADHOC_ON(pAd) && ((!pEntry) || (pEntry && IS_ENTRY_NONE(pEntry)))) ||
                    (pEntry && RTMP_TIME_AFTER(Now, pEntry->LastBeaconRxTime + ADHOC_ENTRY_BEACON_LOST_TIME)))
                {
                	/* Another adhoc joining, add to our MAC table. */
                        if (pEntry == NULL)
                        {
                        	pEntry = MacTableInsertEntry(pAd, bcn_ie_list->Addr2, wdev, ENTRY_ADHOC, OPMODE_STA, FALSE);
                                DBGPRINT(RT_DEBUG_TRACE,("Another Adhoc join Peer ==> %02x:%02x:%02x:%02x:%02x:%02x\n",
                                                                        PRINT_MAC(bcn_ie_list->Addr2)));
#ifdef RT_CFG80211_SUPPORT      
                                RT_CFG80211_JOIN_IBSS(pAd, pAd->MlmeAux.Bssid);
                                CFG80211OS_NewSta(pAd->net_dev, bcn_ie_list->Addr2, NULL, 0);
#endif /* RT_CFG80211_SUPPORT */
                         }

                         if (pEntry == NULL)
                         	return FALSE;

                         SET_ENTRY_CLIENT(pEntry);
#ifdef IWSC_SUPPORT

                         hex_dump("Another adhoc joining - Addr2", bcn_ie_list->Addr2, MAC_ADDR_LEN);
                         hex_dump("Another adhoc joining - WscPeerMAC", pAd->StaCfg.WscControl.WscPeerMAC, MAC_ADDR_LEN);
                         if ((NdisEqualMemory(bcn_ie_list->Addr2, pAd->StaCfg.WscControl.WscPeerMAC, MAC_ADDR_LEN)) &&
                             (pAd->StaCfg.IWscInfo.bSendEapolStart == FALSE) &&
                             (pWpsCtrl->bWscTrigger == TRUE))
                         {
                         	pAd->StaCfg.IWscInfo.bSendEapolStart = TRUE;
                         }
#endif /* IWSC_SUPPORT */

#ifdef DOT11_VHT_AC
{
                         BOOLEAN result;
                         IE_LISTS *ielist;

                         os_alloc_mem(NULL, (UCHAR **)&ielist, sizeof(IE_LISTS));
                         if (!ielist)
                         	return FALSE;
                         
			 NdisZeroMemory((UCHAR *)ielist, sizeof(IE_LISTS));

                         if (bcn_ie_list->vht_cap_len && bcn_ie_list->vht_op_len)
                         {
                         	NdisMoveMemory(&ielist->vht_cap, &bcn_ie_list->vht_cap_ie, sizeof(VHT_CAP_IE));
                                NdisMoveMemory(&ielist->vht_op, &bcn_ie_list->vht_op_ie, sizeof(VHT_OP_IE));
                                ielist->vht_cap_len = bcn_ie_list->vht_cap_len;
                                ielist->vht_op_len = bcn_ie_list->vht_op_len;
                          }
                          
			  result = StaUpdateMacTableEntry(pAd, pEntry, MaxSupportedRateIn500Kbps,
                                                          &bcn_ie_list->HtCapability, bcn_ie_list->HtCapabilityLen,
                                                          &bcn_ie_list->AddHtInfo, bcn_ie_list->AddHtInfoLen,
                                                          ielist, bcn_ie_list->CapabilityInfo);

                          os_free_mem(NULL, ielist);
			  ielist = NULL;
                          
			  if ( result == FALSE)
                          {
                          	DBGPRINT(RT_DEBUG_TRACE, ("ADHOC - Add Entry failed.\n"));
                                return FALSE;
                          }
#ifdef IWSC_SUPPORT
                          else
                                pEntry->bUpdateInfoFromPeerBeacon = TRUE;
#endif /* IWSC_SUPPORT */
}
#else
                          if (StaUpdateMacTableEntry(pAd, pEntry, MaxSupportedRateIn500Kbps,
                                                     &bcn_ie_list->HtCapability, bcn_ie_list->HtCapabilityLen,
                                                     &bcn_ie_list->AddHtInfo, bcn_ie_list->AddHtInfoLen,
                                                     NULL, bcn_ie_list->CapabilityInfo) == FALSE)
                          {
                          	DBGPRINT(RT_DEBUG_TRACE, ("ADHOC - Add Entry failed.\n"));
                                return FALSE;
                          }
#ifdef IWSC_SUPPORT
                          else
                                pEntry->bUpdateInfoFromPeerBeacon = TRUE;
#endif /* IWSC_SUPPORT */
#endif /* DOT11_VHT_AC */

                          if (ADHOC_ON(pAd) && pEntry)
                          {
                          	RTMPSetSupportMCS(pAd, OPMODE_STA, pEntry,
                                                  bcn_ie_list->SupRate, bcn_ie_list->SupRateLen,
                                                  bcn_ie_list->ExtRate, bcn_ie_list->ExtRateLen,
#ifdef DOT11_VHT_AC
                                                  bcn_ie_list->vht_cap_len, &bcn_ie_list->vht_cap_ie,
#endif /* DOT11_VHT_AC */
                                                  &bcn_ie_list->HtCapability, bcn_ie_list->HtCapabilityLen);
                          }

                          pEntry->LastBeaconRxTime = 0;

#ifdef ADHOC_WPA2PSK_SUPPORT
                    /* Adhoc support WPA2PSK by Eddy */
                    	if ((wdev->AuthMode == Ndis802_11AuthModeWPA2PSK) && (pEntry->WPA_Authenticator.WpaState < AS_INITPSK)
#ifdef IWSC_SUPPORT
                         	&& ((pAd->StaCfg.WscControl.WscConfMode == WSC_DISABLE) ||
                                   (pAd->StaCfg.WscControl.bWscTrigger == FALSE) ||
                                   (NdisEqualMemory(pEntry->Addr, pAd->StaCfg.WscControl.WscPeerMAC, MAC_ADDR_LEN) == FALSE))
#ifdef IWSC_TEST_SUPPORT
                          	&& (pAd->StaCfg.IWscInfo.bBlockConnection == FALSE)
#endif /* IWSC_TEST_SUPPORT */
#endif /* IWSC_SUPPORT */
                            )
                    	{
                    		INT len, i;
                        	PEID_STRUCT pEid = NULL;
                        	NDIS_802_11_VARIABLE_IEs *pVIE2 = NULL;
                        	BOOLEAN bHigherMAC = FALSE;

                        	pVIE2 = pVIE;
                        	len = LenVIE;
                        	while (len > 0)
                        	{
                            		pEid = (PEID_STRUCT) pVIE;
                            		if ((pEid->Eid == IE_RSN) && (NdisEqualMemory(pEid->Octet + 2, RSN_OUI, 3)))
                            		{
                         	   		NdisMoveMemory(pEntry->RSN_IE, pVIE, (pEid->Len + 2));
                                   		pEntry->RSNIE_Len = (pEid->Len + 2);
                            		}
                            
			    		pVIE2 += (pEid->Len + 2);
                            		len  -= (pEid->Len + 2);

                        	}
                        
				//pEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED;
                        	// TODO: shiang-usw, need to replace upper setting with tr_entry
                        	pAd->MacTab.tr_entry[pEntry->wcid].PortSecured = WPA_802_1X_PORT_NOT_SECURED;

                        	NdisZeroMemory(&pEntry->WPA_Supplicant.ReplayCounter, LEN_KEY_DESC_REPLAY);
                        	NdisZeroMemory(&pEntry->WPA_Authenticator.ReplayCounter, LEN_KEY_DESC_REPLAY);
                        	pEntry->WPA_Authenticator.WpaState = AS_INITPSK;
                        	pEntry->WPA_Supplicant.WpaState = AS_INITPSK;
                        	pEntry->EnqueueEapolStartTimerRunning = EAPOL_START_PSK;

                        	for (i = 0; i < MAC_ADDR_LEN; i++)
                        	{
                        		if (bcn_ie_list->Addr2[i] > pAd->CurrentAddress[i])
                                	{
                                		bHigherMAC = TRUE;
                                        	break;
                                	}
                                	else if (bcn_ie_list->Addr2[i] < pAd->CurrentAddress[i])
                                		break;
                        	}
                        
				hex_dump("PeerBeacon:: Addr2", bcn_ie_list->Addr2, MAC_ADDR_LEN);
                        	hex_dump("PeerBeacon:: CurrentAddress", pAd->CurrentAddress, MAC_ADDR_LEN);
                        	pEntry->bPeerHigherMAC = bHigherMAC;

                        	if (pEntry->bPeerHigherMAC == FALSE)
                        	{
                        		/* My MAC address is higher than peer's MAC address. */
                                	DBGPRINT(RT_DEBUG_TRACE, ("ADHOC - EnqueueStartForPSKTimer.\n"));
                                	RTMPSetTimer(&pEntry->EnqueueStartForPSKTimer, ENQUEUE_EAPOL_START_TIMER);
                        	}
                    	}
                    	else
                    	{
                    		//pEntry->PortSecured = WPA_802_1X_PORT_SECURED;
                        	// TODO: shiang-usw, need to replace upper setting with tr_entry
                        	pAd->MacTab.tr_entry[pEntry->wcid].PortSecured = WPA_802_1X_PORT_SECURED;
                    	}
#endif /* ADHOC_WPA2PSK_SUPPORT */

                     	if (pEntry /*&& (Elem->Wcid == RESERVED_WCID)*/)
                     	{
                     	
				idx = wdev->DefaultKeyId;
                        	RTMP_SET_WCID_SEC_INFO(pAd, BSS0, idx,
                                	               pAd->SharedKey[BSS0][idx].CipherAlg,
                                        	       pEntry->wcid, SHAREDKEYTABLE);
#ifdef MT_MAC
				if (pAd->chipCap.hif_type == HIF_MT)
				{		
					MT_ADDREMOVE_KEY(pAd, 0, BSS0, idx, pEntry->wcid, PAIRWISEKEYTABLE,
		                			&pAd->SharedKey[BSS0][idx], pEntry->Addr);
				}
#endif /* MT_MAC */				
                     	}
                     }

                                if (pEntry && IS_ENTRY_CLIENT(pEntry))
                                {
                                        pEntry->LastBeaconRxTime = Now;
#ifdef IWSC_SUPPORT
                                        if (pEntry->bUpdateInfoFromPeerBeacon == FALSE)
                                        {
                                                if (StaUpdateMacTableEntry(pAd,
                                                                                                pEntry,
                                                                                                MaxSupportedRateIn500Kbps,
                                                                                                &bcn_ie_list->HtCapability,
                                                                                                bcn_ie_list->HtCapabilityLen,
                                                                                                &bcn_ie_list->AddHtInfo,
                                                                                                bcn_ie_list->AddHtInfoLen,
                                                                                                NULL,
                                                                                                bcn_ie_list->CapabilityInfo) == FALSE)
                                                {
                                                        DBGPRINT(RT_DEBUG_TRACE, ("ADHOC 2 - Add Entry failed.\n"));
                                                        return FALSE;
                                                }

                                                if (ADHOC_ON(pAd) && pEntry)
                                                {
                                                        RTMPSetSupportMCS(pAd,
                                                                                        OPMODE_STA,
                                                                                        pEntry,
                                                                                        bcn_ie_list->SupRate,

                                                                                        bcn_ie_list->SupRateLen,
                                                                                        bcn_ie_list->ExtRate,
                                                                                        bcn_ie_list->ExtRateLen,
#ifdef DOT11_VHT_AC
                                                                                        bcn_ie_list->vht_cap_len,
                                                                                        &bcn_ie_list->vht_cap_ie,
#endif /* DOT11_VHT_AC */
                                                                                        &bcn_ie_list->HtCapability,
                                                                                        bcn_ie_list->HtCapabilityLen);
                                                }

                                                pEntry->bUpdateInfoFromPeerBeacon = TRUE;
                                        }
#endif /* IWSC_SUPPORT */
                                }

                                /* At least another peer in this IBSS, declare MediaState as CONNECTED */
                                if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
                                {
                                        OPSTATUS_SET_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);
                                        RTMP_IndicateMediaState(pAd, NdisMediaStateConnected);
#ifdef MT76XX_BTCOEX_SUPPORT
                                        MLMEHook(pAd, WLAN_CONNECTION_COMPLETION, QueryHashID(pAd, pAd->CommonCfg.Bssid, FALSE));
#endif /*MT76XX_BTCOEX_SUPPORT*/
                        		pAd->ExtraInfo = GENERAL_LINK_UP;
                                        DBGPRINT(RT_DEBUG_TRACE, ("ADHOC  fOP_STATUS_MEDIA_STATE_CONNECTED.\n"));
                                }
#ifdef IWSC_SUPPORT
                                if (pAd->StaCfg.IWscInfo.bSendEapolStart &&
                                        (pAd->Mlme.IWscMachine.CurrState != IWSC_WAIT_PIN) &&
                                        (pAd->StaCfg.WscControl.WscConfMode == WSC_ENROLLEE))
                                {
                                        pAd->StaCfg.IWscInfo.bSendEapolStart = FALSE;
                                        pWpsCtrl->WscState = WSC_STATE_LINK_UP;
                                        pWpsCtrl->WscStatus = STATUS_WSC_LINK_UP;
                                        NdisMoveMemory(pWpsCtrl->EntryAddr, pWpsCtrl->WscPeerMAC, MAC_ADDR_LEN);
                                        WscSendEapolStart(pAd, pWpsCtrl->WscPeerMAC, STA_MODE);
                                }
#endif // IWSC_SUPPORT 
                        }
	return TRUE;
}

VOID Adhoc_checkPeerBeaconLost(RTMP_ADAPTER *pAd)
{
	UINT i = 0;
	UINT adhocGroupWcid = pAd->StaCfg.wdev.tr_tb_idx;
#if 0 /* Do not support Adhoc mode radar detection*/
       /*radar detect*/
       if ((pAd->CommonCfg.Channel > 14)
           && (pAd->CommonCfg.bIEEE80211H == 1)
           && RadarChannelCheck(pAd, pAd->CommonCfg.Channel))
	{
        	RadarDetectPeriodic(pAd);
        }
#endif

        /* If all peers leave, and this STA becomes the last one in this IBSS, then change MediaState
           to DISCONNECTED. But still holding this IBSS (i.e. sending BEACON) so that other STAs can
           join later.*/
        if (/*(RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastBeaconRxTime + ADHOC_BEACON_LOST_TIME)
            || (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK))
             && */OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
        {
#if 0  /* peer should check desired peer info first.*/
              MLME_START_REQ_STRUCT     StartReq;

              DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - excessive BEACON lost, last STA in this IBSS, MediaState=Disconnected\n")); 
              LinkDown(pAd, FALSE);

              StartParmFill(pAd, &StartReq, (CHAR *)pAd->MlmeAux.Ssid, pAd->MlmeAux.SsidLen);
              MlmeEnqueue(pAd, SYNC_STATE_MACHINE, MT2_MLME_START_REQ, sizeof(MLME_START_REQ_STRUCT), &StartReq, 0);
              pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_START;
#endif  /*peer should check desired peer info first.*/
        }

        for (i = BSSID_WCID; i < adhocGroupWcid; i++)
        {
        	MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[i];

		if (pEntry->wcid == 0)
			continue;

                if (IS_ENTRY_ADHOC(pEntry))
                {

                	//CFG TODO: Concurrent deauth to error pEntry ???

                	if (RTMP_TIME_AFTER(pAd->Mlme.Now32, pEntry->LastBeaconRxTime + ADHOC_BEACON_LOST_TIME)
#ifdef IWSC_SUPPORT
                 	/*
                    		2011/09/05:
                    		Broadcom test bed doesn't broadcast beacon when Broadcom is Enrollee.
                  	*/
                    		&& (pAd->StaCfg.WscControl.bWscTrigger == FALSE)
#endif /* IWSC_SUPPORT */
                    	   )
                    	{
                    		MlmeDeAuthAction(pAd, pEntry, REASON_DISASSOC_STA_LEAVING, FALSE);
#ifdef RT_CFG80211_SUPPORT
                        	CFG80211OS_DelSta(pAd->net_dev, pAd->CommonCfg.Bssid);
                                DBGPRINT(RT_DEBUG_TRACE, ("%s: del this ad-hoc %02x:%02x:%02x:%02x:%02x:%02x\n",
                                                __FUNCTION__, PRINT_MAC(pAd->CommonCfg.Bssid)));
#endif /* RT_CFG80211_SUPPORT */
                    	}
		}

	}

        if (pAd->MacTab.Size == 0)
        {
        	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);
                RTMP_IndicateMediaState(pAd, NdisMediaStateDisconnected);
        }
}

VOID LinkUp_Adhoc(RTMP_ADAPTER *pAd, struct wifi_dev *wdev)
{
	UINT8 idx;

#ifdef MT_MAC
    if (pAd->chipCap.hif_type == HIF_MT) 
	{
		AddTxSTypePerPktType(pAd, FC_TYPE_MGMT, SUBTYPE_BEACON, TXS_FORMAT0, BcnTxSHandler);
		TxSTypeCtlPerPktType(pAd, FC_TYPE_MGMT, SUBTYPE_BEACON, TXS_WLAN_IDX_ALL, 
								TXS_FORMAT0, FALSE, TRUE, FALSE, 0);
	}
#endif /* MT_MAC */

	MakeIbssBeacon(pAd);

	if ((pAd->CommonCfg.Channel > 14)
	    && (pAd->CommonCfg.bIEEE80211H == 1)
	    && RadarChannelCheck(pAd, pAd->CommonCfg.Channel)) {
		;	/*Do nothing */
	} 
	else
	{
		AsicEnableIbssSync(pAd);
	}

	/* In ad hoc mode, use MAC table from index 1. */
	/* p.s ASIC use all 0xff as termination of WCID table search.To prevent it's 0xff-ff-ff-ff-ff-ff, Write 0 here. */
	AsicDelWcidTab(pAd, MCAST_WCID);
	AsicDelWcidTab(pAd, 1);


#ifdef MT_MAC
	AsicUpdateRxWCIDTable(pAd, pAd->StaCfg.wdev.tr_tb_idx, pAd->MlmeAux.Bssid);
#endif /* MT_MAC */

	/* If WEP is enabled, add key material and cipherAlg into Asic */
	/* Fill in Shared Key Table(offset: 0x6c00) and Shared Key Mode(offset: 0x7000) */

	if (wdev->WepStatus == Ndis802_11WEPEnabled) {
		UCHAR CipherAlg;

		for (idx = 0; idx < SHARE_KEY_NUM; idx++) {
			CipherAlg = pAd->SharedKey[BSS0][idx].CipherAlg;

			if (pAd->SharedKey[BSS0][idx].KeyLen > 0) {
				/* Set key material and cipherAlg to Asic */
				AsicAddSharedKeyEntry(pAd, BSS0, idx,
						      &pAd->SharedKey[BSS0][idx]);

				if (idx == wdev->DefaultKeyId) {
					INT cnt;

					/* Generate 3-bytes IV randomly for software encryption using */
					for (cnt = 0; cnt < LEN_WEP_TSC; cnt++)
						pAd->SharedKey[BSS0][idx].TxTsc[cnt] = RandomByte(pAd);

					/* Update WCID attribute table and IVEIV table for this group key table */
					RTMPSetWcidSecurityInfo(pAd,
								BSS0,
								idx,
								CipherAlg,
								MCAST_WCID,
								SHAREDKEYTABLE);
				}
			}

#ifdef MT_MAC
			if (pAd->chipCap.hif_type == HIF_MT)
			{
				UCHAR groupWcid = pAd->StaCfg.wdev.tr_tb_idx;
				MT_ADDREMOVE_KEY(pAd, 0, BSS0, wdev->DefaultKeyId, groupWcid, SHAREDKEYTABLE,
                                    	&pAd->SharedKey[BSS0][wdev->DefaultKeyId],BROADCAST_ADDR);
			}
#endif /* MT_MAC */

		}
	}
	/* If WPANone is enabled, add key material and cipherAlg into Asic */
	/* Fill in Shared Key Table(offset: 0x6c00) and Shared Key Mode(offset: 0x7000) */
	else if (wdev->AuthMode == Ndis802_11AuthModeWPANone) {
		wdev->DefaultKeyId = 0;	/* always be zero */

		NdisZeroMemory(&pAd->SharedKey[BSS0][0], sizeof (CIPHER_KEY));
		pAd->SharedKey[BSS0][0].KeyLen = LEN_TK;
		NdisMoveMemory(pAd->SharedKey[BSS0][0].Key, pAd->StaCfg.PMK, LEN_TK);

		if (pAd->StaCfg.PairCipher == Ndis802_11TKIPEnable) {
			NdisMoveMemory(pAd->SharedKey[BSS0][0].RxMic,
				       &pAd->StaCfg.PMK[16], LEN_TKIP_MIC);
			NdisMoveMemory(pAd->SharedKey[BSS0][0].TxMic,
				       &pAd->StaCfg.PMK[16], LEN_TKIP_MIC);
		}

		/* Decide its ChiperAlg */
		if (pAd->StaCfg.PairCipher == Ndis802_11TKIPEnable)
			pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_TKIP;
		else if (pAd->StaCfg.PairCipher == Ndis802_11AESEnable)
			pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_AES;
		else {
			DBGPRINT(RT_DEBUG_TRACE,
				 ("Unknow Cipher (=%d), set Cipher to AES\n",
				  pAd->StaCfg.PairCipher));
			pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_AES;
		}

		/* Set key material and cipherAlg to Asic */
		AsicAddSharedKeyEntry(pAd,
				      BSS0, 0, &pAd->SharedKey[BSS0][0]);

		/* Update WCID attribute table and IVEIV table for this group key table */
		RTMPSetWcidSecurityInfo(pAd,
					BSS0, 0,
					pAd->SharedKey[BSS0][0].CipherAlg, MCAST_WCID,
					SHAREDKEYTABLE);

#ifdef MT_MAC
                        if (pAd->chipCap.hif_type == HIF_MT)
                        {
                                INT groupWcid = pAd->StaCfg.wdev.tr_tb_idx;
                                MT_ADDREMOVE_KEY(pAd, 0, BSS0, wdev->DefaultKeyId, groupWcid, SHAREDKEYTABLE,
                                        &pAd->SharedKey[BSS0][wdev->DefaultKeyId],BROADCAST_ADDR);
                        }
#endif /* MT_MAC */
	}
#ifdef ADHOC_WPA2PSK_SUPPORT
	else if ((wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
#ifdef WPA_SUPPLICANT_SUPPORT
		 && (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP == WPA_SUPPLICANT_DISABLE)
#endif /* WPA_SUPPLICANT_SUPPORT */
	    ) {
		USHORT Wcid = 0;

		wdev->DefaultKeyId = 0;	/* always be zero */

		NdisZeroMemory(&pAd->SharedKey[BSS0][0], sizeof (CIPHER_KEY));
		pAd->SharedKey[BSS0][0].KeyLen = LEN_TK;
		NdisMoveMemory(pAd->SharedKey[BSS0][0].Key, pAd->StaCfg.PMK, LEN_TK);
		pAd->SharedKey[BSS0][0].CipherAlg = CIPHER_AES;

		/* Generate GMK and GNonce randomly */
		GenRandom(pAd, pAd->CommonCfg.Bssid, pAd->StaCfg.GMK);
		GenRandom(pAd, pAd->CommonCfg.Bssid, pAd->StaCfg.GNonce);

		/* Derive GTK per BSSID */
		WpaDeriveGTK(pAd->StaCfg.GMK,
			     pAd->StaCfg.GNonce,
			     pAd->CurrentAddress,
			     pAd->StaCfg.GTK, LEN_TKIP_GTK);

		if (pAd->StaCfg.GroupCipher == Ndis802_11AESEnable) {
			NdisZeroMemory(&pAd->StaCfg.TxGTK, sizeof (CIPHER_KEY));
			NdisMoveMemory(pAd->StaCfg.TxGTK.Key, pAd->StaCfg.GTK, LEN_TK);
			pAd->StaCfg.TxGTK.CipherAlg = CIPHER_AES;
			pAd->StaCfg.TxGTK.KeyLen = LEN_TK;

			/* Add Pair-wise key to Asic */
			GET_GroupKey_WCID(pAd, Wcid, BSS0);
			AsicAddPairwiseKeyEntry(pAd,
						(UCHAR) Wcid,
						&pAd->StaCfg.TxGTK);

			RTMPSetWcidSecurityInfo(pAd,
						BSS0, 0,
						pAd->StaCfg.TxGTK.CipherAlg, (UCHAR) Wcid,
						PAIRWISEKEYTABLE);
		}
	}
#endif /* ADHOC_WPA2PSK_SUPPORT */


#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
	RT_CFG80211_JOIN_IBSS(pAd, pAd->MlmeAux.Bssid);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */

}

/*
	==========================================================================
	Description:
		Pre-build a BEACON frame in the shared memory

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	==========================================================================
*/
ULONG MakeIbssBeacon(RTMP_ADAPTER *pAd)
{
	UCHAR DsLen = 1, IbssLen = 2, *tmac_info;
	UCHAR LocalErpIe[3] = { IE_ERP, 1, 0x04 };
	HEADER_802_11 BcnHdr;
	USHORT CapabilityInfo;
	LARGE_INTEGER FakeTimestamp;
	ULONG FrameLen = 0;
	UCHAR *pBeaconFrame;
	BOOLEAN Privacy;
	UCHAR SupRate[MAX_LEN_OF_SUPPORTED_RATES] = {0};
	UCHAR SupRateLen = 0;
	UCHAR ExtRate[MAX_LEN_OF_SUPPORTED_RATES] = {0};
	UCHAR ExtRateLen = 0;
	struct wifi_dev *wdev = &pAd->StaCfg.wdev;
	MAC_TX_INFO mac_info;
	PNDIS_PACKET pkt;
#ifdef RT_CFG80211_SUPPORT
	PCFG80211_CTRL pCfg80211_ctrl;
#endif /* RT_CFG80211_SUPPORT */

	pkt = pAd->StaCfg.bcn_buf.BeaconPkt;
	ASSERT(pkt);

	tmac_info = GET_OS_PKT_DATAPTR(pkt);
	if (pAd->chipCap.hif_type == HIF_MT)
		pBeaconFrame = tmac_info + pAd->chipCap.tx_hw_hdr_len;
	else
		pBeaconFrame = tmac_info + pAd->chipCap.TXWISize;

	if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B)
	    && (pAd->CommonCfg.Channel <= 14)) {
		SupRate[0] = 0x82;	/* 1 mbps */
		SupRate[1] = 0x84;	/* 2 mbps */
		SupRate[2] = 0x8b;	/* 5.5 mbps */
		SupRate[3] = 0x96;	/* 11 mbps */
		SupRateLen = 4;
		ExtRateLen = 0;
	} else if (pAd->CommonCfg.Channel > 14) {
		SupRate[0] = 0x8C;	/* 6 mbps, in units of 0.5 Mbps, basic rate */
		SupRate[1] = 0x12;	/* 9 mbps, in units of 0.5 Mbps */
		SupRate[2] = 0x98;	/* 12 mbps, in units of 0.5 Mbps, basic rate */
		SupRate[3] = 0x24;	/* 18 mbps, in units of 0.5 Mbps */
		SupRate[4] = 0xb0;	/* 24 mbps, in units of 0.5 Mbps, basic rate */
		SupRate[5] = 0x48;	/* 36 mbps, in units of 0.5 Mbps */
		SupRate[6] = 0x60;	/* 48 mbps, in units of 0.5 Mbps */
		SupRate[7] = 0x6c;	/* 54 mbps, in units of 0.5 Mbps */
		SupRateLen = 8;
		ExtRateLen = 0;

		/* */
		/* Also Update MlmeRate & RtsRate for G only & A only */
		/* */
		pAd->CommonCfg.MlmeRate = RATE_6;
		pAd->CommonCfg.RtsRate = RATE_6;
		pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;
		pAd->CommonCfg.MlmeTransmit.field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.MlmeRate];
		pAd->MacTab.Content[BSS0Mcast_WCID].HTPhyMode.field.MODE = MODE_OFDM;
		pAd->MacTab.Content[BSS0Mcast_WCID].HTPhyMode.field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.MlmeRate];
	} else {
		SupRate[0] = 0x82;	/* 1 mbps */
		SupRate[1] = 0x84;	/* 2 mbps */
		SupRate[2] = 0x8b;	/* 5.5 mbps */
		SupRate[3] = 0x96;	/* 11 mbps */
		SupRateLen = 4;

		ExtRate[0] = 0x0C;	/* 6 mbps, in units of 0.5 Mbps, */
		ExtRate[1] = 0x12;	/* 9 mbps, in units of 0.5 Mbps */
		ExtRate[2] = 0x18;	/* 12 mbps, in units of 0.5 Mbps, */
		ExtRate[3] = 0x24;	/* 18 mbps, in units of 0.5 Mbps */
		ExtRate[4] = 0x30;	/* 24 mbps, in units of 0.5 Mbps, */
		ExtRate[5] = 0x48;	/* 36 mbps, in units of 0.5 Mbps */
		ExtRate[6] = 0x60;	/* 48 mbps, in units of 0.5 Mbps */
		ExtRate[7] = 0x6c;	/* 54 mbps, in units of 0.5 Mbps */
		ExtRateLen = 8;
	}

	pAd->StaActive.SupRateLen = SupRateLen;
	NdisMoveMemory(pAd->StaActive.SupRate, SupRate, SupRateLen);
	pAd->StaActive.ExtRateLen = ExtRateLen;
	NdisMoveMemory(pAd->StaActive.ExtRate, ExtRate, ExtRateLen);

	/* compose IBSS beacon frame */
	MgtMacHeaderInit(pAd, &BcnHdr, SUBTYPE_BEACON, 0, BROADCAST_ADDR,
						pAd->CurrentAddress,
			 pAd->CommonCfg.Bssid);
	Privacy = (wdev->WepStatus == Ndis802_11WEPEnabled)
	    || (wdev->WepStatus == Ndis802_11TKIPEnable)
	    || (wdev->WepStatus == Ndis802_11AESEnable);
	//printk("=========================> %d\n", Privacy);
	CapabilityInfo =
	    CAP_GENERATE(0, 1, Privacy,
			 (pAd->CommonCfg.TxPreamble == Rt802_11PreambleShort),
			 0, 0);

	MakeOutgoingFrame(pBeaconFrame, &FrameLen,
			  sizeof (HEADER_802_11), &BcnHdr,
			  TIMESTAMP_LEN, &FakeTimestamp,
			  2, &pAd->CommonCfg.BeaconPeriod,
			  2, &CapabilityInfo,
			  1, &SsidIe,
			  1, &pAd->CommonCfg.SsidLen,
			  pAd->CommonCfg.SsidLen, pAd->CommonCfg.Ssid,
			  1, &SupRateIe,
			  1, &SupRateLen,
			  SupRateLen, SupRate,
			  1, &DsIe,
			  1, &DsLen,
			  1, &pAd->CommonCfg.Channel,
			  1, &IbssIe,
			  1, &IbssLen, 2, &pAd->StaActive.AtimWin, END_OF_ARGS);

	/* add ERP_IE and EXT_RAE IE of in 802.11g */
	if (ExtRateLen) {
		ULONG tmp;

		MakeOutgoingFrame(pBeaconFrame + FrameLen, &tmp,
				  3, LocalErpIe,
				  1, &ExtRateIe,
				  1, &ExtRateLen,
				  ExtRateLen, ExtRate, END_OF_ARGS);
		FrameLen += tmp;
	}

#ifdef RT_CFG80211_SUPPORT
	pCfg80211_ctrl = &pAd->cfg80211_ctrl;
	if (pCfg80211_ctrl->BeaconExtraIeLen > 0 )
	{
		ULONG tmp;
		MakeOutgoingFrame(pBeaconFrame + FrameLen, &tmp,
				  pCfg80211_ctrl->BeaconExtraIeLen, pCfg80211_ctrl->BeaconExtraIe,
				  END_OF_ARGS);
		FrameLen += tmp;
	}

#else
	/* If adhoc secruity is set for WPA-None, append the cipher suite IE */
	/* Modify by Eddy, support WPA2PSK in Adhoc mode */
	if ((wdev->AuthMode == Ndis802_11AuthModeWPANone)
#ifdef ADHOC_WPA2PSK_SUPPORT
	    || (wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
#endif /* ADHOC_WPA2PSK_SUPPORT */
	    ) {
		UCHAR RSNIe = IE_WPA;
		ULONG tmp;

		RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, BSS0);
#ifdef ADHOC_WPA2PSK_SUPPORT
		if (wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
			RSNIe = IE_RSN;
#endif /* ADHOC_WPA2PSK_SUPPORT */

		MakeOutgoingFrame(pBeaconFrame + FrameLen, &tmp,
				  1, &RSNIe,
				  1, &pAd->StaCfg.RSNIE_Len,
				  pAd->StaCfg.RSNIE_Len, pAd->StaCfg.RSN_IE,
				  END_OF_ARGS);
		FrameLen += tmp;
	}
#ifdef WSC_STA_SUPPORT
	/* add Simple Config Information Element */
	if (pAd->StaCfg.WpsIEBeacon.ValueLen != 0) {
		ULONG WscTmpLen = 0;

		MakeOutgoingFrame(pBeaconFrame + FrameLen, &WscTmpLen,
				  pAd->StaCfg.WpsIEBeacon.ValueLen,
				  pAd->StaCfg.WpsIEBeacon.Value, END_OF_ARGS);
		FrameLen += WscTmpLen;
	}
#endif /* WSC_STA_SUPPORT */
#endif /* RT_CFG80211_SUPPORT */

#ifdef DOT11_N_SUPPORT
	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode)
	    && (pAd->StaCfg.bAdhocN == TRUE)) {
		ULONG TmpLen;
		UCHAR HtLen, HtLen1;

#ifdef RT_BIG_ENDIAN
		HT_CAPABILITY_IE HtCapabilityTmp;
		ADD_HT_INFO_IE addHTInfoTmp;
		USHORT b2lTmp, b2lTmp2;
#endif

		/* add HT Capability IE */
		HtLen = sizeof (pAd->CommonCfg.HtCapability);
		HtLen1 = sizeof (pAd->CommonCfg.AddHTInfo);
#ifndef RT_BIG_ENDIAN
		MakeOutgoingFrame(pBeaconFrame + FrameLen, &TmpLen,
				  1, &HtCapIe,
				  1, &HtLen,
				  HtLen, &pAd->CommonCfg.HtCapability,
				  1, &AddHtInfoIe,
				  1, &HtLen1,
				  HtLen1, &pAd->CommonCfg.AddHTInfo,
				  END_OF_ARGS);
#else
		NdisMoveMemory(&HtCapabilityTmp, &pAd->CommonCfg.HtCapability,
			       HtLen);
		*(USHORT *) (&HtCapabilityTmp.HtCapInfo) =
		    SWAP16(*(USHORT *) (&HtCapabilityTmp.HtCapInfo));
		*(USHORT *) (&HtCapabilityTmp.ExtHtCapInfo) =
		    SWAP16(*(USHORT *) (&HtCapabilityTmp.ExtHtCapInfo));

		NdisMoveMemory(&addHTInfoTmp, &pAd->CommonCfg.AddHTInfo,
			       HtLen1);
		*(USHORT *) (&addHTInfoTmp.AddHtInfo2) =
		    SWAP16(*(USHORT *) (&addHTInfoTmp.AddHtInfo2));
		*(USHORT *) (&addHTInfoTmp.AddHtInfo3) =
		    SWAP16(*(USHORT *) (&addHTInfoTmp.AddHtInfo3));

		MakeOutgoingFrame(pBeaconFrame + FrameLen, &TmpLen,
				  1, &HtCapIe,
				  1, &HtLen,
				  HtLen, &HtCapabilityTmp,
				  1, &AddHtInfoIe,
				  1, &HtLen1,
				  HtLen1, &addHTInfoTmp, END_OF_ARGS);
#endif
		FrameLen += TmpLen;
	}
#endif /* DOT11_N_SUPPORT */

	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));	
	mac_info.FRAG = FALSE;
	
	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = TRUE;
	mac_info.AMPDU = FALSE;
	
	mac_info.BM = 1;
	mac_info.Ack = FALSE;
	mac_info.NSeq = TRUE;
	mac_info.BASize = 0;
	
	mac_info.WCID = MCAST_WCID;
	mac_info.Length = FrameLen;
	
	mac_info.TxRate = RATE_1;
	mac_info.Txopmode = IFS_HTTXOP;
	mac_info.Preamble = LONG_PREAMBLE;
#ifdef MT_MAC
	mac_info.q_idx = Q_IDX_BCN;
	mac_info.TxSPriv = 0;
#endif /* MT_MAC */

	mac_info.hdr_len = 24;
	mac_info.bss_idx = 0;//Carter for GO/client case? what the value should be?

	/*beacon use reserved WCID 0xff */
	if (pAd->CommonCfg.Channel > 14) {
		// TODO: shiang-MT7603, fix me!
		write_tmac_info(pAd, tmac_info, &mac_info,
					      &pAd->CommonCfg.MlmeTransmit);
	} else {
		/* Set to use 1Mbps for Adhoc beacon. */
		HTTRANSMIT_SETTING Transmit;
		Transmit.word = 0;
		write_tmac_info(pAd, tmac_info, &mac_info,
						&Transmit);
	}

#ifdef RT_BIG_ENDIAN
	RTMPFrameEndianChange(pAd, pBeaconFrame, DIR_WRITE, FALSE);
	RTMPWIEndianChange(pAd, tmac_info, TYPE_TXWI);
#endif

#ifdef MT_MAC
    if (pAd->chipCap.hif_type == HIF_MT)
	   RT28xx_UpdateBeaconToAsic(pAd, 0, FrameLen, 0);
#endif /*MT_AMC*/
	DBGPRINT(RT_DEBUG_INFO,
		 ("MakeIbssBeacon (len=%ld), SupRateLen=%d, ExtRateLen=%d, Channel=%d, PhyMode=%d\n",
		  FrameLen, SupRateLen, ExtRateLen, pAd->CommonCfg.Channel,
		  pAd->CommonCfg.PhyMode));
	return FrameLen;
}

