/*
 * Designware CVI on-chip full/high speed USB device controllers
 * Copyright (C) 2005 for Samsung Electronics
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CVI_UDC_OTG_PRIV__
#define __CVI_UDC_OTG_PRIV__

#include <linux/errno.h>
#include <linux/sizes.h>
#include "cvi_ch9.h"
#include "cvi_drv_if.h"
#include "cvi_udc_otg_regs.h"

/*-------------------------------------------------------------------------*/
/* DMA bounce buffer size, 16K is enough even for mass storage */
#define DMA_BUFFER_SIZE	(16 * SZ_1K)

#define EP0_FIFO_SIZE		64
#define EP_FIFO_SIZE		512
#define EP_FIFO_SIZE2		1024
/* ep0-control, ep1in-bulk, ep2out-bulk, ep3in-int */
#define CVI_MAX_ENDPOINTS	4
#define CVI_MAX_HW_ENDPOINTS	8

#define WAIT_FOR_SETUP          0
#define DATA_STATE_XMIT         1
#define DATA_STATE_NEED_ZLP     2
#define WAIT_FOR_OUT_STATUS     3
#define DATA_STATE_RECV         4
#define WAIT_FOR_COMPLETE	5
#define WAIT_FOR_OUT_COMPLETE	6
#define WAIT_FOR_IN_COMPLETE	7
#define WAIT_FOR_NULL_COMPLETE	8

#define TEST_J_SEL		0x1
#define TEST_K_SEL		0x2
#define TEST_SE0_NAK_SEL	0x3
#define TEST_PACKET_SEL		0x4
#define TEST_FORCE_ENABLE_SEL	0x5

/* ************************************************************************* */
/* IO
 */

enum ep_type {
	ep_control, ep_bulk_in, ep_bulk_out, ep_interrupt
};

struct cvi_ep {
	struct usb_ep ep;	/* must be put here! */
	struct cvi_udc *dev;

	const CH9_UsbEndpointDescriptor *desc;
	struct list_head queue;
	unsigned long pio_irqs;
	int len;
	void *dma_buf;

	uint8_t stopped;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;

	enum ep_type ep_type;
	int fifo_num;
};

struct cvi_request {
	struct usb_request req;
	struct list_head queue;
};

struct cvi_udc {
	struct usb_gadget gadget;
	struct usb_gadget_driver *driver;

	void *pdata;

	int ep0state;
	struct cvi_ep ep[CVI_MAX_ENDPOINTS];

	unsigned char usb_address;

	unsigned req_pending:1, req_std:1;

	CH9_UsbSetup *usb_ctrl;
	dma_addr_t usb_ctrl_dma_addr;
	struct cvi_usbotg_reg *reg;
	unsigned int connected;
	uint8_t clear_feature_num;
	uint8_t clear_feature_flag;
	uint8_t test_mode;

};

#define ep_is_in(EP) (((EP)->bEndpointAddress & USB_DIR_IN) == USB_DIR_IN)
#define ep_index(EP) ((EP)->bEndpointAddress & 0xF)
#define ep_maxpacket(EP) ((EP)->ep.maxpacket)

void otg_phy_init(struct cvi_udc *dev);
void otg_phy_off(struct cvi_udc *dev);
void cvi_log_write(uint32_t tag, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4);
void set_trigger_cnt(int cnt);
uint8_t cvi_phy_to_log_ep(uint8_t phy_num, uint8_t dir);
void cvi_udc_pre_setup(struct cvi_udc *dev);
void cvi_disconnect(struct cvi_udc *dev);
void cvi_hsotg_set_bit(uint32_t *reg, uint32_t val);
void cvi_hsotg_clear_bit(uint32_t *reg, uint32_t vla);
int cvi_hsotg_wait_bit_set(uint32_t *reg, uint32_t bit, uint32_t timeout);

#endif	/* __CVI_UDC_OTG_PRIV__ */
