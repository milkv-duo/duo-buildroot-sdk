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
	ap_ioctl.c

    Abstract:
    IOCTL related subroutines

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
*/
#define RTMP_MODULE_OS

/*#include "rt_config.h" */
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"
#include <linux/wireless.h>

struct iw_priv_args ap_privtab[] = {
{ RTPRIV_IOCTL_SET, 
/* 1024 --> 1024 + 512 */
/* larger size specific to allow 64 ACL MAC addresses to be set up all at once. */
  IW_PRIV_TYPE_CHAR | 1536, 0,
  "set"},  
{ RTPRIV_IOCTL_SHOW,
  IW_PRIV_TYPE_CHAR | 1024, 0,
  "show"},
{ RTPRIV_IOCTL_GSITESURVEY,
  0, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_site_survey"}, 
#ifdef INF_AR9
  { RTPRIV_IOCTL_GET_AR9_SHOW,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "ar9_show"}, 
#endif
  { RTPRIV_IOCTL_SET_WSCOOB,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "set_wsc_oob"}, 
{ RTPRIV_IOCTL_GET_MAC_TABLE,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_mac_table"}, 
{ RTPRIV_IOCTL_E2P,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "e2p"},
#if defined(DBG) ||(defined(BB_SOC)&&defined(CONFIG_ATE))
{ RTPRIV_IOCTL_BBP,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "bbp"},
{ RTPRIV_IOCTL_MAC,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "mac"},
#ifdef RTMP_RF_RW_SUPPORT
{ RTPRIV_IOCTL_RF,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "rf"},
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* defined(DBG) ||(defined(BB_SOC)&&defined(CONFIG_ATE)) */

#ifdef WSC_AP_SUPPORT
{ RTPRIV_IOCTL_WSC_PROFILE,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_wsc_profile"},
#endif /* WSC_AP_SUPPORT */
{ RTPRIV_IOCTL_QUERY_BATABLE,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024 ,
  "get_ba_table"},
{ RTPRIV_IOCTL_STATISTICS,
  IW_PRIV_TYPE_CHAR | 1024, IW_PRIV_TYPE_CHAR | 1024,
  "stat"}
};


#ifdef CONFIG_APSTA_MIXED_SUPPORT
const struct iw_handler_def rt28xx_ap_iw_handler_def =
{
#define	N(a)	(sizeof (a) / sizeof (a[0]))
	.private_args	= (struct iw_priv_args *) ap_privtab,
	.num_private_args	= N(ap_privtab),
#if IW_HANDLER_VERSION >= 7
	.get_wireless_stats = rt28xx_get_wireless_stats,
#endif 
};
#endif /* CONFIG_APSTA_MIXED_SUPPORT */


INT rt28xx_ap_ioctl(struct net_device *net_dev, struct ifreq *rq, int cmd)
{
	VOID			*pAd = NULL;
    struct iwreq	*wrqin = (struct iwreq *) rq;
	RTMP_IOCTL_INPUT_STRUCT rt_wrq, *wrq = &rt_wrq;
    INT				Status = NDIS_STATUS_SUCCESS;
    USHORT			subcmd;
	INT			apidx=0;
	UINT32		org_len;
	RT_CMD_AP_IOCTL_CONFIG IoctlConfig, *pIoctlConfig = &IoctlConfig;

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

	pIoctlConfig->Status = 0;
	pIoctlConfig->net_dev = net_dev;
	pIoctlConfig->wdev = RTMP_OS_NETDEV_GET_WDEV(net_dev);
	pIoctlConfig->priv_flags = RT_DEV_PRIV_FLAGS_GET(net_dev);
	pIoctlConfig->pCmdData = wrqin->u.data.pointer;
	pIoctlConfig->CmdId_RTPRIV_IOCTL_SET = RTPRIV_IOCTL_SET;
	pIoctlConfig->name = net_dev->name;
	pIoctlConfig->apidx = 0;

#ifdef E2P_WITHOUT_FW_SUPPORT
        if (cmd == RTPRIV_IOCTL_E2P)
                goto skip_check;
#endif /* E2P_WITHOUT_FW_SUPPORT */

	if ((cmd != SIOCGIWPRIV) &&
		RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_PREPARE, 0,
							pIoctlConfig, 0) != NDIS_STATUS_SUCCESS)
	{
		/* prepare error */
		Status = pIoctlConfig->Status;
		goto LabelExit;
	}

	apidx = pIoctlConfig->apidx;
	
    /*+ patch for SnapGear Request even the interface is down */
    if(cmd== SIOCGIWNAME){
	    DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWNAME\n"));

	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_SIOCGIWNAME, 0, wrqin->u.name, 0);
	    return Status;
    }/*- patch for SnapGear */

#if 0 /* os abl move */
    if((RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_MAIN) && !RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
    {
#ifdef CONFIG_APSTA_MIXED_SUPPORT
	if (wrqin->u.data.pointer == NULL)
		return Status;

	if (cmd == RTPRIV_IOCTL_SET)
	{
		if (strstr(wrqin->u.data.pointer, "OpMode") == NULL)
			return -ENETDOWN;
	}
	else
#endif /* CONFIG_APSTA_MIXED_SUPPORT */
		return -ENETDOWN;
    }

    /* determine this ioctl command is comming from which interface. */
    if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_MAIN)
    {
		pObj->ioctl_if_type = INT_MAIN;
        pObj->ioctl_if = MAIN_MBSSID;
/*        DBGPRINT(RT_DEBUG_INFO, ("%s() I/F(ra%d)(flags=%d): cmd = 0x%08x\n", __FUNCTION__, pObj->ioctl_if, RT_DEV_PRIV_FLAGS_GET(net_dev), cmd)); */
    }
    else if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_MBSSID)
    {
		pObj->ioctl_if_type = INT_MBSSID;
/*    	if (!RTMPEqualMemory(net_dev->name, pAd->net_dev->name, 3))  // for multi-physical card, no MBSSID */
		if (strcmp(net_dev->name, pAd->net_dev->name) != 0) /* sample */
    	{
	        for (index = 1; index < pAd->ApCfg.BssidNum; index++)
	    	{
	    	    if (pAd->ApCfg.MBSSID[index].MSSIDDev == net_dev)
	    	    {
	    	        pObj->ioctl_if = index;
	    	        
/*	    	        DBGPRINT(RT_DEBUG_INFO, ("%s(): I/F(ra%d)(flags=%d): cmd = 0x%08x\n", __FUNCTION__, index, RT_DEV_PRIV_FLAGS_GET(net_dev), cmd)); */
	    	        break;
	    	    }
	    	}
	        /* Interface not found! */
	        if(index == pAd->ApCfg.BssidNum)
	        {
/*	        	DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find I/F\n", __FUNCTION__)); */
	            return -ENETDOWN;
	        }
	    }
	    else    /* ioctl command from I/F(ra0) */
	    {
			GET_PAD_FROM_NET_DEV(pAd, net_dev);	
    	    pObj->ioctl_if = MAIN_MBSSID;
/*	        DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find I/F and use default: cmd = 0x%08x\n", __FUNCTION__, cmd)); */
	    }
        MBSS_MR_APIDX_SANITY_CHECK(pAd, pObj->ioctl_if);
        apidx = pObj->ioctl_if;
    }
#ifdef WDS_SUPPORT
	else if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_WDS)
	{
		pObj->ioctl_if_type = INT_WDS;
		for(index = 0; index < MAX_WDS_ENTRY; index++)
		{
			if (pAd->WdsTab.WdsEntry[index].dev == net_dev)
			{
				pObj->ioctl_if = index;

#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("%s(): I/F(wds%d)(flags=%x): cmd = 0x%08x\n", __FUNCTION__, pObj->ioctl_if, RT_DEV_PRIV_FLAGS_GET(net_dev), cmd));
#endif /* RELEASE_EXCLUDE */
				break;
			}
			
			if(index == MAX_WDS_ENTRY)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find wds I/F\n", __FUNCTION__));
				return -ENETDOWN;
			}
		}
	}
