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
	rt30xx.c

	Abstract:
	Specific funcitons and variables for RT30xx.

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/


#ifndef RT30xx
#error	"You should define RT30xx if you want to compile this file"
#endif /* RT30xx */

#ifndef RTMP_RF_RW_SUPPORT
#error "You Should Enable compile flag RTMP_RF_RW_SUPPORT for this chip"
#endif /* RTMP_RF_RW_SUPPORT */

#include "rt_config.h"

/*
  RF register initialization set
*/
#ifdef RELEASE_EXCLUDE
/*
	Change RF_R07 from 0x70 to 0x60 for fixing high-power issue
	which will make the throughput of RT3090 lower than 10Mbps
*/
#endif /* RELEASE_EXCLUDE */
REG_PAIR   RT3020_RFRegTable[] = {
        {RF_R04,          0x40},
        {RF_R05,          0x03},
        {RF_R06,          0x02},
        {RF_R07,          0x60},      
        {RF_R09,          0x0F},
        {RF_R10,          0x41},
        {RF_R11,          0x21},
        {RF_R12,          0x7B},
        {RF_R14,          0x90},
        {RF_R15,          0x58},
        {RF_R16,          0xB3},
        {RF_R17,          0x92},
        {RF_R18,          0x2C},
        {RF_R19,          0x02},
        {RF_R20,          0xBA},
        {RF_R21,          0xDB},
        {RF_R24,          0x16},      
        {RF_R25,          0x03},
        {RF_R29,          0x1F},
};

UCHAR NUM_RF_3020_REG_PARMS = (sizeof(RT3020_RFRegTable) / sizeof(REG_PAIR));

#ifdef RTMP_FLASH_SUPPORT
UCHAR RT3090_EeBuffer[EEPROM_SIZE] = { 
0x92, 0x30, 0x02, 0x01, 0x00, 0x0c, 0x43, 0x30, 0x92, 0x00, 0x92, 0x30, 0x14, 0x18, 0x01, 0x80, 
0x00, 0x00, 0x92, 0x30, 0x14, 0x18, 0x00, 0x00, 0x01, 0x00, 0x6a, 0xff, 0x13, 0x02, 0xff, 0xff, 
0xff, 0xff, 0xc1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0x8e, 0x75, 0x01, 0x43, 0x22, 0x08, 0x27, 0x00, 0xff, 0xff, 0x16, 0x01, 0xff, 0xff, 0xd9, 0xfa, 
0xcc, 0x88, 0xff, 0xff, 0x0a, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
0xff, 0xff, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 
0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x1d, 0x1a, 
0x15, 0x11, 0x0f, 0x0d, 0x0a, 0x07, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x88, 0x88, 
0xcc, 0xcc, 0xaa, 0x88, 0xcc, 0xcc, 0xaa, 0x88, 0xcc, 0xcc, 0xaa, 0x88, 0xcc, 0xcc, 0xaa, 0x88, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
#endif /* RTMP_FLASH_SUPPORT */


/*
========================================================================
Routine Description:
	Initialize RT35xx.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RT30xx_Init(
	IN PRTMP_ADAPTER		pAd)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;


	/* init capability */
	rtmp_phy_probe(pAd);

	/* 
		WARNING: 
			Currently following table are shared by all RT30xx based IC,
			change it carefully when you add a new IC here.
	*/
	pChipCap->pRFRegTable = RT3020_RFRegTable;
	pChipCap->MaxNumOfBbpId = 185;
	pChipCap->TXWISize = 16;
	pChipCap->RXWISize = 16;
#ifdef RTMP_MAC_PCI
	if (IS_PCI_INF(pAd)) {
		pChipCap->tx_hw_hdr_len = pChipCap->TXWISize;
		pChipCap->rx_hw_hdr_len = pChipCap->RXWISize;
	}
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	if (IS_USB_INF(pAd)) {
		pChipCap->tx_hw_hdr_len = TXINFO_SIZE + pChipCap->TXWISize;
		pChipCap->rx_hw_hdr_len = RXDMA_FIELD_SIZE + pChipCap->RXWISize;
	}
#endif /* RTMP_MAC_USB */

	/* init operator */
#ifdef RT3070
		if (pAd->infType == RTMP_DEV_INF_USB)
		{
			pChipOps->AsicRfInit = NICInitRT3070RFRegisters;
			pChipOps->HighPowerTuning = RT3070_PowerTuning;
		}
#endif /* RT3070 */

#ifdef RT3090
		if (pAd->infType == RTMP_DEV_INF_PCIE)
		{
			pChipOps->AsicRfInit = NICInitRT3090RFRegisters;
#ifdef RTMP_FLASH_SUPPORT
			pChipCap->EEPROM_DEFAULT_BIN = RT3090_EeBuffer;
#endif /* RTMP_FLASH_SUPPORT */
		}

#ifdef RELEASE_EXCLUDE
		/*
			3090, 3090A and 3390 all support hadware tone radar function.
			But the soluation of those are different.
			3090 is the old one.
		*/
#endif /* RELEASE_EXCLUDE */
#ifdef CARRIER_DETECTION_SUPPORT
		if (IS_RT3090A(pAd)||IS_RT3390(pAd))
			pChipCap->carrier_func=TONE_RADAR_V2;
		else
			pChipCap->chipCap.carrier_func=TONE_RADAR_V1;
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* RT3090 */

#ifdef MICROWAVE_OVEN_SUPPORT
		pChipOps->AsicMeasureFalseCCA = RT30xx_AsicMeasureFalseCCA;
		pChipOps->AsicMitigateMicrowave = RT30xx_AsicMitigateMicrowave;
#endif /* MICROWAVE_OVEN_SUPPORT */

		pChipCap->FlgIsVcoReCalMode = VCO_CAL_MODE_1;
		pChipOps->AsicHaltAction = RT30xxHaltAction;
		pChipOps->AsicRfTurnOff = RT30xxLoadRFSleepModeSetup;
		pChipOps->AsicReverseRfFromSleepMode = RT30xxReverseRFSleepModeSetup;
		pChipOps->ChipSwitchChannel = RT30xx_ChipSwitchChannel;
		pChipOps->AsicAdjustTxPower = AsicAdjustTxPower;
		pChipOps->ChipBBPAdjust = RT30xx_ChipBBPAdjust;
		pChipOps->ChipAGCInit = RT30xx_ChipAGCInit;
		/* 1T1R only */
		if (pAd->RfIcType == RFIC_3020)
		{
			pChipOps->SetRxAnt = RT30xxSetRxAnt; 
			pAd->Mlme.bEnableAutoAntennaCheck = FALSE;
		}
		pChipOps->AsicGetTxPowerOffset = AsicGetTxPowerOffset;
		pChipOps->AsicTxAlcGetAutoAgcOffset = AsicGetAutoAgcOffsetForExternalTxAlc;	
		
		pChipCap->FlgIsHwAntennaDiversitySup = FALSE; 
		/*pChipOps->ChipResumeMsduTransmission = NULL; */
		/*pChipOps->VdrTuning1 = NULL; */
		pChipOps->RxSensitivityTuning = NULL;	
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
		pChipCap->FreqCalibrationSupport = FALSE;
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
		pChipCap->MCUType = M8051;
		pChipCap->AMPDUFactor = 2;
}


