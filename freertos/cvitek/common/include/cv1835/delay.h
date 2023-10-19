#ifndef _DELAY_H
#define _DELAY_H

//#include <FreeRTOS_POSIX/unistd.h>

#ifndef udelay
#define udelay(us)		usleep(us)
#endif

#ifndef mdelay
#define mdelay(us)		usleep(us*1000)
#endif

#endif // end of _DELAY_H
