/*
 * drivers/usb/gadget/dwc2_udc.h
 * Designware DWC2 on-chip full/high speed USB device controllers
 * Copyright (C) 2005 for Samsung Electronics
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __DWC2_USB_GADGET
#define __DWC2_USB_GADGET

#include "dwc2_udc_otg_regs.h"

#define _DEBUG	0

#define dwc2dbg_cond(cond, fmt, args...)			\
	do {						\
		if (cond)				\
			printf(fmt, ##args);	\
	} while (0)

#define dwc2dbg(fmt, args...)			\
	dwc2dbg_cond(_DEBUG, fmt, ##args)

#define DEBUG_SETUP 0
#define DEBUG_EP0 0
#define DEBUG_ISR 0
#define DEBUG_OUT_EP 0
#define DEBUG_IN_EP 0

#define PHY0_SLEEP              (1 << 5)

#ifndef CONFIG_SYS_CACHELINE_SIZE
#define CONFIG_SYS_CACHELINE_SIZE       64
#endif

#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

#define ROUND(a, b)		(((a) + (b) - 1) & ~((b) - 1))

#define min_t(type, x, y) ({			\
	type __min1 = (x);			\
	type __min2 = (y);			\
	__min1 < __min2 ? __min1 : __min2; })

#define max_t(type, x, y) ({			\
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1 : __max2; })

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })

#define OTG_DMA_MODE		1

#define EP0_CON		0
#define EP_MASK		0xF

struct dwc2_plat_otg_data {
	void		*handler;
	unsigned int	size;
	int		phy_of_node;
	int		(*phy_control)(int on);
	uintptr_t	regs_phy;
	uintptr_t	regs_otg;
	unsigned int    usb_phy_ctrl;
	unsigned int    usb_flags;
	unsigned int	usb_gusbcfg;
	unsigned int	rx_fifo_sz;
	unsigned int	np_tx_fifo_sz;
	unsigned int	tx_fifo_sz;
	void            *ctrl_req;
};

struct dwc2_drv_obj {
	struct usb_gadget		*gadget;
	struct dwc2_plat_otg_data	plat;
};

int dwc2_udc_probe(struct dwc2_plat_otg_data *pdata);
void dwc2_udc_ep_activate(struct dwc2_ep *ep);
int dwc2_udc_irq(int irq, void *_dev);
int dwc2_queue(struct usb_ep *_ep, struct usb_request *_req);
void dwc2_ep0_read(struct dwc2_udc *dev);
void dwc2_ep0_kick(struct dwc2_udc *dev, struct dwc2_ep *ep);
void dwc2_handle_ep0(struct dwc2_udc *dev);
int dwc2_ep0_write(struct dwc2_udc *dev);
int dwc2_write_fifo_ep0(struct dwc2_ep *ep, struct dwc2_request *req);
void dwc2_set_address(struct dwc2_udc *dev, unsigned char address);
int dwc2_udc_set_halt(struct usb_ep *_ep, int value);
void dwc2_udc_set_nak(struct dwc2_ep *ep);
void dwc2_done(struct dwc2_ep *ep, struct dwc2_request *req, int status);
void dwc2_nuke(struct dwc2_ep *ep, int status);
void dwc2_reconfig_usbd(struct dwc2_udc *dev, int disconnect);
const char *dwc2_get_ep0_name(void);

#endif	/* __DWC2_USB_GADGET */