/*
 Antenna divesity use GPIO3 and EESK pin for control
 Antenna and EEPROM access are both using EESK pin,
 Therefor we should avoid accessing EESK at the same time
 Then restore antenna after EEPROM access
 The original name of this function is AsicSetRxAnt(), now change to 
*/
VOID RT30xxSetRxAnt(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Ant)
{
	UINT32	Value;
#ifdef RTMP_MAC_PCI
	UINT32	x;
#endif /* RTMP_MAC_PCI */

	if (/*(!pAd->NicConfig2.field.AntDiversity) ||*/
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS))	||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))	||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		return;
	}

	/* the antenna selection is through firmware and MAC register(GPIO3) */
	if (IS_RT2070(pAd) || (IS_RT3070(pAd) && pAd->RfIcType == RFIC_3020) ||
			(IS_RT3090(pAd) && pAd->RfIcType == RFIC_3020))
	{
	if (Ant == 0)
	{
		/*
			Main antenna
			E2PROM_CSR only in PCI bus Reg., USB Bus need MCU commad to control the EESK pin.
		*/
#ifdef RTMP_MAC_PCI
		RTMP_IO_READ32(pAd, E2PROM_CSR, &x);
		x |= (EESK);
		RTMP_IO_WRITE32(pAd, E2PROM_CSR, x);
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		AsicSendCommandToMcu(pAd, 0x73, 0xFF, 0x1, 0x0, FALSE);
#endif /* RTMP_MAC_USB */

		RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &Value);
		Value &= ~(0x0808);
		RTMP_IO_WRITE32(pAd, GPIO_CTRL_CFG, Value);
		DBGPRINT_RAW(RT_DEBUG_TRACE, ("AsicSetRxAnt, switch to main antenna\n"));
	}
	else
	{
		/*
			Aux antenna
		 	E2PROM_CSR only in PCI bus Reg., USB Bus need MCU commad to control the EESK pin.
		*/
#ifdef RTMP_MAC_PCI
		RTMP_IO_READ32(pAd, E2PROM_CSR, &x);
		x &= ~(EESK);
		RTMP_IO_WRITE32(pAd, E2PROM_CSR, x);
#endif /* RTMP_MAC_PCI */
#ifdef RTMP_MAC_USB
		AsicSendCommandToMcu(pAd, 0x73, 0xFF, 0x0, 0x0, FALSE);
#endif /* RTMP_MAC_USB */
		RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &Value);
		Value &= ~(0x0808);
		Value |= 0x08;
		RTMP_IO_WRITE32(pAd, GPIO_CTRL_CFG, Value);
		DBGPRINT_RAW(RT_DEBUG_TRACE, ("AsicSetRxAnt, switch to aux antenna\n"));
		}
	}
}


