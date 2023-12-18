/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2011, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt5592_ate.c

	Abstract:
	Specific ATE funcitons and variables for RT5572/RT5592

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/
#ifndef RT30xx
#error	"You should define RT30xx if you want to compile this file"
#endif /* RT30xx */

#include "rt_config.h"

#ifndef RTMP_RF_RW_SUPPORT
#error "You Should Enable compile flag RTMP_RF_RW_SUPPORT for this chip"
#endif /* RTMP_RF_RW_SUPPORT */





VOID RT30xxATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd)
{
	UINT32 Value = 0;
	CHAR TxPwer = 0, TxPwer2 = 0;
	UCHAR index = 0, BbpValue = 0,  Channel = 0;

#ifdef RTMP_RF_RW_SUPPORT
	/* added to prevent RF register reading error */
	UCHAR RFValue = 0;
#endif /* RTMP_RF_RW_SUPPORT */

#ifdef DOT11N_SS3_SUPPORT
	CHAR TxPwer3 = 0;
#endif /* DOT11N_SS3_SUPPORT */


#ifdef CONFIG_QA
	/* for QA mode, TX power values are passed from UI */
	if ((pAd->ate.bQATxStart == TRUE) || (pAd->ate.bQARxStart == TRUE))
	{
		if (pAd->ate.Channel != pAd->LatchRfRegs.Channel)			
		{
			pAd->ate.Channel = pAd->LatchRfRegs.Channel;
		}
		return;
	}
	else
#endif /* CONFIG_QA */
	Channel = pAd->ate.Channel;

	/* fill Tx power value */
	TxPwer = pAd->ate.TxPower0;
	TxPwer2 = pAd->ate.TxPower1;
#ifdef DOT11N_SS3_SUPPORT
	TxPwer3 = pAd->ate.TxPower2;
#endif /* DOT11N_SS3_SUPPORT */


/* 2008/07/10:KH add to support 3070 ATE<-- */

	/*
		The RF programming sequence is difference between 3xxx and 2xxx.
		The 3070 is 1T1R. Therefore, we don't need to set the number of Tx/Rx path
		and the only job is to set the parameters of channels.
	*/
	if ((IS_RT30xx(pAd)) && 
		((pAd->RfIcType == RFIC_3020) || (pAd->RfIcType == RFIC_2020) ||
		(pAd->RfIcType == RFIC_3021) || (pAd->RfIcType == RFIC_3022) || (pAd->RfIcType == RFIC_3320)))
	{
		/* modify by WY for Read RF Reg. error */

		for (index = 0; index < NUM_OF_3020_CHNL; index++)
		{
			if (Channel == FreqItems3020[index].Channel)
			{
				/* Programming channel parameters. */
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R02, FreqItems3020[index].N);

				ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R03, (PUCHAR)&RFValue);
				RFValue = (RFValue & 0xF0) | (FreqItems3020[index].K&(~0xF0));
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R03, (UCHAR)RFValue);
				               
				ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R06, (PUCHAR)&RFValue);
				RFValue = (RFValue & 0xFC) | FreqItems3020[index].R;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R06, (UCHAR)RFValue);

				/* Set Tx Power. */
				ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R12, (PUCHAR)&RFValue);
				RFValue = (RFValue & 0xE0) | TxPwer;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R12, (UCHAR)RFValue);

				/* Set RF offset. */
				ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R23, (PUCHAR)&RFValue);
				/* 2008/08/06: KH modified "pAd->RFFreqOffset" to "pAd->ate.RFFreqOffset" */
				RFValue = (RFValue & 0x80) | pAd->ate.RFFreqOffset;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R23, (UCHAR)RFValue);

				/* Set BW. */
				if (pAd->ate.TxWI.BW == BW_40)
				{
					RFValue = pAd->Mlme.CaliBW40RfR24;
				}
				else
				{
					RFValue = pAd->Mlme.CaliBW20RfR24;
				}
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR)RFValue);

				/* Enable RF tuning */
				ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R07, (PUCHAR)&RFValue);
				RFValue = RFValue | 0x1;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R07, (UCHAR)RFValue);

				   ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R30, (PUCHAR)&RFValue);
                                RFValue |= 0x80;
                                ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R30, (UCHAR)RFValue);
                                RtmpusecDelay(1000);
                                RFValue &= 0x7F;
                                ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R30, (UCHAR)RFValue);   
				/* latch channel for future usage */
				pAd->LatchRfRegs.Channel = Channel;
				pAd->hw_cfg.lan_gain = GET_LNA_GAIN(pAd);
