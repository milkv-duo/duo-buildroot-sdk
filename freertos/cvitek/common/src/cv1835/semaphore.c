#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/semaphore.h"
#include "linux/semaphore.h"

void up(sem_internal_t *xsemaphore)
{
	if(xsemaphore) {
		sem_post(xsemaphore);
	} else
		printf("up fail");
}

int down_interruptible(sem_internal_t *xsemaphore)
{
	if (!xsemaphore) {
		sema_init(xsemaphore, 1);
	}
	if (xsemaphore != NULL) {
		sem_wait(xsemaphore);
		return 0;
	} else {
		printf("down %lx fail\n", xsemaphore);
		return 1;
	}
}

void down(sem_internal_t *xsemaphore)
{
	down_interruptible(xsemaphore);
}
