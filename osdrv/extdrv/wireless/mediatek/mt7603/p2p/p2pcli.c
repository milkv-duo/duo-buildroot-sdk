/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    ap_apcli.c

    Abstract:
    Support AP-Client function.

    Note:
    1. Call RT28xx_ApCli_Init() in init function and
       call RT28xx_ApCli_Remove() in close function

    2. MAC of ApCli-interface is initialized in RT28xx_ApCli_Init()

    3. ApCli index (0) of different rx packet is got in

    4. ApCli index (0) of different tx packet is assigned in

    5. ApCli index (0) of different interface is got in APHardTransmit() by using

    6. ApCli index (0) of IOCTL command is put in pAd->OS_Cookie->ioctl_if

    8. The number of ApCli only can be 1

	9. apcli convert engine subroutines, we should just take care data packet.
    Revision History:
    Who             When            What
    --------------  ----------      ----------------------------------------------
    Shiang, Fonchi  02-13-2007      created
*/

#ifdef P2P_SUPPORT

#include "rt_config.h"

#if 0
VOID ApCli_Remove(
	IN PRTMP_ADAPTER ad_p)
{
	UINT index;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif // RELEASE_EXCLUDE //

	for(index = 0; index < MAX_APCLI_NUM; index++)
	{
		if (ad_p->ApCfg.ApCliTab[index].dev)
		{
			RtmpOSNetDevDetach(ad_p->ApCfg.ApCliTab[index].dev);
			rtmp_wdev_idx_unreg(pAd, wdev);
			RtmpOSNetDevFree(ad_p->ApCfg.ApCliTab[index].dev);

			// Clear it as NULL to prevent latter access error.
			ad_p->flg_apcli_init = FALSE;
			ad_p->ApCfg.ApCliTab[index].dev = NULL;
		}
	}

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s <---\n", __FUNCTION__));
#endif // RELEASE_EXCLUDE //
}
#endif /*0*/

/* --------------------------------- Private -------------------------------- */
INT ApCliIfLookUp(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pAddr)
{
	SHORT i;
	SHORT ifIndex = -1;
	APCLI_STRUCT *pApCliEntry;
	struct wifi_dev *wdev;


	do
	{
		for(i = 0; i < MAX_APCLI_NUM; i++)
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[i];
			wdev = &pApCliEntry->wdev;

//			if(	MAC_ADDR_EQUAL(pAd->ApCfg.ApCliTab[i].CurrentAddress, pAddr))
			if(	MAC_ADDR_EQUAL(wdev->if_addr, pAddr))
			{
				ifIndex = i;
				DBGPRINT(RT_DEBUG_TRACE, ("(%s) ApCliIfIndex = %d\n", __FUNCTION__, ifIndex));
				break;
			}
		}
	} while (FALSE);

	return ifIndex;
}

BOOLEAN isValidApCliIf(
	SHORT ifIndex)
{
	if((ifIndex >= 0) && (ifIndex < MAX_APCLI_NUM))
		return TRUE;
	else
		return FALSE;
}

/*! \brief init the management mac frame header
 *  \param p_hdr mac header
 *  \param subtype subtype of the frame
 *  \param p_ds destination address, don't care if it is a broadcast address
 *  \return none
 *  \pre the station has the following information in the pAd->UserCfg
 *   - bssid
 *   - station address
 *  \post
 *  \note this function initializes the following field
 */
VOID ApCliMgtMacHeaderInit(
    IN	PRTMP_ADAPTER	pAd, 
    IN OUT PHEADER_802_11 pHdr80211, 
    IN UCHAR SubType, 
    IN UCHAR ToDs, 
    IN PUCHAR pDA, 
    IN PUCHAR pBssid,
    IN USHORT ifIndex)
{
    APCLI_STRUCT *pApCliEntry;
    struct wifi_dev *wdev;
    pApCliEntry = &pAd->ApCfg.ApCliTab[ifIndex];
    wdev = &pApCliEntry->wdev;

    NdisZeroMemory(pHdr80211, sizeof(HEADER_802_11));
    pHdr80211->FC.Type = FC_TYPE_MGMT;
    pHdr80211->FC.SubType = SubType;
    pHdr80211->FC.ToDs = ToDs;
    COPY_MAC_ADDR(pHdr80211->Addr1, pDA);
    COPY_MAC_ADDR(pHdr80211->Addr2, wdev->if_addr);
    COPY_MAC_ADDR(pHdr80211->Addr3, pBssid);
}


#ifdef DOT11_N_SUPPORT
/*
	========================================================================

	Routine Description:
		Verify the support rate for HT phy type

	Arguments:
		pAd 				Pointer to our adapter

	Return Value:
		FALSE if pAd->CommonCfg.SupportedHtPhy doesn't accept the pHtCapability.  (AP Mode)

	IRQL = PASSIVE_LEVEL

	========================================================================
*/
BOOLEAN ApCliCheckHt(
	IN PRTMP_ADAPTER 		pAd,
	IN USHORT 				IfIndex,
	IN OUT	HT_CAPABILITY_IE 	*pHtCapability,
	IN OUT	ADD_HT_INFO_IE 		*pAddHtInfo)
{
	PAPCLI_STRUCT pApCliEntry = NULL;
	
	if (IfIndex >= MAX_APCLI_NUM)
		return FALSE;

	pApCliEntry = &pAd->ApCfg.ApCliTab[IfIndex];

#if 0
	/* If use AMSDU, set flag. */
	if (pAd->CommonCfg.DesiredHtPhy.AmsduEnable && (pAd->CommonCfg.REGBACapability.field.AutoBA == FALSE))
		CLIENT_STATUS_SET_FLAG(pApCliEntry, fCLIENT_STATUS_AMSDU_INUSED);
	/* Save Peer Capability */
	if (pHtCapability->HtCapInfo.ShortGIfor20)
		CLIENT_STATUS_SET_FLAG(pApCliEntry, fCLIENT_STATUS_SGI20_CAPABLE);
	if (pHtCapability->HtCapInfo.ShortGIfor40)
		CLIENT_STATUS_SET_FLAG(pApCliEntry, fCLIENT_STATUS_SGI40_CAPABLE);
	if (pHtCapability->HtCapInfo.TxSTBC)
		CLIENT_STATUS_SET_FLAG(pApCliEntry, fCLIENT_STATUS_TxSTBC_CAPABLE);
	if (pHtCapability->HtCapInfo.RxSTBC)
		CLIENT_STATUS_SET_FLAG(pApCliEntry, fCLIENT_STATUS_RxSTBC_CAPABLE);
	if (pAd->CommonCfg.bRdg && pHtCapability->ExtHtCapInfo.RDGSupport)
		CLIENT_STATUS_SET_FLAG(pApCliEntry, fCLIENT_STATUS_RDG_CAPABLE);
	pApCliEntry->MpduDensity = pHtCapability->HtCapParm.MpduDensity;
#endif

	if ((pAd->OpMode == OPMODE_STA))
	{
#if 0 //Dennis TODO		
		if (BaSizeArray[pHtCapability->HtCapParm.MaxRAmpduFactor] > pAd->CommonCfg.BACapability.field.TxBAWinLimit)
			pAd->MacTab.Content[Wcid].BaSizeInUse = (UCHAR)pAd->CommonCfg.BACapability.field.TxBAWinLimit;
		else
			pAd->MacTab.Content[Wcid].BaSizeInUse = BaSizeArray[pHtCapability->HtCapParm.MaxRAmpduFactor];
#endif		
	}
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[0] = 0xff;
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[4] = 0x1;
	//2008/12/17:KH modified to fix the low throughput of AP-Client on Big-Endian Platform<--
	 switch (pAd->CommonCfg.RxStream)
	{
		case 1:			
			pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[0] = 0xff;
			pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[1] = 0x00;
            pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[2] = 0x00;
            pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[3] = 0x00;
			break;
		case 2:
			pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[0] = 0xff;
			pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[1] = 0xff;
            pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[2] = 0x00;
            pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[3] = 0x00;
			break;
		case 3:				
			pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[0] = 0xff;
			pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[1] = 0xff;
            pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[2] = 0xff;
            pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.MCSSet[3] = 0x00;
			break;
	}

	/* Record the RxMcs of AP */
	NdisMoveMemory(pApCliEntry->RxMcsSet, pHtCapability->MCSSet, 16);

	/* choose smaller setting */
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.ChannelWidth = pAddHtInfo->AddHtInfo.RecomWidth & pAd->CommonCfg.DesiredHtPhy.ChannelWidth;
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.GF =  pHtCapability->HtCapInfo.GF &pAd->CommonCfg.DesiredHtPhy.GF;

	/* Send Assoc Req with my HT capability. */
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.AMsduSize =  pAd->CommonCfg.DesiredHtPhy.AmsduSize;
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.MimoPs = pHtCapability->HtCapInfo.MimoPs;
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.ShortGIfor20 =  (pAd->CommonCfg.DesiredHtPhy.ShortGIfor20) & (pHtCapability->HtCapInfo.ShortGIfor20);
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.ShortGIfor40 =  (pAd->CommonCfg.DesiredHtPhy.ShortGIfor40) & (pHtCapability->HtCapInfo.ShortGIfor40);
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.TxSTBC =  (pAd->CommonCfg.DesiredHtPhy.TxSTBC)&(pHtCapability->HtCapInfo.RxSTBC);
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.RxSTBC =  (pAd->CommonCfg.DesiredHtPhy.RxSTBC)&(pHtCapability->HtCapInfo.TxSTBC);
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapParm.MaxRAmpduFactor = pAd->CommonCfg.DesiredHtPhy.MaxRAmpduFactor;
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapParm.MpduDensity = pHtCapability->HtCapParm.MpduDensity;
	pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.ExtHtCapInfo.PlusHTC = pHtCapability->ExtHtCapInfo.PlusHTC;

	if (pAd->CommonCfg.bRdg)
	{
		pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.ExtHtCapInfo.RDGSupport = pHtCapability->ExtHtCapInfo.RDGSupport;
	}
	
	/*COPY_AP_HTSETTINGS_FROM_BEACON(pAd, pHtCapability); */
	return TRUE;
}
#endif /* DOT11_N_SUPPORT */


VOID ComposeP2PPsPoll(
	IN PRTMP_ADAPTER pAd,
	IN PAPCLI_STRUCT pApCliEntry)
{
    	struct wifi_dev *wdev;
       wdev = &pApCliEntry->wdev;

	NdisZeroMemory(&pApCliEntry->PsPollFrame, sizeof (PSPOLL_FRAME));
	pApCliEntry->PsPollFrame.FC.Type = FC_TYPE_CNTL;
	pApCliEntry->PsPollFrame.FC.SubType = SUBTYPE_PS_POLL;
	pApCliEntry->PsPollFrame.Aid = pAd->ApCfg.ApCliTab[0].MlmeAux.Aid | 0xC000;

	COPY_MAC_ADDR(pApCliEntry->PsPollFrame.Bssid, pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid);
	COPY_MAC_ADDR(pApCliEntry->PsPollFrame.Ta, wdev->if_addr);
}

VOID ComposeP2PNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN PAPCLI_STRUCT pApCliEntry)
{
   	struct wifi_dev *wdev;
       wdev = &pApCliEntry->wdev;

	NdisZeroMemory(&pApCliEntry->NullFrame, sizeof (HEADER_802_11));
	pApCliEntry->NullFrame.FC.Type = FC_TYPE_DATA;
	pApCliEntry->NullFrame.FC.SubType = SUBTYPE_DATA_NULL;
	pApCliEntry->NullFrame.FC.ToDs = 1;

	COPY_MAC_ADDR(pApCliEntry->NullFrame.Addr1, pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid);
	COPY_MAC_ADDR(pApCliEntry->NullFrame.Addr2, wdev->if_addr);
	COPY_MAC_ADDR(pApCliEntry->NullFrame.Addr3, pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid);
}

