/*
 * drivers/usb/gadget/cvi_udc_otg.c
 * Designware CVI on-chip full/high speed USB OTG 2.0 device controllers
 *
 * Copyright (C) 2008 for Samsung Electronics
 *
 * BSP Support for Samsung's UDC driver
 * available at:
 * git://git.kernel.org/pub/scm/linux/kernel/git/kki_ap/linux-2.6-samsung.git
 *
 * State machine bugfixes:
 * Marek Szyprowski <m.szyprowski@samsung.com>
 *
 * Ported to u-boot:
 * Marek Szyprowski <m.szyprowski@samsung.com>
 * Lukasz Majewski <l.majewski@samsumg.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <stdlib.h>
#include "include/cvi_ch9.h"
#include "include/cvi_drv_if.h"
#include "include/byteorder.h"
#include "include/cvi_stdtypes.h"
#include "include/cvi_errno.h"
#include "include/cvi_udc_otg_regs.h"
#include "include/cvi_udc_otg_priv.h"
#include "include/cvi_udc.h"
#include "include/dps.h"

/***********************************************************/
#define DRIVER_VERSION "15 March 2009"

static const char ep0name[] = "ep0-control";
static const char ep1name[] = "ep1in-bulk";
static const char ep2name[] = "ep2out-bulk";
static const char ep3name[] = "ep3in-int";

static struct cvi_udc	*the_controller;

static const char driver_name[] = "cvi-udc";

/*
 * Local declarations.
 */

static int cvi_ep_enable(struct usb_ep *ep,
			  const CH9_UsbEndpointDescriptor *);
static int cvi_ep_disable(struct usb_ep *ep);
static struct usb_request *cvi_alloc_request(struct usb_ep *ep);
static void cvi_free_request(struct usb_ep *ep, struct usb_request *);

static int cvi_dequeue(struct usb_ep *ep, struct usb_request *);
static int cvi_fifo_status(struct usb_ep *ep);
static void cvi_fifo_flush(struct usb_ep *ep);
static void stop_activity(struct cvi_udc *dev,
			  struct usb_gadget_driver *driver);
static int udc_enable(struct cvi_udc *dev);
static void cvi_usbd_init(struct cvi_udc *dev);
static void udc_reinit(struct cvi_udc *dev);
static int _cvi_ep_disable(struct cvi_ep *ep);

#undef CVI_LOG

#if defined(CVI_LOG)

#define CVI_LOG_ENTRY_NUM	1024

struct cvi_log_s {
	uint32_t time;
	uint32_t tag;
	uint32_t param1;
	uint32_t param2;
	uint32_t param3;
	uint32_t param4;
};

static unsigned int log_idx;
static struct cvi_log_s cvi_log[CVI_LOG_ENTRY_NUM];

void cvi_log_write(uint32_t tag, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4)
{
	//if (log_idx == CVI_LOG_ENTRY_NUM)
	//	return;

	cvi_log[log_idx].tag = tag;
	cvi_log[log_idx].param1 = param1;
	cvi_log[log_idx].param2 = param2;
	cvi_log[log_idx].param3 = param3;
	cvi_log[log_idx].param4 = param4;
	cvi_log[log_idx].time = timer_get_us() / 1000;

	log_idx++;
	log_idx = log_idx % CVI_LOG_ENTRY_NUM;
}

void set_trigger_cnt(int cnt)
{
	static int test_reset;
	uint32_t *test_ptr = (uint32_t *)0x85000000;

	if (test_reset == cnt)
		*test_ptr = 0xAAA;
	test_reset++;
}

#else

void cvi_log_write(uint32_t tag, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4)
{
}

void set_trigger_cnt(int cnt)
{
}

#endif

static struct usb_ep_ops cvi_ep_ops = {
	.enable = cvi_ep_enable,
	.disable = cvi_ep_disable,

	.alloc_request = cvi_alloc_request,
	.free_request = cvi_free_request,

	.queue = cvi_queue,
	.dequeue = cvi_dequeue,

	.set_halt = cvi_udc_set_halt,
	.fifo_status = cvi_fifo_status,
	.fifo_flush = cvi_fifo_flush,
};

#define create_proc_files() do {} while (0)
#define remove_proc_files() do {} while (0)

/***********************************************************/

const char *cvi_get_ep0_name(void)
{
	return ep0name;
}

static struct cvi_usbotg_reg *reg;

__weak void otg_phy_init(struct cvi_udc *dev) {}
__weak void otg_phy_off(struct cvi_udc *dev) {}

/***********************************************************/

//#include "cvi_udc_otg_xfer_dma.c"

/***********************************************************/
/*
 *	udc_disable - disable USB device controller
 */
