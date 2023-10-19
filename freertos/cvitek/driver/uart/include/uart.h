#ifndef _UART_H_
#define _UART_H_
#ifdef __riscv
#include <stdint.h>
#else
#include "linux/types.h"
#endif
void uart_init(void);
int uart_getc(void);
int uart_tstc(void);
uint8_t uart_putc(uint8_t ch);
void uart_puts(char *str);
int uart_put_buff(char *buf);
#endif
