/****************************************************************************
 * Ralink Tech Inc.
 * Taiwan, R.O.C.
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
	usb_main_dev.c

	Abstract:

	Revision History:
	Who			When		What
	--------	----------	----------------------------------------------
	Name		Date		Modification logs

*/

#include "rt_config.h"

/* Global Variable */
DC_USB_CLASSD          classd_ralink; /* USB class device driver */

/* External Global Variables */
extern USB_DEVICE_ID rtusb_dev_id[];
extern INT const rtusb_usb_id_len;

/*
	========================================================================
	
	Routine Description:
		Configure ralink ralink wireless USB device

	Arguments:		
		pUSBDev			-	pointer to USB device
  		pIntf			-	pointer to USB interface 
		pAd				-   NIC Adapter pointer
	Return Value:
		
	Note:
		
	========================================================================
*/
static NDIS_STATUS USBDevConfigInit(DC_USB_DEVICE *pUSBDev, DC_USB_INTERFACE *pIntf, RTMP_ADAPTER *pAd)
{
	UINT32 Index;
	UINT32 BulkOutIdx = 0;
	NDIS_STATUS Result = NDIS_STATUS_FAILURE;
	DC_USB_PIPE *pUSBPipe = NULL;
	UINT32 USBPipeIndex = 0;
		
	/* Get ralink USB dongle endpoints # */
	pAd->NumberOfPipes = pUSBDev->intf[0].id->bNumEndpoints; /* Just only one interface */
	DBGPRINT(RT_DEBUG_TRACE, ("NumEndpoints=%d\n", pAd->NumberOfPipes ));
	
	/* Store USB Attribute into pAd */
	for (Index = 0; Index < pAd->NumberOfPipes; Index++)
	{
		DC_USB_ENDPOINT_DESCRIPTOR_t *pUSBEndDescr = pUSBDev->intf[0].ed[Index];
		if (DC_USB20_IS_ENDPOINT_IN(pUSBEndDescr)
            && DC_USB20_IS_ENDPOINT_BULK(pUSBEndDescr)
            && (pUSBEndDescr->bEndpointAddress != 0x87)) 
		{
			/* Get RX Control I/O Information (EP1 IN))*/
			pAd->BulkInEpAddr = pUSBEndDescr->bEndpointAddress;
			pAd->BulkInMaxPacketSize = pUSBEndDescr->wMaxPacketSize;

			DBGPRINT_RAW(RT_DEBUG_TRACE, ("BULK IN MaximumPacketSize = %d.\n", pAd->BulkInMaxPacketSize));
			DBGPRINT_RAW(RT_DEBUG_TRACE, ("EP address = 0x%2x  .\n", pUSBEndDescr->bEndpointAddress));
		}
 		else if (DC_USB20_IS_ENDPOINT_OUT(pUSBEndDescr)
                    && DC_USB20_IS_ENDPOINT_BULK(pUSBEndDescr)
                    && (pUSBEndDescr->bEndpointAddress != 0x07)) 
		{
			/* Get EDCA Tx information for EP1~EP4 OUT, HCCA Tx information for EP5 Out, and reserved for EP6 OUT */
			pAd->BulkOutEpAddr[BulkOutIdx++] = pUSBEndDescr->bEndpointAddress;
			pAd->BulkOutMaxPacketSize = pUSBEndDescr->wMaxPacketSize;
				
			DBGPRINT_RAW(RT_DEBUG_TRACE, ("BULK OUT MaximumPacketSize = %d.\n", pAd->BulkOutMaxPacketSize));
			DBGPRINT_RAW(RT_DEBUG_TRACE, ("EP address = 0x%2x.\n", pUSBEndDescr->bEndpointAddress));
 		}

		if (!DC_USB20_IS_ENDPOINT_CONTROL(pUSBEndDescr))
		{
			/* Open USB Pipes for BULK IN,OUT transfer */
			Result = dc_uh_core_pipe_open(pUSBDev, pUSBEndDescr, (DC_USB_PIPE **)&pUSBPipe);
			dc_log_printf("OpenedpUSBPipe=0x%x", pUSBPipe);
			if (!pUSBPipe)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s: Create new USB pipe specific to this endpoint fail.\n", __FUNCTION__));
				return NDIS_STATUS_FAILURE;
			}

			dump_usbpipe(pUSBPipe);
			
			/* Initialize USB HOST Channel */
			SetUSBHostChannel(USBPipeIndex, pUSBDev, pUSBEndDescr->bEndpointAddress, pUSBPipe);
		
			USBPipeIndex++;

			if (Result != DC_SUCCESS)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s : Open USB Pipes for BULK IN/OUT transfer fail.\n", __FUNCTION__));
				ReleaseUSBHostChannel(USBPipeIndex);
			}	
		}
	} /* end for */

	if (!(pAd->BulkInEpAddr && pAd->BulkOutEpAddr[0]))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Could not find both bulk-in and bulk-out endpoints.\n", __FUNCTION__));
		return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;
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
	return TRUE;
}
#endif /* Unused */

