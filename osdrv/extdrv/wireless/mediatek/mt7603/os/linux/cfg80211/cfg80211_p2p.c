/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2013, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/

/****************************************************************************
 
	Abstract:

	All related CFG80211 P2P function body.

	History:

***************************************************************************/

#define RTMP_MODULE_OS

#ifdef RT_CFG80211_SUPPORT

#include "rt_config.h"

#if defined (HE_BD_CFG80211_SUPPORT) && defined (BD_KERNEL_VER)
#undef  LINUX_VERSION_CODE
#define LINUX_VERSION_CODE KERNEL_VERSION(2,6,39)
#endif /* HE_BD_CFG80211_SUPPORT && BD_KERNEL_VER */


UCHAR CFG_WPS_OUI[4] = {0x00, 0x50, 0xf2, 0x04};
UCHAR CFG_P2POUIBYTE[4] = {0x50, 0x6f, 0x9a, 0x9}; /* spec. 1.14 OUI */

BUILD_TIMER_FUNCTION(CFG80211RemainOnChannelTimeout);

static 
VOID CFG80211_RemainOnChannelInit(RTMP_ADAPTER *pAd)
{
	if (pAd->cfg80211_ctrl.Cfg80211RocTimerInit == FALSE)
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("CFG80211_ROC : INIT Cfg80211RocTimer\n"));
		RTMPInitTimer(pAd, &pAd->cfg80211_ctrl.Cfg80211RocTimer, 
			GET_TIMER_FUNCTION(CFG80211RemainOnChannelTimeout), pAd, FALSE);
		pAd->cfg80211_ctrl.Cfg80211RocTimerInit = TRUE;
	}
}

UINT32 CFG80211_GetRestoreChannelTime(RTMP_ADAPTER *pAd)
{
	if (INFRA_ON(pAd)|| 
			RTMP_CFG80211_VIF_P2P_GO_ON(pAd)|| 
			(RTMP_CFG80211_VIF_P2P_CLI_ON(pAd) && RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd)))
	{
		return RESTORE_CH_TIME;
	}
	else
		return 0;
}

VOID CFG80211RemainOnChannelTimeout(
		PVOID SystemSpecific1, PVOID FunctionContext,
		PVOID SystemSpecific2, PVOID SystemSpecific3)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *) FunctionContext;
	PCFG80211_CTRL pCfg80211_ctrl = &pAd->cfg80211_ctrl;
	UCHAR RestoreChannel = pAd->LatchRfRegs.Channel; 
	UCHAR RestoreWidth = pAd->CommonCfg.BBPCurrentBW; 
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	APCLI_STRUCT *pApCliEntry = &pAd->ApCfg.ApCliTab[MAIN_MBSSID];
#endif
	BOOLEAN Cancelled;

	CFG80211DBG(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));
	
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST)) {
		CFG80211DBG(RT_DEBUG_OFF, ("CFG80211RemainOnChannelTimeout pAd->Flags = 0x%lx \n", pAd->Flags));
		return ;
	}

	/* Restore to infra channel and its bandwidth */
	if (INFRA_ON(pAd)|| 
			RTMP_CFG80211_VIF_P2P_GO_ON(pAd)|| 
			RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd))
	{
		/* For HT@20 */
		if ((pAd->LatchRfRegs.Channel != pAd->CommonCfg.Channel) ||
				(RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd) && (pAd->LatchRfRegs.Channel != pApCliEntry->MlmeAux.Channel)))
		{
			RestoreChannel = pAd->CommonCfg.Channel;
			RestoreWidth = BW_20;
		}

		/* For HT@40 */
		if ((pAd->CommonCfg.Channel != pAd->CommonCfg.CentralChannel) ||
				(RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd) && (pApCliEntry->MlmeAux.Channel != pApCliEntry->MlmeAux.CentralChannel)))
		{
			RestoreChannel = pAd->CommonCfg.CentralChannel;
			RestoreWidth = BW_40;
		}

		/* Switch Channel */
		if(RestoreChannel != pAd->LatchRfRegs.Channel)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("80211> [%s] Restore BW from %d -> %d, Channel %d -> %d\n", __FUNCTION__, 
						pAd->CommonCfg.BBPCurrentBW,
						RestoreWidth,
						pAd->LatchRfRegs.Channel, 
						RestoreChannel));

			bbp_set_bw(pAd, RestoreWidth);
			AsicSwitchChannel(pAd, RestoreChannel, FALSE);
			AsicLockChannel(pAd, RestoreChannel);	
			
			//Set listen channel flag to FALSE.
			pAd->cfg80211_ctrl.IsInListenProgress = FALSE;

			if (INFRA_ON(pAd))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("CONCURRENT STA PWR_ACTIVE ROC_END\n"));

				RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate,
						(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED) ? TRUE:FALSE),
						pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.Psm);

				RTMPSetTimer(&pCfg80211_ctrl->Cfg80211RocTimer, CFG80211_GetRestoreChannelTime(pAd));

				//Start SW Tx Dequeue
				if(RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET))
				{
					RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
					DBGPRINT(RT_DEBUG_OFF, ("80211> ROC TimeOut, Start SW Tx DeQ.(line=%d)\n", __LINE__));
				}
				return;
			}

			if (RTMP_CFG80211_VIF_P2P_GO_ON(pAd))
			{
				/* TODO: NOA? */
				CFG80211DBG(RT_DEBUG_TRACE, ("CFG_ROC: Cfg80211RocTimer Timeout\n"));
				RTMPCancelTimer(&pAd->cfg80211_ctrl.Cfg80211RocTimer, &Cancelled);
				pAd->cfg80211_ctrl.Cfg80211RocTimerRunning = FALSE;
				/* Report ROC Expired */
				CFG80211OS_RemainOnChannelExpired(FunctionContext, pCfg80211_ctrl);

				//Start SW Tx Dequeue
				if(RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET))
				{
					RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
					DBGPRINT(RT_DEBUG_OFF, ("80211> ROC TimeOut, Start SW Tx DeQ.(line=%d)\n", __LINE__));
				}
				return;
			}

			if (RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("CFG80211_ROC: ROC_Timeout APCLI_ON Channel: %d\n", 
							pApCliEntry->MlmeAux.Channel));
				CFG80211_P2pClientSendNullFrame(pAd, PWR_ACTIVE);
				RTMPSetTimer(&pCfg80211_ctrl->Cfg80211RocTimer, CFG80211_GetRestoreChannelTime(pAd));

				//Start SW Tx Dequeue
				if(RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET))
				{
					RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
					DBGPRINT(RT_DEBUG_OFF, ("80211> ROC TimeOut, Start SW Tx DeQ.(line=%d)\n", __LINE__));
				}
				return;
			}
		}
	}

	CFG80211DBG(RT_DEBUG_TRACE, ("CFG_ROC: Cfg80211RocTimer Timeout\n"));
	RTMPCancelTimer(&pAd->cfg80211_ctrl.Cfg80211RocTimer, &Cancelled);
	pAd->cfg80211_ctrl.Cfg80211RocTimerRunning = FALSE;

	//Enable_Tx2Q(pAd);

	//Start SW Tx Dequeue
	if(RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET))
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
		DBGPRINT(RT_DEBUG_OFF, ("80211> ROC TimeOut, Start SW Tx DeQ.(line=%d)\n", __LINE__));
	}

	/* Report ROC Expired */
	CFG80211OS_RemainOnChannelExpired(FunctionContext, pCfg80211_ctrl);
}

