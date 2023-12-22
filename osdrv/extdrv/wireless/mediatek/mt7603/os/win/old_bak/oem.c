/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	oem.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Paul Lin	2003-05-30		Initial version
*/

/* */
/* Modify the strings to OEM vandor */
/* Be sure that total length may not exceed 32 */
/* */
char	NIC_VENDOR_DESC[] = "IEEE 802.11n Wireless Card.";
int		NIC_VENDOR_DESC_LEN = sizeof(NIC_VENDOR_DESC);

/* */
/* update the driver version number every time you release a new driver */
/* The high word is the major version. The low word is the minor version. */
/* Ex: driver version is 2.3.4.5 */
/*     NIC_VENDOR_DRIVER_VERSION = 0x00020003 */
/* */
unsigned long	NIC_VENDOR_DRIVER_VERSION = 0x00000000;

/* */
/* Use for Win98 & WinME */
/* */
unsigned short LINKNAME_STRING[] = L"\\DosDevices\\RT61";
unsigned short NTDEVICE_STRING[] = L"\\Device\\RT61";


/* */
/* Device ID & Vendor ID, these values should match EEPROM value */
/* */
/*unsigned short	NIC_PCI_DEVICE_ID = 0x0201; */
unsigned short  NIC2860_PCI_DEVICE_ID = 0x0601;
unsigned short  NIC2561Turbo_PCI_DEVICE_ID = 0x0301;
unsigned short  NIC2561_PCI_DEVICE_ID = 0x0302;
unsigned short	NIC_PCI_VENDOR_ID = 0x1814;

