/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2009, Ralink Technology, Inc.
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

	All related CFG80211 function body.

	History:
		1. 2009/09/17	Sample Lin
			(1) Init version.
		2. 2009/10/27	Sample Lin
			(1) Do not use ieee80211_register_hw() to create virtual interface.
				Use wiphy_register() to register nl80211 command handlers.
			(2) Support iw utility.
		3. 2009/11/03	Sample Lin
			(1) Change name MAC80211 to CFG80211.
			(2) Modify CFG80211_OpsChannelSet().
			(3) Move CFG80211_Register()/CFG80211_UnRegister() to open/close.
		4. 2009/12/16	Sample Lin
			(1) Patch for Linux 2.6.32.
			(2) Add more supported functions in CFG80211_Ops.
		5. 2010/12/10	Sample Lin
			(1) Modify for OS_ABL.
		6. 2011/04/19	Sample Lin
			(1) Add more supported functions in CFG80211_Ops v33 ~ 38.

	Note:
		The feature is supported only in "LINUX" 2.6.28 ~ 2.6.38.

***************************************************************************/

#ifdef RELEASE_EXCLUDE
/*
	nl80211_init() of net/wireless/nl80211.c -->
		genl_register_family_with_ops(nl80211_fam, nl80211_ops, ...)

	genl_init() of net/netlink/genetlink.c -->
		register_pernet_subsys(&genl_pernet_ops); -->
		create a genl_sock for genl_rcv()

	genl_rcv() of net/netlink/genetlink.c -->
		receive a command from upper layer by netlink_rcv_skb() -->
		genl_rcv_msg() -->
			genl_get_cmd() -->
				search command for each ops in the family, family->ops_list
			ops->doit() -->
				ex: nl80211_join_ibss() of net/wireless/nl80211.c -->
					cfg80211_join_ibss() of net/wireless/ibss.c -->
						__cfg80211_join_ibss() -->
							rdev->ops->join_ibss() -->
								CFG80211_OpsIbssJoin()

	iw main() -->
		__handle_cmd() -->
		try to find the command structure and execute it, cmd->handler() -->
		send the command to the nl80211 layer by nl_send_auto_complete() -->
		waiting for any response by nl_recvmsgs()

	EX: in iw/ibss.c
		COMMAND(ibss, leave, NULL,
				NL80211_CMD_LEAVE_IBSS, 0, CIB_NETDEV, leave_ibss,
				"Leave the current IBSS cell.");

		#define __COMMAND(_section, _symname, _name, _args, _nlcmd, _flags, _hidden, _idby, _handler, _help, _sel)\
			static struct cmd							\
				__cmd ## _ ## _symname ## _ ## _handler ## _ ## _nlcmd ## _ ## _idby ## _ ## _hidden\
				__attribute__((used)) __attribute__((section("__cmd")))	= {	\
					.name = (_name),					\
					.args = (_args),					\
					.cmd = (_nlcmd),					\
					.nl_msg_flags = (_flags),			\
					.hidden = (_hidden),				\
					.idby = (_idby),					\
					.handler = (_handler),				\
					.help = (_help),					\
					.parent = _section,					\
					.selector = (_sel),					\
			}

		struct cmd {
			const char *name;
			const char *args;
			const char *help;
			const enum nl80211_commands cmd;
			int nl_msg_flags;
			int hidden;
			const enum command_identify_by idby;
			int (*handler)(struct nl80211_state *state,
					   struct nl_cb *cb,
					   struct nl_msg *msg,
					   int argc, char **argv);
			const struct cmd *(*selector)(int argc, char **argv);
			const struct cmd *parent;
		};

		iw will define many command structure bodies for struct cmd.
*/
#endif /* RELEASE_EXCLUDE */

#define RTMP_MODULE_OS

#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"
#include "rt_config.h"
#include "uapi/linux/nl80211.h"

#if defined (HE_BD_CFG80211_SUPPORT) && defined (BD_KERNEL_VER)
#undef  LINUX_VERSION_CODE
#define LINUX_VERSION_CODE KERNEL_VERSION(2,6,39)
#endif /* HE_BD_CFG80211_SUPPORT && BD_KERNEL_VER */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
#ifdef RT_CFG80211_SUPPORT

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
static const struct wiphy_wowlan_support mtk_wowlan_support = {
	.flags = WIPHY_WOWLAN_MAGIC_PKT,
};
#endif
#endif
/* 36 ~ 64, 100 ~ 136, 140 ~ 161 */
#define CFG80211_NUM_OF_CHAN_5GHZ	(sizeof(Cfg80211_Chan)-CFG80211_NUM_OF_CHAN_2GHZ)

#ifdef OS_ABL_FUNC_SUPPORT
/*
	Array of bitrates the hardware can operate with
	in this band. Must be sorted to give a valid "supported
	rates" IE, i.e. CCK rates first, then OFDM.

	For HT, assign MCS in another structure, ieee80211_sta_ht_cap.
*/
const struct ieee80211_rate Cfg80211_SupRate[12] = {
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 10,    /* bitrate in units of 100 Kbps */
		.hw_value = 0,
		.hw_value_short = 0,
	},
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 20,
		.hw_value = 1,
		.hw_value_short = 1,
	},
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 55,
		.hw_value = 2,
		.hw_value_short = 2,
	},
	{
		.flags = IEEE80211_RATE_SHORT_PREAMBLE,
		.bitrate = 110,
		.hw_value = 3,
		.hw_value_short = 3,
	},
	{
		.flags = 0,
		.bitrate = 60,
		.hw_value = 4,
		.hw_value_short = 4,
	},
	{
		.flags = 0,
		.bitrate = 90,
		.hw_value = 5,
		.hw_value_short = 5,
	},
	{
		.flags = 0,
		.bitrate = 120,
		.hw_value = 6,
		.hw_value_short = 6,
	},
	{
		.flags = 0,
		.bitrate = 180,
		.hw_value = 7,
		.hw_value_short = 7,
	},
	{
		.flags = 0,
		.bitrate = 240,
		.hw_value = 8,
		.hw_value_short = 8,
	},
	{
		.flags = 0,
		.bitrate = 360,
		.hw_value = 9,
		.hw_value_short = 9,
	},
	{
		.flags = 0,
		.bitrate = 480,
		.hw_value = 10,
		.hw_value_short = 10,
	},
	{
		.flags = 0,
		.bitrate = 540,
		.hw_value = 11,
		.hw_value_short = 11,
	},
};
#endif /* OS_ABL_FUNC_SUPPORT */

/* all available channels */
static const UCHAR Cfg80211_Chan[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,

	/* 802.11 UNI / HyperLan 2 */
	36, 38, 40, 44, 46, 48, 52, 54, 56, 60, 62, 64,

	/* 802.11 HyperLan 2 */
	100, 104, 108, 112, 116, 118, 120, 124, 126, 128, 132, 134, 136,

	/* 802.11 UNII */
	140, 149, 151, 153, 157, 159, 161, 165, 167, 169, 171, 173,

	/* Japan */
	184, 188, 192, 196, 208, 212, 216,
};


static const UINT32 CipherSuites[] = {
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,
#ifdef DOT11W_PMF_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	WLAN_CIPHER_SUITE_AES_CMAC,
#endif /* LINUX_VERSION_CODE */
#endif /* DOT11W_PMF_SUPPORT */

};

/*
	The driver's regulatory notification callback.
*/
/*Nobody uses it currently*/
#if 0
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
static INT32 CFG80211_RegNotifier(
	IN struct wiphy					*pWiphy,
	IN struct regulatory_request	*pRequest);
#else
static INT32 CFG80211_RegNotifier(
	IN struct wiphy					*pWiphy,
	IN enum reg_set_by				Request);
#endif /* LINUX_VERSION_CODE */
#endif


/* get RALINK pAd control block in 80211 Ops */
#define MAC80211_PAD_GET(__pAd, __pWiphy)							\
	{																\
		ULONG *__pPriv;												\
		__pPriv = (ULONG *)(wiphy_priv(__pWiphy));					\
		__pAd = (VOID *)(*__pPriv);									\
		if (__pAd == NULL)											\
		{															\
			DBGPRINT(RT_DEBUG_ERROR,								\
					("80211> %s but pAd = NULL!", __FUNCTION__));	\
			return -EINVAL;											\
		}															\
	}



/* get RALINK pAd control block in 80211 Ops return void */
#define void_MAC80211_PAD_GET(__pAd, __pWiphy)							\
	{																\
		ULONG *__pPriv;												\
		__pPriv = (ULONG *)(wiphy_priv(__pWiphy));					\
		__pAd = (VOID *)(*__pPriv);									\
		if (__pAd == NULL)											\
		{															\
			DBGPRINT(RT_DEBUG_ERROR,								\
					("80211> %s but pAd = NULL!", __FUNCTION__));	\
			return ;											\
		}															\
	}


/* get RALINK pAd control block in 80211 Ops */
#define null_MAC80211_PAD_GET(__pAd, __pWiphy)							\
	{																\
		ULONG *__pPriv;												\
		__pPriv = (ULONG *)(wiphy_priv(__pWiphy));					\
		__pAd = (VOID *)(*__pPriv);									\
		if (__pAd == NULL)											\
		{															\
			DBGPRINT(RT_DEBUG_ERROR,								\
					("80211> %s but pAd = NULL!", __FUNCTION__));	\
			return NULL;											\
		}															\
	}

/*

========================================================================
Routine Description:
	Set channel.

Arguments:
	pWiphy			- Wireless hardware description
	pChan			- Channel information
	ChannelType		- Channel type

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: set channel, set freq

	enum nl80211_channel_type {
		NL80211_CHAN_NO_HT,
		NL80211_CHAN_HT20,
		NL80211_CHAN_HT40MINUS,
		NL80211_CHAN_HT40PLUS
	};
========================================================================
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
static int CFG80211_OpsChannelSet(
	IN struct wiphy					*pWiphy,
	IN struct net_device			*pDev,
	IN struct ieee80211_channel		*pChan,
	IN enum nl80211_channel_type	ChannelType)
#else
static int CFG80211_OpsChannelSet(
	IN struct wiphy					*pWiphy,
	IN struct ieee80211_channel		*pChan,
	IN enum nl80211_channel_type	ChannelType)
#endif /* LINUX_VERSION_CODE */
{
	VOID *pAd;
	CFG80211_CB *p80211CB;
	CMD_RTPRIV_IOCTL_80211_CHAN ChanInfo;
	UINT32 ChanId;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	/* get channel number */
	ChanId = ieee80211_frequency_to_channel(pChan->center_freq);
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> Channel = %d, Type = %d\n", ChanId, ChannelType));

	/* init */
	memset(&ChanInfo, 0, sizeof(ChanInfo));
	ChanInfo.ChanId = ChanId;

	p80211CB = NULL;
	RTMP_DRIVER_80211_CB_GET(pAd, &p80211CB);

	if (p80211CB == NULL)
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> p80211CB == NULL!\n"));
		return 0;
	}

	ChanInfo.IfType = pDev->ieee80211_ptr->iftype;

	if (ChannelType == NL80211_CHAN_NO_HT)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_NOHT;
	else if (ChannelType == NL80211_CHAN_HT20)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT20;
	else if (ChannelType == NL80211_CHAN_HT40MINUS)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT40MINUS;
	else if (ChannelType == NL80211_CHAN_HT40PLUS)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT40PLUS;

	ChanInfo.MonFilterFlag = p80211CB->MonFilterFlag;

	/* set channel */
	RTMP_DRIVER_80211_CHAN_SET(pAd, &ChanInfo);

	return 0;
} /* End of CFG80211_OpsChannelSet */
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static int CFG80211_OpsMonitorChannelSet(struct wiphy *pWiphy,
					 struct cfg80211_chan_def *chandef)
{
	VOID *pAd;
	CFG80211_CB *p80211CB;
	CMD_RTPRIV_IOCTL_80211_CHAN ChanInfo;
	UINT32 ChanId;

	struct device *pDev = pWiphy->dev.parent;
	struct net_device *pNetDev = dev_get_drvdata(pDev);
	struct ieee80211_channel		*pChan;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	//return 0;
	MAC80211_PAD_GET(pAd, pWiphy);
	pChan=chandef->chan;
	

	
	    CFG80211DBG(RT_DEBUG_OFF, ("control:%d MHz width:%d center: %d/%d MHz",
	     pChan->center_freq, chandef->width,
	     chandef->center_freq1, chandef->center_freq2));
	

	/* get channel number */
	ChanId = ieee80211_frequency_to_channel(pChan->center_freq);
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> Channel = %d\n", ChanId));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> ChannelType = %d\n", chandef->width));

	/* init */
	memset(&ChanInfo, 0, sizeof(ChanInfo));
	ChanInfo.ChanId = (UINT8)ChanId;

	p80211CB = NULL;
	RTMP_DRIVER_80211_CB_GET(pAd, &p80211CB);

	if (p80211CB == NULL)
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> p80211CB == NULL!\n"));
		return 0;
} 

	ChanInfo.IfType = (UINT8)pNetDev->ieee80211_ptr->iftype;

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> ChanInfo.IfType == %d!\n",ChanInfo.IfType));

	if (cfg80211_get_chandef_type(chandef) == NL80211_CHAN_NO_HT)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_NOHT;
	else if (cfg80211_get_chandef_type(chandef) == NL80211_CHAN_HT20)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT20;
	else if (cfg80211_get_chandef_type(chandef) == NL80211_CHAN_HT40MINUS)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT40MINUS;
	else if (cfg80211_get_chandef_type(chandef) == NL80211_CHAN_HT40PLUS)
		ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT40PLUS;

	ChanInfo.MonFilterFlag = p80211CB->MonFilterFlag;

	/* set channel */
	RTMP_DRIVER_80211_CHAN_SET(pAd, &ChanInfo);

	return 0;
} /* End of CFG80211_OpsChannelSet */
#endif

/*
========================================================================
Routine Description:
	Change type/configuration of virtual interface.

Arguments:
	pWiphy			- Wireless hardware description
	IfIndex			- Interface index
	Type			- Interface type, managed/adhoc/ap/station, etc.
	pFlags			- Monitor flags
	pParams			- Mesh parameters

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: set type, set monitor
========================================================================
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0))
static int CFG80211_OpsVirtualInfChg(
	IN struct wiphy					*pWiphy,
	IN struct net_device			*pNetDevIn,
	IN enum nl80211_iftype			Type,
	struct vif_params				*pParams)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
static int CFG80211_OpsVirtualInfChg(
	IN struct wiphy					*pWiphy,
	IN struct net_device			*pNetDevIn,
	IN enum nl80211_iftype			Type,
	IN UINT32							*pFlags,
	struct vif_params				*pParams)
#else
static int CFG80211_OpsVirtualInfChg(
	IN struct wiphy					*pWiphy,
	IN int							IfIndex,
	IN enum nl80211_iftype			Type,
	IN UINT32							*pFlags,
	struct vif_params				*pParams)
#endif /* LINUX_VERSION_CODE */
{
	VOID *pAd;
	CFG80211_CB *pCfg80211_CB;
	struct net_device *pNetDev;
	CMD_RTPRIV_IOCTL_80211_VIF_PARM VifInfo;
	UINT oldType = pNetDevIn->ieee80211_ptr->iftype;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s (%s) IfTypeChange %d ==> %d\n",
			__FUNCTION__, pNetDevIn->name, oldType, Type));
	MAC80211_PAD_GET(pAd, pWiphy);

	/* sanity check */
#ifdef CONFIG_STA_SUPPORT
	if ((Type != NL80211_IFTYPE_ADHOC) &&
		(Type != NL80211_IFTYPE_STATION) &&
		(Type != NL80211_IFTYPE_MONITOR) &&
		(Type != NL80211_IFTYPE_AP) 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	    && (Type != NL80211_IFTYPE_P2P_CLIENT) 
	    && (Type != NL80211_IFTYPE_P2P_GO)
#endif /* LINUX_VERSION_CODE 2.6.37 */
	)
#endif /* CONFIG_STA_SUPPORT */
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Wrong interface type %d!\n", Type));
		return -EINVAL;
	} /* End of if */

	/* update interface type */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
	pNetDev = pNetDevIn;
#else
	pNetDev = __dev_get_by_index(&init_net, IfIndex);
#endif /* LINUX_VERSION_CODE */

	if (pNetDev == NULL)
		return -ENODEV;

	pNetDev->ieee80211_ptr->iftype = Type;

	VifInfo.net_dev = pNetDev;
	VifInfo.newIfType = (UINT8)Type;
	VifInfo.oldIfType = (UINT8)oldType;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0))
	if (pFlags != NULL)
	{
		VifInfo.MonFilterFlag = 0;

		if (((*pFlags) & NL80211_MNTR_FLAG_FCSFAIL) == NL80211_MNTR_FLAG_FCSFAIL)
			VifInfo.MonFilterFlag |= RT_CMD_80211_FILTER_FCSFAIL;

		if (((*pFlags) & NL80211_MNTR_FLAG_FCSFAIL) == NL80211_MNTR_FLAG_PLCPFAIL)
			VifInfo.MonFilterFlag |= RT_CMD_80211_FILTER_PLCPFAIL;

		if (((*pFlags) & NL80211_MNTR_FLAG_CONTROL) == NL80211_MNTR_FLAG_CONTROL)
			VifInfo.MonFilterFlag |= RT_CMD_80211_FILTER_CONTROL;

		if (((*pFlags) & NL80211_MNTR_FLAG_CONTROL) == NL80211_MNTR_FLAG_OTHER_BSS)
			VifInfo.MonFilterFlag |= RT_CMD_80211_FILTER_OTHER_BSS;
	} 
#endif

	/* Type transer from linux to driver defined */
	if (Type == NL80211_IFTYPE_STATION)
	{	
		Type = RT_CMD_80211_IFTYPE_STATION;
	}
	else if (Type == NL80211_IFTYPE_ADHOC)
	{
		Type = RT_CMD_80211_IFTYPE_ADHOC;
	}
	else if (Type == NL80211_IFTYPE_MONITOR)
	{
		Type = RT_CMD_80211_IFTYPE_MONITOR;
	}
#ifdef CONFIG_AP_SUPPORT		
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	else if (Type == NL80211_IFTYPE_P2P_CLIENT)
	{
		Type = RT_CMD_80211_IFTYPE_P2P_CLIENT;
	}
	else if (Type == NL80211_IFTYPE_P2P_GO)
	{
		Type = RT_CMD_80211_IFTYPE_P2P_GO;
	}	
