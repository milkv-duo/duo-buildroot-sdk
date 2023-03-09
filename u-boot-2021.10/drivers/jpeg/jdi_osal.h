//------------------------------------------------------------------------------
// File: log.h
//
// Copyright (c) 2006, Chips & Media.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef _VDI_OSAL_H_
#define _VDI_OSAL_H_

//#include <stdio.h>
#include <stdlib.h>
//#include <ctype.h>
#include "cvi_jpeg_cfg.h"

#define MAX_PRINT_LENGTH 512

typedef void *osal_file_t;
# ifndef SEEK_SET
# define	SEEK_SET	0
# endif

# ifndef SEEK_CUR
# define	SEEK_CUR	1
# endif

# ifndef SEEK_END
# define	SEEK_END	2
# endif

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN32) || defined(__MINGW32__)
#elif defined(linux) || defined(__linux) || defined(ANDROID)

//#ifndef stdout
//# define	stdout	(void * )1
//#endif
//#ifndef stderr
//# define	stderr	(void * )1
//#endif

#define OSAL_MEMCPY         josal_memcpy
#define OSAL_MEMCMP         josal_memcmp
#define OSAL_MALLOC         josal_malloc
#define OSAL_FREE           josal_free
#define OSAL_FOPEN          josal_fopen
#define OSAL_FWRITE         josal_fwrite
#define OSAL_FREAD          josal_fread
#define OSAL_FSEEK          josal_fseek
#define OSAL_FCLOSE         josal_fclose
#define OSAL_FFLUSH         josal_fflush
#define OSAL_FEOF           josal_feof
#define OSAL_FGETS          josal_fgets
#define INV_DCACHE_RANGE    jinv_dcache_range

#endif

#if defined(__cplusplus)
extern "C" {
#endif

//memory
void *josal_memcpy(void *dst, const void *src, int count);
void *josal_memset(void *dst, int val, int count);
int   josal_memcmp(const void *src, const void *dst, int size);
void *josal_malloc(int size);
void *josal_realloc(void *ptr, int size);
void  josal_free(void *p);

osal_file_t josal_fopen(const char *osal_file_tname, const char *mode);
size_t josal_fwrite(const void *p, int size, int count, osal_file_t fp);
size_t josal_fread(void *p, int size, int count, osal_file_t fp);
long  josal_ftell(osal_file_t fp);
int   josal_fseek(osal_file_t fp, long offset, int origin);
int   josal_fclose(osal_file_t fp);
int   josal_fflush(osal_file_t fp);
int   josal_fprintf(osal_file_t fp, const char *_Format, ...);
int   josal_fscanf(osal_file_t fp, const char *_Format, ...);
int   josal_kbhit(void);
int   josal_getch(void);
int   josal_flush_ch(void);
int   josal_feof(osal_file_t fp);
void *josal_create_mutex(const char *name);
void  josal_close_mutex(void *handle);
int   josal_mutex_lock(void *handle);
int   josal_mutex_unlock(void *handle);
char  josal_fgetc(osal_file_t fp);
char *josal_fgets(void *p, int size, osal_file_t fp);
int   josal_srand(int seed);
int   josal_rand(void);
int   josal_toupper(int c);
size_t josal_fputs(const char *s, osal_file_t fp);
void  jinv_dcache_range(unsigned long start, unsigned long size);

#if defined(__cplusplus)
}
#endif

#endif //#ifndef _VDI_OSAL_H
