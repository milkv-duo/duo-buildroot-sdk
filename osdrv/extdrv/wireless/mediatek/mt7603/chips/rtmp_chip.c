/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt_chip.c

	Abstract:
	Ralink Wireless driver CHIP related functions

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/


#include "rt_config.h"


BOOLEAN rt28xx_eeprom_read16(RTMP_ADAPTER *pAd, USHORT offset, USHORT *value) 
{
    if (pAd->chipOps.eeread) {
        return pAd->chipOps.eeread(pAd, offset, value);
    } else {
        return FALSE;
    }    
}


/*
========================================================================
Routine Description:
	write high memory.
	if firmware do not support auto high/low memory switching, we should switch to high memory by ourself.

Arguments:
	pAd				- WLAN control block pointer
	Offset			- Memory offsets
	Value			- Written value
	Unit				- Unit in "Byte"

Return Value:
	None

Note:
========================================================================
*/
VOID RtmpChipWriteHighMemory(
	IN RTMP_ADAPTER *pAd,
	IN USHORT Offset,
	IN UINT32 Value,
	IN UINT8 Unit)
{
#ifdef RTMP_MAC_USB
	switch(Unit)
	{
		case 1:
#ifdef MT_MAC
#else
			RTUSBSingleWrite(pAd, Offset, Value, TRUE);
#endif
			break;
		case 2:
		{
#ifdef MT_MAC
#else
			UINT16 ShortVal = (UINT16)Value;
			RTUSBMultiWrite(pAd, Offset, (UCHAR *) &ShortVal, 2, TRUE);
#endif
			break;
		}
		case 4:
#ifdef MT_MAC
#else
			RTUSBWriteMACRegister(pAd, Offset, Value, TRUE);
#endif
		default:
			break;
	}
#endif /* RTMP_MAC_USB */
#ifdef RTMP_MAC_PCI
#ifdef SPECIFIC_BCN_BUF_SUPPORT
unsigned long irqFlag = 0;
	RTMP_MAC_SHR_MSEL_LOCK(pAd, HIGHER_SHRMEM, irqFlag);
	RtmpChipWriteMemory(pAd, Offset, Value, Unit);
	RTMP_MAC_SHR_MSEL_UNLOCK(pAd, LOWER_SHRMEM, irqFlag);
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#endif /* RTMP_MAC_PCI */
}


/*
========================================================================
Routine Description:
	write memory

Arguments:
	pAd				- WLAN control block pointer
	Offset			- Memory offsets
	Value			- Written value
	Unit				- Unit in "Byte"
Return Value:
	None

Note:
========================================================================
*/
VOID RtmpChipWriteMemory(
	IN	RTMP_ADAPTER	*pAd,
	IN	USHORT			Offset,
	IN	UINT32			Value,
	IN	UINT8			Unit)
{
	switch(Unit)
	{
		case 1:
			RTMP_IO_WRITE8(pAd, Offset, Value);
			break;
		case 2:
			RTMP_IO_WRITE16(pAd, Offset, Value);
			break;
		case 4:
			RTMP_IO_WRITE32(pAd, Offset, Value);
		default:
			break;
	}
}


/*
========================================================================
Routine Description:
	Initialize specific beacon frame architecture.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RtmpChipBcnSpecInit(RTMP_ADAPTER *pAd)
{
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;


	pChipCap->FlgIsSupSpecBcnBuf = TRUE;
	pChipCap->BcnMaxHwNum = 16;
	pChipCap->WcidHwRsvNum = 255;

/* 	In 16-MBSS support mode, if AP-Client is enabled, 
	the last 8-MBSS would be occupied for AP-Client using. */
#ifdef APCLI_SUPPORT
	pChipCap->BcnMaxNum = (8 - MAX_MESH_NUM);
#else
	pChipCap->BcnMaxNum = (16 - MAX_MESH_NUM);