#endif /* LINUX_VERSION_CODE 2.6.37 */
#endif /* CONFIG_AP_SUPPORT */

	RTMP_DRIVER_80211_VIF_CHG(pAd, &VifInfo);

	/*CFG_TODO*/
	RTMP_DRIVER_80211_CB_GET(pAd, &pCfg80211_CB);
	pCfg80211_CB->MonFilterFlag = VifInfo.MonFilterFlag;
	return 0;
} 

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
#if defined(SIOCGIWSCAN) || defined(RT_CFG80211_SUPPORT)
extern int rt_ioctl_siwscan(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wreq, char *extra);
#endif /* LINUX_VERSION_CODE: 2.6.30 */
/*
========================================================================
Routine Description:
	Request to do a scan. If returning zero, the scan request is given
	the driver, and will be valid until passed to cfg80211_scan_done().
	For scan results, call cfg80211_inform_bss(); you can call this outside
	the scan/scan_done bracket too.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pRequest		- Scan request

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: scan

	struct cfg80211_scan_request {
		struct cfg80211_ssid *ssids;
		int n_ssids;
		struct ieee80211_channel **channels;
		UINT32 n_channels;
		const u8 *ie;
		size_t ie_len;

	 * @ssids: SSIDs to scan for (active scan only)
	 * @n_ssids: number of SSIDs
	 * @channels: channels to scan on.
	 * @n_channels: number of channels for each band
	 * @ie: optional information element(s) to add into Probe Request or %NULL
	 * @ie_len: length of ie in octets
========================================================================
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static int CFG80211_OpsScan(
	IN struct wiphy					*pWiphy,
	IN struct cfg80211_scan_request *pRequest)
#else
static int CFG80211_OpsScan(
	IN struct wiphy					*pWiphy,
	IN struct net_device			*pNdev,
	IN struct cfg80211_scan_request *pRequest)
#endif /* LINUX_VERSION_CODE: 3.6.0 */
{
#ifdef CONFIG_STA_SUPPORT
	VOID *pAd;
	CFG80211_CB *pCfg80211_CB;

	struct iw_scan_req IwReq;
	union iwreq_data Wreq;

	RTMP_ADAPTER *pAdTemp;
	int i = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
	struct net_device *pNdev = NULL;
	pNdev=pRequest->wdev->netdev;
#endif /* LINUX_VERSION_CODE: 3.6.0 */
	MAC80211_PAD_GET(pAd, pWiphy);

	pAdTemp = (RTMP_ADAPTER *)pAd;

	CFG80211DBG(RT_DEBUG_TRACE, ("========================================================================\n"));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==> %s(%d)\n", __FUNCTION__, pNdev->name, pNdev->ieee80211_ptr->iftype));

	/* YF_TODO: record the scan_req per netdevice */
	RTMP_DRIVER_80211_CB_GET(pAd, &pCfg80211_CB);
	pCfg80211_CB->pCfg80211_ScanReq = pRequest; /* used in scan end */

	/* sanity check */
	if ((pNdev->ieee80211_ptr->iftype != NL80211_IFTYPE_STATION) &&
		(pNdev->ieee80211_ptr->iftype != NL80211_IFTYPE_AP) &&
	    (pNdev->ieee80211_ptr->iftype != NL80211_IFTYPE_ADHOC) 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	   && (pNdev->ieee80211_ptr->iftype != NL80211_IFTYPE_P2P_CLIENT)
#endif /* LINUX_VERSION_CODE: 2.6.37 */
	)
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> DeviceType Not Support Scan ==> %d\n", pNdev->ieee80211_ptr->iftype));
		CFG80211OS_ScanEnd(pCfg80211_CB, TRUE);
		return -EOPNOTSUPP;
	} 
	
	/* Driver Internal SCAN SM Check */	
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Network is down!\n"));
		CFG80211OS_ScanEnd(pCfg80211_CB, TRUE);
		return -ENETDOWN;
	} 

	if (RTMP_DRIVER_80211_SCAN(pAd, pNdev->ieee80211_ptr->iftype) != NDIS_STATUS_SUCCESS)
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("\n\n\n\n\n80211> BUSY - SCANING \n\n\n\n\n"));
		CFG80211OS_ScanEnd(pCfg80211_CB, TRUE);
		return 0;
	}


	if (pRequest->ie_len != 0 ) 
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> ExtraIEs Not Null in ProbeRequest from upper layer...\n"));
		/* YF@20120321: Using Cfg80211_CB carry on pAd struct to overwirte the pWpsProbeReqIe. */
		RTMP_DRIVER_80211_SCAN_EXTRA_IE_SET(pAd);		
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> ExtraIEs Null in ProbeRequest from upper layer...\n"));
	}

	memset(&Wreq, 0, sizeof(Wreq));
	memset(&IwReq, 0, sizeof(IwReq));

	DBGPRINT(RT_DEBUG_INFO, ("80211> Num %d of SSID from upper layer...\n",  
		pRequest->n_ssids));

	/* %NULL or zero-length SSID is used to indicate wildcard */
	if ((pRequest->n_ssids == 0) || !pRequest->ssids)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("80211> n_ssids == 0 Wildcard SSID.\n"));
		Wreq.data.flags |= IW_SCAN_ALL_ESSID;
	}
	else if ((pRequest->n_ssids == 1) && (pRequest->ssids->ssid_len ==0))
	{	
		DBGPRINT(RT_DEBUG_TRACE, ("80211> Wildcard SSID In ProbeRequest.\n"));
		Wreq.data.flags |= IW_SCAN_ALL_ESSID;
	} 
	else
	{
		/*DBGPRINT(RT_DEBUG_TRACE, ("80211> Named SSID [%s] In ProbeRequest.\n",  pRequest->ssids->ssid));*/
		Wreq.data.flags |= IW_SCAN_THIS_ESSID;
		memset(&pAdTemp->MlmeAux.params, 0, sizeof(pAdTemp->MlmeAux.params));
		for (i = 0; i < pRequest->n_ssids; ++i) {
			DBGPRINT(RT_DEBUG_TRACE, ("80211> Named SSID [%s] In ProbeRequest.\n",
				pRequest->ssids[i].ssid));
			pAdTemp->MlmeAux.params.ssids[i].ssid_len =
				(pRequest->ssids[i].ssid_len > MAX_LEN_OF_SSID) ? MAX_LEN_OF_SSID : pRequest->ssids[i].ssid_len;
			memcpy(pAdTemp->MlmeAux.params.ssids[i].ssid, pRequest->ssids[i].ssid,
				pAdTemp->MlmeAux.params.ssids[i].ssid_len);

			pAdTemp->MlmeAux.params.num_ssids++;
			if (pAdTemp->MlmeAux.params.num_ssids + 1 >= WPAS_MAX_SCAN_SSIDS)
				break;
		}
	}

	/* Set Channel List for this Scan Action */	
	DBGPRINT(RT_DEBUG_INFO, ("80211> [%d] Channels In ProbeRequest.\n",  pRequest->n_channels)); 
	if ( pRequest->n_channels > 0 ) 
	{
		UINT32 *pChanList;
		UINT idx;
		os_alloc_mem(NULL, (UCHAR **)&pChanList, sizeof(UINT32 *) * pRequest->n_channels);
    	if (pChanList == NULL)
    	{
    		DBGPRINT(RT_DEBUG_ERROR, ("%s::Alloc memory fail\n", __FUNCTION__));
        		return FALSE;
    	}

		for(idx=0; idx < pRequest->n_channels; idx++) 
		{			
			pChanList[idx] = ieee80211_frequency_to_channel(pRequest->channels[idx]->center_freq);
			CFG80211DBG(RT_DEBUG_INFO, ("%d,", pChanList[idx]));
		}
		CFG80211DBG(RT_DEBUG_INFO, ("\n"));

		RTMP_DRIVER_80211_SCAN_CHANNEL_LIST_SET(pAd, pChanList, pRequest->n_channels);

		if (pChanList)
			os_free_mem(NULL, pChanList);	
	}
	
	/* use 1st SSID in the requested SSID list */
	if (pRequest->n_ssids && pRequest->ssids) {
		IwReq.essid_len = pRequest->ssids->ssid_len;
		memcpy(IwReq.essid, pRequest->ssids->ssid, sizeof(IwReq.essid));	
	}
	Wreq.data.length = sizeof(struct iw_scan_req);

	IwReq.scan_type = SCAN_ACTIVE;
#ifdef RT_CFG80211_P2P_SUPPORT
	if ((pNdev->ieee80211_ptr->iftype == NL80211_IFTYPE_P2P_CLIENT)	
	    || (pNdev->ieee80211_ptr->iftype == NL80211_IFTYPE_P2P_GO)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0))
            || (pNdev->ieee80211_ptr->iftype == NL80211_IFTYPE_P2P_DEVICE)
#endif /* LINUX_VERSION_CODE: 3.7.0 */
           )
	{
		IwReq.scan_type = SCAN_P2P;
	}
#ifdef RT_CFG80211_P2P_STATIC_CONCURRENT_DEVICE
        if (strcmp(pNdev->name, "p2p0") == 0)
        {
                IwReq.scan_type = SCAN_P2P;
        }
#endif /* RT_CFG80211_P2P_STATIC_CONCURRENT_DEVICE */


#endif /* RT_CFG80211_P2P_SUPPORT */

	rt_ioctl_siwscan(pNdev, NULL, &Wreq, (char *)&IwReq);
	return 0;

#else 
	return -EOPNOTSUPP;
#endif /* CONFIG_STA_SUPPORT */
} 
#endif /* LINUX_VERSION_CODE */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31))
#ifdef CONFIG_STA_SUPPORT
/*
========================================================================
Routine Description:
	Join the specified IBSS (or create if necessary). Once done, call
	cfg80211_ibss_joined(), also call that function when changing BSSID due
	to a merge.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pParams			- IBSS parameters

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: ibss join

	No fixed-freq and fixed-bssid support.
========================================================================
*/
static int CFG80211_OpsIbssJoin(
	IN struct wiphy					*pWiphy,
	IN struct net_device			*pNdev,
	IN struct cfg80211_ibss_params	*pParams)
{
	VOID *pAd;
	UCHAR *beacon_buf = NULL;
	CMD_RTPRIV_IOCTL_80211_IBSS IbssInfo;
	UINT channel = 0;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	/* init */
	memset(&IbssInfo, 0, sizeof(IbssInfo));
	IbssInfo.BeaconInterval = pParams->beacon_interval;
	memcpy(&IbssInfo.Ssid, pParams->ssid, pParams->ssid_len);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
	channel = pParams->chandef.chan->center_freq;
#else
	channel = pParams->channel->center_freq;
#endif /* LINUX_VERSION_CODE 3.8.0 */ 	

	CFG80211DBG(RT_DEBUG_OFF, ("80211> SSID = %s, BI = %d, CH = %d, CH_FIX = %d, Privacy = %d\n",
                                IbssInfo.Ssid, pParams->beacon_interval, 
				channel, pParams->channel_fixed, pParams->privacy));
	
	if (pParams->ie && (pParams->ie_len > 0))
	{
		hex_dump("ADHOC_IE", pParams->ie , pParams->ie_len);
		IbssInfo.BeaconExtraIeLen = pParams->ie_len;

		os_alloc_mem(NULL, &beacon_buf, pParams->ie_len);
		NdisCopyMemory(beacon_buf, pParams->ie, pParams->ie_len);
		IbssInfo.BeaconExtraIe = beacon_buf;
	}

	if (pParams->privacy)
	{
		IbssInfo.privacy = pParams->privacy;
	}

	if (pParams->bssid)
	{
		CFG80211DBG(RT_DEBUG_OFF, ("Join this BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n",
                                PRINT_MAC(pParams->bssid)));
	}
	else
	{
		CFG80211DBG(RT_DEBUG_OFF, ("Can't find any AdHoc, create IBSS\n"));

	}

	/* ibss join */
	RTMP_DRIVER_80211_IBSS_JOIN(pAd, &IbssInfo);

	if (beacon_buf)
	{
	    os_free_mem(NULL, beacon_buf);
		beacon_buf = NULL;
	}

	return 0;
} /* End of CFG80211_OpsIbssJoin */


/*
========================================================================
Routine Description:
	Leave the IBSS.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: ibss leave
========================================================================
*/
static int CFG80211_OpsIbssLeave(
	IN struct wiphy					*pWiphy,
	IN struct net_device			*pNdev)
{
	VOID *pAd;


	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	//CFG_TODO
	RTMP_DRIVER_80211_STA_LEAVE(pAd, pNdev);
	return 0;
}
#endif /* CONFIG_STA_SUPPORT */
#endif /* LINUX_VERSION_CODE */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
/*
========================================================================
Routine Description:
	Set the transmit power according to the parameters.

Arguments:
	pWiphy			- Wireless hardware description
	Type			- 
	dBm				- dBm

Return Value:
	0				- success
	-x				- fail

Note:
	Type -
	enum nl80211_tx_power_setting - TX power adjustment
	 @NL80211_TX_POWER_AUTOMATIC: automatically determine transmit power
	 @NL80211_TX_POWER_LIMITED: limit TX power by the mBm parameter
	 @NL80211_TX_POWER_FIXED: fix TX power to the mBm parameter
========================================================================
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
static int CFG80211_OpsTxPwrSet(
	IN struct wiphy						*pWiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
	IN struct wireless_dev *wdev,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))	
	IN enum nl80211_tx_power_setting	Type,
#else
	IN enum tx_power_setting			Type,
#endif /* LINUX_VERSION_CODE */	
	IN int								dBm)
{

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	return -EOPNOTSUPP;
} 


/*
========================================================================
Routine Description:
	Store the current TX power into the dbm variable.

Arguments:
	pWiphy			- Wireless hardware description
	pdBm			- dBm

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsTxPwrGet(
	IN struct wiphy						*pWiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
	IN struct wireless_dev *wdev,
#endif
	IN int								*pdBm)
{
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	return -EOPNOTSUPP;
} /* End of CFG80211_OpsTxPwrGet */

#endif

/*
========================================================================
Routine Description:
	Power management.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- 
	FlgIsEnabled	-
	Timeout			-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsPwrMgmt(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN bool 							 enabled, 
	IN INT32 							 timeout)	
{
	VOID *pAd;
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==> power save %s\n", __FUNCTION__,(enabled ? "enable" : "disable")));

	MAC80211_PAD_GET(pAd, pWiphy);

	RTMP_DRIVER_80211_POWER_MGMT_SET(pAd,enabled);
	return 0;
} 


/*
========================================================================
Routine Description:
	Get information for a specific station.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	pMac			- STA MAC
	pSinfo			- STA INFO

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsStaGet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
	IN const UINT8						* pMac,
#else
	IN UINT8						* pMac,
#endif
	IN struct station_info				*pSinfo)
{
	VOID *pAd;
	CMD_RTPRIV_IOCTL_80211_STA StaInfo;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	/* init */
	memset(pSinfo, 0, sizeof(*pSinfo));
	memset(&StaInfo, 0, sizeof(StaInfo));

	memcpy(StaInfo.MAC, pMac, 6);

	/* get sta information */
	if (RTMP_DRIVER_80211_STA_GET(pAd, &StaInfo) != NDIS_STATUS_SUCCESS)
		return -ENOENT;

	if (StaInfo.TxRateFlags != RT_CMD_80211_TXRATE_LEGACY)
	{
		pSinfo->txrate.flags = RATE_INFO_FLAGS_MCS;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
		pSinfo->txrate.bw = RATE_INFO_BW_20;
		if (StaInfo.TxRateFlags & RT_CMD_80211_TXRATE_BW_40)
			pSinfo->txrate.bw = RATE_INFO_BW_40;
#else
		if (StaInfo.TxRateFlags & RT_CMD_80211_TXRATE_BW_40)
			pSinfo->txrate.flags |= RATE_INFO_FLAGS_40_MHZ_WIDTH;
#endif


		if (StaInfo.TxRateFlags & RT_CMD_80211_TXRATE_SHORT_GI)
			pSinfo->txrate.flags |= RATE_INFO_FLAGS_SHORT_GI;


		pSinfo->txrate.mcs = (u8)StaInfo.TxRateMCS;
	}
	else
	{
		pSinfo->txrate.legacy = (u16)StaInfo.TxRateMCS;
	} 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_TX_BITRATE);
#else
	pSinfo->filled |= STATION_INFO_TX_BITRATE;
#endif

	/* fill signal */
	pSinfo->signal = (s8)StaInfo.Signal;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);
#else
	pSinfo->filled |= STATION_INFO_SIGNAL;
#endif

#ifdef CONFIG_AP_SUPPORT
	/* fill tx count */
	pSinfo->tx_packets = StaInfo.TxPacketCnt;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_TX_PACKETS);
#else
	pSinfo->filled |= STATION_INFO_TX_PACKETS;
#endif

	/* fill inactive time */
	pSinfo->inactive_time = StaInfo.InactiveTime;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_INACTIVE_TIME);
#else
	pSinfo->filled |= STATION_INFO_INACTIVE_TIME;
#endif
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	/* fill tx/rx count */
	pSinfo->tx_packets = StaInfo.tx_packets;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_TX_PACKETS);
#else
	pSinfo->filled |= STATION_INFO_TX_PACKETS;
#endif

	pSinfo->tx_retries = StaInfo.tx_retries;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_TX_RETRIES);
#else
	pSinfo->filled |= STATION_INFO_TX_RETRIES;
#endif


	pSinfo->tx_failed = StaInfo.tx_failed;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_TX_FAILED);
#else
	pSinfo->filled |= STATION_INFO_TX_FAILED;
#endif


	pSinfo->rx_packets = StaInfo.rx_packets;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_RX_PACKETS);
#else
	pSinfo->filled |= STATION_INFO_RX_PACKETS;
#endif

	/* fill inactive time */
	pSinfo->inactive_time = StaInfo.InactiveTime;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
	pSinfo->filled |= BIT(NL80211_STA_INFO_INACTIVE_TIME);
#else
	pSinfo->filled |= STATION_INFO_INACTIVE_TIME;
#endif
#endif /* CONFIG_STA_SUPPORT */

	return 0;
} 


/*
========================================================================
Routine Description:
	List all stations known, e.g. the AP on managed interfaces.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	Idx				- 
	pMac			-
	pSinfo			-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsStaDump(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN int								Idx,
	IN UINT8							*pMac,
	IN struct station_info				*pSinfo)
{
	VOID *pAd;

	if (Idx != 0)
		return -ENOENT;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

#ifdef CONFIG_STA_SUPPORT
	if (RTMP_DRIVER_AP_SSID_GET(pAd, pMac) != NDIS_STATUS_SUCCESS)
		return -EBUSY;
	else
		return CFG80211_OpsStaGet(pWiphy, pNdev, pMac, pSinfo);
#endif /* CONFIG_STA_SUPPORT */

	return -EOPNOTSUPP;
} /* End of CFG80211_OpsStaDump */


/*
========================================================================
Routine Description:
	Notify that wiphy parameters have changed.

Arguments:
	pWiphy			- Wireless hardware description
	Changed			-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsWiphyParamsSet(
	IN struct wiphy						*pWiphy,
	IN UINT32							Changed)
{
	VOID *pAd;
	
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);
	if (Changed & WIPHY_PARAM_RTS_THRESHOLD)
	{
		RTMP_DRIVER_80211_RTS_THRESHOLD_ADD(pAd, (void *)&pWiphy->rts_threshold);
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==> rts_threshold(%d)\n", __FUNCTION__,pWiphy->rts_threshold));
		return 0;		
	} else if (Changed & WIPHY_PARAM_FRAG_THRESHOLD) {
		RTMP_DRIVER_80211_FRAG_THRESHOLD_ADD(pAd, (void *)&pWiphy->frag_threshold);
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==> frag_threshold(%d)\n", __FUNCTION__,pWiphy->frag_threshold));
		return 0;		
	}
		
	return -EOPNOTSUPP;
} /* End of CFG80211_OpsWiphyParamsSet */


/*
========================================================================
Routine Description:
	Add a key with the given parameters.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	KeyIdx			-
	Pairwise		-
	pMacAddr		-
	pParams			-

Return Value:
	0				- success
	-x				- fail

Note:
	pMacAddr will be NULL when adding a group key.
========================================================================
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
static int CFG80211_OpsKeyAdd(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN bool								Pairwise,
	IN const UINT8						*pMacAddr,
	IN struct key_params				*pParams)
#else

static int CFG80211_OpsKeyAdd(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN const UINT8						*pMacAddr,
	IN struct key_params				*pParams)
#endif /* LINUX_VERSION_CODE */
{
	VOID *pAd;
	CMD_RTPRIV_IOCTL_80211_KEY KeyInfo;
	UINT8 i = 0;

	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	/* pAd sould not be null unless pWiphy is incorrect */
	if (unlikely(!pParams->key))
		return -EINVAL;

#ifdef RT_CFG80211_DEBUG
	hex_dump("KeyBuf=", (UINT8 *)pParams->key, pParams->key_len);
#endif /* RT_CFG80211_DEBUG */

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> KeyIdx = %d\n", KeyIdx));

	if (pParams->key_len >= sizeof(KeyInfo.KeyBuf))
		return -EINVAL;
	/* End of if */

	/* To avoid all-zero key install */
	for (i = 0; i < pParams->key_len; ++i) {
		if (pParams->key[i])
			break;
	}
	if (i == pParams->key_len) {
		CFG80211DBG(RT_DEBUG_OFF, ("Skip key add for all zero key\n"));
		return 0;
	}

	/* init */
	memset(&KeyInfo, 0, sizeof(KeyInfo));
	memcpy(KeyInfo.KeyBuf, pParams->key, pParams->key_len);
	KeyInfo.KeyBuf[pParams->key_len] = 0x00;
	KeyInfo.KeyId = KeyIdx;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))		
	KeyInfo.bPairwise = (BOOLEAN)Pairwise;
#endif /* LINUX_VERSION_CODE: 2,6,37 */
	KeyInfo.KeyLen = (UINT8)pParams->key_len;
		
	if ((pParams->cipher == WLAN_CIPHER_SUITE_WEP40))
	{
		KeyInfo.KeyType = RT_CMD_80211_KEY_WEP40;
	}
	else if ((pParams->cipher == WLAN_CIPHER_SUITE_WEP104))
	{
		KeyInfo.KeyType = RT_CMD_80211_KEY_WEP104;
	}
	else if ((pParams->cipher == WLAN_CIPHER_SUITE_TKIP) ||
		(pParams->cipher == WLAN_CIPHER_SUITE_CCMP))
	{
		KeyInfo.KeyType = RT_CMD_80211_KEY_WPA;
		if (pParams->cipher == WLAN_CIPHER_SUITE_TKIP)
			KeyInfo.cipher = Ndis802_11TKIPEnable;
		else if (pParams->cipher == WLAN_CIPHER_SUITE_CCMP)
			KeyInfo.cipher = Ndis802_11AESEnable;		
	}
#ifdef DOT11W_PMF_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	else if (pParams->cipher == WLAN_CIPHER_SUITE_AES_CMAC)
	{
			KeyInfo.KeyType = RT_CMD_80211_KEY_AES_CMAC;
			KeyInfo.KeyId = KeyIdx;
			KeyInfo.bPairwise = FALSE;
			KeyInfo.KeyLen = pParams->key_len;			
	}
#endif /* LINUX_VERSION_CODE */
#endif /* DOT11W_PMF_SUPPORT */	
	else
		return -ENOTSUPP;

	KeyInfo.pNetDev = pNdev;

#ifdef CONFIG_AP_SUPPORT
	if ((pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_AP) ||
	   (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_GO))
	{
		if(pMacAddr) 
		{
			CFG80211DBG(RT_DEBUG_TRACE, ("80211> KeyAdd STA(%02X:%02X:%02X:%02X:%02X:%02X) ==>\n", 
						PRINT_MAC(pMacAddr)));
			NdisCopyMemory(KeyInfo.MAC, pMacAddr, MAC_ADDR_LEN);
		}
        CFG80211DBG(RT_DEBUG_OFF, ("80211> AP Key Add\n"));
        RTMP_DRIVER_80211_AP_KEY_ADD(pAd, &KeyInfo);
    }
	else
#endif /* CONFIG_AP_SUPPORT */	 
	{
#ifdef CONFIG_STA_SUPPORT
#ifdef CFG_TDLS_SUPPORT
		if (pMacAddr)
			NdisCopyMemory(KeyInfo.MAC, pMacAddr, MAC_ADDR_LEN);
#endif
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> STA Key Add\n"));
		RTMP_DRIVER_80211_STA_KEY_ADD(pAd, &KeyInfo);
#endif				
	}
	
#ifdef RT_P2P_SPECIFIC_WIRELESS_EVENT
	if(pMacAddr)
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> P2pSendWirelessEvent(%02X:%02X:%02X:%02X:%02X:%02X) ==>\n", 
						PRINT_MAC(pMacAddr)));
		RTMP_DRIVER_80211_SEND_WIRELESS_EVENT(pAd, pMacAddr);
	}
#endif /* RT_P2P_SPECIFIC_WIRELESS_EVENT */
	
	return 0;

} 