/* Set a given time on specific channel to listen action Frame */
BOOLEAN CFG80211DRV_OpsRemainOnChannel(VOID *pAdOrg, VOID *pData, UINT32 duration)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_CHAN *pChanInfo = (CMD_RTPRIV_IOCTL_80211_CHAN *) pData;
	BOOLEAN Cancelled;
	PCFG80211_CTRL pCfg80211_ctrl = &pAd->cfg80211_ctrl;
	UCHAR lock_channel = pChanInfo->ChanId;
	INT32 iRes = -1;

	CFG80211DBG(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));


	/* Channel Switch Case:
	 * 1. P2P_FIND:    [SOCIAL_CH]->[COM_CH]->[ROC_CH]--N_TUs->[ROC_TIMEOUT]
	 *                 Set COM_CH to ROC_CH for merge COM_CH & ROC_CH dwell section.
	 *
	 * 2. OFF_CH_WAIT: [ROC_CH]--200ms-->[ROC_TIMEOUT]->[COM_CH]
	 *                 Most in GO case.
	 *
	 */
	switch (pAd->CommonCfg.BBPCurrentBW)
	{
		case BW_40:
			if ((pChanInfo->ChanId == pAd->CommonCfg.CentralChannel)||
					(pChanInfo->ChanId == pAd->CommonCfg.Channel))
				lock_channel = pAd->CommonCfg.CentralChannel;
			break;

		case BW_20:
		default:
			if (pChanInfo->ChanId == pAd->CommonCfg.Channel)
				lock_channel = pAd->CommonCfg.Channel;
			break;
	}

	/* Switch to Desired Channel */ 
	if (lock_channel != pAd->LatchRfRegs.Channel)
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("CFG80211_PKT: ROC CHANNEL_LOCK %d\n", pChanInfo->ChanId));

		//Disable_Tx2Q(pAd);

		if (INFRA_ON(pAd))
		{
/*
			RTMPSendNullFrame(pAd, 
					pAd->CommonCfg.TxRate, 
					(pAd->CommonCfg.bWmmCapable & pAd->CommonCfg.APEdcaParm.bValid),
					pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.Psm);
			OS_WAIT(10);
*/

			//Stop SW Tx dequeue.
			if(!RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET))
			{
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
				DBGPRINT(RT_DEBUG_OFF, ("80211> ROC, Stop SW Tx DeQ\n"));
			}

			if(MTUsbPollingTxQBufferAvailable(pAd) != STATUS_SUCCESS)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("[warning]%s(%d):Tx buffer have data.\n", __FUNCTION__, __LINE__));
			}

			iRes = RTMPSendNullFrameAndWaitStatus(pAd, PWR_SAVE);
			if(iRes == -1)
			{
				DBGPRINT(RT_DEBUG_WARN, ("%s(line=%d):Send null frame error.\n", __FUNCTION__, __LINE__));
			}
		}	

