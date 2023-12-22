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
    rbus_main_end.c

    Abstract:
    Create and register network interface for RBUS based chipsets in eCos platform.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
*/

#include "rt_config.h"

#define MAX_ETH_FRAME_SIZE 1520
#define PRIORITY_HIGH   0 /* the highest irq priorit */

static cyg_interrupt rtmp_wlan_interrupt;
static cyg_handle_t  rtmp_wlan_interrupt_handle;

int	pcie_int_line = 0;
struct mt_dev_priv *rti_ecos_priv_data;

ETH_DRV_SC(devive_wireless_sc_i0,
           &rti_ecos_priv_data,  /* Driver specific data */
           SECOND_INF_MAIN_DEV_NAME "0",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_netdev_i0, 
                SECOND_INF_MAIN_DEV_NAME "0",
                rti_ecos_init,
                &devive_wireless_sc_i0);

/*
 *  Interrupt handling - ISR and DSR
 */
static cyg_uint32 rti_wlan_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	rt2860_interrupt((void *) data);
   	cyg_interrupt_acknowledge(vector);
   	return CYG_ISR_CALL_DSR;
}

static void rti_wlan_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	/*  schecdule deliver routine  */
	if (data != 0)
		eth_drv_dsr(vector, count, (cyg_addrword_t) data);
	else
		DBGPRINT(RT_DEBUG_ERROR, ("rti_wlan_dsr: data is NULL\n"));
}

static bool rti_ecos_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV 			pNetDev = NULL;
	POS_COOKIE			pOSCookie = NULL;
	ULONG				csr_addr;
	UINT8				MacAddr[6];
	unsigned int			dev_irq;
	USHORT 				value;
	int					status;
	struct mt_dev_priv 	*priv_info = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rti_ecos_init()\n"));

#ifdef SECOND_WIFI
	csr_addr = ra305x_pcie_init(&pcie_int_line);
#else
	csr_addr = 0;
#endif

	if (csr_addr == 0)
		return false;

	/* Important: Check Kernel Configure */
	if (sizeof(ECOS_PKT_BUFFER) > MLEN)
	{
		DBGPRINT_ERR(("%s: The size of structure ECOS_PKT_BUFFER > MLEN\n", __FUNCTION__));
		goto err_out;
	}
	if (MCLBYTES < RX_BUFFER_AGGRESIZE)
	{
		DBGPRINT_ERR(("%s: The size of m_buf clusters (%d) is too small\n", __FUNCTION__, MCLBYTES));
		goto err_out;
	}

	/*RtmpRaBusInit============================================ */
	/* map physical address to virtual address for accessing register */
	dev_irq = RTMP_MAC_IRQ_NUM;

	pNetDev = (struct eth_drv_sc *)tab->device_instance;

	/* Allocate RTMP_ADAPTER adapter structure */
	pOSCookie = (POS_COOKIE) kmalloc(sizeof(struct os_cookie), GFP_ATOMIC);
	if (!pOSCookie)
	{
		DBGPRINT_ERR(("%s: allocate memory for os_cookie failed!\n", __FUNCTION__));
		goto err_out;
	}
	NdisZeroMemory((PUCHAR) pOSCookie, sizeof(struct os_cookie));

	status = RTMPAllocAdapterBlock((PVOID) pOSCookie, (VOID **)&pAd);
	if (status != NDIS_STATUS_SUCCESS)
	{
		kfree(pOSCookie);
		goto err_out;
	}
	/* Here are the RTMP_ADAPTER structure with rbus-bus specific parameters. */
	pAd->CSRBaseAddress = (PUCHAR) csr_addr;

	RtmpRaDevCtrlInit(pAd, RTMP_DEV_INF_PCIE);
	pAd->flash_offset = SECOND_RF_OFFSET;

	/*NetDevInit============================================== */
	priv_info = (struct mt_dev_priv *) kmalloc(sizeof(struct mt_dev_priv), GFP_ATOMIC);
	pAd->net_dev = pNetDev;
	pNetDev->driver_private = priv_info;
	pOSCookie->pci_dev = pNetDev;
	
	/* put private data structure */
	RTMP_OS_NETDEV_SET_PRIV(pNetDev, pAd);
	RTMP_DRIVER_NET_DEV_SET(pAd, pNetDev);

	if (pAd->chipOps.eeinit)
		pAd->chipOps.eeinit(pAd);

        /* get MAC address */
	get_mac_from_eeprom(pAd, &MacAddr[0]);

	/* Set up to handle interrupts */
	cyg_interrupt_create(pcie_int_line,
                        PRIORITY_HIGH,
                        (cyg_addrword_t)pAd->net_dev,
                        (cyg_ISR_t *)rti_wlan_isr,
                        (cyg_DSR_t *)rti_wlan_dsr,
                        &rtmp_wlan_interrupt_handle,
                        &rtmp_wlan_interrupt);
	cyg_interrupt_attach(rtmp_wlan_interrupt_handle);
	cyg_interrupt_configure(pcie_int_line, 1, 0);
	cyg_interrupt_unmask(pcie_int_line);
        
	/* Initialize upper level driver */
	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rti_ecos_init()\n"));

	return true;