/*
    ==========================================================================

	Routine	Description:
		Connected to the BSSID

	Arguments:
		pAd				- Pointer to our adapter
		ApCliIdx		- Which ApCli interface		
	Return Value:		
		FALSE: fail to alloc Mac entry.

	Note:

	==========================================================================
*/
BOOLEAN ApCliLinkUp(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR ifIndex)
{
	BOOLEAN result = FALSE;
	PAPCLI_STRUCT pApCliEntry = NULL;
	PMAC_TABLE_ENTRY pMacEntry = NULL;
	struct wifi_dev *wdev;

	do
	{
		pApCliEntry = &pAd->ApCfg.ApCliTab[ifIndex];
		wdev = &pApCliEntry->wdev;

		if (ifIndex < MAX_APCLI_NUM)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!! APCLI LINK UP - IF(apcli%d) AuthMode(%d)=%s, WepStatus(%d)=%s !!!\n", 
										ifIndex, 
										wdev->AuthMode, GetAuthMode(wdev->AuthMode),
										wdev->WepStatus, GetEncryptType(wdev->WepStatus)));			
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("!!! ERROR : APCLI LINK UP - IF(apcli%d)!!!\n", ifIndex));
			result = FALSE;
			break;
		}


		/* Sanity check: This link had existed.  */
		if (pApCliEntry->Valid)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("!!! ERROR : This link had existed - IF(apcli%d)!!!\n", ifIndex));
			result = FALSE;
			break;
		}
	
		/* Insert the Remote AP to our MacTable. */
		/*pMacEntry = MacTableInsertApCliEntry(pAd, (PUCHAR)(pAd->MlmeAux.Bssid)); */
		pMacEntry = MacTableInsertEntry(pAd, (PUCHAR)(pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid),
										wdev, ENTRY_GC, OPMODE_AP, TRUE);
		if (pMacEntry)
		{
			UCHAR Rates[MAX_LEN_OF_SUPPORTED_RATES];
			PUCHAR pRates = Rates;
			UCHAR RatesLen;
			UCHAR MaxSupportedRate = 0;
			UCHAR P2pIdx = P2P_NOT_FOUND;

			DBGPRINT(RT_DEBUG_INFO, ("%s:: Insert MAC Table success.    ifIndex = %d.\n", __FUNCTION__, ifIndex));
			pMacEntry->Sst = SST_ASSOC;
			DBGPRINT(RT_DEBUG_TRACE, ("\n%s::  Delete BssSearch Table on Channel = %d\n",
						__FUNCTION__, pAd->CommonCfg.Channel));
			DBGPRINT(RT_DEBUG_TRACE, ("\tBSSID = [%02x:%02x:%02x:%02x:%02x:%02x].  p2p_bssid = [%02x:%02x:%02x:%02x:%02x:%02x].\n",
								PRINT_MAC(pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid), PRINT_MAC(pAd->P2pCfg.Bssid)));
			BssTableDeleteEntry(&pAd->ScanTab,
					    pAd->P2pCfg.Bssid,
					    pAd->CommonCfg.Channel);
			if (pApCliEntry->bP2pClient)
			{
				pMacEntry->bP2pClient = TRUE;
				pMacEntry->P2pInfo.P2pClientState = P2PSTATE_CLIENT_ASSOC;
				P2pIdx = P2pGroupTabSearch(pAd, pMacEntry->Addr);
				if (P2pIdx != P2P_NOT_FOUND)
					pMacEntry->P2pInfo.p2pIndex = P2pIdx;
			}
			else
				pMacEntry->bP2pClient = FALSE;
			
			NdisGetSystemUpTime(&pApCliEntry->ApCliRcvBeaconTime);
			pApCliEntry->Valid = TRUE;
			pApCliEntry->MacTabWCID = pMacEntry->Aid;

			COPY_MAC_ADDR(APCLI_ROOT_BSSID_GET(pAd, pApCliEntry->MacTabWCID), pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid);
			pApCliEntry->SsidLen = pAd->ApCfg.ApCliTab[0].MlmeAux.SsidLen;
			NdisMoveMemory(pApCliEntry->Ssid, pAd->ApCfg.ApCliTab[0].MlmeAux.Ssid, pApCliEntry->SsidLen);

			ComposeP2PPsPoll(pAd, pApCliEntry);
			ComposeP2PNullFrame(pAd, pApCliEntry);

			if (pMacEntry->AuthMode >= Ndis802_11AuthModeWPA)
				pMacEntry->PortSecured = WPA_802_1X_PORT_NOT_SECURED;
			else
				pMacEntry->PortSecured = WPA_802_1X_PORT_SECURED;

			/* Store appropriate RSN_IE for WPA SM negotiation later  */
			/* If WPAPSK/WPA2SPK mix mode, driver just stores either WPAPSK or WPA2PSK */
			/* RSNIE. It depends on the AP-Client's authentication mode to store the corresponding RSNIE.    */
			if ((pMacEntry->AuthMode >= Ndis802_11AuthModeWPA) && (pAd->ApCfg.ApCliTab[0].MlmeAux.VarIELen != 0))
			{
				PUCHAR pVIE;
				UCHAR len;
				PEID_STRUCT pEid;

				pVIE = pAd->ApCfg.ApCliTab[0].MlmeAux.VarIEs;
				len	 = pAd->ApCfg.ApCliTab[0].MlmeAux.VarIELen;

				while (len > 0)
				{
					pEid = (PEID_STRUCT) pVIE;	
					/* For WPA/WPAPSK */
					if ((pEid->Eid == IE_WPA) && (NdisEqualMemory(pEid->Octet, WPA_OUI, 4)) 
						&& (pMacEntry->AuthMode == Ndis802_11AuthModeWPA || pMacEntry->AuthMode == Ndis802_11AuthModeWPAPSK))
					{
						NdisMoveMemory(pMacEntry->RSN_IE, pVIE, (pEid->Len + 2));
						pMacEntry->RSNIE_Len = (pEid->Len + 2);							
						DBGPRINT(RT_DEBUG_TRACE, ("ApCliLinkUp: Store RSN_IE for WPA SM negotiation \n"));
					}
					/* For WPA2/WPA2PSK */
					else if ((pEid->Eid == IE_RSN) && (NdisEqualMemory(pEid->Octet + 2, RSN_OUI, 3))
						&& (pMacEntry->AuthMode == Ndis802_11AuthModeWPA2 || pMacEntry->AuthMode == Ndis802_11AuthModeWPA2PSK
#ifdef WPA3_SUPPORT
						|| (pMacEntry->AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
						)) {
						NdisMoveMemory(pMacEntry->RSN_IE, pVIE, (pEid->Len + 2));
						pMacEntry->RSNIE_Len = (pEid->Len + 2);	
						DBGPRINT(RT_DEBUG_TRACE, ("ApCliLinkUp: Store RSN_IE for WPA2 SM negotiation \n"));
					}

					pVIE += (pEid->Len + 2);
					len  -= (pEid->Len + 2);
				}							
			}

			if (pMacEntry->RSNIE_Len == 0)
			{			
				DBGPRINT(RT_DEBUG_TRACE, ("ApCliLinkUp: root-AP has no RSN_IE \n"));			
			}
			else
			{
				hex_dump("The RSN_IE of root-AP", pMacEntry->RSN_IE, pMacEntry->RSNIE_Len);
			}		

			SupportRate(pAd->ApCfg.ApCliTab[0].MlmeAux.SupRate, pAd->ApCfg.ApCliTab[0].MlmeAux.SupRateLen, pAd->ApCfg.ApCliTab[0].MlmeAux.ExtRate,
				pAd->ApCfg.ApCliTab[0].MlmeAux.ExtRateLen, &pRates, &RatesLen, &MaxSupportedRate);

			pMacEntry->MaxSupportedRate = min(pAd->CommonCfg.MaxTxRate, MaxSupportedRate);
			pMacEntry->RateLen = RatesLen;
			set_entry_phy_cfg(pAd, pMacEntry);
			pMacEntry->CapabilityInfo = pAd->ApCfg.ApCliTab[0].MlmeAux.CapabilityInfo;

			/* If WEP is enabled, add paiewise and shared key */
			if (wdev->WepStatus == Ndis802_11WEPEnabled)
			{			
				PCIPHER_KEY pKey; 			
				INT idx, BssIdx;

				BssIdx = pAd->ApCfg.BssidNum + MAX_MESH_NUM + ifIndex;
			
				for (idx=0; idx < SHARE_KEY_NUM; idx++)
				{
					pKey = &pApCliEntry->SharedKey[idx];
										
					if (pKey->KeyLen > 0)
					{
#ifdef RELEASE_EXCLUDE
						/* carella modify: write WCID Attribute Table in Cmd Thread. 
						 * ApCli / WDS Set WCID Attribute Table as: 
						 * Reset WcidAttribute -> Set Encryption Key -> Set WcidAttribute
						 * in CPU slow platform(ex:AMAZON_SE), the Cmd Thread(Reset WcidAttribute)
						 * will be executed after set and overwrite the WcidAttribute.
						 */	
#endif /* RELEASE_EXCLUDE */
						/* Set key material and cipherAlg to Asic */
						RTMP_ASIC_SHARED_KEY_TABLE(pAd, 
	    									  		BssIdx, 
	    									  		idx, 
		    										pKey);	

						if (idx == wdev->DefaultKeyId)
						{						
							INT	cnt;
					
							/* Generate 3-bytes IV randomly for software encryption using */						
					    	for(cnt = 0; cnt < LEN_WEP_TSC; cnt++)
								pKey->TxTsc[cnt] = RandomByte(pAd); 
					
							RTMP_SET_WCID_SEC_INFO(pAd, 
												BssIdx, 
												idx, 
												pKey->CipherAlg, 
												pMacEntry->wcid, 
												SHAREDKEYTABLE);
						}
					}	
				}    		   		  		   					
			}

#ifdef DOT11_N_SUPPORT
			/* If this Entry supports 802.11n, upgrade to HT rate.  */
			if (pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapabilityLen != 0)
			{
				PHT_CAPABILITY_IE pHtCapability = (PHT_CAPABILITY_IE)&pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability;

				if ((pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability.HtCapInfo.GF) && (pAd->CommonCfg.DesiredHtPhy.GF))
				{
					pMacEntry->MaxHTPhyMode.field.MODE = MODE_HTGREENFIELD;
				}
				else
				{	
					pMacEntry->MaxHTPhyMode.field.MODE = MODE_HTMIX;
					pAd->MacTab.fAnyStationNonGF = TRUE;
					pAd->CommonCfg.AddHTInfo.AddHtInfo2.NonGfPresent = 1;
				}

				if ((pHtCapability->HtCapInfo.ChannelWidth) && (pAd->CommonCfg.DesiredHtPhy.ChannelWidth))
				{
					pMacEntry->MaxHTPhyMode.field.BW= BW_40;
					pMacEntry->MaxHTPhyMode.field.ShortGI = ((pAd->CommonCfg.DesiredHtPhy.ShortGIfor40)&(pHtCapability->HtCapInfo.ShortGIfor40));
				}
				else
				{	
					pMacEntry->MaxHTPhyMode.field.BW = BW_20;
					pMacEntry->MaxHTPhyMode.field.ShortGI = ((pAd->CommonCfg.DesiredHtPhy.ShortGIfor20)&(pHtCapability->HtCapInfo.ShortGIfor20));
					pAd->MacTab.fAnyStation20Only = TRUE;
				}

				/* find max fixed rate */
				pMacEntry->MaxHTPhyMode.field.MCS = get_ht_max_mcs(pAd,&wdev->DesiredHtPhyInfo.MCSSet[0],
																	&pHtCapability->MCSSet[0]);

				if (wdev->DesiredTransmitSetting.field.MCS != MCS_AUTO)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("IF-apcli%d : Desired MCS = %d\n", ifIndex,
						wdev->DesiredTransmitSetting.field.MCS));

					set_ht_fixed_mcs(pAd, pMacEntry, wdev->DesiredTransmitSetting.field.MCS, wdev->HTPhyMode.field.MCS);
				}

				pMacEntry->MaxHTPhyMode.field.STBC = (pHtCapability->HtCapInfo.RxSTBC & (pAd->CommonCfg.DesiredHtPhy.TxSTBC));
				pMacEntry->MpduDensity = pHtCapability->HtCapParm.MpduDensity;
				pMacEntry->MaxRAmpduFactor = pHtCapability->HtCapParm.MaxRAmpduFactor;
				pMacEntry->MmpsMode = (UCHAR)pHtCapability->HtCapInfo.MimoPs;
				pMacEntry->AMsduSize = (UCHAR)pHtCapability->HtCapInfo.AMsduSize;				
				pMacEntry->HTPhyMode.word = pMacEntry->MaxHTPhyMode.word;
				if (pAd->CommonCfg.DesiredHtPhy.AmsduEnable && (pAd->CommonCfg.REGBACapability.field.AutoBA == FALSE))
					CLIENT_STATUS_SET_FLAG(pMacEntry, fCLIENT_STATUS_AMSDU_INUSED);

				set_sta_ht_cap(pAd, pMacEntry, pHtCapability);

				NdisMoveMemory(&pMacEntry->HTCapability, &pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability, sizeof(HT_CAPABILITY_IE));
				NdisMoveMemory(pMacEntry->HTCapability.MCSSet, pApCliEntry->RxMcsSet, 16);

#if 0
#ifdef P2P_SUPPORT
				if(!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
				{
					pAd->ApCfg.ApCliTab[0].MlmeAux.CentralChannel = get_cent_ch_by_htinfo(pAd,
															&pAd->ApCfg.ApCliTab[0].MlmeAux.AddHtInfo,
															&pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability);
					pAd->CommonCfg.CentralChannel = pAd->ApCfg.ApCliTab[0].MlmeAux.CentralChannel;
					//printk("1. pAd->CommonCfg.CentralChannel = %d @@@@@@\n", pAd->CommonCfg.CentralChannel);
				}
#endif /* P2P_SUPPORT */
#endif
			}
			else
			{
				pAd->MacTab.fAnyStationIsLegacy = TRUE;
				DBGPRINT(RT_DEBUG_TRACE, ("ApCliLinkUp - MaxSupRate=%d Mbps\n",
								  RateIdToMbps[pMacEntry->MaxSupportedRate]));
			}				

#endif /* DOT11_N_SUPPORT */

			pMacEntry->HTPhyMode.word = pMacEntry->MaxHTPhyMode.word;
			pMacEntry->CurrTxRate = pMacEntry->MaxSupportedRate;
			
			if (wdev->bAutoTxRateSwitch == FALSE)
			{
				pMacEntry->bAutoTxRateSwitch = FALSE;
				/* If the legacy mode is set, overwrite the transmit setting of this entry.  	 */		
				RTMPUpdateLegacyTxSetting((UCHAR)wdev->DesiredTransmitSetting.field.FixedTxMode, pMacEntry);	
			}
			else
			{
				UCHAR TableSize = 0;

				pMacEntry->bAutoTxRateSwitch = TRUE;

				MlmeSelectTxRateTable(pAd, pMacEntry, &pMacEntry->pTable, &TableSize, &pMacEntry->CurrTxRateIndex);
#if defined(MT7603) || defined(MT7628)
				// TODO: shiang-MT7603, I add this here because now we relay on "pEntry->bAutoTxRateSwitch" to decide TxD format!
				MlmeNewTxRate(pAd, pMacEntry);
#endif /* MT7603 */
			}

			/* It had been set in APStartUp. Don't set again. */
			if (!INFRA_ON(pAd))
			{
				NdisMoveMemory(&(pAd->CommonCfg.APEdcaParm), &(pAd->ApCfg.ApCliTab[0].MlmeAux.APEdcaParm), sizeof(EDCA_PARM));

				AsicSetEdcaParm(pAd, &pAd->CommonCfg.APEdcaParm);
			}
			
			/* set this entry WMM capable or not */
			if ((pAd->ApCfg.ApCliTab[0].MlmeAux.APEdcaParm.bValid)
#ifdef DOT11_N_SUPPORT
				|| IS_HT_STA(pMacEntry)
#endif /* DOT11_N_SUPPORT */
				)
			{
				CLIENT_STATUS_SET_FLAG(pMacEntry, fCLIENT_STATUS_WMM_CAPABLE);
			}
			else
			{
				CLIENT_STATUS_CLEAR_FLAG(pMacEntry, fCLIENT_STATUS_WMM_CAPABLE);
			}

			set_sta_ra_cap(pAd, pMacEntry, pAd->ApCfg.ApCliTab[0].MlmeAux.APRalinkIe);
			// TODO: shiang-usw, revise this!
			if (CLIENT_STATUS_TEST_FLAG(pMacEntry, fCLIENT_STATUS_PIGGYBACK_CAPABLE))
			{
				RTMPSetPiggyBack(pAd, TRUE);
				DBGPRINT(RT_DEBUG_TRACE, ("Turn on Piggy-Back\n"));
			}

			pApCliEntry->ApCliBeaconPeriod = pAd->ApCfg.ApCliTab[0].MlmeAux.BeaconPeriod;

				{

					UCHAR	P2pIdx = P2P_NOT_FOUND;
					UCHAR	DevAddr[6] = {0};
			
					pMacEntry->bP2pClient = TRUE;
					pMacEntry->P2pInfo.P2pClientState = P2PSTATE_GO_OPERATING;

					P2pIdx = P2pGroupTabSearch(pAd, pMacEntry->Addr);
					if (P2pIdx == P2P_NOT_FOUND)
						P2pIdx = P2pGroupTabInsert(pAd, DevAddr, P2PSTATE_DISCOVERY_GO, NULL, 0, 0, 0);
			
					if (P2pIdx != P2P_NOT_FOUND)
						pMacEntry->P2pInfo.p2pIndex = P2pIdx;
				}

#if 0
			/* pApCliEntry->Valid is alredy set to be TRUE above */
			/* set the apcli interface be valid. */
			pApCliEntry->Valid = TRUE;
#endif
			result = TRUE;

			pAd->ApCfg.ApCliInfRunned++;
			break;
		}
		result = FALSE;

	} while(FALSE);

	RTMPSetSupportMCS(pAd,
					OPMODE_AP,
					pMacEntry,
					pAd->ApCfg.ApCliTab[0].MlmeAux.SupRate,
					pAd->ApCfg.ApCliTab[0].MlmeAux.SupRateLen,
					pAd->ApCfg.ApCliTab[0].MlmeAux.ExtRate,
					pAd->ApCfg.ApCliTab[0].MlmeAux.ExtRateLen,
#ifdef DOT11_VHT_AC
					pAd->ApCfg.ApCliTab[0].MlmeAux.vht_cap_len,
					&pAd->ApCfg.ApCliTab[0].MlmeAux.vht_cap,
#endif /* DOT11_VHT_AC */
					&pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapability,
					pAd->ApCfg.ApCliTab[0].MlmeAux.HtCapabilityLen);