#if 0
 				if (pAd->Antenna.field.RxPath > 1)
				{
					/* antenna selection */
					ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R01, (PUCHAR)&RFValue);
					ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
	   
					RFValue = (RFValue & ~(0x17)) | 0xC1;
					BbpValue &= 0xE4;
					if (pAd->ate.RxAntennaSel == 1)
					{
						RFValue = RFValue | 0x10;
						BbpValue |= 0x00;

					}
					else if (pAd->ate.RxAntennaSel == 2)
					{
						RFValue = RFValue | 0x04;
						BbpValue |= 0x01;
					}
					else
					{
						/* Only enable two Antenna to receive. */
						BbpValue |= 0x0B;
					}
					
					ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R01, (UCHAR)RFValue);
					ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);  
				}
				
				if (pAd->Antenna.field.TxPath > 1)
				{

					  /* antenna selection */
					ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R01, (PUCHAR)&RFValue);
				       ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BbpValue);
	   
					RFValue = (RFValue & ~(0x2B)) | 0xC1;
					BbpValue &= 0xE7;
					if (pAd->ate.TxAntennaSel == 1)
					{
						RFValue = RFValue | 0x20;
					}
					else if (pAd->ate.TxAntennaSel == 2)
					{
						RFValue = RFValue | 0x08;
						BbpValue |= 0x08;
					}
					else
					{
						BbpValue |= 0x10;
					}
					ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R01, (UCHAR)RFValue);
					ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BbpValue);
	                     }
#endif
				ATEAsicSetTxRxPath(pAd);

				ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R30, (PUCHAR)&RFValue);
				RFValue |= 0x80;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R30, (UCHAR)RFValue);
				RtmpusecDelay(1000);
				RFValue &= 0x7F;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R30, (UCHAR)RFValue);
				break;				
			}
		}

		DBGPRINT(RT_DEBUG_TRACE, ("SwitchChannel#%d(RF=%d, Pwr0=%d, Pwr1=%d, %dT), N=0x%02X, K=0x%02X, R=0x%02X\n",
			Channel, 
			pAd->RfIcType, 
			TxPwer,
			TxPwer2,
			pAd->Antenna.field.TxPath,
			FreqItems3020[index].N, 
			FreqItems3020[index].K, 
			FreqItems3020[index].R));
	}


	/* Change BBP setting during switch from a->g, g->a */
	if (Channel <= 14)
	{
		UINT32 TxPinCfg = 0x00050F0A;/* 2007.10.09 by Brian : 0x0005050A ==> 0x00050F0A */

		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - pAd->hw_cfg.lan_gain));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - pAd->hw_cfg.lan_gain));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - pAd->hw_cfg.lan_gain));
		if (IS_RT3352(pAd) || IS_RT5350(pAd))
		{
			/* Gary : 2010/0721 */
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0x38);
		}
		else
		{
			/* According the Rory's suggestion to solve the middle range issue. */
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0);	
		}

		/* Rx High power VGA offset for LNA select */
		if (pAd->NicConfig2.field.ExternalLNAForG)
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46);
		}
		else
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x84);
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x50);
		}

		/* 2.4 G band selection PIN */
		rtmp_mac_set_band(pAd, BAND_24G);

		if (IS_RT3352(pAd) || IS_RT5350(pAd))
		{
			if (pAd->ate.TxAntennaSel == 1)
			{
				TxPinCfg &= 0xFFFFFFF3;
			}
			else if (pAd->ate.TxAntennaSel == 2)
			{
				TxPinCfg &= 0xFFFFFFFC;
			}
			else
			{
				TxPinCfg &= 0xFFFFFFFF;
			}

			if (pAd->ate.RxAntennaSel == 1)
			{
				TxPinCfg &= 0xFFFFF3FF;
			}
			else if (pAd->ate.RxAntennaSel == 2)
			{
				TxPinCfg &= 0xFFFFFCFF;
			}
			else
			{
				TxPinCfg &= 0xFFFFFFFF;
			}
		}
		else
		{
			/* Turn off unused PA or LNA when only 1T or 1R. */
			if (pAd->Antenna.field.TxPath == 1)
			{
				TxPinCfg &= 0xFFFFFFF3;
			}
			if (pAd->Antenna.field.RxPath == 1)
			{
				TxPinCfg &= 0xFFFFF3FF;
			}

			/* calibration power unbalance issues */
			if (pAd->Antenna.field.TxPath == 2)
			{
				if (pAd->ate.TxAntennaSel == 1)
				{
					TxPinCfg &= 0xFFFFFFF7;
				}
				else if (pAd->ate.TxAntennaSel == 2)
				{
					TxPinCfg &= 0xFFFFFFFD;
				}
			}
		}
		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}
	/* channel > 14 */
	else
	{
	    UINT32	TxPinCfg = 0x00050F05;/* 2007.10.09 by Brian : 0x00050505 ==> 0x00050F05 */
		
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - pAd->hw_cfg.lan_gain));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - pAd->hw_cfg.lan_gain));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - pAd->hw_cfg.lan_gain));
		/* According the Rory's suggestion to solve the middle range issue. */
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0);        

		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0xF2);

		/* Rx High power VGA offset for LNA select */
		if (pAd->NicConfig2.field.ExternalLNAForA)
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46);
		}
		else
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x50);
		}

		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R91, &BbpValue);
		ASSERT((BbpValue == 0x04));

		/* 5 G band selection PIN, bit1 and bit2 are complement */
		rtmp_mac_set_band(pAd, BAND_5G);

		/* Turn off unused PA or LNA when only 1T or 1R. */
		if (pAd->Antenna.field.TxPath == 1)
		{
			TxPinCfg &= 0xFFFFFFF3;
		}

		if (pAd->Antenna.field.RxPath == 1)
		{
			TxPinCfg &= 0xFFFFF3FF;
		}

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}