/*
========================================================================
Routine Description:
	Get information about the key with the given parameters.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	KeyIdx			-
	Pairwise		-
	pMacAddr		-
	pCookie			-
	pCallback		-

Return Value:
	0				- success
	-x				- fail

Note:
	pMacAddr will be NULL when requesting information for a group key.

	All pointers given to the pCallback function need not be valid after
	it returns.

	This function should return an error if it is not possible to
	retrieve the key, -ENOENT if it doesn't exist.
========================================================================
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
static int CFG80211_OpsKeyGet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN bool								Pairwise,
	IN const UINT8						*pMacAddr,
	IN void								*pCookie,
	IN void								(*pCallback)(void *cookie,
												 struct key_params *))
#else

static int CFG80211_OpsKeyGet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN const UINT8						*pMacAddr,
	IN void								*pCookie,
	IN void								(*pCallback)(void *cookie,
												 struct key_params *))
#endif /* LINUX_VERSION_CODE */
{

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	return -ENOTSUPP;
}


/*
========================================================================
Routine Description:
	Remove a key given the pMacAddr (NULL for a group key) and KeyIdx.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	KeyIdx			-
	pMacAddr		-

Return Value:
	0				- success
	-x				- fail

Note:
	return -ENOENT if the key doesn't exist.
========================================================================
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
static int CFG80211_OpsKeyDel(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN bool								Pairwise,
	IN const UINT8						*pMacAddr)
#else

static int CFG80211_OpsKeyDel(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN const UINT8						*pMacAddr)
#endif /* LINUX_VERSION_CODE */
{
    VOID *pAd;
    CMD_RTPRIV_IOCTL_80211_KEY KeyInfo;
	CFG80211_CB *p80211CB;
	p80211CB = NULL;

    CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	if (pMacAddr)
	{
		CFG80211DBG(RT_DEBUG_OFF, ("80211> KeyDel STA(%02X:%02X:%02X:%02X:%02X:%02X) ==>\n", 
				PRINT_MAC(pMacAddr)));
		NdisCopyMemory(KeyInfo.MAC, pMacAddr, MAC_ADDR_LEN);	
	}
	
	MAC80211_PAD_GET(pAd, pWiphy);
	RTMP_DRIVER_80211_CB_GET(pAd, &p80211CB);

	memset(&KeyInfo, 0, sizeof(KeyInfo));
	KeyInfo.KeyId = KeyIdx;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> KeyDel isPairwise %d\n", Pairwise));
	KeyInfo.bPairwise = (BOOLEAN)Pairwise;
#endif /* LINUX_VERSION_CODE 2.6.37 */

#ifdef CONFIG_AP_SUPPORT
    if ((pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_AP) ||
        (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_GO))
    {
    	CFG80211DBG(RT_DEBUG_TRACE, ("80211> AP Key Del\n"));
        RTMP_DRIVER_80211_AP_KEY_DEL(pAd, &KeyInfo);
    }
	else
#endif /* CONFIG_AP_SUPPORT */	
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> STA Key Del\n"));

        if (pMacAddr)
		{
			CFG80211DBG(RT_DEBUG_OFF, ("80211> STA Key Del -- DISCONNECT\n"));
			RTMP_DRIVER_80211_STA_LEAVE(pAd, pNdev);
		}
	}
	
	return 0;
}


/*
========================================================================
Routine Description:
	Set the default key on an interface.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	KeyIdx			-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
static int CFG80211_OpsKeyDefaultSet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx,
	IN bool								Unicast,
	IN bool								Multicast)
#else

static int CFG80211_OpsKeyDefaultSet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx)
#endif /* LINUX_VERSION_CODE */
{
	VOID *pAd;


	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> Default KeyIdx = %d\n", KeyIdx));

#ifdef CONFIG_AP_SUPPORT
    if ((pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_AP) ||
	   (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_GO))
		RTMP_DRIVER_80211_AP_KEY_DEFAULT_SET(pAd, KeyIdx);
	else
#endif /* CONFIG_AP_SUPPORT */	
		RTMP_DRIVER_80211_STA_KEY_DEFAULT_SET(pAd, KeyIdx);
	
	return 0;
} /* End of CFG80211_OpsKeyDefaultSet */


/*
========================================================================
Routine Description:
	Set the Mgmt default key on an interface.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			-
	KeyIdx			-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/

#ifdef DOT11W_PMF_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
static int CFG80211_OpsMgmtKeyDefaultSet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT8							KeyIdx)
{
	VOID *pAd;


	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> Mgmt Default KeyIdx = %d\n", KeyIdx));
	
       RTMP_DRIVER_80211_STA_MGMT_KEY_DEFAULT_SET(pAd, KeyIdx);
	
	return 0;
} /* End of CFG80211_OpsMgmtKeyDefaultSet */
#endif /* LINUX_VERSION_CODE */
#endif /* DOT11W_PMF_SUPPORT */


/*
	Connect to the ESS with the specified parameters. When connected,
	call cfg80211_connect_result() with status code %WLAN_STATUS_SUCCESS.
	If the connection fails for some reason, call cfg80211_connect_result()
	with the status from the AP.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pSme			- 

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: connect

	You must use "iw ra0 connect xxx", then "iw ra0 disconnect";
	You can not use "iw ra0 connect xxx" twice without disconnect;
	Or you will suffer "command failed: Operation already in progress (-114)".

	You must support add_key and set_default_key function;
	Or kernel will crash without any error message in linux 2.6.32.


   struct cfg80211_connect_params - Connection parameters
 
   This structure provides information needed to complete IEEE 802.11
   authentication and association.
 
   @channel: The channel to use or %NULL if not specified (auto-select based
 	on scan results)
   @bssid: The AP BSSID or %NULL if not specified (auto-select based on scan
 	results)
   @ssid: SSID
   @ssid_len: Length of ssid in octets
   @auth_type: Authentication type (algorithm)
   
   @ie: IEs for association request
   @ie_len: Length of assoc_ie in octets
   
   @privacy: indicates whether privacy-enabled APs should be used
   @crypto: crypto settings
   @key_len: length of WEP key for shared key authentication
   @key_idx: index of WEP key for shared key authentication
   @key: WEP key for shared key authentication 	
========================================================================
*/
static int CFG80211_OpsConnect(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN struct cfg80211_connect_params	*pSme)
{
#ifdef CONFIG_STA_SUPPORT
	void *pAdOrg;
	PRTMP_ADAPTER pAd;
	CMD_RTPRIV_IOCTL_80211_CONNECT *pConnInfo;
	CMD_RTPRIV_IOCTL_80211_ASSOC_IE AssocIe;
	struct ieee80211_channel *pChannel = pSme->channel;
	INT32 Pairwise = 0;
	INT32 Groupwise = 0;
	INT32 Keymgmt = 0;
	INT32 WpaVersion = 0;
	INT32 Chan = -1, Idx;
	POS_COOKIE pObj;
	UCHAR ifIndex;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __func__));

	MAC80211_PAD_GET(pAdOrg, pWiphy);
	pAd = (PRTMP_ADAPTER)pAdOrg;
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	ifIndex = pObj->ioctl_if;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] %s: iftype %d ifIndex %d\n", __func__, pNdev->ieee80211_ptr->iftype, ifIndex));
		pConnInfo = &pAd->ApCfg.ApCliTab[ifIndex].ConnInfo;
	} else
#endif
		pConnInfo = &pAd->StaCfg.ConnInfo;

	if (pChannel != NULL)
		Chan = ieee80211_frequency_to_channel(pChannel->center_freq);

	CFG80211DBG(RT_DEBUG_TRACE, ("Groupwise: %x\n", pSme->crypto.cipher_group));
	Groupwise = pSme->crypto.cipher_group;
	//for(Idx=0; Idx<pSme->crypto.n_ciphers_pairwise; Idx++)
	Pairwise |= pSme->crypto.ciphers_pairwise[0];

	CFG80211DBG(RT_DEBUG_TRACE, ("Pairwise %x\n", pSme->crypto.ciphers_pairwise[0]));
	
	for(Idx=0; Idx<pSme->crypto.n_akm_suites; Idx++)
		Keymgmt |= pSme->crypto.akm_suites[Idx];

	WpaVersion = pSme->crypto.wpa_versions;
	CFG80211DBG(RT_DEBUG_TRACE, ("Wpa_versions %x\n", WpaVersion));

	memset(pConnInfo, 0, sizeof(CMD_RTPRIV_IOCTL_80211_CONNECT));
	pConnInfo->WpaVer = 0;

	if (WpaVersion & NL80211_WPA_VERSION_1)
		pConnInfo->WpaVer = 1;
	
	if (WpaVersion & NL80211_WPA_VERSION_2)
		pConnInfo->WpaVer = 2;

	CFG80211DBG(RT_DEBUG_TRACE, ("Keymgmt %x\n", Keymgmt));
	if (Keymgmt ==  WLAN_AKM_SUITE_8021X)
		pConnInfo->FlgIs8021x = TRUE;
	else
		pConnInfo->FlgIs8021x = FALSE;
	
	CFG80211DBG(RT_DEBUG_TRACE, ("Auth_type %x\n", pSme->auth_type));
	if (((Pairwise == WLAN_CIPHER_SUITE_WEP40) || (Pairwise & WLAN_CIPHER_SUITE_WEP104)) &&
		pSme->auth_type == NL80211_AUTHTYPE_SHARED_KEY)
		pConnInfo->AuthType = Ndis802_11AuthModeAutoSwitch;
	else if (pSme->auth_type == NL80211_AUTHTYPE_SHARED_KEY)
		pConnInfo->AuthType = Ndis802_11AuthModeShared;
	else if (pSme->auth_type == NL80211_AUTHTYPE_OPEN_SYSTEM)
		pConnInfo->AuthType = Ndis802_11AuthModeOpen;
	else
		pConnInfo->AuthType = Ndis802_11AuthModeAutoSwitch;

	if (Pairwise == WLAN_CIPHER_SUITE_CCMP) {
		CFG80211DBG(RT_DEBUG_TRACE, ("WLAN_CIPHER_SUITE_CCMP...\n"));
		pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_CCMP;
	} else if (Pairwise == WLAN_CIPHER_SUITE_TKIP) {
		CFG80211DBG(RT_DEBUG_TRACE, ("WLAN_CIPHER_SUITE_TKIP...\n"));
		pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_TKIP;
	} else if ((Pairwise == WLAN_CIPHER_SUITE_WEP40) ||
			(Pairwise & WLAN_CIPHER_SUITE_WEP104)) {
		CFG80211DBG(RT_DEBUG_TRACE, ("WLAN_CIPHER_SUITE_WEP...\n"));
		pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_WEP;
	} else {
		CFG80211DBG(RT_DEBUG_TRACE, ("NONE...\n"));
		pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_NONE;
	}

	if (Groupwise == WLAN_CIPHER_SUITE_CCMP)
		pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_CCMP;
	else if (Groupwise == WLAN_CIPHER_SUITE_TKIP)
		pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_TKIP;
	else
		pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_NONE;

	CFG80211DBG(RT_DEBUG_TRACE, ("ConnInfo.KeyLen ===> %d\n", pSme->key_len));
	CFG80211DBG(RT_DEBUG_TRACE, ("ConnInfo.KeyIdx ===> %d\n", pSme->key_idx));

	pConnInfo->pKey = (UINT8 *)(pSme->key);
	pConnInfo->KeyLen = pSme->key_len;
	pConnInfo->pSsid = pSme->ssid;
	pConnInfo->SsidLen = pSme->ssid_len;
	pConnInfo->KeyIdx = pSme->key_idx;
	/* YF@20120328: Reset to default */
	pConnInfo->bWpsConnection= FALSE;
	pConnInfo->pNetDev = pNdev;

	//hex_dump("AssocInfo:", pSme->ie, pSme->ie_len);

	/* YF@20120328: Use SIOCSIWGENIE to make out the WPA/WPS IEs in AssocReq. */
	memset(&AssocIe, 0, sizeof(AssocIe));
	AssocIe.pNetDev = pNdev;
	AssocIe.ie = pSme->ie;
	AssocIe.ie_len = pSme->ie_len;
	RTMP_DRIVER_80211_STA_ASSSOC_IE_SET(pAd, &AssocIe, pNdev->ieee80211_ptr->iftype);

#ifdef DOT11W_PMF_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	CFG80211DBG(RT_DEBUG_OFF, ("80211> PMF Connect %d\n", pSme->mfp));
	if (pSme->mfp == NL80211_MFP_REQUIRED)
		pConnInfo->mfp = TRUE;
	else
		pConnInfo->mfp = FALSE;
#endif /* LINUX_VERSION_CODE */
#endif /* DOT11W_PMF_SUPPORT */

	if ((pSme->ie_len > 6) /* EID(1) + LEN(1) + OUI(4) */ &&
		(pSme->ie[0] == WLAN_EID_VENDOR_SPECIFIC && 
		pSme->ie[1] >= 4 &&
		pSme->ie[2] == 0x00 && pSme->ie[3] == 0x50 && pSme->ie[4] == 0xf2 &&
		pSme->ie[5] == 0x04))
		pConnInfo->bWpsConnection= TRUE;
	/* Check if WPS is triggerred */
	if (pSme->ie && pSme->ie_len) {
		if (RTMPFindWPSIE(pSme->ie, (UINT32)pSme->ie_len) != NULL)
			pConnInfo->bWpsConnection= TRUE;
	}
	/* %NULL if not specified (auto-select based on scan)*/
	if (pSme->bssid != NULL) {
		CFG80211DBG(RT_DEBUG_OFF, ("80211> Connect bssid %02x:%02x:%02x:%02x:%02x:%02x\n",  
				PRINT_MAC(pSme->bssid)));
		pConnInfo->pBssid = pSme->bssid;
	}

	RTMP_DRIVER_80211_CONNECT(pAd, pConnInfo, pNdev->ieee80211_ptr->iftype);
#endif /*CONFIG_STA_SUPPORT*/	
	return 0;
} /* End of CFG80211_OpsConnect */

#ifdef WPA3_SUPPORT
/*
========================================================================
Routine Description:
	This routine is responsible for requesting auth to the ESS with the specified parameters.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pReq			-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsAuth(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN struct cfg80211_auth_request		*pReq)
{
	void *pAdOrg;
	PRTMP_ADAPTER pAd;
	CMD_RTPRIV_IOCTL_80211_CONNECT *pConnInfo;
	struct ieee80211_channel *pChannel;
	UINT8 fgNewAuthParam = FALSE;
	INT rStatus;
	MLME_AUTH_REQ_STRUCT AuthReq;
	POS_COOKIE pObj;
	UCHAR ifIndex;

	CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] 80211> %s ==>\n", __func__));

	/* init */
	MAC80211_PAD_GET(pAdOrg, pWiphy);
	pAd = (PRTMP_ADAPTER)pAdOrg;
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	ifIndex = pObj->ioctl_if;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] CFG80211_OpsAuth: iftype %d ifIndex %d\n", pNdev->ieee80211_ptr->iftype, ifIndex));
		pConnInfo = &pAd->ApCfg.ApCliTab[ifIndex].ConnInfo;
	} else
#endif
		pConnInfo = &pAd->StaCfg.ConnInfo;
	memset(pConnInfo, 0, sizeof(CMD_RTPRIV_IOCTL_80211_CONNECT));

#if KERNEL_VERSION(4, 10, 0) > CFG80211_VERSION_CODE
	if (pReq->sae_data_len != 0)
		CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] CFG80211_OpsAuth %p %zu\n", pReq->sae_data, pReq->sae_data_len));
#else
	if (pReq->auth_data_len != 0)
		CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] CFG80211_OpsAuth %p %zu\n", pReq->auth_data, pReq->auth_data_len));
#endif
	CFG80211DBG(RT_DEBUG_TRACE, ("auth to  BSS [%02x:%02x:%02x:%02x:%02x:%02x]\n", PRINT_MAC(pReq->bss->bssid)));
	CFG80211DBG(RT_DEBUG_TRACE, ("auth_type:%d\n", pReq->auth_type));


	/*<1> Set  Auth data: Non-IE data to use with SAE or %NULL. This starts with Authentication transaction sequence number field. */
	pConnInfo->ucAuthDataLen = 0;
#if KERNEL_VERSION(4, 10, 0) > CFG80211_VERSION_CODE
	if (pReq->sae_data_len != 0) {
		if (pReq->sae_data_len > AUTH_DATA_MAX_LEN) {
			CFG80211DBG(RT_DEBUG_ERROR, ("request auth with unexpected length:%d\n", pReq->sae_data_len));
			return -EFAULT;
		}

		memcpy(pConnInfo->aucAuthData, pReq->sae_data, pReq->sae_data_len);
		pConnInfo->ucAuthDataLen = pReq->sae_data_len;

		CFG80211DBG(RT_DEBUG_TRACE,
			("Dump auth data in connectSettings, auth len:%d, auth SeqNum %d\n",
			pConnInfo->ucAuthDataLen, pConnInfo->aucAuthData[0]));
		/* hex_dump("Authdata:", pConnInfo->aucAuthData, pConnInfo->ucAuthDataLen); */
	}
#else
	if (pReq->auth_data_len != 0) {
		if (pReq->auth_data_len > AUTH_DATA_MAX_LEN) {
			CFG80211DBG(RT_DEBUG_ERROR,
				("request auth with unexpected length:%d\n",
				pReq->auth_data_len));
			return -EFAULT;
		}

		memcpy(pConnInfo->aucAuthData, pReq->auth_data, pReq->auth_data_len);
		pConnInfo->ucAuthDataLen = pReq->auth_data_len;

		CFG80211DBG(RT_DEBUG_TRACE,
			("Dump auth data in connectSettings, auth len:%d, auth SeqNum %d\n",
			pConnInfo->ucAuthDataLen, pConnInfo->aucAuthData[0]));
		/* hex_dump("Authdata:", pConnInfo->aucAuthData, pConnInfo->ucAuthDataLen); */
	}
#endif

	/*<2> Set  Auth Extra IEs. */
	pConnInfo->ucExtraIeLen = 0;
	if (pReq->ie_len != 0) {
		if (pReq->ie_len > AUTH_IE_MAX_LEN) {
			CFG80211DBG(RT_DEBUG_ERROR, ("request auth with unexpected extra ie length:%d\n", pReq->ie_len));
			return -EFAULT;
		}
		memcpy(pConnInfo->aucExtraIe, pReq->ie, pReq->ie_len);
		pConnInfo->ucExtraIeLen = pReq->ie_len;
		CFG80211DBG(RT_DEBUG_TRACE,
			("Dump ExtraIEs in connectSettings, ExtraIElen:%d\n",
			pConnInfo->ucExtraIeLen));
		hex_dump("Extra IE:", pConnInfo->aucExtraIe, pConnInfo->ucExtraIeLen);
	}


	/* <3> Get Channel Num */
	pChannel = pReq->bss->channel;
	if (pChannel != NULL)
		pConnInfo->ucChannelNum = ieee80211_frequency_to_channel(pChannel->center_freq);
	CFG80211DBG(RT_DEBUG_TRACE, ("ConnInfo.ucChannelNum:%d\n", pConnInfo->ucChannelNum));

	/* <4> Auth type */
	pConnInfo->AuthType = 0;
	switch (pReq->auth_type) {
	case NL80211_AUTHTYPE_OPEN_SYSTEM:
		if (pConnInfo->AuthType != Ndis802_11AuthModeOpen)
			fgNewAuthParam = TRUE;
		pConnInfo->AuthType = Ndis802_11AuthModeOpen;
		break;
	case NL80211_AUTHTYPE_SHARED_KEY:
		if (pConnInfo->AuthType != Ndis802_11AuthModeShared)
			fgNewAuthParam = TRUE;
		pConnInfo->AuthType = Ndis802_11AuthModeShared;
		break;
	case NL80211_AUTHTYPE_SAE:
		if (pConnInfo->AuthType != Ndis802_11AuthModeWPA3SAE)
			fgNewAuthParam = TRUE;
		pConnInfo->AuthType = Ndis802_11AuthModeWPA3SAE;
		break;
	default:
		CFG80211DBG(RT_DEBUG_WARN,
			("Auth type: %d not support, use AutoSwitch system\n",
			pReq->auth_type));
		pConnInfo->AuthType = Ndis802_11AuthModeAutoSwitch;
		break;
	}
	CFG80211DBG(RT_DEBUG_TRACE, ("Auth Algorithm : %d\n", pConnInfo->AuthType));

	/* <5> Wep key */
	if ((pReq->key_len != 0) && pReq->key) {
		UCHAR KeyBuf[50];

		if (pConnInfo->AuthType != Ndis802_11AuthModeShared)
			CFG80211DBG(RT_DEBUG_TRACE, ("Auth Algorithm : %d with wep key\n", pConnInfo->AuthType));

		pConnInfo->pKey = (UINT8 *)(pReq->key);
		pConnInfo->KeyLen = pReq->key_len;
		pConnInfo->KeyIdx = pReq->key_idx;
		CFG80211DBG(RT_DEBUG_TRACE, ("ConnInfo.KeyLen ===> %d\n", pReq->key_len));
		CFG80211DBG(RT_DEBUG_TRACE, ("ConnInfo.KeyIdx ===> %d\n", pReq->key_idx));

		/* reset key */
#ifdef RT_CFG80211_DEBUG
		hex_dump("KeyBuf=", (UINT8 *)pConnInfo->pKey, pConnInfo->KeyLen);
#endif /* RT_CFG80211_DEBUG */
		if (pNdev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) {
			pAd->StaCfg.wdev.DefaultKeyId = pConnInfo->KeyIdx; /* base 0 */
			if (pConnInfo->KeyLen >= sizeof(KeyBuf))
				return FALSE;
			/* End of if */
			memcpy(KeyBuf, pConnInfo->pKey, pConnInfo->KeyLen);
			KeyBuf[pConnInfo->KeyLen] = 0x00;

			CFG80211DBG(RT_DEBUG_ERROR,
						("80211> pAd->StaCfg.wdev.DefaultKeyId = %d\n",
						pAd->StaCfg.wdev.DefaultKeyId));

			Set_Wep_Key_Proc(pAd, (RTMP_STRING *)KeyBuf, (INT)pConnInfo->KeyLen, (INT)pConnInfo->KeyIdx);

		} else if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
			pAd->ApCfg.ApCliTab[ifIndex].wdev.DefaultKeyId = pConnInfo->KeyIdx;
			if (pConnInfo->KeyLen >= sizeof(KeyBuf))
				return FALSE;

			memcpy(KeyBuf, pConnInfo->pKey, pConnInfo->KeyLen);
			KeyBuf[pConnInfo->KeyLen] = 0x00;

			CFG80211DBG(RT_DEBUG_ERROR, ("80211> DefaultKeyId = %d\n", pAd->ApCfg.ApCliTab[ifIndex].wdev.DefaultKeyId));
			RT_CfgSetWepKey(pAd, (RTMP_STRING *)KeyBuf, &pAd->ApCfg.ApCliTab[ifIndex].SharedKey[pConnInfo->KeyIdx], (INT)pConnInfo->KeyIdx);
		}
	} /* End of if */

	/* <6> BSSID */
	if (pConnInfo->pBssid)
		CFG80211DBG(RT_DEBUG_TRACE, ("auth to BSS [%02x:%02x:%02x:%02x:%02x:%02x]",
			PRINT_MAC(pConnInfo->pBssid)));
	CFG80211DBG(RT_DEBUG_TRACE, ("UpperReq [%02x:%02x:%02x:%02x:%02x:%02x]\n",
		PRINT_MAC(pReq->bss->bssid)));
	if (pConnInfo->pBssid != (PUINT8)pReq->bss->bssid) {
		fgNewAuthParam = TRUE;
		pConnInfo->pBssid = (PUINT8)pReq->bss->bssid;
	}

	/* YF@20120328: Reset to default */
	pConnInfo->bWpsConnection = FALSE;
	pConnInfo->pNetDev = pNdev;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		if (!pAd->ApCfg.ApCliTab[ifIndex].fgIsConnInitialized) {
			if (fgNewAuthParam)
				CFG80211DBG(RT_DEBUG_WARN, ("Apcli auth param update\n"));

			rStatus = RTMP_DRIVER_80211_CONNECT(pAd, pConnInfo, pNdev->ieee80211_ptr->iftype);

			if (rStatus != NDIS_STATUS_SUCCESS) {
				CFG80211DBG(RT_DEBUG_WARN, ("%s failed :%x\n", __func__, rStatus));
				return -EINVAL;
			}
		} else {
			/* skip join initial flow when it has been completed with the same auth parameters */

			/* reset extra ies to pAd->StaCfg */
			if (pConnInfo->ucExtraIeLen > 0)
				RTMP_DRIVER_80211_P2PCLI_AUTH_IE_SET(pAd, (void *)pConnInfo->aucExtraIe, pConnInfo->ucExtraIeLen);
			else if (pConnInfo->ucExtraIeLen == 0)
				RTMP_DRIVER_80211_P2PCLI_AUTH_IE_SET(pAd, NULL, 0);

			{
				/* either Ndis802_11AuthModeShared or Ndis802_11AuthModeAutoSwitch, try shared key first */
				if ((pAd->ApCfg.ApCliTab[ifIndex].wdev.AuthMode == Ndis802_11AuthModeShared) || (pAd->ApCfg.ApCliTab[ifIndex].wdev.AuthMode == Ndis802_11AuthModeAutoSwitch)) {
					AuthParmFill(pAd, &AuthReq, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, AUTH_MODE_KEY);
				} else if (pAd->ApCfg.ApCliTab[ifIndex].wdev.AuthMode == Ndis802_11AuthModeWPA3SAE) {
					AuthParmFill(pAd, &AuthReq, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, AUTH_MODE_SAE);
				} else {
					AuthParmFill(pAd, &AuthReq, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, AUTH_MODE_OPEN);
				}

				pAd->ApCfg.ApCliTab[ifIndex].AuthReqCnt = 0;
				MlmeEnqueue(pAd, APCLI_AUTH_STATE_MACHINE, APCLI_MT2_MLME_AUTH_REQ, sizeof(MLME_AUTH_REQ_STRUCT), &AuthReq, ifIndex);
			}
		}
	} else
