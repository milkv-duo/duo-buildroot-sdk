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
 ***************************************************************************

    Module Name:
    rt_main_end.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
*/

#include "rt_config.h"


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
int rt28xx_close(IN VOID *dev)
{
#ifdef SECOND_WIFI
	extern int	pcie_int_line;
#endif /* SECOND_WIFI */
	PNET_DEV net_dev = (PNET_DEV)dev;
	RTMP_ADAPTER	*pAd = NULL;
	POS_COOKIE		pOSCookie = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt28xx_close\n"));

	/* Sanity check for pAd */
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(net_dev);	
	if (pAd == NULL)
		return 0; /* close ok */

	/* Clear handle interrupts */
#ifdef SECOND_WIFI
	if (pAd->infType == RTMP_DEV_INF_PCIE)
		cyg_drv_interrupt_mask(pcie_int_line);
	else	
#endif /* SECOND_WIFI */
		cyg_drv_interrupt_mask(RTMP_INTERRUPT_INIC);

	/* Delete the message queue */
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;
	cyg_mbox_delete(pOSCookie->nettask_handle);

	RTMPDrvClose(pAd, net_dev);


	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt28xx_close\n"));
	
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
========================================================================
*/
int rt28xx_open(IN VOID *dev)
{				 
#ifdef SECOND_WIFI
	extern int	pcie_int_line;
#endif /* SECOND_WIFI */

	PNET_DEV net_dev = (PNET_DEV)dev;
	RTMP_ADAPTER	*pAd = NULL;
	POS_COOKIE		pOSCookie = NULL;    
	int i, retval = 0;

	/* Sanity check for pAd */
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(net_dev);
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free; So the net_dev->priv will be NULL in 2rd open */
		DBGPRINT(RT_DEBUG_ERROR, ("pAd is NULL!\n"));
		return -1;
	}

	RTMP_DRIVER_MCU_SLEEP_CLEAR(pAd);

	/* Init IRQ parameters */
	RTMP_DRIVER_IRQ_INIT(pAd);

	/* Create the message queue */
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;
	cyg_mbox_create(&pOSCookie->nettask_handle, &pOSCookie->nettask_mbox);

	/* Chip & other init */
	if (rt28xx_init(pAd, NULL, NULL) == FALSE)
		goto err;

#ifdef MBSS_SUPPORT
	RT28xx_MBSS_Init(pAd, (PNET_DEV)net_dev);
#endif /* MBSS_SUPPORT */

#ifdef WDS_SUPPORT
	RT28xx_WDS_Init(pAd, (PNET_DEV)net_dev);
#endif /* WDS_SUPPORT */

#ifdef APCLI_SUPPORT
	RT28xx_ApCli_Init(pAd, (PNET_DEV)net_dev);
#endif /* APCLI_SUPPORT */

	RTMPDrvOpen(pAd);

	/* Set handle interrupts */
#ifdef SECOND_WIFI	
	if (pAd->infType == RTMP_DEV_INF_PCIE)
		cyg_drv_interrupt_unmask(pcie_int_line);
	else
#endif /* SECOND_WIFI */
		cyg_drv_interrupt_unmask(RTMP_INTERRUPT_INIC);

	return (retval);

err:
	RTMP_DRIVER_IRQ_RELEASE(pAd);
	return (-1);
}


/*
========================================================================
Routine Description:
	Early check if the packet has enough packet headeroom for latter handling.

Arguments:
	pEndObj		the end object.
	pMblk		the pointer refer to a packet buffer
	
Return Value:
	packet buffer pointer:	if sanity check success.
	NULL:				if failed

Note:
	This function is the entry point of Tx Path for Os delivery packet to 
	our driver. You only can put OS-depened & STA/AP common handle procedures 
	in here.
========================================================================
*/

static inline void RtmpVxNetPktCheck(
	IN PRTMP_ADAPTER pAd)
/*static inline M_BLK_ID RtmpVxNetPktCheck( */
	/*IN END_OBJ *pEndObj, */
	/*IN M_BLK_ID pMblk) */
{
}

