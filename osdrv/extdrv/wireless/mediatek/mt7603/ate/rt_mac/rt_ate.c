/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************
 
 	Module Name:
	rt_ate.c

	Abstract:

	Revision History:
	Who			When	    What
	--------	----------  ----------------------------------------------
	Name		Date	    Modification logs
*/
#include "rt_config.h"

#define ATE_BBP_REG_NUM	168
UCHAR restore_BBP[ATE_BBP_REG_NUM]={0};

/* 802.11 MAC Header, Type:Data, Length:24bytes + 6 bytes QOS/HTC + 2 bytes padding */
UCHAR TemplateFrame[32] = {0x08,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0x00,0xAA,0xBB,0x12,0x34,0x56,0x00,0x11,0x22,0xAA,0xBB,0xCC,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

extern FREQUENCY_ITEM *FreqItems3020;
extern UCHAR NUM_OF_3020_CHNL;

#define TXCONT_TX_PIN_CFG_A 0x040C0050
#define TXCONT_TX_PIN_CFG_G 0x080C00A0

#define ATE_TASK_EXEC_INTV 100
#define ATE_TASK_EXEC_MULTIPLE 10

static CHAR CCKRateTable[] = {0, 1, 2, 3, 8, 9, 10, 11, -1}; /* CCK Mode. */
static CHAR OFDMRateTable[] = {0, 1, 2, 3, 4, 5, 6, 7, -1}; /* OFDM Mode. */
#ifdef DOT11N_SS3_SUPPORT
static CHAR HTMIXRateTable3T3R[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, -1}; /* HT Mix Mode for 3*3. */
#endif /* DOT11N_SS3_SUPPORT */
static CHAR HTMIXRateTable[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 32, -1}; /* HT Mix Mode. */
#ifdef DOT11_VHT_AC
static CHAR VHTACRateTable[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1}; /* VHT AC Mode. */
#endif /* DOT11_VHT_AC */

#ifdef RT5592
extern VOID RTMPReadTxPwrPerRate(IN PRTMP_ADAPTER	pAd);
#endif /* RT5592 */

#ifdef RTMP_INTERNAL_TX_ALC

/* The desired TSSI over CCK */
extern CHAR desiredTSSIOverCCK[4];

/* The desired TSSI over OFDM */
extern CHAR desiredTSSIOverOFDM[8];

#ifdef RT3352
/* The desired TSSI over HT */
extern CHAR desiredTSSIOverHT[16];
#else
/* The desired TSSI over HT */
extern CHAR desiredTSSIOverHT[8];
#endif /* RT3352 */

/* The desired TSSI over HT using STBC */
extern CHAR desiredTSSIOverHTUsingSTBC[8];

/* The Tx power tuning entry*/
extern TX_POWER_TUNING_ENTRY_STRUCT TxPowerTuningTable[];

#if defined( RT3352) || defined(RT3350) || defined(MT7601)
/*
==========================================================================
	Description:
		Get the desired TSSI based on ATE setting.

	Arguments:
		pAd

	Return Value:
		The desired TSSI
==========================================================================
 */
CHAR ATEGetDesiredTSSI(
	IN PRTMP_ADAPTER		pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR desiredTSSI = 0;
	UCHAR MCS = 0;
	UCHAR MaxMCS = 7;
	UCHAR phy_mode = 0, stbc = 0, bw = 0;

#ifdef MT7601
	if (IS_MT7601(pAd))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s::Currently not support singledriver.\n", __FUNCTION__));
		return 0;
 	}
#endif /* MT7601 */

#if defined( RT3352) || defined(RT3350) 
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		MCS = (UCHAR)(pATEInfo->TxWI.TXWI_O.MCS);
		phy_mode = (UCHAR)(pATEInfo->TxWI.TXWI_O.PHYMODE);
		stbc = (UCHAR)(pATEInfo->TxWI.TXWI_O.STBC);
		bw = (UCHAR)(pATEInfo->TxWI.TXWI_O.BW);
	}
#endif /* RTMP_MAC */
	
	if (phy_mode == MODE_CCK)
	{
		if (MCS > 3) /* boundary verification */
		{
			DBGPRINT_ERR(("%s: incorrect MCS: MCS = %d\n", 
				__FUNCTION__, 
				MCS));
			
			MCS = 0;
		}
	
		desiredTSSI = desiredTSSIOverCCK[MCS];
	}
	else if (phy_mode == MODE_OFDM)
	{
		if (MCS > 7) /* boundary verification */
		{
			DBGPRINT_ERR(("%s: incorrect MCS: MCS = %d\n", 
				__FUNCTION__, 
				MCS));

			MCS = 0;
		}

		desiredTSSI = desiredTSSIOverOFDM[MCS];
	}
	else if ((phy_mode == MODE_HTMIX) || (phy_mode == MODE_HTGREENFIELD))
	{
		if (stbc == STBC_NONE)
		{
#ifdef RT3352
			if (IS_RT3352(pAd))
				MaxMCS = 15;
#endif /* RT3352 */
			if (MCS > MaxMCS) /* boundary verification */
			{
				DBGPRINT_ERR(("%s: incorrect MCS: MCS = %d\n", 
					__FUNCTION__, 
					MCS));

				MCS = 0;
			}

			desiredTSSI = desiredTSSIOverHT[MCS];
		}
		else
		{
			if (MCS > MaxMCS) /* boundary verification */
			{
				DBGPRINT_ERR(("%s: incorrect MCS: MCS = %d\n", 
					__FUNCTION__, 
					MCS));

				MCS = 0;
			}

			desiredTSSI = desiredTSSIOverHTUsingSTBC[MCS];
		}

		
		/* 
			For HT BW40 MCS 7 with/without STBC configuration, 
			the desired TSSI value should subtract one from the formula.
		*/
		if ((bw == BW_40) && (MCS == MCS_7))
		{
			desiredTSSI -= 1;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s: desiredTSSI = %d, Latest Tx setting: MODE = %d, MCS = %d, STBC = %d\n", 
		__FUNCTION__, 
		desiredTSSI, 
		phy_mode, 
		MCS, 
		stbc));

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("<--- %s\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

	return desiredTSSI;
#endif /* defined( RT3352) || defined(RT3350) */
}
#endif /* defined( RT3352) || defined(RT3350) || defined(MT7601) */
#endif /* RTMP_INTERNAL_TX_ALC */


/*
==========================================================================
	Description:
		Gives CCK TX rate 2 more dB TX power.
		This routine works only in ATE mode.

		calculate desired Tx power in RF R3.Tx0~5,	should consider -
		0. TxPowerPercentage
		1. auto calibration based on TSSI feedback
		2. extra 2 db for CCK
		3. -10 db upon very-short distance (AvgRSSI >= -40db) to AP

==========================================================================
*/
VOID ATEAsicAdjustTxPower(
	IN PRTMP_ADAPTER pAd) 
{
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->AdjustTxPower != NULL)
		pATEInfo->pChipStruct->AdjustTxPower(pAd);
	else
		DBGPRINT_ERR(("%s: AdjustTxPower() for this chipset does not exist !\n", __FUNCTION__));

	return;
}


#ifdef VCORECAL_SUPPORT
#ifdef RT3593
VOID ATEAsicVCORecalibration(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR RFValue = 0;
/*	UINT32 TxPinCfg = 0; */
	UINT8 mode = pAd->chipCap.FlgIsVcoReCalMode;

	switch (mode)
	{
		case VCO_CAL_MODE_1:
			RT30xxReadRFRegister(pAd, RF_R07, (PUCHAR)&RFValue);
			RFValue = RFValue | 0x01; /* bit 0=vcocal_en*/
			RT30xxWriteRFRegister(pAd, RF_R07, (UCHAR)RFValue);
			break;

		case VCO_CAL_MODE_2:
			RT30xxReadRFRegister(pAd, RF_R03, (PUCHAR)&RFValue);
			RFValue = RFValue | 0x80; /* bit 7=vcocal_en*/
			RT30xxWriteRFRegister(pAd, RF_R03, (UCHAR)RFValue);
			break;

		default:
			return;
	}

}
#endif /* RT3593 */
#endif /* VCORECAL_SUPPORT */


CHAR ATEConvertToRssi(
	IN PRTMP_ADAPTER pAd,
	IN	CHAR	Rssi,
	IN  UCHAR   RssiNumber)
{
	UCHAR	RssiOffset, LNAGain;
	CHAR	BaseVal;

	/* Rssi equals to zero should be an invalid value */
	if (Rssi == 0 || (RssiNumber >= 3))
		return -99;
	
	LNAGain = pAd->hw_cfg.lan_gain;
	if (pAd->LatchRfRegs.Channel > 14)
		RssiOffset = pAd->ARssiOffset[RssiNumber];
	else
		RssiOffset = pAd->BGRssiOffset[RssiNumber];

	BaseVal = -12;
#ifdef RT6352
	if (IS_RT6352(pAd))
		BaseVal = -2;
#endif /* RT6352 */

#ifdef RT65xx
	if (IS_RT65XX(pAd))
		return (Rssi - LNAGain - RssiOffset);

	if (IS_MT76x2(pAd)) {
		if (pAd->LatchRfRegs.Channel > 14)
			return (Rssi + pAd->ARssiOffset[RssiNumber] - (CHAR)LNAGain);
	else
			return (Rssi + pAd->BGRssiOffset[RssiNumber] - (CHAR)LNAGain);
	}

#endif /* RT65xx */
		return (BaseVal - RssiOffset - LNAGain - Rssi);
}


VOID ATESampleRssi(
	IN RTMP_ADAPTER *pAd,
	IN RX_BLK *pRxBlk)
{
	RXWI_STRUC *pRxWI = pRxBlk->pRxWI;
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR rssi[3] = {0};
	CHAR snr[3] = {0};

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
		rssi[0] = pRxWI->RXWI_N.rssi[0];
		rssi[1] = pRxWI->RXWI_N.rssi[1];
		rssi[2] = pRxWI->RXWI_N.rssi[2];
		
		if ( IS_MT76x2(pAd) ) {
			snr[0] = pRxWI->RXWI_N.bbp_rxinfo[2];
			snr[1] = pRxWI->RXWI_N.bbp_rxinfo[3];
			snr[2] = pRxWI->RXWI_N.bbp_rxinfo[4];
		} else {
		snr[0] = pRxWI->RXWI_N.bbp_rxinfo[0];
		snr[1] = pRxWI->RXWI_N.bbp_rxinfo[1];
		snr[2] = pRxWI->RXWI_N.bbp_rxinfo[2];
	}
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		rssi[0] = pRxWI->RXWI_O.RSSI0;
		rssi[1] = pRxWI->RXWI_O.RSSI1;
		rssi[2] = pRxWI->RXWI_O.RSSI2;

		snr[0] = pRxWI->RXWI_O.SNR0;
		snr[1] = pRxWI->RXWI_O.SNR1;
		snr[2] = pRxWI->RXWI_O.SNR2;
	}
#endif /* RTMP_MAC */

	if (rssi[0] != 0)
	{
		pATEInfo->LastRssi0	= ATEConvertToRssi(pAd, rssi[0], RSSI_IDX_0);
		pATEInfo->AvgRssi0X8 = (pATEInfo->AvgRssi0X8 - pATEInfo->AvgRssi0) + pATEInfo->LastRssi0;
		pATEInfo->AvgRssi0 = pATEInfo->AvgRssi0X8 >> 3;
	}

	if (rssi[1]!= 0)
	{
		pATEInfo->LastRssi1	= ATEConvertToRssi(pAd, rssi[1], RSSI_IDX_1);
		pATEInfo->AvgRssi1X8 = (pATEInfo->AvgRssi1X8 - pATEInfo->AvgRssi1) + pATEInfo->LastRssi1;
		pATEInfo->AvgRssi1 = pATEInfo->AvgRssi1X8 >> 3;
	}

	if (rssi[2]!= 0)
	{
		pATEInfo->LastRssi2	= ATEConvertToRssi(pAd, rssi[2], RSSI_IDX_2);
		pATEInfo->AvgRssi2X8 = (pATEInfo->AvgRssi2X8 - pATEInfo->AvgRssi2) + pATEInfo->LastRssi2;
		pATEInfo->AvgRssi2 = pATEInfo->AvgRssi2X8 >> 3;
	}

	pATEInfo->LastSNR0 = snr[0];
	pATEInfo->LastSNR1 = snr[1];
#ifdef DOT11N_SS3_SUPPORT
	pATEInfo->LastSNR2 = snr[2];
#endif /* DOT11N_SS3_SUPPORT */

	pATEInfo->NumOfAvgRssiSample ++;

	return;
}


VOID rt_ee_read_all(PRTMP_ADAPTER pAd, USHORT *Data)
{
	USHORT offset = 0;
	USHORT value;

	for (offset = 0; offset < (EEPROM_SIZE >> 1);)
	{
#ifdef RELEASE_EXCLUDE
		/* "value" is especially for some compilers... */
#endif /* RELEASE_EXCLUDE */
		RT28xx_EEPROM_READ16(pAd, (offset << 1), value);
		Data[offset] = value;
		offset++;
	}

	return;
}


VOID rt_ee_write_all(PRTMP_ADAPTER pAd, USHORT *Data)
{
	USHORT offset = 0;
	USHORT value;

#ifdef RTMP_USB_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_USB)
	{
#ifdef MT7601
		if ( IS_MT7601(pAd) )
		{
			USHORT	length = 0x100;
			UCHAR	*ptr = (UCHAR *)Data;
			UCHAR	index;
			UCHAR	AllFF[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
			for ( offset = 0 ; offset < length ; offset += 16 )
			{
				if ( memcmp( ptr, AllFF, 16 ) )
				{
					for ( index = 0 ; index < 16 ; index += 2 )
					{
						value = *(USHORT *)(ptr + index);
						eFuseWrite(pAd, offset + index ,&value, 2);
					}
				}
				
				ptr += 16;
			}
			return;
		}
		else
#endif /* MT7601 */
		{
		USHORT offset = 0;
		USHORT length = EEPROM_SIZE;

		RTUSBWriteEEPROM(pAd, offset, (UCHAR *)Data, length);
		return;
	}
	}
#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_FLASH_SUPPORT
	if ((IS_RT6352(pAd)) || (IS_MT76x0(pAd)))
	{
		rtmp_ee_flash_write_all(pAd, Data);
		return;
	}
#endif /* RTMP_FLASH_SUPPORT */

	for (offset = 0; offset < (EEPROM_SIZE >> 1);)
	{
#ifdef RELEASE_EXCLUDE
		/* "value" is especially for some compilers... */
#endif /* RELEASE_EXCLUDE */
		value = Data[offset];
		RT28xx_EEPROM_WRITE16(pAd, (offset << 1), value);
		offset++;
	}

	return;
}


VOID rt_ee_write_bulk(PRTMP_ADAPTER pAd, USHORT *Data, USHORT offset, USHORT length)
{
	USHORT pos;
	USHORT value;
	USHORT len = length;

	for (pos = 0; pos < (len >> 1);)
	{
#ifdef RELEASE_EXCLUDE
		/* "value" is especially for some compilers... */
#endif /* RELEASE_EXCLUDE */
		value = Data[pos];
		RT28xx_EEPROM_WRITE16(pAd, offset+(pos*2), value);
		pos++;
	}

	return;
}

#ifdef RT_RF
VOID RtmpRfIoWrite(
	IN PRTMP_ADAPTER pAd)
{
	/* Set RF value 1's set R3[bit2] = [0] */
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAd, (pAd->LatchRfRegs.R3 & (~0x04)));
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R4);

	RtmpusecDelay(200);

	/* Set RF value 2's set R3[bit2] = [1] */
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAd, (pAd->LatchRfRegs.R3 | 0x04));
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R4);

	RtmpusecDelay(200);

	/* Set RF value 3's set R3[bit2] = [0] */
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAd, (pAd->LatchRfRegs.R3 & (~0x04)));
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R4);

	return;
}
#endif /* RT_RF */

VOID ATEAsicSetTxRxPath(
    IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->AsicSetTxRxPath != NULL)
		pATEInfo->pChipStruct->AsicSetTxRxPath(pAd);

	return;
}


/*
==========================================================================
    Description:

	AsicSwitchChannel() dedicated for ATE.
    
==========================================================================
*/
VOID ATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd) 
{
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->ChannelSwitch != NULL)
		pATEInfo->pChipStruct->ChannelSwitch(pAd);

	return;
}


VOID BbpSoftReset(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		UCHAR BbpData = 0;

		/* Soft reset, set BBP R21 bit0=1->0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R21, &BbpData);
		BbpData |= 0x00000001; /* set bit0=1 */
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BbpData);

		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R21, &BbpData);
		BbpData &= ~(0x00000001); /* set bit0=0 */
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BbpData);
	}
#endif /* RTMP_BBP */
	return;
}


#ifdef MT76x2
VOID ITxBfBbpInit(
	IN PRTMP_ADAPTER pAd)
{
	RTMP_IO_WRITE32(pAd, AGC1_R0, 0x00007408);
	RTMP_IO_WRITE32(pAd, TXO_R0,  0x00000020);
	RTMP_IO_WRITE32(pAd, TXBE_R0, 0x00000000);
	RTMP_IO_WRITE32(pAd, TXBE_R4, 0x00000008);
	RTMP_IO_WRITE32(pAd, CAL_R1, 0x00000006);
	RTMP_IO_WRITE32(pAd, CAL_R2, 0x00000005);
	RTMP_IO_WRITE32(pAd, CAL_R3, 0x00000000);
	RTMP_IO_WRITE32(pAd, CAL_R4, 0x00000000);
	RTMP_IO_WRITE32(pAd, CAL_R5, 0x00000400);
	RTMP_IO_WRITE32(pAd, CAL_R6, 0x00000000);
	RTMP_IO_WRITE32(pAd, CAL_R7, 0x00000000);
	RTMP_IO_WRITE32(pAd, MAC_CSR0, 0x00000000);
	return;
}
#endif


#ifdef RTMP_BBP
static VOID BbpHardReset(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 MacData = 0;

#ifdef RELEASE_EXCLUDE
	/* RT35xx ATE will reuse this code segment. */
	/* hardware reset BBP */
#endif /* RELEASE_EXCLUDE */

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData = MacData | 0x00000002;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	RtmpOsMsDelay(10);

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData = MacData & ~(0x00000002);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	return;
}
#endif

int CheckMCSValid(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR Mode,
	IN UCHAR Mcs)
{
	int index;
	PCHAR pRateTab = NULL;

	switch (Mode)
	{
		case MODE_CCK:
			pRateTab = CCKRateTable;
			break;
		case MODE_OFDM:
			pRateTab = OFDMRateTable;
			break;
			
		case 2: /*MODE_HTMIX*/
		case 3: /*MODE_HTGREENFIELD*/
#ifdef DOT11N_SS3_SUPPORT
			if (IS_RT2883(pAd) || IS_RT3883(pAd) || IS_RT3593(pAd))
				pRateTab = HTMIXRateTable3T3R;
			else
#endif /* DOT11N_SS3_SUPPORT */
				pRateTab = HTMIXRateTable;
			break;
#ifdef DOT11_VHT_AC
		case MODE_VHT: 
			pRateTab = VHTACRateTable;
			break;
#endif /* DOT11_VHT_AC */
		default: 
			DBGPRINT_ERR(("unrecognizable Tx Mode %d\n", Mode));
			return -1;
			break;
	}

	index = 0;
	while (pRateTab[index] != -1)
	{
		if (pRateTab[index] == Mcs)
			return 0;
		index++;
	}

	return -1;
}


INT ATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->TxPwrHandler != NULL)
		pATEInfo->pChipStruct->TxPwrHandler(pAd, index);

	return 0;
}


/*
========================================================================

	Routine Description:
		Set Japan filter coefficients if needed.
	Note:
		This routine should only be called when
		entering TXFRAME mode or TXCONT mode.
				
========================================================================
*/
static VOID SetJapanFilter(RTMP_ADAPTER *pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR bw = 0, phy_mode = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
		bw = pATEInfo->TxWI.TXWI_N.BW;
		phy_mode = pATEInfo->TxWI.TXWI_N.PHYMODE;
	}
#endif /* RLT_MAC*/

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		bw = pATEInfo->TxWI.TXWI_O.BW;
		phy_mode = pATEInfo->TxWI.TXWI_O.PHYMODE;
	}
#endif /* RTMP_MAC */

#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		UCHAR BbpData = 0;
	
		/*
			If Channel=14 and Bandwidth=20M and Mode=CCK, set BBP R4 bit5=1
			(Japan Tx filter coefficients)when (TXFRAME or TXCONT).
		*/
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BbpData);

		if ((phy_mode == MODE_CCK) && (pATEInfo->Channel == 14) && (bw == BW_20))
		{
			BbpData |= 0x20;    /* turn on */
			DBGPRINT(RT_DEBUG_TRACE, ("SetJapanFilter!!!\n"));
		}
		else
		{
			BbpData &= 0xdf;    /* turn off */
			DBGPRINT(RT_DEBUG_TRACE, ("ClearJapanFilter!!!\n"));
		}

		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpData);
	}
#endif /* RTMP_BBP */
	return;
}


/*
========================================================================

	Routine Description:
		Disable protection for ATE.
========================================================================
*/
VOID ATEDisableAsicProtect(
	IN		PRTMP_ADAPTER	pAd)
{
#if defined(RTMP_MAC) || defined(RLT_MAC)
	PROT_CFG_STRUC	ProtCfg, ProtCfg4;
	UINT32 Protect[6];
	USHORT			offset;
	UCHAR			step;
	UINT32 MacReg = 0;

	/* Config ASIC RTS threshold register */
	RTMP_IO_READ32(pAd, TX_RTS_CFG, &MacReg);
	MacReg &= 0xFF0000FF;
	MacReg |= (0xFFF << 8);
	RTMP_IO_WRITE32(pAd, TX_RTS_CFG, MacReg);

	/* Initial common protection settings */
	RTMPZeroMemory(Protect, sizeof(Protect));
	ProtCfg4.word = 0;
	ProtCfg.word = 0;
	ProtCfg.field.TxopAllowGF40 = 1;
	ProtCfg.field.TxopAllowGF20 = 1;
	ProtCfg.field.TxopAllowMM40 = 1;
	ProtCfg.field.TxopAllowMM20 = 1;
	ProtCfg.field.TxopAllowOfdm = 1;
	ProtCfg.field.TxopAllowCck = 1;
	ProtCfg.field.RTSThEn = 1;
	ProtCfg.field.ProtectNav = ASIC_SHORTNAV;

	/* Handle legacy(B/G) protection */
	ProtCfg.field.ProtectRate = pAd->CommonCfg.RtsRate;
	ProtCfg.field.ProtectCtrl = 0;
	Protect[0] = ProtCfg.word;
	Protect[1] = ProtCfg.word;
	/* CTS-self is not used */
	pAd->FlgCtsEnabled = 0; 

	/*
		NO PROTECT 
			1.All STAs in the BSS are 20/40 MHz HT
			2. in a 20/40MHz BSS
			3. all STAs are 20MHz in a 20MHz BSS
		Pure HT. no protection.
	*/
	/*
		MM20_PROT_CFG
			Reserved (31:27)
			PROT_TXOP(25:20) -- 010111
			PROT_NAV(19:18)  -- 01 (Short NAV protection)
			PROT_CTRL(17:16) -- 00 (None)
			PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)
	*/
	Protect[2] = 0x01744004;	

	/*
		MM40_PROT_CFG
			Reserved (31:27)
			PROT_TXOP(25:20) -- 111111
			PROT_NAV(19:18)  -- 01 (Short NAV protection)
			PROT_CTRL(17:16) -- 00 (None) 
			PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)
	*/
	Protect[3] = 0x03f44084;

	/*
		CF20_PROT_CFG
			Reserved (31:27)
			PROT_TXOP(25:20) -- 010111
			PROT_NAV(19:18)  -- 01 (Short NAV protection)
			PROT_CTRL(17:16) -- 00 (None)
			PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)
	*/
	Protect[4] = 0x01744004;

	/*
		CF40_PROT_CFG
			Reserved (31:27)
			PROT_TXOP(25:20) -- 111111
			PROT_NAV(19:18)  -- 01 (Short NAV protection)
			PROT_CTRL(17:16) -- 00 (None)
			PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)
	*/
	Protect[5] = 0x03f44084;

	pAd->CommonCfg.IOTestParm.bRTSLongProtOn = FALSE;
	
	offset = CCK_PROT_CFG;
	for (step = 0;step < 6;step++)
		RTMP_IO_WRITE32(pAd, offset + step*4, Protect[step]);
#else
//TODO: MT_MAC disable protect
#endif

	return;
}


#ifdef CONFIG_AP_SUPPORT 
/*
==========================================================================
	Description:
		Used only by ATE to disassociate all STAs and stop AP service.
	Note:
==========================================================================
*/
VOID ATEAPStop(
	IN PRTMP_ADAPTER pAd) 
{
	BOOLEAN     Cancelled;
	UINT32		Value = 0;
	INT         apidx = 0;
		
	DBGPRINT(RT_DEBUG_TRACE, ("!!! ATEAPStop !!!\n"));

	/* To prevent MCU to modify BBP registers w/o indications from driver. */
#ifdef DFS_SUPPORT
		NewRadarDetectionStop(pAd);
#endif /* DFS_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
	if (pAd->CommonCfg.CarrierDetect.Enable == TRUE)
	{
		CarrierDetectionStop(pAd);
	}
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef MESH_SUPPORT
	if (MESH_ON(pAd))
		MeshDown(pAd, TRUE);
#endif /* MESH_SUPPORT */

#ifdef WDS_SUPPORT
	WdsDown(pAd);
#endif /* WDS_SUPPORT */

#ifdef APCLI_SUPPORT
	ApCliIfDown(pAd);
#endif /* APCLI_SUPPORT */

	MacTableReset(pAd, 1);

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Disable pre-tbtt interrupt */
	RTMP_IO_READ32(pAd, INT_TIMER_EN, &Value);
	Value &=0xe;
	RTMP_IO_WRITE32(pAd, INT_TIMER_EN, Value);
#elif defined(MT_MAC)
	//TODO: Carter, MT_MAC disable pre-tbtt interrupt? is it necessary?
#endif
	/* Disable piggyback */
	RTMPSetPiggyBack(pAd, FALSE);

	ATEDisableAsicProtect(pAd);

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
	{
		AsicDisableSync(pAd);

#ifdef LED_CONTROL_SUPPORT
#ifdef RT35xx
	if (IS_RT3572(pAd))
		RTMPSetLED(pAd, LED_LINK_DOWN);
#endif /* RT35xx */
#endif /* LED_CONTROL_SUPPORT */
	}

#ifdef RTMP_MAC_USB
	/* For USB, we need to clear the beacon sync buffer. */
	RTUSBBssBeaconExit(pAd);
#endif /* RTMP_MAC_USB */

	for (apidx = 0; apidx < MAX_MBSSID_NUM(pAd); apidx++)
	{
		if (pAd->ApCfg.MBSSID[apidx].REKEYTimerRunning == TRUE)
		{
			RTMPCancelTimer(&pAd->ApCfg.MBSSID[apidx].REKEYTimer, &Cancelled);
			pAd->ApCfg.MBSSID[apidx].REKEYTimerRunning = FALSE;
		}
	}

	if (pAd->ApCfg.CMTimerRunning == TRUE)
	{
		RTMPCancelTimer(&pAd->ApCfg.CounterMeasureTimer, &Cancelled);
		pAd->ApCfg.CMTimerRunning = FALSE;
	}
	
#ifdef WAPI_SUPPORT
	RTMPCancelWapiRekeyTimerAction(pAd, NULL);
#endif /* WAPI_SUPPORT */
	
	/* Cancel the Timer, to make sure the timer was not queued. */
	OPSTATUS_CLEAR_FLAG(pAd, fOP_AP_STATUS_MEDIA_STATE_CONNECTED);
	RTMP_IndicateMediaState(pAd, NdisMediaStateDisconnected);

	if (pAd->ApCfg.ApQuickResponeForRateUpTimerRunning == TRUE)
		RTMPCancelTimer(&pAd->ApCfg.ApQuickResponeForRateUpTimer, &Cancelled);

#ifdef IDS_SUPPORT
	/* if necessary, cancel IDS timer */
	RTMPIdsStop(pAd);
#endif /* IDS_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
	FT_Release(pAd);
#endif /* DOT11R_FT_SUPPORT */

#ifdef GREENAP_SUPPORT
	if (pAd->ApCfg.bGreenAPEnable == TRUE)
	{
		RTMP_CHIP_DISABLE_AP_MIMOPS(pAd);
		pAd->ApCfg.GreenAPLevel=GREENAP_WITHOUT_ANY_STAS_CONNECT;
		pAd->ApCfg.bGreenAPEnable = FALSE;
	}
#endif /* GREENAP_SUPPORT */

#ifdef DOT11V_WNM_SUPPORT
	DMSTable_Release(pAd);
#endif /* DOT11V_WNM_SUPPORT */

#ifdef SMART_ANTENNA
	RtmpSAStop(pAd);
#endif /* SMART_ANTENNA */

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R186, 0x00);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R187, 0x00);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R188, 0x00);
	}
#endif /* RT6352 */

#ifdef DYNAMIC_VGA_SUPPORT
	if (pAd->CommonCfg.lna_vga_ctl.bDyncVgaEnable == TRUE)
	{
		dynamic_vga_disable(pAd);
	}
#endif /* DYNAMIC_VGA_SUPPORT */

}
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
VOID RTMPStationStop(
    IN  PRTMP_ADAPTER   pAd)
{
    DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPStationStop\n"));

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R186, 0x00);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R187, 0x00);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R188, 0x00);
	}
#endif /* RT6352 */

    /* Do nothing. */

    DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPStationStop\n"));
}


VOID RTMPStationStart(
    IN  PRTMP_ADAPTER   pAd)
{
    DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPStationStart\n"));

#ifdef RTMP_MAC_PCI
	pAd->Mlme.CntlMachine.CurrState = CNTL_IDLE;
#endif /* RTMP_MAC_PCI */

	DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPStationStart\n"));
}
#endif /* CONFIG_STA_SUPPORT */


#ifdef HUAWEI_ATE
#ifdef CONFIG_AP_SUPPORT
static VOID ATEAsicSetBssid(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pBssid) 
{
	ULONG		  Addr4;

	DBGPRINT(RT_DEBUG_TRACE, ("==> ATEAsicSetBssid %x:%x:%x:%x:%x:%x\n",
		pBssid[0],pBssid[1],pBssid[2],pBssid[3], pBssid[4],pBssid[5]));
	
	Addr4 = (ULONG)(pBssid[0])		 | 
			(ULONG)(pBssid[1] << 8)  | 
			(ULONG)(pBssid[2] << 16) |
			(ULONG)(pBssid[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_BSSID_DW0, Addr4);

	Addr4 = 0;
	/* always one BSSID in STA mode*/
	Addr4 = (ULONG)(pBssid[4]) | (ULONG)(pBssid[5] << 8);

	RTMP_IO_WRITE32(pAd, MAC_BSSID_DW1, Addr4);
}
#endif /* CONFIG_AP_SUPPORT */
#endif /* HUAWEI_ATE */


#ifdef RLT_BBP
static INT ate_bbp_core_soft_reset(RTMP_ADAPTER *pAd, BOOLEAN set_bw, INT bw)
{
	UINT32 bbp_val;

	RTMP_BBP_IO_READ32(pAd, CORE_R4, &bbp_val);
	bbp_val |= 0x1;
	RTMP_BBP_IO_WRITE32(pAd, CORE_R4, bbp_val);
	RtmpusecDelay(1000);

	if (set_bw == TRUE)
	{
		RTMP_BBP_IO_READ32(pAd, CORE_R1, &bbp_val);
		bbp_val &= (~0x18);
		switch (bw)
		{
			case BW_40:
				bbp_val |= 0x10;
				break;
			case BW_80:
				bbp_val |= 0x18;
				break;
			case BW_20:
			default:
					break;
		}			
		RTMP_BBP_IO_WRITE32(pAd, CORE_R1, bbp_val);
		RtmpusecDelay(1000);
	}
	RTMP_BBP_IO_READ32(pAd, CORE_R4, &bbp_val);
	bbp_val &= (~0x1);
	RTMP_BBP_IO_WRITE32(pAd, CORE_R4, bbp_val);
	RtmpusecDelay(1000);

	return 0;
}
#endif /* RLT_BBP */

static NDIS_STATUS ATESTART(
	IN PRTMP_ADAPTER pAd)
{
	ATE_INFO *pATEInfo = &(pAd->ate);
	UINT32			MacData=0, atemode=0, temp=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#ifdef RTMP_MAC_PCI
	UINT32			ring_index=0;
	TXD_STRUC		*pTxD = NULL;
#ifdef RT_BIG_ENDIAN
	TXD_STRUC      *pDestTxD = NULL;
	UCHAR tx_hw_info[TXD_SIZE];
#endif /* RT_BIG_ENDIAN */
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
	UCHAR LoopCount=0;
#endif /* RTMP_MAC_USB */
	PATE_CHIP_STRUCT pChipStruct = pATEInfo->pChipStruct;
	BOOLEAN Cancelled;
#ifdef HUAWEI_ATE
#ifdef CONFIG_AP_SUPPORT
	PUCHAR			pFilter = pATEInfo->Filter;
#endif /* CONFIG_AP_SUPPORT */
#endif /* HUAWEI_ATE */
#ifdef RLT_BBP
	UINT32 bbp_val;
#endif /* RLT_BBP */
#ifdef RTMP_BBP
	UCHAR BbpData = 0;
#endif /* RTMP_BBP */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

#ifdef MT76x0
#ifdef MT76x0_TSSI_CAL_COMPENSATION
	if ((IS_MT76x0(pAd)) && (pAd->ate.bQAEnabled == FALSE) && (pATEInfo->Mode == ATE_STOP))
		MT76x0ATE_MakeUpTssiTable(pAd);
#endif /* MT76x0_TSSI_CAL_COMPENSATION */
#endif /* MT76x0 */

#ifdef RT3593
#ifdef RELEASE_EXCLUDE
	/* for recovering RT3593 RX sensitivity after TXCARR */
#endif /* RELEASE_EXCLUDE */
	if ((IS_RT3593(pAd)) && (pAd->ate.bQAEnabled == FALSE) && (pAd->ate.Mode == ATE_TXCARR))
	{
#ifdef CONFIG_RT2880_ATE_CMD_NEW
		Set_ATE_Proc(pAd, "ATESTOP");
#else
		Set_ATE_Proc(pAd, "APSTART");
#endif /* CONFIG_RT2880_ATE_CMD_NEW */
	}
#endif /* RT3593 */

#ifdef RTMP_MAC_PCI
	if (!(ATE_ON(pAd)))
	{
		for (ring_index = 0; ring_index < TX_RING_SIZE; ring_index++)
		{
			if (ATEPayloadAlloc(pAd, ring_index) != (NDIS_STATUS_SUCCESS))
			{
				return (NDIS_STATUS_FAILURE);
			}
		}
		/* check if we have removed the firmware */
		if (IS_PCI_INF(pAd))
		{
			if (pAd->chipCap.MCUType == M8051)
			{
				NICEraseFirmware(pAd);
			}
		}
	}
#endif /* RTMP_MAC_PCI */

	atemode = pATEInfo->Mode;
	pATEInfo->Mode = ATE_START;

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		UINT32 mac_value;

		/* reset/disable DPD */
		pATEInfo->bDPDEnable = FALSE;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R186, 0x00);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R187, 0x00);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R188, 0x00);
		pAd->Tx0_DPD_ALC_tag0 = 0;
		pAd->Tx0_DPD_ALC_tag1 = 0;
		pAd->Tx1_DPD_ALC_tag0 = 0;
		pAd->Tx1_DPD_ALC_tag1 = 0;
		pAd->Tx0_DPD_ALC_tag0_flag = 0x0;
		pAd->Tx0_DPD_ALC_tag1_flag = 0x0;
		pAd->Tx1_DPD_ALC_tag0_flag = 0x0;
		pAd->Tx1_DPD_ALC_tag1_flag = 0x0;

		if (atemode == ATE_STOP)
		{
			RT6352_RTMPReadTxPwrPerRate(pAd);
			RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &mac_value);
			mac_value &= 0xFFFFFFC0;
			RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, mac_value);
		}
	}