static void udc_disable(struct cvi_udc *dev)
{
	cvidbg_cond(DEBUG_SETUP != 0, "%s: %p\n", __func__, dev);

	cvi_set_address(dev, 0);

	dev->ep0state = WAIT_FOR_SETUP;
	dev->gadget.speed = CH9_USB_SPEED_UNKNOWN;
	dev->usb_address = 0;

	otg_phy_off(dev);
}

/*
 *	udc_reinit - initialize software state
 */
static void udc_reinit(struct cvi_udc *dev)
{
	unsigned int i;

	cvidbg_cond(DEBUG_SETUP != 0, "%s: %p\n", __func__, dev);

	/* device/ep0 records init */
	INIT_LIST_HEAD(&dev->gadget.ep_list);
	INIT_LIST_HEAD(&dev->gadget.ep0->ep_list);
	dev->ep0state = WAIT_FOR_SETUP;

	/* basic endpoint records init */
	for (i = 0; i < CVI_MAX_ENDPOINTS; i++) {
		struct cvi_ep *ep = &dev->ep[i];

		if (i != 0)
			list_add_tail(&ep->ep.ep_list, &dev->gadget.ep_list);

		ep->desc = 0;
		ep->stopped = 0;
		INIT_LIST_HEAD(&ep->queue);
		ep->pio_irqs = 0;
	}

	/* the rest was statically initialized, and is read-only */
}

#define BYTES2MAXP(x)	((x) / 8)
#define MAXP2BYTES(x)	((x) * 8)

/* until it's enabled, this UDC should be completely invisible
 * to any USB host.
 */
static int udc_enable(struct cvi_udc *dev)
{
	cvidbg_cond(DEBUG_SETUP != 0, "%s: %p\n", __func__, dev);

	otg_phy_init(dev);
	cvi_usbd_init(dev);
	cvi_reconfig_usbd(dev, 0);

	cvidbg_cond(DEBUG_SETUP != 0,
		     "CVI USB 2.0 OTG Controller Core Initialized : 0x%x\n",
		     cvi_uncached_read32(&reg->gintmsk));

	dev->gadget.speed = CH9_USB_SPEED_UNKNOWN;

	return 0;
}

/*
 * Register entry point for the peripheral controller driver.
 */
int cviusb_gadget_register_driver(struct usb_gadget_driver *driver)
{
	struct cvi_udc *dev = the_controller;
	int retval = 0;

	cvidbg_cond(DEBUG_SETUP != 0, "%s: %s\n", __func__, "no name");

	if (!driver ||
	    (driver->speed != CH9_USB_SPEED_FULL &&
	     driver->speed != CH9_USB_SPEED_HIGH) ||
	    !driver->bind || !driver->disconnect ||
	    !driver->setup || !driver->req_mem_alloc ||
	    !driver->req_mem_free)
		return -EINVAL;
	if (!dev)
		return -ENODEV;
	if (dev->driver)
		return -EBUSY;

	/* first hook up the driver ... */
	dev->driver = driver;

	if (retval) { /* TODO */
		printf("target device_add failed, error %d\n", retval);
		return retval;
	}

	retval = driver->bind(&dev->gadget);
	if (retval) {
		cvidbg_cond(DEBUG_SETUP != 0,
			     "%s: bind to driver --> error %d\n",
			     dev->gadget.name, retval);
		dev->driver = 0;
		return retval;
	}

#if defined(USB_IRQ_MODE)
	enable_irq(USB_IRQS_0);
#endif
	cvidbg_cond(DEBUG_SETUP != 0,
		     "Registered gadget driver %s\n", dev->gadget.name);
	udc_enable(dev);

	return 0;
}

/*
 * Unregister entry point for the peripheral controller driver.
 */
int cviusb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
	struct cvi_udc *dev = the_controller;

	if (!dev)
		return -ENODEV;
	if (!driver || driver != dev->driver)
		return -EINVAL;

	dev->driver = 0;
	stop_activity(dev, driver);

	driver->unbind(&dev->gadget);

#if defined(USB_IRQ_MODE)
	disable_irq(USB_IRQS_0);
#endif

	udc_disable(dev);
	return 0;
}

/*
 *	cvi_done - retire a request; caller blocked irqs
 */
void cvi_done(struct cvi_ep *ep, struct cvi_request *req, int status)
{
	unsigned int stopped = ep->stopped;

	cvidbg("%s: %s %p, req = %p, stopped = %d\n",
		__func__, ep->ep.name, ep, &req->req, stopped);

	list_del_init(&req->queue);

	if (req->req.status == -EINPROGRESS)
		req->req.status = status;
	else
		status = req->req.status;

	if (status && status != -ESHUTDOWN) {
		cvidbg("complete %s req %p stat %d len %u/%u\n",
			ep->ep.name, &req->req, status,
			req->req.actual, req->req.length);
	}

	/* don't modify queue heads during completion callback */
	ep->stopped = 1;

#ifdef DEBUG
	printf("calling complete callback\n");
	{
		int i, len = req->req.length;

		printf("pkt[%d] = ", req->req.length);
		if (len > 64)
			len = 64;
		for (i = 0; i < len; i++) {
			printf("%02x", ((uint8_t *)req->req.buf)[i]);
			if ((i & 7) == 7)
				printf(" ");
		}
		printf("\n");
	}
#endif
	req->req.complete(&ep->ep, &req->req);

	cvidbg("callback completed\n");

	ep->stopped = stopped;
}

