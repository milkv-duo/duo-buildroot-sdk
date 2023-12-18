/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	auto_provision

	Abstract:	
*/

#include "rt_config.h"

extern UCHAR   WPS_OUI[];

#ifdef EASY_CONFIG_SETUP
#ifdef CONFIG_AP_SUPPORT
VOID AutoProvisionBuildAssocRespIE(
	IN	PRTMP_ADAPTER	pAd,
	IN  UCHAR 			ApIdx,
	IN  UCHAR			Reason,
	OUT	PUCHAR			pOutBuf,
	OUT	PUCHAR			pIeLen)
{
	WSC_IE_HEADER 	ieHdr;
	UCHAR 			Data[512] = {0};
	PUCHAR			pData;
	INT				Len = 0, templen = 0;
	UINT8			tempVal = 0;
#ifdef EASY_CONFIG_SETUP
	UCHAR			ExtData[4] = {0};
#endif /* EASY_CONFIG_SETUP */
	UCHAR			Version = 0x10;
	USHORT			Tag = 0, TlvLen = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> WscBuildAssocRespIE\n"));

	/* WSC IE Header */
	ieHdr.elemId = 221;
	ieHdr.length = 4;
	ieHdr.oui[0] = 0x00; ieHdr.oui[1] = 0x50;
    ieHdr.oui[2] = 0xF2; ieHdr.oui[3] = 0x04;

	pData = (PUCHAR) &Data[0];
	Len = 0;
	
	/* Version, 1 byte */
	Tag = cpu2be16(0x104A);
	TlvLen = cpu2be16(1);
	NdisMoveMemory(pData, &Tag, 2);
	NdisMoveMemory(pData+2, &TlvLen, 2);
	NdisMoveMemory(pData+4, &Version, 1);
	templen = 5;
	pData += templen;
	Len   += templen;

	/* Request Type */
	tempVal = 0x03;
	Tag = cpu2be16(0x103B);
	TlvLen = cpu2be16(1);
	NdisMoveMemory(pData, &Tag, 2);
	NdisMoveMemory(pData+2, &TlvLen, 2);
	NdisMoveMemory(pData+4, &tempVal, 1);
	templen = 5;
	pData += templen;
	Len   += templen;

	/*
		Only main bssid supports auto provision.
	*/
	if ((pAd->ApCfg.MBSSID[MAIN_MBSSID].EasyConfigInfo.bEnable == TRUE) && 
		(ApIdx == MAIN_MBSSID))
	{
		/* WPS Vendor Extension */
		NdisMoveMemory(ExtData, RALINK_OUI, 3);
		ExtData[3] = Reason;
		Tag = cpu2be16(0x1049);
		TlvLen = cpu2be16(4);
		NdisMoveMemory(pData, &Tag, 2);
		NdisMoveMemory(pData+2, &TlvLen, 2);
		NdisMoveMemory(pData+4, &ExtData[0], 4);
		templen = 8;
		pData += templen;
		Len   += templen;
	}
     
	ieHdr.length = ieHdr.length + Len;
	NdisMoveMemory(pOutBuf, &ieHdr, sizeof(WSC_IE_HEADER));
	NdisMoveMemory(pOutBuf + sizeof(WSC_IE_HEADER), Data, Len);
	*pIeLen = sizeof(WSC_IE_HEADER) + Len;

	DBGPRINT(RT_DEBUG_TRACE, ("<----- WscBuildAssocRespIE\n"));
}


VOID 	AutoProvisionBuildAssocRspIE(
	IN  PRTMP_ADAPTER		pAd,
	IN  PMLME_QUEUE_ELEM	Elem,
	IN  PMAC_TABLE_ENTRY	pEntry,
	OUT PUCHAR				pOutBuffer,
	OUT PULONG				pFrameLen)
{
	if (pEntry && pEntry->bRaAutoWpsCapable)
	{
		UCHAR		*pWscBuf = NULL, WscIeLen = 0;
		ULONG 		WscTmpLen = 0;
		PEASY_CONFIG_INFO	pEasyConfig = &pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo;

		os_alloc_mem(NULL, (UCHAR **)&pWscBuf, 512);
		if (pWscBuf != NULL)
		{
			CHAR	Rssi = -80;
			Rssi = RTMPMaxRssi(pAd, 
					ConvertToRssi(pAd, &Elem->rssi_info, RSSI_IDX_0),
					ConvertToRssi(pAd, &Elem->rssi_info, RSSI_IDX_1),
					ConvertToRssi(pAd, &Elem->rssi_info, RSSI_IDX_2));

			NdisZeroMemory(pWscBuf, 512);

#ifdef WSC_AP_SUPPORT
			if ((pAd->ApCfg.MBSSID[pEntry->func_tb_idx].WscControl.WscConfMode != WSC_DISABLE) &&
				(MAC_ADDR_EQUAL(pAd->ApCfg.MBSSID[pEntry->func_tb_idx].WscControl.EntryAddr, pEntry->Addr)))
			{
				if (Rssi > pEasyConfig->RssiThreshold)
					WscBuildAssocRespIE(pAd, pEntry->func_tb_idx, 0, pWscBuf, &WscIeLen);
				else
				{
					pEntry->bRaAutoWpsCapable = FALSE;
					WscBuildAssocRespIE(pAd, pEntry->func_tb_idx, 1, pWscBuf, &WscIeLen);
				}
			}
			else 
#endif /* WSC_AP_SUPPORT */
			if (pEasyConfig->bEnable == TRUE)
			{
				if (Rssi > pEasyConfig->RssiThreshold)
					AutoProvisionBuildAssocRespIE(pAd, pEntry->func_tb_idx, 0, pWscBuf, &WscIeLen);
				else
				{
					pEntry->bRaAutoWpsCapable = FALSE;
					AutoProvisionBuildAssocRespIE(pAd, pEntry->func_tb_idx, 1, pWscBuf, &WscIeLen);
				}
			}

			MakeOutgoingFrame(pOutBuffer + (*pFrameLen), &WscTmpLen,
							  WscIeLen,					 pWscBuf,
							  END_OF_ARGS);

			(*pFrameLen) += WscTmpLen;
			os_free_mem(NULL, pWscBuf);
		}
		else
			DBGPRINT(RT_DEBUG_WARN, ("%s:: WscBuf Allocate failed!\n", __FUNCTION__));
	}
}

VOID AutoProvisionCheckEntry(
	IN RTMP_ADAPTER *pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN PEID_STRUCT eid_ptr)
{
#ifdef WAC_SUPPORT
	if (pEntry && pEntry->bSamsungAutoWpsCapable)
		return;
#endif // WAC_SUPPORT //

	if (pAd->ApCfg.MBSSID[MAIN_MBSSID].EasyConfigInfo.bEnable==TRUE)
	{
		if (pEntry && eid_ptr)
		{
			pEntry->bRaAutoWpsCapable = FALSE;
			if (pEntry->func_tb_idx == MAIN_MBSSID)
			{
				if (WpsMICIntegrity(pAd, eid_ptr->Octet, (INT)eid_ptr->Len, pEntry->Addr, pEntry->func_tb_idx))
				{
					pEntry->bRaAutoWpsCapable = TRUE;
				}
			}
		}
	}
}

VOID	AutoProvisionAssignSSID(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR				eid_data,
	IN  INT					eid_len,
	IN  PUCHAR				pAddr2,
	IN  UCHAR				apidx,
	OUT PUCHAR				SsidLen,
	OUT PUCHAR				Ssid)
{
	if (pAd->ApCfg.MBSSID[apidx].EasyConfigInfo.bEnable == FALSE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("MBSSID[%d] do not support Easy Config, skip the checking EC OUI\n",apidx));
		return;
	}
	if (WpsMICIntegrity(pAd, eid_data, eid_len, pAddr2, apidx))
	{
		*SsidLen = pAd->ApCfg.MBSSID[apidx].SsidLen;
		NdisMoveMemory(Ssid, pAd->ApCfg.MBSSID[apidx].Ssid, *SsidLen);
	}
}

