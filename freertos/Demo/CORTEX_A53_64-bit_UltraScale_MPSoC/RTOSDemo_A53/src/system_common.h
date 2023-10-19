#ifndef _SYSTEM_COMMON_H_
#define _SYSTEM_COMMON_H_

#include <stdio.h>

#include "xil_types.h"

#include "bm1882_common.h"

static inline u32 float_to_u32(float x)
{
  union {
    int ival;
    float fval;
  } v = { .fval = x };
  return v.ival;
}

static inline float u32_to_float(u32 x)
{
  union {
    int ival;
    float fval;
  } v = { .ival = x };
  return v.fval;
}

#define array_len(a)              (sizeof(a) / sizeof(a[0]))

#define ALIGNMENT(x, a)			  __ALIGNMENT_MASK((x), (typeof(x))(a)-1)
#define __ALIGNMENT_MASK(x, mask)	(((x)+(mask))&~(mask))
#define PTR_ALIGNMENT(p, a)		 ((typeof(p))ALIGNMENT((unsigned long)(p), (a)))
#define IS_ALIGNMENT(x, a)		(((x) & ((typeof(x))(a) - 1)) == 0)

#define __raw_readb(a)		  (*(volatile unsigned char *)(a))
#define __raw_readw(a)		  (*(volatile unsigned short *)(a))
#define __raw_readl(a)		  (*(volatile unsigned int *)(a))
#define __raw_readq(a)		  (*(volatile unsigned long long *)(a))

#define __raw_writeb(a,v)	   (*(volatile unsigned char *)(a) = (v))
#define __raw_writew(a,v)	   (*(volatile unsigned short *)(a) = (v))
#define __raw_writel(a,v)	   (*(volatile unsigned int *)(a) = (v))
#define __raw_writeq(a,v)	   (*(volatile unsigned long long *)(a) = (v))

#define readb(a)		__raw_readb(a)
#define readw(a)		__raw_readw(a)
#define readl(a)		__raw_readl(a)
#define readq(a)		__raw_readq(a)

#define writeb(a, v)		__raw_writeb(a,v)
#define writew(a, v)		__raw_writew(a,v)
#define writel(a, v)		__raw_writel(a,v)
#define writeq(a, v)		__raw_writeq(a,v)

#define cpu_write8(a, v)	writeb(a, v)
#define cpu_write16(a, v)	writew(a, v)
#define cpu_write32(a, v)	writel(a, v)

#define cpu_read8(a)		readb(a)
#define cpu_read16(a)		readw(a)
#define cpu_read32(a)		readl(a)

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#ifdef ENABLE_DEBUG
#define debug(fmt, args...)	printf(fmt, ##args)
#else
#define debug(...)
#endif

#ifdef ENABLE_PRINT
#define uartlog(fmt, args...)	printf(fmt, ##args)
#else
#define uartlog(...)
#endif

