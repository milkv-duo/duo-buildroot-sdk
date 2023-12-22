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
	rt3070.c

	Abstract:
	Specific funcitons and variables for RT3070

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/

#ifdef RT3070

#include "rt_config.h"


#ifndef RTMP_RF_RW_SUPPORT
#error "You Should Enable compile flag RTMP_RF_RW_SUPPORT for this chip"
#endif /* RTMP_RF_RW_SUPPORT */


VOID NICInitRT3070RFRegisters(IN PRTMP_ADAPTER pAd)
{
	INT i;
	UCHAR RFValue;
	
	/* 
	   Driver must read EEPROM to get RfIcType before initial RF registers 
	   Initialize RF register to default value 
	*/
	if (IS_RT3070(pAd) || IS_RT3071(pAd))
	{
		/* 
		   Init RF calibration
		   Driver should toggle RF R30 bit7 before init RF registers
		*/
		UINT8 RfReg = 0;          
		UINT32 data;

        RT30xxReadRFRegister(pAd, RF_R30, (PUCHAR)&RfReg);
        RfReg |= 0x80;
        RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RfReg);
        RtmpusecDelay(1000);
        RfReg &= 0x7F;
        RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RfReg);        

		/* set default antenna as main */
		if (pAd->RfIcType == RFIC_3020 || pAd->RfIcType == RFIC_2020)
			AsicSetRxAnt(pAd, pAd->RxAnt.Pair1PrimaryRxAnt);

        /* Initialize RF register to default value */
		for (i = 0; i < NUM_RF_3020_REG_PARMS; i++)
        {
            RT30xxWriteRFRegister(pAd, RT3020_RFRegTable[i].Register, RT3020_RFRegTable[i].Value);
        }
 
		RT30xxWriteRFRegister(pAd, RF_R31, 0x14);

		/* add by johnli */
		if (IS_RT3070(pAd))
		{
			/*
			   The DAC issue(LDO_CFG0) has been fixed in RT3070(F). 
			   The voltage raising patch is no longer needed for RT3070(F)	
			*/
			if ((pAd->MACVersion & 0xffff) < 0x0201)
			{
				/* Update MAC 0x05D4 from 01xxxxxx to 0Dxxxxxx (voltage 1.2V to 1.35V) for RT3070 to improve yield rate */
				RTUSBReadMACRegister(pAd, LDO_CFG0, &data);
				data = ((data & 0xF0FFFFFF) | 0x0D000000);
				RTUSBWriteMACRegister(pAd, LDO_CFG0, data, FALSE);
			}
		}
		else if (IS_RT3071(pAd))
		{
			/* Driver should set RF R6 bit6 on before init RF registers */
			RT30xxReadRFRegister(pAd, RF_R06, (PUCHAR)&RfReg);
			RfReg |= 0x40;
			RT30xxWriteRFRegister(pAd, RF_R06, (UCHAR)RfReg);

			/* RT3071 version E has fixed this issue */
			if ((pAd->NicConfig2.field.DACTestBit == 1) && ((pAd->MACVersion & 0xffff) < 0x0211))
			{
				/* patch tx EVM issue temporarily */
				RTUSBReadMACRegister(pAd, LDO_CFG0, &data);
				data = ((data & 0xE0FFFFFF) | 0x0D000000);
				RTUSBWriteMACRegister(pAd, LDO_CFG0, data, FALSE);
			}
			else
			{
				RTMP_IO_READ32(pAd, LDO_CFG0, &data);
				data = ((data & 0xE0FFFFFF) | 0x01000000);
				RTMP_IO_WRITE32(pAd, LDO_CFG0, data);
			}

			/* patch LNA_PE_G1 failed issue */
			RTUSBReadMACRegister(pAd, GPIO_SWITCH, &data);
			data &= ~(0x20);
			RTUSBWriteMACRegister(pAd, GPIO_SWITCH, data, FALSE);
		}
		
        /* For RF filter Calibration */
		RTMPFilterCalibration(pAd);

		/* 
		   Initialize RF R27 register, set RF R27 must be behind RTMPFilterCalibration()
		   TX to RX IQ glitch(RF_R27) has been fixed in RT3070(F). 
		   Raising RF voltage is no longer needed for RT3070(F)
		*/	
		if ((IS_RT3070(pAd)) && ((pAd->MACVersion & 0xffff) < 0x0201))
		{
			RT30xxWriteRFRegister(pAd, RF_R27, 0x3);
		}
		else if ((IS_RT3071(pAd)) && ((pAd->MACVersion & 0xffff) < 0x0211))
		{
			RT30xxWriteRFRegister(pAd, RF_R27, 0x3);
		}

		/* set led open drain enable */
		RTUSBReadMACRegister(pAd, OPT_14, &data);
		data |= 0x01;
		RTUSBWriteMACRegister(pAd, OPT_14, data, FALSE);

		if (IS_RT3071(pAd))
		{
#ifdef RELEASE_EXCLUDE
			/* 
			   From RT3071 Power Sequence v1.1 document, the Normal Operation Setting Registers as follow :
			   BBP_R138 / RF_R1 / RF_R15 / RF_R17 / RF_R20 / RF_R21.
			*/
#endif /* RELEASE_EXCLUDE */
			/* RF power sequence setup, load RF normal operation-mode setup */
			RT30xxLoadRFNormalModeSetup(pAd);
		}
		else if (IS_RT3070(pAd))
		{	
		/* TX_LO1_en, RF R17 register Bit 3 to 0 */
		RT30xxReadRFRegister(pAd, RF_R17, &RFValue);
		RFValue &= (~0x08);
		/* to fix rx long range issue */
		if (pAd->NicConfig2.field.ExternalLNAForG == 0)
		{
			if ((IS_RT3071(pAd) && ((pAd->MACVersion & 0xffff) >= 0x0211)) || IS_RT3070(pAd))
			{
				RFValue |= 0x20;
			}
		}
		/* set RF_R17_bit[2:0] equal to EEPROM setting at 0x48h */
		if (pAd->TxMixerGain24G >= 1)
		{
			RFValue &= (~0x7);  /* clean bit [2:0] */
			RFValue |= pAd->TxMixerGain24G;
		}
		RT30xxWriteRFRegister(pAd, RF_R17, RFValue);
		
			/* add by johnli, reset RF_R27 when interface down & up to fix throughput problem */
			/* LDORF_VC, RF R27 register Bit 2 to 0 */
			RT30xxReadRFRegister(pAd, RF_R27, &RFValue);
			/* 
			   TX to RX IQ glitch(RF_R27) has been fixed in RT3070(F). 
			   Raising RF voltage is no longer needed for RT3070(F)
			*/
			if ((pAd->MACVersion & 0xffff) < 0x0201)
				RFValue = (RFValue & (~0x77)) | 0x3;
			else
				RFValue = (RFValue & (~0x77));
			RT30xxWriteRFRegister(pAd, RF_R27, RFValue);
			/* end johnli */
		}
        }	

}

