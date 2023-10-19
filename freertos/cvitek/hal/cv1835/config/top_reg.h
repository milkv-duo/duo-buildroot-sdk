#ifndef __CV1835_REG_H
#define __CV1835_REG_H

#define SEC_BASE                                0x02000000
#define TOP_BASE                                0x03000000
#define PINMUX_BASE                             (TOP_BASE + 0x00001000)
#define CLKGEN_BASE                             (TOP_BASE + 0x00002000)
#define WATCHDOG_BASE                           (TOP_BASE + 0x00010000)
#define TEMPSEN_BASE                            (TOP_BASE + 0x000E0000)

#define UART0_BASE                              0x04140000
#define UART1_BASE                              0x04150000
#define UART2_BASE                              0x04160000
#define UART3_BASE                              0x04170000
#define SRAM_BASE                               0x0E000000

#define I2C0                    0x0
#define I2C1                    0x1
#define I2C2                    0x2
#define I2C3                    0x3
#define I2C4                    0x4

#define I2C0_BASE				0x4000000
#define I2C1_BASE				0x4010000
#define I2C2_BASE				0x4020000
#define I2C3_BASE				0x4030000
#define I2C4_BASE				0x4040000

#define REG_CLK_ENABLE_REG0                     (CLKGEN_BASE)
#define REG_CLK_ENABLE_REG1                     (CLKGEN_BASE + 0x4)
#define REG_CLK_BYPASS_SEL_REG                  (CLKGEN_BASE + 0x30)
#define REG_CLK_BYPASS_SEL_REG2                 (CLKGEN_BASE + 0x34)
#define REG_CLK_DIV0_CTL_CA53_REG               (CLKGEN_BASE + 0x40)
#define REG_CLK_DIV0_CTL_CPU_AXI0_REG           (CLKGEN_BASE + 0x48)
#define REG_CLK_DIV0_CTL_TPU_AXI_REG            (CLKGEN_BASE + 0x54)
#define REG_CLK_DIV0_CTL_TPU_FAB_REG            (CLKGEN_BASE + 0x5C)

#define TOP_USB_PHY_CTRSTS_REG                  (TOP_BASE + 0x48)
#define UPCR_EXTERNAL_VBUS_VALID_OFFSET         0

#define TOP_DDR_ADDR_MODE_REG                   (TOP_BASE + 0x64)
#define DAMR_REG_USB_REMAP_ADDR_39_32_OFFSET    16
#define DAMR_REG_USB_REMAP_ADDR_39_32_MSK       (0xff)

#define DAMR_REG_VD_REMAP_ADDR_39_32_OFFSET     24
#define DAMR_REG_VD_REMAP_ADDR_39_32_MSK        (0xff)

#define TOP_USB_CTRSTS_REG                      (TOP_BASE + 0x38)
#define UCR_MODE_STRAP_OFFSET                   0x0
#define UCR_MODE_STRAP_NON                      0x0
#define UCR_MODE_STRAP_HOST                     0x2
#define UCR_MODE_STRAP_DEVICE                   0x4
#define UCR_MODE_STRAP_MSK                      (0x7)
#define UCR_PORT_OVER_CURRENT_ACTIVE_OFFSET     10
#define UCR_PORT_OVER_CURRENT_ACTIVE_MSK        1

/* rst */
#define REG_TOP_SOFT_RST                        0x3000

#define BIT_TOP_SOFT_RST_AP                     BIT(1)
#define BIT_TOP_SOFT_RST_JPEG                   BIT(3)
#define BIT_TOP_SOFT_RST_USB                    BIT(11)
#define BIT_TOP_SOFT_RST_SDIO                   BIT(14)
#define BIT_TOP_SOFT_RST_NAND                   BIT(12)

#define REG_TOP_CONF_INFO                       (TOP_BASE + 0x4)
#define BIT_TOP_CONF_INFO_VBUS                  BIT(9)
#define REG_TOP_USB_CTRSTS                      (TOP_BASE + 0x38)
#define REG_TOP_USB_PHY_CTRL                    (TOP_BASE + 0x48)
#define BIT_TOP_USB_PHY_CTRL_EXTVBUS            BIT(0)
#define REG_TOP_DDR_ADDR_MODE                   (TOP_BASE + 0x64)

/* addr remap */
#define REG_TOP_ADDR_REMAP                      0x0064
#define ADDR_REMAP_USB(a)                       ((a & 0xFF) << 16)

/* usb */
#define USB_BASE                                0x040C0000
#define USB_HOST_BASE                           0x040D0000
#define USB_DEV_BASE                            0x040E0000

/* ethernet phy */
#define ETH_PHY_BASE                            0x03009000
#define ETH_PHY_INIT_MASK                       0xFFFFFFF9
#define ETH_PHY_SHUTDOWN                        BIT(1)
#define ETH_PHY_POWERUP                         0xFFFFFFFD
#define ETH_PHY_RESET                           0xFFFFFFFB
#define ETH_PHY_RESET_N                         BIT(2)
#define ETH_PHY_LED_LOW_ACTIVE                  BIT(3)

/* watchdog */
#define DW_WDT_CR                               0x00
#define DW_WDT_TORR                             0x04
#define DW_WDT_CRR                              0x0C

#define DW_WDT_CR_EN_OFFSET                     0x00
#define DW_WDT_CR_RMOD_OFFSET                   0x01
#define DW_WDT_CR_RMOD_VAL                      0x00
#define DW_WDT_CRR_RESTART_VAL                  0x76

#endif /* __CV1835_REG_H */