/*
 *	cvi_nuke - dequeue ALL requests
 */
void cvi_nuke(struct cvi_ep *ep, int status)
{
	struct cvi_request *req;

	cvidbg("%s: %s %p\n", __func__, ep->ep.name, ep);

	/* called with irqs blocked */
	while (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cvi_request, queue);
		cvi_done(ep, req, status);
	}
}

static void stop_activity(struct cvi_udc *dev,
			  struct usb_gadget_driver *driver)
{
	int i;

	/* don't disconnect drivers more than once */
	if (dev->gadget.speed == CH9_USB_SPEED_UNKNOWN)
		driver = 0;
	dev->gadget.speed = CH9_USB_SPEED_UNKNOWN;

	/* prevent new request submissions, kill any outstanding requests  */
	for (i = 0; i < CVI_MAX_ENDPOINTS; i++) {
		struct cvi_ep *ep = &dev->ep[i];

		ep->stopped = 1;
		cvi_nuke(ep, -ESHUTDOWN);
	}

	/* report disconnect; the driver is already quiesced */
	if (driver) {
		driver->disconnect(&dev->gadget);
	}

	/* re-init driver-visible data structures */
	udc_reinit(dev);
}

static void cvi_hsotg_init_fifo(struct cvi_udc *dev)
{
	uint32_t rx_fifo_sz, tx_fifo_sz, np_tx_fifo_sz;
	int timeout, i;
	uint32_t val;

	if (cvi_hsotg_wait_bit_set(&reg->grstctl, AHBIDLE, 10000))
		printf("%s:  HANG! AHB Idle GRSCTL\n", __func__);

	/* setup fifos */
	rx_fifo_sz = RX_FIFO_SIZE;
	np_tx_fifo_sz = NPTX_FIFO_SIZE;
	tx_fifo_sz = PTX_FIFO_SIZE;

	/* Set Rx FIFO Size (in 32-bit words) */
	cvi_uncached_write32(rx_fifo_sz, &reg->grxfsiz);

	/* Set Non Periodic Tx FIFO Size (TXFIFO[0]) */
	cvi_uncached_write32((np_tx_fifo_sz << 16) | rx_fifo_sz,
			      &reg->gnptxfsiz);

	for (i = 1; i < CVI_MAX_ENDPOINTS; i++)
		cvi_uncached_write32((rx_fifo_sz + np_tx_fifo_sz + tx_fifo_sz * (i - 1)) |
				      tx_fifo_sz << 16, &reg->dieptxf[i - 1]);

	/* Flush all the FIFO's */
	cvi_uncached_write32(TX_FIFO_FLUSH_ALL |
			      TX_FIFO_FLUSH |
			      RX_FIFO_FLUSH,
			      &reg->grstctl);
	timeout = 100;
	while (1) {
		val = cvi_uncached_read32(&reg->grstctl);

		if ((val & (TX_FIFO_FLUSH | RX_FIFO_FLUSH)) == 0)
			break;

		if (--timeout == 0) {
			printf("%s: timeout flushing fifos (grstctl = 0x%x)\n",
			       __func__, val);
			break;
		}
		udelay(1);
	}
}

static void cvi_hsotg_txfifo_flush(struct cvi_udc *dev, unsigned int idx)
{
	int timeout;
	int val;

	if (cvi_hsotg_wait_bit_set(&reg->grstctl, AHBIDLE, 10000))
		printf("%s:  HANG! AHB Idle GRSCTL\n", __func__);

	cvi_uncached_write32(TX_FIFO_NUMBER(idx) | TX_FIFO_FLUSH, &reg->grstctl);

	/* wait until the fifo is flushed */
	timeout = 100;

	while (1) {
		val = cvi_uncached_read32(&reg->grstctl);

		if ((val & (TX_FIFO_FLUSH)) == 0)
			break;

		if (--timeout == 0) {
			printf("%s: timeout flushing fifo (GRSTCTL=%08x)\n",
			       __func__, val);
			break;
		}

		udelay(1);
	}

	/* Wait for at least 3 PHY Clocks */
	udelay(1);
}