#endif
	{
		if (!pAd->StaCfg.fgIsConnInitialized) {
			if (fgNewAuthParam)
				CFG80211DBG(RT_DEBUG_WARN, ("auth param update\n"));

			rStatus = RTMP_DRIVER_80211_CONNECT(pAd, pConnInfo, pNdev->ieee80211_ptr->iftype);

			if (rStatus != NDIS_STATUS_SUCCESS) {
				CFG80211DBG(RT_DEBUG_WARN, ("%s failed :%x\n", __func__, rStatus));
				return -EINVAL;
			}
		} else {
			/* skip join initial flow when it has been completed with the same auth parameters */

			/* reset extra ies to pAd->StaCfg */
			if (pConnInfo->ucExtraIeLen > 0)
				RTMP_DRIVER_80211_GEN_AUTH_IE_SET(pAd, (void *)pConnInfo->aucExtraIe, pConnInfo->ucExtraIeLen);
			else if (pConnInfo->ucExtraIeLen == 0)
				RTMP_DRIVER_80211_GEN_AUTH_IE_SET(pAd, NULL, 0);

			{
				/* either Ndis802_11AuthModeShared or Ndis802_11AuthModeAutoSwitch, try shared key first */
				if ((pAd->StaCfg.wdev.AuthMode == Ndis802_11AuthModeShared) || (pAd->StaCfg.wdev.AuthMode == Ndis802_11AuthModeAutoSwitch)) {
					AuthParmFill(pAd, &AuthReq, pAd->MlmeAux.Bssid, AUTH_MODE_KEY);
				} else if (pAd->StaCfg.wdev.AuthMode == Ndis802_11AuthModeWPA3SAE) {
					AuthParmFill(pAd, &AuthReq, pAd->MlmeAux.Bssid, AUTH_MODE_SAE);
				} else {
					AuthParmFill(pAd, &AuthReq, pAd->MlmeAux.Bssid, AUTH_MODE_OPEN);
				}

				MlmeEnqueue(pAd, AUTH_STATE_MACHINE, MT2_MLME_AUTH_REQ, sizeof(MLME_AUTH_REQ_STRUCT), &AuthReq, 0);
			}
		}
	}
	return 0;
} /* End of CFG80211_OpsAuth */

/*
========================================================================
Routine Description:
	This routine is responsible for requesting assoc to the ESS with the specified parameters.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pReq		-

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/

static int CFG80211_OpsAssoc(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN struct cfg80211_assoc_request	*pReq)
{
	void *pAdOrg;
	PRTMP_ADAPTER pAd;

	INT rStatus;
	UINT32 i, j, u4AkmSuite = RSN_AKM_SUITE_NONE;
	P_RSNA_CONFIG_AUTHENTICATION_SUITES_ENTRY prEntry;
	const UCHAR *prDesiredIE = NULL;
	const UCHAR *pucIEStart = NULL;
	P_RSN_INFO_T pRsnInfo;
	CMD_RTPRIV_IOCTL_80211_CONNECT *pConnInfo;
	MLME_ASSOC_REQ_STRUCT AssocReq;
	POS_COOKIE pObj;
	UCHAR ifIndex;
	CMD_RTPRIV_IOCTL_80211_ASSOC_IE AssocIe;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __func__));

	/* init */
	MAC80211_PAD_GET(pAdOrg, pWiphy);
	pAd = (PRTMP_ADAPTER)pAdOrg;
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	ifIndex = pObj->ioctl_if;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] %s: iftype %d ifIndex %d\n", __func__, pNdev->ieee80211_ptr->iftype, ifIndex));
		pConnInfo = &pAd->ApCfg.ApCliTab[ifIndex].ConnInfo;
		pRsnInfo = &pAd->ApCfg.ApCliTab[ifIndex].RsnInfo;
		pObj->ioctl_if_type = INT_APCLI;
	} else
#endif
	{
		pRsnInfo = &pAd->StaCfg.RsnInfo;
		pConnInfo = &pAd->StaCfg.ConnInfo;
	}
	/* [todo]temp use for indicate rx assoc resp, may need to be modified */

	/* The BSS from cfg80211_ops.assoc must give back to
	 * cfg80211_send_rx_assoc() or to cfg80211_assoc_timeout().
	 * To ensure proper refcounting, new association requests
	 * while already associating must be rejected.
	 */
	if (pConnInfo->bss)
		return -ENOENT;
	pConnInfo->bss = pReq->bss;

	/* 1. check BSSID */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		if (memcmp(pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, pReq->bss->bssid, MAC_ADDR_LEN)) {
			/* wrong MAC address */
			DBGPRINT(RT_DEBUG_ERROR, ("incorrect BSSID: [%02x:%02x:%02x:%02x:%02x:%02x] currently connected BSSID[%02x:%02x:%02x:%02x:%02x:%02x]\n",
				PRINT_MAC(pReq->bss->bssid), PRINT_MAC(pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid)));
			return -ENOENT;
		}
	} else
#endif
	if (memcmp(pAd->MlmeAux.Bssid, pReq->bss->bssid, MAC_ADDR_LEN)) {
		/* wrong MAC address */
		DBGPRINT(RT_DEBUG_ERROR, ("incorrect BSSID: [%02x:%02x:%02x:%02x:%02x:%02x] currently connected BSSID[%02x:%02x:%02x:%02x:%02x:%02x]\n",
			PRINT_MAC(pReq->bss->bssid), PRINT_MAC(pAd->MlmeAux.Bssid)));
		return -ENOENT;
	}

	/* <1> Reset WPA info */
	pConnInfo->WpaVer = 0;
	pConnInfo->FlgIs8021x = FALSE;
	pConnInfo->PairwiseEncrypType = RT_CMD_80211_CONN_ENCRYPT_NONE;
	pConnInfo->GroupwiseEncrypType = RT_CMD_80211_CONN_ENCRYPT_NONE;
#ifdef CFG_SUPPORT_802_11W
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		pAd->ApCfg.ApCliTab[ifIndex].u4CipherGroupMgmt = RT_CMD_80211_CONN_ENCRYPT_NONE;
		pAd->ApCfg.ApCliTab[ifIndex].u4Mfp = RSN_AUTH_MFP_DISABLED;
		pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap = RSN_AUTH_MFP_DISABLED;
	} else
#endif
	{
		pAd->StaCfg.u4CipherGroupMgmt = RT_CMD_80211_CONN_ENCRYPT_NONE;
		pAd->StaCfg.u4Mfp = RSN_AUTH_MFP_DISABLED;
		pAd->StaCfg.ucRSNMfpCap = RSN_AUTH_MFP_DISABLED;
	}
#endif

	/* 2.Fill WPA version */
	if (pReq->crypto.wpa_versions & NL80211_WPA_VERSION_1)
		pConnInfo->WpaVer = 1;
	else if (pReq->crypto.wpa_versions & NL80211_WPA_VERSION_2)
		pConnInfo->WpaVer = 2;
	else
		pConnInfo->WpaVer = 0;
	DBGPRINT(RT_DEBUG_TRACE, ("wpa ver=%d\n", pConnInfo->WpaVer));

	/* 3. Fill AKM suites  & decide AuthMode*/
	DBGPRINT(RT_DEBUG_TRACE, ("request numbers of Akm Suite:%d\n", pReq->crypto.n_akm_suites));
	for (i = 0; i < pReq->crypto.n_akm_suites; i++) {
		DBGPRINT(RT_DEBUG_TRACE, ("request Akm Suite[%d]:%x\n", i, pReq->crypto.akm_suites[i]));

		if (pConnInfo->WpaVer == 1) {
			switch (pReq->crypto.akm_suites[i]) {
			case WLAN_AKM_SUITE_8021X:
				if (i == 0) {
					pConnInfo->FlgIs8021x = TRUE;
					pConnInfo->AuthType = Ndis802_11AuthModeWPA;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = WPA_AKM_SUITE_802_1X;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA");
						pAd->StaCfg.AKMSuite = WPA_AKM_SUITE_802_1X;
					}
				}
				u4AkmSuite = WPA_AKM_SUITE_802_1X;
				break;
			case WLAN_AKM_SUITE_PSK:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPAPSK;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPAPSK");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = WPA_AKM_SUITE_PSK;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPAPSK");
						pAd->StaCfg.AKMSuite = WPA_AKM_SUITE_PSK;
					}
				}
				u4AkmSuite = WPA_AKM_SUITE_PSK;
				break;
			default:
				DBGPRINT(RT_DEBUG_WARN, ("invalid Akm Suite (%08x)\n", pReq->crypto.akm_suites[i]));
				return -EINVAL;
			}
		} else if (pConnInfo->WpaVer == 2) {
			switch (pReq->crypto.akm_suites[i]) {
			case WLAN_AKM_SUITE_8021X:
				if (i == 0) {
					pConnInfo->FlgIs8021x = TRUE;
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_802_1X;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_802_1X;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_802_1X;
				break;
			case WLAN_AKM_SUITE_PSK:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2PSK;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_PSK;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_PSK;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_PSK;
				break;
#ifdef CFG_SUPPORT_802_11W
			/* Notice:: Need kernel patch!! */
			case WLAN_AKM_SUITE_8021X_SHA256:
				if (i == 0) {
					pConnInfo->FlgIs8021x = TRUE;
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_802_1X_SHA256;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_802_1X_SHA256;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_802_1X_SHA256;
				break;
			case WLAN_AKM_SUITE_PSK_SHA256:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2PSK;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_PSK_SHA256;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_PSK_SHA256;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_PSK_SHA256;
				break;
#endif
#ifdef CFG_SUPPORT_SUITB
			case WLAN_AKM_SUITE_8021X_SUITE_B:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2PSK;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
				break;
#endif
#ifdef CFG_SUPPORT_SUITB_192
			case WLAN_AKM_SUITE_8021X_SUITE_B_192:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2PSK;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_8021X_SUITE_B_192;
				break;
#endif
#ifdef CFG_SUPPORT_SAE
			/* Need to add in WPA also? */
			case WLAN_AKM_SUITE_SAE:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPA3SAE;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "SAE");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_SAE;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "SAE");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_SAE;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_SAE;
				break;
#endif
#ifdef CFG_SUPPORT_OWE
			case WLAN_AKM_SUITE_OWE:
				if (i == 0) {
					pConnInfo->AuthType = Ndis802_11AuthModeWPA2PSK;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
					if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
						Set_ApCli_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->ApCfg.ApCliTab[ifIndex].AKMSuite = RSN_AKM_SUITE_OWE;
					} else
#endif
					{
						Set_AuthMode_Proc(pAd, "WPA2PSK");
						pAd->StaCfg.AKMSuite = RSN_AKM_SUITE_OWE;
					}
				}
				u4AkmSuite = RSN_AKM_SUITE_OWE;
				break;
#endif
			default:
				DBGPRINT(RT_DEBUG_WARN, ("invalid Akm Suite (%08x)\n", pReq->crypto.akm_suites[0]));
				return -EINVAL;
			}
		}

		for (j = 0; j < MAX_NUM_SUPPORTED_AKM_SUITES; j++) {
				prEntry = &pAd->RSNAKMSuitesTable[j];

			if (prEntry->dot11RSNAKMSuite == u4AkmSuite) {
				prEntry->dot11RSNAKMSuiteEnabled = TRUE;
				DBGPRINT(RT_DEBUG_TRACE, ("match AKM Suite = 0x%x", u4AkmSuite));
			} else {
				prEntry->dot11RSNAKMSuiteEnabled = FALSE;
			}
		}
	}
	if (pConnInfo->WpaVer == 0) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
		if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT)
			pConnInfo->AuthType = pAd->ApCfg.ApCliTab[ifIndex].wdev.AuthMode;
		else
#endif
			pConnInfo->AuthType = pAd->StaCfg.wdev.AuthMode;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT)
		DBGPRINT(RT_DEBUG_TRACE, ("set auth mode:%d, akm suite:0x%x\n", pAd->ApCfg.ApCliTab[ifIndex].wdev.AuthMode, pAd->ApCfg.ApCliTab[ifIndex].AKMSuite));
	else
#endif
		DBGPRINT(RT_DEBUG_TRACE, ("set auth mode:%d, akm suite:0x%x\n", pAd->StaCfg.wdev.AuthMode, pAd->StaCfg.AKMSuite));

	/* 4.Fill pairwise cipher suite */
	DBGPRINT(RT_DEBUG_TRACE, ("number of ciphers pairwise is %d\n", pReq->crypto.n_ciphers_pairwise));
	for (i = 0; i < pReq->crypto.n_ciphers_pairwise; i++) {
		DBGPRINT(RT_DEBUG_TRACE, ("request PairCipher[%d]:%x\n", i, pReq->crypto.ciphers_pairwise[i]));

		switch (pReq->crypto.ciphers_pairwise[i]) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
			pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_WEP;
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_TKIP;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_CCMP;
			break;
		case WLAN_CIPHER_SUITE_AES_CMAC:
			pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_CCMP;
			break;
		case WLAN_CIPHER_SUITE_BIP_GMAC_256:
			pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_GCMP256;
			break;
		case WLAN_CIPHER_SUITE_GCMP_256:
			pConnInfo->PairwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_GCMP256;
			break;
		default:
				DBGPRINT(RT_DEBUG_WARN, ("invalid cipher pairwise (%d)\n", pReq->crypto.ciphers_pairwise[i]));
				return -EINVAL;
		}
	}

	/* 5. Fill group cipher suite */
	if (pReq->crypto.cipher_group) {
		DBGPRINT(RT_DEBUG_TRACE, ("cipher group (%x)\n", pReq->crypto.cipher_group));
		switch (pReq->crypto.cipher_group) {
		case WLAN_CIPHER_SUITE_WEP40:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_WEP;
			break;
		case WLAN_CIPHER_SUITE_WEP104:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_WEP;
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_TKIP;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_CCMP;
			break;
		case WLAN_CIPHER_SUITE_AES_CMAC:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_CCMP;
			break;
		case WLAN_CIPHER_SUITE_BIP_GMAC_256:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_GCMP256;
			break;
		case WLAN_CIPHER_SUITE_GCMP_256:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_GCMP256;
			break;
		case WLAN_CIPHER_SUITE_NO_GROUP_ADDR:
			pConnInfo->GroupwiseEncrypType |= RT_CMD_80211_CONN_ENCRYPT_NONE;
			break;
		default:
			DBGPRINT(RT_DEBUG_WARN, ("invalid cipher group (%d)\n", pReq->crypto.cipher_group));
			return -EINVAL;
		}
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		/*  set encryption mode */
		if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_GCMP256) {
			DBGPRINT(RT_DEBUG_TRACE, ("GCMP256\n"));
			Set_ApCli_EncrypType_Proc(pAd, "GCMP256");
		} else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP) {
			DBGPRINT(RT_DEBUG_TRACE, ("AES\n"));
			Set_ApCli_EncrypType_Proc(pAd, "AES");
		} else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP) {
			DBGPRINT(RT_DEBUG_TRACE, ("TKIP\n"));
			Set_ApCli_EncrypType_Proc(pAd, "TKIP");
		} else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_WEP) {
			DBGPRINT(RT_DEBUG_TRACE, ("WEP\n"));
			Set_ApCli_EncrypType_Proc(pAd, "WEP");
		} else {
			DBGPRINT(RT_DEBUG_TRACE, ("NONE\n"));
			Set_ApCli_EncrypType_Proc(pAd, "NONE");
		}

		/* Groupwise Key Information Setting */
		if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_GCMP256) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK GCMP256\n"));
			pAd->ApCfg.ApCliTab[ifIndex].GroupCipher = Ndis802_11Encryption5Enabled;
		} else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK AES\n"));
			pAd->ApCfg.ApCliTab[ifIndex].GroupCipher = Ndis802_11Encryption3Enabled;
		} else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK TKIP\n"));
			pAd->ApCfg.ApCliTab[ifIndex].GroupCipher = Ndis802_11Encryption2Enabled;
		} else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_WEP) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK WEP\n"));
			pAd->ApCfg.ApCliTab[ifIndex].GroupCipher = Ndis802_11Encryption1Enabled;
		}

		CFG80211DBG(RT_DEBUG_TRACE, ("80211> PairCipher = %d\n", pAd->ApCfg.ApCliTab[ifIndex].PairCipher));
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> GroupCipher = %d\n", pAd->ApCfg.ApCliTab[ifIndex].GroupCipher));
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> EncrypType = %d\n", pAd->ApCfg.ApCliTab[ifIndex].wdev.WepStatus));
	} else
