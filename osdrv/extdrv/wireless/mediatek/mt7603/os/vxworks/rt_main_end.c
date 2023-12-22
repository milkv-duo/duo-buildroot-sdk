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
    rt_main_end.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
   Shiang		Aug/21/08		Initial version for VxWorks
*/

#include "rt_config.h"



/*
 * Declare our function table.  
 */
int MainVirtualIF_open(PNET_DEV net_dev);
int MainVirtualIF_close(PNET_DEV net_dev);

INT rt28xx_ioctl(PNET_DEV endDev, int cmd, caddr_t data);

static int rt28xx_send_packets(END_OBJ *pEndObj, M_BLK_ID pMblk);
static inline M_BLK_ID RtmpVxNetPktCheck(END_OBJ * pEndObj, M_BLK_ID pMblk);
	

STATUS RtmpVxNetDevMCastAdd(
	IN END_OBJ	*pEndObj,
	IN char		*pAddr);
	
STATUS RtmpVxNetDevMCastDel(
	IN END_OBJ	*pEndObj,
	IN char		*pAddr);

STATUS RtmpVxNetDevMCastGet(
	IN END_OBJ *pEndObj,
	IN MULTI_TABLE* pTable);


NET_FUNCS RtmpMAINEndFuncTable = {
    (FUNCPTR)MainVirtualIF_open,		/* STATUS (*start) (END_OBJ*); start the device. */
    (FUNCPTR)MainVirtualIF_close,		/* STATUS (*stop) (END_OBJ*); stop the device. */
    (FUNCPTR)sysRtmpEndUnLoad,		/* STATUS (*unload) (END_OBJ*); Unloading function for the driver.*/
    (FUNCPTR)rt28xx_ioctl,				/* int (*ioctl) (END_OBJ*, int, caddr_t); Ioctl function for the driver. */
    (FUNCPTR)rt28xx_send_packets,	/* STATUS (*send) (END_OBJ* , M_BLK_ID);Send function for the driver. */
    (FUNCPTR)RtmpVxNetDevMCastAdd,	/* STATUS (*mCastAddrAdd) (END_OBJ*, char*);Multicast add function.*/
    (FUNCPTR)RtmpVxNetDevMCastDel,	/* STATUS (*mCastAddrDel) (END_OBJ*, char*);Multicast delete function.  */
    (FUNCPTR)RtmpVxNetDevMCastGet,	/* STATUS (*mCastAddrGet) (END_OBJ*, MULTI_TABLE*);Multicast retrieve function . */
    NULL,								/* STATUS (*pollSend) (END_OBJ*, M_BLK_ID);Polling send function */
    NULL, 								/* STATUS (*pollRcv) (END_OBJ*, M_BLK_ID);Polling receive function */
    endEtherAddressForm,		/* M_BLK_ID (*formAddress) (); put address info into a NET_BUFFER */
    endEtherPacketDataGet,		/* STATUS (*packetDataGet) ();get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet,		/* STATUS (*addrGet) (M_BLK_ID, M_BLK_ID, M_BLK_ID, M_BLK_ID, M_BLK_ID); Get packet addresses. */
    NULL,								/* int (*endBind) (void*, void*, void*, long type); information exchange between */
									/* network service and network driver */
};


#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
/*
========================================================================
Routine Description:
    Send a packet to WLAN via MBSS interface.

Arguments:
    pEndObj	points to our adapter
    pMblk		packet buffer need to delivery

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
static int RtmpOSNetDevMBSSSend(
	IN END_OBJ		*pEndObj,
	IN M_BLK_ID     pMblk)
{
	M_BLK_ID	pNewMblk;

	pNewMblk = RtmpVxNetPktCheck(pEndObj, pMblk);
	if (pNewMblk == NULL)
		return -1;
	
	return rt28xx_send_packets((PNDIS_PACKET)pNewMblk, (PNET_DEV)pEndObj);

}

NET_FUNCS RtmpMBSSEndFuncTable = {
    (FUNCPTR)MBSS_VirtualIF_Open,        /* Function to start the device. */
    (FUNCPTR)MBSS_VirtualIF_Close,         /* Function to stop the device. */
    (FUNCPTR)sysRtmpEndUnLoad,       /* Unloading function for the driver.*/
    (FUNCPTR)rt28xx_ioctl,       /* Ioctl function for the driver. */
    (FUNCPTR)RtmpOSNetDevMBSSSend,         /* Send function for the driver. */
    (FUNCPTR)RtmpVxNetDevMCastAdd,     /* Multicast add function.*/
    (FUNCPTR)RtmpVxNetDevMCastDel,     /* Multicast delete function.  */
    (FUNCPTR)RtmpVxNetDevMCastGet,     /* Multicast retrieve function . */
    NULL,     /* Polling send function */
    NULL,     /* Polling receive function */
    endEtherAddressForm,            /* put address info into a NET_BUFFER */
    endEtherPacketDataGet,              /* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet,	 	/* Get packet addresses. */
    NULL                            /* Bind */
};
#endif /* MBSS_SUPPORT */


#ifdef APCLI_SUPPORT
/*
========================================================================
Routine Description:
    Send a packet to WLAN via Apcli interface.

Arguments:
    pEndObj	points to our adapter
    pMblk		packet buffer need to delivery

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
static int RtmpOSNetDevApCliSend(
	IN END_OBJ * pEndObj,
	IN M_BLK_ID     pMblk)
{
	M_BLK_ID	pNewMblk;

	pNewMblk = RtmpVxNetPktCheck(pEndObj, pMblk);
	if (pNewMblk == NULL)
		return -1;
	
	return rt28xx_send_packets((PNDIS_PACKET)pNewMblk, (PNET_DEV)pEndObj);

}


NET_FUNCS RtmpApCliEndFuncTable = {
    (FUNCPTR)ApCli_VirtualIF_Open,        /* Function to start the device. */
    (FUNCPTR)ApCli_VirtualIF_Close,         /* Function to stop the device. */
    (FUNCPTR)sysRtmpEndUnLoad,       /* Unloading function for the driver.*/
    (FUNCPTR)rt28xx_ioctl,       /* Ioctl function for the driver. */
    (FUNCPTR)RtmpOSNetDevApCliSend,         /* Send function for the driver. */
    (FUNCPTR)RtmpVxNetDevMCastAdd,     /* Multicast add function.*/
    (FUNCPTR)RtmpVxNetDevMCastDel,     /* Multicast delete function.  */
    (FUNCPTR)RtmpVxNetDevMCastGet,     /* Multicast retrieve function . */
    NULL,     /* Polling send function */
    NULL,     /* Polling receive function */
    endEtherAddressForm,            /* put address info into a NET_BUFFER */
    endEtherPacketDataGet,              /* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet,	 	/* Get packet addresses. */
    NULL                            /* Bind */
};
#endif /* APCLI_SUPPORT */

