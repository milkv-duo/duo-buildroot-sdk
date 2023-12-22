/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
    rtusb_dev_id.c

    Abstract:

    Revision History:
    Who        When          What
    ---------  ----------    ----------------------------------------------
 */

#define RTMP_MODULE_OS

#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"

#ifdef DPA_S
USB_DEVICE_ID rtusb_ll_dev_id[] = {
		{USB_DEVICE(0x148F, 0x2870)},
		{}
};


USB_DEVICE_ID rtusb_extern_dev_id[] = {
		{USB_DEVICE(0x148F, 0x3572)},
		{USB_DEVICE(0x04E8, 0x2018)},
		{ }
};
#endif /* DPA_S */

/* module table */
USB_DEVICE_ID rtusb_dev_id[] = {
#ifdef RT2870
#ifndef DPA_T
	{USB_DEVICE(0x148F,0x2770)}, /* Ralink */
	{USB_DEVICE(0x148F,0x2870)}, /* Ralink */
	{USB_DEVICE(0x07B8,0x2870)}, /* AboCom */
	{USB_DEVICE(0x07B8,0x2770)}, /* AboCom */
	{USB_DEVICE(0x0DF6,0x0039)}, /* Sitecom 2770 */
	{USB_DEVICE(0x0DF6,0x003F)}, /* Sitecom 2770 */
	{USB_DEVICE(0x083A,0x7512)}, /* Arcadyan 2770 */
	{USB_DEVICE(0x0789,0x0162)}, /* Logitec 2870 */
	{USB_DEVICE(0x0789,0x0163)}, /* Logitec 2870 */
	{USB_DEVICE(0x0789,0x0164)}, /* Logitec 2870 */
	{USB_DEVICE(0x177f,0x0302)}, /* lsusb */
	{USB_DEVICE(0x0B05,0x1731)}, /* Asus */
	{USB_DEVICE(0x0B05,0x1732)}, /* Asus */
	{USB_DEVICE(0x0B05,0x1742)}, /* Asus */
	{USB_DEVICE(0x0DF6,0x0017)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x002B)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x002C)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x002D)}, /* Sitecom */
	{USB_DEVICE(0x14B2,0x3C06)}, /* Conceptronic */
	{USB_DEVICE(0x14B2,0x3C28)}, /* Conceptronic */
	{USB_DEVICE(0x2019,0xED06)}, /* Planex Communications, Inc. */
	{USB_DEVICE(0x07D1,0x3C09)}, /* D-Link */
	{USB_DEVICE(0x07D1,0x3C11)}, /* D-Link */
	{USB_DEVICE(0x14B2,0x3C07)}, /* AL */
	{USB_DEVICE(0x050D,0x8053)}, /* Belkin */
	{USB_DEVICE(0x14B2,0x3C23)}, /* Airlink */
	{USB_DEVICE(0x14B2,0x3C27)}, /* Airlink */
	{USB_DEVICE(0x07AA,0x002F)}, /* Corega */
	{USB_DEVICE(0x07AA,0x003C)}, /* Corega */
	{USB_DEVICE(0x07AA,0x003F)}, /* Corega */
	{USB_DEVICE(0x1044,0x800B)}, /* Gigabyte */
	{USB_DEVICE(0x15A9,0x0006)}, /* Sparklan */
	{USB_DEVICE(0x083A,0xB522)}, /* SMC */
	{USB_DEVICE(0x083A,0xA618)}, /* SMC */
	{USB_DEVICE(0x083A,0x8522)}, /* Arcadyan */
	{USB_DEVICE(0x083A,0x7522)}, /* Arcadyan */
	{USB_DEVICE(0x0CDE,0x0022)}, /* ZCOM */
	{USB_DEVICE(0x0586,0x3416)}, /* Zyxel */
	{USB_DEVICE(0x0CDE,0x0025)}, /* Zyxel */
	{USB_DEVICE(0x1740,0x9701)}, /* EnGenius */
	{USB_DEVICE(0x1740,0x9702)}, /* EnGenius */
	{USB_DEVICE(0x0471,0x200f)}, /* Philips */
	{USB_DEVICE(0x14B2,0x3C25)}, /* Draytek */
	{USB_DEVICE(0x13D3,0x3247)}, /* AzureWave */
	{USB_DEVICE(0x083A,0x6618)}, /* Accton */
	{USB_DEVICE(0x15c5,0x0008)}, /* Amit */
	{USB_DEVICE(0x0E66,0x0001)}, /* Hawking */
	{USB_DEVICE(0x0E66,0x0003)}, /* Hawking */
	{USB_DEVICE(0x129B,0x1828)}, /* Siemens */
	{USB_DEVICE(0x157E,0x300E)},	/* U-Media */
	{USB_DEVICE(0x050d,0x805c)},
	{USB_DEVICE(0x050d,0x815c)},
	{USB_DEVICE(0x1482,0x3C09)}, /* Abocom*/
	{USB_DEVICE(0x14B2,0x3C09)}, /* Alpha */
	{USB_DEVICE(0x04E8,0x2018)}, /* samsung linkstick2 */
	{USB_DEVICE(0x1690,0x0740)}, /* Askey */
	{USB_DEVICE(0x5A57,0x0280)}, /* Zinwell */
	{USB_DEVICE(0x5A57,0x0282)}, /* Zinwell */
	{USB_DEVICE(0x7392,0x7718)},
	{USB_DEVICE(0x7392,0x7717)},
	{USB_DEVICE(0x1737,0x0070)}, /* Linksys WUSB100 */
	{USB_DEVICE(0x1737,0x0071)}, /* Linksys WUSB600N */
	{USB_DEVICE(0x0411,0x00e8)}, /* Buffalo WLI-UC-G300N*/
	{USB_DEVICE(0x050d,0x815c)}, /* Belkin F5D8053 */
	{USB_DEVICE(0x100D,0x9031)}, /* Motorola 2770 */
	{USB_DEVICE(0x0DB0,0x6899)},