VOID RT3070_PowerTuning(
	IN PRTMP_ADAPTER 			pAd,
	IN RSSI_SAMPLE				*pRssi)
{
	/* request by Gary, if Rssi0 > -42, BBP 82 need to be changed from 0x62 to 0x42, , bbp 67 need to be changed from 0x20 to 0x18 */
	if (!pAd->CommonCfg.HighPowerPatchDisabled)
	{
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

		if (((IS_RT3070(pAd) && ((pAd->MACVersion & 0xffff) < 0x0201)) || IS_RT2070(pAd))
				&& !RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))			
		{
			UCHAR RFValue;

	   		if ((pRssi->AvgRssi[0] != 0) && (pRssi->AvgRssi[0] > (pAd->BbpRssiToDbmDelta - 35)))
            {
				/*RT30xxWriteRFRegister(pAd, RF_R27, 0x20); */
				RT30xxReadRFRegister(pAd, RF_R27, (PUCHAR)&RFValue);
				RFValue &= ~0x3;
				RT30xxWriteRFRegister(pAd, RF_R27, (UCHAR)RFValue);
           	}
            else 
            {
				/*RT30xxWriteRFRegister(pAd, RF_R27, 0x23); */
				RT30xxReadRFRegister(pAd, RF_R27, (PUCHAR)&RFValue);
				RFValue |= 0x3;
				RT30xxWriteRFRegister(pAd, RF_R27, (UCHAR)RFValue);
            }
		 }
	}
}

#endif /* RT3070 */