#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
		if (RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd))	
		{
			CFG80211DBG(RT_DEBUG_TRACE, ("CFG80211_NULL: APCLI PWR_SAVE ROC_START\n"));
			CFG80211_P2pClientSendNullFrame(pAd, PWR_SAVE);
		}
#endif /*RT_CFG80211_P2P_CONCURRENT_DEVICE */

		bbp_set_bw(pAd, BW_20);
		AsicSwitchChannel(pAd, lock_channel, FALSE);
		AsicLockChannel(pAd, lock_channel);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> ComCH == ROC_CH (20/40/80)\n"));
	}

	/* Store Chan Info */
	NdisCopyMemory(&pCfg80211_ctrl->Cfg80211ChanInfo, pChanInfo, sizeof(CMD_RTPRIV_IOCTL_80211_CHAN));

	/* ROC Timer Init */
	CFG80211_RemainOnChannelInit(pAd);

	if (RTMP_CFG80211_ROC_ON(pAd))
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("%s CANCEL Cfg80211RocTimer\n", __FUNCTION__));
		RTMPCancelTimer(&pCfg80211_ctrl->Cfg80211RocTimer, &Cancelled);
		pCfg80211_ctrl->Cfg80211RocTimerRunning = FALSE;
	}

	/* In case of ROC is not for listen state */
	if (duration < MAX_ROC_TIME)	
		RTMPSetTimer(&pCfg80211_ctrl->Cfg80211RocTimer, duration);
	else
		RTMPSetTimer(&pCfg80211_ctrl->Cfg80211RocTimer, (duration - CFG80211_GetRestoreChannelTime(pAd)));

	pCfg80211_ctrl->Cfg80211RocTimerRunning = TRUE;

	/* Report ROC Ready */
	CFG80211OS_ReadyOnChannel(pAdOrg, pChanInfo, duration);

	return TRUE;
}

BOOLEAN CFG80211DRV_OpsCancelRemainOnChannel(VOID *pAdOrg, UINT32 cookie)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	PCFG80211_CTRL pCfg80211_ctrl = &pAd->cfg80211_ctrl;
	BOOLEAN Cancelled;
	UCHAR RestoreChannel = pAd->LatchRfRegs.Channel; 
	UCHAR RestoreWidth = pAd->CommonCfg.BBPCurrentBW; 
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	APCLI_STRUCT *pApCliEntry = &pAd->ApCfg.ApCliTab[MAIN_MBSSID];
#endif	

	CFG80211DBG(RT_DEBUG_TRACE, ("%s\n", __FUNCTION__));

	if (!RTMP_CFG80211_ROC_ON(pAd))
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("%s: No running ROC\n", __FUNCTION__));
		return TRUE;
	}	

	/* Check if other link exists, or do not channel switch */
	if (INFRA_ON(pAd)|| 
			RTMP_CFG80211_VIF_P2P_GO_ON(pAd)|| 
			RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd))
	{
		/* For HT@20 */
		if ((pAd->LatchRfRegs.Channel != pAd->CommonCfg.Channel) ||
				(RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd) && (pAd->LatchRfRegs.Channel != pApCliEntry->MlmeAux.Channel)))
		{
			RestoreChannel = pAd->CommonCfg.Channel;
			RestoreWidth = BW_20;
		}

		/* For HT@40 */
		if ((pAd->CommonCfg.Channel != pAd->CommonCfg.CentralChannel) ||
				(RTMP_CFG80211_VIF_P2P_CLI_CONNECTED(pAd) && (pApCliEntry->MlmeAux.Channel != pApCliEntry->MlmeAux.CentralChannel)))
		{
			RestoreChannel = pAd->CommonCfg.CentralChannel;
			RestoreWidth = BW_40;
		}

		if (RestoreChannel != pAd->LatchRfRegs.Channel)
		{
			CFG80211DBG(RT_DEBUG_TRACE, ("80211> [%s] Restore BW from %d -> %d, Channel %d -> %d\n", __FUNCTION__, 
						pAd->CommonCfg.BBPCurrentBW,
						RestoreWidth,
						pAd->LatchRfRegs.Channel, 
						RestoreChannel));

			bbp_set_bw(pAd, RestoreWidth);
			AsicSwitchChannel(pAd, RestoreChannel, FALSE);
			AsicLockChannel(pAd, RestoreChannel);	

			//Set listen channel flag to FALSE.
			pAd->cfg80211_ctrl.IsInListenProgress = FALSE;

			if (INFRA_ON(pAd))
			{
				RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate,
						(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED) ? TRUE:FALSE),
						pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.Psm);
			}
		}
		else
			CFG80211DBG(RT_DEBUG_TRACE, ("80211> [%s] No need to change current CH: %d & BW: %d \n", __FUNCTION__, 
						pAd->CommonCfg.BBPCurrentBW,
						pAd->LatchRfRegs.Channel));
	}
	else
		CFG80211DBG(RT_DEBUG_TRACE, ("%s: No other link exists, DO NOT switch channel\n", __FUNCTION__));

	//Start SW Tx Dequeue
	if(RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET))
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET);
		DBGPRINT(RT_DEBUG_OFF, ("80211> ROC Cancel, Start SW Tx DeQ\n"));
	}

	CFG80211DBG(RT_DEBUG_TRACE, ("CFG_ROC : CANCEL Cfg80211RocTimer\n"));
	RTMPCancelTimer(&pAd->cfg80211_ctrl.Cfg80211RocTimer, &Cancelled);
	pAd->cfg80211_ctrl.Cfg80211RocTimerRunning = FALSE;
	CFG80211OS_RemainOnChannelExpired(pAd, pCfg80211_ctrl);// send event to supplicant
	return TRUE;
}

