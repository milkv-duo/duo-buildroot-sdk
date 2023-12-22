#include <mips/config.h>
#include <init.h>
#include <mips/types.h>
#include <mips/addrspace.h>
#include <mips/io.h>
#include <delay.h>
#include <kernel.h>
#include <interrupt.h>
#include <assert.h>
#include <mips/ptrace.h>

#include <lwip/opt.h>
#include <lwip/def.h>
#include <lwip/pbuf.h>
#include <lwip/mem.h>
#include <lwip/ip.h>
#include <lwip/sys.h>
#include <arch/cc.h>

#include <netif/etharp.h>
#include <board/rt2880_int.h>

#include "rt_config.h"

#include <rt_pkt.h>

/*---------------------------------------------------------------------*/
/* Symbol & Macro Definitions                                          */
/*---------------------------------------------------------------------*/
#define FORTY_MHZ_INTOLERANT_INTERVAL	(60*1000) /* 1 min */

char unsigned enetaddr[6] =  {0x00, 0x00, 0x55, 0x99, 0x99, 0x99};

/*---------------------------------------------------------------------*/
/* Prototypes of External Functions                                    */
/*---------------------------------------------------------------------*/


/*---------------------------------------------------------------------*/
/* Prototypes of Functions Used                                        */
/*---------------------------------------------------------------------*/
/* function declarations */
static void rt2860_int_enable(PRTMP_ADAPTER pAd, unsigned int mode);
static irqreturn_t rt2860_interrupt(int irq, void *dev_instance, struct pt_regs *regs);
static int rt2860_open(struct netif *netif);
#ifdef CONFIG_USE_LWIP
static err_t rt2860_link_xmit(struct netif *netif, struct pbuf *pbuf);
static err_t rt2860_send_packet(struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr);
#endif /* CONFIG_USE_LWIP */
err_t rt2860_packet_xmit(PNDIS_PACKET p);
err_t rt2860_send_packets(struct netif *netif, PNDIS_PACKET p);

extern UINT8 NetJobAdd(void *fun, ...);

static void RT2860_Init(OUT PRTMP_ADAPTER *ppAdapter, IN struct netif *netif);

#ifdef CONFIG_AP_SUPPORT
#ifdef DFS_SUPPORT
static void pulse_radar_detect_proc(unsigned long data);
static void width_radar_detect_proc(unsigned long data);
static void carrier_sense_proc(unsigned long data);
#endif /* DFS_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


/*---------------------------------------------------------------------*/
/* External Variable Definitions                                       */
/*---------------------------------------------------------------------*/

extern RTMP_ADAPTER	rtmp_Adapter;
static RALINK_TIMER_STRUCT PeriodicTimer;

err_t rt2860_netif_init(struct netif *netif)
{

	netif->name[0] = 'r';
	netif->name[1] = 'a';
	netif->name[2] = '\0';
#ifdef CONFIG_USE_LWIP
	netif->output = rt2860_send_packet;
	netif->linkoutput = rt2860_link_xmit;
#endif
	netif->pkt_xmit = rt2860_send_packets;
	netif->hwaddr_len=6;

	NdisMoveMemory((void *) netif->hwaddr, (void *) enetaddr, 6);

	/* maximum transfer unit */
	netif->mtu = 1500;
  
	/* broadcast capability */
	netif->flags = NETIF_FLAG_BROADCAST;
	netif->priv_flags = INT_MAIN;
	
	/* hw dependent init */	

	rt2860_open(netif);

/*	etharp_init(); */

/*	sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL); */

	return (ERR_OK);
}


err_t rt2860_packet_xmit(PNDIS_PACKET p)
{
	struct net_pkt_blk *pkt = (struct net_pkt_blk *) p;
	struct netif *netif = pkt->dev;
	PRTMP_ADAPTER pAd;
	err_t status = ERR_MEM;
	PNDIS_PACKET pPacket = (PNDIS_PACKET) pkt;

	ASSERT(netif);
	pAd = (PRTMP_ADAPTER) netif->state;
	ASSERT(pAd);

	OSSchedLock();

    /* EapolStart size is 18 */
	if (pkt->len < 14)
	{
		/*printk("bad packet size: %d\n", pkt->len); */
		hex_dump("bad packet", pkt->data, pkt->len);
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		goto done;
	}
		
/*	MiniportMMRequest(pAd, pkt->data, pkt->len); */
#if 0	
	net_pkt_debug(1);
#endif

#ifdef CONFIG_STA_SUPPORT
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS) ||
		RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
	{
		/* Drop send request since hardware is in reset state */
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		goto done;
	}
	else if (!INFRA_ON(pAd) && !ADHOC_ON(pAd))
	{
		/* Drop send request since there are no physical connection yet */
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		goto done;
	}
#endif /* CONFIG_STA_SUPPORT */

	wdev_tx_pkts(pAd, (PPNDIS_PACKET)&pPacket, 1, wdev);

	status = ERR_OK;
done:
	OSSchedUnlock();			   
	return status;
}


static inline void pbuf_to_pkt(struct pbuf *pbuf, struct net_pkt_blk *pkt)
{
	NdisMoveMemory(pkt->data, pbuf->payload, pbuf->len);
	pkt->len = pbuf->len;
}

#ifdef CONFIG_USE_LWIP
static err_t rt2860_link_xmit(struct netif *netif, struct pbuf *pbuf)
{
#if 0
	struct net_pkt_blk	*new_pkt;

	new_pkt = net_pkt_alloc(NET_BUF_2048);

	if (new_pkt == NULL) {
		printk("!!! Tx Error: Can't allocate a new pkt !!!\n");		
		return (ERR_MEM);
	}

	pbuf_to_pkt(pbuf, new_pkt);

	hex_dump(__FUNCTION__, new_pkt->data, new_pkt->len);

	return rt2860_packet_xmit(netif, new_pkt);	
#else
	return (ERR_OK);
#endif
}
#endif /* CONFIG_USE_LWIP */

