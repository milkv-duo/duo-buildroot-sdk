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

RTMP_ADAPTER *rt_ecos_priv_data;

ETH_DRV_SC(devive_wireless_sc0,
           &rt_ecos_priv_data,  /* Driver specific data */
           INF_MAIN_DEV_NAME "0",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_netdev0, 
                INF_MAIN_DEV_NAME "0",
                rt_ecos_init,
                &devive_wireless_sc0);

/*
 *  Interrupt handling - ISR and DSR
 */
static cyg_uint32 rt_wlan_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    rt2860_interrupt((void *) data);
   	cyg_interrupt_acknowledge(vector);
   	return CYG_ISR_CALL_DSR;
}

static void rt_wlan_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	/*  schecdule deliver routine  */
	if (data != 0)
		eth_drv_dsr(vector, count, (cyg_addrword_t) data);
    else
    	DBGPRINT(RT_DEBUG_ERROR, ("rt_wlan_dsr: data is NULL\n"));
}


static bool rt_ecos_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV 			pNetDev = NULL;
	POS_COOKIE			pOSCookie = NULL;
	ULONG				csr_address;
    UINT8				MacAddr[6];
    USHORT 				value;
	int					status;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_init()\n"));

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
	pAd->net_dev = pNetDev;
	pNetDev->driver_private = pAd;

	/* map physical address to virtual address for accessing register */
	csr_address = (unsigned long) RTMP_MAC_CSR_ADDR;
	pAd->CSRBaseAddress = (PUCHAR) csr_address;

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev = pNetDev;
#endif /* CONFIG_AP_SUPPORT */

	RtmpRaDevCtrlInit(pAd, RTMP_DEV_INF_RBUS);

	/*For the following RT28xx_EEPROM_READ16 */
#ifdef RT305x
	RT305x_AsicEeBufferInit(pAd);
#endif /* RT305x */
#ifdef RT3352
	RT3352_AsicEeBufferInit(pAd);
#endif /* RT3352 */
#ifdef RT5350
	RT5350_AsicEeBufferInit(pAd);
#endif /* RT5350 */

	if (pAd->chipOps.eeinit)
		pAd->chipOps.eeinit(pAd);

	/* Set up to handle interrupts */
	cyg_interrupt_create(RTMP_INTERRUPT_INIC,
                        PRIORITY_HIGH,
                        (cyg_addrword_t)pAd->net_dev,
                        (cyg_ISR_t *)rt_wlan_isr,
                        (cyg_DSR_t *)rt_wlan_dsr,
                        &rtmp_wlan_interrupt_handle,
                        &rtmp_wlan_interrupt);
	cyg_interrupt_attach(rtmp_wlan_interrupt_handle);
	cyg_interrupt_configure(RTMP_INTERRUPT_INIC, 1, 0);
    cyg_interrupt_unmask(RTMP_INTERRUPT_INIC);
    
    /* get MAC address */
	RT28xx_EEPROM_READ16(pAd, 0x04, value);
	MacAddr[0] = (UCHAR)(value & 0xff);
	MacAddr[1] = (UCHAR)(value >> 8);
	RT28xx_EEPROM_READ16(pAd, 0x06, value);
	MacAddr[2] = (UCHAR)(value & 0xff);
	MacAddr[3] = (UCHAR)(value >> 8);       
	RT28xx_EEPROM_READ16(pAd, 0x08, value);    
	MacAddr[4] = (UCHAR)(value & 0xff);
	MacAddr[5] = (UCHAR)(value >> 8);

	/* Initialize upper level driver */
	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_init()\n"));
	return true;

err_out:
	return false;
}

static void rt_ecos_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
        PNET_DEV		    pNetDev = NULL;
	POS_COOKIE			pOSCookie = NULL;    
	PRTMP_ADAPTER	    pAd = NULL;
        char                devName[IFNAMSIZ];
        int                 i, value;

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_start()\n"));

        /* if the device is runnung, do nothing */
        if (sc->state & ETH_DRV_STATE_ACTIVE)
                goto exit;

	pNetDev = sc;
	pAd = (PRTMP_ADAPTER)pNetDev->driver_private;
	pOSCookie = (POS_COOKIE) pAd->OS_Cookie;

	if (pAd == NULL) {
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
                goto exit;
        }

        i = 0;