#endif /* APCLI_SUPPORT */

	pChipCap->BcnMaxHwSize = 0x2000;

	/* It's allowed to use the higher(secordary) 8KB shared memory */
	pChipCap->BcnBase[0] = 0x4000;
	pChipCap->BcnBase[1] = 0x4200;
	pChipCap->BcnBase[2] = 0x4400;
	pChipCap->BcnBase[3] = 0x4600;
	pChipCap->BcnBase[4] = 0x4800;
	pChipCap->BcnBase[5] = 0x4A00;
	pChipCap->BcnBase[6] = 0x4C00;
	pChipCap->BcnBase[7] = 0x4E00;
	pChipCap->BcnBase[8] = 0x5000;
	pChipCap->BcnBase[9] = 0x5200;
	pChipCap->BcnBase[10] = 0x5400;
	pChipCap->BcnBase[11] = 0x5600;
	pChipCap->BcnBase[12] = 0x5800;
	pChipCap->BcnBase[13] = 0x5A00;
	pChipCap->BcnBase[14] = 0x5C00;
	pChipCap->BcnBase[15] = 0x5E00;

	pAd->chipOps.BeaconUpdate = RtmpChipWriteHighMemory;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
}


/*
========================================================================
Routine Description:
	Initialize normal beacon frame architecture.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RtmpChipBcnInit(
	IN RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;


	pChipCap->FlgIsSupSpecBcnBuf = FALSE;
	pChipCap->BcnMaxHwNum = 8;
	pChipCap->BcnMaxNum = (pChipCap->BcnMaxHwNum - MAX_MESH_NUM - MAX_APCLI_NUM);
	pChipCap->BcnMaxHwSize = 0x1000;

	pChipCap->BcnBase[0] = 0x7800;
	pChipCap->BcnBase[1] = 0x7A00;
	pChipCap->BcnBase[2] = 0x7C00;
	pChipCap->BcnBase[3] = 0x7E00;
	pChipCap->BcnBase[4] = 0x7200;
	pChipCap->BcnBase[5] = 0x7400;
	pChipCap->BcnBase[6] = 0x5DC0;
	pChipCap->BcnBase[7] = 0x5BC0;

	/*
		If the MAX_MBSSID_NUM is larger than 6,
		it shall reserve some WCID space(wcid 222~253) for beacon frames.
		-	these wcid 238~253 are reserved for beacon#6(ra6).
		-	these wcid 222~237 are reserved for beacon#7(ra7).
	*/
	if (pChipCap->BcnMaxNum == 8)
		pChipCap->WcidHwRsvNum = 222;
	else if (pChipCap->BcnMaxNum == 7)
		pChipCap->WcidHwRsvNum = 238;
	else
		pChipCap->WcidHwRsvNum = 255;

	pAd->chipOps.BeaconUpdate = RtmpChipWriteMemory;
}


#ifdef MT_MAC
VOID mt_bcn_buf_init(RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;

	pChipCap->FlgIsSupSpecBcnBuf = FALSE;
	pChipCap->BcnMaxHwNum = 16;
	pChipCap->BcnMaxNum = 16;

#if defined(MT7603_FPGA) || defined(MT7628_FPGA) || defined(MT7636_FPGA)
	pChipCap->WcidHwRsvNum = 20;
#else
	pChipCap->WcidHwRsvNum = 127;
#endif /* MT7603_FPGA */
	pChipCap->BcnMaxHwSize = 0x2000;  // useless!!

	pChipCap->BcnBase[0] = 0;
	pChipCap->BcnBase[1] = 0;
	pChipCap->BcnBase[2] = 0;
	pChipCap->BcnBase[3] = 0;
	pChipCap->BcnBase[4] = 0;
	pChipCap->BcnBase[5] = 0;
	pChipCap->BcnBase[6] = 0;
	pChipCap->BcnBase[7] = 0;
	pChipCap->BcnBase[8] = 0;
	pChipCap->BcnBase[9] = 0;
	pChipCap->BcnBase[10] = 0;
	pChipCap->BcnBase[11] = 0;
	pChipCap->BcnBase[12] = 0;
	pChipCap->BcnBase[13] = 0;
	pChipCap->BcnBase[14] = 0;
	pChipCap->BcnBase[15] = 0;

#ifdef CONFIG_MULTI_CHANNEL
	/* Record HW Null Frame offset */
	pAd->NullBufOffset[0] = 0;
	pAd->NullBufOffset[1] = 0;
#endif /* CONFIG_MULTI_CHANNEL */

	pAd->chipOps.BeaconUpdate = NULL;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}

}
#endif /* MT_MAC */