static void CfgInitHook(PRTMP_ADAPTER pAd)
{
	OID_SET_HT_PHYMODE HTPhyMode;


	pAd->bBroadComHT = TRUE;

#ifdef CONFIG_AP_SUPPORT
	NdisMoveMemory(pAd->ApCfg.MBSSID[BSS0].Ssid, "Sam_AP", 6);
	pAd->ApCfg.MBSSID[BSS0].SsidLen = 6;
#endif /* CONFIG_AP_SUPPORT */

     pAd->CommonCfg.BACapability.field.AmsduEnable = TRUE;
     pAd->CommonCfg.BACapability.field.MpduDensity = 4;
     pAd->CommonCfg.BACapability.field.RxBAWinLimit = 8;
     pAd->CommonCfg.RegTransmitSetting.field.ShortGI = GI_400;
	 pAd->CommonCfg.RegTransmitSetting.field.HTMODE  = HTMODE_GF;
	 pAd->CommonCfg.RegTransmitSetting.field.BW  = BW_40;
	 pAd->CommonCfg.RegTransmitSetting.field.MCS  = MCS_AUTO;
	 pAd->CommonCfg.RegTransmitSetting.field.STBC = STBC_NONE;

	 pAd->bLinkAdapt = FALSE;
	 pAd->CommonCfg.bRdg = TRUE;


#if 0
	{
		UCHAR *key = "12345678";
		UCHAR keyMaterial[40];
		int i;

		
		/* set WPAPSK */
		pAd->ApCfg.MBSSID[apidx].AuthMode = Ndis802_11AuthModeWPAPSK;

		/* set TKIP */
		pAd->ApCfg.MBSSID[apidx].WepStatus = Ndis802_11TKIPEnable;
	
		/* Init some variable */
		for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
		{
			if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]))
			{
				pAd->MacTab.tr_entry[i].PortSecured  = WPA_802_1X_PORT_NOT_SECURED;
			}
		}
		pAd->ApCfg.PortSecured = WPA_802_1X_PORT_NOT_SECURED;
    
		ApCfg.MBSSID[apidx].DefaultKeyId  = 0;
		if(pAd->ApCfg.MBSSID[apidx].AuthMode >= Ndis802_11AuthModeWPA)
		{   
			pAd->ApCfg.WpaGTKState = SETKEYS;
			pAd->ApCfg.GKeyDoneStations = pAd->MacTab.Size;
			ApCfg.MBSSID[apidx].DefaultKeyId = 1;		
		}

		/* set WPAPSK key */
		RtmpPasswordHash((CHAR *)key, pAd->ApCfg.MBSSID[apidx].Ssid, pAd->ApCfg.MBSSID[apidx].SsidLen, keyMaterial);

#ifdef CONFIG_STA_SUPPORT
		/*pAd->StaCfg.PskKey.KeyLen = 32; */
		NdisMoveMemory(pAd->StaCfg.PMK, keyMaterial, 32);
		/* Use RaConfig as PSK agent. */
		/* Start STA supplicant state machine */
		pAd->StaCfg.WpaState = SS_START;
		hex_dump("PMK", pAd->StaCfg.PMK, 32);


#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_AP_SUPPORT
    	NdisMoveMemory(pAd->ApCfg.PMK, keyMaterial, 32);
		hex_dump("PMK", pAd->ApCfg.PMK, 32);
#endif /* CONFIG_AP_SUPPORT */
		
		RTMPMakeRSNIE(pAd, pAd->ApCfg.MBSSID[apidx].AuthMode, pAd->ApCfg.MBSSID[apidx].WepStatus, apidx);
	}
#endif



#if 1
/*	pAd->ApCfg.MBSSID[apidx].wdev.bWmmCapable = TRUE; */
	pAd->CommonCfg.BACapability.field.AutoBA = TRUE;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 16;
#endif

#ifdef DOT11_N_SUPPORT
/*	pAd->CommonCfg.BACapability.field.AutoBA = TRUE; */
	pAd->CommonCfg.PhyMode = WMODE_GN;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 16;
	pAd->CommonCfg.BACapability.field.AmsduEnable = TRUE;

	HTPhyMode.BW = BW_40;
	HTPhyMode.PhyMode = PHY_11N_2_4G;
	HTPhyMode.ExtOffset = EXTCHA_NONE;
	HTPhyMode.TransmitNo = 1;
	HTPhyMode.HtMode = HTMODE_GF;
	HTPhyMode.SHORTGI = GI_400; 
	HTPhyMode.STBC = STBC_NONE;
	HTPhyMode.MCS = MCS_AUTO;

	if (WMODE_CAP_N(pAd->CommonCfg.PhyMode))
		RTMPSetHT(pAd, &HTPhyMode);

		
{
	extern	void tx_ba_enable(IN PRTMP_ADAPTER pAd);
/*	tx_ba_enable(pAd); */
}

#endif /* DOT11_N_SUPPORT */

	/*pAd->bStaFifoTest = TRUE; */
	rtmp_irq_init(pAd);
}


/*
	========================================================================
	
	Routine Description:
		Initialize transmit data structures

	Arguments:
		Adapter						Pointer to our adapter

	Return Value:
		None

	IRQL = PASSIVE_LEVEL

	Note:
		Initialize all transmit releated private buffer, include those define
		in RTMP_ADAPTER structure and all private data structures.
		
	========================================================================
*/
VOID NICInitTxRxRingAndBacklogQueue(
	IN RTMP_ADAPTER *pAd)
{
	int i;
	
	DBGPRINT(RT_DEBUG_TRACE, ("<--> NICInitTxRxRingAndBacklogQueue\n"));

	/* Initialize all transmit related software queues*/
	rtmp_tx_swq_init(pAd);

	/* Init RX Ring index pointer*/
	pAd->RxRing[0].RxSwReadIdx = 0;
	pAd->RxRing[0].RxCpuIdx = RX_RING_SIZE - 1;
	
	/* Init TX rings index pointer*/
	for (i=0; i<NUM_OF_TX_RING; i++)
	{
		pAd->TxRing[i].TxSwFreeIdx = 0;
		pAd->TxRing[i].TxCpuIdx = 0;
	}

	/* init MGMT ring index pointer*/
	pAd->MgmtRing.TxSwFreeIdx = 0;
	pAd->MgmtRing.TxCpuIdx = 0;
}


