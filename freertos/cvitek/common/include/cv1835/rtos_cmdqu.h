#ifndef __RTOS_COMMAND_QUEUE__
#define __RTOS_COMMAND_QUEUE__
//#include <linux/kernel.h>
#include "rtos_queue.h"
#include "rtos_malloc.h"
#include <linux/spinlock.h>
/*
#define RTOS_CMDQU_DEV_NAME "cvi-rtos-cmdqu"
#define RTOS_CMDQU_INIT                         _IOW('r', 0x01, unsigned long)
#define RTOS_CMDQU_DEINIT                       _IOW('r', 0x02, unsigned long)
#define RTOS_CMDQU_SEND                         _IOW('r', 0x03, unsigned long)
#define RTOS_CMDQU_MALLOC                       _IOW('r', 0x04, unsigned long)
#define RTOS_CMDQU_MFREE                        _IOW('r', 0x05, unsigned long)
#define RTOS_CMDQU_MEMCPY_TO_USR                _IOW('r', 0x06, unsigned long)
#define RTOS_CMDQU_MEMCPY_FROM_USR              _IOW('r', 0x07, unsigned long)
*/
typedef struct rtos_shm_t {
	size_t size;
	union{
	size_t phys_addr;
	size_t addr;
	};
	size_t virt_base; /* kernel logical address in use kernel */
	Header mem_base; /* malloc first base address*/
	size_t virt_phys_offset;
	queue_t linux_cmd_queue;
	queue_t rtos_cmd_queue;
	spinlock_t rtos_memory_lock;
	spinlock_t rtos_queue_lock;

} rtos_shm_t;

#endif