#endif /* RT6352 */
#ifdef MT76x0
	if ((atemode == ATE_STOP) && (IS_MT76x0(pAd)))
	{
		UINT32 mac_value;

		MT76x0AteReadTxPwrPerRate(pAd);
		RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &mac_value);
		mac_value &= 0xFFFFFFC0;
		RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, mac_value);
#ifdef RTMP_MAC_PCI
#ifdef RTMP_EFUSE_SUPPORT
		if ((pAd->bUseEfuse == TRUE) && (pATEInfo->bQAEnabled == FALSE))
		{
			/* supposed it should be buffer mode */
			MT76x0AteReadBufferAndInitAsic(pAd);
		}
#endif /* RTMP_EFUSE_SUPPORT */
#endif /* RTMP_MAC_PCI */
	}
#endif /* MT76x0 */
	if ((atemode == ATE_STOP) && ((!IS_MT7610(pAd)) && (!IS_RT6352(pAd)) && (!IS_MT76x2(pAd))))
	{
		if (pATEInfo->bQAEnabled == FALSE)
		{
			if ((!IS_MT76x0(pAd)) && (!IS_RT6352(pAd)) && (!IS_MT76x2(pAd)))
			{
				/* DUT just enters ATE mode from normal mode. */
				/* Only at this moment, we need to switch back to the channel of normal mode. */
#ifdef RELEASE_EXCLUDE
				/*
				Set the channel parameters because of the station will call swtiching channels during scanning.
				The ATESTART will block it and cause the RF could not receive any packets and the rx_done_complete
				will not be called anymore.
				*/
#endif /* RELEASE_EXCLUDE */
				AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
				/* empty function */
				AsicLockChannel(pAd, pAd->CommonCfg.Channel);
			}
#ifdef HUAWEI_ATE
#ifdef CONFIG_AP_SUPPORT 
		/* 
			The original BSSID will be recovered
			by APStartUp() in ATESTOP.
		*/
		/* Set the default MAC filter address */ 
		ATEAsicSetBssid(pAd, pFilter);
#endif /* CONFIG_AP_SUPPORT */
#endif /* HUAWEI_ATE */
    }
#if defined(RT5592) || defined(RT3593)
		if (IS_RT5592(pAd) || IS_RT3593(pAd))
		{
			/* single sku will change BBP_R1 */
			/* reset BBP_R1[1:0] for both ATE and QA */
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BbpData);
			BbpData &= ~((1 << 1) | (1 << 0));	
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BbpData);

			if (pATEInfo->bQAEnabled == FALSE)
			{
				/* read per-rate power from EEPROM again */
				RTMPReadTxPwrPerRate(pAd);
			}
		}
#endif /* defined(RT5592) || defined(RT3593) */
	}

	if ( (pAd->ate.bQAEnabled == FALSE) && (atemode == ATE_STOP) )
		ATE_ASIC_CALIBRATION(pAd, ATE_START);

#ifdef RTMP_MAC_USB
#ifdef MT7601
	if (IS_MT7601(pAd))
		MT7601DisableTxRx(pAd, GUIRADIO_OFF);
#endif /* MT7601 */

#ifdef MT76x2
	if (IS_MT76x2(pAd))
	{
		/* BBP initialization for MT76x2 */
#ifdef TXBF_SUPPORT
		ITxBfBbpInit(pAd);
#endif		
		DISABLE_TX_RX(pAd, GUIRADIO_OFF);
	}
#endif /* MT76x2 */

#ifdef RELEASE_EXCLUDE
	/*
		If there is an peding rx urb, the ATE will be block until there are not any pending rx urb.
	*/
	/* If LoopCount is too large(e.g., 5), QA will report something wrong. */
#endif /* RELEASE_EXCLUDE */

	/* one second is enough for waiting bulk-in urb */
	while ((pAd->PendingRx > 0) && (LoopCount < 2))	
	{
		/* delay 0.5 seconds */
		OS_WAIT(500);
		LoopCount++;
	}
#endif /* RTMP_MAC_USB */

	/* Disable Rx */
#if defined(RTMP_MAC) || defined(RLT_MAC)
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC Rx disable
#endif

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Disable auto responder */
	RTMP_IO_READ32(pAd, AUTO_RSP_CFG, &temp);
	temp = temp & 0xFFFFFFFE;
	RTMP_IO_WRITE32(pAd, AUTO_RSP_CFG, temp);
#else
	//TODO: MT_MAC disable responder
#endif

#ifdef RELEASE_EXCLUDE
	/* clear bit4 to stop continuous Tx production test */
	/* Disable or cancel pending irp first ??? */
#endif /* RELEASE_EXCLUDE */
#if defined(RTMP_MAC) || defined(RLT_MAC)
	ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC is need?
#endif

	if (atemode == ATE_TXCARR)
	{
		if (pChipStruct->bBBPStoreTXCARR == TRUE)
		{
			UCHAR RestoreRfICType=pAd->RfIcType;

#ifdef RTMP_BBP
			BbpHardReset(pAd);

			if (pAd->chipCap.hif_type == HIF_RTMP)
			{
				UINT32 bbp_id = 0;

				/* Restore All BBP Value */
				for (bbp_id = 0; bbp_id < ATE_BBP_REG_NUM; bbp_id++)
					ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, bbp_id, restore_BBP[bbp_id]);
			}
#endif /* RTMP_BBP */
#ifdef RELEASE_EXCLUDE
			/*
				The RfIcType will be reset to zero after the hardware reset bbp command.
				Therefore, we must restore the original RfIcType.
			*/
#endif /* RELEASE_EXCLUDE */
			pAd->RfIcType=RestoreRfICType;
		}
				
		if (pATEInfo->TxMethod == TX_METHOD_1)
		{
#ifdef RLT_BBP
#ifdef RT65xx
			if (IS_MT7610(pAd))
			{
				RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
				bbp_val &= ~((0x1 << 14) | (0x1 << 8));
				RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);

				ate_bbp_core_soft_reset(pAd, FALSE, 0);
			}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
			if (pAd->chipCap.hif_type == HIF_RTMP)
			{
				/* No Carrier Test set BBP R22 bit6=0, bit[5~0]=0x0 */
				ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
				BbpData &= 0xFFFFFF80; /* clear bit6, bit[5~0] */	
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
				BbpSoftReset(pAd);
			}
#endif /* RTMP_BBP */
#if defined(RTMP_MAC) || defined(RLT_MAC)
			RTMP_IO_WRITE32(pAd, TX_PIN_CFG, (pATEInfo->Default_TX_PIN_CFG));
#else
	//TODO: MT_MAC check
#endif
		}
		else
		{
#ifdef RTMP_BBP
			if (pAd->chipCap.hif_type == HIF_RTMP)
			{
				/* No Carrier Test set BBP R22 bit7=0, bit6=0, bit[5~0]=0x0 */
				ATE_BBP_RESET_TX_MODE(pAd, BBP_R22, &BbpData);
			}
#endif /* RTMP_BBP */
		}
	}
	else if (atemode == ATE_TXCARRSUPP)
	{
		if (pChipStruct->bBBPStoreTXCARRSUPP == TRUE)
		{
			UINT32			bbp_index=0;
			UCHAR			RestoreRfICType=pAd->RfIcType;

#ifdef RTMP_BBP
			BbpHardReset(pAd);

			/* Restore All BBP Value */
			for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);
#endif /* RTMP_BBP */
#ifdef RELEASE_EXCLUDE
			/*
				The RfIcType will be reset to zero after the hardware reset bbp command.
				Therefore, we must restore the original RfIcType.
			*/
#endif /* RELEASE_EXCLUDE */
			pAd->RfIcType=RestoreRfICType;			
		}

#ifdef RT6352
		/* No Carrier Suppression set BBP R21 bit4=0 */
		if (IS_RT6352(pAd))
		{
			BbpData = 0x00;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BbpData);
		}
#endif /* RT6352 */

#ifdef RLT_BBP
#ifdef RT65xx
		if (IS_MT7610(pAd))
		{
			RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
			bbp_val &= ~(0x1 << 15);
			RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);

			RTMP_BBP_IO_READ32(pAd, TXC_R1, &bbp_val);
			bbp_val &= ~(0x1 << 0);
			RTMP_BBP_IO_WRITE32(pAd, TXC_R1, bbp_val);
		}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
		if (pAd->chipCap.hif_type == HIF_RTMP)
		{
			/* No Cont. TX set BBP R22 bit7=0 */
			ATE_BBP_STOP_CTS_TX_MODE(pAd, BBP_R22, &BbpData);
			/* No Carrier Suppression set BBP R24 bit0=0 */
			ATE_BBP_CTS_TX_SIN_WAVE_DISABLE(pAd, BBP_R24, &BbpData);
		}
#endif /* RTMP_BBP */

		if (pATEInfo->TxMethod == TX_METHOD_1)
		{
#ifdef RLT_BBP
#ifdef RT65xx
			if (IS_MT7610(pAd))
			{
				ate_bbp_core_soft_reset(pAd, FALSE, 0);
			}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
			if (pAd->chipCap.hif_type == HIF_RTMP)
				BbpSoftReset(pAd);
#endif /* RTMP_BBP */
#if defined(RTMP_MAC) || defined(RLT_MAC)
			RTMP_IO_WRITE32(pAd, TX_PIN_CFG, (pATEInfo->Default_TX_PIN_CFG));
#else
	//TODO: MT_MAC check
#endif
		}
	}		

	/*
		We should free some resource which was allocated
		when ATE_TXFRAME , ATE_STOP, and ATE_TXCONT.
	*/
	else if ((atemode & ATE_TXFRAME) || (atemode == ATE_STOP))
	{
#ifdef RTMP_MAC_PCI
		RTMP_TX_RING *pTxRing = &pAd->TxRing[QID_AC_BE];
#endif /* RTMP_MAC_PCI */
		if (atemode == ATE_TXCONT)
		{
			if (pChipStruct->bBBPStoreTXCONT == TRUE)
			{
				UINT32			bbp_index=0;
				UCHAR			RestoreRfICType=pAd->RfIcType;

#ifdef RTMP_BBP
				BbpHardReset(pAd);

				/* Restore All BBP Value */
				for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
					ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);
#endif /* RTMP_BBP */
#ifdef RELEASE_EXCLUDE
				/*
					The RfIcType will be reset to zero after the hardware reset bbp command.
					Therefore, we must restore the original RfIcType.
				*/
#endif /* RELEASE_EXCLUDE */
				pAd->RfIcType=RestoreRfICType;
			}
#ifdef RT305x
			if (IS_RT3050(pAd))
			{
				/* fix RT3050S ATE SWITCH Mode */
				/* suggestion from Arvin Wang at 01/28/2010 */
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, 0x38);
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
			}
#endif /* RT305x */

#ifdef RLT_BBP
#ifdef RT65xx
			if (IS_MT7610(pAd))
			{
				RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
				bbp_val &= ~(0x1 << 15);
				RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);
			}
			if ( IS_MT76x2(pAd) )
			{
				RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
				bbp_val &= ~(0x1 << 15);
				RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);
				RTMP_BBP_IO_WRITE32(pAd, CORE_R4, 1);
				RtmpusecDelay(10);
				RTMP_BBP_IO_WRITE32(pAd, CORE_R4, 0);
				RTMP_IO_WRITE32(pAd, DACCLK_EN_DLY_CFG, 0x0);
			}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
			if (pAd->chipCap.hif_type == HIF_RTMP)
			{
				/* Not Cont. TX anymore, so set BBP R22 bit7=0 */
				ATE_BBP_STOP_CTS_TX_MODE(pAd, BBP_R22, &BbpData);
			}
#endif /* RTMP_BBP */

			if (pATEInfo->TxMethod == TX_METHOD_1)
			{
#ifdef RLT_BBP
#ifdef RT65xx
				if (IS_MT7610(pAd))
				{
					ate_bbp_core_soft_reset(pAd, FALSE, 0);
				}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
				if (pAd->chipCap.hif_type == HIF_RTMP)
				{
					BbpSoftReset(pAd);
				}
#endif /* RTMP_BBP */
#if defined(RTMP_MAC) || defined(RLT_MAC)
				RTMP_IO_WRITE32(pAd, TX_PIN_CFG, (pATEInfo->Default_TX_PIN_CFG));
#else
	//TODO: MT_MAC check
#endif
			}
		}

		if (pAd->chipCap.MCUType != ANDES )
		{
			/* Abort Tx, Rx DMA. */
			RtmpDmaEnable(pAd, 0);
		}

#ifdef RTMP_MAC_PCI
#ifdef MT76x0
		/* remove it if verified ok */
		if (IS_MT76x0(pAd))
		{
			/* Abort Tx, Rx DMA. */
			RtmpDmaEnable(pAd, 0);
		}
#endif /* MT76x0 */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_PCI
		if (atemode == ATE_STOP)
		{
			for (ring_index=0; ring_index<TX_RING_SIZE; ring_index++)
			{
				PNDIS_PACKET  pPacket;

#ifndef RT_BIG_ENDIAN
				pTxD = (TXD_STRUC *)pAd->TxRing[QID_AC_BE].Cell[ring_index].AllocVa;
#else
				pDestTxD = (PTXD_STRUC)pAd->TxRing[QID_AC_BE].Cell[ring_index].AllocVa;
				NdisMoveMemory(&tx_hw_info[0], (UCHAR *)pDestTxD, TXD_SIZE);
				pTxD = (TXD_STRUC *)&tx_hw_info[0];
				RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
#endif /* !RT_BIG_ENDIAN */
				pTxD->DMADONE = 0;
				pPacket = pTxRing->Cell[ring_index].pNdisPacket;

				if (pPacket)
				{
					PCI_UNMAP_SINGLE(pAd, pTxD->SDPtr0, pTxD->SDLen0, RTMP_PCI_DMA_TODEVICE);
					RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_SUCCESS);
				}

				/* prepare TX resource for ATE mode */
				pPacket = pATEInfo->pAtePacket[ring_index];
				pTxRing->Cell[ring_index].pNdisPacket = pPacket;

				pPacket = pTxRing->Cell[ring_index].pNextNdisPacket;

				if (pPacket)
				{
					PCI_UNMAP_SINGLE(pAd, pTxD->SDPtr1, pTxD->SDLen1, RTMP_PCI_DMA_TODEVICE);
					RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_SUCCESS);
				}

				/* Always assign pNextNdisPacket as NULL after clear */
				pTxRing->Cell[ring_index].pNextNdisPacket = NULL;
#ifdef RT_BIG_ENDIAN
				RTMPDescriptorEndianChange((PUCHAR)pTxD, TYPE_TXD);
				WriteBackToDescriptor((PUCHAR)pDestTxD, (PUCHAR)pTxD, FALSE, TYPE_TXD);
#endif /* RT_BIG_ENDIAN */
			}
		}
#endif /* RTMP_MAC_PCI */

		/* Start Tx, Rx DMA. */
		RtmpDmaEnable(pAd, 1);
	}

#ifdef RTMP_MAC_USB
/*	pAd->ContinBulkIn = FALSE; */
	RTUSBRejectPendingPackets(pAd);
	RTUSBCleanUpDataBulkOutQueue(pAd);

#ifdef CONFIG_STA_SUPPORT
	/*
		It will be called in MlmeSuspend().
		Cancel pending timers.
	*/
	RTMPCancelTimer(&pAd->MlmeAux.AssocTimer,      &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.ReassocTimer,    &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.DisassocTimer,   &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.AuthTimer,       &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.BeaconTimer,     &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.ScanTimer,       &Cancelled);
#endif /* CONFIG_STA_SUPPORT */

	RTUSBCleanUpMLMEBulkOutQueue(pAd);

#ifdef RELEASE_EXCLUDE
	/* Sometimes kernel will hang on, so we avoid calling MlmeSuspend(). */
#endif /* RELEASE_EXCLUDE */

	/* Disable Rx */
#if defined(RTMP_MAC) || defined(RLT_MAC)
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC stop Rx
#endif

	if (pAd->chipCap.MCUType != ANDES )
	{
		/* Abort Tx, Rx DMA. */
		RtmpDmaEnable(pAd, 0);
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Disable Tx */
	ATE_MAC_TX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC stop Tx
#endif

	/*
		Make sure there are no pending bulk in/out IRPs before we go on.
		pAd->BulkFlags != 0 : wait bulk out finish
	*/	
	while ((pAd->PendingRx > 0))	
	{
		ATE_RTUSBCancelPendingBulkInIRP(pAd);

		/* delay 0.5 seconds */
		RtmpOsMsDelay(500);
		pAd->PendingRx = 0;
	}

	while (((pAd->BulkOutPending[0] == TRUE) ||
			(pAd->BulkOutPending[1] == TRUE) || 
			(pAd->BulkOutPending[2] == TRUE) ||
			(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))
			/* pAd->BulkFlags != 0 : wait bulk out finish */	
	{
		do 
		{
			/* 
				pAd->BulkOutPending[y] will be set to FALSE
				in RTUSBCancelPendingBulkOutIRP(pAd)
			*/
			RTUSBCancelPendingBulkOutIRP(pAd);
		} while (FALSE);			

#ifdef RELEASE_EXCLUDE
		/* 
			we have enough time delay in RTUSBCancelPendingBulkOutIRP(pAd)
			so this is not necessary 
		*/
#endif /* RELEASE_EXCLUDE */
	}

	ASSERT(pAd->PendingRx == 0);
#endif /* RTMP_MAC_USB */

	/* reset Rx statistics. */
	pATEInfo->LastSNR0 = 0;
	pATEInfo->LastSNR1 = 0;
#ifdef DOT11N_SS3_SUPPORT
	pATEInfo->LastSNR2 = 0;
#endif /* DOT11N_SS3_SUPPORT */
	pATEInfo->LastRssi0 = 0;
	pATEInfo->LastRssi1 = 0;
	pATEInfo->LastRssi2 = 0;
	pATEInfo->AvgRssi0 = 0;
	pATEInfo->AvgRssi1 = 0;
	pATEInfo->AvgRssi2 = 0;
	pATEInfo->AvgRssi0X8 = 0;
	pATEInfo->AvgRssi1X8 = 0;
	pATEInfo->AvgRssi2X8 = 0;
	pATEInfo->NumOfAvgRssiSample = 0;

#ifdef CONFIG_QA
	/* Tx frame */
	pATEInfo->bQATxStart = FALSE;
	pATEInfo->bQARxStart = FALSE;
	pATEInfo->seq = 0; 

	/* counters */
	pATEInfo->U2M = 0;
	pATEInfo->OtherData = 0;
	pATEInfo->Beacon = 0;
	pATEInfo->OtherCount = 0;
	pATEInfo->TxAc0 = 0;
	pATEInfo->TxAc1 = 0;
	pATEInfo->TxAc2 = 0;
	pATEInfo->TxAc3 = 0;
	pATEInfo->TxHCCA = 0;
	pATEInfo->TxMgmt = 0;
	pATEInfo->RSSI0 = 0;
	pATEInfo->RSSI1 = 0;
	pATEInfo->RSSI2 = 0;
	pATEInfo->SNR0 = 0;
	pATEInfo->SNR1 = 0;
#ifdef DOT11N_SS3_SUPPORT
	pATEInfo->SNR2 = 0;
#endif /* DOT11N_SS3_SUPPORT */
	/* control */
	pATEInfo->TxDoneCount = 0;
	/* TxStatus : 0 --> task is idle, 1 --> task is running */
	pATEInfo->TxStatus = 0;
#endif /* CONFIG_QA */

	if ((!IS_RT3883(pAd)) && (!IS_RT3593(pAd)) && (!IS_RT8592(pAd))
		&& (!IS_RT6352(pAd)) && (!IS_MT76x0(pAd)))
	{
		/* Soft reset BBP. */
		BbpSoftReset(pAd);
	}
#ifdef CONFIG_AP_SUPPORT 
#ifdef RTMP_MAC_PCI
	RTMP_OS_NETDEV_STOP_QUEUE(pAd->net_dev);
#endif /* RTMP_MAC_PCI */

#ifdef RELEASE_EXCLUDE
   	/* pATEInfo->Mode must HAVE been ATE_START prior to call ATEAPStop(pAd) */
	/* We must disable DFS and Radar Detection, or 8051 will modify BBP registers. */
#endif /* RELEASE_EXCLUDE */
	if (atemode == ATE_STOP)
	ATEAPStop(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT 
#ifdef RELEASE_EXCLUDE
	/* LinkDown() has "AsicDisableSync();" and "RTMP_BBP_IO_R/W8_BY_REG_ID();" inside. */
#endif /* RELEASE_EXCLUDE */
	AsicDisableSync(pAd);
#ifdef RTMP_MAC_PCI
	RTMP_OS_NETDEV_STOP_QUEUE(pAd->net_dev);
#endif /* RTMP_MAC_PCI */
#ifdef RELEASE_EXCLUDE
	/* 
		If we skip "LinkDown()", we should disable protection
		to prevent from sending out RTS or CTS-to-self.
	*/
#endif /* RELEASE_EXCLUDE */
	ATEDisableAsicProtect(pAd);
	RTMPStationStop(pAd);
#endif /* CONFIG_STA_SUPPORT */

	if ((atemode == ATE_STOP) && (pATEInfo->PeriodicTimer.State == FALSE))
	{
		/* Do it for the first time entering ATE mode */
		pATEInfo->PeriodicTimer.State = TRUE;
	}
	
	if (pATEInfo->PeriodicTimer.State == TRUE)
	{
		/* 
			For rx statistics, we cancel pAd->Mlme.PeriodicTimer
			and set pAd->ate.PeriodicTimer.
		*/
		RTMPCancelTimer(&pAd->Mlme.PeriodicTimer, &Cancelled);
		/* Init ATE periodic timer */
		RTMPInitTimer(pAd, &pAd->ate.PeriodicTimer, GET_TIMER_FUNCTION(ATEPeriodicExec), pAd, TRUE);
		/* Set ATE periodic timer */
		RTMPSetTimer(&pAd->ate.PeriodicTimer, ATE_TASK_EXEC_INTV);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("We are still in ATE mode, "));
		DBGPRINT(RT_DEBUG_TRACE, ("so we keep ATE periodic timer running.\n"));
	}

#ifdef LED_CONTROL_SUPPORT
#ifdef RT35xx
	if (IS_RT3572(pAd))
	RTMPExitLEDMode(pAd);	
#endif /* RT35xx */
#endif /* LED_CONTROL_SUPPORT */

#ifdef RTMP_MAC_PCI
#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Disable Tx */
	ATE_MAC_TX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);

	/* Disable Rx */
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

#ifdef RELEASE_EXCLUDE
#ifdef HUAWEI_ATE
#ifdef RT305x
	/*---------------jimmy add----------------------*/
	if (IS_RT3050(pAd))
	{
		USHORT value = 0;
		UCHAR RFValue = 0;
	
		/* Read frequency offset setting from E2P for ATE */
		RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);
	
		if ((value & 0x00FF) != 0x00FF)
			pATEInfo->RFFreqOffset = (UINT32) (value & 0x00FF);
		else
			pATEInfo->RFFreqOffset = 0;
	
		ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R23, (PUCHAR)&RFValue);
		RFValue = ((RFValue & 0x80) | pATEInfo->RFFreqOffset);
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R23, (UCHAR)RFValue);
	
		DBGPRINT(RT_DEBUG_TRACE, ("EEPROM: RF FreqOffset=%d \n", pATEInfo->RFFreqOffset));
	}
	/*-----------------------------------------------*/
#endif /* RT305x */
#endif /* HUAWEI_ATE */
#endif /* RELEASE_EXCLUDE */

#ifdef RT3390
#ifdef RTMP_RF_RW_SUPPORT
#ifdef RELEASE_EXCLUDE
/*
	For 3390 chips, add the following codes to adjust the RX maximum power
*/
#endif /* RELEASE_EXCLUDE */
	if (IS_RT3390(pAd))
	{
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R10,0x71);
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd,RF_R19,0x84);
	}
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* RT3390 */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
	/* Default value in BBP R22 is 0x0. */
   	ATE_BBP_RESET_TX_MODE(pAd, BBP_R22, &BbpData);
	}
#endif /* RTMP_BBP */
	/* Clear bit4 to stop continuous Tx production test. */
	ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);

	/* Clear ATE Bulk in/out counter and continue setup */
	InterlockedExchange(&pAd->BulkOutRemained, 0);

	/* NdisAcquireSpinLock()/NdisReleaseSpinLock() need only one argument in RT28xx */
	NdisAcquireSpinLock(&pAd->GenericLock);	
	pAd->ContinBulkOut = FALSE;		
	pAd->ContinBulkIn = FALSE;
	NdisReleaseSpinLock(&pAd->GenericLock);

	RTUSB_CLEAR_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);
#endif /* RTMP_MAC_USB */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS ATESTOP(
	IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32			MacData=0, ring_index=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#ifdef RTMP_MAC_PCI
#ifdef RT_BIG_ENDIAN
	RXD_STRUC		*pDestRxD;
	UCHAR rx_hw_info[RXD_SIZE];
#endif /* RT_BIG_ENDIAN */
	RXD_STRUC		*pRxD = NULL;
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_BBP
	UCHAR			BbpData = 0;
#endif /* RTMP_BBP */
	ATE_CHIP_STRUCT *pChipStruct = pATEInfo->pChipStruct;
	BOOLEAN Cancelled;
	
	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	if (pChipStruct->bBBPLoadATESTOP == TRUE)
	{
		UINT32			bbp_index=0;
		UCHAR			RestoreRfICType=pAd->RfIcType;

#ifdef RTMP_BBP
		BbpHardReset(pAd);

		/* Supposed that we have had a record in restore_BBP[] */
		/* restore all BBP value */
		for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);
		}
#endif /* RTMP_BBP */
#ifdef RELEASE_EXCLUDE
		/*
			The RfIcType will be reset to zero after the hardware reset bbp command.
			Therefore, we must restore the original RfIcType.
		*/
#endif /* RELEASE_EXCLUDE */
		ASSERT(RestoreRfICType != 0);
		pAd->RfIcType=RestoreRfICType;
	}

#ifdef RELEASE_EXCLUDE
	/*
		Abort Tx, RX DMA.
		Q   : How to do the following I/O if Tx, Rx DMA is aborted ?
		Ans : Bulk endpoints are aborted, while the control endpoint is not.
	*/
#endif /* RELEASE_EXCLUDE */
#ifdef RLT_BBP
#ifdef RT65xx
	if ((IS_RT8592(pAd)) && (IS_MT7610(pAd)))
	{
		/* do nothing */;
	}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
	/* Default value in BBP R22 is 0x0. */
	ATE_BBP_RESET_TX_MODE(pAd, BBP_R22, &BbpData);
	}
#endif /* RTMP_BBP */
#ifdef MT7601
	if (IS_MT7601(pAd)) {
		MT7601DisableTxRx(pAd, GUIRADIO_OFF);
	}
	else
#endif /* MT7601 */
#ifdef MT76x2
	if (IS_MT76x2(pAd))
		DISABLE_TX_RX(pAd, GUIRADIO_OFF);
	else
#endif /* MT76x2 */
	{
#if defined(RTMP_MAC) || defined(RLT_MAC)
		/* Clear bit4 to stop continuous Tx production test. */
		ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
	
		/* Disable Rx */
		ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
		//TODO: MT_MAC check this
#endif

		if (pAd->chipCap.MCUType != ANDES )
		{
			/* Abort Tx, Rx DMA. */
			RtmpDmaEnable(pAd, 0);
		}

#ifdef RTMP_MAC_PCI
#ifdef MT76x0
		/* remove it if verified ok */
		if (IS_MT76x0(pAd))
		{
			/* Abort Tx, Rx DMA. */
			RtmpDmaEnable(pAd, 0);
		}
#endif /* MT76x0 */
#endif /* RTMP_MAC_PCI */

		/* Disable Tx */
#if defined(RTMP_MAC) || defined(RLT_MAC)
		ATE_MAC_TX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif
	}

#ifdef RTMP_MAC_PCI
	if (IS_PCI_INF(pAd))
	{
		if (pAd->chipCap.MCUType == M8051)
		{
			pATEInfo->bFWLoading = TRUE;
			Status = NICLoadFirmware(pAd);
			if (Status != NDIS_STATUS_SUCCESS)
			{
				DBGPRINT_ERR(("NICLoadFirmware failed, Status[=0x%08x]\n", Status));
				return Status;
			}
		}
	}
	pATEInfo->Mode = ATE_STOP;
#ifdef RELEASE_EXCLUDE
	/*
		Even the firmware has been loaded, 
		we still could use ATE_BBP_IO_READ8_BY_REG_ID(). 
		But this is not suggested.
	*/
#endif /* RELEASE_EXCLUDE */
	if ((!IS_RT8592(pAd)) && (!IS_MT76x0(pAd)) && (!IS_RT6352(pAd)) && (!IS_MT76x2(pAd)) && (!IS_MT7601(pAd)))
		BbpSoftReset(pAd);

	RTMP_ASIC_INTERRUPT_DISABLE(pAd);
#endif /* RTMP_MAC_PCI */

	if (pATEInfo->PeriodicTimer.State == FALSE)
	{
		/* 
			For rx statistics, we cancel pAd->Mlme.PeriodicTimer
			and set pATEInfo->PeriodicTimer in stead of. 
			Now we recover it before we leave ATE mode.
		*/
		RTMPCancelTimer(&pATEInfo->PeriodicTimer, &Cancelled);
		/* Init MLME periodic timer */
		RTMPInitTimer(pAd, &pAd->Mlme.PeriodicTimer, GET_TIMER_FUNCTION(MlmePeriodicExecTimer), pAd, TRUE);
		/* Set MLME periodic timer */
		RTMPSetTimer(&pAd->Mlme.PeriodicTimer, MLME_TASK_EXEC_INTV);
	}
	else
	{
		/* ATE periodic timer has been cancelled. */
		Status = NDIS_STATUS_FAILURE;
		DBGPRINT_ERR(("Initialization of MLME periodic timer failed, Status[=0x%08x]\n", Status));

		return Status;
	}	

#ifdef RTMP_MAC_PCI
	NICInitializeAdapter(pAd, TRUE);
	
	for (ring_index = 0; ring_index < RX_RING_SIZE; ring_index++)
	{
#ifdef RT_BIG_ENDIAN
		pDestRxD = (RXD_STRUC *) pAd->RxRing[0].Cell[ring_index].AllocVa;
		NdisMoveMemory(&rx_hw_info[0], pDestRxD, RXD_SIZE);
		pRxD = (RXD_STRUC *)&rx_hw_info[0];
		RTMPDescriptorEndianChange((PUCHAR)pRxD, TYPE_RXD);
#else
		/* Point to Rx indexed rx ring descriptor */
		pRxD = (RXD_STRUC *) pAd->RxRing[0].Cell[ring_index].AllocVa;
#endif /* RT_BIG_ENDIAN */

#ifdef RELEASE_EXCLUDE
	/*
		Reinitialize Rx Ring before Rx DMA is enabled.
		>>>RxCoherent<<< was gone !
	*/
#endif /* RELEASE_EXCLUDE */
		pRxD->DDONE = 0;

#ifdef RT_BIG_ENDIAN
		RTMPDescriptorEndianChange((PUCHAR)pRxD, TYPE_RXD);
		WriteBackToDescriptor((PUCHAR)pDestRxD, (PUCHAR)pRxD, FALSE, TYPE_RXD);
#endif /* RT_BIG_ENDIAN */
	}

	/* We should read EEPROM for all cases. */  
	NICReadEEPROMParameters(pAd, NULL);
	NICInitAsicFromEEPROM(pAd); 

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
#if defined(RT6352_EP_SUPPORT) || defined(RT6352_EL_SUPPORT)
		{
			ULONG SysRegValue;

			RTMP_SYS_IO_READ32(0xb0000060, &SysRegValue);
			if ((SysRegValue & 0x100000) == 0x0)
			{
				SysRegValue |= 0x100000;
				RTMP_SYS_IO_WRITE32(0xb0000060, SysRegValue);
				DBGPRINT(RT_DEBUG_ERROR,("Change as GPIO Mode(0x%x)\n", SysRegValue));
			}
		}

		RT6352_Restore_RF_BBP(pAd);
#endif /* defined(RT6352_EP_SUPPORT) || defined(RT6352_EL_SUPPORT) */

		/* Do R-Calibration */
		R_Calibration(pAd);

#ifdef RTMP_TEMPERATURE_CALIBRATION
		/* Temperature Init */
		RT6352_Temperature_Init(pAd);		
		RT6352_TemperatureCalibration(pAd);
#endif /* RTMP_TEMPERATURE_CALIBRATION */

		AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, TRUE);
		AsicLockChannel(pAd, pAd->CommonCfg.Channel);

		/* TX Self TX DC Calibration */
		RF_SELF_TXDC_CAL(pAd);

		/* Rx DCOC Calibration */
		RxDCOC_Calibration(pAd);

		/* BandWidth Filter Calibration */
		BW_Filter_Calibration(pAd,TRUE);
		BW_Filter_Calibration(pAd,FALSE);

		/* Do LOFT and IQ Calibration */
		LOFT_IQ_Calibration(pAd);

		/* DPD_Calibration */
#ifdef RT6352_EP_SUPPORT
		if (pAd->bExtPA == FALSE)
#endif /* RT6352_EP_SUPPORT */
		{
			DoDPDCalibration(pAd);
			pAd->DoDPDCurrTemperature = 0x7FFFFFFF;
		}

		/* Rx DCOC Calibration */
		RxDCOC_Calibration(pAd);

		/* Do RXIQ Calibration */
		RXIQ_Calibration(pAd);

#if defined(RT6352_EP_SUPPORT) || defined(RT6352_EL_SUPPORT)
		{
			ULONG SysRegValue;

			/* Check the gpio setting first */
			RTMP_SYS_IO_READ32(0xb0000060, &SysRegValue);
			if ((SysRegValue & 0x100000) == 0x100000)
			{
				SysRegValue &= (~0x100000);
				RTMP_SYS_IO_WRITE32(0xb0000060, SysRegValue);
				DBGPRINT(RT_DEBUG_ERROR,("Change as Normal Mode(0x%x)\n", SysRegValue));
			}
	}

		RT6352_Init_ExtPA_ExtLNA(pAd, TRUE);
#endif /* defined(RT6352_EP_SUPPORT) || defined(RT6352_EL_SUPPORT) */
	}
	else
#endif /* RT6352 */
	{
		AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
		/* empty function */
		AsicLockChannel(pAd, pAd->CommonCfg.Channel);
	}
#ifdef RTMP_INTERNAL_TX_ALC
#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		if (pATEInfo->bAutoTxAlc == TRUE)
		{
			OS_SEM_LOCK(&(pATEInfo->TssiSemLock));
			RT635xTssiDcCalibration(pAd);
			OS_SEM_UNLOCK(&(pATEInfo->TssiSemLock));	

			RT635xInitMcsPowerTable(pAd);
			RT635xInitRfPaModeTable(pAd);
		}
	}
#endif /* RT6352 */
#endif /* RTMP_INTERNAL_TX_ALC */

	/* clear garbage interrupts and enable interrupt */
	RTMP_IRQ_ENABLE(pAd);
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	/* 
		Make sure there are no pending bulk in/out IRPs before we go on.
		pAd->BulkFlags != 0 : wait bulk out finish
	*/