#ifdef WSC_AP_SUPPORT

	/* WSC initial connect to AP, jump to Wsc start action and set the correct parameters     */
	if ((result == TRUE) && 
		(pAd->ApCfg.ApCliTab[ifIndex].WscControl.WscConfMode == WSC_ENROLLEE) &&
		(pAd->ApCfg.ApCliTab[ifIndex].WscControl.bWscTrigger == TRUE))
	{
		pAd->ApCfg.ApCliTab[ifIndex].WscControl.WscState = WSC_STATE_LINK_UP;
		pAd->ApCfg.ApCliTab[ifIndex].WscControl.WscStatus = WSC_STATE_LINK_UP;
		pAd->ApCfg.ApCliTab[ifIndex].WscControl.WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
		NdisZeroMemory(pAd->ApCfg.ApCliTab[ifIndex].WscControl.EntryAddr, MAC_ADDR_LEN);        
		NdisMoveMemory(pAd->ApCfg.ApCliTab[ifIndex].WscControl.EntryAddr, pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid, MAC_ADDR_LEN);
		WscSendEapolStart(pAd, pMacEntry->Addr, AP_MODE);
	}
	else
	{
		WscStop(pAd, TRUE, &pAd->ApCfg.ApCliTab[ifIndex].WscControl);
	}
#endif /* WSC_AP_SUPPORT */


#ifdef CONFIG_MULTI_CHANNEL	
	if (pAd->Multi_Channel_Enable == TRUE)
	{
		UINT32 Data;
		RTMP_IO_READ32(pAd, WMM_CTRL, &Data);
		Data |= 0x80000000;/* bit 31 set to 1 */   /*  WMM Channel switch to EDCA2 */
		RTMP_IO_WRITE32(pAd, WMM_CTRL, Data);


		RtmpPrepareHwNullFrame(pAd,
							pMacEntry,
							FALSE,
							FALSE,
							0,
							OPMODE_AP,
							PWR_SAVE,
							TRUE,
							1);

		if ((pAd->ApCfg.ApCliTab[ifIndex].WscControl.WscConfMode != WSC_DISABLE) &&
		    (pAd->ApCfg.ApCliTab[ifIndex].WscControl.bWscTrigger
		    )) 
		{
			printk ("p2p WSC trigger not set Multi-channel!!\n");
		}
		else if (INFRA_ON(pAd) 
			&& pMacEntry->PortSecured == WPA_802_1X_PORT_SECURED 
			&& pMacEntry->WepStatus == Ndis802_11WEPDisabled)
		{
			printk("INFRA_ON(pAd) set HCCAToEDCATimer\n");

			if (pAd->P2pCfg.bStartP2pConnect)
			{
				pAd->P2pCfg.bStartP2pConnect = FALSE;
			}

			RTMPSetTimer(&pAd->Mlme.MCCTimer, pAd->Mlme.HCCAToEDCATimerValue);
		}
		pAd->Mlme.P2pStayTick = 0;

		pAd->StaCfg.bImprovedScan = FALSE;
	}
#endif /* CONFIG_MULTI_CHANNEL */
#ifdef WPA3_SUPPORT
	pApCliEntry->fgIsConnInitialized = FALSE;
#endif

	return result;
}

/*
    ==========================================================================

	Routine	Description:
		Disconnect current BSSID

	Arguments:
		pAd				- Pointer to our adapter
		ApCliIdx		- Which ApCli interface		
	Return Value:		
		None

	Note:

	==========================================================================
*/
VOID ApCliLinkDown(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR ifIndex)
{
	PAPCLI_STRUCT pApCliEntry = NULL;
	MAC_TABLE_ENTRY *pEntry;
	UCHAR P2pIdx = P2P_NOT_FOUND;

	if (ifIndex < MAX_APCLI_NUM)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!! APCLI LINK DOWN - IF(apcli%d)!!!\n", ifIndex));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("!!! ERROR : APCLI LINK DOWN - IF(apcli%d)!!!\n", ifIndex));
		return;
	}
    	
#ifdef CONFIG_MULTI_CHANNEL	
	if (pAd->Multi_Channel_Enable == TRUE)
		MultiChannelTimerStop(pAd);
#endif /* CONFIG_MULTI_CHANNEL */
    	
	pApCliEntry = &pAd->ApCfg.ApCliTab[ifIndex];
	if (pApCliEntry->Valid == FALSE)	
		return;

	/* Find the p2p Entry and change the p2p State. */
	if (pApCliEntry->bP2pClient)
	{
		NdisZeroMemory(pAd->ApCfg.ApCliTab[0].CfgApCliBssid, MAC_ADDR_LEN);
		pEntry = &pAd->MacTab.Content[pApCliEntry->MacTabWCID];
		P2pIdx = P2pGroupTabSearch(pAd, pEntry->Addr);

		if ((P2P_CLI_ON(pAd)) && (IS_ENTRY_APCLI(pEntry)) && 
			(pEntry->WpaState == AS_PTKINITDONE))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s::  Secured = %d.  Addr = %02x:%02x:%02x:%02x:%02x:%02x.\n", 
						__FUNCTION__, pEntry->PortSecured, PRINT_MAC(pEntry->Addr)));
			if (P2pIdx != P2P_NOT_FOUND) 
			pAd->P2pTable.Client[P2pIdx].P2pClientState = P2PSTATE_DISCOVERY;
			if (pAd->P2pCfg.bP2pCliReConnect == FALSE)
				P2pLinkDown(pAd, P2P_DISCONNECTED);
			else
			{
				pAd->P2pCfg.bP2pCliReConnectTimerRunning = TRUE;
				RTMPSetTimer(&pAd->P2pCfg.P2pCliReConnectTimer, P2P_CHECK_CLIENT_TIMER);
			}
#ifdef DPA_S
			RtmpOSWrielessEventSend(pAd->p2p_dev, RT_WLAN_EVENT_CUSTOM, P2P_NOTIF_LINK_LOSS, NULL, NULL, 0);
#endif /* DPA_S */
		}
	}

	if ((INFRA_ON(pAd)) && (pAd->StaActive.SupportedHtPhy.ChannelWidth == BW_40))
	{

	}
	else
	{
		bbp_set_bw(pAd, BW_20);
	}

	pAd->ApCfg.ApCliInfRunned--;
	MacTableDeleteEntry(pAd, pApCliEntry->MacTabWCID, APCLI_ROOT_BSSID_GET(pAd, pApCliEntry->MacTabWCID));

	/* Clean Bss Search Table. */
