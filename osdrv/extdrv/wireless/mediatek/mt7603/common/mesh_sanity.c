/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
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
	mesh_link_mng.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-07-23      For mesh (802.11s) support.
*/
#ifdef MESH_SUPPORT


#include "rt_config.h"
#include "mesh_def.h"
#include "mesh_sanity.h"

BOOLEAN MeshPeerBeaconAndProbeSanity(
	IN PRTMP_ADAPTER pAd,
	IN VOID *Msg,
	IN ULONG MsgLen,
	OUT PUCHAR pHostName,
	OUT PUCHAR pHostNameLen,
	OUT PUCHAR pMeshId,
	OUT PUCHAR pMeshIdLen,
	OUT PMESH_CONFIGURAION_IE pMeshConfiguration)
{
	CHAR *Ptr;
	PFRAME_802_11 pFrame;
	PEID_STRUCT pEid;
	UCHAR SubType;
	ULONG Length = 0;

	pFrame = (PFRAME_802_11)Msg;
    
	/* get subtype from header */
	SubType = (UCHAR)pFrame->Hdr.FC.SubType;

	/*hex_dump("Beacon", Msg, MsgLen); */

	Ptr = (PCHAR)pFrame->Octet;
	Length += LENGTH_802_11;

	/* skip timestamp from payload and advance the pointer */
	Ptr += TIMESTAMP_LEN;
	Length += TIMESTAMP_LEN;

	/* skip beacon interval from payload and advance the pointer */
	Ptr += 2;
	Length += 2;

	/* skip capability info from payload and advance the pointer */
	Ptr += 2;
	Length += 2;

	pEid = (PEID_STRUCT) Ptr;

	/* get variable fields from payload and advance the pointer */
	while ((Length + 2 + pEid->Len) <= MsgLen)    
	{
		switch(pEid->Eid)
		{
			case IE_VENDOR_SPECIFIC:
				if (NdisEqualMemory(pEid->Octet, MeshOUI, 3))
				{
					PUCHAR ptr = (PUCHAR)(pEid->Octet + 3);
					*pHostNameLen = pEid->Len -3;
					NdisMoveMemory(pHostName, ptr, *pHostNameLen);
				}
				break;

			case IE_MESH_ID:
				NdisMoveMemory(pMeshId, pEid->Octet, pEid->Len);
				*pMeshIdLen = pEid->Len;
				break;

			case IE_MESH_CONFIGURATION:
				{
					PUCHAR ptr;
					NdisMoveMemory(&pMeshConfiguration->Version, pEid->Octet, 1);

					if (RTMPEqualMemory(pEid->Octet + 1, MeshOUI, 3))
						NdisMoveMemory(&pMeshConfiguration->PathSelProtocolId, pEid->Octet + 4, 1);

					if (RTMPEqualMemory(pEid->Octet + 5, MeshOUI, 3))
						NdisMoveMemory(&pMeshConfiguration->PathSelMetricId, pEid->Octet + 8, 1);

					ptr = (PUCHAR)&pEid->Octet[0];
					NdisMoveMemory(&pMeshConfiguration->CPI, ptr + 13, 4);
					pMeshConfiguration->CPI = le2cpu32(*(UINT32 *)(&pMeshConfiguration->CPI));

					NdisMoveMemory(&pMeshConfiguration->MeshCapability.word, ptr + 17, 2);
					pMeshConfiguration->MeshCapability.word = le2cpu16(*(UINT16 *)(&pMeshConfiguration->MeshCapability.word));
				}
				break;

			default:
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("MeshPeerBeaconAndProbeSanity - unrecognized EID = %d\n", pEid->Eid));
#endif /* RELEASE_EXCLUDE */
				break;
		}
        
		Length = Length + 2 + pEid->Len;  /* Eid[1] + Len[1]+ content[Len] */
		pEid = (PEID_STRUCT)((UCHAR*)pEid + 2 + pEid->Len);        
	}

	return TRUE;
}

