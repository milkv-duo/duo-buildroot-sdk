//------------------------------------------------------------------------------
// File: vdi_osal.c
//
// Copyright (c) 2006, Chips & Media.  All rights reserved.
//------------------------------------------------------------------------------
//#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN32) || defined(__MINGW32__)
//#elif defined(linux) || defined(__linux) || defined(ANDROID)
//#else

#include <linux/string.h>
#include <stdarg.h>
#include <malloc.h>

#include "jdi.h"
#include "jdi_osal.h"
#include "asm/cache.h"
#include "jpulog.h"

#if defined(JPEG_FVP)
#define FILE_BUFFER_BASE 0xE0000000
#elif defined(JPEG_FPGA)
#define FILE_BUFFER_BASE 0x116000000
#endif

#define FILE_BUFFER_BASE 0x138000000

#define MAX_LIST_NUM                1
#define DEC_BS_FILE_BUFFER_BASE  FILE_BUFFER_BASE
#define DEC_BS_FILE_BUFFER_SIZE  (0x400000 * MAX_LIST_NUM)
#define DEC_YUV_FILE_BUFFER_BASE  (DEC_BS_FILE_BUFFER_BASE + DEC_BS_FILE_BUFFER_SIZE)
#define DEC_YUV_FILE_BUFFER_SIZE (0x800000 * MAX_LIST_NUM)

#define ENC_YUV_FILE_BUFFER_BASE (DEC_YUV_FILE_BUFFER_BASE + DEC_YUV_FILE_BUFFER_SIZE)
#define ENC_YUV_FILE_BUFFER_SIZE (0x800000 * MAX_LIST_NUM)
#define ENC_BS_FILE_BUFFER_BASE  (ENC_YUV_FILE_BUFFER_BASE + ENC_YUV_FILE_BUFFER_SIZE)
#define ENC_BS_FILE_BUFFER_SIZE  (0x400000 * MAX_LIST_NUM)
#define ENC_HUFF_FILE_BUFFER_BASE (ENC_BS_FILE_BUFFER_BASE + ENC_BS_FILE_BUFFER_SIZE)
#define ENC_HUFF_FILE_BUFFER_SIZE (0x100000 * MAX_LIST_NUM)
#define ENC_QMAT_FILE_BUFFER_BASE (ENC_HUFF_FILE_BUFFER_BASE + ENC_HUFF_FILE_BUFFER_BASE)
#define ENC_QMAT_FILE_BUFFER_SIZE (0x100000 * MAX_LIST_NUM)
#define ENC_QP_FILE_BUFFER_BASE   (ENC_QMAT_FILE_BUFFER_BASE + ENC_QMAT_FILE_BUFFER_SIZE)
#define ENC_QP_FILE_BUFFER_SIZE   (0x100000 * MAX_LIST_NUM)
#define ENC_CFG_FILE_BUFFER_BASE  (ENC_QP_FILE_BUFFER_BASE + ENC_QP_FILE_BUFFER_SIZE)
#define ENC_CFG_FILE_BUFFER_SIZE  (0x10000 * MAX_LIST_NUM)

#define MULTI_FILE_BUFFER_BASE      (ENC_CFG_FILE_BUFFER_BASE + ENC_CFG_FILE_BUFFER_SIZE)
#define MULTI_FILE_BUFFER_SIZE      (0x10000)
#define MULTI_YUV_FILE_BUFFER_BASE  (MULTI_FILE_BUFFER_BASE + MULTI_FILE_BUFFER_SIZE)
#define MULTI_YUV_FILE_BUFFER_SIZE  (0x400000)

#if defined(JPEG_FVP)
#define LOG_MSG_BUF_BASE 0xD0000000
#elif defined(JPEG_FPGA)
#define LOG_MSG_BUF_BASE 0x120500000
#endif

#define LOG_MSG_BUF_BASE 0x120500000 // fixme fix me

#define LOG_MSG_BUF_SIZE 0x100000
static char *LOG_MSG_BUF = (char *)LOG_MSG_BUF_BASE;

#define MAX_MALLOC_BLOCK_SIZE   0x50000//0x200000
#define MAX_MALLOC_BLOCK_NUM    7

int jpu_level = BM_MASK_ERR;

//To
#define FILE_BUFFER_SIZE 0x1000000 * (MAX_FD_NUM - 1)
#define CMP_FILE_BUFFER_SIZE 0x1000000
unsigned char osal_malloc_heap[MAX_MALLOC_BLOCK_NUM][MAX_MALLOC_BLOCK_SIZE];   // allocate 64 4M-block for malloc
unsigned char osal_malloc_used[MAX_MALLOC_BLOCK_NUM] = {0};

