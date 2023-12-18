/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	misc.c

	Abstract:

	Handling Misc Problem

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Sean Wang	2009-08-12		Create
	John Li		2009-11-30		Modified
*/
#ifdef RELEASE_EXCLUDE
/* Handling Bluetooth Coexistence Problem */
#endif /* RELEASE_EXCLUDE */

#include "rt_config.h"
#include "misc.h"

#ifdef BT_COEXISTENCE_SUPPORT
DECLARE_TIMER_FUNCTION(DetectExec);
BUILD_TIMER_FUNCTION(DetectExec);

#ifdef DOT11N_DRAFT3
#ifdef RELEASE_EXCLUDE
/*
BOOLEAN BtCheckWifiThroughputOverLimit()
*/
#endif /* RELEASE_EXCLUDE */
BOOLEAN WifiThroughputOverLimit(
	IN	PRTMP_ADAPTER	pAd,
	IN  UCHAR WifiThroughputLimit)
{
	BOOLEAN  bIssue4020 = FALSE;
	ULONG tmpReceivedByteCount = 0;
	ULONG tmpTransmittedByteCount = 0;
	static ULONG TxByteCount = 0;
	static ULONG RxByteCount = 0;
	static ULONG TxRxThroughputPerSeconds = 0; /*Unit: bytes
*/
	LONG diffTX = 0;
	LONG diffRX = 0;

	bIssue4020 = FALSE;
	
	if (pAd == NULL)
	{
		return FALSE;
	}
	
	if (IS_ENABLE_40TO20_BY_TIMER(pAd))
	{	
		tmpReceivedByteCount = pAd->RalinkCounters.ReceivedByteCount; 
		tmpTransmittedByteCount = pAd->RalinkCounters.TransmittedByteCount;		
					
		if ((TxByteCount != 0) || (RxByteCount != 0 ))
		{				
			diffTX = (LONG)(((tmpTransmittedByteCount - TxByteCount)*5) >> 3);
			diffRX = (LONG)(((tmpReceivedByteCount - RxByteCount)* 5) >> 3);		
			#if 0
			diffTX = (LONG)(tmpTransmittedByteCount - TxByteCount);
			diffRX = (LONG)(tmpReceivedByteCount - RxByteCount);
			#endif
			
			if ((diffTX > 0) && (diffRX > 0 ))
			{
				TxRxThroughputPerSeconds = diffTX + diffRX ;/*Unit: bytes 
*/
			}
			else if ((diffTX < 0) && (diffRX > 0))
			{
				TxRxThroughputPerSeconds = diffRX;
			}
			else if ((diffTX > 0) && (diffRX < 0))
			{
				TxRxThroughputPerSeconds = diffTX;
			}
			else 
			{
				TxRxThroughputPerSeconds = 0;
			}

			DBGPRINT(RT_DEBUG_INFO,("TxRxThroughputPerSeconds = %ld Bps, %ld KBps, %ldKbps, %ldMbps", 
				TxRxThroughputPerSeconds,
				(TxRxThroughputPerSeconds >> 10),
				(TxRxThroughputPerSeconds >> 7),
				(TxRxThroughputPerSeconds >> 17)));
		}
	
		TxByteCount = tmpTransmittedByteCount;
		RxByteCount = tmpReceivedByteCount;

		DBGPRINT(RT_DEBUG_INFO,("btWifiThr = %d, TxByteCount = %ld, RxByteCount = %ld",
			WifiThroughputLimit,
			TxByteCount,
			RxByteCount));

		if ((TxRxThroughputPerSeconds >> 17) > WifiThroughputLimit)
		{
			bIssue4020 = TRUE;
		}
		else
		{
			bIssue4020 = FALSE;
		}
	}
	
	return bIssue4020;
}
#endif /* DOT11N_DRAFT3 */

#ifdef RELEASE_EXCLUDE
/*
BLUETOOTH_BUSY_DEGREE BtCheckBusy()
*/
#endif /* RELEASE_EXCLUDE */
BUSY_DEGREE CheckBusy(
	IN PLONG History, 
	IN UCHAR HistorySize)
{
	if (History == NULL)
	{
		return BUSY_0;
	}

	DBGPRINT(RT_DEBUG_INFO,(" ---> Check Busy %ld %ld %ld %ld %ld",
		*History,
		*(History+1),
		*(History+2),
		*(History+3),
		*(History+4)));

#ifdef RTMP_USB_SUPPORT
	if ((*History > 200000) || 
		(*(History+1) > 200000) || 
		(*(History+2) > 200000) ||
		(*(History+3) > 200000) ||
		(*(History+4) > 200000))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("200000, 200000, 200000, 200000, 200000 ==> Inquiry + other Profiles ... BT lagging .. BUSY_5\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_5; 
	}
	
	if (((*History >= 20000) || 
		(*(History+1) >= 20000) || 
		(*(History+2) >= 20000) || 
		(*(History+3) >= 20000) || 
		(*(History+4) >= 20000))&&
		(*History >= 10000) && 
		(*(History+1) >= 10000) && 
		(*(History+2) >= 10000) && 
		(*(History+3) >= 10000) && 
		(*(History+4) >= 10000))	
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("10000, 10000, 20000, 10000, 10000 ==> Multiple Profile... BUSY_4\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_4;	
	}
	
	if (((*History >= 12000) || 
		(*(History+1) >= 12000) || 
		(*(History+2) >= 12000) || 
		(*(History+3) >= 12000) || 
		(*(History+4) >= 12000))&&
		(*History >= 5000) && 
		(*(History+1) >= 5000) && 
		(*(History+2) >= 5000) && 
		(*(History+3) >= 5000) && 
		(*(History+4) >= 5000))	
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("5000, 5000, 12000, 5000, 5000 ==> BT work ... Jabra BT3030 A2DP Playing Music  ... BUSY_3\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_3;	
	}
	
	if (((*History >= 12000) || 
		(*(History+1) >= 12000) || 
		(*(History+2) >= 12000) || 
		(*(History+3) >= 12000) || 
		(*(History+4) >= 12000))&&
		(*History >= 2000) && 
		(*(History+1) >= 2000) && 
		(*(History+2) >= 2000) && 
		(*(History+3) >= 2000) && 
		(*(History+4) >= 2000))	
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("2000, 2000, 12000, 2000, 2000 ==> BT works ... CSR mouse suddenly moves ... BUSY_2\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_2;
	}

	if (((*History >= 10000) || 
		(*(History+1) >= 10000) || 
		(*(History+2) >= 10000) || 
		(*(History+3) >= 10000) || 
		(*(History+4) >= 10000)) &&
		((*History > 0) && 
		(*(History+1) > 0) && 
		(*(History+2) > 0) &&
		(*(History+3) > 0) &&
		(*(History+4) > 0)))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("1,1,10000,1,1 ==> General Profile .. BUSY_1\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_1;
	}	
#endif /* RTMP_USB_SUPPORT */
#ifdef RTMP_PCI_SUPPORT
	if ((*History > 40) || 
		(*(History+1) > 40) || 
		(*(History+2) > 40) ||
		(*(History+3) > 40) ||
		(*(History+4) > 40))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("0,0,40,0,0 ==> Inquiry + other Profiles ... BT lagging .. BUSY_5\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_5; 
	}

	if ((*History > 33) || 
		(*(History+1) > 33) || 
		(*(History+2) > 33) ||
		(*(History+3) > 33) ||
		(*(History+4) > 33))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("0,0,33,0,0 ==> Inquiry  .. BUSY_5\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_5; 
	}
	
	if (((*History >= 20) && 
		(*(History+1) >= 20) && 
		(*(History+2) >= 20) &&
		(*(History+3) >= 20) &&
		(*(History+4) >= 20)))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("20,20,20,20,20 ==> Multiple Profile... BUSY_4\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_4;	
	}

	if ((*History >= 8) && 
		(*(History+1) >= 8) && 
		(*(History+2) >= 8) &&
		(*(History+3) >= 8) &&
		(*(History+4) >= 8))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("8,8,8,8,8 ==> BT work ... Jabra BT3030 A2DP Playing Music  ... BUSY_3\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_3;
	}

	if ((*History >= 10) || 
		(*(History+1) >= 10) || 
		(*(History+2) >= 10) || 
		(*(History+3) >= 10) || 
		(*(History+4) >= 10))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("0,0,10,0,0 ==> BT works ... CSR mouse suddenly moves ... BUSY_2\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_2;
	}

	if (((*History >= 4) || 
		(*(History+1) >= 4) || 
		(*(History+2) >= 4) || 
		(*(History+3) >= 4) || 
		(*(History+4) >= 4))&&
		(*History >= 2) && 
		(*(History+1) >= 2) && 
		(*(History+2) >= 2) && 
		(*(History+3) >= 2) && 
		(*(History+4) >= 2))	
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("1,1,3,1,1 ==> BT works ... CSR keyboard typing ... BUSY_2\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_2;
	}

	if (((*History >= 1) || 
		(*(History+1) >= 1) || 
		(*(History+2) >= 1) || 
		(*(History+3) >= 1) || 
		(*(History+4) >= 1) ) &&
		((*History >= 0) && 
		(*(History+1) >= 0) && 
		(*(History+2) >= 0) &&
		(*(History+3) >= 0) &&
		(*(History+4) >= 0)))
	{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO,("0,0,1,0,0 ==> BT idle .. BUSY_1\n"));
#endif /* RELEASE_EXCLUDE */
		return BUSY_1;
	}	
