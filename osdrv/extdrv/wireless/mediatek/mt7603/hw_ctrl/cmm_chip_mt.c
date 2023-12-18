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
	cmm_asic.c

	Abstract:
	Functions used to communicate with ASIC
	
	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
*/


#include "rt_config.h"


#if 0
INT mt_asic_top_init(RTMP_ADAPTER *pAd)
{
	DBGPRINT(RT_DEBUG_OFF, ("%s()-->:HIF_TYPE=%d\n", __FUNCTION__, pAd->chipCap.hif_type));

	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
#ifdef MT7603_FPGA
		UINT32 mac_val;

		// enable MAC circuit
		RTMP_IO_READ32(pAd, 0x2108, &mac_val);
		mac_val &= (~0x7ff0);
		RTMP_IO_WRITE32(pAd, 0x2108, mac_val);

#ifndef RTMP_MAC_USB
		mac_val = 0x3e013;
		RTMP_IO_WRITE32(pAd, 0x2d004, mac_val);
#endif

		// PSE/MAC Clock setting, Register work-around for FPGA, now it's take cared by ROM code!
		//RTMP_IO_READ32(pAd, 0x1100, &mac_val);
		//mac_val = 0x26110080;
		//RTMP_IO_WRITE32(pAd, 0x1100, mac_val);

		// For FPGA, adjust Clock setting, now 100ms setting will mapping to 400ms in real time
		/* Len's setting,
			MCU run in 10MHz, LMAC run in 1.25MHz
			so GPT timer will have 8 times differentiation with free run timer
			=>LMAC time will 8 times slower than MCU time
			=> host driver set beacon interval 100ms, then real beacon interval will be 800ms
		*/
		RTMP_IO_WRITE32(pAd, 0x24088, 0x900); // Set 40MHz Clock
		RTMP_IO_WRITE32(pAd, 0x2d034, 0x64180003);	// Set 32k clock, this clock is used for lower power.

		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &mac_val);
		//mac_val |= 0xb0; // bit 7/5~4 => 1
		mac_val = 0x52000850;
		RTMP_IO_WRITE32(pAd, MT_WPDMA_GLO_CFG, mac_val);
#endif /* MT7603_FPGA */
	}

	return TRUE;
}
#else
INT mt_asic_top_init(RTMP_ADAPTER *pAd)
{
#if defined(MT7603_FPGA) || defined(MT7628_FPGA)
	UINT32 mac_val;
		
	// TODO: shiang-7603
	// PSE/MAC Clock setting, Register work-around for FPGA, now it's take cared by ROM code!
	//RTMP_IO_READ32(pAd, 0x1100, &mac_val);
	//mac_val = 0x26110080;
	//RTMP_IO_WRITE32(pAd, 0x1100, mac_val);

	// For FPGA, adjust Clock setting, now 100ms setting will mapping to 400ms in real time
	/* Len's setting,
		MCU run in 10MHz, LMAC run in 1.25MHz
		so GPT timer will have 8 times differentiation with free run timer
		=>LMAC time will 8 times slower than MCU time
		=> host driver set beacon interval 100ms, then real beacon interval will be 800ms
	*/
#ifdef MT7628_FPGA
	// enable MAC circuit
	RTMP_IO_READ32(pAd, 0x2108, &mac_val);
	mac_val &= (~0x7ff0);
	RTMP_IO_WRITE32(pAd, 0x2108, mac_val);

	mac_val = 0x3e013;
	RTMP_IO_WRITE32(pAd, 0x2d004, mac_val);
#endif /* MT7628_FPGA */

	RTMP_IO_WRITE32(pAd, 0x24088, 0x900); // Set 40MHz Clock
	RTMP_IO_WRITE32(pAd, 0x2d034, 0x64180003);	// Set 32k clock, this clock is used for lower power.
#endif /* MT7603_FPGA */

	return TRUE;
}
#endif


