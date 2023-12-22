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
	rt85592.c

	Abstract:
	Specific funcitons and configurations for RT85592

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
*/

#ifdef RT8592

#include	"rt_config.h"

#define RT85592_E1	0x85920001
#define RT85592_E2	0x85920002
#define RT85592_E3	0x85920003


static const RT8592_FREQ_ITEM RT85592_FreqPlan_Xtal40M[] =
{
	/* Channel, N, K, mod, R */
	{1, 80, 12, 30, 0},
	{2, 80, 17, 30, 0}, 
	{3, 80, 22, 30, 0},
	{4, 80, 27, 30, 0},
	{5, 81, 2, 30, 0},
	{6, 81, 7, 30, 0},
	{7, 81, 12, 30, 0},
	{8, 81, 17, 30, 0},
	{9, 81, 22, 30, 0},
	{10, 81, 27, 30, 0},
	{11, 82, 2, 30, 0},
	{12, 82, 7, 30, 0},
	{13, 82, 12, 30, 0},
	{14, 82, 24, 30, 0},

	{36, 86, 4, 12, 0},
	{38, 86, 6, 12, 0},
	{40, 86, 8, 12, 0},
	{42, 86, 10, 12, 0},
	{44, 87, 0, 12, 0},
	{46, 87, 2, 12, 0},
	{48, 87, 4, 12, 0},
	{50, 87, 6, 12, 0},
	{52, 87, 8, 12, 0},
	{54, 87, 10, 12, 0},
	{56, 88, 0, 12, 0},
	{58, 88, 2, 12, 0},
	{60, 88, 4, 12, 0},
	{62, 88, 6, 12, 0},
	{64, 88, 8, 12, 0},
	{100, 91, 8, 12, 0},
	{102, 91, 10, 12, 0},
	{104, 92, 0, 12, 0},
	{106, 92, 2, 12, 0},
	{108, 92, 4, 12, 0},
	{110, 92, 6, 12, 0},
	{112, 92, 8, 12, 0},
	{114, 92, 10, 12, 0},
	{116, 93, 0, 12, 0},
	{118, 93, 2, 12, 0},
	{120, 93, 4, 12, 0},
	{122, 93, 6, 12, 0},
	{124, 93, 8, 12, 0},
	{126, 93, 10, 12, 0},
	{128, 94, 0, 12, 0},
	{130, 94, 2, 12, 0},
	{132, 94, 4, 12, 0},
	{134, 94, 6, 12, 0},
	{136, 94, 8, 12, 0},
	{138, 94, 10, 12, 0},
	{140, 95, 0, 12, 0},
	{149, 95, 9, 12, 0},
	{151, 95, 11, 12, 0},
	{153, 96, 1, 12, 0},
	{155, 96, 3, 12, 0},
	{157, 96, 5, 12, 0},
	{159, 96, 7, 12, 0},
	{161, 96, 9, 12, 0},
	{165, 97, 1, 12, 0},
	{184, 82, 0, 12, 0},
	{188, 82, 4, 12, 0},
	{192, 82, 8, 12, 0},
	{196, 83, 0, 12, 0},
};
static UCHAR RT85592_FreqPlan_Sz = (sizeof(RT85592_FreqPlan_Xtal40M) / sizeof(RT8592_FREQ_ITEM));


//+++ temporary functions get from windows team

#define BB_GET_BYTE0(_BbReg) \
	((_BbReg) & 0x000000FF)

#define BB_SET_BYTE0(_BbReg, _BbValueByte) \
	((_BbReg) = (((_BbReg) & ~0x000000FF) | (_BbValueByte)))

#define BB_SET_BYTE1(_BbReg, _BbValueByte) \
	((_BbReg) = (((_BbReg) & ~0x0000FF00) | (_BbValueByte << 8)))

#define BB_SET_BYTE2(_BbReg, _BbValueByte) \
	((_BbReg) = (((_BbReg) & ~0x00FF0000) | (_BbValueByte << 16)))

#define BB_SET_BYTE3(_BbReg, _BbValueByte) \
	((_BbReg) = (((_BbReg) & ~0xFF000000) | (_BbValueByte << 24)))

#define BB_BITWISE_WRITE(_BbReg, _BitLocation, _BitValue) \
	(_BbReg = (((_BbReg) & ~(_BitLocation)) | (_BitValue)))
#define BB_BITMASK_READ(_BbReg, _BitLocation) \
	((_BbReg) & (_BitLocation))

#define  PCIE_PHY_TX_ATTENUATION_CTRL	0x05C8

#ifdef RT_BIG_ENDIAN
typedef union _TX_ATTENUATION_CTRL_STRUC
{
	struct
	{
		UINT32	Reserve1:20;
		UINT32	PCIE_PHY_TX_ATTEN_EN:1;
		UINT32	PCIE_PHY_TX_ATTEN_VALUE:3;
		UINT32	Reserve2:7;
		UINT32	RF_ISOLATION_ENABLE:1;
	} field;
	
	UINT32	word;
} TX_ATTENUATION_CTRL_STRUC, *PTX_ATTENUATION_CTRL_STRUC;
#else
typedef union _TX_ATTENUATION_CTRL_STRUC {
	struct
	{
		UINT32	RF_ISOLATION_ENABLE:1;
		UINT32	Reserve2:7;
		UINT32	PCIE_PHY_TX_ATTEN_VALUE:3;
		UINT32	PCIE_PHY_TX_ATTEN_EN:1;
		UINT32	Reserve1:20;		
	} field;
	
	UINT32	word;
} TX_ATTENUATION_CTRL_STRUC, *PTX_ATTENUATION_CTRL_STRUC;
#endif


//
// VHT BW80 delta power control (+4~-4dBm) for per-rate Tx power control
//
#define EEPROM_VHT_BW80_TX_POWER_DELTA	(0x11E)

//
// Read per-rate Tx power
//
VOID RT85592ReadTxPwrPerRate(
	IN PRTMP_ADAPTER pAd)
{
	UINT32 data, DataBw40ABand, DataBw80ABand, DataBw40GBand;
	USHORT i, value, value2;
	INT TxPwrBw40ABand, TxPwrBw80ABand, TxPwrBw40GBand;
	UCHAR t1,t2,t3,t4;
	BOOLEAN bMinusBw40ABand = FALSE, bMinusBw80ABand = FALSE,bMinusBw40GBand = FALSE;

    DBGPRINT(RT_DEBUG_TRACE, ("%s() -->\n", __FUNCTION__));
	
	//
	// Get power delta for BW40
	//
	TxPwrBw40ABand = 0;
	TxPwrBw40GBand = 0;
	RT28xx_EEPROM_READ16(pAd, EEPROM_TXPOWER_DELTA, value2);
	if ((value2 & 0xFF) != 0xFF)
	{
		if (value2 & 0x80)
			TxPwrBw40GBand = (value2 & 0xF);
	
		if (value2 & 0x40)
			bMinusBw40GBand = FALSE;
		else
			bMinusBw40GBand = TRUE;
	}
	
	if ((value2 & 0xFF00) != 0xFF00)
	{
		if (value2 & 0x8000)
			TxPwrBw40ABand = ((value2&0x0F00) >> 8);

		if (value2 & 0x4000)
			bMinusBw40ABand = FALSE;
		else
			bMinusBw40ABand = TRUE;
	}

	//
	// Get power delta for BW80
	//
	TxPwrBw80ABand = 0;
	RT28xx_EEPROM_READ16(pAd, EEPROM_VHT_BW80_TX_POWER_DELTA, value2);
	if ((value2 & 0xFF) != 0xFF)
	{
		if (value2 & 0x80)
			TxPwrBw80ABand = (value2 & 0xF);
	
		if (value2 & 0x40)
			bMinusBw80ABand = FALSE;
		else
			bMinusBw80ABand = TRUE;
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("%s: bMinusBw40ABand = %d, bMinusBw80ABand = %d, bMinusBw40GBand = %d\n", 
		__FUNCTION__, 
		bMinusBw40ABand, 
		bMinusBw80ABand, 
		bMinusBw40GBand));

	DBGPRINT(RT_DEBUG_TRACE, ("%s: TxPwrBw40ABand = %d, TxPwrBw80ABand = %d, TxPwrBw40GBand = %d\n", 
		__FUNCTION__, 
		TxPwrBw40ABand, 
		TxPwrBw80ABand, 
		TxPwrBw40GBand));

	for (i = 0; i < 5; i++)
	{
		RT28xx_EEPROM_READ16(pAd, (EEPROM_TXPOWER_BYRATE_20MHZ_2_4G + (i * 4)), value);
		data = value;
		
		if (bMinusBw40ABand == FALSE)
		{
			t1 = (value & 0xF) + (TxPwrBw40ABand);
			if (t1 > 0xF)
				t1 = 0xF;
			
			t2 = ((value & 0xF0) >> 4) + (TxPwrBw40ABand);
			if (t2 > 0xF)
				t2 = 0xF;
			
			t3 = ((value & 0xF00) >> 8) + (TxPwrBw40ABand);
			if (t3 > 0xF)
				t3 = 0xF;
			
			t4 = ((value & 0xF000) >> 12) + (TxPwrBw40ABand);
			if (t4 > 0xF)
				t4 = 0xF;
		}
		else
		{
			if ((value & 0xF) > TxPwrBw40ABand)
				t1 = (value & 0xF) - (TxPwrBw40ABand);
			else
				t1 = 0;
			
			if (((value & 0xF0) >> 4) > TxPwrBw40ABand)
				t2 = ((value & 0xF0) >> 4) - (TxPwrBw40ABand);
			else
				t2 = 0;
			
			if (((value & 0xF00) >> 8) > TxPwrBw40ABand)
				t3 = ((value & 0xF00) >> 8) - (TxPwrBw40ABand);
			else
				t3 = 0;
			
			if (((value & 0xF000) >> 12) > TxPwrBw40ABand)
				t4 = ((value & 0xF000) >> 12) - (TxPwrBw40ABand);
			else
				t4 = 0;
		}
		
		DataBw40ABand = t1 + (t2 << 4) + (t3 << 8) + (t4 << 12);

		if (bMinusBw80ABand == FALSE)
		{
			t1 = (value & 0xF) + (TxPwrBw80ABand);
			if (t1 > 0xF)
				t1 = 0xF;
			
			t2 = ((value & 0xF0) >> 4) + (TxPwrBw80ABand);
			if (t2 > 0xF)
				t2 = 0xF;
			
			t3 = ((value & 0xF00) >> 8) + (TxPwrBw80ABand);
			if (t3 > 0xF)
				t3 = 0xF;
			
			t4 = ((value & 0xF000) >> 12) + (TxPwrBw80ABand);
			if (t4 > 0xF)
				t4 = 0xF;
		}
		else
		{
			if ((value & 0xF) > TxPwrBw80ABand)
				t1 = (value & 0xF) - (TxPwrBw80ABand);
			else
				t1 = 0;
			
			if (((value & 0xF0) >> 4) > TxPwrBw80ABand)
				t2 = ((value & 0xF0) >> 4) - (TxPwrBw80ABand);
			else
				t2 = 0;
			
			if (((value & 0xF00) >> 8) > TxPwrBw80ABand)
				t3 = ((value & 0xF00) >> 8) - (TxPwrBw80ABand);
			else
				t3 = 0;
			
			if (((value & 0xF000) >> 12) > TxPwrBw80ABand)
				t4 = ((value & 0xF000) >> 12) - (TxPwrBw80ABand);
			else
				t4 = 0;
		}
		
		DataBw80ABand = t1 + (t2 << 4) + (t3 << 8) + (t4 << 12);
		
		if (bMinusBw40GBand == FALSE)
		{
			t1 = (value & 0xF) + (TxPwrBw40GBand);
			if (t1 > 0xF)
				t1 = 0xF;

			t2 = ((value & 0xF0) >> 4) + (TxPwrBw40GBand);
			if (t2 > 0xF)
				t2 = 0xF;

			t3 = ((value & 0xF00) >> 8) + (TxPwrBw40GBand);
			if (t3 > 0xF)
				t3 = 0xF;

			t4 = ((value & 0xF000) >> 12) + (TxPwrBw40GBand);
			if (t4 > 0xF)
				t4 = 0xF;
		}
		else
		{
			if ((value & 0xF) > TxPwrBw40GBand)
				t1 = (value & 0xF) - (TxPwrBw40GBand);
			else
				t1 = 0;
			
			if (((value & 0xF0) >> 4) > TxPwrBw40GBand)
				t2 = ((value & 0xF0) >> 4) - (TxPwrBw40GBand);
			else
				t2 = 0;

			if (((value & 0xF00) >> 8) > TxPwrBw40GBand)
				t3 = ((value & 0xF00) >> 8) - (TxPwrBw40GBand);
			else
				t3 = 0;

			if (((value & 0xF000) >> 12) > TxPwrBw40GBand)
				t4 = ((value & 0xF000) >> 12) - (TxPwrBw40GBand);
			else
				t4 = 0;
		}
		
		DataBw40GBand = t1 + (t2 << 4) + (t3 << 8) + (t4 << 12);
	
		RT28xx_EEPROM_READ16(pAd, (EEPROM_TXPOWER_BYRATE_20MHZ_2_4G + (i * 4) + 2), value);
		if (bMinusBw40ABand == FALSE)
		{
			t1 = (value & 0xF) + (TxPwrBw40ABand);
			if (t1 > 0xF)
				t1 = 0xF;

			t2 = ((value & 0xF0) >> 4) + (TxPwrBw40ABand);
			if (t2 > 0xF)
				t2 = 0xF;

			t3 = ((value & 0xF00) >> 8) + (TxPwrBw40ABand);
			if (t3 > 0xF)
				t3 = 0xF;

			t4 = ((value & 0xF000) >> 12) + (TxPwrBw40ABand);
			if (t4 > 0xF)
				t4 = 0xF;
		}
		else
		{
			if ((value & 0xF) > TxPwrBw40ABand)
				t1 = (value & 0xF) - (TxPwrBw40ABand);
			else
				t1 = 0;

			if (((value & 0xF0) >> 4) > TxPwrBw40ABand)
				t2 = ((value & 0xF0) >> 4) - (TxPwrBw40ABand);
			else
				t2 = 0;

			if (((value & 0xF00) >> 8) > TxPwrBw40ABand)
				t3 = ((value & 0xF00) >> 8) - (TxPwrBw40ABand);
			else
				t3 = 0;

			if (((value & 0xF000) >> 12) > TxPwrBw40ABand)
				t4 = ((value & 0xF000) >> 12) - (TxPwrBw40ABand);
			else
				t4 = 0;
		}
		
		DataBw40ABand |= ((t1 << 16) + (t2 << 20) + (t3 << 24) + (t4 << 28));

		if (bMinusBw80ABand == FALSE)
		{
			t1 = (value & 0xF) + (TxPwrBw80ABand);
			if (t1 > 0xF)
				t1 = 0xF;

			t2 = ((value & 0xF0) >> 4) + (TxPwrBw80ABand);
			if (t2 > 0xF)
				t2 = 0xF;

			t3 = ((value & 0xF00) >> 8) + (TxPwrBw80ABand);
			if (t3 > 0xF)
				t3 = 0xF;

			t4 = ((value & 0xF000) >> 12) + (TxPwrBw80ABand);
			if (t4 > 0xF)
				t4 = 0xF;
		}
		else
		{
			if ((value & 0xF) > TxPwrBw80ABand)
				t1 = (value & 0xF) - (TxPwrBw80ABand);
			else
				t1 = 0;

			if (((value & 0xF0) >> 4) > TxPwrBw80ABand)
				t2 = ((value & 0xF0) >> 4) - (TxPwrBw80ABand);
			else
				t2 = 0;

			if (((value & 0xF00) >> 8) > TxPwrBw80ABand)
				t3 = ((value & 0xF00) >> 8) - (TxPwrBw80ABand);
			else
				t3 = 0;

			if (((value & 0xF000) >> 12) > TxPwrBw80ABand)
				t4 = ((value & 0xF000) >> 12) - (TxPwrBw80ABand);
			else
				t4 = 0;
		}
		
		DataBw80ABand |= ((t1 << 16) + (t2 << 20) + (t3 << 24) + (t4 << 28));
		
		if (bMinusBw40GBand == FALSE)
		{
			t1 = (value & 0xF) + (TxPwrBw40GBand);
			if (t1 > 0xF)
				t1 = 0xF;

			t2 = ((value & 0xF0) >> 4) + (TxPwrBw40GBand);
			if (t2 > 0xF)
				t2 = 0xF;

			t3 = ((value & 0xF00) >> 8) + (TxPwrBw40GBand);
			if (t3 > 0xF)
				t3 = 0xF;

			t4 = ((value & 0xF000) >> 12) + (TxPwrBw40GBand);
			if (t4 > 0xF)
				t4 = 0xF;
		}
		else
		{
			if ((value & 0xF) > TxPwrBw40GBand)
				t1 = (value & 0xF) - (TxPwrBw40GBand);
			else
				t1 = 0;

			if (((value & 0xF0) >> 4) > TxPwrBw40GBand)
				t2 = ((value & 0xF0) >> 4) - (TxPwrBw40GBand);
			else
				t2 = 0;

			if (((value & 0xF00) >> 8) > TxPwrBw40GBand)
				t3 = ((value & 0xF00) >> 8) - (TxPwrBw40GBand);
			else
				t3 = 0;

			if (((value & 0xF000) >> 12) > TxPwrBw40GBand)
				t4 = ((value & 0xF000) >> 12) - (TxPwrBw40GBand);
			else
				t4 = 0;
		}
		
		DataBw40GBand |= ((t1 << 16) + (t2 << 20) + (t3 << 24) + (t4 << 28));
		
		data |= (value << 16);
	
		pAd->Tx20MPwrCfgABand[i] = data;
		pAd->Tx20MPwrCfgGBand[i] = data;
		
		pAd->Tx40MPwrCfgABand[i] = DataBw40ABand;
		pAd->Tx80MPwrCfgABand[i] = DataBw80ABand;
		pAd->Tx40MPwrCfgGBand[i] = DataBw40GBand;
	
		if (data != 0xFFFFFFFF)
		{
			RTMP_IO_WRITE32(pAd, (TX_PWR_CFG_0 + (i * 4)), data);
		}
		
		DBGPRINT_RAW(RT_DEBUG_TRACE, ("%s(MAC:0x%x): data = 0x%X,  DataBw40ABand = 0x%X,  DataBw80ABand = 0x%X, DataBw40GBand = 0x%x\n", 
			__FUNCTION__, TX_PWR_CFG_0 + (i * 4), data, DataBw40ABand, DataBw80ABand, DataBw40GBand));
	}

    DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));
}