/*
	========================================================================

	Routine Description:
		For RF filter calibration purpose

	Arguments:
		pAd                          Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	========================================================================
*/
VOID RTMPFilterCalibration(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR	R55x = 0, value, FilterTarget = 0x1E, BBPValue=0;
	UINT	loop = 0, count = 0, loopcnt = 0, ReTry = 0;
	UCHAR	RF_R24_Value = 0;

	/* Give bbp filter initial value */
	pAd->Mlme.CaliBW20RfR24 = 0x1F;
	pAd->Mlme.CaliBW40RfR24 = 0x2F; /* Bit[5] must be 1 for BW 40 */

	do 
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))				
			return;
		if (loop == 1)	/*BandWidth = 40 MHz*/
		{
			/* Write 0x27 to RF_R24 to program filter*/
			RT30xxReadRFRegister(pAd, RF_R24, (PUCHAR)(&RF_R24_Value));
			RF_R24_Value = (RF_R24_Value & 0xC0) | 0x27; /* <bit 5>:tx_h20M<bit 5> and <bit 4:0>:tx_agc_fc<bit 4:0>*/
			RT30xxWriteRFRegister(pAd, RF_R24, RF_R24_Value);
			if (IS_RT3071(pAd) || IS_RT3572(pAd))
				FilterTarget = 0x15;
			else
				FilterTarget = 0x19;

			/* when calibrate BW40, BBP mask must set to BW40.*/
			bbp_set_bw(pAd, BW_40);

			/* set to BW40*/
			RT30xxReadRFRegister(pAd, RF_R31, &value);
			value |= 0x20;
			RT30xxWriteRFRegister(pAd, RF_R31, value);
		}
		else	/*BandWidth = 20 MHz*/
		{
			/* Write 0x07 to RF_R24 to program filter*/
			RT30xxReadRFRegister(pAd, RF_R24, (PUCHAR)(&RF_R24_Value));
			RF_R24_Value = (RF_R24_Value & 0xC0) | 0x07; /* <bit 5>:tx_h20M<bit 5> and <bit 4:0>:tx_agc_fc<bit 4:0>*/
			RT30xxWriteRFRegister(pAd, RF_R24, RF_R24_Value);
			if (IS_RT3071(pAd) || IS_RT3572(pAd))
				FilterTarget = 0x13;
			else
				FilterTarget = 0x16;

			/*set to BW20*/
			RT30xxReadRFRegister(pAd, RF_R31, &value);
			value &= (~0x20);
			RT30xxWriteRFRegister(pAd, RF_R31, value);
		}

		/* Write 0x01 to RF_R22 to enable baseband loopback mode*/
		RT30xxReadRFRegister(pAd, RF_R22, &value);
		value |= 0x01;
		RT30xxWriteRFRegister(pAd, RF_R22, value);

		/* Write 0x00 to BBP_R24 to set power & frequency of passband test tone*/
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R24, 0);
		do
		{
			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))				
				return;
			/* Write 0x90 to BBP_R25 to transmit test tone*/
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R25, 0x90);

			RtmpusecDelay(1000);
			/* Read BBP_R55[6:0] for received power, set R55x = BBP_R55[6:0]*/
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R55, &value);
			R55x = value & 0xFF;

		} while ((ReTry++ < 100) && (R55x == 0));
		
		/* Write 0x06 to BBP_R24 to set power & frequency of stopband test tone*/
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R24, 0x06);

		while(TRUE)
		{
			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))				
				return;
			
			/* Write 0x90 to BBP_R25 to transmit test tone*/
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R25, 0x90);

			/*We need to wait for calibration*/
			RtmpusecDelay(1000);
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R55, &value);
			value &= 0xFF;
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("RTUSBFilterCalibration BBP_R55=0x%x, RF_R24=0x%x, R55x=0x%x, FilterTarget=0x%x, (R55x - value)=0x%x\n",
				value, RF_R24_Value, R55x, FilterTarget, (R55x - value)));
#endif /* RELEASE_EXCLUDE */
			if ((R55x - value) < FilterTarget)
			{
				RF_R24_Value ++;
			}
			else if ((R55x - value) == FilterTarget)
			{
				RF_R24_Value ++;
				count ++;
			}
			else
			{
				break;
			}

			/* prevent infinite loop cause driver hang.*/
			if (loopcnt++ > 100)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("RTMPFilterCalibration - can't find a valid value, loopcnt=%d stop calibrating", loopcnt));
				break;
			}

			/*Write RF_R24 to program filter*/
			RT30xxWriteRFRegister(pAd, RF_R24, RF_R24_Value);
		}

		if (count > 0)
		{
			RF_R24_Value = RF_R24_Value - ((count) ? (1) : (0));
		}

		/* Store for future usage*/
		if (loopcnt < 100)
		{
			if (loop++ == 0)
			{
				/*BandWidth = 20 MHz*/
				pAd->Mlme.CaliBW20RfR24 = (UCHAR)RF_R24_Value;
			}
			else
			{
				/*BandWidth = 40 MHz*/
				pAd->Mlme.CaliBW40RfR24 = (UCHAR)RF_R24_Value;
				break;
			}
		}
		else 
			break;

		RT30xxWriteRFRegister(pAd, RF_R24, RF_R24_Value);

		/* reset count*/
		count = 0;
	} while(TRUE);


	/* Set back to initial state*/
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R24, 0);

	RT30xxReadRFRegister(pAd, RF_R22, &value);
	value &= ~(0x01);
	RT30xxWriteRFRegister(pAd, RF_R22, value);

	bbp_set_bw(pAd, BW_20);

	DBGPRINT(RT_DEBUG_TRACE, ("RTMPFilterCalibration - CaliBW20RfR24=0x%x, CaliBW40RfR24=0x%x\n",
					pAd->Mlme.CaliBW20RfR24, pAd->Mlme.CaliBW40RfR24));
}