BOOLEAN AutoProvisionCheckWscMsg(
    IN		PRTMP_ADAPTER	pAd,
    IN		PFRAME_802_11 	pFrame,
    INOUT	PUSHORT			pDataSize)
{
    /*int                 HeaderLen = LENGTH_802_1_H + sizeof(IEEE8021X_FRAME) + sizeof(EAP_FRAME);*/
	RTMP_STRING *pData;
	PEAP_FRAME  		pEapFrame;
	    
	/* Skip the EAP LLC header */
	pData = (RTMP_STRING *) (pFrame + LENGTH_802_1_H);
	if (pData)
		pEapFrame = (PEAP_FRAME)(pData + sizeof(IEEE8021X_FRAME));
	pData += sizeof(IEEE8021X_FRAME) + sizeof(EAP_FRAME);
        
	if ((pData != NULL) &&
		(pEapFrame->Code == EAP_CODE_RSP) &&
		(pEapFrame->Type == EAP_TYPE_WSC))
	{ 
		if (WscCheckWSCHeader((PUCHAR) pData))
		{
			/* EAP-Rsp (Messages) */
			pData += sizeof(WSC_FRAME);
			if (pData && (pData + 9))
			{
				UCHAR MsgType = *(pData + 9);
				if (MsgType == 0x04) /* M1 */
				{
					*pDataSize -= 19;
				}
			}
		}
	}
	return FALSE;
}

#endif /* CONFIG_AP_SUPPORT */

VOID	AutoProvisionGenWpsPTK(
	IN  PRTMP_ADAPTER		pAd,
	IN  UCHAR 			ApIdx)
{
	UCHAR	temp1[64];
	CHAR	temp2[6] = {'r', 'a', 'l', 'i', 'n', 'k'};
	PEASY_CONFIG_INFO	pEasyConfigInfo;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		pEasyConfigInfo = &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo;
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		pEasyConfigInfo = &pAd->StaCfg.EasyConfigInfo;
	}
#endif /* CONFIG_STA_SUPPORT */

	/* use proprietary WPS PTK */
	NdisZeroMemory(temp1, 64);
	NdisMoveMemory(temp1, "ralink_auto_provision", strlen("ralink_auto_provision"));

	NdisZeroMemory(pEasyConfigInfo->WpsPTK, 64);
	/* use proprietary PTK */
	WpaDerivePTK(pAd, temp1, temp1, temp2, temp1, temp2, pEasyConfigInfo->WpsPTK, LEN_PTK);
}

VOID	AutoProvisionDecodeExtData(
	IN  PRTMP_ADAPTER	pAd,
	IN  UCHAR 			ApIdx,
	IN  PUCHAR			pEntryAddr,
	IN  PUCHAR			pData)
{
#ifdef WSC_INCLUDED
	PWSC_CTRL			pWpsCtrl = NULL;
#endif /* WSC_INCLUDED */
	PMAC_TABLE_ENTRY	pEntry = NULL;
	PEASY_CONFIG_INFO	pEasyConfigInfo = NULL;	
	
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		pEasyConfigInfo = &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo;
#ifdef WSC_INCLUDED
		pWpsCtrl = &pAd->ApCfg.MBSSID[ApIdx].WscControl;
#endif /* WSC_INCLUDED */
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		pEasyConfigInfo = &pAd->StaCfg.EasyConfigInfo;
#ifdef WSC_INCLUDED
		pWpsCtrl = &pAd->StaCfg.WscControl;
#endif /* WSC_INCLUDED */
	}
#endif /* CONFIG_STA_SUPPORT */
	
	pEntry = MacTableLookup(pAd, pEntryAddr);
	if (pEntry && (pEntry->bRaAutoWpsCapable && !pEntry->bSamsungAutoWpsCapable))
	{
		CIPHER_KEY	CipherKey;
		CHAR 		EncryptedPIN[16] = {0}, PinStr[9] = {0};
		UINT16		PinLen = 12;
		UCHAR		iv_hdr[4];
		
		NdisZeroMemory(&CipherKey, sizeof(CIPHER_KEY));
		NdisMoveMemory(&CipherKey, pEasyConfigInfo->WpsPTK, 64);
		CipherKey.CipherAlg = CIPHER_WEP128;
		CipherKey.KeyLen = 13;
		NdisMoveMemory(&EncryptedPIN[4], pData+3, 12);						

		/* Construct the 4-bytes WEP IV header */
		RTMPConstructWEPIVHdr(1, 
			  				  CipherKey.TxTsc,
			 				  iv_hdr);
		
		NdisMoveMemory(&EncryptedPIN[0], &iv_hdr, 4);
		
		PinLen = 16;
		if (RTMPSoftDecryptWEP(pAd, &CipherKey, EncryptedPIN, &PinLen))
		{
			NdisMoveMemory(PinStr, &EncryptedPIN[0], 8);
			NdisMoveMemory(&(pEasyConfigInfo->WpsPinCode[0]), &EncryptedPIN[0], 8);			
#ifdef WSC_INCLUDED
			if ((pWpsCtrl->WscConfMode != WSC_DISABLE) && 
				(pEasyConfigInfo->bEnable))
			{
				pWpsCtrl->WscPinCode = simple_strtol(PinStr, 0, 10);
				pWpsCtrl->WscPinCodeLen = 8;
				/*
					To make sure of doing WPS process with WPS Client by PIN method
				*/
				pWpsCtrl->WscMode = WSC_PIN_MODE;
				WscGetRegDataPIN(pAd, pWpsCtrl->WscPinCode, pWpsCtrl);
			}
#endif /* WSC_INCLUDED */
		}
	}
}

BOOLEAN WpsMICIntegrity(
    IN  PRTMP_ADAPTER	pAd,
    IN  PUCHAR 			pData,
    IN  INT 			DataLen,
    IN	PUCHAR			pMacAddr,
    IN	UCHAR			ApIdx)
{
	BOOLEAN		bStatus = FALSE;
	PUCHAR 		pBuf = NULL;
	INT			BufLen = 0, Length = DataLen;
	UCHAR		*Ptr = pData + 4, *pWpsMIC = NULL, *pPeerWpsMIC = NULL;
	WSC_IE		*pWscIE;
	EASY_CONFIG_INFO	*pEasyConfigInfo = NULL;

	if (DataLen > 255)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: DataLen > 255\n", __FUNCTION__));
		return FALSE;
	}
	
	os_alloc_mem(NULL, (UCHAR **)&pBuf, 256);
	if (pBuf == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: pBuf alloc fail !!\n", __FUNCTION__));
		return FALSE;
	}

	pWpsMIC = pBuf + 200;
	pPeerWpsMIC = pBuf + 216;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		pEasyConfigInfo = &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo;
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		pEasyConfigInfo = &pAd->StaCfg.EasyConfigInfo;
	}
#endif /* CONFIG_STA_SUPPORT */

	NdisZeroMemory(pBuf, 256);		
	while (Length > 0)
	{
		WSC_IE	WscIE;
		NdisMoveMemory(&WscIE, Ptr, sizeof(WSC_IE));
		/* Check for WSC IEs */
		pWscIE = &WscIE;

		/* Check for device password ID, PBC = 0x0004 */
		if (be2cpu16(pWscIE->Type) == 0x1049)
		{
			/*
				4: 0x00 0x50 0xF2 0x04
				4: 0x10 0x49 0x00 0x0x
			*/
			BufLen = DataLen - 4 - 4 - (INT)be2cpu16(pWscIE->Length); 
			NdisMoveMemory(pBuf, pData + 4, BufLen);
			/*
				7: 0x10 0x49 0x00 0x0x 0x00 0x0c 0x43
			*/
			NdisMoveMemory(pPeerWpsMIC, Ptr + 7, LEN_KEY_DESC_MIC);
			WpsCalculateMIC(pAd, pBuf, BufLen, pEasyConfigInfo, pMacAddr, pWpsMIC);
			if (NdisEqualMemory(pPeerWpsMIC, pWpsMIC, LEN_KEY_DESC_MIC))
				bStatus = TRUE;
			break;
		}
		
		/* Set the offset and look for PBC information */
		/* Since Type and Length are both short type, we need to offset 4, not 2 */
		Ptr		+= (be2cpu16(pWscIE->Length) + 4);
		Length	-= (be2cpu16(pWscIE->Length) + 4);
	}
	
	os_free_mem(NULL, pBuf);
	return bStatus;
}

