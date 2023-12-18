/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rt_main_dev.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/


#define RTMP_MODULE_OS

/*#include "rt_config.h" */
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#ifndef SA_SHIRQ
#define SA_SHIRQ IRQF_SHARED
#endif
#endif

// TODO: shiang-6590, remove it when MP
#ifdef RTMP_MAC_PCI
MODULE_LICENSE("GPL");
#endif /* RTMP_MAC_PCI */
// TODO: End---

#ifdef RTMP_MAC_USB
#ifdef OS_ABL_SUPPORT
MODULE_LICENSE("GPL");
#endif /* OS_ABL_SUPPORT */
#endif /* RTMP_MAC_USB */


/*---------------------------------------------------------------------*/
/* Private Variables Used                                              */
/*---------------------------------------------------------------------*/

RTMP_STRING *mac = "";		/* default 00:00:00:00:00:00 */
RTMP_STRING *mode = "";		/* supported mode: normal/ate/monitor;  default: normal */
RTMP_STRING *hostname = "";	/* default CMPC */

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,12)
MODULE_PARM (mac, "s");
MODULE_PARM(mode, "s");
#else
module_param (mac, charp, 0);
module_param(mode, charp, 0);
#endif
MODULE_PARM_DESC (mac, "rt_wifi: wireless mac addr");
MODULE_PARM_DESC (mode, "rt_wifi: wireless operation mode");

#ifdef OS_ABL_SUPPORT
RTMP_DRV_ABL_OPS RtmpDrvOps, *pRtmpDrvOps = &RtmpDrvOps;
RTMP_NET_ABL_OPS RtmpDrvNetOps, *pRtmpDrvNetOps = &RtmpDrvNetOps;
#endif /* OS_ABL_SUPPORT */


/*---------------------------------------------------------------------*/
/* Prototypes of Functions Used                                        */
/*---------------------------------------------------------------------*/

/* public function prototype */
int rt28xx_close(VOID *net_dev);
int rt28xx_open(VOID *net_dev);

/* private function prototype */
INT rt28xx_send_packets(IN struct sk_buff *skb_p, IN struct net_device *net_dev);

#ifdef HW_COEXISTENCE_SUPPORT
VOID InitHWCoexistence(VOID *pAd);
#endif /* HW_COEXISTENCE_SUPPORT */

struct net_device_stats *RT28xx_get_ether_stats(struct net_device *net_dev);


/*
========================================================================
Routine Description:
    Close raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int MainVirtualIF_close(IN struct net_device *net_dev)
{
    VOID *pAd = NULL;

	pAd = RTMP_OS_NETDEV_GET_PRIV(net_dev);

	if (pAd == NULL)
		return 0;

	netif_carrier_off(net_dev);
	netif_stop_queue(net_dev);

	RTMPInfClose(pAd);

#ifdef IFUP_IN_PROBE
#else
	VIRTUAL_IF_DOWN(pAd);
#endif /* IFUP_IN_PROBE */

	RT_MOD_DEC_USE_COUNT();

	return 0; /* close ok */
}

/*
========================================================================
Routine Description:
    Open raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int MainVirtualIF_open(struct net_device *net_dev)
{
	VOID *pAd = NULL;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);

	if (pAd == NULL)
		return 0;

#ifdef CONFIG_AP_SUPPORT
	/* pAd->ApCfg.MBSSID[MAIN_MBSSID].bcn_buf.bBcnSntReq = TRUE; */
	RTMP_DRIVER_AP_MAIN_OPEN(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef IFUP_IN_PROBE
	while (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
	{
		OS_WAIT(10);
		DBGPRINT(RT_DEBUG_TRACE, ("Card not ready, NDIS_STATUS_SUCCESS!\n"));
	}
#else
	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;
#endif /* IFUP_IN_PROBE */

	RT_MOD_INC_USE_COUNT();

	netif_start_queue(net_dev);
	netif_carrier_on(net_dev);
	netif_wake_queue(net_dev);

	return 0;
}


/*
========================================================================
Routine Description:
    Close raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int rt28xx_close(VOID *dev)
{
	struct net_device * net_dev = (struct net_device *)dev;
    VOID	*pAd = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt28xx_close\n"));

#if 0
#ifdef RTMP_MAC_USB
/*	DECLARE_WAIT_QUEUE_HEAD(unlink_wakeup); */
/*	DECLARE_WAITQUEUE(wait, current); */

	/*RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS); */
#endif /* RTMP_MAC_USB */
#endif
	GET_PAD_FROM_NET_DEV(pAd, net_dev);
	if (pAd == NULL)
		return 0;

	RTMPDrvClose(pAd, net_dev);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt28xx_close\n"));
	return 0;
}


/*
========================================================================
Routine Description:
    Open raxx interface.

Arguments:
	*net_dev			the raxx interface pointer

Return Value:
    0					Open OK
	otherwise			Open Fail

Note:
========================================================================
*/
int rt28xx_open(VOID *dev)
{
	struct net_device * net_dev = (struct net_device *)dev;
	VOID *pAd = NULL;
	int retval = 0;
	ULONG OpMode;

	if (sizeof(ra_dma_addr_t) < sizeof(dma_addr_t))
		DBGPRINT(RT_DEBUG_ERROR, ("Fatal error for DMA address size!!!\n"));

	GET_PAD_FROM_NET_DEV(pAd, net_dev);
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -1;
	}

	RTMP_DRIVER_MCU_SLEEP_CLEAR(pAd);

	RTMP_DRIVER_OP_MODE_GET(pAd, &OpMode);