typedef struct fileio_buf_t {
	char *_ptr;
	int   _cnt;
	char *_base;
	int   _flag;
	int   _file;
	int   _charbuf;
	int   _bufsiz;
	char *_tmpfname;
} fileio_buf_t;

#define MAX_FD_NUM  5		// 0-bitstream file 1-yuv cmp file 2-
static fileio_buf_t s_file[MAX_FD_NUM] = {0};

static void myprintf(char *MsgBuf)
{
	int size = strlen(MsgBuf);

	if (size == 0)
		return;

	if (LOG_MSG_BUF + size  >= (char *)(LOG_MSG_BUF_BASE + LOG_MSG_BUF_SIZE))
		return;

	josal_memcpy(LOG_MSG_BUF, MsgBuf, size);

	LOG_MSG_BUF += size;
}

void *josal_memcpy(void *dst, const void *src, int count)
{
	void *ret;

	ret = memcpy(dst, src, count);
	flush_dcache_range((unsigned long)dst, count);
	return ret;
}

void *josal_memset(void *dst, int val, int count)
{
	return memset(dst, val, count);
}

int josal_memcmp(const void *src, const void *dst, int size)
{
	return memcmp(src, dst, size);
}

void *josal_malloc(int size)
{
	int i;

	if (size > MAX_MALLOC_BLOCK_SIZE || size == 0)
		return NULL;

	for (i = 0; i < MAX_MALLOC_BLOCK_NUM; i++)
		if (osal_malloc_used[i] == 0)
			break;

	if (i < MAX_MALLOC_BLOCK_NUM) {
		osal_malloc_used[i] = 1;
//		JLOG(INFO, "malloc: %d, addr: 0x%p, size: %d\n", i, osal_malloc_heap[i], size);
		return (void *)osal_malloc_heap[i];
	}

	return NULL;

}

void *josal_realloc(void *ptr, int size)
{
	if (!ptr)
		return josal_malloc(size);

	if (size == 0 || size > MAX_MALLOC_BLOCK_SIZE) {
		josal_free(ptr);
		return NULL;
	}

	if (size <= MAX_MALLOC_BLOCK_SIZE)
		return ptr;

	return NULL;
}

void josal_free(void *p)
{
	//free(p);
	int i;

	for (i = 0; i < MAX_MALLOC_BLOCK_NUM; i++)
		if ((void *)osal_malloc_heap[i] == p)
			break;

	osal_malloc_used[i] = 0;
}

int josal_fflush(osal_file_t fp)
{
	return 1;
}

int josal_feof(osal_file_t fp)
{
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	if ((uint64_t)p_fp->_ptr >= p_fp->_bufsiz)
		return 1;
	else
		return 0;
}

osal_file_t josal_fopen(const char *osal_file_tname, const char *mode)
{
	int i;

	for (i = 0; i < MAX_FD_NUM; i++)
		if (s_file[i]._bufsiz == 0)		// not used
			break;

	if (i == MAX_FD_NUM)
		return NULL;

	if (i != 1)		// 1 - cmp file
		s_file[i]._bufsiz = FILE_BUFFER_SIZE / (MAX_FD_NUM - 1);
	else
		s_file[i]._bufsiz = CMP_FILE_BUFFER_SIZE;		// 256M for YUV compare file

	if (i == 0)
		s_file[i]._base = (char *)FILE_BUFFER_BASE;
	else
		s_file[i]._base = s_file[i - 1]._base + s_file[i - 1]._bufsiz;

	s_file[i]._ptr = (char *)0;

	for (i = 0; i < MAX_FD_NUM; i++) {
		JLOG(INFO, "file = %d, base = 0x%lX, size = 0x%lX\n", i,
		     (unsigned long)s_file[i]._base,
		     (unsigned long)s_file[i]._bufsiz);
	}

	return &s_file[i];
}

size_t josal_fwrite(const void *p, int size, int count, osal_file_t fp)
{
	long addr;
	long real_size;
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	if (!p_fp)
		return 0;

	if ((unsigned long)(size * count + p_fp->_ptr) > p_fp->_bufsiz)
		real_size = p_fp->_bufsiz - (unsigned long)p_fp->_ptr;
	else
		real_size = size * count;

	addr = (long)((long)p_fp->_base + (long)p_fp->_ptr);
	josal_memcpy((void *)addr, (void *)p, real_size);
	p_fp->_ptr += real_size;

	JLOG(INFO, "fp: 0x%lx, size: %ld\n", addr, real_size);
	return real_size;
}

