/****************************************************************************

    Module Name:
	rt_usb_util.c
 
    Abstract:
	Any utility is used in UTIL module for USB function.
 
    Revision History:
    Who        When          What
    ---------  ----------    ----------------------------------------------

***************************************************************************/

#define RTMP_MODULE_OS
#define RTMP_MODULE_OS_UTIL

#include "rtmp_comm.h"
#include "rtmp_osabl.h"
#include "rt_os_util.h"

#ifdef RTMP_MAC_USB
#ifdef OS_ABL_SUPPORT
MODULE_LICENSE("GPL");
#endif /* OS_ABL_SUPPORT */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
/*
========================================================================
Routine Description:
	Dump URB information.

Arguments:
	purb_org		- the URB

Return Value:
	None

Note:
========================================================================
*/
void dump_urb(VOID *purb_org)
{
	struct urb *purb = (struct urb *)purb_org;

	DBGPRINT(RT_DEBUG_OFF, ("urb                  :0x%08lx\n", (unsigned long)purb));
	DBGPRINT(RT_DEBUG_OFF, ("\tdev                   :0x%08lx\n", (unsigned long)purb->dev));
	DBGPRINT(RT_DEBUG_OFF, ("\t\tdev->state          :%d\n", purb->dev->state));
	DBGPRINT(RT_DEBUG_OFF, ("\tpipe                  :0x%08x\n", purb->pipe));
	DBGPRINT(RT_DEBUG_OFF, ("\tstatus                :%d\n", purb->status));
	DBGPRINT(RT_DEBUG_OFF, ("\ttransfer_flags        :0x%08x\n", purb->transfer_flags));
	DBGPRINT(RT_DEBUG_OFF, ("\ttransfer_buffer       :0x%08lx\n",
		(unsigned long)purb->transfer_buffer));
	DBGPRINT(RT_DEBUG_OFF, ("\ttransfer_buffer_length:%d\n", purb->transfer_buffer_length));
	DBGPRINT(RT_DEBUG_OFF, ("\tactual_length         :%d\n", purb->actual_length));
	DBGPRINT(RT_DEBUG_OFF, ("\tsetup_packet          :0x%08lx\n",
		(unsigned long)purb->setup_packet));
	DBGPRINT(RT_DEBUG_OFF, ("\tstart_frame           :%d\n", purb->start_frame));
	DBGPRINT(RT_DEBUG_OFF, ("\tnumber_of_packets     :%d\n", purb->number_of_packets));
	DBGPRINT(RT_DEBUG_OFF, ("\tinterval              :%d\n", purb->interval));
	DBGPRINT(RT_DEBUG_OFF, ("\terror_count           :%d\n", purb->error_count));
	DBGPRINT(RT_DEBUG_OFF, ("\tcontext               :0x%08lx\n",
		(unsigned long)purb->context));
	DBGPRINT(RT_DEBUG_OFF, ("\tcomplete              :0x%08lx\n\n",
		(unsigned long)purb->complete));
}
#else
void dump_urb(VOID *purb_org)
{
	return;
}
#endif /* LINUX_VERSION_CODE */



#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND


void rausb_autopm_put_interface( void *intf)
{
	usb_autopm_put_interface((struct usb_interface *)intf);
}

int  rausb_autopm_get_interface( void *intf)
{
	return usb_autopm_get_interface((struct usb_interface *)intf);
}



/*
========================================================================
Routine Description:
	RTMP_Usb_AutoPM_Put_Interface

Arguments:
	

Return Value:
	

Note:
========================================================================
*/

int RTMP_Usb_AutoPM_Put_Interface (
	IN	VOID			*pUsb_Devsrc,
	IN	VOID			*intfsrc)
{

	INT	 pm_usage_cnt;
	struct usb_interface	*intf =(struct usb_interface *)intfsrc;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
		pm_usage_cnt = atomic_read(&intf->pm_usage_cnt);	
#else
		pm_usage_cnt = intf->pm_usage_cnt;
#endif
		
		if (pm_usage_cnt == 1)
		{
			rausb_autopm_put_interface(intf);

              }

			return 0;
}