/*
	========================================================================
	
	Routine Description:
		Enable Wlan function. this action will enable wlan clock so that chip can accept command. So MUST put in the 
		very beginning of Initialization. And put in the very LAST in the Halt function.

	Arguments:
		pAd		Pointer to our adapter

	Return Value:
		TRUE for success

	IRQL <= DISPATCH_LEVEL
	
	Note:
		Before Enable RX, make sure you have enabled Interrupt.
	========================================================================
*/
INT rt85592_wlan_chip_onoff(
	IN RTMP_ADAPTER *pAd,
	IN BOOLEAN bOn,
	IN BOOLEAN bResetWLAN)
{
	// TODO: check the functionality of the WLAN_FUN_CTRL here, now we just bring up it but no fine tune.
	WLAN_FUN_CTRL_STRUC WlanFunCtrl = {.word=0};
	UINT32 MacReg = 0;
#ifdef RTMP_MAC_USB
	int RET;
#endif /* RTMP_MAC_USB */

#ifdef RTMP_MAC_PCI
	RTMP_SEM_LOCK(&pAd->WlanEnLock);
#endif

#ifdef RTMP_MAC_USB
	RTMP_SEM_EVENT_WAIT(&(pAd->WlanEnLock), RET);
#endif

	RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);
	DBGPRINT(RT_DEBUG_OFF, ("==>%s(): OnOff:%d, pAd->WlanFunCtrl:0x%x, Reg-WlanFunCtrl=0x%x\n",
				__FUNCTION__, bOn, pAd->WlanFunCtrl.word, WlanFunCtrl.word));

	if (bResetWLAN == TRUE)
	{
		WlanFunCtrl.field.GPIO0_OUT_OE_N = 0xFF;
		WlanFunCtrl.field.FRC_WL_ANT_SET = 0;
	}

	if (bOn == TRUE)
	{
		WlanFunCtrl.field.WLAN_CLK_EN = 1;
		WlanFunCtrl.field.WLAN_EN = 1;
	}
	else
	{
		WlanFunCtrl.field.PCIE_APP0_CLK_REQ = 0;
		WlanFunCtrl.field.WLAN_EN = 0;
		WlanFunCtrl.field.WLAN_CLK_EN = 0;
	}

	DBGPRINT(RT_DEBUG_ERROR, ("WlanFunCtrl.word = 0x%x\n", WlanFunCtrl.word));
	RTMP_IO_FORCE_WRITE32(pAd, WLAN_FUN_CTRL, WlanFunCtrl.word);
	pAd->WlanFunCtrl.word = WlanFunCtrl.word;
	RtmpusecDelay(2);

	RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &WlanFunCtrl.word);
	DBGPRINT(RT_DEBUG_ERROR,
		("<== %s():  pAd->WlanFunCtrl.word = 0x%x, Reg->WlanFunCtrl=0x%x!\n",
		__FUNCTION__, pAd->WlanFunCtrl.word, WlanFunCtrl.word));
	
#ifdef RTMP_MAC_PCI
	RTMP_SEM_UNLOCK(&pAd->WlanEnLock);
#endif
	
#ifdef RTMP_MAC_USB  
	RTMP_SEM_EVENT_UP(&(pAd->WlanEnLock));
#endif

	return TRUE;
}


#ifdef IQ_CAL_SUPPORT
//
// Perform the IQ compensation
//
VOID PerformIQCompensation(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR Channel)
{
	if (IS_RT8592(pAd))
	{
		UCHAR IQImbalance,RFIQComp;
		UINT32 BbValue;
		
		//DBGPRINT(RT_DEBUG_TRACE, ("%s: IQControl = 0x%04x\n", __FUNCTION__,pAd->IQControl));

		
		RFIQComp = pAd->IQControl & 0xff;
		IQImbalance = ((pAd->IQControl & 0xff00) >> 8);

		if ((RFIQComp!=0) ||(IQImbalance!=0))
		{
			if (Channel <= 14)
			{
				//TX0
				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[0][0]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18 , BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[0][0]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);

				//TX1
				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[1][0]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[1][0]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);
			}
			else if ((Channel >= 36) && (Channel <=64))
			{
				//TX0
				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[0][1]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[0][1]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);				

				//TX1
				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[1][1]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[1][1]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);
			}
			else if ((Channel >= 100) && (Channel <=138))
			{
				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[0][2]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[0][2]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);	

				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[1][2]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);
		
				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[1][2]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);	
			}
			else if ((Channel >= 140) && (Channel <=165))
			{
				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[0][3]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[0][3]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);	

				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE0(BbValue, pAd->IQGainTx[1][3]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);
				
				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE1(BbValue, pAd->IQPhaseTx[1][3]);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);
			}
			else
			{
				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE0(BbValue, 0);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R18, &BbValue);
				BB_SET_BYTE1(BbValue, 0);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R18, BbValue);
				
				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE0(BbValue, 0);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);

				RTMP_BBP_IO_READ32(pAd, CAL_R19, &BbValue);
				BB_SET_BYTE1(BbValue, 0);
				RTMP_BBP_IO_WRITE32(pAd, CAL_R19, BbValue);
			}
			
			RTMP_BBP_IO_READ32(pAd, CAL_R5, &BbValue);
			BB_SET_BYTE0(BbValue, RFIQComp);
			RTMP_BBP_IO_WRITE32(pAd, CAL_R5, BbValue);
			
			// for delivered samples only
			// turn on TX compensation only
			// turn off RX compensation due to CISCO-1250 IOT issue
			RTMP_BBP_IO_READ32(pAd, CAL_R4, &BbValue);
			BB_SET_BYTE0(BbValue, IQImbalance);
			RTMP_BBP_IO_WRITE32(pAd, CAL_R4, BbValue);
		}
	}
}


//
// Read the IQ compensation configuration from eFuse
//
VOID ReadIQCompensationConfiguraiton(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_TRACE, ("%s: -->\n", __FUNCTION__));

	if (IS_RT8592(pAd))
	{
		USHORT	EepValue;

		//
		// Channel Pool - 0
		//
		RT28xx_EEPROM_READ16(pAd, 0x130, EepValue);
		pAd->IQGainTx[0][0] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[0][0] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);	

		RT28xx_EEPROM_READ16(pAd, 0x132, EepValue);
		pAd->IQGainTx[1][0] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);

		RT28xx_EEPROM_READ16(pAd, 0x134, EepValue);
		pAd->IQPhaseTx[1][0] = ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);

		//
		//Channel Pool - 1
		//
		RT28xx_EEPROM_READ16(pAd, 0x144, EepValue);
		pAd->IQGainTx[0][1] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[0][1] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);	

		RT28xx_EEPROM_READ16(pAd, 0x14a, EepValue);
		pAd->IQGainTx[1][1] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[1][1] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);

		//
		//Channel Pool - 2
		//
		RT28xx_EEPROM_READ16(pAd, 0x146, EepValue);
		pAd->IQGainTx[0][2] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[0][2] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);

		RT28xx_EEPROM_READ16(pAd, 0x14c, EepValue);
		pAd->IQGainTx[1][2] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[1][2] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);


		//
		//Channel Pool - 3
		//
		RT28xx_EEPROM_READ16(pAd, 0x148, EepValue);
		pAd->IQGainTx[0][3] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[0][3] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);

		RT28xx_EEPROM_READ16(pAd, 0x14e, EepValue);
		pAd->IQGainTx[1][3] 	= ((EepValue&0xff)==0xff)?(0x00):(EepValue&0xff);
		pAd->IQPhaseTx[1][3] = ((EepValue&0xff00)==0xff00)?(0x00):((EepValue&0xff00)>>8);	


		//
		//IQ imbalance and RF IQ control
		//
		RT28xx_EEPROM_READ16(pAd, 0x13c, EepValue);
		if ((EepValue&0xff)==0xff)
			EepValue &= 0xff00;
		if ((EepValue&0xff00)==0xff00)
			EepValue &= 0x00ff;	
		pAd->IQControl = EepValue;

		DBGPRINT(RT_DEBUG_ERROR, ("IQControl = 0x%04x\n",pAd->IQControl));

	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s: <--\n", __FUNCTION__));
}
#endif /* IQ_CAL_SUPPORT */


//-----------------------------------------------------------------------------------
UCHAR RT85592_EeBuffer[EEPROM_SIZE] = {
	0x83, 0x38, 0x01, 0x00, 0x00, 0x0c, 0x43, 0x28, 0x83, 0x00, 0x83, 0x28, 0x14, 0x18, 0xff, 0xff,
	0xff, 0xff, 0x83, 0x28, 0x14, 0x18, 0x00, 0x00, 0x01, 0x00, 0x6a, 0xff, 0x00, 0x02, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0c, 0x43, 0x28, 0x83, 0x01, 0x00, 0x0c,
	0x43, 0x28, 0x83, 0x02, 0x33, 0x0a, 0xec, 0x00, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0x20, 0x01, 0x55, 0x77, 0xa8, 0xaa, 0x8c, 0x88, 0xff, 0xff, 0x0a, 0x08, 0x08, 0x06,
	0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x66, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa,
	0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xaa, 0xaa,
	0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xcc, 0xaa,
	0x88, 0x66, 0xcc, 0xaa, 0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xaa, 0xaa,
	0x88, 0x66, 0xaa, 0xaa, 0x88, 0x66, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	} ;


static RTMP_REG_PAIR	RT85592_MACRegTable[] = {
	{PBF_SYS_CTRL,			0x80c00},
	{RLT_PBF_CFG,			0x1f},
	{FCE_CTRL,				0x1},
	{AMPDU_MAX_LEN_20M1S,	0xAA842211},
	{TX_SW_CFG0,			0x600},	/* rising time gap */
	{TX_SW_CFG1,			0x0},
	{TX_SW_CFG2,			0x0},
	{TXOP_CTRL_CFG, 		0x04101B3f},
	{PWR_PIN_CFG,			0x1},
	{MAX_LEN_CFG,			MAX_AGGREGATION_SIZE | 0x00001000},	/* 0x3018, MAX frame length. Max PSDU = 16kbytes.*/
// TODO: shiang-6590, check what tx report will send to us when following default value set as 2
	{0xa44,					0x0}, /* disable Tx info report */
// ---
#ifdef CONFIG_CSO_SUPPORT
	{0x80c,			0x8},
#endif /* CONFIG_CSO_SUPPORT */
};
static UCHAR RT85592_NUM_MAC_REG_PARMS = (sizeof(RT85592_MACRegTable) / sizeof(RTMP_REG_PAIR));


static RTMP_REG_PAIR RT85592_BBPRegTb[]={
	{CORE_R1,	0x40},
	{CORE_R4,	0x0},
	{CORE_R24,	0x0},
	{CORE_R32,	0x4003000a},
	{CORE_R42,	0x0},
	{CORE_R44,	0x0},

	{IBI_R0,		0x209a2098},
	{IBI_R1,		0x29252924},
	{IBI_R2,		0x29272926},
	{IBI_R3,		0x23512350},
	{IBI_R4,		0x29292928},
	{IBI_R5,		0x20ff20ff},
	{IBI_R6,		0x20ff20ff},
	{IBI_R7,		0x20ff20ff},
	{IBI_R9,		0xffffff3f},
	{IBI_R11,	0x80/*0x0*/},	/* BBP update: 1129e3*/

	{AGC1_R0,	0x00021400},
	{AGC1_R1,	0x00000001},
	
	//{AGC1_R2,	0x003A6464},
	{AGC1_R2,	0x003A0808},	/* BBP update:0921e3 */

	{AGC1_R22,	0x00001E21},
	{AGC1_R23,	0x0000272C},
	{AGC1_R24,	0x00002F3A},
	{AGC1_R25,	0x8000005a},
	{AGC1_R26,	0x00602004},
	//{AGC1_R27,	0x000000C8},
	{AGC1_R27,	0x000000FF}, /* BBP update: 1129e3*/

	{AGC1_R30,	0x00000006},
	{AGC1_R32,	0x00003232},
	{AGC1_R33,	0x00004650},
	{AGC1_R34,	0x000A0008},
	{AGC1_R35,	0x11111116},
	{AGC1_R37,	0x2121262C},
	{AGC1_R39,	0x2A2A3036},

	{AGC1_R41,	0x38383E45},
	{AGC1_R43,	0x27272B30},
	{AGC1_R45,	0x26262C31},
	{AGC1_R47,	0x26262B30},
	{AGC1_R49,	0x3333393E},

	{AGC1_R53,	0x26262A2F},
	{AGC1_R55,	0x40404E58},
	{AGC1_R57,	0x00001010},
	{AGC1_R58,	0x00001010},
	{AGC1_R59,	0x004B287D},

	{AGC1_R60,	0x00003214},
	{AGC1_R61,	0x404C362C},
	{AGC1_R62,	0x00362E2C},
	{AGC1_R63,	0x00006E30},

	{TXC_R1, 	0x0},
	
	{RXC_R1,	0x00000012},
	{RXC_R2,	0x00000011},
	{RXC_R3,	0x00000005},
	{RXC_R4,	0x0},

	{TXO_R8,	0x0},

	{TXBE_R0,	0x0},
	{TXBE_R2,	0x0},
	{TXBE_R3,	0x0},
	{TXBE_R4,	0x00000008},
	{TXBE_R6,	0x0},
	{TXBE_R8,	0x00000014},
	{TXBE_R9,	0x20000000},
	{TXBE_R10,	0x0},
	{TXBE_R12,	0x0},
	{TXBE_R13,	0x0},
	{TXBE_R14,	0x0},
	{TXBE_R15,	0x0},
	{TXBE_R16,	0x0},
	{TXBE_R17,	0x0},

	//{RXFE_R0,	0x006000e0},
	{RXFE_R0,	0x005000E6}, /* BBP update: 1129e3*/
	{RXFE_R3,	0x0},
	{RXFE_R4,	0x0},
#ifdef WFA_VHT_PF
	{RXO_R7,	0x290420ff},
#endif /* WFA_VHT_PF */
	{RXO_R13,	0x00000092},
	{RXO_R14,	0x00060612},
	{RXO_R15,	0xC832191C},	/* disable REMOD and MLD for E1 */
	{RXO_R16,	0x0000001E},
	{RXO_R17,	0x0},
	{RXO_R21,	0x00000001},
	{RXO_R24,	0x00000006},
	{RXO_R28,	0x0000003F},
};


static RTMP_REG_PAIR RT85592_BBPRegTb_GBand[]={
	{AGC1_R4,	0x1FEC9F49},
	{AGC1_R5,	0x1FEC9F49},
	{AGC1_R8,	0x0E3448C0},
	{AGC1_R9,	0x0E3448C0},

	{AGC1_R12,	0x804028F9},
	{AGC1_R13,	0x3C0A0412},
	{AGC1_R14,	0x00003A02},

	{AGC1_R31,	0x00000FE3},
	{AGC1_R51,	0x1A1A1C1C},
};
static UCHAR RT85592_BBP_GBand_Tb_Size = (sizeof(RT85592_BBPRegTb_GBand) / sizeof(RTMP_REG_PAIR));