#ifdef CONFIG_WIRELESS_EXT
#if WIRELESS_EXT >= 12
/*	if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_MAIN) */
	if (RTMP_DRIVER_MAIN_INF_CHECK(pAd, RT_DEV_PRIV_FLAGS_GET(net_dev)) == NDIS_STATUS_SUCCESS)
	{
#ifdef CONFIG_APSTA_MIXED_SUPPORT
		if (OpMode == OPMODE_AP)
			net_dev->wireless_handlers = (struct iw_handler_def *) &rt28xx_ap_iw_handler_def;
#endif /* CONFIG_APSTA_MIXED_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
		if (OpMode == OPMODE_STA)
			net_dev->wireless_handlers = (struct iw_handler_def *) &rt28xx_iw_handler_def;
#endif /* CONFIG_STA_SUPPORT */
	}
#endif /* WIRELESS_EXT >= 12 */
#endif /* #ifdef CONFIG_WIRELESS_EXT */

	/*
		Request interrupt service routine for PCI device
		register the interrupt routine with the os

		AP Channel auto-selection will be run in rt28xx_init(),
		so we must reqister IRQ hander here.
	*/
	RtmpOSIRQRequest(net_dev);

	/* Init IRQ parameters stored in pAd */
/*	rtmp_irq_init(pAd); */
	RTMP_DRIVER_IRQ_INIT(pAd);

	/* Chip & other init */
	if (rt28xx_init(pAd, mac, hostname) == FALSE)
		goto err;

#ifdef MBSS_SUPPORT
	/*
		the function can not be moved to RT2860_probe() even register_netdev()
		is changed as register_netdevice().
		Or in some PC, kernel will panic (Fedora 4)
	*/
#if defined(P2P_APCLI_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)

#else
	RT28xx_MBSS_Init(pAd, net_dev);
#endif /* P2P_APCLI_SUPPORT */
#endif /* MBSS_SUPPORT */

#ifdef WDS_SUPPORT
	RT28xx_WDS_Init(pAd, net_dev);
#endif /* WDS_SUPPORT */

#ifdef APCLI_SUPPORT
#if defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) ||  defined(P2P_APCLI_SUPPORT) || defined(CFG80211_MULTI_STA)
#else
	RT28xx_ApCli_Init(pAd, net_dev);
#endif /* P2P_APCLI_SUPPORT */
#endif /* APCLI_SUPPORT */

#ifdef CONFIG_SNIFFER_SUPPORT
	RT28xx_Monitor_Init(pAd, net_dev);
#endif /* CONFIG_SNIFFER_SUPPORT */

#ifdef MESH_SUPPORT
	RTMP_Mesh_Init(pAd, net_dev, hostname);
#endif /* MESH_SUPPORT */


#ifdef RT_CFG80211_SUPPORT
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
	RTMP_CFG80211_DummyP2pIf_Init(pAd);
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */
#ifdef CFG80211_MULTI_STA
	RTMP_CFG80211_MutliStaIf_Init(pAd);
#endif /* CFG80211_MULTI_STA */
#else
#ifdef P2P_SUPPORT
	RTMP_P2P_Init(pAd, net_dev);
#endif /* P2P_SUPPORT */
#endif /* RT_CFG80211_SUPPORT */

#ifdef LINUX
#ifdef RT_CFG80211_SUPPORT
	RTMP_DRIVER_CFG80211_START(pAd);
#endif /* RT_CFG80211_SUPPORT */
#endif /* LINUX */

	RTMPDrvOpen(pAd);

#ifdef VENDOR_FEATURE2_SUPPORT
	DBGPRINT(RT_DEBUG_OFF, ("Number of Packet Allocated in open = %lu\n", OS_NumOfPktAlloc));
	DBGPRINT(RT_DEBUG_OFF, ("Number of Packet Freed in open = %lu\n", OS_NumOfPktFree));
#endif /* VENDOR_FEATURE2_SUPPORT */

	return (retval);

err:
	RTMP_DRIVER_IRQ_RELEASE(pAd);

	return (-1);
}