VOID	WpsCalculateMIC(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR				pBuf,
	IN	INT					BufLen,
	IN	PEASY_CONFIG_INFO	pEasyConfig,
	IN  PUCHAR				pMacAddr,
	OUT	PUCHAR				pMIC)
{
	PUCHAR			pDigest = NULL, pTemp = NULL;

	os_alloc_mem(NULL, &pTemp, 256);
	if(pTemp == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: pTemp alloc fail !!\n", __FUNCTION__));
		return;
	}
	
	os_alloc_mem(NULL, &pDigest, 80);
	if(pDigest == NULL)
	{
		os_free_mem(pAd, pTemp);
		DBGPRINT(RT_DEBUG_WARN, ("%s: pDigest alloc fail !!\n", __FUNCTION__));
		return;
	}
	
	NdisZeroMemory(pTemp, 256);
	NdisMoveMemory(pTemp, pBuf, BufLen);	
	NdisMoveMemory(pTemp + BufLen, pMacAddr, MAC_ADDR_LEN);	
	
	RT_HMAC_SHA1(pEasyConfig->WpsPTK, LEN_PTK_KCK, pTemp, BufLen + MAC_ADDR_LEN, pDigest, SHA1_DIGEST_SIZE);
	NdisMoveMemory(pMIC, pDigest, LEN_KEY_DESC_MIC);

	os_free_mem(pAd, pTemp);
	os_free_mem(pAd, pDigest);
}
#endif /* EASY_CONFIG_SETUP */

#ifdef WAC_SUPPORT

#if 0
This new auto provision proposal is from Samsung.
WAC: Wi-Fi Auto Configuation
#endif
/*
	SAMSUNG DMC
	WAC Attribute IDs
*/
#define	WAC_DEVICE_ADVERTISE_ID	0x00
#define WAC_AUTO_PROVISION_ID	0x01
#define WAC_STATUS_ID			0x02
#define WAC_DEVICE_INFORMATION_ID			0x03
/* Length of WAC Attributes */
#define SAMSUNG_OUI_LEN						4
#define DEVICE_ADVERTISE_ATTRIBUTE_LEN		3
#define DEVICE_INFORMATION_ATTRIBUTE_LEN	7
#define STATUS_ATTRIBUTE_LEN				3
#define AUTO_PROVISIONING_ATTRIBUTE_LEN	10
/* Setting of Device Information Fields */
#define THRESHOLD_OFFSET1						0	/* In current, not used */
#define THRESHOLD_OFFSET2						0	/* In current, not used */
#define INTERNAL_ANTENNA						0
#define EXTERNAL_ANTENNA						1
/* Device Types */
#define DEVICE_TYPE_TV							0
#define DEVICE_TYPE_TC							6
#define DEVICE_TYPE_BD							9
/* Device Models */
#define DEVICE_MODEL_TV							0	/* In current, not used */
/* RSSI Threshold */
#define	RSSI_THRESHOLD							-70
/*
The Format of Device Information Attribute
---------------------------------------------------------------------------------------------------------------------
| Field              | Size(Octets) | Value        | Description
---------------------------------------------------------------------------------------------------------------------
| Attribute ID       | 1            | 0x03         | Indicate Device Information attribute
| Length             | 1            | variable     | Length of Device Type, Year of Production, and Device Model field in octets
| Threshold Offset1  | 1            | variable     | Offset for AP Tx Power, Signed value [dB] (-128 ~ + 127)
| Threshold Offset2  | 1            | variable     | Offset Antenna, etc., Signed value [dB] (-128 ~ +127)
| Antenna Type       | 1            | 0x00 / 0x01  | 0x00 = Internal, 0x01 = External
| Device Type        | 1            | variable     | Device Type Code
| Device Model       | variable     | variable     | Model name of a device
---------------------------------------------------------------------------------------------------------------------

The example of TV
---------------------------------------------------------------------------------------------------------------------
| Field              | Size(Octets) | Value        | Description
---------------------------------------------------------------------------------------------------------------------
| Attribute ID       | 1            | 0x03         | Indicate Device Information attribute
| Length             | 1            | 0x05         |
| Threshold Offset1  | 1            | 0x00         |
| Threshold Offset2  | 1            | 0x00         |
| Antenna Type       | 1            | 0x01         | External
| Device Type        | 1            | 0x00         |
| Device Model       | 1            | 0x00         | Not used
---------------------------------------------------------------------------------------------------------------------
*/

UCHAR SAMSUNG_OUI[] = {0x00, 0x12, 0xFB};
UCHAR SAMSUNG_OUI_TYPE = 0x01;

/*
	Device Advertise Attribute
	Field		Size(Octets)	Value
	ID			1				0
	Length		1				1
	WAC On/Off	1				0x01/0x00 	WAC On(0x01), WAC Off(0x00)
*/
VOID WAC_GenDeviceAdvertiseAttr(
	IN  BOOLEAN			bEnableWAC,
	OUT	PUCHAR			pData,
	OUT PUCHAR			pDataLen)
{
	PUCHAR	pIE_Data = NULL;

	os_alloc_mem(NULL, &pIE_Data, 128);

	if (pIE_Data == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s pIE_Data alloc fail !!\n", __FUNCTION__));
		return;
	}
	
	if (pIE_Data)
	{
	/* Ralink Original
		NdisZeroMemory(pIE_Data, 128);
		pIE_Data[0] = 0xDD;
		pIE_Data[1] = 7;
		NdisMoveMemory(pIE_Data+2, SAMSUNG_OUI, 3);
		pIE_Data[5] = SAMSUNG_OUI_TYPE;
		pIE_Data[6] = WAC_DEVICE_ADVERTISE_ID;
		pIE_Data[7] = 1;
		if (bEnableWAC)
			pIE_Data[8] = 1;
		else
			pIE_Data[8] = 0;
		*pDataLen = 9;
		NdisMoveMemory(pData, pIE_Data, *pDataLen);
		os_free_mem(NULL, pIE_Data);
	*/
		/* SAMSUNG DMC */
		NdisZeroMemory(pIE_Data, 128);
		pIE_Data[0] = 0xDD;
		/* 14 = Leng. of Samsung OUI, Device Advertise Attr. & Device Information Attr. */
		pIE_Data[1] = SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN;
		NdisMoveMemory(pIE_Data+2, SAMSUNG_OUI, 3);
		pIE_Data[5] = SAMSUNG_OUI_TYPE;
		pIE_Data[6] = WAC_DEVICE_ADVERTISE_ID;
		pIE_Data[7] = 1;
		if (bEnableWAC)
			pIE_Data[8] = 1;
		else
			pIE_Data[8] = 0;
		pIE_Data[9] = WAC_DEVICE_INFORMATION_ID;				/* 3 */
		pIE_Data[10] = DEVICE_INFORMATION_ATTRIBUTE_LEN - 2;	/* 5 */
		pIE_Data[11] = THRESHOLD_OFFSET1;						/* 0, Threshold Offset1 */
		pIE_Data[12] = THRESHOLD_OFFSET2;						/* 0, Threshold Offset2 */
		pIE_Data[13] = EXTERNAL_ANTENNA;						/* 1, Antenna Type => External */
		pIE_Data[14] = DEVICE_TYPE_TV;							/* 0, Device Type */
		pIE_Data[15] = DEVICE_MODEL_TV;							/* 0, Device Model */

		/* 2 + Leng. of Samsung OUI, Device Advertise Attr. & Device Information Attr. */
		*pDataLen = 2 + SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN;
		NdisMoveMemory(pData, pIE_Data, *pDataLen);
		os_free_mem(NULL, pIE_Data);
	}
}

