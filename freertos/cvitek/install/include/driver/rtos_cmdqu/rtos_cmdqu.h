#ifndef __RTOS_COMMAND_QUEUE__
#define __RTOS_COMMAND_QUEUE__

#ifdef __linux__
#include <linux/kernel.h>
#endif

#define NR_SYSTEM_CMD           20
#define NR_RTOS_CMD            127
#define NR_RTOS_IP        IP_LIMIT

enum IP_TYPE {
	IP_ISP = 0,
	IP_VCODEC,
	IP_VIP,
	IP_VI,
	IP_RGN,
	IP_AUDIO,
	IP_SYSTEM,
	IP_CAMERA,
	IP_LIMIT,
};

enum SYS_CMD_ID {
	SYS_CMD_INFO_TRANS = 0x50,
	SYS_CMD_INFO_LINUX_INIT_DONE,
	SYS_CMD_INFO_RTOS_INIT_DONE,
	SYS_CMD_INFO_STOP_ISR,
	SYS_CMD_INFO_STOP_ISR_DONE,
	SYS_CMD_INFO_LINUX,
	SYS_CMD_INFO_RTOS,
	SYS_CMD_SYNC_TIME,
	SYS_CMD_INFO_DUMP_MSG,
	SYS_CMD_INFO_DUMP_EN,
	SYS_CMD_INFO_DUMP_DIS,
	SYS_CMD_INFO_DUMP_JPG,
	SYS_CMD_INFO_TRACE_SNAPSHOT_START,
	SYS_CMD_INFO_TRACE_SNAPSHOT_STOP,
	SYS_CMD_INFO_TRACE_STREAM_START,
	SYS_CMD_INFO_TRACE_STREAM_STOP,
	SYS_CMD_INFO_LIMIT,
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