#endif /* WDS_SUPPORT */
#ifdef APCLI_SUPPORT
	else if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_APCLI)
	{
		pObj->ioctl_if_type = INT_APCLI;
		for (index = 0; index < MAX_APCLI_NUM; index++)
		{
			if (pAd->ApCfg.ApCliTab[index].dev == net_dev)
			{
				pObj->ioctl_if = index;

#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("%s(): I/F(apcli%d)(flags=%x): cmd = 0x%08x\n", __FUNCTION__, pObj->ioctl_if, RT_DEV_PRIV_FLAGS_GET(net_dev), cmd));
#endif /* RELEASE_EXCLUDE */
				break;
			}

			if(index == MAX_APCLI_NUM)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find Apcli I/F\n", __FUNCTION__));
				return -ENETDOWN;
			}
		}
		APCLI_MR_APIDX_SANITY_CHECK(pObj->ioctl_if);
	}
#endif /* APCLI_SUPPORT */
#ifdef MESH_SUPPORT
	else if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_MESH)
	{
		pObj->ioctl_if_type = INT_MESH;
		pObj->ioctl_if = 0;
	}
#endif /* MESH_SUPPORT */
#ifdef P2P_SUPPORT
	else if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_P2P)
	{
		pObj->ioctl_if_type = INT_P2P;
		pObj->ioctl_if = MAIN_MBSSID;
		apidx = MAIN_MBSSID;
	}
