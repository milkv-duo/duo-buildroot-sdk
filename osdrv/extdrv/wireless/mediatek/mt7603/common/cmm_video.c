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
	cmm_video.c

    Abstract:
    Ralink WiFi Driver video mode related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------

*/

#include "rt_config.h"


#ifdef VIDEO_TURBINE_SUPPORT

#if 0
UCHAR VideoRateSwitchTable11N1S[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0c, 0x0a,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x00,  0, 40, 101,
    0x01, 0x00,  1, 40, 50,
    0x02, 0x00,  2, 25, 45,
    0x03, 0x21,  0, 20, 35,
    0x04, 0x21,  1, 20, 35,
    0x05, 0x21,  2, 20, 35,
    0x06, 0x21,  3, 15, 35,
    0x07, 0x21,  4, 15, 30,
    0x08, 0x21,  5, 10, 25,
    0x09, 0x21,  6,  8, 14,
    0x0a, 0x21,  7,  8, 14,
    0x0b, 0x23,  7,  8, 14,
};


UCHAR VideoRateSwitchTable11N2S[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0e, 0x0c,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x00,  0, 40, 101,
    0x01, 0x00,  1, 40, 50,
    0x02, 0x00,  2, 25, 45,
    0x03, 0x21,  0, 20, 35,
    0x04, 0x21,  1, 20, 35,
    0x05, 0x21,  2, 20, 35,
    0x06, 0x21,  3, 15, 35,
    0x07, 0x21,  4, 15, 30,
    0x08, 0x20, 11, 15, 30,
    0x09, 0x20, 12, 15, 30,
    0x0a, 0x20, 13,  8, 20,
    0x0b, 0x20, 14,  8, 20,
    0x0c, 0x20, 15,  8, 25,
    0x0d, 0x22, 15,  8, 15,
};


UCHAR VideoRateSwitchTable11N3SForABand[] = {
// Item No.   Mode   Curr-MCS   TrainUp   TrainDown		// Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)
    0x0e, 0x09,  0,  0,  0,						// Initial used item after association
    0x00, 0x21,  0, 30, 101,
    0x01, 0x21,  1, 20, 50,
    0x02, 0x21,  2, 20, 50,
    0x03, 0x21,  3, 15, 50,
    0x04, 0x21,  4, 15, 30,
    0x05, 0x21,  5, 15, 30,
    0x06, 0x20, 12,  15, 30,
    0x07, 0x20, 13,  8, 20,
    0x08, 0x20, 14,  8, 20,
    0x09, 0x20, 15,  8, 25,
    //0x0a, 0x22, 15,  8, 25,
    //0x0a, 0x20, 20, 15, 30,
    0x0a, 0x20, 21,  8, 20,
    0x0b, 0x20, 22,  8, 20,
    0x0c, 0x20, 23,  8, 25,
    0x0d, 0x22, 23,  8, 25,
};


UCHAR VideoRateSwitchTable11BGN1S[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0c, 0x0a,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x00,  0, 40, 101,
    0x01, 0x00,  1, 40, 50,
    0x02, 0x00,  2, 25, 45,
    0x03, 0x21,  0, 20, 35,
    0x04, 0x21,  1, 20, 35,
    0x05, 0x21,  2, 20, 35,
    0x06, 0x21,  3, 15, 35,
    0x07, 0x21,  4, 15, 30,
    0x08, 0x21,  5, 10, 25,
    0x09, 0x21,  6,  8, 14,
    0x0a, 0x21,  7,  8, 14,
    0x0b, 0x23,  7,  8, 14,
};


UCHAR VideoRateSwitchTable11BGN2S[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0e, 0x0c,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x00,  0, 40, 101,
    0x01, 0x00,  1, 40, 50,
    0x02, 0x00,  2, 25, 45,
    0x03, 0x21,  0, 20, 35,
    0x04, 0x21,  1, 20, 35,
    0x05, 0x21,  2, 20, 35,
    0x06, 0x21,  3, 15, 35,
    0x07, 0x21,  4, 15, 30,
    0x08, 0x20, 11, 13, 25,
    0x09, 0x20, 12, 10, 22,
    0x0a, 0x20, 13,  8, 18,
    0x0b, 0x20, 14,  8, 18,
    0x0c, 0x20, 15,  8, 15,
    0x0d, 0x22, 15,  8, 15,
};