BOOLEAN MeshLinkMngOpenSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT16 *pCapabilityInfo,
		OUT UCHAR SupRate[],
		OUT PUCHAR pSupRateLen,
		OUT PUCHAR pMeshIdLen,
		OUT	PUCHAR pMeshId,
		OUT PUCHAR pMeshSelPathId,
		OUT PUCHAR pMeshSelMetricId,
		OUT UINT32 *pCPI,
		OUT PMESH_CAPABILITY pMeshCapabilty,
		OUT UINT16 *pPeerLinkId,
		OUT PMESH_SECURITY_CAPABILITY_IE pMscIe,
		OUT PUCHAR pMsaIe,
		OUT PUCHAR pMsaIeLen,
		OUT PUCHAR pRsnIe,
		OUT PUCHAR pRsnIeLen,
		OUT BOOLEAN *pbWmmCapable,
	    OUT HT_CAPABILITY_IE *pHtCapability,
		OUT UCHAR *pHtCapabilityLen)
{
	PEID_STRUCT eid_ptr;
	UCHAR       WPA1_OUI[4]={0x00,0x50,0xF2,0x01};
    UCHAR       WPA2_OUI[3]={0x00,0x0F,0xAC};

	*pMsaIeLen = 0;
	*pRsnIeLen = 0;

	NdisMoveMemory(pCapabilityInfo, pMsg, 2);
	pMsg += 2;
	MsgLen -= 2;
	*pbWmmCapable = FALSE;

	eid_ptr = (PEID_STRUCT)pMsg;
	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
            case IE_SUPP_RATES:
                if ((eid_ptr->Len <= MAX_LEN_OF_SUPPORTED_RATES) && (eid_ptr->Len > 0))
                {
                    NdisMoveMemory(SupRate, eid_ptr->Octet, eid_ptr->Len);
#ifdef RELEASE_EXCLUDE
                    DBGPRINT(RT_DEBUG_INFO, ("%s - IE_SUPP_RATES., Len=%d. Rates[0]=%x\n", __FUNCTION__, eid_ptr->Len, SupRate[0]));
                    DBGPRINT(RT_DEBUG_INFO, ("SupRate[1]=%x %x %x %x %x %x %x\n",
						SupRate[1], SupRate[2], SupRate[3], SupRate[4], SupRate[5], SupRate[6], SupRate[7]));
#endif /* RELEASE_EXCLUDE */
                    
                    *pSupRateLen = eid_ptr->Len;
                }
                else
                {
                	/* HT rate not ready yet. return true temporarily. */
                    /*DBGPRINT(RT_DEBUG_TRACE, ("PeerAssocReqSanity - wrong IE_SUPP_RATES\n")); */
                    *pSupRateLen = 8;
					SupRate[0] = 0x82;
					SupRate[1] = 0x84;
					SupRate[2] = 0x8b;
					SupRate[3] = 0x96;
					SupRate[4] = 0x12;
					SupRate[5] = 0x24;
					SupRate[6] = 0x48;
					SupRate[7] = 0x6c;
#ifdef RELEASE_EXCLUDE
                    DBGPRINT(RT_DEBUG_INFO, ("PeerAssocReqSanity - wrong IE_SUPP_RATES., Len=%d\n",eid_ptr->Len));
#endif /* RELEASE_EXCLUDE */
                    /*return FALSE; */
                }
                break;
                
            case IE_EXT_SUPP_RATES:
                if (eid_ptr->Len + *pSupRateLen <= MAX_LEN_OF_SUPPORTED_RATES)
                {
                    NdisMoveMemory(&SupRate[*pSupRateLen], eid_ptr->Octet, eid_ptr->Len);
                    *pSupRateLen = (*pSupRateLen) + eid_ptr->Len;
                }
                else
                {
                    NdisMoveMemory(&SupRate[*pSupRateLen], eid_ptr->Octet, MAX_LEN_OF_SUPPORTED_RATES - (*pSupRateLen));
                    *pSupRateLen = MAX_LEN_OF_SUPPORTED_RATES;
                }
                break;

			case IE_WPA:  
            case IE_WPA2:
				if (NdisEqualMemory(eid_ptr->Octet, WME_INFO_ELEM, 6) && (eid_ptr->Len == 7))
				{
					*pbWmmCapable = TRUE;
					break;
				}

				if (pAd->MeshTab.wdev.AuthMode < Ndis802_11AuthModeWPA)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("The AuthMode isn't WPA!!!%d \n",pAd->MeshTab.wdev.AuthMode));
                    break;
				}

				/* If this IE did not begins with "0x00:0x50:0xf2:0x01" or "0x00:0x0f:0xac", it would be proprietary.  So we ignore it. */
                if (!NdisEqualMemory(eid_ptr->Octet, WPA1_OUI, sizeof(WPA1_OUI))
                    && !NdisEqualMemory(&eid_ptr->Octet[2], WPA2_OUI, sizeof(WPA2_OUI)))
                {
                    DBGPRINT(RT_DEBUG_TRACE, ("Not RSN IE, maybe proprietary IE!!!\n"));
                    break;                          
                }

				if (/*(eid_ptr->Len <= MAX_LEN_OF_RSNIE) && */(eid_ptr->Len > MIN_LEN_OF_RSNIE))
				{
					NdisMoveMemory(pRsnIe, eid_ptr, eid_ptr->Len + 2);
					*pRsnIeLen = eid_ptr->Len + 2;
				}
				else
                {
                    *pRsnIeLen = 0;
                    DBGPRINT(RT_DEBUG_TRACE, ("The received the length(%d) of RSN_IE is invalid\n",eid_ptr->Len));
                    break;  
                }
				DBGPRINT(RT_DEBUG_TRACE, ("Link Msg - RSNIE(%d), and its len(%d)!!!\n", eid_ptr->Eid, *pRsnIeLen));
				break;
					
			case IE_MESH_ID:
				if (eid_ptr->Len < MAX_MESH_ID_LEN)
					NdisMoveMemory(pMeshId, eid_ptr->Octet, eid_ptr->Len);
				break;

			case IE_MESH_CONFIGURATION:
				if (NdisEqualMemory(eid_ptr->Octet + 1, MeshOUI, 3))
					NdisMoveMemory(pMeshSelPathId, eid_ptr->Octet + 4, 1);

				if (NdisEqualMemory(eid_ptr->Octet + 5, MeshOUI, 3))
					NdisMoveMemory(pMeshSelMetricId, eid_ptr->Octet + 8, 1);

				NdisMoveMemory(pCPI, eid_ptr->Octet + 13, 4);
				*pCPI = le2cpu32(*(UINT32 *)pCPI);
				NdisMoveMemory(&pMeshCapabilty->word, eid_ptr->Octet + 17, 2);
				pMeshCapabilty->word = le2cpu16(*(UINT16 *)(&pMeshCapabilty->word));
				break;

			case IE_MESH_PEER_LINK_MANAGEMENT:
				if ((*eid_ptr->Octet == SUBTYPE_PEER_LINK_OPEN) /* subtype 1 means Peer-Link-Open frame. */
					&& (eid_ptr->Len == LenPeerLinkMngIE[SUBTYPE_PEER_LINK_OPEN])) /* Length of Peer-Link-open must be 3. */
				{
					NdisMoveMemory(pPeerLinkId, eid_ptr->Octet + 1, 2);
					*pPeerLinkId = le2cpu16(*pPeerLinkId);
				}
				break;

			case IE_MESH_MSCIE:
				if (eid_ptr->Len == 7)  /* its Length MUST be 7 */
				{
					NdisMoveMemory(pMscIe, eid_ptr->Octet, eid_ptr->Len);
				}
				break;

			case IE_MESH_MSAIE:
				if (eid_ptr->Len >= sizeof(MSA_HANDSHAKE_IE))
				{					
					NdisMoveMemory(pMsaIe, eid_ptr->Octet, eid_ptr->Len);
					*pMsaIeLen = eid_ptr->Len;
				}
				break;

            case IE_HT_CAP:
				if (eid_ptr->Len >= sizeof(HT_CAPABILITY_IE))
				{
					NdisMoveMemory(pHtCapability, eid_ptr->Octet, SIZE_HT_CAP_IE);

					*(USHORT *)(&pHtCapability->HtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->HtCapInfo));
					*(USHORT *)(&pHtCapability->ExtHtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->ExtHtCapInfo));

					*pHtCapabilityLen = SIZE_HT_CAP_IE;
					DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - IE_HT_CAP\n"));
				}
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - wrong IE_HT_CAP.eid_ptr->Len = %d\n", eid_ptr->Len));
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

