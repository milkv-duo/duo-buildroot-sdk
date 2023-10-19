#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

void uart_init(void);
int uart_getc(void);
int uart_tstc(void);
void uart_putc(uint8_t ch);
void uart_puts(char *str);

#endif
