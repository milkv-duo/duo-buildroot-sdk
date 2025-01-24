#include <stdint.h>
#include "platform_def.h"

#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

struct dw_regs {
	uint32_t	rbr;		/* 0x00 Data register */
	uint32_t	ier;		/* 0x04 Interrupt Enable Register */
	uint32_t	fcr;		/* 0x08 FIFO Control Register */
	uint32_t	lcr;		/* 0x0C Line control register */
	uint32_t	mcr;		/* 0x10 Line control register */
	uint32_t	lsr;		/* 0x14 Line Status Register */
	uint32_t	msr;		/* 0x18 Modem Status Register */
	uint32_t	spr;		/* 0x20 Scratch Register */
};

#define UART_LCR_WLS_MSK 0x03       /* character length select mask */
#define UART_LCR_WLS_5  0x00        /* 5 bit character length */
#define UART_LCR_WLS_6  0x01        /* 6 bit character length */
#define UART_LCR_WLS_7  0x02        /* 7 bit character length */
#define UART_LCR_WLS_8  0x03        /* 8 bit character length */
#define UART_LCR_STB    0x04        /* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN    0x08        /* Parity eneble */
#define UART_LCR_EPS    0x10        /* Even Parity Select */
#define UART_LCR_STKP   0x20        /* Stick Parity */
#define UART_LCR_SBRK   0x40        /* Set Break */
#define UART_LCR_BKSE   0x80        /* Bank select enable */
#define UART_LCR_DLAB   0x80        /* Divisor latch access bit */

#define UART_MCR_DTR    0x01        /* DTR   */
#define UART_MCR_RTS    0x02        /* RTS   */

#define UART_LSR_THRE   0x20 /* Transmit-hold-register empty */
#define UART_LSR_DR	    0x01 /* Receiver data ready */
#define UART_LSR_TEMT   0x40        /* Xmitter empty */

#define UART_FCR_FIFO_EN    0x01 /* Fifo enable */
#define UART_FCR_RXSR       0x02 /* Receiver soft reset */
#define UART_FCR_TXSR       0x04 /* Transmitter soft reset */

#define UART_MCRVAL (UART_MCR_DTR | UART_MCR_RTS)      /* RTS/DTR */
#define UART_FCR_DEFVAL	(UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR)
#define UART_LCR_8N1    0x03

static struct dw_regs *uart = (struct dw_regs *)PLAT_BOOT_UART_BASE;

void console_init(uintptr_t not_used, unsigned int uart_clk, unsigned int baud_rate)
{
	int baudrate = baud_rate;
	int uart_clock = uart_clk;

	int divisor = uart_clock / (16 * baudrate);

	uart->lcr = uart->lcr | UART_LCR_DLAB | UART_LCR_8N1;
	asm (""::: "memory");
	uart->dll = divisor & 0xff;
	asm (""::: "memory");
	uart->dlm = (divisor >> 8) & 0xff;
	asm (""::: "memory");
	uart->lcr = uart->lcr & (~UART_LCR_DLAB);
	asm (""::: "memory");
	uart->ier = 0;
	asm (""::: "memory");
	uart->mcr = UART_MCRVAL;
	asm (""::: "memory");
	uart->fcr = UART_FCR_DEFVAL;
	asm (""::: "memory");
	uart->lcr = 3;
}

void _uart_putc(uint8_t ch)
{
	do {
		asm (""::: "memory");
	} while (!(uart->lsr & UART_LSR_THRE))
		;
	uart->rbr = ch;
}

void console_putc(uint8_t ch)
{
	if (ch == '\n') {
		_uart_putc('\r');
	}
	_uart_putc(ch);
}

void console_puts(char *str)
{
	if (!str)
		return;

	while (*str) {
		console_putc(*str++);
	}
}
void console_flush(void)
{
	do {
		asm ("" ::: "memory");
	} while ((uart->lsr & (UART_LSR_THRE | UART_LSR_TEMT)) != (UART_LSR_THRE | UART_LSR_TEMT))
		;
}
int console_getc(void)
{
	do {
		asm (""::: "memory");
	} while (!(uart->lsr & UART_LSR_DR))
		;
	return (int)uart->rbr;
}

int uart_tstc(void)
{
	return (!!(uart->lsr & UART_LSR_DR));
}

int console_tstc(void)
{
	return uart->lsr & UART_LSR_DR;
}