BOOLEAN MeshLinkMngCfnSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT16 *pCapabilityInfo,
		OUT UINT16 *pStatusCode,
		OUT UINT16 *pAid,
		OUT UCHAR SupRate[],
		OUT PUCHAR pSupRateLen,
		OUT PUCHAR pMeshIdLen,
		OUT	PUCHAR pMeshId,
		OUT PUCHAR pMeshSelPathId,
		OUT PUCHAR pMeshSelMetricId,
		OUT UINT32 *pCPI,
		OUT PMESH_CAPABILITY pMeshCapabilty,
		OUT UINT16 *pLocalLinkId,
		OUT UINT16 *pPeerLinkId,
		OUT PMESH_SECURITY_CAPABILITY_IE pMscIe,
		OUT PUCHAR pMsaIe,
		OUT PUCHAR pMsaIeLen,
		OUT PUCHAR pRsnIe,
		OUT PUCHAR pRsnIeLen,
		OUT HT_CAPABILITY_IE *pHtCapability,
		OUT UCHAR *pHtCapabilityLen)
{
	PEID_STRUCT eid_ptr;
	UCHAR       WPA1_OUI[4]={0x00,0x50,0xF2,0x01};
    UCHAR       WPA2_OUI[3]={0x00,0x0F,0xAC};

	*pMsaIeLen = 0;
	*pRsnIeLen = 0;

	NdisMoveMemory(pCapabilityInfo, pMsg, 2);
	pMsg += 2;
	MsgLen -= 2;

	NdisMoveMemory(pStatusCode, pMsg, 2);
	pMsg += 2;
	MsgLen -= 2;

	NdisMoveMemory(pAid, pMsg, 2);
	pMsg += 2;
	MsgLen -= 2;


	eid_ptr = (PEID_STRUCT)pMsg;
	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
            case IE_SUPP_RATES:
                if ((eid_ptr->Len <= MAX_LEN_OF_SUPPORTED_RATES) && (eid_ptr->Len > 0))
                {
                    NdisMoveMemory(SupRate, eid_ptr->Octet, eid_ptr->Len);
#ifdef RELEASE_EXCLUDE
                    DBGPRINT(RT_DEBUG_INFO, ("%s - IE_SUPP_RATES., Len=%d. Rates[0]=%x\n", __FUNCTION__, eid_ptr->Len, SupRate[0]));
                    DBGPRINT(RT_DEBUG_INFO, ("SupRate[1]=%x %x %x %x %x %x %x\n",
						SupRate[1], SupRate[2], SupRate[3], SupRate[4], SupRate[5], SupRate[6], SupRate[7]));
#endif /* RELEASE_EXCLUDE */
                    
                    *pSupRateLen = eid_ptr->Len;
                }
                else
                {
                	/* HT rate not ready yet. return true temporarily. */
                    /*DBGPRINT(RT_DEBUG_TRACE, ("PeerAssocReqSanity - wrong IE_SUPP_RATES\n")); */
                    *pSupRateLen = 8;
					SupRate[0] = 0x82;
					SupRate[1] = 0x84;
					SupRate[2] = 0x8b;
					SupRate[3] = 0x96;
					SupRate[4] = 0x12;
					SupRate[5] = 0x24;
					SupRate[6] = 0x48;
					SupRate[7] = 0x6c;
#ifdef RELEASE_EXCLUDE
                    DBGPRINT(RT_DEBUG_INFO, ("PeerAssocReqSanity - wrong IE_SUPP_RATES., Len=%d\n",eid_ptr->Len));
#endif /* RELEASE_EXCLUDE */
                    
                    /*return FALSE; */
                }
                break;
                
            case IE_EXT_SUPP_RATES:
                if (eid_ptr->Len + *pSupRateLen <= MAX_LEN_OF_SUPPORTED_RATES)
                {
                    NdisMoveMemory(&SupRate[*pSupRateLen], eid_ptr->Octet, eid_ptr->Len);
                    *pSupRateLen = (*pSupRateLen) + eid_ptr->Len;
                }
                else
                {
                    NdisMoveMemory(&SupRate[*pSupRateLen], eid_ptr->Octet, MAX_LEN_OF_SUPPORTED_RATES - (*pSupRateLen));
                    *pSupRateLen = MAX_LEN_OF_SUPPORTED_RATES;
                }
                break;

			case IE_WPA:  
            case IE_WPA2:
				if (pAd->MeshTab.wdev.AuthMode < Ndis802_11AuthModeWPA)
                    break;

				/* If this IE did not begins with "0x00:0x50:0xf2:0x01" or "0x00:0x0f:0xac", it would be proprietary.  So we ignore it. */
                if (!NdisEqualMemory(eid_ptr->Octet, WPA1_OUI, sizeof(WPA1_OUI))
                    && !NdisEqualMemory(&eid_ptr->Octet[2], WPA2_OUI, sizeof(WPA2_OUI)))
                {
                    DBGPRINT(RT_DEBUG_TRACE, ("Not RSN IE, maybe proprietary IE!!!\n"));
                    break;                          
                }

				if (/*(eid_ptr->Len <= MAX_LEN_OF_RSNIE) && */(eid_ptr->Len > MIN_LEN_OF_RSNIE))
				{
					NdisMoveMemory(pRsnIe, eid_ptr, eid_ptr->Len + 2);
					*pRsnIeLen = eid_ptr->Len + 2;
				}
				else
                {
                    *pRsnIeLen = 0;
                    DBGPRINT(RT_DEBUG_TRACE, ("The received the length(%d) of RSN_IE is invalid\n",eid_ptr->Len));
                    break;  
                }  

				break;
					
			case IE_MESH_ID:
				if (eid_ptr->Len < MAX_MESH_ID_LEN)
					NdisMoveMemory(pMeshId, eid_ptr->Octet, eid_ptr->Len);
				break;

			case IE_MESH_CONFIGURATION:
				if (NdisEqualMemory(eid_ptr->Octet + 1, MeshOUI, 3))
				{
					*pMeshSelPathId = eid_ptr->Octet[4];
				}
				if (NdisEqualMemory(eid_ptr->Octet + 5, MeshOUI, 3))
				{
					*pMeshSelMetricId = eid_ptr->Octet[8];
				}
				NdisMoveMemory(pCPI, eid_ptr->Octet + 13, 4);
				*pCPI = le2cpu32(*pCPI);
				NdisMoveMemory(&pMeshCapabilty->word, eid_ptr->Octet + 17, 2);
				pMeshCapabilty->word = le2cpu16(pMeshCapabilty->word);
				break;

			case IE_MESH_PEER_LINK_MANAGEMENT:
				{
					if ((eid_ptr->Octet[0] == SUBTYPE_PEER_LINK_CONFIRM) /* subtype 1 means Peer-Link-Confirm frame. */
						&& (eid_ptr->Len == LenPeerLinkMngIE[SUBTYPE_PEER_LINK_CONFIRM])) /* Length of Peer-Link-close must be 3. */
					{
						NdisMoveMemory(pLocalLinkId, eid_ptr->Octet + 1, 2);
						*pLocalLinkId = le2cpu16(*(UINT16 *)(pLocalLinkId));
						NdisMoveMemory(pPeerLinkId, eid_ptr->Octet + 3, 2);
						*pPeerLinkId = le2cpu16(*(UINT16 *)(pPeerLinkId));
					}
				}
				break;

			case IE_MESH_MSCIE:
				if (eid_ptr->Len == 7)  /* its Length MUST be 7 */
				{
					NdisMoveMemory(pMscIe,eid_ptr->Octet, eid_ptr->Len);
				}
				break;

			case IE_MESH_MSAIE:
				if (eid_ptr->Len >= sizeof(MSA_HANDSHAKE_IE))
				{					
					NdisMoveMemory(pMsaIe,eid_ptr->Octet, eid_ptr->Len);
					*pMsaIeLen = eid_ptr->Len;
				}
				break;

            case IE_HT_CAP:
				if (eid_ptr->Len >= sizeof(HT_CAPABILITY_IE))
				{
					NdisMoveMemory(pHtCapability, eid_ptr->Octet, SIZE_HT_CAP_IE);

					*(USHORT *)(&pHtCapability->HtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->HtCapInfo));
					*(USHORT *)(&pHtCapability->ExtHtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->ExtHtCapInfo));

					*pHtCapabilityLen = SIZE_HT_CAP_IE;
					DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - IE_HT_CAP\n"));
				}
				else
				{
					DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - wrong IE_HT_CAP.eid_ptr->Len = %d\n", eid_ptr->Len));
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

BOOLEAN MeshLinkMngClsSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT16 *pLocalLinkId,
		OUT UINT16 *pPeerLinkId,
		OUT UINT16 *pReasonCode)
{
	PEID_STRUCT eid_ptr;

	eid_ptr = (PEID_STRUCT)pMsg;
	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_PEER_LINK_MANAGEMENT:
				{
					if ((eid_ptr->Octet[0] == SUBTYPE_PEER_LINK_CLOSE) /* subtype 1 means Peer-Link-Close frame. */
						&& (eid_ptr->Len == LenPeerLinkMngIE[SUBTYPE_PEER_LINK_CLOSE])) /* Length of Peer-Link-close must be 7. */
					{
						NdisMoveMemory(pLocalLinkId, eid_ptr->Octet + 1, 2);
						*pLocalLinkId = le2cpu16(*(UINT16 *)(pLocalLinkId));
						NdisMoveMemory(pPeerLinkId, eid_ptr->Octet + 3, 2);
						*pPeerLinkId = le2cpu16(*(UINT16 *)(pPeerLinkId));
						NdisMoveMemory(pReasonCode, eid_ptr->Octet + 5, 2);
						*pReasonCode = le2cpu16(*(UINT16 *)(pReasonCode));
					}
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

BOOLEAN MeshPathSelMultipathNoticeSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT8 *pFlag,
		OUT PUCHAR pMeshSA)
{
	PEID_STRUCT eid_ptr;

	eid_ptr = (PEID_STRUCT)pMsg;
	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_MULITI_PATH_NOTICE_IE:
				{
					if (eid_ptr->Len == 7) /* Length must be 7. */
					{
						NdisMoveMemory(pFlag, eid_ptr->Octet, 1);
						NdisMoveMemory(pMeshSA, eid_ptr->Octet + 1, MAC_ADDR_LEN);
					}
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}


BOOLEAN MeshChannelSwitchAnnouncementSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT8 *pChSwMode,
		OUT UINT8 *pNewCh,
		OUT UINT32 *pNewCPI,
		OUT UINT8 *pChSwCnt,
		OUT PUCHAR pMeshSA)
{
	PEID_STRUCT eid_ptr;

	eid_ptr = (PEID_STRUCT)pMsg;
	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_CHANNEL_SWITCH_ANNOUNCEMENT:
				{
					if (eid_ptr->Len == 13) /* Length must be 7. */
					{
						NdisMoveMemory(pChSwMode, eid_ptr->Octet, 1);
						NdisMoveMemory(pNewCh, eid_ptr->Octet + 1, 1);
						NdisMoveMemory(pNewCPI, eid_ptr->Octet + 2, 4);
						*pNewCPI = le2cpu32(*pNewCPI);
						NdisMoveMemory(pChSwCnt, eid_ptr->Octet + 6, 1);
						NdisMoveMemory(pMeshSA, eid_ptr->Octet + 7, MAC_ADDR_LEN);
					}
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);
	}

	return TRUE;
}