#if 0
	/* R66 should be set according to Channel. */
	if (Channel <= 14)
	{	
		/* BG band */
			R66 = 0x2E + pAd->hw_cfg.lan_gain;

		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, R66);
	}
	else
	{	
		/* A band, BW == 20 */
		if (pAd->ate.TxWI.BW == BW_20)
		{
			R66 = (UCHAR)(0x32 + (pAd->hw_cfg.lan_gain * 5) / 3);

    		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, R66);
		}
		else
		{
			/* A band, BW == 40 */
			R66 = (UCHAR)(0x3A + (pAd->hw_cfg.lan_gain * 5) / 3);
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, R66);
		}
	}
#endif

//   here will change to ATE_CHIP_RX_VGA_GAIN_INIT hook
	ATE_CHIP_RX_VGA_GAIN_INIT(pAd);

	RtmpOsMsDelay(1);  

#ifndef RTMP_RF_RW_SUPPORT
	if (Channel > 14)
	{
		/* When 5.5GHz band the LSB of TxPwr will be used to reduced 7dB or not. */
		DBGPRINT(RT_DEBUG_TRACE, ("SwitchChannel#%d(RF=%d, %dT) to , R1=0x%08x, R2=0x%08x, R3=0x%08x, R4=0x%08x\n",
								  Channel, 
								  pAd->RfIcType, 
								  pAd->Antenna.field.TxPath,
								  pAd->LatchRfRegs.R1, 
								  pAd->LatchRfRegs.R2, 
								  pAd->LatchRfRegs.R3, 
								  pAd->LatchRfRegs.R4));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("SwitchChannel#%d(RF=%d, Pwr0=%u, Pwr1=%u, %dT) to , R1=0x%08x, R2=0x%08x, R3=0x%08x, R4=0x%08x\n",
								  Channel, 
								  pAd->RfIcType, 
								  (R3 & 0x00003e00) >> 9,
								  (R4 & 0x000007c0) >> 6,
								  pAd->Antenna.field.TxPath,
								  pAd->LatchRfRegs.R1, 
								  pAd->LatchRfRegs.R2, 
								  pAd->LatchRfRegs.R3, 
								  pAd->LatchRfRegs.R4));
    }
#endif /* !RTMP_RF_RW_SUPPORT */
}