#endif /* !DPA_T */
#endif /* RT2870*/
#ifdef RT2070
	{USB_DEVICE(0x148F,0x2070)}, /* Ralink 2070 */
#endif /* RT2070 */
#ifdef RT3070
	{USB_DEVICE(0x148F,0x3070)}, /* Ralink 3070 */
	{USB_DEVICE(0x148F,0x3071)}, /* Ralink 3071 */
	{USB_DEVICE(0x148F,0x3072)}, /* Ralink 3072 */
	{USB_DEVICE(0x0DB0,0x3820)}, /* Ralink 3070 */
	{USB_DEVICE(0x0DB0,0x871C)}, /* Ralink 3070 */
	{USB_DEVICE(0x0DB0,0x822C)}, /* Ralink 3070 */
	{USB_DEVICE(0x0DB0,0x871B)}, /* Ralink 3070 */
	{USB_DEVICE(0x0DB0,0x822B)}, /* Ralink 3070 */
	{USB_DEVICE(0x0DF6,0x003E)}, /* Sitecom 3070 */
	{USB_DEVICE(0x0DF6,0x0042)}, /* Sitecom 3072 */
	{USB_DEVICE(0x0DF6,0x0048)}, /* Sitecom 3070 */
	{USB_DEVICE(0x0DF6,0x0047)}, /* Sitecom 3071 */
	{USB_DEVICE(0x0DF6,0x005F)}, /* Sitecom 3072 */
	{USB_DEVICE(0x14B2,0x3C12)}, /* AL 3070 */
	{USB_DEVICE(0x18C5,0x0012)}, /* Corega 3070 */
	{USB_DEVICE(0x083A,0x7511)}, /* Arcadyan 3070 */
	{USB_DEVICE(0x083A,0xA701)}, /* SMC 3070 */
	{USB_DEVICE(0x083A,0xA702)}, /* SMC 3072 */
	{USB_DEVICE(0x1740,0x9703)}, /* EnGenius 3070 */
	{USB_DEVICE(0x1740,0x9705)}, /* EnGenius 3071 */
	{USB_DEVICE(0x1740,0x9706)}, /* EnGenius 3072 */
	{USB_DEVICE(0x1740,0x9707)}, /* EnGenius 3070 */
	{USB_DEVICE(0x1740,0x9708)}, /* EnGenius 3071 */
	{USB_DEVICE(0x1740,0x9709)}, /* EnGenius 3072 */
	{USB_DEVICE(0x13D3,0x3273)}, /* AzureWave 3070*/
	{USB_DEVICE(0x13D3,0x3305)}, /* AzureWave 3070*/
	{USB_DEVICE(0x1044,0x800D)}, /* Gigabyte GN-WB32L 3070 */
	{USB_DEVICE(0x2019,0xAB25)}, /* Planex Communications, Inc. RT3070 */
	{USB_DEVICE(0x2019,0x5201)}, /* Planex Communications, Inc. RT8070 */
	{USB_DEVICE(0x07B8,0x3070)}, /* AboCom 3070 */
	{USB_DEVICE(0x07B8,0x3071)}, /* AboCom 3071 */
	{USB_DEVICE(0x07B8,0x3072)}, /* Abocom 3072 */
	{USB_DEVICE(0x7392,0x7711)}, /* Edimax 3070 */
	{USB_DEVICE(0x7392,0x4085)}, /* 2L Central Europe BV 8070 */
	{USB_DEVICE(0x1A32,0x0304)}, /* Quanta 3070 */
	{USB_DEVICE(0x1EDA,0x2012)}, /* AirTies 3070 */
	{USB_DEVICE(0x1EDA,0x2310)}, /* AirTies 3070 */
	{USB_DEVICE(0x07D1,0x3C0A)}, /* D-Link 3072 */
	{USB_DEVICE(0x07D1,0x3C0D)}, /* D-Link 3070 */
	{USB_DEVICE(0x07D1,0x3C0E)}, /* D-Link 3070 */
	{USB_DEVICE(0x07D1,0x3C0F)}, /* D-Link 3070 */
	{USB_DEVICE(0x07D1,0x3C16)}, /* D-Link 3070 */
	{USB_DEVICE(0x07D1,0x3C17)}, /* D-Link 8070 */
	{USB_DEVICE(0x1D4D,0x000C)}, /* Pegatron Corporation 3070 */
	{USB_DEVICE(0x1D4D,0x000E)}, /* Pegatron Corporation 3070 */
	{USB_DEVICE(0x1D4D,0x0011)}, /* Pegatron Corporation 3072 */
	{USB_DEVICE(0x5A57,0x5257)}, /* Zinwell 3070 */
	{USB_DEVICE(0x5A57,0x0283)}, /* Zinwell 3072 */
	{USB_DEVICE(0x04BB,0x0945)}, /* I-O DATA 3072 */
	{USB_DEVICE(0x04BB,0x0947)}, /* I-O DATA 3070 */
	{USB_DEVICE(0x04BB,0x0948)}, /* I-O DATA 3072 */
	{USB_DEVICE(0x203D,0x1480)}, /* Encore 3070 */
	{USB_DEVICE(0x20B8,0x8888)}, /* PARA INDUSTRIAL 3070 */
	{USB_DEVICE(0x0B05,0x1784)}, /* Asus 3072 */
	{USB_DEVICE(0x203D,0x14A9)}, /* Encore 3070*/
	{USB_DEVICE(0x0DB0,0x899A)}, /* MSI 3070*/
	{USB_DEVICE(0x0DB0,0x3870)}, /* MSI 3070*/
	{USB_DEVICE(0x0DB0,0x870A)}, /* MSI 3070*/
	{USB_DEVICE(0x0DB0,0x6899)}, /* MSI 3070 */
	{USB_DEVICE(0x0DB0,0x3822)}, /* MSI 3070 */
	{USB_DEVICE(0x0DB0,0x3871)}, /* MSI 3070 */
	{USB_DEVICE(0x0DB0,0x871A)}, /* MSI 3070 */
	{USB_DEVICE(0x0DB0,0x822A)}, /* MSI 3070 */
	{USB_DEVICE(0x0DB0,0x3821)}, /* Ralink 3070 */
	{USB_DEVICE(0x0DB0,0x821A)}, /* Ralink 3070 */
	{USB_DEVICE(0x5A57,0x0282)}, /* zintech 3072 */
	{USB_DEVICE(0x083A,0xA703)}, /* IO-MAGIC */
	{USB_DEVICE(0x13D3,0x3307)}, /* Azurewave */
	{USB_DEVICE(0x13D3,0x3321)}, /* Azurewave */
	{USB_DEVICE(0x13D3,0x3329)}, /* Azurewave */
	{USB_DEVICE(0x07FA,0x7712)}, /* Edimax */
	{USB_DEVICE(0x0789,0x0166)}, /* Edimax */
	{USB_DEVICE(0x0586,0x341A)}, /* Zyxel */
	{USB_DEVICE(0x0586,0x341E)}, /* Zyxel */
	{USB_DEVICE(0x0586,0x343E)}, /* Zyxel */
	{USB_DEVICE(0x1EDA,0x2012)}, /* Airties */
	{USB_DEVICE(0x1EDA,0x2210)}, /* Airties */
	{USB_DEVICE(0x2001,0x3C1B)}, /* Alpha */
	{USB_DEVICE(0x083A,0xB511)}, /* Panasonic */
