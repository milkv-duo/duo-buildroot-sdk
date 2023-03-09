# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the usb common parts.
#

obj-$(CONFIG_USB_COMMON)	  += usb-common.o
usb-common-y			  += common.o
usb-common-$(CONFIG_TRACING)	  += debug.o
usb-common-$(CONFIG_USB_LED_TRIG) += led.o

obj-$(CONFIG_USB_CONN_GPIO)	+= usb-conn-gpio.o
ifeq ($(CONFIG_CVITEK_USB_LEGACY),y)
usbotg-y		:= usb-otg.o usb-otg-fsm.o
obj-$(CONFIG_USB_OTG)	+= usbotg.o
else
obj-$(CONFIG_USB_OTG_FSM) += usb-otg-fsm.o
endif
obj-$(CONFIG_USB_ULPI_BUS)	+= ulpi.o