BOOLEAN WAC_ParseAttr(
	IN	UCHAR			AttrID,
	IN  PUCHAR			pInData,
	IN	UCHAR			InDataLen,
	OUT	PUCHAR			pOutData,
	IN 	UCHAR			OutDataLen)
{
	UCHAR	eid =0, eid_len = 0, *eid_data;
	UINT	total_ie_len = 0;

	hex_dump("WAC_ParseAttr", pInData, InDataLen);
	
	eid = pInData[0];
    eid_len = pInData[1];
	total_ie_len = eid_len + 2;
	eid_data = pInData+2;

	/* get variable fields from payload and advance the pointer */
	while((eid_data + eid_len) <= ((UCHAR*)pInData + InDataLen))
    {
		if (eid == AttrID)
		{
			if (eid_len <= OutDataLen)
			{
				NdisMoveMemory(pOutData, eid_data, eid_len);
			}
			else
			{
				DBGPRINT(RT_DEBUG_WARN, ("%s: OutData is not enough to record data, eid_len = %d\n", 
										__FUNCTION__, eid_len));
				return FALSE;
			}
			return TRUE;
		}
    	eid = pInData[total_ie_len];
    	eid_len = pInData[total_ie_len + 1];
		eid_data = pInData + total_ie_len + 2;
		total_ie_len += (eid_len + 2);
	}
	return FALSE;
}


VOID	WAC_AddDevAdvAttrIE(
	IN  PRTMP_ADAPTER	pAd,
	IN  UCHAR			apidx,
	INOUT PUCHAR		pBuf,
	INOUT ULONG			*pFrameLen)
{
	PUCHAR	pWAC_IE = NULL;
	UCHAR	WAC_IELen = 0;
	ULONG	TmpWACLen = 0;
	BOOLEAN	bWAC_Enable = FALSE;

#ifdef CONFIG_AP_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
		bWAC_Enable = pAd->ApCfg.MBSSID[apidx].EasyConfigInfo.bEnableWAC;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
		bWAC_Enable = pAd->StaCfg.EasyConfigInfo.bEnableWAC;
#endif /* CONFIG_STA_SUPPORT */
	
	os_alloc_mem(NULL, &pWAC_IE, 128);
	if (pWAC_IE)
	{
		WAC_GenDeviceAdvertiseAttr(bWAC_Enable, pWAC_IE, &WAC_IELen);
		MakeOutgoingFrame(pBuf+(*pFrameLen),	&TmpWACLen,
					  	  WAC_IELen, 			pWAC_IE,
                      	  END_OF_ARGS);
    	(*pFrameLen) += TmpWACLen;
		os_free_mem(NULL, pWAC_IE);
	}
}

#define	WAC_COPY_VENDOR_INFO(__p, __len, __psrc, __src_len, __rv) \
{\
	if (__p)\
	{\
		os_free_mem(NULL, __p);\
		(*__len) = 0;\
	}\
	os_alloc_mem(NULL, (UCHAR **) &__p, (__src_len + 2));\
	if (__p)\
	{\
		(*__len) = (__src_len + 2);\
		NdisMoveMemory((__p+2), __psrc, __src_len);\
		*(__p) = 0xdd;\
		*(__p+1) = __src_len;\
	}\
	else\
		(*__rv) = FALSE;\
}

VOID	WAC_CopyVendorInfo(
	IN  PRTMP_ADAPTER		pAd,
	IN  PWAC_REQUEST		pWAC_Req,
	IN  PEASY_CONFIG_INFO	pEasyInfo)
{
	switch(pWAC_Req->FrameType)
	{
#ifdef CONFIG_STA_SUPPORT
		case WAC_CMD_FOR_PROBE_REQ:
			WAC_COPY_VENDOR_INFO(pEasyInfo->pVendorInfoForProbeReq,
								 &pEasyInfo->VendorInfoForProbeReqLen,
								 pWAC_Req->pVendorInfo,
								 pWAC_Req->VendorInfoLen,
								 &pWAC_Req->RV);
			break;
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
		case WAC_CMD_FOR_PROBE_RSP:
			WAC_COPY_VENDOR_INFO(pEasyInfo->pVendorInfoForProbeRsp,
								 &pEasyInfo->VendorInfoForProbeRspLen,
								 pWAC_Req->pVendorInfo,
								 pWAC_Req->VendorInfoLen,
								 &pWAC_Req->RV);
			break;
		case WAC_CMD_FOR_BEACON:
			WAC_COPY_VENDOR_INFO(pEasyInfo->pVendorInfoForBeacon,
								 &pEasyInfo->VendorInfoForBeaconLen,
								 pWAC_Req->pVendorInfo,
								 pWAC_Req->VendorInfoLen,
								 &pWAC_Req->RV);
			if (pWAC_Req->RV)
			{
				APMakeAllBssBeacon(pAd);
				APUpdateAllBeaconFrame(pAd);
			}
			break;
#endif /* CONFIG_AP_SUPPORT */
		default:
			pWAC_Req->RV = FALSE;
			DBGPRINT(RT_DEBUG_WARN, ("%s: Unknow FrameType = 0x%x\n", 
							__FUNCTION__, pWAC_Req->FrameType));
			break;
	}
}

#define	WAC_FREE_VENDOR_INFO(__p, __len) \
{\
	if (__p)\
	{ \
		os_free_mem(NULL, __p);\
		__p = NULL;\
		(*__len) = 0;\
	} \
}

INT WAC_IoctlReq(
	IN  PRTMP_ADAPTER	pAd,
	IN  UCHAR			ApIdx,
	IN  RTMP_IOCTL_INPUT_STRUCT	*wrq)
{
	PWAC_REQUEST	pWAC_Req = NULL;
	INT				Status = NDIS_STATUS_SUCCESS;
	PEASY_CONFIG_INFO	pEasyInfo = NULL;

#ifdef CONFIG_AP_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
		pEasyInfo = &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
		pEasyInfo = &pAd->StaCfg.EasyConfigInfo;
#endif /* CONFIG_STA_SUPPORT */
	
	os_alloc_mem(NULL, (UCHAR **)&pWAC_Req, sizeof(WAC_REQUEST));

	if (pWAC_Req == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: pWAC_Req alloc fail !!\n", __FUNCTION__));
		return -ENOMEM;
	}

	if (pWAC_Req)
	{
		Status = copy_from_user(pWAC_Req, wrq->u.data.pointer, wrq->u.data.length);
		if (Status == NDIS_STATUS_SUCCESS)
		{
			pWAC_Req->RV = TRUE;
			if (pWAC_Req->bReqType)
			{
				/* set command */
				switch (pWAC_Req->Command)
				{
					case WAC_CMD_ADD:
						WAC_CopyVendorInfo(pAd, pWAC_Req, pEasyInfo);
						break;
					case WAC_CMD_DEL:
#ifdef CONFIG_STA_SUPPORT
						if (pWAC_Req->FrameType == WAC_CMD_FOR_PROBE_REQ)
						{
							WAC_FREE_VENDOR_INFO(pEasyInfo->pVendorInfoForProbeReq, &pEasyInfo->VendorInfoForProbeReqLen);
						}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
						if (pWAC_Req->FrameType == WAC_CMD_FOR_PROBE_RSP)
						{
							WAC_FREE_VENDOR_INFO(pEasyInfo->pVendorInfoForProbeRsp, &pEasyInfo->VendorInfoForProbeRspLen);
						}
						if (pWAC_Req->FrameType == WAC_CMD_FOR_BEACON)
						{
							WAC_FREE_VENDOR_INFO(pEasyInfo->pVendorInfoForBeacon, &pEasyInfo->VendorInfoForBeaconLen);
							APMakeAllBssBeacon(pAd);
							APUpdateAllBeaconFrame(pAd);
						}
#endif /* CONFIG_AP_SUPPORT */
						break;
					default:
						pWAC_Req->RV = FALSE;
						DBGPRINT(RT_DEBUG_WARN, ("%s: Unknow command = 0x%x\n", 
										__FUNCTION__, pWAC_Req->Command));
						break;
				}
				if (pWAC_Req->RV)
				{
					Status = copy_to_user(wrq->u.data.pointer, pWAC_Req, wrq->u.data.length);
				}
				else
					Status = -EFAULT;
			}
			else
			{
				/* get command */
				pWAC_Req->CurrentStatus = pEasyInfo->CurrentStatus;				
				Status = copy_to_user(wrq->u.data.pointer, pWAC_Req, wrq->u.data.length);
			}
		}
		os_free_mem(NULL, pWAC_Req);
	}
	return Status;
}

