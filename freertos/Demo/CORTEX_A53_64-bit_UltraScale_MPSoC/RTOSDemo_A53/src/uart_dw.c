#include <stdint.h>


#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

struct dw_regs {
	volatile uint32_t	rbr;		/* 0x00 Data register */
	volatile uint32_t	ier;		/* 0x04 Interrupt Enable Register */
	volatile uint32_t	fcr;		/* 0x08 FIFO Control Register */
	volatile uint32_t	lcr;		/* 0x0C Line control register */
	volatile uint32_t	mcr;		/* 0x10 Line control register */
	volatile uint32_t	lsr;		/* 0x14 Line Status Register */
	volatile uint32_t	msr;		/* 0x18 Modem Status Register */
	volatile uint32_t	spr;		/* 0x20 Scratch Register */
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

#define UART_LSR_THRE		0x20 /* Transmit-hold-register empty */
#define UART_LSR_DR		0x01 /* Receiver data ready */
#define UART_LSR_TEMT   0x40        /* Xmitter empty */

#define UART_FCR_FIFO_EN    0x01 /* Fifo enable */
#define UART_FCR_RXSR       0x02 /* Receiver soft reset */
#define UART_FCR_TXSR       0x04 /* Transmitter soft reset */

#define UART_MCRVAL (UART_MCR_DTR | UART_MCR_RTS)      /* RTS/DTR */
#define UART_FCR_DEFVAL	(UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR)
#define UART_LCR_8N1    0x03

static struct dw_regs *uart = (struct dw_regs *)0x04140000;

void uart_init(void)
{
	int baudrate = 115200;
	int uart_clock = 25 * 1000 * 1000;

	int divisor = uart_clock / (16 * baudrate);

	uart->lcr = uart->lcr | UART_LCR_DLAB | UART_LCR_8N1;
	uart->dll = divisor & 0xff;
	uart->dlm = (divisor >> 8) & 0xff;
	uart->lcr = uart->lcr & (~UART_LCR_DLAB);

	uart->ier = 0;
	uart->mcr = UART_MCRVAL;
	uart->fcr = UART_FCR_DEFVAL;

	uart->lcr = 3;
}

void _uart_putc(uint8_t ch)
{
	while (!(uart->lsr & UART_LSR_THRE))
		;
	uart->rbr= ch;
}

void uart_putc(uint8_t ch)
{
	if (ch == '\n') {
		_uart_putc('\r');
	}
	_uart_putc(ch);
}

void uart_puts(char *str)
{
	if (!str)
		return;

	while (*str) {
		uart_putc(*str++);
	}
}

int uart_getc(void)
{
	while (!(uart->lsr & UART_LSR_DR))
		;
	return (int)uart->rbr;
}

int uart_tstc(void)
{
	return (!!(uart->lsr & UART_LSR_DR));
}