#endif /* RTMP_PCI_SUPPORT */

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO,("ALL BT Profile Disconnected .. BUSY_0"));
#endif /* RELEASE_EXCLUDE */
	return BUSY_0;
}

#ifdef RELEASE_EXCLUDE
/*
	VOID BtCoexistAdjust()

	Wifi adjust according to BT Acitivitity  
	Case 1: LNA Middle Gain 	 @HT40/HT20 and BUSY_1 equal & above
	Case 2: TxPower 				 @HT40/HT20 and BUSY_1 equal & above
	Case 3: BA Window Size		 @HT40/HT20 and BUSY_2 equal & above
	Case 4: BA Density			@HT40/HT20 and BUSY_2 equal & above
	Case 5: MCS Rate				@HT40 and BUSY_2 equal & above
	Case 6: REC BA Request			@HT40/HT20 and BUSY_2 equal & above
	Case 7: 40/20 BSS Coexistence	@HT40 and BUSY_4
*/
#endif /* RELEASE_EXCLUDE */
VOID Adjust(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN			bIssue4020, 
	IN ULONG			NoBusyTimeCount)
{
	CHAR	Rssi;
#ifdef RT5390
	ULONG MACValue=0;
	ULONG BbpR65=0;
#endif /* RT5390 */
#ifdef RT35xx
	if (IS_RT3592BC8(pAd) && (pAd->CommonCfg.Channel <= 14))
	{
		Rssi = pAd->StaCfg.RssiSample.AvgRssi[0];
	}
	else
#endif // RT35xx //
	Rssi = RTMPMaxRssi(pAd, 
					   pAd->StaCfg.RssiSample.AvgRssi[0], 
					   pAd->StaCfg.RssiSample.AvgRssi[1], 
					   pAd->StaCfg.RssiSample.AvgRssi[2]);
	
	DBGPRINT(RT_DEBUG_INFO,("RSSI = %d\n", Rssi));

	if (IS_ENABLE_LNA_MID_GAIN_DOWN_BY_TIMER(pAd))
	{
		UCHAR BbpR65 = 0;
#ifdef RT5390
			pAd->BT_BC_RESTORE_R65=TRUE;
			if (IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)
#ifdef KHNote
				&& (!pAd->bDisableR65)
#endif /* KHNote */
				)
			{
				if(pAd->BusyDegree <= BUSY_1)
				{
					if (Rssi <= -45)
					{
						/* -45 ~ -72 Middle LNA */
						if (Rssi > -72)
						{
							RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
							if (BbpR65 != 0x29)
							{
								BbpR65 = 0x29;
								RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BbpR65);
							}
							DBGPRINT(RT_DEBUG_TRACE,("CoBT===> ...........Rssi[  -45 < %2d <= -72].. Set R65 to 0x29", Rssi));
						}
						else
						{
							/* -72 ~-100 High LNA */
							RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
							if (BbpR65 != 0x2c)
							{
								BbpR65 = 0x2c;
								RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BbpR65);
							}
							DBGPRINT(RT_DEBUG_TRACE,("CoBT===> ...........Rssi[  %2d <= -72].. Set R65 to 0x2C", Rssi));
						}
					}
					else
					{
						/* 0 ~ -45 Low LAN */
						RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
						if (BbpR65 != 0x25)
						{
							BbpR65 = 0x25;
							RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BbpR65);
						}
						DBGPRINT(RT_DEBUG_TRACE,("CoBT===> ...........Rssi[ -35 < %2d ].. Set R65 to 0x25", Rssi));
					}
				}
				else
				{
					RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
					if (BbpR65 != 0x2c)
					{
						BbpR65 = 0x2c;
						RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BbpR65);
					}
					DBGPRINT(RT_DEBUG_TRACE,("CoBT===> ...........Rssi[%2d], BusyDegree[%d > 3].. Set R65 to 0x2C", Rssi, pAd->BusyDegree));
				}
			}
			else
#endif /* RT5390 */
		if (pAd->BusyDegree >= BUSY_1)
		{
			DBGPRINT(RT_DEBUG_INFO, ("Lower LNA Middle Gain at HT20 or HT40\n"));
			pAd->bPermitLnaGainDown = TRUE;

			DBGPRINT(RT_DEBUG_INFO,("RSSI = %d\n", Rssi));

			if (Rssi <= -35)
			{
			/* if RSSI is smaller than -80, then set R65 to High Gain to fix long distance issue */
				if (Rssi <= -80)
				{
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

					if (BbpR65 == 0x29)
					{
						DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
						BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
					}
				}
				else
				{
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

					if (BbpR65 == 0x2C)
					{
						DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x29 from 0x2C (Middle LNA)\n"));
						BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x29); /* Middle LNA Gain */
					}
				}
			}
			else
			{
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

				if (BbpR65 == 0x29)
				{
					DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
				}
			}
		}
		else
		{
			if (NoBusyTimeCount > IDLE_STATE_THRESHOLD)
			{	
			DBGPRINT(RT_DEBUG_INFO, ("Lower LNA Middle Gain at HT20 or HT40 (Highest LNA)\n"));

			pAd->bPermitLnaGainDown = FALSE;
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
				if (BbpR65 == 0x29)
				{
					DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
				}
			}
			else
			{
				DBGPRINT(RT_DEBUG_INFO, ("Lower LNA Middle Gain at HT20 or HT40\n"));
			}
		}
	}

	if (IS_ENABLE_TX_POWER_DOWN_BY_TIMER(pAd))
	{

#ifdef RT5390
		if ((IS_RT3090A(pAd) && (pAd->BusyDegree <= BUSY_4)) || 
			((IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)) && (pAd->BusyDegree <= BUSY_1)))