size_t josal_fread(void *p, int size, int count, osal_file_t fp)
{
	long addr;
	long real_size;
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	if (!p_fp)
		return 0;

	if ((unsigned long)(size * count + p_fp->_ptr) > p_fp->_bufsiz)
		real_size = p_fp->_bufsiz - (unsigned long)p_fp->_ptr;
	else
		real_size = size * count;

	addr = (long)((long)p_fp->_base + (long)p_fp->_ptr);
	josal_memcpy((void *)p, (void *)addr, real_size);
	p_fp->_ptr += real_size;

	//printf("p_fp: _ptr = 0x%016llx _base = 0x%016llx _bufsiz = 0x%08x\n",
	//       (uint64_t)p_fp->_ptr, (uint64_t)p_fp->_base, p_fp->_bufsiz);

	return real_size;
}

char *josal_fgets(void *p, int size, osal_file_t fp)
{
	int s = josal_fread(p, 1, size, fp);

	if (s == size)
		return p;
	else
		return NULL;
}

char josal_fgetc(osal_file_t fp)
{
	char *ptr;
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	if (!p_fp)
		return -1;

	if ((unsigned long)p_fp->_ptr + sizeof(char) == p_fp->_bufsiz)
		return -1;

	ptr =  p_fp->_base + (unsigned long)p_fp->_ptr;
	p_fp->_ptr++;

	return *ptr;
}

size_t josal_fputs(const char *s, osal_file_t fp)
{
	return josal_fwrite(s, sizeof(char), strlen(s), fp);
}

long josal_ftell(osal_file_t fp)
{
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	return p_fp->_bufsiz;
}

int josal_fseek(osal_file_t fp, long offset, int origin)
{
	char *curr_p;
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	if (!fp)
		return -1;

	switch (origin) {
	case SEEK_CUR:
		curr_p = (char *)p_fp->_ptr;
		break;
	case SEEK_END:
		curr_p = (char *)(uint64_t)p_fp->_bufsiz;
		break;
	case SEEK_SET:
		curr_p = (char *)0;
		break;
	default:
		return -1;
	}

	p_fp->_ptr = curr_p + offset;
	if (p_fp->_ptr > p_fp->_base + p_fp->_bufsiz)
		p_fp->_ptr = p_fp->_base + p_fp->_bufsiz;

	return 0;
}

int josal_fclose(osal_file_t fp)
{
	fileio_buf_t *p_fp = (fileio_buf_t *)fp;

	if (!p_fp)
		return -1;

	p_fp->_base = (char *)0;
	p_fp->_bufsiz = 0;
	p_fp->_ptr = (char *)0;

	return 1;
}

int josal_fscanf(osal_file_t fp, const char *_Format, ...)
{
	return 1;
}

int josal_fprintf(osal_file_t fp, const char *_Format, ...)
{
	va_list ptr;
	char logBuf[MAX_PRINT_LENGTH] = {0};

	va_start(ptr, _Format);

	vsnprintf(logBuf, MAX_PRINT_LENGTH, _Format, ptr);

	va_end(ptr);

	myprintf(logBuf);

	return 1;

}

int josal_kbhit(void)
{
	return 0;
}

int josal_getch(void)
{
	return -1;
}

int josal_flush_ch(void)
{
	return -1;
}

int josal_srand(int seed)
{
	return 0;
}

/* to return a integer between 0~FEEDING_MAX_SIZE(4M) */
int josal_rand(void)
{
	return 0x10000;
}

/* to conver c to upper case */
int josal_toupper(int c)
{
	int ret = c;
	char *ptr = (char *)&ret;
	int i;

	for (i = 0; i < sizeof(int); i++) {
		if (ptr[i] > 96 && ptr[i] < 123)
			ptr[i++] -= 32;
	}
	return ret;
}

void jinv_dcache_range(unsigned long start, unsigned long size)
{
	invalidate_dcache_range(start, size);
}

#ifdef LIB_C_STUB

/*
 * newlib_stubs.c
 * the bellow code is just to build ref-code.
 * customers will removed the bellow code bacuase they need a library which is related to the
 * system library such as newlibc
 */
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#ifndef STDOUT_USART
#define STDOUT_USART 0
#endif

#ifndef STDERR_USART
#define STDERR_USART 0
#endif

#ifndef STDIN_USART
#define STDIN_USART 0
#endif

#undef errno
int errno;
extern int errno;

/*
 * environ
 * A pointer to a list of environment variables and their values.
 * For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

//int _write(int file, char *ptr, int len);

void _exit(int status)
{
	_write(1, "exit", 4);
	while (1) {
		;
	}
}

int _close(int file)
{
	return -1;
}

/*
 * execve
 * Transfer control to a new process. Minimal implementation (for a system without processes):
 */
int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}