UCHAR VideoRateSwitchTable11BGN3S[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0e, 0x00,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x21,  0, 30,101,	/*50*/
    0x01, 0x21,  1, 20, 50,
    0x02, 0x21,  2, 20, 50,
    0x03, 0x21,  3, 20, 50,
    0x04, 0x21,  4, 15, 50,
    0x05, 0x20, 11, 15, 30,
    0x06, 0x20, 12, 15, 30,
    0x07, 0x20, 13,  8, 20,
    0x08, 0x20, 14,  8, 20,
    0x09, 0x20, 15,  8, 25,
    /*0x0a, 0x20, 20, 15, 30,*/
    0x0a, 0x20, 21,  8, 20,
    0x0b, 0x20, 22,  8, 20,
    0x0c, 0x20, 23,  8, 25,
    0x0d, 0x22, 23,  8, 25,
};


UCHAR VideoRateSwitchTable11BGN2SForABand[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0b, 0x09,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x21,  0, 30,101,	/*50*/
    0x01, 0x21,  1, 20, 50,
    0x02, 0x21,  2, 20, 50,
    0x03, 0x21,  3, 15, 50,
    0x04, 0x21,  4, 15, 30,
    0x05, 0x21,  5, 15, 30,
    0x06, 0x20, 12, 15, 30,
    0x07, 0x20, 13,  8, 20,
    0x08, 0x20, 14,  8, 20,
    0x09, 0x20, 15,  8, 25,
    0x0a, 0x22, 15,  8, 25,
};


UCHAR VideoRateSwitchTable11BGN3SForABand[] = {
/* Item No.   Mode   Curr-MCS   TrainUp   TrainDown		 Mode- Bit0: STBC, Bit1: Short GI, Bit4,5: Mode(0:CCK, 1:OFDM, 2:HT Mix, 3:HT GF)*/
    0x0e, 0x09,  0,  0,  0,						/* Initial used item after association*/
    0x00, 0x21,  0, 30,101,	/*50*/
    0x01, 0x21,  1, 20, 50,
    0x02, 0x21,  2, 20, 50,
    0x03, 0x21,  3, 15, 50,
    0x04, 0x21,  4, 15, 30,
    0x05, 0x21,  5, 15, 30,
    0x06, 0x20, 12, 15, 30,
    0x07, 0x20, 13,  8, 20,
    0x08, 0x20, 14,  8, 20,
    0x09, 0x20, 15,  8, 25,
    /*0x0a, 0x22, 15,  8, 25,*/
    /*0x0a, 0x20, 20, 15, 30,*/
    0x0a, 0x20, 21,  8, 20,
    0x0b, 0x20, 22,  8, 20,
    0x0c, 0x20, 23,  8, 25,
    0x0d, 0x22, 23,  8, 25,
};
#endif


BOOLEAN UpdateFromGlobal = FALSE;

void VideoTurbineUpdate(
	IN PRTMP_ADAPTER pAd)
{
	if (UpdateFromGlobal == TRUE) 
	{
		pAd->VideoTurbine.Enable = GLOBAL_AP_VIDEO_CONFIG.Enable;
		pAd->VideoTurbine.ClassifierEnable = GLOBAL_AP_VIDEO_CONFIG.ClassifierEnable;
		pAd->VideoTurbine.HighTxMode = GLOBAL_AP_VIDEO_CONFIG.HighTxMode;
		pAd->VideoTurbine.TxPwr = GLOBAL_AP_VIDEO_CONFIG.TxPwr;
		pAd->VideoTurbine.VideoMCSEnable = GLOBAL_AP_VIDEO_CONFIG.VideoMCSEnable;
		pAd->VideoTurbine.VideoMCS = GLOBAL_AP_VIDEO_CONFIG.VideoMCS;
		pAd->VideoTurbine.TxBASize = GLOBAL_AP_VIDEO_CONFIG.TxBASize;
		pAd->VideoTurbine.TxLifeTimeMode = GLOBAL_AP_VIDEO_CONFIG.TxLifeTimeMode;
		pAd->VideoTurbine.TxLifeTime = GLOBAL_AP_VIDEO_CONFIG.TxLifeTime;
		pAd->VideoTurbine.TxRetryLimit = GLOBAL_AP_VIDEO_CONFIG.TxRetryLimit;
	}
}