#else
		if (pAd->BusyDegree >= BUSY_1)
#endif /* RT5390 */
		{
			DBGPRINT(RT_DEBUG_INFO, ("Lower Tx Power at HT20 or HT40\n"));
				pAd->bPermitTxPowerDown= TRUE;
		}
		else
		{
			if (NoBusyTimeCount > IDLE_STATE_THRESHOLD)
			{
				DBGPRINT(RT_DEBUG_INFO, ("Higher Tx Power at HT20 or HT40\n"));
					pAd->bPermitTxPowerDown= FALSE;
			}
			else
			{
				DBGPRINT(RT_DEBUG_INFO, ("Lower Tx Power at HT20 or HT40\n"));
			}
		}
	}

#ifdef DOT11_N_SUPPORT
	if (IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(pAd))
	{
	/* Fixed long distance issue */
#ifdef RT5390

		if (((IS_RT3090A(pAd) && (pAd->BusyDegree <= BUSY_3)) || 
					((IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)) && (pAd->BusyDegree <= BUSY_1))) && 
					(((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
					(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
					(Rssi <= -80)))
#else
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
#endif /* RT5390 */
		{
			pAd->bPermitTxBaSizeDown = TRUE;
		}
		else
		{
			pAd->bPermitTxBaSizeDown = FALSE;
		}
	}

	if (IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(pAd))
	{
	/* Fixed long distance issue */
#ifdef RT5390
		if (((IS_RT3090A(pAd) && (pAd->BusyDegree <= BUSY_3)) || 
		((IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)) && (pAd->BusyDegree <= BUSY_1))) && 
		(((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80)))
#else
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
#endif /* RT5390 */
		{
			pAd->bPermitTxBaDensityDown = TRUE;
		}
		else
		{
			pAd->bPermitTxBaDensityDown = FALSE;
		}
	}
		
	if (IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd))
	{
#ifdef RT5390
		if (((IS_RT3090A(pAd) && (pAd->BusyDegree <= BUSY_3)) || 
			((IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)) && (pAd->BusyDegree <= BUSY_1))) && 
			(pAd->CommonCfg.BBPCurrentBW == BW_40))
#else
		if ((pAd->BusyDegree >= BUSY_2) && 
				(pAd->CommonCfg.BBPCurrentBW == BW_40))
#endif // RT5390 //
		{
			pAd->bPermitMcsDown = TRUE;
		}
		else
		{
			pAd->bPermitMcsDown = FALSE;
		}
	}
#ifdef RT5390
			if(IS_ENABLE_BT_RATE_UP_BY_TIMER(pAd))
			{
				if ((IS_RT3090A(pAd) && (pAd->BusyDegree <= BUSY_4)) || 
					((IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)) && (pAd->BusyDegree <= BUSY_1)))
				{
					DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. ++ Up Tx Rate at HT20 or HT40\n"));
					pAd->BT_BC_PERMIT_TXRATE_UP=TRUE;
				}
				else
				{
					if( NoBusyTimeCount > IDLE_STATE_THRESHOLD)
					{
						DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. -- Up Tx Rate at HT20 or HT40\n"));
						pAd->BT_BC_PERMIT_TXRATE_UP=FALSE;
					}
					else
					{
						DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. ++ Up Tx Rate at HT20 or HT40\n"));
					}
				}
			}
			if(IS_ENABLE_BT_WIFI_ACTIVE_PULL_HIGH_BY_TIMER(pAd))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. OneSecRxOkCnt = %d, Last Rx=%d, Tx=%d\n", 
						pAd->RalinkCounters.OneSecRxOkCnt, pAd->RalinkCounters.OneSecReceivedByteCount, pAd->RalinkCounters.OneSecTransmittedByteCount));

				if (IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd))
				{
					if ((pAd->BusyDegree <= BUSY_1) && (Rssi < -50) && (pAd->RalinkCounters.OneSecReceivedByteCount > (pAd->RalinkCounters.OneSecTransmittedByteCount + 1000)) && (pAd->CommonCfg.Channel <= 14))
					{
						RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);
						if ((MACValue & 0x2000) == 0)
						{
							MACValue |= 0x2000;
							RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , MACValue);
						}
						pAd->BT_BC_RESTORE_MAC_SYS_CTRL=TRUE;
						DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. ++!! Enable TX halt BT\n"));
					}
					else
					{
						RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);
						if (MACValue & 0x2000)
						{
							MACValue &= (~0x2000);
							RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , MACValue);
						}
						pAd->BT_BC_RESTORE_MAC_SYS_CTRL=FALSE;
						DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. --!! Enable TX halt BT\n"));
					}
				}
				else
				{
					if((pAd->BusyDegree <= BUSY_1) && (pAd->RalinkCounters.OneSecRxOkCnt > 40))
					{
						RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);
						if(MACValue != 0x224c)
						{	
							RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , 0x224c);
						}
						
						DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. ++!! Enable Rx Wifi Active\n"));

						pAd->BT_BC_PERMIT_RXWIFI_ACTIVE=TRUE;
					}
					else
					{
					/*   if(VeryBusyTimeCount == 0) 
						{ */
							RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);

							if(MACValue == 0x224c)
							{
								if(IS_ENABLE_WIFI_ACTIVE_PULL_LOW_BY_FORCE(pAd))
								{
									MACValue = 0x024c ;		
								}
								else
								{
									MACValue = 0x124c ;
								}
								RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , MACValue);
							}
							DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. --!! Enable Rx Wifi Active\n"));
							pAd->BT_BC_PERMIT_RXWIFI_ACTIVE=FALSE;
						/*}
						  else
						  {
						  	DBGPRINT(RT_DEBUG_TRACE, ("CoBT===> ............. ++ Enable Rx Wifi Active\n"));
						   }*/
					}
				}
			}