err_out:
	return false;
}

static void rti_ecos_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
	PNET_DEV pNetDev = NULL;
	POS_COOKIE pOSCookie = NULL;    
	PRTMP_ADAPTER pAd = NULL;
	char devName[IFNAMSIZ];
	int i, value;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_start()\n"));

        /* if the device is runnung, do nothing */
        if (sc->state & ETH_DRV_STATE_ACTIVE)
                goto exit;

	pNetDev = sc;
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);	
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;
	if (pAd == NULL) {
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
                goto exit;
        }

        i = 0;
#ifdef APCLI_SUPPORT
        for (i = 0; i < MAX_APCLI_NUM; i++)
        {
                sprintf(devName, "%s%d", SECOND_INF_APCLI_DEV_NAME, i);
                if (strcmp(pNetDev->dev_name, devName) == 0)
                {
                        ApCli_VirtualIF_Open(sc);
                        return;
                }
        }
#endif /* APCLI_SUPPORT */
    
#ifdef WDS_SUPPORT
        for (i = 0; i < MAX_WDS_ENTRY; i++)
        {
                sprintf(devName, "%s%d", SECOND_INF_WDS_DEV_NAME, i);
                if (strcmp(pNetDev->dev_name, devName) == 0)
                {
                        WdsVirtualIF_open(sc);
                        return;
                }
        }
#endif /* WDS_SUPPORT */

#ifdef MBSS_SUPPORT
        for (i = 1; i < pAd->ApCfg.BssidNum; i++)
        {
                sprintf(devName, "%s%d", SECOND_INF_MAIN_DEV_NAME, i);
                if (strcmp(pNetDev->dev_name, devName) == 0)
                {
                        MBSS_VirtualIF_Open(sc);
                        return;
                }
        }
#endif /* MBSS_SUPPORT */

        sprintf(devName, "%s0", SECOND_INF_MAIN_DEV_NAME);
        if (strcmp(pNetDev->dev_name, devName) != 0)
        {
		DBGPRINT_ERR(("%s: device name(%s) is not found\n", __FUNCTION__, pNetDev->dev_name));
		goto exit;	  
        }

	RTMP_DRIVER_AP_MAIN_OPEN(pAd);
	if (VIRTUAL_IF_UP(pAd) != 0)
		return; 	

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_start()\n"));

exit:
    return;
}

static void  rti_ecos_stop(struct eth_drv_sc *sc)
{
	INT BssId, i;
	PNET_DEV pNetDev = NULL;
	PRTMP_ADAPTER pAd = NULL;
	POS_COOKIE pOSCookie = NULL;
	char devName[IFNAMSIZ];
    
	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_stop()\n"));

	/* if the device is not runnung, do nothing */
	if ((sc->state & ETH_DRV_STATE_ACTIVE) == 0)
		return;
    
	pNetDev = sc;	
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(pNetDev);
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;

	if (pAd == NULL)
		return;

#ifdef APCLI_SUPPORT
	sprintf(devName, "%s0", INF_APCLI_DEV_NAME);
	if (strcmp(pNetDev->dev_name, devName) == 0)
	{
		ApCli_VirtualIF_Close(sc);
		return;
	}
#endif /* APCLI_SUPPORT */

#ifdef MBSS_SUPPORT
	for (i = 1; i < pAd->ApCfg.BssidNum; i++)
	{
		sprintf(devName, "%s%d", SECOND_INF_MAIN_DEV_NAME, i);
		if (strcmp(pNetDev->dev_name, devName) == 0)
		{
			MBSS_VirtualIF_Close(sc);
			return;
		}
	}
#endif /* MBSS_SUPPORT */

	RTMPInfClose(pAd);
	VIRTUAL_IF_DOWN(pAd);
	RT_MOD_DEC_USE_COUNT();

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_stop()\n"));  
}