#ifdef RLT_MAC
/*
========================================================================
Routine Description:
	Initialize specific beacon frame architecture.

Arguments:
	pAd				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID rlt_bcn_buf_init(RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;

	pChipCap->FlgIsSupSpecBcnBuf = FALSE;
#if defined(MT7601) || defined(MT76x2)
	if (IS_MT7601(pAd) || IS_MT76x2(pAd)) 
	{
		pChipCap->BcnMaxHwNum = 8;
		pChipCap->WcidHwRsvNum = 127;
	}
	else
#endif /* MT7601 || MT76x2 */
	{
		pChipCap->BcnMaxHwNum = 16;
		pChipCap->WcidHwRsvNum = 255;
	}

/*
	In 16-MBSS support mode, if AP-Client is enabled, 
	the last 8-MBSS would be occupied for AP-Client using.
*/
#ifdef APCLI_SUPPORT
	pChipCap->BcnMaxNum = (8 - MAX_MESH_NUM);
#else
	pChipCap->BcnMaxNum = (pChipCap->BcnMaxHwNum - MAX_MESH_NUM);
#endif /* APCLI_SUPPORT */

	pChipCap->BcnMaxHwSize = 0x2000;

	pChipCap->BcnBase[0] = 0xc000;
	pChipCap->BcnBase[1] = 0xc200;
	pChipCap->BcnBase[2] = 0xc400;
	pChipCap->BcnBase[3] = 0xc600;
	pChipCap->BcnBase[4] = 0xc800;
	pChipCap->BcnBase[5] = 0xca00;
	pChipCap->BcnBase[6] = 0xcc00;
	pChipCap->BcnBase[7] = 0xce00;
	pChipCap->BcnBase[8] = 0xd000;
	pChipCap->BcnBase[9] = 0xd200;
	pChipCap->BcnBase[10] = 0xd400;
	pChipCap->BcnBase[11] = 0xd600;
	pChipCap->BcnBase[12] = 0xd800;
	pChipCap->BcnBase[13] = 0xda00;
	pChipCap->BcnBase[14] = 0xdc00;
	pChipCap->BcnBase[15] = 0xde00;

#ifdef CONFIG_MULTI_CHANNEL
	/* Record HW Null Frame offset */
	//pAd->NullBufOffset[0] = 0xd000;
	pAd->NullBufOffset[0] = 0xd000;
	pAd->NullBufOffset[1] = 0xd200;
#endif /* CONFIG_MULTI_CHANNEL */

	pAd->chipOps.BeaconUpdate = RtmpChipWriteMemory;
}
#endif /* RLT_MAC */


#ifdef IQ_CAL_SUPPORT
static UCHAR IQCalValue[IQ_CAL_CHANNEL_GROUP_NUM][IQ_CAL_CHAIN_NUM][IQ_CAL_TYPE_NUM];

