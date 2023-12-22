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
	p2p_inf.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------

*/
#define RTMP_MODULE_OS

#ifdef P2P_SUPPORT

#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"


/*
========================================================================
Routine Description:
    Initialize Multi-BSS function.

Arguments:
    pAd				points to our adapter
    pDevMain		points to the main BSS network interface

Return Value:
    None

Note:
	1. Only create and initialize virtual network interfaces.
	2. No main network interface here.
========================================================================
*/
VOID RTMP_P2P_Init(VOID *pAd, PNET_DEV main_dev_p)
{
	RTMP_OS_NETDEV_OP_HOOK netDevHook;

	DBGPRINT(RT_DEBUG_TRACE, ("%s --->\n", __FUNCTION__));

	/* init operation functions and flags */
	NdisZeroMemory(&netDevHook, sizeof(netDevHook));
	netDevHook.open = P2P_VirtualIF_Open;	/* device opem hook point */
	netDevHook.stop = P2P_VirtualIF_Close;	/* device close hook point */
	netDevHook.xmit = P2P_VirtualIF_PacketSend;	/* hard transmit hook point */
	netDevHook.ioctl = rt28xx_ioctl;	/* ioctl hook point */

#if WIRELESS_EXT >= 12
	netDevHook.iw_handler = (void *)&rt28xx_ap_iw_handler_def;
#endif /* WIRELESS_EXT >= 12 */
	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_P2P_INIT,
						0, &netDevHook, 0);

	DBGPRINT(RT_DEBUG_TRACE, ("%s <---\n", __FUNCTION__));

}

/*
========================================================================
Routine Description:
    Open a virtual network interface.

Arguments:
    dev_p           which WLAN network interface

Return Value:
    0: open successfully
    otherwise: open fail

Note:
========================================================================
*/
INT P2P_VirtualIF_Open(PNET_DEV dev_p)
{
	VOID *pAd = RTMP_OS_NETDEV_GET_PRIV(dev_p);

	ASSERT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(dev_p)));

	if (RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_P2P_OPEN_PRE, 0,
							dev_p, 0) != NDIS_STATUS_SUCCESS)
		return -1;

	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();

	RTMP_OS_NETDEV_START_QUEUE(dev_p);

	if (RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_P2P_OPEN_POST, 0,
							dev_p, 0) != NDIS_STATUS_SUCCESS)
		return -1;

	DBGPRINT(RT_DEBUG_TRACE, ("%s: <=== %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(dev_p)));

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
INT P2P_VirtualIF_Close(PNET_DEV dev_p)
{
	VOID *pAd = RTMP_OS_NETDEV_GET_PRIV(dev_p);

	ASSERT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(dev_p)));

	/* stop p2p. */
	RTMP_OS_NETDEV_STOP_QUEUE(dev_p);

	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_P2P_CLOSE, 0, dev_p, 0);

	VIRTUAL_IF_DOWN(pAd);

	RT_MOD_DEC_USE_COUNT();

	return 0;
} 


/*
========================================================================
Routine Description:
    Send a packet to WLAN.

Arguments:
    skb_p           points to our adapter
    dev_p           which WLAN network interface

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
INT P2P_VirtualIF_PacketSend(PNDIS_PACKET skb, PNET_DEV dev)
{
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_INFO, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
	
	MEM_DBG_PKT_ALLOC_INC(skb);

	if(!(RTMP_OS_NETDEV_STATE_RUNNING(dev)))
	{
		/* the interface is down */
		RELEASE_NDIS_PACKET(NULL, skb, NDIS_STATUS_FAILURE);
		return 0;
	}

	return P2P_PacketSend(skb, dev, rt28xx_packet_xmit);
}


VOID RTMP_P2P_Remove(VOID *pAd)
{
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_TRACE, ("%s --->\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */

	RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_P2P_REMOVE, 0, NULL, 0);
		
#ifdef RELEASE_EXCLUDE
	DBGPRINT(RT_DEBUG_TRACE, ("%s <---\n", __FUNCTION__));
#endif /* RELEASE_EXCLUDE */
}

#endif /* P2P_SUPPORT */

