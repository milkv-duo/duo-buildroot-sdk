#include <debug.h>
#include <errno.h>
#include <string.h>
#include <decompress.h>
#include <LzmaDec.h>
#include <lz4frame.h>

static uintptr_t comp_alloc_buf;
static size_t comp_alloc_size;

static void SzInit(void)
{
	comp_alloc_buf = DECOMP_ALLOC_ADDR;
	comp_alloc_size = DECOMP_ALLOC_SIZE;
}

static void *SzAlloc(ISzAllocPtr p, size_t size)
{
	void *x;

	x = (void *)comp_alloc_buf;
	comp_alloc_buf += size;

	if (comp_alloc_buf >= (comp_alloc_buf + comp_alloc_size)) {
		ERROR("%s can't alloc %lu bytes\n", __func__, size);
		return NULL;
	}

	return x;
}

static void SzFree(ISzAllocPtr p, void *address)
{
}

int decompress_lzma(void *dst, size_t *dst_size, const void *src, size_t src_size)
{
	size_t uncomp_size = 0;
	uint32_t tmp;
	int ret;

	ISzAlloc alloc = { SzAlloc, SzFree };
	ELzmaStatus status = 0;
	const uint8_t *prop;

	if (!dst_size) {
		ERROR("%s: dst_size is NULL.\n", __func__);
		return -EINVAL;
	}

	SzInit();

	INFO("LZMA1/%p/%p.\n", dst, src);

	prop = src;
	src += LZMA_PROPS_SIZE;
	src_size -= LZMA_PROPS_SIZE;

	// Copy 4 bytes to uncomp_size
	memcpy(&tmp, src, sizeof(tmp));
	uncomp_size = tmp;

	src += 8;
	src_size -= 8;

	INFO("LZMA2/uncomp_size=0x%lx\n", uncomp_size);
	ret = LzmaDecode(dst, &uncomp_size, src, &src_size, prop, LZMA_PROPS_SIZE, LZMA_FINISH_END, &status, &alloc);

	if (ret != SZ_OK) {
		ERROR("LzmaDecode ERROR: %d (status=%d)\n", ret, status);
		return -EFAULT;
	}

	*dst_size = uncomp_size;
	INFO("LZMA9/0x%lx\n", *dst_size);

	return 0;
}

void *LZ4_malloc(size_t s)
{
	return SzAlloc(NULL, s);
}
void *LZ4_calloc(size_t n, size_t s)
{
	void *p = LZ4_malloc(n * s);

	if (p)
		memset(p, 0, n * s);

	return p;
}

void LZ4_free(void *p)
{
}

int decompress_lz4(void *dst, size_t *dst_size, const void *src, size_t src_size)
{
	LZ4F_dctx *dctx;
	size_t ret;
	size_t dctxStatus;

	SzInit();

	INFO("LZ4/%p/%p.\n", dst, src);

	dctxStatus = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);

	if (LZ4F_isError(dctxStatus)) {
		ERROR("dctxStatus=%lu\n", dctxStatus);
		return -1;
	}

	ret = LZ4F_decompress(dctx, dst, dst_size, src, &src_size, NULL);
	INFO("dst_size=%lu src_size=%lu ret=%zd\n", *dst_size, src_size, ret);
	if (LZ4F_isError(dctxStatus)) {
		ERROR("LZ4F_decompress=%lu\n", ret);
		return -1;
	}

	// No need to free.
	// LZ4F_freeDecompressionContext(dctx);

	return 0;
}

int decompress(void *dst, size_t *dst_size, const void *src, size_t src_size, enum COMPRESS_TYPE type)
{
	int ret = -EINVAL;

	NOTICE("DCP/%p/0x%lx/%p/0x%lx/%d.\n", dst, *dst_size, src, src_size, type);

	switch (type) {
	case COMP_LZ4:
		ret = decompress_lz4(dst, dst_size, src, src_size);
		break;
	case COMP_LZMA:
		ret = decompress_lzma(dst, dst_size, src, src_size);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	NOTICE("DCP/0x%lx/%d.\n", *dst_size, ret);

	return ret;
}