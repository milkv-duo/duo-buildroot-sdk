/*
 * drivers/usb/gadget/cvi_udc_otg_xfer_dma.c
 * Designware CVI on-chip full/high speed USB OTG 2.0 device controllers
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

static void set_max_pktsize(struct cvi_udc *dev, CH9_UsbSpeed speed)
{
	unsigned int ep_ctrl;
	struct cvi_usbotg_reg *reg = dev->reg;
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
	for (i = 1; i < CVI_MAX_ENDPOINTS; i++)
		dev->ep[i].ep.maxpacket = ep_fifo_size;

	/* EP0 - Control IN (64 bytes)*/
	ep_ctrl = cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl);
	cvi_uncached_write32(ep_ctrl & ~(3 << 0), &reg->in_endp[EP0_CON].diepctl);

	/* EP0 - Control OUT (64 bytes)*/
	ep_ctrl = cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl);
	cvi_uncached_write32(ep_ctrl & ~(3 << 0), &reg->out_endp[EP0_CON].doepctl);
}

static inline void cvi_udc_ep0_zlp(struct cvi_udc *dev)
{
	uint32_t ep_ctrl;
	struct cvi_usbotg_reg *reg = dev->reg;

	cvi_uncached_write32(dev->usb_ctrl_dma_addr, &reg->in_endp[EP0_CON].diepdma);
	cvi_uncached_write32(DIEPT_SIZ_PKT_CNT(1), &reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl);
	ep_ctrl &= ~(1 << 30);
	ep_ctrl |= (DEPCTL_EPENA | DEPCTL_CNAK);
	cvi_uncached_write32(ep_ctrl,
			      &reg->in_endp[EP0_CON].diepctl);

	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl));
	dev->ep0state = WAIT_FOR_IN_COMPLETE;
	cvi_log_write(0xAAAAA, ep_ctrl, dev->usb_ctrl_dma_addr,
		       DIEPT_SIZ_PKT_CNT(1), 0);
}

void cvi_udc_pre_setup(struct cvi_udc *dev)
{
	uint32_t ep_ctrl, tmp, tmp1;
	struct cvi_usbotg_reg *reg = dev->reg;

	cvidbg_cond(DEBUG_SETUP,
		     "%s : Prepare Setup packets.\n", __func__);

	cvi_uncached_write32(DOEPT_SIZ_SUS_CNT(1) | DOEPT_SIZ_PKT_CNT(1) | sizeof(CH9_UsbSetup),
			      &reg->out_endp[EP0_CON].doeptsiz);
	cvi_uncached_write32(dev->usb_ctrl_dma_addr, &reg->out_endp[EP0_CON].doepdma);

	tmp = cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl);
	ep_ctrl = DEPCTL_EPENA | DEPCTL_USBACTEP;
	cvi_uncached_write32(ep_ctrl, &reg->out_endp[EP0_CON].doepctl);

	tmp1 = cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl);
	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl));
	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl));
	cvi_log_write(0x99999, ep_ctrl, dev->usb_ctrl_dma_addr,
		       tmp, tmp1);

}

static inline void cvi_ep0_complete_out(struct cvi_udc *dev)
{
	uint32_t ep_ctrl;
	struct cvi_usbotg_reg *reg = dev->reg;

	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl));
	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl));

	cvidbg_cond(DEBUG_OUT_EP,
		     "%s : Prepare Complete Out packet.\n", __func__);

	//cvidbg_cond(1,
	//	   "CO\n");
	cvi_uncached_write32(DOEPT_SIZ_PKT_CNT(1) | sizeof(CH9_UsbSetup),
			      &reg->out_endp[EP0_CON].doeptsiz);
	cvi_uncached_write32(dev->usb_ctrl_dma_addr, &reg->out_endp[EP0_CON].doepdma);

	ep_ctrl = cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl);
	ep_ctrl &= ~(1 << 30);
	ep_ctrl |= (DEPCTL_EPENA | DEPCTL_CNAK);
	cvi_uncached_write32(ep_ctrl,
			      &reg->out_endp[EP0_CON].doepctl);

	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DIEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl));
	cvidbg_cond(DEBUG_EP0 != 0, "%s:EP0 ZLP DOEPCTL0 = 0x%x\n",
		     __func__, cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl));
	cvi_log_write(0x88888, ep_ctrl, dev->usb_ctrl_dma_addr, DOEPT_SIZ_PKT_CNT(1) | sizeof(CH9_UsbSetup), 0);

}

static int setdma_rx(struct cvi_ep *ep, struct cvi_request *req)
{
	uint32_t *buf, ctrl;
	uint32_t length, pktcnt;
	uint32_t ep_num = ep_index(ep);
	uint32_t dma_addr;
	struct cvi_usbotg_reg *reg = ep->dev->reg;

	buf = req->req.buf + req->req.actual;
	length = min_t(uint32_t, req->req.length - req->req.actual,
		       ep_num ? DMA_BUFFER_SIZE : ep->ep.maxpacket);

	ep->len = length;
	ep->dma_buf = buf;

	if (ep_num == EP0_CON || length == 0)
		pktcnt = 1;
	else
		pktcnt = (length - 1) / (ep->ep.maxpacket) + 1;

	ctrl =  cvi_uncached_read32(&reg->out_endp[ep_num].doepctl);

	dma_addr = (uint32_t)((uintptr_t)ep->dma_buf);
	cvi_cache_invalidate(dma_addr, ep->len);

	cvi_uncached_write32(dma_addr, &reg->out_endp[ep_num].doepdma);
	cvi_uncached_write32(DOEPT_SIZ_PKT_CNT(pktcnt) | DOEPT_SIZ_XFER_SIZE(length),
			      &reg->out_endp[ep_num].doeptsiz);
	ctrl &= ~(1 << 30);
	ctrl |= (DEPCTL_EPENA | DEPCTL_CNAK);
	cvi_uncached_write32(ctrl, &reg->out_endp[ep_num].doepctl);

	cvidbg_cond(DEBUG_OUT_EP != 0,
		     "%s: EP%d RX DMA start : DOEPDMA = 0x%x, DOEPTSIZ = 0x%x, DOEPCTL = 0x%x\n",
		     __func__, ep_num,
		     cvi_uncached_read32(&reg->out_endp[ep_num].doepdma),
		     cvi_uncached_read32(&reg->out_endp[ep_num].doeptsiz),
		     cvi_uncached_read32(&reg->out_endp[ep_num].doepctl));
	cvidbg_cond(DEBUG_OUT_EP != 0,
		     "\tbuf = 0x%p, pktcnt = %d, xfersize = %d\n",
		     buf, pktcnt, length);

	cvi_log_write(0x77777, ctrl, dma_addr, DOEPT_SIZ_PKT_CNT(pktcnt) | DOEPT_SIZ_XFER_SIZE(length), 0);
	return 0;

}

