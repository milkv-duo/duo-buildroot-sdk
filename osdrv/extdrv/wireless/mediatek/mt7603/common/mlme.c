/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	mlme.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
*/

#include "rt_config.h"
#include <stdarg.h>
#ifdef DOT11R_FT_SUPPORT
#include "ft.h"
#endif /* DOT11R_FT_SUPPORT */

#ifdef DOT11V_WNM_SUPPORT
#include "wnm.h"
#endif /* DOT11V_WNM_SUPPORT */

UCHAR CISCO_OUI[] = {0x00, 0x40, 0x96};
UCHAR RALINK_OUI[]  = {0x00, 0x0c, 0x43};
UCHAR WPA_OUI[] = {0x00, 0x50, 0xf2, 0x01};
UCHAR RSN_OUI[] = {0x00, 0x0f, 0xac};
UCHAR WAPI_OUI[] = {0x00, 0x14, 0x72};
UCHAR WME_INFO_ELEM[]  = {0x00, 0x50, 0xf2, 0x02, 0x00, 0x01};
UCHAR WME_PARM_ELEM[] = {0x00, 0x50, 0xf2, 0x02, 0x01, 0x01};
UCHAR BROADCOM_OUI[]  = {0x00, 0x90, 0x4c};
UCHAR WPS_OUI[] = {0x00, 0x50, 0xf2, 0x04};
#ifdef CONFIG_STA_SUPPORT
#ifdef IWSC_SUPPORT
UCHAR IWSC_OUI[] = {0x00, 0x50, 0xf2, 0x10};
UCHAR IWSC_ACTION_OUI[] = {0x50, 0x6F, 0x9A, 0x10};
#endif /* IWSC_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

#ifdef WFA_WFD_SUPPORT
UCHAR   WFA_WFD_OUI[] = {0x50, 0x6f, 0x9a, 0x0a};
#endif /* WFA_WFD_SUPPORT */

UCHAR OfdmRateToRxwiMCS[12] = {
	0,  0,	0,  0,
	0,  1,	2,  3,	/* OFDM rate 6,9,12,18 = rxwi mcs 0,1,2,3 */
	4,  5,	6,  7,	/* OFDM rate 24,36,48,54 = rxwi mcs 4,5,6,7 */
};

UCHAR RxwiMCSToOfdmRate[12] = {
	RATE_6,  RATE_9,	RATE_12,  RATE_18,
	RATE_24,  RATE_36,	RATE_48,  RATE_54,	/* OFDM rate 6,9,12,18 = rxwi mcs 0,1,2,3 */
	4,  5,	6,  7,	/* OFDM rate 24,36,48,54 = rxwi mcs 4,5,6,7 */
};


#ifdef WAC_SUPPORT
extern UCHAR	SAMSUNG_OUI[];
extern UCHAR	SAMSUNG_OUI_TYPE;
#endif /* WAC_SUPPORT */


#if 0
PCHAR ReasonString[] = {
	/* 0  */	 "Reserved",
	/* 1  */	 "Unspecified Reason",
	/* 2  */	 "Previous Auth no longer valid",
	/* 3  */	 "STA is leaving / has left",
	/* 4  */	 "DIS-ASSOC due to inactivity",
	/* 5  */	 "AP unable to hanle all associations",
	/* 6  */	 "class 2 error",
	/* 7  */	 "class 3 error",
	/* 8  */	 "STA is leaving / has left",
	/* 9  */	 "require auth before assoc/re-assoc",
	/* 10 */	 "Reserved",
	/* 11 */	 "Reserved",
	/* 12 */	 "Reserved",
	/* 13 */	 "invalid IE",
	/* 14 */	 "MIC error",
	/* 15 */	 "4-way handshake timeout",
	/* 16 */	 "2-way (group key) handshake timeout",
	/* 17 */	 "4-way handshake IE diff among AssosReq/Rsp/Beacon",
	/* 18 */
};
#endif

/* since RT61 has better RX sensibility, we have to limit TX ACK rate not to exceed our normal data TX rate.*/
/* otherwise the WLAN peer may not be able to receive the ACK thus downgrade its data TX rate*/
ULONG BasicRateMask[12] = {0xfffff001 /* 1-Mbps */, 0xfffff003 /* 2 Mbps */, 0xfffff007 /* 5.5 */, 0xfffff00f /* 11 */,
							0xfffff01f /* 6 */	 , 0xfffff03f /* 9 */	  , 0xfffff07f /* 12 */ , 0xfffff0ff /* 18 */,
							0xfffff1ff /* 24 */	 , 0xfffff3ff /* 36 */	  , 0xfffff7ff /* 48 */ , 0xffffffff /* 54 */};

UCHAR BROADCAST_ADDR[MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* e.g. RssiSafeLevelForTxRate[RATE_36]" means if the current RSSI is greater than*/
/*		this value, then it's quaranteed capable of operating in 36 mbps TX rate in*/
/*		clean environment.*/
/*								  TxRate: 1   2   5.5	11	 6	  9    12	18	 24   36   48	54	 72  100*/
CHAR RssiSafeLevelForTxRate[] ={  -92, -91, -90, -87, -88, -86, -85, -83, -81, -78, -72, -71, -40, -40 };

UCHAR  RateIdToMbps[] = { 1, 2, 5, 11, 6, 9, 12, 18, 24, 36, 48, 54, 72, 100};
USHORT RateIdTo500Kbps[] = { 2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 144, 200};

UCHAR SsidIe = IE_SSID;
UCHAR SupRateIe = IE_SUPP_RATES;
UCHAR ExtRateIe = IE_EXT_SUPP_RATES;
#ifdef DOT11_N_SUPPORT
UCHAR HtCapIe = IE_HT_CAP;
UCHAR AddHtInfoIe = IE_ADD_HT;
UCHAR NewExtChanIe = IE_SECONDARY_CH_OFFSET;
UCHAR BssCoexistIe = IE_2040_BSS_COEXIST;
UCHAR ExtHtCapIe = IE_EXT_CAPABILITY;
#endif /* DOT11_N_SUPPORT */
UCHAR ExtCapIe = IE_EXT_CAPABILITY;
UCHAR ErpIe = IE_ERP;
UCHAR DsIe = IE_DS_PARM;
UCHAR TimIe = IE_TIM;
UCHAR WpaIe = IE_WPA;
UCHAR Wpa2Ie = IE_WPA2;
UCHAR IbssIe = IE_IBSS_PARM;
UCHAR WapiIe = IE_WAPI;

extern UCHAR	WPA_OUI[];

UCHAR SES_OUI[] = {0x00, 0x90, 0x4c};

UCHAR ZeroSsid[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


#ifdef CONFIG_STA_SUPPORT
static void periodic_monitor_ac_counter(RTMP_ADAPTER *pAd, UINT32 QId)
{
	static BOOLEAN curStat=FALSE;
	UINT i = 0;
	BOOLEAN isUpdate=TRUE;

	if(pAd->RalinkCounters.OneSecOsTxCount[QId]> 500)
	{
		for(i=0;i<NUM_OF_TX_RING;i++)
		{
			
			if(i!=QId && (pAd->RalinkCounters.OneSecOsTxCount[i] > 500))
			{
					isUpdate = FALSE;
			}
		}
	}else
	{
		isUpdate = FALSE;
	}
	/*update AIFS*/
	if(!curStat && isUpdate)
	{
		AsicSetWmmParam(pAd,QId,WMM_PARAM_AIFSN,pAd->CommonCfg.APEdcaParm.Aifsn[QId]+2);
		curStat = TRUE;
	}else
	if(curStat && !isUpdate)
	{
		AsicSetWmmParam(pAd,QId,WMM_PARAM_AIFSN,pAd->CommonCfg.APEdcaParm.Aifsn[QId]);
		curStat = FALSE;		
	}
}
#endif


#ifdef DYNAMIC_VGA_SUPPORT
void periodic_monitor_false_cca_adjust_vga(RTMP_ADAPTER *pAd)
{
	if ((pAd->CommonCfg.lna_vga_ctl.bDyncVgaEnable) && (pAd->chipCap.dynamic_vga_support) &&
		OPSTATUS_TEST_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED)) {
		UCHAR val1, val2;
		UINT32 bbp_val1, bbp_val2;

		RTMP_BBP_IO_READ32(pAd, AGC1_R8, &bbp_val1);
		val1 = ((bbp_val1 & (0x00007f00)) >> 8) & 0x7f;
		RTMP_BBP_IO_READ32(pAd, AGC1_R9, &bbp_val2);
		val2 = ((bbp_val2 & (0x00007f00)) >> 8) & 0x7f;

		DBGPRINT(RT_DEBUG_INFO, ("vga_init_0 = %x, vga_init_1 = %x\n",  pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0, pAd->CommonCfg.lna_vga_ctl.agc_vga_init_1));
		DBGPRINT(RT_DEBUG_INFO,
			("one second False CCA=%d, fixed agc_vga_0:0%x, fixed agc_vga_1:0%x\n", pAd->RalinkCounters.OneSecFalseCCACnt, val1, val2));

		if (pAd->RalinkCounters.OneSecFalseCCACnt > pAd->CommonCfg.lna_vga_ctl.nFalseCCATh) {
			if (val1 > (pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0 - 0x10)) {
				val1 -= 0x02;
				bbp_val1 = (bbp_val1 & 0xffff80ff) | (val1 << 8);
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val1);
#ifdef DFS_SUPPORT
       		pAd->CommonCfg.RadarDetect.bAdjustDfsAgc = TRUE;
#endif
			}

			if (pAd->CommonCfg.RxStream >= 2) {
				if (val2 > (pAd->CommonCfg.lna_vga_ctl.agc_vga_init_1 - 0x10)) {
					val2 -= 0x02;
					bbp_val2 = (bbp_val2 & 0xffff80ff) | (val2 << 8);
					RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val2);
				}
			}
		} else if (pAd->RalinkCounters.OneSecFalseCCACnt < 
					pAd->CommonCfg.lna_vga_ctl.nLowFalseCCATh) {
			if (val1 < pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0) {
				val1 += 0x02;
				bbp_val1 = (bbp_val1 & 0xffff80ff) | (val1 << 8);
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val1);
#ifdef DFS_SUPPORT
				pAd->CommonCfg.RadarDetect.bAdjustDfsAgc = TRUE;
#endif
			}

			if (pAd->CommonCfg.RxStream >= 2) {
				if (val2 < pAd->CommonCfg.lna_vga_ctl.agc_vga_init_1) {
					val2 += 0x02;
					bbp_val2 = (bbp_val2 & 0xffff80ff) | (val2 << 8);
					RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val2);
				}
			}
		}
	}
}

#ifdef CONFIG_STA_SUPPORT
void periodic_monitor_rssi_adjust_vga(RTMP_ADAPTER *pAd)
{
	UINT32 bbp_val1, bbp_val2;
	UCHAR val1, val2;
	CHAR Rssi = RTMPAvgRssi(pAd, &pAd->StaCfg.RssiSample);
	RTMP_BBP_IO_READ32(pAd, AGC1_R8, &bbp_val1);
	RTMP_BBP_IO_READ32(pAd, AGC1_R9, &bbp_val2);
	
	DBGPRINT(RT_DEBUG_INFO, ("vga_init_0 = %x, vga_init_1 = %x\n",  pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0, pAd->CommonCfg.lna_vga_ctl.agc_vga_init_1));

	if (Rssi > -60) {
		val1 = pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0 - 0x20;
		bbp_val1 = (bbp_val1 & 0xffff80ff) | (val1 << 8);

		bbp_val1 &= ~(0xffff << 16);
		bbp_val1 |= ((0x1836) << 16);

		RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val1);

		val2 = pAd->CommonCfg.lna_vga_ctl.agc_vga_init_1 - 0x20;
		bbp_val2 = (bbp_val2 & 0xffff80ff) | (val2 << 8);
		
		bbp_val2 &= ~(0xffff << 16);
		bbp_val2 |= ((0x1836) << 16);
		
		RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val2);


	} else if ((Rssi >= -75) && (Rssi <= -60)) {
		val1 = pAd->CommonCfg.lna_vga_ctl.agc_vga_init_0 - 0x10;
		bbp_val1 = (bbp_val1 & 0xffff80ff) | (val1 << 8);

		bbp_val1 &= ~(0xffff << 16);
		bbp_val1 |= ((pAd->CommonCfg.lna_vga_ctl.agc_0_vga_set1_2 & 0xffff) << 16);

		RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val1);

		val2 = pAd->CommonCfg.lna_vga_ctl.agc_vga_init_1 - 0x10;
		bbp_val2 = (bbp_val2 & 0xffff80ff) | (val2 << 8);
		
		bbp_val2 &= ~(0xffff << 16);
		bbp_val2 |= ((pAd->CommonCfg.lna_vga_ctl.agc_1_vga_set1_2 & 0xffff) << 16);


		RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val2);
	} else {
		periodic_monitor_false_cca_adjust_vga(pAd);
		bbp_val1 &= ~(0xffff << 16);
		bbp_val1 |= ((pAd->CommonCfg.lna_vga_ctl.agc_0_vga_set1_2 & 0xffff) << 16);
		
		bbp_val2 &= ~(0xffff << 16);
		bbp_val2 |= ((pAd->CommonCfg.lna_vga_ctl.agc_1_vga_set1_2 & 0xffff) << 16);
	}
}
#endif
#endif

#ifdef MT76x2
#ifdef CONFIG_STA_SUPPORT
void periodic_check_channel_smoothing(RTMP_ADAPTER *ad)
{
	UINT32 bbp_value;
	CHAR Rssi = RTMPAvgRssi(ad, &ad->StaCfg.RssiSample);

	if (Rssi < -50) {
		if (!ad->chipCap.chl_smth_enable) {
			RTMP_BBP_IO_READ32(ad, 0x2948, &bbp_value);
			bbp_value &= ~(0x1);
			bbp_value |= (0x1);
			RTMP_BBP_IO_WRITE32(ad, 0x2948, bbp_value);

			RTMP_BBP_IO_READ32(ad, 0x2944, &bbp_value);
			bbp_value &= ~(0x1);
			RTMP_BBP_IO_WRITE32(ad, 0x2944, bbp_value);

			ad->chipCap.chl_smth_enable = TRUE;
		}
	} else {
		if (ad->chipCap.chl_smth_enable) {
			RTMP_BBP_IO_READ32(ad, 0x2948, &bbp_value);
			bbp_value &= ~(0x1);
			RTMP_BBP_IO_WRITE32(ad, 0x2948, bbp_value);

			RTMP_BBP_IO_READ32(ad, 0x2944, &bbp_value);
			bbp_value &= ~(0x1);
			bbp_value |= (0x1);
			RTMP_BBP_IO_WRITE32(ad, 0x2944, bbp_value);

			ad->chipCap.chl_smth_enable = FALSE;
		}
	}
}
#endif
#endif

VOID set_default_ap_edca_param(RTMP_ADAPTER *pAd)
{
	pAd->CommonCfg.APEdcaParm.bValid = TRUE;
	pAd->CommonCfg.APEdcaParm.Aifsn[0] = 3;
	pAd->CommonCfg.APEdcaParm.Aifsn[1] = 7;
	pAd->CommonCfg.APEdcaParm.Aifsn[2] = 1;
	pAd->CommonCfg.APEdcaParm.Aifsn[3] = 1;

	pAd->CommonCfg.APEdcaParm.Cwmin[0] = 4;
	pAd->CommonCfg.APEdcaParm.Cwmin[1] = 4;
	pAd->CommonCfg.APEdcaParm.Cwmin[2] = 3;
	pAd->CommonCfg.APEdcaParm.Cwmin[3] = 2;

	pAd->CommonCfg.APEdcaParm.Cwmax[0] = 6;
	pAd->CommonCfg.APEdcaParm.Cwmax[1] = 10;
	pAd->CommonCfg.APEdcaParm.Cwmax[2] = 4;
	pAd->CommonCfg.APEdcaParm.Cwmax[3] = 3;

	pAd->CommonCfg.APEdcaParm.Txop[0]  = 0;
	pAd->CommonCfg.APEdcaParm.Txop[1]  = 0;
	pAd->CommonCfg.APEdcaParm.Txop[2]  = 94;	
	pAd->CommonCfg.APEdcaParm.Txop[3]  = 47;	
}


#ifdef CONFIG_AP_SUPPORT
VOID set_default_sta_edca_param(RTMP_ADAPTER *pAd)
{
	pAd->ApCfg.BssEdcaParm.bValid = TRUE;
	pAd->ApCfg.BssEdcaParm.Aifsn[0] = 3;
	pAd->ApCfg.BssEdcaParm.Aifsn[1] = 7;
	pAd->ApCfg.BssEdcaParm.Aifsn[2] = 2;
	pAd->ApCfg.BssEdcaParm.Aifsn[3] = 2;

	pAd->ApCfg.BssEdcaParm.Cwmin[0] = 4;
	pAd->ApCfg.BssEdcaParm.Cwmin[1] = 4;
	pAd->ApCfg.BssEdcaParm.Cwmin[2] = 3;
	pAd->ApCfg.BssEdcaParm.Cwmin[3] = 2;

	pAd->ApCfg.BssEdcaParm.Cwmax[0] = 10;
	pAd->ApCfg.BssEdcaParm.Cwmax[1] = 10;
	pAd->ApCfg.BssEdcaParm.Cwmax[2] = 4;
	pAd->ApCfg.BssEdcaParm.Cwmax[3] = 3;

	pAd->ApCfg.BssEdcaParm.Txop[0]  = 0;
	pAd->ApCfg.BssEdcaParm.Txop[1]  = 0;
	pAd->ApCfg.BssEdcaParm.Txop[2]  = 94;	/*96; */
	pAd->ApCfg.BssEdcaParm.Txop[3]  = 47;	/*48; */
}
#endif /* CONFIG_AP_SUPPORT */


UCHAR dot11_max_sup_rate(INT SupRateLen, UCHAR *SupRate, INT ExtRateLen, UCHAR *ExtRate)
{
	INT idx;
	UCHAR MaxSupportedRateIn500Kbps = 0;
	
	/* supported rates array may not be sorted. sort it and find the maximum rate */
	for (idx = 0; idx < SupRateLen; idx++) {
		if (MaxSupportedRateIn500Kbps < (SupRate[idx] & 0x7f))
			MaxSupportedRateIn500Kbps = SupRate[idx] & 0x7f;
	}

	if (ExtRateLen > 0 && ExtRate != NULL)
	{
		for (idx = 0; idx < ExtRateLen; idx++) {
			if (MaxSupportedRateIn500Kbps < (ExtRate[idx] & 0x7f))
				MaxSupportedRateIn500Kbps = ExtRate[idx] & 0x7f;
		}
	}

	return MaxSupportedRateIn500Kbps;
}


UCHAR dot11_2_ra_rate(UCHAR MaxSupportedRateIn500Kbps)
{
	UCHAR MaxSupportedRate;
	
	switch (MaxSupportedRateIn500Kbps)
	{
		case 108: MaxSupportedRate = RATE_54;   break;
		case 96:  MaxSupportedRate = RATE_48;   break;
		case 72:  MaxSupportedRate = RATE_36;   break;
		case 48:  MaxSupportedRate = RATE_24;   break;
		case 36:  MaxSupportedRate = RATE_18;   break;
		case 24:  MaxSupportedRate = RATE_12;   break;
		case 18:  MaxSupportedRate = RATE_9;    break;
		case 12:  MaxSupportedRate = RATE_6;    break;
		case 22:  MaxSupportedRate = RATE_11;   break;
		case 11:  MaxSupportedRate = RATE_5_5;  break;
		case 4:   MaxSupportedRate = RATE_2;    break;
		case 2:   MaxSupportedRate = RATE_1;    break;
		default:  MaxSupportedRate = RATE_11;   break;
	}

	return MaxSupportedRate;
}


/*
	========================================================================

	Routine Description:
		Suspend MSDU transmission
		
	Arguments:
		pAd 	Pointer to our adapter
		
	Return Value:
		None
		
	Note:
	
	========================================================================
*/
VOID RTMPSuspendMsduTransmission(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_TRACE,("SCANNING, suspend MSDU transmission ...\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT /* Roger sync Carrier */
	/* no carrier detection when scanning */
	if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
		CarrierDetectionStop(pAd);
#endif
#endif /* CONFIG_AP_SUPPORT */
	
	/*
		Before BSS_SCAN_IN_PROGRESS, we need to keep Current R66 value and
		use Lowbound as R66 value on ScanNextChannel(...)
	*/
	bbp_get_agc(pAd, &pAd->BbpTuning.R66CurrentValue, RX_CHAIN_0);

	pAd->hw_cfg.bbp_bw = pAd->CommonCfg.BBPCurrentBW;

	RTMPSetAGCInitValue(pAd, BW_20);
	
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS);
}


/*
	========================================================================

	Routine Description:
		Resume MSDU transmission
		
	Arguments:
		pAd 	Pointer to our adapter
		
	Return Value:
		None
		
	IRQL = DISPATCH_LEVEL
	
	Note:
	
	========================================================================
*/
VOID RTMPResumeMsduTransmission(RTMP_ADAPTER *pAd)
{  
	DBGPRINT(RT_DEBUG_TRACE,("SCAN done, resume MSDU transmission ...\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
	/* no carrier detection when scanning*/
	if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
			CarrierDetectionStart(pAd);
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	/*
		After finish BSS_SCAN_IN_PROGRESS, we need to restore Current R66 value
		R66 should not be 0
	*/
	if (pAd->BbpTuning.R66CurrentValue == 0)
	{
		pAd->BbpTuning.R66CurrentValue = 0x38;
		DBGPRINT_ERR(("RTMPResumeMsduTransmission, R66CurrentValue=0...\n"));
	}
	bbp_set_agc(pAd, pAd->BbpTuning.R66CurrentValue, RX_CHAIN_ALL);
	
	pAd->CommonCfg.BBPCurrentBW = pAd->hw_cfg.bbp_bw;
	
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS);
/* sample, for IRQ LOCK to SEM LOCK */
/*
	IrqState = pAd->irq_disabled;
	if (IrqState)
		RTMPDeQueuePacket(pAd, TRUE, NUM_OF_TX_RING, MAX_TX_PROCESS);
	else
*/
	RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
}


/* 
	==========================================================================
	Description:
		Send out a NULL frame to a specified STA at a higher TX rate. The 
		purpose is to ensure the designated client is okay to received at this
		rate.
	==========================================================================
 */
VOID RtmpEnqueueNullFrame(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR *pAddr,
	IN UCHAR TxRate,
	IN UCHAR AID,
	IN UCHAR apidx,
	IN BOOLEAN bQosNull,
	IN BOOLEAN bEOSP,
	IN UCHAR OldUP)
{
	NDIS_STATUS NState;
	HEADER_802_11 *pNullFr;
	UCHAR *pFrame;
	UINT frm_len;
#if 1//defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
	MAC_TABLE_ENTRY *pEntry;
	pEntry = MacTableLookup(pAd, pAddr);
#endif /* DOT11Z_TDLS_SUPPORT */

	NState = MlmeAllocateMemory(pAd, (UCHAR **)&pFrame);
	pNullFr = (PHEADER_802_11) pFrame;

	if (NState == NDIS_STATUS_SUCCESS) 
	{
		frm_len = sizeof(HEADER_802_11);

#ifdef CONFIG_AP_SUPPORT
//		IF_DEV_CONFIG_OPMODE_ON_AP(pAd) 
		if (pEntry && pEntry->wdev->wdev_type == WDEV_TYPE_AP)
		{
			MgtMacHeaderInit(pAd, pNullFr, SUBTYPE_DATA_NULL, 0, pAddr, 
							pAd->ApCfg.MBSSID[apidx].wdev.if_addr,
							pAd->ApCfg.MBSSID[apidx].wdev.bssid);
			pNullFr->FC.ToDs = 0;
			pNullFr->FC.FrDs = 1;
			goto body;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd) {
			pNullFr->FC.FrDs = 0;
			pNullFr->FC.ToDs = 1;

#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
			if ((pEntry != NULL) && (IS_ENTRY_TDLS(pEntry)))
			{
				pNullFr->FC.FrDs = 0;
				pNullFr->FC.ToDs = 0;

				COPY_MAC_ADDR(pNullFr->Addr1, pAddr);
				COPY_MAC_ADDR(pNullFr->Addr2, pAd->CurrentAddress);
				COPY_MAC_ADDR(pNullFr->Addr3, pAd->CommonCfg.Bssid);
			}
#endif /* DOT11Z_TDLS_SUPPORT */
		}
#endif /* CONFIG_STA_SUPPORT */

body:
		pNullFr->FC.Type = FC_TYPE_DATA;

		if (bQosNull)
		{
			UCHAR *qos_p = ((UCHAR *)pNullFr) + frm_len;

			pNullFr->FC.SubType = SUBTYPE_QOS_NULL;

			/* copy QOS control bytes */
			qos_p[0] = ((bEOSP) ? (1 << 4) : 0) | OldUP;
			qos_p[1] = 0;
			frm_len += 2;
		} else
			pNullFr->FC.SubType = SUBTYPE_DATA_NULL;

		/* since TxRate may change, we have to change Duration each time */
		pNullFr->Duration = RTMPCalcDuration(pAd, TxRate, frm_len);

		DBGPRINT(RT_DEBUG_INFO, ("send NULL Frame @%d Mbps to AID#%d...\n", RateIdToMbps[TxRate], AID & 0x3f));
		MiniportMMRequest(pAd, WMM_UP2AC_MAP[7], (PUCHAR)pNullFr, frm_len);

		MlmeFreeMemory(pAd, pFrame);
	}
}


#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
VOID ApCliRTMPSendNullFrame(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			TxRate,
	IN	BOOLEAN 		bQosNull,
	IN 	PMAC_TABLE_ENTRY pMacEntry,
	IN	USHORT          PwrMgmt)
{
	UCHAR NullFrame[48];
	ULONG Length;
	HEADER_802_11 *wifi_hdr;
	STA_TR_ENTRY *tr_entry;
	PAPCLI_STRUCT pApCliEntry = NULL;
	struct wifi_dev *wdev;
	
	pApCliEntry = &pAd->ApCfg.ApCliTab[pMacEntry->func_tb_idx];
	tr_entry = &pAd->MacTab.tr_entry[pMacEntry->wcid];
	wdev = &pApCliEntry->wdev;

    /* WPA 802.1x secured port control */
	// TODO: shiang-usw, check [wdev/tr_entry]->PortSecured!
	if ((wdev->PortSecured == WPA_802_1X_PORT_NOT_SECURED) ||
	    (tr_entry->PortSecured == WPA_802_1X_PORT_NOT_SECURED)) 
		return;

	NdisZeroMemory(NullFrame, 48);
	Length = sizeof(HEADER_802_11);

	wifi_hdr = (HEADER_802_11 *)NullFrame;
	
	wifi_hdr->FC.Type = FC_TYPE_DATA;
	wifi_hdr->FC.SubType = SUBTYPE_DATA_NULL;
	wifi_hdr->FC.ToDs = 1;

	COPY_MAC_ADDR(wifi_hdr->Addr1, pMacEntry->Addr);
#ifdef MAC_REPEATER_SUPPORT
	if (pMacEntry && (pMacEntry->bReptCli == TRUE))
		COPY_MAC_ADDR(wifi_hdr->Addr2, pMacEntry->ReptCliAddr);
	else
#endif /* MAC_REPEATER_SUPPORT */
		COPY_MAC_ADDR(wifi_hdr->Addr2, pApCliEntry->wdev.if_addr);
	COPY_MAC_ADDR(wifi_hdr->Addr3, pMacEntry->Addr);

	if (pAd->CommonCfg.bAPSDForcePowerSave)
		wifi_hdr->FC.PwrMgmt = PWR_SAVE;
	else
		wifi_hdr->FC.PwrMgmt = PwrMgmt;
	wifi_hdr->Duration = pAd->CommonCfg.Dsifs + RTMPCalcDuration(pAd, TxRate, 14);

	/* sequence is increased in MlmeHardTx */
	wifi_hdr->Sequence = pAd->Sequence;
	pAd->Sequence = (pAd->Sequence + 1) & MAXSEQ;	/* next sequence  */

	/* Prepare QosNull function frame */
	if (bQosNull) {
		wifi_hdr->FC.SubType = SUBTYPE_QOS_NULL;
		
		/* copy QOS control bytes */
		NullFrame[Length] = 0;
		NullFrame[Length + 1] = 0;
		Length += 2;	/* if pad with 2 bytes for alignment, APSD will fail */
	}
	
	HAL_KickOutNullFrameTx(pAd, 0, NullFrame, Length);
}
#endif/*APCLI_SUPPORT*/
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
VOID LowPowerDebug(
	IN PRTMP_ADAPTER pAd)
{
	DBGPRINT(RT_DEBUG_TRACE, ("\n************ Previous Setting **************************\n"));
	DBGPRINT(RT_DEBUG_TRACE, ("%s::pAd->StaCfg.WindowsBatteryPowerMode(%lu)\n", __FUNCTION__, pAd->StaCfg.WindowsBatteryPowerMode));
	DBGPRINT(RT_DEBUG_TRACE, ("%s::pAd->StaCfg.WindowsPowerMode(%lu)\n", __FUNCTION__, pAd->StaCfg.WindowsPowerMode));
	DBGPRINT(RT_DEBUG_TRACE, ("%s::pAd->CommonCfg.bAPSDForcePowerSave(%d)\n", __FUNCTION__, pAd->CommonCfg.bAPSDForcePowerSave));
	DBGPRINT(RT_DEBUG_TRACE, ("%s::fOP_STATUS_DOZE(%d)\n", __FUNCTION__, OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE)));
	DBGPRINT(RT_DEBUG_TRACE, ("%s::fpAd->StaCfg.PwrMgmt.psm(%d)\n", __FUNCTION__, pAd->StaCfg.PwrMgmt.Psm));
	DBGPRINT(RT_DEBUG_TRACE, ("*****************************************************\n\n"));
}

VOID RTMPSendNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR TxRate,
	IN BOOLEAN bQosNull,
	IN USHORT PwrMgmt)
{
	UCHAR NullFrame[48];
	ULONG Length;
	HEADER_802_11 *wifi_hdr;
	//STA_TR_ENTRY *tr_entry;
	struct wifi_dev *wdev;
#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		return;
#endif /* CONFIG_ATE */

	wdev = &pAd->StaCfg.wdev;

	/* WPA 802.1x secured port control */
	if (wdev->PortSecured == WPA_802_1X_PORT_NOT_SECURED)
		return;

	NdisZeroMemory(NullFrame, 48);
	Length = sizeof(HEADER_802_11);

	wifi_hdr = (HEADER_802_11 *)NullFrame;

	wifi_hdr->FC.Type = FC_TYPE_DATA;
	wifi_hdr->FC.SubType = SUBTYPE_DATA_NULL;
	wifi_hdr->FC.ToDs = 1;

	COPY_MAC_ADDR(wifi_hdr->Addr1, pAd->CommonCfg.Bssid);
	COPY_MAC_ADDR(wifi_hdr->Addr2, pAd->CurrentAddress);
	COPY_MAC_ADDR(wifi_hdr->Addr3, pAd->CommonCfg.Bssid);

	if (pAd->CommonCfg.bAPSDForcePowerSave) {
		wifi_hdr->FC.PwrMgmt = PWR_SAVE;
	} else {
		BOOLEAN FlgCanPmBitSet = TRUE;

#ifdef DOT11Z_TDLS_SUPPORT
		/* check TDLS condition */
		if (pAd->StaCfg.TdlsInfo.TdlsFlgIsKeepingActiveCountDown == TRUE)
			FlgCanPmBitSet = FALSE;
#endif /* DOT11Z_TDLS_SUPPORT */

		if (FlgCanPmBitSet == TRUE)
		wifi_hdr->FC.PwrMgmt = PwrMgmt;
		else
			wifi_hdr->FC.PwrMgmt = PWR_ACTIVE;
	}
	wifi_hdr->Duration = pAd->CommonCfg.Dsifs + RTMPCalcDuration(pAd, TxRate, 14);

	/* sequence is increased in MlmeHardTx */
	wifi_hdr->Sequence = pAd->Sequence;
	pAd->Sequence = (pAd->Sequence + 1) & MAXSEQ;	/* next sequence  */

	/* Prepare QosNull function frame */
	if (bQosNull) {
		wifi_hdr->FC.SubType = SUBTYPE_QOS_NULL;

		/* copy QOS control bytes */
		NullFrame[Length] = 0;
		NullFrame[Length + 1] = 0;
		Length += 2;	/* if pad with 2 bytes for alignment, APSD will fail */
	}

	HAL_KickOutNullFrameTx(pAd, 0, NullFrame, Length);
}

INT32 RTMPSendNullFrameAndWaitStatus(
	IN PRTMP_ADAPTER pAd,
	IN USHORT PwrMgmt)
{
	pAd->cfg80211_ctrl.TxSAcked = 0;
	pAd->cfg80211_ctrl.TxStatusInUsed = TRUE;

	RTMP_OS_INIT_COMPLETION(&pAd->cfg80211_ctrl.fw_event_done);

	RTMPSendNullFrame(pAd, 
			pAd->CommonCfg.TxRate,
			(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED) ? TRUE:FALSE),
			PwrMgmt);

	return RTMPWaitNullFrameTxDone(pAd);
}

VOID RTMPOffloadPm(RTMP_ADAPTER *pAd, UINT8 ucWlanIdx, UINT8 ucPmNumber, UINT8 ucPmState)
{
	//STA_TR_ENTRY *ptr_entry = &pAd->MacTab.tr_entry[ucWlanIdx];
	//struct wifi_dev *wdev = &pAd->StaCfg.wdev;
	ULONG	u4Now;

	DBGPRINT(RT_DEBUG_TRACE, ("%s(%d)\n", __FUNCTION__, __LINE__));
	LowPowerDebug(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s--->::Current setting, ucWlanIdx(%d), ucPmNumber(%d), ucPmState(%d)\n", __FUNCTION__, ucWlanIdx, ucPmNumber, ucPmState));

	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) && (ucPmState == ENTER_PM_STATE))
	{
		/* H/W enter PM4 offlaod here */
		DBGPRINT(RT_DEBUG_TRACE, ("%s::***** H/W Enter PM4 *****\n", __FUNCTION__));
		//CmdExtPwrMgtBitWifi(pAd, ucWlanIdx, PWR_SAVE);
		CmdExtPmStateCtrl(pAd, ucWlanIdx, ucPmNumber, ucPmState);
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_DOZE);
	}
	else if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) && (ucPmState == EXIT_PM_STATE))
	{
		/* H/W exit PM4 offlaod here */
		DBGPRINT(RT_DEBUG_TRACE, ("%s::***** H/W Exit PM4 *****\n", __FUNCTION__));
		NdisGetSystemUpTime(&u4Now);
#ifdef CONFIG_STA_SUPPORT
		pAd->StaCfg.LastBeaconRxTime= u4Now;
#endif
		//CmdExtPwrMgtBitWifi(pAd, ucWlanIdx, PWR_ACTIVE);
		CmdExtPmStateCtrl(pAd, ucWlanIdx, ucPmNumber, ucPmState);
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_DOZE);

#ifdef RTMP_MAC_USB		
		/* For USB data Tx path, we use CMD thread to wake up H/W, data might be kicked out before H/W wake up,
		    we use fOP_STATUS_DOZE to return Tx and resume Tx after clearing flag fOP_STATUS_DOZE*/
		RTUSBKickBulkOut(pAd);
#endif /* RTMP_MAC_USB */

		DBGPRINT(RT_DEBUG_TRACE, ("%s::Update Beacon time virtually\n", __FUNCTION__)); 
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s(%d):: ***** No Need to handle this. *****\n", __FUNCTION__, __LINE__));
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s<---\n", __FUNCTION__));	
}
#endif /* CONFIG_STA_SUPPORT */

VOID RtmpPrepareHwNullFrame(
	IN PRTMP_ADAPTER pAd,
	IN PMAC_TABLE_ENTRY pEntry,
	IN BOOLEAN bQosNull,
	IN BOOLEAN bEOSP,
	IN UCHAR OldUP,
	IN UCHAR OpMode,
	IN UCHAR PwrMgmt,
	IN BOOLEAN bWaitACK,
	IN CHAR Index)
{

#if defined(MT7603) || defined(MT7628)
	/* nothing to handle */
#else
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	UCHAR *pNullFrame, *tmac_info = (UCHAR *)&pAd->NullTxWI;
	NDIS_STATUS NState;
	HEADER_802_11 *pNullFr;
	ULONG Length;
	UCHAR *ptr;
	UINT i;
	UINT32 longValue;
#ifdef P2P_SUPPORT
	PAPCLI_STRUCT pApCliEntry = NULL;
#endif /* P2P_SUPPORT */
	MAC_TX_INFO mac_info;

	NState = MlmeAllocateMemory(pAd, (PUCHAR *)&pNullFrame);

	NdisZeroMemory(pNullFrame, 48);
	NdisZeroMemory(tmac_info, TXWISize);

	if (NState == NDIS_STATUS_SUCCESS) 
	{
		pNullFr = (PHEADER_802_11) pNullFrame;
		Length = sizeof(HEADER_802_11);
		
		pNullFr->FC.Type = FC_TYPE_DATA;
		pNullFr->FC.SubType = SUBTYPE_DATA_NULL;
		if (Index == 1)
			pNullFr->FC.ToDs = 0;
		else
		pNullFr->FC.ToDs = 1;
		pNullFr->FC.FrDs = 0;

		COPY_MAC_ADDR(pNullFr->Addr1, pEntry->Addr);
#ifdef P2P_SUPPORT
		if (IS_ENTRY_APCLI(pEntry))
		{
			pApCliEntry = &pAd->ApCfg.ApCliTab[pEntry->func_tb_idx];
			COPY_MAC_ADDR(pNullFr->Addr2, pApCliEntry->wdev.if_addr);
			COPY_MAC_ADDR(pNullFr->Addr3, pApCliEntry->CfgApCliBssid);
		}
		else
#endif /* P2P_SUPPORT */
		{
			COPY_MAC_ADDR(pNullFr->Addr2, pAd->CurrentAddress);
			COPY_MAC_ADDR(pNullFr->Addr3, pAd->CommonCfg.Bssid);
		}

		pNullFr->FC.PwrMgmt = PwrMgmt;
	
		pNullFr->Duration = pAd->CommonCfg.Dsifs + RTMPCalcDuration(pAd, pAd->CommonCfg.TxRate, 14);

		/* sequence is increased in MlmeHardTx */
		pNullFr->Sequence = pAd->Sequence;
		pAd->Sequence = (pAd->Sequence+1) & MAXSEQ; /* next sequence  */

		if (bQosNull)
		{
			UCHAR *qos_p = ((UCHAR *)pNullFr) + Length;

			pNullFr->FC.SubType = SUBTYPE_QOS_NULL;

			/* copy QOS control bytes */
			qos_p[0] = ((bEOSP) ? (1 << 4) : 0) | OldUP;
			qos_p[1] = 0;
			Length += 2;
		}

	NdisZeroMemory((UCHAR *)&mac_info, sizeof(mac_info));	
	mac_info.FRAG = FALSE;
	
	mac_info.CFACK = FALSE;
	mac_info.InsTimestamp = FALSE;
	mac_info.AMPDU = FALSE;
	
	mac_info.BM = IS_BM_MAC_ADDR(pNullFr->Addr1);
	mac_info.Ack = TRUE;
	mac_info.NSeq = TRUE;
	mac_info.BASize = 0;
	
	mac_info.WCID = (UCHAR)(pEntry->Aid);
	mac_info.Length = Length;
	
	mac_info.TID = 0;
	mac_info.Txopmode = IFS_HTTXOP;
	mac_info.Preamble = LONG_PREAMBLE;
	mac_info.SpeEn = 1;
	
		if (Index == 1)
		{
			HTTRANSMIT_SETTING Transmit;
				
			Transmit.word = pEntry->MaxHTPhyMode.word;
			Transmit.field.BW = 0;
			if (Transmit.field.MCS > 7)
				Transmit.field.MCS = 7;

			mac_info.PID = (UCHAR)Transmit.field.MCS;
			mac_info.TxRate = (UCHAR)Transmit.field.MCS;
			write_tmac_info(pAd, tmac_info, &mac_info, &Transmit);
		}
		else
		{
			mac_info.PID = (UCHAR)pAd->CommonCfg.MlmeTransmit.field.MCS;
			mac_info.TxRate = (UCHAR)pAd->CommonCfg.MlmeTransmit.field.MCS;
			write_tmac_info(pAd, tmac_info, &mac_info,
							&pAd->CommonCfg.MlmeTransmit);
		}

		if (bWaitACK) {
#ifdef RTMP_MAC
			if (pAd->chipCap.hif_type == HIF_RTMP) {
				TXWI_STRUC *pTxWI = (TXWI_STRUC *)tmac_info;

				pTxWI->TXWI_O.TXRPT = 1;
			}
#endif /* RTMP_MAC */
#ifdef RLT_MAC
			// TODO: shiang, how about RT65xx series??
#ifdef MT7601
			if ((pAd->chipCap.hif_type == HIF_RLT) && (IS_MT7601(pAd))) {
				TXWI_STRUC *pTxWI = (TXWI_STRUC *)tmac_info;

				pTxWI->TXWI_X.TXRPT = 1;
			}
#endif /* MT7601 */
#endif /* RLT_MAC */
		}

		ptr = (PUCHAR)&pAd->NullTxWI;
#ifdef RT_BIG_ENDIAN
		RTMPWIEndianChange(pAd, ptr, TYPE_TXWI);
#endif /* RT_BIG_ENDIAN */
		for (i=0; i < TXWISize; i+=4)
		{
			longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
			if (Index == 0)
				RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[0] + i, longValue);
			else if (Index == 1)
				RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[1] + i, longValue);
				
			ptr += 4;
		}
		
		ptr = pNullFrame;
#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, ptr, DIR_WRITE, FALSE);
#endif /* RT_BIG_ENDIAN */
		for (i= 0; i< Length; i+=4)
		{
			longValue =  *ptr + (*(ptr + 1) << 8) + (*(ptr + 2) << 16) + (*(ptr + 3) << 24);
			if (Index == 0) //for ra0 
				RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[0] + TXWISize+ i, longValue);
			else if (Index == 1) //for p2p0
				RTMP_IO_WRITE32(pAd, pAd->NullBufOffset[1] + TXWISize+ i, longValue);
				
			ptr += 4;
		}
	}

	if (pNullFrame)
		MlmeFreeMemory(pAd, pNullFrame);
#endif /* defined(MT7603) || defined(MT7628) */
}


/*
	==========================================================================
	Description:
		main loop of the MLME
	Pre:
		Mlme has to be initialized, and there are something inside the queue
	Note:
		This function is invoked from MPSetInformation and MPReceive;
		This task guarantee only one FSM will run. 

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
VOID MlmeHandler(RTMP_ADAPTER *pAd) 
{
	MLME_QUEUE_ELEM *Elem = NULL;
#ifdef APCLI_SUPPORT
	SHORT apcliIfIndex;
#endif /* APCLI_SUPPORT */

	/* Only accept MLME and Frame from peer side, no other (control/data) frame should*/
	/* get into this state machine*/

	NdisAcquireSpinLock(&pAd->Mlme.TaskLock);
	if(pAd->Mlme.bRunning) 
	{
		NdisReleaseSpinLock(&pAd->Mlme.TaskLock);
		return;
	} 
	else 
	{
		pAd->Mlme.bRunning = TRUE;
	}
	NdisReleaseSpinLock(&pAd->Mlme.TaskLock);

	while (!MlmeQueueEmpty(&pAd->Mlme.Queue)) 
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_MLME_RESET_IN_PROGRESS) ||
			RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS) ||
			RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) ||
			RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_SUSPEND))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("System halted, removed or MlmeRest, exit MlmeTask!(QNum = %ld)\n",
						pAd->Mlme.Queue.Num));
			break;
		}
		
#ifdef CONFIG_ATE			
		if(ATE_ON(pAd))
		{
			DBGPRINT(RT_DEBUG_INFO, ("%s(): Driver is in ATE mode\n", __FUNCTION__));
			break;
		}	
#endif /* CONFIG_ATE */

		/*From message type, determine which state machine I should drive*/
		if (MlmeDequeue(&pAd->Mlme.Queue, &Elem)) 
		{
#ifdef RTMP_MAC_USB
			if (Elem->MsgType == MT2_RESET_CONF)
			{
				DBGPRINT_RAW(RT_DEBUG_TRACE, ("!!! reset MLME state machine !!!\n"));
#ifdef RT_CFG80211_SUPPORT
				if(pAd->cfg80211_ctrl.FlgCfg80211Scanning)
					pAd->cfg80211_ctrl.FlgCfg80211Scanning = 0;

				CFG80211OS_ScanEnd(pAd->pCfg80211_CB, TRUE);
#endif /* RT_CFG80211_SUPPORT */
				MlmeRestartStateMachine(pAd);
				Elem->Occupied = FALSE;
				Elem->MsgLen = 0;
				continue;
			}
#endif /* RTMP_MAC_USB */

#if 1 //fix mic error when EAPOL enqueue before linkup
			if (Elem->Machine == WPA_STATE_MACHINE && Elem->MsgType == MT2_EAPOLKey)
			{
						if (((!OPSTATUS_TEST_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED)) &&
				( !OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)))
#ifdef MESH_SUPPORT
					&& !MESH_ON(pAd)
#endif /* MESH_SUPPORT */
#ifdef P2P_SUPPORT
					&& !(P2P_GO_ON(pAd) || P2P_CLI_ON(pAd))
#endif /* P2P_SUPPORT */
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
					&& !(RTMP_CFG80211_VIF_P2P_GO_ON(pAd) || RTMP_CFG80211_VIF_P2P_CLI_ON(pAd))
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */

				)
				{
					MLME_QUEUE	*Queue = (MLME_QUEUE *)&pAd->Mlme.Queue;
					ULONG Tail;
					NdisAcquireSpinLock(&(Queue->Lock));
					Tail = Queue->Tail;
					Queue->Tail++;
					Queue->Num++;
					if (Queue->Tail == MAX_LEN_OF_MLME_QUEUE) 
						Queue->Tail = 0;
					
					Queue->Entry[Tail].Wcid = RESERVED_WCID;
					Queue->Entry[Tail].Occupied = TRUE;
					Queue->Entry[Tail].Machine = Elem->Machine;
					Queue->Entry[Tail].MsgType = Elem->MsgType;
					Queue->Entry[Tail].MsgLen = Elem->MsgLen;	
					Queue->Entry[Tail].Priv = Elem->Priv;
					NdisZeroMemory(Queue->Entry[Tail].Msg, MGMT_DMA_BUFFER_SIZE);
					if (Elem->Msg != NULL)
					{
						NdisMoveMemory(Queue->Entry[Tail].Msg, Elem->Msg, Elem->MsgLen);
					}
						
					NdisReleaseSpinLock(&(Queue->Lock));

					Elem->Occupied = FALSE;
					Elem->MsgLen = 0;
					continue;
					
				}
			}
#endif			

			/* if dequeue success*/
			switch (Elem->Machine) 
			{
				/* STA state machines*/
#ifdef CONFIG_STA_SUPPORT
				case ASSOC_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.AssocMachine,
										Elem, pAd->Mlme.AssocMachine.CurrState);
					break;

				case AUTH_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.AuthMachine,
										Elem, pAd->Mlme.AuthMachine.CurrState);
					break;

				case AUTH_RSP_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.AuthRspMachine,
										Elem, pAd->Mlme.AuthRspMachine.CurrState);
					break;

				case SYNC_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.SyncMachine,
										Elem, pAd->Mlme.SyncMachine.CurrState);
					break;

				case MLME_CNTL_STATE_MACHINE:
					MlmeCntlMachinePerformAction(pAd, &pAd->Mlme.CntlMachine, Elem);
					break;

				case WPA_PSK_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.WpaPskMachine,
										Elem, pAd->Mlme.WpaPskMachine.CurrState);
					break;	

#ifdef QOS_DLS_SUPPORT
				case DLS_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.DlsMachine,
										Elem, pAd->Mlme.DlsMachine.CurrState);
					break;
#endif /* QOS_DLS_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
				case FT_OTA_AUTH_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.FtOtaAuthMachine,
										Elem, pAd->Mlme.FtOtaAuthMachine.CurrState);
					break;

				case FT_OTD_ACT_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.FtOtdActMachine,
										Elem, pAd->Mlme.FtOtdActMachine.CurrState);
					break;
#endif /* DOT11R_FT_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
				case TDLS_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.TdlsMachine,
										Elem, pAd->Mlme.TdlsMachine.CurrState);
					break;
#endif /* DOT11Z_TDLS_SUPPORT */

#ifdef STA_EASY_CONFIG_SETUP
				case AUTO_PROVISION_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.AutoProvisionMachine,
										Elem, pAd->Mlme.AutoProvisionMachine.CurrState);
					break;
#endif /* STA_EASY_CONFIG_SETUP */

#endif /* CONFIG_STA_SUPPORT */						

				case ACTION_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.ActMachine,
										Elem, pAd->Mlme.ActMachine.CurrState);
					break;	

#ifdef CONFIG_AP_SUPPORT
				/* AP state amchines*/

				case AP_ASSOC_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.ApAssocMachine,
									Elem, pAd->Mlme.ApAssocMachine.CurrState);
					break;

				case AP_AUTH_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.ApAuthMachine, 
									Elem, pAd->Mlme.ApAuthMachine.CurrState);
					break;

				case AP_SYNC_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.ApSyncMachine,
									Elem, pAd->Mlme.ApSyncMachine.CurrState);
					break;

#ifdef APCLI_SUPPORT
				case APCLI_AUTH_STATE_MACHINE:
					apcliIfIndex = (SHORT)Elem->Priv;
#ifdef MAC_REPEATER_SUPPORT
					if (apcliIfIndex >= 64)
						apcliIfIndex = ((apcliIfIndex - 64) / 16);		
#endif /* MAC_REPEATER_SUPPORT */

					if(isValidApCliIf(apcliIfIndex))
					{
						ULONG AuthCurrState;
#ifdef MAC_REPEATER_SUPPORT
						UCHAR CliIdx = 0;

						apcliIfIndex = (SHORT)Elem->Priv;

						if (apcliIfIndex >= 64)
						{
							CliIdx = ((apcliIfIndex - 64) % 16);
							apcliIfIndex = ((apcliIfIndex - 64) / 16);

							AuthCurrState = pAd->ApCfg.ApCliTab[apcliIfIndex].RepeaterCli[CliIdx].AuthCurrState;
						}
						else
#endif /* MAC_REPEATER_SUPPORT */
							AuthCurrState = pAd->ApCfg.ApCliTab[apcliIfIndex].AuthCurrState;

						StateMachinePerformAction(pAd, &pAd->Mlme.ApCliAuthMachine,
								Elem, AuthCurrState);
					}
					break;

				case APCLI_ASSOC_STATE_MACHINE:
					apcliIfIndex = (SHORT)Elem->Priv;
#ifdef MAC_REPEATER_SUPPORT
					if (apcliIfIndex >= 64)
						apcliIfIndex = ((apcliIfIndex - 64) / 16);		
#endif /* MAC_REPEATER_SUPPORT */

					if(isValidApCliIf(apcliIfIndex))		
					{
						ULONG AssocCurrState = pAd->ApCfg.ApCliTab[apcliIfIndex].AssocCurrState;
#ifdef MAC_REPEATER_SUPPORT
						UCHAR CliIdx = 0;

						apcliIfIndex = (SHORT)Elem->Priv;

						if (apcliIfIndex >= 64)
						{
							CliIdx = ((apcliIfIndex - 64) % 16);
							apcliIfIndex = ((apcliIfIndex - 64) / 16);
							AssocCurrState = pAd->ApCfg.ApCliTab[apcliIfIndex].RepeaterCli[CliIdx].AssocCurrState;
						}
#endif /* MAC_REPEATER_SUPPORT */
						StateMachinePerformAction(pAd, &pAd->Mlme.ApCliAssocMachine,
								Elem, AssocCurrState);
					}
					break;

				case APCLI_SYNC_STATE_MACHINE:
					apcliIfIndex = (SHORT)Elem->Priv;
					if(isValidApCliIf(apcliIfIndex))
						StateMachinePerformAction(pAd, &pAd->Mlme.ApCliSyncMachine, Elem,
							(pAd->ApCfg.ApCliTab[apcliIfIndex].SyncCurrState));
					break;

				case APCLI_CTRL_STATE_MACHINE:
					apcliIfIndex = (SHORT)Elem->Priv;
#ifdef MAC_REPEATER_SUPPORT
					if (apcliIfIndex >= 64)
						apcliIfIndex = ((apcliIfIndex - 64) / 16);		
#endif /* MAC_REPEATER_SUPPORT */

					if(isValidApCliIf(apcliIfIndex))
					{
						ULONG CtrlCurrState = pAd->ApCfg.ApCliTab[apcliIfIndex].CtrlCurrState;
#ifdef MAC_REPEATER_SUPPORT
						UCHAR CliIdx = 0;

						apcliIfIndex = (SHORT)Elem->Priv;

						if (apcliIfIndex >= 64)
						{
							CliIdx = ((apcliIfIndex - 64) % 16);
							apcliIfIndex = ((apcliIfIndex - 64) / 16);
							CtrlCurrState = pAd->ApCfg.ApCliTab[apcliIfIndex].RepeaterCli[CliIdx].CtrlCurrState;
						}
#endif /* MAC_REPEATER_SUPPORT */
						StateMachinePerformAction(pAd, &pAd->Mlme.ApCliCtrlMachine, Elem, CtrlCurrState);
					}
					break;
#endif /* APCLI_SUPPORT */

#endif /* CONFIG_AP_SUPPORT */
				case WPA_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.WpaMachine, Elem, pAd->Mlme.WpaMachine.CurrState);
					break;
#ifdef WSC_INCLUDED
                case WSC_STATE_MACHINE:
#if 1
					if (pAd->pWscElme)
					{
						RTMP_SEM_LOCK(&pAd->WscElmeLock);
						NdisMoveMemory(pAd->pWscElme, Elem, sizeof(MLME_QUEUE_ELEM));
						RTMP_SEM_UNLOCK(&pAd->WscElmeLock);
						RtmpOsTaskWakeUp(&(pAd->wscTask));
					}
#else
					WpsSmProcess(pAd, Elem);
#endif
                    break;
#ifdef IWSC_SUPPORT
				case IWSC_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.IWscMachine, Elem, pAd->Mlme.IWscMachine.CurrState);
                    break;
#endif /* IWSC_SUPPORT */
#endif /* WSC_INCLUDED */

#ifdef MESH_SUPPORT
				case MESH_CTRL_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.MeshCtrlMachine, Elem,
							pAd->MeshTab.CtrlCurrentState);
					break;

				case MESH_LINK_MNG_STATE_MACHINE:
					if (VALID_MESH_LINK_ID(Elem->Priv))
						StateMachinePerformAction(pAd, &pAd->Mlme.MeshLinkMngMachine, Elem,
								pAd->MeshTab.MeshLink[Elem->Priv].CurrentState);
					break;
#endif /* MESH_SUPPORT */

#ifdef P2P_SUPPORT
				case P2P_CTRL_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->P2pCfg.P2PCtrlMachine, Elem,
							pAd->P2pCfg.CtrlCurrentState);
					break;
				case P2P_DISC_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->P2pCfg.P2PDiscMachine, Elem,
							pAd->P2pCfg.DiscCurrentState);
					break;
				case P2P_GO_FORM_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->P2pCfg.P2PGoFormMachine, Elem,
							pAd->P2pCfg.GoFormCurrentState);
					break;
				case P2P_ACTION_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->P2pCfg.P2PActionMachine, Elem,
							pAd->P2pCfg.ActionState);
					break;
#endif /* P2P_SUPPORT */

#ifdef CONFIG_HOTSPOT
				case HSCTRL_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.HSCtrlMachine, Elem,
									HSCtrlCurrentState(pAd, Elem));
					break;
#endif

#ifdef CONFIG_DOT11U_INTERWORKING
				case GAS_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.GASMachine, Elem,
										GASPeerCurrentState(pAd, Elem));
					break;
#endif

#ifdef CONFIG_DOT11V_WNM
				case BTM_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.BTMMachine, Elem,
										BTMPeerCurrentState(pAd, Elem));
					break;
#ifdef CONFIG_HOTSPOT_R2
				case WNM_NOTIFY_STATE_MACHINE:
					StateMachinePerformAction(pAd, &pAd->Mlme.WNMNotifyMachine, Elem,
										WNMNotifyPeerCurrentState(pAd, Elem));
					break;	
#endif
#endif
				default:
					DBGPRINT(RT_DEBUG_TRACE, ("%s(): Illegal SM %ld\n",
								__FUNCTION__, Elem->Machine));
					break;
			} /* end of switch*/

			/* free MLME element*/
			Elem->Occupied = FALSE;
			Elem->MsgLen = 0;

		}
		else {
			DBGPRINT_ERR(("%s(): MlmeQ empty\n", __FUNCTION__));
		}
	}

	NdisAcquireSpinLock(&pAd->Mlme.TaskLock);
	pAd->Mlme.bRunning = FALSE;
	NdisReleaseSpinLock(&pAd->Mlme.TaskLock);
}


/*
========================================================================
Routine Description:
    MLME kernel thread.

Arguments:
	Context			the pAd, driver control block pointer

Return Value:
    0					close the thread

Note:
========================================================================
*/
static INT MlmeThread(ULONG Context)
{
	RTMP_ADAPTER *pAd;
	RTMP_OS_TASK *pTask;
	int status;
	status = 0;

	pTask = (RTMP_OS_TASK *)Context;
	pAd = (PRTMP_ADAPTER)RTMP_OS_TASK_DATA_GET(pTask);
	if (pAd == NULL)
		goto LabelExit;

	RtmpOSTaskCustomize(pTask);

	while (!RTMP_OS_TASK_IS_KILLED(pTask))
	{
		if (RtmpOSTaskWait(pAd, pTask, &status) == FALSE)
		{
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			break;
		}
			
		/* lock the device pointers , need to check if required*/
		/*down(&(pAd->usbdev_semaphore)); */
		
		if (!pAd->PM_FlgSuspend)
			MlmeHandler(pAd);
	}

	/* notify the exit routine that we're actually exiting now 
	 *
	 * complete()/wait_for_completion() is similar to up()/down(),
	 * except that complete() is safe in the case where the structure
	 * is getting deleted in a parallel mode of execution (i.e. just
	 * after the down() -- that's necessary for the thread-shutdown
	 * case.
	 *
	 * complete_and_exit() goes even further than this -- it is safe in
	 * the case that the thread of the caller is going away (not just
	 * the structure) -- this is necessary for the module-remove case.
	 * This is important in preemption kernels, which transfer the flow
	 * of execution immediately upon a complete().
	 */
LabelExit:
	DBGPRINT(RT_DEBUG_TRACE,( "<---%s\n",__FUNCTION__));
	RtmpOSTaskNotifyToExit(pTask);
	return 0;

}

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
static VOID ApCliMlmeInit(RTMP_ADAPTER *pAd)
{
	/* init apcli state machines*/
        ASSERT(APCLI_AUTH_FUNC_SIZE == APCLI_MAX_AUTH_MSG * APCLI_MAX_AUTH_STATE);
        ApCliAuthStateMachineInit(pAd, &pAd->Mlme.ApCliAuthMachine, pAd->Mlme.ApCliAuthFunc);

        ASSERT(APCLI_ASSOC_FUNC_SIZE == APCLI_MAX_ASSOC_MSG * APCLI_MAX_ASSOC_STATE);
        ApCliAssocStateMachineInit(pAd, &pAd->Mlme.ApCliAssocMachine, pAd->Mlme.ApCliAssocFunc);

        ASSERT(APCLI_SYNC_FUNC_SIZE == APCLI_MAX_SYNC_MSG * APCLI_MAX_SYNC_STATE);
        ApCliSyncStateMachineInit(pAd, &pAd->Mlme.ApCliSyncMachine, pAd->Mlme.ApCliSyncFunc);

        ASSERT(APCLI_CTRL_FUNC_SIZE == APCLI_MAX_CTRL_MSG * APCLI_MAX_CTRL_STATE);
        ApCliCtrlStateMachineInit(pAd, &pAd->Mlme.ApCliCtrlMachine, pAd->Mlme.ApCliCtrlFunc);
}
#endif /* APCLI_SUPPORT */

static VOID ApMlmeInit(RTMP_ADAPTER *pAd)
{
	/* init AP state machines*/
        APAssocStateMachineInit(pAd, &pAd->Mlme.ApAssocMachine, pAd->Mlme.ApAssocFunc);
        APAuthStateMachineInit(pAd, &pAd->Mlme.ApAuthMachine, pAd->Mlme.ApAuthFunc);
        APSyncStateMachineInit(pAd, &pAd->Mlme.ApSyncMachine, pAd->Mlme.ApSyncFunc);
}
#endif /* CONFIG_AP_SUPPORT */

/*
	==========================================================================
	Description:
		initialize the MLME task and its data structure (queue, spinlock, 
		timer, state machines).

	IRQL = PASSIVE_LEVEL

	Return:
		always return NDIS_STATUS_SUCCESS

	==========================================================================
*/
NDIS_STATUS MlmeInit(RTMP_ADAPTER *pAd)
{
	NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

	DBGPRINT(RT_DEBUG_TRACE, ("--> MLME Initialize\n"));

	do 
	{
		Status = MlmeQueueInit(pAd, &pAd->Mlme.Queue);
		if(Status != NDIS_STATUS_SUCCESS) 
			break;

		pAd->Mlme.bRunning = FALSE;
		NdisAllocateSpinLock(pAd, &pAd->Mlme.TaskLock);

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			BssTableInit(&pAd->ScanTab);

			/* init STA state machines*/
			AssocStateMachineInit(pAd, &pAd->Mlme.AssocMachine, pAd->Mlme.AssocFunc);
			AuthStateMachineInit(pAd, &pAd->Mlme.AuthMachine, pAd->Mlme.AuthFunc);
			AuthRspStateMachineInit(pAd, &pAd->Mlme.AuthRspMachine, pAd->Mlme.AuthRspFunc);
			SyncStateMachineInit(pAd, &pAd->Mlme.SyncMachine, pAd->Mlme.SyncFunc);

#ifdef QOS_DLS_SUPPORT
			DlsStateMachineInit(pAd, &pAd->Mlme.DlsMachine, pAd->Mlme.DlsFunc);
#endif /* QOS_DLS_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
			TDLS_StateMachineInit(pAd, &pAd->Mlme.TdlsMachine, pAd->Mlme.TdlsFunc);
#endif /* DOT11Z_TDLS_SUPPORT */

#ifdef WSC_STA_SUPPORT
#ifdef IWSC_SUPPORT
			IWSC_StateMachineInit(pAd, &pAd->Mlme.IWscMachine, pAd->Mlme.IWscFunc);
#endif /* IWSC_SUPPORT */
#endif /* WSC_STA_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
			FT_OTA_AuthStateMachineInit(pAd, &pAd->Mlme.FtOtaAuthMachine, pAd->Mlme.FtOtaAuthFunc);
			FT_OTD_StateMachineInit(pAd, &pAd->Mlme.FtOtdActMachine, pAd->Mlme.FtOtdActFunc);
#endif /* DOT11R_FT_SUPPORT */

#ifdef STA_EASY_CONFIG_SETUP
			AutoProvisionStateMachineInit(pAd, &pAd->Mlme.AutoProvisionMachine, pAd->Mlme.AutoProvisionFunc);
#endif /* STA_EASY_CONFIG_SETUP */

			/* Since we are using switch/case to implement it, the init is different from the above */
			/* state machine init*/
			MlmeCntlInit(pAd, &pAd->Mlme.CntlMachine, NULL);

#ifdef PCIE_PS_SUPPORT
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
			{
			    /* only PCIe cards need these two timers*/
				RTMPInitTimer(pAd, &pAd->Mlme.PsPollTimer, GET_TIMER_FUNCTION(PsPollWakeExec), pAd, FALSE);
				RTMPInitTimer(pAd, &pAd->Mlme.RadioOnOffTimer, GET_TIMER_FUNCTION(RadioOnExec), pAd, FALSE);
			}
#endif /* PCIE_PS_SUPPORT */

			RTMPInitTimer(pAd, &pAd->Mlme.LinkDownTimer, GET_TIMER_FUNCTION(LinkDownExec), pAd, FALSE);
			RTMPInitTimer(pAd, &pAd->StaCfg.StaQuickResponeForRateUpTimer, GET_TIMER_FUNCTION(StaQuickResponeForRateUpExec), pAd, FALSE);
			pAd->StaCfg.StaQuickResponeForRateUpTimerRunning = FALSE;
			RTMPInitTimer(pAd, &pAd->StaCfg.WpaDisassocAndBlockAssocTimer, GET_TIMER_FUNCTION(WpaDisassocApAndBlockAssoc), pAd, FALSE);

#ifdef RTMP_MAC_USB
			RTMPInitTimer(pAd, &pAd->Mlme.AutoWakeupTimer, GET_TIMER_FUNCTION(RtmpUsbStaAsicForceWakeupTimeout), pAd, FALSE);
			pAd->Mlme.AutoWakeupTimerRunning = FALSE;
#endif /* RTMP_MAC_USB */

#ifdef STA_EASY_CONFIG_SETUP
			RTMPInitTimer(pAd, 
						  &pAd->StaCfg.EasyConfigInfo.ScanTimer, 
						  GET_TIMER_FUNCTION(AutoProvisionScanTimeOutAction), 
						  pAd, 
						  FALSE);
			
			RTMPInitTimer(pAd, 
						  &pAd->StaCfg.EasyConfigInfo.InfraConnTimer, 
						  GET_TIMER_FUNCTION(InfraConnectionTimeout), 
						  pAd, 
						  FALSE);
			
			RTMPInitTimer(pAd, 
						  &pAd->StaCfg.EasyConfigInfo.AdhocConnTimer, 
						  GET_TIMER_FUNCTION(AdhocConnectionTimeout), 
						  pAd, 
						  FALSE);
			
#endif /* STA_EASY_CONFIG_SETUP */

		}
#endif /* CONFIG_STA_SUPPORT */
		
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			ApMlmeInit(pAd);
#ifdef APCLI_SUPPORT
			ApCliMlmeInit(pAd);
#endif /* APCLI_SUPPORT */
		}

#endif /* CONFIG_AP_SUPPORT */

#ifdef WSC_INCLUDED
		/* Init Wsc state machine */
		ASSERT(WSC_FUNC_SIZE == MAX_WSC_MSG * MAX_WSC_STATE);
		WscStateMachineInit(pAd, &pAd->Mlme.WscMachine, pAd->Mlme.WscFunc);
#endif /* WSC_INCLUDED */

		WpaStateMachineInit(pAd, &pAd->Mlme.WpaMachine, pAd->Mlme.WpaFunc);

#ifdef CONFIG_HOTSPOT
		HSCtrlStateMachineInit(pAd, &pAd->Mlme.HSCtrlMachine, pAd->Mlme.HSCtrlFunc);
		GASStateMachineInit(pAd, &pAd->Mlme.GASMachine, pAd->Mlme.GASFunc);
#endif

#ifdef CONFIG_DOT11V_WNM
		WNMCtrlInit(pAd);
		BTMStateMachineInit(pAd, &pAd->Mlme.BTMMachine, pAd->Mlme.BTMFunc);
#ifdef CONFIG_HOTSPOT_R2
		WNMNotifyStateMachineInit(pAd, &pAd->Mlme.WNMNotifyMachine, pAd->Mlme.WNMNotifyFunc);
#endif
#endif

#ifdef MESH_SUPPORT
		ASSERT(MESH_CTRL_FUNC_SIZE == MESH_CTRL_MAX_EVENTS * MESH_CTRL_MAX_STATES);
		MeshCtrlStateMachineInit(pAd, &pAd->Mlme.MeshCtrlMachine, pAd->Mlme.MeshCtrlFunc);

		ASSERT(MESH_LINK_MNG_FUNC_SIZE == MESH_LINK_MNG_MAX_EVENTS * MESH_LINK_MNG_MAX_STATES);
		MeshLinkMngStateMachineInit(pAd, &pAd->Mlme.MeshLinkMngMachine, pAd->Mlme.MeshLinkMngFunc);
#endif /* MESH_SUPPORT */

		ActionStateMachineInit(pAd, &pAd->Mlme.ActMachine, pAd->Mlme.ActFunc);

		/* Init mlme periodic timer*/
		RTMPInitTimer(pAd, &pAd->Mlme.PeriodicTimer, GET_TIMER_FUNCTION(MlmePeriodicExecTimer), pAd, TRUE);

		/* Set mlme periodic timer*/
		RTMPSetTimer(&pAd->Mlme.PeriodicTimer, MLME_TASK_EXEC_INTV);

		/* software-based RX Antenna diversity*/
		RTMPInitTimer(pAd, &pAd->Mlme.RxAntEvalTimer, GET_TIMER_FUNCTION(AsicRxAntEvalTimeout), pAd, FALSE);

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			/* Init APSD periodic timer*/
			RTMPInitTimer(pAd, &pAd->Mlme.APSDPeriodicTimer, GET_TIMER_FUNCTION(APSDPeriodicExec), pAd, TRUE);
			RTMPSetTimer(&pAd->Mlme.APSDPeriodicTimer, 50);

			/* Init APQuickResponseForRateUp timer.*/
			RTMPInitTimer(pAd, &pAd->ApCfg.ApQuickResponeForRateUpTimer, GET_TIMER_FUNCTION(APQuickResponeForRateUpExec), pAd, FALSE);
			pAd->ApCfg.ApQuickResponeForRateUpTimerRunning = FALSE;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef P2P_SUPPORT
			/* P2P Ctrl State Machine */
			ASSERT(P2P_CTRL_FUNC_SIZE == P2P_CTRL_MAX_EVENTS * P2P_CTRL_MAX_STATES);
			P2PCtrlStateMachineInit(pAd, &pAd->P2pCfg.P2PCtrlMachine, pAd->P2pCfg.P2PCtrlFunc);
	
			/* P2P Discovery State Machine */
			ASSERT(P2P_DISC_FUNC_SIZE == P2P_DISC_MAX_EVENTS * P2P_DISC_MAX_STATES);
			P2PDiscoveryStateMachineInit(pAd, &pAd->P2pCfg.P2PDiscMachine, pAd->P2pCfg.P2PDiscFunc);
	
			/* P2P Group Formation State Machine */
			ASSERT(P2P_GO_FORM_FUNC_SIZE == P2P_GO_NEGO_MAX_EVENTS * P2P_GO_FORM_MAX_STATES);
			P2PGoFormationStateMachineInit(pAd, &pAd->P2pCfg.P2PGoFormMachine, pAd->P2pCfg.P2PGoFormFunc);
	
			/* P2P Action Frame State Machine */
			ASSERT(P2P_ACTION_FUNC_SIZE == MAX_P2P_MSG * MAX_P2P_STATE);
			P2PStateMachineInit(pAd, &pAd->P2pCfg.P2PActionMachine, pAd->P2pCfg.P2PActionFunc);
	
			/* P2P CTWindows timer */
			RTMPInitTimer(pAd, &pAd->P2pCfg.P2pCTWindowTimer, GET_TIMER_FUNCTION(P2PCTWindowTimer), pAd, FALSE);
			/* P2P SwNOA timer */
			RTMPInitTimer(pAd, &pAd->P2pCfg.P2pSwNoATimer, GET_TIMER_FUNCTION(P2pSwNoATimeOut), pAd, FALSE);
			/* P2P Presence Absent timer */
			RTMPInitTimer(pAd, &pAd->P2pCfg.P2pPreAbsenTimer, GET_TIMER_FUNCTION(P2pPreAbsenTimeOut), pAd, FALSE);
			/* P2P WSC Timer */
			RTMPInitTimer(pAd, &pAd->P2pCfg.P2pWscTimer, GET_TIMER_FUNCTION(P2pWscTimeOut), pAd, FALSE);
			/* P2P Re-Transmit Action Frame Timer */
			RTMPInitTimer(pAd, &pAd->P2pCfg.P2pReSendTimer, GET_TIMER_FUNCTION(P2pReSendTimeOut), pAd, FALSE);
			/* P2P CLIENT Re-Connect Timer */
			RTMPInitTimer(pAd, &pAd->P2pCfg.P2pCliReConnectTimer, GET_TIMER_FUNCTION(P2pCliReConnectTimeOut), pAd, FALSE);

#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			{
				ApMlmeInit(pAd);
#ifdef APCLI_SUPPORT
				ApCliMlmeInit(pAd);	
#endif /* APCLI_SUPPORT */
	
				/* Init APSD periodic timer */
				RTMPInitTimer(pAd, &pAd->Mlme.APSDPeriodicTimer, GET_TIMER_FUNCTION(APSDPeriodicExec), pAd, TRUE);
				RTMPSetTimer(&pAd->Mlme.APSDPeriodicTimer, 50);
			}
#endif /* CONFIG_STA_SUPPORT */
#endif /* P2P_SUPPORT */

#if defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
	/*CFG_TODO*/
	ApMlmeInit(pAd);

#if defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) || defined(CFG80211_MULTI_STA)
	ApCliMlmeInit(pAd);
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE || CFG80211_MULTI_STA */
#endif /* RT_CFG80211_P2P_SUPPORT || CFG80211_MULTI_STA */

	} while (FALSE);

	{
		RTMP_OS_TASK *pTask;

		/* Creat MLME Thread */
		pTask = &pAd->mlmeTask;
		RTMP_OS_TASK_INIT(pTask, "RtmpMlmeTask", pAd);
		Status = RtmpOSTaskAttach(pTask, MlmeThread, (ULONG)pTask);
		if (Status == NDIS_STATUS_FAILURE) {
			DBGPRINT (RT_DEBUG_ERROR,  ("%s: unable to start MlmeThread\n", RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev)));
		}
	}
	DBGPRINT(RT_DEBUG_TRACE, ("<-- MLME Initialize\n"));

	return Status;
}


/*
	==========================================================================
	Description:
		Destructor of MLME (Destroy queue, state machine, spin lock and timer)
	Parameters:
		Adapter - NIC Adapter pointer
	Post:
		The MLME task will no longer work properly

	IRQL = PASSIVE_LEVEL

	==========================================================================
 */
VOID MlmeHalt(RTMP_ADAPTER *pAd) 
{
	BOOLEAN Cancelled;
	RTMP_OS_TASK *pTask;
		
	DBGPRINT(RT_DEBUG_TRACE, ("==> MlmeHalt\n"));

	/* Terminate Mlme Thread */
	pTask = &pAd->mlmeTask;
	if (RtmpOSTaskKill(pTask) == NDIS_STATUS_FAILURE) {
		DBGPRINT(RT_DEBUG_ERROR, ("kill mlme task failed!\n"));
	}
	
#if (defined(MT_WOW_SUPPORT) && defined(WOW_IFDOWN_SUPPORT))
	if (!((pAd->WOW_Cfg.bEnable == TRUE) && INFRA_ON(pAd)))
#endif
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		/* disable BEACON generation and other BEACON related hardware timers*/
		AsicDisableSync(pAd);
	}
#ifdef MT76XX_BTCOEX_SUPPORT
		if (IS_MT76XXBTCOMBO(pAd))
		{
			BtAFHCtl(pAd, pAd->CommonCfg.BBPCurrentBW, pAd->CommonCfg.Channel, pAd->CommonCfg.CentralChannel, TRUE);	
              		SendAndesAFH(pAd, pAd->CommonCfg.BBPCurrentBW,  pAd->CommonCfg.Channel, pAd->CommonCfg.CentralChannel, TRUE, 0);
		}
#endif /*MT76XX_BTCOEX_SUPPORT*/
	RTMPCancelTimer(&pAd->Mlme.PeriodicTimer, &Cancelled);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef QOS_DLS_SUPPORT
		UCHAR		i;
#endif /* QOS_DLS_SUPPORT */
		/* Cancel pending timers*/
		RTMPCancelTimer(&pAd->MlmeAux.AssocTimer, &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.ReassocTimer, &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.DisassocTimer, &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.AuthTimer, &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.BeaconTimer, &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.ScanTimer, &Cancelled);

#ifdef DOT11R_FT_SUPPORT
		RTMPCancelTimer(&pAd->MlmeAux.FtOtaAuthTimer, &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.FtOtdActTimer, &Cancelled);
#endif /* DOT11R_FT_SUPPORT */

#ifdef PCIE_PS_SUPPORT
	    if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE)
			&&(pAd->StaCfg.PSControl.field.EnableNewPS == TRUE))
	    {
	   	    RTMPCancelTimer(&pAd->Mlme.PsPollTimer, &Cancelled);
		    RTMPCancelTimer(&pAd->Mlme.RadioOnOffTimer, &Cancelled);
		}
#endif /* PCIE_PS_SUPPORT */

#ifdef QOS_DLS_SUPPORT
		for (i=0; i<MAX_NUM_OF_DLS_ENTRY; i++)
		{
			RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &Cancelled);
		}
#endif /* QOS_DLS_SUPPORT */
		RTMPCancelTimer(&pAd->Mlme.LinkDownTimer, &Cancelled);

#ifdef RTMP_MAC_USB
		if (pAd->Mlme.AutoWakeupTimerRunning)
		{
			RTMPCancelTimer(&pAd->Mlme.AutoWakeupTimer, &Cancelled);
			pAd->Mlme.AutoWakeupTimerRunning = FALSE;
		}
#endif /* RTMP_MAC_USB */

#ifdef WSC_STA_SUPPORT
		if (pAd->StaCfg.WscControl.WscProfileRetryTimerRunning)
		{
			pAd->StaCfg.WscControl.WscProfileRetryTimerRunning = FALSE;
			RTMPCancelTimer(&pAd->StaCfg.WscControl.WscProfileRetryTimer, &Cancelled);
		}
#endif /* WSC_STA_SUPPORT */

#ifdef STA_EASY_CONFIG_SETUP
		RTMPCancelTimer(&pAd->StaCfg.EasyConfigInfo.InfraConnTimer, &Cancelled);
		RTMPCancelTimer(&pAd->StaCfg.EasyConfigInfo.AdhocConnTimer, &Cancelled);
		RTMPCancelTimer(&pAd->StaCfg.EasyConfigInfo.ScanTimer, &Cancelled);
#endif /* STA_EASY_CONFIG_SETUP */

		if (pAd->StaCfg.StaQuickResponeForRateUpTimerRunning)
		{
			RTMPCancelTimer(&pAd->StaCfg.StaQuickResponeForRateUpTimer, &Cancelled);
			pAd->StaCfg.StaQuickResponeForRateUpTimerRunning = FALSE;
		}
		RTMPCancelTimer(&pAd->StaCfg.WpaDisassocAndBlockAssocTimer, &Cancelled);
#ifdef IWSC_SUPPORT
		RTMPCancelTimer(&pAd->StaCfg.IWscInfo.IWscT1Timer, &Cancelled);
		RTMPCancelTimer(&pAd->StaCfg.IWscInfo.IWscT2Timer, &Cancelled);
#endif /* IWSC_SUPPORT */
	}
#endif /* CONFIG_STA_SUPPORT */

	RTMPCancelTimer(&pAd->Mlme.RxAntEvalTimer, &Cancelled);

#ifdef BT_COEXISTENCE_SUPPORT
	if ((pAd->bHWCoexistenceInit == TRUE) && IS_ENABLE_MISC_TIMER(pAd))
	{
		RTMPCancelTimer(&pAd->Mlme.MiscDetectTimer,&Cancelled);
	}
#endif /* BT_COEXISTENCE_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef APCLI_SUPPORT
		UCHAR idx = 0;
#endif /* APCLI_SUPPORT */
		RTMPCancelTimer(&pAd->Mlme.APSDPeriodicTimer, &Cancelled);

		if (pAd->ApCfg.ApQuickResponeForRateUpTimerRunning == TRUE)
			RTMPCancelTimer(&pAd->ApCfg.ApQuickResponeForRateUpTimer, &Cancelled);

#ifdef APCLI_SUPPORT
		for (idx = 0; idx < MAX_APCLI_NUM; idx++)
		{
			PAPCLI_STRUCT pApCliEntry = &pAd->ApCfg.ApCliTab[idx];
			RTMPCancelTimer(&pApCliEntry->MlmeAux.ProbeTimer, &Cancelled);
			RTMPCancelTimer(&pApCliEntry->MlmeAux.ApCliAssocTimer, &Cancelled);
			RTMPCancelTimer(&pApCliEntry->MlmeAux.ApCliAuthTimer, &Cancelled);

#ifdef WSC_AP_SUPPORT
			if (pApCliEntry->WscControl.WscProfileRetryTimerRunning)
		{
				pApCliEntry->WscControl.WscProfileRetryTimerRunning = FALSE;
				RTMPCancelTimer(&pApCliEntry->WscControl.WscProfileRetryTimer, &Cancelled);
		}
#endif /* WSC_AP_SUPPORT */
		}
#endif /* APCLI_SUPPORT */
		RTMPCancelTimer(&pAd->ScanCtrl.APScanTimer, &Cancelled);
	}

#endif /* CONFIG_AP_SUPPORT */

#ifdef MESH_SUPPORT
	MeshHalt(pAd);
#endif /* MESH_SUPPORT */

#ifdef CONFIG_HOTSPOT
	HSCtrlHalt(pAd);
	HSCtrlExit(pAd);
#endif

#ifdef CONFIG_DOT11U_INTERWORKING
	GASCtrlExit(pAd);
#endif

#ifdef CONFIG_DOT11V_WNM
	WNMCtrlExit(pAd);
#endif

#ifdef P2P_SUPPORT
		/* P2P CTWindows timer */
		RTMPCancelTimer(&pAd->P2pCfg.P2pCTWindowTimer, &Cancelled);
		/* P2P SwNOA timer */
		RTMPCancelTimer(&pAd->P2pCfg.P2pSwNoATimer, &Cancelled);
		/* P2P Presence Absent timer */
		RTMPCancelTimer(&pAd->P2pCfg.P2pPreAbsenTimer, &Cancelled);

		if (pAd->P2pCfg.bP2pCliReConnectTimerRunning)
		{
			pAd->P2pCfg.bP2pCliReConnectTimerRunning = FALSE;
			RTMPCancelTimer(&pAd->P2pCfg.P2pCliReConnectTimer, &Cancelled);
		}

		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			RTMPCancelTimer(&pAd->Mlme.APSDPeriodicTimer,	&Cancelled);
			RTMPCancelTimer(&pAd->ApCfg.ApCliTab[0].MlmeAux.ProbeTimer, &Cancelled);
			RTMPCancelTimer(&pAd->ApCfg.ApCliTab[0].MlmeAux.ApCliAssocTimer, &Cancelled);
			RTMPCancelTimer(&pAd->ApCfg.ApCliTab[0].MlmeAux.ApCliAuthTimer, &Cancelled);
	
#ifdef WSC_AP_SUPPORT
			if (pAd->ApCfg.ApCliTab[BSS0].WscControl.WscProfileRetryTimerRunning)
			{
				pAd->ApCfg.ApCliTab[BSS0].WscControl.WscProfileRetryTimerRunning = FALSE;
				RTMPCancelTimer(&pAd->ApCfg.ApCliTab[BSS0].WscControl.WscProfileRetryTimer, &Cancelled);
			}
#endif /* WSC_AP_SUPPORT */
			RTMPCancelTimer(&pAd->ScanCtrl.APScanTimer,	&Cancelled);
		}
#endif /* P2P_SUPPORT */

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
		
#ifdef LED_CONTROL_SUPPORT		
		/* Set LED*/
		RTMPSetLED(pAd, LED_HALT);
		RTMPSetSignalLED(pAd, -100);	/* Force signal strength Led to be turned off, firmware is not done it.*/
#ifdef RTMP_MAC_USB
#ifdef RLT_MAC
	// TODO: shiang-7603
		if (pAd->chipCap.hif_type != HIF_MT)
		{
			LED_CFG_STRUC LedCfg;
			RTMP_IO_READ32(pAd, LED_CFG, &LedCfg.word);
			LedCfg.field.LedPolar = 0;
			LedCfg.field.RLedMode = 0;
			LedCfg.field.GLedMode = 0;
			LedCfg.field.YLedMode = 0;
			RTMP_IO_WRITE32(pAd, LED_CFG, LedCfg.word);
		}
#endif		
#endif /* RTMP_MAC_USB */
#endif /* LED_CONTROL_SUPPORT */

#if (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(MT_WOW_SUPPORT)
		if (!(pAd->WOW_Cfg.bEnable == TRUE) && INFRA_ON(pAd))
#endif /* (defined(WOW_SUPPORT) && defined(RTMP_MAC_USB)) || defined(MT_WOW_SUPPORT)*/
			if (pChipOps->AsicHaltAction)
				pChipOps->AsicHaltAction(pAd);
	}

	RtmpusecDelay(5000);    /*  5 msec to gurantee Ant Diversity timer canceled*/

	MlmeQueueDestroy(&pAd->Mlme.Queue);
	NdisFreeSpinLock(&pAd->Mlme.TaskLock);

	DBGPRINT(RT_DEBUG_TRACE, ("<== MlmeHalt\n"));
}


VOID MlmeResetRalinkCounters(RTMP_ADAPTER *pAd)
{
	pAd->RalinkCounters.LastOneSecRxOkDataCnt = pAd->RalinkCounters.OneSecRxOkDataCnt;

#ifdef CONFIG_ATE
	if (!ATE_ON(pAd))
#endif /* CONFIG_ATE */
		/* for performace enchanement */
		NdisZeroMemory(&pAd->RalinkCounters,
						((ULONG)&pAd->RalinkCounters.OneSecEnd -
						(ULONG)&pAd->RalinkCounters.OneSecStart));

	return;
}


/*
	==========================================================================
	Description:
		This routine is executed periodically to -
		1. Decide if it's a right time to turn on PwrMgmt bit of all 
		   outgoiing frames
		2. Calculate ChannelQuality based on statistics of the last
		   period, so that TX rate won't toggling very frequently between a 
		   successful TX and a failed TX.
		3. If the calculated ChannelQuality indicated current connection not 
		   healthy, then a ROAMing attempt is tried here.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */

VOID MlmePeriodicExecTimer(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3) 
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;
	RTEnqueueInternalCmd(pAd, CMDTHREAD_MLME_PERIOIDC_EXEC, NULL, 0);
}

NTSTATUS MlmePeriodicExec(IN PRTMP_ADAPTER pAd, IN PCmdQElmt CMDQelmt)
{
	ULONG TxTotalCnt;
    ULONG TxTotalByteCnt = 0, RxTotalByteCnt = 0;
	//RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;
#ifdef ANT_DIVERSITY_SUPPORT
	SHORT realavgrssi;
#endif /* ANT_DIVERSITY_SUPPORT */

	/* No More 0x84 MCU CMD from v.30 FW*/
#if 0
#ifdef RTMP_MAC_PCI
#ifdef CONFIG_STA_SUPPORT
#ifdef RT30xx
	/* not for USB device!*/
	/* FWv.18 and after needs the timer tick command every 200 ms*/
	/* LedMode = 0 : Slow Blink, Fast blink, light all normal.  So need to set 0x84 software timer command.*/
	if ((IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3593(pAd)) &&
		(pAd->Mlme.PeriodicRound % 2 == 0) &&
		(pAd->StaCfg.PSControl.field.LedMode == 0) &&
		(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) &&
		(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)))
	{
		AsicSendCommandToMcu(pAd, 0x84, 0xff, 0xff, 0xff, FALSE); 
	}
#endif /* RT30xx */
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_PCI */
#endif /* 0 */

//CFG
#if 0
	if(!pAd->cfg80211_ctrl.isMccOn)
	{
		BOOLEAN Mcc_side1 = FALSE, Mcc_side2 = FALSE;
		if(INFRA_ON(pAd) && (pAd->StaCfg.wdev.PortSecured == WPA_802_1X_PORT_SECURED))
		{
			//printk("INFRA SIDE: ==================================> on \n");
			Mcc_side1 = TRUE;
		}

		if((pAd->ApCfg.ApCliTab[0].CtrlCurrState == APCLI_CTRL_CONNECTED) && 
		   (pAd->ApCfg.ApCliTab[0].wdev.PortSecured == WPA_802_1X_PORT_SECURED))
		{
			//printk("P2P_GC SIDE: ==================================> on \n");
			Mcc_side2 = TRUE;
		}
		if (Mcc_side1 && Mcc_side2 && (!pAd->cfg80211_ctrl.isMccOn))
		{
			printk("MCC: ==================================> on \n");
			CMD_MCC_START_PARM mccStartParm;
			mccStartParm.channel_1st=pAd->CommonCfg.Channel;
			mccStartParm.channel_2nd=pAd->ApCfg.ApCliTab[0].MlmeAux.Channel;
			pAd->cfg80211_ctrl.isMccOn = TRUE;
			CmdMccStart(pAd, &mccStartParm);
		}
	}
	
#endif

#ifdef MICROWAVE_OVEN_SUPPORT
	/* update False CCA count to an array */
	NICUpdateRxStatusCnt1(pAd, pAd->Mlme.PeriodicRound%10);

	if (pAd->CommonCfg.MO_Cfg.bEnable)
	{
		UINT8 stage = pAd->Mlme.PeriodicRound%10;

		if (stage == MO_MEAS_PERIOD)
		{
			ASIC_MEASURE_FALSE_CCA(pAd);
			pAd->CommonCfg.MO_Cfg.nPeriod_Cnt = 0;
		}
		else if (stage == MO_IDLE_PERIOD)
		{
			UINT16 Idx;

			for (Idx = MO_MEAS_PERIOD + 1; Idx < MO_IDLE_PERIOD + 1; Idx++)
				pAd->CommonCfg.MO_Cfg.nFalseCCACnt += pAd->RalinkCounters.FalseCCACnt_100MS[Idx];

			//printk("%s: fales cca1 %d\n", __FUNCTION__, pAd->CommonCfg.MO_Cfg.nFalseCCACnt);
			if (pAd->CommonCfg.MO_Cfg.nFalseCCACnt > pAd->CommonCfg.MO_Cfg.nFalseCCATh)
				ASIC_MITIGATE_MICROWAVE(pAd);

		}
	}
#endif /* MICROWAVE_OVEN_SUPPORT */

#ifdef INF_AMAZON_SE
#ifdef RTMP_MAC_USB
	SoftwareFlowControl(pAd);
#endif /* RTMP_MAC_USB */
#endif /* INF_AMAZON_SE */

#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_PCI
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
	    /* If Hardware controlled Radio enabled, we have to check GPIO pin2 every 2 second.*/
		/* Move code to here, because following code will return when radio is off*/
		if ((pAd->Mlme.PeriodicRound % (MLME_TASK_EXEC_MULTIPLE * 2) == 0) &&
/*			(pAd->StaCfg.bHardwareRadio == TRUE) &&*/
			((IDLE_ON(pAd)) || (pAd->StaCfg.PwrMgmt.Psm == PWR_ACTIVE)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) &&
			((pAd->StaCfg.bHardwareRadio == TRUE))
/*			|| (pAd->StaCfg.WscControl.CheckHWPBCState == HWPBCState_GUI)))*/
			/*&&(pAd->bPCIclkOff == FALSE)*/)
		{
			UINT32 data = 0;

#if defined(RT30xx) || defined(RT5390) || defined(RT5392)
			/* If in power save state, force wake up to configure register, AsicRadioOff will be called later n STAMlmePeriodicExec*/
			if ((OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE)
				|| (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND)) 
				|| RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
				&& (IS_RT3090(pAd) || IS_RT3572(pAd) || IS_RT3593(pAd) || IS_RT5390(pAd) || IS_RT5392(pAd))
				&& (pAd->StaCfg.bRadio == TRUE))
			{
				RT28xxPciAsicRadioOn(pAd, GUI_IDLE_POWER_SAVE);
			}
#endif /* defined(RT30xx) || defined(RT5390) || defined(RT5392) */

			/* Read GPIO pin2 as Hardware controlled radio state*/
/*#ifndef RT3090*/
			/*RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);*/
/*#endif  RT3090 */

			// TODO: shiang-7603
			if (pAd->chipCap.hif_type == HIF_MT) {
				DBGPRINT(RT_DEBUG_OFF, ("%s(): Not support for HIF_MT yet!\n",
						__FUNCTION__));
			}
			else
#ifdef RT3290
			// TODO: shiang, what's the purpose of these code segments??
			if (IS_RT3290(pAd))
			{
				if ((pAd->StaCfg.bHardwareRadio == TRUE) ||
				    (pAd->NicConfig2.field.EnableWPSPBC && 
					  (pAd->StaCfg.bRadio == TRUE) && 
					  (pAd->LedCntl.MCULedCntl.field.LedMode != LED_MODE_SIGNAL_STREGTH)
				    )
				)
				{
					RTMP_IO_FORCE_READ32(pAd, WLAN_FUN_CTRL, &data);
					DBGPRINT(RT_DEBUG_TRACE,(" RT3290: Read GPIO as Hardware controlled radio state\n"));
				}
			}
			else
#endif /* RT3290 */
/*KH(PCIE PS):Added based on Jane<--*/
#ifdef PCIE_PS_SUPPORT
			/* Read GPIO pin2 as Hardware controlled radio state*/
			/* We need to Read GPIO if HW said so no mater what advance power saving*/
			if ((pAd->OpMode == OPMODE_STA) && (IDLE_ON(pAd))
				&& (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
				&& (pAd->StaCfg.PSControl.field.EnablePSinIdle == TRUE))
			{
				/* Want to make sure device goes to L0 state before reading register.*/
				RTMPPCIeLinkCtrlValueRestore(pAd, 0);
				RTMP_IO_FORCE_READ32(pAd, GPIO_CTRL_CFG, &data);
				RTMPPCIeLinkCtrlSetting(pAd, 3);
			}
			else
				RTMP_IO_FORCE_READ32(pAd, GPIO_CTRL_CFG, &data);
#else
				RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);
#endif /* defined(RT3090) || defined(RT3592) || defined(RT3390) */
/*KH(PCIE PS):Added based on Jane-->*/

			/* Update Radio state from GPIO*/
			if (pAd->StaCfg.bHardwareRadio == TRUE)
			{
#ifdef RT3290
				if (IS_RT3290(pAd))
					pAd->StaCfg.bHwRadio = (data & 0x100) ? TRUE : FALSE;
				else
#endif /* RT3290 */
				{
					if (data & 0x04)
						pAd->StaCfg.bHwRadio = TRUE;
					else
						pAd->StaCfg.bHwRadio = FALSE;
				}
#ifdef RT_CFG80211_SUPPORT
#ifdef RFKILL_HW_SUPPORT
				RT_CFG80211_RFKILL_STATUS_UPDATE(pAd, pAd->StaCfg.bHwRadio); 
#endif // RFKILL_HW_SUPPORT //
#endif /* RT_CFG80211_SUPPORT */
			}

			/* Always read HW radio configuration.*/
			if (pAd->StaCfg.bHardwareRadio == TRUE)
			{
				if (pAd->StaCfg.bRadio != (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio))
				{
					pAd->StaCfg.bRadio = (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio);
					if (pAd->StaCfg.bRadio == TRUE)
					{
						MlmeRadioOn(pAd);
						pAd->ExtraInfo = EXTRA_INFO_CLEAR;
					}
					else
					{			    
						MlmeRadioOff(pAd);
						pAd->ExtraInfo = HW_RADIO_OFF;
					}
				}
			}
		}
	}
#endif /* RTMP_MAC_PCI */

	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		RTMP_MLME_PRE_SANITY_CHECK(pAd);
	}

#ifdef STA_LP_PHASE_1_SUPPORT
	if (pAd->CountDowntoPsm > 0)
		pAd->CountDowntoPsm--;
#else
#ifdef RTMP_MAC_USB
	/*
		We go to sleep mode only when count down to zero.
		Count down counter is set after link up. So within 10 seconds after link up, we never go to sleep.
		10 seconds period, we can get IP, finish 802.1x authenticaion. and some critical , timing protocol.
	*/
	if (pAd->CountDowntoPsm > 0)
		pAd->CountDowntoPsm--;
#endif /* RTMP_MAC_USB */
#endif

#endif /* CONFIG_STA_SUPPORT */

	/* Do nothing if the driver is starting halt state.*/
	/* This might happen when timer already been fired before cancel timer with mlmehalt*/
	if ((RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_HALT_IN_PROGRESS |
								fRTMP_ADAPTER_RADIO_OFF |
								fRTMP_ADAPTER_RADIO_MEASUREMENT |
								fRTMP_ADAPTER_RESET_IN_PROGRESS |
								fRTMP_ADAPTER_NIC_NOT_EXIST))))
		return NDIS_STATUS_SUCCESS;

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
		return NDIS_STATUS_SUCCESS;

#ifdef CONFIG_FPGA_MODE
#ifdef CAPTURE_MODE
	cap_status_chk_and_get(pAd);
#endif /* CAPTURE_MODE */
#endif /* CONFIG_FPGA_MODE */


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Do nothing if monitor mode is on*/
		if (MONITOR_ON(pAd))
			return NDIS_STATUS_SUCCESS;

#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			if ((pAd->Mlme.PeriodicRound & 0x1) &&
				(STA_TGN_WIFI_ON(pAd)) &&
				((pAd->MACVersion & 0xffff) == 0x0101))
			{
				UINT32 txop_cfg;

				/* This is the fix for wifi 11n extension channel overlapping test case.  for 2860D*/
				if (pAd->CommonCfg.IOTestParm.bToggle == FALSE)
				{
					txop_cfg = 0x24BF;
					pAd->CommonCfg.IOTestParm.bToggle = TRUE;
				}
				else
				{
					txop_cfg = 0x243f;
					pAd->CommonCfg.IOTestParm.bToggle = FALSE;
				}
				RTMP_IO_WRITE32(pAd, TXOP_CTRL_CFG, txop_cfg);
			}
		}
#endif /* RTMP_MAC */
	}
#endif /* CONFIG_STA_SUPPORT */

	pAd->bUpdateBcnCntDone = FALSE;
	
/*	RECBATimerTimeout(SystemSpecific1,FunctionContext,SystemSpecific2,SystemSpecific3);*/
	pAd->Mlme.PeriodicRound ++;
	pAd->Mlme.GPIORound++;

#ifndef WFA_VHT_PF
#ifdef RT8592
	if (IS_RT8592(pAd))
		rt85592_cca_adjust(pAd);
#endif /* RT8592 */
#endif /* WFA_VHT_PF */

#if defined(MT_MAC) && defined(CONFIG_STA_SUPPORT)
	if((pAd->Mlme.PeriodicRound % 10) == 0)
	{
		periodic_monitor_ac_counter(pAd,QID_AC_VI);
	}
#endif


#ifdef MT_MAC
	if ((pAd->Mlme.PeriodicRound % 1) == 0)
	{
#ifdef RTMP_MAC_PCI
		if (pAd->PDMAWatchDogEn)
		{
			PDMAWatchDog(pAd);
		}

		if (pAd->PSEWatchDogEn)
		{
			PSEWatchDog(pAd);	
		}
#endif
	}

    /* Following is the TxOP scenario, monitor traffic in every minutes */
    if ((pAd->Mlme.PeriodicRound % 10) == 0)
    {
        TxTotalByteCnt = pAd->TxTotalByteCnt;
        RxTotalByteCnt = pAd->RxTotalByteCnt;

        //DBGPRINT(RT_DEBUG_TRACE, ("lala_1: [Tx Total Byte Count] = %lu\n", pAd->TxTotalByteCnt));
        //DBGPRINT(RT_DEBUG_TRACE, ("lala_1: [Rx Total Byte Count] = %lu\n", pAd->RxTotalByteCnt));

        if ((TxTotalByteCnt == 0) || (RxTotalByteCnt == 0))
        {
            /* Avoid to divide 0, when doing the traffic calculating */
           // DBGPRINT(RT_DEBUG_TRACE, ("Not expected one of them is 0, TxTotalByteCnt = %lu, RxTotalByteCnt = %lu\n", TxTotalByteCnt, RxTotalByteCnt));
        }
        else if ((pAd->MacTab.Size == 1) 
                && (pAd->CommonCfg.ManualTxop == 0) 
                && pAd->CommonCfg.bEnableTxBurst
           )
        {
            if ((((TxTotalByteCnt + RxTotalByteCnt) << 3) >> 20) > pAd->CommonCfg.ManualTxopThreshold)
            { /* TxopThreshold unit is Mbps */
                if (TxTotalByteCnt > RxTotalByteCnt)
                {
                    if ((TxTotalByteCnt/RxTotalByteCnt) >= pAd->CommonCfg.ManualTxopUpBound)
                    { /* Boundary unit is Ratio */
                        DBGPRINT(RT_DEBUG_TRACE, ("(Ratio: %lu >= %d) Should be enable TxOP to 0x60\n", 
                                TxTotalByteCnt/RxTotalByteCnt, pAd->CommonCfg.ManualTxopUpBound));
                        AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x60);
                    }
                    else if ((TxTotalByteCnt/RxTotalByteCnt) <= pAd->CommonCfg.ManualTxopLowBound)
                    {
                        if (pAd->chipCap.BiTxOpOn) 
                        {
                            AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x60);
                        }
                        else
                        {
                            DBGPRINT(RT_DEBUG_TRACE, ("(Ratio: %lu <= %d) Should be disable TxOP to 0x0\n", 
                                        TxTotalByteCnt/RxTotalByteCnt, pAd->CommonCfg.ManualTxopLowBound));
                            AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x0);
                        }
                    } 
                    else
                    {
                        DBGPRINT(RT_DEBUG_TRACE, ("(Ratio: %d < %lu < %d) Keep the last TxOP setting\n", 
                                pAd->CommonCfg.ManualTxopLowBound, TxTotalByteCnt/RxTotalByteCnt, pAd->CommonCfg.ManualTxopUpBound));
                    }
                }
                else
                {
                    if ((RxTotalByteCnt/TxTotalByteCnt) >= pAd->CommonCfg.ManualTxopUpBound)
                    { /* Boundary unit is Ratio */
                        DBGPRINT(RT_DEBUG_TRACE, ("(Ratio: %lu >= %d) Should be enable TxOP to 0x60\n", 
                                RxTotalByteCnt/TxTotalByteCnt, pAd->CommonCfg.ManualTxopUpBound));
                        AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x60);
                    }
                    else if ((RxTotalByteCnt/TxTotalByteCnt) <= pAd->CommonCfg.ManualTxopLowBound)
                    {
                        if (pAd->chipCap.BiTxOpOn) 
                        {
                            AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x60);
                        }
                        else
                        {
                            DBGPRINT(RT_DEBUG_TRACE, ("(Ratio: %lu <= %d) Should be disable TxOP to 0x0\n", 
                                        RxTotalByteCnt/TxTotalByteCnt, pAd->CommonCfg.ManualTxopLowBound));
                            AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x0);
                        }
                    } 
                    else
                    {
                        DBGPRINT(RT_DEBUG_TRACE, ("(Ratio: %d < %lu < %d) Keep the last TxOP setting\n", pAd->CommonCfg.ManualTxopLowBound, RxTotalByteCnt/TxTotalByteCnt, pAd->CommonCfg.ManualTxopUpBound));
                    }
                }
            }
            else
            {
                DBGPRINT(RT_DEBUG_TRACE, ("Current TP=%lu < Threshold(%lu), turn-off TxOP\n", 
                        (((TxTotalByteCnt + RxTotalByteCnt) << 3) >> 20), pAd->CommonCfg.ManualTxopThreshold));
		if (!pAd->bLink11b)
			AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x0);
            }
        }
        else if (pAd->MacTab.Size > 1)
        {
            DBGPRINT(RT_DEBUG_TRACE, ("Multi STA turn-off TxOP\n"));
            AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x0);
        }

        pAd->TxTotalByteCnt = 0;
        pAd->RxTotalByteCnt = 0;
    }
#endif /* MT_MAC */

#ifdef RTMP_MAC_USB
	/* execute every 100ms, update the Tx FIFO Cnt for update Tx Rate.*/
	NICUpdateFifoStaCounters(pAd);
#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_FIRMWARE_SUPPORT
	if (pAd->CommonCfg.CarrierDetect.Enable)
		CarrierDetectionPeriodicStateCtrl(pAd);
#endif /* CARRIER_DETECTION_FIRMWARE_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */
#endif /* RTMP_MAC_USB */

	/* by default, execute every 500ms */
	if ((pAd->ra_interval) && 
		((pAd->Mlme.PeriodicRound % (pAd->ra_interval / 100)) == 0))
	{

#ifdef MT_MAC
		if (pAd->chipCap.hif_type == HIF_MT) {
			//AsicRssiUpdate(pAd);
			//AsicTxCntUpdate(pAd, 0);
		}
#endif /* MT_MAC */

		if (RTMPAutoRateSwitchCheck(pAd) == TRUE )
		{
#ifdef CONFIG_AP_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
				APMlmeDynamicTxRateSwitching(pAd);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
			/* perform dynamic tx rate switching based on past TX history*/
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			{
				if ((OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)
#ifdef MESH_SUPPORT
					|| MESH_ON(pAd)
#endif /* MESH_SUPPORT */
#ifdef P2P_SUPPORT
					|| P2P_GO_ON(pAd) || P2P_CLI_ON(pAd)
#endif /* P2P_SUPPORT */
#ifdef RT_CFG80211_SUPPORT
					|| (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP)
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
					|| RTMP_CFG80211_VIF_P2P_CLI_ON(pAd)
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */ 					
#endif /* RT_CFG80211_SUPPORT */
				)
				&& (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE)))
					MlmeDynamicTxRateSwitching(pAd);
		}
#endif /* CONFIG_STA_SUPPORT */
		}else {
#ifdef MT_MAC
			if (pAd->chipCap.hif_type == HIF_MT) {
				MAC_TABLE_ENTRY *pEntry;
				MT_TX_COUNTER TxInfo;
				UINT16 i;
				for (i = 1; i < MAX_LEN_OF_MAC_TABLE; i++) 
				{
					/* point to information of the individual station */
					pEntry = &pAd->MacTab.Content[i];

					if (IS_ENTRY_NONE(pEntry))
						continue;

					if (IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst != SST_ASSOC))
						continue;

#ifdef APCLI_SUPPORT
					if (IS_ENTRY_APCLI(pEntry) && (pEntry->Sst != SST_ASSOC))
						continue;
#endif /* APCLI_SUPPORT */

#ifdef WDS_SUPPORT
					if (IS_ENTRY_WDS(pEntry) && !WDS_IF_UP_CHECK(pAd, pEntry->func_tb_idx))
						continue;
#endif /* WDS_SUPPORT */

#ifdef MESH_SUPPORT
					if (IS_ENTRY_MESH(pEntry) && !MESH_ON(pAd))
						continue;
#endif /* MESH_SUPPORT */

					DBGPRINT(RT_DEBUG_TRACE,("%s(line=%d), Hanmin debug\n", __FUNCTION__, __LINE__));
					AsicTxCntUpdate(pAd, pEntry, &TxInfo);
				}
			}
#endif /* MT_MAC */
		}
	}

#ifdef DFS_SUPPORT
#ifdef CONFIG_AP_SUPPORT
#ifdef RTMP_MAC_USB
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			if ((pAd->CommonCfg.Channel > 14) && (pAd->CommonCfg.bIEEE80211H == TRUE))
				NewUsbTimerCB_Radar(pAd);
		}
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_AP_SUPPORT */
#endif /* DFS_SUPPORT */

#ifdef SMART_ANTENNA
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if ((pAd->ra_interval) && 
			((pAd->Mlme.PeriodicRound % (pAd->ra_interval / 100)) == 0) && 
		 	RTMP_SA_WORK_ON(pAd))
		{
			if(RTMPAutoRateSwitchCheck(pAd) == FALSE)
			{
				TX_STA_CNT1_STRUC StaTx1;
				TX_STA_CNT0_STRUC TxStaCnt0;
				RTMP_SA_TRAINING_PARAM *pTrainEntry = &pAd->pSAParam->trainEntry[0];
					
				/* Update statistic counter */
				NicGetTxRawCounters(pAd, &TxStaCnt0, &StaTx1);

				if (pTrainEntry->pMacEntry && (pTrainEntry->trainStage != SA_INVALID_STAGE))
					pTrainEntry->mcsStableCnt++;
			}

			/* Check if need to run antenna adaptation */
			RtmpSAChkAndGo(pAd);
		}
	}
#endif /* SMART_ANTENNA */

#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
#ifdef CONFIG_STA_SUPPORT
	if (pAd->chipCap.FreqCalibrationSupport)
	{
		if ((pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration == TRUE) && 
		     (INFRA_ON(pAd)))
			FrequencyCalibration(pAd);	
		}
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */

	/* Normal 1 second Mlme PeriodicExec.*/
	if (pAd->Mlme.PeriodicRound %MLME_TASK_EXEC_MULTIPLE == 0)
	{
		pAd->Mlme.OneSecPeriodicRound ++;

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			dynamic_tune_be_tx_op(pAd, 50);	/* change form 100 to 50 for WMM WiFi test @20070504*/
#endif /* CONFIG_AP_SUPPORT */

		NdisGetSystemUpTime(&pAd->Mlme.Now32);

		/* add the most up-to-date h/w raw counters into software variable, so that*/
		/* the dynamic tuning mechanism below are based on most up-to-date information*/
		/* Hint: throughput impact is very serious in the function */
#ifndef RTMP_SDIO_SUPPORT
		NicUpdatFalseCCACounters(pAd);
		NICUpdateRawCounters(pAd);
#endif /*leonardo mark it for FPGA debug*/
		RTMP_SECOND_CCA_DETECTION(pAd);

#ifdef DYNAMIC_VGA_SUPPORT
		dynamic_vga_adjust(pAd);
#endif /* DYNAMIC_VGA_SUPPORT */

#ifdef RTMP_MAC_USB
#ifndef INF_AMAZON_SE
		//RTUSBWatchDog(pAd);
#endif /* INF_AMAZON_SE */
#endif /* RTMP_MAC_USB */

#ifdef DOT11_N_SUPPORT
   		/* Need statistics after read counter. So put after NICUpdateRawCounters*/
		ORIBATimerTimeout(pAd);
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_WIFI_TEST
		if (pAd->Mlme.OneSecPeriodicRound % 1 == 0)
		{
#ifdef RTMP_PCI_SUPPORT
			MonitorTxRing(pAd);
			MonitorTxRing(pAd);
			MonitorTxRing(pAd);
			MonitorTxRing(pAd);
#endif
		}
#endif

#ifdef DYNAMIC_VGA_SUPPORT
#ifdef CONFIG_AP_SUPPORT
#ifdef MT76x2
		if (IS_MT76x2(pAd)) {
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd) {
				if (pAd->Mlme.OneSecPeriodicRound % 1 == 0)
					periodic_monitor_false_cca_adjust_vga(pAd);
			}
		}
#endif
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef MT76x2
		if (IS_MT76x2(pAd)) {
			if (INFRA_ON(pAd)) {
				if (pAd->Mlme.OneSecPeriodicRound % 1 == 0)
					periodic_monitor_rssi_adjust_vga(pAd);
		
					/* only g band need to check if disable channel smoothing */
#if 1
					//if (pAd->CommonCfg.Channel <= 14)
						//periodic_check_channel_smoothing(pAd);
#endif
			}
		}
#endif
#endif
#endif /* DYNAMIC_VGA_SUPPORT */

#ifdef RELEASE_EXCLUDE
		/*
			If MGMT RING is full more than twice within 1 second, we consider there's
			a hardware problem stucking the TX path. In this case, try a hardware reset
			to recover the system
		*/
#endif /* RELEASE_EXCLUDE */
	/*
		if (pAd->RalinkCounters.MgmtRingFullCount >= 2)
			RTMP_SET_FLAG(pAd, fRTMP_HW_ERR);
		else
			pAd->RalinkCounters.MgmtRingFullCount = 0;
	*/

		/* The time period for checking antenna is according to traffic*/
#ifdef ANT_DIVERSITY_SUPPORT
		if ((pAd->CommonCfg.bSWRxAntDiversity) 
			 && (pAd->CommonCfg.RxAntDiversityCfg == ANT_SW_DIVERSITY_ENABLE) && 
			(!pAd->EepromAccess))
			AsicAntennaSelect(pAd, pAd->MlmeAux.Channel);
		else if(pAd->CommonCfg.RxAntDiversityCfg == ANT_FIX_ANT0 || pAd->CommonCfg.RxAntDiversityCfg == ANT_FIX_ANT1)
		{
#ifdef CONFIG_AP_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
				APAsicAntennaAvg(pAd, pAd->RxAnt.Pair1PrimaryRxAnt, &realavgrssi);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
			realavgrssi = (pAd->RxAnt.Pair1AvgRssi[pAd->RxAnt.Pair1PrimaryRxAnt] >> 3);
#endif /* CONFIG_STA_SUPPORT */
			DBGPRINT(RT_DEBUG_TRACE,("Ant-realrssi0(%d), Lastrssi0(%d), EvaluateStableCnt=%d\n", realavgrssi, pAd->RxAnt.Pair1LastAvgRssi, pAd->RxAnt.EvaluateStableCnt));
		}
		else
#endif /* ANT_DIVERSITY_SUPPORT */
		{
			if (pAd->Mlme.bEnableAutoAntennaCheck)
			{
				TxTotalCnt = pAd->RalinkCounters.OneSecTxNoRetryOkCount + 
								 pAd->RalinkCounters.OneSecTxRetryOkCount + 
								 pAd->RalinkCounters.OneSecTxFailCount;
				
				/* dynamic adjust antenna evaluation period according to the traffic*/
				if (TxTotalCnt > 50)
				{
					if (pAd->Mlme.OneSecPeriodicRound % 10 == 0)
						AsicEvaluateRxAnt(pAd);
				}
				else
				{
					if (pAd->Mlme.OneSecPeriodicRound % 3 == 0)
						AsicEvaluateRxAnt(pAd);
				}
			}
		}

#ifdef VIDEO_TURBINE_SUPPORT
	/*
		VideoTurbineUpdate(pAd);
		VideoTurbineDynamicTune(pAd);
	*/
#endif /* VIDEO_TURBINE_SUPPORT */

#ifdef MT76x0_TSSI_CAL_COMPENSATION
		if (IS_MT76x0(pAd) &&
			(pAd->chipCap.bInternalTxALC) &&
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF | fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET) == FALSE))
		{
			if ((pAd->Mlme.OneSecPeriodicRound % 1) == 0)
			{
				/* TSSI compensation */
				MT76x0_IntTxAlcProcess(pAd);
			}
		}
#endif /* MT76x0_TSSI_CAL_COMPENSATION */

#ifdef MT76x2
		if (IS_MT76x2(pAd) &&
			(pAd->chipCap.tssi_enable) &&
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF | 
				fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET) == FALSE)) {
			if ((pAd->Mlme.OneSecPeriodicRound % 1) == 0) {
#ifdef CONFIG_AP_SUPPORT
					mt76x2_tssi_compensation(pAd, pAd->hw_cfg.cent_ch);
#endif

#ifdef CONFIG_STA_SUPPORT
				if (INFRA_ON(pAd))
					mt76x2_tssi_compensation(pAd, pAd->hw_cfg.cent_ch);
#endif
			}
		}
#endif /* MT76x2 */

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF | fRTMP_ADAPTER_DISABLE_DEQUEUEPACKET) == FALSE)
		{
			if ((pAd->Mlme.OneSecPeriodicRound % 10) == 0)
			{
#ifdef MT76x0
				if (IS_MT76x0(pAd)) {
					if (pAd->chipCap.temp_sensor_enable)
						mt76x0_temp_trigger_cal(pAd);
					else
						mt76x0_vco_calibration(pAd, pAd->hw_cfg.cent_ch);
				}
				else
#endif /* MT76x0 */
				{
#ifdef VCORECAL_SUPPORT
					AsicVCORecalibration(pAd);
#endif /* VCORECAL_SUPPORT */
				}
			}
		}

#ifdef DOT11_N_SUPPORT
#ifdef MT_MAC
        if (pAd->chipCap.hif_type == HIF_MT)
        {
            /* Not RDG, update the TxOP else keep the default RDG's TxOP */
            if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE) == FALSE)
            {
#if 0
					if ((pAd->MacTab.Size == 1) 
						&& (pAd->CommonCfg.bEnableTxBurst))
                { /* Only update TxOP for racing */
                    AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x80);
                }
                else
                {
                    AsicUpdateTxOP(pAd, WMM_PARAM_AC_1, 0x0);
                }
#endif
            }
        }
#endif /* MT_MAC */
#endif /* DOT11_N_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			APMlmePeriodicExec(pAd);

#ifdef RELEASE_EXCLUDE
			/*
				The watchdog will prevent the hardware MAC into a deadlock condition
				1. No beacon frame sent within 2 seconds and
				2. No site-survey processing
				3. AP has started up
				4. WDS isn't bridge mode
				5. No Carrier signal exist.
			*/
#endif /* RELEASE_EXCLUDE */			
			if ((pAd->RalinkCounters.OneSecBeaconSentCnt == 0)
				&& (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
				&& (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
				&& ((pAd->CommonCfg.bIEEE80211H != 1)
					|| (pAd->Dot11_H.RDMode != RD_SILENCE_MODE))				
#ifdef WDS_SUPPORT
				&& (pAd->WdsTab.Mode != WDS_BRIDGE_MODE)		
#endif /* WDS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
				&& (isCarrierDetectExist(pAd) == FALSE)
#endif /* CARRIER_DETECTION_SUPPORT */
				)
				pAd->macwd ++;
			else
				pAd->macwd = 0;

#ifdef RELEASE_EXCLUDE
			/*
				When the beacon deadlock occurred over 2 seconds, reset MAC/BBP
				When the below message is appeared, it indicates 
				the MAC seems to occur deadlock. 
				So, driver will reset MAC to attempt recover the whole system.
				This is ASIC/Max's suggestion.
			*/
#endif /* RELEASE_EXCLUDE */
			if (pAd->macwd > 1)
			{
				DBGPRINT(RT_DEBUG_WARN, ("MAC specific condition \n"));

				AsicSetMacWD(pAd);

#ifdef AP_QLOAD_SUPPORT
				Show_QoSLoad_Proc(pAd, NULL);
#endif /* AP_QLOAD_SUPPORT */
			}
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
			RtmpPsActiveExtendCheck(pAd);
#ifdef TDLS_AUTOLINK_SUPPORT
   			 /* TDLS discovery link maintenance */
    			if (IS_TDLS_SUPPORT(pAd) && (pAd->StaCfg.TdlsInfo.TdlsAutoLink))
    			{
				TDLS_MaintainDiscoveryEntryList(pAd);
    			}
#endif /* TDLS_AUTOLINK_SUPPORT */
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */

			STAMlmePeriodicExec(pAd);
		}
#endif /* CONFIG_STA_SUPPORT */


#ifdef SMART_CARRIER_SENSE_SUPPORT
#if 0 /* move to MtCmdNICUpdateRawCounters*/
			pAd->SCSCtrl.PdCount = PDCnt;
			pAd->SCSCtrl.MdrdyCount = MDRDYCnt;
			pAd->SCSCtrl.FalseCCA = pAd->RalinkCounters.FalseCCACnt;
#endif

			if (pAd->SCSCtrl.SCSEnable == SCS_ENABLE)
				MTSmartCarrierSense(pAd);
#endif/*SMART_CARRIER_SENSE_SUPPORT */

#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
		if (IS_RT5392(pAd) && ((pAd->MACVersion & 0x0000FFFF) < 0x0223))
		{	
			if(pAd->BbpResetFlag ==1)
			{
				pAd->BbpResetFlagCount ++;
				if(pAd->BbpResetFlagCount > pAd->BbpResetFlagCountVale)
				{
					pAd->BbpResetFlagCount = 0;
					pAd->BbpResetFlag = 0;
				}
			}
			AsicCheckForHwRecovery(pAd);
		}
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)  */

		RTMP_SECOND_CCA_DETECTION(pAd);

		MlmeResetRalinkCounters(pAd);

#if defined(RTMP_MAC) || defined(RLT_MAC)
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (pAd->chipCap.hif_type == HIF_RTMP ||
				pAd->chipCap.hif_type == HIF_RLT)
			{
#ifdef RTMP_MAC_PCI
				if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) && (pAd->bPCIclkOff == FALSE))
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_PCI
				if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST) && 
					!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
				if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
#endif /* RTMP_MAC_USB */
				{
#ifdef RELEASE_EXCLUDE
					/*
						When Adhoc beacon is enabled and RTS/CTS is enabled, there is
						a chance that hardware MAC FSM will run into a deadlock and 
						sending CTS-to-self over and over.
						Software Patch Solution:
						1. Polling debug state register 0x10F4 every one second.
						2. If in 0x10F4 the ((bit29==1) && (bit7==1)) OR 
										((bit29==1) && (bit5==1)),
							it means the deadlock has occurred.
						3. If the deadlock occurred, reset MAC/BBP by setting 0x1004 
							to 0x0001 for a while then setting it back to 0x000C again.
					*/
#endif /* RELEASE_EXCLUDE */

					UINT32	MacReg = 0;

					RTMP_IO_READ32(pAd, 0x10F4, &MacReg);
					if (((MacReg & 0x20000000) && (MacReg & 0x80)) || ((MacReg & 0x20000000) && (MacReg & 0x20)))
					{
						RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x1);
						RtmpusecDelay(1);
						MacReg = 0;
#ifdef HW_COEXISTENCE_SUPPORT
						if (pAd->bHWCoexistenceInit)
						{
#ifdef BT_COEXISTENCE_SUPPORT
							if (pAd->bHWCoexistenceInit==TRUE && (!IS_RT5390BC8(pAd)) && (!IS_RT3592BC8(pAd)))
							{
								if(IS_ENABLE_BT_WIFI_ACTIVE_PULL_HIGH_BY_TIMER(pAd) && (pAd->BT_BC_PERMIT_RXWIFI_ACTIVE==TRUE))
									MacReg =  0x224c;
								else if(IS_ENABLE_WIFI_ACTIVE_PULL_LOW_BY_FORCE(pAd))
									MacReg = WLAN_WIFI_ACT_PULL_LOW;
							}
							else
#endif /* BT_COEXISTENCE_SUPPORT */
								MacReg = WLAN_WIFI_ACT_PULL_HIGH;
						}
						else
#endif /* HW_COEXISTENCE_SUPPORT */
						{
							MacReg = 0xc;
						}

						if (MacReg)
							RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacReg);

						DBGPRINT(RT_DEBUG_WARN,("Warning, MAC specific condition occurs \n"));
					}
				}
			}
		}
#endif /* CONFIG_STA_SUPPORT */
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

		RTMP_MLME_HANDLER(pAd);
	}

#ifdef RT6352
	if (IS_RT6352(pAd)) {
	if (pAd->CommonCfg.bEnTemperatureTrack == TRUE)
	{
#ifdef RTMP_INTERNAL_TX_ALC
		if (pAd->TxPowerCtrl.bInternalTxALC)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_TRACE, ("Track Power Compensation !!!\n"));
#endif /* RELEASE_EXCLUDE */

			if (RT635xCheckTssiCompensation(pAd))
			{
#ifdef RTMP_TEMPERATURE_CALIBRATION
				RT6352_TemperatureCalibration(pAd);
#endif /* RTMP_TEMPERATURE_CALIBRATION */
				DoDPDCalibration(pAd);
			}
		}
		else
#endif /* RTMP_INTERNAL_TX_ALC */
#ifdef RTMP_TEMPERATURE_COMPENSATION
		if (pAd->bAutoTxAgcG)
		{
			if (RT6352_TemperatureCompensation(pAd, FALSE) == TRUE)
				{
					INT32 TemperatureDiff = 0;

				pAd->CommonCfg.bEnTemperatureTrack = FALSE;

					TemperatureDiff = ((pAd->CurrTemperature - pAd->TemperatureRef25C) * 19) - pAd->DoCalibrationTemperature;

#ifdef RT6352_EP_SUPPORT				
					if ((TemperatureDiff >= 400) || (TemperatureDiff <= -400))
					{
						RT6352_ReCalibration(pAd);
						pAd->DoCalibrationTemperature = (pAd->CurrTemperature - pAd->TemperatureRef25C) * 19;
					}
#endif /* RT6352_EP_SUPPORT */
				}
		}
		else
#endif /* RTMP_TEMPERATURE_COMPENSATION */
		{
#ifdef RTMP_TEMPERATURE_CALIBRATION
			UCHAR bbpval;
			CHAR BBPR49;
				INT32 TemperatureDiff = 0;

			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &bbpval);
			if ((bbpval & 0x10) == 0)
			{
				bbpval &= 0xf8;
				bbpval |= 0x04;
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, bbpval);

				/* save temperature */ 
				RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BBPR49);
				pAd->CurrTemperature = (INT32) BBPR49;

				DBGPRINT(RT_DEBUG_INFO, ("Current Temperature from BBP_R49=0x%x\n", pAd->CurrTemperature));
				RT6352_TemperatureCalibration(pAd);
				pAd->CommonCfg.bEnTemperatureTrack = FALSE;

					TemperatureDiff = ((pAd->CurrTemperature - pAd->TemperatureRef25C) * 19) - pAd->DoCalibrationTemperature;

#ifdef RT6352_EP_SUPPORT				
					if ((TemperatureDiff >= 400) || (TemperatureDiff <= -400))
					{
						RT6352_ReCalibration(pAd);
						pAd->DoCalibrationTemperature = (pAd->CurrTemperature - pAd->TemperatureRef25C) * 19;
					}
#endif /* RT6352_EP_SUPPORT */
			}
#endif /* RTMP_TEMPERATURE_CALIBRATION */
		}
	}
	}
#endif /* RT6352 */

#ifdef WSC_INCLUDED
	WSC_HDR_BTN_MR_HANDLE(pAd);
#endif /* WSC_INCLUDED */

#ifdef __ECOS /* Polling restore button in APSOC */
#ifdef PLATFORM_BUTTON_SUPPORT
        if (pAd->CommonCfg.RestoreHdrBtnFlag)
                Restore_button_CheckHandler(pAd);
#endif /* PLATFORM_BUTTON_SUPPORT */
#endif /* __ECOS */

#ifdef P2P_SUPPORT
	if (P2P_INF_ON(pAd))
		P2pPeriodicExec(SystemSpecific1, FunctionContext, SystemSpecific2, SystemSpecific3);
#endif /* P2P_SUPPORT */


	/*add for hook function on Mlme timer interrupt*/
	RTMP_OS_TXRXHOOK_CALL(WLAN_TX_MLME_PERIOD,NULL,1,pAd);

	pAd->bUpdateBcnCntDone = FALSE;

	return NDIS_STATUS_SUCCESS;
}


/*
	==========================================================================
	Validate SSID for connection try and rescan purpose
	Valid SSID will have visible chars only.
	The valid length is from 0 to 32.
	IRQL = DISPATCH_LEVEL
	==========================================================================
 */
BOOLEAN MlmeValidateSSID(UCHAR *pSsid, UCHAR SsidLen)
{
	int index;

	if (SsidLen > MAX_LEN_OF_SSID)
		return (FALSE);

	/* Check each character value*/
	for (index = 0; index < SsidLen; index++)
	{
		if (pSsid[index] < 0x20)
			return (FALSE);
	}

	/* All checked*/
	return (TRUE);
}


#ifdef CONFIG_STA_SUPPORT
VOID STAMlmePeriodicExec(RTMP_ADAPTER *pAd)
{
	ULONG TxTotalCnt;
	BOOLEAN bCheckBeaconLost = TRUE;
#ifdef WPA3_SUPPORT
	BOOLEAN Status = TRUE;
#endif

#ifdef P2P_SUPPORT
	WSC_CTRL *pWscControl;

	if (P2P_CLI_ON(pAd))
		pWscControl = &pAd->ApCfg.ApCliTab[0].WscControl;
	else
		pWscControl = &pAd->ApCfg.MBSSID[0].WscControl;
#endif /* P2P_SUPPORT */


#ifdef RELEASE_EXCLUDE
	/*
		high-power issue:
		request by Brian, If our 2070/3070/305x AP/Station is too close to receive the stations/APs signal,
		we will dynamically tune the RF power(controlled by RF_R27).
		The tuning rules are
		1. If the connecting devices is too close, we will attenuate the RF power to 0V.
		2. Otherwise, recover the RF power to original level (0.15V)
	*/
#endif /* RELEASE_EXCLUDE */
	RTMP_CHIP_HIGH_POWER_TUNING(pAd, &pAd->StaCfg.RssiSample);

#ifdef WPA_SUPPLICANT_SUPPORT
	if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP == WPA_SUPPLICANT_DISABLE)    
#endif /* WPA_SUPPLICANT_SUPPORT */        
	{
		/* WPA MIC error should block association attempt for 60 seconds*/
		if (pAd->StaCfg.bBlockAssoc && 
			RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastMicErrorTime + (60*OS_HZ)))
			pAd->StaCfg.bBlockAssoc = FALSE;
	}

#ifdef RTMP_MAC_USB
	/* If station is idle, go to sleep*/
	if ( 1
		/*	&& (pAd->StaCfg.PSControl.field.EnablePSinIdle == TRUE)*/
		&& (pAd->StaCfg.WindowsPowerMode > 0)
		&& (pAd->OpMode == OPMODE_STA) && (IDLE_ON(pAd)) 
		&& (pAd->Mlme.SyncMachine.CurrState == SYNC_IDLE)
		&& (pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE)
		&& (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
#ifdef WSC_STA_SUPPORT
		&& (pAd->StaCfg.WscControl.WscState == WSC_STATE_OFF)
#endif /* WSC_STA_SUPPORT */
#ifdef P2P_SUPPORT
		&& (!P2P_INF_ON(pAd))
#endif /* P2P_SUPPORT */
	)
	{
		ASIC_RADIO_OFF(pAd, MLME_RADIO_OFF);

#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND
		{
			POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;

			if(!RTMP_Usb_AutoPM_Put_Interface(pObj->pUsb_Dev,pObj->intf))
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
		}
#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */

		DBGPRINT(RT_DEBUG_TRACE, ("PSM - Issue Sleep command)\n"));
	}

#endif /* RTMP_MAC_USB */

#ifdef ETH_CONVERT_SUPPORT
	if ((pAd->EthConvert.ECMode & ETH_CONVERT_MODE_CLONE)
		&& (pAd->EthConvert.CloneMacVaild == TRUE)
		&& (NdisEqualMemory(pAd->CurrentAddress, pAd->EthConvert.EthCloneMac, MAC_ADDR_LEN) == FALSE))
	{
		MAC_DW0_STRUC csr2;
		MAC_DW1_STRUC csr3;

		/* Link down first	*/
		if (INFRA_ON(pAd))
		{

			if (pAd->MlmeAux.SsidLen) {
				NdisZeroMemory(pAd->EthConvert.SSIDStr, MAX_LEN_OF_SSID);
				NdisMoveMemory(pAd->EthConvert.SSIDStr, pAd->MlmeAux.Ssid, pAd->MlmeAux.SsidLen);
				pAd->EthConvert.SSIDStrLen = pAd->MlmeAux.SsidLen;
			}

			pAd->MlmeAux.CurrReqIsFromNdis = TRUE;
			if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
			{
				RTMP_MLME_RESET_STATE_MACHINE(pAd);
				DBGPRINT(RT_DEBUG_TRACE, ("!!! MLME busy, reset MLME state machine !!!\n"));
			}

			MlmeEnqueue(pAd, 
							MLME_CNTL_STATE_MACHINE, 
							OID_802_11_DISASSOCIATE, 
							0, 
							NULL, 0);
		}
		else if (ADHOC_ON(pAd))
		{
			MakeIbssBeacon(pAd);		/* re-build BEACON frame*/
			AsicEnableIbssSync(pAd);	/* copy to on-chip memory*/
			LinkDown(pAd, FALSE);
		}
		else
		{
			/* Copy the new Mac address to ASIC and start to re-connect to AP.*/
			NdisMoveMemory(&pAd->CurrentAddress[0], &pAd->EthConvert.EthCloneMac[0], MAC_ADDR_LEN);

			AsicSetDevMac(pAd, pAd->CurrentAddress, 0x0);

			DBGPRINT_RAW(RT_DEBUG_TRACE,("Write EthCloneMac to ASIC: =%02x:%02x:%02x:%02x:%02x:%02x\n",
			PRINT_MAC(pAd->CurrentAddress)));

			if(pAd->EthConvert.SSIDStrLen != 0)
			{
				/*DBGPRINT_RAW(RT_DEBUG_TRACE, ("copy MlmeAux.Ssid to AutoReconnect!\n"));*/
				NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid,
									pAd->EthConvert.SSIDStr,
									pAd->EthConvert.SSIDStrLen);
				pAd->MlmeAux.AutoReconnectSsidLen = pAd->EthConvert.SSIDStrLen;
				NdisMoveMemory(pAd->MlmeAux.Ssid, pAd->EthConvert.SSIDStr, pAd->EthConvert.SSIDStrLen);
				pAd->MlmeAux.SsidLen= pAd->EthConvert.SSIDStrLen;
			}
		}
	}
#endif /* ETH_CONVERT_SUPPORT */

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO,("MMCHK - CommonCfg.Ssid[%d]=%c%c%c%c... MlmeAux.Ssid[%d]=%c%c%c%c...\n",
				pAd->CommonCfg.SsidLen, 
				pAd->CommonCfg.Ssid[0], pAd->CommonCfg.Ssid[1], 
				pAd->CommonCfg.Ssid[2], pAd->CommonCfg.Ssid[3],
				pAd->MlmeAux.SsidLen, 
				pAd->MlmeAux.Ssid[0], pAd->MlmeAux.Ssid[1],
				pAd->MlmeAux.Ssid[2], pAd->MlmeAux.Ssid[3]));
#endif /* RELEASE_EXCLUDE */

	if (ADHOC_ON(pAd))
	{
	}
	else
	{
		AsicStaBbpTuning(pAd);
	}

	TxTotalCnt = pAd->RalinkCounters.OneSecTxNoRetryOkCount + 
	pAd->RalinkCounters.OneSecTxRetryOkCount + 
	pAd->RalinkCounters.OneSecTxFailCount;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED) && 
		(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)))
	{
		/* update channel quality for Roaming/Fast-Roaming and UI LinkQuality display*/
		/* bImprovedScan True means scan is not completed */
		if (pAd->StaCfg.bImprovedScan)
			bCheckBeaconLost = FALSE;

#ifdef P2P_SUPPORT
		if (NdisEqualMemory(ZERO_MAC_ADDR, pAd->P2pCfg.ConnectingMAC, MAC_ADDR_LEN) == FALSE)
		{
			UCHAR p2pindex;
			p2pindex = P2pGroupTabSearch(pAd, pAd->P2pCfg.ConnectingMAC);
			if (p2pindex != P2P_NOT_FOUND)
			{
				if (pAd->P2pTable.Client[p2pindex].P2pClientState > P2PSTATE_DISCOVERY_UNKNOWN)
					bCheckBeaconLost = FALSE;
			}			
		}
#endif /* P2P_SUPPORT */

#ifdef CONFIG_WIFI_TEST
		if (pAd->chipCap.pbf_loopback)
			bCheckBeaconLost = FALSE;
#endif /* CONFIG_WIFI_TEST */

		if (bCheckBeaconLost)
		{
			/* The NIC may lost beacons during scaning operation.*/
			MAC_TABLE_ENTRY *pEntry = &pAd->MacTab.Content[BSSID_WCID];
			MlmeCalculateChannelQuality(pAd, pEntry, pAd->Mlme.Now32);
		}
	}

	/* must be AFTER MlmeDynamicTxRateSwitching() because it needs to know if*/
	/* Radio is currently in noisy environment*/
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
	{
		RTMP_CHIP_ASIC_ADJUST_TX_POWER(pAd);
		RTMP_CHIP_ASIC_TEMPERATURE_COMPENSATION(pAd);
	}

#ifdef MT76XX_BTCOEX_SUPPORT
#ifdef RTMP_PCI_SUPPORT
	if(pAd->bPCIclkOff == FALSE && IS_MT76XXBTCOMBO(pAd))		
#endif /*RTMP_PCI_SUPPORT*/	
#ifdef RTMP_USB_SUPPORT
	if(IS_MT76XXBTCOMBO(pAd))		
#endif /*RTMP_PCI_SUPPORT*/	
	{
			CoexistencePeriodicRoutine(pAd);
#if 0
		//temporary status checking for coex
		WLAN_FUN_CTRL_STRUC wlanFunInfo = {.word = 0};
		RTMP_IO_READ32(pAd, WLAN_FUN_INFO, &wlanFunInfo.word);

		//COEX periodic checking
		DBGPRINT(RT_DEBUG_TRACE,("COEX: StaCfg.AvgRssi[0] = %d\n",pAd->StaCfg.RssiSample.AvgRssi[0]));
		DBGPRINT(RT_DEBUG_TRACE,("COEX:CoexMode = %d\n",pAd->CoexMode.CurrentMode));
	             if ((IS_MT7650(pAd) || IS_MT7630(pAd)) && (wlanFunInfo.field.COEX_MODE & 0x4))
		{
			if (pAd->CoexMode.CurrentMode == COEX_MODE_FDD)
			{
				TDDFDDCoexBACapability(pAd, COEX_MODE_FDD);
			} else if  (pAd->CoexMode.CurrentMode == COEX_MODE_TDD)
			{
				TDDFDDCoexBACapability(pAd, COEX_MODE_TDD);
			}
			else 
			{
				DBGPRINT(RT_DEBUG_TRACE,("COEX:Strange CoexMode = %d\n",pAd->CoexMode.CurrentMode));
			}
		}
	             else if (IS_MT7650(pAd)  || IS_MT7630(pAd))
		{
			TDDFDDCoexBACapability(pAd, COEX_MODE_RESET);
		}

		if (pAd->CoexMode.TDDRequest == FALSE)
	        {
	            SendAndesTFSWITCH(pAd, COEX_MODE_TDD);
	        }

         		if ((IS_MT7650(pAd)  || IS_MT7630(pAd)) && INFRA_ON(pAd))
		{
			DBGPRINT(RT_DEBUG_TRACE,("COEX: pAd->CoexMode.CoexTDDRSSITreshold = %d\n",pAd->CoexMode.CoexTDDRSSITreshold));
			DBGPRINT(RT_DEBUG_TRACE,("COEX: pAd->CoexMode.CoexFDDRSSITreshold = %d\n",pAd->CoexMode.CoexFDDRSSITreshold));
			//MT76xx coex RX LNA gain adjustment
			if (pAd->CoexMode.CurrentMode == COEX_MODE_FDD)
			{
				if (pAd->StaCfg.RssiSample.AvgRssi[0] < (pAd->CoexMode.CoexTDDRSSITreshold) )
				{
					if (pAd->CoexMode.TDDRequest == FALSE)
					{
						SendAndesTFSWITCH(pAd, COEX_MODE_TDD);
					}
				}

				if (pAd->StaCfg.RssiSample.AvgRssi[0] > (pAd->CoexMode.CoexFDDRSSITreshold) )
				{
					if (pAd->CoexMode.FDDRequest == FALSE)
					{
						SendAndesTFSWITCH(pAd, COEX_MODE_FDD);
					}
				}                   
				CoexFDDRXAGCGain(pAd, pAd->StaCfg.RssiSample.AvgRssi[0]);
			}
			else if (pAd->CoexMode.CurrentMode == COEX_MODE_TDD)
			{
				if (pAd->StaCfg.RssiSample.AvgRssi[0] > (pAd->CoexMode.CoexFDDRSSITreshold) )
				{
					if (pAd->CoexMode.FDDRequest == FALSE)
					{
						SendAndesTFSWITCH(pAd, COEX_MODE_FDD);
					}
				}

				if (pAd->StaCfg.RssiSample.AvgRssi[0] < (pAd->CoexMode.CoexTDDRSSITreshold) )
				{
					if (pAd->CoexMode.TDDRequest == FALSE)
					{
						SendAndesTFSWITCH(pAd, COEX_MODE_TDD);
					}
				}

				CoexTDDRXAGCGain(pAd); 
			}
		}
#endif
	}
#endif /*MT76XX_BTCOEX_SUPPORT*/

	/*
		Driver needs to up date value of LastOneSecTotalTxCount here;
		otherwise UI couldn't do scanning sometimes when STA doesn't connect to AP or peer Ad-Hoc.
	*/
	pAd->RalinkCounters.LastOneSecTotalTxCount = TxTotalCnt;

#ifdef RT3290
	// TODO: shiang, what's this and how about in AP mode??
	// Need to enable PCIE_APP0_CLK_REQ for HT40 throughput
	if (IS_RT3290(pAd) && (pAd->CommonCfg.BBPCurrentBW == BW_40))
	{
		WLAN_FUN_CTRL_STRUC WlanFunCtrl = {.word = 0};
		if ((pAd->RalinkCounters.LastOneSecTotalTxCount + pAd->RalinkCounters.LastOneSecRxOkDataCnt) >= 2000)
		{
			RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);
			if ((WlanFunCtrl.field.WLAN_EN == TRUE) && (WlanFunCtrl.field.PCIE_APP0_CLK_REQ == FALSE))
			{
				WlanFunCtrl.field.PCIE_APP0_CLK_REQ = TRUE;
				RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
			}
		}
		else if ((pAd->RalinkCounters.LastOneSecTotalTxCount + pAd->RalinkCounters.LastOneSecRxOkDataCnt) <= 500)
		{
			RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);
			if ((WlanFunCtrl.field.WLAN_EN == TRUE) && (WlanFunCtrl.field.PCIE_APP0_CLK_REQ == TRUE))
			{
				WlanFunCtrl.field.PCIE_APP0_CLK_REQ = FALSE;
				RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word); 
			}
		}
	}

	//HWAntennaSwitch
	if (IS_RT3290(pAd) && 
		((pAd->NicConfig3.word != 0xFFFF) && (pAd->NicConfig3.field.CoexAnt == TRUE)) && 
		((pAd->Mlme.OneSecPeriodicRound % 5) == 4))	
	{
		WLAN_FUN_CTRL_STRUC WlanFunCtrl = {.word = 0};
		ULONG MacRegValue;

		DBGPRINT(RT_DEBUG_INFO, ("!!! BBP_R150 = %d, BT_EN = %d\n",pAd->BbpWriteLatch[BBP_R150], pAd->BtFunCtrl.field.BT_EN));

		//Check BT_EN
		if ((((pAd->WlanFunInfo.field.COEX_MODE&0x1E) == 0) || (pAd->BtFunCtrl.field.BT_EN == 0)) && (pAd->BbpWriteLatch[BBP_R150] != 0xc0))
		{
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) 
				|| (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND)) 
				|| RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
				AsicForceWakeup(pAd, DOT11POWERSAVE);

			//Enable RX antenna switch
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, 0xc0);
			DBGPRINT(RT_DEBUG_TRACE, ("!!! EN BBP_R150 = %d\n",pAd->BbpWriteLatch[BBP_R150]));

			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, 0xb0);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, 0x23);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R153, 0x34);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R154, 0x10);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R155, 0x3b);
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R253, 0x05);	

			//Disable FIX_WL_ANT_SEL
			RTMP_IO_READ32(pAd, COEXCFG0, &MacRegValue);
			MacRegValue &= ~(0x00000004);
			RTMP_IO_WRITE32(pAd, COEXCFG0, MacRegValue);			

			// Fixed to Main Antenna
			RTMP_SEM_LOCK(&pAd->WlanEnLock);
			RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);
			if (WlanFunCtrl.field.WLAN_EN == TRUE)
			{			
				if (WlanFunCtrl.field.INV_TR_SW0)
				{
					WlanFunCtrl.field.INV_TR_SW0 = 0;			
					RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
				}
				DBGPRINT(RT_DEBUG_TRACE, ("AsicSetRxAnt, switch to main antenna\n"));
				pAd->WlanFunCtrl.word = WlanFunCtrl.word;	
			}
			RTMP_SEM_UNLOCK(&pAd->WlanEnLock);


			//Disable SW antanna selection
			pAd->NicConfig2.field.AntDiversity = FALSE;	
		}
		else if(((pAd->WlanFunInfo.field.COEX_MODE&0x1E) != 0) && (pAd->BbpWriteLatch[BBP_R150] != 0x00))
		{
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) 
				|| (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND)) 
				|| RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
				AsicForceWakeup(pAd, DOT11POWERSAVE);

			//Disable RX antenna switch
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, 0x00);
			DBGPRINT(RT_DEBUG_TRACE, ("!!! DISABLE BBP_R150 = %d\n",pAd->BbpWriteLatch[BBP_R150]));

			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, 0x00);

			//Enable FIX_WL_ANT_SEL
			RTMP_IO_READ32(pAd, COEXCFG0, &MacRegValue);
			MacRegValue |= 0x00000004;
			RTMP_IO_WRITE32(pAd, COEXCFG0, MacRegValue);	

			//Enable SW antanna selection
			pAd->NicConfig2.word = pAd->EEPROMDefaultValue[EEPROM_NIC_CFG2_OFFSET];
		}	
	}
#endif /* RT3290 */

#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT)
	/* MAC table maintenance */
	if ((pAd->Mlme.PeriodicRound % MLME_TASK_EXEC_MULTIPLE == 0) &&
#ifdef RT_CFG80211_P2P_SUPPORT
	    (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP)	
#else
	    P2P_GO_ON(pAd)
#endif /* RT_CFG80211_P2P_SUPPORT */
	   )
	{
		/* one second timer */
#ifdef RT_CFG80211_P2P_SUPPORT
		MacTableMaintenance(pAd);
#else
		P2PMacTableMaintenance(pAd);
#endif /* RT_CFG80211_P2P_SUPPORT */

#ifdef DOT11_N_SUPPORT
		if (pAd->CommonCfg.bHTProtect)
		{
			APUpdateOperationMode(pAd);
			if (pAd->CommonCfg.IOTestParm.bRTSLongProtOn == FALSE)
			{
				AsicUpdateProtect(pAd, (USHORT)pAd->CommonCfg.AddHTInfo.AddHtInfo2.OperaionMode,
									ALLN_SETPROTECT, FALSE, pAd->MacTab.fAnyStationNonGF);
			}
		}
#endif /* DOT11_N_SUPPORT */
	}
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT */

	/* resume Improved Scanning*/
	if ((pAd->StaCfg.bImprovedScan) &&
		(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)) &&
		(pAd->Mlme.SyncMachine.CurrState == SCAN_PENDING))
	{
		MLME_SCAN_REQ_STRUCT ScanReq;

		pAd->StaCfg.LastScanTime = pAd->Mlme.Now32;

		ScanParmFill(pAd, &ScanReq, pAd->MlmeAux.Ssid, pAd->MlmeAux.SsidLen, BSS_ANY, pAd->MlmeAux.ScanType);
		MlmeEnqueue(pAd, SYNC_STATE_MACHINE, MT2_MLME_SCAN_REQ, sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
		DBGPRINT(RT_DEBUG_WARN, ("bImprovedScan ............. Resume for bImprovedScan, SCAN_PENDING .............. \n"));
	}

	if (INFRA_ON(pAd))
	{
#ifdef QOS_DLS_SUPPORT
		/* Check DLS time out, then tear down those session*/
		RTMPCheckDLSTimeOut(pAd);
#endif /* QOS_DLS_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
		/* TDLS link maintenance*/
		if (IS_TDLS_SUPPORT(pAd))
		{
			if (pAd->Mlme.PeriodicRound % MLME_TASK_EXEC_MULTIPLE == 0)
				TDLS_LinkMaintenance(pAd);
		}
#endif /* DOT11Z_TDLS_SUPPORT */

		/* Is PSM bit consistent with user power management policy?*/
		/* This is the only place that will set PSM bit ON.*/
		if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
			MlmeCheckPsmChange(pAd, pAd->Mlme.Now32);

		/*
			When we are connected and do the scan progress, it's very possible we cannot receive
			the beacon of the AP. So, here we simulate that we received the beacon.
		*/
		if ((bCheckBeaconLost == FALSE) && 
			RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS) && 
			(RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastBeaconRxTime + (1*OS_HZ))))
		{
			ULONG BPtoJiffies;
			LONG timeDiff;

			BPtoJiffies = (((pAd->CommonCfg.BeaconPeriod * 1024 / 1000) * OS_HZ) / 1000);
			timeDiff = (pAd->Mlme.Now32 - pAd->StaCfg.LastBeaconRxTime) / BPtoJiffies;
			if (timeDiff > 0) 
				pAd->StaCfg.LastBeaconRxTime += (timeDiff * BPtoJiffies);

			if (RTMP_TIME_AFTER(pAd->StaCfg.LastBeaconRxTime, pAd->Mlme.Now32))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - BeaconRxTime adjust wrong(BeaconRx=0x%lx, Now=0x%lx)\n", 
				pAd->StaCfg.LastBeaconRxTime, pAd->Mlme.Now32));
			}
		}

#ifdef MT7628_FPGA
		if ((RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastBeaconRxTime + (350 *OS_HZ))) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)) &&
			(pAd->StaCfg.bImprovedScan == FALSE) &&
			((TxTotalCnt + pAd->RalinkCounters.OneSecRxOkCnt) < 600))
#else
		if ((RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastBeaconRxTime + (1*OS_HZ))) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)) &&
			(pAd->StaCfg.bImprovedScan == FALSE) &&
			((TxTotalCnt + pAd->RalinkCounters.OneSecRxOkCnt) < 600))
#endif /* MT7628_FPGA */
		{
#if defined(MT7636) || defined(MT7603)
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) == 0)
#endif
			{
			RTMPSetAGCInitValue(pAd, BW_20);
			DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - No BEACON. restore R66 to the low bound(%d) \n", (0x2E + pAd->hw_cfg.lan_gain)));
		}
		}

#ifdef RTMP_MAC_USB
#if defined(RTMP_MAC) || defined(RLT_MAC)
#ifdef DOT11_N_SUPPORT
		if (pAd->chipCap.hif_type != HIF_MT)
		{
#ifdef RELEASE_EXCLUDE
			/*
				For 1X1 STA pass 11n wifi wmm, need to change txop per case;
				1x1 device for 802.11n WMM Test.
				New chip may not need this for TGn WMM test.
			*/
#endif /* RELEASE_EXCLUDE */
			sta_wmm_adjust(pAd);
		}
#endif /* DOT11_N_SUPPORT */
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

		/* TODO: for debug only. to be removed*/
		pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BE] = 0;
		pAd->RalinkCounters.OneSecOsTxCount[QID_AC_BK] = 0;
		pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VI] = 0;
		pAd->RalinkCounters.OneSecOsTxCount[QID_AC_VO] = 0;
		pAd->RalinkCounters.OneSecDmaDoneCount[QID_AC_BE] = 0;
		pAd->RalinkCounters.OneSecDmaDoneCount[QID_AC_BK] = 0;
		pAd->RalinkCounters.OneSecDmaDoneCount[QID_AC_VI] = 0;
		pAd->RalinkCounters.OneSecDmaDoneCount[QID_AC_VO] = 0;
		pAd->RalinkCounters.OneSecTxDoneCount = 0;
		pAd->RalinkCounters.OneSecTxARalinkCnt = 0;
		pAd->RalinkCounters.OneSecRxARalinkCnt = 0;
#endif /* RTMP_MAC_USB */


#ifdef WMM_ACM_SUPPORT
		/* WMM ACM fix: we are AP and we can send any packet even no TSPEC */
		/* if the ACM of all AC are enabled, we can not send any packet without
		following any TSPEC */
		/*		if (ACMP_IsAllACEnabled(pAd) != ACM_RTN_OK)*/
#endif /* WMM_ACM_SUPPORT */
		/*if ((pAd->RalinkCounters.OneSecTxNoRetryOkCount == 0) &&*/
		/*    (pAd->RalinkCounters.OneSecTxRetryOkCount == 0))*/
		if ((!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
			&& (pAd->cfg80211_ctrl.IsInListenProgress == FALSE)) /*if in listen progress, no need to send Null Data*/
		{
			if (pAd->StaCfg.wdev.UapsdInfo.bAPSDCapable && pAd->CommonCfg.APEdcaParm.bAPSDCapable)
			{
				/* When APSD is enabled, the period changes as 20 sec*/
				if ((pAd->Mlme.OneSecPeriodicRound % 20) == 8)
				{
					/* In 7603, it doesn't support redirect feature, driver needs to wakup h/w firstly before send keep alive null */
					/* In 7636, it supports redirect feature, driver needs not to wakup h/w firstly before send keep alive null */
#ifdef STA_LP_PHASE_1_SUPPORT
					if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
					{
						AsicForceWakeup(pAd, TRUE);
					}
#endif /* STA_LP_PHASE_1_SUPPORT */		

					DBGPRINT(RT_DEBUG_TRACE, ("%s(line=%d)\n", __FUNCTION__, __LINE__));
					RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate, TRUE, pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.PwrMgmt.Psm);
				}
			}
			else
			{
				/* Send out a NULL frame every 10 sec to inform AP that STA is still alive (Avoid being age out)*/
				if ((pAd->Mlme.OneSecPeriodicRound % 10) == 8)
				{
					DBGPRINT(RT_DEBUG_TRACE, ("%s::keep alive Null (%d, %d)\n", __FUNCTION__, pAd->CommonCfg.bAPSDForcePowerSave, pAd->StaCfg.PwrMgmt.Psm));
					/* In 7603, it doesn't support redirect feature, driver needs to wakup h/w firstly before send keep alive null */
					/* In 7636, it supports redirect feature, driver needs not to wakup h/w firstly before send keep alive null */
#ifdef STA_LP_PHASE_1_SUPPORT
					if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
					{
						AsicForceWakeup(pAd, TRUE);
						DBGPRINT(RT_DEBUG_TRACE, ("%s::keep alive Null, wait H/W wakeup\n", __FUNCTION__));
					}
#endif /* STA_LP_PHASE_1_SUPPORT */			

					DBGPRINT(RT_DEBUG_TRACE, ("%s(line=%d)\n", __FUNCTION__, __LINE__));
					RTMPSendNullFrame(pAd, 
										pAd->CommonCfg.TxRate, 
										(pAd->CommonCfg.bWmmCapable & pAd->CommonCfg.APEdcaParm.bValid),
										pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.PwrMgmt.Psm);
					
				}
#ifdef MT7601
				// TODO: shiang-usw, what's the purpose to set this here?
				else
				{
					pAd->Mlme.bSendNullFrameAfterWareUp = TRUE;
				}
#endif /* MT7601 */
			}
		}

		if (CQI_IS_DEAD(pAd->Mlme.ChannelQuality))
		{
#if defined(STA_LP_PHASE_1_SUPPORT) || defined(STA_LP_PHASE_2_SUPPORT)
			pAd->StaCfg.PwrMgmt.bBeaconLost = FALSE;
#endif			
			
			DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - No BEACON. Dead CQI. Auto Recovery attempt #%ld\n",
						pAd->RalinkCounters.BadCQIAutoRecoveryCount));

			if (pAd->StaCfg.bAutoConnectByBssid)
				pAd->StaCfg.bAutoConnectByBssid = FALSE;

#ifdef WPA_SUPPLICANT_SUPPORT
			if ((pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP != WPA_SUPPLICANT_DISABLE) &&
				(pAd->StaCfg.wdev.AuthMode == Ndis802_11AuthModeWPA2))
				pAd->StaCfg.wpa_supplicant_info.bLostAp = TRUE;
#endif /* WPA_SUPPLICANT_SUPPORT */

			pAd->MlmeAux.CurrReqIsFromNdis = FALSE;
			/* Lost AP, send disconnect & link down event*/
#ifdef WPA3_SUPPORT
			/* Compose a DEAUTH to inform AP & supplicant */
			Status = SendDeauthFrame(pAd);
			if (Status == FALSE)
				DBGPRINT(RT_DEBUG_ERROR, ("%s: failed to send Deauth\n", __func__));
#endif
			LinkDown(pAd, FALSE);

#ifdef STA_EASY_CONFIG_SETUP
			pAd->Mlme.AutoProvisionMachine.CurrState = INFRA_AUTO_PROVISION;
			MlmeEnqueue(pAd, AUTO_PROVISION_STATE_MACHINE, MT2_CONN_AP, 0, NULL, 0);
			RTMP_MLME_HANDLER(pAd);
#endif /* STA_EASY_CONFIG_SETUP */
			/* should mark this two function, because link down alse will call this function */
#if 0
#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
			/*send disassociate event to wpa_supplicant*/
			if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP) 
			{
				RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_DISASSOC_EVENT_FLAG, NULL, NULL, 0);
			} 
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
			RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CGIWAP, -1, NULL, NULL, 0);
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */        
#endif			
			/* RTMPPatchMacBbpBug(pAd);*/
#ifdef WPA_SUPPLICANT_SUPPORT
			if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP == WPA_SUPPLICANT_DISABLE)
#endif /* WPA_SUPPLICANT_SUPPORT */			
				MlmeAutoReconnectLastSSID(pAd);
		}
		else if (CQI_IS_BAD(pAd->Mlme.ChannelQuality))
		{
			pAd->RalinkCounters.BadCQIAutoRecoveryCount ++;
			DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - Bad CQI. Auto Recovery attempt #%ld\n",
						pAd->RalinkCounters.BadCQIAutoRecoveryCount));
			MlmeAutoReconnectLastSSID(pAd);
		}

		if (pAd->StaCfg.bAutoRoaming)
		{
			BOOLEAN	rv = FALSE;
			CHAR	dBmToRoam = pAd->StaCfg.dBmToRoam;
			CHAR 	MaxRssi = RTMPMaxRssi(pAd, 
			pAd->StaCfg.RssiSample.LastRssi[0], 
			pAd->StaCfg.RssiSample.LastRssi[1], 
			pAd->StaCfg.RssiSample.LastRssi[2]);			

			if (pAd->StaCfg.bAutoConnectByBssid)
				pAd->StaCfg.bAutoConnectByBssid = FALSE;

			/* Scanning, ignore Roaming*/
			if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS) &&
				(pAd->Mlme.SyncMachine.CurrState == SYNC_IDLE) &&
				(MaxRssi <= dBmToRoam))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("Rssi=%d, dBmToRoam=%d\n", MaxRssi, (CHAR)dBmToRoam));

#ifdef DOT11R_FT_SUPPORT
				if (pAd->StaCfg.Dot11RCommInfo.bFtSupport &&
					pAd->StaCfg.Dot11RCommInfo.bInMobilityDomain)
					rv = FT_CheckForRoaming(pAd);
#endif /* DOT11R_FT_SUPPORT */

				/* Add auto seamless roaming*/
				if (rv == FALSE)
					rv = MlmeCheckForFastRoaming(pAd);

				if (rv == FALSE)
				{
					if ((pAd->StaCfg.LastScanTime + 10 * OS_HZ) < pAd->Mlme.Now32)
					{
						DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - Roaming, No eligable entry, try new scan!\n"));
						pAd->StaCfg.LastScanTime = pAd->Mlme.Now32;
						MlmeAutoScan(pAd);
					}
				}
			}
		}
	}
	else if (ADHOC_ON(pAd))
	{
		Adhoc_checkPeerBeaconLost(pAd);
	}
	else /* no INFRA nor ADHOC connection*/
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("STAMlmePeriodicExec, no association so far\n"));
#endif /* RELEASE_EXCLUDE */		
#ifdef MESH_SUPPORT
		if (MESH_ON(pAd))
			goto SKIP_AUTO_SCAN_CONN;
#endif /* MESH_SUPPORT */        

#ifdef P2P_SUPPORT
		if (P2P_GO_ON(pAd) || P2P_CLI_ON(pAd))
			goto SKIP_AUTO_SCAN_CONN;
#endif /* P2P_SUPPORT */

#ifdef WPA_SUPPLICANT_SUPPORT
		if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & WPA_SUPPLICANT_ENABLE_WPS)
			goto SKIP_AUTO_SCAN_CONN;
#endif /* WPA_SUPPLICANT_SUPPORT */

		if (pAd->StaCfg.bSkipAutoScanConn &&
			RTMP_TIME_BEFORE(pAd->Mlme.Now32, pAd->StaCfg.LastScanTime + (30 * OS_HZ)))
			goto SKIP_AUTO_SCAN_CONN;
		else
			pAd->StaCfg.bSkipAutoScanConn = FALSE;

		if ((pAd->StaCfg.bAutoReconnect == TRUE)
			&& RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP)
			&& (MlmeValidateSSID(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen) == TRUE) && (pAd->CommonCfg.Channel != 0))
		{
			if ((pAd->ScanTab.BssNr==0) && (pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE)
#ifdef WIDI_SUPPORT /* This was giving trouble by turning off passive scanning */
				&& (pAd->StaCfg.bWIDI == FALSE)
#endif /* WIDI_SUPPORT */				
			)
			{
				MLME_SCAN_REQ_STRUCT	   ScanReq;

				if (RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastScanTime + (10 * OS_HZ))
					||(pAd->StaCfg.bNotFirstScan == FALSE))
				{
					DBGPRINT(RT_DEBUG_TRACE, ("STAMlmePeriodicExec():CNTL - ScanTab.BssNr==0, start a new ACTIVE scan SSID[%s]\n", pAd->MlmeAux.AutoReconnectSsid));
					if (pAd->StaCfg.BssType == BSS_ADHOC)	
						pAd->StaCfg.bNotFirstScan = TRUE;

					ScanParmFill(pAd, &ScanReq, (RTMP_STRING *) pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen, BSS_ANY, SCAN_ACTIVE);
					MlmeEnqueue(pAd, SYNC_STATE_MACHINE, MT2_MLME_SCAN_REQ, sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
					pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_LIST_SCAN;
					/* Reset Missed scan number*/
					pAd->StaCfg.LastScanTime = pAd->Mlme.Now32;
				}
				else
					MlmeAutoReconnectLastSSID(pAd);
			}
			else if (pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE
#ifdef DPA_S
				/* Reduce the scan interval in Samsung TV, due to some flash disk can't be recognized */
				&& 
				(RTMP_TIME_AFTER(pAd->Mlme.Now32, pAd->StaCfg.LastScanTime + (10 * OS_HZ))
#ifdef WIDI_SUPPORT
				/* Reduce scan interval will impact the discoveribility of ra0 L2SD based sink; if we turn on ra0's widi, we can't reduce the scan interval!*/
				||( pAd->StaCfg.bWIDI = TRUE)
#endif /* WIDI_SUPPORT */
				)
#endif /* DPA_S */
			)
			{
#ifdef CARRIER_DETECTION_SUPPORT /* Roger sync Carrier*/
				if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
				{
					if ((pAd->Mlme.OneSecPeriodicRound % 5) == 1)
						MlmeAutoReconnectLastSSID(pAd);
				}
				else
#endif /* CARRIER_DETECTION_SUPPORT */ 
				{
#ifdef WPA_SUPPLICANT_SUPPORT
					if(pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP != WPA_SUPPLICANT_ENABLE)
#endif // WPA_SUPPLICANT_SUPPORT //
						MlmeAutoReconnectLastSSID(pAd);
				}
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("pAd->StaCfg.bAutoReconnect is TRUE\n"));
#endif /* RELEASE_EXCLUDE */	
			}
		}
	}

SKIP_AUTO_SCAN_CONN:

#ifdef DOT11_N_SUPPORT
	if ((pAd->MacTab.Content[BSSID_WCID].TXBAbitmap !=0) && (pAd->MacTab.fAnyBASession == FALSE)
		&& (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF)))
	{
		pAd->MacTab.fAnyBASession = TRUE;
		AsicUpdateProtect(pAd, HT_FORCERTSCTS,  ALLN_SETPROTECT, FALSE, FALSE);
	}
	else if ((pAd->MacTab.Content[BSSID_WCID].TXBAbitmap ==0) && (pAd->MacTab.fAnyBASession == TRUE)
		&& (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF)))
	{
		pAd->MacTab.fAnyBASession = FALSE;
		AsicUpdateProtect(pAd, pAd->MlmeAux.AddHtInfo.AddHtInfo2.OperaionMode,  ALLN_SETPROTECT, FALSE, FALSE);
	}
#endif /* DOT11_N_SUPPORT */

#ifdef MESH_SUPPORT
	if(MESH_ON(pAd))
	{
		LONG idx;
		/* period update Neighbor table.*/
		NeighborTableUpdate(pAd);
		/* update Mesh Link*/
		MeshLinkTableMaintenace(pAd);
		/* update Mesh multipath entry.*/
		for (idx = 0; idx < MAX_MESH_LINKS; idx ++)
		{
			if (PeerLinkValidCheck(pAd, idx))
				MultipathEntryMaintain(pAd, idx);
		}
	}
#endif /* MESH_SUPPORT */

//YF_TODO
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) || defined(CFG80211_MULTI_STA)
#ifdef P2P_SUPPORT
	if (P2P_CLI_ON(pAd))
#else
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	if (RTMP_CFG80211_VIF_P2P_CLI_ON(pAd))
#else 
	if ( RTMP_CFG80211_MULTI_STA_ON(pAd, pAd->cfg80211_ctrl.multi_sta_net_dev))	
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */	
#endif /* P2P_SUPPORT */
	{
		if (pAd->Mlme.OneSecPeriodicRound % 2 == 0)
			ApCliIfMonitor(pAd);

		if (pAd->Mlme.OneSecPeriodicRound % 2 == 1)
			ApCliIfUp(pAd);

		ApCliSimulateRecvBeacon(pAd);
	}
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_CONCURRENT_DEVICE */

#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
	/* Perform 20/40 BSS COEX scan every Dot11BssWidthTriggerScanInt	*/
	if ((OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SCAN_2040)) && 
		(pAd->CommonCfg.Dot11BssWidthTriggerScanInt != 0) && 
		((pAd->Mlme.OneSecPeriodicRound % pAd->CommonCfg.Dot11BssWidthTriggerScanInt) == (pAd->CommonCfg.Dot11BssWidthTriggerScanInt-1))
#if defined(MT7603_FPGA) || defined(MT7628_FPGA)
		// TODO: shiang-MT7603, remove me after verification done!!
		&& (pAd->fpga_ctl.fpga_on == 0)
#endif /* MT7603_FPGA */
	)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - LastOneSecTotalTxCount/LastOneSecRxOkDataCnt  = %d/%d \n", 
					pAd->RalinkCounters.LastOneSecTotalTxCount,
					pAd->RalinkCounters.LastOneSecRxOkDataCnt));

		/*
			Check last scan time at least 30 seconds from now.
			Check traffic is less than about 1.5~2Mbps.
			it might cause data lost if we enqueue scanning.
			This criteria needs to be considered
		*/
		if ((pAd->RalinkCounters.LastOneSecTotalTxCount < 70) && (pAd->RalinkCounters.LastOneSecRxOkDataCnt < 70))
		{
			MLME_SCAN_REQ_STRUCT ScanReq;
			/* Fill out stuff for scan request and kick to scan*/
			ScanParmFill(pAd, &ScanReq, ZeroSsid, 0, BSS_ANY, SCAN_2040_BSS_COEXIST);
			MlmeEnqueue(pAd, SYNC_STATE_MACHINE, MT2_MLME_SCAN_REQ, sizeof(MLME_SCAN_REQ_STRUCT), &ScanReq, 0);
			pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_LIST_SCAN;
			/* Set InfoReq = 1, So after scan , alwats sebd 20/40 Coexistence frame to AP*/
			pAd->CommonCfg.BSSCoexist2040.field.InfoReq = 1;
			RTMP_MLME_HANDLER(pAd);
		}

		DBGPRINT(RT_DEBUG_TRACE, (" LastOneSecTotalTxCount/LastOneSecRxOkDataCnt  = %d/%d \n", 
		pAd->RalinkCounters.LastOneSecTotalTxCount, 
		pAd->RalinkCounters.LastOneSecRxOkDataCnt));	
	}
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */

	return;
}


/* Link down report*/
VOID LinkDownExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3) 
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

	if (pAd != NULL)
	{
		MLME_DISASSOC_REQ_STRUCT   DisassocReq;
		
		if ((pAd->StaCfg.wdev.PortSecured == WPA_802_1X_PORT_NOT_SECURED) &&
			(INFRA_ON(pAd)))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("LinkDownExec(): disassociate with current AP...\n"));
			DisassocParmFill(pAd, &DisassocReq, pAd->CommonCfg.Bssid, REASON_DISASSOC_STA_LEAVING);
			MlmeEnqueue(pAd, ASSOC_STATE_MACHINE, MT2_MLME_DISASSOC_REQ, 
						sizeof(MLME_DISASSOC_REQ_STRUCT), &DisassocReq, 0);
			pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_DISASSOC;

			RTMP_IndicateMediaState(pAd, NdisMediaStateDisconnected);
		    pAd->ExtraInfo = GENERAL_LINK_DOWN;
		}	
	}
}


VOID MlmeAutoScan(RTMP_ADAPTER *pAd)
{
	if (pAd->MlmeAux.AutoReconnectSsidLen > MAX_LEN_OF_SSID) {
		DBGPRINT(RT_DEBUG_ERROR,
			("%s(): invalid SsidLen(%d)\n",
			__func__, pAd->MlmeAux.AutoReconnectSsidLen));
		return;
	}

	/* check CntlMachine.CurrState to avoid collision with NDIS SetOID request*/
	if (pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - Driver auto scan\n"));
		MlmeEnqueue(pAd, 
					MLME_CNTL_STATE_MACHINE, 
					OID_802_11_BSSID_LIST_SCAN, 
					pAd->MlmeAux.AutoReconnectSsidLen, 
					pAd->MlmeAux.AutoReconnectSsid, 0);
		RTMP_MLME_HANDLER(pAd);
	}
}


VOID MlmeAutoReconnectLastSSID(RTMP_ADAPTER *pAd)
{
#ifdef WSC_STA_SUPPORT
	WSC_CTRL *pWscControl = &pAd->StaCfg.WscControl;

	if ((pWscControl->WscConfMode != WSC_DISABLE) && 
		(pWscControl->bWscTrigger) &&
		(pWscControl->WscMode == WSC_PBC_MODE) &&
		(pWscControl->WscPBCBssCount != 1))
		return;

	if ((pWscControl->WscConfMode != WSC_DISABLE) &&
		(pWscControl->WscState >= WSC_STATE_START))
	{		
		ULONG ApIdx = 0;

		ApIdx = WscSearchWpsApBySSID(pAd,
									 pWscControl->WscSsid.Ssid, 
									 pWscControl->WscSsid.SsidLength,
									 pWscControl->WscMode);

		if ((ApIdx != BSS_NOT_FOUND) &&
			(pAd->StaCfg.BssType == BSS_INFRA))
		{
			NdisMoveMemory(pWscControl->WscBssid, pAd->ScanTab.BssEntry[ApIdx].Bssid, MAC_ADDR_LEN);
			pAd->MlmeAux.Channel = pAd->ScanTab.BssEntry[ApIdx].Channel;
		}

		CntlWscIterate(pAd);
	}
	else
#endif /* WSC_STA_SUPPORT */
	if (pAd->StaCfg.bAutoConnectByBssid)
	{	
		DBGPRINT(RT_DEBUG_TRACE, ("Driver auto reconnect to last OID_802_11_BSSID setting - %02X:%02X:%02X:%02X:%02X:%02X\n",
									PRINT_MAC(pAd->MlmeAux.Bssid)));

		pAd->MlmeAux.Channel = pAd->CommonCfg.Channel;
		MlmeEnqueue(pAd,
			 MLME_CNTL_STATE_MACHINE,
			 OID_802_11_BSSID,
			 MAC_ADDR_LEN,
			 pAd->MlmeAux.Bssid, 0);

		pAd->Mlme.CntlMachine.CurrState = CNTL_IDLE;

		RTMP_MLME_HANDLER(pAd);
	}
	/* check CntlMachine.CurrState to avoid collision with NDIS SetOID request*/
	else if ((pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE) && 
		(MlmeValidateSSID(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen) == TRUE))
	{
		NDIS_802_11_SSID OidSsid;
		OidSsid.SsidLength = pAd->MlmeAux.AutoReconnectSsidLen;
		NdisMoveMemory(OidSsid.Ssid, pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);

		DBGPRINT(RT_DEBUG_TRACE, ("Driver auto reconnect to last OID_802_11_SSID setting - %s, len - %d\n",
					pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen));
		MlmeEnqueue(pAd, 
					MLME_CNTL_STATE_MACHINE, 
					OID_802_11_SSID, 
					sizeof(NDIS_802_11_SSID), 
					&OidSsid, 0);
		RTMP_MLME_HANDLER(pAd);
	}
}


/*
	==========================================================================
	Description:
		This routine checks if there're other APs out there capable for
		roaming. Caller should call this routine only when Link up in INFRA mode
		and channel quality is below CQI_GOOD_THRESHOLD.

	IRQL = DISPATCH_LEVEL

	Output:
	==========================================================================
 */
VOID MlmeCheckForRoaming(RTMP_ADAPTER *pAd, ULONG Now32)
{
	USHORT	   i;
	BSS_TABLE  *pRoamTab = &pAd->MlmeAux.RoamTab;
	BSS_ENTRY  *pBss;

	DBGPRINT(RT_DEBUG_TRACE, ("==> MlmeCheckForRoaming\n"));
	/* put all roaming candidates into RoamTab, and sort in RSSI order*/
	BssTableInit(pRoamTab);
	for (i = 0; i < pAd->ScanTab.BssNr; i++)
	{
		pBss = &pAd->ScanTab.BssEntry[i];

		if (RTMP_TIME_AFTER(Now32, pBss->LastBeaconRxTime + pAd->StaCfg.BeaconLostTime))
			continue;	 /* AP disappear*/
		if (pBss->Rssi <= RSSI_THRESHOLD_FOR_ROAMING)
			continue;	 /* RSSI too weak. forget it.*/
		if (MAC_ADDR_EQUAL(pBss->Bssid, pAd->CommonCfg.Bssid))
			continue;	 /* skip current AP*/
		if (pBss->Rssi < (pAd->StaCfg.RssiSample.LastRssi[0] + RSSI_DELTA))
			continue;	 /* only AP with stronger RSSI is eligible for roaming*/

		/* AP passing all above rules is put into roaming candidate table		 */
		NdisMoveMemory(&pRoamTab->BssEntry[pRoamTab->BssNr], pBss, sizeof(BSS_ENTRY));
		pRoamTab->BssNr += 1;
	}

	if (pRoamTab->BssNr > 0)
	{
		/* check CntlMachine.CurrState to avoid collision with NDIS SetOID request*/
		if (pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE)
		{
			pAd->RalinkCounters.PoorCQIRoamingCount ++;
			DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - Roaming attempt #%ld\n", pAd->RalinkCounters.PoorCQIRoamingCount));
			MlmeEnqueue(pAd, MLME_CNTL_STATE_MACHINE, MT2_MLME_ROAMING_REQ, 0, NULL, 0);
			RTMP_MLME_HANDLER(pAd);
		}
	}
	DBGPRINT(RT_DEBUG_TRACE, ("<== MlmeCheckForRoaming(# of candidate= %d)\n",pRoamTab->BssNr));   
}


/*
	==========================================================================
	Description:
		This routine checks if there're other APs out there capable for
		roaming. Caller should call this routine only when link up in INFRA mode
		and channel quality is below CQI_GOOD_THRESHOLD.

	IRQL = DISPATCH_LEVEL

	Output:
	==========================================================================
 */
BOOLEAN MlmeCheckForFastRoaming(RTMP_ADAPTER *pAd)
{
	USHORT		i;
	BSS_TABLE	*pRoamTab = &pAd->MlmeAux.RoamTab;
	BSS_ENTRY	*pBss;
	CHAR max_rssi;

	DBGPRINT(RT_DEBUG_TRACE, ("==> MlmeCheckForFastRoaming\n"));
	/* put all roaming candidates into RoamTab, and sort in RSSI order*/
	BssTableInit(pRoamTab);
	for (i = 0; i < pAd->ScanTab.BssNr; i++)
	{
		pBss = &pAd->ScanTab.BssEntry[i];

        if ((pBss->Rssi <= -50) && (pBss->Channel == pAd->CommonCfg.Channel))
			continue;	 /* RSSI too weak. forget it.*/
		if (MAC_ADDR_EQUAL(pBss->Bssid, pAd->CommonCfg.Bssid))
			continue;	 /* skip current AP*/
		if (!SSID_EQUAL(pBss->Ssid, pBss->SsidLen, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen))
			continue;	 /* skip different SSID*/
		max_rssi = RTMPMaxRssi(pAd, pAd->StaCfg.RssiSample.LastRssi[0], pAd->StaCfg.RssiSample.LastRssi[1], pAd->StaCfg.RssiSample.LastRssi[2]);
        if (pBss->Rssi < (max_rssi + RSSI_DELTA)) 
			continue;	 /* skip AP without better RSSI*/
		
        DBGPRINT(RT_DEBUG_TRACE, ("max_rssi = %d, pBss->Rssi = %d\n", max_rssi, pBss->Rssi));
		/* AP passing all above rules is put into roaming candidate table		 */
		NdisMoveMemory(&pRoamTab->BssEntry[pRoamTab->BssNr], pBss, sizeof(BSS_ENTRY));
		pRoamTab->BssNr += 1;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<== MlmeCheckForFastRoaming (BssNr=%d)\n", pRoamTab->BssNr));
	if (pRoamTab->BssNr > 0)
	{
		/* check CntlMachine.CurrState to avoid collision with NDIS SetOID request*/
		if (pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE)
		{
			pAd->RalinkCounters.PoorCQIRoamingCount ++;
			DBGPRINT(RT_DEBUG_TRACE, ("MMCHK - Roaming attempt #%ld\n", pAd->RalinkCounters.PoorCQIRoamingCount));
			MlmeEnqueue(pAd, MLME_CNTL_STATE_MACHINE, MT2_MLME_ROAMING_REQ, 0, NULL, 0);
			RTMP_MLME_HANDLER(pAd);
			return TRUE;
		}
	}

	return FALSE;
}


/*
	==========================================================================
	Description:
		This routine is executed periodically inside MlmePeriodicExec() after 
		association with an AP.
		It checks if StaCfg.PwrMgmt.Psm is consistent with user policy (recorded in
		StaCfg.WindowsPowerMode). If not, enforce user policy. However, 
		there're some conditions to consider:
		1. we don't support power-saving in ADHOC mode, so Psm=PWR_ACTIVE all
		   the time when Mibss==TRUE
		2. When link up in INFRA mode, Psm should not be switch to PWR_SAVE
		   if outgoing traffic available in TxRing or MgmtRing.
	Output:
		1. change pAd->StaCfg.Psm to PWR_SAVE or leave it untouched

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID MlmeCheckPsmChange(RTMP_ADAPTER *pAd, ULONG Now32)
{
	ULONG PowerMode;

	/*
		condition -
		1. Psm maybe ON only happen in INFRASTRUCTURE mode
		2. user wants either MAX_PSP or FAST_PSP
		3. but current psm is not in PWR_SAVE
		4. CNTL state machine is not doing SCANning
		5. no TX SUCCESS event for the past 1-sec period
	*/
	PowerMode = pAd->StaCfg.WindowsPowerMode;

	if (PowerMode != Ndis802_11PowerModeCAM)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s:: (%d, %d, %d, %d, %d)\n", 
									__FUNCTION__,
									INFRA_ON(pAd),
									(PowerMode != Ndis802_11PowerModeCAM),
									(pAd->StaCfg.PwrMgmt.Psm),
									(pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE),
									(pAd->CountDowntoPsm == 0)));
	}

	if (INFRA_ON(pAd) &&
		(PowerMode != Ndis802_11PowerModeCAM) &&
#ifdef STA_LP_PHASE_1_SUPPORT
		/* In Legacy_PSP, Psm is always PWR_ACTIVE, if driver wakes up H/W and H/W needs to enter sleep mode, 
		driver needs to bypass this condition OR H/W can not enter sleep again */
#else
		(pAd->StaCfg.PwrMgmt.Psm == PWR_ACTIVE) &&
#endif /* STA_LP_PHASE_1_SUPPORT */		
/*		(! RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))*/
		(pAd->Mlme.CntlMachine.CurrState == CNTL_IDLE)
#ifdef PCIE_PS_SUPPORT
		&& RTMP_TEST_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP)
#endif /* PCIE_PS_SUPPORT */
		&& (pAd->CountDowntoPsm == 0)
		 /*&&
		(pAd->RalinkCounters.OneSecTxNoRetryOkCount == 0) &&
		(pAd->RalinkCounters.OneSecTxRetryOkCount == 0)*/)
	{
		NdisGetSystemUpTime(&pAd->Mlme.LastSendNULLpsmTime);
		pAd->RalinkCounters.RxCountSinceLastNULL = 0;

#ifdef CONFIG_PM_BIT_HW_MODE
		//CmdExtPwrMgtBitWifi(pAd, BSSID_WCID, PWR_SAVE);
#endif /* CONFIG_PM_BIT_HW_MODE */
		
		RTMP_SET_PSM_BIT(pAd, PWR_SAVE);

#if 0
		if (!(pAd->StaCfg.wdev.UapsdInfo.bAPSDCapable && pAd->CommonCfg.APEdcaParm.bAPSDCapable))
		{
			RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate, (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED) ? TRUE:FALSE), pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.PwrMgmt.Psm);
		}
		else
		{
			RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate, TRUE, pAd->CommonCfg.bAPSDForcePowerSave ? PWR_SAVE : pAd->StaCfg.PwrMgmt.Psm);
		}
#else
		DBGPRINT(RT_DEBUG_TRACE, ("%s:: This is enter PSM NULL\n", __FUNCTION__));
		RTMPSetEnterPsmNullBit(&pAd->StaCfg.PwrMgmt);
		DBGPRINT(RT_DEBUG_TRACE, ("%s::bEnterPsmNull(%d)\n", __FUNCTION__, pAd->StaCfg.PwrMgmt.bEnterPsmNull));
		RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate, TRUE, PWR_SAVE);
#endif

#ifdef WMM_ACM_SUPPORT
		/*
			Can not limit retry count for PS mode;
			Or in WMM ACM UAPSD test, test will fail if any packet is lost.
		*/
		ACMP_RetryCountCtrl(pAd);
#endif /* WMM_ACM_SUPPORT */
	}
}


VOID MlmeSetPsmBit(RTMP_ADAPTER *pAd, USHORT psm)
{
#if 0
	AUTO_RSP_CFG_STRUC csr4;
#endif
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
	USHORT PsmOld = pAd->StaCfg.PwrMgmt.Psm;
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */

	pAd->StaCfg.PwrMgmt.Psm = psm;	  

#ifdef DOT11Z_TDLS_SUPPORT
	/* pAd->StaCfg.PwrMgmt.Psm must be updated before calling the function */
	TDLS_UAPSDP_PsmModeChange(pAd, PsmOld, psm);
#endif /* DOT11Z_TDLS_SUPPORT */
#ifdef CFG_TDLS_SUPPORT
#ifdef UAPSD_SUPPORT
	cfg_tdls_UAPSDP_PsmModeChange(pAd, PsmOld, psm);
#endif /*UAPSD_SUPPORT*/
#endif /* CFG_TDLS_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("MlmeSetPsmBit = %d\n", psm));
}
#endif /* CONFIG_STA_SUPPORT */

VOID RTMPSetEnterPsmNullBit(PPWR_MGMT_STRUCT pPwrMgmt)
{
	//pAd->StaCfg.PwrMgmt.bEnterPsmNull = TRUE;
	pPwrMgmt->bEnterPsmNull = TRUE;	
}

VOID RTMPClearEnterPsmNullBit(PPWR_MGMT_STRUCT pPwrMgmt)
{
	//pAd->StaCfg.PwrMgmt.bEnterPsmNull = FALSE;
	pPwrMgmt->bEnterPsmNull = FALSE;
}

BOOLEAN RTMPEnterPsmNullBitStatus(PPWR_MGMT_STRUCT pPwrMgmt)
{
	//return (pAd->StaCfg.PwrMgmt.bEnterPsmNull);
	return (pPwrMgmt->bEnterPsmNull);
}


/*
	==========================================================================
	Description:
		This routine calculates TxPER, RxPER of the past N-sec period. And 
		according to the calculation result, ChannelQuality is calculated here 
		to decide if current AP is still doing the job. 

		If ChannelQuality is not good, a ROAMing attempt may be tried later.
	Output:
		StaCfg.ChannelQuality - 0..100

	IRQL = DISPATCH_LEVEL

	NOTE: This routine decide channle quality based on RX CRC error ratio.
		Caller should make sure a function call to NICUpdateRawCounters(pAd)
		is performed right before this routine, so that this routine can decide
		channel quality based on the most up-to-date information
	==========================================================================
 */
VOID MlmeCalculateChannelQuality(
	IN PRTMP_ADAPTER pAd,
	IN PMAC_TABLE_ENTRY pMacEntry,
	IN ULONG Now32)
{
	ULONG TxOkCnt, TxCnt/*, TxPER*/, TxPRR;
	ULONG RxCnt, RxPER;
	UCHAR NorRssi;
	CHAR  MaxRssi;
	RSSI_SAMPLE *pRssiSample = NULL;
	UINT32 OneSecTxNoRetryOkCount = 0;
	UINT32 OneSecTxRetryOkCount = 0;
	UINT32 OneSecTxFailCount = 0;
	UINT32 OneSecRxOkCnt = 0;
	UINT32 OneSecRxFcsErrCnt = 0;
	ULONG ChannelQuality = 0;  /* 0..100, Channel Quality Indication for Roaming*/
#ifdef CONFIG_STA_SUPPORT
	ULONG LastBeaconRxTime = 0;
	ULONG BeaconLostTime = pAd->StaCfg.BeaconLostTime;
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT /* Roger sync Carrier*/
	/* longer beacon lost time when carrier detection enabled*/
	if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
	{
		BeaconLostTime = pAd->StaCfg.BeaconLostTime + (pAd->StaCfg.BeaconLostTime/2);
	}
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef APCLI_SUPPORT
		if (pMacEntry && IS_ENTRY_APCLI(pMacEntry) && (pMacEntry->func_tb_idx < MAX_APCLI_NUM))
			LastBeaconRxTime = pAd->ApCfg.ApCliTab[pMacEntry->func_tb_idx].ApCliRcvBeaconTime; 
		else
#endif /*APCLI_SUPPORT*/
			LastBeaconRxTime = pAd->StaCfg.LastBeaconRxTime;
#endif /* CONFIG_STA_SUPPORT */


		if (pMacEntry != NULL)
		{
			pRssiSample = &pMacEntry->RssiSample;
			OneSecTxNoRetryOkCount = pMacEntry->OneSecTxNoRetryOkCount;
			OneSecTxRetryOkCount = pMacEntry->OneSecTxRetryOkCount;
			OneSecTxFailCount = pMacEntry->OneSecTxFailCount;
			OneSecRxOkCnt = pAd->RalinkCounters.OneSecRxOkCnt;
			OneSecRxFcsErrCnt = pAd->RalinkCounters.OneSecRxFcsErrCnt;
		}
		else
		{
			pRssiSample = &pAd->MacTab.Content[0].RssiSample;
			OneSecTxNoRetryOkCount = pAd->RalinkCounters.OneSecTxNoRetryOkCount;
			OneSecTxRetryOkCount = pAd->RalinkCounters.OneSecTxRetryOkCount;
			OneSecTxFailCount = pAd->RalinkCounters.OneSecTxFailCount;
			OneSecRxOkCnt = pAd->RalinkCounters.OneSecRxOkCnt;
			OneSecRxFcsErrCnt = pAd->RalinkCounters.OneSecRxFcsErrCnt;
		}

	if (pRssiSample == NULL)
		return;
	MaxRssi = RTMPMaxRssi(pAd, pRssiSample->LastRssi[0],
								pRssiSample->LastRssi[1],
								pRssiSample->LastRssi[2]);

	
	/*
		calculate TX packet error ratio and TX retry ratio - if too few TX samples, 
		skip TX related statistics
	*/	
	TxOkCnt = OneSecTxNoRetryOkCount + OneSecTxRetryOkCount;
	TxCnt = TxOkCnt + OneSecTxFailCount;
	if (TxCnt < 5) 
	{
		//TxPER = 0;
		TxPRR = 0;
	}
	else 
	{
		//TxPER = (OneSecTxFailCount * 100) / TxCnt; 
		TxPRR = ((TxCnt - OneSecTxNoRetryOkCount) * 100) / TxCnt;
	}

	
	/* calculate RX PER - don't take RxPER into consideration if too few sample*/
	RxCnt = OneSecRxOkCnt + OneSecRxFcsErrCnt;
	if (RxCnt < 5)
		RxPER = 0;	
	else
		RxPER = (OneSecRxFcsErrCnt * 100) / RxCnt;

#if (defined(STA_LP_PHASE_1_SUPPORT) || defined(STA_LP_PHASE_2_SUPPORT)) && defined(CONFIG_STA_SUPPORT)
	if (INFRA_ON(pAd) && pMacEntry && (pMacEntry->wcid == BSSID_WCID) && 
		(pAd->StaCfg.PwrMgmt.bBeaconLost) && 
		OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
	{
		pAd->Mlme.ChannelQuality = 0;
		DBGPRINT(RT_DEBUG_TRACE, ("%s::MT7636 BEACON lost meet\n", __FUNCTION__)); 
	}
	else
#endif		
	{	
	/* decide ChannelQuality based on: 1)last BEACON received time, 2)last RSSI, 3)TxPER, and 4)RxPER*/
#ifdef CONFIG_STA_SUPPORT
	if ((pAd->OpMode == OPMODE_STA) &&
#if (defined(STA_LP_PHASE_1_SUPPORT) ||defined(STA_LP_PHASE_2_SUPPORT)) && defined(CONFIG_STA_SUPPORT)
		    (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE)) &&
#endif			
		INFRA_ON(pAd) && 
		(OneSecTxNoRetryOkCount < 2) && /* no heavy traffic*/
		RTMP_TIME_AFTER(Now32, LastBeaconRxTime + BeaconLostTime))
	{
#ifdef RELEASE_EXCLUDE
		/*
			Bug history:
			1. Samsung 2009/07/02
				Sometimes beacon will lost and re-connect to the AP.
				They use the channel 161 for RT3572 to do test.
				Some channels are bad for RT3572.
		*/
#endif /* RELEASE_EXCLUDE */
		DBGPRINT(RT_DEBUG_OFF, ("BEACON lost > %ld msec with TxOkCnt=%ld -> CQI=0\n", BeaconLostTime * (1000 / OS_HZ) , TxOkCnt)); 
		ChannelQuality = 0;
	}
	else
#endif /* CONFIG_STA_SUPPORT */
	{
		/* Normalize Rssi*/
		if (MaxRssi > -40)
			NorRssi = 100;
		else if (MaxRssi < -90)
			NorRssi = 0;
		else
			NorRssi = (MaxRssi + 90) * 2;
		
		/* ChannelQuality = W1*RSSI + W2*TxPRR + W3*RxPER	 (RSSI 0..100), (TxPER 100..0), (RxPER 100..0)*/
		ChannelQuality = (RSSI_WEIGHTING * NorRssi + 
								   TX_WEIGHTING * (100 - TxPRR) + 
								   RX_WEIGHTING* (100 - RxPER)) / 100;
	}
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("MMCHK - CQI= %ld (Tx Fail=%u/Retry=%u/Total=%lu, Rx Fail=%u/Total=%lu, RSSI=%d dbm)\n", 
		ChannelQuality,
		OneSecTxFailCount,
		OneSecTxRetryOkCount,
		TxCnt,
		OneSecRxFcsErrCnt,
		RxCnt, MaxRssi));
#endif /* RELEASE_EXCLUDE */
	}


#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
		pAd->Mlme.ChannelQuality = (ChannelQuality > 100) ? 100 : ChannelQuality;
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
	{
		if (pMacEntry != NULL)
			pMacEntry->ChannelQuality = (ChannelQuality > 100) ? 100 : ChannelQuality;
	}
#endif /* CONFIG_AP_SUPPORT */

	
}


VOID MlmeSetTxPreamble(RTMP_ADAPTER *pAd, USHORT TxPreamble)
{
	/* Always use Long preamble before verifiation short preamble functionality works well.*/
	/* Todo: remove the following line if short preamble functionality works*/

	if (TxPreamble == Rt802_11PreambleLong)
	{
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED); 
	}
	else
	{
		/* NOTE: 1Mbps should always use long preamble*/
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);
	}
	DBGPRINT(RT_DEBUG_TRACE, ("MlmeSetTxPreamble = %s PREAMBLE\n",
				((TxPreamble == Rt802_11PreambleLong) ? "LONG" : "SHORT")));
			
	AsicSetTxPreamble(pAd, TxPreamble);
}


/*
    ==========================================================================
    Description:
        Update basic rate bitmap
    ==========================================================================
*/
VOID UpdateBasicRateBitmap(RTMP_ADAPTER *pAdapter)
{
    INT  i, j;
                  /* 1  2  5.5, 11,  6,  9, 12, 18, 24, 36, 48,  54 */
    UCHAR rate[] = { 2, 4,  11, 22, 12, 18, 24, 36, 48, 72, 96, 108 };
    UCHAR *sup_p = pAdapter->CommonCfg.SupRate;
    UCHAR *ext_p = pAdapter->CommonCfg.ExtRate;
    ULONG bitmap = pAdapter->CommonCfg.BasicRateBitmap;

    /* if A mode, always use fix BasicRateBitMap */
    /*if (pAdapter->CommonCfg.Channel == WMODE_A)*/
	if (pAdapter->CommonCfg.Channel > 14)
	{
		if (pAdapter->CommonCfg.BasicRateBitmap & 0xF)
		{
			/* no 11b rate in 5G band */
			pAdapter->CommonCfg.BasicRateBitmapOld = \
										pAdapter->CommonCfg.BasicRateBitmap;
			pAdapter->CommonCfg.BasicRateBitmap &= (~0xF); /* no 11b */
		}

		/* force to 6,12,24M in a-band */
		pAdapter->CommonCfg.BasicRateBitmap |= 0x150; /* 6, 12, 24M */
    }
	else
	{
		/* no need to modify in 2.4G (bg mixed) */
		pAdapter->CommonCfg.BasicRateBitmap = \
										pAdapter->CommonCfg.BasicRateBitmapOld;
	}

    if (pAdapter->CommonCfg.BasicRateBitmap > 4095)
    {
        /* (2 ^ MAX_LEN_OF_SUPPORTED_RATES) -1 */
        return;
    }

    for(i=0; i<MAX_LEN_OF_SUPPORTED_RATES; i++)
    {
        sup_p[i] &= 0x7f;
        ext_p[i] &= 0x7f;
    }

    for(i=0; i<MAX_LEN_OF_SUPPORTED_RATES; i++)
    {
        if (bitmap & (1 << i))
        {
            for(j=0; j<MAX_LEN_OF_SUPPORTED_RATES; j++)
            {
                if (sup_p[j] == rate[i])
                    sup_p[j] |= 0x80;
            }

            for(j=0; j<MAX_LEN_OF_SUPPORTED_RATES; j++)
            {
                if (ext_p[j] == rate[i])
                    ext_p[j] |= 0x80;
            }
        }
    }
}


/*
	bLinkUp is to identify the inital link speed.
	TRUE indicates the rate update at linkup, we should not try to set the rate at 54Mbps.
*/
VOID MlmeUpdateTxRates(RTMP_ADAPTER *pAd, BOOLEAN bLinkUp, UCHAR apidx)
{
	int i, num;
	UCHAR Rate = RATE_6, MaxDesire = RATE_1, MaxSupport = RATE_1;
	UCHAR MinSupport = RATE_54;
	ULONG BasicRateBitmap = 0;
	UCHAR CurrBasicRate = RATE_1;
	UCHAR *pSupRate, SupRateLen, *pExtRate, ExtRateLen;
	HTTRANSMIT_SETTING *pHtPhy = NULL, *pMaxHtPhy = NULL, *pMinHtPhy = NULL;
	BOOLEAN *auto_rate_cur_p;
	UCHAR HtMcs = MCS_AUTO;
	struct wifi_dev *wdev = NULL;


	/* find max desired rate*/
	UpdateBasicRateBitmap(pAd);
	
	num = 0;
	auto_rate_cur_p = NULL;
	for (i=0; i<MAX_LEN_OF_SUPPORTED_RATES; i++)
	{
		switch (pAd->CommonCfg.DesireRate[i] & 0x7f)
		{
			case 2:  Rate = RATE_1;   num++;   break;
			case 4:  Rate = RATE_2;   num++;   break;
			case 11: Rate = RATE_5_5; num++;   break;
			case 22: Rate = RATE_11;  num++;   break;
			case 12: Rate = RATE_6;   num++;   break;
			case 18: Rate = RATE_9;   num++;   break;
			case 24: Rate = RATE_12;  num++;   break;
			case 36: Rate = RATE_18;  num++;   break;
			case 48: Rate = RATE_24;  num++;   break;
			case 72: Rate = RATE_36;  num++;   break;
			case 96: Rate = RATE_48;  num++;   break;
			case 108: Rate = RATE_54; num++;   break;
			/*default: Rate = RATE_1;   break;*/
		}
		if (MaxDesire < Rate)  MaxDesire = Rate;
	}

/*===========================================================================*/
	do
	{
#ifdef CONFIG_AP_SUPPORT
#ifdef RT_CFG80211_P2P_SUPPORT
		if (apidx >= MIN_NET_DEVICE_FOR_CFG80211_VIF_P2P_GO)
		{
			DBGPRINT(RT_DEBUG_INFO, ("%s: Update for GO\n", __func__));
			wdev = &pAd->ApCfg.MBSSID[CFG_GO_BSSID_IDX].wdev;
			break;
		}
#endif /* RT_CFG80211_P2P_SUPPORT */
#ifdef P2P_SUPPORT
		if (apidx >= MIN_NET_DEVICE_FOR_P2P_GO)
		{	
			UCHAR idx = apidx - MIN_NET_DEVICE_FOR_P2P_GO;
			wdev = &pAd->ApCfg.MBSSID[idx].wdev;
			break;
		}
#endif /* P2P_SUPPORT */
#ifdef MESH_SUPPORT	
		if (apidx >= MIN_NET_DEVICE_FOR_MESH)
		{
			wdev = &pAd->MeshTab.wdev;
			break;
		}
#endif /* MESH_SUPPORT */
#ifdef APCLI_SUPPORT	
		if (apidx >= MIN_NET_DEVICE_FOR_APCLI)
		{			
			UCHAR idx = apidx - MIN_NET_DEVICE_FOR_APCLI;
			
			if (idx < MAX_APCLI_NUM)
			{
				wdev = &pAd->ApCfg.ApCliTab[idx].wdev;
				break;
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): invalid idx(%d)\n", __FUNCTION__, idx));
				return;
			}
		}
#endif /* APCLI_SUPPORT */
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef WDS_SUPPORT
			if (apidx >= MIN_NET_DEVICE_FOR_WDS)
			{
				UCHAR idx = apidx - MIN_NET_DEVICE_FOR_WDS;

				if (idx < MAX_WDS_ENTRY)
				{
					wdev = &pAd->WdsTab.WdsEntry[idx].wdev;
					break;
				}
				else
				{
					DBGPRINT(RT_DEBUG_ERROR, ("%s(): invalid apidx(%d)\n", __FUNCTION__, apidx));
					return;
				}
			}
#endif /* WDS_SUPPORT */

			if ((apidx < pAd->ApCfg.BssidNum) &&
				(apidx < MAX_MBSSID_NUM(pAd)) &&
				(apidx < HW_BEACON_MAX_NUM))
			{
				wdev = &pAd->ApCfg.MBSSID[apidx].wdev;
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): invalid apidx(%d)\n", __FUNCTION__, apidx));
			}
			break;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			wdev = &pAd->StaCfg.wdev;
			if ((pAd->StaCfg.BssType == BSS_ADHOC) &&
				WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B) &&
				(MaxDesire > RATE_11))
			{
				MaxDesire = RATE_11;
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO,("%s():. 2 MaxDesire = %d. OpMode=%d\n", __FUNCTION__, MaxDesire, pAd->OpMode));
#endif /* RELEASE_EXCLUDE */
			}
			break;
		}
#endif /* CONFIG_STA_SUPPORT */
	} while(FALSE);

	if (wdev)
	{
		pHtPhy = &wdev->HTPhyMode;
		pMaxHtPhy = &wdev->MaxHTPhyMode;
		pMinHtPhy = &wdev->MinHTPhyMode;

		auto_rate_cur_p = &wdev->bAutoTxRateSwitch;
		HtMcs = (UCHAR)wdev->DesiredTransmitSetting.field.MCS;
	}

	pAd->CommonCfg.MaxDesiredRate = MaxDesire;

	if (pMinHtPhy == NULL)
		return;
	pMinHtPhy->word = 0;
	pMaxHtPhy->word = 0;
	pHtPhy->word = 0;

	/*
		Auto rate switching is enabled only if more than one DESIRED RATES are
		specified; otherwise disabled
	*/
	if (num <= 1)
		*auto_rate_cur_p = FALSE;
	else
		*auto_rate_cur_p = TRUE;

	if (HtMcs != MCS_AUTO)
		*auto_rate_cur_p = FALSE;
	else
		*auto_rate_cur_p = TRUE;

#ifdef CONFIG_STA_SUPPORT
	if ((ADHOC_ON(pAd) || INFRA_ON(pAd)) && (pAd->OpMode == OPMODE_STA)
#ifdef P2P_SUPPORT
		&& (apidx == MIN_NET_DEVICE_FOR_MBSSID)
#endif /* P2P_SUPPORT */
		)
	{
		pSupRate = &pAd->StaActive.SupRate[0];
		pExtRate = &pAd->StaActive.ExtRate[0];
		SupRateLen = pAd->StaActive.SupRateLen;
		ExtRateLen = pAd->StaActive.ExtRateLen;
	}
	else
#endif /* CONFIG_STA_SUPPORT */	
	{
		pSupRate = &pAd->CommonCfg.SupRate[0];
		pExtRate = &pAd->CommonCfg.ExtRate[0];
		SupRateLen = pAd->CommonCfg.SupRateLen;
		ExtRateLen = pAd->CommonCfg.ExtRateLen;
	}

	/* find max supported rate */
	for (i=0; i<SupRateLen; i++)
	{
		switch (pSupRate[i] & 0x7f)
		{
			case 2:   Rate = RATE_1;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 0;	 break;
			case 4:   Rate = RATE_2;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 1;	 break;
			case 11:  Rate = RATE_5_5;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 2;	 break;
			case 22:  Rate = RATE_11;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 3;	 break;
			case 12:  Rate = RATE_6;	/*if (pSupRate[i] & 0x80)*/  BasicRateBitmap |= 1 << 4;  break;
			case 18:  Rate = RATE_9;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 5;	 break;
			case 24:  Rate = RATE_12;	/*if (pSupRate[i] & 0x80)*/  BasicRateBitmap |= 1 << 6;  break;
			case 36:  Rate = RATE_18;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 7;	 break;
			case 48:  Rate = RATE_24;	/*if (pSupRate[i] & 0x80)*/  BasicRateBitmap |= 1 << 8;  break;
			case 72:  Rate = RATE_36;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 9;	 break;
			case 96:  Rate = RATE_48;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 10;	 break;
			case 108: Rate = RATE_54;	if (pSupRate[i] & 0x80) BasicRateBitmap |= 1 << 11;	 break;
			default:  Rate = RATE_1;	break;
		}
		
		if (MaxSupport < Rate)
			MaxSupport = Rate;

		if (MinSupport > Rate)
			MinSupport = Rate;
	}
	
	for (i=0; i<ExtRateLen; i++)
	{
		switch (pExtRate[i] & 0x7f)
		{
			case 2:   Rate = RATE_1;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 0;	 break;
			case 4:   Rate = RATE_2;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 1;	 break;
			case 11:  Rate = RATE_5_5;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 2;	 break;
			case 22:  Rate = RATE_11;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 3;	 break;
			case 12:  Rate = RATE_6;	/*if (pExtRate[i] & 0x80)*/  BasicRateBitmap |= 1 << 4;  break;
			case 18:  Rate = RATE_9;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 5;	 break;
			case 24:  Rate = RATE_12;	/*if (pExtRate[i] & 0x80)*/  BasicRateBitmap |= 1 << 6;  break;
			case 36:  Rate = RATE_18;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 7;	 break;
			case 48:  Rate = RATE_24;	/*if (pExtRate[i] & 0x80)*/  BasicRateBitmap |= 1 << 8;  break;
			case 72:  Rate = RATE_36;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 9;	 break;
			case 96:  Rate = RATE_48;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 10;	 break;
			case 108: Rate = RATE_54;	if (pExtRate[i] & 0x80) BasicRateBitmap |= 1 << 11;	 break;
			default:  Rate = RATE_1;
				break;
		}
		if (MaxSupport < Rate)
			MaxSupport = Rate;

		if (MinSupport > Rate)
			MinSupport = Rate;
	}

	// TODO: shiang-7603, need to revise for MT7603!!
#if defined(MT7603) || defined(MT7628)
	if (IS_MT7603(pAd) || IS_MT7628(pAd))
		;
	else
#endif /* MT7603 */
	{
		RTMP_IO_WRITE32(pAd, LEGACY_BASIC_RATE, BasicRateBitmap);
	}

#ifdef RELEASE_EXCLUDE
	/* bug fix 
		pAd->CommonCfg.BasicRateBitmap = BasicRateBitmap;
		calculate the exptected ACK rate for each TX rate. This info is used to caculate
		the DURATION field of outgoing unicast DATA/MGMT frame
	*/
#endif /* RELEASE_EXCLUDE */
	for (i=0; i<MAX_LEN_OF_SUPPORTED_RATES; i++)
	{
		if (BasicRateBitmap & (0x01 << i))
			CurrBasicRate = (UCHAR)i;
		pAd->CommonCfg.ExpectedACKRate[i] = CurrBasicRate;
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("Exptected ACK rate[%d] = %d Mbps\n", RateIdToMbps[i], RateIdToMbps[CurrBasicRate]));
#endif /* RELEASE_EXCLUDE */
	}

	DBGPRINT(RT_DEBUG_TRACE,("%s():[MaxSupport = %d] = MaxDesire %d Mbps\n",
				__FUNCTION__, RateIdToMbps[MaxSupport], RateIdToMbps[MaxDesire]));
	/* max tx rate = min {max desire rate, max supported rate}*/
	if (MaxSupport < MaxDesire)
		pAd->CommonCfg.MaxTxRate = MaxSupport;
	else
		pAd->CommonCfg.MaxTxRate = MaxDesire;

	pAd->CommonCfg.MinTxRate = MinSupport;
	/*
		2003-07-31 john - 2500 doesn't have good sensitivity at high OFDM rates. to increase the success
		ratio of initial DHCP packet exchange, TX rate starts from a lower rate depending
		on average RSSI
			1. RSSI >= -70db, start at 54 Mbps (short distance)
			2. -70 > RSSI >= -75, start at 24 Mbps (mid distance)
			3. -75 > RSSI, start at 11 Mbps (long distance)
	*/
	if (*auto_rate_cur_p)
	{
		short dbm = 0;
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			dbm = pAd->StaCfg.RssiSample.AvgRssi[0] - pAd->BbpRssiToDbmDelta;
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			dbm =0;
#endif /* CONFIG_AP_SUPPORT */
		if (bLinkUp == TRUE)
			pAd->CommonCfg.TxRate = RATE_24;
		else
			pAd->CommonCfg.TxRate = pAd->CommonCfg.MaxTxRate; 

		if (dbm < -75)
			pAd->CommonCfg.TxRate = RATE_11;
		else if (dbm < -70)
			pAd->CommonCfg.TxRate = RATE_24;

		/* should never exceed MaxTxRate (consider 11B-only mode)*/
		if (pAd->CommonCfg.TxRate > pAd->CommonCfg.MaxTxRate)
			pAd->CommonCfg.TxRate = pAd->CommonCfg.MaxTxRate; 

		pAd->CommonCfg.TxRateIndex = 0;
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_TRACE, (" %s(): (Rssi=%d, init TX rate = %d Mbps)\n",
					__FUNCTION__, dbm, RateIdToMbps[pAd->CommonCfg.TxRate]));
#endif /* RELEASE_EXCLUDE */

#ifdef RT3593
		/*Enable AutoFallback ; RT3593 support disable Tx auto fallback per packet*/
		RT3593_AUTO_FALLBACK_ENABLE(pAd);
#endif /* RT3593 */
	}
	else
	{
		pAd->CommonCfg.TxRate = pAd->CommonCfg.MaxTxRate;

		/* Choose the Desire Tx MCS in CCK/OFDM mode */
		if (num > RATE_6)
		{
			if (HtMcs <= MCS_7)		
				MaxDesire = RxwiMCSToOfdmRate[HtMcs];
			else
				MaxDesire = MinSupport;
		}
		else
		{
			if (HtMcs <= MCS_3)		
				MaxDesire = HtMcs;
			else
				MaxDesire = MinSupport;
		}
		
		pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.STBC = pHtPhy->field.STBC;
		pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.ShortGI = pHtPhy->field.ShortGI;
		pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MCS = pHtPhy->field.MCS;
		pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE	= pHtPhy->field.MODE;

#ifdef RT3593
		/*Disable AutoFallback; RT3593 support disable Tx auto fallback per packet*/
		if (!IS_RT3593(pAd))
			AsicSetAutoFallBack(pAd, FALSE);
#endif /* RT3593 */
	}

	if (pAd->CommonCfg.TxRate <= RATE_11)
	{
		pMaxHtPhy->field.MODE = MODE_CCK;

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			pMaxHtPhy->field.MCS = pAd->CommonCfg.TxRate;
			pMinHtPhy->field.MCS = pAd->CommonCfg.MinTxRate;
		}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			pMaxHtPhy->field.MCS = MaxDesire;
		}
#endif /* CONFIG_AP_SUPPORT */	
#ifdef P2P_SUPPORT
		if (apidx >= MIN_NET_DEVICE_FOR_APCLI)
			pMaxHtPhy->field.MCS = MaxDesire;
#endif /* P2P_SUPPORT */	

	}
	else
	{
		pMaxHtPhy->field.MODE = MODE_OFDM;

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			pMaxHtPhy->field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.TxRate];
			if (pAd->CommonCfg.MinTxRate >= RATE_6 && (pAd->CommonCfg.MinTxRate <= RATE_54))
				pMinHtPhy->field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.MinTxRate];
			else
				pMinHtPhy->field.MCS = pAd->CommonCfg.MinTxRate;
		}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
			pMaxHtPhy->field.MCS = OfdmRateToRxwiMCS[MaxDesire];
		}
#endif /* CONFIG_AP_SUPPORT */				

#ifdef P2P_SUPPORT
		if (apidx >= MIN_NET_DEVICE_FOR_APCLI)
			pMaxHtPhy->field.MCS = OfdmRateToRxwiMCS[MaxDesire];
#endif /* P2P_SUPPORT */
	}

	pHtPhy->word = (pMaxHtPhy->word);
	if (bLinkUp && (pAd->OpMode == OPMODE_STA))
	{
		pAd->MacTab.Content[BSSID_WCID].HTPhyMode.word = pHtPhy->word;
		pAd->MacTab.Content[BSSID_WCID].MaxHTPhyMode.word = pMaxHtPhy->word;
		pAd->MacTab.Content[BSSID_WCID].MinHTPhyMode.word = pMinHtPhy->word;
	}
	else
	{
		if (WMODE_CAP(pAd->CommonCfg.PhyMode, WMODE_B) &&
			pAd->CommonCfg.Channel <= 14)
		{
			pAd->CommonCfg.MlmeRate = RATE_1;
			pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_CCK;
			pAd->CommonCfg.MlmeTransmit.field.MCS = RATE_1;				
			pAd->CommonCfg.RtsRate = RATE_11;
		}
		else
		{
			pAd->CommonCfg.MlmeRate = RATE_6;
			pAd->CommonCfg.RtsRate = RATE_6;
			pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;
			pAd->CommonCfg.MlmeTransmit.field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.MlmeRate];
		}
		
		/* Keep Basic Mlme Rate.*/
		pAd->MacTab.Content[MCAST_WCID].HTPhyMode.word = pAd->CommonCfg.MlmeTransmit.word;
		if (pAd->CommonCfg.MlmeTransmit.field.MODE == MODE_OFDM)
			pAd->MacTab.Content[MCAST_WCID].HTPhyMode.field.MCS = OfdmRateToRxwiMCS[RATE_24];
		else
			pAd->MacTab.Content[MCAST_WCID].HTPhyMode.field.MCS = RATE_1;
		pAd->CommonCfg.BasicMlmeRate = pAd->CommonCfg.MlmeRate;

#ifdef CONFIG_AP_SUPPORT
#ifdef MCAST_RATE_SPECIFIC
		{
			/* set default value if MCastPhyMode is not initialized */	
			HTTRANSMIT_SETTING tPhyMode;

			memset(&tPhyMode, 0, sizeof(HTTRANSMIT_SETTING));
			if (memcmp(&pAd->CommonCfg.MCastPhyMode, &tPhyMode, sizeof(HTTRANSMIT_SETTING)) == 0)
			{
				memmove(&pAd->CommonCfg.MCastPhyMode, &pAd->MacTab.Content[MCAST_WCID].HTPhyMode,
							sizeof(HTTRANSMIT_SETTING));
			}
		}
#endif /* MCAST_RATE_SPECIFIC */
#endif /* CONFIG_AP_SUPPORT */
	}

	DBGPRINT(RT_DEBUG_TRACE, (" %s(): (MaxDesire=%d, MaxSupport=%d, MaxTxRate=%d, MinRate=%d, Rate Switching =%d)\n", 
				__FUNCTION__, RateIdToMbps[MaxDesire], RateIdToMbps[MaxSupport],
				RateIdToMbps[pAd->CommonCfg.MaxTxRate],
				RateIdToMbps[pAd->CommonCfg.MinTxRate], 
			 /*OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_TX_RATE_SWITCH_ENABLED)*/*auto_rate_cur_p));
	DBGPRINT(RT_DEBUG_TRACE, (" %s(): (TxRate=%d, RtsRate=%d, BasicRateBitmap=0x%04lx)\n", 
				__FUNCTION__, RateIdToMbps[pAd->CommonCfg.TxRate],
				RateIdToMbps[pAd->CommonCfg.RtsRate], BasicRateBitmap));
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): (MlmeTransmit=0x%x, MinHTPhyMode=%x, MaxHTPhyMode=0x%x, HTPhyMode=0x%x)\n", 
				__FUNCTION__, pAd->CommonCfg.MlmeTransmit.word,
				pAd->MacTab.Content[BSSID_WCID].MinHTPhyMode.word,
				pAd->MacTab.Content[BSSID_WCID].MaxHTPhyMode.word,
				pAd->MacTab.Content[BSSID_WCID].HTPhyMode.word ));
}


#ifdef DOT11_N_SUPPORT
/*
	==========================================================================
	Description:
		This function update HT Rate setting.
		Input Wcid value is valid for 2 case :
		1. it's used for Station in infra mode that copy AP rate to Mactable.
		2. OR Station 	in adhoc mode to copy peer's HT rate to Mactable. 

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID MlmeUpdateHtTxRates(RTMP_ADAPTER *pAd, UCHAR apidx)
{
	//UCHAR StbcMcs;
	RT_HT_CAPABILITY *pRtHtCap = NULL;
	RT_PHY_INFO *pActiveHtPhy = NULL;	
	//ULONG BasicMCS;
	RT_PHY_INFO *pDesireHtPhy = NULL;
	PHTTRANSMIT_SETTING pHtPhy = NULL;
	PHTTRANSMIT_SETTING pMaxHtPhy = NULL;
	PHTTRANSMIT_SETTING pMinHtPhy = NULL;	
	BOOLEAN *auto_rate_cur_p;
	struct wifi_dev *wdev = NULL;


	DBGPRINT(RT_DEBUG_TRACE,("%s()===> \n", __FUNCTION__));

	auto_rate_cur_p = NULL;

	do
	{
#ifdef CONFIG_AP_SUPPORT
#ifdef P2P_SUPPORT
		if (apidx >= MIN_NET_DEVICE_FOR_P2P_GO)
		{
			UCHAR	idx = apidx - MIN_NET_DEVICE_FOR_P2P_GO;
			wdev = &pAd->ApCfg.MBSSID[idx].wdev;
			break;
		}
#endif /* P2P_SUPPORT */
#ifdef MESH_SUPPORT	
		if (apidx >= MIN_NET_DEVICE_FOR_MESH)
		{
			wdev = &pAd->MeshTab.wdev;
			break;
		}
#endif /* MESH_SUPPORT */

#ifdef APCLI_SUPPORT	
		if (apidx >= MIN_NET_DEVICE_FOR_APCLI)
		{
			UCHAR	idx = apidx - MIN_NET_DEVICE_FOR_APCLI;
		
			if (idx < MAX_APCLI_NUM)
			{
				wdev = &pAd->ApCfg.ApCliTab[idx].wdev;
				break;
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): invalid idx(%d)\n", __FUNCTION__, idx));
				return;
			}
		}
#endif /* APCLI_SUPPORT */
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
#ifdef WDS_SUPPORT
			if (apidx >= MIN_NET_DEVICE_FOR_WDS)
			{
				UCHAR	idx = apidx - MIN_NET_DEVICE_FOR_WDS;

			if (idx < MAX_WDS_ENTRY)
			{
				wdev = &pAd->WdsTab.WdsEntry[idx].wdev;
				break;
			}
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): invalid apidx(%d)\n", __FUNCTION__, apidx));
				return;
			}
			}
#endif /* WDS_SUPPORT */

			if ((apidx < pAd->ApCfg.BssidNum) && (apidx < HW_BEACON_MAX_NUM))
				wdev = &pAd->ApCfg.MBSSID[apidx].wdev;
			else
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): invalid apidx(%d)\n", __FUNCTION__, apidx));
			}
			break;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			wdev = &pAd->StaCfg.wdev;
			break;
		}		
#endif /* CONFIG_STA_SUPPORT */
	} while (FALSE);

	if (wdev)
	{
		pDesireHtPhy = &wdev->DesiredHtPhyInfo;
		pActiveHtPhy = &wdev->DesiredHtPhyInfo;
		pHtPhy = &wdev->HTPhyMode;
		pMaxHtPhy = &wdev->MaxHTPhyMode;
		pMinHtPhy = &wdev->MinHTPhyMode;		

		auto_rate_cur_p = &wdev->bAutoTxRateSwitch;
	}

#ifdef CONFIG_STA_SUPPORT	
	if ((ADHOC_ON(pAd) || INFRA_ON(pAd)) && (pAd->OpMode == OPMODE_STA)
#ifdef P2P_SUPPORT
		&& (apidx == BSS0)
#endif /* P2P_SUPPORT */
		)
	{
		if (pAd->StaActive.SupportedPhyInfo.bHtEnable == FALSE)
			return;

		pRtHtCap = &pAd->StaActive.SupportedHtPhy;
		pActiveHtPhy = &pAd->StaActive.SupportedPhyInfo;
		//StbcMcs = (UCHAR)pAd->MlmeAux.AddHtInfo.AddHtInfo3.StbcMcs;
		//BasicMCS =pAd->MlmeAux.AddHtInfo.MCSSet[0]+(pAd->MlmeAux.AddHtInfo.MCSSet[1]<<8)+(StbcMcs<<16);
		if ((pAd->CommonCfg.DesiredHtPhy.TxSTBC) && (pRtHtCap->RxSTBC) && (pAd->Antenna.field.TxPath == 2))
			pMaxHtPhy->field.STBC = STBC_USE;
		else
			pMaxHtPhy->field.STBC = STBC_NONE;
	}
	else
#endif /* CONFIG_STA_SUPPORT */
	{
		if ((!pDesireHtPhy) || pDesireHtPhy->bHtEnable == FALSE)
			return;

		pRtHtCap = &pAd->CommonCfg.DesiredHtPhy;
		//StbcMcs = (UCHAR)pAd->CommonCfg.AddHTInfo.AddHtInfo3.StbcMcs;
		//BasicMCS = pAd->CommonCfg.AddHTInfo.MCSSet[0]+(pAd->CommonCfg.AddHTInfo.MCSSet[1]<<8)+(StbcMcs<<16);
		if ((pAd->CommonCfg.DesiredHtPhy.TxSTBC) && (pAd->Antenna.field.TxPath >= 2))
			pMaxHtPhy->field.STBC = STBC_USE;
		else
			pMaxHtPhy->field.STBC = STBC_NONE;
	}

	/* Decide MAX ht rate.*/
	if ((pRtHtCap->GF) && (pAd->CommonCfg.DesiredHtPhy.GF))
		pMaxHtPhy->field.MODE = MODE_HTGREENFIELD;
	else
		pMaxHtPhy->field.MODE = MODE_HTMIX;

    if ((pAd->CommonCfg.DesiredHtPhy.ChannelWidth) && (pRtHtCap->ChannelWidth))
		pMaxHtPhy->field.BW = BW_40;
	else
		pMaxHtPhy->field.BW = BW_20;

    if (pMaxHtPhy->field.BW == BW_20)
		pMaxHtPhy->field.ShortGI = (pAd->CommonCfg.DesiredHtPhy.ShortGIfor20 & pRtHtCap->ShortGIfor20);
	else
		pMaxHtPhy->field.ShortGI = (pAd->CommonCfg.DesiredHtPhy.ShortGIfor40 & pRtHtCap->ShortGIfor40);

	if (pDesireHtPhy->MCSSet[4] != 0)
	{
		pMaxHtPhy->field.MCS = 32;	
	}

	pMaxHtPhy->field.MCS = (USHORT)get_ht_max_mcs(pAd, &pDesireHtPhy->MCSSet[0],
											&pActiveHtPhy->MCSSet[0]);

	/* Copy MIN ht rate.  rt2860???*/
	pMinHtPhy->field.BW = BW_20;
	pMinHtPhy->field.MCS = 0;
	pMinHtPhy->field.STBC = 0;
	pMinHtPhy->field.ShortGI = 0;
	/*If STA assigns fixed rate. update to fixed here.*/
#ifdef CONFIG_STA_SUPPORT
	if ( (pAd->OpMode == OPMODE_STA) && (pDesireHtPhy->MCSSet[0] != 0xff)
#ifdef P2P_SUPPORT
		&& (apidx == BSS0)
#endif /* P2P_SUPPORT */
		)
	{
		CHAR i;
		UCHAR j, bitmask;
		
		if (pDesireHtPhy->MCSSet[4] != 0)
		{
			pMaxHtPhy->field.MCS = 32;
			pMinHtPhy->field.MCS = 32;
			DBGPRINT(RT_DEBUG_TRACE,("%s():<=== Use Fixed MCS = %d\n",__FUNCTION__, pMinHtPhy->field.MCS));
		}

		for (i=23; (CHAR)i >= 0; i--)
		{	
			j = i/8;	
			bitmask = (1<<(i-(j*8)));
			if ( (pDesireHtPhy->MCSSet[j] & bitmask) && (pActiveHtPhy->MCSSet[j] & bitmask))
			{
				pMaxHtPhy->field.MCS = i;
				pMinHtPhy->field.MCS = i;
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO,("%s():<=== Use Fixed MCS = %d\n", __FUNCTION__, i));
#endif /* RELEASE_EXCLUDE */
				break;
			}
			if (i==0)
				break;
		}
	}
#endif /* CONFIG_STA_SUPPORT */
	
	
	/* Decide ht rate*/
	pHtPhy->field.STBC = pMaxHtPhy->field.STBC;
	pHtPhy->field.BW = pMaxHtPhy->field.BW;
	pHtPhy->field.MODE = pMaxHtPhy->field.MODE;
	pHtPhy->field.MCS = pMaxHtPhy->field.MCS;
	pHtPhy->field.ShortGI = pMaxHtPhy->field.ShortGI;

	/* use default now. rt2860*/
	if (pDesireHtPhy->MCSSet[0] != 0xff)
		*auto_rate_cur_p = FALSE;
	else
		*auto_rate_cur_p = TRUE;
	
	DBGPRINT(RT_DEBUG_TRACE, (" %s():<---.AMsduSize = %d  \n", __FUNCTION__, pAd->CommonCfg.DesiredHtPhy.AmsduSize ));
	DBGPRINT(RT_DEBUG_TRACE,("TX: MCS[0] = %x (choose %d), BW = %d, ShortGI = %d, MODE = %d,  \n", pActiveHtPhy->MCSSet[0],pHtPhy->field.MCS,
		pHtPhy->field.BW, pHtPhy->field.ShortGI, pHtPhy->field.MODE));
	DBGPRINT(RT_DEBUG_TRACE,("%s():<=== \n", __FUNCTION__));
}


VOID BATableInit(RTMP_ADAPTER *pAd, BA_TABLE *Tab) 
{
	int i;

	Tab->numAsOriginator = 0;
	Tab->numAsRecipient = 0;
	Tab->numDoneOriginator = 0;
	NdisAllocateSpinLock(pAd, &pAd->BATabLock);
	for (i = 0; i < MAX_LEN_OF_BA_REC_TABLE; i++) 
	{
		Tab->BARecEntry[i].REC_BA_Status = Recipient_NONE;
		NdisAllocateSpinLock(pAd, &(Tab->BARecEntry[i].RxReRingLock));
	}
	for (i = 0; i < MAX_LEN_OF_BA_ORI_TABLE; i++) 
	{
		Tab->BAOriEntry[i].ORI_BA_Status = Originator_NONE;
	}
}


VOID BATableExit(RTMP_ADAPTER *pAd)
{
	int i;
	
	for(i=0; i<MAX_LEN_OF_BA_REC_TABLE; i++)
	{
		NdisFreeSpinLock(&pAd->BATable.BARecEntry[i].RxReRingLock);
	}
	NdisFreeSpinLock(&pAd->BATabLock);
}
#endif /* DOT11_N_SUPPORT */


VOID MlmeRadioOff(RTMP_ADAPTER *pAd)
{
#ifdef LED_CONTROL_SUPPORT
	RTMPSetLED(pAd, LED_RADIO_OFF);
#endif /* LED_CONTROL_SUPPORT */

#ifdef RTMP_MAC_PCI
	if (IS_PCI_INF(pAd) || IS_RBUS_INF(pAd))
		PciMlmeRadioOFF(pAd);
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
		UsbMlmeRadioOFF(pAd);
#endif /* RTMP_MAC_USB */
}


VOID MlmeRadioOn(RTMP_ADAPTER *pAd)
{
#ifdef RTMP_MAC_PCI
	if (IS_PCI_INF(pAd) || IS_RBUS_INF(pAd))
		PciMlmeRadioOn(pAd);
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd))
		UsbMlmeRadioOn(pAd);
#endif /* RTMP_MAC_USB */
#ifdef LED_CONTROL_SUPPORT
	RTMPSetLED(pAd, LED_LINK_UP);
#endif /* LED_CONTROL_SUPPORT */

}


/*! \brief initialize BSS table
 *	\param p_tab pointer to the table
 *	\return none
 *	\pre
 *	\post

 IRQL = PASSIVE_LEVEL
 IRQL = DISPATCH_LEVEL
  
 */
VOID BssTableInit(BSS_TABLE *Tab) 
{
	int i;

	Tab->BssNr = 0;
	Tab->BssOverlapNr = 0;

	for (i = 0; i < MAX_LEN_OF_BSS_TABLE; i++) 
	{
		UCHAR *pOldAddr = Tab->BssEntry[i].pVarIeFromProbRsp;

		NdisZeroMemory(&Tab->BssEntry[i], sizeof(BSS_ENTRY));

		Tab->BssEntry[i].Rssi = -127;	/* initial the rssi as a minimum value */
		if (pOldAddr)
		{
			RTMPZeroMemory(pOldAddr, MAX_VIE_LEN);
			Tab->BssEntry[i].pVarIeFromProbRsp = pOldAddr;
		}
	}
}


/*! \brief search the BSS table by SSID
 *	\param p_tab pointer to the bss table
 *	\param ssid SSID string 
 *	\return index of the table, BSS_NOT_FOUND if not in the table
 *	\pre
 *	\post
 *	\note search by sequential search

 IRQL = DISPATCH_LEVEL

 */
ULONG BssTableSearch(BSS_TABLE *Tab, UCHAR *pBssid, UCHAR Channel)
{
	UCHAR i;

	for (i = 0; i < Tab->BssNr; i++) 
	{
		
		/*
			Some AP that support A/B/G mode that may used the same BSSID on 11A and 11B/G.
			We should distinguish this case.
		*/
		if ((((Tab->BssEntry[i].Channel <= 14) && (Channel <= 14)) ||
			 ((Tab->BssEntry[i].Channel > 14) && (Channel > 14))) &&
			MAC_ADDR_EQUAL(Tab->BssEntry[i].Bssid, pBssid)
#ifdef WPA3_SUPPORT
			&& (Channel == Tab->BssEntry[i].Channel)
#endif
		) {
			return i;
		}
	}
	return (ULONG)BSS_NOT_FOUND;
}


ULONG BssSsidTableSearch(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 pBssid,
	IN PUCHAR	 pSsid,
	IN UCHAR	 SsidLen,
	IN UCHAR	 Channel) 
{
	UCHAR i;

	for (i = 0; i < Tab->BssNr; i++) 
	{
		
		/* Some AP that support A/B/G mode that may used the same BSSID on 11A and 11B/G.*/
		/* We should distinguish this case.*/
		/*		*/
		if ((((Tab->BssEntry[i].Channel <= 14) && (Channel <= 14)) ||
			 ((Tab->BssEntry[i].Channel > 14) && (Channel > 14))) &&
			MAC_ADDR_EQUAL(Tab->BssEntry[i].Bssid, pBssid) &&
			SSID_EQUAL(pSsid, SsidLen, Tab->BssEntry[i].Ssid, Tab->BssEntry[i].SsidLen)) {
#ifdef WPA3_SUPPORT
			if (Tab->BssEntry[i].Channel == Channel)
#endif
				return i;
		}
	}
	return (ULONG)BSS_NOT_FOUND;
}


ULONG BssTableSearchWithSSID(
	IN BSS_TABLE *Tab, 
	IN PUCHAR	 Bssid,
	IN PUCHAR	 pSsid,
	IN UCHAR	 SsidLen,
	IN UCHAR	 Channel)
{
	UCHAR i;

	for (i = 0; i < Tab->BssNr; i++) 
	{
		if ((((Tab->BssEntry[i].Channel <= 14) && (Channel <= 14)) ||
			((Tab->BssEntry[i].Channel > 14) && (Channel > 14))) &&
			MAC_ADDR_EQUAL(&(Tab->BssEntry[i].Bssid), Bssid) &&
			(SSID_EQUAL(pSsid, SsidLen, Tab->BssEntry[i].Ssid, Tab->BssEntry[i].SsidLen) ||
			(NdisEqualMemory(pSsid, ZeroSsid, SsidLen)) || 
			(NdisEqualMemory(Tab->BssEntry[i].Ssid, ZeroSsid, Tab->BssEntry[i].SsidLen))))
		{ 
			return i;
		}
	}
	return (ULONG)BSS_NOT_FOUND;
}


ULONG BssSsidTableSearchBySSID(BSS_TABLE *Tab, UCHAR *pSsid, UCHAR SsidLen)
{
	UCHAR i;

	for (i = 0; i < Tab->BssNr; i++) 
	{
		if (SSID_EQUAL(pSsid, SsidLen, Tab->BssEntry[i].Ssid, Tab->BssEntry[i].SsidLen)) 
		{ 
			return i;
		}
	}
	return (ULONG)BSS_NOT_FOUND;
}

ULONG BssTableSearchByBSSID(BSS_TABLE *Tab, UCHAR *pBssid)
{
	UCHAR i;

	for (i = 0; i < Tab->BssNr; i++)
		if (MAC_ADDR_EQUAL(Tab->BssEntry[i].Bssid, pBssid))
			return i;

	return (ULONG) BSS_NOT_FOUND;
}

VOID BssTableDeleteEntry(BSS_TABLE *Tab, UCHAR *pBssid, UCHAR Channel)
{
	UCHAR i, j;

	for (i = 0; i < Tab->BssNr; i++) 
	{
		if ((Tab->BssEntry[i].Channel == Channel) && 
			(MAC_ADDR_EQUAL(Tab->BssEntry[i].Bssid, pBssid)))
		{
			UCHAR *pOldAddr = NULL;
			
			for (j = i; j < Tab->BssNr - 1; j++)
			{
				pOldAddr = Tab->BssEntry[j].pVarIeFromProbRsp;
				NdisMoveMemory(&(Tab->BssEntry[j]), &(Tab->BssEntry[j + 1]), sizeof(BSS_ENTRY));
				if (pOldAddr)
				{
					RTMPZeroMemory(pOldAddr, MAX_VIE_LEN);
					NdisMoveMemory(pOldAddr, 
								   Tab->BssEntry[j + 1].pVarIeFromProbRsp, 
								   Tab->BssEntry[j + 1].VarIeFromProbeRspLen);
					Tab->BssEntry[j].pVarIeFromProbRsp = pOldAddr;
				}
			}

			pOldAddr = Tab->BssEntry[Tab->BssNr - 1].pVarIeFromProbRsp;
			NdisZeroMemory(&(Tab->BssEntry[Tab->BssNr - 1]), sizeof(BSS_ENTRY));
			if (pOldAddr)
			{
				RTMPZeroMemory(pOldAddr, MAX_VIE_LEN);
				Tab->BssEntry[Tab->BssNr - 1].pVarIeFromProbRsp = pOldAddr;
			}
			
			Tab->BssNr -= 1;
			return;
		}
	}
}


/*! \brief
 *	\param 
 *	\return
 *	\pre
 *	\post
 */
VOID BssEntrySet(
	IN RTMP_ADAPTER *pAd, 
	OUT BSS_ENTRY *pBss, 
	IN BCN_IE_LIST *ie_list,
	IN CHAR Rssi,
	IN USHORT LengthVIE,	
	IN PNDIS_802_11_VARIABLE_IEs pVIE) 
{
	COPY_MAC_ADDR(pBss->Bssid, ie_list->Bssid);
	/* Default Hidden SSID to be TRUE, it will be turned to FALSE after coping SSID*/
	pBss->Hidden = 1;	
	if (ie_list->SsidLen > 0)
	{
		/* For hidden SSID AP, it might send beacon with SSID len equal to 0*/
		/* Or send beacon /probe response with SSID len matching real SSID length,*/
		/* but SSID is all zero. such as "00-00-00-00" with length 4.*/
		/* We have to prevent this case overwrite correct table*/
		if (NdisEqualMemory(ie_list->Ssid, ZeroSsid, ie_list->SsidLen) == 0)
		{
			NdisZeroMemory(pBss->Ssid, MAX_LEN_OF_SSID);
			NdisMoveMemory(pBss->Ssid, ie_list->Ssid, ie_list->SsidLen);
			pBss->SsidLen = ie_list->SsidLen;
			pBss->Hidden = 0;
		}
	}
	else
	{
		/* avoid  Hidden SSID form beacon to overwirite correct SSID from probe response */
		if (NdisEqualMemory(pBss->Ssid, ZeroSsid, pBss->SsidLen))
		{
			NdisZeroMemory(pBss->Ssid, MAX_LEN_OF_SSID);
			pBss->SsidLen = 0;
		}
	}
	
	pBss->BssType = ie_list->BssType;
	pBss->BeaconPeriod = ie_list->BeaconPeriod;
	if (ie_list->BssType == BSS_INFRA) 
	{
		if (ie_list->CfParm.bValid) 
		{
			pBss->CfpCount = ie_list->CfParm.CfpCount;
			pBss->CfpPeriod = ie_list->CfParm.CfpPeriod;
			pBss->CfpMaxDuration = ie_list->CfParm.CfpMaxDuration;
			pBss->CfpDurRemaining = ie_list->CfParm.CfpDurRemaining;
		}
	} 
	else 
	{
		pBss->AtimWin = ie_list->AtimWin;
	}

	NdisGetSystemUpTime(&pBss->LastBeaconRxTime);
	pBss->CapabilityInfo = ie_list->CapabilityInfo;
	/* The privacy bit indicate security is ON, it maight be WEP, TKIP or AES*/
	/* Combine with AuthMode, they will decide the connection methods.*/
	pBss->Privacy = CAP_IS_PRIVACY_ON(pBss->CapabilityInfo);
	ASSERT(ie_list->SupRateLen <= MAX_LEN_OF_SUPPORTED_RATES);
	if (ie_list->SupRateLen <= MAX_LEN_OF_SUPPORTED_RATES)		
		NdisMoveMemory(pBss->SupRate, ie_list->SupRate, ie_list->SupRateLen);
	else		
		NdisMoveMemory(pBss->SupRate, ie_list->SupRate, MAX_LEN_OF_SUPPORTED_RATES);	
	pBss->SupRateLen = ie_list->SupRateLen;
	ASSERT(ie_list->ExtRateLen <= MAX_LEN_OF_SUPPORTED_RATES);
	if (ie_list->ExtRateLen > MAX_LEN_OF_SUPPORTED_RATES)
		ie_list->ExtRateLen = MAX_LEN_OF_SUPPORTED_RATES;
	NdisMoveMemory(pBss->ExtRate, ie_list->ExtRate, ie_list->ExtRateLen);
	pBss->NewExtChanOffset = ie_list->NewExtChannelOffset;
	pBss->ExtRateLen = ie_list->ExtRateLen;
	pBss->Channel = ie_list->Channel;
	pBss->CentralChannel = ie_list->Channel;
	pBss->Rssi = Rssi;
	/* Update CkipFlag. if not exists, the value is 0x0*/
	pBss->CkipFlag = ie_list->CkipFlag;

	/* New for microsoft Fixed IEs*/
	NdisMoveMemory(pBss->FixIEs.Timestamp, &ie_list->TimeStamp, 8);
	pBss->FixIEs.BeaconInterval = ie_list->BeaconPeriod;
	pBss->FixIEs.Capabilities = ie_list->CapabilityInfo;

	/* New for microsoft Variable IEs*/
	if (LengthVIE != 0)
	{
		pBss->VarIELen = LengthVIE;
		NdisMoveMemory(pBss->VarIEs, pVIE, pBss->VarIELen);
	}
	else
	{
		pBss->VarIELen = 0;
	}

	pBss->AddHtInfoLen = 0;
	pBss->HtCapabilityLen = 0;
#ifdef DOT11_N_SUPPORT
	if (ie_list->HtCapabilityLen> 0)
	{
		pBss->HtCapabilityLen = ie_list->HtCapabilityLen;
		NdisMoveMemory(&pBss->HtCapability, &ie_list->HtCapability, ie_list->HtCapabilityLen);
		if (ie_list->AddHtInfoLen > 0)
		{
			pBss->AddHtInfoLen = ie_list->AddHtInfoLen;
			NdisMoveMemory(&pBss->AddHtInfo, &ie_list->AddHtInfo, ie_list->AddHtInfoLen);

			pBss->CentralChannel = get_cent_ch_by_htinfo(pAd, &ie_list->AddHtInfo,
											&ie_list->HtCapability);
		}

#ifdef DOT11_VHT_AC
		if (ie_list->vht_cap_len) {
			NdisMoveMemory(&pBss->vht_cap_ie, &ie_list->vht_cap_ie, ie_list->vht_cap_len);
			pBss->vht_cap_len = ie_list->vht_cap_len;
		}
		
		if (ie_list->vht_op_len) {
			VHT_OP_IE *vht_op;
					
			NdisMoveMemory(&pBss->vht_op_ie, &ie_list->vht_op_ie, ie_list->vht_op_len);
			pBss->vht_op_len = ie_list->vht_op_len;
			vht_op = &ie_list->vht_op_ie;
			if ((vht_op->vht_op_info.ch_width > 0) &&
				(ie_list->AddHtInfo.AddHtInfo.ExtChanOffset != EXTCHA_NONE) &&
				(ie_list->HtCapability.HtCapInfo.ChannelWidth == BW_40) &&
				(pBss->CentralChannel != ie_list->AddHtInfo.ControlChan))
			{
				UCHAR cent_ch;
				
				cent_ch = vht_cent_ch_freq(pAd, ie_list->AddHtInfo.ControlChan);
				DBGPRINT(RT_DEBUG_TRACE, ("%s():VHT cent_ch=%d, vht_op_info->center_freq_1=%d, Bss->CentCh=%d, change from CentralChannel to cent_ch!\n",
											__FUNCTION__, cent_ch, vht_op->vht_op_info.center_freq_1, pBss->CentralChannel));
				pBss->CentralChannel = vht_op->vht_op_info.center_freq_1;
			}
		}
#endif /* DOT11_VHT_AC */
	}
#endif /* DOT11_N_SUPPORT */

	BssCipherParse(pBss);

	/* new for QOS*/
	if (ie_list->EdcaParm.bValid)
		NdisMoveMemory(&pBss->EdcaParm, &ie_list->EdcaParm, sizeof(EDCA_PARM));
	else
		pBss->EdcaParm.bValid = FALSE;
	if (ie_list->QosCapability.bValid)
		NdisMoveMemory(&pBss->QosCapability, &ie_list->QosCapability, sizeof(QOS_CAPABILITY_PARM));
	else
		pBss->QosCapability.bValid = FALSE;
	if (ie_list->QbssLoad.bValid)
		NdisMoveMemory(&pBss->QbssLoad, &ie_list->QbssLoad, sizeof(QBSS_LOAD_PARM));
	else
		pBss->QbssLoad.bValid = FALSE;

	{
		PEID_STRUCT pEid;
		USHORT Length = 0;

#ifdef WSC_INCLUDED
		pBss->WpsAP = 0x00;
		pBss->WscDPIDFromWpsAP = 0xFFFF;
#endif /* WSC_INCLUDED */

#ifdef CONFIG_STA_SUPPORT
		NdisZeroMemory(&pBss->WpaIE.IE[0], MAX_CUSTOM_LEN);
		NdisZeroMemory(&pBss->RsnIE.IE[0], MAX_CUSTOM_LEN);
		NdisZeroMemory(&pBss->WpsIE.IE[0], MAX_CUSTOM_LEN);
		pBss->WpaIE.IELen = 0;
		pBss->RsnIE.IELen = 0;
		pBss->WpsIE.IELen = 0;
#ifdef WAPI_SUPPORT
		NdisZeroMemory(&pBss->WapiIE.IE[0], MAX_CUSTOM_LEN);
		pBss->WapiIE.IELen = 0;
#endif /* WAPI_SUPPORT */
#ifdef EXT_BUILD_CHANNEL_LIST
		NdisZeroMemory(&pBss->CountryString[0], 3);
		pBss->bHasCountryIE = FALSE;
#endif /* EXT_BUILD_CHANNEL_LIST */
#ifdef DOT11R_FT_SUPPORT
		pBss->bHasMDIE = FALSE;
		NdisZeroMemory(&pBss->FT_MDIE, sizeof(FT_MDIE));
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
		pEid = (PEID_STRUCT) pVIE;
		while ((Length + 2 + (USHORT)pEid->Len) <= LengthVIE)    
		{
#define WPS_AP		0x01
			switch(pEid->Eid)
			{
				case IE_WPA:
#ifdef WAC_SUPPORT
#define WAC_AP		0x04
					if (NdisEqualMemory(pEid->Octet, SAMSUNG_OUI, 3))
					{
#define	WAC_DEVICE_ADVERTISE_ID	0x00					
#define	WAC_DEVICE_INFORMATION_ID	0x03
						UCHAR		oui_type = 0;
						BOOLEAN		bWAC_Enable = FALSE;
						CHAR	device_info[5];

						oui_type = pEid->Octet[3];
						DBGPRINT(RT_DEBUG_WARN, ("%s: Samsung OUI Type = 0x%02X\n", __FUNCTION__, oui_type));
						if (oui_type == SAMSUNG_OUI_TYPE)
						{
							WAC_ParseAttr(WAC_DEVICE_ADVERTISE_ID, pEid->Octet+4, pEid->Len-4, &bWAC_Enable, 1);
							if (bWAC_Enable)
								pBss->WpsAP |= WAC_AP;
						}
#ifdef WAC_QOS_PRIORITY
						pBss->DeviceType = 0xFF;
						pBss->bForcePriority = FALSE;
						if (WAC_ParseAttr(WAC_DEVICE_INFORMATION_ID, pEid->Octet+4, pEid->Len-4, &device_info[0], 5))
						{	
							DBGPRINT(RT_DEBUG_TRACE, ("%s: WAC Device Information: %d, %d, %d, %d, %d\n", 
														__FUNCTION__, device_info[0], device_info[1], device_info[2],
														device_info[3], device_info[4]));
							pBss->DeviceType = (CHAR) device_info[3];
						}

						if (bWAC_Enable)
						{
							pBss->bForcePriority = TRUE;
						}
#endif /* WAC_QOS_PRIORITY */
					}
#endif /* WAC_SUPPORT */
					if (NdisEqualMemory(pEid->Octet, WPS_OUI, 4)
#ifdef IWSC_SUPPORT
						|| NdisEqualMemory(pEid->Octet, IWSC_OUI, 4)
#endif /* IWSC_SUPPORT */
						)
					{
#ifdef WSC_INCLUDED
						pBss->WpsAP |= WPS_AP;
						WscCheckWpsIeFromWpsAP(pAd, 
													pEid, 
#ifdef STA_EASY_CONFIG_SETUP
													&pBss->WpsAP,
#endif /* STA_EASY_CONFIG_SETUP */
													&pBss->WscDPIDFromWpsAP);
#endif /* WSC_INCLUDED */
#ifdef CONFIG_STA_SUPPORT
						if ((pEid->Len + 2) > MAX_CUSTOM_LEN)
						{
							pBss->WpsIE.IELen = 0;
							break;
						}
						pBss->WpsIE.IELen = pEid->Len + 2;
						NdisMoveMemory(pBss->WpsIE.IE, pEid, pBss->WpsIE.IELen);
#endif /* CONFIG_STA_SUPPORT */
						break;
					}
#ifdef CONFIG_STA_SUPPORT
					if (NdisEqualMemory(pEid->Octet, WPA_OUI, 4))
					{
						if ((pEid->Len + 2) > MAX_CUSTOM_LEN)
						{
							pBss->WpaIE.IELen = 0;
							break;
						}
						pBss->WpaIE.IELen = pEid->Len + 2;
						NdisMoveMemory(pBss->WpaIE.IE, pEid, pBss->WpaIE.IELen);
					}
#endif /* CONFIG_STA_SUPPORT */
					break;

#ifdef CONFIG_STA_SUPPORT
				case IE_RSN:
					if (NdisEqualMemory(pEid->Octet + 2, RSN_OUI, 3))
					{
						if ((pEid->Len + 2) > MAX_CUSTOM_LEN)
						{
							pBss->RsnIE.IELen = 0;
							break;
						}
						pBss->RsnIE.IELen = pEid->Len + 2;
						NdisMoveMemory(pBss->RsnIE.IE, pEid, pBss->RsnIE.IELen);
					}
					break;
#ifdef WAPI_SUPPORT
				case IE_WAPI:
					if (NdisEqualMemory(pEid->Octet + 4, WAPI_OUI, 3))
					{
						UCHAR           idx;
						snprintf((RTMP_STRING *) pBss->WapiIE.IE, sizeof(pBss->WapiIE.IE), "wapi_ie=%02x%02x", pEid->Eid, pEid->Len);
						for (idx = 0; idx < pEid->Len; idx++)
						{
							snprintf((RTMP_STRING *) pBss->WapiIE.IE, sizeof(pBss->WapiIE.IE), "%s%02x", pBss->WapiIE.IE, (unsigned char)pEid->Octet[idx]);
						}
						pBss->WapiIE.IELen = (pEid->Len*2) + 8; /* 2: ID(1 byte), LEN (1 byte), 8: len of "wapi_ie="*/
					}
					break;
#endif /* WAPI_SUPPORT */
#ifdef EXT_BUILD_CHANNEL_LIST					
				case IE_COUNTRY:					
					NdisMoveMemory(&pBss->CountryString[0], pEid->Octet, 3);
					pBss->bHasCountryIE = TRUE;
					break;
#endif /* EXT_BUILD_CHANNEL_LIST */
#ifdef DOT11R_FT_SUPPORT
				case IE_FT_MDIE:
					if (pEid->Len == sizeof(FT_MDIE))
					{
						pBss->bHasMDIE = TRUE;
						NdisMoveMemory(&pBss->FT_MDIE, pEid->Octet, pEid->Len);
					}
					break;
#endif /* DOT11R_FT_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
			}
			Length = Length + 2 + (USHORT)pEid->Len;  /* Eid[1] + Len[1]+ content[Len]*/
			pEid = (PEID_STRUCT)((UCHAR*)pEid + 2 + pEid->Len);        
		}
	}
}



/*! 
 *	\brief insert an entry into the bss table
 *	\param p_tab The BSS table
 *	\param Bssid BSSID
 *	\param ssid SSID
 *	\param ssid_len Length of SSID
 *	\param bss_type
 *	\param beacon_period
 *	\param timestamp
 *	\param p_cf
 *	\param atim_win
 *	\param cap
 *	\param rates
 *	\param rates_len
 *	\param channel_idx
 *	\return none
 *	\pre
 *	\post
 *	\note If SSID is identical, the old entry will be replaced by the new one
	 
 IRQL = DISPATCH_LEVEL
 
 */
ULONG BssTableSetEntry(
	IN PRTMP_ADAPTER pAd,
	OUT BSS_TABLE *Tab,
	IN BCN_IE_LIST *ie_list,
	IN CHAR Rssi,
	IN USHORT LengthVIE,	
	IN PNDIS_802_11_VARIABLE_IEs pVIE)
{
	ULONG	Idx;
#ifdef APCLI_SUPPORT
	BOOLEAN bInsert = FALSE;
	PAPCLI_STRUCT pApCliEntry = NULL;
	UCHAR i;
#endif /* APCLI_SUPPORT */


	Idx = BssTableSearch(Tab, ie_list->Bssid, ie_list->Channel);
	if (Idx >= MAX_LEN_OF_BSS_TABLE && Idx != BSS_NOT_FOUND) {
		DBGPRINT(RT_DEBUG_ERROR,
			("%s(): invalid BSS Idx(%ld)\n", __func__, Idx));
		return BSS_NOT_FOUND;
	}
	if (Idx == BSS_NOT_FOUND) 
	{
		if (Tab->BssNr >= MAX_LEN_OF_BSS_TABLE)
	    {
			/*
				It may happen when BSS Table was full. 
				The desired AP will not be added into BSS Table
				In this case, if we found the desired AP then overwrite BSS Table.
			*/
#ifdef APCLI_SUPPORT
			for (i = 0; i < pAd->ApCfg.ApCliNum; i++)
			{
				pApCliEntry = &pAd->ApCfg.ApCliTab[i];
				if (MAC_ADDR_EQUAL(pApCliEntry->MlmeAux.Bssid, ie_list->Bssid)
					|| SSID_EQUAL(pApCliEntry->MlmeAux.Ssid, pApCliEntry->MlmeAux.SsidLen, ie_list->Ssid, ie_list->SsidLen))
				{
					bInsert = TRUE;
					break;
				}
			}
#endif /* APCLI_SUPPORT */
			if(!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED) ||
				!OPSTATUS_TEST_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED))
			{
				if (MAC_ADDR_EQUAL(pAd->ScanCtrl.Bssid, ie_list->Bssid) ||
					SSID_EQUAL(pAd->ScanCtrl.Ssid, pAd->ScanCtrl.SsidLen, ie_list->Ssid, ie_list->SsidLen)
#ifdef APCLI_SUPPORT
					|| bInsert
#endif /* APCLI_SUPPORT */
#ifdef RT_CFG80211_SUPPORT
					/* YF: Driver ScanTable full but supplicant the SSID exist on supplicant */
					|| SSID_EQUAL(pAd->cfg80211_ctrl.Cfg_pending_Ssid, pAd->cfg80211_ctrl.Cfg_pending_SsidLen, ie_list->Ssid, ie_list->SsidLen)
#endif /* RT_CFG80211_SUPPORT */
					)
				{
					Idx = Tab->BssOverlapNr;
					NdisZeroMemory(&(Tab->BssEntry[Idx]), sizeof(BSS_ENTRY));
					BssEntrySet(pAd, &Tab->BssEntry[Idx], ie_list, Rssi, LengthVIE, pVIE);
					Tab->BssOverlapNr += 1;
					Tab->BssOverlapNr = Tab->BssOverlapNr % MAX_LEN_OF_BSS_TABLE;
#ifdef RT_CFG80211_SUPPORT
					pAd->cfg80211_ctrl.Cfg_pending_SsidLen = 0;
					NdisZeroMemory(pAd->cfg80211_ctrl.Cfg_pending_Ssid, MAX_LEN_OF_SSID+1);
#endif /* RT_CFG80211_SUPPORT */
				}
				return Idx;
			}
			else
			{
				return BSS_NOT_FOUND;
			}
		}
		Idx = Tab->BssNr;
		BssEntrySet(pAd, &Tab->BssEntry[Idx], ie_list, Rssi, LengthVIE, pVIE);
		Tab->BssNr++;
	} 
	else
	{
		BssEntrySet(pAd, &Tab->BssEntry[Idx], ie_list, Rssi, LengthVIE, pVIE);
	}

	return Idx;
}


#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
VOID  TriEventInit(RTMP_ADAPTER *pAd) 
{
	UCHAR i;

	for (i = 0;i < MAX_TRIGGER_EVENT;i++)
		pAd->CommonCfg.TriggerEventTab.EventA[i].bValid = FALSE;
	
	pAd->CommonCfg.TriggerEventTab.EventANo = 0;
	pAd->CommonCfg.TriggerEventTab.EventBCountDown = 0;
}


INT TriEventTableSetEntry(
	IN RTMP_ADAPTER *pAd, 
	OUT TRIGGER_EVENT_TAB *Tab, 
	IN UCHAR *pBssid, 
	IN HT_CAPABILITY_IE *pHtCapability,
	IN UCHAR HtCapabilityLen,
	IN UCHAR RegClass,
	IN UCHAR ChannelNo)
{
	/* Event A, legacy AP exist.*/
	if (HtCapabilityLen == 0)
	{
		UCHAR index;
		
		/*
			Check if we already set this entry in the Event Table.
		*/
		for (index = 0; index<MAX_TRIGGER_EVENT; index++)
		{
			if ((Tab->EventA[index].bValid == TRUE) && 
				(Tab->EventA[index].Channel == ChannelNo) && 
				(Tab->EventA[index].RegClass == RegClass)
			)
			{
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("ReturnDueToAlreadyInserted(EventIdx=%d,Ch=%d,Reg=%d)\n", 
											index, ChannelNo, RegClass));
#endif /* RELEASE_EXCLUDE */
				return 0;
			}
		}
		
		/*
			If not set, add it to the Event table
		*/
		if (Tab->EventANo < MAX_TRIGGER_EVENT)
		{
			RTMPMoveMemory(Tab->EventA[Tab->EventANo].BSSID, pBssid, 6);
			Tab->EventA[Tab->EventANo].bValid = TRUE;
			Tab->EventA[Tab->EventANo].Channel = ChannelNo;
			if (RegClass != 0)
			{
				/* Beacon has Regulatory class IE. So use beacon's*/
				Tab->EventA[Tab->EventANo].RegClass = RegClass;
			}
			else
			{
				/* Use Station's Regulatory class instead.*/
				/* If no Reg Class in Beacon, set to "unknown"*/
				/* TODO:  Need to check if this's valid*/
				Tab->EventA[Tab->EventANo].RegClass = 0; /* ????????????????? need to check*/
			}
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("Insert EventA Entry(EvtIdx=%d,Ch=%d,Reg=%d,Mac=%02x:%02x:%02x:%02x:%02x:%02x\n", 
											Tab->EventANo, ChannelNo, RegClass, PRINT_MAC(pBssid)));
#endif /* RELEASE_EXCLUDE */
			Tab->EventANo ++;
		}
	}
#ifdef DOT11V_WNM_SUPPORT
	/* Not complete yet. Ignore for compliing successfully.*/
#else
	else if (pHtCapability->HtCapInfo.Forty_Mhz_Intolerant)
	{
		/* Event B.   My BSS beacon has Intolerant40 bit set*/
		Tab->EventBCountDown = pAd->CommonCfg.Dot11BssWidthChanTranDelay;
	}
#endif /* DOT11V_WNM_SUPPORT */

	return 0;
}
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */


VOID BssTableSsidSort(
	IN RTMP_ADAPTER *pAd, 
	OUT BSS_TABLE *OutTab, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen) 
{
	INT i;
#ifdef WSC_STA_SUPPORT
	PWSC_CTRL	pWpsCtrl = &pAd->StaCfg.WscControl;
#endif /* WSC_STA_SUPPORT */
	struct wifi_dev *wdev = &pAd->StaCfg.wdev;
#ifdef DOT11W_PMF_SUPPORT
	RSN_CAPABILITIES RsnCap;
#endif

	BssTableInit(OutTab);

	if ((SsidLen == 0) && 
		(pAd->StaCfg.bAutoConnectIfNoSSID == FALSE))
		return;

	for (i = 0; i < pAd->ScanTab.BssNr; i++) 
	{
		BSS_ENTRY *pInBss = &pAd->ScanTab.BssEntry[i];
		BOOLEAN	bIsHiddenApIncluded = FALSE;
		
		if ( ((pAd->CommonCfg.bIEEE80211H == 1) && 
				(pAd->MlmeAux.Channel > 14) && 
				 RadarChannelCheck(pAd, pInBss->Channel))
#ifdef DPA_T
			 ||(pInBss->Channel >= 12)
#endif /* !DPA_T */
#ifdef WIFI_REGION32_HIDDEN_SSID_SUPPORT
			 ||((pInBss->Channel == 12) || (pInBss->Channel == 13))
#endif /* WIFI_REGION32_HIDDEN_SSID_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT /* Roger sync Carrier             */
             || (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
#endif /* CARRIER_DETECTION_SUPPORT */
            )
		{
			if (pInBss->Hidden)
				bIsHiddenApIncluded = TRUE;
		}            


		if ((pInBss->BssType == pAd->StaCfg.BssType) && 
			(SSID_EQUAL(Ssid, SsidLen, pInBss->Ssid, pInBss->SsidLen) || bIsHiddenApIncluded))
		{
			BSS_ENTRY *pOutBss = &OutTab->BssEntry[OutTab->BssNr];

#ifdef WPA_SUPPLICANT_SUPPORT
			if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & 0x80)
			{
				/* copy matching BSS from InTab to OutTab*/
				NdisMoveMemory(pOutBss, pInBss, sizeof(BSS_ENTRY));
				OutTab->BssNr++;
				continue;
			}
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef WSC_STA_SUPPORT
			if ((pWpsCtrl->WscConfMode != WSC_DISABLE) && pWpsCtrl->bWscTrigger)
			{
				/* copy matching BSS from InTab to OutTab*/
				NdisMoveMemory(pOutBss, pInBss, sizeof(BSS_ENTRY));
				OutTab->BssNr++;
				continue;
			}
#endif /* WSC_STA_SUPPORT */

#ifdef EXT_BUILD_CHANNEL_LIST
			/* If no Country IE exists no Connection will be established when IEEE80211dClientMode is strict.*/
			if ((pAd->StaCfg.IEEE80211dClientMode == Rt802_11_D_Strict) &&
				(pInBss->bHasCountryIE == FALSE))
			{
				DBGPRINT(RT_DEBUG_TRACE,("StaCfg.IEEE80211dClientMode == Rt802_11_D_Strict, but this AP doesn't have country IE.\n"));
				continue;
			}
#endif /* EXT_BUILD_CHANNEL_LIST */

#ifdef DOT11_N_SUPPORT
			/* 2.4G/5G N only mode*/
			if ((pInBss->HtCapabilityLen == 0) &&
				(WMODE_HT_ONLY(pAd->CommonCfg.PhyMode)))
			{
				DBGPRINT(RT_DEBUG_TRACE,("STA is in N-only Mode, this AP don't have Ht capability in Beacon.\n"));
				continue;
			}

			if ((pAd->CommonCfg.PhyMode == (WMODE_G | WMODE_GN)) &&
				((pInBss->SupRateLen + pInBss->ExtRateLen) < 12))
			{
				DBGPRINT(RT_DEBUG_TRACE,("STA is in GN-only Mode, this AP is in B mode.\n"));
				continue;
			}
#endif /* DOT11_N_SUPPORT */


#ifdef DOT11W_PMF_SUPPORT
			if (((wdev->AuthMode == Ndis802_11AuthModeWPA2) || (wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)
#ifdef WPA3_SUPPORT
			|| (wdev->AuthMode == Ndis802_11AuthModeWPA3SAE)
#endif
				) && (pInBss)) {
                                CLIENT_STATUS_CLEAR_FLAG(pInBss, fCLIENT_STATUS_PMF_CAPABLE);
                                CLIENT_STATUS_CLEAR_FLAG(pInBss, fCLIENT_STATUS_USE_SHA256);

                		NdisMoveMemory(&RsnCap, &pInBss->WPA2.RsnCapability, sizeof(RSN_CAPABILITIES));
                		RsnCap.word = cpu2le16(RsnCap.word);                                
                                if ((pAd->StaCfg.PmfCfg.MFPR) && (RsnCap.field.MFPC == FALSE))
                                        continue;

                                if (((pAd->StaCfg.PmfCfg.MFPC == FALSE) && (RsnCap.field.MFPC == FALSE))
                                        || (pAd->StaCfg.PmfCfg.MFPC && RsnCap.field.MFPC && (pAd->StaCfg.PmfCfg.MFPR == FALSE) && (RsnCap.field.MFPR == FALSE)))
                                {
                                        if ((pAd->StaCfg.PmfCfg.PMFSHA256) && (pInBss->IsSupportSHA256KeyDerivation == FALSE))
                                                continue;
                                }        

                                if ((pAd->StaCfg.PmfCfg.MFPC) && (RsnCap.field.MFPC))
				        CLIENT_STATUS_SET_FLAG(pInBss, fCLIENT_STATUS_PMF_CAPABLE);

                                if ((pAd->StaCfg.PmfCfg.PMFSHA256 && pInBss->IsSupportSHA256KeyDerivation)
                                        || (pAd->StaCfg.PmfCfg.MFPC && RsnCap.field.MFPR)
                                        || (pAd->StaCfg.PmfCfg.MFPC && pInBss->IsSupportSHA256KeyDerivation))
				        CLIENT_STATUS_SET_FLAG(pInBss, fCLIENT_STATUS_USE_SHA256);

                        }                        
#endif /* DOT11W_PMF_SUPPORT */

			/* New for WPA2*/
			/* Check the Authmode first*/
			if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
			{
				/* Check AuthMode and AuthModeAux for matching, in case AP support dual-mode*/
#ifdef WPA3_SUPPORT
				for (i = 0; i < pInBss->u4AuthModeCount; i++)
					if (wdev->AuthMode != pInBss->AuthMode[i])
						continue;/* None matched*/
#else
				if ((wdev->AuthMode != pInBss->AuthMode) && (wdev->AuthMode != pInBss->AuthModeAux))
					/* None matched*/
					continue;
#endif
				/* Check cipher suite, AP must have more secured cipher than station setting*/
				if ((wdev->AuthMode == Ndis802_11AuthModeWPA) || (wdev->AuthMode == Ndis802_11AuthModeWPAPSK))
				{
					/* If it's not mixed mode, we should only let BSS pass with the same encryption*/
					if (pInBss->WPA.bMixMode == FALSE)
						if (wdev->WepStatus != pInBss->WPA.GroupCipher)
							continue;
						
					/* check group cipher*/
					if ((wdev->WepStatus < pInBss->WPA.GroupCipher) &&
						(pInBss->WPA.GroupCipher != Ndis802_11GroupWEP40Enabled) && 
						(pInBss->WPA.GroupCipher != Ndis802_11GroupWEP104Enabled))
						continue;

					/* check pairwise cipher, skip if none matched*/
					/* If profile set to AES, let it pass without question.*/
					/* If profile set to TKIP, we must find one mateched*/
#ifdef WPA3_SUPPORT
					if (wdev->WepStatus == Ndis802_11Encryption2Enabled) {
						USHORT Count;

						Count = 0;
						for (i = 0; i < pInBss->WPA.u4PairwiseCipherCount; i++) {
							if ((wdev->WepStatus != pInBss->WPA.PairCipher[i]))
								Count++;
							else
								break;
						}
						if (Count == pInBss->WPA.u4PairwiseCipherCount)
							continue;
					}
#else
					if ((wdev->WepStatus == Ndis802_11TKIPEnable) && 
						(wdev->WepStatus != pInBss->WPA.PairCipher) && 
						(wdev->WepStatus != pInBss->WPA.PairCipherAux))
						continue;
#endif
				}
				else if ((wdev->AuthMode == Ndis802_11AuthModeWPA2) || (wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)) {
					/* If it's not mixed mode, we should only let BSS pass with the same encryption*/
					if (pInBss->WPA2.bMixMode == FALSE)
						if (wdev->WepStatus != pInBss->WPA2.GroupCipher)
							continue;
						
					/* check group cipher*/
					if ((wdev->WepStatus < pInBss->WPA.GroupCipher) &&
						(pInBss->WPA2.GroupCipher != Ndis802_11GroupWEP40Enabled) && 
						(pInBss->WPA2.GroupCipher != Ndis802_11GroupWEP104Enabled))
						continue;

					/* check pairwise cipher, skip if none matched*/
					/* If profile set to AES, let it pass without question.*/
					/* If profile set to TKIP, we must find one mateched*/
#ifdef WPA3_SUPPORT
					if (wdev->WepStatus == Ndis802_11Encryption2Enabled) {
						USHORT Count;

						Count = 0;
						for (i = 0; i < pInBss->WPA2.u4PairwiseCipherCount; i++) {
							if (wdev->WepStatus != pInBss->WPA2.PairCipher[i])
								Count++;
							else
								break;
						}
						if (Count == pInBss->WPA2.u4PairwiseCipherCount)
							continue;
					}
#else
					if ((wdev->WepStatus == Ndis802_11TKIPEnable) && 
						(wdev->WepStatus != pInBss->WPA2.PairCipher) && 
						(wdev->WepStatus != pInBss->WPA2.PairCipherAux))
						continue;
#endif
				}
#ifdef WAPI_SUPPORT
				else if ((wdev->AuthMode == Ndis802_11AuthModeWAICERT) || (wdev->AuthMode == Ndis802_11AuthModeWAIPSK))
				{					
					/* check cipher algorithm*/
					if ((wdev->WepStatus != pInBss->WAPI.GroupCipher) ||
#ifdef WPA3_SUPPORT
						(pAd->StaCfg.WepStatus != pInBss->WAPI.PairCipher[0])
#else
						(wdev->WepStatus != pInBss->WAPI.PairCipher)
#endif
					)
						continue;											
				}
#endif /* WAPI_SUPPORT */
#ifdef WPA3_SUPPORT
				else if ((wdev->AuthMode == Ndis802_11AuthModeWPA3SAE) || (wdev->AuthMode == Ndis802_11AuthModeWPA2PSKWPA3SAE)) {
					if (pInBss->WPA2.bMixMode == FALSE)
						if (wdev->WepStatus != pInBss->WPA2.GroupCipher)
							continue;
					/* check group cipher*/
					if ((wdev->WepStatus < pInBss->WPA2.GroupCipher) &&
						(pInBss->WPA2.GroupCipher != Ndis802_11GroupWEP40Enabled) &&
						(pInBss->WPA2.GroupCipher != Ndis802_11GroupWEP104Enabled))
						continue;
				}
#endif
			}			
			/* Bss Type matched, SSID matched. */
			/* We will check wepstatus for qualification Bss*/
			else if (wdev->WepStatus != pInBss->WepStatus)
			{
				DBGPRINT(RT_DEBUG_TRACE,("StaCfg.WepStatus=%d, while pInBss->WepStatus=%d\n", wdev->WepStatus, pInBss->WepStatus));
				
				/* For the SESv2 case, we will not qualify WepStatus.*/
				
				if (!pInBss->bSES)
					continue;
			}

			/* Since the AP is using hidden SSID, and we are trying to connect to ANY*/
			/* It definitely will fail. So, skip it.*/
			/* CCX also require not even try to connect it!!*/
			if (SsidLen == 0)
				continue;
			
			/* copy matching BSS from InTab to OutTab*/
			NdisMoveMemory(pOutBss, pInBss, sizeof(BSS_ENTRY));

			OutTab->BssNr++;
		}
		else if ((pInBss->BssType == pAd->StaCfg.BssType) && (SsidLen == 0))
		{
			BSS_ENTRY *pOutBss = &OutTab->BssEntry[OutTab->BssNr];

#ifdef WSC_STA_SUPPORT
			if ((pWpsCtrl->WscConfMode != WSC_DISABLE) && pWpsCtrl->bWscTrigger)
			{
				/* copy matching BSS from InTab to OutTab*/
				NdisMoveMemory(pOutBss, pInBss, sizeof(BSS_ENTRY));
				OutTab->BssNr++;
				continue;
			}
#endif /* WSC_STA_SUPPORT */

#ifdef DOT11_N_SUPPORT
			/* 2.4G/5G N only mode*/
			if ((pInBss->HtCapabilityLen == 0) &&
				WMODE_HT_ONLY(pAd->CommonCfg.PhyMode))
			{
				DBGPRINT(RT_DEBUG_TRACE,("STA is in N-only Mode, this AP don't have Ht capability in Beacon.\n"));
				continue;
			}

			if ((pAd->CommonCfg.PhyMode == (WMODE_G | WMODE_GN)) &&
				((pInBss->SupRateLen + pInBss->ExtRateLen) < 12))
			{
				DBGPRINT(RT_DEBUG_TRACE,("STA is in GN-only Mode, this AP is in B mode.\n"));
				continue;
			}
#endif /* DOT11_N_SUPPORT */

			/* New for WPA2*/
			/* Check the Authmode first*/
			if (wdev->AuthMode >= Ndis802_11AuthModeWPA)
			{
				/* Check AuthMode and AuthModeAux for matching, in case AP support dual-mode*/
#ifdef WPA3_SUPPORT
				for (i = 0; i < pInBss->u4AuthModeCount; i++)
					if (wdev->AuthMode != pInBss->AuthMode[i])
						continue;/* None matched*/
#else
				if ((wdev->AuthMode != pInBss->AuthMode) && (wdev->AuthMode != pInBss->AuthModeAux))
					/* None matched*/
					continue;
#endif
				/* Check cipher suite, AP must have more secured cipher than station setting*/
				if ((wdev->AuthMode == Ndis802_11AuthModeWPA) || (wdev->AuthMode == Ndis802_11AuthModeWPAPSK))
				{
					/* If it's not mixed mode, we should only let BSS pass with the same encryption*/
					if (pInBss->WPA.bMixMode == FALSE)
						if (wdev->WepStatus != pInBss->WPA.GroupCipher)
							continue;
						
					/* check group cipher*/
					if (wdev->WepStatus < pInBss->WPA.GroupCipher)
						continue;

					/* check pairwise cipher, skip if none matched*/
					/* If profile set to AES, let it pass without question.*/
					/* If profile set to TKIP, we must find one mateched*/
#ifdef WPA3_SUPPORT
					if (wdev->WepStatus == Ndis802_11Encryption2Enabled) {
						USHORT Count;

						Count = 0;
						for (i = 0; i < pInBss->WPA.u4PairwiseCipherCount; i++) {
							if (wdev->WepStatus != pInBss->WPA.PairCipher[i])
								Count++;
							else
								break;
						}
						if (Count == pInBss->WPA.u4PairwiseCipherCount)
							continue;
					}
#else
					if ((wdev->WepStatus == Ndis802_11TKIPEnable) && 
						(wdev->WepStatus != pInBss->WPA.PairCipher) && 
						(wdev->WepStatus != pInBss->WPA.PairCipherAux))
						continue;
#endif
				} else if ((wdev->AuthMode == Ndis802_11AuthModeWPA2) || (wdev->AuthMode == Ndis802_11AuthModeWPA2PSK)) {
					/* If it's not mixed mode, we should only let BSS pass with the same encryption*/
					if (pInBss->WPA2.bMixMode == FALSE)
						if (wdev->WepStatus != pInBss->WPA2.GroupCipher)
							continue;
						
					/* check group cipher*/
					if (wdev->WepStatus < pInBss->WPA2.GroupCipher)
						continue;

					/* check pairwise cipher, skip if none matched*/
					/* If profile set to AES, let it pass without question.*/
					/* If profile set to TKIP, we must find one mateched*/
#ifdef WPA3_SUPPORT
					if (wdev->WepStatus == Ndis802_11Encryption2Enabled) {
						USHORT Count;

						Count = 0;
						for (i = 0; i < pInBss->WPA2.u4PairwiseCipherCount; i++) {
							if (wdev->WepStatus != pInBss->WPA2.PairCipher[i])
								Count++;
							else
								break;
						}
						if (Count == pInBss->WPA2.u4PairwiseCipherCount)
							continue;
					}
#else
					if ((wdev->WepStatus == Ndis802_11TKIPEnable) && 
						(wdev->WepStatus != pInBss->WPA2.PairCipher) && 
						(wdev->WepStatus != pInBss->WPA2.PairCipherAux))
						continue;
#endif
				}
#ifdef WAPI_SUPPORT
				else if ((wdev->AuthMode == Ndis802_11AuthModeWAICERT) || (wdev->AuthMode == Ndis802_11AuthModeWAIPSK))
				{					
					/* check cipher algorithm*/
					if ((wdev->WepStatus != pInBss->WAPI.GroupCipher) ||
#ifdef WPA3_SUPPORT
						(pAd->StaCfg.WepStatus != pInBss->WAPI.PairCipher[0])
#else
						(wdev->WepStatus != pInBss->WAPI.PairCipher)
#endif
					)
						continue;											
				}
#endif /* WAPI_SUPPORT */				
			}
			/* Bss Type matched, SSID matched. */
			/* We will check wepstatus for qualification Bss*/
			else if (wdev->WepStatus != pInBss->WepStatus)
					continue;
			
			/* copy matching BSS from InTab to OutTab*/
			NdisMoveMemory(pOutBss, pInBss, sizeof(BSS_ENTRY));

			OutTab->BssNr++;
		}
#ifdef WSC_STA_SUPPORT		
		else if ((pWpsCtrl->WscConfMode != WSC_DISABLE) && 
				 (pWpsCtrl->bWscTrigger) &&
				 MAC_ADDR_EQUAL(pWpsCtrl->WscBssid, pInBss->Bssid))
		{
			BSS_ENTRY *pOutBss = &OutTab->BssEntry[OutTab->BssNr];
			
			/* copy matching BSS from InTab to OutTab*/
			NdisMoveMemory(pOutBss, pInBss, sizeof(BSS_ENTRY));

			/*
				Linksys WRT610N WPS AP will change the SSID from linksys to linksys_WPS_<four random characters> 
				when the Linksys WRT610N is in the state 'WPS Unconfigured' after set to factory default.
			*/
			NdisZeroMemory(pAd->MlmeAux.Ssid, MAX_LEN_OF_SSID);
			NdisMoveMemory(pAd->MlmeAux.Ssid, pInBss->Ssid, pInBss->SsidLen);
			pAd->MlmeAux.SsidLen = pInBss->SsidLen;

			
			/* Update Reconnect Ssid, that user desired to connect.*/
			
			NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, MAX_LEN_OF_SSID);
			NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.Ssid, pAd->MlmeAux.SsidLen);
			pAd->MlmeAux.AutoReconnectSsidLen = pAd->MlmeAux.SsidLen;

			OutTab->BssNr++;
			continue;
		}
#endif /* WSC_STA_SUPPORT */

		if (OutTab->BssNr >= MAX_LEN_OF_BSS_TABLE)
			break;
	}

	BssTableSortByRssi(OutTab, FALSE);
}
#endif /* CONFIG_STA_SUPPORT */

#if defined(CONFIG_STA_SUPPORT) || defined(APCLI_AUTO_CONNECT_SUPPORT)
/* IRQL = DISPATCH_LEVEL*/
VOID BssTableSortByRssi(
	IN OUT BSS_TABLE *OutTab,
	IN BOOLEAN isInverseOrder)
{
	INT i, j;
	BSS_ENTRY *pTmpBss = NULL;


	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&pTmpBss, sizeof(BSS_ENTRY));
	if (pTmpBss == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return;
	}

	for (i = 0; i < OutTab->BssNr - 1; i++) 
	{
		for (j = i+1; j < OutTab->BssNr; j++) 
		{
			if (OutTab->BssEntry[j].Rssi > OutTab->BssEntry[i].Rssi ?
				!isInverseOrder : isInverseOrder)
			{
				if (OutTab->BssEntry[j].Rssi != OutTab->BssEntry[i].Rssi )
				{
					NdisMoveMemory(pTmpBss, &OutTab->BssEntry[j], sizeof(BSS_ENTRY));
					NdisMoveMemory(&OutTab->BssEntry[j], &OutTab->BssEntry[i], sizeof(BSS_ENTRY));
					NdisMoveMemory(&OutTab->BssEntry[i], pTmpBss, sizeof(BSS_ENTRY));
				}
			}
		}
	}

	if (pTmpBss != NULL)
		os_free_mem(NULL, pTmpBss);
}
#endif /* defined(CONFIG_STA_SUPPORT) || defined(APCLI_AUTO_CONNECT_SUPPORT) */


VOID BssCipherParse(BSS_ENTRY *pBss)
{
	PEID_STRUCT 		 pEid;
	PUCHAR				pTmp;
	PRSN_IE_HEADER_STRUCT			pRsnHeader;
	PCIPHER_SUITE_STRUCT			pCipher;
	PAKM_SUITE_STRUCT				pAKM;
	USHORT							Count;
	INT								Length;
	NDIS_802_11_ENCRYPTION_STATUS	TmpCipher;
#ifndef WPA3_SUPPORT
	BOOLEAN                         fgMultiAKMs = FALSE;
#else
	USHORT							TmpCount;
	USHORT							MaxCount;
	NDIS_802_11_AUTHENTICATION_MODE TmpAuthMode;
	UINT32 TmpAKMSuite;

	TmpCount = 0;
#endif

	
	/* WepStatus will be reset later, if AP announce TKIP or AES on the beacon frame.*/
	
	if (pBss->Privacy)
	{
		pBss->WepStatus 	= Ndis802_11WEPEnabled;
	}
	else
	{
		pBss->WepStatus 	= Ndis802_11WEPDisabled;
	}
#ifdef WPA3_SUPPORT
	/* Set default to disable & open authentication before parsing variable IE*/
	for (TmpCount = 0; TmpCount < MAX_NUM_SUPPORTED_AKM_SUITES; TmpCount++)
		pBss->AuthMode[TmpCount] = Ndis802_11AuthModeOpen;

	/* Init WPA/WPA2 setting*/
	pBss->WPA.GroupCipher	= Ndis802_11WEPDisabled;
	pBss->WPA2.GroupCipher	= Ndis802_11WEPDisabled;
	for (TmpCount = 0; TmpCount < MAX_NUM_SUPPORTED_CIPHER_SUITES; TmpCount++) {
		pBss->WPA.PairCipher[TmpCount]	= Ndis802_11WEPDisabled;
		pBss->WPA2.PairCipher[TmpCount]	= Ndis802_11WEPDisabled;
	}
	for (TmpCount = 0; TmpCount < MAX_NUM_SUPPORTED_AKM_SUITES; TmpCount++) {
		pBss->WPA.au4AuthKeyMgtSuite[TmpCount]	= RSN_AKM_SUITE_NONE;
		pBss->WPA2.au4AuthKeyMgtSuite[TmpCount]	= RSN_AKM_SUITE_NONE;
	}
#ifdef DOT11W_PMF_SUPPORT
	pBss->IsSupportSHA256KeyDerivation = FALSE;
#endif /* DOT11W_PMF_SUPPORT */
	pBss->WPA.RsnCapability = 0;
	pBss->WPA2.RsnCapability = 0;
	pBss->WPA.u2PmkidCnt = 0;
	NdisZeroMemory(pBss->WPA.aucPmkidList, (sizeof(UCHAR) * MAX_NUM_SUPPORTED_PMKID * RSN_PMKID_LEN));
	pBss->WPA.u4GroupMgmtKeyCipherSuite = 0;
	pBss->WPA2.u2PmkidCnt = 0;
	NdisZeroMemory(pBss->WPA2.aucPmkidList, (sizeof(UCHAR) * MAX_NUM_SUPPORTED_PMKID * RSN_PMKID_LEN));
	pBss->WPA2.u4GroupMgmtKeyCipherSuite = 0;
	pBss->WPA.bMixMode		= FALSE;
	pBss->WPA2.bMixMode		= FALSE;

#ifdef WAPI_SUPPORT
	pBss->WPAI.GroupCipher	= Ndis802_11WEPDisabled;
	for (TmpCount = 0; TmpCount < MAX_NUM_SUPPORTED_CIPHER_SUITES; TmpCount++)
		pBss->WPAI.PairCipher[TmpCount]	= Ndis802_11WEPDisabled;
	pBss->WPAI.RsnCapability = 0;
	pBss->WPAI.u2PmkidCnt = 0;
	NdisZeroMemory(pBss->WPAI.aucPmkidList, (sizeof(UCHAR) * MAX_NUM_SUPPORTED_PMKID * RSN_PMKID_LEN));
	pBss->WPAI.u4GroupMgmtKeyCipherSuite = 0;
	pBss->WPAI.bMixMode		= FALSE;
#endif

#else /* WPA3_SUPPORT*/
	/* Set default to disable & open authentication before parsing variable IE*/
	pBss->AuthMode		= Ndis802_11AuthModeOpen;
	pBss->AuthModeAux	= Ndis802_11AuthModeOpen;
#ifdef DOT11W_PMF_SUPPORT
        pBss->IsSupportSHA256KeyDerivation = FALSE;
#endif /* DOT11W_PMF_SUPPORT */            

	/* Init WPA setting*/
	pBss->WPA.PairCipher	= Ndis802_11WEPDisabled;
	pBss->WPA.PairCipherAux = Ndis802_11WEPDisabled;
	pBss->WPA.GroupCipher	= Ndis802_11WEPDisabled;
	pBss->WPA.RsnCapability = 0;
	pBss->WPA.bMixMode		= FALSE;

	/* Init WPA2 setting*/
	pBss->WPA2.PairCipher	 = Ndis802_11WEPDisabled;
	pBss->WPA2.PairCipherAux = Ndis802_11WEPDisabled;
	pBss->WPA2.GroupCipher	 = Ndis802_11WEPDisabled;
	pBss->WPA2.RsnCapability = 0;
	pBss->WPA2.bMixMode 	 = FALSE;

#ifdef WAPI_SUPPORT
	/* Init WAPI setting*/
	pBss->WAPI.PairCipher	 = Ndis802_11WEPDisabled;
	pBss->WAPI.PairCipherAux = Ndis802_11WEPDisabled;
	pBss->WAPI.GroupCipher	 = Ndis802_11WEPDisabled;
	pBss->WAPI.RsnCapability = 0;
	pBss->WAPI.bMixMode 	 = FALSE;
#endif /* WAPI_SUPPORT */
#endif /* WPA3_SUPPORT */
	Length = (INT) pBss->VarIELen;

	while (Length > 0)
	{
		/* Parse cipher suite base on WPA1 & WPA2, they should be parsed differently*/
		pTmp = ((PUCHAR) pBss->VarIEs) + pBss->VarIELen - Length;
		pEid = (PEID_STRUCT) pTmp;
		switch (pEid->Eid)
		{
			case IE_WPA:
				if (NdisEqualMemory(pEid->Octet, SES_OUI, 3) && (pEid->Len == 7))
				{
					pBss->bSES = TRUE;
					break;
				}				
				else if (NdisEqualMemory(pEid->Octet, WPA_OUI, 4) != 1)
				{
					/* if unsupported vendor specific IE*/
					break;
				}				
				/*
					Skip OUI, version, and multicast suite
					This part should be improved in the future when AP supported multiple cipher suite.
					For now, it's OK since almost all APs have fixed cipher suite supported.
				*/
				/* pTmp = (PUCHAR) pEid->Octet;*/
				pTmp   += 11;

				/* 
					Cipher Suite Selectors from Spec P802.11i/D3.2 P26.
					Value	   Meaning
					0			None
					1			WEP-40
					2			Tkip
					3			WRAP
					4			AES
					5			WEP-104
				*/
				/* Parse group cipher*/
				switch (*pTmp)
				{
					case 1:
						pBss->WPA.GroupCipher = Ndis802_11GroupWEP40Enabled;
						break;
					case 5:
						pBss->WPA.GroupCipher = Ndis802_11GroupWEP104Enabled;
						break;
					case 2:
						pBss->WPA.GroupCipher = Ndis802_11TKIPEnable;
						break;
					case 4:
						pBss->WPA.GroupCipher = Ndis802_11AESEnable;
						break;
					default:
						break;
				}
				/* number of unicast suite*/
				pTmp   += 1;

#ifdef WPA3_SUPPORT
				/* skip all unicast cipher suites*/
				/*Count = *(PUSHORT) pTmp;				*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);
				pBss->WPA.u4PairwiseCipherCount = (Count > MAX_NUM_SUPPORTED_CIPHER_SUITES)?MAX_NUM_SUPPORTED_CIPHER_SUITES:Count;
				TmpCount = 0;

				/* Parsing all unicast cipher suite*/
				while (Count > 0) {
					/* Skip OUI*/
					pTmp += 3;
					switch (*pTmp) {
					case 1:
					case 5: /* Although WEP is not allowed in WPA related auth mode, we parse it anyway*/
							pBss->WPA.PairCipher[TmpCount] = Ndis802_11Encryption1Enabled;
							break;
					case 2:
							pBss->WPA.PairCipher[TmpCount] = Ndis802_11Encryption2Enabled;
							break;
					case 4:
							pBss->WPA.PairCipher[TmpCount] = Ndis802_11Encryption3Enabled;
							break;
					default:
							break;
					}
					pTmp++;
					Count--;
					TmpCount++;
				}
				/* sort the PairCipher, PairCipher[0] would be the max*/
				for (Count = 0; Count < (pBss->WPA.u4PairwiseCipherCount-1); Count++) {
					MaxCount = Count;
					for (TmpCount = (Count+1); TmpCount < pBss->WPA.u4PairwiseCipherCount; TmpCount++) {
						if (pBss->WPA.PairCipher[TmpCount] > pBss->WPA.PairCipher[MaxCount])
							MaxCount = TmpCount;
					}
					TmpCipher = pBss->WPA.PairCipher[Count];
					pBss->WPA.PairCipher[Count] = pBss->WPA.PairCipher[MaxCount];
					pBss->WPA.PairCipher[MaxCount] = TmpCipher;
				}

				/* 4. get AKM suite counts*/
				/*Count	= *(PUSHORT) pTmp;*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);
				pBss->u4AuthModeCount = (Count > MAX_NUM_SUPPORTED_AKM_SUITES)?MAX_NUM_SUPPORTED_AKM_SUITES:Count;
				pBss->WPA.u4AuthKeyMgtSuiteCount = pBss->u4AuthModeCount;
				TmpCount = 0;

				/* Parsing all AKM suite List */
				while (Count > 0) {
					/* Skip OUI*/
					pTmp += 3;
					switch (*pTmp) {
					case 1:
							/* Set AP support WPA-enterprise mode*/
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA;
							pBss->WPA.au4AuthKeyMgtSuite[TmpCount] = WPA_AKM_SUITE_802_1X;
							break;
					case 2:
							/* Set AP support WPA-PSK mode*/
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPAPSK;
							pBss->WPA.au4AuthKeyMgtSuite[TmpCount] = WPA_AKM_SUITE_PSK;
							break;
					default:
							break;
					}
					pTmp++;
					Count--;
					TmpCount++;
				}
				/* sort the AuthMode, AuthMode[0] would be the max*/
				for (Count = 0; Count < (pBss->u4AuthModeCount-1); Count++) {
					MaxCount = Count;
					for (TmpCount = (Count+1); TmpCount < pBss->u4AuthModeCount; TmpCount++) {
						if (pBss->AuthMode[TmpCount] > pBss->AuthMode[MaxCount])
							MaxCount = TmpCount;
					}
					TmpAuthMode = pBss->AuthMode[Count];
					pBss->AuthMode[Count] = pBss->AuthMode[MaxCount];
					pBss->AuthMode[MaxCount] = TmpAuthMode;
					TmpAKMSuite = pBss->WPA.au4AuthKeyMgtSuite[Count];
					pBss->WPA.au4AuthKeyMgtSuite[Count] = pBss->WPA.au4AuthKeyMgtSuite[MaxCount];
					pBss->WPA.au4AuthKeyMgtSuite[MaxCount] = TmpAKMSuite;
				}

				pTmp   += 1;
				/* Fixed for WPA-None*/
				if (pBss->BssType == BSS_ADHOC) {
					for (Count = 0; Count < pBss->u4AuthModeCount; Count++)
						pBss->AuthMode[Count]	  = Ndis802_11AuthModeWPANone;
					pBss->WepStatus   = pBss->WPA.GroupCipher;
					/* Patched bugs for old driver*/
					for (Count = 0; Count < pBss->WPA.u4PairwiseCipherCount; Count++)
						if (pBss->WPA.PairCipher[Count] == Ndis802_11WEPDisabled)
							pBss->WPA.PairCipher[Count] = pBss->WPA.GroupCipher;
				} else
					pBss->WepStatus   = pBss->WPA.PairCipher[0];

				/* Check the Pair & Group, if different, turn on mixed mode flag*/
				if (pBss->WPA.GroupCipher != pBss->WPA.PairCipher[0])
					pBss->WPA.bMixMode = TRUE;
#else /* WPA3_SUPPORT */
				/* skip all unicast cipher suites*/
				/*Count = *(PUSHORT) pTmp;				*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);

				/* Parsing all unicast cipher suite*/
				while (Count > 0)
				{
					/* Skip OUI*/
					pTmp += 3;
					TmpCipher = Ndis802_11WEPDisabled;
					switch (*pTmp)
					{
						case 1:
						case 5: /* Although WEP is not allowed in WPA related auth mode, we parse it anyway*/
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
					if (TmpCipher > pBss->WPA.PairCipher)
					{
						/* Move the lower cipher suite to PairCipherAux*/
						pBss->WPA.PairCipherAux = pBss->WPA.PairCipher;
						pBss->WPA.PairCipher	= TmpCipher;
					}
					else
					{
						pBss->WPA.PairCipherAux = TmpCipher;
					}
					pTmp++;
					Count--;
				}
				
				/* 4. get AKM suite counts*/
				/*Count	= *(PUSHORT) pTmp;*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);
				pTmp   += 3;
				
				switch (*pTmp)
				{
					case 1:
						/* Set AP support WPA-enterprise mode*/
						if (pBss->AuthMode == Ndis802_11AuthModeOpen)
							pBss->AuthMode = Ndis802_11AuthModeWPA;
						else
							pBss->AuthModeAux = Ndis802_11AuthModeWPA;
						break;
					case 2:
						/* Set AP support WPA-PSK mode*/
						if (pBss->AuthMode == Ndis802_11AuthModeOpen)
							pBss->AuthMode = Ndis802_11AuthModeWPAPSK;
						else
							pBss->AuthModeAux = Ndis802_11AuthModeWPAPSK;
						break;
					default:
						break;
				}
				pTmp   += 1;

				/* Fixed for WPA-None*/
				if (pBss->BssType == BSS_ADHOC)
				{
					pBss->AuthMode	  = Ndis802_11AuthModeWPANone;
					pBss->AuthModeAux = Ndis802_11AuthModeWPANone;
					pBss->WepStatus   = pBss->WPA.GroupCipher;
					/* Patched bugs for old driver*/
					if (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled)
						pBss->WPA.PairCipherAux = pBss->WPA.GroupCipher;
				}
				else
					pBss->WepStatus   = pBss->WPA.PairCipher;					
				
				/* Check the Pair & Group, if different, turn on mixed mode flag*/
				if (pBss->WPA.GroupCipher != pBss->WPA.PairCipher)
					pBss->WPA.bMixMode = TRUE;
#endif /* WPA3_SUPPORT */
				break;

			case IE_RSN:
				pRsnHeader = (PRSN_IE_HEADER_STRUCT) pTmp;
				
				/* 0. Version must be 1*/
				if (le2cpu16(pRsnHeader->Version) != 1)
					break;
				pTmp   += sizeof(RSN_IE_HEADER_STRUCT);

				/* 1. Check group cipher*/
				pCipher = (PCIPHER_SUITE_STRUCT) pTmp;
				if (!RTMPEqualMemory(pTmp, RSN_OUI, 3))
					break;

				/* Parse group cipher*/
				switch (pCipher->Type)
				{
					case 1:
						pBss->WPA2.GroupCipher = Ndis802_11GroupWEP40Enabled;
						break;
					case 5:
						pBss->WPA2.GroupCipher = Ndis802_11GroupWEP104Enabled;
						break;
					case 2:
						pBss->WPA2.GroupCipher = Ndis802_11TKIPEnable;
						break;
					case 4:
						pBss->WPA2.GroupCipher = Ndis802_11AESEnable;
						break;
					default:
						break;
				}
				/* set to correct offset for next parsing*/
				pTmp   += sizeof(CIPHER_SUITE_STRUCT);

#ifdef WPA3_SUPPORT
				/* 2. Get pairwise cipher counts*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);
				pBss->WPA2.u4PairwiseCipherCount = (Count > MAX_NUM_SUPPORTED_CIPHER_SUITES)?MAX_NUM_SUPPORTED_CIPHER_SUITES:Count;
				TmpCount = 0;

				/* 3. Get pairwise cipher*/
				/* Parsing all unicast cipher suite*/
				while (Count > 0) {
					/* Skip OUI*/
					pTmp += 3;
					switch (*pTmp) {
					case 1:
					case 5: /* Although WEP is not allowed in WPA related auth mode, we parse it anyway*/
							pBss->WPA2.PairCipher[TmpCount] = Ndis802_11Encryption1Enabled;
							break;
					case 2:
							pBss->WPA2.PairCipher[TmpCount] = Ndis802_11Encryption2Enabled;
							break;
					case 4:
							pBss->WPA2.PairCipher[TmpCount] = Ndis802_11Encryption3Enabled;
							break;
					default:
							break;
					}
					pTmp++;
					Count--;
					TmpCount++;
				}
				/* sort the PairCipher, PairCipher[0] would be the max*/
				for (Count = 0; Count < (pBss->WPA2.u4PairwiseCipherCount-1); Count++) {
					MaxCount = Count;
					for (TmpCount = (Count+1); TmpCount < pBss->WPA2.u4PairwiseCipherCount; TmpCount++)
						if (pBss->WPA2.PairCipher[TmpCount] > pBss->WPA2.PairCipher[MaxCount])
							MaxCount = TmpCount;
					TmpCipher = pBss->WPA2.PairCipher[Count];
					pBss->WPA2.PairCipher[Count] = pBss->WPA2.PairCipher[MaxCount];
					pBss->WPA2.PairCipher[MaxCount] = TmpCipher;
				}

				/* 4. get AKM suite counts*/
				/*Count	= *(PUSHORT) pTmp;*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);
				pBss->u4AuthModeCount = (Count > MAX_NUM_SUPPORTED_AKM_SUITES)?MAX_NUM_SUPPORTED_AKM_SUITES:Count;
				pBss->WPA2.u4AuthKeyMgtSuiteCount = pBss->u4AuthModeCount;
				TmpCount = 0;

				/* 5. Get AKM ciphers*/
				/* Parsing all AKM suite List */
				while (Count > 0) {
					/* Skip OUI*/
					pAKM = (PAKM_SUITE_STRUCT) pTmp;
					if (!RTMPEqualMemory(pTmp, RSN_OUI, 3))
						break;

					switch (pAKM->Type) {
					case 0:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPANone;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_NONE;
							break;
					case 1:
#ifdef DOT11R_FT_SUPPORT
					case 3:
#endif /* DOT11R_FT_SUPPORT */
							/* Set AP support WPA-enterprise mode*/
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_802_1X;
							break;
					case 2:
#ifdef DOT11R_FT_SUPPORT
					case 4:
#endif /* DOT11R_FT_SUPPORT */
							/* Set AP support WPA-PSK mode*/
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2PSK;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_PSK;
							break;
#ifdef CFG_SUPPORT_802_11W
					case 5:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_802_1X_SHA256;
							break;
					case 6:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2PSK;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_PSK_SHA256;
							pBss->IsSupportSHA256KeyDerivation = TRUE;
							break;
#endif
#ifdef CFG_SUPPORT_SAE
					case 8:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA3SAE;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_SAE;
							break;
#endif
#ifdef CFG_SUPPORT_SUITB
					case 11:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2PSK;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_8021X_SUITE_B;
							break;
#endif
#ifdef CFG_SUPPORT_SUITB_192
					case 12:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2PSK;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_8021X_SUITE_B_192;
							break;
#endif
#ifdef CFG_SUPPORT_OWE
					case 18:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeWPA2PSK;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_OWE;
							break;
#endif
					default:
							pBss->AuthMode[TmpCount] = Ndis802_11AuthModeMax;
							pBss->WPA2.au4AuthKeyMgtSuite[TmpCount] = RSN_AKM_SUITE_NONE;
							break;
					}
					pTmp   += sizeof(AKM_SUITE_STRUCT);
					Count--;
					TmpCount++;
				}
				/* sort the AuthMode, AuthMode[0] would be the max*/
				for (Count = 0; Count < (pBss->u4AuthModeCount-1); Count++) {
					MaxCount = Count;
					for (TmpCount = (Count+1); TmpCount < pBss->u4AuthModeCount; TmpCount++) {
						if (pBss->AuthMode[TmpCount] > pBss->AuthMode[MaxCount])
							MaxCount = TmpCount;
					}
					TmpAuthMode = pBss->AuthMode[Count];
					pBss->AuthMode[Count] = pBss->AuthMode[MaxCount];
					pBss->AuthMode[MaxCount] = TmpAuthMode;
					TmpAKMSuite = pBss->WPA2.au4AuthKeyMgtSuite[Count];
					pBss->WPA2.au4AuthKeyMgtSuite[Count] = pBss->WPA2.au4AuthKeyMgtSuite[MaxCount];
					pBss->WPA2.au4AuthKeyMgtSuite[MaxCount] = TmpAKMSuite;
				}

				/* Fixed for WPA-None*/
				if (pBss->BssType == BSS_ADHOC) {
					pBss->WPA.GroupCipher	= pBss->WPA2.GroupCipher;
					pBss->WPA.u4PairwiseCipherCount = 1;
					/* Patched bugs for old driver*/
					if (pBss->WPA.PairCipher[0] == Ndis802_11WEPDisabled)
						pBss->WPA.PairCipher[0] = pBss->WPA.GroupCipher;
					pBss->WepStatus   = pBss->WPA.PairCipher[0];
				} else
					pBss->WepStatus   = pBss->WPA2.PairCipher[0];

				/* 6. Get RSN capability*/
				/*pBss->WPA2.RsnCapability = *(PUSHORT) pTmp;*/
				pBss->WPA2.RsnCapability = (pTmp[1]<<8) + pTmp[0];
				pTmp += sizeof(USHORT);

				/* 7. Get PmkidCount */
				Count = (pTmp[1]<<8) + pTmp[0];
				pBss->WPA2.u2PmkidCnt = (Count > MAX_NUM_SUPPORTED_PMKID)?MAX_NUM_SUPPORTED_PMKID:Count;
				pTmp += sizeof(USHORT);

				/* 8. Parsing Pmkid List*/
				NdisCopyMemory(pBss->WPA2.aucPmkidList, pTmp, (pBss->WPA2.u2PmkidCnt * RSN_PMKID_LEN));
				pTmp += sizeof(UCHAR) * (pBss->WPA2.u2PmkidCnt * RSN_PMKID_LEN);

				/* 9. Get u4GroupMgmtCipherSuite */
				pBss->WPA2.u4GroupMgmtKeyCipherSuite = (pTmp[3]<<24) + (pTmp[2]<<16) + (pTmp[1]<<8) + pTmp[0];
				pTmp += sizeof(UINT);

				/* Check the Pair & Group, if different, turn on mixed mode flag*/
				if (pBss->WPA2.GroupCipher != pBss->WPA2.PairCipher[0])
					pBss->WPA2.bMixMode = TRUE;

#else /* WPA3_SUPPORT */
				/* 2. Get pairwise cipher counts*/
				/*Count = *(PUSHORT) pTmp;*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);			

				/* 3. Get pairwise cipher*/
				/* Parsing all unicast cipher suite*/
				while (Count > 0)
				{
					/* Skip OUI*/
					pCipher = (PCIPHER_SUITE_STRUCT) pTmp;
					TmpCipher = Ndis802_11WEPDisabled;
					switch (pCipher->Type)
					{
						case 1:
						case 5: /* Although WEP is not allowed in WPA related auth mode, we parse it anyway*/
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
					if (TmpCipher > pBss->WPA2.PairCipher)
					{
						/* Move the lower cipher suite to PairCipherAux*/
						pBss->WPA2.PairCipherAux = pBss->WPA2.PairCipher;
						pBss->WPA2.PairCipher	 = TmpCipher;
					}
					else
					{
						pBss->WPA2.PairCipherAux = TmpCipher;
					}
					pTmp += sizeof(CIPHER_SUITE_STRUCT);
					Count--;
				}
				
				/* 4. get AKM suite counts*/
				/*Count	= *(PUSHORT) pTmp;*/
				Count = (pTmp[1]<<8) + pTmp[0];
				pTmp   += sizeof(USHORT);

				if (Count > 1)
					fgMultiAKMs = TRUE;

				/* 5. Get AKM ciphers*/
				/* Parsing all AKM ciphers*/
				while (Count > 0)
				{					
					pAKM = (PAKM_SUITE_STRUCT) pTmp;
					if (!RTMPEqualMemory(pTmp, RSN_OUI, 3))
						break;

					switch (pAKM->Type)
					{
						case 0:
							if (pBss->AuthMode == Ndis802_11AuthModeOpen)
								pBss->AuthMode = Ndis802_11AuthModeWPANone;
							else
								pBss->AuthModeAux = Ndis802_11AuthModeWPANone;
							break;                                                        
						case 1:
#ifdef DOT11R_FT_SUPPORT
						case 3:
#endif /* DOT11R_FT_SUPPORT */
							/* Set AP support WPA-enterprise mode*/
							if (pBss->AuthMode == Ndis802_11AuthModeOpen)
								pBss->AuthMode = Ndis802_11AuthModeWPA2;
							else
								pBss->AuthModeAux = Ndis802_11AuthModeWPA2;
							break;
						case 2:
#ifdef DOT11R_FT_SUPPORT
						case 4:
#endif /* DOT11R_FT_SUPPORT */
#ifdef DOT11W_PMF_SUPPORT
						case 6:
#endif /* DOT11W_PMF_SUPPORT */                                                        
							/* Set AP support WPA-PSK mode*/
							if (pBss->AuthMode == Ndis802_11AuthModeOpen)
								pBss->AuthMode = Ndis802_11AuthModeWPA2PSK;
							else
								pBss->AuthModeAux = Ndis802_11AuthModeWPA2PSK;

#ifdef DOT11W_PMF_SUPPORT
                                                        if (pAKM->Type == 6)
                                                                pBss->IsSupportSHA256KeyDerivation = TRUE;
#endif /* DOT11W_PMF_SUPPORT */

							break;
						default:
							if (fgMultiAKMs)
								break;

							if (pBss->AuthMode == Ndis802_11AuthModeOpen)
								pBss->AuthMode = Ndis802_11AuthModeMax;
							else
								pBss->AuthModeAux = Ndis802_11AuthModeMax;
							break;
					}
					pTmp   += sizeof(AKM_SUITE_STRUCT);
					Count--;
				}

				/* Fixed for WPA-None*/
				if (pBss->BssType == BSS_ADHOC)
				{
					pBss->WPA.PairCipherAux = pBss->WPA2.PairCipherAux;
					pBss->WPA.GroupCipher	= pBss->WPA2.GroupCipher;
					pBss->WepStatus 		= pBss->WPA.GroupCipher;
					/* Patched bugs for old driver*/
					if (pBss->WPA.PairCipherAux == Ndis802_11WEPDisabled)
						pBss->WPA.PairCipherAux = pBss->WPA.GroupCipher;
				}
				pBss->WepStatus   = pBss->WPA2.PairCipher;					
				
				/* 6. Get RSN capability*/
				/*pBss->WPA2.RsnCapability = *(PUSHORT) pTmp;*/
				pBss->WPA2.RsnCapability = (pTmp[1]<<8) + pTmp[0];
				pTmp += sizeof(USHORT);
				
				/* Check the Pair & Group, if different, turn on mixed mode flag*/
				if (pBss->WPA2.GroupCipher != pBss->WPA2.PairCipher)
					pBss->WPA2.bMixMode = TRUE;
#endif /* WPA3_SUPPORT */
				break;
#ifdef WAPI_SUPPORT
			case IE_WAPI:
#if 0
				{
					int c = 0;
					printk("wapi_ie(len-%d):",Length);
					for (c = 0; c < Length; c++)
						printk("%02x-", (*pTmp+c));
					printk("\n");
				}
#endif 				
				pRsnHeader = (PRSN_IE_HEADER_STRUCT) pTmp;

				/* 0. The version number must be 1*/
				if (le2cpu16(pRsnHeader->Version) != 1)
					break;
				pTmp += sizeof(RSN_IE_HEADER_STRUCT);

				/* 1. Get AKM suite counts*/
				NdisMoveMemory(&Count, pTmp, sizeof(USHORT));	
    			Count = cpu2le16(Count);				
				pTmp += sizeof(USHORT);
#ifdef WPA3_SUPPORT
				pBss->u4AuthModeCount = Count;
#endif

				/* 2. Get AKM ciphers*/
				pAKM = (PAKM_SUITE_STRUCT) pTmp;
				if (!RTMPEqualMemory(pTmp, WAPI_OUI, 3))
					break;

				switch (pAKM->Type)
				{					
					case 1:
						/* Support WAI certificate authentication*/
#ifdef WPA3_SUPPORT
						pBss->AuthMode[0] = Ndis802_11AuthModeWAICERT;
#else
						pBss->AuthMode = Ndis802_11AuthModeWAICERT;
#endif
						break;
					case 2:
#ifdef WPA3_SUPPORT
						/* Support WAI PSK*/
						pBss->AuthMode[0] = Ndis802_11AuthModeWAIPSK;
#else
						/* Support WAI PSK*/
						pBss->AuthMode = Ndis802_11AuthModeWAIPSK;
#endif
						break;
					default:
						break;
				}
				pTmp += (Count * sizeof(AKM_SUITE_STRUCT));

				/* 3. Get pairwise cipher counts*/
				NdisMoveMemory(&Count, pTmp, sizeof(USHORT));	
    			Count = cpu2le16(Count);	
				pTmp += sizeof(USHORT);

#ifdef WPA3_SUPPORT
				pBss->WAPI.u4PairwiseCipherCount = Count;
				TmpCount = 0;

				/* 4. Get pairwise cipher*/
				/* Parsing all unicast cipher suite*/
				while (Count > 0) {

					if (!RTMPEqualMemory(pTmp, WAPI_OUI, 3))
						break;

					switch (*pTmp) {
					case 1:
						pBss->WPAI.PairCipher[TmpCount] = Ndis802_11EncryptionSMS4Enabled;
						break;
					default:
						break;
					}
					pTmp += sizeof(CIPHER_SUITE_STRUCT);
					Count--;
					TmpCount++;
				}
				/* sort the PairCipher, PairCipher[0] would be the max*/
				for (Count = 0; Count < (pBss->WPAI.u4PairwiseCipherCount-1); Count++) {
					MaxCount = Count;
					for (TmpCount = (Count+1); TmpCount < pBss->WPAI.u4PairwiseCipherCount; TmpCount++)
						if (pBss->WPAI.PairCipher[TmpCount] > pBss->WPAI.PairCipher[MaxCount])
							MaxCount = TmpCount;
					TmpCipher = pBss->WPAI.PairCipher[Count];
					pBss->WPAI.PairCipher[Count] = pBss->WPAI.PairCipher[MaxCount];
					pBss->WPAI.PairCipher[MaxCount] = TmpCipher;
				}
#else
				/* 4. Get pairwise cipher*/
				/* Parsing all unicast cipher suite*/
				while (Count > 0)
				{
					if (!RTMPEqualMemory(pTmp, WAPI_OUI, 3))
						break;
				
					/* Skip OUI*/
					pCipher = (PCIPHER_SUITE_STRUCT) pTmp;					
					TmpCipher = Ndis802_11WEPDisabled;
					switch (pCipher->Type)
					{
						case 1:						
							TmpCipher = Ndis802_11EncryptionSMS4Enabled;
							break;
						default:
							break;
					}
					
					if (TmpCipher > pBss->WAPI.PairCipher)
					{
						/* Move the lower cipher suite to PairCipherAux*/
						pBss->WAPI.PairCipherAux = pBss->WAPI.PairCipher;
						pBss->WAPI.PairCipher	 = TmpCipher;
					}
					else
					{
						pBss->WAPI.PairCipherAux = TmpCipher;
					}
					pTmp += sizeof(CIPHER_SUITE_STRUCT);
					Count--;
				}
#endif
				/* 5. Check group cipher*/
				if (!RTMPEqualMemory(pTmp, WAPI_OUI, 3))
					break;
				
				pCipher = (PCIPHER_SUITE_STRUCT) pTmp;				
				/* Parse group cipher*/
				switch (pCipher->Type)
				{
					case 1:
						pBss->WAPI.GroupCipher = Ndis802_11EncryptionSMS4Enabled;
						break;
					default:
						break;
				}
				/* set to correct offset for next parsing*/
				pTmp += sizeof(CIPHER_SUITE_STRUCT);

				/* update the encryption type*/
				pBss->WepStatus = pBss->WAPI.PairCipher;

				/* update the WAPI capability*/
				pBss->WAPI.RsnCapability = (pTmp[1]<<8) + pTmp[0];
				pTmp += sizeof(USHORT);

				break;
#endif /* WAPI_SUPPORT */				
			default:
				break;
		}
		Length -= (pEid->Len + 2);
	}
}


/*! \brief generates a random mac address value for IBSS BSSID
 *	\param Addr the bssid location
 *	\return none
 *	\pre
 *	\post
 */
VOID MacAddrRandomBssid(RTMP_ADAPTER *pAd, UCHAR *pAddr) 
{
	INT i;

	for (i = 0; i < MAC_ADDR_LEN; i++) 
	{
		pAddr[i] = RandomByte(pAd);
	}

	pAddr[0] = (pAddr[0] & 0xfe) | 0x02;  /* the first 2 bits must be 01xxxxxxxx*/
}


/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
*/
VOID AssocParmFill(
	IN PRTMP_ADAPTER pAd,
	IN OUT MLME_ASSOC_REQ_STRUCT *AssocReq,
	IN PUCHAR                     pAddr,
	IN USHORT                     CapabilityInfo,
	IN ULONG                      Timeout,
	IN USHORT                     ListenIntv)
{
	COPY_MAC_ADDR(AssocReq->Addr, pAddr);
	/* Add mask to support 802.11b mode only */
	AssocReq->CapabilityInfo = CapabilityInfo & SUPPORTED_CAPABILITY_INFO; /* not cf-pollable, not cf-poll-request*/
	AssocReq->Timeout = Timeout;
	AssocReq->ListenIntv = ListenIntv;
}


/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
*/
VOID DisassocParmFill(
	IN PRTMP_ADAPTER pAd,
	IN OUT MLME_DISASSOC_REQ_STRUCT *DisassocReq,
	IN PUCHAR pAddr,
	IN USHORT Reason)
{
	COPY_MAC_ADDR(DisassocReq->Addr, pAddr);
	DisassocReq->Reason = Reason;
}


/*! \brief init the management mac frame header
 *	\param p_hdr mac header
 *	\param subtype subtype of the frame
 *	\param p_ds destination address, don't care if it is a broadcast address
 *	\return none
 *	\pre the station has the following information in the pAd->StaCfg
 *	 - bssid
 *	 - station address
 *	\post
 *	\note this function initializes the following field
 */
VOID MgtMacHeaderInit(
	IN RTMP_ADAPTER *pAd, 
	INOUT HEADER_802_11 *pHdr80211, 
	IN UCHAR SubType, 
	IN UCHAR ToDs, 
	IN UCHAR *pDA, 
	IN UCHAR *pSA,
	IN UCHAR *pBssid) 
{
	NdisZeroMemory(pHdr80211, sizeof(HEADER_802_11));
	
	pHdr80211->FC.Type = FC_TYPE_MGMT;
	pHdr80211->FC.SubType = SubType;
	pHdr80211->FC.ToDs = ToDs;
	COPY_MAC_ADDR(pHdr80211->Addr1, pDA);
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
		COPY_MAC_ADDR(pHdr80211->Addr2, pSA);
#else
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		COPY_MAC_ADDR(pHdr80211->Addr2, pBssid);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		COPY_MAC_ADDR(pHdr80211->Addr2, pAd->CurrentAddress);
#endif /* CONFIG_STA_SUPPORT */
#endif /* P2P_SUPPORT */
	COPY_MAC_ADDR(pHdr80211->Addr3, pBssid);
}


VOID MgtMacHeaderInitExt(
    IN RTMP_ADAPTER *pAd,
    IN OUT HEADER_802_11 *pHdr80211,
    IN UCHAR SubType,
    IN UCHAR ToDs,
    IN UCHAR *pAddr1,
    IN UCHAR *pAddr2,
    IN UCHAR *pAddr3)
{
    NdisZeroMemory(pHdr80211, sizeof(HEADER_802_11));

    pHdr80211->FC.Type = FC_TYPE_MGMT;
    pHdr80211->FC.SubType = SubType;
    pHdr80211->FC.ToDs = ToDs;
    COPY_MAC_ADDR(pHdr80211->Addr1, pAddr1);
    COPY_MAC_ADDR(pHdr80211->Addr2, pAddr2);
    COPY_MAC_ADDR(pHdr80211->Addr3, pAddr3);
}


/*!***************************************************************************
 * This routine build an outgoing frame, and fill all information specified 
 * in argument list to the frame body. The actual frame size is the summation 
 * of all arguments.
 * input params:
 *		Buffer - pointer to a pre-allocated memory segment
 *		args - a list of <int arg_size, arg> pairs.
 *		NOTE NOTE NOTE!!!! the last argument must be NULL, otherwise this
 *						   function will FAIL!!!
 * return:
 *		Size of the buffer
 * usage:  
 *		MakeOutgoingFrame(Buffer, output_length, 2, &fc, 2, &dur, 6, p_addr1, 6,p_addr2, END_OF_ARGS);

 IRQL = PASSIVE_LEVEL
 IRQL = DISPATCH_LEVEL
  
 ****************************************************************************/
ULONG MakeOutgoingFrame(UCHAR *Buffer, ULONG *FrameLen, ...) 
{
	UCHAR   *p;
	int 	leng;
	ULONG	TotLeng;
	va_list Args;

	/* calculates the total length*/
	TotLeng = 0;
	va_start(Args, FrameLen);
	do 
	{
		leng = va_arg(Args, int);
		if (leng == END_OF_ARGS) 
		{
			break;
		}
		p = va_arg(Args, PVOID);
		NdisMoveMemory(&Buffer[TotLeng], p, leng);
		TotLeng = TotLeng + leng;
	} while(TRUE);

	va_end(Args); /* clean up */
	*FrameLen = TotLeng;
	return TotLeng;
}


/*! \brief	Initialize The MLME Queue, used by MLME Functions
 *	\param	*Queue	   The MLME Queue
 *	\return Always	   Return NDIS_STATE_SUCCESS in this implementation
 *	\pre
 *	\post
 *	\note	Because this is done only once (at the init stage), no need to be locked

 IRQL = PASSIVE_LEVEL
 
 */
NDIS_STATUS MlmeQueueInit(RTMP_ADAPTER *pAd, MLME_QUEUE *Queue) 
{
	INT i;

	NdisAllocateSpinLock(pAd, &Queue->Lock);

	Queue->Num	= 0;
	Queue->Head = 0;
	Queue->Tail = 0;

	for (i = 0; i < MAX_LEN_OF_MLME_QUEUE; i++) 
	{
		Queue->Entry[i].Occupied = FALSE;
		Queue->Entry[i].MsgLen = 0;
		NdisZeroMemory(Queue->Entry[i].Msg, MGMT_DMA_BUFFER_SIZE);
	}

	return NDIS_STATUS_SUCCESS;
}


/*! \brief	 Enqueue a message for other threads, if they want to send messages to MLME thread
 *	\param	*Queue	  The MLME Queue
 *	\param	 Machine  The State Machine Id
 *	\param	 MsgType  The Message Type
 *	\param	 MsgLen   The Message length
 *	\param	*Msg	  The message pointer
 *	\return  TRUE if enqueue is successful, FALSE if the queue is full
 *	\pre
 *	\post
 *	\note	 The message has to be initialized
 */
BOOLEAN MlmeEnqueue(
	IN RTMP_ADAPTER *pAd,
	IN ULONG Machine,
	IN ULONG MsgType,
	IN ULONG MsgLen,
	IN VOID *Msg,
	IN ULONG Priv)
{
	INT Tail;
	MLME_QUEUE	*Queue = (MLME_QUEUE *)&pAd->Mlme.Queue;

	/* Do nothing if the driver is starting halt state.*/
	/* This might happen when timer already been fired before cancel timer with mlmehalt*/
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return FALSE;

	/* First check the size, it MUST not exceed the mlme queue size*/
	if (MsgLen > MGMT_DMA_BUFFER_SIZE)
	{
		DBGPRINT_ERR(("MlmeEnqueue: msg too large, size = %ld \n", MsgLen));
		return FALSE;
	}
	
	if (MlmeQueueFull(Queue, 1)) 
	{
#ifdef RELEASE_EXCLUDE
		/* Can't add debug print here. It'll make Mlme Stat machine freeze.*/
		DBGPRINT(RT_DEBUG_INFO, ("%s():full, msg dropped and may corrupt MLME\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */		
		return FALSE;
	}

	NdisAcquireSpinLock(&(Queue->Lock));
	Tail = Queue->Tail;
	/*
		Double check for safety in multi-thread system.
	*/
	if (Queue->Entry[Tail].Occupied)
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("%s():full#2, msg dropped and may corrupt MLME\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */		
		NdisReleaseSpinLock(&(Queue->Lock));
		return FALSE;
	}
	Queue->Tail++;
	Queue->Num++;
	if (Queue->Tail == MAX_LEN_OF_MLME_QUEUE) 
		Queue->Tail = 0;
	
	Queue->Entry[Tail].Wcid = RESERVED_WCID;
	Queue->Entry[Tail].Occupied = TRUE;
	Queue->Entry[Tail].Machine = Machine;
	Queue->Entry[Tail].MsgType = MsgType;
	Queue->Entry[Tail].MsgLen = MsgLen;	
	Queue->Entry[Tail].Priv = Priv;
	
	if (Msg != NULL)
	{
		NdisMoveMemory(Queue->Entry[Tail].Msg, Msg, MsgLen);
	}
		
	NdisReleaseSpinLock(&(Queue->Lock));
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("MlmeEnqueue, num=%ld\n",Queue->Num));
#endif /* RELEASE_EXCLUDE */

	return TRUE;
}


/*! \brief	 This function is used when Recv gets a MLME message
 *	\param	*Queue			 The MLME Queue
 *	\param	 TimeStampHigh	 The upper 32 bit of timestamp
 *	\param	 TimeStampLow	 The lower 32 bit of timestamp
 *	\param	 Rssi			 The receiving RSSI strength
 *	\param	 MsgLen 		 The length of the message
 *	\param	*Msg			 The message pointer
 *	\return  TRUE if everything ok, FALSE otherwise (like Queue Full)
 *	\pre
 *	\post
 */
BOOLEAN MlmeEnqueueForRecv(
	IN RTMP_ADAPTER *pAd, 
	IN ULONG Wcid, 
	IN struct raw_rssi_info *rssi_info,
	IN ULONG MsgLen, 
	IN VOID *Msg,
	IN UCHAR OpMode)
{
	INT Tail, Machine = 0xff;
	UINT32 TimeStampHigh, TimeStampLow;
	PFRAME_802_11 pFrame = (PFRAME_802_11)Msg;
	INT MsgType = 0x0;
	MLME_QUEUE *Queue = (MLME_QUEUE *)&pAd->Mlme.Queue;
#ifdef APCLI_SUPPORT
	UCHAR ApCliIdx = 0;
#endif /* APCLI_SUPPORT */
#ifdef MAC_REPEATER_SUPPORT
	UCHAR CliIdx = 0;
#endif /* MAC_REPEATER_SUPPORT */


#ifdef CONFIG_ATE			
	/* Nothing to do in ATE mode */
	if(ATE_ON(pAd))
		return FALSE;
#endif /* CONFIG_ATE */

	/*
		Do nothing if the driver is starting halt state.
		This might happen when timer already been fired before cancel timer with mlmehalt
	*/
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		DBGPRINT_ERR(("%s(): fRTMP_ADAPTER_HALT_IN_PROGRESS\n", __FUNCTION__));
		return FALSE;
	}

	/* First check the size, it MUST not exceed the mlme queue size*/
	if (MsgLen > MGMT_DMA_BUFFER_SIZE)
	{
		DBGPRINT_ERR(("%s(): frame too large, size = %ld \n", __FUNCTION__, MsgLen));
		return FALSE;
	}

	if (MlmeQueueFull(Queue, 0)) 
	{
#ifdef RELEASE_EXCLUDE
		/* Can't add debug print here. It'll make Mlme Stat machine freeze.*/
		DBGPRINT(RT_DEBUG_INFO, ("%s(): full and dropped\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
		RTMP_MLME_HANDLER(pAd);

		return FALSE;
	}

#ifdef CONFIG_AP_SUPPORT
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
	if (OpMode == OPMODE_AP)
#else
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
#endif /* P2P_SUPPORT || RT_CFG80211_P2P_SUPPORT */
	{
#ifdef P2P_SUPPORT
		if (P2P_CLI_ON(pAd))
		{
			if (pFrame->Hdr.FC.SubType == SUBTYPE_PROBE_REQ)
				return FALSE;
		}
#endif /* P2P_SUPPORT */

#ifdef APCLI_SUPPORT
		/*
			Beacon must be handled by ap-sync state machine.
			Probe-rsp must be handled by apcli-sync state machine.
			Those packets don't need to check its MAC address
		*/
		do
		{
			BOOLEAN bToApCli = FALSE;
			UCHAR i;
			/* 
			   1. When P2P GO On and receive Probe Response, preCheckMsgTypeSubset function will 
			      enquene Probe response to APCli sync state machine
			      Solution: when GO On skip preCheckMsgTypeSubset redirect to APMsgTypeSubst
			   2. When P2P Cli On and receive Probe Response, preCheckMsgTypeSubset function will
			      enquene Probe response to APCli sync state machine
			      Solution: handle MsgType == APCLI_MT2_PEER_PROBE_RSP on ApCli Sync state machine
			                when ApCli on idle state.
			*/
			for (i = 0; i < pAd->ApCfg.ApCliNum; i++)
			{
				if (MAC_ADDR_EQUAL(pAd->ApCfg.ApCliTab[i].MlmeAux.Bssid, pFrame->Hdr.Addr2))
				{
					bToApCli = TRUE;
					break;
				}
			}

			if (!MAC_ADDR_EQUAL(pFrame->Hdr.Addr1, pAd->CurrentAddress) &&
#ifdef P2P_SUPPORT
				!P2P_GO_ON(pAd) &&
#endif /* P2P_SUPPORT */
				preCheckMsgTypeSubset(pAd, pFrame, &Machine, &MsgType))
				break;

			if (!MAC_ADDR_EQUAL(pFrame->Hdr.Addr1, pAd->CurrentAddress) && bToApCli)
			{
				if (ApCliMsgTypeSubst(pAd, pFrame, &Machine, &MsgType))
					break;
			}
			else
			{
				if (APMsgTypeSubst(pAd, pFrame, &Machine, &MsgType))
					break;
			}

			DBGPRINT_ERR(("%s(): un-recongnized mgmt->subtype=%d, STA-%02x:%02x:%02x:%02x:%02x:%02x\n", 
						__FUNCTION__, pFrame->Hdr.FC.SubType, PRINT_MAC(pFrame->Hdr.Addr2)));
			return FALSE;

		} while (FALSE);
#else
		if (!APMsgTypeSubst(pAd, pFrame, &Machine, &MsgType))
		{
			DBGPRINT(RT_DEBUG_TRACE,("%s(): un-recongnized mgmt->subtype=%d\n",
							__FUNCTION__, pFrame->Hdr.FC.SubType));
			return FALSE;
		}
#endif /* APCLI_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */	
#ifdef CONFIG_STA_SUPPORT
#if defined(P2P_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)
	if (OpMode == OPMODE_STA)
#else
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
#endif /* P2P_SUPPORT */
	{
		if (!MsgTypeSubst(pAd, pFrame, &Machine, &MsgType)) 
		{
#ifdef WIDI_SUPPORT
			if (!pAd->StaCfg.WscControl.bWscTrigger)
#endif /* WIDI_SUPPORT */
			DBGPRINT(RT_DEBUG_TRACE,("%s(): un-recongnized mgmt->subtype=%d\n",
							__FUNCTION__, pFrame->Hdr.FC.SubType));
			return FALSE;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	TimeStampHigh = TimeStampLow = 0;
#ifdef RTMP_MAC_PCI
	if (!IS_USB_INF(pAd))
		AsicGetTsfTime(pAd, &TimeStampHigh, &TimeStampLow);
#endif /* RTMP_MAC_PCI */

	/* OK, we got all the informations, it is time to put things into queue*/
	NdisAcquireSpinLock(&(Queue->Lock));
	Tail = Queue->Tail;
	/*
		Double check for safety in multi-thread system.
	*/
	if (Queue->Entry[Tail].Occupied)
	{
		NdisReleaseSpinLock(&(Queue->Lock));
		return FALSE;
	}
	Queue->Tail++;
	Queue->Num++;
	if (Queue->Tail == MAX_LEN_OF_MLME_QUEUE) 
		Queue->Tail = 0;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s():num=%ld\n", __FUNCTION__, Queue->Num));
#endif /* RELEASE_EXCLUDE */
	Queue->Entry[Tail].Occupied = TRUE;
	Queue->Entry[Tail].Machine = Machine;
	Queue->Entry[Tail].MsgType = MsgType;
	Queue->Entry[Tail].MsgLen  = MsgLen;
	Queue->Entry[Tail].TimeStamp.u.LowPart = TimeStampLow;
	Queue->Entry[Tail].TimeStamp.u.HighPart = TimeStampHigh;
	NdisMoveMemory(&Queue->Entry[Tail].rssi_info, rssi_info, sizeof(struct raw_rssi_info));
	Queue->Entry[Tail].Signal = rssi_info->raw_snr;
	Queue->Entry[Tail].Wcid = (UCHAR)Wcid;
	Queue->Entry[Tail].OpMode = (UCHAR)OpMode;
	Queue->Entry[Tail].Channel = pAd->LatchRfRegs.Channel;
	Queue->Entry[Tail].Priv = 0;
#ifdef APCLI_SUPPORT
	Queue->Entry[Tail].Priv = ApCliIdx;
#endif /* APCLI_SUPPORT */
#ifdef MAC_REPEATER_SUPPORT
	if (pAd->ApCfg.bMACRepeaterEn)
	{
		for (CliIdx = 0; CliIdx < MAX_EXT_MAC_ADDR_SIZE; CliIdx++)
		{
			if (MAC_ADDR_EQUAL(pAd->ApCfg.ApCliTab[ApCliIdx].RepeaterCli[CliIdx].CurrentAddress, pFrame->Hdr.Addr1))
			{
				Queue->Entry[Tail].Priv = (64 + (MAX_EXT_MAC_ADDR_SIZE * ApCliIdx) + CliIdx);
				break;
			}
		}
	}
#endif /* MAC_REPEATER_SUPPORT */

	if (Msg != NULL)
	{
		NdisMoveMemory(Queue->Entry[Tail].Msg, Msg, MsgLen);
	}

	NdisReleaseSpinLock(&(Queue->Lock));	
	RTMP_MLME_HANDLER(pAd);

	return TRUE;
}


#ifdef WSC_INCLUDED
/*! \brief   Enqueue a message for other threads, if they want to send messages to MLME thread
 *  \param  *Queue    The MLME Queue
 *  \param   TimeStampLow    The lower 32 bit of timestamp, here we used for eventID.
 *  \param   Machine  The State Machine Id
 *  \param   MsgType  The Message Type
 *  \param   MsgLen   The Message length
 *  \param  *Msg      The message pointer
 *  \return  TRUE if enqueue is successful, FALSE if the queue is full
 *  \pre
 *  \post
 *  \note    The message has to be initialized
 */
BOOLEAN MlmeEnqueueForWsc(
	IN RTMP_ADAPTER *pAd,
	IN ULONG eventID,
	IN LONG senderID,
	IN ULONG Machine, 
	IN ULONG MsgType, 
	IN ULONG MsgLen, 
	IN VOID *Msg) 
{
	INT Tail;
	MLME_QUEUE	*Queue = (MLME_QUEUE *)&pAd->Mlme.Queue;

	DBGPRINT(RT_DEBUG_TRACE, ("-----> MlmeEnqueueForWsc\n"));
    /* Do nothing if the driver is starting halt state.*/
    /* This might happen when timer already been fired before cancel timer with mlmehalt*/
    if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
        return FALSE;

	/* First check the size, it MUST not exceed the mlme queue size*/
	if (MsgLen > MGMT_DMA_BUFFER_SIZE)
	{
        DBGPRINT_ERR(("MlmeEnqueueForWsc: msg too large, size = %ld \n", MsgLen));
		return FALSE;
	}
	
    if (MlmeQueueFull(Queue, 1)) 
    {
#ifdef RELEASE_EXCLUDE
        /* Can't add debug print here. It'll make Mlme Stat machine freeze.*/
		DBGPRINT(RT_DEBUG_INFO, ("MlmeEnqueueForWsc: full, msg dropped and may corrupt MLME\n"));
#endif /* RELEASE_EXCLUDE */
        
        return FALSE;
    }

    /* OK, we got all the informations, it is time to put things into queue*/
	NdisAcquireSpinLock(&(Queue->Lock));
    Tail = Queue->Tail;
	/*
		Double check for safety in multi-thread system.
	*/
	if (Queue->Entry[Tail].Occupied)
	{
		NdisReleaseSpinLock(&(Queue->Lock));
		return FALSE;
	}
    Queue->Tail++;
    Queue->Num++;
    if (Queue->Tail == MAX_LEN_OF_MLME_QUEUE) 
    {
        Queue->Tail = 0;
    }
    
#ifdef RELEASE_EXCLUDE
    DBGPRINT(RT_DEBUG_INFO, ("MlmeEnqueueForWsc, num=%ld\n",Queue->Num));
#endif /* RELEASE_EXCLUDE */
    Queue->Entry[Tail].Occupied = TRUE;
    Queue->Entry[Tail].Machine = Machine;
    Queue->Entry[Tail].MsgType = MsgType;
    Queue->Entry[Tail].MsgLen  = MsgLen;
	Queue->Entry[Tail].TimeStamp.u.LowPart = eventID;
	Queue->Entry[Tail].TimeStamp.u.HighPart = senderID;
    if (Msg != NULL)
        NdisMoveMemory(Queue->Entry[Tail].Msg, Msg, MsgLen);

    NdisReleaseSpinLock(&(Queue->Lock));

	DBGPRINT(RT_DEBUG_TRACE, ("<----- MlmeEnqueueForWsc\n"));
	
    return TRUE;
}
#endif /* WSC_INCLUDED */


/*! \brief	 Dequeue a message from the MLME Queue
 *	\param	*Queue	  The MLME Queue
 *	\param	*Elem	  The message dequeued from MLME Queue
 *	\return  TRUE if the Elem contains something, FALSE otherwise
 *	\pre
 *	\post
 */
BOOLEAN MlmeDequeue(MLME_QUEUE *Queue, MLME_QUEUE_ELEM **Elem) 
{
	NdisAcquireSpinLock(&(Queue->Lock));
	*Elem = &(Queue->Entry[Queue->Head]);    
	Queue->Num--;
	Queue->Head++;
	if (Queue->Head == MAX_LEN_OF_MLME_QUEUE) 
	{
		Queue->Head = 0;
	}
	NdisReleaseSpinLock(&(Queue->Lock));
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("MlmeDequeue, num=%ld\n",Queue->Num));
#endif /* RELEASE_EXCLUDE */
	return TRUE;
}


VOID MlmeRestartStateMachine(RTMP_ADAPTER *pAd)
{
#ifdef RTMP_MAC_PCI
	MLME_QUEUE_ELEM *Elem = NULL;
#endif /* RTMP_MAC_PCI */
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN Cancelled;
#ifdef P2P_SUPPORT
	PAPCLI_STRUCT pApCliEntry = NULL;
#endif /* P2P_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("MlmeRestartStateMachine \n"));

#ifdef RTMP_MAC_PCI
	NdisAcquireSpinLock(&pAd->Mlme.TaskLock);
	if(pAd->Mlme.bRunning) 
	{
		NdisReleaseSpinLock(&pAd->Mlme.TaskLock);
		return;
	} 
	else 
	{
		pAd->Mlme.bRunning = TRUE;
	}
	NdisReleaseSpinLock(&pAd->Mlme.TaskLock);

	/* Remove all Mlme queues elements*/
	while (!MlmeQueueEmpty(&pAd->Mlme.Queue)) 
	{
		/*From message type, determine which state machine I should drive*/
		if (MlmeDequeue(&pAd->Mlme.Queue, &Elem)) 
		{
			/* free MLME element*/
			Elem->Occupied = FALSE;
			Elem->MsgLen = 0;

		}
		else {
			DBGPRINT_ERR(("MlmeRestartStateMachine: MlmeQueue empty\n"));
		}
	}
#endif /* RTMP_MAC_PCI */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef QOS_DLS_SUPPORT
		UCHAR i;
#endif /* QOS_DLS_SUPPORT */
		/* Cancel all timer events*/
		/* Be careful to cancel new added timer*/
		RTMPCancelTimer(&pAd->MlmeAux.AssocTimer,	  &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.ReassocTimer,   &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.DisassocTimer,  &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.AuthTimer,	   &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.BeaconTimer,	   &Cancelled);
		RTMPCancelTimer(&pAd->MlmeAux.ScanTimer,	   &Cancelled);

#ifdef QOS_DLS_SUPPORT
		for (i=0; i<MAX_NUM_OF_DLS_ENTRY; i++)
		{
			RTMPCancelTimer(&pAd->StaCfg.DLSEntry[i].Timer, &Cancelled);
		}
#endif /* QOS_DLS_SUPPORT */
	}
#endif /* CONFIG_STA_SUPPORT */

	/* Change back to original channel in case of doing scan*/
#ifdef P2P_SUPPORT
	pApCliEntry = &pAd->ApCfg.ApCliTab[BSS0];
	
	if (!P2P_GO_ON(pAd) && (pApCliEntry->Valid == FALSE))
#endif /* P2P_SUPPORT */
	{
	AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
	AsicLockChannel(pAd, pAd->CommonCfg.Channel);
	}

	/* Resume MSDU which is turned off durning scan*/
	RTMPResumeMsduTransmission(pAd);

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* Set all state machines back IDLE*/
		pAd->Mlme.CntlMachine.CurrState    = CNTL_IDLE;
		pAd->Mlme.AssocMachine.CurrState   = ASSOC_IDLE;
		pAd->Mlme.AuthMachine.CurrState    = AUTH_REQ_IDLE;
		pAd->Mlme.AuthRspMachine.CurrState = AUTH_RSP_IDLE;
		pAd->Mlme.SyncMachine.CurrState    = SYNC_IDLE;
		pAd->Mlme.ActMachine.CurrState    = ACT_IDLE;
#ifdef QOS_DLS_SUPPORT
		pAd->Mlme.DlsMachine.CurrState    = DLS_IDLE;
#endif /* QOS_DLS_SUPPORT */
#ifdef DOT11Z_TDLS_SUPPORT
		pAd->Mlme.TdlsMachine.CurrState    = TDLS_IDLE;
#endif /* DOT11Z_TDLS_SUPPORT */

	}
#endif /* CONFIG_STA_SUPPORT */
	
#ifdef RTMP_MAC_PCI	
	/* Remove running state*/
	NdisAcquireSpinLock(&pAd->Mlme.TaskLock);
	pAd->Mlme.bRunning = FALSE;
	NdisReleaseSpinLock(&pAd->Mlme.TaskLock);
#endif /* RTMP_MAC_PCI */

//CFG_TODO for SCAN
#ifdef RT_CFG80211_SUPPORT
	RTEnqueueInternalCmd(pAd, CMDTHREAD_SCAN_END, NULL, 0);
#endif /* RT_CFG80211_SUPPORT */
}


/*! \brief	test if the MLME Queue is empty
 *	\param	*Queue	  The MLME Queue
 *	\return TRUE if the Queue is empty, FALSE otherwise
 *	\pre
 *	\post
 
 IRQL = DISPATCH_LEVEL
 
 */
BOOLEAN MlmeQueueEmpty(MLME_QUEUE *Queue) 
{
	BOOLEAN Ans;

	NdisAcquireSpinLock(&(Queue->Lock));
	Ans = (Queue->Num == 0);
	NdisReleaseSpinLock(&(Queue->Lock));

	return Ans;
}

/*! \brief	 test if the MLME Queue is full
 *	\param	 *Queue 	 The MLME Queue
 *	\return  TRUE if the Queue is empty, FALSE otherwise
 *	\pre
 *	\post

 IRQL = PASSIVE_LEVEL
 IRQL = DISPATCH_LEVEL

 */
BOOLEAN MlmeQueueFull(MLME_QUEUE *Queue, UCHAR SendId) 
{
	BOOLEAN Ans;

	NdisAcquireSpinLock(&(Queue->Lock));
	if (SendId == 0)
		Ans = ((Queue->Num >= (MAX_LEN_OF_MLME_QUEUE / 2)) || Queue->Entry[Queue->Tail].Occupied);
	else
		Ans = ((Queue->Num == MAX_LEN_OF_MLME_QUEUE) || Queue->Entry[Queue->Tail].Occupied);
	NdisReleaseSpinLock(&(Queue->Lock));

	return Ans;
}


/*! \brief	 The destructor of MLME Queue
 *	\param 
 *	\return
 *	\pre
 *	\post
 *	\note	Clear Mlme Queue, Set Queue->Num to Zero.

 IRQL = PASSIVE_LEVEL
 
 */
VOID MlmeQueueDestroy(MLME_QUEUE *pQueue) 
{
	NdisAcquireSpinLock(&(pQueue->Lock));
	pQueue->Num  = 0;
	pQueue->Head = 0;
	pQueue->Tail = 0;
	NdisReleaseSpinLock(&(pQueue->Lock));
	NdisFreeSpinLock(&(pQueue->Lock));
}


/*! \brief	 To substitute the message type if the message is coming from external
 *	\param	pFrame		   The frame received
 *	\param	*Machine	   The state machine
 *	\param	*MsgType	   the message type for the state machine
 *	\return TRUE if the substitution is successful, FALSE otherwise
 *	\pre
 *	\post

 IRQL = DISPATCH_LEVEL

 */
#ifdef CONFIG_STA_SUPPORT
BOOLEAN MsgTypeSubst(RTMP_ADAPTER *pAd, FRAME_802_11 *pFrame, INT *Machine, INT *MsgType)
{
	USHORT	Seq, Alg;
	UCHAR	EAPType;
	PUCHAR	pData;
	BOOLEAN bRV = FALSE;
#ifdef WSC_STA_SUPPORT
	UCHAR EAPCode;
#endif /* WSC_STA_SUPPORT */    

	/* Pointer to start of data frames including SNAP header*/
	pData = (PUCHAR) pFrame + LENGTH_802_11;

	/* The only data type will pass to this function is EAPOL frame*/
	if (pFrame->Hdr.FC.Type == FC_TYPE_DATA) 
	{
#ifdef DOT11Z_TDLS_SUPPORT
		if (NdisEqualMemory(TDLS_LLC_SNAP_WITH_CATEGORY, pData, LENGTH_802_1_H + 2))
		{
			UCHAR	TDLSType;

			/* ieee802.11z TDLS SNAP header*/
			*Machine = TDLS_STATE_MACHINE;
			TDLSType = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 2);
			return (TDLS_MsgTypeSubst(TDLSType, (INT *)MsgType));
		}
		else
#endif /* DOT11Z_TDLS_SUPPORT */
#ifdef WSC_STA_SUPPORT
        /* check for WSC state machine first*/
		if (pAd->StaCfg.WscControl.WscState >= WSC_STATE_LINK_UP)
        {
            *Machine = WSC_STATE_MACHINE;
            EAPType = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 1);
            EAPCode = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 4);
            
            bRV = WscMsgTypeSubst(EAPType, EAPCode, MsgType);
 			if (bRV)
				return bRV;
        }
#endif /* WSC_STA_SUPPORT */
		if (bRV == FALSE)
		{
	        *Machine = WPA_STATE_MACHINE;
			EAPType = *((UCHAR*)pFrame + LENGTH_802_11 + LENGTH_802_1_H + 1);
	        return (WpaMsgTypeSubst(EAPType, (INT *) MsgType));		
		}
	}

	switch (pFrame->Hdr.FC.SubType) 
	{
		case SUBTYPE_ASSOC_REQ:
			*Machine = ASSOC_STATE_MACHINE;
			*MsgType = MT2_PEER_ASSOC_REQ;
			break;
		case SUBTYPE_ASSOC_RSP:
			*Machine = ASSOC_STATE_MACHINE;
			*MsgType = MT2_PEER_ASSOC_RSP;
			break;
		case SUBTYPE_REASSOC_REQ:
			*Machine = ASSOC_STATE_MACHINE;
			*MsgType = MT2_PEER_REASSOC_REQ;
			break;
		case SUBTYPE_REASSOC_RSP:
			*Machine = ASSOC_STATE_MACHINE;
			*MsgType = MT2_PEER_REASSOC_RSP;
			break;
		case SUBTYPE_PROBE_REQ:
			*Machine = SYNC_STATE_MACHINE;
			*MsgType = MT2_PEER_PROBE_REQ;
			break;
		case SUBTYPE_PROBE_RSP:
			*Machine = SYNC_STATE_MACHINE;
			*MsgType = MT2_PEER_PROBE_RSP;
			break;
		case SUBTYPE_BEACON:
			*Machine = SYNC_STATE_MACHINE;
			*MsgType = MT2_PEER_BEACON;
			break;
		case SUBTYPE_ATIM:
			*Machine = SYNC_STATE_MACHINE;
			*MsgType = MT2_PEER_ATIM;
			break;
		case SUBTYPE_DISASSOC:
			*Machine = ASSOC_STATE_MACHINE;
			*MsgType = MT2_PEER_DISASSOC_REQ;
			break;
		case SUBTYPE_AUTH:
			/* get the sequence number from payload 24 Mac Header + 2 bytes algorithm*/
		NdisMoveMemory(&Seq, (UCHAR *)pFrame+sizeof(HEADER_802_11)+2, sizeof(USHORT));
		NdisMoveMemory(&Alg, (UCHAR *)pFrame+sizeof(HEADER_802_11), sizeof(USHORT));
			if ((Seq < 0) || (Seq > 4)) {
				DBGPRINT(RT_DEBUG_ERROR, ("%s() Rx auth with unexpected TransactionSeqNum:%d\n", __func__, Seq));
				return FALSE;
			}

#ifdef WPA3_SUPPORT
			if (Alg == AUTH_MODE_SAE) {
				if ((Seq == 1) || (Seq == 2)) {
					*Machine = AUTH_STATE_MACHINE;
					*MsgType = MT2_PEER_AUTH_EVEN;
				} else
					DBGPRINT(RT_DEBUG_ERROR, ("%s() Rx SAE auth with unexpected TransSeqNum:%d\n", __func__, Seq));
			} else {
#endif
			if (Seq == 1 || Seq == 3) 
			{
				*Machine = AUTH_RSP_STATE_MACHINE;
				*MsgType = MT2_PEER_AUTH_ODD;
			} 
			else if (Seq == 2 || Seq == 4) 
			{
#ifdef DOT11R_FT_SUPPORT
				if (Alg == AUTH_MODE_FT)
				{
					*Machine = FT_OTA_AUTH_STATE_MACHINE;
					*MsgType = FT_OTA_MT2_PEER_AUTH_EVEN;
				}
				else
#endif /* DOT11R_FT_SUPPORT */
				if (Alg == AUTH_MODE_OPEN || Alg == AUTH_MODE_KEY)
				{
					*Machine = AUTH_STATE_MACHINE;
					*MsgType = MT2_PEER_AUTH_EVEN;
				} 
			} 
			else 
			{
				return FALSE;
			}
#ifdef WPA3_SUPPORT
		}
#endif
			break;
		case SUBTYPE_DEAUTH:
			*Machine = AUTH_RSP_STATE_MACHINE;
			*MsgType = MT2_PEER_DEAUTH;
			break;
		case SUBTYPE_ACTION:
		case SUBTYPE_ACTION_NO_ACK:
			*Machine = ACTION_STATE_MACHINE;
#ifdef DOT11R_FT_SUPPORT
			if ((pFrame->Octet[0]&0x7F) == FT_CATEGORY_BSS_TRANSITION) 
			{
				*Machine = FT_OTD_ACT_STATE_MACHINE;
				*MsgType = FT_OTD_MT2_PEER_EVEN;
			}
			else
#endif /* DOT11R_FT_SUPPORT */
			/*  Sometimes Sta will return with category bytes with MSB = 1, if they receive catogory out of their support*/
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
			break;
	}

	return TRUE;
}
#endif /* CONFIG_STA_SUPPORT */


/*! \brief Initialize the state machine.
 *	\param *S			pointer to the state machine 
 *	\param	Trans		State machine transition function
 *	\param	StNr		number of states 
 *	\param	MsgNr		number of messages 
 *	\param	DefFunc 	default function, when there is invalid state/message combination 
 *	\param	InitState	initial state of the state machine 
 *	\param	Base		StateMachine base, internal use only
 *	\pre p_sm should be a legal pointer
 *	\post

 IRQL = PASSIVE_LEVEL
 
 */
VOID StateMachineInit(
	IN STATE_MACHINE *S, 
	IN STATE_MACHINE_FUNC Trans[], 
	IN ULONG StNr, 
	IN ULONG MsgNr, 
	IN STATE_MACHINE_FUNC DefFunc, 
	IN ULONG InitState, 
	IN ULONG Base) 
{
	ULONG i, j;

	/* set number of states and messages*/
	S->NrState = StNr;
	S->NrMsg   = MsgNr;
	S->Base    = Base;

	S->TransFunc  = Trans;

	/* init all state transition to default function*/
	for (i = 0; i < StNr; i++) 
	{
		for (j = 0; j < MsgNr; j++) 
		{
			S->TransFunc[i * MsgNr + j] = DefFunc;
		}
	}

	/* set the starting state*/
	S->CurrState = InitState;
}


/*! \brief This function fills in the function pointer into the cell in the state machine 
 *	\param *S	pointer to the state machine
 *	\param St	state
 *	\param Msg	incoming message
 *	\param f	the function to be executed when (state, message) combination occurs at the state machine
 *	\pre *S should be a legal pointer to the state machine, st, msg, should be all within the range, Base should be set in the initial state
 *	\post

 IRQL = PASSIVE_LEVEL
 
 */
VOID StateMachineSetAction(
	IN STATE_MACHINE *S, 
	IN ULONG St, 
	IN ULONG Msg, 
	IN STATE_MACHINE_FUNC Func) 
{
	ULONG MsgIdx;

	MsgIdx = Msg - S->Base;

	if (St < S->NrState && MsgIdx < S->NrMsg) 
	{
		/* boundary checking before setting the action*/
		S->TransFunc[St * S->NrMsg + MsgIdx] = Func;
	} 
}


/*! \brief	 This function does the state transition
 *	\param	 *Adapter the NIC adapter pointer
 *	\param	 *S 	  the state machine
 *	\param	 *Elem	  the message to be executed
 *	\return   None
 */
VOID StateMachinePerformAction(
	IN	PRTMP_ADAPTER	pAd, 
	IN STATE_MACHINE *S, 
	IN MLME_QUEUE_ELEM *Elem,
	IN ULONG CurrState)
{
	if (S->TransFunc[(CurrState) * S->NrMsg + Elem->MsgType - S->Base])
		(*(S->TransFunc[(CurrState) * S->NrMsg + Elem->MsgType - S->Base]))(pAd, Elem);
}


/*
	==========================================================================
	Description:
		The drop function, when machine executes this, the message is simply 
		ignored. This function does nothing, the message is freed in 
		StateMachinePerformAction()
	==========================================================================
 */
VOID Drop(RTMP_ADAPTER *pAd, MLME_QUEUE_ELEM *Elem) 
{
}


/*
	==========================================================================
	Description:
	==========================================================================
 */
UCHAR RandomByte(RTMP_ADAPTER *pAd)
{
	ULONG i;
	UCHAR R, Result;

	R = 0;

	if (pAd->Mlme.ShiftReg == 0)
	NdisGetSystemUpTime((ULONG *)&pAd->Mlme.ShiftReg);

	for (i = 0; i < 8; i++) 
	{
		if (pAd->Mlme.ShiftReg & 0x00000001) 
		{
			pAd->Mlme.ShiftReg = ((pAd->Mlme.ShiftReg ^ LFSR_MASK) >> 1) | 0x80000000;
			Result = 1;
		} 
		else 
		{
			pAd->Mlme.ShiftReg = pAd->Mlme.ShiftReg >> 1;
			Result = 0;
		}
		R = (R << 1) | Result;
	}

	return R;
}


UCHAR RandomByte2(RTMP_ADAPTER *pAd)
{
	UINT32 a,b;
	UCHAR value, seed = 0;

	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
					__FUNCTION__, __LINE__));
		return 0;
	}
	
	/*MAC statistic related*/
	a = AsicGetCCACnt(pAd);
	a &= 0x0000ffff;
	b = AsicGetCrcErrCnt(pAd);
	b &= 0x0000ffff;
	value = (UCHAR)((a<<16)|b);

	/*get seed by RSSI or SNR related info */
	seed = get_random_seed_by_phy(pAd);

	return value ^ seed ^ RandomByte(pAd);
}


/*
	========================================================================

	Routine Description:
		Verify the support rate for different PHY type

	Arguments:
		pAd 				Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	========================================================================
*/
VOID RTMPCheckRates(RTMP_ADAPTER *pAd, UCHAR SupRate[], UCHAR *SupRateLen)
{
	UCHAR	RateIdx, i, j;
	UCHAR	NewRate[12]={0}, NewRateLen;
	
	NewRateLen = 0;
	
	if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
		RateIdx = 4;
	else
		RateIdx = 12;

	/* Check for support rates exclude basic rate bit */
	for (i = 0; i < *SupRateLen && i < 12; i++)
		for (j = 0; j < RateIdx; j++)
			if ((SupRate[i] & 0x7f) == RateIdTo500Kbps[j])
				{
					NewRate[NewRateLen++] = SupRate[i];
					break;
				}
			
	*SupRateLen = NewRateLen;
	NdisMoveMemory(SupRate, NewRate, NewRateLen);
}

#ifdef CONFIG_STA_SUPPORT
#ifdef DOT11_N_SUPPORT
BOOLEAN RTMPCheckChannel(RTMP_ADAPTER *pAd, UCHAR CentralCh, UCHAR ch)
{
	UCHAR		k;
	UCHAR		UpperChannel = 0, LowerChannel = 0;
	UCHAR		NoEffectChannelinList = 0;
	
	/* Find upper and lower channel according to 40MHz current operation. */
	if (CentralCh < ch)
	{
		UpperChannel = ch;
		if (CentralCh > 2)
			LowerChannel = CentralCh - 2;
		else
			return FALSE;
	}
	else if (CentralCh > ch)
	{
		UpperChannel = CentralCh + 2;
		LowerChannel = ch;
	}

	for (k = 0;k < pAd->ChannelListNum;k++)
	{
		if (pAd->ChannelList[k].Channel == UpperChannel)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO,("UpperChannel=%d  is in Channel List[%d]\n", UpperChannel, k ));
#endif /* RELEASE_EXCLUDE */
			NoEffectChannelinList ++;
		}
		if (pAd->ChannelList[k].Channel == LowerChannel)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO,("LowerChannel=%d  is in Channel List[%d]\n", LowerChannel, k ));
#endif /* RELEASE_EXCLUDE */
			NoEffectChannelinList ++;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE,("Total Channel in Channel List = [%d]\n", NoEffectChannelinList));
	if (NoEffectChannelinList == 2)
		return TRUE;
	else
		return FALSE;
}


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
BOOLEAN RTMPCheckHt(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR Wcid,
	IN HT_CAPABILITY_IE *pHtCap,
	IN ADD_HT_INFO_IE *pAddHtInfo)
{
	MAC_TABLE_ENTRY *sta;

	if (Wcid >= MAX_LEN_OF_MAC_TABLE)
		return FALSE;

	sta = &pAd->MacTab.Content[Wcid];
	/* If use AMSDU, set flag.*/
	if (pAd->CommonCfg.DesiredHtPhy.AmsduEnable)
		CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_AMSDU_INUSED);
	/* Save Peer Capability*/
	if (pAd->CommonCfg.ht_ldpc && (pAd->chipCap.phy_caps & fPHY_CAP_LDPC)) {
			if (pHtCap->HtCapInfo.ht_rx_ldpc)
				CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_HT_RX_LDPC_CAPABLE);
	}
	
	if (pHtCap->HtCapInfo.ShortGIfor20)
		CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_SGI20_CAPABLE);
	if (pHtCap->HtCapInfo.ShortGIfor40)
		CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_SGI40_CAPABLE);
	if (pHtCap->HtCapInfo.TxSTBC)
		CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_TxSTBC_CAPABLE);
	if (pHtCap->HtCapInfo.RxSTBC)
		CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_RxSTBC_CAPABLE);
	if (pAd->CommonCfg.bRdg && pHtCap->ExtHtCapInfo.RDGSupport)
	{
		CLIENT_STATUS_SET_FLAG(sta, fCLIENT_STATUS_RDG_CAPABLE);
	}
	
	if (Wcid < MAX_LEN_OF_MAC_TABLE)
	{
		sta->MpduDensity = pHtCap->HtCapParm.MpduDensity;
	}

	/* Will check ChannelWidth for MCSSet[4] below*/
	NdisZeroMemory(&pAd->MlmeAux.HtCapability.MCSSet[0], 16);
	pAd->MlmeAux.HtCapability.MCSSet[4] = 0x1;
	switch (pAd->CommonCfg.RxStream)
	{
		case 3:
			pAd->MlmeAux.HtCapability.MCSSet[2] = 0xff;
		case 2:
			pAd->MlmeAux.HtCapability.MCSSet[1] = 0xff;
		case 1:
		default:
			pAd->MlmeAux.HtCapability.MCSSet[0] = 0xff;
			break;
	}	

	pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth = pAddHtInfo->AddHtInfo.RecomWidth & pAd->CommonCfg.DesiredHtPhy.ChannelWidth;
		
	/*
		If both station and AP use 40MHz, still need to check if the 40MHZ band's legality in my country region
		If this 40MHz wideband is not allowed in my country list, use bandwidth 20MHZ instead,
	*/
	if (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_40)
	{
		if (RTMPCheckChannel(pAd, pAd->MlmeAux.CentralChannel, pAd->MlmeAux.Channel) == FALSE)
		{
			pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth = BW_20;
		}

		/*BW setting limited by EEPROM setting*/
		if(pAd->MlmeAux.Channel > 14)  /* a-band*/
		{
			if ( pAd->NicConfig2.field.BW40MAvailForA )  /* 1: OFF; 0: ON */
				pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth = BW_20;
		}
		else
		{
			if ( pAd->NicConfig2.field.BW40MAvailForG )  /* 1: OFF; 0: ON */
				pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth = BW_20;
		}
	}
		
    DBGPRINT(RT_DEBUG_TRACE, ("RTMPCheckHt:: HtCapInfo.ChannelWidth=%d, RecomWidth=%d, DesiredHtPhy.ChannelWidth=%d, BW40MAvailForA/G=%d/%d, PhyMode=%d \n",
		pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth, pAddHtInfo->AddHtInfo.RecomWidth, pAd->CommonCfg.DesiredHtPhy.ChannelWidth,
		pAd->NicConfig2.field.BW40MAvailForA, pAd->NicConfig2.field.BW40MAvailForG, pAd->CommonCfg.PhyMode));
    
	pAd->MlmeAux.HtCapability.HtCapInfo.GF =  pHtCap->HtCapInfo.GF &pAd->CommonCfg.DesiredHtPhy.GF;

	/* Send Assoc Req with my HT capability.*/
	pAd->MlmeAux.HtCapability.HtCapInfo.AMsduSize =  pAd->CommonCfg.DesiredHtPhy.AmsduSize;
	pAd->MlmeAux.HtCapability.HtCapInfo.MimoPs =  pAd->CommonCfg.DesiredHtPhy.MimoPs;
	/* The HT Capabilities element are used to advertise optional HT capabilities of an HT STA.
	 * We shouldn't care about the AP's Capabilities in here
	 */
	pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor20 = pAd->CommonCfg.DesiredHtPhy.ShortGIfor20;
	pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor40 = pAd->CommonCfg.DesiredHtPhy.ShortGIfor40;
	pAd->MlmeAux.HtCapability.HtCapInfo.TxSTBC = pAd->CommonCfg.DesiredHtPhy.TxSTBC;
	pAd->MlmeAux.HtCapability.HtCapInfo.RxSTBC = pAd->CommonCfg.DesiredHtPhy.RxSTBC;
	
	if (CLIENT_STATUS_TEST_FLAG(sta, fCLIENT_STATUS_HT_RX_LDPC_CAPABLE))
		pAd->MlmeAux.HtCapability.HtCapInfo.ht_rx_ldpc = 1;
	else
		pAd->MlmeAux.HtCapability.HtCapInfo.ht_rx_ldpc = 0;

	pAd->MlmeAux.HtCapability.HtCapParm.MaxRAmpduFactor = pAd->CommonCfg.DesiredHtPhy.MaxRAmpduFactor;
	pAd->MlmeAux.HtCapability.HtCapParm.MpduDensity = pAd->CommonCfg.HtCapability.HtCapParm.MpduDensity;
	pAd->MlmeAux.HtCapability.ExtHtCapInfo.PlusHTC = pHtCap->ExtHtCapInfo.PlusHTC;
	sta->HTCapability.ExtHtCapInfo.PlusHTC = pHtCap->ExtHtCapInfo.PlusHTC;
	if (pAd->CommonCfg.bRdg)
	{
		pAd->MlmeAux.HtCapability.ExtHtCapInfo.RDGSupport = pHtCap->ExtHtCapInfo.RDGSupport;
		pAd->MlmeAux.HtCapability.ExtHtCapInfo.PlusHTC = 1;
	}
	
    if (pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth == BW_20)
        pAd->MlmeAux.HtCapability.MCSSet[4] = 0x0;  /* BW20 can't transmit MCS32*/

#ifdef TXBF_SUPPORT
	if (pAd->chipCap.FlgHwTxBfCap)
	    setETxBFCap(pAd, &pAd->MlmeAux.HtCapability.TxBFCap);
#endif /* TXBF_SUPPORT */

	COPY_AP_HTSETTINGS_FROM_BEACON(pAd, pHtCap);
	return TRUE;
}


#ifdef DOT11_VHT_AC
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
BOOLEAN RTMPCheckVht(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR Wcid,
	IN VHT_CAP_IE *vht_cap,
	IN VHT_OP_IE *vht_op)
{
	VHT_CAP_INFO *vht_cap_info = &vht_cap->vht_cap;
	MAC_TABLE_ENTRY *pEntry;

	// TODO: shiang-6590, not finish yet!!!!

	if (Wcid >= MAX_LEN_OF_MAC_TABLE)
		return FALSE;

	pEntry = &pAd->MacTab.Content[Wcid];
	/* Save Peer Capability*/
	if (vht_cap_info->sgi_80M)
		CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI80_CAPABLE);
	if (vht_cap_info->sgi_160M)
		CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_SGI160_CAPABLE);
	if (vht_cap_info->tx_stbc)
		CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_VHT_TXSTBC_CAPABLE);
	if (vht_cap_info->rx_stbc)
		CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_VHT_RXSTBC_CAPABLE);

	if (pAd->CommonCfg.vht_ldpc && (pAd->chipCap.phy_caps & fPHY_CAP_LDPC)) {
		if (vht_cap_info->rx_ldpc)
			CLIENT_STATUS_SET_FLAG(pEntry, fCLIENT_STATUS_VHT_RX_LDPC_CAPABLE);
	}

	/* Will check ChannelWidth for MCSSet[4] below */
	NdisZeroMemory(&pAd->MlmeAux.vht_cap.mcs_set, sizeof(VHT_MCS_SET));
	pAd->MlmeAux.vht_cap.mcs_set.rx_high_rate = pAd->CommonCfg.RxStream * 325;
	pAd->MlmeAux.vht_cap.mcs_set.tx_high_rate = pAd->CommonCfg.TxStream * 325;

	//pAd->MlmeAux.vht_cap.vht_cap.ch_width = vht_cap_info->ch_width;
#ifdef VHT_TXBF_SUPPORT
	if (pAd->chipCap.FlgHwTxBfCap)
	    setVHTETxBFCap(pAd, &pAd->MlmeAux.vht_cap.vht_cap);
#endif /* TXBF_SUPPORT */

	return TRUE;
}
#endif /* DOT11_VHT_AC */
#endif /* DOT11_N_SUPPORT */


/*
	========================================================================

	Routine Description:
		Verify the support rate for different PHY type

	Arguments:
		pAd 				Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	========================================================================
*/
VOID RTMPUpdateMlmeRate(RTMP_ADAPTER *pAd)
{
	UCHAR MinimumRate;
	UCHAR ProperMlmeRate; /*= RATE_54; */
	UCHAR i, j, RateIdx = 12; /* 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54 */
	BOOLEAN	bMatch = FALSE;


	switch (pAd->CommonCfg.PhyMode) 
	{
		case ((UCHAR)WMODE_B):
			ProperMlmeRate = RATE_11;
			MinimumRate = RATE_1;
			break;
		case ((UCHAR)(WMODE_B | WMODE_G)):
#ifdef DOT11_N_SUPPORT
		case ((UCHAR)(WMODE_B | WMODE_G | WMODE_GN | WMODE_A | WMODE_AN)):
		case ((UCHAR)(WMODE_B | WMODE_G | WMODE_GN)):
#ifdef DOT11_VHT_AC
		case ((UCHAR)(WMODE_B | WMODE_G | WMODE_GN | WMODE_A | WMODE_AN | WMODE_AC)):
#endif /* DOT11_VHT_AC */
#endif /* DOT11_N_SUPPORT */
			if ((pAd->MlmeAux.SupRateLen == 4) &&
				(pAd->MlmeAux.ExtRateLen == 0))
				ProperMlmeRate = RATE_11; /* B only AP */
			else
				ProperMlmeRate = RATE_24;
			
			if (pAd->MlmeAux.Channel <= 14)
				MinimumRate = RATE_1;
			else
				MinimumRate = RATE_6;
			break;
		case ((UCHAR)WMODE_A):
#ifdef DOT11_N_SUPPORT
		case ((UCHAR)WMODE_GN):
		case ((UCHAR)(WMODE_G | WMODE_GN)):
		case ((UCHAR)(WMODE_A | WMODE_G | WMODE_GN | WMODE_AN)):
		case ((UCHAR)(WMODE_A | WMODE_AN)):
		case ((UCHAR)WMODE_AN):
#ifdef DOT11_VHT_AC
		case ((UCHAR)(WMODE_A | WMODE_G | WMODE_GN | WMODE_AN | WMODE_AC)):
#endif /* DOT11_VHT_AC */
#endif /* DOT11_N_SUPPORT */
			ProperMlmeRate = RATE_24;
			MinimumRate = RATE_6;
			break;
		case ((UCHAR)(WMODE_A | WMODE_B | WMODE_G)):
			ProperMlmeRate = RATE_24;
			if (pAd->MlmeAux.Channel <= 14)
			   MinimumRate = RATE_1;
			else
				MinimumRate = RATE_6;
			break;
		default:
			ProperMlmeRate = RATE_1;
			MinimumRate = RATE_1;
			break;
	}


#ifdef DOT11_VHT_AC
	if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
	{
		ProperMlmeRate = RATE_11;
		MinimumRate = RATE_1;
	}
	else
	{
		if (WMODE_CAP(pAd->CommonCfg.PhyMode, WMODE_B))
		{
			if ((pAd->MlmeAux.SupRateLen == 4) && (pAd->MlmeAux.ExtRateLen == 0))
				ProperMlmeRate = RATE_11; /* B only AP */
			else
				ProperMlmeRate = RATE_24;

			if (pAd->MlmeAux.Channel <= 14)
				MinimumRate = RATE_1;
			else
				MinimumRate = RATE_6;
		}
		else
		{
			ProperMlmeRate = RATE_24;
			MinimumRate = RATE_6;
		}
	}
#endif /* DOT11_VHT_AC */

	for (i = 0; i < pAd->MlmeAux.SupRateLen; i++)
	{
		for (j = 0; j < RateIdx; j++)
		{
			if ((pAd->MlmeAux.SupRate[i] & 0x7f) == RateIdTo500Kbps[j])
			{
				if (j == ProperMlmeRate)
				{
					bMatch = TRUE;
					break;
				}
			}			
		}

		if (bMatch)
			break;
	}

	if (bMatch == FALSE)
	{
		for (i = 0; i < pAd->MlmeAux.ExtRateLen; i++)
		{
			for (j = 0; j < RateIdx; j++)
			{
				if ((pAd->MlmeAux.ExtRate[i] & 0x7f) == RateIdTo500Kbps[j])
				{
						if (j == ProperMlmeRate)
						{
							bMatch = TRUE;
							break;
						}
				}
			}
			
				if (bMatch)
					break;
		}
	}

	if (bMatch == FALSE)
		ProperMlmeRate = MinimumRate;

	pAd->CommonCfg.MlmeRate = MinimumRate;
	pAd->CommonCfg.RtsRate = ProperMlmeRate;
	if (pAd->CommonCfg.MlmeRate >= RATE_6)
	{
		pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_OFDM;
		pAd->CommonCfg.MlmeTransmit.field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.MlmeRate];
		pAd->MacTab.Content[BSS0Mcast_WCID].HTPhyMode.field.MODE = MODE_OFDM;
		pAd->MacTab.Content[BSS0Mcast_WCID].HTPhyMode.field.MCS = OfdmRateToRxwiMCS[pAd->CommonCfg.MlmeRate];
	}
	else
	{
		pAd->CommonCfg.MlmeTransmit.field.MODE = MODE_CCK;
		pAd->CommonCfg.MlmeTransmit.field.MCS = pAd->CommonCfg.MlmeRate;
		pAd->MacTab.Content[BSS0Mcast_WCID].HTPhyMode.field.MODE = MODE_CCK;
		pAd->MacTab.Content[BSS0Mcast_WCID].HTPhyMode.field.MCS = pAd->CommonCfg.MlmeRate;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s():=>MlmeTransmit=0x%x\n",
				__FUNCTION__, pAd->CommonCfg.MlmeTransmit.word));
}
#endif /* CONFIG_STA_SUPPORT */


CHAR RTMPAvgRssi(RTMP_ADAPTER *pAd, RSSI_SAMPLE *pRssi)
{
	CHAR Rssi;

	if(pAd->Antenna.field.RxPath == 3)
	{
		Rssi = (pRssi->AvgRssi[0] + pRssi->AvgRssi[1] + pRssi->AvgRssi[2])/3;
	}
	else if(pAd->Antenna.field.RxPath == 2)
	{
		Rssi = (pRssi->AvgRssi[0] + pRssi->AvgRssi[1])>>1;
	}
	else
	{
		Rssi = pRssi->AvgRssi[0];
	}

	return Rssi;
}

CHAR RTMPMaxNoise(RTMP_ADAPTER *pAd, CHAR Noise0, CHAR Noise1, CHAR Noise2)
{
	CHAR	larger = -127;

	if ((pAd->Antenna.field.RxPath == 1) && (Noise0 != 0))
		larger = Noise0;

	if ((pAd->Antenna.field.RxPath >= 2) && (Noise1 != 0))
		larger = max(Noise0, Noise1);

	if ((pAd->Antenna.field.RxPath == 3) && (Noise2 != 0))
		larger = max(larger, Noise2);

	if (larger == -127)
		larger = 0;

	return larger;
}

CHAR RTMPMaxRssi(RTMP_ADAPTER *pAd, CHAR Rssi0, CHAR Rssi1, CHAR Rssi2)
{
	CHAR	larger = -127;
	
	if ((pAd->Antenna.field.RxPath == 1) && (Rssi0 != 0))
	{
		larger = Rssi0;
	}

	if ((pAd->Antenna.field.RxPath >= 2) && (Rssi1 != 0))
	{
		larger = max(Rssi0, Rssi1);
	}
	
	if ((pAd->Antenna.field.RxPath == 3) && (Rssi2 != 0))
	{
		larger = max(larger, Rssi2);
	}

	if (larger == -127)
		larger = 0;

	return larger;
}

CHAR RTMPMinRssi(RTMP_ADAPTER *pAd, CHAR Rssi0, CHAR Rssi1, CHAR Rssi2)
{
	CHAR	smaller = -127;

	if ((pAd->Antenna.field.RxPath == 1) && (Rssi0 != 0))
		smaller = Rssi0;

	if ((pAd->Antenna.field.RxPath >= 2) && (Rssi1 != 0))
		smaller = min(Rssi0, Rssi1);

	if ((pAd->Antenna.field.RxPath == 3) && (Rssi2 != 0))
		smaller = min(smaller, Rssi2);

	if (smaller == -127)
		smaller = 0;

	return smaller;
}

CHAR RTMPMinSnr(RTMP_ADAPTER *pAd, CHAR Snr0, CHAR Snr1)
{
	CHAR	smaller = Snr0;
	
	if (pAd->Antenna.field.RxPath == 1) 
	{
		smaller = Snr0;
	}

	if ((pAd->Antenna.field.RxPath >= 2) && (Snr1 != 0))
	{
		smaller = min(Snr0, Snr1);
	}
 
	return smaller;
}


/*
    ========================================================================
    Routine Description:
        Periodic evaluate antenna link status
        
    Arguments:
        pAd         - Adapter pointer
        
    Return Value:
        None
        
    ========================================================================
*/
VOID AsicEvaluateRxAnt(RTMP_ADAPTER *pAd)
{
#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		return;
#endif /* CONFIG_ATE */

#ifdef RTMP_MAC_USB
#ifdef CONFIG_STA_SUPPORT
	if(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
		return;
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_USB */

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS |
							fRTMP_ADAPTER_HALT_IN_PROGRESS |
							fRTMP_ADAPTER_RADIO_OFF |
							fRTMP_ADAPTER_NIC_NOT_EXIST |
							fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)
		|| OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) 
#ifdef RT3090
		|| (pAd->bPCIclkOff == TRUE)
#endif /* RT3090 */
#ifdef ANT_DIVERSITY_SUPPORT
		|| (pAd->EepromAccess)
#endif /* ANT_DIVERSITY_SUPPORT */
#ifdef RT3593
		|| IS_RT3593(pAd)
#endif /* RT3593 */
	)
		return;
	
#ifdef RT3290
	if (IS_RT3290(pAd) && (pAd->RalinkCounters.OneSecTransmittedByteCount >= 500))
		return;
#endif /* RT3290 */

#ifdef MT_MAC
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT)
		return;
#endif /* MT_MAC */

#ifdef ANT_DIVERSITY_SUPPORT
	if ((pAd->CommonCfg.bSWRxAntDiversity) 
		 && (pAd->CommonCfg.RxAntDiversityCfg == ANT_SW_DIVERSITY_ENABLE))
	{
		/* two antenna selection mechanism- one is antenna diversity, the other is failed antenna remove*/
		/* one is antenna diversity:there is only one antenna can rx and tx*/
		/* the other is failed antenna remove:two physical antenna can rx and tx*/
			DBGPRINT(RT_DEBUG_TRACE,("AntDiv - before evaluate Pair1-Ant (%d,%d)\n",
				pAd->RxAnt.Pair1PrimaryRxAnt, pAd->RxAnt.Pair1SecondaryRxAnt));

			AsicSetRxAnt(pAd, pAd->RxAnt.Pair1SecondaryRxAnt);
				
			pAd->RxAnt.EvaluatePeriod = 1; /* 1:Means switch to SecondaryRxAnt, 0:Means switch to Pair1PrimaryRxAnt*/
			pAd->RxAnt.FirstPktArrivedWhenEvaluate = FALSE;
			pAd->RxAnt.RcvPktNumWhenEvaluate = 0;

			/* a one-shot timer to end the evalution*/
			/* dynamic adjust antenna evaluation period according to the traffic*/
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED) ||
				OPSTATUS_TEST_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED))
#ifdef CONFIG_AP_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
				RTMPSetTimer(&pAd->Mlme.RxAntEvalTimer, 5000);
			else
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
				RTMPSetTimer(&pAd->Mlme.RxAntEvalTimer, 100);
			else
#endif /* CONFIG_STA_SUPPORT */
				RTMPSetTimer(&pAd->Mlme.RxAntEvalTimer, 300);
		}
		else
#endif /* ANT_DIVERSITY_SUPPORT */
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
/*			if (pAd->CommonCfg.bRxAntDiversity == ANT_DIVERSITY_DISABLE)*/
			/* for SmartBit 64-byte stream test */
			if (pAd->MacTab.Size > 0)
				APAsicEvaluateRxAnt(pAd);
			return;
		}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("AsicEvaluateRxAnt : RealRxPath=%d \n", pAd->Mlme.RealRxPath));
#endif /* RELEASE_EXCLUDE */

			if (pAd->StaCfg.PwrMgmt.Psm == PWR_SAVE)
				return;

			bbp_set_rxpath(pAd, pAd->Antenna.field.RxPath);
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)
#ifdef MESH_SUPPORT
				|| MESH_ON(pAd)
#endif /* MESH_SUPPORT */
#ifdef P2P_SUPPORT
				 || P2P_GO_ON(pAd) || P2P_CLI_ON(pAd)
#endif /* P2P_SUPPORT */
			)
			{
				ULONG TxTotalCnt = pAd->RalinkCounters.OneSecTxNoRetryOkCount + 
									pAd->RalinkCounters.OneSecTxRetryOkCount + 
									pAd->RalinkCounters.OneSecTxFailCount;

				/* dynamic adjust antenna evaluation period according to the traffic*/
				if (TxTotalCnt > 50)
				{
					RTMPSetTimer(&pAd->Mlme.RxAntEvalTimer, 20);
					pAd->Mlme.bLowThroughput = FALSE;
				}
				else
				{
					RTMPSetTimer(&pAd->Mlme.RxAntEvalTimer, 300);
					pAd->Mlme.bLowThroughput = TRUE;
				}
			}
		}
#endif /* CONFIG_STA_SUPPORT */
	}
}


/*
    ========================================================================
    Routine Description:
        After evaluation, check antenna link status
        
    Arguments:
        pAd         - Adapter pointer
        
    Return Value:
        None
        
    ========================================================================
*/
VOID AsicRxAntEvalTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3) 
{
	RTMP_ADAPTER	*pAd = (RTMP_ADAPTER *)FunctionContext;
#ifdef CONFIG_STA_SUPPORT
	CHAR			larger = -127, rssi0, rssi1, rssi2;
#endif /* CONFIG_STA_SUPPORT */

#ifdef ANT_DIVERSITY_SUPPORT
	BOOLEAN			bSwapAnt = FALSE;
#ifdef CONFIG_AP_SUPPORT
	SHORT realrssi, last_rssi = pAd->RxAnt.Pair1LastAvgRssi;
	ULONG recv = pAd->RxAnt.RcvPktNum[pAd->RxAnt.Pair1SecondaryRxAnt];
#endif /* CONFIG_AP_SUPPORT */
#endif /* ANT_DIVERSITY_SUPPORT */


#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
		return;
#endif /* CONFIG_ATE */

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS |
							fRTMP_ADAPTER_HALT_IN_PROGRESS |
							fRTMP_ADAPTER_RADIO_OFF |
							fRTMP_ADAPTER_NIC_NOT_EXIST) 
		|| OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) 
#ifdef RT3090
		|| (pAd->bPCIclkOff == TRUE) 
#endif /* RT3090 */
#ifdef ANT_DIVERSITY_SUPPORT
		|| (pAd->EepromAccess)
#endif /* ANT_DIVERSITY_SUPPORT */
	)
		return;

#ifdef ANT_DIVERSITY_SUPPORT
	if ((pAd->CommonCfg.bSWRxAntDiversity) 
	 && (pAd->CommonCfg.RxAntDiversityCfg == ANT_SW_DIVERSITY_ENABLE))
	{
#ifdef CONFIG_AP_SUPPORT
#ifdef RELEASE_EXCLUDE
		/* For the corner case, AP can't set the threshold too higher.
		 * In test environment, the Avg. RSSI only get 1 or 2dBm as AP can take care the bad RSSI STA.
		 */
#endif /* RELEASE_EXCLUDE */
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
			APAsicAntennaAvg(pAd, pAd->RxAnt.Pair1SecondaryRxAnt, &realrssi);
			if ((recv != 0) && (realrssi >= (pAd->RxAnt.Pair1LastAvgRssi + 1)))
				bSwapAnt = TRUE;
	}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			if ((pAd->RxAnt.RcvPktNumWhenEvaluate != 0) && (pAd->RxAnt.Pair1AvgRssi[pAd->RxAnt.Pair1SecondaryRxAnt] >= pAd->RxAnt.Pair1AvgRssi[pAd->RxAnt.Pair1PrimaryRxAnt]))
				bSwapAnt = TRUE;
#endif /* CONFIG_STA_SUPPORT */
		if (bSwapAnt == TRUE)	
			{
				UCHAR temp;

				/* select PrimaryRxAntPair*/
				/*    Role change, Used Pair1SecondaryRxAnt as PrimaryRxAntPair.*/
				/*    Since Pair1SecondaryRxAnt Quality good than Pair1PrimaryRxAnt*/
				temp = pAd->RxAnt.Pair1PrimaryRxAnt;
				pAd->RxAnt.Pair1PrimaryRxAnt = pAd->RxAnt.Pair1SecondaryRxAnt;
				pAd->RxAnt.Pair1SecondaryRxAnt = temp;

#ifdef CONFIG_AP_SUPPORT
				pAd->RxAnt.Pair1LastAvgRssi = realrssi;
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
				pAd->RxAnt.Pair1LastAvgRssi = (pAd->RxAnt.Pair1AvgRssi[pAd->RxAnt.Pair1SecondaryRxAnt] >> 3);
#endif /* CONFIG_STA_SUPPORT */
/*				pAd->RxAnt.EvaluateStableCnt = 0;*/
			}
			else
			{
				/* if the evaluated antenna is not better than original, switch back to original antenna*/
				AsicSetRxAnt(pAd, pAd->RxAnt.Pair1PrimaryRxAnt);
				pAd->RxAnt.EvaluateStableCnt ++;
			}

			pAd->RxAnt.EvaluatePeriod = 0; /* 1:Means switch to SecondaryRxAnt, 0:Means switch to Pair1PrimaryRxAnt*/

#ifdef CONFIG_AP_SUPPORT
			IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
			{
				DBGPRINT(RT_DEBUG_TRACE,("AsicRxAntEvalAction::After Eval(fix in #%d), <%d, %d>, RcvPktNumWhenEvaluate=%ld\n",
					pAd->RxAnt.Pair1PrimaryRxAnt, last_rssi, realrssi, recv));
			}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
			DBGPRINT(RT_DEBUG_TRACE,("AsicRxAntEvalAction::After Eval(fix in #%d), <%d, %d>, RcvPktNumWhenEvaluate=%ld\n",
					pAd->RxAnt.Pair1PrimaryRxAnt, (pAd->RxAnt.Pair1AvgRssi[0] >> 3), (pAd->RxAnt.Pair1AvgRssi[1] >> 3), pAd->RxAnt.RcvPktNumWhenEvaluate));
#endif /* CONFIG_STA_SUPPORT */
		}
		else
#endif /* ANT_DIVERSITY_SUPPORT */
	{
#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		{
/*			if (pAd->CommonCfg.bRxAntDiversity == ANT_DIVERSITY_DISABLE)*/
				APAsicRxAntEvalTimeout(pAd);
			return;
		}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if (pAd->StaCfg.PwrMgmt.Psm == PWR_SAVE)
				return;


			/* if the traffic is low, use average rssi as the criteria*/
			if (pAd->Mlme.bLowThroughput == TRUE)
			{
				rssi0 = pAd->StaCfg.RssiSample.LastRssi[0];
				rssi1 = pAd->StaCfg.RssiSample.LastRssi[1];
				rssi2 = pAd->StaCfg.RssiSample.LastRssi[2];
			}
			else
			{
				rssi0 = pAd->StaCfg.RssiSample.AvgRssi[0];
				rssi1 = pAd->StaCfg.RssiSample.AvgRssi[1];
				rssi2 = pAd->StaCfg.RssiSample.AvgRssi[2];
			}

			if(pAd->Antenna.field.RxPath == 3)
			{
				larger = max(rssi0, rssi1);
#ifdef DOT11N_SS3_SUPPORT
				if (pAd->CommonCfg.TxStream >= 3)
				{
					pAd->Mlme.RealRxPath = 3;
				}
				else
#endif /* DOT11N_SS3_SUPPORT */
				if (larger > (rssi2 + 20))
					pAd->Mlme.RealRxPath = 2;
				else
					pAd->Mlme.RealRxPath = 3;
			}
			else if(pAd->Antenna.field.RxPath == 2)
			{
				if (rssi0 > (rssi1 + 20))
					pAd->Mlme.RealRxPath = 1;
				else
					pAd->Mlme.RealRxPath = 2;
			}

			bbp_set_rxpath(pAd, pAd->Mlme.RealRxPath);
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("AsicRxAntEvalTimeout : RealRxPath=%d, AvgRssi=%d, %d, %d \n",
				pAd->Mlme.RealRxPath, rssi0, rssi1, rssi2));
#endif /* RELEASE_EXCLUDE */
		}
#endif /* CONFIG_STA_SUPPORT */
	}
}


VOID APSDPeriodicExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3) 
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)FunctionContext;

	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED) &&
		!OPSTATUS_TEST_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED))
		return;

	pAd->CommonCfg.TriggerTimerCount++;

/* Driver should not send trigger frame, it should be send by application layer*/
/*
	if (pAd->CommonCfg.bAPSDCapable && pAd->CommonCfg.APEdcaParm.bAPSDCapable
		&& (pAd->CommonCfg.bNeedSendTriggerFrame ||
		(((pAd->CommonCfg.TriggerTimerCount%20) == 19) && (!pAd->CommonCfg.bAPSDAC_BE || !pAd->CommonCfg.bAPSDAC_BK || !pAd->CommonCfg.bAPSDAC_VI || !pAd->CommonCfg.bAPSDAC_VO))))
	{
		DBGPRINT(RT_DEBUG_TRACE,("Sending trigger frame and enter service period when support APSD\n"));
		RTMPSendNullFrame(pAd, pAd->CommonCfg.TxRate, TRUE);
		pAd->CommonCfg.bNeedSendTriggerFrame = FALSE;
		pAd->CommonCfg.TriggerTimerCount = 0;
		pAd->CommonCfg.bInServicePeriod = TRUE;
	}*/
}


/*
    ========================================================================
    Routine Description:
        check if this entry need to switch rate automatically
        
    Arguments:
        pAd         
        pEntry 	 	

    Return Value:
        TURE
        FALSE
        
    ========================================================================
*/
BOOLEAN RTMPCheckEntryEnableAutoRateSwitch(
	IN RTMP_ADAPTER *pAd,
	IN MAC_TABLE_ENTRY *pEntry)
{
	BOOLEAN result = TRUE;

	if ((!pEntry) || (!(pEntry->wdev))) {
#if 1
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): entry or wdev is NULL!\n", 
					__FUNCTION__));
#else
		/* If pEntry is null, pEntry->wdev would cause system crash */
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): entry(%p) or wdev(%p) is NULL!\n", 
					__FUNCTION__, pEntry, pEntry->wdev));
#endif
		return FALSE;
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd) {
		result = pEntry->wdev->bAutoTxRateSwitch;
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* only associated STA counts*/
#ifdef P2P_SUPPORT
		if (pEntry && IS_P2P_GO_ENTRY(pEntry))
			result = pAd->ApCfg.MBSSID[pEntry->func_tb_idx].wdev.bAutoTxRateSwitch;
		else if (pEntry && IS_ENTRY_APCLI(pEntry))
			result = pAd->ApCfg.ApCliTab[pEntry->func_tb_idx].wdev.bAutoTxRateSwitch;
		else
#endif /* P2P_SUPPORT */
		if ((pEntry && IS_ENTRY_CLIENT(pEntry) && (pEntry->Sst == SST_ASSOC))
#ifdef QOS_DLS_SUPPORT
			|| (pEntry && IS_ENTRY_DLS(pEntry))
#endif /* QOS_DLS_SUPPORT */
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
			|| (pEntry && IS_ENTRY_TDLS(pEntry))
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
			)
		{
			result = pAd->StaCfg.wdev.bAutoTxRateSwitch;
		}
		else
			result = FALSE;
	}
#endif /* CONFIG_STA_SUPPORT */


#ifdef MESH_SUPPORT
	if (IS_ENTRY_MESH(pEntry))
		result = pEntry->wdev->bAutoTxRateSwitch;
#endif /* MESH_SUPPORT */

#ifdef TXBF_SUPPORT
		if (result == FALSE)
		{
			//Force MCS will be fixed
			if (pAd->chipCap.FlgHwTxBfCap)
			{
				eTxBFProbing(pAd, pEntry);			
			}
		}
#endif /* TXBF_SUPPORT */


	return result;
}


BOOLEAN RTMPAutoRateSwitchCheck(RTMP_ADAPTER *pAd)	
{
#ifdef CONFIG_AP_SUPPORT		
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		INT	apidx = 0;
	
		for (apidx = 0; apidx < pAd->ApCfg.BssidNum; apidx++)
		{
			if (pAd->ApCfg.MBSSID[apidx].wdev.bAutoTxRateSwitch)
				return TRUE;
		}			
#ifdef WDS_SUPPORT
		for (apidx = 0; apidx < MAX_WDS_ENTRY; apidx++)
		{
			if (pAd->WdsTab.WdsEntry[apidx].wdev.bAutoTxRateSwitch)
				return TRUE;
		}
#endif /* WDS_SUPPORT */
#ifdef APCLI_SUPPORT
		for (apidx = 0; apidx < MAX_APCLI_NUM; apidx++)
		{
			if (pAd->ApCfg.ApCliTab[apidx].wdev.bAutoTxRateSwitch)
				return TRUE;
		}		
#endif /* APCLI_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.wdev.bAutoTxRateSwitch)
			return TRUE;
	}
#endif /* CONFIG_STA_SUPPORT */
	return FALSE;
}


/*
    ========================================================================
    Routine Description:
        check if this entry need to fix tx legacy rate
        
    Arguments:
        pAd         
        pEntry 	 	

    Return Value:
        TURE
        FALSE
        
    ========================================================================
*/
UCHAR RTMPStaFixedTxMode(RTMP_ADAPTER *pAd, MAC_TABLE_ENTRY *pEntry)
{
	UCHAR tx_mode = FIXED_TXMODE_HT;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	if (pEntry)
	{
		if (IS_ENTRY_CLIENT(pEntry))
			tx_mode = (UCHAR)pAd->ApCfg.MBSSID[pEntry->func_tb_idx].wdev.DesiredTransmitSetting.field.FixedTxMode;
#ifdef WDS_SUPPORT
		else if (IS_ENTRY_WDS(pEntry))
			tx_mode = (UCHAR)pAd->WdsTab.WdsEntry[pEntry->func_tb_idx].wdev.DesiredTransmitSetting.field.FixedTxMode;
#endif /* WDS_SUPPORT */
#ifdef APCLI_SUPPORT
		else if (IS_ENTRY_APCLI(pEntry))
			tx_mode = (UCHAR)pAd->ApCfg.ApCliTab[pEntry->func_tb_idx].wdev.DesiredTransmitSetting.field.FixedTxMode;
#endif /* APCLI_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef P2P_SUPPORT
		if (IS_P2P_GO_ENTRY(pEntry))
			tx_mode = (UCHAR)pAd->ApCfg.MBSSID[pEntry->func_tb_idx].wdev.DesiredTransmitSetting.field.FixedTxMode;
		else if (IS_ENTRY_APCLI(pEntry))
			tx_mode = (UCHAR)pAd->ApCfg.ApCliTab[pEntry->func_tb_idx].wdev.DesiredTransmitSetting.field.FixedTxMode;
		else
#endif /* P2P_SUPPORT */
		tx_mode = (UCHAR)pAd->StaCfg.wdev.DesiredTransmitSetting.field.FixedTxMode;
	}
#endif /* CONFIG_STA_SUPPORT */

	return tx_mode;
}


/*
    ========================================================================
    Routine Description:
        Overwrite HT Tx Mode by Fixed Legency Tx Mode, if specified.
        
    Arguments:
        pAd         
        pEntry 	 	

    Return Value:
        TURE
        FALSE
        
    ========================================================================
*/
VOID RTMPUpdateLegacyTxSetting(UCHAR fixed_tx_mode, MAC_TABLE_ENTRY *pEntry)
{
	HTTRANSMIT_SETTING TransmitSetting;
	
	if ((fixed_tx_mode != FIXED_TXMODE_CCK) &&
		(fixed_tx_mode != FIXED_TXMODE_OFDM)
#ifdef DOT11_VHT_AC
		&& (fixed_tx_mode != FIXED_TXMODE_VHT)
#endif /* DOT11_VHT_AC */
	)
		return;
							 				
	TransmitSetting.word = 0;

	TransmitSetting.field.MODE = pEntry->HTPhyMode.field.MODE;
	TransmitSetting.field.MCS = pEntry->HTPhyMode.field.MCS;
						
#ifdef DOT11_VHT_AC
	if (fixed_tx_mode == FIXED_TXMODE_VHT)
	{
		TransmitSetting.field.MODE = MODE_VHT;
		TransmitSetting.field.BW = pEntry->MaxHTPhyMode.field.BW;
		/* CCK mode allow MCS 0~3*/
		if (TransmitSetting.field.MCS > ((1 << 4) + MCS_7))
			TransmitSetting.field.MCS = ((1 << 4) + MCS_7);
	}
	else
#endif /* DOT11_VHT_AC */
	if (fixed_tx_mode == FIXED_TXMODE_CCK)
	{
		TransmitSetting.field.MODE = MODE_CCK;
		/* CCK mode allow MCS 0~3*/
		if (TransmitSetting.field.MCS > MCS_3)
			TransmitSetting.field.MCS = MCS_3;
	}
	else
	{
		TransmitSetting.field.MODE = MODE_OFDM;
		/* OFDM mode allow MCS 0~7*/
		if (TransmitSetting.field.MCS > MCS_7)
			TransmitSetting.field.MCS = MCS_7;
	}
	
	if (pEntry->HTPhyMode.field.MODE >= TransmitSetting.field.MODE)
	{
		pEntry->HTPhyMode.word = TransmitSetting.word;
		DBGPRINT(RT_DEBUG_TRACE, ("RTMPUpdateLegacyTxSetting : wcid-%d, MODE=%s, MCS=%d \n", 
				pEntry->wcid, get_phymode_str(pEntry->HTPhyMode.field.MODE), pEntry->HTPhyMode.field.MCS));		
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : the fixed TxMode is invalid \n", __FUNCTION__));	
	}
}

#ifdef CONFIG_STA_SUPPORT
/*
	==========================================================================
	Description:
		dynamic tune BBP R66 to find a balance between sensibility and 
		noise isolation

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicStaBbpTuning(RTMP_ADAPTER *pAd)
{
	UCHAR OrigR66Value = 0, R66;/*, R66UpperBound = 0x30, R66LowerBound = 0x30;*/
	CHAR Rssi;
#ifdef RT2883
	UCHAR byteValue = 0;
#endif /* RT2883 */

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		return;
	}

	/* 2860C did not support Fase CCA, therefore can't tune*/
	if (pAd->MACVersion == 0x28600100)
		return;

	/* work as a STA*/
	if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)  /* no R66 tuning when SCANNING*/
		return;

	if ((pAd->OpMode == OPMODE_STA) 
		&& (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)
#ifdef MESH_SUPPORT
			|| MESH_ON(pAd)
#endif /* MESH_SUPPORT */
			)
		&& !(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
#ifdef RTMP_MAC_PCI		
		&& (pAd->bPCIclkOff == FALSE)
#endif /* RTMP_MAC_PCI */
		)
	{
		bbp_get_agc(pAd, &OrigR66Value, RX_CHAIN_0);
		R66 = OrigR66Value;
		
		if (pAd->Antenna.field.RxPath > 1)
			Rssi = (pAd->StaCfg.RssiSample.AvgRssi[0] + pAd->StaCfg.RssiSample.AvgRssi[1]) >> 1;
		else
			Rssi = pAd->StaCfg.RssiSample.AvgRssi[0];

		RTMP_CHIP_ASIC_AGC_ADJUST(pAd, Rssi, R66);

		// TODO: shiang,I didn't find AsicAGCAdjust for RT30xx, so I move following code from upper #if case.
#ifdef RT3290
#ifdef RELEASE_EXCLUDE
		//Customer-Samsung: <1m distance throughput issue
#endif /* RELEASE_EXCLUDE */
		// TODO: shiang, what's the BbpWriteLatch[ ] ?????
		if (IS_RT3290(pAd))
		{
			if (Rssi >= -36)
			{
				if (pAd->BbpWriteLatch[BBP_R77] != 0x27)
				{
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x52);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R77, 0x27);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x1C);
				}
			}
			else
			{
				if (pAd->BbpWriteLatch[BBP_R77] != 0x58)
				{
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R77, 0x58);
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);	
				}
			}
		}
#endif /* RT3290 */

#ifdef RELEASE_EXCLUDE
		if (pAd->Mlme.OneSecPeriodicRound % 4 == 0)
		{
			DBGPRINT(RT_DEBUG_INFO,
				("RSSI=%d, CCA=%d, BW=%d, LNA_GAIN=0x%x, fixed R66 at 0x%x\n",
				Rssi, pAd->RalinkCounters.OneSecFalseCCACnt,
				pAd->CommonCfg.BBPCurrentBW, pAd->hw_cfg.lan_gain, R66));
		}
#endif /* RELEASE_EXCLUDE */
	}
}
#endif /* CONFIG_STA_SUPPORT */


VOID RTMPSetAGCInitValue(RTMP_ADAPTER *pAd, UCHAR BandWidth)
{
	if (pAd->chipOps.ChipAGCInit != NULL)
		pAd->chipOps.ChipAGCInit(pAd, BandWidth);
}


/*
========================================================================
Routine Description:
	Check if the channel has the property.

Arguments:
	pAd				- WLAN control block pointer
	ChanNum			- channel number
	Property		- channel property, CHANNEL_PASSIVE_SCAN, etc.

Return Value:
	TRUE			- YES
	FALSE			- NO

Note:
========================================================================
*/
BOOLEAN CHAN_PropertyCheck(RTMP_ADAPTER *pAd, UINT32 ChanNum, UCHAR Property)
{
	UINT32 IdChan;

	/* look for all registered channels */
	for(IdChan=0; IdChan<pAd->ChannelListNum; IdChan++)
	{
		if (pAd->ChannelList[IdChan].Channel == ChanNum)
		{
			if ((pAd->ChannelList[IdChan].Flags & Property) == Property)
				return TRUE;

			break;
		}
	}

	return FALSE;
}

#ifdef P2P_CHANNEL_LIST_SEPARATE

BOOLEAN P2P_CHAN_PropertyCheck(
	IN PRTMP_ADAPTER	pAd,
	IN UINT32			ChanNum,
	IN UCHAR			Property)
{
	UINT32 IdChan;
	PCFG80211_CTRL pCfg80211_Ctrl = &pAd->cfg80211_ctrl;


	/* look for all registered channels */
	for(IdChan = 0; IdChan < pCfg80211_Ctrl->ChannelListNum; IdChan++)
	{
		if (pCfg80211_Ctrl->ChannelList[IdChan].Channel == ChanNum)
		{
			if ((pCfg80211_Ctrl->ChannelList[IdChan].Flags & Property) == Property)
				return TRUE;

			break;
		}
	}

	return FALSE;
}
#endif /* P2P_CHANNEL_LIST_SEPARATE */



NDIS_STATUS RtmpEnqueueTokenFrame(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR *pAddr,
	IN UCHAR TxRate,
	IN UCHAR AID,
	IN UCHAR apidx,
	IN UCHAR OldUP)
{
	NDIS_STATUS NState = NDIS_STATUS_FAILURE;
	HEADER_802_11 *pNullFr;
	UCHAR *pFrame;
	UINT frm_len;

	NState = MlmeAllocateMemory(pAd, (UCHAR **)&pFrame);
	if (NState == NDIS_STATUS_SUCCESS) 
	{
                UCHAR *qos_p;
                pNullFr = (PHEADER_802_11) pFrame;	
		frm_len = sizeof(HEADER_802_11);

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd) {
			MgtMacHeaderInit(pAd, pNullFr, SUBTYPE_DATA_NULL, 0, pAddr, 
							pAd->ApCfg.MBSSID[apidx].wdev.if_addr,
							pAd->ApCfg.MBSSID[apidx].wdev.bssid);
			pNullFr->FC.ToDs = 0;
			pNullFr->FC.FrDs = 1;
			pNullFr->Frag = 0x0f;
		}
#endif /* CONFIG_AP_SUPPORT */

		pNullFr->FC.Type = FC_TYPE_DATA;
                qos_p = ((UCHAR *)pNullFr) + frm_len;
			pNullFr->FC.SubType = SUBTYPE_QOS_NULL;

			/* copy QOS control bytes */
			qos_p[0] = OldUP;
		   qos_p[1] = PS_RETRIEVE_TOKEN;
			frm_len += 2;

		/* since TxRate may change, we have to change Duration each time */
		pNullFr->Duration = RTMPCalcDuration(pAd, TxRate, frm_len);

		NState = MiniportMMRequest(pAd, OldUP | MGMT_USE_QUEUE_FLAG, (PUCHAR)pNullFr, frm_len);
		MlmeFreeMemory(pAd, pFrame);
	}

   return NState;
}

#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
VOID AsicCheckForHwRecovery(RTMP_ADAPTER *pAd)
{
	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
#ifdef RELEASE_EXCLUDE
		/* Bug 1. BBP dead. Need to Hard-Reset BBP */
#endif /* RELEASE_EXCLUDE */
		if (pAd->RalinkCounters.LastReceivedByteCount == pAd->RalinkCounters.ReceivedByteCount)
		{
			/* If ReceiveByteCount doesn't change, increase SameRxByteCount by 1 */
			pAd->SameRxByteCount++;
		}
		else
		{
			pAd->SameRxByteCount = 0;
			pAd->BbpResetCount = 0;
		}

#ifdef RELEASE_EXCLUDE
		/* 
			If SameRxByteCount keeps happening :
			for 3 second in infra mode
			for 5 seconds in idle mode
			for 1 second in P2PGO 
		*/
#endif /* RELEASE_EXCLUDE */
		if (
#ifdef CONFIG_STA_SUPPORT
			((INFRA_ON(pAd)) && (pAd->SameRxByteCount > 3)) || 
			((IDLE_ON(pAd)) && (pAd->SameRxByteCount > 5)) 
#endif /* CONFIG_STA_SUPPORT */
#ifdef P2P_SUPPORT
			||((P2P_GO_ON(pAd)) && (pAd->SameRxByteCount > 1))
#else
#ifdef CONFIG_AP_SUPPORT
			(pAd->SameRxByteCount > 0)
#endif /* CONFIG_AP_SUPPORT */
#endif /* P2P_SUPPORT */
		)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("AsicCheckForHwRecovery!!\n"));

#ifdef CONFIG_STA_SUPPORT
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE) 
				|| (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND)) 
				|| RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
				AsicForceWakeup(pAd, DOT11POWERSAVE);	
#endif /* CONFIG_STA_SUPPORT */

#ifdef RELEASE_EXCLUDE			
	/*
				Patch Scan no AP issue.
				Firmware will write RF_R08(xtal_vdd_select) to LDOADC. 
				This is HW issue.
				New RT3572(0x35720223) should not have this issue.
			*/
#endif /* RELEASE_EXCLUDE */
			if (IS_RT5392(pAd))
			{
				UCHAR RfValue = 0;

				RT30xxReadRFRegister(pAd, RF_R42, &RfValue);
				RfValue = (RfValue | 0xC0); /* rx_ctb_en, rx_mix2_en */
				RT30xxWriteRFRegister(pAd, RF_R42, RfValue);
			}

			if(pAd->BbpResetFlag == 0)
			{
			AsicResetBBPAgent(pAd);
				pAd->BbpResetFlag = 1;
			}
			pAd->SameRxByteCount = 0;
			pAd->BbpResetCount++;
		}
	}
		
	/* Update lastReceiveByteCount */
	pAd->RalinkCounters.LastReceivedByteCount = pAd->RalinkCounters.ReceivedByteCount;			
}
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) || defined(RT5592) */

