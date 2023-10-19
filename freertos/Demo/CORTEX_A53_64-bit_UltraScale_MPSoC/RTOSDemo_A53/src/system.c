#include <stdint.h>
#include <string.h>
#include "system_common.h"
#include "uart.h"
#include "mmio.h"
//#include "bm1880v2_reg_fmux_gpio.h"
//#include "bm1880v2_pinlist_swconfig.h"

void system_init(void)
{
	uart_init();
	//irq_init();
	xil_printf("system init done\n");
}

struct irq_action {
	irq_handler_t handler;
	unsigned long flags;
	unsigned int irqn;
	void *priv;
	char name[32];
};

static struct irq_action g_irq_action[32];

__weak void invalidate_dcache_range(unsigned long start, unsigned long size) {}
__weak void flush_dcache_range(unsigned long start, unsigned long size) {}
__weak void invalidate_icache_all(void) {}
__weak void invalidate_dcache_all(void) {}

/*
 * GIC driver
 */

#define REG_BASE_GICD	0x01F01000
#define REG_BASE_GICC	0x01F02000

#define GIC_CPU_CTRL            0x00
#define GIC_CPU_PRIMASK         0x04
#define GIC_CPU_BINPOINT        0x08
#define GIC_CPU_INTACK          0x0c
#define GIC_CPU_EOI             0x10
#define GIC_CPU_RUNNINGPRI      0x14
#define GIC_CPU_HIGHPRI         0x18

#define GIC_DIST_CTRL           0x000
#define GIC_DIST_CTR            0x004
#define GIC_DIST_IGROUP         0x080
#define GIC_DIST_ENABLE_SET     0x100
#define GIC_DIST_ENABLE_CLEAR       0x180
#define GIC_DIST_PENDING_SET        0x200
#define GIC_DIST_PENDING_CLEAR      0x280
#define GIC_DIST_ACTIVE_SET         0x300
#define GIC_DIST_ACTIVE_CLEAR       0x380
#define GIC_DIST_PRI            0x400
#define GIC_DIST_TARGET         0x800
#define GIC_DIST_CONFIG         0xc00
#define GIC_DIST_SOFTINT        0xf00

#define GICC_DIS_BYPASS_MASK	0x1e0

static inline uint32_t read_reg(uint64_t addr)
{
	return *((volatile uint32_t *)addr);
}

static inline void write_reg(uint64_t addr, uint32_t value)
{
	*((volatile uint32_t *)addr) = value;
}

struct gic_data {
	uint64_t dist_base;
	uint64_t cpu_base;
	int gic_irqs;
	int hwirq_base;
	void *chip;
};

struct irq_chip {
	const char  *name;
	void        (*irq_mask)(struct gic_data *data, int irq_num);
	void        (*irq_unmask)(struct gic_data *data, int irq_num);
	int         (*irq_ack)(struct gic_data *data);
	void        (*irq_set_priority)(struct gic_data *data, int irq_num, int priority);
	void        (*irq_eoi)(struct gic_data *data, int irq_num);
	void        (*irq_set_type)(struct gic_data *data, uint32_t flow_type);
};

static void gicd_init(void);
static void gicc_init(void);
static struct irq_chip gic_chip;
static struct gic_data gic_data = {
	.dist_base  = REG_BASE_GICD,
	.cpu_base   = REG_BASE_GICC,
	.chip       = &gic_chip
};

static inline uint32_t gic_data_dist_base(struct gic_data *gic)
{
	return gic->dist_base;
}

static inline uint32_t gic_data_cpu_base(struct gic_data *gic)
{
	return gic->cpu_base;
}

static void irq_trigger_mode(int irqn, int mode)
{
	uint32_t reg_addr, off_in_reg, data;

	reg_addr = gic_data_dist_base(&gic_data) + GIC_DIST_CONFIG + (irqn / 16) * 4;
	off_in_reg = (irqn % 16) * 2;

	data = read_reg(reg_addr);
	data &= ~(3 << off_in_reg);
	data |= mode << off_in_reg;
	write_reg(reg_addr, data);
}
void irq_trigger(int irqn)
{
	irqn += gic_data.hwirq_base;

	write_reg(gic_data_dist_base(&gic_data) + GIC_DIST_PENDING_SET + (irqn / 32) * 4, 1 << (irqn % 32));
}