static void RT2860_Init(
	OUT	PRTMP_ADAPTER *ppAdapter,
	IN	struct netif *netif)
{
	NDIS_STATUS			Status;
	PRTMP_ADAPTER		pAd = NULL;
	UINT				index;
	UCHAR				TmpPhy;
	ULONG				Value;
#ifdef CONFIG_AP_SUPPORT
    OID_SET_HT_PHYMODE	SetHT;
#endif /* CONFIG_AP_SUPPORT */
    PVOID		    	*handle;
	WPDMA_GLO_CFG_STRUC     GloCfg;


	handle = malloc(sizeof(struct os_cookie));
	memset(handle, 0, sizeof(struct os_cookie));
	((POS_COOKIE)handle)->pci_dev = netif;

	/* Allocate RTMP_ADAPTER miniport adapter structure */
	Status = RTMPAllocAdapterBlock(handle, &pAd);
	if (Status != NDIS_STATUS_SUCCESS) {
		BUG();
	}

	ASSERT(*ppAdapter);

	*ppAdapter = pAd;

#ifdef CONFIG_AP_SUPPORT
	pAd->ApCfg.MBSSID[MAIN_MBSSID].MSSIDDev = netif;
#endif /* CONFIG_AP_SUPPORT */
	pAd->net_dev = netif;

	pAd->CSRBaseAddress = (PCHAR)RT2860_CSR_ADDR;

	
/* reset Adapter flags */
	RTMP_CLEAR_FLAGS(pAd);

	/* Allocate BA Reordering memory */
	ba_reordering_resource_init(pAd, MAX_REORDERING_MPDU_NUM);
	
#ifdef CONFIG_AP_SUPPORT	
#ifdef AUTO_CH_SELECT_ENHANCE
	AutoChBssTableInit(pAd);
	ChannelInfoInit(pAd);
#endif /* AUTO_CH_SELECT_ENHANCE */
#endif

	/* Find the physical adapter */
/*	pAd->DeviceID = RT2860_PCI_DEVICE_ID; */

	/* */
	/* 2600 MAC/BBP/RF registers become accessible from now on */
	/* after CSR base address decided */
	/* */
	/* */
	/* Init TX/RX data structures and related parameters */
	/* */
	NICInitTxRxRingAndBacklogQueue(pAd);

	/* */
	/* Make sure MAC gets ready. */
	/* */
	index = 0;
	WaitForAsicReady(pAd);
	DBGPRINT(RT_DEBUG_TRACE, ("%s():MacVersion[ Ver:Rev=0x%08lx]\n",
		__FUNCTION__, pAd->MACVersion));

	/* Disable DMA. */
	AsicSetWPDMA(pAd, PDMA_TX_RX, FALSE);

	/* Load 8051 firmware; */
#if 0
	Status = NICLoadFirmware(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("NICLoadFirmware failed, Status[=0x%08x]\n", Status));
		goto err;
	}
#else
	Status = NDIS_STATUS_SUCCESS;
#endif

	/* Disable interrupts here which is as soon as possible */
	/* This statement should never be true. We might consider to remove it later */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE))
	{
		RTMP_ASIC_INTERRUPT_DISABLE(pAd);
	}

	Status = RTMPAllocDMAMemory(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("RTMPAllocDMAMemory failed, Status[=0x%08x]\n", Status));
		goto err;
	}

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);

	/* initialize MLME */
	Status = MlmeInit(pAd);
	if(Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("MlmeInit failed, Status[=0x%08x]\n", Status));
		goto err;
	}

	/* Initialize pAd->StaCfg, pAd->ApCfg, pAd->CommonCfg to manufacture default */
	UserCfgInit(pAd);

#ifdef CONFIG_AP_SUPPORT
	pAd->OpMode = OPMODE_AP;
#endif /* CONFIG_AP_SUPPORT */
/*	COPY_MAC_ADDR(pAd->ApCfg.MBSSID[apidx].Bssid, netif->hwaddr); */
/*	pAd->bForcePrintTX = TRUE; */

	pAd->CommonCfg.BeaconPeriod = 50;

/*	COPY_MAC_ADDR(pAd->ApCfg.MBSSID[apidx].Bssid, netif->hwaddr); */
/*	pAd->bForcePrintTX = TRUE; */

	CfgInitHook(pAd);

#ifdef CONFIG_AP_SUPPORT			  
	APInitialize(pAd);
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
	/* Already init in UserCfgInit(); */
	/*NdisAllocateSpinLock(pAd, &pAd->MacTabLock); */
	pAd->OpMode = OPMODE_STA;
#endif /* CONFIG_STA_SUPPORT */

#ifdef WLAN_LOOPBACK
/*	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, 0x20); */
#endif


	/*
		Init the hardware, we need to init asic before read registry, otherwise mac register will be reset
	*/
	Status = NICInitializeAdapter(pAd, TRUE);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("NICInitializeAdapter failed, Status[=0x%08x]\n", Status));
		goto err;
	}


	/* Read parameters from Config File */
	Status = RTMPReadParametersHook(pAd);
	if (Status != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT_ERR(("NICReadRegParameters failed, Status[=0x%08x]\n", Status));
		goto err;
	}