EXPORT_SYMBOL(RTMP_Usb_AutoPM_Put_Interface);

/*
========================================================================
Routine Description:
	RTMP_Usb_AutoPM_Get_Interface

Arguments:
	

Return Value: (-1)  error (resume fail )    1 success ( resume success)  2  (do  nothing)
	

Note:
========================================================================
*/

int RTMP_Usb_AutoPM_Get_Interface (
	IN	VOID			*pUsb_Devsrc,
	IN	VOID			*intfsrc)
{

	INT	 pm_usage_cnt;
	struct usb_interface	*intf =(struct usb_interface *)intfsrc;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
	pm_usage_cnt = (INT)atomic_read(&intf->pm_usage_cnt);	
#else
	pm_usage_cnt = intf->pm_usage_cnt;
#endif

	if (pm_usage_cnt == 0)
	{
		int res=1;
		
		res = rausb_autopm_get_interface(intf);
		if (res)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("AsicSwitchChannel autopm_resume fail ------\n"));
			return (-1);
		}			
			
	}
	return 2;


}

EXPORT_SYMBOL(RTMP_Usb_AutoPM_Get_Interface);

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */



#ifdef OS_ABL_SUPPORT
/*
========================================================================
Routine Description:
	Register a USB driver.

Arguments:
	new_driver		- the driver

Return Value:
	0				- successfully
	Otherwise		- fail

Note:
========================================================================
*/
int rausb_register(VOID * new_driver)
{
	return usb_register((struct usb_driver *)new_driver);
}
EXPORT_SYMBOL(rausb_register);


/*
========================================================================
Routine Description:
	De-Register a USB driver.

Arguments:
	new_driver		- the driver

Return Value:
	None

Note:
========================================================================
*/
void rausb_deregister(VOID * driver)
{
	usb_deregister((struct usb_driver *)driver);
}
EXPORT_SYMBOL(rausb_deregister);


/*
========================================================================
Routine Description:
	Create a new urb for a USB driver to use.

Arguments:
	iso_packets		- number of iso packets for this urb

Return Value:
	the URB

Note:
========================================================================
*/
struct urb *rausb_alloc_urb(int iso_packets)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	return usb_alloc_urb(iso_packets, GFP_ATOMIC);
#else
	return usb_alloc_urb(iso_packets);
#endif /* LINUX_VERSION_CODE */
}
EXPORT_SYMBOL(rausb_alloc_urb);


/*
========================================================================
Routine Description:
	Free the memory used by a urb.

Arguments:
	urb				- the URB

Return Value:
	None

Note:
========================================================================
*/
void rausb_free_urb(VOID *urb)
{
	usb_free_urb((struct urb *)urb);
}
EXPORT_SYMBOL(rausb_free_urb);


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
/*
========================================================================
Routine Description:
	Release a use of the usb device structure.

Arguments:
	dev				- the USB device

Return Value:
	None

Note:
========================================================================
*/
void rausb_put_dev(VOID *dev)
{
	usb_put_dev((struct usb_device *)dev);
}
EXPORT_SYMBOL(rausb_put_dev);


/*
========================================================================
Routine Description:
	Increments the reference count of the usb device structure.

Arguments:
	dev				- the USB device

Return Value:
	the device with the incremented reference counter

Note:
========================================================================
*/
struct usb_device *rausb_get_dev(VOID *dev)
{
	return usb_get_dev((struct usb_device *)dev);
}
EXPORT_SYMBOL(rausb_get_dev);
#endif /* LINUX_VERSION_CODE */


/*
========================================================================
Routine Description:
	Issue an asynchronous transfer request for an endpoint.

Arguments:
	urb				- the URB

Return Value:
	0				- successfully
	Otherwise		- fail

Note:
========================================================================
*/
int rausb_submit_urb(VOID *urb)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	return usb_submit_urb((struct urb *)urb, GFP_ATOMIC);
#else
	return usb_submit_urb((struct urb *)urb);
#endif /* LINUX_VERSION_CODE */
}
EXPORT_SYMBOL(rausb_submit_urb);