static int setdma_tx(struct cvi_ep *ep, struct cvi_request *req)
{
	uint32_t *buf, ctrl = 0, tmp, tmp1;
	uint32_t length, pktcnt;
	uint32_t ep_num = ep_index(ep);
	uint32_t dma_addr;
	struct cvi_usbotg_reg *reg = ep->dev->reg;

	buf = req->req.buf + req->req.actual;
	length = req->req.length - req->req.actual;
	if (length > DMA_BUFFER_SIZE)
		length = DMA_BUFFER_SIZE;

	if (ep_num == EP0_CON)
		length = min(length, (uint32_t)ep_maxpacket(ep));

	ep->len = length;
	ep->dma_buf = buf;

	dma_addr = (uint32_t)((uintptr_t)ep->dma_buf);
	cvi_cache_flush(dma_addr, ROUND(ep->len, CONFIG_SYS_CACHELINE_SIZE));

	if (length == 0)
		pktcnt = 1;
	else
		pktcnt = (length - 1) / (ep->ep.maxpacket) + 1;

	/* Flush the endpoint's Tx FIFO */
	//cvi_uncached_write32(TX_FIFO_NUMBER(ep->fifo_num), &reg->grstctl);
	//cvi_uncached_write32(TX_FIFO_NUMBER(ep->fifo_num) | TX_FIFO_FLUSH, &reg->grstctl);
	//while (cvi_uncached_read32(&reg->grstctl) & TX_FIFO_FLUSH)
	//	;

	cvi_uncached_write32(dma_addr, &reg->in_endp[ep_num].diepdma);
	cvi_uncached_write32(DIEPT_SIZ_PKT_CNT(pktcnt) | DIEPT_SIZ_XFER_SIZE(length),
			      &reg->in_endp[ep_num].dieptsiz);

	ctrl = cvi_uncached_read32(&reg->in_endp[ep_num].diepctl);
	tmp = ctrl;

	/* Write the FIFO number to be used for this endpoint */
	ctrl &= DIEPCTL_TX_FIFO_NUM_MASK;
	ctrl &= ~(1 << 30);
	ctrl |= DIEPCTL_TX_FIFO_NUM(ep->fifo_num);

	/* Clear reserved (Next EP) bits */
	ctrl = (ctrl & ~(EP_MASK << DEPCTL_NEXT_EP_BIT));
	ctrl |= (DEPCTL_EPENA | DEPCTL_CNAK);
	//printf("diepctl0 = 0x%x\n", ctrl);
	cvi_uncached_write32(ctrl, &reg->in_endp[ep_num].diepctl);

	tmp1 = cvi_uncached_read32(&reg->in_endp[ep_num].diepctl);
	cvidbg_cond(DEBUG_IN_EP,
		     "%s:EP%d TX DMA start : DIEPDMA0 = 0x%x, DIEPTSIZ0 = 0x%x, DIEPCTL0 = 0x%x\n",
		     __func__, ep_num,
		     cvi_uncached_read32(&reg->in_endp[ep_num].diepdma),
		     cvi_uncached_read32(&reg->in_endp[ep_num].dieptsiz),
		     cvi_uncached_read32(&reg->in_endp[ep_num].diepctl));
	cvidbg_cond(DEBUG_IN_EP,
		     "\tbuf = 0x%p, pktcnt = %d, xfersize = %d\n",
		     buf, pktcnt, length);

	cvi_log_write(0x44444, ctrl, dma_addr, tmp, tmp1);
	cvi_log_write(0x46464, cvi_uncached_read32(&reg->in_endp[ep_num].dieptsiz), length, ep_num, 0);

	return length;
}

static void complete_rx(struct cvi_udc *dev, uint8_t ep_num)
{
	struct cvi_ep *ep = &dev->ep[cvi_phy_to_log_ep(ep_num, 0)];
	struct cvi_request *req = NULL;
	uint32_t ep_tsr = 0, xfer_size = 0, is_short = 0;
	uint32_t dma_addr;
	struct cvi_usbotg_reg *reg = dev->reg;

	if (list_empty(&ep->queue)) {
		cvidbg_cond(DEBUG_OUT_EP != 0,
			     "%s: RX DMA done : NULL REQ on OUT EP-%d\n",
			     __func__, ep_num);
		return;

	}

	req = list_entry(ep->queue.next, struct cvi_request, queue);
	ep_tsr = cvi_uncached_read32(&reg->out_endp[ep_num].doeptsiz);

	if (ep_num == EP0_CON)
		xfer_size = (ep_tsr & DOEPT_SIZ_XFER_SIZE_MAX_EP0);
	else
		xfer_size = (ep_tsr & DOEPT_SIZ_XFER_SIZE_MAX_EP);

	xfer_size = ep->len - xfer_size;

	/*
	 * NOTE:
	 *
	 * Please be careful with proper buffer allocation for USB request,
	 * which needs to be aligned to CONFIG_SYS_CACHELINE_SIZE, not only
	 * with starting address, but also its size shall be a cache line
	 * multiplication.
	 *
	 * This will prevent from corruption of data allocated immediately
	 * before or after the buffer.
	 *
	 * For armv7, the cache_v7.c provides proper code to emit "ERROR"
	 * message to warn users.
	 */
	dma_addr = (uint32_t)((uintptr_t)ep->dma_buf);
	cvi_cache_invalidate(dma_addr, ROUND(xfer_size, CONFIG_SYS_CACHELINE_SIZE));

	req->req.actual += min(xfer_size, req->req.length - req->req.actual);
	is_short = !!(xfer_size % ep->ep.maxpacket);

	cvidbg_cond(DEBUG_OUT_EP != 0,
		     "%s: RX DMA done : ep = %d, rx bytes = %d/%d,",
		     __func__, ep_num, req->req.actual, req->req.length);
	cvidbg_cond(DEBUG_OUT_EP != 0,
		     "is_short = %d, DOEPTSIZ = 0x%x, remained bytes = %d\n",
		     is_short, ep_tsr, req->req.length - req->req.actual);

	if (is_short || req->req.actual == req->req.length) {
		if (ep_num == EP0_CON && dev->ep0state == DATA_STATE_RECV) {
			cvidbg_cond(DEBUG_OUT_EP != 0, "	=> Send ZLP\n");
			cvi_udc_ep0_zlp(dev);
			/* packet will be completed in complete_tx() */
			dev->ep0state = WAIT_FOR_IN_COMPLETE;
		} else {
			cvi_done(ep, req, 0);

			if (!list_empty(&ep->queue)) {
				req = list_entry(ep->queue.next, struct cvi_request, queue);
				cvidbg_cond(DEBUG_OUT_EP != 0,
					     "%s: Next Rx request start...\n",
					     __func__);
				setdma_rx(ep, req);
			}
		}
	} else
		setdma_rx(ep, req);
}

