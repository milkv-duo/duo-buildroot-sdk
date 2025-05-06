/*
 * drivers/usb/gadget/dwc2_udc_otg_xfer_dma.c
 * Designware DWC2 on-chip full/high speed USB OTG 2.0 device controllers
 *
 * Copyright (C) 2009 for Samsung Electronics
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

#include <list.h>
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"
#include <dwc2_ch9.h>
#include <dwc2_drv_if.h>

#include <byteorder.h>
#include <dwc2_stdtypes.h>
#include <dwc2_errno.h>
#include "dwc2_udc_otg_regs.h"
#include "dwc2_udc_otg_priv.h"
#include <dwc2_udc.h>
#include <dps.h>

static unsigned int ep0_fifo_size = 64;
static unsigned int ep_fifo_size =  512;
static unsigned int ep_fifo_size2 = 1024;

static char *state_names[] = {
	"WAIT_FOR_SETUP",
	"DATA_STATE_XMIT",
	"DATA_STATE_NEED_ZLP",
	"WAIT_FOR_OUT_STATUS",
	"DATA_STATE_RECV",
	"WAIT_FOR_COMPLETE",
	"WAIT_FOR_OUT_COMPLETE",
	"WAIT_FOR_IN_COMPLETE",
	"WAIT_FOR_NULL_COMPLETE",
};

/* Bulk-Only Mass Storage Reset (class-specific request) */
#define GET_MAX_LUN_REQUEST	0xFE
#define BOT_RESET_REQUEST	0xFF

static void set_max_pktsize(struct dwc2_udc *dev, CH9_UsbSpeed speed)
{
	unsigned int ep_ctrl;
	struct dwc2_usbotg_reg *reg = dev->reg;
	int i;

	if (speed == CH9_USB_SPEED_HIGH) {
		ep0_fifo_size = 64;
		ep_fifo_size = 512;
		ep_fifo_size2 = 1024;
		dev->gadget.speed = CH9_USB_SPEED_HIGH;
	} else {
		ep0_fifo_size = 64;
		ep_fifo_size = 64;
		ep_fifo_size2 = 64;
		dev->gadget.speed = CH9_USB_SPEED_FULL;
	}

	dev->ep[0].ep.maxpacket = ep0_fifo_size;
	for (i = 1; i < DWC2_MAX_ENDPOINTS; i++)
		dev->ep[i].ep.maxpacket = ep_fifo_size;

	/* EP0 - Control IN (64 bytes)*/
	ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl);
	DWC2_UncachedWrite32(ep_ctrl & ~(3<<0), &reg->in_endp[EP0_CON].diepctl);

	/* EP0 - Control OUT (64 bytes)*/
	ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl);
	DWC2_UncachedWrite32(ep_ctrl & ~(3<<0), &reg->out_endp[EP0_CON].doepctl);
}

static inline void dwc2_udc_ep0_zlp(struct dwc2_udc *dev)
{
	uint32_t ep_ctrl;
	struct dwc2_usbotg_reg *reg = dev->reg;

	DWC2_UncachedWrite32(dev->usb_ctrl_dma_addr, &reg->in_endp[EP0_CON].diepdma);
	DWC2_UncachedWrite32(DIEPT_SIZ_PKT_CNT(1), &reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl);
	ep_ctrl &= ~(1 << 30);
	ep_ctrl |= (DEPCTL_EPENA|DEPCTL_CNAK);
	DWC2_UncachedWrite32(ep_ctrl,
	       &reg->in_endp[EP0_CON].diepctl);

	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl));
	dev->ep0state = WAIT_FOR_IN_COMPLETE;
	dwc2_log_write(0xAAAAA, ep_ctrl, dev->usb_ctrl_dma_addr,
				DIEPT_SIZ_PKT_CNT(1), 0);
}

void dwc2_udc_pre_setup(struct dwc2_udc *dev)
{
	//uint32_t ep_ctrl, tmp, tmp1;
	struct dwc2_usbotg_reg *reg = dev->reg;

	dwc2dbg_cond(DEBUG_SETUP,
		   "%s : Prepare Setup packets.\n", __func__);

	DWC2_UncachedWrite32(DOEPT_SIZ_SUS_CNT(1) | DOEPT_SIZ_PKT_CNT(1) | sizeof(CH9_UsbSetup),
	       &reg->out_endp[EP0_CON].doeptsiz);

#ifdef DWC2_DMA_EN
	DWC2_UncachedWrite32(dev->usb_ctrl_dma_addr, &reg->out_endp[EP0_CON].doepdma);

	tmp = DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl);
	ep_ctrl = DEPCTL_EPENA | DEPCTL_USBACTEP;
	// tmp = ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl);
	// ep_ctrl &= ~(1 << 30);
	// ep_ctrl |= DEPCTL_EPENA;

	DWC2_UncachedWrite32(ep_ctrl, &reg->out_endp[EP0_CON].doepctl);
#endif

	//tmp1 = DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl);
	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl));
	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl));
	//dwc2_log_write(0x99999, ep_ctrl, dev->usb_ctrl_dma_addr,
	//			tmp, tmp1);

}

static inline void dwc2_ep0_complete_out(struct dwc2_udc *dev)
{
	uint32_t ep_ctrl;
	struct dwc2_usbotg_reg *reg = dev->reg;

	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl));
	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl));

	dwc2dbg_cond(DEBUG_OUT_EP,
		"%s : Prepare Complete Out packet.\n", __func__);

	//dwc2dbg_cond(1,
	//	   "CO\n");
	DWC2_UncachedWrite32(DOEPT_SIZ_PKT_CNT(1) | sizeof(CH9_UsbSetup),
	       &reg->out_endp[EP0_CON].doeptsiz);
	DWC2_UncachedWrite32(dev->usb_ctrl_dma_addr, &reg->out_endp[EP0_CON].doepdma);

	ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl);
	ep_ctrl &= ~(1 << 30);
	ep_ctrl |= (DEPCTL_EPENA | DEPCTL_CNAK);
	DWC2_UncachedWrite32(ep_ctrl,
	       &reg->out_endp[EP0_CON].doepctl);

	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl));
	dwc2dbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		__func__, DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl));
	dwc2_log_write(0x88888, ep_ctrl, dev->usb_ctrl_dma_addr, DOEPT_SIZ_PKT_CNT(1) | sizeof(CH9_UsbSetup), 0);
}

static int setdma_rx(struct dwc2_ep *ep, struct dwc2_request *req)
{
	uint32_t *buf, ctrl;
	uint32_t length, pktcnt;
	uint32_t ep_num = ep_index(ep);
	uint32_t dma_addr;
	struct dwc2_usbotg_reg *reg = ep->dev->reg;

	buf = req->req.buf + req->req.actual;
	length = min_t(uint32_t, req->req.length - req->req.actual,
		       ep_num ? DMA_BUFFER_SIZE : ep->ep.maxpacket);

	ep->len = length;
	ep->dma_buf = buf;

	if (ep_num == EP0_CON || length == 0)
		pktcnt = 1;
	else
		pktcnt = (length - 1)/(ep->ep.maxpacket) + 1;

	ctrl =  DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl);

	dma_addr = (uint32_t)((uintptr_t)ep->dma_buf);
	// DWC2_CacheInvalidate(dma_addr, ep->len);

#ifdef DWC2_DMA_EN
	DWC2_UncachedWrite32(dma_addr, &reg->out_endp[ep_num].doepdma);
	DWC2_UncachedWrite32(DOEPT_SIZ_PKT_CNT(pktcnt) | DOEPT_SIZ_XFER_SIZE(length),
	       &reg->out_endp[ep_num].doeptsiz);
	ctrl &= ~(1 << 30);
	ctrl |= (DEPCTL_EPENA|DEPCTL_CNAK);
	DWC2_UncachedWrite32(ctrl, &reg->out_endp[ep_num].doepctl);
#else
	ctrl |= (DEPCTL_EPENA|DEPCTL_CNAK);
	DWC2_UncachedWrite32(ctrl, &reg->out_endp[ep_num].doepctl);