#ifdef CONFIG_MULTI_CHANNEL
	DBGPRINT(RT_DEBUG_TRACE, ("\n++++++++ %s::  Delete BssSearch Table on Channel = %d. ++++++++\n", __FUNCTION__, pAd->ApCfg.ApCliTab[0].Channel));
	DBGPRINT(RT_DEBUG_TRACE, ("                   BSSID = [%02x:%02x:%02x:%02x:%02x:%02x].  p2p_bssid = [%02x:%02x:%02x:%02x:%02x:%02x].\n", PRINT_MAC(pAd->ApCfg.ApCliTab[0].Bssid), PRINT_MAC(pAd->P2pCfg.Bssid)));
	BssTableDeleteEntry(&pAd->ScanTab,
			    /*pAd->ApCfg.ApCliTab[0].Bssid*/pAd->P2pCfg.Bssid,
			    pAd->ApCfg.ApCliTab[0].Channel);
#else

	DBGPRINT(RT_DEBUG_TRACE, ("\n++++++++ %s::  Delete BssSearch Table on Channel = %d. ++++++++\n", __FUNCTION__, pAd->CommonCfg.Channel));
	DBGPRINT(RT_DEBUG_TRACE, ("                   BSSID = [%02x:%02x:%02x:%02x:%02x:%02x].  p2p_bssid = [%02x:%02x:%02x:%02x:%02x:%02x].\n", PRINT_MAC(pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid), PRINT_MAC(pAd->P2pCfg.Bssid)));
	BssTableDeleteEntry(&pAd->ScanTab,
			    /*pAd->ApCfg.ApCliTab[0].MlmeAux.Bssid*/pAd->P2pCfg.Bssid,
			    pAd->CommonCfg.Channel);
#endif /*CONFIG_MULTI_CHANNEL*/
	pApCliEntry->Valid = FALSE;	/* This link doesn't associated with any remote-AP  */

#ifdef CONFIG_MULTI_CHANNEL
	if (pAd->Multi_Channel_Enable == TRUE)
	{
		UINT32 Data;
		RTMP_IO_READ32(pAd, WMM_CTRL, &Data);
		Data &= 0x7fffffff;/* bit 31 set to 0 */	/*  WMM Channel switch to EDCA1 */
		RTMP_IO_WRITE32(pAd, WMM_CTRL, Data);

		if(!P2P_CLI_ON(pAd))
		{
			UINT32 Value=0;
			MultiChannelSwitchToRa(pAd);
			RTMP_IO_READ32(pAd, PBF_CFG, &Value);
			Value |= ((1 << 3) | (1 << 13));/* bit 3 and bit 13 set to 1 */
			Value |= ((1 << 2) | (1 << 12));/* bit 2  and bit 12 set to 1 */
			RTMP_IO_WRITE32(pAd, PBF_CFG, Value);
			pAd->MultiChannelFlowCtl=0;
			RTMP_OS_NETDEV_WAKE_QUEUE(pAd->net_dev);
		}	
	}
#endif /*CONFIG_MULTI_CHANNEL*/

}

/* 
    ==========================================================================
    Description:
        APCLI Interface Up.
    ==========================================================================
 */
VOID ApCliIfUp(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR ifIndex;
	PAPCLI_STRUCT pApCliEntry;

	/* Reset is in progress, stop immediately */
	if ( RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS) ||
		 RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS) ||
		 !RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
		return;

	/* sanity check whether the interface is initialized. */
	if (pAd->flg_apcli_init != TRUE)
		return;

	for(ifIndex = 0; ifIndex < MAX_APCLI_NUM; ifIndex++)
	{
		pApCliEntry = &pAd->ApCfg.ApCliTab[ifIndex];
		if (APCLI_IF_UP_CHECK(pAd, ifIndex) 
			&& (pApCliEntry->Enable == TRUE)
			&& (pApCliEntry->Valid == FALSE))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("(%s) ApCli interface[%d] startup.\n", __FUNCTION__, ifIndex));
			COPY_MAC_ADDR(pApCliEntry->CfgApCliBssid, pAd->P2pCfg.Bssid);
			DBGPRINT(RT_DEBUG_TRACE, ("SSID = [%s].    BSSID = [%02x:%02x:%02x:%02x:%02x:%02x].\n", pApCliEntry->CfgSsid, PRINT_MAC(pApCliEntry->CfgApCliBssid)));
			if (pAd->ApCfg.ApCliTab[0].CtrlCurrState == APCLI_CTRL_DISCONNECTED)
				MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_JOIN_REQ, 0, NULL, ifIndex);
		}
	}

	return;
}


/* 
    ==========================================================================
    Description:
        APCLI Interface Down.
    ==========================================================================
 */
VOID ApCliIfDown(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR ifIndex;
	PAPCLI_STRUCT pApCliEntry;

	for(ifIndex = 0; ifIndex < MAX_APCLI_NUM; ifIndex++)
	{
		pApCliEntry = &pAd->ApCfg.ApCliTab[ifIndex];
		DBGPRINT(RT_DEBUG_TRACE, ("(%s) ApCli interface[%d] startdown.\n", __FUNCTION__, ifIndex));
		MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DISCONNECT_REQ, 0, NULL, ifIndex);
	}

	return;
}



/* 
    ==========================================================================
    Description:
        APCLI Interface Monitor.
    ==========================================================================
 */
VOID ApCliIfMonitor(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR index;
	PAPCLI_STRUCT pApCliEntry;	

	/* Reset is in progress, stop immediately */
	if ( RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS) ||
		 RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS) ||
		 !RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
		return;

	/* sanity check whether the interface is initialized. */
	if (pAd->flg_apcli_init != TRUE)
		return;
	
	for(index = 0; index < MAX_APCLI_NUM; index++)
	{
		pApCliEntry = &pAd->ApCfg.ApCliTab[index];
		if ((pApCliEntry->Valid == TRUE)
			&& (RTMP_TIME_AFTER(pAd->Mlme.Now32 , (pApCliEntry->ApCliRcvBeaconTime + (4 * OS_HZ)))))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("ApCliIfMonitor: IF(apcli%d) - no Beancon is received from root-AP.\n", index));
			DBGPRINT(RT_DEBUG_TRACE, ("ApCliIfMonitor: Reconnect the Root-Ap again.\n"));
			MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DISCONNECT_REQ, 0, NULL, index);
			RTMP_MLME_HANDLER(pAd);
		}
	}

	return;
}

/*! \brief   To substitute the message type if the message is coming from external
 *  \param  pFrame         The frame received
 *  \param  *Machine       The state machine
 *  \param  *MsgType       the message type for the state machine
 *  \return TRUE if the substitution is successful, FALSE otherwise
 *  \pre
 *  \post
 */
BOOLEAN ApCliMsgTypeSubst(
	IN PRTMP_ADAPTER pAd,
	IN PFRAME_802_11 pFrame, 
	OUT INT *Machine, 
	OUT INT *MsgType)
{
	USHORT Seq;
	UCHAR EAPType; 
	BOOLEAN Return = FALSE;
#ifdef WSC_AP_SUPPORT
	UCHAR EAPCode;
    PMAC_TABLE_ENTRY pEntry;
#endif /* WSC_AP_SUPPORT */


	/* only PROBE_REQ can be broadcast, all others must be unicast-to-me && is_mybssid; otherwise,  */
	/* ignore this frame */

	/* WPA EAPOL PACKET */
	if (pFrame->Hdr.FC.Type == FC_TYPE_DATA)
	{		
#ifdef WSC_AP_SUPPORT    
        /* WSC EAPOL PACKET         */
        pEntry = MacTableLookup(pAd, pFrame->Hdr.Addr2);
        if (pEntry && IS_ENTRY_APCLI(pEntry) && pAd->ApCfg.ApCliTab[pEntry->func_tb_idx].WscControl.WscConfMode == WSC_ENROLLEE)
        {
            *Machine = WSC_STATE_MACHINE;
            EAPType = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 1);
            EAPCode = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 4);
            Return = WscMsgTypeSubst(EAPType, EAPCode, MsgType);
        }
        if (!Return)
#endif /* WSC_AP_SUPPORT */
        {
    		*Machine = WPA_STATE_MACHINE;
    		EAPType = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 1);
    		Return = WpaMsgTypeSubst(EAPType, MsgType);
        }
		return Return;
	}
	else if (pFrame->Hdr.FC.Type == FC_TYPE_MGMT) 		
	{
		switch (pFrame->Hdr.FC.SubType) 
		{
			case SUBTYPE_ASSOC_RSP:
				*Machine = APCLI_ASSOC_STATE_MACHINE;
				*MsgType = APCLI_MT2_PEER_ASSOC_RSP;
				break;

			case SUBTYPE_DISASSOC:
				*Machine = APCLI_ASSOC_STATE_MACHINE;
				*MsgType = APCLI_MT2_PEER_DISASSOC_REQ;
				break;

			case SUBTYPE_DEAUTH:
				*Machine = APCLI_AUTH_STATE_MACHINE;
				*MsgType = APCLI_MT2_PEER_DEAUTH;
				break;

			case SUBTYPE_AUTH:
				/* get the sequence number from payload 24 Mac Header + 2 bytes algorithm */
				NdisMoveMemory(&Seq, &pFrame->Octet[2], sizeof(USHORT));
				if (Seq == 2 || Seq == 4)
				{
					*Machine = APCLI_AUTH_STATE_MACHINE;
					*MsgType = APCLI_MT2_PEER_AUTH_EVEN;
				}
				else 
				{
					return FALSE;
				}
				break;

			case SUBTYPE_ACTION:
				*Machine = ACTION_STATE_MACHINE;
				/*  Sometimes Sta will return with category bytes with MSB = 1, if they receive catogory out of their support */
				if ((pFrame->Octet[0]&0x7F) > MAX_PEER_CATE_MSG) 
				{
					*MsgType = MT2_ACT_INVALID;
				}
				else
				{
					*MsgType = (pFrame->Octet[0]&0x7F);
				}
				break;

			default:
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}


BOOLEAN preCheckMsgTypeSubset(
	IN PRTMP_ADAPTER  pAd,
	IN PFRAME_802_11 pFrame, 
	OUT INT *Machine, 
	OUT INT *MsgType)
{
	if (pFrame->Hdr.FC.Type == FC_TYPE_MGMT)
	{
		switch (pFrame->Hdr.FC.SubType) 
		{
			/* Beacon must be processed be AP Sync state machine. */
        		case SUBTYPE_BEACON:
				*Machine = AP_SYNC_STATE_MACHINE;
				*MsgType = APMT2_PEER_BEACON;
            			break;

			/* Only Sta have chance to receive Probe-Rsp. */
			case SUBTYPE_PROBE_RSP:
				*Machine = APCLI_SYNC_STATE_MACHINE;
				*MsgType = APCLI_MT2_PEER_PROBE_RSP;
				break;

			default:
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

/* 
    ==========================================================================
    Description:
        MLME message sanity check
    Return:
        TRUE if all parameters are OK, FALSE otherwise
        
    IRQL = DISPATCH_LEVEL

    ==========================================================================
 */
BOOLEAN ApCliPeerAssocRspSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *pCapabilityInfo, 
    OUT USHORT *pStatus, 
    OUT USHORT *pAid, 
	OUT ULONG *P2PSubelementLen, 
	OUT PUCHAR pP2pSubelement,
    OUT UCHAR SupRate[], 
    OUT UCHAR *pSupRateLen,
    OUT UCHAR ExtRate[], 
    OUT UCHAR *pExtRateLen,
    OUT HT_CAPABILITY_IE *pHtCapability,
    OUT ADD_HT_INFO_IE *pAddHtInfo,	/* AP might use this additional ht info IE  */
    OUT UCHAR *pHtCapabilityLen,
    OUT UCHAR *pAddHtInfoLen,
    OUT UCHAR *pNewExtChannelOffset,
    OUT PEDCA_PARM pEdcaParm,
    OUT UCHAR *pCkipFlag) 
{
	CHAR          IeType, *Ptr;
	PFRAME_802_11 pFrame = (PFRAME_802_11)pMsg;
	PEID_STRUCT   pEid;
	ULONG         Length = 0;
	UCHAR	P2POUIBYTE[4] = {0x50, 0x6f, 0x9a, 0x9};
    
	*pNewExtChannelOffset = 0xff;
	*pHtCapabilityLen = 0;
	*pAddHtInfoLen = 0;
	COPY_MAC_ADDR(pAddr2, pFrame->Hdr.Addr2);
	Ptr = (CHAR *) pFrame->Octet;
	Length += LENGTH_802_11;
        
	NdisMoveMemory(pCapabilityInfo, &pFrame->Octet[0], 2);
	Length += 2;
	NdisMoveMemory(pStatus,         &pFrame->Octet[2], 2);
	Length += 2;
	*pCkipFlag = 0;
	*pExtRateLen = 0;
	pEdcaParm->bValid = FALSE;
    
	if (*pStatus != MLME_SUCCESS) 
		return TRUE;
    
	NdisMoveMemory(pAid, &pFrame->Octet[4], 2);
	Length += 2;

	/* Aid already swaped byte order in RTMPFrameEndianChange() for big endian platform */
	*pAid = (*pAid) & 0x3fff; /* AID is low 14-bit */
        
	/* -- get supported rates from payload and advance the pointer */
	IeType = pFrame->Octet[6];
	*pSupRateLen = pFrame->Octet[7];
	if ((IeType != IE_SUPP_RATES) || (*pSupRateLen > MAX_LEN_OF_SUPPORTED_RATES))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): fail - wrong SupportedRates IE\n", __FUNCTION__));
		return FALSE;
	}
	else 
		NdisMoveMemory(SupRate, &pFrame->Octet[8], *pSupRateLen);

	Length = Length + 2 + *pSupRateLen;

	/* many AP implement proprietary IEs in non-standard order, we'd better */
	/* tolerate mis-ordered IEs to get best compatibility */
	pEid = (PEID_STRUCT) &pFrame->Octet[8 + (*pSupRateLen)];
            
	/* get variable fields from payload and advance the pointer */
	while ((Length + 2 + pEid->Len) <= MsgLen)
	{
		switch (pEid->Eid)
		{
			case IE_EXT_SUPP_RATES:
				if (pEid->Len <= MAX_LEN_OF_SUPPORTED_RATES)
				{
					NdisMoveMemory(ExtRate, pEid->Octet, pEid->Len);
					*pExtRateLen = pEid->Len;
				}
				break;
#ifdef DOT11_N_SUPPORT
			case IE_HT_CAP:
			case IE_HT_CAP2:
				if (pEid->Len >= SIZE_HT_CAP_IE)  /* Note: allow extension.!! */
				{
					NdisMoveMemory(pHtCapability, pEid->Octet, SIZE_HT_CAP_IE);
					*(USHORT *) (&pHtCapability->HtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->HtCapInfo));
					*(USHORT *) (&pHtCapability->ExtHtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->ExtHtCapInfo));
					*pHtCapabilityLen = SIZE_HT_CAP_IE;
				}
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("%s():wrong IE_HT_CAP. \n", __FUNCTION__));
				}
				
				break;
			case IE_ADD_HT:
			case IE_ADD_HT2:
				if (pEid->Len >= sizeof(ADD_HT_INFO_IE))				
				{
					/* This IE allows extension, but we can ignore extra bytes beyond our knowledge , so only */
					/* copy first sizeof(ADD_HT_INFO_IE) */
					NdisMoveMemory(pAddHtInfo, pEid->Octet, sizeof(ADD_HT_INFO_IE));
					*pAddHtInfoLen = SIZE_ADD_HT_INFO_IE;
				}
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("%s():wrong IE_ADD_HT. \n", __FUNCTION__));
				}
				break;
			case IE_SECONDARY_CH_OFFSET:
				if (pEid->Len == 1)
				{
					*pNewExtChannelOffset = pEid->Octet[0];
				}
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("%s():wrong IE_SECONDARY_CH_OFFSET. \n", __FUNCTION__));
				}
				break;
