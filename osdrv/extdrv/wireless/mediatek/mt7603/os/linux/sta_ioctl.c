/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    sta_ioctl.c

    Abstract:
    IOCTL related subroutines

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Rory Chen   01-03-2003    created
	Rory Chen   02-14-2005    modify to support RT61
*/

#define RTMP_MODULE_OS

#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"
#include "rt_config.h"
#if 0
#ifdef DOT11R_FT_SUPPORT
#include	"ft.h"
#endif /* DOT11R_FT_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
#include "tdls_cmm.h"
#endif /* DOT11Z_TDLS_SUPPORT */
#endif

#ifdef DBG
extern ULONG RTDebugLevel;
extern ULONG RTDebugFunc;
#endif

#define NR_WEP_KEYS 				4
#define WEP_SMALL_KEY_LEN 			(40/8)
#define WEP_LARGE_KEY_LEN 			(104/8)

#define GROUP_KEY_NO                4

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#define IWE_STREAM_ADD_EVENT(_A, _B, _C, _D, _E)		iwe_stream_add_event(_A, _B, _C, _D, _E)
#define IWE_STREAM_ADD_POINT(_A, _B, _C, _D, _E)		iwe_stream_add_point(_A, _B, _C, _D, _E)
#define IWE_STREAM_ADD_VALUE(_A, _B, _C, _D, _E, _F)	iwe_stream_add_value(_A, _B, _C, _D, _E, _F)
#else
#define IWE_STREAM_ADD_EVENT(_A, _B, _C, _D, _E)		iwe_stream_add_event(_B, _C, _D, _E)
#define IWE_STREAM_ADD_POINT(_A, _B, _C, _D, _E)		iwe_stream_add_point(_B, _C, _D, _E)
#define IWE_STREAM_ADD_VALUE(_A, _B, _C, _D, _E, _F)	iwe_stream_add_value(_B, _C, _D, _E, _F)
#endif

extern UCHAR    CipherWpa2Template[];

typedef struct GNU_PACKED _RT_VERSION_INFO{
    UCHAR       DriverVersionW;
    UCHAR       DriverVersionX;
    UCHAR       DriverVersionY;
    UCHAR       DriverVersionZ;
    UINT        DriverBuildYear;
    UINT        DriverBuildMonth;
    UINT        DriverBuildDay;
} RT_VERSION_INFO, *PRT_VERSION_INFO;

struct iw_priv_args privtab[] = {
{ RTPRIV_IOCTL_SET, 
  IW_PRIV_TYPE_CHAR | 1024, 0,
  "set"},

{ RTPRIV_IOCTL_SHOW, IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
  ""},
/* --- sub-ioctls definitions --- */   
#ifdef MAT_SUPPORT
	{ SHOW_IPV4_MAT_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "ipv4_matinfo" },
	{ SHOW_IPV6_MAT_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "ipv6_matinfo" },
	{ SHOW_ETH_CLONE_MAC,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "cloneMAC" },
#endif /* MAT_SUPPORT */
    { SHOW_CONN_STATUS,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "connStatus" },
	{ SHOW_DRVIER_VERION,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "driverVer" },
    { SHOW_BA_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "bainfo" },
	{ SHOW_DESC_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "descinfo" },
#ifdef RELEASE_EXCLUDE
#ifdef RTMP_MAC_USB
	{ SHOW_RXBULK_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "rxbulk" },
	{ SHOW_TXBULK_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "txbulk" },
#endif /* RTMP_MAC_USB */
#endif /* RELEASE_EXCLUDE */
    { RAIO_OFF,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "radio_off" },
	{ RAIO_ON,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "radio_on" },
#ifdef MESH_SUPPORT
	{ SHOW_MESH_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "meshinfo" },
	{ SHOW_NEIGHINFO_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "neighinfo" },
	{ SHOW_MESH_ROUTE_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "meshrouteinfo" },
	{ SHOW_MESH_ENTRY_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "meshentryinfo" },
	{ SHOW_MULPATH_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "multipathinfo" },
	{ SHOW_MCAST_AGEOUT_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mcastageoutinfo" },
	{ SHOW_MESH_PKTSIG_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "pktsiginfo" },
	{ SHOW_MESH_PROXY_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "meshproxyinfo" },
#endif /* MESH_SUPPORT */
#ifdef QOS_DLS_SUPPORT
	{ SHOW_DLS_ENTRY_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "dlsentryinfo" },
#endif /* QOS_DLS_SUPPORT */
	{ SHOW_CFG_VALUE,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "show" },
	{ SHOW_ADHOC_ENTRY_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "adhocEntry" },
#ifdef WMM_ACM_SUPPORT
    { SHOW_ACM_BADNWIDTH,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "acmDisplay" },
#endif /* WMM_ACM_SUPPORT */
#if defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT)
	{ SHOW_TDLS_ENTRY_INFO,
	  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "tdlsentryinfo" },
#endif /* defined(DOT11Z_TDLS_SUPPORT) || defined(CFG_TDLS_SUPPORT) */
	{SHOW_DEV_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "devinfo"},
	{SHOW_STA_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "stainfo"},
	{SHOW_TR_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "trinfo"},
	{SHOW_SYS_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "sysinfo"},
	{SHOW_PWR_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "pwrinfo"},
	{SHOW_DIAGNOSE_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "diaginfo"},
#ifdef MT_MAC
	{SHOW_WTBL_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "wtbl"},
	{SHOW_MIB_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "mibinfo"},
#endif /* MT_MAC */
/* --- sub-ioctls relations --- */

	{SHOW_TEMP_INFO,
	IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, "temp"},
#ifdef DBG
{ RTPRIV_IOCTL_BBP,
  IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
  "bbp"},
{ RTPRIV_IOCTL_MAC,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "mac"},  
#ifdef RTMP_RF_RW_SUPPORT
{ RTPRIV_IOCTL_RF,
  IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
  "rf"},
#endif /* RTMP_RF_RW_SUPPORT */
{ RTPRIV_IOCTL_E2P,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "e2p"},
#endif  /* DBG */

{ RTPRIV_IOCTL_STATISTICS,
  0, IW_PRIV_TYPE_CHAR | IW_PRIV_SIZE_MASK,
  "stat"}, 
{ RTPRIV_IOCTL_GSITESURVEY,
  0, IW_PRIV_TYPE_CHAR | 1024,
  "get_site_survey"},

#ifdef WSC_STA_SUPPORT
{ RTPRIV_IOCTL_SET_WSC_PROFILE_U32_ITEM,
  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "" },
{ RTPRIV_IOCTL_SET_WSC_PROFILE_U32_ITEM,
  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 0, 0, "" },
{ RTPRIV_IOCTL_SET_WSC_PROFILE_STRING_ITEM,
  IW_PRIV_TYPE_CHAR | 128, 0, "" },
/* --- sub-ioctls definitions --- */    
	{ WSC_CREDENTIAL_COUNT,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wsc_cred_count" },
	{ WSC_CREDENTIAL_SSID,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_cred_ssid" },
	{ WSC_CREDENTIAL_AUTH_MODE,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_cred_auth" },
	{ WSC_CREDENTIAL_ENCR_TYPE,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_cred_encr" },
	{ WSC_CREDENTIAL_KEY_INDEX,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_cred_keyIdx" },
	{ WSC_CREDENTIAL_KEY,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_cred_key" },
	{ WSC_CREDENTIAL_MAC,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_cred_mac" },	
	{ WSC_SET_DRIVER_CONNECT_BY_CREDENTIAL_IDX,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wsc_conn_by_idx" },
	{ WSC_SET_DRIVER_AUTO_CONNECT,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wsc_auto_conn" },
	{ WSC_SET_CONF_MODE,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wsc_conf_mode" },
	{ WSC_SET_MODE,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wsc_mode" },
	{ WSC_SET_PIN,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_pin" },
	{ WSC_SET_SSID,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_ssid" },
	{ WSC_SET_BSSID,
	  IW_PRIV_TYPE_CHAR | 128, 0, "wsc_bssid" },
	{ WSC_START,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 0, 0, "wsc_start" },
	{ WSC_STOP,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 0, 0, "wsc_stop" },
    { WSC_GEN_PIN_CODE,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 0, 0, "wsc_gen_pincode" },
	{ WSC_AP_BAND,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wsc_ap_band" },
/* --- sub-ioctls relations --- */
#endif /* WSC_STA_SUPPORT */
};

extern INT32 ralinkrate[];
extern UINT32 RT_RateSize;


/*
This is required for LinEX2004/kernel2.6.7 to provide iwlist scanning function
*/
int rt_ioctl_giwname(struct net_device *dev,
		   struct iw_request_info *info,
		   char *name, char *extra)
{
	strncpy(name, "Ralink STA", IFNAMSIZ);
	return 0;
}

int rt_ioctl_siwfreq(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_freq *freq, char *extra)
{
	VOID *pAd = NULL;
/*	int 	chan = -1; */
	RT_CMD_STA_IOCTL_FREQ IoctlFreq, *pIoctlFreq = &IoctlFreq;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;   
    }


	if (freq->e > 1)
		return -EINVAL;
    
	pIoctlFreq->m = freq->m;
	pIoctlFreq->e = freq->e;

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWFREQ, 0,
							pIoctlFreq, 0, RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
		return -EINVAL;

	return 0;
}


int rt_ioctl_giwfreq(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_freq *freq, char *extra)
{
	VOID *pAd = NULL;
	ULONG	m = 2412000;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
		return -ENETDOWN;   
	}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWFREQ, 0,
						&m, RT_DEV_PRIV_FLAGS_GET(dev), RT_DEV_PRIV_FLAGS_GET(dev));

	freq->m = m * 100;
	freq->e = 1;
	freq->i = 0;
	
	return 0;
}


int rt_ioctl_siwmode(struct net_device *dev,
		   struct iw_request_info *info,
		   __u32 *mode, char *extra)
{
	VOID *pAd = NULL;
	LONG Mode;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
    	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
       	return -ENETDOWN;   
    }

	if (*mode == IW_MODE_ADHOC)
		Mode = RTMP_CMD_STA_MODE_ADHOC;
	else if (*mode == IW_MODE_INFRA)
		Mode = RTMP_CMD_STA_MODE_INFRA;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,4,20))
	else if (*mode == IW_MODE_MONITOR)
		Mode = RTMP_CMD_STA_MODE_MONITOR;
#endif /* LINUX_VERSION_CODE */
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("===>rt_ioctl_siwmode::SIOCSIWMODE (unknown %d)\n", *mode));
		return -EINVAL;
	}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWMODE, 0,
							NULL, Mode, RT_DEV_PRIV_FLAGS_GET(dev));
	return 0;
}


int rt_ioctl_giwmode(struct net_device *dev,
		   struct iw_request_info *info,
		   __u32 *mode, char *extra)
{
	VOID *pAd = NULL;
	ULONG Mode;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
		return -ENETDOWN;

	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;   
    }

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWMODE, 0,
						&Mode, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	if (Mode == RTMP_CMD_STA_MODE_ADHOC)
		*mode = IW_MODE_ADHOC;
	else if (Mode == RTMP_CMD_STA_MODE_INFRA)
		*mode = IW_MODE_INFRA;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,4,20))
	else if (Mode == RTMP_CMD_STA_MODE_MONITOR) {
		*mode = IW_MODE_MONITOR;

#define RADIOTAP_TYPE 0
#define PRISM_TYPE 1

		if (get_sniffer_mode(dev) == RADIOTAP_TYPE)
			dev->type = ARPHRD_IEEE80211_RADIOTAP;

		if (get_sniffer_mode(dev) == PRISM_TYPE)
			dev->type = ARPHRD_IEEE80211_PRISM;

	}
#endif /* LINUX_VERSION_CODE */
	else
		*mode = IW_MODE_AUTO;

	DBGPRINT(RT_DEBUG_TRACE, ("==>rt_ioctl_giwmode(mode=%d)\n", *mode));
	return 0;
}

int rt_ioctl_siwsens(struct net_device *dev,
		   struct iw_request_info *info,
		   char *name, char *extra)
{
	VOID *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    	{
        	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        	return -ENETDOWN;   
    	}

	return 0;
}

int rt_ioctl_giwsens(struct net_device *dev,
		   struct iw_request_info *info,
		   char *name, char *extra)
{
	return 0;
}

int rt_ioctl_giwrange(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *data, char *extra)
{
	VOID *pAd = NULL;
	struct iw_range *range = (struct iw_range *) extra;
	UINT16 val;
	int i;
	ULONG Mode, ChannelListNum;
	UCHAR *pChannel;
	UINT32 *pFreq;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
#ifndef RT_CFG80211_SUPPORT
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
    	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
    	return -ENETDOWN;   
	}
#endif /* RT_CFG80211_SUPPORT */
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */

	/*DBGPRINT(RT_DEBUG_TRACE ,("===>rt_ioctl_giwrange\n"));*/
	data->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(struct iw_range));

	range->txpower_capa = IW_TXPOW_DBM;

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWMODE, 0,
						&Mode, 0, RT_DEV_PRIV_FLAGS_GET(dev));

/*	if (INFRA_ON(pAd)||ADHOC_ON(pAd)) */
	if ((Mode == RTMP_CMD_STA_MODE_INFRA) || (Mode == RTMP_CMD_STA_MODE_ADHOC))
	{
		range->min_pmp = 1 * 1024;
		range->max_pmp = 65535 * 1024;
		range->min_pmt = 1 * 1024;
		range->max_pmt = 1000 * 1024;
		range->pmp_flags = IW_POWER_PERIOD;
		range->pmt_flags = IW_POWER_TIMEOUT;
		range->pm_capa = IW_POWER_PERIOD | IW_POWER_TIMEOUT |
			IW_POWER_UNICAST_R | IW_POWER_ALL_R;
	}

	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 14;

	range->retry_capa = IW_RETRY_LIMIT;
	range->retry_flags = IW_RETRY_LIMIT;
	range->min_retry = 0;
	range->max_retry = 255;

/*	range->num_channels =  pAd->ChannelListNum; */
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_CHAN_LIST_NUM_GET, 0,
						&ChannelListNum, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	range->num_channels = (UINT16)ChannelListNum;

	os_alloc_mem(NULL, (UCHAR **)&pChannel, sizeof(UCHAR)*ChannelListNum);
	if (pChannel == NULL)
		return -ENOMEM;
	os_alloc_mem(NULL, (UCHAR **)&pFreq, sizeof(UINT32)*ChannelListNum);
	if (pFreq == NULL)
	{
		os_free_mem(NULL, pChannel);
		return -ENOMEM;
	}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_CHAN_LIST_GET, 0,
						pChannel, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_FREQ_LIST_GET, 0,
						pFreq, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	val = 0;
	for (i = 1; i <= range->num_channels; i++) 
	{
/*		UINT32 m = 2412000; */
		range->freq[val].i = pChannel[i-1];
/*		MAP_CHANNEL_ID_TO_KHZ(pAd->ChannelList[i-1].Channel, m); */
		range->freq[val].m = pFreq[i-1] * 100; /* OS_HZ */
		
		range->freq[val].e = 1;
		val++;
		if (val == IW_MAX_FREQUENCIES)
			break;
	}
	os_free_mem(NULL, pChannel);
	os_free_mem(NULL, pFreq);

	range->num_frequency = (UINT8)val;

	range->max_qual.qual = 100; /* what is correct max? This was not
					* documented exactly. At least
					* 69 has been observed. */
	range->max_qual.level = 0; /* dB */
	range->max_qual.noise = 0; /* dB */

	/* What would be suitable values for "average/typical" qual? */
	range->avg_qual.qual = 20;
	range->avg_qual.level = -60;
	range->avg_qual.noise = -95;
	range->sensitivity = 3;

	range->max_encoding_tokens = NR_WEP_KEYS;
	range->num_encoding_sizes = 2;
	range->encoding_size[0] = 5;
	range->encoding_size[1] = 13;

	range->min_rts = 0;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;

#if WIRELESS_EXT > 17
	/* IW_ENC_CAPA_* bit field */
	range->enc_capa = IW_ENC_CAPA_WPA | IW_ENC_CAPA_WPA2 | 
					IW_ENC_CAPA_CIPHER_TKIP | IW_ENC_CAPA_CIPHER_CCMP;
#endif

	return 0;
}

int rt_ioctl_giwpriv(	
	struct net_device *dev,
	struct iw_request_info *info,
	struct iw_point *dwrq,	char *extra)
{
#ifdef ANDROID_SUPPORT
	VOID *pAd = NULL;
	int len = 0;
	char *ext;
	int ret = 0;

	len = dwrq->length;
	ext = kmalloc(len, /*GFP_KERNEL*/GFP_ATOMIC);
	if(!ext)	
		return -ENOMEM;

	if (copy_from_user(ext, dwrq->pointer, len))
	{
		kfree(ext);
		printk("andriod_handle_private   copy_from_user\n");
		return -EFAULT;
	}
	ext[len-1] = 0x00;
	GET_PAD_FROM_NET_DEV(pAd, dev);

	if(rtstrcasecmp(ext,"START") == TRUE)
	{
		//Turn on Wi-Fi hardware
		//OK if successful
		printk("sSTART Turn on Wi-Fi hardware \n");
		kfree(ext);
		return -1;
	}
	else if(rtstrcasecmp(ext,"STOP") == TRUE)
	{
		printk("STOP Turn off  Wi-Fi hardware \n");
		kfree(ext);
		return -1;
	}
	else if(rtstrcasecmp(ext,"RSSI") == TRUE)
	{
		CHAR AvgRssi0;
		RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWPRIVRSSI,
								0, &AvgRssi0, 0, dev->priv_flags);
		snprintf(ext, min(dwrq->length, (UINT16)(strlen(ext)+1)),"rssi %d", AvgRssi0);
	}
	else if(rtstrcasecmp(ext,"LINKSPEED") == TRUE)
	{
		snprintf(ext, min(dwrq->length, (UINT16)(strlen(ext)+1)),"LINKSPEED %d", 150);
	}
	else if(rtstrcasecmp(ext,"MACADDR") == TRUE)
	{
		UCHAR mac[6];
		RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIFHWADDR,
								0, mac, 0, dev->priv_flags);
		snprintf(ext, min(dwrq->length, (UINT16)(strlen(ext)+1)),
			"MACADDR = %02x.%02x.%02x.%02x.%02x.%02x",
			mac[0], mac[1], mac[2],
			mac[3], mac[4], mac[5]);
	}
	else if(rtstrcasecmp(ext,"SCAN-ACTIVE") == TRUE)
	{
		snprintf(ext, min(dwrq->length, (UINT16)(strlen(ext)+1)),"OK");
	}
	else if(rtstrcasecmp(ext,"SCAN-PASSIVE") == TRUE)
	{
		snprintf(ext, min(dwrq->length, (UINT16)(strlen(ext)+1)),"OK");
	}
	else
	{
		goto FREE_EXT;
	}

	if (copy_to_user(dwrq->pointer, ext, min(dwrq->length, (UINT16)(strlen(ext)+1)) ) )
		ret = -EFAULT;

FREE_EXT:

	kfree(ext);

	return ret;
#else
	return 0;
#endif
}


int rt_ioctl_siwap(struct net_device *dev,
		      struct iw_request_info *info,
		      struct sockaddr *ap_addr, char *extra)
{
	VOID *pAd = NULL;
    UCHAR Bssid[6];

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
       	return -ENETDOWN;   
    }

#if 0 /* os abl move */
	if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
    {
        RTMP_MLME_RESET_STATE_MACHINE(pAd);
        DBGPRINT(RT_DEBUG_TRACE, ("!!! MLME busy, reset MLME state machine !!!\n"));
    }

    /* tell CNTL state machine to call NdisMSetInformationComplete() after completing */
    /* this request, because this request is initiated by NDIS. */
    pAd->MlmeAux.CurrReqIsFromNdis = FALSE; 
	/* Prevent to connect AP again in STAMlmePeriodicExec */
	pAd->MlmeAux.AutoReconnectSsidLen= 32;

    memset(Bssid, 0, MAC_ADDR_LEN);
    memcpy(Bssid, ap_addr->sa_data, MAC_ADDR_LEN);

	if (MAC_ADDR_EQUAL(Bssid, ZERO_MAC_ADDR))
	{
		if (INFRA_ON(pAd))
		{
			LinkDown(pAd, FALSE);
		}
	}
    else
	{
		MlmeEnqueue(pAd, 
			MLME_CNTL_STATE_MACHINE, 
			OID_802_11_BSSID, 
			sizeof(NDIS_802_11_MAC_ADDRESS),
			(VOID *)&Bssid, 0);
	}
#endif /* 0 */

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWAP, 0,
					(VOID *)(ap_addr->sa_data), 0, RT_DEV_PRIV_FLAGS_GET(dev));

    memcpy(Bssid, ap_addr->sa_data, MAC_ADDR_LEN);

    DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCSIWAP %02x:%02x:%02x:%02x:%02x:%02x\n",
        Bssid[0], Bssid[1], Bssid[2], Bssid[3], Bssid[4], Bssid[5]));

	return 0;
}

int rt_ioctl_giwap(struct net_device *dev,
		      struct iw_request_info *info,
		      struct sockaddr *ap_addr, char *extra)
{
	VOID *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;   
    }

#if 0 /* os abl move */
	if (INFRA_ON(pAd) || ADHOC_ON(pAd))
	{
		ap_addr->sa_family = ARPHRD_ETHER;
		memcpy(ap_addr->sa_data, &pAd->CommonCfg.Bssid, ETH_ALEN);
	}
#ifdef WPA_SUPPLICANT_SUPPORT    
    /* Add for RT2870 */
    else if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP != WPA_SUPPLICANT_DISABLE)
    {
        ap_addr->sa_family = ARPHRD_ETHER;
        memcpy(ap_addr->sa_data, &pAd->MlmeAux.Bssid, ETH_ALEN);
    }
#endif /* WPA_SUPPLICANT_SUPPORT */    
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWAP(=EMPTY)\n"));
		return -ENOTCONN;
	}
#endif /* 0 */

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWAP, 0,
						(VOID *)(ap_addr->sa_data), 0,
						RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWAP(=EMPTY)\n"));
		return -ENOTCONN;
	}
	ap_addr->sa_family = ARPHRD_ETHER;

	return 0;
}

/*
 * Units are in db above the noise floor. That means the
 * rssi values reported in the tx/rx descriptors in the
 * driver are the SNR expressed in db.
 *
 * If you assume that the noise floor is -95, which is an
 * excellent assumption 99.5 % of the time, then you can
 * derive the absolute signal level (i.e. -95 + rssi). 
 * There are some other slight factors to take into account
 * depending on whether the rssi measurement is from 11b,
 * 11g, or 11a.   These differences are at most 2db and
 * can be documented.
 *
 * NB: various calculations are based on the orinoco/wavelan
 *     drivers for compatibility
 */
static void set_quality(VOID *pAd,
                        struct iw_quality *iq, 
						RT_CMD_STA_IOCTL_BSS *pBss)
/*                        BSS_ENTRY *pBssEntry) */
{
#if 0 /* os abl move */
	__u8 ChannelQuality;

	/* Normalize Rssi */
	if (pBssEntry->Rssi >= -50)
        ChannelQuality = 100;
	else if (pBssEntry->Rssi >= -80) /* between -50 ~ -80dbm */
		ChannelQuality = (__u8)(24 + ((pBssEntry->Rssi + 80) * 26)/10);
	else if (pBssEntry->Rssi >= -90)   /* between -80 ~ -90dbm */
        ChannelQuality = (__u8)((pBssEntry->Rssi + 90) * 26)/10;   
	else
		ChannelQuality = 0;
        
    iq->qual = (__u8)ChannelQuality;
    
    iq->level = (__u8)(pBssEntry->Rssi);

    if (pBssEntry->Rssi >= -70)
		iq->noise = -92;
	else
		iq->noise = pBssEntry->Rssi - pBssEntry->MinSNR;		
#endif /* 0 */

	iq->qual = pBss->ChannelQuality;
	iq->level = (__u8)(pBss->Rssi);
	iq->noise = pBss->Noise;

/*    iq->updated = pAd->iw_stats.qual.updated; */
/*	iq->updated = ((struct iw_statistics *)(pAd->iw_stats))->qual.updated; */
	iq->updated = 1;     /* Flags to know if updated */

#if WIRELESS_EXT >= 17
	iq->updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED | IW_QUAL_NOISE_UPDATED;
#endif

#if WIRELESS_EXT >= 19
	iq->updated |= IW_QUAL_DBM;	/* Level + Noise are dBm */
#endif
}