#ifdef APCLI_SUPPORT
        for (i = 0; i < MAX_APCLI_NUM; i++)
        {
                sprintf(devName, "%s%d", INF_APCLI_DEV_NAME, i);
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
                sprintf(devName, "%s%d", INF_WDS_DEV_NAME, i);
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
                sprintf(devName, "%s%d", INF_MBSSID_DEV_NAME, i);
                if (strcmp(pNetDev->dev_name, devName) == 0)
                {
                        MBSS_VirtualIF_Open(sc);
                        return;
                }
        }
#endif /* MBSS_SUPPORT */

        sprintf(devName, "%s0", INF_MAIN_DEV_NAME);
        if (strcmp(pNetDev->dev_name, devName) != 0)
        {
		DBGPRINT_ERR(("%s: device name(%s) is not found\n", __FUNCTION__, pNetDev->dev_name));
                goto exit;    
        }
    
#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].bcn_buf.bBcnSntReq = TRUE;
#endif /* CONFIG_AP_SUPPORT */
	if (VIRTUAL_IF_UP(pAd) != 0)
		return;

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();

#ifdef PLATFORM_BUTTON_SUPPORT
    /* Polling reset button in APSOC */
#ifdef RT5350
	value = HAL_REG32(RTMP_SYS_CTL_ADDR + RTMP_SYS_GPIOMODE_OFFSET);
	value |= 0x001c;
	HAL_REG32(RTMP_SYS_CTL_ADDR + RTMP_SYS_GPIOMODE_OFFSET) = value;
	value = HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_POL_OFFSET);
	value &= 0xfffe;
	HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_POL_OFFSET) = value;
	pAd->CommonCfg.RestoreHdrBtnFlag = TRUE;
#endif

#ifdef RT6352
	value = HAL_REG32(RTMP_SYS_CTL_ADDR + RTMP_SYS_GPIOMODE_OFFSET);
	value |= 0x0001;
	value &= 0xffffdfff; //For WIFI LED
        HAL_REG32(RTMP_SYS_CTL_ADDR + RTMP_SYS_GPIOMODE_OFFSET) = value;
        value = HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_POL_OFFSET);
	value &= 0xfdf9;
	HAL_REG32(RTMP_PIO_CTL_ADDR + RTMP_PIO2100_POL_OFFSET) = value;
	pAd->CommonCfg.RestoreHdrBtnFlag = TRUE;
#endif

#ifdef WSC_INCLUDED
        /* Polling WPS button in APSOC */
	WSC_HDR_BTN_MR_HDR_SUPPORT_SET(pAd, 1);
#endif /* WSC_INCLUDED */
#endif /* PLATFORM_BUTTON_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_start()\n"));

exit:
    return;
}

static void  rt_ecos_stop(struct eth_drv_sc *sc)
{
	INT 			BssId;
	PNET_DEV		pNetDev = NULL;
	PRTMP_ADAPTER   pAd = NULL;
	POS_COOKIE		pOSCookie = NULL;
#ifdef APCLI_SUPPORT
    char            devName[IFNAMSIZ];
#endif /* APCLI_SUPPORT */
    
	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_stop()\n"));

    /* if the device is not runnung, do nothing */
    if ((sc->state & ETH_DRV_STATE_ACTIVE) == 0)
        return;
    
	pNetDev = sc;
	pAd = (RTMP_ADAPTER *)sc->driver_private;
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
    if (pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev != pNetDev) {
        MBSS_VirtualIF_Close(sc);
        return;
    }
#endif /* MBSS_SUPPORT */

	BssId = RT28xx_MBSS_IdxGet(pAd, pNetDev);
#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[BssId].bcn_buf.bBcnSntReq = FALSE;

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
    
	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_stop()\n"));  
}

static int rt_ecos_control(struct eth_drv_sc *sc, 
                          unsigned long cmd, 
                          void *data, 
                          int len)
{
	return 0;
}

static int rt_ecos_can_send(struct eth_drv_sc *sc)
{
    return 1;
}

//Copy from eCos kernel, remove the eth_drv_send(ifp);
static void rt_eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRESS key, int status)
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