#endif /* RT3070 */
#ifdef RT35xx
#ifndef DPA_T
	{USB_DEVICE(0x148F,0x3572)}, /* Ralink 3572 */
	{USB_DEVICE(0x1740,0x9801)}, /* EnGenius 3572 */
	{USB_DEVICE(0x0DF6,0x0041)}, /* Sitecom 3572 */
	{USB_DEVICE(0x0DF6,0x0042)},
	{USB_DEVICE(0x04BB,0x0944)}, /* I-O DATA 3572 */
	{USB_DEVICE(0x1690,0x0740)}, /* 3572 */
	{USB_DEVICE(0x1690,0x0744)}, /* 3572 */
	{USB_DEVICE(0x5A57,0x0284)}, /* Zinwell 3572 */
	{USB_DEVICE(0x167B,0x4001)}, /* 3572 */
	{USB_DEVICE(0x1690,0x0764)}, /* 3572 */ 
#endif /* !DPA_T */
	{USB_DEVICE(0x0930,0x0A07)}, /* TOSHIBA */
#ifndef DPA_T
	{USB_DEVICE(0x1690,0x0761)}, /* Askey */
	{USB_DEVICE(0x13B1,0x002F)}, /* Cisco LinkSys AE1000 */
	{USB_DEVICE(0x1737,0x0079)}, /* Cisco LinkSys WUSB600N */
	{USB_DEVICE(0x0DF6,0x0065)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x0066)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x0068)}, /* Sitecom */