INT RT30xxATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index)
{
	ULONG R;
	CHAR TxPower = 0;
	UCHAR Bbp94 = 0;
	BOOLEAN bPowerReduce = FALSE;
#ifdef RTMP_RF_RW_SUPPORT
	UCHAR RFValue = 0;
#endif /* RTMP_RF_RW_SUPPORT */

#ifdef CONFIG_QA
	if ((pAd->ate.bQATxStart == TRUE) || (pAd->ate.bQARxStart == TRUE))
	{
		/* 
			When QA is used for Tx, pAd->ate.TxPower0/1 and real tx power
			are not synchronized.
		*/
		return 0;
	}
	else
#endif /* CONFIG_QA */

	if (index == 0)
	{
		TxPower = pAd->ate.TxPower0;
	}
	else if (index == 1)
	{
		TxPower = pAd->ate.TxPower1;
	}
#ifdef DOT11N_SS3_SUPPORT
	else if (index == 2)
	{
		if (IS_RT2883(pAd) || IS_RT3883(pAd) )
			TxPower = pAd->ate.TxPower2;
		else
		{ 	
			DBGPRINT(RT_DEBUG_ERROR, ("Only TxPower0 and TxPower1 are adjustable !\n"));
			DBGPRINT(RT_DEBUG_ERROR, ("TxPower%d is out of range !\n", index));	
		}	
	}
#endif /* DOT11N_SS3_SUPPORT */
	else
	{
#ifdef DOT11N_SS3_SUPPORT
		DBGPRINT(RT_DEBUG_ERROR, ("Only TxPower0, TxPower1, and TxPower2 are adjustable !\n"));
#else
		DBGPRINT(RT_DEBUG_ERROR, ("Only TxPower0 and TxPower1 are adjustable !\n"));
#endif /* DOT11N_SS3_SUPPORT */
		DBGPRINT(RT_DEBUG_ERROR, ("TxPower%d is out of range !\n", index));
	}

#ifdef RTMP_RF_RW_SUPPORT
		if ((IS_RT30xx(pAd)  || IS_RT3390(pAd)))
		{
			/* Set Tx Power */
			UCHAR ANT_POWER_INDEX=RF_R12+index;
			ATE_RF_IO_READ8_BY_REG_ID(pAd, ANT_POWER_INDEX, (PUCHAR)&RFValue);
			RFValue = (RFValue & 0xE0) | TxPower;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, ANT_POWER_INDEX, (UCHAR)RFValue);
			DBGPRINT(RT_DEBUG_TRACE, ("3070 or 2070:%s (TxPower[%d]=%d, RFValue=%x)\n", __FUNCTION__, index,TxPower, RFValue));
		}
#endif /* RTMP_RF_RW_SUPPORT */
	{
		if (pAd->ate.Channel <= 14)
		{
			if (TxPower > 31)
			{
				/* R3, R4 can't large than 31 (0x24), 31 ~ 36 used by BBP 94 */
				R = 31;
				if (TxPower <= 36)
					Bbp94 = BBPR94_DEFAULT + (UCHAR)(TxPower - 31);		
			}
			else if (TxPower < 0)
			{
				/* R3, R4 can't less than 0, -1 ~ -6 used by BBP 94 */
				R = 0;
				if (TxPower >= -6)
					Bbp94 = BBPR94_DEFAULT + TxPower;
			}
			else
			{  
				/* 0 ~ 31 */
				R = (ULONG) TxPower;
				Bbp94 = BBPR94_DEFAULT;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("%s (TxPower=%d, R=%ld, BBP_R94=%d)\n", __FUNCTION__, TxPower, R, Bbp94));
		}
		else /* 5.5 GHz */
		{
			if (TxPower > 15)
			{
				/* R3, R4 can't large than 15 (0x0F) */
				R = 15;
			}
			else if (TxPower < 0)
			{
				/* R3, R4 can't less than 0 */
				/* -1 ~ -7 */
				ASSERT((TxPower >= -7));
				R = (ULONG)(TxPower + 7);
				bPowerReduce = TRUE;
			}
			else
			{  
				/* 0 ~ 15 */
				R = (ULONG) TxPower;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("%s (TxPower=%d, R=%lu)\n", __FUNCTION__, TxPower, R));
		}

		if (pAd->ate.Channel <= 14)
		{
			if (index == 0)
			{
				/* shift TX power control to correct RF(R3) register bit position */
				R = R << 9;		
				R |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);
				pAd->LatchRfRegs.R3 = R;
			}
			else
			{
				/* shift TX power control to correct RF(R4) register bit position */
				R = R << 6;		
				R |= (pAd->LatchRfRegs.R4 & 0xfffff83f);
				pAd->LatchRfRegs.R4 = R;
			}
		}
		else /* 5.5GHz */
		{
			if (bPowerReduce == FALSE)
			{
				if (index == 0)
				{
					/* shift TX power control to correct RF(R3) register bit position */
					R = (R << 10) | (1 << 9);		
					R |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);
					pAd->LatchRfRegs.R3 = R;
				}
				else
				{
					/* shift TX power control to correct RF(R4) register bit position */
					R = (R << 7) | (1 << 6);		
					R |= (pAd->LatchRfRegs.R4 & 0xfffff83f);
					pAd->LatchRfRegs.R4 = R;
				}
			}
			else
			{
				if (index == 0)
				{
					/* shift TX power control to correct RF(R3) register bit position */
					R = (R << 10);		
					R |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);

					/* Clear bit 9 of R3 to reduce 7dB. */ 
					pAd->LatchRfRegs.R3 = (R & (~(1 << 9)));
				}
				else
				{
					/* shift TX power control to correct RF(R4) register bit position */
					R = (R << 7);		
					R |= (pAd->LatchRfRegs.R4 & 0xfffff83f);

					/* Clear bit 6 of R4 to reduce 7dB. */ 
					pAd->LatchRfRegs.R4 = (R & (~(1 << 6)));
				}
			}
		}
		RtmpRfIoWrite(pAd);
	}

	return 0;
}	