static RTMP_REG_PAIR RT85592_BBPRegTb_ABand[]={
	{AGC1_R4,	0x1FF1A250},
	{AGC1_R5,	0x1FF1A250},
	{AGC1_R8,	0x0E2034C0},
	{AGC1_R9,	0x0E2034C0},

	{AGC1_R13,	0x34020412},
	{AGC1_R14,	0x00003202},

	{AGC1_R31,	0x00000FD3},
	{AGC1_R51,	0x17171C1C},
};
static UCHAR RT85592_BBP_ABand_Tb_Size = (sizeof(RT85592_BBPRegTb_ABand) / sizeof(RTMP_REG_PAIR));


static REG_PAIR rf_init_tb[]={
	{RF_R00,		0x80},
	{RF_R01,		0x3f},
/*	{RF_R02,		0x80},*/
	{RF_R03,		0x8},
	{RF_R05,		0x0},
	{RF_R07,		0x1b},
	{RF_R10,		0x94},

	{RF_R13,		0x41},
	{RF_R14,		0x08},
	{RF_R15,		0x40},
/*	{RF_R16,		0x07}, */ /* change for E2/E3 */
/*	{RF_R17,		0x26},*/
	{RF_R18,		0x03},
	{RF_R19,		0x08},

	{RF_R21,		0x8D},
	{RF_R26,		0x82},
	{RF_R27,		0x42},
	{RF_R28,		0x00},
	{RF_R29,		0x10},

	{RF_R30,		0x2C},
	{RF_R31,		0x38},
	{RF_R33,		0x00},
	{RF_R34,		0x07},
	{RF_R35,		0x12},

	{RF_R47,		0x0C},

	{RF_R53,		0x44},

	{RF_R63,		0x07},
};
static UCHAR rf_init_tb_sz = (sizeof(rf_init_tb) / sizeof(REG_PAIR));


static REG_PAIR_PHY rf_adapt_tb[]={
/*     reg, 	s_ch,e_ch, phy, bw, 	val */
	{RF_R06,	   1,	 14,	0xff, 0xff, 0x70},
	{RF_R06,	 36, 196,	0xff, 0xff, 0xF0},	

	{RF_R11,	   1,	 14, 0xff, 0xff, 0x48},
	{RF_R11,	 36, 196, 0xff, 0xff, 0x40},

	{RF_R12,	   1,	 14, 0xff, 0xff, 0x78},
	{RF_R12,	 36,  165, 0xff, 0xff, 0xEC},

	{RF_R20,	   1,	 14, 0xff, 0xff, 0x81},
	{RF_R20,	 36, 165, 0xff, 0xff, 0x01},

	{RF_R22,	   1,	 14, 0xff, RF_BW_20 | RF_BW_10, 0x70},
	{RF_R22,	   1,  14, 0xff, RF_BW_40, 0x75},
	{RF_R22,	   1,  14, 0xff, RF_BW_80, 0x7A},
	{RF_R22,	 36,165, 0xff, RF_BW_20 | RF_BW_10, 0xE0},
	{RF_R22,	 36,165, 0xff, RF_BW_40, 0xE5},
	{RF_R22,	 36,165, 0xff, RF_BW_80, 0xEA},

	{RF_R23,	   1,	 10, 0xff, 0xff, 0x09},
	{RF_R23,	 11,	 14, 0xff, 0xff, 0x08},
	{RF_R23,	 36,  48, 0xff, 0xff, 0x48},
	{RF_R23,	 50,  64, 0xff, 0xff, 0x40},
	{RF_R23,	100,165, 0xff, 0xff, 0xC0},

	{RF_R24,	   1,	 14, 0xff, 0xff, 0x4A},
	{RF_R24,	 36,  64, 0xff, 0xff, 0x07},
	{RF_R24,	100,130, 0xff, 0xff, 0x02},
	{RF_R24,	132,140, 0xff, 0xff, 0x01},
	{RF_R24,	149,165, 0xff, 0xff, 0x00},

	{RF_R25,	   1,	 14, 0xff, 0xff, 0x80},
	{RF_R25,	 36, 165, 0xff, 0xff, 0xBB},

	{RF_R32,	   1, 14, RF_MODE_CCK, 0xff, 0x80},
	{RF_R32,	   1, 165, RF_MODE_OFDM, 0xff, 0x40},

	{RF_R36,	   1,	 14, 0xff, 0xff, 0x80},
	{RF_R36,	 36, 165, 0xff, 0xff, 0x00},

	{RF_R37,	   1,	 14, 0xff, 0xff, 0x84},
	{RF_R37,	 36,  48, 0xff, 0xff, 0x88},
	{RF_R37,	 50, 165, 0xff, 0xff, 0x80},
	
	{RF_R38,	   1,	 14, 0xff, 0xff, 0x89},
	{RF_R38,	 36, 157, 0xff, 0xff, 0x85},
	{RF_R38,	159, 165, 0xff, 0xff, 0x8A},
	
	{RF_R39,	   1,	 14, 0xff, 0xff, 0x1B},
	{RF_R39,	 36,  64, 0xff, 0xff, 0x1C},
	{RF_R39,	100,138, 0xff, 0xff, 0x1A},
	{RF_R39,	140,165, 0xff, 0xff, 0x18},

	{RF_R40,	   1,	 14, 0xff, 0xff, 0x0C},
	{RF_R40,	 36, 157, 0xff, 0xff, 0x42},
	{RF_R40,	159, 165, 0xff, 0xff, 0x44},
	
	{RF_R41,	   1,	 14, 0xff, 0xff, 0x9B},
	{RF_R41,	 36, 157, 0xff, 0xff, 0xAB},
	{RF_R41,	159, 165, 0xff, 0xff, 0x8B},
	
	{RF_R42,	   1,	 14, 0xff, 0xff, 0xD5},
	{RF_R42,	 36, 165, 0xff, 0xff, 0xD7},
	
	{RF_R43,	   1,	 14, 0xff, 0xff, 0x72},
	{RF_R43,	 36,  64, 0xff, 0xff, 0xBB},
	{RF_R43,	100,138, 0xff, 0xff, 0x3B},
	{RF_R43,	140,165, 0xff, 0xff, 0x1B},

	{RF_R44,	   1,	 14, 0xff, 0xff, 0x2E},
	{RF_R44,	 36,  48, 0xff, 0xff, 0x45},
	{RF_R44,	 50,  64, 0xff, 0xff, 0x4A},
	{RF_R44,	100,112, 0xff, 0xff, 0x62},
	{RF_R44,	114,130, 0xff, 0xff, 0x72},
	{RF_R44,	132,140, 0xff, 0xff, 0x1A},
	{RF_R44,	149,165, 0xff, 0xff, 0x02},

	{RF_R45,	   1,	 14, 0xff, 0xff, 0x6A},
	{RF_R45,	 36,  48, 0xff, 0xff, 0x19},
	{RF_R45,	 50, 112, 0xff, 0xff, 0x29},
	{RF_R45,	114,138, 0xff, 0xff, 0x19},
	{RF_R45,	140,165, 0xff, 0xff, 0x11},

	{RF_R46,	   1,	 14, 0xff, 0xff, 0x6B},
	{RF_R46,	 36,  64, 0xff, 0xff, 0x8C},
	{RF_R46,	100, 112, 0xff, 0xff, 0x88},
	{RF_R46,	114, 165, 0xff, 0xff, 0x80},

	{RF_R48,	   1,	 14, 0xff, 0xff, 0x10},
	{RF_R48,	 36, 165, 0xff, 0xff, 0x00},

	{RF_R51,	   1,	 14, 0xff, 0xff, 0x3E},
	{RF_R51,	 36,  64, 0xff, 0xff, 0x9E},
	{RF_R51,	100,130, 0xff, 0xff, 0x9F},
	{RF_R51,	132,165, 0xff, 0xff, 0x9C},

	{RF_R52,	   1,	 14, 0xff, 0xff, 0x48},
	{RF_R52,	 36,  64, 0xff, 0xff, 0x08},
	{RF_R52,	100,165, 0xff, 0xff, 0x00},
	
	{RF_R54,	   1,	14, 0xff, 0xff, 0x48},
	{RF_R54,	 36,130, 0xff, 0xff, 0x99},
	{RF_R54,	132,140, 0xff, 0xff, 0x9B},
	{RF_R54,	149,165, 0xff, 0xff, 0x98},

	{RF_R55,	   1,	 14, RF_MODE_CCK, 0xff, 0x47},
	{RF_R55,	   1,	 14, RF_MODE_OFDM, 0xff, 0x43},
	{RF_R55,	 36,  48, 0xff, 0xff, 0x03},
	{RF_R55,	 50, 112, 0xff, 0xff, 0x09},
	{RF_R55,	114,130, 0xff, 0xff, 0x08},
	{RF_R55,	132,140, 0xff, 0xff, 0x0F},
	{RF_R55,	149,165, 0xff, 0xff, 0x0C},

	{RF_R56,	   1,	 14, 0xff, 0xff, 0xA1},
	{RF_R56,	 36,  64, 0xff, 0xff, 0xC4},
	{RF_R56,	100,130, 0xff, 0xff, 0x94},
	{RF_R56,	132,165, 0xff, 0xff, 0x8C},
	
	{RF_R57,	   1,	 14, 0xff, 0xff, 0x00},
	{RF_R57,	 36, 165, 0xff, 0xff, 0xFF},

	{RF_R58,	   1,	 14, 0xff, 0xff, 0x39},
	{RF_R58,	 36,  48, 0xff, 0xff, 0x21},
	{RF_R58,	 50,165, 0xff, 0xff, 0x09},

	{RF_R59,	   1,	10, 0xff, 0xff, 0x09},
	{RF_R59,	 11,	14, 0xff, 0xff, 0x08},
	{RF_R59,	 36, 48, 0xff, 0xff, 0x42},
	{RF_R59,	 50, 64, 0xff, 0xff, 0x40},
	{RF_R59,100,165, 0xff, 0xff, 0xC0},
	
	{RF_R60,	   1,	 14, 0xff, 0xff, 0x45},
	{RF_R60,	 36, 165, 0xff, 0xff, 0x05},
	
	{RF_R61,	  1,  14, 0xff, 0xff, 0x49},
	{RF_R61,	 36,165, 0xff, 0xff, 0x01},

	{RF_R62,	   1,	 14, 0xff, 0xff, 0x39},
	{RF_R62,	 36,  48, 0xff, 0xff, 0x21},
	{RF_R62,	 50, 165, 0xff, 0xff, 0x09},
};
static INT rf_adapt_tb_sz = (sizeof(rf_adapt_tb) / sizeof(REG_PAIR_PHY));


static REG_PAIR_PHY rf_adapt_tb_e3[]={
/*     reg, 	s_ch,e_ch, phy, bw, 	val */
	{RF_R23,	   1,	 10, 0xff, 0xff, 0x09},
	{RF_R23,	 11,	 14, 0xff, 0xff, 0x08},
	{RF_R23,	 36,  48, 0xff, 0xff, 0x4C},
	{RF_R23,	 50,  64, 0xff, 0xff, 0x40},
	{RF_R23,	100,130, 0xff, 0xff, 0xC0},
	{RF_R23,	132,140, 0xff, 0xff, 0xC6},
	{RF_R23,	149,165, 0xff, 0xff, 0xC8},

	{RF_R44,	   1,	 14, 0xff, 0xff, 0x2E},
	{RF_R44,	 36,  48, 0xff, 0xff, 0x65},
	{RF_R44,	 50, 130, 0xff, 0xff, 0x72},
	{RF_R44,	132,140, 0xff, 0xff, 0x32},
	{RF_R44,	149,165, 0xff, 0xff, 0x2A},

	{RF_R51,	   1,	 14, 0xff, 0xff, 0x3E},
	{RF_R51,	 36, 130, 0xff, 0xff, 0x9F},
	{RF_R51,	132,140, 0xff, 0xff, 0x9D},
	{RF_R51,	149,165, 0xff, 0xff, 0x9C},

	{RF_R54,	   1,	14, 0xff, 0xff, 0x48},
	{RF_R54,	 36, 48, 0xff, 0xff, 0x98},
	{RF_R54,	 50,112, 0xff, 0xff, 0x9A},
	{RF_R54,	114,140, 0xff, 0xff, 0x98},
	{RF_R54,	149,165, 0xff, 0xff, 0x9B},

	{RF_R55,	   1,	 14, RF_MODE_CCK, 0xff, 0x47},
	{RF_R55,	   1,	 14, RF_MODE_OFDM, 0xff, 0x43},
	{RF_R55,	 36,  48, 0xff, 0xff, 0x08},
	{RF_R55,	 50,  64, 0xff, 0xff, 0x0F},
	{RF_R55,	100,130, 0xff, 0xff, 0x09},
	{RF_R55,	132,140, 0xff, 0xff, 0x0F},
	{RF_R55,	149,165, 0xff, 0xff, 0x0E},

	{RF_R58,	   1,	 14, 0xff, 0xff, 0x39},
	{RF_R58,	 36,  48, 0xff, 0xff, 0x21},
	{RF_R58,	 50,140, 0xff, 0xff, 0x09},
	{RF_R58,	149,165, 0xff, 0xff, 0x11},

	{RF_R59,	   1,	10, 0xff, 0xff, 0x09},
	{RF_R59,	 11,	14, 0xff, 0xff, 0x08},
	{RF_R59,	 36, 48, 0xff, 0xff, 0x48},
	{RF_R59,	 50, 64, 0xff, 0xff, 0x40},
	{RF_R59,100,130, 0xff, 0xff, 0xC0},
	{RF_R59,132,140, 0xff, 0xff, 0xC6},
	{RF_R59,149,165, 0xff, 0xff, 0xC8},

	{RF_R62,	   1,	 14, 0xff, 0xff, 0x39},
	{RF_R62,	 36,  48, 0xff, 0xff, 0x31},
	{RF_R62,	 50, 140, 0xff, 0xff, 0x09},
	{RF_R62,	149, 165, 0xff, 0xff, 0x11},
};
static INT rf_adapt_tb_e3_sz = (sizeof(rf_adapt_tb_e3) / sizeof(REG_PAIR_PHY));


VOID dump_pwr_info(RTMP_ADAPTER *pAd)
{
	UINT32 mac_val;
	static UINT32 mac_reglist[]={TX_PWR_CFG_0, TX_PWR_CFG_1, TX_PWR_CFG_2,
						TX_PWR_CFG_3, TX_PWR_CFG_4, 
									    TX_PWR_CFG_7, TX_PWR_CFG_8,
						TX_PWR_CFG_9};
	static RTMP_STRING *pwr_rate_str[]={
							"O18/12/09/06 : C11/5.5/02/01", /* TX_PWR_CFG_0 */
							"H03/02/01/00 : O--/48/36/24", /* TX_PWR_CFG_1 */
							"H11/10/09/08 : H--/06/05/04", /* TX_PWR_CFG_2 */
							"S03/02/01/00 : H--/14/13/12", /* TX_PWR_CFG_3 */
							"N--/--/--/-- : S--/06/05/04", /* TX_PWR_CFG_4 */
							"H--/--/--/07 : O--/--/--/54", /* TX_PWR_CFG_7 */
							"N--/--/--/-- : H--/--/--/15", /* TX_PWR_CFG_8 */
							"N--/--/--/-- : S--/--/--/07" /* TX_PWR_CFG_9 */};
	UCHAR rf_val;
	USHORT ee_val, ee_addlist[]={0x6e, 0x70, 0x72, 0x74, 0x76, 0xd4, 0xd6, 0xd8, 0xda, 0xdc};
	INT idx;


	DBGPRINT(RT_DEBUG_OFF, ("\n%s(): TxPower Related Settings:\n", __FUNCTION__));
	DBGPRINT(RT_DEBUG_OFF, ("\tEEPROM Setting\n"));
	for (idx = 0; idx < (sizeof(ee_addlist) / sizeof(USHORT)); idx++)
	{
		RT28xx_EEPROM_READ16(pAd, ee_addlist[idx], ee_val);
		DBGPRINT(RT_DEBUG_OFF, ("\t\tEEPROM(%x)=0x%04x\n", ee_addlist[idx], ee_val));
	}

	DBGPRINT(RT_DEBUG_OFF, ("\tMAC Setting\n"));
	for (idx = 0; idx < (sizeof(mac_reglist) / sizeof(UINT32)); idx++)
	{
		RTMP_IO_READ32(pAd, mac_reglist[idx], &mac_val);
		DBGPRINT(RT_DEBUG_OFF, ("\t\tMAC(%x)=0x%08x(%s)\n", mac_reglist[idx], mac_val, pwr_rate_str[idx]));
	}
	
	DBGPRINT(RT_DEBUG_OFF, ("\tRF Setting\n"));
	RT30xxReadRFRegister(pAd, RF_R49, &rf_val);
	DBGPRINT(RT_DEBUG_OFF, ("\t\tRF_R49(TX0_ALC)=0x%x(0x%x)\n", rf_val & 0x3f, rf_val));
	RT30xxReadRFRegister(pAd, RF_R50, &rf_val);
	DBGPRINT(RT_DEBUG_OFF, ("\t\tRF_R50(TX1_ALC)=0x%x(0x%x)\n", rf_val & 0x3f, rf_val));
	
	DBGPRINT(RT_DEBUG_OFF, ("\tBBP Setting\n"));
	
}


