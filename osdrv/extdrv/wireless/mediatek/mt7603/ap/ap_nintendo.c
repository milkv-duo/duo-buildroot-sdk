/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    ap_nintendo.c

    Abstract:
    Miniport Query information related subroutines

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    JuemingChen 12-28-2006    created

*/
#include    "rt_config.h"

#ifdef NINTENDO_AP

#define CAPABILITY_PRIVACY 0x0010

/*static RT_NINTENDO_TABLE	DS_TABLE; */

#ifdef CHIP25XX
/*
static spinlock_t			NINTENDO_TABLE_Lock;
static spinlock_t			NINTENDO_TABLE_Lock;
*/
#define TEMP_SEM_LOCK(__lock, __irqflag)	spin_lock_bh(__lock)
#define TEMP_SEM_UNLOCK(__lock, __irqflag)	spin_unlock_bh(__lock)
#else /*CHIP25XX */
/*static NDIS_SPIN_LOCK		NINTENDO_TABLE_Lock; //os_lock */
#define TEMP_SEM_LOCK(__lock, __irqflag)	RTMP_SEM_LOCK(__lock)
#define TEMP_SEM_UNLOCK(__lock, __irqflag)	RTMP_SEM_UNLOCK(__lock)
#endif /*CHIP25XX */

/*
static UCHAR NINTENDO_UP_BUFFER[512];
static UCHAR Local_KeyIdx = 0;
static CIPHER_KEY Local_SharedKey;
static UCHAR Local_bHideSsid;
static UCHAR Local_AuthMode;
static UCHAR Local_WepStatus;
static USHORT Local_CapabilityInfo;
*/

#define NINTO_INFO	pAd->nindo_ctrl_block

static int MakeUsbWepKey(const unsigned char wepseed[20], unsigned char wepkey[13])
{
	int i=0;
	const char* MASKkey0 = "gwi'6&fs=0Nf~";
	const char* MASKkey1 = "%(egEr)ag(s&m";
	const unsigned char wepkeylength = 13; 
	const unsigned char ttable[16] = {10,13,14,8,9,3,6,0,12,5,2,7,11,1,15,4 };
	const unsigned char ctable[13] = {5,1,12,4,2,3,10,0,11,7,9,8,6 };
	unsigned char keytemp[wepkeylength]; 

	for( i =0; i < wepkeylength; i ++ ){
		wepkey[i] = wepseed[i] ^ wepseed[wepkeylength + i % 7]; 
	}
	for( i =0; i < 7; i ++ ){
		wepkey[3 + i]=wepkey[3 + i]^ wepseed[wepkeylength + i]; 
	}

	for( i =0; i < wepkeylength; i ++ ){
		wepkey[i] = wepkey[i] ^ MASKkey0[i]; 
	}

	memcpy(keytemp, wepkey, wepkeylength); 
	for( i =0; i  < wepkeylength; i ++ ){
		wepkey[ctable[i]] = keytemp[i]; 
	}

	for ( i =0; i < wepkeylength; i ++ ){
		wepkey[i] = wepkey[i] ^ MASKkey1[i]; 
	}

	for( i =0; i < wepkeylength; i ++ ){
		wepkey[i] = ttable[(wepkey[i] >> 4) & 0x0F] <<4 | ttable[wepkey[i] & 0x0F]; 
	}

	for( i =0; i < 3; i ++ ){
		wepkey[i] = wepkey[i] ^ wepkey[i + 6];
		wepkey[i + 3]=wepkey[i + 3]^ wepkey[i + 9];
		wepkey[i + 6]=wepkey[i + 6]^ wepkey[i + 3];
		wepkey[i + 9]=wepkey[i + 9]^ wepkey[i];
		wepkey[12]     =wepkey[12]     ^ wepkey[i];
	}
	return 0;
}