VOID RT30xxATERxVGAInit(
	IN PRTMP_ADAPTER		pAd)
{
	PATE_INFO pATEInfo = &(pAd->ate);
	UCHAR R66 = 0x30;
	CHAR LNAGain = pAd->hw_cfg.lan_gain;
	
	/* R66 should be set according to Channel. */
	if (pATEInfo->Channel <= 14)
	{	
			R66 = 0x2E + LNAGain;
	}
	else
	{	
			/* A band, BW == 20 */
		if (pATEInfo->TxWI.BW == BW_20)
		{
			R66 = (UCHAR)(0x32 + (LNAGain*5)/3);
		}
		else
		{
			/* A band, BW == 40 */
			R66 = (UCHAR)(0x3A + (LNAGain*5)/3);
		}
	}

	ATEBBPWriteWithRxChain(pAd, BBP_R66, R66, RX_CHAIN_ALL);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, 
				("%s :Ch=%d, BW=%d, LNAGain=0x%x, set R66 as 0x%x \n", 
				__FUNCTION__, pATEInfo->Channel, pATEInfo->TxWI.BW, 
				LNAGain, R66));
#endif /* RELEASE_EXCLUDE */


	return;
}



INT	RT30xx_Set_ATE_TX_BW_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT powerIndex;
	UCHAR value = 0;
	UCHAR BBPCurrentBW;
	
	BBPCurrentBW = simple_strtol(arg, 0, 10);

	if ((BBPCurrentBW == 0) || IS_RT2070(pAd))
	{
		pAd->ate.TxWI.BW = BW_20;
	}
	else
	{
		pAd->ate.TxWI.BW = BW_40;
 	}

	/* RT35xx ATE will reuse this code segment. */
	/* Fix the error spectrum of CCK-40MHZ. */
	/* Turn on BBP 20MHz mode by request here. */
	if ((pAd->ate.TxWI.TXWI_O.PHYMODE == MODE_CCK) && (pAd->ate.TxWI.TXWI_O.BW == BW_40))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_BW_Proc!! Warning!! CCK only supports 20MHZ!!\nBandwidth switch to 20\n"));
		pAd->ate.TxWI.BW = BW_20;
	}

	if (pAd->ate.TxWI.BW == BW_20)
	{
		if (pAd->ate.Channel <= 14)
		{
			/* BW=20;G band */
 			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
 			{
				if (pAd->Tx20MPwrCfgGBand[powerIndex] == 0xffffffff)
					continue;

				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx20MPwrCfgGBand[powerIndex]);	
				}
				RtmpOsMsDelay(5);				
			}
		}
		else
		{
			/* BW=20;A band */
 			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
 			{
				if (pAd->Tx20MPwrCfgABand[powerIndex] == 0xffffffff)
					continue;

 				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx20MPwrCfgABand[powerIndex]);	
 				}
 				RtmpOsMsDelay(5);				
 			}
		}

		/* BW=20 */
		{
		/* Set BBP R4 bit[4:3]=0:0 */
 		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
 		value &= (~0x18);
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);
		}


		/* Set BBP R66=0x3C */
		value = 0x3C;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, value);



		/* set BW = 20 MHz */
		if (IS_RT30xx(pAd))
                {
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR) pAd->Mlme.CaliBW20RfR24);
                        ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R31, &value);
			value &= (~0x20);
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R31, value);
                }

		/* set BW = 20 MHz */
		{
			pAd->LatchRfRegs.R4 &= ~0x00200000;
			RtmpRfIoWrite(pAd);
		}
		/* BW = 20 MHz */
		{
			/* Set BBP R68=0x0B to improve Rx sensitivity. */
			value = 0x0B;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R68, value);
			/* Set BBP R69=0x16 */
			if ( IS_RT5390(pAd))
				value = 0x12;
			else
				value = 0x16;
	 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, value);
			/* Set BBP R70=0x08 */
			if ( IS_RT5390(pAd))
				value = 0x0A;
			else
				value = 0x08;
	 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, value);
			/* Set BBP R73=0x11 */
		        if ( IS_RT5390(pAd))
			    value = 0x13;
		        else
			    value = 0x11;

	 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, value);
		}

		/* Please don't move this block backward. */
		/* BBP_R4 should be overwritten for every chip if the condition matched. */
		if (pAd->ate.Channel == 14)
		{
			INT TxMode = pAd->ate.TxWI.TXWI_O.PHYMODE;

			if (TxMode == MODE_CCK)
			{
				/* when Channel==14 && Mode==CCK && BandWidth==20M, BBP R4 bit5=1 */
 				ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
				value |= 0x20; /* set bit5=1 */
 				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);				
			}
		}
	}
	/* If bandwidth = 40M, set RF Reg4 bit 21 = 0. */
	else if (pAd->ate.TxWI.TXWI_O.BW == BW_40)
	{
		if (pAd->ate.Channel <= 14)
		{
			/* BW=40;G band */
			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
			{
				if (pAd->Tx40MPwrCfgGBand[powerIndex] == 0xffffffff)
					continue;

				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx40MPwrCfgGBand[powerIndex]);	
				}
				RtmpOsMsDelay(5);				
			}
		}
		else
		{
			/* BW=40;A band */
			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
			{
				if (pAd->Tx40MPwrCfgABand[powerIndex] == 0xffffffff)
					continue;

				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx40MPwrCfgABand[powerIndex]);	
				}
				RtmpOsMsDelay(5);				
			}		

			if ((pAd->ate.TxWI.TXWI_O.PHYMODE >= 2) && (pAd->ate.TxWI.TXWI_O.MCS == 7))
			{
    			value = 0x28;
    			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, value);
			}
		}

		{
			/* Set BBP R4 bit[4:3]=1:0 */
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
			value &= (~0x18);
			value |= 0x10;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);
		}


