#ifndef __MUTEX_H__
#define __MUTEX_H__
#include "linux/types.h"
#include "FreeRTOS_POSIX.h"

#define DEFINE_MUTEX(x) \
	pthread_mutex_t x = FREERTOS_POSIX_MUTEX_INITIALIZER

typedef pthread_mutex_t mutex;

#define mutex_init(x) \
	pthread_mutex_init(x, NULL)

#define mutex_lock(x) \
	pthread_mutex_lock(x)

#define mutex_lock_interruptible(x) \
	pthread_mutex_trylock(x)

#define mutex_unlock(x) \
	pthread_mutex_unlock(x)
#endif /* end of __MUTEX_H__ */