/*	pAd->ContinBulkIn = FALSE; */

	while (pAd->PendingRx > 0)
	{
		ATE_RTUSBCancelPendingBulkInIRP(pAd);
		RtmpOsMsDelay(500);
	}

	while (((pAd->BulkOutPending[0] == TRUE) ||
			(pAd->BulkOutPending[1] == TRUE) || 
			(pAd->BulkOutPending[2] == TRUE) ||
			(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))
			/* pAd->BulkFlags != 0 : wait bulk out finish */
	{
		do 
		{
			RTUSBCancelPendingBulkOutIRP(pAd);
		} while (FALSE);			

		RtmpOsMsDelay(500);
	}

	ASSERT(pAd->PendingRx == 0);
/*=========================================================================*/
/*      Reset Rx RING                                                      */ 
/*=========================================================================*/
	pAd->PendingRx = 0;
	/* Next Rx Read index */
	pAd->NextRxBulkInReadIndex = 0;	
	/* Rx Bulk pointer */
	pAd->NextRxBulkInIndex = RX_RING_SIZE - 1;	
	pAd->NextRxBulkInPosition = 0;
	for (ring_index = 0; ring_index < (RX_RING_SIZE); ring_index++)
	{
		PRX_CONTEXT  pRxContext = &(pAd->RxContext[ring_index]);
		NdisZeroMemory(pRxContext->TransferBuffer, MAX_RXBULK_SIZE);

		pRxContext->pAd	= pAd;
		pRxContext->pIrp = NULL;
		pRxContext->BulkInOffset = 0;
		pRxContext->bRxHandling = FALSE;
		pRxContext->InUse		= FALSE;
		pRxContext->IRPPending	= FALSE;
		pRxContext->Readable	= FALSE;
	}

/*=========================================================================*/
/*      Reset Tx RING                                                      */ 
/*=========================================================================*/
	do 
	{
		RTUSBCancelPendingBulkOutIRP(pAd);
	} while (FALSE);

	/* Enable auto responder. */
	RTMP_IO_READ32(pAd, AUTO_RSP_CFG, &MacData);
	MacData = MacData | (0x01);
	RTMP_IO_WRITE32(pAd, AUTO_RSP_CFG, MacData);

	AsicEnableBssSync(pAd, pAd->CommonCfg.BeaconPeriod);
#ifdef RELEASE_EXCLUDE
	/* Soft reset BBP.*/
	/* In 2870 chipset, ATE_BBP_IO_READ8_BY_REG_ID() == RTMP_BBP_IO_READ8_BY_REG_ID() */
	/* Both rt2870ap and rt2870sta use BbpSoftReset(pAd) to do BBP soft reset */
#endif /* RELEASE_EXCLUDE */
	BbpSoftReset(pAd);
	{
#ifdef CONFIG_STA_SUPPORT
		/* Set all state machines back IDLE */
		pAd->Mlme.CntlMachine.CurrState    = CNTL_IDLE;
		pAd->Mlme.AssocMachine.CurrState   = ASSOC_IDLE;
		pAd->Mlme.AuthMachine.CurrState    = AUTH_REQ_IDLE;
		pAd->Mlme.AuthRspMachine.CurrState = AUTH_RSP_IDLE;
		pAd->Mlme.SyncMachine.CurrState    = SYNC_IDLE;
		pAd->Mlme.ActMachine.CurrState    = ACT_IDLE;
#endif /* CONFIG_STA_SUPPORT */

		/*
			===> refer to MlmeRestartStateMachine().
			When we entered ATE_START mode, PeriodicTimer was not cancelled.
			So we don't have to set it here.
		*/
		
		ASSERT(pAd->CommonCfg.Channel != 0);			

		AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);

		/* empty function */
		AsicLockChannel(pAd, pAd->CommonCfg.Channel);

#ifdef CONFIG_AP_SUPPORT 
	    APStartUp(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT 
	    RTMPStationStart(pAd);
#endif /* CONFIG_STA_SUPPORT */
	}	

	/* Clear ATE Bulk in/out counter and continue setup. */
	InterlockedExchange(&pAd->BulkOutRemained, 0);				
	NdisAcquireSpinLock(&pAd->GenericLock);
	pAd->ContinBulkOut = FALSE;		
	pAd->ContinBulkIn = FALSE;
	NdisReleaseSpinLock(&pAd->GenericLock);		

	/* Wait 50ms to prevent next URB to bulkout during HW reset. */
	/* todo : remove this if not necessary */
	RtmpOsMsDelay(50);

	pATEInfo->Mode = ATE_STOP;
#endif /* RTMP_MAC_USB */

	ATE_ASIC_CALIBRATION(pAd, ATE_STOP);

	/* restore RX_FILTR_CFG due to that QA maybe set it to 0x3 */
	AsicSetRxFilter(pAd);

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Enable Tx */
	ATE_MAC_TX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

	/* Enable Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Enable Rx */
	ATE_MAC_RX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

#ifdef RTMP_MAC_PCI
#ifdef CONFIG_AP_SUPPORT 
	APStartUp(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT 
	RTMPStationStart(pAd);
#endif /* CONFIG_STA_SUPPORT */
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	/* Wait 10ms for all of the bulk-in URBs to complete. */
#ifdef RELEASE_EXCLUDE
	/* todo : remove this if not necessary */
#endif /* RELEASE_EXCLUDE */
	RtmpOsMsDelay(10);

	/* Everything is ready to start normal Tx/Rx. */
	RTUSBBulkReceive(pAd);
#endif /* RTMP_MAC_USB */
#ifdef RTMP_MAC_PCI
	RTMP_OS_NETDEV_START_QUEUE(pAd->net_dev);
#endif /* RTMP_MAC_PCI */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXCARR(
	IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#ifdef RTMP_BBP
	PATE_CHIP_STRUCT pChipStruct = pATEInfo->pChipStruct;
	UCHAR BbpData = 0;
#endif /* RTMP_BBP */
#ifdef RLT_BBP
	UINT32 bbp_val;
#endif /* RLT_BBP */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	pATEInfo->Mode = ATE_TXCARR;
#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		if (pChipStruct->bBBPStoreTXCARR == TRUE)
		{
			UINT32 bbp_index=0;

			/* Zero All BBP Value */	
			for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
				restore_BBP[bbp_index]=0;

			/* Record All BBP Value */
			for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
				ATE_BBP_IO_READ8_BY_REG_ID(pAd,bbp_index,&restore_BBP[bbp_index]);
		}
	}
#endif /* RTMP_BBP */
#ifdef RTMP_MAC_USB
	/* Disable Rx */
#ifdef RELEASE_EXCLUDE
	/* May be we need not to do this, because these have been done in ATE_START mode ??? */
#endif /* RELEASE_EXCLUDE */
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#endif /* RTMP_MAC_USB */

	/* QA has done the following steps if it is used. */
	if (pATEInfo->bQATxStart == FALSE) 
	{
#ifdef RELEASE_EXCLUDE
		/* RT3883, RT3352 and RT5350 does not need BbpSoftReset() */
#endif /* RELEASE_EXCLUDE */
		if ((!IS_RT3883(pAd)) && (!IS_RT3352(pAd))
			&& (!IS_RT5350(pAd)) && (!IS_RT3593(pAd)) && (!IS_MT7610(pAd)))
			BbpSoftReset(pAd);/* Soft reset BBP. */

		if (pATEInfo->TxMethod == TX_METHOD_1)
		{
#ifdef RT5592
			/* move from ChannelSwitch to here */
			if (IS_RT5592(pAd))
				ATEAsicSetTxRxPath(pAd);
#endif /* RT5592 */
			if (!IS_RT5592(pAd))			
			{
#if defined(RTMP_MAC) || defined(RLT_MAC)
				/* store the original value of TX_PIN_CFG */
				RTMP_IO_READ32(pAd, TX_PIN_CFG, &(pATEInfo->Default_TX_PIN_CFG));
#else
	//TODO: MT_MAC check
#endif

				/* give TX_PIN_CFG(0x1328) a proper value. */
			if (pATEInfo->Channel <= 14)
			{
				/* G band */
				MacData = TXCONT_TX_PIN_CFG_G;
#if defined(RTMP_MAC) || defined(RLT_MAC)
					RTMP_IO_WRITE32(pAd, TX_PIN_CFG, MacData);
#else
					//TODO: MT_MAC check
#endif
			}
			else
			{
				/* A band */
				MacData = TXCONT_TX_PIN_CFG_A;
#if defined(RTMP_MAC) || defined(RLT_MAC)
					RTMP_IO_WRITE32(pAd, TX_PIN_CFG, MacData);
#else
					//TODO: MT_MAC check
#endif
				}
			}

#ifdef RLT_BBP
#ifdef RT65xx
			if (IS_MT7610(pAd) || IS_MT76x2(pAd))
			{
				RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
				bbp_val |= (0x1 << 14);
				bbp_val |= (0x1 << 8);
				RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);
			}
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
			if (pAd->chipCap.hif_type == HIF_RTMP) {
				/* Carrier Test set BBP R22 bit6=1, bit[5~0]=0x01 */
				ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
				BbpData &= 0xFFFFFF80; /* bit6, bit[5~0] */
				BbpData |= 0x00000041; /* set bit6=1, bit[5~0]=0x01 */
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
			}
#endif /* RTMP_BBP */
		}
		else
		{
#ifdef RTMP_BBP
			/* Carrier Test set BBP R22 bit7=1, bit6=1, bit[5~0]=0x01 */
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
			BbpData &= 0xFFFFFF00; /* clear bit7, bit6, bit[5~0] */
			BbpData |= 0x000000C1; /* set bit7=1, bit6=1, bit[5~0]=0x01 */
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
#endif /* RTMP_BBP */
			// TODO: how to handle this for RLT_BBP?

#if defined(RTMP_MAC) || defined(RLT_MAC)			
			/* Set MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 1. */
			ATE_MAC_TX_CTS_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
			//TODO: MT_MAC check
#endif
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXCONT(
	IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	UCHAR			BbpData = 0;
	PATE_CHIP_STRUCT pChipStruct = pATEInfo->pChipStruct;
#ifdef RLT_BBP
	UINT32 bbp_val;
#endif /* RLT_BBP */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	pATEInfo->Mode = ATE_TXCONT;
	
	if (pATEInfo->bQATxStart == TRUE)
	{
		/*
			set MAC_SYS_CTRL(0x1004) bit4(Continuous Tx Production Test)
			and bit2(MAC TX enable) back to zero.
		*/
#if defined(RTMP_MAC) || defined(RLT_MAC)
		ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
		ATE_MAC_TX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
		//TODO: MT_MAC check
#endif

#ifdef RTMP_BBP
		/* set BBP R22 bit7=0 */
		ATE_BBP_STOP_CTS_TX_MODE(pAd, BBP_R22, &BbpData);
#endif /* RTMP_BBP */
	}
	else
	{
		if (pATEInfo->TxMethod == TX_METHOD_1)
		{
#ifdef RT5592
			/* move from ChannelSwitch to here */
			if (IS_RT5592(pAd))
				ATEAsicSetTxRxPath(pAd);
#endif /* RT5592 */
			if (!IS_RT5592(pAd))
			{
#if defined(RTMP_MAC) || defined(RLT_MAC)
				/* store the original value of TX_PIN_CFG */
				RTMP_IO_READ32(pAd, TX_PIN_CFG, &(pATEInfo->Default_TX_PIN_CFG));
#else
				//TODO: MT_MAC check
#endif
			}
		}
	}

	if (pChipStruct->bBBPStoreTXCONT == TRUE)
	{
#ifdef RTMP_BBP
		UINT32 bbp_index=0;

		/* Zero All BBP Value */
		for(bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
			restore_BBP[bbp_index]=0;

		/* Record All BBP Value */
		for(bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
			ATE_BBP_IO_READ8_BY_REG_ID(pAd,bbp_index,&restore_BBP[bbp_index]);
#endif /* RTMP_BBP */
	}
#ifdef RELEASE_EXCLUDE 
	/* 
		For TxCont mode - 
		Step 1: Send 50 packets first then wait for a moment.
		Step 2: Send more 50 packet then start continue mode.
	*/
#endif /* RELEASE_EXCLUDE */

	/* Step 1: send 50 packets first. */
	pATEInfo->TxCount = 50;

#ifdef RELEASE_EXCLUDE
	/* RT3883, RT3352, RT5350 and RT3593 does not need BbpSoftReset() */
#endif /* RELEASE_EXCLUDE */
	if ((!IS_RT3883(pAd)) && (!IS_RT3352(pAd)) && (!IS_RT5350(pAd)) && (!IS_RT3593(pAd)) && (!IS_RT65XX(pAd)))
		BbpSoftReset(pAd);/* Soft reset BBP. */

	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

#ifdef RTMP_MAC_PCI
#ifdef RELEASE_EXCLUDE 
	/* Fix can't smooth kick */
#endif /* RELEASE_EXCLUDE */
	{
		RTMP_IO_READ32(pAd, pTxRing->hw_didx_addr, &pTxRing->TxDmaIdx);
		pTxRing->TxSwFreeIdx = pTxRing->TxDmaIdx;
		pTxRing->TxCpuIdx = pTxRing->TxDmaIdx;
		RTMP_IO_WRITE32(pAd, pTxRing->hw_cidx_addr, pTxRing->TxCpuIdx);
	}
#endif /* RTMP_MAC_PCI */

	/* Do it after Tx/Rx DMA is aborted. */
	pATEInfo->TxDoneCount = 0;
	
	/* Only needed if we have to send some normal frames. */
	if (pATEInfo->bQAEnabled == FALSE)
		SetJapanFilter(pAd);

#ifdef RTMP_MAC_PCI
	TxRingGetReady(pAd);
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	/* Setup frame format. */
	MT_ATESetupFrame(pAd, 0);
#endif /* RTMP_MAC_USB */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Enable Tx */
	ATE_MAC_TX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);

	/* Disable Rx */
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

	/* Start Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);

#ifdef RTMP_MAC_USB
	InterlockedExchange(&pAd->BulkOutRemained, pATEInfo->TxCount);
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_QA
	if (pATEInfo->bQATxStart == TRUE)
	{
		pATEInfo->TxStatus = 1;
	}
#endif /* CONFIG_QA */

#ifdef RTMP_MAC_PCI
	/* kick Tx Ring */
	RTMP_IO_WRITE32(pAd, pAd->TxRing[QID_AC_BE].hw_cidx_addr, pAd->TxRing[QID_AC_BE].TxCpuIdx);

	RtmpOsMsDelay(5);
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	NdisAcquireSpinLock(&pAd->GenericLock);
	pAd->ContinBulkOut = FALSE;		
	NdisReleaseSpinLock(&pAd->GenericLock);

	RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

	/* Kick bulk out */ 
	RTUSBKickBulkOut(pAd);

	/* To make sure all the 50 frames have been bulk out before executing step 2 */
	while (atomic_read(&pAd->BulkOutRemained) > 0)
	{
		RtmpOsMsDelay(5);
	}
#endif /* RTMP_MAC_USB */

	if (pATEInfo->TxMethod == TX_METHOD_1)
	{
		if (!IS_RT5592(pAd))
		{
			/* give TX_PIN_CFG(0x1328) a proper value. */
		if (pATEInfo->Channel <= 14)
		{
			/* G band */
			MacData = TXCONT_TX_PIN_CFG_G;
#if defined(RTMP_MAC) || defined(RLT_MAC)
				RTMP_IO_WRITE32(pAd, TX_PIN_CFG, MacData);
#else
				//TODO: MT_MAC check
#endif
		}
		else
		{
			/* A band */
			MacData = TXCONT_TX_PIN_CFG_A;
#if defined(RTMP_MAC) || defined(RLT_MAC)
				RTMP_IO_WRITE32(pAd, TX_PIN_CFG, MacData);
#else
				//TODO: MT_MAC check
#endif
			}
		}
#ifdef RLT_BBP
#ifdef RT65xx
		if (IS_MT7610(pAd) || IS_MT76x2(pAd))
		{
			RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
			bbp_val |= (0x1 << 15);
			RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);
		}
		if ( IS_MT76x2(pAd) )
			RTMP_IO_WRITE32(pAd, DACCLK_EN_DLY_CFG, 0x80008000);
#endif /* RT65xx */
#endif /* RLT_BBP */
#ifdef RTMP_BBP
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			/* Cont. TX set BBP R22 bit7=1 */
			ATE_BBP_START_CTS_TX_MODE(pAd, BBP_R22, &BbpData);
		}
#endif /* RTMP_BBP */

#ifdef RT5592EP_SUPPORT
		/* enable continuous tx production test */
		if (pAd->chipCap.Priv == RT5592_TYPE_EP)
			ATE_MAC_TX_CTS_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#endif /* RT5592EP_SUPPORT */
	}
	else
	{
		/* Step 2: send more 50 packets then start Continuous Tx Mode. */
		/* Abort Tx, RX DMA. */
		RtmpDmaEnable(pAd, 0);
#ifdef RTMP_BBP
		/* Cont. TX set BBP R22 bit7=1 */
		ATE_BBP_START_CTS_TX_MODE(pAd, BBP_R22, &BbpData);
#endif /* RTMP_BBP */

		pATEInfo->TxCount = 50;
#ifdef RTMP_MAC_PCI
#ifdef RELEASE_EXCLUDE 
		/* Fix can't smooth kick */
#endif /* RELEASE_EXCLUDE */
		{
			RTMP_IO_READ32(pAd, pTxRing->hw_didx_addr, &pTxRing->TxDmaIdx);
			pTxRing->TxSwFreeIdx = pTxRing->TxDmaIdx;
			pTxRing->TxCpuIdx = pTxRing->TxDmaIdx;
			RTMP_IO_WRITE32(pAd, pTxRing->hw_cidx_addr, pTxRing->TxCpuIdx);
		}
#endif /* RTMP_MAC_PCI */

		pATEInfo->TxDoneCount = 0;
		if (pATEInfo->bQAEnabled == FALSE)
			SetJapanFilter(pAd);

#ifdef RTMP_MAC_PCI
		TxRingGetReady(pAd);
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
		/* Build up Tx frame. */
		MT_ATESetupFrame(pAd, 0);
#endif /* RTMP_MAC_USB */

#if defined(RTMP_MAC) || defined(RLT_MAC)
		/* Enable Tx */
		ATE_MAC_TX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);

		/* Disable Rx */
		ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
		//TODO: MT_MAC check
#endif

		/* Start Tx, Rx DMA. */
		RtmpDmaEnable(pAd, 1);

#ifdef RTMP_MAC_USB
		InterlockedExchange(&pAd->BulkOutRemained, pATEInfo->TxCount);
#endif /* RTMP_MAC_USB */
#ifdef CONFIG_QA
		if (pATEInfo->bQATxStart == TRUE)
		{
			pATEInfo->TxStatus = 1;
		}
#endif /* CONFIG_QA */

#ifdef RTMP_MAC_PCI
		/* kick Tx Ring */
		RTMP_IO_WRITE32(pAd, pAd->TxRing[QID_AC_BE].hw_cidx_addr, pAd->TxRing[QID_AC_BE].TxCpuIdx);
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
		NdisAcquireSpinLock(&pAd->GenericLock);
		pAd->ContinBulkOut = FALSE;		
		NdisReleaseSpinLock(&pAd->GenericLock);

		RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);
		/* Kick bulk out */
		RTUSBKickBulkOut(pAd);

		/* Let pAd->BulkOutRemained be consumed to zero. */
#endif /* RTMP_MAC_USB */
		RtmpusecDelay(500);

#if defined(RTMP_MAC) || defined(RLT_MAC)
		/* enable continuous tx production test */
		ATE_MAC_TX_CTS_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
		//TODO: MT_MAC check
#endif
	}

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXCARS(
        IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	PATE_CHIP_STRUCT pChipStruct = pATEInfo->pChipStruct;
#ifdef RTMP_BBP
	UCHAR			BbpData = 0;
#endif /* RTMP_BBP */
#ifdef RLT_BBP
	UINT32 bbp_val;
#endif /* RLT_BBP */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	pATEInfo->Mode = ATE_TXCARRSUPP;

	if (pChipStruct->bBBPStoreTXCARRSUPP == TRUE)
	{
#ifdef RTMP_BBP
		UINT32 bbp_index=0;

		/* Zero All BBP Value */
        for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
                restore_BBP[bbp_index]=0;

        /* Record All BBP Value */
        for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
                ATE_BBP_IO_READ8_BY_REG_ID(pAd,bbp_index,&restore_BBP[bbp_index]);
#endif /* RTMP_BBP */
	}
#ifdef RTMP_MAC_USB
	/* Disable Rx */
#ifdef RELEASE_EXCLUDE
	/* May be we need not to do this, because these have been done in ATE_START mode ??? */
#endif /* RELEASE_EXCLUDE */
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#endif /* RTMP_MAC_USB */

	/* QA has done the following steps if it is used. */
	if (pATEInfo->bQATxStart == FALSE) 
	{
#if !defined(RT3883) && !defined(RT3593) && !defined(RT65xx)
		if (!IS_RT3883(pAd) && !IS_RT3593(pAd) && !IS_MT7610(pAd))
		{
#ifdef RELEASE_EXCLUDE
			/* RT3883 ,RT3593 and RT6590 do not need BbpSoftReset() */
			/* Soft reset BBP. */
#endif /* RELEASE_EXCLUDE */
			BbpSoftReset(pAd);
		}
#endif /* !defined(RT3883) && !defined(RT3593) && !defined(RT65xx) */

		if (pATEInfo->TxMethod == TX_METHOD_1)
		{
#ifdef RT5592
			/* move from ChannelSwitch to here */
			if (IS_RT5592(pAd))
				ATEAsicSetTxRxPath(pAd);
#endif /* RT5592 */
			if (!IS_RT5592(pAd))
			{
#if defined(RTMP_MAC) || defined(RLT_MAC)
				/* store the original value of TX_PIN_CFG */
				RTMP_IO_READ32(pAd, TX_PIN_CFG, &(pATEInfo->Default_TX_PIN_CFG));
#else
				//TODO: MT_MAC check
#endif

				/* give TX_PIN_CFG(0x1328) a proper value. */
			if (pATEInfo->Channel <= 14)
			{
				/* G band */
				MacData = TXCONT_TX_PIN_CFG_G;
#if defined(RTMP_MAC) || defined(RLT_MAC)
					RTMP_IO_WRITE32(pAd, TX_PIN_CFG, MacData);
#else
					//TODO: MT_MAC check
#endif
			}
			else
			{
				/* A band */
				MacData = TXCONT_TX_PIN_CFG_A;
#if defined(RTMP_MAC) || defined(RLT_MAC)
					RTMP_IO_WRITE32(pAd, TX_PIN_CFG, MacData);
#else
					//TODO: MT_MAC check
#endif
				}
			}

#ifdef RLT_BBP
#ifdef RT65xx
			if (IS_MT7610(pAd) || IS_MT76x2(pAd))
			{
				RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
				bbp_val |= (0x1 << 15);
				RTMP_BBP_IO_WRITE32(pAd, CORE_R24, bbp_val);

				RTMP_BBP_IO_READ32(pAd, TXC_R1, &bbp_val);
				bbp_val |= (0x1 << 0);
				RTMP_BBP_IO_WRITE32(pAd, TXC_R1, bbp_val);
			}
#endif /* RT65xx */
#endif /* RLT_BBP */

#ifdef RT6352
			if (IS_RT6352(pAd))
			{
				/* Carrier Suppression set BBP R21 bit4=1 (Enable Continuous Tx Mode) */
				BbpData = 0x10;
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BbpData);
				BbpData = 0x00;
			}
#endif /* RT6352 */

#ifdef RTMP_BBP
			if (pAd->chipCap.hif_type == HIF_RTMP) {
				/* Carrier Suppression set BBP R22 bit7=1 (Enable Continuous Tx Mode) */
				ATE_BBP_START_CTS_TX_MODE(pAd, BBP_R22, &BbpData);
				/* Carrier Suppression set BBP R24 bit0=1 (TX continuously send out 5.5MHZ sin save) */
				ATE_BBP_CTS_TX_SIN_WAVE_ENABLE(pAd, BBP_R24, &BbpData);
			}
#endif /* RTMP_BBP */
		}
		else
		{
#ifdef RTMP_BBP
			/* Carrier Suppression set BBP R22 bit7=1 (Enable Continuous Tx Mode) */
			ATE_BBP_START_CTS_TX_MODE(pAd, BBP_R22, &BbpData);

			/* Carrier Suppression set BBP R24 bit0=1 (TX continuously send out 5.5MHZ sin save) */
			ATE_BBP_CTS_TX_SIN_WAVE_ENABLE(pAd, BBP_R24, &BbpData);
#endif /* RTMP_BBP */
			// TODO: how to handle this for RLT_BBP??

#if defined(RTMP_MAC) || defined(RLT_MAC)
			/* Set MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 1. */
			ATE_MAC_TX_CTS_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
			//TODO: MT_MAC check
#endif
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXFRAME(
	IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32			MacData=0;
#ifdef RTMP_MAC_USB
	ULONG			IrqFlags = 0;
#endif /* RTMP_MAC_USB */
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	UCHAR			BbpData = 0;
	RTMP_STRING IPGStr[8] = {0};
#ifdef RTMP_INTERNAL_TX_ALC
#if defined(RT3350) || defined(RT3352)
	UCHAR		RFValue, BBP49Value;
	CHAR		ChannelPower = pATEInfo->TxPower0;
	CHAR		*TssiRefPerChannel = pATEInfo->TssiRefPerChannel;
	UCHAR		CurrentChannel = pATEInfo->Channel;
#endif /* defined(RT3350) || defined(RT3352) */
#endif /* RTMP_INTERNAL_TX_ALC */
#ifdef RTMP_TEMPERATURE_COMPENSATION
#ifdef RT5592
	UCHAR			RFValue = 0;
#endif /* RT5592 */
#endif /* RTMP_TEMPERATURE_COMPENSATION */

#ifdef RTMP_INTERNAL_TX_ALC
#if defined(RT3350) || defined(RT3352)
	if (pATEInfo->bTSSICalbrEnableG == TRUE)
	{
		if ((!IS_RT3350(pAd)) && (!IS_RT3352(pAd)))                  
		{
			DBGPRINT_ERR(("Not support TSSI calibration since not 3350/3352 chip!!!\n"));
			Status = NDIS_STATUS_FAILURE;

			return Status;
		}

		/* Internal TSSI 0 */
		RFValue = (0x3 | 0x0 << 2 | 0x3 << 4);
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R27, RFValue);

		RFValue = (0x3 | 0x0 << 2);
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R28, RFValue);

		/* set BBP R49[7] = 1 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);
		BBP49Value = BbpData;
		BbpData |= 0x80;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R49, BbpData);
	}
#endif /* defined(RT3350) || defined(RT3352) */
#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef RTMP_TEMPERATURE_COMPENSATION
#ifdef RT5592
	if ((pATEInfo->bTSSICalbrEnableG == TRUE) || (pATEInfo->bTSSICalbrEnableA == TRUE))
	{
		if (!IS_RT5592(pAd))                  
		{
			DBGPRINT_ERR(("Not support TSSI calibration since not 5572/5592 chip!!!\n"));
			Status = NDIS_STATUS_FAILURE;
	
			return Status;
		}

		/* BBP R47[7]=1 : ADC 6 on */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpData);
		BbpData |= 0x80;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpData);

		/* RF R27[7:6]=0x1 : Adc_insel 01:Temp */
		/* Write RF R27[3:0]=EEPROM 0x76 bit[3:0] */
		ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R27, &RFValue);
		RFValue &= ~0xC0;
		RFValue |= 0x40;
		RFValue = ((RFValue & 0xF0) | pAd->TssiGain); /* [3:0] = (tssi_gain and tssi_atten) */
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R27, RFValue);
		/* Wait 1ms. */
		RtmpusecDelay(1000);
	}
#endif /* RT5592 */
#endif /* RTMP_TEMPERATURE_COMPENSATION */
	
	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s(Count=%u)\n", __FUNCTION__, pATEInfo->TxCount));
	pATEInfo->Mode |= ATE_TXFRAME;

	if (pATEInfo->bQATxStart == FALSE)  
	{
#ifdef RT5592
		/* move from ChannelSwitch to here */
		if (IS_RT5592(pAd))
			ATEAsicSetTxRxPath(pAd);
#endif /* RT5592 */

		/* set IPG to sync tx power with QA tools */
		/* default value of IPG is 200 */
		snprintf(IPGStr, sizeof(IPGStr), "%u", pATEInfo->IPG);
		DBGPRINT(RT_DEBUG_TRACE, ("IPGstr=%s\n", IPGStr));
		Set_ATE_IPG_Proc(pAd, IPGStr);
	}

	if ( pAd->ate.bQAEnabled == FALSE )
		ATE_ASIC_CALIBRATION(pAd, ATE_TXFRAME);

#ifdef RTMP_MAC_PCI
#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		/* Default value in BBP R22 is 0x0. */
		ATE_BBP_RESET_TX_MODE(pAd, BBP_R22, &BbpData);
	}
#endif /* RTMP_BBP */
#ifdef RELEASE_EXCLUDE
	/* RT3883, RT3352, RT5350, RT3593, RT5592, RT85592, RT6590 and RT6352 do not need BbpSoftReset() */
#endif /* RELEASE_EXCLUDE */
	if (!(IS_RT3883(pAd) || IS_RT3352(pAd) || IS_RT5350(pAd) || IS_RT3593(pAd) || IS_RT6352(pAd) || IS_MT7610(pAd) || IS_RT8592(pAd)))
	{
		/* Soft reset BBP. */
		BbpSoftReset(pAd);
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* clear bit4 to stop continuous Tx production test */
	ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

	/* Abort Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 0);

#ifdef RELEASE_EXCLUDE 
	/* Fix can't smooth kick */
#endif /* RELEASE_EXCLUDE */
	{
		RTMP_IO_READ32(pAd, pTxRing->hw_didx_addr, &pTxRing->TxDmaIdx);
		pTxRing->TxSwFreeIdx = pTxRing->TxDmaIdx;
		pTxRing->TxCpuIdx = pTxRing->TxDmaIdx;
		RTMP_IO_WRITE32(pAd, pTxRing->hw_cidx_addr, pTxRing->TxCpuIdx);
	}

	pATEInfo->TxDoneCount = 0;

	if ((!IS_RT8592(pAd)) && (!IS_MT7610(pAd)))
	{
		if (pATEInfo->bQAEnabled == FALSE)
		{
			SetJapanFilter(pAd);
		}
	}
	
	TxRingGetReady(pAd);

	/* Start Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);

#ifdef MT76x0
	if (IS_MT76x0(pAd))
	{
		if (pATEInfo->bTestTimeReduction == FALSE)
		{
			RtmpOsMsDelay(200);
		}
	}
#endif /* MT76x0 */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Enable Tx */
	ATE_MAC_TX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
#ifdef RELEASE_EXCLUDE
	// TODO: Does RT3573 need BbpSoftReset() ???
	// TODO: Is it related to RT3593 RX sensitivity after TXCARR ???
#endif /* RELEASE_EXCLUDE */
	/* Soft reset BBP. */
	BbpSoftReset(pAd);

#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		/* Default value in BBP R22 is 0x0. */
	   	ATE_BBP_RESET_TX_MODE(pAd, BBP_R22, &BbpData);
	}
#endif /* RTMP_BBP */

	/* Clear bit4 to stop continuous Tx production test. */
	ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_QA
	/* add this for LoopBack mode */
	if (pATEInfo->bQARxStart == FALSE)  
	{
#ifdef TXBF_SUPPORT
		/* Enable Rx if Sending Sounding. Otherwise Disable */
		if (pATEInfo->txSoundingMode != 0)
		{
			ATE_MAC_RX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
		}
		else
#endif /* TXBF_SUPPORT */
		{
#if defined(RTMP_MAC) || defined(RLT_MAC)
			/* Disable Rx */
			ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
			//TODO: MT_MAC check
#endif
		}
	}

	if (pATEInfo->bQATxStart == TRUE)  
	{
		pATEInfo->TxStatus = 1;
	}
#else
#ifdef TXBF_SUPPORT
	/* Enable Rx if Sending Sounding. Otherwise Disable */
	if (pATEInfo->txSoundingMode != 0)
	{
		ATE_MAC_RX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
	}
	else
#endif /* TXBF_SUPPORT */
	{
		/* Disable Rx */
		ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
	}
#endif /* CONFIG_QA */

#ifdef RTMP_MAC_PCI
	RTMP_IO_READ32(pAd, pAd->TxRing[QID_AC_BE].hw_didx_addr, &pAd->TxRing[QID_AC_BE].TxDmaIdx);
	RTMP_IO_WRITE32(pAd, pAd->TxRing[QID_AC_BE].hw_cidx_addr, pAd->TxRing[QID_AC_BE].TxCpuIdx);

	pAd->RalinkCounters.KickTxCount++;
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	/* Enable Tx */
	ATE_MAC_TX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);

	if (pATEInfo->bQAEnabled == FALSE)
		SetJapanFilter(pAd);
	
	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

	pATEInfo->TxDoneCount = 0;

	/* Setup frame format. */
	ATESetUpFrame(pAd, 0);	

	/* Start Tx, RX DMA. */
	RtmpDmaEnable(pAd, 1);

	/* Check count is continuous or not yet. */
#ifdef RELEASE_EXCLUDE 
		/*
			Due to the type mismatch between "pAd->BulkOutRemained"(atomic_t)
			and "pATEInfo->TxCount"(UINT32)
		*/
#endif /* RELEASE_EXCLUDE */
	if (pATEInfo->TxCount == 0)
	{
		InterlockedExchange(&pAd->BulkOutRemained, 0);
	}
	else
	{
		InterlockedExchange(&pAd->BulkOutRemained, pATEInfo->TxCount);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("bulk out count = %d\n", atomic_read(&pAd->BulkOutRemained)));
	ASSERT((atomic_read(&pAd->BulkOutRemained) >= 0));

	if (atomic_read(&pAd->BulkOutRemained) == 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Send packet continuously\n"));

		/* NdisAcquireSpinLock() == spin_lock_bh() */
		/* NdisAcquireSpinLock only need one argument. */
		NdisAcquireSpinLock(&pAd->GenericLock);
		pAd->ContinBulkOut = TRUE;		
		NdisReleaseSpinLock(&pAd->GenericLock);

		/* BULK_OUT_LOCK() == spin_lock_irqsave() */
		BULK_OUT_LOCK(&pAd->BulkOutLock[0], IrqFlags);
		pAd->BulkOutPending[0] = FALSE;
		BULK_OUT_UNLOCK(&pAd->BulkOutLock[0], IrqFlags);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Send packets depend on counter\n"));

		NdisAcquireSpinLock(&pAd->GenericLock);
		pAd->ContinBulkOut = FALSE;		
		NdisReleaseSpinLock(&pAd->GenericLock);

		BULK_OUT_LOCK(&pAd->BulkOutLock[0], IrqFlags);
		pAd->BulkOutPending[0] = FALSE;
		BULK_OUT_UNLOCK(&pAd->BulkOutLock[0], IrqFlags);
	}				

	RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

	/* Kick bulk out */ 
	RTUSBKickBulkOut(pAd);
#endif /* RTMP_MAC_USB */

#ifdef RTMP_INTERNAL_TX_ALC
#if defined(RT3350) || defined(RT3352)
	if (pATEInfo->bTSSICalbrEnableG == TRUE)
	{
		if ((IS_RT3350(pAd)) || (IS_RT3352(pAd))) 
		{
			if ((pATEInfo->TxWI.TXWI_O.MCS == 7)
				&& (pATEInfo->TxWI.TXWI_O.BW == BW_20)	&& (pATEInfo->TxAntennaSel == 1))                  
			{
				if (pATEInfo->Channel == 7)
				{
					/* step 1: get calibrated channel 7 TSSI reading as reference */
					RtmpOsMsDelay(500);
					DBGPRINT(RT_DEBUG_TRACE, ("Channel %d, Calibrated Tx.Power0= 0x%04x\n", CurrentChannel, ChannelPower));

					/* read BBP R49[4:0] and write to EEPROM 0x6E */
					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);
					DBGPRINT(RT_DEBUG_TRACE, ("BBP R49 = 0x%02x\n", BbpData)); 
					BbpData &= 0x1f;
					TssiRefPerChannel[CurrentChannel-1] = BbpData;
					DBGPRINT(RT_DEBUG_TRACE, ("TSSI = 0x%02x\n", TssiRefPerChannel[CurrentChannel-1]));  
				}

				/* step 2: calibrate channel 1 and 13 TSSI delta values */
				else if (pATEInfo->Channel == 1)
				{
					/* Channel 1 */
					RtmpOsMsDelay(500);
					DBGPRINT(RT_DEBUG_TRACE, ("Channel %d, Calibrated Tx.Power0= 0x%04x\n", CurrentChannel, ChannelPower));

					/* read BBP R49[4:0] */
					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);
					DBGPRINT(RT_DEBUG_TRACE, ("BBP R49 = 0x%02x\n", BbpData)); 
					BbpData &= 0x1f;
					TssiRefPerChannel[CurrentChannel-1] = BbpData;
					DBGPRINT(RT_DEBUG_TRACE, ("TSSI = 0x%02x\n", TssiRefPerChannel[CurrentChannel-1]));
				}
				else if (pATEInfo->Channel == 13)
				{
					/* Channel 13 */
					RtmpOsMsDelay(500);
					DBGPRINT(RT_DEBUG_TRACE, ("Channel %d, Calibrated Tx.Power0= 0x%04x\n", CurrentChannel, ChannelPower));

					/* read BBP R49[4:0] */
					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);
					DBGPRINT(RT_DEBUG_TRACE, ("BBP R49 = 0x%02x\n", BbpData)); 
					BbpData &= 0x1f;
					TssiRefPerChannel[CurrentChannel-1] = BbpData;
					DBGPRINT(RT_DEBUG_TRACE, ("TSSI = 0x%02x\n", TssiRefPerChannel[CurrentChannel-1]));
				}
				else
				{
					DBGPRINT(RT_DEBUG_OFF, ("Channel %d, Calibrated Tx.Power0= 0x%04x\n", CurrentChannel, ChannelPower));
				}
			}
		}
	}