INT CFG80211_setPowerMgmt(VOID *pAdCB, UINT Enable)
{
#ifdef RT_CFG80211_P2P_SUPPORT	
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
#endif /* RT_CFG80211_P2P_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("@@@ %s: %d\n", __FUNCTION__, Enable));

#ifdef RT_CFG80211_P2P_SUPPORT		
	pAd->cfg80211_ctrl.bP2pCliPmEnable = (BOOLEAN)Enable;
#endif /* RT_CFG80211_P2P_SUPPORT */

	return 0;	
}

#if defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) || defined(CFG80211_MULTI_STA)
VOID CFG80211_P2pClientSendNullFrame(VOID *pAdCB, INT PwrMgmt)
{
    PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
    MAC_TABLE_ENTRY *pEntry;

    pEntry = MacTableLookup(pAd, pAd->ApCfg.ApCliTab[MAIN_MBSSID].MlmeAux.Bssid);
    if (pEntry == NULL)
    {
            DBGPRINT(RT_DEBUG_TRACE, ("CFG80211_ROC: Can't Find In Table: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                               PRINT_MAC(pAd->ApCfg.ApCliTab[MAIN_MBSSID].MlmeAux.Bssid)));
    }
    else
    {
            ApCliRTMPSendNullFrame(pAd,
                                   RATE_6,
                                   (CLIENT_STATUS_TEST_FLAG(pEntry, fCLIENT_STATUS_WMM_CAPABLE)) ? TRUE:FALSE,
									pEntry, (USHORT)PwrMgmt);
            OS_WAIT(20);
    }
}

VOID CFG80211DRV_P2pClientKeyAdd(VOID *pAdOrg, VOID *pData)
{

	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	CMD_RTPRIV_IOCTL_80211_KEY *pKeyInfo;
	
    	DBGPRINT(RT_DEBUG_TRACE, ("CFG Debug: CFG80211DRV_P2pClientKeyAdd\n"));
    	pKeyInfo = (CMD_RTPRIV_IOCTL_80211_KEY *)pData;
	
	if (pKeyInfo->KeyType == RT_CMD_80211_KEY_WEP40 || pKeyInfo->KeyType == RT_CMD_80211_KEY_WEP104)
		;
	else
	{	
		INT 	BssIdx;
		PAPCLI_STRUCT pApCliEntry;
		MAC_TABLE_ENTRY	*pMacEntry=(MAC_TABLE_ENTRY *)NULL;
		STA_TR_ENTRY *tr_entry;
		struct wifi_dev *p2p_wdev = NULL;

		BssIdx = pAd->ApCfg.BssidNum + MAX_MESH_NUM + MAIN_MBSSID;
		pApCliEntry = &pAd->ApCfg.ApCliTab[MAIN_MBSSID];
		
		p2p_wdev = &(pApCliEntry->wdev);

		pMacEntry = &pAd->MacTab.Content[pApCliEntry->MacTabWCID]; 
		tr_entry = &pAd->MacTab.tr_entry[pMacEntry->wcid];
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
        	if (pKeyInfo->bPairwise == FALSE )
#else
        	if (pKeyInfo->KeyId > 0)
#endif		
		{
			
			if (pApCliEntry->wdev.WepStatus == Ndis802_11Encryption3Enabled)
			{
				DBGPRINT(RT_DEBUG_OFF,
					("APCLI: Set AES Security Set. [%d] (GROUP) %d\n",
					BssIdx, pKeyInfo->KeyLen));
				if (!memcmp(pApCliEntry->SharedKey[pKeyInfo->KeyId].Key,
					pKeyInfo->KeyBuf, LEN_TK)) {
					DBGPRINT(RT_DEBUG_WARN, ("%s skip reinstall gtk\n",
						 __func__));
					goto skip_reinstall_gtk;
				}
				pMacEntry->rx_ccmp_pn_bmc[pKeyInfo->KeyId] = 0;
				pMacEntry->rx_ccmp_pn_bmc_zero[pKeyInfo->KeyId] = TRUE;
				NdisZeroMemory(&pApCliEntry->SharedKey[pKeyInfo->KeyId], sizeof(CIPHER_KEY));  
				pApCliEntry->SharedKey[pKeyInfo->KeyId].KeyLen = LEN_TK;
				NdisMoveMemory(pApCliEntry->SharedKey[pKeyInfo->KeyId].Key, pKeyInfo->KeyBuf, pKeyInfo->KeyLen);
				
				pApCliEntry->SharedKey[pKeyInfo->KeyId].CipherAlg = CIPHER_AES;

				AsicAddSharedKeyEntry(pAd, (UCHAR)BssIdx, pKeyInfo->KeyId,
						      &pApCliEntry->SharedKey[pKeyInfo->KeyId]);

					
				RTMPAddWcidAttributeEntry(pAd, (UCHAR)BssIdx, pKeyInfo->KeyId,
							  pApCliEntry->SharedKey[pKeyInfo->KeyId].CipherAlg, 
							  NULL);				

				if (INFRA_ON(pAd))	
				{
					if((pAd->StaCfg.wdev.bw != p2p_wdev->bw) && ((pAd->StaCfg.wdev.channel == p2p_wdev->channel)))
					{
						pAd->Mlme.bStartScc = TRUE;
						AsicSwitchChannel(pAd, pAd->StaCfg.wdev.CentralChannel, FALSE);
						AsicLockChannel(pAd, pAd->StaCfg.wdev.CentralChannel);	
						bbp_set_bw(pAd, BW_40);
					}
				}

				

#ifdef MT_MAC
				if (pAd->chipCap.hif_type == HIF_MT)
	        			CmdProcAddRemoveKey(pAd, 0, BSS0, pKeyInfo->KeyId, APCLI_MCAST_WCID, 
						    SHAREDKEYTABLE, &pApCliEntry->SharedKey[pKeyInfo->KeyId], 
						    BROADCAST_ADDR);
#endif /* MT_MAC*/										  
skip_reinstall_gtk:
				if (pMacEntry->AuthMode >= Ndis802_11AuthModeWPA)
				{
					/* set 802.1x port control */
					tr_entry->PortSecured = WPA_802_1X_PORT_SECURED;
					pMacEntry->PrivacyFilter = Ndis802_11PrivFilterAcceptAll;
				}
			}
		}	
		else
		{	
			if(pMacEntry)
			{
				DBGPRINT(RT_DEBUG_OFF,
					("APCLI: Set AES Security Set. [%d] (PAIRWISE) %d\n",
					BssIdx, pKeyInfo->KeyLen));
				if (!memcmp(pMacEntry->PairwiseKey.Key,
					pKeyInfo->KeyBuf, LEN_TK)) {
					DBGPRINT(RT_DEBUG_WARN, ("%s skip reinstall ptk\n",
						 __func__));
					goto skip_reinstall_ptk;
				}

				pMacEntry->rx_ccmp_pn_uc = 0;
				NdisZeroMemory(&pMacEntry->PairwiseKey, sizeof(CIPHER_KEY));  
				pMacEntry->PairwiseKey.KeyLen = LEN_TK;
				
				NdisCopyMemory(&pMacEntry->PTK[OFFSET_OF_PTK_TK], pKeyInfo->KeyBuf, OFFSET_OF_PTK_TK);
				NdisMoveMemory(pMacEntry->PairwiseKey.Key, &pMacEntry->PTK[OFFSET_OF_PTK_TK], pKeyInfo->KeyLen);
				
				pMacEntry->PairwiseKey.CipherAlg = CIPHER_AES;
				
				AsicAddPairwiseKeyEntry(pAd, (UCHAR)pMacEntry->Aid, &pMacEntry->PairwiseKey);
				RTMPSetWcidSecurityInfo(pAd, (UINT8)BssIdx, 0,
					pMacEntry->PairwiseKey.CipherAlg,
					(UINT8)(pMacEntry->Aid), PAIRWISEKEYTABLE);
skip_reinstall_ptk:
			;
#ifdef MT_MAC
    			if (pAd->chipCap.hif_type == HIF_MT)
            			CmdProcAddRemoveKey(pAd, 0, pMacEntry->func_tb_idx, 0, pMacEntry->wcid, 
						    PAIRWISEKEYTABLE, &pMacEntry->PairwiseKey, 
						    pMacEntry->Addr);
#endif /* MT_MAC*/

#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE

			// p2p cli key done case!!
			//check if Infra STA is port secured
			if (INFRA_ON(pAd))
			{
				if (!(pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & WPA_SUPPLICANT_ENABLE_WPS)) //WPS not under goining!
				{
					struct wifi_dev *wdev = &pAd->StaCfg.wdev;
					BOOLEAN bStart = FALSE;
					PAPCLI_STRUCT pApCliEntry = &pAd->ApCfg.ApCliTab[MAIN_MBSSID];
					struct wifi_dev *p2p_wdev = NULL;			
					p2p_wdev = &pApCliEntry->wdev;
					/*check the security setting  OPEN ==> just start; security ==> check key done then start*/
					switch (wdev->AuthMode)
					{
						case Ndis802_11AuthModeAutoSwitch:
							//fall through
						case Ndis802_11AuthModeShared:
						case Ndis802_11AuthModeWPAPSK:
						case Ndis802_11AuthModeWPA2PSK:
						case Ndis802_11AuthModeWPA1PSKWPA2PSK:
#ifdef WPA3_SUPPORT
						case Ndis802_11AuthModeWPA2PSKWPA3SAE:
						case Ndis802_11AuthModeWPA3SAE:
#endif
						case Ndis802_11AuthModeWPA:
						case Ndis802_11AuthModeWPA2:
						case Ndis802_11AuthModeWPA1WPA2:
							if (pAd->StaCfg.wdev.PortSecured ==WPA_802_1X_PORT_SECURED)
							{
								bStart = TRUE;
							}
							break;
						default:
							bStart = TRUE;
							break;
				       }
				
					if (bStart == TRUE)
					{
						if ((wdev->bw != p2p_wdev->bw) && ((wdev->channel == p2p_wdev->channel)))
						{
							pAd->Mlme.bStartScc = TRUE;				
						}				
					}
				}
			}
#endif /*RT_CFG80211_P2P_CONCURRENT_DEVICE */


			}
			else	
			{
				DBGPRINT(RT_DEBUG_OFF,
					("APCLI: Set AES Security Set. But pMacEntry NULL\n"));
			}			
		}		
	}
}

VOID CFG80211DRV_SetP2pCliAssocIe(VOID *pAdOrg, const VOID *pData, UINT ie_len)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	APCLI_STRUCT *apcli_entry;
	hex_dump("P2PCLI=", pData, ie_len);

	apcli_entry = &pAd->ApCfg.ApCliTab[MAIN_MBSSID];
	
	if (ie_len > 0)	
	{
		if (apcli_entry->wpa_supplicant_info.pWpaAssocIe)
		{
			os_free_mem(NULL, apcli_entry->wpa_supplicant_info.pWpaAssocIe);
			apcli_entry->wpa_supplicant_info.pWpaAssocIe = NULL;
		}

		os_alloc_mem(NULL, (UCHAR **)&apcli_entry->wpa_supplicant_info.pWpaAssocIe, ie_len);
		if (apcli_entry->wpa_supplicant_info.pWpaAssocIe)
		{
			apcli_entry->wpa_supplicant_info.WpaAssocIeLen = ie_len;
			NdisMoveMemory(apcli_entry->wpa_supplicant_info.pWpaAssocIe, pData, apcli_entry->wpa_supplicant_info.WpaAssocIeLen);
		}
		else
			apcli_entry->wpa_supplicant_info.WpaAssocIeLen = 0;
	}
	else
	{
		if (apcli_entry->wpa_supplicant_info.pWpaAssocIe)
		{
			os_free_mem(NULL, apcli_entry->wpa_supplicant_info.pWpaAssocIe);
			apcli_entry->wpa_supplicant_info.pWpaAssocIe = NULL;
		}
		apcli_entry->wpa_supplicant_info.WpaAssocIeLen = 0;
	}
}

#ifdef WPA3_SUPPORT
VOID CFG80211DRV_SetP2pCliAuthIe(
	VOID						*pAdOrg,
	VOID						*pData,
	UINT                         ie_len)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
	UCHAR ifIndex;
	APCLI_STRUCT *apcli_entry;

	ifIndex = pObj->ioctl_if;
	apcli_entry = &pAd->ApCfg.ApCliTab[ifIndex];

	hex_dump("P2PCLI=", pData, ie_len);

	if (ie_len > 0) {
		if (apcli_entry->wpa_supplicant_info.pWpaAuthIe) {
			os_free_mem(NULL, apcli_entry->wpa_supplicant_info.pWpaAuthIe);
			apcli_entry->wpa_supplicant_info.pWpaAuthIe = NULL;
		}

		os_alloc_mem(NULL, (UCHAR **)&apcli_entry->wpa_supplicant_info.pWpaAuthIe, ie_len);
		if (apcli_entry->wpa_supplicant_info.pWpaAuthIe) {
			apcli_entry->wpa_supplicant_info.WpaAuthIeLen = ie_len;
			NdisMoveMemory(apcli_entry->wpa_supplicant_info.pWpaAuthIe, pData, apcli_entry->wpa_supplicant_info.WpaAuthIeLen);
		} else
			apcli_entry->wpa_supplicant_info.WpaAuthIeLen = 0;
	} else {
		if (apcli_entry->wpa_supplicant_info.pWpaAuthIe) {
			os_free_mem(NULL, apcli_entry->wpa_supplicant_info.pWpaAuthIe);
			apcli_entry->wpa_supplicant_info.pWpaAuthIe = NULL;
		}
		apcli_entry->wpa_supplicant_info.WpaAuthIeLen = 0;
	}
}
#endif