#ifdef CONFIG_STA_SUPPORT
/*	pAd->StaCfg.bAutoReconnect = TRUE; */
	NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, pAd->CommonCfg.Ssid, pAd->CommonCfg.SsidLen);
	pAd->MlmeAux.AutoReconnectSsidLen = pAd->CommonCfg.SsidLen;
#endif /* CONFIG_STA_SUPPORT */

   	/*Init Ba Capability parameters. */
	pAd->CommonCfg.DesiredHtPhy.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;
	pAd->CommonCfg.DesiredHtPhy.AmsduEnable = (USHORT)pAd->CommonCfg.BACapability.field.AmsduEnable;
	pAd->CommonCfg.DesiredHtPhy.AmsduSize= (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.DesiredHtPhy.MimoPs= (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	/* UPdata to HT IE */
	pAd->CommonCfg.HtCapability.HtCapInfo.MimoPs = (USHORT)pAd->CommonCfg.BACapability.field.MMPSmode;
	pAd->CommonCfg.HtCapability.HtCapInfo.AMsduSize = (USHORT)pAd->CommonCfg.BACapability.field.AmsduSize;
	pAd->CommonCfg.HtCapability.HtCapParm.MpduDensity = (UCHAR)pAd->CommonCfg.BACapability.field.MpduDensity;


	/* after reading Registry, we now know if in AP mode or STA mode */

	/* Load 8051 firmware; crash when FW image not existent */
	/* Status = NICLoadFirmware(pAd); */
	/* if (Status != NDIS_STATUS_SUCCESS) */
	/*    break; */
	printk("2. Phy Mode = %d\n", pAd->CommonCfg.PhyMode);

	/* We should read EEPROM for all cases.  rt2860b */
	NICReadEEPROMParameters(pAd, NULL);

	printk("3. Phy Mode = %d\n", pAd->CommonCfg.PhyMode);

	/* Set PHY to appropriate mode */
	RTMPSetPhyMode(pAd, pAd->CommonCfg.PhyMode);

	printk("MCS Set = %02x %02x %02x %02x %02x\n", pAd->CommonCfg.HtCapability.MCSSet[0],
           pAd->CommonCfg.HtCapability.MCSSet[1], pAd->CommonCfg.HtCapability.MCSSet[2],
           pAd->CommonCfg.HtCapability.MCSSet[3], pAd->CommonCfg.HtCapability.MCSSet[4]);
	NICInitAsicFromEEPROM(pAd); /*rt2860b */

	AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
	AsicLockChannel(pAd, pAd->CommonCfg.Channel);

	if (pAd && (Status != NDIS_STATUS_SUCCESS))
	{
		BOOLEAN Cancelled;

		/* Undo everything if it failed */

		/*RTMPCancelTimer(&pAd->RfTuningTimer, &Cancelled); */
		RTMPCancelTimer(&pAd->Mlme.PeriodicTimer, &Cancelled);
		RTMPCancelTimer(&pAd->Mlme.APSDPeriodicTimer, &Cancelled);

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE))
		{
/*			NdisMDeregisterInterrupt(&pAd->Interrupt); */
			RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE);
		}
		RTMPFreeAdapter(pAd);
	}
	else if (pAd)
	{
		/* Microsoft HCT require driver send a disconnect event after driver initialization. */
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED);
	
		DBGPRINT(RT_DEBUG_TRACE, ("NDIS_STATUS_MEDIA_DISCONNECT Event B!\n"));

#ifdef CONFIG_AP_SUPPORT
		if (pAd->OpMode == OPMODE_AP)
		{
			if (pAd->ApCfg.bAutoChannelAtBootup || (pAd->CommonCfg.Channel == 0))
			{
				/* Enable Interrupt */
				/*pAd->bStaFifoTest = TRUE; */
				rtmp_irq_init(pAd);

				RTMP_IRQ_ENABLE(pAd);
				/* Now Enable RxTx */
				RTMPEnableRxTx(pAd);
				/* Now we can receive the beacon and do the listen beacon */
				pAd->CommonCfg.Channel = AP_AUTO_CH_SEL(pAd, TRUE);
			}

#ifdef DOT11_N_SUPPORT
			RTMPInitTimer(pAd, &PeriodicTimer, GET_TIMER_FUNCTION(APDetectOverlappingExec), pAd, TRUE);
			RTMPSetTimer(&PeriodicTimer, FORTY_MHZ_INTOLERANT_INTERVAL);

			/* If WMODE_CAP_N(phymode) and BW=40 check extension channel, after select channel */
			N_ChannelCheck(pAd);
#endif /* DOT11_N_SUPPORT */
			
			APStartUp(pAd);
#if 0 /* Dennis Lee */
			if (pAd->ate2880.mode == ATE_2880_APSTART)			
				APStartUp(pAd);
			else
				printk("skip APStartUp due to ate mode\n");
#endif /* */
			
   		
			printk("Main bssid = %02x:%02x:%02x:%02x:%02x:%02x\n", 
						PRINT_MAC(pAd->ApCfg.MBSSID[0].Bssid));

		}

#ifdef WSC_AP_SUPPORT
        WscInit(pAd);
#endif /* WSC_AP_SUPPORT */

#endif /* CONFIG_AP_SUPPORT */

		rtmp_irq_init(pAd);

#ifdef CONFIG_AP_SUPPORT
#ifdef MBSS_SUPPORT
	/* the function can not be moved to RT2860_probe() even register_netdev()
	   is changed as register_netdevice().
	   Or in some PC, kernel will panic (Fedora 4) */
    RT28xx_MBSS_Init(pAd, net_dev);
#endif /* MBSS_SUPPORT */

#ifdef WDS_SUPPORT
	RT28xx_WDS_Init(pAd, net_dev);
#endif /* WDS_SUPPORT */

#ifdef MAT_SUPPORT
	MATEngineInit(pAd);
#endif /* MAT_SUPPORT */

#ifdef APCLI_SUPPORT
	RT28xx_ApCli_Init(pAd, net_dev);
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	}

	DBGPRINT_S(("<==== rt28xx_init, Status=%x\n", Status));

	return;
