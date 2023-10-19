#include <stdio.h>

#undef putchar

int puts(const char *s)
{
	int count = 0;
	while (*s) {
		if (putchar(*s++) != EOF) {
			count++;
		} else {
			count = EOF;
			break;
		}
	}

	/* According to the puts(3) manpage, the function should write a
     * trailing newline.
     */
	if ((count != EOF) && (putchar('\n') != EOF))
		count++;
	else
		count = EOF;

	return count;
}
