/* Standard includes. */
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "mmio.h"
#include "delay.h"

/* cvitek includes. */
#include "printf.h"
#include "rtos_cmdqu.h"
#include "fast_image.h"
#include "cvi_mailbox.h"
#include "intr_conf.h"
#include "top_reg.h"
#include "memmap.h"

#include "comm.h"
#include "cvi_spinlock.h"
#include "jenc.h"

//#define __DEBUG__

#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif

extern struct transfer_config_t transfer_config;
struct trace_snapshot_t snapshot;

typedef struct _TASK_CTX_S {
	char        name[32];
	u16         stack_size;
	UBaseType_t priority;
	void (*runTask)(void *pvParameters);
	u8            queLength;
	QueueHandle_t queHandle;
} TASK_CTX_S;

/****************************************************************************
 * Function prototypes
 ****************************************************************************/
void prvQueueISR(void);
void prvCmdQuRunTask(void *pvParameters);

/****************************************************************************
 * Global parameters
 ****************************************************************************/
TASK_CTX_S gTaskCtx[E_QUEUE_MAX] = {
	{
		.name = "ISP",
		.stack_size = configMINIMAL_STACK_SIZE * 8,
		.priority = tskIDLE_PRIORITY + 3,
#ifndef FAST_IMAGE_ENABLE
		.runTask = NULL,
		.queLength = 1,
		.queHandle = NULL,
#else
		.runTask = prvISPRunTask,
		.queLength = 10,
		.queHandle = NULL,
#endif
	},
	{
		.name = "VCODEC",
		.stack_size = configMINIMAL_STACK_SIZE,
		.priority = tskIDLE_PRIORITY + 3,
#ifndef FAST_IMAGE_ENABLE
		.runTask = NULL,
		.queLength = 1,
		.queHandle = NULL,
#else
		.runTask = prvVcodecRunTask,
		.queLength = 10,
		.queHandle = NULL,
#endif
	},
	{
		.name = "VI",
		.stack_size = configMINIMAL_STACK_SIZE,
		.priority = tskIDLE_PRIORITY + 3,
#ifndef FAST_IMAGE_ENABLE
		.runTask = NULL,
		.queLength = 1,
		.queHandle = NULL,
#else
		.runTask = prvVIRunTask,
		.queLength = 10,
		.queHandle = NULL,
#endif
	},
	{
		.name = "CAMERA",
		.stack_size = configMINIMAL_STACK_SIZE,
		.priority = tskIDLE_PRIORITY + 3,
#ifndef FAST_IMAGE_ENABLE
		.runTask = NULL,
		.queLength = 1,
		.queHandle = NULL,
#else
		.runTask = prvCameraRunTask,
		.queLength = 10,
		.queHandle = NULL,
#endif
	},
	{
		.name = "RGN",
		.stack_size = configMINIMAL_STACK_SIZE,
		.priority = tskIDLE_PRIORITY + 3,
		.runTask = prvRGNRunTask,
		.queLength = 10,
		.queHandle = NULL,
	},
	{
		.name = "CMDQU",
		.stack_size = configMINIMAL_STACK_SIZE,
		.priority = tskIDLE_PRIORITY + 5,
		.runTask = prvCmdQuRunTask,
		.queLength = 30,
		.queHandle = NULL,
	},
	{
		.name = "AUDIO",
		.stack_size = configMINIMAL_STACK_SIZE*15,
		.priority = tskIDLE_PRIORITY + 3,
		.runTask = prvAudioRunTask,
		.queLength = 10,
		.queHandle = NULL,
	},
};

volatile struct mailbox_set_register *mbox_reg;
volatile struct mailbox_done_register *mbox_done_reg;
volatile unsigned long *mailbox_context; // mailbox buffer context is 64 Bytess

/****************************************************************************
 * Function definitions
 ****************************************************************************/
QueueHandle_t main_GetMODHandle(QUEUE_HANDLE_E handle_idx)
{
	if (handle_idx >= E_QUEUE_MAX)
		return NULL;

	return gTaskCtx[handle_idx].queHandle;
}

void main_create_tasks(void)
{
	u8 i = 0;

#define TASK_INIT(_idx) \
do { \
	gTaskCtx[_idx].queHandle = xQueueCreate(gTaskCtx[_idx].queLength, sizeof(cmdqu_t)); \
	if (gTaskCtx[_idx].queHandle != NULL && gTaskCtx[_idx].runTask != NULL) { \
		xTaskCreate(gTaskCtx[_idx].runTask, gTaskCtx[_idx].name, gTaskCtx[_idx].stack_size, \
			    NULL, gTaskCtx[_idx].priority, NULL); \
	} \
} while(0)

	for (; i < ARRAY_SIZE(gTaskCtx); i++) {
		TASK_INIT(i);
	}
}