#ifdef RTMP_TEMPERATURE_COMPENSATION
/* Power index table for G band */
static const TX_POWER_TUNING_ENTRY_STRUCT RT85592_TxPowerTuningTable_2G[] = {
/* 0  */ {0x00, -15}, 
/* 1  */ {0x01, -15}, 
/* 2  */ {0x00, -14}, 
/* 3  */ {0x01, -14}, 
/* 4  */ {0x00, -13}, 
/* 5  */ {0x01, -13}, 
/* 6  */ {0x00, -12}, 
/* 7  */ {0x01, -12}, 
/* 8  */ {0x00, -11}, 
/* 9  */ {0x01, -11}, 
/* 10 */ {0x00, -10}, 
/* 11 */ {0x01, -10}, 
/* 12 */ {0x00, -9}, 
/* 13 */ {0x01, -9}, 
/* 14 */ {0x00, -8}, 
/* 15 */ {0x01, -8}, 
/* 16 */ {0x00, -7}, 
/* 17 */ {0x01, -7}, 
/* 18 */ {0x00, -6}, 
/* 19 */ {0x01, -6}, 
/* 20 */ {0x00, -5}, 
/* 21 */ {0x01, -5}, 
/* 22 */ {0x00, -4}, 
/* 23 */ {0x01, -4}, 
/* 24 */ {0x00,	-3}, 
/* 25 */ {0x01,	-3}, 
/* 26 */ {0x00,	-2}, 
/* 27 */ {0x01, -2}, 
/* 28 */ {0x00,	-1}, 
/* 29 */ {0x01,	-1}, 
/* 30 */ {0x00,	0}, 
/* 31 */ {0x01, 0}, 
/* 32 */ {0x02,	0}, 
/* 33 */ {0x03,	0}, 
/* 34 */ {0x04,	0}, 
/* 35 */ {0x05,	0}, 
/* 36 */ {0x06, 0}, 
/* 37 */ {0x07, 0}, 
/* 38 */ {0x08,	0}, 
/* 39 */ {0x09,	0}, 
/* 40 */ {0x0A,	0}, 
/* 41 */ {0x0B,	0}, 
/* 42 */ {0x0C,	0}, 
/* 43 */ {0x0D,	0}, 
/* 44 */ {0x0E,	0}, 
/* 45 */ {0x0F, 0}, 
/* 46 */ {0x10,	0}, 
/* 47 */ {0x11,	0}, 
/* 48 */ {0x12,	0}, 
/* 49 */ {0x13,	0}, 
/* 50 */ {0x14,	0}, 
/* 51 */ {0x15, 0}, 
/* 52 */ {0x16,	0}, 
/* 53 */ {0x17,	0}, 
/* 54 */ {0x18,	0}, 
/* 55 */ {0x19,	0}, 
/* 56 */ {0x1A,	0}, 
/* 57 */ {0x1B,	0}, 
/* 58 */ {0x1C,	0}, 
/* 59 */ {0x1D,	0}, 
/* 60 */ {0x1E,	0}, 
/* 61 */ {0x1F,	0}, 
/* 62 */ {0x20,	0}, 
/* 63 */ {0x21,	0}, 
/* 64 */ {0x22,	0}, 
/* 65 */ {0x23, 0}, 
/* 66 */ {0x24,	0}, 
/* 67 */ {0x25,	0}, 
/* 68 */ {0x26,	0}, 
/* 69 */ {0x27,	0}, 
/* 70 */ {0x27-1, 1}, 
/* 71 */ {0x27,	1}, 
/* 72 */ {0x27-1, 2}, 
/* 73 */ {0x27,	2}, 
/* 74 */ {0x27-1, 3}, 
/* 75 */ {0x27,	3}, 
/* 76 */ {0x27-1, 4}, 
/* 77 */ {0x27, 4}, 
/* 78 */ {0x27-1, 5}, 
/* 79 */ {0x27,	5}, 
/* 80 */ {0x27-1, 6}, 
/* 81 */ {0x27, 6}, 
/* 82 */ {0x27-1, 7}, 
/* 83 */ {0x27,	7}, 
/* 84 */ {0x27-1, 8}, 
/* 85 */ {0x27, 8}, 
/* 86 */ {0x27-1, 9}, 
/* 87 */ {0x27, 9}, 
/* 88 */ {0x27-1, 10}, 
/* 89 */ {0x27, 10}, 
/* 90 */ {0x27-1, 11}, 
/* 91 */ {0x27,	11}, 
/* 92 */ {0x27-1, 12}, 
/* 93 */ {0x27,	12}, 
/* 94 */ {0x27-1, 13}, 
/* 95 */ {0x27,	13}, 
/* 96 */ {0x27-1, 14}, 
/* 97 */ {0x27,	14}, 
/* 98 */ {0x27-1, 15}, 
/* 99 */ {0x27, 15}, 
};


/* Power index table for A band */
static const TX_POWER_TUNING_ENTRY_STRUCT RT85592_TxPowerTuningTable_5G[] = {
/* 0  */ {0x00,	-15}, 
/* 1  */ {0x01,	-15}, 
/* 2  */ {0x00,	-14}, 
/* 3  */ {0x01,	-14}, 
/* 4  */ {0x00,	-13}, 
/* 5  */ {0x01,	-13}, 
/* 6  */ {0x00,	-12}, 
/* 7  */ {0x01,	-12}, 
/* 8  */ {0x00,	-11}, 
/* 9  */ {0x01,	-11}, 
/* 10 */ {0x00,	-10}, 
/* 11 */ {0x01,	-10}, 
/* 12 */ {0x00,	-9}, 
/* 13 */ {0x01,	-9}, 
/* 14 */ {0x00,	-8}, 
/* 15 */ {0x01,	-8}, 
/* 16 */ {0x00,	-7}, 
/* 17 */ {0x01,	-7}, 
/* 18 */ {0x00,	-6}, 
/* 19 */ {0x01,	-6}, 
/* 20 */ {0x00, -5}, 
/* 21 */ {0x01, -5}, 
/* 22 */ {0x00,	-4}, 
/* 23 */ {0x01,	-4}, 
/* 24 */ {0x00,	-3}, 
/* 25 */ {0x01, -3}, 
/* 26 */ {0x00,	-2}, 
/* 27 */ {0x01,	-2}, 
/* 28 */ {0x00, -1}, 
/* 29 */ {0x01,	-1}, 
/* 30 */ {0x00,	0}, 
/* 31 */ {0x01,	0}, 
/* 32 */ {0x02,	0}, 
/* 33 */ {0x03, 0}, 
/* 34 */ {0x04,	0}, 
/* 35 */ {0x05,	0}, 
/* 36 */ {0x06,	0}, 
/* 37 */ {0x07,	0}, 
/* 38 */ {0x08, 0}, 
/* 39 */ {0x09, 0}, 
/* 40 */ {0x0A, 0}, 
/* 41 */ {0x0B,	0}, 
/* 42 */ {0x0C,	0}, 
/* 43 */ {0x0D, 0}, 
/* 44 */ {0x0E,	0}, 
/* 45 */ {0x0F,	0}, 
/* 46 */ {0x10,	0}, 
/* 47 */ {0x11,	0}, 
/* 48 */ {0x12,	0}, 
/* 49 */ {0x13,	0}, 
/* 50 */ {0x14,	0}, 
/* 51 */ {0x15,	0}, 
/* 52 */ {0x16,	0}, 
/* 53 */ {0x17,	0}, 
/* 54 */ {0x18,	0}, 
/* 55 */ {0x19,	0}, 
/* 56 */ {0x1A,	0}, 
/* 57 */ {0x1B,	0}, 
/* 58 */ {0x1C,	0}, 
/* 59 */ {0x1D,	0}, 
/* 60 */ {0x1E,	0}, 
/* 61 */ {0x1F,	0}, 
/* 62 */ {0x20,	0}, 
/* 63 */ {0x21,	0}, 
/* 64 */ {0x22,	0}, 
/* 65 */ {0x23, 0}, 
/* 66 */ {0x24,	0}, 
/* 67 */ {0x25,	0}, 
/* 68 */ {0x26,	0}, 
/* 69 */ {0x27,	0}, 
/* 70 */ {0x28,	0},
/* 71 */ {0x29,	0},
/* 72 */ {0x2A,	0},
/* 73 */ {0x2B,	0},
/* 74 */ {0x2B-1, 1}, 
/* 75 */ {0x2B,	1}, 
/* 76 */ {0x2B-1, 2}, 
/* 77 */ {0x2B,	2}, 
/* 78 */ {0x2B-1, 3}, 
/* 79 */ {0x2B,	3}, 
/* 80 */ {0x2B-1, 4}, 
/* 81 */ {0x2B,	4}, 
/* 82 */ {0x2B-1, 5}, 
/* 83 */ {0x2B, 5}, 
/* 84 */ {0x2B-1, 6}, 
/* 85 */ {0x2B, 6}, 
/* 86 */ {0x2B-1, 7}, 
/* 87 */ {0x2B, 7}, 
/* 88 */ {0x2B-1, 8}, 
/* 89 */ {0x2B, 8}, 
/* 90 */ {0x2B-1, 9}, 
/* 91 */ {0x2B, 9}, 
/* 92 */ {0x2B-1, 10}, 
/* 93 */ {0x2B, 10}, 
/* 94 */ {0x2B-1, 11}, 
/* 95 */ {0x2B, 11}, 
/* 96 */ {0x2B-1, 12}, 
/* 97 */ {0x2B, 12}, 
/* 98 */ {0x2B-1, 13}, 
/* 99 */ {0x2B, 13}, 
/* 100 */{0x2B-1, 14}, 
/* 101 */{0x2B, 14}, 
/* 102 */{0x2B-1, 15}, 
/* 103 */{0x2B, 15}, 
};


typedef struct  _rx_temp_comp_struct
{
	CHAR adc_val;
	CHAR comp_g[2];	/* 0: 1ss, 1: 2ss */
	CHAR comp_a[2]; /* 0: 1ss, 1: 2ss */
}RX_TEMP_COMP_STRUCT;


RX_TEMP_COMP_STRUCT rx_temp_comp_tb[] = 
{
	{-56,	{-2, -2},	{-2, -2}},	/* range < -48 */
	{-48,	{0, -2},	{0, -2}},		/* range >= -48 && < -36 */
	{-36,	{0,	0},	{0, -2}},		/* range >= -36 && < -28 */
	{-28,	{0,	0},	{0, 0}},		/* range >= -28 && < -16 */
	{-16,	{0,	0},	{0, 0}},		/* range >= -16 && < -4 */
	{  -4,	{0,	0},	{0, 0}},		/* range >= -4 && < 4 */
	{   4,	{0,	2},	{0, 2}},		/* range >= 4 && < 16 */
	{ 16,	{2,	4},	{2, 2}},		/* range >= 16 && < 26 */
	{ 26,	{4,	6},	{4, 4}},		/* range >= 26 && < 36 */
	{ 36,	{6,	8},	{4, 6}},		/* range >= 36 && < 48 */
	{ 48,	{8, 10},	{6, 8}},		/* range >= 48 && < 57 */
	{ 57,	{12, 10},	{8, 8}},		/* range >= 57 */
};

#define RX_TEMP_COMP_TB_SZ (sizeof(rx_temp_comp_tb) / sizeof(RX_TEMP_COMP_STRUCT))


/*@!Release
	RF Rx init VGA temperatrue compensation
*/
extern int rx_temp_dbg;

INT rx_temp_compensation(RTMP_ADAPTER *pAd)
{
	CHAR temper_val, comp_db, vga_val;
	UINT32 bbp_val;
	INT idx;
	RX_TEMP_COMP_STRUCT *rx_tmp = NULL;


	temper_val = pAd->curr_temp;
	if (rx_temp_dbg)
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): do Rx Temp comp, temper_val=%d\n", __FUNCTION__, temper_val));

	/* Temperature value already read in AsicGetAutoAgcOffsetForTemperatureSensor() */
	//bbp_get_temp(pAd, &temper_val);

	ASSERT((pAd->CommonCfg.RxStream <= 2));
	
	for (idx = 0; idx < RX_TEMP_COMP_TB_SZ; idx++)
	{
		rx_tmp = &rx_temp_comp_tb[idx];
		if (temper_val < rx_tmp->adc_val) {
			idx = (idx == 0 ? 0 : (idx-1));
			break;
		} else if (temper_val == rx_tmp->adc_val)
			break;
	}

	if (idx == RX_TEMP_COMP_TB_SZ)
		idx--;
	rx_tmp = &rx_temp_comp_tb[idx];
	if (pAd->CommonCfg.Channel <= 14)
		comp_db = rx_tmp->comp_g[pAd->CommonCfg.RxStream - 1];
	else
		comp_db = rx_tmp->comp_a[pAd->CommonCfg.RxStream - 1];
	if (rx_temp_dbg)
		DBGPRINT(RT_DEBUG_TRACE, ("\tDepends on temperature value, rx_temp_comp_tb[idx:%d, compensate:%d]\n",
					idx, comp_db));
	
	RTMP_BBP_IO_READ32(pAd, AGC1_R8, &bbp_val);
	vga_val = (CHAR)(pAd->rx_temp_base[0] + comp_db * 2);
	if (vga_val < 0x0)
		vga_val = 0x0;
	if (vga_val > 0x5c)
		vga_val = 0x5c;
	bbp_val &= 0xFFFF80FF;
	bbp_val |= (vga_val << 8);
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val);
	if (rx_temp_dbg)
		DBGPRINT(RT_DEBUG_TRACE, ("\tSet AGC1_R8=0x%x\n", bbp_val));
	
	RTMP_BBP_IO_READ32(pAd, AGC1_R9, &bbp_val);
	vga_val = (CHAR)(pAd->rx_temp_base[1] + comp_db * 2);
	if (vga_val < 0x0)
		vga_val = 0x0;
	if (vga_val > 0x5c)
		vga_val = 0x5c;
	bbp_val &= 0xFFFF80FF;
	bbp_val |= (vga_val << 8);
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val);
	if (rx_temp_dbg)
		DBGPRINT(RT_DEBUG_TRACE, ("\tSet AGC1_R9=0x%x\n", bbp_val));
	
	return TRUE;
}


/*@!Release
	RF Tx temperatrue compensation
	
	
*/
static INT tx_temp_comp(RTMP_ADAPTER *pAd)
{


	return TRUE;
}
#endif /* RTMP_TEMPERATURE_COMPENSATION */


INT rt85592_lna_gain_adjust(RTMP_ADAPTER *pAd, CHAR gain)
{
	UINT32 bbp_val[2];

	RTMP_BBP_IO_READ32(pAd, AGC1_R8, &bbp_val[0]);
	RTMP_BBP_IO_READ32(pAd, AGC1_R9, &bbp_val[1]);
	bbp_val[0] = ((bbp_val[0] & 0xffffff3f) | (gain << 6));
	bbp_val[1] = ((bbp_val[1] & 0xffffff3f) | (gain << 6));
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val[0]);
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val[1]);

	return 0;
}


#ifndef WFA_VHT_PF
INT rt85592_cca_adjust(RTMP_ADAPTER *pAd)
{
	if (pAd->ChipID == RT85592_E3)
	{
		INT round = pAd->Mlme.PeriodicRound % 10;
		BOOLEAN pd_cfg = FALSE;
		UINT32 cca_cnt = 0;
		UCHAR rf_val;

// TODO: remove it after bring up finished!
		round = 0;
// TODO: ----End

		switch (round)
		{
			case 1:
				// T-1 Start, reset CCA count=0
				RTMP_IO_READ32(pAd, 0x1704, &cca_cnt);
				break;
			case 2:
				// T-1 End, get CCA and check if need to adjust!
				RTMP_IO_READ32(pAd, 0x1704, &cca_cnt);
				if (cca_cnt > 200)
					pd_cfg = TRUE;
				break;
			default:
				break;
		}

		if (pd_cfg)
		{
			// RF_R31[7:3]=00000
			RT30xxReadRFRegister(pAd, RF_R31, &rf_val);
			rf_val &= 0x07;
			RT30xxWriteRFRegister(pAd, RF_R31, rf_val);

			rt85592_lna_gain_adjust(pAd, 2);
		} else {
			// back to original value
		}

		if (round != 0) {
			DBGPRINT(RT_DEBUG_INFO, ("%s():round=%d, pd_cfg=%d\n",
						__FUNCTION__, round, pd_cfg));
		}
	}

	return 0;
}
#endif /* WFA_VHT_PF */


