#ifndef _SYSTEM_COMMON_H_
#define _SYSTEM_COMMON_H_

//#include <stdio.h>

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

#define ALIGNMENT(x, a) __ALIGNMENT_MASK((x), (typeof(x))(a) - 1)
#define __ALIGNMENT_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define PTR_ALIGNMENT(p, a) ((typeof(p))ALIGNMENT((unsigned long)(p), (a)))
#define IS_ALIGNMENT(x, a) (((x) & ((typeof(x))(a) - 1)) == 0)

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#ifdef USE_BMTAP
#define call_atomic(nodechip_idx, atomic_func, p_command, eng_id)              \
	emit_task_descriptor(p_command, eng_id)
#endif

#define TOP_USB_PHY_CTRSTS_REG (TOP_BASE + 0x48)
#define UPCR_EXTERNAL_VBUS_VALID_OFFSET 0

#define TOP_DDR_ADDR_MODE_REG (TOP_BASE + 0x64)
#define DAMR_REG_USB_REMAP_ADDR_39_32_OFFSET 16
#define DAMR_REG_USB_REMAP_ADDR_39_32_MSK (0xff)

#define DAMR_REG_VD_REMAP_ADDR_39_32_OFFSET 24
#define DAMR_REG_VD_REMAP_ADDR_39_32_MSK (0xff)

#define SW_RESET (TOP_BASE + 0x3000)
#define JPEG_RESET 4

#define TOP_USB_CTRSTS_REG (TOP_BASE + 0x38)
#define UCR_MODE_STRAP_OFFSET 0
#define UCR_MODE_STRAP_NON 0x0
#define UCR_MODE_STRAP_HOST 0x2
#define UCR_MODE_STRAP_DEVICE 0x4
#define UCR_MODE_STRAP_MSK (0x7)
#define UCR_PORT_OVER_CURRENT_ACTIVE_OFFSET 10
#define UCR_PORT_OVER_CURRENT_ACTIVE_MSK 1

#define PINMUX_UART0 0
#define PINMUX_UART1 1
#define PINMUX_UART2 2
#define PINMUX_UART3 3
#define PINMUX_UART3_2 4
#define PINMUX_I2C0 5
#define PINMUX_I2C1 6
#define PINMUX_I2C2 7
#define PINMUX_I2C3 8
#define PINMUX_I2C4 9
#define PINMUX_I2C4_2 10
#define PINMUX_SPI0 11
#define PINMUX_SPI1 12
#define PINMUX_SPI2 13
#define PINMUX_SPI2_2 14
#define PINMUX_SPI3 15
#define PINMUX_SPI3_2 16
#define PINMUX_I2S0 17
#define PINMUX_I2S1 18
#define PINMUX_I2S2 19
#define PINMUX_I2S3 20
#define PINMUX_USBID 21
#define PINMUX_SDIO0 22
#define PINMUX_SDIO1 23
#define PINMUX_ND 24
#define PINMUX_EMMC 25
#define PINMUX_SPI_NOR 26
#define PINMUX_SPI_NAND 27
#define PINMUX_CAM0 28
#define PINMUX_CAM1 29
#define PINMUX_PCM0 30
#define PINMUX_PCM1 31
#define PINMUX_CSI0 32
#define PINMUX_CSI1 33
#define PINMUX_CSI2 34
#define PINMUX_DSI 35
#define PINMUX_VI0 36
#define PINMUX_VO 37
#define PINMUX_PWM1 38

/* addr remap */
#define REG_TOP_ADDR_REMAP 0x0064
#define ADDR_REMAP_USB(a) (((a) & 0xFF) << 16)

/* rst */
#define BITS_PER_REG 32
#define REG_TOP_SOFT_RST 0x3000

/* irq */
#define IRQ_LEVEL 0
#define IRQ_EDGE 3