#endif /* P2P_SUPPORT */
    else
    {
/*    	DBGPRINT(RT_DEBUG_WARN, ("IOCTL is not supported in WDS interface\n")); */
    	return -EOPNOTSUPP;
    }
#endif /* 0 */

#ifdef E2P_WITHOUT_FW_SUPPORT
skip_check:
#endif /* E2P_WITHOUT_FW_SUPPORT */ 

	switch(cmd)
	{
		case RTPRIV_IOCTL_ATE:
			{
				RTMP_COM_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_ATE, 0, wrqin->ifr_name, 0);
			}
			break;

        case SIOCGIFHWADDR:
			DBGPRINT(RT_DEBUG_TRACE, ("IOCTLIOCTLIOCTL::SIOCGIFHWADDR\n"));
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_AP_SIOCGIFHWADDR, 0, NULL, 0);
/*            if (pObj->ioctl_if < MAX_MBSSID_NUM(pAd)) */
/*    			strcpy((RTMP_STRING *) wrq->u.name, (RTMP_STRING *) pAd->ApCfg.MBSSID[pObj->ioctl_if].Bssid); */
			break;
#if 0
		case SIOCGIWNAME:
			DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWNAME\n"));
			strcpy(wrqin->u.name, "RTWIFI SoftAP");
			break;
#endif /* 0 */
		case SIOCSIWESSID:  /*Set ESSID */
#if 0
#ifdef APCLI_SUPPORT
#ifdef ApCli_8021X_SUPPORT
			if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_APCLI)
			{
        			struct iw_point	*essid=&wrq->u.essid;
        			rt_apcli_ioctl_siwessid(net_dev, NULL, essid, essid->pointer);
			}
			else
				Status = -EOPNOTSUPP;
#else
			Status = -EOPNOTSUPP;
#endif/*ApCli_8021X_SUPPORT*/
#endif/*APCLI_SUPPORT*/
#endif
			break;
		case SIOCGIWESSID:  /*Get ESSID */
			{
				RT_CMD_AP_IOCTL_SSID IoctlSSID, *pIoctlSSID = &IoctlSSID;
				struct iw_point *erq = &wrqin->u.essid;
				PCHAR pSsidStr = NULL;

				erq->flags=1;
              /*erq->length = pAd->ApCfg.MBSSID[pObj->ioctl_if].SsidLen; */

				pIoctlSSID->priv_flags = RT_DEV_PRIV_FLAGS_GET(net_dev);
				pIoctlSSID->apidx = apidx;
				RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_AP_SIOCGIWESSID, 0, pIoctlSSID, 0);

				pSsidStr = (PCHAR)pIoctlSSID->pSsidStr;
				erq->length = (USHORT)pIoctlSSID->length;