#endif /* RT5390 */

	if (IS_ENABLE_REJECT_ORE_BA_BY_TIMER(pAd))
	{	
	/* Fixed long distance issue */
#ifdef RT5390
		if (((IS_RT3090A(pAd) && (pAd->BusyDegree <= BUSY_3)) || 
					((IS_RT5390BC8(pAd) || IS_RT3592BC8(pAd)) && (pAd->BusyDegree <= BUSY_1))) && 
					(((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
						(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
						(Rssi <= -80)))
#else
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
#endif /* RT5390 */
		{
			BASessionTearDownALL(pAd, BSSID_WCID);
			pAd->bPermitRecBaDown = TRUE;
		}
		else
		{
			pAd->bPermitRecBaDown = FALSE;
		}
	}
	
#ifdef DOT11N_DRAFT3
	if (IS_ENABLE_40TO20_BY_TIMER(pAd))
	{
	if (((pAd->BusyDegree >= WIFI_2040_SWITCH_THRESHOLD) && 
			(pAd->BusyDegree != BUSY_5)) && 
			(pAd->CommonCfg.BBPCurrentBW == BW_40) && 
			(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SCAN_2040)) &&
			(bIssue4020 == TRUE))
		{
			BSS_2040_COEXIST_IE OldValue;

		DBGPRINT(RT_DEBUG_INFO, ("HT40 --> HT20\n"));
		DBGPRINT(RT_DEBUG_INFO,("ACT - Update2040CoexistFrameAndNotify. BSSCoexist2040 = %x. EventANo = %d. \n", pAd->CommonCfg.BSSCoexist2040.word, pAd->CommonCfg.TriggerEventTab.EventANo));
			OldValue.word = pAd->CommonCfg.BSSCoexist2040.word;
			pAd->CommonCfg.BSSCoexist2040.word = 0;

			/*if (pAd->CommonCfg.TriggerEventTab.EventBCountDown > 0)
*/
			pAd->CommonCfg.BSSCoexist2040.field.BSS20WidthReq = 1;

		/*
			Need to check !!!!
			How STA will set Intolerant40 if implementation dependent. Now we don't set this bit first!!!!!
			So Only check BSS20WidthReq change.
		*/
			/*if (OldValue.field.BSS20WidthReq != pAd->CommonCfg.BSSCoexist2040.field.BSS20WidthReq)
*/
			{
				Send2040CoexistAction(pAd, BSSID_WCID, TRUE);
			}
		}
	}
#endif /* DOT11N_DRAFT3 */
#endif /* DOT11_N_SUPPORT */
}

#ifdef RELEASE_EXCLUDE
/*
VOID BtCoexistTxPowerDown()
*/
#endif /* RELEASE_EXCLUDE */
VOID TxPowerDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				Rssi,
	INOUT CHAR			*pDeltaPowerByBbpR1, 
	INOUT CHAR			*pDeltaPwr)
{
#ifdef RELEASE_EXCLUDE
	/* If bluetooth is busy, to lower Tx Power to balance with BT power */
#endif /* RELEASE_EXCLUDE */
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		IS_ENABLE_TX_POWER_DOWN_BY_TIMER(pAd) &&
		(pAd->bPermitTxPowerDown== TRUE) && 
		(pAd->CommonCfg.TxPowerPercentage == 0xffffffff))
	{
		CHAR DeltaPowerByBbpR1 = *pDeltaPowerByBbpR1;
		CHAR DeltaPwr = *pDeltaPwr;

		DBGPRINT(RT_DEBUG_INFO, (">> ++ DeltaPwr =%d, DeltaPowerByBbpR1= %d, RSSI = %d\n",
			DeltaPwr, DeltaPowerByBbpR1, Rssi));

		if (Rssi <= -80)
		{	
			DBGPRINT(RT_DEBUG_INFO, (">> -1 75%% power dbm\n"));
			DeltaPowerByBbpR1 = 0;
			DeltaPwr -= 1;
		}
		else if ((Rssi <= -50) && (Rssi >= -60))
		{
			DeltaPowerByBbpR1 = -6;
			DeltaPwr -= 1;

			DBGPRINT(RT_DEBUG_INFO, (">> -7 18%% power dbm\n"));
		}
		else if ((Rssi <= -60) && (Rssi >= -70))
		{
			DeltaPowerByBbpR1 = -6;
			DBGPRINT(RT_DEBUG_INFO, (">> -6 25%% power dbm\n"));
		}
		else if ((Rssi <= -70) && (Rssi >= -80)) 
		{
			DBGPRINT(RT_DEBUG_INFO, (">> -3 50%% dbm\n"));
			DeltaPowerByBbpR1 = 0;
			DeltaPwr -= 3;
		}
		else /* drop into lowest power
*/
		{
			DeltaPowerByBbpR1 = 0;
			DeltaPowerByBbpR1 -= 12;
		}
		
		*pDeltaPowerByBbpR1 = DeltaPowerByBbpR1;
		*pDeltaPwr = DeltaPwr;
			
		DBGPRINT(RT_DEBUG_INFO, (">> -- DeltaPwr =%d, DeltaPowerByBbpR1= %d, RSSI = %d\n", 
			DeltaPwr, DeltaPowerByBbpR1, Rssi));
	}
}

#ifdef RELEASE_EXCLUDE
/*
VOID BtCoexistMcsDown()
*/
#endif /* RELEASE_EXCLUDE */
VOID McsDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				CurrRateIdx, 
	IN RTMP_RA_LEGACY_TB *pCurrTxRate, 
	INOUT CHAR			*pUpRateIdx, 
	INOUT CHAR			*pDownRateIdx)
{
#ifdef RELEASE_EXCLUDE
	/* If bluetooth is busy, now we decrease rate to MCS Thresdhold */
#endif /* RELEASE_EXCLUDE */
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd) && 
		(pAd->bPermitMcsDown == TRUE))
	{
		UCHAR	btMCSThreshold = 0x00;
		UCHAR	UpRateIdx = *pUpRateIdx;
		UCHAR	DownRateIdx = *pDownRateIdx;

		btMCSThreshold = (UCHAR)(GET_PARAMETER_OF_MCS_THRESHOLD(pAd));
		/*0,1,2,3 => MCS= 3, 4, 5, 6
*/
		if (btMCSThreshold <= 0x3)
		{
			btMCSThreshold = btMCSThreshold + 0x3;
		}
		else
		{
			btMCSThreshold = 0x03;
		}
		
		if (CurrRateIdx > 0)
		{
			/* Rate is equeal to btMCSThreshold
*/
			if ((pCurrTxRate->CurrMCS == btMCSThreshold)) 
			{
				UpRateIdx = CurrRateIdx;
			}
	
			/* Rate be decreased to btMCSThreshold
*/
			else if (pCurrTxRate->CurrMCS > btMCSThreshold)
			{
				UpRateIdx = CurrRateIdx - 1;
			}
			/*Rate is under btMCSThreshold
*/
			/*else
*/
			/*{
*/
			/*	UpRateIdx = CurrRateIdx + 1;
*/
			/*}
*/
			/*DownRateIdx = CurrRateIdx - 1;	
*/
	
			DBGPRINT(RT_DEBUG_INFO,("CurrRateIdx=%d, UpRateIdx=%d, DownRateIdx=%d\n",
				CurrRateIdx, UpRateIdx, DownRateIdx));
		}

		*pUpRateIdx = UpRateIdx;
		*pDownRateIdx = DownRateIdx;
	}
}

#ifdef RELEASE_EXCLUDE
/*
VOID BtCoexistMcsDown2()
*/
#endif /* RELEASE_EXCLUDE */
VOID McsDown2(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR			MCS3, 
	IN UCHAR			MCS4, 
	IN UCHAR			MCS5, 
	IN UCHAR			MCS6, 
	INOUT UCHAR			*pTxRateIdx)
{
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd) && 
		(pAd->bPermitMcsDown == TRUE))
	{
		UCHAR	btMCSThreshold = 0x00;
		UCHAR	TxRateIdx = *pTxRateIdx;

		btMCSThreshold = (UCHAR)(GET_PARAMETER_OF_MCS_THRESHOLD(pAd));
		/*0,1,2,3 => MCS= 3, 4, 5, 6
*/
		if (btMCSThreshold <= 0x3)
		{
			btMCSThreshold = btMCSThreshold + 0x3;
		}
		else
		{
			btMCSThreshold = 0x03;
		}
	
		if (btMCSThreshold == 0x03)
		{
			btMCSThreshold = MCS3;
		}
		else if (btMCSThreshold == 0x04)
		{
			btMCSThreshold = MCS4;
		}
		else if (btMCSThreshold == 0x05)
		{
			btMCSThreshold = MCS5;
		}
		else if (btMCSThreshold == 0x06)
		{
			btMCSThreshold = MCS6;
		}
		else
		{
			btMCSThreshold = MCS3;
		}
	
		if (TxRateIdx > btMCSThreshold)
		{
			TxRateIdx = btMCSThreshold; 
		}

		*pTxRateIdx = TxRateIdx;
	}
}

