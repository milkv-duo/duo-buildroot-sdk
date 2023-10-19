#include "arch_sleep.h"
#include "sleep.h"

void usleep(unsigned long useconds)
{
	arch_usleep(useconds);
}
void nanosleep(unsigned long nseconds)
{
	arch_nsleep(nseconds);
}

