#include "xtime_l.h"
#include "xparameters.h"
#include "time.h"
#include "linux/errno.h"
#include "linux/math64.h"
#include "FreeRTOS_POSIX/time.h"

int gettimeofday(struct timeval *tv /*in*/, struct timezone *tz /*in*/)
{
	XTime tick;

	XTime_GetTime(&tick);
	tv->tv_sec = tick / XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ;
	tv->tv_usec = tick / (XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ / 1000000);
	/* not support tz */
	if (tz) {
		tz->tz_minuteswest = -1;
		tz->tz_dsttime = -1;
		return -EINVAL;
	}
	return 0;
}

struct timespec ns_to_timespec(const s64 nsec)
{
	struct timespec ts;
	s32 rem;

	if (!nsec)
		return (struct timespec) {0, 0};

	ts.tv_sec = div_s64_rem(nsec, NSEC_PER_SEC, &rem);
	if (unlikely(rem < 0)) {
		ts.tv_sec--;
		rem += NSEC_PER_SEC;
	}
	ts.tv_nsec = rem;

	return ts;
}

struct timeval ns_to_timeval(const s64 nsec)
{
        struct timespec ts = ns_to_timespec(nsec);
        struct timeval tv;

        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = (suseconds_t) ts.tv_nsec / 1000;

        return tv;
}

inline long long timeval_to_ns(const struct timeval *tv)
{
	return ((long long) tv->tv_sec * UM_NSEC_PER_SEC) +
		tv->tv_usec * UM_NSEC_PER_USEC;
}

inline long long timespec_to_ns(const struct timespec *ts)
{
	return ((long long) ts->tv_sec * UM_NSEC_PER_SEC) +
		ts->tv_nsec;
}

inline u64 ktime_get_ns(void)
{
//TODO
	XTime tick;

	XTime_GetTime(&tick);
	return (u64)tick;
}

/**
 * ktime_get_ts64 - get the monotonic clock in timespec64 format
 * @ts:		pointer to timespec variable
 *
 * The function calculates the monotonic clock from the realtime
 * clock and the wall_to_monotonic offset and stores the result
 * in normalized timespec64 format in the variable pointed to by @ts.
 */
void ktime_get_ts(struct timespec *ts)
{
	XTime tick;

	XTime_GetTime(&tick);
	ts->tv_sec = tick / XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ;
	ts->tv_nsec = tick / (XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ / 1000);
}