#if 0 /* os abl move */
#ifdef APCLI_SUPPORT
				if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_APCLI)
				{
					if (pAd->ApCfg.ApCliTab[pObj->ioctl_if].Valid == TRUE)
					{
						erq->length = pAd->ApCfg.ApCliTab[pObj->ioctl_if].SsidLen;
						pSsidStr = (PCHAR)&pAd->ApCfg.ApCliTab[pObj->ioctl_if].Ssid;
					}
					else {
						erq->length = 0;
						pSsidStr = NULL;
					}
				}
				else
#endif /* APCLI_SUPPORT */
				{
				erq->length = pAd->ApCfg.MBSSID[apidx].SsidLen;
					pSsidStr = (PCHAR)pAd->ApCfg.MBSSID[apidx].Ssid;
				}
#endif /* 0 */

				if((erq->pointer) && (pSsidStr != NULL))
				{
					/*if(copy_to_user(erq->pointer, pAd->ApCfg.MBSSID[pObj->ioctl_if].Ssid, erq->length)) */
					if(copy_to_user(erq->pointer, pSsidStr, erq->length))
					{
						Status = RTMP_IO_EFAULT;
						break;
					}
				}
				DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWESSID (Len=%d, ssid=%s...)\n", erq->length, (char *)erq->pointer));
			}
			break;
		case SIOCGIWNWID: /* get network id */
		case SIOCSIWNWID: /* set network id (the cell) */
			Status = RTMP_IO_EOPNOTSUPP;
			break;
		case SIOCGIWFREQ: /* get channel/frequency (Hz) */
		{
			ULONG Channel;
			RTMP_DRIVER_CHANNEL_GET(pAd, &Channel);
			wrqin->u.freq.m = Channel; /*pAd->CommonCfg.Channel; */
			wrqin->u.freq.e = 0;
			wrqin->u.freq.i = 0;
		}
			break; 
		case SIOCSIWFREQ: /*set channel/frequency (Hz) */
			Status = RTMP_IO_EOPNOTSUPP;
			break;
		case SIOCGIWNICKN:
		case SIOCSIWNICKN: /*set node name/nickname */
			Status = RTMP_IO_EOPNOTSUPP;
			break;
		case SIOCGIWRATE:  /*get default bit rate (bps) */
            {
				RT_CMD_IOCTL_RATE IoctlRate, *pIoctlRate = &IoctlRate;

				pIoctlRate->priv_flags = RT_DEV_PRIV_FLAGS_GET(net_dev);
				RTMP_DRIVER_BITRATE_GET(pAd, pIoctlRate);

#if 0
                int rate_index = 0;
                INT32 ralinkrate[256] =
					{2,  4, 11, 22, 12, 18,   24,  36, 48, 72, 96, 108, 109, 110, 111, 112,
					13, 26,   39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260,
					39, 78,  117, 156, 234, 312, 351, 390,
					27, 54,   81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
					81, 162, 243, 324, 486, 648, 729, 810,
					14, 29,   43,  57,  87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288,
					43, 87,  130, 173, 260, 317, 390, 433,
					30, 60,   90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
					90, 180, 270, 360, 540, 720, 810, 900,
					0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
					20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
					40,41,42,43,44,45,46,47}; /* 3*3 */
#endif

#if 0 /* os abl move */
				HTTRANSMIT_SETTING		HtPhyMode;

#ifdef MESH_SUPPORT
				if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_MESH)
					HtPhyMode = pAd->MeshTab.HTPhyMode;
				else
#endif /* MESH_SUPPORT */
#ifdef APCLI_SUPPORT
#ifdef P2P_SUPPORT
				if ((RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_P2P) && P2P_CLI_ON(pAd))
