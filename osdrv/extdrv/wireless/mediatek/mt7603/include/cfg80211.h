/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************/

/****************************************************************************

	Abstract:

	All MAC80211/CFG80211 Related Structure & Definition.

***************************************************************************/
#ifndef __CFG80211_H__
#define __CFG80211_H__

#ifdef RT_CFG80211_SUPPORT

#include <linux/ieee80211.h>

#ifdef CFG_CFG80211_VERSION
#define CFG80211_VERSION_CODE CFG_CFG80211_VERSION
#else
#define CFG80211_VERSION_CODE LINUX_VERSION_CODE
#endif

#if KERNEL_VERSION(4, 7, 0) <= CFG80211_VERSION_CODE
/**
 * enum nl80211_band - Frequency band
 * @NL80211_BAND_2GHZ: 2.4 GHz ISM band
 * @NL80211_BAND_5GHZ: around 5 GHz band (4.9 - 5.7 GHz)
 * @NL80211_BAND_60GHZ: around 60 GHz band (58.32 - 64.80 GHz)
 * @NUM_NL80211_BANDS: number of bands, avoid using this in userspace
 *	 since newer kernel versions may support more bands
 */
#define KAL_BAND_2GHZ NL80211_BAND_2GHZ
#define KAL_BAND_5GHZ NL80211_BAND_5GHZ
#define KAL_NUM_BANDS NUM_NL80211_BANDS
#else
#define KAL_BAND_2GHZ IEEE80211_BAND_2GHZ
#define KAL_BAND_5GHZ IEEE80211_BAND_5GHZ
#define KAL_NUM_BANDS IEEE80211_NUM_BANDS
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
#define random32() prandom_u32()
#endif

#ifdef CFG80211_BUILD_CHANNEL_LIST
#define RT_REG_RULE(regr, start, end, bw, gain, eirp, reg_flags) \
        regr.freq_range.start_freq_khz = MHZ_TO_KHZ(start);     \
        regr.freq_range.end_freq_khz = MHZ_TO_KHZ(end); \
        regr.freq_range.max_bandwidth_khz = MHZ_TO_KHZ(bw);     \
        regr.power_rule.max_antenna_gain = DBI_TO_MBI(gain);\
        regr.power_rule.max_eirp = DBM_TO_MBM(eirp);    \
        regr.flags = reg_flags;
#endif /* CFG80211_BUILD_CHANNEL_LIST */

typedef enum _NDIS_HOSTAPD_STATUS {
	Hostapd_Diable = 0,
	Hostapd_EXT,
	Hostapd_CFG
} NDIS_HOSTAPD_STATUS, *PNDIS_HOSTAPD_STATUS;


typedef struct __CFG80211_CB {

	/* we can change channel/rate information on the fly so we backup them */
	struct ieee80211_supported_band Cfg80211_bands[KAL_NUM_BANDS];
	struct ieee80211_channel *pCfg80211_Channels;
	struct ieee80211_rate *pCfg80211_Rates;

	/* used in wiphy_unregister */
	struct wireless_dev *pCfg80211_Wdev;

	/* used in scan end */
	struct cfg80211_scan_request *pCfg80211_ScanReq;

	/* monitor filter */
	UINT32 MonFilterFlag;

	/* channel information */
	struct ieee80211_channel ChanInfo[MAX_NUM_OF_CHANNELS];

	/* to protect scan status */
	spinlock_t scan_notify_lock;
		
} CFG80211_CB;




/*
========================================================================
Routine Description:
	Register MAC80211 Module.

Arguments:
	pAd				- WLAN control block pointer
	pDev			- Generic device interface
	pNetDev			- Network device

Return Value:
	NONE

Note:
	pDev != pNetDev
	#define SET_NETDEV_DEV(net, pdev)	((net)->dev.parent = (pdev))

	Can not use pNetDev to replace pDev; Or kernel panic.
========================================================================
*/
BOOLEAN CFG80211_Register(
	VOID						*pAd,
	struct device				*pDev,
	struct net_device			*pNetDev);

/**
 * kalCfg80211ScanDone - abstraction of cfg80211_scan_done
 *
 * @request: the corresponding scan request (sanity checked by callers!)
 * @aborted: set to true if the scan was aborted for any reason,
 *	userspace will be notified of that
 *
 * Since linux-4.8.y the 2nd parameter is changed from bool to
 * struct cfg80211_scan_info, but we don't use all fields yet.
 */
#if KERNEL_VERSION(4, 8, 0) <= CFG80211_VERSION_CODE
static inline void kalCfg80211ScanDone(struct cfg80211_scan_request *request,
				       bool aborted)
{
	struct cfg80211_scan_info info = { .aborted = aborted };

	cfg80211_scan_done(request, &info);
}
#else
static inline void kalCfg80211ScanDone(struct cfg80211_scan_request *request,
				       bool aborted)
{
	cfg80211_scan_done(request, aborted);
}
#endif

#endif /* RT_CFG80211_SUPPORT */

#endif /* __CFG80211_H__ */

/* End of cfg80211.h */