#endif

	dwc2dbg_cond(DEBUG_OUT_EP != 0,
			"%s: EP%d RX DMA start : DOEPDMA = 0x%x, DOEPTSIZ = 0x%x, DOEPCTL = 0x%x buf = 0x%p, pktcnt = %d, xfersize = %d\n",
			__func__, ep_num,
			DWC2_UncachedRead32(&reg->out_endp[ep_num].doepdma),
			DWC2_UncachedRead32(&reg->out_endp[ep_num].doeptsiz),
			DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl),
			buf, pktcnt, length);
	dwc2_log_write(0x77777, ctrl, dma_addr, DOEPT_SIZ_PKT_CNT(pktcnt) | DOEPT_SIZ_XFER_SIZE(length), 0);
	return 0;

}

void dwc2_ep_fifo_write(struct dwc2_udc *dev, uint8_t ep_idx, uint8_t *src, uint16_t len)
{
	struct dwc2_usbotg_reg *reg = dev->reg;
	uint32_t *pSrc = (uint32_t *)src;
	uint32_t count32b, i;

	count32b = ((uint32_t)len + 3U) / 4U;
	for (i = 0U; i < count32b; i++) {
		DWC2_UncachedWrite32(*pSrc, &reg->ep[ep_idx].fifo);
		pSrc++;
	}
}

static int setdma_tx(struct dwc2_ep *ep, struct dwc2_request *req)
{
	uint32_t *buf;
	uint32_t ctrl = 0;
	uint32_t tmp, tmp1;
	uint32_t length, pktcnt;
	uint32_t ep_num = ep_index(ep);
	uint32_t dma_addr;
	struct dwc2_usbotg_reg *reg = ep->dev->reg;

	buf = req->req.buf + req->req.actual;
	length = req->req.length - req->req.actual;
	if (length > DMA_BUFFER_SIZE)
		length = DMA_BUFFER_SIZE;

	if (ep_num == EP0_CON)
		length = min_t(uint32_t, length, ep_maxpacket(ep));

	ep->len = length;
	ep->dma_buf = buf;

	dma_addr = (uint32_t)((uintptr_t)ep->dma_buf);
	// DWC2_CacheFlush(dma_addr, ROUND(ep->len, CONFIG_SYS_CACHELINE_SIZE));
	// flush_dcache_range(dma_addr, ROUND(ep->len, CONFIG_SYS_CACHELINE_SIZE));

	if (length == 0)
		pktcnt = 1;
	else
		pktcnt = (length - 1)/(ep->ep.maxpacket) + 1;

	/* Flush the endpoint's Tx FIFO */
	//DWC2_UncachedWrite32(TX_FIFO_NUMBER(ep->fifo_num), &reg->grstctl);
	//DWC2_UncachedWrite32(TX_FIFO_NUMBER(ep->fifo_num) | TX_FIFO_FLUSH, &reg->grstctl);
	//while (DWC2_UncachedRead32(&reg->grstctl) & TX_FIFO_FLUSH)
	//	;
#ifdef DWC2_DMA_EN
	DWC2_UncachedWrite32(dma_addr, &reg->in_endp[ep_num].diepdma);
#endif
	DWC2_UncachedWrite32(DIEPT_SIZ_PKT_CNT(pktcnt) | DIEPT_SIZ_XFER_SIZE(length),
	       &reg->in_endp[ep_num].dieptsiz);

	tmp = ctrl = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl);

	/* Write the FIFO number to be used for this endpoint */
	ctrl &= DIEPCTL_TX_FIFO_NUM_MASK;
	ctrl &= ~(1 << 30);
	ctrl |= DIEPCTL_TX_FIFO_NUM(ep->fifo_num);

	/* Clear reserved (Next EP) bits */
	ctrl = (ctrl&~(EP_MASK<<DEPCTL_NEXT_EP_BIT));
	ctrl |= (DEPCTL_EPENA|DEPCTL_CNAK);
	//tf_printf("diepctl0 = 0x%x\n", ctrl);
	DWC2_UncachedWrite32(ctrl, &reg->in_endp[ep_num].diepctl);

#ifndef DWC2_DMA_EN
	dwc2_ep_fifo_write(ep->dev, ep_num, (uint8_t *)buf, length);
	if (length > 0) {
		DWC2_UncachedWrite32(1UL << (ep_num & 0x0f), &reg->diepempmsk);
	}
#endif

	tmp1 = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl);
	dwc2dbg_cond(DEBUG_IN_EP,
		"%s:EP%d TX DMA start : DIEPDMA0 = 0x%x, DIEPTSIZ0 = 0x%x, DIEPCTL0 = 0x%x buf = 0x%p, pktcnt = %d, xfersize = %d\n",
		__func__, ep_num,
		DWC2_UncachedRead32(&reg->in_endp[ep_num].diepdma),
		DWC2_UncachedRead32(&reg->in_endp[ep_num].dieptsiz),
		DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl),
		buf, pktcnt, length);
	//dwc2dbg_cond(1, "TD\n");
	dwc2_log_write(0x44444, ctrl, dma_addr, tmp, tmp1);
	dwc2_log_write(0x46464, DWC2_UncachedRead32(&reg->in_endp[ep_num].dieptsiz), length, ep_num, 0);

	return length;
}

static void complete_rx(struct dwc2_udc *dev, uint8_t ep_num)
{
	struct dwc2_ep *ep = &dev->ep[dwc2_phy_to_log_ep(ep_num, 0)];
	struct dwc2_request *req = NULL;
	uint32_t ep_tsr = 0, xfer_size = 0, is_short = 0;
	// uint32_t dma_addr;
	struct dwc2_usbotg_reg *reg = dev->reg;

	if (list_empty(&ep->queue)) {
		dwc2dbg_cond(DEBUG_OUT_EP != 0,
			   "%s: RX DMA done : NULL REQ on OUT EP-%d\n",
			   __func__, ep_num);
		return;

	}

	req = list_entry(ep->queue.next, struct dwc2_request, queue);
	ep_tsr = DWC2_UncachedRead32(&reg->out_endp[ep_num].doeptsiz);

	if (ep_num == EP0_CON)
		xfer_size = (ep_tsr & DOEPT_SIZ_XFER_SIZE_MAX_EP0);
	else

		// xfer_size = (ep_tsr & DOEPT_SIZ_XFER_SIZE_MAX_EP);
		xfer_size = req->req.rxfifo_cnt;

	xfer_size = ep->len - xfer_size;

	/*
	 * NOTE:
	 *
	 * Please be careful with proper buffer allocation for USB request,
	 * which needs to be aligned to CONFIG_SYS_CACHELINE_SIZE, not only
	 * with starting address, but also its size shall be a cache line
	 * multiplication.
	 *
	 * This will prevent from corruption of data allocated immediatelly
	 * before or after the buffer.
	 *
	 * For armv7, the cache_v7.c provides proper code to emit "ERROR"
	 * message to warn users.
	 */
	// dma_addr = (uint32_t)((uintptr_t)ep->dma_buf);
	// DWC2_CacheInvalidate(dma_addr, ROUND(xfer_size, CONFIG_SYS_CACHELINE_SIZE));

	req->req.actual += min(xfer_size, req->req.length - req->req.actual);
	is_short = !!(xfer_size % ep->ep.maxpacket);

	dwc2dbg_cond(DEBUG_OUT_EP != 0,
		   "%s: RX DMA done : ep = %d, rx bytes = %d/%d, is_short = %d, DOEPTSIZ = 0x%x, remained bytes = %d\n",
		   __func__, ep_num, req->req.actual, req->req.length,
		   is_short, ep_tsr, req->req.length - req->req.actual);

	if (is_short || req->req.actual == req->req.length) {
		if (ep_num == EP0_CON && dev->ep0state == DATA_STATE_RECV) {
			dwc2dbg_cond(DEBUG_OUT_EP != 0, "	=> Send ZLP\n");
			dwc2_udc_ep0_zlp(dev);
			/* packet will be completed in complete_tx() */
			dev->ep0state = WAIT_FOR_IN_COMPLETE;
		} else {
			dwc2_done(ep, req, 0);

			if (!list_empty(&ep->queue)) {
				req = list_entry(ep->queue.next,
					struct dwc2_request, queue);
				dwc2dbg_cond(DEBUG_OUT_EP != 0,
					   "%s: Next Rx request start...\n",
					   __func__);
				setdma_rx(ep, req);
			}
		}
	} else
		setdma_rx(ep, req);
}

