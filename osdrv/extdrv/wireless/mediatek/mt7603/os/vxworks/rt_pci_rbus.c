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
    rt_pci_rbus.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#include <rt_config.h>


static void rx_done_tasklet(unsigned long data);
static void mgmt_dma_done_tasklet(unsigned long data);
static void ac0_dma_done_tasklet(unsigned long data);
static void ac1_dma_done_tasklet(unsigned long data);
static void ac2_dma_done_tasklet(unsigned long data);
static void ac3_dma_done_tasklet(unsigned long data);
static void hcca_dma_done_tasklet(unsigned long data);
static void fifo_statistic_full_tasklet(unsigned long data);

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
static void uapsd_eosp_sent_tasklet(unsigned long data);
#endif /* UAPSD_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */


/***************************************************************************
  *
  *	Interface-depended memory allocation/Free related procedures.
  *		Mainly for Hardware TxDesc/RxDesc/MgmtDesc, DMA Memory for TxData/RxData, etc.,
  *
  **************************************************************************/
/* Function for TxRx/Mgmt Desc Memory allocation. */
void RtmpAllocDescBuf(
	IN RTMP_ADAPTER *pAd,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT VOID **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	/* TODO: Need to check if there are any exists VxWorks function take care the pci_alloc_consistent() related jobs. */
	/*	check cacheDmaMalloc() */
#ifdef VXWORKS_5X
	*VirtualAddress = (PVOID) cacheDmaMalloc(Length);
#endif /* VXWORKS_5X */
#ifdef VXWORKS_6X
	*VirtualAddress = (PVOID) kmalloc(Length, 0);
	if (!Cached)
		*VirtualAddress = (PVOID) KSEG1((UINT32)(*VirtualAddress));
#endif /* VXWORKS_6X */

	if (pAd->infType == RTMP_DEV_INF_PCI) /* RT2880 PCI */
		*PhysicalAddress = (ra_dma_addr_t)PCI_VIRT_TO_PHYS(*VirtualAddress);
	else
		*PhysicalAddress = (ra_dma_addr_t)*VirtualAddress;

}


/* Function for free allocated Desc Memory. */
void RtmpFreeDescBuf(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	PVOID	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
#ifdef VXWORKS_5X
	cacheDmaFree(VirtualAddress);
#endif /* VXWORKS_5X */
#ifdef VXWORKS_6X
	kfree(VirtualAddress);
#endif /* VXWORKS_6X */
}


/* Function for TxData DMA Memory allocation. */
void RTMP_AllocateFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	/*POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie; */

#ifdef VXWORKS_5X
	*VirtualAddress = (PVOID) cacheDmaMalloc(Length);
#endif /* VXWORKS_5X */
#ifdef VXWORKS_6X
	*VirtualAddress = (PVOID) kmalloc(Length, 0);
	if (!Cached)
		*VirtualAddress = (PVOID) KSEG1((UINT32)(*VirtualAddress));
#endif /* VXWORKS_6X */

	if (pAd->infType == RTMP_DEV_INF_PCI) /* RT2880 PCI */
		*PhysicalAddress = (ra_dma_addr_t)PCI_VIRT_TO_PHYS(*VirtualAddress);
	else
		*PhysicalAddress = (ra_dma_addr_t)*VirtualAddress;
}


void RTMP_FreeFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	IN	PVOID	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	/*POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie; */
	
	/* TODO: Need to check if there are any exists VxWorks function who will take care about pci_free_consistent() related jobs. */
#ifdef VXWORKS_5X
	cacheDmaFree(VirtualAddress);
#endif /* VXWORKS_5X */
#ifdef VXWORKS_6X
	kfree(VirtualAddress);
#endif /* VXWORKS_6X */
}

#if 0
/*
 * FUNCTION: Allocate a common buffer for DMA
 * ARGUMENTS:
 *     AdapterHandle:  AdapterHandle
 *     Length:  Number of bytes to allocate
 *     Cached:  Whether or not the memory can be cached
 *     VirtualAddress:  Pointer to memory is returned here
 *     PhysicalAddress:  Physical address corresponding to virtual address
 */
