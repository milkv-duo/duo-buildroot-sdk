
#ifndef __RT2870_H__
#define __RT2870_H__

#ifdef RT2870

#ifndef RTMP_USB_SUPPORT
#error "For RT2870, you should define the compile flag -DRTMP_USB_SUPPORT"
#endif

#ifndef RTMP_MAC_USB
#error "For RT2870, you should define the compile flag -DRTMP_MAC_USB"
#endif

#include "rtmp_type.h"

#endif /* RT2870 */
#endif /*__RT2870_H__ */

