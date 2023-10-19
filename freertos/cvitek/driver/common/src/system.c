#ifdef __riscv
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "top_reg.h"
#include "FreeRTOS.h"
#include "intr_conf.h"
#include "irq.h"
#include "csr.h"
#include "mmio.h"
#include "rtos_cmdqu.h"
#include "cvi_mailbox.h"
#include "memmap.h"
#else
//#include <stdint.h>
#include <string.h>
#include "pinmux.h"
#include "uart.h"
#include "i2c.h"
#include "top_reg.h"
#include "FreeRTOS.h"
#include "xscugic.h"
#include "intr_conf.h"
#include "linux/platform_device.h"
#include "linux/kernel.h"


struct platform_device *cvitek_devices[] __initdata = {
	/* add paltform device here  e.g. &xxx_platform_device, */
	
};

struct device_node *cvitek_device_node[] __initdata = {
	/* add device node here  e.g. &xxx_device_node, */
	
};

struct reserved_mem *cvitek_reserved_mem[] __initdata = {
	/* add reserved memory here  e.g. &xxx_reserved_mem,*/
	
};
#endif


void pre_system_init(void)
{
	pinmux_init();
	uart_init();
	irq_init();
	printf("Pre system init done\n");
}

void post_system_init(void)
{
#ifdef __riscv

#else
	/* init I2C */


	/* init platform device */
	

	/* init device node */
	

	/* add resources for each devices */
	

	/* init reserved memory */
	

	misc_init();
#endif
	printf("Post system init done\n");
}

//#define __DEBUG__

#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif


#ifdef __riscv

struct irq_chip {
	const char  *name;
	void (*irq_mask)(int irq_num);
	void (*irq_unmask)(int irq_num);
	int  (*irq_ack)(void);
	void (*irq_set_priority)(int irq_num, int priority);
	void (*irq_eoi)(int irq_num);
	void (*irq_set_threshold)(uint32_t threshold);
};

struct irq_action {
	irq_handler_t handler;
	unsigned int flags;
	unsigned int irqn;
	void *priv;
	char name[16];
};

#define IRQ_NUM_MAX   128
static struct irq_action g_irq_action[IRQ_NUM_MAX];
static struct irq_chip sirq_chip;

void disable_irq(unsigned int irqn)
{
	sirq_chip.irq_mask(irqn);
}

void enable_irq(unsigned int irqn)
{
	sirq_chip.irq_unmask(irqn);
}

static void plic_mask_irq( int irq_num)
{
	uint32_t mask = (irq_num);
	uint32_t value = 0;

	if(irq_num < 16) {
		printf("mask irq_num is %d\n", irq_num);
		return;
	}
	value = mmio_read_32(PLIC_ENABLE1 + 4 * (mask / 32));
	value &= ~(0x1 << (mask % 32));
	mmio_write_32((PLIC_ENABLE1 + (mask / 32) * 4), value);

}
static void plic_unmask_irq( int irq_num)
{
	uint32_t mask = (irq_num);
	uint32_t value = 0;

	if(irq_num < 16) {
		printf("unmask irq_num is %d\n",irq_num);
		return;
	}
	//printf("irq_num unmask=%d\n",mask);
	value = mmio_read_32(PLIC_ENABLE1 + 4 * (mask / 32));
	value |= (0x1 << (mask % 32));
	//printf("value=%x\n",value);
	mmio_write_32((PLIC_ENABLE1 + (mask / 32) * 4), value);
}
static int plic_ack_irq()
{
	return mmio_read_32(PLIC_CLAIM);
}
static void plic_eoi_irq(int irq_num)
{
//	printf("plic_eoi_irq %d irq=%d mip = %x\n", __LINE__, irq_num, read_csr(mip));
	mmio_write_32(PLIC_CLAIM, irq_num);
}

static void plic_set_priority_irq(int irq_num, int priority)
{
	//printf("plic_set_priority_irq addr(%x)=%d\n",PLIC_PRIORITY0 + irq_num*4,priority);
	mmio_write_32((PLIC_PRIORITY0 + irq_num*4), priority);
}

static void plic_set_threshold(uint32_t threshold)
{
	mmio_write_32((PLIC_THRESHOLD), threshold);
}

static struct irq_chip sirq_chip = {
	.name           = "RISCV PLIC",
	.irq_mask       = plic_mask_irq,
	.irq_unmask     = plic_unmask_irq,
	.irq_ack        = plic_ack_irq,
	.irq_set_priority = plic_set_priority_irq,
	.irq_eoi        = plic_eoi_irq,
	.irq_set_threshold = plic_set_threshold,
};