static void complete_tx(struct dwc2_udc *dev, uint8_t ep_num)
{
	struct dwc2_ep *ep = &dev->ep[dwc2_phy_to_log_ep(ep_num, 1)];
	struct dwc2_request *req;
	//uint32_t ep_tsr = 0, xfer_size = 0, is_short = 0;
	uint32_t xfer_size = 0;
	uint32_t last;
	//struct dwc2_usbotg_reg *reg = dev->reg;

	dwc2_log_write(0x55555, dev->ep0state, list_empty(&ep->queue), 0, 0);

	if (dev->ep0state == WAIT_FOR_NULL_COMPLETE) {
		dev->ep0state = WAIT_FOR_OUT_COMPLETE;
		dwc2_ep0_complete_out(dev);
		return;
	}

	if (list_empty(&ep->queue)) {
		dwc2dbg_cond(DEBUG_IN_EP,
			"%s: TX DMA done : NULL REQ on IN EP-%d\n",
			__func__, ep_num);
		return;

	}

	req = list_entry(ep->queue.next, struct dwc2_request, queue);

	xfer_size = ep->len;

	req->req.actual += min(xfer_size, req->req.length - req->req.actual);

	// ep_tsr = DWC2_UncachedRead32(&reg->in_endp[ep_num].dieptsiz);

	// is_short = (xfer_size < ep->ep.maxpacket);
	// dwc2dbg_cond(DEBUG_IN_EP,
	// "%s: TX DMA done : ep = %d, tx bytes = %d/%d, "
	// "is_short = %d, DIEPTSIZ = 0x%x, remained bytes = %d\n",
	// __func__, ep_num, req->req.actual, req->req.length,
	// is_short, ep_tsr, req->req.length - req->req.actual);

	//dwc2dbg_cond(1,
	//	   "TC\n");
	if (ep_num == 0) {
		if (dev->ep0state == DATA_STATE_XMIT) {
			dwc2dbg_cond(DEBUG_IN_EP, "%s: ep_num = %d, ep0stat == DATA_STATE_XMIT\n",
				__func__, ep_num);
			last = dwc2_write_fifo_ep0(ep, req);
			if (last)
				dev->ep0state = WAIT_FOR_COMPLETE;
		} else if (dev->ep0state == WAIT_FOR_IN_COMPLETE) {
			dwc2dbg_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, completing request\n",
				__func__, ep_num);
			dwc2_done(ep, req, 0);
			dev->ep0state = WAIT_FOR_SETUP;
		} else if (dev->ep0state == WAIT_FOR_COMPLETE) {
			dwc2dbg_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, completing request\n",
				__func__, ep_num);
			dwc2_done(ep, req, 0);
			dev->ep0state = WAIT_FOR_OUT_COMPLETE;
			dwc2_ep0_complete_out(dev);
		} else {
			dwc2dbg_cond(DEBUG_IN_EP,
				"%s: ep_num = %d, invalid ep state\n",
				__func__, ep_num);
		}
		return;
	}

	if (req->req.actual == req->req.length)
		dwc2_done(ep, req, 0);

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct dwc2_request, queue);
		dwc2dbg_cond(DEBUG_IN_EP,
			"%s: Next Tx request start...\n", __func__);
		setdma_tx(ep, req);
	}
}

static inline void dwc2_udc_check_tx_queue(struct dwc2_udc *dev, uint8_t ep_num)
{
	struct dwc2_ep *ep = &dev->ep[dwc2_phy_to_log_ep(ep_num, 1)];
	struct dwc2_request *req;

	dwc2dbg_cond(DEBUG_IN_EP,
		"%s: Check queue, ep_num = %d\n", __func__, ep_num);

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct dwc2_request, queue);
		dwc2dbg_cond(DEBUG_IN_EP,
			"%s: Next Tx request(0x%p) start...\n",
			__func__, req);

		if (ep_is_in(ep))
			setdma_tx(ep, req);
		else
			setdma_rx(ep, req);
	} else {
		dwc2dbg_cond(DEBUG_IN_EP,
			"%s: NULL REQ on IN EP-%d\n", __func__, ep_num);

		return;
	}

}

static int dwc2_set_test_mode(struct dwc2_udc *dev, int testmode)
{
	struct dwc2_usbotg_reg *reg = dev->reg;
	uint32_t dctl = DWC2_UncachedRead32(&reg->dctl);

	dctl &= ~DCTL_TSTCTL_MASK;
	switch (testmode) {
	case CH9_TEST_J:
	case CH9_TEST_K:
	case CH9_TEST_SE0_NAK:
	case CH9_TEST_PACKET:
	case CH9_TEST_FORCE_EN:
		tf_printf("run test mode %d\n", testmode);
		dctl |= testmode << DCTL_TSTCTL_SHIFT;
		break;
	default:
		return -EINVAL;
	}
	DWC2_UncachedWrite32(dctl, &reg->dctl);

	return 0;
}


static void process_ep_in_intr(struct dwc2_udc *dev)
{
	uint32_t ep_intr, ep_intr_status;
	uint8_t ep_num = 0;
	struct dwc2_usbotg_reg *reg = dev->reg;

	ep_intr = DWC2_UncachedRead32(&reg->daint);
	dwc2dbg_cond(DEBUG_IN_EP,
		"*** %s: EP In interrupt : DAINT = 0x%x\n", __func__, ep_intr);

	ep_intr &= DAINT_MASK;

	while (ep_intr) {
		if (ep_intr & DAINT_IN_EP_INT(1)) {
			ep_intr_status = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepint);
			dwc2dbg_cond(DEBUG_IN_EP,
				   "\tEP%d-IN : DIEPINT = 0x%x\n",
				   ep_num, ep_intr_status);
			dwc2_log_write(0x737373, ep_num, ep_intr_status,
					DWC2_UncachedRead32(&reg->in_endp[ep_num].dieptsiz),
					DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl));
			dwc2_log_write(0x747474,
					DWC2_UncachedRead32(&reg->in_endp[ep_num].diepdma),
					DWC2_UncachedRead32(&reg->in_endp[ep_num].dtxfsts),
					0, 0);

			/* Interrupt Clear */
			DWC2_UncachedWrite32(ep_intr_status, &reg->in_endp[ep_num].diepint);

			if (ep_intr_status & TRANSFER_DONE) {
				complete_tx(dev, ep_num);

				if (ep_num == 0) {
					if (dev->test_mode) {
						dwc2_set_test_mode(dev, dev->test_mode);
					}
					if (dev->ep0state ==
					    WAIT_FOR_IN_COMPLETE)
						dev->ep0state = WAIT_FOR_SETUP;

					if (dev->ep0state == WAIT_FOR_SETUP)
						dwc2_udc_pre_setup(dev);

					/* continue transfer after set_clear_halt for DMA mode */
					if (dev->clear_feature_flag == 1) {
						dwc2_udc_check_tx_queue(dev,
							dev->clear_feature_num);
						dev->clear_feature_flag = 0;
					}
				}
			}

			// if ((ep_num == 0) && (ep_intr_status & CTRL_IN_EP_TIMEOUT)) {
			// struct dwc2_ep *ep = &dev->ep[dwc2_phy_to_log_ep(ep_num, 1)];

			// dwc2_nuke(ep, -EIO);
			// }

		}
		ep_num++;
		ep_intr >>= 1;
	}
}

