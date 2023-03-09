#include <linux/types.h>
#include <cpu_func.h>

#define L1_CACHE_BYTES     64
#define ALIGN(x, a)              (((x) + (a) - 1) & ~((a) - 1))

/*
 * dcache.ipa rs1 (invalidate)
 * | 31 - 25 | 24 - 20 | 19 - 15 | 14 - 12 | 11 - 7 | 6 - 0 |
 *   0000001    01010      rs1       000      00000  0001011
 *
 * dcache.cpa rs1 (clean)
 * | 31 - 25 | 24 - 20 | 19 - 15 | 14 - 12 | 11 - 7 | 6 - 0 |
 *   0000001    01001      rs1       000      00000  0001011
 *
 * dcache.cipa rs1 (clean then invalidate)
 * | 31 - 25 | 24 - 20 | 19 - 15 | 14 - 12 | 11 - 7 | 6 - 0 |
 *   0000001    01011      rs1       000      00000  0001011
 *
 * sync.s
 * | 31 - 25 | 24 - 20 | 19 - 15 | 14 - 12 | 11 - 7 | 6 - 0 |
 *   0000000    11001     00000      000      00000  0001011
 */
#define DCACHE_IPA_A0	".long 0x02a5000b"
#define DCACHE_CPA_A0	".long 0x0295000b"
#define DCACHE_CIPA_A0	".long 0x02b5000b"

#define SYNC_S		".long 0x0190000b"

#define CACHE_OP_RANGE(OP, start, size) \
	register unsigned long i asm("a0") = start & ~(L1_CACHE_BYTES - 1); \
	for (; i < ALIGN(start + size, L1_CACHE_BYTES); i += L1_CACHE_BYTES) \
		__asm__ __volatile__(OP); \
	 __asm__ __volatile__(SYNC_S)

void invalidate_dcache_range(unsigned long start, unsigned long end)
{
	CACHE_OP_RANGE(DCACHE_IPA_A0, start, end - start);
}

void flush_dcache_range(unsigned long start, unsigned long end)
{
	CACHE_OP_RANGE(DCACHE_CIPA_A0, start, end - start);
}

/*
 * Once uboot execute csrsi mhcr 0x2.
 * Will enter trap mode.
 * Comment the implement to avoid problem first
 */
#if CV181X_UBOOT_USE_DCACHE_API || ATHENA2_UBOOT_USE_DCACHE_API
void dcache_enable(void)
{
	asm volatile(
		"csrs mhcr, %0;" ::"rI"(0x2)
	);
}

void dcache_disable(void)
{
	flush_dcache_range(0x80000000, 0x80000000 + CONFIG_SYS_BOOTMAPSZ);
	asm volatile(
		"csrc mhcr, %0;" ::"rI"(0x2)
	);
}
#endif