#ifdef RELEASE_EXCLUDE
/*
INT BtCoexistTxBaSizeDown()
*/
#endif /* RELEASE_EXCLUDE */
UCHAR TxBaSizeDown(RTMP_ADAPTER *pAd, UCHAR old_basize)
{
	UCHAR new_basize = old_basize;

	if ((pAd->bHWCoexistenceInit == TRUE) && 
		INFRA_ON(pAd) && 
		(pAd->OpMode == OPMODE_STA) && 
		(old_basize != 0) && 
		IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(pAd) && 
		(pAd->bPermitTxBaSizeDown == TRUE))
	{
		UCHAR btAMPDUSize = 0;

		/* When Bluetooh is busy, we set the BA Size to smaller */
		btAMPDUSize = (UCHAR)(GET_PARAMETER_OF_AMPDU_SIZE(pAd));
		if (btAMPDUSize <= 3)
		{
			/*0,1,2,3 => ba size=1, 3, 5, 7 */
			if (btAMPDUSize == 0)
				new_basize = 1;
			else
				new_basize = 1 + (btAMPDUSize << 1);
		}
		else 
		{
			new_basize = 1;
		}
		
		DBGPRINT(RT_DEBUG_INFO, (">>>>> dynamic BAWinSize = %d, profile AMPDU size=%ld\n", 
					new_basize, GET_PARAMETER_OF_AMPDU_SIZE(pAd)));	
	}

	return BAWinSize;
}


#ifdef RELEASE_EXCLUDE
/*
VOID BtCoexistTxBaDensityDown()
*/
#endif /* RELEASE_EXCLUDE */
UCHAR TxBaDensityDown(RTMP_ADAPTER *pAd, UCHAR mpdu_density)
{
	UCHAR density = mpdu_density;

	if ((pAd->bHWCoexistenceInit == TRUE) && 
		INFRA_ON(pAd) && 
		(pAd->OpMode == OPMODE_STA) && 
		IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(pAd) && 
		(pAd->bPermitTxBaDensityDown == TRUE))
	{
		UCHAR btAMPDUDensity = 0;

		/* When Bluetooh is busy, we set the BA density to larger */
		btAMPDUDensity = (UCHAR)(GET_PARAMETER_OF_AMPDU_DENSITY(pAd));

		/*
			0,1,2,3 => BA density=0x01, 0x03, 0x05, 0x07
				    =>                   0.25u, 1u,   4u,     16u
		*/
		density = (btAMPDUDensity <= 3) ? (1 + (btAMPDUDensity << 1)) : 0x07;
		
		DBGPRINT(RT_DEBUG_INFO, (">>>>> dynamic BA density= %d, profile AMPDU density=%ld\n", 
					density, GET_PARAMETER_OF_AMPDU_DENSITY(pAd)));
	}

	return density;
}


#ifdef RELEASE_EXCLUDE
/*
VOID BtCoexistReadParametersFromFile()
*/
VOID ReadParametersFromFile(
	IN PRTMP_ADAPTER pAd,
	RTMP_STRING *tmpbuf,
	RTMP_STRING *pBuffer)
{
	if (RTMPGetKeyParameter("BTCoexistenceOn", tmpbuf, 10, pBuffer, TRUE))
	{
		if (simple_strtol(tmpbuf, 0, 10) == 0)
			pAd->bMiscOn = FALSE;
		else
			pAd->bMiscOn = TRUE;
	
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_TRACE, ("BTCoexistenceOn=%d\n", pAd->bMiscOn));
#endif /* RELEASE_EXCLUDE */
	}
	if (RTMPGetKeyParameter("BTConfiguration", tmpbuf, 10, pBuffer, TRUE))
	{
		ULONG Config = simple_strtol(tmpbuf, 0, 10);
		pAd->ulConfiguration = Config;
	
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_TRACE, ("BTConfiguration=0x%04lx\n", pAd->ulConfiguration));
#endif /* RELEASE_EXCLUDE */
	}
}

/*
VOID BtCoexistInit()
*/
#endif /* RELEASE_EXCLUDE */
VOID MiscInit(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RT3290
	// TODO: shiang, remove this!!
	if (IS_RT3290(pAd))
		return;
#endif /* RT3290 */

#ifdef RTMP_PCI_SUPPORT
#ifdef RELEASE_EXCLUDE
	/*
		Patch RT3090BC4V22 single crystal case: PCIE Power Save Leve must be 2, it 
		acts as power saving level 3 except turning off RF LDO.XTAL would be on in this mode
		for BT clock used.
	*/
#endif /* RELEASE_EXCLUDE */
	if (((pAd->NicConfig3.field.CrystalShared == CYRSTALL_SHARED) || 
		IS_ENABLE_SINGLE_CRYSTAL_SHARING_BY_FORCE(pAd)) &&
		(pAd->StaCfg.PSControl.field.rt30xxPowerMode == 0x3))
	{	
		UCHAR  LinkCtrlSetting = 0;
		USHORT PCIePowerSetting = 0;
		
#ifdef RELEASE_EXCLUDE
		/*
			0: To turn off MAC RX/TX, MPLL, RF block
			2: To turn off MAC RX/TX, MPLL, RF block, PCIe Clock Off Enable
			3: To turn off MAC RX/TX, MPLL, RF block, PCIe Clock Off Enable, RF Power
		*/
#endif /* RELEASE_EXCLUDE */
		PCIePowerSetting = 2;
		DBGPRINT(RT_DEBUG_TRACE, ("before PCIePower Save Level = %d due to single crystall\n", pAd->StaCfg.PSControl.field.rt30xxPowerMode));
		DBGPRINT(RT_DEBUG_TRACE, ("after PCIePower Save Level = 2 due to single crystall\n"));
		
		pAd->PCIePowerSaveLevel = (USHORT)PCIePowerSetting;
#ifdef RELEASE_EXCLUDE
		/*
			for 3090F , we need to add high-byte arg for 0x83 command to indicate the link control setting in 
			PCI Configuration Space. Because firmware can't read PCI Configuration Space
		*/
#endif /* RELEASE_EXCLUDE */
		if ((pAd->Rt3xxRalinkLinkCtrl & 0x2) && (pAd->Rt3xxHostLinkCtrl & 0x2))
		{
			LinkCtrlSetting = 1;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("====> rt30xxF LinkCtrlSetting = 0x%x.\n", LinkCtrlSetting));
		AsicSendCommandToMcu(pAd, 0x83, 0xff, (UCHAR)PCIePowerSetting, LinkCtrlSetting, FALSE);
	}
#endif /* RTMP_PCI_SUPPORT */
#ifdef RTMP_USB_SUPPORT
	if ((pAd->NicConfig3.field.CoexMethod == TRUE) && (IS_ENABLE_THREE_WIRE_BY_FORCE(pAd)))
	{
#ifdef RELEASE_EXCLUDE
 		/* 3-Wired Method => 8070 */
#endif /* RELEASE_EXCLUDE */
		pAd->bCoexMethod = TRUE;
#ifdef RELEASE_EXCLUDE
		/* Disable HW CTRL, since the GPIO#2 will be used for BT_Active and not for Radio on/off */
#endif /* RELEASE_EXCLUDE */
		pAd->StaCfg.bHardwareRadio = FALSE;
	}
	else
	{
#ifdef RELEASE_EXCLUDE
		/* 2-Wired Method => 3070, 3870 */
#endif /* RELEASE_EXCLUDE */
		pAd->bCoexMethod = FALSE;
	}
#endif /* RTMP_USB_SUPPORT */
	if (IS_ENABLE_MISC_TIMER(pAd))
	{
#ifdef RELEASE_EXCLUDE
		/* Read Bluetooth priority or active counter every DETECT_TIMEOUT ms */
#endif /* RELEASE_EXCLUDE */
		RTMPInitTimer(pAd, &pAd->Mlme.MiscDetectTimer, GET_TIMER_FUNCTION(DetectExec), pAd, TRUE);
		RTMPSetTimer(&pAd->Mlme.MiscDetectTimer, DETECT_TIMEOUT);
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_TRACE,("Set BT Coexistence detect timer\n"));
#endif /* RELEASE_EXCLUDE */
	}

	DBGPRINT(RT_DEBUG_TRACE,("nicConfig2 =0x%04x\n", pAd->NicConfig2.word));
	DBGPRINT(RT_DEBUG_TRACE,("nicConfig3 =0x%04x\n", pAd->NicConfig3.word));