static void process_ep_out_intr(struct dwc2_udc *dev)
{
	uint32_t ep_intr, ep_intr_status;
	uint8_t ep_num = 0;
	struct dwc2_usbotg_reg *reg = dev->reg;

	ep_intr = DWC2_UncachedRead32(&reg->daint);
	dwc2dbg_cond(DEBUG_OUT_EP != 0,
		   "*** %s: EP OUT interrupt : DAINT = 0x%x\n",
		   __func__, ep_intr);

	ep_intr = (ep_intr >> DAINT_OUT_BIT) & DAINT_MASK;

	while (ep_intr) {
		if (ep_intr & 0x1) {
			ep_intr_status = DWC2_UncachedRead32(&reg->out_endp[ep_num].doepint);
			dwc2dbg_cond(DEBUG_OUT_EP != 0,
				   "\tEP%d-OUT : DOEPINT = 0x%x\n",
				   ep_num, ep_intr_status);

			/* Interrupt Clear */
			DWC2_UncachedWrite32(ep_intr_status, &reg->out_endp[ep_num].doepint);

			if (ep_num == 0) {
				if (ep_intr_status &
				    CTRL_OUT_EP_SETUP_PHASE_DONE) {
					dwc2dbg_cond(DEBUG_OUT_EP != 0,
						   "SETUP packet arrived\n");
					//dwc2dbg_cond(1, "SP\n");
					dwc2_log_write(0x3333, 0, 0, 0, 0);
					dwc2_handle_ep0(dev);
				} else if (ep_intr_status & TRANSFER_DONE) {
					if (dev->ep0state !=
					    WAIT_FOR_OUT_COMPLETE) {
						dwc2_log_write(0x66666, 1, ep_intr_status, 0, 0);
						complete_rx(dev, ep_num);
					} else {
						dwc2_log_write(0x66666, 2, ep_intr_status, 0, 0);
						dev->ep0state = WAIT_FOR_SETUP;
						dwc2_udc_pre_setup(dev);
					}
				}
			} else {
				if (ep_intr_status & TRANSFER_DONE)
					complete_rx(dev, ep_num);
			}
		}
		ep_num++;
		ep_intr >>= 1;
	}
}

/*
 *	usb client interrupt handler.
 */
void dwc2_ep_fifo_read(struct dwc2_udc *dev, uint8_t ep_idx, uint8_t *dest, uint16_t len)
{
	struct dwc2_usbotg_reg *reg = dev->reg;
	uint32_t *pDest = (uint32_t *)dest;
	uint32_t i;
	uint32_t count32b = ((uint32_t)len + 3U) / 4U;

	for (i = 0U; i < count32b; i++) {
		*pDest = DWC2_UncachedRead32(&reg->ep[ep_idx].fifo);
		pDest++;
	}
}

int dwc2_udc_irq(int irq, void *_dev)
{
	struct dwc2_udc *dev = _dev;
	uint32_t intr_status;
	uint32_t usb_status, gintmsk;
	struct dwc2_usbotg_reg *reg = dev->reg;

	intr_status = DWC2_UncachedRead32(&reg->gintsts);
	gintmsk = DWC2_UncachedRead32(&reg->gintmsk);

	dwc2dbg_cond(DEBUG_ISR,
		  "\n*** %s : GINTSTS=0x%x(on state %s), GINTMSK : 0x%x, DAINT : 0x%x, DAINTMSK : 0x%x\n",
		  __func__, intr_status, state_names[dev->ep0state], gintmsk,
		  DWC2_UncachedRead32(&reg->daint), DWC2_UncachedRead32(&reg->daintmsk));

	if (!intr_status) {
		return 0;
	}

	dwc2_log_write(0xEFEF, intr_status, dev->ep0state,
			DWC2_UncachedRead32(&reg->daint), 0);

#ifndef DWC2_DMA_EN //disable DMA
	if (intr_status & INT_RX_FIFO_NOT_EMPTY) {
		uint32_t read_count, ep_idx, tmp;
		struct dwc2_ep *ep;
		struct dwc2_request *req;

		dwc2dbg_cond(DEBUG_ISR, "\tRx fifo level interrupt\n");

		tmp = gintmsk & ~INT_RX_FIFO_NOT_EMPTY;
		DWC2_UncachedWrite32(tmp, &reg->gintmsk);

		tmp = DWC2_UncachedRead32(&reg->grxstsp);
		ep_idx = tmp & GRXSTSP_EPNUM_MASK;

		ep = &dev->ep[dwc2_phy_to_log_ep(ep_idx, 0)];
		req = list_entry(ep->queue.next, struct dwc2_request, queue);
		if ((tmp & GRXSTSP_PKTSTS_MASK) == OUT_PKT_RECEIVED) {
			read_count = (tmp & GRXSTSP_BCNT_MASK) >> 4;
			if (read_count != 0) {
				dwc2_ep_fifo_read(dev, ep_idx, req->req.buf, read_count);
				// req->req.actual += read_count;
				req->req.rxfifo_cnt = read_count;
			}
		} else if ((tmp & GRXSTSP_PKTSTS_MASK) == SETUP_PKT_RECEIVED) {
			read_count = (tmp & GRXSTSP_BCNT_MASK) >> 4;
			dwc2_ep_fifo_read(dev, ep_idx, (uint8_t *)dev->usb_ctrl, read_count);
		} else {
			/* ... */
		}

		tmp = gintmsk | INT_RX_FIFO_NOT_EMPTY;
		DWC2_UncachedWrite32(tmp, &reg->gintmsk);
	}
#endif

	if (intr_status & INT_ENUMDONE) {
		dwc2dbg_cond(DEBUG_ISR, "\tSpeed Detection interrupt\n");

		DWC2_UncachedWrite32(INT_ENUMDONE, &reg->gintsts);
		usb_status = (DWC2_UncachedRead32(&reg->dsts) & 0x6);

		if (usb_status & (USB_FULL_30_60MHZ | USB_FULL_48MHZ)) {
			dwc2dbg_cond(DEBUG_ISR,
				   "\t\tFull Speed Detection\n");
			set_max_pktsize(dev, CH9_USB_SPEED_FULL);
			dwc2_log_write(0x2222, 2, 0, 0, 0);

		} else {
			dwc2dbg_cond(DEBUG_ISR,
				"\t\tHigh Speed Detection : 0x%x\n",
				usb_status);
			//dwc2dbg_cond(1, "HS\n");
			dwc2_log_write(0x2222, 1, 0, 0, 0);
			set_max_pktsize(dev, CH9_USB_SPEED_HIGH);
		}
	}

	if (intr_status & INT_EARLY_SUSPEND) {
		dwc2dbg_cond(DEBUG_ISR, "\tEarly suspend interrupt\n");
		DWC2_UncachedWrite32(INT_EARLY_SUSPEND, &reg->gintsts);
	}

	if (intr_status & INT_SUSPEND) {
		usb_status = DWC2_UncachedRead32(&reg->dsts);
		dwc2dbg_cond(DEBUG_ISR,
			"\tSuspend interrupt :(DSTS):0x%x\n", usb_status);
		DWC2_UncachedWrite32(INT_SUSPEND, &reg->gintsts);
		if (dev->gadget.speed != CH9_USB_SPEED_UNKNOWN && dev->driver) {
			if (dev->driver->suspend)
				dev->driver->suspend(&dev->gadget);
		}
	}

	if (intr_status & INT_RESUME) {
		dwc2dbg_cond(DEBUG_ISR, "\tResume interrupt\n");
		DWC2_UncachedWrite32(INT_RESUME, &reg->gintsts);

		if (dev->gadget.speed != CH9_USB_SPEED_UNKNOWN
			&& dev->driver
			&& dev->driver->resume)	{
			dev->driver->resume(&dev->gadget);
		}
	}

	if ((intr_status & INT_RESET) || (intr_status & INT_RESETDET)) {
		usb_status = DWC2_UncachedRead32(&reg->gotgctl);
		dwc2dbg_cond(DEBUG_ISR,
			"\tReset interrupt - (GOTGCTL):0x%x\n", usb_status);
		//dwc2dbg_cond(1,	"R\n");
		dwc2_log_write(0x1111, usb_status, 0, 0, 0);
		if (intr_status & INT_RESET)
			DWC2_UncachedWrite32(INT_RESET, &reg->gintsts);
		if (intr_status & INT_RESETDET)
			DWC2_UncachedWrite32(INT_RESETDET, &reg->gintsts);

		if ((usb_status & 0xc0000) == (0x3 << 18)) {
			unsigned int connected = dev->connected;

			dwc2dbg_cond(DEBUG_ISR,
				"\t\tOTG core got reset!!\n");
			dwc2_disconnect(dev);
			if (connected)
				dwc2_reconfig_usbd(dev, 1);
			dev->ep0state = WAIT_FOR_SETUP;
		} else {
			dwc2dbg_cond(DEBUG_ISR,
				   "\t\tRESET handling skipped\n");
		}
	}

	if (intr_status & INT_IN_EP)
		process_ep_in_intr(dev);

	if (intr_status & INT_OUT_EP)
		process_ep_out_intr(dev);

	return 0;
}