#endif /* defined(RT3350) || defined(RT3352) */
#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef RTMP_TEMPERATURE_COMPENSATION
#ifdef RT5592
	if ((pATEInfo->bTSSICalbrEnableG == TRUE) || (pATEInfo->bTSSICalbrEnableA == TRUE))
	{
		if (IS_RT5592(pAd))
		{
			if ((pATEInfo->TxWI.TXWI_O.MCS == 7)
				&& (pATEInfo->TxWI.TXWI_O.BW == BW_20))                  
			{
				if (pATEInfo->bTSSICalbrEnableG == TRUE)
				{
						DBGPRINT(RT_DEBUG_TRACE, ("Read  BBP_R49\n"));
					DBGPRINT(RT_DEBUG_TRACE, ("Channel %d\n", pATEInfo->Channel));

						ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);

						DBGPRINT(RT_DEBUG_TRACE, ("BBP R49=0x%x\n", BbpData));			
					
					if (pATEInfo->TxAntennaSel == ANT_0)
					{
						/* G band, TX0 */
						if (pATEInfo->Channel == 7)
						{
							/* step 1: read BBP R49 channel 7 TSSI reading as reference */
							pATEInfo->TssiReadSampleG[0][1] = (CHAR)BbpData;
					}
					/* step 2: calibrate channel 1 and 13 TSSI delta values */
					else if (pATEInfo->Channel == 1)
					{
						/* Channel 1 */
							pATEInfo->TssiReadSampleG[0][0] = (CHAR)BbpData;
					}
					else if (pATEInfo->Channel == 13)
					{
						/* Channel 13 */
							pATEInfo->TssiReadSampleG[0][2] = (CHAR)BbpData;
					}
					else
					{
						DBGPRINT(RT_DEBUG_OFF, ("Channel %d\n", pATEInfo->Channel));
					}
				}
					else if (pATEInfo->TxAntennaSel == ANT_1)
				{
						/* G band, TX1 */
						if (pATEInfo->Channel == 7)
						{
							/* step 1: read BBP R49 channel 7 TSSI reading as reference */
							pATEInfo->TssiReadSampleG[1][1] = (CHAR)BbpData;
						}
						/* step 2: calibrate channel 1 and 13 TSSI delta values */
						else if (pATEInfo->Channel == 1)
						{
							/* Channel 1 */
							pATEInfo->TssiReadSampleG[1][0] = (CHAR)BbpData;
						}
						else if (pATEInfo->Channel == 13)
						{
							/* Channel 13 */
							pATEInfo->TssiReadSampleG[1][2] = (CHAR)BbpData;
						}
						else
					{
						DBGPRINT(RT_DEBUG_TRACE, ("Channel %d\n", pATEInfo->Channel));
						}
					}
				}

				if (pATEInfo->bTSSICalbrEnableA == TRUE)
				{
						DBGPRINT(RT_DEBUG_TRACE, ("Read  BBP_R49\n"));
					DBGPRINT(RT_DEBUG_TRACE, ("Channel %d\n", pATEInfo->Channel));

						ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);

						DBGPRINT(RT_DEBUG_TRACE, ("BBP R49=0x%x\n", BbpData));			

					if (pATEInfo->TxAntennaSel == ANT_0)
					{
						/* A band, TX0 */
						if (pATEInfo->Channel == 100)
						{
							/* step 1: read BBP R49 channel 100 TSSI reading as reference */
							pATEInfo->TssiReadSampleA[0][1] = (CHAR)BbpData;
					}
					/* step 2: calibrate channel 36 and 161 TSSI delta values */
					else if (pATEInfo->Channel == 36)
					{
						/* Channel 36 */
							pATEInfo->TssiReadSampleA[0][0] = (CHAR)BbpData;
					}
					else if (pATEInfo->Channel == 161)
					{
						/* Channel 161 */
							pATEInfo->TssiReadSampleA[0][2] = (CHAR)BbpData;
						}
						else
						{
						DBGPRINT(RT_DEBUG_TRACE, ("Channel %d\n", pATEInfo->Channel));
						}
					}
					else if (pATEInfo->TxAntennaSel == ANT_1)
					{
						/* A band, TX1 */
						if (pATEInfo->Channel == 100)
						{
							/* step 1: read BBP R49 channel 100 TSSI reading as reference */
							pATEInfo->TssiReadSampleA[1][1] = (CHAR)BbpData;
						}
						/* step 2: calibrate channel 36 and 161 TSSI delta values */
						else if (pATEInfo->Channel == 36)
						{
							/* Channel 36 */
							pATEInfo->TssiReadSampleA[1][0] = (CHAR)BbpData;
						}
						else if (pATEInfo->Channel == 161)
						{
							/* Channel 161 */
							pATEInfo->TssiReadSampleA[1][2] = (CHAR)BbpData;
					}
					else
					{
							DBGPRINT(RT_DEBUG_TRACE, ("Channel %d\n", pATEInfo->Channel));
						}
					}
				}
			}
		}
	}
#endif /* RT5592 */
#endif /* RTMP_TEMPERATURE_COMPENSATION */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS RXFRAME(
	IN PRTMP_ADAPTER pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#ifdef RTMP_BBP
	UCHAR			BbpData = 0;
#endif /* RTMP_BBP */
#ifdef RTMP_MAC_USB
	UINT32			ring_index=0;
#endif /* RTMP_MAC_USB */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	ATE_ASIC_CALIBRATION(pAd, ATE_RXFRAME);

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Disable Rx of MAC block */
	ATE_MAC_RX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

#ifdef RTMP_BBP
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		/* Default value in BBP R22 is 0x0. */
		ATE_BBP_RESET_TX_MODE(pAd, BBP_R22, &BbpData);
	}
#endif /* RTMP_BBP */

#if defined (RT3883) || defined (RT3352) || defined (RT5350) 
	if (IS_RT3883(pAd) || IS_RT3352(pAd) || IS_RT5350(pAd)) 
	{
		if (pATEInfo->TxWI.TXWI_O.BW == BW_20)
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R103, 0xC0); 
		}
		else
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R103, 0x00);
		}
	}
#endif /* defined (RT3883) || defined (RT3352) || defined (RT5350) */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Clear bit4 to stop continuous Tx production test. */
	ATE_MAC_TX_CTS_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

	pATEInfo->Mode |= ATE_RXFRAME;

#ifdef RTMP_MAC_USB
	if (pAd->chipCap.MCUType != ANDES )
	{
		/* Abort Tx, Rx DMA. */
		RtmpDmaEnable(pAd, 0);
	}
#endif /* RTMP_MAC_USB */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Disable Tx of MAC block. */
	ATE_MAC_TX_DISABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

#ifdef RTMP_MAC_USB
    /* Reset Rx RING */
	for (ring_index = 0; ring_index < (RX_RING_SIZE); ring_index++)
	{
		PRX_CONTEXT  pRxContext = &(pAd->RxContext[ring_index]);

		pRxContext->InUse = FALSE;
		pRxContext->IRPPending = FALSE;
		pRxContext->Readable = FALSE;

		/* Get the URB from kernel(i.e., host control driver) back to driver. */
		RTUSB_UNLINK_URB(pRxContext->pUrb);

		/* Sleep 200 microsecs to give cancellation time to work. */
		RtmpusecDelay(200);
		pAd->BulkInReq = 0;

		pAd->PendingRx = 0;
		/* Next Rx Read index */
		pAd->NextRxBulkInReadIndex = 0;
		/* Rx Bulk pointer */
		pAd->NextRxBulkInIndex		= RX_RING_SIZE - 1;	
		pAd->NextRxBulkInPosition = 0;			
	}

	/* read to clear counters */
	RTMP_IO_READ32(pAd, RX_STA_CNT0, &MacData); /* RX PHY & RX CRC count */
	RTMP_IO_READ32(pAd, RX_STA_CNT1, &MacData); /* RX PLCP error count & CCA false alarm count */
	RTMP_IO_READ32(pAd, RX_STA_CNT2, &MacData); /* RX FIFO overflow frame count & RX duplicated filtered frame count */

	pAd->ContinBulkIn = TRUE;

	/* Enable Tx, RX DMA. */
	RtmpDmaEnable(pAd, 1);
#endif /* RTMP_MAC_USB */
#ifdef MT76x0
	if (IS_MT76x0(pAd))
	{
		if (pATEInfo->bTestTimeReduction == FALSE)
		{
			RtmpOsMsDelay(100);
		}
	}
#endif /* MT76x0 */

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/* Enable Rx of MAC block. */
	ATE_MAC_RX_ENABLE(pAd, MAC_SYS_CTRL, &MacData);
#else
	//TODO: MT_MAC check
#endif

#ifdef RELEASE_EXCLUDE
/*
        In some platform, the RX sensitivy is bad. There are two solutions for this bug.
        One is to chnage BBP_R31 from 0x08 to 0x0A.
        The other is to change MAC_R5D4 from xDxxxxxx to x1xxxxxx.
*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT3090
	if (IS_RT3090(pAd))
	{
                /* RT309x version E has fixed this issue */
                if ((pAd->NicConfig2.field.DACTestBit == 1) && ((pAd->MACVersion & 0xffff) < 0x0211))
                {
                        /* patch tx EVM issue temporarily */
                        RTMP_IO_READ32(pAd, LDO_CFG0, &MacData);
                        MacData = ((MacData & 0xE0FFFFFF) | 0x0D000000);
                        RTMP_IO_WRITE32(pAd, LDO_CFG0, MacData);
                }
                else
                {
                        RTMP_IO_READ32(pAd, LDO_CFG0, &MacData);
                        MacData = ((MacData & 0xE0FFFFFF) | 0x01000000);
                        RTMP_IO_WRITE32(pAd, LDO_CFG0, MacData);
                }
	}
#endif /* RT3090 */

#ifdef RTMP_MAC_USB
	/* Kick bulk in. */
	RTUSBBulkReceive(pAd);
#endif /* RTMP_MAC_USB */

#ifdef RT6352
	if (pATEInfo->bQAEnabled == FALSE)
	{
		if (IS_RT6352(pAd) && (pAd->CommonCfg.Chip_VerID > 1) && (pAd->CommonCfg.Chip_E_Number == 2))
		{
			UCHAR 	RFValue;

			if (pATEInfo->RxAntennaSel != 0) /* 1R */
			{
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R31, 0x08);
			}
			else
			{
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R31, 0x48);
			}

			RT635xReadRFRegister(pAd, RF_BANK0, RF_R28, &RFValue);
			if ((pATEInfo->Channel == 11) && (pATEInfo->TxWI.TXWI_O.BW == BW_40))
				RFValue |= 0x4;
			else
				RFValue &= (~0x4);
			RT635xWriteRFRegister(pAd, RF_BANK0, RF_R28, RFValue);
		}
	}
#endif /* RT6352 */

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


/*
==========================================================================
    Description:
        Set ATE operation mode to
        0. ATESTART  = Start/Reset ATE Mode
        1. ATESTOP   = Stop ATE Mode
        2. TXCARR    = Transmit Carrier
        3. TXCONT    = Continuous Transmit
        4. TXFRAME   = Transmit Frames
        5. RXFRAME   = Receive Frames
#ifdef CONFIG_QA
        6. TXSTOP    = Stop Any Type of Transmition
        7. RXSTOP    = Stop Receiving Frames        
#endif

    Return:
        NDIS_STATUS_SUCCESS if all parameters are OK.
==========================================================================
*/
static NDIS_STATUS ATECmdHandler(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	ATE_INFO *pATEInfo = &(pAd->ate);
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	BOOLEAN bNeedTxRx = FALSE;
	UINT value32;

	DBGPRINT(RT_DEBUG_TRACE, ("===> %s\n", __FUNCTION__));

#ifdef CONFIG_RT2880_ATE_CMD_NEW
#ifdef RELEASE_EXCLUDE
	/* 
		Do not call ATEAsicSwitchChannel(pAd) here. 
		Some platform(e.g., INF_AR9) will hang when you enter ATE mode. 
		Maybe it is due to switching channel(access of RF register) and
		leaving normal mode at the same time.
	*/
#endif /* RELEASE_EXCLUDE */
	if (!strcmp(arg, "ATESTART")) 		
	{
		/* Enter/Reset ATE mode and set Tx/Rx Idle */
		Status = ATESTART(pAd);
	}
	else if (!strcmp(arg, "ATESTOP")) 
	{
		/* Leave ATE mode */
		Status = ATESTOP(pAd);
	}
#else
	if (!strcmp(arg, "APSTOP")) 		
	{
		Status = ATESTART(pAd);
	}
	else if (!strcmp(arg, "APSTART")) 
	{
		Status = ATESTOP(pAd);
	}
#endif
	else if (!strcmp(arg, "TXCARR"))	
	{
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXCARR;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXCARR;
		}
#endif /* RT6352 */
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpOsMsDelay(5);

		Status = TXCARR(pAd);
	}
	else if (!strcmp(arg, "TXCARS"))
	{
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXCARRSUPP;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXCARRSUPP;
		}
#endif /* RT6352 */
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpOsMsDelay(5);

		Status = TXCARS(pAd);
	}
	else if (!strcmp(arg, "TXCONT"))	
	{
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXCONT;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXCONT;
		}
#endif /* RT6352 */
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpOsMsDelay(5);

		Status = TXCONT(pAd);
	}
	else if (!strcmp(arg, "TXFRAME")) 
	{
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXFRAME;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXFRAME;
		}
#endif /* RT6352 */
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpOsMsDelay(5);

		Status = TXFRAME(pAd);
#ifdef MT76x0
		if (IS_MT76x0(pAd))
		{
			BOOLEAN ttr;
			
			ttr = pATEInfo->bTestTimeReduction;

			if (1 /* ttr == TRUE */)
			{
				ATEAsicSwitchChannel(pAd);
			}
		}
#endif /* MT76x0 */
#ifdef MT76x2
#ifdef TXBF_SUPPORT
		/* Enable TxBf profile update */
		RTMP_IO_READ32(pAd, PFMU_R1, &value32);
		value32 |= 0x330;
		RTMP_IO_WRITE32(pAd, PFMU_R1, value32);
#endif
#endif
		Status = TXFRAME(pAd);
	}
	else if (!strcmp(arg, "RXFRAME")) 
	{
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_TXSTOP;
			pATEInfo->Mode |= ATE_RXFRAME;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_TXSTOP;
			pATEInfo->Mode |= ATE_RXFRAME;
		}
#endif /* RT6352 */
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpusecDelay(5);

		Status = RXFRAME(pAd);
	}
	else if (!strcmp(arg, "TXAPPLY")) 
	{
		/* sanity check */
		if ((pATEInfo->Mode != ATE_TXFRAME))
		{
			/* need "TXFRAME", not only "TXAPPLY" */
			bNeedTxRx = TRUE;
		}
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXFRAME;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_RXSTOP;
			pATEInfo->Mode |= ATE_TXFRAME;
		}
#endif /* RT6352 */
		
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpOsMsDelay(5);

		if (bNeedTxRx == TRUE)
		{
			Status = TXFRAME(pAd);
		}
#ifdef MT76x0
		if (IS_MT76x0(pAd))
		{
			BOOLEAN ttr;
			
			ttr = pATEInfo->bTestTimeReduction;

			if (1 /* ttr == TRUE */)
			{
				ATEAsicSwitchChannel(pAd);
			}
		}
#endif /* MT76x0 */
	}
	else if (!strcmp(arg, "RXAPPLY")) 
	{
		/* sanity check */
		if ((pATEInfo->Mode != ATE_RXFRAME) || (pATEInfo->Mode == ATE_START))
		{
			/* need "RXFRAME", not only "RXAPPLY" */
			bNeedTxRx = TRUE;
		}
#ifdef RT3593
		if (IS_RT3593(pAd))
		{
			/* Set ATE Mode in advance for RT3593ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_TXSTOP;
			pATEInfo->Mode |= ATE_RXFRAME;
		}
#endif /* RT3593 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			/* Set ATE Mode in advance for RT6352ATEAsicSetTxRxPath() */
			pATEInfo->Mode &= ATE_TXSTOP;
			pATEInfo->Mode |= ATE_RXFRAME;
		}
#endif /* RT6352 */
		ATEAsicSwitchChannel(pAd);
		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pATEInfo->Channel);
		RtmpusecDelay(5);

		if (bNeedTxRx == TRUE)
		{
			Status = RXFRAME(pAd);
		}
	}
#ifdef CONFIG_QA
	/* Enter ATE mode and set Tx/Rx Idle */
	else if (!strcmp(arg, "TXSTOP"))
	{
		Status = TXSTOP(pAd);
	}
	else if (!strcmp(arg, "RXSTOP"))
	{
		Status = RXSTOP(pAd);
	}
#endif /* CONFIG_QA */
	else
	{	
		DBGPRINT_ERR(("ATE : Invalid arg in %s!\n", __FUNCTION__));
		Status = NDIS_STATUS_INVALID_DATA;
	}
	RtmpOsMsDelay(5);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== %s\n", __FUNCTION__));
	return Status;
}


INT	Set_ATE_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	
	/* Handle ATEACTIVE and ATEPASSIVE commands as a special case */
	if (!strcmp(arg, "ATEACTIVE"))
	{
		pATEInfo->PassiveMode = FALSE;
		return TRUE;
	}

	if (!strcmp(arg, "ATEPASSIVE"))
	{
		pATEInfo->PassiveMode = TRUE;
		return TRUE;
	}

	/* Disallow all other ATE commands in passive mode */
	if (pATEInfo->PassiveMode)
		return TRUE;

	if (ATECmdHandler(pAd, arg) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
		return TRUE;
	}
	else
	{
		DBGPRINT_ERR(("Set_ATE_Proc Failed\n"));
		return FALSE;
	}
}


/* 
==========================================================================
    Description:
        Set ATE ADDR1=DA for TxFrame(AP  : To DS = 0 ; From DS = 1)
        or
        Set ATE ADDR3=DA for TxFrame(STA : To DS = 1 ; From DS = 0)        
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_DA_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *value;
	INT					octet;

	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */	
	if (strlen(arg) != 17)  
		return FALSE;

	for (octet = 0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}
#ifdef CONFIG_AP_SUPPORT
		AtoH(value, &pATEInfo->Addr1[octet++], 1);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		AtoH(value, &pATEInfo->Addr3[octet++], 1);
#endif /* CONFIG_STA_SUPPORT */
	}

	/* sanity check */
	if (octet != MAC_ADDR_LEN)
	{
		return FALSE;  
	}
#ifdef CONFIG_AP_SUPPORT		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_DA_Proc (DA = %02x:%02x:%02x:%02x:%02x:%02x)\n", 
		pATEInfo->Addr1[0], pATEInfo->Addr1[1], pATEInfo->Addr1[2], pATEInfo->Addr1[3],
		pATEInfo->Addr1[4], pATEInfo->Addr1[5]));

#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_DA_Proc (DA = %02x:%02x:%02x:%02x:%02x:%02x)\n", 
		pATEInfo->Addr3[0], pATEInfo->Addr3[1], pATEInfo->Addr3[2], pATEInfo->Addr3[3],
		pATEInfo->Addr3[4], pATEInfo->Addr3[5]));
#endif /* CONFIG_STA_SUPPORT */
	
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_DA_Proc Success\n"));
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE ADDR3=SA for TxFrame(AP  : To DS = 0 ; From DS = 1)
        or
        Set ATE ADDR2=SA for TxFrame(STA : To DS = 1 ; From DS = 0)
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_SA_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *value;
	INT					octet;

	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */	
	if (strlen(arg) != 17)  
		return FALSE;

	for (octet=0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}
#ifdef CONFIG_AP_SUPPORT
		AtoH(value, &pATEInfo->Addr3[octet++], 1);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		AtoH(value, &pATEInfo->Addr2[octet++], 1);
#endif /* CONFIG_STA_SUPPORT */
	}

	/* sanity check */
	if (octet != MAC_ADDR_LEN)
	{
		return FALSE;
	}
#ifdef CONFIG_AP_SUPPORT		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_SA_Proc (SA = %02x:%02x:%02x:%02x:%02x:%02x)\n", 
		pATEInfo->Addr3[0], pATEInfo->Addr3[1], pATEInfo->Addr3[2], pATEInfo->Addr3[3],
		pATEInfo->Addr3[4], pATEInfo->Addr3[5]));
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_SA_Proc (SA = %02x:%02x:%02x:%02x:%02x:%02x)\n", 
		pATEInfo->Addr2[0], pATEInfo->Addr2[1], pATEInfo->Addr2[2], pATEInfo->Addr2[3],
		pATEInfo->Addr2[4], pATEInfo->Addr2[5]));
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_SA_Proc Success\n"));

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE ADDR2=BSSID for TxFrame(AP  : To DS = 0 ; From DS = 1)
        or
        Set ATE ADDR1=BSSID for TxFrame(STA : To DS = 1 ; From DS = 0)

    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_BSSID_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *value;
	INT					octet;

	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */	
	if (strlen(arg) != 17)  
		return FALSE;

	for (octet=0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}
#ifdef CONFIG_AP_SUPPORT
		AtoH(value, &pATEInfo->Addr2[octet++], 1);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		AtoH(value, &pATEInfo->Addr1[octet++], 1);
#endif /* CONFIG_STA_SUPPORT */
	}

	/* sanity check */
	if (octet != MAC_ADDR_LEN)
	{
		return FALSE;
	}
#ifdef CONFIG_AP_SUPPORT		
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_BSSID_Proc (BSSID = %02x:%02x:%02x:%02x:%02x:%02x)\n",	
		pATEInfo->Addr2[0], pATEInfo->Addr2[1], pATEInfo->Addr2[2], pATEInfo->Addr2[3],
		pATEInfo->Addr2[4], pATEInfo->Addr2[5]));

#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_BSSID_Proc (BSSID = %02x:%02x:%02x:%02x:%02x:%02x)\n",	
		pATEInfo->Addr1[0], pATEInfo->Addr1[1], pATEInfo->Addr1[2], pATEInfo->Addr1[3],
		pATEInfo->Addr1[4], pATEInfo->Addr1[5]));
#endif /* CONFIG_STA_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_BSSID_Proc Success\n"));

	return TRUE;
}


#ifdef HUAWEI_ATE
#ifdef CONFIG_AP_SUPPORT
/* 
==========================================================================
    Description:
        Set ATE specific MAC address for RxFrame
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_MAC_Filter_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *value;
	INT					i;

	if (pATEInfo->bMACFilterEnable == FALSE)
	{
		DBGPRINT_ERR(("ATE MAC address filter disabled !\n"));

		return FALSE;
	}

	/* Mac address acceptable format 01:02:03:04:05:06 length 17 */	
	if (strlen(arg) != 17)  
		return FALSE;

	for (i=0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}
		AtoH(value, &pATEInfo->Filter[i++], 1);
	}

	/* sanity check */
	if (i != MAC_ADDR_LEN)
	{
		return FALSE;
	}

	/* 
		The original BSSID will be recovered
		by APStartUp() in ATESTOP.
	*/
	ATEAsicSetBssid(pAd, pATEInfo->Filter);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_MAC_Filter_Proc (FILETER = %02x:%02x:%02x:%02x:%02x:%02x)\n",	
		pATEInfo->Filter[0], pATEInfo->Filter[1], pATEInfo->Filter[2],
		pATEInfo->Filter[3], pATEInfo->Filter[4], pATEInfo->Filter[5]));

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_MAC_Filter_Proc Success\n"));

	return TRUE;
}


/* 
==========================================================================
    Description:
        Enable ATE specific MAC address filter for RxFrame
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT Set_ATE_MAC_Filter_Enable_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR MACFilterEnabled = FALSE;

	MACFilterEnabled = simple_strtol(arg, 0, 10);

	if (MACFilterEnabled == 0)
	{
		pATEInfo->bMACFilterEnable = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATE MAC Filter diabled !\n"));
	}
	else
	{
		pATEInfo->bMACFilterEnable = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATE MAC Filter enabled !\n"));
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_MAC_Filter_Enable_Proc Success\n"));

	return TRUE;
}
#endif /* CONFIG_AP_SUPPORT */
#endif /* HUAWEI_ATE */


/* 
==========================================================================
    Description:
        Set ATE Tx Channel

    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_CHANNEL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR channel;
	
#ifdef HUAWEI_ATE
#if defined(RT3062) || defined(RT3050)
	EEPROM_TX_PWR_STRUC	    Power;
	EEPROM_TX_PWR_STRUC	    Power2;
	UCHAR channel_power_index;
	UCHAR channel_is_odd;
#endif /* defined(RT3062) || defined(RT3050) */
#endif /* HUAWEI_ATE */

	channel = simple_strtol(arg, 0, 10);

	/* to allow A band channel : ((channel < 1) || (channel > 14)) */
	if ((channel < 1) || (channel > 216))
	{
		DBGPRINT_ERR(("Set_ATE_CHANNEL_Proc::Out of range, it should be in range of 1~14.\n"));
		return FALSE;
	}

	pATEInfo->Channel = channel;

#ifdef HUAWEI_ATE
#if defined(RT3062) || defined(RT3050)
	/* 
		This is just for RT3062 and RT3050 now. 
		It is supposed that you have calibrated tx power value in e2p for ate channel.
		We read tx power from e2p corresponding to ate channel.
	*/
	
	channel_power_index = ((channel-1) >> 1);
	channel_is_odd = (channel%2);

	/*
		Read Tx power value for all channels

		Value from 1 - 0x7f. Default value is 24.
		Power value : 2.4G 0x00 (0) ~ 0x1F (31)
					: 5.5G 0xF9 (-7) ~ 0x0F (15)
		0. 11b/g, ch1 - ch 14
	*/
	if (channel_power_index < 7)
	{
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX_PWR_OFFSET + (channel_power_index << 1), Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX2_PWR_OFFSET + (channel_power_index << 1), Power2.word);

		if (channel_is_odd)
		{
			if ((Power.field.Byte1 > 31) || (Power.field.Byte1 < 0))
				pATEInfo->TxPower0 = DEFAULT_RF_TX_POWER;
			else
				pATEInfo->TxPower0 = Power.field.Byte1;

			if ((Power2.field.Byte1 > 31) || (Power2.field.Byte1 < 0))
				pATEInfo->TxPower1 = DEFAULT_RF_TX_POWER;
			else
				pATEInfo->TxPower1 = Power2.field.Byte1;				
		}
		else
		{
			if ((Power.field.Byte0 > 31) || (Power.field.Byte0 < 0))
				pATEInfo->TxPower0 = DEFAULT_RF_TX_POWER;
			else
				pATEInfo->TxPower0 = Power.field.Byte0;

			if ((Power2.field.Byte0 > 31) || (Power2.field.Byte0 < 0))
				pATEInfo->TxPower1 = DEFAULT_RF_TX_POWER;
			else
				pATEInfo->TxPower1 = Power2.field.Byte0;				
		}

		/* update tx power */
		ATETxPwrHandler(pAd, 0);
		ATETxPwrHandler(pAd, 1);
	}
	
#endif /* defined(RT3062) || defined(RT3050) */
#endif /* HUAWEI_ATE */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_CHANNEL_Proc (ATE Channel = %d)\n", pATEInfo->Channel));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_CHANNEL_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Initialize the channel - set the power and switch to selected channel
			0 => use current value
			else set channel to specified channel
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_INIT_CHAN_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	int index;
	int value;

	/* Get channel parameter */
	value = simple_strtol(arg, 0, 10);

	if (value<0 || value>216)
	{
		DBGPRINT_ERR(("Set_ATE_INIT_CHAN_Proc::Channel out of range\n"));
		return FALSE;
	}

	if (value != 0)
		pATEInfo->Channel = value;

	for (index=0; index<MAX_NUM_OF_CHANNELS; index++)
	{
		if (pATEInfo->Channel == pAd->TxPower[index].Channel)
		{
			pATEInfo->TxPower0 = pAd->TxPower[index].Power;
			pATEInfo->TxPower1 = pAd->TxPower[index].Power2;
#ifdef DOT11N_SS3_SUPPORT
			if (IS_RT2883(pAd) || IS_RT3593(pAd) || IS_RT3883(pAd))
				pATEInfo->TxPower2 = pAd->TxPower[index].Power3;
#endif /* DOT11N_SS3_SUPPORT */
			break;
		}
	}

	if (index == MAX_NUM_OF_CHANNELS)
	{
		DBGPRINT_ERR(("Set_ATE_INIT_CHAN_Proc::Channel not found\n"));
		return FALSE;
	}

	/* Force non-QATool mode */
	pATEInfo->bQATxStart = FALSE;
	pATEInfo->bQARxStart = FALSE;

	ATETxPwrHandler(pAd, 0);
	ATETxPwrHandler(pAd, 1);
#ifdef DOT11N_SS3_SUPPORT
	ATETxPwrHandler(pAd, 2);
#endif /* DOT11N_SS3_SUPPORT */

#if defined(RT2883) || defined(RT3883) || defined(RT3593) || defined(MT76x2)
	if (IS_RT2883(pAd) || IS_RT3883(pAd) || IS_RT3593(pAd) || IS_MT76x2(pAd))
	{
		ATEAsicSwitchChannel(pAd);
	}
#endif /* defined(RT2883) || defined(RT3883) || defined(RT3593) || defined(MT76x2) */

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_INIT_CHAN_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Power
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
static INT ATESetAntennaTxPower(RTMP_ADAPTER *pAd, RTMP_STRING *arg, INT Antenna)

{
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR TxPower;
	INT  index, maximun_index;

	pATEInfo = &(pAd->ate);
	TxPower = simple_strtol(arg, 0, 10);
	index = Antenna;
	maximun_index = pAd->Antenna.field.TxPath - 1;

	if ((index < 0) || (index > maximun_index))
	{
		DBGPRINT_ERR(("No such antenna! The range is 0~%d.\n", maximun_index));
		return FALSE;
	}

	if (pATEInfo->Channel <= 14) /* 2.4 GHz */
	{
#ifdef RT65xx
		if (IS_MT76x0(pAd) || IS_MT76x2(pAd))
		{
			if ((TxPower > 63 /* 0x3F */) || (TxPower < 0))
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~63 in G band.\n"));
				return FALSE;
			}
		}
		else
#endif /* RT65xx */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
			if ((TxPower > 47 /* 0x2F */) || (TxPower < 0))
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~47 in G band.\n"));
				return FALSE;
			}
		}
		else
#endif /* RT6352 */
#ifdef MT7601
		if (IS_MT7601(pAd))
		{
			CHAR MaxPower;
			UINT32 RegValue = 0;

			RTMP_IO_READ32(pAd, TX_ALC_CFG_0, &RegValue);
			MaxPower = (RegValue & 0x003F0000) >> 16;
			if (TxPower > MaxPower )
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~39 in G band.\n"));
				return FALSE;
			}
		}
		else
#endif /* MT7601 */
#ifdef RT5592
		if (IS_RT5592(pAd))
		{
			if ((TxPower > 39 /* 0x27 */) || (TxPower < 0))
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~39 in G band.\n"));
				return FALSE;
			}
		}
		else
#endif /* RT5592 */
		if (!IS_RT3390(pAd))
		{
			if ((TxPower > 31) || (TxPower < 0))
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~31 in G band.\n"));
				return FALSE;
			}
		}
	}
	else /* 5.5 GHz */
	{
#ifdef RELEASE_EXCLUDE
		/* 
			New TxPower range is 0~31(5 bits) in A band;
			RT28xx and RT2883 TxPower range is -7~15 in A band.
		*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT65xx
		if (IS_MT76x0(pAd) || IS_MT76x2(pAd))
		{
			if ((TxPower > 63 /* 0x3F */) || (TxPower < 0))
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~63 in A band.\n"));
				return FALSE;
			}
		}
		else
#endif /* RT65xx */
#ifdef RT5592
		if (IS_RT5592(pAd))
		{
			if ((TxPower > 43 /* 0x2B */) || (TxPower < 0))
			{
				DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
				DBGPRINT_ERR(("TxPower range is 0~43 in A band.\n"));
				return FALSE;
			}
		}
		else
#endif /* RT5592 */
		if ((TxPower > (pATEInfo->MaxTxPowerBandA)) || (TxPower < (pATEInfo->MinTxPowerBandA)))
		{
			DBGPRINT_ERR(("Set_ATE_TX_POWER%d_Proc::Out of range! (Value=%d)\n", index, TxPower));
			DBGPRINT_ERR(("TxPower range is %d~%d in A band.\n", pATEInfo->MinTxPowerBandA, pATEInfo->MaxTxPowerBandA));
			return FALSE;
		}
	}

	switch (index)
	{
		case 0:
			pATEInfo->TxPower0 = TxPower;
			break;
		case 1:
			pATEInfo->TxPower1 = TxPower;
			break;
#ifdef DOT11N_SS3_SUPPORT
		case 2:
			pATEInfo->TxPower2 = TxPower;
			break;	
#endif /* DOT11N_SS3_SUPPORT */
		default: 
#ifdef RELEASE_EXCLUDE
			/* fatal error */
#endif /* RELEASE_EXCLUDE */
			return FALSE;	
	}

#ifdef RELEASE_EXCLUDE
	/* it can not be removed (for example: RT3593) */
#endif /* RELEASE_EXCLUDE */
	ATETxPwrHandler(pAd, index);

#if defined(RT2883) || defined(RT3883)
	if (IS_RT2883(pAd) || IS_RT3883(pAd))
	{
		ATEAsicSwitchChannel(pAd);
	}
#endif /* defined(RT2883) || defined(RT3883) */

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_POWER%d_Proc Success\n", index));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Power0
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_POWER0_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT ret;
	
	ret = ATESetAntennaTxPower(pAd, arg, 0);
	return ret;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Power1
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_POWER1_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT ret;
	
	ret = ATESetAntennaTxPower(pAd, arg, 1);
	return ret;
}


#ifdef DOT11N_SS3_SUPPORT
/* 
==========================================================================
    Description:
        Set ATE Tx Power2
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_POWER2_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT ret;
	
	ret = ATESetAntennaTxPower(pAd, arg, 2);
	return ret;
}
#endif /* DOT11N_SS3_SUPPORT */


/* 
==========================================================================
    Description:
        Set ATE Tx Power for evaluation 
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_POWER_EVALUATION_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{

	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->TxPwrEvaluation!= NULL)
		pATEInfo->pChipStruct->TxPwrEvaluation(pAd);
	else
		return FALSE;

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Antenna
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_Antenna_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR value;
	INT maximun_index = pAd->Antenna.field.TxPath;
	
	value = simple_strtol(arg, 0, 10);

	if ((value > maximun_index) || (value < 0))
	{
		DBGPRINT_ERR(("Set_ATE_TX_Antenna_Proc::Out of range (Value=%d)\n", value));
		DBGPRINT_ERR(("Set_ATE_TX_Antenna_Proc::The range is 0~%d\n", maximun_index));

		return FALSE;
	}

	pATEInfo->TxAntennaSel = value;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_Antenna_Proc (Antenna = %d)\n", pATEInfo->TxAntennaSel));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_Antenna_Proc Success\n"));

	/* calibration power unbalance issues */
#ifdef RELEASE_EXCLUDE
	/* It must not be removed for MT7620 TSSI calibration. */
