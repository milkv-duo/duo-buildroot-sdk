#ifndef _MCU_PARAM_ST7789V_H_
#define _MCU_PARAM_ST7789V_H_

#include <linux/cvi_comm_vo.h>

#define COMMAND 0
#define DATA	1

const struct VO_PINMUX st7789v3_pins_cfg = {
	.pin_num = 11,
	.d_pins = {
		{VO_MIPI_TXM4, VO_MUX_MCU_DATA0},
		{VO_MIPI_TXP4, VO_MUX_MCU_DATA1},
		{VO_MIPI_TXM3, VO_MUX_MCU_DATA2},
		{VO_MIPI_TXP3, VO_MUX_MCU_DATA3},
		{VO_MIPI_TXM2, VO_MUX_MCU_DATA4},
		{VO_MIPI_TXP2, VO_MUX_MCU_DATA5},
		{VO_MIPI_TXM1, VO_MUX_MCU_DATA6},
		{VO_MIPI_TXP1, VO_MUX_MCU_DATA7},
		{VO_MIPI_TXM0, VO_MUX_MCU_RD},
		{VO_MIPI_TXP0, VO_MUX_MCU_WR},
		{VO_VIVO_D10, VO_MUX_MCU_RS},
	}
};

const struct VO_MCU_INSTRS st7789v3_instrs = {
	.instr_num = 72,
	.instr_cmd = {
		{.delay = 0,   .data_type = COMMAND, .data = 0x11},
		{.delay = 0,   .data_type = COMMAND, .data = 0x35},
		{.delay = 0,   .data_type = DATA,    .data = 0x00},
		{.delay = 0,   .data_type = COMMAND, .data = 0x36},//Display Setting
		{.delay = 0,   .data_type = DATA,    .data = (1<<6)/*(1<<5)|(1<<6)*/},
		{.delay = 0,   .data_type = COMMAND, .data = 0x3A},
		{.delay = 0,   .data_type = DATA,    .data = 0x05},
		{.delay = 0,   .data_type = COMMAND, .data = 0xB2},
		{.delay = 0,   .data_type = DATA,    .data = 0x0C},
		{.delay = 0,   .data_type = DATA,    .data = 0x0C},
		{.delay = 0,   .data_type = DATA,    .data = 0x00},
		{.delay = 0,   .data_type = DATA,    .data = 0x33},
		{.delay = 0,   .data_type = DATA,    .data = 0x33},
		{.delay = 0,   .data_type = COMMAND, .data = 0xB7},
		{.delay = 0,   .data_type = DATA,    .data = 0x75},
		{.delay = 0,   .data_type = COMMAND, .data = 0xBB},
		{.delay = 0,   .data_type = DATA,    .data = 0x19},
		{.delay = 0,   .data_type = COMMAND, .data = 0xC0},
		{.delay = 0,   .data_type = DATA,    .data = 0x2C},
		{.delay = 0,   .data_type = COMMAND, .data = 0xC2},
		{.delay = 0,   .data_type = DATA,    .data = 0x01},
		{.delay = 0,   .data_type = COMMAND, .data = 0xC3},
		{.delay = 0,   .data_type = DATA,    .data = 0x0C},
		{.delay = 0,   .data_type = COMMAND, .data = 0xC4},
		{.delay = 0,   .data_type = DATA,    .data = 0x20},
		{.delay = 0,   .data_type = COMMAND, .data = 0xC6},
		{.delay = 0,   .data_type = DATA,    .data = 0x0F},
		{.delay = 0,   .data_type = COMMAND, .data = 0xD0},
		{.delay = 0,   .data_type = DATA,    .data = 0xA4},
		{.delay = 0,   .data_type = DATA,    .data = 0xA1},
		{.delay = 0,   .data_type = COMMAND, .data = 0xE0},//Gamma setting
		{.delay = 0,   .data_type = DATA,    .data = 0xD0},
		{.delay = 0,   .data_type = DATA,    .data = 0x04},
		{.delay = 0,   .data_type = DATA,    .data = 0x0C},
		{.delay = 0,   .data_type = DATA,    .data = 0x0E},
		{.delay = 0,   .data_type = DATA,    .data = 0x0E},
		{.delay = 0,   .data_type = DATA,    .data = 0x29},
		{.delay = 0,   .data_type = DATA,    .data = 0x37},
		{.delay = 0,   .data_type = DATA,    .data = 0x44},
		{.delay = 0,   .data_type = DATA,    .data = 0x47},
		{.delay = 0,   .data_type = DATA,    .data = 0x0B},
		{.delay = 0,   .data_type = DATA,    .data = 0x17},
		{.delay = 0,   .data_type = DATA,    .data = 0x16},
		{.delay = 0,   .data_type = DATA,    .data = 0x1B},
		{.delay = 0,   .data_type = DATA,    .data = 0x1F},
		{.delay = 0,   .data_type = COMMAND, .data = 0xE1},
		{.delay = 0,   .data_type = DATA,    .data = 0xD0},
		{.delay = 0,   .data_type = DATA,    .data = 0x04},
		{.delay = 0,   .data_type = DATA,    .data = 0x0C},
		{.delay = 0,   .data_type = DATA,    .data = 0x0E},
		{.delay = 0,   .data_type = DATA,    .data = 0x0F},
		{.delay = 0,   .data_type = DATA,    .data = 0x29},
		{.delay = 0,   .data_type = DATA,    .data = 0x37},
		{.delay = 0,   .data_type = DATA,    .data = 0x44},
		{.delay = 0,   .data_type = DATA,    .data = 0x4A},
		{.delay = 0,   .data_type = DATA,    .data = 0x0C},
		{.delay = 0,   .data_type = DATA,    .data = 0x17},
		{.delay = 0,   .data_type = DATA,    .data = 0x16},
		{.delay = 0,   .data_type = DATA,    .data = 0x1B},
		{.delay = 0,   .data_type = DATA,    .data = 0x1F},
		{.delay = 0,   .data_type = COMMAND, .data = 0x29},

		{.delay = 0,   .data_type = COMMAND, .data = 0x2A},
		{.delay = 0,   .data_type = DATA,    .data = 0x0 },//Xstart
		{.delay = 0,   .data_type = DATA,    .data = 0x0 },
		{.delay = 0,   .data_type = DATA,    .data = 0x0 },//Xend
		{.delay = 0,   .data_type = DATA,    .data = 0xEF},
		{.delay = 0,   .data_type = COMMAND, .data = 0x2B},
		{.delay = 0,   .data_type = DATA,    .data = 0x0 },//Ystart
		{.delay = 0,   .data_type = DATA,    .data = 0x0 },
		{.delay = 0,   .data_type = DATA,    .data = 0x01},//Yend
		{.delay = 0,   .data_type = DATA,    .data = 0x3F},
		{.delay = 0,   .data_type = COMMAND, .data = 0x2C},
	}
};

const VO_HW_MCU_CFG_S st7789v3Cfg = {
	.pins = st7789v3_pins_cfg,
	.mode = VO_MCU_MODE_RGB565,
	.lcd_power_gpio_num = GPIOB_03,
	.lcd_power_avtive = GPIO_ACTIVE_HIGH,
	.backlight_gpio_num = GPIOA_30,
	.backlight_avtive = GPIO_ACTIVE_HIGH,
	.reset_gpio_num = GPIOE_13,
	.reset_avtive = GPIO_ACTIVE_LOW,
	.instrs = st7789v3_instrs,
};

#endif // _MCU_PARAM_ST7789V_H_
