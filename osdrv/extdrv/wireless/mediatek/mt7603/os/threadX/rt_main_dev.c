/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
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
   Shiang		Aug/21/09		Initial version for ThreadX
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
void MainVirtualIF_close(IN PNET_DEV net_dev)
{
	RTMP_ADAPTER *pAd = RTMP_OS_NETDEV_GET_PRIV(net_dev);

	if (!pAd)
		return;


#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].bcn_buf.bBcnSntReq = FALSE;

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		/* kick out all STAs behind the bss. */
		MbssKickOutStas(pAd, MAIN_MBSSID, REASON_DISASSOC_INACTIVE);
	}

	APMakeAllBssBeacon(pAd);
	APUpdateAllBeaconFrame(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef MESH_SUPPORT
	MeshMakeBeacon(pAd, MESH_BEACON_IDX(pAd));
	MeshUpdateBeaconFrame(pAd, MESH_BEACON_IDX(pAd));
#endif /* MESH_SUPPORT */

	VIRTUAL_IF_DOWN(pAd);

	RT_MOD_DEC_USE_COUNT();

	return;
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
RTP_BOOL MainVirtualIF_open(IN PIFACE pi)
{
	RTMP_ADAPTER *pAd = RTMP_OS_NETDEV_GET_PRIV(pi);

	
	DBGPRINT(RT_DEBUG_TRACE, ("Into MainVirtualIF_open()!pAd=0x%x\n", pAd));
	if (pAd == NULL)
		return RTP_FALSE;

	pAd->net_dev = pi;
	RtmpPI = pi;

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev = pi;
	pAd->ApCfg.MBSSID[MAIN_MBSSID].bcn_buf.bBcnSntReq = TRUE;
#endif /* CONFIG_AP_SUPPORT */

	if (VIRTUAL_IF_UP(pAd) != 0)
	{
		dc_log_printf("MainVirtualIF_Open failed\n");
		return RTP_FALSE;
	}

	dc_log_printf("MainVirtualIF_Open success!\n");

	return RTP_TRUE;
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
		QBSS_LoadUpdate(pAd);

#ifdef DOT11K_RRM_SUPPORT
		RRM_QuietUpdata(pAd);
#endif /* DOT11K_RRM_SUPPORT */
	}
#endif /* RTMP_MAC_PCI */

	if (pAd->OpMode == OPMODE_AP)
	{
		/* */
		/* step 7 - if DTIM, then move backlogged bcast/mcast frames from PSQ to TXQ whenever DtimCount==0 */
#if 0    
		/* NOTE: This updated BEACON frame will be sent at "next" TBTT instead of at cureent TBTT. The reason is */
		/*       because ASIC already fetch the BEACON content down to TX FIFO before driver can make any */
		/*       modification. To compenstate this effect, the actual time to deilver PSQ frames will be */
		/*       at the time that we wrapping around DtimCount from 0 to DtimPeriod-1 */
		if ((pAd->ApCfg.DtimCount + 1) == pAd->ApCfg.DtimPeriod)
#else
		if (pAd->ApCfg.DtimCount == 0)
#endif
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
						RTMP_SET_PACKET_TXTYPE(pEntry, TX_LEGACY_FRAME);
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
			
#ifdef RELEASE_EXCLUDE
			DBGPRINT(RT_DEBUG_INFO, ("DTIM=%d/%d, tx mcast/bcast out...\n",pAd->ApCfg.DtimCount,pAd->ApCfg.DtimPeriod));
#endif /* RELEASE_EXCLUDE */			
			
			if (pAd->MacTab.McastPsQueue.Number == 0)
			{			
                UINT bss_index;

                /* clear MCAST/BCAST backlog bit for all BSS */
                for(bss_index=BSS0; bss_index<pAd->ApCfg.BssidNum; bss_index++)
					WLAN_MR_TIM_BCMC_CLEAR(bss_index);
			}
			pAd->MacTab.PsQIdleCount = 0;

            if (bPS == TRUE) 
			{
				RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, /*MAX_TX_IN_TBTT*/MAX_PACKETS_IN_MCAST_PS_QUEUE);
			}
		}
	}
#endif /* CONFIG_AP_SUPPORT */
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
	IN PVOID	handle,
	OUT	PVOID	*ppAd,
	IN UINT32	SizeOfpAd)
{
	RTMP_DC_RAM_BLOCK_ELEMENT *pDcRamElement;

	pDcRamElement = &RtmpDCRamMgrPSBBuf[RTMP_DC_RAM_ID_DEVCTRL];
	if (pDcRamElement->bValid != TRUE)
		return NDIS_STATUS_FAILURE;

	*ppAd = pDcRamElement->baseAddr;
    	NdisZeroMemory(*ppAd, sizeof(RTMP_ADAPTER));
	((PRTMP_ADAPTER)*ppAd)->OS_Cookie = handle;

	RT_WIFI_DevCtrl = (UCHAR *)pDcRamElement->baseAddr;
		
	return (NDIS_STATUS_SUCCESS);
}


PNET_DEV RtmpPhyNetDevInit(
	IN RTMP_ADAPTER *pAd,
	IN RTMP_OS_NETDEV_OP_HOOK *pNetDevHook)
{
	PIFACE pi = NULL;
	
	/*RtmpOSNetDevCreate(pAd, INT_MAIN, 0, sizeof(struct mt_dev_priv), INF_MAIN_DEV_NAME); */
	NdisZeroMemory((unsigned char *)pNetDevHook, sizeof(RTMP_OS_NETDEV_OP_HOOK));
	pNetDevHook->open = MainVirtualIF_open;
	pNetDevHook->stop = MainVirtualIF_close;
	pNetDevHook->xmit = rt28xx_send_packets;
	/*pNetDevHook->ioctl = rt28xx_ioctl; */
	pNetDevHook->priv_flags = INT_MAIN;

	pNetDevHook->needProtcted = FALSE;
	memcpy(&pNetDevHook->devName[0],  "ra0", strlen("ra0"));
	
	/*
		For ThreadX, the NET_DEV structure is assigned when interface open. 
		So here we return "NULL" directly and the following assignment is move to "open"
	pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev = pi;
			pAd->net_dev = pi;

		For pAd, because we use static memory, so this is a dummy function now.
			RTMP_OS_NETDEV_SET_PRIV(net_dev, pAd);
	*/
	return NULL;
	
}


BOOLEAN RtmpPhyNetDevExit(
	IN RTMP_ADAPTER *pAd, 
	IN PNET_DEV net_dev)
{
	PIFACE pi;

	pi = (PIFACE)net_dev;

#ifdef MESH_SUPPORT
	RTMP_Mesh_Remove(pAd);
#endif /* MESH_SUPPORT */

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
					pi->pdev->device_name));
		RtmpOSNetDevDetach(net_dev);
	}

	return TRUE;
}

