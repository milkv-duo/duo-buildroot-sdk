#ifndef _CV180X_INTERRUPT_CONFIG_H_
#define _CV180X_INTERRUPT_CONFIG_H_

/* irq */
#define IRQ_LEVEL 0
#define IRQ_EDGE 3

#define NA 0xFFFF

#define TEMPSEN_IRQ_O NA
#define RTC_ALARM_O NA
#define RTC_PWR_BUTTON1_LONGPRESS_O NA
#define VBAT_DEB_IRQ_O NA
#define JPEG_INTERRUPT 16
#define H264_INTERRUPT 17
#define H265_INTERRUPT 18
#define VC_SBM_INT 19
#define ISP_INT 20
#define SC_INTR_0 21
#define VIP_INT_CSI_MAC0 22
#define VIP_INT_CSI_MAC1 23
#define LDC_INT 24
#define SDMA_INTR_CPU0 NA
#define SDMA_INTR_CPU1 NA
#define SDMA_INTR_CPU2 25
#define USB_IRQS NA
#define ETH0_SBD_INTR_O NA
#define ETH0_LPI_INTR_O NA
#define EMMC_WAKEUP_INTR NA
#define EMMC_INTR NA
#define SD0_WAKEUP_INTR NA
#define SD0_INTR NA
#define SD1_WAKEUP_INTR NA
#define SD1_INTR NA
#define SPI_NAND_INTR NA
#define I2S0_INT 26
#define I2S1_INT 27
#define I2S2_INT 28
#define I2S3_INT 29
#define UART0_INTR 30
#define UART1_INTR 31
#define UART2_INTR NA
#define UART3_INTR NA
#define UART4_INTR NA
#define I2C0_INTR 32
#define I2C1_INTR 33
#define I2C2_INTR 34
#define I2C3_INTR 35
#define I2C4_INTR 36
#define SPI_0_SSI_INTR 37
#define SPI_1_SSI_INTR 38
#define SPI_2_SSI_INTR NA
#define SPI_3_SSI_INTR NA
#define WDT0_INTR NA
#define WDT1_INTR NA
#define WDT2_INTR 39
#define KEYSCAN_IRQ 40
#define GPIO0_INTR_FLAG 41
#define GPIO1_INTR_FLAG 42
#define GPIO2_INTR_FLAG 43
#define GPIO3_INTR_FLAG 44
#define WGN0_IRQ 45
#define WGN1_IRQ NA
#define WGN2_IRQ NA
#define MBOX_INT1 46
#define IRRX_INT 47
#define GPIO_INT 48
#define UART_INT 49
#define SPI_INT NA
#define I2C_INT 50
#define WDT_INT 51
#define TPU_INTR NA
#define TDMA_INTERRUPT 52
#define SW_INT_0_CPU0 NA
#define SW_INT_1_CPU0 NA
#define SW_INT_0_CPU1 NA
#define SW_INT_1_CPU1 NA
#define SW_INT_0_CPU2 53
#define SW_INT_1_CPU2 54
#define TIMER_INTR_0 NA
#define TIMER_INTR_1 NA
#define TIMER_INTR_2 NA
#define TIMER_INTR_3 NA
#define TIMER_INTR_4 55
#define TIMER_INTR_5 56
#define TIMER_INTR_6 57
#define TIMER_INTR_7 58
#define PERI_FIREWALL_IRQ NA
#define HSPERI_FIREWALL_IRQ NA
#define DDR_FW_INTR NA
#define ROM_FIREWALL_IRQ NA
#define SPACC_IRQ 59
#define TRNG_IRQ NA
#define AXI_MON_INTR NA
#define DDRC_PI_PHY_INTR NA
#define SF_SPI_INT NA
#define EPHY_INT_N_O NA
#define IVE_INT 60
#define DBGSYS_APBUSMON_HANG_INT NA
#define INTR_SARADC NA
#define MBOX_INT_CA53 NA
#define MBOX_INT_C906 NA
#define MBOX_INT_C906_2ND 61
#define NPMUIRQ_0 NA
#define CTIIRQ_0 NA
#define NEXTERRIRQ NA

#define IRQF_TRIGGER_NONE 0x00000000
#define IRQF_TRIGGER_RISING 0x00000001
#define IRQF_TRIGGER_FALLING 0x00000002
#define IRQF_TRIGGER_HIGH 0x00000004
#define IRQF_TRIGGER_LOW 0x00000008
#define IRQF_TRIGGER_MASK                                                      \
	(IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | IRQF_TRIGGER_RISING |          \
	 IRQF_TRIGGER_FALLING)

typedef int (*irq_handler_t)(int irqn, void *priv);

int request_irq(int irqn, irq_handler_t handler,
		       unsigned long flags, const char *name, void *priv);

/*
void disable_irq(unsigned int irqn);
void enable_irq(unsigned int irqn);

void cpu_enable_irqs(void);
void cpu_disable_irqs(void);

extern void irq_trigger(int irqn);
extern void irq_clear(int irqn);
extern int irq_get_nums(void);
void pinmux_config(int io_type);
*/
/*FIXME
 * NUM_IRQ may be different accross
 * different platform, we should review it when porting
 */
#define NUM_IRQ (62)

#endif //end of_CV180X_INTERRUPT_CONFIG_H_