/*
	add by johnli, RF power sequence setup

	==========================================================================
	Description:

	Load RF normal operation-mode setup
	
	==========================================================================
 */
VOID RT30xxLoadRFNormalModeSetup(
	IN PRTMP_ADAPTER 	pAd)
{
	UCHAR RFValue, bbpreg = 0;

	{
		/* improve power consumption */
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R138, &bbpreg);
		if (pAd->Antenna.field.TxPath == 1)
			bbpreg = (bbpreg | 0x20);	/* turn off tx DAC_1*/
		if (pAd->Antenna.field.RxPath == 1)
			bbpreg &= (~0x2);	/* turn off tx ADC_1*/
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R138, bbpreg);
	}

	/*RX0_PD & TX0_PD, RF R1 register Bit 2 & Bit 3 to 0 and RF_BLOCK_en,RX1_PD & TX1_PD, Bit0, Bit 4 & Bit5 to 1*/
	RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
	RFValue = (RFValue & (~0x0C)) | 0x31;
	RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

	/* TX_LO2_en, RF R15 register Bit 3 to 0*/
	RT30xxReadRFRegister(pAd, RF_R15, &RFValue);
	RFValue &= (~0x08);
	RT30xxWriteRFRegister(pAd, RF_R15, RFValue);


	/* TX_LO1_en, RF R17 register Bit 3 to 0*/
	RT30xxReadRFRegister(pAd, RF_R17, &RFValue);
	RFValue &= (~0x08);
	/* to fix rx long range issue*/
	if (((pAd->MACVersion & 0xffff) >= 0x0211) && (pAd->NicConfig2.field.ExternalLNAForG == 0))
	{
		RFValue |= 0x20;
	}
	/* set RF_R17_bit[2:0] equal to EEPROM setting at 0x48h*/
	if (pAd->TxMixerGain24G >= 2)
	{
		RFValue &= (~0x7);  /* clean bit [2:0]*/
		RFValue |= pAd->TxMixerGain24G;
	}
	RT30xxWriteRFRegister(pAd, RF_R17, RFValue);

	/* RX_LO1_en, RF R20 register Bit 3 to 0*/
	RT30xxReadRFRegister(pAd, RF_R20, &RFValue);
	RFValue &= (~0x08);
	RT30xxWriteRFRegister(pAd, RF_R20, RFValue);

	/* RX_LO2_en, RF R21 register Bit 3 to 0*/
	RT30xxReadRFRegister(pAd, RF_R21, &RFValue);
	RFValue &= (~0x08);
	RT30xxWriteRFRegister(pAd, RF_R21, RFValue);
}

/*
	==========================================================================
	Description:

	Load RF sleep-mode setup
	
	==========================================================================
 */
VOID RT30xxLoadRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd)
{
	UCHAR RFValue;
	UINT32 MACValue;

	{
			/* RF_BLOCK_en. RF R1 register Bit 0 to 0*/
			RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
			RFValue &= (~0x01);
			RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

			/* VCO_IC, RF R7 register Bit 4 & Bit 5 to 0*/
			RT30xxReadRFRegister(pAd, RF_R07, &RFValue);
			RFValue &= (~0x30);
			RT30xxWriteRFRegister(pAd, RF_R07, RFValue);

			/* Idoh, RF R9 register Bit 1, Bit 2 & Bit 3 to 0*/
			RT30xxReadRFRegister(pAd, RF_R09, &RFValue);
			RFValue &= (~0x0E);
			RT30xxWriteRFRegister(pAd, RF_R09, RFValue);

			/* RX_CTB_en, RF R21 register Bit 7 to 0*/
			RT30xxReadRFRegister(pAd, RF_R21, &RFValue);
			RFValue &= (~0x80);
			RT30xxWriteRFRegister(pAd, RF_R21, RFValue);
	}
#if 0
	/* Don't touch LDO_CFG0 for 3090F & 3593, possibly the board is single power scheme*/
	if (IS_RT3090(pAd) ||	/*IS_RT3090 including RT309x and RT3071/72*/
		(IS_RT3070(pAd) && ((pAd->MACVersion & 0xffff) < 0x0201)))
	{
			RT30xxReadRFRegister(pAd, RF_R27, &RFValue);
			RFValue |= 0x77;
			RT30xxWriteRFRegister(pAd, RF_R27, RFValue);

		RTMP_IO_READ32(pAd, LDO_CFG0, &MACValue);
		MACValue |= 0x1D000000;
		RTMP_IO_WRITE32(pAd, LDO_CFG0, MACValue);
	}
#endif
}

/*
	==========================================================================
	Description:

	Reverse RF sleep-mode setup
	
	==========================================================================
 */