/*
	========================================================================
	
	Routine Description:
		Load ralink wireless USB device

	Arguments:		
  		Intf			-	pointer to USB interface
		pUSBDev			-	pointer to USB device
	Return Value:
		
	Note:
		
	========================================================================
*/
static NDIS_STATUS RTUSBEthLoad(
	DC_USB_INTERFACE *pIntf,
	DC_USB_DEVICE *pUSBDev)
{	
	PIFACE pNetDev = NULL;
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)NULL;
	NDIS_STATUS Result;
	PVOID pOSCookie;
	RTMP_OS_NETDEV_OP_HOOK netDevHook;
	
	/* Allocate RTMP_ADAPTER adapter structure */
	pOSCookie = kmalloc(sizeof(struct os_cookie), GFP_KERNEL);
	
	if (!pOSCookie)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Allocate memory for os handle failed.\n", __FUNCTION__));
		Result = DC_OUT_OF_MEMORY;
		goto error0;
	}

	((POS_COOKIE)pOSCookie)->pUsb_Dev = pUSBDev;
	
	Result = RTMPAllocAdapterBlock(pOSCookie, &pAd);

	if (Result != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Allocate memory for pAd failed.\n", __FUNCTION__));
		goto error1;
	}

	/* Initialize WIFI driver USB BULK IN/OUT attributes */
	Result = USBDevConfigInit(pUSBDev, pIntf, pAd);

	if(Result != NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Initialize WIFI driver USB BULK IN/OUT attributes failed.\n", __FUNCTION__));
		goto error2;
	}

	RtmpRaDevCtrlInit(pAd, RTMP_DEV_INF_USB);

	/* Register RT_WIFI Network driver */
	pNetDev = RtmpPhyNetDevInit(pAd, &netDevHook);

	/*All done, it's time to register the net device to threadX kernel. */
	Result = RtmpOSNetDevAttach(pNetDev, &netDevHook);
	
	if (Result == NDIS_STATUS_SUCCESS)
		return Result;
	
error2:
	RTMPFreeAdapter(pAd);

error1:
	kfree(pOSCookie);

error0:
	return Result;
}


DC_RETURN_CODE rtusb_probe(DC_USB_DEVICE *pUSBDev, DC_USB_INTERFACE *intf)
{

	DC_RETURN_CODE Result = FALSE;
	DC_USB_DEVICE_DESCRIPTOR_t	*dd = (DC_USB_DEVICE_DESCRIPTOR_t *)pUSBDev->dd;
	UINT32 Index;

	DBGPRINT(RT_DEBUG_TRACE, ("%s : USB Device Vendor ID = %x, Product ID = %x.", __FUNCTION__, dd->idVendor, dd->idProduct));

	for (Index = 0; Index < rtusb_usb_id_len; Index++)
	{
		if (dd->idVendor != rtusb_dev_id[Index].uVendorIDorClass || 
			dd->idProduct != rtusb_dev_id[Index].uProductIDorSubClass)
		{
			continue;
		}else{
			Result = TRUE;
			break;
		}
	}

	if (Result == TRUE)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s : Ralink driver supports this attached USB device.", __FUNCTION__));
		/* call generic probe procedure */
		Result = RTUSBEthLoad(intf, pUSBDev);
		if (Result == NDIS_STATUS_SUCCESS)
		{
			Result = TRUE;
		}
		else
			Result = FALSE;		
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Ralink driver do not support this attached USB device.", __FUNCTION__));
	}

	return Result;
}


