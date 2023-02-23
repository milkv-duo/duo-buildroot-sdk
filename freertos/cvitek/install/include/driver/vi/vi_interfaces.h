#ifndef __VI_INTERFACES_H__
#define __VI_INTERFACES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************
 *  Common interface for cvi_vi
 ******************************************************/
int vi_drv_open(struct cvi_vi_ctx *viCtx);
int vi_drv_cfg(void);
int vi_drv_streamon(void);
int vi_drv_streamoff(void);
int vi_drv_release(void);

#ifdef __cplusplus
}
#endif

#endif /* __VI_INTERFACES_H__ */
