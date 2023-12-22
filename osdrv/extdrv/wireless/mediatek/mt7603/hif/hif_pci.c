
#include "rt_config.h"


#ifdef RTMP_MAC_PCI
INT NICInitPwrPinCfg(RTMP_ADAPTER *pAd)
{
#if defined(RTMP_MAC) || defined(RLT_MAC)
	UINT32 mac_val = 0;
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */
	// TODO: shiang-7603
	if (pAd->chipCap.hif_type == HIF_MT) {
		DBGPRINT(RT_DEBUG_OFF, ("%s(%d): Not support for HIF_MT yet!\n",
							__FUNCTION__, __LINE__));

		return FALSE;
	}

#if defined(RTMP_MAC) || defined(RLT_MAC)
	/*@!Release
		For MT76x0 series, 
		PWR_PIN_CFG[2:0]: obsolete, no function
		Don't need to change PWR_PIN_CFG here.
	*/
#ifdef RT8592
	// TODO:shiang-6590, why we don't need to change PWR_PIN_CFG as 1 here? Windows did it!!
	if (IS_RT8592(pAd))
		mac_val = 0x1;
	else
#endif /* RT8592 */
#if defined(MT76x0) || defined(MT76x2) || defined(MT7601)
	if (IS_MT76x0(pAd) || IS_MT76x2(pAd) || IS_MT7601(pAd)) {
		/*
			PWR_PIN_CFG[2:0]: obsolete, no function
			Don't need to change PWR_PIN_CFG here.
		*/
		mac_val = 0x0;
	}
	else
#endif /* defined(MT76x0) || defined(MT76x2) || defined(MT7601) */
		mac_val = 0x3;	/* To fix driver disable/enable hang issue when radio off*/
	RTMP_IO_WRITE32(pAd, PWR_PIN_CFG, mac_val);
#endif /* defined(RTMP_MAC) || defined(RLT_MAC) */

	return TRUE;
}



#endif /* RTMP_MAC_PCI */