/** Queue one request
 *  Kickstart transfer if needed
 */
int dwc2_queue(struct usb_ep *_ep, struct usb_request *_req)
{
	struct dwc2_request *req;
	struct dwc2_ep *ep;
	struct dwc2_udc *dev;
	uint32_t ep_num, gintsts;
	struct dwc2_usbotg_reg *reg;

	req = container_of(_req, struct dwc2_request, req);
	if (!_req || !_req->complete || !_req->buf
		     || !list_empty(&req->queue)) {

		dwc2dbg("%s: bad params\n", __func__);
		return -EINVAL;
	}

	ep = container_of(_ep, struct dwc2_ep, ep);

	if (!_ep || (!ep->desc && ep->ep.name != dwc2_get_ep0_name())) {

		dwc2dbg("%s: bad ep: %s, %d, %p\n", __func__,
		      ep->ep.name, !ep->desc, _ep);
		return -EINVAL;
	}

	ep_num = ep_index(ep);
	dev = ep->dev;
	if (!dev->driver || dev->gadget.speed == CH9_USB_SPEED_UNKNOWN) {

		dwc2dbg("%s: bogus device state %p\n", __func__, dev->driver);
		return -ESHUTDOWN;
	}
	reg = dev->reg;

	_req->status = -EINPROGRESS;
	_req->actual = 0;

	/* kickstart this i/o queue? */
	dwc2dbg("\n*** %s: %s-%s req = %p, len = %d, buf = %p Q empty = %d, stopped = %d\n",
			__func__, _ep->name, ep_is_in(ep) ? "in" : "out",
			_req, _req->length, _req->buf,
			list_empty(&ep->queue), ep->stopped);

#ifdef DWC2_DBG
	{
		int i, len = _req->length;

		tf_printf("pkt = ");
		if (len > 64)
			len = 64;
		for (i = 0; i < len; i++) {
			tf_printf("%02x", ((uint8_t *)_req->buf)[i]);
			if ((i & 7) == 7)
				tf_printf(" ");
		}
		tf_printf("\n");
	}
#endif

	if (list_empty(&ep->queue) && !ep->stopped) {

		if (ep_num == 0) {
			/* EP0 */
			list_add_tail(&req->queue, &ep->queue);
			dwc2_ep0_kick(dev, ep);
			req = 0;

		} else if (ep_is_in(ep)) {
			gintsts = DWC2_UncachedRead32(&reg->gintsts);
			dwc2dbg_cond(DEBUG_IN_EP,
				   "%s: ep_is_in, DWC2_UDC_OTG_GINTSTS=0x%x\n",
				   __func__, gintsts);

			setdma_tx(ep, req);
		} else {
			gintsts = DWC2_UncachedRead32(&reg->gintsts);
			dwc2dbg_cond(DEBUG_OUT_EP != 0,
				   "%s:ep_is_out, DWC2_UDC_OTG_GINTSTS=0x%x\n",
				   __func__, gintsts);

			setdma_rx(ep, req);
		}
	}

	/* pio or dma irq handler advances the queue. */
	if (req != 0)
		list_add_tail(&req->queue, &ep->queue);

	return 0;
}

/****************************************************************/
/* End Point 0 related functions                                */
/****************************************************************/

/* return:  0 = still running, 1 = completed, negative = errno */
int dwc2_write_fifo_ep0(struct dwc2_ep *ep, struct dwc2_request *req)
{
	uint32_t max;
	unsigned int count;
	int is_last;

	max = ep_maxpacket(ep);

	dwc2dbg_cond(DEBUG_EP0 != 0, "%s: max = %d\n", __func__, max);

	count = setdma_tx(ep, req);

	/* last packet is usually short (or a zlp) */
	if (count != max)
		is_last = 1;
	else {
		if ((req->req.length != req->req.actual + count)
		    || req->req.zero)
			is_last = 0;
		else
			is_last = 1;
	}

	dwc2dbg_cond(DEBUG_EP0 != 0,
		   "%s: wrote %s %d bytes%s %d left %p\n", __func__,
		   ep->ep.name, count,
		   is_last ? "/L" : "",
		   req->req.length - req->req.actual - count, req);

	/* requests complete when all IN data is in the FIFO */
	if (is_last) {
		ep->dev->ep0state = WAIT_FOR_SETUP;
		return 1;
	}

	return 0;
}

static int dwc2_fifo_read(struct dwc2_ep *ep, uintptr_t *cp, int max)
{
	// DWC2_CacheInvalidate((uintptr_t)cp, ROUND(max, CONFIG_SYS_CACHELINE_SIZE));
	// inv_dcache_range((unsigned long)cp, ROUND(max, CONFIG_SYS_CACHELINE_SIZE));

	dwc2dbg_cond(DEBUG_EP0 != 0,
		   "%s: bytes=%d, ep_index=%d 0x%p\n", __func__,
		   max, ep_index(ep), cp);

	return max;
}

/**
 * dwc2_set_address - set the USB address for this device
 * @address:
 *
 * Called from control endpoint function
 * after it decodes a set address setup packet.
 */
void dwc2_set_address(struct dwc2_udc *dev, unsigned char address)
{
	struct dwc2_usbotg_reg *reg = dev->reg;

	uint32_t ctrl = DWC2_UncachedRead32(&reg->dcfg);

	DWC2_UncachedWrite32(DEVICE_ADDRESS(address) | ctrl, &reg->dcfg);

	dwc2_udc_ep0_zlp(dev);

	dwc2dbg_cond(DEBUG_EP0 != 0,
		   "%s: USB OTG 2.0 Device address=%d, DCFG=0x%x\n",
		   __func__, address, DWC2_UncachedRead32(&reg->dcfg));

	dev->usb_address = address;
	dev->connected = 1;
}

static inline void dwc2_udc_ep0_set_stall(struct dwc2_ep *ep, uint32_t is_in)
{
	struct dwc2_udc *dev = ep->dev;
	struct dwc2_usbotg_reg *reg = dev->reg;
	uint32_t		ep_ctrl = 0;

	dwc2_log_write(0x087087, is_in, 0, 0, 0);

	if (is_in)
		ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl);
	else
		ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[EP0_CON].doepctl);

	/* set the disable and stall bits */
	//if (ep_ctrl & DEPCTL_EPENA)
	//	ep_ctrl |= DEPCTL_EPDIS;

	ep_ctrl |= DEPCTL_STALL;

	if (is_in) {
		DWC2_UncachedWrite32(ep_ctrl, &reg->in_endp[EP0_CON].diepctl);
		dwc2dbg_cond(DEBUG_EP0 != 0,
			   "%s: set ep%d stall, DIEPCTL0 = 0x%p\n",
			   __func__, ep_index(ep), &reg->in_endp[EP0_CON].diepctl);
	} else {
		DWC2_UncachedWrite32(ep_ctrl, &reg->out_endp[EP0_CON].doepctl);
		dwc2dbg_cond(DEBUG_EP0 != 0,
			   "%s: set ep%d stall, DOEPCTL0 = 0x%p\n",
			   __func__, ep_index(ep), &reg->out_endp[EP0_CON].doepctl);
	}

	/*
	 * The application can only set this bit, and the core clears it,
	 * when a SETUP token is received for this endpoint
	 */
	dev->ep0state = WAIT_FOR_SETUP;

#ifdef DWC2_DMA_EN
	dwc2_udc_pre_setup(dev);
#endif
}