VOID GetIQCalibration(IN PRTMP_ADAPTER pAd)
{
	UINT16 E2PValue;	
	
	/* 2G IQ Calibration for TX0 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX0_2G, E2PValue);
	IQCalValue[IQ_CAL_2G][IQ_CAL_TX0][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX0_2G, E2PValue);
	IQCalValue[IQ_CAL_2G][IQ_CAL_TX0][IQ_CAL_PHASE] = E2PValue & 0x00FF;
	
	/* 2G IQ Calibration for TX1 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX1_2G, E2PValue);
	IQCalValue[IQ_CAL_2G][IQ_CAL_TX1][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX1_2G, E2PValue);
	IQCalValue[IQ_CAL_2G][IQ_CAL_TX1][IQ_CAL_PHASE] = E2PValue & 0x00FF;

#ifdef A_BAND_SUPPORT
	/* 5G IQ Calibration Value for TX0 of Ch36~Ch64 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX0_CH36_TO_CH64_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP1_5G][IQ_CAL_TX0][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX0_CH36_TO_CH64_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP1_5G][IQ_CAL_TX0][IQ_CAL_PHASE] = E2PValue & 0x00FF;

	/* 5G IQ Calibration Value for TX1 of Ch36~Ch64 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX1_CH36_TO_CH64_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP1_5G][IQ_CAL_TX1][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX1_CH36_TO_CH64_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP1_5G][IQ_CAL_TX1][IQ_CAL_PHASE] = E2PValue & 0x00FF;

	/* 5G IQ Calibration Value for TX0 of Ch100~Ch138 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX0_CH100_TO_CH138_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP2_5G][IQ_CAL_TX0][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX0_CH100_TO_CH138_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP2_5G][IQ_CAL_TX0][IQ_CAL_PHASE] = E2PValue & 0x00FF;

	/* 5G IQ Calibration Value for TX1 of Ch100~Ch138 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX1_CH100_TO_CH138_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP2_5G][IQ_CAL_TX1][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX1_CH100_TO_CH138_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP2_5G][IQ_CAL_TX1][IQ_CAL_PHASE] = E2PValue & 0x00FF;

	/* 5G IQ Calibration Value for TX0 of Ch140~Ch165 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX0_CH140_TO_CH165_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP3_5G][IQ_CAL_TX0][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX0_CH140_TO_CH165_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP3_5G][IQ_CAL_TX0][IQ_CAL_PHASE] = E2PValue & 0x00FF;

	/* 5G IQ Calibration Value for TX1 of Ch140~Ch165 */
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_GAIN_CAL_TX1_CH140_TO_CH165_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP3_5G][IQ_CAL_TX1][IQ_CAL_GAIN] = E2PValue & 0x00FF;
	RT28xx_EEPROM_READ16(pAd, EEPROM_IQ_PHASE_CAL_TX1_CH140_TO_CH165_5G, E2PValue);
	IQCalValue[IQ_CAL_GROUP3_5G][IQ_CAL_TX1][IQ_CAL_PHASE] = E2PValue & 0x00FF;
	
#endif /* A_BAND_SUPPORT */
}


inline UCHAR IQCal(
	IN enum IQ_CAL_CHANNEL_INDEX 	ChannelIndex,
	IN enum IQ_CAL_TXRX_CHAIN 		TxRxChain,
	IN enum IQ_CAL_TYPE 				IQCalType)
{
	if (IQCalValue[ChannelIndex][TxRxChain][IQCalType] != 0xFF)
	return IQCalValue[ChannelIndex][TxRxChain][IQCalType];
	else
		return 0;
}
#endif /* IQ_CAL_SUPPORT */


