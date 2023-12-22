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
	mesh.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2007-06-25      For mesh (802.11s) support.
*/
#define RTMP_MODULE_OS

#ifdef MESH_SUPPORT


/*#include "rt_config.h" */
/*#include "mesh_sanity.h" */
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"

/*static VOID MeshCfgInit(RTMP_ADAPTER *pAd, RTMP_STRING *pHostName); */


/*
========================================================================
Routine Description:
    Init Mesh function.

Arguments:
    ad_p            points to our adapter
    main_dev_p      points to the main BSS network interface

Return Value:
    None

Note:
	1. Only create and initialize virtual network interfaces.
	2. No main network interface here.
========================================================================
*/
VOID RTMP_Mesh_Init(
	IN VOID 				*pAd,
	IN PNET_DEV				main_dev_p,
	IN RTMP_STRING *pHostName)
{
	RTMP_OS_NETDEV_OP_HOOK	netDevOpHook;
	ULONG OpMode;


	/* init operation functions */
	NdisZeroMemory((PUCHAR)&netDevOpHook, sizeof(RTMP_OS_NETDEV_OP_HOOK));
	netDevOpHook.open = Mesh_VirtualIF_Open;
	netDevOpHook.stop = Mesh_VirtualIF_Close;
	netDevOpHook.xmit = rt28xx_send_packets;
	netDevOpHook.ioctl = rt28xx_ioctl;	

	/* init operation functions */
	RTMP_DRIVER_OP_MODE_GET(pAd, &OpMode);

#ifdef CONFIG_STA_SUPPORT
#if WIRELESS_EXT >= 12
	if (OpMode == OPMODE_STA)
	{
		netDevOpHook.iw_handler = (void *)&rt28xx_iw_handler_def;
	}
#endif /*WIRELESS_EXT >= 12 */
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_APSTA_MIXED_SUPPORT
#if WIRELESS_EXT >= 12
	if (OpMode == OPMODE_AP)
	{
		netDevOpHook.iw_handler = &rt28xx_ap_iw_handler_def;
	}
#endif /*WIRELESS_EXT >= 12 */
#endif /* CONFIG_APSTA_MIXED_SUPPORT */

	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MESH_INIT,
						0, &netDevOpHook, 0);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s <---\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
	
}


/*
========================================================================
Routine Description:
    Open a virtual network interface.

Arguments:
    pDev           which WLAN network interface

Return Value:
    0: open successfully
    otherwise: open fail

Note:
========================================================================
*/
INT Mesh_VirtualIF_Open(
	IN PNET_DEV		pDev)
{
	VOID *pAd;

	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	ASSERT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(pDev)));
	

	if (RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MESH_OPEN_PRE, 0,
							pDev, 0) != NDIS_STATUS_SUCCESS)
		return -1;

	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();

	RTMP_OS_NETDEV_START_QUEUE(pDev);

	if (RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MESH_OPEN_POST, 0,
							pDev, 0) != NDIS_STATUS_SUCCESS)
		return -1;

#if 0 /* os abl move */
	if (ADHOC_ON(pAd))
		return -1;

	pAd->MeshTab.bcn_buf.bBcnSntReq = TRUE;

	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();

	RTMP_OS_NETDEV_START_QUEUE(dev_p);
	
	/* Statup Mesh Protocol Stack. */
	MeshUp(pAd);

#ifdef CONFIG_STA_SUPPORT
	AsicSetPreTbtt(pAd, TRUE);
	AsicEnableMESHSync(pAd);
#endif /* CONFIG_STA_SUPPORT */
#endif /* 0 */

	DBGPRINT(RT_DEBUG_TRACE, ("%s: <=== %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(pDev)));

	return 0;
}


/*
========================================================================
Routine Description:
    Close a virtual network interface.

Arguments:
    dev_p           which WLAN network interface

Return Value:
    0: close successfully
    otherwise: close fail

Note:
========================================================================
*/
INT Mesh_VirtualIF_Close(
	IN	PNET_DEV	pDev)
{
	VOID *pAd;

	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	ASSERT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(pDev)));
	

	/* stop mesh. */
	RTMP_OS_NETDEV_STOP_QUEUE(pDev);

	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MESH_CLOSE, 0, pDev, 0);