#if !defined (RT35xx) && !defined (RT3352) && !defined (RT5350)
		/* Set BBP R66=0x3C */
		value = 0x3C;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, value);
#endif /* !defined (RT35xx) && !defined (RT3352) && !defined (RT5350) */



		/* set BW = 40 MHz */
		if(IS_RT30xx(pAd))
                 {
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR) pAd->Mlme.CaliBW40RfR24);
                        ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R31, &value);
			value |= 0x20;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R31, value);
                  }
		else
		/* set BW = 40 MHz */
		{
		pAd->LatchRfRegs.R4 |= 0x00200000;
		RtmpRfIoWrite(pAd);
		}
		/* BW = 40 MHz */
		{
			/* Set BBP R68=0x0C to improve Rx sensitivity. */
			value = 0x0C;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R68, value);
			/* Set BBP R69=0x1A */
			value = 0x1A;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, value);
			/* Set BBP R70=0x0A */
			value = 0x0A;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, value);
			/* Set BBP R73=0x16 */
		        if (IS_RT5390(pAd))
			    value = 0x13;
		        else
			    value = 0x16;
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, value);
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_BW_Proc (BBPCurrentBW = %d)\n", pAd->ate.TxWI.BW));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_BW_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}	


VOID RT30xxATEAsicSetTxRxPath(
    IN PRTMP_ADAPTER pAd)
{
	UCHAR	RFValue = 0, BbpValue = 0;

	if (pAd->Antenna.field.RxPath > 1)
	{
	/* antenna selection */
		ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R01, (PUCHAR)&RFValue);
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
	   
		RFValue = (RFValue & ~(0x17)) | 0xC1;
		BbpValue &= 0xE4;
		if (pAd->ate.RxAntennaSel == 1)
		{
			RFValue = RFValue | 0x10;
			BbpValue |= 0x00;

		}
		else if (pAd->ate.RxAntennaSel == 2)
		{
			RFValue = RFValue | 0x04;
			BbpValue |= 0x01;
		}
		else
		{
			/* Only enable two Antenna to receive. */
			BbpValue |= 0x0B;
		}
					
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R01, (UCHAR)RFValue);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);  
	}
				
	if (pAd->Antenna.field.TxPath > 1)
	{
		  /* antenna selection */
		ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R01, (PUCHAR)&RFValue);
	       ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BbpValue);
	   
		RFValue = (RFValue & ~(0x2B)) | 0xC1;
		BbpValue &= 0xE7;
		if (pAd->ate.TxAntennaSel == 1)
		{
			RFValue = RFValue | 0x20;
		}
		else if (pAd->ate.TxAntennaSel == 2)
		{
			RFValue = RFValue | 0x08;
			BbpValue |= 0x08;
		}
		else
		{
			BbpValue |= 0x10;
		}
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R01, (UCHAR)RFValue);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BbpValue);
	}
	
	
}