/*
========================================================================
Routine Description:
	Allocate dma-consistent buffer.

Arguments:
	dev				- the USB device
	size			- buffer size
	dma				- used to return DMA address of buffer

Return Value:
	a buffer that may be used to perform DMA to the specified device

Note:
========================================================================
*/
void *rausb_buffer_alloc(VOID *dev,
							size_t size,
							ra_dma_addr_t *dma)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	dma_addr_t DmaAddr = (dma_addr_t)(*dma);
	void *buf;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	buf = usb_alloc_coherent(dev, size, GFP_ATOMIC, &DmaAddr);
#else
	buf = usb_buffer_alloc(dev, size, GFP_ATOMIC, &DmaAddr);
#endif
	*dma = (ra_dma_addr_t)DmaAddr;
	return buf;

#else
	return kmalloc(size, GFP_ATOMIC);
#endif
}
EXPORT_SYMBOL(rausb_buffer_alloc);


/*
========================================================================
Routine Description:
	Free memory allocated with usb_buffer_alloc.

Arguments:
	dev				- the USB device
	size			- buffer size
	addr			- CPU address of buffer
	dma				- used to return DMA address of buffer

Return Value:
	None

Note:
========================================================================
*/
void rausb_buffer_free(VOID *dev,
							size_t size,
							void *addr,
							ra_dma_addr_t dma)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	dma_addr_t DmaAddr = (dma_addr_t)(dma);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	usb_free_coherent(dev, size, addr, DmaAddr);
#else
	usb_buffer_free(dev, size, addr, DmaAddr);
#endif
#else
	kfree(addr);
#endif
}
EXPORT_SYMBOL(rausb_buffer_free);

void rausb_fill_bulk_urb(void *urb,
						 void *dev,
						 unsigned int pipe,
						 void *transfer_buffer,
						 int buffer_length,
						 USB_COMPLETE_HANDLER complete_fn,
						 void *context)
{
	usb_fill_bulk_urb(urb, dev, pipe, transfer_buffer, buffer_length, complete_fn, context);

}
EXPORT_SYMBOL(rausb_fill_bulk_urb);


/*
========================================================================
Routine Description:
	Send a control message to a device.

Arguments:
	dev				- the USB device

Return Value:
	0				- successfully
	Otherwise		- fail

Note:
========================================================================
*/
int rausb_control_msg(VOID *dev,
						unsigned int pipe,
						__u8 request,
						__u8 requesttype,
						__u16 value,
						__u16 index,
						void *data,
						__u16 size,
						int timeout)
{
	int ret;

	ret = usb_control_msg((struct usb_device *)dev, pipe, request, requesttype, value, index,
							data, size, timeout);
	if (ret == -ENODEV)
		return RTMP_USB_CONTROL_MSG_ENODEV;
	if (ret < 0)
		return RTMP_USB_CONTROL_MSG_FAIL;
	return ret;
}
EXPORT_SYMBOL(rausb_control_msg);

unsigned int rausb_sndctrlpipe(VOID *dev, ULONG address)
{
	return usb_sndctrlpipe(dev, address);
}
EXPORT_SYMBOL(rausb_sndctrlpipe);

unsigned int rausb_rcvctrlpipe(VOID *dev, ULONG address)
{
	return usb_rcvctrlpipe(dev, address);
}
EXPORT_SYMBOL(rausb_rcvctrlpipe);

unsigned int rausb_sndbulkpipe(void *dev, ULONG address)
{
	return usb_sndbulkpipe(dev, address);
}
EXPORT_SYMBOL(rausb_sndbulkpipe);

unsigned int rausb_rcvbulkpipe(void *dev, ULONG address)
{
	return usb_rcvbulkpipe(dev, address);
}
EXPORT_SYMBOL(rausb_rcvbulkpipe);


/*
========================================================================
Routine Description:
	Cancel a transfer request and wait for it to finish.

Arguments:
	urb				- the URB

Return Value:
	None

Note:
========================================================================
*/
void rausb_kill_urb(VOID *urb)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,7)
	usb_kill_urb((struct urb *)urb);
#else
	usb_unlink_urb((struct urb *)urb);
#endif /* LINUX_VERSION_CODE */
}
EXPORT_SYMBOL(rausb_kill_urb);