/* For P2P_CLIENT Connection Setting in AP_CLI SM */
BOOLEAN CFG80211DRV_P2pClientConnect(VOID *pAdOrg, VOID *pData)
{
	CMD_RTPRIV_IOCTL_80211_CONNECT *pConnInfo;
	UCHAR Connect_SSID[NDIS_802_11_LENGTH_SSID + 1];
	UINT32 Connect_SSIDLen;
#ifdef WPA3_SUPPORT
	ULONG BssIdx = BSS_NOT_FOUND;
#endif
	APCLI_STRUCT *apcli_entry;
	POS_COOKIE pObj;
	UCHAR ifIndex;

	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdOrg;
	apcli_entry = &pAd->ApCfg.ApCliTab[MAIN_MBSSID];

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pObj->ioctl_if_type = INT_APCLI;
	ifIndex = pObj->ioctl_if;

	pConnInfo = (CMD_RTPRIV_IOCTL_80211_CONNECT *)pData;

	DBGPRINT(RT_DEBUG_OFF, ("APCLI Connection onGoing.....\n"));
#ifdef WPA3_SUPPORT
	pAd->CommonCfg.Channel = pConnInfo->ucChannelNum;

	CFG80211DBG(RT_DEBUG_TRACE, ("pConnInfo->pBssid is [%02x:%02x:%02x:%02x:%02x:%02x]\n", PRINT_MAC(pConnInfo->pBssid)));

	BssIdx = BssTableSearch(&pAd->ScanTab, (PUCHAR)pConnInfo->pBssid, pConnInfo->ucChannelNum);
	if (BssIdx == BSS_NOT_FOUND) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s:BSS_NOT_FOUND in last Scan Result!\n", __func__));
		return FALSE;
	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Find BssEntry[%ld] in Scan Result, SsidLen = %d.\n", __func__, BssIdx, pAd->ScanTab.BssEntry[BssIdx].SsidLen));
		memset(&Connect_SSID, 0, sizeof(Connect_SSID));
		if (pAd->ScanTab.BssEntry[BssIdx].SsidLen != 0) {
			Connect_SSIDLen = pAd->ScanTab.BssEntry[BssIdx].SsidLen;
			if (Connect_SSIDLen > NDIS_802_11_LENGTH_SSID)
				Connect_SSIDLen = NDIS_802_11_LENGTH_SSID;
			memcpy(Connect_SSID, pAd->ScanTab.BssEntry[BssIdx].Ssid, Connect_SSIDLen);
			DBGPRINT(RT_DEBUG_TRACE, ("!!!%s: Ssid is %s!!!\n", __func__, pAd->ScanTab.BssEntry[BssIdx].Ssid));
		} else
			Connect_SSIDLen = 0;
		NdisZeroMemory(apcli_entry->MlmeAux.Bssid, MAC_ADDR_LEN);
		NdisMoveMemory(apcli_entry->MlmeAux.Bssid, pConnInfo->pBssid, MAC_ADDR_LEN);
		NdisZeroMemory(apcli_entry->MlmeAux.Ssid, MAX_LEN_OF_SSID);
		NdisMoveMemory(apcli_entry->MlmeAux.Ssid, Connect_SSID, Connect_SSIDLen);
		apcli_entry->MlmeAux.SsidLen = Connect_SSIDLen;
	}