#endif
	{
		/*  set encryption mode */
		if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_GCMP256) {
			DBGPRINT(RT_DEBUG_TRACE, ("GCMP256\n"));
			Set_EncrypType_Proc(pAd, "GCMP256");
		} else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP) {
			DBGPRINT(RT_DEBUG_TRACE, ("AES\n"));
			Set_EncrypType_Proc(pAd, "AES");
		} else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP) {
			DBGPRINT(RT_DEBUG_TRACE, ("TKIP\n"));
			Set_EncrypType_Proc(pAd, "TKIP");
		} else if (pConnInfo->PairwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_WEP) {
			DBGPRINT(RT_DEBUG_TRACE, ("WEP\n"));
			Set_EncrypType_Proc(pAd, "WEP");
		} else {
			DBGPRINT(RT_DEBUG_TRACE, ("NONE\n"));
			Set_EncrypType_Proc(pAd, "NONE");
		}

		/* Groupwise Key Information Setting */
		if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_GCMP256) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK GCMP256\n"));
			pAd->StaCfg.GroupCipher = Ndis802_11Encryption5Enabled;
		} else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_CCMP) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK AES\n"));
			pAd->StaCfg.GroupCipher = Ndis802_11Encryption3Enabled;
		} else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_TKIP) {
			DBGPRINT(RT_DEBUG_TRACE, ("GTK TKIP\n"));
			pAd->StaCfg.GroupCipher = Ndis802_11Encryption2Enabled;
		} else if (pConnInfo->GroupwiseEncrypType & RT_CMD_80211_CONN_ENCRYPT_WEP)
			pAd->StaCfg.GroupCipher = Ndis802_11Encryption1Enabled;

		CFG80211DBG(RT_DEBUG_TRACE, ("80211> PairCipher = %d\n", pAd->StaCfg.PairCipher));
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> GroupCipher = %d\n", pAd->StaCfg.GroupCipher));
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> EncrypType = %d\n", pAd->StaCfg.wdev.WepStatus));
	}

	/* 6. parse desired ie from upper layer */
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> pReq->ie_len = %d\n", pReq->ie_len));
	if (pReq->ie && pReq->ie_len > 0) {
		pucIEStart = (const UCHAR *)pReq->ie;
		prDesiredIE = RTMPFindIE(0x30, pucIEStart, pReq->ie_len);
		if (prDesiredIE != NULL) {
			if (RTMPParseRSNIE(pAd, (P_RSN_INFO_ELEM_T)prDesiredIE, pRsnInfo)) {
#ifdef CFG_SUPPORT_802_11W
				/* Fill RSNE MFP Cap */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
				if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
					pAd->ApCfg.ApCliTab[ifIndex].u4CipherGroupMgmt = pRsnInfo->u4GroupMgmtKeyCipherSuite;
					if (pRsnInfo->u2RsnCap & ELEM_WPA_CAP_MFPC) {
						pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap = RSN_AUTH_MFP_OPTIONAL;
						if (pRsnInfo->u2RsnCap & ELEM_WPA_CAP_MFPR)
							pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap = RSN_AUTH_MFP_REQUIRED;
					} else
						pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap = RSN_AUTH_MFP_DISABLED;
				} else
#endif
				{
					pAd->StaCfg.u4CipherGroupMgmt = pRsnInfo->u4GroupMgmtKeyCipherSuite;
					if (pRsnInfo->u2RsnCap & ELEM_WPA_CAP_MFPC) {
						pAd->StaCfg.ucRSNMfpCap = RSN_AUTH_MFP_OPTIONAL;
						if (pRsnInfo->u2RsnCap & ELEM_WPA_CAP_MFPR)
							pAd->StaCfg.ucRSNMfpCap = RSN_AUTH_MFP_REQUIRED;
					} else
						pAd->StaCfg.ucRSNMfpCap = RSN_AUTH_MFP_DISABLED;
				}
#endif
			}
		}

#ifdef CFG_SUPPORT_OWE
		/* Gen OWE IE */
		prDesiredIE = RTMPFindIE(0xff, pucIEStart, pReq->ie_len);
		if (prDesiredIE != NULL) {
			UINT16 ucLength = (*(prDesiredIE+1)+2);

			memcpy(&pConnInfo->rOweInfo, prDesiredIE, ucLength);

			CFG80211DBG(RT_DEBUG_TRACE, ("DUMP OWE INFO, EID %x length %x\n", *prDesiredIE, ucLength));
			hex_dump("DUMP OWE INFO:", (UCHAR *)&pConnInfo->rOweInfo, ucLength);
		} else {
			memset(&pConnInfo->rOweInfo, 0, sizeof(struct OWE_INFO_T));
		}
#endif
	}
	memset(&AssocIe, 0, sizeof(AssocIe));
	AssocIe.pNetDev = pNdev;
	AssocIe.ie = pReq->ie;
	AssocIe.ie_len = pReq->ie_len;
	RTMP_DRIVER_80211_STA_ASSSOC_IE_SET(pAd, &AssocIe, pNdev->ieee80211_ptr->iftype);

	/* 7.MFP Seeting */
#ifdef CFG_SUPPORT_802_11W
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		DBGPRINT(RT_DEBUG_WARN, ("pReq->use_mfp == %d, pAd->ApCfg.ApCliTab[%d].ucRSNMfpCap == %d\n", pReq->use_mfp, ifIndex, pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap));
		pAd->ApCfg.ApCliTab[ifIndex].u4Mfp = IW_AUTH_MFP_DISABLED;
		if (pReq->use_mfp)
			pAd->ApCfg.ApCliTab[ifIndex].u4Mfp = IW_AUTH_MFP_REQUIRED;
		else {
			/* Change Mfp parameter from DISABLED to OPTIONAL
			* if upper layer set MFPC = 1 in RSNE
			* since upper layer can't bring MFP OPTIONAL information
			* to driver by sme->mfp
			*/
			if (pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap == RSN_AUTH_MFP_OPTIONAL)
				pAd->ApCfg.ApCliTab[ifIndex].u4Mfp = IW_AUTH_MFP_OPTIONAL;
			else if (pAd->ApCfg.ApCliTab[ifIndex].ucRSNMfpCap == RSN_AUTH_MFP_REQUIRED)
				DBGPRINT(RT_DEBUG_WARN, ("Apcli - mfp parameter(DISABLED) conflict with mfp cap(REQUIRED)\n"));
		}
	} else
#endif
	{
		DBGPRINT(RT_DEBUG_WARN, ("pReq->use_mfp == %d, pAd->StaCfg.ucRSNMfpCap == %d\n", pReq->use_mfp, pAd->StaCfg.ucRSNMfpCap));
		pAd->StaCfg.u4Mfp = IW_AUTH_MFP_DISABLED;
		if (pReq->use_mfp)
			pAd->StaCfg.u4Mfp = IW_AUTH_MFP_REQUIRED;
		else {
			/* Change Mfp parameter from DISABLED to OPTIONAL
			* if upper layer set MFPC = 1 in RSNE
			* since upper layer can't bring MFP OPTIONAL information
			* to driver by sme->mfp
			*/
			if (pAd->StaCfg.ucRSNMfpCap == RSN_AUTH_MFP_OPTIONAL)
				pAd->StaCfg.u4Mfp = IW_AUTH_MFP_OPTIONAL;
			else if (pAd->StaCfg.ucRSNMfpCap == RSN_AUTH_MFP_REQUIRED)
				DBGPRINT(RT_DEBUG_WARN, ("mfp parameter(DISABLED) conflict with mfp cap(REQUIRED)\n"));
		}
	}
#endif

		/*[TODO]may to check if assoc parameters change as cfg80211_auth*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		if (!pAd->ApCfg.ApCliTab[ifIndex].fgIsConnInitialized) {

			rStatus = RTMP_DRIVER_80211_CONNECT(pAd, pConnInfo, pNdev->ieee80211_ptr->iftype);

			if (rStatus != NDIS_STATUS_SUCCESS) {
				CFG80211DBG(RT_DEBUG_WARN, ("%s failed :%x\n", __func__, rStatus));
				return -EINVAL;
			}
		} else {
			/* skip join initial flow when it has been completed */
			DBGPRINT(RT_DEBUG_WARN, ("CNTL state is %ld\n", pAd->ApCfg.ApCliTab[ifIndex].CtrlCurrState));

			AssocParmFill(pAd, &AssocReq, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.Bssid, pAd->ApCfg.ApCliTab[ifIndex].MlmeAux.CapabilityInfo, ASSOC_TIMEOUT, 5);
			MlmeEnqueue(pAd, APCLI_ASSOC_STATE_MACHINE, APCLI_MT2_MLME_ASSOC_REQ, sizeof(MLME_ASSOC_REQ_STRUCT), &AssocReq, ifIndex);

		}
	} else
#endif
	{
		if (!pAd->StaCfg.fgIsConnInitialized) {

			rStatus = RTMP_DRIVER_80211_CONNECT(pAd, pConnInfo, pNdev->ieee80211_ptr->iftype);

			if (rStatus != NDIS_STATUS_SUCCESS) {
				CFG80211DBG(RT_DEBUG_WARN, ("%s failed :%x\n", __func__, rStatus));
				return -EINVAL;
			}
		} else {
			/* skip join initial flow when it has been completed */
			DBGPRINT(RT_DEBUG_WARN, ("CNTL state is %ld\n", pAd->Mlme.CntlMachine.CurrState));

			AssocParmFill(pAd, &AssocReq, pAd->MlmeAux.Bssid, pAd->MlmeAux.CapabilityInfo,
				ASSOC_TIMEOUT, pAd->StaCfg.DefaultListenCount);

			MlmeEnqueue(pAd, ASSOC_STATE_MACHINE, MT2_MLME_ASSOC_REQ,
					sizeof(MLME_ASSOC_REQ_STRUCT), &AssocReq, 0);

		}
	}
	return 0;
} /* End of CFG80211_OpsAssoc */


/*----------------------------------------------------------------------------*/
/*!
 * @brief This routine is responsible for requesting to deauth from
 *        currently connected ESS
 *
 * @param
 *
 * @retval 0:       successful
 *         others:  failure
 */
/*----------------------------------------------------------------------------*/
static int CFG80211_OpsDeauth(
			IN struct wiphy *pWiphy,
			IN struct net_device *pNdev,
			IN struct cfg80211_deauth_request *pReq)
{
#ifdef CONFIG_STA_SUPPORT
	void *pAdOrg;
	PRTMP_ADAPTER pAd;
	CMD_RTPRIV_IOCTL_80211_CONNECT *pConnInfo;
	POS_COOKIE pObj;
	UCHAR ifIndex;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __func__));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> ReasonCode = %d, iftype = %d\n", pReq->reason_code, pNdev->ieee80211_ptr->iftype));

	MAC80211_PAD_GET(pAdOrg, pWiphy);
	pAd = (PRTMP_ADAPTER)pAdOrg;
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	ifIndex = pObj->ioctl_if;

#ifdef WPA3_SUPPORT
	/* The BSS from cfg80211_ops.assoc must give back to
	 * cfg80211_send_rx_assoc() or to cfg80211_assoc_timeout().
	 * To ensure proper refcounting, new association requests
	 * while already associating must be rejected.
	 */
 #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37))
	if (pNdev->ieee80211_ptr->iftype == RT_CMD_80211_IFTYPE_P2P_CLIENT) {
		CFG80211DBG(RT_DEBUG_TRACE, ("[wlan] CFG80211_OpsDeauth: iftype %d ifIndex %d\n", pNdev->ieee80211_ptr->iftype, ifIndex));
		pConnInfo = &pAd->ApCfg.ApCliTab[ifIndex].ConnInfo;
	} else
#endif
		pConnInfo = &pAd->StaCfg.ConnInfo;

	if (pConnInfo->bss) {
		CFG80211DBG(RT_DEBUG_TRACE, ("assoc timeout notify\n"));
		/* ops caller have already hold the mutex. */
		cfg80211_assoc_timeout(pNdev, pConnInfo->bss);
		CFG80211DBG(RT_DEBUG_TRACE, ("assoc timeout notify, Done\n"));
		pConnInfo->bss = NULL;
	}
#endif
	RTMP_DRIVER_80211_STA_LEAVE(pAd, pNdev);
#endif /* CONFIG_STA_SUPPORT */
	return 0;
}

static int CFG80211_OpsDisassoc(
				IN struct wiphy *pWiphy,
				IN struct net_device *pNdev,
				IN struct cfg80211_disassoc_request *pReq)
{
	void *pAdOrg;
	PRTMP_ADAPTER pAd;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __func__));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> ReasonCode = %d\n", pReq->reason_code));

	MAC80211_PAD_GET(pAdOrg, pWiphy);
	pAd = (PRTMP_ADAPTER)pAdOrg;

	/* not implemented yet */

	return -EINVAL;
}

#endif /* WPA3_SUPPORT */

/*
========================================================================
Routine Description:
	Disconnect from the BSS/ESS.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	ReasonCode		- 

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: connect
========================================================================
*/
static int CFG80211_OpsDisconnect(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN UINT16								ReasonCode)
{
#ifdef CONFIG_STA_SUPPORT
	VOID *pAd;


	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> ReasonCode = %d\n", ReasonCode));

	MAC80211_PAD_GET(pAd, pWiphy);

	RTMP_DRIVER_80211_STA_LEAVE(pAd, pNdev);
#endif /*CONFIG_STA_SUPPORT*/	
	return 0;
} 
#endif /* LINUX_VERSION_CODE */


#ifdef RFKILL_HW_SUPPORT
static int CFG80211_OpsRFKill(
	IN struct wiphy						*pWiphy)
{
	VOID		*pAd;
	BOOLEAN		active;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	RTMP_DRIVER_80211_RFKILL(pAd, &active);
	wiphy_rfkill_set_hw_state(pWiphy, !active);	
	return active;
}


VOID CFG80211_RFKillStatusUpdate(
	IN PVOID							pAd,
	IN BOOLEAN							active)
{
	struct wiphy *pWiphy;
	CFG80211_CB *pCfg80211_CB;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	RTMP_DRIVER_80211_CB_GET(pAd, &pCfg80211_CB);
	pWiphy = pCfg80211_CB->pCfg80211_Wdev->wiphy;
	wiphy_rfkill_set_hw_state(pWiphy, !active);
	return;
}
#endif /* RFKILL_HW_SUPPORT */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
/*
========================================================================
Routine Description:
	Get site survey information.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	Idx				-
	pSurvey			-

Return Value:
	0				- success
	-x				- fail

Note:
	For iw utility: survey dump
========================================================================
*/
/*Nobody uses it currently*/
#if 0 
static int CFG80211_OpsSurveyGet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN int								Idx,
	IN struct survey_info				*pSurvey)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	VOID *pAd;
	CMD_RTPRIV_IOCTL_80211_SURVEY SurveyInfo;


	if (Idx != 0)
		return -ENOENT;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));

	MAC80211_PAD_GET(pAd, pWiphy);

	/* get information from driver */
	RTMP_DRIVER_80211_SURVEY_GET(pAd, &SurveyInfo);

	/* return the information to upper layer */
	pSurvey->channel = ((CFG80211_CB *)(SurveyInfo.pCfg80211))->pCfg80211_Channels;
	pSurvey->filled = SURVEY_INFO_CHANNEL_TIME_BUSY |
						SURVEY_INFO_CHANNEL_TIME_EXT_BUSY;
	pSurvey->channel_time_busy = SurveyInfo.ChannelTimeBusy; /* unit: us */
	pSurvey->channel_time_ext_busy = SurveyInfo.ChannelTimeExtBusy;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> busy time = %ld %ld\n",
				(ULONG)SurveyInfo.ChannelTimeBusy,
				(ULONG)SurveyInfo.ChannelTimeExtBusy));
	return 0;
#else

	return -ENOTSUPP;
#endif /* LINUX_VERSION_CODE */
} /* End of CFG80211_OpsSurveyGet */
#endif

/*
========================================================================
Routine Description:
	Cache a PMKID for a BSSID.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pPmksa			- PMKID information

Return Value:
	0				- success
	-x				- fail

Note:
	This is mostly useful for fullmac devices running firmwares capable of
	generating the (re) association RSN IE.
	It allows for faster roaming between WPA2 BSSIDs.
========================================================================
*/
static int CFG80211_OpsPmksaSet(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN struct cfg80211_pmksa			*pPmksa)
{
#ifdef CONFIG_STA_SUPPORT
	VOID *pAd;
	RT_CMD_STA_IOCTL_PMA_SA IoctlPmaSa, *pIoctlPmaSa = &IoctlPmaSa;


	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	if ((pPmksa->bssid == NULL) || (pPmksa->pmkid == NULL))
		return -ENOENT;

	pIoctlPmaSa->Cmd = RT_CMD_STA_IOCTL_PMA_SA_ADD;
	pIoctlPmaSa->pBssid = (UCHAR *)pPmksa->bssid;
	pIoctlPmaSa->pPmkid = pPmksa->pmkid;

	RTMP_DRIVER_80211_PMKID_CTRL(pAd, pIoctlPmaSa);
#endif /* CONFIG_STA_SUPPORT */

	return 0;
} /* End of CFG80211_OpsPmksaSet */


/*
========================================================================
Routine Description:
	Delete a cached PMKID.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface
	pPmksa			- PMKID information

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsPmksaDel(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev,
	IN struct cfg80211_pmksa			*pPmksa)
{
#ifdef CONFIG_STA_SUPPORT
	VOID *pAd;
	RT_CMD_STA_IOCTL_PMA_SA IoctlPmaSa, *pIoctlPmaSa = &IoctlPmaSa;

	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	if ((pPmksa->bssid == NULL) || (pPmksa->pmkid == NULL))
		return -ENOENT;

	pIoctlPmaSa->Cmd = RT_CMD_STA_IOCTL_PMA_SA_REMOVE;
	pIoctlPmaSa->pBssid = (UCHAR *)pPmksa->bssid;
	pIoctlPmaSa->pPmkid = pPmksa->pmkid;

	RTMP_DRIVER_80211_PMKID_CTRL(pAd, pIoctlPmaSa);
#endif /* CONFIG_STA_SUPPORT */

	return 0;
} /* End of CFG80211_OpsPmksaDel */


/*
========================================================================
Routine Description:
	Flush a cached PMKID.

Arguments:
	pWiphy			- Wireless hardware description
	pNdev			- Network device interface

Return Value:
	0				- success
	-x				- fail

Note:
========================================================================
*/
static int CFG80211_OpsPmksaFlush(
	IN struct wiphy						*pWiphy,
	IN struct net_device				*pNdev)
{
#ifdef CONFIG_STA_SUPPORT
	VOID *pAd;
	RT_CMD_STA_IOCTL_PMA_SA IoctlPmaSa, *pIoctlPmaSa = &IoctlPmaSa;


	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);

	pIoctlPmaSa->Cmd = RT_CMD_STA_IOCTL_PMA_SA_FLUSH;
	RTMP_DRIVER_80211_PMKID_CTRL(pAd, pIoctlPmaSa);
#endif /* CONFIG_STA_SUPPORT */

	return 0;
} /* End of CFG80211_OpsPmksaFlush */
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static int CFG80211_OpsRemainOnChannel(
	IN struct wiphy *pWiphy,
	IN struct wireless_dev *pWdev,	
	IN struct ieee80211_channel *pChan,		
	IN unsigned int duration,	
	OUT u64 *cookie)
#else
static int CFG80211_OpsRemainOnChannel(
	IN struct wiphy *pWiphy,
	IN struct net_device *dev,	
	IN struct ieee80211_channel *pChan,	
	IN enum nl80211_channel_type ChannelType,	
	IN unsigned int duration,	
	OUT u64 *cookie)
#endif /* LINUX_VERSION_CODE: 3.6.0 */
{
	VOID *pAd;
	UINT32 ChanId;		
	CMD_RTPRIV_IOCTL_80211_CHAN ChanInfo;	
	u32 rndCookie;

	PRTMP_ADAPTER prAd;
	PCFG80211_CTRL pCfg80211_ctrl;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	struct net_device *dev = NULL;
	INT ChannelType = RT_CMD_80211_CHANTYPE_HT20;
	dev = pWdev->netdev;	
#endif /* LINUX_VERSION_CODE: 3.6.0 */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	rndCookie = 0x1234;
#else
	rndCookie = random32() | 1;	
#endif /* LINUX_VERSION_CODE: 3.10.0 */

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));

	MAC80211_PAD_GET(pAd, pWiphy);
	
	/*Init listen channel flag*/
	prAd = (PRTMP_ADAPTER)pAd;		
	pCfg80211_ctrl = &prAd->cfg80211_ctrl;
	pCfg80211_ctrl->IsInListenProgress = TRUE;

	/*CFG_TODO: Shall check channel type*/
	
	/* get channel number */	
	ChanId = ieee80211_frequency_to_channel(pChan->center_freq);	
	CFG80211DBG(RT_DEBUG_TRACE, ("%s: CH = %d, Type = %d, duration = %d, cookie=%d\n", __FUNCTION__, 
		ChanId, ChannelType, duration, rndCookie));	

	/* init */
	*cookie = rndCookie;
	memset(&ChanInfo, 0, sizeof(ChanInfo));	
	ChanInfo.ChanId = (UINT8)ChanId;
	ChanInfo.IfType = (UINT8)dev->ieee80211_ptr->iftype;
	ChanInfo.ChanType = (UINT8)ChannelType;
	ChanInfo.chan = pChan;
	ChanInfo.cookie = rndCookie;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))	
	ChanInfo.pWdev = pWdev;
#endif /* LINUX_VERSION_CODE: 3.6.0 */

	/* set channel */	
	RTMP_DRIVER_80211_REMAIN_ON_CHAN_SET(pAd, &ChanInfo, duration);
	return 0;	
}