PNET_DEV RtmpPhyNetDevInit(VOID *pAd, RTMP_OS_NETDEV_OP_HOOK *pNetDevHook)
{
	struct net_device *net_dev = NULL;
	ULONG InfId, OpMode;
#if defined(CONFIG_CSO_SUPPORT) || defined(CONFIG_TSO_SUPPORT)
	UCHAR flg;
#endif /* defined(CONFIG_CSO_SUPPORT) || defined(CONFIG_TSO_SUPPORT) */


	RTMP_DRIVER_MAIN_INF_GET(pAd, &InfId);

/*	net_dev = RtmpOSNetDevCreate(pAd, INT_MAIN, 0, sizeof(struct mt_dev_priv), INF_MAIN_DEV_NAME); */
	RTMP_DRIVER_MAIN_INF_CREATE(pAd, &net_dev);
	if (net_dev == NULL)
	{
		DBGPRINT(RT_DEBUG_OFF,
			("%s(): main physical net device creation failed!\n",
			__func__));
		return NULL;
	}

	NdisZeroMemory((unsigned char *)pNetDevHook, sizeof(RTMP_OS_NETDEV_OP_HOOK));
	pNetDevHook->open = MainVirtualIF_open;
	pNetDevHook->stop = MainVirtualIF_close;
	pNetDevHook->xmit = rt28xx_send_packets;
#ifdef IKANOS_VX_1X0
	pNetDevHook->xmit = IKANOS_DataFramesTx;
#endif /* IKANOS_VX_1X0 */
	pNetDevHook->ioctl = rt28xx_ioctl;
	pNetDevHook->priv_flags = InfId; /*INT_MAIN; */
	pNetDevHook->get_stats = RT28xx_get_ether_stats;

	pNetDevHook->needProtcted = FALSE;

#if (WIRELESS_EXT < 21) && (WIRELESS_EXT >= 12)
	pNetDevHook->get_wstats = rt28xx_get_wireless_stats;
#endif

	RTMP_DRIVER_OP_MODE_GET(pAd, &OpMode);

	/* put private data structure */
	RTMP_OS_NETDEV_SET_PRIV(net_dev, pAd);

#ifdef CONFIG_STA_SUPPORT
#if WIRELESS_EXT >= 12
	if (OpMode == OPMODE_STA)
	{
		pNetDevHook->iw_handler = (void *)&rt28xx_iw_handler_def;
	}
#endif /*WIRELESS_EXT >= 12 */
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_APSTA_MIXED_SUPPORT
#if WIRELESS_EXT >= 12
	if (OpMode == OPMODE_AP)
	{
		pNetDevHook->iw_handler = &rt28xx_ap_iw_handler_def;
	}
#endif /*WIRELESS_EXT >= 12 */
#endif /* CONFIG_APSTA_MIXED_SUPPORT */

	/* double-check if pAd is associated with the net_dev */
	if (RTMP_OS_NETDEV_GET_PRIV(net_dev) == NULL)
	{
		RtmpOSNetDevFree(net_dev);
		return NULL;
	}

	RTMP_DRIVER_NET_DEV_SET(pAd, net_dev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	SET_MODULE_OWNER(net_dev);
#endif

#ifdef CONFIG_CSO_SUPPORT
	RTMP_DRIVER_ADAPTER_CSO_SUPPORT_TEST(pAd, &flg);
	if (flg)
		net_dev->features |= NETIF_F_HW_CSUM;
#endif /* CONFIG_CSO_SUPPORT */

#ifdef CONFIG_TSO_SUPPORT
	RTMP_DRIVER_ADAPTER_TSO_SUPPORT_TEST(pAd, &flg);
	if (flg) {
		net_dev->features |= NETIF_F_SG;
		net_dev->features |= NETIF_F_TSO;
		net_dev->features |= NETIF_F_TSO6;
	}
#endif /* CONFIG_CSO_SUPPORT */

	return net_dev;
}


VOID *RtmpNetEthConvertDevSearch(VOID *net_dev_, UCHAR *pData)
{
	struct net_device *pNetDev;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
	struct net_device *net_dev = (struct net_device *)net_dev_;
	struct net *net;
	net = dev_net(net_dev);

	BUG_ON(!net);
	for_each_netdev(net, pNetDev)
#else
	struct net *net;

	struct net_device *net_dev = (struct net_device *)net_dev_;
	BUG_ON(!net_dev->nd_net);
	net = net_dev->nd_net;
	for_each_netdev(net, pNetDev)
#endif
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
		for_each_netdev(pNetDev)
#else
	for (pNetDev = dev_base; pNetDev; pNetDev = pNetDev->next)
#endif
#endif
	{
		if ((pNetDev->type == ARPHRD_ETHER)
			&& NdisEqualMemory(pNetDev->dev_addr, &pData[6], pNetDev->addr_len))
			break;
	}

	return (VOID *)pNetDev;
}


#ifdef CONFIG_TSO_SUPPORT
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
#define tcp_hdr(skb) (skb->h.th)
#define tcp_hdrlen(skb) (skb->h.th->doff << 2)
#define skb_transport_offset(skb) (skb->h.raw - skb->data)
#define skb_transport_header(skb) (skb->h.raw)
#define ipv6_hdr(skb) (skb->nh.ipv6h)
#define ip_hdr(skb) (skb->nh.iph)
#define skb_network_offset(skb) (skb->nh.raw - skb->data)
#define skb_network_header(skb) (skb->nh.raw)
#define skb_tail_pointer(skb) skb->tail
#define skb_copy_to_linear_data_offset(skb, offset, from, len) \
                                 memcpy(skb->data + offset, from, len)
#define skb_network_header_len(skb) (skb->h.raw - skb->nh.raw)
#define skb_mac_header(skb) skb->mac.raw
#endif /* < 2.6.22 */


static char *ip_sum_str[]={"none", "unncessary", "complete", "partial"};
VOID dump_skb_info(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;


	DBGPRINT(RT_DEBUG_OFF, ("Binding sock info:\n"));
	if (sk) {
		//NETIF_F_HW_CSUM;NETIF_F_TSO
		DBGPRINT(RT_DEBUG_OFF, ("\tsk_route_caps=0x%x\n", sk->sk_route_caps));
		DBGPRINT(RT_DEBUG_OFF, ("\tsk_gso_type=%d\n", sk->sk_gso_type));
	} else {
		DBGPRINT(RT_DEBUG_OFF, ("\tNo binding sock!\n"));
	}

	DBGPRINT(RT_DEBUG_OFF, ("Binding Skb info:\n"));
	DBGPRINT(RT_DEBUG_OFF, ("\tprotocol=%d\n", skb->protocol));
	DBGPRINT(RT_DEBUG_OFF, ("\tip_summed=%d", skb->ip_summed));
	if (skb->ip_summed <= 3)
		DBGPRINT(RT_DEBUG_OFF, ("(%s)\n", ip_sum_str[skb->ip_summed]));
	else
		DBGPRINT(RT_DEBUG_OFF, ("\n"));

	DBGPRINT(RT_DEBUG_OFF, ("\tcsum=0x%x\n", skb->csum));
	DBGPRINT(RT_DEBUG_OFF, ("\tcsum_start=%d\n", skb->csum_start));
	DBGPRINT(RT_DEBUG_OFF, ("\tcsum_offset=%d\n", skb->csum_offset));
	DBGPRINT(RT_DEBUG_OFF, ("\tskb->head=0x%x\n", skb->head));
	DBGPRINT(RT_DEBUG_OFF, ("\tskb->data=0x%x\n", skb->data));
	DBGPRINT(RT_DEBUG_OFF, ("\tskb->len=%d\n", skb->len));
	if (skb->protocol == htons(ETH_P_IP)){
		DBGPRINT(RT_DEBUG_OFF, ("\tip_hdr=0x%x\n", ip_hdr(skb)));
		DBGPRINT(RT_DEBUG_OFF, ("\ttcp_hdr=0x%x\n", tcp_hdr(skb)));
	}
	DBGPRINT(RT_DEBUG_OFF, ("\tgso_size=%d\n", skb_shinfo(skb)->gso_size));
	DBGPRINT(RT_DEBUG_OFF, ("\tgso_type=%d\n", skb_shinfo(skb)->gso_type));
	DBGPRINT(RT_DEBUG_OFF, ("\tgso_seg=%d\n", skb_shinfo(skb)->gso_segs));
	DBGPRINT(RT_DEBUG_OFF, ("\tnr_frags=%d\n", skb_shinfo(skb)->nr_frags));
}


VOID dump_nonlinear_pkt(struct sk_buff *skb)
{
	if (skb_is_nonlinear(skb)) {
		int i, nr_frags = skb_shinfo(skb)->nr_frags, len;
		void *buf;
		DBGPRINT(RT_DEBUG_OFF,
			("%s(): skb is non-linear! skb->len=%d, skb->data_len=%d, ",
			__func__, skb->len, skb->data_len));
		DBGPRINT(RT_DEBUG_OFF,
			("non-paged data len=%d, nr_frags=%d!\n",
			skb_headlen(skb), nr_frags));

		len = skb->len;
		buf = kmalloc(len, GFP_KERNEL);
		if (buf) {
			void *ptr = buf;

			memcpy(buf, skb->data, skb_headlen(skb));
			len = skb_headlen(skb);
			ptr += len;
			for (i =0; i < nr_frags; i++) {
				skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
				void *addr;

				addr = ((void *) page_address(frag->page)) + frag->page_offset;
				DBGPRINT(RT_DEBUG_OFF,
					("frag %d: page=0x%x, offset=%d, addr=0x%x, size=%d\n",
					i, frag->page, frag->page_offset, addr, frag->size));
				memcpy(ptr, addr, frag->size);
				ptr += frag->size;
				len += frag->size;
			}

			hex_dump("SG packet buffer", buf, len > 100 ? 100 : len);
			kfree(buf);
		}
	}
}


INT rt28xx_tso_xmit(struct sk_buff *skb)
{
	uint16_t tucse;
	unsigned short gso_type;
	unsigned int gso_seg, nr_frags;
	uint8_t ipcso, tucso;

//+++ Required for Ralink CSO/TSO
	uint8_t tups, ips, is_tcp = 0;
	uint16_t mss;


	NdisZeroMemory(&skb->cb[CB_OFF + 31], 5);

#ifdef TX_PKT_SG
	if (skb_is_nonlinear(skb))
		RTMP_SET_PKT_SG(skb, 1);
#endif /* TX_PKT_SG */

	if (skb_is_gso(skb) || (skb->ip_summed == CHECKSUM_PARTIAL)) {
		if (skb_is_gso(skb)) {
			DBGPRINT(RT_DEBUG_OFF,
				("%s(): skb_is_gso()=%d, skb->ip_summed=%d!\n",
				__func__, skb_is_gso(skb), skb->ip_summed));
			if (skb_is_nonlinear(skb))
				dump_nonlinear_pkt(skb);
				//hex_dump("rt28xx_tso_xmit", (UCHAR *)skb->data, skb->len);
		}

		if (skb->protocol == htons(ETH_P_IP))
		{
			struct iphdr *iph = ip_hdr(skb);

			if (0 /*iph->protocol == IPPROTO_TCP*/) {
				dump_skb_info(skb);
			}
		}

		mss = skb_shinfo(skb)->gso_size; /* gso_size= mtu - fragheaderlen in ip_output.c */
		gso_type = skb_shinfo(skb)->gso_type;
		gso_seg = skb_shinfo(skb)->gso_segs;
		nr_frags = skb_shinfo(skb)->nr_frags;

		if (skb->protocol == htons(ETH_P_IP)) {
			struct iphdr *iph = ip_hdr(skb);

			iph->check = 0;
			tcp_hdr(skb)->check = ~csum_tcpudp_magic(iph->saddr, iph->daddr,
							0, IPPROTO_TCP, 0);
			is_tcp = (iph->protocol == IPPROTO_TCP) ? 1: 0;
		} else if (skb->protocol == htons(ETH_P_IPV6)) {
			tcp_hdr(skb)->check =
				~csum_ipv6_magic(&ipv6_hdr(skb)->saddr, &ipv6_hdr(skb)->daddr,
							0, IPPROTO_TCP, 0);
		}
		ips = skb_network_offset(skb);
		ipcso = (void *)&(ip_hdr(skb)->check) - (void *)skb->data;
		tups = skb_transport_offset(skb) - ips;
		tucso = (void *)&(tcp_hdr(skb)->check) - (void *)skb->data;
		tucse = 0;

		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			RTMP_SET_PKT_CSO(skb, 1);
			RTMP_SET_PKT_IPS(skb, ips);
			RTMP_SET_PKT_TUPS(skb, tups / 4);
		}

		if (is_tcp) {
			RTMP_SET_PKT_TCP(skb, 1);
		}

		if (skb_is_gso(skb)) {
			RTMP_SET_PKT_TSO(skb, 1);
			RTMP_SET_PKT_MSS(skb, mss);
		}

		if (skb_is_gso(skb)) {
			DBGPRINT(RT_DEBUG_OFF, ("\tmss=%d, gso_type=GSO_%s(0x%x), nr_frags=%d!\n",
					mss, (gso_type == SKB_GSO_TCPV4 ? "TCP" : "NonTCP"),
					gso_type, nr_frags));
			DBGPRINT(RT_DEBUG_OFF,
				("\tips=0x%x, ipcso=0x%x, tups=0x%x, tucso=0x%x, tucse=0x%x\n",
					ips, ipcso, tups, tucso, tucse));
			DBGPRINT(RT_DEBUG_OFF,
				("RalinkRequiredFields:\n\tcso/tso=%d/%d,",
				((skb->ip_summed == CHECKSUM_PARTIAL) ? 1 : 0),
				(skb_is_gso(skb) ? 1 : 0)));
			DBGPRINT(RT_DEBUG_OFF,
				("is_tcp=%d, ips=%d, tups=%d, mss=%d!\n",
				is_tcp, ips, tups, mss));
			DBGPRINT(RT_DEBUG_OFF, ("\tfields saved in CB[]:cso/tso=%d:%d,",
				RTMP_GET_PKT_CSO(skb),
				RTMP_GET_PKT_TSO(skb)));
			DBGPRINT(RT_DEBUG_OFF, ("is_tcp=%d, ips=%d, tups=%d, mss=%d!\n",
				RTMP_GET_PKT_TCP(skb),
				RTMP_GET_PKT_IPS(skb),
				RTMP_GET_PKT_TUPS(skb),
				RTMP_GET_PKT_MSS(skb)));
		}
		return TRUE;
	}

	return FALSE;
}
#endif /* CONFIG_TSO_SUPPORT */