#ifdef CONFIG_AP_SUPPORT

/*
	Device Advertise Attribute
	Field		Size(Octets)	Value
	ID			1				0
	Length		1				1
	Status Code	1				0	Accepted
								1	Rejected because of invalid parameters
								2	Rejected because a WAC AP is unable to accommodate request
								3	Rejected because the RSS is not stronger than -25 dBm
								4 ~ 255	Reserved
*/
/* Ralink Original
VOID WAC_GenStatusAttr(
	IN  PRTMP_ADAPTER	pAd,
	IN	UCHAR			ApIdx,
	IN	CHAR			PeerRssi,
	IN  BOOLEAN			bInvalidData,
	OUT	PUCHAR			pData,
	OUT PUCHAR			pDataLen)
*/
/* SAMSUNG DMC */
VOID WAC_GenStatusAttr(
	IN  PRTMP_ADAPTER	pAd,
	IN	UCHAR			ApIdx,
	IN	CHAR			PeerRssi,
	IN  BOOLEAN			bInvalidData,
	IN CHAR				ThresholdOffset1,
	OUT	PUCHAR			pData,
	OUT PUCHAR			pDataLen)
{
	PUCHAR		pIE_Data = NULL;

	if (pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC)
	{
		os_alloc_mem(NULL, &pIE_Data, 128);
		if (pIE_Data)
		{
#if 0 /* Ralink Original */
			NdisZeroMemory(pIE_Data, 128);
			pIE_Data[0] = 0xDD;
			pIE_Data[1] = 10;
			NdisMoveMemory(pIE_Data+2, SAMSUNG_OUI, 3);
			pIE_Data[5] = SAMSUNG_OUI_TYPE;
			pIE_Data[6] = WAC_DEVICE_ADVERTISE_ID;
			pIE_Data[7] = 1;
			if (pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC)
				pIE_Data[8] = 1;
			else
				pIE_Data[8] = 0;			
			pIE_Data[9] = WAC_STATUS_ID;
			pIE_Data[10] = 1;
			if (bInvalidData)
			{
				pIE_Data[11] = 1;
				pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.CurrentStatus = OFC_FAILED_INVALID_PARAM;
			}
			else
			{
#ifdef WSC_AP_SUPPORT
				PWSC_CTRL	pWscCtrl = &pAd->ApCfg.MBSSID[ApIdx].WscControl;
				if (pWscCtrl->EapMsgRunning ||
					(pWscCtrl->WscConfMode & WSC_REGISTRAR) == FALSE ||
					pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC == FALSE)
				{
					DBGPRINT(RT_DEBUG_WARN, ("%s: EapMsgRunning = %d\n", __FUNCTION__, 
												pWscCtrl->EapMsgRunning));
					pIE_Data[11] = 2;
					pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.CurrentStatus = OFC_FAILED_REJECT;
				}
				else
#endif /* WSC_AP_SUPPORT */
				{
					if (PeerRssi < -25)
					{
						DBGPRINT(RT_DEBUG_WARN, ("%s: PeerRssi = %d\n", __FUNCTION__, PeerRssi));
						pIE_Data[11] = 3;
						pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.CurrentStatus = OFC_FAILED_LOW_RSSI;
					}
					else
						pIE_Data[11] = 0;
				}
			}
			*pDataLen = 12;
			NdisMoveMemory(pData, pIE_Data, *pDataLen);
			os_free_mem(NULL, pIE_Data);
#endif
			/* SAMSUNG DMC*/
			NdisZeroMemory(pIE_Data, 128);
			pIE_Data[0] = 0xDD;			/* 17 = Leng. of Samsung OUI, Device Advertise Attr., Device Information Attr. & Status Attr. */
			pIE_Data[1] = SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN + STATUS_ATTRIBUTE_LEN;
			NdisMoveMemory(pIE_Data+2, SAMSUNG_OUI, 3);
			pIE_Data[5] = SAMSUNG_OUI_TYPE;
			pIE_Data[6] = WAC_DEVICE_ADVERTISE_ID;
			pIE_Data[7] = 1;
			if (pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC)
				pIE_Data[8] = 1;
			else
				pIE_Data[8] = 0;
			pIE_Data[9] = WAC_DEVICE_INFORMATION_ID;              /* 3 */
			pIE_Data[10] = DEVICE_INFORMATION_ATTRIBUTE_LEN - 2;  /* 5 */
			pIE_Data[11] = THRESHOLD_OFFSET1;                     /* 0, Threshold Offset1 */
			pIE_Data[12] = THRESHOLD_OFFSET2;                     /* 0, Threshold Offset2 */
			pIE_Data[13] = EXTERNAL_ANTENNA;                      /* 1, Antenna Type => External */
			pIE_Data[14] = DEVICE_TYPE_TV;                        /* 0, Device Type */
			pIE_Data[15] = DEVICE_MODEL_TV;                       /* 0, Device Model */
			pIE_Data[16] = WAC_STATUS_ID;
			pIE_Data[17] = 1;
			if (bInvalidData)
			{
				pIE_Data[18] = 1;
				pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.CurrentStatus = OFC_FAILED_INVALID_PARAM;
			}
			else
			{
	#ifdef WSC_AP_SUPPORT
				PWSC_CTRL	pWscCtrl = &pAd->ApCfg.MBSSID[ApIdx].WscControl;
				if (pWscCtrl->EapMsgRunning ||
					(pWscCtrl->WscConfMode & WSC_REGISTRAR) == FALSE ||
					pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC == FALSE)
				{
					DBGPRINT(RT_DEBUG_WARN, ("%s: EapMsgRunning = %d\n", __FUNCTION__,
												pWscCtrl->EapMsgRunning));
					pIE_Data[18] = 2;
					pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.CurrentStatus = OFC_FAILED_REJECT;
				}
				else
	#endif // WSC_AP_SUPPORT //
				{
					/* 
						Ralink Original
						if (PeerRssi < -25)					
					*/
					/* SAMSUNG DMC */
					if (PeerRssi + ThresholdOffset1 < RSSI_THRESHOLD)
					{
						DBGPRINT(RT_DEBUG_WARN, ("%s: PeerRssi = %d\n", __FUNCTION__, PeerRssi));
						pIE_Data[18] = 3;
						pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.CurrentStatus = OFC_FAILED_LOW_RSSI;
					}
					else
						pIE_Data[18] = 0;
				}
			}
			/* 19 = 2 + Leng. of Samsung OUI, Device Advertise Attr., Device Information Attr., & Status Attr. */
			*pDataLen = 2 + SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN + STATUS_ATTRIBUTE_LEN;
			NdisMoveMemory(pData, pIE_Data, *pDataLen);
			os_free_mem(NULL, pIE_Data);
		}
	}
}