void irq_clear(int irqn)
{
	irqn += gic_data.hwirq_base;

	write_reg(gic_data_dist_base(&gic_data) + GIC_DIST_PENDING_CLEAR + (irqn / 32) * 4, 1 << (irqn % 32));
}

int irq_get_nums(void)
{
	return gic_data.gic_irqs - gic_data.hwirq_base;
}

static void gic_mask_irq(struct gic_data *d, int irq_num)
{
	uint32_t mask = 1 << (irq_num % 32);

	write_reg(gic_data_dist_base(d) + GIC_DIST_ENABLE_CLEAR + (irq_num / 32) * 4, mask);
}

static void gic_unmask_irq(struct gic_data *d, int irq_num)
{
	uint32_t mask = 1 << (irq_num % 32);

	write_reg(gic_data_dist_base(d) + GIC_DIST_ENABLE_SET + (irq_num / 32) * 4, mask);
}

static void gic_eoi_irq(struct gic_data *d, int irq_num)
{
	write_reg(gic_data_cpu_base(d) + GIC_CPU_EOI, irq_num);
}

static int gic_ack_irq(struct gic_data *d)
{
	return read_reg(gic_data_cpu_base(d) + GIC_CPU_INTACK);
}

static void gic_set_priority_irq(struct gic_data *d, int irq_num, int priority)
{
	uint32_t reg;
	uint32_t spi_pri_base = gic_data_dist_base(d) + GIC_DIST_PRI;
	int bit;

	reg = read_reg(spi_pri_base + (irq_num / 4 * 4));
	bit = (irq_num % 4) * 8;
	reg &= ~(0xF0UL << bit);
	reg |= (0xF0UL & (priority << 4)) << bit;
	write_reg(spi_pri_base + (irq_num / 4 * 4), reg);
}

static void gic_set_type(struct gic_data *d, uint32_t type)
{
	return;
}

static struct irq_chip gic_chip = {
	.name           = "GIC",
	.irq_mask       = gic_mask_irq,
	.irq_unmask     = gic_unmask_irq,
	.irq_ack        = gic_ack_irq,
	.irq_set_priority = gic_set_priority_irq,
	.irq_eoi        = gic_eoi_irq,
	.irq_set_type   = gic_set_type,
};

static uint8_t gic_get_cpumask(struct gic_data *data)
{
	uint32_t base = gic_data_dist_base(&gic_data);
	uint32_t mask, i;

	for (i = mask = 0; i < 32; i += 4) {
		mask = read_reg(base + GIC_DIST_TARGET + i);
		mask |= mask >> 16;
		mask |= mask >> 8;
		if (mask)
			break;
	}
#ifndef CONFIG_CHIP_BM1882
	if (!mask)
		xil_printf("GIC CPU mask not found - will fail to boot.\n");
#endif

	return mask;
}