/*
	NOTE: MAX_NUM_OF_CHANNELS shall  equal sizeof(txpwr_chlist))
*/
static UCHAR txpwr_chlist[] = {
	1, 2,3,4,5,6,7,8,9,10,11,12,13,14,
	36,38,40,44,46,48,52,54,56,60,62,64,
	100,102,104,108,110,112,116,118,120,124,126,128,132,134,136,140,
	149,151,153,157,159,161,165,167,169,171,173,
	42, 58, 106, 122, 155,
};


INT RT85592_ReadChannelPwr(RTMP_ADAPTER *pAd)
{
	UINT32 i, choffset, idx, ss_idx, ss_offset_g[2], ss_num;
	EEPROM_TX_PWR_STRUC Power;
	EEPROM_TX_PWR_STRUC Power2;
	CHAR tx_pwr1, tx_pwr2;

	/*
		Read Tx power value for all channels
		Value in range of 1 ~ 0x7f. Default value is 24.
		Power value:
			2.4G => 0x00 (0) ~ 0x1F (31)
			5.5G => 0xF9 (-7) ~ 0x0F (15)
	*/
	DBGPRINT(RT_DEBUG_TRACE, ("%s()--->\n", __FUNCTION__));
	choffset = 0;
	ss_num = 2;
	for (i = 0; i < sizeof(txpwr_chlist); i++)
	{
		pAd->TxPower[i].Channel = txpwr_chlist[i];
		pAd->TxPower[i].Power = DEFAULT_RF_TX_POWER;
		pAd->TxPower[i].Power2 = DEFAULT_RF_TX_POWER;
	}

	/* 0. 11b/g, ch1 - ch 14, 2SS */
	ss_offset_g[0] = EEPROM_G_TX_PWR_OFFSET;
	ss_offset_g[1] = EEPROM_G_TX2_PWR_OFFSET;
	for (ss_idx = 0; ss_idx < 2; ss_idx++)
	{
		for (i = 0; i < 7; i++)
		{
			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, ss_offset_g[ss_idx] + idx, Power.word);

			tx_pwr1 = tx_pwr2 = DEFAULT_RF_TX_POWER;
			if ((Power.field.Byte0 <= 0x27) && (Power.field.Byte0 >= 0))
				tx_pwr1 = Power.field.Byte0;
			if ((Power.field.Byte1 <= 0x27) || (Power.field.Byte1 >= 0))
				tx_pwr2 = Power.field.Byte1;

			if (ss_idx == 0) {
				pAd->TxPower[idx].Power = tx_pwr1;
				pAd->TxPower[idx + 1].Power = tx_pwr2;
			}
			else {
				pAd->TxPower[idx].Power2 = tx_pwr1;
				pAd->TxPower[idx + 1].Power2 = tx_pwr2;
			}
			choffset++;
		}
	}

	{
		/* 1. U-NII lower/middle band: 36, 38, 40; 44, 46, 48; 52, 54, 56; 60, 62, 64 (including central frequency in BW 40MHz)*/
		ASSERT((pAd->TxPower[choffset].Channel == 36));
		choffset = 14;
		ASSERT((pAd->TxPower[choffset].Channel == 36));
		for (i = 0; i < 6; i++)
		{
			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + idx, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + idx, Power2.word);

			if ((Power.field.Byte0 < 0x20) && (Power.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 < 0x20) && (Power.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;			

			if ((Power2.field.Byte0 < 0x20) && (Power2.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 < 0x20) && (Power2.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power2 = Power2.field.Byte1;
		}

		/* 2. HipperLAN 2 100, 102 ,104; 108, 110, 112; 116, 118, 120; 124, 126, 128; 132, 134, 136; 140 (including central frequency in BW 40MHz)*/
		choffset = 14 + 12;
		ASSERT((pAd->TxPower[choffset].Channel == 100));
		for (i = 0; i < 8; i++)
		{
			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + idx, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + (choffset - 14) + idx, Power2.word);

			if ((Power.field.Byte0 < 0x20) && (Power.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 < 0x20) && (Power.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;

			if ((Power2.field.Byte0 < 0x20) && (Power2.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 < 0x20) && (Power2.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power2 = Power2.field.Byte1;
		}

		/* 3. U-NII upper band: 149, 151, 153; 157, 159, 161; 165, 167, 169; 171, 173 (including central frequency in BW 40MHz)*/
		choffset = 14 + 12 + 16;
		ASSERT((pAd->TxPower[choffset].Channel == 149));
		for (i = 0; i < 6; i++)
		{
			idx = i * 2;
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX_PWR_OFFSET + (choffset - 14) + idx, Power.word);
			RT28xx_EEPROM_READ16(pAd, EEPROM_A_TX2_PWR_OFFSET + (choffset - 14) + idx, Power2.word);

			if ((Power.field.Byte0 < 0x20) && (Power.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power = Power.field.Byte0;

			if ((Power.field.Byte1 < 0x20) && (Power.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power = Power.field.Byte1;

			if ((Power2.field.Byte0 < 0x20) && (Power2.field.Byte0 >= 0))
				pAd->TxPower[idx + choffset + 0].Power2 = Power2.field.Byte0;

			if ((Power2.field.Byte1 < 0x20) && (Power2.field.Byte1 >= 0))
				pAd->TxPower[idx + choffset + 1].Power2 = Power2.field.Byte1;
		}

		/* choffset = 14 + 12 + 16 + 7; */
		choffset = 14 + 12 + 16 + 11;

#ifdef DOT11_VHT_AC
		ASSERT((pAd->TxPower[choffset].Channel == 42));

		// TODO: shiang-6590, fix me for the TxPower setting code here!
		/* For VHT80MHz, we need assign tx power for central channel 42, 58, 106, 122, and 155 */
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Update Tx power control of the central channel (42, 58, 106, 122 and 155) for VHT BW80\n", __FUNCTION__));
		
		NdisMoveMemory(&pAd->TxPower[53], &pAd->TxPower[16], sizeof(CHANNEL_TX_POWER)); // channel 42 = channel 40
		NdisMoveMemory(&pAd->TxPower[54], &pAd->TxPower[22], sizeof(CHANNEL_TX_POWER)); // channel 58 = channel 56
		NdisMoveMemory(&pAd->TxPower[55], &pAd->TxPower[28], sizeof(CHANNEL_TX_POWER)); // channel 106 = channel 104
		NdisMoveMemory(&pAd->TxPower[56], &pAd->TxPower[34], sizeof(CHANNEL_TX_POWER)); // channel 122 = channel 120
		NdisMoveMemory(&pAd->TxPower[57], &pAd->TxPower[44], sizeof(CHANNEL_TX_POWER)); // channel 155 = channel 153

		pAd->TxPower[choffset].Channel = 42;
		pAd->TxPower[choffset+1].Channel = 58;
		pAd->TxPower[choffset+2].Channel = 106;
		pAd->TxPower[choffset+3].Channel = 122;
		pAd->TxPower[choffset+4].Channel = 155;

		choffset += 5;		/* the central channel of VHT80 */
		choffset = (MAX_NUM_OF_CHANNELS - 1);
#endif /* DOT11_VHT_AC */

		/* 4. Print and Debug*/
		for (i = 0; i < choffset; i++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("E2PROM: TxPower[%03d], Channel=%d, Power[Tx0:%d, Tx1:%d]\n",
						i, pAd->TxPower[i].Channel, pAd->TxPower[i].Power, pAd->TxPower[i].Power2 ));
		}
	}

	return TRUE;
}


static INT rf_tr_agc_config(RTMP_ADAPTER *pAd, INT rf_bw)
{
	signed char rx_agc_fc_offset[3] = {2,2,2}; /* array idx 0: 20M, 1:40M, 2:80m */
	UINT8 tx_agc_fc_offset[3] = {0,0,0}; /* array idx 0: 20M, 1:40M, 2:80m */
	CHAR rf32_val, rf31_val, rf_diff;

	if (pAd->ChipID == RT85592_E3) {
		rx_agc_fc_offset[0] = -2;
		rx_agc_fc_offset[1] = 0;
		rx_agc_fc_offset[2] = 0;
	}

	RT30xxReadRFRegister(pAd, RF_R32, &rf32_val);
	RT30xxReadRFRegister(pAd, RF_R31, &rf31_val);
	rf32_val &= (~0xf8);
	rf31_val &= (~0xf8);
	if (rf_bw == RF_BW_40)
	{
		rf32_val |= (pAd->bw_cal[1] << 3);
		rf_diff = pAd->bw_cal[1] + rx_agc_fc_offset[1];
		if (rf_diff > 0x1f)
			rf_diff = 0x1f;
		rf31_val |= (rf_diff << 3);
	}
	else if (rf_bw == RF_BW_20)
	{
		rf32_val |= ((pAd->bw_cal[1] + tx_agc_fc_offset[0]) << 3);
		rf_diff = pAd->bw_cal[1] + rx_agc_fc_offset[0];
		if (rf_diff > 0x1f)
			rf_diff = 0x1f;
		rf31_val |= (rf_diff << 3);
	}
	else if (rf_bw == RF_BW_80)
	{
		rf_diff = (pAd->bw_cal[1] - tx_agc_fc_offset[2]);
		if (rf_diff < 0)
			rf_diff = 0;
		pAd->bw_cal[2] = rf_diff;
		rf32_val |= (pAd->bw_cal[2] << 3);

		rf_diff = (pAd->bw_cal[2] - rx_agc_fc_offset[2]);
		if ( rf_diff < 0)
			rf_diff = 0;
		rf31_val |= (rf_diff << 3);
	}

	RT30xxWriteRFRegister(pAd, RF_R32, rf32_val);
	RT30xxWriteRFRegister(pAd, RF_R31, rf31_val);

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): BW Calibration Value=>20MHz:%02x, 40MHz:%02x,80MHz:%02x\n",
				__FUNCTION__, pAd->bw_cal[0], pAd->bw_cal[1], pAd->bw_cal[2]));
	DBGPRINT(RT_DEBUG_TRACE, ("\tApply BW Calibration Value for BW%d =>RF_R32:%02x, RF_R31:%02x!\n",
				(rf_bw == RF_BW_40 ? 40 : (rf_bw == RF_BW_20 ? 20 : 80)), rf32_val, rf31_val));
	return 0;
}


static INT bbp_core_soft_reset(RTMP_ADAPTER *pAd, BOOLEAN set_bw, INT bw)
{
	UINT32 bbp_val;

	RTMP_BBP_IO_READ32(pAd, CORE_R4, &bbp_val);
	bbp_val |= 0x1;
	RTMP_BBP_IO_WRITE32(pAd, CORE_R4, bbp_val);
	RtmpusecDelay(1000);

	if (set_bw == TRUE) {
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


static CHAR lp_tx_filter_bw_cal(RTMP_ADAPTER *pAd)
{
	INT cnt;
	UINT32 bbp_val;
	CHAR cal_val;
	
	RTMP_BBP_IO_WRITE32(pAd, CAL_R1, 0x82);
	cnt = 0;
	do {
		RtmpusecDelay(1000);
		RTMP_BBP_IO_READ32(pAd, CAL_R1, &bbp_val);
		if (bbp_val == 0x02 || cnt == 20)
			break;
		cnt++;
	}while(cnt < 20);
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("\t\tCheck CAL_R1(Cnt:%d, Value:0x%x)\n",
			cnt, bbp_val));
#endif /* RELEASE_EXCLUDE */

	RTMP_BBP_IO_READ32(pAd, CAL_R32, &bbp_val);
	cal_val = bbp_val & 0x7F;
	if (cal_val >= 0x40)
		cal_val -= 128;
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("\t\tCAL_R32:0x%x => cal_val=0x%02x\n",
			bbp_val, cal_val & 0xff));
#endif /* RELEASE_EXCLUDE */

	return cal_val;
}


static INT cal_freq_shift(RTMP_ADAPTER *pAd, INT tone)
{
	UINT32 bbp_val;

	RTMP_BBP_IO_READ32(pAd, CAL_R3, &bbp_val);
	bbp_val &= (~0x6);
	if (tone <=3)
		bbp_val |= (tone << 1);	
	RTMP_BBP_IO_WRITE32(pAd, CAL_R3, bbp_val);

	return 0;
}


static INT bbp_lp_config(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	UCHAR rf_val;

	RT30xxReadRFRegister(pAd, RF_R36, &rf_val);
	if (enable == TRUE)
		rf_val |= 0x02;
	else
		rf_val &= (~0x02);
	RT30xxWriteRFRegister(pAd, RF_R36, rf_val);

	return 0;
}


static INT rf_txf_mode(RTMP_ADAPTER *pAd, INT bw)
{
	UCHAR rf_val;

	RT30xxReadRFRegister(pAd, RF_R22, &rf_val);
	rf_val &= (~0x0c);
	switch (bw)
	{
		case BW_40:
			rf_val |= 0x04;
			break;
		case BW_80:
			rf_val |= 0x08;
			break;
		case BW_20:
		default:
			break;
	}	
	RT30xxWriteRFRegister(pAd, RF_R22, rf_val);

	return 0;
}


static INT rf_tx_agc_fc(RTMP_ADAPTER *pAd, CHAR tx_agc_fc)
{
	UCHAR rf_val;
	
	RT30xxReadRFRegister(pAd, RF_R32, (PUCHAR)&rf_val);
	rf_val = (rf_val & (~0xf8)) | (tx_agc_fc << 3);
	RT30xxWriteRFRegister(pAd, RF_R32, (UCHAR)rf_val);
	RtmpusecDelay(10000);

	return 0;
}


/*@!Release
	RF Bandwidth calibration
	
	
*/
INT bw_filter_cal(RTMP_ADAPTER *pAd)
{
	UINT8 tx_agc_fc;
	UINT8 filter_target, filter_target_20m = 0x0b, filter_target_40m = 0x12;
	INT loop = 0, bw;
	UINT32 bbp_val;
	CHAR cal_r32_init, cal_r32_val, cal_diff;

	if (pAd->ChipID == RT85592_E3)
		filter_target_40m = 0x10;

	//I-3
	RTMP_BBP_IO_READ32(pAd, TXBE_R1, &bbp_val);
	bbp_val = ((bbp_val & (~0xff00)) | (0x66<<8));
	RTMP_BBP_IO_WRITE32(pAd, TXBE_R1, bbp_val);

	do{
		// I-4,5,6,7,8,9
		if (loop == 0) {
			bw = BW_20;
			filter_target = filter_target_20m;
		}
		else
		{
			bw = BW_40;
			filter_target = filter_target_40m;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("%s():Start BW Cal for %dMHz\n", 
					__FUNCTION__, (bw == BW_20 ? 20 : 40)));
		rf_txf_mode(pAd, bw);

		bbp_core_soft_reset(pAd, TRUE, bw);

 		bbp_lp_config(pAd, TRUE);

		tx_agc_fc = 0;
		rf_tx_agc_fc(pAd, tx_agc_fc);

		cal_freq_shift(pAd, 0);

		bbp_core_soft_reset(pAd, FALSE, 0);

		cal_r32_init = lp_tx_filter_bw_cal(pAd);
		cal_freq_shift(pAd, 3);
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("\tcal_r32_init=%d\n", cal_r32_init));
#endif /* RELEASE_EXCLUDE */

do_cal:
		rf_tx_agc_fc(pAd, tx_agc_fc);

		bbp_core_soft_reset(pAd, FALSE, 0);

		cal_r32_val = lp_tx_filter_bw_cal(pAd);
		cal_diff = cal_r32_init - cal_r32_val;
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("\t\tR32(Init=%d,Cal=%d,Diff=%d,FilterTarget=%d,TxAgcFc=%d)\n",
						cal_r32_init, cal_r32_val, cal_diff, filter_target, tx_agc_fc));
#endif /* RELEASE_EXCLUDE */
		if (((cal_diff > filter_target) && (tx_agc_fc == 0)) ||
			((cal_diff < filter_target) && (tx_agc_fc == 0x1f)))
		{
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("\t\tBW%dM FilterCal cannot find valid val(loop=%d), set TxAgcFc=0\n",
							(loop == 0 ? 20 : 40), loop));
#endif /* RELEASE_EXCLUDE */
			tx_agc_fc = 0;
		}
		else if ((cal_diff <= filter_target) && (tx_agc_fc < 0x1f))
		{
			tx_agc_fc++;
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("\t\ttx_agc_fc++, redo cal!\n"));
#endif /* RELEASE_EXCLUDE */
			goto do_cal;
		}
		else
		{
			/* do nothing */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("\t\tget tx_agc_fc, cal done!\n"));
#endif /* RELEASE_EXCLUDE */
		}

#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("\t=>BW %dMHz FilterCal Value=0x%x!\n",
						(loop == 0 ? 20 : 40), tx_agc_fc));