#ifdef WDS_SUPPORT
/*
========================================================================
Routine Description:
    Send a packet to WLAN via WDS interface.

Arguments:
    pEndObj	points to our adapter
    pMblk		packet buffer need to delivery

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
static int RtmpOSNetDevWDSSend(
	IN END_OBJ * pEndObj,
	IN M_BLK_ID     pMblk)
{
	M_BLK_ID	pNewMblk;

	pNewMblk = RtmpVxNetPktCheck(pEndObj, pMblk);
	if (pNewMblk == NULL)
		return -1;
	
	return rt28xx_send_packets((PNDIS_PACKET)pNewMblk, (PNET_DEV)pEndObj);

}


NET_FUNCS RtmpWDSEndFuncTable = {
    (FUNCPTR)WdsVirtualIF_open,        /* Function to start the device. */
    (FUNCPTR)WdsVirtualIF_close,         /* Function to stop the device. */
    (FUNCPTR)sysRtmpEndUnLoad,       /* Unloading function for the driver.*/
    (FUNCPTR)rt28xx_ioctl,       			/* Ioctl function for the driver. */
    (FUNCPTR)RtmpOSNetDevWDSSend,         /* Send function for the driver. */
    (FUNCPTR)RtmpVxNetDevMCastAdd,     /* Multicast add function.*/
    (FUNCPTR)RtmpVxNetDevMCastDel,     /* Multicast delete function.  */
    (FUNCPTR)RtmpVxNetDevMCastGet,     /* Multicast retrieve function . */
    NULL,     /* Polling send function */
    NULL,     /* Polling receive function */
    NULL,            /* put address info into a NET_BUFFER */
    NULL,              /* get pointer to data in NET_BUFFER */
    NULL,          /* Get packet addresses. */
    NULL                            /* Bind */
};
#endif /* WDS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#ifdef MESH_SUPPORT
/*
========================================================================
Routine Description:
    Send a packet to WLAN via WDS interface.

Arguments:
    pEndObj	points to our adapter
    pMblk		packet buffer need to delivery

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
static int RtmpOSNetDevMeshSend(
	IN END_OBJ * pEndObj,
	IN M_BLK_ID     pMblk)
{
	M_BLK_ID	pNewMblk;

	pNewMblk = RtmpVxNetPktCheck(pEndObj, pMblk);
	if (pNewMblk == NULL)
		return -1;
	
	return rt28xx_send_packets((PNDIS_PACKET)pNewMblk, (PNET_DEV)pEndObj);

}
NET_FUNCS RtmpMeshEndFuncTable = {
    (FUNCPTR)Mesh_VirtualIF_Open,        /* Function to start the device. */
    (FUNCPTR)Mesh_VirtualIF_Close,         /* Function to stop the device. */
    (FUNCPTR)sysRtmpEndUnLoad,       /* Unloading function for the driver.*/
    (FUNCPTR)rt28xx_ioctl,       /* Ioctl function for the driver. */
    (FUNCPTR)RtmpOSNetDevMeshSend,   /* Send function for the driver. */
    (FUNCPTR)RtmpVxNetDevMCastAdd,     /* Multicast add function.*/
    (FUNCPTR)RtmpVxNetDevMCastDel,     /* Multicast delete function.  */
    (FUNCPTR)RtmpVxNetDevMCastGet,     /* Multicast retrieve function . */
    NULL,     /* Polling send function */
    NULL,     /* Polling receive function */
    endEtherAddressForm,            /* put address info into a NET_BUFFER */
    endEtherPacketDataGet,              /* get pointer to data in NET_BUFFER */
    endEtherPacketAddrGet,	 	/* Get packet addresses. */
    NULL                           				/* Bind */
};
#endif /* MESH_SUPPORT */


/*---------------------------------------------------------------------*/
/* Prototypes of Functions Used                                        					     */
/*---------------------------------------------------------------------*/

/* public function prototype */
int rt28xx_close(IN VOID *pNetDev);
int rt28xx_open(IN  VOID *pNetDev);


/*
========================================================================

 RtmpVxNetDevMCastAdd - add the multicast address to the multicast list of the device

 	This routine add the multicast address to the list of whatever the driver.

 RETURNS: OK or ERROR.
========================================================================
*/
STATUS RtmpVxNetDevMCastAdd(
	IN END_OBJ	*pEndObj,
	IN char		*pAddr)
{
	STATUS	retVal;

	retVal = etherMultiAdd(&pEndObj->multiList, pAddr);

	if (retVal == ENETRESET)
	{
		/* FIXME: Currently we didn't implement the multicast filter list in our driver.*/
		pEndObj->nMulti++;
		retVal = OK;
	}

	return (retVal == OK) ? OK : ERROR;
}


/*
========================================================================

 RtmpVxNetDevMCastDel - Delete the multicast address list for the device

	 This routine delete the multicast list of whatever the driver is already listening for.

 RETURNS: OK or ERROR.
========================================================================
*/
STATUS RtmpVxNetDevMCastDel(
	IN END_OBJ	*pEndObj,
	IN char		*pAddr)
{
	STATUS	retVal;

	retVal = etherMultiDel(&pEndObj->multiList, pAddr);

	if (retVal == ENETRESET)
	{
		/* FIXME: Currently we didn't implement the multicast filter list in our driver.*/
		pEndObj->nMulti--;
		retVal = OK;
	}

	return (retVal == OK) ? OK : ERROR;
}


/*
========================================================================

 RtmpVxNetDevMCastGet - get the multicast address list for the device

 This routine gets the multicast list of whatever the driver
 is already listening for.

 RETURNS: OK or ERROR.
========================================================================
*/