/* Ralink Original
BOOLEAN WAC_PeerProbeReqSanity(
    IN  PRTMP_ADAPTER	pAd,
    IN  UCHAR			ApIdx,
    IN  VOID			*Msg, 
    IN  ULONG			MsgLen,
    OUT	BOOLEAN			*pPeerWAC,
    OUT	UCHAR			*pParseResult)
*/
/* SAMSUNG DMC */
BOOLEAN WAC_PeerProbeReqSanity(
    IN  PRTMP_ADAPTER	pAd,
    IN  UCHAR			ApIdx,
    IN  VOID			*Msg, 
    IN  ULONG			MsgLen,
    OUT	BOOLEAN			*pPeerWAC,
	OUT CHAR			*pThresholdOffset1,
	OUT	UCHAR			*pPeerDeviceType,
    OUT	UCHAR			*pParseResult)
{
    PFRAME_802_11 Fr = (PFRAME_802_11)Msg;
    UCHAR		*Ptr;
    UCHAR		eid =0, eid_len = 0, *eid_data;
	UCHAR		oui_type = 0;
	UINT		total_ie_len = 0;
	BOOLEAN		bWAC_Enable= FALSE;
	BOOLEAN		bFoundSamsungWAC_OUI = FALSE;

    Ptr = Fr->Octet;
    eid = Ptr[0];
    eid_len = Ptr[1];
	total_ie_len = eid_len + 2;
	eid_data = Ptr+2;
	
    /* get variable fields from payload and advance the pointer */
	while((eid_data + eid_len) <= ((UCHAR*)Fr + MsgLen))
    {    	
        switch(eid)
        {
	        case IE_VENDOR_SPECIFIC:
				if (eid_len <= 4)
					break;
				
				if (NdisEqualMemory(eid_data, SAMSUNG_OUI, 3))
				{
					oui_type = eid_data[3];
					DBGPRINT(RT_DEBUG_TRACE, ("%s: Samsung OUI Type = 0x%02X\n", __FUNCTION__, oui_type));
					if (oui_type == SAMSUNG_OUI_TYPE)
					{
						CHAR	device_info[5];
						bFoundSamsungWAC_OUI = TRUE;

						if (WAC_ParseAttr(WAC_DEVICE_ADVERTISE_ID, eid_data+4, eid_len-4, &bWAC_Enable, 1))
						{
							*pParseResult |= 0x01;
							if (bWAC_Enable)
								*pPeerWAC = TRUE;
						}

						if (bWAC_Enable)
						{
							/* SAMSUNG DMC */
							DBGPRINT(RT_DEBUG_TRACE, ("%s: WAC_DEVICE_INFO_ID Parsing...\n", __FUNCTION__));
						
							if (WAC_ParseAttr(WAC_DEVICE_INFORMATION_ID, eid_data+4, eid_len-4, &device_info[0], 5))
							{	
								DBGPRINT(RT_DEBUG_TRACE, ("%s: WAC Device Information: %d, %d, %d, %d, %d\n", __FUNCTION__, device_info[0], device_info[1], device_info[2], device_info[3], device_info[4]));					
								*pThresholdOffset1 = (CHAR) device_info[0];
								*pPeerDeviceType = (CHAR) device_info[3];
							}
							else
							{
								DBGPRINT(RT_DEBUG_TRACE, ("%s: WAC Device Information Parsing Error \n", __FUNCTION__));					
							}
							
							if (WAC_ParseAttr(WAC_AUTO_PROVISION_ID, 
										  eid_data+4, 
										  eid_len-4, 
										  pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WpsPinCode, 
										  8))
							{
#ifdef WSC_INCLUDED
								PWSC_CTRL	pWpsCtrl = &pAd->ApCfg.MBSSID[ApIdx].WscControl;
								UCHAR		PinStr[9];
#endif /* WSC_INCLUDED */
								*pParseResult |= 0x02;
#ifdef WSC_INCLUDED
								NdisMoveMemory(&PinStr[0], &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WpsPinCode[0], 8);
								PinStr[8] = '\0';
								/*
									To make sure of doing WPS process with WPS Client by PIN method
								*/
								pWpsCtrl->WscMode = WSC_PIN_MODE;
								WscGetRegDataPIN(pAd, simple_strtol(PinStr, 0, 10), pWpsCtrl);
								DBGPRINT(RT_DEBUG_TRACE, ("%s: WscPinCode = %08u\n", __FUNCTION__, pWpsCtrl->WscPinCode));
#endif /* WSC_INCLUDED */
							}
								
						}
						else
							*pPeerWAC = FALSE;
					}
					break;
				}
        }

		eid = Ptr[total_ie_len];
    	eid_len = Ptr[total_ie_len + 1];
		eid_data = Ptr + total_ie_len + 2;
		total_ie_len += (eid_len + 2);
	}
	return bFoundSamsungWAC_OUI;
}

VOID	WAC_MaintainPeerList(
	IN  PLIST_HEADER	pWscEnList)
{
	PWSC_PEER_ENTRY 	pPeerEntry = NULL;
	RT_LIST_ENTRY *pListEntry = NULL, *pTempListEntry = NULL;
	ULONG				now_time = 0;

	NdisGetSystemUpTime(&now_time);
	pListEntry = pWscEnList->pHead;
	pPeerEntry = (PWSC_PEER_ENTRY)pListEntry;

	while (pPeerEntry != NULL)
	{
		if (RTMP_TIME_AFTER(now_time, pPeerEntry->receive_time + (5 * OS_HZ)))
		{
			pTempListEntry = pListEntry->pNext;
			delEntryList(pWscEnList, pListEntry);
			os_free_mem(NULL, pPeerEntry);
			pListEntry = pTempListEntry;
		}
		else
			pListEntry = pListEntry->pNext;
		pPeerEntry = (PWSC_PEER_ENTRY)pListEntry;
	}
	
	return;
}

VOID	WAC_CheckWACEntry(
	IN  PRTMP_ADAPTER	pAd,
	IN  MAC_TABLE_ENTRY *pEntry)
{
	if (pEntry == NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: pEntry is NULL!!\n", __FUNCTION__));
		return;
	}
	
	if (pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.bEnableWAC)
	{
#ifdef WAC_QOS_PRIORITY
		PWSC_PEER_ENTRY 	pWACPeerEntry = NULL;
		PLIST_HEADER		pWacFroceEnList = &pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.WAC_ForcePriorityList;
#endif /* WAC_QOS_PRIORITY */
		PLIST_HEADER	pWscEnList = &pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.WAC_PeerList;

		RTMP_SEM_LOCK(&pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.WAC_PeerListSemLock);
		WAC_MaintainPeerList(pWscEnList);
		if (WscFindPeerEntry(pWscEnList, pEntry->Addr))
		{
			pEntry->bRaAutoWpsCapable = TRUE;
			pEntry->bSamsungAutoWpsCapable = TRUE;
		}
		RTMP_SEM_UNLOCK(&pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.WAC_PeerListSemLock);

#ifdef WAC_QOS_PRIORITY
		RTMP_SEM_LOCK(&pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.WAC_ForcePriorityListSemLock);

		pWACPeerEntry = WscFindPeerEntry(pWacFroceEnList, pEntry->Addr);
		if (pWACPeerEntry)
		{
			STA_TR_ENTRY *tr_entry = &pAd->MacTab.tr_entry[pEntry->wcid];
			if ((pWACPeerEntry->device_type == WAC_DEVICE_TYPE_TV) ||
				(pWACPeerEntry->device_type == WAC_DEVICE_TYPE_BD)) {
				tr_entry->bSamsungForcePriority = TRUE;
			}
		}
		RTMP_SEM_UNLOCK(&pAd->ApCfg.MBSSID[pEntry->func_tb_idx].EasyConfigInfo.WAC_ForcePriorityListSemLock);
#endif /* WAC_QOS_PRIORITY */
	}
}