/*
 * fork
 * Create a new process. Minimal implementation (for a system without processes):
 */
int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

/*
 * fstat
 * Status of an open file. For consistency with other minimal implementations in these examples,
 * all files are regarded as character special devices.
 * The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

/*
 * getpid
 * Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes.
 * Minimal implementation,
 * for a system without processes:
 */
int _getpid(void)
{
	return 1;
}

/*
 * isatty
 * Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file)
{
	switch (file) {
	case STDOUT_FILENO:
	case STDERR_FILENO:
	case STDIN_FILENO:
		return 1;
	default:
		//errno = ENOTTY;
		errno = EBADF;
		return 0;
	}
}

/*
 * kill
 * Send a signal. Minimal implementation:
 */
int _kill(int pid, int sig)
{
	errno = EINVAL;
	return (-1);
}

/*
 * link
 * Establish a new name for an existing file. Minimal implementation:
 */

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

/*
 * lseek
 * Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir)
{
	return 0;
}

/*
 * sbrk
 * Increase program data space.
 * Malloc and related functions depend on this
 */
caddr_t _sbrk(int incr)
{

	// extern char _ebss; // Defined by the linker
	char _ebss;
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &_ebss;
	}
	prev_heap_end = heap_end;

	heap_end += incr;
	return (caddr_t)prev_heap_end;
}

/*
 * read
 * Read a character to a file. `libc' subroutines will use this system routine for input from all files,
 * including stdin
 * Returns -1 on error or blocks until the number of characters have been read.
 */

int _read(int file, char *ptr, int len)
{
	int n;
	int num = 0;

	switch (file) {
	case STDIN_FILENO:
		for (n = 0; n < len; n++) {
			char c = 0;
#if   STDIN_USART == 1
			while ((USART1->SR & USART_FLAG_RXNE) == (u16)RESET) {
				}
			c = (char)(USART1->DR & (u16)0x01FF);
#elif STDIN_USART == 2
			while ((USART2->SR & USART_FLAG_RXNE) == (u16)RESET) {
				}
			c = (char)(USART2->DR & (u16)0x01FF);
#elif STDIN_USART == 3
			while ((USART3->SR & USART_FLAG_RXNE) == (u16)RESET) {
				}
			c = (char)(USART3->DR & (u16)0x01FF);
#endif
			*ptr++ = c;
			num++;
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return num;
}

/*
 * stat
 * Status of a file (by name). Minimal implementation:
 * int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */
int stat(const char *filepath, struct stat *st)
{
	return _stat(filepath, st);
}

int _stat(const char *filepath, struct stat *st)
{
	st->st_mode = S_IFCHR;
	st->st_size = CMP_FILE_BUFFER_SIZE;
	return 0;
}

/*
 * times
 * Timing information for current process. Minimal implementation:
 */
clock_t _times(struct tms *buf)
{
	return -1;
}

/*
 * unlink
 * Remove a file's directory entry. Minimal implementation:
 */
int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

/*
 * wait
 * Wait for a child process. Minimal implementation:
 */
int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

/*
 * write
 * Write a character to a file. `libc' subroutines will use this system routine for output to all files,
 * including stdout
 * Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len)
{
	int n;

	switch (file) {
	case STDOUT_FILENO: /*stdout*/
		for (n = 0; n < len; n++) {
#if STDOUT_USART == 1
			while ((USART1->SR & USART_FLAG_TC) == (u16)RESET) {
				}
			USART1->DR = (*ptr++ & (u16)0x01FF);
#elif STDOUT_USART == 2
			while ((USART2->SR & USART_FLAG_TC) == (u16)RESET) {
				}
			USART2->DR = (*ptr++ & (u16)0x01FF);
#elif STDOUT_USART == 3
			while ((USART3->SR & USART_FLAG_TC) == (u16)RESET) {
				}
			USART3->DR = (*ptr++ & (u16)0x01FF);
#endif
		}
		break;
	case STDERR_FILENO: /* stderr */
		for (n = 0; n < len; n++) {
#if STDERR_USART == 1
			while ((USART1->SR & USART_FLAG_TC) == (u16)RESET) {
				}
			USART1->DR = (*ptr++ & (u16)0x01FF);
#elif STDERR_USART == 2
			while ((USART2->SR & USART_FLAG_TC) == (u16)RESET) {
				}
			USART2->DR = (*ptr++ & (u16)0x01FF);
#elif STDERR_USART == 3
			while ((USART3->SR & USART_FLAG_TC) == (u16)RESET) {
				}
			USART3->DR = (*ptr++ & (u16)0x01FF);
#endif
		}
		break;
	default:
		errno = EBADF;
		return -1;
	}
	return len;
}

#endif
//#endif