static void complete_tx(struct cvi_udc *dev, uint8_t ep_num)
{
	struct cvi_ep *ep = &dev->ep[cvi_phy_to_log_ep(ep_num, 1)];
	struct cvi_request *req;
	//uint32_t ep_tsr = 0, xfer_size = 0, is_short = 0;
	uint32_t xfer_size = 0;
	uint32_t last;
	//struct cvi_usbotg_reg *reg = dev->reg;

	cvi_log_write(0x55555, dev->ep0state, list_empty(&ep->queue), 0, 0);

	if (dev->ep0state == WAIT_FOR_NULL_COMPLETE) {
		dev->ep0state = WAIT_FOR_OUT_COMPLETE;
		cvi_ep0_complete_out(dev);
		return;
	}

	if (list_empty(&ep->queue)) {
		cvidbg_cond(DEBUG_IN_EP,
			     "%s: TX DMA done : NULL REQ on IN EP-%d\n",
			     __func__, ep_num);
		return;

	}

	req = list_entry(ep->queue.next, struct cvi_request, queue);

	xfer_size = ep->len;
	req->req.actual += min(xfer_size, req->req.length - req->req.actual);

	if (ep_num == 0) {
		if (dev->ep0state == DATA_STATE_XMIT) {
			cvidbg_cond(DEBUG_IN_EP,
				     "%s: ep_num = %d, ep0stat == DATA_STATE_XMIT\n",
				     __func__, ep_num);
			last = cvi_write_fifo_ep0(ep, req);
			if (last)
				dev->ep0state = WAIT_FOR_COMPLETE;
		} else if (dev->ep0state == WAIT_FOR_IN_COMPLETE) {
			cvidbg_cond(DEBUG_IN_EP,
				     "%s: ep_num = %d, completing request\n",
				     __func__, ep_num);
			cvi_done(ep, req, 0);
			dev->ep0state = WAIT_FOR_SETUP;
		} else if (dev->ep0state == WAIT_FOR_COMPLETE) {
			cvidbg_cond(DEBUG_IN_EP,
				     "%s: ep_num = %d, completing request\n",
				     __func__, ep_num);
			cvi_done(ep, req, 0);
			dev->ep0state = WAIT_FOR_OUT_COMPLETE;
			cvi_ep0_complete_out(dev);
		} else {
			cvidbg_cond(DEBUG_IN_EP,
				     "%s: ep_num = %d, invalid ep state\n",
				     __func__, ep_num);
		}
		return;
	}

	if (req->req.actual == req->req.length)
		cvi_done(ep, req, 0);

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cvi_request, queue);
		cvidbg_cond(DEBUG_IN_EP,
			     "%s: Next Tx request start...\n", __func__);
		setdma_tx(ep, req);
	}
}

static inline void cvi_udc_check_tx_queue(struct cvi_udc *dev, uint8_t ep_num)
{
	struct cvi_ep *ep = &dev->ep[cvi_phy_to_log_ep(ep_num, 1)];
	struct cvi_request *req;

	cvidbg_cond(DEBUG_IN_EP,
		     "%s: Check queue, ep_num = %d\n", __func__, ep_num);

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cvi_request, queue);
		cvidbg_cond(DEBUG_IN_EP,
			     "%s: Next Tx request(0x%p) start...\n",
			     __func__, req);

		if (ep_is_in(ep))
			setdma_tx(ep, req);
		else
			setdma_rx(ep, req);
	} else {
		cvidbg_cond(DEBUG_IN_EP,
			     "%s: NULL REQ on IN EP-%d\n", __func__, ep_num);

		return;
	}

}

static int cvi_set_test_mode(struct cvi_udc *dev, int testmode)
{
	struct cvi_usbotg_reg *reg = dev->reg;
	uint32_t dctl = cvi_uncached_read32(&reg->dctl);

	dctl &= ~DCTL_TSTCTL_MASK;
	switch (testmode) {
	case CH9_TEST_J:
	case CH9_TEST_K:
	case CH9_TEST_SE0_NAK:
	case CH9_TEST_PACKET:
	case CH9_TEST_FORCE_EN:
		printf("run test mode %d\n", testmode);
		dctl |= testmode << DCTL_TSTCTL_SHIFT;
		break;
	default:
		return -EINVAL;
	}
	cvi_uncached_write32(dctl, &reg->dctl);

	return 0;
}

static void process_ep_in_intr(struct cvi_udc *dev)
{
	uint32_t ep_intr, ep_intr_status;
	uint8_t ep_num = 0;
	struct cvi_usbotg_reg *reg = dev->reg;

	ep_intr = cvi_uncached_read32(&reg->daint);
	cvidbg_cond(DEBUG_IN_EP,
		     "*** %s: EP In interrupt : DAINT = 0x%x\n", __func__, ep_intr);

	ep_intr &= DAINT_MASK;

	while (ep_intr) {
		if (ep_intr & DAINT_IN_EP_INT(1)) {
			ep_intr_status = cvi_uncached_read32(&reg->in_endp[ep_num].diepint);
			cvidbg_cond(DEBUG_IN_EP,
				     "\tEP%d-IN : DIEPINT = 0x%x\n",
				     ep_num, ep_intr_status);
			cvi_log_write(0x737373, ep_num, ep_intr_status,
				       cvi_uncached_read32(&reg->in_endp[ep_num].dieptsiz),
				       cvi_uncached_read32(&reg->in_endp[ep_num].diepctl));
			cvi_log_write(0x747474,
				       cvi_uncached_read32(&reg->in_endp[ep_num].diepdma),
				       cvi_uncached_read32(&reg->in_endp[ep_num].dtxfsts),
				       0, 0);

			/* Interrupt Clear */
			cvi_uncached_write32(ep_intr_status, &reg->in_endp[ep_num].diepint);

			if (ep_intr_status & TRANSFER_DONE) {
				complete_tx(dev, ep_num);

				if (ep_num == 0) {
					if (dev->test_mode) {
						cvi_set_test_mode(dev, dev->test_mode);
					}
					if (dev->ep0state ==
					    WAIT_FOR_IN_COMPLETE)
						dev->ep0state = WAIT_FOR_SETUP;

					if (dev->ep0state == WAIT_FOR_SETUP)
						cvi_udc_pre_setup(dev);

					/* continue transfer after
					 * set_clear_halt for DMA mode
					 */
					if (dev->clear_feature_flag == 1) {
						cvi_udc_check_tx_queue(dev, dev->clear_feature_num);
						dev->clear_feature_flag = 0;
					}
				}
			}
		}
		ep_num++;
		ep_intr >>= 1;
	}
}

