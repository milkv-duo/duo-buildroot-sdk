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
#ifdef RT33xx

#include "rt_config.h"

#ifndef RTMP_RF_RW_SUPPORT
#error "You Should Enable compile flag RTMP_RF_RW_SUPPORT for this chip"
#endif /* RTMP_RF_RW_SUPPORT */





VOID RT33xxATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd)
{
	UINT32 Value = 0;
	CHAR TxPwer = 0, TxPwer2 = 0;
	UCHAR BbpValue = 0, Channel = 0;
	UCHAR index = 0, RFValue = 0;

#ifdef CONFIG_QA
	/* for QA mode, TX power values are passed from UI */
	if ((pAd->ate.bQATxStart == TRUE) || (pAd->ate.bQARxStart == TRUE))
	{
		if (pAd->ate.Channel != pAd->LatchRfRegs.Channel)			
		{
			pAd->ate.Channel = pAd->LatchRfRegs.Channel;
			pAd->hw_cfg.lan_gain = GET_LNA_GAIN(pAd);
		}
		return;
	}
	else
#endif /* CONFIG_QA */
	Channel = pAd->ate.Channel;

	/* fill Tx power value */
	TxPwer = pAd->ate.TxPower0;
	TxPwer2 = pAd->ate.TxPower1;
	


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
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value &= (~0x6);
		Value |= (0x04);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

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
#endif	
/* here change to 	ATE_CHIP_RX_VGA_GAIN_INIT hook*/
	ATE_CHIP_RX_VGA_GAIN_INIT(pAd);

#ifdef RELEASE_EXCLUDE
	/*
		On 11A, We should delay and wait RF/BBP to be stable
		and the appropriate time should be 1000 micro seconds. 

		2005/06/05 - On 11G, We also need this delay time.
		Otherwise it's difficult to pass the WHQL.
	*/
#endif /* RELEASE_EXCLUDE */
	RtmpOsMsDelay(1);  
}


