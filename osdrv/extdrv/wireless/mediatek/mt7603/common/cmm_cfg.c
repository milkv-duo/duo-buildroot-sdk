/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	cmm_cfg.c

    Abstract:
    Ralink WiFi Driver configuration related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/



#include "rt_config.h"

static BOOLEAN RT_isLegalCmdBeforeInfUp(RTMP_STRING *SetCmd);


INT ComputeChecksum(UINT PIN)
{
	INT digit_s;
    UINT accum = 0;

	PIN *= 10;
	accum += 3 * ((PIN / 10000000) % 10); 
	accum += 1 * ((PIN / 1000000) % 10); 
	accum += 3 * ((PIN / 100000) % 10); 
	accum += 1 * ((PIN / 10000) % 10); 
	accum += 3 * ((PIN / 1000) % 10); 
	accum += 1 * ((PIN / 100) % 10); 
	accum += 3 * ((PIN / 10) % 10); 

	digit_s = (accum % 10);
	return ((10 - digit_s) % 10);
} /* ComputeChecksum*/

UINT GenerateWpsPinCode(
	IN	PRTMP_ADAPTER	pAd,
    IN  BOOLEAN         bFromApcli,	
	IN	UCHAR			apidx)
{
	UCHAR	macAddr[MAC_ADDR_LEN];
	UINT 	iPin;
	UINT	checksum;
#ifdef WIDI_SUPPORT	
	BOOLEAN bGenWidiPIN = FALSE;
#endif /* WIDI_SUPPORT */

	NdisZeroMemory(macAddr, MAC_ADDR_LEN);

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef APCLI_SUPPORT
	    if (bFromApcli)
	        NdisMoveMemory(&macAddr[0], pAd->ApCfg.ApCliTab[apidx].wdev.if_addr, MAC_ADDR_LEN);
	    else
#endif /* APCLI_SUPPORT */
		NdisMoveMemory(&macAddr[0], pAd->ApCfg.MBSSID[apidx].wdev.if_addr, MAC_ADDR_LEN);
	}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		NdisMoveMemory(&macAddr[0], pAd->CurrentAddress, MAC_ADDR_LEN);
#endif /* CONFIG_STA_SUPPORT */

#ifdef P2P_SUPPORT
	if (apidx >= MIN_NET_DEVICE_FOR_P2P_GO)
		NdisMoveMemory(&macAddr[0], pAd->P2PCurrentAddress, MAC_ADDR_LEN);

	if (bFromApcli)
	{
		APCLI_MR_APIDX_SANITY_CHECK(apidx);
		NdisMoveMemory(&macAddr[0], pAd->ApCfg.ApCliTab[apidx].wdev.if_addr, MAC_ADDR_LEN);
	}
#endif /* P2P_SUPPORT */
	iPin = macAddr[3] * 256 * 256 + macAddr[4] * 256 + macAddr[5];

	iPin = iPin % 10000000;

#ifdef WIDI_SUPPORT
#ifdef P2P_SUPPORT
	if (pAd->P2pCfg.bWIDI &&
		((apidx >= MIN_NET_DEVICE_FOR_P2P_GO) || bFromApcli))
	{
			bGenWidiPIN = TRUE;
	}
	else
#endif /* P2P_SUPPORT */
	if (pAd->StaCfg.bWIDI && (pAd->OpMode == OPMODE_STA))
		bGenWidiPIN = TRUE;
	
	if (bGenWidiPIN)
		iPin = ((iPin / 1000) * 1000);
#endif /* WIDI_SUPPORT */
	
	checksum = ComputeChecksum( iPin );
	iPin = iPin*10 + checksum;

	return iPin;
}


static char *phy_mode_str[]={"CCK", "OFDM", "HTMIX", "GF", "VHT"};
char* get_phymode_str(int Mode)
{
	if (Mode >= MODE_CCK && Mode <= MODE_VHT)
		return phy_mode_str[Mode];
	else
		return "N/A";
}


static UCHAR *phy_bw_str[] = {"20M", "40M", "80M", "BOTH", "10M"}; 
char* get_bw_str(int bandwidth)
{
	if (bandwidth >= BW_20 && bandwidth <= BW_10)
		return phy_bw_str[bandwidth];
	else
		return "N/A";
}


/* 
    ==========================================================================
    Description:
        Set Country Region to pAd->CommonCfg.CountryRegion.
        This command will not work, if the field of CountryRegion in eeprom is programmed.
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT RT_CfgSetCountryRegion(RTMP_ADAPTER *pAd, RTMP_STRING *arg, INT band)
{
	LONG region;
	UCHAR *pCountryRegion;
	
	region = simple_strtol(arg, 0, 10);

	if (band == BAND_24G)
		pCountryRegion = &pAd->CommonCfg.CountryRegion;
	else
		pCountryRegion = &pAd->CommonCfg.CountryRegionForABand;
	
    /*
               1. If this value is set before interface up, do not reject this value.
               2. Country can be set only when EEPROM not programmed
    */
    if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE) && (*pCountryRegion & EEPROM_IS_PROGRAMMED))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("CfgSetCountryRegion():CountryRegion in eeprom was programmed\n"));
		return FALSE;
	}

	if((region >= 0) && 
	   (((band == BAND_24G) &&((region <= REGION_MAXIMUM_BG_BAND) || 
	   (region == REGION_31_BG_BAND) || (region == REGION_32_BG_BAND) || (region == REGION_33_BG_BAND) )) || 
		((band == BAND_5G) && (region <= REGION_MAXIMUM_A_BAND)) ||
		((region >= REGION_BAND_START) && (region <= REGION_BAND_END)))
	  )
	{
		*pCountryRegion= (UCHAR) region;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("CfgSetCountryRegion():region(%ld) out of range!\n", region));
		return FALSE;
	}

	return TRUE;
	
}


static UCHAR CFG_WMODE_MAP[]={
	PHY_11BG_MIXED, (WMODE_B | WMODE_G), /* 0 => B/G mixed */
	PHY_11B, (WMODE_B), /* 1 => B only */
	PHY_11A, (WMODE_A), /* 2 => A only */
	PHY_11ABG_MIXED, (WMODE_A | WMODE_B | WMODE_G), /* 3 => A/B/G mixed */
	PHY_11G, WMODE_G, /* 4 => G only */
	PHY_11ABGN_MIXED, (WMODE_B | WMODE_G | WMODE_GN | WMODE_A | WMODE_AN), /* 5 => A/B/G/GN/AN mixed */
	PHY_11N_2_4G, (WMODE_GN), /* 6 => N in 2.4G band only */
	PHY_11GN_MIXED, (WMODE_G | WMODE_GN), /* 7 => G/GN, i.e., no CCK mode */
	PHY_11AN_MIXED, (WMODE_A | WMODE_AN), /* 8 => A/N in 5 band */
	PHY_11BGN_MIXED, (WMODE_B | WMODE_G | WMODE_GN), /* 9 => B/G/GN mode*/
	PHY_11AGN_MIXED, (WMODE_G | WMODE_GN | WMODE_A | WMODE_AN), /* 10 => A/AN/G/GN mode, not support B mode */
	PHY_11N_5G, (WMODE_AN), /* 11 => only N in 5G band */
#ifdef DOT11_VHT_AC
	PHY_11VHT_N_ABG_MIXED, (WMODE_B | WMODE_G | WMODE_GN |WMODE_A | WMODE_AN | WMODE_AC), /* 12 => B/G/GN/A/AN/AC mixed*/
	PHY_11VHT_N_AG_MIXED, (WMODE_G | WMODE_GN |WMODE_A | WMODE_AN | WMODE_AC), /* 13 => G/GN/A/AN/AC mixed , no B mode */
	PHY_11VHT_N_A_MIXED, (WMODE_A | WMODE_AN | WMODE_AC), /* 14 => A/AC/AN mixed */
	PHY_11VHT_N_MIXED, (WMODE_AN | WMODE_AC), /* 15 => AC/AN mixed, but no A mode */
#endif /* DOT11_VHT_AC */
	PHY_MODE_MAX, WMODE_INVALID /* default phy mode if not match */
};


static RTMP_STRING *BAND_STR[] = {"Invalid", "2.4G", "5G", "2.4G/5G"};
static RTMP_STRING *WMODE_STR[]= {"", "A", "B", "G", "gN", "aN", "AC"};

UCHAR *wmode_2_str(UCHAR wmode)
{
	UCHAR *str;
	INT idx, pos, max_len;

	max_len = WMODE_COMP * 3;
	if (os_alloc_mem(NULL, &str, max_len) == NDIS_STATUS_SUCCESS)
	{
		NdisZeroMemory(str, max_len);
		pos = 0;
		for (idx = 0; idx < WMODE_COMP; idx++)
		{
			if (wmode & (1 << idx)) {
				if ((strlen(str) +  strlen(WMODE_STR[idx + 1])) >= (max_len - 1))
					break;
				if (strlen(str)) {
					NdisMoveMemory(&str[pos], "/", 1);
					pos++;
				}
				NdisMoveMemory(&str[pos], WMODE_STR[idx + 1], strlen(WMODE_STR[idx + 1]));
				pos += strlen(WMODE_STR[idx + 1]);
			}
			if (strlen(str) >= max_len)
				break;
		}

		return str;
	}
	else
		return NULL;
}


RT_802_11_PHY_MODE wmode_2_cfgmode(UCHAR wmode)
{
	INT i, mode_cnt = sizeof(CFG_WMODE_MAP) / (sizeof(UCHAR) * 2);

	for (i = 1; i < mode_cnt; i+=2)
	{	
		if (CFG_WMODE_MAP[i] == wmode)
			return CFG_WMODE_MAP[i - 1];
	}

	DBGPRINT(RT_DEBUG_ERROR, ("%s(): Cannot get cfgmode by wmode(%x)\n", 
				__FUNCTION__, wmode));

	return 0;
}


UCHAR cfgmode_2_wmode(UCHAR cfg_mode)
{
	DBGPRINT(RT_DEBUG_OFF, ("cfg_mode=%d\n", cfg_mode));
	if (cfg_mode >= PHY_MODE_MAX)
		cfg_mode =  PHY_MODE_MAX;
	
	return CFG_WMODE_MAP[cfg_mode * 2 + 1];
}

#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
static BOOLEAN wmode_valid(RTMP_ADAPTER *pAd, enum WIFI_MODE wmode)
{
	if ((WMODE_CAP_5G(wmode) && (!PHY_CAP_5G(pAd->chipCap.phy_caps))) ||
		(WMODE_CAP_2G(wmode) && (!PHY_CAP_2G(pAd->chipCap.phy_caps))) ||
		(WMODE_CAP_N(wmode) && RTMP_TEST_MORE_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DOT_11N))
	)
		return FALSE;
	else
		return TRUE;
}
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

static BOOLEAN wmode_valid_and_correct(RTMP_ADAPTER *pAd, UCHAR* wmode)
{
	BOOLEAN ret = TRUE;

	if (*wmode == WMODE_INVALID)
		*wmode = (WMODE_B | WMODE_G | WMODE_GN |WMODE_A | WMODE_AN | WMODE_AC);

	while(1)
	{
		if (WMODE_CAP_5G(*wmode) && (!PHY_CAP_5G(pAd->chipCap.phy_caps)))
		{
			*wmode = *wmode & ~(WMODE_A | WMODE_AN | WMODE_AC);
		}
		else if (WMODE_CAP_2G(*wmode) && (!PHY_CAP_2G(pAd->chipCap.phy_caps)))
		{
			*wmode = *wmode & ~(WMODE_B | WMODE_G | WMODE_GN);
		}
		else if (WMODE_CAP_N(*wmode) && ((!PHY_CAP_N(pAd->chipCap.phy_caps)) || RTMP_TEST_MORE_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DOT_11N)))
		{
			*wmode = *wmode & ~(WMODE_GN | WMODE_AN);
		}
		else if (WMODE_CAP_AC(*wmode) && (!PHY_CAP_AC(pAd->chipCap.phy_caps)))
		{
			*wmode = *wmode & ~(WMODE_AC);
		}

		if ( *wmode == 0 )
		{
			*wmode = (WMODE_B | WMODE_G | WMODE_GN |WMODE_A | WMODE_AN | WMODE_AC);
			break;
		}
		else
			break;
	}

	return ret;
}