int rt_ioctl_iwaplist(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *data, char *extra)
{
 	VOID *pAd = NULL;	

/*	struct sockaddr addr[IW_MAX_AP]; */
	struct sockaddr *addr = NULL;
	struct iw_quality qual[IW_MAX_AP];
	int i;
	RT_CMD_STA_IOCTL_BSS_LIST BssList, *pBssList = &BssList;
	RT_CMD_STA_IOCTL_BSS *pList;

	memset(qual, 0, sizeof(qual));
	GET_PAD_FROM_NET_DEV(pAd, dev);

   	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
		data->length = 0;
		return 0;
        /*return -ENETDOWN; */
	}

	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&(pBssList->pList), sizeof(RT_CMD_STA_IOCTL_BSS_LIST) * IW_MAX_AP);
	if (pBssList->pList == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return 0;
	}

	os_alloc_mem(NULL, (UCHAR **)&addr, sizeof(struct sockaddr) * IW_MAX_AP);
	if (addr == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		os_free_mem(NULL, pBssList);
		return 0;
	}

	pBssList->MaxNum = IW_MAX_AP;
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_BSS_LIST_GET, 0,
						pBssList, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	for (i = 0; i <IW_MAX_AP ; i++)
	{
		if (i >=  pBssList->BssNum) /*pAd->ScanTab.BssNr) */
			break;
		addr[i].sa_family = ARPHRD_ETHER;
		pList = (pBssList->pList) + i;
		memcpy(addr[i].sa_data, pList->Bssid, MAC_ADDR_LEN);
		set_quality(pAd, &qual[i], pList); /*&pAd->ScanTab.BssEntry[i]); */
	}
	data->length = (USHORT)i;
	memcpy(extra, addr, i*sizeof(addr[0]));
	data->flags = 1;		/* signal quality present (sort of) */
	memcpy(extra + i*sizeof(addr[0]), &qual, i*sizeof(qual[i]));

	os_free_mem(NULL, addr);
	os_free_mem(NULL, pBssList->pList);
	return 0;
}

#if defined(SIOCGIWSCAN) || defined(RT_CFG80211_SUPPORT)
int rt_ioctl_siwscan(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wreq, char *extra)
{
	VOID *pAd = NULL;
	int Status = NDIS_STATUS_SUCCESS;
	RT_CMD_STA_IOCTL_SCAN IoctlScan, *pIoctlScan = &IoctlScan;
#ifdef WPA_SUPPLICANT_SUPPORT
	struct iw_scan_req *req = (struct iw_scan_req *)extra;
#endif /* WPA_SUPPLICANT_SUPPORT */

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
/* because android will set scan and get scan when interface down */
#ifndef ANDROID_SUPPORT
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
		return -ENETDOWN;   
	}
#endif /* ANDROID_SUPPORT */

#if 0 /* os abl move */
	if (MONITOR_ON(pAd))
    {
        DBGPRINT(RT_DEBUG_TRACE, ("!!! Driver is in Monitor Mode now !!!\n"));
        return -EINVAL;
    }

#ifdef MESH_SUPPORT
	if (pAd->MeshTab.MeshOnly == TRUE)
		return Status;
#endif /* MESH_SUPPORT */

#ifdef WPA_SUPPLICANT_SUPPORT
	if ((pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & 0x7F) == WPA_SUPPLICANT_ENABLE)
	{
		pAd->StaCfg.wpa_supplicant_info.WpaSupplicantScanCount++;
	}
#endif /* WPA_SUPPLICANT_SUPPORT */

    pAd->StaCfg.bScanReqIsFromWebUI = TRUE;
	do{

#ifdef WPA_SUPPLICANT_SUPPORT
		if (((pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & 0x7F) == WPA_SUPPLICANT_ENABLE) &&
			(pAd->StaCfg.wpa_supplicant_info.WpaSupplicantScanCount > 3))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!! WpaSupplicantScanCount > 3\n"));
			Status = NDIS_STATUS_SUCCESS;
			break;
		}
#endif /* WPA_SUPPLICANT_SUPPORT */

		if ((OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED)) &&
			((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA) || 
				(pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK) ||
				(pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2) ||
				(pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)) &&	
			(pAd->StaCfg.PortSecured == WPA_802_1X_PORT_NOT_SECURED))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("!!! Link UP, Port Not Secured! ignore this set::OID_802_11_BSSID_LIST_SCAN\n"));
			Status = NDIS_STATUS_SUCCESS;
			break;
		}

#ifdef WPA_SUPPLICANT_SUPPORT
#if WIRELESS_EXT > 17
        if (wreq->data.length == sizeof(struct iw_scan_req) &&
                wreq->data.flags & IW_SCAN_THIS_ESSID)
        {
                NDIS_802_11_SSID          Ssid;
                struct iw_scan_req *req = (struct iw_scan_req *)extra;
                Ssid.SsidLength = req->essid_len;
                DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_siwscan:: req.essid_len-%d, essid-%s\n", req->essid_len, req->essid));
                NdisZeroMemory(&Ssid.Ssid, NDIS_802_11_LENGTH_SSID);
                NdisMoveMemory(Ssid.Ssid, req->essid, Ssid.SsidLength);
                StaSiteSurvey(pAd, &Ssid, SCAN_ACTIVE);
        }
        else
#endif
#endif /* WPA_SUPPLICANT_SUPPORT */
		StaSiteSurvey(pAd, NULL, SCAN_ACTIVE);
	}while(0);
#endif /* 0 */

	memset(pIoctlScan, 0, sizeof(RT_CMD_STA_IOCTL_SCAN));
#ifdef WPA_SUPPLICANT_SUPPORT
#if WIRELESS_EXT > 17
	pIoctlScan->FlgScanThisSsid = (wreq->data.length == sizeof(struct iw_scan_req) &&
									wreq->data.flags & IW_SCAN_THIS_ESSID);
	pIoctlScan->SsidLen = req->essid_len;
	pIoctlScan->pSsid = (CHAR *)(req->essid);
#endif
	pIoctlScan->ScanType = req->scan_type;
#endif /* WPA_SUPPLICANT_SUPPORT */
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWSCAN, 0,
							pIoctlScan, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	RT_CMD_STATUS_TRANSLATE(pIoctlScan->Status);
	Status = pIoctlScan->Status;
	return Status;
}

int rt_ioctl_giwscan(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *data, char *extra)
{
	VOID *pAd = NULL;
	int i = 0, status = 0, array_index = 0;
	RTMP_STRING *current_ev = extra, *previous_ev = extra;
	RTMP_STRING *end_buf, *current_val;
	RTMP_STRING custom[MAX_CUSTOM_LEN] = {0};
#ifndef IWEVGENIE
	unsigned char idx;
#endif /* IWEVGENIE */
	struct iw_event iwe;
	RT_CMD_STA_IOCTL_SCAN_TABLE IoctlScan, *pIoctlScan = &IoctlScan;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
/* because android will set scan and get scan when interface down */
#ifndef ANDROID_SUPPORT
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}
#endif /* ANDROID_SUPPORT */

#if 0
	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SCAN_SANITY_CHECK, 0,
							NULL, 0) != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_giwscan:: Still scanning\n"));
		return -EAGAIN;
	}
#endif

#if 0 /* os abl move */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
    {
		/*
		 * Still scanning, indicate the caller should try again.
		 */
		pAd->StaCfg.bScanReqIsFromWebUI = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_giwscan:: Still scanning\n"));
		return -EAGAIN;
	}

	if (pAd->StaCfg.bImprovedScan)
	{
		/*
		 * Fast scanning doesn't complete yet.
		 */
		pAd->StaCfg.bScanReqIsFromWebUI = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_giwscan:: Still scanning\n"));
		return -EAGAIN;
	}
#endif /* 0 */

	pIoctlScan->priv_flags = RT_DEV_PRIV_FLAGS_GET(dev);
	pIoctlScan->pBssTable = NULL;
	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWSCAN, 0,
							pIoctlScan, 0,
							RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
	{
		status = -EINVAL;
		goto go_out;
	}

#if 0 /* os abl move */
#ifdef MESH_SUPPORT
	if(RT_DEV_PRIV_FLAGS_GET(dev) == INT_MESH)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Mesh do not support rt_ioctl_giwscan \n"));
			return -EINVAL;
	}

	if (pAd->MeshTab.MeshOnly == TRUE)
	{
		data->length = 0;
		return 0;
	}
#endif /* MESH_SUPPORT */

#ifdef WPA_SUPPLICANT_SUPPORT
	if ((pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & 0x7F) == WPA_SUPPLICANT_ENABLE)
	{
		pAd->StaCfg.wpa_supplicant_info.WpaSupplicantScanCount = 0;
	}
#endif /* WPA_SUPPLICANT_SUPPORT */
#endif /* 0 */

	if (pIoctlScan->BssNr == 0)
	{
		data->length = 0;
		status = 0;
		goto go_out;
	}

#if WIRELESS_EXT >= 17
    if (data->length > 0)
        end_buf = extra + data->length;
    else
        end_buf = extra + IW_SCAN_MAX_DATA;
#else
    end_buf = extra + IW_SCAN_MAX_DATA;
#endif

	for (i = 0; i < pIoctlScan->BssNr; i++) 
	{
		if (current_ev >= end_buf)
        {
#if WIRELESS_EXT >= 17
			status = -E2BIG;
			goto go_out;
#else
			break;
#endif
        }
		
		/*MAC address */
		/*================================ */
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWAP;
		iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
				memcpy(iwe.u.ap_addr.sa_data, &pIoctlScan->pBssTable[i].Bssid, ETH_ALEN);

        previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
        if (current_ev == previous_ev)
        {
#if WIRELESS_EXT >= 17
            status = -E2BIG;
			goto go_out;
#else
			break;
#endif
        }

		/* 
		Protocol:
			it will show scanned AP's WirelessMode .
			it might be
					802.11a
					802.11a/n
					802.11g/n
					802.11b/g/n
					802.11g
					802.11b/g
		*/
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWNAME;


	{
		RT_CMD_STA_IOCTL_BSS_TABLE *pBssEntry=&pIoctlScan->pBssTable[i];
		BOOLEAN isGonly=FALSE;
		int rateCnt=0;

		if (pBssEntry->Channel>14)
		{
			if (pBssEntry->HtCapabilityLen!=0)
				strcpy(iwe.u.name,"802.11a/n");
			else	
				strcpy(iwe.u.name,"802.11a");
		}
		else
		{
			/*
				if one of non B mode rate is set supported rate . it mean G only. 
			*/
			for (rateCnt=0;rateCnt<pBssEntry->SupRateLen;rateCnt++)
			{									
				/*
					6Mbps(140) 9Mbps(146) and >=12Mbps(152) are supported rate , it mean G only. 
				*/
				if (pBssEntry->SupRate[rateCnt]==140 || pBssEntry->SupRate[rateCnt]==146 || pBssEntry->SupRate[rateCnt]>=152)
					isGonly=TRUE;
			}

			for (rateCnt=0;rateCnt<pBssEntry->ExtRateLen;rateCnt++)
			{
				if (pBssEntry->ExtRate[rateCnt]==140 || pBssEntry->ExtRate[rateCnt]==146 || pBssEntry->ExtRate[rateCnt]>=152)
					isGonly=TRUE;
			}		
			
			
			if (pBssEntry->HtCapabilityLen!=0)
			{
				if (isGonly==TRUE)
					strcpy(iwe.u.name,"802.11g/n");
				else
					strcpy(iwe.u.name,"802.11b/g/n");
			}
			else
			{
				if (isGonly==TRUE)
					strcpy(iwe.u.name,"802.11g");
				else
				{
					if (pBssEntry->SupRateLen==4 && pBssEntry->ExtRateLen==0)
						strcpy(iwe.u.name,"802.11b");
					else
						strcpy(iwe.u.name,"802.11b/g");		
				}
			}
		}
	}

		previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_ADDR_LEN);
		if (current_ev == previous_ev)
		{
#if WIRELESS_EXT >= 17
	   		status = -E2BIG;
			goto go_out;
#else
			break;
#endif
		}

		/*ESSID */
		/*================================ */
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWESSID;
		iwe.u.data.length = pIoctlScan->pBssTable[i].SsidLen;
		iwe.u.data.flags = 1;
 
        previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_POINT(info, current_ev,end_buf, &iwe, (RTMP_STRING *) pIoctlScan->pBssTable[i].Ssid);
        if (current_ev == previous_ev)
        {
#if WIRELESS_EXT >= 17
            status = -E2BIG;
			goto go_out;
#else
			break;
#endif
        }
		
		/*Network Type */
		/*================================ */
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWMODE;
		if (pIoctlScan->pBssTable[i].BssType == Ndis802_11IBSS)
		{
			iwe.u.mode = IW_MODE_ADHOC;
		}
		else if (pIoctlScan->pBssTable[i].BssType == Ndis802_11Infrastructure)
		{
			iwe.u.mode = IW_MODE_INFRA;
		}
		else
		{
			iwe.u.mode = IW_MODE_AUTO;
		}
		iwe.len = IW_EV_UINT_LEN;

        previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe,  IW_EV_UINT_LEN);
        if (current_ev == previous_ev)
        {
#if WIRELESS_EXT >= 17
            status = -E2BIG;
			goto go_out;
#else
			break;
#endif
        }

		/*Channel and Frequency */
		/*================================ */
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWFREQ;
		{
			UCHAR ch = pIoctlScan->pBssTable[i].Channel;
			ULONG	m = 0;
/*			MAP_CHANNEL_ID_TO_KHZ(ch, m); */
			RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_CHID_2_FREQ, 0,
								(VOID *)&m, ch, RT_DEV_PRIV_FLAGS_GET(dev));
			iwe.u.freq.m = m * 100;
			iwe.u.freq.e = 1;
		iwe.u.freq.i = 0;
		previous_ev = current_ev;
		current_ev = IWE_STREAM_ADD_EVENT(info, current_ev,end_buf, &iwe, IW_EV_FREQ_LEN);
        if (current_ev == previous_ev)
	        {
#if WIRELESS_EXT >= 17
	            status = -E2BIG;
				goto go_out;
#else
			break;
#endif
		}	    
		}	    

        /*Add quality statistics */
        /*================================ */
        memset(&iwe, 0, sizeof(iwe));
    	iwe.cmd = IWEVQUAL;
    	iwe.u.qual.level = 0;
    	iwe.u.qual.noise = 0;
		set_quality(pAd, &iwe.u.qual, &pIoctlScan->pBssTable[i].Signal);
    	current_ev = IWE_STREAM_ADD_EVENT(info, current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
	if (current_ev == previous_ev)
		{
#if WIRELESS_EXT >= 17
	            status = -E2BIG;
				goto go_out;
#else
			break;
#endif
		}

		/*Encyption key */
		/*================================ */
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWENCODE;
		if (pIoctlScan->pBssTable[i].FlgIsPrivacyOn)
			iwe.u.data.flags =IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		else
			iwe.u.data.flags = IW_ENCODE_DISABLED;
		previous_ev = current_ev;
		array_index = iwe.u.data.flags >> 15;
		if (array_index == 1) {
			iwe.u.data.length = 0;
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, NULL);
		} else {
			iwe.u.data.length = 16;
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
			memcpy(custom, pIoctlScan->MainSharedKey[array_index], 16);
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, custom);
		}
        if (current_ev == previous_ev)
        {
#if WIRELESS_EXT >= 17
            status = -E2BIG;
			goto go_out;
#else
			break;
#endif
        }

		/*Bit Rate */
		/*================================ */
		if (pIoctlScan->pBssTable[i].SupRateLen)
        {
            UCHAR tmpRate = pIoctlScan->pBssTable[i].SupRate[pIoctlScan->pBssTable[i].SupRateLen-1];
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = SIOCGIWRATE;
    		current_val = current_ev + IW_EV_LCP_LEN;            
            if (tmpRate == 0x82)
                iwe.u.bitrate.value =  1 * 1000000;
            else if (tmpRate == 0x84)
                iwe.u.bitrate.value =  2 * 1000000;
            else if (tmpRate == 0x8B)
                iwe.u.bitrate.value =  5.5 * 1000000;
            else if (tmpRate == 0x96)
                iwe.u.bitrate.value =  11 * 1000000;
            else
    		    iwe.u.bitrate.value =  (tmpRate/2) * 1000000;
            
			if (pIoctlScan->pBssTable[i].ExtRateLen)
			{
				UCHAR tmpSupRate =(pIoctlScan->pBssTable[i].SupRate[pIoctlScan->pBssTable[i].SupRateLen-1]& 0x7f);
				UCHAR tmpExtRate =(pIoctlScan->pBssTable[i].ExtRate[pIoctlScan->pBssTable[i].ExtRateLen-1]& 0x7f);
				iwe.u.bitrate.value = (tmpSupRate > tmpExtRate) ? (tmpSupRate)*500000 : (tmpExtRate)*500000;	
			}

			if (tmpRate == 0x6c && pIoctlScan->pBssTable[i].HtCapabilityLen > 0)
			{
				int rate_count = RT_RateSize/sizeof(__s32);
/*				HT_CAP_INFO capInfo = pIoctlScan->pBssTable[i].HtCapability.HtCapInfo; */
				int shortGI = pIoctlScan->pBssTable[i].ChannelWidth ? pIoctlScan->pBssTable[i].ShortGIfor40 : pIoctlScan->pBssTable[i].ShortGIfor20;
				int maxMCS = pIoctlScan->pBssTable[i].MCSSet ?  15 : 7;
				int rate_index = 12 + ((UCHAR)pIoctlScan->pBssTable[i].ChannelWidth * 24) +
								((UCHAR)shortGI *48) + ((UCHAR)maxMCS);
				if (rate_index < 0)
					rate_index = 0;
				if (rate_index >= rate_count)
					rate_index = rate_count-1;
				iwe.u.bitrate.value	=  ralinkrate[rate_index] * 500000;
			}
            
			iwe.u.bitrate.disabled = 0;
			current_val = IWE_STREAM_ADD_VALUE(info, current_ev,
				current_val, end_buf, &iwe,
    			IW_EV_PARAM_LEN);            

        	if((current_val-current_ev)>IW_EV_LCP_LEN)
            	current_ev = current_val;
        	else
        	{
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
        }
        }
            
#ifdef IWEVGENIE
        /*WPA IE */
		if (pIoctlScan->pBssTable[i].WpaIeLen > 0)
        {
			memset(&iwe, 0, sizeof(iwe));
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
			memcpy(custom, &(pIoctlScan->pBssTable[i].pWpaIe[0]), 
						   pIoctlScan->pBssTable[i].WpaIeLen);
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = pIoctlScan->pBssTable[i].WpaIeLen;
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, custom);
			if (current_ev == previous_ev)
			{
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
		}
		}
            
		/*WPA2 IE */
        if (pIoctlScan->pBssTable[i].RsnIeLen > 0)
        {
        	memset(&iwe, 0, sizeof(iwe));
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
			memcpy(custom, &(pIoctlScan->pBssTable[i].pRsnIe[0]), 
						   pIoctlScan->pBssTable[i].RsnIeLen);
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = pIoctlScan->pBssTable[i].RsnIeLen;
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, custom);
			if (current_ev == previous_ev)
			{
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
        }
        }

		/*WPS IE */
		if (pIoctlScan->pBssTable[i].WpsIeLen > 0)
        {
        	memset(&iwe, 0, sizeof(iwe));
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
			memcpy(custom, &(pIoctlScan->pBssTable[i].pWpsIe[0]), 
						   pIoctlScan->pBssTable[i].WpsIeLen);
			iwe.cmd = IWEVGENIE;
			iwe.u.data.length = pIoctlScan->pBssTable[i].WpsIeLen;
			current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe, custom);
			if (current_ev == previous_ev)
			{
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
        }
        }
#else
        /*WPA IE */
		/*================================ */
        if (pIoctlScan->pBssTable[i].WpaIeLen > 0)
        {
    		NdisZeroMemory(&iwe, sizeof(iwe));
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
    		iwe.cmd = IWEVCUSTOM;
            iwe.u.data.length = (pIoctlScan->pBssTable[i].WpaIeLen * 2) + 7;
            NdisMoveMemory(custom, "wpa_ie=", 7);
            for (idx = 0; idx < pIoctlScan->pBssTable[i].WpaIeLen; idx++)
                sprintf(custom, "%s%02x", custom, pIoctlScan->pBssTable[i].pWpaIe[idx]);
            previous_ev = current_ev;
    		current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe,  custom);
            if (current_ev == previous_ev)
            {
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
        }
        }

        /*WPA2 IE */
        if (pIoctlScan->pBssTable[i].RsnIeLen > 0)
        {
    		NdisZeroMemory(&iwe, sizeof(iwe));
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
    		iwe.cmd = IWEVCUSTOM;
            iwe.u.data.length = (pIoctlScan->pBssTable[i].RsnIeLen * 2) + 7;
            NdisMoveMemory(custom, "rsn_ie=", 7);
			for (idx = 0; idx < pIoctlScan->pBssTable[i].RsnIeLen; idx++)
                sprintf(custom, "%s%02x", custom, pIoctlScan->pBssTable[i].pRsnIe[idx]);
            previous_ev = current_ev;
    		current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe,  custom);
            if (current_ev == previous_ev)
            {
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
        }
        }

#ifdef WSC_INCLUDED
		/*WPS IE */
		if (pIoctlScan->pBssTable[i].WpsIeLen > 0)
        {
    		NdisZeroMemory(&iwe, sizeof(iwe));
			memset(&custom[0], 0, MAX_CUSTOM_LEN);
    		iwe.cmd = IWEVCUSTOM;
            iwe.u.data.length = (pIoctlScan->pBssTable[i].WpsIeLen * 2) + 7;
            NdisMoveMemory(custom, "wps_ie=", 7);
			for (idx = 0; idx < pIoctlScan->pBssTable[i].WpsIeLen; idx++)
                sprintf(custom, "%s%02x", custom, pIoctlScan->pBssTable[i].pWpsIe[idx]);
            previous_ev = current_ev;
    		current_ev = IWE_STREAM_ADD_POINT(info, current_ev, end_buf, &iwe,  custom);
            if (current_ev == previous_ev)
            {
#if WIRELESS_EXT >= 17
                status = -E2BIG;
				goto go_out;
#else
			    break;
#endif
        }
        }
#endif /* WSC_INCLUDED */

#endif /* IWEVGENIE */
	}

	data->length = (USHORT)(current_ev - extra);
/*    pAd->StaCfg.bScanReqIsFromWebUI = FALSE; */
/*	DBGPRINT(RT_DEBUG_ERROR ,("===>rt_ioctl_giwscan. %d(%d) BSS returned, data->length = %d\n",i , pAd->ScanTab.BssNr, data->length)); */

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SCAN_END, 0,
						data, data->length, RT_DEV_PRIV_FLAGS_GET(dev));

go_out:
	if (pIoctlScan->pBssTable != NULL)
		os_free_mem(NULL, pIoctlScan->pBssTable);

	return status;
}
#endif

int rt_ioctl_siwessid(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *essid)
{
	VOID *pAd = NULL;
	RT_CMD_STA_IOCTL_SSID IoctlEssid, *pIoctlEssid = &IoctlEssid;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
       	return -ENETDOWN;   
    }

	if (data->flags)
	{
		/* Includes null character. */
		if (data->length > (IW_ESSID_MAX_SIZE + 1)) 
			return -E2BIG;
	}

#if 0 /* os abl move */
	if (data->flags)
	{
		RTMP_STRING *pSsidString = NULL;

		/* Includes null character. */
		if (data->length > (IW_ESSID_MAX_SIZE + 1)) 
			return -E2BIG;

		pSsidString = kmalloc(MAX_LEN_OF_SSID+1, MEM_ALLOC_FLAG);
		if (pSsidString)
        {
			NdisZeroMemory(pSsidString, MAX_LEN_OF_SSID+1);
			NdisMoveMemory(pSsidString, essid, data->length);
			if (Set_SSID_Proc(pAd, pSsidString) == FALSE)
			{
				kfree(pSsidString);
				return -EINVAL;
			}
			kfree(pSsidString);
		}
		else
			return -ENOMEM;
		}
	else
    {
		/* ANY ssid */
		if (Set_SSID_Proc(pAd, "") == FALSE)
			return -EINVAL;
    }
	return 0;
#endif /* 0 */

	pIoctlEssid->FlgAnySsid = (UCHAR)data->flags;
	pIoctlEssid->SsidLen = data->length;
	pIoctlEssid->pSsid = (CHAR *)essid;
	pIoctlEssid->Status = 0;
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWESSID, 0,
						pIoctlEssid, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	RT_CMD_STATUS_TRANSLATE(pIoctlEssid->Status);
	return pIoctlEssid->Status;
}

int rt_ioctl_giwessid(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *essid)
{
	VOID *pAd = NULL;
	RT_CMD_STA_IOCTL_SSID IoctlEssid, *pIoctlEssid = &IoctlEssid;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}

	data->flags = 1;

#if 0 /* os abl move */
	data->flags = 1;		
    if (MONITOR_ON(pAd))
    {
        data->length  = 0;
        return 0;
    }

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
	{
		DBGPRINT(RT_DEBUG_TRACE ,("MediaState is connected\n"));
		data->length = pAd->CommonCfg.SsidLen;
		memcpy(essid, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen);
	}
#ifdef RTMP_MAC_USB    
#ifdef WPA_SUPPLICANT_SUPPORT
    /* Add for RT2870 */
    else if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP != WPA_SUPPLICANT_DISABLE)
    {
        data->length = pAd->CommonCfg.SsidLen;
		memcpy(essid, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen);
	}
#endif /* WPA_SUPPLICANT_SUPPORT */
#endif /* RTMP_MAC_USB */
	else
	{/*the ANY ssid was specified */
		data->length  = 0;
		DBGPRINT(RT_DEBUG_TRACE ,("MediaState is not connected, ess\n"));
	}

	return 0;
#endif /* 0 */

	pIoctlEssid->pSsid = (CHAR *)essid;
	pIoctlEssid->Status = 0;
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWESSID, 0,
						pIoctlEssid, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	data->length = (USHORT)pIoctlEssid->SsidLen;

	RT_CMD_STATUS_TRANSLATE(pIoctlEssid->Status);
	return pIoctlEssid->Status;
}