STATUS RtmpVxNetDevMCastGet(
	IN END_OBJ *pEndObj,
	IN MULTI_TABLE* pTable)
{
    return (etherMultiGet (&pEndObj->multiList, pTable));
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
int MainVirtualIF_close(IN PNET_DEV net_dev)
{
	RTMP_ADAPTER *pAd = RTMP_OS_NETDEV_GET_PRIV(net_dev);

	if (pAd == NULL)
		return 0;


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
	1. if open fail, kernel will not call the close function.
	2. Free memory for
		(1) Mlme Memory Handler:		MlmeHalt()
		(2) TX & RX:					RTMPFreeTxRxRingMemory()
		(3) BA Reordering: 				ba_reordering_resource_release()
========================================================================
*/
int MainVirtualIF_open(IN PNET_DEV net_dev)
{
	RTMP_ADAPTER *pAd = RTMP_OS_NETDEV_GET_PRIV(net_dev);

	
	DBGPRINT(RT_DEBUG_TRACE, ("Into MainVirtualIF_open()!pAd=0x%x\n", pAd));
	if (pAd == NULL)
		return 0;

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].bcn_buf.bBcnSntReq = TRUE;
#endif /* CONFIG_AP_SUPPORT */
	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

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
int rt28xx_close(IN VOID *dev)
{
	END_OBJ *pEndObj = (struct end_object *)dev;
	RTMP_ADAPTER	*pAd = RTMP_OS_NETDEV_GET_PRIV(pEndObj);
	UINT32			i = 0;

#ifdef RTMP_MAC_USB
	/*DECLARE_WAIT_QUEUE_HEAD(unlink_wakeup); */
	/*DECLARE_WAITQUEUE(wait, current); */

	/*RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS); */
#endif /* RTMP_MAC_USB */


    DBGPRINT(RT_DEBUG_TRACE, ("===> rt28xx_close\n"));

	/* Sanity check for pAd */
	if (pAd == NULL)
		return 0; /* close ok */

#ifdef WMM_ACM_SUPPORT
	/* must call first */
	ACMP_Release(pAd);
#endif /* WMM_ACM_SUPPORT */

#ifdef MESH_SUPPORT
	/* close all mesh link before the interface go down. */
	if (MESH_ON(pAd))
		MeshDown(pAd, TRUE);
#endif /* MESH_SUPPORT */

#ifdef WDS_SUPPORT
	WdsDown(pAd);
#endif /* WDS_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		BOOLEAN Cancelled;
#ifdef RTMP_MAC_PCI
		RTMPPCIeLinkCtrlValueRestore(pAd, RESTORE_CLOSE);
#endif /* RTMP_MAC_PCI */

		/* If dirver doesn't wake up firmware here, */
		/* NICLoadFirmware will hang forever when interface is up again. */
		if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
        {      
		    AsicForceWakeup(pAd, TRUE);
        }

#ifdef QOS_DLS_SUPPORT
		/* send DLS-TEAR_DOWN message, */
		if (pAd->CommonCfg.bDLSCapable)
		{
			UCHAR i;

			/* tear down local dls table entry */
			for (i=0; i<MAX_NUM_OF_INIT_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status	= DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= FALSE;
				}
			}

			/* tear down peer dls table entry */
			for (i=MAX_NUM_OF_INIT_DLS_ENTRY; i<MAX_NUM_OF_DLS_ENTRY; i++)
			{
				if (pAd->StaCfg.DLSEntry[i].Valid && (pAd->StaCfg.DLSEntry[i].Status == DLS_FINISH))
				{
					RTMPSendDLSTearDownFrame(pAd, pAd->StaCfg.DLSEntry[i].MacAddr);
					pAd->StaCfg.DLSEntry[i].Status = DLS_NONE;
					pAd->StaCfg.DLSEntry[i].Valid	= FALSE;
				}
			}
			RTMP_MLME_HANDLER(pAd);
		}
#endif /* QOS_DLS_SUPPORT */

		if (INFRA_ON(pAd) &&
			(!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			MLME_DISASSOC_REQ_STRUCT	DisReq;
			MLME_QUEUE_ELEM *MsgElem = (MLME_QUEUE_ELEM *) kmalloc(sizeof(MLME_QUEUE_ELEM), MEM_ALLOC_FLAG);
			if (Elem != NULL)
			{
				COPY_MAC_ADDR(DisReq.Addr, pAd->CommonCfg.Bssid);
				DisReq.Reason =  REASON_DEAUTH_STA_LEAVING;

				MsgElem->Machine = ASSOC_STATE_MACHINE;
				MsgElem->MsgType = MT2_MLME_DISASSOC_REQ;
				MsgElem->MsgLen = sizeof(MLME_DISASSOC_REQ_STRUCT);
				NdisMoveMemory(MsgElem->Msg, &DisReq, sizeof(MLME_DISASSOC_REQ_STRUCT));

				/* Prevent to connect AP again in STAMlmePeriodicExec */
				pAd->MlmeAux.AutoReconnectSsidLen= 32;
				NdisZeroMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->MlmeAux.AutoReconnectSsidLen);

				pAd->Mlme.CntlMachine.CurrState = CNTL_WAIT_OID_DISASSOC;
				MlmeDisassocReqAction(pAd, MsgElem);
				kfree(MsgElem);
			
				RtmpusecDelay(1000);
			}
		}

#ifdef RTMP_MAC_USB
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_REMOVE_IN_PROGRESS);
#endif /* RTMP_MAC_USB */

#ifdef CCX_SUPPORT
		RTMPCancelTimer(&pAd->StaCfg.LeapAuthTimer, &Cancelled);
#endif

		RTMPCancelTimer(&pAd->StaCfg.StaQuickResponeForRateUpTimer, &Cancelled);
		RTMPCancelTimer(&pAd->StaCfg.WpaDisassocAndBlockAssocTimer, &Cancelled);

#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
		/* send wireless event to wpa_supplicant for infroming interface down. */
		RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, RT_INTERFACE_DOWN, NULL, NULL, 0);
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */

		MlmeRadioOff(pAd);
#ifdef RTMP_MAC_PCI
		pAd->bPCIclkOff = FALSE;    
#endif /* RTMP_MAC_PCI */
	}
#endif /* CONFIG_STA_SUPPORT */

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

	for (i = 0 ; i < NUM_OF_TX_RING; i++)
	{
		while (pAd->DeQueueRunning[i] == TRUE)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Waiting for TxQueue[%d] done..........\n", i));
			RtmpusecDelay(1000);
		}
	}
	