static int rti_ecos_control(struct eth_drv_sc *sc, 
                          unsigned long cmd, 
                          void *data, 
                          int len)
{
	POS_COOKIE		 pOSCookie = NULL;    
	PRTMP_ADAPTER	 pAd = NULL;
	char devName[IFNAMSIZ];

	DBGPRINT(RT_DEBUG_INFO, ("rt_ecos_control()\n"));	
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);	
	if (pAd == NULL)
	    return;

	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;
	switch(cmd)
	{
		case SIOCGIFPHY:
		{			
#ifdef APCLI_SUPPORT			
			INT i=0;
			UCHAR ifIndex;
			struct ifreq *ifr = (struct ifreq *)data;
			unsigned int *p = (unsigned int *)ifr->ifr_ifru.ifru_data;
			*p = 0;

			sprintf(devName, "%s0", INF_APCLI_DEV_NAME);
			if (strcmp(sc->dev_name, devName) == 0)
			{
				pOSCookie->ioctl_if_type = INT_APCLI;
				pOSCookie->ioctl_if = 0;
				ifIndex = pOSCookie->ioctl_if;
				
				if((pAd->ApCfg.ApCliTab[ifIndex].CtrlCurrState == APCLI_CTRL_CONNECTED)
					&& (pAd->ApCfg.ApCliTab[ifIndex].SsidLen != 0))
				{
					for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
					{
						PMAC_TABLE_ENTRY pEntry = &pAd->MacTab.Content[i];
				
						if ( IS_ENTRY_APCLI(pEntry)
							&& (pEntry->Sst == SST_ASSOC)
							&& (pEntry->PortSecured == WPA_802_1X_PORT_SECURED))
							{	
								*p = 1;
							}
					}
				}
			}				
#endif /* APCLI_SUPPORT */
			break;
		}

	}
	
	return 0;
}

static int rti_ecos_can_send(struct eth_drv_sc *sc)
{
    return 1;
}

//Copy from eCos kernel, remove the eth_drv_send(ifp);
static void rti_eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRESS key, int status)
{
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;
    struct mbuf *m0 = (struct mbuf *)key;
    CYGARC_HAL_SAVE_GP();

    // Check for errors here (via 'status')
    ifp->if_opackets++;
    // Done with packet

    // Guard against a NULL return - can be caused by race conditions in
    // the driver, this is the neatest fixup:
    if (m0) { 
//        mbuf_key = m0;
        m_freem(m0);
    }
    // Start another if possible
//    eth_drv_send(ifp); 
    CYGARC_HAL_RESTORE_GP();
}