VOID RT30xxReverseRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd,
	IN BOOLEAN			FlgIsInitState)
{
	UCHAR RFValue;
	UINT32 MACValue;

	if(!IS_RT3572(pAd))
	{

		/* RF_BLOCK_en, RF R1 register Bit 0 to 1*/
		RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
		RFValue |= 0x01;
		RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

		/* VCO_IC, RF R7 register Bit 4 & Bit 5 to 1*/
#ifdef RELEASE_EXCLUDE			
/*
        Change RF_R07 from 0x70 to 0x60 for fixing high-power issue
        which will make the throughput of RT3090 lower than 10Mbps
*/
#endif /* RELEASE_EXCLUDE */
		RT30xxReadRFRegister(pAd, RF_R07, &RFValue);
		RFValue |= 0x20;
		RT30xxWriteRFRegister(pAd, RF_R07, RFValue);

		/* Idoh, RF R9 register Bit 1, Bit 2 & Bit 3 to 1*/
		RT30xxReadRFRegister(pAd, RF_R09, &RFValue);
		RFValue |= 0x0E;
		RT30xxWriteRFRegister(pAd, RF_R09, RFValue);

		/* RX_CTB_en, RF R21 register Bit 7 to 1*/
		RT30xxReadRFRegister(pAd, RF_R21, &RFValue);
		RFValue |= 0x80;
		RT30xxWriteRFRegister(pAd, RF_R21, RFValue);
	}

	if (IS_RT3090(pAd) ||	/* IS_RT3090 including RT309x and RT3071/72*/
		IS_RT3390(pAd) ||
		(IS_RT3070(pAd) && ((pAd->MACVersion & 0xffff) < 0x0201)))
	{
		{
			RT30xxReadRFRegister(pAd, RF_R27, &RFValue);
			if ((pAd->MACVersion & 0xffff) < 0x0211)
				RFValue = (RFValue & (~0x77)) | 0x3;
			else
				RFValue = (RFValue & (~0x77));
			RT30xxWriteRFRegister(pAd, RF_R27, RFValue);
		}

		/* RT3071 version E has fixed this issue*/
#if 0
		if ((pAd->NicConfig2.field.DACTestBit == 1) && ((pAd->MACVersion & 0xffff) < 0x0211))
		{
			/* patch tx EVM issue temporarily*/
			RTMP_IO_READ32(pAd, LDO_CFG0, &MACValue);
			MACValue = ((MACValue & 0xE0FFFFFF) | 0x0D000000);
			RTMP_IO_WRITE32(pAd, LDO_CFG0, MACValue);
		}
		else if (!IS_RT3090(pAd))
		{
			RTMP_IO_READ32(pAd, LDO_CFG0, &MACValue);
			MACValue = ((MACValue & 0xE0FFFFFF) | 0x01000000);
			RTMP_IO_WRITE32(pAd, LDO_CFG0, MACValue);
		}
#endif
	}

}
/* end johnli*/

VOID RT30xxHaltAction(
	IN PRTMP_ADAPTER 	pAd)
{
	UINT32		TxPinCfg = 0x00050F0F;


	/* Turn off LNA_PE or TRSW_POL*/

		if ((IS_RT3071(pAd) || IS_RT3572(pAd))
#ifdef RTMP_EFUSE_SUPPORT
			&& (pAd->bUseEfuse)
#endif /* RTMP_EFUSE_SUPPORT */
			)
		{
			TxPinCfg &= 0xFFFBF0F0; /* bit18 off */
		}
		else
		{
			TxPinCfg &= 0xFFFFF0F0;
		}
#ifdef RT35xx
		if (IS_RT3572(pAd))
			RT30xxWriteRFRegister(pAd, RF_R08, (UCHAR)0x00);
#endif /* RT35xx */

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);   
}