#endif /* RELEASE_EXCLUDE */

		if (loop == 0)
			pAd->bw_cal[0] = tx_agc_fc;
		else
			pAd->bw_cal[1] = tx_agc_fc;

		loop++;
	}while(loop <= 1);

	bbp_lp_config(pAd, FALSE);

	rf_txf_mode(pAd, BW_20);

	RTMP_BBP_IO_WRITE32(pAd, CAL_R1, 0);

	cal_freq_shift(pAd, 0);

	RTMP_BBP_IO_READ32(pAd, CORE_R1, &bbp_val);
	bbp_val &= (~0x18);
	RTMP_BBP_IO_WRITE32(pAd, CORE_R1, bbp_val);

	DBGPRINT(RT_DEBUG_TRACE, ("BW Cal Result =>\n\t20MHz: %02x\n\t40MHz: %02x\n\t80MHz: %02x\n", 
					pAd->bw_cal[0], pAd->bw_cal[1], pAd->bw_cal[2]));
	return TRUE;
}


/*@!Release
	RF R calibration
	
	
*/
static INT rt85592_R_cal(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_TRACE, ("-->%s(): Do R Calibration!\n", __FUNCTION__));

	/* Init RF calibration, toggle bit 7 before init RF registers */
	/*
	UINT8 rf_val;

	RT30xxReadRFRegister(pAd, RF_R02, (PUCHAR)&rf_val);
	rf_val = ((rf_val & ~0x80) | 0x80);
	*/
	RT30xxWriteRFRegister(pAd, RF_R02, (UCHAR)0x80);
	RtmpusecDelay(1000);
#if 0
	rf_val = ((rf_val & ~0x80) | 0x00);
	RT30xxWriteRFRegister(pAd, RF_R02, (UCHAR)rf_val);
#endif
	return TRUE;
}


/*
	==========================================================================
	Description:

	Reverse RF sleep-mode setup

	==========================================================================
 */
static VOID rt85592_rf_turnon(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN FlgIsInitState)
{
	UCHAR RFValue;

	RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
	RFValue = ((RFValue & ~0x3F) | 0x3F);
	RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

	RT30xxReadRFRegister(pAd, RF_R06, &RFValue);
	//RFValue = 0xE4;
	RFValue = ((RFValue & ~0xC0) | 0xC0); /* vco_ic (VCO bias current control, 11: high) */
	RT30xxWriteRFRegister(pAd, RF_R06, RFValue);

	//RT30xxReadRFRegister(pAd, RF_R02, &RFValue);
	//RFValue = ((RFValue & ~0x80) | 0x80); /* rescal_en (initiate calibration) */
	RT30xxWriteRFRegister(pAd, RF_R02, 0x80);
	
	RT30xxReadRFRegister(pAd, RF_R22, &RFValue);
	RFValue = ((RFValue & ~0xE0) | 0x20); /* cp_ic (reference current control, 001: 0.33 mA) */
	RT30xxWriteRFRegister(pAd, RF_R22, RFValue);

	RT30xxReadRFRegister(pAd, RF_R42, &RFValue);
	RFValue = ((RFValue & ~0x40) | 0x40); /* rx_ctb_en */
	RT30xxWriteRFRegister(pAd, RF_R42, RFValue);

	RT30xxReadRFRegister(pAd, RF_R20, &RFValue);
	RFValue = ((RFValue & ~0x77) | 0x10); /* ldo_rf_vc(0) and ldo_pll_vc(111: +0.05) */
	RT30xxWriteRFRegister(pAd, RF_R20, RFValue);

	RT30xxReadRFRegister(pAd, RF_R03, &RFValue);
	RFValue = ((RFValue & ~0x80) | 0x80); /* vcocal_en (initiate VCO calibration (reset after completion)) */
	RT30xxWriteRFRegister(pAd, RF_R03, RFValue);
}


/*
	==========================================================================
	Description:

	Load RF sleep-mode setup

	==========================================================================
 */
static VOID rt85592_rf_turnoff(RTMP_ADAPTER *pAd)
{
	UCHAR rfreg;

	// TODO: shiang-6590, what should I do for this??
	return;
	
	/* Disabe rf_block */
	RT30xxReadRFRegister(pAd, RF_R01, &rfreg);
	if ((pAd->Antenna.field.TxPath == 2) && (pAd->Antenna.field.RxPath == 2))
		rfreg = ((rfreg & ~0x3F) | 0x3F);
	else
		rfreg = ((rfreg & ~0x0F) | 0x0F); // Enable rf_block_en, pll_en, rx0_en and tx0_en
	RT30xxWriteRFRegister(pAd, RF_R01, rfreg);

	RT30xxReadRFRegister(pAd, RF_R06, &rfreg);
	rfreg = ((rfreg & ~0xC0) | 0xC0); /* vco_ic (VCO bias current control, 00: off) */
	RT30xxWriteRFRegister(pAd, RF_R06, rfreg);

	RT30xxReadRFRegister(pAd, RF_R22, &rfreg);
	rfreg = 0xEA; 	/* Set 0xEA for A band 80BW */
	RT30xxWriteRFRegister(pAd, RF_R22, rfreg);

	RT30xxReadRFRegister(pAd, RF_R42, &rfreg);
	rfreg = 0xD7; /* rx_ctb_en */
	RT30xxWriteRFRegister(pAd, RF_R42, rfreg);

	RT30xxReadRFRegister(pAd, RF_R20, &rfreg);
	rfreg = 0x01; /* ldo_pll_vc and ldo_rf_vc (111: -0.15) */
	RT30xxWriteRFRegister(pAd, RF_R20, rfreg);

	RT30xxReadRFRegister(pAd, RF_R03, &rfreg);
	rfreg = ((rfreg & ~0x80) | 0x80); // vcocal_en (initiate VCO calibration (reset after completion))
	RT30xxWriteRFRegister(pAd, RF_R03, rfreg);
}


VOID RT85592LoadRFNormalModeSetup(RTMP_ADAPTER *pAd)
{
	UCHAR RFValue;
	UINT32 bbp_val;

#if 0
	/* improve power consumption */
	RTMP_BBP_IO_READ32(pAd, CORE_R32, &bbp_val);
	if (pAd->Antenna.field.TxPath == 1)
		bbp_val |= 0x20;		/* turn off tx DAC_1 */
	if (pAd->Antenna.field.RxPath == 1)
		bbp_val &=  (~0x2);	/* turn off tx ADC_1 */
	RTMP_BBP_IO_WRITE32(pAd, CORE_R32, bbp_val);
#endif
	RT30xxReadRFRegister(pAd, RF_R38, &RFValue);
	RFValue = ((RFValue & ~0x20) | 0x00); /* rx_lo1_en (enable RX LO1, 0: LO1 follows TR switch) */
	RT30xxWriteRFRegister(pAd, RF_R38, RFValue);

	RT30xxReadRFRegister(pAd, RF_R39, &RFValue);
	RFValue = ((RFValue & ~0x80) | 0x00); /* rx_lo2_en (enable RX LO2, 0: LO2 follows TR switch) */
	RT30xxWriteRFRegister(pAd, RF_R39, RFValue);

	RTMP_BBP_IO_READ32(pAd, CORE_R1, &bbp_val);
	bbp_val |= 0x40;
	RTMP_BBP_IO_WRITE32(pAd, CORE_R1, bbp_val);

}


static VOID NICInitRT85592RFRegisters(RTMP_ADAPTER *pAd)
{
	INT IdReg;
	UCHAR rf_val;

// TODO: shiang-8592, clear RF_CSR_REG busy bit first and try to work-around it in RT6856 platform
	{
		RF_CSR_CFG_STRUC reg_val = {.word = 0};

		DBGPRINT(RT_DEBUG_TRACE, ("-->%s()\n", __FUNCTION__));
		RTMP_IO_READ32(pAd, RF_CSR_CFG, &reg_val.word);
		if (reg_val.word != 0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Before Init RF, RF_CSR_CFG.word = 0x%x, RF_CSR_CFG.Kick=%d\n",
										reg_val.word, reg_val.non_bank.RF_CSR_KICK));
			RTMP_IO_WRITE32(pAd, RF_CSR_CFG, 0);
			RTMP_IO_READ32(pAd, RF_CSR_CFG, &reg_val.word);
			DBGPRINT(RT_DEBUG_ERROR, ("After write RF_CSR_CFG, read value back=0x%x\n",
										reg_val.word));
		}
	}
// TODO: ---End

	for(IdReg=0; IdReg<rf_init_tb_sz; IdReg++)
	{
		RT30xxWriteRFRegister(pAd, rf_init_tb[IdReg].Register,
				rf_init_tb[IdReg].Value);
		DBGPRINT(RT_DEBUG_INFO, ("RF_R%02d=%02x\n",
					rf_init_tb[IdReg].Register,
					rf_init_tb[IdReg].Value));
	}

	rf_val = (pAd->ChipID == RT85592_E3) ? 0x06 : 0x07;
	RT30xxWriteRFRegister(pAd, RF_R16, rf_val);
	
	rt85592_R_cal(pAd);

	RT85592LoadRFNormalModeSetup(pAd);

	return;
}


/*
========================================================================
Routine Description:
	Initialize specific MAC registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
static VOID NICInitRT85592MacRegisters(RTMP_ADAPTER *pAd)
{
	INT IdReg;
	UINT32 mac_val;

	RTMP_IO_READ32(pAd, CMB_CTRL, &mac_val);
	mac_val &= (~0x00006000);
	RTMP_IO_WRITE32(pAd, CMB_CTRL, mac_val);

	for(IdReg=0; IdReg<RT85592_NUM_MAC_REG_PARMS; IdReg++)
	{
		RTMP_IO_WRITE32(pAd, RT85592_MACRegTable[IdReg].Register,
				RT85592_MACRegTable[IdReg].Value);
	}

	mac_val = (pAd->ChipID == RT85592_E3) ? 0x2 : 0x03;
	RTMP_IO_WRITE32(pAd, TXOP_HLDR_ET, mac_val);
	
#ifdef MCS_LUT_SUPPORT
	RTMP_IO_READ32(pAd, TX_FBK_LIMIT, &mac_val);
	if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT))
		mac_val |= 0x40000;
	else
		mac_val &= (~0x40000);
	RTMP_IO_WRITE32(pAd, TX_FBK_LIMIT, mac_val);
#endif /* MCS_LUT_SUPPORT */

}


/*
========================================================================
Routine Description:
	Initialize specific BBP registers.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
static VOID NICInitRT85592BbpRegisters(RTMP_ADAPTER *pAd)
{
	INT IdReg;

	for(IdReg=0; IdReg < pAd->chipCap.bbpRegTbSize; IdReg++)
	{
		RTMP_BBP_IO_WRITE32(pAd, RT85592_BBPRegTb[IdReg].Register,
				RT85592_BBPRegTb[IdReg].Value);
	}
}


static VOID RT85592_AsicAntennaDefaultReset(
	IN struct _RTMP_ADAPTER	*pAd,
	IN EEPROM_ANTENNA_STRUC *pAntenna)
{	
	pAntenna->word = 0;
	pAntenna->field.RfIcType = 0xf;
	pAntenna->field.TxPath = 2;
	pAntenna->field.RxPath = 2;
}


static VOID RT85592_ChipBBPAdjust(RTMP_ADAPTER *pAd)
{
	static char *ext_str[]={"extNone", "extAbove", "", "extBelow"};
	UINT8 rf_bw, ext_ch;

	DBGPRINT(RT_DEBUG_TRACE, ("-->%s()\n", __FUNCTION__));
#ifdef DOT11_N_SUPPORT
	if (get_ht_cent_ch(pAd, &rf_bw, &ext_ch) == FALSE)
#endif /* DOT11_N_SUPPORT */
	{
		rf_bw = BW_20;
		ext_ch = EXTCHA_NONE;
		pAd->CommonCfg.CentralChannel = pAd->CommonCfg.Channel;
	}

#ifdef DOT11_VHT_AC
	if (WMODE_CAP(pAd->CommonCfg.PhyMode, WMODE_AC) &&
		(pAd->CommonCfg.Channel > 14) &&
		(rf_bw == BW_40) &&
		(pAd->CommonCfg.vht_bw == VHT_BW_80) &&
		(pAd->CommonCfg.vht_cent_ch != pAd->CommonCfg.CentralChannel))
	{
		rf_bw = BW_80;
		pAd->CommonCfg.vht_cent_ch = vht_cent_ch_freq(pAd, pAd->CommonCfg.Channel);
	}

//+++Add by shiang for debug
	DBGPRINT(RT_DEBUG_OFF, ("\trf_bw=%d, ext_ch=%d, PrimCh=%d, HT-CentCh=%d, VHT-CentCh=%d\n",
				rf_bw, ext_ch, pAd->CommonCfg.Channel,
				pAd->CommonCfg.CentralChannel, pAd->CommonCfg.vht_cent_ch));
//---Add by shiang for debug
#endif /* DOT11_VHT_AC */

	bbp_set_bw(pAd, rf_bw);

	/* TX/Rx : control channel setting */
	rtmp_mac_set_ctrlch(pAd, ext_ch);
	bbp_set_ctrlch(pAd, ext_ch);

#ifdef DOT11_N_SUPPORT
		DBGPRINT(RT_DEBUG_TRACE, ("\t%s, ChannelWidth=%d, Channel=%d, ExtChanOffset=%d(%d) \n",
									ext_str[ext_ch],
									pAd->CommonCfg.HtCapability.HtCapInfo.ChannelWidth,
									pAd->CommonCfg.Channel,
									pAd->CommonCfg.RegTransmitSetting.field.EXTCHA,
									pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset));
#endif /* DOT11_N_SUPPORT */
}


INT set_freq_plan(RTMP_ADAPTER *pAd, UCHAR Channel)
{
	INT idx;
	UCHAR RFValue;

	for (idx = 0; idx < RT85592_FreqPlan_Sz; idx++)
	{
		if (Channel == RT85592_FreqPlan_Xtal40M[idx].Channel)
		{
			/* Frequeny plan setting */
			/* 
 			 * R setting 
 			 * R11[1:0]
 			 */
			RT30xxReadRFRegister(pAd, RF_R11, &RFValue);
			RFValue &= (~0x03);
			RFValue |= RT85592_FreqPlan_Xtal40M[idx].R;
			RT30xxWriteRFRegister(pAd, RF_R11, RFValue);

			/*
 			 * N setting
 			 * R9[4], R8[7:0] (RF PLL freq selection) 
 			 */
			//RT30xxReadRFRegister(pAd, RF_R08, &RFValue);
			RFValue = (RT85592_FreqPlan_Xtal40M[idx].N & 0x00ff);
			RT30xxWriteRFRegister(pAd, RF_R08, RFValue);

			RT30xxReadRFRegister(pAd, RF_R09, &RFValue);
			RFValue &= (~0x10);
			RFValue |= ((RT85592_FreqPlan_Xtal40M[idx].N & 0x100) >> 8) << 4;
			RT30xxWriteRFRegister(pAd, RF_R09, RFValue);
			
			/* 
 			 * K setting 
 			 * R7[7], R9[3:0] (RF PLL freq selection)
 			 */
			RT30xxReadRFRegister(pAd, RF_R09, &RFValue);
			RFValue &= (~0x0f);
			RFValue |= (RT85592_FreqPlan_Xtal40M[idx].K & 0x0f);
			RT30xxWriteRFRegister(pAd, RF_R09, RFValue);
			RT30xxReadRFRegister(pAd, RF_R07, &RFValue);
			RFValue &= (~0x80);
			RFValue |= ((RT85592_FreqPlan_Xtal40M[idx].K & 0x10) >> 4) << 7;
			RT30xxWriteRFRegister(pAd, RF_R07, RFValue);

			/* 
 			 * mode setting 
 			 * R9[7:5] (RF PLL freq selection)
 			 * R11[3:2] (RF PLL)
 			 */
			RT30xxReadRFRegister(pAd, RF_R11, &RFValue);
			RFValue &= (~0x0c);
			RFValue |= (RT85592_FreqPlan_Xtal40M[idx].mod & 0x3) << 2;
			RT30xxWriteRFRegister(pAd, RF_R11, RFValue);

			RT30xxReadRFRegister(pAd, RF_R09, &RFValue);
			RFValue &= (~0xe0);
			RFValue |= (RT85592_FreqPlan_Xtal40M[idx].mod >> 2) << 5;
			RT30xxWriteRFRegister(pAd, RF_R09, RFValue);
			break;
		}
	}

	return idx;
}