void RTMP_AllocateSharedMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	/*POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie; */

	*VirtualAddress = (PVOID) memalign(sizeof(long), sizeof(char)*Length);
	if (*VirtualAddress == NULL)
		return;
	*PhysicalAddress = (ra_dma_addr_t)(PCI_VIRT_TO_PHYS(*VirtualAddress));
}
#endif /* Unused */

/*
 * FUNCTION: Allocate a packet buffer for DMA
 * ARGUMENTS:
 *     AdapterHandle:  AdapterHandle
 *     Length:  Number of bytes to allocate
 *     Cached:  Whether or not the memory can be cached
 *     VirtualAddress:  Pointer to memory is returned here
 *     PhysicalAddress:  Physical address corresponding to virtual address
 * Notes:
 *     Cached is ignored: always cached memory
 */
PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	M_BLK_ID	pMblk;
	POS_COOKIE	pObj;


	pObj = (POS_COOKIE)pAd->OS_Cookie;
	pMblk = RtmpVxNetTupleGet(pObj->pNetPool[RTMP_NETPOOL_RX], Length, M_DONTWAIT, MT_DATA, 1);
	if (pMblk == NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate rx %ld size packet\n",Length));
		*VirtualAddress = (PVOID) NULL;
		*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) NULL;
	} 
	else
	{
/*		pMblk->mBlkPktHdr.len = pMblk->mBlkHdr.mLen = Length; */
		if (pMblk->m_data - pMblk->pClBlk->clNode.pClBuf != 64)
		{
			hex_dump_mBlk(__FUNCTION__, pMblk, FALSE);
			printf("%s(pClBuf=0x%x, mData=0x%x): headroom(%d) is not 64 bytes!\n", 
					__FUNCTION__, pMblk->pClBlk->clNode.pClBuf, pMblk->m_data, 
					pMblk->m_data - pMblk->pClBlk->clNode.pClBuf);
		}
		*VirtualAddress = (PVOID) pMblk->m_data;
		*PhysicalAddress = (ra_dma_addr_t)pMblk->m_data;
		PCI_MAP_SINGLE(pAd, 
						 pMblk->pClBlk->clNode.pClBuf /**VirtualAddress*/ , 
						 pMblk->pClBlk->clSize /*(Length + NETBUF_LEADING_CLSPACE_DRV)*/, 
						 0, 
						 PCI_DMA_FROMDEVICE);
	}

	if (pAd->infType == RTMP_DEV_INF_PCI) /* RT2880 PCI */
		*PhysicalAddress = (ra_dma_addr_t)PCI_VIRT_TO_PHYS(*PhysicalAddress);

	return (PNDIS_PACKET) pMblk;
}


VOID Invalid_Remaining_Packet(
	IN	PRTMP_ADAPTER pAd,
	IN	 ULONG VirtualAddress)
{
	NDIS_PHYSICAL_ADDRESS PhysicalAddress;

	PhysicalAddress = PCI_MAP_SINGLE(pAd, (void *)(VirtualAddress+1600), RX_BUFFER_NORMSIZE-1600, 0, PCI_DMA_FROMDEVICE);
}


int rt28xx_irq_request(IN PNET_DEV pNetDev)
{
	PRTMP_ADAPTER pAd;
	int retval = 0;

	pAd = (PRTMP_ADAPTER)(RTMP_OS_NETDEV_GET_PRIV(pNetDev));
	ASSERT(pAd);

	SYS_INT_CONNECT(INUM_TO_IVEC(100), rt2860_interrupt, pAd, retval);

	return retval; 
	
}


int rt28xx_irq_release(IN PNET_DEV pNetDev)
{
	PRTMP_ADAPTER pAd;
	STATUS	status;
	
	pAd = (PRTMP_ADAPTER)(RTMP_OS_NETDEV_GET_PRIV(pNetDev));
	ASSERT(pAd);

	/* TODO: Make sure we already disable the source of interrupts  before calling this routine. */
	SYS_INT_DISCONNECT(INUM_TO_IVEC(100), rt2860_interrupt, pAd, status);
	
	DBGPRINT(RT_DEBUG_TRACE, ("disConnect() the IRQ with status=%d!\n", status));
	
	return status;
}