INT RT33xxATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index)
{
	CHAR TxPower = 0;
	UCHAR RFValue = 0;

#ifdef RT33xx
	/* 
		(non-positive number)
		including the transmit power controlled
		by the MAC and the BBP R1
	*/
	CHAR		TotalDeltaPower = 0; 
	const TX_POWER_TUNING_ENTRY_STRUCT *TxPowerTuningTable = pAd->chipCap.TxPowerTuningTable_2G;
	INT			i,j;
	CHAR		Value;
	ULONG		TxPwr[5];
	PTX_POWER_TUNING_ENTRY_STRUCT pTxPowerTuningEntry = NULL;
#endif /* RT33xx */

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
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Only TxPower0 and TxPower1 are adjustable !\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("TxPower%d is out of range !\n", index));
	}


		if ((IS_RT30xx(pAd)  || IS_RT3390(pAd)))
		{
#ifdef RT33xx		
			if (IS_RT3390(pAd))
			{
				pAd->TxPowerCtrl.idxTxPowerTable = TxPower;

				/* Valid pAd->TxPowerCtrl.idxTxPowerTable: -30 ~ 45 */
				pTxPowerTuningEntry = &TxPowerTuningTable[pAd->TxPowerCtrl.idxTxPowerTable + TX_POWER_TUNING_ENTRY_OFFSET]; /* zero-based array */

				pAd->TxPowerCtrl.RF_TX_ALC = pTxPowerTuningEntry->RF_TX_ALC;
				pAd->TxPowerCtrl.MAC_PowerDelta = pTxPowerTuningEntry->MAC_PowerDelta;

				/* Tx power adjustment over RF */
				RT30xxReadRFRegister(pAd, RF_R12, (PUCHAR)(&RFValue));
				RFValue = ((RFValue & 0xE0) | pAd->TxPowerCtrl.RF_TX_ALC);
				RT30xxWriteRFRegister(pAd, RF_R12, (UCHAR)(RFValue));

				/* Tx power adjustment over MAC */
				TotalDeltaPower += pAd->TxPowerCtrl.MAC_PowerDelta;
				
				if (pAd->ate.TxWI.BW == BW_40)
				{		
					TxPwr[0] = pAd->Tx40MPwrCfgGBand[0];
					TxPwr[1] = pAd->Tx40MPwrCfgGBand[1];
					TxPwr[2] = pAd->Tx40MPwrCfgGBand[2];
					TxPwr[3] = pAd->Tx40MPwrCfgGBand[3];
					TxPwr[4] = pAd->Tx40MPwrCfgGBand[4];
				}
				else
				{		
					TxPwr[0] = pAd->Tx20MPwrCfgGBand[0];
					TxPwr[1] = pAd->Tx20MPwrCfgGBand[1];
					TxPwr[2] = pAd->Tx20MPwrCfgGBand[2];
					TxPwr[3] = pAd->Tx20MPwrCfgGBand[3];
					TxPwr[4] = pAd->Tx20MPwrCfgGBand[4];		
				}

				for (i=0; i<5; i++)
				{
					if (TxPwr[i] != 0xffffffff)
					{
						for (j=0; j<8; j++)
						{
							Value = (CHAR)((TxPwr[i] >> j*4) & 0x0F);							

							/*
								The upper bounds of the MAC 0x1314~0x1324 are variable
								when the STA uses the internal Tx ALC.
							*/
							switch (TX_PWR_CFG_0 + (i * 4))
							{
								case TX_PWR_CFG_0: 
								{
									if ((Value + TotalDeltaPower) < 0)
									{
										Value = 0;
									}
									else if ((Value + TotalDeltaPower) > 0xE)
									{
										Value = 0xE;
									}
									else
									{
										Value += TotalDeltaPower;
									}
								}
								break;

								case TX_PWR_CFG_1: 
								{
									if ((j >= 0) && (j <= 3))
									{
										if ((Value + TotalDeltaPower) < 0)
										{
											Value = 0;
										}
										else if ((Value + TotalDeltaPower) > 0xC)
										{
											Value = 0xC;
										}
										else
										{
											Value += TotalDeltaPower;
										}
									}
									else
									{
										if ((Value + TotalDeltaPower) < 0)
										{
											Value = 0;
										}
										else if ((Value + TotalDeltaPower) > 0xE)
										{
											Value = 0xE;
										}
										else
										{
											Value += TotalDeltaPower;
										}
									}
								}
								break;

								case TX_PWR_CFG_2: 
								{
									if ((j == 0) || (j == 2) || (j == 3))
									{
										if ((Value + TotalDeltaPower) < 0)
										{
											Value = 0;
										}
										else if ((Value + TotalDeltaPower) > 0xC)
										{
											Value = 0xC;
										}
										else
										{
											Value += TotalDeltaPower;
										}
									}
									else
									{
										if ((Value + TotalDeltaPower) < 0)
										{
											Value = 0;
										}
										else if ((Value + TotalDeltaPower) > 0xE)
										{
											Value = 0xE;
										}
										else
										{
											Value += TotalDeltaPower;
										}
									}
								}
								break;

								case TX_PWR_CFG_3: 
								{
									if ((j == 0) || (j == 2) || (j == 3) || 
									    ((j >= 4) && (j <= 7)))
									{
										if ((Value + TotalDeltaPower) < 0)
										{
											Value = 0;
										}
										else if ((Value + TotalDeltaPower) > 0xC)
										{
											Value = 0xC;
										}
										else
										{
											Value += TotalDeltaPower;
										}
									}
									else
									{
										if ((Value + TotalDeltaPower) < 0)
										{
											Value = 0;
										}
										else if ((Value + TotalDeltaPower) > 0xE)
										{
											Value = 0xE;
										}
										else
										{
											Value += TotalDeltaPower;
										}
									}
								}
								break;

								case TX_PWR_CFG_4: 
								{
									if ((Value + TotalDeltaPower) < 0)
									{
										Value = 0;
									}
									else if ((Value + TotalDeltaPower) > 0xC)
									{
										Value = 0xC;
									}
									else
									{
										Value += TotalDeltaPower;
									}
								}
								break;

								default: 
								{							
									/* do nothing */
									DBGPRINT(RT_DEBUG_ERROR, ("%s: unknown register = 0x%X\n", 
										__FUNCTION__, 
										(TX_PWR_CFG_0 + (i * 4))));
								}
								break;
							}
							TxPwr[i] = (TxPwr[i] & ~(0x0000000F << j*4)) | (Value << j*4);							
						}
					}
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + i*4, TxPwr[i]);
				}
			}
			else /* RT30xx */
