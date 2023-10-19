#ifndef __SCHED_HEADER__
#define __SCHED_HEADER__
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/sched.h"
#include "task.h"

typedef TaskHandle_t task_struct;
#define current xTaskGetCurrentTaskHandle()

#define TASK_RUNNING        0
#define TASK_INTERRUPTIBLE  1
#define TASK_UNINTERRUPTIBLE    2
#define __TASK_STOPPED      4
#define __TASK_TRACED       8

/* in tsk->state again */
#define TASK_DEAD       64
#define TASK_WAKEKILL       128
#define TASK_WAKING     256
#define TASK_PARKED     512
#define TASK_NOLOAD     1024
#define TASK_NEW        2048
#define TASK_STATE_MAX      4096

#define TASK_STATE_TO_CHAR_STR "RSDTtXZxKWPNn"

//extern char ___assert_task_state[1 - 2*!!(
//        sizeof(TASK_STATE_TO_CHAR_STR)-1 != ilog2(TASK_STATE_MAX)+1)];

/* Convenience macros for the sake of set_task_state */
#define TASK_KILLABLE       (TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)
#define TASK_STOPPED        (TASK_WAKEKILL | __TASK_STOPPED)
#define TASK_TRACED     (TASK_WAKEKILL | __TASK_TRACED)

#define TASK_IDLE       (TASK_UNINTERRUPTIBLE | TASK_NOLOAD)

/* Convenience macros for the sake of wake_up */
#define TASK_NORMAL     (TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)
#define TASK_ALL        (TASK_NORMAL | __TASK_STOPPED | __TASK_TRACED)


task_struct kthread_run(TaskFunction_t threadfn, void *data, char *fn_name, ...);
#if 0
{
	TaskHandle_t t;
	UBaseType_t task_prio = uxTaskPriorityGet(current);
	xTaskCreate(threadfn, fn_name, configMINIMAL_STACK_SIZE,
		data, task_prio, &t);
	return t;
}
#endif
int kthread_stop(task_struct kthread);
#if 0
{
	if (kthread) {
		vTaskDelete(kthread);
		return 0;
	} else
		return -1;
}
#endif
#endif