#define SDMA_INTR 0
#define I2S0_TX_EMP_INTR 1
#define I2S0_TX_OR_INTR 2
#define I2S0_RX_DA_INTR 3
#define I2S0_RX_OR_INTR 4
#define I2S1_TX_EMP_INTR 5
#define I2S1_TX_OR_INTR 6
#define I2S1_RX_DA_INTR 7
#define I2S1_RX_OR_INTR 8
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
// #defiNE RESERVED 22
// #defiNE RESERVED 23
// #defiNE RESERVED 24
// #defiNE RESERVED 25
// #defiNE RESERVED 26
// #defiNE RESERVED 27
// #defiNE RESERVED 28
// #defiNE RESERVED 29
// #defiNE RESERVED 30
// #defiNE RESERVED 31
// #defiNE RESERVED 32
// #defiNE RESERVED 33
// #defiNE RESERVED 34
// #defiNE RESERVED 35
// #defiNE RESERVED 36
// #defiNE RESERVED 37
// #defiNE RESERVED 38
// #defiNE RESERVED 39
// #defiNE RESERVED 40
// #defiNE RESERVED 41
// #defiNE RESERVED 42
// #defiNE RESERVED 43
// #defiNE RESERVED 44
// #defiNE RESERVED 45
// #defiNE RESERVED 46
// #defiNE RESERVED 47
// #defiNE RESERVED 48
// #defiNE RESERVED 49
// #defiNE RESERVED 50
// #defiNE RESERVED 51
// #defiNE RESERVED 52
// #defiNE RESERVED 53
// #defiNE RESERVED 54
#define ETH0_LPI_INTR 55
// #defiNE RESERVED 56
#define ETH0_INTR 57
#define SD0_WAKEUP_INTR 58
#define SD0_INTR 59
#define EMMC_WAKEUP_INTR 60
#define EMMC_INTR 61
#define GPIO3_INTR 62
#define DDR_PI_PHY_INTR 63
#define SF_INTR 64
#define SPI0_INTR 65
#define GPIO2_INTR 66
#define GPIO1_INTR 67
#define GPIO0_INTR 68
#define WDT_INTR 69
#define IC4_INTR 70
#define IC3_INTR 71
#define IC2_INTR 72
#define IC1_INTR 73
#define IC0_INTR 74
#define JPEG_INTR 75
#define H264C_INTR 76
#define H265C_INTR 77
#define KEYSCAN_IRQ 78
#define TPU0_INTR 79
#define TDMA0_INTR 80
// #defiNE RESERVED 81
// #defiNE RESERVED 82
#define USB_IRQS_0 83
// #defiNE RESERVED 84
// #defiNE RESERVED 85
// #defiNE RESERVED 86
// #defiNE RESERVED 87
// #defiNE RESERVED 88
// #defiNE RESERVED 89
// #defiNE RESERVED 90
// #defiNE RESERVED 91
// #defiNE RESERVED 92
#define UART_INTR_UART4 93
#define UART_DMA_TX_REQ_UART4 94
#define UART_DMA_RX_REQ_UART4 95
#define NPMUIRQ_0 96
#define NPMUIRQ_1 97
#define CTIIRQ_0 98
#define CTIIRQ_1 99
#define NEXTERRIRQ 100
// #defiNE RESERVED 101
// #defiNE RESERVED 102
// #defiNE RESERVED 103
// #defiNE RESERVED 104
// #defiNE RESERVED 105
// #defiNE RESERVED 106
// #defiNE RESERVED 107
#define GP_REG15_0 108
#define GP_REG15_1 109
#define GP_REG15_2 110
#define GP_REG15_3 111
#define GP_REG15_4 112
#define GP_REG15_5 113
#define GP_REG15_6 114
#define GP_REG15_7 115
#define GP_REG15_8 116
#define GP_REG15_9 117
#define GP_REG15_10 118
#define GP_REG15_11 119
#define GP_REG15_12 120
#define GP_REG15_13 121
#define GP_REG15_14 122
#define GP_REG15_15 123
#define SPI1_INTR 124
#define SPI2_INTR 125
#define SPI3_INTR 126
#define I2S2_TX_EMP_INTR 127
#define I2S2_TX_OR_INTR 128
#define I2S2_RX_DA_INTR 129
#define I2S2_RX_OR_INTR 130
#define I2S3_TX_EMP_INTR 131
#define I2S3_TX_OR_INTR 132
#define I2S3_RX_DA_INTR 133
#define I2S3_RX_OR_INTR 134
#define WGN0_IRQ 135
#define WGN1_IRQ 136
#define WGN2_IRQ 137
#define VIP_INT_ISP_TOP 138
#define VIP_INT_SC_TOP 139
#define IRQ_RTC2AP_0 140
#define IRQ_RTC2AP_1 141
#define IRQ_RTC2AP_2 142
#define IRQ_RTC2AP_3 143
#define IRQ_RTC2AP_4 144
#define IRQ_RTC2AP_5 145
#define IRQ_RTC2AP_6 146
#define IRQ_RTC2AP_7 147
// #defiNE RESERVED 148
// #defiNE RESERVED 149
// #defiNE RESERVED 150
// #defiNE RESERVED 151
// #defiNE RESERVED 152
// #defiNE RESERVED 153
// #defiNE RESERVED 154
#define VIP_INT_CSI_MAC0 155
#define VIP_INT_CSI_MAC1 156
#define VIP_INT_DWA_WRAP 157
// #defiNE RESERVED 158
#define PERI_FIREWALL_IRQ 159
#define HSPERI_FIREWALL_IRQ 160
#define SPACC_IRQ 161
#define TRNG_IRQ 162
// #defiNE RESERVED 163
#define TEMPSEN_IRQ_O 164
#define INTR_SARADC 165
#define EPHY_SMI_INT_N 166
#define EPHY_SMI_MGP_INT_N 167
#define EPHY_SMI_EXMGP_INT_N 168
#define DDR_FW_INTR 169
#define ROM_FIREWALL_IRQ 170
// #defiNE RESERVED 171
#define DDR_AXI_MON_INTR 172
// #defiNE RESERVED 173
#define VBAT_DEB_IRQ_O 174
// #defiNE RESERVED 175
// #defiNE RESERVED 176
// #defiNE RESERVED 177
// #defiNE RESERVED 178
// #defiNE RESERVED 179
#define TIMER_INTR_FLAG 180
#define SPI_NAND_INTR 181
#define DBGSYS_APBUSMON_HANG_INT 182
#define TIMER_INTR_0 183
#define TIMER_INTR_1 184
#define TIMER_INTR_2 185
#define TIMER_INTR_3 186
#define TIMER_INTR_4 187
#define TIMER_INTR_5 188
#define TIMER_INTR_6 189
#define TIMER_INTR_7 190

#define IRQF_TRIGGER_NONE 0x00000000
#define IRQF_TRIGGER_RISING 0x00000001
#define IRQF_TRIGGER_FALLING 0x00000002
#define IRQF_TRIGGER_HIGH 0x00000004
#define IRQF_TRIGGER_LOW 0x00000008
#define IRQF_TRIGGER_MASK                                                      \
	(IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | IRQF_TRIGGER_RISING |          \
	 IRQF_TRIGGER_FALLING)

// IRQ API
typedef int (*irq_handler_t)(int irqn, void *priv);

void disable_irq(unsigned int irqn);
void enable_irq(unsigned int irqn);

void cpu_enable_irqs(void);
void cpu_disable_irqs(void);

// PINMUX API
void pinmux_config(int io_type);

// RESET API
void cv_reset_assert(uint32_t id);
void cv_reset_deassert(uint32_t id);

#define NUM_IRQ (256)

#define SPI_SECTOR_SIZE 256
#define SPI_DMMR_TEST_SIZE 1024

#endif