static void CFG80211_OpsMgmtFrameRegister(
    struct wiphy *pWiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	struct wireless_dev *wdev,
#else
    struct net_device *dev,
#endif /* LINUX_VERSION_CODE: 3.6.0 */	
    UINT16 frame_type, bool reg)
{
	VOID *pAd;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	struct net_device *dev = NULL;
#endif /* LINUX_VERSION_CODE: 3.6.0 */

	void_MAC80211_PAD_GET(pAd, pWiphy);
	
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	RTMP_DRIVER_NET_DEV_GET(pAd, &dev);
#endif /* LINUX_VERSION_CODE: 3.6.0 */	

	CFG80211DBG(RT_DEBUG_INFO, ("80211> %s ==>\n", __FUNCTION__));
	CFG80211DBG(RT_DEBUG_INFO, ("frame_type = %x, req = %d , (%d)\n", frame_type, reg,  dev->ieee80211_ptr->iftype));

	if (frame_type == IEEE80211_STYPE_PROBE_REQ)
		RTMP_DRIVER_80211_MGMT_FRAME_REG(pAd, dev, reg);
	else if (frame_type == IEEE80211_STYPE_ACTION)
		RTMP_DRIVER_80211_ACTION_FRAME_REG(pAd, dev, reg);
	else
		CFG80211DBG(RT_DEBUG_ERROR, ("Unkown frame_type = %x, req = %d\n", frame_type, reg));	
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
int CFG80211_OpsSuspend(IN struct wiphy *wiphy,struct cfg80211_wowlan *wow)
{
#ifdef MT_WOW_SUPPORT
	VOID *pAd;
	PRTMP_ADAPTER pAd_wow;
#endif

	CFG80211DBG(RT_DEBUG_OFF, ("%s\n",__FUNCTION__));
	if (wow == NULL){
		CFG80211DBG(RT_DEBUG_OFF, ("cfg80211_wowlan is NULL!\n"));
		return 0;
		}
#ifdef MT_WOW_SUPPORT
	CFG80211DBG(RT_DEBUG_TRACE, ("wowlan.any = %d , wowlan.disconnect = %d , wowlan.magic_pkt = %d , \nwowlan.gtk_rekey_failure = %d , wowlan.eap_identity_req = %d , wowlan.four_way_handshake = %d , \n wowlan.rfkill_release = %d \n",
		wow->any,wow->disconnect,wow->magic_pkt,wow->gtk_rekey_failure,wow->eap_identity_req,wow->four_way_handshake,wow->rfkill_release));
	MAC80211_PAD_GET(pAd, wiphy);
	pAd_wow = (PRTMP_ADAPTER)pAd;
	CFG80211DBG(RT_DEBUG_ERROR, ("Driver WOW_Enable = %d \n",pAd_wow->WOW_Cfg.bEnable));
	if(wow && pAd_wow->WOW_Cfg.bEnable){
		RTMP_DRIVER_ADAPTER_RT28XX_WOW_ENABLE(pAd);
		return 0;
	}
	else
#endif

	return 1;
}

void CFG80211_OpsSetWakeup(IN struct wiphy *wiphy, bool enabled)
{
	CFG80211DBG(RT_DEBUG_OFF, ("%s ==> wowlan_enable = %d \n",__FUNCTION__,enabled));
			
}

int CFG80211_OpsResume(IN struct wiphy *wiphy)
{
	CFG80211DBG(RT_DEBUG_OFF, ("%s\n", __func__));

	return 0;
}

int CFG80211_OpsSetRekeyData(struct wiphy *wiphy, struct net_device *dev,
                                        struct cfg80211_gtk_rekey_data *data)
{
	void *pAdSrc = NULL;
	PRTMP_ADAPTER pAd = NULL;
	CFG80211DBG(RT_DEBUG_OFF, ("%s\n", __func__));

	MAC80211_PAD_GET(pAdSrc, wiphy);
	pAd = (PRTMP_ADAPTER)pAdSrc;
	NdisCopyMemory(pAd->WOW_Cfg.PTK, data->kck, LEN_PTK_KCK);
	NdisCopyMemory(&pAd->WOW_Cfg.PTK[LEN_PTK_KCK], data->kek, LEN_PTK_KEK);
	NdisCopyMemory(pAd->WOW_Cfg.ReplayCounter, data->replay_ctr, LEN_KEY_DESC_REPLAY);

	return 0;
}

#endif

#if (KERNEL_VERSION(3, 4, 0) < LINUX_VERSION_CODE)
//Supplicant_NEW_TDLS
#ifdef CFG_TDLS_SUPPORT
static int CFG80211_OpsTdlsMgmt
	( 
	IN struct wiphy *pWiphy,
    IN struct net_device *pDev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
	IN const u8 *peer,
#else
    IN u8 *peer,
#endif
    IN u8 action_code, 
    IN u8 dialog_token,
    IN u16 status_code,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
	IN u32 peer_capability,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0))
	IN bool initiator,
#endif
#endif
    IN const u8 *extra_ies,
    IN size_t extra_ies_len
	)
{
	int ret = 0;
	VOID *pAd;
	MAC80211_PAD_GET(pAd, pWiphy);

	CFG80211DBG(RT_DEBUG_WARN, ("80211> extra_ies_len : %zd ==>\n", extra_ies_len));

	switch (action_code) {
	case (u8)(WLAN_TDLS_SETUP_REQUEST):
	case (u8)(WLAN_TDLS_DISCOVERY_REQUEST):
	case (u8)(WLAN_TDLS_SETUP_CONFIRM):
	case (u8)(WLAN_TDLS_TEARDOWN):
	case (u8)(WLAN_TDLS_SETUP_RESPONSE):
	case (u8)(WLAN_PUB_ACTION_TDLS_DISCOVER_RES):
		cfg_tlds_build_frame(pAd,peer,dialog_token,action_code,status_code,extra_ies,extra_ies_len,FALSE,0,0);
		break;
	case (u8)(TDLS_ACTION_CODE_WFD_TUNNELED_PROBE_REQ):
		cfg_tdls_TunneledProbeRequest(pAd,peer,extra_ies,extra_ies_len);
		break;
	case (u8)(TDLS_ACTION_CODE_WFD_TUNNELED_PROBE_RSP):
		cfg_tdls_TunneledProbeResponse(pAd,peer,extra_ies,extra_ies_len);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;	
	
}

static int CFG80211_OpsTdlsOper(
	IN struct wiphy *pWiphy,
	IN struct net_device *pDev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 15, 0))
		IN const u8 *peer,
#else
		IN u8 *peer,
#endif
	IN enum nl80211_tdls_operation oper)
{
	
	VOID *pAd;
	MAC80211_PAD_GET(pAd, pWiphy);
	
	switch(oper)
	{
	case NL80211_TDLS_ENABLE_LINK:				
		CFG80211DRV_StaTdlsInsertDeletepEntry(pAd, peer, tdls_insert_entry);
	break;
	case NL80211_TDLS_DISABLE_LINK:
		CFG80211DRV_StaTdlsInsertDeletepEntry(pAd, peer, tdls_delete_entry);
	break;
	default:
		break;
	}
	return 0;
}
#endif /*CFG_TDLS_SUPPORT*/
#endif /* LINUX_VERSION_CODE: 3.1.10 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 0))
static int CFG80211_OpsMgmtTx(
    IN struct wiphy *pWiphy,
    IN struct wireless_dev *wdev,
    IN struct cfg80211_mgmt_tx_params *params,
    IN u64 *pCookie)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static int CFG80211_OpsMgmtTx(
    IN struct wiphy *pWiphy,
    IN struct wireless_dev *wdev,
#if (KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE)
	struct cfg80211_mgmt_tx_params *params,
#else
    IN struct ieee80211_channel *pChan,
    IN bool Offchan,
    IN unsigned int Wait,
    IN const u8 *pBuf,
    IN size_t Len,
    IN bool no_cck,	
    IN bool done_wait_for_ack, 
#endif  /* >= 3.14.0 */
    IN u64 *pCookie)
#else
static int CFG80211_OpsMgmtTx(
    IN struct wiphy *pWiphy,
    IN struct net_device *pDev,
    IN struct ieee80211_channel *pChan,
    IN bool Offchan,
    IN enum nl80211_channel_type ChannelType,
    IN bool ChannelTypeValid,
    IN unsigned int Wait,
    IN const u8 *pBuf,
    IN size_t Len,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
    IN bool no_cck,	
#endif /* LINUX_VERSION_CODE: 3.2.0 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
    IN bool done_wait_for_ack, 
#endif /* LINUX_VERSION_CODE: 3.3.0 */
    IN u64 *pCookie)
#endif /* LINUX_VERSION_CODE: 3.6.0 */	
{
    VOID *pAd;
    UINT32 ChanId;
	PRTMP_ADAPTER prAd;
	PCFG80211_CTRL pCfg80211_ctrl;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	struct net_device *pDev = NULL;
#endif /* LINUX_VERSION_CODE: 3.6.0 */

    CFG80211DBG(RT_DEBUG_INFO, ("80211> %s ==>\n", __FUNCTION__));

	/* check null pointer */
if (pWiphy == NULL || pCookie == NULL
#if (KERNEL_VERSION(3, 18, 0) <= LINUX_VERSION_CODE)
	|| wdev == NULL || params == NULL
#elif (KERNEL_VERSION(3, 6, 0) <= LINUX_VERSION_CODE)
	|| wdev == NULL
#else
	|| pDev == NULL
#endif
	) {
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> %s() NULL pointer parameters!\n", __func__));
		return 0;
	}

#if (KERNEL_VERSION(3, 6, 0) <= LINUX_VERSION_CODE)
	pDev = wdev->netdev;
#endif /* LINUX_VERSION_CODE: 3.6.0 */

	MAC80211_PAD_GET(pAd, pWiphy);
	prAd = (PRTMP_ADAPTER)pAd;
    pCfg80211_ctrl = &prAd->cfg80211_ctrl;
    pCfg80211_ctrl->TxStatusSeq = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
#if (KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE)
    pCfg80211_ctrl->IsNeedTxStatus = !(params->dont_wait_for_ack);
#else
    pCfg80211_ctrl->IsNeedTxStatus = !done_wait_for_ack;
#endif
#else
    pCfg80211_ctrl->IsNeedTxStatus = FALSE;
#endif
    CFG80211DBG(RT_DEBUG_INFO, ("80211> IsNeedTxStatus = %d\n", pCfg80211_ctrl->IsNeedTxStatus));

#if (KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE)
	if (!(params->chan)) {
#else
	if (!pChan) {
#endif
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> %s() NULL ieee80211_channel!\n", __func__));
		*pCookie = 5678;
		CFG80211_SendMgmtFrameDone(pAd, 5678, FALSE);
		return 0;
	}
		
    /* get channel number */
#if (KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE)
    ChanId = ieee80211_frequency_to_channel(params->chan->center_freq);
#else
    ChanId = ieee80211_frequency_to_channel(pChan->center_freq);
#endif
    CFG80211DBG(RT_DEBUG_INFO, ("80211> Mgmt Channel = %d\n", ChanId));

	/* Send the Frame with basic rate 6 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#if (KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE)
    if (params->no_cck)
		; //pAd->isCfgDeviceInP2p = TRUE;
#else
    if (no_cck)
		; //pAd->isCfgDeviceInP2p = TRUE;
#endif/* LINUX_VERSION_CODE: 3.18.0 */		
#else
		
#endif /* LINUX_VERSION_CODE: 3.2.0 */	
	
	*pCookie = 5678;
	RTMP_DRIVER_80211_TX_NETDEV_SET(pAd, pDev);
    RTMP_DRIVER_80211_CHANNEL_LOCK(pAd, ChanId);
#if (KERNEL_VERSION(3, 14, 0) <= LINUX_VERSION_CODE)
    RTMP_DRIVER_80211_MGMT_FRAME_SEND(pAd, (void *)(params->buf), params->len);
#else
    RTMP_DRIVER_80211_MGMT_FRAME_SEND(pAd, (void *)pBuf, Len);
#endif
	/* Mark it for using Supplicant-Based off-channel wait
		if (Offchan)
			RTMP_DRIVER_80211_CHANNEL_RESTORE(pAd);
	 */

	if (pCfg80211_ctrl->IsNeedTxStatus)
		*pCookie = pCfg80211_ctrl->TxStatusSeq;
	CFG80211DBG(RT_DEBUG_TRACE, ("%s, Cookie = 0x%llx, IsNeedTxStatus (%d), DevName (%s)\n",
					__func__, (long long unsigned int)*pCookie,
					pCfg80211_ctrl->IsNeedTxStatus, pDev->name));
	/*reset IsNeedTxStatus to FALSE*/
	pCfg80211_ctrl->IsNeedTxStatus = FALSE; 
    return 0;
} 

static int CFG80211_OpsTxCancelWait(
    IN struct wiphy *pWiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
    IN struct wireless_dev *wdev,
#else
    IN struct net_device *pDev,
#endif /* LINUX_VERSION_CODE: 3.6.0 */
    u64 cookie)
{
	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));
    return 0;
} 

static int CFG80211_OpsCancelRemainOnChannel(
    struct wiphy *pWiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	struct wireless_dev *wdev,
#else	
    struct net_device *dev,
#endif /* LINUX_VERSION_CODE: 3.6.0 */
    u64 cookie)
{
    VOID *pAd;	
    CFG80211DBG(RT_DEBUG_INFO, ("80211> %s ==>\n", __FUNCTION__));	
    MAC80211_PAD_GET(pAd, pWiphy);
    /* It cause the Supplicant-based OffChannel Hang */	
	RTMP_DRIVER_80211_CANCEL_REMAIN_ON_CHAN_SET(pAd, (ULONG)cookie);
    return 0;
}
 
#ifdef CONFIG_AP_SUPPORT
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static int CFG80211_OpsSetBeacon(
        struct wiphy *pWiphy,
        struct net_device *netdev,
        struct beacon_parameters *info)
{
    VOID *pAd;
    CMD_RTPRIV_IOCTL_80211_BEACON bcn;
    UCHAR *beacon_head_buf, *beacon_tail_buf;
#if (KERNEL_VERSION(3, 2, 0) <= LINUX_VERSION_CODE)
	UCHAR *proberesp_ies, *assocresp_ies;
#endif

	NdisZeroMemory(&bcn, sizeof(CMD_RTPRIV_IOCTL_80211_BEACON));
	
    CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
    MAC80211_PAD_GET(pAd, pWiphy);	

    hex_dump("Beacon head", info->head, info->head_len);
    hex_dump("Beacon tail", info->tail, info->tail_len);
    CFG80211DBG(RT_DEBUG_TRACE, ("80211>dtim_period = %d \n", info->dtim_period));
    CFG80211DBG(RT_DEBUG_TRACE, ("80211>interval = %d \n", info->interval));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)) 
    CFG80211DBG(RT_DEBUG_TRACE, ("80211>ssid = %s \n", info->ssid));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211>ssid_len = %u\n", info->ssid_len));
    	
    CFG80211DBG(RT_DEBUG_TRACE, ("80211>beacon_ies_len = %d \n", info->beacon_ies_len));
    CFG80211DBG(RT_DEBUG_TRACE, ("80211>proberesp_ies_len = %d \n", info->proberesp_ies_len));
    CFG80211DBG(RT_DEBUG_TRACE, ("80211>assocresp_ies_len = %d \n", info->assocresp_ies_len));		

	if (info->proberesp_ies_len > 0 && info->proberesp_ies) {
		os_alloc_mem(NULL, &proberesp_ies, info->proberesp_ies_len);
		NdisCopyMemory(proberesp_ies, info->proberesp_ies, info->proberesp_ies_len);

		if (proberesp_ies) {
			RTMP_DRIVER_80211_AP_PROBE_RSP(pAd, proberesp_ies, info->proberesp_ies_len);
			os_free_mem(NULL, proberesp_ies);
		} else
			CFG80211DBG(RT_DEBUG_ERROR, ("%s() alloc mem for proberesp_ies failed\n",
				__func__));
	}

	if (info->assocresp_ies_len > 0 && info->assocresp_ies) {
		os_alloc_mem(NULL, &assocresp_ies, info->assocresp_ies_len);
		NdisCopyMemory(assocresp_ies, info->assocresp_ies, info->assocresp_ies_len);

		if (assocresp_ies) {
			RTMP_DRIVER_80211_AP_ASSOC_RSP(pAd, assocresp_ies, info->assocresp_ies_len);
			os_free_mem(NULL, assocresp_ies);
		} else
			CFG80211DBG(RT_DEBUG_ERROR,
				("%s() 80211>alloc mem for assocresp_ies failed\n",
				__func__));
	}
#endif /* LINUX_VERSION_CODE: 3.2.0 */    		

    os_alloc_mem(NULL, &beacon_head_buf, info->head_len);
    NdisCopyMemory(beacon_head_buf, info->head, info->head_len);
    
	os_alloc_mem(NULL, &beacon_tail_buf, info->tail_len);
	NdisCopyMemory(beacon_tail_buf, info->tail, info->tail_len);

    bcn.beacon_head_len = info->head_len;
	bcn.beacon_tail_len = info->tail_len;
    bcn.beacon_head = beacon_head_buf;
	bcn.beacon_tail = beacon_tail_buf;
    bcn.dtim_period = info->dtim_period;
	bcn.interval = info->interval;


    RTMP_DRIVER_80211_BEACON_SET(pAd, &bcn);

    if (beacon_head_buf)
        os_free_mem(NULL, beacon_head_buf);
	
	if (beacon_tail_buf)	
	    os_free_mem(NULL, beacon_tail_buf);

    return 0;
}

static int CFG80211_OpsAddBeacon(
        struct wiphy *pWiphy,
        struct net_device *netdev,
        struct beacon_parameters *info)
{
	
    	VOID *pAd;
    	CMD_RTPRIV_IOCTL_80211_BEACON bcn;
    	UCHAR *beacon_head_buf, *beacon_tail_buf;
#if (KERNEL_VERSION(3, 2, 0) <= LINUX_VERSION_CODE)
	UCHAR *proberesp_ies, *assocresp_ies;
#endif
	NdisZeroMemory(&bcn, sizeof(CMD_RTPRIV_IOCTL_80211_BEACON));
		
    	MAC80211_PAD_GET(pAd, pWiphy);	
    	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));

    	hex_dump("Beacon head", info->head, info->head_len);
    	hex_dump("Beacon tail", info->tail, info->tail_len);
    	CFG80211DBG(RT_DEBUG_TRACE, ("80211>dtim_period = %d \n", info->dtim_period));
    	CFG80211DBG(RT_DEBUG_TRACE, ("80211>interval = %d \n", info->interval));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)) 
   	CFG80211DBG(RT_DEBUG_TRACE, ("80211>ssid = %s \n", info->ssid));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211>ssid_len = %u\n", info->ssid_len));

	CFG80211DBG(RT_DEBUG_TRACE, ("80211>beacon_ies_len = %d\n", info->beacon_ies_len));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211>proberesp_ies_len = %d\n", info->proberesp_ies_len));
	CFG80211DBG(RT_DEBUG_TRACE, ("80211>assocresp_ies_len = %d\n", info->assocresp_ies_len));

	if (info->proberesp_ies_len > 0 && info->proberesp_ies) {
		os_alloc_mem(NULL, &proberesp_ies, info->proberesp_ies_len);
		NdisCopyMemory(proberesp_ies, info->proberesp_ies, info->proberesp_ies_len);

		if (proberesp_ies) {
			RTMP_DRIVER_80211_AP_PROBE_RSP(pAd, proberesp_ies, info->proberesp_ies_len);
			os_free_mem(NULL, proberesp_ies);
		} else
			CFG80211DBG(RT_DEBUG_ERROR,
				("%s() alloc mem for proberesp_ies failed\n",
				__func__));
	}

	if (info->assocresp_ies_len > 0 && info->assocresp_ies) {
		os_alloc_mem(NULL, &assocresp_ies, info->assocresp_ies_len);
		NdisCopyMemory(assocresp_ies, info->assocresp_ies, info->assocresp_ies_len);

		if (assocresp_ies) {
			RTMP_DRIVER_80211_AP_ASSOC_RSP(pAd, assocresp_ies, info->assocresp_ies_len);
			os_free_mem(NULL, assocresp_ies);
		} else
			CFG80211DBG(RT_DEBUG_ERROR,
				("%s() 80211>alloc mem for assocresp_ies failed\n",
				__func__));
	}
#endif /* LINUX_VERSION_CODE: 3.2.0 */

	os_alloc_mem(NULL, &beacon_head_buf, info->head_len);
	NdisCopyMemory(beacon_head_buf, info->head, info->head_len);
    
	os_alloc_mem(NULL, &beacon_tail_buf, info->tail_len);
	NdisCopyMemory(beacon_tail_buf, info->tail, info->tail_len);

	bcn.beacon_head_len = info->head_len;
	bcn.beacon_tail_len = info->tail_len;
	bcn.beacon_head = beacon_head_buf;
	bcn.beacon_tail = beacon_tail_buf;
	bcn.dtim_period = info->dtim_period;
	bcn.interval = info->interval;

	RTMP_DRIVER_80211_BEACON_ADD(pAd, &bcn);

	if (beacon_head_buf)
		os_free_mem(NULL, beacon_head_buf);
	
	if (beacon_tail_buf)	
	    	os_free_mem(NULL, beacon_tail_buf);

    	return 0;
}

static int CFG80211_OpsDelBeacon(
        struct wiphy *pWiphy,
        struct net_device *netdev)
{
    VOID *pAd;
    MAC80211_PAD_GET(pAd, pWiphy);

    CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));

    RTMP_DRIVER_80211_BEACON_DEL(pAd);
    return 0;
}

#else // ! LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)

static int CFG80211_OpsStartAp(
	struct wiphy *pWiphy,
	struct net_device *netdev,
	struct cfg80211_ap_settings *settings)
{
    VOID *pAd;
    CMD_RTPRIV_IOCTL_80211_BEACON bcn;
	UCHAR *beacon_head_buf = NULL;
	UCHAR *beacon_tail_buf = NULL;
    
    MAC80211_PAD_GET(pAd, pWiphy);	
    CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));    
	NdisZeroMemory(&bcn,sizeof(CMD_RTPRIV_IOCTL_80211_BEACON));
	
	if (settings->beacon.head_len > 0) 
	{
		os_alloc_mem(NULL, &beacon_head_buf, settings->beacon.head_len);
		if (beacon_head_buf)
			NdisCopyMemory(beacon_head_buf,
				settings->beacon.head, settings->beacon.head_len);
	}

	if (settings->beacon.tail_len > 0) 
	{
		os_alloc_mem(NULL, &beacon_tail_buf, settings->beacon.tail_len);
		if (beacon_tail_buf)
			NdisCopyMemory(beacon_tail_buf,
				settings->beacon.tail, settings->beacon.tail_len);
	}

	bcn.beacon_head_len = settings->beacon.head_len;
	bcn.beacon_tail_len = settings->beacon.tail_len;
	bcn.beacon_head = beacon_head_buf;
	bcn.beacon_tail = beacon_tail_buf;
	bcn.dtim_period = settings->dtim_period;
	bcn.interval = settings->beacon_interval;
	bcn.ssid_len = settings->ssid_len;
	bcn.privacy = (BOOLEAN)settings->privacy;
	NdisZeroMemory(&bcn.ssid[0], MAX_LEN_OF_SSID);
	if (settings->ssid && (settings->ssid_len <= 32))
		NdisCopyMemory(&bcn.ssid[0], settings->ssid, settings->ssid_len);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	bcn.hidden_ssid = (UCHAR)settings->hidden_ssid;