/*
    ==========================================================================
    Description:
       Check sanity RSN IE and get PMKID offset.
    Return:
         TRUE if match
         FALSE otherwise
    ==========================================================================
*/
BOOLEAN MeshValidateRSNIE(
    IN 	PRTMP_ADAPTER    pAd,
    IN 	PUCHAR			pRsnIe,
    IN	USHORT			peerRsnIeLen,
    OUT	UCHAR			*PureRsnLen,
    OUT	UCHAR			*PmkIdLen)
{	
	PEID_STRUCT      eid_ptr;
	PUCHAR			 pBufTmp;
	USHORT			 Count;
	UCHAR			 offset = 0;
	BOOLEAN			 isValid = FALSE;
	BOOLEAN			 RsnEid = IE_WPA;

	/* Check if both MPs are RSNIE */
	if (pAd->MeshTab.wdev.AuthMode < Ndis802_11AuthModeWPA)
	{
		if (peerRsnIeLen == 0)
		{
			/* Don't check it */
			return TRUE;
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the local MP has no RSNIE \n"));
			return FALSE;
		}
	}
	else
	{
		if (peerRsnIeLen == 0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the peer MP has no RSNIE \n"));
			return FALSE;
		}		
	}
		
	/*	hex_dump("Peer RSNIE", pRsnIe, peerRsnIeLen); */
	eid_ptr = (PEID_STRUCT)pRsnIe;

	if (pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPA2PSK ||
		pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPA2
#ifdef WPA3_SUPPORT
		|| (pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
		)
		RsnEid = IE_WPA2;
	
	if (eid_ptr->Eid != IE_WPA && eid_ptr->Eid != IE_WPA2)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE has unknown element ID(%d) \n", eid_ptr->Eid));
	    return FALSE;
	}

	if (eid_ptr->Len < sizeof(RSNIE2))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, its length is too short(%d)\n", eid_ptr->Len));
	    return FALSE;
	}

	*PureRsnLen = 0;	
	*PmkIdLen = 0;

	/* Store STA RSN_IE capability */
	pBufTmp = (PUCHAR)&eid_ptr->Octet[0];