static PECOS_PKT_BUFFER rti_convert_sglist_to_mbuf(struct eth_drv_sc *sc,
				struct eth_drv_sg *sg_list,
				int sg_len,
				int total_len,
				unsigned long key)
{
	PRTMP_ADAPTER		pAd = NULL;
	PECOS_PKT_BUFFER	pPacket = NULL;
	NDIS_STATUS			Status;
	INT					packetLength = 0;
	BOOLEAN				isFreeSG = TRUE;
        struct mbuf         *pMBuf = NULL;

        DBGPRINT(RT_DEBUG_INFO, ("==>Convert_sglist_to_mbuf()\n"));

	if (total_len >= MAX_ETH_FRAME_SIZE) {
                DBGPRINT(RT_DEBUG_ERROR, ("%s:total_len >= MAX_ETH_FRAME_SIZE\n", __FUNCTION__));
                isFreeSG = TRUE;
                goto FREE_SG;
        }

	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);

        pMBuf = (struct mbuf *) key;
        if (sg_len == 2)
        {
                struct mbuf *pMBuf2 = pMBuf->m_next;

                if ((pMBuf2->m_flags & M_EXT) && ((pMBuf2->m_data - pMBuf2->m_ext.ext_buf) > sg_list[0].len))
                {
                        Status = RTMP_AllocateNdisPacket_AppandMbuf(pAd, (PNDIS_PACKET *)&pPacket, pMBuf2);
                        if (Status != NDIS_STATUS_SUCCESS)
                        {
                                DBGPRINT(RT_DEBUG_ERROR, ("%s:can't allocate NDIS PACKET\n", __FUNCTION__));
                                goto FREE_SG;
                        }
                        pPacket->pDataPtr -= sg_list[0].len;
                        NdisCopyMemory(pPacket->pDataPtr, (void *) sg_list[0].buf, sg_list[0].len);
                        pPacket->pktLen += sg_list[0].len;                
                        pMBuf->m_next = NULL;
                        goto XMIT;
                }
                goto COPY_MBUF;
        }
        else if (sg_len == 1)
        {
                if ((pMBuf->m_flags & M_EXT) && ((pMBuf->m_data - pMBuf->m_ext.ext_buf) <= pMBuf->m_ext.ext_size))
                {
                        Status = RTMP_AllocateNdisPacket_AppandMbuf(pAd, (PNDIS_PACKET *)&pPacket, pMBuf);
                        if (Status != NDIS_STATUS_SUCCESS)
                        {
                                DBGPRINT(RT_DEBUG_ERROR, ("%s:can't allocate NDIS PACKET\n", __FUNCTION__));
                                goto FREE_SG;
                        }
                        isFreeSG = FALSE;
                        goto XMIT;
                }
                goto COPY_MBUF;
        }
        else if (sg_len == 0)
                goto FREE_SG;

COPY_MBUF: /*when sg_len > 2 */
        Status = RTMPAllocateNdisPacket(pAd, (PNDIS_PACKET *)&pPacket, NULL, 0, NULL, MAX_ETH_FRAME_SIZE);
        if (Status != NDIS_STATUS_SUCCESS)
        {
                DBGPRINT(RT_DEBUG_ERROR, ("%s:can't allocate NDIS PACKET\n", __FUNCTION__));
                goto FREE_SG;
        }

        while (sg_len > 0)
        {
                NdisCopyMemory(GET_OS_PKT_DATAPTR(pPacket) + packetLength, (void *) sg_list->buf, sg_list->len);
                packetLength += sg_list->len;
                sg_len--;
                sg_list++;
        }

        pPacket->pDataMBuf->m_len = packetLength;
XMIT:
        GET_OS_PKT_LEN(pPacket) = total_len;
   
FREE_SG:
        if (isFreeSG)
			rti_eth_drv_tx_done(sc,key,0);

        return pPacket;
}

static void rti_ecos_send(struct eth_drv_sc *sc,
				struct eth_drv_sg *sg_list,
				int sg_len,
				int total_len,
				unsigned long key)
{
	PECOS_PKT_BUFFER	pPacket = NULL;

	DBGPRINT(RT_DEBUG_INFO, ("==>rt_ecos_send()\n"));

	pPacket = rti_convert_sglist_to_mbuf(sc, sg_list, sg_len, total_len, key);
	if (pPacket != NULL)
		rt28xx_send_packets(pPacket, sc);

	DBGPRINT(RT_DEBUG_INFO, ("<==rt_ecos_send()\n"));
	return;
}

static void rti_ecos_recv(struct eth_drv_sc *sc,
                        struct eth_drv_sg *sg_list,
                        int sg_len)
{
    /* Not Use */
	return;
} 

static void rti_ecos_deliver(struct eth_drv_sc *sc)
{
	POS_COOKIE			    pOSCookie = NULL;    
	PRTMP_ADAPTER	        pAd = NULL;
	PRTMP_NET_TASK_STRUCT   pNetTask = NULL;
	unsigned long flags;

	DBGPRINT(RT_DEBUG_INFO, ("rt_ecos_deliver()\n"));	
	pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);
	if (pAd == NULL)
	    return;

	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;

	while(1)
	{
		if (pOSCookie->nettask_handle == 0)
			break;

	        /* Note: here must call cyg_mbox_tryget, not cyg_mbox_get */
		RTMP_INT_LOCK(&pAd->irq_lock, flags);
		pNetTask = (PRTMP_NET_TASK_STRUCT) cyg_mbox_tryget(pOSCookie->nettask_handle);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		
		if (pNetTask == NULL)
			break;

		pNetTask->funcPtr(pNetTask->data); 
	}

	DBGPRINT(RT_DEBUG_INFO, ("<=== rt_ecos_deliver()\n"));	
}