VOID RT30xx_ChipSwitchChannel(
	IN PRTMP_ADAPTER 			pAd,
	IN UCHAR					Channel,
	IN BOOLEAN					bScan)
{
	CHAR TxPwer = 0, TxPwer2 = DEFAULT_RF_TX_POWER; /*Bbp94 = BBPR94_DEFAULT, TxPwer2 = DEFAULT_RF_TX_POWER;*/
	UCHAR index;
	UINT32 Value = 0; /*BbpReg, Value;*/
	UCHAR RFValue = 0;
	UINT32 i = 0;
#ifdef RT33xx
#ifdef RTMP_MAC_PCI
	UCHAR Tx0FinePowerCtrl = 0, Tx1FinePowerCtrl = 0;
	BBP_R109_STRUC BbpR109 = {{0}};
#endif
#endif

	/* Search Tx power value*/

	/*
		We can't use ChannelList to search channel, since some central channl's txpowr doesn't list 
		in ChannelList, so use TxPower array instead.
	*/
	for (index = 0; index < MAX_NUM_OF_CHANNELS; index++)
	{
		if (Channel == pAd->TxPower[index].Channel)
		{
			TxPwer = pAd->TxPower[index].Power;
			TxPwer2 = pAd->TxPower[index].Power2;

#ifdef RT33xx
#ifdef RTMP_MAC_PCI
			if ((IS_RT3090A(pAd) || IS_RT3390(pAd)) &&
				(pAd->infType == RTMP_DEV_INF_PCI || pAd->infType == RTMP_DEV_INF_PCIE))
			{
				Tx0FinePowerCtrl = pAd->TxPower[index].Tx0FinePowerCtrl;
				Tx1FinePowerCtrl = pAd->TxPower[index].Tx1FinePowerCtrl;
			}
#endif /* RTMP_MAC_PCI */
#endif /* RT33xx */
			break;
		}
	}

	if (index == MAX_NUM_OF_CHANNELS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Can't find the Channel#%d \n", __FUNCTION__, Channel));
	}

	/* The RF programming sequence is difference between 3xxx and 2xxx*/
	{
		/* modify by WY for Read RF Reg. error */
		UCHAR	calRFValue;
		for (index = 0; index < NUM_OF_3020_CHNL; index++)
		{
			if (Channel == FreqItems3020[index].Channel)
			{
				/* Programming channel parameters*/
				RT30xxWriteRFRegister(pAd, RF_R02, FreqItems3020[index].N);
				/*
					RT3370/RT3390 RF version is 0x3320 RF_R3 [7:4] is not reserved bits
					RF_R3[6:4] (pa1_bc_cck) : PA1 Bias CCK
					RF_R3[7] (pa2_cc_cck) : PA2 Cascode Bias CCK
				 */
				RT30xxReadRFRegister(pAd, RF_R03, (PUCHAR)(&RFValue));
				RFValue = (RFValue & 0xF0) | (FreqItems3020[index].K & ~0xF0); /* <bit 3:0>:K<bit 3:0>*/
				RT30xxWriteRFRegister(pAd, RF_R03, RFValue);
				RT30xxReadRFRegister(pAd, RF_R06, &RFValue);
				RFValue = (RFValue & 0xFC) | FreqItems3020[index].R;
				RT30xxWriteRFRegister(pAd, RF_R06, RFValue);

				/* Set Tx0 Power*/
				RT30xxReadRFRegister(pAd, RF_R12, &RFValue);
				RFValue = (RFValue & 0xE0) | TxPwer;
				RT30xxWriteRFRegister(pAd, RF_R12, RFValue);

				/*Set Tx1 Power*/
				RT30xxReadRFRegister(pAd, RF_R13, &RFValue);
				RFValue = (RFValue & 0xE0) | TxPwer2;
				RT30xxWriteRFRegister(pAd, RF_R13, RFValue);

#ifdef RT33xx
#ifdef RTMP_MAC_PCI
		
				/* Set the BBP Tx fine power control in 0.1dB step*/
	
				if ((IS_RT3090A(pAd) || IS_RT3390(pAd)) &&
					(pAd->infType == RTMP_DEV_INF_PCI || pAd->infType == RTMP_DEV_INF_PCIE))
				{
					BbpR109.field.Tx0PowerCtrl = Tx0FinePowerCtrl;

					if (pAd->Antenna.field.TxPath >= 2)
					{
						BbpR109.field.Tx1PowerCtrl = Tx1FinePowerCtrl;
					}
					else
					{
						BbpR109.field.Tx1PowerCtrl = 0;
					}
					
					RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R109, BbpR109.byte);

					DBGPRINT(RT_DEBUG_INFO, ("%s: Channel = %d, BBP_R109 = 0x%X\n", 
						__FUNCTION__, 
						Channel, 
						BbpR109.byte));
				}
#endif /* RTMP_MAC_PCI */
#endif /* RT33xx */

				/* Tx/Rx Stream setting*/
				RT30xxReadRFRegister(pAd, RF_R01, &RFValue);

				RFValue &= 0x03; /*clear bit[7~2]*/
				if (pAd->Antenna.field.TxPath == 1)
					RFValue |= 0xA0;
				else if (pAd->Antenna.field.TxPath == 2)
					RFValue |= 0x80;
				if (pAd->Antenna.field.RxPath == 1)
					RFValue |= 0x50;
				else if (pAd->Antenna.field.RxPath == 2)
					RFValue |= 0x40;
				RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

				/* Set RF offset*/
				RT30xxReadRFRegister(pAd, RF_R23, &RFValue);
				RFValue = (RFValue & 0x80) | pAd->RfFreqOffset;
				RT30xxWriteRFRegister(pAd, RF_R23, RFValue);

				/* Set BW*/
				if (!bScan && (pAd->CommonCfg.BBPCurrentBW == BW_40))
				{
					calRFValue = pAd->Mlme.CaliBW40RfR24;
				}
				else
				{
					calRFValue = pAd->Mlme.CaliBW20RfR24;
				}
				/*
					RT3370/RT3390 RF version is 0x3320 RF_R24 [7:6] is not reserved bits
					RF_R24[6] (BB_Rx1_out_en) : enable baseband output and ADC input
					RF_R24[7] (BB_Tx1_out_en) : enable DAC output or baseband input
				 */
				RT30xxReadRFRegister(pAd, RF_R24, (PUCHAR)(&RFValue));
				calRFValue = (RFValue & 0xC0) | (calRFValue & ~0xC0); /* <bit 5>:tx_h20M<bit 5> and <bit 4:0>:tx_agc_fc<bit 4:0>*/
				RT30xxWriteRFRegister(pAd, RF_R24, calRFValue);

				/*
					RT3370/RT3390 RF version is 0x3320 RF_R31 [7:6] is not reserved bits
					RF_R31[4:0] (rx_agc_fc) : capacitor control in baseband filter
					RF_R31[5] (rx_ h20M) : rx_ h20M: 0=10 MHz and 1=20MHz
					RF_R31[7:6] (drv_bc_cck) : Driver Bias CCK
				 */
				/* Set BW*/
				if (IS_RT3390(pAd)) /* RT3390 has different AGC for Tx and Rx*/
				{
					if (!bScan && (pAd->CommonCfg.BBPCurrentBW == BW_40))
					{
						calRFValue = pAd->Mlme.CaliBW40RfR31;
					}
					else
					{
						calRFValue = pAd->Mlme.CaliBW20RfR31;
					}
				}
				RT30xxReadRFRegister(pAd, RF_R31, (PUCHAR)(&RFValue));
				calRFValue = (RFValue & 0xC0) | (calRFValue & ~0xC0); /* <bit 5>:rx_h20M<bit 5> and <bit 4:0>:rx_agc_fc<bit 4:0>*/				
				RT30xxWriteRFRegister(pAd, RF_R31, calRFValue);

				/* Enable RF tuning*/
				RT30xxReadRFRegister(pAd, RF_R07, &RFValue);
				RFValue = RFValue | 0x1;
				RT30xxWriteRFRegister(pAd, RF_R07, RFValue);

                                RT30xxReadRFRegister(pAd, RF_R30, (PUCHAR)&RFValue);
                                RFValue |= 0x80;
                                RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RFValue);
                                RtmpusecDelay(1000);
                                RFValue &= 0x7F;
                                RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RFValue);    

				/* latch channel for future usage.*/
				pAd->LatchRfRegs.Channel = Channel;
				pAd->hw_cfg.lan_gain = GET_LNA_GAIN(pAd);
				
		DBGPRINT(RT_DEBUG_TRACE, ("SwitchChannel#%d(RF=%d, Pwr0=%d, Pwr1=%d, %dT), N=0x%02X, K=0x%02X, R=0x%02X\n",
			Channel, 
			pAd->RfIcType, 
			TxPwer,
			TxPwer2,
			pAd->Antenna.field.TxPath,
			FreqItems3020[index].N, 
			FreqItems3020[index].K, 
			FreqItems3020[index].R));

				break;
			}
		}
	}

	/* Change BBP setting during siwtch from a->g, g->a*/
	if (Channel <= 14)
	{
		ULONG	TxPinCfg = 0x00050F0A;/*Gary 2007/08/09 0x050A0A*/

		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - pAd->hw_cfg.lan_gain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - pAd->hw_cfg.lan_gain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - pAd->hw_cfg.lan_gain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0);/*(0x44 - pAd->hw_cfg.lan_gain));	According the Rory's suggestion to solve the middle range issue.*/

		/* Rx High power VGA offset for LNA select*/
		{
			if (pAd->NicConfig2.field.ExternalLNAForG)
			{
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46);
			}
			else
			{
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x84);
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x50);
			}
		}

		/* 5G band selection PIN, bit1 and bit2 are complement*/
		rtmp_mac_set_band(pAd, BAND_24G);

		{
			/* Turn off unused PA or LNA when only 1T or 1R*/
			if (pAd->Antenna.field.TxPath == 1)
			{
				TxPinCfg &= 0xFFFFFFF3;
			}
			if (pAd->Antenna.field.RxPath == 1)
			{
				TxPinCfg &= 0xFFFFF3FF;
			}
		}

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);

