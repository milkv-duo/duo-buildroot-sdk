/*
 * Wrapper functions for accessing the file_struct fd array.
 */

#ifndef __LINUX_FILE_H
#define __LINUX_FILE_H

#include <linux/compiler.h>
#include <linux/types.h>

struct file;

struct fd {
	struct file *file;
	unsigned int flags;
};

#define FDPUT_FPUT       1
#define FDPUT_POS_UNLOCK 2

extern void put_unused_fd(unsigned int fd);
extern void fd_install(unsigned int fd, struct file *file);
extern unsigned long __fdget(unsigned int fd);

static inline struct fd __to_fd(unsigned long v)
{
	return (struct fd){(struct file *)(v & ~3), v & 3};
}

static inline struct fd fdget(unsigned int fd)
{
	return __to_fd(__fdget(fd));
}

typedef struct _iobuf {
	char *_ptr;
	int _cnt;
	char *_base;
	int _flag;
	int _file;
	int _charbuf;
	int _bufsiz;
	char *_tmpfname;
} FILE;

#define fprintf(...) 0
#define fseek(...) 0
#define ftell(...) 0
#define fread(...) 0
#define fwrite(...) 0
#define fputs(...) 0
#define fgets(...) 0
#define fgetc(...) 0
#define fflush(...) 0
#define feof(...) 0
#define fopen(...) 0
#define fclose(...) 0
#define sscanf(...) 0
#define sprintf(...) 0

#define EOF	(-1)
#endif