static void kill_all_requests(struct cvi_udc *dev, struct cvi_ep *ep, int result)
{
	uint32_t ep_num = ep_index(ep);
	uint32_t size_max = (ep_num == 0) ? (NPTX_FIFO_SIZE * 4) : (PTX_FIFO_SIZE * 4);
	uint32_t size;

	cvidbg("%s: %p\n", __func__, ep);

	/* make sure it's actually queued on this endpoint */
	cvi_nuke(ep, result);

	size = (cvi_uncached_read32(&reg->in_endp[ep_num].dtxfsts) & 0xFFFF) * 4;
	if (size < size_max)
		cvi_hsotg_txfifo_flush(dev, ep->fifo_num);

}

void cvi_disconnect(struct cvi_udc *dev)
{
	int i;

	if (!dev->connected)
		return;

	dev->connected = 0;

	for (i = 1; i < CVI_MAX_ENDPOINTS; i++) {
		struct cvi_ep *ep = &dev->ep[i];

		if (ep->ep.name) {
			kill_all_requests(dev, ep, -ESHUTDOWN);
		}
	}

	/* HACK to let gadget detect disconnected state */
	if (dev->driver->disconnect) {
		dev->driver->disconnect(&dev->gadget);
	}
}

void cvi_reconfig_usbd(struct cvi_udc *dev, int is_usb_reset)
{
	/* 2. Soft-reset OTG Core and then unreset again. */
	unsigned int val;
	uint32_t dflt_gusbcfg;
	struct cvi_plat_otg_data *pdata = (struct cvi_plat_otg_data *)dev->pdata;
	struct cvi_ep *ep = &dev->ep[0];

	cvidbg("Resetting OTG controller\n");

	kill_all_requests(dev, ep, -ECONNRESET);
	udc_reinit(dev);
	if (!is_usb_reset) {
		uint32_t greset;
		int count = 0;
		uint32_t snpsid = cvi_uncached_read32(&reg->gsnpsid) & CVI_CORE_REV_MASK;

		/* check snpsid */
		if (snpsid < (CVI_CORE_REV_4_20a & CVI_CORE_REV_MASK)) {
			/* Core Soft Reset */
			cvi_uncached_write32(CORE_SOFT_RESET, &reg->grstctl);
			do {
				udelay(1);
				greset = cvi_uncached_read32(&reg->grstctl);
				if (++count > 50) {
					printf("%s() HANG! Soft Reset GRSTCTL=%0x\n", __func__, greset);
					return;
				}
			} while (greset & CORE_SOFT_RESET);
		} else {
			/* Core Soft Reset */
			cvi_uncached_write32(CORE_SOFT_RESET, &reg->grstctl);
			do {
				udelay(1);
				greset = cvi_uncached_read32(&reg->grstctl);
				if (++count > 50) {
					printf("%s() HANG! Soft 4.2 Reset GRSTCTL=%0x\n",
						__func__, greset);
					return;
				}
			} while (!(greset & CSFTRST_DONE));
			greset = cvi_uncached_read32(&reg->grstctl);
			greset &= ~CORE_SOFT_RESET;
			greset |= CSFTRST_DONE;
			cvi_uncached_write32(greset, &reg->grstctl);
		}

		/* Wait for AHB master IDLE state */
		count = 0;
		do {
			udelay(1);
			greset = cvi_uncached_read32(&reg->grstctl);
			if (++count > 50) {
				printf("%s() HANG! AHB Idle GRSTCTL=%0x\n",
					__func__, greset);
				return;
			}
		} while (!(greset & AHB_MASTER_IDLE));
	} else {
		int i;

		for (i = 1; i < CVI_MAX_ENDPOINTS; i++) {
			struct cvi_ep *ep = &dev->ep[i];

			if (ep->ep.name)
				_cvi_ep_disable(ep);
		}
	}

	dflt_gusbcfg =
		1 << 30		/* ForceDevMode.*/
		| 1 << 19		/* 1'b1: PHY does not power down internal clock.*/
		| 0 << 15		/* PHY Low Power Clock sel*/
		| 0 << 14		/* Non-Periodic TxFIFO Rewind Enable*/
		| 0x5 << 10	/* Turnaround time*/
		| 0 << 9 | 0 << 8	/* [0:HNP disable,1:HNP enable][ 0:SRP disable*/
		/* 1:SRP enable] H1= 1,1*/
		| 0 << 7		/* Ulpi DDR sel*/
		| 0 << 6		/* 0: high speed utmi+, 1: full speed serial*/
		| 0 << 4		/* 0: utmi+, 1:ulpi*/
		| 1 << 3		/* phy i/f  0:8bit, 1:16bit*/
		| 0x7 << 0;	/* HS/FS Timeout**/

	if (pdata->usb_gusbcfg)
		dflt_gusbcfg = pdata->usb_gusbcfg;

	cvi_uncached_write32(dflt_gusbcfg, &reg->gusbcfg);

	cvi_hsotg_init_fifo(dev);

	if (!is_usb_reset) {
		/* Put the OTG device core in the disconnected state.*/
		val = cvi_uncached_read32(&reg->dctl);
		val |= SOFT_DISCONNECT;
		cvi_uncached_write32(val, &reg->dctl);
	}

	/* Configure OTG Core to initial settings of device mode.*/
	/* [1: full speed(30Mhz) 0:high speed]*/
	cvi_uncached_write32(EP_MISS_CNT(1) | DEV_SPEED_HIGH_SPEED_20, &reg->dcfg);

	/* Clear any pending OTG interrupts */
	cvi_uncached_write32(0xffffffff, &reg->gotgint);

	/* Clear any pending interrupts */
	cvi_uncached_write32(0xffffffff, &reg->gintsts);

	/* Unmask the core interrupts*/
	cvi_uncached_write32(GINTMSK_INIT, &reg->gintmsk);

	/* Initialize ahbcfg.*/
	cvi_uncached_write32(GAHBCFG_INIT, &reg->gahbcfg);

	/* Unmask device IN EP common interrupts*/
	cvi_uncached_write32(DIEPMSK_INIT, &reg->diepmsk);

	/* Unmask device OUT EP common interrupts*/
	cvi_uncached_write32(DOEPMSK_INIT, &reg->doepmsk);

	/* Unmask EPO interrupts*/
	cvi_uncached_write32(((1 << EP0_CON) << DAINT_OUT_BIT)
			      | (1 << EP0_CON), &reg->daintmsk);

	if (!is_usb_reset) {
		val = cvi_uncached_read32(&reg->dctl);
		val |= PWRONPRGDONE;
		cvi_uncached_write32(val, &reg->dctl);
		udelay(10);  /* see openiboot */
		val = cvi_uncached_read32(&reg->dctl);
		val &= ~PWRONPRGDONE;
		cvi_uncached_write32(val, &reg->dctl);
	}

	/* prepare the setup */
	cvi_udc_pre_setup(dev);
	/* enable, but don't activate EP0in */
	cvi_uncached_write32(DEPCTL_USBACTEP, &reg->in_endp[0].diepctl);

	/* clear global NAKs */
	val = CGOUTNAK | CGNPINNAK;
	if (!is_usb_reset)
		val |= SOFT_DISCONNECT;
	val |= cvi_uncached_read32(&reg->dctl);
	cvi_uncached_write32(val, &reg->dctl);

	/* must be at-least 3ms to allow bus to see disconnect */
	mdelay(3);
}

