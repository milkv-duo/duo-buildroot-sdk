#ifndef _DEPRECATED_U_DWA_UAPI_H_
#define _DEPRECATED_U_DWA_UAPI_H_

#include <linux/dwa_uapi.h>

#define CVIDWA_BEGIN_JOB _IOWR('D', 0x00, struct gdc_handle_data)
#define CVIDWA_END_JOB _IOW('D', 0x01, struct gdc_handle_data)
#define CVIDWA_ADD_ROT_TASK _IOW('D', 0x02, struct gdc_task_attr)
#define CVIDWA_ADD_LDC_TASK _IOW('D', 0x03, struct gdc_task_attr)
#define CVIDWA_ROT _IOW('D', 0x04, unsigned long long)
#define CVIDWA_LDC _IOW('D', 0x05, unsigned long long)

#endif // _DEPRECATED_U_DWA_UAPI_H_