static void process_ep_out_intr(struct cvi_udc *dev)
{
	uint32_t ep_intr, ep_intr_status;
	uint8_t ep_num = 0;
	struct cvi_usbotg_reg *reg = dev->reg;

	ep_intr = cvi_uncached_read32(&reg->daint);
	cvidbg_cond(DEBUG_OUT_EP != 0,
		     "*** %s: EP OUT interrupt : DAINT = 0x%x\n",
		     __func__, ep_intr);

	ep_intr = (ep_intr >> DAINT_OUT_BIT) & DAINT_MASK;

	while (ep_intr) {
		if (ep_intr & 0x1) {
			ep_intr_status = cvi_uncached_read32(&reg->out_endp[ep_num].doepint);
			cvidbg_cond(DEBUG_OUT_EP != 0,
				     "\tEP%d-OUT : DOEPINT = 0x%x\n",
				     ep_num, ep_intr_status);

			/* Interrupt Clear */
			cvi_uncached_write32(ep_intr_status, &reg->out_endp[ep_num].doepint);

			if (ep_num == 0) {
				if (ep_intr_status &
				    CTRL_OUT_EP_SETUP_PHASE_DONE) {
					cvidbg_cond(DEBUG_OUT_EP != 0,
						     "SETUP packet arrived\n");
					//cvidbg_cond(1, "SP\n");
					cvi_log_write(0x3333, 0, 0, 0, 0);
					cvi_handle_ep0(dev);
				} else if (ep_intr_status & TRANSFER_DONE) {
					if (dev->ep0state !=
					    WAIT_FOR_OUT_COMPLETE) {
						cvi_log_write(0x66666, 1, ep_intr_status, 0, 0);
						complete_rx(dev, ep_num);
					} else {
						cvi_log_write(0x66666, 2, ep_intr_status, 0, 0);
						dev->ep0state = WAIT_FOR_SETUP;
						cvi_udc_pre_setup(dev);
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
int cvi_udc_irq(int irq, void *_dev)
{
	struct cvi_udc *dev = _dev;
	uint32_t intr_status;
	uint32_t usb_status, gintmsk;
	struct cvi_usbotg_reg *reg = dev->reg;

	intr_status = cvi_uncached_read32(&reg->gintsts);
	gintmsk = cvi_uncached_read32(&reg->gintmsk);

	cvidbg_cond(DEBUG_ISR,
		     "\n*** %s : GINTSTS=0x%x(on state %s), GINTMSK : 0x%x,",
		     __func__, intr_status, state_names[dev->ep0state], gintmsk);
	cvidbg_cond(DEBUG_ISR,
		     "DAINT : 0x%x, DAINTMSK : 0x%x\n",
		     cvi_uncached_read32(&reg->daint), cvi_uncached_read32(&reg->daintmsk));

	if (!intr_status) {
		return 0;
	}

	cvi_log_write(0xEFEF, intr_status, dev->ep0state,
		       cvi_uncached_read32(&reg->daint), 0);

	if (intr_status & INT_ENUMDONE) {
		cvidbg_cond(DEBUG_ISR, "\tSpeed Detection interrupt\n");

		cvi_uncached_write32(INT_ENUMDONE, &reg->gintsts);
		usb_status = (cvi_uncached_read32(&reg->dsts) & 0x6);

		if (usb_status & (USB_FULL_30_60MHZ | USB_FULL_48MHZ)) {
			cvidbg_cond(DEBUG_ISR,
				     "\t\tFull Speed Detection\n");
			set_max_pktsize(dev, CH9_USB_SPEED_FULL);
			cvi_log_write(0x2222, 2, 0, 0, 0);

		} else {
			cvidbg_cond(DEBUG_ISR,
				     "\t\tHigh Speed Detection : 0x%x\n",
				     usb_status);
			//cvidbg_cond(1, "HS\n");
			cvi_log_write(0x2222, 1, 0, 0, 0);
			set_max_pktsize(dev, CH9_USB_SPEED_HIGH);
		}
	}

	if (intr_status & INT_EARLY_SUSPEND) {
		cvidbg_cond(DEBUG_ISR, "\tEarly suspend interrupt\n");
		cvi_uncached_write32(INT_EARLY_SUSPEND, &reg->gintsts);
	}

	if (intr_status & INT_SUSPEND) {
		usb_status = cvi_uncached_read32(&reg->dsts);
		cvidbg_cond(DEBUG_ISR,
			     "\tSuspend interrupt :(DSTS):0x%x\n", usb_status);
		cvi_uncached_write32(INT_SUSPEND, &reg->gintsts);

		if (dev->gadget.speed != CH9_USB_SPEED_UNKNOWN && dev->driver) {
			if (dev->driver->suspend)
				dev->driver->suspend(&dev->gadget);
		}
		//cvi_hsotg_set_bit(&reg->pcgcctl, PCGCTL_GATEHCLK);
		//cvi_hsotg_set_bit(&reg->pcgcctl, PCGCTL_STOPPCLK);
	}

	if (intr_status & INT_RESUME) {
		cvidbg_cond(DEBUG_ISR, "\tResume interrupt\n");
		cvi_uncached_write32(INT_RESUME, &reg->gintsts);

		//cvi_hsotg_clear_bit(&reg->pcgcctl, PCGCTL_GATEHCLK);
		//cvi_hsotg_clear_bit(&reg->pcgcctl, PCGCTL_STOPPCLK);

		if (dev->gadget.speed != CH9_USB_SPEED_UNKNOWN && dev->driver && dev->driver->resume) {

			dev->driver->resume(&dev->gadget);
		}
	}

	if ((intr_status & INT_RESET) || (intr_status & INT_RESETDET)) {
		//cvi_hsotg_clear_bit(&reg->pcgcctl, PCGCTL_GATEHCLK);
		//cvi_hsotg_clear_bit(&reg->pcgcctl, PCGCTL_STOPPCLK);

		usb_status = cvi_uncached_read32(&reg->gotgctl);
		cvidbg_cond(DEBUG_ISR,
			     "\tReset interrupt - (GOTGCTL):0x%x\n", usb_status);
		//cvidbg_cond(1,	"R\n");
		cvi_log_write(0x1111, usb_status, 0, 0, 0);
		if (intr_status & INT_RESET)
			cvi_uncached_write32(INT_RESET, &reg->gintsts);
		if (intr_status & INT_RESETDET)
			cvi_uncached_write32(INT_RESETDET, &reg->gintsts);

		if ((usb_status & 0xc0000) == (0x3 << 18)) {
			unsigned int connected = dev->connected;

			cvidbg_cond(DEBUG_ISR,
				     "\t\tOTG core got reset!!\n");
			cvi_disconnect(dev);
			if (connected)
				cvi_reconfig_usbd(dev, 1);
			dev->ep0state = WAIT_FOR_SETUP;
		} else {
			cvidbg_cond(DEBUG_ISR,
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
int cvi_queue(struct usb_ep *_ep, struct usb_request *_req)
{
	struct cvi_request *req;
	struct cvi_ep *ep;
	struct cvi_udc *dev;
	uint32_t ep_num, gintsts;
	struct cvi_usbotg_reg *reg;

	req = container_of(_req, struct cvi_request, req);
	if (!_req || !_req->complete || !_req->buf || !list_empty(&req->queue)) {
		cvidbg("%s: bad params\n", __func__);
		return -EINVAL;
	}

	ep = container_of(_ep, struct cvi_ep, ep);

	if (!_ep || (!ep->desc && ep->ep.name != cvi_get_ep0_name())) {
		cvidbg("%s: bad ep: %s, %d, %p\n", __func__,
			ep->ep.name, !ep->desc, _ep);
		return -EINVAL;
	}

	ep_num = ep_index(ep);
	dev = ep->dev;
	if (!dev->driver || dev->gadget.speed == CH9_USB_SPEED_UNKNOWN) {
		cvidbg("%s: bogus device state %p\n", __func__, dev->driver);
		return -ESHUTDOWN;
	}
	reg = dev->reg;

	_req->status = -EINPROGRESS;
	_req->actual = 0;

	/* kickstart this i/o queue? */
	cvidbg("\n*** %s: %s-%s req = %p, len = %d, buf = %p, Q empty = %d, stopped = %d\n",
		__func__, _ep->name, ep_is_in(ep) ? "in" : "out",
		_req, _req->length, _req->buf,
		list_empty(&ep->queue), ep->stopped);

#ifdef DEBUG
	{
		int i, len = _req->length;

		printf("pkt = ");
		if (len > 64)
			len = 64;
		for (i = 0; i < len; i++) {
			printf("%02x", ((uint8_t *)_req->buf)[i]);
			if ((i & 7) == 7)
				printf(" ");
		}
		printf("\n");
	}
#endif

	if (list_empty(&ep->queue) && !ep->stopped) {

		if (ep_num == 0) {
			/* EP0 */
			list_add_tail(&req->queue, &ep->queue);
			cvi_ep0_kick(dev, ep);
			req = 0;

		} else if (ep_is_in(ep)) {
			gintsts = cvi_uncached_read32(&reg->gintsts);
			cvidbg_cond(DEBUG_IN_EP,
				     "%s: ep_is_in, CVI_UDC_OTG_GINTSTS=0x%x\n",
				     __func__, gintsts);

			setdma_tx(ep, req);
		} else {
			gintsts = cvi_uncached_read32(&reg->gintsts);
			cvidbg_cond(DEBUG_OUT_EP != 0,
				     "%s:ep_is_out, CVI_UDC_OTG_GINTSTS=0x%x\n",
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
int cvi_write_fifo_ep0(struct cvi_ep *ep, struct cvi_request *req)
{
	uint32_t max;
	unsigned int count;
	int is_last;

	max = ep_maxpacket(ep);

	cvidbg_cond(DEBUG_EP0 != 0, "%s: max = %d\n", __func__, max);

	count = setdma_tx(ep, req);

	/* last packet is usually short (or a zlp) */
	if (count != max)
		is_last = 1;
	else {
		if ((req->req.length != req->req.actual + count) || req->req.zero)
			is_last = 0;
		else
			is_last = 1;
	}

	cvidbg_cond(DEBUG_EP0 != 0,
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

static int cvi_fifo_read(struct cvi_ep *ep, uintptr_t *cp, int max)
{
	cvi_cache_invalidate((uintptr_t)cp, ROUND(max, CONFIG_SYS_CACHELINE_SIZE));

	cvidbg_cond(DEBUG_EP0 != 0,
		     "%s: bytes=%d, ep_index=%d 0x%p\n", __func__,
		     max, ep_index(ep), cp);

	return max;
}

/**
 * cvi_set_address - set the USB address for this device
 * @address:
 *
 * Called from control endpoint function
 * after it decodes a set address setup packet.
 */
void cvi_set_address(struct cvi_udc *dev, unsigned char address)
{
	struct cvi_usbotg_reg *reg = dev->reg;

	uint32_t ctrl = cvi_uncached_read32(&reg->dcfg);

	cvi_uncached_write32(DEVICE_ADDRESS(address) | ctrl, &reg->dcfg);

	cvi_udc_ep0_zlp(dev);

	cvidbg_cond(DEBUG_EP0 != 0,
		     "%s: USB OTG 2.0 Device address=%d, DCFG=0x%x\n",
		     __func__, address, cvi_uncached_read32(&reg->dcfg));

	dev->usb_address = address;
	dev->connected = 1;
}

static inline void cvi_udc_ep0_set_stall(struct cvi_ep *ep, uint32_t is_in)
{
	struct cvi_udc *dev = ep->dev;
	struct cvi_usbotg_reg *reg = dev->reg;
	uint32_t		ep_ctrl = 0;

	cvi_log_write(0x087087, is_in, 0, 0, 0);

	if (is_in)
		ep_ctrl = cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl);
	else
		ep_ctrl = cvi_uncached_read32(&reg->out_endp[EP0_CON].doepctl);

	/* set the disable and stall bits */
	//if (ep_ctrl & DEPCTL_EPENA)
	//	ep_ctrl |= DEPCTL_EPDIS;

	ep_ctrl |= DEPCTL_STALL;

	if (is_in) {
		cvi_uncached_write32(ep_ctrl, &reg->in_endp[EP0_CON].diepctl);
		cvidbg_cond(DEBUG_EP0 != 0,
			     "%s: set ep%d stall, DIEPCTL0 = 0x%p\n",
			     __func__, ep_index(ep), &reg->in_endp[EP0_CON].diepctl);
	} else {
		cvi_uncached_write32(ep_ctrl, &reg->out_endp[EP0_CON].doepctl);
		cvidbg_cond(DEBUG_EP0 != 0,
			     "%s: set ep%d stall, DOEPCTL0 = 0x%p\n",
			     __func__, ep_index(ep), &reg->out_endp[EP0_CON].doepctl);
	}

	/*
	 * The application can only set this bit, and the core clears it,
	 * when a SETUP token is received for this endpoint
	 */
	dev->ep0state = WAIT_FOR_SETUP;

	cvi_udc_pre_setup(dev);
}

void cvi_ep0_read(struct cvi_udc *dev)
{
	struct cvi_request *req;
	struct cvi_ep *ep = &dev->ep[0];

	if (!list_empty(&ep->queue)) {
		req = list_entry(ep->queue.next, struct cvi_request, queue);

	} else {
		cvidbg("%s: ---> BUG\n", __func__);
		return;
	}

	cvidbg_cond(DEBUG_EP0 != 0,
		     "%s: req = %p, req.length = 0x%x, req.actual = 0x%x\n",
		     __func__, req, req->req.length, req->req.actual);

	if (req->req.length == 0) {
		/* zlp for Set_configuration, Set_interface,
		 * or Bulk-Only mass storge reset
		 */

		ep->len = 0;
		cvi_udc_ep0_zlp(dev);

		cvidbg_cond(DEBUG_EP0 != 0,
			     "%s: req.length = 0, bRequest = %d\n",
			     __func__, dev->usb_ctrl->bRequest);
		return;
	}

	setdma_rx(ep, req);
}

/*
 * DATA_STATE_XMIT
 */
int cvi_ep0_write(struct cvi_udc *dev)
{
	struct cvi_request *req;
	struct cvi_ep *ep = &dev->ep[0];
	int ret, need_zlp = 0;

	if (list_empty(&ep->queue))
		req = 0;
	else
		req = list_entry(ep->queue.next, struct cvi_request, queue);

	if (!req) {
		cvidbg_cond(DEBUG_EP0 != 0, "%s: NULL REQ\n", __func__);
		return 0;
	}

	cvidbg_cond(DEBUG_EP0 != 0,
		     "%s: req = %p, req.length = 0x%x, req.actual = 0x%x\n",
		     __func__, req, req->req.length, req->req.actual);

	ret = cvi_write_fifo_ep0(ep, req);

	if ((ret == 1) && !need_zlp) {
		/* Last packet */
		dev->ep0state = WAIT_FOR_COMPLETE;
		cvidbg_cond(DEBUG_EP0 != 0,
			     "%s: finished, waiting for status\n", __func__);

	} else {
		dev->ep0state = DATA_STATE_XMIT;
		cvidbg_cond(DEBUG_EP0 != 0,
			     "%s: not finished\n", __func__);
	}

	return 1;
}

static int cvi_udc_get_status(struct cvi_udc *dev, CH9_UsbSetup *crq)
{
	uint8_t ep_num = cvi_phy_to_log_ep(crq->wIndex & 0x7F, !!(crq->wIndex & 0x80));
	uint16_t g_status = 0;
	uint32_t ep_ctrl;
	struct cvi_usbotg_reg *reg = dev->reg;

	cvidbg_cond(DEBUG_SETUP != 0,
		     "%s: *** USB_REQ_GET_STATUS\n", __func__);
	printf("crq->brequest:0x%x\n", crq->bmRequestType & CH9_REQ_RECIPIENT_MASK);
	switch (crq->bmRequestType & CH9_REQ_RECIPIENT_MASK) {
	case CH9_USB_REQ_RECIPIENT_INTERFACE:
		g_status = 0;
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tGET_STATUS:CH9_USB_REQ_RECIPIENT_INTERFACE, g_stauts = %d\n",
			     g_status);
		break;

	case CH9_USB_REQ_RECIPIENT_DEVICE:
		g_status = 0x1; /* Self powered */
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tGET_STATUS: CH9_USB_REQ_RECIPIENT_DEVICE, g_stauts = %d\n",
			     g_status);
		break;

	case CH9_USB_REQ_RECIPIENT_ENDPOINT:
		if (crq->wLength > 2) {
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tGET_STATUS:Not support EP or wLength\n");
			return 1;
		}

		g_status = dev->ep[ep_num].stopped;
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tGET_STATUS: CH9_USB_REQ_RECIPIENT_ENDPOINT, g_stauts = %d\n",
			     g_status);

		break;

	default:
		return 1;
	}

	memcpy(dev->usb_ctrl, &g_status, sizeof(g_status));

	cvi_cache_flush((unsigned long)dev->usb_ctrl, ROUND(sizeof(g_status), CONFIG_SYS_CACHELINE_SIZE));

	cvi_uncached_write32(dev->usb_ctrl_dma_addr, &reg->in_endp[EP0_CON].diepdma);
	cvi_uncached_write32(DIEPT_SIZ_PKT_CNT(1) | DIEPT_SIZ_XFER_SIZE(2),
			      &reg->in_endp[EP0_CON].dieptsiz);

	ep_ctrl = cvi_uncached_read32(&reg->in_endp[EP0_CON].diepctl);
	ep_ctrl &= ~(1 << 30);
	cvi_uncached_write32(ep_ctrl | DEPCTL_EPENA | DEPCTL_CNAK,
			      &reg->in_endp[EP0_CON].diepctl);
	dev->ep0state = WAIT_FOR_NULL_COMPLETE;

	return 0;
}

void cvi_udc_set_nak(struct cvi_ep *ep)
{
	uint8_t		ep_num;
	uint32_t		ep_ctrl = 0;
	struct cvi_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);
	cvidbg("%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = cvi_uncached_read32(&reg->in_endp[ep_num].diepctl);
		ep_ctrl |= DEPCTL_SNAK;
		cvi_uncached_write32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		cvidbg("%s: set NAK, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, cvi_uncached_read32(&reg->in_endp[ep_num].diepctl));
	} else {
		ep_ctrl = cvi_uncached_read32(&reg->out_endp[ep_num].doepctl);
		ep_ctrl |= DEPCTL_SNAK;
		cvi_uncached_write32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		cvidbg("%s: set NAK, DOEPCTL%d = 0x%x\n",
			__func__, ep_num, cvi_uncached_read32(&reg->out_endp[ep_num].doepctl));
	}
}

static void cvi_udc_ep_set_stall(struct cvi_ep *ep)
{
	uint8_t		ep_num;
	uint32_t		ep_ctrl = 0;
	struct cvi_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);
	cvidbg("%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = cvi_uncached_read32(&reg->in_endp[ep_num].diepctl);

		/* set the disable and stall bits */
		if (ep_ctrl & DEPCTL_EPENA)
			ep_ctrl |= DEPCTL_EPDIS;

		ep_ctrl |= DEPCTL_STALL;

		cvi_uncached_write32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		cvidbg("%s: set stall, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, cvi_uncached_read32(&reg->in_endp[ep_num].diepctl));

	} else {
		ep_ctrl = cvi_uncached_read32(&reg->out_endp[ep_num].doepctl);

		/* set the stall bit */
		ep_ctrl |= DEPCTL_STALL;

		cvi_uncached_write32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		cvidbg("%s: set stall, DOEPCTL%d = 0x%x\n",
			__func__, ep_num, cvi_uncached_read32(&reg->out_endp[ep_num].doepctl));
	}
}

static void cvi_udc_ep_clear_stall(struct cvi_ep *ep)
{
	uint8_t		ep_num;
	uint32_t		ep_ctrl = 0;
	struct cvi_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);
	cvidbg("%s: ep_num = %d, ep_type = %d\n", __func__, ep_num, ep->ep_type);

	if (ep_is_in(ep)) {
		ep_ctrl = cvi_uncached_read32(&reg->in_endp[ep_num].diepctl);

		/* clear stall bit */
		ep_ctrl &= ~DEPCTL_STALL;

		/*
		 * USB Spec 9.4.5: For endpoints using data toggle, regardless
		 * of whether an endpoint has the Halt feature set, a
		 * ClearFeature(ENDPOINT_HALT) request always results in the
		 * data toggle being reinitialized to DATA0.
		 */
		if (ep->bmAttributes == CH9_USB_EP_INTERRUPT || ep->bmAttributes == CH9_USB_EP_BULK) {
			ep_ctrl |= DEPCTL_SETD0PID; /* DATA0 */
		}

		cvi_uncached_write32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
		cvidbg("%s: cleared stall, DIEPCTL%d = 0x%x\n",
			__func__, ep_num, cvi_uncached_read32(&reg->in_endp[ep_num].diepctl));

	} else {
		ep_ctrl = cvi_uncached_read32(&reg->out_endp[ep_num].doepctl);

		/* clear stall bit */
		ep_ctrl &= ~DEPCTL_STALL;

		if (ep->bmAttributes == CH9_USB_EP_INTERRUPT || ep->bmAttributes == CH9_USB_EP_BULK) {
			ep_ctrl |= DEPCTL_SETD0PID; /* DATA0 */
		}

		cvi_uncached_write32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
		cvidbg("%s: cleared stall, DOEPCTL%d = 0x%x\n",
			__func__, ep_num, cvi_uncached_read32(&reg->out_endp[ep_num].doepctl));
	}
}

int cvi_udc_set_halt(struct usb_ep *_ep, int value)
{
	struct cvi_ep	*ep;
	struct cvi_udc	*dev;
	uint8_t		ep_num;

	ep = container_of(_ep, struct cvi_ep, ep);
	ep_num = ep_index(ep);

	if (!_ep || !ep->desc || ep_num == EP0_CON ||
	    ep->desc->bmAttributes == CH9_USB_EP_ISOCHRONOUS) {
		cvidbg("%s: %s bad ep or descriptor\n", __func__, ep->ep.name);
		return -EINVAL;
	}

	/* Attempt to halt IN ep will fail if any transfer requests
	 * are still queue
	 */
	if (value && ep_is_in(ep) && !list_empty(&ep->queue)) {
		cvidbg("%s: %s queue not empty, req = %p\n",
			__func__, ep->ep.name,
			list_entry(ep->queue.next, struct cvi_request, queue));

		return -EAGAIN;
	}

	dev = ep->dev;
	cvidbg("%s: ep_num = %d, value = %d\n", __func__, ep_num, value);

	if (value == 0) {
		ep->stopped = 0;
		cvi_udc_ep_clear_stall(ep);
	} else {
		if (ep_num == 0)
			dev->ep0state = WAIT_FOR_SETUP;

		ep->stopped = 1;
		cvi_udc_ep_set_stall(ep);
	}

	return 0;
}

void cvi_udc_ep_activate(struct cvi_ep *ep)
{
	uint8_t ep_num;
	uint32_t ep_ctrl = 0, daintmsk = 0;
	struct cvi_usbotg_reg *reg = ep->dev->reg;

	ep_num = ep_index(ep);

	/* Read DEPCTLn register */
	if (ep_is_in(ep)) {
		ep_ctrl = cvi_uncached_read32(&reg->in_endp[ep_num].diepctl);
		daintmsk = 1 << ep_num;
	} else {
		ep_ctrl = cvi_uncached_read32(&reg->out_endp[ep_num].doepctl);
		daintmsk = (1 << ep_num) << DAINT_OUT_BIT;
	}

	cvidbg("%s: EPCTRL%d = 0x%x, ep_is_in = %d\n",
		__func__, ep_num, ep_ctrl, ep_is_in(ep));

	/* If the EP is already active don't change the EP Control
	 * register.
	 */
	if (!(ep_ctrl & DEPCTL_USBACTEP)) {
		ep_ctrl = (ep_ctrl & ~DEPCTL_TYPE_MASK) |
			(ep->bmAttributes << DEPCTL_TYPE_BIT);
		ep_ctrl = (ep_ctrl & ~DEPCTL_MPS_MASK) |
			(ep->ep.maxpacket << DEPCTL_MPS_BIT);
		ep_ctrl |= (DEPCTL_SETD0PID | DEPCTL_USBACTEP | DEPCTL_SNAK);

		if (ep_is_in(ep)) {
			cvi_uncached_write32(ep_ctrl, &reg->in_endp[ep_num].diepctl);
			cvidbg("%s: USB Ative EP%d, DIEPCTRL%d = 0x%x\n",
				__func__, ep_num, ep_num,
				cvi_uncached_read32(&reg->in_endp[ep_num].diepctl));
		} else {
			cvi_uncached_write32(ep_ctrl, &reg->out_endp[ep_num].doepctl);
			cvidbg("%s: USB Ative EP%d, DOEPCTRL%d = 0x%x\n",
				__func__, ep_num, ep_num,
				cvi_uncached_read32(&reg->out_endp[ep_num].doepctl));
		}
	}

	/* Unmask EP Interrtupt */
	cvi_uncached_write32(cvi_uncached_read32(&reg->daintmsk) | daintmsk, &reg->daintmsk);
	cvidbg("%s: DAINTMSK = 0x%x\n", __func__, cvi_uncached_read32(&reg->daintmsk));

}

static int cvi_udc_clear_feature(struct usb_ep *_ep)
{
	struct cvi_udc	*dev;
	struct cvi_ep	*ep;
	uint8_t		ep_num;
	CH9_UsbSetup *usb_ctrl;

	ep = container_of(_ep, struct cvi_ep, ep);
	ep_num = ep_index(ep);

	dev = ep->dev;
	usb_ctrl = dev->usb_ctrl;

	cvidbg_cond(DEBUG_SETUP != 0,
		     "%s: ep_num = %d, is_in = %d, clear_feature_flag = %d\n",
		     __func__, ep_num, ep_is_in(ep), dev->clear_feature_flag);

	if (usb_ctrl->wLength != 0) {
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tCLEAR_FEATURE: wLength is not zero.....\n");
		return 1;
	}

	switch (usb_ctrl->bmRequestType & CH9_REQ_RECIPIENT_MASK) {
	case CH9_USB_REQ_RECIPIENT_DEVICE:
		switch (usb_ctrl->wValue) {
		case CH9_USB_FS_DEVICE_REMOTE_WAKEUP:
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tOFF:USB_DEVICE_REMOTE_WAKEUP\n");
			break;

		case CH9_USB_FS_TEST_MODE:
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tCLEAR_FEATURE: USB_DEVICE_TEST_MODE\n");
			/** @todo Add CLEAR_FEATURE for TEST modes. */
			break;
		}

		cvi_udc_ep0_zlp(dev);
		break;

	case CH9_USB_REQ_RECIPIENT_ENDPOINT:
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tCLEAR_FEATURE:CH9_USB_REQ_RECIPIENT_ENDPOINT, wValue = %d\n",
			     usb_ctrl->wValue);

		if (usb_ctrl->wValue == CH9_USB_FS_ENDPOINT_HALT) {
			if (ep_num == 0) {
				cvi_udc_ep0_set_stall(ep, 1);
				return 0;
			}

			cvi_udc_ep0_zlp(dev);

			cvi_udc_ep_clear_stall(ep);
			cvi_udc_ep_activate(ep);
			ep->stopped = 0;

			dev->clear_feature_num = ep_num;
			dev->clear_feature_flag = 1;
		}
		break;
	}

	return 0;
}

static int cvi_udc_set_feature(struct usb_ep *_ep)
{
	struct cvi_udc	*dev;
	struct cvi_ep	*ep;
	uint8_t		ep_num;
	CH9_UsbSetup *usb_ctrl;

	ep = container_of(_ep, struct cvi_ep, ep);
	ep_num = ep_index(ep);
	dev = ep->dev;
	usb_ctrl = dev->usb_ctrl;

	cvidbg_cond(DEBUG_SETUP != 0,
		     "%s: *** USB_REQ_SET_FEATURE , ep_num = %d\n",
		      __func__, ep_num);

	if (usb_ctrl->wLength != 0) {
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tSET_FEATURE: wLength is not zero.....\n");
		return 1;
	}

	switch (usb_ctrl->bmRequestType & CH9_REQ_RECIPIENT_MASK) {
	case CH9_USB_REQ_RECIPIENT_DEVICE:
		switch (usb_ctrl->wValue) {
		case CH9_USB_FS_DEVICE_REMOTE_WAKEUP:
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tSET_FEATURE:USB_DEVICE_REMOTE_WAKEUP\n");
			break;
		case CH9_USB_FS_B_HNP_ENABLE:
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tSET_FEATURE: USB_DEVICE_B_HNP_ENABLE\n");
			break;

		case CH9_USB_FS_A_HNP_SUPPORT:
			/* RH port supports HNP */
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tSET_FEATURE:USB_DEVICE_A_HNP_SUPPORT\n");
			break;

		case CH9_USB_FS_A_ALT_HNP_SUPPORT:
			/* other RH port does */
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tSET: USB_DEVICE_A_ALT_HNP_SUPPORT\n");
			break;
		case CH9_USB_FS_TEST_MODE:
			dev->test_mode = usb_ctrl->wIndex >> 8;
			break;
		}

		cvi_udc_ep0_zlp(dev);
		return 0;

	case CH9_USB_REQ_RECIPIENT_INTERFACE:
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tSET_FEATURE: CH9_USB_REQ_RECIPIENT_INTERFACE\n");
		break;

	case CH9_USB_REQ_RECIPIENT_ENDPOINT:
		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tSET_FEATURE: CH9_USB_REQ_RECIPIENT_ENDPOINT\n");
		if (usb_ctrl->wValue == CH9_USB_FS_ENDPOINT_HALT) {
			if (ep_num == 0) {
				cvi_udc_ep0_set_stall(ep, 1);
				return 0;
			}
			ep->stopped = 1;
			cvi_udc_ep_set_stall(ep);
		}

		cvi_udc_ep0_zlp(dev);
		return 0;
	}

	return 1;
}

/*
 * WAIT_FOR_SETUP (OUT_PKT_RDY)
 */
static void cvi_ep0_setup(struct cvi_udc *dev)
{
	struct cvi_ep *ep = &dev->ep[0];
	int i;
	uint8_t ep_num;
	CH9_UsbSetup *usb_ctrl = dev->usb_ctrl;
	uint8_t three_stage = 0;

	/* Nuke all previous transfers */
	cvi_nuke(ep, -EPROTO);

	/* read control req from fifo (8 bytes) */
	cvi_fifo_read(ep, (uintptr_t *)usb_ctrl, 8);

	cvidbg_cond(DEBUG_SETUP != 0,
		     "%s: bmRequestType = 0x%x(%s), bRequest = 0x%x, wLength = 0x%x, wValue = 0x%x, wIndex= 0x%x\n",
		     __func__, usb_ctrl->bmRequestType,
		     (usb_ctrl->bmRequestType & USB_DIR_IN) ? "IN" : "OUT",
		     usb_ctrl->bRequest,
		     usb_ctrl->wLength, usb_ctrl->wValue, usb_ctrl->wIndex);

	three_stage = usb_ctrl->wLength ? 1 : 0;
#ifdef DEBUG
	{
		int i, len = sizeof(*usb_ctrl);
		char *p = (char *)usb_ctrl;

		printf("pkt = ");
		for (i = 0; i < len; i++) {
			printf("%02x", ((uint8_t *)p)[i]);
			if ((i & 7) == 7)
				printf(" ");
		}
		printf("\n");
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
			cvi_log_write(0xBBBBB, 1, usb_ctrl->wValue, 0, 0);
			cvidbg_cond(DEBUG_SETUP != 0,
				     "%s: *** USB_REQ_SET_ADDRESS (%d)\n",
				     __func__, usb_ctrl->wValue);
			if (usb_ctrl->bmRequestType
				!= (CH9_USB_REQ_TYPE_STANDARD | CH9_USB_REQ_RECIPIENT_DEVICE))
				break;

			cvi_set_address(dev, usb_ctrl->wValue);
			return;

		case CH9_USB_REQ_SET_CONFIGURATION:
			cvi_log_write(0xBBBBB, 2, usb_ctrl->wValue, 0, 0);
			cvidbg_cond(DEBUG_SETUP != 0,
				     "=====================================\n");
			cvidbg_cond(DEBUG_SETUP != 0,
				     "%s: USB_REQ_SET_CONFIGURATION (%d)\n",
				     __func__, usb_ctrl->wValue);

			break;

		case CH9_USB_REQ_GET_DESCRIPTOR:
			cvi_log_write(0xBBBBB, 3, 0, 0, 0);
			cvidbg_cond(DEBUG_SETUP != 0,
				     "%s: *** USB_REQ_GET_DESCRIPTOR\n",
				     __func__);
			break;

		case CH9_USB_REQ_SET_INTERFACE:
			cvi_log_write(0xBBBBB, 4, 0, 0, 0);
			cvidbg_cond(DEBUG_SETUP != 0,
				     "%s: *** USB_REQ_SET_INTERFACE (%d)\n",
				     __func__, usb_ctrl->wValue);

			break;

		case CH9_USB_REQ_GET_CONFIGURATION:
			cvi_log_write(0xBBBBB, 5, 0, 0, 0);
			cvidbg_cond(DEBUG_SETUP != 0,
				     "%s: *** USB_REQ_GET_CONFIGURATION\n",
				     __func__);
			break;

		case CH9_USB_REQ_GET_STATUS:
			cvi_log_write(0xBBBBB, 6, 0, 0, 0);
			if (!cvi_udc_get_status(dev, usb_ctrl))
				return;

			break;

		case CH9_USB_REQ_CLEAR_FEATURE:
			cvi_log_write(0xBBBBB, 7, 0, 0, 0);
			ep_num = cvi_phy_to_log_ep(usb_ctrl->wIndex & 0x7F, !!(usb_ctrl->wIndex & 0x80));

			if (!cvi_udc_clear_feature(&dev->ep[ep_num].ep))
				return;

			break;

		case CH9_USB_REQ_SET_FEATURE:
			cvi_log_write(0xBBBBB, 8, 0, 0, 0);
			ep_num = cvi_phy_to_log_ep(usb_ctrl->wIndex & 0x7F, !!(usb_ctrl->wIndex & 0x80));

			if (!cvi_udc_set_feature(&dev->ep[ep_num].ep))
				return;

			break;

		default:
			cvi_log_write(0xBBBBB, 9, 0, 0, 0);
			cvidbg_cond(DEBUG_SETUP != 0,
				     "%s: *** Default of usb_ctrl->bRequest=0x%x happened.\n",
				     __func__, usb_ctrl->bRequest);
			break;
		}
	}

	if (dev->driver) {
		/* device-2-host (IN) or no data setup command,
		 * process immediately
		 */
		cvidbg_cond(DEBUG_SETUP != 0,
			     "%s:usb_ctrlreq will be passed to fsg_setup()\n",
			     __func__);

		i = dev->driver->setup(&dev->gadget, usb_ctrl);

		if (i < 0) {
			uint32_t dir = (usb_ctrl->wLength == 0) ? 1 : ep_is_in(ep);
			/* setup processing failed, force stall */
			cvi_udc_ep0_set_stall(ep, dir);
			dev->ep0state = WAIT_FOR_SETUP;

			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tdev->driver->setup failed (%d), bRequest = %d\n",
				     i, usb_ctrl->bRequest);
			return;

		} else if (dev->req_pending) {
			dev->req_pending = 0;
			cvidbg_cond(DEBUG_SETUP != 0,
				     "\tdev->req_pending...\n");
		}

		cvidbg_cond(DEBUG_SETUP != 0,
			     "\tep0state = %s\n", state_names[dev->ep0state]);

	}

	if (!three_stage)
		cvi_udc_ep0_zlp(dev);
}

/*
 * handle ep0 interrupt
 */
void cvi_handle_ep0(struct cvi_udc *dev)
{
	if (dev->ep0state == WAIT_FOR_SETUP) {
		cvidbg_cond(DEBUG_OUT_EP != 0,
			     "%s: WAIT_FOR_SETUP\n", __func__);
		cvi_ep0_setup(dev);

	} else {
		cvidbg_cond(DEBUG_OUT_EP != 0,
			     "%s: strange state!!(state = %s)\n",
			     __func__, state_names[dev->ep0state]);
	}
}

void cvi_ep0_kick(struct cvi_udc *dev, struct cvi_ep *ep)
{
	cvidbg_cond(DEBUG_EP0 != 0,
		     "%s: ep_is_in = %d\n", __func__, ep_is_in(ep));
	if (ep_is_in(ep)) {
		dev->ep0state = DATA_STATE_XMIT;
		cvi_ep0_write(dev);

	} else {
		dev->ep0state = DATA_STATE_RECV;
		cvi_ep0_read(dev);
	}
}