#else
	Connect_SSIDLen = pConnInfo->SsidLen;
	if (Connect_SSIDLen > NDIS_802_11_LENGTH_SSID)
		Connect_SSIDLen = NDIS_802_11_LENGTH_SSID;
	
	memset(&Connect_SSID, 0, sizeof(Connect_SSID));
	memcpy(Connect_SSID, pConnInfo->pSsid, Connect_SSIDLen);
#endif
	apcli_entry->wpa_supplicant_info.WpaSupplicantUP = WPA_SUPPLICANT_ENABLE;

	/* Check the connection is WPS or not */
	if (pConnInfo->bWpsConnection) {
		DBGPRINT(RT_DEBUG_TRACE, ("AP_CLI WPS Connection onGoing.....\n"));
		apcli_entry->wpa_supplicant_info.WpaSupplicantUP |= WPA_SUPPLICANT_ENABLE_WPS;
	}

	/* Set authentication mode */
#ifdef WPA3_SUPPORT
	if (pConnInfo->AuthType == Ndis802_11AuthModeWPA3SAE)
		Set_ApCli_AuthMode_Proc(pAd, "SAE");
	else
#else
	if (pConnInfo->WpaVer == 2)
	{
		if (!pConnInfo->FlgIs8021x)
		{
			DBGPRINT(RT_DEBUG_TRACE,("APCLI WPA2PSK\n"));
			Set_ApCli_AuthMode_Proc(pAd, "WPA2PSK");
		}
	}
	else if (pConnInfo->WpaVer == 1)
	{
		if (!pConnInfo->FlgIs8021x) 
		{
			DBGPRINT(RT_DEBUG_TRACE,("APCLI WPAPSK\n"));
			Set_ApCli_AuthMode_Proc(pAd, "WPAPSK");
		}
	}
	else