void dwc2_ep0_read(struct dwc2_udc *dev)
{
	struct dwc2_request *req;
	struct dwc2_ep *ep = &dev->ep[0];

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct dwc2_request, queue);

	} else {
		dwc2dbg("%s: ---> BUG\n", __func__);
		return;
	}

	dwc2dbg_cond(DEBUG_EP0 != 0,
		   "%s: req = %p, req.length = 0x%x, req.actual = 0x%x\n",
		   __func__, req, req->req.length, req->req.actual);

	if (req->req.length == 0) {
		/* zlp for Set_configuration, Set_interface, or Bulk-Only mass storge reset */

		ep->len = 0;
		dwc2_udc_ep0_zlp(dev);

		dwc2dbg_cond(DEBUG_EP0 != 0,
			   "%s: req.length = 0, bRequest = %d\n",
			   __func__, dev->usb_ctrl->bRequest);
		return;
	}

	setdma_rx(ep, req);
}

/*
 * DATA_STATE_XMIT
 */
int dwc2_ep0_write(struct dwc2_udc *dev)
{
	struct dwc2_request *req;
	struct dwc2_ep *ep = &dev->ep[0];
	int ret, need_zlp = 0;

	if (list_empty(&ep->queue))
		req = 0;
	else
		req = list_entry(ep->queue.next, struct dwc2_request, queue);

	if (!req) {
		dwc2dbg_cond(DEBUG_EP0 != 0, "%s: NULL REQ\n", __func__);
		return 0;
	}

	dwc2dbg_cond(DEBUG_EP0 != 0,
		   "%s: req = %p, req.length = 0x%x, req.actual = 0x%x\n",
		   __func__, req, req->req.length, req->req.actual);

	// if (req->req.length - req->req.actual == ep0_fifo_size) {
	// /* Next write will end with the packet size, */
	// /* so we need Zero-length-packet */
	// need_zlp = 1;
	// }

	ret = dwc2_write_fifo_ep0(ep, req);

	if ((ret == 1) && !need_zlp) {
		/* Last packet */
		dev->ep0state = WAIT_FOR_COMPLETE;
		dwc2dbg_cond(DEBUG_EP0 != 0,
			   "%s: finished, waiting for status\n", __func__);

	} else {
		dev->ep0state = DATA_STATE_XMIT;
		dwc2dbg_cond(DEBUG_EP0 != 0,
			   "%s: not finished\n", __func__);
	}

	return 1;
}

static int dwc2_udc_get_status(struct dwc2_udc *dev,
		CH9_UsbSetup *crq)
{
	uint8_t ep_num = dwc2_phy_to_log_ep(crq->wIndex & 0x7F, !!(crq->wIndex & 0x80));
	uint16_t g_status = 0;
	uint32_t ep_ctrl;
	struct dwc2_usbotg_reg *reg = dev->reg;

	dwc2dbg_cond(DEBUG_SETUP != 0,
		   "%s: *** USB_REQ_GET_STATUS\n", __func__);
	tf_printf("crq->brequest:0x%x\n", crq->bmRequestType & CH9_REQ_RECIPIENT_MASK);
	switch (crq->bmRequestType & CH9_REQ_RECIPIENT_MASK) {
	case CH9_USB_REQ_RECIPIENT_INTERFACE:
		g_status = 0;
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tGET_STATUS:CH9_USB_REQ_RECIPIENT_INTERFACE, g_stauts = %d\n",
			   g_status);
		break;

	case CH9_USB_REQ_RECIPIENT_DEVICE:
		g_status = 0x1; /* Self powered */
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tGET_STATUS: CH9_USB_REQ_RECIPIENT_DEVICE, g_stauts = %d\n",
			   g_status);
		break;

	case CH9_USB_REQ_RECIPIENT_ENDPOINT:
		if (crq->wLength > 2) {
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tGET_STATUS:Not support EP or wLength\n");
			return 1;
		}

		g_status = dev->ep[ep_num].stopped;
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tGET_STATUS: CH9_USB_REQ_RECIPIENT_ENDPOINT, g_stauts = %d\n",
			   g_status);

		break;

	default:
		return 1;
	}

	memcpy(dev->usb_ctrl, &g_status, sizeof(g_status));

	// DWC2_CacheFlush((unsigned long) dev->usb_ctrl, ROUND(sizeof(g_status), CONFIG_SYS_CACHELINE_SIZE));
	// flush_dcache_range((unsigned long)dev->usb_ctrl, ROUND(sizeof(g_status), CONFIG_SYS_CACHELINE_SIZE));

#ifdef DWC2_DMA_EN
	DWC2_UncachedWrite32(dev->usb_ctrl_dma_addr, &reg->in_endp[EP0_CON].diepdma);
	DWC2_UncachedWrite32(DIEPT_SIZ_PKT_CNT(1) | DIEPT_SIZ_XFER_SIZE(2),
		&reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl);
	ep_ctrl &= ~(1 << 30);
	DWC2_UncachedWrite32(ep_ctrl|DEPCTL_EPENA|DEPCTL_CNAK,
		&reg->in_endp[EP0_CON].diepctl);
#else
	DWC2_UncachedWrite32(DIEPT_SIZ_PKT_CNT(1) | DIEPT_SIZ_XFER_SIZE(2),
		&reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[EP0_CON].diepctl);
	ep_ctrl |= (DEPCTL_EPENA|DEPCTL_CNAK);
	DWC2_UncachedWrite32(ep_ctrl, &reg->in_endp[EP0_CON].diepctl);

	dwc2_ep_fifo_write(dev, ep_num, (uint8_t *)dev->usb_ctrl, sizeof(g_status));
	DWC2_UncachedWrite32(1UL << (ep_num & 0x0f), &reg->diepempmsk);
#endif
	dev->ep0state = WAIT_FOR_NULL_COMPLETE;

	return 0;
}

void dwc2_udc_set_nak(struct dwc2_ep *ep)
{
	uint8_t		ep_num;
	uint32_t		ep_ctrl = 0;
	struct dwc2_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);
	dwc2dbg("%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl);
		ep_ctrl |= DEPCTL_SNAK;
		DWC2_UncachedWrite32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		dwc2dbg("%s: set NAK, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl));
	} else {
		ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl);
		ep_ctrl |= DEPCTL_SNAK;
		DWC2_UncachedWrite32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		dwc2dbg("%s: set NAK, DOEPCTL%d = 0x%x\n",
		      __func__, ep_num, DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl));
	}
}


static void dwc2_udc_ep_set_stall(struct dwc2_ep *ep)
{
	uint8_t		ep_num;
	uint32_t		ep_ctrl = 0;
	struct dwc2_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);
	dwc2dbg("%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl);

		/* set the disable and stall bits */
		if (ep_ctrl & DEPCTL_EPENA)
			ep_ctrl |= DEPCTL_EPDIS;

		ep_ctrl |= DEPCTL_STALL;

		DWC2_UncachedWrite32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		dwc2dbg("%s: set stall, DIEPCTL%d = 0x%x\n",
		      __func__, ep_num, DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl));

	} else {
		ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl);

		/* set the stall bit */
		ep_ctrl |= DEPCTL_STALL;

		DWC2_UncachedWrite32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		dwc2dbg("%s: set stall, DOEPCTL%d = 0x%x\n",
		      __func__, ep_num, DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl));
	}
}

static void dwc2_udc_ep_clear_stall(struct dwc2_ep *ep)
{
	uint8_t		ep_num;
	uint32_t		ep_ctrl = 0;
	struct dwc2_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);
	dwc2dbg("%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl);

		/* clear stall bit */
		ep_ctrl &= ~DEPCTL_STALL;

		/*
		 * USB Spec 9.4.5: For endpoints using data toggle, regardless
		 * of whether an endpoint has the Halt feature set, a
		 * ClearFeature(ENDPOINT_HALT) request always results in the
		 * data toggle being reinitialized to DATA0.
		 */
		if (ep->bmAttributes == CH9_USB_EP_INTERRUPT
		    || ep->bmAttributes == CH9_USB_EP_BULK) {
			ep_ctrl |= DEPCTL_SETD0PID; /* DATA0 */
		}

		DWC2_UncachedWrite32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		dwc2dbg("%s: cleared stall, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl));

	} else {
		ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl);

		/* clear stall bit */
		ep_ctrl &= ~DEPCTL_STALL;

		if (ep->bmAttributes == CH9_USB_EP_INTERRUPT
		    || ep->bmAttributes == CH9_USB_EP_BULK) {
			ep_ctrl |= DEPCTL_SETD0PID; /* DATA0 */
		}

		DWC2_UncachedWrite32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		dwc2dbg("%s: cleared stall, DOEPCTL%d = 0x%x\n",
		      __func__, ep_num, DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl));
	}
}