BOOLEAN wmode_band_equal(UCHAR smode, UCHAR tmode)
{
	BOOLEAN eq = FALSE;
	UCHAR *str1, *str2;
	
	if ((WMODE_CAP_5G(smode) == WMODE_CAP_5G(tmode)) &&
		(WMODE_CAP_2G(smode) == WMODE_CAP_2G(tmode)))
		eq = TRUE; 

	str1 = wmode_2_str(smode);
	str2 = wmode_2_str(tmode);
	if (str1 && str2)
	{
		DBGPRINT(RT_DEBUG_TRACE,
			("Old WirelessMode:%s(0x%x), "
			 "New WirelessMode:%s(0x%x)!\n",
			str1, smode, str2, tmode));
	}
	if (str1)
		os_free_mem(NULL, str1);
	if (str2)
		os_free_mem(NULL, str2);
		
	return eq;
}


/* 
    ==========================================================================
    Description:
        Set Wireless Mode
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT RT_CfgSetWirelessMode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	LONG cfg_mode;
	UCHAR wmode, *mode_str;


	cfg_mode = simple_strtol(arg, 0, 10);

	/* check if chip support 5G band when WirelessMode is 5G band */
	wmode = cfgmode_2_wmode((UCHAR)cfg_mode);
	if (!wmode_valid_and_correct(pAd, &wmode)) {
		DBGPRINT(RT_DEBUG_ERROR,
				("%s(): Invalid wireless mode(%ld, wmode=0x%x), ChipCap(%s)\n",
				__FUNCTION__, cfg_mode, wmode,
				BAND_STR[pAd->chipCap.phy_caps & 0x3]));
		return FALSE;
	}

	if (wmode_band_equal(pAd->CommonCfg.PhyMode, wmode) == TRUE)
		DBGPRINT(RT_DEBUG_OFF, ("wmode_band_equal(): Band Equal!\n"));
	else
		DBGPRINT(RT_DEBUG_OFF, ("wmode_band_equal(): Band Not Equal!\n"));
	
	pAd->CommonCfg.PhyMode = wmode;
	pAd->CommonCfg.cfg_wmode = wmode;

	mode_str = wmode_2_str(wmode);
	if (mode_str)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): Set WMODE=%s(0x%x)\n",
				__FUNCTION__, mode_str, wmode));
		os_free_mem(NULL, mode_str);
	}

	return TRUE;
}


/* maybe can be moved to GPL code, ap_mbss.c, but the code will be open */
#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
static UCHAR RT_CfgMbssWirelessModeMaxGet(RTMP_ADAPTER *pAd)
{
	UCHAR wmode = 0, *mode_str;
	INT idx;
	struct wifi_dev *wdev;

	for(idx = 0; idx < pAd->ApCfg.BssidNum; idx++) {
		wdev = &pAd->ApCfg.MBSSID[idx].wdev;
		mode_str = wmode_2_str(wdev->PhyMode);
		if (mode_str)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s(BSS%d): wmode=%s(0x%x)\n",
					__FUNCTION__, idx, mode_str, wdev->PhyMode));
			os_free_mem(pAd, mode_str);
		}
		wmode |= wdev->PhyMode;
	}

	mode_str = wmode_2_str(wmode);
	if (mode_str)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): Combined WirelessMode = %s(0x%x)\n", 
					__FUNCTION__, mode_str, wmode));
		os_free_mem(pAd, mode_str);
	}
	return wmode;
}


/* 
    ==========================================================================
    Description:
        Set Wireless Mode for MBSS
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT RT_CfgSetMbssWirelessMode(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT cfg_mode;
	UCHAR wmode;


	cfg_mode = simple_strtol(arg, 0, 10);

	wmode = cfgmode_2_wmode((UCHAR)cfg_mode);
	if ((wmode == WMODE_INVALID) || (!wmode_valid(pAd, wmode))) {
		DBGPRINT(RT_DEBUG_ERROR,
				("%s(): Invalid wireless mode(%d, wmode=0x%x), ChipCap(%s)\n",
				__FUNCTION__, cfg_mode, wmode,
				BAND_STR[pAd->chipCap.phy_caps & 0x3]));
		return FALSE;
	}
	
	if (WMODE_CAP_5G(wmode) && WMODE_CAP_2G(wmode))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("AP cannot support 2.4G/5G band mxied mode!\n"));
		return FALSE;
	}

#ifdef MT76x0
	if (IS_MT7610E(pAd) && WMODE_CAP_2G(wmode))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("AP doesn't support 2.4G band! (RfIcType=%d)\n", pAd->RfIcType));
		return FALSE;
	}
#endif /* MT76x0 */

	if (pAd->ApCfg.BssidNum > 1)
	{
		/* pAd->CommonCfg.PhyMode = maximum capability of all MBSS */
		if (wmode_band_equal(pAd->CommonCfg.PhyMode, wmode) == TRUE)
		{
			wmode = RT_CfgMbssWirelessModeMaxGet(pAd);

			DBGPRINT(RT_DEBUG_TRACE,
					("mbss> Maximum phy mode = %d!\n", wmode));
		}
		else
		{
			UINT32 IdBss;

			/* replace all phy mode with the one with different band */
			DBGPRINT(RT_DEBUG_TRACE,
					("mbss> Different band with the current one!\n"));
			DBGPRINT(RT_DEBUG_TRACE,
					("mbss> Reset band of all BSS to the new one!\n"));

			for(IdBss=0; IdBss<pAd->ApCfg.BssidNum; IdBss++)
				pAd->ApCfg.MBSSID[IdBss].wdev.PhyMode = wmode;
		}
	}

	pAd->CommonCfg.PhyMode = wmode;
	pAd->CommonCfg.cfg_wmode = wmode;
	return TRUE;
}
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


static BOOLEAN RT_isLegalCmdBeforeInfUp(RTMP_STRING *SetCmd)
{
		BOOLEAN TestFlag;
		TestFlag =	!strcmp(SetCmd, "Debug") ||
#ifdef CONFIG_APSTA_MIXED_SUPPORT
					!strcmp(SetCmd, "OpMode") ||
#endif /* CONFIG_APSTA_MIXED_SUPPORT */
#ifdef EXT_BUILD_CHANNEL_LIST
					!strcmp(SetCmd, "CountryCode") ||
					!strcmp(SetCmd, "DfsType") ||
					!strcmp(SetCmd, "ChannelListAdd") ||
					!strcmp(SetCmd, "ChannelListShow") ||
					!strcmp(SetCmd, "ChannelListDel") ||
#endif /* EXT_BUILD_CHANNEL_LIST */
#ifdef SINGLE_SKU
					!strcmp(SetCmd, "ModuleTxpower") ||
#endif /* SINGLE_SKU */
					FALSE; /* default */
       return TestFlag;
}


INT RT_CfgSetShortSlot(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	LONG ShortSlot;

	ShortSlot = simple_strtol(arg, 0, 10);

	if (ShortSlot == 1)
		pAd->CommonCfg.bUseShortSlotTime = TRUE;
	else if (ShortSlot == 0)
		pAd->CommonCfg.bUseShortSlotTime = FALSE;
	else
		return FALSE;  /*Invalid argument */

	return TRUE;
}


/* 
    ==========================================================================
    Description:
        Set WEP KEY base on KeyIdx
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT	RT_CfgSetWepKey(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_STRING *keyString,
	IN	CIPHER_KEY		*pSharedKey,
	IN	INT				keyIdx)
{
	INT				KeyLen;
	INT				i;
	/*UCHAR			CipherAlg = CIPHER_NONE;*/
	BOOLEAN			bKeyIsHex = FALSE;

	/* TODO: Shall we do memset for the original key info??*/
	memset(pSharedKey, 0, sizeof(CIPHER_KEY));
	KeyLen = strlen(keyString);
	switch (KeyLen)
	{
		case 5: /*wep 40 Ascii type*/
		case 13: /*wep 104 Ascii type*/
#ifdef MT_MAC
		case 16: /*wep 128 Ascii type*/
#endif			
			bKeyIsHex = FALSE;
			pSharedKey->KeyLen = (UCHAR)KeyLen;
			NdisMoveMemory(pSharedKey->Key, keyString, KeyLen);
			break;
			
		case 10: /*wep 40 Hex type*/
		case 26: /*wep 104 Hex type*/
#ifdef MT_MAC
		case 32: /*wep 128 Hex type*/
#endif			
			for(i=0; i < KeyLen; i++)
			{
				if( !isxdigit(*(keyString+i)) )
					return FALSE;  /*Not Hex value;*/
			}
			bKeyIsHex = TRUE;
			pSharedKey->KeyLen = (UCHAR)(KeyLen/2);
			AtoH(keyString, pSharedKey->Key, pSharedKey->KeyLen);
			break;
			
		default: /*Invalid argument */
			DBGPRINT(RT_DEBUG_TRACE, ("RT_CfgSetWepKey(keyIdx=%d):Invalid argument (arg=%s)\n", keyIdx, keyString));
			return FALSE;
	}

	pSharedKey->CipherAlg = ((KeyLen % 5) ? CIPHER_WEP128 : CIPHER_WEP64);
#ifdef MT_MAC
	if (KeyLen == 32)
		pSharedKey->CipherAlg = CIPHER_WEP152;
#endif		
	DBGPRINT(RT_DEBUG_TRACE, ("RT_CfgSetWepKey:(KeyIdx=%d,type=%s, Alg=%s)\n", 
						keyIdx, (bKeyIsHex == FALSE ? "Ascii" : "Hex"), CipherName[pSharedKey->CipherAlg]));

	return TRUE;
}


/* 
    ==========================================================================
    Description:
        Set WPA PSK key

    Arguments:
        pAdapter	Pointer to our adapter
        keyString	WPA pre-shared key string
        pHashStr	String used for password hash function
        hashStrLen	Lenght of the hash string
        pPMKBuf		Output buffer of WPAPSK key

    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
*/
INT RT_CfgSetWPAPSKKey(
	IN RTMP_ADAPTER	*pAd, 
	IN RTMP_STRING *keyString,
	IN INT			keyStringLen,
	IN UCHAR		*pHashStr,
	IN INT			hashStrLen,
	OUT PUCHAR		pPMKBuf)
{
	UCHAR keyMaterial[40];

	if ((keyStringLen < 8) || (keyStringLen > 64))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("WPAPSK Key length(%d) error, required 8 ~ 64 characters!(keyStr=%s)\n", 
									keyStringLen, keyString));
		return FALSE;
	}

	NdisZeroMemory(pPMKBuf, 32);
	if (keyStringLen == 64)
	{
	    AtoH(keyString, pPMKBuf, 32);
	}
	else
	{
	    RtmpPasswordHash(keyString, pHashStr, hashStrLen, keyMaterial);
	    NdisMoveMemory(pPMKBuf, keyMaterial, 32);		
	}

	return TRUE;
}


