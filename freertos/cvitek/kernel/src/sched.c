#include <stdarg.h>
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/sched.h"
#include "sched.h"

task_struct kthread_run(TaskFunction_t threadfn, void *data, char *fn_name, ...)
{
	TaskHandle_t t;

	va_list args;
	char buf[0x10];
	int flags;
	UBaseType_t task_prio = uxTaskPriorityGet(current);

	va_start(args, fn_name);
	vsnprintf(buf, sizeof(buf) - 1, fn_name, args);
	va_end(args);

	xTaskCreate(threadfn, fn_name, configMINIMAL_STACK_SIZE,
		data, task_prio, &t);
	return t;
}

int kthread_stop(task_struct kthread)
{
	if (kthread) {
		vTaskDelete(kthread);
		return 0;
	} else
		return -1;
}