#ifdef RELEASE_EXCLUDE
#ifdef RTMP_USB_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE,("BT Coexistence Method = %d(0=>2-wire, 1=>3-wire)\n", pAd->bCoexMethod));
#endif /* RTMP_USB_SUPPORT */
	DBGPRINT(RT_DEBUG_TRACE,("BT Configuration = %08lx\n", pAd->ulConfiguration));
#endif /* RELEASE_EXCLUDE */
}

#ifdef RELEASE_EXCLUDE
/*
VOID BtCoexistUserCfgInit()
*/
#endif /* RELEASE_EXCLUDE */
VOID MiscUserCfgInit(
	IN PRTMP_ADAPTER pAd)
{
	pAd->bMiscOn = TRUE;
#ifdef RTMP_USB_SUPPORT
	pAd->bCoexMethod = FALSE;
#endif /* RTMP_USB_SUPPORT */
#ifdef DOT11_N_SUPPORT
	pAd->bPermitRecBaDown = FALSE;
	pAd->bPermitMcsDown = FALSE;
	pAd->bPermitTxBaSizeDown = FALSE;
	pAd->bPermitTxBaDensityDown = FALSE;
#endif /* DOT11_N_SUPPORT */
	pAd->bPermitTxPowerDown = FALSE;
	pAd->bPermitLnaGainDown = FALSE;
#ifdef RELEASE_EXCLUDE
	/* 
		USB Default: 0x380004FD = 0x01 + 0x04 + 0x08 + 0x10 + 0x20 + 0x40 + 0x80 + 0x0400
		PCI Default: 0x380004DD = 0x01 + 0x04 + 0x08 + 0x10 + 0x40 + 0x80 + 0x0400

		0xC0000000: GET_PARAMETER_OF_AMPDU_SIZE
		0x30000000: GET_PARAMETER_OF_AMPDU_DENSITY
		0x0C000000: GET_PARAMETER_OF_MCS_THRESHOLD
		0x03000000: GET_PARAMETER_OF_TXRX_THR_THRESHOLD

		0x01: Enable BT timer
		0x02: 40/20 BSS Coexistence @HT40 and BUSY_1
		0x04: BA Window Size		@HT40 and BUSY_2
		0x08: BA Density		@HT40 and BUSY_2
		0x10: MCS Rate			@HT40 and BUSY_2
		0x20: REC BA Request		@HT40 and BUSY_2
		0x40: TxPower			@HT40/HT20 and BUSY_1
		0x80: LNA Middle Gain		@HT40/HT20 and BUSY_1
		0x0000: default is 2 wire, 0x0400 is 1 wire, 0x0800 is 3 wire for usb or force to single crystal for pci
	*/
#endif /* RELEASE_EXCLUDE */
#ifdef RTMP_USB_SUPPORT
	pAd->ulConfiguration = 0x380004FD;
#endif /* RTMP_USB_SUPPORT */
#ifdef RTMP_PCI_SUPPORT
	pAd->ulConfiguration = 0x380004DD;
#endif /* RTMP_PCI_SUPPORT */
	pAd->ulActiveCountPastPeriod = 0;
	pAd->BusyDegree = BUSY_0;
}

#ifdef RTMP_USB_SUPPORT
#ifdef RELEASE_EXCLUDE
/*
NTSTATUS BtCoexistDetectExecAtCmdThread()
*/
#endif /* RELEASE_EXCLUDE */
NTSTATUS DetectExecAtCmdThread(
	IN PRTMP_ADAPTER pAd, 
	IN PCmdQElmt CMDQelmt)
{
	BOOLEAN bIssue4020 = FALSE;
	UCHAR j = 0;
#ifdef DOT11N_DRAFT3
	UCHAR btWifiThr = 0;
#endif /* DOT11N_DRAFT3 */
	ULONG data = 0;
	ULONG data_switch=0;	
	static ULONG TimeCount = 0; /* per 0.01 second, count it. */
	static ULONG NoBusyTimeCount = 0; /* per second, count it. */
	static ULONG History[HISTORY_RECORD_NUM] = {0};
	static ULONG BtHardKeyOffTimeCount = 0;
	BUSY_DEGREE BusyDegree = BUSY_0;


	TimeCount++;
		
	if (BtHardKeyOffTimeCount == 0)
	{
#ifdef RELEASE_EXCLUDE
		/* 2-wire */
#endif /* RELEASE_EXCLUDE */
		if(pAd->bCoexMethod == TRUE)
		{
#ifdef RELEASE_EXCLUDE
			/*8070=>3 wire */
#endif /* RELEASE_EXCLUDE */
			AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x01, FALSE);
		}
		else
		{
#ifdef RELEASE_EXCLUDE
			/*3070,3870 =>2wire */
#endif /* RELEASE_EXCLUDE */
			AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x00, FALSE); 
		}
	}
	else
	{
#ifdef RELEASE_EXCLUDE
		/*
			Hardkey push down => BT Power Off!!
			Count BT Active,but no disable Wifi Tx
		*/
#endif /* RELEASE_EXCLUDE */
		AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x02, FALSE); 	
	}

	RTMP_IO_READ32(pAd, ACT_CNT, &data);
	data_switch &= 0;
	data_switch |= ((data & 0x000000FF) << 8);
	data_switch |= ((data & 0x0000FF00) >> 8);
	pAd->ulActiveCountPastPeriod += data_switch;

	DBGPRINT(RT_DEBUG_INFO, (">>>>> data = 0x%08lx, data_switch=0x%08lx", (data & 0x0000FFFF), data_switch));

	/* About 1 second to check busy from Buletooth history active counter */
	if (TimeCount >= CHECK_TIME_INTERVAL)
	{
		DBGPRINT(RT_DEBUG_INFO,("<--- WATCH TIME"));	
		DBGPRINT(RT_DEBUG_INFO,("BW=%d, bt active per sec=%ld, No Busy Time Count =%ld",
				pAd->CommonCfg.BBPCurrentBW,
				pAd->ulActiveCountPastPeriod, NoBusyTimeCount));

#ifdef RELEASE_EXCLUDE
		/* Hardkey push down => BT Power Off!! */
#endif /* RELEASE_EXCLUDE */
		if (pAd->ulActiveCountPastPeriod >= 250000)
		{	
			if (BtHardKeyOffTimeCount == 0)
			{
#ifdef RELEASE_EXCLUDE
				/* Count BT priority,but no disable Wifi Tx */
#endif /* RELEASE_EXCLUDE */
				DBGPRINT(RT_DEBUG_INFO,("Hardware key OFF!!!!"));

				AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x02, FALSE);	
			}
			BtHardKeyOffTimeCount++;
			pAd->ulActiveCountPastPeriod = 0;
			DBGPRINT(RT_DEBUG_INFO,("ulActiveCountPastPeriod >= 250000, force ulActiveCountPastPeriod = 0"));
		}
		else
		{
			if (BtHardKeyOffTimeCount > 0)
			{
				BtHardKeyOffTimeCount = 0;
				DBGPRINT(RT_DEBUG_INFO,("Hardware key ON!!!!"));

				if(pAd->bCoexMethod == TRUE)
				{
					/*8070=>3 wire */
					AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x01, FALSE);
				}
				else
				{
					/*3070,3870 =>2wire */
					AsicSendCommandToMcu(pAd, MCU_SET_ACTIVE_POLLING, 0xff , 0x01, 0x00, FALSE); 
				}
			}	
		}

		for(j=HISTORY_RECORD_NUM-1 ; j>=1 ; j--)
			History[j]=History[j-1];

		History[0]=pAd->ulActiveCountPastPeriod;
		BusyDegree = CheckBusy(&History[0],HISTORY_RECORD_NUM);
		pAd->BusyDegree = BusyDegree;

		if (pAd->BusyDegree == BUSY_0)
			NoBusyTimeCount++;
		else
			NoBusyTimeCount = 0;
		