INT	RT_CfgSetFixedTxPhyMode(RTMP_STRING *arg)
{
	INT fix_tx_mode = FIXED_TXMODE_HT;
	ULONG value;


	if (rtstrcasecmp(arg, "OFDM") == TRUE)
		fix_tx_mode = FIXED_TXMODE_OFDM;
	else if (rtstrcasecmp(arg, "CCK") == TRUE)
	    fix_tx_mode = FIXED_TXMODE_CCK;
	else if (rtstrcasecmp(arg, "HT") == TRUE)
	    fix_tx_mode = FIXED_TXMODE_HT;
	else if (rtstrcasecmp(arg, "VHT") == TRUE)
		fix_tx_mode = FIXED_TXMODE_VHT;
	else
	{
		value = simple_strtol(arg, 0, 10);
		switch (value)
		{
			case FIXED_TXMODE_CCK:
			case FIXED_TXMODE_OFDM:
			case FIXED_TXMODE_HT:
			case FIXED_TXMODE_VHT:
				fix_tx_mode = value;
			default:
				fix_tx_mode = FIXED_TXMODE_HT;
		}
	}

	return fix_tx_mode;
					
}	


INT	RT_CfgSetMacAddress(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT	i, mac_len;
	
	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */
	mac_len = strlen(arg);
	if(mac_len != 17)  
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : invalid length (%d)\n", __FUNCTION__, mac_len));
		return FALSE;
	}

	if(strcmp(arg, "00:00:00:00:00:00") == 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : invalid mac setting \n", __FUNCTION__));
		return FALSE;
	}

	for (i = 0; i < MAC_ADDR_LEN; i++)
	{
		AtoH(arg, &pAd->CurrentAddress[i], 1);
		arg = arg + 3;
	}	

	pAd->bLocalAdminMAC = TRUE;
	return TRUE;
}


INT	RT_CfgSetTxMCSProc(RTMP_STRING *arg, BOOLEAN *pAutoRate)
{
	INT	Value = simple_strtol(arg, 0, 10);
	INT	TxMcs;
	
	if ((Value >= 0 && Value <= 23) || (Value == 32)) /* 3*3*/
	{
		TxMcs = Value;
		*pAutoRate = FALSE;
	}
	else
	{		
		TxMcs = MCS_AUTO;
		*pAutoRate = TRUE;
	}

	return TxMcs;

}


INT	RT_CfgSetAutoFallBack(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR AutoFallBack = (UCHAR)simple_strtol(arg, 0, 10);

	if (AutoFallBack)
		AutoFallBack = TRUE;
	else
		AutoFallBack = FALSE;

	AsicSetAutoFallBack(pAd, (AutoFallBack) ? TRUE : FALSE);
	DBGPRINT(RT_DEBUG_TRACE, ("RT_CfgSetAutoFallBack::(AutoFallBack=%d)\n", AutoFallBack));
	return TRUE;
}


#ifdef WSC_INCLUDED
INT	RT_CfgSetWscPinCode(
	IN RTMP_ADAPTER *pAd,
	IN RTMP_STRING *pPinCodeStr,
	OUT PWSC_CTRL   pWscControl)
{
	UINT pinCode;

	pinCode = (UINT) simple_strtol(pPinCodeStr, 0, 10); /* When PinCode is 03571361, return value is 3571361.*/
	if (strlen(pPinCodeStr) == 4)
	{
		pWscControl->WscEnrolleePinCode = pinCode;
		pWscControl->WscEnrolleePinCodeLen = 4;
	}
	else if ( ValidateChecksum(pinCode) )
	{
		pWscControl->WscEnrolleePinCode = pinCode;
		pWscControl->WscEnrolleePinCodeLen = 8;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RT_CfgSetWscPinCode(): invalid Wsc PinCode (%d)\n", pinCode));
		return FALSE;
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("RT_CfgSetWscPinCode():Wsc PinCode=%d\n", pinCode));
	
	return TRUE;
	
}
#endif /* WSC_INCLUDED */

/*
========================================================================
Routine Description:
	Handler for CMD_RTPRIV_IOCTL_STA_SIOCGIWNAME.

Arguments:
	pAd				- WLAN control block pointer
	*pData			- the communication data pointer
	Data			- the communication data

Return Value:
	NDIS_STATUS_SUCCESS or NDIS_STATUS_FAILURE

Note:
========================================================================
*/
INT RtmpIoctl_rt_ioctl_giwname(
	IN	RTMP_ADAPTER			*pAd,
	IN	VOID					*pData,
	IN	ULONG					Data)
{
#ifdef P2P_SUPPORT
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif /* P2P_SUPPORT */
	UCHAR CurOpMode = OPMODE_AP;

	if (CurOpMode == OPMODE_AP)
	{
	/* For SIOCGIWNAME, struct iwreq_data.name's size is IFNAMSIZ (16). */
#ifdef P2P_SUPPORT
		if (pObj->ioctl_if_type == INT_P2P)
		{
			if (P2P_CLI_ON(pAd))
				strncpy(pData, "Ralink P2P Cli", IFNAMSIZ);
			else if (P2P_GO_ON(pAd))
				strncpy(pData, "Ralink P2P GO", IFNAMSIZ);
			else
				strncpy(pData, "Ralink P2P", IFNAMSIZ);
		}
		else
#endif /* P2P_SUPPORT */
		strncpy(pData, "RTWIFI SoftAP", IFNAMSIZ);
	}

	return NDIS_STATUS_SUCCESS;
}


INT RTMP_COM_IoctlHandle(
	IN	VOID					*pAdSrc,
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq,
	IN	INT						cmd,
	IN	USHORT					subcmd,
	IN	VOID					*pData,
	IN	ULONG					Data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;
	INT Status = NDIS_STATUS_SUCCESS, i;


	pObj = pObj; /* avoid compile warning */

	switch(cmd)
	{
		case CMD_RTPRIV_IOCTL_NETDEV_GET:
		/* get main net_dev */
		{
			VOID **ppNetDev = (VOID **)pData;
			*ppNetDev = (VOID *)(pAd->net_dev);
		}
			break;

		case CMD_RTPRIV_IOCTL_NETDEV_SET:
			{
				struct wifi_dev *wdev = NULL;
				/* set main net_dev */
				pAd->net_dev = pData;

#ifdef CONFIG_AP_SUPPORT
				if (pAd->OpMode == OPMODE_AP) {
					pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev.if_dev = (void *)pData;
					pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev.func_dev = (void *)&pAd->ApCfg.MBSSID[MAIN_MBSSID];
					pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev.func_idx = MAIN_MBSSID;
					pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev.sys_handle = (void *)pAd;
					RTMP_OS_NETDEV_SET_WDEV(pData, &pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev);
					wdev = &pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev;
				}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
				if (pAd->OpMode == OPMODE_STA) {
					pAd->StaCfg.wdev.if_dev = pData;
					pAd->StaCfg.wdev.func_dev = (void *)&pAd->StaCfg;
					pAd->StaCfg.wdev.func_idx = 0;
					pAd->StaCfg.wdev.sys_handle = (void *)pAd;
					RTMP_OS_NETDEV_SET_WDEV(pData, &pAd->StaCfg.wdev);
					wdev = &pAd->StaCfg.wdev;
				}
#endif /* CONFIG_STA_SUPPORT */
				if (wdev) {
					if (rtmp_wdev_idx_reg(pAd, wdev) < 0) {
						DBGPRINT(RT_DEBUG_ERROR, ("Assign wdev idx for %s failed, free net device!\n",
								RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev)));
						RtmpOSNetDevFree(pAd->net_dev);
					}
				}
				break;
			}
		case CMD_RTPRIV_IOCTL_OPMODE_GET:
		/* get Operation Mode */
			*(ULONG *)pData = pAd->OpMode;
			break;


		case CMD_RTPRIV_IOCTL_TASK_LIST_GET:
		/* get all Tasks */
		{
			RT_CMD_WAIT_QUEUE_LIST *pList = (RT_CMD_WAIT_QUEUE_LIST *)pData;

			pList->pMlmeTask = &pAd->mlmeTask;
#ifdef RTMP_TIMER_TASK_SUPPORT
			pList->pTimerTask = &pAd->timerTask;
#endif /* RTMP_TIMER_TASK_SUPPORT */
			pList->pCmdQTask = &pAd->cmdQTask;
#ifdef WSC_INCLUDED
			pList->pWscTask = &pAd->wscTask;
#endif /* WSC_INCLUDED */
		}
			break;

#ifdef RTMP_MAC_PCI
		case CMD_RTPRIV_IOCTL_IRQ_INIT:
			/* init IRQ */
			rtmp_irq_init(pAd);
			break;
#endif /* RTMP_MAC_PCI */

		case CMD_RTPRIV_IOCTL_IRQ_RELEASE:
			/* release IRQ */
			RTMP_OS_IRQ_RELEASE(pAd, pAd->net_dev);
			break;

#ifdef RTMP_MAC_PCI
		case CMD_RTPRIV_IOCTL_MSI_ENABLE:
			/* enable MSI */
			RTMP_MSI_ENABLE(pAd);
			*(ULONG **)pData = (ULONG *)(pObj->pci_dev);
			break;
#endif /* RTMP_MAC_PCI */

		case CMD_RTPRIV_IOCTL_NIC_NOT_EXIST:
			/* set driver state to fRTMP_ADAPTER_NIC_NOT_EXIST */
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST);
			break;

		case CMD_RTPRIV_IOCTL_MCU_SLEEP_CLEAR:
			RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);
			break;

#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND
                case CMD_RTPRIV_IOCTL_USB_DEV_GET:
                /* get USB DEV */
                {
                        VOID **ppUsb_Dev = (VOID **)pData;
                        *ppUsb_Dev = (VOID *)(pObj->pUsb_Dev);
                }
                        break;

                case CMD_RTPRIV_IOCTL_USB_INTF_GET:
                /* get USB INTF */
                {
                        VOID **ppINTF = (VOID **)pData;
                        *ppINTF = (VOID *)(pObj->intf);
                }
                        break;

		case CMD_RTPRIV_IOCTL_ADAPTER_SUSPEND_SET:
		/* set driver state to fRTMP_ADAPTER_SUSPEND */
			RTMP_SET_FLAG(pAd,fRTMP_ADAPTER_SUSPEND);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_SUSPEND_CLEAR:
		/* clear driver state to fRTMP_ADAPTER_SUSPEND */
			RTMP_CLEAR_FLAG(pAd,fRTMP_ADAPTER_SUSPEND);
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_MCU_SEND_IN_BAND_CMD);
			RTMP_CLEAR_PSFLAG(pAd, fRTMP_PS_MCU_SLEEP);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_SEND_DISSASSOCIATE:
		/* clear driver state to fRTMP_ADAPTER_SUSPEND */
			if (INFRA_ON(pAd) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
			{
				MLME_DISASSOC_REQ_STRUCT	DisReq;
				MLME_QUEUE_ELEM *MsgElem;
				os_alloc_mem(NULL, (UCHAR **)&MsgElem, sizeof(MLME_QUEUE_ELEM));
				if (MsgElem)
				{
					COPY_MAC_ADDR(DisReq.Addr, pAd->CommonCfg.Bssid);
					DisReq.Reason =  REASON_DEAUTH_STA_LEAVING;
					MsgElem->Machine = ASSOC_STATE_MACHINE;
					MsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
					MsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
					NdisMoveMemory(MsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));
					/* Prevent to connect AP again in STAMlmePeriodicExec*/
					pAd->MlmeAux.AutoReconnectSsidLen= 32;
					NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);
					pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_DISASSOC;
					MlmeDisassocReqAction(pAd, MsgElem);
					os_free_mem(NULL, MsgElem);
				}
				/*				RtmpusecDelay(1000);*/
				RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CGIWAP, -1, NULL, NULL, 0);
			}
			break;
			
		case CMD_RTPRIV_IOCTL_ADAPTER_SUSPEND_TEST:
		/* test driver state to fRTMP_ADAPTER_SUSPEND */
			*(UCHAR *)pData = RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_SUSPEND);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_IDLE_RADIO_OFF_TEST:
		/* test driver state to fRTMP_ADAPTER_IDLE_RADIO_OFF */
			*(UCHAR *)pData = RTMP_TEST_FLAG(pAd,fRTMP_ADAPTER_IDLE_RADIO_OFF);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_USB_ASICRADIO_OFF:
			ASIC_RADIO_OFF(pAd, SUSPEND_RADIO_OFF);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_USB_ASICRADIO_ON:
			ASIC_RADIO_ON(pAd, RESUME_RADIO_ON);
			break;

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */

#ifdef MT_WOW_SUPPORT
		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_WOW_STATUS:
			*(UCHAR *)pData = (UCHAR)pAd->WOW_Cfg.bEnable;
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_WOW_ENABLE:
			pAd->PM_FlgSuspend = 1;			
			
			if ((pAd->WOW_Cfg.bEnable) && INFRA_ON(pAd) && (!pAd->WOW_Cfg.bWoWRunning)){				
				if(pAd->CommonCfg.Channel == pAd->CommonCfg.CentralChannel){
					bbp_set_bw(pAd,BW_20);
					AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
					}
				else if(pAd->CommonCfg.Channel != pAd->CommonCfg.CentralChannel){
					bbp_set_bw(pAd,BW_40);
					AsicSwitchChannel(pAd, pAd->CommonCfg.CentralChannel, FALSE);
					}
				DBGPRINT(RT_DEBUG_OFF,("Enter WoW, BW=%d Channel=%d Central Channel=%d\n",
				pAd->CommonCfg.BBPCurrentBW,pAd->CommonCfg.Channel,pAd->CommonCfg.CentralChannel));
							
				pAd->WOW_Cfg.bWoWRunning = TRUE;			
				DBGPRINT(RT_DEBUG_OFF,("\x1b[31mEnter WoW!!!\x1b[m\n"));			
				ASIC_WOW_ENABLE(pAd);							
				CmdExtPmStateCtrl(pAd, BSSID_WCID, PM4, ENTER_PM_STATE);			
				}
			
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_WOW_DISABLE:
			pAd->PM_FlgSuspend = 0;

			if (pAd->WOW_Cfg.bWoWRunning){
				pAd->WOW_Cfg.bUpdateSeqFromWoWResume = TRUE;
				ASIC_WOW_DISABLE(pAd);
				CmdExtPmStateCtrl(pAd, BSSID_WCID, PM4, EXIT_PM_STATE); 
				pAd->WOW_Cfg.bWoWRunning = FALSE;
			}
			break;
#endif
#endif /* CONFIG_PM */	

		case CMD_RTPRIV_IOCTL_AP_BSSID_GET:
			if (pAd->StaCfg.wdev.PortSecured == WPA_802_1X_PORT_NOT_SECURED)
				NdisCopyMemory(pData, pAd->MlmeAux.Bssid, 6);
			else
				return NDIS_STATUS_FAILURE;
			break;

#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND
		case CMD_RTPRIV_IOCTL_ADAPTER_SUSPEND_SET:
		/* set driver state to fRTMP_ADAPTER_SUSPEND */
			RTMP_SET_FLAG(pAd,fRTMP_ADAPTER_SUSPEND);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_SUSPEND_CLEAR:
		/* clear driver state to fRTMP_ADAPTER_SUSPEND */
			RTMP_CLEAR_FLAG(pAd,fRTMP_ADAPTER_SUSPEND);
			break;
#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */

		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_USB_ASICRADIO_OFF:
		/* RT28xxUsbAsicRadioOff */
			//RT28xxUsbAsicRadioOff(pAd);
			ASIC_RADIO_OFF(pAd, SUSPEND_RADIO_OFF);
			break;

		case CMD_RTPRIV_IOCTL_ADAPTER_RT28XX_USB_ASICRADIO_ON:
		/* RT28xxUsbAsicRadioOn */
			//RT28xxUsbAsicRadioOn(pAd);
			ASIC_RADIO_ON(pAd, RESUME_RADIO_ON);
			break;
#endif /* CONFIG_STA_SUPPORT */

		case CMD_RTPRIV_IOCTL_SANITY_CHECK:
		/* sanity check before IOCTL */
			if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