#ifdef CONFIG_STA_SUPPORT
static UCHAR RT85592_ChipAGCAdjust(RTMP_ADAPTER *pAd, CHAR Rssi, UCHAR old_val)
{
	// TODO: shiang-6590, not implement yet!!!

	return 0;
}
#endif // CONFIG_STA_SUPPORT //


static VOID RT85592_ChipAGCInit(RTMP_ADAPTER *pAd, UCHAR BandWidth)
{
	// TODO: shiang-6590, not implement yet!!!
	/* bbp_set_agc(pAd, R66, RX_CHAIN_ALL); */
}


#define WINDOWS 0

VOID RT85592_ChipSwitchChannel(
	struct _RTMP_ADAPTER *pAd,
	UCHAR Channel,
	BOOLEAN	 bScan)
{
	CHAR TxPwer = 0, TxPwer2 = DEFAULT_RF_TX_POWER;
	UINT idx;
	UCHAR RFValue = 0, PreRFValue;
	UINT32 TxPinCfg, bbp_val, bbp_val2, mac_val;
	UCHAR rf_phy_mode, rf_bw = RF_BW_20;

	if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
		rf_phy_mode = RF_MODE_CCK;
	else
		rf_phy_mode = RF_MODE_OFDM;

	if (pAd->CommonCfg.BBPCurrentBW == BW_80)
		rf_bw = RF_BW_80;
	else if (pAd->CommonCfg.BBPCurrentBW == BW_40)
		rf_bw = RF_BW_40;
	else
		rf_bw = RF_BW_20;
	
#ifdef CONFIG_AP_SUPPORT
#ifdef AP_QLOAD_SUPPORT
	/* clear all statistics count for QBSS Load */
	QBSS_LoadStatusClear(pAd);
#endif /* AP_QLOAD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	/*
		We can't use ChannelList to search channel, since some central channel's txpowr doesn't list
		in ChannelList, so use TxPower array instead.
	*/
	for (idx = 0; idx < MAX_NUM_OF_CHANNELS; idx++)
	{
		if (Channel == pAd->TxPower[idx].Channel)
		{
			TxPwer = pAd->TxPower[idx].Power;
			TxPwer2 = pAd->TxPower[idx].Power2;
			pAd->hw_cfg.cur_ch_pwr[0] = TxPwer;
			pAd->hw_cfg.cur_ch_pwr[1] = TxPwer2;
			break;
		}
	}

	if (idx == MAX_NUM_OF_CHANNELS)
		DBGPRINT(RT_DEBUG_ERROR, ("%s(): Can't find the Channel#%d \n",
					__FUNCTION__, Channel));

	idx = set_freq_plan(pAd, Channel);
	if (idx < RT85592_FreqPlan_Sz)
		DBGPRINT(RT_DEBUG_TRACE,
				("%s(): Switch to Ch#%d(RF=%d, Pwr0=%d, Pwr1=%d, %dT), "
				"N=0x%02X, K=0x%02X, R=0x%02X, BBP_BW=%d\n",
				__FUNCTION__,
				Channel, pAd->RfIcType,
				TxPwer, TxPwer2,
				pAd->Antenna.field.TxPath,
				RT85592_FreqPlan_Xtal40M[idx].N,
				RT85592_FreqPlan_Xtal40M[idx].K,
				RT85592_FreqPlan_Xtal40M[idx].R,
				pAd->CommonCfg.BBPCurrentBW));

	/* Enable RF block */
	RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
	/* Enable rf_block_en, pll_en */
	RFValue |= 0x3;
	RFValue &= (~0x28);
	if (pAd->Antenna.field.TxPath == 2)
		RFValue |= 0x28;	/* Enable tx0_en, tx1_en */
	else if (pAd->Antenna.field.TxPath == 1)
		RFValue |= 0x08;	/* Enable tx0_en */

	RFValue &= (~0x14);
	if (pAd->Antenna.field.RxPath == 2)
		RFValue |= 0x14;	/* Enable rx0_en, rx1_en */
	else if (pAd->Antenna.field.RxPath == 1)
		RFValue |= 0x04;	/* Enable rx0_en */
	RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

	/* RF setting */
	for (idx = 0; idx < rf_adapt_tb_sz; idx++)
	{
		if ((Channel >= rf_adapt_tb[idx].s_ch) &&
			(Channel <= rf_adapt_tb[idx].e_ch) &&
			(rf_phy_mode & rf_adapt_tb[idx].phy) &&
			(rf_bw & rf_adapt_tb[idx].bw)
		)
		{
			RT30xxWriteRFRegister(pAd, rf_adapt_tb[idx].reg, 
					                   rf_adapt_tb[idx].val);
		}
	}

	if (pAd->ChipID == RT85592_E3) {
		for (idx = 0; idx < rf_adapt_tb_e3_sz; idx++)
		{
			if ((Channel >= rf_adapt_tb_e3[idx].s_ch) &&
				(Channel <= rf_adapt_tb_e3[idx].e_ch) &&
				(rf_phy_mode & rf_adapt_tb_e3[idx].phy) &&
				(rf_bw & rf_adapt_tb_e3[idx].bw)
			)
			{
				RT30xxWriteRFRegister(pAd, rf_adapt_tb_e3[idx].reg, 
						                   rf_adapt_tb_e3[idx].val);
			}
		}
	}

	if (pAd->ChipID == RT85592_E3) {
		RFValue = ((rf_bw == RF_BW_20 || rf_bw == RF_BW_10) ? 0x05 : 0x06);
		RT30xxWriteRFRegister(pAd, RF_R16, RFValue);
	}

#ifdef RTMP_TEMPERATURE_COMPENSATION
	if (pAd->chipCap.bTempCompTxALC) {
		RT30xxReadRFRegister(pAd, RF_R27, &RFValue);
		RFValue = (RFValue & 0xf0);	/* Set [3:0] to TssiGain */
		if (Channel <= 14)
			RFValue |= pAd->TxPowerCtrl.TssiGain[IEEE80211_BAND_2G];
		else
			RFValue |= pAd->TxPowerCtrl.TssiGain[IEEE80211_BAND_5G];
		RT30xxWriteRFRegister(pAd, RF_R27, RFValue);
		DBGPRINT(RT_DEBUG_TRACE, ("%s(): TempCompTxALC, set RF_R27 Init TssiGain=0x%x\n",
					__FUNCTION__, RFValue));
	}
#endif /* RTMP_TEMPERATURE_COMPENSATION */

	rf_tr_agc_config(pAd, rf_bw);


	/*@!Release
		R49 CH0 TX power ALC code(RF DAC value) 
		G-band bit<7:6>=1:0, bit<5:0> range from 0x0~0x27
		A-band bit<7:6>=0:1, bit<5:0> range from 0x0~0x2B
		
		R50 CH0 TX power ALC code(RF DAC value) 
		G-band bit<7:6>=1:0, bit<5:0> range from 0x0~0x27
		A-band bit<7:6>=0:1, bit<5:0> range from 0x0~0x2B
	*/
	if (Channel <= 14)
	{
		if (TxPwer > 0x27)
			TxPwer = 0x27;
		RFValue = 0x80 | TxPwer;
		RT30xxWriteRFRegister(pAd, RF_R49, RFValue);

		if (TxPwer2 > 0x27)
			TxPwer2 = 0x27;
		RFValue = 0x80 | TxPwer2;
		RT30xxWriteRFRegister(pAd, RF_R50, RFValue);
	}
	else
	{
		if (TxPwer > 0x2B)
			TxPwer = 0x2B;
		RFValue = 0x40 | TxPwer;
		if (pAd->ChipID == RT85592_E2)
			RFValue &= (~0x40);	
		RT30xxWriteRFRegister(pAd, RF_R49, RFValue);

		if (TxPwer2 > 0x2B)
			TxPwer2 = 0x2B;
		RFValue = 0x40 | TxPwer2;
		if (pAd->ChipID == RT85592_E2) // shiang test code for tx pwr dbg 
        	RFValue &= (~0x40);
		RT30xxWriteRFRegister(pAd, RF_R50, RFValue);
	}
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): Set TxAlc for RF_R49/50=0x%x: 0x%x\n",
				__FUNCTION__, TxPwer, TxPwer2));
	
	RT30xxReadRFRegister(pAd, RF_R17, &RFValue);
	PreRFValue = RFValue;
#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
	if( (pAd->FreqCalibrationCtrl.bEnableFrequencyCalibration == TRUE) && INFRA_ON(pAd))
		RFValue = ((RFValue & ~0x7F) | (pAd->FreqCalibrationCtrl.AdaptiveFreqOffset & 0x7F));
	else
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */
		RFValue = ((RFValue & ~0x7F) | (pAd->RfFreqOffset & 0x7F)); 
	RFValue = min((INT)RFValue, 0x5F);
	if (PreRFValue != RFValue)
		RT30xxWriteRFRegister(pAd, RF_R17, (UCHAR)RFValue);

	RT30xxReadRFRegister(pAd, RF_R03, (PUCHAR)&RFValue);
	RFValue = ((RFValue & ~0x80) | 0x80); /* vcocal_en (initiate VCO calibration (reset after completion)) - It should be at the end of RF configuration. */
	RT30xxWriteRFRegister(pAd, RF_R03, (UCHAR)RFValue);
	pAd->LatchRfRegs.Channel = Channel; /* Channel latch */
	pAd->hw_cfg.lan_gain = GET_LNA_GAIN(pAd);


#ifdef IQ_CAL_SUPPORT
// TODO: shaing-6590, get from windows team, need to revise it!
	/* BBP: IqCompensation */ 
	PerformIQCompensation(pAd, Channel);
#endif /* IQ_CAL_SUPPORT */
// TODO: End---

	//+++ BBP update:0921e3
	if (rf_bw == RF_BW_20)
		bbp_val = 0x00003232;
	else
		bbp_val = 0x00005050;
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R32, bbp_val);
	//---

	if (Channel <= 14)
	{
		for (idx = 0; idx < RT85592_BBP_GBand_Tb_Size; idx++)
			RTMP_BBP_IO_WRITE32(pAd, RT85592_BBPRegTb_GBand[idx].Register,
									RT85592_BBPRegTb_GBand[idx].Value);

		RTMP_BBP_IO_READ32(pAd, CORE_R1, &bbp_val);
		bbp_val &= (~0x80);
		RTMP_BBP_IO_WRITE32(pAd, CORE_R1, bbp_val);
	
		if (pAd->ChipID == RT85592_E3)
		{
			bbp_val = (rf_bw == RF_BW_20) ? 0x804040F9 : 0x804048F9;
			RTMP_BBP_IO_WRITE32(pAd, AGC1_R12, bbp_val);
		}

		bbp_val = (rf_bw == RF_BW_20) ? 0x000C0806 : 0x00090806;
		RTMP_BBP_IO_WRITE32(pAd, AGC1_R28, bbp_val);
		
		/* 5G band selection PIN, bit1 and bit2 are complement */
		rtmp_mac_set_band(pAd, BAND_24G);

		/* Turn off unused PA or LNA when only 1T or 1R */
		TxPinCfg = 0x00050F0A; /* Gary 2007/08/09 0x050A0A */
		if (pAd->Antenna.field.TxPath == 1)
			TxPinCfg &= 0xFFFFFFF3;
		if (pAd->Antenna.field.RxPath == 1)
			TxPinCfg &= 0xFFFFF3FF;
		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);

		if (WINDOWS)
		{
			TX_ATTENUATION_CTRL_STRUC TxAttenuationCtrl = {.word=0};

			// PCIe PHY Transmit attenuation adjustment
			RTMP_IO_READ32(pAd, PCIE_PHY_TX_ATTENUATION_CTRL, &TxAttenuationCtrl.word);
			if (Channel == 14) // Channel #14
			{
				TxAttenuationCtrl.field.PCIE_PHY_TX_ATTEN_EN = 1; // Enable PCIe PHY Tx attenuation
				TxAttenuationCtrl.field.PCIE_PHY_TX_ATTEN_VALUE = 4; // 9/16 full drive level
			}
			else // Channel #1~#13
			{
				TxAttenuationCtrl.field.PCIE_PHY_TX_ATTEN_EN = 0; // Disable PCIe PHY Tx attenuation
				TxAttenuationCtrl.field.PCIE_PHY_TX_ATTEN_VALUE = 0; // n/a
			}

			RTMP_IO_WRITE32(pAd, PCIE_PHY_TX_ATTENUATION_CTRL, TxAttenuationCtrl.word);
		}
	}
	else
	{
		for (idx = 0; idx < RT85592_BBP_ABand_Tb_Size; idx++)
			RTMP_BBP_IO_WRITE32(pAd, RT85592_BBPRegTb_ABand[idx].Register,
									RT85592_BBPRegTb_ABand[idx].Value);

		RTMP_BBP_IO_READ32(pAd, CORE_R1, &bbp_val);
		bbp_val |= 0x80;
		RTMP_BBP_IO_WRITE32(pAd, CORE_R1, bbp_val);

		switch (rf_bw)
		{
			case RF_BW_80:
				bbp_val = 0x00080806;
				if (pAd->ChipID == RT85592_E3)
					bbp_val2 = 0x804040F9;
				else
				bbp_val2 = 0x80404CF9;
				break;
			case RF_BW_40:
				bbp_val = 0x00080806;
				if (pAd->ChipID == RT85592_E3)
					bbp_val2 = 0x804048F9;
				else
					bbp_val2 = 0x804028F9;
				break;
			case RF_BW_20:
			default:
				bbp_val = 0x000C0806;
				bbp_val2 = 0x804040F9;
				break;
		}

		RTMP_BBP_IO_WRITE32(pAd, AGC1_R12, bbp_val2);
		RTMP_BBP_IO_WRITE32(pAd, AGC1_R28, bbp_val);

#ifdef DOT11_VHT_AC
		if (rf_bw == RF_BW_80) {
			UCHAR bbp_ch_idx;

			bbp_ch_idx = vht_prim_ch_idx(Channel, pAd->CommonCfg.Channel);
			if (pAd->ChipID == RT85592_E3)
			{
				bbp_val = 0x0E2034C0;
				bbp_val2 = 0x0E2034C0;
				if (bbp_ch_idx == 1) {
					if (Channel == 155)
						bbp_val = 0x0E182CC0;
				} else if (bbp_ch_idx == 0 || bbp_ch_idx == 3) {
					bbp_val = 0x0E1c30C0;
					bbp_val2 = 0x0E1c30C0;
				}
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val);
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val2);
			} else {
				if (bbp_ch_idx == 0 || bbp_ch_idx == 3)
					bbp_val = 0x0E1C30C0;
				else 
					bbp_val = 0x0E2034C0;
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val);
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, bbp_val);
			}
			RTMP_BBP_IO_WRITE32(pAd, AGC1_R13, 0x3402000E);
			RTMP_BBP_IO_WRITE32(pAd, AGC1_R14, 0x0000323E);
			RTMP_BBP_IO_WRITE32(pAd, RXO_R15, 0xC8321914);
		}
		else
#endif /* DOT11_VHT_AC */
		if (rf_bw == RF_BW_40 || rf_bw == RF_BW_20)
		{
			if (Channel >= 149) {
				if (pAd->ChipID == RT85592_E3)
					bbp_val = 0x0E182CC0;
				else
					bbp_val = 0x0E182FC0;
				RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, bbp_val);
			}
		}

		// 5G band selection PIN, bit1 and bit2 are complement */
		rtmp_mac_set_band(pAd, BAND_5G);

		/* Turn off unused PA or LNA when only 1T or 1R */
		TxPinCfg = 0x00050F05;/* Gary 2007/8/9 0x050505 */
		if (pAd->Antenna.field.TxPath == 1)
			TxPinCfg &= 0xFFFFFFF3;

		if (pAd->Antenna.field.RxPath == 1)
			TxPinCfg &= 0xFFFFF3FF;

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}

#ifdef WFA_VHT_PF
	/* BBP update: 1129e3 pf */
	if (rf_bw == RF_BW_40 || rf_bw == RF_BW_80)
		bbp_val = 0x003A0101;
	else
		bbp_val = 0x003A0808;
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R2, bbp_val);

	/* BBP update: 1019e3 PF only*/
	/* BBP update: 1129e3 pf */
	if (Channel > 14)
	{
		RTMP_BBP_IO_WRITE32(pAd, AGC1_R8, 0x0E1428C0);
		RTMP_BBP_IO_WRITE32(pAd, AGC1_R9, 0x0E1428C0);
	}

	/* BBP update: 1129e3 pf */
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R32, 0x00005050);
	
	/* BBP update: 1023pf */
	bbp_val =  (rf_bw == RF_BW_40) ? 0x30302B30 : 0x27272B30;
	RTMP_BBP_IO_WRITE32(pAd, AGC1_R43, bbp_val);
	
	rt85592_lna_gain_adjust(pAd, 2);