#ifdef DOT11N_DRAFT3
		btWifiThr = (UCHAR)(GET_PARAMETER_OF_TXRX_THR_THRESHOLD(pAd));
		/* 0,1,2,3 => 0, 6, 12, 18 Mbps */
		if ((btWifiThr <= 3)&& IS_ENABLE_40TO20_BY_TIMER(pAd))
			bIssue4020 = WifiThroughputOverLimit(pAd,(btWifiThr*6));
		else 
#endif /* DOT11N_DRAFT3 */
			bIssue4020 = FALSE;
		
		pAd->ulActiveCountPastPeriod=0;
		TimeCount = 0;
	}

	if (TimeCount == 0)
		Adjust(pAd, bIssue4020, NoBusyTimeCount);
	
	return NDIS_STATUS_SUCCESS;
}

#ifdef RELEASE_EXCLUDE
/*
	VOID BtCoexistDetectExec()
*/
#endif /* RELEASE_EXCLUDE */
VOID DetectExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	PRTMP_ADAPTER	pAd = (RTMP_ADAPTER *)FunctionContext;
		
	if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS))||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
				DBGPRINT(RT_DEBUG_INFO,("error RESET, HALT, RADIO_OFF, NIC_NOT_EXIST"));
				return;
	}

	if ((pAd->bHWCoexistenceInit == TRUE) && 
		(!IDLE_ON(pAd))&& 
		(pAd->OpMode == OPMODE_STA))
	{
		RTEnqueueInternalCmd(pAd,RT_CMD_COEXISTENCE_DETECTION, NULL, 0);
	}
	else
	{
#ifdef DOT11_N_SUPPORT
		pAd->bPermitRecBaDown = FALSE;
		pAd->bPermitMcsDown = FALSE;
		pAd->bPermitTxBaSizeDown = FALSE;
		pAd->bPermitTxBaDensityDown = FALSE;
#endif /* DOT11_N_SUPPORT */
		pAd->bPermitTxPowerDown = FALSE;
		pAd->bPermitLnaGainDown = FALSE;
	}	
}
#endif /* RTMP_USB_SUPPORT */

#ifdef RTMP_PCI_SUPPORT
#ifdef RELEASE_EXCLUDE
/*
	VOID BtCoexistDetectExec()
*/
#endif /* RELEASE_EXCLUDE */
VOID DetectExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	BOOLEAN 				bIssue4020 = FALSE;
	BOOLEAN 				bPowerSaving = FALSE;
	ULONG					data = 0;
	UCHAR					j = 0;
#ifdef DOT11N_DRAFT3
	UCHAR					btWifiThr = 0;
#endif /* DOT11N_DRAFT3 */
	BUSY_DEGREE	BusyDegree = BUSY_0;
	static UCHAR			ulPowerSavingTimeCount = 0; /* per 0.01 second, count it.
*/
	static ULONG			TimeCount = 0; /* per 0.01 second, count it.
*/
	static ULONG			NoBusyTimeCount = 0; /* per second, count it.
*/
	static ULONG			VeryBusyTimeCount = 0;		/* per second, count it.
*/
	static ULONG			History[HISTORY_RECORD_NUM] = {0};
	PRTMP_ADAPTER			pAd = NULL;
#ifdef RT5390
	BOOLEAN		bInitialValue = FALSE;
	UCHAR BbpR65 = 0;
	ULONG MACValue = 0;
#endif /* RT5390 */

	pAd = (RTMP_ADAPTER *)FunctionContext;
	
	if (pAd == NULL)
	{
		return;
	}
	
#ifdef RT5390	
do
#else
	if ((pAd->bHWCoexistenceInit == TRUE) && 
		(!IDLE_ON(pAd))&& 
		(pAd->OpMode == OPMODE_STA))
#endif /* RT5390 */
	{
#ifdef RT5390 
		if ((pAd->NicConfig2.field.CoexBit==TRUE) && (!IDLE_ON(pAd))&& (pAd->OpMode == OPMODE_STA) && IS_ENABLE_MISC_TIMER(pAd))
		{
			bInitialValue = FALSE;
		}
		else
	{
			bInitialValue = TRUE;
			break;
		}

		if (IS_RT3592BC8(pAd) && ((pAd->CommonCfg.Channel > 14) 
#ifdef KHNote
			|| (!IS_BT_ENABLE(pAd))
#endif /* KHNote */
			))
		{
			bInitialValue = TRUE;
			break;
		}
#endif /* RT5390 */

		TimeCount++;

		if ((pAd->bPCIclkOff == TRUE) ||
			RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND) || 
			RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF) ||
			OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
		{
			if (pAd->bPCIclkOff == TRUE)
			{
				DBGPRINT(RT_DEBUG_INFO,("pAd->bPCIclkOff == TRUE\n"));
			}
			if (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND))
			{
				DBGPRINT(RT_DEBUG_INFO,("fRTMP_PS_SET_PCI_CLK_OFF_COMMAND == TRUE\n"));
			}
			if (RTMP_TEST_PSFLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
			{
				DBGPRINT(RT_DEBUG_INFO,("fRTMP_ADAPTER_IDLE_RADIO_OFF == TRUE\n"));
			}
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
			{
				DBGPRINT(RT_DEBUG_INFO,("fOP_STATUS_DOZE == TRUE\n"));
			}
			bPowerSaving = TRUE;
			
			ulPowerSavingTimeCount ++;
		}

		if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS))	||
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))	||
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))			||
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			DBGPRINT(RT_DEBUG_INFO,("error RESET, HALT, RADIO_OFF, NIC_NOT_EXIST\n"));
			if( TimeCount >= CHECK_TIME_INTERVAL) 
			{
				pAd->ulActiveCountPastPeriod=0;
				TimeCount = 0;
				NoBusyTimeCount = 0;

#ifdef DOT11_N_SUPPORT
				pAd->bPermitRecBaDown = FALSE;
				pAd->bPermitMcsDown = FALSE;
				pAd->bPermitTxBaSizeDown = FALSE;
				pAd->bPermitTxBaDensityDown = FALSE;
#endif /* DOT11_N_SUPPORT */
				pAd->bPermitTxPowerDown = FALSE;
				pAd->bPermitLnaGainDown = FALSE;
			}
			return;
		}
		
		if (bPowerSaving == FALSE)
		{
			RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);
	
			if (data & 0x0010)
			{
				pAd->ulActiveCountPastPeriod++;
			}
		}