#ifdef IFUP_IN_PROBE
			|| (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS))
			|| (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
			|| (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
#endif /* IFUP_IN_PROBE */
			)
			{
				if(pData == NULL ||	RT_isLegalCmdBeforeInfUp((RTMP_STRING *) pData) == FALSE)
				return NDIS_STATUS_FAILURE;
			}
			break;

		case CMD_RTPRIV_IOCTL_SIOCGIWFREQ:
		/* get channel number */
			*(ULONG *)pData = pAd->CommonCfg.Channel;
			break;
#ifdef CONFIG_SNIFFER_SUPPORT
		case CMD_RTPRIV_IOCTL_SNIFF_INIT:
			Monitor_Init(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_SNIFF_OPEN:
			if (Monitor_Open(pAd, pData) != TRUE)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_SNIFF_CLOSE:
			if (Monitor_Close(pAd, pData) != TRUE)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_SNIFF_REMOVE:
			Monitor_Remove(pAd);
			break;
#endif /*CONFIG_SNIFFER_SUPPORT*/

#ifdef MESH_SUPPORT
		case CMD_RTPRIV_IOCTL_MESH_INIT:
			MeshInit(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_MESH_REMOVE:
			MESH_Remove(pAd);
			break;

		case CMD_RTPRIV_IOCTL_MESH_OPEN_PRE:
			if (MESH_OpenPre(pData) != 0)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_MESH_OPEN_POST:
			if (MESH_OpenPost(pData) != 0)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_MESH_IS_VALID:
			*(BOOLEAN *)pData = MeshValid(&pAd->MeshTab);
			break;

		case CMD_RTPRIV_IOCTL_MESH_CLOSE:
			MESH_Close(pData);
			break;
#endif /* MESH_SUPPORT */

#ifdef P2P_SUPPORT
		case CMD_RTPRIV_IOCTL_P2P_INIT:
			P2pInit(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_P2P_REMOVE:
			P2P_Remove(pAd);
			break;

		case CMD_RTPRIV_IOCTL_P2P_OPEN_PRE:
			if (P2P_OpenPre(pData) != 0)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_P2P_OPEN_POST:
			if (P2P_OpenPost(pData) != 0)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_P2P_CLOSE:
			P2P_Close(pData);
			break;
#endif /* P2P_SUPPORT */

		case CMD_RTPRIV_IOCTL_BEACON_UPDATE:
		/* update all beacon contents */
#ifdef CONFIG_AP_SUPPORT
			//CFG_TODO
			APMakeAllBssBeacon(pAd);
			APUpdateAllBeaconFrame(pAd);
#endif /* CONFIG_AP_SUPPORT */
#ifdef MESH_SUPPORT
			MeshMakeBeacon(pAd, pAd->MeshTab.bcn_buf.BcnBufIdx);
			MeshUpdateBeaconFrame(pAd, pAd->MeshTab.bcn_buf.BcnBufIdx);
#endif /* MESH_SUPPORT */
			break;

		case CMD_RTPRIV_IOCTL_RXPATH_GET:
		/* get the number of rx path */
			*(ULONG *)pData = pAd->Antenna.field.RxPath;
			break;

		case CMD_RTPRIV_IOCTL_CHAN_LIST_NUM_GET:
			*(ULONG *)pData = pAd->ChannelListNum;
			break;

		case CMD_RTPRIV_IOCTL_CHAN_LIST_GET:
		{
			UINT32 i;
			UCHAR *pChannel = (UCHAR *)pData;

			for (i = 1; i <= pAd->ChannelListNum; i++)
			{
				*pChannel = pAd->ChannelList[i-1].Channel;
				pChannel ++;
			}
		}
			break;

		case CMD_RTPRIV_IOCTL_FREQ_LIST_GET:
		{
			UINT32 i;
			UINT32 *pFreq = (UINT32 *)pData;
			UINT32 m;

			for (i = 1; i <= pAd->ChannelListNum; i++)
			{
				m = 2412000;
				MAP_CHANNEL_ID_TO_KHZ(pAd->ChannelList[i-1].Channel, m);
				(*pFreq) = m;
				pFreq ++;
			}
		}
			break;

#ifdef EXT_BUILD_CHANNEL_LIST
       case CMD_RTPRIV_SET_PRECONFIG_VALUE:
       /* Set some preconfigured value before interface up*/
           pAd->CommonCfg.DfsType = MAX_RD_REGION;
           break;
#endif /* EXT_BUILD_CHANNEL_LIST */


#ifdef RTMP_USB_SUPPORT
		case CMD_RTPRIV_IOCTL_USB_MORE_FLAG_SET:
		{
			RT_CMD_USB_MORE_FLAG_CONFIG *pConfig;
			UINT32 VendorID, ProductID;


			pConfig = (RT_CMD_USB_MORE_FLAG_CONFIG *)pData;
			VendorID = pConfig->VendorID;
			ProductID = pConfig->ProductID;

			if (VendorID == 0x0DB0)
			{
				if ((ProductID == 0x871C) || (ProductID == 0x822C))
				{
					RTMP_SET_MORE_FLAG(pAd, (fRTMP_ADAPTER_DISABLE_DOT_11N | fRTMP_ADAPTER_WSC_PBC_PIN0));
				}
				if ((ProductID == 0x871A) || (ProductID == 0x822A))
				{
					RTMP_SET_MORE_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DOT_11N);
				}
				if ((ProductID == 0x871B) || (ProductID == 0x822B))
				{
					RTMP_SET_MORE_FLAG(pAd, fRTMP_ADAPTER_WSC_PBC_PIN0);
				}
			}

	    	if (VendorID == 0x07D1)
	    	{
				if (ProductID == 0x3C0F)
					RTMP_SET_MORE_FLAG(pAd, fRTMP_ADAPTER_DISABLE_DOT_11N);
	    	}
		}
			break;

		case CMD_RTPRIV_IOCTL_USB_CONFIG_INIT:
		{
			RT_CMD_USB_DEV_CONFIG *pConfig;
			UINT32 i;
			pConfig = (RT_CMD_USB_DEV_CONFIG *)pData;

			pAd->NumberOfPipes = pConfig->NumberOfPipes;
			pAd->BulkInMaxPacketSize = pConfig->BulkInMaxPacketSize;
			pAd->BulkOutMaxPacketSize = pConfig->BulkOutMaxPacketSize;

			for (i = 0; i < 6; i++) 
				pAd->BulkOutEpAddr[i] = pConfig->BulkOutEpAddr[i];

			for (i = 0; i < 2; i++)
				pAd->BulkInEpAddr[i] = pConfig->BulkInEpAddr[i];

			pAd->config = pConfig->pConfig;
		}
			break;

		case CMD_RTPRIV_IOCTL_USB_SUSPEND:

#ifdef CONFIG_STA_SUPPORT

			pAd->PM_FlgSuspend = 1;

			pAd->PSEWatchDogEn = 0;

#ifdef MT_WOW_SUPPORT
			if ((pAd->WOW_Cfg.bEnable) && INFRA_ON(pAd) && (!pAd->WOW_Cfg.bWoWRunning))
#endif
			{
				MlmeRadioOff(pAd);
			}		


			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

			if (Data)
			{
				RTUSBCancelPendingBulkInIRP(pAd);
				RTUSBCancelPendingBulkOutIRP(pAd);
			}

			if (pAd->dp_ctrl.nUsb2DisconMode != 0)
				usb2_disconnect_cmd(pAd, pAd->dp_ctrl.nUsb2DisconMode);

			MCUSysExit(pAd);

#endif //CONFIG_STA_SUPPORT
			break;

		case CMD_RTPRIV_IOCTL_USB_RESUME:

#ifdef CONFIG_STA_SUPPORT

			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BULKIN_RESET);
			pAd->PM_FlgSuspend = 0;
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			MCU_CTRL_INIT(pAd);

			if (pAd->chipOps.FwInit && (pAd->chipCap.hif_type == HIF_MT))
				pAd->chipOps.FwInit(pAd);
			
			{	//We don not need to redownload FW, but have to change the stage
				struct MCU_CTRL *Ctl = &pAd->MCUCtrl;
				Ctl->Stage = FW_RUN_TIME;
			}
			
			AsicDMASchedulerInit(pAd, DMA_SCH_LMAC);

			pAd->PSEWatchDogEn = 1;
			
#ifdef MT_WOW_SUPPORT
			if ((pAd->WOW_Cfg.bEnable) && INFRA_ON(pAd) && (!pAd->WOW_Cfg.bWoWRunning))
#endif
			{
				MlmeRadioOn(pAd);
			}

#ifdef RTMP_MAC_USB
			{
				int index;
				/* Support multiple BulkIn IRP,*/
				/* the value on pAd->CommonCfg.NumOfBulkInIRP may be large than 1.*/
				for (index=0; index<pAd->CommonCfg.NumOfBulkInIRP; index++)
				{
					RTUSBBulkReceive(pAd);
					DBGPRINT(RT_DEBUG_TRACE, ("RTUSBBulkReceive!\n" ));
				}
			}
#endif /* RTMP_MAC_USB */	
#endif //CONFIG_STA_SUPPORT
			break;

		case CMD_RTPRIV_IOCTL_USB_INIT:
			InitUSBDevice(pData, pAd);
			break;

#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_PCI_SUPPORT
		case CMD_RTPRIV_IOCTL_PCI_SUSPEND:
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			break;

		case CMD_RTPRIV_IOCTL_PCI_RESUME:
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF);
			break;

		case CMD_RTPRIV_IOCTL_PCI_CSR_SET:
			pAd->CSRBaseAddress = (PUCHAR)Data;
			DBGPRINT(RT_DEBUG_ERROR, ("pAd->CSRBaseAddress =0x%lx, csr_addr=0x%lx!\n", (ULONG)pAd->CSRBaseAddress, (ULONG)Data));
			break;

#ifndef __ECOS
		case CMD_RTPRIV_IOCTL_PCIE_INIT:
			RTMPInitPCIeDevice(pData, pAd);
			break;
#endif /* !__ECOS */
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_SDIO_SUPPORT
      case CMD_RTPRIV_IOCTL_SDIO_INIT:
         InitSDIODevice(pAd);
         break;
#endif
#ifdef RT_CFG80211_SUPPORT
		case CMD_RTPRIV_IOCTL_CFG80211_CFG_START:
			RT_CFG80211_REINIT(pAd);
			RT_CFG80211_CRDA_REG_RULE_APPLY(pAd);
			break;
#endif /* RT_CFG80211_SUPPORT */

#ifdef INF_PPA_SUPPORT
		case CMD_RTPRIV_IOCTL_INF_PPA_INIT:
			os_alloc_mem(NULL, (UCHAR **)&(pAd->pDirectpathCb), sizeof(PPA_DIRECTPATH_CB));
			break;

		case CMD_RTPRIV_IOCTL_INF_PPA_EXIT:
			if (ppa_hook_directpath_register_dev_fn && (pAd->PPAEnable == TRUE))
			{
				UINT status;
				status = ppa_hook_directpath_register_dev_fn(&pAd->g_if_id, pAd->net_dev, NULL, 0);
				DBGPRINT(RT_DEBUG_TRACE, ("Unregister PPA::status=%d, if_id=%d\n", status, pAd->g_if_id));
			}
			os_free_mem(NULL, pAd->pDirectpathCb);
			break;
#endif /* INF_PPA_SUPPORT*/

		case CMD_RTPRIV_IOCTL_VIRTUAL_INF_UP:
		/* interface up */
		{
			RT_CMD_INF_UP_DOWN *pInfConf = (RT_CMD_INF_UP_DOWN *)pData;
			// TODO: Shiang-usw, this function looks have some problem, need to revise!
			if (VIRTUAL_IF_NUM(pAd) == 0)
			{
				ULONG start, end, diff_ms;
				/* Get the current time for calculating startup time */
				NdisGetSystemUpTime(&start);
				
				VIRTUAL_IF_INC(pAd);
				if (pInfConf->rt28xx_open(pAd->net_dev) != 0)
				{
					VIRTUAL_IF_DEC(pAd);
					DBGPRINT(RT_DEBUG_TRACE, ("rt28xx_open return fail!\n"));
					return NDIS_STATUS_FAILURE;
				}
				
				/* Get the current time for calculating startup time */
				NdisGetSystemUpTime(&end); diff_ms = (end-start)*1000/OS_HZ;
				DBGPRINT(RT_DEBUG_ERROR, ("WiFi Startup Cost (%s): %lu.%03lus\n",
						RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev),diff_ms/1000,diff_ms%1000));
			}
			else
			{
				VIRTUAL_IF_INC(pAd);
#ifdef CONFIG_AP_SUPPORT
				{
					extern VOID APMakeAllBssBeacon(IN PRTMP_ADAPTER pAd);
					extern VOID  APUpdateAllBeaconFrame(IN PRTMP_ADAPTER pAd);
					APMakeAllBssBeacon(pAd);
					APUpdateAllBeaconFrame(pAd);
				}
#endif /* CONFIG_AP_SUPPORT */
#ifdef MESH_SUPPORT
				{
					extern VOID MeshMakeBeacon(IN PRTMP_ADAPTER pAd, IN UCHAR idx);
					extern VOID MeshUpdateBeaconFrame(IN PRTMP_ADAPTER pAd, IN UCHAR idx);
					MeshMakeBeacon(pAd, pAd->MeshTab.bcn_buf.BcnBufIdx);
					MeshUpdateBeaconFrame(pAd, pAd->MeshTab.bcn_buf.BcnBufIdx);
				}
#endif /* MESH_SUPPORT */
			}
		}
			break;

		case CMD_RTPRIV_IOCTL_VIRTUAL_INF_DOWN:
		/* interface down */
		{
			RT_CMD_INF_UP_DOWN *pInfConf = (RT_CMD_INF_UP_DOWN *)pData;

			VIRTUAL_IF_DEC(pAd);
			if (VIRTUAL_IF_NUM(pAd) == 0)
				pInfConf->rt28xx_close(pAd->net_dev);
		}
			break;

		case CMD_RTPRIV_IOCTL_VIRTUAL_INF_GET:
		/* get virtual interface number */
			*(ULONG *)pData = VIRTUAL_IF_NUM(pAd);
			break;

		case CMD_RTPRIV_IOCTL_INF_TYPE_GET:
		/* get current interface type */
			*(ULONG *)pData = pAd->infType;
			break;

		case CMD_RTPRIV_IOCTL_INF_STATS_GET:
		/* get statistics */
		{
			RT_CMD_STATS *pStats = (RT_CMD_STATS *)pData;

			pStats->pStats = pAd->stats;
			if (pAd->OpMode == OPMODE_STA) {
				pStats->rx_packets =
				(ULONG)pAd->WlanCounters.ReceivedFragmentCount.QuadPart;
				pStats->tx_packets =
				(ULONG)pAd->WlanCounters.TransmittedFragmentCount.QuadPart;
				pStats->rx_bytes = pAd->RalinkCounters.ReceivedByteCount;
				pStats->tx_bytes = pAd->RalinkCounters.TransmittedByteCount;
				pStats->rx_errors = pAd->Counters8023.RxErrors;
				pStats->tx_errors = pAd->Counters8023.TxErrors;
				/* multicast packets received*/
				pStats->multicast =
				(ULONG)pAd->WlanCounters.MulticastReceivedFrameCount.QuadPart;
				pStats->collisions = 0;  /* Collision packets*/
				/* receiver ring buff overflow*/
				pStats->rx_over_errors = pAd->Counters8023.RxNoBuffer;
				/*pAd->WlanCounters.FCSErrorCount;      recved pkt with crc error*/
				pStats->rx_crc_errors = 0;
				pStats->rx_frame_errors = 0; /* recv'd frame alignment error*/
				/* recv'r fifo overrun*/
				pStats->rx_fifo_errors = pAd->Counters8023.RxNoBuffer;
			}
#ifdef CONFIG_AP_SUPPORT
				else if(pAd->OpMode == OPMODE_AP)
				{
					INT index;
					for(index = 0; index < MAX_MBSSID_NUM(pAd); index++)
					{
						if (pAd->ApCfg.MBSSID[index].wdev.if_dev == (PNET_DEV)(pStats->pNetDev))
						{
							break;
						}
					}
						
					if(index >= MAX_MBSSID_NUM(pAd))
					{
						//reset counters
						pStats->rx_packets = 0;
						pStats->tx_packets = 0;
						pStats->rx_bytes = 0;
						pStats->tx_bytes = 0;
						pStats->rx_errors = 0;
						pStats->tx_errors = 0;
						pStats->multicast = 0;   /* multicast packets received*/
						pStats->collisions = 0;  /* Collision packets*/
						pStats->rx_over_errors = 0; /* receiver ring buff overflow*/
						pStats->rx_crc_errors = 0; /* recved pkt with crc error*/
						pStats->rx_frame_errors = 0; /* recv'd frame alignment error*/
						pStats->rx_fifo_errors = 0; /* recv'r fifo overrun*/
						   
						DBGPRINT(RT_DEBUG_ERROR, ("CMD_RTPRIV_IOCTL_INF_STATS_GET: can not find mbss I/F\n"));
						return NDIS_STATUS_FAILURE;
					}
					
					pStats->rx_packets = pAd->ApCfg.MBSSID[index].RxCount;
					pStats->tx_packets = pAd->ApCfg.MBSSID[index].TxCount;
					pStats->rx_bytes = pAd->ApCfg.MBSSID[index].ReceivedByteCount;
					pStats->tx_bytes = pAd->ApCfg.MBSSID[index].TransmittedByteCount;
					pStats->rx_errors = pAd->ApCfg.MBSSID[index].RxErrorCount;
					pStats->tx_errors = pAd->ApCfg.MBSSID[index].TxErrorCount;
					pStats->multicast = pAd->ApCfg.MBSSID[index].mcPktsRx; /* multicast packets received */
					pStats->collisions = 0;  /* Collision packets*/
					pStats->rx_over_errors = 0;                   /* receiver ring buff overflow*/
					pStats->rx_crc_errors = 0;/* recved pkt with crc error*/
					pStats->rx_frame_errors = 0;          /* recv'd frame alignment error*/
					pStats->rx_fifo_errors = 0;                   /* recv'r fifo overrun*/
				}
#endif
		}
			break;

		case CMD_RTPRIV_IOCTL_INF_IW_STATUS_GET:
		/* get wireless statistics */
		{
			UCHAR CurOpMode = OPMODE_AP;
#ifdef CONFIG_AP_SUPPORT 
			PMAC_TABLE_ENTRY pMacEntry = NULL;
#endif /* CONFIG_AP_SUPPORT */
			RT_CMD_IW_STATS *pStats = (RT_CMD_IW_STATS *)pData;

			pStats->qual = 0;
			pStats->level = 0;
			pStats->noise = 0;
			pStats->pStats = pAd->iw_stats;
			
#ifdef CONFIG_STA_SUPPORT
			if (pAd->OpMode == OPMODE_STA)
			{
				CurOpMode = OPMODE_STA;
#ifdef P2P_SUPPORT
				if (pStats->priv_flags == INT_P2P)
					CurOpMode = OPMODE_AP;
#endif /* P2P_SUPPORT */					
			}
#endif /* CONFIG_STA_SUPPORT */

			/*check if the interface is down*/
			if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
				return NDIS_STATUS_FAILURE;	

#ifdef CONFIG_AP_SUPPORT
			if (CurOpMode == OPMODE_AP)
			{
#ifdef APCLI_SUPPORT
				if ((pStats->priv_flags == INT_APCLI)
#ifdef P2P_SUPPORT
					|| (P2P_CLI_ON(pAd))
#endif /* P2P_SUPPORT */
					)
				{
					INT ApCliIdx = ApCliIfLookUp(pAd, (PUCHAR)pStats->dev_addr);
					if ((ApCliIdx >= 0) && VALID_WCID(pAd->ApCfg.ApCliTab[ApCliIdx].MacTabWCID))
						pMacEntry = &pAd->MacTab.Content[pAd->ApCfg.ApCliTab[ApCliIdx].MacTabWCID];
				}
				else
#endif /* APCLI_SUPPORT */
				{
					/*
						only AP client support wireless stats function.
						return NULL pointer for all other cases.
					*/
					pMacEntry = NULL;
				}
			}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
			if (CurOpMode == OPMODE_STA)
				pStats->qual = (UINT8)((pAd->Mlme.ChannelQuality * 12)/10 + 10);
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
			if (CurOpMode == OPMODE_AP)
			{
				if (pMacEntry != NULL)
					pStats->qual =
						(UINT8)((pMacEntry->ChannelQuality * 12)/10 + 10);
				else
					pStats->qual =
						(UINT8)((pAd->Mlme.ChannelQuality * 12)/10 + 10);
			}
#endif /* CONFIG_AP_SUPPORT */

			if (pStats->qual > 100)
				pStats->qual = 100;

#ifdef CONFIG_STA_SUPPORT
			if (CurOpMode == OPMODE_STA)
			{
				pStats->level =
					RTMPMaxRssi(pAd, pAd->StaCfg.RssiSample.AvgRssi[0],
									pAd->StaCfg.RssiSample.AvgRssi[1],
									pAd->StaCfg.RssiSample.AvgRssi[2]);
			}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
			if (CurOpMode == OPMODE_AP)
			{
				if (pMacEntry != NULL)
					pStats->level =
						RTMPMaxRssi(pAd, pMacEntry->RssiSample.AvgRssi[0],
										pMacEntry->RssiSample.AvgRssi[1],
										pMacEntry->RssiSample.AvgRssi[2]);
#ifdef P2P_APCLI_SUPPORT
				else
					pStats->level =
						RTMPMaxRssi(pAd, pAd->StaCfg.RssiSample.AvgRssi[0],
										pAd->StaCfg.RssiSample.AvgRssi[1],
										pAd->StaCfg.RssiSample.AvgRssi[2]);
#endif /* P2P_APCLI_SUPPORT */
			}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
			pStats->noise = RTMPMaxNoise(pAd, pAd->ApCfg.RssiSample.LastNoiseLevel[0],
			pAd->ApCfg.RssiSample.LastNoiseLevel[1],
			pAd->ApCfg.RssiSample.LastNoiseLevel[2]);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
			pStats->noise = RTMPMaxNoise(pAd, pAd->StaCfg.RssiSample.LastNoiseLevel[0],
			pAd->StaCfg.RssiSample.LastNoiseLevel[1],
			pAd->StaCfg.RssiSample.LastNoiseLevel[2]);
#endif /* CONFIG_STA_SUPPORT */
		}
			break;

		case CMD_RTPRIV_IOCTL_INF_MAIN_CREATE:
			*(VOID **)pData = RtmpPhyNetDevMainCreate(pAd);
			break;

		case CMD_RTPRIV_IOCTL_INF_MAIN_ID_GET:
			*(ULONG *)pData = INT_MAIN;
			break;

		case CMD_RTPRIV_IOCTL_INF_MAIN_CHECK:
			if (Data != INT_MAIN)
				return NDIS_STATUS_FAILURE;
			break;

		case CMD_RTPRIV_IOCTL_INF_P2P_CHECK:
			if (Data != INT_P2P)
				return NDIS_STATUS_FAILURE;
			break;

