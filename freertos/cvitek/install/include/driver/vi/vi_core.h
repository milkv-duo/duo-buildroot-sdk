#ifndef __VI_CORE_H__
#define __VI_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vi_common.h>
#include <vi_defines.h>


/*******************************************************
 *  Common interface for core
 ******************************************************/
void vi_irq_handler(struct cvi_vi_dev *vdev);
int vi_create_instance(struct cvi_vi_dev *vdev);
int vi_destroy_instance(void);
int vi_start_streaming(struct cvi_vi_dev *vdev);
int vi_stop_streaming(struct cvi_vi_dev *vdev);
int vi_enq_buf(void);

#ifdef __cplusplus
}
#endif

#endif /* __VI_CORE_H__ */
