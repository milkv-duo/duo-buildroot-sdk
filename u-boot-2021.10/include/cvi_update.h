#ifndef __CVI_UPDATE_H__
#define __CVI_UPDATE_H__

#define EXTRA_FLAG_SIZE 32

#undef pr_debug
#ifdef DEBUG
#define pr_debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define pr_debug(fmt, ...)
#endif

int _prgImage(char *file, uint32_t chunk_header_size, char *file_name);

#endif /* __CVI_UPDATE_H__ */