#ifdef RTMP_MAC_USB
	/* ensure there are no more active urbs. */
	/*add_wait_queue (&unlink_wakeup, &wait); */
	/*pAd->wait = &unlink_wakeup; */

	/* maybe wait for deletions to finish. */
	i = 0;
	while(i < 25)
	{
		unsigned long IrqFlags;

		RTMP_IRQ_LOCK(&pAd->BulkInLock, IrqFlags);
		if (pAd->PendingRx == 0)
		{
			RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
			break;
		}
		RTMP_IRQ_UNLOCK(&pAd->BulkInLock, IrqFlags);
                RtmpusecDelay(UNLINK_TIMEOUT_MS*1000);	/*Time in microsecond */
		i++;
	}
	/*pAd->wait = NULL; */
	/*remove_wait_queue (&unlink_wakeup, &wait); */
#endif /* RTMP_MAC_USB */

#ifdef CONFIG_AP_SUPPORT

	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef RTMP_MAC_USB
	  /*RTMPCancelTimer(&pAd->CommonCfg.BeaconUpdateTimer, &Cancelled); */
#endif /* RTMP_MAC_USB */

#ifdef DOT11N_DRAFT3
		if (pAd->CommonCfg.Bss2040CoexistFlag & BSS_2040_COEXIST_TIMER_FIRED)
		{
			RTMPCancelTimer(&pAd->CommonCfg.Bss2040CoexistTimer, &Cancelled);
			pAd->CommonCfg.Bss2040CoexistFlag  = 0;
		}
#endif /* DOT11N_DRAFT3 */

		/* PeriodicTimer already been canceled by MlmeHalt() API. */
		/*RTMPCancelTimer(&pAd->PeriodicTimer,	&Cancelled); */
	}
#endif /* CONFIG_AP_SUPPORT */

	/* Close kernel threads or tasklets */
	RtmpMgmtTaskExit(pAd);

	/* Stop Mlme state machine */
	MlmeHalt(pAd);

	/* Close kernel threads or tasklets */
	RtmpNetTaskExit(pAd);


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		MacTableReset(pAd, 1);
#ifdef MAT_SUPPORT
		MATEngineExit(pAd);
#endif /* MAT_SUPPORT */
	}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef MAT_SUPPORT
		MATEngineExit(pAd);
#endif /* MAT_SUPPORT */

		/* Shutdown Access Point function, release all related resources */
		APShutdown(pAd);
#ifdef AUTO_CH_SELECT_ENHANCE
		/* Free BssTab & ChannelInfo tabbles. */
		AutoChBssTableDestroy(pAd);
		ChannelInfoDestroy(pAd);
#endif /* AUTO_CH_SELECT_ENHANCE */
	}
#endif /* CONFIG_AP_SUPPORT */

	MeasureReqTabExit(pAd);
	TpcReqTabExit(pAd);

#ifdef WSC_INCLUDED
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
		WscStop(pAd, FALSE, &pAd->ApCfg.MBSSID[MAIN_MBSSID].WscControl);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		WscStop(pAd,
#ifdef CONFIG_AP_SUPPORT
				FALSE,
#endif /* CONFIG_AP_SUPPORT */
				&pAd->StaCfg.WscControl);
#endif /* CONFIG_STA_SUPPORT */

#ifdef OLD_DH_KEY
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	    	WSC_VFREE_KEY_MEM(pAd->ApCfg.MBSSID[0].WscControl.pPubKeyMem, pAd->ApCfg.MBSSID[0].WscControl.pSecKeyMem);
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		WSC_VFREE_KEY_MEM(pAd->StaCfg.WscControl.pPubKeyMem, pAd->StaCfg.WscControl.pSecKeyMem);
#endif /* CONFIG_STA_SUPPORT */
#endif /* OLD_DH_KEY */

#ifndef OLD_DH_KEY
	DH_freeall();
#endif /* OLD_DH_KEY */

	/* WSC hardware push button function 0811 */
	WSC_HDR_BTN_Stop(pAd);
#endif /* WSC_INCLUDED */

#ifdef RTMP_MAC_PCI
	{
			BOOLEAN brc;
			/*	ULONG			Value; */

			if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
			{
				RTMP_ASIC_INTERRUPT_DISABLE(pAd);
			}

			/* Receive packets to clear DMA index after disable interrupt. */
			/*RTMPHandleRxDoneInterrupt(pAd); */
			/* put to radio off to save power when driver unload.  After radiooff, can't write /read register.  So need to finish all */
			/* register access before Radio off. */


			brc=RT28xxPciAsicRadioOff(pAd, RTMP_HALT, 0);
			if (brc==FALSE)
			{
				DBGPRINT(RT_DEBUG_ERROR,("%s call RT28xxPciAsicRadioOff fail !!\n", __FUNCTION__)); 
			}
	}
	

/*
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
	{
		RTMP_ASIC_INTERRUPT_DISABLE(pAd);
	}

	/* Disable Rx, register value supposed will remain after reset */
	NICIssueReset(pAd);
*/

	/* Free IRQ */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
	{
		/* Deregister interrupt function */
		RtmpOSIRQRelease(pEndObj);
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
	}
#endif /* RTMP_MAC_PCI */

	/* Free Ring or USB buffers */
	RTMPFreeTxRxRingMemory(pAd);

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);

#ifdef DOT11_N_SUPPORT
	/* Free BA reorder resource */
	ba_reordering_resource_release(pAd);
#endif /* DOT11_N_SUPPORT */

#ifdef RTMP_MAC_USB
#ifdef INF_AMAZON_SE
	if (pAd->UsbVendorReqBuf)
		os_free_mem(pAd, pAd->UsbVendorReqBuf);
#endif /* INF_AMAZON_SE */
#endif /* RTMP_MAC_USB */


	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_START_UP);

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
	END_OBJ *pEndObj = (END_OBJ *)dev;
	PRTMP_ADAPTER pAd;
	int retval = 0;


	 pAd = (PRTMP_ADAPTER)pEndObj->devObject.pDevice;
	/* Sanity check for pAd */
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free; So the net_dev->priv will be NULL in 2rd open */
		DBGPRINT(RT_DEBUG_ERROR, ("pAd is NULL!\n"));
		return -1;
	}

	/* Request interrupt service routine for PCI device */
	/* register the interrupt routine with the os */