int rt_ioctl_siwnickn(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *nickname)
{
	VOID *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_TRACE ,("INFO::Network is down!\n"));
        return -ENETDOWN;   
    }

	if (data->length > IW_ESSID_MAX_SIZE)
		return -EINVAL;

#if 0 /* os abl move */
	memset(pAd->nickname, 0, IW_ESSID_MAX_SIZE + 1);
	memcpy(pAd->nickname, nickname, data->length);
#endif /* 0 */

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWNICKN, 0,
							nickname, data->length, RT_DEV_PRIV_FLAGS_GET(dev));
	return 0;
}

int rt_ioctl_giwnickn(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *data, char *nickname)
{
	VOID *pAd = NULL;
	RT_CMD_STA_IOCTL_NICK_NAME NickName, *pNickName = &NickName;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
		data->length = 0;
        return -ENETDOWN;
	}

#if 0 /* os abl move */
	if (data->length > strlen((RTMP_STRING *) pAd->nickname) + 1)
		data->length = strlen((RTMP_STRING *) pAd->nickname) + 1;
	if (data->length > 0) {
		memcpy(nickname, pAd->nickname, data->length-1);
		nickname[data->length-1] = '\0';
	}
#endif /* 0 */

	pNickName->NameLen = data->length;
	pNickName->pName = (CHAR *)nickname;

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWNICKN, 0,
							pNickName, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	data->length = (USHORT)pNickName->NameLen;
	return 0;
}

int rt_ioctl_siwrts(struct net_device *dev,
		       struct iw_request_info *info,
		       struct iw_param *rts, char *extra)
{
	VOID *pAd = NULL;
	UINT16 val;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
        DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;   
    }
	
	if (rts->disabled)
		val = MAX_RTS_THRESHOLD;
	else if (rts->value < 0 || rts->value > MAX_RTS_THRESHOLD)
		return -EINVAL;
	else if (rts->value == 0)
	    val = MAX_RTS_THRESHOLD;
	else
		val = (UINT16)rts->value;
	
/*	if (val != pAd->CommonCfg.RtsThreshold) */
/*		pAd->CommonCfg.RtsThreshold = val; */

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWRTS, 0,
						NULL, val, RT_DEV_PRIV_FLAGS_GET(dev));
	return 0;
}

int rt_ioctl_giwrts(struct net_device *dev,
		       struct iw_request_info *info,
		       struct iw_param *rts, char *extra)
{
	VOID *pAd = NULL;
	USHORT RtsThreshold;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*    	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
		if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    	{
      		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        	return -ENETDOWN;   
    	}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWRTS, 0,
						&RtsThreshold, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	rts->value = RtsThreshold;
	rts->disabled = (rts->value == MAX_RTS_THRESHOLD);
	rts->fixed = 1;

	return 0;
}

int rt_ioctl_siwfrag(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_param *frag, char *extra)
{
	VOID *pAd = NULL;
	UINT16 val;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
		if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    	{
      		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        	return -ENETDOWN;   
    	}

	if (frag->disabled)
		val = MAX_FRAG_THRESHOLD;
	else if (frag->value >= MIN_FRAG_THRESHOLD || frag->value <= MAX_FRAG_THRESHOLD)
        val = __cpu_to_le16(frag->value & ~0x1); /* even numbers only */
	else if (frag->value == 0)
	    val = MAX_FRAG_THRESHOLD;
	else
		return -EINVAL;

/*	pAd->CommonCfg.FragmentThreshold = val; */
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWFRAG, 0,
						info, val, RT_DEV_PRIV_FLAGS_GET(dev));
	return 0;
}

int rt_ioctl_giwfrag(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_param *frag, char *extra)
{
	VOID *pAd = NULL;
	USHORT FragmentThreshold;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*    	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
		if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    	{
      		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        	return -ENETDOWN;   
    	}
		
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWFRAG, 0,
						&FragmentThreshold, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	frag->value = FragmentThreshold;
	frag->disabled = (frag->value == MAX_FRAG_THRESHOLD);
	frag->fixed = 1;

	return 0;
}

#define MAX_WEP_KEY_SIZE 13
#define MIN_WEP_KEY_SIZE 5
int rt_ioctl_siwencode(struct net_device *dev,
			  struct iw_request_info *info,
			  struct iw_point *erq, char *extra)
{
	VOID *pAd = NULL;
	RT_CMD_STA_IOCTL_SECURITY IoctlSec, *pIoctlSec = &IoctlSec;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
		if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    	{
      		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        	return -ENETDOWN;   
    	}

#if 0 /* os abl move */
	if ((erq->length == 0) &&
        (erq->flags & IW_ENCODE_DISABLED))
	{
		pAd->StaCfg.PairCipher = Ndis802_11WEPDisabled;
		pAd->StaCfg.GroupCipher = Ndis802_11WEPDisabled;
		pAd->StaCfg.WepStatus = Ndis802_11WEPDisabled;
        pAd->StaCfg.AuthMode = Ndis802_11AuthModeOpen;
        goto done;
	}
	else if (erq->flags & IW_ENCODE_RESTRICTED || erq->flags & IW_ENCODE_OPEN)
	{
	    /*pAd->StaCfg.PortSecured = WPA_802_1X_PORT_SECURED; */
		STA_PORT_SECURED(pAd);
		pAd->StaCfg.PairCipher = Ndis802_11WEPEnabled;
		pAd->StaCfg.GroupCipher = Ndis802_11WEPEnabled;
		pAd->StaCfg.WepStatus = Ndis802_11WEPEnabled;
		if (erq->flags & IW_ENCODE_RESTRICTED)
			pAd->StaCfg.AuthMode = Ndis802_11AuthModeShared;
    	else
			pAd->StaCfg.AuthMode = Ndis802_11AuthModeOpen;
	}
    
    if (erq->length > 0) 
	{
		int keyIdx = (erq->flags & IW_ENCODE_INDEX) - 1;
		/* Check the size of the key */
		if (erq->length > MAX_WEP_KEY_SIZE) 
		{
			return -EINVAL;
		}
		/* Check key index */
		if ((keyIdx < 0) || (keyIdx >= NR_WEP_KEYS))
        {
            DBGPRINT(RT_DEBUG_TRACE ,("==>rt_ioctl_siwencode::Wrong keyIdx=%d! Using default key instead (%d)\n", 
                                        keyIdx, pAd->StaCfg.DefaultKeyId));
            
            /*Using default key */
			keyIdx = pAd->StaCfg.DefaultKeyId;   
        }
		else
			pAd->StaCfg.DefaultKeyId = keyIdx;

        NdisZeroMemory(pAd->SharedKey[BSS0][keyIdx].Key,  16);
		
		if (erq->length == MAX_WEP_KEY_SIZE)
        {      
			pAd->SharedKey[BSS0][keyIdx].KeyLen = MAX_WEP_KEY_SIZE;
            pAd->SharedKey[BSS0][keyIdx].CipherAlg = CIPHER_WEP128;
		}
		else if (erq->length == MIN_WEP_KEY_SIZE)
        {      
            pAd->SharedKey[BSS0][keyIdx].KeyLen = MIN_WEP_KEY_SIZE;
            pAd->SharedKey[BSS0][keyIdx].CipherAlg = CIPHER_WEP64;
		}
		else
			/* Disable the key */
			pAd->SharedKey[BSS0][keyIdx].KeyLen = 0;

		/* Check if the key is not marked as invalid */
		if(!(erq->flags & IW_ENCODE_NOKEY)) 
		{
			/* Copy the key in the driver */
			NdisMoveMemory(pAd->SharedKey[BSS0][keyIdx].Key, extra, erq->length);
        }
	} 
    else 
			{
		/* Do we want to just set the transmit key index ? */
		int index = (erq->flags & IW_ENCODE_INDEX) - 1;
		if ((index >= 0) && (index < 4)) 
        {      
			pAd->StaCfg.DefaultKeyId = index;
        }
        else
			/* Don't complain if only change the mode */
		if(!(erq->flags & IW_ENCODE_MODE))
		{
				return -EINVAL;
		}
	}
		
done:
    DBGPRINT(RT_DEBUG_TRACE ,("==>rt_ioctl_siwencode::erq->flags=%x\n",erq->flags));
	DBGPRINT(RT_DEBUG_TRACE ,("==>rt_ioctl_siwencode::AuthMode=%x\n",pAd->StaCfg.AuthMode));
	DBGPRINT(RT_DEBUG_TRACE ,("==>rt_ioctl_siwencode::DefaultKeyId=%x, KeyLen = %d\n",pAd->StaCfg.DefaultKeyId , pAd->SharedKey[BSS0][pAd->StaCfg.DefaultKeyId].KeyLen));
	DBGPRINT(RT_DEBUG_TRACE ,("==>rt_ioctl_siwencode::WepStatus=%x\n",pAd->StaCfg.WepStatus));
#endif /* 0 */

	pIoctlSec->pData = (CHAR *)extra;
	pIoctlSec->length = erq->length;
	pIoctlSec->KeyIdx = (erq->flags & IW_ENCODE_INDEX) - 1;
	pIoctlSec->flags = 0;

	if (erq->flags & IW_ENCODE_DISABLED)
		pIoctlSec->flags |= RT_CMD_STA_IOCTL_SECURITY_DISABLED;
	if (erq->flags & IW_ENCODE_RESTRICTED)
		pIoctlSec->flags |= RT_CMD_STA_IOCTL_SECURITY_RESTRICTED;
	if (erq->flags & IW_ENCODE_OPEN)
		pIoctlSec->flags |= RT_CMD_STA_IOCTL_SECURITY_OPEN;
	if (erq->flags & IW_ENCODE_NOKEY)
		pIoctlSec->flags |= RT_CMD_STA_IOCTL_SECURITY_NOKEY;
	if (erq->flags & IW_ENCODE_MODE)
		pIoctlSec->flags |= RT_CMD_STA_IOCTL_SECURITY_MODE;

	pIoctlSec->Status = 0;

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWENCODE, 0,
						pIoctlSec, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	RT_CMD_STATUS_TRANSLATE(pIoctlSec->Status);
	return pIoctlSec->Status;
}

int
rt_ioctl_giwencode(struct net_device *dev,
			  struct iw_request_info *info,
			  struct iw_point *erq, char *key)
{
/*	int kid; */
	VOID *pAd = NULL;
	RT_CMD_STA_IOCTL_SECURITY IoctlSec, *pIoctlSec = &IoctlSec;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	/*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
  		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
    	return -ENETDOWN;   
	}
		
#if 0 /* os abl move */
	kid = erq->flags & IW_ENCODE_INDEX;
	DBGPRINT(RT_DEBUG_TRACE, ("===>rt_ioctl_giwencode %d\n", erq->flags & IW_ENCODE_INDEX));

	if (pAd->StaCfg.WepStatus == Ndis802_11WEPDisabled)
	{
		erq->length = 0;
		erq->flags = IW_ENCODE_DISABLED;
	} 
	else if ((kid > 0) && (kid <=4))
	{
		/* copy wep key */
		erq->flags = kid ;			/* NB: base 1 */
		if (erq->length > pAd->SharedKey[BSS0][kid-1].KeyLen)
			erq->length = pAd->SharedKey[BSS0][kid-1].KeyLen;
		memcpy(key, pAd->SharedKey[BSS0][kid-1].Key, erq->length);
		if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeShared)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		
	}
	else if (kid == 0)
	{
		if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeShared)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		erq->length = pAd->SharedKey[BSS0][pAd->StaCfg.DefaultKeyId].KeyLen;
		memcpy(key, pAd->SharedKey[BSS0][pAd->StaCfg.DefaultKeyId].Key, erq->length);
		/* copy default key ID */
		if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeShared)
			erq->flags |= IW_ENCODE_RESTRICTED;		/* XXX */
		else
			erq->flags |= IW_ENCODE_OPEN;		/* XXX */
		erq->flags = pAd->StaCfg.DefaultKeyId + 1;			/* NB: base 1 */
		erq->flags |= IW_ENCODE_ENABLED;	/* XXX */
	}
#endif /* 0 */

	pIoctlSec->pData = (CHAR *)key;
	pIoctlSec->KeyIdx = erq->flags & IW_ENCODE_INDEX;
	pIoctlSec->length = erq->length;

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWENCODE, 0,
						pIoctlSec, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	erq->length = pIoctlSec->length;
	erq->flags = (USHORT)pIoctlSec->KeyIdx;
	if (pIoctlSec->flags & RT_CMD_STA_IOCTL_SECURITY_DISABLED)
		erq->flags = RT_CMD_STA_IOCTL_SECURITY_DISABLED;
	{
		if (pIoctlSec->flags & RT_CMD_STA_IOCTL_SECURITY_ENABLED)
			erq->flags |= IW_ENCODE_ENABLED;
		if (pIoctlSec->flags & RT_CMD_STA_IOCTL_SECURITY_RESTRICTED)
			erq->flags |= IW_ENCODE_RESTRICTED;	
		if (pIoctlSec->flags & RT_CMD_STA_IOCTL_SECURITY_OPEN)
			erq->flags |= IW_ENCODE_OPEN;
	}
	return 0;

}

int rt_ioctl_setparam(struct net_device *dev, struct iw_request_info *info,
			 void *w, char *extra)
{
	VOID *pAd;
/*	POS_COOKIE pObj; */
	RTMP_STRING *this_char = extra;
	RTMP_STRING *value = NULL;
	int  Status=0;
	RT_CMD_PARAM_SET CmdParam;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if 0 /* os abl move */
	pObj = (POS_COOKIE) pAd->OS_Cookie;
#ifdef MESH_SUPPORT
	if (RT_DEV_PRIV_FLAGS_GET(dev) == INT_MESH)
	{
		pObj->ioctl_if_type = INT_MESH;
		pObj->ioctl_if = 0;
	}
	else
#endif /* MESH_SUPPORT */
#ifdef P2P_SUPPORT
		if (RT_DEV_PRIV_FLAGS_GET(dev) == INT_P2P)
		{
			pObj->ioctl_if_type = INT_P2P;
			pObj->ioctl_if = 0;
		}
		else
#endif /* P2P_SUPPORT */
	{
		pObj->ioctl_if_type = INT_MAIN;
        pObj->ioctl_if = MAIN_MBSSID;
	}
#endif /* 0 */

#if 0
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_IW_SET_PARAM_PRE, 0,
				NULL, RT_DEV_PRIV_FLAGS_GET(dev), RT_DEV_PRIV_FLAGS_GET(dev));
#endif
	
	if (!*this_char)
		return -EINVAL;

	value = rtstrchr(this_char, '=');
	if (value)
		*value++ = 0;

	/*check if the interface is down */
/*    	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
		if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, this_char) != NDIS_STATUS_SUCCESS)
    	{
    		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
			return -ENETDOWN;
#if 0    	
			if (!*this_char)
				return -EINVAL;
	                                                                                                                            
			if ((value = rtstrchr(this_char, '=')) != NULL)                                                                             
	    		*value++ = 0;
	                                                                                                                            
			if (!value || !RT_isLegalCmdBeforeInfUp(this_char))
			{
				DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
				return -ENETDOWN;
			}
#endif				
    	}
	else
	{
#if 0	
		if (!*this_char)
			return -EINVAL;
	                                                                                                                            
		if ((value = rtstrchr(this_char, '=')) != NULL)                                                                             
		    *value++ = 0;
#endif
		if (!value && (strcmp(this_char, "SiteSurvey") != 0))
		    return -EINVAL;
		else if (!value && (strcmp(this_char, "SiteSurvey") == 0))
			goto SET_PROC;

		/* reject setting nothing besides ANY ssid(ssidLen=0) */
		if (value) {
			if (!*value && (strcmp(this_char, "SSID") != 0))
				return -EINVAL;
		}

	}

SET_PROC:
	CmdParam.pThisChar = this_char;
	CmdParam.pValue = value;
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_PARAM_SET, 0,
						&CmdParam, 0, RT_DEV_PRIV_FLAGS_GET(dev));
/*	Status = RTMPSTAPrivIoctlSet(pAd, this_char, value); */
		
    return Status;
}


#ifdef WSC_STA_SUPPORT
#if 0 /* no use */
BOOLEAN CheckWscAuthType(
    IN USHORT authType)
{
	switch(authType)
	{
		case WSC_AUTHTYPE_OPEN:
			break;
		case WSC_AUTHTYPE_WPAPSK:
			break;
		case WSC_AUTHTYPE_SHARED:
			break;
		case WSC_AUTHTYPE_WPA:
			break;
		case WSC_AUTHTYPE_WPA2:
			break;
		case WSC_AUTHTYPE_WPA2PSK:
			break;
        default:
            return FALSE;
	}

    return TRUE;
}

USHORT CheckWscEncryType(
    IN USHORT encryType)
{
	switch(encryType)
	{
		case WSC_ENCRTYPE_NONE:
			break;
		case WSC_ENCRTYPE_WEP:
			break;        
		case WSC_ENCRTYPE_TKIP:
			break;
		case WSC_ENCRTYPE_AES:
			break;
        default:
            return FALSE;
	}

    return TRUE;
}
#endif /* 0 */

static int
rt_private_set_wsc_u32_item(struct net_device *dev, struct iw_request_info *info,
			 UINT32 *uwrq, char *extra)
{
    VOID *pAd = NULL;
/*    int  Status=0; */
/*    UINT32 subcmd = *uwrq; */
/*    PWSC_PROFILE    pWscProfile = NULL; */
/*   	UINT32 value = 0; */
	RT_CMD_STA_IOCTL_WSC_U32_ITEM IoctlWscU32, *pIoctlWscU32 = &IoctlWscU32;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}
#if 0
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_INF_TYPE_SET,
			0, NULL, RT_DEV_PRIV_FLAGS_GET(dev), RT_DEV_PRIV_FLAGS_GET(dev));
#endif

#if 0 /* os abl move */
	pWscProfile = &pAd->StaCfg.WscControl.WscProfile;

    switch(subcmd)
    {
        case WSC_CREDENTIAL_COUNT:
            value = *(uwrq + 1);
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_COUNT, value = %d\n", __FUNCTION__, value));
            if (value >= 0 && value <= 8)
            {
                pWscProfile->ProfileCnt = value;
            }
            else
                Status = -EINVAL;
            break;        
        case WSC_SET_DRIVER_CONNECT_BY_CREDENTIAL_IDX:
            value = *(uwrq + 1);
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_DRIVER_CONNECT_BY_CREDENTIAL_IDX, value = %d\n", __FUNCTION__, value));
            if ((value >= 0 && value <= 7) &&
                (value < pWscProfile->ProfileCnt))
            {
                WscWriteConfToPortCfg(pAd, &pAd->StaCfg.WscControl, &pAd->StaCfg.WscControl.WscProfile.Profile[value], TRUE);
                pAd->MlmeAux.CurrReqIsFromNdis = TRUE;
                LinkDown(pAd, TRUE);
            }
            else
                Status = -EINVAL;
            break;
        case WSC_SET_DRIVER_AUTO_CONNECT:
            value = *(uwrq + 1);
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_DRIVER_AUTO_CONNECT, value = %d\n", __FUNCTION__, value));
            if ((value == 0x00) || 
				(value == 0x01) || 
				(value == 0x02))
            {
                pAd->StaCfg.WscControl.WscDriverAutoConnect = value;
            }
            else
                Status = -EINVAL;
            break;
        case WSC_SET_CONF_MODE:
            value = *(uwrq + 1);
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_CONF_MODE, value = %d\n", __FUNCTION__, value));
		if (value == 2)
			value = 4;
            switch(value)
            {
                case WSC_DISABLE:
                    Set_WscConfMode_Proc(pAd, "0");
                    break;
                case WSC_ENROLLEE:
                    Set_WscConfMode_Proc(pAd, "1");
                    break;
                case WSC_REGISTRAR:
                    Set_WscConfMode_Proc(pAd, "2");
                    break;
                default:
                    Status = -EINVAL;
                    break;
            }
            break;
        case WSC_SET_MODE:
            value = *(uwrq + 1);
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_MODE, value = %d\n", __FUNCTION__, value));
            switch(value)
            {
                case WSC_PIN_MODE:
                    if (Set_WscMode_Proc(pAd, "1") == FALSE)
						Status = -EINVAL;
                    break;
                case WSC_PBC_MODE:
                    if (Set_WscMode_Proc(pAd, "2") == FALSE)
						Status = -EINVAL;
                    break;
				case WSC_SMPBC_MODE:
					if (Set_WscMode_Proc(pAd, "3") == FALSE)
						Status = -EINVAL;
					break;
                default:
                    Status = -EINVAL;
                    break;
            }
            break;
        case WSC_START:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_START\n", __FUNCTION__));
            Set_WscGetConf_Proc(pAd, "1");
            break;
        case WSC_STOP:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_STOP\n", __FUNCTION__));
						
            /* Disassociate the link if WPS is working. */
        	if ( INFRA_ON(pAd) && 
                 (pAd->StaCfg.WscControl.bWscTrigger == TRUE) && 
                 (pAd->StaCfg.WscControl.WscConfMode != WSC_DISABLE) )
        	{
        		MLME_DISASSOC_REQ_STRUCT	DisReq;
        										
        		/* Set to immediately send the media disconnect event */
        		pAd->MlmeAux.CurrReqIsFromNdis = TRUE;

        		DBGPRINT(RT_DEBUG_TRACE, ("disassociate with current AP \n"));
        		DisassocParmFill(pAd, &DisReq, pAd->CommonCfg.Bssid, REASON_DISASSOC_STA_LEAVING);
        		MlmeEnqueue(pAd, ASSOC_STATE_MACHINE, MT2_MLME_DISASSOC_REQ, 
        					sizeof(MLME_DISASSOC_REQ_STRUCT), &DisReq, 0);

        		pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_DISASSOC;
				RTMP_MLME_HANDLER(pAd);
        	}

			{
	        	/* Turn off WSC state matchine */
	        	WscStop(pAd,
#ifdef CONFIG_AP_SUPPORT
	        			FALSE,
#endif /* CONFIG_AP_SUPPORT */
	        			&pAd->StaCfg.WscControl);
	            pAd->StaCfg.WscControl.WscConfMode = WSC_DISABLE;
				BssTableDeleteEntry(&pAd->MlmeAux.SsidBssTab, pAd->MlmeAux.Bssid, pAd->MlmeAux.Channel);
			}
            break;
        case WSC_GEN_PIN_CODE:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_GEN_PIN_CODE\n", __FUNCTION__));
            Set_WscGenPinCode_Proc(pAd, "1");
            break;

		case WSC_AP_BAND:
			value = *(uwrq + 1);
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_PBC_BAND, value = %d\n", __FUNCTION__, value));
			if (value < PREFERRED_WPS_AP_PHY_TYPE_MAXIMUM)
			{
				pAd->StaCfg.WscControl.WpsApBand= value;
			}
			break;
			
        default:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - unknow subcmd = %d, value = %d\n", __FUNCTION__, subcmd, value));
            break;
    }
#endif /* 0 */
    
	pIoctlWscU32->pUWrq = uwrq;
	pIoctlWscU32->Status = NDIS_STATUS_SUCCESS;
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_IW_SET_WSC_U32_ITEM, 0,
						pIoctlWscU32, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	RT_CMD_STATUS_TRANSLATE(pIoctlWscU32->Status);
    return pIoctlWscU32->Status;
}

static int
rt_private_set_wsc_string_item(struct net_device *dev, struct iw_request_info *info,
		struct iw_point *dwrq, char *extra)
{
	RT_CMD_STA_IOCTL_WSC_STR_ITEM IoctlWscStr, *pIoctlWscStr = &IoctlWscStr;
/*    int  Status=0; */
/*    UINT32 subcmd = dwrq->flags; */
/*    UINT32 tmpProfileIndex = (UINT32)(extra[0] - 0x30); */
/*    UINT32 dataLen; */
    VOID *pAd = NULL;
/*    PWSC_PROFILE    pWscProfile = NULL; */
/*    USHORT  tmpAuth = 0, tmpEncr = 0; */

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}
#if 0
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_INF_TYPE_SET,
			0, NULL, RT_DEV_PRIV_FLAGS_GET(dev), RT_DEV_PRIV_FLAGS_GET(dev));
#endif