#if 0
extern u32 debug_level;
#define debug_out(flag, fmt, args...)           \
  do {                                          \
    if (flag <= debug_level)                    \
      printf(fmt, ##args);                      \
  } while (0)
#endif

static inline void opdelay(unsigned int times)
{
	while (times--)
		__asm__ volatile("nop");
}

#ifdef USE_BMTAP
#define call_atomic(nodechip_idx, atomic_func, p_command, eng_id)       \
  emit_task_descriptor(p_command, eng_id)
#endif

#define TOP_USB_PHY_CTRSTS_REG	(TOP_BASE + 0x48)
#define UPCR_EXTERNAL_VBUS_VALID_OFFSET		0

#define TOP_DDR_ADDR_MODE_REG	(TOP_BASE + 0x64)
#define DAMR_REG_USB_REMAP_ADDR_39_32_OFFSET	16
#define DAMR_REG_USB_REMAP_ADDR_39_32_MSK		(0xff)

#define DAMR_REG_VD_REMAP_ADDR_39_32_OFFSET  24
#define DAMR_REG_VD_REMAP_ADDR_39_32_MSK   (0xff)

#define SW_RESET  (TOP_BASE + 0x3000)
#define JPEG_RESET   4

#define TOP_USB_CTRSTS_REG		(TOP_BASE + 0x38)
#define UCR_MODE_STRAP_OFFSET	0
#define UCR_MODE_STRAP_NON		0x0
#define UCR_MODE_STRAP_HOST		0x2
#define UCR_MODE_STRAP_DEVICE	0x4
#define UCR_MODE_STRAP_MSK		(0x7)
#define UCR_PORT_OVER_CURRENT_ACTIVE_OFFSET		10
#define UCR_PORT_OVER_CURRENT_ACTIVE_MSK		1

#define PINMUX_UART0    0
#define PINMUX_UART1    1
#define PINMUX_UART2    2
#define PINMUX_UART3    3
#define PINMUX_UART3_2  4
#define PINMUX_I2C0     5
#define PINMUX_I2C1     6
#define PINMUX_I2C2     7
#define PINMUX_I2C3     8
#define PINMUX_I2C4     9
#define PINMUX_I2C4_2   10
#define PINMUX_SPI0     11
#define PINMUX_SPI1     12
#define PINMUX_SPI2     13
#define PINMUX_SPI2_2   14
#define PINMUX_SPI3     15
#define PINMUX_SPI3_2   16
#define PINMUX_I2S0     17
#define PINMUX_I2S1     18
#define PINMUX_I2S2     19
#define PINMUX_I2S3     20
#define PINMUX_USBID    21
#define PINMUX_SDIO0    22
#define PINMUX_SDIO1    23
#define PINMUX_ND       24
#define PINMUX_EMMC     25
#define PINMUX_SPI_NOR  26
#define PINMUX_SPI_NAND 27
#define PINMUX_CAM0     28
#define PINMUX_CAM1     29
#define PINMUX_PCM0     30
#define PINMUX_PCM1     31
#define PINMUX_CSI0     32
#define PINMUX_CSI1     33
#define PINMUX_CSI2     34
#define PINMUX_DSI      35
#define PINMUX_VI0      36
#define PINMUX_VO       37
#define PINMUX_PWM1     38

/* addr remap */
#define REG_TOP_ADDR_REMAP      0x0064
#define ADDR_REMAP_USB(a)       ((a&0xFF)<<16)

/* rst */
#define REG_TOP_SOFT_RST        0x3000
#define BIT_TOP_SOFT_RST_AP     (1 << 1)
#define BIT_TOP_SOFT_RST_USB    (1 << 11)
#define BIT_TOP_SOFT_RST_SDIO   (1 << 14)

/* irq */
#define IRQ_LEVEL   0
#define IRQ_EDGE    3

#define SDMA_INTR 0
#define I2S0_TX_EMP_INTR  1
#define I2S0_TX_OR_INTR   2
#define I2S0_RX_DA_INTR   3
#define I2S0_RX_OR_INTR   4
#define I2S1_TX_EMP_INTR  5
#define I2S1_TX_OR_INTR   6
#define I2S1_RX_DA_INTR   7
#define I2S1_RX_OR_INTR   8
#define UART_INTR_UART0 9
#define UART_DMA_TX_REQ_UART0 10
#define UART_DMA_RX_REQ_UART0 11
#define UART_INTR_UART1 12
#define UART_DMA_TX_REQ_UART1 13
#define UART_DMA_RX_REQ_UART1 14
#define UART_INTR_UART2 15
#define UART_DMA_TX_REQ_UART2 16
#define UART_DMA_RX_REQ_UART2 17
#define UART_INTR_UART3 18
#define UART_DMA_TX_REQ_UART3 19
#define UART_DMA_RX_REQ_UART3 20
#define RTC_ALARM_O 21
#define SD1_WAKEUP_INTR 22
#define SD1_INTR  23
#define ETH1_SBD_PERCH_RX_INTR_O_0  24
#define ETH1_SBD_PERCH_RX_INTR_O_1  25
#define ETH1_SBD_PERCH_RX_INTR_O_2  26
#define ETH1_SBD_PERCH_RX_INTR_O_3  27
#define ETH1_SBD_PERCH_RX_INTR_O_4  28
#define ETH1_SBD_PERCH_RX_INTR_O_5  29
#define ETH1_SBD_PERCH_RX_INTR_O_6  30
#define ETH1_SBD_PERCH_RX_INTR_O_7  31
#define ETH1_SBD_PERCH_TX_INTR_O_0  32
#define ETH1_SBD_PERCH_TX_INTR_O_1  33
#define ETH1_SBD_PERCH_TX_INTR_O_2  34
#define ETH1_SBD_PERCH_TX_INTR_O_3  35
#define ETH1_SBD_PERCH_TX_INTR_O_4  36
#define ETH1_SBD_PERCH_TX_INTR_O_5  37
#define ETH1_SBD_PERCH_TX_INTR_O_6  38
#define ETH1_SBD_PERCH_TX_INTR_O_7  39
#define ETH0_SBD_PERCH_RX_INTR_O_0  40
#define ETH0_SBD_PERCH_RX_INTR_O_1  41
#define ETH0_SBD_PERCH_RX_INTR_O_2  42
#define ETH0_SBD_PERCH_RX_INTR_O_3  43
#define ETH0_SBD_PERCH_RX_INTR_O_4  44
#define ETH0_SBD_PERCH_RX_INTR_O_5  45
#define ETH0_SBD_PERCH_RX_INTR_O_6  46
#define ETH0_SBD_PERCH_RX_INTR_O_7  47
#define ETH0_SBD_PERCH_TX_INTR_O_0  48
#define ETH0_SBD_PERCH_TX_INTR_O_1  49
#define ETH0_SBD_PERCH_TX_INTR_O_2  50
#define ETH0_SBD_PERCH_TX_INTR_O_3  51
#define ETH0_SBD_PERCH_TX_INTR_O_4  52
#define ETH0_SBD_PERCH_TX_INTR_O_5  53
#define ETH0_SBD_PERCH_TX_INTR_O_6  54
#define ETH0_SBD_PERCH_TX_INTR_O_7  55
#define ETH1_SBD_INTR_O 56
#define ETH0_SBD_INTR_O 57
#define SD0_WAKEUP_INTR 58
#define SD0_INTR  59
#define EMMC_WAKEUP_INTR  60
#define EMMC_INTR 61
// #define NA  62
#define DDR_PI_PHY_INTR 63
#define SF_SPI_INT  64
#define SPI_0_SSI_INTR  65
#define GPIO2_INTR_FLAG 66
#define GPIO1_INTR_FLAG 67
#define GPIO0_INTR_FLAG 68
#define WDT_INTR  69
#define IC4_INTR  70
#define IC3_INTR  71
#define IC2_INTR  72
#define IC1_INTR  73
#define IC0_INTR  74
#define JPEG_INTRPT_REQ 75
#define H264C_INTERRUPT 76
#define H265C_INTERRUPT 77
// #define NA  78
#define TPU_INTR  79
#define TDMA_INTERRUPT  80
// #define NA  81
#define USB_OTGIRQ  82
#define USB_IRQS_0  83
#define USB_IRQS_1  84
#define USB_INTERRUPT_REQ_0 85
#define USB_INTERRUPT_REQ_1 86
#define USB_INTERRUPT_REQ_2 87
#define USB_INTERRUPT_REQ_3 88
#define USB_INTERRUPT_REQ_4 89
#define USB_INTERRUPT_REQ_5 90
#define USB_INTERRUPT_REQ_6 91
#define USB_INTERRUPT_REQ_7 92
// #define NA  93
// #define NA  94
// #define NA  95
#define NPMUIRQ_0 96
#define NPMUIRQ_1 97
#define NPMUIRQ_2 98
#define NPMUIRQ_3 99
#define CTIIRQ_0  100
#define CTIIRQ_1  101
#define CTIIRQ_2  102
#define CTIIRQ_3  103
#define NEXTERRIRQ  104
// #define NA  105
// #define NA  106
// #define NA  107
#define GP_REG15_0  108
#define GP_REG15_1  109
#define GP_REG15_2  110
#define GP_REG15_3  111
#define GP_REG15_4  112
#define GP_REG15_5  113
#define GP_REG15_6  114
#define GP_REG15_7  115
#define GP_REG15_8  116
#define GP_REG15_9  117
#define GP_REG15_10 118
#define GP_REG15_11 119
#define GP_REG15_12 120
#define GP_REG15_13 121
#define GP_REG15_14 122
#define GP_REG15_15 123
#define SPI_1_SSI_INTR  124
#define SPI_2_SSI_INTR  125
#define SPI_3_SSI_INTR  126
#define I2S2_TX_EMP_INTR    127
#define I2S2_TX_OR_INTR     128
#define I2S2_RX_DA_INTR     129
#define I2S2_RX_OR_INTR     130
#define I2S3_TX_EMP_INTR    131
#define I2S3_TX_OR_INTR     132
#define I2S3_RX_DA_INTR     133
#define I2S3_RX_OR_INTR     134
// #define NA  135
// #define NA  136
// #define NA  137
#define VIP_INT_ISP_TOP 138
#define VIP_INT_SC_TOP_0  139
#define VIP_INT_SC_TOP_1  140
#define VIP_INT_SC_TOP_2  141
#define VIP_INT_SC_TOP_3  142
#define VIP_INT_SC_TOP_4  143
#define VIP_INT_SC_TOP_5  144
#define VIP_INT_SC_TOP_6  145
#define VIP_INT_SC_TOP_7  146
#define VIP_INT_SC_TOP_8  147
#define VIP_INT_SC_TOP_9  148
#define VIP_INT_SC_TOP_10 149
#define VIP_INT_SC_TOP_11 150
#define VIP_INT_SC_TOP_12 151
#define VIP_INT_SC_TOP_13   152
#define VIP_INT_SC_TOP_14   153
#define VIP_INT_SC_TOP_15   154
#define VIP_INT_CSI_MAC0    155
#define VIP_INT_CSI_MAC1    156
#define VIP_INT_DWA_WRAP    157
// #define NA  158
#define PERI_FIREWALL_IRQ 159
#define HSPERI_FIREWALL_IRQ 160
#define SPACC_IRQ 161
#define TRNG_IRQ  162
// #define NA  163
#define TEMPSEN_IRQ_O 164
#define INTR_SARADC 165
// #define NA  166
// #define NA  167
// #define NA  168
#define DDR_FW_INTR  169
#define ROM_FIREWALL_IRQ 170
#define SRAM_FIREWALL_IRQ  171
#define DDR_AXI_MON_INTR  172
// #define NA  173
// #define NA  174
// #define NA  175
// #define NA  176
// #define NA  177
// #define NA  178
// #define NA  179
#define TIMER_INTR_FLAG  180
#define SPI_NAND_INTR  181
#define DBGSYS_APBUSMON_HANG_INT  182

#define IRQF_TRIGGER_NONE    0x00000000
#define IRQF_TRIGGER_RISING  0x00000001
#define IRQF_TRIGGER_FALLING 0x00000002
#define IRQF_TRIGGER_HIGH    0x00000004
#define IRQF_TRIGGER_LOW     0x00000008
#define IRQF_TRIGGER_MASK   (IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
		                 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)

typedef int (*irq_handler_t)(int irqn, void *priv);

extern int request_irq(unsigned int irqn, irq_handler_t handler, unsigned long flags,
		const char *name, void *priv);

void disable_irq(unsigned int irqn);
void enable_irq(unsigned int irqn);

void cpu_enable_irqs(void);
void cpu_disable_irqs(void);

extern void irq_trigger(int irqn);
extern void irq_clear(int irqn);
extern int irq_get_nums(void);
void pinmux_config(int io_type);

/*FIXME
 * NUM_IRQ may be different accross
 * different platform, we should review it when porting
 */
#define NUM_IRQ (256)

#define SPI_SECTOR_SIZE 256
#define SPI_DMMR_TEST_SIZE 1024

#endif
