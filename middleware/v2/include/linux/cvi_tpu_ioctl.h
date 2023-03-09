/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: cvi_tpu_ioctl.h
 * Description:
 */

#ifndef __CVI_TPU_IOCTL_H__
#define __CVI_TPU_IOCTL_H__

struct cvi_cache_op_arg {
	unsigned long long paddr;
	unsigned long long size;
	int dma_fd;
};

struct cvi_submit_dma_arg {
	int fd;
	unsigned int seq_no;
};

struct cvi_wait_dma_arg {
	unsigned int seq_no;
	int ret;
};

struct cvi_pio_mode {
	unsigned long long cmdbuf;
	unsigned long long sz;
};

struct cvi_load_tee_arg {
	// normal domain
	unsigned long long cmdbuf_addr_ree;
	unsigned int cmdbuf_len_ree;
	unsigned long long weight_addr_ree;
	unsigned int weight_len_ree;
	unsigned long long neuron_addr_ree;

	// security domain
	unsigned long long dmabuf_addr_tee;
};

struct cvi_submit_tee_arg {
	unsigned long long dmabuf_tee_addr;
	unsigned long long gaddr_base2;
	unsigned long long gaddr_base3;
	unsigned long long gaddr_base4;
	unsigned long long gaddr_base5;
	unsigned long long gaddr_base6;
	unsigned long long gaddr_base7;
	unsigned int seq_no;
};

struct cvi_unload_tee_arg {
	unsigned long long addr;
	unsigned long long size;
};

struct cvi_tdma_copy_arg {
	unsigned long long paddr_src;
	unsigned long long paddr_dst;
	unsigned int h;
	unsigned int w_bytes;
	unsigned int stride_bytes_src;
	unsigned int stride_bytes_dst;
	unsigned int enable_2d;
	unsigned int leng_bytes;
	unsigned int seq_no;
};

struct cvi_tdma_wait_arg {
	unsigned int seq_no;
	int ret;
};

#define IOCTL_TPU_BASE 'p'
#define CVITPU_SUBMIT_DMABUF _IOW(IOCTL_TPU_BASE, 0x01, unsigned long long)
#define CVITPU_DMABUF_FLUSH_FD _IOW(IOCTL_TPU_BASE, 0x02, unsigned long long)
#define CVITPU_DMABUF_INVLD_FD _IOW(IOCTL_TPU_BASE, 0x03, unsigned long long)
#define CVITPU_DMABUF_FLUSH _IOW(IOCTL_TPU_BASE, 0x04, unsigned long long)
#define CVITPU_DMABUF_INVLD _IOW(IOCTL_TPU_BASE, 0x05, unsigned long long)
#define CVITPU_WAIT_DMABUF _IOWR(IOCTL_TPU_BASE, 0x06, unsigned long long)
#define CVITPU_PIO_MODE _IOW(IOCTL_TPU_BASE, 0x07, unsigned long long)

#define CVITPU_LOAD_TEE _IOWR(IOCTL_TPU_BASE, 0x08, unsigned long long)
#define CVITPU_SUBMIT_TEE _IOW(IOCTL_TPU_BASE, 0x09, unsigned long long)
#define CVITPU_UNLOAD_TEE _IOW(IOCTL_TPU_BASE, 0x0A, unsigned long long)
#define CVITPU_SUBMIT_PIO _IOW(IOCTL_TPU_BASE, 0x0B, unsigned long long)
#define CVITPU_WAIT_PIO _IOWR(IOCTL_TPU_BASE, 0x0C, unsigned long long)

#endif /* __CVI_TPU_IOCTL_H__ */