#if 0 /* os abl move */
	pWscProfile = &pAd->StaCfg.WscControl.WscProfile;

    if ((subcmd != WSC_SET_SSID) && 
		(subcmd != WSC_SET_PIN) &&
		(subcmd != WSC_SET_BSSID) &&
        (tmpProfileIndex > 7 || tmpProfileIndex < 0))
    {
        DBGPRINT(RT_DEBUG_TRACE, ("%s - subcmd = %d, tmpProfileIndex = %d\n", __FUNCTION__, subcmd, tmpProfileIndex));
        return -EINVAL;
    }

    if ((subcmd != WSC_SET_SSID) && 
		(subcmd != WSC_SET_PIN) &&
		(subcmd != WSC_SET_BSSID))
    /* extra: "1 input_string", dwrq->length includes '\0'. 3 is size of [index, blank and '\0'] */
    dataLen = dwrq->length - 3;
    else
        dataLen = dwrq->length;
    
    switch(subcmd)
    {
        case WSC_CREDENTIAL_SSID:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_SSID(%s)\n", __FUNCTION__, extra+2));
			if (dataLen == (NDIS_802_11_LENGTH_SSID+1))
				dataLen = NDIS_802_11_LENGTH_SSID;
            if (dataLen > 0 && dataLen <= NDIS_802_11_LENGTH_SSID)
            {
                pWscProfile->Profile[tmpProfileIndex].SSID.SsidLength = dataLen;
                NdisZeroMemory(pWscProfile->Profile[tmpProfileIndex].SSID.Ssid, NDIS_802_11_LENGTH_SSID);
                NdisMoveMemory(pWscProfile->Profile[tmpProfileIndex].SSID.Ssid, extra+2, dataLen);
            }
            else
                Status = -E2BIG;
            break;
        case WSC_CREDENTIAL_AUTH_MODE:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_AUTH_MODE(%s)\n", __FUNCTION__, extra+2));
            if ((tmpAuth = WscGetAuthTypeFromStr(extra+2)) != 0)
            {
                pWscProfile->Profile[tmpProfileIndex].AuthType = tmpAuth;
            }
            else
                Status = -EINVAL;
            break;
        case WSC_CREDENTIAL_ENCR_TYPE:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_ENCR_TYPE(%s)\n", __FUNCTION__, extra+2));
            if ((tmpEncr = WscGetEncrypTypeFromStr(extra+2)) != 0)
            {
                pWscProfile->Profile[tmpProfileIndex].EncrType = tmpEncr;
            }
            else
                Status = -EINVAL;
            break;
        case WSC_CREDENTIAL_KEY_INDEX:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_KEY_INDEX(%s)\n", __FUNCTION__, extra+2));
            if ( *(extra+2) >= 0x31 && *(extra+2) <= 0x34)
            {
                pWscProfile->Profile[tmpProfileIndex].KeyIndex = (UCHAR)*(extra+2) - 0x30;
            }
            else
                Status = -EINVAL;
            break;
        case WSC_CREDENTIAL_KEY:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_KEY(%s)\n", __FUNCTION__, extra+2));
            if ((dataLen >= 8 && dataLen <= 64) ||
                (dataLen == 5 || dataLen == 10 || dataLen == 13 || dataLen == 26))
            {
                pWscProfile->Profile[tmpProfileIndex].KeyLength = dataLen;
                NdisZeroMemory(pWscProfile->Profile[tmpProfileIndex].Key, 64);
                NdisMoveMemory(pWscProfile->Profile[tmpProfileIndex].Key, extra+2, dataLen);
            }
            else
                Status = -EINVAL;
            break;
        case WSC_CREDENTIAL_MAC:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_CREDENTIAL_MAC(%s)\n", __FUNCTION__, extra+2));
            {
                INT sscanf_rv = 0;
                UINT tmp_val[6] = {0};
                sscanf_rv = sscanf(extra+2, "%02x:%02x:%02x:%02x:%02x:%02x", 
                                                          &tmp_val[0],
                                                          &tmp_val[1],
                                                          &tmp_val[2],
                                                          &tmp_val[3],
                                                          &tmp_val[4],
                                                          &tmp_val[5]);
                if ( sscanf_rv == 6)
                {
                    int ii;
                    NdisZeroMemory(pWscProfile->Profile[tmpProfileIndex].MacAddr, 6);
                    for (ii=0; ii<6; ii++)
                        pWscProfile->Profile[tmpProfileIndex].MacAddr[ii] = (UCHAR)tmp_val[ii];
                }
                else
                    Status = -EINVAL;
            }            
            break;
        case WSC_SET_SSID:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_SSID(%s)\n", __FUNCTION__, extra));
			if (dataLen == (NDIS_802_11_LENGTH_SSID+1))
				dataLen = NDIS_802_11_LENGTH_SSID;
            if (dataLen > 0 && dataLen <= NDIS_802_11_LENGTH_SSID)
            {
            	Set_WscSsid_Proc(pAd, (RTMP_STRING *) extra);
            }
            else
                Status = -E2BIG;
            break;
		case WSC_SET_PIN:
        	DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_PIN, value = (%s)\n", __FUNCTION__, extra));
			if ( dataLen > 0 )
			{
				if (Set_WscPinCode_Proc(pAd, extra) == FALSE)
					Status = -EINVAL;
			}
            else
                Status = -EINVAL;
            break;
		case WSC_SET_BSSID:
			if ( dataLen > 0 )
			{
				if (Set_WscBssid_Proc(pAd, (RTMP_STRING *) extra) == FALSE)
					Status = -EINVAL;
			}
            else
                Status = -EINVAL;
			DBGPRINT(RT_DEBUG_TRACE, ("%s - WSC_SET_BSSID\n", __FUNCTION__));
			break;
        default:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - unknow subcmd = %d\n", __FUNCTION__, subcmd));
            break;
    }
    
    return Status;
#endif /* 0 */

	pIoctlWscStr->Subcmd = dwrq->flags;
	pIoctlWscStr->pData = (CHAR *)extra;
	pIoctlWscStr->length = dwrq->length;

	pIoctlWscStr->Status = RTMP_STA_IoctlHandle(pAd, NULL,
						CMD_RTPRIV_IOCTL_STA_IW_SET_WSC_STR_ITEM, 0,
						pIoctlWscStr, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	RT_CMD_STATUS_TRANSLATE(pIoctlWscStr->Status);
    return pIoctlWscStr->Status;
}
#endif /* WSC_STA_SUPPORT */

static int
rt_private_get_statistics(struct net_device *dev, struct iw_request_info *info,
		struct iw_point *wrq, char *extra)
{
	INT				Status = 0;
    VOID   *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}

    if (extra == NULL)
    {
        wrq->length = 0;
        return -EIO;
    }
    
    memset(extra, 0x00, IW_PRIV_SIZE_MASK);

#if 0 /* os abl move */
    sprintf(extra, "\n\n");

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
	{
	    sprintf(extra+strlen(extra), "Tx success                      = %ld\n", (ULONG)pAd->ate.TxDoneCount);
	    /*sprintf(extra+strlen(extra), "Tx success without retry        = %ld\n", (ULONG)pAd->ate.TxDoneCount); */
	}
	else
#endif /* CONFIG_ATE */
	{
	    sprintf(extra+strlen(extra), "Tx success                      = %lu\n", (ULONG)pAd->WlanCounters.TransmittedFragmentCount.u.LowPart);
	}
    sprintf(extra+strlen(extra), "Tx retry count          		  = %lu\n", (ULONG)pAd->WlanCounters.RetryCount.u.LowPart);
    sprintf(extra+strlen(extra), "Tx fail to Rcv ACK after retry  = %lu\n", (ULONG)pAd->WlanCounters.FailedCount.u.LowPart);
    sprintf(extra+strlen(extra), "RTS Success Rcv CTS             = %lu\n", (ULONG)pAd->WlanCounters.RTSSuccessCount.u.LowPart);
    sprintf(extra+strlen(extra), "RTS Fail Rcv CTS                = %lu\n", (ULONG)pAd->WlanCounters.RTSFailureCount.u.LowPart);

    sprintf(extra+strlen(extra), "Rx success                      = %lu\n", (ULONG)pAd->WlanCounters.ReceivedFragmentCount.QuadPart);
    sprintf(extra+strlen(extra), "Rx with CRC                     = %lu\n", (ULONG)pAd->WlanCounters.FCSErrorCount.u.LowPart);
    sprintf(extra+strlen(extra), "Rx drop due to out of resource  = %lu\n", (ULONG)pAd->Counters8023.RxNoBuffer);
    sprintf(extra+strlen(extra), "Rx duplicate frame              = %lu\n", (ULONG)pAd->WlanCounters.FrameDuplicateCount.u.LowPart);

    sprintf(extra+strlen(extra), "False CCA (one second)          = %lu\n", (ULONG)pAd->RalinkCounters.OneSecFalseCCACnt);

#ifdef CONFIG_ATE
	if (ATE_ON(pAd))
	{
		if (pAd->ate.RxAntennaSel == 0)
		{
    		sprintf(extra+strlen(extra), "RSSI-A                          = %ld\n", (LONG)(pAd->ate.LastRssi0 - pAd->BbpRssiToDbmDelta));
			sprintf(extra+strlen(extra), "RSSI-B (if available)           = %ld\n", (LONG)(pAd->ate.LastRssi1 - pAd->BbpRssiToDbmDelta));
			sprintf(extra+strlen(extra), "RSSI-C (if available)           = %ld\n\n", (LONG)(pAd->ate.LastRssi2 - pAd->BbpRssiToDbmDelta));
		}
		else
		{
    		sprintf(extra+strlen(extra), "RSSI                            = %ld\n", (LONG)(pAd->ate.LastRssi0 - pAd->BbpRssiToDbmDelta));
		}
	}
	else
#endif /* CONFIG_ATE */
	{
    		    	
		sprintf(extra+strlen(extra), "RSSI-A                          = %ld\n", (LONG)(pAd->StaCfg.RssiSample.AvgRssi[0] - pAd->BbpRssiToDbmDelta));
		sprintf(extra+strlen(extra), "RSSI-B (if available)           = %ld\n", (LONG)(pAd->StaCfg.RssiSample.AvgRssi[1] - pAd->BbpRssiToDbmDelta));
        	sprintf(extra+strlen(extra), "RSSI-C (if available)           = %ld\n\n", (LONG)(pAd->StaCfg.RssiSample.AvgRssi[2] - pAd->BbpRssiToDbmDelta));
		sprintf(extra+strlen(extra), "SNR-A                          = %ld\n", (LONG)(pAd->StaCfg.RssiSample.AvgSnr[0]));
        	sprintf(extra+strlen(extra), "SNR-B (if available)           = %ld\n\n", (LONG)(pAd->StaCfg.RssiSample.AvgSnr[1]));		}   
#ifdef WPA_SUPPLICANT_SUPPORT
    sprintf(extra+strlen(extra), "WpaSupplicantUP                 = %d\n\n", pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP);
#endif /* WPA_SUPPLICANT_SUPPORT */

#ifdef DOT11R_FT_SUPPORT
	sprintf(extra+strlen(extra), "FtSupport                       = %d\n\n", pAd->StaCfg.Dot11RCommInfo.bFtSupport);
#endif /* DOT11R_FT_SUPPORT */

	/* display pin code */
	sprintf(extra+strlen(extra), "RT2860 Linux STA PinCode\t%08u\n", GenerateWpsPinCode(pAd, FALSE, BSS0));
#ifdef WSC_STA_SUPPORT
{
	char	mode_str[16]={0};
	ULONG	wps_status, wps_state;
    int     idx = 0;

	wps_state = pAd->StaCfg.WscControl.WscState;
	wps_status = pAd->StaCfg.WscControl.WscStatus;
	
	if (pAd->StaCfg.WscControl.WscMode == WSC_PIN_MODE)
		sprintf(mode_str, "PIN -");
	else
		sprintf(mode_str, "PBC -");
	
		sprintf(extra+strlen(extra), "WPS Information(Driver Auto-Connect is %s - %d):\n",
	                                                  pAd->StaCfg.WscControl.WscDriverAutoConnect ? "Enabled":"Disabled",
	                                                  pAd->StaCfg.WscControl.WscDriverAutoConnect);
	/* display pin code */
	/*sprintf(extra+strlen(extra), "RT2860 Linux STA PinCode\t%08u\n", pAd->StaCfg.WscControl.WscEnrolleePinCode); */
	/* display status */
	if ((wps_state == WSC_STATE_OFF) || (wps_status & 0xff00))
	{
		if (wps_status == STATUS_WSC_CONFIGURED)
		{
			sprintf(extra+strlen(extra), "WPS messages exchange successfully !!!\n");
		}
		else if ((wps_status == STATUS_WSC_NOTUSED))
		{
			sprintf(extra+strlen(extra), "WPS not used.\n");
		}
		else if(wps_status & 0xff00)	/* error message */
		{
			if (wps_status == STATUS_WSC_PBC_TOO_MANY_AP)
				sprintf(extra+strlen(extra), "%s Too many PBC AP. Please wait... \n", mode_str);
			else if (wps_status == STATUS_WSC_PBC_NO_AP)
				sprintf(extra+strlen(extra), "%s No available PBC AP. Please wait... \n", mode_str);
			else if (wps_status & 0x0100)
				sprintf(extra+strlen(extra), "%s Proceed to get the Registrar profile. Please wait... \n", mode_str);
			else	/* status of eap failed */
				sprintf(extra+strlen(extra), "WPS didn't complete !!!\n");
		}
		else
		{
			/* wrong state */
		}
	}
	else
	{
		sprintf(extra+strlen(extra), "%s WPS Proceed. Please wait... \n", mode_str);
	}
	sprintf(extra+strlen(extra), "\n");
    sprintf(extra+strlen(extra), "WPS Profile Count               = %d\n", pAd->StaCfg.WscControl.WscProfile.ProfileCnt);
    for (idx = 0; idx < pAd->StaCfg.WscControl.WscProfile.ProfileCnt ; idx++)
    {
        PWSC_CREDENTIAL pCredential = &pAd->StaCfg.WscControl.WscProfile.Profile[idx];

        if (strlen(extra) + sizeof(WSC_CREDENTIAL) >= IW_PRIV_SIZE_MASK)
        {
            break;
        }
        
        sprintf(extra+strlen(extra), "Profile[%d]:\n", idx);        
        sprintf(extra+strlen(extra), "SSID                            = %s\n", pCredential->SSID.Ssid);
        sprintf(extra+strlen(extra), "MAC                             = %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                                                           pCredential->MacAddr[0],
                                                                           pCredential->MacAddr[1],
                                                                           pCredential->MacAddr[2],
                                                                           pCredential->MacAddr[3],
                                                                           pCredential->MacAddr[4],
                                                                           pCredential->MacAddr[5]);
        sprintf(extra+strlen(extra), "AuthType                        = %s\n", WscGetAuthTypeStr(pCredential->AuthType));
        sprintf(extra+strlen(extra), "EncrypType                      = %s\n", WscGetEncryTypeStr(pCredential->EncrType)); 
        sprintf(extra+strlen(extra), "KeyIndex                        = %d\n", pCredential->KeyIndex);
        if (pCredential->KeyLength != 0)
        {
            if (pCredential->AuthType & (WSC_AUTHTYPE_WPAPSK | WSC_AUTHTYPE_WPA2PSK | WSC_AUTHTYPE_WPANONE))
            {
            if (pCredential->KeyLength < 64)
                sprintf(extra+strlen(extra), "Key                             = %s\n", pCredential->Key);
            else
            {
                char key_print[65] = {0};
                NdisMoveMemory(key_print, pCredential->Key, 64);
                sprintf(extra+strlen(extra), "Key                             = %s\n", key_print);
            }
        }
            else if ((pCredential->AuthType == WSC_AUTHTYPE_OPEN) ||
                     (pCredential->AuthType == WSC_AUTHTYPE_SHARED))
            {
                /*check key string is ASCII or not */
                if (RTMPCheckStrPrintAble((PCHAR)pCredential->Key, (UCHAR)pCredential->KeyLength))
                    sprintf(extra+strlen(extra), "Key                             = %s\n", pCredential->Key);
                else
                {
                    int idx;
                    sprintf(extra+strlen(extra), "Key                             = ");
                    for (idx = 0; idx < pCredential->KeyLength; idx++)
                        sprintf(extra+strlen(extra), "%02X", pCredential->Key[idx]);
                    sprintf(extra+strlen(extra), "\n");
                }
            }
        }
    }
    sprintf(extra+strlen(extra), "\n");
}
#endif /* WSC_STA_SUPPORT */
#endif /* 0 */

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_IW_GET_STATISTICS, 0,
						extra, IW_PRIV_SIZE_MASK, RT_DEV_PRIV_FLAGS_GET(dev));

	wrq->length = (USHORT)(strlen(extra) + 1); /* 1: size of '\0' */
    DBGPRINT(RT_DEBUG_TRACE, ("<== rt_private_get_statistics, wrq->length = %d\n", wrq->length));

    return Status;
}

#if 0 /* os abl move to sta/sta_cfg.c */
#ifdef DOT11_N_SUPPORT
void	getBaInfo(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_STRING *pOutBuf)
{
	INT i, j;
	BA_ORI_ENTRY *pOriBAEntry;
	BA_REC_ENTRY *pRecBAEntry;

	for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
	{
		PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
		if (((IS_ENTRY_CLIENT(pEntry) || IS_ENTRY_APCLI(pEntry) || IS_ENTRY_TDLS(pEntry)) && (pEntry->Sst == SST_ASSOC))
			|| IS_ENTRY_WDS(pEntry) || IS_ENTRY_MESH(pEntry))
		{		
			sprintf(pOutBuf, "%s\n%02X:%02X:%02X:%02X:%02X:%02X (Aid = %d) (AP) -\n",
                pOutBuf,
				pEntry->Addr[0], pEntry->Addr[1], pEntry->Addr[2],
				pEntry->Addr[3], pEntry->Addr[4], pEntry->Addr[5], pEntry->Aid);
			
			sprintf(pOutBuf, "%s[Recipient]\n", pOutBuf);
			for (j=0; j < NUM_OF_TID; j++)
			{
				if (pEntry->BARecWcidArray[j] != 0)
				{
					pRecBAEntry =&pAd->BATable.BARecEntry[pEntry->BARecWcidArray[j]];
					sprintf(pOutBuf, "%sTID=%d, BAWinSize=%d, LastIndSeq=%d, ReorderingPkts=%d\n", pOutBuf, j, pRecBAEntry->BAWinSize, pRecBAEntry->LastIndSeq, pRecBAEntry->list.qlen);
				}
			}
			sprintf(pOutBuf, "%s\n", pOutBuf);

			sprintf(pOutBuf, "%s[Originator]\n", pOutBuf);
			for (j=0; j < NUM_OF_TID; j++)
			{
				if (pEntry->BAOriWcidArray[j] != 0)
				{
					pOriBAEntry =&pAd->BATable.BAOriEntry[pEntry->BAOriWcidArray[j]];
					sprintf(pOutBuf, "%sTID=%d, BAWinSize=%d, StartSeq=%d, CurTxSeq=%d\n",
								pOutBuf, j, pOriBAEntry->BAWinSize, pOriBAEntry->Sequence,
								pAd->MacTab.tr_entry[pEntry->wcid].TxSeq[j]);
				}
			}
			sprintf(pOutBuf, "%s\n\n", pOutBuf);
		}
        if (strlen(pOutBuf) > (IW_PRIV_SIZE_MASK - 100))
                break;
	}

	return;
}
#endif /* DOT11_N_SUPPORT */
#endif /* 0 */

static int
rt_private_show(struct net_device *dev, struct iw_request_info *info,
		struct iw_point *wrq, RTMP_STRING *extra)
{
	RTMP_IOCTL_INPUT_STRUCT wrqin;
	INT				Status = 0;
	VOID   			*pAd;
/*	POS_COOKIE		pObj; */
	USHORT             subcmd = wrq->flags;
	RT_CMD_STA_IOCTL_SHOW IoctlShow, *pIoctlShow = &IoctlShow;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#if 0
	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}
#endif

/*	pObj = (POS_COOKIE) pAd->OS_Cookie; */
	if (extra == NULL)
	{
		wrq->length = 0;
		return -EIO;
	}
	memset(extra, 0x00, IW_PRIV_SIZE_MASK);
    
#if 0 /* os abl move */
#ifdef MESH_SUPPORT
	if (RT_DEV_PRIV_FLAGS_GET(dev) == INT_MESH)
	{
		pObj->ioctl_if_type = INT_MESH;
		pObj->ioctl_if = 0;
	}
	else
#endif /* MESH_SUPPORT */
	{
		pObj->ioctl_if_type = INT_MAIN;
		pObj->ioctl_if = MAIN_MBSSID;
	}

    switch(subcmd)
    {
#ifdef ETH_CONVERT_SUPPORT
#ifdef MAT_SUPPORT
        case SHOW_IPV4_MAT_INFO:
            {
                extern VOID getIPMacTbInfo(MAT_STRUCT *, char *);
                getIPMacTbInfo(&pAd->MatCfg, extra);
                wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            }
            break;
        case SHOW_IPV6_MAT_INFO:
            {
                extern VOID getIPv6MacTbInfo(MAT_STRUCT *, char *);
                getIPv6MacTbInfo(&pAd->MatCfg, extra);
                wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            }
            break;
        case SHOW_ETH_CLONE_MAC:
            sprintf(extra, "%02X:%02X:%02X:%02X:%02X:%02X\n", pAd->EthConvert.EthCloneMac[0],
                                                              pAd->EthConvert.EthCloneMac[1],
                                                              pAd->EthConvert.EthCloneMac[2],
                                                              pAd->EthConvert.EthCloneMac[3],
                                                              pAd->EthConvert.EthCloneMac[4],
                                                              pAd->EthConvert.EthCloneMac[5]);
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;
#endif /* MAT_SUPPORT */
#endif /* ETH_CONVERT_SUPPORT */

        case SHOW_CONN_STATUS:
            if (MONITOR_ON(pAd))
            {
#ifdef DOT11_N_SUPPORT
                if (WMODE_CAP_N(pAd->CommonCfg.PhyMode) &&
                    pAd->CommonCfg.RegTransmitSetting.field.BW)
                    sprintf(extra, "Monitor Mode(CentralChannel %d)\n", pAd->CommonCfg.CentralChannel);
                else
#endif /* DOT11_N_SUPPORT */
                    sprintf(extra, "Monitor Mode(Channel %d)\n", pAd->CommonCfg.Channel);
            }
            else
            {
                if (pAd->IndicateMediaState == NdisMediaStateConnected)
            	{        	    
            	    if (INFRA_ON(pAd))
                    {   
                    sprintf(extra, "Connected(AP: %s[%02X:%02X:%02X:%02X:%02X:%02X])\n", 
                                    pAd->CommonCfg.Ssid, 
                                    pAd->CommonCfg.Bssid[0],
                                    pAd->CommonCfg.Bssid[1],
                                    pAd->CommonCfg.Bssid[2],
                                    pAd->CommonCfg.Bssid[3],
                                    pAd->CommonCfg.Bssid[4],
                                    pAd->CommonCfg.Bssid[5]);
            		DBGPRINT(RT_DEBUG_TRACE ,("Ssid=%s ,Ssidlen = %d\n",pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen));
            	}
                    else if (ADHOC_ON(pAd))
                        sprintf(extra, "Connected\n");
            	}
            	else
            	{
            	    sprintf(extra, "Disconnected\n");
            		DBGPRINT(RT_DEBUG_TRACE ,("ConnStatus is not connected\n"));
            	}
            }
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;
        case SHOW_DRVIER_VERION:
            sprintf(extra, "Driver version-%s, %s %s\n", STA_DRIVER_VERSION, __DATE__, __TIME__ );
#ifdef MESH_SUPPORT
	     			sprintf(extra+strlen(extra), "Support MESH \n");
#endif /* MESH_SUPPORT */
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;
#ifdef DOT11_N_SUPPORT
        case SHOW_BA_INFO:
            getBaInfo(pAd, extra);
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;
#endif /* DOT11_N_SUPPORT */
		case SHOW_DESC_INFO:
			{
				Show_DescInfo_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
#ifdef RELEASE_EXCLUDE
#ifdef RTMP_MAC_USB
		case SHOW_RXBULK_INFO:
			{
				Show_RxBulk_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
			
		case SHOW_TXBULK_INFO:
			{
				Show_TxBulk_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
#endif /* RTMP_MAC_USB */
#endif /* RELEASE_EXCLUDE */
        case RAIO_OFF:
            if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS))
            {
                if (pAd->Mlme.CntlMachine.CurrState != CNTL_IDLE)
		        {
		            RTMP_MLME_RESET_STATE_MACHINE(pAd);
		            DBGPRINT(RT_DEBUG_TRACE, ("!!! MLME busy, reset MLME state machine !!!\n"));
		        }
            }
            pAd->StaCfg.bSwRadio = FALSE;
            if (pAd->StaCfg.bRadio != (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio))
            {
                pAd->StaCfg.bRadio = (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio);
                if (pAd->StaCfg.bRadio == FALSE)
                {
					RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_CMD_RADIO_OFF);
                    MlmeRadioOff(pAd);
                    /* Update extra information */
					pAd->ExtraInfo = SW_RADIO_OFF;
                } 
            }
            sprintf(extra, "Radio Off\n");
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;
        case RAIO_ON:
            pAd->StaCfg.bSwRadio = TRUE;
            /*if (pAd->StaCfg.bRadio != (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio)) */
            {
                pAd->StaCfg.bRadio = (pAd->StaCfg.bHwRadio && pAd->StaCfg.bSwRadio);
                if (pAd->StaCfg.bRadio == TRUE)
                {
					RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_CMD_RADIO_OFF);
                    MlmeRadioOn(pAd);
                    /* Update extra information */
					pAd->ExtraInfo = EXTRA_INFO_CLEAR;
                }
            }
            sprintf(extra, "Radio On\n");
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;