#endif /* !DPA_T */
#endif /* RT35xx */
#ifdef RT3370
	{USB_DEVICE(0x148F,0x3370)}, /* Ralink 3370 */
	{USB_DEVICE(0x0DF6,0x0050)}, /* Sitecom 3370 */
	{USB_DEVICE(0x083A,0xB511)}, /* Panasonic 3370 */
	{USB_DEVICE(0x0471,0x20DD)}, /* Philips */
#endif /* RT3370*/
#ifdef RT3573
	{USB_DEVICE(0x148F,0x3573)}, /* Ralink 3573 */
	{USB_DEVICE(0x7392,0x7733)}, /* Edimax */
	{USB_DEVICE(0x0DF6,0X0067)}, /* Sitecom */
	{USB_DEVICE(0x1875,0x7733)}, /* Air Live */
	{USB_DEVICE(0x0E66,0x0021)}, /* Hawking */
	{USB_DEVICE(0x2019,0xED19)}, /* Planex */
#endif /* RT3573 */
#ifdef RT5370
	{USB_DEVICE(0x148F,0x5370)}, /* Ralink 5370 */
	{USB_DEVICE(0x148F,0x5372)}, /* Ralink 5372 */
	{USB_DEVICE(0x13D3,0x3365)}, /* Azurewave */
	{USB_DEVICE(0x13D3,0x3329)}, /* Azurewave */
	{USB_DEVICE(0x2001,0x3C15)}, /* Alpha */
	{USB_DEVICE(0x2001,0x3C19)}, /* DWA-125/A3 */
	{USB_DEVICE(0x2001,0x3C1C)}, /* GO-USB-N150 */
	{USB_DEVICE(0x2001,0x3C1D)}, /* DWA-123/B1 */
	{USB_DEVICE(0x043E,0x7A12)}, /* Arcadyan */
	{USB_DEVICE(0x043E,0x7A22)}, /* LG innotek */
	{USB_DEVICE(0x043E,0x7A32)}, /* Arcadyan */
	{USB_DEVICE(0x043E,0x7A42)}, /* LG innotek */
	{USB_DEVICE(0x04DA,0x1800)}, /* Panasonic */
	{USB_DEVICE(0x04DA,0x1801)}, /* Panasonic */
	{USB_DEVICE(0x04DA,0x23F6)},
	{USB_DEVICE(0x0471,0x2104)}, /* Philips, 5372 */
