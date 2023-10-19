#include <stdint.h>
#include "hal_uart_dw.h"
#include "top_reg.h"

static struct dw_regs *uart = 0;

void hal_uart_init(device_uart dev_uart, int baudrate, int uart_clock)
{
	int divisor = uart_clock / (16 * baudrate);
	switch (dev_uart) {
		case UART0:
			uart = (struct dw_regs *)UART0_BASE;
			break;
		case UART1:
			uart = (struct dw_regs *)UART1_BASE;
			break;
		case UART2:
			uart = (struct dw_regs *)UART2_BASE;
			break;
		case UART3:
			uart = (struct dw_regs *)UART3_BASE;
			break;
		default:
			break;
	}
	uart->lcr = uart->lcr | UART_LCR_DLAB | UART_LCR_8N1;
	uart->dll = divisor & 0xff;
	uart->dlm = (divisor >> 8) & 0xff;
	uart->lcr = uart->lcr & (~UART_LCR_DLAB);

	uart->ier = 0;
	uart->mcr = UART_MCRVAL;
	uart->fcr = UART_FCR_DEFVAL;

	uart->lcr = 3;
}

void hal_uart_putc(uint8_t ch)
{
	while (!(uart->lsr & UART_LSR_THRE))
		;
	uart->rbr = ch;
}

int hal_uart_getc(void)
{
	while (!(uart->lsr & UART_LSR_DR))
		return (int)uart->rbr;
}

int hal_uart_tstc(void)
{
	return (!!(uart->lsr & UART_LSR_DR));
}
