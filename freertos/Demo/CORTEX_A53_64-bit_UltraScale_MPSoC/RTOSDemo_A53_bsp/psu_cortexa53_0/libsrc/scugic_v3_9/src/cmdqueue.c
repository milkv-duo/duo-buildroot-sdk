#include "malloc.h"
#include "queue.h"

//#define QUEUE_NUM       0x20

void queue_new(queue_t *self)
{
    if(!self->queue_buffer) {
        self->queue_buffer = memory_alloc(sizeof(cmdqu_t) * QUEUE_NUM);
        //printk("queue_buffer =%lx\n", queue_buffer);
//        memset(self->queue_buffer, 0, sizeof(cmdqu_t) * QUEUE_NUM);
       self->head = 0;
	    self->tail = 0;
    }
}

bool queue_is_empty(queue_t *self)
{
    if (self->head == self->tail)
        return 1;//empty
    else
        return 0;
}

bool queue_is_full(queue_t *self)
{
    if (self->head == (self->tail + 1) % QUEUE_NUM)
        return 1;//full
    else
        return 0;
}

cmdqu_t *queue_peek(queue_t *self)
{
    if(queue_is_empty(self)) {
        xil_printf("queue_peek is empty\n");
        return -1;
    }
    return (cmdqu_t*)(self->queue_buffer + self->head * sizeof(cmdqu_t));
}
bool queue_enqueue(queue_t *self, cmdqu_t *data)
{
    if(queue_is_full(self)) {
        xil_printf("queue is full\n");
    } else {
        memcpy(self->queue_buffer + self->tail * sizeof(cmdqu_t), data, sizeof(cmdqu_t));
        self->tail = (self->tail + 1) % QUEUE_NUM;
    }
}

cmdqu_t *queue_dequeue(queue_t *self)
{
    if(queue_is_empty(self)) {
        xil_printf("queue is empty\n");
        return 0;
    } else {
        cmdqu_t *peekqu = queue_peek(self);
        self->head = (self->head + 1) % QUEUE_NUM;
        return peekqu;
    }
}