static void cvi_usbd_init(struct cvi_udc *dev)
{
	unsigned int temp;
	uint32_t dflt_gusbcfg;
	struct cvi_plat_otg_data *pdata = (struct cvi_plat_otg_data *)dev->pdata;

	cvidbg("Init OTG controller\n");

	/* Unmask subset of endpoint interrupts */
	cvi_uncached_write32(DOEPMSK_INIT, &reg->doepmsk);
	cvi_uncached_write32(DIEPMSK_INIT, &reg->diepmsk);
	cvi_uncached_write32(0, &reg->daintmsk);

	/* Be in disconnected state until gadget is registered */
	temp = cvi_uncached_read32(&reg->dctl);
	temp |= SOFT_DISCONNECT;
	cvi_uncached_write32(temp, &reg->dctl);

	/* setup fifo*/
	cvi_hsotg_init_fifo(dev);

	dflt_gusbcfg =
		1 << 30		/* ForceDevMode.*/
		| 1 << 19		/* 1'b1: PHY does not power down internal clock.*/
		| 0 << 15		/* PHY Low Power Clock sel*/
		| 0 << 14		/* Non-Periodic TxFIFO Rewind Enable*/
		| 0x5 << 10	/* Turnaround time*/
		| 0 << 9 | 0 << 8	/* [0:HNP disable,1:HNP enable][ 0:SRP disable*/
		/* 1:SRP enable] H1= 1,1*/
		| 0 << 7		/* Ulpi DDR sel*/
		| 0 << 6		/* 0: high speed utmi+, 1: full speed serial*/
		| 0 << 4		/* 0: utmi+, 1:ulpi*/
		| 0 << 3		/* phy i/f  0:8bit, 1:16bit*/
		| 0x7 << 0;	/* HS/FS Timeout**/

	if (pdata->usb_gusbcfg)
		dflt_gusbcfg = pdata->usb_gusbcfg;

	cvi_uncached_write32(dflt_gusbcfg, &reg->gusbcfg);
	/* Initialize OTG Link Core.*/
	cvi_uncached_write32(GAHBCFG_INIT, &reg->gahbcfg);
}

