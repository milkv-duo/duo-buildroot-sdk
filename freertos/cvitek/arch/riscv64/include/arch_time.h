#ifndef __TIME_L_H__
#define __TIME_L_H__

inline static unsigned long GetSysTime(void)
{
	unsigned long sys_tick = 0;

	asm volatile("rdtime %0": "=r"(sys_tick));
	return sys_tick;
}

#endif