#endif /* RT5370 */
#ifdef RT5372
	{USB_DEVICE(0x148F,0x5372)}, /* Ralink 5372 */
	{USB_DEVICE(0x13D3,0x3365)}, /* Azurewave */
	{USB_DEVICE(0x0471,0x2104)}, /* Philips */
#endif /*  RT5372 */
#ifdef RT5572
#ifndef DPA_T
	{USB_DEVICE(0x148F,0x5572)}, /* Ralink 5572 */
	{USB_DEVICE(0x043E,0x7A32)}, /* Arcadyan */
	{USB_DEVICE(0x2001,0x3C1A)}, /* D-Link DWA-160B2 */
	{USB_DEVICE(0x043E,0x7A13)}, /* Proware */
#endif
#endif /* RT5572 */
#ifdef MT76x0
	{USB_DEVICE(0x148F,0x7610), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE(0x0E8D,0x7610), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE_AND_INTERFACE_INFO(0x0E8D, 0x7630, 0xff, 0x02, 0xff), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE_AND_INTERFACE_INFO(0x0E8D, 0x7630, 0xff, 0xff, 0xff), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE_AND_INTERFACE_INFO(0x0E8D, 0x7650, 0xff, 0x02, 0xff), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE_AND_INTERFACE_INFO(0x0E8D, 0x7650, 0xff, 0xff, 0xff), .driver_info = RLT_MAC_BASE},
#endif
#ifdef MT76x2
	{USB_DEVICE(0x0E8D, 0x7612), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE_AND_INTERFACE_INFO(0x0E8D, 0x7632, 0xff, 0xff, 0xff), .driver_info = RLT_MAC_BASE},
	{USB_DEVICE_AND_INTERFACE_INFO(0x0E8D, 0x7662, 0xff, 0xff, 0xff), .driver_info = RLT_MAC_BASE},
#endif
#ifdef MT7601U
	{USB_DEVICE(0x148f,0x6370)}, /* Ralink 6370 */
	{USB_DEVICE(0x148f,0x7601)}, /* MT 6370 */
#endif /* MT7601U */

#ifdef MT7603
	{USB_DEVICE(0x0E8D,0x7603)},
#endif
#ifdef MT7636
	{USB_DEVICE(0x0E8D,0x7606)},
#endif
	{ }/* Terminating entry */
};

INT const rtusb_usb_id_len = sizeof(rtusb_dev_id) / sizeof(USB_DEVICE_ID);
#ifdef DPA_S
INT const rtusb_extern_usb_id_len = sizeof(rtusb_extern_dev_id) / sizeof(USB_DEVICE_ID);
INT const rtusb_ll_usb_len = sizeof(rtusb_ll_dev_id) / sizeof(USB_DEVICE_ID);
#endif /* DPA_S */
MODULE_DEVICE_TABLE(usb, rtusb_dev_id);
#ifdef DPA_S
MODULE_DEVICE_TABLE(usb, rtusb_extern_dev_id);
MODULE_DEVICE_TABLE(usb, rtusb_ll_dev_id);
#endif /* DPA_S */