#endif /* WFA_VHT_PF */

#ifdef RTMP_TEMPERATURE_COMPENSATION
	RTMP_BBP_IO_READ32(pAd, AGC1_R8, &bbp_val);
	pAd->rx_temp_base[0] = (bbp_val & 0xff00) >> 8;
	RTMP_BBP_IO_READ32(pAd, AGC1_R9, &bbp_val);
	pAd->rx_temp_base[1] = (bbp_val & 0xff00) >> 8;
	DBGPRINT(RT_DEBUG_TRACE, ("%s(): When Set Channel, AGC1_R8/R9=0x%02x:0x%02x\n",
				__FUNCTION__, pAd->rx_temp_base[0], pAd->rx_temp_base[1]));
#endif /* RTMP_TEMPERATURE_COMPENSATION */

#ifdef IQ_CAL_SUPPORT
	/* IQ Calibration */
	RTMP_CHIP_IQ_CAL(pAd, Channel);
#endif /* IQ_CAL_SUPPORT */

	if (pAd->chipCap.FlgIsVcoReCalMode == VCO_CAL_MODE_3)
	{
		RTMP_IO_READ32(pAd, TX_PIN_CFG, &TxPinCfg);
		TxPinCfg &= 0xFCFFFFF0;
		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);

		RT30xxReadRFRegister(pAd, RF_R03, (PUCHAR)&RFValue);
		RFValue = RFValue | 0x80; /* bit 7 = vcocal_en */
		RT30xxWriteRFRegister(pAd, RF_R03, (UCHAR)RFValue);
		
		RtmpusecDelay(100);

		RTMP_IO_READ32(pAd, TX_PIN_CFG, &TxPinCfg);
		if (pAd->CommonCfg.Channel <= 14)
		{
			if (pAd->Antenna.field.TxPath == 1)
				TxPinCfg |= 0x2;
			else if (pAd->Antenna.field.TxPath == 2)
				TxPinCfg |= 0xA;
			else if (pAd->Antenna.field.TxPath == 3)
				TxPinCfg |= 0x0200000A;
		}
		else
		{
			if (pAd->Antenna.field.TxPath == 1)
				TxPinCfg |= 0x1;
			else if (pAd->Antenna.field.TxPath == 2)
				TxPinCfg |= 0x5;
			else if (pAd->Antenna.field.TxPath == 3)
				TxPinCfg |= 0x01000005;
		}
	}

	/*
	  On 11A, We should delay and wait RF/BBP to be stable
	  and the appropriate time should be 1000 micro seconds
	  2005/06/05 - On 11G, We also need this delay time. Otherwise it's difficult to pass the WHQL.
	*/
	RtmpusecDelay(2000);
}


#ifdef CONFIG_STA_SUPPORT
static VOID RT85592_NetDevNickNameInit(RTMP_ADAPTER *pAd)
{
	snprintf((RTMP_STRING *) pAd->nickname, sizeof(pAd->nickname), "RT85592STA");
}
#endif /* CONFIG_STA_SUPPORT */


INT RT85592_DisableTxRx(RTMP_ADAPTER *pAd, UCHAR Level)
{
	UINT32 MacReg = 0;
	UINT32 MTxCycle;

	DBGPRINT(RT_DEBUG_TRACE, ("----> %s\n", __FUNCTION__));

	if (Level == RTMP_HALT)
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
	}

	StopDmaTx(pAd, Level);

check_tx_again_1:
	/* Check page count in TxQ, */
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		BOOLEAN bFree = TRUE;
		RTMP_IO_READ32(pAd, 0x438, &MacReg);
		if (MacReg != 0)
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa30, &MacReg);
		if (MacReg & 0x000000FF)
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa34, &MacReg);
		if (MacReg & 0xFF00FF00)
			bFree = FALSE;
		if (bFree)
			break;
	}

	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check TxQ page count max\n"));
		RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x438, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x438 = 0x%08x\n", MacReg));

		goto check_tx_again_1;
	}

	RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x0a30 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x0a34 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x438, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x438 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x10E0, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x10E0 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x10E4, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x10E4 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x10E8, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x10E8 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x10EC, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x10EC = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x10F0, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x10F0 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, 0x10F4, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x10F4 = 0x%08x\n", MacReg));

	RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, (">> 0x1200 = 0x%08x\n", MacReg));
	
check_tx_again_2:
	/*
		Check MAC Tx idle
	*/
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacReg);
		if (MacReg & 0x1)
			RtmpusecDelay(50);
		else
			break;
	}
	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Tx idle max(0x%08x)\n", MacReg));
		goto check_tx_again_2;
	}				

	RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x (before disable MAC TX/RX)\n", MacReg));

	RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x (before disable MAC TX/RX)\n", MacReg));

	RTMP_IO_READ32(pAd, 0x0430, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x0430 = 0x%08x (before disable MAC TX/RX)\n", MacReg));

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacReg);
	if (Level == RTMP_HALT)
		MacReg &= ~(0x0000000c); /* Disable MAC TX/RX */
	else
		MacReg &= ~(0x00000008); /* Disable MAC RX */
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacReg);
		
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x1004 = 0x%08x (after disable MAC TX/RX)\n", MacReg));
	
	RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x (after disable MAC TX/RX)\n", MacReg));

	RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x (after disable MAC TX/RX)\n", MacReg));

	RTMP_IO_READ32(pAd, 0x0430, &MacReg);
	DBGPRINT(RT_DEBUG_TRACE, ("0x0430 = 0x%08x (after disable MAC TX/RX)\n", MacReg));

check_rx_again_1:
	/* Check page count in RxQ, */
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		BOOLEAN bFree = TRUE;
		RTMP_IO_READ32(pAd, 0x430, &MacReg);
		if (MacReg & (0x00FF0000))
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa30, &MacReg);
		if (MacReg != 0)
			bFree = FALSE;
		RTMP_IO_READ32(pAd, 0xa34, &MacReg);
		if (MacReg != 0)
			bFree = FALSE;
		if (bFree)
			break;
	}
	
	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check RxQ page count max\n"));
		
		RTMP_IO_READ32(pAd, 0x0a30, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a30 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x0a34, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0a34 = 0x%08x\n", MacReg));

		RTMP_IO_READ32(pAd, 0x0430, &MacReg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x0430 = 0x%08x\n", MacReg));
		goto check_rx_again_1;
	}

check_rx_again_2:
	/* Check MAC Rx idle */
	for (MTxCycle = 0; MTxCycle < 2000; MTxCycle++)
	{
		RTMP_IO_READ32(pAd, MAC_STATUS_CFG, &MacReg);
		if (MacReg & 0x2)
			RtmpusecDelay(50);
		else
			break;
	}
	
	if (MTxCycle >= 2000)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Check MAC Rx idle max(0x%08x)\n", MacReg));
		RTMP_IO_READ32(pAd, 0x208, &MacReg);
		printk("0x208 = 0x%08x (before write 0x244=0x18000000)\n", MacReg);			
		goto check_rx_again_2;
	}

	StopDmaRx(pAd, Level);

	if (Level == RTMP_HALT)
	{
		NICEraseFirmware(pAd);
		
		/* Disable RF/MAC */
		rt85592_wlan_chip_onoff(pAd, FALSE, FALSE);
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("<---- %s\n", __FUNCTION__));

	return 0;
}


/*
========================================================================
Routine Description:
	Initialize RT85592.

Arguments:
	pAd					- WLAN control block pointer

Return Value:
	None

Note:
========================================================================
*/
VOID RT85592_Init(RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;

	DBGPRINT(RT_DEBUG_TRACE, ("-->%s():\n", __FUNCTION__));
	
	pAd->RfIcType = RFIC_UNKNOWN;
	rlt_phy_probe(pAd);

	/* 
		Init chip capabilities
	*/
	pChipCap->hif_type = HIF_RLT;
	pChipCap->MCUType = ANDES;
	pChipCap->asic_caps |= (fASIC_CAP_RX_SC | fASIC_CAP_PMF_ENC | fASIC_CAP_MCS_LUT);
#ifdef CONFIG_CSO_SUPPORT
	pChipCap->asic_caps |= fASIC_CAP_CSO;
#endif /* CONFIG_CSO_SUPPORT */

	pChipCap->phy_caps = (fPHY_CAP_24G | fPHY_CAP_5G);
	pChipCap->phy_caps |= (fPHY_CAP_HT | fPHY_CAP_VHT);

	pChipCap->max_nss = 2;
	pChipCap->max_vht_mcs = VHT_MCS_CAP_7;
	pChipCap->TXWISize = 20;
	pChipCap->RXWISize = 28;
	pChipCap->WPDMABurstSIZE = 3;
	pChipCap->tx_hw_hdr_len = pChipCap->TXWISize + TSO_SIZE;
	pChipCap->rx_hw_hdr_len = RAL_RXINFO_SIZE + pChipCap->RXWISize;

	pChipCap->ba_max_cnt = 31;
	pChipCap->FlgIsHwWapiSup = TRUE;
	// TODO: ++++++following parameters are not finialized yet!!

#ifdef RTMP_FLASH_SUPPORT
	pChipCap->EEPROM_DEFAULT_BIN = RT85592_EeBuffer;
#endif /* RTMP_FLASH_SUPPORT */
	pChipCap->SnrFormula = SNR_FORMULA2;
	pChipCap->VcoPeriod = 10;
	pChipCap->FlgIsVcoReCalMode = VCO_CAL_MODE_2; /*VCO_CAL_MODE_3;*/ /* VCO_CAL_MODE_2; */
	pChipCap->FlgIsHwAntennaDiversitySup = FALSE;
#ifdef STREAM_MODE_SUPPORT
	pChipCap->FlgHwStreamMode = FALSE;
#endif /* STREAM_MODE_SUPPORT */
#ifdef TXBF_SUPPORT
	pChipCap->FlgHwTxBfCap = FALSE;
#endif /* TXBF_SUPPORT */
#ifdef FIFO_EXT_SUPPORT
	pChipCap->FlgHwFifoExtCap = TRUE;
#endif /* FIFO_EXT_SUPPORT */
	
	pChipCap->RfReg17WtMethod = RF_REG_WT_METHOD_STEP_ON;

	pChipCap->MaxNumOfRfId = 63;
	pChipCap->pRFRegTable = rf_init_tb;

	pChipCap->MaxNumOfBbpId = 200;	
	pChipCap->pBBPRegTable = (REG_PAIR *)RT85592_BBPRegTb;
	pChipCap->bbpRegTbSize = (sizeof(RT85592_BBPRegTb) / sizeof(RTMP_REG_PAIR));

#ifdef NEW_MBSSID_MODE
	pChipCap->MBSSIDMode = MBSSID_MODE1;
#else
	pChipCap->MBSSIDMode = MBSSID_MODE0;
#endif /* NEW_MBSSID_MODE */


#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_FREQ_CALIBRATION_SUPPORT
	// TODO: shiang-6590, fix me for FrqCal in STA mode!!
	/* Frequence Calibration */
	pChipCap->FreqCalibrationSupport = FALSE;
	pChipCap->FreqCalInitMode = 0;
	pChipCap->FreqCalMode = 0;
	pChipCap->RxWIFrqOffset = 0;
#endif /* RTMP_FREQ_CALIBRATION_SUPPORT */
#endif /* CONFIG_STA_SUPPORT */

	pChipCap->AMPDUFactor = 3;

	RTMP_DRS_ALG_INIT(pAd, RATE_ALG_GRP);
		
	/*
		Following function configure beacon related parameters in pChipCap
			FlgIsSupSpecBcnBuf / BcnMaxHwNum / 
			WcidHwRsvNum / BcnMaxHwSize / BcnBase[]
	*/
#if 0
#ifdef SPECIFIC_BCN_BUF_SUPPORT
	RtmpChipBcnSpecInit(pAd);
#else
	RtmpChipBcnInit(pAd);
	for (idx = 0; idx < pChipCap->BcnMaxHwNum; idx++)
		pChipCap->BcnBase[idx] += 0x4000;
#endif /* SPECIFIC_BCN_BUF_SUPPORT */
#else
	rlt_bcn_buf_init(pAd);
#endif

	// TODO: ------Upper parameters are not finialized yet!!


	/*
		init operator
	*/
	// TODO: ++++++following parameters are not finialized yet!!
	/* BBP adjust */
	pChipOps->ChipBBPAdjust = RT85592_ChipBBPAdjust;
#ifdef CONFIG_STA_SUPPORT
	pChipOps->ChipAGCAdjust = NULL; /* RT85592_ChipAGCAdjust; */
#endif /* CONFIG_STA_SUPPORT */

	/* Channel */
	pChipOps->ChipSwitchChannel = RT85592_ChipSwitchChannel;
	pChipOps->ChipAGCInit = RT85592_ChipAGCInit;

	pChipOps->AsicMacInit = NICInitRT85592MacRegisters;
	pChipOps->AsicBbpInit = NICInitRT85592BbpRegisters;
	pChipOps->AsicRfInit = NICInitRT85592RFRegisters;
	pChipOps->AsicRfTurnOn = NULL;

	pChipOps->AsicHaltAction = NULL;
	pChipOps->AsicRfTurnOff = rt85592_rf_turnoff;
	pChipOps->AsicReverseRfFromSleepMode = rt85592_rf_turnon;
	
	pChipOps->AsicResetBbpAgent = NULL;
	
	/* MAC */

	/* EEPROM */
	pChipOps->NICInitAsicFromEEPROM = NULL;
	
	/* Antenna */
	pChipOps->AsicAntennaDefaultReset = RT85592_AsicAntennaDefaultReset;

	/* TX ALC */
	pChipOps->InitDesiredTSSITable = NULL;
 	pChipOps->ATETssiCalibration = NULL;
	pChipOps->ATETssiCalibrationExtend = NULL;
	pChipOps->ATEReadExternalTSSI = NULL;
	pChipOps->TSSIRatio = NULL;
	
#ifdef RTMP_TEMPERATURE_COMPENSATION
	pChipCap->bTempCompTxALC = TRUE;
	pChipCap->TxAlcTxPowerUpperBound_2G = 69;
	pChipCap->TxPowerTuningTable_2G = RT85592_TxPowerTuningTable_2G;
#ifdef A_BAND_SUPPORT
	pChipCap->TxAlcTxPowerUpperBound_5G = 73;
	pChipCap->TxPowerTuningTable_5G = RT85592_TxPowerTuningTable_5G;
#endif /* A_BAND_SUPPORT */
	pChipOps->AsicTxAlcGetAutoAgcOffset = AsicGetAutoAgcOffsetForTemperatureSensor;
#endif /* RTMP_TEMPERATURE_COMPENSATION */
	pChipOps->AsicGetTxPowerOffset = AsicGetTxPowerOffset;
	pChipOps->AsicExtraPowerOverMAC = AsicExtraPowerOverMAC;

	/* Others */
	pChipOps->DisableTxRx = RT85592_DisableTxRx;
#ifdef CONFIG_STA_SUPPORT
	pChipOps->NetDevNickNameInit = RT85592_NetDevNickNameInit;
#endif /* CONFIG_STA_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
	pAd->chipCap.carrier_func = TONE_RADAR_V2;
	pChipOps->ToneRadarProgram = ToneRadarProgram_v2;
#endif /* CARRIER_DETECTION_SUPPORT */

	/* Chip tuning */
	pChipOps->RxSensitivityTuning = NULL;

/* Following callback functions already initiailized in RtmpChipOpsEepromHook( ) */
	/*  Calibration access related callback functions */
/*
	int (*eeinit)(struct _RTMP_ADAPTER *pAd);
	int (*eeread)(struct _RTMP_ADAPTER *pAd, USHORT offset, PUSHORT pValue);
	int (*eewrite)(struct _RTMP_ADAPTER *pAd, USHORT offset, USHORT value);
*/
	/* MCU related callback functions */
/*
	int (*loadFirmware)(struct _RTMP_ADAPTER *pAd);
	int (*eraseFirmware)(struct _RTMP_ADAPTER *pAd);
	int (*sendCommandToMcu)(struct _RTMP_ADAPTER *pAd, UCHAR cmd, UCHAR token, UCHAR arg0, UCHAR arg1, BOOLEAN FlgIsNeedLocked);
*/

/* 
	Following callback functions already initiailized in RtmpChipOpsHook() 
	1. Power save related
*/
#ifdef GREENAP_SUPPORT
	pChipOps->EnableAPMIMOPS = EnableAPMIMOPSv2;
	pChipOps->DisableAPMIMOPS = DisableAPMIMOPSv2;
#endif /* GREENAP_SUPPORT */
	// TODO: ------Upper parameters are not finialized yet!!
}

#endif /* RT8592 */