static VOID NintendoEnable(
	IN	PRTMP_ADAPTER	pAd)
{
	NINTO_INFO.Local_KeyIdx = pAd->ApCfg.MBSSID[BSS0].wdev.DefaultKeyId;

	if(0 == pAd->bNintendoCapable)
	{
		NINTO_INFO.Local_bHideSsid			= pAd->ApCfg.MBSSID[BSS0].bHideSsid;
		NINTO_INFO.Local_AuthMode			= pAd->ApCfg.MBSSID[BSS0].wdev.AuthMode;
		NINTO_INFO.Local_WepStatus			= pAd->ApCfg.MBSSID[BSS0].wdev.WepStatus;
		NINTO_INFO.Local_CapabilityInfo	= pAd->ApCfg.MBSSID[BSS0].CapabilityInfo;
		NdisMoveMemory(NINTO_INFO.Local_SharedKey.Key, pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].Key, NINTENDO_WEPKEY_LN);
		NINTO_INFO.Local_SharedKey.KeyLen = pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].KeyLen;
		NINTO_INFO.Local_SharedKey.CipherAlg = pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].CipherAlg;
	}

	pAd->ApCfg.MBSSID[BSS0].bHideSsid = 1;
	pAd->ApCfg.MBSSID[BSS0].wdev.AuthMode = Ndis802_11AuthModeShared;
	pAd->ApCfg.MBSSID[BSS0].wdev.WepStatus = Ndis802_11WEPEnabled;
	pAd->ApCfg.MBSSID[BSS0].CapabilityInfo |= CAPABILITY_PRIVACY;
/*
	pAd->ApCfg.MBSSID[BSS0].GroupKeyWepStatus = Ndis802_11WEPEnabled;
	pAd->CommonCfg.GroupKeyWepStatus =  Ndis802_11WEPEnabled;
*/
	NdisMoveMemory(pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].Key, pAd->NINTENDO_WEP_KEY, NINTENDO_WEPKEY_LN);
	pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].KeyLen = (UCHAR) NINTENDO_WEPKEY_LN;
	pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].CipherAlg = CIPHER_WEP128;
	pAd->bNintendoCapable = 1;
}

static VOID NintendoDisable(
	IN	PRTMP_ADAPTER	pAd)
{
	if(1 == pAd->bNintendoCapable)
	{
		UINT8 Wcid;
	
		pAd->ApCfg.MBSSID[BSS0].bHideSsid = NINTO_INFO.Local_bHideSsid;
		pAd->ApCfg.MBSSID[BSS0].wdev.AuthMode = NINTO_INFO.Local_AuthMode;
		pAd->ApCfg.MBSSID[BSS0].wdev.WepStatus = NINTO_INFO.Local_WepStatus;
		pAd->ApCfg.MBSSID[BSS0].CapabilityInfo = NINTO_INFO.Local_CapabilityInfo;
		NdisMoveMemory(pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].Key, NINTO_INFO.Local_SharedKey.Key, NINTENDO_WEPKEY_LN);
		pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].KeyLen = (UCHAR) NINTO_INFO.Local_SharedKey.KeyLen;
		pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx].CipherAlg = NINTO_INFO.Local_SharedKey.CipherAlg;

		APMakeAllBssBeacon(pAd);
		APUpdateAllBeaconFrame(pAd);
		APUpdateCapabilityAndErpIe(pAd);
		AsicAddSharedKeyEntry(pAd, BSS0, (UCHAR) NINTO_INFO.Local_KeyIdx, 
							  &NINTO_INFO.Local_SharedKey);

		GET_GroupKey_WCID(pAd, Wcid, BSS0);
		RTMPSetWcidSecurityInfo(pAd, BSS0, 
								NINTO_INFO.Local_KeyIdx, 
								NINTO_INFO.Local_SharedKey.CipherAlg,
								Wcid, 
								SHAREDKEYTABLE);
	}
	pAd->bNintendoCapable = 0;
}

extern VOID InitNINTENDO_TABLE(
	IN PRTMP_ADAPTER pAd)
{
	memset(&NINTO_INFO.DS_TABLE, 0 , sizeof(NINTO_INFO.DS_TABLE));
	NdisAllocateSpinLock(pAd, &NINTO_INFO.NINTENDO_TABLE_Lock);
	memcpy(pAd->NINTENDO_WEP_SEED, "zzzzzzzzzzzzzzzzzzzzzzzzzz", NINTENDO_SSID_NICKNAME_LN);
	MakeUsbWepKey(pAd->NINTENDO_WEP_SEED, pAd->NINTENDO_WEP_KEY);

	pAd->bNintendoCapable = 0;
	return;
}