static void rti_ecos_poll(struct eth_drv_sc *sc)
{
	return;
}

static int  rti_ecos_int_vector(struct eth_drv_sc *sc)
{
	return pcie_int_line;
}

#ifdef MBSS_SUPPORT
/* Interface: rai1 */
ETH_DRV_SC(devive_wireless_mbss_sc_i1,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "1",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i1,
                SECOND_INF_MAIN_DEV_NAME "1",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i1);
/* Interface: rai2 */
ETH_DRV_SC(devive_wireless_mbss_sc_i2,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "2",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i2,
                SECOND_INF_MAIN_DEV_NAME "2",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i2);

/* Interface: rai3 */
ETH_DRV_SC(devive_wireless_mbss_sc_i3,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "3",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i3,
                SECOND_INF_MAIN_DEV_NAME "3",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i3);

/* Interface: rai4 */
ETH_DRV_SC(devive_wireless_mbss_sc_i4,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "4",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i4,
                SECOND_INF_MAIN_DEV_NAME "4",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i4);

/* Interface: ra5 */
ETH_DRV_SC(devive_wireless_mbss_sc_i5,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "5",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i5,
                SECOND_INF_MAIN_DEV_NAME "5",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i5);

/* Interface: rai6 */
ETH_DRV_SC(devive_wireless_mbss_sc_i6,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "6",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i6,
                SECOND_INF_MAIN_DEV_NAME "6",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i6);

ETH_DRV_SC(devive_wireless_mbss_sc_i7,
           &rti_ecos_priv_data,
           SECOND_INF_MAIN_DEV_NAME "7",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev_i7,
                SECOND_INF_MAIN_DEV_NAME "7",
                rti_ecos_mbss_init,
                &devive_wireless_mbss_sc_i7);

static bool rti_ecos_mbss_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV 			pNetDev = NULL;
	UINT8				MacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	cyg_netdevtab_entry_t   *pNetdevEntry;
 	struct                  		eth_drv_sc *sc;
	char                    		devName[IFNAMSIZ];
	struct mt_dev_priv 	*priv_info = NULL;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_mbss_init()\n"));
	pNetDev = (struct eth_drv_sc *)tab->device_instance;

	sprintf(devName, "%s0", SECOND_INF_MAIN_DEV_NAME);

	for (pNetdevEntry = &__NETDEVTAB__[0]; pNetdevEntry != &__NETDEVTAB_END__; pNetdevEntry++)
	{
	        sc = (struct eth_drv_sc *)pNetdevEntry->device_instance;
	        if (strcmp(sc->dev_name, devName) == 0)
	        {
			pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);
			break;
	        }
	}
    
	if (pAd == NULL)
	{
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
		goto err_out;
	}

	/*NetDevInit============================================== */
	priv_info = (struct mt_dev_priv *) kmalloc(sizeof(struct mt_dev_priv), GFP_ATOMIC);
	pNetDev->driver_private = priv_info;
	
	/* put private data structure */
	RTMP_OS_NETDEV_SET_PRIV(pNetDev, pAd);

	/* Initialize upper level driver */
	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_mbss_init()\n"));
	return true;

err_out:
	return false;
}
#endif /* MBSS_SUPPORT */

#ifdef APCLI_SUPPORT
/* Interface: apclii0 */
ETH_DRV_SC(devive_wireless_apcli_sc_i0,
           &rti_ecos_priv_data,
           SECOND_INF_APCLI_DEV_NAME "0",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_apcli_netdev_i0,
                SECOND_INF_APCLI_DEV_NAME "0",
                rti_ecos_apcli_init,
                &devive_wireless_apcli_sc_i0);