int dwc2_udc_set_halt(struct usb_ep *_ep, int value)
{
	struct dwc2_ep	*ep;
	struct dwc2_udc	*dev;
	uint8_t		ep_num;

	ep = container_of(_ep, struct dwc2_ep, ep);
	ep_num = ep_index(ep);

	if (!_ep || !ep->desc || ep_num == EP0_CON ||
		     ep->desc->bmAttributes == CH9_USB_EP_ISOCHRONOUS) {
		dwc2dbg("%s: %s bad ep or descriptor\n", __func__, ep->ep.name);
		return -EINVAL;
	}

	/* Attempt to halt IN ep will fail if any transfer requests are still queue */
	if (value && ep_is_in(ep) && !list_empty(&ep->queue)) {
		dwc2dbg("%s: %s queue not empty, req = %p\n",
			__func__, ep->ep.name,
			list_entry(ep->queue.next, struct dwc2_request, queue));

		return -EAGAIN;
	}

	dev = ep->dev;
	dwc2dbg("%s: ep_num = %d, value = %d\n", __func__, ep_num, value);

	if (value == 0) {
		ep->stopped = 0;
		dwc2_udc_ep_clear_stall(ep);
	} else {
		if (ep_num == 0)
			dev->ep0state = WAIT_FOR_SETUP;

		ep->stopped = 1;
		dwc2_udc_ep_set_stall(ep);
	}

	return 0;
}

void dwc2_udc_ep_activate(struct dwc2_ep *ep)
{
	uint8_t ep_num;
	uint32_t ep_ctrl = 0, daintmsk = 0;
	struct dwc2_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);

	/* Read DEPCTLn register */
	if (ep_is_in(ep)) {
		ep_ctrl = DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl);
		daintmsk = 1 << ep_num;
	} else {
		ep_ctrl = DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl);
		daintmsk = (1 << ep_num) << DAINT_OUT_BIT;
	}

	dwc2dbg("%s: EPCTRL%d = 0x%x, ep_is_in = %d\n",
		__func__, ep_num, ep_ctrl, ep_is_in(ep));

	/* If the EP is already active don't change the EP Control register. */
	if (!(ep_ctrl & DEPCTL_USBACTEP)) {
		ep_ctrl = (ep_ctrl & ~DEPCTL_TYPE_MASK) |
			(ep->bmAttributes << DEPCTL_TYPE_BIT);
		ep_ctrl = (ep_ctrl & ~DEPCTL_MPS_MASK) |
			(ep->ep.maxpacket << DEPCTL_MPS_BIT);
		ep_ctrl |= (DEPCTL_SETD0PID | DEPCTL_USBACTEP | DEPCTL_SNAK);

		if (ep_is_in(ep)) {
			DWC2_UncachedWrite32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
			dwc2dbg("%s: USB Ative EP%d, DIEPCTRL%d = 0x%x\n",
			      __func__, ep_num, ep_num,
			      DWC2_UncachedRead32(&reg->in_endp[ep_num].diepctl));
		} else {
			DWC2_UncachedWrite32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
			dwc2dbg("%s: USB Ative EP%d, DOEPCTRL%d = 0x%x\n",
			      __func__, ep_num, ep_num,
			      DWC2_UncachedRead32(&reg->out_endp[ep_num].doepctl));
		}
	}

	/* Unmask EP Interrtupt */
	DWC2_UncachedWrite32(DWC2_UncachedRead32(&reg->daintmsk)|daintmsk, &reg->daintmsk);
	dwc2dbg("%s: DAINTMSK = 0x%x\n", __func__, DWC2_UncachedRead32(&reg->daintmsk));

}

static int dwc2_udc_clear_feature(struct usb_ep *_ep)
{
	struct dwc2_udc	*dev;
	struct dwc2_ep	*ep;
	uint8_t		ep_num;
	CH9_UsbSetup *usb_ctrl;

	ep = container_of(_ep, struct dwc2_ep, ep);
	ep_num = ep_index(ep);

	dev = ep->dev;
	usb_ctrl = dev->usb_ctrl;

	dwc2dbg_cond(DEBUG_SETUP != 0,
		   "%s: ep_num = %d, is_in = %d, clear_feature_flag = %d\n",
		   __func__, ep_num, ep_is_in(ep), dev->clear_feature_flag);

	if (usb_ctrl->wLength != 0) {
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tCLEAR_FEATURE: wLength is not zero.....\n");
		return 1;
	}

	switch (usb_ctrl->bmRequestType & CH9_REQ_RECIPIENT_MASK) {
	case CH9_USB_REQ_RECIPIENT_DEVICE:
		switch (usb_ctrl->wValue) {
		case CH9_USB_FS_DEVICE_REMOTE_WAKEUP:
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tOFF:USB_DEVICE_REMOTE_WAKEUP\n");
			break;

		case CH9_USB_FS_TEST_MODE:
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tCLEAR_FEATURE: USB_DEVICE_TEST_MODE\n");
			/** @todo Add CLEAR_FEATURE for TEST modes. */
			break;
		}

		dwc2_udc_ep0_zlp(dev);
		break;

	case CH9_USB_REQ_RECIPIENT_ENDPOINT:
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tCLEAR_FEATURE:CH9_USB_REQ_RECIPIENT_ENDPOINT, wValue = %d\n",
			   usb_ctrl->wValue);

		if (usb_ctrl->wValue == CH9_USB_FS_ENDPOINT_HALT) {
			if (ep_num == 0) {
				dwc2_udc_ep0_set_stall(ep, 1);
				return 0;
			}

			dwc2_udc_ep0_zlp(dev);

			dwc2_udc_ep_clear_stall(ep);
			dwc2_udc_ep_activate(ep);
			ep->stopped = 0;

			dev->clear_feature_num = ep_num;
			dev->clear_feature_flag = 1;
		}
		break;
	}

	return 0;
}

static int dwc2_udc_set_feature(struct usb_ep *_ep)
{
	struct dwc2_udc	*dev;
	struct dwc2_ep	*ep;
	uint8_t		ep_num;
	CH9_UsbSetup *usb_ctrl;

	ep = container_of(_ep, struct dwc2_ep, ep);
	ep_num = ep_index(ep);
	dev = ep->dev;
	usb_ctrl = dev->usb_ctrl;

	dwc2dbg_cond(DEBUG_SETUP != 0,
		   "%s: *** USB_REQ_SET_FEATURE , ep_num = %d\n",
		    __func__, ep_num);

	if (usb_ctrl->wLength != 0) {
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tSET_FEATURE: wLength is not zero.....\n");
		return 1;
	}

	switch (usb_ctrl->bmRequestType & CH9_REQ_RECIPIENT_MASK) {
	case CH9_USB_REQ_RECIPIENT_DEVICE:
		switch (usb_ctrl->wValue) {
		case CH9_USB_FS_DEVICE_REMOTE_WAKEUP:
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tSET_FEATURE:USB_DEVICE_REMOTE_WAKEUP\n");
			break;
		case CH9_USB_FS_B_HNP_ENABLE:
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tSET_FEATURE: USB_DEVICE_B_HNP_ENABLE\n");
			break;

		case CH9_USB_FS_A_HNP_SUPPORT:
			/* RH port supports HNP */
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tSET_FEATURE:USB_DEVICE_A_HNP_SUPPORT\n");
			break;

		case CH9_USB_FS_A_ALT_HNP_SUPPORT:
			/* other RH port does */
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tSET: USB_DEVICE_A_ALT_HNP_SUPPORT\n");
			break;
		case CH9_USB_FS_TEST_MODE:
			dev->test_mode = usb_ctrl->wIndex >> 8;
			break;
		}

		dwc2_udc_ep0_zlp(dev);
		return 0;

	case CH9_USB_REQ_RECIPIENT_INTERFACE:
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tSET_FEATURE: CH9_USB_REQ_RECIPIENT_INTERFACE\n");
		break;

	case CH9_USB_REQ_RECIPIENT_ENDPOINT:
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tSET_FEATURE: CH9_USB_REQ_RECIPIENT_ENDPOINT\n");
		if (usb_ctrl->wValue == CH9_USB_FS_ENDPOINT_HALT) {
			if (ep_num == 0) {
				dwc2_udc_ep0_set_stall(ep, 1);
				return 0;
			}
			ep->stopped = 1;
			dwc2_udc_ep_set_stall(ep);
		}

		dwc2_udc_ep0_zlp(dev);
		return 0;
	}

	return 1;
}