#endif /*LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)*/


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	/* set channel callback has been replaced by using chandef of cfg80211_ap_settings */
	if(settings->chandef.chan)
	{
		CFG80211_CB *p80211CB;
		CMD_RTPRIV_IOCTL_80211_CHAN ChanInfo;
		UINT32 ChanId;
		
		/* init */
		memset(&ChanInfo, 0, sizeof(ChanInfo));
	
		p80211CB = NULL;
		RTMP_DRIVER_80211_CB_GET(pAd, &p80211CB);
	
		if (p80211CB == NULL)
		{
			CFG80211DBG(RT_DEBUG_ERROR, ("80211> p80211CB == NULL!\n"));
			if (beacon_head_buf)
				os_free_mem(NULL, beacon_head_buf);
			if (beacon_tail_buf)
				os_free_mem(NULL, beacon_tail_buf);
			return 0;
		}
		
		/* get channel number */
		ChanId = ieee80211_frequency_to_channel(settings->chandef.chan->center_freq);
		CFG80211DBG(RT_DEBUG_OFF, ("80211> Channel = %d\n", ChanId));
		ChanInfo.ChanId = (UINT8)ChanId;
	
		ChanInfo.IfType = RT_CMD_80211_IFTYPE_P2P_GO;
	
		CFG80211DBG(RT_DEBUG_OFF, ("80211> ChanInfo.IfType == %d!\n", ChanInfo.IfType));
	
		if (cfg80211_get_chandef_type(&settings->chandef) == NL80211_CHAN_NO_HT)
			ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_NOHT;
		else if (cfg80211_get_chandef_type(&settings->chandef) == NL80211_CHAN_HT20)
			ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT20;
		else if (cfg80211_get_chandef_type(&settings->chandef) == NL80211_CHAN_HT40MINUS)
			ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT40MINUS;
		else if (cfg80211_get_chandef_type(&settings->chandef) == NL80211_CHAN_HT40PLUS)
			ChanInfo.ChanType = RT_CMD_80211_CHANTYPE_HT40PLUS;
		
		CFG80211DBG(RT_DEBUG_OFF, ("80211> ChanInfo.ChanType == %d!\n", ChanInfo.ChanType));
		ChanInfo.MonFilterFlag = p80211CB->MonFilterFlag;
	
		/* set channel */
		RTMP_DRIVER_80211_CHAN_SET(pAd, &ChanInfo);
	}
#endif

	RTMP_DRIVER_80211_BEACON_ADD(pAd, &bcn);

	if (beacon_head_buf)
		os_free_mem(NULL, beacon_head_buf);
	if (beacon_tail_buf)
		os_free_mem(NULL, beacon_tail_buf);

	return 0;
}


static int CFG80211_OpsChangeBeacon(
	struct wiphy *pWiphy,
	struct net_device *netdev,
	struct cfg80211_beacon_data *info)
{
	VOID *pAd;
	CMD_RTPRIV_IOCTL_80211_BEACON bcn;
	UCHAR *beacon_head_buf = NULL;
	UCHAR *beacon_tail_buf = NULL;
    
    MAC80211_PAD_GET(pAd, pWiphy);	
    CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	
	if (info->head_len > 0) 
	{
		os_alloc_mem(NULL, &beacon_head_buf, info->head_len);
		if (beacon_head_buf)
			NdisCopyMemory(beacon_head_buf, info->head, info->head_len);
	}

	if (info->tail_len > 0) {
		os_alloc_mem(NULL, &beacon_tail_buf, info->tail_len);
		if (beacon_tail_buf)
			NdisCopyMemory(beacon_tail_buf, info->tail, info->tail_len);
	}

	memset(&bcn, 0, sizeof(CMD_RTPRIV_IOCTL_80211_BEACON));

	bcn.beacon_head_len = info->head_len;
	bcn.beacon_tail_len = info->tail_len;
	bcn.beacon_head = beacon_head_buf;
	bcn.beacon_tail = beacon_tail_buf;
	bcn.dtim_period = 0; /* No Change */
	bcn.interval = 0;    /* No Change */

  	/* Update assoc resp extra ie */
    	if (info->assocresp_ies_len && info->assocresp_ies)
    	{
		RTMP_DRIVER_80211_AP_ASSOC_RSP(pAd, (void *)info->assocresp_ies, info->assocresp_ies_len);
    	}


	RTMP_DRIVER_80211_BEACON_SET(pAd, &bcn);

	if (beacon_head_buf)
		os_free_mem(NULL, beacon_head_buf);
	if (beacon_tail_buf)
		os_free_mem(NULL, beacon_tail_buf);
	return 0;

}

static int CFG80211_OpsStopAp(
	struct wiphy *pWiphy,
	struct net_device *netdev)
{
	VOID *pAd;
	MAC80211_PAD_GET(pAd, pWiphy);

	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s ==>\n", __FUNCTION__));

	RTMP_DRIVER_80211_BEACON_DEL(pAd);
	return 0;
}
#endif	/* LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0) */
#endif /* CONFIG_AP_SUPPORT */

static int CFG80211_OpsChangeBss(
        struct wiphy *pWiphy,
        struct net_device *netdev,
	struct bss_parameters *params)
{
	VOID *pAd;
	CMD_RTPRIV_IOCTL_80211_BSS_PARM bssInfo;

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	MAC80211_PAD_GET(pAd, pWiphy);
	
	bssInfo.use_short_preamble = params->use_short_preamble;
	bssInfo.use_short_slot_time = params->use_short_slot_time;
	bssInfo.use_cts_prot = params->use_cts_prot;
	
	RTMP_DRIVER_80211_CHANGE_BSS_PARM(pAd, &bssInfo);

	return 0;
}

static int CFG80211_OpsStaDel(
	struct wiphy *pWiphy, 
	struct net_device *dev,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
	struct station_del_parameters *params
#else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
	const UINT8 * pMacAddr
#else
	UINT8 * pMacAddr
#endif
#endif

)
{
	VOID *pAd;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
	const u8 *pMacAddr = params->mac;
#endif
	MAC80211_PAD_GET(pAd, pWiphy);

	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	if (pMacAddr ==  NULL)
	{
		RTMP_DRIVER_80211_AP_STA_DEL(pAd, NULL);
	}
	else
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> Delete STA(%02X:%02X:%02X:%02X:%02X:%02X) ==>\n", 
					PRINT_MAC(pMacAddr)));
		CFG80211_ApStaDel(pAd, pMacAddr);
	}

	return 0;
}

static int CFG80211_OpsStaAdd(
        struct wiphy *wiphy,
        struct net_device *dev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
		const UINT8 * mac,
#else
		UINT8 * mac,
#endif
	struct station_parameters *params)
{
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> %s ==>\n", __FUNCTION__));
	return 0;
}

static int CFG80211_OpsStaChg(
	struct wiphy *pWiphy,
	struct net_device *dev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
	const UINT8 * pMacAddr,
#else
	UINT8 * pMacAddr,
#endif
	struct station_parameters *params)
{
	void *pAd;
	CFG80211_CB *p80211CB;
   
	CFG80211DBG(RT_DEBUG_TRACE, ("80211> Change STA(%02X:%02X:%02X:%02X:%02X:%02X) ==>\n", PRINT_MAC(pMacAddr)));
	MAC80211_PAD_GET(pAd, pWiphy);

	p80211CB = NULL;
    RTMP_DRIVER_80211_CB_GET(pAd, &p80211CB);

    if ((dev->ieee80211_ptr->iftype != RT_CMD_80211_IFTYPE_AP) &&
	   (dev->ieee80211_ptr->iftype != RT_CMD_80211_IFTYPE_P2P_GO)
#ifdef CFG_TDLS_SUPPORT		
	    && (dev->ieee80211_ptr->iftype != RT_CMD_80211_IFTYPE_STATION)
#endif /* CFG_TDLS_SUPPORT */
		)
		return -EOPNOTSUPP;
		
	if(!(params->sta_flags_mask & BIT(NL80211_STA_FLAG_AUTHORIZED)))
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> %x ==>\n", params->sta_flags_mask));
		return -EOPNOTSUPP;
	}	

	if (params->sta_flags_set & BIT(NL80211_STA_FLAG_AUTHORIZED))
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> STA(%02X:%02X:%02X:%02X:%02X:%02X) ==> PortSecured\n", 
			PRINT_MAC(pMacAddr)));
		CFG80211_StaPortSecured(pAd, pMacAddr, 1);
	}
	else	
	{
		CFG80211DBG(RT_DEBUG_TRACE, ("80211> STA(%02X:%02X:%02X:%02X:%02X:%02X) ==> PortNotSecured\n", 
                        PRINT_MAC(pMacAddr)));
		CFG80211_StaPortSecured(pAd, pMacAddr, 0);
	}
	return 0;
}

#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 12, 0))
static struct wireless_dev* CFG80211_OpsVirtualInfAdd(
        IN struct wiphy                                 *pWiphy,
        IN const char 									*name,
		IN unsigned char name_assign_type,
        IN enum nl80211_iftype                 			 Type,
        struct vif_params                               *pParams)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
static struct wireless_dev* CFG80211_OpsVirtualInfAdd(
        IN struct wiphy                                 *pWiphy,
        IN const char 									*name,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
							IN unsigned char name_assign_type,
#endif
        IN enum nl80211_iftype                 			 Type,
        IN u32                                          *pFlags,
        struct vif_params                               *pParams)
#else
static struct net_device* CFG80211_OpsVirtualInfAdd(
        IN struct wiphy                                 *pWiphy,
        IN char 										*name,
        IN enum nl80211_iftype                 			 Type,
        IN UINT32                                          *pFlags,
        struct vif_params                               *pParams)
#endif /* LINUX_VERSION_CODE: 3.6.0 */
{
	VOID *pAd;
	CMD_RTPRIV_IOCTL_80211_VIF_SET vifInfo;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	PWIRELESS_DEV pDev = NULL;	
#else	
	PNET_DEV pDev = NULL;
#endif /* LINUX_VERSION_CODE: 3.6.0 */
    	null_MAC80211_PAD_GET(pAd, pWiphy);

	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s [Name:%s, type: %d] ==>\n", __FUNCTION__, name, Type));

	vifInfo.vifType = Type;
	vifInfo.vifNameLen = strlen(name);
    memset(vifInfo.vifName, 0, sizeof(vifInfo.vifName)); 	
	NdisCopyMemory(vifInfo.vifName, name, vifInfo.vifNameLen);

	if (RTMP_DRIVER_80211_VIF_ADD(pAd, &vifInfo) != NDIS_STATUS_SUCCESS)
	{
		CFG80211DBG(RT_DEBUG_OFF, ("80211> %s VIF ADD ERROR.\n", __FUNCTION__));
		//CFG TODO: ERRPTR
		return ERR_PTR(-ENODEV);
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	pDev = RTMP_CFG80211_FindVifEntryWdev_ByType(pAd, Type);
#else	
	/* Return NetDevice */
	pDev = RTMP_CFG80211_FindVifEntry_ByType(pAd, Type);
#endif /* LINUX_VERSION_CODE: 3.6.0 */
	
	return pDev;
}

static int CFG80211_OpsVirtualInfDel(
    IN struct wiphy                                 *pWiphy,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))	
	IN struct wireless_dev *pwdev
#else
	IN struct net_device *dev	
#endif /* LINUX_VERSION_CODE: 3.6.0 */	
	)
{
	void *pAd;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))		
	struct net_device *dev = NULL;
	dev = pwdev->netdev;

	if (!dev)
		return 0;
#endif /* LINUX_VERSION_CODE: 3.6.0 */

	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s, %s [%d]==>\n", __FUNCTION__, dev->name, dev->ieee80211_ptr->iftype));
	MAC80211_PAD_GET(pAd, pWiphy);

	RTMP_DRIVER_80211_VIF_DEL(pAd, dev, dev->ieee80211_ptr->iftype);
	return 0;
}
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */

/*Nobody uses it currently*/
#if 0
static int CFG80211_OpsBitrateSet(
        IN struct wiphy                                 *pWiphy,
	IN struct net_device *dev,
	IN const u8 *peer,
	IN const struct cfg80211_bitrate_mask *mask)
{
	return 0;
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
static int CFG80211_start_p2p_device(
	struct wiphy *pWiphy,
	struct wireless_dev *wdev)
{
	void *pAd;
	struct net_device *dev = wdev->netdev;
	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s, %s [%d]==>\n", __FUNCTION__, dev->name, dev->ieee80211_ptr->iftype));
	MAC80211_PAD_GET(pAd, pWiphy);
	return 0;
}      

static void CFG80211_stop_p2p_device(
	struct wiphy *pWiphy,
	struct wireless_dev *wdev)
{
	void *pAd;
	struct net_device *dev = wdev->netdev;
	CFG80211DBG(RT_DEBUG_OFF, ("80211> %s, %s [%d]==>\n", __FUNCTION__, dev->name, dev->ieee80211_ptr->iftype));
	void_MAC80211_PAD_GET(pAd, pWiphy);
	return;
}
#endif /* LINUX_VERSION_CODE: 3.7.0 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
static const struct ieee80211_txrx_stypes
ralink_mgmt_stypes[NUM_NL80211_IFTYPES] = {
		[NL80211_IFTYPE_STATION] = {
				.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
				BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
				.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
				BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
		},
		[NL80211_IFTYPE_P2P_CLIENT] = {
				.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
				BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
				.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
				BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
		},
        [NL80211_IFTYPE_AP] = {
                        .tx = 0xffff,
                        .rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
                        BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
                        BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
                        BIT(IEEE80211_STYPE_DISASSOC >> 4) |
                        BIT(IEEE80211_STYPE_AUTH >> 4) |
                        BIT(IEEE80211_STYPE_DEAUTH >> 4) |
                        BIT(IEEE80211_STYPE_ACTION >> 4),
        },
		[NL80211_IFTYPE_P2P_GO] = {
				.tx = 0xffff,
				.rx = BIT(IEEE80211_STYPE_ASSOC_REQ >> 4) |
				BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) |
				BIT(IEEE80211_STYPE_PROBE_REQ >> 4) |
				BIT(IEEE80211_STYPE_DISASSOC >> 4) |
				BIT(IEEE80211_STYPE_AUTH >> 4) |
				BIT(IEEE80211_STYPE_DEAUTH >> 4) |
				BIT(IEEE80211_STYPE_ACTION >> 4),				
		},

};
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0))
static const struct ieee80211_iface_limit ra_p2p_sta_go_limits[] =
{
	{
		.max = 3,
		.types = BIT(NL80211_IFTYPE_STATION) |
		         BIT(NL80211_IFTYPE_AP),
	},

	{
		.max = 1,
		.types = BIT(NL80211_IFTYPE_P2P_GO) |
			 BIT(NL80211_IFTYPE_P2P_CLIENT),
	},
};

static const struct ieee80211_iface_combination
ra_iface_combinations_ap_sta[] = {
	{
#ifdef RT_CFG80211_P2P_MULTI_CHAN_SUPPORT
		.num_different_channels = 2,
#else
		.num_different_channels = 1,
#endif /* RT_CFG80211_P2P_MULTI_CHAN_SUPPORT */
		.max_interfaces = 3,
		/* CFG TODO*/
		/* .beacon_int_infra_match = true,*/
		.limits = ra_p2p_sta_go_limits,
		.n_limits = 1,
	},
};

static const struct ieee80211_iface_combination
ra_iface_combinations_p2p[] = {
	{
#ifdef RT_CFG80211_P2P_MULTI_CHAN_SUPPORT
		.num_different_channels = 2,
#else
		.num_different_channels = 1,
#endif /* RT_CFG80211_P2P_MULTI_CHAN_SUPPORT */
		.max_interfaces = 3,
		/* CFG TODO*/
		//.beacon_int_infra_match = true,
		.limits = ra_p2p_sta_go_limits,
		.n_limits = ARRAY_SIZE(ra_p2p_sta_go_limits),
	},
};

const struct ieee80211_iface_combination *p_ra_iface_combinations_ap_sta = ra_iface_combinations_ap_sta;
const INT ra_iface_combinations_ap_sta_num = ARRAY_SIZE(ra_iface_combinations_ap_sta);

const struct ieee80211_iface_combination *p_ra_iface_combinations_p2p = ra_iface_combinations_p2p;
const INT ra_iface_combinations_p2p_num = ARRAY_SIZE(ra_iface_combinations_p2p);

#endif /* LINUX_VERSION_CODE: 3.8.0 */

struct cfg80211_ops CFG80211_Ops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	.suspend = CFG80211_OpsSuspend,
	.set_wakeup = CFG80211_OpsSetWakeup,
	.resume = CFG80211_OpsResume,
	.set_rekey_data	= CFG80211_OpsSetRekeyData,
#endif
#ifdef CFG_TDLS_SUPPORT
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,0))
	.tdls_mgmt = CFG80211_OpsTdlsMgmt,
	.tdls_oper = CFG80211_OpsTdlsOper,	
#endif	
#endif /* CFG_TDLS_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
	.set_beacon	= CFG80211_OpsSetBeacon,
	.add_beacon	= CFG80211_OpsAddBeacon,
	.del_beacon	= CFG80211_OpsDelBeacon,
#else
	.start_ap	    = CFG80211_OpsStartAp,
	.change_beacon	= CFG80211_OpsChangeBeacon,
	.stop_ap	    = CFG80211_OpsStopAp,
#endif	/* LINUX_VERSION_CODE 3.4 */
#endif /* CONFIG_AP_SUPPORT */
	/* set channel for a given wireless interface */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
	//CFG_TODO
	.set_monitor_channel = CFG80211_OpsMonitorChannelSet,
#else
	.set_channel	     = CFG80211_OpsChannelSet,
#endif /* LINUX_VERSION_CODE: 3.6.0 */

	/* change type/configuration of virtual interface */
	.change_virtual_intf		= CFG80211_OpsVirtualInfChg,
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE	
	.add_virtual_intf           = CFG80211_OpsVirtualInfAdd,
	.del_virtual_intf           = CFG80211_OpsVirtualInfDel,
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
	.start_p2p_device = CFG80211_start_p2p_device,
	.stop_p2p_device = CFG80211_stop_p2p_device,
#endif /* LINUX_VERSION_CODE: 3.7.0 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
	/* request to do a scan */
	/*
		Note: must exist whatever AP or STA mode; Or your kernel will crash
		in v2.6.38.
	*/
	.scan						= CFG80211_OpsScan,
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31))
#ifdef CONFIG_STA_SUPPORT
	/* join the specified IBSS (or create if necessary) */
	.join_ibss					= CFG80211_OpsIbssJoin,
	/* leave the IBSS */
	.leave_ibss					= CFG80211_OpsIbssLeave,
#endif /* CONFIG_STA_SUPPORT */
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))
	/* set the transmit power according to the parameters */
	.set_tx_power				= CFG80211_OpsTxPwrSet,
	/* store the current TX power into the dbm variable */
	.get_tx_power				= CFG80211_OpsTxPwrGet,
#endif /* LINUX_VERSION_CODE: 3.8.0 */	
	/* configure WLAN power management */
	.set_power_mgmt				= CFG80211_OpsPwrMgmt,
	/* get station information for the station identified by @mac */
	.get_station				= CFG80211_OpsStaGet,
	/* dump station callback */
	.dump_station				= CFG80211_OpsStaDump,
	/* notify that wiphy parameters have changed */
	.set_wiphy_params			= CFG80211_OpsWiphyParamsSet,
	/* add a key with the given parameters */
	.add_key					= CFG80211_OpsKeyAdd,
	/* get information about the key with the given parameters */
	.get_key					= CFG80211_OpsKeyGet,
	/* remove a key given the @mac_addr */
	.del_key					= CFG80211_OpsKeyDel,
	/* set the default key on an interface */
	.set_default_key			= CFG80211_OpsKeyDefaultSet,
#ifdef DOT11W_PMF_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	.set_default_mgmt_key		= CFG80211_OpsMgmtKeyDefaultSet,
#endif /* LINUX_VERSION_CODE */
#endif /* DOT11W_PMF_SUPPORT */

	/* connect to the ESS with the specified parameters */
	.connect					= CFG80211_OpsConnect,
#ifdef WPA3_SUPPORT
	.auth						= CFG80211_OpsAuth,
	.assoc						= CFG80211_OpsAssoc,
	.deauth						= CFG80211_OpsDeauth,
	.disassoc					= CFG80211_OpsDisassoc,
#endif
	/* disconnect from the BSS/ESS */
	.disconnect					= CFG80211_OpsDisconnect,
#endif /* LINUX_VERSION_CODE */

#ifdef RFKILL_HW_SUPPORT
	/* polls the hw rfkill line */
	.rfkill_poll				= CFG80211_OpsRFKill,
#endif /* RFKILL_HW_SUPPORT */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
	/* get site survey information */
	//.dump_survey				= CFG80211_OpsSurveyGet,
	/* cache a PMKID for a BSSID */
	.set_pmksa					= CFG80211_OpsPmksaSet,
	/* delete a cached PMKID */
	.del_pmksa					= CFG80211_OpsPmksaDel,
	/* flush all cached PMKIDs */
	.flush_pmksa				= CFG80211_OpsPmksaFlush,
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
	/*
		Request the driver to remain awake on the specified
		channel for the specified duration to complete an off-channel
		operation (e.g., public action frame exchange).
	*/
	.remain_on_channel			= CFG80211_OpsRemainOnChannel,
	/* cancel an on-going remain-on-channel operation */
	.cancel_remain_on_channel	=  CFG80211_OpsCancelRemainOnChannel,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37))
	/* transmit an action frame */
	.action						= NULL,
#else 
	.mgmt_tx                    = CFG80211_OpsMgmtTx,
#endif /* LINUX_VERSION_CODE */
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
	 .mgmt_tx_cancel_wait       = CFG80211_OpsTxCancelWait,