VOID TxSwQDepthAdjust(IN RTMP_ADAPTER *pAd, IN UINT32 qLen)
{
	ULONG IrqFlags;
	INT qIdx;
	QUEUE_HEADER *pTxQ, *pEntry;
	PNDIS_PACKET pPacket;
	
	RTMP_IRQ_LOCK(&pAd->irq_lock, IrqFlags);
	pAd->TxSwQMaxLen = qLen;
	for (qIdx = 0; qIdx < NUM_OF_TX_RING; qIdx++)
	{
		pTxQ = &pAd->TxSwQueue[qIdx];
		while(pTxQ->Number >= pAd->TxSwQMaxLen)
		{
			pEntry = RemoveHeadQueue(pTxQ);
			if (pEntry)
			{
				pPacket = QUEUE_ENTRY_TO_PACKET(pEntry);
				RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
			}
			else
				break;
		}
	}
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, IrqFlags);
	
	DBGPRINT(RT_DEBUG_OFF, ("%s():Set TxSwQMaxLen as %d\n", 
			__FUNCTION__, pAd->TxSwQMaxLen));
}


VOID VideoTurbineDynamicTune(
	IN PRTMP_ADAPTER pAd)
{
	if (pAd->VideoTurbine.Enable == TRUE) 
	{
			UINT32 MacReg = 0;

#ifdef RT3883
		if (IS_RT3883(pAd))
		{
			RTMP_IO_READ32(pAd, TX_AC_RTY_LIMIT, &MacReg);
			MacReg = 0x0f1f0f0f;
			RTMP_IO_WRITE32(pAd, TX_AC_RTY_LIMIT, MacReg);

			RTMP_IO_READ32(pAd, TX_AC_FBK_SPEED, &MacReg);
			MacReg = 0x06000003;
			RTMP_IO_WRITE32(pAd, TX_AC_FBK_SPEED, MacReg);
		}
		else
#endif /* RT3883 */
		{
			/* Tx retry limit = 2F,1F */
			RTMP_IO_READ32(pAd, TX_RTY_CFG, &MacReg);
			MacReg &= 0xFFFF0000;
			MacReg |= GetAsicVideoRetry(pAd);
			RTMP_IO_WRITE32(pAd, TX_RTY_CFG, MacReg);
		}

		pAd->VideoTurbine.TxBASize = GetAsicVideoTxBA(pAd);

		Set_RateAdaptInterval(pAd, "100:50");
		TxSwQDepthAdjust(pAd, 1024);
			
	}
	else 
	{
			UINT32 MacReg = 0;

#ifdef RT3883
		if (IS_RT3883(pAd))
		{
			RTMP_IO_READ32(pAd, TX_AC_RTY_LIMIT, &MacReg);
			MacReg = 0x07070707;
			RTMP_IO_WRITE32(pAd, TX_AC_RTY_LIMIT, MacReg);
	
			RTMP_IO_READ32(pAd, TX_AC_FBK_SPEED, &MacReg);
			MacReg = 0x0;
			RTMP_IO_WRITE32(pAd, TX_AC_FBK_SPEED, MacReg);
		}
#endif /* RT3883 */

		/* Default Tx retry limit = 1F,0F */
		RTMP_IO_READ32(pAd, TX_RTY_CFG, &MacReg);
		MacReg &= 0xFFFF0000;
			MacReg |= GetAsicDefaultRetry(pAd);
		RTMP_IO_WRITE32(pAd, TX_RTY_CFG, MacReg);

		pAd->VideoTurbine.TxBASize = GetAsicDefaultTxBA(pAd);

		/* reset to default rate adaptation simping interval */
		if ((pAd->ra_interval != DEF_RA_TIME_INTRVAL) || 
			(pAd->ra_fast_interval != DEF_QUICK_RA_TIME_INTERVAL))
			Set_RateAdaptInterval(pAd, "500:100");

		TxSwQDepthAdjust(pAd, MAX_PACKETS_IN_QUEUE);
	}
}

UINT32 GetAsicDefaultRetry(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 RetryLimit;

	RetryLimit = 0x1F0F;

	return RetryLimit;
}

UCHAR GetAsicDefaultTxBA(
	IN PRTMP_ADAPTER pAd)
{
        return pAd->CommonCfg.TxBASize;
}

UINT32 GetAsicVideoRetry(
	IN PRTMP_ADAPTER pAd)
{
	return pAd->VideoTurbine.TxRetryLimit;
}

UCHAR GetAsicVideoTxBA(
	IN PRTMP_ADAPTER pAd)
{
	return pAd->VideoTurbine.TxBASize;
}

VOID VideoConfigInit(
	IN PRTMP_ADAPTER pAd)
{
	pAd->VideoTurbine.Enable = FALSE;
	pAd->VideoTurbine.TxRetryLimit = 0x2F1F;
	pAd->VideoTurbine.TxBASize = pAd->CommonCfg.TxBASize; 
}

#endif /* VIDEO_TURBINE_SUPPORT */


