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
	mesh_tlv.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-06-25      For mesh (802.11s) support.
*/
#ifdef MESH_SUPPORT


#include "rt_config.h"

/* Mesh IEs */
UCHAR MeshIdIE = IE_MESH_ID;
UCHAR MeshConfigurationIE = IE_MESH_CONFIGURATION;
UCHAR MeshLinkMetricReportIE = IE_MESH_LINK_METRIC_REPORT;
UCHAR MeshChSwitchAnnounIE = IE_MESH_CHANNEL_SWITCH_ANNOUNCEMENT;
UCHAR MeshPeerLinkMngIE = IE_MESH_PEER_LINK_MANAGEMENT;
UCHAR VendorSpecificIE = IE_VENDOR_SPECIFIC;
UCHAR MeshMSCIE	= IE_MESH_MSCIE;
UCHAR MeshMSAIE = IE_MESH_MSAIE;

/* Mesh OUIs */
UCHAR MeshOUI[3] = {0x00, 0x0f, 0xac};

UINT8 LenPeerLinkMngIE[3] = {
	3,	/* Peer Link Open */
	5,	/* Peer Link confirm */
	7,	/* Peer Link close */
};

VOID MeshHeaderInit(
    IN PRTMP_ADAPTER pAd,
    OUT PHEADER_802_11 pMeshHdr, 
    IN PUCHAR Addr1, 
    IN PUCHAR Addr2,
    IN PUCHAR Addr3)
{
	NdisZeroMemory(pMeshHdr, sizeof(HEADER_802_11));
	
	pMeshHdr->FC.Type = BTYPE_MESH;
	pMeshHdr->FC.SubType = SUBTYPE_MULTIHOP;
	pMeshHdr->FC.FrDs = 0;
	pMeshHdr->FC.ToDs = 0;

	COPY_MAC_ADDR(pMeshHdr->Addr1, Addr1);
	COPY_MAC_ADDR(pMeshHdr->Addr2, Addr2);
	COPY_MAC_ADDR(pMeshHdr->Addr3, Addr3);

	return;
}

VOID InsertMeshHeader(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 MeshFlag,
	IN UINT8 TTL,
	IN UINT32 MeshSeq,
	IN PUCHAR Addr4,
	IN PUCHAR Addr5,
	IN PUCHAR Addr6)
{
	ULONG TempLen;
	PMESH_FLAG pMeshFlagBuf = (PMESH_FLAG)&MeshFlag;
	UINT32 MeshSeqBuf;

	MeshSeqBuf = cpu2le32(MeshSeq);
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,				&MeshFlag,
						1,				&TTL,
						4,				&MeshSeqBuf,
						END_OF_ARGS);
	*pFrameLen = *pFrameLen + TempLen;
	pFrameBuf += TempLen;

	if (Addr4 && (pMeshFlagBuf->field.AE & 0x01))
	{
		MakeOutgoingFrame(	pFrameBuf,		&TempLen,
							MAC_ADDR_LEN,				Addr4,
							END_OF_ARGS);
		*pFrameLen = *pFrameLen + TempLen;
		pFrameBuf += TempLen;
	}

	if (Addr5 && Addr6 && (pMeshFlagBuf->field.AE & 0x02))
	{
		MakeOutgoingFrame(	pFrameBuf,		&TempLen,
							MAC_ADDR_LEN,				Addr5,
							MAC_ADDR_LEN,				Addr6,
							END_OF_ARGS);
		*pFrameLen = *pFrameLen + TempLen;
	}

	return;
}

