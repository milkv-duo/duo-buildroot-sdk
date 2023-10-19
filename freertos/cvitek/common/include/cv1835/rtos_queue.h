#ifndef RTOS_QUEUE_H
#define RTOS_QUEUE_H
#include <stdbool.h>
#include <stddef.h>

#define QUEUE_NUM 0x40

typedef struct queue_t queue_t;
typedef struct cmdqu_t cmdqu_t;

struct cmdqu_t {
	unsigned int ip_id;
	unsigned int cmd_id;
	void *param_ptr;
}; // __attribute__((aligned(0x20)));

struct queue_t {
	unsigned int head;
	unsigned int tail;
	char *linux_queue_buffer;
	union {
		char *queue_buffer;
		char *rtos_queue_buffer;
	};
};

void queue_init(void *rtos_shm_para);
void queue_new(queue_t *self);
bool queue_is_empty(queue_t *self);
bool queue_enqueue(queue_t *self, cmdqu_t *data);
cmdqu_t *queue_peek(queue_t *self);
cmdqu_t *queue_dequeue(queue_t *self);

#endif // RTOS_QUEUE_H