static int cvi_ep_enable(struct usb_ep *_ep,
			  const CH9_UsbEndpointDescriptor *desc)
{
	struct cvi_ep *ep;
	struct cvi_udc *dev;

	cvidbg("%s: %p\n", __func__, _ep);

	ep = container_of(_ep, struct cvi_ep, ep);
	if (!_ep || !desc || ep->desc || _ep->name == ep0name ||
	    desc->bDescriptorType != CH9_USB_DT_ENDPOINT ||
	    ep->bEndpointAddress != desc->bEndpointAddress ||
	    ep_maxpacket(ep) < le16ToCpu(desc->wMaxPacketSize)) {

		cvidbg("%s: bad ep or descriptor\n", __func__);
		return -EINVAL;
	}

	/* xfer types must match, except that interrupt ~= bulk */
	if (ep->bmAttributes != desc->bmAttributes &&
	    ep->bmAttributes != CH9_USB_EP_BULK &&
	    desc->bmAttributes != CH9_USB_EP_INTERRUPT) {

		cvidbg("%s: %s type mismatch\n", __func__, _ep->name);
		return -EINVAL;
	}

	/* hardware _could_ do smaller, but driver doesn't */
	if ((desc->bmAttributes == CH9_USB_EP_BULK &&
	     le16ToCpu(desc->wMaxPacketSize) >
	     ep_maxpacket(ep)) || !desc->wMaxPacketSize) {

		cvidbg("%s: bad %s maxpacket\n", __func__, _ep->name);
		return -ERANGE;
	}

	dev = ep->dev;
	if (!dev->driver || dev->gadget.speed == CH9_USB_SPEED_UNKNOWN) {

		cvidbg("%s: bogus device state\n", __func__);
		return -ESHUTDOWN;
	}

	ep->stopped = 0;
	ep->ep.desc = desc;
	ep->desc = desc;
	ep->pio_irqs = 0;
	ep->ep.maxpacket = le16ToCpu(desc->wMaxPacketSize);

	/* Reset halt state */
	cvi_udc_set_nak(ep);
	cvi_udc_set_halt(_ep, 0);

	cvi_udc_ep_activate(ep);

	cvidbg("%s: enabled %s, stopped = %d, maxpacket = %d\n",
		__func__, _ep->name, ep->stopped, ep->ep.maxpacket);

	return 0;
}

void cvi_hsotg_set_bit(uint32_t *reg, uint32_t val)
{
	cvi_uncached_write32(cvi_uncached_read32(reg) | val, reg);
}

void cvi_hsotg_clear_bit(uint32_t *reg, uint32_t val)
{
	cvi_uncached_write32(cvi_uncached_read32(reg) & ~val, reg);
}

int cvi_hsotg_wait_bit_set(uint32_t *reg, uint32_t bit, uint32_t timeout)
{
	uint32_t i;

	for (i = 0; i < timeout; i++) {
		if (cvi_uncached_read32(reg) & bit)
			return 0;
		udelay(1);
	}

	return -ETIMEDOUT;
}

static void cvi_ep_stop_xfer(struct cvi_udc *dev, struct cvi_ep *ep)
{
	uint32_t ep_num = ep_index(ep);
	uint32_t *epctrl_reg, *epint_reg;

	epctrl_reg = ep_is_in(ep) ? &reg->in_endp[ep_num].diepctl :
		     &reg->out_endp[ep_num].doepctl;
	epint_reg = ep_is_in(ep) ? &reg->in_endp[ep_num].diepint :
		    &reg->out_endp[ep_num].doepint;
	if (ep_is_in(ep)) {
		cvi_hsotg_set_bit(epctrl_reg, DEPCTL_SNAK);
		/* Wait for Nak effect */
		if (cvi_hsotg_wait_bit_set(epint_reg, INEPNAKEFF, 100))
			printf("%s: timeout DIEPINT.NAKEFF\n", __func__);
	} else {
		if (!(cvi_uncached_read32(&reg->gintsts) & INT_GOUTNakEff)) {
			cvi_hsotg_set_bit(&reg->dctl, SGOUTNAK);
		}
		/* Wait for global nak to take effect */
		if (cvi_hsotg_wait_bit_set(&reg->gintsts, INT_GOUTNakEff, 100)) {
			printf("%s: timeout GINTSTS.GOUTNAKEFF\n", __func__);
		}
	}

	/* disable ep */
	cvi_hsotg_set_bit(epctrl_reg, DEPCTL_SNAK | DEPCTL_EPDIS);
	/* Wait for ep to be disabled */
	if (cvi_hsotg_wait_bit_set(epint_reg, EPDISBLD, 100)) {
		printf("%s: timeout DOEPCTL.EPDisable\n", __func__);
	}
	/* Clear EPDISBLD interrupt */
	cvi_hsotg_set_bit(epint_reg, EPDISBLD);
	if (ep_is_in(ep)) {
		cvi_hsotg_txfifo_flush(dev, ep->fifo_num);
	} else {
		cvi_hsotg_set_bit(&reg->dctl, CGOUTNAK);
	}
}