NDIS_STATUS RtmpMgmtTaskInit(IN RTMP_ADAPTER *pAd)
{
	RTMP_OS_TASK *pTask;
	NDIS_STATUS status;
	

	/*
		Creat TimerQ Thread
	*/
	/* We need init timerQ related structure before create the timer thread. */
	RtmpTimerQInit(pAd);

	pTask = &pAd->timerTask;
	
	RTMP_OS_TASK_INIT(&pAd->timerTask, "RtmpTimerTask", pAd, &pAd->RscTaskMemList);
	status = RtmpOSTaskAttach(pTask, RtmpTimerQThread, (ULONG)(&pAd->timerTask));
	if (status == NDIS_STATUS_FAILURE) 
	{
		printf("%s: unable to start RtmpTimerQThread\n",RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev));
	}

#ifdef WSC_INCLUDED
	/* start the crediential write task first. */
	WscThreadInit(pAd);
#endif /* WSC_INCLUDED */

	return status;
	
}


/*
========================================================================
Routine Description:
    Close kernel threads.

Arguments:
	*pAd				the raxx interface data pointer

Return Value:
    NONE

Note:
========================================================================
*/
VOID RtmpMgmtTaskExit(
	IN RTMP_ADAPTER *pAd)
{
	RTMP_OS_TASK	*pTask;
	NDIS_STATUS		ret;
	
	
	/* We need clear timerQ related structure before exits of the timer thread. */
	RtmpTimerQExit(pAd);

#ifdef WSC_INCLUDED
	WscThreadExit(pAd);
#endif /* WSC_INCLUDED */

	/* Terminate Threads */
	pTask = &pAd->timerTask;
	ret = RtmpOSTaskKill(pTask);
	if (ret == NDIS_STATUS_FAILURE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: kill task(%s) failed!\n", 
					RTMP_OS_NETDEV_GET_DEVNAME(pAd->net_dev), pTask->taskName));
	}

}


NDIS_STATUS RtmpNetTaskInit(IN RTMP_ADAPTER *pAd)
{
	POS_COOKIE pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	logMsg("%s()--->\n", __FUNCTION__, 0,0,0,0,0);

	RTMP_NET_TASK_INIT(&pObj->rx_done_task, rx_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->mgmt_dma_done_task, mgmt_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac0_dma_done_task, ac0_dma_done_tasklet, (unsigned long)pAd);
#ifdef CONFIG_ATE
	RTMP_NET_TASK_INIT(&pObj->ate_ac0_dma_done_task, ate_ac0_dma_done_tasklet, (unsigned long)pAd);
#endif /* CONFIG_ATE */
	RTMP_NET_TASK_INIT(&pObj->ac1_dma_done_task, ac1_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac2_dma_done_task, ac2_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac3_dma_done_task, ac3_dma_done_tasklet, (unsigned long)pAd);
		
	RTMP_NET_TASK_INIT(&pObj->hcca_dma_done_task, hcca_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->tbtt_task, tbtt_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->fifo_statistic_full_task, fifo_statistic_full_tasklet, (unsigned long)pAd);

	strcpy(&pObj->rx_done_task.taskName[0], "rx");
	strcpy(&pObj->mgmt_dma_done_task.taskName[0], "mgmt");
	strcpy(&pObj->ac0_dma_done_task.taskName[0], "ac0");
#ifdef CONFIG_ATE
	strcpy(&pObj->ate_ac0_dma_done_task.taskName[0], "ate_ac0");
#endif /* CONFIG_ATE */
	strcpy(&pObj->ac1_dma_done_task.taskName[0], "ac1");
	strcpy(&pObj->ac2_dma_done_task.taskName[0], "ac2");
	strcpy(&pObj->ac3_dma_done_task.taskName[0], "ac3");
	strcpy(&pObj->hcca_dma_done_task.taskName[0], "hcca");
	strcpy(&pObj->tbtt_task.taskName[0], "tbtt");
	strcpy(&pObj->fifo_statistic_full_task.taskName[0], "fifo");
	
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef UAPSD_SUPPORT	
		RTMP_NET_TASK_INIT(&pObj->uapsd_eosp_sent_task, uapsd_eosp_sent_tasklet, (unsigned long)pAd);
		strcpy(&pObj->uapsd_eosp_sent_task.taskName[0], "uapsd");
#endif /* UAPSD_SUPPORT */

#ifdef DFS_SUPPORT
#ifdef DFS_SOFTWARE_SUPPORT
		RTMP_NET_TASK_INIT(&pObj->pulse_radar_detect_task, pulse_radar_detect_tasklet, (unsigned long)pAd);
		RTMP_NET_TASK_INIT(&pObj->width_radar_detect_task, width_radar_detect_tasklet, (unsigned long)pAd);
#endif /* DFS_SOFTWARE_SUPPORT */
#endif /* DFS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
		RTMP_NET_TASK_INIT(&pObj->carrier_sense_task, carrier_sense_tasklet, (unsigned long)pAd);
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef DFS_HARDWARE_SUPPORT
		RTMP_NET_TASK_INIT(&pObj->dfs_task, dfs_tasklet, (unsigned long)pAd);
#endif /* DFS_HARDWARE_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */

	logMsg("<---%s()\n", __FUNCTION__, 0,0,0,0,0);

	return NDIS_STATUS_SUCCESS;
}