/*
 * WAIT_FOR_SETUP (OUT_PKT_RDY)
 */
static void dwc2_ep0_setup(struct dwc2_udc *dev)
{
	struct dwc2_ep *ep = &dev->ep[0];
	int i;
	uint8_t ep_num;
	CH9_UsbSetup *usb_ctrl = dev->usb_ctrl;
	uint8_t three_stage = 0;

	/* Nuke all previous transfers */
	dwc2_nuke(ep, -EPROTO);

	/* read control req from fifo (8 bytes) */
	dwc2_fifo_read(ep, (uintptr_t *)usb_ctrl, 8);

	// if (usb_ctrl->bRequest == 0)
	// {
	//	usb_ctrl->bRequest = 0x06;
	//	usb_ctrl->wValue = 0x100;
	// }

	dwc2dbg_cond(DEBUG_SETUP != 0,
				"%s: bmRequestType = 0x%x(%s), bRequest = 0x%x wLength = 0x%x, wValue = 0x%x, wIndex= 0x%x\n",
				__func__, usb_ctrl->bmRequestType,
				(usb_ctrl->bmRequestType & USB_DIR_IN) ? "IN" : "OUT",
				usb_ctrl->bRequest,
				usb_ctrl->wLength, usb_ctrl->wValue, usb_ctrl->wIndex);

	three_stage = usb_ctrl->wLength ? 1 : 0;
#ifdef DWC2_DBG
	{
		int i, len = sizeof(*usb_ctrl);
		char *p = (char *)usb_ctrl;

		tf_printf("pkt = ");
		for (i = 0; i < len; i++) {
			tf_printf("%02x", ((uint8_t *)p)[i]);
			if ((i & 7) == 7)
				tf_printf(" ");
		}
		tf_printf("\n");
	}
#endif

	/* Set direction of EP0 */
	if (usb_ctrl->bmRequestType & USB_DIR_IN) {
		ep->bEndpointAddress |= USB_DIR_IN;
	} else {
		ep->bEndpointAddress &= ~USB_DIR_IN;
	}
	/* cope with automagic for some standard requests. */
	dev->req_std = (usb_ctrl->bmRequestType & CH9_USB_REQ_TYPE_MASK)
		== CH9_USB_REQ_TYPE_STANDARD;

	dev->req_pending = 1;

	/* Handle some SETUP packets ourselves */
	if (dev->req_std) {
		switch (usb_ctrl->bRequest) {
		case CH9_USB_REQ_SET_ADDRESS:
			dwc2_log_write(0xBBBBB, 1, usb_ctrl->wValue, 0, 0);
			dwc2dbg_cond(DEBUG_SETUP != 0,
			   "%s: *** USB_REQ_SET_ADDRESS (%d)\n",
			   __func__, usb_ctrl->wValue);
			if (usb_ctrl->bmRequestType
				!= (CH9_USB_REQ_TYPE_STANDARD | CH9_USB_REQ_RECIPIENT_DEVICE))
				break;

			dwc2_set_address(dev, usb_ctrl->wValue);
			return;

		case CH9_USB_REQ_SET_CONFIGURATION:
			dwc2_log_write(0xBBBBB, 2, usb_ctrl->wValue, 0, 0);
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "=====================================\n");
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "%s: USB_REQ_SET_CONFIGURATION (%d)\n",
				   __func__, usb_ctrl->wValue);

			break;

		case CH9_USB_REQ_GET_DESCRIPTOR:
			dwc2_log_write(0xBBBBB, 3, 0, 0, 0);
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "%s: *** USB_REQ_GET_DESCRIPTOR\n",
				   __func__);
			break;

		case CH9_USB_REQ_SET_INTERFACE:
			dwc2_log_write(0xBBBBB, 4, 0, 0, 0);
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "%s: *** USB_REQ_SET_INTERFACE (%d)\n",
				   __func__, usb_ctrl->wValue);

			break;

		case CH9_USB_REQ_GET_CONFIGURATION:
			dwc2_log_write(0xBBBBB, 5, 0, 0, 0);
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "%s: *** USB_REQ_GET_CONFIGURATION\n",
				   __func__);
			break;

		case CH9_USB_REQ_GET_STATUS:
			dwc2_log_write(0xBBBBB, 6, 0, 0, 0);
			if (!dwc2_udc_get_status(dev, usb_ctrl))
				return;

			break;

		case CH9_USB_REQ_CLEAR_FEATURE:
			dwc2_log_write(0xBBBBB, 7, 0, 0, 0);
			ep_num = dwc2_phy_to_log_ep(usb_ctrl->wIndex & 0x7F, !!(usb_ctrl->wIndex & 0x80));

			if (!dwc2_udc_clear_feature(&dev->ep[ep_num].ep))
				return;

			break;

		case CH9_USB_REQ_SET_FEATURE:
			dwc2_log_write(0xBBBBB, 8, 0, 0, 0);
			ep_num = dwc2_phy_to_log_ep(usb_ctrl->wIndex & 0x7F, !!(usb_ctrl->wIndex & 0x80));

			if (!dwc2_udc_set_feature(&dev->ep[ep_num].ep))
				return;

			break;

		default:
			dwc2_log_write(0xBBBBB, 9, 0, 0, 0);
			dwc2dbg_cond(DEBUG_SETUP != 0,
						"%s: *** Default of usb_ctrl->bRequest=0x%x happened.\n",
						__func__, usb_ctrl->bRequest);
			break;
		}
	}

	if (dev->driver) {
		/* device-2-host (IN) or no data setup command, process immediately */
		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "%s:usb_ctrlreq will be passed to fsg_setup()\n",
			    __func__);

		i = dev->driver->setup(&dev->gadget, usb_ctrl);

		if (i < 0) {
			uint32_t dir = (usb_ctrl->wLength == 0) ? 1 : ep_is_in(ep);
			/* setup processing failed, force stall */
			dwc2_udc_ep0_set_stall(ep, dir);
			dev->ep0state = WAIT_FOR_SETUP;

			dwc2dbg_cond(DEBUG_SETUP != 0, "\tdev->driver->setup failed (%d), bRequest = %d\n",
						i, usb_ctrl->bRequest);
			return;


		} else if (dev->req_pending) {
			dev->req_pending = 0;
			dwc2dbg_cond(DEBUG_SETUP != 0,
				   "\tdev->req_pending...\n");
		}

		dwc2dbg_cond(DEBUG_SETUP != 0,
			   "\tep0state = %s\n", state_names[dev->ep0state]);

	}

	if (!three_stage)
		dwc2_udc_ep0_zlp(dev);
}

/*
 * handle ep0 interrupt
 */
void dwc2_handle_ep0(struct dwc2_udc *dev)
{
	if (dev->ep0state == WAIT_FOR_SETUP) {
		dwc2dbg_cond(DEBUG_OUT_EP != 0,
			   "%s: WAIT_FOR_SETUP\n", __func__);
		dwc2_ep0_setup(dev);

	} else {
		dwc2dbg_cond(DEBUG_OUT_EP != 0,
			   "%s: strange state!!(state = %s)\n",
			__func__, state_names[dev->ep0state]);
	}
}

void dwc2_ep0_kick(struct dwc2_udc *dev, struct dwc2_ep *ep)
{
	dwc2dbg_cond(DEBUG_EP0 != 0,
		   "%s: ep_is_in = %d\n", __func__, ep_is_in(ep));
	if (ep_is_in(ep)) {
		dev->ep0state = DATA_STATE_XMIT;
		dwc2_ep0_write(dev);

	} else {
		dev->ep0state = DATA_STATE_RECV;
		dwc2_ep0_read(dev);
	}
}
