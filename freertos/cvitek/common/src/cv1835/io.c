#include <io.h>

/*
 * Copy data from IO memory space to "real" memory space.
 * from linux arch/arm64/kernel/io.c
 */
void memcpy_fromio(void *to, const volatile void __iomem *from, size_t count)
{
	while (count && (!IS_ALIGNED((unsigned long)from, 8) ||
			 !IS_ALIGNED((unsigned long)to, 8))) {
		*(u8 *)to = __raw_readb(from);
		from++;
		to++;
		count--;
	}

	while (count >= 8) {
		*(u64 *)to = __raw_readq(from);
		from += 8;
		to += 8;
		count -= 8;
	}

	while (count) {
		*(u8 *)to = __raw_readb(from);
		from++;
		to++;
		count--;
	}
}