err:
	printk("!!! RT28xx Initialized fail !!!\n");
}

static int rt2860_open(struct netif *netif)
{				 
	PRTMP_ADAPTER pAd;
	int retval = 0;

	/* register the interrupt routine with the os */
	if ((retval = request_irq(IRQ_WLAN_NO, rt2860_interrupt, SA_SHIRQ, (void *)NULL, (void *)netif))) {
		printk("GE: request_irq  ERROR(%d)\n", retval);
		return retval;
	}

	RT2860_Init(&pAd, netif);


	pAd->CommonCfg.BACapability.field.AutoBA = TRUE;
	pAd->CommonCfg.BACapability.field.RxBAWinLimit = 16;
/*	pAd->ApCfg.MBSSID[apidx].wdev.bWmmCapable = TRUE; */

	/*pAd->bStaFifoTest = TRUE; */
		rtmp_irq_init(pAd);
		/* Enable Interrupt */
		RTMP_IRQ_ENABLE(pAd);

		/* Now Enable RxTx */
		RTMPEnableRxTx(pAd);
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_START_UP);

#ifdef CONFIG_STA_SUPPORT
	pAd->StaCfg.bAutoReconnect = TRUE;
	NdisMoveMemory(pAd->MlmeAux.AutoReconnectSsid, "Sam_AP", 6);
	pAd->MlmeAux.AutoReconnectSsidLen = 6;
#endif /* CONFIG_STA_SUPPORT */

		


#if 0
{
	extern	void tx_ba_enable(IN PRTMP_ADAPTER pAd);
	TX_LINK_CFG_STRUC  TxLinkCfg;
    
	RTMP_IO_READ32(pAd, TX_LINK_CFG, &TxLinkCfg.word);
	TxLinkCfg.field.TxRDGEn = 1;
	RTMP_IO_WRITE32(pAd, TX_LINK_CFG, TxLinkCfg.word);

	tx_ba_enable(pAd);
}
#endif

	RTDebugLevel = RT_DEBUG_TRACE; 
	RTDebugFunc = 0;
{
/*	UINT32 reg; */
/*	UINT8  byte; */
/*	UINT16 tmp; */

/*	RTMP_IO_READ32(pAd, XIFS_TIME_CFG, &reg); */

/*	tmp = 0x0805; */
/*	reg  = (reg & 0xffff0000) | tmp; */
/*	RTMP_IO_WRITE32(pAd, XIFS_TIME_CFG, reg); */

}
	
	pAd->CommonCfg.bAggregationCapable = TRUE;
	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_TX_AMSDU_INUSED);
	OPSTATUS_SET_FLAG(pAd, fOP_STATUS_WMM_INUSED);

	netif->state = pAd;

#if 1
	/* 
	 * debugging helper
	 * 		show the size of main table in Adapter structure
	 *		MacTab  -- 185K
	 *		BATable -- 137K
	 * 		Total 	-- 385K  !!!!! (5/26/2006)
	 */
	printk("\n\n==============================================================\n\n");
	printk("=== pAd = %p, size = %ld ===\n\n", pAd, sizeof(RTMP_ADAPTER));
	printk("sizeof(pAd->MacTab) = %ld [%ld * <%ld:sizeof(MAC_TABLE_ENTRY)>]\n", sizeof(pAd->MacTab),
		MAX_LEN_OF_MAC_TABLE, sizeof(MAC_TABLE_ENTRY));
	printk("sizeof(pAd->AccessControlList) = %ld\n", sizeof(pAd->AccessControlList));
#ifdef CONFIG_STA_SUPPORT
	printk("sizeof(pAd->StaCfg) = %ld\n", sizeof(pAd->StaCfg));
	printk("sizeof(pAd->StaActive) = %ld\n", sizeof(pAd->StaActive));
	printk("sizeof(pAd->ScanTab) = %ld\n", sizeof(pAd->ScanTab));
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT	
	printk("sizeof(pAd->ApCfg) = %ld\n", sizeof(pAd->ApCfg));
#endif /* CONFIG_AP_SUPPORT */
	printk("sizeof(pAd->BATable) = %ld\n", sizeof(pAd->BATable));	
	printk("\n==============================================================\n\n");
/*	BUG(); */
#endif 

#if 0
	pAd->OpMode = OPMODE_AP;
	pAd->bForcePrintTX = INFO;
	COPY_MAC_ADDR(pAd->ApCfg.MBSSID[apidx].Bssid, enetaddr);
#endif


	return (retval);
}


#ifdef CONFIG_USE_LWIP
err_t
rt2860_send_packet(struct netif *netif, struct pbuf *p, struct ip_addr *ipaddr)
{
	return etharp_output(netif, ipaddr, p);
}
#endif

static void rt2860_int_enable(PRTMP_ADAPTER pAd, unsigned int mode)
{
	UINT32 regValue;

	pAd->int_disable_mask &= ~(mode);
	regValue = pAd->int_enable_reg & ~(pAd->int_disable_mask);		
	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
	{
		RTMP_IO_WRITE32(pAd, INT_MASK_CSR, regValue);     /* 1:enable */
	}
	else
		DBGPRINT(RT_DEBUG_TRACE, ("fOP_STATUS_DOZE !\n"));

	if (regValue != 0)
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
}


static void rt2860_int_disable(PRTMP_ADAPTER pAd, unsigned int mode)
{
	UINT32 regValue;

	pAd->int_disable_mask |= mode;
	regValue = pAd->int_enable_reg & ~(pAd->int_disable_mask);
	RTMP_IO_WRITE32(pAd, INT_MASK_CSR, regValue);     /* 0: disable */
	if (regValue == 0) {
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
	}
}