#ifdef MESH_SUPPORT
		case SHOW_MESH_INFO:
			{
				Set_MeshInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;

		case SHOW_NEIGHINFO_INFO:
			{
				Set_NeighborInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;

		case SHOW_MESH_ROUTE_INFO:
			{
				Set_MeshRouteInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;

		case SHOW_MESH_ENTRY_INFO:
			{
				Set_MeshEntryInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;

		case SHOW_MULPATH_INFO:
			{
				Set_MultipathInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;

		case SHOW_MCAST_AGEOUT_INFO:
			{
				Set_MultiCastAgeOut_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;

		case SHOW_MESH_PKTSIG_INFO:
			{
				Set_PktSig_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
			
		case SHOW_MESH_PROXY_INFO:
			{
				Set_MeshProxyInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
#endif /* MESH_SUPPORT */

#ifdef QOS_DLS_SUPPORT
		case SHOW_DLS_ENTRY_INFO:
			{
				Set_DlsEntryInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
#endif /* QOS_DLS_SUPPORT */

#ifdef DOT11Z_TDLS_SUPPORT
		case SHOW_TDLS_ENTRY_INFO:
			{
				Set_TdlsEntryInfo_Display_Proc(pAd, NULL);
				wrq->length = 0; /* 1: size of '\0' */
			}
			break;
#endif /* DOT11Z_TDLS_SUPPORT */

		case SHOW_CFG_VALUE:
			{
				Status = RTMPShowCfgValue(pAd, (RTMP_STRING *) wrq->pointer, extra, IW_PRIV_SIZE_MASK);
				if (Status == 0)
					wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
			}
			break;
		case SHOW_ADHOC_ENTRY_INFO:
			Show_Adhoc_MacTable_Proc(pAd, extra);
			wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
			break;

#ifdef WMM_ACM_SUPPORT
#if 0
        case SHOW_ACM_BADNWIDTH:
            AcmCmdBandwidthGuiDisplay(pAd, extra);
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            break;
#endif            
        case SHOW_ACM_STREAM:
            AcmCmdStreamGuiDisplay(pAd, extra);
            wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
            printk("SHOW_ACM_STREAM - wrq->length = %d\n", wrq->length);
            break;

#endif
        default:
            DBGPRINT(RT_DEBUG_TRACE, ("%s - unknow subcmd = %d\n", __FUNCTION__, subcmd));
            break;
    }
#endif /* 0 */
    
	wrqin.u.data.pointer = wrq->pointer;
	wrqin.u.data.length = wrq->length;

	pIoctlShow->pData = (CHAR *)extra;
	pIoctlShow->MaxSize = IW_PRIV_SIZE_MASK;
	pIoctlShow->InfType = RT_DEV_PRIV_FLAGS_GET(dev);
	RTMP_STA_IoctlHandle(pAd, &wrqin, CMD_RTPRIV_IOCTL_SHOW, subcmd,
						pIoctlShow, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	wrq->length = wrqin.u.data.length;
    return Status;
}

#ifdef SIOCSIWMLME
int rt_ioctl_siwmlme(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu,
			   char *extra)
{
	VOID   *pAd = NULL;
	struct iw_mlme *pMlme = (struct iw_mlme *)wrqu->data.pointer;
/*	MLME_QUEUE_ELEM				MsgElem; */
/*	MLME_QUEUE_ELEM				*pMsgElem = NULL; */
/*	MLME_DISASSOC_REQ_STRUCT	DisAssocReq; */
/*	MLME_DEAUTH_REQ_STRUCT      DeAuthReq; */
	USHORT Subcmd = 0;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	DBGPRINT(RT_DEBUG_TRACE, ("====> %s\n", __FUNCTION__));

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}

	if (pMlme == NULL)
		return -EINVAL;

#if 0 /* os abl move */
	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&pMsgElem, sizeof(MLME_QUEUE_ELEM));
	if (pMsgElem == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		return -ENOMEM;
	}

	switch(pMlme->cmd)
	{
#ifdef IW_MLME_DEAUTH	
		case IW_MLME_DEAUTH:
			DBGPRINT(RT_DEBUG_TRACE, ("====> %s - IW_MLME_DEAUTH\n", __FUNCTION__));			                
			COPY_MAC_ADDR(DeAuthReq.Addr, pAd->CommonCfg.Bssid);
			DeAuthReq.Reason = pMlme->reason_code;
			pMsgElem->MsgLen = sizeof(MLME_DEAUTH_REQ_STRUCT);
			NdisMoveMemory(pMsgElem->Msg, &DeAuthReq, sizeof(MLME_DEAUTH_REQ_STRUCT));
			MlmeDeauthReqAction(pAd, pMsgElem);
			if (INFRA_ON(pAd))
			{
			    LinkDown(pAd, FALSE);
			    pAd->Mlme.AssocMachine.CurrState = ASSOC_IDLE;
			}
			break;
#endif /* IW_MLME_DEAUTH */
#ifdef IW_MLME_DISASSOC
		case IW_MLME_DISASSOC:
			DBGPRINT(RT_DEBUG_TRACE, ("====> %s - IW_MLME_DISASSOC\n", __FUNCTION__));
			COPY_MAC_ADDR(DisAssocReq.Addr, pAd->CommonCfg.Bssid);
			DisAssocReq.Reason =  pMlme->reason_code;

			pMsgElem->Machine = ASSOC_STATE_MACHINE;
			pMsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
			pMsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
			NdisMoveMemory(pMsgElem->Msg, &DisAssocReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

			pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_DISASSOC;
			MlmeDisassocReqAction(pAd, pMsgElem);
			break;
#endif /* IW_MLME_DISASSOC */
		default:
			DBGPRINT(RT_DEBUG_TRACE, ("====> %s - Unknow Command\n", __FUNCTION__));
			break;
	}
	
	if (pMsgElem != NULL)
		os_free_mem(NULL, pMsgElem);
#endif /* 0 */

	switch(pMlme->cmd)
	{
#ifdef IW_MLME_DEAUTH	
		case IW_MLME_DEAUTH:
			Subcmd = RT_CMD_STA_IOCTL_IW_MLME_DEAUTH;
			break;
#endif /* IW_MLME_DEAUTH */
#ifdef IW_MLME_DISASSOC
		case IW_MLME_DISASSOC:
			Subcmd = RT_CMD_STA_IOCTL_IW_MLME_DISASSOC;
			break;
#endif /* IW_MLME_DISASSOC */
		default:
			DBGPRINT(RT_DEBUG_TRACE, ("====> %s - Unknow Command\n", __FUNCTION__));
			break;
	}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWMLME, Subcmd,
		info, pMlme->reason_code, RT_DEV_PRIV_FLAGS_GET(dev));
	return 0;
}
#endif /* SIOCSIWMLME */

#if WIRELESS_EXT > 17

#ifdef WEXT_WPS
#ifndef IW_AUTH_KEY_MGMT_WPS
#define IW_AUTH_KEY_MGMT_WPS 4
#endif /* !IW_AUTH_KEY_MGMT_WPS */
#endif /* WEXT_WPS */

int rt_ioctl_siwauth(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	VOID   *pAd = NULL;
	struct iw_param *param = &wrqu->param;
	RT_CMD_STA_IOCTL_SECURITY_ADV IoctlWpa, *pIoctlWpa = &IoctlWpa;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
  		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
    	return -ENETDOWN;   
	}

#if 0 /* os abl move */
	switch (param->flags & IW_AUTH_INDEX) {
    	case IW_AUTH_WPA_VERSION:
            if (param->value == IW_AUTH_WPA_VERSION_WPA)
            {            
                pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPAPSK;
				if (pAd->StaCfg.BssType == BSS_ADHOC)
					pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPANone;
            }
            else if (param->value == IW_AUTH_WPA_VERSION_WPA2)
                pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPA2PSK;
			
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_WPA_VERSION - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_CIPHER_PAIRWISE:
            if (param->value == IW_AUTH_CIPHER_NONE)
            {
                pAd->StaCfg.WepStatus = Ndis802_11WEPDisabled;
                pAd->StaCfg.PairCipher = Ndis802_11WEPDisabled;
            }
            else if (param->value == IW_AUTH_CIPHER_WEP40 ||
                     param->value == IW_AUTH_CIPHER_WEP104)
            {
                pAd->StaCfg.WepStatus = Ndis802_11WEPEnabled;
                pAd->StaCfg.PairCipher = Ndis802_11WEPEnabled;
#ifdef WPA_SUPPLICANT_SUPPORT                
                pAd->StaCfg.wdev.IEEE8021X = FALSE;
#endif /* WPA_SUPPLICANT_SUPPORT */
            }
            else if (param->value == IW_AUTH_CIPHER_TKIP)
            {
                pAd->StaCfg.WepStatus = Ndis802_11TKIPEnable;
                pAd->StaCfg.PairCipher = Ndis802_11TKIPEnable;
            }
            else if (param->value == IW_AUTH_CIPHER_CCMP)
            {
                pAd->StaCfg.WepStatus = Ndis802_11AESEnable;
                pAd->StaCfg.PairCipher = Ndis802_11AESEnable;
            }
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_CIPHER_PAIRWISE - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_CIPHER_GROUP:
            if (param->value == IW_AUTH_CIPHER_NONE)
            {
                pAd->StaCfg.GroupCipher = Ndis802_11WEPDisabled;
            }
            else if (param->value == IW_AUTH_CIPHER_WEP40)
            {
                pAd->StaCfg.GroupCipher = Ndis802_11GroupWEP40Enabled;
            }
			else if (param->value == IW_AUTH_CIPHER_WEP104)
            {
				pAd->StaCfg.GroupCipher = Ndis802_11GroupWEP104Enabled;
            }
            else if (param->value == IW_AUTH_CIPHER_TKIP)
            {
                pAd->StaCfg.GroupCipher = Ndis802_11TKIPEnable;
            }
            else if (param->value == IW_AUTH_CIPHER_CCMP)
            {
                pAd->StaCfg.GroupCipher = Ndis802_11AESEnable;
            }
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_CIPHER_GROUP - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_KEY_MGMT:
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
			pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP &= 0x7F;
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
            if (param->value == IW_AUTH_KEY_MGMT_802_1X)
            { 
                if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK)
                {
                    pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPA;
#ifdef WPA_SUPPLICANT_SUPPORT                    
                    pAd->StaCfg.wdev.IEEE8021X = FALSE;
#endif /* WPA_SUPPLICANT_SUPPORT */
                }
                else if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)
                {
                    pAd->StaCfg.AuthMode = Ndis802_11AuthModeWPA2;
#ifdef WPA_SUPPLICANT_SUPPORT
                    pAd->StaCfg.wdev.IEEE8021X = FALSE;
#endif /* WPA_SUPPLICANT_SUPPORT */
                }
#ifdef WPA_SUPPLICANT_SUPPORT                
                else
                    /* WEP 1x */
                    pAd->StaCfg.wdev.IEEE8021X = TRUE;
#endif /* WPA_SUPPLICANT_SUPPORT */                
            }
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
#ifdef WEXT_WPS
			else if (param->value == IW_AUTH_KEY_MGMT_WPS)
			{
				pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP |= WPA_SUPPLICANT_ENABLE_WPS;
			}
#endif /* WEXT_WPS */
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
            else if (param->value == 0)
            {
                /*pAd->StaCfg.PortSecured = WPA_802_1X_PORT_SECURED; */
				STA_PORT_SECURED(pAd);
            }
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_KEY_MGMT - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
            break;
    	case IW_AUTH_PRIVACY_INVOKED:
            /*if (param->value == 0)
			{
                pAd->StaCfg.AuthMode = Ndis802_11AuthModeOpen;
                pAd->StaCfg.WepStatus = Ndis802_11WEPDisabled;
                pAd->StaCfg.PairCipher = Ndis802_11WEPDisabled;
        	    pAd->StaCfg.GroupCipher = Ndis802_11WEPDisabled;
            }*/            
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_PRIVACY_INVOKED - param->value = %d!\n", __FUNCTION__, param->value));
    		break;
    	case IW_AUTH_DROP_UNENCRYPTED:
            if (param->value != 0)
                pAd->StaCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
			else
			{
                /*pAd->StaCfg.PortSecured = WPA_802_1X_PORT_SECURED; */
				STA_PORT_SECURED(pAd);
			}
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_DROP_UNENCRYPTED - param->value = %d!\n", __FUNCTION__, param->value));
    		break;
    	case IW_AUTH_80211_AUTH_ALG: 
			if (param->value == IW_AUTH_ALG_OPEN_SYSTEM)
				pAd->StaCfg.AuthMode = Ndis802_11AuthModeOpen;
			else if (param->value == IW_AUTH_ALG_SHARED_KEY)
				pAd->StaCfg.AuthMode = Ndis802_11AuthModeShared;
            else
				pAd->StaCfg.AuthMode = Ndis802_11AuthModeAutoSwitch;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_80211_AUTH_ALG - param->value = %d!\n", __FUNCTION__, param->value));
			break;
    	case IW_AUTH_WPA_ENABLED:
    		DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_WPA_ENABLED - Driver supports WPA!(param->value = %d)\n", __FUNCTION__, param->value));
    		break;
    	default:
    		return -EOPNOTSUPP;
}
#endif /* 0 */

	pIoctlWpa->flags = 0;
	pIoctlWpa->value = param->value; /* default */

	switch (param->flags & IW_AUTH_INDEX) {
    	case IW_AUTH_WPA_VERSION:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_VERSION;
            if (param->value == IW_AUTH_WPA_VERSION_WPA)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_VERSION1;
            else if (param->value == IW_AUTH_WPA_VERSION_WPA2)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_VERSION2;

            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_WPA_VERSION - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_CIPHER_PAIRWISE:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_PAIRWISE;
            if (param->value == IW_AUTH_CIPHER_NONE)
                pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_PAIRWISE_NONE;
            else if (param->value == IW_AUTH_CIPHER_WEP40)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_PAIRWISE_WEP40;
            else if (param->value == IW_AUTH_CIPHER_WEP104)
                pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_PAIRWISE_WEP104;
            else if (param->value == IW_AUTH_CIPHER_TKIP)
                pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_PAIRWISE_TKIP;
            else if (param->value == IW_AUTH_CIPHER_CCMP)
                pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_PAIRWISE_CCMP;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_CIPHER_PAIRWISE - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_CIPHER_GROUP:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_GROUP;
            if (param->value == IW_AUTH_CIPHER_NONE)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_GROUP_NONE;
            else if (param->value == IW_AUTH_CIPHER_WEP40)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_GROUP_WEP40;
			else if (param->value == IW_AUTH_CIPHER_WEP104)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_GROUP_WEP104;
            else if (param->value == IW_AUTH_CIPHER_TKIP)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_GROUP_TKIP;
            else if (param->value == IW_AUTH_CIPHER_CCMP)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_GROUP_CCMP;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_CIPHER_GROUP - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_KEY_MGMT:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_KEY_MGMT;
            if (param->value == IW_AUTH_KEY_MGMT_802_1X)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_KEY_MGMT_1X;
#ifdef WEXT_WPS
			else if (param->value == IW_AUTH_KEY_MGMT_WPS)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_KEY_MGMT_WPS;
#endif /* WEXT_WPS */
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_KEY_MGMT - param->value = %d!\n", __FUNCTION__, param->value));
            break;
    	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_RX_UNENCRYPTED_EAPOL;
            break;
    	case IW_AUTH_PRIVACY_INVOKED:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_PRIVACY_INVOKED;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_PRIVACY_INVOKED - param->value = %d!\n", __FUNCTION__, param->value));
    		break;
    	case IW_AUTH_DROP_UNENCRYPTED:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_DROP_UNENCRYPTED;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_DROP_UNENCRYPTED - param->value = %d!\n", __FUNCTION__, param->value));
    		break;
    	case IW_AUTH_80211_AUTH_ALG: 
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_80211_AUTH_ALG;
			if (param->value & IW_AUTH_ALG_SHARED_KEY) 
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_AUTH_80211_AUTH_ALG_SHARED;
            else if (param->value & IW_AUTH_ALG_OPEN_SYSTEM)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_AUTH_80211_AUTH_ALG_OPEN;
            else if (param->value & IW_AUTH_ALG_LEAP)
				pIoctlWpa->value = RT_CMD_STA_IOCTL_WPA_AUTH_80211_AUTH_ALG_LEAP;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_80211_AUTH_ALG - param->value = %d!\n", __FUNCTION__, param->value));
			break;
    	case IW_AUTH_WPA_ENABLED:
			pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_WPA_ENABLED;
    		DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_AUTH_WPA_ENABLED - Driver supports WPA!(param->value = %d)\n", __FUNCTION__, param->value));
    		break;
    	default:
    		return -EOPNOTSUPP;
}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWAUTH, 0,
						pIoctlWpa, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	return 0;
}

int rt_ioctl_giwauth(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	VOID   *pAd = NULL;
	struct iw_param *param = &wrqu->param;
	RT_CMD_STA_IOCTL_SECURITY_ADV IoctlWpa, *pIoctlWpa = &IoctlWpa;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
  		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
    	return -ENETDOWN;   
    }

#if 0 /* os abl move */
	switch (param->flags & IW_AUTH_INDEX) {
	case IW_AUTH_DROP_UNENCRYPTED:
        param->value = (pAd->StaCfg.WepStatus == Ndis802_11WEPDisabled) ? 0 : 1;
		break;

	case IW_AUTH_80211_AUTH_ALG:
        param->value = (pAd->StaCfg.AuthMode == Ndis802_11AuthModeShared) ? IW_AUTH_ALG_SHARED_KEY : IW_AUTH_ALG_OPEN_SYSTEM;
		break;

	case IW_AUTH_WPA_ENABLED:
		param->value = (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA) ? 1 : 0;
		break;

	default:
		return -EOPNOTSUPP;
	}
#endif /* 0 */

	pIoctlWpa->flags = 0;
	pIoctlWpa->value = 0;

	switch (param->flags & IW_AUTH_INDEX) {
	case IW_AUTH_DROP_UNENCRYPTED:
		pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_DROP_UNENCRYPTED;
		break;

	case IW_AUTH_80211_AUTH_ALG:
		pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_80211_AUTH_ALG;
		break;

	case IW_AUTH_WPA_ENABLED:
		pIoctlWpa->flags = RT_CMD_STA_IOCTL_WPA_AUTH_WPA_ENABLED;
		break;

	default:
		return -EOPNOTSUPP;
	}

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWAUTH, 0,
						pIoctlWpa, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	switch (param->flags & IW_AUTH_INDEX) {
	case IW_AUTH_DROP_UNENCRYPTED:
		param->value = pIoctlWpa->value;
		break;

	case IW_AUTH_80211_AUTH_ALG:
		param->value = (pIoctlWpa->value == 0) ? IW_AUTH_ALG_SHARED_KEY : IW_AUTH_ALG_OPEN_SYSTEM;
		break;

	case IW_AUTH_WPA_ENABLED:
		param->value = pIoctlWpa->value;
		break;
	}

    DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_giwauth::param->value = %d!\n", param->value));
	return 0;
}

#if 0 /* os abl move */
void fnSetCipherKey(
    IN  PRTMP_ADAPTER   pAd,
    IN  INT             keyIdx,    
    IN  UCHAR           CipherAlg,
    IN  BOOLEAN         bGTK,
    IN  struct iw_encode_ext *ext)
{
    NdisZeroMemory(&pAd->SharedKey[BSS0][keyIdx], sizeof(CIPHER_KEY));
    pAd->SharedKey[BSS0][keyIdx].KeyLen = LEN_TK;
    NdisMoveMemory(pAd->SharedKey[BSS0][keyIdx].Key, ext->key, LEN_TK);
    NdisMoveMemory(pAd->SharedKey[BSS0][keyIdx].TxMic, ext->key + LEN_TK, LEN_TKIP_MIC);
    NdisMoveMemory(pAd->SharedKey[BSS0][keyIdx].RxMic, ext->key + LEN_TK + LEN_TKIP_MIC, LEN_TKIP_MIC);
    pAd->SharedKey[BSS0][keyIdx].CipherAlg = CipherAlg;

    /* Update group key information to ASIC Shared Key Table */
	AsicAddSharedKeyEntry(pAd, 
						  BSS0, 
						  keyIdx, 
						  &pAd->SharedKey[BSS0][keyIdx]);
			
	/* Update ASIC WCID attribute table and IVEIV table */
	if (!bGTK)
		RTMPSetWcidSecurityInfo(pAd, 
	    						BSS0, 
	    						keyIdx, 
	    						pAd->SharedKey[BSS0][keyIdx].CipherAlg, 
	       						BSSID_WCID, 
	       						SHAREDKEYTABLE);
}
#endif /* 0 */

int rt_ioctl_siwencodeext(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu,
			   char *extra)
{
	VOID   *pAd = NULL;
	struct iw_point *encoding = &wrqu->encoding;
	struct iw_encode_ext *ext = (struct iw_encode_ext *)extra;
    int /* keyIdx, */ alg = ext->alg;
	RT_CMD_STA_IOCTL_SECURITY IoctlSec, *pIoctlSec = &IoctlSec;
	
	GET_PAD_FROM_NET_DEV(pAd, dev);
	
    /*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
  		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
    	return -ENETDOWN;   
	}

#if 0 /* os abl move */
    if (encoding->flags & IW_ENCODE_DISABLED)
	{
        keyIdx = (encoding->flags & IW_ENCODE_INDEX) - 1;
        /* set BSSID wcid entry of the Pair-wise Key table as no-security mode */
	    AsicRemovePairwiseKeyEntry(pAd, BSSID_WCID);
        pAd->SharedKey[BSS0][keyIdx].KeyLen = 0;
		pAd->SharedKey[BSS0][keyIdx].CipherAlg = CIPHER_NONE;
		AsicRemoveSharedKeyEntry(pAd, 0, (UCHAR)keyIdx);
        NdisZeroMemory(&pAd->SharedKey[BSS0][keyIdx], sizeof(CIPHER_KEY));
        DBGPRINT(RT_DEBUG_TRACE, ("%s::Remove all keys!(encoding->flags = %x)\n", __FUNCTION__, encoding->flags));
    }
	else
    {
        /* Get Key Index and convet to our own defined key index */
    	keyIdx = (encoding->flags & IW_ENCODE_INDEX) - 1;
    	if((keyIdx < 0) || (keyIdx >= NR_WEP_KEYS))
    		return -EINVAL;               
					
        if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
        {
            pAd->StaCfg.DefaultKeyId = keyIdx;
            DBGPRINT(RT_DEBUG_TRACE, ("%s::DefaultKeyId = %d\n", __FUNCTION__, pAd->StaCfg.DefaultKeyId));
        }

        switch (alg) {
    		case IW_ENCODE_ALG_NONE:
                DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_ENCODE_ALG_NONE\n", __FUNCTION__));
    			break;
    		case IW_ENCODE_ALG_WEP:
                DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_ENCODE_ALG_WEP - ext->key_len = %d, keyIdx = %d\n", __FUNCTION__, ext->key_len, keyIdx));
    			if (ext->key_len == MAX_WEP_KEY_SIZE)
                {      
        			pAd->SharedKey[BSS0][keyIdx].KeyLen = MAX_WEP_KEY_SIZE;
                    pAd->SharedKey[BSS0][keyIdx].CipherAlg = CIPHER_WEP128;
				}
        		else if (ext->key_len == MIN_WEP_KEY_SIZE)
                {      
                    pAd->SharedKey[BSS0][keyIdx].KeyLen = MIN_WEP_KEY_SIZE;
                    pAd->SharedKey[BSS0][keyIdx].CipherAlg = CIPHER_WEP64;
				}
        		else
                    return -EINVAL;
                                
                NdisZeroMemory(pAd->SharedKey[BSS0][keyIdx].Key,  16);
			    NdisMoveMemory(pAd->SharedKey[BSS0][keyIdx].Key, ext->key, ext->key_len);

				if ((pAd->StaCfg.GroupCipher == Ndis802_11GroupWEP40Enabled) ||
					(pAd->StaCfg.GroupCipher == Ndis802_11GroupWEP104Enabled))				
				{
					RT_CMD_SHARED_KEY_ADD CmdKey;
					CmdKey.KeyIdx = keyIdx;
					CmdKey.FlgHaveGTK = TRUE;

					/* Set Group key material to Asic */
					AsicAddSharedKeyEntry(pAd, BSS0, keyIdx, &pAd->SharedKey[BSS0][keyIdx]);										

					/* Assign pairwise key info */
					RTMPSetWcidSecurityInfo(pAd,
										 	BSS0, 
										 	keyIdx, 
										 	pAd->SharedKey[BSS0][keyIdx].CipherAlg, 												 
										 	BSSID_WCID, 
										 	SHAREDKEYTABLE);

					STA_PORT_SECURED(pAd);					    				
				}
    			break;
            case IW_ENCODE_ALG_TKIP:
                DBGPRINT(RT_DEBUG_TRACE, ("%s::IW_ENCODE_ALG_TKIP - keyIdx = %d, ext->key_len = %d\n", __FUNCTION__, keyIdx, ext->key_len));
                if (ext->key_len == 32)
                {
                	if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPANone)
                	{
                		RTMPZeroMemory(pAd->StaCfg.PMK, LEN_PMK);
                		RTMPMoveMemory(pAd->StaCfg.PMK, ext->key, ext->key_len);
                	}
					else
					{
	                    if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
	                    {
	                        fnSetCipherKey(pAd, keyIdx, CIPHER_TKIP, FALSE, ext);
	                        if (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA2)
	                        {
	                            STA_PORT_SECURED(pAd);
	                        }
						}
	                    else if (ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY)
	                    {
	                        fnSetCipherKey(pAd, keyIdx, CIPHER_TKIP, TRUE, ext);
			
	                        /* set 802.1x port control */
	            	        STA_PORT_SECURED(pAd);
	                    }
					}
                }
                else
                    return -EINVAL;
                break;
            case IW_ENCODE_ALG_CCMP:
				if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPANone)
            	{
            		RTMPZeroMemory(pAd->StaCfg.PMK, LEN_PMK);
            		RTMPMoveMemory(pAd->StaCfg.PMK, ext->key, ext->key_len);
            	}
				else
				{
	                if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
					{
	                    fnSetCipherKey(pAd, keyIdx, CIPHER_AES, FALSE, ext);
	                    if (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPA2)
	                    	STA_PORT_SECURED(pAd);
	                }
	                else if (ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY)
	                {
	                    fnSetCipherKey(pAd, keyIdx, CIPHER_AES, TRUE, ext);
	                    
	                    /* set 802.1x port control */
	        	        STA_PORT_SECURED(pAd);
	                }
				}
                break;
    		default:
    			return -EINVAL;
		}
    }
#endif /* 0 */
			
	pIoctlSec->pData = (CHAR *)ext->key;
	pIoctlSec->length = ext->key_len;
	pIoctlSec->KeyIdx = (encoding->flags & IW_ENCODE_INDEX) - 1;
	if (alg == IW_ENCODE_ALG_NONE )
		pIoctlSec->Alg = RT_CMD_STA_IOCTL_SECURITY_ALG_NONE;
	else if (alg == IW_ENCODE_ALG_WEP)
		pIoctlSec->Alg = RT_CMD_STA_IOCTL_SECURITY_ALG_WEP;
	else if (alg == IW_ENCODE_ALG_TKIP)
		pIoctlSec->Alg = RT_CMD_STA_IOCTL_SECURITY_ALG_TKIP;
	else if (alg == IW_ENCODE_ALG_CCMP)
		pIoctlSec->Alg = RT_CMD_STA_IOCTL_SECURITY_ALG_CCMP;
	else 
	{
		DBGPRINT(RT_DEBUG_WARN, ("Warning: Security type is not supported. (alg = %d) \n", alg));
		pIoctlSec->Alg = alg;
		return -EOPNOTSUPP;
	}
	pIoctlSec->ext_flags = 0;
	if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
		pIoctlSec->ext_flags |= RT_CMD_STA_IOCTL_SECURTIY_EXT_SET_TX_KEY;
	if (ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY)
		pIoctlSec->ext_flags |= RT_CMD_STA_IOCTL_SECURTIY_EXT_GROUP_KEY;
	if (encoding->flags & IW_ENCODE_DISABLED)
		pIoctlSec->flags = RT_CMD_STA_IOCTL_SECURITY_DISABLED;
	else
		pIoctlSec->flags = 0;

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWENCODEEXT, 0,
				pIoctlSec, 0, RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
		return -EINVAL;

    return 0;
}