VOID InsertMeshActField(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 Category,
	IN UINT8 ActCode)
{
	ULONG TempLen;

	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,				&Category,
						1,				&ActCode,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertCapabilityInfo(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT16 CapabilityInfo)
{
	ULONG TempLen;

	CapabilityInfo = cpu2le16(CapabilityInfo);
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						2,				&CapabilityInfo,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertStatusCode(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT16 StatusCode)
{
	ULONG TempLen;

	StatusCode = cpu2le16(StatusCode);
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						2,				&StatusCode,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertReasonCode(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT16 ReasonCode)
{
	ULONG TempLen;

	ReasonCode = cpu2le16(ReasonCode);
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						2,				&ReasonCode,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertAID(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen, 
	IN UINT16 Aid)
{
	ULONG TempLen;

	Aid = cpu2le16(Aid);
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						2,				&Aid,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertSupRateIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen)
{
	ULONG TempLen;

	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,							&SupRateIe,
						1,							&pAd->CommonCfg.SupRateLen,
						pAd->CommonCfg.SupRateLen,	pAd->CommonCfg.SupRate,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertExtRateIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen)
{
	ULONG TempLen;

	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,							&ExtRateIe,
						1,							&pAd->CommonCfg.ExtRateLen,
						pAd->CommonCfg.ExtRateLen,	pAd->CommonCfg.ExtRate,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return;
}

VOID InsertMeshConfigurationIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN BOOLEAN AcptLink)
{
	ULONG TempLen;
	UCHAR Len = 19;
	UCHAR Ver = MESH_VERSION;
	MESH_CAPABILITY MeshCapInfo;
	UINT32 CPI = cpu2le32(pAd->MeshTab.CPI);

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	/*if(!MESH_ON(pAd)) */
	/*	return; */

	MeshCapInfo.word = cpu2le16(pAd->MeshTab.MeshCapability.word);
	/* Force accept the link when Peer-Link-Open/Peer-Link-Confirm. */
	if (AcptLink == TRUE)
		MeshCapInfo.field.AcceptPeerLinks = 1;

	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,				&MeshConfigurationIE,
						1,				&Len,
						1,				&Ver,
						3,				&MeshOUI,
						1,				(PUCHAR)&pAd->MeshTab.PathProtocolId,
						3,				&MeshOUI,
						1,				(PUCHAR)&pAd->MeshTab.PathMetricId,
						3,				&MeshOUI,
						1,				(PUCHAR)&pAd->MeshTab.ContgesionCtrlId,
						4,				(PUCHAR)&CPI,
						2,				(PUCHAR)&MeshCapInfo.word,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}


VOID InsertMeshIdIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen)
{
	ULONG TempLen;
	UCHAR MeshIdLen = pAd->MeshTab.MeshIdLen;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	/*if(!MESH_ON(pAd)) */
	/*	return; */

	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,				&MeshIdIE,
						1,				&MeshIdLen,
						MeshIdLen,		(PUCHAR)pAd->MeshTab.MeshId, 
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}


VOID InsertMSCIE(
	IN PRTMP_ADAPTER 	pAd,
	OUT PUCHAR 			pFrameBuf,
	OUT PULONG 			pFrameLen)
{
	ULONG TempLen;
	UCHAR MCSIE_Len = sizeof(MESH_SECURITY_CAPABILITY_IE);
	MESH_SECURITY_CONFIGURATION		MeshSecConfig;
	
	if (!pAd->MeshTab.wdev.if_dev)
		return;

	/*if(!MESH_ON(pAd)) */
	/*	return; */

	MeshSecConfig.word = 0;
	if (pAd->MeshTab.bKeyholderDone)
		MeshSecConfig.field.MeshAuthenticator 	= 1;
	if ((pAd->MeshTab.bKeyholderDone) && (pAd->MeshTab.bConnectedToMKD))
		MeshSecConfig.field.ConnectedToMKD 		= 1;
	MeshSecConfig.field.DefaultRole				= 1;

	pAd->MeshTab.LocalMSCIE.MeshSecurityConfig.word = MeshSecConfig.word;

	MakeOutgoingFrame(	pFrameBuf,		&TempLen,						
						1,				&MeshMSCIE,
						1,				&MCSIE_Len,
						MCSIE_Len,		&pAd->MeshTab.LocalMSCIE,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMSAIE(
	IN PRTMP_ADAPTER 	pAd,
	IN UCHAR			LinkIdx,
	IN UCHAR			state,
	OUT PUCHAR 			pFrameBuf,
	OUT PULONG 			pFrameLen)
{
	ULONG 	TempLen;				
	UCHAR	Buf[MESH_MAX_MSAIE_LEN];
	UCHAR	len;
		
	if (!pAd->MeshTab.wdev.if_dev)
		return;

	/*if(!MESH_ON(pAd)) */
	/*	return; */
	
	/* clear the local MP's MSAIE field */
	NdisZeroMemory(Buf, MESH_MAX_MSAIE_LEN);	
	len = 0;

	/* Copy content from Local MP's MSAIE */
	NdisMoveMemory(Buf, 
				   pAd->MeshTab.MeshLink[LinkIdx].Entry.LocalMsaIe, 
				   pAd->MeshTab.MeshLink[LinkIdx].Entry.LocalMsaIeLen);
	len += pAd->MeshTab.MeshLink[LinkIdx].Entry.LocalMsaIeLen;
										
	/* Zero this MA-ID field in peer link open frame */
	if (state == SUBTYPE_PEER_LINK_OPEN)
	{
		PMSA_HANDSHAKE_IE	pLocalMsaIE;

		pLocalMsaIE = (PMSA_HANDSHAKE_IE)Buf;	
		NdisZeroMemory(pLocalMsaIE->MA_ID, MAC_ADDR_LEN);			
	}
				
	/* Fill in Optional Parameter */
	if (!pAd->MeshTab.EasyMeshSecurity)
	{
		/* Optional Parameter - MKD-ID */
		if (state == SUBTYPE_PEER_LINK_CONFIRM)
		{



		}
		

		/* Optional Parameter - Key Holder Transport List */
		if (state == SUBTYPE_PEER_LINK_CONFIRM)
		{



		}
		

		/* Optional Parameter - PMK-MKDName */
		/* PMK-MKDName shall be present if the RSNIE in this message contains a PMKMAName */
		/* (sender) value in the PMKID list field in P.L. open frame. */
		if ((state == SUBTYPE_PEER_LINK_OPEN) && (pAd->MeshTab.PMKID_Len > 0))
		{


		}

		/* Optional Parameter - MKD-NAS_ID */
		if (state == SUBTYPE_PEER_LINK_CONFIRM)
		{



		}
		

	}

	
	MakeOutgoingFrame(	pFrameBuf,				&TempLen,						
						1,						&MeshMSAIE,
						1,						&len,
						len,					Buf,
						END_OF_ARGS);
	
	*pFrameLen = *pFrameLen + TempLen;

	return; 
}


VOID InsertRSNIE(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR			pAddr,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen)
{
	ULONG 	TempLen;	
	UCHAR	total_len;
	USHORT	count = 0;
	UCHAR	PmkMaName[LEN_PMKID];
	UCHAR	rsn_ie = Wpa2Ie;

	total_len = pAd->MeshTab.RSNIE_Len;
	
	if (pAd->MeshTab.wdev.AuthMode == Ndis802_11AuthModeWPANone)
		rsn_ie = WpaIe;

	/* If the local MP isn't an MKD and it had completed its Initial MSA, */
	/* insert the PMKMAName(sender) into PMKID list */
	/* PMK-MAName(sender), the identifier of the currently-valid PMK-MA belonging to the key */
	/* hierarchy created by the local MP during a prior Initial MSA Authentication, that may be used to */
	/* secure a link with the peer MP. This entry shall be omitted if no currently valid PMK-MA exists, */
	/* or if the local MP requests Initial MSA Authentication. */
	if (!(pAd->MeshTab.OpMode & MESH_MKD) && pAd->MeshTab.bInitialMsaDone && pAd->MeshTab.PMKID_Len == LEN_PMKID)
	{
		count = 1;
		total_len += (2+ LEN_PMKID);	/* PMKID-Count(2) + one-PMKID */
	}

	/* PMK-MAName(receiver), the identifier of a PMK-MA belonging to the key hierarchy created */
	/* by the peer MP during its Initial MSA Authentication. */
	/* This entry is included only if a PMKMAName(sender) is included, */
	/* and only if the MA function of the local MP has cached the identified */
	/* PMK-MA that may be used to secure a link with the peer MP. */
	if (count > 0 && (pAddr != NULL) && pAd->MeshTab.bKeyholderDone)
	{
		PMAC_TABLE_ENTRY pEntry = NULL;
		UCHAR	idx;
		
		if((pEntry = MeshTableLookup(pAd, pAddr, TRUE)) != NULL)
		{
			idx = pEntry->func_tb_idx;
			
			if ((pAd->MeshTab.MeshLink[idx].Entry.bInitialMsaLink == TRUE) && 
				(pAd->MeshTab.LocalMpInitialMsaLink != (idx + 1)))
			{
				count += 1;
				total_len += LEN_PMKID;
				NdisMoveMemory(PmkMaName, pAd->MeshTab.MeshLink[idx].Entry.PMK_MA_NAME, LEN_PMKID);
			}
		}
	}
	
	MakeOutgoingFrame(	pFrameBuf,					&TempLen,
						1,							&rsn_ie,
						1,							&total_len,
						pAd->MeshTab.RSNIE_Len,		pAd->MeshTab.RSN_IE,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	if (count == 2)
	{			
#ifdef RT_BIG_ENDIAN
		count = SWAP16(count);
#endif

		pFrameBuf += TempLen;
		
		MakeOutgoingFrame(	pFrameBuf,				&TempLen,
							2,						&count,
							LEN_PMKID,				pAd->MeshTab.PMKID,
							LEN_PMKID,				PmkMaName,
							END_OF_ARGS);
		*pFrameLen = *pFrameLen + TempLen;		

		
	}
	else if (count == 1)
	{
#ifdef RT_BIG_ENDIAN
		count = SWAP16(count);
#endif

		pFrameBuf += TempLen;
		
		MakeOutgoingFrame(	pFrameBuf,				&TempLen,
							2,						&count,
							LEN_PMKID,				pAd->MeshTab.PMKID,
							END_OF_ARGS);
		*pFrameLen = *pFrameLen + TempLen;		
	}


	return;
}

VOID InsertMeshPeerLinkMngIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 SubType,
	IN UINT16 LocalLinkId,
	IN UINT16 PeerLinkId,
	IN UINT16 ReasonCode)
{
	ULONG TempLen;
	UINT8 Len;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	Len = LenPeerLinkMngIE[SubType];
	LocalLinkId = cpu2le16(LocalLinkId);
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,				&MeshPeerLinkMngIE,
						1,				&Len,
						1,				&SubType,
						2,				(PUCHAR)&LocalLinkId,
						END_OF_ARGS);
	*pFrameLen = *pFrameLen + TempLen;
	pFrameBuf += TempLen;

	/* Peer Link Id is not presetn for the peer link open, */
	/* is present for the peer link confirm and may be present */
	/* for Peer Link Close. */
	if (SubType != SUBTYPE_PEER_LINK_OPEN)
	{
		PeerLinkId = cpu2le16(PeerLinkId);
		MakeOutgoingFrame(	pFrameBuf,		&TempLen,
							2,				(PUCHAR)&PeerLinkId,
							END_OF_ARGS);
		*pFrameLen = *pFrameLen + TempLen;
		pFrameBuf += TempLen;
	}

	/* reason code field is not resent for Peer Link Open and Peer Link Confirm. */
	if (SubType == SUBTYPE_PEER_LINK_CLOSE)
	{
		ReasonCode = cpu2le16(ReasonCode);
		MakeOutgoingFrame(	pFrameBuf,		&TempLen,
							2,				(PUCHAR)&ReasonCode,
							END_OF_ARGS);
		*pFrameLen = *pFrameLen + TempLen;
	}

	return; 
}

VOID InsertMeshPathRequestIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 MeshPreqFlag,
	IN UCHAR HopCount,
	IN UCHAR TTL,
	IN ULONG ReqID,
	IN PUCHAR pOrigAddr,
	IN ULONG OrigDsn,
	IN PUCHAR pOrigProxyAddr,
	IN ULONG LifeTime,
	IN ULONG Metrics,
	IN UCHAR DestCount,
	IN MESH_DEST_ENTRY MeshDestEntry)
{
	ULONG TempLen;
	UCHAR MeshPreqLen = 26;
	UCHAR MeshPathRequestIE = IE_MESH_PREQ;
	UCHAR ProxyAddrLen = 0;
	
	if (!pAd->MeshTab.wdev.if_dev)
		return;

	if(!MESH_ON(pAd))
		return;

	if (pOrigProxyAddr)
	{
		ProxyAddrLen = MAC_ADDR_LEN;
		MeshPreqLen += MAC_ADDR_LEN;
	}
	else
	{
		ProxyAddrLen = 0;
	}

	MeshPreqLen += (DestCount * 11);

	ReqID = cpu2le32(ReqID);
	OrigDsn = cpu2le32(OrigDsn);
	LifeTime = cpu2le32(LifeTime);
	Metrics = cpu2le32(Metrics);

	MakeOutgoingFrame(	pFrameBuf,			&TempLen,
						1,					&MeshPathRequestIE,
						1,					&MeshPreqLen,
						1,					&MeshPreqFlag,
						1,					&HopCount,
						1,					&TTL,
						4,					&ReqID,
						MAC_ADDR_LEN,		pOrigAddr,
						4,					&OrigDsn,
						ProxyAddrLen,		pOrigProxyAddr,
						4,					&LifeTime,
						4,					&Metrics,
						1,					&DestCount,
						11,					&MeshDestEntry, 
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshPathResponseIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 MeshPrepFlag,
	IN UCHAR HopCount,
	IN UCHAR TTL,
	IN PUCHAR pDestAddr,
	IN ULONG DestSeq,
	IN PUCHAR pDestProxyAddr,
	IN ULONG LifeTime,
	IN ULONG Metrics,
	IN PUCHAR pOrigAddr,
	IN ULONG OrigSeq)
{
	ULONG TempLen;
	UCHAR MeshPrepLen = 31;
	UCHAR MeshPathResponseIE = IE_MESH_PREP;
	UCHAR ProxyAddrLen = 0;
	

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	if(!MESH_ON(pAd))
		return;

	if (pDestProxyAddr)
	{
		ProxyAddrLen = MAC_ADDR_LEN;
		MeshPrepLen += MAC_ADDR_LEN;
	}
	else
	{
		ProxyAddrLen = 0;
	}

	DestSeq = cpu2le32(DestSeq);
	LifeTime = cpu2le32(LifeTime);
	Metrics = cpu2le32(Metrics);
	OrigSeq = cpu2le32(OrigSeq);

	MakeOutgoingFrame(	pFrameBuf,			&TempLen,
						1,					&MeshPathResponseIE,
						1,					&MeshPrepLen,
						1,					&MeshPrepFlag,
						1,					&HopCount,
						1,					&TTL,
						MAC_ADDR_LEN,		pDestAddr,
						4,					&DestSeq,
						ProxyAddrLen,		pDestProxyAddr,
						4,					&LifeTime,
						4,					&Metrics,
						MAC_ADDR_LEN,		pOrigAddr,
						4,					&OrigSeq,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshPathErrorIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UCHAR DestCount,
	IN PMESH_PERR_ENTRY pMeshPerrEntry)
{
	ULONG TempLen;
	UCHAR MeshPerrLen = 2;
	UCHAR MeshPathErrorIE = IE_MESH_PERR;
	UCHAR MeshPerrFlag = 0;
	
	if (!pAd->MeshTab.wdev.if_dev)
		return;

	if(!MESH_ON(pAd))
		return;

	MeshPerrLen += (DestCount * 10);

	MakeOutgoingFrame(	pFrameBuf,			&TempLen,
						1,					&MeshPathErrorIE,
						1,					&MeshPerrLen,
						1,					&MeshPerrFlag,
						1,					&DestCount,
						(10*DestCount),		pMeshPerrEntry, 
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshProxyUpdateIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 ProxyFlags,
	IN UINT8 ProxySeq,
	IN PUCHAR pProxyMPAddr,
	IN USHORT ProxiedCount,
	IN PUCHAR pMeshProxiedEntry)
{
	ULONG TempLen;
	UCHAR IeLen = 12;
	UCHAR MeshProxyUpdateIE = IE_MESH_PU;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	if(!MESH_ON(pAd))
		return;

	IeLen += (ProxiedCount * 6);

	MakeOutgoingFrame(	pFrameBuf,							&TempLen,
						1,									&MeshProxyUpdateIE,
						1,									&IeLen,
						1,									&ProxyFlags,
						1,									&ProxySeq,
						MAC_ADDR_LEN,						pProxyMPAddr,
						2,									&ProxiedCount,
						(MAC_ADDR_LEN*ProxiedCount),		pMeshProxiedEntry, 
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshProxyUpdateConfirmationIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 ProxyConfirmFlagFlags,
	IN UINT8 ProxySeq,
	IN PUCHAR pMeshProxiedEntry)
{
	ULONG TempLen;
	UCHAR IeLen = 10;
	UCHAR MeshProxyUpdateIE = IE_MESH_PU;
	UINT8 MeshProxyUpdateConfirmFlag = 0;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	if(!MESH_ON(pAd))
		return;

	MakeOutgoingFrame(	pFrameBuf,							&TempLen,
						1,									&MeshProxyUpdateIE,
						1,									&IeLen,
						1,									&MeshProxyUpdateConfirmFlag,
						1,									&ProxySeq,
						MAC_ADDR_LEN,						pMeshProxiedEntry, 
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshHostNameIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen)
{
	ULONG TempLen;
	UCHAR Len;
	UCHAR HostNameLen;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	HostNameLen = strlen((RTMP_STRING *) pAd->MeshTab.HostName);
	Len = sizeof(MeshOUI) + HostNameLen;
	MakeOutgoingFrame(	pFrameBuf,		&TempLen,
						1,				&VendorSpecificIE,
						1,				&Len,
						3,				&MeshOUI,
						HostNameLen,	(PUCHAR)&pAd->MeshTab.HostName,
						END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshCongestionNotofocationIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT	PULONG	pFrameLen,
	IN PUCHAR	pPeerMac,
	IN UCHAR	SwitchOn)
{
	ULONG TempLen;
	UCHAR Len = 7;
	UCHAR MeshMultiPathNotice = IE_MESH_MULITI_PATH_NOTICE_IE;
	UCHAR Flag;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	Flag = (SwitchOn == FALSE) ? 0x00 : 0x01;
	MakeOutgoingFrame(pFrameBuf,	&TempLen,
					1,				&MeshMultiPathNotice,
					1,				&Len,
					1,				&Flag,
					MAC_ADDR_LEN,	pPeerMac,
					END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertMeshChSwAnnIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT	PULONG	pFrameLen,
	IN UINT8 ChSwMode,
	IN UINT8 NewCh,
	IN UINT32 NewCPI,
	IN UINT8 ChSwCnt,
	IN PUCHAR pMeshSA)
{
	ULONG TempLen;
	UCHAR Len = 13;
	UCHAR MeshChSwAnnIE = IE_MESH_CHANNEL_SWITCH_ANNOUNCEMENT;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	NewCPI = cpu2le32(NewCPI);

	MakeOutgoingFrame(pFrameBuf,		&TempLen,
					1,				&MeshChSwAnnIE,
					1,				&Len,
					1,				&ChSwMode,
					1,				&NewCh,
					4,				&NewCPI,
					1,				&ChSwCnt,
					MAC_ADDR_LEN,	pMeshSA,
					END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

#ifdef DOT11_N_SUPPORT
VOID InsertHtCapIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT	PULONG	pFrameLen,
	IN PHT_CAPABILITY_IE pHtCapability)
{
		ULONG TempLen;
		UCHAR HtLen;

#ifdef RT_BIG_ENDIAN
		HT_CAPABILITY_IE HtCapabilityTmp;
#endif

		/* add HT Capability IE */
		HtLen = sizeof(HT_CAPABILITY_IE);
#ifndef RT_BIG_ENDIAN
		MakeOutgoingFrame(pFrameBuf,		&TempLen,
							1,				&HtCapIe,
							1,				&HtLen,
							HtLen,			&pAd->CommonCfg.HtCapability, 
							END_OF_ARGS);
#else
	NdisMoveMemory(&HtCapabilityTmp, &pAd->CommonCfg.HtCapability, HtLen);
					*(USHORT *)(&HtCapabilityTmp.HtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.HtCapInfo));
					*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo) = SWAP16(*(USHORT *)(&HtCapabilityTmp.ExtHtCapInfo));

	MakeOutgoingFrame(pFrameBuf,				&TempLen,
						1,						&HtCapIe,
						1,						&HtLen,
						HtLen,					&HtCapabilityTmp, 
						END_OF_ARGS);
#endif
	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

VOID InsertAddHtInfoIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT	PULONG	pFrameLen,
	IN PADD_HT_INFO_IE pAddHTInfo)
{
	ULONG TempLen;
	UCHAR HtLen;

#ifdef RT_BIG_ENDIAN
	ADD_HT_INFO_IE	addHTInfoTmp;
#endif

	HtLen = sizeof(ADD_HT_INFO_IE);

#ifndef RT_BIG_ENDIAN
	MakeOutgoingFrame(pFrameBuf,		&TempLen,
						1,				&AddHtInfoIe,
						1,				&HtLen,
						HtLen,			pAddHTInfo, 
						END_OF_ARGS);
#else
	NdisMoveMemory(&addHTInfoTmp, pAddHTInfo, HtLen);
					*(USHORT *)(&addHTInfoTmp.AddHtInfo2) = SWAP16(*(USHORT *)(&addHTInfoTmp.AddHtInfo2));
					*(USHORT *)(&addHTInfoTmp.AddHtInfo3) = SWAP16(*(USHORT *)(&addHTInfoTmp.AddHtInfo3));

	MakeOutgoingFrame(pFrameBuf,				&TempLen,
						1,						&AddHtInfoIe,
						1,						&HtLen,
						HtLen,					&addHTInfoTmp, 
						END_OF_ARGS);
#endif
	*pFrameLen = *pFrameLen + TempLen;

	return; 
}
#endif /* DOT11_N_SUPPORT */

VOID InsertLinkMetricReportIE(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT	PULONG	pFrameLen,
	IN UINT32 LinkMetric)
{
	ULONG TempLen;
	UCHAR Len = 4;
	UCHAR IEId = IE_MESH_LINK_METRIC_REPORT;
	UINT32 LinkMetricValue;

	if (!pAd->MeshTab.wdev.if_dev)
		return;

	LinkMetricValue = cpu2le32(LinkMetric);

	MakeOutgoingFrame(pFrameBuf,	&TempLen,
					1,				&IEId,
					1,				&Len,
					Len,			(PUCHAR)&LinkMetricValue,
					END_OF_ARGS);

	*pFrameLen = *pFrameLen + TempLen;

	return; 
}

#endif /* MESH_SUPPORT */