static UCHAR AddNINTENDO_ENTRY(PRTMP_ADAPTER pAd, PRT_NINTENDO_SSID ptmp, UCHAR * pDS_Addr)
{
	int loopi;
	ULONG				IrqFlags;
	
	TEMP_SEM_LOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags)
	for(loopi = 0; (loopi < NINTO_INFO.DS_TABLE.number) && (loopi < NINTENDO_MAX_ENTRY); loopi++)
	{
		if(!memcmp(pDS_Addr, NINTO_INFO.DS_TABLE.entry[loopi].DS_Addr, MAC_ADDR_LEN))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("same MAC %02x%02x%02x%02x%02x%02x\n",
						PRINT_MAC(pDS_Addr)));
			break;
		}
	}

	if (loopi >= NINTO_INFO.DS_TABLE.number)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("add  MAC %02x%02x%02x%02x%02x%02x\n",
					PRINT_MAC(pDS_Addr)));
		memcpy(NINTO_INFO.DS_TABLE.entry[NINTO_INFO.DS_TABLE.number].NICKname, ptmp->NICKname, NINTENDO_SSID_NICKNAME_LN);
		memcpy(NINTO_INFO.DS_TABLE.entry[NINTO_INFO.DS_TABLE.number].DS_Addr, pDS_Addr, MAC_ADDR_LEN);
		NINTO_INFO.DS_TABLE.number++;
	}
	TEMP_SEM_UNLOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags);
	IrqFlags = 0;
	return 0;
}

extern UCHAR DelNINTENDO_ENTRY(PRTMP_ADAPTER pAd, UCHAR * pDS_Addr)
{
	int		loopi;
	ULONG				IrqFlags;
	
	TEMP_SEM_LOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags)
	for(loopi = 0; loopi < NINTO_INFO.DS_TABLE.number && loopi < NINTENDO_MAX_ENTRY; loopi++)
	{
		if(!memcmp(pDS_Addr, NINTO_INFO.DS_TABLE.entry[loopi].DS_Addr, MAC_ADDR_LEN))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("DelNINTENDO_ENTRY loopi %d\n",loopi));
			if(loopi < (NINTO_INFO.DS_TABLE.number - 1) )
			{
				memcpy(NINTO_INFO.DS_TABLE.entry[loopi].NICKname, NINTO_INFO.DS_TABLE.entry[NINTO_INFO.DS_TABLE.number].NICKname, NINTENDO_SSID_NICKNAME_LN);
				memcpy(NINTO_INFO.DS_TABLE.entry[loopi].DS_Addr, NINTO_INFO.DS_TABLE.entry[NINTO_INFO.DS_TABLE.number].DS_Addr, MAC_ADDR_LEN);
			}
			memset(&NINTO_INFO.DS_TABLE.entry[NINTO_INFO.DS_TABLE.number],0,sizeof(RT_NINTENDO_ENTRY));
			NINTO_INFO.DS_TABLE.number--;
			break;
		}
	}
	TEMP_SEM_UNLOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags);
	IrqFlags = 0;
	return 0;
}

extern VOID RTMPIoctlNintendoCapable(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq)
{
	if(1 == wrq->u.data.length)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlNintendoCapable No parameter\n"));
	}
	else if(2 == wrq->u.data.length)
	{
		UCHAR	temp[2];
		INT		Status;

		Status = copy_from_user(temp, wrq->u.data.pointer, wrq->u.data.length);
		DBGPRINT(RT_DEBUG_TRACE, ("RTMPIoctlNintendoCapable %02x %02x\n",temp[0],temp[1]));
		if(0x30 == temp[0])
		{
			NintendoDisable(pAd);
		}
		else if(0x31 == temp[0])
		{
			UINT Wcid;
		
			NintendoEnable(pAd);
			APMakeAllBssBeacon(pAd);
			APUpdateAllBeaconFrame(pAd);
			APUpdateCapabilityAndErpIe(pAd);
			AsicAddSharedKeyEntry(pAd, BSS0, (UCHAR) NINTO_INFO.Local_KeyIdx, 
								  &pAd->SharedKey[BSS0][NINTO_INFO.Local_KeyIdx]);

			GET_GroupKey_WCID(pAd, Wcid, BSS0);			
			RTMPSetWcidSecurityInfo(pAd, BSS0, NINTO_INFO.Local_KeyIdx, 
									CIPHER_WEP128, Wcid, SHAREDKEYTABLE);
		}
		else
			DBGPRINT(RT_DEBUG_TRACE, ("iwpriv ra0 nintendocapable 0/1\n"));
	}
	
	return;
}