void APRxDoneIntProcess(PRTMP_ADAPTER pAd)
{
	unsigned long flags;
	BOOLEAN	bReschedule;

	pAd->int_pending &= ~(RTMP_INT_RX); 
	bReschedule = rtmp_rx_done_handle(pAd);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid rotting packet */
	if (pAd->int_pending & RTMP_INT_RX || bReschedule) 
	{
		NetJobAdd(APRxDoneIntProcess, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_RX);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);

}

void FifoStaCountersProcess(PRTMP_ADAPTER pAd)
{
	unsigned long flags;

	pAd->int_pending &= ~(RTMP_FifoStaFullInt); 
	NICUpdateFifoStaCounters(pAd);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid rotting packet */
	if (pAd->int_pending & RTMP_FifoStaFullInt) 
	{
		NetJobAdd(FifoStaCountersProcess, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_FifoStaFullInt);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);

}

static void mgmt_dma_done_process(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	pAd->int_pending &= ~RTMP_INT_MGMT_DLY;
	RTMPHandleMgmtRingDmaDoneInterrupt(pAd);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_MGMT_DLY) {
		NetJobAdd(mgmt_dma_done_process, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_MGMT_DLY);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);	
}


static void hcca_dma_done_process(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	pAd->int_pending &= ~RTMP_INT_HCCA_DLY;
	RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_HCCA_DONE);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_HCCA_DLY) {
		NetJobAdd(hcca_dma_done_process, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_HCCA_DLY);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);	
}

static void ac3_dma_done_process(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	pAd->int_pending &= ~RTMP_INT_AC3_DLY;
	RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC3_DONE);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_AC3_DLY) {
		NetJobAdd(ac3_dma_done_process, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC3_DLY);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);	
}

static void ac2_dma_done_process(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	pAd->int_pending &= ~RTMP_INT_AC2_DLY;
	RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC2_DONE);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_AC2_DLY) {
		NetJobAdd(ac2_dma_done_process, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC2_DLY);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);	
}

static void ac1_dma_done_process(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	pAd->int_pending &= ~RTMP_INT_AC1_DLY;
	RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC1_DONE);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_AC1_DLY) {
		NetJobAdd(ac1_dma_done_process, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC1_DLY);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);	
}

static void ac0_dma_done_process(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	pAd->int_pending &= ~RTMP_INT_AC0_DLY;
	RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC0_DONE);

	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);	
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_AC0_DLY) 
	{
		NetJobAdd(ac0_dma_done_process, pAd, 0, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC0_DLY);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);	
}


#ifdef CONFIG_AP_SUPPORT
#ifdef DFS_SUPPORT
static void pulse_radar_detect_proc(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	RadarSMDetect(pAd, RADAR_PULSE);
}

static void width_radar_detect_proc(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	RadarSMDetect(pAd, RADAR_WIDTH);
}
#endif /* DFS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
static void carrier_sense_proc(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	CarrierDetectionCheck(pAd);
}
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

void APTxDataIntProcess(PRTMP_ADAPTER pAd, RTMP_INT_SOURCE_CSR_STRUC TxRingBitmap)
{
	unsigned long flags;

	pAd->int_pending &= ~(TxRingBitmap.word);
/*	RTMPHandleMgmtRingDmaDoneInterrupt(pAd); */
	RTMPHandleTxRingDmaDoneInterrupt(pAd, TxRingBitmap);


	RTMP_IRQ_LOCK(&pAd->irq_lock, flags);
	/*
	 * double check to avoid lose of interrupts
	 */
	if (pAd->int_pending & TxDataInt) {
		TxRingBitmap.word = pAd->int_pending;
		NetJobAdd(APTxDataIntProcess, pAd, TxRingBitmap, 0);
		RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	/* enable TxDataInt again */
	rt2860_int_enable(pAd, TxDataInt);
	RTMP_IRQ_UNLOCK(&pAd->irq_lock, flags);
}

void rt2860_show()
{
	UINT32 reg;

	reg = readl(RTMP_MAC_CSR_ADDR + RX_STA_CNT2);
	printk("RX_STA_CNT2 = %08x\n", reg);					 	
}


int print_int_count;

static unsigned long rx_time;
static unsigned long tx_time;

static irqreturn_t rt2860_interrupt(int irq, void *dev_instance, struct pt_regs *regs)
{
	struct netif *netif = (struct netif *) dev_instance;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) netif->state;
	UINT32 IntSource;


	/* Inital the Interrupt source. */
	IntSource = 0x00000000L;

	/* Get the interrupt sources & saved to local variable */

	/*
		Flag fOP_STATUS_DOZE On, means ASIC put to sleep, elase means ASICK WakeUp
		And at the same time, clock maybe turned off that say there is no DMA service.
		when ASIC get to sleep.
		To prevent system hang on power saving.
		We need to check it before handle the INT_SOURCE_CSR, ASIC must be wake up.
	*/
	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
	{
		RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource);
		RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource); /* write 1 to clear */
	}
	else
		DBGPRINT(RT_DEBUG_TRACE, (">>>fOP_STATUS_DOZE<<<\n"));

	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_START_UP))
	{
		return;
	}

	/*
		Handle interrupt, walk through all bits
		Should start from highest priority interrupt
		The priority can be adjust by altering processing if statement
	*/
