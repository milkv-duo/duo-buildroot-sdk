#ifndef _DELAY_H
#define _DELAY_H

#include "FreeRTOS.h"
#include <sleep.h>

#ifndef udelay
#define udelay(us)		usleep(us)
#endif

/* use vTaskDelay to replace usleep api
 * usleep is a busy waiting api
 */
#ifndef mdelay
#define mdelay(ms)		usleep(ms * 1000)
/*#define mdelay(ms)  \
do { \
	if (ms < (configTICK_RATE_HZ / 1000)) \
		usleep(ms * 1000); \
	else \
	    vTaskDelay(pdMS_TO_TICKS(ms)); \
} while(0)
*/
#endif

#endif // end of _DELAY_H