VOID	WAC_PeerProbeReq(
	IN  PRTMP_ADAPTER 	pAd, 
	IN  UCHAR 			ApIdx, 
	IN  MLME_QUEUE_ELEM *Elem,
	IN  UCHAR			*Addr2,
	INOUT UCHAR			*pBuf,
	INOUT ULONG			*pFrameLen)
{
	PUCHAR	pWAC_IE = NULL;
	UCHAR	WAC_IELen = 0;
	ULONG	TmpWACLen = 0;		
	BOOLEAN	bPeerWAC = FALSE;
	/* SAMSUNG DMC */
	CHAR Threshold_Offset1 = 0;
	UCHAR	ParseResult = 0;
	UCHAR	PeerDeviceType = 0xff;

	if (WAC_PeerProbeReqSanity(pAd, ApIdx, Elem->Msg, Elem->MsgLen, &bPeerWAC, &Threshold_Offset1, &PeerDeviceType, &ParseResult))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Found Samsung WAC OUI - bPeerWAC = %d, ParseResult = 0x%x\n", __FUNCTION__, bPeerWAC, ParseResult));
		os_alloc_mem(NULL, &pWAC_IE, 128);
		if (pWAC_IE)
		{
			ULONG WACTmpLen = 0;
				
			/*
				Peer is WAC enable
			*/
			if (((ParseResult & 0x01) == TRUE) && (bPeerWAC == TRUE))
			{
#ifdef WAC_QOS_PRIORITY
				PLIST_HEADER	pWacForceList = &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WAC_ForcePriorityList;
#endif /* WAC_QOS_PRIORITY */
				PFRAME_802_11 p80211Frame = (PFRAME_802_11)Elem->Msg;
				CHAR PeerRssi = 0;
				PeerRssi = RTMPMaxRssi(pAd, 
							ConvertToRssi(pAd, &Elem->rssi_info, RSSI_IDX_0), 
							ConvertToRssi(pAd, &Elem->rssi_info, RSSI_IDX_1), 
							ConvertToRssi(pAd, &Elem->rssi_info, RSSI_IDX_2));
				if (MAC_ADDR_EQUAL(p80211Frame->Hdr.Addr3, pAd->ApCfg.MBSSID[ApIdx].Bssid) && (ParseResult & 0x02))
				{
#ifdef WSC_AP_SUPPORT
					PWSC_CTRL	pWscCtrl = &pAd->ApCfg.MBSSID[ApIdx].WscControl;
					if (NdisEqualMemory(pWscCtrl->EntryAddr, ZERO_MAC_ADDR, MAC_ADDR_LEN) ||
						NdisEqualMemory(pWscCtrl->EntryAddr, p80211Frame->Hdr.Addr2, MAC_ADDR_LEN))
					{
						pWscCtrl->EapMsgRunning = FALSE;
					}
#endif /* WSC_AP_SUPPORT */
					
					/* 
						Ralink Original
						WAC_GenStatusAttr(pAd, ApIdx, PeerRssi, FALSE, pWAC_IE, &WAC_IELen);
					*/
					/* SAMSUNG DMC */
					WAC_GenStatusAttr(pAd, ApIdx, PeerRssi, FALSE, Threshold_Offset1, pWAC_IE, &WAC_IELen);
					if ((ParseResult & 0x02) && (pWAC_IE[11] == 0))
					{
						PLIST_HEADER pWscEnList = &pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WAC_PeerList;
						RTMP_SEM_LOCK(&pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WAC_PeerListSemLock);
						WscInsertPeerEntryByMAC(pWscEnList, Addr2);	
						RTMP_SEM_UNLOCK(&pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WAC_PeerListSemLock);
					}
				}						
				else
					WAC_GenDeviceAdvertiseAttr(pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC, pWAC_IE, &WAC_IELen);
#ifdef WAC_QOS_PRIORITY
				RTMP_SEM_LOCK(&pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WAC_ForcePriorityListSemLock);
				WAC_InsertForcePriorityEntryByMAC(pWacForceList, Addr2, PeerDeviceType);
				RTMP_SEM_UNLOCK(&pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.WAC_ForcePriorityListSemLock);
#endif /* WAC_QOS_PRIORITY */
			}
			else
				WAC_GenDeviceAdvertiseAttr(pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC, pWAC_IE, &WAC_IELen);
			
    		MakeOutgoingFrame(pBuf+(*pFrameLen),	&WACTmpLen,
    						  WAC_IELen,			pWAC_IE,
                              END_OF_ARGS);
    		(*pFrameLen) += WACTmpLen;
			os_free_mem(NULL, pWAC_IE);
		}
	}
	else
	{
		os_alloc_mem(NULL, &pWAC_IE, 128);
		if (pWAC_IE)
		{
			WAC_GenDeviceAdvertiseAttr(pAd->ApCfg.MBSSID[ApIdx].EasyConfigInfo.bEnableWAC, pWAC_IE, &WAC_IELen);
			MakeOutgoingFrame(pBuf+(*pFrameLen),	&TmpWACLen,
						  	  WAC_IELen, 			pWAC_IE,
                          	  END_OF_ARGS);
        	(*pFrameLen) += TmpWACLen;
			os_free_mem(NULL, pWAC_IE);
		}
	}
}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
/*
	Device Advertise Attribute
	Field		Size(Octets)	Value
	ID			1				0
	Length		1				1
	WAC On/Off	1				0x01/0x00 	WAC On(0x01), WAC Off(0x00)
*/
VOID WAC_GenUicastProbeReqAttr(
	IN  BOOLEAN			bEnableWAC,
	IN  PUCHAR			pPIN,
	OUT	PUCHAR			pData,
	OUT PUCHAR			pDataLen)
{
	PUCHAR	pIE_Data = NULL;

	os_alloc_mem(NULL, &pIE_Data, 128);

	if (pIE_Data == NULL)
	{
		DBGPRINT(RT_DEBUG_WARN, ("%s: pIE_Data alloc fail !!\n", __FUNCTION__));
		return;
	}
	
	if (pIE_Data)
	{
/* Ralink Original
		NdisZeroMemory(pIE_Data, 128);
		pIE_Data[0] = 0xDD;
		if (bEnableWAC)
			pIE_Data[1] = 17;
		else
			pIE_Data[1] = 7;
		NdisMoveMemory(pIE_Data+2, SAMSUNG_OUI, 3);
		pIE_Data[5] = SAMSUNG_OUI_TYPE;
		pIE_Data[6] = WAC_DEVICE_ADVERTISE_ID;
		pIE_Data[7] = 1;
		if (bEnableWAC)
			pIE_Data[8] = 1;
		else
			pIE_Data[8] = 0;
		if (bEnableWAC)
		{
			pIE_Data[9] = WAC_AUTO_PROVISION_ID;
			pIE_Data[10] = 8;
			NdisMoveMemory(pIE_Data+11, pPIN, 8);
			*pDataLen = 19;
		}
		else
			*pDataLen = 9;
		NdisMoveMemory(pData, pIE_Data, *pDataLen);
		os_free_mem(NULL, pIE_Data);
*/
		/* SAMSUNG DMC */
		NdisZeroMemory(pIE_Data, 128);
		pIE_Data[0] = 0xDD;
		if (bEnableWAC)
			/* 24 = Leng. of Samsung OUI, Device Advertise Attr., Device Information Attr. & Auto Provisioning Attr. */
			pIE_Data[1] = SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN + AUTO_PROVISIONING_ATTRIBUTE_LEN;
		else
			/* 14 = Leng. of Samsung OUI, Device Advertise Attr. & Device Information Attr. */
			pIE_Data[1] = SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN;
		NdisMoveMemory(pIE_Data+2, SAMSUNG_OUI, 3);
		pIE_Data[5] = SAMSUNG_OUI_TYPE;
		pIE_Data[6] = WAC_DEVICE_ADVERTISE_ID;
		pIE_Data[7] = 1;
		if (bEnableWAC)
			pIE_Data[8] = 1;
		else
			pIE_Data[8] = 0;
		pIE_Data[9] = WAC_DEVICE_INFORMATION_ID;				/* 3 */
		pIE_Data[10] = DEVICE_INFORMATION_ATTRIBUTE_LEN - 2;	/* 5 */
		pIE_Data[11] = THRESHOLD_OFFSET1;						/* 0, Threshold Offset1 */
		pIE_Data[12] = THRESHOLD_OFFSET2;						/* 0, Threshold Offset2 */
		pIE_Data[13] = EXTERNAL_ANTENNA;						/* 1, Antenna Type => External */
		pIE_Data[14] = DEVICE_TYPE_TV;							/* 0, Device Type */
		pIE_Data[15] = DEVICE_MODEL_TV;							/* 0, Device Model */
		if (bEnableWAC)
		{
			pIE_Data[16] = WAC_AUTO_PROVISION_ID;
			pIE_Data[17] = 8;
			NdisMoveMemory(pIE_Data+18, pPIN, 8);
			/* 26 = 2 + Leng. of Samsung OUI, Device Advertise Attr., Device Information Attr. & Auto Provisioning Attr. */
			*pDataLen = 2 + SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN + AUTO_PROVISIONING_ATTRIBUTE_LEN;
		}
		else
		{
			/* 16 = 2 + Leng. of Samsung OUI, Device Advertise Attr. & Device Information Attr. */
			*pDataLen = 2 + SAMSUNG_OUI_LEN + DEVICE_ADVERTISE_ATTRIBUTE_LEN + DEVICE_INFORMATION_ATTRIBUTE_LEN;
		}
		NdisMoveMemory(pData, pIE_Data, *pDataLen);
		os_free_mem(NULL, pIE_Data);
	}
}

