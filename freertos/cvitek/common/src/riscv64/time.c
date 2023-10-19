#include "FreeRTOSConfig.h"
#include "arch_time.h"
#include <sys/time.h>

int gettimeofday(struct timeval *tv /*in*/, void *tz /*in*/)
{
	unsigned long tick;
	tick = GetSysTime();
	tv->tv_sec = tick / configSYS_CLOCK_HZ;
	tv->tv_usec = (tick / (configSYS_CLOCK_HZ / 1000000)) - tv->tv_sec * 1000000;
	/* not support tz */
	if (tz) {
		return -1;
	}
	return 0;
}
