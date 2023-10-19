#ifndef RTOS_QUEUE_H
#define RTOS_QUEUE_H
//#include <linux/kernel.h>
//#include <linux/slab.h>
#include <stddef.h>
#include <stdbool.h>

#define QUEUE_NUM       0x20
//typedef size_t unsigned long;
//typedef bool   unsigned char;
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
        unsigned long virt_phy_offset;
        union {
//            unsigned long linux_cmd_queue_addr;
            queue_t linux_cmd_queue;
        };
        union {
//            unsigned long rtos_cmd_queue_addr;
            queue_t rtos_cmd_queue;
        };
//        spinlock_t rtos_memory_lock;
//        spinlock_t rtos_queue_lock;
} shm_para_t;

void queue_new(queue_t *self);
bool queue_is_empty(queue_t *self);
bool queue_enqueue(queue_t *self, cmdqu_t *data);
cmdqu_t *queue_peek(queue_t *self);
cmdqu_t *queue_dequeue(queue_t *self);
#endif //RTOS_QUEUE_H