#endif /* DOT11_N_SUPPORT */
#if 0
			case IE_AIRONET_CKIP:
				// 0. Check Aironet IE length, it must be larger or equal to 28
				//    Cisco's AP VxWork version(will not be supported) used this IE length as 28
				//    Cisco's AP IOS version used this IE length as 30 
				if (pEid->Len < (CKIP_NEGOTIATION_LENGTH - 2))
					break;

				// 1. Copy CKIP flag byte to buffer for process
				*pCkipFlag = *(pEid->Octet + 8);				
				break;

			case IE_AIRONET_IPADDRESS:
				if (pEid->Len != 0x0A)
				break;

				// Get Cisco Aironet IP information
				if (NdisEqualMemory(pEid->Octet, CISCO_OUI, 3) == 1)
					NdisMoveMemory(pAd->StaCfg.AironetIPAddress, pEid->Octet + 4, 4);
				break;
#endif
			/* CCX2, WMM use the same IE value */
			/* case IE_CCX_V2: */
			case IE_VENDOR_SPECIFIC:
				/* handle WME PARAMTER ELEMENT */
				if (NdisEqualMemory(pEid->Octet, WME_PARM_ELEM, 6) && (pEid->Len == 24))
				{
					PUCHAR ptr;
					int i;
        
					/* parsing EDCA parameters */
					pEdcaParm->bValid          = TRUE;
					pEdcaParm->bQAck           = FALSE; /* pEid->Octet[0] & 0x10; */
					pEdcaParm->bQueueRequest   = FALSE; /* pEid->Octet[0] & 0x20; */
					pEdcaParm->bTxopRequest    = FALSE; /* pEid->Octet[0] & 0x40; */
					/*pEdcaParm->bMoreDataAck    = FALSE; *//* pEid->Octet[0] & 0x80; */
					pEdcaParm->EdcaUpdateCount = pEid->Octet[6] & 0x0f;
					pEdcaParm->bAPSDCapable    = (pEid->Octet[6] & 0x80) ? 1 : 0;
					ptr = (PUCHAR) &pEid->Octet[8];
					for (i=0; i<4; i++)
					{
						UCHAR aci = (*ptr & 0x60) >> 5; /* b5~6 is AC INDEX */
						pEdcaParm->bACM[aci]  = (((*ptr) & 0x10) == 0x10);   /* b5 is ACM */
						pEdcaParm->Aifsn[aci] = (*ptr) & 0x0f;               /* b0~3 is AIFSN */
						pEdcaParm->Cwmin[aci] = *(ptr+1) & 0x0f;             /* b0~4 is Cwmin */
						pEdcaParm->Cwmax[aci] = *(ptr+1) >> 4;               /* b5~8 is Cwmax */
						pEdcaParm->Txop[aci]  = *(ptr+2) + 256 * (*(ptr+3)); /* in unit of 32-us */
						ptr += 4; /* point to next AC */
					}
				}
				if (NdisEqualMemory(pEid->Octet, P2POUIBYTE, sizeof(P2POUIBYTE)) && (pEid->Len >= 4))
				{
					if (*P2PSubelementLen == 0)
					{
						RTMPMoveMemory(pP2pSubelement, &pEid->Eid, (pEid->Len+2));
						*P2PSubelementLen = (pEid->Len+2);
					}
					else if (*P2PSubelementLen > 0)
					{
						if (((*P2PSubelementLen) + (pEid->Len+2)) <= MAX_VIE_LEN)
						{
						RTMPMoveMemory(pP2pSubelement + *P2PSubelementLen, &pEid->Eid, (pEid->Len+2));
						*P2PSubelementLen += (pEid->Len+2);
					}
						else
						{
							DBGPRINT(RT_DEBUG_ERROR, ("%s: ERROR!! 111 Sum of P2PSubelementLen= %lu, > MAX_VIE_LEN !!\n", __FUNCTION__ ,((*P2PSubelementLen) + (pEid->Len+2))));
							return FALSE;
						}
					}
					DBGPRINT(RT_DEBUG_ERROR, (" ! ===>P2P - %s	P2P IE Len becomes = %ld.\n", __FUNCTION__, *P2PSubelementLen));
				}
				break;
				default:
					DBGPRINT(RT_DEBUG_TRACE, ("%s():ignore unrecognized EID = %d\n", __FUNCTION__, pEid->Eid));
					break;
		}

		Length = Length + 2 + pEid->Len; 
		pEid = (PEID_STRUCT)((UCHAR*)pEid + 2 + pEid->Len);        
	}

	return TRUE;
}


MAC_TABLE_ENTRY *ApCliTableLookUpByWcid(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR wcid,
	IN PUCHAR pAddrs)
{
	/*USHORT HashIdx; */
	ULONG ApCliIndex;
	PMAC_TABLE_ENTRY pCurEntry = NULL;
	PMAC_TABLE_ENTRY pEntry = NULL;

	if (wcid <=0 || wcid >= MAX_LEN_OF_MAC_TABLE )
		return NULL;

	NdisAcquireSpinLock(&pAd->MacTabLock);

	do
	{
		pCurEntry = &pAd->MacTab.Content[wcid];

		ApCliIndex = 0xff;
		if ((pCurEntry) && IS_ENTRY_APCLI(pCurEntry))
		{
			ApCliIndex = pCurEntry->wdev_idx;
		}

		if ((ApCliIndex == 0xff) || (ApCliIndex >= MAX_APCLI_NUM))
			break;

		if (pAd->ApCfg.ApCliTab[ApCliIndex].Valid != TRUE)
			break;

		if (MAC_ADDR_EQUAL(pCurEntry->Addr, pAddrs))
		{
			pEntry = pCurEntry;
			break;
		}
	} while(FALSE);

	NdisReleaseSpinLock(&pAd->MacTabLock);

	return pEntry;
}

/*
	==========================================================================
	Description:
		Check the WDS Entry is valid or not.
	==========================================================================
 */
static inline BOOLEAN ValidApCliEntry(
	IN PRTMP_ADAPTER pAd,
	IN INT apCliIdx)
{
	BOOLEAN result;
	PMAC_TABLE_ENTRY pMacEntry;
	APCLI_STRUCT *pApCliEntry;
	do
	{
		if ((apCliIdx < 0) || (apCliIdx >= MAX_APCLI_NUM))
		{
			result = FALSE;
			break;
		}

		pApCliEntry = (APCLI_STRUCT *)&pAd->ApCfg.ApCliTab[apCliIdx];
		if (pApCliEntry->Valid != TRUE)
		{
			result = FALSE;
			break;
		}

		if ((pApCliEntry->MacTabWCID <= 0) 
			|| (pApCliEntry->MacTabWCID >= MAX_LEN_OF_MAC_TABLE))
		{
			result = FALSE;
			break;
		}
	
		pMacEntry = &pAd->MacTab.Content[pApCliEntry->MacTabWCID];
		if (!IS_ENTRY_APCLI(pMacEntry))
		{
			result = FALSE;
			break;
		}
			
		result = TRUE;
	} while(FALSE);

	return result;
}


BOOLEAN ApCliAllowToSendPacket(
	IN RTMP_ADAPTER *pAd,
	IN struct wifi_dev *wdev,
	IN PNDIS_PACKET pPacket,
	OUT UCHAR		*pWcid)
{
	UCHAR apCliIdx;
	BOOLEAN	allowed = FALSE;

	/*DBGPRINT(RT_DEBUG_TRACE, ("ApCliAllowToSendPacket():Packet to ApCli interface!\n")); */
	apCliIdx = wdev->func_idx;
	if (ValidApCliEntry(pAd, apCliIdx))
	{
		/*DBGPRINT(RT_DEBUG_TRACE, ("ApCliAllowToSendPacket(): Set the WCID as %d!\n", pAd->ApCfg.ApCliTab[apCliIdx].MacTabWCID)); */
		*pWcid = pAd->ApCfg.ApCliTab[apCliIdx].MacTabWCID;
		allowed = TRUE;
	}

	return allowed;
	
}


/*
	========================================================================
	
	Routine Description:
		Validate the security configuration against the RSN information 
		element

	Arguments:
		pAdapter	Pointer	to our adapter
		eid_ptr 	Pointer to VIE
		
	Return Value:
		TRUE 	for configuration match 
		FALSE	for otherwise
		
	Note:
		
	========================================================================
*/
BOOLEAN 	ApCliValidateRSNIE(
	IN		PRTMP_ADAPTER	pAd, 
	IN 		PEID_STRUCT    	pEid_ptr,
	IN		USHORT			eid_len,
	IN		USHORT			idx)
{
	PUCHAR              pVIE;
	PUCHAR				pTmp;
	UCHAR         		len;
	PEID_STRUCT         pEid;			
	CIPHER_SUITE		WPA;			/* AP announced WPA cipher suite */
	CIPHER_SUITE		WPA2;			/* AP announced WPA2 cipher suite */
	USHORT				Count;
	UCHAR               Sanity;
#ifndef WPA3_SUPPORT
	PAPCLI_STRUCT   	pApCliEntry = NULL;
	struct wifi_dev *wdev;
#endif
	PRSN_IE_HEADER_STRUCT			pRsnHeader;
	NDIS_802_11_ENCRYPTION_STATUS	TmpCipher;
	NDIS_802_11_AUTHENTICATION_MODE TmpAuthMode;
	NDIS_802_11_AUTHENTICATION_MODE WPA_AuthMode;
	NDIS_802_11_AUTHENTICATION_MODE WPA_AuthModeAux;
	NDIS_802_11_AUTHENTICATION_MODE WPA2_AuthMode;
	NDIS_802_11_AUTHENTICATION_MODE WPA2_AuthModeAux;
#ifdef WPA3_SUPPORT
	INT i, j;
	UINT32 TmpAKMSuite;
#endif

	pVIE = (PUCHAR) pEid_ptr;
	len	 = eid_len;

	/* if (len >= MAX_LEN_OF_RSNIE || len <= MIN_LEN_OF_RSNIE) */
	/*	return FALSE; */

	/* Init WPA setting */
#ifdef WPA3_SUPPORT
	for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++)
		WPA.PairCipher[i] = Ndis802_11WEPDisabled;
