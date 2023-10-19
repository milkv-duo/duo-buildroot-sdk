#ifndef __SEMAPHORE_HEADER__
#define __SEMAPHORE_HEADER__

#include "linux/types.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/semaphore.h"

#define semaphore sem_internal
#define DEFINE_SEMAPHORE(name)  \
    struct sem_internal name;

void down(sem_internal_t *xsemaphore);
void up(sem_internal_t *xsemaphore);
int down_interruptible(sem_internal_t *sem);

static inline void sema_init(sem_internal_t *sem, int val)
{
    sem_init(sem, NULL, val);
}

#endif