void RtmpNetTaskExit(IN RTMP_ADAPTER *pAd)
{
	/* TODO: Shiang, need to fix that */
#if 0
	POS_COOKIE pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	tasklet_kill(&pObj->rx_done_task);
	tasklet_kill(&pObj->mgmt_dma_done_task);
	tasklet_kill(&pObj->ac0_dma_done_task);
	tasklet_kill(&pObj->ac1_dma_done_task);
	tasklet_kill(&pObj->ac2_dma_done_task);
	tasklet_kill(&pObj->ac3_dma_done_task);
	tasklet_kill(&pObj->hcca_dma_done_task);
	tasklet_kill(&pObj->tbtt_task);
	tasklet_kill(&pObj->fifo_statistic_full_task);
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
#ifdef CARRIER_DETECTION_SUPPORT
		tasklet_kill(&pObj->carrier_sense_task);
#endif /* CARRIER_DETECTION_SUPPORT */
#ifdef DFS_SUPPORT
#ifdef DFS_SOFTWARE_SUPPORT
		tasklet_kill(&pObj->width_radar_detect_task);
		tasklet_kill(&pObj->pulse_radar_detect_task);
#endif /* DFS_SOFTWARE_SUPPORT */
#endif /* DFS_SUPPORT */

#ifdef DFS_HARDWARE_SUPPORT
		tasklet_kill(&pObj->dfs_task);
#endif /* DFS_HARDWARE_SUPPORT */
	}
#endif /* CONFIG_AP_SUPPORT */
#endif
}


static inline void rt2860_int_enable(PRTMP_ADAPTER pAd, unsigned int mode)
{
	UINT32 regValue;

	pAd->int_disable_mask &= ~(mode);
	regValue = pAd->int_enable_reg & ~(pAd->int_disable_mask);		
	RTMP_IO_WRITE32(pAd, INT_MASK_CSR, regValue);     /* 1:enable */
	if (regValue != 0)
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);
}


static inline void rt2860_int_disable(PRTMP_ADAPTER pAd, unsigned int mode)
{
	UINT32 regValue;

	pAd->int_disable_mask |= mode;
	regValue = pAd->int_enable_reg & ~(pAd->int_disable_mask);
	RTMP_IO_WRITE32(pAd, INT_MASK_CSR, regValue);     /* 0: disable */
	if (regValue == 0)
	{
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_ACTIVE);		
	}
}


/***************************************************************************
  *
  *	tasklet related procedures.
  *
  **************************************************************************/
static void mgmt_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;

	pAd->int_pending &= ~RTMP_INT_MGMT_DLY;
	RTMPHandleMgmtRingDmaDoneInterrupt(pAd);

#ifdef RELEASE_EXCLUDE
	/*
		if you use RTMP_SEM_LOCK, sometimes kernel will hang up, no any
		bug report output
	*/