DEFINE_CVI_SPINLOCK(mailbox_lock, SPIN_MBOX);

void main_cvirtos(void)
{
	printf("create cvi task\n");

	request_irq(MBOX_INT_C906_2ND, prvQueueISR, 0, "mailbox", (void *)0);

#ifdef FAST_IMAGE_ENABLE
	start_camera(0);
#endif

	main_create_tasks();

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was either insufficient FreeRTOS heap memory available for the idle
    and/or timer tasks to be created, or vTaskStartScheduler() was called from
    User mode.  See the memory management section on the FreeRTOS web site for
    more details on the FreeRTOS heap http://www.freertos.org/a00111.html.  The
    mode from which main() is called is set in the C start up code and must be
    a privileged mode (not user mode). */
    for (;;)
        ;
}

#ifdef FAST_IMAGE_ENABLE
extern JPEG_PIC encoded_pics[MAX_JPEG_NUM];
#endif
void prvCmdQuRunTask(void *pvParameters)
{
	/* Remove compiler warning about unused parameter. */
	(void)pvParameters;

	cmdqu_t rtos_cmdq;
	cmdqu_t *cmdq;
	cmdqu_t *rtos_cmdqu_t;
	static int stop_ip = 0;
	int ret = 0;
	int flags;
	int valid;
	int send_to_cpu = SEND_TO_CPU1;

	unsigned int reg_base = MAILBOX_REG_BASE;

	/* set mcu_status to type1 running*/
	transfer_config.mcu_status = MCU_STATUS_RTOS_T1_RUNNING;

	if (transfer_config.conf_magic == C906_MAGIC_HEADER)
		send_to_cpu = SEND_TO_CPU1;
	else if (transfer_config.conf_magic == CA53_MAGIC_HEADER)
		send_to_cpu = SEND_TO_CPU0;
	/* to compatible code with linux side */
	cmdq = &rtos_cmdq;
	mbox_reg = (struct mailbox_set_register *) reg_base;
	mbox_done_reg = (struct mailbox_done_register *) (reg_base + 2);
	mailbox_context = (unsigned long *) (MAILBOX_REG_BUFF);

	cvi_spinlock_init();
	printf("prvCmdQuRunTask run\n");

	for (;;) {
		xQueueReceive(gTaskCtx[E_QUEUE_CMDQU].queHandle, &rtos_cmdq, portMAX_DELAY);

		switch (rtos_cmdq.cmd_id) {
#if ( configUSE_TRACE_FACILITY == 1 )
			case SYS_CMD_INFO_TRACE_SNAPSHOT_START:
				debug_printf("SYS_CMD_INFO_TRACE_SNAPSHOT_START\n");
				vTraceEnable(TRC_START);
				break;
			case SYS_CMD_INFO_TRACE_SNAPSHOT_STOP:
				snapshot.ptr = xTraceGetTraceBuffer();
				snapshot.size = uiTraceGetTraceBufferSize();
				snapshot.type = 0;
				rtos_cmdq.param_ptr = &snapshot;
				vTraceStop();
				flush_dcache_range(&snapshot, sizeof (struct trace_snapshot_t));
				flush_dcache_range(snapshot.ptr, snapshot.size);
				debug_printf("SYS_CMD_INFO_TRACE_SNAPSHOT_STOP PA =%lx\n", &snapshot);
				debug_printf("SYS_CMD_INFO_TRACE_SNAPSHOT_STOP ptr =%lx\n", snapshot.ptr);
				debug_printf("SYS_CMD_INFO_TRACE_SNAPSHOT_STOP size =%lx\n", snapshot.size);
				goto send_label;
				break;
#endif
			case SYS_CMD_INFO_DUMP_JPG:
				if (rtos_cmdq.param_ptr >= MAX_JPEG_NUM) {
					printf("SYS_CMD_INFO_DUMP_JPG idx = %d, set idx to 0\n", rtos_cmdq.param_ptr);
					rtos_cmdq.param_ptr = 0;
				}
				#ifdef FAST_IMAGE_ENABLE
				rtos_cmdq.param_ptr = &encoded_pics[rtos_cmdq.param_ptr];
				#else
				rtos_cmdq.param_ptr = 0x0;
				#endif
			    goto send_label;
				break;
			case SYS_CMD_INFO_DUMP_EN:
				dump_uart_enable();
				break;
			case SYS_CMD_INFO_DUMP_DIS:
				dump_uart_disable();
				break;
			case SYS_CMD_INFO_DUMP_MSG:
				/* SYS_CMD_INFO_DUMP_MSG is block mode*/
				rtos_cmdq.cmd_id = SYS_CMD_INFO_DUMP_MSG;
				rtos_cmdq.param_ptr = (unsigned int) dump_uart_msg();
				goto send_label;
				break;
			case SYS_CMD_INFO_LINUX_INIT_DONE:
				rtos_cmdq.cmd_id = SYS_CMD_INFO_RTOS_INIT_DONE;
				rtos_cmdq.param_ptr = &transfer_config;
				goto send_label;
				break;
			case SYS_CMD_INFO_STOP_ISR:
				stop_ip = 0;
				rtos_cmdq.cmd_id = SYS_CMD_INFO_STOP_ISR;
				rtos_cmdq.ip_id = IP_VI;
				xQueueSend(gTaskCtx[E_QUEUE_VI].queHandle, &rtos_cmdq, 0U);
				break;
			case SYS_CMD_INFO_STOP_ISR_DONE:
				// stop interrupt in order to avoid losing frame
				if (rtos_cmdq.ip_id == IP_VI) {
					stop_ip |= STOP_CMD_DONE_VI;
					rtos_cmdq.ip_id = IP_VCODEC;
					rtos_cmdq.cmd_id = SYS_CMD_INFO_STOP_ISR;
					xQueueSend(gTaskCtx[E_QUEUE_VCODEC].queHandle, &rtos_cmdq, 0U);
					break;
				}
				if (rtos_cmdq.ip_id == IP_VCODEC)
					stop_ip |= STOP_CMD_DONE_VCODE;
				if (stop_ip != STOP_CMD_DONE_ALL)
					break;
				else {
					// all isr of ip is disabled, and send msg back to linux
					rtos_cmdq.ip_id = IP_SYSTEM;
				}
			case SYS_CMD_INFO_LINUX:
			default:
send_label:
				/* used to send command to linux*/
				rtos_cmdqu_t = (cmdqu_t *) mailbox_context;

				debug_printf("RTOS_CMDQU_SEND\n");
				debug_printf("ip_id=%d cmd_id=%d param_ptr=%x\n", cmdq->ip_id, cmdq->cmd_id, (unsigned int)cmdq->param_ptr);
				debug_printf("mailbox_context = %x\n", mailbox_context);
				debug_printf("linux_cmdqu_t = %x\n", rtos_cmdqu_t);
				debug_printf("cmdq->ip_id = %d\n", cmdq->ip_id);
				debug_printf("cmdq->cmd_id = %d\n", cmdq->cmd_id);
				debug_printf("cmdq->block = %d\n", cmdq->block);
				debug_printf("cmdq->para_ptr = %x\n", cmdq->param_ptr);

				drv_spin_lock_irqsave(&mailbox_lock, flags);
				if (flags == MAILBOX_LOCK_FAILED) {
					printf("[%s][%d] drv_spin_lock_irqsave failed! ip_id = %d , cmd_id = %d\n" , cmdq->ip_id , cmdq->cmd_id);
					break;
				}

				for (valid = 0; valid < MAILBOX_MAX_NUM; valid++) {
					if (rtos_cmdqu_t->resv.valid.linux_valid == 0 && rtos_cmdqu_t->resv.valid.rtos_valid == 0) {
						// mailbox buffer context is 4 bytes write access
						int *ptr = (int *)rtos_cmdqu_t;

						cmdq->resv.valid.rtos_valid = 1;
						*ptr = ((cmdq->ip_id << 0) | (cmdq->cmd_id << 8) | (cmdq->block << 15) |
								(cmdq->resv.valid.linux_valid << 16) |
								(cmdq->resv.valid.rtos_valid << 24));
						rtos_cmdqu_t->param_ptr = cmdq->param_ptr;
						debug_printf("rtos_cmdqu_t->linux_valid = %d\n", rtos_cmdqu_t->resv.valid.linux_valid);
						debug_printf("rtos_cmdqu_t->rtos_valid = %d\n", rtos_cmdqu_t->resv.valid.rtos_valid);
						debug_printf("rtos_cmdqu_t->ip_id =%x %d\n", &rtos_cmdqu_t->ip_id, rtos_cmdqu_t->ip_id);
						debug_printf("rtos_cmdqu_t->cmd_id = %d\n", rtos_cmdqu_t->cmd_id);
						debug_printf("rtos_cmdqu_t->block = %d\n", rtos_cmdqu_t->block);
						debug_printf("rtos_cmdqu_t->param_ptr addr=%x %x\n", &rtos_cmdqu_t->param_ptr, rtos_cmdqu_t->param_ptr);
						debug_printf("*ptr = %x\n", *ptr);
						// clear mailbox
						mbox_reg->cpu_mbox_set[send_to_cpu].cpu_mbox_int_clr.mbox_int_clr = (1 << valid);
						// trigger mailbox valid to rtos
						mbox_reg->cpu_mbox_en[send_to_cpu].mbox_info |= (1 << valid);
						mbox_reg->mbox_set.mbox_set = (1 << valid);
						break;
					}
					rtos_cmdqu_t++;
				}
				drv_spin_unlock_irqrestore(&mailbox_lock, flags);
				if (valid >= MAILBOX_MAX_NUM) {
				    printf("No valid mailbox is available\n");
				    return -1;
				}
				break;
		}
	}
}

