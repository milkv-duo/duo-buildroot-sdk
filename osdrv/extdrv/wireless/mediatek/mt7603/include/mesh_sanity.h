/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2005, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attempt
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	mesh.h

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-07-23		To Support Mesh (802.11s) feature.
*/
#ifndef __MESH_SANITY_H__
#define __MESH_SANITY_H__

BOOLEAN MeshPeerBeaconAndProbeSanity(
	IN PRTMP_ADAPTER pAd,
	IN VOID *Msg,
	IN ULONG MsgLen,
	OUT PUCHAR pHostName,
	OUT PUCHAR pHostNameLen,
	OUT PUCHAR pMeshId,
	OUT PUCHAR pMeshIdLen,
	OUT PMESH_CONFIGURAION_IE pMeshConfiguration);

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
		OUT UCHAR *pHtCapabilityLen);

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
		OUT UCHAR *pHtCapabilityLen);

BOOLEAN MeshLinkMngClsSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT16 *pLocalLinkId,
		OUT UINT16 *pPeerLinkId,
		OUT UINT16 *pReasonCode);

BOOLEAN MeshPathSelMultipathNoticeSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT8 *pFlag,
		OUT PUCHAR pMeshSA);

BOOLEAN MeshChannelSwitchAnnouncementSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT UINT8 *pChSwMode,
		OUT UINT8 *pNewCh,
		OUT UINT32 *pNewCPI,
		OUT UINT8 *pChSwCnt,
		OUT PUCHAR pMeshSA);

BOOLEAN MeshValidateRSNIE(
    IN  PRTMP_ADAPTER    pAd,
    IN  PUCHAR			pRsnIe,
    IN	USHORT			peerRsnIeLen,
    OUT	UCHAR			*PureRsnLen,
    OUT	UCHAR			*PmkIdLen);

BOOLEAN MeshCheckGroupCipher(
    IN PRTMP_ADAPTER    pAd,
    IN PUCHAR			pData,
    IN UCHAR			Eid);

BOOLEAN MeshCheckPairwiseCipher(
    IN PRTMP_ADAPTER    pAd,
    IN PUCHAR			pData,
    IN UCHAR			Eid);

BOOLEAN MeshCheckAKMSuite(
    IN PRTMP_ADAPTER    pAd,
    IN PUCHAR			pData,
    IN UCHAR			Eid);

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
	IN UCHAR				pmkid_len);

UINT16 MeshCheckPeerMsaIeCipherValidity(
	IN PRTMP_ADAPTER    	pAd,
	IN UCHAR				state,
	IN PMESH_LINK_ENTRY		pMeshLinkEntry,
	IN PUCHAR				pRcvdMsaIe);

BOOLEAN MeshPathRequestSanity(
		IN PRTMP_ADAPTER	pAd,
		IN VOID			*pMsg,
		IN ULONG			MsgLen,
		OUT UINT8		*pFlag,
		OUT UINT8 		*pHopCount,
		OUT UINT8		*pTTL,
		OUT UINT32		*pID,
		OUT PUCHAR		pOrigMac,
		OUT UINT32		*pOrigDsn,
		OUT PUCHAR		pProxyMac,
		OUT UINT32		*pLifeTime,
		OUT UINT32		*pMetric,
		OUT UINT8		*pDestCount,
		OUT PMESH_DEST_ENTRY	pDestEntry);

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
		OUT PMESH_DEPENDENT_ENTRY	pDependEntry);

BOOLEAN MeshPathErrorSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID		*pMsg,
		IN ULONG		MsgLen,
		OUT UINT8	*pFlag,
		OUT UINT8	*pDestNum,
		OUT PMESH_PERR_ENTRY	pErrorEntry);

BOOLEAN MeshLinkMetricReportSanity(
		IN PRTMP_ADAPTER pAd,
		IN VOID *pMsg,
		IN ULONG MsgLen,
		OUT PUINT32 pLinkMetric);
#endif /* __MESH_SANITY_H__ */