/*
========================================================================
Routine Description:
    The entry point for Linux kernel sent packet to our driver.

Arguments:
    sk_buff *skb 	the pointer refer to a sk_buffer.

Return Value:
    0

Note:
	This function is the entry point of Tx Path for OS delivery packet to
	our driver. You only can put OS-depened & STA/AP common handle procedures
	in here.
========================================================================
*/
int rt28xx_packet_xmit(void *pkt)
{
	struct sk_buff *skb = (struct sk_buff *)pkt;
	struct net_device *net_dev = skb->dev;
	struct wifi_dev *wdev;
	PNDIS_PACKET pPacket = (PNDIS_PACKET)skb;

	wdev = RTMP_OS_NETDEV_GET_WDEV(net_dev);
	ASSERT(wdev);

#ifdef CONFIG_TSO_SUPPORT
	rt28xx_tso_xmit(skb);
#endif /* CONFIG_TSO_SUPPORT */

	return RTMPSendPackets((NDIS_HANDLE)wdev, (PPNDIS_PACKET) &pPacket, 1,
							skb->len, RtmpNetEthConvertDevSearch);
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
int rt28xx_send_packets(struct sk_buff *skb, struct net_device *ndev)
{
	if (!(RTMP_OS_NETDEV_STATE_RUNNING(ndev)))
	{
		RELEASE_NDIS_PACKET(NULL, (PNDIS_PACKET)skb, NDIS_STATUS_FAILURE);
		return 0;
	}

	NdisZeroMemory((PUCHAR)&skb->cb[CB_OFF], 26);
	MEM_DBG_PKT_ALLOC_INC(skb);

	return rt28xx_packet_xmit(skb);
}


#if WIRELESS_EXT >= 12
/* This function will be called when query /proc */
struct iw_statistics *rt28xx_get_wireless_stats(struct net_device *net_dev)
{
	VOID *pAd = NULL;
	struct iw_statistics *pStats;
	RT_CMD_IW_STATS DrvIwStats, *pDrvIwStats = &DrvIwStats;


	GET_PAD_FROM_NET_DEV(pAd, net_dev);


	/*DBGPRINT(RT_DEBUG_TRACE, ("rt28xx_get_wireless_stats --->\n"));*/

	pDrvIwStats->priv_flags = RT_DEV_PRIV_FLAGS_GET(net_dev);
	pDrvIwStats->dev_addr = (PUCHAR)net_dev->dev_addr;

	if (RTMP_DRIVER_IW_STATS_GET(pAd, pDrvIwStats) != NDIS_STATUS_SUCCESS)
		return NULL;

	pStats = (struct iw_statistics *)(pDrvIwStats->pStats);
	pStats->status = 0; /* Status - device dependent for now */

#if 0 /* os abl move */
#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
		pAd->iw_stats.qual.qual = ((pAd->Mlme.ChannelQuality * 12)/10 + 10);
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
	{
		if (pMacEntry != NULL)
			pAd->iw_stats.qual.qual = ((pMacEntry->ChannelQuality * 12)/10 + 10);
	}
#endif /* CONFIG_AP_SUPPORT */

	if(pAd->iw_stats.qual.qual > 100)
		pAd->iw_stats.qual.qual = 100;

#ifdef CONFIG_STA_SUPPORT
	if (pAd->OpMode == OPMODE_STA)
	{
		pAd->iw_stats.qual.level =
			RTMPMaxRssi(pAd, pAd->StaCfg.RssiSample.AvgRssi[0],
							pAd->StaCfg.RssiSample.AvgRssi[1],
							pAd->StaCfg.RssiSample.AvgRssi[2]);
	}
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
	if (pAd->OpMode == OPMODE_AP)
	{
		if (pMacEntry != NULL)
			pAd->iw_stats.qual.level =
				RTMPMaxRssi(pAd, pMacEntry->RssiSample.AvgRssi[0],
								pMacEntry->RssiSample.AvgRssi[1],
								pMacEntry->RssiSample.AvgRssi[2]);
	}
#endif /* CONFIG_AP_SUPPORT */

#if 1
#ifdef CONFIG_AP_SUPPORT
	pAd->iw_stats.qual.noise = RTMPMaxRssi(pAd, pAd->ApCfg.RssiSample.AvgRssi[0],
                                                        pAd->ApCfg.RssiSample.AvgRssi[1],
                                                        pAd->ApCfg.RssiSample.AvgRssi[2]) -
                                                        RTMPMinSnr(pAd, pAd->ApCfg.RssiSample.AvgSnr[0],
                                                        pAd->ApCfg.RssiSample.AvgSnr[1]);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	pAd->iw_stats.qual.noise = RTMPMaxRssi(pAd, pAd->StaCfg.RssiSample.AvgRssi[0],
							pAd->StaCfg.RssiSample.AvgRssi[1],
							pAd->StaCfg.RssiSample.AvgRssi[2]) -
							RTMPMinSnr(pAd, pAd->StaCfg.RssiSample.AvgSnr[0],
							pAd->StaCfg.RssiSample.AvgSnr[1]);
#endif /* CONFIG_STA_SUPPORT */

#ifdef P2P_SUPPORT
	if ((RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_P2P) && P2P_CLI_ON(pAd))
	{
		if (pMacEntry != NULL)
		{
			pAd->iw_stats.qual.qual = ((pMacEntry->ChannelQuality * 12)/10 + 10);

			if(pAd->iw_stats.qual.qual > 100)
				pAd->iw_stats.qual.qual = 100;

			pAd->iw_stats.qual.level =
				RTMPMaxRssi(pAd, pMacEntry->RssiSample.AvgRssi[0],
								pMacEntry->RssiSample.AvgRssi[1],
								pMacEntry->RssiSample.AvgRssi[2]);

			pAd->iw_stats.qual.noise = RTMPMaxRssi(pAd, pAd->ApCfg.RssiSample.AvgRssi[0],
									pAd->ApCfg.RssiSample.AvgRssi[1],
									pAd->ApCfg.RssiSample.AvgRssi[2]) -
									RTMPMinSnr(pAd, pAd->ApCfg.RssiSample.AvgSnr[0],
									pAd->ApCfg.RssiSample.AvgSnr[1]);
		}
	}
#endif /* P2P_SUPPORT */
#else
	pAd->iw_stats.qual.noise = pAd->BbpWriteLatch[66]; /* noise level (dBm) */

	pAd->iw_stats.qual.noise += 256 - 143;
#endif
#endif /* 0 */

	pStats->qual.updated = 1;     /* Flags to know if updated */
#ifdef IW_QUAL_DBM
	pStats->qual.updated |= IW_QUAL_DBM;	/* Level + Noise are dBm */
#endif /* IW_QUAL_DBM */
	pStats->qual.qual = pDrvIwStats->qual;
	pStats->qual.level = pDrvIwStats->level;
	pStats->qual.noise = pDrvIwStats->noise;
	pStats->discard.nwid = 0;     /* Rx : Wrong nwid/essid */
	pStats->miss.beacon = 0;      /* Missed beacons/superframe */

	/*DBGPRINT(RT_DEBUG_TRACE, ("<--- rt28xx_get_wireless_stats\n"));*/
	return pStats;
}
#endif /* WIRELESS_EXT */


INT rt28xx_ioctl(PNET_DEV net_dev, struct ifreq *rq, INT cmd)
{
	VOID *pAd = NULL;
	INT ret = 0;
	ULONG OpMode;

	GET_PAD_FROM_NET_DEV(pAd, net_dev);
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	RTMP_DRIVER_OP_MODE_GET(pAd, &OpMode);

#ifdef CONFIG_AP_SUPPORT
/*	IF_DEV_CONFIG_OPMODE_ON_AP(pAd) */
	RT_CONFIG_IF_OPMODE_ON_AP(OpMode)
	{		
		ret = rt28xx_ap_ioctl(net_dev, rq, cmd);
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
/*	IF_DEV_CONFIG_OPMODE_ON_STA(pAd) */
	RT_CONFIG_IF_OPMODE_ON_STA(OpMode)
	{
#ifdef P2P_SUPPORT
		if (RTMP_DRIVER_P2P_INF_CHECK(pAd, RT_DEV_PRIV_FLAGS_GET(net_dev)) == NDIS_STATUS_SUCCESS)
			ret = rt28xx_ap_ioctl(net_dev, rq, cmd);
		else
#endif /* P2P_SUPPORT */
		ret = rt28xx_sta_ioctl(net_dev, rq, cmd);
	}
#endif /* CONFIG_STA_SUPPORT */

	return ret;
}


/*
    ========================================================================

    Routine Description:
        return ethernet statistics counter

    Arguments:
        net_dev                     Pointer to net_device

    Return Value:
        net_device_stats*

    Note:

    ========================================================================
*/
struct net_device_stats *RT28xx_get_ether_stats(struct net_device *net_dev)
{
    VOID *pAd = NULL;
	struct net_device_stats *pStats;

	if (net_dev)
		GET_PAD_FROM_NET_DEV(pAd, net_dev);

	if (pAd)
	{
		RT_CMD_STATS DrvStats, *pDrvStats = &DrvStats;

#ifdef RELEASE_EXCLUDE
	    DBGPRINT(RT_DEBUG_INFO, ("RT28xx_get_ether_stats --->\n"));
#endif /* RELEASE_EXCLUDE */

		//assign net device for RTMP_DRIVER_INF_STATS_GET()
		pDrvStats->pNetDev = net_dev;
		RTMP_DRIVER_INF_STATS_GET(pAd, pDrvStats);

		pStats = (struct net_device_stats *)(pDrvStats->pStats);
		pStats->rx_packets = pDrvStats->rx_packets;
		pStats->tx_packets = pDrvStats->tx_packets;

		pStats->rx_bytes = pDrvStats->rx_bytes;
		pStats->tx_bytes = pDrvStats->tx_bytes;

		pStats->rx_errors = pDrvStats->rx_errors;
		pStats->tx_errors = pDrvStats->tx_errors;

		pStats->rx_dropped = 0;
		pStats->tx_dropped = 0;

	    pStats->multicast = pDrvStats->multicast;
	    pStats->collisions = pDrvStats->collisions;

	    pStats->rx_length_errors = 0;
	    pStats->rx_over_errors = pDrvStats->rx_over_errors;
	    pStats->rx_crc_errors = 0;/*pAd->WlanCounters.FCSErrorCount;     // recved pkt with crc error */
	    pStats->rx_frame_errors = pDrvStats->rx_frame_errors;
	    pStats->rx_fifo_errors = pDrvStats->rx_fifo_errors;
	    pStats->rx_missed_errors = 0;                                            /* receiver missed packet */

	    /* detailed tx_errors */
	    pStats->tx_aborted_errors = 0;
	    pStats->tx_carrier_errors = 0;
	    pStats->tx_fifo_errors = 0;
	    pStats->tx_heartbeat_errors = 0;
	    pStats->tx_window_errors = 0;

	    /* for cslip etc */
	    pStats->rx_compressed = 0;
	    pStats->tx_compressed = 0;

		return pStats;
	}
	else
    	return NULL;
}


BOOLEAN RtmpPhyNetDevExit(VOID *pAd, PNET_DEV net_dev)
{
#ifdef MESH_SUPPORT
	RTMP_Mesh_Remove(pAd);
#endif /* MESH_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#if defined(P2P_APCLI_SUPPORT) || defined(RT_CFG80211_P2P_CONCURRENT_DEVICE) || defined(CFG80211_MULTI_STA)
#else
	/* remove all AP-client virtual interfaces. */
	RT28xx_ApCli_Remove(pAd);
#endif /* P2P_APCLI_SUPPORT */
#endif /* APCLI_SUPPORT */

#ifdef WDS_SUPPORT
	/* remove all WDS virtual interfaces. */
	RT28xx_WDS_Remove(pAd);
#endif /* WDS_SUPPORT */

#ifdef MBSS_SUPPORT
#if defined(P2P_APCLI_SUPPORT) || defined(RT_CFG80211_P2P_SUPPORT) || defined(CFG80211_MULTI_STA)

#else
	RT28xx_MBSS_Remove(pAd);
#endif /* P2P_APCLI_SUPPORT */
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_SNIFFER_SUPPORT
	RT28xx_Monitor_Remove(pAd);
#endif	/* CONFIG_SNIFFER_SUPPORT */

#ifdef RT_CFG80211_SUPPORT
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
#ifndef RT_CFG80211_P2P_STATIC_CONCURRENT_DEVICE
	RTMP_CFG80211_AllVirtualIF_Remove(pAd);
#endif /* RT_CFG80211_P2P_STATIC_CONCURRENT_DEVICE */
	RTMP_CFG80211_DummyP2pIf_Remove(pAd);
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */
#ifdef CFG80211_MULTI_STA
	RTMP_CFG80211_MutliStaIf_Remove(pAd);
#endif /* CFG80211_MULTI_STA */
#else
#ifdef P2P_SUPPORT
	RTMP_P2P_Remove(pAd);
#endif /* P2P_SUPPORT */
#endif /* RT_CFG80211_SUPPORT */

#ifdef INF_PPA_SUPPORT
	RTMP_DRIVER_INF_PPA_EXIT(pAd);
#endif /* INF_PPA_SUPPORT */

	/* Unregister network device */
	if (net_dev != NULL)
	{
		DBGPRINT(RT_DEBUG_OFF,
			("RtmpOSNetDevDetach(): RtmpOSNetDeviceDetach(), dev->name=%s!\n",
			net_dev->name));
		/* Remove wifi dirver to lock the rtnl_lock here.
		*  It will cause kernel crash sometimes.
		*  The lock action leave to kernel.
		*/
		RtmpOSNetDevDetach(net_dev);
	}

	return TRUE;

}


/*******************************************************************************

	Device IRQ related functions.

 *******************************************************************************/
int RtmpOSIRQRequest(IN PNET_DEV pNetDev)
{
#if defined(RTMP_PCI_SUPPORT) || defined(RTMP_RBUS_SUPPORT)
	struct net_device *net_dev = pNetDev;
#endif
	ULONG infType;
	VOID *pAd = NULL;
	int retval = 0;

	GET_PAD_FROM_NET_DEV(pAd, pNetDev);

	ASSERT(pAd);

	RTMP_DRIVER_INF_TYPE_GET(pAd, &infType);

#ifdef RTMP_PCI_SUPPORT
	if (infType == RTMP_DEV_INF_PCI || infType == RTMP_DEV_INF_PCIE)
	{
		struct pci_dev *pci_dev;
/*		POS_COOKIE _pObj = (POS_COOKIE)(pAd->OS_Cookie); */
/*		RTMP_MSI_ENABLE(pAd); */
		RTMP_DRIVER_PCI_MSI_ENABLE(pAd, &pci_dev);

		retval = request_irq(pci_dev->irq,  rt2860_interrupt, SA_SHIRQ, (net_dev)->name, (net_dev));
		if (retval != 0)
			DBGPRINT(RT_DEBUG_OFF, ("RT2860: request_irq  ERROR(%d)\n", retval));
	}
#endif /* RTMP_PCI_SUPPORT */

#ifdef RTMP_RBUS_SUPPORT
	if (infType == RTMP_DEV_INF_RBUS)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
		if ((retval = request_irq(net_dev->irq, rt2860_interrupt, IRQF_SHARED, net_dev->name ,net_dev)))
#else
		if ((retval = request_irq(net_dev->irq,rt2860_interrupt, SA_INTERRUPT, net_dev->name ,net_dev)))
#endif
		{
			DBGPRINT(RT_DEBUG_OFF, ("RT2860: request_irq  ERROR(%d)\n", retval));
		}
	}
#endif /* RTMP_RBUS_SUPPORT */

	return retval;
}


#ifdef WDS_SUPPORT
/*
    ========================================================================

    Routine Description:
        return ethernet statistics counter

    Arguments:
        net_dev                     Pointer to net_device

    Return Value:
        net_device_stats*

    Note:

    ========================================================================
*/
struct net_device_stats *RT28xx_get_wds_ether_stats(
    IN PNET_DEV net_dev)
{
    VOID *pAd = NULL;
/*	INT WDS_apidx = 0,index; */
	struct net_device_stats *pStats;
	RT_CMD_STATS WdsStats, *pWdsStats = &WdsStats;

	if (net_dev) {
		GET_PAD_FROM_NET_DEV(pAd, net_dev);
	}

/*	if (RT_DEV_PRIV_FLAGS_GET(net_dev) == INT_WDS) */
	{
		if (pAd)
		{
#if 0 /* os abl move */
			/*struct net_device_stats	stats; */
			for(index = 0; index < MAX_WDS_ENTRY; index++)
			{
				if (pAd->WdsTab.WdsEntry[index].dev == net_dev)
				{
					WDS_apidx = index;

					break;
				}
			}

			if(index == MAX_WDS_ENTRY)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): can not find wds I/F\n", __FUNCTION__));
				return NULL;
			}
#endif /* 0 */

			pWdsStats->pNetDev = net_dev;
			if (RTMP_COM_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_WDS_STATS_GET,
					0, pWdsStats, RT_DEV_PRIV_FLAGS_GET(net_dev)) != NDIS_STATUS_SUCCESS)
				return NULL;

			pStats = (struct net_device_stats *)pWdsStats->pStats; /*pAd->stats; */

			pStats->rx_packets = pWdsStats->rx_packets; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.ReceivedFragmentCount.QuadPart; */
			pStats->tx_packets = pWdsStats->tx_packets; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.TransmittedFragmentCount.QuadPart; */

			pStats->rx_bytes = pWdsStats->rx_bytes; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.ReceivedByteCount; */
			pStats->tx_bytes = pWdsStats->tx_bytes; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.TransmittedByteCount; */

			pStats->rx_errors = pWdsStats->rx_errors; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.RxErrorCount; */
			pStats->tx_errors = pWdsStats->tx_errors; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.TxErrors; */

			pStats->rx_dropped = 0;
			pStats->tx_dropped = 0;

	  		pStats->multicast = pWdsStats->multicast; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.MulticastReceivedFrameCount.QuadPart;   // multicast packets received */
	  		pStats->collisions = pWdsStats->collisions; /* Collision packets */

	  		pStats->rx_length_errors = 0;
	  		pStats->rx_over_errors = pWdsStats->rx_over_errors; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.RxNoBuffer;                   // receiver ring buff overflow */
	  		pStats->rx_crc_errors = 0;/*pAd->WlanCounters.FCSErrorCount;     // recved pkt with crc error */
	  		pStats->rx_frame_errors = 0; /* recv'd frame alignment error */
	  		pStats->rx_fifo_errors = pWdsStats->rx_fifo_errors; /*pAd->WdsTab.WdsEntry[WDS_apidx].WdsCounter.RxNoBuffer;                   // recv'r fifo overrun */
	  		pStats->rx_missed_errors = 0;                                            /* receiver missed packet */

	  		    /* detailed tx_errors */
	  		pStats->tx_aborted_errors = 0;
	  		pStats->tx_carrier_errors = 0;
	  		pStats->tx_fifo_errors = 0;
	  		pStats->tx_heartbeat_errors = 0;
	  		pStats->tx_window_errors = 0;

	  		    /* for cslip etc */
	  		pStats->rx_compressed = 0;
	  		pStats->tx_compressed = 0;

			return pStats;
		}
		else
			return NULL;
	}
/*	else */
/*    		return NULL; */
}
#endif /* WDS_SUPPORT */