#else
	WPA.PairCipher    	= Ndis802_11WEPDisabled;
	WPA.PairCipherAux 	= Ndis802_11WEPDisabled;
#endif
	WPA.GroupCipher   	= Ndis802_11WEPDisabled;
	WPA.RsnCapability 	= 0;
	WPA.bMixMode      	= FALSE;
	WPA_AuthMode	  	= Ndis802_11AuthModeOpen;
	WPA_AuthModeAux		= Ndis802_11AuthModeOpen;
#ifdef WPA3_SUPPORT
	for (i = 0; i < MAX_NUM_SUPPORTED_AKM_SUITES; i++)
		WPA.au4AuthKeyMgtSuite[i] = 0;
#endif

	/* Init WPA2 setting */
#ifdef WPA3_SUPPORT
	for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++)
		WPA2.PairCipher[i] = Ndis802_11WEPDisabled;
#else
	WPA2.PairCipher    	= Ndis802_11WEPDisabled;
	WPA2.PairCipherAux 	= Ndis802_11WEPDisabled;
#endif
	WPA2.GroupCipher   	= Ndis802_11WEPDisabled;
	WPA2.RsnCapability 	= 0;
	WPA2.bMixMode      	= FALSE;
	WPA2_AuthMode	  	= Ndis802_11AuthModeOpen;
	WPA2_AuthModeAux	= Ndis802_11AuthModeOpen;
#ifdef WPA3_SUPPORT
	for (i = 0; i < MAX_NUM_SUPPORTED_AKM_SUITES; i++)
		WPA2.au4AuthKeyMgtSuite[i] = 0;
#endif

	Sanity = 0;

	/* 1. Parse Cipher this received RSNIE */
	while (len > 0)
	{		
		pTmp = pVIE;
		pEid = (PEID_STRUCT) pTmp;	

		switch(pEid->Eid)
		{
			case IE_WPA:
				if (NdisEqualMemory(pEid->Octet, WPA_OUI, 4) != 1)
				{
					/* if unsupported vendor specific IE */
					break;
				}	
				/* Skip OUI ,version and multicast suite OUI */
				pTmp += 11;

				/* Cipher Suite Selectors from Spec P802.11i/D3.2 P26. */
	            /*  Value      Meaning */
	            /*  0           None  */
	            /*  1           WEP-40 */
	            /*  2           Tkip */
	            /*  3           WRAP */
	            /*  4           AES */
	            /*  5           WEP-104 */
				/* Parse group cipher */
				switch (*pTmp)
				{
					case 1:
					case 5:	/* Although WEP is not allowed in WPA related auth mode, we parse it anyway */
						WPA.GroupCipher = Ndis802_11WEPEnabled;
						break;
					case 2:
						WPA.GroupCipher = Ndis802_11TKIPEnable;
						break;
					case 4:
						WPA.GroupCipher = Ndis802_11AESEnable;
						break;
					default:
						break;
				}

				/* number of unicast suite */
				pTmp += 1;

				/* Store unicast cipher count */
			    NdisMoveMemory(&Count, pTmp, sizeof(USHORT));
    			Count = cpu2le16(Count);		

				/* pointer to unicast cipher */
			    pTmp += sizeof(USHORT);	

				/* Parsing all unicast cipher suite			 */	
				while (Count > 0)
				{
					/* Skip cipher suite OUI */
					pTmp += 3;
					TmpCipher = Ndis802_11WEPDisabled;
					switch (*pTmp)
					{
						case 1:
						case 5: /* Although WEP is not allowed in WPA related auth mode, we parse it anyway */
							TmpCipher = Ndis802_11WEPEnabled;
							break;
						case 2:
							TmpCipher = Ndis802_11TKIPEnable;							
							break;
						case 4:
							TmpCipher = Ndis802_11AESEnable;						
							break;
						default:
							break;
					}
#ifdef WPA3_SUPPORT
					for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++)
						if (TmpCipher > WPA.PairCipher[i]) {
							j = i;
							for (j = (MAX_NUM_SUPPORTED_CIPHER_SUITES - 2); j >= i; j--)
								WPA.PairCipher[j+1] = WPA.PairCipher[j];
							WPA.PairCipher[i] = TmpCipher;
							break;
						}
#else
					if (TmpCipher > WPA.PairCipher)
					{
						/* Move the lower cipher suite to PairCipherAux */
						WPA.PairCipherAux = WPA.PairCipher;
						WPA.PairCipher    = TmpCipher;
					}
					else
					{
						WPA.PairCipherAux = TmpCipher;
					}
#endif
					pTmp++;
					Count--;
				}
			
				/* Get AKM suite counts */
				NdisMoveMemory(&Count, pTmp, sizeof(USHORT));
				Count = cpu2le16(Count);		

				pTmp   += sizeof(USHORT);

				/* Parse AKM ciphers */
				/* Parsing all AKM cipher suite	 */			
				while (Count > 0)
				{
			    	/* Skip cipher suite OUI */
					pTmp   += 3;
					TmpAuthMode = Ndis802_11AuthModeOpen;
					switch (*pTmp)
					{	
						case 1:
							/* WPA-enterprise */
							TmpAuthMode = Ndis802_11AuthModeWPA;
							TmpAKMSuite = WPA_AKM_SUITE_802_1X;
							break;
						case 2:
							/* WPA-personal */
							TmpAuthMode = Ndis802_11AuthModeWPAPSK;
							TmpAKMSuite = WPA_AKM_SUITE_PSK;
							break;
						default:
							break;
					}
#ifdef WPA3_SUPPORT
					for (i = 0; i < MAX_NUM_SUPPORTED_AKM_SUITES; i++)
						if (TmpAKMSuite > WPA.au4AuthKeyMgtSuite[i]) {
							j = i;
							for (j = (MAX_NUM_SUPPORTED_AKM_SUITES - 2); j >= i; j--)
								WPA.au4AuthKeyMgtSuite[j+1] = WPA.au4AuthKeyMgtSuite[j];
							WPA.au4AuthKeyMgtSuite[i] = TmpAKMSuite;
							break;
						}
#endif
					if (TmpAuthMode > WPA_AuthMode)
					{
						/* Move the lower AKM suite to WPA_AuthModeAux */
						WPA_AuthModeAux = WPA_AuthMode;
						WPA_AuthMode    = TmpAuthMode;
					}
					else
					{
						WPA_AuthModeAux = TmpAuthMode;
					}
				    pTmp++;
					Count--;										
				}

				/* ToDo - Support WPA-None ? */

				/* Check the Pair & Group, if different, turn on mixed mode flag */
#ifdef WPA3_SUPPORT
				if (WPA.GroupCipher != WPA.PairCipher[0])
#else
				if (WPA.GroupCipher != WPA.PairCipher)
#endif
					WPA.bMixMode = TRUE;
#ifndef WPA3_SUPPORT
				DBGPRINT(RT_DEBUG_TRACE, ("ApCliValidateRSNIE - RSN-WPA1 PairWiseCipher(%s), GroupCipher(%s), AuthMode(%s)\n",
											((WPA.bMixMode) ? "Mix" : GetEncryptType(WPA.PairCipher)), 
											GetEncryptType(WPA.GroupCipher),
											GetAuthMode(WPA_AuthMode)));
#endif
				Sanity |= 0x1;
				break;
			case IE_RSN:
				pRsnHeader = (PRSN_IE_HEADER_STRUCT) pTmp;
				
				/* 0. Version must be 1 */
				/*  The pRsnHeader->Version exists in native little-endian order, so we may need swap it for RT_BIG_ENDIAN systems. */
				if (le2cpu16(pRsnHeader->Version) != 1)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - RSN Version isn't 1(%d) \n", pRsnHeader->Version));
					break;
				}	

				pTmp   += sizeof(RSN_IE_HEADER_STRUCT);

				/* 1. Check cipher OUI		*/		
				if (!RTMPEqualMemory(pTmp, RSN_OUI, 3))
				{
					/* if unsupported vendor specific IE */
					break;
				}

				/* Skip cipher suite OUI */
				pTmp += 3;

				/* Parse group cipher */
				switch (*pTmp)
				{
					case 1:
					case 5:	/* Although WEP is not allowed in WPA related auth mode, we parse it anyway */
						WPA2.GroupCipher = Ndis802_11WEPEnabled;
						break;
					case 2:
						WPA2.GroupCipher = Ndis802_11TKIPEnable;
						break;
					case 4:
						WPA2.GroupCipher = Ndis802_11AESEnable;
						break;
					default:
						break;
				}

				/* number of unicast suite */
				pTmp += 1;

				/* Get pairwise cipher counts	 */			
				NdisMoveMemory(&Count, pTmp, sizeof(USHORT));
				Count = cpu2le16(Count);
				
				pTmp   += sizeof(USHORT);

				/* 3. Get pairwise cipher */
				/* Parsing all unicast cipher suite */
				while (Count > 0)
				{
					/* Skip OUI*/
					pTmp += 3;
					TmpCipher = Ndis802_11WEPDisabled;
					switch (*pTmp)
					{
						case 1:
						case 5: /* Although WEP is not allowed in WPA related auth mode, we parse it anyway */
							TmpCipher = Ndis802_11WEPEnabled;
							break;
						case 2:
							TmpCipher = Ndis802_11TKIPEnable;							
							break;
						case 4:
							TmpCipher = Ndis802_11AESEnable;							
							break;
						default:
							break;
					}
#ifdef WPA3_SUPPORT
					for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++)
						if (TmpCipher > WPA2.PairCipher[i]) {
							j = i;
							for (j = (MAX_NUM_SUPPORTED_CIPHER_SUITES - 2); j >= i; j--)
								WPA2.PairCipher[j+1] = WPA2.PairCipher[j];
							WPA2.PairCipher[i] = TmpCipher;
							break;
						}
#else
					if (TmpCipher > WPA2.PairCipher)
					{
						/* Move the lower cipher suite to PairCipherAux */
						WPA2.PairCipherAux = WPA2.PairCipher;
						WPA2.PairCipher    = TmpCipher;
					}
					else
					{
						WPA2.PairCipherAux = TmpCipher;
					}
#endif
					pTmp ++;
					Count--;
				}

				/* Get AKM suite counts		 */		
				NdisMoveMemory(&Count, pTmp, sizeof(USHORT));
				Count = cpu2le16(Count);		

				pTmp   += sizeof(USHORT);

				/* Parse AKM ciphers */
				/* Parsing all AKM cipher suite	 */			
				while (Count > 0)
				{
			    	/* Skip cipher suite OUI */
					pTmp   += 3;
					TmpAuthMode = Ndis802_11AuthModeOpen;
					switch (*pTmp) {
					case 1:
						/* WPA2-enterprise */
						TmpAuthMode = Ndis802_11AuthModeWPA2;
						TmpAKMSuite = RSN_AKM_SUITE_802_1X;
						break;
					case 2:
						/* WPA2-personal */
						TmpAuthMode = Ndis802_11AuthModeWPA2PSK;
						TmpAKMSuite = RSN_AKM_SUITE_PSK;
						break;
#ifdef CFG_SUPPORT_802_11W
					case 5:
						/* WPA2-personal */
						TmpAuthMode = Ndis802_11AuthModeWPA2;
						TmpAKMSuite = RSN_AKM_SUITE_802_1X_SHA256;
						break;
					case 6:
						/* WPA2-personal */
						TmpAuthMode = Ndis802_11AuthModeWPA2PSK;
						TmpAKMSuite = RSN_AKM_SUITE_PSK_SHA256;
						break;
#endif
#ifdef WPA3_SUPPORT
					case 8:
						/* WPA3-SAEl */
						TmpAuthMode = Ndis802_11AuthModeWPA3SAE;
						TmpAKMSuite = RSN_AKM_SUITE_SAE;
						break;
					case 11:
						TmpAuthMode = Ndis802_11AuthModeWPA2PSK;
						TmpAKMSuite = RSN_AKM_SUITE_8021X_SUITE_B;
						break;
					case 12:
						TmpAuthMode = Ndis802_11AuthModeWPA2PSK;
						TmpAKMSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
						break;
					case 18:
						TmpAuthMode = Ndis802_11AuthModeWPA2PSK;
						TmpAKMSuite = RSN_AKM_SUITE_OWE;
						break;
#endif
					default:
						break;
					}
					if (TmpAuthMode > WPA2_AuthMode)
					{
						/* Move the lower AKM suite to WPA2_AuthModeAux */
						WPA2_AuthModeAux = WPA2_AuthMode;
						WPA2_AuthMode    = TmpAuthMode;
					}
					else
					{
						WPA2_AuthModeAux = TmpAuthMode;
					}
#ifdef WPA3_SUPPORT
					for (i = 0; i < MAX_NUM_SUPPORTED_AKM_SUITES; i++)
						if (TmpAKMSuite > WPA2.au4AuthKeyMgtSuite[i]) {
							j = i;
							for (j = (MAX_NUM_SUPPORTED_AKM_SUITES - 2); j >= i; j--)
								WPA2.au4AuthKeyMgtSuite[j+1] = WPA2.au4AuthKeyMgtSuite[j];
							WPA2.au4AuthKeyMgtSuite[i] = TmpAKMSuite;
							break;
						}
#endif
				    pTmp++;
					Count--;										
				}

				/* Check the Pair & Group, if different, turn on mixed mode flag */