#endif /* RELEASE_EXCLUDE */
#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		if (pATEInfo->bTestTimeReduction == FALSE)
		{
			ATEAsicSwitchChannel(pAd);
		}
	}
#endif /* RT6352 */
#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Rx Antenna
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_RX_Antenna_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR value;
	INT maximun_index = pAd->Antenna.field.RxPath;
	
	value = simple_strtol(arg, 0, 10);

	if ((value > maximun_index) || (value < 0))
	{
		DBGPRINT_ERR(("Set_ATE_RX_Antenna_Proc::Out of range (Value=%d)\n", value));
		DBGPRINT_ERR(("Set_ATE_RX_Antenna_Proc::The range is 0~%d\n", maximun_index));

		return FALSE;
	}

	pATEInfo->RxAntennaSel = value;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_RX_Antenna_Proc (Antenna = %d)\n", pATEInfo->RxAntennaSel));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_RX_Antenna_Proc Success\n"));

	/* calibration power unbalance issues */
#ifdef RELEASE_EXCLUDE
	/* It must not be removed for MT7620 TSSI calibration. */
#endif /* RELEASE_EXCLUDE */
#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		if (pATEInfo->bTestTimeReduction == FALSE)
		{
			ATEAsicSwitchChannel(pAd);
		}
	}
#endif /* RT6352 */
#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


VOID DefaultATEAsicExtraPowerOverMAC(
	IN	PRTMP_ADAPTER 		pAd)
{
#if defined(RTMP_MAC) || defined(RLT_MAC)
	UINT32 ExtraPwrOverMAC = 0;
	UINT32 ExtraPwrOverTxPwrCfg7 = 0, ExtraPwrOverTxPwrCfg8 = 0, ExtraPwrOverTxPwrCfg9 = 0;

	/* For OFDM_54 and HT_MCS_7, extra fill the corresponding register value into MAC 0x13D4 */
	RTMP_IO_READ32(pAd, 0x1318, &ExtraPwrOverMAC);  
	ExtraPwrOverTxPwrCfg7 |= (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for OFDM 54 */
	RTMP_IO_READ32(pAd, 0x131C, &ExtraPwrOverMAC);  
	ExtraPwrOverTxPwrCfg7 |= (ExtraPwrOverMAC & 0x0000FF00) << 8; /* Get Tx power for HT MCS 7 */			
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_7, ExtraPwrOverTxPwrCfg7);

	/* For STBC_MCS_7, extra fill the corresponding register value into MAC 0x13DC */
	RTMP_IO_READ32(pAd, 0x1324, &ExtraPwrOverMAC);  
	ExtraPwrOverTxPwrCfg9 |= (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for STBC MCS 7 */
	RTMP_IO_WRITE32(pAd, TX_PWR_CFG_9, ExtraPwrOverTxPwrCfg9);

	if (IS_RT5392(pAd))
	{	
		/*  For HT_MCS_15, extra fill the corresponding register value into MAC 0x13DC */
		RTMP_IO_READ32(pAd, 0x1320, &ExtraPwrOverMAC);  
		ExtraPwrOverTxPwrCfg8 |= (ExtraPwrOverMAC & 0x0000FF00) >> 8; /* Get Tx power for HT MCS 15 */
		RTMP_IO_WRITE32(pAd, TX_PWR_CFG_8, ExtraPwrOverTxPwrCfg8);
		
		DBGPRINT(RT_DEBUG_TRACE, ("Offset =0x13D8, TxPwr = 0x%08X, ", (UINT)ExtraPwrOverTxPwrCfg8));
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("Offset = 0x13D4, TxPwr = 0x%08X, Offset = 0x13DC, TxPwr = 0x%08X\n", 
		(UINT)ExtraPwrOverTxPwrCfg7, 
		(UINT)ExtraPwrOverTxPwrCfg9));
#else
	//TODO: MT_MAC check
#endif
}


VOID ATEAsicExtraPowerOverMAC(
	IN	PRTMP_ADAPTER 		pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->AsicExtraPowerOverMAC!= NULL)
		pATEInfo->pChipStruct->AsicExtraPowerOverMAC(pAd);

	return;
}


VOID ATEAsicTemperCompensation(
	IN	PRTMP_ADAPTER 		pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->TemperCompensation!= NULL)
		pATEInfo->pChipStruct->TemperCompensation(pAd);

	return;
}


#ifdef RT3350
/* 
==========================================================================
    Description:
        Set ATE PA bias to improve EVM
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT Set_ATE_PA_Bias_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR PABias = 0;
	UCHAR RFValue;
	
	if (!IS_RT3350(pAd))
	{
		return FALSE;
	}

	PABias = simple_strtol(arg, 0, 10);

	if (PABias >= 16)
	{
		DBGPRINT_ERR(("Set_ATE_PA_Bias_Proc::Out of range, it should be in range of 0~15.\n"));
		return FALSE;
	}

	pATEInfo->PABias = PABias;

#ifdef RTMP_RF_RW_SUPPORT
	if (IS_RT3350(pAd))
	{
#ifdef RTMP_RF
		RT30xxReadRFRegister(pAd, RF_R19, (PUCHAR)&RFValue);
		RFValue = (((RFValue & 0x0F) | (pATEInfo->PABias << 4)));
		RT30xxWriteRFRegister(pAd, RF_R19, (UCHAR)RFValue);
#endif /* RTMP_RF */
	}
#endif /* RTMP_RF_RW_SUPPORT */
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_PA_Bias_Proc (PABias = %d)\n", pATEInfo->PABias));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_PA_Bias_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}
#endif /* RT3350 */


/* 
==========================================================================
    Description:
        Set ATE RF frequence offset
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_FREQ_OFFSET_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	INT ret = FALSE;

	if (pATEInfo->pChipStruct->Set_FREQ_OFFSET_Proc != NULL)
	{
		ret = pATEInfo->pChipStruct->Set_FREQ_OFFSET_Proc(pAd, arg);
	}

	if (ret == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_FREQ_OFFSET_Proc (RFFreqOffset = %d)\n", pATEInfo->RFFreqOffset));
		DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_FREQ_OFFSET_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
		DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	}

	return ret;
}


/* 
==========================================================================
    Description:
        Set ATE RF BW
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_BW_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	INT status = FALSE;
	UCHAR bw = 0;


	if (pATEInfo->pChipStruct->Set_BW_Proc != NULL)
	{
		status = pATEInfo->pChipStruct->Set_BW_Proc(pAd, arg);

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		bw = pATEInfo->TxWI.TXWI_N.BW;
#endif /* RLT_MAC*/
#ifdef RTMP_MAC

f (pAd->chipCap.hif_type == HIF_RTMP)
		bw = pATEInfo->TxWI.TXWI_O.BW;
#endif /* RTMP_MAC */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_BW_Proc (BBPCurrentBW = %d)\n", bw));
	}	
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_BW_Proc %s\n", status == TRUE ? "success" : "failed"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF,  (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame length
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_LENGTH_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	pATEInfo->TxLength = simple_strtol(arg, 0, 10);

	if ((pATEInfo->TxLength < 24) || (pATEInfo->TxLength > (MAX_FRAME_SIZE - 34/* == 2312 */)))
	{
		pATEInfo->TxLength = (MAX_FRAME_SIZE - 34/* == 2312 */);
		DBGPRINT_ERR(("Set_ATE_TX_LENGTH_Proc::Out of range, it should be in range of 24~%d.\n", (MAX_FRAME_SIZE - 34/* == 2312 */)));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_LENGTH_Proc (TxLength = %d)\n", pATEInfo->TxLength));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_LENGTH_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF,  (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame count
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_COUNT_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	
	pATEInfo->TxCount = simple_strtol(arg, 0, 10);

#ifdef RTMP_MAC_PCI
	if (pATEInfo->TxCount == 0)
	{
		pATEInfo->TxCount = 0xFFFFFFFF;
	}
#endif /* RTMP_MAC_PCI */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_COUNT_Proc (TxCount = %d)\n", pATEInfo->TxCount));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_COUNT_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame MCS
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_MCS_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR MCS, phy_mode = 0;
	INT result;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		phy_mode = pATEInfo->TxWI.TXWI_N.PHYMODE;
#endif /* RLT_MAC*/

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		phy_mode = pATEInfo->TxWI.TXWI_O.PHYMODE;
#endif /* RTMP_MAC */

	MCS = simple_strtol(arg, 0, 10);
	result = CheckMCSValid(pAd, phy_mode, MCS);

	if (result != -1)
	{
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT)
			pATEInfo->TxWI.TXWI_N.MCS = MCS;
#endif /* RLT_MAC*/

#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP)
			pATEInfo->TxWI.TXWI_O.MCS = MCS;
#endif /* RTMP_MAC */
	}
	else
	{
		DBGPRINT_ERR(("Set_ATE_TX_MCS_Proc::Out of range, refer to rate table.\n"));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_MCS_Proc (MCS = %d)\n", MCS));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_MCS_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame STBC
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_STBC_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR stbc = simple_strtol(arg, 0, 10);

	if (stbc > 1)
	{
		DBGPRINT_ERR(("Set_ATE_TX_STBC_Proc::Out of range\n"));
		return FALSE;
	}
	
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
#ifdef MT7601
		if (IS_MT7601(pAd))
			pATEInfo->TxWI.TXWI_X.STBC = stbc;
		else
#endif /* MT7601 */
			pATEInfo->TxWI.TXWI_N.STBC = stbc;
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		pATEInfo->TxWI.TXWI_O.STBC = stbc;
#endif /* RTMP_MAC */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_STBC_Proc (GI = %d)\n", stbc));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_STBC_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame Mode
        0: MODE_CCK
        1: MODE_OFDM
        2: MODE_HTMIX
        3: MODE_HTGREENFIELD
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_MODE_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR BbpData = 0;
	UCHAR phy_mode, bw = BW_20;
	
	phy_mode = simple_strtol(arg, 0, 10);

	if (phy_mode > MODE_VHT)
	{
		phy_mode = MODE_CCK;
		DBGPRINT_ERR(("Set_ATE_TX_MODE_Proc::Out of range.\nIt should be in range of 0~4\n"));
		DBGPRINT(RT_DEBUG_OFF, ("0: CCK, 1: OFDM, 2: HT_MIX, 3: HT_GREEN_FIELD, 4: VHT.\n"));
		return FALSE;
	}

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
		pATEInfo->TxWI.TXWI_N.PHYMODE = phy_mode;
		bw = pATEInfo->TxWI.TXWI_N.BW;

		if (phy_mode == MODE_CCK)
		{
			pATEInfo->TxWI.TXWI_N.BW = BW_20;
			bw = BW_20;
		}
	}
#endif /* RLT_MAC*/

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		pATEInfo->TxWI.TXWI_O.PHYMODE = phy_mode;
		bw = pATEInfo->TxWI.TXWI_O.BW;

		if (phy_mode == MODE_CCK)
		{
			pATEInfo->TxWI.TXWI_O.BW = BW_20;
			bw = BW_20;
		}
	}
#endif /* RTMP_MAC */

#ifdef RT65xx
	/* Turn on BBP 20MHz mode by request here. */
	if (IS_MT7610(pAd))
	{
		return TRUE;
	}
	else
#endif /* RT65xx */
#if defined(RT5592) || defined(MT7601)
	/* Turn on BBP 20MHz mode by request here. */
	if (IS_RT5592(pAd) || IS_MT7601(pAd))
	{
		if (bw == BW_20)
		{
			BbpData = 0x40;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpData);
		}
		else /* BW == 40MHz */
		{
			BbpData = 0x50;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpData);
		}
	}
	else
#endif /* defined(RT5592) || defined(MT7601) */
	/* Turn on BBP 20MHz mode by request here. */
	if (phy_mode == MODE_CCK)
	{
#ifdef RTMP_BBP
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BbpData);
		BbpData &= (~0x18);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpData);
#endif /* RTMP_BBP */
		DBGPRINT(RT_DEBUG_OFF, ("Set_ATE_TX_MODE_Proc::CCK Only support 20MHZ. Switch to 20MHZ.\n"));
	}
#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
	if (IS_RT5390F(pAd))
	{
		if (phy_mode == MODE_CCK)
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R55, 0x46);
		else			
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R55, 0x43);
	}
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */

#ifdef RT3350
	if (IS_RT3350(pAd))
	{
		if (phy_mode == MODE_CCK)
		{
			USHORT value;
			UCHAR  rf_offset;
			UCHAR  rf_value;

			RT28xx_EEPROM_READ16(pAd, 0x126, value);
			rf_value = value & 0x00FF;
			rf_offset = (value & 0xFF00) >> 8;

			if(rf_offset == 0xff)
			    rf_offset = RF_R21;
			if(rf_value == 0xff)
			    rf_value = 0x4F;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
		
			RT28xx_EEPROM_READ16(pAd, 0x12a, value);
			rf_value = value & 0x00FF;
			rf_offset = (value & 0xFF00) >> 8;

			if(rf_offset == 0xff)
			    rf_offset = RF_R29;
			if(rf_value == 0xff)
			    rf_value = 0x07;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
		

			/* set RF_R24 */
			if (bw == BW_40)
			{    
				value = 0x3F;
			}
			else
			{
				value = 0x1F;
			}
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR)value);
		}
		else
		{
			USHORT value;
			UCHAR  rf_offset;
			UCHAR  rf_value;

			RT28xx_EEPROM_READ16(pAd, 0x124, value);
			rf_value = value & 0x00FF;
			rf_offset = (value & 0xFF00) >> 8;

			if(rf_offset == 0xff)
			    rf_offset = RF_R21;
			if(rf_value == 0xff)
			    rf_value = 0x6F;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
		
			RT28xx_EEPROM_READ16(pAd, 0x128, value);
			rf_value = value & 0x00FF;
			rf_offset = (value & 0xFF00) >> 8;

			if(rf_offset == 0xff)
			    rf_offset = RF_R29;
			if(rf_value == 0xff)
			    rf_value = 0x07;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, rf_offset, (UCHAR)rf_value);
		
			/* set RF_R24 */
			if (bw == BW_40)
			{    
				value = 0x28;
			}
			else
			{
				value = 0x18;
			}
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR)value);
		}
	}
#endif /* RT3350 */

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_MODE_Proc (TxMode = %d)\n", phy_mode));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_MODE_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame GI
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_GI_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR sgi;

	sgi = simple_strtol(arg, 0, 10);
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
		pATEInfo->TxWI.TXWI_N.ShortGI= (sgi > 1 ? 0 : sgi);
	}
#endif /* RLT_MAC*/

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		pATEInfo->TxWI.TXWI_O.ShortGI = (sgi > 1 ? 0 : sgi);
	}
#endif /* RTMP_MAC */

	if (sgi > 1)
	{
		DBGPRINT_ERR(("Set_ATE_TX_GI_Proc::Out of range\n"));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_GI_Proc (GI = %d)\n", sgi));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_GI_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


INT	Set_ATE_RX_FER_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	pATEInfo->bRxFER = simple_strtol(arg, 0, 10);

	if (pATEInfo->bRxFER == 1)
	{
		pATEInfo->RxCntPerSec = 0;
		pATEInfo->RxTotalCnt = 0;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_RX_FER_Proc (bRxFER = %d)\n", pATEInfo->bRxFER));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_RX_FER_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


INT Set_ATE_Read_RF_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef RTMP_RF_RW_SUPPORT
	UCHAR RFValue;
	INT index=0;

	if (IS_RT30xx(pAd) || IS_RT3572(pAd))
	{
#ifdef RTMP_RF
		for (index = 0; index < 32; index++)
		{
			RT30xxReadRFRegister(pAd, index, (PUCHAR)&RFValue);
			DBGPRINT(RT_DEBUG_OFF, ("R%d=%d\n",index,RFValue));
		}
#endif /* RTMP_RF */
	}
	else
#ifdef RT305x
	if (IS_RT305x(pAd))
	{
		for (index = 0; index < 32; index++)
		{
			RT30xxReadRFRegister(pAd, index, (PUCHAR)&RFValue);
			DBGPRINT(RT_DEBUG_OFF, ("R%d=%d\n",index,RFValue));
		}		
	}
	else
#endif /* RT305x */
#endif /* RTMP_RF_RW_SUPPORT */
	{
		DBGPRINT(RT_DEBUG_OFF, ("R1 = %x\n", pAd->LatchRfRegs.R1));
		DBGPRINT(RT_DEBUG_OFF, ("R2 = %x\n", pAd->LatchRfRegs.R2));
		DBGPRINT(RT_DEBUG_OFF, ("R3 = %x\n", pAd->LatchRfRegs.R3));
		DBGPRINT(RT_DEBUG_OFF, ("R4 = %x\n", pAd->LatchRfRegs.R4));
	}
	return TRUE;
}


#if (!defined(RTMP_RF_RW_SUPPORT)) && (!defined(RLT_RF))
INT Set_ATE_Write_RF1_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);	

	pAd->LatchRfRegs.R1 = value;
	RtmpRfIoWrite(pAd);

	return TRUE;
}


INT Set_ATE_Write_RF2_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);

	pAd->LatchRfRegs.R2 = value;
	RtmpRfIoWrite(pAd);

	return TRUE;
}


INT Set_ATE_Write_RF3_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);

	pAd->LatchRfRegs.R3 = value;
	RtmpRfIoWrite(pAd);

	return TRUE;
}


INT Set_ATE_Write_RF4_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);

	pAd->LatchRfRegs.R4 = value;
	RtmpRfIoWrite(pAd);

	return TRUE;
}
#endif /* !defined(RTMP_RF_RW_SUPPORT) && !defined(RLT_RF) */


/* 
==========================================================================
    Description:
        Load and Write EEPROM from a binary file prepared in advance.
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT Set_ATE_Load_E2P_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	BOOLEAN		    	ret = FALSE;
#ifdef RTMP_RBUS_SUPPORT
	RTMP_STRING *src = EEPROM_DEFAULT_FILE_PATH;
#else
	RTMP_STRING *src = EEPROM_BIN_FILE_NAME;
#endif /* RTMP_RBUS_SUPPORT */
	RTMP_OS_FD		srcf;
	INT32 			retval;
	USHORT 			WriteEEPROM[(EEPROM_SIZE >> 1)];
	INT				FileLength = 0;
	UINT32 			value = (UINT32) simple_strtol(arg, 0, 10);
	RTMP_OS_FS_INFO	osFSInfo;

	DBGPRINT(RT_DEBUG_OFF, ("===> %s (value=%d)\n\n", __FUNCTION__, value));

	if (value > 0)
	{
		/* zero the e2p buffer */
		NdisZeroMemory((PUCHAR)WriteEEPROM, EEPROM_SIZE);

		RtmpOSFSInfoChange(&osFSInfo, TRUE);

		do
		{
			/* open the bin file */
			srcf = RtmpOSFileOpen(src, O_RDONLY, 0);

			if (IS_FILE_OPEN_ERR(srcf)) 
			{
				DBGPRINT_ERR(("%s - Error opening file %s\n", __FUNCTION__, src));
				break;
			}

			/* read the firmware from the file *.bin */
			FileLength = RtmpOSFileRead(srcf, (RTMP_STRING *)WriteEEPROM, EEPROM_SIZE);

			if (FileLength != EEPROM_SIZE)
			{
				DBGPRINT_ERR(("%s : error file length (=%d) in e2p.bin\n",
					   __FUNCTION__, FileLength));
				break;
			}
			else
			{
				/* write the content of .bin file to EEPROM */
#if defined(RTMP_MAC_PCI) && defined(RTMP_PCI_SUPPORT)
                {
                    USHORT index=0;
                    USHORT value=0;
                    INT32 e2p_size=512;/* == 0x200 for PCI interface */
                    USHORT tempData=0;
					
                    for (index = 0 ; index < (e2p_size >> 1); )
                    {
                        /* "value" is especially for some compilers... */
                        tempData = le2cpu16(WriteEEPROM[index]);
                        value = tempData;
                        RT28xx_EEPROM_WRITE16(pAd, (index << 1), value);
                        index ++;
                    }
                }
#else
				rt_ee_write_all(pAd, WriteEEPROM);
#endif /* defined(RTMP_MAC_PCI) && defined(RTMP_PCI_SUPPORT) */
				ret = TRUE;
			}
			break;
		} while(TRUE);

		/* close firmware file */
		if (IS_FILE_OPEN_ERR(srcf))
		{
			;
		}
		else
		{
			retval = RtmpOSFileClose(srcf);			

			if (retval)
			{
				DBGPRINT_ERR(("--> Error %d closing %s\n", -retval, src));
				
			} 
		}

		/* restore */
		RtmpOSFSInfoChange(&osFSInfo, FALSE);		
	}

    DBGPRINT(RT_DEBUG_OFF, ("<=== %s (ret=%d)\n", __FUNCTION__, ret));

    return ret;
}


#ifdef RTMP_EFUSE_SUPPORT
/* 
==========================================================================
    Description:
        Load and Write E-Fuse from pAd->EEPROMImage.
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT Set_ATE_Load_E2P_From_Buf_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	BOOLEAN		    	ret = FALSE;
	UINT32 			value = (UINT32) simple_strtol(arg, 0, 10);

	DBGPRINT(RT_DEBUG_OFF, ("===> %s (value=%d)\n\n", __FUNCTION__, value));

	if (value > 0)
	{
#if defined(RTMP_MAC_PCI) && defined(RTMP_PCI_SUPPORT)
            {
                USHORT index=0;
                USHORT value=0;
                INT32 e2p_size=512;/* == 0x200 for PCI interface */
                USHORT tempData=0;
				
                for (index = 0 ; index < (e2p_size >> 1); )
                {
                    /* "value" is especially for some compilers... */
		        tempData = le2cpu16(pAd->EEPROMImage[index]);
                    value = tempData;
                    RT28xx_EEPROM_WRITE16(pAd, (index << 1), value);
                    index ++;
                }
            }
#else
		rt_ee_write_all(pAd, pAd->EEPROMImage);
#endif /* defined(RTMP_MAC_PCI) && defined(RTMP_PCI_SUPPORT) */
		ret = TRUE;
	
	}

    DBGPRINT(RT_DEBUG_OFF, ("<=== %s (ret=%d)\n", __FUNCTION__, ret));

    return ret;
}
#endif /* RTMP_EFUSE_SUPPORT */


INT Set_ATE_Read_E2P_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	USHORT buffer[EEPROM_SIZE >> 1];
	USHORT *p;
	int i;
	
	rt_ee_read_all(pAd, (USHORT *)buffer);
	p = buffer;
	for (i = 0; i < (EEPROM_SIZE >> 1); i++)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%4.4x ", *p));
		if (((i+1) % 16) == 0)
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		p++;
	}
	return TRUE;
}


#ifdef LED_CONTROL_SUPPORT
#ifdef RT35xx
/*
	========================================================================
	
	Routine Description:
		Set LED Signal Stregth, dedicated for ATE.
 

	Arguments:
		pAd						Pointer to our adapter
		Dbm						Signal Stregth

	Return Value:
		None

	IRQL = PASSIVE_LEVEL
	
	Note:
		Can be run on any IRQL level. 

		According to Microsoft Zero Config Wireless Signal Stregth definition as belows.
        <= 0   No Signal
		<= 1   Very Low
		<= 3   Low
		<= 7   Good
		<= 15  Very Good
		 > 15  Excellent		
	========================================================================
*/
INT Set_ATE_LED_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR nLed = simple_strtol(arg, 0, 10);

	if (!IS_RT3572(pAd))
		return FALSE;

	if (nLed <= 0)
		nLed = 0;
	else if (nLed <= 1 )
		nLed = 1;
	else if (nLed <= 3)
		nLed = 3;
	else if (nLed <= 7)
		nLed = 7;
	else if (nLed <= 15)
		nLed = 15;
	else 
		nLed = 31;

	/* always update signal strength to firmware */
	if (LED_MODE(pAd) == LED_MODE_SIGNAL_STREGTH)
	{
		AsicSendCommandToMcu(pAd, MCU_SET_LED_GPIO_SIGNAL_CFG, 0xff, nLed, pAd->LedCntl.MCULedCntl.field.Polarity, FALSE);
		pATEInfo->LedIndicatorStrength = nLed;
	}
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_LED_Proc :: %d\n", pATEInfo->LedIndicatorStrength));

	return TRUE;
}


INT Set_ATE_HW_LED_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	if (IS_RT3572(pAd))
	{
		RTMPSetLED(pAd, LED_HW_CONTROL);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
#endif /* RT35xx */
#endif /* LED_CONTROL_SUPPORT */


/* 
==========================================================================
    Description:
        Enable ATE auto Tx alc (Tx auto level control).
        According to the chip temperature, auto adjust the transmit power.  
        
        0: disable
        1: enable
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_AUTO_ALC_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32 value = simple_strtol(arg, 0, 10);

	if (value > 0)
	{
#ifdef MT76x0_TSSI_CAL_COMPENSATION
		if (IS_MT76x0(pAd))
		{
			MT76x0ATE_TSSI_DC_Calibration(pAd);
			MT76x0ATE_Enable9BitIchannelADC(pAd, TRUE);
		}
#endif /* MT76x0_TSSI_CAL_COMPENSATION */
#ifdef RTMP_INTERNAL_TX_ALC
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
#ifdef RT6352_EP_SUPPORT
			if (pAd->bExtPA == FALSE)
			{
#endif /* RT6352_EP_SUPPORT */
			OS_SEM_LOCK(&(pATEInfo->TssiSemLock));
			RT635xTssiDcCalibration(pAd);
			OS_SEM_UNLOCK(&(pATEInfo->TssiSemLock));	
#ifdef RT6352_EP_SUPPORT
			}
#endif /* RT6352_EP_SUPPORT */
		}
#endif /* RT6352 */
#endif /* RTMP_INTERNAL_TX_ALC */
		pATEInfo->bAutoTxAlc = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATEAUTOALC = TRUE , auto alc enabled!\n"));
	}
	else
	{
		pATEInfo->bAutoTxAlc = FALSE;

		if (IS_RT6352(pAd) || IS_MT76x0(pAd))
		{
			UINT32 MacValue;
#if defined(RTMP_MAC) || defined(RLT_MAC)
			/* clean up MAC 0x13B4 */
			RTMP_IO_READ32(pAd, TX_ALC_CFG_1, &MacValue);	
			MacValue = MacValue & (~0x3f);
			RTMP_IO_WRITE32(pAd, TX_ALC_CFG_1, MacValue);
#else
			//TODO: MT_MAC check
#endif
		}
		DBGPRINT(RT_DEBUG_TRACE, ("ATEAUTOALC = FALSE , auto alc disabled!\n"));
	}	

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}


/* 
==========================================================================
    Description:
        Enable Tx temperature sensor.
        
        0: disable
        1: enable
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TEMP_SENSOR_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	BOOLEAN value = simple_strtol(arg, 0, 10);

	if (value > 0)
	{
		pATEInfo->bLowTemperature = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATETEMPSENSOR = TRUE , temperature sensor enabled!\n"));
	}
	else
	{
		pATEInfo->bLowTemperature = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATETEMPSENSOR = FALSE , temperature sensor disabled!\n"));
	}	

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}


#ifdef VCORECAL_SUPPORT
/* 
==========================================================================
    Description:
        Enable ATE auto VCO periodic calibration.
        
        0: disable
        1: enable
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_VCO_CAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value = simple_strtol(arg, 0, 10);

	if (value > 0)
	{
		pAd->ate.bAutoVcoCal = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATEVCOCAL = TRUE , auto vcoc enabled!\n"));
	}
	else
	{
		pAd->ate.bAutoVcoCal = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATEVCOCAL = FALSE , auto vcoc disabled!\n"));
	}	

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}
#endif /* VCORECAL_SUPPORT */


#ifdef TXBF_SUPPORT
/* 
==========================================================================
    Description:
        Set ATE Tx Beamforming mode
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TXBF_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg))
{
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR value;
	
	value = simple_strtol(arg, 0, 10);

	switch (value)
	{
#ifdef RLT_MAC
#ifndef MT76x2
		case 0:
			/* no BF */
			pATEInfo->TxWI.TXWI_N.iTxBF = pATEInfo->TxWI.TXWI_N.eTxBF = 0;
			break;
		case 1:
			/* ETxBF */
			pATEInfo->TxWI.TXWI_N.eTxBF = 1;
			break;
		case 2:
			/* ITxBF */
			pATEInfo->TxWI.TXWI_N.iTxBF = 1;
			break;
		case 3:
			/* Enable TXBF support */
			pATEInfo->bTxBF = TRUE;
			break;
#else
		case 0:
			/* no BF */
			pATEInfo->bTxBF = FALSE;
			pATEInfo->TxWI.TXWI_N.iTxBF = 0;
			pATEInfo->TxWI.TXWI_N.eTxBF = 0;
			break;
		case 1:
			/* ETxBF */
			pATEInfo->bTxBF = TRUE;
			pATEInfo->TxWI.TXWI_N.eTxBF = 1;
			pATEInfo->TxWI.TXWI_N.iTxBF = 0;		
			break;
		case 2:
			/* ITxBF */
			pATEInfo->bTxBF = TRUE;
			pATEInfo->TxWI.TXWI_N.iTxBF = 1;
			pATEInfo->TxWI.TXWI_N.eTxBF = 0;			
			break;
		case 3:
			/* Enable TXBF support */
			pATEInfo->bTxBF = TRUE;
			pATEInfo->TxWI.TXWI_N.eTxBF = 1;
			pATEInfo->TxWI.TXWI_N.iTxBF = 1;			
			break;
#endif /* MT76x2 */
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		case 0:
			/* no BF */
			pATEInfo->TxWI.TXWI_O.iTxBF = pATEInfo->TxWI.TXWI_N.eTxBF = 0;
			break;
		case 1:
			/* ETxBF */
			pATEInfo->TxWI.TXWI_O.eTxBF = 1;
			break;
		case 2:
			/* ITxBF */
			pATEInfo->TxWI.TXWI_O.iTxBF = 1;
			break;
		case 3:
			/* Enable TXBF support */
		pATEInfo->bTxBF = TRUE;
			break;
#endif /* RTMP_MAC */
		case 4:
			/* Disable TXBF support */
			pATEInfo->bTxBF = FALSE;
			break;
		default:
			DBGPRINT_ERR(("Set_ATE_TXBF_Proc: Invalid parameter %d\n", value));
			break;
	}

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
	}


/* 
==========================================================================
    Description:
        Set ATE Sounding type
			0 => no sounding
			1 => Data sounding
			2 => 2 stream NDP sounding
			3 => 3 stream NDP Sounding
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TXSOUNDING_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	CHAR value;
	
	value = simple_strtol(arg, 0, 10);

	if (value<0 || value>3)
	{
		DBGPRINT_ERR(("Set_ATE_TXSOUNDING_Proc: Invalid parameter %d\n", value));
		return FALSE;
	}	

	pATEInfo->txSoundingMode = value;

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}


/* 
==========================================================================
    Description:
        Do a Divider Calibration on calibration channels and save in EEPROM
			0 => do G and A band
			1 => G band only
			2 => A band only

    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TXBF_DIVCAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	int value;
	ITXBF_DIV_PARAMS divParams;
	CHAR initChanArg[] = "0";

	value = simple_strtol(arg, 0, 10);

	if (value<0 || value>2)
		return FALSE;

	/* G band */
	if (value==0 || value==1)
	{
		pATEInfo->Channel = 1;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 14;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		/* Display delta phase information */
		ITxBFGetEEPROM(pAd, NULL, NULL, &divParams);

#ifndef MT76x2
		DBGPRINT(RT_DEBUG_WARN, ("Divider Cal Done:\n"
						"ch1-ch14 = [%2d, %2d] degrees\n"
						"ant0-ant2 = [%2d, %2d] degrees\n",
				(UCHAR)(divParams.gBeg[0]-divParams.gEnd[0])*360/256,
				(UCHAR)(divParams.gBeg[1]-divParams.gEnd[1])*360/256,
				(UCHAR)(divParams.gBeg[0]-divParams.gBeg[1])*360/256,
				(UCHAR)(divParams.gEnd[0]-divParams.gEnd[1])*360/256) );
#endif
	}

	/* A Band */
	if (value==0 || value==2)
	{
		pATEInfo->Channel = 36;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 120;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

#ifndef MT76x2	
		pATEInfo->Channel = 165;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);
#else

		pATEInfo->Channel = 64;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 100;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 120;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 144;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 149;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);

		pATEInfo->Channel = 173;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfDividerCalibration(pAd, 1, 0, NULL);
#endif
	}

	return TRUE;
}


/*
==========================================================================
    Description:
        Do a LNA Calibration on calibration channels and save in EEPROM
			0 => do G and A band
			1 => G band only
			2 => A band only

    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TXBF_LNACAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	int value;
	int i;
	CHAR initChanArg[] = "0";
	CHAR fMethod;

	value = simple_strtol(arg, 0, 10);

	if (value<0 || value>2)
		return FALSE;

	fMethod = 0;
#ifdef MT76x2
	fMethod = 5;
#endif

	/* G Band */
	if (value==0 || value==1)
	{
		pATEInfo->Channel = 1;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, TRUE);

		pATEInfo->Channel = 14;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, TRUE);
	}

	/* A Band */
	if (value==0 || value==2)
	{
#ifndef MT76x2	
		static UCHAR channels[6] = {36, 64, 100, 128, 132, 165};
		for (i=0; i<6; i++)
		{
			pATEInfo->Channel = channels[i];
			Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
			pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);
		}
#else
		pATEInfo->Channel = 36;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);

		pATEInfo->Channel = 64;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);

		pATEInfo->Channel = 100;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);

		pATEInfo->Channel = 120;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);

		pATEInfo->Channel = 140;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);

		pATEInfo->Channel = 149;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);

		pATEInfo->Channel = 173;
		Set_ATE_INIT_CHAN_Proc(pAd, initChanArg);
		pAd->chipOps.fITxBfLNACalibration(pAd, 1, fMethod, FALSE);				
#endif
	}

	if (pAd->chipCap.FlgITxBfBinWrite)
	{
		// Wite the calibrated phase into bit file
		set_BinModeWriteBack_Proc(pAd, "1");
	}

	return TRUE;
}


/* 
==========================================================================
    Description:
	Sanity check for the channel of Implicit TxBF calibration.
        	
    Return:
	TRUE if all parameters are OK, FALSE otherwise

    Note: 
	1. This sanity check function only work for Implicit TxBF calibration.
	2. Currently supported channels are:
        	1, 14, 36, 64, 128, 132, 165               for 11n
        	1, 14, 36, 64, 100, 120, 140, 149, 173 for 11ac
==========================================================================
*/
static BOOLEAN rtmp_ate_txbf_cal_valid_ch(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR channel)
{
	BOOLEAN bValidCh;

	/* TODO: shall we check the capability of the chipset here ?? */
	switch (channel)
	{
#ifdef MT76x2
		case 0:
			bValidCh = TRUE;
			break;
#endif
		case 1:
		case 14:
#ifdef A_BAND_SUPPORT
#ifndef MT76x2	
		case 36:
		case 64:
		case 100:
		case 128:
		case 132:
		case 165:
#else
		case 36:
		case 64:
		case 100:
		case 120:
		case 140:
		case 149:
		case 173:
#endif
#endif /* A_BAND_SUPPORT */

			bValidCh = TRUE;
			break;
		default:
			bValidCh = FALSE;
			break;
	}

	return bValidCh;	
}


/* 
==========================================================================
    Description:
        Set to start the initialization procedures of iTxBf calibration in DUT side
			0 => do nothing
			1 => do following initializations
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise

    Note: 
   	This cmd shall only used in DUT side for calibration
==========================================================================
*/
INT Set_ATE_TXBF_INIT_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	int val;
	USHORT eepromVal;
	UCHAR cmdStr[32];
	
	val = simple_strtol(arg, 0, 10);
	if (val != 1)
		return FALSE;

	/* Do ATESTART */
#ifdef CONFIG_RT2880_ATE_CMD_NEW
	Set_ATE_Proc(pAd, "ATESTART");
#else
	Set_ATE_Proc(pAd, "APSTOP");