#endif /* RELEASE_EXCLUDE */
	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_MGMT_DLY) 
	{
		tasklet_hi_schedule(&pObj->mgmt_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_MGMT_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


static void rx_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	BOOLEAN	bReschedule = 0;
	POS_COOKIE pObj;
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
    pObj = (POS_COOKIE) pAd->OS_Cookie;
	
	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	pAd->int_pending &= ~(RTMP_INT_RX);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
	
	bReschedule = rtmp_rx_done_handle(pAd);

	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/*
	 * double check to avoid rotting packet 
	 */
	if (pAd->int_pending & RTMP_INT_RX || bReschedule) 
	{
		tasklet_hi_schedule(&pObj->rx_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_RX);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);

}


void fifo_statistic_full_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
    pObj = (POS_COOKIE) pAd->OS_Cookie;

	pAd->int_pending &= ~(RTMP_FifoStaFullInt); 
	NICUpdateFifoStaCounters(pAd);
	
	RTMP_INT_LOCK(&pAd->irq_lock, flags);  
	/* double check to avoid rotting packet */
	if (pAd->int_pending & RTMP_FifoStaFullInt) 
	{
		tasklet_hi_schedule(&pObj->fifo_statistic_full_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_FifoStaFullInt);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);

}


static void hcca_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pAd->int_pending &= ~RTMP_INT_HCCA_DLY;

	RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_HCCA_DONE);

	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if (pAd->int_pending & RTMP_INT_HCCA_DLY)
	{
		tasklet_hi_schedule(&pObj->hcca_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_HCCA_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


static void ac3_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;

	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pAd->int_pending &= ~RTMP_INT_AC3_DLY;
	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC3_DONE);

	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & RTMP_INT_AC3_DLY) || bReschedule)
	{
		tasklet_hi_schedule(&pObj->ac3_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC3_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


static void ac2_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;
	
	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pAd->int_pending &= ~RTMP_INT_AC2_DLY;
	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC2_DONE);

	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & RTMP_INT_AC2_DLY) || bReschedule) 
	{
		tasklet_hi_schedule(&pObj->ac2_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC2_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


static void ac1_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;

	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pAd->int_pending &= ~RTMP_INT_AC1_DLY;
	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC1_DONE);

	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & RTMP_INT_AC1_DLY) || bReschedule) 
	{
		tasklet_hi_schedule(&pObj->ac1_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC1_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


static void ac0_dma_done_tasklet(unsigned long data)
{
	unsigned long flags;
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	POS_COOKIE pObj;
	BOOLEAN bReschedule = 0;

	/* Do nothing if the driver is starting halt state. */
	/* This might happen when timer already been fired before cancel timer with mlmehalt */
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS | fRTMP_ADAPTER_NIC_NOT_EXIST))
		return;
	
	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pAd->int_pending &= ~RTMP_INT_AC0_DLY;
	bReschedule = RTMPHandleTxRingDmaDoneInterrupt(pAd, TX_AC0_DONE);
	
	RTMP_INT_LOCK(&pAd->irq_lock, flags);
	/* double check to avoid lose of interrupts */
	if ((pAd->int_pending & RTMP_INT_AC0_DLY) || bReschedule)
	{
		tasklet_hi_schedule(&pObj->ac0_dma_done_task);
		RTMP_INT_UNLOCK(&pAd->irq_lock, flags);
		return;
	}

	rt2860_int_enable(pAd, RTMP_INT_AC0_DLY);
	RTMP_INT_UNLOCK(&pAd->irq_lock, flags);    
}


#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_SUPPORT
/*
========================================================================
Routine Description:
    Used to send the EOSP frame.

Arguments:
    data			Pointer to our adapter

Return Value:
    None

Note:
========================================================================
*/
static void uapsd_eosp_sent_tasklet(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;

	RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, WCID_ALL, MAX_TX_PROCESS);
}
#endif /* UAPSD_SUPPORT */

#ifdef DFS_HARDWARE_SUPPORT

void schedule_dfs_task(PRTMP_ADAPTER pAd)
{
	POS_COOKIE pObj;
	
    pObj = (POS_COOKIE) pAd->OS_Cookie;
	tasklet_hi_schedule(&pObj->dfs_task);
}