#else
				if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_APCLI)
#endif /* P2P_SUPPORT */
					HtPhyMode = pAd->ApCfg.ApCliTab[pObj->ioctl_if].HTPhyMode;
				else
#endif /* APCLI_SUPPORT */
#ifdef WDS_SUPPORT
				if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_WDS)
					HtPhyMode = pAd->WdsTab.WdsEntry[pObj->ioctl_if].HTPhyMode;
				else
#endif /* WDS_SUPPORT */
					HtPhyMode = pAd->ApCfg.MBSSID[pObj->ioctl_if].HTPhyMode;

#ifdef MBSS_SUPPORT
				/* reset phy mode for MBSS */
				MBSS_PHY_MODE_RESET(pObj->ioctl_if, HtPhyMode);
#endif /* MBSS_SUPPORT */

#if 0
#ifdef DOT11_N_SUPPORT
                if (pHtPhyMode->field.MODE >= MODE_HTMIX)
                {
/*                	rate_index = 16 + ((UCHAR)pHtPhyMode->field.BW *16) + ((UCHAR)pHtPhyMode->field.ShortGI *32) + ((UCHAR)pHtPhyMode->field.MCS); */
                	rate_index = 16 + ((UCHAR)pHtPhyMode->field.BW *24) + ((UCHAR)pHtPhyMode->field.ShortGI *48) + ((UCHAR)pHtPhyMode->field.MCS);
                }
                else 
#endif /* DOT11_N_SUPPORT */
		  if (pHtPhyMode->field.MODE == MODE_OFDM)
                	rate_index = (UCHAR)(pHtPhyMode->field.MCS) + 4;
                else 
                	rate_index = (UCHAR)(pHtPhyMode->field.MCS);

                if (rate_index < 0)
                    rate_index = 0;

                if (rate_index > 255)
                    rate_index = 255;
    
			    wrqin->u.bitrate.value = ralinkrate[rate_index] * 500000;
#else

			RtmpDrvMaxRateGet(pAd, HtPhyMode.field.MODE, HtPhyMode.field.ShortGI,
							HtPhyMode.field.BW, HtPhyMode.field.MCS,
							(UINT32 *)&wrqin->u.bitrate.value);
#endif
#endif /* 0 */
			wrqin->u.bitrate.value = pIoctlRate->BitRate;
			wrqin->u.bitrate.disabled = 0;
            }
			break;
		case SIOCSIWRATE:  /*set default bit rate (bps) */
		case SIOCGIWRTS:  /* get RTS/CTS threshold (bytes) */
		case SIOCSIWRTS:  /*set RTS/CTS threshold (bytes) */
		case SIOCGIWFRAG:  /*get fragmentation thr (bytes) */
		case SIOCSIWFRAG:  /*set fragmentation thr (bytes) */
		case SIOCGIWENCODE:  /*get encoding token & mode */
		case SIOCSIWENCODE:  /*set encoding token & mode */
			Status = RTMP_IO_EOPNOTSUPP;
			break;
		case SIOCGIWAP:  /*get access point MAC addresses */
			{
/*				PCHAR pBssidStr; */

				wrqin->u.ap_addr.sa_family = ARPHRD_ETHER;
				/*memcpy(wrqin->u.ap_addr.sa_data, &pAd->ApCfg.MBSSID[pObj->ioctl_if].Bssid, ETH_ALEN); */

				RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_AP_SIOCGIWAP, 0,
								wrqin->u.ap_addr.sa_data, RT_DEV_PRIV_FLAGS_GET(net_dev));
#if 0 /* os abl move */
#ifdef APCLI_SUPPORT
				if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_APCLI)
				{
					if (pAd->ApCfg.ApCliTab[pObj->ioctl_if].Valid == TRUE)
						pBssidStr = (PCHAR)&APCLI_ROOT_BSSID_GET(pAd, pAd->ApCfg.ApCliTab[pObj->ioctl_if].MacTabWCID);
					else
						pBssidStr = NULL;
				}
				else
