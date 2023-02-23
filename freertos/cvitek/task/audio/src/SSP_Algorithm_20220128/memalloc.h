
#ifndef MEMALLOC_H
#define MEMALLOC_H

 /* Include files */
#include <string.h>
#include <stdio.h>
#ifndef PLATFORM_RTOS
#include <stdlib.h>
#else
#include "malloc.h"
#endif
//#ifndef printf
//#define printf(...) printf(__VA_ARGS__)
//#endif
#define pr_err(...) printf("ERR:" __VA_ARGS__)
#define printfw(...)
#define printfe(...)
#define _inline

/** Wrapper for calloc. To do your own dynamic allocation, all you need to do is to replace this function, mem_realloc and mem_free
        Note tthat  mem_alloc needs to CLEAR THE MEMORY */
#ifndef OVERRIDE_SPEECH_ALLOC
static _inline void *mem_alloc (int size)
{
   /* WARNING: this is not equivalent to malloc(). If you want to use malloc()
      or your own allocator, YOU NEED TO CLEAR THE MEMORY ALLOCATED. Otherwise
      you will experience strange bugs */
#ifndef PLATFORM_RTOS
   return calloc(size,1);
#else
   //return (void *)calloc(size,1);
   uintptr_t *ptr =  pvPortMalloc(size);

   memset(ptr, 0, size);
   return ptr;
#endif
}
#endif

/** Wrapper for calloc. To do your own dynamic allocation, all you need to do is replace this function, mem_realloc and mem_alloc */
#ifndef OVERRIDE_SPEECH_FREE
static _inline void mem_free (void *ptr)
{
#ifndef PLATFORM_RTOS
   free(ptr);
#else
   vPortFree(ptr);
#endif
}
#endif

/** Copy n elements from src to dst. The 0* term provides compile-time type checking  */
#ifndef OVERRIDE_SPEECH_COPY
#define SPEECH_COPY(dst, src, n) (memcpy((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#endif

/** Copy n elements from src to dst, allowing overlapping regions. The 0* term provides compile-time type checking */
#ifndef OVERRIDE_SPEECH_MOVE
#define SPEECH_MOVE(dst, src, n) (memmove((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#endif

/** For n elements worth of memory, set every byte to the value of c, starting at address dst */
#ifndef OVERRIDE_SPEECH_MEMSET
#define SPEECH_MEMSET(dst, c, n) (memset((dst), (c), (n)*sizeof(*(dst))))
#endif

/*#ifndef OVERRIDE_SPEECH_FATAL
static _inline void _speech_fatal(const char *str, const char *file, int line)
{
   printfe("Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   fprintf (stderr, "Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   exit(1);
}
#endif */

/*#ifndef OVERRIDE_SPEECH_WARNING
static _inline void speech_warning(const char *str)
{
#ifndef DISABLE_WARNINGS
   printfw("warning: %s\n", str);
   fprintf (stderr, "warning: %s\n", str);
#endif
}
#endif */

/*#define speech_fatal(str) _speech_fatal(str, __FILE__, __LINE__); */

#endif


