#ifndef __COMM_HEADER__
#define __COMM_HEADER__

#include "comm_def.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

extern void prvISPRunTask(void *pvParameters);
extern void prvVcodecRunTask(void *pvParameters);
extern void prvVipRunTask(void *pvParameters);
extern void prvVIRunTask(void *pvParameters);
extern void prvRGNRunTask(void *pvParameters);
extern void prvAudioRunTask(void *pvParameters);
extern void prvCameraRunTask(void *pvParameters);

QueueHandle_t main_GetMODHandle(QUEUE_HANDLE_E handle_idx);

/*
 * Sensor API
 */
// extern void start_sensor(uint32_t u32SnsId);
extern void start_camera(uint32_t u32SnsId);

#endif // end of __COMM_HEADER__
