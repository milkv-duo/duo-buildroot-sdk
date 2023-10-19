#include "rtos_queue.h"
#include "rtos_malloc.h"
#include "rtos_cmdqu.h"
#include <linux/spinlock.h>

spinlock_t *rtos_queue_lock;
void queue_new(queue_t *self)
{
#if 0
	/* this queue is created by linux now*/
    if(!self->queue_buffer) {
        self->queue_buffer = memory_alloc(sizeof(cmdqu_t) * QUEUE_NUM);
		// memory_set(self->queue_buffer, 0, sizeof(cmdqu_t) * QUEUE_NUM);
		self->head = 0;
		self->tail = 0;
    }
#endif
}

bool queue_is_empty(queue_t *self)
{
	if (self->head == self->tail)
		return 1; // empty
	else
		return 0;
}

bool queue_is_full(queue_t *self)
{
	if (self->head == (self->tail + 1) % QUEUE_NUM)
		return 1; // full
	else
		return 0;
}

cmdqu_t *queue_peek(queue_t *self)
{
	if (queue_is_empty(self)) {
		printf("queue_peek is empty\n");
		return 0;
	}
	return (cmdqu_t *)(self->queue_buffer + self->head * sizeof(cmdqu_t));
}
bool queue_enqueue(queue_t *self, cmdqu_t *data)
{
	unsigned long flags;
	spin_lock_irqsave(rtos_queue_lock, flags);
	if (queue_is_full(self)) {
		printf("queue is full\n");
	} else {
		memcpy(self->queue_buffer + self->tail * sizeof(cmdqu_t), data,
		       sizeof(cmdqu_t));
		self->tail = (self->tail + 1) % QUEUE_NUM;
	}
	spin_unlock_irqrestore(rtos_queue_lock, flags);
}

cmdqu_t *queue_dequeue(queue_t *self)
{
	unsigned long flags;
	spin_lock_irqsave(rtos_queue_lock, flags);
	if (queue_is_empty(self)) {
		printf("queue is empty\n");
		spin_unlock_irqrestore(rtos_queue_lock, flags);
		return 0;
	} else {
		cmdqu_t *peekqu = queue_peek(self);
		self->head = (self->head + 1) % QUEUE_NUM;
		spin_unlock_irqrestore(rtos_queue_lock, flags);
		return peekqu;
	}
}
void queue_init(void *rtos_shm_para)
{
	rtos_queue_lock = &((rtos_shm_t *)rtos_shm_para)->rtos_queue_lock;
	/* already init in linux*/
	//queue_new(&((rtos_shm_t *)rtos_shm_para)->linux_cmd_queue);
	//queue_new(&((rtos_shm_t *)rtos_shm_para)->rtos_cmd_queue);
}