INT mt_hif_sys_init(RTMP_ADAPTER *pAd)
{

#ifdef RTMP_MAC_PCI
	{
		UINT32 mac_val;
	
		RTMP_IO_READ32(pAd, MT_WPDMA_GLO_CFG, &mac_val);
		//mac_val |= 0xb0; // bit 7/5~4 => 1
		mac_val = 0x52000850;
		RTMP_IO_WRITE32(pAd, MT_WPDMA_GLO_CFG, mac_val);
	}
#endif /* RTMP_MAC_PCI */

#ifdef RTMP_MAC_USB
	{
       	USB_DMA_CFG_STRUC	UsbCfg;
            	
       	/* USB1.1 do not use bulk in aggregation */
		if (pAd->BulkInMaxPacketSize >= 512)
			UsbCfg.field.RxBulkAggEn = 1;
		else 
			UsbCfg.field.RxBulkAggEn = 0;

		/* for last packet, PBF might use more than limited, so minus 2 to prevent from error */
		UsbCfg.field.RxBulkAggLmt = (MAX_RXBULK_SIZE /1024) - 3;
		UsbCfg.field.RxBulkAggTOut = 0x80; 

		UsbCfg.field.RxBulkEn = 1;
		UsbCfg.field.TxBulkEn = 1;

		UsbCfg.word &= ~UDMA_WLCFG_0_RX_MPSZ_PAD0_MASK; //bit18
		UsbCfg.word |= UDMA_WLCFG_0_RX_MPSZ_PAD0(1);
				
		RTMP_IO_WRITE32(pAd, UDMA_WLCFG_0, UsbCfg.word);
	}
#endif /* RTMP_MAC_USB */

#ifdef RTMP_MAC_SDIO

		UINT32 Value;
		UINT32 counter=0;

		RTMP_SDIO_WRITE32(pAd, WHLPCR, W_INT_EN_CLR);
		RTMP_SDIO_READ32(pAd, WHLPCR, &Value);

		RTMP_SDIO_READ32(pAd, WCIR, &Value);
   
		if(GET_POR_INDICATOR(Value)) {// POR
			RTMP_SDIO_WRITE32(pAd, WCIR, POR_INDICATOR);
			RTMP_SDIO_READ32(pAd, WCIR, &Value);
			DBGPRINT(RT_DEBUG_OFF, ("%s(): MCR_WCIR: Value:%x\n", __FUNCTION__, Value));
                }
		RtmpOsMsDelay(100);	

#if 1
//		RTMP_SDIO_WRITE32(pAd, WHIER, 0x0);
		RTMP_SDIO_WRITE32(pAd, WHLPCR, W_INT_EN_CLR);
   
#endif
		//Poll W_FUNC for FW own back
		DBGPRINT(RT_DEBUG_OFF, ("%s(): Request FW-Own back\n",__FUNCTION__));

		RTMP_SDIO_READ32(pAd, WHLPCR, &Value);
		while(!GET_W_FW_OWN_REQ_SET(Value)) {

			RTMP_SDIO_WRITE32(pAd, WHLPCR, W_FW_OWN_REQ_CLR);
			RTMP_SDIO_READ32(pAd, WHLPCR, &Value);
			DBGPRINT(RT_DEBUG_OFF, ("%s(): Request FW-Own processing: %x\n",__FUNCTION__,Value));
			counter++;
			if(counter >50){
				DBGPRINT(RT_DEBUG_ERROR, ("%s:  FW-Own back Faiure\n",__FUNCTION__));
				break;
			}
		}

		RTMP_SDIO_WRITE32(pAd, WHLPCR, W_INT_EN_CLR);		
		RTMP_SDIO_READ32(pAd, WHLPCR, &Value);
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MCR_WHLPCR: Value:%x\n", __FUNCTION__, Value));
		
		RTMP_SDIO_WRITE32(pAd, WHIER, 0x46);
		RTMP_SDIO_READ32(pAd, WHIER, &Value);
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MCR_WHIER: Value:%x\n", __FUNCTION__, Value));
		RTMP_SDIO_READ32(pAd, WHISR, &Value);
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MCR_WHISR: Value:%x\n", __FUNCTION__, Value));
		RTMP_SDIO_READ32(pAd, WCIR, &Value);
		DBGPRINT(RT_DEBUG_OFF, ("%s(): MCR_WCIR: Value:%x\n", __FUNCTION__, Value));

   
#endif

	return TRUE;
}