#ifdef HW_ANTENNA_DIVERSITY_SUPPORT
UINT32 SetHWAntennaDivsersity(
	IN PRTMP_ADAPTER		pAd,
	IN BOOLEAN				Enable)
{
	if (Enable == TRUE)
	{
		UINT8 BBPValue = 0, RFValue = 0;
		USHORT value;

		// RF_R29 bit7:6
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_GAIN, value);
		
		RT30xxReadRFRegister(pAd, RF_R29, &RFValue);
		RFValue &= 0x3f; // clear bit7:6
		RFValue |= (value << 6);
		RT30xxWriteRFRegister(pAd, RF_R29, RFValue);

		// BBP_R47 bit7=1
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);
		BBPValue |= 0x80;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BBPValue);
	
		BBPValue = 0xbe;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, BBPValue);
		BBPValue = 0xb0;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, BBPValue);
		BBPValue = 0x23;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, BBPValue);
		BBPValue = 0x3a;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R153, BBPValue);
		BBPValue = 0x10;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R154, BBPValue);
		BBPValue = 0x3b;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R155, BBPValue);
		BBPValue = 0x04;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R253, BBPValue);

		DBGPRINT(RT_DEBUG_TRACE, ("HwAnDi> Enable!\n"));
	}
	else
	{
		UINT8 BBPValue = 0;

		/*
			main antenna: BBP_R152 bit7=1
			aux antenna: BBP_R152 bit7=0
		 */
		if (pAd->FixDefaultAntenna == 0)
		{
			/* fix to main antenna */
			/* do not care BBP R153, R155, R253 */
			BBPValue = 0x3e;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, BBPValue);
			BBPValue = 0x30;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, BBPValue);
			BBPValue = 0x23;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, BBPValue);
			BBPValue = 0x00;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R154, BBPValue);
		}
		else
		{
			/* fix to aux antenna */
			/* do not care BBP R153, R155, R253 */
			BBPValue = 0x3e;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, BBPValue);
			BBPValue = 0x30;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, BBPValue);
			BBPValue = 0xa3;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, BBPValue);
			BBPValue = 0x00;
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R154, BBPValue);
		}

		DBGPRINT(RT_DEBUG_TRACE, ("HwAnDi> Disable!\n"));
	}

	return 0;
}
#endif // HW_ANTENNA_DIVERSITY_SUPPORT // 


#ifdef ANT_DIVERSITY_SUPPORT
#ifdef RELEASE_EXCLUDE
	/* HWAntennaDiversityEnable (PPAD) for  RT5350/RT5370G/RT5390R */
#endif
VOID HWAntennaDiversityEnable(RTMP_ADAPTER *pAd)
{
#if defined(RT5350)
	UINT8 rt5350regs[7] = { 0xC0, 0xB0, 0x23, 0x34, 0x10, 0x3B, 0x05 };
#elif defined(RT5370) || defined(RT5390)
	UINT8 rt5390regs[7] = { 0xBE, 0xAE, 0x20, 0x34, 0x40, 0x3B, 0x04 };
#endif
#ifdef MT7601
	UINT8 mt7601regs[7] = { 0xD2, 0x32, 0x23, 0x41, 0x00, 0x4F, 0x7E };
#endif
	UINT8 *regs;
	UINT8 BBPValue = 0, RFValue = 0;

#if defined(RT5370) || defined(RT5390)
	if ( IS_RT5390(pAd) )
		regs = rt5390regs;
#endif

#ifdef RT5350
	if (IS_RT5350(pAd))
		regs = rt5350regs;
#endif

#ifdef MT7601
	if ( IS_MT7601(pAd) )
		regs = mt7601regs;
#endif

#if defined(RT5350) || defined(RT5370) || defined(RT5390)
	if ( IS_RT5350(pAd) || IS_RT5390(pAd) )
	{
		/* RF_R29 bit[7:6] = b'11 */
		RT30xxReadRFRegister(pAd, RF_R29, &RFValue);
		RFValue |= 0xC0; /* rssi_gain */
		RT30xxWriteRFRegister(pAd, RF_R29, RFValue);
	}
#endif /* defined(RT5350) || defined(RT5370) || defined(RT5390) */

	/* BBP_R47 bit7=1 */
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BBPValue);
	BBPValue |= 0x80; /* ADC6 on */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BBPValue);

#ifdef MT7601
	if ( IS_MT7601(pAd) )
	{
		/* BBP_R60 bit7=1 */
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R60, &BBPValue);
		BBPValue |= 0x80; /* RSSI debug on */
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R60, BBPValue);
	}
