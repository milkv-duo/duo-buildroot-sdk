#ifndef RTOS_QUEUE_H
#define RTOS_QUEUE_H
#include <stddef.h>
#include <stdbool.h>

#define QUEUE_NUM       0x80

typedef struct queue_t queue_t;
typedef struct cmdqu_t cmdqu_t;

struct cmdqu_t {
    size_t ip_id;
    size_t cmd_id;
    size_t *param_ptr;
}__attribute__ ((aligned (0x20)));

struct queue_t {
    unsigned int head;
    unsigned int tail;
    char *linux_queue_buffer;
    union {
        char *queue_buffer;
        char *rtos_queue_buffer;
    };
};

struct shm_para_t {
    size_t virt_phys_offset;
    queue_t linux_cmd_queue;
    queue_t rtos_cmd_queue;
//    spinlock_t rtos_memory_lock;
//    spinlock_t rtos_queue_lock;
} shm_para_t;

void queue_new(queue_t *self);
bool queue_is_empty(queue_t *self);
bool queue_enqueue(queue_t *self, cmdqu_t *data);
cmdqu_t *queue_peek(queue_t *self);
cmdqu_t *queue_dequeue(queue_t *self);

#endif //RTOS_QUEUE_H