/*
========================================================================
Routine Description:
    The entry point for Linux kernel sent packet to our driver.

Arguments:
    sk_buff *skb		the pointer refer to a sk_buffer.

Return Value:
    0					

Note:
	This function is the entry point of Tx Path for Os delivery packet to 
	our driver. You only can put OS-depened & STA/AP common handle procedures 
	in here.
========================================================================
*/
int rt28xx_packet_xmit(void *pPacketSrc)
{
	PECOS_PKT_BUFFER pPacket = (PECOS_PKT_BUFFER)pPacketSrc;
	PNET_DEV net_dev = pPacket->net_dev;
	PRTMP_ADAPTER   pAd;
	struct wifi_dev *wdev;

	wdev = RTMP_OS_NETDEV_GET_WDEV(net_dev);
	ASSERT(wdev);

	return RTMPSendPackets((NDIS_HANDLE)wdev, (PPNDIS_PACKET) &pPacket, 1,
							pPacket->pktLen, NULL);	
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
int rt28xx_send_packets(PECOS_PKT_BUFFER skb, PNET_DEV ndev)
{
	if (!(RTMP_OS_NETDEV_STATE_RUNNING(ndev)))
	{
		RELEASE_NDIS_PACKET(NULL, (PNDIS_PACKET)skb, NDIS_STATUS_FAILURE);
		return 0;
	}

	skb->net_dev = ndev;
	NdisZeroMemory((PUCHAR)&skb->cb[CB_OFF], 26);
	MEM_DBG_PKT_ALLOC_INC(skb);

	return rt28xx_packet_xmit(skb);
}



void tbtt_tasklet(unsigned long data)
{

/*#define MAX_TX_IN_TBTT		(16) */

#ifdef CONFIG_AP_SUPPORT
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

#ifdef RTMP_MAC_PCI
	if (pAd->OpMode == OPMODE_AP)
	{
		/* update channel utilization */
		QBSS_LoadUpdate(pAd, 0);

	}
#endif /* RTMP_MAC_PCI */

	if (pAd->OpMode == OPMODE_AP)
	{
		/* */
		/* step 7 - if DTIM, then move backlogged bcast/mcast frames from PSQ to TXQ whenever DtimCount==0 */
		if (pAd->ApCfg.DtimCount == 0)
		{
			PQUEUE_ENTRY    pEntry;
			BOOLEAN			bPS = FALSE;
			UINT 			count = 0;
			unsigned long 		IrqFlags;
			
			RTMP_IRQ_LOCK(&pAd->irq_lock, IrqFlags);
			while (pAd->MacTab.McastPsQueue.Head)
			{
				bPS = TRUE;
				if (pAd->TxSwQueue[QID_AC_BE].Number <= (pAd->TxSwQMaxLen + MAX_PACKETS_IN_MCAST_PS_QUEUE))
				{
					pEntry = RemoveHeadQueue(&pAd->MacTab.McastPsQueue);
					/*if(pAd->MacTab.McastPsQueue.Number) */
					if (count)
					{
						RTMP_SET_PACKET_MOREDATA(pEntry, TRUE);
					}
					InsertHeadQueue(&pAd->TxSwQueue[QID_AC_BE], pEntry);
					count++;
				}
				else
				{
					break;
				}
			}
			RTMP_IRQ_UNLOCK(&pAd->irq_lock, IrqFlags);
						
			if (pAd->MacTab.McastPsQueue.Number == 0)
			{
				UINT bss_index;

				/* clear MCAST/BCAST backlog bit for all BSS */
				for(bss_index=BSS0; bss_index<pAd->ApCfg.BssidNum; bss_index++)
					WLAN_MR_TIM_BCMC_CLEAR(bss_index);
			}
			pAd->MacTab.PsQIdleCount = 0;

			/* Dequeue outgoing framea from TxSwQueue0..3 queue and process it */
			if (bPS == TRUE) 
			{
				// TODO: shiang-usw, modify the WCID_ALL to pMBss->tr_entry because we need to tx B/Mcast frame here!!
				RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, /*MAX_TX_IN_TBTT*/MAX_PACKETS_IN_MCAST_PS_QUEUE);
			}
		}

#if 0 //EddySingle
		if ((pAd->CommonCfg.Channel > 14)
			&& (pAd->CommonCfg.bIEEE80211H == 1)
			&& (pAd->CommonCfg.RadarDetect.RDMode == RD_SWITCHING_MODE))
		{
			DBGPRINT(RT_DEBUG_TRACE, ("%s():Channel Switching...(%d/%d)\n",
						__FUNCTION__, 
						pAd->CommonCfg.RadarDetect.CSCount,
						pAd->CommonCfg.RadarDetect.CSPeriod));
			
			pAd->CommonCfg.RadarDetect.CSCount++;
			if (pAd->CommonCfg.RadarDetect.CSCount >= pAd->CommonCfg.RadarDetect.CSPeriod)
			{
#ifdef DFS_HARDWARE_SUPPORT
				pAd->CommonCfg.DFSAPRestart=1;
				schedule_dfs_task(pAd);
#else
				APStop(pAd);
				APStartUp(pAd);
#endif /* NEW_DFS */
			}
		}
#endif
	}
#endif /* CONFIG_AP_SUPPORT */
}


