#ifndef __TIME_HEADER__
#define __TIME_HEADER__
#include "xil_types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/time.h"

#define NSEC_PER_SEC            1000000000L
#define UM_NSEC_PER_SEC         NSEC_PER_SEC
#define UM_NSEC_PER_USEC        1000L

typedef long suseconds_t;

struct timezone {
	int tz_minuteswest; /* minutes W of Greenwich */
	int tz_dsttime; /* type of dst correction */
};

struct timeval {
	long tv_sec; /* seconds */
	long tv_usec; /* microseconds */
};

int gettimeofday(struct timeval *tv /*in*/, struct timezone *tz /*in*/);
struct timeval ns_to_timeval(const s64 nsec);
long long timeval_to_ns(const struct timeval *tv);
long long timespec_to_ns(const struct timespec *ts);

typedef __s64 time64_t;

typedef struct __timespec64 {
	time64_t	tv_sec;			/* seconds */
	long		tv_nsec;		/* nanoseconds */
} timespec64, timespec;

u64 ktime_get_ns(void);
void ktime_get_ts(struct timespec *ts);

#endif