static void gicd_init(void)
{
	int hwirq_base = 32;
	int gic_irqs;
	uint32_t base = gic_data_dist_base(&gic_data);
	uint32_t cpumask;
	int i;

	gic_irqs = read_reg(base + GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if (gic_irqs > 1020)
		gic_irqs = 1020;
	gic_data.gic_irqs = gic_irqs;
	gic_data.hwirq_base = hwirq_base;

	write_reg(base + GIC_DIST_CTRL, 0);

	cpumask = gic_get_cpumask(&gic_data);
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;
	for (i = 32; i < gic_irqs; i += 4)
		write_reg(base + GIC_DIST_TARGET + i * 4 / 4, cpumask);

	/* Set all global interrupts to be level triggered, active low. */
	for (i = 32; i < gic_irqs; i += 16)
		write_reg(base + GIC_DIST_CONFIG + i / 4, 0);

	/* Set priority on all global interrupts. */
	for (i = 32; i < gic_irqs; i += 4)
		write_reg(base + GIC_DIST_PRI + i, 0xa0a0a0a0);

	/* Disable all global interrupts. */
	for (i = 32; i < gic_irqs; i += 32) {
		write_reg(base + GIC_DIST_ACTIVE_CLEAR + i / 8, 0xffffffff);
		write_reg(base + GIC_DIST_ENABLE_CLEAR + i / 8, 0xffffffff);
	}

	write_reg(base + GIC_DIST_CTRL, 1);
}

static void gicc_init(void)
{
	int i;
	uint32_t base, bypass;

	base = gic_data_dist_base(&gic_data);
	write_reg(base + GIC_DIST_ACTIVE_CLEAR, 0xffffffff);
	write_reg(base + GIC_DIST_ENABLE_CLEAR, 0xffff0000);
	write_reg(base + GIC_DIST_ENABLE_SET, 0x0000ffff);

	for (i = 0; i < 32; i += 4)
		write_reg(base + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	base = gic_data_cpu_base(&gic_data);
	write_reg(base + GIC_CPU_PRIMASK, 0xf0);

	bypass = read_reg(base + GIC_CPU_CTRL);
	bypass &= GICC_DIS_BYPASS_MASK;

	/* Enable CPU interrupt */
	write_reg(base + GIC_CPU_CTRL, bypass | 1);
}

static void cpu_route_irqs_el2(void)
{
	int get_arm_current_el(void);

	uint64_t el;
	uint64_t hcr_el2 = 0x0038; // set AMO, IMO, and FMO

	asm volatile(
		"mrs %0, currentel\n"
		"lsr %0, %0, #2\n"
		"and %0, %0, #3\n"
		: "=r" (el) : );

	if (el == 2) {
		asm volatile("mrs x0, hcr_el2\n"
			     "bic x0, x0, #0x08000000\n" // clear TGE
			     "orr x0, x0, %0\n"
			     "msr hcr_el2, x0\n"
			     :
			     : "r"(hcr_el2)
			     : "x0");
	}
}

void cpu_enable_irqs(void)
{
	cpu_route_irqs_el2();

//Need to fix it with SError Exception
//    asm volatile (    \
//        "msr	daifclr, #6	  "   \
//        :                         \
//        :                         \
//        : "memory");              \
//
}

void cpu_disable_irqs(void)
{
    asm volatile (    \
        "msr	daifset, #6	  "   \
        :                         \
        :                         \
        : "memory");              \

}

void irq_init(void)
{

	cpu_enable_irqs();
	gicd_init();
	gicc_init();
}

int request_irq(unsigned int irqn, irq_handler_t handler, unsigned long flags,
		const char *name, void *priv)
{
	if ((irqn < 0) || (irqn >= NUM_IRQ))
		return -1;

	irqn += gic_data.hwirq_base;

	if (flags & (IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING))
		irq_trigger_mode(irqn, IRQ_EDGE);
	else
		irq_trigger_mode(irqn, IRQ_LEVEL);

	g_irq_action[irqn].handler = handler;
	if (name) {
		memcpy(g_irq_action[irqn].name, name, sizeof(g_irq_action[irqn].name));
		g_irq_action[irqn].name[sizeof(g_irq_action[irqn].name) - 1] = 0;
	}
	g_irq_action[irqn].irqn = irqn - gic_data.hwirq_base;
	g_irq_action[irqn].flags = flags;
	g_irq_action[irqn].priv = priv;

	gic_chip.irq_set_priority(&gic_data, irqn, 10);

	gic_chip.irq_unmask(&gic_data, irqn);

	return 0;
}

void  printf_once(char *string, int irq)
{
        static uint8_t __print_once = FALSE;
        static int id = 0;

        if (!__print_once || (id != irq)) {
                __print_once = TRUE;
		id =(int) irq;
                xil_printf("%s %d\n",string, irq);
        }
}

void do_irq(void)
{
	int irqn;
	irqn = gic_chip.irq_ack(&gic_data);
	printf_once("do_irq", irqn - 32);
	g_irq_action[irqn].handler(g_irq_action[irqn].irqn, g_irq_action[irqn].priv);
	gic_chip.irq_eoi(&gic_data, irqn);

}

void disable_irq(unsigned int irqn)
{

	irqn += gic_data.hwirq_base;
	gic_chip.irq_mask(&gic_data, irqn);
}

void enable_irq(unsigned int irqn)
{
	irqn += gic_data.hwirq_base;
	gic_chip.irq_unmask(&gic_data, irqn);
}