#endif /* CONFIG_RT2880_ATE_CMD_NEW */

	/* set ATETXBF=3 */
	Set_ATE_TXBF_Proc(pAd, "3");


	/* Set self mac address as 22:22:22:22:22:22 */
	RTMP_IO_WRITE32(pAd, MAC_ADDR_DW0, 0x22222222);
	RTMP_IO_WRITE32(pAd, MAC_ADDR_DW1, 0x00002222);

	/* set ATEDA=11:11:11:11:11:11 */
	/* set ATESA=22:22:22:22:22:22 */
	/* set ATEBSSID=22:22:22:22:22:22 */
	for (val = 0; val < MAC_ADDR_LEN; val++)
	{
		pATEInfo->Addr1[val] = 0x11; /* the RA */
		pATEInfo->Addr2[val] = 0x22; /* the TA */
		pATEInfo->Addr3[val] = 0x22;	/* the BSSID */
	}

	/* set ATETXMODE=2 */
	Set_ATE_TX_MODE_Proc(pAd, "2");
	
	/* set ATETXMCS=16 */
	Set_ATE_TX_MCS_Proc(pAd, "16");
	
	/* set ATETXBW=0 */
	Set_ATE_TX_BW_Proc(pAd, "0");
	
	/* set ATETXGI=0 */
	Set_ATE_TX_GI_Proc(pAd, "0");
	
	/* set ATETXANT=0 */
	Set_ATE_TX_Antenna_Proc(pAd, "0");
	
	/* set ATERXANT=0 */
	Set_ATE_RX_Antenna_Proc(pAd, "0");
#ifndef MT76x2	
	/* set ATETXFREQOFFSET=eeprom */
	/* read EEPROM Frequency offset from EEPROM and set it to BBP */
	RT28xx_EEPROM_READ16(pAd, 0x44, eepromVal);
	snprintf(cmdStr, sizeof(cmdStr), "%d\n", (eepromVal & 0xff));
	Set_ATE_TX_FREQ_OFFSET_Proc(pAd, cmdStr);

#ifdef RTMP_BBP
	/* bbp 65=29 */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x29);
	
	/* bbp 163=bd */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R163, 0xbd);
	
	/* bbp 173=28 */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, 0x28);
#endif /* RTMP_BBP */
#else
	RTMP_IO_WRITE32(pAd, TXBE_R12, 0x00000028);
#endif /* MT76x2 */

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set to do iTxBF calibration procedures for specific channel, following show us the supported channels.
        	1, 14, 36, 64, 128, 132, 165                in 11n
        	1, 14, 36, 64, 100, 120, 140, 149, 173  in 11ac

    Return:
        TRUE if all parameters are OK, FALSE otherwise

    Note: 
   	This cmd shall only used in DUT side for calibration
==========================================================================
*/
INT Set_ATE_TXBF_CAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ch;
	UCHAR cmdStr[32];
#ifdef MT76x2
	UINT CR_BK[35], value32;
#endif

	ch = simple_strtol(arg, 0, 10);
	if (rtmp_ate_txbf_cal_valid_ch(pAd, ch) == FALSE)
		return FALSE;

	/* iwpriv ra0 set ATECHANNEL=Channel */
	snprintf(cmdStr, sizeof(cmdStr), "%d\n", ch);
	if (Set_ATE_CHANNEL_Proc(pAd, cmdStr) == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATEINITCHAN =0 */
	if (Set_ATE_INIT_CHAN_Proc(pAd, "0") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXSOUNDING=3 */
#ifndef MT76x2	
	if (Set_ATE_TXSOUNDING_Proc(pAd, "3") == FALSE)
#else
	if (Set_ATE_TXSOUNDING_Proc(pAd, "2") == FALSE)
#endif
		return FALSE;
	
	/* iwpriv ra0 set ETxBfNoncompress=0 */
	if (Set_ETxBfNoncompress_Proc(pAd, "0") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXMCS=0 */
	if (Set_ATE_TX_MCS_Proc(pAd, "0") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXCNT=1 */
	if (Set_ATE_TX_COUNT_Proc(pAd, "1") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXLEN=258 */
	if (Set_ATE_TX_LENGTH_Proc(pAd, "258") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set InvTxBfTag=0 */
	if (Set_InvTxBfTag_Proc(pAd, "0") == FALSE)
		return FALSE;
	
#ifndef MT76x2	
	/* Disable TX Phase Compensation */
	RTMP_IO_READ32(pAd, TXBE_R12, &value32);
	RTMP_IO_WRITE32(pAd, TXBE_R12, value32 & (~0x28));
#endif	
	
	/* iwpriv ra0 set ATE=TXFRAME */
	if (Set_ATE_Proc(pAd, "TXFRAME") == FALSE)
		return FALSE;
	
#ifdef MT76x2
	RtmpOsMsDelay(100); // waiting 100ms for making sure TxBf profiles being calculated
#endif	
	
	if (pAd->chipOps.fITxBfCal(pAd, "1") == FALSE) 
		return FALSE;

	if (pAd->chipCap.FlgITxBfBinWrite)
	{
		// Wite the calibrated phase into bit file
		set_BinModeWriteBack_Proc(pAd, "1");
	}

	return TRUE;

	
}

#ifdef MT76x2
/* 
==========================================================================
    Description:
        Set to do iTxBF calibration procedures for specific channel, following show us the supported channels.
        	1, 14, 36, 64, 128, 132, 165                in 11n
        	1, 14, 36, 64, 100, 120, 140, 149, 173  in 11ac

    Return:
        TRUE if all parameters are OK, FALSE otherwise

    Note: 
   	This cmd shall only used in DUT side for calibration
==========================================================================
*/
UCHAR CHTbl[9]={1, 14, 36, 64, 100, 120, 140, 149, 173};

INT Set_ATE_TXBF_New_CAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ch, calLoop, i;
	UCHAR cmdStr[32];
	UINT  value32;
	BOOLEAN allChflg = FALSE;
	PATE_INFO pATEInfo = &(pAd->ate);
	
	ch = simple_strtol(arg, 0, 10);
	if (rtmp_ate_txbf_cal_valid_ch(pAd, ch) == FALSE)
		return FALSE;

	calLoop = 1;
	if (ch == 0) 
	{
		allChflg = TRUE;
		calLoop = sizeof(CHTbl);
	}	

	for (i = 0; i < calLoop; i++)
	{
		if (allChflg) ch = CHTbl[i];
		
		/* iwpriv ra0 set ATECHANNEL=Channel */
		snprintf(cmdStr, sizeof(cmdStr), "%d\n", ch);
		if (Set_ATE_CHANNEL_Proc(pAd, cmdStr) == FALSE)
			return FALSE;
	
		/* iwpriv ra0 set ATEINITCHAN =0 */
		if (Set_ATE_INIT_CHAN_Proc(pAd, "0") == FALSE)
			return FALSE;

		/* Disable RX Phase Compensation */
		RTMP_IO_READ32(pAd, TXBE_R12, &value32);
		RTMP_IO_WRITE32(pAd, TXBE_R12, value32 & (~0x28));

		// Residual phase calculation
		ITxBFPhaseCalibrationStartUp(pAd, 1);
	}

	if (pAd->chipCap.FlgITxBfBinWrite)
	{
		// Wite the calibrated phase into bit file
		set_BinModeWriteBack_Proc(pAd, "1");
	}
	
	return TRUE;
	
}
#endif


/* 
==========================================================================
    Description:
        Set to start the initialization procedures of iTxBf calibration in Golden side at specified channel
			arg => valid values are :
									1, 14, 36, 64, 128, 132, 165                in 11n
        							1, 14, 36, 64, 100, 120, 140, 149, 173		in 11ac
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise

    Note: 
   	This cmd shall only used in GOLDEN side for calibration feedback
==========================================================================
*/
INT Set_ATE_TXBF_GOLDEN_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ch;
	UCHAR cmdStr[32];
	USHORT eepromVal;
	
	ch = simple_strtol(arg, 0, 10);
	if (rtmp_ate_txbf_cal_valid_ch(pAd, ch) == FALSE)
		return FALSE;	

	/* iwpriv ra0 set ATE=ATESTART */
#ifdef	CONFIG_RT2880_ATE_CMD_NEW
	Set_ATE_Proc(pAd, "ATESTART");
#else
	Set_ATE_Proc(pAd, "APSTOP");
#endif // CONFIG_RT2880_ATE_CMD_NEW //

	/* set the ate channel and read txpower from EEPROM and set to bbp */
	/* iwpriv ra0 set ATECHANNEL=Channel */
	/* iwpriv ra0 set ATETXPOWER=0 */
	snprintf(cmdStr, sizeof(cmdStr), "%d\n", ch);
	Set_ATE_INIT_CHAN_Proc(pAd, cmdStr);
	

	/* Set self mac address as 11:11:11:11:11:11 */
	/* iwpriv ra0 set ATESA=11:11:11:11:11:11 */
	RTMP_IO_WRITE32(pAd, 0x1008, 0x11111111);
	RTMP_IO_WRITE32(pAd, 0x100c, 0x00001111);
	
	/* iwpriv ra0 set ATETXMODE=2 */
	Set_ATE_TX_MODE_Proc(pAd, "2");
	
	/* iwpriv ra0 set ATETXBW=0 */
	Set_ATE_TX_BW_Proc(pAd, "0");
	
	/* iwpriv ra0 set ATETXGI=0 */
	Set_ATE_TX_GI_Proc(pAd, "0");
	
	/* iwpriv ra0 set ATETXANT=1 */
	Set_ATE_TX_Antenna_Proc(pAd, "1");
	
	/* iwpriv ra0 set ATERXANT=1 */
	Set_ATE_RX_Antenna_Proc(pAd, "1");

	/* iwpriv ra0 set ATETXFREQOFFSET=ValueOfEEPROM */
	RT28xx_EEPROM_READ16(pAd, 0x44, eepromVal);
	snprintf(cmdStr, sizeof(cmdStr), "%d\n", (eepromVal & 0xff));
	Set_ATE_TX_FREQ_OFFSET_Proc(pAd, cmdStr);

#ifdef RTMP_BBP
	/* iwpriv ra0 bbp 65=29 */
	/* iwpriv ra0 bbp 163=9d */
	/* iwpriv ra0 bbp 173=00 */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x29);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R163, 0x9d);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, 0x00);
#endif /* RTMP_BBP */

	/* iwpriv ra0 set ATE=RXFRAME */
	Set_ATE_Proc(pAd, "RXFRAME");

#ifdef RTMP_BBP	
	/* reset the BBP_R173 as 0 to eliminate the compensation */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, 0x00);
#endif /* RTMP_BBP */

	return TRUE;

}


/* 
==========================================================================
    Description:
	Set to do iTxBF calibration verification procedures at sepcified channel, following show us the supported channels.
		arg => valid values are :
								1, 14, 36, 64, 128, 132, 165                in 11n
        							1, 14, 36, 64, 100, 120, 140, 149, 173  in 11ac

    Return:
	TRUE if all parameters are OK, FALSE otherwise

    Note: 
	This cmd shall only used in GOLDEN side for calibration verification
==========================================================================
*/
INT Set_ATE_TXBF_VERIFY_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ch;
	UCHAR cmdStr[32];
	
	ch = simple_strtol(arg, 0, 10);
	if (rtmp_ate_txbf_cal_valid_ch(pAd, ch) == FALSE)
		return FALSE;

	/* iwpriv ra0 set ATECHANNEL=Channel */
	snprintf(cmdStr, sizeof(cmdStr), "%d\n", ch);
	if (Set_ATE_CHANNEL_Proc(pAd, cmdStr) == FALSE)
		return FALSE;
	
#ifndef MT76x2	
	/* iwpriv ra0 set ATETXSOUNDING=3 */
	if (Set_ATE_TXSOUNDING_Proc(pAd, "3") == FALSE)
#else
	/* iwpriv ra0 set ATETXSOUNDING=2*/
	if (Set_ATE_TXSOUNDING_Proc(pAd, "2") == FALSE)
#endif
		return FALSE;
	
	/* iwpriv ra0 set ETxBfNoncompress=0 */
	if (Set_ETxBfNoncompress_Proc(pAd, "0") == FALSE)
		return FALSE;

	/* iwpriv ra0 set ATETXMCS=0 */	
	if (Set_ATE_TX_MCS_Proc(pAd, "0") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXCNT=1 */
	if (Set_ATE_TX_COUNT_Proc(pAd, "1") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXLEN=258 */
	if (Set_ATE_TX_LENGTH_Proc(pAd, "258") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set InvTxBfTag=0 */
	if (Set_InvTxBfTag_Proc(pAd, "0") == FALSE)
		return FALSE;

	/* iwpriv ra0 set ATE=TXFRAME */
	if (Set_ATE_Proc(pAd, "TXFRAME") == FALSE)
		return FALSE;
	
#ifdef MT76x2
	RtmpOsMsDelay(100); // waiting 100ms for making sure TxBf profiles being calculated
#endif	

	/* iwpriv ra0 set ITxBfCal=0 */
	return pAd->chipOps.fITxBfCal(pAd, "0");
}


INT Set_ATE_ForceBBP_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR bbpReg;

	bbpReg = simple_strtol(arg, 0, 10);

	/*
		0: no any restriction for BBP writing
		1~255: force to not allow to change this specific BBP register.
		
		Note: 
			BBP_R0 is not write-able, so use 0 as the rest operation shall be safe enough
	*/
	pATEInfo->forceBBPReg = bbpReg;
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_ForceBBP_Proc:(forceBBPReg value=%d)\n", pATEInfo->forceBBPReg));

	return TRUE;
}


/* 
==========================================================================
    Description:
	Set to do iTxBF calibration verification without R173 compensation procedures at sepcified channel, following show us the supported channels.
		arg => valid values are :
								1, 14, 36, 64, 128, 132, 165                in 11n
        							1, 14, 36, 64, 100, 120, 140, 149, 173  in 11ac

    Return:
	TRUE if all parameters are OK, FALSE otherwise

    Note: 
	This cmd shall only used in GOLDEN side for calibration verification
==========================================================================
*/
INT Set_ATE_TXBF_VERIFY_NoComp_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ch;
	UCHAR cmdStr[32];
	UCHAR bbpR173 = 0;
	int retval;
	
	ch = simple_strtol(arg, 0, 10);
	if (rtmp_ate_txbf_cal_valid_ch(pAd, ch) == FALSE)
		return FALSE;

	/* iwpriv ra0 set ATECHANNEL=Channel */
	snprintf(cmdStr, sizeof(cmdStr), "%d\n", ch);
	if (Set_ATE_CHANNEL_Proc(pAd, cmdStr) == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXSOUNDING=3 */
	if (Set_ATE_TXSOUNDING_Proc(pAd, "3") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ETxBfNoncompress=0 */
	if (Set_ETxBfNoncompress_Proc(pAd, "0") == FALSE)
		return FALSE;

	/* iwpriv ra0 set ATETXMCS=0 */	
	if (Set_ATE_TX_MCS_Proc(pAd, "0") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXCNT=1 */
	if (Set_ATE_TX_COUNT_Proc(pAd, "1") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set ATETXLEN=258 */
	if (Set_ATE_TX_LENGTH_Proc(pAd, "258") == FALSE)
		return FALSE;
	
	/* iwpriv ra0 set InvTxBfTag=0 */
	if (Set_InvTxBfTag_Proc(pAd, "0") == FALSE)
		return FALSE;

#ifdef RTMP_BBP
	/* save current BBP_R173 value and reset it as 0 */
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R173, &bbpR173);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, 0);
#endif /* RTMP_BBP */

	/* force BBP_R173 value when do following procedures. */
	Set_ATE_ForceBBP_Proc(pAd, "173");
	
	/* iwpriv ra0 set ATE=TXFRAME */
	if (Set_ATE_Proc(pAd, "TXFRAME") == FALSE)
	{
		Set_ATE_ForceBBP_Proc(pAd, "0");
		return FALSE;
	}

	/* enable the update of BBP_R173 */
	Set_ATE_ForceBBP_Proc(pAd, "0");
	
	/* iwpriv ra0 set ITxBfCal=0 */
	retval = pAd->chipOps.fITxBfCal(pAd, "0");

#ifdef RTMP_BBP
	/* recovery the BBP_173 to original value */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R173, bbpR173);
#endif /* RTMP_BBP */

	/* done and return */
	return retval;
	
}
#endif /* TXBF_SUPPORT */	

#ifdef MT76x2
#ifdef TXBF_SUPPORT
/* 
==========================================================================
    Description:
	Set to do iTxBF calibration verification procedures at sepcified channel, following show us the supported channels.
		arg => valid values are :
								1, 14, 36, 64, 128, 132, 165                in 11n
        							1, 14, 36, 64, 100, 120, 140, 149, 173  in 11ac

    Return:
	TRUE if all parameters are OK, FALSE otherwise

    Note: 
	This cmd shall only used in GOLDEN side for calibration verification
==========================================================================
*/

INT Set_ATE_New_Phase_Verify(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR ch, calLoop, i;
	BOOLEAN allChflg=FALSE;
	UCHAR cmdStr[32];
	UINT  CR_BK[35], value32;
	ITXBF_PHASE_PARAMS phaseParams;
	ITXBF_DIV_PARAMS divParams;
	UCHAR phaseValues[2], divValue[2];
	PATE_INFO pATEInfo = &(pAd->ate);
	
	ch = simple_strtol(arg, 0, 10);
	if (rtmp_ate_txbf_cal_valid_ch(pAd, ch) == FALSE)
		return FALSE;

	calLoop = 1;
	if (ch == 0) 
	{
		allChflg = TRUE;
		calLoop = sizeof(CHTbl);
	}	

	for (i = 0; i < calLoop; i++)
	{
		if (allChflg) ch = CHTbl[i];

		/* iwpriv ra0 set ATECHANNEL=Channel */
		snprintf(cmdStr, sizeof(cmdStr), "%d\n", ch);
		if (Set_ATE_CHANNEL_Proc(pAd, cmdStr) == FALSE)
			return FALSE;
	
		/* iwpriv ra0 set ATEINITCHAN =0 */
		if (Set_ATE_INIT_CHAN_Proc(pAd, "0") == FALSE)
			return FALSE;

		/* Disable RX Phase Compensation */
		RTMP_IO_READ32(pAd, TXBE_R12, &value32);
		RTMP_IO_WRITE32(pAd, TXBE_R12, value32 & (~0x28));

		// Divider caliulation
		pAd->chipOps.fITxBfDividerCalibration(pAd, 2, 0, NULL);

		// LNA compensation
		//mt_rf_write(pAd, RF_Path0,	   RFDIGI_TRX4, 0x28585);
		//mt_rf_write(pAd, RF_Path1,	   RFDIGI_TRX4, 0x28585);
		pAd->chipOps.fITxBfLNAPhaseCompensate(pAd);

		RtmpOsMsDelay(10); // waiting 10ms
		
		ITxBFPhaseCalibrationStartUp(pAd, 0);
	}
	
	return TRUE;
}
#endif /* TXBF_SUPPORT */
#endif /* MT76x2 */



/* 
==========================================================================
    Description:
        Set ATE Tx frame IPG
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_IPG_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32           data, value;

	pATEInfo->IPG = simple_strtol(arg, 0, 10);
	value = pATEInfo->IPG;


#if defined(RTMP_MAC) || defined(RLT_MAC)
	RTMP_IO_READ32(pAd, XIFS_TIME_CFG, &data);

	if (value <= 0)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Set_ATE_IPG_Proc::IPG is disabled(IPG == 0).\n"));
		return TRUE;
	}

	ASSERT(value > 0);

	if ((value > 0) && (value < 256))
	{               
	    RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &data);
	    data &= 0x0;
	    RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, data);

	    RTMP_IO_READ32(pAd, EDCA_AC1_CFG, &data);
	    data &= 0x0;
	    RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, data);

	    RTMP_IO_READ32(pAd, EDCA_AC2_CFG, &data);
	    data &= 0x0;
	    RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, data);

	    RTMP_IO_READ32(pAd, EDCA_AC3_CFG, &data);
	    data &= 0x0;
	    RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, data);
	}
	else
	{
	    UINT32 aifsn, slottime;

	    RTMP_IO_READ32(pAd, BKOFF_SLOT_CFG, &slottime);
	    slottime &= 0x000000FF;

	    aifsn = value / slottime;                  
	    value = value % slottime;

	    RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &data);
	    data &= 0x0;
	    data |= (aifsn << 8);
	    RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, data);

	    RTMP_IO_READ32(pAd, EDCA_AC1_CFG, &data);
	    data &= 0x0;
	    data |= (aifsn << 8);
	    RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, data);

	    RTMP_IO_READ32(pAd, EDCA_AC2_CFG, &data);
	    data &= 0x0;
	    data |= (aifsn << 8);
	    RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, data);

	    RTMP_IO_READ32(pAd, EDCA_AC3_CFG, &data);
	    data &= 0x0;
	    data |= (aifsn << 8);
	    RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, data);
	}

	data = (value & 0xFFFF0000) | value | (value << 8);
	RTMP_IO_WRITE32(pAd, XIFS_TIME_CFG, data);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_IPG_Proc (IPG = %u)\n", pATEInfo->IPG));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_IPG_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

#else
	//TODO: MT_MAC check
#endif

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE payload pattern for TxFrame
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_Payload_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *value;

	value = arg;

	/* only one octet acceptable */	
	if (strlen(value) != 2)  
		return FALSE;

	AtoH(value, &(pATEInfo->Payload), 1);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_Payload_Proc (repeated pattern = 0x%2x)\n", pATEInfo->Payload));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_Payload_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE fixed/random payload pattern for TxFrame
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_Fixed_Payload_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *value;

	value = arg;

	/* only one octet acceptable */	
	if (strlen(value) != 2)  
		return FALSE;

	value = simple_strtol(arg, 0, 10);

	if ( value == 0 )
		pATEInfo->bFixedPayload = FALSE;
	else
		pATEInfo->bFixedPayload = TRUE;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_Fixed_Payload_Proc (Fixed Payload  = %d)\n", pATEInfo->bFixedPayload));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_Fixed_Payload_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}


#ifdef IQ_CAL_SUPPORT
/* 
==========================================================================
    Description:
        Enable ATE I/Q gain/phase compensation.
        
        0: disable(default)
        1: enable
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_IQ_Compensation_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UINT32 value = simple_strtol(arg, 0, 10);

	if (value > 0)
	{
		pATEInfo->bIQCompEnable = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("bIQCompEnable = TRUE , I/Q compensation enabled!\n"));
	}
	else
	{
		pATEInfo->bIQCompEnable = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("bIQCompEnable = FALSE , I/Q compensation disabled!\n"));
	}	

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}
#endif /* IQ_CAL_SUPPORT */


/* 
==========================================================================
    Description:
        Enable ATE test time reduction.
        
        0: disable
        1: enable
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TTR_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT32 value = simple_strtol(arg, 0, 10);

	if (value > 0)
	{
		pAd->ate.bTestTimeReduction = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATETTR = TRUE , test time reduction enabled!\n"));
	}
	else
	{
		pAd->ate.bTestTimeReduction = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATETTR = FALSE , test time reduction disabled!\n"));
	}	

	return TRUE;
}


INT	Set_ATE_Show_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	RTMP_STRING *Mode_String = NULL;
	RTMP_STRING *TxMode_String = NULL;
	UCHAR bw = 0, phy_mode = 0, sgi =0, mcs =0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
		bw = pATEInfo->TxWI.TXWI_N.BW;
		phy_mode = pATEInfo->TxWI.TXWI_N.PHYMODE;
		sgi = pATEInfo->TxWI.TXWI_N.ShortGI;
		mcs = pATEInfo->TxWI.TXWI_N.MCS;
	}
#endif /* RLT_MAC*/

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		bw = pATEInfo->TxWI.TXWI_O.BW;
		phy_mode = pATEInfo->TxWI.TXWI_O.PHYMODE;
		sgi = pATEInfo->TxWI.TXWI_O.ShortGI;
		mcs = pATEInfo->TxWI.TXWI_O.MCS;
	}
#endif /* RTMP_MAC */


	switch (pATEInfo->Mode)
	{
#ifdef CONFIG_RT2880_ATE_CMD_NEW
		case (fATE_IDLE):
			Mode_String = "ATESTART";
			break;
		case (fATE_EXIT):
			Mode_String = "ATESTOP";
			break;
#else
		case (fATE_IDLE):
			Mode_String = "APSTOP";
			break;
		case (fATE_EXIT):
			Mode_String = "APSTART";
			break;
#endif /* CONFIG_RT2880_ATE_CMD_NEW */
		case ((fATE_TX_ENABLE)|(fATE_TXCONT_ENABLE)):
			Mode_String = "TXCONT";
			break;
		case ((fATE_TX_ENABLE)|(fATE_TXCARR_ENABLE)):
			Mode_String = "TXCARR";
			break;
		case ((fATE_TX_ENABLE)|(fATE_TXCARRSUPP_ENABLE)):
			Mode_String = "TXCARS";
			break;
		case (fATE_TX_ENABLE):
			Mode_String = "TXFRAME";
			break;
		case (fATE_RX_ENABLE):
			Mode_String = "RXFRAME";
			break;
		default:
		{
			Mode_String = "Unknown ATE mode";
			DBGPRINT(RT_DEBUG_OFF, ("ERROR! Unknown ATE mode!\n"));
			break;
		}
	}	
	DBGPRINT(RT_DEBUG_OFF, ("ATE Mode=%s\n", Mode_String));
#ifdef RT3350
	if (IS_RT3350(pAd))
		DBGPRINT(RT_DEBUG_OFF, ("PABias=%u\n", pATEInfo->PABias));
#endif /* RT3350 */
	DBGPRINT(RT_DEBUG_OFF, ("TxPower0=%d\n", pATEInfo->TxPower0));
	DBGPRINT(RT_DEBUG_OFF, ("TxPower1=%d\n", pATEInfo->TxPower1));
#ifdef DOT11N_SS3_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("TxPower2=%d\n", pATEInfo->TxPower2));
#endif /* DOT11N_SS3_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("TxAntennaSel=%d\n", pATEInfo->TxAntennaSel));
	DBGPRINT(RT_DEBUG_OFF, ("RxAntennaSel=%d\n", pATEInfo->RxAntennaSel));
	DBGPRINT(RT_DEBUG_OFF, ("BBPCurrentBW=%u\n", bw));
	DBGPRINT(RT_DEBUG_OFF, ("GI=%u\n", sgi));
	DBGPRINT(RT_DEBUG_OFF, ("MCS=%u\n", mcs));

	switch (phy_mode)
	{
		case 0:
			TxMode_String = "CCK";
			break;
		case 1:
			TxMode_String = "OFDM";
			break;
		case 2:
			TxMode_String = "HT-Mix";
			break;
		case 3:
			TxMode_String = "GreenField";
			break;
		default:
		{
			TxMode_String = "Unknown TxMode";
			DBGPRINT(RT_DEBUG_OFF, ("ERROR! Unknown TxMode!\n"));
			break;
		}
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("TxMode=%s\n", TxMode_String));
	DBGPRINT(RT_DEBUG_OFF, ("Addr1=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pATEInfo->Addr1[0], pATEInfo->Addr1[1], pATEInfo->Addr1[2], pATEInfo->Addr1[3], pATEInfo->Addr1[4], pATEInfo->Addr1[5]));
	DBGPRINT(RT_DEBUG_OFF, ("Addr2=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pATEInfo->Addr2[0], pATEInfo->Addr2[1], pATEInfo->Addr2[2], pATEInfo->Addr2[3], pATEInfo->Addr2[4], pATEInfo->Addr2[5]));
	DBGPRINT(RT_DEBUG_OFF, ("Addr3=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pATEInfo->Addr3[0], pATEInfo->Addr3[1], pATEInfo->Addr3[2], pATEInfo->Addr3[3], pATEInfo->Addr3[4], pATEInfo->Addr3[5]));
	DBGPRINT(RT_DEBUG_OFF, ("Channel=%u\n", pATEInfo->Channel));
	DBGPRINT(RT_DEBUG_OFF, ("TxLength=%u\n", pATEInfo->TxLength));
	DBGPRINT(RT_DEBUG_OFF, ("TxCount=%u\n", pATEInfo->TxCount));
	DBGPRINT(RT_DEBUG_OFF, ("RFFreqOffset=%u\n", pATEInfo->RFFreqOffset));
#ifdef RT35xx
	if (IS_RT3572(pAd))
		DBGPRINT(RT_DEBUG_OFF, ("LedIndicatorStregth=%d\n", pATEInfo->LedIndicatorStrength));
#endif /* RT35xx */
	DBGPRINT(RT_DEBUG_OFF, ("bAutoTxAlc=%d\n", pATEInfo->bAutoTxAlc));
#ifdef VCORECAL_SUPPORT 
	if (IS_RT3593(pAd) || IS_MT76x0(pAd))
	DBGPRINT(RT_DEBUG_OFF, ("bAutoVcoCal=%d\n", pATEInfo->bAutoVcoCal));
#endif /* VCORECAL_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("bTestTimeReduction=%d\n", pATEInfo->bTestTimeReduction));
	DBGPRINT(RT_DEBUG_OFF, ("IPG=%u\n", pATEInfo->IPG));
	DBGPRINT(RT_DEBUG_OFF, ("Payload=0x%02x\n", pATEInfo->Payload));
#ifdef TXBF_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("bTxBF=%d\n", pATEInfo->bTxBF));
	DBGPRINT(RT_DEBUG_OFF, ("txSoundingMode=%d\n", pATEInfo->txSoundingMode));
#endif /* TXBF_SUPPORT */
#ifdef IQ_CAL_SUPPORT
	if (IS_RT5592(pAd))
	{
		DBGPRINT(RT_DEBUG_OFF, ("bIQCompEnable=%d\n", pATEInfo->bIQCompEnable));
	}
#endif /* IQ_CAL_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("Set_ATE_Show_Proc Success\n"));
#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	return TRUE;
}


INT	Set_ATE_Help_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef CONFIG_RT2880_ATE_CMD_NEW
	DBGPRINT(RT_DEBUG_OFF, ("ATE=ATESTART, ATESTOP, TXCONT, TXCARR, TXCARS, TXFRAME, RXFRAME\n"));
#else
	DBGPRINT(RT_DEBUG_OFF, ("ATE=APSTOP, APSTART, TXCONT, TXCARR, TXCARS, TXFRAME, RXFRAME\n"));
#endif /* CONFIG_RT2880_ATE_CMD_NEW */
	DBGPRINT(RT_DEBUG_OFF, ("ATEDA\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATESA\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEBSSID\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATECHANNEL, range:0~14(unless A band !)\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXPOW0, set power level of antenna 1.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXPOW1, set power level of antenna 2.\n"));
#ifdef DOT11N_SS3_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("ATETXPOW2, set power level of antenna 3.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXANT, set TX antenna. 0:all, 1:antenna one, 2:antenna two, 3:antenna three.\n"));
#else
	DBGPRINT(RT_DEBUG_OFF, ("ATETXANT, set TX antenna. 0:all, 1:antenna one, 2:antenna two.\n"));
#endif /* DOT11N_SS3_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("ATERXANT, set RX antenna.0:all, 1:antenna one, 2:antenna two, 3:antenna three.\n"));
#ifdef RT3350
	if (IS_RT3350(pAd))
		DBGPRINT(RT_DEBUG_OFF, ("ATEPABIAS, set power amplifier bias for EVM, range 0~15\n"));
#endif /* RT3350 */
#ifdef RTMP_RF_RW_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("ATETXFREQOFFSET, set frequency offset, range 0~95\n"));
#else
	DBGPRINT(RT_DEBUG_OFF, ("ATETXFREQOFFSET, set frequency offset, range 0~63\n"));
#endif /* RTMP_RF_RW_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("ATETXBW, set BandWidth, 0:20MHz, 1:40MHz , 2:80MHz.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXLEN, set Frame length, range 24~%d\n", (MAX_FRAME_SIZE - 34/* == 2312 */)));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXCNT, set how many frame going to transmit.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXMCS, set MCS, reference to rate table.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXMODE, set Mode 0:CCK, 1:OFDM, 2:HT-Mix, 3:GreenField, 4:VHT, reference to rate table.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXGI, set GI interval, 0:Long, 1:Short\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERXFER, 0:disable Rx Frame error rate. 1:enable Rx Frame error rate.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERRF, show all RF registers.\n"));
#ifndef RTMP_RF_RW_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF1, set RF1 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF2, set RF2 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF3, set RF3 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF4, set RF4 register.\n"));
#endif /* !RTMP_RF_RW_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("ATELDE2P, load EEPROM from .bin file.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERE2P, display all EEPROM content.\n"));
#ifdef LED_CONTROL_SUPPORT
#ifdef RT35xx
	if (IS_RT3572(pAd))
	{
		DBGPRINT(RT_DEBUG_OFF, ("ATELED, test signal LED, range 0~31.\n"));
		DBGPRINT(RT_DEBUG_OFF, ("ATEHWLED, control LED by hardware.\n"));
	}
#endif /* RT35xx */
#endif /* LED_CONTROL_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("ATEAUTOALC, enable ATE auto Tx alc (Tx auto level control).\n"));
#ifdef VCORECAL_SUPPORT 
	if (IS_RT3593(pAd) || (IS_MT76x0(pAd)))
		DBGPRINT(RT_DEBUG_OFF, ("ATEVCOCAL, enable ATE Tx VCO calibration.\n"));
#endif /* VCORECAL_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("ATEIPG, set ATE Tx frame IPG.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEPAYLOAD, set ATE payload pattern for TxFrame.\n"));
#ifdef TXBF_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("ATETXBF, enable ATE Tx beam forming.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXSOUNDING, Sounding mode 0:none, 1:Data sounding, 2:2 stream NDP, 3:3 stream NDP.\n"));
#endif /* TXBF_SUPPORT */ 
#ifdef RTMP_INTERNAL_TX_ALC
	DBGPRINT(RT_DEBUG_OFF, ("ATETSSICBA, start internal TSSI calibration.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETSSICBAEX, start extended internal TSSI calibration.\n"));
#endif /* RTMP_INTERNAL_TX_ALC */
#ifdef RTMP_TEMPERATURE_COMPENSATION
		DBGPRINT(RT_DEBUG_OFF, ("ATETSSICBAEX, start advanced temperature TSSI calibration.\n"));
#endif /* RTMP_TEMPERATURE_COMPENSATION */
#ifdef RT33xx	
	DBGPRINT(RT_DEBUG_OFF, ("ATETXEVMSHOW, show the information of EVM calibration.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXEVMCAL, write EVM calibration value to corresponding RF register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXEVMFILL, fill EVM calibration result in corresponding EEPROM.\n"));
#endif /* RT33xx */
#ifdef IQ_CAL_SUPPORT
	if (IS_RT5592(pAd))
	{
		DBGPRINT(RT_DEBUG_OFF, ("ATEIQCOMP, enable I/Q gain/phase compensation.\n"));
	}
#endif /* IQ_CAL_SUPPORT */
	DBGPRINT(RT_DEBUG_OFF, ("ATESHOW, display all parameters of ATE.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEHELP, online help.\n"));

	return TRUE;
}

#ifdef RTMP_TEMPERATURE_CALIBRATION
#ifdef MT76x0
extern BOOLEAN mt76x0_get_tssi_report(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bResetTssiInfo,
	OUT PCHAR pTssiReport);

INT MT76x0_ATETempCalibration(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
/*	PATE_INFO pATEInfo = &(pAd->ate); */
	INT32 ref25 = 0, curr_reading = 0, curr_temperature = 0;
	INT ret = TRUE;
	UINT16 e2p_data = 0;
	BOOLEAN bResetTssiInfo = TRUE;
	
	curr_temperature = (INT32)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("curr_temperature = 0x%x\n", curr_temperature));

	/* to get current temperature */
	if (mt76x0_get_tssi_report(pAd,
								bResetTssiInfo,
								&pAd->CurrTemperature) == TRUE)
	{
		curr_reading = pAd->CurrTemperature;
		DBGPRINT(RT_DEBUG_TRACE, ("curr_reading = 0x%x\n", curr_reading));

		ref25 = curr_reading + ((25 - curr_temperature)*10)/19;
		/* write calibration result */
		RT28xx_EEPROM_READ16(pAd, 0xD0, e2p_data);
		e2p_data &= ~(0xFF00);
		e2p_data |= (((ref25 & 0xFF) << 8) & 0xFF00);	
		RT28xx_EEPROM_WRITE16(pAd, 0xD0, e2p_data);
	}
	else
	{
		ret = FALSE;
		DBGPRINT(RT_DEBUG_ERROR, 
					("%s(): Failed to get current temperature !\n",
					__FUNCTION__));
	}

	return ret;
}
#endif /* MT76x0 */	
#ifdef RT6352
INT RT6352_ATETempCalibration(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
/*	PATE_INFO pATEInfo = &(pAd->ate); */
	INT32 ref25 = 0, curr_reading = 0, curr_temperature = 0;
	INT ret = TRUE;
	UINT16 e2p_data = 0;

	curr_temperature = (INT32)simple_strtol(arg, 0, 10);
	DBGPRINT(RT_DEBUG_TRACE, ("curr_temperature = 0x%x\n", curr_temperature));

	/* to get bbp r49 */
	RT6352_Temperature_Init(pAd);	
	curr_reading = pAd->CurrTemperature;
	DBGPRINT(RT_DEBUG_TRACE, ("curr_reading = 0x%x\n", curr_reading));

	ref25 = curr_reading + ((25 - curr_temperature)*10)/19;
	/* write calibration result */
	RT28xx_EEPROM_READ16(pAd, 0xD0, e2p_data);
	e2p_data &= ~(0xFF00);
	e2p_data |= (((ref25 & 0xFF) << 8) & 0xFF00);	
	RT28xx_EEPROM_WRITE16(pAd, 0xD0, e2p_data);

	return ret;
}
#endif /* RT6352 */	


INT Set_ATE_TEMP_CAL_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT ret = TRUE;

#ifdef MT76x0
	if (IS_MT76x0(pAd))
		ret = MT76x0_ATETempCalibration(pAd, arg);		
#endif /* MT76x0 */	
#ifdef RT6352
	if (IS_RT6352(pAd))
		ret = RT6352_ATETempCalibration(pAd, arg);		
#endif /* RT6352 */	

	return ret;
}


INT Set_ATE_SHOW_TSSI_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT ret = TRUE;
#ifdef RT6352
	UCHAR wait, BBPValue, RFValue, RFB0R2Value;
	CHAR BbpR49 = 0;
#endif /* RT6352 */

#ifdef MT76x0
	if (IS_MT76x0(pAd))
	{
		if (mt76x0_get_tssi_report(pAd,
									TRUE,
									&pAd->CurrTemperature) == TRUE)
		{
			printk("BBP CORE_R35[0x208c]: 0x%02X\n", pAd->CurrTemperature);
		}
		else
		{
			ret = FALSE;
			printk("Read temperature failed !\n");
		}
	}
#endif /* MT76x0 */
#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		/* 1. set BBP R47[4]=1 to enable TSSI for next reading */
		/* enable RF before reading temperature sensor */
		ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_BANK0, RF_R02, &RFValue);
		RFB0R2Value = RFValue; /* keep it for recovery later */
		RFValue |= 0x10;
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_BANK0, RF_R02, RFValue);

		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);

		if (!(BBPValue & (1<<4)))
		{
			BBPValue |= (1<<4);
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BBPValue);		
		}

		/* 2. polling BBP_R47 until BBP R47[4]==0 */
		for (wait=0; wait<200; wait++)
		{
			RtmpusecDelay(100);
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);

			if (!(BBPValue & (1<<4)))
			{
				break;
			}
		}

		if ((BBPValue & 0x10) != 0)
		{
			ret = FALSE;
			printk("Read TSSI failed !\n");
			return ret;
		}

		/* 3. set BBP R47[3:0]=4 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);
		BBPValue &= ~(0x0F);
		BBPValue |= (0x04);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BBPValue);

		/* 4. fetch temperature reading */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpR49);

		/* 5. recover RF after reading temperature sensor */
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_BANK0, RF_R02, RFB0R2Value);

		printk("BBP R49: 0x%02X\n", BbpR49);
	}