void irq_init(void)
{
	int i;
	// clear interrupt enable
	write_csr(mie, 0);
	// clear interrupt pending
	write_csr(mip, 0);

	// Clean the setting of all IRQ
	for (i = 0; i < IRQ_NUM_MAX / 4; i = i + 4) {
		mmio_write_32(((uintptr_t) PLIC_PRIORITY0 + i), 0);
	}

	for (i = 0; i <= IRQ_NUM_MAX / 32; i++) {
		mmio_write_32((PLIC_PENDING1  + i * 4), 0);
		mmio_write_32((PLIC_ENABLE1  + i * 4), 0);
	}

	memset(g_irq_action, 0, sizeof(struct irq_action) * IRQ_NUM_MAX);
	sirq_chip.irq_set_threshold(0);

    set_csr(mstatus, MSTATUS_MIE);

	// MIP_MTIE & MIP_MTIE will be enabled in port.c
    //set_csr(mie, MIP_MEIE);
    //set_csr(mie, MIP_MTIE);
}

int request_irq(int irqn, irq_handler_t handler, unsigned long flags,
        const char *name, void *priv)
{
	//printf("request_irq irqn=%d\n handler=%lx  name = %s\n",irqn,(long) handler,name);
	g_irq_action[irqn].handler = handler;
	if (name) {
		memcpy(g_irq_action[irqn].name, name, sizeof(g_irq_action[irqn].name));
		g_irq_action[irqn].name[sizeof(g_irq_action[irqn].name) - 1] = 0;
	}
	g_irq_action[irqn].irqn = irqn ; //- gic_data.hwirq_base;
	g_irq_action[irqn].flags = flags;
	g_irq_action[irqn].priv = priv;
	// set highest priority
	sirq_chip.irq_set_priority( irqn, 7);
	// unmask irq
	sirq_chip.irq_unmask( irqn);

	return 0;
}

void do_irq(void)
{
	int irqn;

	do{
		irqn = sirq_chip.irq_ack();
		if(g_irq_action[irqn].handler && irqn) {
			//printf("do_irq irqn=%d\n",irqn);
			g_irq_action[irqn].handler(g_irq_action[irqn].irqn, g_irq_action[irqn].priv);
		} else if(irqn)
			printf("g_irq_action[%i] NULL",irqn);
		else //plic_claim =0
			break;
		// clear plic pending
		sirq_chip.irq_eoi(irqn);
	}while(1);
	// clear external interrupt pending
	clear_csr(mip, MIP_MEIE);
}

#else
extern XScuGic xInterruptController;
#define GIC_SPI 0
#define GIC_PPI 1

int request_irq(int irqn, Xil_ExceptionHandler handler,
		unsigned long flags, const char *name, void *priv)
{
	return request_irq_mode(irqn, handler, flags, name, priv, GIC_SPI);
}
int request_sgirq(int irqn, Xil_ExceptionHandler handler,
		  unsigned long flags, const char *name, void *priv)
{
	return request_irq_mode(irqn, handler, flags, name, priv, GIC_PPI);
}
int request_irq_mode(int irqn, Xil_ExceptionHandler handler,
		     unsigned long flags, const char *name, void *priv,
		     int mode)
{
	BaseType_t xStatus;

	if ((irqn < 0) || (irqn >= NUM_IRQ))
		return -1;

	if (mode == GIC_SPI)
		irqn += hwirq_base;

	/* Sets the interrupt priority and trigger type. */
	if (flags & (IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING))
		XScuGic_SetPriorityTriggerType(
			&xInterruptController, irqn,
			portLOWEST_USABLE_INTERRUPT_PRIORITY
				<< portPRIORITY_SHIFT,
			IRQ_EDGE);
	else
		XScuGic_SetPriorityTriggerType(
			&xInterruptController, irqn,
			portLOWEST_USABLE_INTERRUPT_PRIORITY
				<< portPRIORITY_SHIFT,
			IRQ_LEVEL);

	/* Connect to the interrupt handler. */
	xStatus = XScuGic_Connect(&xInterruptController, irqn,
				  (Xil_ExceptionHandler)handler, priv);
	configASSERT(xStatus == XST_SUCCESS);

	/* Enable the interrupt in the GICD. */
	XScuGic_Enable(&xInterruptController, irqn);
	return 0;
}
#endif