#ifdef RELEASE_EXCLUDE
	/* the offset of group cipher */
	/* If WPA2, skip version(2-bytes) */
	/* If WPA,  skip OUI(4-bytes) and version(2-bytes) */
#endif /* RELEASE_EXCLUDE */
	offset = (eid_ptr->Eid == IE_WPA2) ? 2 : 6;
	
	/* Check group cipher */
	if (MeshCheckGroupCipher(pAd, pBufTmp + offset, RsnEid))
	{
		/* skip group cipher, then point to pairwise-cipher-count */
		offset += 4;	
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the group cipher isn't supported\n"));
		return FALSE;
	}
	
	/* Store pairwise-cipher-count */
    NdisMoveMemory(&Count, pBufTmp + offset, sizeof(USHORT));
#ifdef RT_BIG_ENDIAN
    Count = SWAP16(Count);		
#endif

	if (Count > 0)
	{
		/* skip pairwise-cipher-count, then point to pairwise-cipher-suite-list */
		offset += sizeof(USHORT);		    	
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the count of piarwise-cipher is invalid(%d)\n", Count));
		return FALSE;
	}

	/* clear this flag */
	isValid = FALSE;
	while (Count > 0)
	{
		if ((isValid == FALSE) && (MeshCheckPairwiseCipher(pAd, pBufTmp + offset, RsnEid)))
		{
			isValid = TRUE;			
		}

		offset += 4;
		Count--;
	}
        	
    if (!isValid)    
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the pairwise cipher isn't supported\n"));
		return FALSE;
	}

	/* Store AKM-suite-count */
    NdisMoveMemory(&Count, pBufTmp + offset, sizeof(USHORT));
#ifdef RT_BIG_ENDIAN
    Count = SWAP16(Count);		
#endif
	
	if (Count > 0)
	{
		/* skip AKM-suite-count, then pointer to AKM-suite-list */
		offset += sizeof(USHORT);		
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the count of AKM-suite is invalid(%d)\n", Count));
		return FALSE;
	}

	/* clear this flag */
	isValid = FALSE;
	while (Count > 0)
	{
		if ((isValid == FALSE) && (MeshCheckAKMSuite(pAd, pBufTmp + offset, RsnEid)))
		{
			isValid = TRUE;			
		}

		offset += 4;
		Count--;
	}
        	
    if (!isValid)    
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MeshValidateRSNIE, the AKM suite isn't supported\n"));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("MeshValidateRSNIE is completed \n"));

	/* Check if the remaining length is larger than the size of RSN capability(2-bytes). */
	if (eid_ptr->Len - offset >= 2)
	{
		/* skip RSN capability(2-bytes) */
		offset += sizeof(USHORT);		
	}	

	*PureRsnLen = offset;	/* the length is the RSNIE except elementID, length and PMKID */

#ifdef RELEASE_EXCLUDE
	/* decide PMKID length */
	/* If PMKID exists, the remaining length shall be */
	/* 18 = PMKID_count(2-bytes) + one PMKID(16-bytes) or */
	/* 34 = PMKID_count(2-bytes) + two PMKIDs(32-bytes) */
#endif /* RELEASE_EXCLUDE */
	if (eid_ptr->Len - offset >= 2)
	{
		NdisMoveMemory(&Count, pBufTmp + offset, sizeof(USHORT));
#ifdef RT_BIG_ENDIAN
    	Count = SWAP16(Count);		
#endif
		offset += sizeof(USHORT);	

		if (Count == 2 && (eid_ptr->Len - offset == (2*LEN_PMKID)))		
			*PmkIdLen = 2*LEN_PMKID;		
		else if (Count == 1 && (eid_ptr->Len - offset == LEN_PMKID))					
			*PmkIdLen = LEN_PMKID;		
		else
			*PmkIdLen = 0;	

		if (*PmkIdLen > 0)
		{		
			DBGPRINT(RT_DEBUG_TRACE, ("MeshValidateRSNIE, %d PMKID exist and its len is %d \n", Count, (eid_ptr->Len - offset)));
		}
	}
						
    return TRUE;
}

/*
    ==========================================================================
    Description:
       Check sanity of group cipher selector
    Return:
         TRUE if match
         FALSE otherwise
    ==========================================================================
*/
BOOLEAN MeshCheckGroupCipher(
    IN PRTMP_ADAPTER    pAd,
    IN PUCHAR			pData,
    IN UCHAR			Eid)
{	

	/* WPA and WPA2 format not the same in RSN_IE */
	if (Eid == IE_WPA2)
	{
		/* skip version(2-bytes) */
		if (NdisEqualMemory(pData, &pAd->MeshTab.RSN_IE[2], 4))
			return TRUE;
	}
	else if (Eid == IE_WPA)
	{
		/* skip OUI(4-bytes) and version(2-bytes) */
		if (NdisEqualMemory(pData, &pAd->MeshTab.RSN_IE[6], 4))
			return TRUE;
	}	        

    return FALSE;
}

/*
    ==========================================================================
    Description:
       Check sanity of unicast cipher selector.
    Return:
         TRUE if match
         FALSE otherwise
    ==========================================================================
*/
BOOLEAN MeshCheckPairwiseCipher(
    IN PRTMP_ADAPTER    pAd,
    IN PUCHAR			pData,
    IN UCHAR			Eid)
{
	PUCHAR pTmp;
	USHORT Count;

	pTmp = &pAd->MeshTab.RSN_IE[0];

	if(Eid == IE_WPA2)
    /* skip Version(2),Multicast cipter(4) 2+4==6 */
        pTmp += 6;
    else	
    /*skip OUI(4),Vesrion(2),Multicast cipher(4) 4+2+4==10 */
        pTmp += 10;/*point to number of unicast */
		
    NdisMoveMemory(&Count, pTmp, sizeof(USHORT));	
#ifdef RT_BIG_ENDIAN
    Count = SWAP16(Count);		
#endif
    pTmp   += sizeof(USHORT);/*pointer to unicast cipher */
   
    while (Count > 0)
    {		
		if(RTMPEqualMemory(pData, pTmp, 4))
		   return TRUE;
    	else
		{
			pTmp += 4;
			Count--;
		}
    }
    return FALSE;/* do not match the unicast cipher */

}