int
rt_ioctl_giwencodeext(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	VOID *pAd = NULL;
/*	PCHAR pKey = NULL; */
	struct iw_point *encoding = &wrqu->encoding;
	struct iw_encode_ext *ext = (struct iw_encode_ext *)extra;
	int /* idx, */ max_key_len;
	RT_CMD_STA_IOCTL_SECURITY IoctlSec, *pIoctlSec = &IoctlSec;

	GET_PAD_FROM_NET_DEV(pAd, dev);

	DBGPRINT(RT_DEBUG_TRACE ,("===> rt_ioctl_giwencodeext\n"));

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}

	max_key_len = encoding->length - sizeof(*ext);
	if (max_key_len < 0)
		return -EINVAL;
	memset(ext, 0, sizeof(*ext));

#if 0 /* os abl move */
	idx = encoding->flags & IW_ENCODE_INDEX;
	if (idx) 
	{
		if (idx < 1 || idx > 4)
			return -EINVAL;
		idx--;

		if ((pAd->StaCfg.WepStatus == Ndis802_11TKIPEnable) ||
			(pAd->StaCfg.WepStatus == Ndis802_11AESEnable))
		{
			if (idx != pAd->StaCfg.DefaultKeyId)
			{
				ext->key_len = 0;
				return 0;
			}
		}
	} 
	else
		idx = pAd->StaCfg.DefaultKeyId;

	encoding->flags = idx + 1;
	memset(ext, 0, sizeof(*ext));

	ext->key_len = 0;
	switch(pAd->StaCfg.WepStatus) {
		case Ndis802_11WEPDisabled:
			ext->alg = IW_ENCODE_ALG_NONE;
			encoding->flags |= IW_ENCODE_DISABLED;		
			break;
		case Ndis802_11WEPEnabled:
			ext->alg = IW_ENCODE_ALG_WEP;
			if (pAd->SharedKey[BSS0][idx].KeyLen > max_key_len)
				return -E2BIG;
			else
			{
				ext->key_len = pAd->SharedKey[BSS0][idx].KeyLen;				
				pKey = (PCHAR)&(pAd->SharedKey[BSS0][idx].Key[0]);
			}
			break;
		case Ndis802_11TKIPEnable:
		case Ndis802_11AESEnable:
			if (pAd->StaCfg.WepStatus == Ndis802_11TKIPEnable)
				ext->alg = IW_ENCODE_ALG_TKIP;
			else
				ext->alg = IW_ENCODE_ALG_CCMP;
			
			if (max_key_len < 32)
				return -E2BIG;
			else
			{
				ext->key_len = 32;
				pKey = (PCHAR)&pAd->StaCfg.PMK[0];
			}
			break;
		default:
			return -EINVAL;
	}
#endif /* 0 */

	memset(pIoctlSec, 0, sizeof(RT_CMD_STA_IOCTL_SECURITY));
	pIoctlSec->KeyIdx = encoding->flags & IW_ENCODE_INDEX;
	pIoctlSec->MaxKeyLen = max_key_len;

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWENCODEEXT, 0,
				pIoctlSec, 0, RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
	{
		ext->key_len = 0;
		RT_CMD_STATUS_TRANSLATE(pIoctlSec->Status);
		return pIoctlSec->Status;
	}

	encoding->flags = (USHORT)pIoctlSec->KeyIdx;
	ext->key_len = pIoctlSec->length;

	if (pIoctlSec->Alg == RT_CMD_STA_IOCTL_SECURITY_ALG_NONE)
		ext->alg = IW_ENCODE_ALG_NONE;
	else if (pIoctlSec->Alg == RT_CMD_STA_IOCTL_SECURITY_ALG_WEP)
		ext->alg = IW_ENCODE_ALG_WEP;
	else if (pIoctlSec->Alg == RT_CMD_STA_IOCTL_SECURITY_ALG_TKIP)
		ext->alg = IW_ENCODE_ALG_TKIP;
	else if (pIoctlSec->Alg == RT_CMD_STA_IOCTL_SECURITY_ALG_CCMP)
		ext->alg = IW_ENCODE_ALG_CCMP;

	if (pIoctlSec->flags & RT_CMD_STA_IOCTL_SECURITY_DISABLED)
		encoding->flags |= IW_ENCODE_DISABLED;

	if (ext->key_len && pIoctlSec->pData)
	{
		encoding->flags |= IW_ENCODE_ENABLED;
		memcpy(ext->key, pIoctlSec->pData, ext->key_len);
	}
	
	return 0;
}

#ifdef SIOCSIWGENIE
int rt_ioctl_siwgenie(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	VOID   *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, dev);	
	
	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}	
#ifdef WPA_SUPPLICANT_SUPPORT
#if 0 /* os abl move */
	if (pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP != WPA_SUPPLICANT_DISABLE)
	{
		DBGPRINT(RT_DEBUG_TRACE ,("===> rt_ioctl_siwgenie\n"));
		pAd->StaCfg.wpa_supplicant_info.bRSN_IE_FromWpaSupplicant = FALSE;
		if ((wrqu->data.length > 0) &&
		    (extra == NULL))
		{
			return -EINVAL;
		}
		else if (wrqu->data.length) 
		{
			if (pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe)
			{
				kfree(pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe);
				pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe = NULL;
			}
			pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe = kmalloc(wrqu->data.length, MEM_ALLOC_FLAG);
			if (pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe)
			{
				pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen = wrqu->data.length;
				NdisMoveMemory(pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe, extra, pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen);
				pAd->StaCfg.wpa_supplicant_info.bRSN_IE_FromWpaSupplicant = TRUE;
			}
			else
				pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen = 0;
		}
		return 0;
	}
#endif /* 0 */

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWGENIE, 0,
						extra, wrqu->data.length,
						RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
		return -EINVAL;
	else
		return 0;
#endif /* WPA_SUPPLICANT_SUPPORT */

	return -EOPNOTSUPP;
}
#endif /* SIOCSIWGENIE */

int rt_ioctl_giwgenie(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	VOID   *pAd = NULL;
	RT_CMD_STA_IOCTL_RSN_IE IoctlRsnIe, *pIoctlRsnIe = &IoctlRsnIe;

	GET_PAD_FROM_NET_DEV(pAd, dev);	
	
	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}

#if 0 /* os abl move */
	if ((pAd->StaCfg.RSNIE_Len == 0) ||
		(pAd->StaCfg.AuthMode < Ndis802_11AuthModeWPA))
	{
		wrqu->data.length = 0;
		return 0;
	}

#ifdef WPA_SUPPLICANT_SUPPORT
#ifdef SIOCSIWGENIE
	if ((pAd->StaCfg.wpa_supplicant_info.WpaSupplicantUP & 0x7F) == WPA_SUPPLICANT_ENABLE &&
		(pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen > 0))
	{
		if (wrqu->data.length < pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen)
			return -E2BIG;

		wrqu->data.length = pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen;
		memcpy(extra, pAd->StaCfg.wpa_supplicant_info.pWpaAssocIe, pAd->StaCfg.wpa_supplicant_info.WpaAssocIeLen);
	}
	else
#endif /* SIOCSIWGENIE */
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
	{
		UCHAR RSNIe = IE_WPA;
		
		if (wrqu->data.length < (pAd->StaCfg.RSNIE_Len + 2)) /* ID, Len */
			return -E2BIG;
		wrqu->data.length = pAd->StaCfg.RSNIE_Len + 2;
		
		if ((pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK) ||
            (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2))
			RSNIe = IE_RSN;
		
		extra[0] = (char)RSNIe;
		extra[1] = pAd->StaCfg.RSNIE_Len;
		memcpy(extra+2, &pAd->StaCfg.RSN_IE[0], pAd->StaCfg.RSNIE_Len);
	}
#endif /* 0 */

	pIoctlRsnIe->length = wrqu->data.length;
	pIoctlRsnIe->pRsnIe = (UCHAR *)extra;

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWGENIE, 0,
						pIoctlRsnIe, 0,
						RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
		return -E2BIG;

	wrqu->data.length = (USHORT)pIoctlRsnIe->length;
	return 0;
}

int rt_ioctl_siwpmksa(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu,
			   char *extra)
{
	VOID   *pAd = NULL;
	struct iw_pmksa *pPmksa = (struct iw_pmksa *)wrqu->data.pointer;
/*	INT	CachedIdx = 0, idx = 0; */
	RT_CMD_STA_IOCTL_PMA_SA IoctlPmaSa, *pIoctlPmaSa = &IoctlPmaSa;

	GET_PAD_FROM_NET_DEV(pAd, dev);	

	/*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
       	DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
        return -ENETDOWN;
	}

	if (pPmksa == NULL)
		return -EINVAL;

	DBGPRINT(RT_DEBUG_TRACE ,("===> rt_ioctl_siwpmksa\n"));

#if 0 /* os abl move */
	switch(pPmksa->cmd)
	{
		case IW_PMKSA_FLUSH:
			NdisZeroMemory(pAd->StaCfg.SavedPMK, sizeof(BSSID_INFO)*PMKID_NO);
			DBGPRINT(RT_DEBUG_TRACE ,("rt_ioctl_siwpmksa - IW_PMKSA_FLUSH\n"));
			break;
		case IW_PMKSA_REMOVE:
			for (CachedIdx = 0; CachedIdx < pAd->StaCfg.SavedPMKNum; CachedIdx++)
			{
		        /* compare the BSSID */
		        if (NdisEqualMemory(pPmksa->bssid.sa_data, pAd->StaCfg.SavedPMK[CachedIdx].BSSID, MAC_ADDR_LEN))
		        {
		        	NdisZeroMemory(pAd->StaCfg.SavedPMK[CachedIdx].BSSID, MAC_ADDR_LEN);
					NdisZeroMemory(pAd->StaCfg.SavedPMK[CachedIdx].PMKID, 16);
					for (idx = CachedIdx; idx < (pAd->StaCfg.SavedPMKNum - 1); idx++)
					{
						NdisMoveMemory(&pAd->StaCfg.SavedPMK[idx].BSSID[0], &pAd->StaCfg.SavedPMK[idx+1].BSSID[0], MAC_ADDR_LEN);
						NdisMoveMemory(&pAd->StaCfg.SavedPMK[idx].PMKID[0], &pAd->StaCfg.SavedPMK[idx+1].PMKID[0], 16);
					}
					pAd->StaCfg.SavedPMKNum--;
			        break;
		        }
	        }
			
			DBGPRINT(RT_DEBUG_TRACE ,("rt_ioctl_siwpmksa - IW_PMKSA_REMOVE\n"));
			break;
		case IW_PMKSA_ADD:
			for (CachedIdx = 0; CachedIdx < pAd->StaCfg.SavedPMKNum; CachedIdx++)
			{
		        /* compare the BSSID */
		        if (NdisEqualMemory(pPmksa->bssid.sa_data, pAd->StaCfg.SavedPMK[CachedIdx].BSSID, MAC_ADDR_LEN))
			        break;			
	        }

	        /* Found, replace it */
	        if (CachedIdx < PMKID_NO)
	        {
		        DBGPRINT(RT_DEBUG_OFF, ("Update PMKID, idx = %d\n", CachedIdx));
		        NdisMoveMemory(&pAd->StaCfg.SavedPMK[CachedIdx].BSSID[0], pPmksa->bssid.sa_data, MAC_ADDR_LEN);
				NdisMoveMemory(&pAd->StaCfg.SavedPMK[CachedIdx].PMKID[0], pPmksa->pmkid, 16);
		        pAd->StaCfg.SavedPMKNum++;
	        }
	        /* Not found, replace the last one */
	        else
	        {
		        /* Randomly replace one */
		        CachedIdx = (pPmksa->bssid.sa_data[5] % PMKID_NO);
		        DBGPRINT(RT_DEBUG_OFF, ("Update PMKID, idx = %d\n", CachedIdx));
		        NdisMoveMemory(&pAd->StaCfg.SavedPMK[CachedIdx].BSSID[0], pPmksa->bssid.sa_data, MAC_ADDR_LEN);
				NdisMoveMemory(&pAd->StaCfg.SavedPMK[CachedIdx].PMKID[0], pPmksa->pmkid, 16);
	        }
			
			DBGPRINT(RT_DEBUG_TRACE ,("rt_ioctl_siwpmksa - IW_PMKSA_ADD\n"));
			break;
		default:
			DBGPRINT(RT_DEBUG_TRACE ,("rt_ioctl_siwpmksa - Unknow Command!!\n"));
			break;
	}
#endif /* 0 */

	if (pPmksa->cmd == IW_PMKSA_FLUSH)
		pIoctlPmaSa->Cmd = RT_CMD_STA_IOCTL_PMA_SA_FLUSH;
	else if (pPmksa->cmd == IW_PMKSA_REMOVE)
		pIoctlPmaSa->Cmd = RT_CMD_STA_IOCTL_PMA_SA_REMOVE;
	else if (pPmksa->cmd == IW_PMKSA_ADD)
		pIoctlPmaSa->Cmd = RT_CMD_STA_IOCTL_PMA_SA_ADD;
	else
		pIoctlPmaSa->Cmd = 0;
	pIoctlPmaSa->pBssid = (UCHAR *)pPmksa->bssid.sa_data;
	pIoctlPmaSa->pPmkid = pPmksa->pmkid;

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWPMKSA, 0,
						pIoctlPmaSa, 0, RT_DEV_PRIV_FLAGS_GET(dev));

	return 0;
}
#endif /* #if WIRELESS_EXT > 17 */

#ifdef DBG
static int
rt_private_ioctl_bbp(struct net_device *dev, struct iw_request_info *info,
		struct iw_point *wrq, char *extra)
{
	RTMP_IOCTL_INPUT_STRUCT wrqin;
#if 0
	RTMP_STRING *this_char;
	RTMP_STRING *value = NULL;
	UCHAR				regBBP = 0;
/*	CHAR				arg[255]={0}; */
	UINT32				bbpId;
	UINT32				bbpValue;
	BOOLEAN				bIsPrintAllBBP = FALSE;
#endif
	INT					Status = 0;
    VOID       			*pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, dev);	

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("==>rt_private_ioctl_bbp\n"));
#endif /* RELEASE_EXCLUDE */

	memset(extra, 0x00, IW_PRIV_SIZE_MASK);

	wrqin.u.data.pointer = wrq->pointer;
	wrqin.u.data.length = wrq->length;

	RTMP_STA_IoctlHandle(pAd, &wrqin, CMD_RTPRIV_IOCTL_BBP, 0,
						extra, IW_PRIV_SIZE_MASK, RT_DEV_PRIV_FLAGS_GET(dev));

	wrq->length = wrqin.u.data.length;

#if 0 /* os abl move */
	if (wrq->length > 1) /*No parameters. */
				{
		sprintf(extra, "\n");
					
		/*Parsing Read or Write */
		this_char = wrq->pointer;
		DBGPRINT(RT_DEBUG_TRACE, ("this_char=%s\n", this_char));
		if (!*this_char)                                                                            
			goto next;

		if ((value = rtstrchr(this_char, '=')) != NULL)
			*value++ = 0;		
		
		if (!value || !*value)
		{ /*Read */
			DBGPRINT(RT_DEBUG_TRACE, ("this_char=%s, value=%s\n", this_char, value));
			if (sscanf(this_char, "%d", &(bbpId)) == 1)                                             
			{  
				if (bbpId <= pAd->chipCap.MaxNumOfBbpId)
				{                                                                                   
#ifdef CONFIG_ATE
					if (ATE_ON(pAd))
					{
						ATE_BBP_IO_READ8_BY_REG_ID(pAd, bbpId, &regBBP);
					}
					else
#endif /* CONFIG_ATE */
					{
					RTMP_BBP_IO_READ8_BY_REG_ID(pAd, bbpId, &regBBP);
					}
					sprintf(extra+strlen(extra), "R%02d[0x%02X]:%02X\n", bbpId, bbpId, regBBP);
                    wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
					DBGPRINT(RT_DEBUG_TRACE, ("msg=%s\n", extra));
				}                                                                                   
				else                                                                                
				{/*Invalid parametes, so default printk all bbp */
					bIsPrintAllBBP = TRUE;
					goto next;                                                                          
				}                                                                                   
			}                                                                                       
			else                                                                                    
			{ /*Invalid parametes, so default printk all bbp */
				bIsPrintAllBBP = TRUE;
				goto next;                                                                              
			}                                                                                       
		}                                                                                           
		else                                                                                        
		{ /*Write */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("this_char=%s, value=%s\n", this_char, value));
#endif /* RELEASE_EXCLUDE */			
			if ((sscanf(this_char, "%d", &(bbpId)) == 1) && (sscanf(value, "%x", &(bbpValue)) == 1))
			{
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("bbpID=%02d, value=0x%x\n", bbpId, bbpValue));               
#endif /* RELEASE_EXCLUDE */
				if (bbpId <= pAd->chipCap.MaxNumOfBbpId)
				{                                                                                   
#ifdef CONFIG_ATE
					if (ATE_ON(pAd))
					{
						ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, bbpId, bbpValue);          
						/* read it back for showing */                                                      
						ATE_BBP_IO_READ8_BY_REG_ID(pAd, bbpId, &regBBP);  
					}
					else
#endif /* CONFIG_ATE */
					{
					    RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, bbpId, bbpValue);          
    					/* read it back for showing */                                                      
    					RTMP_BBP_IO_READ8_BY_REG_ID(pAd, bbpId, &regBBP);                          
			}
					sprintf(extra+strlen(extra), "R%02d[0x%02X]:%02X\n", bbpId, bbpId, regBBP);
                    wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
					DBGPRINT(RT_DEBUG_TRACE, ("msg=%s\n", extra));                                       
				}                                                                                   
				else                                                                                
				{/*Invalid parametes, so default printk all bbp */
					bIsPrintAllBBP = TRUE;                                                          
					goto next;                                                                          
				}                                                                                   
			}                                                                                       
			else                                                                                    
			{ /*Invalid parametes, so default printk all bbp */
				bIsPrintAllBBP = TRUE;                                                              
				goto next;                                                                              
			}                                                                                       
		}                             
		}
	else
		bIsPrintAllBBP = TRUE;

next:    
	if (bIsPrintAllBBP)
	{   
		memset(extra, 0x00, IW_PRIV_SIZE_MASK);
		sprintf(extra, "\n");
		for (bbpId = 0; bbpId <= pAd->chipCap.MaxNumOfBbpId; bbpId++)
		{
		    if (strlen(extra) >= (IW_PRIV_SIZE_MASK - 20))
                break;
#ifdef CONFIG_ATE
			if (ATE_ON(pAd))
			{
				ATE_BBP_IO_READ8_BY_REG_ID(pAd, bbpId, &regBBP); 
			}
			else
#endif /* CONFIG_ATE */
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, bbpId, &regBBP);
			sprintf(extra+strlen(extra), "R%02d[0x%02X]:%02X    ", bbpId, bbpId, regBBP);
			if (bbpId%5 == 4)
			sprintf(extra+strlen(extra), "%03d = %02X\n", bbpId, regBBP);  /* edit by johnli, change display format */
		}
		
        wrq->length = strlen(extra) + 1; /* 1: size of '\0' */
        DBGPRINT(RT_DEBUG_TRACE, ("wrq->length = %d\n", wrq->length));
	}
#endif /* 0 */
	
	DBGPRINT(RT_DEBUG_TRACE, ("<==rt_private_ioctl_bbp\n\n"));	
    
    return Status;
}
#endif /* DBG */

int rt_ioctl_siwrate(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
    VOID   *pAd = NULL;
    UINT32          rate = wrqu->bitrate.value, fixed = wrqu->bitrate.fixed;
	RT_CMD_RATE_SET CmdRate;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
  		DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_siwrate::Network is down!\n"));
    	return -ENETDOWN;   
	}    

    DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_siwrate::(rate = %d, fixed = %d)\n", rate, fixed));
    /* rate = -1 => auto rate
       rate = X, fixed = 1 => (fixed rate X)       
    */

#if 0 /* os abl move */
    if (rate == -1)
    {
        /*Auto Rate */
        pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;	
		pAd->StaCfg.bAutoTxRateSwitch = TRUE;
		if ((!WMODE_CAP_N(pAd->CommonCfg.PhyMode)) ||
		    (pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE <= MODE_OFDM))
            RTMPSetDesiredRates(pAd, -1);
			
#ifdef DOT11_N_SUPPORT
            SetCommonHT(pAd);
#endif /* DOT11_N_SUPPORT */
    }
    else
    {        
        if (fixed)
        {
        	pAd->StaCfg.bAutoTxRateSwitch = FALSE;
            if ((!WMODE_CAP_N(pAd->CommonCfg.PhyMode)) ||
                (pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE <= MODE_OFDM))
                RTMPSetDesiredRates(pAd, rate);
            else
            {
                pAd->StaCfg.DesiredTransmitSetting.field.MCS = MCS_AUTO;
#ifdef DOT11_N_SUPPORT
                SetCommonHT(pAd);
#endif /* DOT11_N_SUPPORT */
            }
            DBGPRINT(RT_DEBUG_TRACE, ("rt_ioctl_siwrate::(HtMcs=%d)\n",pAd->StaCfg.DesiredTransmitSetting.field.MCS));
        }
        else
        {
            /* TODO: rate = X, fixed = 0 => (rates <= X) */
            return -EOPNOTSUPP;
        }
    }
#endif /* 0 */

	CmdRate.Rate = rate;
	CmdRate.Fixed = fixed;

	if (RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCSIWRATE, 0,
							&CmdRate, 0,
							RT_DEV_PRIV_FLAGS_GET(dev)) != NDIS_STATUS_SUCCESS)
		return -EOPNOTSUPP;

    return 0;
}

int rt_ioctl_giwrate(struct net_device *dev,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
    VOID   *pAd = NULL;
/*    int rate_index = 0, rate_count = 0; */
/*    HTTRANSMIT_SETTING ht_setting; */
	ULONG Rate;

	GET_PAD_FROM_NET_DEV(pAd, dev);

    /*check if the interface is down */
/*	if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
  		DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
    	return -ENETDOWN;   
	}

#if 0 /* os abl move */
    rate_count = RT_RateSize/sizeof(__s32);

    if ((pAd->StaCfg.bAutoTxRateSwitch == FALSE) &&
        (INFRA_ON(pAd)) &&
        ((!WMODE_CAP_N(pAd->CommonCfg.PhyMode)) || (pAd->MacTab.Content[BSSID_WCID].HTPhyMode.field.MODE <= MODE_OFDM)))
        ht_setting.word = pAd->StaCfg.HTPhyMode.word;
    else
        ht_setting.word = pAd->MacTab.Content[BSSID_WCID].HTPhyMode.word;
    
#ifdef DOT11_N_SUPPORT
    if (ht_setting.field.MODE >= MODE_HTMIX)
    {
/*    	rate_index = 12 + ((UCHAR)ht_setting.field.BW *16) + ((UCHAR)ht_setting.field.ShortGI *32) + ((UCHAR)ht_setting.field.MCS); */
    	rate_index = 12 + ((UCHAR)ht_setting.field.BW *24) + ((UCHAR)ht_setting.field.ShortGI *48) + ((UCHAR)ht_setting.field.MCS);
    }
    else 
#endif /* DOT11_N_SUPPORT */
    if (ht_setting.field.MODE == MODE_OFDM)                
    	rate_index = (UCHAR)(ht_setting.field.MCS) + 4;
    else if (ht_setting.field.MODE == MODE_CCK)   
    	rate_index = (UCHAR)(ht_setting.field.MCS);

    if (rate_index < 0)
        rate_index = 0;
    
    if (rate_index >= rate_count)
        rate_index = rate_count-1;

    wrqu->bitrate.value = ralinkrate[rate_index] * 500000;
#endif /* 0 */

	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWRATE, 0,
						&Rate, 0, RT_DEV_PRIV_FLAGS_GET(dev));
	wrqu->bitrate.value = Rate;
    wrqu->bitrate.disabled = 0;

    return 0;
}