#ifdef WPA3_SUPPORT
				if (WPA2.GroupCipher != WPA2.PairCipher[0])
					WPA2.bMixMode = TRUE;
#else
				if (WPA2.GroupCipher != WPA2.PairCipher)
					WPA2.bMixMode = TRUE;

				DBGPRINT(RT_DEBUG_TRACE, ("ApCliValidateRSNIE - RSN-WPA2 PairWiseCipher(%s), GroupCipher(%s), AuthMode(%s)\n",
									(WPA2.bMixMode ? "Mix" : GetEncryptType(WPA2.PairCipher)), GetEncryptType(WPA2.GroupCipher),
									GetAuthMode(WPA2_AuthMode)));
#endif
				Sanity |= 0x2;
				break;
			default:
					DBGPRINT(RT_DEBUG_WARN, ("ApCliValidateRSNIE - Unknown pEid->Eid(%d) \n", pEid->Eid));
				break;
		}

		/* skip this Eid */
		pVIE += (pEid->Len + 2);
		len  -= (pEid->Len + 2);
	
	}
#ifndef WPA3_SUPPORT
	/* 2. Validate this RSNIE with mine */
	pApCliEntry = &pAd->ApCfg.ApCliTab[idx];
	wdev = &pApCliEntry->wdev;

	/* Peer AP doesn't include WPA/WPA2 capable */
	if (Sanity == 0) 
	{
		/* Check the authenticaton mode */		
		if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s - The authentication mode doesn't match \n", __FUNCTION__));
			return FALSE;
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s - The pre-RSNA authentication mode is used. \n", __FUNCTION__));
			return TRUE;
		}
	}

	/* Recovery user-defined cipher suite */
	pApCliEntry->PairCipher  = wdev->WepStatus;
	pApCliEntry->GroupCipher = wdev->WepStatus;
	pApCliEntry->bMixCipher  = FALSE;

	Sanity = 0;	
	
	/* Check AuthMode and WPA_AuthModeAux for matching, in case AP support dual-AuthMode */
	/* WPAPSK */
	if ((WPA_AuthMode == wdev->AuthMode) || 
		((WPA_AuthModeAux != Ndis802_11AuthModeOpen) && (WPA_AuthModeAux == wdev->AuthMode)))
	{
		/* Check cipher suite, AP must have more secured cipher than station setting */
		if (WPA.bMixMode == FALSE)
		{
			if (wdev->WepStatus != WPA.GroupCipher)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - WPA validate cipher suite error \n"));
				return FALSE;
			}
		}

		/* check group cipher */
		if (wdev->WepStatus < WPA.GroupCipher)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - WPA validate group cipher error \n"));
			return FALSE;
		}

		/* check pairwise cipher, skip if none matched */
		/* If profile set to AES, let it pass without question. */
		/* If profile set to TKIP, we must find one mateched */
		if ((wdev->WepStatus == Ndis802_11TKIPEnable) && 
			(wdev->WepStatus != WPA.PairCipher) && 
			(wdev->WepStatus != WPA.PairCipherAux))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - WPA validate pairwise cipher error \n"));
			return FALSE;
		}	

		Sanity |= 0x1;
	}
	/* WPA2PSK */
	else if ((WPA2_AuthMode == wdev->AuthMode) || 
			 ((WPA2_AuthModeAux != Ndis802_11AuthModeOpen) && (WPA2_AuthModeAux == wdev->AuthMode)))
	{
		/* Check cipher suite, AP must have more secured cipher than station setting */
		if (WPA2.bMixMode == FALSE)
		{
			if (wdev->WepStatus != WPA2.GroupCipher)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - WPA2 validate cipher suite error \n"));
				return FALSE;
			}
		}

		/* check group cipher */
		if (wdev->WepStatus < WPA2.GroupCipher)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - WPA2 validate group cipher error \n"));
			return FALSE;
		}

		/* check pairwise cipher, skip if none matched */
		/* If profile set to AES, let it pass without question. */
		/* If profile set to TKIP, we must find one mateched */
		if ((wdev->WepStatus == Ndis802_11TKIPEnable) && 
			(wdev->WepStatus != WPA2.PairCipher) && 
			(wdev->WepStatus != WPA2.PairCipherAux))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - WPA2 validate pairwise cipher error \n"));
			return FALSE;
		}

		Sanity |= 0x2;
	}

	if (Sanity == 0) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("ApCliValidateRSNIE - Validate RSIE Failure \n"));
		return FALSE;
	}

	/* Re-assign pairwise-cipher and group-cipher. Re-build RSNIE.  */
	if ((wdev->AuthMode == Ndis802_11AuthModeWPA) || (wdev->AuthMode == Ndis802_11AuthModeWPAPSK))
	{
		pApCliEntry->GroupCipher = WPA.GroupCipher;
			
		if (wdev->WepStatus == WPA.PairCipher)
			pApCliEntry->PairCipher = WPA.PairCipher;
		else if (WPA.PairCipherAux != Ndis802_11WEPDisabled)
			pApCliEntry->PairCipher = WPA.PairCipherAux;
		else	/* There is no PairCipher Aux, downgrade our capability to TKIP */
			pApCliEntry->PairCipher = Ndis802_11TKIPEnable;			
	} else if ((wdev->AuthMode == Ndis802_11AuthModeWPA2) || (wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)) {
		pApCliEntry->GroupCipher = WPA2.GroupCipher;
			
		if (wdev->WepStatus == WPA2.PairCipher)
			pApCliEntry->PairCipher = WPA2.PairCipher;
		else if (WPA2.PairCipherAux != Ndis802_11WEPDisabled)
			pApCliEntry->PairCipher = WPA2.PairCipherAux;
		else	/* There is no PairCipher Aux, downgrade our capability to TKIP */
			pApCliEntry->PairCipher = Ndis802_11TKIPEnable;					
	}

	/* Set Mix cipher flag */
	if (pApCliEntry->PairCipher != pApCliEntry->GroupCipher)
	{
		pApCliEntry->bMixCipher = TRUE;	

		/* re-build RSNIE */
		/* RTMPMakeRSNIE(pAd, pApCliEntry->AuthMode, pApCliEntry->WepStatus, (idx + MIN_NET_DEVICE_FOR_APCLI)); */
	}
	
	/* re-build RSNIE */
	RTMPMakeRSNIE(pAd, wdev->AuthMode, wdev->WepStatus, (idx + MIN_NET_DEVICE_FOR_APCLI));
#endif
	return TRUE;	
}

BOOLEAN  ApCliHandleRxBroadcastFrame(
	IN  PRTMP_ADAPTER   pAd,
	IN	RX_BLK			*pRxBlk,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN	UCHAR			wdev_idx)
{
	RXINFO_STRUC *pRxInfo = pRxBlk->pRxInfo;
	PHEADER_802_11 pHeader = pRxBlk->pHeader;
//	RXWI_STRUC *pRxWI = pRxBlk->pRxWI;		
	PAPCLI_STRUCT pApCliEntry = NULL;
	struct wifi_dev *wdev;

	
	/* It is possible to receive the multicast packet when in AP Client mode */
	/* Such as a broadcast from remote AP to AP-client, address1 is ffffff, address2 is remote AP's bssid, addr3 is sta4 mac address */
																																								
	pApCliEntry	= &pAd->ApCfg.ApCliTab[pEntry->wdev_idx];																											
	wdev = &pApCliEntry->wdev;

					
	/* Filter out Bcast frame which AP relayed for us */
	/* Multicast packet send from AP1 , received by AP2 and send back to AP1, drop this frame   	 */				
	if (MAC_ADDR_EQUAL(pHeader->Addr3, wdev->if_addr))
		return FALSE;	/* give up this frame */

	if (pEntry->PrivacyFilter != Ndis802_11PrivFilterAcceptAll)
		return FALSE;	/* give up this frame */
					
#ifdef RELEASE_EXCLUDE					
	DBGPRINT(RT_DEBUG_INFO, ("IF-apcli%d : B/M-cast frame pRxBlk->wcid=%d, keyidx=%d \n", 
									pEntry->func_tb_idx, pRxBlk->wcid, pRxBlk->key_idx));
#endif /* RELEASE_EXCLUDE */
							
	/* skip the 802.11 header */
	pRxBlk->pData += LENGTH_802_11;
	pRxBlk->DataSize -= LENGTH_802_11;

	/* Use software to decrypt the encrypted frame. */
	/* Because this received frame isn't my BSS frame, Asic passed to driver without decrypting it. */
	/* If receiving an "encrypted" unicast packet(its WEP bit as 1) and doesn't match my BSSID, it  */
	/* pass to driver with "Decrypted" marked as 0 in RxD. */
//	if ((pRxD->MyBss == 0) && (pRxD->Decrypted == 0) && (pHeader->FC.Wep == 1)) 
	if ((pRxInfo->MyBss == 0) && (pRxInfo->Decrypted == 0) && (pHeader->FC.Wep == 1)) 
	{											
		if (RTMPSoftDecryptionAction(pAd, 
									 (PUCHAR)pHeader, 0, 
									 &pApCliEntry->SharedKey[pRxBlk->key_idx], 
									 pRxBlk->pData, 
									 &(pRxBlk->DataSize)) == NDIS_STATUS_FAILURE)			
		{						
			return FALSE;  /* give up this frame */
		}
	}
	pRxInfo->MyBss = 1;				
	Indicate_Legacy_Packet(pAd, pRxBlk, wdev_idx);								

	return TRUE;
}


VOID APCliInstallPairwiseKey(
	IN  PRTMP_ADAPTER   pAd,
	IN  MAC_TABLE_ENTRY *pEntry)
{
	UCHAR	IfIdx;
	UINT8	BssIdx;

	IfIdx = pEntry->func_tb_idx;

	BssIdx = pAd->ApCfg.BssidNum + MAX_MESH_NUM + IfIdx;

	WPAInstallPairwiseKey(pAd, 
						  BssIdx, 
						  pEntry, 
						  FALSE);	
}


BOOLEAN APCliInstallSharedKey(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pKey,
	IN  UCHAR           KeyLen,
	IN	UCHAR			DefaultKeyIdx,
	IN  MAC_TABLE_ENTRY *pEntry)
{
	UCHAR	IfIdx;
	UCHAR	GTK_len = 0;
	APCLI_STRUCT *pApCliEntry;
	struct wifi_dev *wdev;

	if (!pEntry || !IS_ENTRY_APCLI(pEntry))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : This Entry doesn't exist!!! \n", __FUNCTION__));		
		return FALSE;
	}

	IfIdx = pEntry->func_tb_idx;

	if (pAd->ApCfg.ApCliTab[IfIdx].GroupCipher == Ndis802_11TKIPEnable && KeyLen >= LEN_TKIP_GTK)
	{
		GTK_len = LEN_TKIP_GTK;
	}
	else if (pAd->ApCfg.ApCliTab[IfIdx].GroupCipher == Ndis802_11AESEnable && 
			 KeyLen >= LEN_AES_GTK)
	{
		GTK_len = LEN_AES_GTK;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : GTK is invalid (GroupCipher=%d, DataLen=%d) !!! \n", 
								__FUNCTION__, pAd->ApCfg.ApCliTab[IfIdx].GroupCipher, KeyLen));		
		return FALSE;
	}

	/* Update GTK */
	/* set key material, TxMic and RxMic for WPAPSK	 */
	pApCliEntry = &pAd->ApCfg.ApCliTab[IfIdx];
	wdev = &pApCliEntry->wdev;


	NdisMoveMemory(pApCliEntry->GTK, pKey, GTK_len);