static bool rti_ecos_apcli_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV			pNetDev = NULL;
	UINT8				MacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	cyg_netdevtab_entry_t	*pNetdevEntry;
	struct							eth_drv_sc *sc;
	char							devName[IFNAMSIZ];
	struct mt_dev_priv	*priv_info = NULL;
	
	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_apcli_init()\n"));
	pNetDev = (struct eth_drv_sc *)tab->device_instance;
	
	sprintf(devName, "%s0", SECOND_INF_MAIN_DEV_NAME);
	for (pNetdevEntry = &__NETDEVTAB__[0]; pNetdevEntry != &__NETDEVTAB_END__; pNetdevEntry++)
	{
		sc = (struct eth_drv_sc *)pNetdevEntry->device_instance;
		if (strcmp(sc->dev_name, devName) == 0)
		{
			pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);
			break;
		}
	}
		
	if (pAd == NULL)
	{
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
		goto err_out;
	}
	
	/*NetDevInit============================================== */
	priv_info = (struct mt_dev_priv *) kmalloc(sizeof(struct mt_dev_priv), GFP_ATOMIC);
	pNetDev->driver_private = priv_info;
		
	/* put private data structure */
	RTMP_OS_NETDEV_SET_PRIV(pNetDev, pAd);
	
	/* Initialize upper level driver */
	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);
	
	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_apcli_init()\n"));
	return true;
	
err_out:
	return false;
}
#endif /* APCLI_SUPPORT */

#ifdef WDS_SUPPORT
/* Interface: wdsi0 */
ETH_DRV_SC(devive_wireless_wds_sc_i0,
           &rti_ecos_priv_data,
           SECOND_INF_WDS_DEV_NAME "0",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev_i0,
                SECOND_INF_WDS_DEV_NAME "0",
                rti_ecos_wds_init,
                &devive_wireless_wds_sc_i0);
/* Interface: wdsi1 */
ETH_DRV_SC(devive_wireless_wds_sc_i1,
           &rti_ecos_priv_data,
           SECOND_INF_WDS_DEV_NAME "1",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev_i1,
                SECOND_INF_WDS_DEV_NAME "1",
                rti_ecos_wds_init,
                &devive_wireless_wds_sc_i1);
/* Interface: wdsi2 */
ETH_DRV_SC(devive_wireless_wds_sc_i2,
           &rti_ecos_priv_data,
           SECOND_INF_WDS_DEV_NAME "2",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev_i2,
                SECOND_INF_WDS_DEV_NAME "2",
                rti_ecos_wds_init,
                &devive_wireless_wds_sc_i2);
/* Interface: wdsi3 */
ETH_DRV_SC(devive_wireless_wds_sc_i3,
           &rti_ecos_priv_data,
           SECOND_INF_WDS_DEV_NAME "3",
           rti_ecos_start,
           rti_ecos_stop,
           rti_ecos_control,
           rti_ecos_can_send,
           rti_ecos_send,
           rti_ecos_recv,
           rti_ecos_deliver,
           rti_ecos_poll,
           rti_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev_i3,
                SECOND_INF_WDS_DEV_NAME "3",
                rti_ecos_wds_init,
                &devive_wireless_wds_sc_i3);

static bool rti_ecos_wds_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV			pNetDev = NULL;
	UINT8				MacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	cyg_netdevtab_entry_t	*pNetdevEntry;
	struct							eth_drv_sc *sc;
	char							devName[IFNAMSIZ];
	struct mt_dev_priv	*priv_info = NULL;
		
	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_wds_init()\n"));
	pNetDev = (struct eth_drv_sc *)tab->device_instance;
		
	sprintf(devName, "%s0", SECOND_INF_MAIN_DEV_NAME);
	for (pNetdevEntry = &__NETDEVTAB__[0]; pNetdevEntry != &__NETDEVTAB_END__; pNetdevEntry++)
	{
		sc = (struct eth_drv_sc *)pNetdevEntry->device_instance;
		if (strcmp(sc->dev_name, devName) == 0)
		{
			pAd = (PRTMP_ADAPTER) RtmpOsGetNetDevPriv(sc);
			break;
		}
	}
			
	if (pAd == NULL)
	{
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
		goto err_out;
	}
		
	/*NetDevInit============================================== */
	priv_info = (struct mt_dev_priv *) kmalloc(sizeof(struct mt_dev_priv), GFP_ATOMIC);
	pNetDev->driver_private = priv_info;
			
	/* put private data structure */
	RTMP_OS_NETDEV_SET_PRIV(pNetDev, pAd);
		
	/* Initialize upper level driver */
	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);
		
	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_wds_init()\n"));
	return true;
		
err_out:
	return false;
}
#endif /* WDS_SUPPORT */

