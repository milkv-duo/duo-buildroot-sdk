#ifndef __RTOS_COMMAND_QUEUE__
#define __RTOS_COMMAND_QUEUE__

#include <linux/kernel.h>

#define NR_SYSTEM_CMD           20

struct valid_t {
	unsigned char linux_valid;
	unsigned char rtos_valid;
} __attribute__((packed));

typedef union resv_t {
	struct valid_t valid;
	unsigned short mstime; // 0 : noblock, -1 : block infinite
} resv_t;

typedef struct cmdqu_t cmdqu_t;
/* cmdqu size should be 8 bytes because of mailbox buffer size */
struct cmdqu_t {
	unsigned char ip_id;
	unsigned char cmd_id : 7;
	unsigned char block : 1;
	union resv_t resv;
	unsigned int  param_ptr;
} __attribute__((packed)) __attribute__((aligned(0x8)));

/* keep those commands for ioctl system used */
enum SYSTEM_CMD_TYPE {
	CMDQU_SEND = 1,
	CMDQU_SEND_WAIT,
	CMDQU_SEND_WAKEUP,
	CMDQU_SYSTEM_LIMIT = NR_SYSTEM_CMD,
};

#define RTOS_CMDQU_DEV_NAME "cvi-rtos-cmdqu"
#define RTOS_CMDQU_SEND                         _IOW('r', CMDQU_SEND, unsigned long)
#define RTOS_CMDQU_SEND_WAIT                    _IOW('r', CMDQU_SEND_WAIT, unsigned long)
#define RTOS_CMDQU_SEND_WAKEUP                  _IOW('r', CMDQU_SEND_WAKEUP, unsigned long)

int rtos_cmdqu_send(cmdqu_t *cmdq);
int rtos_cmdqu_send_wait(cmdqu_t *cmdq, int wait_cmd_id);

#endif  // end of __RTOS_COMMAND_QUEUE__