extern VOID RTMPIoctlNintendoGetTable(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq)
{
	ULONG				IrqFlags;
	int					Status;

	wrq->u.data.length = sizeof(RT_NINTENDO_TABLE);
	TEMP_SEM_LOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags)
	memcpy(NINTO_INFO.NINTENDO_UP_BUFFER,&NINTO_INFO.DS_TABLE,sizeof(RT_NINTENDO_TABLE));
	TEMP_SEM_UNLOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags);
	IrqFlags = 0;
	Status = copy_to_user(wrq->u.data.pointer, NINTO_INFO.NINTENDO_UP_BUFFER, wrq->u.data.length);
	return;
}

extern VOID RTMPIoctlNintendoSetTable(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq)
{
	int loopi;
	int length;
	PRT_NINTENDO_TABLE	pset = NULL;
	ULONG				IrqFlags;
	INT					Status;
	
	length = sizeof(RT_NINTENDO_TABLE);
	if(1 == wrq->u.data.length)
	{
		DBGPRINT(RT_DEBUG_TRACE, (" wrq->u.data.length %02x Use iwprive?\n",wrq->u.data.length));
		for(loopi=0; loopi<NINTENDO_MAX_ENTRY; loopi++)
			NINTO_INFO.DS_TABLE.entry[loopi].registe = 1;
		return;
	}
	else if(length != wrq->u.data.length)
	{
		DBGPRINT(RT_DEBUG_TRACE, (" NINTENDO_TABLE size not match %02x %02x\n",length,wrq->u.data.length));
		return;
	}
	os_alloc_mem(pAd, (UCHAR **)&pset, sizeof(RT_NINTENDO_TABLE));
	if (pset == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR,("%s():alloc memory failed\n", __FUNCTION__));
		return;
	}
	Status = copy_from_user(pset, wrq->u.data.pointer, wrq->u.data.length);

	TEMP_SEM_LOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags)
	if(pset->number != NINTO_INFO.DS_TABLE.number)
	{
		DBGPRINT(RT_DEBUG_TRACE, (" NINTENDO_TABLE number not match %02x %02x\n",pset->number,NINTO_INFO.DS_TABLE.number));
		goto out;
	}
	for(loopi = 0; loopi < NINTO_INFO.DS_TABLE.number && loopi < NINTENDO_MAX_ENTRY; loopi++)
	{
		if(!memcmp(NINTO_INFO.DS_TABLE.entry[loopi].DS_Addr, pset->entry[loopi].DS_Addr, MAC_ADDR_LEN))
		{
			DBGPRINT(RT_DEBUG_TRACE, (" NINTENDO_TABLE entry[%d].registe %d \n",loopi,pset->entry[loopi].registe));
			NINTO_INFO.DS_TABLE.entry[loopi].registe = pset->entry[loopi].registe;
		}
	}
out:
	TEMP_SEM_UNLOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags);
	IrqFlags = 0;
	if(pset)
		os_free_mem(NULL, pset);
	return;
}