#endif /* MT7601 */

	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R150, regs[0]); /* ENABLE_ANTSW_OFDM and RSSI_ANTSWT */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R151, regs[1]); /* ENABLE_ANTSW_CCK and RSSI_LNASWTH_HM */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R152, regs[2]); /* RSSI_LNASWTH_HL */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R153, regs[3]); /* RSSI_ANALOG_LOWTH */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R154, regs[4]); /* ANTSW_PWROFFSET, ANTSW_DELAYOFFSET and auto-control BBP R152[7] (RX_DEFAULT_ANT) */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R155, regs[5]); /* RSSI_OFFSET */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R253, regs[6]); /* MEASURE_RSSI_OFFSET */

#ifdef MT7601
	if ( IS_MT7601(pAd) )
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R195, 0x30);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R196, 0x32);
		
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R195, 0x31);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R196, 0x23);

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R195, 0x32);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R196, 0x45);

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R195, 0x35);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R196, 0x4a);

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R195, 0x36);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R196, 0x5a);

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R195, 0x37);
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R196, 0x5a);
	}
#endif /* MT7601 */

	DBGPRINT(RT_DEBUG_OFF, ("HwAnDiv --> Enable!\n"));
}
#endif /* ANT_DIVERSITY_SUPPORT */ 


UINT8 NICGetBandSupported(RTMP_ADAPTER *pAd)
{
	if (BOARD_IS_5G_ONLY(pAd))
	{
		return RFIC_5GHZ;
	}
	else if (BOARD_IS_2G_ONLY(pAd))
	{
		return RFIC_24GHZ;
	}
	else if (RFIC_IS_5G_BAND(pAd))
	{
		return RFIC_DUAL_BAND;		
	}
	else
		return RFIC_24GHZ;
}


INT WaitForAsicReady(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val = 0;
#if defined(RTMP_MAC) || defined(RLT_MAC)
	UINT32 reg;
	int idx = 0;
#endif

	// TODO: shiang-7603
	return TRUE;
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return TRUE;
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	reg = MAC_CSR0;
#ifdef RT3290	
	if (IS_RT3290(pAd))
		reg = ASIC_VERSION;
#endif /* RT3290 */
	do
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return FALSE;
		
		RTMP_IO_READ32(pAd, reg, &mac_val);
		if ((mac_val != 0x00) && (mac_val != 0xFFFFFFFF))
			return TRUE;

		RtmpOsMsDelay(5);
	} while (idx++ < 500);

#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

	DBGPRINT(RT_DEBUG_ERROR,
				("%s(0x%x):AsicNotReady!\n",
				__FUNCTION__, mac_val));
	
	return FALSE;
}


INT AsicGetMacVersion(RTMP_ADAPTER *pAd)
{
	UINT32 reg = 0;

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
		return FALSE;
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	reg = MAC_CSR0;
#ifdef RT3290
	if (IS_RT3290(pAd))
		reg = 0x0;
#endif /* RT3290 */

#ifdef RT65xx
	if (IS_RT65XX(pAd))
		RTMP_IO_READ32(pAd, ASIC_VERSION, &pAd->ChipID);
#endif /* RT65xx */
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

	if (WaitForAsicReady(pAd) == TRUE)
	{
		RTMP_IO_READ32(pAd, reg, &pAd->MACVersion);
		DBGPRINT(RT_DEBUG_OFF, ("MACVersion[Ver:Rev]=0x%08x : 0x%08x\n",
					pAd->MACVersion, pAd->ChipID));
		return TRUE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s() failed!\n", __FUNCTION__));
		return FALSE;
	}
}


/*
========================================================================
Routine Description:
	Initialize chip related information.

Arguments:
	pCB				- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
int RtmpChipOpsHook(VOID *pCB)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pCB;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;
#if defined(RTMP_MAC) || defined(RLT_MAC) || defined(RT65xx)
	UINT32 MacValue;
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) || defined(RT65xx) */
	int ret = 0;