#ifdef RELEASE_EXCLUDE
		/* check if BT is busy per 1 second, timer interval is 10ms */
#endif /* RELEASE_EXCLUDE */
		if (TimeCount >= CHECK_TIME_INTERVAL)
		{

			DBGPRINT(RT_DEBUG_INFO,("<--- WATCH TIME\n"));	
			DBGPRINT(RT_DEBUG_INFO,("-->BW=%d, bt active per sec=%ld, No Busy Time Count =%ld, Very Busy Time Count = %ld, PowerSavingTimeCount =%d\n", 
					pAd->CommonCfg.BBPCurrentBW,
					pAd->ulActiveCountPastPeriod,
					NoBusyTimeCount, 
					VeryBusyTimeCount,
					ulPowerSavingTimeCount
					));

			for (j = HISTORY_RECORD_NUM-1; j >= 1; j--)
			{
				History[j]=History[j-1];
			}

			if (ulPowerSavingTimeCount == 0)
			{
				History[0] = pAd->ulActiveCountPastPeriod;
			}
			else if (ulPowerSavingTimeCount < CHECK_TIME_INTERVAL)
			{
				History[0] = (pAd->ulActiveCountPastPeriod * CHECK_TIME_INTERVAL )/(CHECK_TIME_INTERVAL - ulPowerSavingTimeCount);
			}
			else 
			{
				History[0] = 0;
			}
	
			BusyDegree = CheckBusy(&History[0],HISTORY_RECORD_NUM);
			pAd->BusyDegree = BusyDegree;
			
			if (pAd->BusyDegree == BUSY_0)
			{	
				NoBusyTimeCount++;
			}
			else
			{
				NoBusyTimeCount = 0;
			}
		
			if (pAd->BusyDegree >= BUSY_4)
			{	
				VeryBusyTimeCount++;
			}
			else
			{
				VeryBusyTimeCount = 0;
			}
			
#ifdef DOT11N_DRAFT3
			btWifiThr = (UCHAR)(GET_PARAMETER_OF_TXRX_THR_THRESHOLD(pAd));

			/*0,1,2,3 => 0, 6, 12, 18
*/
			if ((btWifiThr <= 3) && IS_ENABLE_40TO20_BY_TIMER(pAd))
			{
				bIssue4020 = (WifiThroughputOverLimit(pAd,(btWifiThr*6)) == TRUE) & (VeryBusyTimeCount > 15);
			}
			else 
#endif /* DOT11N_DRAFT3 */
			bIssue4020 = FALSE;

			DBGPRINT(RT_DEBUG_INFO, ("-->VeryBusyTimeCount = %ld, bIssue4020 = %d\n", VeryBusyTimeCount, bIssue4020));
			
			pAd->ulActiveCountPastPeriod = 0;
			TimeCount = 0;
			ulPowerSavingTimeCount = 0;
		}
		
		if (TimeCount == 0)
		{
			Adjust(pAd, bIssue4020, NoBusyTimeCount);
		}	
#ifdef RT5390
}while(FALSE);
#else
	}
#endif /* RT5390 */
	else
	{
		pAd->ulActiveCountPastPeriod = 0;
		TimeCount = 0;
		NoBusyTimeCount = 0;
		VeryBusyTimeCount = 0;

#ifdef DOT11_N_SUPPORT
		pAd->bPermitRecBaDown = FALSE;
		pAd->bPermitMcsDown = FALSE;
		pAd->bPermitTxBaSizeDown = FALSE;
		pAd->bPermitTxBaDensityDown = FALSE;
#endif /* DOT11_N_SUPPORT */
		pAd->bPermitTxPowerDown = FALSE;
		pAd->bPermitLnaGainDown = FALSE;
	}
}

#ifdef RT5390

VOID RTMPBtWiFiSwitch(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			BTWiFiSwitchMode)
{
	ULONG			MacValue;
	UCHAR			BTEnable = 0;
	UCHAR			BTDisable = 0;

	RTMP_IO_READ32(pAd, LED_CFG, &MacValue);
	if (MacValue & 0x40000000)
	{
		BTEnable = 0x0c;
		BTDisable = 0x03;
	}
	else
	{
		BTEnable = 0x03;
		BTDisable = 0x0c;
	}

	/*
	   Only firmware mode will update LED_CFG (0x102c).
	   If it is not firmware mode, we have to update LED_CFG.
	*/
	if ((pAd->LedCntl.field.LedMode == 0) || (pAd->LedCntl.field.LedMode > LED_MODE_SIGNAL_STREGTH))
	{
		MacValue &= 0xf0ffffff;
		if (BTWiFiSwitchMode == BT_SWTICH_ENABLE)
		{
			MacValue |= (BTEnable << 24);
		}
		else
		{
			MacValue |= (BTDisable << 24);
		}
		RTMP_IO_WRITE32(pAd, LED_CFG, MacValue);
	}
	else
	{
		
		/* firmware control mode, tell firmware to set bit[27:24]. */
	
		if (BTWiFiSwitchMode == BT_SWTICH_ENABLE)
		{
			AsicSendCommandToMcu(pAd, FALSE, 0x91, 0xff, BTEnable, 1, FALSE);
		}
		else 
		{
			AsicSendCommandToMcu(pAd, FALSE, 0x91, 0xff, BTDisable, 1, FALSE);
		}
	}
}


VOID RTMPBtWiFiDiversity(RTMP_ADAPTER *pAd, UCHAR BTMode)
{
	ULONG MACValue, x;
	
	if (BT_WIFI_DIRECT)
	{ /* direct */

		RTMP_IO_READ32(pAd, E2PROM_CSR, &x);
		x &= ~(EESK);
		RTMP_IO_WRITE32(pAd, E2PROM_CSR, x);
		RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &MACValue);
		MACValue &= ~(0x0808);
		MACValue |= 0x08;		
		RTMP_IO_WRITE32(pAd, GPIO_CTRL_CFG, MACValue);
	}
	else
	{
		RTMP_IO_READ32(pAd, E2PROM_CSR, &x);
		x |= (EESK);
		RTMP_IO_WRITE32(pAd, E2PROM_CSR, x);
		RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &MACValue);
		MACValue &= ~(0x0808);		
		RTMP_IO_WRITE32(pAd, GPIO_CTRL_CFG, MACValue);
	}
}
#endif /* RT5390 */
#endif /* RTMP_PCI_SUPPORT */

#endif /* BT_COEXISTENCE_SUPPORT */