#endif /* RT6352 */	

	return ret;
}
#endif /* RTMP_TEMPERATURE_CALIBRATION */


#if defined(RTMP_INTERNAL_TX_ALC) || defined(RTMP_TEMPERATURE_COMPENSATION)
INT Set_ATE_TSSI_CALIBRATION_EX_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{    
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->ExtendedTssiCalibration != NULL)
	{
		pATEInfo->pChipStruct->ExtendedTssiCalibration(pAd, arg);
	}
	else
	{
		RTMP_CHIP_ATE_TSSI_CALIBRATION_EXTEND(pAd, arg);
	}
	
	return TRUE;
}
#endif /* defined(RTMP_INTERNAL_TX_ALC) || defined(RTMP_TEMPERATURE_COMPENSATION) */


#ifdef RTMP_INTERNAL_TX_ALC
#if defined(RT3370) || defined(RT3390)
CHAR RTUSBInsertTssi(UCHAR InChannel, UCHAR Channel0, UCHAR Channel1,CHAR Tssi0, CHAR Tssi1)
{
	CHAR     InTssi;
	CHAR     ChannelDelta, InChannelDelta;
	CHAR     TssiDelta;

	ChannelDelta = Channel1 - Channel0;
	InChannelDelta = InChannel - Channel0;
	TssiDelta = Tssi1 - Tssi0;

	InTssi = Tssi0 + ((InChannelDelta * TssiDelta) / ChannelDelta);

	return InTssi;
}
#endif /* defined(RT3370) || defined(RT3390) */


INT Set_ATE_TSSI_CALIBRATION_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{    
	PATE_INFO pATEInfo = &(pAd->ate);

	if (pATEInfo->pChipStruct->TssiCalibration != NULL)
{
		pATEInfo->pChipStruct->TssiCalibration(pAd, arg);
	}
	else
	{
		RTMP_CHIP_ATE_TSSI_CALIBRATION(pAd, arg);
	}

	return TRUE;
}


#if defined(RT3350) || defined(RT3352)
INT RT335x_Set_ATE_TSSI_CALIBRATION_ENABLE_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
		{
	BOOLEAN	bTSSICalbrEnableG = FALSE;
	
	if (pAd->TxPowerCtrl.bInternalTxALC == FALSE)                  
			{
		DBGPRINT_ERR(("Please set e2p 0x36 as 0x2024!!!\n"));
		return FALSE;
		}

	if ((!IS_RT3350(pAd)) && (!IS_RT3352(pAd)))                  
	{
		DBGPRINT_ERR(("Not support TSSI calibration since not 3350/3352 chip!!!\n"));
		return FALSE;
	}

	if (strcmp(arg, "0") == 0)
		{
		bTSSICalbrEnableG = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("TSSI calibration disabled!\n"));
	}
	else if (strcmp(arg, "1") == 0)
			{
		bTSSICalbrEnableG = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("TSSI calibration enabled!\n"));
			}
	else
	{
		return FALSE;
		}

	pAd->ate.bTSSICalbrEnableG = bTSSICalbrEnableG;
			
#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
	}


CHAR InsertTssi(UCHAR InChannel, UCHAR Channel0, UCHAR Channel1,CHAR Tssi0, CHAR Tssi1)
{
	CHAR     InTssi;
	CHAR     ChannelDelta, InChannelDelta;
	CHAR     TssiDelta;

	ChannelDelta = Channel1 - Channel0;
	InChannelDelta = InChannel - Channel0;
	TssiDelta = Tssi1 - Tssi0;

	InTssi = Tssi0 + ((InChannelDelta * TssiDelta) / ChannelDelta);

	return InTssi;
}


INT RT335xATETssiCalibrationExtend(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{    
	CHAR		TssiRefPerChannel[CFG80211_NUM_OF_CHAN_2GHZ], TssiDeltaPerChannel[CFG80211_NUM_OF_CHAN_2GHZ];
	UCHAR		CurrentChannel;
	UCHAR		BbpData = 0;
	USHORT		EEPData;

	if (pAd->ate.bTSSICalbrEnableG == FALSE)
	{
		DBGPRINT_ERR(("No TSSI readings obtained !!!\n"));
		DBGPRINT_ERR(("TSSI calibration failed !!!\n"));

		return FALSE;
	}
	else
	{
		pAd->ate.bTSSICalbrEnableG = FALSE;
	}

	NdisCopyMemory(TssiRefPerChannel, pAd->ate.TssiRefPerChannel, CFG80211_NUM_OF_CHAN_2GHZ);
	NdisCopyMemory(TssiDeltaPerChannel, pAd->ate.TssiDeltaPerChannel, CFG80211_NUM_OF_CHAN_2GHZ);

	/* step 1: write TSSI_ref to EEPROM 0x6E */
	CurrentChannel = 7;
	BbpData = TssiRefPerChannel[CurrentChannel-1];
	DBGPRINT(RT_DEBUG_TRACE, ("TSSI_ref = 0x%02x\n", TssiRefPerChannel[CurrentChannel-1]));  
	RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_OVER_OFDM_54, EEPData);
	EEPData &= 0xff00;
	EEPData |= BbpData;
	DBGPRINT(RT_DEBUG_TRACE, ("Write  E2P 0x6e: 0x%04x\n", EEPData));  
	RT28xx_EEPROM_WRITE16(pAd, EEPROM_TSSI_OVER_OFDM_54, EEPData);
	RtmpusecDelay(10);

	/* step 2: insert the TSSI table */
	/* insert channel 2 to 6 TSSI values */
	for (CurrentChannel = 2; CurrentChannel < 7; CurrentChannel++)
		TssiRefPerChannel[CurrentChannel-1] = InsertTssi(CurrentChannel, 1, 7, TssiRefPerChannel[0], TssiRefPerChannel[6]);

	/* insert channel 8 to 12 TSSI values */
	for (CurrentChannel = 8; CurrentChannel < 13; CurrentChannel++)
		TssiRefPerChannel[CurrentChannel-1] = InsertTssi(CurrentChannel, 7, 13, TssiRefPerChannel[6], TssiRefPerChannel[12]);

	/* channel 14 TSSI equals channel 13 TSSI */
	TssiRefPerChannel[13] = TssiRefPerChannel[12];

	for (CurrentChannel = 1; CurrentChannel <= 14; CurrentChannel++)
	{
		TssiDeltaPerChannel[CurrentChannel-1] = TssiRefPerChannel[CurrentChannel-1] - TssiRefPerChannel[6];

		/* boundary check */
		if(TssiDeltaPerChannel[CurrentChannel-1] > 7 )
			TssiDeltaPerChannel[CurrentChannel-1]  = 7;
		if(TssiDeltaPerChannel[CurrentChannel-1] < -8 )
			TssiDeltaPerChannel[CurrentChannel-1]  = -8;

		/* eeprom only use 4 bit for TSSI delta */
		TssiDeltaPerChannel[CurrentChannel-1]  &= 0x0f;
		DBGPRINT(RT_DEBUG_TRACE, ("Channel %d, TSSI= 0x%x, TssiDelta=0x%x\n", 
		CurrentChannel, TssiRefPerChannel[CurrentChannel-1], TssiDeltaPerChannel[CurrentChannel-1]));    
	}

	/* step 3: store TSSI delta values to EEPROM */
	RT28xx_EEPROM_READ16(pAd, EEPROM_TX_POWER_OFFSET_OVER_CH_1-1, EEPData);
	EEPData &= 0x00ff;
	EEPData |= (TssiDeltaPerChannel[0] << 8) | (TssiDeltaPerChannel[1] << 12);
	RT28xx_EEPROM_WRITE16(pAd, EEPROM_TX_POWER_OFFSET_OVER_CH_1-1, EEPData);
	RtmpusecDelay(10);

	for (CurrentChannel = 3; CurrentChannel <= 14; CurrentChannel += 4)
	{
		EEPData = (TssiDeltaPerChannel[CurrentChannel+2] << 12) |(TssiDeltaPerChannel[CurrentChannel+1] << 8)
			| (TssiDeltaPerChannel[CurrentChannel] << 4) | TssiDeltaPerChannel[CurrentChannel-1];
		RT28xx_EEPROM_WRITE16(pAd, (EEPROM_TX_POWER_OFFSET_OVER_CH_3 +((CurrentChannel-3)/2)), EEPData);
		RtmpusecDelay(10);
	}

	/* step 4: disable legacy ALC and set TSSI enabled and TSSI extend mode to EEPROM */
	RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_ENABLE, EEPData);
	/* disable legacy ALC */
	EEPData &= ~(1 << 1);
	/* enable TSSI */
	EEPData |= (1 << 13);
	RT28xx_EEPROM_WRITE16(pAd, EEPROM_TSSI_ENABLE, EEPData);
	RtmpusecDelay(10);

	RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_MODE_EXTEND, EEPData);
	/* set extended TSSI mode */
	EEPData |= (1 << 15);
	RT28xx_EEPROM_WRITE16(pAd, EEPROM_TSSI_MODE_EXTEND, EEPData);
	RtmpusecDelay(10);

	/* step 5: synchronize ATE private data structure with the values written to EEPROM */
	NdisCopyMemory(pAd->ate.TssiRefPerChannel, TssiRefPerChannel, CFG80211_NUM_OF_CHAN_2GHZ);
	NdisCopyMemory(pAd->ate.TssiDeltaPerChannel, TssiDeltaPerChannel, CFG80211_NUM_OF_CHAN_2GHZ);

	return TRUE;
}

#endif /* defined(RT3350) || defined(RT3352) */
#endif /* RTMP_INTERNAL_TX_ALC */


#ifdef RTMP_TEMPERATURE_COMPENSATION
#ifdef RT5592
INT RT5592_ATEReadExternalTSSI(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR	index = 0, inputData = 0, BbpData = 0;
	INT 	TempBbp = 0;
	USHORT	EEPData;

	inputData = simple_strtol(arg, 0, 10);

	if ((pATEInfo->bTSSICalbrEnableG == FALSE) && (pATEInfo->bTSSICalbrEnableA == FALSE))
	{
		DBGPRINT_ERR(("Both bTSSICalbrEnableG and bTSSICalbrEnableA are FALSE !!!\n"));
		DBGPRINT_ERR(("TSSI calibration failed !!!\n"));

		return FALSE;
	}

	/* Take average of BBP R49 readings and write it into EEPROM. */
	if (pATEInfo->bTSSICalbrEnableG == TRUE)
	{
		pATEInfo->bTSSICalbrEnableG = FALSE;

		/* G band */
		for (index = 0; index < TSSI_READ_SAMPLE_NUM; index++)
		{
			TempBbp += pATEInfo->TssiReadSampleG[index];
			pATEInfo->TssiReadSampleG[index] = 0;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("Sum of TSSI readings (2.4G) = 0x%x\n", TempBbp));

		BbpData = (UCHAR)(TempBbp / TSSI_READ_SAMPLE_NUM);

		if ((TempBbp % TSSI_READ_SAMPLE_NUM) > (TSSI_READ_SAMPLE_NUM >> 1))
		{
			BbpData += 1;
		}

		RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_STEP_OVER_2DOT4G, EEPData);
		EEPData &= 0xff00;
		EEPData |= BbpData;

#ifdef RTMP_EFUSE_SUPPORT
		if (pAd->bUseEfuse)
		{
			if (pAd->bFroceEEPROMBuffer)
				NdisMoveMemory(&(pAd->EEPROMImage[EEPROM_TSSI_STEP_OVER_2DOT4G]), (PUSHORT)(&EEPData), 2);
			else
				eFuseWrite(pAd, EEPROM_TSSI_STEP_OVER_2DOT4G, (PUSHORT)(&EEPData), 2);
		}
		else
#endif /* RTMP_EFUSE_SUPPORT */
		{
			RT28xx_EEPROM_WRITE16(pAd, EEPROM_TSSI_STEP_OVER_2DOT4G, EEPData);
			RtmpusecDelay(10);
		}
	}

	if (pATEInfo->bTSSICalbrEnableA == TRUE)
	{
		/* A band */
		pATEInfo->bTSSICalbrEnableA = FALSE;
		
		TempBbp = 0;

		for (index = 0; index < TSSI_READ_SAMPLE_NUM; index++)
		{
			TempBbp += pATEInfo->TssiReadSampleA[index];
			pATEInfo->TssiReadSampleA[index] = 0;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("Sum of TSSI readings (5G) = 0x%x\n", TempBbp));

		BbpData = (UCHAR)(TempBbp / TSSI_READ_SAMPLE_NUM);

		if ((TempBbp % TSSI_READ_SAMPLE_NUM) > (TSSI_READ_SAMPLE_NUM >> 1))
		{
			BbpData += 1;
		}

		RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_STEP_OVER_5DOT5G, EEPData);
		EEPData &= 0xff00;
		EEPData |= BbpData;

#ifdef RTMP_EFUSE_SUPPORT
		if (pAd->bUseEfuse)
		{
			if (pAd->bFroceEEPROMBuffer)
				NdisMoveMemory(&(pAd->EEPROMImage[EEPROM_TSSI_STEP_OVER_5DOT5G]), (PUSHORT)(&EEPData), 2);
			else
				eFuseWrite(pAd, EEPROM_TSSI_STEP_OVER_5DOT5G, (PUSHORT)(&EEPData), 2);
		}
		else
#endif /* RTMP_EFUSE_SUPPORT */
		{
			RT28xx_EEPROM_WRITE16(pAd, EEPROM_TSSI_STEP_OVER_5DOT5G, EEPData);
			RtmpusecDelay(10);
		}
	}
	
	RT28xx_EEPROM_READ16(pAd, EEPROM_NIC2_OFFSET, EEPData);
	/*
		enable temperature compensation 
		(i.e., let pAd->bAutoTxAgcA == pAd->bAutoTxAgcG == TRUE)
	*/
	EEPData |= (1 << 1);
	/*
		disable internal ALC compensation 
		(i.e., let pAd->TxPowerCtrl.bInternalTxALC == FALSE)
	*/
	EEPData &= ~(1 << 13);
#ifdef RTMP_EFUSE_SUPPORT
	if (pAd->bUseEfuse)
	{
		if (pAd->bFroceEEPROMBuffer)
			NdisMoveMemory(&(pAd->EEPROMImage[EEPROM_NIC2_OFFSET]), (PUCHAR)(&EEPData), 2);
		else
			eFuseWrite(pAd, EEPROM_NIC2_OFFSET, (PUSHORT)(&EEPData), 2);
	}
	else
#endif /* RTMP_EFUSE_SUPPORT */
	{
		RT28xx_EEPROM_WRITE16(pAd, EEPROM_NIC2_OFFSET /* EEPROM_TSSI_ENABLE */, EEPData);
		RtmpusecDelay(10);
	}

	return TRUE;
}


INT RT5592_Set_ATE_TSSI_CALIBRATION_ENABLE_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	BOOLEAN	bTSSICalbrEnableA = FALSE, bTSSICalbrEnableG = FALSE;
	
	if (!IS_RT5592(pAd))                  
	{
		DBGPRINT_ERR(("Not support temperature calibration since not 5572/5592 chip!!!\n"));
		return FALSE;
	}

	if (strcmp(arg, "0") == 0)
	{
		bTSSICalbrEnableA = FALSE;
		bTSSICalbrEnableG = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("temperature calibration disabled!\n"));
	}
	else if (strcmp(arg, "1") == 0)
	{
		bTSSICalbrEnableA = TRUE;
		bTSSICalbrEnableG = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("A band temperature calibration enabled!\n"));
	}
	else if (strcmp(arg, "2") == 0)
	{
		bTSSICalbrEnableA = FALSE;
		bTSSICalbrEnableG = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("G band temperature calibration enabled!\n"));
	}
	else if (strcmp(arg, "3") == 0)
	{
		bTSSICalbrEnableA = TRUE;
		bTSSICalbrEnableG = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("Dual band temperature calibration enabled!\n"));
	}
	else
	{
		return FALSE;
	}

	pAd->ate.bTSSICalbrEnableG = bTSSICalbrEnableG;
	pAd->ate.bTSSICalbrEnableA = bTSSICalbrEnableA;

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */

	return TRUE;
}
#endif /* RT5592 */


INT Set_ATE_READ_EXTERNAL_TSSI_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef RT5592
	INT ret = FALSE;

	if (IS_RT5592(pAd))
	{
		ret = RT5592_ATEReadExternalTSSI(pAd, arg);

		return ret;
	}
#endif /* RT5592 */

	RTMP_CHIP_ATE_READ_EXTERNAL_TSSI(pAd, arg);
	return TRUE;
}
#endif /* RTMP_TEMPERATURE_COMPENSATION */


#ifdef RT33xx
static VOID TX_EVM_CAL_GET(
	IN PRTMP_ADAPTER pAd,
	OUT PUINT8 pRfReg,
	OUT PUINT8 pBitMap,
	OUT PUINT8 pRange,
	OUT PUINT8 pShift)
{
	UINT16 Value;                              
 	UINT loop;
 
	do
 	{
		RT28xx_EEPROM_READ16(pAd, 0x012c,  Value);
		if (Value == 0xffff)
		{
			DBGPRINT(RT_DEBUG_OFF, ("Invalid value of TxEvmCal (%x)\n", Value));
			break;
		}
 
		*pRfReg = (UINT8) ((Value & 0xff00) >> 8);
		*pBitMap = (UINT8) (Value & 0x00ff);
 
		*pRange = *pBitMap;
		*pShift = 0;

		for (loop = 0; loop<8; loop++)
		{
	 		if ((*pRange & (0x01 << loop)) == 0)
				(*pShift)++;
			else
				break;
		}
		*pRange = (*pRange) >> (*pShift);
 
	} while (0);
}

 
INT Set_ATE_TX_EVM_CALIBRATION_Show_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 RfReg;
	UINT8 BitMap;
	UINT8 Range;
	UINT8 Shift;
 
	TX_EVM_CAL_GET(pAd, &RfReg, &BitMap, &Range, &Shift);
	DBGPRINT(RT_DEBUG_OFF, ("RfReg=%d, Range=%d, BitMap=%x, Shift=%d\n", RfReg, Range, BitMap, Shift));
 
	return TRUE; 
}

 
INT Set_ATE_TX_EVM_CALIBRATION_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 RfReg;
	UINT8 BitMap;
	UINT8 Range;
	UINT8 Shift;
	LONG Value;
	UCHAR RFValue;
	
 
	TX_EVM_CAL_GET(pAd, &RfReg, &BitMap, &Range, &Shift);
	Value = simple_strtol(arg, 0, 10);
 
	if (Value > Range)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Invalid Range value(%ld), Range=%d\n", Value, Range));
		return TRUE;
	}
	
 	/* write to coresponding Rf register */
	ATE_RF_IO_READ8_BY_REG_ID(pAd, RfReg, &RFValue);
	RFValue &= ~(BitMap);
	RFValue |= (Value << Shift);
	ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RfReg, RFValue);
 
	return TRUE;
}

 
INT Set_ATE_TX_EVM_CALIBRATION_Fill_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UINT8 RfReg;
	UINT8 BitMap;
	UINT8 Range;
	UINT8 Shift;
	LONG Value;
	UINT EPRomOffset;
	UINT16 TargetValue;
 
	TX_EVM_CAL_GET(pAd, &RfReg, &BitMap, &Range, &Shift);
 
	switch (RfReg)
	{
		case RF_R09:
			EPRomOffset = EEPROM_EVM_RF09;
			break;
		case RF_R19:
			EPRomOffset = EEPROM_EVM_RF19;
			break;
		case RF_R21:
			EPRomOffset = EEPROM_EVM_RF21;
			break;
		case RF_R29:
			EPRomOffset = EEPROM_EVM_RF29;
			break;
		default:
			DBGPRINT(RT_DEBUG_OFF, ("Invalid RfReg(%d)", RfReg));
			return TRUE;
	}
 
	TargetValue = simple_strtol(arg, 0, 10); 
	if (TargetValue > Range)
	{
		DBGPRINT(RT_DEBUG_OFF, ("Invalid Range value(%d), Range=%d",TargetValue, Range));
		return TRUE;
	}
 
	Value=0;
	Value=RfReg<<8;
	ATE_RF_IO_READ8_BY_REG_ID(pAd, RfReg, (PUCHAR)(&Value));
	Value &= ~(BitMap);
	Value |= (TargetValue << Shift);
	RT28xx_EEPROM_WRITE16(pAd, EPRomOffset, Value);
 
	return TRUE; 
}
#endif /* RT33xx */


#ifdef MT7601
INT Set_ATE_Read_Temperature_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	CHAR	Temperature;

	DBGPRINT(RT_DEBUG_TRACE,("Set_MT7601ATE_Read_Temperature_Proc\n"));

	MT7601_Read_Temperature(pAd, &Temperature);

	DBGPRINT(RT_DEBUG_TRACE,("Temperature = %d (0x%X)\n", Temperature, Temperature));

	return TRUE;
}


INT Set_ATE_Read_TSSI_DC_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR BbpReg;

	DBGPRINT(RT_DEBUG_TRACE,("Set_ATE_Read_TSSI_DC_Proc\n"));

	RTMP_IO_WRITE32(pAd, 0x50C, 0x30);
	RTMP_IO_WRITE32(pAd, 0x504, 0xC0030);
	/* Set VGA gain */
	rlt_rf_write(pAd, RF_BANK5, RF_R03, 0x8);
	/* Mixer disable */
	rlt_rf_write(pAd, RF_BANK4, RF_R39, 0x0);

	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4  , 0x0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R58  , 0x0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21  , 0x1);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21  , 0x0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47  , 0x50);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22  , 0x40);
	RtmpOsMsDelay(10);

	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpReg);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, 0x40);
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpReg);
	DBGPRINT(RT_DEBUG_TRACE,("TSSI DC = %d (0x%X)\n", BbpReg, BbpReg));
	RtmpOsMsDelay(1);

	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22  , 0x0);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21  , 0x1);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21  , 0x0);
	RTMP_IO_WRITE32(pAd, 0x504, 0x0);
	RTMP_IO_WRITE32(pAd, 0x50C, 0x0);
	rlt_rf_write(pAd, RF_BANK4, RF_R39, 0xB3);

	return TRUE;
}
#endif /* MT7601 */

#ifdef MT76x2
INT Set_ATE_DO_CALIBRATION_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT rv = 0;
	UINT32 cal_id, param;
	
	if (arg)
	{
		rv = sscanf(arg, "%d-%d", &(cal_id), &(param));
		DBGPRINT(RT_DEBUG_TRACE, ("%s():cal_id = %d, param = %d\n", __FUNCTION__, cal_id, param));
		if (rv == 2)
		{
			mt76x2_ate_do_calibration(pAd, cal_id, param);
		}
	}

	return TRUE;
}


INT Set_ATE_Load_CR_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT rv = 0;
	UINT32 mode;
	UINT8 temp_level, channel;

	if (arg)
	{
		rv = sscanf(arg, "%d-%d-%d", &(mode), &(temp_level), &(channel));
		DBGPRINT(RT_DEBUG_TRACE, ("%s():mode = %d, temp_level = %d, channel = %d\n", __FUNCTION__, mode, temp_level, channel));
		if (rv == 3)
		{
			andes_load_cr(pAd, mode, temp_level, channel);
		}
	}

	return TRUE;
}

#endif /* MT76x2 */

struct _ATE_CHIP_STRUCT RALINKDefault =
{
	/* functions */
	.ChannelSwitch = NULL,
	.TxPwrHandler = NULL,
	.TssiCalibration = NULL,
	.ExtendedTssiCalibration = NULL,
	.RxVGAInit = NULL,
	.AsicSetTxRxPath = NULL,
	.AdjustTxPower = NULL,	
	.AsicExtraPowerOverMAC = NULL,

	/* command handlers */
	.Set_BW_Proc = NULL,
	.Set_FREQ_OFFSET_Proc = NULL,

	/* variables */
	.maxTxPwrCnt = 5,
	.bBBPStoreTXCARR = FALSE,
	.bBBPStoreTXCARRSUPP = FALSE,	
	.bBBPStoreTXCONT = FALSE,
	.bBBPLoadATESTOP = FALSE,
};

#ifdef RT28xx
#ifdef RTMP_MAC_PCI
extern ATE_CHIP_STRUCT RALINK2860;
extern ATE_CHIP_STRUCT RALINK2880;
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
extern ATE_CHIP_STRUCT RALINK2870;
#endif /* RTMP_MAC_USB */
#endif /* RT28xx */

#ifndef RT3593
#ifdef RT35xx
#ifdef RTMP_MAC_PCI
extern ATE_CHIP_STRUCT RALINK3062;
extern ATE_CHIP_STRUCT RALINK3562;
extern ATE_CHIP_STRUCT RALINK3592;
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
extern ATE_CHIP_STRUCT RALINK3572;
#endif /* RTMP_MAC_USB */
#endif /* RT35xx */
#endif /* !RT3593 */

#ifdef RT2883
extern ATE_CHIP_STRUCT RALINK2883;
#endif /* RT2883 */

#ifdef RT3883
extern ATE_CHIP_STRUCT RALINK3883;
#endif /* RT3883 */	

#ifdef RT305x
#ifdef RT3050
extern ATE_CHIP_STRUCT RALINK3050;
#endif /* RT3050 */

#ifdef RT3052
extern ATE_CHIP_STRUCT RALINK3052;
#endif /* RT3052 */

#ifdef RT3350
extern ATE_CHIP_STRUCT RALINK3350;
#endif /* RT3350 */

#ifdef RT3352
extern ATE_CHIP_STRUCT RALINK3352;
#endif /* RT3352 */

#ifdef RT5350
extern ATE_CHIP_STRUCT RALINK5350;
#endif /* RT5350 */
#endif /* RT305x */

#ifdef RT5592
#ifdef RTMP_MAC_PCI
extern ATE_CHIP_STRUCT RALINK5592;
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
extern ATE_CHIP_STRUCT RALINK5572;
#endif /* RTMP_MAC_USB */
#endif /* RT5592 */

#ifdef RT3593
/* both for RT3573 and RT3593 */
extern ATE_CHIP_STRUCT RALINK3593;
#endif /* RT3593 */

#ifndef RT35xx
#ifndef RT5592
#ifndef RT3593
#ifdef RT30xx
extern ATE_CHIP_STRUCT RALINK30xx;
#endif /* RT30xx */
#endif /* !RT3593 */
#endif /* !RT5592 */
#endif /* !RT35xx */

#ifdef RT33xx
extern ATE_CHIP_STRUCT RALINK33xx;
#endif /* RT30xx */

#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
extern ATE_CHIP_STRUCT RALINK5390;
extern ATE_CHIP_STRUCT RALINK5392;
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */

#ifdef RT6352
extern ATE_CHIP_STRUCT RALINK6352;
#endif /* RT6352 */

#ifdef MT7601
extern ATE_CHIP_STRUCT MT7601ATE;
#endif

#ifdef MT7603
extern ATE_CHIP_STRUCT MT7603ATE;
#endif

#ifdef RT8592
extern ATE_CHIP_STRUCT RALINK85592;
#endif /* RT8592 */

#ifdef MT76x0
#ifdef RTMP_MAC_PCI
extern ATE_CHIP_STRUCT RALINK6590;
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
extern ATE_CHIP_STRUCT RALINK6570;
#endif /* RTMP_MAC_USB */
#endif /* MT76x0 */

#ifdef MT76x2
extern ATE_CHIP_STRUCT mt76x2ate;
#endif /* MT76x2 */
/*
==========================================================================
	Description:
		Assign chip structure when initialization.
		This routine is specific for ATE.

==========================================================================
*/
NDIS_STATUS ChipStructAssign(
	IN	PRTMP_ADAPTER	pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);

	pATEInfo->pChipStruct = &RALINKDefault;