/*
    ==========================================================================
    Description:
       Check invalidity of authentication method selection.
    Return:
         TRUE if match
         FALSE otherwise
    ==========================================================================
*/
BOOLEAN MeshCheckAKMSuite(
    IN PRTMP_ADAPTER    pAd,
    IN PUCHAR			pData,
    IN UCHAR			Eid)
{
	PUCHAR pTmp;
	USHORT Count;

	pTmp = &pAd->MeshTab.RSN_IE[0];

	if(Eid == IE_WPA2)
    /* skip Version(2),Multicast cipter(4) 2+4==6 */
        pTmp +=6;
    else	
    /*skip OUI(4),Vesrion(2),Multicast cipher(4) 4+2+4==10 */
        pTmp += 10;/*point to number of unicast */
	    
    NdisMoveMemory(&Count, pTmp, sizeof(USHORT));	
#ifdef RT_BIG_ENDIAN
    Count = SWAP16(Count);		
#endif
    pTmp   += sizeof(USHORT);/*pointer to unicast cipher */

    /* Skip all unicast cipher suite */
    while (Count > 0)
    	{
		/* Skip OUI */
		pTmp += 4;
		Count--;
	}

	NdisMoveMemory(&Count, pTmp, sizeof(USHORT));
#ifdef RT_BIG_ENDIAN
    Count = SWAP16(Count);		
#endif
    pTmp   += sizeof(USHORT);/*pointer to AKM cipher */
    
    while (Count > 0)
    {		
		if(RTMPEqualMemory(pData, pTmp, 4))
		   return TRUE;
    	else
		{
			pTmp += 4;
			Count--;
		}
    }
    return FALSE;/* do not match the AKM */

}

UINT16	MeshValidateOpenAndCfnPeerLinkMsg(
	IN PRTMP_ADAPTER    	pAd,
	IN UCHAR				state,
	IN PMESH_LINK_ENTRY		pMeshLinkEntry,
	IN PUCHAR				pRcvdMscIe,
	IN PUCHAR				pRcvdMsaIe,
	IN UCHAR				RcvdMsaIeLen,
	IN PUCHAR				pRcvdRsnIe,
	IN UCHAR				RcvdRsnIeLen,
	IN UCHAR				pure_rsn_len,
	IN UCHAR				pmkid_len)
{			
#ifdef RELEASE_EXCLUDE
	/* MSCIE is identical to the MSCIE included in the peer link */
	/* open (or confirm) frame received from the candidate peer MP. */
#endif /* RELEASE_EXCLUDE */
	if (!RTMPEqualMemory(pRcvdMscIe, (PUCHAR)&pMeshLinkEntry->RcvdMscIe, sizeof(MESH_SECURITY_CAPABILITY_IE)))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("The MSCIE is different between peer link open and confirm frame \n"));		
		return MESH_SECURITY_FAILED_VERIFICATION;		
	}

#ifdef RELEASE_EXCLUDE
	/* The Handshake Control field of MSAIE is identical to that */
	/* included in the received peer link open (or confirm) frame. */
#endif /* RELEASE_EXCLUDE */
	if (!RTMPEqualMemory(pRcvdMsaIe, pMeshLinkEntry->RcvdMsaIe, sizeof(MESH_HANDSHAKE_CONTROL)))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("The Handshake Control field of MSAIE is different between peer link open and confirm frame \n"));		
		return MESH_SECURITY_FAILED_VERIFICATION;		
	}	

	if (pAd->MeshTab.wdev.AuthMode >= Ndis802_11AuthModeWPA)
	{						
#ifdef RELEASE_EXCLUDE
		/* RSNIE is identical to the RSNIE included in the peer link */
		/* confirm frame received from the candidate peer MP, except the PMKID list. */
#endif /* RELEASE_EXCLUDE */
		if (pure_rsn_len != pMeshLinkEntry->RcvdRSNIE_Len || 
			!RTMPEqualMemory(pRcvdRsnIe + 2, pMeshLinkEntry->RcvdRSNIE, pure_rsn_len))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("The RSNIE is different between candidate MP's link open and confirm frame \n"));			
			return MESH_SECURITY_FAILED_VERIFICATION;			
		}	
				
#ifdef RELEASE_EXCLUDE
		/* the local MP shall verify that the PMK-MAName value contained in the received */
		/* peer link confirm frame identifies the key chosen by the key selection procedure, */
		/* or is empty if Initial MSA Authentication shall occur. */
#endif /* RELEASE_EXCLUDE */
		if (pmkid_len > 0 && pMeshLinkEntry->RcvdPMKID_Len > 0)
		{
			UCHAR pmkid_offset = RcvdRsnIeLen - pmkid_len;
								
			if (pmkid_len != pMeshLinkEntry->RcvdPMKID_Len ||
				!NdisEqualMemory(pRcvdRsnIe + pmkid_offset, pMeshLinkEntry->RcvdPMKID, pmkid_len))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The result of Key Selection is different between candidate MP's link open and confirm frame\n"));	
				hex_dump("Receive PMKID ", pRcvdRsnIe + pmkid_offset, pmkid_len);
				hex_dump("Desired PMKID ", pMeshLinkEntry->RcvdPMKID, pMeshLinkEntry->RcvdPMKID_Len);
				return MESH_SECURITY_FAILED_VERIFICATION;
				
			}	
		}
		else if (pmkid_len == 0 && pMeshLinkEntry->RcvdPMKID_Len == 0)
		{
			;
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("The PMKID is different between candidate MP's link open and confirm frame\n"));	
			return MESH_SECURITY_FAILED_VERIFICATION;
		}		

#ifdef RELEASE_EXCLUDE
		/* If the candidate peer MP is the selector MP, the values in the Selected AKM Suite and */
		/* Selected Pairwise Cipher Suite fields are identical to the values received in peer link frame. */
