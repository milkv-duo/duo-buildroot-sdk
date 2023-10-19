#ifndef __CV180X_REG_H
#define __CV180X_REG_H

#define SEC_BASE                                0x02000000
#define TOP_BASE                                0x03000000
// #define PINMUX_BASE                             (TOP_BASE + 0x00001000)
#define CLKGEN_BASE                             (TOP_BASE + 0x00002000)
#define WDT_BASE                                (TOP_BASE + 0x00011000)
#define TEMPSEN_BASE                            (TOP_BASE + 0x000E0000)

#ifdef RISCV_QEMU
#define UART0_BASE                              0x10000000
#else
#define UART0_BASE                              0x04140000
#endif
#define UART1_BASE                              0x04150000
#define UART2_BASE                              0x04160000
#define UART3_BASE                              0x04170000
#define SRAM_BASE                               0x0E000000

#define MAILBOX_REG_BASE                        0x01900000
#define MAILBOX_REG_BUFF                        (MAILBOX_REG_BASE + 0x0400)
#define SPINLOCK_REG_BASE                       (MAILBOX_REG_BASE + 0x00c0)

#define I2C0                    0x0
#define I2C1                    0x1
#define I2C2                    0x2
#define I2C3                    0x3
#define I2C4                    0x4

/* I2C */
#define I2C0_BASE				0x4000000
#define I2C1_BASE				0x4010000
#define I2C2_BASE				0x4020000
#define I2C3_BASE				0x4030000
#define I2C4_BASE				0x4040000

/* Clock */
#define REG_CLK_ENABLE_REG0                     (CLKGEN_BASE)
#define REG_CLK_ENABLE_REG1                     (CLKGEN_BASE + 0x4)
#define REG_CLK_ENABLE_REG2                     (CLKGEN_BASE + 0x8)
#define REG_CLK_BYPASS_SEL_REG                  (CLKGEN_BASE + 0x30)
#define REG_CLK_BYPASS_SEL_REG2                 (CLKGEN_BASE + 0x34)
#define REG_CLK_DIV0_CTL_CA53_REG               (CLKGEN_BASE + 0x40)
#define REG_CLK_DIV0_CTL_CPU_AXI0_REG           (CLKGEN_BASE + 0x48)
#define REG_CLK_DIV0_CTL_TPU_AXI_REG            (CLKGEN_BASE + 0x54)
#define REG_CLK_DIV0_CTL_TPU_FAB_REG            (CLKGEN_BASE + 0x5C)

/* USB */
#define TOP_USB_PHY_CTRSTS_REG                  (TOP_BASE + 0x48)
#define UPCR_EXTERNAL_VBUS_VALID_OFFSET         0

/* DRAM */
#define TOP_DDR_ADDR_MODE_REG                   (TOP_BASE + 0x64)
#define DAMR_REG_USB_REMAP_ADDR_39_32_OFFSET    16
#define DAMR_REG_USB_REMAP_ADDR_39_32_MSK       (0xff)

#define DAMR_REG_VD_REMAP_ADDR_39_32_OFFSET     24
#define DAMR_REG_VD_REMAP_ADDR_39_32_MSK        (0xff)

#define SW_RESET  (TOP_BASE + 0x3000)
#define JPEG_RESET   4

#define JPU_BASE				0x0B000000

/* rst */
#define REG_TOP_SOFT_RST                        0x3000

/* addr remap */
#define REG_TOP_ADDR_REMAP                      0x0064
#define ADDR_REMAP_USB(a)                       ((a & 0xFF) << 16)

/* ethernet phy */

/* watchdog */

#endif /* __CV180X_REG_H */