#endif /* APCLI_SUPPORT */
#ifdef P2P_SUPPORT
				if ((RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_P2P) && P2P_CLI_ON(pAd))
				{
					if (pAd->ApCfg.ApCliTab[pObj->ioctl_if].Valid == TRUE)
						pBssidStr = (PCHAR)&APCLI_ROOT_BSSID_GET(pAd, pAd->ApCfg.ApCliTab[pObj->ioctl_if].MacTabWCID);
					else
						pBssidStr = NULL;
				}
				else
#endif /* P2P_SUPPORT */
				{
					pBssidStr = (PCHAR) &pAd->ApCfg.MBSSID[pObj->ioctl_if].Bssid[0];
				}

				if (pBssidStr != NULL)
				{
					memcpy(wrqin->u.ap_addr.sa_data, pBssidStr, ETH_ALEN);
					DBGPRINT(RT_DEBUG_TRACE, ("IOCTL::SIOCGIWAP(=%02x:%02x:%02x:%02x:%02x:%02x)\n",
						pBssidStr[0],pBssidStr[1],pBssidStr[2], pBssidStr[3],pBssidStr[4],pBssidStr[5]));
				}
				else
				{
					memset(wrqin->u.ap_addr.sa_data, 0, ETH_ALEN);
				}
#endif /* 0 */
			}
			break;
		case SIOCGIWMODE:  /*get operation mode */
			wrqin->u.mode = IW_MODE_INFRA;   /*SoftAP always on INFRA mode. */
			break;
		case SIOCSIWAP:  /*set access point MAC addresses */
		case SIOCSIWMODE:  /*set operation mode */
		case SIOCGIWSENS:   /*get sensitivity (dBm) */
		case SIOCSIWSENS:	/*set sensitivity (dBm) */
		case SIOCGIWPOWER:  /*get Power Management settings */
		case SIOCSIWPOWER:  /*set Power Management settings */
		case SIOCGIWTXPOW:  /*get transmit power (dBm) */
		case SIOCSIWTXPOW:  /*set transmit power (dBm) */
		/*case SIOCGIWRANGE:	//Get range of parameters */
		case SIOCGIWRETRY:	/*get retry limits and lifetime */
		case SIOCSIWRETRY:	/*set retry limits and lifetime */
			Status = RTMP_IO_EOPNOTSUPP;
			break;
		case SIOCGIWRANGE:	/*Get range of parameters */
		    {
/*				struct iw_range range; */
				struct iw_range *prange = NULL;
				ULONG len;

				/* allocate memory */
				os_alloc_mem(NULL, (UCHAR **)&prange, sizeof(struct iw_range));
				if (prange == NULL)
				{
					DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory fail!!!\n", __FUNCTION__));
					break;
				}

				memset(prange, 0, sizeof(struct iw_range));
				prange->we_version_compiled = WIRELESS_EXT;
				prange->we_version_source = 14;

				/*
					what is correct max? This was not
					documented exactly. At least
					69 has been observed.
				*/
				prange->max_qual.qual = 100;
				prange->max_qual.level = 0; /* dB */
				prange->max_qual.noise = 0; /* dB */
				len = copy_to_user(wrq->u.data.pointer, prange, sizeof(struct iw_range));
				os_free_mem(NULL, prange);
		    }
		    break;
		    
		case RT_PRIV_IOCTL:
		case RT_PRIV_IOCTL_EXT:
		{
			subcmd = wrqin->u.data.flags;

			Status = RTMP_AP_IoctlHandle(pAd, wrq, CMD_RT_PRIV_IOCTL, subcmd, wrqin->u.data.pointer, 0);
		}
			break;
		
#ifdef HOSTAPD_SUPPORT
		case SIOCSIWGENIE:
			DBGPRINT(RT_DEBUG_TRACE,("ioctl SIOCSIWGENIE apidx=%d\n",apidx));
			DBGPRINT(RT_DEBUG_TRACE,("ioctl SIOCSIWGENIE length=%d, pointer=%x\n", wrqin->u.data.length, wrqin->u.data.pointer));

