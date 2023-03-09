#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN32) || defined(__MINGW32__)
#elif defined(linux) || defined(__linux) || defined(ANDROID)

#include <malloc.h>
#include "jdi.h"
#include "jpulog.h"
#include "jdi_osal.h"
#include "dm/device.h"
#include "timer.h"
#include "asm/cache.h"

#define JPU_BIT_REG_SIZE    0x1000
#define JPU_BIT_REG_BASE    (0x0B000000)

/*
 * DRAM Total Size = 256MB =   0x10000000
 * DRAM_PHYSICAL_BASE =       0x100000000

	   | Offset      | Size      | Note
 * System     | 0x100000000 | 0x2000000 | 32MB
 * System Mem | 0x102000000 | 0x4000000 | 64MB
 * File Buf   | 0x106000000 | 0x2000000 | 32MB
 * Enc BS     | 0x108000000 | 0x1000000 | 16MB
 * ENC SRC    | 0x10C000000 | 0x1000000 | 16MB
 * LOAD SRC   | 0x10D000000
 */
//#define SYSTEM_SIZE                0x2000000
#define JDI_DRAM_PHYSICAL_BASE   (0x130000000)
#define JDI_DRAM_PHYSICAL_SIZE    0x00200000 //0x004000000

#define JDI_SYSTEM_ENDIAN	JDI_LITTLE_ENDIAN

#define SPM_MEM_PHYSICAL_BASE 0x140000000
#define SPM_MEM_PHYSICAL_SIZE 0xC0000000
typedef struct jpu_buffer_t jpudrv_buffer_t;

typedef struct jpu_buffer_pool_t {
	jpudrv_buffer_t jdb;
	int inuse;
} jpu_buffer_pool_t;

static int s_jpu_fd;
static jpu_instance_pool_t *s_pjip;
static jpu_instance_pool_t s_jip;
static int s_task_num;
static int s_clock_state;
static jpudrv_buffer_t s_jdb_video_memory;
static jpudrv_buffer_t s_jdb_register;
static jpu_buffer_pool_t s_jpu_buffer_pool[MAX_JPU_BUFFER_POOL];
static int s_jpu_buffer_pool_count;

static int jpu_swap_endian(unsigned char *data, int len, int endian);
int jdi_probe(void)
{
	int ret;

	ret = jdi_init();
	jdi_release();

	return ret;
}

int jdi_init(void)
{
	int ret;
	void *buf = NULL;

	if (s_jpu_fd != -1 && s_jpu_fd != 0x00) {
		s_task_num++;
		return 0;
	}

	s_jpu_fd = 1;

	memset((void *)&s_jpu_buffer_pool, 0x00, sizeof(jpu_buffer_pool_t) * MAX_JPU_BUFFER_POOL);
	s_jpu_buffer_pool_count = 0;

	s_pjip = jdi_get_instance_pool();
	if (!s_pjip) {
		JLOG(ERR, "[jdi] fail to create instance pool for saving context\n");
		goto ERR_JDI_INIT;
	}

	buf = malloc(JDI_DRAM_PHYSICAL_SIZE);

	if (buf) {
		s_jdb_video_memory.phys_addr = (unsigned long)buf;
		s_jdb_video_memory.size = JDI_DRAM_PHYSICAL_SIZE;
		JLOG(INFO, "alloc s_jdb_video_memory.phys_addr buf %p\n", buf);
	} else {
		JLOG(ERR, "alloc s_jdb_video_memory.phys_addr failed\n");
		goto ERR_JDI_INIT;
	}

	if (!s_pjip->instance_pool_inited) {
		memset(&s_pjip->vmem, 0x00, sizeof(jpeg_mm_t));
		ret = jmem_init(&s_pjip->vmem, (unsigned long)s_jdb_video_memory.phys_addr, s_jdb_video_memory.size);
		if (ret < 0) {
			JLOG(ERR, "[JDI] fail to init jpu memory management logic\n");
			goto ERR_JDI_INIT;
		}
	}

	s_jdb_register.phys_addr = JPU_BIT_REG_BASE;
	s_jdb_register.virt_addr = JPU_BIT_REG_BASE;
	s_jdb_register.size = JPU_BIT_REG_SIZE;

	jdi_set_clock_gate(1);

	s_task_num++;

	JLOG(INFO, "[jdi] success to init driver\n");
	return s_jpu_fd;

ERR_JDI_INIT:

	jdi_release();
	return -1;
}