#endif /* RT33xx */			
			{
				/* Set Tx Power */
				UCHAR ANT_POWER_INDEX=RF_R12+index;
				ATE_RF_IO_READ8_BY_REG_ID(pAd, ANT_POWER_INDEX, (PUCHAR)&RFValue);
				RFValue = (RFValue & 0xE0) | TxPower;
				ATE_RF_IO_WRITE8_BY_REG_ID(pAd, ANT_POWER_INDEX, (UCHAR)RFValue);
				DBGPRINT(RT_DEBUG_TRACE, ("3070 or 2070:%s (TxPower[%d]=%d, RFValue=%x)\n", __FUNCTION__, index,TxPower, RFValue));
			}
		}

	DBGPRINT(RT_DEBUG_TRACE, ("<--- %s\n", __FUNCTION__));
	
	return 0;
}	


VOID RT33xxATERxVGAInit(
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
	ATEBBPWriteWithRxChain(pAd, BBP_R66, R66, RX_CHAIN_ALL);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, 
				("%s :Ch=%d, BW=%d, LNAGain=0x%x, set R66 as 0x%x \n", 
				__FUNCTION__, pATEInfo->Channel, pATEInfo->TxWI.BW, 
				LNAGain, R66));
#endif /* RELEASE_EXCLUDE */

	return;
}



INT	RT33xx_Set_ATE_TX_BW_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	INT powerIndex;
	UCHAR value = 0;
	UCHAR BBPCurrentBW;
	
	BBPCurrentBW = simple_strtol(arg, 0, 10);


	if ((BBPCurrentBW == 0)
		|| IS_RT2070(pAd))
	{
		pAd->ate.TxWI.BW = BW_20;
	}
	else
	{
		pAd->ate.TxWI.BW = BW_40;
 	}

#ifdef RELEASE_EXCLUDE
	/* Fix the error spectrum of CCK-40MHZ. */
	/* Turn on BBP 20MHz mode by request here. */
#endif /* RELEASE_EXCLUDE */
	if ((pAd->ate.TxWI.TXWI_O.PHYMODE == MODE_CCK) && (pAd->ate.TxWI.TXWI_O.BW == BW_40))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_BW_Proc!! Warning!! CCK only supports 20MHZ!!\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("Bandwidth switch to 20!!\n"));
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

				RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx20MPwrCfgGBand[powerIndex]);	
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

				RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx20MPwrCfgABand[powerIndex]);	
 				RtmpOsMsDelay(5);				
 			}
		}

		/* set BW = 20 MHz */
		/* Set BBP R4 bit[4:3]=0:0 */
 		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
 		value &= (~0x18);
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);

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
		else
		/* set BW = 20 MHz */
		{
			pAd->LatchRfRegs.R4 &= ~0x00200000;
			RtmpRfIoWrite(pAd);
		}

		/* BW = 20 MHz */
		/* Set BBP R68=0x0B to improve Rx sensitivity. */
		value = 0x0B;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R68, value);
		/* Set BBP R69=0x16 */
		value = 0x16;
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, value);
		/* Set BBP R70=0x08 */
		value = 0x08;
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, value);
		/* Set BBP R73=0x11 */
	    
		value = 0x11;

 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, value);


