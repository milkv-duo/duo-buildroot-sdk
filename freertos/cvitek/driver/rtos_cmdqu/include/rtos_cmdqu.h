#ifndef __RTOS_COMMAND_QUEUE__
#define __RTOS_COMMAND_QUEUE__

#ifdef __linux__
#include <linux/kernel.h>
#endif

#define NR_SYSTEM_CMD           20
#define NR_RTOS_CMD            127
#define NR_RTOS_IP        IP_LIMIT

enum SYS_CMD_ID {
	CMD_TEST_A	= 0x10,
	CMD_TEST_B,
	CMD_TEST_C,
	CMD_DUO_LED,
	SYS_CMD_INFO_LIMIT,
};

enum DUO_LED_STATUS {
	DUO_LED_ON	= 0x02,
	DUO_LED_OFF,
	DUO_LED_DONE,
};

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

#ifdef __linux__
/* keep those commands for ioctl system used */
enum SYSTEM_CMD_TYPE {
	CMDQU_SEND = 1,
	CMDQU_REQUEST,
	CMDQU_REQUEST_FREE,
	CMDQU_SEND_WAIT,
	CMDQU_SEND_WAKEUP,
	CMDQU_SYSTEM_LIMIT = NR_SYSTEM_CMD,
};

#define RTOS_CMDQU_DEV_NAME "cvi-rtos-cmdqu"
#define RTOS_CMDQU_SEND                         _IOW('r', CMDQU_SEND, unsigned long)
#define RTOS_CMDQU_REQUEST                      _IOW('r', CMDQU_REQUEST, unsigned long)
#define RTOS_CMDQU_REQUEST_FREE                 _IOW('r', CMDQU_REQUEST_FREE, unsigned long)
#define RTOS_CMDQU_SEND_WAIT                    _IOW('r', CMDQU_SEND_WAIT, unsigned long)
#define RTOS_CMDQU_SEND_WAKEUP                  _IOW('r', CMDQU_SEND_WAKEUP, unsigned long)

int rtos_cmdqu_send(cmdqu_t *cmdq);
int rtos_cmdqu_send_wait(cmdqu_t *cmdq, int wait_cmd_id);
int request_rtos_irq(unsigned char ip_id, void *handler, const char *devname, void *dev_id);
int free_rtos_irq(unsigned char ip_id);

#endif // end of __linux__

#endif  // end of __RTOS_COMMAND_QUEUE__