#ifdef RTMP_RBUS_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_RBUS)
	{
		SYS_INT_CONNECT(INUM_TO_IVEC(IV_RT2880_INIC_VEC), rt2860_interrupt, pAd, retval);
	}
	else
#endif /* RTMP_RBUS_SUPPORT */
#ifdef RTMP_PCI_SUPPORT
	if (pAd->infType == RTMP_DEV_INF_PCI) /* RT2880 PCI */
	{
		SYS_INT_CONNECT(INUM_TO_IVEC(IV_RT2880_PCI_VEC), rt2860_interrupt, pAd, retval);
	}
#endif /* RTMP_PCI_SUPPORT */
	/* Init IRQ parameters */
	rtmp_irq_init(pAd);
	
	/* Chip & other init */
	if (rt28xx_init(pAd, NULL, NULL) == FALSE)
		goto err;

#ifdef MBSS_SUPPORT
	/* the function can not be moved to RT2860_probe() even register_netdev()
	   is changed as register_netdevice().
	   Or in some PC, kernel will panic (Fedora 4) */
	RT28xx_MBSS_Init(pAd, pAd->net_dev);
#endif /* MBSS_SUPPORT */

#ifdef WDS_SUPPORT
	RT28xx_WDS_Init(pAd, pAd->net_dev);
#endif /* WDS_SUPPORT */

#ifdef APCLI_SUPPORT
	RT28xx_ApCli_Init(pAd, pAd->net_dev);
#endif /* APCLI_SUPPORT */

#ifdef MESH_SUPPORT
	RTMP_Mesh_Init(pAd, pAd->net_dev, pHostName);
#endif /* MESH_SUPPORT */	

	/* Enable Interrupt */
	RTMP_IRQ_ENABLE(pAd);

	/* Now Enable RxTx */
	RTMPEnableRxTx(pAd);
	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

	{
		UINT32 reg = 0;
		RTMP_IO_READ32(pAd, 0x1300, &reg);
		DBGPRINT(RT_DEBUG_TRACE, ("0x1300 = %08x\n", reg));
	}

	END_FLAGS_SET (pEndObj, IFF_UP | IFF_RUNNING);
	
/*+++Add for VxWorks Shell Cmd supprot */
#ifdef RTMP_VX_SHELL_CMD_SUPPORT
#ifdef VXWORKS_6X
	RtmpVxShellCmdArrayAdd();
#endif /* VXWORKS_6X */
#endif /* RTMP_VX_SHELL_CMD_SUPPORT */
/*---Add for VxWorks Shell Cmd supprot */

	return (retval);