#ifdef RT8592
	if (IS_RT8592(pAd))
	{
		pATEInfo->pChipStruct = &RALINK85592;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK85592 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT8592 */

#ifdef MT76x0
#ifdef RTMP_MAC_USB
	if (IS_RT6570(pAd))
	{
		pATEInfo->pChipStruct = &RALINK6570;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK6570 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RTMP_MAC_USB */

#ifdef RTMP_MAC_PCI
	if (IS_MT7610(pAd))
	{
		pATEInfo->pChipStruct = &RALINK6590;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK6590 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RTMP_MAC_PCI */
#endif /* MT76x0 */

#ifdef MT76x2
	if (IS_MT76x2(pAd))
	{
		pATEInfo->pChipStruct = &mt76x2ate;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MT76x2 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* MT76x2 */

#ifdef RT6352
	if (IS_RT6352(pAd))
	{
		/* 
			Notice: IS_RT6352(_pAd) is currently defined 
			as (((_pAd)->MACVersion & 0xFFFF0000) == 0x53900000)
		*/
		pATEInfo->pChipStruct = &RALINK6352;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK6352 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* IS_RT6352 */

#ifdef RT3593
	/* both for RT3593 and RT3573 */
	if (IS_RT3593(pAd))
		{
		pATEInfo->pChipStruct = &RALINK3593;
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT3593 */

#ifdef RT2883
	if (IS_RT2883(pAd))
	{
		pATEInfo->pChipStruct = &RALINK2883;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK2883 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT2883 */

#ifdef RT3883
	if (IS_RT3883(pAd))
	{
		pATEInfo->pChipStruct = &RALINK3883;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK3883 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT3883 */

#ifdef RT35xx
	if (IS_RT3572(pAd))
	{
#ifdef RTMP_MAC_PCI
		if (IS_PCI_ONLY_INF(pAd))
		{
#ifdef A_BAND_SUPPORT
			if (pAd->RfIcType == RFIC_3052)
			{
				pATEInfo->pChipStruct = &RALINK3562;
			}
#else
			if (pAd->RfIcType == RFIC_3022)
			{
				pATEInfo->pChipStruct = &RALINK3062;
			}
#endif /* A_BAND_SUPPORT */
		}
		else if (IS_PCIE_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK3592;
		}
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		if (IS_USB_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK3572;
		}
#endif /* RTMP_MAC_USB */
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT35xx */

#ifdef RT305x
#ifdef RT3050
	if (IS_RT3050(pAd))
	{
		pATEInfo->pChipStruct = &RALINK3050;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK3050 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT3050 */

#ifdef RT3052
	if (IS_RT3052(pAd))
	{
		pATEInfo->pChipStruct = &RALINK3052;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK3052 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT3052 */

#ifdef RT3350
	if (IS_RT3350(pAd))
	{
		pATEInfo->pChipStruct = &RALINK3350;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK3350 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT3350 */

#ifdef RT3352
	if (IS_RT3352(pAd))
	{
		pATEInfo->pChipStruct = &RALINK3352;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK3352 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT3352 */

#ifdef RT5350
	if (IS_RT5350(pAd))
	{
		pATEInfo->pChipStruct = &RALINK5350;
		DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK5350 hook !\n", __FUNCTION__));
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT5350 */
#endif /* RT305x */

#ifdef RT5592
	if (IS_RT5592(pAd))
	{
#ifdef RTMP_MAC_PCI
		if (IS_PCIE_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK5592;
			DBGPRINT(RT_DEBUG_OFF, ("Hook RALINK5592 !!!\n"));
		}
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		if (IS_USB_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK5572;
		}
#endif /* RTMP_MAC_USB */
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT5592 */

#ifdef RT28xx
	if (IS_RT2860(pAd))
	{
#ifdef RT2860
		if (IS_PCI_ONLY_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK2860;
			DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK2860 hook !\n", __FUNCTION__));
			return NDIS_STATUS_SUCCESS;
		}
#endif /* RT2860 */

#ifdef RT2870
		if (IS_USB_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK2870;
		}
#endif /* RT2870 */

#ifdef RT2880
		if (IS_RBUS_INF(pAd))
		{
			pATEInfo->pChipStruct = &RALINK2880;
			DBGPRINT(RT_DEBUG_OFF, ("%s(): RALINK2880 hook !\n", __FUNCTION__));
			return NDIS_STATUS_SUCCESS;
		}
#endif /* RT2880 */
	}
#endif /* RT28xx */

#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
	if (IS_RT5390(pAd))
	{
			pATEInfo->pChipStruct = &RALINK5390;
		return NDIS_STATUS_SUCCESS;
	}
	else if (IS_RT5392(pAd))
	{
		pATEInfo->pChipStruct = &RALINK5392;
		return NDIS_STATUS_SUCCESS;
	}
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */

#ifdef RT33xx
	if (IS_RT3390(pAd))
	{
		pATEInfo->pChipStruct = &RALINK33xx;
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT33xx */

#ifdef RT30xx
	if (IS_RT30xx(pAd))
	{
		pATEInfo->pChipStruct = &RALINK30xx;
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT30xx */

#ifdef RT8592
	if (IS_RT8592(pAd))
	{
		pATEInfo->pChipStruct = &RALINK85592;
		return NDIS_STATUS_SUCCESS;
	}
#endif /* RT8592 */

#ifdef MT7601
	if ( IS_MT7601(pAd) )
	{
		pATEInfo->pChipStruct = &MT7601ATE;
	}
#endif /* MT7601 */
#ifdef MT7603
	pATEInfo->pChipStruct = &MT7603ATE;
#endif /* MT7601 */


	/* sanity check */
	if (pATEInfo->pChipStruct == &RALINKDefault)
	{
		/* Unknown chipset ! */
		DBGPRINT_ERR(("Warning - Unknown chipset !!!\n"));
		DBGPRINT_ERR(("MAC Version is %u\n", ((pAd->MACVersion & 0xFFFF0000) >> 16)));
		DBGPRINT_ERR(("Interface type is %d\n", pAd->infType));

		return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;
}


/*
==========================================================================
	Description:
		Initialize ATE_INFO structure.
		This routine is specific for ATE.

==========================================================================
*/
NDIS_STATUS ATEInit(
	IN	PRTMP_ADAPTER	pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
#ifdef HUAWEI_ATE
#ifdef CONFIG_AP_SUPPORT
	UINT byte = 0;
#endif /* CONFIG_AP_SUPPORT */
#endif /* HUAWEI_ATE */

	NdisZeroMemory(pATEInfo, sizeof(ATE_INFO));

	if (ChipStructAssign(pAd) != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("%s failed !\n", __FUNCTION__));
		return NDIS_STATUS_FAILURE;
	}
	
	OS_NdisAllocateSpinLock(&(pATEInfo->TssiSemLock));

	pATEInfo->Mode = ATE_STOP;
#ifdef RT3350
	pATEInfo->PABias = 0;
#endif /* RT3350  */
	pATEInfo->TxCount = 0xFFFFFFFF;
	pATEInfo->TxDoneCount = 0;
	pATEInfo->RFFreqOffset = 0;
#ifdef RT6352
	if (IS_RT6352(pAd))
		pATEInfo->bDPDEnable = FALSE;
#endif /* RT6352 */
#if defined(RT5370) || defined(RT5390) || defined(RT5592) || defined(RT3593) || defined(RT6352) || defined(RT65xx)
	if (IS_RT5392(pAd) || IS_RT5592(pAd) || IS_RT3593(pAd) || IS_RT6352(pAd) || IS_RT65XX(pAd))
		pATEInfo->Payload = 0xAA; 
	else
#endif /* defined(RT5370) || defined(RT5390) || defined(RT5592) || defined(RT3593) || defined(RT6352) || defined(RT65xx) */
		pATEInfo->Payload = 0xA5;/* to be backward compatible */
	pATEInfo->bFixedPayload = 1;
	pATEInfo->IPG = 200;/* 200 : sync with QA */	
	pATEInfo->TxLength = 1058;/* 1058 : sync with QA */
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
	{
		pATEInfo->TxWI.TXWI_N.BW = BW_20;
		pATEInfo->TxWI.TXWI_N.PHYMODE = MODE_OFDM;
		pATEInfo->TxWI.TXWI_N.MCS = 7;
		pATEInfo->TxWI.TXWI_N.ShortGI = 0;/* LONG GI : 800 ns*/
	}
#endif /* RLT_MAC*/
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		pATEInfo->TxWI.TXWI_O.BW = BW_20;
		pATEInfo->TxWI.TXWI_O.PHYMODE = MODE_OFDM;
		pATEInfo->TxWI.TXWI_O.MCS = 7;
		pATEInfo->TxWI.TXWI_O.ShortGI = 0;/* LONG GI : 800 ns*/
	}
#endif /* RTMP_MAC */
	/* please do not change this default channel value */
	pATEInfo->Channel = 1;
	pATEInfo->TxAntennaSel = 1;
	pATEInfo->RxAntennaSel = 0;

#ifdef RELEASE_EXCLUDE
#ifdef HUAWEI_ATE
#ifdef RT305x
#ifndef RT3350
	if (!IS_RT3350(pAd))
	/* This is supported just for RT3050 now */
	{
		USHORT value = 0;
		EEPROM_TX_PWR_STRUC	    Power;
		EEPROM_TX_PWR_STRUC	    Power2;

		/* Read frequency offset setting from E2P for ATE */
		RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);

		if ((value & 0x00FF) != 0x00FF)
			pATEInfo->RFFreqOffset = (UINT32) (value & 0x00FF);
		else
			pATEInfo->RFFreqOffset = 0;

		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX_PWR_OFFSET, Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX2_PWR_OFFSET, Power2.word);

		if ((Power.field.Byte1 > 31) || (Power.field.Byte1 < 0))
			pATEInfo->TxPower0 = DEFAULT_RF_TX_POWER;
		else
			pATEInfo->TxPower0 = Power.field.Byte1;

		if ((Power2.field.Byte1 > 31) || (Power2.field.Byte1 < 0))
			pATEInfo->TxPower1 = DEFAULT_RF_TX_POWER;
		else
			pATEInfo->TxPower1 = Power2.field.Byte1;				
	}
#endif /* !RT3350 */
#endif /* RT305x */
#endif /* HUAWEI_ATE */
#endif /* RELEASE_EXCLUDE */

#ifdef RTMP_MAC_PCI 
#ifdef RT35xx
	/* This is supposed just for rt3062 now. */
	if (IS_RT3572(pAd))
	{
		USHORT value = 0;
		EEPROM_TX_PWR_STRUC	    Power;
		EEPROM_TX_PWR_STRUC	    Power2;

		/* Read frequency offset setting from E2P for ATE */
		RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, value);

		if ((value & 0x00FF) != 0x00FF)
			pATEInfo->RFFreqOffset = (UINT32) (value & 0x00FF);
		else
			pATEInfo->RFFreqOffset = 0;

		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX_PWR_OFFSET, Power.word);
		RT28xx_EEPROM_READ16(pAd, EEPROM_G_TX2_PWR_OFFSET, Power2.word);

		if ((Power.field.Byte1 > 31) || (Power.field.Byte1 < 0))
			pATEInfo->TxPower0 = DEFAULT_RF_TX_POWER;
		else
			pATEInfo->TxPower0 = Power.field.Byte1;

		if ((Power2.field.Byte1 > 31) || (Power2.field.Byte1 < 0))
			pATEInfo->TxPower1 = DEFAULT_RF_TX_POWER;
		else
			pATEInfo->TxPower1 = Power2.field.Byte1;				
	}
#endif /* RT35xx */
#endif /* RTMP_MAC_PCI */
	pATEInfo->QID = QID_AC_BE;

#ifdef DOT11N_SS3_SUPPORT
	if (IS_RT2883(pAd) || IS_RT3883(pAd) || IS_RT3593(pAd))
	{
		/* For 3T/3R ++ */
		/* use broadcast address as default value */
		pATEInfo->Addr1[0] = 0xFF;
		pATEInfo->Addr1[1] = 0xFF;
		pATEInfo->Addr1[2] = 0xFF;
		pATEInfo->Addr1[3] = 0xFF;
		pATEInfo->Addr1[4] = 0xFF;
		pATEInfo->Addr1[5] = 0xFF;

		pATEInfo->Addr2[0] = 0x00;
		pATEInfo->Addr2[1] = 0x11;
		pATEInfo->Addr2[2] = 0x22;
		pATEInfo->Addr2[3] = 0xAA;
		pATEInfo->Addr2[4] = 0xBB;
		pATEInfo->Addr2[5] = 0xCC;

		NdisMoveMemory(pATEInfo->Addr3, pATEInfo->Addr2, MAC_ADDR_LEN);

		{		
			UINT32 data;

			data = 0xFFFFFFFF;
	    	RTMP_IO_WRITE32(pAd, 0x1044, data); 
	    	RTMP_IO_READ32(pAd, 0x1048, &data); 

	    	data = data | 0x0000FFFF;
	    	RTMP_IO_WRITE32(pAd, 0x1048, data); 
		}
		/* For stream mode in 3T/3R -- */
	}
	else
#endif /* DOT11N_SS3_SUPPORT */
	{
		pATEInfo->Addr1[0] = 0x00;
		pATEInfo->Addr1[1] = 0x11;
		pATEInfo->Addr1[2] = 0x22;
		pATEInfo->Addr1[3] = 0xAA;
		pATEInfo->Addr1[4] = 0xBB;
		pATEInfo->Addr1[5] = 0xCC;

		NdisMoveMemory(pATEInfo->Addr2, pATEInfo->Addr1, MAC_ADDR_LEN);
		NdisMoveMemory(pATEInfo->Addr3, pATEInfo->Addr1, MAC_ADDR_LEN);
	}
	pATEInfo->bRxFER = 0;
	pATEInfo->bQAEnabled = FALSE;
	pATEInfo->bQATxStart = FALSE;
	pATEInfo->bQARxStart = FALSE;
	pATEInfo->bAutoTxAlc = FALSE;
	pATEInfo->bLowTemperature = FALSE;
	pATEInfo->bAutoVcoCal = FALSE;
	pATEInfo->bTestTimeReduction = FALSE;
#ifdef RTMP_INTERNAL_TX_ALC
#if defined(RT3350) || defined(RT3352)
	if (IS_RT3350(pAd) || IS_RT3352(pAd))
	{
		pATEInfo->bTSSICalbrEnableG = FALSE;
		NdisZeroMemory((PUCHAR)&(pATEInfo->TssiRefPerChannel), CFG80211_NUM_OF_CHAN_2GHZ);
		NdisZeroMemory((PUCHAR)&(pATEInfo->TssiDeltaPerChannel), CFG80211_NUM_OF_CHAN_2GHZ);
	}
#endif /* defined(RT3350) || defined(RT3352) */
#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef IQ_CAL_SUPPORT
	pATEInfo->bIQCompEnable = FALSE;
#endif /* IQ_CAL_SUPPORT */
#ifdef RT5592
#ifdef RTMP_TEMPERATURE_COMPENSATION
	if (IS_RT5592(pAd))
	{
		pATEInfo->bTSSICalbrEnableG = FALSE;
		pATEInfo->bTSSICalbrEnableA = FALSE;
		/* TX 0 */
		NdisZeroMemory((PUCHAR)&(pATEInfo->TssiReadSampleG[0]), TSSI_READ_SAMPLE_NUM);
		NdisZeroMemory((PUCHAR)&(pATEInfo->TssiReadSampleA[0]), TSSI_READ_SAMPLE_NUM);
		/* TX 1 */
		NdisZeroMemory((PUCHAR)&(pATEInfo->TssiReadSampleG[1]), TSSI_READ_SAMPLE_NUM);
		NdisZeroMemory((PUCHAR)&(pATEInfo->TssiReadSampleA[1]), TSSI_READ_SAMPLE_NUM);
	}
#endif /* RTMP_TEMPERATURE_COMPENSATION */
#endif /* RT5592 */
	/* Default TXCONT/TXCARR/TXCARS mechanism is TX_METHOD_1 */
	pATEInfo->TxMethod = TX_METHOD_1;
	if ((IS_RT2070(pAd) || IS_RT2860(pAd) || IS_RT2872(pAd) || IS_RT2883(pAd)))
	{
		/* Early chipsets must be applied original TXCONT/TXCARR/TXCARS mechanism. */
		pATEInfo->TxMethod = TX_METHOD_0;
	}

	/* Power range is 0~31 in A band. */
	pATEInfo->MinTxPowerBandA = 0;
	pATEInfo->MaxTxPowerBandA = 31;
	if ((IS_RT2860(pAd)) || (IS_RT2872(pAd)) || (IS_RT2883(pAd)))
	{
		/* Power range of early chipsets is -7~15 in A band. */
		pATEInfo->MinTxPowerBandA = -7;
		pATEInfo->MaxTxPowerBandA = 15;
	}

#ifdef TXBF_SUPPORT
	pATEInfo->bTxBF = FALSE;	
#endif /* TXBF_SUPPORT */
#ifdef RTMP_MAC_PCI
	pATEInfo->bFWLoading = FALSE;
#endif /* RTMP_MAC_PCI */

#ifdef HUAWEI_ATE
#ifdef CONFIG_AP_SUPPORT
	for (byte = 0; byte < MAC_ADDR_LEN; byte++)
	{
		/* Default ATE MAC filter is 11:11:11:11:11:11. */
		pAd->ate.Filter[byte] = 0x11;
	}

	pATEInfo->bMACFilterEnable = FALSE;
#endif /* CONFIG_AP_SUPPORT */
#endif /* HUAWEI_ATE */

#ifdef RTMP_MAC_USB
#ifdef RT35xx
	if (IS_RT3572(pAd))
	{
		/* init as unsed value to ensure driver will set to MCU once */
		pATEInfo->LedIndicatorStrength = 0xFF;
		/* stored for hardware reset of BBP */
/*		pATEInfo->RfIcType = RFIC_3052; */
	}
#endif /* RT35xx */
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_QA
	pATEInfo->TxStatus = 0;
	RtmpOsTaskPidInit(&(pATEInfo->AtePid));
/*	pATEInfo->AtePid = THREAD_PID_INIT_VALUE; */
#ifdef DOT11_VHT_AC
	pATEInfo->vht_nss = 1;
#endif /* DOT11_VHT_AC */
#endif /* CONFIG_QA */
	pATEInfo->OneSecPeriodicRound = 0;
	pATEInfo->PeriodicRound = 0;

	return NDIS_STATUS_SUCCESS;
}


#ifdef CONFIG_QA
/*
==========================================================================
	Description:
		This routine is specific for ATE.
		When we start tx from QA GUI, it will modify BBP registers without
		notify ATE driver what the tx subtype is.

    Return:
        VOID
==========================================================================
*/
VOID ReadQATxTypeFromBBP(
	IN	PRTMP_ADAPTER	pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR   Bbp22Value = 0, Bbp24Value = 0;

#ifdef RT65xx
	if (IS_RT65XX(pAd))
	{
		UINT32 bbp_val;

		RTMP_BBP_IO_READ32(pAd, CORE_R24, &bbp_val);
		Bbp22Value = (bbp_val & 0xff00) >> 8;
	}
	else
#endif /* RT65xx */
	{
#ifdef RTMP_BBP
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &Bbp22Value);
#endif /* RTMP_BBP */
	}

	switch (Bbp22Value)
	{
		case BBP22_TXFRAME:
			{
#ifdef RTMP_MAC_PCI
				if (pATEInfo->TxCount == 0)
				{
					pATEInfo->TxCount = 0xFFFFFFFF;
				}
#endif /* RTMP_MAC_PCI */
				DBGPRINT(RT_DEBUG_TRACE,("START TXFRAME\n"));
				pATEInfo->bQATxStart = TRUE;
				Set_ATE_Proc(pAd, "TXFRAME");
			}
			break;

		case BBP22_TXCONT_OR_CARRSUPP:
			{
				DBGPRINT(RT_DEBUG_TRACE,("BBP22_TXCONT_OR_CARRSUPP\n"));
#ifdef RT65xx
				if (IS_RT65XX(pAd))
				{
					UINT32 bbp_val;

					RTMP_BBP_IO_READ32(pAd, TXC_R1, &bbp_val);
					Bbp24Value = (bbp_val & 0x01);
				}
				else
#endif /* RT65xx */
				{
#ifdef RTMP_BBP
				ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R24, &Bbp24Value);
#endif /* RTMP_BBP */
				}

				switch (Bbp24Value)
				{
					case BBP24_TXCONT:
						{
							DBGPRINT(RT_DEBUG_TRACE,("START TXCONT\n"));
							pATEInfo->bQATxStart = TRUE;

							if (pATEInfo->TxMethod == TX_METHOD_0)
							{
								Set_ATE_Proc(pAd, "TXCONT");
							}
						}
						break;

					case BBP24_CARRSUPP:
						{
							DBGPRINT(RT_DEBUG_TRACE,("START TXCARRSUPP\n"));
							pATEInfo->bQATxStart = TRUE;

							if (pATEInfo->TxMethod == TX_METHOD_0)
							{
								Set_ATE_Proc(pAd, "TXCARS");
							}
						}
						break;

					default:
						{
							DBGPRINT_ERR(("Unknown TX subtype !\n"));
						}
						break;
				}
			}
			break;	

		case BBP22_TXCARR:
			{
				DBGPRINT(RT_DEBUG_TRACE,("START TXCARR\n"));
				pATEInfo->bQATxStart = TRUE;

				if (pATEInfo->TxMethod == TX_METHOD_0)
				{
					Set_ATE_Proc(pAd, "TXCARR");
				}
			}
			break;							

		default:
			{
				DBGPRINT_ERR(("Unknown Start TX subtype !\n"));
			}
			break;
	}

	return;
}
#endif /* CONFIG_QA */

#ifdef RTMP_BBP
NDIS_STATUS ATEBBPWriteWithRxChain(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR bbpId,
	IN CHAR bbpVal,
	IN RX_CHAIN_IDX rx_ch_idx)
{
	UCHAR idx = 0, val = 0;

	if (((pAd->MACVersion & 0xffff0000) < 0x28830000) || 
		(pAd->Antenna.field.RxPath == 1))
	{
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, bbpId, bbpVal);
		return NDIS_STATUS_SUCCESS;
	}
	
	while (rx_ch_idx != 0)
	{
		if (idx >= pAd->Antenna.field.RxPath)
			break;
		
		if (rx_ch_idx & 0x01)
		{
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &val);
			val = (val & (~0x60)/* clear bit5 and bit6 */) | (idx << 5);
#ifdef RTMP_MAC_USB
			if (IS_USB_INF(pAd))
			{
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, val);
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, bbpId, bbpVal);
			}
#endif /* RTMP_MAC_USB */

#ifdef RTMP_MAC_PCI
			if (IS_PCI_INF(pAd) || IS_RBUS_INF(pAd))
			{
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, val);
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, bbpId, bbpVal);
			}
#endif /* RTMP_MAC_PCI */

			DBGPRINT(RT_DEBUG_INFO, 
					("%s(Idx):Write(R%d,val:0x%x) to Chain(0x%x, idx:%d)\n",
						__FUNCTION__, bbpId, bbpVal, rx_ch_idx, idx));
		}
		rx_ch_idx >>= 1;
		idx++;
	}

	return NDIS_STATUS_SUCCESS;
}
#endif /* RTMP_BBP */

#define SMM_BASEADDR                      0x4000
#define PKT_BASEADDR                      0x8000


#ifdef RLT_MAC
// TODO: shiang-6590, fix me, how about this register in RT85592?
#define PBF_CAP_CTRL	0x0440
#endif /* RLT_MAC */
INT Set_ADCDump_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		return FALSE;
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	// TODO: this function need to revise to make it more clear!
	if (pAd->chipCap.hif_type == HIF_RTMP)
	{
		UCHAR BBP_R21_Ori=0,BBP_R60_Ori=0,BBP_R142_ORI=0,BBP_R143_ORI=0;
		UINT32 MACValue=0,PBF_SYS_CTRL_ORI=0,PBF_CAP_CTRL_ORI=0;
		UINT32 CaptureModeOffset=0,CaptureStartAddr=0;
		UINT32 SMM_Addr;
		UINT32 PKT_Addr;
		int i = 0; 
		RTMP_STRING *src = "ADCDump.txt";
		RTMP_OS_FD				srcf;
		RTMP_OS_FS_INFO			osFSInfo;
		UCHAR				msg[128];
		UCHAR				msg1[128];
		CAPTURE_MODE_SHARE_MEMORY     SMMValued;
		CAPTURE_MODE_PACKET_BUFFER    PKTValue1d;
		CAPTURE_MODE_PACKET_BUFFER    PKTValue2d;
		UCHAR retval=0;
		UCHAR DataSourceADC6=simple_strtol(arg, 0, 10);
		
		pAd->ate.Mode = ATE_START;

		/* Disable Tx/Rx */
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x00);
		BBP_IO_READ8_BY_REG_ID(pAd, BBP_R21, &BBP_R21_Ori);

		/* Disable BBP power saving */
		   
		/* disable all Tx/Rx Queue */
		RTMP_IO_WRITE32(pAd, PBF_CFG, 0x00000000);

		/* capture mode */
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MACValue);
		PBF_SYS_CTRL_ORI=MACValue;
		MACValue |= 0x00004000; /* bit[14]=1 */
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, MACValue);

		/* capture setting */
		if (DataSourceADC6 == 1)
		{
			BBP_IO_READ8_BY_REG_ID(pAd, BBP_R60, &BBP_R60_Ori);                              
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R60, 0x80);
			BBP_IO_READ8_BY_REG_ID(pAd, BBP_R142, &BBP_R142_ORI);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R142, 0x10);
			BBP_IO_READ8_BY_REG_ID(pAd, BBP_R143, &BBP_R143_ORI);
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R143, 0x05);

			RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &MACValue);
			PBF_CAP_CTRL_ORI=MACValue;
			MACValue |= 0x00008000; /* set bit[15]=1 for ADC 6 */
			MACValue &= ~0x80000000; /* set bit[31]=0 */
			RTMP_IO_WRITE32(pAd, PBF_CAP_CTRL, MACValue);
		}
		else
		{
			RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &MACValue);
			PBF_CAP_CTRL_ORI=MACValue;
			MACValue &= ~0x80008000; /* set bit[31]=0, bit[15]=0 for ADC 8 */
			RTMP_IO_WRITE32(pAd, PBF_CAP_CTRL, MACValue);
		}

		/* trigger offset */
		RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &MACValue);
		MACValue &= ~(0x1FFF0000);
		RTMP_IO_WRITE32(pAd, PBF_CAP_CTRL, MACValue);
							
		if ((CaptureModeOffset > 0) && (CaptureModeOffset <= 0x1FFF))
		{
			RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &MACValue);
			MACValue |= CaptureModeOffset << 16;
			RTMP_IO_WRITE32(pAd, PBF_CAP_CTRL, MACValue);
		}

		/* start capture */
		RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &MACValue);
		MACValue = MACValue | 0x40000000; /* set bit[30]=1 */
		RTMP_IO_WRITE32(pAd, PBF_CAP_CTRL, MACValue);

		if (0)
		{
			/* start TX */
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x4);
		}
		else
		{
			/* start RX */
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x8);
		}
	                        
		/* Wait until [0x440] bit30=0 */
		do
		{
			i++;
			RtmpusecDelay(10);
			RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &MACValue);
			MACValue = MACValue & 0x40000000; /* bit[30] */

			if (MACValue == 0)
			{
				break;
			}

			if (i == 1000) /* 3 sec */
			{
				printk("Error, over 3s\n");
				break;
			}
		} while (MACValue != 0);

		if (DataSourceADC6 == 1)
		{
			/* restore BBP R60 */
			BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R60, BBP_R60_Ori);
		}

		/* Stop TX/RX */
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0);

		/* Read [0x440] bit[12:0] */
		RTMP_IO_READ32(pAd, PBF_CAP_CTRL, &CaptureStartAddr);
		CaptureStartAddr = CaptureStartAddr & 0x00001FFF;

		/* Dump data from MAC memory */
		RtmpOSFSInfoChange(&osFSInfo, TRUE);

		SMM_Addr=SMM_BASEADDR+CaptureStartAddr*2;
		PKT_Addr=PKT_BASEADDR+CaptureStartAddr*4;
		
		/* SMM Address must be four byte alignment*/
		SMM_Addr=(SMM_Addr/4)*4;

		/* open file */
		if (src && *src)
		{
			srcf = RtmpOSFileOpen(src, O_WRONLY|O_CREAT, 0);

			if (IS_FILE_OPEN_ERR(srcf)) 
			{
				DBGPRINT(RT_DEBUG_ERROR, ("--> Error opening %s\n", src));
				return FALSE;
			}
			else 
			{
				memset(msg, 0x00, 128);
				memset(msg1, 0x00, 128);

				for (i=0;i<0x1000;i++)
				{
					RTMP_IO_READ32(pAd,SMM_Addr, &SMMValued.Value);
					SMM_Addr += 4;

					if(SMM_Addr >= 0x8000)
						SMM_Addr = SMM_Addr - SMM_BASEADDR;

					RTMP_IO_READ32(pAd,PKT_Addr, &PKTValue1d.Value);
					PKT_Addr += 4;

					if(PKT_Addr >= 0x10000)
						PKT_Addr = PKT_Addr - PKT_BASEADDR;

					RTMP_IO_READ32(pAd,PKT_Addr, &PKTValue2d.Value);
					PKT_Addr += 4;

					if(PKT_Addr >= 0x10000)
						PKT_Addr = PKT_Addr - PKT_BASEADDR;

					sprintf(msg, "%d %d %d %d %d %d\n",SMMValued.field.LOW_BYTE1,SMMValued.field.LOW_BYTE0
					              ,PKTValue1d.field.BYTE3,PKTValue1d.field.BYTE2
					              ,PKTValue1d.field.BYTE1,PKTValue1d.field.BYTE0);
					sprintf(msg1, "%d %d %d %d %d %d\n",SMMValued.field.LOW_BYTE1,SMMValued.field.LOW_BYTE0
					              ,PKTValue2d.field.BYTE3,PKTValue2d.field.BYTE2
					              ,PKTValue2d.field.BYTE1,PKTValue2d.field.BYTE0);

					retval=RtmpOSFileWrite(srcf, (RTMP_STRING *)msg, strlen(msg));
					retval=RtmpOSFileWrite(srcf, (RTMP_STRING *)msg1, strlen(msg1));
				}           
			}
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("--> Error src  or srcf is null\n"));
			return FALSE;
		}

		retval=RtmpOSFileClose(srcf);
				
		if (retval)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("--> Error %d closing %s\n", -retval, src));
		}

		RtmpOSFSInfoChange(&osFSInfo, FALSE);

		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BBP_R21_Ori);
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R60, BBP_R60_Ori); 

		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R142, BBP_R142_ORI);
		BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R142, BBP_R142_ORI);
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, PBF_SYS_CTRL_ORI);
		RTMP_IO_WRITE32(pAd, PBF_CAP_CTRL, PBF_CAP_CTRL_ORI);

		/* Finish */
		/* normal mode */
		RTMP_IO_READ32(pAd, PBF_SYS_CTRL, &MACValue);
		MACValue &= ~0x00004000;
		RTMP_IO_WRITE32(pAd, PBF_SYS_CTRL, MACValue);

		/* reset packet buffer */
		RTMP_IO_WRITE32(pAd, PBF_CTRL,0x00000020 );

		/* enable Tx/Rx Queue */
		RTMP_IO_WRITE32(pAd, PBF_CFG, 0x00F40016);
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x0C);
		pAd->ate.Mode = ATE_STOP;

		return TRUE;
	}
#endif /* RTMP_MAC */

	return FALSE;
}


/* 100ms periodic execution */
VOID ATEPeriodicExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	PRTMP_ADAPTER pAd = (RTMP_ADAPTER *)FunctionContext;
	PATE_INFO pATEInfo = &(pAd->ate);
	
	if (ATE_ON(pAd))
	{
		pATEInfo->PeriodicRound++;
#ifdef MT76x0
		if (IS_MT7610(pAd))
		{
			if ((pATEInfo->bQAEnabled == FALSE) && (pATEInfo->bAutoTxAlc == TRUE)
					&& (pATEInfo->Mode == ATE_TXFRAME))
			{
				ATEAsicAdjustTxPower(pAd);
			}
			ATEAsicExtraPowerOverMAC(pAd);
		}
#endif /* MT76x0 */
#ifdef MT76x2
		if ( IS_MT76x2(pAd) )
		{
			if ( (pATEInfo->Mode == ATE_TXFRAME) && (pATEInfo->bAutoTxAlc == TRUE) )			{
				ATEAsicAdjustTxPower(pAd);
			}
		}
#endif /* MT76x2 */
#ifdef RT6352
		if (IS_RT6352(pAd))
		{
#ifdef RTMP_TEMPERATURE_CALIBRATION
			if ((pATEInfo->bAutoTxAlc == FALSE) && (pATEInfo->bQAEnabled == FALSE))
			{
				if (pATEInfo->bLowTemperature == TRUE)
				{
					INT32 wait;
					CHAR BbpR49;			
					UINT8 BBPValue;

					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);

					if (!(BBPValue & (1<<4)))
					{
						BBPValue |= (1<<4);
						ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BBPValue);		
					}

					/* polling BBP_R47 until BBP R47[4]==0 */
					for (wait=0; wait<200; wait++)
					{
						RtmpusecDelay(200);
						ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);

						if (!(BBPValue & (1<<4)))
						{
							break;
						}
					}

					if ((BBPValue & 0x10) != 0)
						return;

					/* set BBP R47[3:0]=4 */
					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);
					BBPValue &= ~(0x0F);
					BBPValue |= (0x04);
					ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BBPValue);

					/* fetch temperature reading */
					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpR49);
					pAd->CurrTemperature = (INT32)BbpR49;
					DBGPRINT(RT_DEBUG_INFO, ("%s: Current Temperature from BBP_R49=0x%x\n", __FUNCTION__, pAd->CurrTemperature));
				}
			}
			else
#endif /* RTMP_TEMPERATURE_CALIBRATION */
			if ((pATEInfo->bAutoTxAlc == TRUE) && (pATEInfo->Mode == ATE_TXFRAME))
			{
				OS_SEM_LOCK(&(pATEInfo->TssiSemLock));
				/* For MT7620, let QA come in for TSSI */
				ATEAsicAdjustTxPower(pAd);
				OS_SEM_UNLOCK(&(pATEInfo->TssiSemLock));	
				ATEAsicExtraPowerOverMAC(pAd);
#ifdef RT6352_EP_SUPPORT
				if (pAd->bExtPA == FALSE)
				{
#endif /* RT6352_EP_SUPPORT */
					if (pATEInfo->bDPDEnable == TRUE)
					{
						if (pATEInfo->TxAntennaSel == ANT_0) /* TX0 */
							Set_TestDPDCalibrationTX0_Proc(pAd, "9");
						else if (pATEInfo->TxAntennaSel == ANT_1) /* TX1 */
							Set_TestDPDCalibrationTX1_Proc(pAd, "9");
						else /* both TX */
							Set_TestDPDCalibration_Proc(pAd, "9"); /* both TX */
					}
#ifdef RT6352_EP_SUPPORT
				}
#endif /* RT6352_EP_SUPPORT */
			}
#ifdef RTMP_TEMPERATURE_CALIBRATION
			if (pATEInfo->bQAEnabled == FALSE)
			{
				if (pATEInfo->bLowTemperature == TRUE)
				{
					RT6352_TemperatureCalibration(pAd);
				}
			}
#endif /* RTMP_TEMPERATURE_CALIBRATION */
		}
#endif /* RT6352 */

		/* Normal 1 second ATE PeriodicExec.*/
		if (pATEInfo->PeriodicRound % (ATE_TASK_EXEC_MULTIPLE) == 0)
		{
			pATEInfo->OneSecPeriodicRound++;

			/* for performace enchanement */
			NdisZeroMemory(&pAd->RalinkCounters,
							(UINT32)&pAd->RalinkCounters.OneSecEnd -
							(UINT32)&pAd->RalinkCounters.OneSecStart);
#ifdef RELEASE_EXCLUDE
			/* request from Baron : move this routine from later to here */
			/* for showing Rx error count in ATE RXFRAME */
#endif /* RELEASE_EXCLUDE */
			NICUpdateRawCounters(pAd);

			if (pATEInfo->bRxFER == 1)
			{
				pATEInfo->RxTotalCnt += pATEInfo->RxCntPerSec;
				ate_print(KERN_EMERG "ATEPeriodicExec: Rx packet cnt = %d/%d\n",
				pATEInfo->RxCntPerSec, pATEInfo->RxTotalCnt);
				pATEInfo->RxCntPerSec = 0;

				if (pATEInfo->RxAntennaSel == 0)
					ate_print(KERN_EMERG "ATEPeriodicExec: Rx AvgRssi0=%d, AvgRssi1=%d, AvgRssi2=%d\n\n",
						pATEInfo->AvgRssi0, pATEInfo->AvgRssi1, pATEInfo->AvgRssi2);
				else
					ate_print(KERN_EMERG "ATEPeriodicExec: Rx AvgRssi=%d\n\n", pATEInfo->AvgRssi0);
			}

			MlmeResetRalinkCounters(pAd);

			/* In QA Mode, QA will handle all registers. */
			if (pATEInfo->bQAEnabled == TRUE)
			{
				return;
			}

			if ((!IS_RT6352(pAd)) && (!IS_MT76x0(pAd)) &&  (!IS_MT76x2(pAd)))
			{
				/* MT7620 and MT7610E have adjusted Tx power above */
				if ((pATEInfo->bAutoTxAlc == TRUE) && (pATEInfo->Mode == ATE_TXFRAME))
					ATEAsicAdjustTxPower(pAd);
			
				ATEAsicExtraPowerOverMAC(pAd);
			}

			/* only for MT7601 so far */
			ATEAsicTemperCompensation(pAd);

			/* do VCO calibration per four seconds */
			if (pATEInfo->PeriodicRound % (ATE_TASK_EXEC_MULTIPLE * 4) == 0)
			{
#ifdef VCORECAL_SUPPORT
				if ((pAd->ate.bAutoVcoCal == TRUE) && (pATEInfo->Mode == ATE_TXFRAME))
				{
#ifdef MT76x0
					if (IS_MT76x0(pAd))
					{
						MT76x0ATE_VCO_CalibrationMode3(pAd);
					}
					else
#endif /* MT76x0 */
					{
#ifdef RT3593
						if (IS_RT3593(pAd))
						{
							ATEAsicVCORecalibration(pAd);
						}
#endif /* RT3593 */
					}
				}
#endif /* VCORECAL_SUPPORT */
			}

#ifdef RELEASE_EXCLUDE
			/*
				Add the following codes to fix high-power issue under ATE mode.
			*/
#endif /* RELEASE_EXCLUDE */
#if (defined(RT3052) && !defined(RT3352)) || defined(RT3070)
			/* request by Gary, if Rssi0 > -42, BBP 82 need to be changed from 0x62 to 0x42, , bbp 67 need to be changed from 0x20 to 0x18*/
			if ((!pAd->CommonCfg.HighPowerPatchDisabled)
#ifdef RT3052
				&& (IS_RT3052B(pAd))
#endif /* RT3052 */
			)
			{
#ifdef RELEASE_EXCLUDE
				/*
					high-power issue:
					request by Brain, 2070/3070/305x AP/Station is too close to receive the stations/APs signal.
					For fixing this bug, we dynamically tune the RF power(controlled by RF_R27).
					The tuning rules are
					1. If the connecting devices is too close, we will attenuate the RF power to 0V.
					2.Otherwise, recover the RF power to original level (0.15V)
				*/
#endif /* RELEASE_EXCLUDE */
#ifdef RT3070
				if ((IS_RT3070(pAd) && ((pAd->MACVersion & 0xffff) < 0x0201)))
#endif /* RT3070 */
				{
#ifdef RTMP_RF
					if ((pATEInfo->AvgRssi0 != 0) && (pATEInfo->AvgRssi0 > (pAd->BbpRssiToDbmDelta - 35))) 
					{ 
						RT30xxWriteRFRegister(pAd, RF_R27, 0x20); 
					} 
					else 
					{ 
						RT30xxWriteRFRegister(pAd, RF_R27, 0x23); 
					}
#endif /* RTMP_RF */
				}

#ifdef RT3052
				if (pAd->Antenna.field.RxPath == 2)
				{
					if ((pATEInfo->AvgRssi0 != 0) && (pATEInfo->AvgRssi0 > (pAd->BbpRssiToDbmDelta - 42) ))
					{
						ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x42);
						ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x18);
					}
					else
					{
						ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
						ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, 0x20);
					}
				}
#endif /* RT3052 */
			}
#endif /* RT305x */
		}
	}
	else
	{
		DBGPRINT_ERR(("%s is NOT called in ATE mode.\n", __FUNCTION__));
	}

	return;
}