#if 0 /* os abl move */
			if(wrqin->u.data.length > 20 && MAX_LEN_OF_RSNIE > wrqin->u.data.length && wrqin->u.data.pointer)
			{
				UCHAR RSNIE_Len[2];
				UCHAR RSNIe[2];
				int offset_next_ie=0;

				DBGPRINT(RT_DEBUG_TRACE,("ioctl SIOCSIWGENIE pAd->IoctlIF=%d\n",apidx));

				RSNIe[0]=*(UINT8 *)wrqin->u.data.pointer;
				if(IE_WPA != RSNIe[0] && IE_RSN != RSNIe[0] )
				{
					DBGPRINT(RT_DEBUG_TRACE,("IE %02x != 0x30/0xdd\n",RSNIe[0]));
					Status = -EINVAL;
					break;
				}
				RSNIE_Len[0]=*((UINT8 *)wrqin->u.data.pointer + 1);
				if(wrqin->u.data.length != RSNIE_Len[0]+2)
				{
					DBGPRINT(RT_DEBUG_TRACE,("IE use WPA1 WPA2\n"));
					NdisZeroMemory(pAd->ApCfg.MBSSID[apidx].RSN_IE[1], MAX_LEN_OF_RSNIE);
					RSNIe[1]=*(UINT8 *)wrqin->u.data.pointer;
					RSNIE_Len[1]=*((UINT8 *)wrqin->u.data.pointer + 1);
					DBGPRINT(RT_DEBUG_TRACE,( "IE1 %02x %02x\n",RSNIe[1],RSNIE_Len[1]));
					pAd->ApCfg.MBSSID[apidx].RSNIE_Len[1] = RSNIE_Len[1];
					NdisMoveMemory(pAd->ApCfg.MBSSID[apidx].RSN_IE[1], (UCHAR *)(wrqin->u.data.pointer)+2, RSNIE_Len[1]);
					offset_next_ie=RSNIE_Len[1]+2;
				}
				else
					DBGPRINT(RT_DEBUG_TRACE,("IE use only %02x\n",RSNIe[0]));

				NdisZeroMemory(pAd->ApCfg.MBSSID[apidx].RSN_IE[0], MAX_LEN_OF_RSNIE);
				RSNIe[0]=*(((UINT8 *)wrqin->u.data.pointer)+offset_next_ie);
				RSNIE_Len[0]=*(((UINT8 *)wrqin->u.data.pointer) + offset_next_ie + 1);
				if(IE_WPA != RSNIe[0] && IE_RSN != RSNIe[0] )
				{
					Status = -EINVAL;
					break;
				}
				pAd->ApCfg.MBSSID[apidx].RSNIE_Len[0] = RSNIE_Len[0];
				NdisMoveMemory(pAd->ApCfg.MBSSID[apidx].RSN_IE[0], ((UCHAR *)(wrqin->u.data.pointer))+2+offset_next_ie, RSNIE_Len[0]);
				APMakeAllBssBeacon(pAd);
				APUpdateAllBeaconFrame(pAd);
			}
#endif /* 0 */

			RTMP_AP_IoctlHandle(pAd, wrqin, CMD_RTPRIV_IOCTL_AP_SIOCSIWGENIE, 0, NULL, 0);
			break;