#ifdef WDS_SUPPORT
		case CMD_RTPRIV_IOCTL_WDS_INIT:
			WDS_Init(pAd, pData);
			break;

		case CMD_RTPRIV_IOCTL_WDS_REMOVE:
			WDS_Remove(pAd);
			break;

		case CMD_RTPRIV_IOCTL_WDS_STATS_GET:
			if (Data == INT_WDS)
			{
				if (WDS_StatsGet(pAd, pData) != TRUE)
					return NDIS_STATUS_FAILURE;
			}
			else
				return NDIS_STATUS_FAILURE;
			break;
#endif /* WDS_SUPPORT */

#ifdef CONFIG_ATE
#ifdef CONFIG_QA
		case CMD_RTPRIV_IOCTL_ATE:
			if (!wrq) {
				DBGPRINT(RT_DEBUG_WARN,
					 ("null wrq in CMD_RTPRIV_IOCTL_ATE\n"));
				Status = -EINVAL;
				break;
			}

			RtmpDoAte(pAd, wrq, pData);
			break;
#endif /* CONFIG_QA */ 
#endif /* CONFIG_ATE */

		case CMD_RTPRIV_IOCTL_MAC_ADDR_GET:
			{
				UCHAR mac_addr[MAC_ADDR_LEN];
				USHORT Addr01, Addr23, Addr45;
					
				RT28xx_EEPROM_READ16(pAd, 0x04, Addr01);
				RT28xx_EEPROM_READ16(pAd, 0x06, Addr23);
				RT28xx_EEPROM_READ16(pAd, 0x08, Addr45);			
			
				mac_addr[0] = (UCHAR)(Addr01 & 0xff);
				mac_addr[1] = (UCHAR)(Addr01 >> 8);
				mac_addr[2] = (UCHAR)(Addr23 & 0xff);
				mac_addr[3] = (UCHAR)(Addr23 >> 8);
				mac_addr[4] = (UCHAR)(Addr45 & 0xff);
				mac_addr[5] = (UCHAR)(Addr45 >> 8);
			
				for(i=0; i<6; i++)
					*(UCHAR *)(pData+i) = mac_addr[i];
				break;
			}

#ifdef CONFIG_AP_SUPPORT
		case CMD_RTPRIV_IOCTL_AP_SIOCGIWRATEQ:
		/* handle for SIOCGIWRATEQ */
		{
			RT_CMD_IOCTL_RATE *pRate = (RT_CMD_IOCTL_RATE *)pData;
			HTTRANSMIT_SETTING HtPhyMode;

#ifdef MESH_SUPPORT
			if (pRate->priv_flags == INT_MESH)
				HtPhyMode = pAd->MeshTab.wdev.HTPhyMode;
			else
#endif /* MESH_SUPPORT */
#ifdef APCLI_SUPPORT
			if (pRate->priv_flags == INT_APCLI)
				HtPhyMode = pAd->ApCfg.ApCliTab[pObj->ioctl_if].wdev.HTPhyMode;
			else
#endif /* APCLI_SUPPORT */
#ifdef WDS_SUPPORT
			if (pRate->priv_flags == INT_WDS)
				HtPhyMode = pAd->WdsTab.WdsEntry[pObj->ioctl_if].wdev.HTPhyMode;
			else
#endif /* WDS_SUPPORT */
				HtPhyMode = pAd->ApCfg.MBSSID[pObj->ioctl_if].wdev.HTPhyMode;

		RtmpDrvMaxRateGet(pAd, (UINT8)HtPhyMode.field.MODE, (UINT8)HtPhyMode.field.ShortGI,
			(UINT8)HtPhyMode.field.BW, (UINT8)HtPhyMode.field.MCS,
							(UINT32 *)&pRate->BitRate);
		}
		break;
#endif /* CONFIG_AP_SUPPORT */

		case CMD_RTPRIV_IOCTL_SIOCGIWNAME:
			RtmpIoctl_rt_ioctl_giwname(pAd, pData, 0);
			break;

#ifdef CONFIG_CSO_SUPPORT
		case CMD_RTPRIV_IOCTL_ADAPTER_CSO_SUPPORT_TEST:
			*(UCHAR *)pData = (pAd->MoreFlags & fASIC_CAP_CSO) ? 1:0;
			break;
#endif /* CONFIG_CSO_SUPPORT */
#ifdef CONFIG_TSO_SUPPORT
		case CMD_RTPRIV_IOCTL_ADAPTER_TSO_SUPPORT_TEST:
			*(UCHAR *)pData = (pAd->MoreFlags & fASIC_CAP_TSO) ? 1:0;
			break;
#endif /* CONFIG_TSO_SUPPORT */
	}
#ifdef RT_CFG80211_SUPPORT
	if ((CMD_RTPRIV_IOCTL_80211_START <= cmd) &&
		(cmd <= CMD_RTPRIV_IOCTL_80211_END))
	{
		Status = CFG80211DRV_IoctlHandle(pAd, wrq, cmd, subcmd, pData, Data);
	}
#endif /* RT_CFG80211_SUPPORT */

	if (cmd >= CMD_RTPRIV_IOCTL_80211_COM_LATEST_ONE)
		return NDIS_STATUS_FAILURE;

	return Status;
}

/* 
    ==========================================================================
    Description:
        Issue a site survey command to driver
	Arguments:
	    pAdapter                    Pointer to our adapter
	    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage: 
               1.) iwpriv ra0 set site_survey
    ==========================================================================
*/
INT Set_SiteSurvey_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	NDIS_802_11_SSID Ssid;
	
#ifdef CONFIG_AP_SUPPORT
#ifdef AP_SCAN_SUPPORT
#ifdef P2P_SUPPORT
	POS_COOKIE pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif /* P2P_SUPPORT */
#endif /* AP_SCAN_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	//check if the interface is down
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
		return -ENETDOWN;   
	}

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (MONITOR_ON(pAd))
    	{
        	DBGPRINT(RT_DEBUG_TRACE, ("!!! Driver is in Monitor Mode now !!!\n"));
        	return -EINVAL;
    	}
	}
#endif // CONFIG_STA_SUPPORT //

    NdisZeroMemory(&Ssid, sizeof(NDIS_802_11_SSID));

#ifdef CONFIG_AP_SUPPORT
#ifdef AP_SCAN_SUPPORT
#ifdef P2P_SUPPORT
	if (pObj->ioctl_if_type == INT_P2P)
#else
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* P2P_SUPPORT */
	{
		if ((strlen(arg) != 0) && (strlen(arg) <= MAX_LEN_OF_SSID))
    	{
        	NdisMoveMemory(Ssid.Ssid, arg, strlen(arg));
        	Ssid.SsidLength = strlen(arg);
		}

		if (Ssid.SsidLength == 0)
			ApSiteSurvey(pAd, &Ssid, SCAN_PASSIVE, FALSE);
		else
			ApSiteSurvey(pAd, &Ssid, SCAN_ACTIVE, FALSE);

		return TRUE;
	}
#endif /* AP_SCAN_SUPPORT */
#endif // CONFIG_AP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		Ssid.SsidLength = 0; 
		if ((arg != NULL) &&
			(strlen(arg) <= MAX_LEN_OF_SSID))
		{
			RTMPMoveMemory(Ssid.Ssid, arg, strlen(arg));
			Ssid.SsidLength = strlen(arg);
		}

		pAd->StaCfg.bSkipAutoScanConn = TRUE;
		StaSiteSurvey(pAd, &Ssid, SCAN_ACTIVE);
	}
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Set_SiteSurvey_Proc\n"));

    return TRUE;
}

