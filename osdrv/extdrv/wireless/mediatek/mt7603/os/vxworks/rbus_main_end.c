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
    rbus_main_end.c

    Abstract:
    Create and register network interface for RBUS based chipsets in VxWorks platform.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#include "rt_config.h"



END_OBJ* RtmpVxRbusEndLoad(
	IN RTMP_VX_INIT_STRING *initToken)
{
	END_OBJ *pENDDev;
	ULONG						csr_addr;
	int							status;
	POS_COOKIE					handle = NULL;
	RTMP_ADAPTER				*pAd;
	/*unsigned int				dev_irq; */
	/*RTMP_OS_NETDEV_OP_HOOK	netDevHook; */
	

	DBGPRINT(RT_DEBUG_TRACE, ("===> RtmpVxRbusEndLoad()\n"));

	
/*RtmpRaBusInit============================================ */
	/* map physical address to virtual address for accessing register */
	csr_addr = (unsigned long) RTMP_MAC_CSR_ADDR;
	

/*RtmpDevInit============================================== */
	/* Allocate RTMP_ADAPTER adapter structure */
	handle = (POS_COOKIE)malloc(sizeof(struct os_cookie));
	if (!handle)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Allocate memory for os_cookie failed!\n"));
		goto err_out;
	}
	memset((PUCHAR)handle, 0, sizeof(struct os_cookie));

	status = RTMPAllocAdapterBlock((PVOID)handle, &pAd);
	if (status != NDIS_STATUS_SUCCESS)
	{
		free(handle);
		goto err_out;
	}
	RtmpRaDevCtrlInit(pAd, RTMP_DEV_INF_RBUS);
	handle->unitNum = initToken->phyNum;
	
	/* Here are the RTMP_ADAPTER structure with rbus-bus specific parameters. */
	pAd->CSRBaseAddress = (PUCHAR)csr_addr;


/*NetDevInit============================================== */
	/*it's time to register the net device to vxworks kernel. */
	pENDDev = (END_OBJ *)malloc(sizeof(END_OBJ));
	if (pENDDev == NULL)
	{
		DBGPRINT(RT_DEBUG_TRACE, (" malloc failed for main physical net device!\n"));
		goto err_out_free_radev;
	}
	memset((PUCHAR)pENDDev, 0, sizeof(END_OBJ));
	pAd->net_dev = pENDDev;
	
	status = RtmpVxNetPoolInit(handle);
	if (status == NDIS_STATUS_FAILURE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Allocate NetPool failed!\n"));
		goto err_out_free_netdev;
	}
	/* We only assign the Tx pool for netdevice due to we use the Rx pool internal. */
	pENDDev->pNetPool = handle->pNetPool[RTMP_NETPOOL_TX];
	
#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev = pENDDev;
#endif /* CONFIG_AP_SUPPORT */

	if ((END_OBJ_INIT(pENDDev, (DEV_OBJ *)pAd, &initToken->devName[0],
	              initToken->phyNum, &RtmpMAINEndFuncTable, "ra0") == ERROR)) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not init net device %s!\n", "ra0"));
		goto err_out_free_netpool;
	}
	
	if ((END_MIB_INIT(pENDDev, M2_ifType_ethernet_csmacd,
	              &ZERO_MAC_ADDR[0],  6, ETHERMTU, END_SPEED) == ERROR))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not init MIB of net device %s!\n", "ra0"));
		goto err_out_free_netpool;
	}

	/*END_OBJ_READY(pENDDev, IFF_NOTRAILERS |IFF_SIMPLEX |IFF_BROADCAST |IFF_ALLMULTI); */
	END_OBJ_READY(pENDDev, IFF_NOTRAILERS |IFF_BROADCAST |IFF_MULTICAST);
	
	DBGPRINT(RT_DEBUG_TRACE, ("RtmpVxRbusEndLoad(): Create END Object(%s) success with CSR addr(0x%lx)\n", 
								pENDDev->devObject.name, (ULONG)csr_addr));

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt2880_probe\n"));

	return pENDDev;

err_out_free_netpool:
	RtmpVxNetPoolFree(handle);
	
err_out_free_netdev:
	if (pENDDev)
		RtmpOSNetDevFree(pENDDev);

err_out_free_radev:
	/* free RTMP_ADAPTER strcuture and os_cookie*/
	if (pAd)
		RTMPFreeAdapter(pAd);
		
err_out:
	return NULL;
	
}


VOID rt2880_END_Unload(END_OBJ *endObj)
{
	RTMP_ADAPTER *pAd;


	if (endObj == NULL)
		return;
	
	pAd = endObj->devObject.pDevice;
	if (pAd != NULL)
	{
#if defined (AP_LED) || defined (STA_LED)
	extern RALINK_TIMER_STRUCT LedCheckTimer;
	extern unsigned char CheckTimerEbl;
	{
		BOOLEAN  Cancelled;
		RTMPCancelTimer(&LedCheckTimer, &Cancelled);
		CheckTimerEbl=0;
	}
#endif /* (AP_LED) || (STA_LED) */

		RtmpPhyNetDevExit(pAd, endObj);

		RtmpRaDevCtrlExit(pAd);
		RTMPFreeAdapterBlock(pAd);
	}
	else
	{
		RtmpOSNetDevDetach(endObj);
	}
	
	/* Free the root net_device. */
	RtmpOSNetDevFree(endObj);
	
}