#ifdef GREENAP_SUPPORT
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
#endif /* GREENAP_SUPPORT */

	/* sanity check */
	if (WaitForAsicReady(pAd) == FALSE)
		return -1;

	// TODO: shiang-7603
	if (IS_MT7603(pAd) || IS_MT7628(pAd)) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));
	}
	else
	{
#if defined(RTMP_MAC) || defined(RLT_MAC)
		RTMP_IO_READ32(pAd, MAC_CSR0, &MacValue);
		pAd->MACVersion = MacValue;
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */
	}

	if (pAd->MACVersion == 0xffffffff)
		return -1;

#ifdef RT65xx
	if (IS_RT65XX(pAd)) {
		RTMP_IO_READ32(pAd, ASIC_VERSION, &MacValue);
		pAd->ChipID = MacValue;

		if (pAd->ChipID == 0xffffffff)
			return -1;
	}
#endif /* RT65xx */

	/* default init */
	RTMP_DRS_ALG_INIT(pAd, RATE_ALG_LEGACY);

#ifdef RTMP_RBUS_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
	{
#ifdef RT2880
		RTMP_SYS_IO_READ32(0xa030000c, &pAd->CommonCfg.CID);
#else
		RTMP_SYS_IO_READ32(0xb000000c, &pAd->CommonCfg.CID);
		RTMP_SYS_IO_READ32(0xb0000000, &pAd->CommonCfg.CN);
#endif

#ifdef RT6352
#ifdef RELEASE_EXCLUDE
		/* 0: DRQFN-148 pin 1: TFBGA-269 ball */
#endif /* RELEASE_EXCLUDE */
		if (IS_RT6352(pAd)) {
			pAd->CommonCfg.PKG_ID = (UCHAR)((pAd->CommonCfg.CID >> 16) & 0x0001);
			pAd->CommonCfg.Chip_VerID = (UCHAR)((pAd->CommonCfg.CID >> 8) & 0x0f);
			pAd->CommonCfg.Chip_E_Number = (UCHAR)((pAd->CommonCfg.CID) & 0x0f);
		}
#endif /* RT6352 */

		DBGPRINT(RT_DEBUG_TRACE, ("CN: %lx\tCID = %lx\n",
				pAd->CommonCfg.CN, pAd->CommonCfg.CID));
	}
#endif /* RTMP_RBUS_SUPPORT */


#ifdef RT3883
	if (IS_RT3883(pAd))
	{
		RT3883_Init(pAd);
		goto done;
	}
#endif /* RT3883 */

#ifdef RT3290
	if (IS_RT3290(pAd))
	{
		RT3290_Init(pAd);
		goto done;
	}
#endif /* RT290 */

#ifdef RT8592
	if (IS_RT8592(pAd)) {
		RT85592_Init(pAd);
		goto done;
	}
#endif /* RT8592 */

#ifdef MT76x0
	if (IS_MT76x0(pAd)) {
		MT76x0_Init(pAd);
		goto done;
	}
#endif /* MT76x0 */

#ifdef MT76x2
	if (IS_MT76x2(pAd)) {
		mt76x2_init(pAd);
		goto done;
	}
#endif

#ifdef MT7601
	if (IS_MT7601(pAd)) {
		MT7601_Init(pAd);
		goto done;
	}
#endif /* MT7601 */

#ifdef RT6352
	if (IS_RT6352(pAd)) {
		RT6352_Init(pAd);
		goto done;
	}
#endif /* RT6352 */

#ifdef MT7603
	if (IS_MT7603(pAd)) {
		mt7603_init(pAd);
		goto done;
	}
#endif /* MT7603 */

#ifdef MT7628
	if (IS_MT7628(pAd)) {
		mt7628_init(pAd);
		goto done;
	}
#endif /* MT7628 */

#ifdef MT7636
    if (IS_MT7636(pAd)) {
        mt7636_init(pAd);
        goto done;
    }
#endif /* MT7636 */


#ifdef GREENAP_SUPPORT
	pChipOps->EnableAPMIMOPS = EnableAPMIMOPSv1;
	pChipOps->DisableAPMIMOPS = DisableAPMIMOPSv1;
