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
    rt_pci_rbus.c

    Abstract:
    Create and register network interface.

    Revision History:
    Who         When            What
    --------    ----------      ------------------------------------------
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


VOID Invalid_Remaining_Packet(
	IN	PRTMP_ADAPTER pAd,
	IN	 ULONG VirtualAddress)
{
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
	
	RTMP_OS_TASK_INIT(&pAd->timerTask, "RtmpTimerTask", pAd);
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

	DBGPRINT(RT_DEBUG_ERROR, ("===>RtmpNetTaskInit\n")); 

	RTMP_NET_TASK_INIT(&pObj->rx_done_task, rx_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->mgmt_dma_done_task, mgmt_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac0_dma_done_task, ac0_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac1_dma_done_task, ac1_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac2_dma_done_task, ac2_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->ac3_dma_done_task, ac3_dma_done_tasklet, (unsigned long)pAd);
		
	RTMP_NET_TASK_INIT(&pObj->hcca_dma_done_task, hcca_dma_done_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->tbtt_task, tbtt_tasklet, (unsigned long)pAd);
	RTMP_NET_TASK_INIT(&pObj->fifo_statistic_full_task, fifo_statistic_full_tasklet, (unsigned long)pAd);

	strcpy(&pObj->rx_done_task.taskName[0], "rx");
	strcpy(&pObj->mgmt_dma_done_task.taskName[0], "mgmt");
	strcpy(&pObj->ac0_dma_done_task.taskName[0], "ac0");
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
		RTMP_NET_TASK_INIT(&pObj->pulse_radar_detect_task, pulse_radar_detect_tasklet, (unsigned long)pAd);
		RTMP_NET_TASK_INIT(&pObj->width_radar_detect_task, width_radar_detect_tasklet, (unsigned long)pAd);
#endif /* DFS_SUPPORT */
#ifdef CARRIER_DETECTION_SUPPORT
		RTMP_NET_TASK_INIT(&pObj->carrier_sense_task, carrier_sense_tasklet, (unsigned long)pAd);
#endif /* CARRIER_DETECTION_SUPPORT */

#ifdef NEW_DFS
		RTMP_NET_TASK_INIT(&pObj->dfs_task, dfs_tasklet, (unsigned long)pAd);
#endif /* NEW_DFS */
	}
#endif /* CONFIG_AP_SUPPORT */

	DBGPRINT(RT_DEBUG_ERROR, ("<===RtmpNetTaskInit\n")); 

	return NDIS_STATUS_SUCCESS;
}


void RtmpNetTaskExit(IN RTMP_ADAPTER *pAd)
{
	/* TODO: Shiang, need to fix that */
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

	/* if you use RTMP_SEM_LOCK, sometimes kernel will hang up, no any */
	/* bug report output */
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
	/*
	 * double check to avoid rotting packet 
	 */
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

/*	printf("ac3_dma_done_process\n"); */
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

	/* printf("ac1_dma_done_process\n"); */
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
#endif /* NEW_DFS */
#endif /* CONFIG_AP_SUPPORT */


/***************************************************************************
  *
  *	interrupt handler related procedures.
  *
  **************************************************************************/
VOID rt2860_interrupt(void *dev_instance)
{
	PNET_DEV                pNetDev;
	PRTMP_ADAPTER           pAd;
	POS_COOKIE              pObj;
	UINT32 IntSource;

	/*
	 * Note 03312008: we can not return here before
	 * RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource.word);
	 * RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource.word);
	 * Or kernel will panic after ifconfig ra0 down sometimes
	 */

	pNetDev = (PNET_DEV) dev_instance;
	pAd = (PRTMP_ADAPTER)pNetDev->driver_private;
	pObj = (POS_COOKIE) pAd->OS_Cookie;

	/* Inital the Interrupt source. */
	IntSource = 0x00000000L;

	RTMP_IO_READ32(pAd, INT_SOURCE_CSR, &IntSource);
	RTMP_IO_WRITE32(pAd, INT_SOURCE_CSR, IntSource); /* write 1 to clear */

	/* Do nothing if NIC doesn't exist */
	if (IntSource == 0xffffffff)
	{
		RTMP_SET_FLAG(pAd, (fRTMP_ADAPTER_NIC_NOT_EXIST | fRTMP_ADAPTER_HALT_IN_PROGRESS));
        return;
	}

	/* Do nothing if Reset in progress */
	if (RTMP_TEST_FLAG(pAd, (fRTMP_ADAPTER_RESET_IN_PROGRESS |fRTMP_ADAPTER_HALT_IN_PROGRESS)))
        return;

	if (IntSource & RTMP_TxCoherent)
	{
		rt2860_int_disable(pAd, RTMP_TxCoherent);
		DBGPRINT(RT_DEBUG_ERROR, (">>>TxCoherent<<<\n"));
		RTMPHandleRxCoherentInterrupt(pAd);
	}

	if (IntSource & RTMP_RxCoherent)
	{
		rt2860_int_disable(pAd, RTMP_RxCoherent);
		DBGPRINT(RT_DEBUG_ERROR, (">>>RxCoherent<<<\n"));
		RTMPHandleRxCoherentInterrupt(pAd);
	}

	if (IntSource & RTMP_TxRxCoherent)
	{
		rt2860_int_disable(pAd, RTMP_TxRxCoherent);
		DBGPRINT(RT_DEBUG_ERROR, (">>>TxRxCoherent<<<\n"));
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
		pAd->int_pending |= RTMP_INT_AC1_DLY;						
	}

	if (IntSource & RTMP_INT_AC0_DLY)
	{
		pAd->int_pending |= RTMP_INT_AC0_DLY;

		if ((pAd->int_disable_mask & RTMP_INT_AC0_DLY) == 0) 
		{
			rt2860_int_disable(pAd, RTMP_INT_AC0_DLY);
			tasklet_hi_schedule(&pObj->ac0_dma_done_task);
		}
		pAd->int_pending |= RTMP_INT_AC0_DLY;						
	}

	if (IntSource & RTMP_PreTBTTInt)
        RTMPHandlePreTBTTInterrupt(pAd);

	if (IntSource & RTMP_TBTTInt)
        RTMPHandleTBTTInterrupt(pAd);
    
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd)
	{
		if (IntSource & RTMP_McuCommand)
			RTMPHandleMcuInterrupt(pAd);
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

