// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* Notes:
 * 1. Put all task priority and stack size definition in this file
 * 2. If the task priority is less than 10, use SSV_TASK_PRIO_MIN + X style,
 *    otherwise use SSV_TASK_PRIO_MAX - X style
 * 3. If this is a daemon task, the macro prefix is SSV_TASKD_, otherwise
 *    it's SSV_TASK_
 * 4. If the configMAX_PRIORITIES is modified, please make all priority are
 *    greater than 0
 * 5. Make sure ssv_task.h is consistent between wifi lib and idf
 */

#ifndef _SSV_TASK_H_
#define _SSV_TASK_H_

#include "sdkconfig.h"
// #include "freertos/FreeRTOSConfig.h"
#include "mesh_types.h"

#define SSV_TASK_PRIO_MAX (configMAX_PRIORITIES)
#define SSV_TASK_PRIO_MIN (0)

/* Bt contoller Task */
/* controller */
#define SSV_TASK_BT_CONTROLLER_PRIO   (SSV_TASK_PRIO_MAX - 2)
#ifdef CONFIG_NEWLIB_NANO_FORMAT
#define TASK_EXTRA_STACK_SIZE      (0)
#else
#define TASK_EXTRA_STACK_SIZE      (512)
#endif

#define BT_TASK_EXTRA_STACK_SIZE      TASK_EXTRA_STACK_SIZE
#define SSV_TASK_BT_CONTROLLER_STACK  (3584 + TASK_EXTRA_STACK_SIZE)


/* idf task */
#define SSV_TASK_TIMER_PRIO           OS_TASK_HIGH_PRIO //(SSV_TASK_PRIO_MAX - 3)
#define SSV_TASK_TIMER_STACK          (CONFIG_SSV_TIMER_TASK_STACK_SIZE +  TASK_EXTRA_STACK_SIZE)
#define SSV_TASKD_EVENT_PRIO          (SSV_TASK_PRIO_MAX - 5)
//#define SSV_TASKD_EVENT_STACK         (CONFIG_SSV_SYSTEM_EVENT_TASK_STACK_SIZE + TASK_EXTRA_STACK_SIZE)
#define SSV_TASK_TCPIP_PRIO           (SSV_TASK_PRIO_MAX - 7)
#define SSV_TASK_TCPIP_STACK          (CONFIG_LWIP_TCPIP_TASK_STACK_SIZE + TASK_EXTRA_STACK_SIZE)
#define SSV_TASK_MAIN_PRIO            (SSV_TASK_PRIO_MIN + 1)
//#define SSV_TASK_MAIN_STACK           (CONFIG_SSV_MAIN_TASK_STACK_SIZE + TASK_EXTRA_STACK_SIZE)

#endif