//iversonnote
	pApCliEntry->wdev.DefaultKeyId = DefaultKeyIdx;

	/* Update shared key table */
	NdisZeroMemory(&pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx], sizeof(CIPHER_KEY));  
	pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].KeyLen = GTK_len;
	NdisMoveMemory(pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].Key, pKey, LEN_TK);
	if (GTK_len == LEN_TKIP_GTK)
	{
		NdisMoveMemory(pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].RxMic, pKey + 16, LEN_TKIP_MIC);
		NdisMoveMemory(pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].TxMic, pKey + 24, LEN_TKIP_MIC);
	}

	/* Update Shared Key CipherAlg */
	pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].CipherAlg = CIPHER_NONE;
	if (pAd->ApCfg.ApCliTab[IfIdx].GroupCipher == Ndis802_11TKIPEnable)
		pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].CipherAlg = CIPHER_TKIP;
	else if (pAd->ApCfg.ApCliTab[IfIdx].GroupCipher == Ndis802_11AESEnable)
		pAd->ApCfg.ApCliTab[IfIdx].SharedKey[DefaultKeyIdx].CipherAlg = CIPHER_AES;

	return TRUE;
}

VOID APCli_Init(
	IN	PRTMP_ADAPTER				pAd,
	IN	RTMP_OS_NETDEV_OP_HOOK		*pNetDevOps)
{
#define APCLI_MAX_DEV_NUM	32
	PNET_DEV new_dev_p;
	INT idx;
	APCLI_STRUCT *pApCliEntry;
	struct wifi_dev *wdev;
	
	/* sanity check to avoid redundant virtual interfaces are created */
	if (pAd->flg_apcli_init != FALSE)
		return;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

	/* init */
	for(idx = 0; idx < MAX_APCLI_NUM; idx++)
		pAd->ApCfg.ApCliTab[idx].wdev.if_dev = NULL;

	/* create virtual network interface */
	for (idx = 0; idx < MAX_APCLI_NUM; idx++)
	{
		UINT32 MC_RowID = 0, IoctlIF = 0;
		char *dev_name;

#ifdef MULTIPLE_CARD_SUPPORT
		MC_RowID = pAd->MC_RowID;
#endif /* MULTIPLE_CARD_SUPPORT */
#ifdef HOSTAPD_SUPPORT
		IoctlIF = pAd->IoctlIF;
#endif /* HOSTAPD_SUPPORT */

	dev_name = get_dev_name_prefix(pAd, INT_APCLI);
		new_dev_p = RtmpOSNetDevCreate(MC_RowID, &IoctlIF, INT_APCLI, idx,
									sizeof(struct mt_dev_priv), dev_name);
		if (!new_dev_p) {
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): Create net_device for %s(%d) fail!\n",
						__FUNCTION__, dev_name, idx));
			break;
		}
#ifdef HOSTAPD_SUPPORT
		pAd->IoctlIF = IoctlIF;
#endif /* HOSTAPD_SUPPORT */
//iversonnote
#if 0		
		pApCliEntry = &pAd->ApCfg.ApCliTab[idx];
		wdev = &pApCliEntry->wdev;
		wdev->wdev_type = WDEV_TYPE_STA;
		wdev->func_dev = pApCliEntry;
		wdev->sys_handle = (void *)pAd;
		wdev->if_dev = new_dev_p;
		wdev->tx_pkt_allowed = ApCliAllowToSendPacket;
		wdev->wdev_hard_tx = APHardTransmit;
#endif
		RTMP_OS_NETDEV_SET_PRIV(new_dev_p, pAd);
		RTMP_OS_NETDEV_SET_WDEV(new_dev_p, wdev);

		if (rtmp_wdev_idx_reg(pAd, wdev) < 0) {
			DBGPRINT(RT_DEBUG_ERROR, ("Assign wdev idx for %s failed, free net device!\n",
						RTMP_OS_NETDEV_GET_DEVNAME(new_dev_p)));
			RtmpOSNetDevFree(new_dev_p);
			break;
		}

		/* init MAC address of virtual network interface */
		COPY_MAC_ADDR(wdev->if_addr, pAd->CurrentAddress);

		if (pAd->chipCap.MBSSIDMode >= MBSSID_MODE1)
		{
			if ((pAd->ApCfg.BssidNum > 0) || (MAX_MESH_NUM > 0)) 
		{
				UCHAR MacMask = 0;

				if ((pAd->ApCfg.BssidNum + MAX_APCLI_NUM + MAX_MESH_NUM) <= 2)
					MacMask = 0xFE;
				else if ((pAd->ApCfg.BssidNum + MAX_APCLI_NUM + MAX_MESH_NUM) <= 4)
					MacMask = 0xFC;
				else if ((pAd->ApCfg.BssidNum + MAX_APCLI_NUM + MAX_MESH_NUM) <= 8)
					MacMask = 0xF8;
#ifdef SPECIFIC_BCN_BUF_SUPPORT
				else if ((pAd->ApCfg.BssidNum + MAX_APCLI_NUM + MAX_MESH_NUM) <= 16)
					MacMask = 0xF0;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */

				/* 	
					Refer to HW definition - 
						Bit1 of MAC address Byte0 is local administration bit 
						and should be set to 1 in extended multiple BSSIDs'
						Bit3~ of MAC address Byte0 is extended multiple BSSID index.
				 */
				if (pAd->chipCap.MBSSIDMode == MBSSID_MODE1)
			{
				/* 	
					Refer to HW definition - 
						Bit1 of MAC address Byte0 is local administration bit 
						and should be set to 1 in extended multiple BSSIDs'
						Bit3~ of MAC address Byte0 is extended multiple BSSID index.
				 */ 
#ifdef ENHANCE_NEW_MBSSID_MODE
					wdev->if_addr[0] &= (MacMask << 2);
#endif /* ENHANCE_NEW_MBSSID_MODE */
					wdev->if_addr[0] |= 0x2;
					wdev->if_addr[0] += (((pAd->ApCfg.BssidNum + MAX_MESH_NUM) - 1) << 2);
				}
#ifdef ENHANCE_NEW_MBSSID_MODE
				else
				{
					wdev->if_addr[0] |= 0x2;
					wdev->if_addr[pAd->chipCap.MBSSIDMode - 1] &= (MacMask);
					wdev->if_addr[pAd->chipCap.MBSSIDMode - 1] += ((pAd->ApCfg.BssidNum + MAX_MESH_NUM) - 1);
				}
#endif /* ENHANCE_NEW_MBSSID_MODE */
			}
		}
		else
		{
			wdev->if_addr[MAC_ADDR_LEN - 1] = (wdev->if_addr[MAC_ADDR_LEN - 1] + pAd->ApCfg.BssidNum + MAX_MESH_NUM) & 0xFF;
		}
		
		pNetDevOps->priv_flags = INT_APCLI; /* we are virtual interface */
		pNetDevOps->needProtcted = TRUE;
		pNetDevOps->wdev = wdev;
		NdisMoveMemory(pNetDevOps->devAddr, &wdev->if_addr[0], MAC_ADDR_LEN);

		/* register this device to OS */
		RtmpOSNetDevAttach(pAd->OpMode, new_dev_p, pNetDevOps);
//iversonnote
		/* backup our virtual network interface */
//		pApCliEntry->dev = new_dev_p;
        
#ifdef WSC_AP_SUPPORT
		pApCliEntry->WscControl.pAd = pAd;        
		NdisZeroMemory(pApCliEntry->WscControl.EntryAddr, MAC_ADDR_LEN);
		pApCliEntry->WscControl.WscConfigMethods= 0x018C;
/*		WscGenerateUUID(pAd, &pApCliEntry->WscControl.Wsc_Uuid_E[0], &pApCliEntry->WscControl.Wsc_Uuid_Str[0], 0, FALSE); */
/*		WscInit(pAd, TRUE, apcli_index); */
		RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_WSC_INIT, 0, (VOID *)pApCliEntry, idx);
#endif /* WSC_AP_SUPPORT */

	}

	pAd->flg_apcli_init = TRUE;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s <---\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
}


VOID ApCli_Remove(
	IN PRTMP_ADAPTER 	pAd)
{
	UINT index;
	struct wifi_dev *wdev;

	for(index = 0; index < MAX_APCLI_NUM; index++)
	{
		wdev = &pAd->ApCfg.ApCliTab[index].wdev;
		if (wdev->if_dev)
		{
			RtmpOSNetDevDetach(wdev->if_dev);
			rtmp_wdev_idx_unreg(pAd, wdev);
			RtmpOSNetDevFree(wdev->if_dev);

			/* Clear it as NULL to prevent latter access error. */
			pAd->flg_apcli_init = FALSE;
			wdev->if_dev = NULL;
		}
	}
}


BOOLEAN ApCli_Open(
	IN	PRTMP_ADAPTER		pAd,
	IN	PNET_DEV			dev_p)
{
	UCHAR ifIndex;


	for (ifIndex = 0; ifIndex < MAX_APCLI_NUM; ifIndex++)
	{
		if (pAd->ApCfg.ApCliTab[ifIndex].wdev.if_dev == dev_p)
		{
			RTMP_OS_NETDEV_START_QUEUE(dev_p);
			ApCliIfUp(pAd);
//iversonnote
//#ifdef APCLI_WPA_SUPPLICANT_SUPPORT
//			RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_INTERFACE_UP, NULL, NULL, 0);
//#endif /* APCLI_WPA_SUPPLICANT_SUPPORT */
			return TRUE;
		}
	}

	return FALSE;
}


BOOLEAN ApCli_Close(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNET_DEV		dev_p)
{
	UCHAR ifIndex;
#ifdef MAC_REPEATER_SUPPORT
	UCHAR CliIdx;
#endif /* MAC_REPEATER_SUPPORT */
	struct wifi_dev *wdev;
	APCLI_STRUCT *apcli_entry;

	for (ifIndex = 0; ifIndex < MAX_APCLI_NUM; ifIndex++)
	{
		apcli_entry = &pAd->ApCfg.ApCliTab[ifIndex];
		wdev = &apcli_entry->wdev;
		if (wdev->if_dev == dev_p)
		{
#ifdef APCLI_WPA_SUPPLICANT_SUPPORT
			RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_INTERFACE_DOWN, NULL, NULL, 0);

			if (apcli_entry->pWpaAssocIe)
		{
				os_free_mem(NULL, apcli_entry->pWpaAssocIe);
				apcli_entry->pWpaAssocIe = NULL;
				apcli_entry->WpaAssocIeLen = 0;
			}
#endif /* APCLI_WPA_SUPPLICANT_SUPPORT */

			RTMP_OS_NETDEV_STOP_QUEUE(dev_p);

			/* send disconnect-req to sta State Machine. */
			if (apcli_entry->Enable)
			{
#ifdef MAC_REPEATER_SUPPORT
				if (pAd->ApCfg.bMACRepeaterEn)
				{
					for(CliIdx = 0; CliIdx < MAX_EXT_MAC_ADDR_SIZE; CliIdx++)
			{
						if (pAd->ApCfg.ApCliTab[ifIndex].RepeaterCli[CliIdx].CliEnable)
						{
							MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DISCONNECT_REQ, 0, NULL, (64 + MAX_EXT_MAC_ADDR_SIZE*ifIndex + CliIdx));
							RTMP_MLME_HANDLER(pAd);
							RTMPRemoveRepeaterEntry(pAd, ifIndex, CliIdx);
						}
					}
				}
#endif /* MAC_REPEATER_SUPPORT */

				MlmeEnqueue(pAd, APCLI_CTRL_STATE_MACHINE, APCLI_CTRL_DISCONNECT_REQ, 0, NULL, ifIndex);
				RTMP_MLME_HANDLER(pAd);
				DBGPRINT(RT_DEBUG_TRACE, ("(%s) ApCli interface[%d] startdown.\n", __FUNCTION__, ifIndex));
			}
			return TRUE;
		}
	}

	return FALSE;
}

#endif /* P2P_SUPPORT */

