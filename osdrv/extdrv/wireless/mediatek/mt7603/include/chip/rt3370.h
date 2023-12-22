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
	rt3070.h
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __RT3370_H__
#define __RT3370_H__

#ifdef RT3370


#ifndef RTMP_USB_SUPPORT
#error "For RT3070, you should define the compile flag -DRTMP_USB_SUPPORT"
#endif

#ifndef RTMP_MAC_USB
#error "For RT3070, you should define the compile flag -DRTMP_MAC_USB"
#endif

#ifndef RTMP_RF_RW_SUPPORT
#error "For RT3070, you should define the compile flag -DRTMP_RF_RW_SUPPORT"
#endif

#ifndef RT33xx
#error "For RT3370, you should define the compile flag -DRT33xx"
#endif

#ifndef RT30xx
#error "For RT3070, you should define the compile flag -DRT30xx"
#endif


#include "chip/rt30xx.h"
#include "chip/rt33xx.h"

extern REG_PAIR RT3370_BBPRegTable[];
extern UCHAR RT3370_NUM_BBP_REG_PARMS;


/* */
/* Device ID & Vendor ID, these values should match EEPROM value */
/* */

#endif /* RT3370 */

#endif /*__RT3370_H__ */