#endif /* RELEASE_EXCLUDE */
		if (!pMeshLinkEntry->bValidLocalMpAsSelector)
		{
			UCHAR	akm_offset = 0, cipher_offset = 0;
			
#ifdef RELEASE_EXCLUDE
			/* the Selected AKM offset - */
			/* skip the Handshake Control field(1) + MA_ID(6) */
#endif /* RELEASE_EXCLUDE */
			akm_offset = sizeof(MESH_HANDSHAKE_CONTROL) + MAC_ADDR_LEN;

#ifdef RELEASE_EXCLUDE
			/* the Selected Pairwise-cipher offset - */
			/* skip the Handshake Control field(1) + MA_ID(6) + Selected_AKM(4) */
#endif /* RELEASE_EXCLUDE */
			cipher_offset = sizeof(MESH_HANDSHAKE_CONTROL) + MAC_ADDR_LEN + LEN_OUI_SUITE;
			
			if (!RTMPEqualMemory(pRcvdMsaIe + akm_offset, &pMeshLinkEntry->RcvdMsaIe[akm_offset], LEN_OUI_SUITE))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The Selected AKM Suite of MSAIE is different between peer link open and confirm frame \n"));				
				return MESH_SECURITY_FAILED_VERIFICATION;				
			}	
			
			if (!RTMPEqualMemory(pRcvdMsaIe + cipher_offset, &pMeshLinkEntry->RcvdMsaIe[cipher_offset], LEN_OUI_SUITE))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The Selected Pairwise Cipher Suite of MSAIE is different between peer link open and confirm frame \n"));				
				return MESH_SECURITY_FAILED_VERIFICATION;				
			}	
		}	
		
	}
		
#ifdef RELEASE_EXCLUDE
	/* the local MP shall verify that the MA-ID value received in the peer link confirm */
	/* frame matches the result of the 802.1X role selection procedure. */
#endif /* RELEASE_EXCLUDE */
	{
		UCHAR LocalMaAddr[MAC_ADDR_LEN];
		UCHAR PeerMaAddr[MAC_ADDR_LEN];
			
		if (pMeshLinkEntry->bValidLocalMpAsAuthenticator)
			NdisMoveMemory(LocalMaAddr, pAd->MeshTab.wdev.if_addr, MAC_ADDR_LEN);
		else
			NdisMoveMemory(LocalMaAddr, pMeshLinkEntry->PeerMacAddr, MAC_ADDR_LEN);

		if (state == SUBTYPE_PEER_LINK_CONFIRM)		
			NdisMoveMemory(PeerMaAddr, pRcvdMsaIe + 1, MAC_ADDR_LEN);
		else
			NdisMoveMemory(PeerMaAddr, &pMeshLinkEntry->RcvdMsaIe[1], MAC_ADDR_LEN);
							
		if (!NdisEqualMemory(LocalMaAddr, PeerMaAddr, MAC_ADDR_LEN))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("The MA-ID of MSAIE doesn't match \n"));			
			return MESH_SECURITY_FAILED_VERIFICATION;			
		}
	}
	

	return MLME_SUCCESS;
}

UINT16 MeshCheckPeerMsaIeCipherValidity(
	IN PRTMP_ADAPTER    	pAd,
	IN UCHAR				state,
	IN PMESH_LINK_ENTRY		pMeshLinkEntry,
	IN PUCHAR				pRcvdMsaIe)
{
	if (pAd->MeshTab.wdev.AuthMode >= Ndis802_11AuthModeWPA)
	{			
		UCHAR	akm_offset = 0, cipher_offset = 0;
				
#ifdef RELEASE_EXCLUDE
		/* the Selected AKM offset - */
		/* skip the Handshake Control field(1) + MA_ID(6) */
#endif /* RELEASE_EXCLUDE */
		akm_offset = sizeof(MESH_HANDSHAKE_CONTROL) + MAC_ADDR_LEN;

#ifdef RELEASE_EXCLUDE
		/* the Selected Pairwise-cipher offset - */
		/* skip the Handshake Control field(1) + MA_ID(6) + Selected_AKM(4) */
#endif /* RELEASE_EXCLUDE */
		cipher_offset = akm_offset + LEN_OUI_SUITE;
	
#ifdef RELEASE_EXCLUDE
		/* Verify that the AKM suite and pairwise cipher suite selected */
		/* in the MSAIE are among those supported by the local MP if */
		/* 1. the local MP is not the Selector MP and received peer link open frame */
		/* 2. received peer link confirm frame */
#endif /* RELEASE_EXCLUDE */
		if (((state == SUBTYPE_PEER_LINK_OPEN) && (!pMeshLinkEntry->bValidLocalMpAsSelector)) ||
			 (state == SUBTYPE_PEER_LINK_CONFIRM))
		{
			UCHAR	 RsnEid = IE_WPA;
						
			if (pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPA2PSK ||
				pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPA2
#ifdef WPA3_SUPPORT
				|| (pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
				)
				RsnEid = IE_WPA2;
			
			if (!MeshCheckPairwiseCipher(pAd, &pRcvdMsaIe[cipher_offset], RsnEid))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The Selected Pairwise Cipher isn't supported by local MP\n"));				
				return MESH_SECURITY_ROLE_NEGOTIATION_DIFFERS;				
			}

			if (!MeshCheckAKMSuite(pAd, &pRcvdMsaIe[akm_offset], RsnEid))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The Selected AKM isn't supported by local MP \n"));				
				return MESH_SECURITY_ROLE_NEGOTIATION_DIFFERS;				
			}	

			/* verify peer AKM and pairwise-cipher OK, update them to local MSAIE */
			if (state == SUBTYPE_PEER_LINK_OPEN)			
				NdisMoveMemory(&pMeshLinkEntry->LocalMsaIe[akm_offset], &pRcvdMsaIe[akm_offset], 2*LEN_OUI_SUITE);

		}	

#ifdef RELEASE_EXCLUDE
		/* Upon reception of a peer link confirm frame, */
		/* If the local MP is the selector MP, it shall verify that the selected AKM suite and pairwise cipher */
		/* suite values match its selections that have been sent to the candidate peer MP in the peer link open */
		/* frame. */
#endif /* RELEASE_EXCLUDE */
		if ((state == SUBTYPE_PEER_LINK_CONFIRM) && (pMeshLinkEntry->bValidLocalMpAsSelector))
		{											
			if (!NdisEqualMemory(&pRcvdMsaIe[akm_offset], &pMeshLinkEntry->LocalMsaIe[akm_offset], LEN_OUI_SUITE))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The Selected AKM in P.L. confirm frame isn't supported by local Selector MP \n"));						
				return MESH_SECURITY_FAILED_VERIFICATION;				
			}

			if (!NdisEqualMemory(&pRcvdMsaIe[cipher_offset], &pMeshLinkEntry->LocalMsaIe[cipher_offset], LEN_OUI_SUITE))				
			{
				DBGPRINT(RT_DEBUG_ERROR, ("The Selected Pairwise Cipher in P.L. confirm frame isn't supported by local Selector MP \n"));										
				return MESH_SECURITY_FAILED_VERIFICATION;			
			}	
		}
	}
							
	return MLME_SUCCESS;

}