#endif /* HOSTAPD_SUPPORT */

		case SIOCGIWPRIV:
			if (wrqin->u.data.pointer) 
			{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
				if (access_ok(wrqin->u.data.pointer, sizeof(ap_privtab)) != TRUE)
#else
				if (access_ok(VERIFY_WRITE, wrqin->u.data.pointer, sizeof(ap_privtab)) != TRUE)
#endif
					break;
				if ((sizeof(ap_privtab) / sizeof(ap_privtab[0])) <= wrq->u.data.length)
				{
					wrqin->u.data.length = sizeof(ap_privtab) / sizeof(ap_privtab[0]);
					if (copy_to_user(wrqin->u.data.pointer, ap_privtab, sizeof(ap_privtab)))
						Status = RTMP_IO_EFAULT;
				}
				else
					Status = RTMP_IO_E2BIG;
			}
			break;
		case RTPRIV_IOCTL_SET:
			{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
				if (access_ok(wrqin->u.data.pointer, wrqin->u.data.length) == TRUE)
#else
				if (access_ok(VERIFY_READ, wrqin->u.data.pointer, wrqin->u.data.length) == TRUE)
#endif
					Status = RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_SET, 0, NULL, 0);
			}
			break;
		    
		case RTPRIV_IOCTL_SHOW:
			{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
				if (access_ok(wrqin->u.data.pointer, wrqin->u.data.length) == TRUE)
#else
				if (access_ok(VERIFY_READ, wrqin->u.data.pointer, wrqin->u.data.length) == TRUE)
#endif
					Status = RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_SHOW, 0, NULL, 0);
			}
			break;	
			
#ifdef INF_AR9
#ifdef AR9_MAPI_SUPPORT
		case RTPRIV_IOCTL_GET_AR9_SHOW:
			{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
				if (access_ok(wrqin->u.data.pointer, wrqin->u.data.length) == TRUE)
#else
				if (access_ok(VERIFY_READ, wrqin->u.data.pointer, wrqin->u.data.length) == TRUE)
#endif
					Status = RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_GET_AR9_SHOW, 0, NULL, 0);
			}	
		    break;
#endif /*AR9_MAPI_SUPPORT*/
#endif /* INF_AR9 */

#ifdef WSC_AP_SUPPORT
		case RTPRIV_IOCTL_SET_WSCOOB:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_SET_WSCOOB, 0, NULL, 0);
		    break;
#endif/*WSC_AP_SUPPORT*/

/* modified by Red@Ralink, 2009/09/30 */
		case RTPRIV_IOCTL_GET_MAC_TABLE:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_GET_MAC_TABLE, 0, NULL, 0);
		    break;

		case RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, 0, NULL, 0);
			break;
/* end of modification */

#ifdef AP_SCAN_SUPPORT
		case RTPRIV_IOCTL_GSITESURVEY:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_GSITESURVEY, 0, NULL, 0);
			break;
#endif /* AP_SCAN_SUPPORT */

		case RTPRIV_IOCTL_STATISTICS:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_STATISTICS, 0, NULL, 0);
			break;

#ifdef WSC_AP_SUPPORT
		case RTPRIV_IOCTL_WSC_PROFILE:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_WSC_PROFILE, 0, NULL, 0);
		    break;
#endif /* WSC_AP_SUPPORT */
#ifdef DOT11_N_SUPPORT
		case RTPRIV_IOCTL_QUERY_BATABLE:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_QUERY_BATABLE, 0, NULL, 0);
		    break;
#endif /* DOT11_N_SUPPORT */
		case RTPRIV_IOCTL_E2P:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_E2P, 0, NULL, 0);
			break;

#ifdef DBG
		case RTPRIV_IOCTL_BBP:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_BBP, 0, NULL, 0);
			break;
			
		case RTPRIV_IOCTL_MAC:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_MAC, 0, NULL, 0);
			break;
            
#ifdef RTMP_RF_RW_SUPPORT
		case RTPRIV_IOCTL_RF:
			RTMP_AP_IoctlHandle(pAd, wrq, CMD_RTPRIV_IOCTL_RF, 0, NULL, 0);
			break;
#endif /* RTMP_RF_RW_SUPPORT */
#endif /* DBG */

		default:
/*			DBGPRINT(RT_DEBUG_ERROR, ("IOCTL::unknown IOCTL's cmd = 0x%08x\n", cmd)); */
			Status = RTMP_IO_EOPNOTSUPP;
			break;
	}

LabelExit:
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
