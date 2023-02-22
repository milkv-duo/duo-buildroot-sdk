#include <stdint.h>
#include <ctype.h>
#include <string.h>

#define MAXNBUF (sizeof(intmax_t) * 8 + 1)

char const hex2ascii_data[] = "0123456789abcdefghijklmnopqrstuvwxyz";
#define hex2ascii(hex) (hex2ascii_data[hex])

char *ntostr(char *nbuf, uintmax_t num, int _base, int upper)
{
	typeof(num) base = (typeof(base))_base;
	char *p, *q, c;

	p = q = nbuf;
	*p = '\0';

	do {
		c = hex2ascii(num % base);
		*++p = upper ? (char)toupper(c) : c;
	} while (num /= base);

	while (q < p) {
		c = *q;
		*q++ = *p;
		*p-- = c;
	}

	return p;
}

void bytes_reverse(void *buf, size_t n)
{
	uint8_t t, *a = buf;
	int i, j;

	for (i = 0, j = n - 1; i < j; i++, j--) {
		t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
}