#if defined(RT3090) || defined(RT3390)
		/* PCIe PHY Transmit attenuation adjustment*/
		if ((IS_RT3090A(pAd) || IS_RT3390(pAd)) && IS_PCIE_INF(pAd))
		{
			INTERNAL_1_STRUCT Internal_1 = { { 0 } };

			RTMP_IO_READ32(pAd, INTERNAL_1, &Internal_1.word);
#ifdef RELEASE_EXCLUDE

/*Explanation for #14 channel interference*/

/*

 

RF(2.5GHz)

---------

Internal

125Mhz/32Khz(PCIe power-saving)

---------

PCIe PHY(2.5Ghz for spec 1.x)

 

All Ralink¡¦s PCIe devices have two external phy interfaces. One is RF which works in 2.5GHz in B/G 

Modes and the other is PCIe Phy which creates p2p Lane with PCIe bus and works in 2.5GHz based on PCIe spec 1.x.

 

When the channel is set to channel #14 under 20 MHz bandwidth, the maximum frequency is 

equal to 2496 [2412(channel 1 central channel)+12*5( the space between channel 1 and 13)+

12(the space between channel 13 and 14)=2484(the central channel of channel 14)+

12MHz (bandwidth/2)]. The frequency is closed to 2.5GHz and bandwidth 40 MHz is closer.

 

Because the Tx signal of PCIe PHY will feed back to RF PHY, if the frequency of the feed-back 

signals from PCIe is closed to current working channel of RF PHY, the feed-back signal will be 

mixed with the Rx signal of RF and interfere in the Rx of RF. 

 

To solve this problem, we must degrade the power of PCIe Tx when devices switch to channel #14. 

 

*/ 

#endif /* RELEASE_EXCLUDE */

			if (Channel == 14) /* Channel #14*/
			{
				Internal_1.field.PCIE_PHY_TX_ATTEN_EN = 1; /* Enable PCIe PHY Tx attenuation*/
				Internal_1.field.PCIE_PHY_TX_ATTEN_VALUE = 4; /* 9/16 full drive level*/
			}
			else /* Channel #1~#13*/
			{
				Internal_1.field.PCIE_PHY_TX_ATTEN_EN = 0; /* Disable PCIe PHY Tx attenuation*/
				Internal_1.field.PCIE_PHY_TX_ATTEN_VALUE = 0; /* n/a*/
			}

			RTMP_IO_WRITE32(pAd, INTERNAL_1, Internal_1.word);
		}
#endif

		filter_coefficient_ctrl(pAd, Channel);
	}
