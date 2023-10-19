
#include <stdio.h>
//#include "uart.h"

#undef putchar

int putchar(int c)
{
	int res;
	if (uart_putc((unsigned char)c) >= 0)
		res = c;
	else
		res = EOF;

	return res;
}