err:
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
static inline M_BLK_ID RtmpVxNetPktCheck(
	IN END_OBJ *pEndObj,
	IN M_BLK_ID pMblk)
{
	M_BLK_ID		pNewMblk;
	RTMP_ADAPTER	*pAd;
    BOOLEAN			bNeedCopy = FALSE;


	pAd = RTMP_OS_NETDEV_GET_PRIV(pEndObj);
	
	if (!(pEndObj->flags & IFF_UP))
	{
		RELEASE_NDIS_PACKET(pAd, (PNDIS_PACKET)pMblk, NDIS_STATUS_FAILURE);
		return NULL;
	}

#if 0
	/* This check not used now due to some packet from vxworks kernel didn't set this up.*/
	if ((pMblk->m_flags & M_PKTHDR) != M_PKTHDR)
	{
		DBGPRINT(RT_DEBUG_WARN, ("M_PKTHDR not set!\n"));
		RELEASE_NDIS_PACKET(pAd, (PNDIS_PACKET)pMblk, NDIS_STATUS_FAILURE);
		return NULL;
	}
#endif

	/* Check if we need to do copy the original mBlk to a new mBlk for further access */
	if ((pMblk->m_next) || 
		((pMblk->m_data - pMblk->pClBlk->clNode.pClBuf) <= CB_MAX_OFFSET) 
#ifdef CACHE_DMA_SUPPORT
		||(memcmp(pMblk->pClBlk->clNode.pClBuf, CACHE_DMA_SIGNAUTURE, 4) != 0)
#endif /* CACHE_DMA_SUPPORT */
		)
	{
#if 0
		DBGPRINT(RT_DEBUG_TRACE, ("Warning!!! a packet sent with m_next(0x%x), headroom(0x%x - 0x%x = %d)\n", 
						(pMblk->m_next), pMblk->m_data, pMblk->pClBlk->clNode.pClBuf, 
						(pMblk->m_data - pMblk->pClBlk->clNode.pClBuf)));
#endif
		bNeedCopy = TRUE;
	}
	else
	{
		NdisZeroMemory(pMblk->pClBlk->clNode.pClBuf, CB_MAX_OFFSET);
	}
	
	/* Assign the pEndObj to mBlk first, or the skb_copy will failed! */
	SET_OS_PKT_NETDEV(pMblk, pEndObj);
	if (bNeedCopy)
	{
		pNewMblk = skb_copy((PNDIS_PACKET)pMblk, 0);
		RELEASE_NDIS_PACKET(pAd, pMblk, NDIS_STATUS_SUCCESS);
		return pNewMblk;
	}
		
	return pMblk;
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
STATUS rt28xx_packet_xmit(M_BLK_ID pMblk)
{
	int status = 0;
	struct end_object *pEndObj;
	RTMP_ADAPTER *pAd;
	PNDIS_PACKET pPacket, pNewPacket;

	pEndObj = GET_OS_PKT_NETDEV(pMblk);
	pAd =  (RTMP_ADAPTER *)(RTMP_OS_NETDEV_GET_PRIV(pEndObj));
	pPacket= (PNDIS_PACKET) pMblk;


        /* EapolStart size is 18 */
	if ( GET_OS_PKT_LEN(pMblk) < 14)
	{
		/*DBGPRINT(RT_DEBUG_TRACE, ("bad packet size: %d\n", pkt->len)); */
		hex_dump("bad packet", GET_OS_PKT_DATAPTR(pMblk), GET_OS_PKT_LEN(pMblk));
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		goto done;
	}

	/* Check if we need to copy the packet */
	if (GET_OS_PKT_HEADROOM(pPacket) < CB_MAX_OFFSET)
	{
		pNewPacket = skb_copy(pPacket, 0);
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_SUCCESS);
		if (pNewPacket == NULL)
		{
			goto done;
		}
		pPacket = pNewPacket;
	}

#ifdef CONFIG_5VT_ENHANCE
	RTMP_SET_PACKET_5VT(pPacket, 0);
#endif /* CONFIG_5VT_ENHANCE */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
#ifdef ETH_CONVERT_SUPPORT
		/* Don't move this checking into wdev_tx_pkts(), becasue the net_device is OS-depeneded. */
		PUCHAR	pPktSrcAddr =  (PUCHAR)(GET_OS_PKT_DATAPTR(pPacket) + 6);

		if ((pAd->EthConvert.ECMode & ETH_CONVERT_MODE_CLONE) 
			 && (!pAd->EthConvert.CloneMacVaild)
		 	&& (pAd->EthConvert.macAutoLearn)
		 	&& (!(pPktSrcAddr[0] & 0x1)))
		{
			END_OBJ *pEndEntry = NULL;
			/*END_TBL_ROW* pNode; */
			/*IMPORT LIST endList; */
			long	endIfType;
			RTMP_STRING *pDevNameBuf = NULL, *token, *pDevNameList = BRIDGE_PORTS, *pUnitNum;
			RTMP_STRING DevName[END_NAME_MAX] = {0};
			BOOLEAN bOK = TRUE;
			INT		devNameLen;
			
			pDevNameBuf = malloc(strlen(pDevNameList) + 1);
			if (pDevNameBuf)
			{
				memset(pDevNameBuf, 0, strlen(pDevNameList) + 1);
				for (; *pDevNameList != '\0'; pDevNameList++)
				{        
					if (!isspace((int)*pDevNameList))  /* ignore space */            
						*pDevNameBuf++ = *pDevNameList;
				}
				*pDevNameBuf = '\0';

				for (token = strtok(pDevNameBuf, ","); 
					 token != NULL;        
					 token = strtok(NULL, ","))
				{
					pUnitNum = strpbrk(token, "0123456789");
					if (pUnitNum)
					{
						devNameLen = min(pUnitNum - token, END_NAME_MAX - 1);        
						strncpy(DevName, token, devNameLen);
						pEndEntry = endFindByName(DevName, atoi(pUnitNum));
						endIfType = muxIfTypeGet(pEndEntry);
						if (( endIfType == M2_ifType_ethernet_csmacd) &&
							NdisEqualMemory(RTMP_OS_NETDEV_GET_PHYADDR(pEndEntry), pPktSrcAddr, 6))
						{
							bOK = FALSE;
							break;						
						}
					}
					else
						break;
				}
			
				free(pDevNameBuf);
			}
/*			
			for (pNode = (END_TBL_ROW *)lstFirst (&endList); pNode != NULL; 
				pNode = (END_TBL_ROW *)lstNext (&pNode->node))
			{
				for (pEndEntry = (END_OBJ *)lstFirst(&pNode->units); pEndEntry != NULL; 
					pEndEntry = (END_OBJ *)lstNext(&pEndEntry->node))
				{
					endIfType = muxIfTypeGet(pEndEntry);
					if (( endIfType == M2_ifType_ethernet_csmacd) &&
						NdisEqualMemory(RTMP_OS_NETDEV_GET_PHYADDR(pEndEntry), pPktSrcAddr, 6))
						break;
				}
			}
*/
			if (bOK) 
			{
				NdisMoveMemory(&pAd->EthConvert.EthCloneMac[0], pPktSrcAddr, MAC_ADDR_LEN);
				pAd->EthConvert.CloneMacVaild = TRUE;
#ifdef RELEASE_EXCLUDE
				DBGPRINT(RT_DEBUG_INFO, ("Incming Mac=%02x:%02x:%02x:%02x:%02x:%02x," 
										    "NewMAC=%02x:%02x:%02x:%02x:%02x:%02x!\n",
							PRINT_MAC(pPktSrcAddr), PRINT_MAC(pAd->EthConvert.EthCloneMac)));
#endif /* RELEASE_EXCLUDE */
			}
		}

		if ((pAd->EthConvert.ECMode == ETH_CONVERT_MODE_CLONE)
			&& (NdisEqualMemory(pAd->CurrentAddress, pPktSrcAddr, MAC_ADDR_LEN) == FALSE))
		{
			/* Drop pkt since we are in pure clone mode and the src is not the cloned mac address. */
			RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
			goto done;
		}
#endif /* ETH_CONVERT_SUPPORT */
	}
#endif /* CONFIG_STA_SUPPORT */

	wdev_tx_pkts((NDIS_HANDLE)pAd, (PPNDIS_PACKET) &pPacket, 1, wdev);

	status = 0;

done:
			   
	return status;
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
static int rt28xx_send_packets(
	IN END_OBJ * pEndObj,
	IN M_BLK_ID     pMblk)
{
	M_BLK_ID     pNewMblk;
	NDIS_STATUS		status = NDIS_STATUS_FAILURE;

	pNewMblk = RtmpVxNetPktCheck(pEndObj, pMblk);
	if (pNewMblk)
		return rt28xx_packet_xmit(pNewMblk);
	else
		return 0;

}


/******************************************************************************
*
* RtmpVxIoctlAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the endAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

LOCAL void RtmpVxIoctlAddrFilterSet(
    IN END_OBJ *pEndObj	/* device to be updated */
)
{
	ETHER_MULTI* pCurr = END_MULTI_LST_FIRST (pEndObj);

	while (pCurr != NULL)
	{
		/* TODO - set up the multicast list */
		pCurr = END_MULTI_LST_NEXT(pCurr);
	}
	/* TODO - update the device filter list */
	
}


/******************************************************************************
*
* RtmpVxIoctlConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*/

void RtmpVxIoctlConfig(
	IN END_OBJ *pEndObj)
{

	/* Set promiscuous mode if it's asked for. */

	if (END_FLAGS_GET(pEndObj) & IFF_PROMISC)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s():promiscuous mode on!\n", __FUNCTION__));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s():promiscuous mode off!\n", __FUNCTION__));
	}

	/* Set up address filter for multicasting. */

	if (END_MULTI_LST_CNT(pEndObj) > 0)
	{
		RtmpVxIoctlAddrFilterSet(pEndObj);
	}

	/* TODO - shutdown device completely */

	/* TODO - reset all device counters/pointers, etc. */

	/* TODO - initialize the hardware according to flags */

	return;
}