static const iw_handler rt_handler[] =
{
	(iw_handler) NULL,			            /* SIOCSIWCOMMIT */
	(iw_handler) rt_ioctl_giwname,			/* SIOCGIWNAME   */
	(iw_handler) NULL,			            /* SIOCSIWNWID   */
	(iw_handler) NULL,			            /* SIOCGIWNWID   */
	(iw_handler) rt_ioctl_siwfreq,		    /* SIOCSIWFREQ   */
	(iw_handler) rt_ioctl_giwfreq,		    /* SIOCGIWFREQ   */
	(iw_handler) rt_ioctl_siwmode,		    /* SIOCSIWMODE   */
	(iw_handler) rt_ioctl_giwmode,		    /* SIOCGIWMODE   */
	(iw_handler) NULL,		                /* SIOCSIWSENS   */
	(iw_handler) NULL,		                /* SIOCGIWSENS   */
	(iw_handler) NULL /* not used */,		/* SIOCSIWRANGE  */
	(iw_handler) rt_ioctl_giwrange,		    /* SIOCGIWRANGE  */
	(iw_handler) rt_ioctl_giwpriv,		/* SIOCSIWPRIV  for Android */
	(iw_handler) NULL /* kernel code */,    /* SIOCGIWPRIV   */
	(iw_handler) NULL /* not used */,		/* SIOCSIWSTATS  */
	(iw_handler) rt28xx_get_wireless_stats /* kernel code */,    /* SIOCGIWSTATS  */
	(iw_handler) NULL,		                /* SIOCSIWSPY    */
	(iw_handler) NULL,		                /* SIOCGIWSPY    */
	(iw_handler) NULL,				        /* SIOCSIWTHRSPY */
	(iw_handler) NULL,				        /* SIOCGIWTHRSPY */
	(iw_handler) rt_ioctl_siwap,            /* SIOCSIWAP     */
	(iw_handler) rt_ioctl_giwap,		    /* SIOCGIWAP     */
#ifdef SIOCSIWMLME
	(iw_handler) rt_ioctl_siwmlme,	        /* SIOCSIWMLME   */
#else
	(iw_handler) NULL,				        /* SIOCSIWMLME */
#endif /* SIOCSIWMLME */
	(iw_handler) rt_ioctl_iwaplist,		    /* SIOCGIWAPLIST */
#ifdef SIOCGIWSCAN
	(iw_handler) rt_ioctl_siwscan,		    /* SIOCSIWSCAN   */
	(iw_handler) rt_ioctl_giwscan,		    /* SIOCGIWSCAN   */
#else
	(iw_handler) NULL,				        /* SIOCSIWSCAN   */
	(iw_handler) NULL,				        /* SIOCGIWSCAN   */
#endif /* SIOCGIWSCAN */
	(iw_handler) rt_ioctl_siwessid,		    /* SIOCSIWESSID  */
	(iw_handler) rt_ioctl_giwessid,		    /* SIOCGIWESSID  */
	(iw_handler) rt_ioctl_siwnickn,		    /* SIOCSIWNICKN  */
	(iw_handler) rt_ioctl_giwnickn,		    /* SIOCGIWNICKN  */
	(iw_handler) NULL,				        /* -- hole --    */
	(iw_handler) NULL,				        /* -- hole --    */
	(iw_handler) rt_ioctl_siwrate,          /* SIOCSIWRATE   */
	(iw_handler) rt_ioctl_giwrate,          /* SIOCGIWRATE   */
	(iw_handler) rt_ioctl_siwrts,		    /* SIOCSIWRTS    */
	(iw_handler) rt_ioctl_giwrts,		    /* SIOCGIWRTS    */
	(iw_handler) rt_ioctl_siwfrag,		    /* SIOCSIWFRAG   */
	(iw_handler) rt_ioctl_giwfrag,		    /* SIOCGIWFRAG   */
	(iw_handler) NULL,		                /* SIOCSIWTXPOW  */
	(iw_handler) NULL,		                /* SIOCGIWTXPOW  */
	(iw_handler) NULL,		                /* SIOCSIWRETRY  */
	(iw_handler) NULL,		                /* SIOCGIWRETRY  */
	(iw_handler) rt_ioctl_siwencode,		/* SIOCSIWENCODE */
	(iw_handler) rt_ioctl_giwencode,		/* SIOCGIWENCODE */
	(iw_handler) NULL,		                /* SIOCSIWPOWER  */
	(iw_handler) NULL,		                /* SIOCGIWPOWER  */
	(iw_handler) NULL,						/* -- hole -- */	
	(iw_handler) NULL,						/* -- hole -- */
#if WIRELESS_EXT > 17	
    (iw_handler) rt_ioctl_siwgenie,         /* SIOCSIWGENIE  */
	(iw_handler) rt_ioctl_giwgenie,         /* SIOCGIWGENIE  */
	(iw_handler) rt_ioctl_siwauth,		    /* SIOCSIWAUTH   */
	(iw_handler) rt_ioctl_giwauth,		    /* SIOCGIWAUTH   */
	(iw_handler) rt_ioctl_siwencodeext,	    /* SIOCSIWENCODEEXT */
	(iw_handler) rt_ioctl_giwencodeext,		/* SIOCGIWENCODEEXT */
	(iw_handler) rt_ioctl_siwpmksa,         /* SIOCSIWPMKSA  */
#endif
};

static const iw_handler rt_priv_handlers[] = {
	(iw_handler) NULL, /* + 0x00 */
	(iw_handler) NULL, /* + 0x01 */
	(iw_handler) rt_ioctl_setparam, /* + 0x02 */
#ifdef DBG
	(iw_handler) rt_private_ioctl_bbp, /* + 0x03 */	
#else
	(iw_handler) NULL, /* + 0x03 */
#endif
	(iw_handler) NULL, /* + 0x04 */
	(iw_handler) NULL, /* + 0x05 */
	(iw_handler) NULL, /* + 0x06 */
	(iw_handler) NULL, /* + 0x07 */
	(iw_handler) NULL, /* + 0x08 */
	(iw_handler) rt_private_get_statistics, /* + 0x09 */
	(iw_handler) NULL, /* + 0x0A */
	(iw_handler) NULL, /* + 0x0B */
	(iw_handler) NULL, /* + 0x0C */
	(iw_handler) NULL, /* + 0x0D */
	(iw_handler) NULL, /* + 0x0E */
	(iw_handler) NULL, /* + 0x0F */
	(iw_handler) NULL, /* + 0x10 */
	(iw_handler) rt_private_show, /* + 0x11 */
    (iw_handler) NULL, /* + 0x12 */
	(iw_handler) NULL, /* + 0x13 */
#ifdef WSC_STA_SUPPORT	
	(iw_handler) rt_private_set_wsc_u32_item, /* + 0x14 */
#else
    (iw_handler) NULL, /* + 0x14 */
#endif /* WSC_STA_SUPPORT */
	(iw_handler) NULL, /* + 0x15 */
#ifdef WSC_STA_SUPPORT	
	(iw_handler) rt_private_set_wsc_string_item, /* + 0x16 */
#else
    (iw_handler) NULL, /* + 0x16 */
#endif /* WSC_STA_SUPPORT */
	(iw_handler) NULL, /* + 0x17 */
	(iw_handler) NULL, /* + 0x18 */
};

const struct iw_handler_def rt28xx_iw_handler_def =
{
#define	N(a)	(sizeof (a) / sizeof (a[0]))
	.standard	= (iw_handler *) rt_handler,
	.num_standard	= sizeof(rt_handler) / sizeof(iw_handler),
#if defined(CONFIG_WEXT_PRIV) || LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 32)
	.private	= (iw_handler *) rt_priv_handlers,
	.num_private		= N(rt_priv_handlers),
	.private_args	= (struct iw_priv_args *) privtab,
	.num_private_args	= N(privtab),
#endif /* CONFIG_WEXT_PRIV || LINUX_VERSION_CODE <= 2.6.32 */
#if IW_HANDLER_VERSION >= 7
    .get_wireless_stats = rt28xx_get_wireless_stats,
#endif 
};


INT rt28xx_sta_ioctl(struct net_device *net_dev, struct ifreq *rq, INT cmd)
{
/*	POS_COOKIE			pObj; */
	VOID        		*pAd = NULL;
	struct iwreq        *wrqin = (struct iwreq *) rq;
	RTMP_IOCTL_INPUT_STRUCT rt_wrq, *wrq = &rt_wrq;
/*	BOOLEAN				StateMachineTouched = FALSE; */
	INT					Status = NDIS_STATUS_SUCCESS;
	USHORT				subcmd;
	UINT32				org_len;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);

	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	wrq->u.data.pointer = wrqin->u.data.pointer;
	wrq->u.data.length = wrqin->u.data.length;
	org_len = wrq->u.data.length;

/*	pObj = (POS_COOKIE) pAd->OS_Cookie; */
	
    /*check if the interface is down */
/*    if(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */

#ifdef E2P_WITHOUT_FW_SUPPORT
	if (cmd == RTPRIV_IOCTL_E2P)
		goto skip_check;
#endif /* E2P_WITHOUT_FW_SUPPORT */

	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
    {
	    if (wrqin->u.data.pointer == NULL)
	    {
		    return Status;
	    }

		if (cmd != RTPRIV_IOCTL_SET)
		{
            DBGPRINT(RT_DEBUG_TRACE, ("INFO::Network is down!\n"));
		    return -ENETDOWN;  
        }
    }
	
#ifdef E2P_WITHOUT_FW_SUPPORT
skip_check:
#endif /* E2P_WITHOUT_FW_SUPPORT */

#if 0 /* os abl move */
#ifdef MESH_SUPPORT
	if (RT_DEV_PRIV_FLAGS_GET(net_dev) & INT_MESH)
	{
		pObj->ioctl_if_type = INT_MESH;
		pObj->ioctl_if = 0;
	} else
#endif /* MESH_SUPPORT */
#ifdef P2P_SUPPORT
	if (RT_DEV_PRIV_FLAGS_GET(net_dev) & INT_P2P)
	{
		pObj->ioctl_if_type = INT_P2P;
		pObj->ioctl_if = 0;
	} else
#endif /* P2P_SUPPORT */
	{	/* determine this ioctl command is comming from which interface. */
		pObj->ioctl_if_type = INT_MAIN;
		pObj->ioctl_if = MAIN_MBSSID;
	}
#endif /* 0 */

#if 0
	RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_INF_TYPE_SET,
						0, NULL, RT_DEV_PRIV_FLAGS_GET(net_dev),
						RT_DEV_PRIV_FLAGS_GET(net_dev));
#endif

	switch(cmd)
	{			
		case RTPRIV_IOCTL_ATE:
			{
				/*
					ATE is always controlled by ra0
				*/
				RTMP_COM_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_ATE, 0, wrqin->ifr_name, 0);
			}
			break;

        case SIOCGIFHWADDR:
			DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIFHWADDR\n"));
/*			memcpy(wrqin->u.name, pAd->CurrentAddress, ETH_ALEN); */
			RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIFHWADDR,
							0, wrqin->u.name, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));
			break;	
		case SIOCGIWNAME:
        {
        	char *name=&wrqin->u.name[0];
        	rt_ioctl_giwname(net_dev, NULL, name, NULL);
            break;
		}
		case SIOCGIWESSID:  /*Get ESSID */
        {
        	struct iw_point *essid=&wrqin->u.essid;
        	rt_ioctl_giwessid(net_dev, NULL, essid, essid->pointer);
            break;
		}
		case SIOCSIWESSID:  /*Set ESSID */
        	{
        	struct iw_point	*essid=&wrqin->u.essid;
        	rt_ioctl_siwessid(net_dev, NULL, essid, essid->pointer);
            break;  
		}
		case SIOCSIWNWID:   /* set network id (the cell) */
		case SIOCGIWNWID:   /* get network id */
			Status = -EOPNOTSUPP;
			break;
		case SIOCSIWFREQ:   /*set channel/frequency (Hz) */
        	{
        	struct iw_freq *freq=&wrqin->u.freq;
        	rt_ioctl_siwfreq(net_dev, NULL, freq, NULL);
			break;
		}
		case SIOCGIWFREQ:   /* get channel/frequency (Hz) */
        	{
        	struct iw_freq *freq=&wrqin->u.freq;
        	rt_ioctl_giwfreq(net_dev, NULL, freq, NULL);
			break;
		}
		case SIOCSIWNICKN: /*set node name/nickname */
        	{
        	/*struct iw_point *data=&wrq->u.data; */
        	/*rt_ioctl_siwnickn(net_dev, NULL, data, NULL); */
			break;
			}
		case SIOCGIWNICKN: /*get node name/nickname */
        {
			RT_CMD_STA_IOCTL_NICK_NAME NickName, *pNickName = &NickName;
			CHAR nickname[IW_ESSID_MAX_SIZE+1];
			struct iw_point	*erq = NULL;
        	erq = &wrqin->u.data;

			pNickName->NameLen = IW_ESSID_MAX_SIZE+1;
			pNickName->pName = (CHAR *)nickname;
			RTMP_STA_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_STA_SIOCGIWNICKN, 0,
							pNickName, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));

			/*strlen((RTMP_STRING *) pAd->nickname); */
			erq->length = (USHORT)pNickName->NameLen;
            Status = copy_to_user(erq->pointer, nickname, erq->length);
			break;
		}
		case SIOCGIWRATE:   /*get default bit rate (bps) */
		    rt_ioctl_giwrate(net_dev, NULL, &wrqin->u, NULL);
            break;
	    case SIOCSIWRATE:  /*set default bit rate (bps) */
	        rt_ioctl_siwrate(net_dev, NULL, &wrqin->u, NULL);
            break;
        case SIOCGIWRTS:  /* get RTS/CTS threshold (bytes) */
        	{
        	struct iw_param *rts=&wrqin->u.rts;
        	rt_ioctl_giwrts(net_dev, NULL, rts, NULL);
            break;
		}
        case SIOCSIWRTS:  /*set RTS/CTS threshold (bytes) */
        	{
        	struct iw_param *rts=&wrqin->u.rts;
        	rt_ioctl_siwrts(net_dev, NULL, rts, NULL);
            break;
		}
        case SIOCGIWFRAG:  /*get fragmentation thr (bytes) */
        	{
        	struct iw_param *frag=&wrqin->u.frag;
        	rt_ioctl_giwfrag(net_dev, NULL, frag, NULL);
            break;
		}
        case SIOCSIWFRAG:  /*set fragmentation thr (bytes) */
        	{
			struct iw_param *frag = &wrqin->u.frag;
			struct iw_request_info info;

			rt_ioctl_siwfrag(net_dev, &info, frag, NULL);
            break;
		}
        case SIOCGIWENCODE:  /*get encoding token & mode */
        	{
        	struct iw_point *erq=&wrqin->u.encoding;
        	if(erq)
        		rt_ioctl_giwencode(net_dev, NULL, erq, erq->pointer);
            break;
		}
        case SIOCSIWENCODE:  /*set encoding token & mode */
        	{
        	struct iw_point *erq=&wrqin->u.encoding;
        	if(erq)
        		rt_ioctl_siwencode(net_dev, NULL, erq, erq->pointer);
            break;
		}
		case SIOCGIWAP:     /*get access point MAC addresses */
        	{
        	struct sockaddr *ap_addr=&wrqin->u.ap_addr;
        	rt_ioctl_giwap(net_dev, NULL, ap_addr, ap_addr->sa_data);
			break;
		}
	    case SIOCSIWAP:  /*set access point MAC addresses */
        	{
        	struct sockaddr *ap_addr=&wrqin->u.ap_addr;
        	rt_ioctl_siwap(net_dev, NULL, ap_addr, ap_addr->sa_data);
            break;
		}
		case SIOCGIWMODE:   /*get operation mode */
        	{
        	__u32 *mode=&wrqin->u.mode;
        	rt_ioctl_giwmode(net_dev, NULL, mode, NULL);
            break;
		}
		case SIOCSIWMODE:   /*set operation mode */
        	{
        	__u32 *mode=&wrqin->u.mode;
        	rt_ioctl_siwmode(net_dev, NULL, mode, NULL);
            break;
		}
		case SIOCGIWSENS:   /*get sensitivity (dBm) */
		case SIOCSIWSENS:	/*set sensitivity (dBm) */
		case SIOCGIWPOWER:  /*get Power Management settings */
		case SIOCSIWPOWER:  /*set Power Management settings */
		case SIOCGIWTXPOW:  /*get transmit power (dBm) */
		case SIOCSIWTXPOW:  /*set transmit power (dBm) */
		case SIOCGIWRANGE:	/*Get range of parameters */
		case SIOCGIWRETRY:	/*get retry limits and lifetime */
		case SIOCSIWRETRY:	/*set retry limits and lifetime */
			Status = -EOPNOTSUPP;
			break;

		case RT_PRIV_IOCTL:
#ifdef RT_CFG80211_ANDROID_PRIV_LIB_SUPPORT
			//YF: Android Private Lib Entry
			rt_android_private_command_entry(pAd, net_dev, rq, cmd); 
			break;
#endif /* RT_CFG80211_ANDROID_PRIV_LIB_SUPPORT */

        case RT_PRIV_IOCTL_EXT:
			subcmd = wrqin->u.data.flags;

			Status = RTMP_STA_IoctlHandle(pAd, wrq, CMD_RT_PRIV_IOCTL, subcmd,
										NULL, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));
			break;		
		case SIOCGIWPRIV:
			if (wrqin->u.data.pointer) 
			{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
				if (access_ok(wrqin->u.data.pointer, sizeof(privtab)) != TRUE)
#else
				if (access_ok(VERIFY_WRITE, wrqin->u.data.pointer, sizeof(privtab)) != TRUE)
#endif
					break;
				if ((sizeof(privtab) / sizeof(privtab[0])) <= wrq->u.data.length)
				{
					wrqin->u.data.length = sizeof(privtab) / sizeof(privtab[0]);
					if (copy_to_user(wrqin->u.data.pointer, privtab, sizeof(privtab)))
						Status = -EFAULT;
				}
				else
					Status = -E2BIG;
			}
			break;
		case RTPRIV_IOCTL_SET:
			if (wrqin->u.data.length > RTPRIV_IOCTL_SET_SET_MAX_LEN) {
				Status = -EINVAL;
				DBGPRINT(RT_DEBUG_ERROR,
					 ("RTPRIV_IOCTL_SET len too long %u\n",
					  wrqin->u.data.length));
				break;
			}
			do {
				char *str = NULL;
				size_t len = wrqin->u.data.length;

				os_alloc_mem(NULL, (UCHAR **)&str, len + 1);
				if (!str) {
					Status = -ENOMEM;
					DBGPRINT(RT_DEBUG_ERROR,
					    ("RTPRIV_IOCTL_SET copy_from_user fail %zu\n", len+1));
						break;
				}
				if (copy_from_user(str, wrqin->u.data.pointer, len)) {
					Status = -EFAULT;
					DBGPRINT(RT_DEBUG_ERROR,
					    ("RTPRIV_IOCTL_SET copy_from_user fail %zu\n", len));
					os_free_mem(NULL, str);
					break;
				}
				str[len] = '\0';
				Status = rt_ioctl_setparam(net_dev, NULL, NULL, str);
				os_free_mem(NULL, str);
				str = NULL;
			} while (0);
			break;
		case RTPRIV_IOCTL_GSITESURVEY:
			RTMP_STA_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_SITESURVEY_GET, 0,
								NULL, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));
/*			RTMPIoctlGetSiteSurvey(pAd, wrq); */
		    break;			
#ifdef DBG
		case RTPRIV_IOCTL_MAC:
			RTMP_STA_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_MAC, 0,
								NULL, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));
/*			RTMPIoctlMAC(pAd, wrq); */
			break;
		case RTPRIV_IOCTL_E2P:
			RTMP_STA_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_E2P, 0,
								NULL, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));
/*			RTMPIoctlE2PROM(pAd, wrq); */
			break;
#ifdef RTMP_RF_RW_SUPPORT
		case RTPRIV_IOCTL_RF:
			RTMP_STA_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_RF, 0,
								NULL, 0, RT_DEV_PRIV_FLAGS_GET(net_dev));
/*			RTMPIoctlRF(pAd, wrq); */
			break;
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* DBG */

        case SIOCETHTOOL:
                break;
		default:
			DBGPRINT(RT_DEBUG_ERROR, ("IOCTL::unknown IOCTL's cmd = 0x%08x\n", cmd));
			Status = -EOPNOTSUPP;
			break;
	}

/*    if(StateMachineTouched) // Upper layer sent a MLME-related operations */
/*    	RTMP_MLME_HANDLER(pAd); */

	if (Status != 0)
	{
		RT_CMD_STATUS_TRANSLATE(Status);
	}
	else
	{
		/*
			If wrq length is modified, we reset the lenght of origin wrq;

			Or we can not modify it because the address of wrq->u.data.length
			maybe same as other union field, ex: iw_range, etc.

			if the length is not changed but we change it, the value for other
			union will also be changed, this is not correct.
		*/
		if (wrq->u.data.length != org_len)
			wrqin->u.data.length = wrq->u.data.length;
	}

	return Status;
}

#if 0 /* os abl move to sta/sta_cfg.c */
#ifdef DBG
/* 
    ==========================================================================
    Description:
        Read / Write MAC
    Arguments:
        pAd                    Pointer to our adapter
        wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage: 
               1.) iwpriv ra0 mac 0        ==> read MAC where Addr=0x0
               2.) iwpriv ra0 mac 0=12     ==> write MAC where Addr=0x0, value=12
    ==========================================================================
*/
VOID RTMPIoctlMAC(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq)
{
	RTMP_STRING *this_char;
	RTMP_STRING *value;
	INT					j = 0, k = 0;
/*	RTMP_STRING msg[1024]; */
	RTMP_STRING *msg = NULL;
/*	RTMP_STRING arg[255]; */
	RTMP_STRING *arg = NULL;
	ULONG				macAddr = 0;
	UCHAR				temp[16];
	RTMP_STRING temp2[16];
	UINT32				macValue = 0;
	INT					Status;
	BOOLEAN				bIsPrintAllMAC = FALSE;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("==>RTMPIoctlMAC\n"));
#endif /* RELEASE_EXCLUDE */

	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&msg, sizeof(RTMP_STRING)*1024);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		goto LabelOK;
	}
	os_alloc_mem(NULL, (UCHAR **)&arg, sizeof(RTMP_STRING)*255);
	if (arg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		goto LabelOK;
	}

	memset(msg, 0x00, 1024);
	memset(arg, 0x00, 255);
	if (wrq->u.data.length > 1) /*No parameters. */
	{   
	    Status = copy_from_user(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
		arg[254] = 0x00;
		sprintf(msg, "\n");
		
		/*Parsing Read or Write */
	    this_char = arg;
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("this_char=%s\n", this_char));
#endif /* RELEASE_EXCLUDE */
		if (!*this_char)
			goto next;

		if ((value = rtstrchr(this_char, '=')) != NULL)
			*value++ = 0;

		if (!value || !*value)
		{ /*Read */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("Read: this_char=%s, strlen=%d\n", this_char, strlen(this_char)));
#endif /* RELEASE_EXCLUDE */
			/* Sanity check */
			if(strlen(this_char) > 4)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					goto LabelOK;
			}

			/* Mac Addr */
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}
			
			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			if(strlen(this_char) == 4)
			{
				AtoH(this_char, temp, 2);
				macAddr = *temp*256 + temp[1];					
				if (macAddr < 0xFFFF)
				{
					RTMP_IO_READ32(pAd, macAddr, &macValue);
					DBGPRINT(RT_DEBUG_TRACE, ("MacAddr=%lx, MacValue=%x\n", macAddr, macValue));
					sprintf(msg+strlen(msg), "[0x%08lX]:%08X  ", macAddr , macValue);
#ifdef RELEASE_EXCLUDE
					DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
				}
				else
				{/*Invalid parametes, so default printk all mac */
					bIsPrintAllMAC = TRUE;
					goto next;
				}
			}
		}
		else
		{ /*Write */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("Write: this_char=%s, strlen(value)=%d, value=%s\n", this_char, strlen(value), value));
#endif /* RELEASE_EXCLUDE */
			memcpy(&temp2, value, strlen(value));
			temp2[strlen(value)] = '\0';

			/* Sanity check */
			if((strlen(this_char) > 4) || strlen(temp2) > 8)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					goto LabelOK;
			}

			j = strlen(temp2);
			while(j-- > 0)
			{
				if(temp2[j] > 'f' || temp2[j] < '0')
					goto LabelOK;
			}

			/*MAC Addr */
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}

			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			/*MAC value */
			k = j = strlen(temp2);
			while(j-- > 0)
			{
				temp2[8-k+j] = temp2[j];
			}
			
			while(k < 8)
				temp2[7-k++]='0';
			temp2[8]='\0';

			{
				AtoH(this_char, temp, 2);
				macAddr = *temp*256 + temp[1];

				AtoH(temp2, temp, 4);
				macValue = *temp*256*256*256 + temp[1]*256*256 + temp[2]*256 + temp[3];

				DBGPRINT(RT_DEBUG_TRACE, ("MacAddr=%02lx, MacValue=0x%x\n", macAddr, macValue));
				
				RTMP_IO_WRITE32(pAd, macAddr, macValue);
				sprintf(msg+strlen(msg), "[0x%08lX]:%08X  ", macAddr, macValue);
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
			}
		}
	}
	else
		bIsPrintAllMAC = TRUE;