static PECOS_PKT_BUFFER rt_convert_sglist_to_mbuf(struct eth_drv_sc *sc,
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

	pAd = (PRTMP_ADAPTER) sc->driver_private;
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
		rt_eth_drv_tx_done(sc,key,0);

        return pPacket;
}

static void rt_ecos_send(struct eth_drv_sc *sc,
				struct eth_drv_sg *sg_list,
				int sg_len,
				int total_len,
				unsigned long key)
{
	PECOS_PKT_BUFFER	pPacket = NULL;

    DBGPRINT(RT_DEBUG_INFO, ("==>rt_ecos_send()\n"));

    pPacket = rt_convert_sglist_to_mbuf(sc, sg_list, sg_len, total_len, key);
    if (pPacket != NULL)
        rt28xx_packet_xmit(pPacket);

    DBGPRINT(RT_DEBUG_INFO, ("<==rt_ecos_send()\n"));
    return;
}

static void rt_ecos_recv(struct eth_drv_sc *sc,
                        struct eth_drv_sg *sg_list,
                        int sg_len)
{
    /* Not Use */
	return;
} 

static void rt_ecos_deliver(struct eth_drv_sc *sc)
{
	POS_COOKIE			    pOSCookie = NULL;    
	PRTMP_ADAPTER	        pAd = NULL;
	PRTMP_NET_TASK_STRUCT   pNetTask = NULL;
	unsigned long flags;

	DBGPRINT(RT_DEBUG_INFO, ("rt_ecos_deliver()\n"));	
	pAd = (PRTMP_ADAPTER)sc->driver_private;
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

static void rt_ecos_poll(struct eth_drv_sc *sc)
{
	return;
}

static int  rt_ecos_int_vector(struct eth_drv_sc *sc)
{
	return RTMP_INTERRUPT_INIC;
}

#ifdef MBSS_SUPPORT
/* Interface: ra1 */
ETH_DRV_SC(devive_wireless_mbss_sc1,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "1",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev1,
                INF_MBSSID_DEV_NAME "1",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc1);
/* Interface: ra2 */
ETH_DRV_SC(devive_wireless_mbss_sc2,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "2",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev2,
                INF_MBSSID_DEV_NAME "2",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc2);

/* Interface: ra3 */
ETH_DRV_SC(devive_wireless_mbss_sc3,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "3",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev3,
                INF_MBSSID_DEV_NAME "3",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc3);

/* Interface: ra4 */
ETH_DRV_SC(devive_wireless_mbss_sc4,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "4",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev4,
                INF_MBSSID_DEV_NAME "4",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc4);

/* Interface: ra5 */
ETH_DRV_SC(devive_wireless_mbss_sc5,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "5",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev5,
                INF_MBSSID_DEV_NAME "5",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc5);

/* Interface: ra6 */
ETH_DRV_SC(devive_wireless_mbss_sc6,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "6",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev6,
                INF_MBSSID_DEV_NAME "6",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc6);

ETH_DRV_SC(devive_wireless_mbss_sc7,
           &rt_ecos_priv_data,
           INF_MBSSID_DEV_NAME "7",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_mbss_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_mbss_netdev7,
                INF_MBSSID_DEV_NAME "7",
                rt_ecos_mbss_init,
                &devive_wireless_mbss_sc7);

static bool rt_ecos_mbss_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV 			pNetDev = NULL;
    UINT8				    MacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    cyg_netdevtab_entry_t   *pNetdevEntry;
    struct                  eth_drv_sc *sc;
	char                    devName[IFNAMSIZ];
/*    int                     i; */

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_mbss_init()\n"));
	pNetDev = (struct eth_drv_sc *)tab->device_instance;

    sprintf(devName, "%s0", INF_MAIN_DEV_NAME);
    for (pNetdevEntry = &__NETDEVTAB__[0]; pNetdevEntry != &__NETDEVTAB_END__; pNetdevEntry++)
    {
        sc = (struct eth_drv_sc *)pNetdevEntry->device_instance;
        if (strcmp(sc->dev_name, devName) == 0)
        {
            pAd = (PRTMP_ADAPTER) sc->driver_private;
            break;
        }
	}
    
	if (pAd == NULL)
    {
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
		goto err_out;
    }

#if 0
    if (pAd->ApCfg.BssidNum == 0)
        RTMPReadParametersHook(pAd);

    for (i = 1; i < pAd->ApCfg.BssidNum; i++)
    {
        sprintf(devName, "%s%d", INF_MBSSID_DEV_NAME, i);
        if (strcmp(pNetDev->dev_name, devName) == 0)
        {
#endif
        	/* Initialize upper level driver */
        	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);

        	DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_mbss_init()\n"));
        	return true;
/*        } */
/*    } */
err_out:
	return false;
}

static void rt_ecos_mbss_send(struct eth_drv_sc *sc,
				struct eth_drv_sg *sg_list,
				int sg_len,
				int total_len,
				unsigned long key)
{
	PECOS_PKT_BUFFER	pPacket = NULL;

    DBGPRINT(RT_DEBUG_INFO, ("==>rt_ecos_mbss_send()\n"));

    pPacket = rt_convert_sglist_to_mbuf(sc, sg_list, sg_len, total_len, key);
    if (pPacket != NULL)
        rt28xx_send_packets(pPacket, sc);

	DBGPRINT(RT_DEBUG_INFO, ("<=== rt_ecos_mbss_send()\n"));
    return;
}
#endif /* MBSS_SUPPORT */

#ifdef APCLI_SUPPORT
/* Interface: apcli0 */
ETH_DRV_SC(devive_wireless_apcli_sc0,
           &rt_ecos_priv_data,
           INF_APCLI_DEV_NAME "0",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_apcli_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );

NETDEVTAB_ENTRY(devive_wireless_apcli_netdev0,
                INF_APCLI_DEV_NAME "0",
                rt_ecos_apcli_init,
                &devive_wireless_apcli_sc0);

static bool rt_ecos_apcli_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		pAd = NULL;
	PNET_DEV 			pNetDev = NULL;
    UINT8				    MacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    cyg_netdevtab_entry_t   *pNetdevEntry;
    struct                  eth_drv_sc *sc;
	char                    devName[IFNAMSIZ];
/*    int                     i; */

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_apcli_init()\n"));
	pNetDev = (struct eth_drv_sc *)tab->device_instance;

    sprintf(devName, "%s0", INF_MAIN_DEV_NAME);
    for (pNetdevEntry = &__NETDEVTAB__[0]; pNetdevEntry != &__NETDEVTAB_END__; pNetdevEntry++)
    {
        sc = (struct eth_drv_sc *)pNetdevEntry->device_instance;
        if (strcmp(sc->dev_name, devName) == 0)
        {
            pAd = (PRTMP_ADAPTER) sc->driver_private;
            break;
        }
	}
    
	if (pAd == NULL)
    {
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
		goto err_out;
    }

#if 0
    if (pAd->ApCfg.BssidNum == 0)
        RTMPReadParametersHook(pAd);

    for (i = 0; i < MAX_APCLI_NUM; i++)
    {
        sprintf(devName, "%s%d", INF_APCLI_DEV_NAME, i);
        if (strcmp(pNetDev->dev_name, devName) == 0)
        {
#endif
        	/* Initialize upper level driver */
        	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);

    	    DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_apcli_init()\n"));
        	return true;
/*        } */
/*    } */

err_out:
	return false;
}

static void rt_ecos_apcli_send(struct eth_drv_sc *sc,
				struct eth_drv_sg *sg_list,
				int sg_len,
				int total_len,
				unsigned long key)
{
	PECOS_PKT_BUFFER	pPacket = NULL;

    DBGPRINT(RT_DEBUG_INFO, ("==>rt_ecos_apcli_send()\n"));

    pPacket = rt_convert_sglist_to_mbuf(sc, sg_list, sg_len, total_len, key);
    if (pPacket != NULL)
        rt28xx_send_packets(pPacket, sc);

	DBGPRINT(RT_DEBUG_INFO, ("<=== rt_ecos_apcli_send()\n"));
    return;
}
#endif /* APCLI_SUPPORT */

#ifdef WDS_SUPPORT
/* Interface: wds0 */
ETH_DRV_SC(devive_wireless_wds_sc0,
           &rt_ecos_priv_data,
           INF_WDS_DEV_NAME "0",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_wds_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev0,
                INF_WDS_DEV_NAME "0",
                rt_ecos_wds_init,
                &devive_wireless_wds_sc0);
/* Interface: wds1 */
ETH_DRV_SC(devive_wireless_wds_sc1,
           &rt_ecos_priv_data,
           INF_WDS_DEV_NAME "1",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_wds_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev1,
                INF_WDS_DEV_NAME "1",
                rt_ecos_wds_init,
                &devive_wireless_wds_sc1);
/* Interface: wds2 */
ETH_DRV_SC(devive_wireless_wds_sc2,
           &rt_ecos_priv_data,
           INF_WDS_DEV_NAME "2",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_wds_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev2,
                INF_WDS_DEV_NAME "2",
                rt_ecos_wds_init,
                &devive_wireless_wds_sc2);
/* Interface: wds3 */
ETH_DRV_SC(devive_wireless_wds_sc3,
           &rt_ecos_priv_data,
           INF_WDS_DEV_NAME "3",
           rt_ecos_start,
           rt_ecos_stop,
           rt_ecos_control,
           rt_ecos_can_send,
           rt_ecos_wds_send,
           rt_ecos_recv,
           rt_ecos_deliver,
           rt_ecos_poll,
           rt_ecos_int_vector
           );
NETDEVTAB_ENTRY(devive_wireless_wds_netdev3,
                INF_WDS_DEV_NAME "3",
                rt_ecos_wds_init,
                &devive_wireless_wds_sc3);

static bool rt_ecos_wds_init(struct cyg_netdevtab_entry *tab)
{
	PRTMP_ADAPTER		    pAd = NULL;
	PNET_DEV 			    pNetDev = NULL;
    UINT8				    MacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    cyg_netdevtab_entry_t   *pNetdevEntry;
    struct                  eth_drv_sc *sc;
	char                    devName[IFNAMSIZ];
/*    int                     i; */

	DBGPRINT(RT_DEBUG_TRACE, ("===> rt_ecos_wds_init()\n"));
	pNetDev = (struct eth_drv_sc *)tab->device_instance;

    sprintf(devName, "%s0", INF_MAIN_DEV_NAME);
    for (pNetdevEntry = &__NETDEVTAB__[0]; pNetdevEntry != &__NETDEVTAB_END__; pNetdevEntry++)
    {
        sc = (struct eth_drv_sc *)pNetdevEntry->device_instance;
        if (strcmp(sc->dev_name, devName) == 0)
        {
            pAd = (PRTMP_ADAPTER) sc->driver_private;
            break;
        }
	}
    
	if (pAd == NULL)
    {
		DBGPRINT_ERR(("%s: pAd is NULL.\n", __FUNCTION__));
		goto err_out;
    }

#if 0
    if (pAd->ApCfg.BssidNum == 0)
        RTMPReadParametersHook(pAd);

    for (i = 0; i < MAX_WDS_ENTRY; i++)
    {
        sprintf(devName, "%s%d", INF_WDS_DEV_NAME, i);
        if (strcmp(pNetDev->dev_name, devName) == 0)
        {
#endif
        	/* Initialize upper level driver */
        	(pNetDev->funs->eth_drv->init)(pNetDev, MacAddr);

    	    DBGPRINT(RT_DEBUG_TRACE, ("<=== rt_ecos_wds_init()\n"));
        	return true;
/*        } */
/*    } */

err_out:
	return false;
}

static void rt_ecos_wds_send(struct eth_drv_sc *sc,
				struct eth_drv_sg *sg_list,
				int sg_len,
				int total_len,
				unsigned long key)
{
	PECOS_PKT_BUFFER	pPacket = NULL;

    DBGPRINT(RT_DEBUG_INFO, ("==>rt_ecos_wds_send()\n"));

    pPacket = rt_convert_sglist_to_mbuf(sc, sg_list, sg_len, total_len, key);
    if (pPacket != NULL)
        rt28xx_send_packets(pPacket, sc);

	DBGPRINT(RT_DEBUG_INFO, ("<=== rt_ecos_wds_send()\n"));
    return;
}
#endif /* WDS_SUPPORT */