void tbtt_tasklet(unsigned long data)
{
/*#define MAX_TX_IN_TBTT		(16) */

#ifdef CONFIG_AP_SUPPORT
		PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

#ifdef RTMP_MAC_PCI
	if (pAd->OpMode == OPMODE_AP)
	{
#ifdef AP_QLOAD_SUPPORT
		/* update channel utilization */
		QBSS_LoadUpdate(pAd);
#endif /* AP_QLOAD_SUPPORT */

#ifdef DOT11K_RRM_SUPPORT
		RRM_QuietUpdata(pAd);
#endif /* DOT11K_RRM_SUPPORT */
	}
#endif /* RTMP_MAC_PCI */

	if (pAd->OpMode == OPMODE_AP)
	{
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
			QUEUE_ENTRY *pEntry;
			BOOLEAN bPS = FALSE;
			UINT count = 0;
			unsigned long IrqFlags;
			
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
					// TODO: shiang-usw, modify the WCID_ALL to pMBss->tr_entry because we need to tx B/Mcast frame here!!
					RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, /*MAX_TX_IN_TBTT*/MAX_PACKETS_IN_MCAST_PS_QUEUE);
				}
			}
	}
#endif /* CONFIG_AP_SUPPORT */
}


INT rt28xx_ioctl(
	IN	PNET_DEV	endDev, 
	IN	int			cmd, 
	IN	caddr_t		data)
{
	RTMP_ADAPTER	*pAd = NULL;
	INT				ret = 0;


	pAd = (RTMP_ADAPTER *)endDev->devObject.pDevice;
	if (pAd == NULL)
	{
		/* if 1st open fail, pAd will be free;
		   So the net_dev->priv will be NULL in 2rd open */
		return -ENETDOWN;
	}

#ifdef VXWORKS_6X
	/* The cmd format : ((unsigned long)(inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))) */
	DBGPRINT(RT_DEBUG_INFO, ("Cmd=0x%x, IN/OUT=0x%x, Group=%d, Number=%d, Len=%d! IOCBASECMD=0x%x!\n", 
			cmd, cmd & 0xff000000, IOCGROUP(cmd), (cmd & 0xff), IOCPARM_LEN(cmd), IOCBASECMD(cmd)));
#endif /* VXWORKS_6X */

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		ret = rt28xx_ap_ioctl(endDev, cmd, data);
	}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		ret = rt28xx_sta_ioctl(endDev, cmd, data);
	}
#endif /* CONFIG_STA_SUPPORT */

	return ret;
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
	*ppAd = (PVOID)kmalloc(sizeof(RTMP_ADAPTER), 0);
    
	if (*ppAd) 
	{
		NdisZeroMemory(*ppAd, sizeof(RTMP_ADAPTER));
		((PRTMP_ADAPTER)*ppAd)->OS_Cookie = handle;
		return (NDIS_STATUS_SUCCESS);
	}
	else
	{
		return (NDIS_STATUS_FAILURE);
	}
}


NDIS_STATUS RtmpVxNetPoolFree(struct os_cookie *pObj)
{
	int	clIdx;
	
	
	for (clIdx=0; clIdx < RTMP_NETPOOL_CNT; clIdx++)
	{
		/* free cl buffer */
		if (pObj->pClMemPtr[clIdx])
#ifdef CACHE_DMA_SUPPORT
			cacheDmaFree(pObj->pClMemPtr[clIdx]);
#else
			free(pObj->pClMemPtr[clIdx]);
#endif /* CACHE_DMA_SUPPORT */
	
		/* free mclBlk buffer */
		if (pObj->pMclMemPtr[clIdx])
		free(pObj->pMclMemPtr[clIdx]);
	
	}

	if(*pObj->pNetPool)
		free(*pObj->pNetPool);	

	return NDIS_STATUS_SUCCESS;
	
}


NDIS_STATUS RtmpVxNetPoolInit(struct os_cookie *pObj)
{
	M_CL_CONFIG		mclBlkConfig;
	CL_DESC			clConfig;
	int				poolIdx, clSize;
	UCHAR			*memPtr;
	NET_POOL_ID		pNetPool;
	
	/*
		This is how we would set up an END netPool using netBufLib(1).
	 	This code is pretty generic.

		1. Due to we need to reserve space for the cb, so we need to allocate two net pools and 
			each with one cluster:
				clConfig with cluster size 1536, used for the Mgmt/Tx Ring.
				clConfig with cluster size 3840, used for the Rx Ring. (AMSUD/RalinkAgg).
		2. Allocate netpool/mBlk/Cluster blocks from normal memory. Every mBlk and cluster is 
			prepended by a 4-byte header (which is why the size calculations for clusters and 
			mBlk structures contained an extra sizeof(long).
	  */

	/* Allocate memory for the net pools. one for Tx, one for Rx. */
	memPtr = malloc(sizeof(NET_POOL) * RTMP_NETPOOL_CNT);
	if (memPtr == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR,("malloc for NET_POOL failed!\n"));
	    return NDIS_STATUS_FAILURE;
	}
	memset(memPtr, 0, sizeof(NET_POOL) * RTMP_NETPOOL_CNT);
	pObj->pNetPool[RTMP_NETPOOL_TX] = (NET_POOL_ID)memPtr;
	pObj->pNetPool[RTMP_NETPOOL_RX] = (NET_POOL_ID)((ULONG)memPtr + sizeof(NET_POOL));


	for (poolIdx = 0; poolIdx < RTMP_NETPOOL_CNT; poolIdx++)
	{
		pNetPool = pObj->pNetPool[poolIdx]; 
		clSize = 0;

	/*
			Initialize the CL_CONFIG, need provide four values:
				clSize - the size of a cluster in this cluster pool
				clNum - the number of clusters in this cluster pool
				memArea - pointer to an area of memory that can contain all the clusters
				memSize - the size of that memory area
	  */
		memset(&clConfig, 0, sizeof(clConfig));
		if (poolIdx == RTMP_NETPOOL_TX)
		{
			clConfig.clNum = (TX_RING_SIZE + MGMT_RING_SIZE);
			clSize = MGMT_DMA_BUFFER_SIZE;
			/* clConfig.clSize = ROUND_UP(MGMT_DMA_BUFFER_SIZE + sizeof(long),  _CACHE_ALIGN_SIZE)  - sizeof(long); */
		}
		else if (poolIdx == RTMP_NETPOOL_RX)
		{	
			clConfig.clNum = RX_RING_SIZE * 3;
			clSize = RX_BUFFER_NORMSIZE;
			/*clConfig.clSize = ROUND_UP(RX_BUFFER_NORMSIZE + sizeof(long), _CACHE_ALIGN_SIZE) - sizeof(long); */
		}
		else
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s:wrong poolIdx(%d)!\n", __FUNCTION__, poolIdx));
			goto err_free_mem;
		}