#endif
	if (pConnInfo->AuthType == Ndis802_11AuthModeAutoSwitch)
		Set_ApCli_AuthMode_Proc(pAd, "WEPAUTO");
    else if (pConnInfo->AuthType == Ndis802_11AuthModeShared)
		Set_ApCli_AuthMode_Proc(pAd, "SHARED");
	else
		Set_ApCli_AuthMode_Proc(pAd, "OPEN");

#ifdef WPA3_SUPPORT
	/* set extra ies to pAd->ApCfg.ApCliTab[ifIndex] */
	if (pConnInfo->ucExtraIeLen > 0)
		RTMP_DRIVER_80211_P2PCLI_AUTH_IE_SET(pAd, (void *)pConnInfo->aucExtraIe, pConnInfo->ucExtraIeLen);
	else if (pConnInfo->ucExtraIeLen == 0)
		RTMP_DRIVER_80211_P2PCLI_AUTH_IE_SET(pAd, NULL, 0);
#endif

#ifndef WPA3_SUPPORT
	/* Set PTK Encryption Mode */
	if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP) {
		DBGPRINT(RT_DEBUG_TRACE,("AES\n"));
		Set_ApCli_EncrypType_Proc(pAd, "AES");
	}
	else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP) {
		DBGPRINT(RT_DEBUG_TRACE,("TKIP\n"));
		Set_ApCli_EncrypType_Proc(pAd, "TKIP");
	}
	else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_WEP)
	{
		DBGPRINT(RT_DEBUG_TRACE,("WEP\n"));
		Set_ApCli_EncrypType_Proc(pAd, "WEP");
	}