static void dfs_tasklet(unsigned long data)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER) data;
	
	if (pAd->CommonCfg.DFSAPRestart == 1)
	{
		int i, j;

		pAd->CommonCfg.dfs_w_counter += 10;
		/* reset period table */
		for (i = 0; i < NEW_DFS_MAX_CHANNEL; i++)
		{
			for (j = 0; j < NEW_DFS_MPERIOD_ENT_NUM; j++)
			{
				pAd->CommonCfg.DFS_T[i][j].period = 0;
				pAd->CommonCfg.DFS_T[i][j].idx = 0;
				pAd->CommonCfg.DFS_T[i][j].idx2 = 0;
			}
		}

		APStop(pAd);
		APStartUp(pAd);
		pAd->CommonCfg.DFSAPRestart = 0;
	}
	else
	/* check radar here */
	{
		int idx;
		
		if (pAd->CommonCfg.radarDeclared == 0)
		{
			for (idx = 0; idx < 3; idx++)
			{
				if (SWRadarCheck(pAd, idx) == 1)
				{
					pAd->CommonCfg.radarDeclared = 1;
					break;
				}
			}
		}
	}
	
	
}
#endif /* DFS_HARDWARE_SUPPORT */

#endif /* CONFIG_AP_SUPPORT */


/***************************************************************************
  *
  *	interrupt handler related procedures.
  *
  **************************************************************************/
int print_int_count;

void rt2860_interrupt(void *dev_instance)
{
	PRTMP_ADAPTER pAd;
	UINT32 IntSource;
	POS_COOKIE pObj;
	
	pAd = (PRTMP_ADAPTER)dev_instance;
	pObj = (POS_COOKIE) pAd->OS_Cookie;


	/* Note 03312008: we can not return here before
		RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource.word);
		RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource.word);
		Or kernel will panic after ifconfig ra0 down sometimes */


	/* */
	/* Inital the Interrupt source. */
	/* */
	IntSource = 0x00000000L;
/*	McuIntSource.word = 0x00000000L; */

	/* */
	/* Get the interrupt sources & saved to local variable */
	/* */
	/*RTMP_IO_READ32(pAd, where, &McuIntSource.word); */
	/*RTMP_IO_WRITE32(pAd, , McuIntSource.word); */

	/* */
	/* Flag fOP_STATUS_DOZE On, means ASIC put to sleep, elase means ASICK WakeUp */
	/* And at the same time, clock maybe turned off that say there is no DMA service. */
	/* when ASIC get to sleep. */
	/* To prevent system hang on power saving. */
	/* We need to check it before handle the INT_SOURCE_CSR, ASIC must be wake up. */
	/* */
	/* RT2661 => when ASIC is sleeping, MAC register cannot be read and written. */
	/* RT2860 => when ASIC is sleeping, MAC register can be read and written. */
/*	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE)) */
	{
		RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource);
		RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource); /* write 1 to clear */
	}
/*	else */
/*		DBGPRINT(RT_DEBUG_TRACE, (">>>fOP_STATUS_DOZE<<<\n")); */

	/*RTMP_ISR_LOG (RT_DEBUG_TRACE, "%s():0x%x\n", __FUNCTION__, IntSource.word, 0, 0, 0, 0); */
	
/*	RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IsrAfterClear); */
/*	RTMP_IO_READ32(pAd, MCU_INT_SOURCE_CSR, &McuIsrAfterClear); */
/*	DBGPRINT(RT_DEBUG_INFO, ("====> RTMPHandleInterrupt(ISR=%08x,Mcu ISR=%08x, After clear ISR=%08x, MCU ISR=%08x)\n", */
/*			IntSource.word, McuIntSource.word, IsrAfterClear, McuIsrAfterClear)); */

	/* Do nothing if Reset in progress */
	if (RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS |fRTMP_ADAPTER_HALT_IN_PROGRESS)))
		return;

	/* */
	/* Handle interrupt, walk through all bits */
	/* Should start from highest priority interrupt */
	/* The priority can be adjust by altering processing if statement */
	/* */