#endif /* LINUX_VERSION_CODE */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35))
	/* configure connection quality monitor RSSI threshold */
	.set_cqm_rssi_config		= NULL,
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	/* notify driver that a management frame type was registered */
	.mgmt_frame_register		= CFG80211_OpsMgmtFrameRegister,
#endif /* LINUX_VERSION_CODE : 2.6.37 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
	/* set antenna configuration (tx_ant, rx_ant) on the device */
	.set_antenna				= NULL,
	/* get current antenna configuration from device (tx_ant, rx_ant) */
	.get_antenna				= NULL,
#endif /* LINUX_VERSION_CODE */
	.change_bss                             = CFG80211_OpsChangeBss,
	.del_station                            = CFG80211_OpsStaDel, 
	.add_station                            = CFG80211_OpsStaAdd,
	.change_station                         = CFG80211_OpsStaChg,
//	.set_bitrate_mask                       = CFG80211_OpsBitrateSet,
};

/* =========================== Global Function ============================== */

static INT CFG80211NetdevNotifierEvent(
	struct notifier_block *nb, ULONG state, VOID *ndev)
{
	VOID *pAd;	
	struct net_device *pNev = ndev;
	struct wireless_dev *pWdev = pNev->ieee80211_ptr;
	
	if (!ndev || !pWdev || !pWdev->wiphy)
		return NOTIFY_DONE;
	
   MAC80211_PAD_GET(pAd, pWdev->wiphy);

#if 0 /* fix coverity issue, since MAC80211_PAD_GET is a define, and will just return if pAd == NULL */
	if(!pAd)
		return NOTIFY_DONE;
#endif
	
	switch (state) {
		case NETDEV_UNREGISTER:
			break;
			
		case NETDEV_GOING_DOWN:	
			RTMP_DRIVER_80211_NETDEV_EVENT(pAd, pNev, state);	
			break;
	}

	return NOTIFY_DONE;
}

struct notifier_block cfg80211_netdev_notifier = {
	.notifier_call = CFG80211NetdevNotifierEvent,
};

/*
========================================================================
Routine Description:
	Allocate a wireless device.

Arguments:
	pAd				- WLAN control block pointer
	pDev			- Generic device interface

Return Value:
	wireless device

Note:
========================================================================
*/
static struct wireless_dev *CFG80211_WdevAlloc(
	IN CFG80211_CB					*pCfg80211_CB,
	IN CFG80211_BAND				*pBandInfo,
	IN VOID 						*pAd,
	IN struct device				*pDev)
{
	struct wireless_dev *pWdev;
	ULONG *pPriv;


	/*
	 * We're trying to have the following memory layout:
	 *
	 * +------------------------+
	 * | struct wiphy			|
	 * +------------------------+
	 * | pAd pointer			|
	 * +------------------------+
	 */

	pWdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (pWdev == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Wireless device allocation fail!\n"));
		return NULL;
	} /* End of if */

	pWdev->wiphy = wiphy_new(&CFG80211_Ops, sizeof(ULONG *));
	if (pWdev->wiphy == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Wiphy device allocation fail!\n"));
		goto LabelErrWiphyNew;
	} /* End of if */

	/* keep pAd pointer */
	pPriv = (ULONG *)(wiphy_priv(pWdev->wiphy));
	*pPriv = (ULONG)pAd;

	set_wiphy_dev(pWdev->wiphy, pDev);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
	pWdev->wiphy->max_scan_ssids = (u8)pBandInfo->MaxBssTable;
#endif /* KERNEL_VERSION */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
	/* @NL80211_FEATURE_INACTIVITY_TIMER:
	   This driver takes care of freeingup
	   the connected inactive stations in AP mode.*/

	/*what if you get compile error for below flag, please add the patch into your kernel*/
	/* http://www.permalink.gmane.org/gmane.linux.kernel.wireless.general/86454 */
	pWdev->wiphy->features |= NL80211_FEATURE_INACTIVITY_TIMER;
#endif

#ifdef WPA3_SUPPORT
	pWdev->wiphy->features |= NL80211_FEATURE_SAE;
#endif

	pWdev->wiphy->interface_modes = BIT(NL80211_IFTYPE_AP) | BIT(NL80211_IFTYPE_STATION);

#ifdef CONFIG_STA_SUPPORT
	pWdev->wiphy->interface_modes |= BIT(NL80211_IFTYPE_ADHOC);

#ifdef RT_CFG80211_P2P_SINGLE_DEVICE
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	pWdev->wiphy->interface_modes |= (BIT(NL80211_IFTYPE_P2P_CLIENT)
								    | BIT(NL80211_IFTYPE_P2P_GO));

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
	pWdev->wiphy->software_iftypes |= BIT(NL80211_IFTYPE_P2P_DEVICE);
#endif /* LINUX_VERSION_CODE 3.7.0 */
#endif /* LINUX_VERSION_CODE 2.6.37 */
#endif /* RT_CFG80211_P2P_SINGLE_DEVICE */
#endif /* CONFIG_STA_SUPPORT */

#ifdef RT_CFG80211_P2P_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0))
	pWdev->wiphy->software_iftypes |= (BIT(NL80211_IFTYPE_P2P_CLIENT) | BIT(NL80211_IFTYPE_P2P_GO));

	/* NL80211_IFTYPE_P2P_DEVICE Kernel Symbol start from 3.7 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0))
	pWdev->wiphy->software_iftypes |= BIT(NL80211_IFTYPE_P2P_DEVICE);
#endif /* LINUX_VERSION_CODE 3.7.0 */
#endif /* LINUX_VERSION_CODE 3.0.0 */
#endif /* RT_CFG80211_P2P_SUPPORT */

	//pWdev->wiphy->reg_notifier = CFG80211_RegNotifier;

	/* init channel information */
	CFG80211_SupBandInit(pCfg80211_CB, pBandInfo, pWdev->wiphy, NULL, NULL);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)
	pWdev->wiphy->regulatory_flags |= (REGULATORY_DISABLE_BEACON_HINTS|REGULATORY_CUSTOM_REG);
#endif /* endif */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
	/* CFG80211_SIGNAL_TYPE_MBM: signal strength in mBm (100*dBm) */
	pWdev->wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	pWdev->wiphy->max_scan_ie_len = IEEE80211_MAX_DATA_LEN;	
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
	pWdev->wiphy->max_num_pmkids = 4; 
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
    pWdev->wiphy->max_remain_on_channel_duration = MAX_ROC_TIME;
#endif /* KERNEL_VERSION */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
	pWdev->wiphy->mgmt_stypes = ralink_mgmt_stypes;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
	pWdev->wiphy->cipher_suites = CipherSuites;
	pWdev->wiphy->n_cipher_suites = ARRAY_SIZE(CipherSuites);
#endif /* LINUX_VERSION_CODE */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#ifdef CONFIG_PM
#ifdef MT_WOW_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	pWdev->wiphy->wowlan = &mtk_wowlan_support;
#else
	pWdev->wiphy->wowlan.flags = WIPHY_WOWLAN_MAGIC_PKT;
#endif
#endif
#endif
#endif /* LINUX_VERSION_CODE */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
	pWdev->wiphy->flags |= WIPHY_FLAG_AP_UAPSD;
#endif /* LINUX_VERSION_CODE: 3.2.0 */


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0))
	/*what if you get compile error for below flag, please add the patch into your kernel*/
        /* 018-cfg80211-internal-ap-mlme.patch */
	pWdev->wiphy->flags |= WIPHY_FLAG_HAVE_AP_SME;

	/*what if you get compile error for below flag, please add the patch into your kernel*/
     /* 008-cfg80211-offchan-flags.patch */
	pWdev->wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
	
	//CFG_TODO
	//pWdev->wiphy->flags |= WIPHY_FLAG_STRICT_REGULATORY;	
#endif /* LINUX_VERSION_CODE: 3.3.0 */

	//Driver Report Support TDLS to supplicant
#ifdef CFG_TDLS_SUPPORT
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,1,10))
	pWdev->wiphy->flags |= WIPHY_FLAG_SUPPORTS_TDLS;
	pWdev->wiphy->flags |= WIPHY_FLAG_TDLS_EXTERNAL_SETUP;
#endif /* LINUX_VERSION_CODE: 3.1.10 */
#endif /* CFG_TDLS_SUPPORT */

	/* CFG_TODO */
	//pWdev->wiphy->flags |= WIPHY_FLAG_IBSS_RSN;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
	DBGPRINT(RT_DEBUG_TRACE, ("[%s] Change to AP/STA combination mode!!\n", __func__));
	pWdev->wiphy->iface_combinations = p_ra_iface_combinations_ap_sta;
	pWdev->wiphy->n_iface_combinations = ARRAY_SIZE(ra_iface_combinations_ap_sta);
#endif /* LINUX_VERSION_CODE: 3.8.0 */

	if (wiphy_register(pWdev->wiphy) < 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Register wiphy device fail!\n"));
		goto LabelErrReg;
	} 
		
	return pWdev;

 LabelErrReg:
	wiphy_free(pWdev->wiphy);

 LabelErrWiphyNew:
	os_free_mem(NULL, pWdev);

	return NULL;
} /* End of CFG80211_WdevAlloc */


/*
========================================================================
Routine Description:
	Register MAC80211 Module.

Arguments:
	pAdCB			- WLAN control block pointer
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
	IN VOID						*pAd,
	IN struct device			*pDev,
	IN struct net_device		*pNetDev)
{
	CFG80211_CB *pCfg80211_CB = NULL;
	CFG80211_BAND BandInfo;
	INT err = FALSE;


	/* allocate Main Device Info structure */
	os_alloc_mem(NULL, (UCHAR **)&pCfg80211_CB, sizeof(CFG80211_CB));
	if (pCfg80211_CB == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Allocate MAC80211 CB fail!\n"));
		return FALSE;
	} 
	
	/* allocate wireless device */
	RTMP_DRIVER_80211_BANDINFO_GET(pAd, &BandInfo);

	pCfg80211_CB->pCfg80211_Wdev = \
				CFG80211_WdevAlloc(pCfg80211_CB, &BandInfo, pAd, pDev);
	if (pCfg80211_CB->pCfg80211_Wdev == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("80211> Allocate Wdev fail!\n"));
		os_free_mem(NULL, pCfg80211_CB);
		return FALSE;
	} 

	/* bind wireless device with net device */
#ifdef CONFIG_AP_SUPPORT
	/* default we are AP mode */
	pCfg80211_CB->pCfg80211_Wdev->iftype = NL80211_IFTYPE_AP;
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	/* default we are station mode */
	pCfg80211_CB->pCfg80211_Wdev->iftype = NL80211_IFTYPE_STATION;
#endif /* CONFIG_STA_SUPPORT */

#if 0
	CFG_TODO
	struct ieee80211_channel *ch;
	ch = pCfg80211_CB->pCfg80211_Wdev->wiphy->bands[KAL_BAND_2GHZ]->channels;
	cfg80211_chandef_create(&pCfg80211_CB->pCfg80211_Wdev->preset_chandef, ch, NL80211_CHAN_NO_HT);
#endif 
	
	pNetDev->ieee80211_ptr = pCfg80211_CB->pCfg80211_Wdev;
	SET_NETDEV_DEV(pNetDev, wiphy_dev(pCfg80211_CB->pCfg80211_Wdev->wiphy));
	pCfg80211_CB->pCfg80211_Wdev->netdev = pNetDev;

#ifdef RFKILL_HW_SUPPORT
	wiphy_rfkill_start_polling(pCfg80211_CB->pCfg80211_Wdev->wiphy);
#endif /* RFKILL_HW_SUPPORT */

	RTMP_DRIVER_80211_CB_SET(pAd, pCfg80211_CB);
	RTMP_DRIVER_80211_RESET(pAd);
	RTMP_DRIVER_80211_SCAN_STATUS_LOCK_INIT(pAd, TRUE);	

	//CFG TODO
	//err = register_netdevice_notifier(&cfg80211_netdev_notifier);
	if (err) 
	{
		CFG80211DBG(RT_DEBUG_ERROR, ("80211> Failed to register notifierl %d\n", err));
	}

	CFG80211DBG(RT_DEBUG_ERROR, ("80211> CFG80211_Register\n"));
	return TRUE;
} /* End of CFG80211_Register */




/* =========================== Local Function =============================== */

/*
========================================================================
Routine Description:
	The driver's regulatory notification callback.

Arguments:
	pWiphy			- Wireless hardware description
	pRequest		- Regulatory request

Return Value:
	0

Note:
========================================================================
*/
/*Nobody uses it currently*/
#if 0

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
static INT32 CFG80211_RegNotifier(
	IN struct wiphy					*pWiphy,
	IN struct regulatory_request	*pRequest)
{
	VOID *pAd;
	ULONG *pPriv;


	/* sanity check */
	pPriv = (ULONG *)(wiphy_priv(pWiphy));
	pAd = (VOID *)(*pPriv);

	if (pAd == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("crda> reg notify but pAd = NULL!"));
		return 0;
	} /* End of if */

	/*
		Change the band settings (PASS scan, IBSS allow, or DFS) in mac80211
		based on EEPROM.

		IEEE80211_CHAN_DISABLED: This channel is disabled.
		IEEE80211_CHAN_PASSIVE_SCAN: Only passive scanning is permitted
					on this channel.
		IEEE80211_CHAN_NO_IBSS: IBSS is not allowed on this channel.
		IEEE80211_CHAN_RADAR: Radar detection is required on this channel.
		IEEE80211_CHAN_NO_FAT_ABOVE: extension channel above this channel
					is not permitted.
		IEEE80211_CHAN_NO_FAT_BELOW: extension channel below this channel
					is not permitted.
	*/
#ifdef RELEASE_EXCLUDE
	if (pWiphy->bands[KAL_BAND_5GHZ])
	{
		struct ieee80211_supported_band *pBand;
		struct ieee80211_channel *pChannel;
		UINT32 IdChan;


		/* RALINK follows DFS rule from upper layer */

		/*
			Atheros Communications Inc. driver/net/wireless/ath9k/regd.c

			Always apply Radar/DFS rules on freq range 5260 MHz - 5700 MHz

			We always enable radar detection/DFS on this frequency range.
			Additionally we also apply on this frequency range:

			- If STA mode does not yet have DFS supports disable active scanning
			- If adhoc mode does not support DFS yet then disable adhoc in the
				frequency.
			- If AP mode does not yet support radar detection/DFS do not allow
				AP mode.

			Step1: Loop all channels of 5GHZ and check if the channel needs DFS
					based on settings in EEPROM.
			Step2: If yes and the channel is not disabled,
					pChannel->flags |=	IEEE80211_CHAN_RADAR |
										IEEE80211_CHAN_NO_IBSS |
										IEEE80211_CHAN_PASSIVE_SCAN;
		 */
		pBand = pWiphy->bands[KAL_BAND_5GHZ];
	
		for(IdChan=0; IdChan<pBand->n_channels; IdChan++)
		{
			pChannel = &pBand->channels[IdChan];

			/* TODO: add DFS */
		}
	} /* End of if */
#endif /* RELEASE_EXCLUDE */

	/*
		Change regulatory rule here.

		struct ieee80211_channel {
			enum ieee80211_band band;
			UINT16 center_freq;
			u8 max_bandwidth;
			UINT16 hw_value;
			UINT32 flags;
			int max_antenna_gain;
			int max_power;
			bool beacon_found;
			UINT32 orig_flags;
			int orig_mag, orig_mpwr;
		};

		In mac80211 layer, it will change flags, max_antenna_gain,
		max_bandwidth, max_power.
	*/

	switch(pRequest->initiator)
	{
		case NL80211_REGDOM_SET_BY_CORE:
			/*
				Core queried CRDA for a dynamic world regulatory domain.
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by core: "));
			break;

		case NL80211_REGDOM_SET_BY_USER:
			/*
				User asked the wireless core to set the regulatory domain.
				(when iw, network manager, wpa supplicant, etc.)
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by user: "));
			break;

		case NL80211_REGDOM_SET_BY_DRIVER:
			/*
				A wireless drivers has hinted to the wireless core it thinks
				its knows the regulatory domain we should be in.
				(when driver initialization, calling regulatory_hint)
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by driver: "));
			break;

		case NL80211_REGDOM_SET_BY_COUNTRY_IE:
			/*
				The wireless core has received an 802.11 country information
				element with regulatory information it thinks we should consider.
				(when beacon receive, calling regulatory_hint_11d)
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by country IE: "));
			break;
	} /* End of switch */

	CFG80211DBG(RT_DEBUG_OFF, ("%c%c\n", pRequest->alpha2[0], pRequest->alpha2[1]));

	/* only follow rules from user */
	if (pRequest->initiator == NL80211_REGDOM_SET_BY_USER)
	{
		/* keep Alpha2 and we can re-call the function when interface is up */
		CMD_RTPRIV_IOCTL_80211_REG_NOTIFY RegInfo;

		RegInfo.Alpha2[0] = pRequest->alpha2[0];
		RegInfo.Alpha2[1] = pRequest->alpha2[1];
		RegInfo.pWiphy = pWiphy;

		RTMP_DRIVER_80211_REG_NOTIFY(pAd, &RegInfo);
	} /* End of if */

	return 0;
} /* End of CFG80211_RegNotifier */

#else

static INT32 CFG80211_RegNotifier(
	IN struct wiphy					*pWiphy,
	IN enum reg_set_by				Request)
{
	struct device *pDev = pWiphy->dev.parent;
	struct net_device *pNetDev = dev_get_drvdata(pDev);
	VOID *pAd = (VOID *)RTMP_OS_NETDEV_GET_PRIV(pNetDev);
	UINT32 ReqType = Request;


	/* sanity check */
	if (pAd == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("crda> reg notify but pAd = NULL!"));
		return 0;
	} /* End of if */

	/*
		Change the band settings (PASS scan, IBSS allow, or DFS) in mac80211
		based on EEPROM.

		IEEE80211_CHAN_DISABLED: This channel is disabled.
		IEEE80211_CHAN_PASSIVE_SCAN: Only passive scanning is permitted
					on this channel.
		IEEE80211_CHAN_NO_IBSS: IBSS is not allowed on this channel.
		IEEE80211_CHAN_RADAR: Radar detection is required on this channel.
		IEEE80211_CHAN_NO_FAT_ABOVE: extension channel above this channel
					is not permitted.
		IEEE80211_CHAN_NO_FAT_BELOW: extension channel below this channel
					is not permitted.
	*/
#ifdef RELEASE_EXCLUDE
	if (pWiphy->bands[KAL_BAND_5GHZ])
	{
		struct ieee80211_supported_band *pBand;
		struct ieee80211_channel *pChannel;
		UINT32 IdChan;


		/* RALINK follows DFS rule from upper layer */

		/*
			Atheros Communications Inc. driver/net/wireless/ath9k/regd.c

			Always apply Radar/DFS rules on freq range 5260 MHz - 5700 MHz

			We always enable radar detection/DFS on this frequency range.
			Additionally we also apply on this frequency range:

			- If STA mode does not yet have DFS supports disable active scanning
			- If adhoc mode does not support DFS yet then disable adhoc in the
				frequency.
			- If AP mode does not yet support radar detection/DFS do not allow
				AP mode.

			Step1: Loop all channels of 5GHZ and check if the channel needs DFS
					based on settings in EEPROM.
			Step2: If yes and the channel is not disabled,
					pChannel->flags |=	IEEE80211_CHAN_RADAR |
										IEEE80211_CHAN_NO_IBSS |
										IEEE80211_CHAN_PASSIVE_SCAN;
		 */
		pBand = pWiphy->bands[KAL_BAND_5GHZ];
	
		for(IdChan=0; IdChan<pBand->n_channels; IdChan++)
		{
			pChannel = &pBand->channels[IdChan];

			/* TODO: add DFS */
		}
	} /* End of if */
#endif /* RELEASE_EXCLUDE */

	/*
		Change regulatory rule here.

		struct ieee80211_channel {
			enum ieee80211_band band;
			UINT16 center_freq;
			u8 max_bandwidth;
			UINT16 hw_value;
			UINT32 flags;
			int max_antenna_gain;
			int max_power;
			bool beacon_found;
			UINT32 orig_flags;
			int orig_mag, orig_mpwr;
		};

		In mac80211 layer, it will change flags, max_antenna_gain,
		max_bandwidth, max_power.
	*/

	switch(ReqType)
	{
		case REGDOM_SET_BY_CORE:
			/*
				Core queried CRDA for a dynamic world regulatory domain.
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by core: "));
			break;

		case REGDOM_SET_BY_USER:
			/*
				User asked the wireless core to set the regulatory domain.
				(when iw, network manager, wpa supplicant, etc.)
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by user: "));
			break;

		case REGDOM_SET_BY_DRIVER:
			/*
				A wireless drivers has hinted to the wireless core it thinks
				its knows the regulatory domain we should be in.
				(when driver initialization, calling regulatory_hint)
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by driver: "));
			break;

		case REGDOM_SET_BY_COUNTRY_IE:
			/*
				The wireless core has received an 802.11 country information
				element with regulatory information it thinks we should consider.
				(when beacon receive, calling regulatory_hint_11d)
			*/
			CFG80211DBG(RT_DEBUG_ERROR, ("crda> requlation requestion by country IE: "));
			break;
	} /* End of switch */

	DBGPRINT(RT_DEBUG_OFF, ("00\n"));

	/* only follow rules from user */
	if (ReqType == REGDOM_SET_BY_USER)
	{
		/* keep Alpha2 and we can re-call the function when interface is up */
		CMD_RTPRIV_IOCTL_80211_REG_NOTIFY RegInfo;

		RegInfo.Alpha2[0] = '0';
		RegInfo.Alpha2[1] = '0';
		RegInfo.pWiphy = pWiphy;

		RTMP_DRIVER_80211_REG_NOTIFY(pAd, &RegInfo);
	} /* End of if */

	return 0;
} /* End of CFG80211_RegNotifier */
#endif /* LINUX_VERSION_CODE */
#endif


#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX_VERSION_CODE */

/* End of crda.c */