void rtusb_disconnect(DC_USB_DEVICE *pUSBDev)
{
	

}


/* Ralink USB driver register API, must call after USB Core initialization */
DC_RETURN_CODE RTUSBDriverRegister()
{
	DC_RETURN_CODE Result;
	
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink USB Dongle Init."));

	classd_ralink.next = NULL;
	classd_ralink.prev = NULL;
	classd_ralink.probe = rtusb_probe;
	classd_ralink.disconnect = rtusb_disconnect;

	/* Register USB class driver into USB Core for future probe */
	Result = dc_uh_class_driver_register(&classd_ralink);

	if (Result != DC_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : dc_uh_class_driver_register failed.", __FUNCTION__));
	}

	return Result;
}


/*
========================================================================
Routine Description:
    USB command kernel thread.

Arguments:
	*Context			the pAd, driver control block pointer

Return Value:
    0					close the thread

Note:
========================================================================
*/
INT RTUSBCmdThread(
	IN ULONG Context)
{
	RTMP_ADAPTER *pAd;
	RTMP_OS_TASK *pTask;
	int status;
	
	pTask = (RTMP_OS_TASK *)Context;
	pAd = (PRTMP_ADAPTER)pTask->priv;
	
	RtmpOSTaskCustomize(pTask);

	NdisAcquireSpinLock(&pAd->CmdQLock);
	pAd->CmdQ.CmdQState = RTMP_TASK_STAT_RUNNING;
	NdisReleaseSpinLock(&pAd->CmdQLock);
	
	while (pAd->CmdQ.CmdQState == RTMP_TASK_STAT_RUNNING)
	{
		/* lock the device pointers */
		RTMP_SEM_EVENT_WAIT(&(pTask->taskSema), status);

		if (pAd->CmdQ.CmdQState == RTMP_TASK_STAT_STOPED)
			break;
		
		if (status != 0)
		{
#if 0	/* modify for ThreadX testing */
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			break;
#else
			dc_log_printf("CmdThread:breakFromSem\n");
			continue;
#endif
		}
		/* lock the device pointers , need to check if required*/
		/*down(&(pAd->usbdev_semaphore)); */

		if (!pAd->PM_FlgSuspend)
			CMDHandler(pAd);
	}

	if (!pAd->PM_FlgSuspend)
	{	/* Clear the CmdQElements. */
		CmdQElmt	*pCmdQElmt = NULL;

		NdisAcquireSpinLock(&pAd->CmdQLock);
		pAd->CmdQ.CmdQState = RTMP_TASK_STAT_STOPED;
		while(pAd->CmdQ.size)
		{
			RTThreadDequeueCmd(&pAd->CmdQ, &pCmdQElmt);
			if (pCmdQElmt)
			{
				if (pCmdQElmt->CmdFromNdis == TRUE)
				{
					if (pCmdQElmt->buffer != NULL)
						os_free_mem(pAd, pCmdQElmt->buffer);
					os_free_mem(pAd, (PUCHAR)pCmdQElmt);
				}
				else
				{
					if ((pCmdQElmt->buffer != NULL) && (pCmdQElmt->bufferlength != 0))
						os_free_mem(pAd, pCmdQElmt->buffer);
					os_free_mem(pAd, (PUCHAR)pCmdQElmt);
				}
			}
		}

		NdisReleaseSpinLock(&pAd->CmdQLock);
	}
	/* notify the exit routine that we're actually exiting now 
	 *
	 * complete()/wait_for_completion() is similar to up()/down(),
	 * except that complete() is safe in the case where the structure
	 * is getting deleted in a parallel mode of execution (i.e. just
	 * after the down() -- that's necessary for the thread-shutdown
	 * case.
	 *
	 * complete_and_exit() goes even further than this -- it is safe in
	 * the case that the thread of the caller is going away (not just
	 * the structure) -- this is necessary for the module-remove case.
	 * This is important in preemption kernels, which transfer the flow
	 * of execution immediately upon a complete().
	 */
	DBGPRINT(RT_DEBUG_TRACE,( "<---RTUSBCmdThread\n"));

	return 0;
}