#if 0
{
	UINT32 regValue;

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &regValue);
	printf("MAC_SYS_CTRL = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x400, &regValue);
	printf("MCU 0x400 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x404, &regValue);
	printf("MCU 0x404 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x408, &regValue);
	printf("MCU 0x408 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x40C, &regValue);
	printf("MCU 0x40C = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x410, &regValue);
	printf("MCU 0x410 = %08x\n", regValue);

	RTMP_IO_READ32(pAd, 0x414, &regValue);
	printf("MCU_INT_STA = %08x\n", regValue);
}
#endif 

#ifdef DBG
	if ((RTDebugLevel == RT_DEBUG_LOUD) && (((++print_int_count) % 100) == 0))
	{
		UINT32 reg = 0;
		int Count, free;

		RTMP_IO_READ32(pAd, INT_MASK_CSR, &reg);     /* 1:enable */
		logMsg("%d: INT_MASK_CSR = %08x, IntSource %08x\n", print_int_count, reg, IntSource,0,0,0);
		RTMP_IO_READ32(pAd, TX_CTX_IDX0 + 0 * 0x10 , &reg);
		logMsg("TX_CTX_IDX0 = %08x\n", reg,0,0,0,0,0);
		RTMP_IO_READ32(pAd, TX_DTX_IDX0 + 0 * 0x10 , &reg);
		logMsg("TX_DTX_IDX0 = %08x\n", reg,0,0,0,0,0);
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &reg);
		logMsg("WPDMA_GLO_CFG = %08x\n", reg,0,0,0,0,0);
		for (Count = 0; Count < 1; Count++)
		{
			if (pAd->TxRing[Count].TxSwFreeIdx> pAd->TxRing[Count].TxCpuIdx)
				free = pAd->TxRing[Count].TxSwFreeIdx - pAd->TxRing[Count].TxCpuIdx -1;
			else
				free = pAd->TxRing[Count].TxSwFreeIdx + TX_RING_SIZE - pAd->TxRing[Count].TxCpuIdx -1;
		
			logMsg("%d: Free = %d TxSwFreeIdx = %d\n", Count, free, pAd->TxRing[Count].TxSwFreeIdx,0,0,0); 
		}
		logMsg("pAd->int_disable_mask = %08x\n", pAd->int_disable_mask,0,0,0,0,0);
		logMsg("pAd->int_enable_reg = %08x\n", pAd->int_enable_reg,0,0,0,0,0);
		logMsg("pAd->int_pending = %08x\n", pAd->int_pending,0,0,0,0,0);
		RTMP_IO_READ32(pAd, RX_DRX_IDX , &reg);
		logMsg("pAd->RxRing[0].RxSwReadIdx = %08x, RX_DRX_IDX = %08x\n", pAd->RxRing[0].RxSwReadIdx, reg,0,0,0,0);
	}
#endif
		

	pAd->bPCIclkOff = FALSE;

	/* If required spinlock, each interrupt service routine has to acquire */
	/* and release itself. */
	/* */
	
	/* Do nothing if NIC doesn't exist */
	if (IntSource == 0xffffffff)
	{
		RTMP_SET_FLAG(pAd, (fRTMP_ADAPTER_NIC_NOT_EXIST | fRTMP_ADAPTER_HALT_IN_PROGRESS));
        	return;
	}
	
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

	if (IntSource & RTMP_FifoStaFullInt) 
	{
		if ((pAd->int_disable_mask & RTMP_FifoStaFullInt) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_FifoStaFullInt);
			tasklet_hi_schedule(&pObj->fifo_statistic_full_task);
		}
		pAd->int_pending |= RTMP_FifoStaFullInt; 
	}

	if (IntSource & RTMP_INT_MGMT_DLY) 
	{
		if ((pAd->int_disable_mask & RTMP_INT_MGMT_DLY) ==0 )
		{
			rt2860_int_disable(pAd, RTMP_INT_MGMT_DLY);
			tasklet_hi_schedule(&pObj->mgmt_dma_done_task);			
		}
		pAd->int_pending |= RTMP_INT_MGMT_DLY ;
	}

	if (IntSource & RTMP_INT_RX)
	{
		if ((pAd->int_disable_mask & RTMP_INT_RX) == 0)
		{
			rt2860_int_disable(pAd, RTMP_INT_RX);
			tasklet_hi_schedule(&pObj->rx_done_task);
		}
		pAd->int_pending |= RTMP_INT_RX;
	}

	if (IntSource & RTMP_INT_HCCA_DLY)
	{
		if ((pAd->int_disable_mask & RTMP_INT_HCCA_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_HCCA_DLY);
			tasklet_hi_schedule(&pObj->hcca_dma_done_task);
		}
		pAd->int_pending |= RTMP_INT_HCCA_DLY;						
	}

	if (IntSource & RTMP_INT_AC3_DLY)
	{
		if ((pAd->int_disable_mask & RTMP_INT_AC3_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC3_DLY);
			tasklet_hi_schedule(&pObj->ac3_dma_done_task);
		}
		pAd->int_pending |= RTMP_INT_AC3_DLY;						
	}

	if (IntSource & RTMP_INT_AC2_DLY)
	{
		if ((pAd->int_disable_mask & RTMP_INT_AC2_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC2_DLY);
			tasklet_hi_schedule(&pObj->ac2_dma_done_task);
		}
		pAd->int_pending |= RTMP_INT_AC2_DLY;						
	}

	if (IntSource & RTMP_INT_AC1_DLY)
	{
		pAd->int_pending |= RTMP_INT_AC1_DLY;
		if ((pAd->int_disable_mask & RTMP_INT_AC1_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC1_DLY);
			tasklet_hi_schedule(&pObj->ac1_dma_done_task);
		}
	}

	if (IntSource & RTMP_INT_AC0_DLY)
	{
/*
		if (IntSource.word & 0x2) {
			UINT32 reg;
			RTMP_IO_READ32(pAd, DELAY_INT_CFG, &reg);
			printf("IntSource.word = %08x, DELAY_REG = %08x\n", IntSource.word, reg);
		}
*/
		pAd->int_pending |= RTMP_INT_AC0_DLY;
		if ((pAd->int_disable_mask & RTMP_INT_AC0_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC0_DLY);
			tasklet_hi_schedule(&pObj->ac0_dma_done_task);
		}
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

	if (IntSource & RTMP_PreTBTTInt)
	{
		RTMPHandlePreTBTTInterrupt(pAd);
	}

	if (IntSource & RTMP_TBTTInt)
	{
		RTMPHandleTBTTInterrupt(pAd);
	}

#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (IntSource & RTMP_McuCommand)
		{
			RTMPHandleMcuInterrupt(pAd);
		}
	}
#endif /* CONFIG_AP_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (IntSource & RTMP_AutoWakeupInt)
			RTMPHandleTwakeupInterrupt(pAd);
	}
#endif /* CONFIG_STA_SUPPORT */

}