#ifdef RELEASE_EXCLUDE
		/*
			If Channel=14, Bandwidth=20M and Mode=CCK, Set BBP R4 bit5=1
			(to set Japan filter coefficients).
			This segment of code will only works when ATETXMODE and ATECHANNEL
			were set to MODE_CCK and 14 respectively before ATETXBW is set to 0.
		*/
		/* 
			Please don't move this block backward.
			BBP_R4 should be overwritten for every chip if the condition matched.
		*/
#endif /* RELEASE_EXCLUDE */
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

				RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx40MPwrCfgGBand[powerIndex]);	
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

				RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx40MPwrCfgABand[powerIndex]);	
				RtmpOsMsDelay(5);				
			}		

			if ((pAd->ate.TxWI.TXWI_O.PHYMODE >= 2) && (pAd->ate.TxWI.TXWI_O.MCS == 7))
			{
    			value = 0x28;
    			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, value);
			}
		}

		/* Set BBP R4 bit[4:3]=1:0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
		value &= (~0x18);
		value |= 0x10;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);

		/* Set BBP R66=0x3C */
		value = 0x3C;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, value);

		/* set BW = 40 MHz */
		if(IS_RT30xx(pAd))
                 {
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R24, (UCHAR) pAd->Mlme.CaliBW40RfR24);
                        ATE_RF_IO_READ8_BY_REG_ID(pAd, RF_R31, &value);
			value |= 0x20;
			ATE_RF_IO_WRITE8_BY_REG_ID(pAd, RF_R31, value);
                  }

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

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_BW_Proc (BBPCurrentBW = %d)\n", pAd->ate.TxWI.BW));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_BW_Proc Success\n"));

#ifdef CONFIG_AP_SUPPORT
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF,  (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}	


VOID RT33xxATEAsicSetTxRxPath(
    IN PRTMP_ADAPTER pAd)
{
/* rt33xx all 1*1 ,so do nothing */
}


/* 
==========================================================================
    Description:
        Set RT5572/RT5592 ATE RF central frequency offset
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	RT33xx_Set_ATE_TX_FREQ_OFFSET_Proc(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	UCHAR RFFreqOffset = 0;
	ULONG R4 = 0;
#ifdef RTMP_RF_RW_SUPPORT
	UCHAR RFValue = 0;
#endif /* RTMP_RF_RW_SUPPORT */

	
	RFFreqOffset = simple_strtol(arg, 0, 10);

#ifdef RTMP_RF_RW_SUPPORT
	/* RT35xx ATE will reuse this code segment. */
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
	if (IS_RT30xx(pAd))
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
#ifdef GEMTEK_ATE
	DBGPRINT(RT_DEBUG_OFF, (KERN_EMERG "Gemtek:Success\n"));
#endif /* GEMTEK_ATE */
#endif /* CONFIG_AP_SUPPORT */
	
	return TRUE;
}



struct _ATE_CHIP_STRUCT RALINK33xx =
{
	/* functions */
	.ChannelSwitch = RT33xxATEAsicSwitchChannel,
	.TxPwrHandler = RT33xxATETxPwrHandler,
	.TssiCalibration = NULL,
	.ExtendedTssiCalibration = NULL,
	.RxVGAInit = RT33xxATERxVGAInit,
	.AsicSetTxRxPath = RT33xxATEAsicSetTxRxPath,
	.AsicExtraPowerOverMAC = NULL,
	.TemperCompensation = NULL,
	
	.Set_BW_Proc = RT33xx_Set_ATE_TX_BW_Proc,
	.Set_FREQ_OFFSET_Proc = RT33xx_Set_ATE_TX_FREQ_OFFSET_Proc,

	/* variables */
	.maxTxPwrCnt = 5,
	.bBBPStoreTXCARR = TRUE,
	.bBBPStoreTXCARRSUPP = TRUE,	
	.bBBPStoreTXCONT = FALSE,
	.bBBPLoadATESTOP = TRUE,/* ralink debug */
};


#endif /* RT33xx */