void prvQueueISR(void)
{
	//printf("prvQueueISR\n");

	unsigned char set_val;
//	unsigned char done_val;
	unsigned char valid_val;
	int i;
	cmdqu_t *cmdq;
	BaseType_t YieldRequired = pdFALSE;

	set_val = mbox_reg->cpu_mbox_set[RECEIVE_CPU].cpu_mbox_int_int.mbox_int;
	/* Now, we do not implement info back feature */
	// done_val = mbox_done_reg->cpu_mbox_done[RECEIVE_CPU].cpu_mbox_int_int.mbox_int;

	if (set_val) {
		for(i = 0; i < MAILBOX_MAX_NUM; i++) {
			valid_val = set_val  & (1 << i);

			if (valid_val) {
				cmdqu_t rtos_cmdq;
				cmdq = (cmdqu_t *)(mailbox_context) + i;

				debug_printf("mailbox_context =%x\n", mailbox_context);
				debug_printf("sizeof mailbox_context =%x\n", sizeof(cmdqu_t));
				/* mailbox buffer context is send from linux, clear mailbox interrupt */
				mbox_reg->cpu_mbox_set[RECEIVE_CPU].cpu_mbox_int_clr.mbox_int_clr = valid_val;
				// need to disable enable bit
				mbox_reg->cpu_mbox_en[RECEIVE_CPU].mbox_info &= ~valid_val;

				// copy cmdq context (8 bytes) to buffer ASAP
				*((unsigned long *) &rtos_cmdq) = *((unsigned long *)cmdq);
				/* need to clear mailbox interrupt before clear mailbox buffer */
				*((unsigned long*) cmdq) = 0;

				/* mailbox buffer context is send from linux*/
				if (rtos_cmdq.resv.valid.linux_valid == 1) {
					debug_printf("cmdq=%x\n", cmdq);
					debug_printf("cmdq->ip_id =%d\n", rtos_cmdq.ip_id);
					debug_printf("cmdq->cmd_id =%d\n", rtos_cmdq.cmd_id);
					debug_printf("cmdq->param_ptr =%x\n", rtos_cmdq.param_ptr);
					debug_printf("cmdq->block =%x\n", rtos_cmdq.block);
					debug_printf("cmdq->linux_valid =%d\n", rtos_cmdq.resv.valid.linux_valid);
					debug_printf("cmdq->rtos_valid =%x\n", rtos_cmdq.resv.valid.rtos_valid);
					switch (rtos_cmdq.ip_id) {
					case IP_ISP:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_ISP].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					case IP_VCODEC:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_VCODEC].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					case IP_VI:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_VI].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					case IP_RGN:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_RGN].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					case IP_AUDIO:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_AUDIO].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					case IP_SYSTEM:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_CMDQU].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					case IP_CAMERA:
						xQueueSendFromISR(gTaskCtx[E_QUEUE_CAMERA].queHandle, &rtos_cmdq, &YieldRequired);
						break;
					default:
						printf("unknown ip_id =%d cmd_id=%d\n", rtos_cmdq.ip_id, rtos_cmdq.cmd_id);
						break;
					}
					portYIELD_FROM_ISR(YieldRequired);
				} else
					printf("rtos cmdq is not valid %d, ip=%d , cmd=%d\n",
						rtos_cmdq.resv.valid.rtos_valid, rtos_cmdq.ip_id, rtos_cmdq.cmd_id);
			}
		}
	}
}