extern UCHAR CheckNINTENDO_TABLE(
	IN PRTMP_ADAPTER pAd, 
	PCHAR pDS_Ssid, 
	UCHAR DS_SsidLen, 
	PUCHAR pDS_Addr)
{
	int		loopi;
	UCHAR	ErrorAck = 1;
	ULONG				IrqFlags;
	PRT_NINTENDO_SSID	pNINTENDOSSID_REQ = (PRT_NINTENDO_SSID)pDS_Ssid;
	PRT_NINTENDO_SSID	pNINTENDOSSID_RSP = (PRT_NINTENDO_SSID)pAd->ApCfg.MBSSID[MAIN_MBSSID].Ssid;

	pAd->ApCfg.MBSSID[MAIN_MBSSID].SsidLen = MAX_LEN_OF_SSID;
	memset(pNINTENDOSSID_RSP, 0, MAX_LEN_OF_SSID);
	memcpy(pNINTENDOSSID_RSP->NINTENDOFixChar, NINTENDO_SSID_NAME, NINTENDO_SSID_NAME_LN);

	if (32 == DS_SsidLen)
	{
		if ((pNINTENDOSSID_REQ->zero1 == 0)
		&& (pNINTENDOSSID_REQ->ID == 0)
		&& (pNINTENDOSSID_REQ->zero2 == 0))
		{
			if (NINTENDO_PROBE_REQ_ON & pNINTENDOSSID_REQ->registe)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("DS login on\n"));
				AddNINTENDO_ENTRY(pAd, pNINTENDOSSID_REQ, pDS_Addr);
				pNINTENDOSSID_RSP->registe	|= NINTENDO_PROBE_RSP_ON;
				ErrorAck = 0;
			}
			else
			{
				DBGPRINT(RT_DEBUG_TRACE, ("DS login off\n"));
			}
			if (NINTENDO_PROBE_REQ_SIGNAL & pNINTENDOSSID_REQ->registe)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("DS SIGNAL\n"));
			}
		}
	}
	else
	{
		TEMP_SEM_LOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags)
		for(loopi = 0; loopi < NINTO_INFO.DS_TABLE.number && loopi < NINTENDO_MAX_ENTRY; loopi++)
		{
			if(!memcmp(pDS_Addr, NINTO_INFO.DS_TABLE.entry[loopi].DS_Addr, MAC_ADDR_LEN))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("CheckNINTENDO_TABLE DS_TABLE.entry[%d].registe %d\n",loopi,NINTO_INFO.DS_TABLE.entry[loopi].registe));
				if (0 != NINTO_INFO.DS_TABLE.entry[loopi].registe)
				{
					memcpy(pNINTENDOSSID_RSP->NICKname, pAd->NINTENDO_WEP_SEED, NINTENDO_SSID_NICKNAME_LN);
					pNINTENDOSSID_RSP->registe	|= NINTENDO_PROBE_RSP_ON;
					DBGPRINT(RT_DEBUG_TRACE, ("\n pAd->NINTENDO_WEP_KEY = "));
					for(loopi=0;loopi<NINTENDO_WEPKEY_LN;loopi++) 
						DBGPRINT(RT_DEBUG_TRACE, ("%02x", pAd->NINTENDO_WEP_KEY[loopi]));
					DBGPRINT(RT_DEBUG_TRACE, ("\n"));
					ErrorAck = 0;
				}
			}
		}
		TEMP_SEM_UNLOCK(&NINTO_INFO.NINTENDO_TABLE_Lock, IrqFlags);
		IrqFlags = 0;
	}
	return ErrorAck;	
}

int	Set_NintendiCapable_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
    RTMP_IOCTL_INPUT_STRUCT	wrq;

	wrq.u.data.pointer = arg;
	wrq.u.data.length = 2;
	RTMPIoctlNintendoCapable(pAd, &wrq);
	return TRUE;
}

INT	Set_NintendoGet_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
    RTMP_IOCTL_INPUT_STRUCT	wrq;

	wrq.u.data.pointer = arg;
	wrq.u.data.length = 2;
	RTMPIoctlNintendoGetTable(pAd, &wrq);
	return TRUE;
}

INT	Set_NintendoSet_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg)
{
    RTMP_IOCTL_INPUT_STRUCT	wrq;

	wrq.u.data.pointer = arg;
	wrq.u.data.length = 1;
	RTMPIoctlNintendoSetTable(pAd, &wrq);
	return TRUE;
}

#endif /* NINTENDO_AP */