UCHAR WAC_SearchSamsungWACAP(
	IN	PRTMP_ADAPTER	pAd)
{
	INT i = 0, Len = 0;
	BSS_ENTRY *pInBss;
	UCHAR Count = 0;
	PWSC_CTRL pWpsCtrl = &pAd->StaCfg.WscControl;
	PBEACON_EID_STRUCT	pEid = NULL;
	UCHAR *pData;
	UUID_BSSID_CH_INFO	apCandiList[8];
	UUID_BSSID_CH_INFO	*pApCandEntry;

 /* Ralink Original
 	CHAR            RssiThreshold = -25;
*/
	/* 
		SAMSUNG DMC
			- does not check RSSI tightly in device driver.
	*/
	CHAR RssiThreshold = -100;

	NdisZeroMemory(&apCandiList[0], sizeof(apCandiList));
	pAd->StaCfg.EasyConfigInfo.CurrentStatus = OFC_FAILED_NO_WAC_AP;
	for (i = 0; i < pAd->ScanTab.BssNr; i++) 
	{
		/* BSS entry for VarIE processing */
		pInBss  = (BSS_ENTRY *) &pAd->ScanTab.BssEntry[i];

		if ((pInBss->WpsAP & 0x04) && (Count < 8))
		{
			if (pInBss->Rssi >= RssiThreshold)
			{
				pApCandEntry = &apCandiList[Count];
				NdisZeroMemory(pApCandEntry->Ssid, MAX_LEN_OF_SSID);
				NdisZeroMemory(pApCandEntry->Bssid, MAC_ADDR_LEN);
				NdisZeroMemory(pApCandEntry->MacAddr, MAC_ADDR_LEN);
				NdisZeroMemory(pApCandEntry->Uuid, 16);

				pApCandEntry->Channel = pInBss->Channel;
				pApCandEntry->SsidLen = pInBss->SsidLen;
				NdisMoveMemory(pApCandEntry->Ssid, pInBss->Ssid, pInBss->SsidLen);
				NdisMoveMemory(pApCandEntry->Bssid, pInBss->Bssid, MAC_ADDR_LEN);
				NdisMoveMemory(pApCandEntry->MacAddr, pInBss->MacAddr, MAC_ADDR_LEN);
				Count++;
			}
			else
			{
				pAd->StaCfg.EasyConfigInfo.CurrentStatus = OFC_FAILED_LOW_RSSI;
				DBGPRINT(RT_DEBUG_WARN, ("%s: pInBss->Rssi = %d\n", __FUNCTION__, pInBss->Rssi));
			}
		}
	}
	
#if 0
	if (Count != 1)
	{
		NdisZeroMemory(&pWpsCtrl->WscSsid, sizeof(NDIS_802_11_SSID));
		NdisZeroMemory(pWpsCtrl->WscBssid, MAC_ADDR_LEN);
		NdisZeroMemory(pWpsCtrl->WscPeerMAC, MAC_ADDR_LEN);
		pAd->MlmeAux.Channel = pAd->CommonCfg.Channel;
	}
	else
#else
	if (Count != 0)
#endif
	{
		NdisZeroMemory(&pWpsCtrl->WscSsid, sizeof(NDIS_802_11_SSID));
		NdisZeroMemory(pWpsCtrl->WscBssid, MAC_ADDR_LEN);
		NdisZeroMemory(pWpsCtrl->WscPeerMAC, MAC_ADDR_LEN);
		
		pWpsCtrl->WscSsid.SsidLength = apCandiList[0].SsidLen;
		NdisMoveMemory(pWpsCtrl->WscSsid.Ssid, apCandiList[0].Ssid, apCandiList[0].SsidLen);
		NdisMoveMemory(pWpsCtrl->WscBssid, apCandiList[0].Bssid, MAC_ADDR_LEN);
		NdisMoveMemory(pWpsCtrl->WscPeerMAC, apCandiList[0].MacAddr, MAC_ADDR_LEN);
		pAd->MlmeAux.Channel = apCandiList[0].Channel;
	}

	return Count;
}

BOOLEAN WAC_FindWACAP(
	IN	PRTMP_ADAPTER	pAd)
{
	UCHAR		ApCount = 0;
	PWSC_CTRL	pWpsCtrl = &pAd->StaCfg.WscControl;
	ApCount = WAC_SearchSamsungWACAP(pAd);
	pAd->StaCfg.EasyConfigInfo.bWACAP = FALSE;
	if (ApCount != 0)
	{
		/*
			Update Reconnect Ssid, that user desired to connect.
		*/
		NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, MAX_LEN_OF_SSID);
		NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, pWpsCtrl->WscSsid.Ssid, pWpsCtrl->WscSsid.SsidLength);
		pAd->MlmeAux.AutoReconnectSsidLen = pWpsCtrl->WscSsid.SsidLength;

		pAd->bConfigChanged = TRUE;

		// Turn off WSC state matchine
		WscStop(pAd, 
#ifdef CONFIG_AP_SUPPORT
				FALSE, 
#endif /* CONFIG_AP_SUPPORT */
				pWpsCtrl);

		// Set WSC state to WSC_STATE_INIT
		pWpsCtrl->WscState = WSC_STATE_INIT;
		pWpsCtrl->WscStatus = STATUS_WSC_SCAN_AP;

		// Init Registrar pair structures
		WscInitRegistrarPair(pAd, pWpsCtrl, BSS0);

		pAd->StaCfg.BssType = BSS_INFRA;
		pWpsCtrl->WscConfStatus = WSC_SCSTATE_UNCONFIGURED;
		pWpsCtrl->WscConfMode = WSC_ENROLLEE;
		pWpsCtrl->WscMode = 1;
		pAd->StaCfg.EasyConfigInfo.bDoAutoWps = TRUE;
		pAd->StaCfg.EasyConfigInfo.bWACAP = TRUE;
		pAd->StaCfg.EasyConfigInfo.CurrentStatus = OFC_PROCEED_SECOND_PHASE;
		MlmeEnqueue(pAd, 
					MLME_CNTL_STATE_MACHINE, 
					OID_802_11_BSSID,
					MAC_ADDR_LEN,
					pWpsCtrl->WscBssid, 0);
		// call Mlme handler to execute it
		RTMP_MLME_HANDLER(pAd);
		return TRUE;
	}
	return FALSE;
}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#ifdef WAC_QOS_PRIORITY
VOID	WAC_InsertForcePriorityEntryByMAC(
	IN  PLIST_HEADER	pWscEnList,
	IN	PUCHAR			pMacAddr,
	IN	UCHAR			DeviceType)
{
	PWSC_PEER_ENTRY		pWscPeer = NULL;
	
	pWscPeer = WscFindPeerEntry(pWscEnList, pMacAddr);
	if (pWscPeer)
	{
		NdisGetSystemUpTime(&pWscPeer->receive_time);
		pWscPeer->device_type = DeviceType;
	}
	else
	{
		os_alloc_mem(NULL, &pWscPeer, WSC_PEER_ENTRY_SIZE);
		if (pWscPeer)
		{
			NdisZeroMemory(pWscPeer, WSC_PEER_ENTRY_SIZE);
			NdisMoveMemory(pWscPeer->mac_addr, pMacAddr, MAC_ADDR_LEN);
			NdisGetSystemUpTime(&pWscPeer->receive_time);
			pWscPeer->device_type = DeviceType;
			insertTailList(pWscEnList, (RT_LIST_ENTRY *)pWscPeer);
		}
		ASSERT(pWscPeer != NULL);
	}
}
#endif /* WAC_QOS_PRIORITY */
#endif /* CONFIG_AP_SUPPORT */
#endif /* WAC_SUPPORT */