#ifdef RELEASE_EXCLUDE
/*
	309x/307x both do not support A band.
*/
#endif /* RELEASE_EXCLUDE */
#if 0
	else
	{
		ULONG	TxPinCfg = 0x00050F05;/*Gary 2007/8/9 0x050505*/
		UINT8	bbpValue;
		
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - pAd->hw_cfg.lan_gain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - pAd->hw_cfg.lan_gain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - pAd->hw_cfg.lan_gain));
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0);/*(0x44 - pAd->hw_cfg.lan_gain));    According the Rory's suggestion to solve the middle range issue.*/   

		/* Set the BBP_R82 value here */
		bbpValue = 0xF2;
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, bbpValue);


		/* Rx High power VGA offset for LNA select*/
		if (pAd->NicConfig2.field.ExternalLNAForA)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46);
		}
		else
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x50);
		}

		/* 5G band selection PIN, bit1 and bit2 are complement*/
		rtmp_mac_set_band(pAd, BAND_5G);

		/* Turn off unused PA or LNA when only 1T or 1R*/
		{
			/* Turn off unused PA or LNA when only 1T or 1R*/
			if (pAd->Antenna.field.TxPath == 1)
			{
				TxPinCfg &= 0xFFFFFFF3;
			}
			if (pAd->Antenna.field.RxPath == 1)
			{
				TxPinCfg &= 0xFFFFF3FF;
			}
		}

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}
#endif

	/*
		On 11A, We should delay and wait RF/BBP to be stable
		and the appropriate time should be 1000 micro seconds 
		2005/06/05 - On 11G, We also need this delay time. Otherwise it's difficult to pass the WHQL.
	*/
	RtmpusecDelay(1000);
}


VOID RT30xx_ChipBBPAdjust(RTMP_ADAPTER *pAd)
{
	UINT8 rf_bw, ext_ch;

#ifdef DOT11_N_SUPPORT
	if (get_ht_cent_ch(pAd, &rf_bw, &ext_ch) == FALSE)
#endif /* DOT11_N_SUPPORT */
	{
		rf_bw = BW_20;
		ext_ch = EXTCHA_NONE;
		pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
	}
	
	bbp_set_bw(pAd, rf_bw);
	/*  TX/RX : control channel setting */
	rtmp_mac_set_ctrlch(pAd, ext_ch);
	bbp_set_ctrlch(pAd, ext_ch);
	
	/* request by Gary 20070208 for middle and long range G Band*/
	/* request by Brian 20070306*/
	bbp_set_agc(pAd, 0x38, RX_CHAIN_ALL);

	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x0A);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, 0x10);
	
#ifdef DOT11_N_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("ApStartUp : 20MHz, ChannelWidth=%d, Channel=%d, ExtChanOffset=%d(%d) \n",
									pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth, 
									pAd->CommonCfg.Channel, 
									pAd->CommonCfg.RegTransmitSetting.field.EXTCHA,
									pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset));
#endif /* DOT11_N_SUPPORT */

 	/* request by Gary 20070208 for middle and long range G band*/
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, 0x2D);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, 0x2D);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, 0x2D);
	/*RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0x2D);*/
		

}	

VOID RT30xx_ChipAGCInit(
	IN PRTMP_ADAPTER		pAd,
	IN UCHAR				BandWidth)
{

	UCHAR R66 = 0x30;
	CHAR lan_gain = pAd->hw_cfg.lan_gain;
	
	if (pAd->LatchRfRegs.Channel <= 14)
	{	/* BG band*/
		/* Gary was verified Amazon AP and find that RT307x has BBP_R66 invalid default value */
		if (IS_RT3070(pAd)||IS_RT3090(pAd) || IS_RT3390(pAd))
			R66 = 0x1C + 2 * lan_gain;
	}
	else
	{
		/* A band */
		if (BandWidth == BW_20)
			R66 = (UCHAR)(0x32 + (lan_gain * 5) / 3);
#ifdef DOT11_N_SUPPORT
		else
			R66 = (UCHAR)(0x3A + (lan_gain * 5) / 3);
#endif // DOT11_N_SUPPORT //
	}

	bbp_set_agc(pAd, R66, RX_CHAIN_ALL);
	
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s():Ch=%d, BandWidth=%d, LNA_GAIN=0x%x, set R66 as 0x%x \n", 
				__FUNCTION__, pAd->LatchRfRegs.Channel, BandWidth, lan_gain, R66));
#endif /* RELEASE_EXCLUDE */

}


#ifdef MICROWAVE_OVEN_SUPPORT
VOID RT30xx_AsicMeasureFalseCCA(
	IN PRTMP_ADAPTER pAd
)
{
	UINT32 reg;

	/* reset RF LPF threshold */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0x12);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0xA);

	/* Set to high gain LAN */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);

	/* clear false cca counter */
	RTMP_IO_READ32(pAd, RX_STA_CNT1, &reg);

	/* reset false CCA counter */
	pAd->CommonCfg.MO_Cfg.nFalseCCACnt = 0;
}

VOID RT30xx_AsicMitigateMicrowave(
	IN PRTMP_ADAPTER pAd
)
{

	/* narrow down RF LPF */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, 0xD);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, 0x5);

	/* set middle gain */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x28);
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x60);
}
#endif /* MICROWAVE_OVEN_SUPPORT */

