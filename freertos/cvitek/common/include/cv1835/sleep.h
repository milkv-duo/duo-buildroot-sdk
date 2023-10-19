#ifndef _SLEEP_H
#define _SLEEP_H

void arch_usleep(unsigned long useconds);
void arch_sleep(unsigned int seconds);

#define usleep_range(a, b) arch_usleep(a)

#endif // end of _SLEEP_H
