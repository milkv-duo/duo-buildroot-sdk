
#ifndef _JDI_HPI_H_
#define _JDI_HPI_H_

#include <stdlib.h>
//#include <string.h>
#include "jpuconfig.h"
#include "regdefine.h"
#include "mm.h"
#include "jdi_osal.h"

#define MAX_JPU_BUFFER_POOL 32

#define JpuWriteReg(ADDR, DATA) jdi_write_register(ADDR, DATA) // system register write
#define JpuReadReg(ADDR) jdi_read_register(ADDR) // system register write
#define JpuWriteMem(ADDR, DATA, LEN, ENDIAN) jdi_write_memory(ADDR, DATA, LEN, ENDIAN) // system memory write
#define JpuReadMem(ADDR, DATA, LEN, ENDIAN) jdi_read_memory(ADDR, DATA, LEN, ENDIAN) // system memory write

typedef struct jpu_buffer_t {
	unsigned int size;
	unsigned long phys_addr;
	unsigned long base;
	unsigned long virt_addr;
} jpu_buffer_t;

typedef struct jpu_instance_pool_t {
	unsigned char jpgInstPool[MAX_NUM_INSTANCE][MAX_INST_HANDLE_SIZE];
	void *jpu_mutex;
	int jpu_instance_num;
	int instance_pool_inited;
	void *pendingInst;
	jpeg_mm_t vmem;
} jpu_instance_pool_t;

#ifdef SUPPORT_128BIT_BUS

typedef enum {
	JDI_128BIT_LITTLE_64BIT_LITTLE_ENDIAN = ((0 << 2) + (0 << 1) + (0 << 0)), //128 bit little, 64 bit little
	JDI_128BIT_BIG_64BIT_LITTLE_ENDIAN = ((1 << 2) + (0 << 1) + (0 << 0)), //128 bit big , 64 bit little
	JDI_128BIT_LITTLE_64BIT_BIG_ENDIAN = ((0 << 2) + (0 << 1) + (1 << 0)), //128 bit little, 64 bit big
	JDI_128BIT_BIG_64BIT_BIG_ENDIAN = ((1 << 2) + (0 << 1) + (1 << 0)), //128 bit big, 64 bit big
	JDI_128BIT_LITTLE_32BIT_LITTLE_ENDIAN = ((0 << 2) + (1 << 1) + (0 << 0)), //128 bit little, 32 bit little
	JDI_128BIT_BIG_32BIT_LITTLE_ENDIAN = ((1 << 2) + (1 << 1) + (0 << 0)), //128 bit big , 32 bit little
	JDI_128BIT_LITTLE_32BIT_BIG_ENDIAN = ((0 << 2) + (1 << 1) + (1 << 0)), //128 bit little, 32 bit big
	JDI_128BIT_BIG_32BIT_BIG_ENDIAN = ((1 << 2) + (1 << 1) + (1 << 0)), //128 bit big, 32 bit big
} EndianMode;

#define JDI_LITTLE_ENDIAN JDI_128BIT_LITTLE_64BIT_LITTLE_ENDIAN
#define JDI_BIG_ENDIAN JDI_128BIT_BIG_64BIT_BIG_ENDIAN
#ifndef BIT(x)
#define BIT(x) (1 << (x))
#endif
#define JDI_128BIT_ENDIAN_MASK BIT(2)
#define JDI_64BIT_ENDIAN_MASK BIT(1)
#define JDI_ENDIAN_MASK BIT(0)

#define JDI_32BIT_LITTLE_ENDIAN JDI_128BIT_LITTLE_32BIT_LITTLE_ENDIAN
#define JDI_32BIT_BIG_ENDIAN JDI_128BIT_LITTLE_32BIT_BIG_ENDIAN

#else

typedef enum {
	JDI_LITTLE_ENDIAN = 0,
	JDI_BIG_ENDIAN,
	JDI_32BIT_LITTLE_ENDIAN,
	JDI_32BIT_BIG_ENDIAN,
} EndianMode;
#endif

typedef enum {
	JDI_LOG_CMD_PICRUN  = 0,
	JDI_LOG_CMD_MAX
} jdi_log_cmd;

#if defined(__cplusplus)
extern "C" {
#endif
	int jdi_probe(void);
	int jdi_init(void);
	int jdi_release(void);	//this function may be called only at system off.
	jpu_instance_pool_t *jdi_get_instance_pool(void);
	int jdi_allocate_dma_memory(jpu_buffer_t *vb);
	void jdi_free_dma_memory(jpu_buffer_t *vb);

	int jdi_wait_interrupt(int timeout);
	int jdi_hw_reset(void);

	int jdi_set_clock_gate(int enable);
	int jdi_get_clock_gate(void);

	int jdi_open_instance(unsigned long instIdx);
	int jdi_close_instance(unsigned long instIdx);
	int jdi_get_instance_num(void);

	void jdi_write_register(unsigned int addr, unsigned int data);
	unsigned int jdi_read_register(unsigned int addr);

	int jdi_write_memory(unsigned long       addr, unsigned char *data, int len, int endian);
	int jdi_read_memory(unsigned long addr, unsigned char *data, int len, int endian);

	int jdi_lock(void);
	void jdi_unlock(void);
	void jdi_log(int cmd, int step);

#ifdef JPU_FPGA_PLATFORM
#define HPI_SET_TIMING_MAX 1000
	int jdi_set_timing_opt(void);
	int jdi_set_clock_freg(int Device, int OutFreqMHz, int InFreqMHz);
#endif

	int  getch(void);
	int kbhit(void);

#if defined(__cplusplus)
}
#endif

#endif //#ifndef _JDI_HPI_H_