#endif /* GREENAP_SUPPORT */

#ifdef RTMP_MAC
	// TODO: default settings for rest of the chips!! change this to really default chip.
	RTxx_default_Init(pAd);
#endif /* RTMP_MAC */

	/* We depends on RfICType and MACVersion to assign the corresponding operation callbacks. */
#ifdef RT2880
	if (IS_RT2880(pAd))
		RT2880_Init(pAd);
#endif /* RT2880 */

#ifdef RT305x
#ifdef RT3352
	/*FIXME by Steven: RFIC=RFIC_3022 in some RT3352 board*/
/*	if (pAd->RfIcType == RFIC_3322) {*/
	if (IS_RT3352(pAd))
		RT3352_Init(pAd);
	else
#endif /* RT3352 */
#ifdef RT5350
	if (IS_RT5350(pAd))
		RT5350_Init(pAd);
	else
#endif /* RT5350 */
/* comment : the RfIcType is not ready yet, because EEPROM doesn't be initialized. */
/*	if ((pAd->MACVersion == 0x28720200) && 
		((pAd->RfIcType == RFIC_3320) || (pAd->RfIcType == RFIC_3020) || (pAd->RfIcType == RFIC_3021) || (pAd->RfIcType == RFIC_3022))) */
	if (IS_RT3050_3052_3350(pAd))
		RT305x_Init(pAd);
	else
#endif /* RT305x */
#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392)
	if (IS_RT5390(pAd) || IS_RT5392(pAd))
		RT5390_Init(pAd);
	else
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) */
#ifdef RT2883
	if (IS_RT2883(pAd) && (pAd->infType == RTMP_DEV_INF_RBUS))
		RT2883_Init(pAd);
	else
#endif /* RT2883 */
#ifdef RT35xx
	if (IS_RT3572(pAd))
		RT35xx_Init(pAd);
	else
#endif /* RT35xx */
#ifdef RT3593
	if (IS_RT3593(pAd))
		RT3593_Init(pAd);
	else
#endif /* RT3593 */
#ifdef RT30xx
	if (IS_RT30xx(pAd))
	{
#ifdef RT33xx
		if (IS_RT3390(pAd))
			RT33xx_Init(pAd);
		else
#endif /* RT33xx */
			RT30xx_Init(pAd);
	}
#endif /* RT30xx */

#ifdef RT5592
	if (IS_RT5592(pAd))
		RT5592_Init(pAd);
#endif /* RT5592 */

done:
	DBGPRINT(RT_DEBUG_TRACE, ("Chip specific bbpRegTbSize=%d!\n", pChipCap->bbpRegTbSize));
	DBGPRINT(RT_DEBUG_TRACE, ("Chip VCO calibration mode = %d!\n", pChipCap->FlgIsVcoReCalMode));
#ifdef DOT11W_PMF_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("[PMF] Encryption mode = %d\n", pChipCap->FlgPMFEncrtptMode));
#endif /* DOT11W_PMF_SUPPORT */

	return ret;
}

#ifdef RT65xx
BOOLEAN isExternalPAMode(RTMP_ADAPTER *ad, INT channel)
{
	BOOLEAN pa_mode = FALSE;

        if (channel > 14) {
	        if (ad->chipCap.PAType == EXT_PA_2G_5G)
                	pa_mode = TRUE;
                else if (ad->chipCap.PAType == EXT_PA_5G_ONLY)
                        pa_mode = TRUE;
                else
        	        pa_mode = FALSE;
        } else {
                if (ad->chipCap.PAType == EXT_PA_2G_5G)
                        pa_mode = TRUE;
                else if ((ad->chipCap.PAType == EXT_PA_5G_ONLY) ||
                         (ad->chipCap.PAType == INT_PA_2G_5G))
                        pa_mode = FALSE;
                else if (ad->chipCap.PAType == EXT_PA_2G_ONLY)
                        pa_mode = TRUE;
        }

	return pa_mode;
}
#endif /* RT65xx */