#if 0 /* os abl move */
	MeshDown(pAd, TRUE);

	pAd->MeshTab.bcn_buf.bBcnSntReq = FALSE;

#ifdef CONFIG_STA_SUPPORT
	/* Disable pre-tbtt interrupt */
	AsicSetPreTbtt(pAd, FALSE);

	/*update beacon Sync */
	/*if rausb0 is up => stop beacon */
	/*if rausb0 is down => we will call AsicDisableSync() in usb_rtusb_close_device() */
	if (INFRA_ON(pAd))
		AsicEnableBssSync(pAd, pAd->CommonCfg.BeaconPeriod);
	else if (ADHOC_ON(pAd))
		AsicEnableIbssSync(pAd);
	else
		AsicDisableSync(pAd);
#endif /* CONFIG_STA_SUPPORT */

	pAd->MeshTab.bcn_buf.bBcnSntReq = FALSE;

#ifdef CONFIG_AP_SUPPORT
	APMakeAllBssBeacon(pAd);
	APUpdateAllBeaconFrame(pAd);
#endif /* CONFIG_AP_SUPPORT */
#endif /* 0 */

	VIRTUAL_IF_DOWN(pAd);

	RT_MOD_DEC_USE_COUNT();

	return 0;
} 


#if 0 /* os abl move to common/mesh.c */
VOID MeshCfgInit(RTMP_ADAPTER *pAd, RTMP_STRING *pHostName)
{
	INT	i;

	/* default configuration of Mesh. */
	pAd->MeshTab.OpMode = MESH_MP;

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		pAd->MeshTab.OpMode |= MESH_AP;
#endif /* CONFIG_AP_SUPPORT */

	pAd->MeshTab.PathProtocolId = MESH_HWMP;
	pAd->MeshTab.PathMetricId = MESH_AIRTIME;
	pAd->MeshTab.ContgesionCtrlId = NULL_PROTOCOL;
	pAd->MeshTab.TTL = MESH_TTL;
	pAd->MeshTab.MeshMaxTxRate = 0;
	pAd->MeshTab.MeshMultiCastAgeOut = MULTIPATH_AGEOUT;
	pAd->MeshTab.UCGEnable = FALSE;
	pAd->MeshTab.MeshCapability.field.Forwarding = 1;
	if (pAd->MeshTab.MeshIdLen == 0)
	{
		pAd->MeshTab.MeshIdLen = strlen(DEFAULT_MESH_ID);
		NdisMoveMemory(pAd->MeshTab.MeshId, DEFAULT_MESH_ID, pAd->MeshTab.MeshIdLen);
	}

	/* initialize state */
	pAd->MeshTab.EasyMeshSecurity = TRUE;	/* Default is TRUE for CMPC */
	pAd->MeshTab.bInitialMsaDone = FALSE;
	pAd->MeshTab.bKeyholderDone  = FALSE;
	pAd->MeshTab.bConnectedToMKD = FALSE;
	pAd->MeshTab.MeshOnly = FALSE;

	pAd->MeshTab.bAutoTxRateSwitch = TRUE;
	pAd->MeshTab.DesiredTransmitSetting.field.MCS = MCS_AUTO;

	for (i = 0; i < MAX_MESH_LINKS; i++)
		NdisZeroMemory(&pAd->MeshTab.MeshLink[i].Entry, sizeof(MESH_LINK_ENTRY));

	if (strlen(pHostName) > 0)
	{
		if (strlen(pHostName) < MAX_HOST_NAME_LEN)
			strcpy((RTMP_STRING *) pAd->MeshTab.HostName, pHostName);
		else
			strncpy((RTMP_STRING *) pAd->MeshTab.HostName, pHostName, MAX_HOST_NAME_LEN-1);
	}
	else
		strcpy((RTMP_STRING *) pAd->MeshTab.HostName, DEFAULT_MESH_HOST_NAME);

}
#endif /* 0 */

#ifdef LINUX
#if (WIRELESS_EXT >= 12)
struct iw_statistics *Mesh_VirtualIF_get_wireless_stats(
	IN  struct net_device *net_dev);
#endif
#endif /* LINUX */


VOID RTMP_Mesh_Remove(
	IN VOID 			*pAd)
{
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MESH_REMOVE, 0, NULL, 0);

#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s <---\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
}


#endif /* MESH_SUPPORT */
