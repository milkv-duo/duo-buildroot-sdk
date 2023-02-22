#ifndef __DECOMPRESS_H__
#define __DECOMPRESS_H__

enum COMPRESS_TYPE { COMP_NONE, COMP_LZMA, COMP_LZ4 };

int decompress(void *dst, size_t *dst_size, const void *src, size_t src_size, enum COMPRESS_TYPE type);

#endif /* __DECOMPRESS_H__ */