INT	Set_Antenna_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ANT_DIVERSITY_TYPE UsedAnt;
	int i;
	DBGPRINT(RT_DEBUG_OFF, ("==> Set_Antenna_Proc *******************\n"));

	for (i = 0; i < strlen(arg); i++)
		if (!isdigit(arg[i]))
			return -EINVAL;

	UsedAnt = simple_strtol(arg, 0, 10);

	switch (UsedAnt)
	{
#ifdef ANT_DIVERSITY_SUPPORT
		/* 0: Disabe --> set Antenna CON1*/
		case ANT_DIVERSITY_DISABLE:
#endif /* ANT_DIVERSITY_SUPPORT */
		/* 2: Fix in the PHY Antenna CON1*/
		case ANT_FIX_ANT0:
			AsicSetRxAnt(pAd, 0);
#ifdef ANT_DIVERSITY_SUPPORT
			pAd->CommonCfg.RxAntDiversityCfg = UsedAnt;
#endif /* ANT_DIVERSITY_SUPPORT */
			DBGPRINT(RT_DEBUG_OFF, ("<== Set_Antenna_Proc(Fix in Ant CON1), (%d,%d)\n", 
					pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			break;
#ifdef ANT_DIVERSITY_SUPPORT
		/* 1: Enable --> HW/SW Antenna diversity*/
		case ANT_DIVERSITY_ENABLE:
			if ((pAd->chipCap.FlgIsHwAntennaDiversitySup) && (pAd->chipOps.HwAntEnable) &&
					pAd->CommonCfg.bHWRxAntDiversity) /* HW_ANT_DIV (PPAD) */
			{
				pAd->chipOps.HwAntEnable(pAd);
				pAd->CommonCfg.RxAntDiversityCfg = ANT_HW_DIVERSITY_ENABLE;

				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Auto Switch Mode), (%d,%d)\n", 
					__FUNCTION__, pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			}	
			else if (pAd->CommonCfg.bSWRxAntDiversity)/* SW_ANT_DIV */
			{
				pAd->RxAnt.EvaluateStableCnt = 0;
				pAd->CommonCfg.RxAntDiversityCfg = ANT_SW_DIVERSITY_ENABLE;

				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Auto Switch Mode), (%d,%d)\n", 
					__FUNCTION__, pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Auto Switch Mode) EEPROM not enable diversity(%d/%d)\n", 
					__FUNCTION__, pAd->CommonCfg.bSWRxAntDiversity, pAd->CommonCfg.bHWRxAntDiversity));

			break;
#endif /* ANT_DIVERSITY_SUPPORT */
    	/* 3: Fix in the PHY Antenna CON2*/
		case ANT_FIX_ANT1:
			AsicSetRxAnt(pAd, 1);
#ifdef ANT_DIVERSITY_SUPPORT
			pAd->CommonCfg.RxAntDiversityCfg = UsedAnt;
#endif /* ANT_DIVERSITY_SUPPORT */
			DBGPRINT(RT_DEBUG_OFF, ("<== %s(Fix in Ant CON2), (%d,%d)\n", 
							__FUNCTION__, pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			break;
#ifdef ANT_DIVERSITY_SUPPORT
		/* 4: Enable SW Antenna Diversity */
		case ANT_SW_DIVERSITY_ENABLE:
			if (pAd->CommonCfg.bSWRxAntDiversity)
			{
				pAd->RxAnt.EvaluateStableCnt = 0;
				pAd->CommonCfg.RxAntDiversityCfg = UsedAnt;
				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Diversity Mode --> SW), (%d,%d)\n", 
						__FUNCTION__, pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Diversity Mode --> SW) EEPROM not enable diversity(%d/%d)\n", 
						__FUNCTION__, pAd->CommonCfg.bSWRxAntDiversity, pAd->CommonCfg.bHWRxAntDiversity));

			break;
		/* 5: Enable HW Antenna Diversity - PPAD */
		case ANT_HW_DIVERSITY_ENABLE:
			if ((pAd->chipCap.FlgIsHwAntennaDiversitySup) && (pAd->chipOps.HwAntEnable) 
					&& (pAd->CommonCfg.bHWRxAntDiversity)) /* HW_ANT_DIV (PPAD) */
			{
				pAd->chipOps.HwAntEnable(pAd);
				pAd->CommonCfg.RxAntDiversityCfg = UsedAnt;

				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Diversity Mode --> HW), (%d,%d)\n", 
					__FUNCTION__, pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			}
			else
				DBGPRINT(RT_DEBUG_OFF, ("<== %s(Diversity Mode --> HW), EEPROM not enable diversity(%d/%d)\n", 
					__FUNCTION__, pAd->CommonCfg.bSWRxAntDiversity, pAd->CommonCfg.bHWRxAntDiversity));

			break;
#endif /* ANT_DIVERSITY_SUPPORT */
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("<== %s(N/A cmd: %d), (%d,%d)\n", __FUNCTION__, UsedAnt,
					pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));
			break;
	}
	
	return TRUE;
}
			

#ifdef RT5350
INT Set_Hw_Antenna_Div_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	return Set_Antenna_Proc(pAd, arg);	
}
#endif /* RT5350 */


#ifdef RT6352
INT Set_RfBankSel_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	LONG RfBank;

	RfBank = simple_strtol(arg, 0, 10);

	pAd->RfBank = RfBank;

	return TRUE;
}

#ifdef RTMP_TEMPERATURE_CALIBRATION
INT Set_TemperatureCAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	RT6352_Temperature_Init(pAd);
	return TRUE;
}
#endif /* RTMP_TEMPERATURE_CALIBRATION */
#endif /* RT6352 */

#ifdef HW_TX_RATE_LOOKUP_SUPPORT
INT Set_HwTxRateLookUp_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR Enable;
	UINT32 MacReg;

	Enable = simple_strtol(arg, 0, 10);

	RTMP_IO_READ32(pAd, TX_FBK_LIMIT, &MacReg);
	if (Enable)
	{
		MacReg |= 0x00040000;
		pAd->bUseHwTxLURate = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("==>UseHwTxLURate (ON)\n"));
	}
	else
	{
		MacReg &= (~0x00040000);
		pAd->bUseHwTxLURate = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("==>UseHwTxLURate (OFF)\n"));
	}
	RTMP_IO_WRITE32(pAd, TX_FBK_LIMIT, MacReg);

	DBGPRINT(RT_DEBUG_WARN, ("UseHwTxLURate = %d \n", pAd->bUseHwTxLURate));

	return TRUE;
}
#endif /* HW_TX_RATE_LOOKUP_SUPPORT */

#ifdef MAC_REPEATER_SUPPORT
#ifdef MULTI_MAC_ADDR_EXT_SUPPORT
INT Set_EnMultiMacAddrExt_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR Enable = simple_strtol(arg, 0, 10);

	pAd->bUseMultiMacAddrExt = (Enable ? TRUE : FALSE);
	AsicSetMacAddrExt(pAd, pAd->bUseMultiMacAddrExt);

	DBGPRINT(RT_DEBUG_WARN, ("UseMultiMacAddrExt = %d, UseMultiMacAddrExt(%s)\n",
				pAd->bUseMultiMacAddrExt, (Enable ? "ON" : "OFF")));

	return TRUE;
}

INT	Set_MultiMacAddrExt_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR tempMAC[6], idx;
	RTMP_STRING *token;
	RTMP_STRING sepValue[] = ":", DASH = '-';
	ULONG offset, Addr;
	INT i;
	
	if(strlen(arg) < 19)  /*Mac address acceptable format 01:02:03:04:05:06 length 17 plus the "-" and tid value in decimal format.*/
		return FALSE;

	token = strchr(arg, DASH);
	if ((token != NULL) && (strlen(token)>1))
	{
		idx = (UCHAR) simple_strtol((token+1), 0, 10);

		if (idx > 15)
			return FALSE;
		
		*token = '\0';
		for (i = 0, token = rstrtok(arg, &sepValue[0]); token; token = rstrtok(NULL, &sepValue[0]), i++)
		{
			if((strlen(token) != 2) || (!isxdigit(*token)) || (!isxdigit(*(token+1))))
				return FALSE;
			AtoH(token, (&tempMAC[i]), 1);
		}

		if(i != 6)
			return FALSE;

		DBGPRINT(RT_DEBUG_OFF, ("\n%02x:%02x:%02x:%02x:%02x:%02x-%02x\n", 
								tempMAC[0], tempMAC[1], tempMAC[2], tempMAC[3], tempMAC[4], tempMAC[5], idx));

		offset = 0x1480 + (HW_WCID_ENTRY_SIZE * idx);	
		Addr = tempMAC[0] + (tempMAC[1] << 8) +(tempMAC[2] << 16) +(tempMAC[3] << 24);
		RTMP_IO_WRITE32(pAd, offset, Addr);
		Addr = tempMAC[4] + (tempMAC[5] << 8);
		RTMP_IO_WRITE32(pAd, offset + 4, Addr);	

		return TRUE;
	}

	return FALSE;
}
#endif /* MULTI_MAC_ADDR_EXT_SUPPORT */
#endif /* MAC_REPEATER_SUPPORT */

INT set_tssi_enable(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	long tssi_enable = 0;

	tssi_enable = simple_strtol(arg, 0, 10);

	if (tssi_enable == 1) {
		pAd->chipCap.tssi_enable = TRUE;
		DBGPRINT(RT_DEBUG_OFF, ("turn on TSSI mechanism\n")); 
	} else if (tssi_enable == 0) {
		pAd->chipCap.tssi_enable = FALSE;
		DBGPRINT(RT_DEBUG_OFF, ("turn off TSS mechanism\n"));
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("illegal param(%ld)\n", tssi_enable));
		return FALSE;
    }
	return TRUE;
}

#ifdef RLT_MAC
#ifdef CONFIG_WIFI_TEST
INT set_pbf_loopback(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 enable = 0;
	UINT32 value;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	enable = simple_strtol(arg, 0, 10);
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &value);

	if (enable == 1) {
		pAd->chipCap.pbf_loopback = TRUE;
		value |= PBF_LOOP_EN;
		DBGPRINT(RT_DEBUG_OFF, ("turn on pbf loopback\n"));
	} else if(enable == 0) {
		pAd->chipCap.pbf_loopback = FALSE;
		value &= ~PBF_LOOP_EN;
		DBGPRINT(RT_DEBUG_OFF, ("turn off pbf loopback\n"));
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("illegal param(%d)\n"));
		return FALSE;
	}

	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, value);

	return TRUE;
}

INT set_pbf_rx_drop(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 enable = 0;
	UINT32 value;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
					__FUNCTION__));
		return FALSE;
	}

	enable = simple_strtol(arg, 0, 10);

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		RTMP_IO_READ32(pAd, RLT_PBF_CFG, &value);
#endif

	if (enable == 1) {
		pAd->chipCap.pbf_rx_drop = TRUE;
		value |= RX_DROP_MODE;
		DBGPRINT(RT_DEBUG_OFF, ("turn on pbf loopback\n"));
	} else if (enable == 0) {
		pAd->chipCap.pbf_rx_drop = FALSE;
		value &= ~RX_DROP_MODE;
		DBGPRINT(RT_DEBUG_OFF, ("turn off pbf loopback\n"));
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("illegal param(%d)\n"));
		return FALSE;
	}

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		RTMP_IO_WRITE32(pAd, RLT_PBF_CFG, value);
#endif

	return TRUE;
}
#endif