INT rt28xx_ioctl(
	IN	PNET_DEV	endDev, 
	IN	caddr_t		data,
	IN	int			cmd)
{
	RTMP_ADAPTER	*pAd = NULL;
	INT				ret = 0;
	ULONG OpMode;

	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(endDev);
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

	RTMP_DRIVER_OP_MODE_GET(pAd, &OpMode);

#ifdef CONFIG_AP_SUPPORT
	RT_CONFIG_IF_OPMODE_ON_AP(OpMode)
	{
		ret = rt28xx_ap_ioctl(endDev, cmd, data);
	}
#endif /* CONFIG_AP_SUPPORT */


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
NET_DEV_STATS *RT28xx_get_ether_stats(PNET_DEV net_dev)
{
	return NULL;
}

/*
========================================================================
Routine Description:
    Allocate memory for adapter control block.

Arguments:
    pAd					Pointer to our adapter

Return Value:
	NDIS_STATUS_SUCCESS
	NDIS_STATUS_FAILURE
	NDIS_STATUS_RESOURCES

Note:
========================================================================
*/
NDIS_STATUS AdapterBlockAllocateMemory(
	IN	PVOID					handle,
	OUT	PVOID					*ppAd,
	IN	UINT32					SizeOfpAd)
{
	*ppAd = (PVOID)kmalloc(sizeof(RTMP_ADAPTER), GFP_ATOMIC);

	if (*ppAd) 
	{
		NdisZeroMemory((PCHAR) *ppAd, sizeof(RTMP_ADAPTER));
		((PRTMP_ADAPTER)*ppAd)->OS_Cookie = handle;
		return (NDIS_STATUS_SUCCESS);
	}
	else
	{
		return (NDIS_STATUS_FAILURE);
	}
}

BOOLEAN RtmpPhyNetDevExit(
	IN RTMP_ADAPTER *pAd, 
	IN PNET_DEV net_dev)
{
	/*END_OBJ *pEndDev; */

	/*pEndDev = (END_OBJ *)net_dev; */


#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	/* remove all AP-client virtual interfaces. */
	RT28xx_ApCli_Remove(pAd);
#endif /* APCLI_SUPPORT */

#ifdef WDS_SUPPORT
	/* remove all WDS virtual interfaces. */
	RT28xx_WDS_Remove(pAd);
#endif /* WDS_SUPPORT */

#ifdef MBSS_SUPPORT
	RT28xx_MBSS_Remove(pAd);
#endif /* MBSS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	/* Unregister network device */
	if (net_dev != NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RtmpOSNetDevDetach(): RtmpOSNetDeviceDetach(), dev->name=%s!\n", 
					net_dev->dev_name));
		RtmpOSNetDevDetach(net_dev);
	}

	return TRUE;
}