#endif /* OS_ABL_SUPPORT */


VOID RtmpOsUsbInitHTTxDesc(
	IN	VOID			*pUrbSrc,
	IN	VOID			*pUsb_Dev,
	IN	UINT			BulkOutEpAddr,
	IN	PUCHAR			pSrc,
	IN	ULONG			BulkOutSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	VOID			*pTxContext,
	IN	ra_dma_addr_t		TransferDma)
{
	PURB pUrb = (PURB)pUrbSrc;
	dma_addr_t DmaAddr = (dma_addr_t)(TransferDma);


	ASSERT(pUrb);

	/*Initialize a tx bulk urb */
	RTUSB_FILL_HTTX_BULK_URB(pUrb,
						pUsb_Dev,
						BulkOutEpAddr,
						pSrc,
						BulkOutSize,
						(usb_complete_t)Func,
						pTxContext,
						DmaAddr);
}


VOID	RtmpOsUsbInitRxDesc(
	IN	VOID			*pUrbSrc,
	IN	VOID			*pUsb_Dev,
	IN	UINT			BulkInEpAddr,
	IN	UCHAR			*pTransferBuffer,
	IN	UINT32			BufSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	VOID			*pRxContext,
	IN	ra_dma_addr_t		TransferDma)
{
	PURB pUrb = (PURB)pUrbSrc;
	dma_addr_t DmaAddr = (dma_addr_t)(TransferDma);


	ASSERT(pUrb);

	/*Initialize a rx bulk urb */
	RTUSB_FILL_RX_BULK_URB(pUrb,
						pUsb_Dev,
						BulkInEpAddr,
						pTransferBuffer,
						BufSize,
						(usb_complete_t)Func,
						pRxContext,
						DmaAddr);
}


VOID *RtmpOsUsbContextGet(
	IN	VOID			*pUrb)
{
	return ((purbb_t)pUrb)->rtusb_urb_context;
}


NTSTATUS RtmpOsUsbStatusGet(
	IN	VOID			*pUrb)
{
	return ((purbb_t)pUrb)->rtusb_urb_status;
}


VOID RtmpOsUsbDmaMapping(
	IN	VOID			*pUrb)
{
	RTUSB_URB_DMA_MAPPING(((purbb_t)pUrb));
}


/*
========================================================================
Routine Description:
	Get the data pointer from the URB.

Arguments:
	pUrb			- USB URB

Return Value:
	the data pointer

Note:
========================================================================
*/
VOID *RtmpOsUsbUrbDataGet(
	IN	VOID					*pUrb)
{
	return RTMP_USB_URB_DATA_GET(pUrb);
}


/*
========================================================================
Routine Description:
	Get the status from the URB.

Arguments:
	pUrb			- USB URB

Return Value:
	the status

Note:
========================================================================
*/
NTSTATUS RtmpOsUsbUrbStatusGet(
	IN	VOID					*pUrb)
{
	return RTMP_USB_URB_STATUS_GET(pUrb);
}


/*
========================================================================
Routine Description:
	Get the data length from the URB.

Arguments:
	pUrb			- USB URB

Return Value:
	the data length

Note:
========================================================================
*/
ULONG RtmpOsUsbUrbLenGet(
	IN	VOID					*pUrb)
{
	return RTMP_USB_URB_LEN_GET(pUrb);
}

/*
========================================================================
Routine Description:
	Get USB Vendor ID.

Arguments:
	pUsbDev			- the usb device

Return Value:
	the name

Note:
========================================================================
*/
UINT32 RtmpOsGetUsbDevVendorID(IN VOID *pUsbDev) {
	return ((struct usb_device *) pUsbDev)->descriptor.idVendor;
}

/*
========================================================================
Routine Description:
	Get USB Product ID.

Arguments:
	pUsbDev			- the usb device

Return Value:
	the name

Note:
========================================================================
*/
UINT32 RtmpOsGetUsbDevProductID(IN VOID *pUsbDev) {
	return ((struct usb_device *) pUsbDev)->descriptor.idProduct;
}

#endif /* RTMP_MAC_USB */