INT set_fw_debug(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 fw_debug_param;

	fw_debug_param = simple_strtol(arg, 0, 10);

#ifdef RLT_MAC
	AndesRltFunSet(pAd, LOG_FW_DEBUG_MSG, fw_debug_param);
#endif /* RLT_MAC */

	return TRUE;
}
#endif

#ifdef MT_MAC
INT set_get_fid(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
    //TODO: Carter, at present, only can read pkt in Port2(LMAC port)
	UCHAR   loop = 0, dw_idx = 0;
	long q_idx = 0;
    UINT32  head_fid_addr = 0, dw_content, next_fid_addr = 0;
    volatile UINT32  value = 0x00000000L;
    q_idx = simple_strtol(arg, 0, 10);

    value = 0x00400000 | (q_idx << 16);//port2. queue by input value.
    RTMP_IO_WRITE32(pAd, 0x8024, value);
    RTMP_IO_READ32(pAd, 0x8024, &head_fid_addr);//get head FID.
    head_fid_addr = head_fid_addr & 0xfff;

    if (head_fid_addr == 0xfff) {
	DBGPRINT(RT_DEBUG_ERROR, ("%s, q_idx:%ld empty!!\n", __func__, q_idx));
        return TRUE;
    }

    value = (0 | (head_fid_addr << 16));
    while (1) {
        for (dw_idx = 0; dw_idx < 8; dw_idx++) {
            RTMP_IO_READ32(pAd, ((MT_PCI_REMAP_ADDR_1 + (((value & 0x0fff0000) >> 16) * 128)) + (dw_idx * 4)), &dw_content);//get head FID.
            DBGPRINT(RT_DEBUG_ERROR, ("pkt:%d, fid:%x, dw_idx = %d, dw_content = 0x%x\n", loop, ((value & 0x0fff0000) >> 16), dw_idx, dw_content));
        }
        RTMP_IO_WRITE32(pAd, 0x8028, value);
        RTMP_IO_READ32(pAd, 0x8028, &next_fid_addr);//get next FID.
        if ((next_fid_addr & 0xfff) == 0xfff) {
            return TRUE;
        }

        value = (0 | ((next_fid_addr & 0xffff) << 16));
        loop++;
        if (loop > 5) {
            return TRUE;
        }
    }
    return TRUE;
}


#ifdef RTMP_MAC_PCI
INT Set_PDMAWatchDog_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 Dbg;

    Dbg = simple_strtol(arg, 0, 10);

	if (Dbg == 1)
	{
		pAd->PDMAWatchDogEn = 1;
	}
	else if (Dbg == 0)
	{
		pAd->PDMAWatchDogEn = 0;
	}
	else if (Dbg == 2)
	{
		PDMAResetAndRecovery(pAd);
	}
	else if (Dbg == 3)
	{
		pAd->PDMAWatchDogDbg = 0;
	}
	else if (Dbg == 4)
	{
		pAd->PDMAWatchDogDbg = 1;
	}

	return TRUE;
}


INT SetPSEWatchDog_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 Dbg;

    Dbg = simple_strtol(arg, 0, 10);

	if (Dbg == 1)
	{
		pAd->PSEWatchDogEn = 1;
	}
	else if (Dbg == 0)
	{
		pAd->PSEWatchDogEn = 0;
	}
	else if (Dbg == 2)
	{
		PSEResetAndRecovery(pAd);
	}
	else if (Dbg == 3)
	{
		DumpPseInfo(pAd);
	}

	return TRUE;
}
#endif


INT set_fw_log(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 LogType;
    LogType = simple_strtol(arg, 0, 10);

	if (LogType < 3)
		CmdFwLog2Host(pAd, (UINT8)LogType);
	else
		DBGPRINT(RT_DEBUG_OFF, (":%s: Unknown Log Type = %d\n", __FUNCTION__, LogType));

	return TRUE;
}

#ifdef THERMAL_PROTECT_SUPPORT
INT set_thermal_protection_criteria_proc(
	IN PRTMP_ADAPTER	pAd, 
	IN RTMP_STRING		*arg)
{
	UINT8 HighEn;
	CHAR HighTempTh;
	UINT8 LowEn;
	CHAR LowTempTh;
	CHAR *Param;

	Param = rstrtok(arg, ",");

	if (Param != NULL)
	{
		HighEn = simple_strtol(Param, 0, 10);
	}
	else
	{
		goto error;
	}

	Param = rstrtok(NULL, ",");

	if (Param != NULL)
	{
		HighTempTh = simple_strtol(Param, 0, 10);
	}
	else
	{
		goto error;
	}

	Param = rstrtok(NULL, ",");

	if (Param != NULL)
	{
		LowEn = simple_strtol(Param, 0, 10);
	}
	else
	{
		goto error;
	}

	Param = rstrtok(NULL, ",");

	if (Param != NULL)
	{
		LowTempTh = simple_strtol(Param, 0, 10);
	}
	else
	{
		goto error;
	}


	CmdThermalProtect(pAd, HighEn, HighTempTh, LowEn, LowTempTh);	

	DBGPRINT(RT_DEBUG_OFF, ("%s: high_en=%d, high_thd = %d, low_en = %d, low_thd = %d\n", __FUNCTION__, HighEn, HighTempTh, LowEn, LowTempTh));
	
	return TRUE;

error:
	DBGPRINT(RT_DEBUG_OFF, ("iwpriv ra0 set tpc=high_en,high_thd,low_en,low_thd\n"));
	return TRUE;

}
#endif /* THERMAL_PROTECT_SUPPORT */

VOID StatRateToString(RTMP_ADAPTER *pAd, CHAR *Output, UCHAR TxRx, UINT32 RawData)
{
	extern UCHAR tmi_rate_map_ofdm[];
	extern UCHAR tmi_rate_map_cck_lp[];
	extern UCHAR tmi_rate_map_cck_sp[];
	UCHAR phy_mode, rate, preamble;
	CHAR *phyMode[5] = {"CCK", "OFDM", "MM", "GF", "VHT"};

	phy_mode = RawData>>13;
	rate = RawData & 0x3F;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED))
		preamble = SHORT_PREAMBLE;
	else
		preamble = LONG_PREAMBLE;

	if ( TxRx == 0 )
		sprintf(Output+strlen(Output), "Last TX Rate                    = ");
	else
		sprintf(Output+strlen(Output), "Last RX Rate                    = ");

	if ( phy_mode == MODE_CCK ) {

		if ( TxRx == 0 )
		{
			if (preamble)
				rate = tmi_rate_map_cck_lp[rate];
			else
				rate = tmi_rate_map_cck_sp[rate];
		}
		
		if ( rate == TMI_TX_RATE_CCK_1M_LP )
			sprintf(Output+strlen(Output), "1M, ");
		else if ( rate == TMI_TX_RATE_CCK_2M_LP )
			sprintf(Output+strlen(Output), "2M, ");
		else if ( rate == TMI_TX_RATE_CCK_5M_LP )
			sprintf(Output+strlen(Output), "5M, ");
		else if ( rate == TMI_TX_RATE_CCK_11M_LP )
			sprintf(Output+strlen(Output), "11M, ");
		else if ( rate == TMI_TX_RATE_CCK_2M_SP )
			sprintf(Output+strlen(Output), "2M, ");
		else if ( rate == TMI_TX_RATE_CCK_5M_SP )
			sprintf(Output+strlen(Output), "5M, ");
		else if ( rate == TMI_TX_RATE_CCK_11M_SP )
			sprintf(Output+strlen(Output), "11M, ");
		else
			sprintf(Output+strlen(Output), "unkonw, ");

	} else if ( phy_mode == MODE_OFDM ) {

		if ( TxRx == 0 )
		{
			rate = tmi_rate_map_ofdm[rate];
		}

		if ( rate == TMI_TX_RATE_OFDM_6M )
			sprintf(Output+strlen(Output), "6M, ");
		else if ( rate == TMI_TX_RATE_OFDM_9M )
			sprintf(Output+strlen(Output), "9M, ");
		else if ( rate == TMI_TX_RATE_OFDM_12M )
			sprintf(Output+strlen(Output), "12M, ");
		else if ( rate == TMI_TX_RATE_OFDM_18M )
			sprintf(Output+strlen(Output), "18M, ");
		else if ( rate == TMI_TX_RATE_OFDM_24M )
			sprintf(Output+strlen(Output), "24M, ");
		else if ( rate == TMI_TX_RATE_OFDM_36M )
			sprintf(Output+strlen(Output), "36M, ");
		else if ( rate == TMI_TX_RATE_OFDM_48M )
			sprintf(Output+strlen(Output), "48M, ");
		else if ( rate == TMI_TX_RATE_OFDM_54M )
			sprintf(Output+strlen(Output), "54M, ");
		else
			sprintf(Output+strlen(Output), "unkonw, ");
	} else {
			sprintf(Output+strlen(Output), "MCS%d, ", rate);
	}
	sprintf(Output+strlen(Output), "%2dM, ", ((RawData>>7) & 0x1)? 40: 20);
	sprintf(Output+strlen(Output), "%cGI, ", ((RawData>>9) & 0x1)? 'S': 'L');
	sprintf(Output+strlen(Output), "%s%s\n", phyMode[(phy_mode) & 0x3], ((RawData>>10) & 0x3)? ", STBC": " ");

}


INT Set_themal_sensor(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	/* 0: get temperature; 1: get adc */
	UINT32 value;
	UINT32 temperature=0; 
	value = simple_strtol(arg, 0, 10);

	if ((value == 0) || (value == 1)) {
#if defined(MT7603) || defined(MT7628)
		temperature = MtAsicGetThemalSensor(pAd, (CHAR)value);
		DBGPRINT(RT_DEBUG_OFF, ("%s: ThemalSensor = 0x%x\n", __FUNCTION__, temperature));
#else
		CmdGetThemalSensorResult(pAd, value);
#endif /* MT7603 ||MT7628  */
	} else
		DBGPRINT(RT_DEBUG_OFF, (":%s: 0: get temperature; 1: get adc\n", __FUNCTION__));

	return TRUE;
}


INT Set_rx_pspoll_filter_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value;	
	value = simple_strtol(arg, 0, 10);

	pAd->rx_pspoll_filter = (USHORT)value;
	MtAsicSetRxPspollFilter(pAd, (CHAR)pAd->rx_pspoll_filter);

	DBGPRINT(RT_DEBUG_OFF, (":%s: rx_pspoll_filter=%d\n", __FUNCTION__, pAd->rx_pspoll_filter));
	return TRUE;
}
#endif

#ifdef SINGLE_SKU_V2
INT SetSKUEnable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	long value;
	
	value = simple_strtol(arg, 0, 10);
	
	if (value)
	{
		pAd->SKUEn = 1;
		DBGPRINT(RT_DEBUG_ERROR, ("==>SetSKUEnable_Proc (ON)\n"));
	}
	else
	{
		pAd->SKUEn = 0;
		DBGPRINT(RT_DEBUG_ERROR, ("==>SetSKUEnable_Proc (OFF)\n"));
	}

	AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
	
	return TRUE;
}
#endif /* SINGLE_SKU_V2 */

/* run-time turn EDCCA on/off */
INT Set_ed_chk_proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT ed_chk = simple_strtol(arg, 0, 10);
	
	DBGPRINT(RT_DEBUG_OFF, ("%s()::ed_chk=%d\n", 
		__FUNCTION__, ed_chk));

	if (ed_chk != 0)
		RTMP_CHIP_ASIC_SET_EDCCA(pAd, TRUE);
	else
		RTMP_CHIP_ASIC_SET_EDCCA(pAd, FALSE);
	
	return TRUE;
}

