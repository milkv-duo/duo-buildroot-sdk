#ifndef __CV1835_REG_H
#define __CV1835_REG_H

#define TOP_BASE        0x03000000
#define PINMUX_BASE     (TOP_BASE + 0x1000)
#define WATCHDOG_BASE	(TOP_BASE + 0x00010000)

/*
 * Pinmux definitions
 */
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
#define PINMUX_RMII1    38
#define PINMUX_EPHY_LED 39
#define PINMUX_I80      40
#define PINMUX_LVDS     41

/* rst */
#define REG_TOP_SOFT_RST        0x3000
#define BIT_TOP_SOFT_RST_USB    BIT(11)
#define BIT_TOP_SOFT_RST_SDIO   BIT(14)
#define BIT_TOP_SOFT_RST_NAND   BIT(12)

#define REG_TOP_USB_CTRSTS	(TOP_BASE + 0x38)

#define REG_TOP_CONF_INFO		(TOP_BASE + 0x4)
#define BIT_TOP_CONF_INFO_VBUS		BIT(9)
#define REG_TOP_USB_PHY_CTRL		(TOP_BASE + 0x48)
#define BIT_TOP_USB_PHY_CTRL_EXTVBUS	BIT(0)
#define REG_TOP_DDR_ADDR_MODE		(TOP_BASE + 0x64)

/* irq */
#define IRQ_LEVEL   0
#define IRQ_EDGE    3

/* usb */
#define USB_BASE            0x040C0000
#define USB_HOST_BASE       0x040D0000
#define USB_DEV_BASE        0x040E0000

/* ethernet phy */
#define ETH_PHY_BASE        0x03009000
#define ETH_PHY_INIT_MASK   0xFFFFFFF9
#define ETH_PHY_SHUTDOWN    BIT(1)
#define ETH_PHY_POWERUP     0xFFFFFFFD
#define ETH_PHY_RESET       0xFFFFFFFB
#define ETH_PHY_RESET_N     BIT(2)
#define ETH_PHY_LED_LOW_ACTIVE  BIT(3)

/* watchdog */
#define WDT_BASE	0x03010000
#define DW_WDT_CR	0x00
#define DW_WDT_TORR	0x04
#define DW_WDT_CRR	0x0C

#define DW_WDT_CR_EN_OFFSET	0x00
#define DW_WDT_CR_RMOD_OFFSET	0x01
#define DW_WDT_CR_RMOD_VAL	0x00
#define DW_WDT_CRR_RESTART_VAL	0x76

/* rtc */
#define RTC_BASE	0x03005000
#define RTC_DB_REQ_WARM_RST	0x60
#define RTC_EN_WARM_RST_REQ	0xcc

#define RTCFC_BASE	0x03004000
#define RTC_CTRL0_UNLOCKKEY	0x4
#define RTC_CTRL0 0x8

/* rst */
#define CV183X_SOFT_RST_REG0 0x03003000
#define CV183X_SOFT_RST_REG1 0x03003004

/* gp_reg */
#define GP_REG2 0x03000088
#define GP_REG3 0x0300008C
#define GP_REG4 0x03000090
#define GP_REG5 0x03000094
#define GP_REG8 0x030000A0

#endif /* __CV1835_REG_H */