int jdi_release(void)
{

	if (s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return 0;

	if (jdi_lock() < 0) {
		JLOG(ERR, "[jdi] fail to handle lock function\n");
		return -1;
	}

	if (s_task_num > 1) {// means that the opened instance remains
		s_task_num--;
		jdi_unlock();
		return 0;
	}

	if (s_jdb_video_memory.phys_addr) {
		JLOG(INFO, "free s_jdb_video_memory.phys_addr buf %p\n", (void *)s_jdb_video_memory.phys_addr);
		free((void *)s_jdb_video_memory.phys_addr);
	}
	s_task_num--;

	jmem_exit(&s_pjip->vmem);

	memset(&s_jdb_register, 0x00, sizeof(jpudrv_buffer_t));

	if (s_jpu_fd != -1 && s_jpu_fd != 0x00)
		s_jpu_fd = -1;

	s_pjip = NULL;

	jdi_unlock();

	return 0;
}

jpu_instance_pool_t *jdi_get_instance_pool(void)
{
	if (!s_pjip) {
		s_pjip = &s_jip;

		memset(s_pjip, 0x00, sizeof(jpu_instance_pool_t));
	}

	return (jpu_instance_pool_t *)s_pjip;
}

int jdi_open_instance(unsigned long instIdx)
{
	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	s_pjip->jpu_instance_num++;

	return 0;
}

int jdi_close_instance(unsigned long instIdx)
{
	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	s_pjip->jpu_instance_num--;

	return 0;
}

int jdi_get_instance_num(void)
{
	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	return s_pjip->jpu_instance_num;
}

int jdi_hw_reset(void)
{
	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	// to do any action for hw reset

	return 0;
}

int jdi_lock(void)
{
	return 0;
}

void jdi_unlock(void)
{

}

void jdi_write_register(unsigned int addr, unsigned int data)
{
	unsigned long reg_addr;

	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return;

	reg_addr = addr + (unsigned long)s_jdb_register.virt_addr;
	*(unsigned int *)reg_addr = data;
	flush_dcache_range(reg_addr, reg_addr + sizeof(unsigned int));
//	flush_dcache_all();
}

unsigned int jdi_read_register(unsigned int addr)
{
	unsigned long reg_addr;

	reg_addr = addr + (unsigned long)s_jdb_register.virt_addr;
//	INV_DCACHE_RANGE((unsigned int)reg_addr, sizeof(unsigned int));
//	invalidate_dcache_all();
	invalidate_dcache_range(reg_addr, reg_addr + sizeof(unsigned int));

	return *(unsigned int *)reg_addr;
}

int jdi_write_memory(unsigned long addr, unsigned char *data, int len, int endian)
{
	jpudrv_buffer_t jdb = {0, };
	unsigned long offset;
	int i;

	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	for (i = 0; i < MAX_JPU_BUFFER_POOL; i++) {
		if (s_jpu_buffer_pool[i].inuse == 1) {
			jdb = s_jpu_buffer_pool[i].jdb;
			if (addr >= jdb.phys_addr && addr < (jdb.phys_addr + jdb.size))
				break;
		}
	}

	if (!jdb.size) {
		JLOG(ERR, "address 0x%08lx is not mapped address!!!\n", addr);
		return -1;
	}

	offset = addr - (unsigned long)jdb.phys_addr;

	jpu_swap_endian(data, len, endian);

	//by zhao for cache testing
	OSAL_MEMCPY((void *)((unsigned long)jdb.virt_addr + offset), (void *)data, len);
	//josal_memcpy_nocache((void *)((unsigned long)jdb.virt_addr+offset), (void *)data, len);
	return len;
}

int jdi_read_memory(unsigned long addr, unsigned char *data, int len, int endian)
{
	jpudrv_buffer_t jdb = {0};
	unsigned long offset;
	int i;

	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	for (i = 0; i < MAX_JPU_BUFFER_POOL; i++) {
		if (s_jpu_buffer_pool[i].inuse == 1) {
			jdb = s_jpu_buffer_pool[i].jdb;
			if (addr >= jdb.phys_addr && addr < (jdb.phys_addr + jdb.size))
				break;
		}
	}

	if (!jdb.size)
		return -1;

	offset = addr - (unsigned long)jdb.phys_addr;

	INV_DCACHE_RANGE(((unsigned long)jdb.virt_addr + offset), len);
	OSAL_MEMCPY(data, (const void *)((unsigned long)jdb.virt_addr + offset), len);
	jpu_swap_endian(data, len,  endian);

	return len;
}

int jdi_allocate_dma_memory(jpu_buffer_t *vb)
{
	int i;

	unsigned long offset;
	jpudrv_buffer_t jdb = {0, };

	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return -1;

	jdb.size = vb->size;
	jdb.phys_addr = (unsigned long)jmem_alloc(&s_pjip->vmem, jdb.size, 0);

	if (jdb.phys_addr == (unsigned long)-1)
		return -1; // not enough memory

	offset = (unsigned long)(jdb.phys_addr - s_jdb_video_memory.phys_addr);
	jdb.base = (unsigned long)s_jdb_video_memory.base + offset;
	jdb.virt_addr = jdb.phys_addr;

	vb->phys_addr = (unsigned long)jdb.phys_addr;
	vb->base = (unsigned long)jdb.base;
	vb->virt_addr = (unsigned long)vb->phys_addr;

	for (i = 0; i < MAX_JPU_BUFFER_POOL; i++) {
		if (s_jpu_buffer_pool[i].inuse == 0) {
			s_jpu_buffer_pool[i].jdb = jdb;
			s_jpu_buffer_pool_count++;
			s_jpu_buffer_pool[i].inuse = 1;
			break;
		}
	}

	return 0;
}

void jdi_free_dma_memory(jpu_buffer_t *vb)
{
	int i;
//	int ret = 0;
	jpudrv_buffer_t jdb = {0, };

	if (!s_pjip || s_jpu_fd == -1 || s_jpu_fd == 0x00)
		return;

	if (vb->size == 0)
		return;

	for (i = 0; i < MAX_JPU_BUFFER_POOL; i++) {
		if (s_jpu_buffer_pool[i].jdb.phys_addr == vb->phys_addr) {
			s_jpu_buffer_pool[i].inuse = 0;
			s_jpu_buffer_pool_count--;
			jdb = s_jpu_buffer_pool[i].jdb;
			break;
		}
	}

	if (!jdb.size) {
		JLOG(ERR, "[JDI] invalid buffer to free address = 0x%x\n", (int)jdb.virt_addr);
		return;
	}

	jmem_free(&s_pjip->vmem, (unsigned long)jdb.phys_addr, 0);
	memset(vb, 0, sizeof(jpu_buffer_t));
}

int jdi_set_clock_gate(int enable)
{
	s_clock_state = enable;

	return 0;
}

int jdi_get_clock_gate(void)
{
	return s_clock_state;
}

static int intr_reason;

int irq_handler_jpeg_codec(int irqn, void *priv)
{
	int curr_int = 0;

#ifdef PROFILE_PERFORMANCE
	int ms;

	ms = timer_meter_get_ms();
	BM_DBG_PERF("time = %d ms\n", ms);
#endif

	curr_int = jdi_read_register(MJPEG_PIC_STATUS_REG);

	intr_reason |= curr_int;
	BM_DBG_TRACE("curr_int = 0x%X, intr_reason = 0x%X\n", curr_int, intr_reason);

	JpuWriteReg(MJPEG_PIC_STATUS_REG, curr_int);

	return 0;
}

int jdi_wait_interrupt(int timeout)
{
#ifdef SUPPORT_INTERRUPT
	int out_reason = 0;

	while (1) {
		BM_DBG_TRACE("intr_reason = 0x%X\n", intr_reason);

		out_reason = intr_reason;
		if (out_reason) {
			int int_en = jdi_read_register(MJPEG_INTR_MASK_REG);

			JpuWriteReg(MJPEG_INTR_MASK_REG, 0);

			intr_reason &= (~out_reason);
			BM_DBG_TRACE("out_reason = 0x%X\n", out_reason);

			JpuWriteReg(MJPEG_INTR_MASK_REG, int_en);
			break;
		}
	}

	return out_reason;
#else

	while (1) {
		if (jdi_read_register(MJPEG_PIC_STATUS_REG))
			break;

		//Sleep(1);	// 1ms sec
		//if (count++ > timeout)
		//	return -1;
	}

	return 0;
#endif
}

void jdi_log(int cmd, int step)
{
	int i;

	switch (cmd) {
	case JDI_LOG_CMD_PICRUN:
		if (step == 1) {
			JLOG(INFO, "\n**PIC_RUN start\n");
		} else {
			JLOG(INFO, "\n**PIC_RUN end\n");
		}
		break;
	}

	for (i = 0; i <= 0x238; i = i + 16) {
		JLOG(INFO, "0x%04xh: 0x%08x 0x%08x 0x%08x 0x%08x\n", i,
		     jdi_read_register(i), jdi_read_register(i + 4),
		     jdi_read_register(i + 8), jdi_read_register(i + 0xc));
	}
}

int jpu_swap_endian(unsigned char *data, int len, int endian)
{
	unsigned long *p;
	unsigned long v1, v2, v3;
	int i;
	int swap = 0;

	p = (unsigned long *)data;

	if (endian == JDI_SYSTEM_ENDIAN)
		swap = 0;
	else
		swap = 1;

	if (swap) {
		if (endian == JDI_LITTLE_ENDIAN || endian == JDI_BIG_ENDIAN) {
			for (i = 0; i < len / 4; i += 2) {
				v1 = p[i];
				v2  = (v1 >> 24) & 0xFF;
				v2 |= ((v1 >> 16) & 0xFF) <<  8;
				v2 |= ((v1 >>  8) & 0xFF) << 16;
				v2 |= ((v1 >>  0) & 0xFF) << 24;
				v3 =  v2;
				v1  = p[i + 1];
				v2  = (v1 >> 24) & 0xFF;
				v2 |= ((v1 >> 16) & 0xFF) <<  8;
				v2 |= ((v1 >>  8) & 0xFF) << 16;
				v2 |= ((v1 >>  0) & 0xFF) << 24;
				p[i]   =  v2;
				p[i + 1] = v3;
			}
		} else {
			int sys_endian = JDI_SYSTEM_ENDIAN;
			int swap4byte = 0;

			swap = 0;

			if (endian == JDI_32BIT_LITTLE_ENDIAN) {
				if (sys_endian == JDI_BIG_ENDIAN) {
					swap = 1;
				}
			} else {
				if (sys_endian == JDI_BIG_ENDIAN) {
					swap4byte = 1;
				} else if (sys_endian == JDI_LITTLE_ENDIAN) {
					swap4byte = 1;
					swap = 1;
				} else {
					swap = 1;
				}
			}
			if (swap) {
				for (i = 0; i < len / 4; i++) {
					v1 = p[i];
					v2  = (v1 >> 24) & 0xFF;
					v2 |= ((v1 >> 16) & 0xFF) <<  8;
					v2 |= ((v1 >>  8) & 0xFF) << 16;
					v2 |= ((v1 >>  0) & 0xFF) << 24;
					p[i] = v2;
				}
			}
			if (swap4byte) {
				for (i = 0; i < len / 4; i += 2) {
					v1 = p[i];
					v2 = p[i + 1];
					p[i]   = v2;
					p[i + 1] = v1;
				}
			}
		}
	}
	return swap;
}

#endif
