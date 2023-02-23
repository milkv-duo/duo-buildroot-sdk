#ifndef __DUMP_UART_H__
#define __DUMP_UART_H__

#define DUMP_PRINT_DEFAULT_SIZE 0x1000

/* this structure should be modified both freertos & osdrv side */
struct dump_uart_s {
	uint64_t dump_uart_ptr;
	unsigned int  dump_uart_max_size;
	unsigned int  dump_uart_pos;
	unsigned char dump_uart_enable;
	unsigned char dump_uart_overflow;
} __attribute__((packed));

#ifndef __linux__
/* used for freertos */
struct dump_uart_s *dump_uart_init(void);
struct dump_uart_s *dump_uart_msg(void);
void dump_uart_enable(void);
void dump_uart_disable(void);
#endif
#endif // end of __DUMP_UART_H__