/*
 * invaild or writeback cache 
 * and convert virtual address to physical address 
 */
ra_dma_addr_t vxworks_pci_map_single(void *handle, void *ptr, size_t size, int sd_idx, int direction)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)handle;
	void *pCachePtr;
	int cacheLen;
	
	if (sd_idx == 1)
	{
		pCachePtr = ((TX_BLK *)ptr)->pSrcBufData;
		cacheLen = ((TX_BLK *)ptr)->SrcBufLen;
	}
	else
	{
		pCachePtr = ptr;
		cacheLen = size;
	}
	
#ifndef CACHE_DMA_SUPPORT	
	switch(direction)
	{
	case PCI_DMA_TODEVICE:
			/* write back only */
		if (cacheFlush(DATA_CACHE, pCachePtr, cacheLen) != 0)
			logMsg("cacheFlush error\n",0,0,0,0,0,0); /* test code */
		break;

	case PCI_DMA_FROMDEVICE:
			/* invalidate only */
			if (cacheInvalidate(DATA_CACHE, pCachePtr, cacheLen) != 0)
				logMsg("cacheInvalidate error\n",0,0,0,0,0,0); /* test code */
		break;

	case PCI_DMA_BIDIRECTIONAL:
			/* write back and invalidate */
		cacheFlush(DATA_CACHE, pCachePtr, cacheLen);
		cacheInvalidate(DATA_CACHE, pCachePtr, cacheLen);
		break;

	default:
			/* buggy ... */
		break;
	}
#endif /* CACHE_DMA_SUPPORT */

	if (pAd->infType == RTMP_DEV_INF_PCI) /* RT2880 PCI */
		return (ra_dma_addr_t)PCI_VIRT_TO_PHYS(pCachePtr);
	else
		return (ra_dma_addr_t)pCachePtr;

}