BOOLEAN MeshPathRequestSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID		*pMsg,
		IN ULONG		MsgLen,
		OUT UINT8	*pFlag,
		OUT UINT8 	*pHopCount,
		OUT UINT8	*pTTL,
		OUT UINT32	*pID,
		OUT PUCHAR	pOrigMac,
		OUT UINT32	*pOrigDsn,
		OUT PUCHAR	pProxyMac,
		OUT UINT32	*pLifeTime,
		OUT UINT32	*pMetric,
		OUT UINT8	*pDestCount,
		OUT PMESH_DEST_ENTRY	pDestEntry)
{
	PEID_STRUCT eid_ptr;
	UINT8	VarOffset = 0;
	UINT8	EntryCount = 0;
	MESH_PREQ_FLAG PreqFlag;

	eid_ptr = (PEID_STRUCT)pMsg;

	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_PREQ:
				{
					NdisMoveMemory(pFlag, eid_ptr->Octet, 1);
					VarOffset += 1;
					NdisMoveMemory(pHopCount, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					NdisMoveMemory(pTTL, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					NdisMoveMemory(pID, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pID = le2cpu32(*pID);
					NdisMoveMemory(pOrigMac, eid_ptr->Octet + VarOffset, MAC_ADDR_LEN);
					VarOffset += MAC_ADDR_LEN;
					NdisMoveMemory(pOrigDsn, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pOrigDsn = le2cpu32(*pOrigDsn);
					PreqFlag = (MESH_PREQ_FLAG)(*pFlag);
					if (PreqFlag.field.AE == 1)
					{
						NdisMoveMemory(pProxyMac, eid_ptr->Octet + VarOffset, MAC_ADDR_LEN);
						VarOffset += MAC_ADDR_LEN;
					}
					NdisMoveMemory(pLifeTime, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pLifeTime = le2cpu32(*pLifeTime);
					NdisMoveMemory(pMetric, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pMetric = le2cpu32(*pMetric);
					NdisMoveMemory(pDestCount, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					EntryCount = (*pDestCount);
					if (EntryCount >= 1)
						NdisMoveMemory(pDestEntry, eid_ptr->Octet + VarOffset, 11*EntryCount);
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

BOOLEAN MeshPathResponseSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID		*pMsg,
		IN ULONG		MsgLen,
		OUT UINT8	*pFlag,
		OUT UINT8 	*pHopCount,
		OUT UINT8	*pTTL,
		OUT PUCHAR	pDestMac,
		OUT UINT32	*pDesDsn,
		OUT PUCHAR	pProxyMac,
		OUT UINT32	*pLifeTime,
		OUT UINT32	*pMetric,
		OUT PUCHAR	pOrigMac,
		OUT UINT32	*pOrigDsn,
		OUT UINT8	*pDependMPCount,
		OUT PMESH_DEPENDENT_ENTRY	pDependEntry)
{
	PEID_STRUCT eid_ptr;
	UINT8	VarOffset = 0;
	UINT8	EntryCount = 0;
	MESH_PREP_FLAG PrepFlag;

	eid_ptr = (PEID_STRUCT)pMsg;

	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_PREP:
				{
					NdisMoveMemory(pFlag, eid_ptr->Octet, 1);
					VarOffset += 1;
					NdisMoveMemory(pHopCount, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					NdisMoveMemory(pTTL, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					NdisMoveMemory(pDestMac, eid_ptr->Octet + VarOffset, MAC_ADDR_LEN);
					VarOffset += MAC_ADDR_LEN;
					NdisMoveMemory(pDesDsn, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pDesDsn = le2cpu32(*pDesDsn);
					PrepFlag = (MESH_PREP_FLAG)(*pFlag);
					if (PrepFlag.field.AE == 1)
					{
						NdisMoveMemory(pProxyMac, eid_ptr->Octet + VarOffset, MAC_ADDR_LEN);
						VarOffset += MAC_ADDR_LEN;
					}
					NdisMoveMemory(pLifeTime, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pLifeTime = le2cpu32(*pLifeTime);
					NdisMoveMemory(pMetric, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pMetric = le2cpu32(*pMetric);
					NdisMoveMemory(pOrigMac, eid_ptr->Octet + VarOffset, MAC_ADDR_LEN);
					VarOffset += MAC_ADDR_LEN;
					NdisMoveMemory(pOrigDsn, eid_ptr->Octet + VarOffset, 4);
					VarOffset += 4;
					*pOrigDsn = le2cpu32(*pOrigDsn);
					NdisMoveMemory(pDependMPCount, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					EntryCount = (*pDependMPCount);
					if (EntryCount >= 1)
						NdisMoveMemory(pDependEntry, eid_ptr->Octet + VarOffset, 10*EntryCount);
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

BOOLEAN MeshPathErrorSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID		*pMsg,
		IN ULONG		MsgLen,
		OUT UINT8	*pFlag,
		OUT UINT8	*pDestNum,
		OUT PMESH_PERR_ENTRY	pErrorEntry)
{
	PEID_STRUCT eid_ptr;
	UINT8	VarOffset = 0;
	UINT8	EntryCount = 0;

	eid_ptr = (PEID_STRUCT)pMsg;

	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_PREP:
				{
					NdisMoveMemory(pFlag, eid_ptr->Octet, 1);
					VarOffset += 1;
					NdisMoveMemory(pDestNum, eid_ptr->Octet + VarOffset, 1);
					VarOffset += 1;
					EntryCount = (*pDestNum);
					if (EntryCount >= 1)
						NdisMoveMemory(pErrorEntry, eid_ptr->Octet + VarOffset, 10*EntryCount);
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

BOOLEAN MeshLinkMetricReportSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT PUINT32 pLinkMetric)
{
	PEID_STRUCT eid_ptr;
	UINT32 LinkMetric;

	eid_ptr = (PEID_STRUCT)pMsg;

	while (((UCHAR*)eid_ptr + eid_ptr->Len + 1) < ((PUCHAR)pMsg + MsgLen))
	{
		switch(eid_ptr->Eid)
		{
			case IE_MESH_LINK_METRIC_REPORT:
				{
					if (eid_ptr->Len == 4)
					{
						NdisMoveMemory(&LinkMetric, eid_ptr->Octet, eid_ptr->Len);
						*pLinkMetric = le2cpu32(LinkMetric);
					}
				}
				break;

			default:
				break;
		}
		eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
	}

	return TRUE;
}

#endif /* MESH_SUPPORT */