#endif
#ifndef WPA3_SUPPORT
	if (pConnInfo->pBssid != NULL)
#endif
	{
		NdisZeroMemory(pAd->ApCfg.ApCliTab[ifIndex].CfgApCliBssid, MAC_ADDR_LEN);
		NdisCopyMemory(pAd->ApCfg.ApCliTab[ifIndex].CfgApCliBssid, pConnInfo->pBssid, MAC_ADDR_LEN);
#ifdef P2P_SUPPORT
		NdisCopyMemory(pAd->P2pCfg.Bssid, pConnInfo->pBssid, MAC_ADDR_LEN);
#endif
	}
	
	OPSTATUS_SET_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED);

	pAd->cfg80211_ctrl.FlgCfg80211Connecting = TRUE;
	Set_ApCli_Ssid_Proc(pAd, (RTMP_STRING *)Connect_SSID);
	Set_ApCli_Enable_Proc(pAd, "1");
#ifdef WPA3_SUPPORT
	CFG80211DBG(RT_DEBUG_ERROR, ("80211> APCLI CONNECTING SSID = %s, BSSID = [%02x:%02x:%02x:%02x:%02x:%02x]\n", Connect_SSID, PRINT_MAC(apcli_entry->CfgApCliBssid)));
#else
	CFG80211DBG(RT_DEBUG_OFF, ("80211> APCLI CONNECTING SSID = %s\n", Connect_SSID));
#endif

	return TRUE;	
}

VOID CFG80211_P2pClientConnectResultInform(
	VOID *pAdCB, UCHAR *pBSSID, UCHAR *pReqIe, UINT32 ReqIeLen, 
	UCHAR *pRspIe, UINT32 RspIeLen, UCHAR FlgIsSuccess)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;

	CFG80211OS_P2pClientConnectResultInform(pAd->ApCfg.ApCliTab[MAIN_MBSSID].wdev.if_dev, pBSSID, 
					pReqIe, ReqIeLen, pRspIe, RspIeLen, FlgIsSuccess);

	pAd->cfg80211_ctrl.FlgCfg80211Connecting = FALSE;
}

VOID CFG80211_LostP2pGoInform(VOID *pAdCB, UINT16 Reason)

{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdCB;
	PNET_DEV pNetDev = NULL;
	UINT checkDevType = RT_CMD_80211_IFTYPE_P2P_CLIENT;

#ifdef CFG80211_MULTI_STA
	checkDevType = RT_CMD_80211_IFTYPE_STATION;
#endif /* CFG80211_MULTI_STA */

	DBGPRINT(RT_DEBUG_TRACE, ("80211> CFG80211_LostGoInform ==> \n"));

	pAd->cfg80211_ctrl.FlgCfg80211Connecting = FALSE;
	if ((pAd->cfg80211_ctrl.Cfg80211VifDevSet.vifDevList.size > 0) &&        
	    ((pNetDev = RTMP_CFG80211_FindVifEntry_ByType(pAd, checkDevType)) != NULL)
	   )
	{

#if 0 //MCC
        if(pAd->cfg80211_ctrl.isMccOn)
        {
				DBGPRINT(RT_DEBUG_OFF,
					("MCC:==============================> off by GC\n"));
                pAd->cfg80211_ctrl.isMccOn = FALSE;
                CmdMccStop(pAd, NULL);
        }
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	        if (pNetDev->ieee80211_ptr->sme_state == CFG80211_SME_CONNECTING)
       	 	{
                   cfg80211_connect_result(pNetDev, NULL, NULL, 0, NULL, 0,
                                                                   WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_KERNEL);
        	}
        	else if (pNetDev->ieee80211_ptr->sme_state == CFG80211_SME_CONNECTED)
        	{
                   cfg80211_disconnected(pNetDev, WLAN_REASON_DEAUTH_LEAVING, NULL, 0, GFP_KERNEL);
        	}
#else
					cfg80211_disconnected(pNetDev, Reason, NULL, 0,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 2, 0))
						FALSE,/* locally_generated */
#endif
						GFP_KERNEL);

#endif
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("80211> BUG CFG80211_LostGoInform, BUT NetDevice not exist.\n"));
		
	Set_ApCli_Enable_Proc(pAd, "0");	
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	pAd->Mlme.bStartScc = FALSE;
#endif /*RT_CFG80211_P2P_CONCURRENT_DEVICE */

}
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */
#endif /* RT_CFG80211_SUPPORT */