/* 
==========================================================================
    Description:
        Set RT5572/RT5592 ATE RF central frequency offset
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	RT30xx_Set_ATE_TX_FREQ_OFFSET_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR RFFreqOffset = 0;
	ULONG R4 = 0;
#ifdef RTMP_RF_RW_SUPPORT
	UCHAR RFValue = 0;
#endif /* RTMP_RF_RW_SUPPORT */
	
	RFFreqOffset = simple_strtol(arg, 0, 10);

#ifdef RTMP_RF_RW_SUPPORT
	/* RT35xx ATE will reuse this code segment. */
	/* 2008/08/06: KH modified the limit of offset value from 64 to 96(0x5F + 0x01) */
	if (RFFreqOffset >= 96)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_FREQOFFSET_Proc::Out of range(0 ~ 95).\n"));
		return FALSE;
	}
#else
	if (RFFreqOffset >= 64)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_FREQOFFSET_Proc::Out of range(0 ~ 63).\n"));
		return FALSE;
	}
#endif /* RTMP_RF_RW_SUPPORT */

	pAd->ate.RFFreqOffset = RFFreqOffset;

#ifdef RTMP_RF_RW_SUPPORT
	if (IS_RT30xx(pAd) || IS_RT3572(pAd))
	{
		ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R23, (PUCHAR)&RFValue);
		RFValue = ((RFValue & 0x80) | pAd->ate.RFFreqOffset);
		ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R23, (UCHAR)RFValue);
	}
	else
#endif /* RTMP_RF_RW_SUPPORT */
	{
		/* RT28xx */
		/* shift TX power control to correct RF register bit position */
		R4 = pAd->ate.RFFreqOffset << 15;		
		R4 |= (pAd->LatchRfRegs.R4 & ((~0x001f8000)));
		pAd->LatchRfRegs.R4 = R4;
		
		RtmpRfIoWrite(pAd);
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_FREQOFFSET_Proc (RFFreqOffset = %d)\n", pAd->ate.RFFreqOffset));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_FREQOFFSET_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}



struct _ATE_CHIP_STRUCT RALINK30xx =
{
	/* functions */
	.ChannelSwitch = RT30xxATEAsicSwitchChannel,
	.TxPwrHandler = RT30xxATETxPwrHandler,
	.TssiCalibration = NULL,
	.ExtendedTssiCalibration = NULL,
	.RxVGAInit = RT30xxATERxVGAInit,
	.AsicSetTxRxPath = RT30xxATEAsicSetTxRxPath,
	.AsicExtraPowerOverMAC = NULL,
	.TemperCompensation = NULL,
	
	.Set_BW_Proc = RT30xx_Set_ATE_TX_BW_Proc,
	.Set_FREQ_OFFSET_Proc = RT30xx_Set_ATE_TX_FREQ_OFFSET_Proc,

	/* variables */
	.maxTxPwrCnt = 5,
	.bBBPStoreTXCARR = TRUE,
	.bBBPStoreTXCARRSUPP = TRUE,	
	.bBBPStoreTXCONT = FALSE,
	.bBBPLoadATESTOP = TRUE,/* ralink debug */
};