#if 0
{
	UINT32 regValue;

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &regValue);
	printk("MAC_SYS_CTRL = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x400, &regValue);
	printk("MCU 0x400 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x404, &regValue);
	printk("MCU 0x404 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x408, &regValue);
	printk("MCU 0x408 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x40C, &regValue);
	printk("MCU 0x40C = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x410, &regValue);
	printk("MCU 0x410 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x414, &regValue);
	printk("MCU_INT_STA = %08x\n", regValue);
}
#endif 

#if 0
	if (((++print_int_count) % 1500) == 0) 
	{
		UINT32 reg;
		int Count, free;

		RTMP_IO_READ32(pAd, INT_MASK_CSR, &reg);     /* 1:enable */
		printk("%d: INT_MASK_CSR = %08x, IntSource %08x\n", print_int_count, reg, IntSource);
		RTMP_IO_READ32(pAd, TX_CTX_IDX0 + 0 * 0x10 , &reg);
		printk("TX_CTX_IDX0 = %08x\n", reg);
		RTMP_IO_READ32(pAd, TX_DTX_IDX0 + 0 * 0x10 , &reg);
		printk("TX_DTX_IDX0 = %08x\n", reg);
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &reg);
		printk("WPDMA_GLO_CFG = %08x\n", reg);
		for (Count = 0; Count < 1; Count++)
		{
			if (pAd->TxRing[Count].TxSwFreeIdx> pAd->TxRing[Count].TX_CTX_IDX)
				free = pAd->TxRing[Count].TxSwFreeIdx - pAd->TxRing[Count].TX_CTX_IDX -1;
			else
				free = pAd->TxRing[Count].TxSwFreeIdx + TX_RING_SIZE - pAd->TxRing[Count].TX_CTX_IDX -1;
		
			printk("%d: Free = %d TxSwFreeIdx = %d\n", Count, free, pAd->TxRing[Count].TxSwFreeIdx); 
		}
		printk("pAd->int_disable_mask = %08x\n", pAd->int_disable_mask);
		printk("pAd->int_enable_reg = %08x\n", pAd->int_enable_reg);
		printk("pAd->int_pending = %08x\n", pAd->int_pending);
		printk("Rx Jobs = %d\n", rx_process);
		RTMP_IO_READ32(pAd, RX_DRX_IDX , &reg);
		printk("pAd->RxRing[0].RxSwReadIdx = %08x, RX_DRX_IDX = %08x\n", pAd->RxRing[0].RxSwReadIdx, reg);
/*		NetJobAdd(APRxDoneIntProcess, pAd, 0, 0); */
	}
#endif
		

	if (IntSource & RTMP_PreTBTTInt)
	{
		RTMPHandlePreTBTTInterrupt(pAd);
	}
	if (IntSource & RTMP_TBTTInt)
	{
		RTMPHandleTBTTInterrupt(pAd);
	}

#ifdef CONFIG_STA_SUPPORT
    if (IntSource & RTMP_AutoWakeupInt)
		RTMPHandleTwakeupInterrupt(pAd);
#endif /* CONFIG_STA_SUPPORT */

	if (IntSource & RTMP_TxCoherent)
	{
		DBGPRINT(RT_DEBUG_ERROR, (">>>TxCoherent<<<\n"));
		RTMPHandleRxCoherentInterrupt(pAd);
	}

	if (IntSource & RTMP_RxCoherent)
	{
		DBGPRINT(RT_DEBUG_ERROR, (">>>RxCoherent<<<\n"));
		RTMPHandleRxCoherentInterrupt(pAd);
	}

	if (IntSource & RTMP_FifoStaFullInt) {
		if ((pAd->int_disable_mask & RTMP_FifoStaFullInt) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_FifoStaFullInt);
			NetJobAdd(FifoStaCountersProcess, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_FifoStaFullInt; 
	}


	if (IntSource & RTMP_INT_MGMT_DLY) {
		if ((pAd->int_disable_mask & RTMP_INT_MGMT_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_MGMT_DLY);
			NetJobAdd(mgmt_dma_done_process, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_INT_MGMT_DLY; 
	}

	if (IntSource & RTMP_INT_RX)
	{
		if (time_after(jiffies, (rx_time+10*OS_HZ)))
		{
			printk("Now Rx Time = %ld, Last Rx Time = %ld\n", jiffies, rx_time);
		}
		rx_time = jiffies;

		if (1)/*if (pAd->OpMode == OPMODE_AP) */
		{
			if ((pAd->int_disable_mask & RTMP_INT_RX) == 0) 
			{
				rt2860_int_disable(pAd, RTMP_INT_RX);
				NetJobAdd(APRxDoneIntProcess, pAd, 0, 0);
			}
			pAd->int_pending |= RTMP_INT_RX; 
		}		
	}


#if 1
	if (IntSource & RTMP_INT_HCCA_DLY) {
		if ((pAd->int_disable_mask & RTMP_INT_HCCA_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_HCCA_DLY);
			NetJobAdd(hcca_dma_done_process, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_INT_HCCA_DLY;						
	}

	if (IntSource & RTMP_INT_AC3_DLY) {
		if ((pAd->int_disable_mask & RTMP_INT_AC3_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC3_DLY);
			NetJobAdd(ac3_dma_done_process, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_INT_AC3_DLY;						
	}

	if (IntSource & RTMP_INT_AC2_DLY) {
		if ((pAd->int_disable_mask & RTMP_INT_AC2_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC2_DLY);
			NetJobAdd(ac2_dma_done_process, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_INT_AC2_DLY;						
	}

	if (IntSource & RTMP_INT_AC1_DLY) {
		if ((pAd->int_disable_mask & RTMP_INT_AC1_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC1_DLY);
			NetJobAdd(ac1_dma_done_process, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_INT_AC1_DLY;						
	}



	if (IntSource & RTMP_INT_AC0_DLY) {
		if (time_after(jiffies,(tx_time+10*OS_HZ)))
		{
			printk("Now Tx Time = %ld, Last Tx Time = %ld\n", jiffies, tx_time);
		}
		tx_time = jiffies;

/*
		if (IntSource.word & 0x2) {
			UINT32 reg;
			RTMP_IO_READ32(pAd, DELAY_INT_CFG, &reg);
			printk("IntSource.word = %08x, DELAY_REG = %08x\n", IntSource.word, reg);
		}
*/

		if ((pAd->int_disable_mask & RTMP_INT_AC0_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC0_DLY);
			NetJobAdd(ac0_dma_done_process, pAd, 0, 0);
		}
		pAd->int_pending |= RTMP_INT_AC0_DLY;						
	}

#ifdef CONFIG_AP_SUPPORT
#ifdef CARRIER_DETECTION_SUPPORT
#ifdef TONE_RADAR_DETECT_SUPPORT
	if (IntSource & RTMP_RadarInt)
	{
		RTMPHandleRadarInterrupt(pAd);
	}
#endif /* TONE_RADAR_DETECT_SUPPORT */
#endif /* CARRIER_DETECTION_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#else
#if 1
	if (IntSource.word & TxDataInt) {

		if (((IntSource.word & TxDataInt) & ~(0x0a)) != 0) {
			printk("!!! %08x \n", IntSource.word & TxDataInt);
		}
		if ((pAd->int_disable_mask & TxDataInt) == 0) 
		{
			/* mask TxDataInt */
			rt2860_int_disable(pAd, TxDataInt);
			NetJobAdd(APTxDataIntProcess, pAd, TxDataInt, 0);
		}
		pAd->int_pending |= IntSource.word & TxDataInt;			
#else
/*		NetJobAdd(APTxDataIntProcess, pAd, IntSource, 0); */

		NetJobAdd(RTMPHandleTxRingDmaDoneInterrupt, pAd, IntSource, 0);
#endif
		if (pAd->bStaFifoTest == TRUE)
			DBGPRINT(RT_DEBUG_TRACE, (">>>TxDataInt=0xfa<<< receive 0x%x\n", IntSource));
		/*RTMPHandleTxRingDmaDoneInterrupt(pAd, IntSource); */
	}
#endif

#ifdef CONFIG_AP_SUPPORT
	if (IntSource & RTMP_McuCommand)
	{
		UINT32 McuIntSrc;

		rt2860_int_disable(pAd, RTMP_McuCommand);
		RTMP_IO_READ32(pAd, 0x7024, &McuIntSrc);

		/* clear MCU Int source register. */
		RTMP_IO_WRITE32(pAd, 0x7024, 0);

#ifdef DFS_SUPPORT
		if (pAd->CommonCfg.bIEEE80211H)
		{
			/* pulse radar signal Int. */
			if (McuIntSrc & 0x40)
			{
				NetJobAdd(pulse_radar_detect_proc);
			}

			/* width radar signal Int. */
			if(((pAd->CommonCfg.RadarDetect.RDDurRegion == FCC)
					|| (pAd->CommonCfg.RadarDetect.RDDurRegion == JAP_W56)
					|| (JapRadarType(pAd) == JAP_W56))
				&& (McuIntSrc & 0x80))
			{
				NetJobAdd(width_radar_detect_proc);
			}
		}
#endif /* DFS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
		if ((pAd->CommonCfg.CarrierDetect.Enable)
			&& (McuIntSrc & 0x04))
		{
			tasklet_hi_schedule(carrier_sense_proc);
		}
#endif /* CARRIER_DETECTION_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */


	/* Do nothing if Reset in progress */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS) ||
		RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))
		return IRQ_HANDLED;
	
	
	return (IRQ_HANDLED);
}

void tx_ba_enable(
	IN	PRTMP_ADAPTER pAd)
{
	UINT Data = 0xff80;

	UCHAR pAckStr[4][20] = {"NORMAL", "NO-ACK", "NO-EXPLICIT-ACK", "BLOCK-ACK"};
	EDCA_PARM DefaultEdcaParm;

	/* byte0 b1-0 means ACK POLICY - 0: normal ACK, 1: no ACK, 2:no explicit ACK, 3:BA */
	pAd->CommonCfg.AckPolicy[0] = ((UCHAR)Data & 0x02) << 5;
	pAd->CommonCfg.AckPolicy[1] = ((UCHAR)Data & 0x02) << 5;
	pAd->CommonCfg.AckPolicy[2] = ((UCHAR)Data & 0x02) << 5;
	pAd->CommonCfg.AckPolicy[3] = ((UCHAR)Data & 0x02) << 5;
	DBGPRINT(RT_DEBUG_TRACE, ("ACK policy = %s\n", pAckStr[(UCHAR)Data & 0x02]));

	/* any non-ZERO value in byte1 turn on EDCA & QOS format */
	if (Data & 0x0000ff00) 
	{
		NdisZeroMemory(&DefaultEdcaParm, sizeof(EDCA_PARM));
		DefaultEdcaParm.bValid = TRUE;
		DefaultEdcaParm.Aifsn[0] = 3;
		DefaultEdcaParm.Aifsn[1] = 7;
		DefaultEdcaParm.Aifsn[2] = 2;
		DefaultEdcaParm.Aifsn[3] = 2;

		DefaultEdcaParm.Cwmin[0] = 4;
		DefaultEdcaParm.Cwmin[1] = 4;
		DefaultEdcaParm.Cwmin[2] = 3;
		DefaultEdcaParm.Cwmin[3] = 2;

		DefaultEdcaParm.Cwmax[0] = 10;
		DefaultEdcaParm.Cwmax[1] = 10;
		DefaultEdcaParm.Cwmax[2] = 4;
		DefaultEdcaParm.Cwmax[3] = 3;

		DefaultEdcaParm.Txop[0]  = (USHORT)(Data&0xff);
		DefaultEdcaParm.Txop[1]  = 0;
		DefaultEdcaParm.Txop[2]  = 96;
		DefaultEdcaParm.Txop[3]  = 48;
		AsicSetEdcaParm(pAd, &DefaultEdcaParm);
	}
	else
		AsicSetEdcaParm(pAd, NULL);
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
err_t rt2860_send_packets(
	IN struct netif 	*net_dev, 
	IN PNDIS_PACKET 	skb_p)
{
	RTMP_SET_PACKET_WDEV(skb_p, MAIN_MBSSID);
	RTPKT_TO_OSPKT(skb_p)->dev = net_dev;

	/* transmit the packet */
	return rt2860_packet_xmit(skb_p);
}