#ifdef VXWORKS_5X
		clSize += NETBUF_LEADING_SPACE;
#endif /* VXWORKS_5X */
		clConfig.clSize = ROUND_UP(clSize + sizeof(long), _CACHE_ALIGN_SIZE) - sizeof(long);

		clConfig.memSize = clConfig.clNum * (clConfig.clSize + sizeof(long));
#ifdef CACHE_DMA_SUPPORT
		memPtr = (char *)cacheDmaMalloc(clConfig.memSize + _CACHE_ALIGN_SIZE);
#else
		memPtr = (char *)malloc(clConfig.memSize + _CACHE_ALIGN_SIZE);
#endif /* CACHE_DMA_SUPPORT */
		if (memPtr == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR,("%s(): malloc for clConfig[%d] with size(%d) failed!\n", 
						__FUNCTION__, poolIdx, clConfig.memSize));
			goto err_free_mem;
		}
		clConfig.memArea = (char *)ROUND_UP(memPtr + sizeof(long),  _CACHE_ALIGN_SIZE) - sizeof(long);
		pObj->pClMemPtr[poolIdx] = memPtr;	

	
	/*
		Setup mBlk/cluster block pool with more mbufs than clBlks
		Calculate the total memory for all the M-Blks and CL-Blks.
			mclBlkConfig need to provide four parameters:
				mBlkNum - a count of `mBlk' structures, set as twice of the number of clusters
				clBlkNum - a count of `clBlk' structures
				memSize - the size of that memory area
				memArea - pointer to an area of memory that can contain all the 'mBlk' and 'clBlk' structures
	  */
		memset(&mclBlkConfig, 0, sizeof(mclBlkConfig));
		mclBlkConfig.mBlkNum = clConfig.clNum * 2;
		mclBlkConfig.clBlkNum = clConfig.clNum;
		mclBlkConfig.memSize = (mclBlkConfig.mBlkNum * (M_BLK_SZ + sizeof (void *))) +
						(mclBlkConfig.clBlkNum *(CL_BLK_SZ + sizeof(void *)));
		mclBlkConfig.memArea = memalign (sizeof(long), mclBlkConfig.memSize);
		if (mclBlkConfig.memArea == NULL)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s() malloc for mclBlkConfig failed!\n", __FUNCTION__));
			goto err_free_mem;
		}
		pObj->pMclMemPtr[poolIdx] = mclBlkConfig.memArea;

			
		/*
			call kernel function to initialize the netpool
		*/
		if (netPoolInit(pNetPool, &mclBlkConfig, &clConfig, 1, NULL) == -1)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("%s(): netPoolInit(%d) failed!\n", __FUNCTION__, poolIdx));
			goto err_free_mem;
		}

		DBGPRINT(RT_DEBUG_TRACE, ("pNetPool[%d]=0x%x!\n", poolIdx, pObj->pNetPool[poolIdx]));
	}

		
#if 0
	for (poolIdx = 0; poolIdx < RTMP_NETPOOL_CNT; poolIdx++)
		hex_dump_NetPool(__FUNCTION__, pObj->pNetPool[poolIdx], FALSE);
#endif


	return NDIS_STATUS_SUCCESS;


err_free_mem:
	RtmpVxNetPoolFree(pObj);

	return NDIS_STATUS_FAILURE;
	
}


PNET_DEV RtmpPhyNetDevInit(
	IN RTMP_ADAPTER *pAd,
	IN RTMP_OS_NETDEV_OP_HOOK *pNetDevHook)
{
	 END_OBJ	*pEndObj = NULL;
	
	pEndObj = (END_OBJ *)malloc(sizeof(END_OBJ));
	if (pEndObj == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("RtmpPhyNetDevInit(): creation failed for main physical net device!\n"));
		return NULL;
	}

	NdisZeroMemory((unsigned char *)pNetDevHook, sizeof(RTMP_OS_NETDEV_OP_HOOK));
	pNetDevHook->open = MainVirtualIF_open;
	pNetDevHook->stop = MainVirtualIF_close;
	pNetDevHook->xmit = rt28xx_send_packets;
	pNetDevHook->ioctl = rt28xx_ioctl;
	pNetDevHook->priv_flags = INT_MAIN;

	pNetDevHook->needProtcted = FALSE;
	memcpy(&pNetDevHook->devName[0],  "ra0", strlen("ra0"));
	pAd->net_dev = pEndObj;
	
#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev = pEndObj;
#endif /* CONFIG_AP_SUPPORT */

	if ((END_OBJ_INIT(pEndObj, (DEV_OBJ *)pAd, &pNetDevHook->devName[0],
	              0, &RtmpMAINEndFuncTable, &pNetDevHook->devName[0]) == ERROR)) 
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not init net device %s!\n", &pNetDevHook->devName[0]));
		return NULL;
	}
	
	if ((END_MIB_INIT(pEndObj, M2_ifType_ethernet_csmacd,
	              &pNetDevHook->devAddr[0],  6, ETHERMTU, END_SPEED) == ERROR))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Can not init MIB of net device %s!\n", &pNetDevHook->devName[0]));
		return NULL;
	}

	END_OBJ_READY(pEndObj, IFF_NOTRAILERS |IFF_BROADCAST |IFF_MULTICAST);
	
	DBGPRINT(RT_DEBUG_ERROR, ("RtmpPhyNetDevInit(): Allocate END object success!\n"));

	return pEndObj;
	
}


BOOLEAN RtmpPhyNetDevExit(
	IN RTMP_ADAPTER *pAd, 
	IN PNET_DEV net_dev)
{
	END_OBJ *pEndDev;

	pEndDev = (END_OBJ *)net_dev;

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
					net_dev->devObject.name));
		RtmpOSNetDevDetach(net_dev);
	}

	return TRUE;
}

