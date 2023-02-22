#ifndef SAMPLE_CVG_H
#define SAMPLE_CVG_H

#include <linux/f_cvg.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>

#define TOP_CVG_IOCTL_CONNSIG		CVG_IOCTL_CONNSIG
#define TOP_CVG_IOCTL_SUBMITURB		CVG_IOCTL_SUBMITURB
#define TOP_CVG_IOCTL_DISCARDURB	CVG_IOCTL_DISCARDURB
#define TOP_CVG_IOCTL_REAPTXURB		CVG_IOCTL_REAPTXURB
#define TOP_CVG_IOCTL_REAPRXURB		CVG_IOCTL_REAPRXURB
#define TOP_CVG_IOCTL_QUEUEION		CVG_IOCTL_QUEUEION

#define UINT_PTR_T			size_t*

typedef struct cvg_uurb			CVG_UURB_T;
typedef struct cvg_connectsignal	CVG_CONNECTSIGNAL_T;
typedef struct cvg_ion_queue		CVG_ION_QUEUE_T;

#endif