static int _cvi_ep_disable(struct cvi_ep *ep)
{
	uint32_t ep_num = ep_index(ep);
	uint32_t *epctrl_reg;
	uint32_t ctrl;

	if (ep == &the_controller->ep[0]) {
		printf("%s: call for ep0-out\n", __func__);
		return -EINVAL;
	}

	epctrl_reg = ep_is_in(ep) ? &reg->in_endp[ep_num].diepctl :
		     &reg->out_endp[ep_num].doepctl;
	ctrl = cvi_uncached_read32(epctrl_reg);
	if (ctrl & DEPCTL_EPENA)
		cvi_ep_stop_xfer(the_controller, ep);
	ctrl &= ~DEPCTL_EPENA;
	ctrl &= ~DEPCTL_USBACTEP;
	ctrl |= DEPCTL_SNAK;
	cvi_uncached_write32(ctrl, epctrl_reg);
	/* Nuke all pending requests */
	kill_all_requests(the_controller, ep, -ESHUTDOWN);

	ep->desc = 0;
	ep->stopped = 1;

	return 0;
}

/*
 * Disable EP
 */
static int cvi_ep_disable(struct usb_ep *_ep)
{
	struct cvi_ep *ep;

	cvidbg("%s: %p\n", __func__, _ep);

	ep = container_of(_ep, struct cvi_ep, ep);

	if (!_ep || !ep->desc) {
		cvidbg("%s: %s not enabled\n", __func__,
			_ep ? ep->ep.name : NULL);
		return -EINVAL;
	}

	_cvi_ep_disable(ep);

	cvidbg("%s: disabled %s\n", __func__, _ep->name);

	return 0;
}

static struct usb_request *cvi_alloc_request(struct usb_ep *ep)
{
	struct cvi_udc *dev = the_controller;
	struct usb_gadget_driver *driver = dev->driver;
	struct cvi_request *req;

	cvidbg("%s: %s %p\n", __func__, ep->name, ep);

	req = driver->req_mem_alloc(&dev->gadget, sizeof(*req));
	if (!req)
		return 0;

	memset(req, 0, sizeof(*req));
	INIT_LIST_HEAD(&req->queue);

	return &req->req;
}

static void cvi_free_request(struct usb_ep *ep, struct usb_request *_req)
{
	struct cvi_udc *dev = the_controller;
	struct usb_gadget_driver *driver = dev->driver;
	struct cvi_request *req;

	cvidbg("%s: %p\n", __func__, ep);

	req = container_of(_req, struct cvi_request, req);
	if (!list_empty(&req->queue))
		printf("warning! free unfinished request!\n");
	driver->req_mem_free(&dev->gadget, req);
}

/* dequeue JUST ONE request */
static int cvi_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
	struct cvi_ep *ep;
	struct cvi_request *req;

	cvidbg("%s: %p\n", __func__, _ep);

	ep = container_of(_ep, struct cvi_ep, ep);
	if (!_ep || ep->ep.name == ep0name)
		return -EINVAL;

	/* make sure it's actually queued on this endpoint */
	list_for_each_entry(req, &ep->queue, queue) {
		if (&req->req == _req)
			break;
	}
	if (&req->req != _req) {
		return -EINVAL;
	}

	cvi_done(ep, req, -ECONNRESET);

	return 0;
}

/*
 * Return bytes in EP FIFO
 */
static int cvi_fifo_status(struct usb_ep *_ep)
{
	int count = 0;
	struct cvi_ep *ep;

	ep = container_of(_ep, struct cvi_ep, ep);
	if (!_ep) {
		cvidbg("%s: bad ep\n", __func__);
		return -ENODEV;
	}

	cvidbg("%s: %d\n", __func__, ep_index(ep));

	/* LPD can't report unclaimed bytes from IN fifos */
	if (ep_is_in(ep))
		return -EOPNOTSUPP;

	return count;
}

/*
 * Flush EP FIFO
 */
static void cvi_fifo_flush(struct usb_ep *_ep)
{
	struct cvi_ep *ep;

	ep = container_of(_ep, struct cvi_ep, ep);
	if (!_ep || (!ep->desc && ep->ep.name != ep0name)) {
		cvidbg("%s: bad ep\n", __func__);
		return;
	}

	cvidbg("%s: %d\n", __func__, ep_index(ep));
}

static int pullup(struct usb_gadget *gadget, int is_on)
{
	unsigned int temp = cvi_uncached_read32(&reg->dctl);

	if (!is_on) {
		temp |= SOFT_DISCONNECT;
		cvi_uncached_write32(temp, &reg->dctl);
	} else {
		temp &= ~SOFT_DISCONNECT;
		cvi_uncached_write32(temp, &reg->dctl);
	}

	return 0;
}

