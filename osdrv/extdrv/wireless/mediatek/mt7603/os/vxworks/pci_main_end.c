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
    2860_main_end.c

    Abstract:
    Create and register network interface for PCI based chipsets in VxWorks platform.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#include "rt_config.h"



/* */
/* Ralink 28xx based PCI device table, include all supported chipsets */
/* */


/* */
/* Our PCI driver structure */
/* */


/***************************************************************************
 *
 *	PCI device initialization related procedures.
 *
 ***************************************************************************/
static INT  rt2860_init_module(VOID)
{
	return 0;
}


/* */
/* Driver module unload function */
/* */
static VOID  rt2860_cleanup_module(VOID)
{
	return;
}


/* */
/* PCI device probe & initialization function */
/* */
static INT rt2860_probe()
{
	return 0;
}



static VOID  rt2860_remove_one()
{
	return;
}

#if 0
/*
========================================================================
Routine Description:
    Check the chipset vendor/product ID.

Arguments:
    _dev_p				Point to the PCI or USB device

Return Value:
    TRUE				Check ok
	FALSE				Check fail

Note:
========================================================================
*/
BOOLEAN RT28XXChipsetCheck(
	IN void *_dev_p)
{
	/* always TRUE */
	return TRUE;
}
#endif /* Unused */

END_OBJ* RtmpVxPciEndLoad( /* RT2880 PCI */
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
	csr_addr = (unsigned long) initToken->csrAddr;
	

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
	RtmpRaDevCtrlInit(pAd, RTMP_DEV_INF_PCI);
	handle->unitNum = initToken->phyNum;
	
	/* Here are the RTMP_ADAPTER structure with rbus-bus specific parameters. */
	pAd->CSRBaseAddress = (PUCHAR)csr_addr;


/*NetDevInit============================================== */
	/*it's time to register the net device to vxworks kernel. */
#if 0
	pENDDev = RtmpPhyNetDevInit(pAd, &netDevHook);
	if (pENDDev == NULL)
	{
		printf("RtmpOSNetDevCreate() failed for main physical net device!\n");
		goto err_out_free_radev;
	}
#else
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
	              initToken->phyNum, &RtmpMAINEndFuncTable, "ra1") == ERROR)) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not init net device %s!\n", "ra1"));
		goto err_out_free_netpool;
	}

	if ((END_MIB_INIT(pENDDev, M2_ifType_ethernet_csmacd,
	              &ZERO_MAC_ADDR[0],  6, ETHERMTU, END_SPEED) == ERROR))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not init MIB of net device %s!\n", "ra1"));
		goto err_out_free_netpool;
	}

	/*END_OBJ_READY(pENDDev, IFF_NOTRAILERS |IFF_SIMPLEX |IFF_BROADCAST |IFF_ALLMULTI); */
	END_OBJ_READY(pENDDev, IFF_NOTRAILERS |IFF_BROADCAST |IFF_MULTICAST);
	
	DBGPRINT(RT_DEBUG_TRACE, ("RtmpVxRbusEndLoad(): Allocate END object success!\n"));

#endif

	DBGPRINT(RT_DEBUG_TRACE, ("%s: at CSR addr 0x%1x\n", pENDDev->devObject.name, (ULONG)csr_addr));

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