next:
	if (bIsPrintAllMAC)
	{
		struct file		*file_w;
		RTMP_STRING *fileName = "MacDump.txt";
		mm_segment_t	orig_fs;

		orig_fs = get_fs();
		set_fs(KERNEL_DS); 

		/* open file */
		file_w = filp_open(fileName, O_WRONLY|O_CREAT, 0);
		if (IS_ERR(file_w)) 
		{
			DBGPRINT(RT_DEBUG_TRACE, ("-->2) %s: Error %ld opening %s\n", __FUNCTION__, -PTR_ERR(file_w), fileName));
		}
		else 
		{
			if (file_w->f_op && file_w->f_op->write) 
			{
				file_w->f_pos = 0;
				macAddr = 0x1000;
				
				while (macAddr <= 0x1800)
				{
					RTMP_IO_READ32(pAd, macAddr, &macValue);
					sprintf(msg, "%08lx = %08X\n", macAddr, macValue);
					
					/* write data to file */
					file_w->f_op->write(file_w, msg, strlen(msg), &file_w->f_pos);
					
					printk("%s", msg);
					macAddr += 4;
				}
				sprintf(msg, "\nDump all MAC values to %s\n", fileName);
			}
			filp_close(file_w, NULL);
		}
		set_fs(orig_fs); 
	}
	if(strlen(msg) == 1)
		sprintf(msg+strlen(msg), "===>Error command format!");

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("copy to user [msg=%s]\n", msg));
	DBGPRINT(RT_DEBUG_INFO, ("strlen(msg) =%d\n", strlen(msg)));
#endif /* RELEASE_EXCLUDE */	
	/* Copy the information into the user buffer */
	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);
	
LabelOK:
	if (msg != NULL)
		os_free_mem(NULL, msg);
	if (arg != NULL)
		os_free_mem(NULL, arg);

	DBGPRINT(RT_DEBUG_TRACE, ("<==RTMPIoctlMAC\n\n"));
	return;
}

/* 
    ==========================================================================
    Description:
        Read / Write E2PROM
    Arguments:
        pAd                    Pointer to our adapter
        wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage: 
               1.) iwpriv ra0 e2p 0     	==> read E2PROM where Addr=0x0
               2.) iwpriv ra0 e2p 0=1234    ==> write E2PROM where Addr=0x0, value=1234
    ==========================================================================
*/
VOID RTMPIoctlE2PROM(
	IN	PRTMP_ADAPTER	pAd, 
	IN	struct iwreq	*wrq)
{
	RTMP_STRING *this_char;
	RTMP_STRING *value;
	INT					j = 0, k = 0;
/*	RTMP_STRING msg[1024]; */
	RTMP_STRING *msg = NULL;
/*	RTMP_STRING arg[255]; */
	RTMP_STRING *arg = NULL;
	USHORT				eepAddr = 0;
	UCHAR				temp[16];
	RTMP_STRING temp2[16];
	USHORT				eepValue;
	int					Status;
	BOOLEAN				bIsPrintAllE2P = FALSE;

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("==>RTMPIoctlE2PROM\n"));
#endif /* RELEASE_EXCLUDE */

	/* allocate memory */
	os_alloc_mem(NULL, (UCHAR **)&msg, sizeof(RTMP_STRING)*1024);
	if (msg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		goto LabelOK;
	}
	os_alloc_mem(NULL, (UCHAR **)&arg, sizeof(RTMP_STRING)*255);
	if (arg == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
		goto LabelOK;
	}

	memset(msg, 0x00, 1024);
	memset(arg, 0x00, 255);
	if (wrq->u.data.length > 1) /*No parameters. */
	{   
	    Status = copy_from_user(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
		sprintf(msg, "\n");
		arg[254] = 0x00;

	    /*Parsing Read or Write */
		this_char = arg;
		
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("this_char=%s\n", this_char));
#endif /* RELEASE_EXCLUDE */
		
		if (!*this_char)
			goto next;

		if ((value = rtstrchr(this_char, '=')) != NULL)
			*value++ = 0;

		if (!value || !*value)
		{ /*Read */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("Read: this_char=%s, strlen=%d\n", this_char, strlen(this_char)));
#endif /* RELEASE_EXCLUDE */

			/* Sanity check */
			if(strlen(this_char) > 4)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					goto LabelOK;
			}

			/* E2PROM addr */
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}
			
			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			if(strlen(this_char) == 4)
			{
				AtoH(this_char, temp, 2);
				eepAddr = *temp*256 + temp[1];					
				if (eepAddr < 0xFFFF)
				{
					RT28xx_EEPROM_READ16(pAd, eepAddr, eepValue);
#ifdef RELEASE_EXCLUDE
					DBGPRINT(RT_DEBUG_INFO, ("eepAddr=%x, eepValue=0x%x\n", eepAddr, eepValue));
#endif /* RELEASE_EXCLUDE */
					sprintf(msg+strlen(msg), "[0x%04X]:0x%04X  ", eepAddr , eepValue);
#ifdef RELEASE_EXCLUDE
					DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
				}
				else
				{/*Invalid parametes, so default printk all bbp */
					bIsPrintAllE2P = TRUE;
					goto next;
				}
			}
		}
		else
		{ /*Write */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("Write: this_char=%s, strlen(value)=%d, value=%s\n", this_char, strlen(value), value));
#endif /* RELEASE_EXCLUDE */
			memcpy(&temp2, value, strlen(value));
			temp2[strlen(value)] = '\0';

			/* Sanity check */
			if((strlen(this_char) > 4) || strlen(temp2) > 8)
				goto next;

			j = strlen(this_char);
			while(j-- > 0)
			{
				if(this_char[j] > 'f' || this_char[j] < '0')
					goto LabelOK;
			}
			j = strlen(temp2);
			while(j-- > 0)
			{
				if(temp2[j] > 'f' || temp2[j] < '0')
					goto LabelOK;
			}

			/*MAC Addr */
			k = j = strlen(this_char);
			while(j-- > 0)
			{
				this_char[4-k+j] = this_char[j];
			}

			while(k < 4)
				this_char[3-k++]='0';
			this_char[4]='\0';

			/*MAC value */
			k = j = strlen(temp2);
			while(j-- > 0)
			{
				temp2[4-k+j] = temp2[j];
			}
			
			while(k < 4)
				temp2[3-k++]='0';
			temp2[4]='\0';

			AtoH(this_char, temp, 2);
			eepAddr = *temp*256 + temp[1];

			AtoH(temp2, temp, 2);
			eepValue = *temp*256 + temp[1];

#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("eepAddr=%02x, eepValue=0x%x\n", eepAddr, eepValue));
#endif /* RELEASE_EXCLUDE */
			RT28xx_EEPROM_WRITE16(pAd, eepAddr, eepValue);
			sprintf(msg+strlen(msg), "[0x%02X]:%02X  ", eepAddr, eepValue);
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
		}
	}
	else
		bIsPrintAllE2P = TRUE;
next:
	if (bIsPrintAllE2P)
	{
		struct file		*file_w;
		RTMP_STRING *fileName = "EEPROMDump.txt";
		mm_segment_t	orig_fs;

		orig_fs = get_fs();
		set_fs(KERNEL_DS); 

		/* open file */
		file_w = filp_open(fileName, O_WRONLY|O_CREAT, 0);
		if (IS_ERR(file_w)) 
		{
			DBGPRINT(RT_DEBUG_TRACE, ("-->2) %s: Error %ld opening %s\n", __FUNCTION__, -PTR_ERR(file_w), fileName));
		}
		else 
		{
			if (file_w->f_op && file_w->f_op->write) 
			{
				file_w->f_pos = 0;
				eepAddr = 0x00;
				
				while (eepAddr <= 0xFE)
				{
					RT28xx_EEPROM_READ16(pAd, eepAddr, eepValue);
					sprintf(msg, "%08x = %04x\n", eepAddr , eepValue);
					
					/* write data to file */
					file_w->f_op->write(file_w, msg, strlen(msg), &file_w->f_pos);
					
					printk("%s", msg);
					eepAddr += 2;
				}
				sprintf(msg, "\nDump all EEPROM values to %s\n", fileName);
			}
			filp_close(file_w, NULL);
		}
		set_fs(orig_fs); 
	}
	if(strlen(msg) == 1)
		sprintf(msg+strlen(msg), "===>Error command format!");

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("copy to user [msg=%s]\n", msg));
#endif /* RELEASE_EXCLUDE */

	/* Copy the information into the user buffer */
	wrq->u.data.length = strlen(msg);
	Status = copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length);

LabelOK:
	if (msg != NULL)
		os_free_mem(NULL, msg);
	if (arg != NULL)
		os_free_mem(NULL, arg);
	
	DBGPRINT(RT_DEBUG_TRACE, ("<==RTMPIoctlE2PROM\n"));
	return;
}


#ifdef RTMP_RF_RW_SUPPORT
/* 
    ==========================================================================
    Description:
        Read / Write RF register
Arguments:
    pAd                    Pointer to our adapter
    wrq                         Pointer to the ioctl argument

    Return Value:
        None

    Note:
        Usage: 
               1.) iwpriv ra0 rf                ==> read all RF registers
               2.) iwpriv ra0 rf 1              ==> read RF where RegID=1
               3.) iwpriv ra0 rf 1=10		    ==> write RF R1=0x10
    ==========================================================================
*/
VOID RTMPIoctlRF(
	IN	PRTMP_ADAPTER	pAd,
	IN	struct iwreq	*wrq)
{
	CHAR				*this_char;
	CHAR				*value;
	UCHAR				regRF = 0;
	CHAR				*mpool, *msg; /*msg[2048]; */
	CHAR				*arg; /*arg[255]; */
	CHAR				*ptr;
	INT					rfId;
	LONG				rfValue;
	BOOLEAN				bIsPrintAllRF = FALSE;
	int maxRFIdx = 31;
	
#ifdef RTMP_RBUS_SUPPORT
/* TODO:Need to add Macversion check! */
	if (pAd->chipCap.MaxNumOfRfId)
		maxRFIdx = pAd->chipCap.MaxNumOfRfId;
#endif /* RTMP_RBUS_SUPPORT */

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("==>RTMPIoctlRF\n"));
#endif /* RELEASE_EXCLUDE */

	mpool = (CHAR *) kmalloc(sizeof(CHAR)*(2048+256+12), MEM_ALLOC_FLAG);
	if (mpool == NULL) {
		return;
	}

	msg = (CHAR *)((ULONG)(mpool+3) & (ULONG)~0x03);
	arg = (CHAR *)((ULONG)(msg+2048+3) & (ULONG)~0x03);

	memset(msg, 0x00, 2048);
	memset(arg, 0x00, 255);
	if (wrq->u.data.length > 1) /*No parameters. */
	{
		NdisMoveMemory(arg, wrq->u.data.pointer, (wrq->u.data.length > 255) ? 255 : wrq->u.data.length);
		arg[254] = 0x00;
		ptr = arg;
		sprintf(msg, "\n");
	    /*Parsing Read or Write */
		while ((this_char = strsep((char **)&ptr, ",")) != NULL)
		{
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("this_char=%s\n", this_char));
#endif /* RELEASE_EXCLUDE */
		if (!*this_char)
			goto next;

		if ((value = strchr(this_char,'=')) != NULL)
			*value++ = 0;

		if (!value || !*value)
		{ /*Read */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("this_char=%s, value=%s\n", this_char, value));
#endif /* RELEASE_EXCLUDE */
			if (sscanf((RTMP_STRING *) this_char, "%d", &(rfId)) == 1)
			{
					if (rfId <= maxRFIdx)
				{
					RT30xxReadRFRegister(pAd, rfId, &regRF);

						sprintf(msg+strlen(msg), "R%02d:%02X  ", rfId, regRF);
#ifdef RELEASE_EXCLUDE
					DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
				}
				else
				{/*Invalid parametes, so default printk all RF */
					bIsPrintAllRF = TRUE;
					goto next;
				}
			}
			else
			{
				/* Invalid parametes, so default printk all RF */
				bIsPrintAllRF = TRUE;
				goto next;
			}
		}
		else
		{ /*Write */
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("this_char=%s, value=%s\n", this_char, value));
#endif /* RELEASE_EXCLUDE */
				if ((sscanf((RTMP_STRING *)this_char, "%d", &(rfId)) == 1) && (sscanf(value, "%lx", &(rfValue)) == 1))
			{
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("rfID=%02d, value=0x%lx\n", rfId, rfValue));
#endif /* RELEASE_EXCLUDE */
					if (rfId <= maxRFIdx)
						{
							RT30xxReadRFRegister(pAd, rfId, &regRF);
						RT30xxWriteRFRegister(pAd, rfId, rfValue);
							/*Read it back for showing */
							RT30xxReadRFRegister(pAd, rfId, &regRF);
						sprintf(msg+strlen(msg), "R%02d:%02X\n", rfId, regRF);
#ifdef RELEASE_EXCLUDE
			                DBGPRINT(RT_DEBUG_INFO, ("msg=%s\n", msg));
#endif /* RELEASE_EXCLUDE */
				                }
				else
					{
					bIsPrintAllRF = TRUE;
						break;
				}
			}
			else
				{
				bIsPrintAllRF = TRUE;
					break;
				}
			}
		}
	}
	else
		bIsPrintAllRF = TRUE;

next:
	/* Copy the information into the user buffer */
	if (bIsPrintAllRF)
	{
		memset(msg, 0x00, 2048);
		sprintf(msg, "\n");
		for (rfId = 0; rfId <= maxRFIdx; rfId++)
		{
			RT30xxReadRFRegister(pAd, rfId, &regRF);
			sprintf(msg+strlen(msg), "R%02d:%02X    ", rfId, regRF);
			if (rfId%5 == 4)
				sprintf(msg+strlen(msg), "\n");
		}
#ifdef RELEASE_EXCLUDE
		DBGPRINT(RT_DEBUG_INFO, ("strlen(msg)=%d\n", (UINT32)strlen(msg)));
#endif /* RELEASE_EXCLUDE */
		wrq->u.data.length = strlen(msg);
		if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length)) 
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));			
		}
	}
	else
	{
		if(strlen(msg) == 1)
			sprintf(msg+strlen(msg), "===>Error command format!");

		wrq->u.data.length = strlen(msg);
		if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));			
		}
	}

	kfree(mpool);
	DBGPRINT(RT_DEBUG_TRACE, ("<==RTMPIoctlRF\n\n"));
}
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* DBG */


#ifdef WMM_ACM_SUPPORT

static void AcmCmdStreamGuiDisplay(
	IN	PRTMP_ADAPTER	pAd,
	OUT char            *extra)
{
	ACM_STREAM_INFO *stream_p, *next_stm_p;
	UINT32 type;
	UINT8 peer_mac[6];
	UINT32 num, size;
	UINT32 category[2] = { ACM_SM_CATEGORY_PEER, ACM_SM_CATEGORY_ACT };
	UINT32 category_num = 1;
	UINT32 i, j;

	/* init */
	NdisZeroMemory(peer_mac, 6);
    NdisMoveMemory(peer_mac, pAd->CommonCfg.Bssid, 6);
    
	type = 1; /* default: EDCA streams */

	for(i=0; i<category_num; i++)
	{
		num = ACMP_StreamNumGet(pAd, category[i], type, peer_mac);

		if (num == 0)
		{
			if (category[i] == ACM_SM_CATEGORY_REQ)
				sprintf(extra, "%s    No any requested TSPEC exists!<br>", extra);
			else
				sprintf(extra, "%s    No any activated TSPEC exists!<br>", extra);
			continue;
		}

		size = sizeof(ACM_STREAM_INFO) * num;
		stream_p = (ACM_STREAM_INFO *)kmalloc(size, GFP_ATOMIC);

		if (stream_p == NULL)
		{
			sprintf(extra, "%sAllocate stream memory fail! "
					"AcmCmdStreamDisplay()<br>", extra);
			return;
		}

		if (ACMP_StreamsGet(pAd, category[i], type,
							&num, peer_mac, stream_p) != ACM_RTN_OK)
		{
			sprintf(extra, "%sGet stream information fail! "
					"AcmCmdStreamDisplay()<br>", extra);
			kfree(stream_p);
			return;
		}

		if (category[i] == ACM_SM_CATEGORY_REQ)
		{
			sprintf(extra, "%s<br>    ------------------- Requested List "
					"-------------------", extra);
		}
		else
		{
			if (category[i] == ACM_SM_CATEGORY_ACT)
			{
				sprintf(extra, "%s<br>    ------------------- ACT stream List "
						"-------------------", extra);
			}
			else
			{
				sprintf(extra, "%s<br>    ------------------- CDB stream List "
						"-------------------", extra);
			}
		}

		for(j=0, next_stm_p=stream_p; j<num; j++)
		{
		    if ((strlen(extra) + 467) > IW_PRIV_SIZE_MASK)
            {
                sprintf(extra, "%s<br><font=red>Still has some information, memory is not enough.</font>", extra);
                break;
            }
			/* display the stream information */
			ACM_CMD_Stream_Gui_Display(pAd, next_stm_p, extra);
			next_stm_p ++;
		}

		kfree(stream_p);
	}
}


static void ACM_CMD_Stream_Gui_Display(
	ACM_PARAM_IN	PRTMP_ADAPTER	ad_p,
	ACM_PARAM_IN	ACM_STREAM_INFO		*stream_p,
	ACM_PARAM_OUT   CHAR                *extra)
{
	ACM_TSPEC *tspec_p = &stream_p->Tspec;
	UINT16 temp;


	sprintf(extra, "%s<br>=== QAP MAC = <font color=#800080>%02x:%02x:%02x:%02x:%02x:%02x</font><br>",
            extra,
			 stream_p->DevMac[0],
			 stream_p->DevMac[1],
			 stream_p->DevMac[2],
			 stream_p->DevMac[3],
			 stream_p->DevMac[4],
			 stream_p->DevMac[5]);

	if (tspec_p->TsInfo.AccessPolicy == ACM_ACCESS_POLICY_EDCA)
	{
		if (stream_p->StreamType == ACM_STREAM_TYPE_11E)
			sprintf(extra, "%sStream Type: EDCA", extra);
		else
			sprintf(extra, "%sStream Type: WME", extra);
	}
	else
	{
		if (tspec_p->TsInfo.AccessPolicy == ACM_ACCESS_POLICY_HCCA)
		{
			if (stream_p->StreamType == ACM_STREAM_TYPE_11E)
				sprintf(extra, "%sStream Type: HCCA", extra);
			else
				sprintf(extra, "%sStream Type: WSM", extra);
		}
		else
		{
			if (stream_p->StreamType == ACM_STREAM_TYPE_11E)
				sprintf(extra, "%sStream Type: HCCA + EDCA", extra);
			else
				sprintf(extra, "%sStream Type: WSN + WME", extra);
		}
	}

	switch(stream_p->Status)
	{
		case TSPEC_STATUS_REQUEST:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Requesting...<br>", extra);
			break;

		case TSPEC_STATUS_ACTIVE:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Active<br>", extra);
			break;

		case TSPEC_STATUS_ACTIVE_SUSPENSION:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Active but suspended<br>", extra);
			break;

		case TSPEC_STATUS_REQ_DELETING:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Requesting & deleting...<br>", extra);
			break;

		case TSPEC_STATUS_ACT_DELETING:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Active & deleting...<br>", extra);
			break;

		case TSPEC_STATUS_RENEGOTIATING:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Renegotiation...<br>", extra);
			break;

		case TSPEC_STATUS_HANDLING:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Request Handling...<br>", extra);
			break;

		case TSPEC_STATUS_FAIL:
			switch(stream_p->Cause)
			{
				case TSPEC_CAUSE_UNKNOWN:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Internal Error!<br>", extra);
					break;

				case TSPEC_CAUSE_REQ_TIMEOUT:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Request (ADDTS) timeout!<br>", extra);;
					break;

				case TSPEC_CAUSE_SUGGESTED_TSPEC:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Suggested TSPEC is provided!<br>", extra);
					break;

				case TSPEC_CAUSE_REJECTED:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Rejected by QAP!<br>", extra);
					break;

				case TSPEC_CAUSE_UNKNOWN_STATUS:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Unknown response status code!<br>", extra);
					break;

				case TSPEC_CAUSE_INACTIVITY_TIMEOUT:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Inactivity timeout!<br>", extra);
					break;

				case TSPEC_CAUSE_DELETED_BY_QAP:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Deleted by QAP!<br>", extra);
					break;

				case TSPEC_CAUSE_DELETED_BY_QSTA:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Deleted by QSTA!<br>", extra);
					break;

				case TSPEC_CAUSE_BANDWIDTH:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) In order to increase bandwidth!<br>", extra);
					break;

				case TSPEC_CAUSE_REJ_MANY_TS:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Reject due to too many TS in a AC!<br>", extra);
					break;

				case TSPEC_CASUE_REJ_INVALID_PARAM:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: (ERR) Reject due to invalid parameters!<br>", extra);
					break;

				default:
					sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Fatal error, unknown cause!<br>", extra);
					break;
			}
			break;

		default:
			sprintf(extra, "%s&nbsp;&nbsp;&nbsp;Status: Fatal error, unknown status!<br>", extra);
			break;
	}

	sprintf(extra, "%sTSID = %d<br>", extra, tspec_p->TsInfo.TSID);

	sprintf(extra, "%sUP = %d<br>", extra, tspec_p->TsInfo.UP);

	if (stream_p->FlgOutLink == 1)
	{
		if (tspec_p->TsInfo.AccessPolicy== ACM_ACCESS_POLICY_EDCA)
        {      
            char *ac_id_str[8]= {"AC_BE", "AC_BK", "AC_BK", "AC_BE", "AC_VI", "AC_VI", "AC_VO", "AC_VO"};
			sprintf(extra, "%sAC ID = %d&nbsp;(%s)<br>", extra, stream_p->AcmAcId, ac_id_str[tspec_p->TsInfo.UP]);
        }
		else
			sprintf(extra, "%sTS ID = %d<br>", extra, stream_p->AcmAcId - ACM_DEV_NUM_OF_AC);
	}

	switch(tspec_p->TsInfo.Direction)
	{
		case ACM_DIRECTION_UP_LINK:
			sprintf(extra, "%sDirection = UP LINK<br>", extra);
			break;

		case ACM_DIRECTION_DOWN_LINK:
			sprintf(extra, "%sDirection = DOWN LINK<br>", extra);
			break;

		case ACM_DIRECTION_DIRECT_LINK:
			sprintf(extra, "%sDirection = DIRECT LINK<br>", extra);
			break;

		case ACM_DIRECTION_BIDIREC_LINK:
			sprintf(extra, "%sDirection = BIDIRECTIONAL LINK<br>", extra);
			break;
	}

	sprintf(extra, "%sInactivity timeout = %u us<br>", extra, stream_p->InactivityCur);

	if (tspec_p->NominalMsduSize& ACM_NOM_MSDU_SIZE_CHECK_BIT)
	{
		sprintf(extra, "%sNorminal MSDU Size (Fixed) = %d B<br>", extra,
				(tspec_p->NominalMsduSize& (~ACM_NOM_MSDU_SIZE_CHECK_BIT)));
	}
	else
	{
		sprintf(extra, "%sNorminal MSDU Size (Variable) = %d B<br>", extra,
			(tspec_p->NominalMsduSize& (~ACM_NOM_MSDU_SIZE_CHECK_BIT)));
	}

	sprintf(extra, "%sInactivity Interval = %u us<br>", extra, tspec_p->InactivityInt);

	if (tspec_p->SuspensionInt!= ACM_TSPEC_SUSPENSION_DISABLE)
		sprintf(extra, "%sSuspension Interval = %u us<br>", extra, tspec_p->SuspensionInt);
	else
		sprintf(extra, "%sSuspension Interval is disabled!<br>", extra);

	sprintf(extra, "%sMean Data Rate = %d bps<br>", extra, tspec_p->MeanDataRate);
	sprintf(extra, "%sMin Physical Rate = %d bps<br>", extra, tspec_p->MinPhyRate);

	if (tspec_p->TsInfo.AccessPolicy!= ACM_ACCESS_POLICY_HCCA)
	{
		/* only for EDCA or HCCA + EDCA */
		temp = tspec_p->SurplusBandwidthAllowance;
		temp = (UINT16)(temp << ACM_SURPLUS_INT_BIT_NUM);
		temp = (UINT16)(temp >> ACM_SURPLUS_INT_BIT_NUM);
		temp = ACM_SurplusFactorDecimalBin2Dec(temp);

		sprintf(extra, "%sSurplus factor = %d.%d<br>", extra,
			(tspec_p->SurplusBandwidthAllowance>> ACM_SURPLUS_DEC_BIT_NUM),
			temp);
		sprintf(extra, "%sMedium Time = %d us<br>", extra, (tspec_p->MediumTime<< 5));
	}
} 

#endif
#endif