static int wakeup(struct usb_gadget *gadget)
{
	cvi_hsotg_set_bit(&reg->dctl, RMTWKUPSIG);
	mdelay(10);
	cvi_hsotg_clear_bit(&reg->dctl, RMTWKUPSIG);

	return 0;
}

static const struct usb_gadget_ops cvi_udc_ops = {
	/* current versions must always be self-powered */
	.pullup = pullup,
	.wakeup = wakeup,
};

uint8_t cvi_phy_to_log_ep(uint8_t phy_num, uint8_t dir)
{
	return (phy_num) ? ((phy_num << 1) - (!!dir)) : 0;
}

/*
 *	probe - binds to the platform device
 */

int cvi_udc_probe(struct cvi_plat_otg_data *pdata)
{
	struct cvi_udc *dev;
	int retval = 0;

	cvidbg("%s: %p\n", __func__, pdata);

	if (pdata->size < sizeof(*dev)) {
		printf("size for handler is too samll (%ld, %d)\n", sizeof(*dev), pdata->size);
		return -1;
	}

	cvi_log_write(0xbeefbeef, 0, 0, 0, 0);
	dev = (struct cvi_udc *)pdata->handler;
	memset(dev, 0, sizeof(*dev));

	dev->pdata = (void *)pdata;

	reg = (struct cvi_usbotg_reg *)pdata->regs_otg;

	/* gadget init */
	dev->usb_address = 0;
	dev->gadget.ops = &cvi_udc_ops;
	dev->gadget.ep0 = &dev->ep[0].ep;
	dev->gadget.name = driver_name;
	dev->gadget.is_dualspeed = 1;
	dev->gadget.is_otg = 0;
	dev->gadget.is_a_peripheral = 0;
	dev->gadget.b_hnp_enable = 0;
	dev->gadget.a_hnp_support = 0;
	dev->gadget.a_alt_hnp_support = 0;
	dev->gadget.max_speed = CH9_USB_SPEED_HIGH;
	/* eps init */
	dev->ep[0].ep.name = ep0name;
	dev->ep[0].ep.ops = &cvi_ep_ops;
	dev->ep[0].ep.maxpacket = EP0_FIFO_SIZE;
	dev->ep[0].dev = dev;
	dev->ep[0].bEndpointAddress = 0;
	dev->ep[0].bmAttributes = 0;
	dev->ep[0].ep_type = ep_control;
	dev->ep[1].ep.name = ep1name;
	dev->ep[1].ep.ops = &cvi_ep_ops;
	dev->ep[1].ep.maxpacket = EP_FIFO_SIZE;
	dev->ep[1].dev = dev;
	dev->ep[1].bEndpointAddress = USB_DIR_IN | 1;
	dev->ep[1].bmAttributes = CH9_USB_EP_BULK;
	dev->ep[1].ep_type = ep_bulk_out;
	dev->ep[1].fifo_num = 1;
	dev->ep[2].ep.name = ep2name;
	dev->ep[2].ep.ops = &cvi_ep_ops;
	dev->ep[2].ep.maxpacket = EP_FIFO_SIZE;
	dev->ep[2].dev = dev;
	dev->ep[2].bEndpointAddress = USB_DIR_OUT | 1;
	dev->ep[2].bmAttributes = CH9_USB_EP_BULK;
	dev->ep[2].ep_type = ep_bulk_in;
	dev->ep[2].fifo_num = 1;
	dev->ep[3].ep.name = ep3name;
	dev->ep[3].ep.ops = &cvi_ep_ops;
	dev->ep[3].ep.maxpacket = EP_FIFO_SIZE;
	dev->ep[3].dev = dev;
	dev->ep[3].bEndpointAddress = USB_DIR_IN | 2;
	dev->ep[3].bmAttributes = CH9_USB_EP_INTERRUPT;
	dev->ep[3].ep_type = ep_interrupt;
	dev->ep[3].fifo_num = 2;

	the_controller = dev;

	dev->usb_ctrl = (CH9_UsbSetup *)pdata->ctrl_req;
	if (!dev->usb_ctrl) {
		printf("No memory available for UDC!\n");
		return -ENOMEM;
	}

	dev->usb_ctrl_dma_addr = (dma_addr_t)((uintptr_t)dev->usb_ctrl);
	dev->reg = reg;

	udc_reinit(dev);

	return retval;
}

int cviusb_gadget_handle_interrupts(int index)
{
	uint32_t intr_status = cvi_uncached_read32(&reg->gintsts);
	uint32_t gintmsk = cvi_uncached_read32(&reg->gintmsk);

#ifdef DEBUG
	{
		static uint32_t print_cnt = 0xFFFF;

		if (print_cnt) {
			print_cnt--;
		} else {
			printf("still alive\n");
			print_cnt = 0xFFFF;
		}
	}
#endif
	if (intr_status & gintmsk)
		return cvi_udc_irq(1, (void *)the_controller);
	return 0;
}
